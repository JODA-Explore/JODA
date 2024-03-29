//
// Created by Nico Schäfer on 9/7/17.
//

#include "../include/joda/container/JSONContainer.h"

#include <glog/logging.h>
#include <joda/container/JSONContainer.h>
#include <joda/document/FileOrigin.h>
#include <joda/document/TemporaryOrigin.h>
#include <joda/fs/DirectoryRegister.h>
#include <joda/misc/JSONFileWriter.h>
#include <joda/query/project/PointerCopyProject.h>
#include <rapidjson/writer.h>

#include <fstream>
#include <unordered_set>

#include "../../document/src/BloomAttributeHandler.h"
#include "../../document/src/DocumentCostHandler.h"
#include "joda/config/config.h"

JSONContainer::JSONContainer() : JSONContainer(config::JSONContainerSize) {}

JSONContainer::JSONContainer(size_t maxsize) : maxSize(maxsize) {
  contId = contIdCounter.fetch_add(1);
  if (config::JSONContainerSize != 0) {
    this->maxSize = config::JSONContainerSize;
  } else {
    if (maxSize == 0) {
      this->maxSize = JSON_CONTAINER_DEFAULT_SIZE;
    }
  }
  LOG(INFO) << "Created JSONContainer " << contId << " with maxSize " << maxSize;

  assert(maxSize > 0);
  alloc = std::make_unique<RJMemoryPoolAlloc>(
      maxSize * config::chunk_size);  // Container size * chunk-fraction

  cache = std::make_unique<QueryCache>();
  moduleStorage = std::make_unique<joda::extension::ModuleExecutorStorage>();
}

bool JSONContainer::hasSpace(size_t size) const {
  if (maxSize == 0) {
    return true;
  }
  return alloc->Size() + size < maxSize;
}

RJMemoryPoolAlloc* JSONContainer::getAlloc() { return alloc.get(); }

void JSONContainer::insertDoc(RapidJsonDocument&& doc, size_t baseIndex) {
  CHECK(!final) << "Container may not be final";

  // Update indices
  maxID = std::max(maxID, doc.getId());
  minID = std::min(minID, doc.getId());
  docs.emplace_back(std::move(doc));
  baseIds.emplace_back(baseIndex);
}

void JSONContainer::insertViewDoc(std::unique_ptr<RJDocument>&& doc,
                                  size_t baseIndex) {
  insertDoc({std::move(doc), std::make_unique<TemporaryOrigin>()}, baseIndex);
}

void JSONContainer::setDocuments(const DocIndex& docIndex,
                                 std::vector<RapidJsonDocument*>& loadedDocs,
                                 bool loadedCheck) {
  ScopedRef useCont(this);

  if (loadedCheck) {
    for (size_t i = 0; i < docIndex.size(); ++i) {
      if (docIndex[i] && loadedDocs[i] == nullptr && docs[i].isValid()) {
        loadedDocs[i] = &docs[i];
      }
    }
  } else {
    for (size_t i = 0; i < docIndex.size(); ++i) {
      if (docIndex[i] && docs[i].isValid()) {
        loadedDocs[i] = &docs[i];
      }
    }
  }
  setLastUsed();
}

void JSONContainer::setDocuments(const std::vector<size_t>& docIndexes,
                                 std::vector<RapidJsonDocument*>& loadedDocs,
                                 bool isRange, bool loadedCheck) {
  ScopedRef useCont(this);

  if (loadedCheck) {
    if (isRange) {
      for (int i = 0; i < docIndexes.size(); i += 2) {
        for (auto k = docIndexes[i]; k <= docIndexes[i + 1]; ++k) {
          if (loadedDocs[k] != nullptr) continue;

          loadedDocs[k] = &docs[k];
        }
      }
    } else {
      for (auto const& idx : docIndexes) {
        if (loadedDocs[idx] != nullptr) continue;
        loadedDocs[idx] = &docs[idx];
      }
    }
  } else {
    if (isRange) {
      for (int i = 0; i < docIndexes.size(); i += 2) {
        for (auto k = docIndexes[i]; k <= docIndexes[i + 1]; ++k) {
          loadedDocs[k] = &docs[k];
        }
      }
    } else {
      for (auto const& idx : docIndexes) {
        loadedDocs[idx] = &docs[idx];
      }
    }
  }
  setLastUsed();
}

unsigned long JSONContainer::size() const { return docs.size(); }

void JSONContainer::finalize() {
  assert(!final);
  final = true;
  docs.shrink_to_fit();
  calculateBloom();
  std::sort(docs.begin(), docs.end(), compareDocContainer);
  setLastUsed();
}

void JSONContainer::metaFinalize() {
  final = true;
  docs.shrink_to_fit();
  std::sort(docs.begin(), docs.end(), compareDocContainer);
  setLastUsed();
}

std::unique_ptr<const DocIndex> JSONContainer::checkDocuments(
    std::function<bool(RapidJsonDocument&, size_t)>& func) {
  ScopedRef useCont(this);
  DocIndex ret(docs.size());
  for (size_t i = 0; i < docs.size(); ++i) {
    auto& doc = docs[i];
    if (doc.isValid() && func(doc, i)) {
      ret[i] = true;
    }
  }
  setLastUsed();
  return std::make_unique<const DocIndex>(std::move(ret));
}

