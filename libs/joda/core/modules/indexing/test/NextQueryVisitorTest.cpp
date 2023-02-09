#include <gtest/gtest.h>
#include <joda/query/Query.h>
#include <joda/queryparsing/QueryParser.h>
#include "../src/adaptive/visitors/NextPredicateVisitor.h"

class NextQueryVisitorTest : public::testing::Test {
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

TEST_F(NextQueryVisitorTest, GetQuerySingle) {

    std::shared_ptr<joda::query::Query> q = parseQuery("LOAD TWITTER CHOOSE '/user/screen_name' == \"ricardorossello\"");

    NextPredicateVisitor nextQueryVisitor;
    nextQueryVisitor.visit(q->getChoose().get());

    auto adaptiveQueryNode = nextQueryVisitor.getNextPredicate();
    auto adaptiveQuery = std::move(adaptiveQueryNode->query);

    EXPECT_EQ(adaptiveQuery->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::NONE);
    EXPECT_EQ(adaptiveQuery->contentType, AdaptiveIndexQuery::CONTENT_TYPE::STRING);
    EXPECT_EQ(adaptiveQuery->cmpType, AdaptiveIndexQuery::COMPARISON_TYPE::EQUALS);
    EXPECT_EQ(adaptiveQuery->propertyPath, "/user/screen_name");
    EXPECT_EQ(adaptiveQuery->conjunctionType, AdaptiveIndexQuery::CONJUNCTION_TYPE::NONE);
}

TEST_F(NextQueryVisitorTest, GetQueryFirst) {

    std::shared_ptr<joda::query::Query> q = parseQuery("LOAD TWITTER CHOOSE '/user/screen_name' == \"ricardorossello\" || SCONTAINS('/user/screen_name', \"sello\")");

    NextPredicateVisitor nextQueryVisitor;
    nextQueryVisitor.visit(q->getChoose().get());

    auto adaptiveQueryNode = nextQueryVisitor.getNextPredicate();
    auto adaptiveQuery = std::move(adaptiveQueryNode->query);

    EXPECT_EQ(adaptiveQuery->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::NONE);
    EXPECT_EQ(adaptiveQuery->contentType, AdaptiveIndexQuery::CONTENT_TYPE::STRING);
    EXPECT_EQ(adaptiveQuery->cmpType, AdaptiveIndexQuery::COMPARISON_TYPE::EQUALS);
    EXPECT_EQ(adaptiveQuery->propertyPath, "/user/screen_name");
    EXPECT_EQ(adaptiveQuery->conjunctionType, AdaptiveIndexQuery::CONJUNCTION_TYPE::NONE);
}

TEST_F(NextQueryVisitorTest, GetQueryLast) {

    std::shared_ptr<joda::query::Query> q = parseQuery("LOAD TWITTER CHOOSE SCONTAINS('/user/screen_name', \"sello\") || '/user/screen_name' == \"ricardorossello\"");

    NextPredicateVisitor nextQueryVisitor;
    nextQueryVisitor.visit(q->getChoose().get());

    auto adaptiveQueryNode = nextQueryVisitor.getNextPredicate();
    auto adaptiveQuery = std::move(adaptiveQueryNode->query);

    EXPECT_EQ(adaptiveQuery->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::NONE);
    EXPECT_EQ(adaptiveQuery->contentType, AdaptiveIndexQuery::CONTENT_TYPE::STRING);
    EXPECT_EQ(adaptiveQuery->cmpType, AdaptiveIndexQuery::COMPARISON_TYPE::EQUALS);
    EXPECT_EQ(adaptiveQuery->propertyPath, "/user/screen_name");
    EXPECT_EQ(adaptiveQuery->conjunctionType, AdaptiveIndexQuery::CONJUNCTION_TYPE::NONE);
}

TEST_F(NextQueryVisitorTest, GetQueryLastNested) {

    std::shared_ptr<joda::query::Query> q = parseQuery("LOAD TWITTER CHOOSE SCONTAINS('/user/screen_name', \"sello\") && !('/user/screen_name' == \"ricardorossello\")");

    NextPredicateVisitor nextQueryVisitor;
    nextQueryVisitor.visit(q->getChoose().get());

    auto adaptiveQueryNode = nextQueryVisitor.getNextPredicate();
    auto adaptiveQuery = std::move(adaptiveQueryNode->query);

    EXPECT_EQ(adaptiveQuery->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::NONE);
    EXPECT_EQ(adaptiveQuery->contentType, AdaptiveIndexQuery::CONTENT_TYPE::STRING);
    EXPECT_EQ(adaptiveQuery->cmpType, AdaptiveIndexQuery::COMPARISON_TYPE::EQUALS);
    EXPECT_EQ(adaptiveQuery->propertyPath, "/user/screen_name");
    EXPECT_EQ(adaptiveQuery->conjunctionType, AdaptiveIndexQuery::CONJUNCTION_TYPE::NONE);
}

TEST_F(NextQueryVisitorTest, GetQueryNested) {

    std::shared_ptr<joda::query::Query> q = parseQuery("LOAD TWITTER CHOOSE SCONTAINS('/user/screen_name', \"sello\") && !(SCONTAINS('/user/screen_name', \"sello\") && '/user/screen_name' == \"ricardorossello\")");

    NextPredicateVisitor nextQueryVisitor;
    nextQueryVisitor.visit(q->getChoose().get());

    auto adaptiveQueryNode = nextQueryVisitor.getNextPredicate();
    auto adaptiveQuery = std::move(adaptiveQueryNode->query);

    EXPECT_EQ(adaptiveQuery->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::NONE);
    EXPECT_EQ(adaptiveQuery->contentType, AdaptiveIndexQuery::CONTENT_TYPE::STRING);
    EXPECT_EQ(adaptiveQuery->cmpType, AdaptiveIndexQuery::COMPARISON_TYPE::EQUALS);
    EXPECT_EQ(adaptiveQuery->propertyPath, "/user/screen_name");
    EXPECT_EQ(adaptiveQuery->conjunctionType, AdaptiveIndexQuery::CONJUNCTION_TYPE::NONE);
}