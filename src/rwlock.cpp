#include "rwlock.h"

void rwlock::lock() {
    std::unique_lock<std::mutex> lk(mut);
    ++m_wcount;  // 多个写操作会阻塞后续来的读请求
    cond_w.wait(lk, [this](){ return this->m_rcount == 0;});
}

void rwlock::unlock() {
    std::lock_guard<std::mutex> lk(mut);
    --m_wcount;
    if (m_wcount > 0) {
        cond_w.notify_one();  //如果还有写请求排队，唤醒写请求
    } else {
        cond_r.notify_all();
    }
}

bool rwlock::try_lock() {
    std::lock_guard<std::mutex> lk(mut);
    if (m_wcount == 0 && m_rcount == 0) {
        ++m_wcount;
        return true;
    }
    return false;
}

void rwlock::lock_shared() {
    std::unique_lock<std::mutex> lk(mut);
    cond_r.wait(lk, [this]() { this->m_wcount == 0; });
    ++m_rcount;
}

void rwlock::unlock_shared() {
    std::lock_guard<std::mutex> lk(mut);
    --m_rcount;
    if ( m_wcount > 0 && m_rcount == 0) {
        cond_w.notify_one();
    }
}

bool rwlock::try_lock_shared() {
    std::lock_guard<std::mutex> lk(mut);
    if (m_wcount != 0) {
        return false;
    }
    m_rcount++;
    return true;
}