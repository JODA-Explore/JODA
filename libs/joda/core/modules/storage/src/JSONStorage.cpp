//
// Created by Nico Schäfer on 9/7/17.
//

#include "../include/joda/storage/JSONStorage.h"

#include <joda/concurrency/ThreadManager.h>
#include <joda/config/config.h>
#include <joda/fs/DirectoryRegister.h>

#include <utility>

void JSONStorage::insertDocuments(
    moodycamel::ConcurrentQueue<std::unique_ptr<JSONContainer>>& queue,
    const std::atomic_bool& cf, std::atomic_uint& cs) {
  std::unique_ptr<JSONContainer> cont;
  moodycamel::ConsumerToken ctok(queue);
  while (true) {
    if (!queue.try_dequeue(ctok, cont)) {  // If can't receive container
      if (cf.load() && cs.load() == 0) {
        return;  // Check if finished, if yes return
      }
      continue;  // Else continue
    }
    cs.fetch_sub(1);
    if (cont == nullptr) {
      LOG(WARNING) << "Received nullptr as container.";
    }
    documentMutex.lock();
    docCount += cont->size();
    container.push_back(std::move(cont));
    documentMutex.unlock();
  }
}

void JSONStorage::insertDocuments(std::unique_ptr<JSONContainer>&& cont) {
  if (cont == nullptr) {
    return;
  }
  std::lock_guard<std::mutex> lock(documentMutex);
  docCount += cont->size();
  container.push_back(std::move(cont));
}

unsigned long JSONStorage::size() const { return docCount; }

unsigned long JSONStorage::contSize() const { return container.size(); }

size_t JSONStorage::threadCount(size_t containerSize) const {
  size_t threads = std::max(
      std::min(g_ThreadManagerInstance.getMaxThreads(), containerSize), {1});
  LOG(INFO) << "Threads: " << std::to_string(threads);
  return threads;
}

size_t JSONStorage::estimatedSize() const {
  size_t size = sizeof(JSONStorage);
  for (auto&& item : container) {
    size += item->estimatedSize();
  }
  return size;
}

size_t JSONStorage::parsedSize() const {
  size_t size = sizeof(JSONStorage);
  for (auto&& item : container) {
    size += item->parsedSize();
  }
  return size;
}

size_t JSONStorage::estimatedCapacity() const {
  size_t size = 0;
  for (auto&& item : container) {
    size += item->getAlloc()->Capacity();
  }
  return size;
}

void JSONStorage::freeAllMemory() {
  LOG(INFO) << "Freeing memory of " << name;
  for (auto&& cont : container) {
    cont->removeDocuments();
  }
}

std::string JSONStorage::getStorageID() const {
  if (!name.empty()) {
    return "JSONStorage_" + name;
  }
  return "JSONStorage";
}

JSONStorage::JSONStorage(std::string query_string)
    : name(std::move(query_string)) {
  if (!config::storeJson) {
    regtmpdir =
        joda::filesystem::DirectoryRegister::getInstance().getUniqueDir();
    joda::filesystem::DirectoryRegister::getInstance().registerDirectory(
        regtmpdir, true);
  }
}
JSONStorage::~JSONStorage() {
  std::lock_guard<std::mutex> lock(documentMutex);

  DLOG(INFO) << "Removing containers " << name;
  for (auto& cont : container) {
    cont.reset();
  }

  DLOG(INFO) << "Cleaning up storage " << name;
  if (!config::storeJson) {
    joda::filesystem::DirectoryRegister::getInstance().removeDirectory(
        regtmpdir);
  }
}

void JSONStorage::preparePurge() {
  std::lock_guard<std::mutex> lock(documentMutex);
  for (auto& cont : container) {
    cont->preparePurge();
  }
}

const std::string& JSONStorage::getName() const { return name; }

void JSONStorage::writeFile(const std::string& file) {
  std::lock_guard<std::mutex> lock(documentMutex);
  LOG(INFO) << "Writing storage " << getName() << " to file " << file;
  for (auto&& cont : container) {
    // TODO: FILE ENDING
    cont->writeFile(file, true);
  }
}
void JSONStorage::writeFiles(const std::string& file) {
  std::lock_guard<std::mutex> lock(documentMutex);
  moodycamel::ConcurrentQueue<JSONContainer*> containerQueue(container.size(),
                                                             1, 0);
  moodycamel::ProducerToken ptok(containerQueue);

  if (contSize() == 0) {
    return;
  }
  const unsigned long sent_container{contSize()};
  std::atomic_ulong worked_container{0};
  auto threads = threadCount(contSize());

  std::vector<std::thread> threadList;
  for (size_t i = 0; i < threads; ++i) {
    threadList.push_back(
        std::thread([i, &ptok, &containerQueue, &worked_container, &file,
                     &sent_container]() {
          DLOG(INFO) << "Start writing to "
                     << file + "/" + std::to_string(i) + ".json";
          while (worked_container.load() < sent_container) {
            JSONContainer* cont = nullptr;
            if (!containerQueue.try_dequeue_from_producer(ptok, cont)) {
              continue;
            }
            assert(cont != nullptr);
            worked_container.fetch_add(1);
            cont->writeFile(file + "/" + std::to_string(i) + ".json", true);
          }
        }));
  }
  /*
   * Fill queue
   */
  for (auto&& item : container) {
    auto* ptr = item.get();
    if (item != nullptr) {
      containerQueue.enqueue(ptok, ptr);
    }
  }

  for (auto&& thread : threadList) {
    thread.join();
  }
}

