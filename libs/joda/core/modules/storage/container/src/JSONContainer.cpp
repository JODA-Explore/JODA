//
// Created by Nico Schäfer on 9/7/17.
//

#include <unordered_set>
#include <fstream>
#include <glog/logging.h>
#include "../include/joda/container/JSONContainer.h"
#include "joda/config/config.h"
#include <joda/fs/DirectoryRegister.h>
#include <rapidjson/writer.h>
#include <joda/misc/JSONFileWriter.h>
#include <joda/document/FileOrigin.h>
#include <joda/fs/DirectoryRegister.h>

JSONContainer::JSONContainer() : JSONContainer(config::JSONContainerSize) {

}

JSONContainer::JSONContainer(size_t maxsize) : maxSize(maxsize) {
  if(config::JSONContainerSize != 0){
    this->maxSize = config::JSONContainerSize;
  }else{
    if (maxSize == 0) {
      this->maxSize = JSON_CONTAINER_DEFAULT_SIZE;
      LOG(WARNING) << "JSONContainer size not able to autodetect, this should not happen. Fallback size: " << this->maxSize;
    }
  }

  assert(maxSize > 0);
  alloc = std::make_unique<RJMemoryPoolAlloc>(
      maxSize * config::chunk_size); //Container size * chunk-fraction

  cache = std::make_unique<QueryCache>();

}

bool JSONContainer::hasSpace(size_t size) const {
  if (maxSize == 0) return true;
  return alloc->Size() + size < maxSize;
}

RJMemoryPoolAlloc *JSONContainer::getAlloc() {
  return alloc.get();
}

void JSONContainer::insertDoc(RapidJsonDocument &&doc) {
  assert(!final);
  //Update indices
  maxID = std::max(maxID, doc.getId());
  minID = std::min(minID, doc.getId());
  docs.emplace_back(std::move(doc));
}

unsigned long JSONContainer::size() const {
  return docs.size();
}

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

std::shared_ptr<const DocIndex> JSONContainer::checkDocuments(std::function<bool(RapidJsonDocument &, size_t)> &func) {
  reparse();
  DocIndex ret(docs.size());
  for (size_t i = 0; i < docs.size(); ++i) {
    auto &doc = docs[i];
    if (doc.valid && func(doc.doc, i)) {
      ret[i] = true;
    }
  }
  setLastUsed();
  return std::make_shared<const DocIndex>(std::move(ret));
}

void JSONContainer::calculateBloom() {
  if (!config::bloom_enabled || bloomCalculated) return;
  //Bloom
  bloom_parameters parameters;
  parameters.projected_element_count = config::bloom_count;
  parameters.false_positive_probability = config::bloom_prob;
  parameters.compute_optimal_parameters();
  attr_bloom = bloom_filter(parameters);

  for (auto &&doc : docs) {
    if (doc.isValid() && doc.doc.getJson()->IsObject())
      recursiveBloomAttrSearch(*doc.doc.getJson());
  }
  bloomCalculated = true;

}

void JSONContainer::recursiveBloomAttrSearch(const RJValue &obj, const std::string &attr) {
  //Add attr to bloom
  if (!attr.empty()) attr_bloom.insert(attr);

  //Get members
  if (obj.IsObject())
    for (const auto &member : obj.GetObject()) {
      std::string tmp = attr;
      tmp += "/";
      tmp += member.name.GetString();
      recursiveBloomAttrSearch(member.value, tmp);
    }
}

bool JSONContainer::probContainsAttr(const std::string &attr) const {
  DCHECK(final) << "JSONContainer has to be final";
  return !config::bloom_enabled || !bloomCalculated || attr_bloom.contains(attr);
}

void JSONContainer::setLastUsed() {
  lastUsed = static_cast<unsigned long>(std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()).count());
}

bool JSONContainer::isReparsable() {
  for (auto &&doc : docs) {
    if (!doc.doc.getOrigin()->isReparsable()) return false;
  }
  return true;
}

void JSONContainer::removeDocuments() {
  if (deleted) return;
  if (!isReparsable()) serializeMissing();
  for (auto &&doc : docs) {
    doc.remove();
  }

  alloc = std::make_unique<RJMemoryPoolAlloc>();
  deleted = true;
}

