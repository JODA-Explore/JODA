//
// Created by Nico on 21/08/2019.
//

#include <joda/document/RapidJsonDocument.h>
#include <joda/query/values/IValueProvider.h>
#include <joda/query/values/PointerProvider.h>
#include <variant>
#include <joda/misc/RJFwd.h>

#ifndef JODA_VALUEACCEPTER_H
#define JODA_VALUEACCEPTER_H

namespace joda::query {

class ValueAccepter {
 public:
  explicit ValueAccepter(const std::unique_ptr<joda::query::IValueProvider> &ival) {
    auto pProvider = dynamic_cast<PointerProvider *>(ival.get());
    if (pProvider != nullptr) {
      pp = pProvider;
    } else {
      def = ival.get();
    }
    DCHECK(pp != nullptr || def != nullptr);
  }

  ValueAccepter() {}

  template<class Handler>
  bool Accept(const RapidJsonDocument &json,
              RJMemoryPoolAlloc &alloc, Handler &h) const {

    if (pp != nullptr) {
      return pp->Accept(json, alloc, h);
    } else {
      return DefaultAccept(json, alloc, h);
    }
  }

  void prepareGenerator(const RapidJsonDocument *json_, RJMemoryPoolAlloc *alloc_) {
    this->json_ = json_;
    this->alloc_ = alloc_;
  }

  template<typename Handler>
  bool operator()(Handler &handler) {
    return Accept(*json_, *alloc_, handler);
  }

  template<class Handler>
  static bool Accept(const std::unique_ptr<IValueProvider> &ival, const RapidJsonDocument &json,
                     RJMemoryPoolAlloc &alloc, Handler &h) {

    if (dynamic_cast<PointerProvider *>(ival.get()) != nullptr) {
      auto *pp = dynamic_cast<PointerProvider *>(ival.get());
      return pp->Accept(json, alloc, h);
    } else {
      return DefaultAccept<Handler>(ival, json, alloc, h);
    }

  }

/**
 * If possible returns a pointer or Virtualobject, if it does not exist, it returns a nullptr_t nulltr.
 * @param json
 * @param alloc
 * @return
 */
  std::variant<const RJValue,
               std::optional<const RJValue *>,
               const VirtualObject *> getPointerIfExists(const RapidJsonDocument &json,
                                                         RJMemoryPoolAlloc &alloc) const {
    if (def != nullptr) {
      if (def->isAtom()) {
        RJValue val = def->getAtomValue(json, alloc);

        return std::move(val);
      } else {
        auto *val = def->getValue(json, alloc);
        if (val == nullptr) return (nullptr_t) nullptr;
        return val;
      }
    }
    if (pp != nullptr) {
      return pp->getPointerIfExists(json, alloc);
    }
    return std::optional<const RJValue *>(nullptr);

  }

 private:
  PointerProvider *pp = nullptr;
  IValueProvider *def = nullptr;
  const RapidJsonDocument *json_ = nullptr;
  RJMemoryPoolAlloc *alloc_ = nullptr;

  template<class Handler>
  static bool DefaultAccept(const std::unique_ptr<IValueProvider> &ival, const RapidJsonDocument &json,
                            RJMemoryPoolAlloc &alloc, Handler &h) {
    if (ival->isAtom()) {
      auto val = ival->getAtomValue(json, alloc);
      return val.Accept(h);
    } else {
      auto *val = ival->getValue(json, alloc);
      return val->Accept(h);
    }
  }

  template<class Handler>
  bool DefaultAccept(const RapidJsonDocument &json,
                     RJMemoryPoolAlloc &alloc, Handler &h) const {
    if (def->isAtom()) {
      auto val = def->getAtomValue(json, alloc);
      return val.Accept(h);
    } else {
      auto *val = def->getValue(json, alloc);
      return val->Accept(h);
    }
  }


  /*
   * Object
   */
 public:
  const RJValue *getPointer(const RapidJsonDocument &json,
                            RJMemoryPoolAlloc &alloc) const {
    if (def != nullptr) return def->getValue(json, alloc);
    if (pp != nullptr && pp->objIsPointerEvaluatable(json)) return pp->getValue(json, alloc);
    return nullptr;
  }

  const VirtualObject *getObjVO(const RapidJsonDocument &json,
                                RJMemoryPoolAlloc &alloc) const {
    if (pp != nullptr) {
      return pp->getVO(json);
    }
    return nullptr;
  }

  /*
   * Materialization
   */

  std::vector<std::string> getMaterializeAttributes() const {
    if (pp != nullptr) {
      return {};
    } else {
      return def->getAttributes();
    }
  }
};

}

#endif //JODA_VALUEACCEPTER_H
