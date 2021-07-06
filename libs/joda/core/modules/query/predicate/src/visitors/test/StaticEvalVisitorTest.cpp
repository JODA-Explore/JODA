//
// Created by Nico Schäfer on 12/13/17.
//

#include <gtest/gtest.h>
#include <joda/query/values/IsXBoolProvider.h>

#include "../../../../include/joda/query/Query.h"
#include "../../../../values/include/joda/query/values/IValueProvider.h"
#include "../../../../values/include/joda/query/values/PointerProvider.h"
#include "../../../include/joda/query/predicate/PredFWD.h"
#include "../../../include/joda/query/predicate/StaticEvalVisitor.h"
#include "../../../include/joda/query/predicate/ToStringVisitor.h"
#include "joda/query/values/AtomProvider.h"

class StaticEvalVisitorTest : public ::testing::Test {
 protected:
  std::unique_ptr<joda::query::Predicate> IValToPred(
      std::unique_ptr<joda::query::IValueProvider>&& val) {
    return std::make_unique<joda::query::ValToPredicate>(std::move(val));
  }
  std::unique_ptr<joda::query::Predicate> getBoolPred(bool b) {
    return std::make_unique<joda::query::ValToPredicate>(b);
  }

  std::unique_ptr<joda::query::IValueProvider> getNumVal(double d) {
    return std::make_unique<joda::query::DoubleProvider>(d);
  }

  std::unique_ptr<joda::query::IValueProvider> getDynVal() {
    return std::make_unique<joda::query::PointerProvider>("");
  }

  std::unique_ptr<joda::query::Predicate> getDynPred() {
    typedef std::vector<std::unique_ptr<joda::query::IValueProvider>> IValVec;
    IValVec vec;
    vec.emplace_back(std::make_unique<joda::query::PointerProvider>(""));
    return IValToPred(
        std::make_unique<joda::query::IsXBoolProvider<joda::query::IV_Bool>>(
            std::move(vec)));
  }

  std::unique_ptr<joda::query::IValueProvider> getStringVal(std::string str) {
    return std::make_unique<joda::query::StringProvider>(str);
  }
  void checkEqual(const std::unique_ptr<joda::query::Predicate>& toTest,
                  const std::unique_ptr<joda::query::Predicate>& control) {
    joda::query::ToStringVisitor stringify;
    toTest->accept(stringify);
    auto testStr = stringify.popString();
    control->accept(stringify);
    auto contrStr = stringify.popString();
    auto to_test = testStr.c_str();
    auto control_string = testStr.c_str();
    EXPECT_STREQ(to_test, control_string);
  }
};

/*
 * AND Tests
 */

TEST_F(StaticEvalVisitorTest, TrueAndTrue) {
  std::unique_ptr<joda::query::Predicate> andPred =
      std::make_unique<joda::query::AndPredicate>(getBoolPred(true),
                                                  getBoolPred(true));
  joda::query::StaticEvalVisitor sev;
  andPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(true));  // T && T = T
}

TEST_F(StaticEvalVisitorTest, FalseAndTrue) {
  std::unique_ptr<joda::query::Predicate> andPred =
      std::make_unique<joda::query::AndPredicate>(getBoolPred(false),
                                                  getBoolPred(true));
  joda::query::StaticEvalVisitor sev;
  andPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(false));  // F && T = F
}

TEST_F(StaticEvalVisitorTest, TrueAndFalse) {
  std::unique_ptr<joda::query::Predicate> andPred =
      std::make_unique<joda::query::AndPredicate>(getBoolPred(true),
                                                  getBoolPred(false));
  joda::query::StaticEvalVisitor sev;
  andPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(false));  // T && F = F
}

TEST_F(StaticEvalVisitorTest, FalseAndFalse) {
  std::unique_ptr<joda::query::Predicate> andPred =
      std::make_unique<joda::query::AndPredicate>(getBoolPred(false),
                                                  getBoolPred(false));
  joda::query::StaticEvalVisitor sev;
  andPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(false));  // F && F = F
}

