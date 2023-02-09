#include <gtest/gtest.h>
#include <joda/parser/pipeline/FileOpener.h>
#include <joda/parser/pipeline/InStream.h>
#include <joda/parser/pipeline/LineSeparatedStreamReader.h>
#include <joda/parser/pipeline/ListDir.h>
#include <joda/parser/pipeline/ListFile.h>
#include <joda/parser/pipeline/TextParser.h>
#include <joda/parser/pipeline/URLStream.h>
#include <joda/pipelineatomics/PipelineIOTask.h>
#include <joda/pipelineatomics/TaskID.h>
#include <joda/queryexecution/PipelineQueryPlan.h>
#include <joda/queryparsing/QueryParser.h>

#include "../src/pipeline/Pipeline.h"
#include "../src/pipeline/tasks/as/AsPipeline.h"
#include "../src/pipeline/tasks/choose/ChoosePipeline.h"
#include "../src/pipeline/tasks/storage/StorageBuffer.h"
#include "../src/pipeline/tasks/storage/StorageReceiver.h"
#include "../src/pipeline/tasks/storage/StorageSender.h"
#include "../src/pipeline/tasks/agg/AggMerger.h"

#include "../src/pipeline/optimization/OptimizationRule.h"
#include "../src/pipeline/optimization/SimpleOptimizationRule.h"
#include "../src/pipeline/optimization/QueryCombinationRule.h"
#include "../src/pipeline/optimization/FilterAggregationRule.h"

#include "../src/pipeline/optimization/tasks/ChooseAsPipeline.h"
#include "../src/pipeline/optimization/tasks/ChooseAsAggPipeline.h"
#include "../src/pipeline/optimization/tasks/ChooseAggPipeline.h"

namespace pl = joda::queryexecution::pipeline;
namespace tasks = pl::tasks;
namespace load = tasks::load;
namespace choose = tasks::choose;
namespace as = tasks::as;
namespace optimization = pl::optimization;

class PlanTest : public ::testing::Test {
 public:
  std::shared_ptr<pl::ThreadPool> pool = std::make_shared<pl::ThreadPool>(4);
};

std::shared_ptr<joda::query::Query> parseQuery(const std::string& q) {
  joda::queryparsing::QueryParser qp;
  auto q_parsed = qp.parse(q);
  DCHECK(q_parsed != nullptr) << qp.getLastError();
  return q_parsed;
}

std::vector<std::pair<size_t, std::string>> extractTasks(
    const RJDocument& plJson) {
  std::vector<std::pair<size_t, std::string>> out;
  if (!plJson.HasMember("Tasks")) {
    return out;
  }

  const auto& tasks = plJson["Tasks"];
  for (const auto& task : tasks.GetArray()) {
    out.emplace_back(
        task["Num"].GetInt64(), task["Name"].GetString());
  }

  return out;
}

std::vector<std::pair<size_t, size_t>> extractConnections(
    const RJDocument& plJson) {
  std::vector<std::pair<size_t, size_t>> out;
  if (!plJson.HasMember("Connections")) {
    return out;
  }

  const auto& conns = plJson["Connections"];
  for (const auto& con : conns.GetArray()) {
    out.emplace_back(
        std::make_pair(con["From"].GetInt64(), con["To"].GetInt64()));
  }

  return out;
}

::testing::AssertionResult containsTasks(std::vector<std::string>& tasks,
                                         const RJDocument& plJson) {
  auto pl_tasks = extractTasks(plJson);

  std::string extractedTaskList;
  for (const auto& t : pl_tasks) {
    if (extractedTaskList.size() > 0) {
      extractedTaskList += ", ";
    }
    extractedTaskList += t.second + "(" + std::to_string(t.first) + ")";
  }

  std::string expectedTaskList;
  for (const auto& t : tasks) {
    if (expectedTaskList.size() > 0) {
      expectedTaskList += ", ";
    }
    expectedTaskList += t;
  }

  if (pl_tasks.size() != tasks.size()) {
    return ::testing::AssertionFailure()
           << "Wrong number of tasks. Expected [" << expectedTaskList
           << "] but got [" << extractedTaskList << "]";
  }

  for (size_t i = 0; i < tasks.size(); ++i) {
    if (tasks[i] != pl_tasks[i].second) {
      return ::testing::AssertionFailure()
             << "Wrong task name. Expected '" << tasks[i] << "' but got '"
             << pl_tasks[i].second << "' at index " << i << ". Expected [" << expectedTaskList << "] but got [" << extractedTaskList << "]";
    }
  }

  return ::testing::AssertionSuccess();
}

