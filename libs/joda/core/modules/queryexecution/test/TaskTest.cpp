#include <gtest/gtest.h>
#include <joda/document/FileOrigin.h>
#include <joda/parser/pipeline/FileOpener.h>
#include <joda/parser/pipeline/LineSeparatedStreamReader.h>
#include <joda/parser/pipeline/ListDir.h>
#include <joda/parser/pipeline/ListFile.h>
#include <joda/parser/pipeline/TextParser.h>
#include <joda/pipelineatomics/PipelineIOTask.h>
#include <joda/pipelineatomics/TaskID.h>
#include <joda/pipelineatomics/queue/DefaultPipelineQueues.h>
#include <joda/pipelineatomics/queue/PipelineIOQueue.h>
#include <joda/storage/collection/StorageCollection.h>
#include <joda/extension/python/PythonImportTask.h>
#include <joda/extension/python/PythonExport.h>

#include "../src/pipeline/Pipeline.h"
#include "../src/pipeline/tasks/Compatability.h"
#include "../src/pipeline/tasks/storage/StorageBuffer.h"
#include "../src/pipeline/tasks/storage/StorageReceiver.h"
#include "../src/pipeline/optimization/tasks/ChooseAggPipeline.h"
#include "../src/pipeline/optimization/tasks/ChooseAsPipeline.h"
#include "../src/pipeline/optimization/tasks/ChooseAsAggPipeline.h"

namespace std {
// Debug filepath/sample pairs
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& o,
                                     const pair<filesystem::path, double>& p) {
  return o << p.first << "(" << p.first << ")";
}
}  // namespace std

namespace pl = joda::queryexecution::pipeline;
namespace tasks = pl::tasks;
namespace load = tasks::load;

class PipelineTaskTest : public ::testing::Test {
  public:

 void TearDown() override {
    StorageCollection::getInstance().clear();
  }

};

template <class Q>
class QueueProducerLock {
 public:
  QueueProducerLock(Q* q) : q(q) { q->registerProducer(); }
  ~QueueProducerLock() { q->unregisterProducer(); }

 private:
  Q* q;
};

template <class T, pl::queue::QueueSizeTrait Q = pl::queue::LargeQueueTrait>
pl::PipelineIOQueue<T, Q::BulkSize> createQueue(size_t minCapacity = 10) {
  return pl::PipelineIOQueue<T, Q::BulkSize>(0, minCapacity);
}

template <class T, pl::queue::QueueSizeTrait S = pl::queue::LargeQueueTrait>
::testing::AssertionResult testQueueContent(
    pl::PipelineIOQueue<T, S::BulkSize>& queue, std::vector<T>& expected) {
  auto& q = queue.getQueue();
  auto expected_size = expected.size();
  auto q_size = q.size();
  if (q_size != expected_size) {
    return ::testing::AssertionFailure()
           << "Queue size is " << q_size << " but expected "
           << expected_size;
  }

  for (size_t i = 0; i < expected_size; i++) {
    T elem;
    q.pop(elem);
    auto f = std::find(expected.begin(), expected.end(), elem);
    if (f == expected.end()) {
      return ::testing::AssertionFailure()
             << "Element " << elem << " not found in expected elements";
    } else {
      expected.erase(f);
    }
  }
  if (expected.size() != 0) {
    return ::testing::AssertionFailure() << "Expected elements not found";
  }

  return ::testing::AssertionSuccess();
}

