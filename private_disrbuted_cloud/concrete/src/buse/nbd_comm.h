
#pragma once

#include <stdint.h>
#include <linux/nbd.h>

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct
{
    uint32_t reqType;
    uint32_t dataLen;
    uint64_t offset;
    char *dataBuf;
}NbdRequest;


int NbdDevOpen(const char* dev_name, uint64_t size);

NbdRequest *NbdRequestRead(int sock);

void NbdRequestDone(NbdRequest* req, int error);

#ifdef __cplusplus
} // extern "C"
#endif

