#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>
#include <sys/epoll.h>
#include <stdlib.h>

#define BUFFER_LENGTH 1024

// 定义回调函数类型
typedef int (*RCALLBACK)(int fd);

// 前向声明回调函数
int accept_cb(int fd);
int recv_cb(int fd);
int send_cb(int fd);

/**
 * 连接项结构体，保存每个连接的信息
 * @fd: 文件描述符
 * @rbuffer: 数据接收缓冲区
 * @rlen: 接收缓冲区中的数据长度
 * @wbuffer: 数据发送缓冲区
 * @wlen: 发送缓冲区中的数据长度
 * @recv_t: 接收回调联合体（监听socket用accept，普通socket用recv）
 * @send_callback: 发送回调函数
 * @bug accept_callback: Output extra lines
 * TODO: Fix the above bugs
 */
struct conn_item
{
    int fd;

    char rbuffer[BUFFER_LENGTH];
    int rlen;

    char wbuffer[BUFFER_LENGTH];
    int wlen;

    union
    {
        RCALLBACK accept_callback; // 用于监听socket的accept回调
        RCALLBACK recv_callback;   // 用于普通socket的recv回调
    } recv_t;
    RCALLBACK send_callback; // 发送回调
};

int epfd = 0;                          // epoll实例的文件描述符
struct conn_item connlist[1024] = {0}; // 连接项数组，索引为文件描述符

/**
 * 设置epoll事件
 * @param fd 需要设置的文件描述符
 * @param event 事件类型（EPOLLIN/EPOLLOUT）
 * @param flag 0表示修改现有事件，非0表示添加新事件
 * @return 始终返回0
 */
int set_event(int fd, int event, int flag)
{
    struct epoll_event ev;
    ev.events = event;
    ev.data.fd = fd;

    if (flag)
    {
        // 添加新事件
        epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
    }
    else
    {
        // 修改现有事件
        epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
    }
    return 0;
}

/**
 * 接受连接的回调函数
 * @param fd 监听socket的文件描述符
 * @return 成功返回clientfd，失败返回-1
 */
int accept_cb(int fd)
{
    struct sockaddr_in clientaddr;
    socklen_t len = sizeof(clientaddr);
    // 接受新连接
    int clientfd = accept(fd, (struct sockaddr *)&clientaddr, &len);
    if (clientfd < 0)
        return clientfd;

    // 将新连接的socket加入epoll监听读事件
    set_event(clientfd, EPOLLIN, 1); // 第三个参数1表示添加新事件

    // 初始化连接信息
    connlist[clientfd].fd = clientfd;
    memset(connlist[clientfd].rbuffer, 0, BUFFER_LENGTH);
    connlist[clientfd].rlen = 0;
    memset(connlist[clientfd].wbuffer, 0, BUFFER_LENGTH);
    connlist[clientfd].wlen = 0;
    printf("client: %d\n", connlist[clientfd].fd);
    // 设置回调函数
    connlist[clientfd].recv_t.recv_callback = recv_cb;
    connlist[clientfd].send_callback = send_cb;

    return clientfd;
}

/**
 * 接收数据的回调函数
 * @param fd 数据来源的socket文件描述符
 * @return 接收到的字节数，连接关闭返回0
 */
int recv_cb(int fd)
{
    char *buffer = connlist[fd].rbuffer;
    int idx = connlist[fd].rlen;
    // 接收数据
    int count = recv(fd, buffer + idx, BUFFER_LENGTH - idx, 0);

    if (count == 0)
    { // 连接关r闭
        printf("disconnect\n");
        epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
        close(fd);
        return -1;
    }
    else if (count > 0)
    {
        connlist[fd].rlen += count; // 更新缓冲区索引
    }

// 解决发送字符串问题
#if 1 // echo: need to send
    memcpy(connlist[fd].wbuffer, connlist[fd].rbuffer, connlist[fd].rlen);
    connlist[fd].wlen = connlist[fd].rlen;
#endif

    // 修改事件为监听写事件（准备发送数据）
    set_event(fd, EPOLLOUT, 0); // 第三个参数0表示修改事件
    return count;
}

/**
 * 发送数据的回调函数
 * @param fd 目标socket文件描述符
 * @return 发送的字节数
 */
int send_cb(int fd)
{
    char *buffer = connlist[fd].wbuffer;
    int idx = connlist[fd].wlen;
    // 发送数据
    int count = send(fd, buffer, idx, 0);
    // 修改事件为监听读事件（准备接收下一批数据）
    set_event(fd, EPOLLIN, 0);
    return count;
}

// 主函数（TCP服务器）
int main()
{
    // 创建监听socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(2048);

    // 绑定地址
    if (bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
    {
        perror("bind");
        return -1;
    }

    // 开始监听
    listen(sockfd, 10);

    // 初始化监听socket的连接项
    connlist[sockfd].fd = sockfd;
    connlist[sockfd].recv_t.accept_callback = accept_cb; // 设置accept回调

    // 创建epoll实例
    epfd = epoll_create(1);
    // 将监听socket加入epoll
    set_event(sockfd, EPOLLIN, 1); // 第三个参数1表示添加新事件

    struct epoll_event events[1024] = {0};
    while (1)
    {
        // 等待事件发生
        int nready = epoll_wait(epfd, events, 1024, -1);
        for (int i = 0; i < nready; ++i)
        {
            int connfd = events[i].data.fd;
            // 处理读事件（包括新连接和数据到达）
            if (events[i].events & EPOLLIN)
            {
                // 调用对应的回调函数
                // ! bug is created by this lines, the recv_t is accept, but the printf function is still running.
                int count = connlist[connfd].recv_t.recv_callback(connfd);
                printf("recv[count: %d] <-- buffer:%s\n", count, connlist[connfd].rbuffer);
            }
            // 处理写事件（数据可发送）
            else if (events[i].events & EPOLLOUT)
            {
                printf("send --> buffer:%s\n", connlist[connfd].wbuffer);
                int count = connlist[connfd].send_callback(connfd);
            }
        }
    }

    close(sockfd);
}