void JSONContainer::calculateBloom() {
  if (!config::bloom_enabled || bloomCalculated) {
    return;
  }
  // Bloom
  bloom_parameters parameters;
  parameters.projected_element_count = config::bloom_count;
  parameters.false_positive_probability = config::bloom_prob;
  parameters.compute_optimal_parameters();
  attr_bloom = bloom_filter(parameters);

  for (auto& doc : docs) {
    if (doc.isValid() && doc.getJson()->IsObject()) {
      BloomAttributeHandler h;
      if (!isView()) {
        doc.getJson()->Accept(h);
      } else {
        auto& view = doc.getView();
        view->Accept(h);
      }
      auto atts = h.getPaths();
      for (const auto& att : atts) {
        attr_bloom.insert(att);
      }
    }
  }
  bloomCalculated = true;
}

void JSONContainer::recursiveBloomAttrSearch(const RJValue& obj,
                                             const std::string& attr) {
  // Add attr to bloom
  if (!attr.empty()) {
    attr_bloom.insert(attr);
  }

  // Get members
  if (obj.IsObject()) {
    for (const auto& member : obj.GetObject()) {
      std::string tmp = attr;
      tmp += "/";
      tmp += member.name.GetString();
      recursiveBloomAttrSearch(member.value, tmp);
    }
  }
}

bool JSONContainer::probContainsAttr(const std::string& attr) const {
  DCHECK(final) << "JSONContainer has to be final";
  return !config::bloom_enabled || !bloomCalculated ||
         attr_bloom.contains(attr);
}

void JSONContainer::setLastUsed() {
  lastUsed = static_cast<unsigned long>(
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now().time_since_epoch())
          .count());
}

bool JSONContainer::isReparsable() {
  for (auto&& doc : docs) {
    if (!doc.getOrigin()->isReparsable()) {
      return false;
    }
  }
  return true;
}

void JSONContainer::removeDocuments() {
  if (deleted) {
    return;
  }
  lastParsedSize = estimatedSize();
  if (!isReparsable()) {
    serializeMissing();
  }
  for (auto&& doc : docs) {
    doc.removeDoc();
  }

  removeViews();
  alloc = std::make_unique<RJMemoryPoolAlloc>();
  deleted = true;
}

void JSONContainer::reparse() {
  if (!deleted) {
    return;
  }
  DCHECK(isReparsable()) << "Should be reparsable to reparse";
  if (docs.empty()) {
    return;
  }
  std::vector<bool> parsed(docs.size(), false);
  std::vector<FileOrigin::ParseInterval> intervals;

  DLOG(INFO) << "Reparsing container";
  for (size_t i = 0; i < docs.size(); ++i) {
    auto& doc = docs[i];
    if (doc.isValid()) {
      parsed[i] = true;
      continue;
    }
    auto& orig = doc.getOrigin();
    if (orig->isReparsable()) {
      const auto* fileOrig = dynamic_cast<const FileOrigin*>(orig.get());
      if (fileOrig != nullptr) {  // Get Parsing Intervals
        intervals.push_back(fileOrig->getInterval());
        continue;
      }
      auto tmpDoc = orig->reparse(*alloc);
      parsed[i] = true;
      if (tmpDoc == nullptr) {
        LOG(ERROR) << "Document could not be reparsed. Skipping";
      } else {
        doc.setJson(std::move(tmpDoc));
      }
    } else {
      DCHECK(false) << "Fatal error: All documents should have been reparsable";
    }
  }

  // Merge intervals
  intervals = FileOrigin::mergeIntervals(std::move(intervals));
  auto fileDocs = FileOrigin::parseIntervals(*alloc, std::move(intervals));
  // Reverse to be able to pop_back
  DCHECK_EQ(((size_t)std::count(parsed.begin(), parsed.end(), false)),
            fileDocs.size())
      << "All remaining documents should have been parsed";
  std::reverse(fileDocs.begin(), fileDocs.end());
  for (size_t i = 0; i < docs.size(); ++i) {
    if (parsed[i]) {
      continue;
    }
    auto& doc = docs[i];
    doc.setJson(std::move(fileDocs.back()));
    DCHECK(doc.isValid()) << "Parser should only return valid documents";
    fileDocs.pop_back();
    if (fileDocs.empty()) {
      break;
    }
  }

  DCHECK(fileDocs.empty()) << "All documents should be assigned";

  removeViews();
  setViews();
  deleted = false;
  lastParsedSize = 0;
  calculateBloom();
}

