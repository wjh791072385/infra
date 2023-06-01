#ifndef __RWLOCK_BOOST_H__
#define __RWLOCK_BOOST_H__

#include <mutex>
#include <condition_variable>

class shared_mutex
    {
      typedef std::mutex              mutex_t;
      typedef std::condition_variable cond_t;
      typedef unsigned                count_t;

      mutex_t mut_;
      cond_t  gate1_;
      // the gate2_ condition variable is only used by functions that
      // have taken write_entered_ but are waiting for no_readers()
      cond_t  gate2_;
      count_t state_;

      static const count_t write_entered_ = 1U << (sizeof(count_t) - 1);
      static const count_t n_readers_ = ~write_entered_;

      bool no_writer() const
      {
        return (state_ & write_entered_) == 0;
      }

      bool one_writer() const
      {
        return (state_ & write_entered_) != 0;
      }

      bool no_writer_no_readers() const
      {
        //return (state_ & write_entered_) == 0 &&
        //       (state_ & n_readers_) == 0;
        return state_ == 0;
      }

      bool no_writer_no_max_readers() const
      {
        return (state_ & write_entered_) == 0 &&
               (state_ & n_readers_) != n_readers_;
      }

      bool no_readers() const
      {
        return (state_ & n_readers_) == 0;
      }

      bool one_or_more_readers() const
      {
        return (state_ & n_readers_) > 0;
      }

      shared_mutex(shared_mutex const&);
      shared_mutex& operator=(shared_mutex const&);

    public:
      shared_mutex();
      ~shared_mutex();

      // Exclusive ownership

      void lock();
      bool try_lock();
      void unlock();

      // Shared ownership

      void lock_shared();
      bool try_lock_shared();
      void unlock_shared();
    };

    inline shared_mutex::shared_mutex()
    : state_(0)
    {
    }

    inline shared_mutex::~shared_mutex()
    {
      std::lock_guard<mutex_t> _(mut_);
    }

    // Exclusive ownership

    inline void shared_mutex::lock()
    {
      std::unique_lock<mutex_t> lk(mut_);
      gate1_.wait(lk, std::bind(&shared_mutex::no_writer, std::ref(*this)));
      state_ |= write_entered_;
      gate2_.wait(lk, std::bind(&shared_mutex::no_readers, std::ref(*this)));
    }

    inline bool shared_mutex::try_lock()
    {
      std::unique_lock<mutex_t> lk(mut_);
      if (!no_writer_no_readers())
      {
        return false;
      }
      state_ = write_entered_;
      return true;
    }

    inline void shared_mutex::unlock()
    {
      std::lock_guard<mutex_t> _(mut_);
      state_ = 0;
      // notify all since multiple *lock_shared*() calls may be able
      // to proceed in response to this notification
      gate1_.notify_all();
    }

    // Shared ownership

    inline void shared_mutex::lock_shared()
    {
      std::unique_lock<mutex_t> lk(mut_);
      gate1_.wait(lk, std::bind(&shared_mutex::no_writer_no_max_readers, std::ref(*this)));
      count_t num_readers = (state_ & n_readers_) + 1;
      state_ &= ~n_readers_;
      state_ |= num_readers;
    }

    inline bool shared_mutex::try_lock_shared()
    {
      std::unique_lock<mutex_t> lk(mut_);
      if (!no_writer_no_max_readers())
      {
        return false;
      }
      count_t num_readers = (state_ & n_readers_) + 1;
      state_ &= ~n_readers_;
      state_ |= num_readers;
      return true;
    }

    inline void shared_mutex::unlock_shared()
    {
      std::lock_guard<mutex_t> _(mut_);
      count_t num_readers = (state_ & n_readers_) - 1;
      state_ &= ~n_readers_;
      state_ |= num_readers;
      if (no_writer())
      {
        if (num_readers == n_readers_ - 1)
          gate1_.notify_one();
      }
      else
      {
        if (num_readers == 0)
          gate2_.notify_one();
      }
    }

#endif