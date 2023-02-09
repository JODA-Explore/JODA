#include <iostream>
#include "MemoryManager.h"
#include "joda/misc/MemoryUtility.h"
#include "ManagedIndexData.h"

MemoryManager::MemoryManager(unsigned long resultSize, std::function<void(std::string, ManagedDataType)> remEle) : removeElement(remEle) {
    buffer = resultSize * 2;
    recentlyUsedElement = nullptr;
    oldestUsedElement = nullptr;
}

bool MemoryManager::isAvailable(uint64_t size) {
    auto memory =  MemoryUtility::totalRam().getBytes();
    memory -= MemoryUtility::sysRamUsage().getBytes();
    memory -= size;
    memory -= buffer;

    return memory > 0;
}

bool MemoryManager::free(uint64_t size, ManagedIndexData *callingElement) {

    uint64_t freeAble = 0;
    ManagedIndexData *managedElement = oldestUsedElement;

    while (freeAble < size && managedElement != nullptr) {
        if (callingElement == nullptr || managedElement->jsonPath != callingElement->jsonPath ||
            (managedElement->dataType != UnionTreeNodeData && managedElement->dataType != callingElement->dataType)) {
            freeAble += managedElement->getSize();
        }

        managedElement = managedElement->next;
    }

    if (freeAble < size) {
        return false; // cannot free enough memory
    } else {

        if (managedElement == nullptr) {
            managedElement = recentlyUsedElement;
        }

        while (!isAvailable(size)) {

            if (managedElement == nullptr) {
                managedElement = oldestUsedElement;
            }

            while (managedElement != nullptr) {
                // skip if directly related to queried element
                if (callingElement == nullptr || managedElement->jsonPath != callingElement->jsonPath ||
                    (managedElement->dataType != UnionTreeNodeData &&
                     managedElement->dataType != callingElement->dataType)) {

                    if (managedElement->previous != nullptr) {
                        if (managedElement->next != nullptr) {
                            // between two elements
                            managedElement->previous->next = managedElement->next;
                            managedElement->next->previous = managedElement->previous;
                        } else {
                            // recent element with older elements
                            managedElement->previous->next = nullptr;
                            setRecent(managedElement->previous);
                        }
                    } else {
                        if (managedElement->next != nullptr) {
                            // left with more recent elements
                            setOldest(managedElement->next);
                            managedElement->next->previous = nullptr;
                        } else {
                            // last element left in chain
                            setOldest(nullptr);
                            setRecent(nullptr);
                        }
                    }

                    removeElement(managedElement->jsonPath, managedElement->dataType);
                }

                managedElement = managedElement->previous;
            }
        }
    }
    return true;
}

bool MemoryManager::ensureMemory(size_t neededSize, ManagedIndexData *callingElement) {
    if (!isAvailable(neededSize)) {
        if (!free(neededSize, callingElement)) {
            std::cout << "Not enough memory and not enough to free." << std::endl;
            return false;
        }
    }

    return true;
}

ManagedIndexData *MemoryManager::getRecent() {
    return recentlyUsedElement;
}

void MemoryManager::setRecent(ManagedIndexData *recent) {
    recentlyUsedElement = recent;
}

ManagedIndexData *MemoryManager::getOldest() {
    return oldestUsedElement;
}

void MemoryManager::setOldest(ManagedIndexData *oldest) {
    oldestUsedElement = oldest;
}
