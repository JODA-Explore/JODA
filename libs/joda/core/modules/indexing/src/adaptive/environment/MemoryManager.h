#pragma once

#include <functional>
#include "ManagedDataType.h"

class ManagedIndexData;

class MemoryManager {
public:
    MemoryManager(
            size_t resultSize,
            std::function<void(std::string, ManagedDataType)> removeElement);

    bool ensureMemory(size_t neededMemory, ManagedIndexData *callingElement);

    ManagedIndexData* getRecent();
    ManagedIndexData* getOldest();
    void setRecent(ManagedIndexData* recent);
    void setOldest(ManagedIndexData* oldest);
private:
    bool free(uint64_t size, ManagedIndexData *callingElement);
    bool isAvailable(uint64_t size);

    unsigned long buffer;
    ManagedIndexData* recentlyUsedElement;
    ManagedIndexData* oldestUsedElement;
    std::function<void(std::string, ManagedDataType)> removeElement;
};