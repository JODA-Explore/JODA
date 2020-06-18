//
// Created by Nico on 16/05/2019.
//

#ifndef JODA_ATTRIBUTEJACCARD_H
#define JODA_ATTRIBUTEJACCARD_H

#include "IJSONSimilarityMeasure.h"
#include "../../../../src/RJAttributeReader.h"
#include <joda/misc/RJFwd.h>
#include <rapidjson/istreamwrapper.h>

class AttributeJaccard;

template<>
struct similarityRepresentation<AttributeJaccard> {
  bool is_implemented = true;
  RJAttributeReader handler;
  typedef std::set<std::string> Representation;

  inline const Representation getRepresentation(const RJDocument &lhs) const {
    Representation paths;
    std::string prefix;
    getAttributes(lhs, prefix, paths);
    return paths;
  }

  inline const Representation getRepresentation(const std::string &lhs) {
    handler.clear();
    rapidjson::Reader reader;
    rapidjson::StringStream stream(lhs.c_str());
    reader.Parse(stream, handler);
    auto paths = handler.getAttributes();
    return paths;
  }

  inline const Representation getRepresentation(rapidjson::IStreamWrapper &lhs) {
    handler.clear();
    rapidjson::Reader reader;
    reader.Parse(lhs, handler);
    auto paths = handler.getAttributes();
    return paths;
  }

  inline static void getAttributes(const RJValue &doc, std::string &prefix, Representation &set) {
    if (doc.IsObject()) {
      for (const auto &m : doc.GetObject()) {
        set.insert(m.name.GetString());
        if (m.value.IsObject() || m.value.IsArray()) {
          getAttributes(m.value, prefix, set);
        }
      }
    } else if (doc.IsArray()) {
      for (const auto &item : doc.GetArray()) {
        if (item.IsObject() || item.IsArray()) {
          getAttributes(item, prefix, set);
        }
      }
    }
  }

};

/**
 * Uses the Jaccard coefficient of two sets of all unique attribute names contained within two documents to calculate their similarity. 
 */
class AttributeJaccard : public IJSONSimilarityMeasure {
 public:
  double measure(const RJDocument &lhs, const RJDocument &rhs) override;
  static double measure(const similarityRepresentation<AttributeJaccard>::Representation &lhs,
                        const similarityRepresentation<AttributeJaccard>::Representation &rhs);

};

#endif //JODA_ATTRIBUTEJACCARD_H
