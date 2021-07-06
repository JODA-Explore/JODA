//
// Created by Nico Schäfer on 12/7/17.
//

#ifndef JODA_SEQNUMBERPROVIDER_H
#define JODA_SEQNUMBERPROVIDER_H

#include <atomic>

#include "IValueProvider.h"
namespace joda::query {
/**
 * SEQNUM(), returns a sequentially increasing number for each document.
 */
class SeqNumberProvider : public joda::query::IValueProvider {
 public:
  explicit SeqNumberProvider(
      std::vector<std::unique_ptr<IValueProvider>> &&parameters);
  explicit SeqNumberProvider(std::shared_ptr<std::atomic_ulong> currNum);
  std::unique_ptr<IValueProvider> duplicate() const override;
  bool isConst() const override;

  RJValue getAtomValue(const RapidJsonDocument &json,
                       RJMemoryPoolAlloc &alloc) const override;
  const RJValue *getValue(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) const override;
  std::string getName() const override;
  IValueType getReturnType() const override;

  CREATE_FACTORY(SeqNumberProvider)

 protected:
  std::shared_ptr<std::atomic_ulong> currNum;

 private:
  SeqNumberProvider();
};
}  // namespace joda::query

#endif  // JODA_SEQNUMBERPROVIDER_H
