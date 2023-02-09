#include <joda/document/TemporaryOrigin.h>
#include <joda/join/relation/ThetaJoinExecutor.h>

namespace joda::join {
ThetaJoinExecutor::ThetaJoinExecutor(
    std::unique_ptr<query::IValueProvider>&& predicate)
    : predicate(std::move(predicate)) {
  if (!this->predicate->isBool()) {
    throw WherePredicateNotBoolException();
  }
}

std::shared_ptr<JSONContainer> ThetaJoinExecutor::handleOuterContainer(
    const std::shared_ptr<JSONContainer>& cont) const {
  auto outCont = std::make_shared<JSONContainer>();
  auto* alloc = cont->getAlloc();

  RJMemoryPoolAlloc tempAlloc(alloc->Capacity());

  // Theta nested Join
  // For every inner container
  for (const auto& innerCont : innerContainers) {
    // For every document in inner
    innerCont->forAll([&cont, &alloc, &outCont, &tempAlloc,
                       this](const RapidJsonDocument& innerDoc) {
      // For every doc in outer
      cont->forAll([&innerDoc, &alloc, &outCont, &tempAlloc,
                    this](const RapidJsonDocument& outerDoc) {
        tempAlloc.Clear();
        // Build temporary document
        auto tmpDoc = std::make_unique<RJDocument>(&tempAlloc);
        tmpDoc->SetObject();

        // Copy sub-documents
        RJDocument lhs(&tempAlloc);
        lhs.Populate(innerDoc);

        RJDocument rhs(&tempAlloc);
        rhs.Populate(outerDoc);

        // Move into main doc
        tmpDoc->AddMember("left", lhs, tempAlloc);
        tmpDoc->AddMember("right", rhs, tempAlloc);
        RapidJsonDocument tmp(std::move(tmpDoc),
                              std::make_unique<TemporaryOrigin>());

        // Check predicate
        if (predicate->getNonTruthyBool(tmp, tempAlloc)) {
          // Copy joined document
          auto newDoc = std::make_unique<RJDocument>(alloc);
          newDoc->Populate(tmp);
          // Add doc to container
          auto orig = std::make_unique<TemporaryOrigin>();
          outCont->insertDoc(std::move(newDoc), std::move(orig));
        }
      });
    });
  }
  outCont->finalize();

  return outCont;
}

std::string ThetaJoinExecutor::toString() const {
  return "WHERE " + predicate->toString();
}

}  // namespace joda::join