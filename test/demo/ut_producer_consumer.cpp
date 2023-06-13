#include "gtest/gtest.h"
#include "producer_consumer.h"
#include <thread>
#include <unistd.h>

using namespace std;

void pro(procon<int> &pc) {
    for (int i = 0; i < 10; i++) {
        pc.produce(i + 100);
        // cout << "produce " << i + 100 << endl;
    }
}

void con2(procon<int> &pc) {
    for (int i = 0; i < 5; i++) {
        int c;
        pc.consumer(c);
        // cout << "consumer1 " << c << endl;
        usleep(1000);  //每隔1ms消费一个
    }
}

void con1(procon<int> &pc) {
    for (int i = 0; i < 5; i++) {
        int c;
        pc.consumer(c);
        // cout << "consumer2 " << c << endl;
        usleep(1000);  //每隔0.1s消费一个
    }
}

TEST(TestProCon, test_01)
{
    procon<int> pc(5);

    // pc缓冲区最大为10，因此producer生产超过10时，会等待消费者消费一个，然后再生产一个
    thread t1(pro, std::ref(pc));
    thread t2(con1, std::ref(pc));
    thread t3(con2, std::ref(pc));

    t1.join();
    t2.join();
    t3.join();

    EXPECT_EQ(pc.buffer.size(), 0);
}