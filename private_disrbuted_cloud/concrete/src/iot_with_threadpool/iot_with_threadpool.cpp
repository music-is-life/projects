#include <sys/types.h> /* getaddrinfo */
#include <sys/socket.h> /* getaddrinfo, socket, sendmsg*/
#include <netdb.h> /* getaddrinfo */
#include <unistd.h> /* close */
#include <cstring> /* memset */

#include "ThreadPool.hpp"
#include "reactor.hpp"
#include "nbd_comm.h"
#include "select.hpp"

using namespace ilrd;

typedef struct AtlasHeader
{
    uint64_t    m_requestUid;
    uint32_t    m_alarmUid;
    uint32_t    m_iotOffset;
    uint32_t    m_type;
    uint32_t    m_len;
} AtlasHeader_t;

typedef struct info
{
    int sock;
    NbdRequest *req;
}info_t;

AtlasHeader_t ConvertToAtlas(NbdRequest *request);
NbdRequest *ConevrtToNbd(AtlasHeader_t atlas);
int nbdHandler(int nbdSock);
int AtlasRead(NbdRequest request,AtlasHeader_t atlas);
int InitSocket(struct sockaddr_in *servinfo, int connection_type, int port);
ThreadPool &GetThreadPool();

int SendMsgWrapper(info_t *params);

ThreadPool &GetThreadPool()
{
    static ThreadPool tp(16);
    return tp;
}


static int read_all(int fd, char* buf, size_t count)
{
	int bytes_read;

	while (count > 0)
	{
    	bytes_read = read(fd, buf, count);
        if(bytes_read <= 0)
        {
            return 1;
        }
    	buf += bytes_read;
    	count -= bytes_read;
	}

	return 0;
}


int iotHandler(int *sockets)
{
    AtlasHeader_t atlas;
    NbdRequest *req = NULL;

    read_all(sockets[1], reinterpret_cast<char*>(&atlas), sizeof(AtlasHeader_t));
    req = ConevrtToNbd(atlas);

    if(req->reqType == NBD_CMD_READ)
    {
        read_all(sockets[1], req->dataBuf, req->dataLen);
    }

    NbdRequestDone(req, 0);

    return 0;
}

int SendMsgWrapper(info_t *params)
{
    AtlasHeader_t atlas = ConvertToAtlas(params->req);
    struct iovec iov[2];
    struct msghdr msg;

    std::cout << "Handling Request" << std::endl;

    iov[0].iov_base = &atlas;
    iov[0].iov_len = sizeof(AtlasHeader_t);
    memset(&msg, 0, sizeof(msg));

    msg.msg_iovlen = 1;

    if(params->req->reqType == NBD_CMD_WRITE)
    {
        iov[1].iov_base = params->req->dataBuf;
        iov[1].iov_len = params->req->dataLen;
        msg.msg_iovlen = 2;
    }
    msg.msg_iov = iov;

    if(-1 == sendmsg(params->sock, &msg, 0))
    {
        delete params;
        return 1;
    }
    delete params;
    return 0; 
}

int nbdHandler(int *sockets)
{
    NbdRequest *req = NbdRequestRead(sockets[0]);
    info_t *params = new info_t;
    params->req = req;
    params->sock = sockets[1];

    Function<int(void)> func(SendMsgWrapper, params);
    ThreadPool::Future f = GetThreadPool().Async(func);

    if(f.getStatus() == 1)
    {
        std::cout << "sendmsg\n";
        return 1;
    }

    return 0;
}

int InitSocket(struct sockaddr_in *servinfo, int connection_type, int port)
{
    int sockfd;

    if( ( (sockfd = socket(AF_INET,
                        connection_type,
                        0))) == -1)
    {
        printf("socket() %d", __LINE__);
        return -1;
    }
    memset(servinfo, 0, sizeof(*servinfo));

    servinfo->sin_family = AF_INET; /* Dont care if IPV4 or 5*/
    servinfo->sin_addr.s_addr = htonl(INADDR_ANY); /* gonna use UDP so we need DGRAM */
    servinfo->sin_port = htons(port);

    return sockfd;
}

int ConnectToPort(int port)
{
    int sockfd;
    struct sockaddr_in servinfo;

    if( (sockfd = InitSocket(&servinfo, SOCK_STREAM, port)) == -1)
    {
        std::cout<< "InitSocket" <<  __LINE__ << std::endl;   
    }

    if (connect(sockfd, reinterpret_cast<struct sockaddr*>(&servinfo), sizeof(servinfo)) == -1)
    {
        close(sockfd);
        perror("client: connect");
    }

    return sockfd;
}

AtlasHeader_t ConvertToAtlas(NbdRequest *request)
{
    AtlasHeader_t hdr;
    hdr.m_requestUid = reinterpret_cast<uint64_t>(request);
    hdr.m_iotOffset = request->offset;
    hdr.m_type = request->reqType;
    hdr.m_len = request->dataLen;
    return hdr;
}

NbdRequest *ConevrtToNbd(AtlasHeader_t atlas)
{
    NbdRequest *req = reinterpret_cast<NbdRequest*>(atlas.m_requestUid);
    return req;
}

int main(void)
{
    Reactor<Select> r;
    int sockets[2];

    static const int StorageSize = 0x1000000;
    sockets[0] = NbdDevOpen("/dev/nbd0", StorageSize);
    sockets[1] = ConnectToPort(29000);

    std::cout <<  "iotfd : "<< sockets[1] << std::endl;

    Function<int(void)> f1(&nbdHandler, sockets);
    Function<int(void)> f2(&iotHandler, sockets);

    r.RegisterReadHandler(sockets[0], f1);
    r.RegisterReadHandler(sockets[1], f2);

    r.Run();

    return 0;
}