TEST_F(StaticEvalVisitorTest, TrueAndDyn) {
  std::unique_ptr<joda::query::Predicate> andPred =
      std::make_unique<joda::query::AndPredicate>(getBoolPred(true),
                                                  getDynPred());
  joda::query::StaticEvalVisitor sev;
  andPred->accept(sev);
  checkEqual(sev.getPred(),
             std::make_unique<joda::query::AndPredicate>(
                 getBoolPred(true), getDynPred()));  // T && D = T && D
}

TEST_F(StaticEvalVisitorTest, FalseAndDyn) {
  std::unique_ptr<joda::query::Predicate> andPred =
      std::make_unique<joda::query::AndPredicate>(getBoolPred(false),
                                                  getDynPred());
  joda::query::StaticEvalVisitor sev;
  andPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(false));  // F && D = F
}

TEST_F(StaticEvalVisitorTest, DynAndFalse) {
  std::unique_ptr<joda::query::Predicate> andPred =
      std::make_unique<joda::query::AndPredicate>(getDynPred(),
                                                  getBoolPred(false));
  joda::query::StaticEvalVisitor sev;
  andPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(false));  // D && F = F
}

TEST_F(StaticEvalVisitorTest, DynAndTrue) {
  std::unique_ptr<joda::query::Predicate> andPred =
      std::make_unique<joda::query::AndPredicate>(getDynPred(),
                                                  getBoolPred(false));
  joda::query::StaticEvalVisitor sev;
  andPred->accept(sev);
  checkEqual(sev.getPred(),
             std::make_unique<joda::query::AndPredicate>(
                 getDynPred(), getBoolPred(false)));  // D && T = D && T
}

TEST_F(StaticEvalVisitorTest, DynAndDyn) {
  std::unique_ptr<joda::query::Predicate> andPred =
      std::make_unique<joda::query::AndPredicate>(getDynPred(), getDynPred());
  joda::query::StaticEvalVisitor sev;
  andPred->accept(sev);
  checkEqual(sev.getPred(),
             std::make_unique<joda::query::AndPredicate>(
                 getDynPred(), getDynPred()));  // D && D = D && D
}

/*
 * OR Tests
 */
TEST_F(StaticEvalVisitorTest, TrueOrTrue) {
  std::unique_ptr<joda::query::Predicate> orPred =
      std::make_unique<joda::query::OrPredicate>(getBoolPred(true),
                                                 getBoolPred(true));
  joda::query::StaticEvalVisitor sev;
  orPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(true));  // T || T = T
}

TEST_F(StaticEvalVisitorTest, FalseOrTrue) {
  std::unique_ptr<joda::query::Predicate> orPred =
      std::make_unique<joda::query::OrPredicate>(getBoolPred(false),
                                                 getBoolPred(true));
  joda::query::StaticEvalVisitor sev;
  orPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(true));  // F || T = T
}

TEST_F(StaticEvalVisitorTest, TrueOrFalse) {
  std::unique_ptr<joda::query::Predicate> orPred =
      std::make_unique<joda::query::OrPredicate>(getBoolPred(true),
                                                 getBoolPred(false));
  joda::query::StaticEvalVisitor sev;
  orPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(true));  // T || F = T
}

TEST_F(StaticEvalVisitorTest, FalseOrFalse) {
  std::unique_ptr<joda::query::Predicate> orPred =
      std::make_unique<joda::query::OrPredicate>(getBoolPred(false),
                                                 getBoolPred(false));
  joda::query::StaticEvalVisitor sev;
  orPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(false));  // F || F = F
}

TEST_F(StaticEvalVisitorTest, TrueOrDyn) {
  std::unique_ptr<joda::query::Predicate> andPred =
      std::make_unique<joda::query::OrPredicate>(getBoolPred(true),
                                                 getDynPred());
  joda::query::StaticEvalVisitor sev;
  andPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(true));  // T || D = T
}