void JSONContainer::reparseSubset(unsigned long start, unsigned long end) {
  if (!deleted) {
    return;
  }
  if (docs.empty()) {
    return;
  }

  end = std::min(docs.size() - 1, end);
  std::vector<bool> parsed(1 + (end - start), false);
  std::vector<FileOrigin::ParseInterval> intervals;
  DLOG(INFO) << "Reparsing container [" << start << "," << end << "]";
  if (isView()) {
    auto minBase = baseIds[start];
    auto maxBase = baseIds[end];
    baseContainer->reparseSubset(minBase, maxBase);
  }
  for (unsigned long i = start; i <= end; ++i) {
    auto& doc = docs[i];
    if (doc.isValid()) {
      parsed[i] = true;
      continue;
    }
    auto& orig = doc.getOrigin();
    if (orig->isReparsable()) {
      const auto* fileOrig = dynamic_cast<const FileOrigin*>(orig.get());
      if (fileOrig != nullptr) {  // Get Parsing Intervals
        intervals.push_back(fileOrig->getInterval());
        continue;
      }
      parsed[i - start] = true;
      auto tmpDoc = orig->reparse(*alloc);
      if (tmpDoc == nullptr) {
        LOG(ERROR) << "Document could not be reparsed. Skipping";

      } else {
        doc.setJson(std::move(tmpDoc));
      }
    } else {
      DCHECK(false) << "This should not happen";
    }
  }

  // Merge intervals
  intervals = FileOrigin::mergeIntervals(std::move(intervals));
  auto fileDocs = FileOrigin::parseIntervals(*alloc, std::move(intervals));
  // Reverse to be able to pop_back
  DCHECK_EQ(((size_t)std::count(parsed.begin(), parsed.end(), false)),
            fileDocs.size())
      << "All remaining documents should have been parsed";
  std::reverse(fileDocs.begin(), fileDocs.end());
  for (size_t i = 0; i < docs.size(); ++i) {
    if (parsed[i]) {
      continue;
    }
    auto& doc = docs[i];
    doc.setJson(std::move(fileDocs.back()));
    DCHECK(doc.isValid()) << "Parser should only return valid documents";
    fileDocs.pop_back();
    if (fileDocs.empty()) {
      break;
    }
  }

  DCHECK(fileDocs.empty()) << "All documents should be assigned";

  removeViews();
  setViews();
}

void JSONContainer::reparseSubset(const DocIndex& index) {
  if (!deleted) {
    return;
  }
  if (docs.empty()) {
    return;
  }

  std::vector<bool> parsed(docs.size(), false);
  std::vector<FileOrigin::ParseInterval> intervals;
  DLOG(INFO) << "Reparsing container with indices";
  if (isView()) {
    auto baseIndex = DocIndex(baseContainer->size());
    for (unsigned long i = 0; i < docs.size(); ++i) {
      if (!index[i]) continue;
      baseIndex[baseIds[i]] = true;
    }
    baseContainer->reparseSubset(baseIndex);
  }
  for (unsigned long i = 0; i < docs.size(); ++i) {
    if (!index[i]) continue;
    auto& doc = docs[i];
    if (doc.isValid()) {
      parsed[i] = true;
      continue;
    }
    auto& orig = doc.getOrigin();
    if (orig->isReparsable()) {
      const auto* fileOrig = dynamic_cast<const FileOrigin*>(orig.get());
      if (fileOrig != nullptr) {  // Get Parsing Intervals
        intervals.push_back(fileOrig->getInterval());
        continue;
      }
      parsed[i] = true;
      auto tmpDoc = orig->reparse(*alloc);
      if (tmpDoc == nullptr) {
        LOG(ERROR) << "Document could not be reparsed. Skipping";

      } else {
        doc.setJson(std::move(tmpDoc));
      }
    } else {
      DCHECK(false) << "This should not happen";
    }
  }

  // Merge intervals
  intervals = FileOrigin::mergeIntervals(std::move(intervals));
  auto fileDocs = FileOrigin::parseIntervals(*alloc, std::move(intervals));
  // Reverse to be able to pop_back
  std::reverse(fileDocs.begin(), fileDocs.end());
  for (size_t i = 0; i < docs.size(); ++i) {
    if (parsed[i] || !index[i]) {
      continue;
    }
    auto& doc = docs[i];
    doc.setJson(std::move(fileDocs.back()));
    DCHECK(doc.isValid()) << "Parser should only return valid documents";
    fileDocs.pop_back();
    if (fileDocs.empty()) {
      break;
    }
  }

  DCHECK(fileDocs.empty()) << "All documents should be assigned";

  removeViews();
  setViews();
}

const std::unique_ptr<QueryCache>& JSONContainer::getCache() const {
  return cache;
}

const std::unique_ptr<ContainerIndex>& JSONContainer::getAdaptiveIndex() const {
  return adaptiveIndex;
}

const void JSONContainer::setAdaptiveIndex(
    std::unique_ptr<ContainerIndex> index) {
  adaptiveIndex = std::move(index);
}

const std::unique_ptr<joda::extension::ModuleExecutorStorage>&
JSONContainer::getModuleStorage() const {
  return moduleStorage;
}

std::vector<std::unique_ptr<RJDocument>> JSONContainer::projectDocuments(
    const DocIndex& ids,
    const std::vector<std::unique_ptr<joda::query::IProjector>>& proj,
    RJMemoryPoolAlloc& alloc,
    const std::vector<std::unique_ptr<joda::query::ISetProjector>>& setProj) {
  std::vector<std::unique_ptr<RJDocument>> ret;
  ScopedRef useCont(this);
  DCHECK(ids.size() == docs.size());
  bool used = false;
  for (size_t i = 0; i < docs.size(); ++i) {
    if (!ids[i]) {
      continue;
    }
    auto& doc = docs[i];
    auto newDoc = std::make_unique<RJDocument>(&alloc);
    newDoc->SetNull();
    // assert(!proj.empty() && "If empty, then star projection is handled
    // before");
    bool valid = false;
    for (auto&& projection : proj) {
      used = true;
      projection->project(doc, *newDoc);
      valid = true;
    }

    if (setProj.empty()) {
      if (valid) {
        ret.push_back(std::move(newDoc));
      }
    } else {
      std::vector<std::unique_ptr<RJDocument>> newDocs;
      newDocs.push_back(std::move(newDoc));
      for (auto&& sp : setProj) {
        used = true;
        sp->project(doc, newDocs);
        valid = true;
      }
      if (valid) {
        std::move(newDocs.begin(), newDocs.end(), std::back_inserter(ret));
      }
    }
  }
  if (used) {
    setLastUsed();
  }
  return ret;
}

