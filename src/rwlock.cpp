#include "rwlock.h"

rwlock_wfirst::rwlock_wfirst() : m_wcount(0), m_rcount(0), is_writing(false) {
    
}

// 多个写操作会阻塞后续来的读请求,增加了m_wcount，写操作的wait条件是m_wcount == 0
void rwlock_wfirst::lock() {
    std::unique_lock<std::mutex> lk(mut);
    ++m_wcount;  
    cond_w.wait(lk, [this](){ return !this->is_writing && this->m_rcount == 0;});
    is_writing = true;
}

void rwlock_wfirst::unlock() {
    std::lock_guard<std::mutex> lk(mut);
    --m_wcount;
    if (m_wcount > 0) {
        cond_w.notify_one();  //如果还有写请求排队，唤醒写请求
    } else {
        cond_r.notify_all();
    }
    is_writing = false;
}

bool rwlock_wfirst::try_lock() {
    std::lock_guard<std::mutex> lk(mut);
    if (m_wcount == 0 && m_rcount == 0) {
        ++m_wcount;
        is_writing = true;
        return true;
    }
    return false;
}

void rwlock_wfirst::lock_shared() {
    std::unique_lock<std::mutex> lk(mut);
    cond_r.wait(lk, [this]() { return this->m_wcount == 0; });
    ++m_rcount;
}

void rwlock_wfirst::unlock_shared() {
    std::lock_guard<std::mutex> lk(mut);
    --m_rcount;

    // 一开始有一些读操作，然后有写操作，会阻塞后续的读操作
    // 等待之前的读操作结束后,唤醒写操作
    if ( m_wcount > 0 && m_rcount == 0) {
        cond_w.notify_one();
    }
}

bool rwlock_wfirst::try_lock_shared() {
    std::lock_guard<std::mutex> lk(mut);
    if (m_wcount != 0) {
        return false;
    }
    m_rcount++;
    return true;
}

/****************************************************************************
 ****************************************************************************/

// 读优先读写锁实现

// 读操作会阻塞后续来的写操作
rwlock_rfirst::rwlock_rfirst() : m_wcount(0), m_rcount(0), write_waiting(0), is_writing(false) {

}

void rwlock_rfirst::lock() {
    std::unique_lock<std::mutex> lk(mut);

    // 等待读操作+1，读操作会被后来的写操作阻塞住
    ++write_waiting;
    cond_w.wait(lk, [this](){ return !is_writing && this->m_rcount == 0;});
    --write_waiting;
    ++m_wcount; 
    is_writing = true;
}

void rwlock_rfirst::unlock() {
    std::lock_guard<std::mutex> lk(mut);
    --m_wcount;

    // 优先唤醒读进程， --m_wcount之后必然为0，因为m_wcount是在wait之后才增加
    if (m_rcount > 0) {
        cond_r.notify_all();
    } else if (write_waiting > 0){
        cond_w.notify_one();
    }
    is_writing = false;
}

bool rwlock_rfirst::try_lock() {
    std::lock_guard<std::mutex> lk(mut);
    if (m_wcount == 0 && m_rcount == 0) {
        ++m_wcount;
        is_writing = true;
        return true;
    }
    return false;
}

void rwlock_rfirst::lock_shared() {
    std::unique_lock<std::mutex> lk(mut);
    ++m_rcount; //先进行自增，阻塞后续的写操作
    cond_r.wait(lk, [this]() { return this->m_wcount == 0; });
}

void rwlock_rfirst::unlock_shared() {
    std::lock_guard<std::mutex> lk(mut);
    --m_rcount;

    // 只要有读操作在进行，写操作的m_wcount一定恒为0
    if (m_rcount == 0 && write_waiting > 0) {
        cond_w.notify_one();
    }
}

bool rwlock_rfirst::try_lock_shared() {
    std::lock_guard<std::mutex> lk(mut);
    if (m_wcount != 0) {
        return false;
    }
    m_rcount++;
    return true;
}