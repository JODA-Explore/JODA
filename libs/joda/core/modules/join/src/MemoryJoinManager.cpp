//
// Created by Nico Schäfer on 02/04/18.
//

#include <glog/logging.h>
#include <joda/config/config.h>
#include <joda/document/TemporaryOrigin.h>
#include <joda/join/MemoryJoinManager.h>
#include <joda/query/values/IValueProvider.h>

MemoryJoinManager::MemoryJoinManager(
    const std::string& name,
    std::unique_ptr<joda::query::IValueProvider>&& valProv)
    : JoinManager(name, std::move(valProv)) {}

MemoryJoinManager::~MemoryJoinManager() = default;

void MemoryJoinManager::join(const RapidJsonDocument& doc) {
  // Initialize Pointer
  RJMemoryPoolAlloc alloc;
  const RJValue* valPtr = nullptr;
  RJValue val;
  if (valProv->isAtom()) {
    val = valProv->getAtomValue(doc, alloc);
    valPtr = &val;
  } else {
    valPtr = valProv->getValue(doc, alloc);
  }
  if (valPtr == nullptr) {
    return;
  }

  // Get id
  auto id = getJoinVal(*valPtr);
  if (id == NOT_HASHABLE) {
    return;
  }

  // Write to file
  mut.lock();
  auto* d = docs[id].get();
  mut.unlock();
  DCHECK(d != nullptr) << "Has to be created beforehand. ID is " << id;
  d->join(*(doc.getJson()));
}

const std::string& MemoryJoinManager::getName() const {
  return JoinManager::getName();
}

JODA_JOINID MemoryJoinManager::getJoinVal(const RJValue& val) {
  if (val.IsBool()) {
    if (val.GetBool()) {
      if (!trueValCreated) {
        std::lock_guard<std::mutex> guard(mut);
        docs[TRUE_VAL] = std::make_unique<MemoryJoinDoc>(true);
        trueValCreated = true;
      }
      return TRUE_VAL;
    }
    if (!falseValCreated) {
      std::lock_guard<std::mutex> guard(mut);
      docs[FALSE_VAL] = std::make_unique<MemoryJoinDoc>(false);
      falseValCreated = true;
    }
    return FALSE_VAL;
  }

  /*
//Check for doubleNumber
if(val.IsDouble()){
  auto d = val.GetString();
  //Check if already hashed
  auto fIt = dMap.find(d);
  if(fIt != dMap.end()){
    return fIt->second;
  }else{
    auto id = ++currID;
    dMap[d] = id;
    return id;
  }
}
 */

  // Check for integer number
  if (val.IsInt64()) {
    auto d = val.GetInt64();
    // Check if already hashed
    std::lock_guard<std::mutex> guard(mut);
    auto fIt = lMap.find(d);
    if (fIt != lMap.end()) {
      return fIt->second;
    }
    auto id = ++currID;
    lMap[d] = id;
    docs[id] = std::make_unique<MemoryJoinDoc>(d);
    return id;
  }

  // Check for String
  if (val.IsString()) {
    auto s = val.GetString();
    const auto str = std::string(s);
    std::lock_guard<std::mutex> guard(mut);
    // Check if already hashed
    auto fIt = sMap.find(str);
    if (fIt != sMap.end()) {
      return fIt->second;
    }
    auto id = ++currID;
    sMap[str] = id;
    docs[id] = std::make_unique<MemoryJoinDoc>(str);
    return id;
  }

  return NOT_HASHABLE;
}

void MemoryJoinManager::loadJoin(std::shared_ptr<JSONStorage>& load) {
  std::lock_guard<std::mutex> guard(mut);
  size_t size = 0;
  for (auto&& doc : docs) {
    size += doc.second->getMemSize();
  }
  auto cont =
      std::make_unique<JSONContainer>(size / config::storageRetrievalThreads);
  for (auto&& doc : docs) {
    auto tmpdoc = std::make_unique<RJDocument>(cont->getAlloc());
    doc.second->deepCopyInto(*tmpdoc);
    if (!cont->hasSpace(doc.second->getMemSize()) && cont->size() > 0) {
      cont->finalize();
      load->insertDocuments(std::move(cont));
      cont = std::make_unique<JSONContainer>(size /
                                             config::storageRetrievalThreads);
    }
    cont->insertDoc(std::move(tmpdoc), std::make_unique<TemporaryOrigin>());
  }
  cont->finalize();
  load->insertDocuments(std::move(cont));
}
