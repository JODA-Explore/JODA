#include <gtest/gtest.h>
#include <joda/query/values/IsXBoolProvider.h>

#include <joda/query/Query.h>
#include <joda/query/values/IValueProvider.h>
#include <joda/query/values/PointerProvider.h>
#include "joda/query/values/AtomProvider.h"
#include "joda/query/values/BinaryBoolProvider.h"
#include "joda/query/values/NotProvider.h"
#include "joda/query/values/ComparisonProvider.h"
#include "joda/query/values/EqualityProvider.h"


class OptimizeTest : public ::testing::Test {
 protected:

  std::unique_ptr<joda::query::IValueProvider> getBoolPred(bool b) {
    return std::make_unique<joda::query::BoolProvider>(b);
  }

  std::unique_ptr<joda::query::IValueProvider> getNumVal(double d) {
    return std::make_unique<joda::query::DoubleProvider>(d);
  }

  std::unique_ptr<joda::query::IValueProvider> getDynVal() {
    return std::make_unique<joda::query::PointerProvider>("");
  }

  std::unique_ptr<joda::query::IValueProvider> getDynPred() {
    typedef std::vector<std::unique_ptr<joda::query::IValueProvider>> IValVec;
    IValVec vec;
    vec.emplace_back(std::make_unique<joda::query::PointerProvider>(""));
    return
        std::make_unique<joda::query::IsXBoolProvider<joda::query::IV_Bool>>(
            std::move(vec));
  }

  std::unique_ptr<joda::query::IValueProvider> getStringVal(std::string str) {
    return std::make_unique<joda::query::StringProvider>(str);
  }
  void checkEqual(const std::unique_ptr<joda::query::IValueProvider>& toTest,
                  const std::unique_ptr<joda::query::IValueProvider>& control) {
    auto testStr = toTest->toString();
    auto contrStr = control->toString();
    auto to_test = testStr.c_str();
    auto control_string = testStr.c_str();
    EXPECT_STREQ(to_test, control_string);
  }

  std::unique_ptr<joda::query::IValueProvider> optimize(std::unique_ptr<joda::query::IValueProvider>& toOptimize) {
    auto opt = toOptimize->duplicate();
    auto new_opt = opt->optimize();
    if(new_opt != nullptr) {
      opt = std::move(new_opt);
    }
    return opt;
  }

  template<typename T>
   std::unique_ptr<joda::query::IValueProvider> make_provider(std::unique_ptr<joda::query::IValueProvider>&& lhs, std::unique_ptr<joda::query::IValueProvider>&& rhs){
     std::vector<std::unique_ptr<joda::query::IValueProvider>> vec;
      vec.emplace_back(std::move(lhs));
      vec.emplace_back(std::move(rhs));
      return std::make_unique<T>( std::move(vec));
   }

    template<typename T>
   std::unique_ptr<joda::query::IValueProvider> make_provider(std::unique_ptr<joda::query::IValueProvider>&& val){
     std::vector<std::unique_ptr<joda::query::IValueProvider>> vec;
      vec.emplace_back(std::move(val));
      return std::make_unique<T>( std::move(vec));
   }
};

/*
 * AND Tests
 */

TEST_F(OptimizeTest, TrueAndTrue) {
  std::unique_ptr<joda::query::IValueProvider> andPred =
      make_provider<joda::query::AndProvider>(getBoolPred(true),
                                                  getBoolPred(true));
  auto opt = optimize(andPred);
  checkEqual(opt, getBoolPred(true));  // T && T = T
}

TEST_F(OptimizeTest, FalseAndTrue) {
  std::unique_ptr<joda::query::IValueProvider> andPred =
      make_provider<joda::query::AndProvider>(getBoolPred(false),
                                                  getBoolPred(true));
  auto opt = optimize(andPred);
  checkEqual(opt, getBoolPred(false));  // F && T = F
}

