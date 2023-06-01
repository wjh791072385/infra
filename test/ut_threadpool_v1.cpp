#include "gtest/gtest.h"
#include "threadpool_v1.h"
#include <iostream>

using namespace std;

class request{
  public:
    request(int i) : i(i) {}

    int process() {
        // sleep(1);
        // printf("request handle %d\n", i);
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
    
    while (!pool->is_completed()) {
        usleep(1000);
    }
}