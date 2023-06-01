#ifndef __THREADPOOL_V1_H__
#define __THREADPOOL_V1_H__

#include <deque>
#include <semaphore.h>
#include <exception>
#include <pthread.h>
#include <atomic>

template<typename T>
class threadpool{
  public:
    threadpool(int max_thread_num, int max_request_num);
    ~threadpool();
    int append(T *t);
    bool is_completed();

  private:
    static void* worker(void *args); // pthread_create要求传入函数返回void *
    void run();

  public:
    std::atomic_bool is_running;
    int max_thread_num;
    std::atomic_uint32_t busy_thread_num; //记录正在忙的数量，用于保证每个线程都执行完
    int max_request_num;
    pthread_t *tds;

    pthread_mutex_t m_mtx;
    std::deque<T *> m_task_queue; // 任务队列
    sem_t *m_sem; 
};

template<typename T>
threadpool<T>::threadpool(int max_thread_num, int max_request_num) 
    : max_thread_num(max_thread_num), max_request_num(max_request_num), is_running(true) {
    int ret = 0;
    do {
        // ret = sem_init(&m_sem, 0);
        //  if ( 0 != ret ) {
        //     break;
        // }
    
        // Mac OS X不支持创建无名的信号量，只能使用sem_open创建有名的信号量
        // 注意传入信号量名字不能带路径
        m_sem = sem_open("mysem", 0);
       
        ret = pthread_mutex_init(&m_mtx, NULL);
        if (0 != ret) {
            break;
        }

        // init threads
        tds = new pthread_t[max_thread_num];
        if (NULL == tds) {
            ret = -1;
            break;
        }

        for (int i = 0; i < max_thread_num; i++) {
            ret = pthread_create(&tds[i], NULL, worker, this);
            if (0 != ret) {
                break;
            }
        }

        if (0 != ret) break;

        for (int i = 0; i < max_thread_num; i++) {
            ret = pthread_detach(tds[i]);
            if (ret != 0) {
                break;
            }
        }
    } while(0);
    
    if (0 != ret) {
        throw std::exception();
    }
}

template<typename T>
threadpool<T>::~threadpool() {
    is_running = false;
    // sem_destroy(&m_sem);
    sem_unlink("mysem");
    pthread_mutex_destroy(&m_mtx);
    delete []tds;
}

template<typename T>
int threadpool<T>::append(T *t) {
    if (!is_running.load()) return false;
    pthread_mutex_lock(&m_mtx);

    if (m_task_queue.size() >= max_request_num) {
        pthread_mutex_unlock(&m_mtx);
        return -1;
    }
    m_task_queue.push_back(t);
    pthread_mutex_unlock(&m_mtx);
    
    // 唤醒工作线程
    sem_post(m_sem);
    return 0;
}

template<typename T>
void* threadpool<T>::worker(void *args) {
    threadpool *pool = (threadpool *)args;
    pool->run();
    return pool;
}

template<typename T> 
void threadpool<T>::run() {
    while (is_running.load()) {
        sem_wait(m_sem);
        pthread_mutex_lock(&m_mtx);
        if (m_task_queue.empty()) {
            pthread_mutex_unlock(&m_mtx);
            continue;
        }

        // 取出任务执行
        T *request = m_task_queue.front();
        m_task_queue.pop_front();
        pthread_mutex_unlock(&m_mtx);
        if (NULL == request) {
            continue;
        } 

        // 执行任务
        busy_thread_num.fetch_add(1);
        request->process();
        busy_thread_num.fetch_sub(1);

    }
}

template<typename T> 
bool threadpool<T>::is_completed() {
    return busy_thread_num.load() == 0;
}

#endif
