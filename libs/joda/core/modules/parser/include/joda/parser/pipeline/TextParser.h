#ifndef JODA_PIPELINE_TEXTPARSER_H
#define JODA_PIPELINE_TEXTPARSER_H

#include <joda/parser/JSONPipelineStreamParser.h>
#include <joda/parser/JSONPipelineTextParser.h>
#include <joda/parser/ParserIntermediates.h>
#include <joda/pipelineatomics/Async.h>
#include <joda/pipelineatomics/ExecutorTemplate.h>
#include <joda/pipelineatomics/TaskID.h>
#include <joda/pipelineatomics/queue/PipelineIOQueue.h>
#include <joda/similarity/SimilarityScheduler.h>
#include <joda/similarity/measures/AttributeJaccard.h>
#include <joda/similarity/measures/PathJaccard.h>

#include <memory>

namespace joda::queryexecution::pipeline::tasks::load {

typedef IOExecutor<
    docparsing::JSONPipelineTextParser<>, tasks::AsyncType::MultiThreaded>
    DefaultTextParser;
typedef IOExecutor<
    docparsing::JSONPipelineTextParser<SimilarityScheduler<AttributeJaccard>>, tasks::AsyncType::MultiThreaded>
    AttributeJaccardSimilarityTextParser;
typedef IOExecutor<
    docparsing::JSONPipelineTextParser<SimilarityScheduler<PathJaccard>>, tasks::AsyncType::MultiThreaded>
   PathJaccardSimilarityTextParser;

typedef IOExecutor<
    docparsing::JSONPipelineStreamParser<>, tasks::AsyncType::MultiThreaded>
    DefaultStreamParser;
typedef IOExecutor<
    docparsing::JSONPipelineStreamParser<SimilarityScheduler<AttributeJaccard>>, tasks::AsyncType::MultiThreaded>
    AttributeJaccardSimilarityStreamParser;
typedef IOExecutor<
    docparsing::JSONPipelineStreamParser<SimilarityScheduler<PathJaccard>>, tasks::AsyncType::MultiThreaded>
   PathJaccardSimilarityStreamParser;

}  // namespace joda::queryexecution::pipeline::tasks::load

JODA_REGISTER_PIPELINE_TASK_IN_NS(DefaultTextParser, "DefaultTextParser",
                                  joda::queryexecution::pipeline::tasks::load)
JODA_REGISTER_PIPELINE_TASK_IN_NS(AttributeJaccardSimilarityTextParser, "AttributeJaccardSimilarityTextParser",
                                  joda::queryexecution::pipeline::tasks::load)
JODA_REGISTER_PIPELINE_TASK_IN_NS(PathJaccardSimilarityTextParser, "PathJaccardSimilarityTextParser",
                                  joda::queryexecution::pipeline::tasks::load)


JODA_REGISTER_PIPELINE_TASK_IN_NS(DefaultStreamParser, "DefaultStreamParser",
                                  joda::queryexecution::pipeline::tasks::load)
JODA_REGISTER_PIPELINE_TASK_IN_NS(AttributeJaccardSimilarityStreamParser, "AttributeJaccardSimilarityStreamParser",
                                  joda::queryexecution::pipeline::tasks::load)
JODA_REGISTER_PIPELINE_TASK_IN_NS(PathJaccardSimilarityStreamParser, "PathJaccardSimilarityStreamParser",
                                  joda::queryexecution::pipeline::tasks::load)
#endif  // JODA_PIPELINE_TEXTPARSER_H