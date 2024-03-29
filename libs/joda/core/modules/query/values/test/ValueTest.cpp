//
// Created by Nico on 28/02/2019.
//

#include <gtest/gtest.h>
#include <joda/document/TemporaryOrigin.h>
#include <joda/query/values/ArraySizeProvider.h>
#include <joda/query/values/BinaryNumberProvider.h>
#include <joda/query/values/BinaryStringProvider.h>
#include <joda/query/values/ConstantNumber.h>
#include <joda/query/values/FileNameProvider.h>
#include <joda/query/values/FilePosProvider.h>
#include <joda/query/values/IDProvider.h>
#include <joda/query/values/INListProvider.h>
#include <joda/query/values/IValueProvider.h>
#include <joda/query/values/IsXBoolProvider.h>
#include <joda/query/values/ListAttributesProvider.h>
#include <joda/query/values/NullProvider.h>
#include <joda/query/values/RegexExtractProvider.h>
#include <joda/query/values/RegexExtractFirstProvider.h>
#include <joda/query/values/RegexProvider.h>
#include <joda/query/values/RegexReplaceProvider.h>
#include <joda/query/values/SeqNumberProvider.h>
#include <joda/query/values/SubStringProvider.h>
#include <joda/query/values/TypeProvider.h>
#include <joda/query/values/UnaryNumberProvider.h>
#include <joda/query/values/UnaryStringProvider.h>
#include <joda/query/values/CastProvider.h>
#include <joda/query/values/BinaryBoolProvider.h>
#include <joda/query/values/EqualityProvider.h>
#include <joda/query/values/ComparisonProvider.h>
#include <joda/query/values/NotProvider.h>
#include <joda/query/values/IteratorProvider.h>
#include <rapidjson/writer.h>
#include "IValueTestHelper.h"
#include "rapidjson/stringbuffer.h"
using namespace joda::query;

class DummyObject : public IValueProvider {
 public:
  RJValue getAtomValue(const RapidJsonDocument& /*json*/,
                       RJMemoryPoolAlloc& /*alloc*/) const override {
    return RJValue();
  }

  const RJValue* getValue(const RapidJsonDocument& /*json*/,
                          RJMemoryPoolAlloc& /*alloc*/) const override {
    return nullptr;
  }

  std::unique_ptr<IValueProvider> duplicate() const override {
    return std::unique_ptr<IValueProvider>();
  }

  std::string getName() const override { return "DummyObject"; }

  bool isConst() const override { return true; }

  IValueType getReturnType() const override { return IV_Object; }
};

class DummyArray : public IValueProvider {
 public:
  RJValue getAtomValue(const RapidJsonDocument& /*json*/,
                       RJMemoryPoolAlloc& /*alloc*/) const override {
    return RJValue();
  }

  const RJValue* getValue(const RapidJsonDocument& /*json*/,
                          RJMemoryPoolAlloc& /*alloc*/) const override {
    return nullptr;
  }

  std::unique_ptr<IValueProvider> duplicate() const override {
    return std::unique_ptr<IValueProvider>();
  }

  std::string getName() const override { return "DummyArray"; }

  bool isConst() const override { return true; }

  IValueType getReturnType() const override { return IV_Array; }
};

class DummyNull : public IValueProvider {
 public:
  RJValue getAtomValue(const RapidJsonDocument& /*json*/,
                       RJMemoryPoolAlloc& /*alloc*/) const override {
    return RJValue();
  }

  const RJValue* getValue(const RapidJsonDocument& /*json*/,
                          RJMemoryPoolAlloc& /*alloc*/) const override {
    return &val;
  }

  std::unique_ptr<IValueProvider> duplicate() const override {
    return std::unique_ptr<IValueProvider>();
  }

  std::string getName() const override { return "DummyNull"; }

  bool isConst() const override { return true; }

  IValueType getReturnType() const override { return IV_Null; }

 private:
  RJValue val{};
};

class ValueTest : public ::testing::Test {
 protected:
  ValueTest() {
    auto rjdoc = std::make_unique<RJDocument>();
    rjdoc->Parse(
        R"({"num":1,"str":"test","arr":[1,"arrstr",false,null],"bool":true,"null":null, "obj":{"nested":true}})");
    DCHECK(!rjdoc->HasParseError()) << rjdoc->GetParseError();
    doc = RapidJsonDocument(0, std::move(rjdoc),
                            std::make_unique<TemporaryOrigin>());
  }

 protected:
  RapidJsonDocument doc;
  using Params = std::vector<std::unique_ptr<IValueProvider>>;

  template <class T>
  auto createValue(const std::vector<IValueType>& paramTypes) {
    std::vector<std::unique_ptr<IValueProvider>> params;
    for (const auto& type : paramTypes) {
      switch (type) {
        case IV_String:
          params.emplace_back(IValueTestHelper::getStringVal("test"));
          break;
        case IV_Number:
          params.emplace_back(IValueTestHelper::getNumVal(1l));
          break;
        case IV_Bool:
          params.emplace_back(IValueTestHelper::getBoolVal(true));
          break;
        case IV_Array:
          params.emplace_back(std::make_unique<DummyArray>());
          break;
        case IV_Object:
          params.emplace_back(std::make_unique<DummyObject>());
          break;
        default:
          params.emplace_back(IValueTestHelper::getPointer("/tmp"));
          break;
      }
    }
    return T::_FACTORY(std::move(params));
  }

  template <class T, typename N1, typename N2, typename R>
  auto testBinaryAtom(N1 lhs, N2 rhs, R res) {
    std::stringstream ss;
    ss << "FUNC(" << lhs << "," << rhs << ") = " << res;
    SCOPED_TRACE(ss.str());
    std::vector<std::unique_ptr<IValueProvider>> params;
    params.emplace_back(std::make_unique<AtomProvider<N1>>(lhs));
    params.emplace_back(std::make_unique<AtomProvider<N2>>(rhs));
    auto func = T::_FACTORY(std::move(params));

    std::unique_ptr<IValueProvider> resObj =
        std::make_unique<AtomProvider<R>>(res);
    EXPECT_TRUE(checkFunction(func, resObj));
  }


  template <class T, typename N>
  auto testBinaryAtom(N lhs, N rhs) {
    std::stringstream ss;
    ss << "FUNC(" << lhs << "," << rhs << ") = null";
    SCOPED_TRACE(ss.str());
    std::vector<std::unique_ptr<IValueProvider>> params;
    params.emplace_back(std::make_unique<AtomProvider<N>>(lhs));
    params.emplace_back(std::make_unique<AtomProvider<N>>(rhs));
    auto func = T::_FACTORY(std::move(params));

    std::unique_ptr<IValueProvider> resObj = std::make_unique<DummyNull>();
    EXPECT_TRUE(checkFunction(func, resObj));
  }

  template <class T, typename N, typename R>
  auto testUnaryAtom(N val, R res) {
    std::stringstream ss;
    ss << "FUNC(" << val << ") = " << res;
    SCOPED_TRACE(ss.str());
    std::vector<std::unique_ptr<IValueProvider>> params;
    params.emplace_back(std::make_unique<AtomProvider<N>>(val));
    auto func = T::_FACTORY(std::move(params));

    std::unique_ptr<IValueProvider> resObj =
        std::make_unique<AtomProvider<R>>(res);
    EXPECT_TRUE(checkFunction(func, resObj));
  }

  template <class T, typename N>
  auto testUnaryAtom(N val) {
    std::stringstream ss;
    ss << "FUNC(" << val << ") = null";
    SCOPED_TRACE(ss.str());
    std::vector<std::unique_ptr<IValueProvider>> params;
    params.emplace_back(std::make_unique<AtomProvider<N>>(val));
    auto func = T::_FACTORY(std::move(params));

    std::unique_ptr<IValueProvider> resObj = std::make_unique<DummyNull>();
    EXPECT_TRUE(checkFunction(func, resObj));
  }

  template <class T>
  auto testParams(const std::vector<IValueType>& paramTypes,
                  bool testcount = true, bool checkAny = true) {
    std::vector<IValueType> t = {};
    /*
     * Check for less Params
     */

    int i = 0;
    while (t.size() < paramTypes.size()) {
      if (testcount) {
        EXPECT_THROW(createValue<T>(t), WrongParameterCountException);
      }

      t.push_back(paramTypes[i]);
      i++;
    }

    // Correct amount parameters
    EXPECT_NO_THROW(createValue<T>(t));

    /*
     * Check for more Params
     */
    if (testcount) {
      t.push_back(IV_Any);
      EXPECT_THROW(createValue<T>(t), WrongParameterCountException);
    }

    /*
     * Check types
     */
    if (!paramTypes.empty()) {
      for (size_t j = 0; j < paramTypes.size(); ++j) {
        if (paramTypes[j] != IV_Any) {
          if (checkAny) {
            auto t = paramTypes;
            t[j] = IV_Any;
            EXPECT_NO_THROW(
                createValue<T>(t));  // IV_Any should always be accepted
          }
          auto changedTypes = changeParam(paramTypes, j);
          for (const auto& item : changedTypes) {
            EXPECT_THROW(createValue<T>(item), WrongParameterTypeException);
          }
        }
      }
    }
  }

  std::vector<std::vector<IValueType>> changeParam(
      const std::vector<IValueType>& paramTypes, size_t i) {
    DCHECK(i >= 0 && i < paramTypes.size());

    std::vector<std::vector<IValueType>> allChanges = {};
    std::vector<IValueType> t = paramTypes;
    switch (paramTypes[i]) {
      case IV_String:
        t[i] = IV_Number;
        allChanges.push_back(t);
        t[i] = IV_Bool;
        allChanges.push_back(t);
        t[i] = IV_Object;
        allChanges.push_back(t);
        t[i] = IV_Array;
        allChanges.push_back(t);
        break;
      case IV_Number:
        t[i] = IV_String;
        allChanges.push_back(t);
        t[i] = IV_Bool;
        allChanges.push_back(t);
        t[i] = IV_Object;
        allChanges.push_back(t);
        t[i] = IV_Array;
        allChanges.push_back(t);
        break;
      case IV_Bool:
        t[i] = IV_Number;
        allChanges.push_back(t);
        t[i] = IV_String;
        allChanges.push_back(t);
        t[i] = IV_Object;
        allChanges.push_back(t);
        t[i] = IV_Array;
        allChanges.push_back(t);
        break;
      case IV_Object:
        t[i] = IV_Number;
        allChanges.push_back(t);
        t[i] = IV_Bool;
        allChanges.push_back(t);
        t[i] = IV_String;
        allChanges.push_back(t);
        t[i] = IV_Array;
        allChanges.push_back(t);
        break;
      case IV_Array:
        t[i] = IV_Number;
        allChanges.push_back(t);
        t[i] = IV_Bool;
        allChanges.push_back(t);
        t[i] = IV_Object;
        allChanges.push_back(t);
        t[i] = IV_String;
        allChanges.push_back(t);
        break;
      default:
        t[i] = IV_Null;
        allChanges.push_back(t);
        break;
    }
    return allChanges;
  }