TEST_F(StaticEvalVisitorTest, FalseOrDyn) {
  std::unique_ptr<joda::query::Predicate> andPred =
      std::make_unique<joda::query::OrPredicate>(getBoolPred(false),
                                                 getDynPred());
  joda::query::StaticEvalVisitor sev;
  andPred->accept(sev);
  checkEqual(sev.getPred(), getDynPred());  // F || D = D
}

TEST_F(StaticEvalVisitorTest, DynOrFalse) {
  std::unique_ptr<joda::query::Predicate> andPred =
      std::make_unique<joda::query::OrPredicate>(getDynPred(),
                                                 getBoolPred(false));
  joda::query::StaticEvalVisitor sev;
  andPred->accept(sev);
  checkEqual(sev.getPred(), getDynPred());  // D || F = D
}

TEST_F(StaticEvalVisitorTest, DynOrTrue) {
  std::unique_ptr<joda::query::Predicate> andPred =
      std::make_unique<joda::query::OrPredicate>(getDynPred(),
                                                 getBoolPred(false));
  joda::query::StaticEvalVisitor sev;
  andPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(true));  // D || T = T
}

TEST_F(StaticEvalVisitorTest, DynOrDyn) {
  std::unique_ptr<joda::query::Predicate> andPred =
      std::make_unique<joda::query::OrPredicate>(getDynPred(), getDynPred());
  joda::query::StaticEvalVisitor sev;
  andPred->accept(sev);
  checkEqual(sev.getPred(),
             std::make_unique<joda::query::OrPredicate>(
                 getDynPred(), getDynPred()));  // D || D = D || D
}

/*
 * NOT Tests
 */

TEST_F(StaticEvalVisitorTest, NotTrue) {
  std::unique_ptr<joda::query::Predicate> notPred =
      std::make_unique<joda::query::NegatePredicate>(getBoolPred(true));
  joda::query::StaticEvalVisitor sev;
  notPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(false));  //! T = F
}

TEST_F(StaticEvalVisitorTest, NotFalse) {
  std::unique_ptr<joda::query::Predicate> notPred =
      std::make_unique<joda::query::NegatePredicate>(getBoolPred(false));
  joda::query::StaticEvalVisitor sev;
  notPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(true));  //! F = T
}

TEST_F(StaticEvalVisitorTest, NotDyn) {
  std::unique_ptr<joda::query::Predicate> notPred =
      std::make_unique<joda::query::NegatePredicate>(getDynPred());
  joda::query::StaticEvalVisitor sev;
  notPred->accept(sev);
  checkEqual(sev.getPred(), std::make_unique<joda::query::NegatePredicate>(
                                getDynPred()));  //! D = !D
}

/*
 * Equalize Test
 */
TEST_F(StaticEvalVisitorTest, EqualNumTrue) {
  std::unique_ptr<joda::query::Predicate> eqPred =
      std::make_unique<joda::query::EqualizePredicate>(getNumVal(1),
                                                       getNumVal(1), true);
  joda::query::StaticEvalVisitor sev;
  eqPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(true));  // 1 == 1 = T
}
TEST_F(StaticEvalVisitorTest, EqualNumFalse) {
  std::unique_ptr<joda::query::Predicate> eqPred =
      std::make_unique<joda::query::EqualizePredicate>(getNumVal(1),
                                                       getNumVal(2), true);
  joda::query::StaticEvalVisitor sev;
  eqPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(false));  // 1 == 2 = F
}

TEST_F(StaticEvalVisitorTest, NEqualNumTrue) {
  std::unique_ptr<joda::query::Predicate> eqPred =
      std::make_unique<joda::query::EqualizePredicate>(getNumVal(1),
                                                       getNumVal(2), false);
  joda::query::StaticEvalVisitor sev;
  eqPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(true));  // 1 != 2 = T
}
TEST_F(StaticEvalVisitorTest, NEqualNumFalse) {
  std::unique_ptr<joda::query::Predicate> eqPred =
      std::make_unique<joda::query::EqualizePredicate>(getNumVal(1),
                                                       getNumVal(1), false);
  joda::query::StaticEvalVisitor sev;
  eqPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(false));  // 1 != 1 = F
}

