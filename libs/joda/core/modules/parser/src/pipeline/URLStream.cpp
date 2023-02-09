#include <joda/document/URLOrigin.h>
#include <joda/parser/pipeline/URLStream.h>

#ifdef JODA_ENABLE_FROMURL
#include <cpprest/http_client.h>
#include <cpprest/interopstream.h>
#include <cpprest/uri.h>
#endif

template <bool Lineseparated>
void joda::queryexecution::pipeline::tasks::load::URLStreamExec<Lineseparated>::
    fillBuffer(std::optional<Output>& buff,
               std::function<void(std::optional<Output>&)> sendPartial) {
  if (url.empty()) return;
#ifdef JODA_ENABLE_FROMURL
  try {
    // Validate URL
    if (!web::uri::validate(url)) {
      LOG(ERROR) << "URL invalid: " + url;
      url.clear();
      return;
    }
    // Get fileid
    auto urlID = g_FileNameRepoInstance.addFile(url);

    Output out;
    out.second = sample;
    out.first.streamOrigin = std::make_unique<URLOrigin>(urlID);

    // Parse URI
    web::uri uri(url);
    url.clear();

    // Perform async request
    auto client = web::http::client::http_client(uri);
    auto resp = client.request(web::http::methods::GET);
    auto stream = std::make_unique<Concurrency::streams::async_istream<char>>(
        resp.get().body().streambuf());
    if (stream->fail()) {
      LOG(ERROR) << "Error while sending request: " << stream->exceptions();
      return;
    }
    if (*stream) {
      out.first.stream = std::move(stream);
    }
    buff = std::move(out);
  } catch (std::exception& e) {
    LOG(ERROR) << "Exception: " << e.what() << std::endl;
  }

#else
  LOG(ERROR) << "Compiled without FROM URL support, but invoked JSONURLReader";
#endif
}

template <bool Lineseparated>
bool joda::queryexecution::pipeline::tasks::load::URLStreamExec<
    Lineseparated>::finished() const {
  return url.empty();
}

template <bool Lineseparated>
joda::queryexecution::pipeline::tasks::load::URLStreamExec<
    Lineseparated>::URLStreamExec(const std::string& url, const double sample)
    : url(url), sample(sample) {}

template class joda::queryexecution::pipeline::tasks::load::URLStreamExec<true>;
template class joda::queryexecution::pipeline::tasks::load::URLStreamExec<
    false>;