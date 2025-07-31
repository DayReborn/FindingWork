

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

#define DEBUG //printf

#define ENABLE_ARRAY 1  // 启用数组存储
#define ENABLE_RBTREE 1 // 启用红黑树存储
#define ENABLE_HASH 1   // 启用哈希表存储

#define ENABLE_REACTOR 0
#define ENABLE_NTYCO 1
#define ENABLE_IOURING 2

#define NETWORK_SELECT ENABLE_REACTOR

//===================================================
// KVS协议相关定义
//===================================================

extern const char *command[]; // command list

extern const char *response[]; // response list

//===================================================
// KVS协议相关函数声明
//===================================================

#define KVS_MAX_TOKENS 128

int kvs_split_token(char *msg, char *tokens[]);

int kvs_protocol_filter(char **tokens, int count, char *response);

int kvs_protocol(char *msg, int length, char *responese);

int init_kvengine(void);

//===================================================
// KVS数组array相关定义
//===================================================

#define KVS_ARRAY_SIZE 1024 // 数组大小

#if ENABLE_ARRAY

// KVS数组项结构体定义
typedef struct kvs_array_item_s
{
    char *key;
    char *value;
} kvs_array_item_t;

// KVS数组结构体定义
typedef struct kvs_array_s
{
    kvs_array_item_t *table; // 存储键值对的数组
    int total;               // 当前存储的键值对数量
    int idx;                 // 当前使用的索引
} kvs_array_t;

// 全局KVS数组实例
extern kvs_array_t global_array; // 全局KVS数组实例

// 函数声明
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
// KVS红黑树相关定义
//===================================================
#if ENABLE_RBTREE

// 红黑树节点颜色定义
#define RED 1
#define BLACK 2

// 是否启用键为字符类型
#define ENABLE_KEY_CHAR 1

// 定义键类型
#if ENABLE_KEY_CHAR
typedef char *KEY_TYPE;
#else
typedef int KEY_TYPE; // key
#endif

// 红黑树节点结构体定义
typedef struct _rbtree_node
{
    unsigned char color;
    struct _rbtree_node *right;
    struct _rbtree_node *left;
    struct _rbtree_node *parent;
    KEY_TYPE key;
    void *value; // 值可以是任意类型
} rbtree_node;

// 红黑树结构体定义
typedef struct _rbtree
{
    rbtree_node *root;
    rbtree_node *nil;
} rbtree;

// KVS红黑树实例类型定义
typedef struct _rbtree kvs_rbtree_t;

// 全局KVS红黑树实例
extern kvs_rbtree_t global_rbtree; // 全局KVS红黑树实例

// 函数声明
int kvs_rbtree_create(kvs_rbtree_t *inst);

void kvs_rbtree_destory(kvs_rbtree_t *inst);

int kvs_rbtree_set(kvs_rbtree_t *inst, char *key, char *value);

char *kvs_rbtree_get(kvs_rbtree_t *inst, char *key);

int kvs_rbtree_del(kvs_rbtree_t *inst, char *key);

int kvs_rbtree_mod(kvs_rbtree_t *inst, char *key, char *value);

int kvs_rbtree_exist(kvs_rbtree_t *inst, char *key);

#endif

//===================================================
// KVS哈希表相关定义
//===================================================
#if ENABLE_HASH

// 最大键长度和最大值长度
#define MAX_KEY_LEN 128
#define MAX_VALUE_LEN 512
#define MAX_TABLE_SIZE 102400

// 是否启用键指针
#define ENABLE_KEY_POINTER 1

// 哈希节点结构体定义
typedef struct hashnode_s
{
#if ENABLE_KEY_POINTER
    char *key;
    char *value;
#else
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
#endif
    struct hashnode_s *next;

} hashnode_t;

// 哈希表结构体定义
typedef struct hashtable_s
{

    hashnode_t **nodes; //* change **,

    int max_slots;
    int count;

} hashtable_t;

// KVS哈希表实例类型定义
typedef struct hashtable_s kvs_hash_t;

// 全局KVS哈希表实例
extern kvs_hash_t global_hash; // 全局KVS哈希表实例

int kvs_hash_create(kvs_hash_t *hash);

void kvs_hash_destory(kvs_hash_t *hash);

int kvs_hash_set(hashtable_t *hash, char *key, char *value);

char *kvs_hash_get(kvs_hash_t *hash, char *key);

int kvs_hash_mod(kvs_hash_t *hash, char *key, char *value);

int kvs_hash_del(kvs_hash_t *hash, char *key);

int kvs_hash_exist(kvs_hash_t *hash, char *key);

#endif

//===================================================
// Reactor相关函数声明
//===================================================

typedef int (*msg_handler)(char *msg, int length, char *responese);

extern int reactor_start(unsigned short port, msg_handler handler);

extern int ntyco_start(unsigned short port, msg_handler handler);

extern int iouring_start(unsigned short port, msg_handler handler);

#endif