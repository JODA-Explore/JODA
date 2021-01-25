//
// Created by Nico on 12.06.2020.
//
#include <gtest/gtest.h>
#include <memory>
#include <joda/storage/JSONStorage.h>
#include <joda/storage/collection/StorageCollection.h>

class CollectionTest : public ::testing::Test {

 protected:
  StorageCollection collection;

  virtual void SetUp() {
    auto stores = getStorages();

    collection.addDependency(stores[1],stores[0]); //b -> a
    collection.addDependency(stores[2],stores[0]); //c -> a
    collection.addDependency(stores[3],stores[2]); //d -> c
  }

  std::vector<std::shared_ptr<JSONStorage>> getStorages(){
    std::vector<std::shared_ptr<JSONStorage>> ret;
    ret.emplace_back(collection.getOrAddStorage("A"));
    ret.emplace_back(collection.getOrAddStorage("B"));
    ret.emplace_back(collection.getOrAddStorage("C"));
    ret.emplace_back(collection.getOrAddStorage("D"));
    ret.emplace_back(collection.getOrAddStorage("E"));
    return ret;
  }

  std::vector<std::pair<JSONStorage *, size_t>> getCandidates() {
    std::vector<std::pair<JSONStorage *, size_t>> ret;
    auto stores = getStorages();

    ret.emplace_back(stores[0].get(),100);
    ret.emplace_back(stores[1].get(),50);
    ret.emplace_back(stores[2].get(),200);
    ret.emplace_back(stores[3].get(),60);
    ret.emplace_back(stores[4].get(),500);
    return ret;
  }

};

TEST_F(CollectionTest, FIFO) {
  auto candidates = getCandidates();
  auto stores = getStorages();
  collection.orderContainerByFIFO(candidates);
  ASSERT_EQ(stores.size(),candidates.size());
  ASSERT_TRUE(candidates[0].first == stores[0].get());
  ASSERT_TRUE(candidates[1].first == stores[1].get());
  ASSERT_TRUE(candidates[2].first == stores[2].get());
  ASSERT_TRUE(candidates[3].first == stores[3].get());
  ASSERT_TRUE(candidates[4].first == stores[4].get());
}

TEST_F(CollectionTest, LARGEST) {
  auto candidates = getCandidates();
  auto stores = getStorages();
  collection.orderContainerBySize(candidates);
  ASSERT_EQ(stores.size(),candidates.size());
  ASSERT_TRUE(candidates[0].first == stores[4].get());
  ASSERT_TRUE(candidates[1].first == stores[2].get());
  ASSERT_TRUE(candidates[2].first == stores[0].get());
  ASSERT_TRUE(candidates[3].first == stores[3].get());
  ASSERT_TRUE(candidates[4].first == stores[1].get());
}

TEST_F(CollectionTest, DEPENDENCIES) {
  auto candidates = getCandidates();
  auto stores = getStorages();
  collection.orderContainerByDependencies(candidates);
  ASSERT_EQ(stores.size(),candidates.size());
  auto name = candidates[0].first->getName();
  ASSERT_TRUE(name == "D" || name == "E" || name == "B"); //1. D,E,B
  name = candidates[1].first->getName();
  ASSERT_TRUE(name == "D" || name == "E" || name == "B"); //2. D,E,B
  name = candidates[2].first->getName();
  ASSERT_TRUE(name == "D" || name == "E" || name == "B"); //3. D,E,B
  name = candidates[3].first->getName();
  ASSERT_TRUE(name == "C" );
  name = candidates[4].first->getName();
  ASSERT_TRUE(name == "A" );
}