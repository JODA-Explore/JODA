#include "DataContext.h"
#include <joda/misc/RecurringTimer.h>
#include <iostream>

DataContext::DataContext(JSONContainer &c) : container(c) {
}

unsigned long DataContext::size() const {
    return container.size();
}

RapidJsonDocument &DataContext::at(int index) {
    return *partiallyLoadedDocuments->at(index);
}

/**
 * load functions only work on the premise that if you go deeper in a query, no NEW docs need to be loaded. The FIRST load, loads ALL documents
 * that can possibly be accessed.
 * @param startMatchingDocs
 */
void DataContext::loadRange(const std::vector<size_t> &docs) {
    if (allLoaded) return;

    if (!loaded) {
        createEmptyDocumentsIfNotExist();

        container.setDocuments(docs, *partiallyLoadedDocuments, true, false);
        loaded = true;
    } else if (previousQueryFinished) {
        container.setDocuments(docs, *partiallyLoadedDocuments, true, true);
        previousQueryFinished = false;
    }
}

void DataContext::load(const std::vector<size_t> &docs) {
    if (allLoaded) return;

    if (!loaded) {
        createEmptyDocumentsIfNotExist();

        container.setDocuments(docs, *partiallyLoadedDocuments, false, false);
        loaded = true;
    } else if (previousQueryFinished) {
        container.setDocuments(docs, *partiallyLoadedDocuments, false, true);
        previousQueryFinished = false;
    }
}

void DataContext::load(const DocIndex &docIndex) {
    if (allLoaded) return;

    if (!loaded) {
        createEmptyDocumentsIfNotExist();

        container.setDocuments(docIndex, *partiallyLoadedDocuments, false);
        loaded = true;
    } else if (previousQueryFinished) {
        container.setDocuments(docIndex, *partiallyLoadedDocuments, true);
        previousQueryFinished = false;
    }
}

void DataContext::setAdaptiveHistogram(AdaptiveHistogram *adaptiveHistogram) {
    this->adaptiveHistogram = adaptiveHistogram;
}

AdaptiveHistogram *DataContext::getAdaptiveHistogram() const {
    return adaptiveHistogram;
}

MemoryManager* DataContext::getMemoryManager() const {
    return memoryManager;
}

void DataContext::setMemoryManager(MemoryManager *memoryManager) {
    this->memoryManager = memoryManager;
}

void DataContext::loadAll() {
    if (allLoaded) return;

    createEmptyDocumentsIfNotExist();

    std::vector<size_t> docs;
    docs.push_back(0);
    docs.push_back(size() - 1);

    if (loaded) {
        container.setDocuments(docs, *partiallyLoadedDocuments, true, true);
        allLoaded = true;
    } else {
        container.setDocuments(docs, *partiallyLoadedDocuments, true, false);
        loaded = true;
        allLoaded = true;
    }
}

std::shared_ptr<const DocIndex> DataContext::checkDocuments(std::function<bool(RapidJsonDocument &, size_t)> &func) {
    loadAll();

    DocIndex ret(size());
    for (size_t i = 0; i < size(); ++i) {
        if (func(*partiallyLoadedDocuments->at(i), i)) {
            ret.set(i);
        }
    }
    return std::make_shared<const DocIndex>(std::move(ret));
}

void DataContext::createEmptyDocumentsIfNotExist() {
    if (!partiallyLoadedDocuments) {
        partiallyLoadedDocuments = std::make_unique<std::vector<RapidJsonDocument*>>(size());
    }
}

std::shared_ptr<const DocIndex>
DataContext::checkDocuments(std::function<bool(RapidJsonDocument &, size_t)> &func, const DocIndex &docIndex) {
    load(docIndex);

    DocIndex ret(size());
    for (size_t i = 0; i < size(); ++i) {

        if (docIndex[i] && func(*partiallyLoadedDocuments->at(i), i)) {
            ret.set(i);
        }
    }

    previousQueryExecutionFinished();
    return std::make_shared<const DocIndex>(std::move(ret));
}

void DataContext::addCacheResult(const AdaptiveIndexQuery &query, std::shared_ptr<const DocIndex> docIndex) {
    cachedResults.push_back({ query, docIndex });
}

std::shared_ptr<const DocIndex> DataContext::getCacheResult(const AdaptiveIndexQuery &query) {
    if (cachedResults.size() == 0) {
        return nullptr;
    }

    for (auto& result: cachedResults) {
        if (query.equals(result.first)) {
            return result.second;
        }
    }

    return nullptr;
}
