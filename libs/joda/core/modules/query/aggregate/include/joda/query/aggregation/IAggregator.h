//
// Created by Nico Schäfer on 10/6/17.
//

#ifndef JODA_IAGGREGATOR_H
#define JODA_IAGGREGATOR_H
#include <string>
#include <vector>
#include <memory>
#include <rapidjson/fwd.h>
#include <rapidjson/allocators.h>
#include <joda/query/values/IValueProvider.h>
#include "joda/misc/infix_iterator.h"
#include <sstream>
#include <joda/concurrency/Queue.h>

namespace joda::query {
/**
 * IAggregators are used to aggregate values.
 * Multiple of these may be used during execution and will be merged at the end.
 * Every aggregator is only called from one Thread, no syncronization is needed.
 */
class IAggregator {
 public:
  /**
 * Creates a new IAggregator with given parameters
 * @throws WrongParameterException if the parameters do not fit the expected (wrong count or type)
 * @param toPointer Pointer to the destination attribute
 * @param params A vector of IValueProviders
 */
  IAggregator(const std::string& toPointer, std::vector<std::unique_ptr<IValueProvider>>&& params): toPointer(toPointer), params(std::move(params)){
    for (auto &param : this->params) {
      IValueProvider::replaceConstSubexpressions(param);
    }
  }
  IAggregator(IAggregator const &) = delete;
  void operator=(IAggregator const &x) = delete;

  virtual ~IAggregator() = default;


  /**
* Accumulates another value. This function is called for every value once
* @param json The Rapidjsondocument to retrieve values from. (Or an empty RapidJsonDocument for const values)
* @param alloc A MemoryPoolAllocator to create the values with
*/
  virtual void accumulate(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) = 0;

  /*
   * Merge (Merges two instances)
   */
  /**
   * Merges the given instance with another.
   * @param other Other instance to be merged. Has to be of same class!
   */
  virtual void merge(IAggregator* other) = 0;

  /*
   * Terminators (Terminate and collect data. Only called once)
   */
  /**
   * Terminates the aggregation and collects the values
   * @return Collected values (GroupKey,Value)
   */
  virtual RJValue terminate(RJMemoryPoolAlloc &alloc)=0;

  /**
   * Duplicates the Aggregator
   * @return A new Instance of the given Aggregator
   */
  virtual std::unique_ptr<IAggregator> duplicate() const = 0;

  virtual /**
   * Returns the name of the Aggregator
   * @return Name of Aggregator
   */
  const std::string &getDestPointer() const {
    return toPointer;
  }

  virtual /**
   * Returns the name of the Aggregator
   * @return Name of Aggregator
   */
  const std::string getName() const = 0;

  std::string getParameterStringRepresentation() const {
    std::vector<std::string> pars;
    for (auto &&param : params) {
      pars.push_back(param->toString());
    }
    std::stringstream ss;
    std::move(std::begin(pars), std::end(pars),
              infix_ostream_iterator<std::string>(ss, ", "));
    return ss.str();
  }

  /*
   * Returns a string representation of the aggregator
   */
  virtual std::string toString() const {
    return "'" + toPointer + "':" + getName()+"("+getParameterStringRepresentation()+")";
  };

 protected:
  std::string toPointer;
  std::vector<std::unique_ptr<IValueProvider>> params;

  void checkParamSize(unsigned int expected){
    if(params.size() != expected){
      throw WrongParameterCountException(params.size(),expected,getName());
    }
  }
  void checkParamType(unsigned int i, IValueType expected){
    if(!(params[i]->isAny()||params[i]->getReturnType() == expected)){
      throw WrongParameterTypeException(i,expected,getName());
    }
  }

  std::vector<std::unique_ptr<IValueProvider>> duplicateParameters() const {
    std::vector<std::unique_ptr<IValueProvider>> ret;
    for (auto &&param : params) {
      ret.push_back(param->duplicate());
    }
    return ret;
  }

};

//Aggregator Queue
struct JODA_AGGREGATOR_QUEUE {
  typedef std::unique_ptr<IAggregator> payload_t;
  typedef JODA_SHARED_QUEUE<payload_t> queue_t;

  static std::unique_ptr<queue_t> getQueue(){
    return std::make_unique<queue_t>();
  }
  static std::unique_ptr<queue_t> getQueue(size_t minCapacity, size_t maxProducers) {
    return std::make_unique<queue_t>(minCapacity,maxProducers);
  }
};

typedef JODA_AGGREGATOR_QUEUE AggregatorQueue;
}

#endif //JODA_IAGGREGATOR_H
