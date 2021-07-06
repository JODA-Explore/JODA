// INListProvider
// Created by Nico Schäfer on 12/7/17.
//

#ifndef JODA_FILEPOSPROVIDER_H
#define JODA_FILEPOSPROVIDER_H

#include <joda/document/FileOrigin.h>

#include "IValueProvider.h"

namespace joda::query {
/**
 * FILEPOSSTART()/FILEPOSEND() retrieves the file positions from document
 * origins
 */
template <bool start>
class FilePosProvider : public joda::query::IValueProvider {
 public:
  FilePosProvider(std::vector<std::unique_ptr<IValueProvider>> &&parameters)
      : IValueProvider(std::move(parameters)) {
    checkParamSize(0);
  }

  std::unique_ptr<IValueProvider> duplicate() const override {
    return std::make_unique<FilePosProvider<start>>(duplicateParameters());
  }

  bool isConst() const override { return false; }

  RJValue getAtomValue(const RapidJsonDocument &json,
                       RJMemoryPoolAlloc &alloc) const override {
    assert(isAtom() && "Did not check for atom before calling");
    RJValue val;
    auto *tmp = json.getOrigin();
    auto *fileOrig = dynamic_cast<const FileOrigin *>(tmp);
    if (fileOrig != nullptr) {
      if (start)
        val.SetInt64(fileOrig->getStart());
      else
        val.SetInt64(fileOrig->getEnd());
    }
    return val;
  }
  const RJValue *getValue(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) const override {
    assert(!isAtom() && "Did not check for atom before calling");
    return nullptr;
  }

  std::string getName() const override {
    return start ? "FILEPOSSTART" : "FILEPOSEND";
  }

  joda::query::IValueType getReturnType() const override { return IV_Number; }

  inline static std::unique_ptr<IValueProvider> _FACTORY(
      std::vector<std::unique_ptr<IValueProvider>> &&params) {
    return std::make_unique<FilePosProvider<start>>(std::move(params));
  }

 protected:
};
}  // namespace joda::query

#endif  // JODA_FILEPOSPROVIDER_H
