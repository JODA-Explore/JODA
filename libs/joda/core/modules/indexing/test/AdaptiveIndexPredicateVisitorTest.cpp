#include <gtest/gtest.h>
#include <joda/query/Query.h>
#include <joda/queryparsing/QueryParser.h>
#include "joda/indexing/AdaptiveIndexQueryTreeVisitor.h"
#include "../src/adaptive/visitors/SetPredicateVisitor.h"
#include <boost/dynamic_bitset.hpp>

class AdaptiveIndexPredicateVisitorTest : public::testing::Test {
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

/*TEST_F(AdaptiveIndexPredicateVisitorTest, ISSTRING) {
    std::shared_ptr<AdaptiveIndexQuery> adaptiveQuery = toAdaptiveQuery("LOAD TWITTER CHOOSE ISSTRING('/user/screen_name')");

    EXPECT_EQ(adaptiveQuery->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::ISSTRING);
    EXPECT_EQ(adaptiveQuery->propertyPath, "/user/screen_name");
}

TEST_F(AdaptiveIndexPredicateVisitorTest, ISNULL) {
    std::shared_ptr<AdaptiveIndexQuery> adaptiveQuery = toAdaptiveQuery("LOAD TWITTER CHOOSE ISNULL('/user/screen_name')");

    EXPECT_EQ(adaptiveQuery->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::ISNULL);
    EXPECT_EQ(adaptiveQuery->propertyPath, "/user/screen_name");
}

TEST_F(AdaptiveIndexPredicateVisitorTest, ISBOOL) {
    std::shared_ptr<AdaptiveIndexQuery> adaptiveQuery = toAdaptiveQuery("LOAD TWITTER CHOOSE ISBOOL('/user/screen_name')");

    EXPECT_EQ(adaptiveQuery->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::ISBOOL);
    EXPECT_EQ(adaptiveQuery->propertyPath, "/user/screen_name");
}

TEST_F(AdaptiveIndexPredicateVisitorTest, ISNUMBER) {
    std::shared_ptr<AdaptiveIndexQuery> adaptiveQuery = toAdaptiveQuery("LOAD TWITTER CHOOSE ISNUMBER('/user/screen_name')");

    EXPECT_EQ(adaptiveQuery->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::ISNUMBER);
    EXPECT_EQ(adaptiveQuery->propertyPath, "/user/screen_name");
}

TEST_F(AdaptiveIndexPredicateVisitorTest, ISOBJECT) {
    std::shared_ptr<AdaptiveIndexQuery> adaptiveQuery = toAdaptiveQuery("LOAD TWITTER CHOOSE ISOBJECT('/user/screen_name')");

    EXPECT_EQ(adaptiveQuery->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::ISOBJECT);
    EXPECT_EQ(adaptiveQuery->propertyPath, "/user/screen_name");
}

TEST_F(AdaptiveIndexPredicateVisitorTest, EXISTS) {
    std::shared_ptr<AdaptiveIndexQuery> adaptiveQuery = toAdaptiveQuery("LOAD TWITTER CHOOSE EXISTS('/user/screen_name')");

    EXPECT_EQ(adaptiveQuery->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::EXISTS);
    EXPECT_EQ(adaptiveQuery->propertyPath, "/user/screen_name");
}

TEST_F(AdaptiveIndexPredicateVisitorTest, AdaptiveVisitorGetFirstAnd) {
    AdaptiveIndexPredicateVisitor adaptiveVisitor;
    std::shared_ptr<joda::query::Query> q = parseQuery("LOAD TWITTER CHOOSE EXISTS('/user/screen_name') && ISNUMBER('/retweeted_status/user/friends_count')");
    adaptiveVisitor.visit(q->getChoose().get());

    std::shared_ptr<AdaptiveIndexQuery> adaptiveQuery = adaptiveVisitor.getQuery();

    EXPECT_EQ(adaptiveQuery->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::EXISTS);
    EXPECT_EQ(adaptiveQuery->propertyPath, "/user/screen_name");
    EXPECT_EQ(adaptiveQuery->conjunctionType, AdaptiveIndexQuery::CONJUNCTION_TYPE::AND);

    SetPredicateVisitor cutFirstVisitor(true);
    q->getPredicate()->accept(cutFirstVisitor);

    StaticEvalVisitor staticEvalVisitor;
    cutFirstVisitor.getPred()->accept(staticEvalVisitor);

    AdaptiveIndexPredicateVisitor adaptiveVisitor2;
    staticEvalVisitor.getPred()->accept(adaptiveVisitor2);

    std::shared_ptr<AdaptiveIndexQuery> adaptiveQuery2 = adaptiveVisitor2.getQuery();

    EXPECT_EQ(adaptiveQuery2->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::ISNUMBER);
    EXPECT_EQ(adaptiveQuery2->propertyPath, "/retweeted_status/user/friends_count");
    EXPECT_EQ(adaptiveQuery2->conjunctionType, AdaptiveIndexQuery::CONJUNCTION_TYPE::NONE);
}

TEST_F(AdaptiveIndexPredicateVisitorTest, AdaptiveVisitorGetFirstOr) {
    AdaptiveIndexPredicateVisitor adaptiveVisitor;
    std::shared_ptr<joda::query::Query> q = parseQuery("LOAD TWITTER CHOOSE EXISTS('/user/screen_name') || ISNUMBER('/retweeted_status/user/friends_count')");
    adaptiveVisitor.visit(q->getChoose().get());

    std::shared_ptr<AdaptiveIndexQuery> adaptiveQuery = adaptiveVisitor.getQuery();

    EXPECT_EQ(adaptiveQuery->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::EXISTS);
    EXPECT_EQ(adaptiveQuery->propertyPath, "/user/screen_name");
    EXPECT_EQ(adaptiveQuery->conjunctionType, AdaptiveIndexQuery::CONJUNCTION_TYPE::OR);

    SetPredicateVisitor cutFirstVisitor(true);
    q->getPredicate()->accept(cutFirstVisitor);

    StaticEvalVisitor staticEvalVisitor;
    cutFirstVisitor.getPred()->accept(staticEvalVisitor);

    AdaptiveIndexPredicateVisitor adaptiveVisitor2;
    staticEvalVisitor.getPred()->accept(adaptiveVisitor2);

    std::shared_ptr<AdaptiveIndexQuery> adaptiveQuery2 = adaptiveVisitor2.getQuery();

    if (adaptiveQuery2) {
        EXPECT_EQ(adaptiveQuery2->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::ISNUMBER);
        EXPECT_EQ(adaptiveQuery2->propertyPath, "/retweeted_status/user/friends_count");
        EXPECT_EQ(adaptiveQuery2->conjunctionType, AdaptiveIndexQuery::CONJUNCTION_TYPE::NONE);
    }
}*/

/*TEST_F(AdaptiveIndexPredicateVisitorTest, AdaptiveVisitorGetFirstAndOr) {
    AdaptiveIndexPredicateVisitor adaptiveVisitor;
    std::shared_ptr<joda::query::Query> q = parseQuery(
            "LOAD TWITTER CHOOSE EXISTS('/user/screen_name') || ISNUMBER('/retweeted_status/user/friends_count') && '/user/screen_name' == \"Testname\"");
    adaptiveVisitor.visit(q->getChoose().get());

    std::shared_ptr<AdaptiveIndexQuery> adaptiveQuery = adaptiveVisitor.getQuery();

    EXPECT_EQ(adaptiveQuery->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::EXISTS);
    EXPECT_EQ(adaptiveQuery->contentType, AdaptiveIndexQuery::CONTENT_TYPE::NONE);
    EXPECT_EQ(adaptiveQuery->propertyPath, "/user/screen_name");
    EXPECT_EQ(adaptiveQuery->conjunctionType, AdaptiveIndexQuery::CONJUNCTION_TYPE::OR);

    SetFirstTrueVisitor cutFirstVisitor;
    q->getPredicate()->accept(cutFirstVisitor);

    AdaptiveIndexPredicateVisitor adaptiveVisitor2;
    std::unique_ptr<Predicate> q2 = cutFirstVisitor.getPred();
    q2->accept(adaptiveVisitor2);

    std::shared_ptr<AdaptiveIndexQuery> adaptiveQuery2 = adaptiveVisitor2.getQuery();

    EXPECT_EQ(adaptiveQuery2->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::ISNUMBER);
    EXPECT_EQ(adaptiveQuery2->contentType, AdaptiveIndexQuery::CONTENT_TYPE::NONE);
    EXPECT_EQ(adaptiveQuery2->propertyPath, "/retweeted_status/user/friends_count");
    EXPECT_EQ(adaptiveQuery2->conjunctionType, AdaptiveIndexQuery::CONJUNCTION_TYPE::AND);

    SetFirstTrueVisitor cutFirstVisitor2;
    q2->accept(cutFirstVisitor2);

    AdaptiveIndexPredicateVisitor adaptiveVisitor3;
    cutFirstVisitor2.getPred()->accept(adaptiveVisitor3);

    std::shared_ptr<AdaptiveIndexQuery> adaptiveQuery3 = adaptiveVisitor3.getQuery();

    EXPECT_EQ(adaptiveQuery3->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::NONE);
    EXPECT_EQ(adaptiveQuery3->contentType, AdaptiveIndexQuery::CONTENT_TYPE::STRING);
    EXPECT_EQ(adaptiveQuery3->cmpType, AdaptiveIndexQuery::COMPARISON_TYPE::EQUALS);
    EXPECT_EQ(adaptiveQuery3->propertyPath, "/user/screen_name");
    EXPECT_EQ(adaptiveQuery3->conjunctionType, AdaptiveIndexQuery::CONJUNCTION_TYPE::NONE);
}

TEST_F(AdaptiveIndexPredicateVisitorTest, StaticEval) {

    AdaptiveIndexPredicateVisitor adaptiveVisitor1;
    StaticEvalVisitor staticEvalVisitor;

    std::shared_ptr<joda::query::Query> q = parseQuery("LOAD TWITTER CHOOSE EXISTS('/user/screen_name') && EXISTS('/retweeted_status/user/friends_count')");
    q->getPredicate()->accept(staticEvalVisitor);

    std::unique_ptr<Predicate> pred = staticEvalVisitor.getPred();

    AdaptiveIndexPredicateVisitor adaptiveVisitor2;
    pred->accept(adaptiveVisitor2);

    std::shared_ptr<AdaptiveIndexQuery> adaptiveQuery = adaptiveVisitor2.getQuery();;

    EXPECT_EQ(adaptiveQuery->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::EXISTS);
    EXPECT_EQ(adaptiveQuery->propertyPath, "/user/screen_name");
}

TEST_F(AdaptiveIndexPredicateVisitorTest, trueQuery) {
    std::shared_ptr<AdaptiveIndexQuery> adaptiveQuery = toAdaptiveQuery("LOAD TWITTER CHOOSE true && EXISTS('/user/screen_name')");
    EXPECT_EQ(adaptiveQuery->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::EXISTS);
    EXPECT_EQ(adaptiveQuery->propertyPath, "/user/screen_name");
}*/

TEST_F(AdaptiveIndexPredicateVisitorTest, QueryTreeBuilderSingle) {
    AdaptiveIndexQueryTreeVisitor adaptiveVisitor;
    std::shared_ptr<joda::query::Query> q = parseQuery("LOAD TWITTER CHOOSE ISSTRING('/user/screen_name')");
    adaptiveVisitor.visit(q->getChoose().get());

    std::shared_ptr<AdaptiveIndexQueryNode> adaptiveQueryNode = adaptiveVisitor.getRoot();
    AdaptiveIndexQuery &adaptiveQuery = *std::static_pointer_cast<ExecutableAdaptiveIndexQueryNode>(adaptiveQueryNode)->query;

    EXPECT_EQ(adaptiveQuery.functionType, AdaptiveIndexQuery::FUNCTION_TYPE::ISSTRING);
    EXPECT_EQ(adaptiveQuery.propertyPath, "/user/screen_name");
}

TEST_F(AdaptiveIndexPredicateVisitorTest, QueryTreeBuilderAnd) {
    AdaptiveIndexQueryTreeVisitor adaptiveVisitor;
    std::shared_ptr<joda::query::Query> q = parseQuery("LOAD TWITTER CHOOSE EXISTS('/user/screen_name') && ISNUMBER('/retweeted_status/user/friends_count')");
    adaptiveVisitor.visit(q->getChoose().get());

    std::shared_ptr<AdaptiveIndexQueryNode> adaptiveQueryNode = adaptiveVisitor.getRoot();
    AndAdaptiveIndexQueryNode &binaryNode = *std::static_pointer_cast<AndAdaptiveIndexQueryNode>(adaptiveQueryNode);


    ExecutableAdaptiveIndexQueryNode *nodeLhs = dynamic_cast<ExecutableAdaptiveIndexQueryNode*>(binaryNode.lhs.get());
    ExecutableAdaptiveIndexQueryNode *nodeRhs = dynamic_cast<ExecutableAdaptiveIndexQueryNode*>(binaryNode.rhs.get());

    EXPECT_EQ(nodeLhs->query->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::EXISTS);
    EXPECT_EQ(nodeLhs->query->propertyPath, "/user/screen_name");

    EXPECT_EQ(nodeRhs->query->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::ISNUMBER);
    EXPECT_EQ(nodeRhs->query->propertyPath, "/retweeted_status/user/friends_count");
}

TEST_F(AdaptiveIndexPredicateVisitorTest, QueryTreeBuilderOr) {
    AdaptiveIndexQueryTreeVisitor adaptiveVisitor;
    std::shared_ptr<joda::query::Query> q = parseQuery("LOAD TWITTER CHOOSE EXISTS('/user/screen_name') || ISNUMBER('/retweeted_status/user/friends_count')");
    adaptiveVisitor.visit(q->getChoose().get());

    std::shared_ptr<AdaptiveIndexQueryNode> adaptiveQueryNode = adaptiveVisitor.getRoot();
    OrAdaptiveIndexQueryNode &binaryNode = *std::static_pointer_cast<OrAdaptiveIndexQueryNode>(adaptiveQueryNode);


    ExecutableAdaptiveIndexQueryNode *nodeLhs = dynamic_cast<ExecutableAdaptiveIndexQueryNode*>(binaryNode.lhs.get());
    ExecutableAdaptiveIndexQueryNode *nodeRhs = dynamic_cast<ExecutableAdaptiveIndexQueryNode*>(binaryNode.rhs.get());

    EXPECT_EQ(nodeLhs->query->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::EXISTS);
    EXPECT_EQ(nodeLhs->query->propertyPath, "/user/screen_name");

    EXPECT_EQ(nodeRhs->query->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::ISNUMBER);
    EXPECT_EQ(nodeRhs->query->propertyPath, "/retweeted_status/user/friends_count");
}

TEST_F(AdaptiveIndexPredicateVisitorTest, QueryTreeBuilderNested) {
    AdaptiveIndexQueryTreeVisitor adaptiveVisitor;
    std::shared_ptr<joda::query::Query> q = parseQuery("LOAD TWITTER CHOOSE EXISTS('/user/screen_name') || ISNUMBER('/retweeted_status/user/friends_count') && '/user/screen_name' == \"Testname\"");
    adaptiveVisitor.visit(q->getChoose().get());

    std::shared_ptr<AdaptiveIndexQueryNode> adaptiveQueryNode = adaptiveVisitor.getRoot();
    OrAdaptiveIndexQueryNode &binaryNode = *std::static_pointer_cast<OrAdaptiveIndexQueryNode>(adaptiveQueryNode);

    ExecutableAdaptiveIndexQueryNode *nodeLhsExists = dynamic_cast<ExecutableAdaptiveIndexQueryNode*>(binaryNode.lhs.get());

    BinaryAdaptiveIndexQueryNode *nodeRhsAnd = dynamic_cast<BinaryAdaptiveIndexQueryNode*>(binaryNode.rhs.get());

    ExecutableAdaptiveIndexQueryNode *nodeRhsAndLhs = dynamic_cast<ExecutableAdaptiveIndexQueryNode*>(nodeRhsAnd->lhs.get());
    ExecutableAdaptiveIndexQueryNode *nodeRhsAndRhs = dynamic_cast<ExecutableAdaptiveIndexQueryNode*>(nodeRhsAnd->rhs.get());

    EXPECT_EQ(nodeLhsExists->query->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::EXISTS);
    EXPECT_EQ(nodeLhsExists->query->propertyPath, "/user/screen_name");

    EXPECT_EQ(nodeRhsAndLhs->query->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::ISNUMBER);
    EXPECT_EQ(nodeRhsAndLhs->query->propertyPath, "/retweeted_status/user/friends_count");

    EXPECT_EQ(nodeRhsAndRhs->query->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::NONE);
    EXPECT_EQ(nodeRhsAndRhs->query->contentType, AdaptiveIndexQuery::CONTENT_TYPE::STRING);
    EXPECT_EQ(nodeRhsAndRhs->query->cmpType, AdaptiveIndexQuery::COMPARISON_TYPE::EQUALS);
    EXPECT_EQ(nodeRhsAndRhs->query->propertyPath, "/user/screen_name");
}

TEST_F(AdaptiveIndexPredicateVisitorTest, QueryTreeBuilderNestedNot) {
    AdaptiveIndexQueryTreeVisitor adaptiveVisitor;
    std::shared_ptr<joda::query::Query> q = parseQuery("LOAD TWITTER CHOOSE EXISTS('/user/screen_name') || ISNUMBER('/retweeted_status/user/friends_count') && !'/user/screen_name' == \"Testname\"");
    adaptiveVisitor.visit(q->getChoose().get());

    std::shared_ptr<AdaptiveIndexQueryNode> adaptiveQueryNode = adaptiveVisitor.getRoot();
    OrAdaptiveIndexQueryNode &binaryNode = *std::static_pointer_cast<OrAdaptiveIndexQueryNode>(adaptiveQueryNode);

    ExecutableAdaptiveIndexQueryNode *nodeLhsExists = dynamic_cast<ExecutableAdaptiveIndexQueryNode*>(binaryNode.lhs.get());

    AndAdaptiveIndexQueryNode *nodeRhsAnd = dynamic_cast<AndAdaptiveIndexQueryNode*>(binaryNode.rhs.get());

    ExecutableAdaptiveIndexQueryNode *nodeRhsAndLhs = dynamic_cast<ExecutableAdaptiveIndexQueryNode*>(nodeRhsAnd->lhs.get());
    ExecutableAdaptiveIndexQueryNode *nodeRhsAndRhs = dynamic_cast<ExecutableAdaptiveIndexQueryNode*>(nodeRhsAnd->rhs.get());

    EXPECT_EQ(nodeLhsExists->query->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::EXISTS);
    EXPECT_EQ(nodeLhsExists->query->propertyPath, "/user/screen_name");
    EXPECT_EQ(nodeLhsExists->negated, false);

    EXPECT_EQ(nodeRhsAndLhs->query->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::ISNUMBER);
    EXPECT_EQ(nodeRhsAndLhs->query->propertyPath, "/retweeted_status/user/friends_count");
    EXPECT_EQ(nodeRhsAndLhs->negated, false);

    EXPECT_EQ(nodeRhsAndRhs->query->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::NONE);
    EXPECT_EQ(nodeRhsAndRhs->query->contentType, AdaptiveIndexQuery::CONTENT_TYPE::STRING);
    EXPECT_EQ(nodeRhsAndRhs->query->cmpType, AdaptiveIndexQuery::COMPARISON_TYPE::EQUALS);
    EXPECT_EQ(nodeRhsAndRhs->query->propertyPath, "/user/screen_name");
    EXPECT_EQ(nodeRhsAndRhs->negated, true);
}

TEST_F(AdaptiveIndexPredicateVisitorTest, QueryTreeBuilderNestedMultiNot) {
    AdaptiveIndexQueryTreeVisitor adaptiveVisitor;
    std::shared_ptr<joda::query::Query> q = parseQuery("LOAD TWITTER CHOOSE EXISTS('/user/screen_name') || !(ISNUMBER('/retweeted_status/user/friends_count') && !'/user/screen_name' == \"Testname\")");
    adaptiveVisitor.visit(q->getChoose().get());

    std::shared_ptr<AdaptiveIndexQueryNode> adaptiveQueryNode = adaptiveVisitor.getRoot();
    OrAdaptiveIndexQueryNode &binaryNode = *std::static_pointer_cast<OrAdaptiveIndexQueryNode>(adaptiveQueryNode);

    ExecutableAdaptiveIndexQueryNode *nodeLhsExists = dynamic_cast<ExecutableAdaptiveIndexQueryNode*>(binaryNode.lhs.get());

    AndAdaptiveIndexQueryNode *nodeRhsAnd = dynamic_cast<AndAdaptiveIndexQueryNode*>(binaryNode.rhs.get());
    EXPECT_EQ(nodeRhsAnd->negated, true);

    ExecutableAdaptiveIndexQueryNode *nodeRhsAndLhs = dynamic_cast<ExecutableAdaptiveIndexQueryNode*>(nodeRhsAnd->lhs.get());
    ExecutableAdaptiveIndexQueryNode *nodeRhsAndRhs = dynamic_cast<ExecutableAdaptiveIndexQueryNode*>(nodeRhsAnd->rhs.get());

    EXPECT_EQ(nodeLhsExists->query->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::EXISTS);
    EXPECT_EQ(nodeLhsExists->query->propertyPath, "/user/screen_name");
    EXPECT_EQ(nodeLhsExists->negated, false);

    EXPECT_EQ(nodeRhsAndLhs->query->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::ISNUMBER);
    EXPECT_EQ(nodeRhsAndLhs->query->propertyPath, "/retweeted_status/user/friends_count");
    EXPECT_EQ(nodeRhsAndLhs->negated, false);

    EXPECT_EQ(nodeRhsAndRhs->query->functionType, AdaptiveIndexQuery::FUNCTION_TYPE::NONE);
    EXPECT_EQ(nodeRhsAndRhs->query->contentType, AdaptiveIndexQuery::CONTENT_TYPE::STRING);
    EXPECT_EQ(nodeRhsAndRhs->query->cmpType, AdaptiveIndexQuery::COMPARISON_TYPE::EQUALS);
    EXPECT_EQ(nodeRhsAndRhs->query->propertyPath, "/user/screen_name");
    EXPECT_EQ(nodeRhsAndRhs->negated, true);
}

TEST_F(AdaptiveIndexPredicateVisitorTest, vecBool) {
    size_t size = 200000;

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<bool> docIndex(size);
    for (size_t j = 0; j < 1000; ++j) {
        for (size_t i = 0; i < size; i += 2) {
            docIndex[i] = true;
            docIndex[i + 1] = false;
        }
    }

    auto finish = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish-start);

