#include <gtest/gtest.h>
#include <joda/queryparsing/QueryParser.h>
#include <joda/queryexecution/QueryPlan.h>
#include <joda/config/ConfigParser.h>
#include <joda/fs/DirectoryRegister.h>
#include <joda/storage/collection/StorageCollection.h>

class AdaptiveSystemTests : public ::testing::Test {
public:
    AdaptiveSystemTests()  {
        char array[] = "joda";
        char *a  =  &array[0];
        char **argv = &a;
        FLAGS_minloglevel = 0;
        DirectoryRegister::getInstance();
        auto options =  ConfigParser::parseConfigs(1, argv);
        ConfigParser::setConfig(options);

        config::queryCache = false;
        config::adaptiveIndex = true;

        StorageCollection::getInstance().removeStorage("TWITTER");
    }

    static unsigned long executeQuery(const char* qstr) {
        QueryParser qp;
        auto q =  qp.parse(qstr);
        EXPECT_NE(q, nullptr) << qp.getLastError();
        if (q == nullptr) return 0;
        QueryPlan plan(q);
        return plan.executeQuery(nullptr);

    }

protected:
    void runTwitterQuery(const char * query, int resultSize) {

        ASSERT_FALSE(StorageCollection::getInstance().storageExists("TWITTER"));
        executeQuery( "LOAD TWITTER FROM FILES \"/home/sebastian/twitter\" LINESEPARATED");
        ASSERT_TRUE(StorageCollection::getInstance().storageExists("TWITTER"));

        for (int i = 0; i < 5; ++i) {
            std::cout << "Iteration " << (i + 1) << std::endl;

            auto resultId = executeQuery(query);
            auto result = StorageCollection::getInstance().getStorage(resultId);
            EXPECT_NE(result,nullptr);
            ASSERT_EQ(result->size(), resultSize);
            StorageCollection::getInstance().removeStorage(resultId);
        }

        StorageCollection::getInstance().removeStorage("TWITTER");
    }
};

TEST_F(AdaptiveSystemTests, Complex1) {
    runTwitterQuery("LOAD TWITTER CHOOSE '/user/screen_name' == \"J03LP1M3NT3L\" && SCONTAINS('/user/screen_name', \"T3L\")", 2);
}

TEST_F(AdaptiveSystemTests, Complex2) {
    runTwitterQuery("LOAD TWITTER CHOOSE '/user/screen_name' == \"ricardorossello\" || SCONTAINS('/user/screen_name', \"sello\")", 3);
}

TEST_F(AdaptiveSystemTests, Complex3) {
    runTwitterQuery("LOAD TWITTER CHOOSE '/user/screen_name' == \"J03LP1M3NT3L\" && SCONTAINS('/user/screen_name', \"sello\")", 0);
}

TEST_F(AdaptiveSystemTests, Complex4) {
    runTwitterQuery("LOAD TWITTER CHOOSE (('/retweeted_status/user/friends_count' <= 1 && SCONTAINS('/user/screen_name', \"T3L\")) || (!(EXISTS('/in_reply_to_user_id')) || SCONTAINS('/retweeted_status/user/location', \"Fra\")))", 10704);
}

TEST_F(AdaptiveSystemTests, Structure1) {
    runTwitterQuery("LOAD TWITTER CHOOSE '/retweeted_status/retweet_count' > 10000", 1417);
}

TEST_F(AdaptiveSystemTests, Structure2) {
    runTwitterQuery("LOAD TWITTER CHOOSE '/retweeted_status/user/verified' == true", 4880);
}

TEST_F(AdaptiveSystemTests, Structure3) {
    runTwitterQuery("LOAD TWITTER CHOOSE '/retweeted_status/user/friends_count' == 600", 15);
}

TEST_F(AdaptiveSystemTests, StartsWith) {
    runTwitterQuery("LOAD TWITTER CHOOSE STARTSWITH('/text', \"I voted @BTS_twt for the #BBMAs Top Social Artist Award #BTSBBMAs\")", 12);
}

TEST_F(AdaptiveSystemTests, BoolTrue) {
    runTwitterQuery("LOAD TWITTER CHOOSE '/user/verified' == true", 430);
}

TEST_F(AdaptiveSystemTests, BoolFalse) {
    runTwitterQuery("LOAD TWITTER CHOOSE '/user/verified' == false", 78789);
}

TEST_F(AdaptiveSystemTests, BoolNotTrue) {
    runTwitterQuery("LOAD TWITTER CHOOSE '/user/verified' != true", 89043);
}

TEST_F(AdaptiveSystemTests, BoolNotFalse) {
    runTwitterQuery("LOAD TWITTER CHOOSE '/user/verified' != false", 10684);
}

TEST_F(AdaptiveSystemTests, NumberSmallerNeg) {
    runTwitterQuery("LOAD TWITTER CHOOSE '/retweeted_status/user/friends_count' < -30", 0);
}

