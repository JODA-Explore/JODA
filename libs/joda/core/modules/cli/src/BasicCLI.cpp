#include "joda/cli/BasicCLI.h"
#include <joda/queryparsing/QueryParser.h>
#include <unistd.h>
#include <iostream>

namespace joda::cli {

  BasicCLI::BasicCLI(const std::vector<std::string> &onceQueries): initial_queries(onceQueries), benchmark(config::benchfile){  }

  void BasicCLI::start()  {
    if(isInteractive() && initial_queries.empty()) {
      DLOG(INFO) << "TTY attached and no queries given, starting complex CLI";
      startInteractiveCLI();
      return;
    } 

    if(initial_queries.empty()){
      std::cerr << "No queries given and no TTY attached. Exiting." << std::endl;
      return;
    }
    DLOG(INFO) << "Using basic CLI with streaming enabled";
    // Enable stream features of JODA
    config::enable_streams = true;

    // Parse queries
    std::vector<std::shared_ptr<query::Query>> parsed_queries;
    queryparsing::QueryParser qp;
    for (const auto &q : initial_queries) {
      auto parsed_q = qp.parse(q);
      if(parsed_q == nullptr){
        LOG(ERROR) << "Could not parse query '" << q << "': " << qp.getLastError();
        std::cerr << "Could not parse query '" << q << "': " << qp.getLastError() << std::endl;
        return;
      }
      parsed_queries.push_back(std::move(parsed_q));
    }

    // Plan Queries
    if(config::enable_multi_query){
      queryexecution::PipelineQueryPlan planer;
      planer.createPlan(parsed_queries);

      // TODO optimize without output

      // Execute queries
      planer.executeAndGetResult(&benchmark);
      benchmark.finishLine();
    }else{
      for(auto& q : parsed_queries){
        auto queryName = q->toString();
        benchmark.addValue("Query", queryName);
        Timer timer;

        queryexecution::PipelineQueryPlan planer;
        planer.createPlan(q);
        planer.executeAndGetResult(&benchmark);

        timer.stop();
        timer.log("Query", "MAIN");
        benchmark.addValue(Benchmark::RUNTIME, "Query", timer.durationSeconds());
        benchmark.finishLine();
      }
    }


  }

  bool BasicCLI::isInteractive() const{
    // Check wether or not a TTY is attached to stdin
    return isatty(STDIN_FILENO) ;
  }

  void  BasicCLI::startInteractiveCLI() const{
    CLI cli;
    cli.start();
  }

}  // namespace joda::cli