TEST_F(OptimizeTest, TrueAndFalse) {
  std::unique_ptr<joda::query::IValueProvider> andPred =
      make_provider<joda::query::AndProvider>(getBoolPred(true),
                                                  getBoolPred(false));
  auto opt = optimize(andPred);
  checkEqual(opt, getBoolPred(false));  // T && F = F
}

TEST_F(OptimizeTest, FalseAndFalse) {
  std::unique_ptr<joda::query::IValueProvider> andPred =
      make_provider<joda::query::AndProvider>(getBoolPred(false),
                                                  getBoolPred(false));
  auto opt = optimize(andPred);
  checkEqual(opt, getBoolPred(false));  // F && F = F
}

TEST_F(OptimizeTest, TrueAndDyn) {
  std::unique_ptr<joda::query::IValueProvider> andPred =
      make_provider<joda::query::AndProvider>(getBoolPred(true),
                                                  getDynPred());
  auto opt = optimize(andPred);
  checkEqual(opt,
             make_provider<joda::query::AndProvider>(
                 getBoolPred(true), getDynPred()));  // T && D = T && D
}

TEST_F(OptimizeTest, FalseAndDyn) {
  std::unique_ptr<joda::query::IValueProvider> andPred =
      make_provider<joda::query::AndProvider>(getBoolPred(false),
                                                  getDynPred());
  auto opt = optimize(andPred);
  checkEqual(opt, getBoolPred(false));  // F && D = F
}

TEST_F(OptimizeTest, DynAndFalse) {
  std::unique_ptr<joda::query::IValueProvider> andPred =
      make_provider<joda::query::AndProvider>(getDynPred(),
                                                  getBoolPred(false));
  auto opt = optimize(andPred);
  checkEqual(opt, getBoolPred(false));  // D && F = F
}

TEST_F(OptimizeTest, DynAndTrue) {
  std::unique_ptr<joda::query::IValueProvider> andPred =
      make_provider<joda::query::AndProvider>(getDynPred(),
                                                  getBoolPred(false));
  auto opt = optimize(andPred);
  checkEqual(opt,
             make_provider<joda::query::AndProvider>(
                 getDynPred(), getBoolPred(false)));  // D && T = D && T
}

TEST_F(OptimizeTest, DynAndDyn) {
  std::unique_ptr<joda::query::IValueProvider> andPred =
      make_provider<joda::query::AndProvider>(getDynPred(), getDynPred());
  auto opt = optimize(andPred);
  checkEqual(opt,
             make_provider<joda::query::AndProvider>(
                 getDynPred(), getDynPred()));  // D && D = D && D
}

/*
 * OR Tests
 */
TEST_F(OptimizeTest, TrueOrTrue) {
  std::unique_ptr<joda::query::IValueProvider> orPred =
      make_provider<joda::query::OrProvider>(getBoolPred(true),
                                                 getBoolPred(true));
  auto opt = optimize(orPred);
  checkEqual(opt, getBoolPred(true));  // T || T = T
}

TEST_F(OptimizeTest, FalseOrTrue) {
  std::unique_ptr<joda::query::IValueProvider> orPred =
      make_provider<joda::query::OrProvider>(getBoolPred(false),
                                                 getBoolPred(true));
  auto opt = optimize(orPred);
  checkEqual(opt, getBoolPred(true));  // F || T = T
}

TEST_F(OptimizeTest, TrueOrFalse) {
  std::unique_ptr<joda::query::IValueProvider> orPred =
      make_provider<joda::query::OrProvider>(getBoolPred(true),
                                                 getBoolPred(false));
  auto opt = optimize(orPred);
  checkEqual(opt, getBoolPred(true));  // T || F = T
}

TEST_F(OptimizeTest, FalseOrFalse) {
  std::unique_ptr<joda::query::IValueProvider> orPred =
      make_provider<joda::query::OrProvider>(getBoolPred(false),
                                                 getBoolPred(false));
  auto opt = optimize(orPred);
  checkEqual(opt, getBoolPred(false));  // F || F = F
}

