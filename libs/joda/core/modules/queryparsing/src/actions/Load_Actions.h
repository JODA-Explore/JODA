//
// Created by Nico on 08/05/2019.
//

#ifndef JODA_LOAD_ACTIONS_H
#define JODA_LOAD_ACTIONS_H

#include <joda/parser/JSONURLSource.h>
#include "../grammar/Grammar.h"
#include "../states/States.h"

#include "joda/query/values/PointerProvider.h"
namespace joda::queryparsing::grammar {

template <>
struct loadAction<loadIdent> {
  template <typename Input>
  static void apply(const Input &in, loadState &state) {
    state.loadVar = in.string();
  }
};

template <>
struct loadAction<fromFilesKW> {
  static void apply0(loadState &state) { state.source = DIRECTORY_SOURCE; }
};

template <>
struct loadAction<fromFileKW> {
  static void apply0(loadState &state) { state.source = FILE_SOURCE; }
};

template <>
struct loadAction<fromURLKW> {
  template <typename Input>
  static void apply(const Input &in, loadState &state) {
#ifndef JODA_ENABLE_FROMURL
    throw tao::pegtl::parse_error(
        "This program was compiled without 'FROM URL' support.", in);
#endif
    state.source = URL_SOURCE;
  }
};

template <>
struct loadAction<loadFileName> {
  template <typename Input>
  static void apply(const Input &in, loadState &state) {
    state.loadFile = in.string();
  }
};

template <>
struct loadAction<groupPointer> {
  template <typename Input>
  static void apply(const Input &in, loadState &state) {
    std::string pointer = in.string();
    state.putValProv(std::make_unique<joda::query::PointerProvider>(
        pointer.substr(1, pointer.size() - 2)));
  }
};

template <>
struct loadAction<loadFilesLineSeperatedCommand> {
  static void apply0(loadState &state) { state.lineSeperated = true; }
};

template <>
struct loadAction<sampleNum> {
  template <typename Input>
  static void apply(const Input &in, loadState &state) {
    std::string str = in.string();
    assert(!str.empty() && "String can not be empty");
    state.sampleNum = std::stod(str);
  }
};

template <>
struct loadAction<loadImportSource> {
  template <typename Input>
  static void apply(const Input &in, loadState &state) {
    if (!state.loadFile.empty()) {
      switch (state.source) {
        case NO_SOURCE:
          LOG(ERROR) << "No source for load, this should not be possible.";
          break;
        case FILE_SOURCE:
          state.sources.emplace_back(
              std::make_unique<docparsing::JSONFileSource>(
                  state.loadFile, state.lineSeperated, state.sampleNum));
          break;
        case DIRECTORY_SOURCE:
          state.sources.emplace_back(
              std::make_unique<docparsing::JSONFileDirectorySource>(
                  state.loadFile, state.lineSeperated, state.sampleNum));
          break;
        case URL_SOURCE:
          state.sources.emplace_back(
              std::make_unique<docparsing::JSONURLSource>(state.loadFile));
          break;
      }
    }
    state.source = NO_SOURCE;
    state.loadFile.clear();
    state.sampleNum = 1;
    state.lineSeperated = JODA_DEFAULT_LINE_SEPERATED;
  }
};

}  // namespace joda::queryparsing::grammar
#endif  // JODA_LOAD_ACTIONS_H
