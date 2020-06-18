//
// Created by Nico Schäfer on 11/9/17.
//

#include "../include/joda/queryparsing/QueryParser.h"

#include <glog/logging.h>
#include <joda/query/predicate/StaticEvalVisitor.h>
#include <joda/queryparsing/QueryParser.h>

#include <iostream>
#include <tao/pegtl.hpp>
#include <tao/pegtl/analyze.hpp>
#include <tao/pegtl/contrib/tracer.hpp>
#include "FunctionWrapper.h"
#include "actions/Actions.h"
#include "Control.h"

std::shared_ptr<joda::query::Query>  joda::queryparsing::QueryParser::parse(const std::string &str) {
  std::string tmp;
  tao::pegtl::memory_input<> in(str, str);
  grammar::queryState qs;
  try {
    if (tao::pegtl::parse<grammar::query, grammar::queryAction, grammar::query_control>(in, qs)) {
      // Check for correct result
      query::StaticEvalVisitor sev;
      qs.q->getPredicate()->accept(sev);
      qs.q->setPredicate(sev.getPred());
      return qs.q;
    }
  } catch (tao::pegtl::parse_error &e) {
    lastError = std::make_unique<tao::pegtl::parse_error>(e);
    LOG(ERROR) << "Error parsing query: " << e.what();
  }

  return nullptr;
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
  while (i < lastError->positions.front().byte) {
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

std::string joda::queryparsing::QueryParser::getFunctionNames() { return grammar::functionKWs::toString(); }

const size_t joda::queryparsing::QueryParser::checkLanguage() {
  return tao::pegtl::analyze<grammar::query>();
}