TEST_F(PipelineTaskTest, CompatabilityAndIDTest) {
  std::vector<std::string> ids;

  ids.emplace_back(tasks::ID<load::ListFile>::NAME);                            // 0
  ids.emplace_back(tasks::ID<load::ListDir>::NAME);                             // 1
  ids.emplace_back(tasks::ID<load::LSListFile>::NAME);                          // 2
  ids.emplace_back(tasks::ID<load::LSListDir>::NAME);                           // 3
  ids.emplace_back(tasks::ID<load::UnformattedFileOpener>::NAME);               // 4
  ids.emplace_back(tasks::ID<load::LSFileOpener>::NAME);                        // 5
  ids.emplace_back(tasks::ID<load::LineSeparatedStreamReader>::NAME);           // 6
  ids.emplace_back(tasks::ID<load::DefaultTextParser>::NAME);                   // 7
  ids.emplace_back(tasks::ID<load::DefaultStreamParser>::NAME);                 // 8
  ids.emplace_back(tasks::ID<tasks::storage::StorageBuffer>::NAME);             // 9
  ids.emplace_back(tasks::ID<tasks::storage::StorageReceiver>::NAME);           // 10
  ids.emplace_back(tasks::ID<tasks::storage::StorageSender>::NAME);             // 11
  ids.emplace_back(tasks::ID<tasks::choose::ChoosePipeline>::NAME);             // 12
  ids.emplace_back(tasks::ID<tasks::as::AsPipeline>::NAME);                     // 13
  ids.emplace_back(tasks::ID<tasks::store::JSONStringifier>::NAME);             // 14
  ids.emplace_back(tasks::ID<tasks::store::WriteFile>::NAME);                   // 15
  ids.emplace_back(tasks::ID<tasks::store::WriteFiles>::NAME);                  // 16
  ids.emplace_back(tasks::ID<tasks::load::InStream>::NAME);                     // 17
  ids.emplace_back(tasks::ID<load::LSFileMapper>::NAME);                        // 18
  ids.emplace_back(tasks::ID<tasks::agg::AggPipeline>::NAME);                   // 19
  ids.emplace_back(tasks::ID<tasks::agg::AggMerger>::NAME);                     // 20
  ids.emplace_back(tasks::ID<tasks::join::LoadJoin>::NAME);                     // 21
  ids.emplace_back(tasks::ID<tasks::join::LoadJoinFiles>::NAME);                // 22
  ids.emplace_back(tasks::ID<tasks::join::LoadJoinFileParser>::NAME);           // 23
  ids.emplace_back(tasks::ID<tasks::join::StoreJoin>::NAME);                    // 24
  ids.emplace_back(tasks::ID<tasks::agg::WindowAgg>::NAME);                     // 25
  ids.emplace_back(tasks::ID<tasks::join::InnerJoin>::NAME);                    // 26
  ids.emplace_back(tasks::ID<tasks::join::OuterJoin>::NAME);                    // 27
  ids.emplace_back(tasks::ID<tasks::store::WriteOutStream>::NAME);              // 28
  ids.emplace_back(tasks::ID<tasks::optimization::ChooseAsPipeline>::NAME);     // 29
  ids.emplace_back(tasks::ID<tasks::optimization::ChooseAggPipeline>::NAME);    // 30
  ids.emplace_back(tasks::ID<tasks::optimization::ChooseAsAggPipeline>::NAME);  // 31


  #ifdef JODA_ENABLE_PYTHON
  ids.emplace_back(tasks::ID<tasks::store::PythonExporter>::NAME);              // 32
  ids.emplace_back(tasks::ID<tasks::load::PythonImport>::NAME);                 // 33
  #endif // JODA_ENABLE_PYTHON
  

  // Check StorageSender/StorageReceiver compatability
  for (size_t i = 0; i < ids.size(); i++) {
    for (size_t j = 0; j < ids.size(); j++) {
      if (i == j) continue;
      if (tasks::TaskCompatability::belongsToGroup(
              ids[i], tasks::TaskCompatability::ContainerSenderGroup) &&
          tasks::TaskCompatability::belongsToGroup(
              ids[j], tasks::TaskCompatability::ContainerReceiverGroup)) {
        EXPECT_TRUE(tasks::TaskCompatability::check(ids[i], ids[j]))
            << "Task " << ids[i] << " and " << ids[j] << " are not compatible";
      }
    }
  }

  // Check Name uniqueness
  std::set<std::string> uniqueids(ids.begin(), ids.end());
  ASSERT_EQ(ids.size(), uniqueids.size());

  // Check compatibility
  EXPECT_TRUE(tasks::TaskCompatability::check(ids[0],
                                              ids[4]));  // File -> FileOpener
  EXPECT_TRUE(tasks::TaskCompatability::check(ids[1],
                                              ids[4]));  // Dir -> FileOpener
  EXPECT_TRUE(
      tasks::TaskCompatability::check(ids[2],
                                      ids[5]));  // LSFile -> LSFileOpener
  EXPECT_TRUE(
      tasks::TaskCompatability::check(ids[3],
                                      ids[5]));  // LSDir -> LSFileOpener
  EXPECT_TRUE(tasks::TaskCompatability::check(
      ids[2], ids[18]));  // LSFile -> LSFileMapper
  EXPECT_TRUE(
      tasks::TaskCompatability::check(ids[3],
                                      ids[18]));  // LSDir -> LSFileMapper
  EXPECT_TRUE(tasks::TaskCompatability::check(
      ids[5], ids[6]));  // LSFileOpener -> LineSeparatedStreamReader
  EXPECT_TRUE(tasks::TaskCompatability::check(
      ids[4], ids[8]));  // FileOpener -> StreamParser
  EXPECT_TRUE(tasks::TaskCompatability::check(
      ids[6], ids[7]));  // LineSeparatedStreamReader -> TextParser
  EXPECT_TRUE(tasks::TaskCompatability::check(
      ids[18], ids[7]));  // LSFileMapper -> TextParser
  EXPECT_TRUE(tasks::TaskCompatability::check(
      ids[17], ids[8]));  // InStream -> StreamParser
  EXPECT_TRUE(tasks::TaskCompatability::check(
      ids[8], ids[10]));  // StreamParser -> StorageReceiver
  EXPECT_TRUE(tasks::TaskCompatability::check(
      ids[12], ids[13]));  // ChoosePipeline -> AsPipeline
  EXPECT_TRUE(tasks::TaskCompatability::check(
      ids[13], ids[14]));  // AsPipeline -> JSONStringifier
  EXPECT_TRUE(tasks::TaskCompatability::check(
      ids[14], ids[15]));  // JSONStringifier -> WriteFile
  EXPECT_TRUE(tasks::TaskCompatability::check(
      ids[14], ids[16]));  // JSONStringifier -> WriteFiles

  // Check Requirements
  EXPECT_EQ(tasks::TaskRequirement::get(ids[0]), ids[4]);  // File -> FileOpener
  EXPECT_EQ(tasks::TaskRequirement::get(ids[1]), ids[4]);  // Dir -> FileOpener
  EXPECT_EQ(tasks::TaskRequirement::get(ids[2]),
            ids[5]);  // LSFile -> LSFileOpener
  EXPECT_EQ(tasks::TaskRequirement::get(ids[3]),
            ids[5]);  // LSDir -> LSFileOpener
  EXPECT_EQ(tasks::TaskRequirement::get(ids[5]),
            ids[6]);  // LSFileOpener -> LineSeparatedStreamReader
  EXPECT_EQ(tasks::TaskRequirement::get(ids[4]),
            ids[8]);  // FileOpener -> StreamParser
  EXPECT_EQ(tasks::TaskRequirement::get(ids[6]),
            ids[7]);  // LineSeparatedStreamReader -> TextParser
  EXPECT_EQ(tasks::TaskRequirement::get(ids[17]),
            ids[8]);  // InStream -> StreamParser
  EXPECT_EQ(tasks::TaskRequirement::get(ids[6]),
            ids[7]);  // LineSeparatedStreamReader -> TextParser
  EXPECT_EQ(tasks::TaskRequirement::get(ids[12]),
            ids[13]);  // ChoosePipeline -> AsPipeline
  EXPECT_EQ(tasks::TaskRequirement::get(ids[19]),
            ids[20]);  // AggPipeline -> AggMerger
  EXPECT_EQ(tasks::TaskRequirement::get(ids[22]),
            ids[23]);  // LoadJoin -> LoadJoinFileParser

  // Check Prerequisites
  EXPECT_EQ(tasks::TaskPrerequisite::get(ids[15]),
            ids[14]);  // WriteFile -> JSONStringifier
  EXPECT_EQ(tasks::TaskPrerequisite::get(ids[16]),
            ids[14]);  // WriteFiles -> JSONStringifier
}

