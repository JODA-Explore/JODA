

#include <glog/logging.h>
#include <joda/cli/BasicCLI.h>
#include <joda/concurrency/ThreadManager.h>
#include <joda/config/ConfigParser.h>
#include <joda/config/config.h>
#include <joda/fs/DirectoryRegister.h>
#include <joda/network/JodaServer.h>
#include <joda/storage/collection/StorageCollection.h>
#include <joda/version.h>
#include <joda/queryparsing/QueryParser.h>
#include <joda/extension/ModuleRegister.h>

#ifdef JODA_ENABLE_PYTHON
#include <Python.h>
#endif

#include <fstream>
#include <iostream>

#ifndef JODA_VERSION_STRING
#define JODA_VERSION_STRING "NOT SET"
#endif

void initGlobalSettings() {
  g_ThreadManagerInstance.setMaxThreads(config::storageRetrievalThreads);
}

int main(int argc, char* argv[]) {
  /*
   * Options
   */

  // Fix order of initialization (and thereby deconstruction)
  [[maybe_unused]] volatile auto& dr = joda::filesystem::DirectoryRegister::getInstance();
  [[maybe_unused]] volatile auto& sc = StorageCollection::getInstance();

  google::InitGoogleLogging(argv[0]);
  // Log segfault & co
  google::InstallFailureSignalHandler();


  boost::program_options::variables_map options;
  try {
    options = ConfigParser::parseConfigs(argc, argv);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    ConfigParser::produceHelpMessage();
    LOG(ERROR) << e.what();
    return -1;
  }
  if (options.count("logtostderr")) {
    FLAGS_alsologtostderr = true;
    FLAGS_colorlogtostderr = true;
  }

  if (options.count("help")) {
    ConfigParser::produceHelpMessage();
    return 0;
  }


  try {
    ConfigParser::setConfig(options);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    ConfigParser::produceHelpMessage();
    LOG(ERROR) << e.what();
    return -1;
  }

  if (options.count("version")) {
    std::cout << "Joda Version " << JODA_VERSION_STRING << " (API v"
              << JODA_API_VERSION_STRING << ")" << std::endl;
    std::cout << "Build revision " << JODA_GIT_REVISION << " ("
              << JODA_BUILD_TIME << ")" << std::endl;
    return 0;
  }

  if (options.count("dump-config")) {
    ConfigParser::dumpConfig();
    return 0;
  }

  if (options.count("dump-functions")) {
    std::cout << joda::queryparsing::QueryParser::getFunctionNames() << std::endl;
    return 0;
  }

  // Load modules
  if(options.count("module")){
    auto modules = options["module"].as<std::vector<std::string>>();
    for (const auto& m : modules){
      LOG(INFO) << "Loading module " << m;
      try{
        joda::extension::ModuleRegister::getInstance().registerModule(m);
      } catch (const std::exception& e){
        LOG(ERROR) << "Error while loading module " << m << ": " << e.what();
      }
    }
  }

  std::vector<std::string> onceQueries;

  if (options.count("queryfile")) {
    std::ifstream infile(options["queryfile"].as<std::string>());
    for (std::string line; getline(infile, line);) {
      if (line.empty() || line.front() == '#' ||
          std::all_of(line.begin(), line.end(), isspace))
        continue;
      onceQueries.emplace_back(line);
    }
    LOG(INFO) << "Loaded " << onceQueries.size() << " queries";
  } else if (options.count("query")) {
    onceQueries = options["query"].as<std::vector<std::string>>();
  }

  bool server = false;
  unsigned int port;
  std::string bind;
  if (options.count("server")) {
    server = true;
    port = options["port"].as<unsigned int>();
    bind = options["bind"].as<std::string>();
  }

  initGlobalSettings();

  if (fs::exists(config::tmpdir.c_str())) {
    try {
      auto count = fs::remove_all(config::tmpdir);
      LOG(INFO) << "Cleaned TMP '" << config::tmpdir.c_str() << "'. " << count
                << " files and directories deleted.";
    } catch (const fs::filesystem_error& e) {
      LOG(ERROR) << "Could not clean TMP dir: " << e.what();
    }
  }

  LOG(INFO) << "Joda Version " << JODA_VERSION_STRING << " (API v"
            << JODA_API_VERSION_STRING << ")";
  LOG(INFO) << "Build revision " << JODA_GIT_REVISION << " (" << JODA_BUILD_TIME
            << ")" << std::endl;


  /*
   * Main Loop
   */

  if (!server) {  // Local Execution Mode
    joda::cli::BasicCLI cli(onceQueries);
    cli.start();
  } else {  // Server mode
    try {
      joda::network::JodaServer serv;
      std::thread t([&]() { serv.start(bind, (int)port); });

      char ch;
      bool exec = true;
      std::cout << "Press q to quit server.\n";
      while (exec) {  // Wait for q key to quit server.
        std::cin >> ch;
        if (ch == 'q') exec = false;
      }
      LOG(INFO) << "Shutting down";
      serv.stop();
      t.join();

    } catch (const std::exception& e) {
      LOG(ERROR) << "Exception while starting server: " << e.what();
    }
  }
  sc.getInstance().clear();

  return 0;
}
