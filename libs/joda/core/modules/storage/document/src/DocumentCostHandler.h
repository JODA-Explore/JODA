//
// Created by Nico on 08/11/2019.
//

#ifndef JODA_DOCUMENTCOSTHANDLER_H
#define JODA_DOCUMENTCOSTHANDLER_H
#include <glob.h>
#include <cstdint>
#include <joda/misc/RJFwd.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/encodings.h>
#include <vector>

class DocumentCostHandler {
  typedef rapidjson::UTF8<>::Ch Ch;
 public:
  DocumentCostHandler() {}

  bool Default() {
    return true;
  };

  bool Null() {
    theoreticalCost += sizeof(RJValue);
    return true;
  };

  bool Bool(bool b) {
    theoreticalCost += sizeof(RJValue);
    return true;
  };

  bool Int(int i) {
    theoreticalCost += sizeof(RJValue);
    return true;
  };

  bool Uint(unsigned int i) {
    theoreticalCost += sizeof(RJValue);
    return true;
  };

  bool Int64(int64_t int64) {
    theoreticalCost += sizeof(RJValue);
    return true;
  };

  bool Uint64(uint64_t uint64) {
    theoreticalCost += sizeof(RJValue);
    return true;
  };

  bool Double(double d) {
    theoreticalCost += sizeof(RJValue);
    return true;
  };

  bool RawNumber(const Ch *str, rapidjson::SizeType len, bool copy) {
    return String(str, len, copy);
  };

  bool String(const Ch *ch, rapidjson::SizeType len, bool copy) {
    theoreticalCost += sizeof(RJValue);
    if (copy && !canBeShortString(len))
      theoreticalCost += sizeof(RJValue::Ch) * len;
    return true;
  };

  bool StartObject() {
    return true;
  };

  bool Key(const Ch *str, rapidjson::SizeType len, bool copy) {
    return String(str, len, copy);
  };

  bool EndObject(rapidjson::SizeType len) {
    theoreticalCost += sizeof(RJValue);

    return true;
  };

  bool StartArray() {
    return true;
  };

  bool EndArray(rapidjson::SizeType len) {
    theoreticalCost += sizeof(RJValue);
    return true;
  };

  void checkDocument(const RJDocument &doc) {
    doc.Accept(*this);
    cost += sizeof(RJDocument) - sizeof(RJValue);
    checkValue(doc);
  }

  void checkValue(const RJValue &doc) {
    switch (doc.GetType()) {
      case rapidjson::kObjectType: {
        cost += sizeof(RJValue::Member) * (doc.MemberCapacity() - doc.MemberCount());
        for (const auto &item : doc.GetObject()) {
          checkValue(item.name);
          checkValue(item.value);
        }
      }
        break;
      case rapidjson::kArrayType: {
        cost += sizeof(RJValue) * (doc.Capacity() - doc.Size());
        for (const auto &item : doc.GetArray()) {
          checkValue(item);
        }
      }
        break;
      case rapidjson::kStringType: {
        cost += sizeof(RJValue);
        auto len = doc.GetStringLength();
        if (!isShortString(doc)) {
          cost += sizeof(RJValue::Ch) * len;
        }
      }
        break;
      default:cost += sizeof(RJValue);
    }
  }

  void addViewPaths(const std::vector<std::string> &paths) {
    cost += sizeof(std::vector<std::string>) + paths.size();
    theoreticalCost += sizeof(std::vector<std::string>) + paths.size();
    for (const auto &path : paths) {
      cost += path.size() * sizeof(char);
      theoreticalCost += path.size() * sizeof(char);
    }
  }

  size_t getCost() const {
    return cost;
  }

  size_t getTheoreticalCost() const {
    return theoreticalCost;
  }

 private:
  size_t cost = 0;
  size_t theoreticalCost = 0;

   // Checks against the rules in https://rapidjson.org/structrapidjson_1_1_generic_value_1_1_short_string.html
  bool canBeShortString(size_t len) const {
    if (RAPIDJSON_64BIT == 0 || RAPIDJSON_48BITPOINTER_OPTIMIZATION == 1) return len <= 13;
    return len <= 21;
  }

  bool isShortString(const RJValue &doc) const {
    return canBeShortString(doc.GetStringLength());
  }
};

#endif //JODA_DOCUMENTCOSTHANDLER_H