DocumentCostHandler JSONContainer::createTempViewDocs(
    const DocIndex& ids,
    const std::vector<std::unique_ptr<joda::query::IProjector>>& proj,
    const std::vector<std::unique_ptr<joda::query::ISetProjector>>& setProj) {
  DocumentCostHandler costHandler;
  auto tmpCont = std::make_unique<JSONContainer>();
  tmpCont->baseContainer = this;
  if (proj.empty() && setProj.empty()) {
    for (size_t i = 0; i < docs.size(); ++i) {
      if (!ids[i]) {
        continue;
      }
      auto newDoc = std::make_unique<RJDocument>(tmpCont->getAlloc());
      newDoc->SetNull();
      costHandler.checkDocument(*newDoc);
      tmpCont->insertDoc(std::move(newDoc), docs[i].getOrigin()->clone(), i);
    }
  } else {
    bool used = false;
    for (size_t j = 0; j < docs.size(); ++j) {
      auto& doc = docs[j];
      if (!ids[j]) {
        continue;
      }
      auto newDoc = std::make_unique<RJDocument>(tmpCont->getAlloc());
      newDoc->SetNull();
      bool valid = false;
      for (size_t i = 1; i < proj.size(); ++i) {  // First projection is "*"
        used = true;
        /*
         * Copy missing base array
         */
        auto ptr = proj[i]->getRawToPointer();
        auto* tokens = ptr.GetTokens();
        int earliestPossibleArray = -1;
        for (size_t k = 0; k < ptr.GetTokenCount(); ++k) {
          std::string s = (tokens + k)->name;
          if ((!s.empty() && std::find_if(s.begin(), s.end(),
                                          [](char c) {
                                            return std::isdigit(c) == 0;
                                          }) == s.end()) ||
              s == "-") {
            earliestPossibleArray = k;
            break;
          }
        }
        if (earliestPossibleArray >= 0) {
          for (size_t k = earliestPossibleArray; k < ptr.GetTokenCount(); ++k) {
            auto* token = tokens + k;
            DCHECK(token != nullptr);
            std::string s = token->name;
            // If is array accessor
            if ((!s.empty() && std::find_if(s.begin(), s.end(),
                                            [](char c) {
                                              return std::isdigit(c) == 0;
                                            }) == s.end()) ||
                s == "-") {
              RJPointer tmpPtr(tokens, k);
              const auto* const tmpval = doc.Get(tmpPtr);
              if (tmpval != nullptr && tmpval->IsArray()) {
                if (tmpPtr.Get(*newDoc) == nullptr) {
                  tmpPtr.Set(*newDoc, *tmpval, newDoc->GetAllocator());
                  DCHECK(!tmpval->IsNull());
                }
                break;
              }
            }
            /*
             * Copy missing base array END
             */
          }
        }
        proj[i]->project(doc, *newDoc, true);
        valid = true;
      }

      if (setProj.empty()) {
        costHandler.checkDocument(*newDoc);
        if (valid) {
          tmpCont->insertViewDoc(std::move(newDoc), j);
        }
      } else {
        std::vector<std::unique_ptr<RJDocument>> newDocs;
        newDocs.push_back(std::move(newDoc));
        for (auto&& sp : setProj) {
          used = true;
          /*
           * Copy missing base array
           */
          auto ptr = sp->getRawToPointer();
          auto* tokens = ptr.GetTokens();
          int earliestPossibleArray = -1;
          for (size_t k = 0; k < ptr.GetTokenCount(); ++k) {
            std::string s = (tokens + k)->name;
            if ((!s.empty() && std::find_if(s.begin(), s.end(),
                                            [](char c) {
                                              return std::isdigit(c) == 0;
                                            }) == s.end()) ||
                s == "-") {
              earliestPossibleArray = k;
              break;
            }
          }
          if (earliestPossibleArray >= 0) {
            for (size_t k = earliestPossibleArray; k < ptr.GetTokenCount();
                 ++k) {
              auto* token = tokens + k;
              DCHECK(token != nullptr);
              std::string s = token->name;
              // If is array accessor
              if ((!s.empty() && std::find_if(s.begin(), s.end(),
                                              [](char c) {
                                                return std::isdigit(c) == 0;
                                              }) == s.end()) ||
                  s == "-") {
                RJPointer tmpPtr(tokens, k);
                const auto* const tmpval = doc.Get(tmpPtr);
                if (tmpval != nullptr && tmpval->IsArray()) {
                  for (auto& nD : newDocs) {
                    if (tmpPtr.Get(*nD) == nullptr) {
                      tmpPtr.Set(*nD, *tmpval, nD->GetAllocator());
                    }
                  }
                  break;
                }
              }
            }
          }
          /*
           * Copy missing base array END
           */
          sp->project(doc, newDocs);
          valid = true;
        }
        if (valid) {
          for (auto&& d : newDocs) {
            costHandler.checkDocument(*d);
            tmpCont->insertViewDoc(std::move(d), j);
          }
        }
      }
    }
    // Add Paths
    tmpCont->materializedAttributes.reserve(
        tmpCont->materializedAttributes.size() + proj.size() + setProj.size());
    for (size_t i = 1; i < proj.size(); ++i) {
      tmpCont->materializedAttributes.emplace_back(proj[i]->getToPointer());
    }
    for (auto&& sp : setProj) {
      tmpCont->materializedAttributes.emplace_back(sp->getToPointer());
    }
    costHandler.addViewPaths(tmpCont->materializedAttributes);
    if (used) {
      setLastUsed();
    }
  }
  addSubContainer(tmpCont.get());
  tmpCont->setViews();
  return costHandler;
}

