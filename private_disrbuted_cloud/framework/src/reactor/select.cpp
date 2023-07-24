#include <sys/select.h> /* fd_set */
#include <sys/epoll.h> /* epoll */
#include <iostream>
#include "select.hpp"

namespace ilrd
{

    /* ------------------- Select Implementation ---------------------*/

    Select::Select(): m_maxfd(0)
    {
        FD_ZERO(&m_master);
        FD_ZERO(&m_read_set);
    }

    void Select::SetRead(int fd)
    {
        FD_SET(fd, &m_master);
        if(fd > m_maxfd)
        {
            m_maxfd = fd;
        }
    }

    void Select::ClearRead(int fd)
    {
        FD_CLR(fd, &m_master);
    }

    int Select::WaitForEvent()
    {
        m_read_set = m_master;
        return select(m_maxfd+1, &m_read_set, NULL, NULL, NULL);
    }

    int Select::GetNextReadFd()
    {
        int i = 0;

        for(i = 0; i <= m_maxfd; ++i)
        {
            if(FD_ISSET(i, &m_read_set))
            {
                FD_CLR(i, &m_read_set);
                return i;
            }
        }
        return 0;
    }

    /* ------------------- Epoll Implementation ---------------------*/


}
