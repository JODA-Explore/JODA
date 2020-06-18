//
// Created by Nico on 26/02/2019.
//

#include "QueryThread.h"

#include <joda/config/config.h>
#include <joda/document/TemporaryOrigin.h>
#include <joda/misc/RecurringTimer.h>
#include <joda/query/predicate/BloomAttributeVisitor.h>
#include <joda/storage/JSONStorage.h>

QueryThread::QueryThread(IQueue *iqueue, OQueue *oqueue,
                         QueryThreadConfig &conf)
    : IWorkerThread(iqueue, oqueue, conf) {
  oqueue->registerProducer();
  if (this->conf.aggQueue != nullptr) this->conf.aggQueue->registerProducer();
  DLOG(INFO) << "Started QueryThread " << getThreadID();
}

QueryThread::~QueryThread() {
  oqueue->unregisterProducer();
  if (this->conf.aggQueue != nullptr) this->conf.aggQueue->unregisterProducer();
  DLOG(INFO) << "Stopped QueryThread" << getThreadID();
}

void QueryThread::work() {
  DCHECK(conf.q != nullptr);
  std::string tmpdir = conf.tmpdir;
  std::string tmpFileName = getThreadID() + ".json";
  std::string tmpFile;
  unsigned long tmpFileID = 0;
  if (!tmpdir.empty()) {
    tmpFile = tmpdir + "/" + tmpFileName;
    tmpFileID = g_FileNameRepoInstance.addFile(tmpFile);
  }

  /*
   * Timers
   */

  std::vector<std::string> bloomAttr;
  bloom_timer.start();
  if (config::bloom_enabled) {
    joda::query::BloomAttributeVisitor bloomVisitor;
    conf.q->getPredicate()->accept(bloomVisitor);
    if (bloomVisitor.isValid()) {
      bloomAttr = bloomVisitor.getAttributes();
    }
  }
  bloom_timer.stop();

  auto tok = IQueue::ctok_t(iqueue->queue);
  std::unique_ptr<OQueue::ptok_t> ptok = nullptr;
  if (oqueue != nullptr) {
    ptok = std::make_unique<OQueue::ptok_t>(oqueue->queue);
  }

  while (shouldRun) {
    if (!iqueue->isFinished()) {
      IPayload inCont = nullptr;
      iqueue->retrieve(inCont);
      if (inCont == nullptr) continue;

      /*
       * -------------------------  Query Exec
       */

      /*
       * Bloom Filter check
       */
      bloom_timer.start();
      if (config::bloom_enabled) {
        bool use = true;
        for (auto &&bloom : bloomAttr) {
          use &= inCont->probContainsAttr(bloom);
        }
        if (!use) {
          if (!config::storeJson) inCont->removeDocuments();
          LOG(INFO) << "Skipped container because of bloom";
          continue;
        }  // If needed attribute is not in container, skip
      }
      bloom_timer.stop();

      /*
       * Select
       */
      LOG(INFO) << "Starting selection";
      select_timer.start();
      auto selectResult = select(*inCont);
      select_timer.stop();
      size_t selCount = 0;
      bool skip = false;
      if (selectResult == nullptr || selectResult->empty()) {
        skip = true;
      } else {
        selCount = std::count(selectResult->begin(), selectResult->end(), true);
        skip = selCount == 0;
      }
      if (skip) {
        if (!config::storeJson) inCont->removeDocuments();
        continue;
      }

      /*
       * Project
       */
      double contFill = ((double)selCount) / inCont->size();
      OPayload tmpCont =
          std::make_unique<JSONContainer>(inCont->getMaxSize() * contFill);

      LOG(INFO) << "Starting Projection";
      project_timer.start();
      auto projectResult =
          project(*inCont, *selectResult, *tmpCont->getAlloc());
      project_timer.stop();

      copy_timer.start();

      if (ptok != nullptr || hasAggregators() || conf.joinManager != nullptr) {
        for (auto &&item : projectResult) {
          if (item->IsNull()) continue;
          tmpCont->insertDoc(JSONStorage::getID(), std::move(item),
                             std::make_unique<TemporaryOrigin>());
        }
        tmpCont->finalize();
      }
      copy_timer.stop();

      if (tmpCont->size() == 0) {
        DLOG(INFO) << "Empty container, skipping";
        continue;
      }
      /*
       * Aggregate
       */

      if (hasAggregators()) {
        aggregate_timer.start();
        aggregate(tmpCont->getDocuments());
        aggregate_timer.stop();
        if (!config::storeJson) inCont->removeDocuments();
        continue;
      }

      /*
       * Store result
       */
      CHECK(oqueue != nullptr) << "New design should require oqueue";

      if (oqueue != nullptr) {
        LOG(INFO) << "Storing result";

        /*if(!conf.storedir.empty()){
          LOG(INFO) << "Writing container to "<<conf.storedir+"/"+tmpFileName;
          tmpCont->writeFile(conf.storedir+"/"+tmpFileName,true);
        }*/
        if (!config::storeJson) {
          serialize_timer.start();
          tmpCont->removeDocuments();
          serialize_timer.stop();
        }
        oqueue->send(*ptok, std::move(tmpCont));

      } /*else if(conf.joinManager != nullptr){
        LOG(INFO) << "Joining container";
        join_timer.start();
        conf.joinManager->join(*tmpCont);
        join_timer.stop();
      }
      */

      LOG(INFO) << "Finished query";
      if (!config::storeJson)
        inCont->removeDocuments();  // Remove documents if not needed anymore

    } else {
      shouldRun = false;
    }
  }
  oqueue->producerFinished();

  if (hasAggregators()) {
    DCHECK(conf.aggQueue != nullptr);
    auto aggPtok = std::make_unique<joda::query::AggregatorQueue::queue_t::ptok_t>(
        conf.aggQueue->queue);
    DCHECK(aggPtok != nullptr);
    for (auto &&aggregator : conf.aggregators) {
      conf.aggQueue->send(*aggPtok, std::move(aggregator));
    }
    conf.aggQueue->producerFinished();
  }
  logTimers();
}

