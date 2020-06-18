//
// Created by Nico on 13/03/2019.
//

#ifndef JODA_JSONINTERFACE_H
#define JODA_JSONINTERFACE_H

#include "CursesFWD.h"
#include <joda/misc/RJFwd.h>
#include <memory>
#include <curses.h>

namespace joda::cli {
/**
 * This class is resonsible for displaying JSON files in interactive CLI mode.
 */
class JSONInterface {
 public:
  /**
   * Initializes the ncurses windows
  */
  explicit JSONInterface();

  /**
   * Deconstructs the ncurses windows
  */
  virtual ~JSONInterface();

/**
 * Displays a single JSON document.
 * @param json The document to display
 */
  void showJSON(std::shared_ptr<RJDocument>& json);
 private:

  void browse();
  WINDOW* jsonwin;
  WINDOW* bg;

  int y = 0;
  int maxX = 0;
  int maxY = 0;

  void createWindows();
  void refreshAll();
  void destroyWindows();
  void destroyWindow(WINDOW *local_win);
  static std::wstring getWString(std::string& str);
};
}

#endif //JODA_JSONINTERFACE_H
