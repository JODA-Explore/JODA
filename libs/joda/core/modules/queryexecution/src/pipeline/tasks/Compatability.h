#ifndef JODA_PIPELINE_TASKID_COMPATABILITY_H
#define JODA_PIPELINE_TASKID_COMPATABILITY_H
#include <cstdio>

#include <joda/parser/pipeline/TextParser.h>
#include <joda/parser/pipeline/FileOpener.h>
#include <joda/parser/pipeline/LSFileMapper.h>
#include <joda/parser/pipeline/LineSeparatedStreamReader.h>
#include <joda/parser/pipeline/ListFile.h>
#include <joda/parser/pipeline/ListDir.h>
#include <joda/parser/pipeline/InStream.h>
#include <joda/parser/pipeline/URLStream.h>
#include "storage/StorageBuffer.h"
#include "storage/StorageReceiver.h"
#include "storage/StorageSender.h"
#include "choose/ChoosePipeline.h"
#include "as/AsPipeline.h"
#include "agg/AggPipeline.h"
#include "agg/FilterAggPipeline.h"
#include "agg/WindowAgg.h"
#include "agg/AggMerger.h"
#include <joda/export/FileExport.h>
#include <joda/export/DirectoryExport.h>
#include <joda/export/JSONStringifier.h>
#include <joda/export/StreamExport.h>
#include <joda/join/pipeline/LoadJoin.h>
#include <joda/join/pipeline/LoadJoinFiles.h>
#include <joda/join/pipeline/LoadJoinFileParser.h>
#include <joda/join/pipeline/StoreJoin.h>
#include <joda/join/pipeline/InnerJoin.h>
#include <joda/join/pipeline/OuterJoin.h>

#ifdef JODA_ENABLE_PYTHON
#include <joda/extension/python/PythonImportTask.h>
#include <joda/extension/python/PythonExport.h>
#endif // JODA_ENABLE_PYTHON