  void checkType(std::unique_ptr<IValueProvider>& ival, IValueType type) {
    EXPECT_EQ(ival->getReturnType(), type);
    switch (type) {
      case IV_String:
        EXPECT_TRUE(ival->isString());
        EXPECT_FALSE(ival->isAny());
        EXPECT_FALSE(ival->isArray());
        EXPECT_FALSE(ival->isNumber());
        EXPECT_FALSE(ival->isBool());
        EXPECT_FALSE(ival->isObject());
        EXPECT_FALSE(ival->isNull());
        break;
      case IV_Number:
        EXPECT_TRUE(ival->isNumber());
        EXPECT_FALSE(ival->isAny());
        EXPECT_FALSE(ival->isArray());
        EXPECT_FALSE(ival->isString());
        EXPECT_FALSE(ival->isBool());
        EXPECT_FALSE(ival->isObject());
        EXPECT_FALSE(ival->isNull());
        break;
      case IV_Bool:
        EXPECT_TRUE(ival->isBool());
        EXPECT_FALSE(ival->isAny());
        EXPECT_FALSE(ival->isArray());
        EXPECT_FALSE(ival->isNumber());
        EXPECT_FALSE(ival->isString());
        EXPECT_FALSE(ival->isObject());
        EXPECT_FALSE(ival->isNull());
        break;
      case IV_Object:
        EXPECT_TRUE(ival->isObject());
        EXPECT_FALSE(ival->isAny());
        EXPECT_FALSE(ival->isArray());
        EXPECT_FALSE(ival->isNumber());
        EXPECT_FALSE(ival->isBool());
        EXPECT_FALSE(ival->isString());
        EXPECT_FALSE(ival->isNull());
        break;
      case IV_Array:
        EXPECT_TRUE(ival->isArray());
        EXPECT_FALSE(ival->isAny());
        EXPECT_FALSE(ival->isString());
        EXPECT_FALSE(ival->isNumber());
        EXPECT_FALSE(ival->isBool());
        EXPECT_FALSE(ival->isObject());
        EXPECT_FALSE(ival->isNull());
        break;
      case IV_Any:
        EXPECT_TRUE(ival->isAny());
        EXPECT_FALSE(ival->isString());
        EXPECT_FALSE(ival->isArray());
        EXPECT_FALSE(ival->isNumber());
        EXPECT_FALSE(ival->isBool());
        EXPECT_FALSE(ival->isObject());
        EXPECT_FALSE(ival->isNull());
        break;
      case IV_Null:
        EXPECT_TRUE(ival->isNull());
        EXPECT_FALSE(ival->isAny());
        EXPECT_FALSE(ival->isString());
        EXPECT_FALSE(ival->isArray());
        EXPECT_FALSE(ival->isNumber());
        EXPECT_FALSE(ival->isBool());
        EXPECT_FALSE(ival->isObject());
        break;
    }
  }

  std::string IValueToValueString(std::unique_ptr<IValueProvider>& ival) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    if (ival->isAtom()) {
      auto v = ival->getAtomValue(doc, doc.getJson()->GetAllocator());
      v.Accept(writer);
    } else {
      auto v = ival->getValue(doc, doc.getJson()->GetAllocator());
      v->Accept(writer);
    }

    return buffer.GetString();
  }

  void checkDuplicate(std::unique_ptr<IValueProvider>& ival) {
    auto dupe = ival->duplicate();
    EXPECT_STREQ(ival->getName().c_str(), dupe->getName().c_str());
    EXPECT_STREQ(ival->toString().c_str(), dupe->toString().c_str());
    EXPECT_TRUE(ival->equal(dupe.get(), doc))
        << "OriginalValue: " << IValueToValueString(ival)
        << " ; DuplicatedValue: " << IValueToValueString(dupe);
  }

  template<typename R>
  ::testing::AssertionResult checkFunctionWithAtomRes(
      std::unique_ptr<IValueProvider>& ival,
      R res) {
    std::unique_ptr<IValueProvider> resObj =
        std::make_unique<AtomProvider<R>>(res);
    return checkFunction(ival, resObj);
  }

  ::testing::AssertionResult checkFunction(
      std::unique_ptr<IValueProvider>& ival,
      std::unique_ptr<IValueProvider>& res) {
    DCHECK(res->isConst());
    if (ival->equal(res.get(), doc)) {
      return ::testing::AssertionSuccess();
    }
    {
      return ::testing::AssertionFailure()
             << "IVal: " << IValueToValueString(ival)
             << " ; Expected: " << IValueToValueString(res);
    }
  }
};

TEST_F(ValueTest, BasicTypeTest) {
  ///////Basic Types
  {
    SCOPED_TRACE("BoolVal");
    auto bVal = IValueTestHelper::getBoolVal(true);
    checkType(bVal, IV_Bool);
  }

  {
    SCOPED_TRACE("StringVal");
    auto strVal = IValueTestHelper::getStringVal("str");
    checkType(strVal, IV_String);
  }

  {
    SCOPED_TRACE("DoubleVal");
    auto numVal = IValueTestHelper::getNumVal(1.0);
    checkType(numVal, IV_Number);
  }

  {
    SCOPED_TRACE("uInt64Val");
    auto numVal = IValueTestHelper::getNumVal(1ul);
    checkType(numVal, IV_Number);
  }

  {
    SCOPED_TRACE("Int64Val");
    auto numVal = IValueTestHelper::getNumVal(-1l);
    checkType(numVal, IV_Number);
  }

  {
    SCOPED_TRACE("Pointer");
    auto ptrVal = IValueTestHelper::getPointer("/tmp");
    checkType(ptrVal, IV_Any);
  }

  {
    SCOPED_TRACE("Null");
    std::unique_ptr<IValueProvider> nullVal = std::make_unique<NullProvider>();
    checkType(nullVal, IV_Null);
  }
}

TEST_F(ValueTest, ConstantTest) {
  {
    SCOPED_TRACE("Pi");
    std::unique_ptr<IValueProvider> piVal = std::make_unique<PiProvider>();
    checkType(piVal, IV_Number);
    std::unique_ptr<IValueProvider> resObj =
        std::make_unique<AtomProvider<double>>(M_PI);
    EXPECT_TRUE(checkFunction(piVal, resObj));
  }

  {
    SCOPED_TRACE("Now");
    std::unique_ptr<IValueProvider> nowVal = std::make_unique<NowProvider>();
    auto ts = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());
    checkType(nowVal, IV_Number);
    RJMemoryPoolAlloc alloc;
    auto val = nowVal->getAtomValue(doc, alloc);
    EXPECT_TRUE(val.IsInt64() && val.GetInt64() > ts.count() - 10 &&
                val.GetInt64() < ts.count() + 10);
  }
}

TEST_F(ValueTest, ArraySizeProvider) {
  SCOPED_TRACE("ArraySizeProvider");
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getPointer("/arr"));
  auto ival = ArraySizeProvider::_FACTORY(std::move(p));
  checkType(ival, IV_Number);

  // Parameters
  std::vector<IValueType> t = {IV_Array};
  testParams<ArraySizeProvider>(t);

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  auto res = IValueTestHelper::getNumVal(4l);
  EXPECT_TRUE(checkFunction(ival, res));
}

TEST_F(ValueTest, DivProvider) {
  SCOPED_TRACE("DivProvider");
  using TestType = DivProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getPointer("/num"),
                          IValueTestHelper::getNumVal(2l));
  auto ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Number);

  // Parameters
  std::vector<IValueType> t = {IV_Number, IV_Number};
  testParams<TestType>(t, false);

  // Duplicate
  checkDuplicate(ival);

  // Check Function

  {
    testBinaryAtom<TestType, double, double, double>(1, 2, 0.5);
    testBinaryAtom<TestType, int64_t, int64_t, int64_t>(1, 2, 0);
    testBinaryAtom<TestType, u_int64_t, u_int64_t, u_int64_t>(1, 2, 0);
  }

  // 1/0 => NullPtr
  {
    testBinaryAtom<TestType, double>(1, 0);
    testBinaryAtom<TestType, int64_t>(1, 0);
    testBinaryAtom<TestType, u_int64_t>(1, 0);
  }

  //-4/2 => -2
  {
    testBinaryAtom<TestType, double, double, double>(-4, 2, -2);
    testBinaryAtom<TestType, int64_t, int64_t, int64_t>(-4, 2, -2);
  }
}

TEST_F(ValueTest, ProductProvider) {
  SCOPED_TRACE("ProductProvider");
  using TestType = ProductProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getNumVal(2l),
                          IValueTestHelper::getNumVal(2l));
  std::unique_ptr<IValueProvider> ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Number);

  // Parameters
  std::vector<IValueType> t = {IV_Number, IV_Number};
  testParams<TestType>(t, false);

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  // 2*2 = 4
  {
    testBinaryAtom<TestType, double, double, double>(2, 2, 4);
    testBinaryAtom<TestType, int64_t, int64_t, int64_t>(2, 2, 4);
    testBinaryAtom<TestType, u_int64_t, u_int64_t, u_int64_t>(2, 2, 4);
  }

  // 1*0 => 0
  {
    testBinaryAtom<TestType, double, double, double>(1, 0, 0);
    testBinaryAtom<TestType, int64_t, int64_t, int64_t>(1, 0, 0);
    testBinaryAtom<TestType, u_int64_t, u_int64_t, u_int64_t>(1, 0, 0);
  }

  //-4*2 => -8
  {
    testBinaryAtom<TestType, double, double, double>(-4, 2, -8);
    testBinaryAtom<TestType, int64_t, int64_t, int64_t>(-4, 2, -8);
  }
}

TEST_F(ValueTest, SumProvider) {
  SCOPED_TRACE("SumProvider");
  using TestType = SumProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getNumVal(2l),
                          IValueTestHelper::getNumVal(2l));
  auto ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Number);

  // Parameters
  std::vector<IValueType> t = {IV_Number, IV_Number};
  testParams<TestType>(t, false);

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  // 3+2 = 5
  {
    testBinaryAtom<TestType, double, double, double>(3, 2, 5);
    testBinaryAtom<TestType, int64_t, int64_t, int64_t>(3, 2, 5);
    testBinaryAtom<TestType, u_int64_t, u_int64_t, u_int64_t>(3, 2, 5);
  }

  // 1+0 => 1
  {
    testBinaryAtom<TestType, double, double, double>(1, 0, 1);
    testBinaryAtom<TestType, int64_t, int64_t, int64_t>(1, 0, 1);
    testBinaryAtom<TestType, u_int64_t, u_int64_t, u_int64_t>(1, 0, 1);
  }

  //-4+2 => -2
  {
    testBinaryAtom<TestType, double, double, double>(-4, 2, -2);
    testBinaryAtom<TestType, int64_t, int64_t, int64_t>(-4, 2, -2);
  }
}

TEST_F(ValueTest, SubtractProvider) {
  SCOPED_TRACE("SubtractProvider");
  using TestType = SubtractProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getNumVal(2l),
                          IValueTestHelper::getNumVal(2l));
  auto ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Number);

  // Parameters
  std::vector<IValueType> t = {IV_Number, IV_Number};
  testParams<TestType>(t, false);

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  // 2-2 = 0
  {
    testBinaryAtom<TestType, double, double, double>(2, 2, 0);
    testBinaryAtom<TestType, int64_t, int64_t, int64_t>(2, 2, 0);
    testBinaryAtom<TestType, u_int64_t, u_int64_t, u_int64_t>(2, 2, 0);
  }

  // 1-0 => 1
  {
    testBinaryAtom<TestType, double, double, double>(1, 0, 1);
    testBinaryAtom<TestType, int64_t, int64_t, int64_t>(1, 0, 1);
    testBinaryAtom<TestType, u_int64_t, u_int64_t, u_int64_t>(1, 0, 1);
  }

  //-4-2 => -6
  {
    testBinaryAtom<TestType, double, double, double>(-4, 2, -6);
    testBinaryAtom<TestType, u_int64_t, u_int64_t, u_int64_t>(-4, 2, -6);
  }
}

