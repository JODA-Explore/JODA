//
// Created by Nico Schäfer on 12/6/17.
//

#ifndef JODA_FLATTENPROJECTOR_H
#define JODA_FLATTENPROJECTOR_H

#include "ISetProjector.h"
#include "../../../../../values/include/joda/query/values/IValueProvider.h"

namespace joda::query {
/**
 * Transformation flattening an array of values to multiple documents, each containing one of the values
 */
class FlattenProjector : public joda::query::ISetProjector {
 public:
  FlattenProjector(const std::string &to, std::unique_ptr<IValueProvider> &&from);
  void project(const RapidJsonDocument &json, std::vector<std::shared_ptr<RJDocument>> &newDocs) override;
  std::string getType() override;
  static const std::string type;
  std::string toString() override;
 protected:
  std::unique_ptr<IValueProvider> from;

};
}

#endif //JODA_FLATTENPROJECTOR_H