namespace joda::queryexecution::pipeline::tasks {

/*
 *   _____                            _   _ _     _ _ _ _
 *  / ____|                          | | (_) |   (_) (_) |
 * | |     ___  _ __ ___  _ __   __ _| |_ _| |__  _| |_| |_ _   _
 * | |    / _ \| '_ ` _ \| '_ \ / _` | __| | '_ \| | | | __| | | |
 * | |___| (_) | | | | | | |_) | (_| | |_| | |_) | | | | |_| |_| |
 *  \_____\___/|_| |_| |_| .__/ \__,_|\__|_|_.__/|_|_|_|\__|\__, |
 *                       | |                                 __/ |
 *                       |_|                                |___/
 */

/*
 * TCLASSA:    Classname of Task A
 * TCLASSB:    Classname of Task B
 * Denotes that Task A can supply Task B with data
 */
#define JODA_COMPATIBLE_TASK(TCLASSA, TCLASSB) \
  if (a == ID<TCLASSA>::NAME && b == ID<TCLASSB>::NAME) return true;

struct TaskCompatability {
  static constexpr bool check(const std::string_view& a,const std::string_view& b) {
    // Load
    JODA_COMPATIBLE_TASK(load::LSListFile, load::LSFileOpener) // LS File => LS File Opener
    JODA_COMPATIBLE_TASK(load::LSListFile, load::LSFileMapper) // LS File => LS File Mapper
    JODA_COMPATIBLE_TASK(load::ListFile, load::UnformattedFileOpener) // LS File => Unformatted File Opener
    JODA_COMPATIBLE_TASK(load::LSListDir, load::LSFileOpener) // LS Dir => LS File Opener
    JODA_COMPATIBLE_TASK(load::LSListDir, load::LSFileMapper) // LS Dir => LS File Mapper
    JODA_COMPATIBLE_TASK(load::ListDir, load::UnformattedFileOpener) // LS Dir => Unformatted File Opener
    JODA_COMPATIBLE_TASK(load::LSFileOpener, load::LineSeparatedStreamReader) // LS File Opener => LS Stream Reader
    // - Text Parser
    JODA_COMPATIBLE_TASK(load::LineSeparatedStreamReader, load::DefaultTextParser) // LS Stream Reader => Default Text Parser
    JODA_COMPATIBLE_TASK(load::LSFileMapper, load::DefaultTextParser) // LSFileMapper => Default Text Parser
    JODA_COMPATIBLE_TASK(load::LineSeparatedStreamReader, load::PathJaccardSimilarityTextParser) // LS Stream Reader => Path Jaccard Similarity Text Parser
    JODA_COMPATIBLE_TASK(load::LSFileMapper, load::PathJaccardSimilarityTextParser) //  LS File Mapper => Path Jaccard Similarity Text Parser
    JODA_COMPATIBLE_TASK(load::LineSeparatedStreamReader, load::AttributeJaccardSimilarityTextParser) // LS Stream Reader => Attribute Jaccard Similarity Text Parser
    JODA_COMPATIBLE_TASK(load::LSFileMapper, load::AttributeJaccardSimilarityTextParser) //  LS File Mapper => Attribute Jaccard Similarity Text Parser
    // - Stream Parser
    JODA_COMPATIBLE_TASK(load::UnformattedFileOpener, load::DefaultStreamParser) // Unformatted File Opener => Unformatted Stream Parser
    JODA_COMPATIBLE_TASK(load::InStream, load::DefaultStreamParser) // CIN => Unformatted Stream Parser
    JODA_COMPATIBLE_TASK(load::URLStream, load::DefaultStreamParser) // URL => Unformatted Stream Parser
    JODA_COMPATIBLE_TASK(load::UnformattedFileOpener, load::PathJaccardSimilarityStreamParser) // Unformatted File Opener => Path Jaccard Similarity Stream Parser
    JODA_COMPATIBLE_TASK(load::InStream, load::PathJaccardSimilarityStreamParser) // CIN => Path Jaccard Similarity Stream Parser
    JODA_COMPATIBLE_TASK(load::URLStream, load::PathJaccardSimilarityStreamParser) // URL =>  Path Jaccard Similarity Stream Parser
    JODA_COMPATIBLE_TASK(load::UnformattedFileOpener, load::AttributeJaccardSimilarityStreamParser) // Unformatted File Opener => Attribute Jaccard Similarity Stream Parser
    JODA_COMPATIBLE_TASK(load::InStream, load::AttributeJaccardSimilarityStreamParser) // CIN => Attribute Jaccard Similarity Stream Parser
    JODA_COMPATIBLE_TASK(load::URLStream, load::AttributeJaccardSimilarityStreamParser) // URL =>  Attribute Jaccard Similarity Stream Parser

    // Choose
    JODA_COMPATIBLE_TASK(choose::ChoosePipeline, agg::FilterAggPipeline) //Choose => FilterAgg
    // See groups for container related

    // As
    JODA_COMPATIBLE_TASK(choose::ChoosePipeline, as::AsPipeline) //Choose => As
    
    // See groups for container related

    // Agg
    JODA_COMPATIBLE_TASK(agg::AggPipeline, agg::AggMerger) // Agg => Agg Merger
    JODA_COMPATIBLE_TASK(agg::FilterAggPipeline, agg::AggMerger) // FilterAgg => Agg Merger

    // See groups for container related

    // Store
    // See groups for container related
    JODA_COMPATIBLE_TASK(store::JSONStringifier, store::WriteFile) // JSON Stringifier => Write File
    JODA_COMPATIBLE_TASK(store::JSONStringifier, store::WriteFiles) // JSON Stringifier => Write Dir
    JODA_COMPATIBLE_TASK(store::JSONStringifier, store::WriteOutStream) // Write Out Stream => Write Dir

    // Join
    // See groups for container related
    JODA_COMPATIBLE_TASK(join::LoadJoinFiles, join::LoadJoinFileParser) // Load Join Files => Load Join File Parser

    // GROUPS
    // All container senders are compatible with all container receivers
    if(belongsToGroup(a, ContainerSenderGroup) && belongsToGroup(b, ContainerReceiverGroup)) return true;


    return false;
  }

