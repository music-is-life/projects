/*********************************
 * Reviewer: 
 * Author: Connor Mcgregor
 * File: PriQueue.hpp
 * ******************************/

#pragma once
#include <queue>	/*std::priority_queue*/
#include <condition_variable>
#include <mutex>
#include <vector>
#include <functional>

namespace ilrd
{

template<typename T, typename COMPARE = std::less<T> >
class BtsQueue
{
public:
    BtsQueue () {};
    void Enqueue(const T& task);
    void Dequeue(T& task);
	void Clear();
  
    BtsQueue(const BtsQueue& other) = delete;
    void operator=(const BtsQueue& other) = delete;
private:
    /* non_copyable(non_copyable const &); */

	std::priority_queue<T, std::vector<T>, COMPARE> m_priqueue;
	std::mutex m_lock_dequeue;
    std::condition_variable cond;

}; // class PriQueue



/* ---------------------- BtsQueue Implementations ----------------------*/


template<typename T, typename COMPARE >
void BtsQueue<T, COMPARE>::Enqueue(const T& task)
{
    std::unique_lock<std::mutex> mlock(m_lock_dequeue);
    m_priqueue.push(task);
    mlock.unlock();
    cond.notify_one();
}

template<typename T, typename COMPARE >
void BtsQueue<T, COMPARE>::Dequeue(T& task)
{
    std::unique_lock<std::mutex> mlock(m_lock_dequeue);

    while(m_priqueue.empty())
    {
        cond.wait(mlock);
    }
    task = m_priqueue.top();
    m_priqueue.pop();
}

template<typename T, typename COMPARE >
void BtsQueue<T, COMPARE>::Clear()
{
    std::unique_lock<std::mutex> mlock(m_lock_dequeue);

    while(!m_priqueue.empty())
    {
        m_priqueue.pop();
    }
}


} // ilrd