TEST_F(OptimizeTest, TrueOrDyn) {
  std::unique_ptr<joda::query::IValueProvider> andPred =
      make_provider<joda::query::OrProvider>(getBoolPred(true),
                                                 getDynPred());
  auto opt = optimize(andPred);
  checkEqual(opt, getBoolPred(true));  // T || D = T
}

TEST_F(OptimizeTest, FalseOrDyn) {
  std::unique_ptr<joda::query::IValueProvider> andPred =
      make_provider<joda::query::OrProvider>(getBoolPred(false),
                                                 getDynPred());
  auto opt = optimize(andPred);
  checkEqual(opt, getDynPred());  // F || D = D
}

TEST_F(OptimizeTest, DynOrFalse) {
  std::unique_ptr<joda::query::IValueProvider> andPred =
      make_provider<joda::query::OrProvider>(getDynPred(),
                                                 getBoolPred(false));
  auto opt = optimize(andPred);
  checkEqual(opt, getDynPred());  // D || F = D
}

TEST_F(OptimizeTest, DynOrTrue) {
  std::unique_ptr<joda::query::IValueProvider> andPred =
      make_provider<joda::query::OrProvider>(getDynPred(),
                                                 getBoolPred(false));
  auto opt = optimize(andPred);
  checkEqual(opt, getBoolPred(true));  // D || T = T
}

TEST_F(OptimizeTest, DynOrDyn) {
  std::unique_ptr<joda::query::IValueProvider> andPred =
      make_provider<joda::query::OrProvider>(getDynPred(), getDynPred());
  auto opt = optimize(andPred);
  checkEqual(opt,
             make_provider<joda::query::OrProvider>(
                 getDynPred(), getDynPred()));  // D || D = D || D
}

/*
 * NOT Tests
 */

TEST_F(OptimizeTest, NotTrue) {
  std::unique_ptr<joda::query::IValueProvider> notPred =
      make_provider<joda::query::NotProvider>(getBoolPred(true));
  auto opt = optimize(notPred);
  checkEqual(opt, getBoolPred(false));  //! T = F
}

TEST_F(OptimizeTest, NotFalse) {
  std::unique_ptr<joda::query::IValueProvider> notPred =
      make_provider<joda::query::NotProvider>(getBoolPred(false));
  auto opt = optimize(notPred);
  checkEqual(opt, getBoolPred(true));  //! F = T
}

TEST_F(OptimizeTest, NotDyn) {
  std::unique_ptr<joda::query::IValueProvider> notPred =
      make_provider<joda::query::NotProvider>(getDynPred());
  auto opt = optimize(notPred);
  checkEqual(opt, make_provider<joda::query::NotProvider>(
                                getDynPred()));  //! D = !D
}

/*
 * Equalize Test
 */
TEST_F(OptimizeTest, EqualNumTrue) {
  std::unique_ptr<joda::query::IValueProvider> eqPred =
      make_provider<joda::query::EqualProvider>(getNumVal(1),
                                                       getNumVal(1));
  auto opt = optimize(eqPred);
  checkEqual(opt, getBoolPred(true));  // 1 == 1 = T
}
TEST_F(OptimizeTest, EqualNumFalse) {
  std::unique_ptr<joda::query::IValueProvider> eqPred =
      make_provider<joda::query::EqualProvider>(getNumVal(1),
                                                       getNumVal(2));
  auto opt = optimize(eqPred);
  checkEqual(opt, getBoolPred(false));  // 1 == 2 = F
}

