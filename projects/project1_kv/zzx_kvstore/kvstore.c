#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "kvstore.h"

enum
{
    KVS_CMD_START = 0,
    // array命令
    KVS_CMD_SET = KVS_CMD_START,
    KVS_CMD_GET,
    KVS_CMD_DEL,
    KVS_CMD_MOD,
    KVS_CMD_EXIST,
    // rbtree命令
    KVS_CMD_RSET,
    KVS_CMD_RGET,
    KVS_CMD_RDEL,
    KVS_CMD_RMOD,
    KVS_CMD_REXIST,
    // hash命令
    KVS_CMD_HSET,
    KVS_CMD_HGET,
    KVS_CMD_HDEL,
    KVS_CMD_HMOD,
    KVS_CMD_HEXIST,
    // 命令数量
    KVS_CMD_COUNT,
};

const char *command[] = {
    "SET", "GET", "DEL", "MOD", "EXIST",
    "RSET", "RGET", "RDEL", "RMOD", "REXIST",
    "HSET", "HGET", "HDEL", "HMOD", "HEXIST"
}; // command list

const char *response[] = {

}; // response list

/**
 * @brief 将输入字符串按空格分割成多个token（子字符串）
 * @param msg 待分割的输入字符串（会被修改，内部添加'\0'）
 * @param tokens 用于存储分割结果的指针数组（调用者需保证足够大小）
 * @return int 成功返回找到的token数量，失败返回-1
 * @note 该函数会修改原始msg内容（用'\0'替换空格）
 * @note 连续多个空格会被视为单个分隔符
 */
int kvs_split_token(char *msg, char *tokens[])
{
    if (msg == NULL || tokens == NULL)
    {
        DEBUG("Invalid arguments to kvs_split_token\n");
        return -1;
    }
    int idx = 0;
    char *token = strtok(msg, " ");
    while (token != NULL)
    {
        DEBUG("idx: %d, %s\n", idx, token);
        tokens[idx++] = token;
        token = strtok(NULL, " ");
    }
    return idx;
}

/**
 * @brief KVS协议过滤器，检查并处理客户端请求
 * @param tokens 分割后的命令token数组
 * @param count token数量
 * @param response 用于存储响应消息的缓冲区
 * @return int 成功返回响应消息长度，失败返回负数
 * @note 如果tokens或response为NULL，或count小于1，则返回-1
 * @note 如果命令格式不匹配，则返回-2
 */
