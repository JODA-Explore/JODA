//
// Created by Nico Schäfer on 06/02/18.
//

#include "joda/config/ConfigParser.h"
#include <glog/logging.h>
#include <joda/misc/MemoryUtility.h>
#include <experimental/filesystem>
#include <iostream>
#include <thread>
#include "joda/config/config.h"

namespace fs = std::experimental::filesystem;

const po::options_description ConfigParser::getCMDOptions() {
  po::options_description options("Commandline");
  options.add_options()("help,h", "Show help")("version,v", "Show version")(
      "dump-config", "Sumps current config")(
      "query,q", po::value<std::vector<std::string>>(),
      "Query to execute and then quit")(
      "queryfile,f", po::value<std::string>(),
      "Query file, containing lineseparated queries to execute and then quit")(
      "benchmark,b", "Runs the benchmark")("profile",
                                           "Shows statistics during execution")(
      "server", "Starts the program in server mode.");
  return options;
}

const po::options_description ConfigParser::getConfigOptions() {
  po::options_description options("Configuration");
  options.add_options()
      // Directories
      ("data_dir,d", po::value<std::string>()->default_value("."),
       "Data dictionary for benchmarks (Default: Current directory)")(
          "tmpdir", po::value<std::string>(),
          "Directory for temporary files. (Default: OS chooses appropriate)")
      // Storage
      ("nostorage,s", "Do not keep JSON files in memory")
      // Benchmark
      ("benchfile", po::value<std::string>(), "Sets benchmark file")
      // Multithreading
      ("maxthreads,t",
       po::value<unsigned int>()->default_value(
           std::max(1u, std::thread::hardware_concurrency() - 1)),
       "Maximum threads to use")("readthreads,r",
                                 po::value<unsigned int>()->default_value(1),
                                 "Threads to use for reading files")(
          "parsethreads", po::value<unsigned int>(),
          "Threads to use for parsing files")
      // Network
      ("port,p", po::value<unsigned int>()->default_value(5632),
       "Port for server mode")(
          "bind", po::value<std::string>()->default_value("localhost"),
          "Binding for server mode")
      // Misc
      ("timing,c", "Times the execution")("logtostderr",
                                          "Logs to terminal instead of file")(
          "noninteractive", "Disables the interactive CLI")

      ;

  return options;
}

const po::options_description ConfigParser::getHiddenConfigOptions() {
  // Get home path
  auto env = getenv("HOME");
  if (env == nullptr) {
    LOG(WARNING) << "Cannot get home directory";
  }
  bool persistent = false;
  fs::path home;
  if (env != nullptr) {
    home = fs::path(env);
    home += "/.joda_history";
    persistent = true;
    DLOG(INFO) << "History file default will be " << home;
  }

  po::options_description options("Hidden");
  options.add_options()
      // Indices
      ("indices.caching.enable", po::value<bool>()->default_value(true),
       "Enables caching")
      // Bloom
      ("indices.bloom.enable", po::value<bool>()->default_value(false),
       "Enables bloom")(
          "indices.bloom.approx_count",
          po::value<unsigned int>()->default_value(1000),
          "How many attributes all documents of a container roughly have")(
          "indices.bloom.fp_prob", po::value<double>()->default_value(0.001),
          "Maximum tolerable false positive probability")
      // Storage
      ("storage.eviction_strategy",
       po::value<config::EvictionStrategies>()->default_value(
           config::NO_EVICTION),
       "Eviction strategy to use (NO_EVICTION, LARGEST, LRU, EXPLORER)")(
          "storage.container.max_size",
          po::value<unsigned int>()->default_value(0),
          "Maximum size of containers (in byte). If 0, then size will be "
          "picked automatically depending on the data")(
          "storage.container.chunk_size",
          po::value<double>()->default_value(0.25),
          "Chunk size as fraction of container-size.")(
          "storage.container.text_binary_mod",
          po::value<double>()->default_value(1.75),
          "If container max_size is set to auto (0), the file sizes times this "
          "modifier.")("storage.max_memory",
                       po::value<unsigned long long>()->default_value(
                           MemoryUtility::totalRam().getBytes()),
                       "Maximum amount of memory used by the system")
      // Parsing
      ("parsing.read.bulk_size", po::value<unsigned int>()->default_value(50),
       "# of lines to read/send in bulk")(
          "parsing.read.reader",
          po::value<std::string>()->default_value("MMAP"),
          "Reader to choose per default (MMAP/FSTREAM)")(
          "parsing.parse.bulk_size",
          po::value<unsigned int>()->default_value(50),
          "# of lines to parse/send in bulk")
      // Similarity
      //("sim.min_cont_size",po::value<size_t>()->default_value(100),"Minimum
      // container size (smaller will be merged)")
      ("sim.min_similarity", po::value<double>()->default_value(0.6),
       "Minimum similarity required to group documents")(
          "sim.measure",
          po::value<config::Sim_Measures>()->default_value(
              config::NO_SIMILARITY),
          "Similarity measure to use (NO_SIMILARITY, PATH_JACCARD)")
      // ("sim.merge_on_parse",po::value<bool>()->default_value(false), "Enables
      // merging of similar containers") Delta Trees
      ("delta_tree.enable",
       po::value<bool>()->default_value(true)->implicit_value(true),
       "Enable delta tree containers. Reduces memory usage and may increase "
       "performance for some query loads.")(
          "delta_tree.vo_enable",
          po::value<bool>()->default_value(true)->implicit_value(false),
          "Enable virtual object improvement. Uses more memory, but improves "
          "multiple queries on one delta tree.")
      // History
      ("history.file", po::value<std::string>()->default_value(home),
       "Path to history file")("history.size",
                               po::value<int>()->default_value(0),
                               "Max size of history. 0 for unlimited")(
          "history.persistent", po::value<bool>()->default_value(persistent),
          "Keep persistent history file?");

  return options;
}

