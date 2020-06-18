//
// Created by Nico Schäfer on 12/07/18.
//

#ifndef JODA_CLI_H
#define JODA_CLI_H
#include <string>
#include <vector>
#include "joda/query/Query.h"
#include "joda/misc/Benchmark.h"
#include "joda/queryexecution/QueryPlan.h"
#include <readline/readline.h>
#include <readline/history.h>

#ifdef JODA_ENABLE_PROFILING
#include "gperftools/profiler.h"
#endif

namespace joda::cli {
/**
 * This class is responsible for the main CLI loop
 */
class CLI {
 public:
  CLI();
  /**
   * Starts the CLI and reacts to user-input
   * @param onceQueries Optional queries to be executed once with a following shutdown of the system
   */
  void start(const std::vector<std::string> &onceQueries);

  /**
   * Changes interactivity of the CLI.
   * In interactive mode, the program will use ncurses to show query progress and display result documents.
   * In noninteractive mode only three sample documents will be shown in the CLI.
   *  @param simpleMode If true, the CLI will be interactive, if false not
   */
  void setSimpleMode(bool simpleMode);
 protected:
  bool simpleMode = false;
  void checkTerminal();
  /*
   * String/Command modification
   */
  std::string &ltrim(std::string &str);
  std::string &rtrim(std::string &str);
  void parseCommand();
  void print(const std::string& toprint);

  /*
   * Commands
   */
  std::vector<std::pair<std::string,std::function<void(const std::string&)>>> getCommands();
  void quit();
  void query(std::string& query);
  void toggleCache();
  void help();
  void dumpConfig();
  void toggleCrack();
  void listSources();
  void listResults();
  void unknownCommand();
  void executeQuery(std::shared_ptr<query::Query> &query, bool printResult = true);
  void executeNonInteractiveQuery(std::shared_ptr<query::Query> &query, bool printResult = true);
  void logo();
#ifdef JODA_ENABLE_PROFILING
  void profileStart(const std::string &name);
  void profileStop();
  bool profileRunning = false;
#endif


  /*
   * Benchmarking
   */

  void benchmarkQuery(std::shared_ptr<query::Query> q, const std::string &name);
  void benchmarkQuery(std::vector<std::shared_ptr<query::Query>> qs, const std::string &name);


  /*
   * Class variables
   */
  bool execute = true;
  Benchmark bench;
  std::shared_ptr<JSONStorage> checkResult(unsigned long resultId);
};
}

#endif //JODA_CLI_H