int kvs_protocol_filter(char **tokens, int count, char *response)
{
    if (tokens == NULL || count < 1 || response == NULL)
    {
        DEBUG("Invalid arguments to kvs_protocol_filter\n");
        return -1;
    }

    int ret = 0;
    int length = 0;

    int cmd = KVS_CMD_START;
    for (cmd = KVS_CMD_START; cmd < KVS_CMD_COUNT; cmd++)
    {
        if (strcmp(tokens[0], command[cmd]) == 0)
        {
            DEBUG("Command matched: %s\n", command[cmd]);
            break; // 找到匹配的命令
        }
    }
    switch (cmd)
    {
    //================================================================
    // array命令
    //================================================================
    case KVS_CMD_SET:
        if (count != 3)
        {
            DEBUG("Invalid SET command format, expected 3 tokens, got %d\n", count);
            return -2; // 错误的命令格式
        }
        ret = kvs_array_set(&global_array, tokens[1], tokens[2]);
        if (ret < 0)
        {
            length = sprintf(response, "ERROR\r\n");
        }
        else if (ret == 1)
        {
            length = sprintf(response, "EXIST\r\n");
        }
        else if (ret == 2)
        {
            length = sprintf(response, "UPDATED\r\n");
        }
        else
        {
            length = sprintf(response, "OK\r\n");
        }
        break;

    case KVS_CMD_GET:
    {
        if (count != 2)
        {
            DEBUG("Invalid GET command format, expected 2 tokens, got %d\n", count);
            return -2; // 错误的命令格式
        }
        char *result = kvs_array_get(&global_array, tokens[1]);
        if (result == NULL)
        {
            length = sprintf(response, "NOT FOUND\r\n");
        }
        else
        {
            length = sprintf(response, "%s\r\n", result);
        }
        break;
    }

    case KVS_CMD_DEL:
        if (count != 2)
        {
            DEBUG("Invalid DEL command format, expected 2 tokens, got %d\n", count);
            return -2; // 错误的命令格式
        }
        ret = kvs_array_del(&global_array, tokens[1]);
        if (ret < 0)
        {
            length = sprintf(response, "ERROR\r\n");
        }
        else if (ret == 1)
        {
            length = sprintf(response, "NOT FOUND\r\n");
        }
        else
        {
            length = sprintf(response, "OK\r\n");
        }
        break;

    case KVS_CMD_MOD:
        if (count != 3)
        {
            DEBUG("Invalid MOD command format, expected 3 tokens, got %d\n", count);
            return -2; // 错误的命令格式
        }
        ret = kvs_array_mod(&global_array, tokens[1], tokens[2]);
        if (ret < 0)
        {
            length = sprintf(response, "ERROR\r\n");
        }
        else if (ret == 1)
        {
            length = sprintf(response, "NOT FOUND\r\n");
        }
        else
        {
            length = sprintf(response, "OK\r\n");
        }
        break;

    case KVS_CMD_EXIST:
        if (count != 2)
        {
            DEBUG("Invalid EXIST command format, expected 2 tokens, got %d\n", count);
            return -2; // 错误的命令格式
        }
        ret = kvs_array_exist(&global_array, tokens[1]);
        if (ret < 0)
        {
            length = sprintf(response, "ERROR\r\n");
        }
        else if (ret == 0)
        {
            length = sprintf(response, "EXIST\r\n");
        }
        else
        {
            length = sprintf(response, "NOT EXIST\r\n");
        }
        break;
    //================================================================
    // rbtree命令
    //================================================================
    case KVS_CMD_RSET:
        if (count != 3)
        {
            DEBUG("Invalid RSET command format, expected 3 tokens, got %d\n", count);
            return -2; // 错误的命令格式
        }
        // ret = kvs_array_set(&global_array, tokens[1], tokens[2]);
        ret = kvs_rbtree_set(&global_rbtree, tokens[1], tokens[2]);
        if (ret < 0)
        {
            length = sprintf(response, "ERROR\r\n");
        }
        else if (ret == 1)
        {
            length = sprintf(response, "EXIST\r\n");
        }
        else if (ret == 2)
        {
            length = sprintf(response, "UPDATED\r\n");
        }
        else
        {
            length = sprintf(response, "OK\r\n");
        }
        break;

    case KVS_CMD_RGET:
    {
        if (count != 2)
        {
            DEBUG("Invalid RGET command format, expected 2 tokens, got %d\n", count);
            return -2; // 错误的命令格式
        }
        char *result = kvs_rbtree_get(&global_rbtree, tokens[1]);
        if (result == NULL)
        {
            length = sprintf(response, "NOT FOUND\r\n");
        }
        else
        {
            length = sprintf(response, "%s\r\n", result);
        }
        break;
    }

    case KVS_CMD_RDEL:
        if (count != 2)
        {
            DEBUG("Invalid RDEL command format, expected 2 tokens, got %d\n", count);
            return -2; // 错误的命令格式
        }
        ret = kvs_rbtree_del(&global_rbtree, tokens[1]);
        if (ret < 0)
        {
            length = sprintf(response, "ERROR\r\n");
        }
        else if (ret == 1)
        {
            length = sprintf(response, "NOT FOUND\r\n");
        }
        else
        {
            length = sprintf(response, "OK\r\n");
        }
        break;

    case KVS_CMD_RMOD:
        if (count != 3)
        {
            DEBUG("Invalid RMOD command format, expected 3 tokens, got %d\n", count);
            return -2; // 错误的命令格式
        }
        ret = kvs_rbtree_mod(&global_rbtree, tokens[1], tokens[2]);
        if (ret < 0)
        {
            length = sprintf(response, "ERROR\r\n");
        }
        else if (ret == 1)
        {
            length = sprintf(response, "NOT FOUND\r\n");
        }
        else
        {
            length = sprintf(response, "OK\r\n");
        }
        break;

    case KVS_CMD_REXIST:
        if (count != 2)
        {
            DEBUG("Invalid REXIST command format, expected 2 tokens, got %d\n", count);
            return -2; // 错误的命令格式
        }
        ret = kvs_rbtree_exist(&global_rbtree, tokens[1]);
        if (ret < 0)
        {
            length = sprintf(response, "ERROR\r\n");
        }
        else if (ret == 0)
        {
            length = sprintf(response, "EXIST\r\n");
        }
        else
        {
            length = sprintf(response, "NOT EXIST\r\n");
        }
        break;
    //================================================================
    // hash命令
    //================================================================
    case KVS_CMD_HSET:
        if (count != 3)
        {
            DEBUG("Invalid HSET command format, expected 3 tokens, got %d\n", count);
            return -2; // 错误的命令格式
        }
        // ret = kvs_array_set(&global_array, tokens[1], tokens[2]);
        ret = kvs_hash_set(&global_hash, tokens[1], tokens[2]);
        if (ret < 0)
        {
            length = sprintf(response, "ERROR\r\n");
        }
        else if (ret == 1)
        {
            length = sprintf(response, "EXIST\r\n");
        }
        else if (ret == 2)
        {
            length = sprintf(response, "UPDATED\r\n");
        }
        else
        {
            length = sprintf(response, "OK\r\n");
        }
        break;

    case KVS_CMD_HGET:
    {
        if (count != 2)
        {
            DEBUG("Invalid HGET command format, expected 2 tokens, got %d\n", count);
            return -2; // 错误的命令格式
        }
        char *result = kvs_hash_get(&global_hash, tokens[1]);
        if (result == NULL)
        {
            length = sprintf(response, "NOT FOUND\r\n");
        }
        else
        {
            length = sprintf(response, "%s\r\n", result);
        }
        break;
    }

    case KVS_CMD_HDEL:
        if (count != 2)
        {
            DEBUG("Invalid HDEL command format, expected 2 tokens, got %d\n", count);
            return -2; // 错误的命令格式
        }
        ret = kvs_hash_del(&global_hash, tokens[1]);
        if (ret < 0)
        {
            length = sprintf(response, "ERROR\r\n");
        }
        else if (ret == 1)
        {
            length = sprintf(response, "NOT FOUND\r\n");
        }
        else
        {
            length = sprintf(response, "OK\r\n");
        }
        break;

    case KVS_CMD_HMOD:
        if (count != 3)
        {
            DEBUG("Invalid HMOD command format, expected 3 tokens, got %d\n", count);
            return -2; // 错误的命令格式
        }
        ret = kvs_hash_mod(&global_hash, tokens[1], tokens[2]);
        if (ret < 0)
        {
            length = sprintf(response, "ERROR\r\n");
        }
        else if (ret == 1)
        {
            length = sprintf(response, "NOT FOUND\r\n");
        }
        else
        {
            length = sprintf(response, "OK\r\n");
        }
        break;

    case KVS_CMD_HEXIST:
        if (count != 2)
        {
            DEBUG("Invalid HEXIST command format, expected 2 tokens, got %d\n", count);
            return -2; // 错误的命令格式
        }
        ret = kvs_hash_exist(&global_hash, tokens[1]);
        if (ret < 0)
        {
            length = sprintf(response, "ERROR\r\n");
        }
        else if (ret == 0)
        {
            length = sprintf(response, "EXIST\r\n");
        }
        else
        {
            length = sprintf(response, "NOT EXIST\r\n");
        }
        break;

    default:
        DEBUG("Unknown command: %s\n", tokens[0]);
        break;
    }

    DEBUG("Response generated: %s\n", response);
    return length; // 返回响应长度
}

