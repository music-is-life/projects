#pragma once

#include <iostream>
#include <unordered_map> /* unoredered_map */
#include <memory> /* shared_ptr */
#include "host.hpp" 

namespace ilrd
{

class RequestDispatcher
{
public:
    int nbdHandler(int *socket);
    int iotHandler(int socket);
	
private:
	std::unordered_map< uint32_t, std::shared_ptr<RequestHost>> m_activeRequests;
};
} //namespace ilrd