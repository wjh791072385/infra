#include <unistd.h>
#include <iostream>
#include <thread>
#include "gtest/gtest.h"
#include "semctx_v0.h"

using namespace std;

// 实际后台执行的任务
void job(int parm, void *cb_data, void (*cb_func) (int, void *)) {
    // cout << "sleep " << parm << "s...." << endl;
    usleep(parm);
    cb_func(parm, cb_data);
}

void job_nonblock(int parm, void *cb_data, void (*cb_func) (int, void *)) {
    thread td(job, parm, cb_data, cb_func);
    td.detach();
}

TEST(TestSemctx_v0, test_semctx)
{
    semctx_t ctx;
    semctx_init(&ctx);
    job_nonblock(2, &ctx, semctx_up);
    semctx_down(&ctx);
    semctx_fini(&ctx); // 只是销毁了信号量

    // cout << "ret = " << ctx.ret << endl;

}

TEST(TestSemctx_v0, test_async_ctx)
{
    async_ctx_t ctx;
    async_ctx_init(&ctx);

    for (int i = 0; i < 5; i++) {
        ctx.notback++;  // 这里直接++, 是<atomic>中的原子操作
        job_nonblock(i, &ctx, async_ctx_up);
    }

    // cout << "block..." << endl;
    async_ctx_down(&ctx);
    async_ctx_fini(&ctx);
    // cout << "ret = " << ctx.ret << endl;
}