std::unique_ptr<JSONContainer> JSONContainer::createViewFromContainer(
    const DocIndex& ids,
    const std::vector<std::unique_ptr<joda::query::IProjector>>& proj,
    const std::vector<std::unique_ptr<joda::query::ISetProjector>>& setProj) {
  DCHECK(config::enable_views &&
         (((!proj.empty() && proj.front()->getType() ==
                                 joda::query::PointerCopyProject::allCopy)) ||
          (proj.empty() && setProj.empty())))
      << "* Projection has to be the first projection";

  ScopedRef useCont(this, false);  // Does not need to parse everything yet
  auto tmpCont = std::make_unique<JSONContainer>();
  tmpCont->baseContainer = this;
  if ((proj.empty() ||
       (proj.size() == 1 &&
        proj.front()->getType() == joda::query::PointerCopyProject::allCopy)) &&
      setProj.empty()) {
    for (size_t i = 0; i < docs.size(); ++i) {
      if (!ids[i]) {
        continue;
      }
      auto newDoc = std::make_unique<RJDocument>(tmpCont->getAlloc());
      tmpCont->insertDoc(std::move(newDoc), docs[i].getOrigin()->clone(), i);
    }
  } else {
    ScopedRef useContContent(
        this);  // Needs to parse, and needs the actual content
    bool used = false;
    for (size_t j = 0; j < docs.size(); ++j) {
      auto& doc = docs[j];
      if (!ids[j]) {
        continue;
      }
      auto newDoc = std::make_unique<RJDocument>(tmpCont->getAlloc());
      bool valid = false;
      for (size_t i = 1; i < proj.size(); ++i) {  // First projection is "*"
        used = true;
        /*
         * Copy missing base array
         */
        const auto& ptr = proj[i]->getRawToPointer();
        auto* tokens = ptr.GetTokens();
        int earliestPossibleArray = -1;
        for (size_t k = 0; k < ptr.GetTokenCount(); ++k) {
          std::string s = (tokens + k)->name;
          if ((!s.empty() && std::find_if(s.begin(), s.end(),
                                          [](char c) {
                                            return std::isdigit(c) == 0;
                                          }) == s.end()) ||
              s == "-") {
            earliestPossibleArray = k;
            break;
          }
        }
        if (earliestPossibleArray >= 0) {
          for (size_t k = earliestPossibleArray; k < ptr.GetTokenCount(); ++k) {
            auto* token = tokens + k;
            DCHECK(token != nullptr);
            std::string s = token->name;
            // If is array accessor
            if ((!s.empty() && std::find_if(s.begin(), s.end(),
                                            [](char c) {
                                              return std::isdigit(c) == 0;
                                            }) == s.end()) ||
                s == "-") {
              RJPointer tmpPtr(tokens, k);
              const auto* const tmpval = doc.Get(tmpPtr);
              if (tmpval != nullptr && tmpval->IsArray()) {
                if (tmpPtr.Get(*newDoc) == nullptr) {
                  tmpPtr.Set(*newDoc, *tmpval, newDoc->GetAllocator());
                  DCHECK(!tmpval->IsNull());
                }
                break;
              }
            }
            /*
             * Copy missing base array END
             */
          }
        }
        proj[i]->project(doc, *newDoc, true);
        valid = true;
      }

      if (setProj.empty()) {
        if (valid) {
          tmpCont->insertViewDoc(std::move(newDoc), j);
        }
      } else {
        std::vector<std::unique_ptr<RJDocument>> newDocs;
        newDocs.push_back(std::move(newDoc));
        for (auto&& sp : setProj) {
          used = true;
          /*
           * Copy missing base array
           */
          const auto& ptr = sp->getRawToPointer();
          auto* tokens = ptr.GetTokens();
          int earliestPossibleArray = -1;
          for (size_t k = 0; k < ptr.GetTokenCount(); ++k) {
            std::string s = (tokens + k)->name;
            if ((!s.empty() && std::find_if(s.begin(), s.end(),
                                            [](char c) {
                                              return std::isdigit(c) == 0;
                                            }) == s.end()) ||
                s == "-") {
              earliestPossibleArray = k;
              break;
            }
          }
          if (earliestPossibleArray >= 0) {
            for (size_t k = earliestPossibleArray; k < ptr.GetTokenCount();
                 ++k) {
              auto* token = tokens + k;
              DCHECK(token != nullptr);
              std::string s = token->name;
              // If is array accessor
              if ((!s.empty() && std::find_if(s.begin(), s.end(),
                                              [](char c) {
                                                return std::isdigit(c) == 0;
                                              }) == s.end()) ||
                  s == "-") {
                RJPointer tmpPtr(tokens, k);
                const auto* const tmpval = doc.Get(tmpPtr);
                if (tmpval != nullptr && tmpval->IsArray()) {
                  for (auto& nD : newDocs) {
                    if (tmpPtr.Get(*nD) == nullptr) {
                      tmpPtr.Set(*nD, *tmpval, nD->GetAllocator());
                    }
                  }
                  break;
                }
              }
            }
          }
          /*
           * Copy missing base array END
           */
          sp->project(doc, newDocs);
          valid = true;
        }
        if (valid) {
          for (auto&& d : newDocs) {
            tmpCont->insertViewDoc(std::move(d), j);
          }
        }
      }
    }
    // Add Paths
    tmpCont->materializedAttributes.reserve(
        tmpCont->materializedAttributes.size() + proj.size() + setProj.size());
    for (size_t i = 1; i < proj.size(); ++i) {
      tmpCont->materializedAttributes.emplace_back(proj[i]->getToPointer());
    }
    for (auto&& sp : setProj) {
      tmpCont->materializedAttributes.emplace_back(sp->getToPointer());
    }
    if (used) {
      setLastUsed();
    }
  }
  addSubContainer(tmpCont.get());
  tmpCont->setViews();
  return tmpCont;
}

