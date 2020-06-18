//
// Created by Nico Schäfer on 09/07/18.
//

#ifndef JODA_IJSONSIMILARITYMEASURE_H
#define JODA_IJSONSIMILARITYMEASURE_H
#include <joda/misc/RJFwd.h>
#include <type_traits>
#include <string>

//
//To add new Similarity measures the following steps have to be done
//1: Create the Class
//2: Create the similarityRepresentation for the class
//3: Add the similarity measure to the config::Sim_Measures enum
//4: In ReaderParser add the measure to the Constructor
//5: In the responsible Parsers, add the template instantiation
//This can most probably be simplified by c++ magic
//



/**
 * A similarity measure used to compare two JSON documents with each other
 */
class IJSONSimilarityMeasure {
 public:
  /**
   * Measures the similarity between two RJDocuments.
   * @param lhs First document to compare
   * @param rhs Second document to compare
   * @return A measure for similarity between [0,1]
   */
  virtual double measure(const RJDocument &lhs, const RJDocument &rhs) = 0;
};



template<typename T>
struct similarityRepresentation {
  static_assert(std::is_base_of<IJSONSimilarityMeasure, T>::value, "T must derive from IJSONSimilarityMeasure");
  typedef void* Representation;
  /**
   * Returns the internal representation needed to calculate a similarity measure with T
   * @param lhs The document to transform into the representation
   * @return The representation of Document
   */
  Representation getRepresentation(const RJDocument &lhs) {
    return nullptr;
  }

  /**
   * Returns the internal representation needed to calculate a similarity measure with T
   * @param lhs The string representation of the document
   * @return  The representation of Document
   */
  Representation getRepresentation(const std::string& lhs){
    return nullptr;
  }

  bool is_implemented = false;
};

#endif //JODA_IJSONSIMILARITYMEASURE_H