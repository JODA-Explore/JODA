//
// Created by Nico on 28/02/2019.
//

#ifndef JODA_IVALUETESTHELPER_H
#define JODA_IVALUETESTHELPER_H
#include <joda/query/values/AtomProvider.h>
#include <joda/query/values/IValueProvider.h>
#include <joda/query/values/PointerProvider.h>

class IValueTestHelper {
 public:
  template <typename V>
  static void param(std::vector<V> &vec) {}

  template <typename V, typename T1, typename... Types>
  static void param(std::vector<V> &vec, T1 &&t1, Types &&... args) {
    vec.emplace_back(std::forward<T1 &&>(t1));
    param(vec, std::forward<Types &&>(args)...);
  }

  static std::unique_ptr<joda::query::IValueProvider> getNumVal(double d) {
    return std::make_unique<joda::query::DoubleProvider>(d);
  }

  static std::unique_ptr<joda::query::IValueProvider> getNumVal(int64_t i) {
    return std::make_unique<joda::query::Int64Provider>(i);
  }

  static std::unique_ptr<joda::query::IValueProvider> getNumVal(u_int64_t i) {
    return std::make_unique<joda::query::UInt64Provider>(i);
  }
  static std::unique_ptr<joda::query::IValueProvider> getPointer(
      std::string ptr, bool relative = false) {
    return std::make_unique<joda::query::PointerProvider>(ptr, relative);
  }
  static std::unique_ptr<joda::query::IValueProvider> getStringVal(
      std::string str) {
    return std::make_unique<joda::query::StringProvider>(str);
  }
  static std::unique_ptr<joda::query::IValueProvider> getBoolVal(bool b) {
    return std::make_unique<joda::query::BoolProvider>(b);
  }
};
#endif  // JODA_IVALUETESTHELPER_H
