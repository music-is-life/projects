#include <sys/types.h> /* getaddrinfo */
#include <sys/socket.h> /* getaddrinfo, socket, sendmsg*/
#include <netdb.h> /* getaddrinfo */
#include <unistd.h> /* close */
#include <cstring> /* memset */

#include "dispacher.hpp"
#include "reactor.hpp"
#include "select.hpp"

using namespace ilrd;

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
        std::cout<< "InitSocket faild" <<  __LINE__ << std::endl;   
    }

    if (connect(sockfd, reinterpret_cast<struct sockaddr*>(&servinfo), sizeof(servinfo)) == -1)
    {
        close(sockfd);
        perror("connect");
    }

    return sockfd;
}


int main(void)
{
    Reactor<Select> r;
    int sockets[IOT_COUNT + 1];

    static const int StorageSize = 0x1000000;
    sockets[0] = NbdDevOpen("/dev/nbd0", StorageSize);

    RequestDispatcher rd;

    // Registering NBD to reactor.
    Function<int(void)> f1(&RequestDispatcher::nbdHandler, &rd, sockets);
    r.RegisterReadHandler(sockets[0], f1);


    // Registering IOT's to reactor.
    for(size_t i = 0; i < IOT_COUNT; ++i)
    {
        sockets[i + 1] = ConnectToPort(29000 + i);
        r.RegisterReadHandler(sockets[i + 1], Function<int(void)>(&RequestDispatcher::iotHandler, &rd,sockets[i + 1]));
    }


    r.Run();

    return 0;
}
