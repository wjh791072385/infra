#include "gtest/gtest.h"
#include "rwlock_guard.h"
#include "rwlock.h"
#include "rwlock_boost.h"
#include <chrono>
#include <mutex>

using namespace std;

#define THREAD_NUM 4
#define WRITE_CNT 1000
#define READ_CNT 100000

/* *************************************************************

临界区读操作为遍历长度1w的vector<br>
临界区写操作为向vector中追加一个数字

|               | 4线程20w次读2000次写 | 4线程10w次读10w次写 |
| ------------- | -------------------- | ------------------- |
| mutex         | 20.1295s             | 8.48541             |
| rwlock_wfirst | 9.09972s             | 8.79313             |
| rwlock_rfirst | 8.70412s             | 4.46938             |
| rwlock_boost  | 9.92822s             | 8.13775             |

可以看出在存在大量读数据的情况下，使用读优先的锁，会有一定的优势（仅仅根据当前场景测试
************************************************************* */

enum locktype{
    rwlock_wfirst_type,
    rwlock_rfirst_type,
    rwlock_boost_type,
    mutex_lock_type
};

rwlock_wfirst rwt;
rwlock_rfirst_t rrt;
mutex mt;
shared_mutex smu;

void bench_test_write(locktype type, vector<int> &vec) {
    for (int i = 0; i < WRITE_CNT; i++) {
        if (type == rwlock_wfirst_type) {
            rwlock_writeguard<rwlock_wfirst> lg(rwt);
            vec.push_back(i);
        } else if (type == rwlock_rfirst_type) {
            rwlock_writeguard<rwlock_rfirst> lg(rrt);
            vec.push_back(i);
        } else if (type == rwlock_boost_type){
            smu.lock();
            vec.push_back(i);
            smu.unlock();
        } else {
            lock_guard<std::mutex> lg(mt);
            vec.push_back(i);
        }
    }
}

// 更好的测试读场景：把读操作放到后台异步任务中，读操作结束后，通过回调函数释放锁
void bench_test_read(locktype type, vector<int> &vec) {
    for (int i = 0; i < READ_CNT; i++) {
        if (type == rwlock_wfirst_type) {
            rwlock_readguard<rwlock_wfirst> lg(rwt);
            for (auto &c : vec) {}
        } else if (type == rwlock_rfirst_type) {
            rwlock_readguard<rwlock_rfirst> lg(rrt);
            for (auto &c : vec) {}
        } else if (type == rwlock_boost_type){
            smu.lock_shared();
            for (auto &c : vec) {}
            smu.unlock_shared();
        } else {
            lock_guard<std::mutex> lg(mt);
            for (auto &c : vec) {}
        }
    }
}

TEST(Testrwlock, test_all)
{			
    thread *tds[THREAD_NUM];

    vector<int> vec(10000);  //用来模拟临界区的访问

    auto start = std::chrono::high_resolution_clock::now();

    // 根据传入不同的locktype 测试不同实现的读写锁
    for (int i = 0; i < THREAD_NUM; i++) {
        if (i & 1) {
            tds[i] = new thread(bench_test_write, rwlock_rfirst_type, ref(vec));
        } else {
            tds[i] = new thread(bench_test_read, rwlock_rfirst_type, ref(vec));
        } 
    }

    for (int i = 0; i < THREAD_NUM; i++) {
        tds[i]->join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "spend time:" << elapsed.count() << std::endl;
}