std::vector<std::string> JSONStorage::stringify(unsigned long start,
                                                unsigned long end) {
  // TODO: maybe multithread
  DLOG(INFO) << "Getting strings of Storage in range [" << start << "," << end
             << "]";
  std::vector<std::string> ret;
  if (docCount == 0) {
    return ret;
  }
  end = std::min(end, docCount - 1);
  if (start > end) {
    return ret;
  }
  auto count = (end - start) + 1;
  ret.reserve(count);
  auto it = container.begin();

  unsigned long skippedSize = 0;
  // Browse container until designated is reached
  while (start >= (*it)->size() + skippedSize) {
    skippedSize += (*it)->size();
    it++;
    DCHECK(it != container.end());
  }

  while (ret.size() < count) {
    size_t contStart = 0;
    if (skippedSize < start) {
      contStart = start - skippedSize;
    }
    unsigned long contEnd = contStart + count - ret.size() - 1;
    if (it == container.end()) {
      DCHECK(false) << "If all Container contributed to the strings, this "
                       "should not be possible";
      return ret;
    }

    if ((*it)->size() != 0) {
      DLOG(INFO) << "Getting strings of container in range [" << contStart
                 << "," << contEnd << "], with a size of " << (*it)->size();
      auto tmp = (*it)->stringify(contStart, contEnd);
      DLOG(INFO) << "Added " << tmp.size() << " documents ";

      skippedSize += (*it)->size();
      std::move(tmp.begin(), tmp.end(), std::back_inserter(ret));
      it++;
    } else {
      DLOG(INFO) << "Skipping empty container";
    }
  }
  return ret;
}

std::vector<std::shared_ptr<RJDocument>> JSONStorage::getRaw(
    unsigned long start, unsigned long end) {
  std::lock_guard<std::mutex> lock(documentMutex);
  std::vector<std::shared_ptr<RJDocument>> ret;
  if (docCount == 0) {
    return ret;
  }
  end = std::min(end, docCount - 1);
  if (start > end) {
    return ret;
  }
  auto count = (end - start) + 1;
  ret.reserve(count);
  auto it = container.begin();

  unsigned long skippedSize = 0;
  // Browse container until designated is reached
  while (start >= (*it)->size() + skippedSize) {
    skippedSize += (*it)->size();
    it++;
    DCHECK(it != container.end());
  }

  while (ret.size() < count) {
    size_t contStart = start - skippedSize;
    contStart = std::max(contStart, static_cast<size_t>(0));
    unsigned long contEnd = contStart + count - ret.size() - 1;
    if (it == container.end()) {
      DCHECK(false) << "If all Container contributed to the raws, this should "
                       "not be possible";
      return ret;
    }

    if ((*it)->size() != 0) {
      auto tmp = (*it)->getRaw(contStart, contEnd);
      DCHECK_EQ(tmp.size(), contEnd - contStart + 1);
      std::move(tmp.begin(), tmp.end(), std::back_inserter(ret));
      it++;
    } else {
      DLOG(INFO) << "Skipping empty container";
    }
  }
  return ret;
}

const std::string& JSONStorage::getRegtmpdir() const { return regtmpdir; }

void JSONStorage::insertDocumentsQueue(JsonContainerQueue::queue_t* queue) {
  DCHECK(queue != nullptr);
  std::unique_ptr<JSONContainer> cont;
  JsonContainerQueue::queue_t::queue_t::consumer_token_t ctok(queue->queue);
  while (!queue->isFinished()) {
    DCHECK(cont == nullptr);
    queue->retrieve(ctok, cont);
    if (cont == nullptr) {
      LOG(WARNING) << "Received nullptr as container.";
      continue;
    }
    DCHECK(cont != nullptr);
    documentMutex.lock();
    docCount += cont->size();
    ;
    container.push_back(std::move(cont));
    documentMutex.unlock();
    DCHECK(cont == nullptr);
  }
}
void JSONStorage::insertDocumentsQueue(JsonContainerQueue::queue_t* queue,
                                       size_t& insertedDocs,
                                       size_t& insertedConts) {
  DCHECK(queue != nullptr);
  std::unique_ptr<JSONContainer> cont;
  JsonContainerQueue::queue_t::queue_t::consumer_token_t ctok(queue->queue);
  while (!queue->isFinished()) {
    DCHECK(cont == nullptr);
    queue->retrieve(ctok, cont);
    if (cont == nullptr) {
      LOG(WARNING) << "Received nullptr as container.";
      continue;
    }
    DCHECK(cont != nullptr);
    documentMutex.lock();
    docCount += cont->size();
    insertedConts++;
    insertedDocs += cont->size();
    container.push_back(std::move(cont));
    documentMutex.unlock();
    DCHECK(cont == nullptr);
  }
}

void JSONStorage::getDocumentsQueue(JsonContainerRefQueue::queue_t* queue) {
  DCHECK(queue != nullptr);
  queue->registerProducer();
  auto ptok = JsonContainerRefQueue::queue_t::ptok_t(queue->queue);
  for (auto& cont : container) {
    queue->send(ptok, cont.get());
  }
  queue->producerFinished();
}

unsigned long JSONStorage::getLastUsed() const {
  unsigned long maxLastUsed = 0;
  for (const auto& cont : container) {
    if (cont->getLastUsed() > maxLastUsed) {
      maxLastUsed = cont->getLastUsed();
    }
  }
  return maxLastUsed;
}

const std::vector<std::unique_ptr<JSONContainer>>& JSONStorage::getContainer()
    const {
  return container;
}
