//
// Created by Nico Schäfer on 02/04/18.
//

#ifndef JODA_MEMORYJOINDOC_H
#define JODA_MEMORYJOINDOC_H


#include <mutex>
#include <rapidjson/document.h>
#include <rapidjson/allocators.h>
#include "joda/misc/RJFwd.h"

/**
 * This class represents an RJDocument containing all join documents
 */
class MemoryJoinDoc {
 public:
  /**
   * Initializes an MemoryJoinDoc with an number as hash-value
   * @param i The value to be hashed for
   */
  explicit MemoryJoinDoc(long i);
  /**
   * Initializes an MemoryJoinDoc with an string as hash-value
   * @param str The value to be hashed for
   */
  explicit MemoryJoinDoc(const std::string& str);
  /**
   * Initializes an MemoryJoinDoc with an Boolean as hash-value
   * @param b The value to be hashed for
   */
  explicit MemoryJoinDoc(bool b);

  /**
   * Adds an document to the list of joined documents by moving it
   * @param val The document/value to add
   */
  void join(RJValue&& val);
  /**
   * Adds an document to the list of joined documents by deep-copying it
   * @param val The document/value to add
   */
  void join(const RJValue& val);
  /**
   * Copies the internal document into a RJDocument
   * @param val THe document to copy into
   */
  void deepCopyInto(RJDocument &val);
  /**
   * Returns the memory size of the internal document.
   * @return the memory size of the internal document.
   */
  size_t getMemSize();
 protected:
  MemoryJoinDoc();
  RJMemoryPoolAlloc alloc;
  RJDocument doc;
  std::mutex mut;

};

#endif //JODA_MEMORYJOINDOC_H
