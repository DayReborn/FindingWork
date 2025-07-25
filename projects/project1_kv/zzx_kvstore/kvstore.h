

#ifndef __KV_STORE_H__
#define __KV_STORE_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>

//===================================================
// 全局宏定义
//===================================================

#define DEBUG 
#define ENABLE_ARRAY 1      // 启用数组存储

#define ENABLE_REACTOR 0
#define ENABLE_NTYCO 1
#define ENABLE_IOURING 2

#define NETWORK_SELECT ENABLE_REACTOR

//===================================================
// KVS协议相关定义
//===================================================


enum
{
    KVS_CMD_START = 0,
    KVS_CMD_SET = KVS_CMD_START,
    KVS_CMD_GET,
    KVS_CMD_DEL,
    KVS_CMD_MOD,
    KVS_CMD_EXIST,
    KVS_CMD_COUNT,
};

extern const char *command[]; // command list

extern const char *response[]; // response list


//===================================================
// KVS数组相关定义
//===================================================

#define KVS_ARRAY_SIZE 1024 // 数组大小

#if ENABLE_ARRAY


typedef struct kvs_array_item_s
{
    char *key;
    char *value;
} kvs_array_item_t;

typedef struct kvs_array_s
{
    kvs_array_item_t *table; // 存储键值对的数组
    int total;               // 当前存储的键值对数量
    int idx;                 // 当前使用的索引
} kvs_array_t;

extern kvs_array_t global_array; // 全局KVS数组实例

void *kvs_malloc(size_t size);

void kvs_free(void *ptr);

int kvs_array_create(kvs_array_t *inst);

void kvs_array_destory(kvs_array_t *inst);

int kvs_array_set(kvs_array_t *inst, char *key, char *value);

char *kvs_array_get(kvs_array_t *inst, char *key);

int kvs_array_del(kvs_array_t *inst, char *key);

int kvs_array_mod(kvs_array_t *inst, char *key, char *value);

int kvs_array_exist(kvs_array_t *inst, char *key);

#endif

//===================================================
// Reactor相关函数声明
//===================================================

typedef int (*msg_handler)(char *msg, int length, char *responese);

extern int reactor_start(unsigned short port, msg_handler handler);

extern int ntyco_start(unsigned short port, msg_handler handler);

extern int iouring_start(unsigned short port, msg_handler handler);

//===================================================
// KVS协议相关函数声明
//===================================================

#define KVS_MAX_TOKENS 128

int kvs_split_token(char *msg, char *tokens[]);

int kvs_protocol_filter(char **tokens, int count, char *response);

int kvs_protocol(char *msg, int length, char *responese);

int init_kvengine(void);
#endif