//
// Created by Nico Schäfer on 08/01/18.
//

#ifndef JODA_DIRECTORYREGISTER_H
#define JODA_DIRECTORYREGISTER_H

#include <filesystem>
#include <unordered_set>
#include <mutex>
#include <boost/optional.hpp>

namespace fs = std::filesystem;

namespace joda::filesystem {
/**
 * An exception signaling that a directory is already registered in the given register
 */
class DirectoryAlreadyInUseException : std::exception {

};

/**
 * This class manages the lifetime of (temporary) directories.
 * All registered directories are deleted on deletion of the register
 */
class DirectoryRegister {
 public:

  static DirectoryRegister &getInstance() {
    static DirectoryRegister instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
  }

  DirectoryRegister(DirectoryRegister const &) = delete;
  void operator=(DirectoryRegister const &) = delete;

  /**
   * Deletes all directories on deletion of the Register
   */
  virtual ~DirectoryRegister();

  //Register new directory
  /**
   * Creates and registers a directory
   * @param dir The path to the directory
   * @param checkDuplicate If true, the function fails if the chosen directory was already created
   * @return False if a duplicate was found and checkDuplicate is True or another error occurs; True else
   */
  bool registerDirectory(const std::string& dir, bool checkDuplicate);
  /**
   * Creates and registers a directory
   * @param dir The path to the directory
   * @param checkDuplicate If true, the function fails if the chosen directory was already created
   * @return False if a duplicate was found and checkDuplicate is True or another error occurs; True else
   */
  bool registerDirectory(const fs::path& dir, bool checkDuplicate);

  /**
   * Deletes a registered directory
   * @param dir The directory to delete
   * @return True if the directory was successfully deleted; False if an error occured
   */
  bool removeDirectory(const std::string& dir);
  /**
   * Deletes a registered directory
   * @param dir The directory to delete
   * @return True if the directory was successfully deleted; False if an error occured
   */
  bool removeDirectory(const fs::path& dir);


  /**
   * Creates a new named directory in the configured temporary directory
   * @param name The name of the directory to be created
   * @return The path to the directory if successful; Nothing else
   */
  boost::optional<std::string> getTMPDir(const std::string &name);
  /**
   * Returns an arbitrary unique directory path.
   * This directory is not yet registered or created.
   * The uniqueness is only ensured per system run.
   * @return A path to a unique directory.
   */
  std::string getUniqueDir();
 protected:
  bool createDir(const fs::path &dir, bool checkExisting = false);
  bool removeDir(const fs::path& dir);
  std::unordered_set<std::string> dirs {};
  unsigned long currID = 0;
  std::mutex mut;

 private:
  DirectoryRegister() {}

};
}



#endif //JODA_DIRECTORYREGISTER_H
