//
// Created by Nico Schäfer on 02/04/18.
//

#ifndef JODA_FILEJOINMANAGER_H
#define JODA_FILEJOINMANAGER_H

#include <unordered_map>
#include "JoinManager.h"

/**
 * The FileJoinManager is responsible for joining/grouping documents using files on the filesystem
 */
class FileJoinManager : public JoinManager{
 public:
  /**
   * Initializes the FileJoinManager and creates an temporary directory to store temporary files within
   * @param name Name/Identifier of the join. It is used to create the directory.
   * @param valProv The IValueProvider to receive the join values from.
   */
  FileJoinManager(const std::string &name, std::unique_ptr<joda::query::IValueProvider>&& valProv);
  /**
   * Deletes the temporary directory created when initializing the class
   */
  ~FileJoinManager() override;

  /**
   * Joins a single document with the others
   * @param doc The RapidJsonDocument to join
   */
  void join(const RapidJsonDocument& doc) override;

  /**
   * Returns the temporary directory used fo the joins.
   * @return the temporary directory used fo the joins.
   */
  const std::string &getBaseDir() const;
 protected:


  bool createFile(const std::string& file,bool val);
  bool createFile(const std::string& file,long val);
  bool createFile(const std::string& file,std::string val);
  std::string getFile(JODA_JOINID id);
  JODA_JOINID getJoinVal(const RJValue& val);
  bool appendDocToFile(const RapidJsonDocument &doc, const std::string &file);


  std::string baseDir;
  std::mutex mut;


  JODA_JOINID currID = STARTNORMAL_VAL;
  std::unordered_map<long,JODA_JOINID> lMap; //Integer as long
  std::unordered_map<std::string,JODA_JOINID> sMap; //String as String
  std::unordered_map<JODA_JOINID,std::mutex> fLock; //file-lock

  bool trueValCreated = false;
  bool falseValCreated = false;
};

#endif //JODA_FILEJOINMANAGER_H
