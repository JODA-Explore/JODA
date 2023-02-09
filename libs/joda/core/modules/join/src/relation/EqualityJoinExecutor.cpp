#include <joda/join/relation/EqualityJoinExecutor.h>
#include <joda/document/TemporaryOrigin.h>

namespace joda::join {
EqualityJoinExecutor::EqualityJoinExecutor(
    std::unique_ptr<query::IValueProvider>&& inner,
    std::unique_ptr<query::IValueProvider>&& outer) {

  innerAtts = inner->getAttributes();
  outerAtts = outer->getAttributes();
  std::vector<std::unique_ptr<joda::query::IValueProvider>> params;
  params.emplace_back(std::move(inner));
  params.emplace_back(std::move(outer));
  pred = std::make_unique<query::EqualProvider>(std::move(params));
}

std::shared_ptr<JSONContainer> EqualityJoinExecutor::handleOuterContainer(
    const std::shared_ptr<JSONContainer>& cont) const {
    
  if(cont->isView()){
    cont->materializeAttributesIfRequired(outerAtts);
  }

  auto outCont = std::make_shared<JSONContainer>();
  auto* alloc = cont->getAlloc();

  

  // Nested Join
  // For every inner container
  for (const auto& innerCont : innerContainers) {
    if(innerCont->isView()){
      innerCont->materializeAttributesIfRequired(innerAtts);
    }
    // For every document in inner
    innerCont->forAll([&cont, &alloc, &outCont, this](const RapidJsonDocument& innerDoc) { 
      // For every doc in outer
      cont->forAll([&innerDoc, &alloc, &outCont, this](const RapidJsonDocument& outerDoc) {
        // If equal => join
        if(pred->getAtomValue(innerDoc,outerDoc,*alloc).GetBool()){
          auto newDoc = std::make_unique<RJDocument>(alloc);
          newDoc->SetObject();

          // Copy documents
          RJDocument lhs(alloc);
          lhs.Populate(innerDoc);

          RJDocument rhs(alloc);
          rhs.Populate(outerDoc);

          // Move into main doc
          newDoc->AddMember("left", lhs, *alloc);
          newDoc->AddMember("right", rhs, *alloc);

          // Add doc to container
          auto orig = std::make_unique<TemporaryOrigin>();
          outCont->insertDoc(std::move(newDoc),std::move(orig));

        }
      });
    });
  }
  outCont->finalize();

  return outCont;
}

std::string EqualityJoinExecutor::toString() const { 
  return "ON (" + pred->getLhs()->toString() + ", " + pred->getRhs()->toString() + ")";
}

}  // namespace joda::join