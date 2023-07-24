/*********************************
 * Reviewer:  				
 * Author: Daniel Binyamin			
 * File: Command.cpp					
 * ******************************/


#pragma once
#include <cstdint> /* uint32_t */
#include <memory> /* shared_ptr */
#include "config.hpp"


namespace ilrd
{

class Command
{

public:
    Command(std::shared_ptr<Info> thread_info) {}
    virtual ~Command() {}
    virtual void BuildMsg(std::shared_ptr<Info> thread_info) = 0;
    virtual int SendMsg(std::shared_ptr<Info> thread_info);

};

class Write : public Command
{
public:
    Write(std::shared_ptr<Info> thread_info): Command(thread_info) {}
    void BuildMsg(std::shared_ptr<Info> thread_info);
};

class Read : public Command
{

public:
    Read(std::shared_ptr<Info> thread_info): Command(thread_info) {}
    void BuildMsg(std::shared_ptr<Info> thread_info);
};


} // namespace ilrd