    std::cout << microseconds.count() << std::endl;
}

TEST_F(AdaptiveIndexPredicateVisitorTest, bitset) {
    size_t size = 200000;

    auto start = std::chrono::high_resolution_clock::now();
    boost::dynamic_bitset<> DocIndex(size);
    for (size_t j = 0; j < 1000; ++j) {
        for (size_t i = 0; i < size; i += 2) {
            DocIndex.set(i);
            DocIndex.reset(i+1);
        }
    }

    auto finish = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish-start);

    std::cout << microseconds.count() << std::endl;
}

TEST_F(AdaptiveIndexPredicateVisitorTest, vecBoolFlip) {
    size_t size = 200000;

    std::vector<bool> docIndex(size);
    for (size_t i = 0; i < size; i += 2) {
        docIndex[i] = true;
        docIndex[i + 1] = false;
    }

    auto start = std::chrono::high_resolution_clock::now();

    for (size_t j = 0; j < 10000; ++j) {
        docIndex.flip();
    }

    auto finish = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish-start);

    std::cout << microseconds.count() << std::endl;
}

TEST_F(AdaptiveIndexPredicateVisitorTest, bitsetFlip) {
    size_t size = 200000;

    boost::dynamic_bitset<> DocIndex(size);
    for (size_t i = 0; i < size; i += 2) {
        DocIndex.set(i);
        DocIndex.reset(i+1);
    }

    auto start = std::chrono::high_resolution_clock::now();

    for (size_t j = 0; j < 10000; ++j) {
        DocIndex.flip();
    }

    auto finish = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish-start);

    std::cout << microseconds.count() << std::endl;
}

