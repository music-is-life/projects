#include <mutex>
#include <condition_variable>

namespace ilrd
{


class Semaphore {
public:
    Semaphore (int count_ = 0);
    void Post();
    void Wait();

private:
    std::mutex m_mtx;
    std::condition_variable m_cv;
    int m_count;
};

} //ilrd
