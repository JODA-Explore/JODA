//
// Created by Nico Schäfer on 3/22/17.
//

#ifndef JODA_CONFIG_H

#define JODA_CONFIG_H


#define JSON_CONTAINER_DEFAULT_SIZE (50*1024*1024) //50MB
#define JSON_STORAGE_DEFAULT_THREADS std::max(1u,std::thread::hardware_concurrency()-1)

#include <string>
#include <istream>


class config {
 public:
  //Indices
  static bool queryCache;

  //Bloom
  static bool bloom_enabled;
  static uint bloom_count;  // How many elements roughly do we expect to insert?
  static double bloom_prob; // Maximum tolerable false positive probability? (0,1)

  //Directories
  static std::string home;
  static std::string tmpdir;

  //Storage
  static bool storeJson;
  static unsigned long long maxmemory;
  enum EvictionStrategies { NO_EVICTION, LARGEST, LRU, FIFO, DEPENDENCIES, EXPLORER };
  static EvictionStrategies evictionStrategy; //Which eviction strategy to use

  //Benchmark
  static bool benchmark;
  static std::string benchfile;

  //Containers
  static size_t JSONContainerSize;
  static double chunk_size;
  static double text_binary_mod;

  //Multithreading
  static size_t storageRetrievalThreads;
  static size_t parsingThreads;
  static size_t readingThreads;

  //Parsing
  static size_t read_bulk_size;
  static std::string read_reader;
  static size_t parse_bulk_size;

  //Similarity
  static size_t sim_min_cont_size; //Minimum container size (smaller will be merged)
  static double sim_min_similarity; //Minimum similarity required to merge documents
  enum Sim_Measures { NO_SIMILARITY, PATH_JACCARD, ATTRIBUTE_JACCARD };

  static Sim_Measures sim_measure; //Which similarity measure to use
  static bool sim_merge_on_parse;


  //History
  static std::string history_file;
  static bool persistent_history;
  static int history_size;

  //CLI
  static bool disable_interactive_CLI;

  //View
  static bool enable_views;
  static bool enable_views_vo;


};


inline std::istream& operator>>(std::istream& in, config::Sim_Measures& unit)
{
  std::string token;
  in >> token;
  if (token == "NO_SIMILARITY")
    unit = config::NO_SIMILARITY;
  else if (token == "PATH_JACCARD")
    unit = config::PATH_JACCARD;
  else if (token == "ATTRIBUTE_JACCARD")
    unit = config::ATTRIBUTE_JACCARD;
  else
    in.setstate(std::ios_base::failbit);
  return in;
}

inline std::istream &operator>>(std::istream &in, config::EvictionStrategies &eviction) {
  std::string token;
  in >> token;
  if (token == "NO_EVICTION")
    eviction = config::NO_EVICTION;
  else if (token == "LARGEST")
    eviction = config::LARGEST;
  else if (token == "LRU")
    eviction = config::LRU;
  else if (token == "FIFO")
    eviction = config::FIFO;
  else if (token == "DEPENDENCIES")
    eviction = config::DEPENDENCIES;
  else if (token == "EXPLORER")
    eviction = config::EXPLORER;
  else
    in.setstate(std::ios_base::failbit);
  return in;
}

#endif //JODA_CONFIG_H