TEST_F(AdaptiveIndexPredicateVisitorTest, vecBoolAnd) {
    size_t size = 200000;

    std::vector<bool> docIndex(size);
    for (size_t i = 0; i < size; i += 2) {
        docIndex[i] = true;
        docIndex[i + 1] = false;
    }

    std::vector<bool> docIndex2(size);
    for (size_t i = 0; i < size; i += 2) {
        docIndex2[i] = true;
        docIndex2[i + 1] = false;
    }

    auto start = std::chrono::high_resolution_clock::now();

    for (size_t j = 0; j < 100; ++j) {
        std::vector<bool> docIndex3(size);

        for (size_t i = 0; i < docIndex2.size(); ++i) {
            if (docIndex[i] && docIndex2[i]) {
                docIndex3[i] = true;
            }
        }
    }

    auto finish = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish-start);

    std::cout << microseconds.count() << std::endl;
}

TEST_F(AdaptiveIndexPredicateVisitorTest, bitsetAnd) {
    size_t size = 200000;

    boost::dynamic_bitset<> DocIndex(size);
    for (size_t i = 0; i < size; i += 2) {
        DocIndex.set(i);
        DocIndex.reset(i+1);
    }

    boost::dynamic_bitset<> DocIndex2(size);
    for (size_t i = 0; i < size; i += 2) {
        DocIndex2.set(i);
        DocIndex2.reset(i+1);
    }

    auto start = std::chrono::high_resolution_clock::now();

    for (size_t j = 0; j < 100; ++j) {
        boost::dynamic_bitset<> bs = DocIndex & DocIndex2;
    }

    auto finish = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish-start);

    std::cout << microseconds.count() << std::endl;
}

