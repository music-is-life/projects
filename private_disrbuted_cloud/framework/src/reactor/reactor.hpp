
/*********************************
 * Reviewer: 
 * Author: Mike Meyers
 * File: reactor.hpp
 * ******************************/

#pragma once

#include "imonitor.hpp"
#include "function.hpp"

#include <iostream>
#include <assert.h>
#include <unordered_map>

namespace ilrd
{
template<typename MONITOR> 
class Reactor
{
public:
  explicit Reactor(); 
 
  void RegisterReadHandler(int fd, const Function<int(void)> &func);
  void Run();
  //Void Stop();
  
  
private:
	std::unordered_map<int, Function<int(void)>> m_handlers;
  MONITOR m_monit;
};  

  /*------------------ Reactor Implementation ----------------*/
  template<typename MONITOR> 
  Reactor<MONITOR>::Reactor()
  {
    (void)static_cast<IMonitor*>(&m_monit);
  }

  template<typename MONITOR> 
  void Reactor<MONITOR>::RegisterReadHandler(int fd, const Function<int(void)> &func)
  {
    m_handlers[fd] = func;
    m_monit.SetRead(fd);
  }

  template<typename MONITOR> 
  void Reactor<MONITOR>::Run()
  {
    int n_events = 0, curr_fd = 0;

    while(1)
    {
      n_events = m_monit.WaitForEvent();
      while(n_events)
      {
        curr_fd = m_monit.GetNextReadFd();
        m_handlers[curr_fd]();
        --n_events;
      }
    }
  }


}//ilrd