TEST_F(AdaptiveSystemTests, NumberBigger) {
    runTwitterQuery("LOAD TWITTER CHOOSE '/retweeted_status/user/friends_count' > 50000", 1252);
}

TEST_F(AdaptiveSystemTests, NumberEqual) {
    runTwitterQuery("LOAD TWITTER CHOOSE '/retweeted_status/user/friends_count' == 400", 38);
}

TEST_F(AdaptiveSystemTests, NumberNotEqual) {
    runTwitterQuery("LOAD TWITTER CHOOSE '/retweeted_status/user/friends_count' != 400", 89435);
}

TEST_F(AdaptiveSystemTests, NumberSmallerEqual) {
    runTwitterQuery("LOAD TWITTER CHOOSE '/retweeted_status/user/friends_count' <= 1", 1096);
}

TEST_F(AdaptiveSystemTests, NumberSmaller) {
    runTwitterQuery("LOAD TWITTER CHOOSE '/retweeted_status/user/friends_count' < 3", 1272);
}

TEST_F(AdaptiveSystemTests, NumberBiggerEqual) {
    runTwitterQuery("LOAD TWITTER CHOOSE '/retweeted_status/user/friends_count' >= 500000", 60);
}

TEST_F(AdaptiveSystemTests, FuncEXISTS) {
    runTwitterQuery("LOAD TWITTER CHOOSE EXISTS('/retweeted_status')", 39220);
}

TEST_F(AdaptiveSystemTests, FuncISNUMBER) {
    runTwitterQuery("LOAD TWITTER CHOOSE ISNUMBER('/retweeted_status/retweet_count')", 39220);
}

TEST_F(AdaptiveSystemTests, FuncTYPENUMBER) {
    runTwitterQuery("LOAD TWITTER CHOOSE TYPE('/retweeted_status/retweet_count') == \"NUMBER\"", 39220);
}

TEST_F(AdaptiveSystemTests, FuncISBOOL) {
    runTwitterQuery("LOAD TWITTER CHOOSE ISBOOL('/retweeted_status/is_quote_status')", 39220);
}

TEST_F(AdaptiveSystemTests, FuncTYPEBOOL) {
    runTwitterQuery("LOAD TWITTER CHOOSE TYPE('/retweeted_status/is_quote_status') == \"BOOL\"", 39220);
}

TEST_F(AdaptiveSystemTests, FuncISNULL) {
    runTwitterQuery("LOAD TWITTER CHOOSE ISNULL('/in_reply_to_user_id')", 69080);
}

TEST_F(AdaptiveSystemTests, FuncNOTISNULL) {
runTwitterQuery("LOAD TWITTER CHOOSE !ISNULL('/in_reply_to_user_id')", 20393);
}

TEST_F(AdaptiveSystemTests, FuncTYPENULL) {
    runTwitterQuery("LOAD TWITTER CHOOSE TYPE('/in_reply_to_user_id') == \"NULL\"", 69080);
}

TEST_F(AdaptiveSystemTests, FuncISSTRING) {
    runTwitterQuery("LOAD TWITTER CHOOSE ISSTRING('/retweeted_status/user/location')", 27578);
}

TEST_F(AdaptiveSystemTests, FuncTYPESTRING) {
    runTwitterQuery("LOAD TWITTER CHOOSE TYPE('/retweeted_status/user/location') == \"STRING\"", 27578);
}

TEST_F(AdaptiveSystemTests, FuncISOBJECT) {
    runTwitterQuery("LOAD TWITTER CHOOSE ISOBJECT('/retweeted_status/entities')", 39220);
}

TEST_F(AdaptiveSystemTests, FuncTYPEOBJECT) {
    runTwitterQuery("LOAD TWITTER CHOOSE TYPE('/retweeted_status/entities') == \"OBJECT\"", 39220);
}

TEST_F(AdaptiveSystemTests, FuncISARRAY) {
    runTwitterQuery("LOAD TWITTER CHOOSE ISARRAY('/retweeted_status/place/bounding_box/coordinates')", 1843);
}

TEST_F(AdaptiveSystemTests, FuncTYPEARRAY) {
    runTwitterQuery("LOAD TWITTER CHOOSE TYPE('/retweeted_status/place/bounding_box/coordinates') == \"ARRAY\"", 1843);
}

TEST_F(AdaptiveSystemTests, Unsupported1) {
    runTwitterQuery("LOAD TWITTER CHOOSE SCONTAINS('/user/screen_name', \"sello\")", 3);
}

/*TEST_F(AdaptiveSystemTests, StringEqual) {
runTwitterQuery("LOAD TWITTER CHOOSE TYPE('/user/screen_name') == \"RimaTupick52039\"", 1843);
}*/

// TODO: Test double-skipper: test <= 500 with double values, <= 500.0 with double; == 500 with double; == 500.0 with double;....