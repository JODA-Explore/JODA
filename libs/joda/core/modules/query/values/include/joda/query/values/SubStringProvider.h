//
// Created by Nico Schäfer
//

#ifndef JODA_SUBSTRINGPROVIDER_H
#define JODA_SUBSTRINGPROVIDER_H

#include <regex>

#include "IValueProvider.h"
namespace joda::query {
/**
 * SUBSTRING(), extracts a substring from the given string
 */
class SubStringProvider : public joda::query::IValueProvider {
 public:
  explicit SubStringProvider(
      std::vector<std::unique_ptr<IValueProvider>> &&parameters);
  std::unique_ptr<IValueProvider> duplicate() const override;
  bool isConst() const override;

  RJValue getAtomValue(const RapidJsonDocument &json,
                       RJMemoryPoolAlloc &alloc) const override;
  const RJValue *getValue(const RapidJsonDocument &json,
                          RJMemoryPoolAlloc &alloc) const override;

  bool isAtom() const override;
  std::string getName() const override;
  IValueType getReturnType() const override;

  CREATE_FACTORY(SubStringProvider)

 private:
  // http://www.zedwood.com/article/cpp-utf-8-mb_substr-function
  static std::string utf8_substr(const std::string &str, int start,
                                 int length = INT_MAX) {
    {
      int i, ix, j, realstart, reallength;
      if (length == 0) return "";
      if (start < 0 || length < 0) {
        // find j=utf8_strlen(str);
        for (j = 0, i = 0, ix = str.length(); i < ix; i += 1, j++) {
          unsigned char c = str[i];
          if (c >= 0 && c <= 127)
            i += 0;
          else if (c >= 192 && c <= 223)
            i += 1;
          else if (c >= 224 && c <= 239)
            i += 2;
          else if (c >= 240 && c <= 247)
            i += 3;
          else if (c >= 248 && c <= 255)
            return "";  // invalid utf8
        }
        if (length != INT_MAX && j + length - start <= 0) return "";
        if (start < 0) start += j;
        if (length < 0) length = j + length - start;
      }

      j = 0, realstart = 0, reallength = 0;
      for (i = 0, ix = str.length(); i < ix; i += 1, j++) {
        if (j == start) {
          realstart = i;
        }
        if (j >= start && (length == INT_MAX || j <= start + length)) {
          reallength = i - realstart;
        }
        unsigned char c = str[i];
        if (c >= 0 && c <= 127)
          i += 0;
        else if (c >= 192 && c <= 223)
          i += 1;
        else if (c >= 224 && c <= 239)
          i += 2;
        else if (c >= 240 && c <= 247)
          i += 3;
        else if (c >= 248 && c <= 255)
          return "";  // invalid utf8
      }
      if (j == start) {
        realstart = i;
      }
      if (j >= start && (length == INT_MAX || j <= start + length)) {
        reallength = i - realstart;
      }

      return str.substr(realstart, reallength);
    }
  }
};
}  // namespace joda::query

#endif  // JODA_SUBSTRINGPROVIDER_H
