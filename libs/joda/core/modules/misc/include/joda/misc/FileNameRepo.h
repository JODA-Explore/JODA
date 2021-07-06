//
// Created by Nico Schäfer on 29/12/17.
//

#ifndef JODA_FILENAMEREPO_H
#define JODA_FILENAMEREPO_H
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
typedef unsigned long FILEID;
/**
 * The FileNameRepo class is used as a central mapping between string filenames
 * and unique numerical ids for efficient filename storage.
 */
class FileNameRepo {
 public:
  /**
   * The ConstFileNames are reserved file ids for internal usage.
   */
  enum ConstFileNames {
    TMPObjectName = 0,
    ProjectionFileName = 1,
    ValidFileNameStart = 2
  };

  /**
   * Adds a (new) file to he file->ID mappings.
   * If the file was already mapped, the same id is returned as previously.
   * @param file The path to the file
   * @return the mapped file ID
   */
  FILEID addFile(const std::string& file);

  /**
   * Retrieves a file path previously mapped to an ID.
   *
   * If a reserved id is given, a string representation of this reserved keyword
   * is returned. If the id does not exist an emtpy string is returned.
   *
   * @param id The id to translate into a file path
   * @return the translated file path
   */
  std::string getFile(FILEID id);

  /**
   * Retrieves multiple file paths previously mapped to IDs.
   *
   * If a reserved id is given, a string representation of this reserved keyword
   * is returned. If the id does not exist an emtpy string is returned.
   *
   * @param ids The ids to translate into a file path
   * @return the translated file paths
   */
  std::vector<std::string> getFiles(const std::unordered_set<FILEID>& ids);

  /**
   * Checks if the given id is in a valid range.
   *
   * @param id The id to check
   * @return True if the id should point to a file.
   */
  static constexpr bool validFile(FILEID id) {
    return id >= ValidFileNameStart;
  }

 protected:
  std::mutex mut;
  FILEID getNextID();
  std::unordered_map<FILEID, std::string> id_file;
  std::unordered_map<std::string, FILEID> file_id;
  FILEID currID = 1;
};

extern FileNameRepo g_FileNameRepoInstance;

#endif  // JODA_FILENAMEREPO_H
