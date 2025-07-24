

#ifndef __KV_STORE_H__
#define __KV_STORE_H__

typedef int (*msg_handler)(char *msg, int length, char *responese);

extern int reactor_start(unsigned short port, msg_handler handler);
extern int ntyco_start(unsigned short port, msg_handler handler);
extern int iouring_start(unsigned short port, msg_handler handler);

const char *command[] = {
    "SET", "GET", "DEL", "MOD", "EXIST"}; // command list

const char *response[] = { 

}; // response list

#endif