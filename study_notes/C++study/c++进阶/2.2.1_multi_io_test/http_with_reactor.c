#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>
#include <sys/epoll.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>

#define BUFFER_LENGTH 512     ///< 读写缓冲区大小
#define ENABLE_HTTP_RESPONSE 0 ///< 启用HTTP响应功能开关

/**
 * @brief 回调函数类型定义
 * @param fd 文件描述符
 * @return 成功返回处理字节数，失败返回-1
 */
typedef int (*RCALLBACK)(int fd);

// 前向声明回调函数原型
int accept_cb(int fd);
int recv_cb(int fd);
int send_cb(int fd);

/**
 * @brief 连接项结构体，管理每个连接的状态信息
 *
 * @param fd 套接字文件描述符
 * @param rbuffer 接收缓冲区
 * @param rlen 接收数据长度
 * @param wbuffer 发送缓冲区
 * @param wlen 发送数据长度
 * @param resource 请求资源路径
 * @param recv_t 接收回调联合体：
 *        - accept_callback: 用于监听套接字的接受连接回调
 *        - recv_callback: 用于普通套接字的数据接收回调
 * @param send_callback 数据发送回调函数
 *
 * @bug accept_callback在某些情况下会输出多余行
 * TODO 需要修复accept_callback的异常输出问题
 */
struct conn_item
{
    int fd;

    char rbuffer[BUFFER_LENGTH];
    int rlen;

    char wbuffer[BUFFER_LENGTH];
    int wlen;

    char resource[BUFFER_LENGTH]; ///< 客户端请求的资源路径，如"abc.html"

    union
    {
        RCALLBACK accept_callback; ///< 监听套接字使用的接受连接回调
        RCALLBACK recv_callback;   ///< 普通套接字使用的数据接收回调
    } recv_t;
    RCALLBACK send_callback; ///< 数据发送回调函数
};

int epfd = 0;                           ///< epoll实例文件描述符

// ! @bug 这边如果这个值设置的非常大会有编译错误
// ! 例如如果设置为1048576 = 1024*1024,BUFFER_LENGTH 1024 我们缓冲区的长度再为1024的话
// ! 最终就会申请创建一个rbuffer[BUFFER_LENGTH], wbuffer[BUFFER_LENGTH]合计2G的内存
struct conn_item connlist[1048576] = {0}; ///< 连接项数组，索引为文件描述符

struct timeval reactor_begin;

#define TIME_SUB_MS(tv1, tv2)  ((tv1.tv_sec - tv2.tv_sec) * 1000 + (tv1.tv_usec - tv2.tv_usec) / 1000)


#if ENABLE_HTTP_RESPONSE
typedef struct conn_item connection_t; ///< 连接项类型别名

#define ROOT_DIR "/home/zzx/Finding_work/2.1.1-multi-io/" ///< 网站根目录

/**
 * @brief HTTP请求解析函数
 * @param conn 连接项指针
 * @return 总是返回0（待实现）
 * @note 需要从类似"GET /index.html HTTP/1.1"的请求行中解析资源路径
 */
int http_request(connection_t *conn)
{
    // 待实现：解析请求并填充发送缓冲区
    return 0;
}

/**
 * @brief HTTP响应生成函数
 * @param conn 连接项指针
 * @return 返回响应数据长度
 * @note 支持静态文件响应，当前实现发送index.html文件内容
 */
int http_response(connection_t *conn)
{
#if 0
    // 硬编码的HTTP响应示例
    conn->wlen = sprintf(conn->wbuffer,
        "HTTP/1.1 200 OK\r\n"
        "Accept-Ranges: bytes\r\n"
        "Content-Length: 82\r\n"
        "Content-Type: text/html\r\n"
        "Date: Sat, 06 Aug 2023 13:16:46 GMT\r\n\r\n"
        "<html><head><title>0voice.king</title></head><body><h1>King</h1></body></html>\r\n\r\n");
#else
    // 动态文件响应实现
    int filefd = open("index.html", O_RDONLY);
    struct stat stat_buf;
    fstat(filefd, &stat_buf);

    // 生成响应头
    conn->wlen = sprintf(conn->wbuffer,
                         "HTTP/1.1 200 OK\r\n"
                         "Accept-Ranges: bytes\r\n"
                         "Content-Length: %ld\r\n"
                         "Content-Type: text/html\r\n"
                         "Date: Sat, 06 Aug 2023 13:16:46 GMT\r\n\r\n",
                         stat_buf.st_size);

    // 读取文件内容到发送缓冲区
    int count = read(filefd, conn->wbuffer + conn->wlen, BUFFER_LENGTH - conn->wlen);
    conn->wlen += count;

    // TODO: 实现分段文件发送功能（支持偏移和长度控制）
#endif
    return conn->wlen;
}
#endif

