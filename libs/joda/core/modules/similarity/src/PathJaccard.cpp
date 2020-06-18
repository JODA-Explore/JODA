//
// Created by Nico Schäfer on 09/07/18.
//



#include <iostream>
#include "../include/joda/similarity/measures/PathJaccard.h"

double PathJaccard::measure(const RJDocument &lhs, const RJDocument &rhs) {
  similarityRepresentation<PathJaccard> rep;
  auto lhsPaths = rep.getRepresentation(lhs);
  auto rhsPaths = rep.getRepresentation(rhs);

  return PathJaccard::measure(lhsPaths,rhsPaths);
}


double PathJaccard::measure(const similarityRepresentation<PathJaccard>::Representation &lhs,
                            const similarityRepresentation<PathJaccard>::Representation &rhs) {
  auto lhsSize = lhs.size();
  auto rhsSize = rhs.size();
  if(lhsSize == 0 && rhsSize == 0) return 1;
  else if(lhsSize == 0 || rhsSize == 0) return 0;

  std::vector<std::string> intersect;
  intersect.reserve(std::min(lhs.size(),rhs.size()));
  std::set_intersection(lhs.begin(),lhs.end(),rhs.begin(),rhs.end(),std::back_inserter(intersect));

  //Return Jaccard
  return ((double)intersect.size())/(lhsSize+rhsSize-intersect.size());
}
