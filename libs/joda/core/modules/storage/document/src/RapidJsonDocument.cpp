//
// Created by Nico Schäfer on 3/20/17.
//

#include "../include/joda/document/RapidJsonDocument.h"
#include <rapidjson/fwd.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>
#include "../include/joda/document/RapidJsonDocument.h"



std::shared_ptr<RJDocument> RapidJsonDocument::getJson() const {
  return json;
}


void RapidJsonDocument::removeDoc() {
  this->json = nullptr;
}

void RapidJsonDocument::setJson(const std::shared_ptr<RJDocument> &json) {
  RapidJsonDocument::json = json;
}

RapidJsonDocument::RapidJsonDocument() = default;

unsigned long RapidJsonDocument::getId() const {
  return id;
}

const IOrigin *const RapidJsonDocument::getOrigin() const {
  return origin.get();
}

RapidJsonDocument::RapidJsonDocument(unsigned long id,
                                     std::shared_ptr<RJDocument> &&json,
                                     std::unique_ptr<const IOrigin> &&origin)
    : id(id), origin(std::move(origin)), json(std::move(json)) {}

RapidJsonDocument::RapidJsonDocument(RapidJsonDocument &&doc) noexcept : id(doc.id), origin(std::move(doc.origin)), json(std::move(doc.json))  {

}

RapidJsonDocument::RapidJsonDocument(const RapidJsonDocument &doc) : id(doc.id), origin(doc.origin->clone()), json(doc.json) {

}

void RapidJsonDocument::setJson(std::shared_ptr<RJDocument> &&json) {
  RapidJsonDocument::json = std::move(json);
}

void RapidJsonDocument::setOrigin(std::unique_ptr<const IOrigin> &&orig) {
  origin = std::move(orig);
}

RapidJsonDocument &RapidJsonDocument::operator=(const RapidJsonDocument &other) {
  id = other.id;
  origin=std::move(other.origin->clone());
  json = other.json;
  return *this;
}

RapidJsonDocument &RapidJsonDocument::operator=(RapidJsonDocument &&other) noexcept {
  id = other.id;
  origin=std::move(other.origin);
  json = std::move(other.json);
  return *this;
}