::testing::AssertionResult testQueryForTasks(const std::string& query,
                                             std::vector<std::string>& tasks) {
  joda::queryexecution::PipelineQueryPlan pqp(1);
  auto q = parseQuery(query);
  pqp.createPlan(q);
  auto pipeline = pqp.getPipeline()->toJSON();
  return containsTasks(tasks, pipeline);
}

::testing::AssertionResult testOptimizationForTasks(const std::string& query, std::vector<std::unique_ptr<optimization::OptimizationRule>>& optimizations,
                                             std::vector<std::string>& before,std::vector<std::string>& after) {
  joda::queryexecution::PipelineQueryPlan pqp(1);
  auto q = parseQuery(query);
  pqp.createPlan(q);
  auto bp = pqp.getPipeline()->toJSON();
  auto b = containsTasks(before, bp);
  if (!b) {
    return b;
  }
  pqp.getPipeline()->optimize(optimizations);
  auto ap = pqp.getPipeline()->toJSON();
  return containsTasks(after, ap);
}

::testing::AssertionResult testOptimizationForTasks(const std::vector<std::string>& queries, std::vector<std::unique_ptr<optimization::OptimizationRule>>& optimizations,
                                             std::vector<std::string>& before,std::vector<std::string>& after) {
  joda::queryexecution::PipelineQueryPlan pqp(1);
  std::vector<std::shared_ptr<joda::query::Query>> qs;
  for (const auto& q : queries) {
    auto parsed = parseQuery(q);
    qs.push_back(std::move(parsed));
  }
  pqp.createPlan(qs);
  auto bp = pqp.getPipeline()->toJSON();
  auto b = containsTasks(before, bp);
  if (!b) {
    return b;
  }
  pqp.getPipeline()->optimize(optimizations);
  auto ap = pqp.getPipeline()->toJSON();
  return containsTasks(after, ap);
}

/*
  _      ____          _____
 | |    / __ \   /\   |  __ \. 
 | |   | |  | | /  \  | |  | |
 | |   | |  | |/ /\ \ | |  | |
 | |___| |__| / ____ \| |__| |
 |______\____/_/    \_\_____/
*/

TEST_F(PlanTest, Noop) {
  std::vector<std::string> tasks = {};

  config::enable_streams = true;
  EXPECT_TRUE(testQueryForTasks("LOAD A ", tasks));
}

TEST_F(PlanTest, LoadAllTest) {
  std::vector<std::string> tasks = {
      tasks::ID<load::LSListFile>::NAME,
      tasks::ID<load::ListDir>::NAME,
      tasks::ID<load::LSListDir>::NAME,
      tasks::ID<load::ListFile>::NAME,
      tasks::ID<load::URLStream>::NAME,
      tasks::ID<load::InStream>::NAME,
      tasks::ID<load::LSFileOpener>::NAME,
      tasks::ID<load::LineSeparatedStreamReader>::NAME,
      tasks::ID<load::DefaultTextParser>::NAME,
      tasks::ID<load::UnformattedFileOpener>::NAME,
      tasks::ID<load::DefaultStreamParser>::NAME,
      tasks::ID<tasks::storage::StorageReceiver>::NAME};

  config::enable_streams = true;
  EXPECT_TRUE(testQueryForTasks(
      "LOAD A FROM FILE \"./test/data/json/numtest.json\" LINESEPARATED, "
      "FROM FILES \"./test/data/json\", "
      "FROM FILES \"./test/data/json\" LINESEPARATED, "
      "FROM FILE \"./test/data/json/formattest.json\", "
      "FROM URL \"http://api.joda.json\", "
      "FROM STREAM",
      tasks));
}

TEST_F(PlanTest, LoadLSTest) {
  std::vector<std::string> tasks = {
      tasks::ID<load::LSListFile>::NAME,
      tasks::ID<load::LSListDir>::NAME,
      tasks::ID<load::LSFileOpener>::NAME,
      tasks::ID<load::LineSeparatedStreamReader>::NAME,
      tasks::ID<load::DefaultTextParser>::NAME,
      tasks::ID<tasks::storage::StorageReceiver>::NAME};

  EXPECT_TRUE(testQueryForTasks(
      "LOAD A FROM FILE \"./test/data/json/numtest.json\" LINESEPARATED, "
      "FROM FILES \"./test/data/json\" LINESEPARATED ",
      tasks));
}

