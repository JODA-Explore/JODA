//
// Created by Nico Schäfer on 12/6/17.
//

#include "../include/joda/query/project/ValueProviderProjector.h"

joda::query::ValueProviderProjector::ValueProviderProjector(
    const std::string& to, std::unique_ptr<IValueProvider>&& valprov)
    : IProjector(to), valprov(std::move(valprov)) {
  IValueProvider::replaceConstSubexpressions(this->valprov);
  auto optimized_val = this->valprov->optimize();
  if (optimized_val != nullptr) {
    this->valprov = std::move(optimized_val);
  }
  accepter = ValueAccepter(this->valprov);
}

std::string joda::query::ValueProviderProjector::getType() { return type; }

RJValue joda::query::ValueProviderProjector::getVal(
    const RapidJsonDocument& json, RJMemoryPoolAlloc& alloc) {
  CHECK(valprov != nullptr);

  if (valprov->isAtom()) {
    return valprov->getAtomValue(json, alloc);
  }
  RJDocument doc(&alloc);

  if (!config::enable_views) {
    auto* ptr = valprov->getValue(json, alloc);
    if (ptr == nullptr) {
      return RJValue();
    }
    RJValue val;
    val.CopyFrom(*ptr, alloc, true);
    return val;
  }
  return std::visit(
      [this, &doc, &json, &alloc](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, const RJValue>) {
          return arg;
        }
        if constexpr (std::is_same_v<T, std::optional<const RJValue*>>) {
          if (!arg.has_value()) {
            accepter.prepareGenerator(&json, &alloc);
            doc.Populate(accepter);
            if (doc.IsObject()) {
              return RJValue(doc.GetObject());
            }
            if (doc.IsArray()) {
              return RJValue(doc.GetArray());
            }
            // Is non existing pointer
            return RJValue();
          }
          auto* p = arg.value();
          if (p == nullptr) {
            return RJValue();
          }
          RJValue val;
          val.CopyFrom(*p, alloc, true);
          return val;

        } else if constexpr (std::is_same_v<T, const VirtualObject*>) {
          if (arg == nullptr) {
            return RJValue();
          }
          return arg->deepCopy(alloc);
        } else {
          return RJValue();
        }
      },
      accepter.getPointerIfExists(json, alloc));
}

const std::string joda::query::ValueProviderProjector::type =
    "ValueProviderProjection";
std::string joda::query::ValueProviderProjector::toString() {
  return IProjector::toString() + valprov->toString();
}

std::vector<std::string>
joda::query::ValueProviderProjector::getMaterializeAttributes() const {
  return accepter.getMaterializeAttributes();
}
