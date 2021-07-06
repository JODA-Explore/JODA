//
// Created by Nico Schäfer on 15/08/18.
//

#ifndef JODA_CLICOMLETION_H
#define JODA_CLICOMLETION_H
#include <string>
#include <vector>

namespace joda::cli {
class CLICompletion {
 public:
  /**
   * Provides command completion to the CLI
   * @param text Buffer of the current line
   * @param start start position in the buffer
   * @param end end position of the buffer
   * @return List of possible completions
   */
  static char **completer(const char *text, int start, int end);

  static void setCli_commands(const std::vector<std::string> &cli_commands);

 private:
  static std::vector<std::string> cli_commands;
  static std::vector<std::string> matches;
  static size_t match_index;

  /**
   * Generates command completions
   * @param text Buffer of current line
   * @param state State of the completion (0 at start)
   * @return One completion or nullpointer if no more found
   */
  static char *basic_command_completer(const char *text, int state);
  /**
   * Generates query completion
   * @param text Buffer of current line
   * @param state State of the completion (0 at start)
   * @return One completion or nullpointer if no more found
   */
  static char *basic_query_completer(const char *text, int state);

  static char *storage_completer(const char *text, int state);

  static char *query_function_completer(const char *text, int state);

  static char *return_match();

  static void init_match(const std::vector<std::string> &cmds,
                         const char *text);
};
}  // namespace joda::cli

#endif  // JODA_CLICOMLETION_H