const std::string ConfigParser::getConfigFile() {
  std::vector<std::string> possibleLocations;
  possibleLocations.emplace_back("/etc/joda/joda.conf");
  possibleLocations.emplace_back("./joda.conf");
  std::string conf;

  // Check all possible locations starting and return first found
  for (auto&& location : possibleLocations) {
    try {
      const fs::path p = fs::path(location.c_str());
      if (fs::exists(p)) {
        conf = p.string();
        break;
      }
    } catch (const std::exception& e) {
      LOG(ERROR) << "Error while checking for config locations: " << e.what();
    }
  }
  LOG_IF(INFO, conf.empty()) << "No config file found, using defaults";
  return conf;
}

const po::variables_map ConfigParser::parseConfigs(int argc, char** argv) {
  auto generic = getCMDOptions();
  auto config = getConfigOptions();
  auto hidden = getHiddenConfigOptions();

  po::options_description cmdline_options;
  cmdline_options.add(generic).add(config).add(hidden);

  po::options_description config_file_options;
  config_file_options.add(config).add(hidden);

  po::options_description visible("Allowed options");
  visible.add(generic).add(config);

  std::string configFile = getConfigFile();

  // Parse
  po::variables_map vm;
  if (!configFile.empty()) {
    LOG(INFO) << "Parsing config file " << configFile;
    po::store(po::parse_config_file<char>(configFile.c_str(),
                                          config_file_options, false),
              vm);
  }
  po::store(po::parse_command_line(argc, argv, cmdline_options), vm);
  po::notify(vm);
  return vm;
}

