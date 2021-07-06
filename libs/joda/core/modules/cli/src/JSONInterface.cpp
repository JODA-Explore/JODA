//
// Created by Nico on 13/03/2019.
//

#include "JSONInterface.h"
#include <glog/logging.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <codecvt>
#include <locale>
#include "CursesHandler.h"

joda::cli::JSONInterface::JSONInterface() { createWindows(); }

joda::cli::JSONInterface::~JSONInterface() { destroyWindows(); }

void joda::cli::JSONInterface::createWindows() {
  refresh();
  getmaxyx(stdscr, maxY, maxX);
  jsonwin = newpad(1000, maxX);
  keypad(jsonwin, TRUE);  // jsonwin window can receive KEY_UP ..
  bg = newwin(maxY, maxX, 0, 0);
  wborder(bg, '|', '|', '-', '-', '+', '+', '+', '+');
  touchwin(bg);
  refreshAll();
}

void joda::cli::JSONInterface::refreshAll() {
  wrefresh(bg);
  prefresh(jsonwin, y, 0, 1, 1, maxY - 2, maxX - 2);
}

void joda::cli::JSONInterface::destroyWindows() {
  destroyWindow(jsonwin);
  destroyWindow(bg);
}

void joda::cli::JSONInterface::destroyWindow(WINDOW* local_win) {
  if (local_win == nullptr) {
    return;
  }
  wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  wrefresh(local_win);
  delwin(local_win);
}

void joda::cli::JSONInterface::browse() {
  bool exec = true;
  while (exec) {
    auto ch = wgetch(jsonwin);
    switch (ch) {
      case KEY_UP:
        y = std::max(0, y - 1);
        refreshAll();
        break;
      case KEY_DOWN:
        y++;
        refreshAll();
        break;
      case 'q':
        exec = false;
        break;
    }
  }
}

std::wstring joda::cli::JSONInterface::getWString(std::string& str) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>> cv;

  std::wstring wstr = cv.from_bytes(str);
  return wstr;
}

void joda::cli::JSONInterface::showJSON(
    const std::shared_ptr<JSONStorage>& store, size_t index) {
  CursesPrettyHandler writer(jsonwin);
  werase(jsonwin);
  store->AcceptDocuments(writer, index, index);
  refreshAll();
  browse();
}