TEST_F(PipelineTaskTest, ListFile) {
  // Test Pipeline
  auto t = std::make_unique<tasks::PipelineIOTask<load::ListFile>>("");

  typedef load::ListFile::Output Output;
  auto output_q = createQueue<Output>(10);

  auto& o_q = output_q.getQueue();
  ASSERT_EQ(o_q.size(), 0);

  EXPECT_EQ(load::ListFile::Async, pl::tasks::AsyncType::SingleThreaded);

  load::ListFile invalid("not - a - valid - path");
  invalid.execute(nullptr, &output_q);
  EXPECT_EQ(o_q.size(), 0);

  load::ListFile nonexistent("/this/path/does/not/exist");
  nonexistent.execute(nullptr, &output_q);
  EXPECT_EQ(o_q.size(), 0);

  auto p = "./test/data/json/numtest.json";
  load::ListFile exists(p);
  {
    QueueProducerLock ol(&output_q);
    exists.execute(nullptr, &output_q);
  }
  EXPECT_EQ(o_q.size(), 1);

  std::vector<Output> expected;
  expected.emplace_back(p, 1.0);
  EXPECT_TRUE(testQueueContent<Output>(output_q, expected));
}

TEST_F(PipelineTaskTest, ListDir) {
  // Test Pipeline
  auto t = std::make_unique<tasks::PipelineIOTask<load::ListDir>>("");

  typedef load::ListDir::Output Output;
  auto output_q = createQueue<Output>(10);


  auto& o_q = output_q.getQueue();
  ASSERT_EQ(o_q.size(), 0);

  EXPECT_EQ(load::ListDir::Async, pl::tasks::AsyncType::SingleThreaded);

  load::ListDir invalid("not - a - valid - path");
  invalid.execute(nullptr, &output_q);
  EXPECT_EQ(o_q.size(), 0);

  load::ListDir nonexistent("/this/path/does/not/exist");
  nonexistent.execute(nullptr, &output_q);
  EXPECT_EQ(o_q.size(), 0);

  auto p = "./test/data/json";
  load::ListDir exists(p);
  {
    QueueProducerLock ol(&output_q);
    exists.execute(nullptr, &output_q);
  }
  EXPECT_EQ(o_q.size(), 3);

  std::vector<Output> expected;
  expected.emplace_back(p + std::string("/numtest.json"), 1.0);
  expected.emplace_back(p + std::string("/formattest.json"), 1.0);
  expected.emplace_back(p + std::string("/viewtest.json"), 1.0);
  EXPECT_TRUE(testQueueContent<Output>(output_q, expected));
}