TEST_F(StaticEvalVisitorTest, EqualStringTrue) {
  std::unique_ptr<joda::query::Predicate> eqPred =
      std::make_unique<joda::query::EqualizePredicate>(
          getStringVal("test1"), getStringVal("test1"), true);
  joda::query::StaticEvalVisitor sev;
  eqPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(true));  //"test1 == "test1 = T
}
TEST_F(StaticEvalVisitorTest, EqualStringFalse) {
  std::unique_ptr<joda::query::Predicate> eqPred =
      std::make_unique<joda::query::EqualizePredicate>(
          getStringVal("test1"), getStringVal("test2"), true);
  joda::query::StaticEvalVisitor sev;
  eqPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(false));  //"test1 == "test2 = F
}

TEST_F(StaticEvalVisitorTest, NEqualStringTrue) {
  std::unique_ptr<joda::query::Predicate> eqPred =
      std::make_unique<joda::query::EqualizePredicate>(
          getStringVal("test1"), getStringVal("test2"), false);
  joda::query::StaticEvalVisitor sev;
  eqPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(true));  //"test1 != "test2 = T
}
TEST_F(StaticEvalVisitorTest, NEqualStringFalse) {
  std::unique_ptr<joda::query::Predicate> eqPred =
      std::make_unique<joda::query::EqualizePredicate>(
          getStringVal("test1"), getStringVal("test1"), false);
  joda::query::StaticEvalVisitor sev;
  eqPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(false));  //"test1" != "test1" = F
}

TEST_F(StaticEvalVisitorTest, EqualDyn) {
  std::unique_ptr<joda::query::Predicate> eqPred =
      std::make_unique<joda::query::EqualizePredicate>(getDynVal(), getDynVal(),
                                                       true);
  joda::query::StaticEvalVisitor sev;
  eqPred->accept(sev);
  checkEqual(sev.getPred(),
             std::make_unique<joda::query::EqualizePredicate>(
                 getDynVal(), getDynVal(), true));  // dyn == dyn = dyn == dyn
}

/*
 * Compare Test
 */
TEST_F(StaticEvalVisitorTest, OneGOne) {
  std::unique_ptr<joda::query::Predicate> compPred =
      std::make_unique<joda::query::ComparePredicate>(
          getNumVal(1), getNumVal(1), true, false);
  joda::query::StaticEvalVisitor sev;
  compPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(false));  // 1 > 1 = F
}

TEST_F(StaticEvalVisitorTest, OneGEOne) {
  std::unique_ptr<joda::query::Predicate> compPred =
      std::make_unique<joda::query::ComparePredicate>(getNumVal(1),
                                                      getNumVal(1), true, true);
  joda::query::StaticEvalVisitor sev;
  compPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(true));  // 1 >= 1 = T
}

TEST_F(StaticEvalVisitorTest, OneGETwo) {
  std::unique_ptr<joda::query::Predicate> compPred =
      std::make_unique<joda::query::ComparePredicate>(getNumVal(1),
                                                      getNumVal(2), true, true);
  joda::query::StaticEvalVisitor sev;
  compPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(false));  // 1 >= 2 = F
}

TEST_F(StaticEvalVisitorTest, OneGTwo) {
  std::unique_ptr<joda::query::Predicate> compPred =
      std::make_unique<joda::query::ComparePredicate>(
          getNumVal(1), getNumVal(2), true, false);
  joda::query::StaticEvalVisitor sev;
  compPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(false));  // 1 > 2 = F
}

TEST_F(StaticEvalVisitorTest, TwoGOne) {
  std::unique_ptr<joda::query::Predicate> compPred =
      std::make_unique<joda::query::ComparePredicate>(
          getNumVal(2), getNumVal(1), true, false);
  joda::query::StaticEvalVisitor sev;
  compPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(true));  // 2 > 1 = T
}

