//
// Created by Nico Schäfer on 12/6/17.
//

#ifndef JODA_VALUEPROVIDERPROJECTOR_H
#define JODA_VALUEPROVIDERPROJECTOR_H

#include <joda/query/values/ValueAccepter.h>
#include "IProjector.h"
#include "../../../../../values/include/joda/query/values/IValueProvider.h"

namespace joda::query {
/**
 * Uses a IValueProvider to retrieve a (transformed) value from the source document to store in the destination
 */
class ValueProviderProjector : public joda::query::IProjector {
 public:
  ValueProviderProjector(const std::string &to,
                         std::unique_ptr<IValueProvider> &&valprov);
  std::string getType() override;
  std::string toString() override;
  std::vector<std::string> getMaterializeAttributes() const override;
 protected:
  RJValue getVal(const RapidJsonDocument &json, RJMemoryPoolAlloc &alloc) override;
  std::unique_ptr<IValueProvider> valprov;
  static const std::string type;
  ValueAccepter accepter;
};
}

#endif //JODA_VALUEPROVIDERPROJECTOR_H
