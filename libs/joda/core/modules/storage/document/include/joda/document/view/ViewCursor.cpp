//
// Created by Nico on 27/08/2019.
//

#include "ViewCursor.h"
#include <glog/logging.h>
#include <algorithm>

ViewCursor::ViewCursor(const RJValue* doc, ViewCursor* prev,
                       const std::vector<std::string>* viewPaths)
    : _stack({doc}), previousCursor(prev), viewPaths(viewPaths) {}

const RJValue* ViewCursor::getVal() const { return _stack.back(); }

const ViewCursor* ViewCursor::getPreviousCursor() const {
  return previousCursor;
}

void ViewCursor::getOverwrites(
    const std::string& curr,
    std::vector<ViewCursor::ViewObjectMember>& previousOverwrites) {
  if (_stack.back() == nullptr || !_stack.back()->IsObject()) {
    return;
  }

  auto baseAttPath = curr + "/";
  if (viewPaths == nullptr) {
    return;
  }
  for (const auto& viewpath : *viewPaths) {
    if (viewpath.find(curr) == 0) {
      auto nameStr = viewpath.substr(baseAttPath.size());
      if (nameStr.find('/') != std::string::npos) {
        continue;
      }
      auto f = std::find_if(
          previousOverwrites.begin(), previousOverwrites.end(),
          [&nameStr](const ViewObjectMember& m) { return m.name == nameStr; });
      if (f == previousOverwrites.end()) {
        auto it = _stack.back()->FindMember(nameStr);
        if (it == _stack.back()->MemberEnd()) {
          previousOverwrites.emplace_back(std::move(nameStr),
                                          nullptr);  // Add removed Value
        } else {
          previousOverwrites.emplace_back(std::move(nameStr),
                                          &it->value);  // Add Changed
        }
      }
    }
  }
}

void ViewCursor::followAtt(const std::string& attr) {
  if (_stack.back() == nullptr && previousCursor == nullptr) {
    _stack.emplace_back(nullptr);
    return;
  }
  bool found = false;
  if (nextDestinationSet) {
    _stack.emplace_back(nextDestination);
    nextDestination = nullptr;
    nextDestinationSet = false;
    found = true;
  }

  if ((!found) && _stack.back() != nullptr && _stack.back()->IsObject()) {
    DCHECK(_stack.back() != nullptr);
    auto iterator = _stack.back()->FindMember(attr);
    if (iterator != _stack.back()->MemberEnd()) {
      _stack.emplace_back(&iterator->value);
      found = true;
    }
  }

  if (!found) {
    _stack.emplace_back(nullptr);
  }

  if (previousCursor != nullptr) {
    previousCursor->followAtt(attr);
    DCHECK_EQ(previousCursor->_stack.size(), _stack.size());
  }
}

void ViewCursor::unfollow() {
  DCHECK(!_stack.empty());
  _stack.pop_back();
  if (previousCursor != nullptr) {
    previousCursor->unfollow();
  }
}

void ViewCursor::follow(const RJPointer& ptr) {
  auto* token = ptr.GetTokens();
  auto count = ptr.GetTokenCount();
  while (count > 0) {
    followAtt(token->name);
    count--;
    token++;
  }
}

void ViewCursor::reset() {
  nextDestination = nullptr;
  nextDestinationSet = false;
  while (_stack.size() > 1) {
    _stack.pop_back();
  }
  if (previousCursor != nullptr) {
    previousCursor->reset();
  }
  DCHECK(_stack.size() == 1);
}

void ViewCursor::setNextDestination(const RJValue* nextDestination) {
  ViewCursor::nextDestination = nextDestination;
  nextDestinationSet = true;
}

bool ViewCursor::isBase(const std::string& currPointer) const {
  if (previousCursor == nullptr) {
    return true;
  }
  for (const auto& viewpath : *viewPaths) {
    if (currPointer.find(viewpath) == 0) {
      return true;
    }
  }
  return false;
}

bool ViewCursor::isShared(const std::string& currPointer) const {
  const ViewCursor* baseCursor = this;
  // Get Basecursor
  while (!baseCursor->isBase(currPointer)) {
    baseCursor = baseCursor->previousCursor;
  }
  const ViewCursor* iCursor = this;
  if (iCursor == baseCursor) {
    return false;
  }
  while (iCursor != baseCursor->previousCursor) {
    for (const auto& viewPath : *iCursor->viewPaths) {
      if (viewPath.find(currPointer) == 0) {
        return true;
      }
    }
    iCursor = iCursor->previousCursor;
  }
  return false;
}