TEST_F(PlanTest, LoadStreamTest) {
  std::vector<std::string> tasks = {
      tasks::ID<load::ListDir>::NAME,
      tasks::ID<load::ListFile>::NAME,
      tasks::ID<load::URLStream>::NAME,
      tasks::ID<load::InStream>::NAME,
      tasks::ID<load::UnformattedFileOpener>::NAME,
      tasks::ID<load::DefaultStreamParser>::NAME,
      tasks::ID<tasks::storage::StorageReceiver>::NAME};

  config::enable_streams = true;
  EXPECT_TRUE(
      testQueryForTasks("LOAD A "
                        "FROM FILES \"./test/data/json\", "
                        "FROM FILE \"./test/data/json/formattest.json\", "
                        "FROM URL \"http://api.joda.json\", "
                        "FROM STREAM",
                        tasks));
}

//   _____ _    _  ____   ____   _____ ______
//  / ____| |  | |/ __ \ / __ \ / ____|  ____|
// | |    | |__| | |  | | |  | | (___ | |__
// | |    |  __  | |  | | |  | |\___ \|  __|
// | |____| |  | | |__| | |__| |____) | |____
//  \_____|_|  |_|\____/ \____/|_____/|______|

TEST_F(PlanTest, SimpleConstTrue) {
  std::vector<std::string> tasks = {};

  EXPECT_TRUE(
      testQueryForTasks("LOAD A "
                        "CHOOSE 1 == 1",
                        tasks));
}

TEST_F(PlanTest, SimpleConstFalse) {
  std::vector<std::string> tasks = {
      tasks::ID<tasks::storage::StorageSender>::NAME,
      tasks::ID<tasks::choose::ChoosePipeline>::NAME,
      tasks::ID<tasks::as::AsPipeline>::NAME,
      tasks::ID<tasks::storage::StorageReceiver>::NAME};

  EXPECT_TRUE(
      testQueryForTasks("LOAD A "
                        "CHOOSE 1 != 1",
                        tasks));
}

TEST_F(PlanTest, SimpleConstTrueLoad) {
  std::vector<std::string> tasks = {
      tasks::ID<load::ListDir>::NAME,
      tasks::ID<load::UnformattedFileOpener>::NAME,
      tasks::ID<load::DefaultStreamParser>::NAME,
      tasks::ID<tasks::storage::StorageReceiver>::NAME};

  EXPECT_TRUE(
      testQueryForTasks("LOAD A "
                        "FROM FILES \"./test/data/json\""
                        "CHOOSE 1 == 1",
                        tasks));
}

TEST_F(PlanTest, SimpleChoose) {
  std::vector<std::string> tasks = {
    tasks::ID<tasks::storage::StorageSender>::NAME,
      tasks::ID<tasks::choose::ChoosePipeline>::NAME,
      tasks::ID<tasks::as::AsPipeline>::NAME,
      tasks::ID<tasks::storage::StorageReceiver>::NAME
      };

  EXPECT_TRUE(
      testQueryForTasks("LOAD A "
                        "CHOOSE '/a' == 1",
                        tasks));
}

TEST_F(PlanTest, SimpleChooseLoad) {
  std::vector<std::string> tasks = {
      tasks::ID<load::URLStream>::NAME,
      tasks::ID<load::DefaultStreamParser>::NAME,
      tasks::ID<tasks::storage::StorageReceiver>::NAME,
      tasks::ID<tasks::storage::StorageSender>::NAME,
      tasks::ID<tasks::choose::ChoosePipeline>::NAME,
      tasks::ID<tasks::as::AsPipeline>::NAME,
      tasks::ID<tasks::storage::StorageReceiver>::NAME
      };

  EXPECT_TRUE(
      testQueryForTasks("LOAD A "
                        "FROM URL \"http://test.url.com\""
                        "CHOOSE '/a' == 1",
                        tasks));
}


  
  //   ____        _   _           _          _   _                 
  //  / __ \      | | (_)         (_)        | | (_)                
  // | |  | |_ __ | |_ _ _ __ ___  _ ______ _| |_ _  ___  _ __  ___ 
  // | |  | | '_ \| __| | '_ ` _ \| |_  / _` | __| |/ _ \| '_ \/ __|
  // | |__| | |_) | |_| | | | | | | |/ / (_| | |_| | (_) | | | \__ \
  //  \____/| .__/ \__|_|_| |_| |_|_/___\__,_|\__|_|\___/|_| |_|___/
  //        | |                                                     
  //        |_|                                                     
 
