#include "gtest/gtest.h"
#include "thread_res_v0.h"
#include <stdint.h>

thread_res_t *g_thread_res = NULL;

typedef struct {
    uint32_t    job_nums;
}job_t;

bool sleep_flag = false;

bool need_sleep(void *args) {
    job_t *job = (job_t *)args;
    if (job->job_nums == 0) {
        sleep_flag = true;
        return true;
    }
    sleep_flag = false;
    return false;
}

void handle(void *args) {
    job_t *job = (job_t *)args;
    // printf("job_nums = %d\n", job->job_nums);
    job->job_nums--;
}

TEST(TestThread_res_v0, test_res) {
    job_t *job = new job_t();
    job->job_nums = 5;

    g_thread_res = thread_res_create(job, handle, need_sleep, NULL);

    // 等待任务做完，此时线程睡眠
    usleep(1000);
    EXPECT_EQ(job->job_nums, 0);
    EXPECT_TRUE(sleep_flag);

    job->job_nums = 2;
    thread_res_wakeup(g_thread_res);

    usleep(1000);
    EXPECT_EQ(job->job_nums, 0);
    EXPECT_TRUE(sleep_flag);

    thread_res_destory(g_thread_res);

    cout << "常驻线程测试ok" << endl;
}