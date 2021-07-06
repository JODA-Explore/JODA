//
// Created by Nico Schäfer on 12/07/18.
//

#include "joda/cli/CLI.h"

#include <curses.h>
#include <joda/config/ConfigParser.h>
#include <joda/misc/MemoryUtility.h>
#include <joda/misc/ProgressBar.h>
#include <joda/query/predicate/ToStringVisitor.h>

#include <fstream>
#include <iostream>

#include "CLICompletion.h"
#include "ResultInterface.h"
#include "joda/misc/Timer.h"
#include "joda/queryparsing/QueryParser.h"
#include "joda/storage/collection/StorageCollection.h"

namespace joda {
namespace cli_funcs {
int eoq = 0;
int escape = 0;
int quote;

int cmd_startup();
int bind_cr(int /*count*/, int /*key*/);

int cmd_startup() {
  eoq = 0;
  rl_bind_key('\n', bind_cr);
  rl_bind_key('\r', bind_cr);
  return 1;
}

int bind_cr(int /*count*/, int /*key*/) {
  int i = rl_end - 1;
  auto string = rl_copy_text(0, rl_end);
  std::string tmpline(string, static_cast<unsigned long>(rl_end));
  free(string);
  while (i >= 0) {
    DCHECK(i < static_cast<int>(tmpline.size()))
        << "i is initialized incorrectly";
    if (std::isspace(tmpline[i]) != 0) {
      i--;
      continue;
    }
    if (tmpline[i] == ';') {
      printf("\n");
      rl_done = 1;
      return 1;
    } else {
      break;
    }
  }
  rl_insert_text("\n");
  return 1;
}

}  // namespace cli_funcs
}  // namespace joda

std::string& joda::cli::CLI::ltrim(std::string& str) {
  auto it2 = std::find_if(str.begin(), str.end(), [](char ch) {
    return !std::isspace<char>(ch, std::locale::classic());
  });
  str.erase(str.begin(), it2);
  return str;
}

std::string& joda::cli::CLI::rtrim(std::string& str) {
  auto it1 = std::find_if(str.rbegin(), str.rend(), [](char ch) {
    return !std::isspace<char>(ch, std::locale::classic());
  });
  str.erase(it1.base(), str.end());
  return str;
}

void joda::cli::CLI::parseCommand() {
  char* line;

  if ((line = readline("")) == nullptr) {
    DCHECK(false) << "Error while reading from readline? Buffer too small?";
    unknownCommand();
  }
  std::string strline(line);
  std::replace(strline.begin(), strline.end(), '\n', ' ');
  std::copy(strline.begin(), strline.end(), line);
  add_history(line);
  rl_free(line);

  auto command = ltrim(rtrim(strline));
  if (!command.empty()) {
    command = command.substr(0, command.size() - 1);
  }

  for (auto& cmd : getCommands()) {
    // Command found
    if (command.compare(0, cmd.first.length(), cmd.first) == 0) {
      auto arg = command.substr(cmd.first.length(), command.size());
      cmd.second.operator()(arg);
      return;
    }
  }
  if (command.compare(0, 4, "LOAD") == 0) {
    query(command);
  } else {
    unknownCommand();
  }
}

