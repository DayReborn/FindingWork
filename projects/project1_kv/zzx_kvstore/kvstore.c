#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "kvstore.h"


#define ENABLE_REACTOR 0
#define ENABLE_NTYCO 1
#define ENABLE_IOURING 2

#define NETWORK_SELECT ENABLE_IOURING


/**
 * @param msg  request message
 * @param length length of the request message
 * @param responese buffer to store the response message
 * @return  length of the response message
 */
int kvs_protocol(char *msg, int length, char *responese)
{
    printf("recv %d : %s\n", length, msg);
    memcpy(responese, msg, length);
    return strlen(responese); // return the length of the response message
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        return -1; // Invalid arguments
    }
    else
    {
    unsigned short port = (unsigned short)atoi(argv[1]);
    
    #if (NETWORK_SELECT == ENABLE_REACTOR) // Reactor-based server

    printf("Starting reactor on port %d\n", port);
    reactor_start(port, kvs_protocol);

    #elif (NETWORK_SELECT == ENABLE_NTYCO) // NtyCo-based server
    printf ("Starting NtyCo server on port %d\n", port);
    ntyco_start(port, kvs_protocol);

    #elif (NETWORK_SELECT == ENABLE_IOURING) // io_uring-based server
    printf("Starting io_uring server on port %d\n", port);
    iouring_start(port, kvs_protocol);

    #endif

    }
}