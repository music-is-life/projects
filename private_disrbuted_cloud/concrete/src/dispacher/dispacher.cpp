#include "dispacher.hpp"

namespace ilrd
{

int RequestDispatcher::nbdHandler(int *sockets)
{
    std::shared_ptr<RequestHost> sp = RequestHost::Create(sockets);
    m_activeRequests.insert(std::make_pair(sp->GetRequestUid(), sp));

    return 0;
}

int RequestDispatcher::iotHandler(int socket)
{
    uint32_t key = RequestHost::Peek(socket);
    int status = m_activeRequests.at(key)->HandleReply(socket);

    if(DONE == status) //-1 Case request has finished.
    {
        m_activeRequests.erase(key);
    }
    else if(status == 1)
    {
        return 1;
    }

    return 0;
}
} //namespace ilrd