TEST_F(AdaptiveIndexPredicateVisitorTest, vecBoolOr) {
    size_t size = 200000;

    std::vector<bool> docIndex(size);
    for (size_t i = 0; i < size; i += 2) {
        docIndex[i] = true;
        docIndex[i + 1] = false;
    }

    std::vector<bool> docIndex2(size);
    for (size_t i = 0; i < size; i += 2) {
        docIndex2[i] = true;
        docIndex2[i + 1] = false;
    }

    auto start = std::chrono::high_resolution_clock::now();

    for (size_t j = 0; j < 100; ++j) {
        std::vector<bool> docIndex3(size);

        for (size_t i = 0; i < docIndex2.size(); ++i) {
            if (docIndex[i] || docIndex2[i]) {
                docIndex3[i] = true;
            }
        }
    }

    auto finish = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish-start);

    std::cout << microseconds.count() << std::endl;
}

TEST_F(AdaptiveIndexPredicateVisitorTest, bitsetOr) {
    size_t size = 200000;

    boost::dynamic_bitset<> docIndex(size);
    for (size_t i = 0; i < size; i += 2) {
        docIndex.set(i);
        docIndex.reset(i+1);
    }

    boost::dynamic_bitset<> docIndex2(size);
    for (size_t i = 0; i < size; i += 2) {
        docIndex2.set(i);
        docIndex2.reset(i+1);
    }

    auto start = std::chrono::high_resolution_clock::now();

    for (size_t j = 0; j < 100; ++j) {
        boost::dynamic_bitset<> bs = docIndex | docIndex2;
    }

    auto finish = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish-start);

    std::cout << microseconds.count() << std::endl;
}

TEST_F(AdaptiveIndexPredicateVisitorTest, bitsetToVecBool) {
    size_t size = 20000000;

    boost::dynamic_bitset<> docIndexBit(size);
    for (size_t i = 0; i < size; i += 2) {
        docIndexBit.set(i);
        docIndexBit.reset(i+1);
    }
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<bool> docIndex(size);

    for (size_t j = 0; j < size; ++j) {
        docIndex[j] = docIndexBit[j];
    }


    auto finish = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish-start);

    std::cout << microseconds.count() << std::endl;
}