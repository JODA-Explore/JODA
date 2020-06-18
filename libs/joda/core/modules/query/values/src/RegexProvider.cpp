//
// Created by Nico Schäfer on 23/01/18.
//

#include <glog/logging.h>
#include "../include/joda/query/values/RegexProvider.h"

bool joda::query::RegexProvider::isConst() const {
  return false;
}

RJValue joda::query::RegexProvider::getAtomValue(const RapidJsonDocument &json,
                                                 RJMemoryPoolAlloc &alloc) const {
  DCHECK(isAtom()) << "Did not check for atom before calling";
  std::string toCheck;
  RJValue val;
  val.SetBool(
      getParamString(toCheck, params[0], json) &&
          std::regex_search(toCheck, regex)
  );
  return val;
}

const RJValue *joda::query::RegexProvider::getValue(const RapidJsonDocument &json,
                                                    RJMemoryPoolAlloc &alloc) const {
  DCHECK(!isAtom())<< "Did not check for atom before calling";
  return nullptr;
}

std::unique_ptr<joda::query::IValueProvider> joda::query::RegexProvider::duplicate() const {
 return std::make_unique<joda::query::RegexProvider>(duplicateParameters());
}

std::string joda::query::RegexProvider::getName() const {
  return "REGEX";
}

joda::query::IValueType joda::query::RegexProvider::getReturnType() const {
  return IV_Bool;
}

joda::query::RegexProvider::RegexProvider(std::vector<std::unique_ptr<joda::query::IValueProvider>> &&parameters) :
    joda::query::IValueProvider(std::move(parameters)) {
  checkParamSize(2);
  checkParamType(0,IV_String);
  checkParamType(1,IV_String);
  if(!this->params[1]->isConst())
    throw joda::query::WrongParameterException(getName()+": Parameter 1 has to be constant");
  if(params[1]->isConst()){
    //Get Value
    RJMemoryPoolAlloc alloc;
    auto strVal = params[1]->getAtomValue(RapidJsonDocument(),alloc);
    std::string regstr = strVal.GetString();
    try {
      regex = std::regex(regstr, std::regex::optimize);
    }catch(const std::regex_error& e){
      throw joda::query::WrongParameterException(getName()+": The regex supplied in parameter 1 is invalid. Regex error: " + e.what());
    }
  }else{
    CHECK(false) << "Forgot to change logic of RegexExtractProvider";
  }
}

