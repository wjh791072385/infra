#include "gtest/gtest.h"
#include "async_jobcache_v0.h"
#include <unistd.h>
#include <thread>
#include <stdio.h>

using namespace std;

map<int, int> mp;  //用来存储已经handle过的数据

// handle_data需要为非阻塞，才能并发，非阻塞最好的方法就是里面再开一个协程/线程之类的去处理
// 且handle需要为异步接口，即支持回调
int handle_data(void *data, void *cb_data, void (*cb_func)(int ret, void *cb_data)) {
    int *t = (int *)data;
    // printf("handle data %d\n", *t);
    mp[*t]++;
    cb_func(0, cb_data); // 传0表示处理成功, 其他返回值表示失败
    return 0;
}

void worker(jobcache_t *cache, int data) {
    jobcache_insert(cache, &data, sizeof(data));

    data += 10;
    jobcache_insert(cache, &data, sizeof(data));
}

TEST(TestJobcache, test_jobcache) {
    jobcache_t *cache = jobcache_init();
    jobcache_register(cache, handle_data);

    int data = 100;
    jobcache_insert(cache, &data, sizeof(data));

    data = 202;
    jobcache_insert(cache, &data, sizeof(data));
    usleep(2000);

    // 多线程插入
    thread *ths[10];
    for (int i = 0; i < 10; i++) {
        ths[i] = new thread(worker, cache, i + 10);
    }

    for (int i = 0; i < 10; i++) {
        ths[i]->join();
    }

    usleep(5000);
    jobcache_exit(cache);

    // 校验处理了的结果
    EXPECT_EQ(mp[100], 1);
    EXPECT_EQ(mp[202], 1);

    for (int i = 0; i < 10; i ++) {
        EXPECT_EQ(mp[i + 10], 1);
        EXPECT_EQ(mp[i + 20], 1);
    }
}