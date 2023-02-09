#include <gtest/gtest.h>
#include <joda/query/Query.h>
#include <joda/queryparsing/QueryParser.h>
#include "../src/adaptive/visitors/SetPredicateVisitor.h"
#include "../src/adaptive/visitors/NextPredicateVisitor.h"

class SetPredicateVisitorTest : public::testing::Test {
protected:
    std::shared_ptr<joda::query::Query> parseQuery(const std::string &str, bool expect = true) {
        joda::queryparsing::QueryParser qp;
        auto q = qp.parse(str);
        if (q == nullptr){
            EXPECT_FALSE(expect) << qp.getLastError();
            return nullptr;
        }else{
            EXPECT_TRUE(expect);
            return q;
        }
    }
};

TEST_F(SetPredicateVisitorTest, FirstAndTrue) {

    std::shared_ptr<joda::query::Query> q = parseQuery("LOAD TWITTER CHOOSE '/user/screen_name' == \"ricardorossello\" && '/user/screen_name' == 6");

    SetPredicateVisitor setPredicateVisitor(true);
    setPredicateVisitor.visit(q->getChoose().get());

    q->setChoose(setPredicateVisitor.getPred());

    auto optimized = q->getChoose()->optimize();
    if(optimized != nullptr){
        q->setChoose(std::move(optimized));
    }

    NextPredicateVisitor nextQueryVisitor;
    nextQueryVisitor.visit(q->getChoose().get());

    auto adaptiveQueryNode = nextQueryVisitor.getNextPredicate();
    auto adaptiveQuery = std::move(adaptiveQueryNode->query);

    EXPECT_EQ(adaptiveQuery->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::NONE);
    EXPECT_EQ(adaptiveQuery->contentType, AdaptiveIndexQuery::CONTENT_TYPE::INT);
    EXPECT_EQ(adaptiveQuery->cmpType, AdaptiveIndexQuery::COMPARISON_TYPE::EQUALS);
    EXPECT_EQ(adaptiveQuery->propertyPath, "/user/screen_name");
    EXPECT_EQ(adaptiveQuery->conjunctionType, AdaptiveIndexQuery::CONJUNCTION_TYPE::NONE);
}

TEST_F(SetPredicateVisitorTest, FirstAndFalse) {

    std::shared_ptr<joda::query::Query> q = parseQuery("LOAD TWITTER CHOOSE '/user/screen_name' == \"ricardorossello\" && '/user/screen_name' == 6");

    SetPredicateVisitor setPredicateVisitor(false);
    setPredicateVisitor.visit(q->getChoose().get());

    q->setChoose(setPredicateVisitor.getPred());

    auto optimized = q->getChoose()->optimize();
    if(optimized != nullptr){
        q->setChoose(std::move(optimized));
    }


    NextPredicateVisitor nextQueryVisitor;
    nextQueryVisitor.visit(q->getChoose().get());

    EXPECT_FALSE(nextQueryVisitor.getNextPredicate());
}

TEST_F(SetPredicateVisitorTest, FirstOrTrue) {

    std::shared_ptr<joda::query::Query> q = parseQuery("LOAD TWITTER CHOOSE '/user/screen_name' == \"ricardorossello\" || '/user/screen_name' == 6");

    SetPredicateVisitor setPredicateVisitor(true);
    setPredicateVisitor.visit(q->getChoose().get());

    q->setChoose(setPredicateVisitor.getPred());

    auto optimized = q->getChoose()->optimize();
    if(optimized != nullptr){
        q->setChoose(std::move(optimized));
    }

    NextPredicateVisitor nextQueryVisitor;
    nextQueryVisitor.visit(q->getChoose().get());

    EXPECT_FALSE(nextQueryVisitor.getNextPredicate());
}

TEST_F(SetPredicateVisitorTest, FirstOrFalse) {

    std::shared_ptr<joda::query::Query> q = parseQuery("LOAD TWITTER CHOOSE '/user/screen_name' == \"ricardorossello\" || '/user/screen_name' == 6");

    SetPredicateVisitor setPredicateVisitor(false);
    setPredicateVisitor.visit(q->getChoose().get());

    q->setChoose(setPredicateVisitor.getPred());

    auto optimized = q->getChoose()->optimize();
    if(optimized != nullptr){
        q->setChoose(std::move(optimized));
    }

    NextPredicateVisitor nextQueryVisitor;
    nextQueryVisitor.visit(q->getChoose().get());

    auto adaptiveQueryNode = nextQueryVisitor.getNextPredicate();
    auto adaptiveQuery = std::move(adaptiveQueryNode->query);

    EXPECT_EQ(adaptiveQuery->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::NONE);
    EXPECT_EQ(adaptiveQuery->contentType, AdaptiveIndexQuery::CONTENT_TYPE::INT);
    EXPECT_EQ(adaptiveQuery->cmpType, AdaptiveIndexQuery::COMPARISON_TYPE::EQUALS);
    EXPECT_EQ(adaptiveQuery->propertyPath, "/user/screen_name");
    EXPECT_EQ(adaptiveQuery->conjunctionType, AdaptiveIndexQuery::CONJUNCTION_TYPE::NONE);
}