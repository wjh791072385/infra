#include "gtest/gtest.h"
#include <thread>

using namespace std;


class write_priotity_lock
{
public:
    void read_lock()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_read_cv.wait(lock,[this](){
            return this->m_write_count == 0;
        });

        m_read_count++;
    }

    void write_lock()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_write_count++;
        m_write_cv.wait(lock,[this](){
            return !is_writing && this->m_read_count == 0;
        });
        is_writing = true;
    }

    void read_release()
    {
        std::unique_lock<std::mutex> lock(m_mutex); // <-- 此处的锁是必须的！！
        --m_read_count;
        if(m_read_count == 0 && m_write_count > 0)
        {
            m_write_cv.notify_one();
        }
    }

    void write_release()
    {
        std::unique_lock<std::mutex> lock(m_mutex); // <-- 此处的锁是必须的！！
        --m_write_count;
        if(m_write_count >= 1)
        {
            m_write_cv.notify_one(); // 唤醒一个等待的写条件变量
        }
        else
        {
            m_read_cv.notify_all(); // 唤醒所有等待的写条件变量
        }
        is_writing = false;
    }

private:
    std::condition_variable m_write_cv;
    std::condition_variable m_read_cv;
    bool is_writing;
    int32_t m_read_count;
    int32_t m_write_count;
    std::mutex m_mutex;
};

write_priotity_lock pl;

void testfun() {
    pl.write_lock();

    // sleep(1);

    pl.write_release();
}

TEST(TestDemo, testdemo) 
{
    thread *tds[10];
    for (int i = 0; i < 10; i++) {
        tds[i] = new thread(testfun);
    }

    for (int i = 0; i < 10; i++) {
        tds[i]->join();
    }
}