/**
 * @brief KVS协议处理器，解析并执行客户端请求
 * @param msg 客户端请求消息（以空格分隔的字符串）
 * @param length 请求消息长度（字节数）
 * @param responese 用于存储响应消息的缓冲区
 * @return int 成功返回响应消息长度，失败返回负数
 * @note 如果msg或responese为NULL，或length小于等于0，则返回-1
 * @note 如果分割token失败，则返回-2
 * @note 如果过滤协议失败，则返回-3
 * @note 如果成功处理请求，则返回响应长度
 */
int kvs_protocol(char *msg, int length, char *responese)
{
    // SET Key Value
    // GET Key
    // DEL Key
    if (msg == NULL || length <= 0 || responese == NULL)
    {
        DEBUG("Invalid arguments to kvs_protocol\n");
        return -1;
    }

    printf("\nrecv %d : %s\n", length, msg);

    char *tokens[KVS_MAX_TOKENS] = {0};
    int count = kvs_split_token(msg, tokens);
    if (count == -1)
    {
        DEBUG("Failed to split tokens[%d]\n", count);
        return -2;
    }

    int ret = kvs_protocol_filter(tokens, count, responese);
    if (ret < 0)
    {
        DEBUG("Failed to filter protocol [%d]\n", ret);
        return -3;
    }

    return ret; // 返回响应长度
}

