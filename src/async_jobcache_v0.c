#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <string.h>
#include <stdio.h>
#include "async_jobcache_v0.h"
#include "kernel_utils.h"
#include "thread_res_v0.h"

#define CAPACITY        4096
#define MAXDEPTH        1024
#define MAX_RETRY_CNT   3       // 最大重试次数
#define MAX_RETRY_TIME  5000    // 5000ms

struct jobcache{
    // jobs management
    uint32_t                capacity;
    atomic_uint_least32_t   size;
    pthread_spinlock_t      lock;
    list_head_t             list;

    // job handle
    int                   (*handle) (void *, void *cb_data, void (*cb_func)(int ret, void *cb_data));

    // concurrency 并发数
    atomic_uint_least32_t   depth;

    // resident thread
    thread_res_t            *worker;
}jobcache;


typedef struct {
    // 重试时需要能关联到cache
    jobcache_t         *cache;

    list_head_t          link;

    void                *data;

    // 重试次数
    uint32_t            retry_cnt;
}_job_t;

bool _job_sleep(void *args) {
    jobcache_t *cache = (jobcache_t *)args;

    // 在thread_res中是先判断need_sleep, 再执行_worker函数
    // 如果需要执行的任务数量为0，显然需要挂起
    // 如果并发度depth已经用完了，也要暂时挂起，但job已经在insert中插入了，不会丢失
    return atomic_load(&cache->size) == 0 || atomic_load(&cache->depth);
}


void _job_free(_job_t *job) {
    if ( NULL == job ) {
        return;
    }

    if ( NULL != job->data) {
        free(job->data);
    }

    free(job);
}

_job_t* _job_alloc(jobcache_t *cache, void *data, uint32_t len) {
    _job_t *job = (_job_t *)malloc(sizeof(_job_t));

    job->data = malloc(len);
    if ( NULL == job->data ) {
        // log_error("malloc job data failed")
        return NULL;
    }

    memcpy(job->data, data, len);

    job->cache = cache;
    job->retry_cnt = 0;

    return job;
}

_job_t* _job_deq(jobcache_t *cache) {
    pthread_spin_lock(&cache->lock);

    if ( 0 == atomic_load(&cache->size) ) {
        return NULL;
    }

    list_head_t *link = NULL;
    list_get_first(&link, &cache->list);
    if ( link == NULL ) {
        pthread_spin_unlock(&cache->lock);
        return NULL;
    }

    // 取出任务
    list_del(link);
    _job_t *job = (_job_t *)container_of(link, _job_t, link);

    pthread_spin_unlock(&cache->lock);

    return job;
}

void _job_enq(jobcache_t *cache, _job_t *job) {
    pthread_spin_lock(&cache->lock);

    list_add_tail(&job->link, &job->cache->list);
    atomic_fetch_add(&job->cache->size, 1);

    pthread_spin_unlock(&cache->lock);
}

void _job_start(jobcache_t *cache) {
    thread_res_wakeup(cache->worker);
}

void _job_done(int ret, void *args) {
    _job_t *job = (_job_t *)args;
    jobcache_t *cache = job->cache;

    if (0 == ret) {
        _job_free(job);
    } else {
        // 失败则重试
        if ( ++job->retry_cnt <= MAX_RETRY_CNT ) {
            _job_enq(cache, job);
            // log_info("job retry %st, ret = ", job->retry_cnt, ret);
            // printf("job retry %d st, ret(%d)\n", job->retry_cnt, ret);
        } else {
            // log_info("job retry over max_retry_num, ret(%d)", ret);
            // printf("job retry over max_retry_num, ret(%d)\n", ret);
        }
    }

    // 归还并发度
    atomic_fetch_add(&cache->depth, 1);

    // 触发任务
    _job_start(cache);
}


static void _worker(void *args) {
    jobcache_t *cache = (jobcache_t *)args;

    // 限制并发度
    while (atomic_fetch_add(&cache->depth, -1) > 0) {
        _job_t *job = _job_deq(cache);

        if ( NULL == job ) {
            // 队列已经取空了，还原并发度，直接break
            atomic_fetch_add(&cache->depth, 1);
            break;
        }

        // _worker并发执行，前提是handle需要是非阻塞的
        // 这里handle可以改为异步接口，cache->handle(job->data, job, _job_done)
        cache->handle(job->data, job, _job_done);
    }
}

/*******************************************************************************
*******************************************************************************/

jobcache_t* jobcache_init() {
    jobcache_t *cache = (jobcache_t *)malloc(sizeof(jobcache_t));
    cache->size = 0;
    cache->capacity = CAPACITY;
    cache->depth = MAXDEPTH;

    pthread_spin_init(&cache->lock, 0);
    list_init(&cache->list);
    cache->worker = thread_res_create(cache, _worker, _job_sleep, NULL);

    if ( NULL == cache->worker ) {
        return NULL;
    }
    return cache;
}

void jobcache_exit(jobcache_t *cache) {
    thread_res_destory(cache->worker);

    // clear job
    pthread_spin_lock(&cache->lock);

    list_head_t *cur, *next;
    list_foreach_safe(cur, next, &cache->list) {
        list_del(cur);

        if ( NULL != cur )
        {
            _job_t *t = (_job_t*)container_of(cur, _job_t, link);
            _job_free(t);
        }
    }

    pthread_spin_unlock(&cache->lock);

    pthread_spin_destroy(&cache->lock);
}

void jobcache_register(jobcache_t *cache, int (*handle) (void *, void *cb_data, void (*cb_func)(int ret, void *cb_data))) {
    if (handle == NULL) {
        return;
    }
    cache->handle = handle;
}

int jobcache_insert(jobcache_t *cache, void *data, uint32_t len) {
    if (atomic_load(&cache->size) >= cache->capacity) {
        // log_info("jobcache is full")
        return -1;
    }

    // 不使用深拷贝，data由外部负责申请，jobcache负责销毁
    // 使用深拷贝，data数据需要完全复制一份，data在外部可复用，更合理
    int ret = 0;
    do {
        _job_t *job = _job_alloc(cache, data, len);
        if ( NULL == job ) {
            ret = -1;
            break;
        }

        // 加入cache
        _job_enq(cache, job);

        // 触发任务
        _job_start(cache);

    } while(0);

    return ret;
}