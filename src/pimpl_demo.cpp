#include "pimpl_demo.h"
#include <vector>

class Logmgr::LogmgrImpl{
  public:
    LogmgrImpl(int max_size);
    int max_size_;
    bool status_;
    std::vector<int> vec_;
};

Logmgr::LogmgrImpl::LogmgrImpl(int max_size) {
    max_size_ = 100;
    status_ = true;
}

// 智能指针初始化
Logmgr::Logmgr(int max_size) : m_impl_(new LogmgrImpl(max_size)){}

Logmgr::~Logmgr() {}

bool Logmgr::append(int x) {
    if (m_impl_->vec_.size() >= m_impl_->max_size_) {
        return false;
    }

    m_impl_->vec_.emplace_back(x);
    return true;
}

int Logmgr::get_current_size() {
    return m_impl_->vec_.size();
}

/*******************************************************************************
*******************************************************************************/

/**
 * c通过typedef实现信息隐藏
*/

struct _logmgr {
  int max_size;
  int vec[10];
  int cnt;  
};

int init_logmgr(logmgr_t **log, int *max_size) {
    *log = (logmgr_t *)malloc(sizeof(logmgr_t));

    if (*log == NULL) {
        return -1;
    }

    (*log)->max_size = *max_size;
    (*log)->cnt = 0;
    return 0;
}

int append_log(logmgr_t *log, int *data) {
    if (log->cnt >= log->max_size) {
        return -1;
    }

    log->vec[log->cnt++] = *data;
    return 0;
}

int get_current_size(logmgr_t *log) {
    return log->cnt;
}