void joda::cli::CLI::start(const std::vector<std::string>& onceQueries) {
  size_t query_i = 0;
  if (isatty(STDIN_FILENO) != 0) {
    using_history();
    rl_readline_name = "JODA";
    if (config::persistent_history) {
      if (config::history_file.empty()) {
        LOG(WARNING)
            << "Persistent history enabled, but no history file provided.";
      } else {
        DLOG(INFO) << "Reading history file \"" << config::history_file << "\"";
        auto err = read_history(config::history_file.c_str());
        if (err != 0) {
          if (err == 2) {
            DLOG(INFO)
                << "History file does not exist. Will be created on exit.";
          } else {
            LOG(ERROR) << "Error " << err << " while reading history file.";
          }
        } else {
          if (config::history_size > 0) {
            stifle_history(config::history_size);
          }
        }
      }
    }
    rl_startup_hook = joda::cli_funcs::cmd_startup;
    rl_attempted_completion_function = CLICompletion::completer;
    std::vector<std::string> cmds;
    for (const auto& command : getCommands()) {
      cmds.emplace_back(command.first + ";");
    }
    CLICompletion::setCli_commands(cmds);
  }
  while (execute) {
    std::shared_ptr<query::Query> query;
    if (onceQueries.empty()) {
      parseCommand();
    } else {
      auto onceQuery = onceQueries[query_i];
      joda::queryparsing::QueryParser qp;
      query = qp.parse(onceQuery);
      query_i++;
      auto lastQuery = query_i == onceQueries.size();
      if (lastQuery) {
        executeQuery(query, lastQuery);
      } else {
        executeNonInteractiveQuery(query, lastQuery);
      }
      if (lastQuery) {
        execute = false;
      }
    }
  }

  if (isatty(STDIN_FILENO) != 0) {
    DLOG(INFO) << "Writing history file";
    auto err = write_history(config::history_file.c_str());
    if (err != 0) {
      LOG(ERROR) << "Error " << err << " while writing history file.";
    }
  }
}

void joda::cli::CLI::print(const std::string& toprint) {
  std::cout << toprint << std::endl;
}

void joda::cli::CLI::quit() {
  execute = false;
  print("Quitting");
}

void joda::cli::CLI::toggleCache() {
  config::queryCache = !config::queryCache;
  print(std::string() + "Cache is now turned " +
        (config::queryCache ? "on" : "off"));
}

void joda::cli::CLI::listSources() {
  std::string tmp;
  tmp = "----Data Sources----\n";
  for (auto& storage : StorageCollection::getInstance().getStorages()) {
    tmp +=
        storage->getName() + " (" + std::to_string(storage->size()) + ", " +
        MemoryUtility::MemorySize(storage->estimatedSize()).getHumanReadable() +
        ")\n";
  }
  tmp += "--------------------";
  print(tmp);
}

void joda::cli::CLI::listResults() {
  std::string tmp;
  tmp = "----Results----\n";
  for (auto& storage :
       StorageCollection::getInstance().getTemporaryStorages()) {
    tmp +=
        storage->getName() + " (" + std::to_string(storage->size()) + ", " +
        MemoryUtility::MemorySize(storage->estimatedSize()).getHumanReadable() +
        ")\n";
  }
  tmp += "---------------";
  print(tmp);
}

void joda::cli::CLI::query(std::string& query) {
  joda::queryparsing::QueryParser qp;
  auto q = qp.parse(query);
  if (q == nullptr) {
    std::cerr << qp.getLastErrorColor() << "\n";
  } else {
    executeQuery(q);
  }
}
void joda::cli::CLI::executeNonInteractiveQuery(
    std::shared_ptr<query::Query>& query, bool printResult) {
  if (query != nullptr) {
    auto queryName = query->toString();
    bench.addValue("Query", queryName);
    query::ToStringVisitor stringify;

    try {
      Timer timer;
      QueryPlan plan(query);
      auto resultId = plan.executeQuery(&bench);

      auto result = checkResult(resultId);

      LOG(INFO) << MemoryUtility::getRamUsage();

      if (result != nullptr && printResult) {
        print("-----RESULTS------");
        auto tmp = result->stringify(0, 2);
        for (auto&& line : tmp) {
          print(line);
        }
        print("-------END--------");
      }

      StorageCollection::getInstance().removeStorage(resultId);
      timer.stop();
      timer.log("Query", "MAIN");
      bench.addValue(Benchmark::RUNTIME, "Query", timer.durationSeconds());
      bench.finishLine();
      if (config::benchmark) {
        print("-----STATISTICS------");
        print(bench.lastLineToString());
        print("----------------");
      }

    } catch (const std::exception& e) {
      std::cout << e.what() << std::endl;
    }
  }
}

