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

typedef struct {
    uint32_t count;
    uint32_t score;
    list_head_t list;
}node;

TEST(TestList, test_list) {
    // 一般head定义为栈变量
    list_head_t head;
    list_init(&head);

    node *ns[5];
    for (uint32_t i = 0; i < 5; i++) {
        ns[i] = (node *)malloc(sizeof(node));
        ns[i]->count = i + 100;
        ns[i]->score = i + 1000;
        list_add_tail(&ns[i]->list, &head);
    }


    list_head_t *cur, *next;
    int i = 0;
    list_foreach_safe(cur, next, &head) {
        // 直接访问cur即可
        node *t = (node *)container_of(cur, node, list);
        EXPECT_EQ(i + 100, t->count);
        EXPECT_EQ(i + 1000, t->score);
        i++;
    }

    EXPECT_EQ(i, 5);


    // 删除节点
    list_foreach_safe(cur, next, &head) {
        list_del(cur);

        // 此时cur是独立的节点
        node *t = (node *)container_of(cur, node, list);
        free(t);
    }

    // 再次遍历判断size
    i = 0;
    list_foreach_safe(cur, next, &head) {
        // 直接访问cur即可
        i++;
    }
    EXPECT_EQ(i, 0);
}