size_t JSONContainer::estimatedSize() const {
  size_t size = sizeof(JSONContainer);
  size += alloc->Capacity();
  size += docs.size() * (sizeof(RapidJsonDocument) + sizeof(FileOrigin));
  if (viewStruc != nullptr) {
    size += viewStruc->estimateSize();
  }
  // size += cache->getEstimatedSize(); Not implemented//TODO
  return size;
}

std::vector<std::string> JSONContainer::stringify(unsigned long start,
                                                  unsigned long end) {
  std::vector<std::string> ret;
  if (docs.empty()) {
    return ret;
  }
  ScopedRef useCont(this, false);
  end = std::min(end, docs.size() - 1);
  ret.reserve(end - start + 1);
  reparseSubset(start, end);
  if (start > end) {
    return ret;
  }
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  for (unsigned long i = start; i <= end; ++i) {
    auto& doc = docs[i];
    if (doc.isValid()) {
      buffer.Clear();
      writer.Reset(buffer);
      if (!isView()) {
        doc.getJson()->Accept(writer);
      } else {
        auto& view = doc.getView();
        view->Accept(writer);
      }

      ret.emplace_back(buffer.GetString());
    } else {
      ret.emplace_back();
    }
  }
  setLastUsed();
  return ret;
}

void JSONContainer::writeFile(const std::string& file, bool append) {
  JSONFileWriter writer;
  auto jsons = stringify();
  writer.writeFile(file, jsons, append);
  setLastUsed();
}

std::unique_ptr<const DocIndex> JSONContainer::getAllIDs() const {
  auto allIds = std::make_unique<DocIndex>(docs.size());
  allIds->set();
  return allIds;
}

std::vector<std::unique_ptr<RJDocument>> JSONContainer::getRaw(
    unsigned long start, unsigned long end) {
  std::vector<std::unique_ptr<RJDocument>> ret;
  if (docs.empty()) {
    return ret;
  }
  ScopedRef useCont(this, false);
  reparseSubset(start, end);

  end = std::min(end, docs.size() - 1);
  if (start > end) {
    return ret;
  }

  for (unsigned long i = start; i <= end; ++i) {
    auto& doc = docs[i];
    if (doc.isValid()) {
      auto newDoc = std::make_unique<RJDocument>();
      if (isView()) {
        auto& view = doc.getView();
        newDoc->Populate(*view);
      } else {
        newDoc->CopyFrom(*doc.getJson(), newDoc->GetAllocator(), true);
      }
      ret.push_back(std::move(newDoc));
    } else {
      ret.emplace_back();
    }
  }
  return ret;
}

std::vector<std::unique_ptr<RJDocument>> JSONContainer::getRaw(
    const DocIndex& ids) {
  std::vector<std::unique_ptr<RJDocument>> ret;
  if (docs.empty()) {
    return ret;
  }
  ScopedRef ref(this);

  for (size_t i = 0; i < docs.size(); ++i) {
    if (!ids[i]) {
      continue;
    }
    auto& item = docs[i];
    if (item.isValid()) {
      auto doc = std::make_unique<RJDocument>();
      if (isView()) {
        auto& view = item.getView();
        doc->Populate(*view);
      } else {
        doc->CopyFrom(*item.getJson(), doc->GetAllocator(), true);
      }
      ret.push_back(std::move(doc));
    } else {
      ret.emplace_back();
    }
  }
  setLastUsed();
  return ret;
}