std::shared_ptr<JSONStorage> joda::cli::CLI::checkResult(
    unsigned long resultId) {
  if (resultId >= JODA_STORE_VALID_ID_START) {
    auto result = StorageCollection::getInstance().getStorage(resultId);
    if (result == nullptr) {
      std::cerr
          << "Could not execute query, check log for additional information"
          << std::endl;
    } else {
      print("ResultSize: " + std::__cxx11::to_string(result->size()) + "\n");
      return result;
    }
  } else {
    if (resultId == JODA_STORE_EXTERNAL_RS_ID) {
      std::cout << "Successfully exported data." << std::endl;
    } else if (resultId == JODA_STORE_SKIPPED_QUERY_ID) {
      std::cout << "Skipped query because of an error, see log for more info."
                << std::endl;
    } else if (resultId == JODA_STORE_EMPTY_RS_ID) {
      std::cout << "Input set and result set are empty" << std::endl;
    } else {
      std::cerr << "Got invalid result set" << std::endl;
    }
  }
  return nullptr;
}

void joda::cli::CLI::executeQuery(std::shared_ptr<query::Query>& query,
                                  bool printResult) {
  if (simpleMode) {
    return executeNonInteractiveQuery(query, printResult);
  }
  if (query != nullptr) {
    auto queryName = query->toString();
    bench.addValue("Query", queryName);
    query::ToStringVisitor stringify;
    try {
      Timer timer;
      QueryPlan plan(query);
      std::chrono::steady_clock::time_point start =
          std::chrono::steady_clock::now();
      auto resultTaks = std::async(std::launch::async,
                                   [&]() { return plan.executeQuery(&bench); });
      bool parsing = plan.hasToParse();
      ResultInterface interface;
      std::unique_ptr<ProgressBar<size_t>> pb;
      std::string status1;
      std::string status2;

      if (parsing) {
        status2 = "Waiting for parser";
      }
      interface.setStatus2(status2);
      while (resultTaks.wait_for(std::chrono::milliseconds(100)) !=
             std::future_status::ready) {
        auto stats = plan.getStats();
        std::chrono::steady_clock::time_point now =
            std::chrono::steady_clock::now();
        auto time_elapsed =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - start)
                .count();
        double seconds = static_cast<double>(time_elapsed) / 1000.0;
        double parsedDocs_s = static_cast<double>(stats.parsedDocs) / seconds;
        double parsedConts_s = static_cast<double>(stats.parsedConts) / seconds;
        if (pb == nullptr && stats.numConts > 0) {
          pb = std::make_unique<ProgressBar<size_t>>(stats.numConts, 30);
        }
        if (pb != nullptr) {
          pb->set_progress(stats.evaluatedConts);
        }
        if (parsing) {
          status1 = "Parsed Docs/Conts: " + std::to_string(stats.parsedDocs) +
                    "(" + std::to_string(static_cast<size_t>(parsedDocs_s)) +
                    "/s)/" + std::to_string(stats.parsedConts) + "(" +
                    std::to_string(static_cast<size_t>(parsedConts_s)) + "/s)";
        }
        interface.setStatus1(status1);
        if (pb != nullptr) {
          auto str = pb->toString();
          interface.setStatus2(str);
        }
      }
      auto stats = plan.getStats();
      std::chrono::steady_clock::time_point now =
          std::chrono::steady_clock::now();
      auto time_elapsed =
          std::chrono::duration_cast<std::chrono::milliseconds>(now - start)
              .count();
      double seconds = static_cast<double>(time_elapsed) / 1000.0;
      double parsedDocs_s = static_cast<double>(stats.parsedDocs) / seconds;
      double parsedConts_s = static_cast<double>(stats.parsedConts) / seconds;
      if (parsing) {
        status1 = "Parsed Docs/Conts: " + std::to_string(stats.parsedDocs) +
                  "(" + std::to_string(static_cast<size_t>(parsedDocs_s)) +
                  "/s)/" + std::to_string(stats.parsedConts) + "(" +
                  std::to_string(static_cast<size_t>(parsedConts_s)) + "/s)";
      }
      interface.setStatus1(status1);
      if (pb != nullptr) {
        pb->set_progress(stats.evaluatedConts);
        auto str = pb->toString();
        interface.setStatus2(str);
      } else {
        std::string str;
        interface.setStatus2(str);
      }

      auto resultId = resultTaks.get();

      auto result = checkResult(resultId);

      if (result != nullptr && printResult) {
        interface.updateStorage(result);
        interface.interact();
      }

      StorageCollection::getInstance().removeStorage(resultId);
      timer.stop();
      timer.log("Query", "MAIN");
      bench.addValue(Benchmark::RUNTIME, "Query", timer.durationSeconds());
      bench.finishLine();
    } catch (const std::exception& e) {
      std::cout << e.what() << std::endl;
    }
  }
}

