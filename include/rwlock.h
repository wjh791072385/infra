#include <mutex>
#include <condition_variable>

class rwlock{
  public:
    rwlock() = default;
    ~rwlock() = default;

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
    int32_t m_rcount;
    int32_t m_wcount;
    
};