TEST_F(ValueTest, ModuloProvider) {
  const auto FuncName = "ModuloProvider";
  using TestType = ModuloProvider;
  SCOPED_TRACE(FuncName);
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getNumVal(2l),
                          IValueTestHelper::getNumVal(2l));
  auto ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Number);

  // Parameters
  std::vector<IValueType> t = {IV_Number, IV_Number};
  testParams<TestType>(t, false);

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  // 2%2 = 0
  {
    testBinaryAtom<TestType, double, double, double>(2, 2, 0);
    testBinaryAtom<TestType, int64_t, int64_t, int64_t>(2, 2, 0);
    testBinaryAtom<TestType, u_int64_t, u_int64_t, u_int64_t>(2, 2, 0);
  }

  // 3%2 => 1
  {
    testBinaryAtom<TestType, double, double, double>(3, 2, 1);
    testBinaryAtom<TestType, int64_t, int64_t, int64_t>(3, 2, 1);
    testBinaryAtom<TestType, u_int64_t, u_int64_t, u_int64_t>(3, 2, 1);
  }

  //-182%-24 => -14
  {
    testBinaryAtom<TestType, double, double, double>(-182, -24, -14);
    testBinaryAtom<TestType, int64_t, int64_t, int64_t>(-182, -24, -14);
  }

  // 6.7%2.553 =>1.594
  {
    testBinaryAtom<TestType, double, double, double>(6.7, 2.553, std::fmod(6.7, 2.553));
  }
}

TEST_F(ValueTest, FileNameProvider) {
  SCOPED_TRACE("FileNameProvider");
  // Return Type
  Params p = {};
  auto ival = FileNameProvider::_FACTORY(std::move(p));
  checkType(ival, IV_String);

  // Parameters
  std::vector<IValueType> t = {};
  testParams<FileNameProvider>(t);

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  {
    // Temporary FileT
    std::unique_ptr<IValueProvider> res = std::make_unique<DummyNull>();
    EXPECT_TRUE(checkFunction(ival, res));
  }

  // TODO: FileName function (Only integration test?)
}

TEST_F(ValueTest, FilePosProvider_true) {
  SCOPED_TRACE("FilePosProvider<true>");
  // Return Type
  Params p = {};
  auto ival = FilePosProvider<true>::_FACTORY(std::move(p));
  checkType(ival, IV_Number);

  // Parameters
  std::vector<IValueType> t = {};
  testParams<FilePosProvider<true>>(t);

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  {
    // Temporary FileT
    std::unique_ptr<IValueProvider> res = std::make_unique<DummyNull>();
    EXPECT_TRUE(checkFunction(ival, res));
  }

  // TODO: FilePos function (Only integration test?)
}

TEST_F(ValueTest, FilePosProvider_false) {
  SCOPED_TRACE("FilePosProvider<false>");
  // Return Type
  Params p = {};
  auto ival = FilePosProvider<false>::_FACTORY(std::move(p));
  checkType(ival, IV_Number);

  // Parameters
  std::vector<IValueType> t = {};
  testParams<FilePosProvider<false>>(t);

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  {
    // Temporary FileT
    std::unique_ptr<IValueProvider> res = std::make_unique<DummyNull>();
    EXPECT_TRUE(checkFunction(ival, res));
  }

  // TODO: FilePos function (Only integration test?)
}

TEST_F(ValueTest, IDProvider) {
  SCOPED_TRACE("IDProvider");
  // Return Type
  Params p = {};
  auto ival = IDProvider::_FACTORY(std::move(p));
  checkType(ival, IV_Number);

  // Parameters
  std::vector<IValueType> t = {};
  testParams<IDProvider>(t);

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  {
    // Temporary FileT
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getNumVal(0l);
    EXPECT_TRUE(checkFunction(ival, res));
  }
}

TEST_F(ValueTest, INListProvider) {
  SCOPED_TRACE("INListProvider");
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getNumVal(1l),
                          IValueTestHelper::getPointer("/arr"));
  auto ival = INListProvider::_FACTORY(std::move(p));
  checkType(ival, IV_Bool);

  // Parameters
  std::vector<IValueType> t = {IV_Any, IV_Array};
  testParams<INListProvider>(t);

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  {
    SCOPED_TRACE("INListProvider_Func1");
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(true);
    EXPECT_TRUE(checkFunction(ival, res));
  }

  {
    SCOPED_TRACE("INListProvider_Func2");
    // str
    IValueTestHelper::param(p, IValueTestHelper::getStringVal("arrstr"),
                            IValueTestHelper::getPointer("/arr"));
    ival = INListProvider::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(true);
    EXPECT_TRUE(checkFunction(ival, res));
  }

  {
    SCOPED_TRACE("INListProvider_Func3");
    // null
    IValueTestHelper::param(p, std::make_unique<DummyNull>(),
                            IValueTestHelper::getPointer("/arr"));
    ival = INListProvider::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(true);
    EXPECT_TRUE(checkFunction(ival, res));
  }

  {
    SCOPED_TRACE("INListProvider_Func4");
    // false
    IValueTestHelper::param(p, IValueTestHelper::getBoolVal(false),
                            IValueTestHelper::getPointer("/arr"));
    ival = INListProvider::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(true);
    EXPECT_TRUE(checkFunction(ival, res));
  }

  {
    SCOPED_TRACE("INListProvider_Func5");
    // false
    IValueTestHelper::param(p, IValueTestHelper::getNumVal(4l),
                            IValueTestHelper::getPointer("/arr"));
    ival = INListProvider::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
}

TEST_F(ValueTest, IsXBoolProvider_IV_String) {
  SCOPED_TRACE("IsXBoolProvider<IV_String>");
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getPointer("/str"));
  auto ival = IsXBoolProvider<IV_String>::_FACTORY(std::move(p));
  checkType(ival, IV_Bool);

  // Parameters
  std::vector<IValueType> t = {IV_Any};
  testParams<IsXBoolProvider<IV_String>>(t);

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  {
    SCOPED_TRACE("IsXBoolProvider<IV_String>_Func1");
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(true);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_String>_Func2");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/num"));
    ival = IsXBoolProvider<IV_String>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_String>_Func3");
    IValueTestHelper::param(p, IValueTestHelper::getPointer(""));
    ival = IsXBoolProvider<IV_String>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_String>_Func4");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/arr"));
    ival = IsXBoolProvider<IV_String>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_String>_Func5");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/null"));
    ival = IsXBoolProvider<IV_String>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_String>_Func6");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/bool"));
    ival = IsXBoolProvider<IV_String>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
}

TEST_F(ValueTest, IsXBoolProvider_IV_Number) {
  SCOPED_TRACE("IsXBoolProvider<IV_Number>");
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getPointer("/str"));
  auto ival = IsXBoolProvider<IV_Number>::_FACTORY(std::move(p));
  checkType(ival, IV_Bool);

  // Parameters
  std::vector<IValueType> t = {IV_Any};
  testParams<IsXBoolProvider<IV_Number>>(t);

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Number>_Func1");
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Number>_Func2");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/num"));
    ival = IsXBoolProvider<IV_Number>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(true);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Number>_Func3");
    IValueTestHelper::param(p, IValueTestHelper::getPointer(""));
    ival = IsXBoolProvider<IV_Number>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Number>_Func4");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/arr"));
    ival = IsXBoolProvider<IV_Number>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Number>_Func5");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/null"));
    ival = IsXBoolProvider<IV_Number>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Number>_Func6");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/bool"));
    ival = IsXBoolProvider<IV_Number>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
}

TEST_F(ValueTest, IsXBoolProvider_IV_Bool) {
  SCOPED_TRACE("IsXBoolProvider<IV_Bool>");
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getPointer("/str"));
  auto ival = IsXBoolProvider<IV_Bool>::_FACTORY(std::move(p));
  checkType(ival, IV_Bool);

  // Parameters
  std::vector<IValueType> t = {IV_Any};
  testParams<IsXBoolProvider<IV_Bool>>(t);

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Bool>_Func1");
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Bool>_Func2");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/num"));
    ival = IsXBoolProvider<IV_Bool>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Bool>_Func3");
    IValueTestHelper::param(p, IValueTestHelper::getPointer(""));
    ival = IsXBoolProvider<IV_Bool>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Bool>_Func4");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/arr"));
    ival = IsXBoolProvider<IV_Bool>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Bool>_Func5");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/null"));
    ival = IsXBoolProvider<IV_Bool>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Bool>_Func6");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/bool"));
    ival = IsXBoolProvider<IV_Bool>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(true);
    EXPECT_TRUE(checkFunction(ival, res));
  }
}

TEST_F(ValueTest, IsXBoolProvider_IV_Object) {
  SCOPED_TRACE("IsXBoolProvider<IV_Object>");
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getPointer("/str"));
  auto ival = IsXBoolProvider<IV_Object>::_FACTORY(std::move(p));
  checkType(ival, IV_Bool);

  // Parameters
  std::vector<IValueType> t = {IV_Any};
  testParams<IsXBoolProvider<IV_Object>>(t);

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Object>_Func1");
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Object>_Func2");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/num"));
    ival = IsXBoolProvider<IV_Object>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Object>_Func3");
    IValueTestHelper::param(p, IValueTestHelper::getPointer(""));
    ival = IsXBoolProvider<IV_Object>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(true);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Object>_Func4");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/arr"));
    ival = IsXBoolProvider<IV_Object>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Object>_Func5");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/null"));
    ival = IsXBoolProvider<IV_Object>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Object>_Func6");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/bool"));
    ival = IsXBoolProvider<IV_Object>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
}

TEST_F(ValueTest, IsXBoolProvider_IV_Array) {
  SCOPED_TRACE("IsXBoolProvider<IV_Array>");
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getPointer("/str"));
  auto ival = IsXBoolProvider<IV_Array>::_FACTORY(std::move(p));
  checkType(ival, IV_Bool);

  // Parameters
  std::vector<IValueType> t = {IV_Any};
  testParams<IsXBoolProvider<IV_Array>>(t);

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Array>_Func1");
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Array>_Func2");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/num"));
    ival = IsXBoolProvider<IV_Array>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Array>_Func3");
    IValueTestHelper::param(p, IValueTestHelper::getPointer(""));
    ival = IsXBoolProvider<IV_Array>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Array>_Func4");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/arr"));
    ival = IsXBoolProvider<IV_Array>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(true);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Array>_Func5");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/null"));
    ival = IsXBoolProvider<IV_Array>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Array>_Func6");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/bool"));
    ival = IsXBoolProvider<IV_Array>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
}

