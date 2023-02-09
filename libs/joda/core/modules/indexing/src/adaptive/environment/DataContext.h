#pragma once

#include <joda/container/JSONContainer.h>
#include "MemoryManager.h"
#include <set>

class AdaptiveHistogram;

class DataContext {
public:
    DataContext(JSONContainer &container);
    RapidJsonDocument& at(int index);
    unsigned long size() const;

    void loadRange(const std::vector<size_t>& docs);
    void load(const std::vector<size_t>& docs);
    void load(const DocIndex &docIndex);
    void loadAll();

    std::shared_ptr<const DocIndex> checkDocuments(std::function<bool(RapidJsonDocument &, size_t)> &func);
    std::shared_ptr<const DocIndex> checkDocuments(std::function<bool(RapidJsonDocument &, size_t)> &func, const DocIndex &docIndex);

    void setMemoryManager(MemoryManager *memoryManager);
    MemoryManager *getMemoryManager() const;

    void setAdaptiveHistogram(AdaptiveHistogram *adaptiveHistogram);
    AdaptiveHistogram *getAdaptiveHistogram() const;

    void previousQueryExecutionFinished() { previousQueryFinished = true; trieValueCache.clear(); trieValueCacheIndex.clear(); }

    void setHistogramSplitted(bool histogramSplitted) { this->histogramSplitted = histogramSplitted; }
    bool getHistogramSplitted() { return histogramSplitted; }

    void addCacheResult(const AdaptiveIndexQuery &query, std::shared_ptr<const DocIndex> docIndex);
    std::shared_ptr<const DocIndex> getCacheResult(const AdaptiveIndexQuery &query);

    size_t tempSizeTracker;

    std::vector<size_t> trieValueCacheIndex;
    std::vector<std::string> trieValueCache;
private:
    JSONContainer &container;
    MemoryManager *memoryManager;
    AdaptiveHistogram *adaptiveHistogram;
    std::vector<std::pair<const AdaptiveIndexQuery, std::shared_ptr<const DocIndex>>> cachedResults;

    bool loaded = false;
    bool allLoaded = false;
    bool histogramSplitted = false;
    bool previousQueryFinished = false;
    std::unique_ptr<std::vector<RapidJsonDocument*>> partiallyLoadedDocuments;

    void createEmptyDocumentsIfNotExist();
};