//
// Created by Nico Schäfer on 11/22/17.
//

#ifndef JODA_DIRECTORYFILESCANNER_H
#define JODA_DIRECTORYFILESCANNER_H
#include <string>
#include <vector>

namespace joda::filesystem {
/**
 * This class scans a directory on the filesystem and returns a list of all files within
 */
class DirectoryFileScanner {
 public:
  /**
   * Returns all files in the given directory
   * @param dir The directory to scan
   * @return A list of filepaths in string representation
   */
  std::vector<std::string> getFilesInDir(const std::string &dir);

  /**
   * Returns all files in the given directory with the provided file extension
   * @param dir The directory to scan
   * @param ending The file extension (without "."). If ending is empty, all files will be returned
   * @return  A list of filepaths in string representation
   */
  std::vector<std::string> getFilesInDir(const std::string &dir, const std::string &ending);
  /**
   * Checks if a given file path is a valid file
   * @param file The file path to check
   * @return True if the file is valid; False else
   */
  bool fileIsValid(const std::string &file);

  /**
   * Checks if a given file path has the provided file extension
   * @param file The file path to check
   * @param ending The ending to check against
   * @return True if the file has the given extension; False else
   */
  bool hasEnding(const std::string &file, const std::string &ending);
  /**
   * Returns the size (in byte) of the given file path.
   * @param file The file path to check
   * @return The size of the file if it is valid; 0 if not
   */
  size_t getFileSize(const std::string& file) const;

  /**
   * Returns the size (in byte) of a given directory.
   * @param dir The directory to calculate the size of
   * @param recursive Decides if subdirectories are traversed recursivley
   * @param extension An optional file extension to filter the found files. If empty all files are counted
   * @return The size of the directory if the path was valid; 0 else
   */
  size_t getDirectorySize(const std::string &dir, bool recursive, const std::string &extension) const;

};
}

#endif //JODA_DIRECTORYFILESCANNER_H
