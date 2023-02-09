#include "ModulePost.h"
#include <joda/network/JodaServer.h>
#include <joda/storage/collection/StorageCollection.h>
#include <joda/config/config.h>
#include <fstream>
#include <joda/fs/DirectoryRegister.h>
#include <joda/extension/ModuleRegister.h>
#include <algorithm>
#include <string>

void joda::network::apiv2::ModulePost::registerEndpoint(
    const std::string& prefix, httplib::Server& server) {
  auto fullEndpoint = prefix + endpoint;
  server.Post(fullEndpoint.c_str(), [fullEndpoint](const httplib::Request& req,
                                                   httplib::Response& res) {
    try {
      JodaServer::logExecutionTime(fullEndpoint, [&]() { store_module(req, res); });
    } catch (JodaAPIException& e) {
      JodaServer::handleError(e, res);
    }
  });
}

void joda::network::apiv2::ModulePost::store_module(
    const httplib::Request& req, httplib::Response& res) {
  if(!req.has_file("module")){
    throw JodaAPIException("No module file provided in 'module' form field");
  }
  auto module_data = req.get_file_value("module");

  LOG(INFO) << "Receiving module file: " << module_data.filename << " (" << module_data.content.length() << " bytes)";

  std::string filename = module_data.filename;
  // Convert module_data.filename to lowercase
  std::transform(filename.begin(), filename.end(),filename.begin(), ::tolower);
  auto file_path = ensure_directory(filename);


  // Check if file already exists
  if(std::filesystem::exists(file_path)){
    throw JodaAPIException("Module file with same name already exists");
  }

  {// Write file
  std::ofstream ofs(file_path);
  ofs << module_data.content;
  }

  try{
    joda::extension::ModuleRegister::getInstance().registerModule(file_path.string());
  } catch (std::exception& e){
    std::filesystem::remove(file_path);
    throw JodaAPIException("Could not register module: " + std::string(e.what()));
  }

  
  // Response
  RJDocument ret(rapidjson::kObjectType);
  ret.AddMember("status","success",ret.GetAllocator());
  JodaServer::sendResponse(ret, res);
}

std::filesystem::path joda::network::apiv2::ModulePost::ensure_directory(const std::string &name) {
  auto& dr = filesystem::DirectoryRegister::getInstance();
  auto mod_dir = dr.ensureTMPDir("modules");
  if(!mod_dir.has_value()){
    throw JodaAPIException("Could not create module directory");
  }

  auto module_dir = std::filesystem::path(mod_dir.value());
  auto file_name = module_dir.append(name);
  return file_name;
}