TEST_F(PipelineTaskTest, LineFileOpener) {
  // Test Pipeline
  auto t = std::make_unique<tasks::PipelineIOTask<load::LSFileOpener>>();

  // Input
  typedef typename load::LSFileOpener::Input Input;
  auto input_q = createQueue<Input>(10);

  // Output
  typedef typename load::LSFileOpener::Output Output;
  auto output_q = createQueue<Output>(10);



  auto& i_q = input_q.getQueue();
  auto& o_q = output_q.getQueue();
  ASSERT_EQ(o_q.size(), 0);

  EXPECT_EQ(load::LSFileOpener::Async, pl::tasks::AsyncType::SingleThreaded);
  auto p = "./test/data/json/numtest.json";
  {
    QueueProducerLock il(&input_q);
    Input o = std::make_pair(p, 1.0);
    i_q.push(std::move(o));

  }

  load::LSFileOpener opener;
  {
    QueueProducerLock ol(&output_q);
    opener.execute(&input_q, &output_q);
  }
  ASSERT_EQ(o_q.size(), 1);
  Output o;
  o_q.pop(o);
  EXPECT_NE(o.first.streamOrigin, nullptr);
  EXPECT_NE(o.first.stream, nullptr);
  EXPECT_STREQ(
      g_FileNameRepoInstance.getFile(o.first.streamOrigin->getID()).c_str(), p);
}

TEST_F(PipelineTaskTest, UnformattedFileOpener) {
  // Test Pipeline
  auto t =
      std::make_unique<tasks::PipelineIOTask<load::UnformattedFileOpener>>();

  // Input
  typedef typename load::UnformattedFileOpener::Input Input;
  auto input_q = createQueue<Input>(10);

  // Output
  typedef typename load::UnformattedFileOpener::Output Output;
  auto output_q = createQueue<Output>(10);



  auto& i_q = input_q.getQueue();
  auto& o_q = output_q.getQueue();
  ASSERT_EQ(o_q.size(), 0);

  EXPECT_EQ(load::UnformattedFileOpener::Async,
            pl::tasks::AsyncType::SingleThreaded);
  auto p = "./test/data/json/numtest.json";
  {
    QueueProducerLock il(&input_q);
    Input i = std::make_pair(p, 1.0);
    i_q.push(std::move(i));

  }

  load::UnformattedFileOpener opener;
  {
    QueueProducerLock ol(&output_q);
    opener.execute(&input_q, &output_q);
  }
  ASSERT_EQ(o_q.size(), 1);
  Output o;
  o_q.pop(o);
  EXPECT_NE(o.first.streamOrigin, nullptr);
  EXPECT_NE(o.first.stream, nullptr);
  EXPECT_STREQ(
      g_FileNameRepoInstance.getFile(o.first.streamOrigin->getID()).c_str(), p);
}

