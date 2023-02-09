//
// Created by Nico Schäfer on 12/07/18.
//

#ifndef JODA_BASIC_CLI_H
#define JODA_BASIC_CLI_H

#include <string>
#include <vector>
#include <joda/config/config.h>

#include "CLI.h"
#include "joda/misc/Benchmark.h"
#include "joda/query/Query.h"
#include "joda/queryexecution/PipelineQueryPlan.h"

#ifdef JODA_ENABLE_PROFILING
#include "gperftools/profiler.h"
#endif

namespace joda::cli {
/**
 * This class is responsible for the main CLI loop
 */
class BasicCLI {
 public:

  /**
   * Creates a new non-interactive CLI with the given starting queries
   */
  BasicCLI(const std::vector<std::string> &onceQueries);

  /**
   * Starts the CLI with the given queries to be executed.
   * If no queries are given, and interactivity is possible, a new interactive CLI will be started.
   */
  void start();

 protected:
 // List of initially passed queries
 std::vector<std::string> initial_queries;

   /**
   * Checks if the CLI is in interactive mode.
   * If data is piped into JODA or itr is called by a script the CLI will not be interactive.
   * @return True if the CLI is in interactive mode, false otherwise
   */
  bool isInteractive() const ;

  /**
   * Creates and starts a new interactive CLI with more features.
   */
  void startInteractiveCLI() const;

  private:
  Benchmark benchmark;

};
}  // namespace joda::cli

#endif  // JODA_BASIC_CLI_H
