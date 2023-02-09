#include "ManagedIndexData.h"

ManagedIndexData::ManagedIndexData(const std::string& jp, ManagedDataType dt)
    : jsonPath(jp), dataType(dt) {}

void ManagedIndexData::updateSize(size_t additionalSize) {
  memorySize += additionalSize;
}

void ManagedIndexData::setSize(size_t size) { memorySize = size; }

void ManagedIndexData::updateLastUsed(const DataContext& dataContext) {
  updateLastUsed(0, dataContext);
}

void ManagedIndexData::updateLastUsed(size_t additionalSize,
                                      const DataContext& dataContext) {
  memorySize += additionalSize;

  if (previous != nullptr) {
    if (next == nullptr) {
      return;  // already end of chain
    } else {   // middle of chain
      previous->next = next;
      next->previous = previous;
    }
  } else if (next != nullptr) {  // start of chain
    next->previous = nullptr;
    dataContext.getMemoryManager()->setOldest(next);
  }

  ManagedIndexData* recent = dataContext.getMemoryManager()->getRecent();

  if (recent != nullptr) {  // handle empty memory queue
    this->previous = recent;
    recent->next = this;
  } else {
    dataContext.getMemoryManager()->setOldest(this);
  }

  dataContext.getMemoryManager()->setRecent(this);
}

size_t ManagedIndexData::getSize() const { return memorySize; }
