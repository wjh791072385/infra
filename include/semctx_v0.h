/*******************************************************************************
* Description: c semctx
* Author     :
* Create     : 2023/xx/xx
*
*******************************************************************************/

#ifndef __SEMCTX_V0_H__
#define __SEMCTX_V0_H__

#include <semaphore.h>
#include <atomic>

struct semctx_t
{
    sem_t sem;
    int ret;
};

static inline void semctx_init(semctx_t *ctx)
{
    sem_init(&ctx->sem, 0, 0);
    ctx->ret = 0;
}

static inline void semctx_fini(semctx_t *ctx)
{
    sem_destroy(&ctx->sem);
}

static inline void semctx_down(semctx_t *ctx)
{
    sem_wait(&ctx->sem);
}

static inline void semctx_up(int ret, void *args)
{
    semctx_t *ctx = (semctx_t *)args;
    ctx->ret = ret;
    sem_post(&ctx->sem);
}

/*******************************************************************************
*******************************************************************************/

// 批量异步处理多任务，阻塞直到全都结束
// 每次使用前先 atomic_s32_inc(&ctx.notback), 然后将async_ctx_up传入回调
// 在多个任务后调用async_ctx_down来阻塞住，确保批量执行成功，以及ret是否存在不为0的
struct async_ctx_t
{
    sem_t sem;
    int ret;
    std::atomic_int32_t notback;  // 必须是原子变量，保证线程安全
};

static inline void async_ctx_init(async_ctx_t *ctx)
{
    sem_init(&ctx->sem, 0, 0);
    ctx->ret = 0;
    atomic_init(&ctx->notback, 1);
}

static inline void async_ctx_fini(async_ctx_t *ctx)
{
    sem_destroy(&ctx->sem);
}

static inline void async_ctx_down(async_ctx_t *ctx)
{
    if ( --ctx->notback > 0 ) {
        sem_wait(&ctx->sem);
    }
}

static inline void async_ctx_up(int ret, void *args)
{
    async_ctx_t *ctx = (async_ctx_t *)args;
    if ( 0 != ret ) {
        ctx->ret = ret;
    }

    if ( 0 == (--ctx->notback)) {
        sem_post(&ctx->sem);
    }
}

#endif