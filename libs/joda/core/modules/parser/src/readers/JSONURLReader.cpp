//
// Created by Nico on 27/11/2018.
//

#include "JSONURLReader.h"
#include <fcntl.h>
#include <joda/document/FileOrigin.h>
#include <joda/document/URLOrigin.h>
#include <joda/misc/FileNameRepo.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <iostream>
#include "joda/config/config.h"

#ifdef JODA_ENABLE_FROMURL
#include <cpprest/http_client.h>
#include <cpprest/interopstream.h>
#include <cpprest/uri.h>
#endif

joda::docparsing::JSONURLReader::JSONURLReader(IQueue* iqueue, OQueue* oqueue,
                                               void* /*ptr*/)
    : IWorkerThread(iqueue, oqueue, nullptr) {
  oqueue->registerProducer();
  DLOG(INFO) << "Started JSONURLReader";
#ifndef JODA_ENABLE_FROMURL
  LOG(ERROR) << "Compiled without FROM URL support, but invoked JSONURLReader";
#endif
}

joda::docparsing::JSONURLReader::~JSONURLReader() {
  oqueue->unregisterProducer();
  DLOG(INFO) << "Stopped JSONURLReader";
}

void joda::docparsing::JSONURLReader::work() {
  auto tok = OQueue::ptok_t(oqueue->queue);

  while (shouldRun) {
    if (!iqueue->isFinished()) {
      URLPayload sfile;
      iqueue->retrieve(sfile);
      auto& url = sfile.url;

      // Check for empty filename
      if (url.empty()) {
        LOG(WARNING) << "Empty URL was received by reader";
        continue;
      }

#ifdef JODA_ENABLE_FROMURL
      // Get fileid
      auto urlID = g_FileNameRepoInstance.addFile(url);

      try {
        OPayload out;
        out.first = std::make_unique<URLOrigin>(urlID);

        if (!web::uri::validate(url)) {
          LOG(ERROR) << "URL invalid: " + url;
          continue;
        }
        web::uri uri(url);

        auto client = web::http::client::http_client(uri);
        auto resp = client.request(web::http::methods::GET);
        auto stream =
            std::make_unique<Concurrency::streams::async_istream<char>>(
                resp.get().body().streambuf());

        /*
std::ostringstream os;
os << stream->rdbuf();
auto tmp = os.str();
 */

        if (stream->fail()) {
          LOG(ERROR) << "Error while sending request: " << stream->exceptions();
          continue;
        }

        if (*stream) {
          out.second = std::move(stream);
          oqueue->send(tok, std::move(out));
        }

      } catch (std::exception& e) {
        LOG(ERROR) << "Exception: " << e.what() << std::endl;
      }
#endif

    } else {
      shouldRun = false;
    }
  }
  oqueue->producerFinished();
}
