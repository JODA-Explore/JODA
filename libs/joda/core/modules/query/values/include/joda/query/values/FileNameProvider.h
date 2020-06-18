//
// Created by Nico Schäfer on 12/7/17.
//

#ifndef JODA_FILENAMEPROVIDER_H
#define JODA_FILENAMEPROVIDER_H

#include "IValueProvider.h"

namespace joda::query {
/**
 * FILENAME() retrieves the filename from the document origin
 */
class FileNameProvider : public joda::query::IValueProvider {
 public:
  explicit FileNameProvider(
      std::vector<std::unique_ptr<IValueProvider>> &&parameters);
  std::unique_ptr<IValueProvider> duplicate() const override;
  bool isConst() const override;
  RJValue getAtomValue(const RapidJsonDocument &json,
                       RJMemoryPoolAlloc &alloc) const override;
  const RJValue *getValue(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) const override;

  std::string getName() const override;
  IValueType getReturnType() const override;

  CREATE_FACTORY(FileNameProvider)
};
}

#endif  // JODA_FILENAMEPROVIDER_H
