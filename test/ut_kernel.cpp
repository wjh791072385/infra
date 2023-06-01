#include "gtest/gtest.h"
#include "kernel_utils.h"


typedef struct{
    int a;
    double b;
    long long c;
}num_t;

TEST(TestContainer, test_Container) {
    num_t n = {100, 200, 300};
    num_t *t = container_of(&n.b, num_t, b);
    EXPECT_EQ(t->a, 100);
    EXPECT_EQ(t->b, 200);
    EXPECT_EQ(t->c, 300);
}