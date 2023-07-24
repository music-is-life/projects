#include "Command.hpp"
#include <cstring> /* memset */

using namespace ilrd;

void Write::BuildMsg(std::shared_ptr<Info> thread_info)
{
    thread_info->iov[0].iov_base = &thread_info->header;
    thread_info->iov[0].iov_len = sizeof(AtlasHeader_t);
    memset(&thread_info->msg, 0, sizeof(thread_info->msg));

    thread_info->iov[1].iov_base = thread_info->nbd_req->dataBuf + 
                                    (thread_info->header.m_fragIdx -
                                    thread_info->nbd_req->offset / FRAGMENT_SIZE) *
                                    thread_info->header.m_len;
                                        
    thread_info->iov[1].iov_len = thread_info->header.m_len;
    thread_info->msg.msg_iovlen = 2;
    thread_info->msg.msg_iov = thread_info->iov;
}

void Read::BuildMsg(std::shared_ptr<Info> thread_info)
{
    thread_info->iov[0].iov_base = &thread_info->header;
    thread_info->iov[0].iov_len = sizeof(AtlasHeader_t);
    memset(&thread_info->msg, 0, sizeof(thread_info->msg));

    thread_info->msg.msg_iovlen = 1;
    thread_info->msg.msg_iov = thread_info->iov;
}

int Command::SendMsg(std::shared_ptr<Info> thread_info)
{
    if(-1 == sendmsg(thread_info->fd, &thread_info->msg, 0))
    {
        perror("sendmsg");
        return 1;
    }
    return 0; 
}