TEST_F(PipelineTaskTest, LineSeparatedStreamReader) {
  // Test Pipeline
  auto t = std::make_unique<
      tasks::PipelineIOTask<load::LineSeparatedStreamReader>>();

  // Input
  typedef typename load::LineSeparatedStreamReader::Input Input;
  auto input_q = createQueue<Input>(10);

  // Output
  typedef typename load::LineSeparatedStreamReader::Output Output;
  auto output_q = createQueue<Output, pl::queue::LargeQueueTrait>(10);



  auto& i_q = input_q.getQueue();
  auto& o_q = output_q.getQueue();
  ASSERT_EQ(o_q.size(), 0);

  EXPECT_EQ(load::LineSeparatedStreamReader::Async,
            pl::tasks::AsyncType::SingleThreaded);

  auto p = "./test/data/json/numtest.json";
  auto file = std::make_unique<std::ifstream>();
  file->open(p);
  ASSERT_TRUE(file->is_open());
  auto so = joda::docparsing::StreamOrigin{
      std::make_unique<FileOrigin>(g_FileNameRepoInstance.addFile(p)),
      std::move(file)};
  {
    QueueProducerLock il(&input_q);
    auto o = std::make_pair(std::move(so), 1.0);
    i_q.push(std::move(o));
  }


  load::LineSeparatedStreamReader reader;
  {
    QueueProducerLock ol(&output_q);
    reader.execute(&input_q, &output_q);
  }
  ASSERT_EQ(o_q.size(), 1);
  Output o;
  o_q.pop(o);
  EXPECT_EQ(o.size(),5);
}

TEST_F(PipelineTaskTest, LSFileMapper) {
  // Test Pipeline
  auto t = std::make_unique<tasks::PipelineIOTask<load::LSFileMapper>>();

  // Input
  typedef typename load::LSFileMapper::Input Input;
  auto input_q = createQueue<Input>(10);

  // Output
  typedef typename load::LSFileMapper::Output Output;
  auto output_q = createQueue<Output, pl::queue::LargeQueueTrait>(10);



  auto& i_q = input_q.getQueue();
  auto& o_q = output_q.getQueue();
  ASSERT_EQ(o_q.size(), 0);

  EXPECT_EQ(load::LSFileMapper::Async, pl::tasks::AsyncType::SingleThreaded);

  auto p = "./test/data/json/numtest.json";

  {
    QueueProducerLock il(&input_q);
    Input i = std::make_pair(p, 1.0);
    i_q.push(std::move(i));
  }


  load::LSFileMapper mapper;
  {
    QueueProducerLock ol(&output_q);
    mapper.execute(&input_q, &output_q);
  }
  ASSERT_EQ(o_q.size(), 1);
  Output o;
  o_q.pop(o);
  EXPECT_EQ(o.size(), 5);
}

