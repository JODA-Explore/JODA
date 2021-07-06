//
// Created by Nico Schäfer on 9/19/17.
//

#ifndef JODA_BENCHMARK_H
#define JODA_BENCHMARK_H

#include <glog/logging.h>
#include <rapidjson/document.h>
#include <cassert>
#include <filesystem>
#include <iomanip>
#include <mutex>
#include <string>
#include <vector>
#include "RJFwd.h"
namespace fs = std::filesystem;

/**
 * The Benchmark class is mainly used to manage the measured execution times of
 * queries. But additional information can also be stored in the benchmark
 * class.
 *
 * The benchmark file is a JSON document where the top-level entry is an array.
 * This array then contains one benchmark "line" for each query execution.
 */
class Benchmark {
 public:
  /**
   * Initializes an in-memory benchmark document.
   * This can be used to display collected informations in CLI
   */
  Benchmark();

  /**
   * Initializes an benchmark document on the filesystem.
   * If the file already exists it is parsed into a JSON document.
   * If this is not a valid benchmark file, an error will be logged and the
   * benchmark will only be in-memory instead.
   * @param file The path to the file, where the benchmark should be stored.
   */
  explicit Benchmark(const std::string &file);
  Benchmark(const Benchmark &b) = delete;
  Benchmark &operator=(const Benchmark &b) = delete;
  Benchmark(Benchmark &&b) = default;
  Benchmark &operator=(Benchmark &&b) = default;

  /**
   * If the benchmark is stored on the filesystem, the file will be flushed and
   * closed. If not, all data is simply discarded.
   */
  virtual ~Benchmark();

  /**
   * Adds a value of type T at the JSON path "/<header>"
   * @param header The attribute name to be stored
   * @param val The value to be stored
   */
  template <typename T>
  void addValue(const std::string &header, T &&val) {
    return addValueAt("/" + header, val);
  }

  /**
   * Adds a value of type T at the JSON path "<prefix>/<header>"
   * @param prefix The attribute prefix to be stored
   * @param header The attribute name to be stored
   * @param val The value to be stored
   */
  template <typename T>
  void addValue(const std::string &prefix, const std::string &header, T &&val) {
    return addValueAt(prefix + "/" + header, val);
  }

  /**
   * Adds a value of type T at the JSON pointer (in string representation)
   * "pointer"
   * @param pointer The pointer where the value will be stored
   * @param val The value to be stored
   */
  template <typename T>
  void addValueAt(const std::string &pointer, T &&val) {
    if (!valid) return;
    RJPointer ptr(pointer.c_str());
    return addValueAt(ptr, val);
  }

  /**
   * Adds a value of type T at the JSON pointer (in string representation)
   * "pointer"
   * @param pointer The pointer where the value will be stored
   * @param val The value to be stored
   */
  template <typename T>
  void addValueAt(const RJPointer &pointer, T &&val) {
    if (!valid) return;
    std::lock_guard<std::mutex> lock(mut);
    DCHECK(currentLine.IsObject());
    pointer.Set(currentLine, val, benchDoc.GetAllocator());
  }

  /**
   * Adds a new entry in the "thread" attribute with the given values
   */
  void addThread(double bloom, double select, double project, double agg,
                 double copy, double serialize, double sample_view_cost);

  /**
   * Completes, and optionally flushes, one line in the benchmark
   * array-document. A new line is created for further use.
   */
  void finishLine();

  /**
   * Stringifies the current benchmark file with all lines and returns it.
   * @return The benchmark file as string representation
   */
  std::string toString() const;

  /**
   * Stringifies the last benchmark line and returns it.
   * If no line exists before the current one, an empty string is returned.
   * @return The last benchmark line as string representation
   */
  std::string lastLineToString() const;

  /**
   * Returns the last benchmark line
   * If no line exists before the current one, a null value is returned.
   * @return The last benchmark line as RJValue
   */
  const RJValue &getLastLine() const;

  /**
   * Stringifies the current benchmark line and returns it.
   * @return The current benchmark line as string representation
   */
  std::string currentLineToString() const;

  /**
   * Checks whether the benchmark is in a valid format.
   * If not, all operations on it are no-ops
   * @return True if it is valid, false else.
   */
  bool isValid() const;

 protected:
  std::mutex mut;
  RJDocument benchDoc;
  RJValue currentLine;
  bool valid = false;
  bool local = true;
  fs::path benchfile;

  void parseBenchfile();
  void writeBenchfile() const;

 public:
  /*
   * prefixes
   */
  static auto constexpr RUNTIME = "/Runtime";
  static auto constexpr THREADS = "/Runtime/Threads";
};

#endif  // JODA_BENCHMARK_H
