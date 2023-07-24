#include "reactor.hpp"
#include "nbd_comm.h"
#include "select.hpp"
#include <cstring>

using namespace ilrd;

char *storage;

int Handle(int nbdSock)
{
    NbdRequest* req = NbdRequestRead(nbdSock);

    std::cout << "Handling Request" << std::endl;
    switch (req->reqType)
    {
    case NBD_CMD_READ:
        memcpy(req->dataBuf, storage + req->offset, req->dataLen);
        break;
    
    case NBD_CMD_WRITE:
        memcpy(storage + req->offset, req->dataBuf, req->dataLen);
        break;
    
    default:
        break;
    }

    NbdRequestDone(req ,0);

    return 0;
}


int main(void)
{
    Reactor<Select> r;

    static const int StorageSize = 0x1000000;
    storage = new char[StorageSize];
    int nbdSock = NbdDevOpen("/dev/nbd0", StorageSize);

    Function<int(void)> f1(&Handle, nbdSock);

    r.RegisterReadHandler(nbdSock, f1);

    r.Run();

    return 0;
}