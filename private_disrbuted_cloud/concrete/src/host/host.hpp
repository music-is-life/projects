#pragma once

#include <set>
#include <vector>
#include "nbd_comm.h"
#include "Command.hpp"
#include "config.hpp"
#include "transmitor.hpp"


namespace ilrd
{


class RequestHost
{
public:
/*     RequestHost();
 */
	static std::shared_ptr<RequestHost> Create(int *sockets);
    void Distributor(int *fd);
    int HandleReply(int fd);
    static uint32_t Peek(int fd);

    uint32_t GetRequestUid();
private:
    RequestHost() {}
    NbdRequest *nbd_req;
    std::shared_ptr<Command> sp_command;
    std::unordered_map<uint32_t ,std::shared_ptr<Transmitor>> m_iot_transmition;
    uint32_t m_requestUid;
};

}// namespace ilrd