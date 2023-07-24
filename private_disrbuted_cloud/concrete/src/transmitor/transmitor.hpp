#pragma once

#include <set>
#include <memory>
#include "Command.hpp"
#include "observer.hpp"
#include "config.hpp"

namespace ilrd
{

class Transmitor
{
public:
    Transmitor(std::shared_ptr<Command> command);
    int SendIOTFragments(std::shared_ptr<Info> thread_info);
    int HandleReply(int fd, NbdRequest *nbd_req);
    static void Add(const Callback<std::shared_ptr<Info>>& callback);
    static void Notify(std::shared_ptr<Info> info);

    static uint32_t FragIdxPeek(int fd);

    
private:
    std::set<int> iot_set;
    static Dispatcher<std::shared_ptr<Info>> m_dispatcher;
    std::shared_ptr<Command> sp_command;
};

} //namespace ilrd