#include "gtest/gtest.h"
#include "threadpool_v1.h"
#include <iostream>

using namespace std;

const int N = 10;
bool st[N];

class request{
  public:
    request(int i) : i(i) {}

    int process() {
        st[i] = true;
        // printf("i = %d\n", i);
        return 0;
    }

   int i;
};

TEST(TestThreadpool_v1, function)
{
    threadpool<request> *pool = new threadpool<request>(4, 100);

    for (int i = 0; i < 10; i++) {
        pool->append(new request(i));
    }
    
    while ( pool->is_busy() || pool->m_task_queue.size() > 0) {
        usleep(1000);
        // cout << "pool is not completed" << endl;
    }

    for (int i = 0; i < 10; i++) {
        EXPECT_TRUE(st[i] == true);
    }

    delete pool;
}