void JSONContainer::reparse() {
  if (!deleted) return;
  DCHECK(isReparsable()) << "Should be reparsable to reparse";
  if (docs.empty()) return;
  /*
   * Get Parsing Intervals
   */
  DLOG(INFO) << "Reparsing container";
  for (auto &doc : docs) {
    auto *orig = doc.doc.getOrigin();
    if (orig->isReparsable()) {
      auto tmpDoc = orig->reparse(*alloc);
      if (tmpDoc == nullptr) {
        LOG(ERROR) << "Document could not be reparsed. Skipping";
        doc.valid = false;
      } else {
        doc.doc.setJson(std::move(tmpDoc));
        doc.valid = true;
      }
    } else {
      DCHECK(false) << "This should not happen";
    }
  }
  deleted = false;
  calculateBloom();
}

void JSONContainer::reparseSubset(unsigned long start, unsigned long end) {
  if (!deleted) return;
  if (docs.empty()) return;
  end = std::min(docs.size() - 1, end);
  DLOG(INFO) << "Reparsing container [" << start << "," << end << "]";
  for (unsigned long i = start; i <= end; ++i) {
    auto &doc = docs[i];
    auto *orig = doc.doc.getOrigin();
    if (!doc.isValid() && orig->isReparsable()) {
      auto tmpDoc = orig->reparse(*alloc);
      if (tmpDoc == nullptr) {
        LOG(ERROR) << "Document could not be reparsed. Skipping";
        doc.valid = false;
      } else {
        doc.doc.setJson(std::move(tmpDoc));
        doc.valid = true;
      }
    } else {
      DCHECK(false) << "This should not happen";
    }
  }
}

std::vector<RapidJsonDocument> JSONContainer::getDocuments(const DocIndex &ids) {
  std::vector<RapidJsonDocument> ret;
  reparse();
  DCHECK(ids.size() == docs.size());
  bool used = false;
  for (size_t i = 0; i < docs.size(); ++i) {
    if (ids[i] && docs[i].valid) {
      used = true;
      ret.push_back(docs[i].doc);
    }
  }
  if (used) setLastUsed();
  return ret;
}

std::vector<RapidJsonDocument> JSONContainer::getDocuments() {
  std::vector<RapidJsonDocument> ret;
  reparse();
  for (auto &&doc : docs) {
    ret.push_back(doc.doc);
  }

  setLastUsed();
  return ret;
}

const std::unique_ptr<QueryCache> &JSONContainer::getCache() const {
  return cache;
}

std::vector<std::shared_ptr<RJDocument>> JSONContainer::projectDocuments(const DocIndex &ids,
                                                                         const std::vector<std::unique_ptr<
                                                                             joda::query::IProjector>> &proj,
                                                                         RJMemoryPoolAlloc &alloc,
                                                                         const std::vector<
                                                                             std::unique_ptr<joda::query::ISetProjector>> &setProj) {
  std::vector<std::shared_ptr<RJDocument>> ret;
  reparse();
  DCHECK(ids.size() == docs.size());
  bool used = false;
  for (size_t i = 0; i < docs.size(); ++i) {
    if (ids[i]) {
      auto &doc = docs[i];
      if (!doc.valid) continue;
      auto newDoc = std::make_shared<RJDocument>(&alloc);
      newDoc->SetNull();
      //assert(!proj.empty() && "If empty, then star projection is handled before");
      bool valid = false;
      for (auto &&projection : proj) {
        used = true;
        projection->project(doc.doc, *newDoc);
        valid = true;
      }

      if (setProj.empty()) {
        if (valid)ret.push_back(std::move(newDoc));
      } else {
        std::vector<std::shared_ptr<RJDocument>> newDocs;
        newDocs.push_back(std::move(newDoc));
        for (auto &&sp : setProj) {
          used = true;
          sp->project(doc.doc, newDocs);
          valid = true;
        }
        if (valid)
          std::move(newDocs.begin(), newDocs.end(), std::back_inserter(ret));
      }
    }
  }
  if (used) setLastUsed();
  return ret;
}

size_t JSONContainer::estimatedSize() const {
  size_t size = sizeof(JSONContainer);
  size += alloc->Size();
  size += docs.size() * sizeof(RapidJsonDocument);
  //size += cache->getEstimatedSize(); Not implemented//TODO
  return size;
}

