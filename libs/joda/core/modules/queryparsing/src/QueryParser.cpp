//
// Created by Nico Schäfer on 11/9/17.
//

#include "../include/joda/queryparsing/QueryParser.h"

#include <glog/logging.h>
#include <joda/queryparsing/QueryParser.h>

#include <iostream>
#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/analyze.hpp>
#include "Control.h"
#include "FunctionWrapper.h"
#include "actions/Actions.h"

std::shared_ptr<joda::query::Query> joda::queryparsing::QueryParser::parse(
    const std::string& str) {
  std::string tmp;
  tao::pegtl::memory_input<> in(str, str);
  grammar::queryState qs;
  try {
     // Check for correct result
    if (tao::pegtl::parse<grammar::query, grammar::queryAction,
                          grammar::query_control>(in, qs)) {
      // Optimize
      auto new_pred = qs.q->getChoose()->optimize();
      if(new_pred != nullptr) {
        qs.q->setChoose(std::move(new_pred));
      }
      return qs.q;
    }
  } catch (tao::pegtl::parse_error& e) {
    lastError = std::make_unique<tao::pegtl::parse_error>(e);
    LOG(ERROR) << "Error parsing query: " << e.what();
  }

  return nullptr;
}

std::vector<std::shared_ptr<joda::query::Query>> joda::queryparsing::QueryParser::parseMultiple(const std::string& str){
  std::string tmp;
  tao::pegtl::memory_input<> in(str, str);
  grammar::queriesState qs;

  std::vector<std::shared_ptr<joda::query::Query>> queries;
  try {
     // Check for correct result
    if (tao::pegtl::parse<grammar::queries, grammar::queriesAction,
                          grammar::query_control>(in, qs)) {
      // Optimize
      for (auto& q : qs.q) {
        auto new_pred = q->getChoose()->optimize();
        if(new_pred != nullptr) {
          q->setChoose(std::move(new_pred));
        }
      }
      return qs.q;
    }
  } catch (tao::pegtl::parse_error& e) {
    lastError = std::make_unique<tao::pegtl::parse_error>(e);
    LOG(ERROR) << "Error parsing query: " << e.what();
  }

  return queries;
}

const std::string joda::queryparsing::QueryParser::getLastError() const {
  if (lastError == nullptr) return "";
  return lastError->what();
}

const std::string joda::queryparsing::QueryParser::getLastErrorColor() const {
  if (lastError == nullptr) return "";
  std::stringstream ss;
  ss << "Error parsing query: ";
  size_t i = 0;
  std::string what = lastError->what();
  while (i < lastError->positions().front().byte) {
    ss << what[i];
    i++;
  }

  ss << "\033[1;31m";  // Bold Red

  ss << what[i];
  i++;

  ss << "\033[0m";  // Reset

  while (i < what.size()) {
    ss << what[i];
    i++;
  }

  return ss.str();
}

std::string joda::queryparsing::QueryParser::getFunctionNames() {
  return grammar::functionKWs::toString();
}

const size_t joda::queryparsing::QueryParser::checkLanguage() {
  return tao::pegtl::analyze<grammar::query>();
}