TEST_F(ValueTest, IsXBoolProvider_IV_Null) {
  SCOPED_TRACE("IsXBoolProvider<IV_Null>");
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getPointer("/str"));
  auto ival = IsXBoolProvider<IV_Null>::_FACTORY(std::move(p));
  checkType(ival, IV_Bool);

  // Parameters
  std::vector<IValueType> t = {IV_Any};
  testParams<IsXBoolProvider<IV_Null>>(t);

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Null>_Func1");
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Null>_Func2");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/num"));
    ival = IsXBoolProvider<IV_Null>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Null>_Func3");
    IValueTestHelper::param(p, IValueTestHelper::getPointer(""));
    ival = IsXBoolProvider<IV_Null>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Null>_Func4");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/arr"));
    ival = IsXBoolProvider<IV_Null>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Null>_Func5");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/null"));
    ival = IsXBoolProvider<IV_Null>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(true);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Null>_Func6");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/bool"));
    ival = IsXBoolProvider<IV_Null>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
}

TEST_F(ValueTest, IsXBoolProvider_IV_Any) {
  SCOPED_TRACE("IsXBoolProvider<IV_Any>");
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getPointer("/str"));
  auto ival = IsXBoolProvider<IV_Any>::_FACTORY(std::move(p));
  checkType(ival, IV_Bool);

  // Parameters
  std::vector<IValueType> t = {IV_Any};
  testParams<IsXBoolProvider<IV_Any>>(t);

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Any>_Func1");
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(true);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Any>_Func2");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/num"));
    ival = IsXBoolProvider<IV_Any>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(true);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Any>_Func3");
    IValueTestHelper::param(p, IValueTestHelper::getPointer(""));
    ival = IsXBoolProvider<IV_Any>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(true);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Any>_Func4");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/arr"));
    ival = IsXBoolProvider<IV_Any>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(true);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Any>_Func5");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/null"));
    ival = IsXBoolProvider<IV_Any>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(true);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("IsXBoolProvider<IV_Any>_Func6");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/bool"));
    ival = IsXBoolProvider<IV_Any>::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(true);
    EXPECT_TRUE(checkFunction(ival, res));
  }
}

TEST_F(ValueTest, ListAttributesProvider) {
  SCOPED_TRACE("ListAttributesProvider");
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getPointer(""));
  auto ival = ListAttributesProvider::_FACTORY(std::move(p));
  checkType(ival, IV_Array);

  // Parameters
  std::vector<IValueType> t = {IV_Object};
  testParams<ListAttributesProvider>(t, false);

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  {
    SCOPED_TRACE("ListAttributesProvider_Func1");
    auto val = IValueToValueString(ival);
    EXPECT_STREQ(val.c_str(), R"(["num","str","arr","bool","null","obj"])");
  }
  {
    SCOPED_TRACE("ListAttributesProvider_Func2");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/obj"));
    ival = ListAttributesProvider::_FACTORY(std::move(p));
    auto val = IValueToValueString(ival);
    EXPECT_STREQ(val.c_str(), R"(["nested"])");
  }
  {
    SCOPED_TRACE("ListAttributesProvider_Func3");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/num"));
    ival = ListAttributesProvider::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = std::make_unique<DummyNull>();
    EXPECT_TRUE(checkFunction(ival, res));
  }
}

TEST_F(ValueTest, RegexExtractProvider) {
  SCOPED_TRACE("RegexExtractProvider");
  // Return Type
  Params p = {};
  IValueTestHelper::param(p,
                          IValueTestHelper::getStringVal(
                              "some string with 'the data i want' inside"),
                          IValueTestHelper::getStringVal("'(.*?)'"));
  auto ival = RegexExtractProvider::_FACTORY(std::move(p));
  checkType(ival, IV_Array);

  // Parameters
  std::vector<IValueType> t = {IV_String, IV_String};
  testParams<RegexExtractProvider>(t, true, false);
  {
    IValueTestHelper::param(
        p, IValueTestHelper::getStringVal("some string without my data inside"),
        IValueTestHelper::getStringVal("'(.*?'"));
    EXPECT_THROW(RegexExtractProvider::_FACTORY(std::move(p)),
                 WrongParameterException);
  }

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  {
    SCOPED_TRACE("RegexExtractProvider_Func1");
    auto val = IValueToValueString(ival);
    EXPECT_STREQ(val.c_str(), R"(["the data i want"])");
  }
  {
    SCOPED_TRACE("RegexExtractProvider_Func2");
    IValueTestHelper::param(
        p, IValueTestHelper::getStringVal("some string without my data inside"),
        IValueTestHelper::getStringVal("'(.*?)'"));
    ival = RegexExtractProvider::_FACTORY(std::move(p));
    auto val = IValueToValueString(ival);
    EXPECT_STREQ(val.c_str(), R"([])");
  }
  {
    SCOPED_TRACE("RegexExtractProvider_Func3");
    IValueTestHelper::param(
        p, IValueTestHelper::getStringVal("'more' '' 'data' to be had "),
        IValueTestHelper::getStringVal("'(.*?)'"));
    ival = RegexExtractProvider::_FACTORY(std::move(p));
    auto val = IValueToValueString(ival);
    EXPECT_STREQ(val.c_str(), R"(["more","","data"])");
  }
}

TEST_F(ValueTest, RegexExtractFirstProvider) {
  SCOPED_TRACE("RegexExtractFirstProvider");
  // Return Type
  Params p = {};
  IValueTestHelper::param(p,
                          IValueTestHelper::getStringVal(
                              "some string with 'the data i want' inside"),
                          IValueTestHelper::getStringVal("'(.*?)'"));
  auto ival = RegexExtractFirstProvider::_FACTORY(std::move(p));
  checkType(ival, IV_String);

  // Parameters
  std::vector<IValueType> t = {IV_String, IV_String};
  testParams<RegexExtractFirstProvider>(t, true, false);
  {
    IValueTestHelper::param(
        p, IValueTestHelper::getStringVal("some string without my data inside"),
        IValueTestHelper::getStringVal("'(.*?'"));
    EXPECT_THROW(RegexExtractFirstProvider::_FACTORY(std::move(p)),
                 WrongParameterException);
  }

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  {
    SCOPED_TRACE("RegexExtractFirstProvider_Func1");
    auto val = IValueToValueString(ival);
    EXPECT_STREQ(val.c_str(), R"("the data i want")");
  }
  {
    SCOPED_TRACE("RegexExtractFirstProvider_Func2");
    IValueTestHelper::param(
        p, IValueTestHelper::getStringVal("some string without my data inside"),
        IValueTestHelper::getStringVal("'(.*?)'"));
    ival = RegexExtractFirstProvider::_FACTORY(std::move(p));
    auto val = IValueToValueString(ival);
    EXPECT_STREQ(val.c_str(), R"(null)");
  }
  {
    SCOPED_TRACE("RegexExtractFirstProvider_Func3");
    IValueTestHelper::param(
        p, IValueTestHelper::getStringVal("'more' '' 'data' to be had "),
        IValueTestHelper::getStringVal("'(.*?)'"));
    ival = RegexExtractFirstProvider::_FACTORY(std::move(p));
    auto val = IValueToValueString(ival);
    EXPECT_STREQ(val.c_str(), R"("more")");
  }
}

TEST_F(ValueTest, RegexProvider) {
  SCOPED_TRACE("RegexProvider");
  // Return Type
  Params p = {};
  IValueTestHelper::param(p,
                          IValueTestHelper::getStringVal(
                              "some string with 'the data i want' inside"),
                          IValueTestHelper::getStringVal("'(.*?)'"));
  auto ival = RegexProvider::_FACTORY(std::move(p));
  checkType(ival, IV_Bool);

  // Parameters
  std::vector<IValueType> t = {IV_String, IV_String};
  testParams<RegexProvider>(t, true, false);
  {
    IValueTestHelper::param(
        p, IValueTestHelper::getStringVal("some string without my data inside"),
        IValueTestHelper::getStringVal("'(.*?'"));
    EXPECT_THROW(RegexProvider::_FACTORY(std::move(p)),
                 WrongParameterException);
  }

  // Duplicate
  checkDuplicate(ival);
  // Check Function
  {
    SCOPED_TRACE("RegexProvider_Func1");
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(true);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("RegexProvider_Func2");
    IValueTestHelper::param(
        p, IValueTestHelper::getStringVal("some string without my data inside"),
        IValueTestHelper::getStringVal("'(.*?)'"));
    ival = RegexProvider::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("RegexProvider_Func3");
    IValueTestHelper::param(
        p, IValueTestHelper::getStringVal("'more' '' 'data' to be had "),
        IValueTestHelper::getStringVal("'(.*?)'"));
    ival = RegexProvider::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(true);
    EXPECT_TRUE(checkFunction(ival, res));
  }
}

TEST_F(ValueTest, RegexReplaceProvider) {
  SCOPED_TRACE("RegexReplaceProvider");
  // Return Type
  Params p = {};
  IValueTestHelper::param(p,
                          IValueTestHelper::getStringVal(
                              "some string with 'the data i want' inside"),
                          IValueTestHelper::getStringVal("'(.*?)'"),
                          IValueTestHelper::getStringVal("no data"));
  auto ival = RegexReplaceProvider::_FACTORY(std::move(p));
  checkType(ival, IV_String);

  // Parameters
  std::vector<IValueType> t = {IV_String, IV_String, IV_String};
  testParams<RegexReplaceProvider>(t, true, false);
  {
    IValueTestHelper::param(
        p, IValueTestHelper::getStringVal("some string without my data inside"),
        IValueTestHelper::getStringVal("'(.*?'"));
    EXPECT_THROW(RegexReplaceProvider::_FACTORY(std::move(p)),
                 WrongParameterException);
  }

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  {
    SCOPED_TRACE("RegexReplaceProvider_Func1");
    std::unique_ptr<IValueProvider> res =
        IValueTestHelper::getStringVal("some string with no data inside");
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("RegexReplaceProvider_Func2");
    IValueTestHelper::param(
        p, IValueTestHelper::getStringVal("some string without my data inside"),
        IValueTestHelper::getStringVal("'(.*?)'"),
        IValueTestHelper::getStringVal("no data"));
    ival = RegexReplaceProvider::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res =
        IValueTestHelper::getStringVal("some string without my data inside");
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("RegexReplaceProvider_Func3");
    IValueTestHelper::param(
        p, IValueTestHelper::getStringVal("some string without my data inside"),
        IValueTestHelper::getStringVal(R"((?!\bbeer\b)\b\w+\b)"),
        IValueTestHelper::getStringVal("beer"));
    ival = RegexReplaceProvider::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getStringVal(
        "beer beer beer beer beer beer");  // hmmmm beer
    EXPECT_TRUE(checkFunction(ival, res));
  }
}

TEST_F(ValueTest, SCONTAINSProvider) {
  SCOPED_TRACE("SCONTAINSProvider");
  // Return Type
  Params p = {};
  IValueTestHelper::param(p,
                          IValueTestHelper::getStringVal(
                              "some string with 'the data i want' inside"),
                          IValueTestHelper::getStringVal("'the data i want'"));
  auto ival = SCONTAINSProvider::_FACTORY(std::move(p));
  checkType(ival, IV_Bool);

  // Parameters
  std::vector<IValueType> t = {IV_String, IV_String};
  testParams<SCONTAINSProvider>(t);

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  {
    SCOPED_TRACE("SCONTAINSProvider_Func1");
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(true);
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("SCONTAINSProvider_Func2");
    IValueTestHelper::param(
        p, IValueTestHelper::getStringVal("some string without my data inside"),
        IValueTestHelper::getStringVal("'the data i want'"));
    ival = SCONTAINSProvider::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res = IValueTestHelper::getBoolVal(false);
    EXPECT_TRUE(checkFunction(ival, res));
  }
}

