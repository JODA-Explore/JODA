//
// Created by Nico Schäfer on 3/20/17.
//

#include "../include/joda/document/RapidJsonDocument.h"
#include <joda/document/view/ViewLayer.h>
#include <rapidjson/document.h>
#include <rapidjson/fwd.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

const std::unique_ptr<RJDocument>& RapidJsonDocument::getJson() const {
  return json;
}

void RapidJsonDocument::removeDoc() { this->json = nullptr; }

RapidJsonDocument::RapidJsonDocument() = default;

unsigned long RapidJsonDocument::getId() const { return id; }

DOC_ID RapidJsonDocument::getNewID() { return ++currID; }

std::atomic_ulong RapidJsonDocument::currID{0};
const IOrigin* const RapidJsonDocument::getOrigin() const {
  return origin.get();
}

RapidJsonDocument::RapidJsonDocument(std::unique_ptr<RJDocument>&& json,
                                     std::unique_ptr<const IOrigin>&& origin)
    : RapidJsonDocument(getNewID(), std::move(json), std::move(origin)) {}

RapidJsonDocument::RapidJsonDocument(unsigned long id,
                                     std::unique_ptr<RJDocument>&& json,
                                     std::unique_ptr<const IOrigin>&& origin)
    : id(id), origin(std::move(origin)), json(std::move(json)) {}

RapidJsonDocument::RapidJsonDocument(RapidJsonDocument&& doc) noexcept
    : id(doc.id),
      origin(std::move(doc.origin)),
      json(std::move(doc.json)),
      view(std::move(doc.view)),
      isView_(doc.isView_) {}

void RapidJsonDocument::setJson(std::unique_ptr<RJDocument>&& json) {
  RapidJsonDocument::json = std::move(json);
}

void RapidJsonDocument::setOrigin(std::unique_ptr<const IOrigin>&& orig) {
  origin = std::move(orig);
}

RapidJsonDocument& RapidJsonDocument::operator=(
    RapidJsonDocument&& other) noexcept {
  id = other.id;
  origin = std::move(other.origin);
  json = std::move(other.json);
  view = std::move(other.view);
  isView_ = other.isView_;
  return *this;
}

bool RapidJsonDocument::isView() const { return isView_; }

void RapidJsonDocument::setView(std::unique_ptr<ViewLayer>&& view) {
  RapidJsonDocument::view = std::move(view);
  isView_ = RapidJsonDocument::view != nullptr;
}

RJValue const* RapidJsonDocument::Get(const RJPointer& ptr) const {
  if (isView()) {
    DCHECK(view != nullptr) << "View was not computed";
    return view->Get(ptr);
  }
  DCHECK(json != nullptr) << "Json was not loaded";
  return ptr.Get(*json);
}

const std::unique_ptr<ViewLayer>& RapidJsonDocument::getView() const {
  return view;
}
