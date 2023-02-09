
#include "ModuleRequest.h"
#include <joda/network/JodaServer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

void joda::network::apiv2::ModuleRequest::registerEndpoint(
    const std::string& prefix, httplib::Server& server) {
  auto fullEndpoint = prefix + endpoint;
  server.Get(fullEndpoint.c_str(), [fullEndpoint](const httplib::Request& req,
                                                  httplib::Response& res) {
    try {
      JodaServer::logExecutionTime(fullEndpoint,
                                   [&]() { sendModules(req, res); });
    } catch (JodaAPIException& e) {
      JodaServer::handleError(e, res);
    }
  });


}

void joda::network::apiv2::ModuleRequest::sendModules(
    const httplib::Request& /*req*/, httplib::Response& res) {
  res.set_header("charset", "utf-8");
  res.set_chunked_content_provider(
      "application/json", [](uint64_t offset_, httplib::DataSink& out) {
        out.write("[", 1);
        auto modules = extension::ModuleRegister::getInstance().getModules();
        for (size_t i = 0; i < modules.size(); ++i) {
          auto str = moduleToJSON(modules[i]);
          out.write(str.c_str(), str.size());
          if (i != modules.size() - 1) {
            out.write(",", 1);
          }
        }
        out.write("]", 1);
        out.done();
        return true;
      });
}

std::string joda::network::apiv2::ModuleRequest::moduleToJSON(const extension::ModuleSummary &summary) {
  auto doc = RJDocument(rapidjson::kObjectType);


  doc.AddMember("Name", summary.name, doc.GetAllocator());
  doc.AddMember("Path", summary.path, doc.GetAllocator());
  doc.AddMember("Language",summary.language, doc.GetAllocator());
  doc.AddMember("Type", std::string(extension::getModuleTypeString(summary.type)), doc.GetAllocator());


  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);
  return buffer.GetString();
}
