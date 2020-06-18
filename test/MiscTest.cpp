//
// Created by Nico on 28/11/2018.
//

#include "../libs/joda/core/modules/parser/include/joda/parser/ReaderParserFlags.h"
#include <boost/program_options/variables_map.hpp>
#include "joda/config/ConfigParser.h"
#include <iostream>
#include "joda/config/config.h"
#include "joda/network/legacy/JodaServer.h"

void runTest(int argc, char *argv[]){

//  auto fileQueue = JSONFileReader::RQueueFactory::getQueue();
//  auto textQueue = JSONFileReader::PQueueFactory::getQueue();
//  auto contQueue = JSONTextParser::CQueueFactory::getQueue();
//
//  fileQueue->send(std::string(argv[2]));
//  fileQueue->finishedWriting.store(true);
//
//  auto reader = JSONFileReader(fileQueue,textQueue);
//  auto parser = JSONTextParser(textQueue,contQueue);
//  reader.read();
//
//  parser.parse();
//
//  while(!textQueue->isFinished() || !contQueue->isFinished()){
//    LOG(INFO) << "#Files: " << fileQueue->inQueue.load();
//    LOG(INFO) << "#Texts: " << textQueue->inQueue.load();
//    auto load = contQueue->inQueue.load();
//    LOG(INFO) << "#Conts: " << load;
//    if(load > 0) {
//      auto e = std::unique_ptr<JSONContainer>();
//      contQueue->retrieve(e);
//    }
//
//    using namespace std::chrono_literals;
//    std::this_thread::sleep_for(50ms);
//  }
//  LOG(INFO) << "Everything finished; " ;

}



int main(int argc, char *argv[]) {

  google::InitGoogleLogging(argv[0]);
  boost::program_options::variables_map options;
  try {
    options = ConfigParser::parseConfigs(argc, argv);
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    ConfigParser::produceHelpMessage();
    LOG(ERROR) << e.what();
    return -1;
  }
  if (options.count("logtostderr")) {
    FLAGS_alsologtostderr = true;
  }

  if (options.count("help")) {
    ConfigParser::produceHelpMessage();
    return 0;
  }

  try {
    ConfigParser::setConfig(options);
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    ConfigParser::produceHelpMessage();
    LOG(ERROR) << e.what();
    return -1;
  }

  if (options.count("version")) {
    std::cout << "Joda Version " << JODA_VERSION << " API Version " << JODA_SERVER_API_VER << std::endl;
    return 0;
  }

  if (options.count("dump-config")) {
    ConfigParser::dumpConfig();
    return 0;
  }

  runTest(argc,argv);
}

