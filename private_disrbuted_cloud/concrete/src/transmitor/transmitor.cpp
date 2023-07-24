#include "transmitor.hpp"


typedef struct
{
    uint32_t    m_requestUid;
    uint32_t    m_fragIdx;

} AtlasPeek;

namespace ilrd
{
    
Dispatcher<std::shared_ptr<Info>> Transmitor::m_dispatcher;

int read_all(int fd, char* buf, size_t count, int flag = 0)
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

void Transmitor::Add(const Callback<std::shared_ptr<Info>>& callback)
{
    m_dispatcher.Add(callback);
}

int GetFirstFragIdx(NbdRequest *nbd_request)
{
    return nbd_request->offset / FRAGMENT_SIZE;
}

Transmitor::Transmitor(std::shared_ptr<Command> command): sp_command(command) {}

int Transmitor::SendIOTFragments(std::shared_ptr<Info> thread_info)
{
    for(size_t i = thread_info->header.m_fragIdx; i < thread_info->LastFragIdx; i+=IOT_COUNT)
    {
        iot_set.insert(i);
    }

    for(; thread_info->header.m_fragIdx < thread_info->LastFragIdx;
                             thread_info->header.m_fragIdx+=IOT_COUNT,
                             thread_info->header.m_iotOffset +=  FRAGMENT_SIZE)
    {
        std::cout << "-----------------Sending------------------\n";
        std::cout << "header.m_requestUid: " << thread_info->header.m_requestUid << "\n";
        std::cout << "header.m_fragIdx: " << thread_info->header.m_fragIdx << "\n";
        std::cout << "header.m_iotOffset: " << thread_info->header.m_iotOffset << "\n";
        std::cout << "header.m_len: " << thread_info->header.m_len << "\n";
        std::cout << "header.m_type: " << thread_info->header.m_type << "\n";

        Notify(thread_info);

        sp_command->BuildMsg(thread_info);

        if(1 == sp_command->SendMsg(thread_info))
        {
            std:: cout << "SendMsgWrapper faild!\n";
            return 1;
        }
    }
    return 0;
}

int Transmitor::HandleReply(int fd, NbdRequest *nbd_req)
{
    AtlasHeader_t header;
    if(1 == read_all(fd, reinterpret_cast<char*>(&header), sizeof(AtlasHeader_t)))
    {
        return 1;
    }

    std::cout << "-----------------Reciving------------------\n";
    std::cout << "header.m_requestUid: " << header.m_requestUid << "\n";
    std::cout << "header.m_fragIdx: " << header.m_fragIdx << "\n";
    std::cout << "header.m_iotOffset: " << header.m_iotOffset << "\n";
    std::cout << "header.m_len: " << header.m_len << "\n";
    std::cout << "header.m_type: " << header.m_type << "\n";

    if(header.m_type == NBD_CMD_READ)
    {
        int status = read_all(fd, nbd_req->dataBuf + 
                    (header.m_fragIdx - GetFirstFragIdx(nbd_req)) * header.m_len, 
                    header.m_len);

        if(1 == status)
        {
            std::cout << "ReadAll faild!!" << std::endl;
            return 1;
        }
    }

    iot_set.erase(header.m_fragIdx);
    if(iot_set.empty())
    {
        return DONE;
    }

    return 0;
}

uint32_t Transmitor::FragIdxPeek(int fd)
{
    AtlasPeek key;
    if(1 == read_all(fd, reinterpret_cast<char*>(&key), sizeof(uint64_t), MSG_PEEK))
    {
        std::cout << "readall faild\n";
    }
    return key.m_fragIdx;
}

void Transmitor::Notify(std::shared_ptr<Info> info)
{
    m_dispatcher.Notify(info);
}

}//namespace ilrd