const std::string QueryThread::getThreadID() const {
  std::stringstream ss;
  ss << "QT" << std::this_thread::get_id();
  return ss.str();
}

std::shared_ptr<const DocIndex> QueryThread::select(JSONContainer &cont) const {
  /*
   * Get Execution Method
   */
  IQueryExecutor *exec = nullptr;
  unsigned long estSize = NOT_APPLICABLE;
  // std::string debugStr = "Executors ";

  for (auto &&executor : conf.executors) {
    CHECK(executor != nullptr);
    auto s = executor->estimatedWork(*(conf.q), cont);
    // debugStr += "(" + executor->getName() + ", " + std::to_string(s) + ") ";
    if (s < estSize) {
      estSize = s;
      exec = executor.get();
    }
  }
  DCHECK(exec != nullptr);
  // debugStr += " => " + exec->getName();
  DLOG(INFO) << "Chose " << exec->getName() << " in " << getThreadID();
  /*
   * Execute Executor
   */
  auto selectResult = exec->execute(*(conf.q), cont);

  /*
   * After Select all Executors
   */
  for (auto &&executor : conf.executors) {
    executor->alwaysAfterSelect(*(conf.q), selectResult, cont);
  }

  return selectResult;
}

std::vector<std::shared_ptr<RJDocument>> QueryThread::project(
    JSONContainer &cont, const DocIndex &ids, RJMemoryPoolAlloc &alloc) const {
  std::vector<std::shared_ptr<RJDocument>> ret;

  auto &proj = (conf.q)->getProjectors();
  auto &setproj = (conf.q)->getSetProjectors();
  /*
   * Check for batch completion
   */
  if (proj.empty() && setproj.empty()) {  // Star Expression
    ret = {};
    auto docs = cont.getDocuments(ids);
    for (auto &doc : docs) {
      auto d = std::make_shared<RJDocument>(&alloc);
      d->CopyFrom(*doc.getJson(), d->GetAllocator());
      ret.push_back(std::move(d));
    }
    return ret;
  }

  /*
  if (setproj.empty() && proj.size() == 1) {
    CHECK(proj[0] != nullptr);
    if (proj[0]->getType() == IDProject::type) { //ID Expression
      auto ptr = RJPointer(proj[0]->getToPointer().c_str());
      for (auto &&id : ids) {
        auto doc = std::make_shared<RJDocument>(&alloc);
        doc->SetObject();
        RJValue v;
        v.SetInt64(id);
        ptr.Set(*doc, v);
        ret.push_back(std::move(doc));
      }
      return ret;
    }
  }
*/
  /*
   * Default execution
   */
  ret = cont.projectDocuments(ids, proj, alloc, setproj);
  return ret;
}

void QueryThread::aggregate(const std::vector<RapidJsonDocument> &docs) const {
  RJMemoryPoolAlloc alloc;
  for (auto &&doc : docs) {
    for (auto &j : conf.aggregators) {
      j->accumulate(doc, alloc);
    }
  }
}

bool QueryThread::hasAggregators() const { return !conf.aggregators.empty(); }

void QueryThread::logTimers() const {
  if (conf.bench == nullptr) return;

  conf.bench->addThread(
      bloom_timer.durationSeconds(), select_timer.durationSeconds(),
      project_timer.durationSeconds(), aggregate_timer.durationSeconds(),
      copy_timer.durationSeconds(), serialize_timer.durationSeconds());
}
