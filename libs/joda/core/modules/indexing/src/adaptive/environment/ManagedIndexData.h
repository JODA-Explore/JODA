#pragma once

#include <chrono>
#include <boost/smart_ptr/weak_ptr.hpp>
#include "DataContext.h"
#include "ManagedDataType.h"

class ManagedIndexData {
    friend class MemoryManager;

public:
    size_t getSize() const;
protected:
    ManagedIndexData(const std::string& jp, ManagedDataType dataType);

    std::string jsonPath;
    void updateSize(size_t additionalSize);
    void setSize(size_t size);
    void updateLastUsed(const DataContext &dataContext);
    void updateLastUsed(size_t additionalSize, const DataContext &dataContext);
private:
    size_t memorySize = 0;
    ManagedDataType dataType;
    ManagedIndexData *previous = nullptr;
    ManagedIndexData *next = nullptr;
};