TEST_F(OptimizeTest, NEqualNumTrue) {
  std::unique_ptr<joda::query::IValueProvider> eqPred =
      make_provider<joda::query::UnequalProvider>(getNumVal(1),
                                                       getNumVal(2));
  auto opt = optimize(eqPred);
  checkEqual(opt, getBoolPred(true));  // 1 != 2 = T
}
TEST_F(OptimizeTest, NEqualNumFalse) {
  std::unique_ptr<joda::query::IValueProvider> eqPred =
      make_provider<joda::query::UnequalProvider>(getNumVal(1),
                                                       getNumVal(1));
  auto opt = optimize(eqPred);
  checkEqual(opt, getBoolPred(false));  // 1 != 1 = F
}

TEST_F(OptimizeTest, EqualStringTrue) {
  std::unique_ptr<joda::query::IValueProvider> eqPred =
      make_provider<joda::query::EqualProvider>(
          getStringVal("test1"), getStringVal("test1"));
  auto opt = optimize(eqPred);
  checkEqual(opt, getBoolPred(true));  //"test1 == "test1 = T
}
TEST_F(OptimizeTest, EqualStringFalse) {
  std::unique_ptr<joda::query::IValueProvider> eqPred =
      make_provider<joda::query::EqualProvider>(
          getStringVal("test1"), getStringVal("test2"));
  auto opt = optimize(eqPred);
  checkEqual(opt, getBoolPred(false));  //"test1 == "test2 = F
}

TEST_F(OptimizeTest, NEqualStringTrue) {
  std::unique_ptr<joda::query::IValueProvider> eqPred =
      make_provider<joda::query::UnequalProvider>(
          getStringVal("test1"), getStringVal("test2"));
  auto opt = optimize(eqPred);
  checkEqual(opt, getBoolPred(true));  //"test1 != "test2 = T
}
TEST_F(OptimizeTest, NEqualStringFalse) {
  std::unique_ptr<joda::query::IValueProvider> eqPred =
      make_provider<joda::query::UnequalProvider>(
          getStringVal("test1"), getStringVal("test1"));
  auto opt = optimize(eqPred);
  checkEqual(opt, getBoolPred(false));  //"test1" != "test1" = F
}

TEST_F(OptimizeTest, EqualDyn) {
  std::unique_ptr<joda::query::IValueProvider> eqPred =
      make_provider<joda::query::EqualProvider>(getDynVal(), getDynVal());
  auto opt = optimize(eqPred);
  checkEqual(opt,
             make_provider<joda::query::EqualProvider>(
                 getDynVal(), getDynVal()));  // dyn == dyn = dyn == dyn
}

/*
 * Compare Test
 */
TEST_F(OptimizeTest, OneGOne) {
  std::unique_ptr<joda::query::IValueProvider> compPred =
      make_provider<joda::query::GreaterProvider>(
          getNumVal(1), getNumVal(1));
  auto opt = optimize(compPred);
  checkEqual(opt, getBoolPred(false));  // 1 > 1 = F
}

TEST_F(OptimizeTest, OneGEOne) {
  std::unique_ptr<joda::query::IValueProvider> compPred =
      make_provider<joda::query::GreaterEqualProvider>(getNumVal(1),
                                                      getNumVal(1));
  auto opt = optimize(compPred);
  checkEqual(opt, getBoolPred(true));  // 1 >= 1 = T
}

TEST_F(OptimizeTest, OneGETwo) {
  std::unique_ptr<joda::query::IValueProvider> compPred =
      make_provider<joda::query::GreaterEqualProvider>(getNumVal(1),
                                                      getNumVal(2));
  auto opt = optimize(compPred);
  checkEqual(opt, getBoolPred(false));  // 1 >= 2 = F
}

TEST_F(OptimizeTest, OneGTwo) {
  std::unique_ptr<joda::query::IValueProvider> compPred =
      make_provider<joda::query::GreaterProvider>(
          getNumVal(1), getNumVal(2));
  auto opt = optimize(compPred);
  checkEqual(opt, getBoolPred(false));  // 1 > 2 = F
}

TEST_F(OptimizeTest, TwoGOne) {
  std::unique_ptr<joda::query::IValueProvider> compPred =
      make_provider<joda::query::GreaterProvider>(
          getNumVal(2), getNumVal(1));
  auto opt = optimize(compPred);
  checkEqual(opt, getBoolPred(true));  // 2 > 1 = T
}

