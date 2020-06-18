//
// Created by Nico Schäfer on 09/01/18.
//

#include <glog/logging.h>
#include <fstream>
#include <rapidjson/error/en.h>
#include <joda/document/RapidJsonDocument.h>
#include <joda/document/TemporaryOrigin.h>
#include <joda/config/config.h>
#include "joda/join/JoinParser.h"
#include "joda/fs/DirectoryFileScanner.h"
#include "joda/concurrency/readerwriterqueue.h"

void JoinParser::parse(const FileJoinManager &jm, std::shared_ptr<JSONStorage> &storage) {
  using moodycamel::ConcurrentQueue;
  ConcurrentQueue<std::unique_ptr<JSONContainer>> contQueue;
  ConcurrentQueue<std::string> files;

  //Get files
  const auto& dir = jm.getBaseDir();
  joda::filesystem::DirectoryFileScanner ds;
  auto fileList = ds.getFilesInDir(dir,"");
  DLOG(INFO) << "Joining from " << dir;
  if(fileList.empty()){
    LOG(WARNING)<< "Nothing to join, finishing";
    return;
  }

  std::atomic_long filesSent{0};
  std::atomic_long filesParsed{0};
  std::atomic_long tid{0};
  std::atomic_bool finished_container{false};
  std::atomic_uint sent_container{0};

  for (auto &&item : fileList) {
    files.enqueue(item);
  }
  filesSent.store(fileList.size());

  //Start archiving
  auto archiver = std::thread([&](){
    storage->insertDocuments(contQueue, finished_container, sent_container);
  });

  std::vector<std::future<void>> readers;
  auto parsingThreads =
      std::max((ulong) 1, std::min((ulong) std::thread::hardware_concurrency() / 4, std::min((ulong) 4, fileList.size())));

  for (int i = 0; i < parsingThreads; ++i) {
    readers.push_back(std::move(std::async(
        std::launch::async, [&] {
          auto id = tid.load();
          auto tmpFileName = storage->getRegtmpdir()+"/jp"+std::to_string(id)+".json";
          auto tmpFileId = g_FileNameRepoInstance.addFile(tmpFileName);
          auto cont = std::make_unique<JSONContainer>();
          while(filesParsed.load() < filesSent.load()){
            //Get File
            std::string file;
            if(!files.try_dequeue(file)) continue;
            std::ifstream infile(file);
            if(!infile.is_open()){
              LOG(ERROR) << "Could no open file '"<<file<<"'. Skipping";
              filesParsed.fetch_add(1);
              continue;
            }
            std::string header_1;
            std::string header_2;
            //Get Header I: Type Info
            if(!std::getline(infile,header_1)){
              LOG(ERROR) << "File '"<<file<<"' has no header. Skipping";
              filesParsed.fetch_add(1);
              continue;
            }
            //Get Header II: Variable
            if(!std::getline(infile,header_2)){
              LOG(ERROR) << "File '"<<file<<"' has incorrect header. Skipping";
              filesParsed.fetch_add(1);
              continue;
            }

            RJValue val;
            if(header_1 == "BOOL"){
              if(header_2 == "true") val.SetBool(true);
              else if(header_2 == "false")val.SetBool(false);
              else {
                LOG(ERROR) << "File '"<<file<<"' has incorrect header. Skipping";
                filesParsed.fetch_add(1);
                continue;
              }
            } else if(header_1 == "INT"){
              try{
                long long num = std::atoll(header_2.c_str());
                val.SetInt64(num);
              }catch(const std::exception& e){
                LOG(ERROR) << "File '"<<file<<"' has incorrect header. Skipping";
                filesParsed.fetch_add(1);
                continue;
              }

            }else if(header_1 == "STR"){
              val.SetString(header_2.c_str(),*cont->getAlloc());
            }else{
              LOG(ERROR) << "File '"<<file<<"' has wrong header: '"<<header_1<<"'";
              filesParsed.fetch_add(1);
              continue;
            }


            std::shared_ptr<RJDocument> joinDoc = std::make_shared<RJDocument>(cont->getAlloc());
            joinDoc->SetObject();
            RJValue joinName;
            joinName.SetString(jm.getName().c_str(),*cont->getAlloc());
            joinDoc->AddMember(joinName,val,joinDoc->GetAllocator());
            std::string line;
            RJValue arrVal;
            arrVal.SetArray();
            while (std::getline(infile, line))
            {
              RJDocument tmpDoc(cont->getAlloc());
              tmpDoc.Parse(line.c_str());
              if (tmpDoc.HasParseError()) {
                LOG(ERROR) << std::string(rapidjson::GetParseError_En(tmpDoc.GetParseError())) <<
                          " in File: " << file << ":" << line;

                continue;
              }
              arrVal.PushBack(std::move(tmpDoc),joinDoc->GetAllocator());
            }

            joinDoc->AddMember("joins",std::move(arrVal),joinDoc->GetAllocator());
            cont->insertDoc(storage->getID(),std::move(joinDoc),std::make_unique<TemporaryOrigin>());
            infile.close();
            filesParsed.fetch_add(1);

            //Check and send cont
            if(!cont->hasSpace(0)){
              cont->finalize();
              if(!config::storeJson){
                cont->removeDocuments();
              }
              contQueue.enqueue(std::move(cont)); //Enqueue
              sent_container.fetch_add(1);
              cont = std::make_unique<JSONContainer>(); //& Create new one
            }
          }

          if (cont->size() > 0) {
            cont->finalize();
            if(!config::storeJson){
              cont->removeDocuments();
            }
            contQueue.enqueue(std::move(cont));
            sent_container.fetch_add(1);
          }

        })));
  }

  auto loaded = filesSent.load();
  auto parsed = filesParsed.load();
  while (loaded != parsed) {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(100ms);
    LOG(INFO) << "Parsed/Read: " << std::to_string(parsed) << "/" << std::to_string(loaded)
              << " Reading threads: " << std::to_string(readers.size())  <<
              " Container Queue: " << std::to_string(contQueue.size_approx());

    parsed = filesParsed.load();
  }
  DLOG(INFO) << "Stopping parsing threads";
  for (auto &&parser : readers) {
    parser.get();
  }

  while (contQueue.size_approx() > 0) {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(100ms);
  }
  finished_container.store(true);
  DLOG(INFO) << "Stopping storage thread";
  archiver.join();
  LOG(INFO) << "Parsing stopped, used " << std::to_string(readers.size()) << " threads for parsing.";
}