TEST_F(PlanTest, LSFileMapOptimization) {
  std::vector<std::string> before = {
      tasks::ID<load::LSListFile>::NAME,
      tasks::ID<load::LSListDir>::NAME,
      tasks::ID<load::LSFileOpener>::NAME,
      tasks::ID<load::LineSeparatedStreamReader>::NAME,
      tasks::ID<load::DefaultTextParser>::NAME,
      tasks::ID<tasks::storage::StorageReceiver>::NAME};

  std::vector<std::string> after = {
      tasks::ID<load::LSListFile>::NAME,
      tasks::ID<load::LSListDir>::NAME,
      tasks::ID<load::LSFileMapper>::NAME,
      tasks::ID<load::DefaultTextParser>::NAME,
      tasks::ID<tasks::storage::StorageReceiver>::NAME};

  std::vector<std::unique_ptr<optimization::OptimizationRule>> rules;
  rules.emplace_back(std::make_unique<optimization::FileMapOptimization>());


  EXPECT_TRUE(testOptimizationForTasks(
      "LOAD A FROM FILE \"./test/data/json/numtest.json\" LINESEPARATED, "
      "FROM FILES \"./test/data/json\" LINESEPARATED ", rules,
      before, after));
}

TEST_F(PlanTest, QueryCombinationOptimization) {
  std::vector<std::string> before = {
      tasks::ID<tasks::storage::StorageSender>::NAME,
      tasks::ID<tasks::storage::StorageReceiver>::NAME,
      tasks::ID<tasks::storage::StorageSender>::NAME,
      tasks::ID<tasks::storage::StorageReceiver>::NAME
      };

  std::vector<std::string> after = {
      tasks::ID<tasks::storage::StorageSender>::NAME,
      tasks::ID<tasks::storage::StorageBuffer>::NAME,
      tasks::ID<tasks::storage::StorageReceiver>::NAME
      };

  std::vector<std::unique_ptr<optimization::OptimizationRule>> rules;
  rules.emplace_back(std::make_unique<optimization::QueryCombinationRule>());

  std::vector<std::string> queries = {"LOAD A STORE B", "LOAD B STORE C"};

  EXPECT_TRUE(testOptimizationForTasks(
      queries, rules,
      before, after));
}

TEST_F(PlanTest, FilterAggregationOptimization) {
  std::vector<std::string> before = {
      tasks::ID<tasks::storage::StorageSender>::NAME,
      tasks::ID<tasks::choose::ChoosePipeline>::NAME,
      tasks::ID<tasks::as::AsPipeline>::NAME,
      tasks::ID<tasks::agg::AggPipeline>::NAME,
      tasks::ID<tasks::agg::AggMerger>::NAME,
      tasks::ID<tasks::storage::StorageReceiver>::NAME
      };

  std::vector<std::string> after = {
      tasks::ID<tasks::storage::StorageSender>::NAME,
      tasks::ID<tasks::choose::ChoosePipeline>::NAME,
      tasks::ID<tasks::agg::FilterAggPipeline>::NAME,
      tasks::ID<tasks::agg::AggMerger>::NAME,
      tasks::ID<tasks::storage::StorageReceiver>::NAME
      };

  std::vector<std::unique_ptr<optimization::OptimizationRule>> rules;
  rules.emplace_back(std::make_unique<optimization::FilterAggregationRule>());

  std::vector<std::string> queries = {"LOAD A CHOOSE EXISTS('') AGG ('':COUNT(''))"};

  EXPECT_TRUE(testOptimizationForTasks(
      queries, rules,
      before, after));
}

