//
// Created by Nico Schäfer on 11/9/17.
//

#ifndef JODA_QUERYPARSER_H
#define JODA_QUERYPARSER_H

#include <tao/pegtl/parse_error.hpp>

#include "../../../../query/include/joda/query/Query.h"

namespace joda::queryparsing {
/**
 * The QueryParser is responsible for parsing queries from textual
 * representation into the internal Query class.
 */
class QueryParser {
 public:
  /**
   * Parses the given input string into a Query.
   * If an error occurs, it is logged, stored and a nullpointer is returned.
   * @param str the input textual representation
   * @return A Query if the input was a valid JODA query, nullpointer if not.
   */
  std::shared_ptr<query::Query> parse(const std::string& str);

  /**
   * Parses the given input string into one or multiple Query objects.
   * If an error occurs, it is logged, stored, and a empty list is returned.
   * @param str the input textual representation
   * @return A list of Query pointers if the input was valid, empty list if not.
   */
  std::vector<std::shared_ptr<query::Query>> parseMultiple(const std::string& str);


  /**
   * Returns the current error message, if parsing did not succeed.
   * An empty string is returned of no error happened
   * @return
   */
  const std::string getLastError() const;
  /**
   * Returns the current error message with UNIX terminal color codes embedded,
   * if parsing did not succeed. An empty string is returned of no error
   * happened
   * @return
   */
  const std::string getLastErrorColor() const;

  /**
   * Returns a string containing a space seperated list of functions supported
   * by the system.
   * @return
   */
  static std::string getFunctionNames();

  /**
   * Analyzes the underlying query grammar.
   * Used during testing.
   * @return A error code describing issues with the grammar.
   */
  static const size_t checkLanguage();

 protected:
  std::unique_ptr<tao::pegtl::parse_error> lastError;
};
}  // namespace joda::queryparsing
#endif  // JODA_QUERYPARSER_H