TEST_F(PipelineTaskTest, DefaultStreamParser) {
  // Test Pipeline
  auto t =
      std::make_unique<tasks::PipelineIOTask<load::DefaultStreamParser>>(0);

  // Input
  typedef typename load::DefaultStreamParser::Input Input;
  auto input_q = createQueue<Input>(10);

  // Output
  typedef typename load::DefaultStreamParser::Output Output;
  auto output_q = createQueue<Output>(10);



  auto& i_q = input_q.getQueue();
  auto& o_q = output_q.getQueue();
  ASSERT_EQ(o_q.size(), 0);

  EXPECT_EQ(load::DefaultStreamParser::Async,
            pl::tasks::AsyncType::MultiThreaded);

  auto p = "./test/data/json/formattest.json";
  auto file = std::make_unique<std::ifstream>();
  file->open(p);
  ASSERT_TRUE(file->is_open());
  auto so = joda::docparsing::StreamOrigin{
      std::make_unique<FileOrigin>(g_FileNameRepoInstance.addFile(p)),
      std::move(file)};
  {
    QueueProducerLock il(&input_q);
    auto o = std::make_pair(std::move(so), 1.0);
    i_q.push( std::move(o));
  }


  load::DefaultStreamParser parser(500000);
  {
    QueueProducerLock ol(&output_q);
    parser.execute(&input_q, &output_q);
  }
  ASSERT_EQ(o_q.size(), 1);
  Output cont;
  o_q.pop(cont);
  ASSERT_NE(cont, nullptr);

  std::vector<std::string> sourceDocs;
  sourceDocs.emplace_back("{\"desc\":\"ones\",\"a\":1,\"b\":1,\"c\":1}");
  sourceDocs.emplace_back("{\"desc\":\"asc\",\"a\":1,\"b\":2,\"c\":3}");
  sourceDocs.emplace_back("{\"desc\":\"zerofront\",\"a\":0,\"b\":1,\"c\":1}");
  sourceDocs.emplace_back("{\"desc\":\"zeroback\",\"a\":1,\"b\":1,\"c\":0}");
  sourceDocs.emplace_back("{\"desc\":\"missing_b\",\"a\":1,\"c\":1}");
  EXPECT_EQ(cont->size(), sourceDocs.size());
  auto docs = cont->stringify();
  std::sort(sourceDocs.begin(), sourceDocs.end());
  std::sort(docs.begin(), docs.end());
  EXPECT_TRUE(std::equal(sourceDocs.begin(), sourceDocs.end(), docs.begin()));
}

TEST_F(PipelineTaskTest, DefaultTextParser) {
  // Test Pipeline
  auto t = std::make_unique<tasks::PipelineIOTask<load::DefaultTextParser>>(0);

  // Input
  typedef typename load::DefaultTextParser::Input Input;
  auto input_q = createQueue<Input, pl::queue::LargeQueueTrait>(10);

  // Output
  typedef typename load::DefaultTextParser::Output Output;
  auto output_q = createQueue<Output>(10);



  auto& i_q = input_q.getQueue();
  auto& o_q = output_q.getQueue();
  ASSERT_EQ(o_q.size(), 0);

  EXPECT_EQ(load::DefaultTextParser::Async,
            pl::tasks::AsyncType::MultiThreaded);

  Input ibuff;
  auto sourceDocs = std::vector<std::string>{"1", "\"string\"", "{\"doc\":1}"};
  for (auto& d : sourceDocs) {
    auto copy = std::string(d);
    auto so = joda::docparsing::StringOrigin(std::make_unique<FileOrigin>(1),
                                             std::move(copy));
    ibuff.emplace_back(std::move(so));
  }

  {
    QueueProducerLock il(&input_q);
     i_q.push(std::move(ibuff));
  }


  load::DefaultTextParser parser(1000000);
  {
    QueueProducerLock ol(&output_q);
    parser.execute(&input_q, &output_q);
  }
  ASSERT_EQ(o_q.size(), 1);
  Output cont;
  o_q.pop(cont);
  ASSERT_NE(cont, nullptr);

  EXPECT_EQ(cont->size(), 3);
  auto docs = cont->stringify();
  std::sort(sourceDocs.begin(), sourceDocs.end());
  std::sort(docs.begin(), docs.end());
  EXPECT_TRUE(std::equal(sourceDocs.begin(), sourceDocs.end(), docs.begin()));
}

