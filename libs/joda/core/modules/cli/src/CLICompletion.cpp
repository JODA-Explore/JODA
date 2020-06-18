//
// Created by Nico Schäfer on 15/08/18.
//

#include "CLICompletion.h"
#include <algorithm>
#include <glog/logging.h>
#include <readline/readline.h>
#include "joda/cli/CLI.h"
#include <regex>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <joda/queryparsing/QueryParser.h>
#include <joda/storage/collection/StorageCollection.h>

size_t joda::cli::CLICompletion::match_index = 0;
std::vector<std::string> joda::cli::CLICompletion::cli_commands = {};
std::vector<std::string> joda::cli::CLICompletion::matches = {};

char **joda::cli::CLICompletion::completer(const char *text, int start, int end) {
  std::string line(rl_line_buffer, 0, start); //Get line until before the currently typed word
  std::string word(text);
  rl_attempted_completion_over = 1;
  //Choose Context

  //Basic Commands Context
  if (start == 0) { //Currently typed word is at start of buffer
    //  DLOG(INFO) << "basic_command_completer";
    return rl_completion_matches(text, CLICompletion::basic_command_completer); //Return all commands
  }
  //Load Variable Context
  std::regex loadVar(R"reg(^\s*LOAD\s*(?!>[[:alpha:]])$)reg");
  if (std::regex_search(line, loadVar)) { //Only "load" before currently typed word
    //   DLOG(INFO) << "storage_completer";
    return rl_completion_matches(text, CLICompletion::storage_completer); //Return storage names
  }

  //Expand Files
  std::regex loadFiles(R"reg(^\s*LOAD\s*(?:[[:alpha:]]*)\s*FROM\s*FILE(?:S?)\s*"(?!.*"))reg");
  if (std::regex_search(line, loadFiles)) {
    rl_attempted_completion_over = 0;
    //  DLOG(INFO) << "file_completer";
    return nullptr;
  }

  //TODO Expand function names

  // DLOG(INFO) << "basic_query_completer";
  //complete query
  return rl_completion_matches(text, CLICompletion::basic_query_completer);
}

char *joda::cli::CLICompletion::basic_command_completer(const char *text, int state) {
  if (state == 0) {
    init_match(cli_commands, text);
  }

  return return_match();
}

char *joda::cli::CLICompletion::basic_query_completer(const char *text, int state) {
  if (state == 0) {
    std::vector<std::string> qCommands;
    std::string line(rl_line_buffer);

    if (line.find("DELETE") == std::string::npos) {
      qCommands.emplace_back("DELETE");
      if (line.find("AGG") == std::string::npos) {
        qCommands.emplace_back("AGG");
        if (line.find("AS") == std::string::npos) {
          qCommands.emplace_back("AS");

          //Also add functions
          auto functions = joda::queryparsing::QueryParser::getFunctionNames();
          std::stringstream ss(functions);
          std::string token;
          while (std::getline(ss, token, ' ')) {
            qCommands.emplace_back(token + "(");
          }

          if (line.find("CHOOSE") == std::string::npos) {
            qCommands.emplace_back("CHOOSE");
//            if(line.find("FROM FILES")==std::string::npos){
//              qCommands.emplace_back("FROM FILES");
//            }
//            if(line.find("FROM FILE")==std::string::npos){
//              qCommands.emplace_back("FROM FILE");
//            }
//            if(line.find("FROM GROUPED")==std::string::npos){
//              qCommands.emplace_back("FROM GROUPED");
//            }
          }
        }
      }
    }

    init_match(qCommands, text);
  }

  return return_match();
}

void joda::cli::CLICompletion::setCli_commands(const std::vector<std::string> &cli_commands) {
  CLICompletion::cli_commands = cli_commands;
  joda::cli::CLICompletion::cli_commands.emplace_back("LOAD");
}

char *joda::cli::CLICompletion::return_match() {
  if (match_index >= matches.size()) {
    // We return nullptr to notify the caller no more matches are available.
    return nullptr;
  } else {
    // Return a malloc'd char* for the match. The caller frees it.
    return strdup(matches[match_index++].c_str());
  }
}

void joda::cli::CLICompletion::init_match(const std::vector<std::string> &cmds, const char *text) {
//Clear matches
  matches.clear();
  match_index = 0;

  //Retrieve matches
  std::string textstr = std::string(text);
  for (auto &word : cmds) {
    if (word.size() >= textstr.size() &&
        word.compare(0, textstr.size(), textstr) == 0) {
      matches.push_back(word);
    }
  }
}

char *joda::cli::CLICompletion::storage_completer(const char *text, int state) {
  if (state == 0) {
    std::vector<std::string> storages;
    for (const auto &storage : StorageCollection::getInstance().getStorages()) {
      storages.emplace_back(storage->getName());
    }
    init_match(storages, text);
  }

  return return_match();
}
