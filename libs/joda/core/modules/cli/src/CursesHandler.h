//
// Created by Nico on 20/08/2019.
//

#ifndef JODA_CURSESHANDLER_H
#define JODA_CURSESHANDLER_H

#include <cstdint>
#include <rapidjson/rapidjson.h>
#include <vector>
#include <joda/misc/RJFwd.h>
#include <codecvt>
#include "CursesFWD.h"
#include <locale>

class CursesPrettyHandler {
  typedef RJChar::Ch Ch;
 public:
  CursesPrettyHandler(WINDOW *win, size_t idention = jDefaultIdention) : win(win), colors(has_colors() == TRUE),
                                                                         identionSize(idention) {
  }

  bool Null() {
    writePrefix();
    if (colors) wattron(win, COLOR_PAIR(JODA_JSON_OUTPUT_COLOR_NULL));
    writeWString("null");
    if (colors) wattroff(win, COLOR_PAIR(JODA_JSON_OUTPUT_COLOR_NULL));
    return true;
  }

  bool Bool(bool b) {
    writePrefix();
    if (colors) wattron(win, COLOR_PAIR(JODA_JSON_OUTPUT_COLOR_BOOL));
    if (b) writeWString("true");
    else writeWString("false");
    if (colors) wattroff(win, COLOR_PAIR(JODA_JSON_OUTPUT_COLOR_BOOL));
    return true;
  }

  bool Int(int i) {
    writePrefix();
    if (colors) wattron(win, COLOR_PAIR(JODA_JSON_OUTPUT_COLOR_NUMBER));
    writeWString(std::to_string(i));
    if (colors) wattroff(win, COLOR_PAIR(JODA_JSON_OUTPUT_COLOR_NUMBER));
    return true;
  }

  bool Uint(unsigned i) {
    writePrefix();
    if (colors) wattron(win, COLOR_PAIR(JODA_JSON_OUTPUT_COLOR_NUMBER));
    writeWString(std::to_string(i));
    if (colors) wattroff(win, COLOR_PAIR(JODA_JSON_OUTPUT_COLOR_NUMBER));
    return true;
  }

  bool Int64(int64_t i) {
    writePrefix();
    if (colors) wattron(win, COLOR_PAIR(JODA_JSON_OUTPUT_COLOR_NUMBER));
    writeWString(std::to_string(i));
    if (colors) wattroff(win, COLOR_PAIR(JODA_JSON_OUTPUT_COLOR_NUMBER));
    return true;
  }

  bool Uint64(uint64_t i) {
    writePrefix();
    if (colors) wattron(win, COLOR_PAIR(JODA_JSON_OUTPUT_COLOR_NUMBER));
    writeWString(std::to_string(i));
    if (colors) wattroff(win, COLOR_PAIR(JODA_JSON_OUTPUT_COLOR_NUMBER));
    return true;
  }

  bool Double(double d) {
    writePrefix();
    if (colors) wattron(win, COLOR_PAIR(JODA_JSON_OUTPUT_COLOR_NUMBER));
    writeWString(std::to_string(d));
    if (colors) wattroff(win, COLOR_PAIR(JODA_JSON_OUTPUT_COLOR_NUMBER));
    return true;
  }

  bool RawNumber(const Ch *str, rapidjson::SizeType length, bool copy) {
    writePrefix();
    if (colors) wattron(win, COLOR_PAIR(JODA_JSON_OUTPUT_COLOR_NUMBER));
    auto basicString = std::string(str, length);
    writeWString(basicString);
    if (colors) wattroff(win, COLOR_PAIR(JODA_JSON_OUTPUT_COLOR_NUMBER));
    return true;
  }

  bool String(const Ch *str, rapidjson::SizeType length, bool copy) {
    writePrefix();
    if (colors) wattron(win, COLOR_PAIR(JODA_JSON_OUTPUT_COLOR_STRING));
    auto basicString = std::string(str, length);
    basicString = "\"" + basicString + "\"";
    writeWString(basicString);
    if (colors) wattroff(win, COLOR_PAIR(JODA_JSON_OUTPUT_COLOR_STRING));
    return true;
  }

  bool Key(const Ch *str, rapidjson::SizeType length, bool copy) {
    writePrefix();
    if (colors) wattron(win, COLOR_PAIR(JODA_JSON_OUTPUT_COLOR_KEY));
    auto basicString = std::string(str, length);
    basicString = "\"" + basicString + "\"";
    writeWString(basicString);
    if (colors) wattroff(win, COLOR_PAIR(JODA_JSON_OUTPUT_COLOR_KEY));
    return true;
  }

  bool StartObject() {
    writePrefix();
    writeWString("{");
    levels.emplace_back(false);
    return true;
  }

  bool EndObject(rapidjson::SizeType memberCount) {
    auto empty = levels.back().valueCount == 0;
    levels.pop_back();
    if (!empty) {
      writeWString("\n");
      writeIndent();
    }
    writeWString("}");
    return true;
  }

  bool StartArray() {
    writePrefix();
    writeWString("[");
    levels.emplace_back(true);
    return true;
  }

  bool EndArray(rapidjson::SizeType elementCount) {
    auto empty = levels.back().valueCount == 0;
    levels.pop_back();
    if (!empty) {
      writeWString("\n");
      writeIndent();
    }
    writeWString("]");
    return true;
  }

 private:
  void writeIndent() {
    std::string str;
    str.insert(0, identionSize * levels.size(), ' ');
    writeWString(str);
  }

  void writePrefix() {
    if (!levels.empty()) { // this value is not at root
      auto &level = levels.back();

      if (level.inArray) {
        if (level.valueCount > 0) {
          writeWString(","); // add comma if it is not the first element in array
        }
        writeWString("\n");
        writeIndent();
      } else {  // in object
        if (level.valueCount > 0) {
          if (level.valueCount % 2 == 0) {
            writeWString(",\n");
          } else {
            writeWString(": ");
          }
        } else writeWString("\n");

        if (level.valueCount % 2 == 0) writeIndent();
      }
      level.valueCount++;
    }
  }

  void writeWString(const std::string &str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> cv;

    std::wstring wstr = cv.from_bytes(str);
    waddwstr(win, wstr.c_str());
  }

  size_t identionSize;
  WINDOW *win;
  bool colors;

  struct Level {
    Level(bool inArray_) : valueCount(0), inArray(inArray_) {}

    size_t valueCount;  //!< number of values in this level
    bool inArray;       //!< true if in array, otherwise in object
  };

  std::vector<Level> levels;

  static constexpr size_t jDefaultIdention = 4;
};

#endif //JODA_CURSESHANDLER_H