TEST_F(PipelineTaskTest, StorageBuffer) {
  // Test Pipeline
  auto t =
      std::make_unique<tasks::PipelineIOTask<tasks::storage::StorageBuffer>>(
          "A");
  // Input
  typedef typename tasks::storage::StorageBuffer::Input Input;
  auto input_q = createQueue<Input>(200);

  // Output
  typedef typename tasks::storage::StorageBuffer::Output Output;
  auto output_q = createQueue<Output>(200);



  auto& i_q = input_q.getQueue();
  auto& o_q = output_q.getQueue();
  ASSERT_EQ(o_q.size(), 0);

  EXPECT_EQ(tasks::storage::StorageBuffer::Async,
            pl::tasks::AsyncType::Synchronous);

  std::vector<std::shared_ptr<JSONContainer>> ptrs;
  for (size_t i = 0; i < 100; i++) {
    ptrs.emplace_back(std::make_shared<JSONContainer>());
  }
  {
    QueueProducerLock il(&input_q);
    for (auto& p : ptrs) {
      i_q.push(p);
    }
  }


  tasks::storage::StorageBuffer buffer("A");
  {
    QueueProducerLock ol(&output_q);
    while(buffer.execute(&input_q, &output_q) != pl::tasks::TaskStatus::FINISHED){
      // Do nothing
    }
  }


  std::vector<std::shared_ptr<JSONContainer>> results;
  results.reserve(100);
  std::shared_ptr<JSONContainer> o;
  while (!output_q.isFinished()) {
    o_q.pop(o);
    if (o != nullptr) {
      results.emplace_back(std::move(o));
    }
  }

  auto store = StorageCollection::getInstance().getStorage("A");
  EXPECT_NE(store, nullptr);
  EXPECT_EQ(store->contSize(), 100);
  EXPECT_EQ(results.size(), ptrs.size());
  std::sort(results.begin(), results.end());
  std::sort(ptrs.begin(), ptrs.end());
  EXPECT_TRUE(std::equal(results.begin(), results.end(), ptrs.begin()));
}

TEST_F(PipelineTaskTest, StorageReceiver) {
  // Test Pipeline
  auto t =
      std::make_unique<tasks::PipelineIOTask<tasks::storage::StorageReceiver>>(
          "B");
  // Input
  typedef typename tasks::storage::StorageReceiver::Input Input;
  auto input_q = createQueue<Input>(200);


  auto& i_q = input_q.getQueue();

  EXPECT_EQ(tasks::storage::StorageReceiver::Async,
            pl::tasks::AsyncType::Synchronous);

  std::vector<std::shared_ptr<JSONContainer>> ptrs;
  for (size_t i = 0; i < 100; i++) {
    QueueProducerLock il(&input_q);
    i_q.push(std::make_shared<JSONContainer>());
  }


  tasks::storage::StorageReceiver recv("B");
  std::unique_ptr<moodycamel::ProducerToken> emptyPtr;
  recv.execute(&input_q, nullptr);
  auto store = StorageCollection::getInstance().getStorage("B");
  EXPECT_NE(store, nullptr);
  EXPECT_EQ(store->contSize(), 100);
}

TEST_F(PipelineTaskTest, StorageSender) {
  auto store = StorageCollection::getInstance().getOrAddStorage("C");
  EXPECT_NE(store, nullptr);
  // Test Pipeline
  auto t =
      std::make_unique<tasks::PipelineIOTask<tasks::storage::StorageSender>>(
          "C");

  typedef tasks::storage::StorageSender::Output Output;
  auto output_q = createQueue<Output>(100);


  auto& o_q = output_q.getQueue();

  for (size_t i = 0; i < 100; i++) {
    store->insertDocuments(std::make_shared<JSONContainer>());
  }

  ASSERT_EQ(o_q.size(), 0);

  EXPECT_EQ(tasks::storage::StorageSender::Async,
            pl::tasks::AsyncType::Synchronous);

  tasks::storage::StorageSender sender("C");
  {
    QueueProducerLock ol(&output_q);

    sender.execute(nullptr, &output_q);
  }
  EXPECT_EQ(o_q.size(), 100);
}

TEST_F(PipelineTaskTest, ChoosePipeline) {
  // TODO
}

TEST_F(PipelineTaskTest, AsPipeline) {
  // TODO
}

TEST_F(PipelineTaskTest, AggPipeline) {
  // TODO
}

TEST_F(PipelineTaskTest, AggMerger) {
  // TODO
}

TEST_F(PipelineTaskTest, WindowAgg) {
  // TODO
}

TEST_F(PipelineTaskTest, InnerJoin) {
  // TODO
}

TEST_F(PipelineTaskTest, OuterJoin) {
  // TODO
}

TEST_F(PipelineTaskTest, LoadJoin) {
  // TODO
}

TEST_F(PipelineTaskTest, StoreJoin) {
  // TODO
}