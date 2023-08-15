#include <memory>

/**
 * c++通过Pimpl机制实现信息隐藏
*/

class Logmgr{
  public:
    Logmgr(int max_size);
    ~Logmgr();
    bool append(int x);
    int get_current_size();

  private:
    class LogmgrImpl;
    std::unique_ptr<LogmgrImpl> m_impl_;
};


/**
 * c通过typedef实现信息隐藏
*/

typedef struct _logmgr logmgr_t;

int init_logmgr(logmgr_t **log, int *max_size);

int append_log(logmgr_t *log, int *data);

int get_current_size(logmgr_t *log);



