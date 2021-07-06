#include <utility>

//
// Created by Nico on 13/03/2019.
//

#include <codecvt>
#include "CursesFWD.h"
#include "JSONInterface.h"
#include "ResultInterface.h"

joda::cli::ResultInterface::~ResultInterface() {
  destroyWindows();
  endwin();
  std::cout << std::endl << std::endl << std::flush;
}

void joda::cli::ResultInterface::createWindows() {
  /*
   * -----------------------------------------------
   * |  R                                          |
   * |      E                                      |
   * |         S                                   |
   * |           U                                 |
   * |             L                               |
   * |               T                             |
   * |                 S                           |
   * |                                             |
   * | [====================]>Progress             |
   * -----------------------------------------------
   */
  getmaxyx(stdscr, maxRows, maxCols);

  // Results
  resultborder = newwin(maxRows - 1, maxCols, 0, 0);
  wborder(resultborder, '|', '|', '-', '-', '+', '+', '+', '+');
  maxResults = static_cast<size_t>(std::max(maxRows - 3, 0));
  maxLineLen = maxCols - 2;
  results = newwin(maxResults, maxCols - 2, 1, 1);
  keypad(results, TRUE);  // Results window can receive KEY_UP ..
  cache.clear();

  buildStatusBar();
  refreshAll();
}

void joda::cli::ResultInterface::destroyWindows() {
  destroyWindow(results);
  destroyWindow(resultborder);
  destroyWindow(status_1);
  destroyWindow(status_2);
  destroyWindow(status_3);
  destroyWindow(status);
}

void joda::cli::ResultInterface::destroyWindow(WINDOW* local_win) {
  if (local_win == nullptr) {
    return;
  }
  wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  wrefresh(local_win);
  delwin(local_win);
}

void joda::cli::ResultInterface::refreshAll() {
  wrefresh(status);
  wrefresh(resultborder);
  wrefresh(results);
}

void joda::cli::ResultInterface::updateContent() {
  if (storage == nullptr) {
    return;
  }
  if (storage->size() == 0) {
    std::string status = "Empty result set";
    setStatus2(status);
  }
  werase(results);
  if (maxResultI - minResultI < maxResults) {
    maxResultI = std::min(storageSize, minResultI + maxResults - 1);
  }
  if (cache.empty()) {
    initCache();
  }

  auto j = currResult - minResultI;
  DCHECK_LE(cache.size(), maxResults);
  for (size_t i = 0; i < cache.size(); ++i) {
    if (i == j) {
      wattron(results, A_STANDOUT);
    } else {
      wattroff(results, A_STANDOUT);
      wattron(results, A_NORMAL);
    }
    auto wstr = getWString(cache[i]);
    mvwaddwstr(results, i, 0, wstr.c_str());
  }
  wrefresh(results);
}

void joda::cli::ResultInterface::scrollResults(bool down) {
  if (down) {
    if (currResult < storageSize) {
      currResult++;
      if (currResult > maxResultI && maxResultI < storageSize) {
        maxResultI++;
        minResultI++;
        addCache(maxResultI, true);
      }
    }
  } else {
    if (currResult > 0) {
      currResult--;
      if (currResult < minResultI && minResultI > 0) {
        maxResultI--;
        minResultI--;
        addCache(minResultI, false);
      }
    }
  }
}

void joda::cli::ResultInterface::interact() {
  forceRefresh();
  bool exec = true;
  while (exec) {
    auto ch = wgetch(results);
    switch (ch) {
      case KEY_UP:
        scrollResults(false);
        updateContent();
        break;
      case KEY_DOWN:
        scrollResults(true);
        updateContent();
        break;
      case 'q':
        exec = false;
        break;
      case KEY_ENTER:
      case '\n':
        startJSONViewer();
        forceRefresh();
        break;
      case KEY_RESIZE:
        destroyWindows();
        createWindows();
        break;
    }
  }
}

void joda::cli::ResultInterface::startJSONViewer() const {
  joda::cli::JSONInterface ji;
  LOG(INFO) << "Showing JSON";
  ji.showJSON(storage, currResult);
}

void joda::cli::ResultInterface::forceRefresh() {
  touchwin(status);
  touchwin(results);
  touchwin(resultborder);
  refreshAll();
}

joda::cli::ResultInterface::ResultInterface() {
  setlocale(LC_ALL, "");
  initscr();
  if (has_colors()) {
    start_color();
    use_default_colors();
    init_pair(JODA_JSON_OUTPUT_COLOR_NUMBER, COLOR_BLUE, -1);
    init_pair(JODA_JSON_OUTPUT_COLOR_STRING, COLOR_GREEN, -1);
    init_pair(JODA_JSON_OUTPUT_COLOR_BOOL, COLOR_BLUE, -1);
    init_pair(JODA_JSON_OUTPUT_COLOR_NULL, COLOR_BLUE, -1);
    init_pair(JODA_JSON_OUTPUT_COLOR_KEY, COLOR_MAGENTA, -1);
  }
  cbreak();
  curs_set(0);
  keypad(stdscr, TRUE);
  noecho();
  createWindows();
  updateContent();
}

void joda::cli::ResultInterface::updateStorage(
    std::shared_ptr<JSONStorage>& storage) {
  this->storage = storage;
  storageSize = storage->size() - 1;
  updateContent();
}

void joda::cli::ResultInterface::buildStatusBar() {
  auto len = maxLineLen / 3;
  destroyWindow(status_1);
  destroyWindow(status_2);
  destroyWindow(status_3);
  destroyWindow(status);
  status = newwin(1, maxLineLen, maxRows - 1, 0);

  status_1 = derwin(status, 1, len, 0, 0);
  status_2 = derwin(status, 1, len, 0, len + 1);
  status_3 = derwin(status, 1, len, 0, 2 * len + 1);

  mvwaddstr(status_3, 0, 0, "q: Quit; Arrow Keys to navigate");

  wrefresh(status);
}

void joda::cli::ResultInterface::setStatus1(std::string& str) {
  werase(status_1);
  mvwaddstr(status_1, 0, 0, str.c_str());
  wrefresh(status_1);
}
void joda::cli::ResultInterface::setStatus2(std::string& str) {
  werase(status_2);
  mvwaddstr(status_2, 0, 0, str.c_str());
  wrefresh(status_2);
}
void joda::cli::ResultInterface::setStatus3(std::string& str) {
  werase(status_3);
  mvwaddstr(status_3, 0, 0, str.c_str());
  wrefresh(status_3);
}

void joda::cli::ResultInterface::initCache() {
  cache = storage->stringify(minResultI, maxResultI);
  for (auto& i : cache) {
    i = i.substr(0, static_cast<unsigned long>(maxLineLen));
  }
}

void joda::cli::ResultInterface::addCache(size_t i, bool back) {
  auto entry = storage->stringify(i, i);
  entry.front() =
      entry.front().substr(0, static_cast<unsigned long>(maxLineLen));
  if (entry.size() == 1) {
    if (back) {
      cache.emplace_back(std::move(entry.front()));
      if (cache.size() > maxResults) {
        cache.erase(cache.begin());
      }
    } else {
      cache.insert(cache.begin(), std::move(entry.front()));
      if (cache.size() > maxResults) {
        cache.pop_back();
      }
    }
  }
  DCHECK_LE(cache.size(), maxResults);
}

std::wstring joda::cli::ResultInterface::getWString(std::string& str) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>> cv;

  std::wstring wstr = cv.from_bytes(str);
  return wstr;
}
