#include "gtest/gtest.h"
#include "skiplist_v1.h"

TEST(TestSkiplist, test_all)
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
    sp.display_list();

    node = sp.search_element(3);
    EXPECT_TRUE(node == nullptr);
}

