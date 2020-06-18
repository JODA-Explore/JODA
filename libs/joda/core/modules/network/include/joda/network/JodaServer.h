//
// Created by Nico on 22/08/2019.
//

#ifndef JODA_JODASERVER_H
#define JODA_JODASERVER_H

#include <httplib.h>
#include <joda/misc/RJFwd.h>
#include <joda/misc/Timer.h>
#include <glog/logging.h>

namespace joda::network {

/**
 *  Describes an exception in the JODA network API
 */
class JodaAPIException : public std::runtime_error {
 public:
 /**
  * Initializes an exception in the JODA network API with a description
  * @param what the description of the exception
  */ 
  explicit JodaAPIException(const std::string &what) : std::runtime_error("API Error: " + what) {};
};

/**
 * JodaServer is responsible for initializing and starting a HTTP server providing netwok functionality to JODA
 */ 
class JodaServer {
 public:
 /**
  * The prefix is used to specify where the JODA api endpoint should be.
  * "http://<host>:<port><prefix><...>"
  */ 
  static constexpr auto prefix = "/api";

  /**
   * Starts the server synchronously.
   * @param addr the address to bind the server to (e.g.: 0.0.0.0 or 127.0.0.1)
   * @param port the port to bind the server to
   * @return false if server could not be started.
   */
  bool start(const std::string &addr, int port);

  /**
   * Stops the server.
   * The "start" function will return after executing this function.
   */
  void stop();

  /**
   * Helper function for the API to use.
   * It uses the error message contained in a JodaAPIException to send a http response containing the error message.
   * @param e the exception to extract the error message from
   * @param res the response over which the error will be reported to the client
   */
  static void handleError(JodaAPIException &e, httplib::Response &res);

  /**
   * Helper function for the API to use.
   * It serializes a JSON document and sends it over a http response to the client.
   * @param doc the document to send to the client
   * @param res the response over which the document will be sent to the client
   */
  static void sendResponse(RJDocument &doc, httplib::Response &res);

  /**
   * Helper function for the API to use.
   * It executes a function, times it's execution and logs this time.
   * @param endpoint a string describing which API endpoint the function comes from
   * @param f function to execute and time
   */
  template<class F>
  static void logExecutionTime(const std::string &endpoint, F f) {
    Timer timer;
    f();
    timer.stop();
    LOG(INFO) << endpoint << " execution took " << timer.toHumanDuration();
  }

 private:
  httplib::Server server;

  static void favicon(const httplib::Request &req, httplib::Response &res);

};

/**
 *  Describes a missing parameter in a request to a JODA api endpoint
 */
class JodaMissingParameterException : public JodaAPIException {
 public:
  explicit JodaMissingParameterException(const std::string &param) : JodaAPIException(
      "Missing '" + param + "' parameter") {}
};

/**
 *  Describes an invalid parameter in a request to a JODA api endpoint
 */
class JodaInvalidParameterException : public JodaAPIException {
 public:
  JodaInvalidParameterException(const std::string &param, const std::string &what) : JodaAPIException(
      "Invalid '" + param + "' parameter: " + what) {}
};

}

#endif //JODA_JODASERVER_H
