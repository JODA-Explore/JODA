//
// Created by Nico Schäfer on 3/22/17.
//

#include "joda/config/config.h"

//Indices
bool config::queryCache = true;

//Bloom
bool config::bloom_enabled = true;
uint config::bloom_count = 1000; // How many elements roughly do we expect to insert?
double config::bloom_prob = 0.001;// Maximum tolerable false positive probability? (0,1)

//Directories
std::string config::home = std::string();
std::string config::tmpdir = std::string();

//Storage
bool config::storeJson = true;
unsigned long long config::maxmemory = 0;
config::EvictionStrategies config::evictionStrategy = NO_EVICTION;

//Benchmark
bool config::benchmark = false;
std::string config::benchfile = std::string();

//Containers
size_t config::JSONContainerSize = 0;
double config::chunk_size = 0;
double config::text_binary_mod = 0;

//Multithreading
size_t config::storageRetrievalThreads = 0;//JSON_STORAGE_DEFAULT_THREADS;
size_t config::readingThreads = 0;//JSON_STORAGE_DEFAULT_THREADS;
size_t config::parsingThreads = 0;//JSON_STORAGE_DEFAULT_THREADS;

//Parsing
size_t config::read_bulk_size = 0;
std::string config::read_reader = std::string();
size_t config::parse_bulk_size = 0;

//Similarity
size_t config::sim_min_cont_size = 0;
double config::sim_min_similarity = 0;
config::Sim_Measures config::sim_measure = NO_SIMILARITY;
bool config::sim_merge_on_parse = false;


//History
std::string config::history_file = "";
bool config::persistent_history = false;
int config::history_size;

//CLI
bool config::disable_interactive_CLI = false;

//View
bool config::enable_views = false;
bool config::enable_views_vo = false;
