#include <iostream>
#include "pthread.h"
#include <signal.h>
#include "ThreadPool.hpp"

static const int BAD_APPLE = -1;

namespace ilrd
{

/* ------------- ThreadPool Implementation ----------------*/

    ThreadPool::ThreadPool(int num_of_threads): m_isCancel(1)
    {
        int i = 0;
        for(i = 0; i < num_of_threads; ++i)
        {
            m_threads.emplace_back(std::thread(&ThreadPool::TaskDespacher, this));
        }   
    }

    ThreadPool::Future ThreadPool::Async(const Function<int(void)>& func)
    {
        std::shared_ptr<Task> sp(new Task(func));
        ThreadPool::Future future_ret(sp);

        m_tasks.Enqueue(sp);

        return(future_ret);
    }

    void ThreadPool::TaskDespacher()
    {
        std::shared_ptr<Task> sp;
        int status = 0;

        while(m_isCancel)
        {
            m_tasks.Dequeue(sp);
            status = sp->RunTask();
            if(status == BAD_APPLE)
            {
                break;
            }
            sp->setStatus(status);
            sp->PostSem();
        }
    }

    int BadApple(int num)
    {
        (void)num;
        return -1;
    }

    void ThreadPool::ShutDown()
    {
        size_t i = 0;
        Function<int(void)> bad_apple_f(BadApple, 0);
        for(i = 0; i < m_threads.size(); ++i)
        {
            Async(bad_apple_f);
        }
        std::cout << "Starting ShutDown" << std::endl;

        for(i = 0; i < m_threads.size(); ++i)
        {
            m_threads[i].join();
        }
        m_isCancel = 0;
    }

    void ThreadPool::Cancel()
    {
        std::cout << "Initiating Cancel!" << std::endl;
        m_isCancel = 0;


        m_tasks.Clear();
        ShutDown();
    }

    ThreadPool::~ThreadPool()
    {   
        size_t i = 0;
        
        if(m_isCancel)
        {
            for(i = 0; i < m_threads.size(); ++i)
            {
                pthread_kill(m_threads[i].native_handle(), SIGTERM);
            }
        }
    }
/* ------------- Task Implementation ----------------*/

    ThreadPool::Task::Task(const Function<int(void)> &func, int priority): m_function(func)
    , m_priority(priority) {}

    void ThreadPool::Task::WaitSem()
    {
        m_sem.Wait();
    }

    void ThreadPool::Task::PostSem()
    {
        m_sem.Post();
    }

    int ThreadPool::Task::getStatus()
    {
        return m_status;
    }

    void ThreadPool::Task::setStatus(int status)
    {
        m_status = status;
    }

    int ThreadPool::Task::RunTask()
    {
        return m_function();
    }

/* ------------- Future Implementation ----------------*/

    ThreadPool::Future::Future(const std::shared_ptr<Task> &s_ptr): m_ptr(s_ptr) {}

    int ThreadPool::Future::Wait() const
    {
        Task *t = m_ptr.get();
        t->WaitSem();
        return t->getStatus();
    }

    int ThreadPool::Future::getStatus()
    {
        return m_ptr->getStatus();
    }
    
} //ilrd