TEST_F(PlanTest, FilterAggregationOptimizationNotApplicable) {
  std::vector<std::string> before = {
      tasks::ID<tasks::storage::StorageSender>::NAME,
      tasks::ID<tasks::choose::ChoosePipeline>::NAME,
      tasks::ID<tasks::as::AsPipeline>::NAME,
      tasks::ID<tasks::agg::AggPipeline>::NAME,
      tasks::ID<tasks::agg::AggMerger>::NAME,
      tasks::ID<tasks::storage::StorageReceiver>::NAME
      };

  std::vector<std::string> after = {
      tasks::ID<tasks::storage::StorageSender>::NAME,
      tasks::ID<tasks::choose::ChoosePipeline>::NAME,
      tasks::ID<tasks::as::AsPipeline>::NAME,
      tasks::ID<tasks::agg::AggPipeline>::NAME,
      tasks::ID<tasks::agg::AggMerger>::NAME,
      tasks::ID<tasks::storage::StorageReceiver>::NAME
      };

  std::vector<std::unique_ptr<optimization::OptimizationRule>> rules;
  rules.emplace_back(std::make_unique<optimization::FilterAggregationRule>());

  std::vector<std::string> queries = {"LOAD A CHOOSE EXISTS('') AS ('':'/1') AGG ('':COUNT(''))"};

  EXPECT_TRUE(testOptimizationForTasks(
      queries, rules,
      before, after));
}

TEST_F(PlanTest, ChooseAsOptimization) {
  std::vector<std::string> before = {
      tasks::ID<tasks::storage::StorageSender>::NAME,
      tasks::ID<tasks::choose::ChoosePipeline>::NAME,
      tasks::ID<tasks::as::AsPipeline>::NAME,
      tasks::ID<tasks::storage::StorageReceiver>::NAME
      };

  std::vector<std::string> after = {
      tasks::ID<tasks::storage::StorageSender>::NAME,
      tasks::ID<tasks::optimization::ChooseAsPipeline>::NAME,
      tasks::ID<tasks::storage::StorageReceiver>::NAME
      };

  std::vector<std::unique_ptr<optimization::OptimizationRule>> rules;
  rules.emplace_back(std::make_unique<optimization::ChooseAsOptimization>());

  std::vector<std::string> queries = {"LOAD A CHOOSE EXISTS('')"};

  EXPECT_TRUE(testOptimizationForTasks(
      queries, rules,
      before, after));
}

TEST_F(PlanTest, ChooseAggOptimization) {
  std::vector<std::string> before = {
      tasks::ID<tasks::storage::StorageSender>::NAME,
      tasks::ID<tasks::choose::ChoosePipeline>::NAME,
      tasks::ID<tasks::as::AsPipeline>::NAME,
      tasks::ID<tasks::agg::AggPipeline>::NAME,
      tasks::ID<tasks::agg::AggMerger>::NAME,
      tasks::ID<tasks::storage::StorageReceiver>::NAME
      };

  std::vector<std::string> after = {
      tasks::ID<tasks::storage::StorageSender>::NAME,
      tasks::ID<tasks::optimization::ChooseAggPipeline>::NAME,
      tasks::ID<tasks::agg::AggMerger>::NAME,
      tasks::ID<tasks::storage::StorageReceiver>::NAME
      };

  std::vector<std::unique_ptr<optimization::OptimizationRule>> rules;
  rules.emplace_back(std::make_unique<optimization::FilterAggregationRule>());
  rules.emplace_back(std::make_unique<optimization::ChooseAggOptimization>());

  std::vector<std::string> queries = {"LOAD A CHOOSE EXISTS('') AGG ('':COUNT(''))"};

  EXPECT_TRUE(testOptimizationForTasks(
      queries, rules,
      before, after));
}

TEST_F(PlanTest, ChooseAsAggOptimization) {
  std::vector<std::string> before = {
      tasks::ID<tasks::storage::StorageSender>::NAME,
      tasks::ID<tasks::choose::ChoosePipeline>::NAME,
      tasks::ID<tasks::as::AsPipeline>::NAME,
      tasks::ID<tasks::agg::AggPipeline>::NAME,
      tasks::ID<tasks::agg::AggMerger>::NAME,
      tasks::ID<tasks::storage::StorageReceiver>::NAME
      };

  std::vector<std::string> after = {
      tasks::ID<tasks::storage::StorageSender>::NAME,
      tasks::ID<tasks::optimization::ChooseAsAggPipeline>::NAME,
      tasks::ID<tasks::agg::AggMerger>::NAME,
      tasks::ID<tasks::storage::StorageReceiver>::NAME
      };

  std::vector<std::unique_ptr<optimization::OptimizationRule>> rules;
  rules.emplace_back(std::make_unique<optimization::ChooseAsAggOptimization>());

  std::vector<std::string> queries = {"LOAD A CHOOSE EXISTS('') AS ('':'/1') AGG ('':COUNT(''))"};

  EXPECT_TRUE(testOptimizationForTasks(
      queries, rules,
      before, after));
}