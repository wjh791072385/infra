#include "gtest/gtest.h"
#include "skiplist_v1.h"
#include <thread>
#include <ctime>

using namespace std;

#define MAX_THREAD_NUM 1
// #define TEST_COUNT 1000000
#define TEST_COUNT 1000

TEST(TestSkiplist, function)
{
    skiplist<int, int> sp(6);
    int ret = sp.insert_element(3, 5);
    EXPECT_EQ(ret, 0);

    ret = sp.insert_element(3, 6);
    EXPECT_EQ(ret, 1);

    ret = sp.insert_element(1, 5);
    EXPECT_EQ(ret, 0);

    ret = sp.insert_element(18, 5);
    EXPECT_EQ(ret, 0);

    skiplist_node<int, int>* node= sp.search_element(3);
    EXPECT_TRUE(node != nullptr);
    EXPECT_EQ(node->value, 6);

    node = sp.search_element(4);
    EXPECT_TRUE(node == nullptr);

    ret = sp.delete_element(3);

    node = sp.search_element(3);
    EXPECT_TRUE(node == nullptr);
}

void test_performance(skiplist<int, int> &sp) {
    
    srand(time(0));
    for (int i = 0; i < TEST_COUNT; i++) {
        sp.insert_element(rand(), 1);
    }

    for (int i = 0; i < TEST_COUNT; i++) {
        sp.search_element(rand());
    }
}

TEST(TestSkiplist, performance)
{
    skiplist<int, int> sp(32);
    thread *tds[MAX_THREAD_NUM];

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < MAX_THREAD_NUM; i++) {
        tds[i] = new thread(test_performance, ref(sp));
    }

    for (int i = 0; i < MAX_THREAD_NUM; i++) {
        tds[i]->join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    // std::cout << "spend time:" << elapsed.count() << std::endl;
}

