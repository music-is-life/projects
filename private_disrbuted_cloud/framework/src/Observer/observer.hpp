/*********************************
 * Reviewer:  				
 * Author: Daniel Binyamin			
 * File: Observer.cpp					
 * ******************************/

#pragma once
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <set>
#include <iostream>

#include "function.hpp"

namespace ilrd
{

template<typename RET>
class Callback;


template<typename EVENT>
class Dispatcher
{

public:
    void Add(const Callback<EVENT>& callback);
    void Remove(const Callback<EVENT>& callback);
    void Notify(EVENT event); 
    
private:
    std::set<const Callback<EVENT>*> m_callbacks;
};

template<typename EVENT>
class Callback
{
public:
    Callback(const Function<void(EVENT)>& func);
    ~Callback();
private:
    friend class Dispatcher<EVENT>;
    Function<void(EVENT)> m_function;
    mutable Dispatcher<EVENT> *m_dispatcher;
};

/* ------------------ Dispatcher Implementation -----------------*/

template<typename EVENT>
void Dispatcher<EVENT>::Add(const Callback<EVENT>& callback)
{
    m_callbacks.insert(&callback);
    callback.m_dispatcher = this;
}

template<typename EVENT>
void Dispatcher<EVENT>::Remove(const Callback<EVENT>& callback)
{
    m_callbacks.erase(&callback);
}


template<typename EVENT>
void Dispatcher<EVENT>::Notify(EVENT event)
{
    typename std::set<const Callback<EVENT> *>::const_iterator iter;

    for(iter = m_callbacks.begin(); iter != m_callbacks.end(); ++iter)
    {
        (*iter)->m_function(event);
    }
}


/* ------------------ Callback Implementation -----------------*/

template<typename EVENT>
Callback<EVENT>::Callback(const Function<void(EVENT)>& func): m_function(func) {}

template<typename EVENT>
Callback<EVENT>::~Callback()
{
    if(m_dispatcher)
    {
        m_dispatcher->Remove(*this);
    }
}

} //namespace ilrd