std::vector<std::string> JSONContainer::stringify(unsigned long start, unsigned long end) {
  std::vector<std::string> ret;
  if(docs.empty()) return ret;
  reparseSubset(start,end);

  end = std::min(end, docs.size() - 1);
  if (start > end) return ret;
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  for (unsigned long i = start; i <= end; ++i) {
    if (docs[i].isValid()) {
      buffer.Clear();
      writer.Reset(buffer);
      docs[i].doc.getJson()->Accept(writer);
      ret.emplace_back(buffer.GetString());
    } else ret.emplace_back();
  }
  if (!config::storeJson) removeDocuments();
  setLastUsed();
  return ret;
}

void JSONContainer::writeFile(const std::string &file, bool append) {
  JSONFileWriter writer;
  auto jsons = stringify();
  writer.writeFile(file, jsons, append);
  setLastUsed();
}

std::shared_ptr<const DocIndex> JSONContainer::getAllIDs() const {
  return std::make_shared<const DocIndex>(docs.size(), true);
}

std::vector<std::shared_ptr<RJDocument>> JSONContainer::getRaw(unsigned long start, unsigned long end) {
  std::vector<std::shared_ptr<RJDocument>> ret;
  if(docs.empty()) return ret;
  reparseSubset(start,end);

  end = std::min(end, docs.size() - 1);
  if (start > end) return ret;

  for (unsigned long i = start; i <= end; ++i) {
    if (docs[i].isValid()) {
      auto doc = std::make_shared<RJDocument>();
      doc->CopyFrom(*docs[i].doc.getJson(), doc->GetAllocator());
      ret.push_back(std::move(doc));
    } else ret.emplace_back();
  }
  if (!config::storeJson) removeDocuments();
  setLastUsed();
  return ret;
}

bool JSONContainer::serialize(const std::string &file, FILEID id) {
  std::ofstream newfile;
  newfile.open(file.c_str(), std::ofstream::out | std::ofstream::app);
  if (!newfile.is_open()) {
    LOG(ERROR) << "Could not open file '" << file << "'.";
    return false;
  }
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  for (auto &&doc : docs) {
    if (doc.isValid()) {
      buffer.Clear();
      writer.Reset(buffer);
      auto start = newfile.tellp();
      doc.doc.getJson()->Accept(writer);
      newfile << buffer.GetString() << std::endl;
      newfile.flush();
      doc.doc.setOrigin(std::make_unique<FileOrigin>(id, start, newfile.tellp(), 0));
    }else{
      LOG(ERROR) << "Can't serialize invalid document";
    }
  }
  newfile.close();
  return true;
}


size_t JSONContainer::getMaxSize() const {
  return maxSize;
}

bool JSONContainer::hasMetaSpace(size_t size) {
  auto d = size * config::text_binary_mod;
  auto ret = theoreticalSize + d <= maxSize;
  if (ret) theoreticalSize += d;
  return ret;
}

void JSONContainer::insertDoc(DOC_ID id,
                              std::shared_ptr<RJDocument> &&doc,
                              std::unique_ptr<IOrigin> &&origin) {
  insertDoc({id, std::move(doc), std::move(origin)});
}

bool JSONContainer::serializeMissing() {
  std::string file;
  if (FileNameRepo::validFile(id)) {
    file = g_FileNameRepoInstance.getFile(id);
  } else {
    file = joda::filesystem::DirectoryRegister::getInstance().getUniqueDir();
    auto succ = joda::filesystem::DirectoryRegister::getInstance().registerDirectory(file, false);
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
  for (auto &&doc : docs) {
    if (doc.isValid() && !doc.doc.getOrigin()->isReparsable()) {
      buffer.Clear();
      writer.Reset(buffer);
      auto start = newfile.tellp();
      doc.doc.getJson()->Accept(writer);
      newfile << buffer.GetString() << std::endl;
      newfile.flush();
      doc.doc.setOrigin(std::make_unique<FileOrigin>(id, start, newfile.tellp(), 0));
    } else {
      LOG(ERROR) << "Can't serialize invalid document";
    }
  }
  newfile.close();
  return true;
}

JSONContainer::~JSONContainer() {

}

unsigned long JSONContainer::getLastUsed() const {
  return lastUsed;
}








