#include "AsPipeline.h"

#include <joda/query/Query.h>
#include <joda/query/project/PointerCopyProject.h>
namespace joda::queryexecution::pipeline::tasks::as {
void AsExec::fillBuffer(std::optional<Input>& ibuff, std::optional<Output>& obuff,
                        std::function<void(std::optional<Output>&)> sendPartial) {
  if(ibuff.has_value()) {
    auto filteredcont = std::move(ibuff.value());
    ibuff.reset();
    if (filteredcont.getContainer() == nullptr) {
      return;
    }

    auto o = handleOne(filteredcont); //transform documents
    if(o.has_value()){
      obuff = std::move(*o);
    }

    sendPartial(obuff);
  }
}

std::optional<AsExec::O::Output> AsExec::handleOne(I::Input& input) {
  auto& cont = input.getContainer();

  std::shared_ptr<const DocIndex> index = nullptr;
  if (input.isConst()) {           // Const  All/Nothing query
    if (input.getConstFilter()) {  // All query
      if (!projectionRequired) {   // All Query + * Query
        return {cont};
      } else {
        // Index will be required => create index selecting everything
        DocIndex _index(cont->size());
        _index.set();
        index = std::make_shared<DocIndex>(std::move(_index));
      }
    } else {  // Nothing query
      // => Skip container
      return {};
    }
  } else {
    index = input.getIndex();
  }

  auto contLock = cont->useContInScope(false);

  // Actual Transformation

  auto& projectors = query->getProjectors();

  if (cont->isView()) {  // Materialize required attributes
    cont->materializeAttributesIfRequired(asAttributes);
  }

  std::shared_ptr<JSONContainer> outCont;

  if (query->canCreateView()) {  // Create delta tree if possible
    // Check if using a delta tree is beneficial
    auto useView = cont->useViewBasedOnSample(*index, projectors,
                                              query->getSetProjectors());
    if (useView) {  // If yes => create the delta tree
      LOG(INFO) << "Creating View";
      outCont = cont->createViewFromContainer(*index, projectors,
                                              query->getSetProjectors());
    }
  }

  if (outCont == nullptr) {  // delta tree not used => default transformation
    auto selCount = index->count();
    double contFill = (static_cast<double>(selCount) / cont->size());

    // Create new container
    outCont = std::make_shared<JSONContainer>(cont->getMaxSize() * contFill);
    // Project documents
    auto projectResult = defaultProject(*cont, *index, *outCont->getAlloc());

    for (auto&& item : projectResult) {
      // Add non-null documents to container
      if (item->IsNull()) {
        continue;
      }
      outCont->insertDoc(std::move(item), std::make_unique<TemporaryOrigin>());
    }
  }
  return {outCont};
}

void AsExec::finalize(std::optional<Output>& obuff, std::function<void(std::optional<O::Output>&)> sendPartial) {}

AsExec::AsExec(const std::shared_ptr<const query::Query>& query)
    : query(query), asAttributes(query->getASAttributes()) {
  if (!asAttributes.empty() && asAttributes.front().empty()) {
    asAttributes.erase(asAttributes.begin());
  }
  projectionRequired = hasToProject();
}

bool AsExec::hasToProject() const {
  auto& projectors = query->getProjectors();
  return !(
      (query)->getSetProjectors().empty() &&
      (projectors.empty() || (projectors.size() == 1 &&
                              projectors.front()->getType() ==
                                  joda::query::PointerCopyProject::allCopy)));
}

std::vector<std::unique_ptr<RJDocument>> AsExec::defaultProject(
    JSONContainer& cont, const DocIndex& ids, RJMemoryPoolAlloc& alloc) const {
  std::vector<std::unique_ptr<RJDocument>> ret;

  auto& proj = query->getProjectors();
  auto& setproj = query->getSetProjectors();
  /*
   * Check for batch completion
   */
  if (proj.empty() && setproj.empty()) {  // Star Expression
    return cont.getRaw(ids, alloc);
  }

  return cont.projectDocuments(ids, proj, alloc, setproj);
}

std::string AsExec::toString() const {
  std::string ret;
  for (const auto& item : query->getProjectors()) {
    ret += "(" + item->toString() + "),";
  }
  for (const auto& item : query->getSetProjectors()) {
    ret += "(" + item->toString() + "),";
  }
  ret = ret.substr(0, ret.size() - 1);
  return ret;
}

std::shared_ptr<const joda::query::Query> AsExec::getData() const {
  return query;
}

}  // namespace joda::queryexecution::pipeline::tasks::as