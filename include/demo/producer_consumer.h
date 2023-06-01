#ifndef __PRODUCER_CONSUMER_H__
#define __PRODUCER_CONSUMER_H__

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <queue>

template<typename T>
class procon {
  public:
    procon(int max_size = 100) : max_size(max_size) {}
    ~procon(){}

    void produce(const T& t);

    void consumer(T& t);

  public:
    std::mutex mtx;
    std::condition_variable is_full;
    std::condition_variable is_empty;
    std::queue<T> buffer;
    unsigned int max_size;
};

template<typename T>
void procon<T>::produce(const T &t) {
    std::unique_lock<std::mutex> lock(mtx);
    is_full.wait(lock, [this]{ return this->buffer.size() < this->max_size; });

    //produce
    buffer.push(t);
    is_empty.notify_one();
}

template<typename T>
void procon<T>::consumer(T &t) {
    std::unique_lock<std::mutex> lock(mtx);
    is_empty.wait(lock, [this]{ return !this->buffer.empty(); });

    // consumer
    t = buffer.front();
    buffer.pop();

    is_full.notify_one();
}

#endif