/**
 * @brief 初始化KVS引擎
 * @return int 成功返回0，失败返回负数
 * @note 该函数会创建全局KVS数组实例
 */
int init_kvengine(void)
{
#if ENABLE_ARRAY
    memset(&global_array, 0, sizeof(kvs_array_t));
    if (kvs_array_create(&global_array) < 0)
    {
        DEBUG("Failed to create global KVS array\n");
        return -1; // 创建失败
    }
    DEBUG("Global KVS array created successfully\n");
#endif

#if ENABLE_RBTREE
    memset(&global_rbtree, 0, sizeof(kvs_rbtree_t));
    if (kvs_rbtree_create(&global_rbtree) < 0)
    {
        DEBUG("Failed to create global KVS rbtree\n");
        return -1; // 创建失败
    }
    DEBUG("Global KVS rbtree created successfully\n");
#endif
#if ENABLE_HASH
    memset(&global_hash, 0, sizeof(kvs_hash_t));
    if (kvs_hash_create(&global_hash) < 0)
    {
        DEBUG("Failed to create global KVS hash\n");
        return -1; // 创建失败
    }
    DEBUG("Global KVS hash created successfully\n");
#endif
    return 0; // 初始化成功
}

void dest_kvengine(void)
{
#if ENABLE_ARRAY
    kvs_array_destory(&global_array);
    DEBUG("Global KVS array destroyed successfully\n");
#endif
#if ENABLE_RBTREE
    kvs_rbtree_destory(&global_rbtree);
    DEBUG("Global KVS rbtree destroyed successfully\n");
#endif
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

        // 初始化KVS引擎
        if (init_kvengine() < 0)
        {
            DEBUG("Failed to initialize KVS engine\n");
            return -1; // 初始化失败
        }
        DEBUG("KVS engine initialized successfully\n");

        // 启动网络服务
#if (NETWORK_SELECT == ENABLE_REACTOR) // Reactor-based server
        DEBUG("Starting reactor on port %d\n", port);
        reactor_start(port, kvs_protocol);
#elif (NETWORK_SELECT == ENABLE_NTYCO)   // NtyCo-based server
        DEBUG("Starting NtyCo server on port %d\n", port);
        ntyco_start(port, kvs_protocol);
#elif (NETWORK_SELECT == ENABLE_IOURING) // io_uring-based server
        DEBUG("Starting io_uring server on port %d\n", port);
        iouring_start(port, kvs_protocol);
#endif
        dest_kvengine(); // 销毁KVS引擎实例
        DEBUG("KVS engine destroyed successfully\n");
    }
}