TEST_F(ValueTest, FINDSTRProvider) {
  SCOPED_TRACE("FINDSTRProvider");
  // Return Type
  Params p = {};
  IValueTestHelper::param(p,
                          IValueTestHelper::getStringVal(
                              "some string with 'the data i want' inside"),
                          IValueTestHelper::getStringVal("'the data i want'"));
  auto ival = FINDSTRProvider::_FACTORY(std::move(p));
  checkType(ival, IV_Number);

  // Parameters
  std::vector<IValueType> t = {IV_String, IV_String};
  testParams<FINDSTRProvider>(t);

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  {
    SCOPED_TRACE("FINDSTRProvider_Func1");
    std::unique_ptr<IValueProvider> res =
        IValueTestHelper::getNumVal(static_cast<int64_t>(17));
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("FINDSTRProvider_Func2");
    IValueTestHelper::param(
        p, IValueTestHelper::getStringVal("some string without my data inside"),
        IValueTestHelper::getStringVal("'the data i want'"));
    ival = FINDSTRProvider::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res =
        IValueTestHelper::getNumVal(static_cast<int64_t>(std::string::npos));
    EXPECT_TRUE(checkFunction(ival, res));
  }
}

TEST_F(ValueTest, SPLITProvider) {
  SCOPED_TRACE("SPLITProvider");
  // Return Type
  Params p = {};
  IValueTestHelper::param(p,
                          IValueTestHelper::getStringVal(
                              "some string with 'the data i want' inside"),
                          IValueTestHelper::getStringVal("'the data i want'"));
  auto ival = SPLITProvider::_FACTORY(std::move(p));
  checkType(ival, IV_Array);

  // Parameters
  std::vector<IValueType> t = {IV_String, IV_String};
  testParams<SPLITProvider>(t);

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  {
    SCOPED_TRACE("SPLITProvider_Func1");
    IValueTestHelper::param(
        p, IValueTestHelper::getStringVal("some string"),
        IValueTestHelper::getStringVal(" "));
    ival = SPLITProvider::_FACTORY(std::move(p));
    auto val = IValueToValueString(ival);
    EXPECT_STREQ(val.c_str(), R"(["some","string"])");
  }
  {
    SCOPED_TRACE("SPLITProvider_Func2");
    IValueTestHelper::param(
        p, IValueTestHelper::getStringVal("some string "),
        IValueTestHelper::getStringVal(" "));
    ival = SPLITProvider::_FACTORY(std::move(p));
    auto val = IValueToValueString(ival);
    EXPECT_STREQ(val.c_str(), R"(["some","string",""])");
  }
    {
    SCOPED_TRACE("SPLITProvider_Func3");
    IValueTestHelper::param(
        p, IValueTestHelper::getStringVal("1<=2<=3<=4"),
        IValueTestHelper::getStringVal("<="));
    ival = SPLITProvider::_FACTORY(std::move(p));
    auto val = IValueToValueString(ival);
    EXPECT_STREQ(val.c_str(), R"(["1","2","3","4"])");
  }
}

TEST_F(ValueTest, SubStringProvider) {
  SCOPED_TRACE("SubStringProvider");
  // Return Type
  Params p = {};
  IValueTestHelper::param(
      p,
      IValueTestHelper::getStringVal(
          "some string with 'the data i want' inside"),
      IValueTestHelper::getNumVal(static_cast<u_int64_t>(17)),
      IValueTestHelper::getNumVal(static_cast<u_int64_t>(17)));
  auto ival = SubStringProvider::_FACTORY(std::move(p));
  checkType(ival, IV_String);

  // Parameters
  std::vector<IValueType> t = {IV_String, IV_Number, IV_Number};
  testParams<SubStringProvider>(t, false, false);

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  {
    SCOPED_TRACE("SubStringProvider_Func1");
    auto val = IValueToValueString(ival);
    EXPECT_STREQ(val.c_str(), R"("'the data i want'")");
  }
  {
    SCOPED_TRACE("SubStringProvider_Func2 (out of bounds)");
    IValueTestHelper::param(
        p,
        IValueTestHelper::getStringVal(
            "some string with 'the data i want' inside"),
        IValueTestHelper::getNumVal(static_cast<u_int64_t>(50)));
    ival = SubStringProvider::_FACTORY(std::move(p));
    auto val = IValueToValueString(ival);
    EXPECT_STREQ(val.c_str(), R"("")");
  }
  {
    SCOPED_TRACE("SubStringProvider_Func3 (Single param)");
    IValueTestHelper::param(
        p,
        IValueTestHelper::getStringVal(
            "some string with 'the data i want' inside"),
        IValueTestHelper::getNumVal(static_cast<u_int64_t>(17)));
    ival = SubStringProvider::_FACTORY(std::move(p));
    auto val = IValueToValueString(ival);
    EXPECT_STREQ(val.c_str(), R"("'the data i want' inside")");
  }
}

TEST_F(ValueTest, SeqNumberProvider) {
  SCOPED_TRACE("SeqNumberProvider");
  // Return Type
  Params p = {};
  auto ival = SeqNumberProvider::_FACTORY(std::move(p));
  checkType(ival, IV_Number);

  // Parameters
  std::vector<IValueType> t = {};
  testParams<SeqNumberProvider>(t);

  // Duplicate
  auto dupe = ival->duplicate();
  EXPECT_STREQ(ival->getName().c_str(), dupe->getName().c_str());
  EXPECT_STREQ(ival->toString().c_str(), dupe->toString().c_str());

  // Check Function
  EXPECT_STREQ(IValueToValueString(ival).c_str(), "0");
  EXPECT_STREQ(IValueToValueString(dupe).c_str(), "1");
  EXPECT_STREQ(IValueToValueString(ival).c_str(), "2");
  EXPECT_STREQ(IValueToValueString(dupe).c_str(), "3");
}

TEST_F(ValueTest, TypeProvider) {
  SCOPED_TRACE("TypeProvider");
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getPointer(""));
  auto ival = TypeProvider::_FACTORY(std::move(p));
  checkType(ival, IV_String);

  // Parameters
  std::vector<IValueType> t = {IV_Any};
  testParams<TypeProvider>(t);

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  {
    SCOPED_TRACE("TypeProvider_Func1");
    std::unique_ptr<IValueProvider> res =
        IValueTestHelper::getStringVal("OBJECT");
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("TypeProvider_Func2");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/num"));
    ival = TypeProvider::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res =
        IValueTestHelper::getStringVal("NUMBER");
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("TypeProvider_Func3");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/arr"));
    ival = TypeProvider::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res =
        IValueTestHelper::getStringVal("ARRAY");
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("TypeProvider_Func4");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/bool"));
    ival = TypeProvider::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res =
        IValueTestHelper::getStringVal("BOOL");
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("TypeProvider_Func5");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/str"));
    ival = TypeProvider::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res =
        IValueTestHelper::getStringVal("STRING");
    EXPECT_TRUE(checkFunction(ival, res));
  }
  {
    SCOPED_TRACE("TypeProvider_Func6");
    IValueTestHelper::param(p, IValueTestHelper::getPointer("/null"));
    ival = TypeProvider::_FACTORY(std::move(p));
    std::unique_ptr<IValueProvider> res =
        IValueTestHelper::getStringVal("NULL");
    EXPECT_TRUE(checkFunction(ival, res));
  }
}

TEST_F(ValueTest, Atan2Provider) {
  SCOPED_TRACE("Atan2Provider");
  using TestType = Atan2Provider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getNumVal(2l),
                          IValueTestHelper::getNumVal(2l));
  std::unique_ptr<IValueProvider> ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Number);
  // Parameters
  std::vector<IValueType> t = {IV_Number, IV_Number};
  testParams<TestType>(t, true);
  // Duplicate
  checkDuplicate(ival);
  // Check Function

  testBinaryAtom<TestType, double, double, double>(
      -10, 10, -0.78539816339744830961566084581987572);
  testBinaryAtom<TestType, int64_t, int64_t, double>(
      -10, 10, -0.78539816339744830961566084581987572);
  testBinaryAtom<TestType, u_int64_t, u_int64_t, double>(13, 53,
                                              0.240534247945904616990335055618);
}

TEST_F(ValueTest, AbsProvider) {
  SCOPED_TRACE("AbsProvider");
  using TestType = AbsProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getNumVal(2l));
  std::unique_ptr<IValueProvider> ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Number);
  // Parameters
  std::vector<IValueType> t = {IV_Number};
  testParams<TestType>(t, true);
  // Duplicate
  checkDuplicate(ival);
  // Check Function

  testUnaryAtom<TestType, int64_t, int64_t>(-6, 6);
  testUnaryAtom<TestType, int64_t, int64_t>(2142342, 2142342);
  testUnaryAtom<TestType, u_int64_t, u_int64_t>(2142342, 2142342);
  testUnaryAtom<TestType, double, double>(-3345345.2342384238424,
                                          3345345.2342384238424);
  testUnaryAtom<TestType, double, double>(345, 345);
}

TEST_F(ValueTest, RoundProvider) {
  SCOPED_TRACE("RoundProvider");
  using TestType = RoundProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getNumVal(2l));
  std::unique_ptr<IValueProvider> ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Number);
  // Parameters
  std::vector<IValueType> t = {IV_Number};
  testParams<TestType>(t, true);
  // Duplicate
  checkDuplicate(ival);
  // Check Function

  testUnaryAtom<TestType, int64_t, int64_t>(-6, -6);
  testUnaryAtom<TestType, int64_t, int64_t>(2142342, 2142342);
  testUnaryAtom<TestType, u_int64_t, u_int64_t>(2142342, 2142342);
  testUnaryAtom<TestType, double, double>(-3345345.2342384238424, -3345345);
  testUnaryAtom<TestType, double, double>(345.7567, 346);
}

TEST_F(ValueTest, TruncProvider) {
  SCOPED_TRACE("TruncProvider");
  using TestType = TruncProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getNumVal(2l));
  std::unique_ptr<IValueProvider> ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Number);
  // Parameters
  std::vector<IValueType> t = {IV_Number};
  testParams<TestType>(t, true);
  // Duplicate
  checkDuplicate(ival);
  // Check Function

  testUnaryAtom<TestType, int64_t, int64_t>(-6, -6);
  testUnaryAtom<TestType, int64_t, int64_t>(2142342, 2142342);
  testUnaryAtom<TestType, u_int64_t, u_int64_t>(2142342, 2142342);
  testUnaryAtom<TestType, double, double>(-3345345.2342384238424, -3345345);
  testUnaryAtom<TestType, double, double>(345.7567, 345);
}

