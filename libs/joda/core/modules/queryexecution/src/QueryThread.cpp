//
// Created by Nico on 26/02/2019.
//

#include "QueryThread.h"
#include <joda/config/config.h>
#include <joda/document/TemporaryOrigin.h>
#include <joda/misc/RecurringTimer.h>
#include <joda/query/predicate/BloomAttributeVisitor.h>
#include <joda/query/project/PointerCopyProject.h>
#include <joda/storage/JSONStorage.h>

QueryThread::QueryThread(IQueue* iqueue, OQueue* oqueue,
                         QueryThreadConfig& conf)
    : IWorkerThread(iqueue, oqueue, conf) {
  if (oqueue != nullptr) {
    oqueue->registerProducer();
  }
  if (this->conf.aggQueue != nullptr) {
    this->conf.aggQueue->registerProducer();
  }
  DLOG(INFO) << "Started QueryThread " << getThreadID();
}

QueryThread::~QueryThread() {
  if (oqueue != nullptr) {
    oqueue->registerProducer();
  }
  if (this->conf.aggQueue != nullptr) {
    this->conf.aggQueue->unregisterProducer();
  }
  DLOG(INFO) << "Stopped QueryThread" << getThreadID();
}

void QueryThread::work() {
  DCHECK(conf.q != nullptr);
  std::string tmpdir = conf.tmpdir;
  std::string tmpFileName = getThreadID() + ".json";
  std::string tmpFile;
  if (!tmpdir.empty()) {
    tmpFile = tmpdir + "/" + tmpFileName;
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

  auto chooseAttributes = (conf.q)->getChooseAttributes();
  auto asAttributes = (conf.q)->getASAttributes();
  // Remove first "" pointer, or all views will always be completely
  // materialized
  if (!asAttributes.empty() && asAttributes.front().empty()) {
    asAttributes.erase(asAttributes.begin());
  }
  auto aggAttributes = (conf.q)->getAGGAttributes();

  while (shouldRun) {
    if (!iqueue->isFinished()) {
      ContRef inCont = nullptr;
      ContRef pipelineCont;
      iqueue->retrieve(tok, inCont);
      if (inCont == nullptr) {
        continue;
      }
      pipelineCont = inCont;
      auto contLock = pipelineCont->useContInScope(false);

      /*
       * -------------------------  Query Exec
       */

      /*
       * Bloom Filter check
       */
      bloom_timer.start();
      if (config::bloom_enabled) {
        bool use = true;
        for (auto&& bloom : bloomAttr) {
          use &= pipelineCont->probContainsAttr(bloom);
        }
        if (!use) {
          if (!config::storeJson) {
            pipelineCont->removeDocuments();
          }
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
      if (pipelineCont->isView()) {
        pipelineCont->materializeAttributesIfRequired(chooseAttributes);
      }
      auto selectResult = select(*pipelineCont);
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
        if (!config::storeJson) {
          inCont->removeDocuments();
        }
        continue;
      }

      /*
       * Project
       */

      OwnedCont tmpCont;
      bool isSelected = false;
      if (hasToProject()) {
        double contFill =
            (static_cast<double>(selCount) /
             pipelineCont->size());
        LOG(INFO) << "Starting Projection";
        project_timer.start();
        auto& projectors = (conf.q)->getProjectors();
        if (pipelineCont->isView()) {
          pipelineCont->materializeAttributesIfRequired(asAttributes);
        }
        bool created = false;
        if (canCreateView()) {
          sample_view_cost_timer.start();
          auto useView = pipelineCont->useViewBasedOnSample(
              *selectResult, projectors, (conf.q)->getSetProjectors());
          sample_view_cost_timer.stop();
          if (useView) {
            LOG(INFO) << "Creating View";
            tmpCont = pipelineCont->createViewFromContainer(
                *selectResult, projectors, (conf.q)->getSetProjectors());
            created = true;
          }
        }
        if (!created) {
          tmpCont = std::make_unique<JSONContainer>(pipelineCont->getMaxSize() *
                                                    contFill);
          auto projectResult = defaultProject(*pipelineCont, *selectResult,
                                              *tmpCont->getAlloc());
          copy_timer.start();

          if (ptok != nullptr || hasAggregators() ||
              conf.joinManager != nullptr) {
            for (auto&& item : projectResult) {
              if (item->IsNull()) {
                continue;
              }
              tmpCont->insertDoc(std::move(item),
                                 std::make_unique<TemporaryOrigin>());
            }
          }
          copy_timer.stop();
        }

        project_timer.stop();
        pipelineCont = tmpCont.get();
        isSelected = true;
      }

      auto tmpContScope = pipelineCont->useContInScope(false);
      if (pipelineCont->size() == 0) {
        DLOG(INFO) << "Empty container, skipping";
        continue;
      }
      /*
       * Aggregate
       */

      if (hasAggregators()) {
        DCHECK(oqueue == nullptr);
        LOG(INFO) << "Starting aggregation";
        aggregate_timer.start();
        if (pipelineCont->isView()) {
          auto& atts = aggAttributes;
          pipelineCont->materializeAttributesIfRequired(atts);
        }
        aggregate(pipelineCont, selectResult, isSelected);

        aggregate_timer.stop();
        continue;
      }

      /*
       * Store result
       */

      if (oqueue != nullptr) {
        LOG(INFO) << "Storing result";
        tmpCont->finalize();
        oqueue->send(*ptok, std::move(tmpCont));
      }

      LOG(INFO) << "Finished query";

    } else {
      shouldRun = false;
    }
  }
  if (oqueue != nullptr) {
    oqueue->producerFinished();
  }

  if (hasAggregators()) {
    DCHECK(conf.aggQueue != nullptr);
    auto aggPtok =
        std::make_unique<joda::query::AggregatorQueue::queue_t::ptok_t>(
            conf.aggQueue->queue);
    DCHECK(aggPtok != nullptr);
    for (auto&& aggregator : conf.aggregators) {
      conf.aggQueue->send(*aggPtok, std::move(aggregator));
    }
    conf.aggQueue->producerFinished();
  }
  logTimers();
}

void QueryThread::aggregate(QueryThread::ContRef pipelineCont,
                            const std::shared_ptr<const DocIndex>& selectResult,
                            bool isSelected) const {
  RJMemoryPoolAlloc a;
  auto& aggs = conf.aggregators;
  if (!isSelected) {
    pipelineCont->forAll(
        [&a, &aggs](const RapidJsonDocument& d) {
          for (auto& j : aggs) {
            j->accumulate(d, a);
          }
        },
        *selectResult);
  } else {
    pipelineCont->forAll([&a, &aggs](const RapidJsonDocument& d) {
      for (auto& j : aggs) {
        j->accumulate(d, a);
      }
    });
  }
}

void QueryThread::aggregate(const std::vector<RapidJsonDocument>& docs) const {
  RJMemoryPoolAlloc alloc;
  for (auto&& doc : docs) {
    for (auto& j : conf.aggregators) {
      j->accumulate(doc, alloc);
    }
  }
}

const std::string QueryThread::getThreadID() const {
  std::stringstream ss;
  ss << "QT" << std::this_thread::get_id();
  return ss.str();
}

std::shared_ptr<const DocIndex> QueryThread::select(JSONContainer& cont) const {
  /*
   * Get Execution Method
   */
  IQueryExecutor* exec = nullptr;
  unsigned long estSize = NOT_APPLICABLE;
  // std::string debugStr = "Executors ";

  for (auto&& executor : conf.executors) {
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
  for (auto&& executor : conf.executors) {
    executor->alwaysAfterSelect(*(conf.q), selectResult, cont);
  }

  return selectResult;
}

std::vector<std::unique_ptr<RJDocument>> QueryThread::defaultProject(
    JSONContainer& cont, const DocIndex& ids, RJMemoryPoolAlloc& alloc) const {
  std::vector<std::unique_ptr<RJDocument>> ret;

  auto& proj = (conf.q)->getProjectors();
  auto& setproj = (conf.q)->getSetProjectors();
  /*
   * Check for batch completion
   */
  if (proj.empty() && setproj.empty()) {  // Star Expression
    return cont.getRaw(ids, alloc);
  }

  return cont.projectDocuments(ids, proj, alloc, setproj);
}

bool QueryThread::hasAggregators() const { return !conf.aggregators.empty(); }

bool QueryThread::canCreateView() const {
  return conf.q->canCreateView() && oqueue != nullptr;  // Does have to project
}

bool QueryThread::hasToProject() const {
  auto& projectors = conf.q->getProjectors();
  return oqueue != nullptr ||
         !((conf.q)->getSetProjectors().empty() &&
           (projectors.empty() ||
            (projectors.size() == 1 &&
             projectors.front()->getType() ==
                 joda::query::PointerCopyProject::allCopy)));
}

void QueryThread::logTimers() const {
  if (conf.bench == nullptr) {
    return;
  }

  conf.bench->addThread(
      bloom_timer.durationSeconds(), select_timer.durationSeconds(),
      project_timer.durationSeconds(), aggregate_timer.durationSeconds(),
      copy_timer.durationSeconds(), serialize_timer.durationSeconds(),
      sample_view_cost_timer.durationSeconds());
}
