#include <sys/stat.h> /* open */
#include <fcntl.h> /* open */
#include <sys/types.h>        
#include <sys/socket.h>
#include <sys/ioctl.h> /* ioctl */
#include <assert.h> /* assert */
#include <stdio.h> /* printf */
#include <arpa/inet.h> /* ntohl */
#include <sys/types.h> /*fork */
#include <string.h> /* memcpy */
#include <unistd.h> /* fork */
#include <stddef.h> /* offsetof*/
#include <stdlib.h> /* malloc-free */
#include "nbd_comm.h"

#define BLOCK_SIZE 1024

static int read_all(int fd, char *buf, size_t count);
static int write_all(int fd, char* buf, size_t count);
static u_int64_t ntohll(u_int64_t a);
static int InitNbdNet(const char *dev_name, uint64_t size, int *sp);
static void CleanUp(int nbd, int *sp);


typedef struct WRequest
{
    NbdRequest rqst;
    int socket;
    struct nbd_reply reply;
    char buffer[1];
}Ndb_Message;

/*----------------------- Functions Definitions ---------------------------*/

int NbdDevOpen(const char* dev_name, uint64_t size)
{
    int sp[2] = {-1}, nbd, error;
    pid_t pid;


    do
    {
        nbd = InitNbdNet(dev_name, size, sp);
        if(!nbd)
        {
            break;
        }

        pid = fork();
        if(-1 == pid)
        {
            printf("fork faild\n");
            break;
        }

        if(pid == 0)
        {
            close(sp[0]);

            printf("inside child process, next is NBD_DO_IT\n");
            error = ioctl(nbd, NBD_DO_IT); /* eneters Blocking. */
            if(-1 == error)
            {
                printf("NBD_DO_IT faild.\n");
                break;   
            }
        }
        else
        {
            printf("parent process returns sp.\n");
            close(sp[1]);

            return sp[0];
        }
    } while (0);

    /* goto Only if fails */
    CleanUp(nbd, sp);
    return -1;
}

/*-------------------------------------------------------------*/

NbdRequest *NbdRequestRead(int sock)
{
    Ndb_Message *req = NULL;
    struct nbd_request request;

    read_all(sock, (char*)&request, sizeof(request));

    req = (Ndb_Message*)malloc(offsetof(Ndb_Message, buffer) + ntohl(request.len));

    if(!req)
    {
        printf("malloc faild");
        return NULL;
    }


    memcpy(req->reply.handle, request.handle, sizeof(req->reply.handle));
    req->reply.magic = htonl(NBD_REPLY_MAGIC);
    req->reply.error = htonl(0);

    req->rqst.reqType = ntohl(request.type);
    req->rqst.offset = ntohll(request.from);
    req->rqst.dataLen = ntohl(request.len);
    req->socket = sock;
    req->rqst.dataBuf = req->buffer;


    if(req->rqst.reqType == NBD_CMD_WRITE)
    {
        read_all(sock, req->buffer, req->rqst.dataLen);
    }

    return (NbdRequest*)req;
}

/*-------------------------------------------------------------*/

void NbdRequestDone(NbdRequest* req, int error)
{
    Ndb_Message *tmp = (Ndb_Message*)req;
    size_t len = sizeof(struct nbd_reply);

    tmp->reply.error = htonl(error);


    if(NBD_CMD_READ == tmp->rqst.reqType)
    {
        len += tmp->rqst.dataLen;
    }
    write_all(tmp->socket, (char*)&tmp->reply, len);
    free(req);
}

/*----------------------- Static Functions ---------------------------*/

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


static int write_all(int fd, char* buf, size_t count)
{
	int bytes_written;

	while (count > 0)
	{
		bytes_written = write(fd, buf, count);
        if(bytes_written <= 0)
        {
            return 1;
        }
    	buf += bytes_written;
    	count -= bytes_written;
	}

	return 0;
}

static int InitNbdNet(const char *dev_name, uint64_t size, int *sp)
{
    int error, nbd;

    do
    {
        nbd = open(dev_name, O_RDWR);
        if(-1 == nbd)
        {
            printf("open faild.\n");
            break;
        }
        error = socketpair(AF_UNIX, SOCK_STREAM, 0, sp);

        printf("finished open\n");
        error = ioctl(nbd, NBD_SET_SIZE, size);
        if(-1 == error)
        {
            printf("nbd_clear_sock\n");
            break;
        }
        error = ioctl(nbd, NBD_CLEAR_SOCK);
        if(-1 == error)
        {
            printf("nbd_clear_sock\n");
            break;
        }
        error = ioctl(nbd, NBD_SET_BLKSIZE, BLOCK_SIZE);
        if(-1 == error)
        {
            printf("SET_BLKSIZE faild\n");
            break;
        }
        if(
        ioctl(nbd, NBD_CLEAR_QUE) == -1 ||
        ioctl(nbd, NBD_CLEAR_SOCK) == -1)
        {
        printf("nbd_clear_que or clear_sock faild\n"); /*0fck */
        break;
        }

        if(-1 == ioctl(nbd, NBD_SET_TIMEOUT, 5))
        {
            printf("NBD_TIMEOUT faild.\n");
            break;
        }

        if(-1 == ioctl(nbd, NBD_SET_SOCK, sp[1]))
        {
            printf("NBD_SET_SOCK faild.");
            break;
        }

        if(-1 == ioctl(nbd, NBD_SET_TIMEOUT, 5))
        {
            break;
        }    

        printf("finished ioctl calls\n");

        return nbd;
    } while (0);

    printf("performing CleanUP!\n");
    CleanUp(nbd, sp);
    return -1;
}

static void CleanUp(int nbd, int *sp)
{
    if(!nbd)
    {
        return;
    }
    if(sp[0])
    {
        close(sp[0]);
    }
    if(sp[1])
    {
        close(sp[1]);
    }

    close(nbd);
}

static u_int64_t ntohll(u_int64_t a)
{
	u_int32_t lo = a & 0xffffffff;
	u_int32_t hi = a >> 32U;
	lo = ntohl(lo);
	hi = ntohl(hi);
	return ((u_int64_t) lo) << 32U | hi;
}
