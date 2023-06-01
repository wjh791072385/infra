#ifndef __RWLOCK_H__
#define __RWLOCK_H__

#include <mutex>
#include <condition_variable>

class rwlock_wfirst;
typedef rwlock_wfirst rwlock_wfirst_t;

class rwlock_wfirst{
  public:
    rwlock_wfirst();
    ~rwlock_wfirst() = default;

    void lock();
    bool try_lock();
    void unlock();
    
    void lock_shared();
    bool try_lock_shared();
    void unlock_shared();
  
  public:
    std::mutex mut;
    std::condition_variable cond_w;
    std::condition_variable cond_r;
    bool is_writing;
    int32_t m_rcount;
    int32_t m_wcount;
};

/****************************************************************************
 ****************************************************************************/

#include <mutex>
#include <condition_variable>

class rwlock_rfirst;
typedef rwlock_rfirst rwlock_rfirst_t;

class rwlock_rfirst{
  public:
    rwlock_rfirst();
    ~rwlock_rfirst() = default;

    void lock();
    bool try_lock();
    void unlock();
    
    void lock_shared();
    bool try_lock_shared();
    void unlock_shared();
  
  public:
    std::mutex mut;
    std::condition_variable cond_w;
    std::condition_variable cond_r;
    bool is_writing;
    int32_t write_waiting;
    int32_t m_rcount;
    int32_t m_wcount;
};

#endif