TEST_F(ValueTest, CeilProvider) {
  SCOPED_TRACE("CeilProvider");
  using TestType = CeilProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getNumVal(2l));
  std::unique_ptr<IValueProvider> ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Number);
  // Parameters
  std::vector<IValueType> t = {IV_Number};
  testParams<TestType>(t, true);
  // Duplicate
  checkDuplicate(ival);
  // Check Function

  testUnaryAtom<TestType, int64_t, int64_t>(-6, -6);
  testUnaryAtom<TestType, int64_t, int64_t>(2142342, 2142342);
  testUnaryAtom<TestType, u_int64_t, u_int64_t>(2142342, 2142342);
  testUnaryAtom<TestType, double, double>(-3345345.2342384238424, -3345345);
  testUnaryAtom<TestType, double, double>(345.7567, 346);
  testUnaryAtom<TestType, double, double>(345.2567, 346);
}

TEST_F(ValueTest, FloorProvider) {
  SCOPED_TRACE("FloorProvider");
  using TestType = FloorProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getNumVal(2l));
  std::unique_ptr<IValueProvider> ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Number);
  // Parameters
  std::vector<IValueType> t = {IV_Number};
  testParams<TestType>(t, true);
  // Duplicate
  checkDuplicate(ival);
  // Check Function

  testUnaryAtom<TestType, int64_t, int64_t>(-6, -6);
  testUnaryAtom<TestType, int64_t, int64_t>(2142342, 2142342);
  testUnaryAtom<TestType, u_int64_t, u_int64_t>(2142342, 2142342);
  testUnaryAtom<TestType, double, double>(-3345345.2342384238424, -3345346);
  testUnaryAtom<TestType, double, double>(345.7567, 345);
  testUnaryAtom<TestType, double, double>(345.2567, 345);
}

TEST_F(ValueTest, SqrtProvider) {
  SCOPED_TRACE("SqrtProvider");
  using TestType = SqrtProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getNumVal(2l));
  std::unique_ptr<IValueProvider> ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Number);
  // Parameters
  std::vector<IValueType> t = {IV_Number};
  testParams<TestType>(t, true);
  // Duplicate
  checkDuplicate(ival);
  // Check Function

  testUnaryAtom<TestType, int64_t>(-6);
  testUnaryAtom<TestType, int64_t, double>(2142342, 1463.67414406349338913920);
  testUnaryAtom<TestType, u_int64_t, double>(2142342,
                                             1463.67414406349338913920);
  testUnaryAtom<TestType, double, double>(3345345.2342384238424,
                                          1829.0284946491194133);
  testUnaryAtom<TestType, double>(-345.7567);
}

TEST_F(ValueTest, DegreesProvider) {
  SCOPED_TRACE("DegreesProvider");
  using TestType = DegreesProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getNumVal(2l));
  std::unique_ptr<IValueProvider> ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Number);
  // Parameters
  std::vector<IValueType> t = {IV_Number};
  testParams<TestType>(t, true);
  // Duplicate
  checkDuplicate(ival);
  // Check Function

  testUnaryAtom<TestType, int64_t, double>(5, 286.4788975654116);
  testUnaryAtom<TestType, u_int64_t, double>(5, 286.4788975654116);
  testUnaryAtom<TestType, double, double>(0.34, 19.48056503444799);
  testUnaryAtom<TestType, double, double>(-0.34, -19.48056503444799);
}

TEST_F(ValueTest, RadiansProvider) {
  SCOPED_TRACE("RadiansProvider");
  using TestType = RadiansProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getNumVal(2l));
  std::unique_ptr<IValueProvider> ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Number);
  // Parameters
  std::vector<IValueType> t = {IV_Number};
  testParams<TestType>(t, true);
  // Duplicate
  checkDuplicate(ival);
  // Check Function

  testUnaryAtom<TestType, int64_t, double>(6, 0.10471975511965977);
  testUnaryAtom<TestType, u_int64_t, double>(6, 0.10471975511965977);
  testUnaryAtom<TestType, double, double>(19.48056503444799, 0.34);
  testUnaryAtom<TestType, double, double>(-19.48056503444799, -0.34);
}

TEST_F(ValueTest, AcosProvider) {
  SCOPED_TRACE("AcosProvider");
  using TestType = AcosProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getNumVal(2l));
  std::unique_ptr<IValueProvider> ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Number);
  // Parameters
  std::vector<IValueType> t = {IV_Number};
  testParams<TestType>(t, true);
  // Duplicate
  checkDuplicate(ival);
  // Check Function

  testUnaryAtom<TestType, int64_t>(6);
  testUnaryAtom<TestType, int64_t, double>(0, std::acos(0));
  testUnaryAtom<TestType, int64_t, double>(1, std::acos(1));
  testUnaryAtom<TestType, int64_t, double>(-1, std::acos(-1));
  testUnaryAtom<TestType, int64_t>(-6);
  testUnaryAtom<TestType, u_int64_t, double>(0, std::acos(0));
  testUnaryAtom<TestType, u_int64_t, double>(1, std::acos(1));
  testUnaryAtom<TestType, u_int64_t>(6);
  testUnaryAtom<TestType, double>(3842.234238);
  testUnaryAtom<TestType, double>(-3842.234238);
  testUnaryAtom<TestType, double, double>(0.54535, std::acos(0.54535));
}

TEST_F(ValueTest, AsinProvider) {
  SCOPED_TRACE("AsinProvider");
  using TestType = AsinProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getNumVal(2l));
  std::unique_ptr<IValueProvider> ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Number);
  // Parameters
  std::vector<IValueType> t = {IV_Number};
  testParams<TestType>(t, true);
  // Duplicate
  checkDuplicate(ival);
  // Check Function

  testUnaryAtom<TestType, int64_t>(6);
  testUnaryAtom<TestType, int64_t, double>(0, std::asin(0));
  testUnaryAtom<TestType, int64_t, double>(1, std::asin(1));
  testUnaryAtom<TestType, int64_t, double>(-1, std::asin(-1));
  testUnaryAtom<TestType, int64_t>(-6);
  testUnaryAtom<TestType, u_int64_t, double>(0, std::asin(0));
  testUnaryAtom<TestType, u_int64_t, double>(1, std::asin(1));
  testUnaryAtom<TestType, u_int64_t>(6);
  testUnaryAtom<TestType, double>(3842.234238);
  testUnaryAtom<TestType, double>(-3842.234238);
  testUnaryAtom<TestType, double, double>(0.54535, std::asin(0.54535));
}

TEST_F(ValueTest, AtanProvider) {
  SCOPED_TRACE("AtanProvider");
  using TestType = AtanProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getNumVal(2l));
  std::unique_ptr<IValueProvider> ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Number);
  // Parameters
  std::vector<IValueType> t = {IV_Number};
  testParams<TestType>(t, true);
  // Duplicate
  checkDuplicate(ival);
  // Check Function

  testUnaryAtom<TestType, int64_t, double>(6, std::atan(6));
  testUnaryAtom<TestType, int64_t, double>(0, std::atan(0));
  testUnaryAtom<TestType, int64_t, double>(1, std::atan(1));
  testUnaryAtom<TestType, int64_t, double>(-1, std::atan(-1));
  testUnaryAtom<TestType, int64_t, double>(-6, std::atan(-6));
  testUnaryAtom<TestType, u_int64_t, double>(0, std::atan(0));
  testUnaryAtom<TestType, u_int64_t, double>(1, std::atan(1));
  testUnaryAtom<TestType, u_int64_t, double>(6, std::atan(6));
  testUnaryAtom<TestType, double, double>(3842.234238, std::atan(3842.234238));
  testUnaryAtom<TestType, double, double>(-3842.234238,
                                          std::atan(-3842.234238));
  testUnaryAtom<TestType, double, double>(0.54535, std::atan(0.54535));
}

TEST_F(ValueTest, CosProvider) {
  SCOPED_TRACE("CosProvider");
  using TestType = CosProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getNumVal(2l));
  std::unique_ptr<IValueProvider> ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Number);
  // Parameters
  std::vector<IValueType> t = {IV_Number};
  testParams<TestType>(t, true);
  // Duplicate
  checkDuplicate(ival);
  // Check Function

  testUnaryAtom<TestType, int64_t, double>(6, std::cos(6));
  testUnaryAtom<TestType, int64_t, double>(0, std::cos(0));
  testUnaryAtom<TestType, int64_t, double>(1, std::cos(1));
  testUnaryAtom<TestType, int64_t, double>(-1, std::cos(-1));
  testUnaryAtom<TestType, int64_t, double>(-6, std::cos(-6));
  testUnaryAtom<TestType, u_int64_t, double>(0, std::cos(0));
  testUnaryAtom<TestType, u_int64_t, double>(1, std::cos(1));
  testUnaryAtom<TestType, u_int64_t, double>(6, std::cos(6));
  testUnaryAtom<TestType, double, double>(3842.234238, std::cos(3842.234238));
  testUnaryAtom<TestType, double, double>(-3842.234238, std::cos(-3842.234238));
  testUnaryAtom<TestType, double, double>(0.54535, std::cos(0.54535));
}

TEST_F(ValueTest, SinProvider) {
  SCOPED_TRACE("SinProvider");
  using TestType = SinProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getNumVal(2l));
  std::unique_ptr<IValueProvider> ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Number);
  // Parameters
  std::vector<IValueType> t = {IV_Number};
  testParams<TestType>(t, true);
  // Duplicate
  checkDuplicate(ival);
  // Check Function

  testUnaryAtom<TestType, int64_t, double>(6, std::sin(6));
  testUnaryAtom<TestType, int64_t, double>(0, std::sin(0));
  testUnaryAtom<TestType, int64_t, double>(1, std::sin(1));
  testUnaryAtom<TestType, int64_t, double>(-1, std::sin(-1));
  testUnaryAtom<TestType, int64_t, double>(-6, std::sin(-6));
  testUnaryAtom<TestType, u_int64_t, double>(0, std::sin(0));
  testUnaryAtom<TestType, u_int64_t, double>(1, std::sin(1));
  testUnaryAtom<TestType, u_int64_t, double>(6, std::sin(6));
  testUnaryAtom<TestType, double, double>(3842.234238, std::sin(3842.234238));
  testUnaryAtom<TestType, double, double>(-3842.234238, std::sin(-3842.234238));
  testUnaryAtom<TestType, double, double>(0.54535, std::sin(0.54535));
}

TEST_F(ValueTest, TanProvider) {
  SCOPED_TRACE("TanProvider");
  using TestType = TanProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getNumVal(2l));
  std::unique_ptr<IValueProvider> ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Number);
  // Parameters
  std::vector<IValueType> t = {IV_Number};
  testParams<TestType>(t, true);
  // Duplicate
  checkDuplicate(ival);
  // Check Function

  testUnaryAtom<TestType, int64_t, double>(6, std::tan(6));
  testUnaryAtom<TestType, int64_t, double>(0, std::tan(0));
  testUnaryAtom<TestType, int64_t, double>(1, std::tan(1));
  testUnaryAtom<TestType, int64_t, double>(-1, std::tan(-1));
  testUnaryAtom<TestType, int64_t, double>(-6, std::tan(-6));
  testUnaryAtom<TestType, u_int64_t, double>(0, std::tan(0));
  testUnaryAtom<TestType, u_int64_t, double>(1, std::tan(1));
  testUnaryAtom<TestType, u_int64_t, double>(6, std::tan(6));
  testUnaryAtom<TestType, double, double>(3842.234238, std::tan(3842.234238));
  testUnaryAtom<TestType, double, double>(-3842.234238, std::tan(-3842.234238));
  testUnaryAtom<TestType, double, double>(0.54535, std::tan(0.54535));
}

