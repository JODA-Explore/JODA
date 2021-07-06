//
// Created by nico on 19/12/17.
//

#include <glog/logging.h>
#include <joda/JodaClient.h>
#include <limits.h>
#include <boost/asio/io_service.hpp>
#include <boost/program_options.hpp>
#include <iostream>

namespace po = boost::program_options;
#ifndef JODA_CLIENT_VERSION
#define JODA_CLIENT_VERSION "NOT SET"
#endif

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);
  // Log segfault & co
  google::InstallFailureSignalHandler();

  po::options_description options("Joda Client - JSON Analysis On Demand");
  options.add_options()("help,h", "Show help")("version,v", "Show version")(
      "logtostderr", "Logs to terminal instead of file")(
      "query,q", po::value<std::string>(),
      "Executes query in non-interactive mode.")(
      "offset,o", po::value<unsigned long>()->default_value(0),
      "(Only with --query) Offset of the result. (default: 0)")(
      "count,c", po::value<unsigned long>()->default_value(ULONG_MAX),
      "(Only with --query) Amount of document to get (default: all).")(
      "address,a", po::value<std::string>()->default_value("localhost"),
      "Address to connect to")("port,p",
                               po::value<unsigned int>()->default_value(5632),
                               "Port to connect to");

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, options), vm);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    std::cout << options << std::endl;
    LOG(ERROR) << e.what();
    return 1;
  }

  if (vm.count("help")) {
    std::cout << options << std::endl;
    return 0;
  }

  if (vm.count("version")) {
    std::cout << "Joda Client Version " << JODA_CLIENT_VERSION
              << " API Version " << joda::network::client::JodaClient::VERSION
              << std::endl;
    return 0;
  }

  auto address = vm["address"].as<std::string>();
  auto port = vm["port"].as<unsigned int>();

  bool interactive = true;
  unsigned long offset = 0;
  unsigned long count = UINT_MAX;
  std::string query;

  if (vm.count("query")) {
    interactive = false;
    query = vm["query"].as<std::string>();
    offset = vm["offset"].as<unsigned long>();
    count = vm["count"].as<unsigned long>();
  }

  try {
    joda::network::client::JodaClient client(address, port);
    if (interactive) {
      LOG(INFO) << "Starting CLI";
      client.cli();
    } else {
      LOG(INFO) << "Starting non.interactive execution";
      client.nonInteractiveQuery(query, offset, count);
    }
  } catch (const std::exception& e) {
    LOG(ERROR) << std::string("Error during communication: ") << e.what();
  }

  return 0;
}