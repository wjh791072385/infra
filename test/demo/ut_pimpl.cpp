#include "gtest/gtest.h"
#include "pimpl_demo.h"


TEST(TestPimpl, testcpulspimpl)
{
    int max_size = 10;
    Logmgr mgr(max_size);
    EXPECT_EQ(mgr.get_current_size(), 0);
    mgr.append(3);
    EXPECT_EQ(mgr.get_current_size(), 1);
    mgr.append(100);
    EXPECT_EQ(mgr.get_current_size(), 2);
}

TEST(TestPimpl, testc)
{
    int max_size = 10;
    logmgr_t *log;
    init_logmgr(&log, &max_size);
    EXPECT_EQ(get_current_size(log), 0);

    int data = 1;
    append_log(log, &data);
    EXPECT_EQ(get_current_size(log), 1);

    data = 10;
    append_log(log, &data);
    EXPECT_EQ(get_current_size(log), 2);
}