void ConfigParser::setConfig(const po::variables_map& vm) {
  LOG(INFO) << "Setting configs";
  // Indices
  config::queryCache = vm["indices.caching.enable"].as<bool>();

  // Bloom
  config::bloom_enabled = vm["indices.bloom.enable"].as<bool>();
  config::bloom_count = vm["indices.bloom.approx_count"].as<unsigned int>();
  config::bloom_prob = vm["indices.bloom.fp_prob"].as<double>();

  // Directories
  config::home = vm["data_dir"].as<std::string>();
  if (vm.count("tmpdir") != 0u) {
    config::tmpdir = vm["tmpdir"].as<std::string>();
  } else {
    config::tmpdir = std::string(fs::temp_directory_path().c_str()) + "/JODA";
  }

  // Storage
  config::storeJson = vm.count("nostorage") == 0;
  config::maxmemory = vm["storage.max_memory"].as<unsigned long long>();
  config::evictionStrategy =
      vm["storage.eviction_strategy"].as<config::EvictionStrategies>();

  // Benchmark
  if (vm.count("benchfile") != 0u) {
    config::benchmark = true;
    config::benchfile = vm["benchfile"].as<std::string>();
    ;
  }
  if (vm.count("timing") != 0u) {
    config::benchmark = true;
  }

  // Containers
  config::JSONContainerSize =
      vm["storage.container.max_size"].as<unsigned int>();
  config::chunk_size = vm["storage.container.chunk_size"].as<double>();
  config::text_binary_mod =
      vm["storage.container.text_binary_mod"].as<double>();

  // Multithreading
  config::storageRetrievalThreads = vm["maxthreads"].as<unsigned int>();
  config::readingThreads = vm["readthreads"].as<unsigned int>();
  if (vm.count("parsethreads") != 0u) {
    config::parsingThreads = vm["parsethreads"].as<unsigned int>();
  } else {
    config::parsingThreads = std::max(
        1u, static_cast<unsigned int>(((std::thread::hardware_concurrency()) -
                                       config::readingThreads) /
                                      config::readingThreads));
  }

  // Parsing
  config::read_bulk_size = vm["parsing.read.bulk_size"].as<unsigned int>();
  config::read_reader = vm["parsing.read.reader"].as<std::string>();
  config::parse_bulk_size = vm["parsing.parse.bulk_size"].as<unsigned int>();

  // Similarity
  config::sim_measure = vm["sim.measure"].as<config::Sim_Measures>();
  // config::sim_min_cont_size = vm["sim.min_cont_size"].as<size_t>();
  config::sim_min_similarity = vm["sim.min_similarity"].as<double>();
  // config::sim_merge_on_parse = vm["sim.merge_on_parse"].as<bool>();

  // History
  config::history_file = vm["history.file"].as<std::string>();
  if (vm.count("history.persistent") != 0u) {
    config::persistent_history = true;
  }
  config::history_size = vm["history.size"].as<int>();

  // CLI
  if (vm.count("noninteractive") != 0u) {
    config::disable_interactive_CLI = true;
  }

  // Views
  config::enable_views = vm.count("delta_tree.enable") != 0u
                             ? vm["delta_tree.enable"].as<bool>()
                             : false;
  config::enable_views_vo = vm.count("delta_tree.vo_enable") != 0u
                                ? vm["delta_tree.vo_enable"].as<bool>()
                                : false;
}

void ConfigParser::produceHelpMessage() {
  auto generic = getCMDOptions();
  auto config = getConfigOptions();
  po::options_description visible("Joda - JSON Analysis On Demand");
  visible.add(generic).add(config);
  std::cout << visible << std::endl;
}

void ConfigParser::dumpConfig() {
  // Indices
  std::cout << "indices.caching.enable:" << config::queryCache << std::endl;

  // Bloom
  std::cout << "indices.bloom.enable:" << config::bloom_enabled << std::endl;
  std::cout << "indices.bloom.approx_count:" << config::bloom_count
            << std::endl;
  std::cout << "indices.bloom.fp_prob:" << config::bloom_prob << std::endl;

  // Directories
  std::cout << "data_dir:" << config::home << std::endl;
  std::cout << "tmpdir:" << config::tmpdir << std::endl;

  // Storage
  std::cout << "nostorage:" << !config::storeJson << std::endl;

  // Benchmark
  std::cout << "benchfile:" << config::benchfile << std::endl;

  // Containers
  std::cout << "storage.eviction_strategy:" << config::evictionStrategy
            << std::endl;
  std::cout << "storage.max_memory:" << config::maxmemory << std::endl;
  std::cout << "storage.container.max_size:" << config::JSONContainerSize
            << std::endl;
  std::cout << "storage.container.chunk_size:" << config::chunk_size
            << std::endl;

  // Multithreading
  std::cout << "maxthreads:" << config::storageRetrievalThreads << std::endl;
  std::cout << "readthreads:" << config::readingThreads << std::endl;

  // Parsing
  std::cout << "parsing.read.bulk_size:" << config::read_bulk_size << std::endl;
  std::cout << "parsing.read.reader:" << config::read_reader << std::endl;
  std::cout << "parsing.parse.bulk_size:" << config::parse_bulk_size
            << std::endl;

  // Similarity
  std::cout << "sim.measure:" << config::sim_measure << std::endl;
  // std::cout << "sim.min_cont_size:"<< config::sim_min_cont_size <<std::endl;
  std::cout << "sim.min_similarity:" << config::sim_min_similarity << std::endl;
  // std::cout << "sim.merge_on_parse:"<< config::sim_merge_on_parse
  // <<std::endl;

  // History
  std::cout << "history.file:" << config::history_file << std::endl;
  std::cout << "history.persistent:" << config::persistent_history << std::endl;
  std::cout << "history.size:" << config::history_size << std::endl;

  // Delta trees
  std::cout << "delta_tree.enable:" << config::enable_views << std::endl;
  std::cout << "delta_tree.vo_enable:" << config::enable_views_vo << std::endl;
}
