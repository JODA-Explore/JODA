//
// Created by Nico on 27/08/2019.
//

#ifndef JODA_VIEWCURSOR_H
#define JODA_VIEWCURSOR_H

#include <joda/misc/RJFwd.h>
#include <memory>
#include <vector>
#include "ViewStructure.h"

class ViewCursor {
 public:
  struct ViewObjectMember {
    ViewObjectMember(const std::string &name, const RJValue *val)
        : name(name), val(val){};

    ViewObjectMember(std::string &&name, const RJValue *val)
        : name(std::move(name)), val(val){};
    std::string name;
    const RJValue *val;
  };

 public:
  ViewCursor(const RJValue *doc, ViewCursor *,
             const std::vector<std::string> *viewPaths);
  void followAtt(const std::string &attr);
  void follow(const RJPointer &ptr);
  void setNextDestination(const RJValue *nextDestination);
  void unfollow();

  const RJValue *getVal() const;
  const ViewCursor *getPreviousCursor() const;
  bool isBase(const std::string &currPointer) const;
  bool isShared(const std::string &currPointer) const;
  void getOverwrites(
      const std::string &curr,
      std::vector<ViewCursor::ViewObjectMember> &previousOverwrites);

  void reset();

 private:
  std::vector<const RJValue *> _stack;
  ViewCursor *previousCursor;
  const std::vector<std::string> *viewPaths;
  const RJValue *nextDestination = nullptr;
  bool nextDestinationSet = false;
};

#endif  // JODA_VIEWCURSOR_H
