#include "TestFramework.hpp"
#include "dsa/HashTable.hpp"
#include "dsa/LinkedList.hpp"
#include "dsa/BST.hpp"
#include "dsa/Queue.hpp"
#include "dsa/Stack.hpp"
#include "dsa/Sorting.hpp"
#include <string>

using namespace medifind::dsa;

TEST(DsaSuite, HashTableBasicAndRehash) {
    HashTable<std::string, int> table(5); // small initial capacity to force rehash
    ASSERT_TRUE(table.empty());
    ASSERT_EQ(table.size(), 0);

    table.insert("Paracetamol", 100);
    table.insert("Amoxicillin", 200);
    table.insert("Ibuprofen", 300);
    table.insert("Cetirizine", 400);
    table.insert("Metformin", 500);
    table.insert("Atorvastatin", 600);

    ASSERT_EQ(table.size(), 6);
    ASSERT_TRUE(table.contains("Paracetamol"));
    ASSERT_TRUE(table.contains("Ibuprofen"));
    ASSERT_FALSE(table.contains("NonExistent"));

    ASSERT_EQ(*table.find("Paracetamol"), 100);
    ASSERT_EQ(*table.find("Atorvastatin"), 600);

    // Update existing
    table.insert("Paracetamol", 150);
    ASSERT_EQ(table.size(), 6);
    ASSERT_EQ(*table.find("Paracetamol"), 150);

    // Remove
    ASSERT_TRUE(table.remove("Amoxicillin"));
    ASSERT_FALSE(table.contains("Amoxicillin"));
    ASSERT_EQ(table.size(), 5);
}

TEST(DsaSuite, LinkedListOperations) {
    LinkedList<std::string> list;
    ASSERT_TRUE(list.empty());

    list.push_back("Alpha");
    list.push_back("Beta");
    list.push_front("Zero");

    ASSERT_EQ(list.size(), 3);
    ASSERT_EQ(list.front(), "Zero");
    ASSERT_EQ(list.back(), "Beta");

    list.pop_front();
    ASSERT_EQ(list.front(), "Alpha");
    ASSERT_EQ(list.size(), 2);

    list.pop_back();
    ASSERT_EQ(list.front(), "Alpha");
    ASSERT_EQ(list.back(), "Alpha");
    ASSERT_EQ(list.size(), 1);

    list.pop_front();
    ASSERT_TRUE(list.empty());
}

TEST(DsaSuite, BinarySearchTreeOrderingAndPrefix) {
    BinarySearchTree<std::string, int> bst;
    bst.insert("Paracetamol", 1);
    bst.insert("Amoxicillin", 2);
    bst.insert("Azithromycin", 3);
    bst.insert("Atorvastatin", 4);
    bst.insert("Ibuprofen", 5);

    ASSERT_EQ(bst.size(), 5);
    ASSERT_TRUE(bst.contains("Amoxicillin"));
    ASSERT_FALSE(bst.contains("Cetirizine"));

    // In-order traversal must be sorted lexicographically
    auto inOrder = bst.inOrder();
    ASSERT_EQ(inOrder.size(), 5);
    ASSERT_EQ(inOrder[0].first, "Amoxicillin");
    ASSERT_EQ(inOrder[1].first, "Atorvastatin");
    ASSERT_EQ(inOrder[2].first, "Azithromycin");
    ASSERT_EQ(inOrder[3].first, "Ibuprofen");
    ASSERT_EQ(inOrder[4].first, "Paracetamol");

    // Prefix search for "A" should return Amoxicillin, Atorvastatin, Azithromycin
    auto prefixA = bst.findByPrefix("A");
    ASSERT_EQ(prefixA.size(), 3);
    ASSERT_EQ(prefixA[0].first, "Amoxicillin");
    ASSERT_EQ(prefixA[1].first, "Atorvastatin");
    ASSERT_EQ(prefixA[2].first, "Azithromycin");
}

TEST(DsaSuite, QueueFifoOrder) {
    Queue<int> q;
    ASSERT_TRUE(q.empty());

    q.enqueue(10);
    q.enqueue(20);
    q.enqueue(30);

    ASSERT_EQ(q.size(), 3);
    ASSERT_EQ(q.front(), 10);

    ASSERT_EQ(q.dequeue(), 10);
    ASSERT_EQ(q.dequeue(), 20);
    ASSERT_EQ(q.dequeue(), 30);
    ASSERT_TRUE(q.empty());
}

TEST(DsaSuite, StackLifoOrder) {
    Stack<std::string> st;
    ASSERT_TRUE(st.empty());

    st.push("Screen1");
    st.push("Screen2");
    st.push("Screen3");

    ASSERT_EQ(st.size(), 3);
    ASSERT_EQ(st.top(), "Screen3");

    ASSERT_EQ(st.pop(), "Screen3");
    ASSERT_EQ(st.pop(), "Screen2");
    ASSERT_EQ(st.pop(), "Screen1");
    ASSERT_TRUE(st.empty());
}

TEST(DsaSuite, SortingMergeAndQuickSort) {
    std::vector<int> data = {45, 12, 89, 3, 27, 64, 18, 90, 2};

    auto copy1 = data;
    Sorting::mergeSort(copy1);
    ASSERT_EQ(copy1.size(), 9);
    for (size_t i = 1; i < copy1.size(); ++i) {
        ASSERT_TRUE(copy1[i - 1] <= copy1[i]);
    }
    ASSERT_EQ(copy1.front(), 2);
    ASSERT_EQ(copy1.back(), 90);

    auto copy2 = data;
    Sorting::quickSort(copy2);
    ASSERT_EQ(copy2.size(), 9);
    for (size_t i = 1; i < copy2.size(); ++i) {
        ASSERT_TRUE(copy2[i - 1] <= copy2[i]);
    }
    ASSERT_EQ(copy2.front(), 2);
    ASSERT_EQ(copy2.back(), 90);

    // Binary search on sorted array
    auto foundIdx = Sorting::binarySearch(copy1, 27, [](int v) { return v; });
    ASSERT_TRUE(foundIdx.has_value());
    ASSERT_EQ(copy1[*foundIdx], 27);

    auto notFound = Sorting::binarySearch(copy1, 999, [](int v) { return v; });
    ASSERT_FALSE(notFound.has_value());
}
