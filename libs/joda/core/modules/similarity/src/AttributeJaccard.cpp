//
// Created by Nico on 16/05/2019.
//

#include <rapidjson/document.h>
#include <algorithm>
#include "joda/similarity/measures/AttributeJaccard.h"

double AttributeJaccard::measure(const RJDocument &lhs, const RJDocument &rhs) {
  similarityRepresentation<AttributeJaccard> rep;
  auto lhsPaths = rep.getRepresentation(lhs);
  auto rhsPaths = rep.getRepresentation(rhs);

  return AttributeJaccard::measure(lhsPaths, rhsPaths);
}

double AttributeJaccard::measure(const similarityRepresentation<AttributeJaccard>::Representation &lhs,
                                 const similarityRepresentation<AttributeJaccard>::Representation &rhs) {
  auto lhsSize = lhs.size();
  auto rhsSize = rhs.size();
  if (lhsSize == 0 && rhsSize == 0) return 1;
  else if (lhsSize == 0 || rhsSize == 0) return 0;

  std::vector<std::string> intersect;
  intersect.reserve(std::min(lhs.size(), rhs.size()));
  std::set_intersection(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), std::back_inserter(intersect));

  //Return Jaccard
  return ((double) intersect.size()) / (lhsSize + rhsSize - intersect.size());
}