  // Groups
  static constexpr auto ContainerReceiverGroup = 1;
  static constexpr auto ContainerSenderGroup = 2;

  /*
 * TCLASSA:    Classname of Task A
 * GROUP:   The Group
 * Denotes that Task A belongs to GROUP
 */
#define JODA_TASK_GROUP(TCLASSA, GROUP) \
  if (group == GROUP && task == ID<TCLASSA>::NAME ) return true;

  static constexpr bool belongsToGroup(const std::string_view& task, const int group) {
    // --- Container Sender ---
    JODA_TASK_GROUP(storage::StorageBuffer, ContainerSenderGroup)
    JODA_TASK_GROUP(storage::StorageSender, ContainerSenderGroup) // Sender => Send
    // Parser
    JODA_TASK_GROUP(load::DefaultStreamParser, ContainerSenderGroup) // Default Stream Parser => Send
    JODA_TASK_GROUP(load::PathJaccardSimilarityTextParser, ContainerSenderGroup) // Path Jaccard Similarity Text Parser => Send
    JODA_TASK_GROUP(load::AttributeJaccardSimilarityTextParser, ContainerSenderGroup) // Attribute Jaccard Similarity Text Parser => Send
    JODA_TASK_GROUP(load::DefaultTextParser, ContainerSenderGroup) // Default Text Parser => Send
    JODA_TASK_GROUP(load::PathJaccardSimilarityStreamParser, ContainerSenderGroup) // Path Jaccard Similarity Stream Parser => Send
    JODA_TASK_GROUP(load::AttributeJaccardSimilarityStreamParser, ContainerSenderGroup) // Attribute Jaccard Similarity Stream Parser => Send
    // CHOOSE/AS/AGG
    JODA_TASK_GROUP(as::AsPipeline, ContainerSenderGroup) // AS => Send
    JODA_TASK_GROUP(agg::AggMerger, ContainerSenderGroup) // Agg Merger => Send
    JODA_TASK_GROUP(agg::WindowAgg, ContainerSenderGroup) // Window Agg => Send
    // Join
    JODA_TASK_GROUP(join::LoadJoin, ContainerSenderGroup) // Join Load => Send
    JODA_TASK_GROUP(join::LoadJoinFileParser, ContainerSenderGroup) // Load Join File Parser => Send
    JODA_TASK_GROUP(join::OuterJoin, ContainerSenderGroup) // Join Outer => Send
    // Modules
    #ifdef JODA_ENABLE_PYTHON
    JODA_TASK_GROUP(load::PythonImport, ContainerSenderGroup) // Python Import => Send
    #endif // JODA_ENABLE_PYTHON

    // --- Container Receiver ---
    JODA_TASK_GROUP(storage::StorageBuffer, ContainerReceiverGroup)
    JODA_TASK_GROUP(storage::StorageReceiver, ContainerReceiverGroup) // Receiver => Receive
    // CHOOSE/AS/AGG
    JODA_TASK_GROUP(choose::ChoosePipeline, ContainerReceiverGroup) // Choose => Receive
    JODA_TASK_GROUP(agg::AggPipeline, ContainerReceiverGroup) // Agg => Receive
    JODA_TASK_GROUP(agg::FilterAggPipeline, ContainerReceiverGroup) // FilterAgg => Receive
    JODA_TASK_GROUP(agg::WindowAgg, ContainerReceiverGroup) // Window Agg => Receive
    // Stringify
    JODA_TASK_GROUP(store::JSONStringifier, ContainerReceiverGroup) // JSON Stringifier => Receive
    // Join
    JODA_TASK_GROUP(join::StoreJoin, ContainerReceiverGroup) // Join Store => Receive
    JODA_TASK_GROUP(join::InnerJoin, ContainerReceiverGroup) // Join Inner => Receive
    JODA_TASK_GROUP(join::OuterJoin, ContainerReceiverGroup) // Join Outer => Receive
    // Modules
    #ifdef JODA_ENABLE_PYTHON
    JODA_TASK_GROUP(store::PythonExporter, ContainerReceiverGroup) // Python Exporter => Receive
    #endif // JODA_ENABLE_PYTHON
    
    return false;
  }

};


/*
*   _____                  _                               _   
*  |  __ \                (_)                             | |  
*  | |__) |___  __ _ _   _ _ _ __ ___ _ __ ___   ___ _ __ | |_ 
*  |  _  // _ \/ _` | | | | | '__/ _ \ '_ ` _ \ / _ \ '_ \| __|
*  | | \ \  __/ (_| | |_| | | | |  __/ | | | | |  __/ | | | |_ 
*  |_|  \_\___|\__, |\__,_|_|_|  \___|_| |_| |_|\___|_| |_|\__|
*                 | |                                          
*                 |_|                                          
*/

/*
 * TCLASSA:    Classname of Task A
 * TCLASSB:    Classname of Task B
 * Denotes that Task A requires Task B to be present AFTER it
 */
#define JODA_REQUIRED_TASK(TCLASSA, TCLASSB) \
  if (t == ID<TCLASSA>::NAME) return ID<TCLASSB>::NAME;

struct TaskRequirement {
  static std::string get(const std::string& t) {

    // --- Load ---
    // Lineseparated
    JODA_REQUIRED_TASK(load::LSListFile, load::LSFileOpener)
    JODA_REQUIRED_TASK(load::LSListDir, load::LSFileOpener)
    JODA_REQUIRED_TASK(load::LSFileOpener, load::LineSeparatedStreamReader) // LS File Opener => LS Stream Reader
    // Unformatted
    JODA_REQUIRED_TASK(load::ListFile, load::UnformattedFileOpener)
    JODA_REQUIRED_TASK(load::ListDir, load::UnformattedFileOpener)
    JODA_REQUIRED_TASK(load::UnformattedFileOpener, load::DefaultStreamParser) // Unformatted File Opener => Unformatted Stream Parser
    // Stream
    JODA_REQUIRED_TASK(load::InStream, load::DefaultStreamParser) // CIN => Unformatted Stream Parser
    JODA_REQUIRED_TASK(load::URLStream, load::DefaultStreamParser) // URL => Unformatted Stream Parser
    JODA_REQUIRED_TASK(load::LineSeparatedStreamReader, load::DefaultTextParser) // LS Stream Reader => Default Text Parser

    // --- Choose ---
    JODA_REQUIRED_TASK(choose::ChoosePipeline, as::AsPipeline) // Choose => As

    // --- Agg ---
    JODA_REQUIRED_TASK(agg::AggPipeline, agg::AggMerger) // Agg => Agg Merger
    JODA_REQUIRED_TASK(agg::FilterAggPipeline, agg::AggMerger) // FilterAgg => Agg Merger

    // --- Join ---
    JODA_REQUIRED_TASK(join::LoadJoinFiles, join::LoadJoinFileParser) // Join Load => Load Join File Parser


    return "";
  }
};

/*
 * TCLASSA:    Classname of Task A
 * TCLASSB:    Classname of Task B
 * Denotes that Task A requires Task B to be present BEFORE it
 */
#define JODA_PREREQUIRED_TASK(TCLASSA, TCLASSB) \
  if (t == ID<TCLASSA>::NAME) return ID<TCLASSB>::NAME;

struct TaskPrerequisite {
  static std::string get(const std::string& t) {

    // --- Export ---
    JODA_PREREQUIRED_TASK(store::WriteFile, store::JSONStringifier) // JSON Stringifier => Write File
    JODA_PREREQUIRED_TASK(store::WriteFiles, store::JSONStringifier) // JSON Stringifier => Write Dir
    JODA_PREREQUIRED_TASK(store::WriteOutStream, store::JSONStringifier) //  JSON Stringifier => Write Out Stream

    return "";
  }
};

}  // namespace joda::queryexecution::pipeline::tasks

#endif  // JODA_PIPELINE_TASKID_COMPATABILITY_H