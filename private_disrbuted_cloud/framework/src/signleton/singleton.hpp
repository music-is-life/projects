#pragma once

#include <iostream>
#include <atomic>
#include <cstdlib>

#define DEAD_BEEF 0xDEADBEEF

namespace ilrd
{
template<typename T>
class Singleton
{
public:
	explicit Singleton();
    T* operator->() const;
    ~Singleton() = default;

    Singleton(const Singleton &other) = delete;		//prevent copy
    void operator=(const Singleton &) = delete;	//prevent assignment
private:
	static void CleanUp();
	static T *m_instance;
};// class Singleton

template <typename T>
T *Singleton<T>::m_instance = NULL;


template <typename T>
Singleton<T>::Singleton()
{
    static bool m_is_block = 0;
    if(NULL == m_instance)
    {
        if(!__atomic_test_and_set(&m_is_block, __ATOMIC_SEQ_CST))
        {
            __atomic_store_n(&m_instance, new T, __ATOMIC_SEQ_CST); // Doing Allocation (1), Ctor (2), Assign (3).
            atexit(CleanUp);
        }
        else
        {
            while(NULL == __atomic_load_n(&m_instance, __ATOMIC_SEQ_CST)); // This Load is unnessesery but we want the busy wait thread will exit asap. 
        }
    }
}

template <typename T>
T* Singleton<T>::operator->() const
{
    return m_instance;
}

template <typename T>
void Singleton<T>::CleanUp()
{
    delete m_instance;
    m_instance = reinterpret_cast<T*>(DEAD_BEEF);
}

}//namespace ilrd