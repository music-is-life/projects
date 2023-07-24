#include <iostream> /* std::cout, definitions */
#include <netdb.h> /* getaddrinfo */
#include <unistd.h> /* close */
#include <cstring> /* memset */

#include "function.hpp"
#include "singleton.hpp"
#include "ThreadPool.hpp"
#include "Command.hpp"
#include "host.hpp"
#include "factory.hpp"
#include "config.hpp"

namespace ilrd
{

/*----------------------- Static Functions ---------------------------*/

static int read_all(int fd, char* buf, size_t count, int flag = 0);
static int GetFirstFragIdx(NbdRequest *nbd_request);
static int FragIdxToIOT(uint32_t frag_idx);

/*----------------------- RequestHost Functions ---------------------------*/



std::shared_ptr<RequestHost> RequestHost::Create(int *sockets)
{
    static uint32_t requestUid = 0;
    RequestHost *req_host = new RequestHost;
    ++requestUid;
    req_host->nbd_req = NbdRequestRead(sockets[0]);

    if(!req_host->nbd_req)
    {
        return NULL;
    }

    req_host->m_requestUid = requestUid;
    req_host->Distributor(sockets);

    return std::shared_ptr<RequestHost>(req_host);
}

uint32_t RequestHost::GetRequestUid()
{
    return m_requestUid;
}

void RequestHost::Distributor(int *fd)
{
    size_t i = 0, k = 0;

    size_t fragment_count = (nbd_req->dataLen / FRAGMENT_SIZE);
    size_t first_frag_idx = GetFirstFragIdx(nbd_req);
    size_t final_fragment_idx = fragment_count + first_frag_idx;

    size_t stopCondition = (fragment_count < IOT_COUNT) ? fragment_count : IOT_COUNT;

    Singleton<ThreadPool> tp;
    Singleton<Factory<Command, int, std::shared_ptr<Info>>> factory;
    factory->Add<Write>(NBD_CMD_WRITE);
    factory->Add<Read>(NBD_CMD_READ);


    std::shared_ptr<Info> sp_params[IOT_COUNT];
    sp_command = factory->Create(nbd_req->reqType, sp_params[0]);

    for(k = 0, i = first_frag_idx % IOT_COUNT; k < stopCondition; i = (i + 1) % IOT_COUNT, ++k) // Running on all IOT's
    {
        std::shared_ptr<Transmitor> transmitor(new Transmitor(sp_command));
        m_iot_transmition[i] = transmitor;

        sp_params[k].reset(new Info);

        sp_params[k]->fd = fd[i+1];
        sp_params[k]->LastFragIdx = final_fragment_idx;
        sp_params[k]->nbd_req = nbd_req;

        sp_params[k]->header.m_type = nbd_req->reqType;
        sp_params[k]->header.m_len = FRAGMENT_SIZE;
        sp_params[k]->header.m_requestUid = m_requestUid;
        sp_params[k]->header.m_fragIdx = first_frag_idx + k;
        sp_params[k]->header.m_iotOffset = sp_params[k]->header.m_fragIdx / IOT_COUNT * FRAGMENT_SIZE;

        Function<int(void)> func(&Transmitor::SendIOTFragments, transmitor.get(), sp_params[k]);
        tp->Async(func);
    }
}

int RequestHost::HandleReply(int fd)
{
    int currIot = FragIdxToIOT(Transmitor::FragIdxPeek(fd));
    int status = m_iot_transmition[currIot]->HandleReply(fd, nbd_req);

    if(status == DONE)
    {
        m_iot_transmition.erase(currIot);

        if(m_iot_transmition.empty())
        {
        NbdRequestDone(nbd_req, 0);
        return DONE;
        }
    }

    return 0;
}

uint32_t RequestHost::Peek(int fd)
{
    uint32_t key;
    if(1 == read_all(fd, reinterpret_cast<char*>(&key), sizeof(uint32_t), MSG_PEEK))
    {
        std::cout << "readall faild\n";
    }
    return key;
}

int GetFirstFragIdx(NbdRequest *nbd_request)
{
    return nbd_request->offset / FRAGMENT_SIZE;
}

int FragIdxToIOT(uint32_t frag_idx)
{
    return frag_idx % IOT_COUNT;
}

int read_all(int fd, char* buf, size_t count, int flag)
{
	int bytes_read;

	while (count > 0)
	{
    	bytes_read = recv(fd, buf, count, flag);
        if(bytes_read <= 0)
        {
            return 1;
        }
    	buf += bytes_read;
    	count -= bytes_read;
	}

	return 0;
}

}//namespace ilrd