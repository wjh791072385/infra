#include "thread_res_v0.h"
#include <stdlib.h>
#include <stdatomic.h>
#include <stdint.h>
#include <semaphore.h>
#include <stdio.h>

#define MAX_HIT 3

struct thread_res{

    // 因为可能在多线程环境下访问,使用原子变量
    // 表示当前线程是否在运行中
    atomic_bool             run;

    // 表示当前线程是否在工作中, 似乎不用原子变量也行
    // atomic_bool     busy;
    // lock用来保护 busy和wait
    pthread_spinlock_t      lock;
    bool                    busy;
    sem_t                   wait;

    // 用于协同控制thread_res
    sem_t                   ctrl;

    // 记录线程连续执行任务次数
    uint32_t                hits;

    void                  (*cb_func) (void *);
    void                   *cb_data;
    bool                  (*need_sleep) (void *);
    void                  (*clean_up) (void *);
    pthread_t               pid;
};


static void* worker(void *arg) {
    thread_res_t *res = (thread_res_t *)arg;
    res->hits = 0;
    sem_init(&res->wait, 0, 0);

    // 表示整体初始化完成
    sem_post(&res->ctrl);

    while (atomic_load(&res->run)) {
        // 先判断是否满足睡眠条件
        pthread_spin_lock(&res->lock);
        if (res->need_sleep(res->cb_data)) {
            res->busy = false;
            pthread_spin_unlock(&res->lock);

            res->hits = 0;

            sem_wait(&res->wait);

            // 唤醒后先判断是不是destory唤醒的
            if ( !atomic_load(&res->run) ) {
                break;
            }
        } else {
            pthread_spin_unlock(&res->lock);
        }

        // 处理请求，最多MAX_HIT次，避免其他线程任务被饿死
        res->cb_func(res->cb_data);
        if ( ++res->hits > MAX_HIT ) {
            sched_yield();
        }
    }

    if (res->clean_up != NULL) {
        res->clean_up(res->cb_data);
    }

    // 唤醒ctrl,结束while循环一定调用了destory
    sem_post(&res->ctrl);
    sem_destroy(&res->wait);
    return 0;
}



thread_res_t* thread_res_create(void *cb_data, void (*cb_func) (void *),
                                bool (*need_sleep) (void *), void (*clean_up) (void *)) {
    // 分配内存
    thread_res_t *res = (thread_res_t *)malloc(sizeof(thread_res_t));
    res->cb_data = cb_data;
    res->cb_func = cb_func;
    res->need_sleep = need_sleep;
    res->clean_up = clean_up;
    atomic_init(&res->run, true);
    atomic_init(&res->busy, false);
    pthread_spin_init(&res->lock, 0);

    // ctrl协同控制很重要
    // 此处必须要用ctrl，保证wait也初始化完成，防止可能出现wait还没init，就执行thread_res_wakeup
    // 常驻线程一开始应该是需要进入睡眠状态的，由res_wakeup来把状态该为busy
    sem_init(&res->ctrl, 0, 0);

    int ret = 0;
    do {
        ret = pthread_create(&res->pid, NULL, worker, res);
        if ( 0 != ret ) {
            break;
        }

        ret = pthread_detach(res->pid);
    } while(0);

    sem_wait(&res->ctrl);
    sem_destroy(&res->ctrl);


    if ( 0 != ret ) {
        pthread_spin_destroy(&res->lock);
        free(res);
        return NULL;
    }

    return res;
}

void thread_res_destory(thread_res_t *res) {
    if ( NULL == res ) {
        return;
    }

    sem_init(&res->ctrl, 0, 0);

    // 置为false，如果当前正在运行，下一次循环后会退出
    atomic_store(&res->run, false);

    // 如果处于睡眠则需要唤醒
    // 如果正在运行，多post一次也没事，因为即将销毁
    sem_post(&res->wait);

    // 等待worker结束
    sem_wait(&res->ctrl);
    sem_destroy(&res->ctrl);

    pthread_spin_destroy(&res->lock);
    free(res);
}


void thread_res_wakeup(thread_res_t *res) {
    pthread_spin_lock(&res->lock);

    // 如果在睡眠则唤醒
    if ( !res->busy )
    {
        res->busy = true;
        sem_post(&res->wait);
    }
    pthread_spin_unlock(&res->lock);
}