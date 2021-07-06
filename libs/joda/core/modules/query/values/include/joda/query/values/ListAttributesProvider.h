//
// Created by Nico Schäfer on 12/7/17.
//

#ifndef JODA_LISTATTRIBUTESPROVIDER_H
#define JODA_LISTATTRIBUTESPROVIDER_H

#include "IValueProvider.h"
namespace joda::query {
/**
 * LISTATTRIBUTES() returns a list of all attributes in a given object
 */
class ListAttributesProvider : public joda::query::IValueProvider {
 public:
  explicit ListAttributesProvider(
      std::vector<std::unique_ptr<IValueProvider>> &&parameters);
  std::unique_ptr<IValueProvider> duplicate() const override;

  bool isConst() const override;

  RJValue getAtomValue(const RapidJsonDocument &json,
                       RJMemoryPoolAlloc &alloc) const override;
  const RJValue *getValue(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) const override;

  bool isAtom() const override;
  std::string getName() const override;
  IValueType getReturnType() const override;

  CREATE_FACTORY(ListAttributesProvider)
 protected:
  bool recursive = false;
  void recursiveAdd(const std::string &prefix, RJValue &arr, const RJValue &obj,
                    RJMemoryPoolAlloc &alloc) const;
};
}  // namespace joda::query

#endif  // JODA_LISTATTRIBUTESPROVIDER_H