/**
 * @brief 设置epoll事件
 * @param fd 目标文件描述符
 * @param event 事件类型（EPOLLIN/EPOLLOUT）
 * @param flag 操作标志：0-修改事件，非0-添加事件
 * @return 总是返回0
 */
int set_event(int fd, int event, int flag)
{
    struct epoll_event ev;
    ev.events = event;
    ev.data.fd = fd;

    if (flag)
    {
        epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
    }
    else
    {
        epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
    }
    return 0;
}

/**
 * @brief 接受连接回调函数
 * @param fd 监听套接字文件描述符
 * @return 成功返回客户端fd，失败返回-1
 */
int accept_cb(int fd)
{
    struct sockaddr_in clientaddr;
    socklen_t len = sizeof(clientaddr);
    int clientfd = accept(fd, (struct sockaddr *)&clientaddr, &len);
    if (clientfd < 0)
        return clientfd;

    set_event(clientfd, EPOLLIN, 1);

    // 初始化新连接项
    struct conn_item *conn = &connlist[clientfd];
    conn->fd = clientfd;
    memset(conn->rbuffer, 0, BUFFER_LENGTH);
    conn->rlen = 0;
    memset(conn->wbuffer, 0, BUFFER_LENGTH);
    conn->wlen = 0;

    // 设置回调函数
    conn->recv_t.recv_callback = recv_cb;
    conn->send_callback = send_cb;

    if ((clientfd % 1000) == 999)
    {
        struct timeval tv_cur;
        gettimeofday(&tv_cur,NULL);
        int time_used  = TIME_SUB_MS(tv_cur, reactor_begin);
        memcpy(&reactor_begin, &tv_cur, sizeof(struct timeval));
        printf("cliendfd: %d, time_used: %d\n", conn->fd, time_used);
    }

    return clientfd;
}

/**
 * @brief 数据接收回调函数
 * @param fd 客户端文件描述符
 * @return 接收字节数，连接关闭返回0，错误返回-1
 */
int recv_cb(int fd)
{
    struct conn_item *conn = &connlist[fd];
    int count = recv(fd, conn->rbuffer + conn->rlen, BUFFER_LENGTH - conn->rlen, 0);

    if (count == 0)
    {
        // printf("Client %d disconnected\n", fd);
        epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
        close(fd);
        return -1;
    }
    else if (count > 0)
    {
        conn->rlen += count;
    }

// 数据处理分支
#if ENABLE_HTTP_RESPONSE
    http_request(conn);  // 解析HTTP请求
    http_response(conn); // 生成HTTP响应
#else
    // Echo模式：直接回显接收数据
    memcpy(conn->wbuffer, conn->rbuffer, conn->rlen);
    conn->wbuffer[conn->rlen] = '\0';
    conn->wlen = conn->rlen;
    conn->rlen -= conn->rlen;
#endif

    set_event(fd, EPOLLOUT, 0);
    return count;
}

/**
 * @brief 数据发送回调函数
 * @param fd 客户端文件描述符
 * @return 发送字节数
 */
int send_cb(int fd)
{
    struct conn_item *conn = &connlist[fd];
    int count = send(fd, conn->wbuffer, conn->wlen, 0);
    set_event(fd, EPOLLIN, 0); // 重置为监听读事件
    return count;
}

int Init_server(unsigned short port)
{
    // 初始化监听套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serveraddr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(port)};

    if (bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
    {
        perror("Bind failed");
        return -1;
    }
    listen(sockfd, 100);
    return sockfd;
}

/**
 * @brief 主函数（TCP服务器入口）
 * @return 程序退出状态码
 */
int main()
{
    int port_count = 100;
    unsigned short port = 2048;
    int i = 0;

    // 创建epoll实例
    epfd = epoll_create(1);

    for (i = 0; i < port_count; ++i)
    {
        int sockfd = Init_server(i + port);
        // 初始化监听套接字连接项
        connlist[sockfd].fd = sockfd;
        connlist[sockfd].recv_t.accept_callback = accept_cb;
        set_event(sockfd, EPOLLIN, 1);
    }

    gettimeofday(&reactor_begin,NULL);

    struct epoll_event events[65535] = {0};
    while (1)
    {
        int nready = epoll_wait(epfd, events, 65535, -1);
        for (int i = 0; i < nready; ++i)
        {
            int connfd = events[i].data.fd;

            if (events[i].events & EPOLLIN)
            {
                int count = connlist[connfd].recv_t.recv_callback(connfd);
                // if(count == -1) printf("Recv[%d bytes] <-- Client is closed!\n", count);
                // else printf("Recv[%d bytes] <-- %s\n", count, connlist[connfd].rbuffer);
            }
            else if (events[i].events & EPOLLOUT)
            {
                // printf("Send --> %s\n", connlist[connfd].wbuffer);
                int count = connlist[connfd].send_callback(connfd);
            }
        }
    }
    return 0;
}