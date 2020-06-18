//
// Created by Nico Schäfer on 06/02/18.
//

#ifndef JODA_CONFIGPARSER_H
#define JODA_CONFIGPARSER_H

#include <boost/program_options.hpp>

namespace po = boost::program_options;

/**
 * A parser for parsing the config files and command line parameters
 */
class ConfigParser {
 public:
  /**
   * Parses the command line parameters and the config file (if available)
   * @param argc argc of main function
   * @param argv argv of main function
   * @return The parsed variables
   */
  static const po::variables_map parseConfigs(int argc, char *argv[]);

  /**
   * Sets the configurations of the program, by providing the previously parsed variables
   * @param vm The variables map to get configs from
   */
  static void setConfig(const po::variables_map& vm);

  /**
   * Prints the help message that appears when providing the "-h", "--help" command line flag or when using a wrong option
   */
  static void produceHelpMessage();

  /**
   * Dumps a visual representation of all configs
   */
  static void dumpConfig();
 protected:
  static const po::options_description getCMDOptions() ;
  static const po::options_description getConfigOptions() ;
  static const po::options_description getHiddenConfigOptions() ;
  static const std::string getConfigFile();
};

#endif //JODA_CONFIGPARSER_H