TEST_F(ValueTest, LenProvider) {
  SCOPED_TRACE("LenProvider");
  using TestType = LenProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getStringVal("tst"));
  std::unique_ptr<IValueProvider> ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Number);
  // Parameters
  std::vector<IValueType> t = {IV_String};
  testParams<TestType>(t, true);
  // Duplicate
  checkDuplicate(ival);
  // Check Function

  testUnaryAtom<TestType, std::string, u_int64_t>("", 0);
  testUnaryAtom<TestType, std::string, u_int64_t>("a", 1);
  testUnaryAtom<TestType, std::string, u_int64_t>("aa", 2);
  testUnaryAtom<TestType, std::string, u_int64_t>("aaa", 3);
}

TEST_F(ValueTest, ConcatProvider) {
  SCOPED_TRACE("ConcatProvider");
  using TestType = ConcatProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getStringVal("tst"),
                          IValueTestHelper::getStringVal("tst"));
  std::unique_ptr<IValueProvider> ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_String);
  // Parameters
  std::vector<IValueType> t = {IV_String, IV_String};
  testParams<TestType>(t, true);
  // Duplicate
  checkDuplicate(ival);
  // Check Function

  testBinaryAtom<TestType, std::string, std::string, std::string>("", "", "");
  testBinaryAtom<TestType, std::string, std::string, std::string>("", "a", "a");
  testBinaryAtom<TestType, std::string, std::string, std::string>("b", "", "b");
  testBinaryAtom<TestType, std::string, std::string, std::string>("a", "b", "ab");
}

TEST_F(ValueTest, UpperProvider) {
  SCOPED_TRACE("UpperProvider");
  using TestType = UpperProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getStringVal("tst"));
  std::unique_ptr<IValueProvider> ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_String);
  // Parameters
  std::vector<IValueType> t = {IV_String};
  testParams<TestType>(t, true);
  // Duplicate
  checkDuplicate(ival);
  // Check Function

  testUnaryAtom<TestType, std::string, std::string>("", "");
  testUnaryAtom<TestType, std::string, std::string>("a", "A");
  testUnaryAtom<TestType, std::string, std::string>("A", "A");
  testUnaryAtom<TestType, std::string, std::string>(";.-+#op", ";.-+#OP");
}

TEST_F(ValueTest, LowerProvider) {
  SCOPED_TRACE("LowerProvider");
  using TestType = LowerProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getStringVal("tst"));
  std::unique_ptr<IValueProvider> ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_String);
  // Parameters
  std::vector<IValueType> t = {IV_String};
  testParams<TestType>(t, true);
  // Duplicate
  checkDuplicate(ival);
  // Check Function

  testUnaryAtom<TestType, std::string, std::string>("", "");
  testUnaryAtom<TestType, std::string, std::string>("A", "a");
  testUnaryAtom<TestType, std::string, std::string>("a", "a");
  testUnaryAtom<TestType, std::string, std::string>(";.-+#OP", ";.-+#op");
}

TEST_F(ValueTest, LtrimProvider) {
  SCOPED_TRACE("LtrimProvider");
  using TestType = LtrimProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getStringVal("tst"));
  std::unique_ptr<IValueProvider> ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_String);
  // Parameters
  std::vector<IValueType> t = {IV_String};
  testParams<TestType>(t, true);
  // Duplicate
  checkDuplicate(ival);
  // Check Function

  testUnaryAtom<TestType, std::string, std::string>("", "");
  testUnaryAtom<TestType, std::string, std::string>("    ", "");
  testUnaryAtom<TestType, std::string, std::string>("aaaa", "aaaa");
  testUnaryAtom<TestType, std::string, std::string>("     aaaa       ",
                                                    "aaaa       ");
}

TEST_F(ValueTest, RtrimProvider) {
  SCOPED_TRACE("RtrimProvider");
  using TestType = RtrimProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getStringVal("tst"));
  std::unique_ptr<IValueProvider> ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_String);
  // Parameters
  std::vector<IValueType> t = {IV_String};
  testParams<TestType>(t, true);
  // Duplicate
  checkDuplicate(ival);
  // Check Function

  testUnaryAtom<TestType, std::string, std::string>("", "");
  testUnaryAtom<TestType, std::string, std::string>("    ", "");
  testUnaryAtom<TestType, std::string, std::string>("aaaa", "aaaa");
  testUnaryAtom<TestType, std::string, std::string>("     aaaa       ",
                                                    "     aaaa");
}

TEST_F(ValueTest, IntCastProvider) {
  SCOPED_TRACE("IntCastProvider");
  using TestType = IntCastProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getNumVal(2l));
  std::unique_ptr<IValueProvider> ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Number);


  // Duplicate
  checkDuplicate(ival);
  // Check Function

  testUnaryAtom<TestType, int64_t, int64_t>(6, 6);
  testUnaryAtom<TestType, int64_t, int64_t>(-6, -6);

  testUnaryAtom<TestType, double, int64_t>(3842.234238, 3842);
  testUnaryAtom<TestType, double, int64_t>(-3842.234238, -3842);

  testUnaryAtom<TestType, bool, int64_t>(true, 1);
  testUnaryAtom<TestType, bool, int64_t>(false, 0);

  testUnaryAtom<TestType, std::string, int64_t>("5", 5);
  testUnaryAtom<TestType, std::string, int64_t>("-5", -5);
  testUnaryAtom<TestType, std::string, int64_t>("   5    ", 5);
  testUnaryAtom<TestType, std::string, int64_t>("   -5    ", -5);
  testUnaryAtom<TestType, std::string, int64_t>("0.5",0);
  testUnaryAtom<TestType, std::string, int64_t>("-1.6",-1);
  testUnaryAtom<TestType, std::string, int64_t>("-1.4",-1);
  testUnaryAtom<TestType, std::string, int64_t>("5 with words", 5);


  testUnaryAtom<TestType, std::string>("aas");
}

TEST_F(ValueTest, FloatCastProvider) {
  SCOPED_TRACE("FloatCastProvider");
  using TestType = FloatCastProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getNumVal(2l));
  std::unique_ptr<IValueProvider> ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Number);


  // Duplicate
  checkDuplicate(ival);
  // Check Function

  testUnaryAtom<TestType, int64_t, double>(6, 6.0);
  testUnaryAtom<TestType, int64_t, double>(-6, -6.0);

  testUnaryAtom<TestType, double, double>(3842.234238, 3842.234238);
  testUnaryAtom<TestType, double, double>(-3842.234238, -3842.234238);

  testUnaryAtom<TestType, bool, int64_t>(true, 1.0);
  testUnaryAtom<TestType, bool, int64_t>(false, 0.0);

  testUnaryAtom<TestType, std::string, double>("5", 5.0);
  testUnaryAtom<TestType, std::string, double>("-5", -5.0);
  testUnaryAtom<TestType, std::string, double>("   5    ", 5.0);
  testUnaryAtom<TestType, std::string, double>("   -5    ", -5.0);
  testUnaryAtom<TestType, std::string, double>("0.5",0.5);
  testUnaryAtom<TestType, std::string, double>("-1.6",-1.6);
  testUnaryAtom<TestType, std::string, double>("-1.4",-1.4);
  testUnaryAtom<TestType, std::string, double>("5 with words", 5.0);


  testUnaryAtom<TestType, std::string>("aas");
}

TEST_F(ValueTest, StringCastProvider) {
  SCOPED_TRACE("StringCastProvider");
  using TestType = StringCastProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getNumVal(2l));
  std::unique_ptr<IValueProvider> ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_String);


  // Duplicate
  checkDuplicate(ival);
  // Check Function

  testUnaryAtom<TestType, int64_t,  std::string>(6, "6");
  testUnaryAtom<TestType, int64_t,  std::string>(-6, "-6");

  testUnaryAtom<TestType, double,  std::string>(3842.234238, "3842.234238");
  testUnaryAtom<TestType, double,  std::string>(-3842.234238, "-3842.234238");

  testUnaryAtom<TestType, bool, std::string>(true, "true");
  testUnaryAtom<TestType, bool, std::string>(false, "false");

  testUnaryAtom<TestType, std::string, std::string>("aas","aas");
}

TEST_F(ValueTest, AndProvider) {
  SCOPED_TRACE("AndProvider");
  using TestType = AndProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getBoolVal(true),
                          IValueTestHelper::getBoolVal(false));
  auto ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Bool);

  // Parameters
  std::vector<IValueType> t = {IV_Bool, IV_Bool};
  testParams<TestType>(t, false);

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  testBinaryAtom<TestType, bool, bool, bool>(false, false, false);
  testBinaryAtom<TestType, bool, bool, bool>(true, false, false);
  testBinaryAtom<TestType, bool, bool, bool>(false, true, false);
  testBinaryAtom<TestType, bool, bool, bool>(true, true, true);
}

TEST_F(ValueTest, OrProvider) {
  SCOPED_TRACE("OrProvider");
  using TestType = OrProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getBoolVal(true),
                          IValueTestHelper::getBoolVal(false));
  auto ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Bool);

  // Parameters
  std::vector<IValueType> t = {IV_Bool, IV_Bool};
  testParams<TestType>(t, false);

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  testBinaryAtom<TestType, bool, bool, bool>(false, false, false);
  testBinaryAtom<TestType, bool, bool, bool>(true, false, true);
  testBinaryAtom<TestType, bool, bool, bool>(false, true, true);
  testBinaryAtom<TestType, bool, bool, bool>(true, true, true);
}

TEST_F(ValueTest, XorProvider) {
  SCOPED_TRACE("XorProvider");
  using TestType = XorProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getBoolVal(true),
                          IValueTestHelper::getBoolVal(false));
  auto ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Bool);

  // Parameters
  std::vector<IValueType> t = {IV_Bool, IV_Bool};
  testParams<TestType>(t, false);

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  testBinaryAtom<TestType, bool, bool, bool>(false, false, false);
  testBinaryAtom<TestType, bool, bool, bool>(true, false, true);
  testBinaryAtom<TestType, bool, bool, bool>(false, true, true);
  testBinaryAtom<TestType, bool, bool, bool>(true, true, false);
}

TEST_F(ValueTest, ImplicationProvider) {
  SCOPED_TRACE("ImplicationProvider");
  using TestType = ImplicationProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getBoolVal(true),
                          IValueTestHelper::getBoolVal(false));
  auto ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Bool);

  // Parameters
  std::vector<IValueType> t = {IV_Bool, IV_Bool};
  testParams<TestType>(t, false);

  // Duplicate
  checkDuplicate(ival);

  // Check Function
  testBinaryAtom<TestType, bool, bool, bool>(false, false, true);
  testBinaryAtom<TestType, bool, bool, bool>(true, false, false);
  testBinaryAtom<TestType, bool, bool, bool>(false, true, true);
  testBinaryAtom<TestType, bool, bool, bool>(true, true, true);
}

