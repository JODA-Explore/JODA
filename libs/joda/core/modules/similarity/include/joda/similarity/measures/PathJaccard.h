//
// Created by Nico Schäfer on 09/07/18.
//

#ifndef JODA_PATHJACCARD_H
#define JODA_PATHJACCARD_H

#include <joda/misc/RJFwd.h>
#include <rapidjson/istreamwrapper.h>
#include <algorithm>
#include <string>
#include <vector>
#include "../../../../src/RJPathsReader.h"
#include "IJSONSimilarityMeasure.h"

class PathJaccard;
template <>
struct similarityRepresentation<PathJaccard> {
  bool is_implemented = true;
  RJPathsReader handler;
  typedef std::vector<std::string> Representation;

  inline const Representation getRepresentation(const RJDocument &lhs) const {
    Representation paths;
    paths.reserve(JODA_DEFAULT_PATH_DEPTH);
    std::string prefix;
    getPaths(lhs, prefix, paths);
    std::sort(paths.begin(), paths.end());
    return paths;
  }

  inline const Representation getRepresentation(const std::string &lhs) {
    handler.clear();
    rapidjson::Reader reader;
    rapidjson::StringStream stream(lhs.c_str());
    reader.Parse(stream, handler);
    auto paths = handler.getPaths();
    std::sort(paths.begin(), paths.end());
    return paths;
  }

  inline const Representation getRepresentation(
      rapidjson::IStreamWrapper &lhs) {
    handler.clear();
    rapidjson::Reader reader;
    reader.Parse(lhs, handler);
    auto paths = handler.getPaths();
    return paths;
  }

  template <class T>
  inline static void getPaths(const T &doc, std::string &prefix,
                              std::vector<std::string> &set) {
    if (doc.IsObject()) {
      for (const auto &m : doc.GetObject()) {
        std::string path = prefix + "/" + m.name.GetString();
        set.emplace_back(path);
        if (m.value.IsObject() || m.value.IsArray()) {
          getPaths(m.value, path, set);
        }
      }
    } else if (doc.IsArray()) {
      int i = 0;
      for (const auto &item : doc.GetArray()) {
        std::string path = prefix + "/" + std::to_string(i);
        i++;
        if (item.IsObject() || item.IsArray()) {
          getPaths(item, path, set);
        }
      }
    }
  }
};

/**
 * Uses the Jaccard coefficient of two sets of all unique JSON pointers(/paths)
 * contained within two documents to calculate their similarity.
 */
class PathJaccard : public IJSONSimilarityMeasure {
 public:
  double measure(const RJDocument &lhs, const RJDocument &rhs) override;
  static double measure(
      const similarityRepresentation<PathJaccard>::Representation &lhs,
      const similarityRepresentation<PathJaccard>::Representation &rhs);
};

#endif  // JODA_PATHJACCARD_H
