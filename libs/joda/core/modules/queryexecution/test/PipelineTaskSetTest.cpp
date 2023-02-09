#include <gtest/gtest.h>
#include "../src/pipeline/Pipeline.h"
#include <joda/pipelineatomics/PipelineIOTask.h>
#include <joda/pipelineatomics/queue/DefaultPipelineQueues.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <joda/parser/pipeline/ListFile.h>
#include <joda/parser/pipeline/ListDir.h>
#include <joda/parser/pipeline/LineSeparatedStreamReader.h>
#include <joda/parser/pipeline/TextParser.h>
#include <joda/parser/pipeline/FileOpener.h>
#include "../src/pipeline/tasks/storage/StorageBuffer.h"
#include <joda/storage/collection/StorageCollection.h>

namespace pl = joda::queryexecution::pipeline;
namespace tasks = pl::tasks;
namespace load = tasks::load;

class PipelineTaskSetTest : public ::testing::Test {
  public:
    std::shared_ptr<pl::ThreadPool> pool = std::make_shared<pl::ThreadPool>(4);

    void TearDown() override {
      StorageCollection::getInstance().clear();
    }
};

template<class T, pl::queue::QueueSizeTrait Q = pl::queue::LargeQueueTrait>
pl::PipelineIOQueue<T, Q::BulkSize> createQueue(size_t minCapacity = 10){
  return pl::PipelineIOQueue<T,Q::BulkSize>(0, minCapacity);
}

std::string stringifyJSON(const RJDocument& doc) {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  doc.Accept(writer);
  return buffer.GetString();
}

TEST_F(PipelineTaskSetTest, ListToStoreLineSeparated) {
  pl::Pipeline p(pool);

  auto path = "./test/data/json/numtest.json";
  pl::tasks::PipelineTaskPtr list = std::make_unique<tasks::PipelineIOTask<load::LSListFile>>(path);
  p.addTask(std::move(list));

  pl::tasks::PipelineTaskPtr open = std::make_unique<tasks::PipelineIOTask<load::LSFileOpener>>();
  p.addTask(std::move(open));

  auto read = std::make_unique<tasks::PipelineIOTask<load::LineSeparatedStreamReader>>();
  p.addTask(std::move(read));

  auto parse = std::make_unique<tasks::PipelineIOTask<load::DefaultTextParser>>(10000);
  p.addTask(std::move(parse));


 
  auto store = std::make_unique<tasks::PipelineIOTask<tasks::storage::StorageBuffer>>("A");
  auto* outptr = store.get();
  p.addTask(std::move(store));

  auto outQueue = createQueue<tasks::storage::StorageBuffer::Output>(100);
  outptr->setOutputQueue(&outQueue);

  
  p.execute();

  auto pipelinerepr = p.toJSON();
  auto pipelinestr = stringifyJSON(pipelinerepr);
  auto s = StorageCollection::getInstance().getStorage("A");
  EXPECT_NE(s, nullptr);
  EXPECT_EQ(s->size(),5);
}

TEST_F(PipelineTaskSetTest, ListToStoreStream) {
  pl::Pipeline p(pool);

  auto path = "./test/data/json/formattest.json";
  pl::tasks::PipelineTaskPtr list = std::make_unique<tasks::PipelineIOTask<load::ListFile>>(path);
  p.addTask(std::move(list));

  pl::tasks::PipelineTaskPtr open = std::make_unique<tasks::PipelineIOTask<load::UnformattedFileOpener>>();
  p.addTask(std::move(open));

  auto read = std::make_unique<tasks::PipelineIOTask<load::DefaultStreamParser>>(10000);
  p.addTask(std::move(read));

  auto store = std::make_unique<tasks::PipelineIOTask<tasks::storage::StorageBuffer>>("B");
  auto* outptr = store.get();
  p.addTask(std::move(store));

  auto outQueue = createQueue<tasks::storage::StorageBuffer::Output>(100);
  outptr->setOutputQueue(&outQueue);

  
  p.execute();

  auto pipelinerepr = p.toJSON();
  auto pipelinestr = stringifyJSON(pipelinerepr);
  auto s = StorageCollection::getInstance().getStorage("B");
  EXPECT_NE(s, nullptr);
  EXPECT_EQ(s->size(),5);
}

TEST_F(PipelineTaskSetTest, ListToStoreStreamAndLS) {
     FLAGS_minloglevel = google::INFO;  // Disable Info/Warning/Error Logging
   FLAGS_alsologtostderr = true;
  pl::Pipeline p(pool);

  auto streamPath = "./test/data/json/formattest.json";
  pl::tasks::PipelineTaskPtr list = std::make_unique<tasks::PipelineIOTask<load::ListFile>>(streamPath);
  p.addTask(std::move(list));

  auto lspath = "./test/data/json/numtest.json";
  pl::tasks::PipelineTaskPtr lslist = std::make_unique<tasks::PipelineIOTask<load::LSListFile>>(lspath);
  p.addTask(std::move(lslist));

  pl::tasks::PipelineTaskPtr open = std::make_unique<tasks::PipelineIOTask<load::UnformattedFileOpener>>();
  p.addTask(std::move(open));


  pl::tasks::PipelineTaskPtr lsopen = std::make_unique<tasks::PipelineIOTask<load::LSFileOpener>>();
  p.addTask(std::move(lsopen));

  auto lsread = std::make_unique<tasks::PipelineIOTask<load::LineSeparatedStreamReader>>();
  p.addTask(std::move(lsread));

  auto streamparse = std::make_unique<tasks::PipelineIOTask<load::DefaultStreamParser>>(10000);
  p.addTask(std::move(streamparse));

  auto lsparse = std::make_unique<tasks::PipelineIOTask<load::DefaultTextParser>>(10000);
  p.addTask(std::move(lsparse));


  auto store = std::make_unique<tasks::PipelineIOTask<tasks::storage::StorageBuffer>>("C");
  auto* outptr = store.get();
  p.addTask(std::move(store));

  auto outQueue = createQueue<tasks::storage::StorageBuffer::Output>(100);
  outptr->setOutputQueue(&outQueue);
  
  p.execute();

  auto pipelinerepr = p.toJSON();
  auto pipelinestr = stringifyJSON(pipelinerepr);
  auto s = StorageCollection::getInstance().getStorage("C");
  EXPECT_NE(s, nullptr);
  EXPECT_EQ(s->size(),10) << pipelinestr;
}