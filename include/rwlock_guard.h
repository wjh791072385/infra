template <typename _RWlock>
class rwlock_readguard{
  public:
    explicit rwlock_readguard(_RWlock &rw_lock) : _rw_lock(rw_lock) {
        _rw_lock.lock_shared();
    } 

    ~rwlock_readguard() {
      _rw_lock.unlock_shared();
    }
  private:
    rwlock_readguard() = delete;
    rwlock_readguard(rwlock_readguard &) = delete;
    rwlock_readguard& operator=(const rwlock_readguard&) = delete;

  private:
    _RWlock &_rw_lock;
};

template <typename _RWlock>
class rwlock_writeguard{
  public:
    explicit rwlock_writeguard(_RWlock &rw_lock) : _rw_lock(rw_lock) {
        _rw_lock.lock();
    } 

    ~rwlock_writeguard() {
      _rw_lock.unlock();
    }
  private:
    rwlock_writeguard() = delete;
    rwlock_writeguard(rwlock_writeguard &) = delete;
    rwlock_writeguard& operator=(const rwlock_writeguard&) = delete;

  private:
    _RWlock &_rw_lock;
};
