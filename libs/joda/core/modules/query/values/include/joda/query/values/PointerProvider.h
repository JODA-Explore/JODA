//
// Created by Nico Schäfer on 11/13/17.
//

#ifndef JODA_POINTERPROVIDER_H
#define JODA_POINTERPROVIDER_H

#include <rapidjson/fwd.h>
#include <rapidjson/pointer.h>

#include "IValueProvider.h"

namespace joda::query {
/**
 * '<JSON Pointer>', a pointer to a specific value in a document
 */
class PointerProvider : public joda::query::IValueProvider {
 public:
  PointerProvider(const std::string &str);

  bool comparable() const override;
  bool equalizable() const override;
  std::unique_ptr<IValueProvider> duplicate() const override;
  std::string toString() const override;
  std::string getAttributeString() const;
  bool isConst() const override;
  RJValue getAtomValue(const RapidJsonDocument &json,
                       RJMemoryPoolAlloc &alloc) const override;
  const RJValue *getValue(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) const override;
  void getAttributes(std::vector<std::string> &vec) const override;
  bool isString() const override;
  bool isNumber() const override;
  bool isBool() const override;
  bool isObject() const override;
  bool isArray() const override;
  bool isAny() const override;
  std::string getName() const override;
  IValueType getReturnType() const override;

 protected:
  RJPointer pointer;
};
}

#endif  // JODA_POINTERPROVIDER_H
