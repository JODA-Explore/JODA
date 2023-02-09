#include <gtest/gtest.h>
#include "../src/adaptive/UnionTreeNode.h"

class MemoryManagerTest : public::testing::Test {
protected:
    MemoryManagerTest()
        :   Test(),
            memoryManager(100 * 4, removeElement),
            dataContext(container) {
        dataContext.setMemoryManager(&memoryManager);
    }

    JSONContainer container;
    MemoryManager memoryManager;
    DataContext dataContext;

    std::string path1 = "/";
    std::string path2 = "/";
    std::vector<size_t> emptyDocs;

    static void removeElement(std::string x, ManagedDataType dt) {
        std::cout << "remove called for: " << x << std::endl;
    }
};

TEST_F(MemoryManagerTest, AddFirstObject_UnionNode) {
    UnionTreeNode node("/");
    node.nodeUsed(dataContext);

    EXPECT_TRUE(memoryManager.getRecent() == dynamic_cast<ManagedIndexData*>(&node));
}

TEST_F(MemoryManagerTest, AddFirstObject_AdaptiveTrie) {
    AdaptiveTrie adaptiveTrie(path1);
    adaptiveTrie.initialize(dataContext, emptyDocs, true);

    EXPECT_TRUE(memoryManager.getRecent() == dynamic_cast<ManagedIndexData*>(&adaptiveTrie));
}

TEST_F(MemoryManagerTest, AddFirstObject_NumberIndex) {
    NumberIndex numberIndex(emptyDocs, INT64_MIN, INT64_MAX, path1);
    numberIndex.used(dataContext);

    EXPECT_TRUE(memoryManager.getRecent() == dynamic_cast<ManagedIndexData*>(&numberIndex));
}

TEST_F(MemoryManagerTest, Multiple_UnionNode) {
    UnionTreeNode node1("/");
    node1.nodeUsed(dataContext);

    UnionTreeNode node2("/user");
    node2.nodeUsed(dataContext);

    UnionTreeNode node3("/follower");
    node3.nodeUsed(dataContext);

    node2.nodeUsed(dataContext);
    node1.nodeUsed(dataContext);
    node2.nodeUsed(dataContext);
    node1.nodeUsed(dataContext);

    EXPECT_TRUE(memoryManager.getRecent() == dynamic_cast<ManagedIndexData*>(&node1));
    EXPECT_TRUE(memoryManager.getOldest() == dynamic_cast<ManagedIndexData*>(&node3));
}

TEST_F(MemoryManagerTest, Multiple_Mixed) {
    UnionTreeNode node1("/");
    node1.nodeUsed(dataContext);

    AdaptiveTrie adaptiveTrie(path1);
    adaptiveTrie.initialize(dataContext, emptyDocs, true);

    AdaptiveTrie adaptiveTrie2(path1);
    adaptiveTrie2.initialize(dataContext, emptyDocs, true);

    NumberIndex numberIndex(emptyDocs, INT64_MIN, INT64_MAX, path1);
    numberIndex.used(dataContext);

    UnionTreeNode node2("/user");
    node2.nodeUsed(dataContext);

    UnionTreeNode node3("/follower");
    node3.nodeUsed(dataContext);

    NumberIndex numberIndex2(emptyDocs, INT64_MIN, INT64_MAX, path1);
    numberIndex2.used(dataContext);

    NumberIndex numberIndex3(emptyDocs, INT64_MIN, INT64_MAX, path1);
    numberIndex3.used(dataContext);

    EXPECT_TRUE(memoryManager.getRecent() == dynamic_cast<ManagedIndexData*>(&numberIndex3));
    EXPECT_TRUE(memoryManager.getOldest() == dynamic_cast<ManagedIndexData*>(&node1));
}

TEST_F(MemoryManagerTest, Multiple_Free) {

    JSONContainer cnt;
    DataContext dc(cnt);
    MemoryManager memoryManager(123, removeElement);
    dc.setMemoryManager(&memoryManager);

    UnionTreeNode node1("/");
    node1.nodeUsed(dc);

    AdaptiveTrie adaptiveTrie(path1);
    adaptiveTrie.initialize(dc, emptyDocs, true);

    AdaptiveTrie adaptiveTrie2(path1);
    adaptiveTrie2.initialize(dc, emptyDocs, true);

    NumberIndex numberIndex(emptyDocs, INT64_MIN, INT64_MAX, path1);
    numberIndex.used(dc);

    UnionTreeNode node2("/user");
    node2.nodeUsed(dc);

    UnionTreeNode node3("/follower");
    node3.nodeUsed(dc);

    NumberIndex numberIndex2(emptyDocs, INT64_MIN, INT64_MAX, path2);
    numberIndex2.used(dc);

    memoryManager.ensureMemory(LLONG_MAX, &node3);

    EXPECT_TRUE(memoryManager.getRecent() == dynamic_cast<ManagedIndexData*>(&numberIndex2));
    EXPECT_TRUE(memoryManager.getOldest() == dynamic_cast<ManagedIndexData*>(&node1));
}

TEST_F(MemoryManagerTest, Remove_Oldest) {
    UnionTreeNode node1(path1);
    node1.nodeUsed(dataContext);

    AdaptiveTrie adaptiveTrie(path1);
    adaptiveTrie.initialize(dataContext, emptyDocs, true);
    node1.nodeUsed(dataContext);


    NumberIndex numberIndex(emptyDocs, INT64_MIN, INT64_MAX, path1);
    numberIndex.used(dataContext);
    node1.nodeUsed(dataContext);

    UnionTreeNode node2(path2);
    node2.nodeUsed(dataContext);

    memoryManager.ensureMemory(1, &adaptiveTrie);

    EXPECT_TRUE(memoryManager.getOldest() == dynamic_cast<ManagedIndexData*>(&adaptiveTrie));
    EXPECT_TRUE(memoryManager.getRecent() == dynamic_cast<ManagedIndexData*>(&node2));
}