std::vector<std::unique_ptr<RJDocument>> JSONContainer::getRaw(
    const DocIndex& ids, RJMemoryPoolAlloc& alloc) {
  std::vector<std::unique_ptr<RJDocument>> ret;
  if (docs.empty()) {
    return ret;
  }
  ScopedRef ref(this);

  for (size_t i = 0; i < docs.size(); ++i) {
    if (!ids[i]) {
      continue;
    }
    auto& item = docs[i];
    if (item.isValid()) {
      auto doc = std::make_unique<RJDocument>(&alloc);
      if (isView()) {
        auto& view = item.getView();
        doc->Populate(*view);
      } else {
        doc->CopyFrom(*item.getJson(), alloc, true);
      }
      ret.push_back(std::move(doc));
    } else {
      ret.emplace_back();
    }
  }
  setLastUsed();
  return ret;
}

size_t JSONContainer::getMaxSize() const { return maxSize; }

bool JSONContainer::hasMetaSpace(size_t size) {
  auto d = size * config::text_binary_mod;
  auto ret = theoreticalSize + d <= maxSize;
  if (ret) {
    theoreticalSize += d;
  }
  return ret;
}

void JSONContainer::insertDoc(std::unique_ptr<RJDocument>&& doc,
                              std::unique_ptr<IOrigin>&& origin,
                              size_t baseIndex) {
  insertDoc({std::move(doc), std::move(origin)}, baseIndex);
}

bool JSONContainer::serializeMissing() {
  std::string file;
  if (FileNameRepo::validFile(id)) {
    file = g_FileNameRepoInstance.getFile(id);
  } else {
    file = joda::filesystem::DirectoryRegister::getInstance().getUniqueDir();
    auto succ =
        joda::filesystem::DirectoryRegister::getInstance().registerDirectory(
            file, false);
    if (!succ) {
      LOG(ERROR) << "Could not register directory " << file;
      return false;
    }
    file += "/cont.json";
    id = g_FileNameRepoInstance.addFile(file);
  }
  DLOG(INFO) << "Serializing missing container documents to " << file;

  std::ofstream newfile;
  newfile.open(file.c_str(), std::ofstream::out | std::ofstream::app);
  if (!newfile.is_open()) {
    LOG(ERROR) << "Could not open file '" << file << "'.";
    return false;
  }
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  for (auto& doc : docs) {
    if (doc.isValid() && !doc.getOrigin()->isReparsable()) {
      buffer.Clear();
      writer.Reset(buffer);
      auto start = newfile.tellp();
      doc.getJson()->Accept(writer);
      newfile << buffer.GetString() << std::endl;
      newfile.flush();
      doc.setOrigin(
          std::make_unique<FileOrigin>(id, start, newfile.tellp(), 0));
    } else {
      LOG(ERROR) << "Can't serialize invalid document";
    }
  }
  newfile.close();
  return true;
}

JSONContainer::~JSONContainer() {
  while (!subContainers.empty()) {
    subContainers.back()->materializeView();  // This calls RemoveSubContainer
                                              // => subcontainer will be smaller
  }
  if (isView()) {
    baseContainer->removeSubContainer(this);
  }
  LOG(INFO) << "Removed JSONContainer " << contId;
}

void JSONContainer::preparePurge() {
  subContainers.clear();
  baseContainer = nullptr;
}

void JSONContainer::materializeAttributesIfRequired(
    const std::vector<std::string>& atts) {
  /*
   * Shared objects are objects of which at least one attribute is overwritten
   * by the view. Thereby we materialize all attributes for which
   * materializedAttributes contains an entry which is prefixed by the att
   */
  if (!isView()) {
    return;
  }
  if (atts.empty()) {
    return;
  }

  // Empty view (selection only view) => no materialization required
  if (materializedAttributes.empty()) {
    return;
  }

  std::vector<std::string> newatts;
  std::vector<std::string> subatts;
  for (const auto& att : atts) {
    if (att.empty()) {
      return materializeView();  // Materialize whole document
    }
    if (std::find(materializedAttributes.begin(), materializedAttributes.end(),
                  att) != materializedAttributes.end()) {
      continue;
    }

    auto attPath = (att.back() != '/') ? att + "/" : att;
    bool found = false;
    for (const auto& materializedAttribute : materializedAttributes) {
      if (materializedAttribute.size() > attPath.size() &&
          materializedAttribute.compare(0, attPath.size(), attPath) == 0) {
        LOG(INFO) << "Required '" << att << "', which is a shared object.";
        newatts.emplace_back(att);
        found = true;
        break;
      }
    }
    if (!found) {
      subatts.emplace_back(
          att);  // If not prefixed in this view, maybe in one below
    }
  }
  if (!subatts.empty()) {
    baseContainer->materializeAttributesIfRequired(subatts);
  }
  if (!newatts.empty()) {
    materializeAttributes(newatts);
  }
}

void JSONContainer::materializeAttributes(
    const std::vector<std::string>& atts) {
  if (!isView()) {
    return;
  }
  if (atts.empty()) {
    return;
  }
  ScopedRef useCont(this);

  for (const auto& att : atts) {
    if (std::find(materializedAttributes.begin(), materializedAttributes.end(),
                  att) != materializedAttributes.end()) {
      continue;
    }
    LOG(INFO) << "Materializing '" << att << "'";
    if (att.empty()) {
      return materializeView();  // Materialize whole document
    }
    RJPointer p(att);
    for (auto& doc : docs) {
      if (!doc.isValid()) {
        continue;
      }
      RJDocument val(alloc.get());
      auto& view = doc.getView();
      view->setPrefix(att);
      val.Populate(*view);
      view->setPrefix("");
      p.Set(*doc.getJson(), val, *alloc);
      DCHECK(val.IsNull()) << "Value should be moved out";
    }
    // Remove all paths prefixed by this path
    // eg: /A/B is overwritten in view, but /A is materialized => Only keep /A
    for (auto i = materializedAttributes.begin();
         i != materializedAttributes.end();) {
      if (i->compare(0, att.size(), att) == 0) {
        i = materializedAttributes.erase(i);
      } else {
        i++;
      }
    }
    materializedAttributes.emplace_back(att);
  }
  viewsComputed = false;
}