TEST_F(OptimizeTest, TwoGEOne) {
  std::unique_ptr<joda::query::IValueProvider> compPred =
      make_provider<joda::query::GreaterEqualProvider>(getNumVal(2),
                                                      getNumVal(1));
  auto opt = optimize(compPred);
  checkEqual(opt, getBoolPred(true));  // 2 >= 1 = T
}

TEST_F(OptimizeTest, OneLOne) {
  std::unique_ptr<joda::query::IValueProvider> compPred =
      make_provider<joda::query::LessProvider>(
          getNumVal(1), getNumVal(1));
  auto opt = optimize(compPred);
  checkEqual(opt, getBoolPred(false));  // 1 < 1 = F
}

TEST_F(OptimizeTest, OneLEOne) {
  std::unique_ptr<joda::query::IValueProvider> compPred =
      make_provider<joda::query::LessEqualProvider>(
          getNumVal(1), getNumVal(1));
  auto opt = optimize(compPred);
  checkEqual(opt, getBoolPred(true));  // 1 <= 1 = T
}

TEST_F(OptimizeTest, OneLETwo) {
  std::unique_ptr<joda::query::IValueProvider> compPred =
      make_provider<joda::query::LessEqualProvider>(
          getNumVal(1), getNumVal(2));
  auto opt = optimize(compPred);
  checkEqual(opt, getBoolPred(true));  // 1 <= 2 = T
}

TEST_F(OptimizeTest, OneLTwo) {
  std::unique_ptr<joda::query::IValueProvider> compPred =
      make_provider<joda::query::LessProvider>(
          getNumVal(1), getNumVal(2));
  auto opt = optimize(compPred);
  checkEqual(opt, getBoolPred(true));  // 1 < 2 = T
}

TEST_F(OptimizeTest, TwoLOne) {
  std::unique_ptr<joda::query::IValueProvider> compPred =
      make_provider<joda::query::LessProvider>(
          getNumVal(2), getNumVal(1));
  auto opt = optimize(compPred);
  checkEqual(opt, getBoolPred(false));  // 2 < 1 = F
}

TEST_F(OptimizeTest, TwoLEOne) {
  std::unique_ptr<joda::query::IValueProvider> compPred =
      make_provider<joda::query::LessEqualProvider>(
          getNumVal(2), getNumVal(1));
  auto opt = optimize(compPred);
  checkEqual(opt, getBoolPred(false));  // 2 <= 1 = F
}

TEST_F(OptimizeTest, CompDyn) {
  std::unique_ptr<joda::query::IValueProvider> compPred =
      make_provider<joda::query::LessEqualProvider>(getDynVal(), getDynVal());
  auto opt = optimize(compPred);
  checkEqual(opt,
             make_provider<joda::query::LessEqualProvider>(
                 getDynVal(), getDynVal()));  // D <= D = D <= D
}

/*
 * Misc Complex
 */

TEST_F(OptimizeTest, MiscComplxI) {
  std::unique_ptr<joda::query::IValueProvider> andPred =
      make_provider<joda::query::AndProvider>(
          getBoolPred(true), make_provider<joda::query::LessEqualProvider>(
                                 getNumVal(2), getNumVal(1)));
  auto eqPred = make_provider<joda::query::UnequalProvider>(
      std::make_unique<joda::query::PointerProvider>("/tmp"),
      getStringVal("test2"));
  std::unique_ptr<joda::query::IValueProvider> orPred =
      make_provider<joda::query::OrProvider>(std::move(andPred),
                                                 eqPred->duplicate());

  auto opt = optimize(orPred);
  checkEqual(opt,
             eqPred->duplicate());  //(T && 2<=1)|| '/tmp' != "test2"  => '/tmp'
                                    //!= "test2"
}
