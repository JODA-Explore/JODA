#ifndef JODA_DOCPARSING_PARSERINTERMEDIATES_H
#define JODA_DOCPARSING_PARSERINTERMEDIATES_H
#include <memory>
#include <joda/document/IDPositionOrigin.h>

namespace joda::docparsing {
struct StreamOrigin {
  std::unique_ptr<IDPositionOrigin> streamOrigin;
  std::unique_ptr<std::istream> stream;
};

struct StringOrigin {
  StringOrigin(std::unique_ptr<IDPositionOrigin>&& origin, std::string&& str)
      : stringOrigin(std::move(origin)), string(std::move(str)) {}

  std::unique_ptr<IDPositionOrigin> stringOrigin;
  std::string string;
};
}  // namespace joda::docparsing
#endif // JODA_DOCPARSING_PARSERINTERMEDIATES_H