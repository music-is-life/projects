
#pragma once

#include <stdint.h> /* uint32_t */
#include <sys/types.h> /* getaddrinfo */
#include <sys/socket.h> /* getaddrinfo, socket, sendmsg*/
#include <iostream>
#include "nbd_comm.h"

namespace ilrd
{

enum {DONE = -1};
static uint32_t const IOT_COUNT = 7;
static const uint32_t  FRAGMENT_SIZE = 1024;

typedef struct AtlasHeader
{
    uint32_t    m_requestUid;
    uint32_t    m_fragIdx;
    uint32_t    m_alarmUid;
    uint32_t    m_iotOffset;
    uint32_t    m_type;
    uint32_t    m_len;
} AtlasHeader_t;

typedef struct info Info;

struct info
{
    int fd;
    size_t LastFragIdx;
    AtlasHeader_t header;
    NbdRequest *nbd_req;
    struct iovec iov[2];
    struct msghdr msg;
};




}