TEST_F(ValueTest, EqualProvider) {
  SCOPED_TRACE("EqualProvider");
  using TestType = EqualProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getBoolVal(true),
                          IValueTestHelper::getBoolVal(false));
  auto ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Bool);

  // Parameters
  std::vector<IValueType> t = {IV_Any, IV_Any};
  testParams<TestType>(t, true);

  // Duplicate
  checkDuplicate(ival);

  // Check Function

  // -- Bool
  testBinaryAtom<TestType, bool, bool, bool>(false, false, true);
  testBinaryAtom<TestType, bool, bool, bool>(true, false, false);
  testBinaryAtom<TestType, bool, bool, bool>(false, true, false);
  testBinaryAtom<TestType, bool, bool, bool>(true, true, true);

  // -- String
  testBinaryAtom<TestType, std::string, std::string, bool>("A", "B", false);
  testBinaryAtom<TestType, std::string, std::string, bool>("A", "A", true);
  testBinaryAtom<TestType, std::string, std::string, bool>("A", "a", false);
  testBinaryAtom<TestType, std::string, std::string, bool>("  A  ", "A", false);

  // -- Int
  testBinaryAtom<TestType, int64_t, int64_t, bool>(1,1, true);
  testBinaryAtom<TestType, int64_t, int64_t, bool>(1,2, false);
  testBinaryAtom<TestType, int64_t, int64_t, bool>(1,-1, false);

  // -- Double
  testBinaryAtom<TestType, double, double, bool>(1,1, true);
  testBinaryAtom<TestType, double, double, bool>(1,2, false);
  testBinaryAtom<TestType, double, double, bool>(1,-1, false);

  // -- Mixed Num
  testBinaryAtom<TestType, double, int64_t, bool>(1,1, true);
  testBinaryAtom<TestType, u_int64_t, double, bool>(1,2, false);
  testBinaryAtom<TestType, double, double, bool>(1,-1, false);
  testBinaryAtom<TestType, u_int64_t, int64_t, bool>(1,1, false);

  // -- Mixed Type
  testBinaryAtom<TestType, double, std::string, bool>(1,"1", false);
  testBinaryAtom<TestType, u_int64_t, bool, bool>(1,true, false);

  // Objects/Arrays
  // TODO
}

TEST_F(ValueTest, UnequalProvider) {
  SCOPED_TRACE("UnequalProvider");
  using TestType = UnequalProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getBoolVal(true),
                          IValueTestHelper::getBoolVal(false));
  auto ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Bool);

  // Parameters
  std::vector<IValueType> t = {IV_Any, IV_Any};
  testParams<TestType>(t, true);

  // Duplicate
  checkDuplicate(ival);

  // Check Function

  // -- Bool
  testBinaryAtom<TestType, bool, bool, bool>(false, false, !true);
  testBinaryAtom<TestType, bool, bool, bool>(true, false, !false);
  testBinaryAtom<TestType, bool, bool, bool>(false, true, !false);
  testBinaryAtom<TestType, bool, bool, bool>(true, true, !true);

  // -- String
  testBinaryAtom<TestType, std::string, std::string, bool>("A", "B", !false);
  testBinaryAtom<TestType, std::string, std::string, bool>("A", "A", !true);
  testBinaryAtom<TestType, std::string, std::string, bool>("A", "a", !false);
  testBinaryAtom<TestType, std::string, std::string, bool>("  A  ", "A", !false);

  // -- Int
  testBinaryAtom<TestType, int64_t, int64_t, bool>(1,1, !true);
  testBinaryAtom<TestType, int64_t, int64_t, bool>(1,2, !false);
  testBinaryAtom<TestType, int64_t, int64_t, bool>(1,-1, !false);

  // -- Double
  testBinaryAtom<TestType, double, double, bool>(1,1, !true);
  testBinaryAtom<TestType, double, double, bool>(1,2, !false);
  testBinaryAtom<TestType, double, double, bool>(1,-1, !false);

  // -- Mixed Num
  testBinaryAtom<TestType, double, int64_t, bool>(1,1, !true);
  testBinaryAtom<TestType, u_int64_t, double, bool>(1,2, !false);
  testBinaryAtom<TestType, double, double, bool>(1,-1, !false);
  testBinaryAtom<TestType, u_int64_t, int64_t, bool>(1,1, !false);

  // -- Mixed Type
  testBinaryAtom<TestType, double, std::string, bool>(1,"1", !false);
  testBinaryAtom<TestType, u_int64_t, bool, bool>(1,true, !false);

  // Objects/Arrays
  // TODO
}

TEST_F(ValueTest, LessProvider) {
  SCOPED_TRACE("LessProvider");
  using TestType = LessProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getNumVal(1ul),
                          IValueTestHelper::getNumVal(1ul));
  auto ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Bool);


  // Duplicate
  checkDuplicate(ival);

  // Check Function

  // -- String
  testBinaryAtom<TestType, std::string, std::string, bool>("A", "B", true);
  testBinaryAtom<TestType, std::string, std::string, bool>("A", "A", false);
  testBinaryAtom<TestType, std::string, std::string, bool>("A", "a", true);

  // -- Int
  testBinaryAtom<TestType, int64_t, int64_t, bool>(1,1, false);
  testBinaryAtom<TestType, int64_t, int64_t, bool>(1,2, true);
  testBinaryAtom<TestType, int64_t, int64_t, bool>(1,-1, false);

  // -- Double
  testBinaryAtom<TestType, double, double, bool>(1,1, false);
  testBinaryAtom<TestType, double, double, bool>(1,2, true);
  testBinaryAtom<TestType, double, double, bool>(1,-1, false);

  // -- Mixed Num
  testBinaryAtom<TestType, double, int64_t, bool>(1,1, false);
  testBinaryAtom<TestType, u_int64_t, double, bool>(1,2, true);
  testBinaryAtom<TestType, double, double, bool>(1,-1, false);
  testBinaryAtom<TestType, u_int64_t, int64_t, bool>(1,1, false);
}

TEST_F(ValueTest, LessEqualProvider) {
  SCOPED_TRACE("LessEqualProvider");
  using TestType = LessEqualProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getNumVal(1ul),
                          IValueTestHelper::getNumVal(1ul));
  auto ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Bool);


  // Duplicate
  checkDuplicate(ival);

  // Check Function

  // -- String
  testBinaryAtom<TestType, std::string, std::string, bool>("A", "B", true);
  testBinaryAtom<TestType, std::string, std::string, bool>("A", "A", true);
  testBinaryAtom<TestType, std::string, std::string, bool>("A", "a", true);

  // -- Int
  testBinaryAtom<TestType, int64_t, int64_t, bool>(1,1, true);
  testBinaryAtom<TestType, int64_t, int64_t, bool>(1,2, true);
  testBinaryAtom<TestType, int64_t, int64_t, bool>(1,-1, false);

  // -- Double
  testBinaryAtom<TestType, double, double, bool>(1,1, true);
  testBinaryAtom<TestType, double, double, bool>(1,2, true);
  testBinaryAtom<TestType, double, double, bool>(1,-1, false);

  // -- Mixed Num
  testBinaryAtom<TestType, double, int64_t, bool>(1,1, true);
  testBinaryAtom<TestType, u_int64_t, double, bool>(1,2, true);
  testBinaryAtom<TestType, double, double, bool>(1,-1, false);
  testBinaryAtom<TestType, u_int64_t, int64_t, bool>(1,1, true);
}

TEST_F(ValueTest, GreaterEqualProvider) {
  SCOPED_TRACE("GreaterEqualProvider");
  using TestType = GreaterEqualProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getNumVal(1ul),
                          IValueTestHelper::getNumVal(1ul));
  auto ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Bool);


  // Duplicate
  checkDuplicate(ival);

  // Check Function

  // -- String
  testBinaryAtom<TestType, std::string, std::string, bool>("A", "B", !true);
  testBinaryAtom<TestType, std::string, std::string, bool>("A", "A", !false);
  testBinaryAtom<TestType, std::string, std::string, bool>("A", "a", !true);

  // -- Int
  testBinaryAtom<TestType, int64_t, int64_t, bool>(1,1, !false);
  testBinaryAtom<TestType, int64_t, int64_t, bool>(1,2, !true);
  testBinaryAtom<TestType, int64_t, int64_t, bool>(1,-1, !false);

  // -- Double
  testBinaryAtom<TestType, double, double, bool>(1,1, !false);
  testBinaryAtom<TestType, double, double, bool>(1,2, !true);
  testBinaryAtom<TestType, double, double, bool>(1,-1, !false);

  // -- Mixed Num
  testBinaryAtom<TestType, double, int64_t, bool>(1,1, !false);
  testBinaryAtom<TestType, u_int64_t, double, bool>(1,2, !true);
  testBinaryAtom<TestType, double, double, bool>(1,-1, !false);
  testBinaryAtom<TestType, u_int64_t, int64_t, bool>(1,1, !false);
}

TEST_F(ValueTest, GreaterProvider) {
  SCOPED_TRACE("GreaterProvider");
  using TestType = GreaterProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getNumVal(1ul),
                          IValueTestHelper::getNumVal(1ul));
  auto ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Bool);


  // Duplicate
  checkDuplicate(ival);

  // Check Function

  // -- String
  testBinaryAtom<TestType, std::string, std::string, bool>("A", "B", !true);
  testBinaryAtom<TestType, std::string, std::string, bool>("A", "A", !false);
  testBinaryAtom<TestType, std::string, std::string, bool>("A", "a", !true);

  // -- Int
  testBinaryAtom<TestType, int64_t, int64_t, bool>(1,1, !false);
  testBinaryAtom<TestType, int64_t, int64_t, bool>(1,2, !true);
  testBinaryAtom<TestType, int64_t, int64_t, bool>(2,1, true);
  testBinaryAtom<TestType, int64_t, int64_t, bool>(1,-1, !false);

  // -- Double
  testBinaryAtom<TestType, double, double, bool>(1,1, !false);
  testBinaryAtom<TestType, double, double, bool>(1,2, !true);
  testBinaryAtom<TestType, double, double, bool>(2,1, true);
  testBinaryAtom<TestType, double, double, bool>(1,-1, !false);

  // -- Mixed Num
  testBinaryAtom<TestType, double, int64_t, bool>(1,1, !false);
  testBinaryAtom<TestType, u_int64_t, double, bool>(1,2, !true);
  testBinaryAtom<TestType, double, int64_t, bool>(1,-1, !false);
  testBinaryAtom<TestType, u_int64_t, double, bool>(2,1, true);;
  testBinaryAtom<TestType, u_int64_t, int64_t, bool>(1,1, !false);
}


TEST_F(ValueTest, NotProvider) {
  SCOPED_TRACE("NotProvider");
  using TestType = NotProvider;
  // Return Type
  Params p = {};
  IValueTestHelper::param(p, IValueTestHelper::getBoolVal(true));
  std::unique_ptr<IValueProvider> ival = TestType::_FACTORY(std::move(p));
  checkType(ival, IV_Bool);
  // Parameters
  std::vector<IValueType> t = {IV_Bool};
  testParams<TestType>(t, true);
  // Duplicate
  checkDuplicate(ival);
  
  // Check Function
  testUnaryAtom<TestType, bool, bool>(false, true);
  testUnaryAtom<TestType, bool, bool>(true, false);
}