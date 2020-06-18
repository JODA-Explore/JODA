//
// Created by Nico on 13/03/2019.
//

#ifndef JODA_INTERFACE_H
#define JODA_INTERFACE_H

#include "CursesFWD.h"
#include <mutex>
#include <joda/storage/JSONStorage.h>
#include <joda/misc/ProgressBar.h>
#include <curses.h>

namespace joda::cli {
class ResultInterface {
 public:
  explicit ResultInterface();
  virtual ~ResultInterface();


  void interact();
  void updateStorage(std::shared_ptr<JSONStorage>& storage);

  void setStatus1(std::string& str);
  void setStatus2(std::string& str);
  void setStatus3(std::string& str);
 private:

  std::vector<std::string> cache{};

  std::shared_ptr<JSONStorage> storage;

  size_t storageSize = 0;

  int maxRows, maxCols=0;
  /*
   * Windows
   */
  WINDOW * status= nullptr;
  WINDOW * status_1= nullptr;
  WINDOW * status_2= nullptr;
  WINDOW * status_3= nullptr;

  WINDOW * results= nullptr;
  WINDOW * resultborder= nullptr;
  int maxResults;
  int maxLineLen;
  size_t currResult=0;
  size_t minResultI=0;
  size_t maxResultI=0;
  void scrollResults(bool down=true);

  void buildStatusBar();
  void createWindows();
  void refreshAll();
  void destroyWindows();
  void destroyWindow(WINDOW *local_win);
  void updateContent();

  void initCache();
  void addCache(size_t i, bool back);

  void forceRefresh();
  void startJSONViewer() const;

  static std::wstring getWString(std::string& str);
};
}

#endif //JODA_INTERFACE_H