TEST_F(StaticEvalVisitorTest, TwoGEOne) {
  std::unique_ptr<joda::query::Predicate> compPred =
      std::make_unique<joda::query::ComparePredicate>(getNumVal(2),
                                                      getNumVal(1), true, true);
  joda::query::StaticEvalVisitor sev;
  compPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(true));  // 2 >= 1 = T
}

TEST_F(StaticEvalVisitorTest, OneLOne) {
  std::unique_ptr<joda::query::Predicate> compPred =
      std::make_unique<joda::query::ComparePredicate>(
          getNumVal(1), getNumVal(1), false, false);
  joda::query::StaticEvalVisitor sev;
  compPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(false));  // 1 < 1 = F
}

TEST_F(StaticEvalVisitorTest, OneLEOne) {
  std::unique_ptr<joda::query::Predicate> compPred =
      std::make_unique<joda::query::ComparePredicate>(
          getNumVal(1), getNumVal(1), false, true);
  joda::query::StaticEvalVisitor sev;
  compPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(true));  // 1 <= 1 = T
}

TEST_F(StaticEvalVisitorTest, OneLETwo) {
  std::unique_ptr<joda::query::Predicate> compPred =
      std::make_unique<joda::query::ComparePredicate>(
          getNumVal(1), getNumVal(2), false, true);
  joda::query::StaticEvalVisitor sev;
  compPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(true));  // 1 <= 2 = T
}

TEST_F(StaticEvalVisitorTest, OneLTwo) {
  std::unique_ptr<joda::query::Predicate> compPred =
      std::make_unique<joda::query::ComparePredicate>(
          getNumVal(1), getNumVal(2), false, false);
  joda::query::StaticEvalVisitor sev;
  compPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(true));  // 1 < 2 = T
}

TEST_F(StaticEvalVisitorTest, TwoLOne) {
  std::unique_ptr<joda::query::Predicate> compPred =
      std::make_unique<joda::query::ComparePredicate>(
          getNumVal(2), getNumVal(1), false, false);
  joda::query::StaticEvalVisitor sev;
  compPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(false));  // 2 < 1 = F
}

TEST_F(StaticEvalVisitorTest, TwoLEOne) {
  std::unique_ptr<joda::query::Predicate> compPred =
      std::make_unique<joda::query::ComparePredicate>(
          getNumVal(2), getNumVal(1), false, true);
  joda::query::StaticEvalVisitor sev;
  compPred->accept(sev);
  checkEqual(sev.getPred(), getBoolPred(false));  // 2 <= 1 = F
}

TEST_F(StaticEvalVisitorTest, CompDyn) {
  std::unique_ptr<joda::query::Predicate> compPred =
      std::make_unique<joda::query::ComparePredicate>(getDynVal(), getDynVal(),
                                                      false, true);
  joda::query::StaticEvalVisitor sev;
  compPred->accept(sev);
  checkEqual(sev.getPred(),
             std::make_unique<joda::query::ComparePredicate>(
                 getDynVal(), getDynVal(), false, true));  // D <= D = D <= D
}

/*
 * Misc Complex
 */

TEST_F(StaticEvalVisitorTest, MiscComplxI) {
  std::unique_ptr<joda::query::Predicate> andPred =
      std::make_unique<joda::query::AndPredicate>(
          getBoolPred(true), std::make_unique<joda::query::ComparePredicate>(
                                 getNumVal(2), getNumVal(1), false, true));
  auto eqPred = std::make_unique<joda::query::EqualizePredicate>(
      std::make_unique<joda::query::PointerProvider>("/tmp"),
      getStringVal("test2"), false);
  std::unique_ptr<joda::query::Predicate> orPred =
      std::make_unique<joda::query::OrPredicate>(std::move(andPred),
                                                 eqPred->duplicate());

  joda::query::StaticEvalVisitor sev;
  orPred->accept(sev);
  checkEqual(sev.getPred(),
             eqPred->duplicate());  //(T && 2<=1)|| '/tmp' != "test2"  => '/tmp'
                                    //!= "test2"
}