#pragma once

#include <vector>    /* std::vector */
#include <thread>    /* std::thread */
#include "function.hpp"
#include "BtsQueue.hpp"
#include "semaphore.hpp"


namespace ilrd
{

class ThreadPool
{
private:
    class Compirason; 
    class Task;
public:

    class Future;
    explicit ThreadPool(int num_of_threads = 16);
    ~ThreadPool();
    Future Async(const Function<int(void)>& func);
    
    void ShutDown();    // terminate New tasks! finishes what currently in queue
    void Cancel();        // cancel all tasks and empties the queue
    
    ThreadPool(const ThreadPool& other) = delete;
    void operator=(const ThreadPool& other) = delete;

private:

class Compirason 
{
public:
    bool operator() (const Task&, const Task&)
    {
        return true;
    }
    
};
    std::vector<std::thread> m_threads;
    BtsQueue<std::shared_ptr<Task>> m_tasks;
    void TaskDespacher();
    int m_isCancel;


}; //class thread pool

class ThreadPool::Task
{
 public:
    explicit Task(const Function<int(void)> &func, int priority = 0);
    int getStatus();
    void setStatus(int status);

    int RunTask();
    void WaitSem();
    void PostSem();
  private:
    Function<int(void)> m_function;
    int m_status;
    int m_priority;
    Semaphore m_sem;
 }; //class Future

class ThreadPool::Future
{
public:
    explicit Future(const std::shared_ptr<Task> &s_ptr = nullptr);

    int Wait() const;
    int getStatus();
    
private:
    std::shared_ptr<Task> m_ptr;
};


} // namespace ilrd
