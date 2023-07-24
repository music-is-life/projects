#include "semaphore.hpp"

namespace ilrd
{


Semaphore::Semaphore (int count_): m_count(count_) {}

void Semaphore::Post()
{
    std::unique_lock<std::mutex> mlock(m_mtx);
    ++m_count;
    m_cv.notify_one();
}

void Semaphore::Wait()
{
    std::unique_lock<std::mutex> mlock(m_mtx);
    while(!m_count)
    {
        m_cv.wait(mlock);
    }
    --m_count;
}

} //ilrd