joda::cli::CLI::CLI() : bench(config::benchfile) { checkTerminal(); }

void joda::cli::CLI::benchmarkQuery(const std::shared_ptr<query::Query>& q,
                                    const std::string& /*name*/) {
  QueryPlan plan(q);
  StorageCollection::getInstance().removeStorage(plan.executeQuery(&bench));
  bench.finishLine();
  print("-----STATISTICS------");
  print(bench.lastLineToString());
  print("----------------");
}

void joda::cli::CLI::benchmarkQuery(
    std::vector<std::shared_ptr<query::Query>> qs, const std::string& name) {
  auto index = 1;
  for (auto&& q : qs) {
    bench.addValue("Index", index);
    benchmarkQuery(q, name);
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s);
    index++;
  }
}

void joda::cli::CLI::unknownCommand() { print("Unknown command"); }

void joda::cli::CLI::logo() {
  auto str = R"del(
                 *(#####(*/*/////**/(/*%&&%%%      #/**///////*//(&     &%&&&%*///**/////*( /###############((/%&                        (######%
                 **(((((/,*&%%%((///////////////////////////////////////////////////(#%%%%/ *((((((((((((((((((((/(%&                   (/((((((/#
                 **(((((/*    &&&&%(/////////////////////////////////////////////#%&&&%.    *(((((((((((((((((((((((//%                 *(((((((/*
                 **(((((/*      ,&&&&&%(/////////////////////////////////////#%&&&&&%       *((((((((((((((((((((((((((/%              /((((((((((*
                 **(((((/*        &&&&&&&%#(/////////,         .//////////#&&&&&&&%/        *((((((*          //((((((((/#             /((((((((((/
                 **(((((/*         (%&&&&&&%///////,              *///////%&&&&&&,          *((((((*             *(((((((/           (*((((((((((((/
                 **(((((/*           /&&&&%(//////*                *//////(&&&%%            *((((((*              *(((((((/          */((((((((((((//
                 **(((((/*               ,///////*                  ///////,                *((((((*               //(((((*/        /((((((( ,((((((**
                 **(((((/*                *//////,        by        *//////,                *((((((*               //(((((*,        /((((((/  /((((((/
                 **(((((/*                *//////,   Nico Schäfer   *//////.                *((((((*               //(((((/,       *((((((/#&&/(((((((/
                 **(((((/*                *///////                 (///////                 *((((((*               //(((((*.      (/((((((((((((((((((//
                 **(((((/*                **/////**                *//////*                 *((((((*              %/((((((,      /*((((((((((((((((((((**
      %/*        **(((((/,                 *////////              *///////.                 *((((((*             %/((((((/       /((((((((((((((((((((((/
    &(/((//      //(((((/.                  *///////,%          /*///////*                  *((((((*           &%/(((((((,      /(((((((         ,(((((((/
  **((((((((/////(((((((,                    /////////////////*/////////*                   *((((((/*///////(//(((((((((.      (/((((((*          /((((((//
    /((((((((((((((((((/                      ////////////////////////*                     *(((((((((((((((((((((((((/       (*((((((/            /((((((*
      //((((((((((((((*                         .///////////////////,                       *((((((((((((((((((((((//         /((((((/             *(((((((*
        *//((((((((/.                              .//////////////                          *((((((((((((((((((//*           /(((((((               /((((((//
)del";
  print(str);
}

std::vector<std::pair<std::string, std::function<void(const std::string&)>>>
joda::cli::CLI::getCommands() {
  return {
      {"quit", std::bind(&CLI::quit, this)},
      {"logo", std::bind(&CLI::logo, this)},
      {"cache", std::bind(&CLI::toggleCache, this)},
      {"sources", std::bind(&CLI::listSources, this)},
      {"results", std::bind(&joda::cli::CLI::listResults, this)},
      {"help", std::bind(&CLI::help, this)},
      {"dump config", std::bind(&joda::cli::CLI::dumpConfig, this)},
#ifdef JODA_ENABLE_PROFILING
      {"profile stop", std::bind(&CLI::profileStop, this)},
      {"profile start",
       std::bind(&CLI::profileStart, this, std::placeholders::_1)},
#endif
  };
}

void joda::cli::CLI::help() {
  std::cout << "---------------------- JODA ----------------------\n";
  std::cout << "quit        - Quits the program \n";
  std::cout << "help        - Shows this message \n";
  std::cout << "results     - Shows a list of available results \n";
  std::cout << "sources     - Shows a list of data sources \n";
  std::cout << "cache       - Enables cache index \n";
  std::cout << "dump config - Shows all currently set settings \n";
  std::cout << "LOAD ...    - Executes a query \n";
#ifdef JODA_ENABLE_PROFILING
  std::cout << "profile start <name>  - Starts profiling with name \n";
  std::cout << "profile stop          - Stops profiling \n";
#endif
  std::cout << "--------------------------------------------------\n";
  std::cout << std::flush;
}

void joda::cli::CLI::dumpConfig() {
  std::cout << "---------------------- Config --------------------\n";
  ConfigParser::dumpConfig();
  std::cout << "--------------------------------------------------\n";
  std::cout << std::flush;
}

void joda::cli::CLI::setSimpleMode(bool simpleMode) {
  if (simpleMode) {
    LOG(INFO)
        << "Using non interactive query processing due to terminal support.";
  }
  CLI::simpleMode = simpleMode;
}

void joda::cli::CLI::checkTerminal() {
  SCREEN* terminal = newterm(nullptr, nullptr, nullptr);
  if (terminal == nullptr) {
    setSimpleMode(true);
  } else {
    if (config::disable_interactive_CLI) {
      setSimpleMode(true);
    }
    endwin();
  }
}

#ifdef JODA_ENABLE_PROFILING

void joda::cli::CLI::profileStart(const std::string& name) {
  if (profileRunning)
    std::cerr << "Profiler already running \n";
  else {
    auto n = name;
    if (!n.empty() && n.back() == ';') n.pop_back();
    ltrim(rtrim(n));
    if (n.empty()) {
      std::cerr << "No profile name given, using \"joda_profile\" \n";
      n = "joda_profile";
    }
    std::cout << "Starting profiling with profile name \"" << n << "\" \n";
    ProfilerStart(n.c_str());
    profileRunning = true;
  }
}

void joda::cli::CLI::profileStop() {
  if (!profileRunning)
    std::cerr << "No profiler running \n";
  else {
    std::cout << "Stopping profiling\n";
    ProfilerStop();
    profileRunning = false;
  }
}

#endif