bool JSONContainer::isView() const { return baseContainer != nullptr; }

unsigned long JSONContainer::getLastUsed() const { return lastUsed; }

void JSONContainer::materializeView() {
  if (!isView()) {
    return;
  }
  LOG(INFO) << "Materializing complete view";
  ScopedRef useCont(this);
  auto a = std::make_unique<RJMemoryPoolAlloc>();

  for (auto& doc : docs) {
    auto newJson = std::make_unique<RJDocument>(a.get());
    auto& view = doc.getView();
    CHECK(view != nullptr);
    newJson->Populate(*view);
    doc.setJson(std::move(newJson));
  }
  alloc = std::move(a);
  baseContainer->removeSubContainer(this);
  baseContainer = nullptr;

  removeViews();
}

void JSONContainer::addSubContainer(JSONContainer* cont) {
  subContainers.push_back(cont);
}

void JSONContainer::removeSubContainer(JSONContainer* cont) {
  auto it = std::find(subContainers.begin(), subContainers.end(), cont);
  if (it != subContainers.end()) {
    subContainers.erase(it);
  }
}

void JSONContainer::setViews() {
  //TODO views are always used, even in normal containers. How to fix?
  if (viewsComputed || !config::enable_views /*|| baseContainer == nullptr*/) {
    return;
  }
  if (viewStruc == nullptr) {
    viewStruc = std::make_unique<ViewStructure>(size());
  }
  if (!isView()) {
    materializedAttributes.clear();
    materializedAttributes.emplace_back("");
    for (auto& doc : docs) {
      if (!(doc.getJson()->IsObject() || doc.getJson()->IsArray())) {
        continue;
      }
      doc.setView(std::make_unique<ViewLayer>(doc.getJson().get(),
                                              &materializedAttributes, nullptr,
                                              viewStruc.get()));
      DCHECK(doc.getView() != nullptr);
    }
  } else {
    baseContainer->setViews();
    for (size_t i = 0; i < docs.size(); i++) {
      auto& doc = docs[i];
      doc.setView(std::make_unique<ViewLayer>(
          doc.getJson().get(), &materializedAttributes,
          baseContainer->docs[baseIds[i]].getView().get(), viewStruc.get()));
      DCHECK(doc.getView() != nullptr);
    }
  }

  viewsComputed = true;
}

void JSONContainer::removeViews() {
  for (const auto& container : subContainers) {
    container->removeViews();
  }
  viewsComputed = false;
  viewStruc = nullptr;
  for (auto& doc : docs) {
    doc.setView(nullptr);
  }
  if (materializedAttributes.empty() && baseContainer != nullptr) {
    // If the view did not change anything, then remove basecontainer
    // As this was a selection only view and all documents are reparsable
    baseContainer->removeSubContainer(this);
    baseContainer = nullptr;
  }
}
bool JSONContainer::useViewBasedOnSample(
    const DocIndex& ids,
    const std::vector<std::unique_ptr<joda::query::IProjector>>& proj,
    const std::vector<std::unique_ptr<joda::query::ISetProjector>>& setProj) {
  auto count = ids.count();
  if (count <= 100) {
    return true;  // too small count, makes no sense
  }
  auto sample_count_f = count * 0.01;
  auto sample_num =
      std::max(static_cast<size_t>(1), static_cast<size_t>(sample_count_f));
  RJMemoryPoolAlloc tmp_alloc;

  size_t sampled = 0;
  size_t i = 0;
  DocIndex tmpids(ids.size());
  while (sampled < sample_num) {
    if (ids[i]) {
      tmpids[i] = true;
      sampled++;
    }
    i++;
  }

  auto viewCosts = createTempViewDocs(tmpids, proj, setProj);
  DocumentCostHandler defaultCost;
  if (proj.empty() && setProj.empty()) {
    forAll(
        [&defaultCost](const RapidJsonDocument& d, size_t i) {
          defaultCost.checkDocument(*d.getJson());
        },
        tmpids);
  } else {
    auto default_documents = projectDocuments(tmpids, proj, tmp_alloc, setProj);
    for (const auto& document : default_documents) {
      defaultCost.checkDocument(*document);
    }
  }

  auto viewCostSize = viewCosts.getCost();
  auto defaultCostSize = defaultCost.getCost();
  return viewCostSize < defaultCostSize;
}

bool JSONContainer::isBaseContainer(const JSONContainer* cont) const {
  return cont == baseContainer;
}

size_t JSONContainer::parsedSize() const {
  if (lastParsedSize != 0) {
    return lastParsedSize;
  }
  { return estimatedSize(); }
}

unsigned long JSONContainer::getContainerID() const { return contId; }

std::atomic_ulong JSONContainer::contIdCounter{1};