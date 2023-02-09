//
// Created by Nico on 08/05/2019.
//

#ifndef JODA_STORE_ACTIONS_H
#define JODA_STORE_ACTIONS_H

#include "../grammar/Grammar.h"
#include "../states/States.h"

#include <joda/export/DirectoryExport.h>
#include <joda/export/StreamExport.h>
#include <joda/export/FileExport.h>
#include <joda/export/JoinExport.h>
#include <joda/export/StorageExport.h>
#include <joda/extension/python/PythonExport.h>

namespace joda::queryparsing::grammar {

template <>
struct storeAction<storeIdent> {
  template <typename Input>
  static void apply(const Input &in, storeState &state) {
    state.exportDest = std::make_unique<StorageExport>(
        StorageCollection::getInstance().getOrAddStorage(in.string()));
  }
};

template <>
struct storeAction<groupPointer> {
  template <typename Input>
  static void apply(const Input &in, storeState &state) {
    std::string pointer = in.string();
    state.putValProv(std::make_unique<joda::query::PointerProvider>(
        pointer.substr(1, pointer.size() - 2)));
    state.exportDest = std::make_unique<JoinExport>(
        StorageCollection::getInstance().getOrStartJoin(
            std::make_unique<joda::query::PointerProvider>(
                pointer.substr(1, pointer.size() - 2))));
  }
};
template <>
struct storeAction<storeFileName> {
  template <typename Input>
  static void apply(const Input &in, storeState &state) {
    state.exportDest = std::make_unique<FileExport>(in.string());
  }
};

template <>
struct storeAction<storeFilesName> {
  template <typename Input>
  static void apply(const Input &in, storeState &state) {
    state.exportDest = std::make_unique<DirectoryExport>(in.string());
  }
};

template <>
struct storeAction<inStreamKW> {
  template <typename Input>
  static void apply(const Input &in, storeState &state) {
    if (!config::enable_streams) {
      throw tao::pegtl::parse_error(
          "Stream output can't be used in the given context", in);
    }
    state.exportDest = std::make_unique<StreamExport>();
  }
};

template <>
struct storeAction<customASId> {
  template <typename Input>
  static bool apply(const Input &in, storeState &state) {
    std::string custom_func = in.string();
    if(joda::extension::ModuleRegister::getInstance().exportExists(custom_func)){
      state.custom_name = custom_func;
      return true;
    }
    return false;
  }
};

template <>
struct storeAction<storeCutomParam> {
  template <typename Input>
  static void apply(const Input &in, storeState &state) {
    state.exportDest = joda::extension::ModuleRegister::getInstance().getExportDestination(state.custom_name, in.string());
  }
};


}  // namespace joda::queryparsing::grammar
#endif  // JODA_STORE_ACTIONS_H
