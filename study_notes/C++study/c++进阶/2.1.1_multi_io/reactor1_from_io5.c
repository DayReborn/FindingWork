#include <sys/socket.h> // 套接套接字操作相关函数和数据结构
#include <errno.h>      // 错误码定义（如errno）
#include <netinet/in.h> // IPv4地址结构sockaddr_in定义

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>
#include <sys/select.h>
#include <sys/poll.h>
#include <sys/epoll.h>
#include <stdlib.h> // 添加此行，声明 malloc 和 free

#define BUFFER_LENGTH 1024

int epfd = 0;

// * listenfd
// * listenfd 触发 EPOLLIN(事件)， 执行accept_cb()
int accept_cb(int fd);

// * clientfd
// * clientfd 触发 EPOLLIN(事件)， 执行recv_cb()
// * clientfd 触发 EPOLLIN(事件)， 执行send_cb()
int recv_cb(int fd);
int send_cb(int fd);

struct conn_item
{
    int fd;
    char buffer[BUFFER_LENGTH];
    int idx;
};

struct conn_item connlist[1024] = {0};

int accept_cb(int fd)
{
    struct sockaddr_in clientaddr;
    socklen_t len = sizeof(clientaddr);
    struct epoll_event ev;

    int clientfd = accept(fd, (struct sockaddr *)&clientaddr, &len);

    if (clientfd < 0)
        return clientfd;

    ev.events = EPOLLIN; // * 水平触发状态，数据没读完就一直触发触发触发！！！
    // * ev.events = EPOLLIN | EPOLLET;  // * 边沿触发状态，数据只触发一次！
    ev.data.fd = clientfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &ev);

    // ! 初始化部分
    connlist[clientfd].fd = clientfd;
    memset(connlist[clientfd].buffer, 0, BUFFER_LENGTH);
    connlist[clientfd].idx = 0;

    return clientfd;
}

int recv_cb(int fd)
{
    char *buffer = connlist[fd].buffer;
    int idx = connlist[fd].idx;
    int count = recv(fd, buffer + idx, BUFFER_LENGTH - idx, 0);

    if (count == 0)
    {
        printf("disconnect\n");
        //* 怎么解决缺少epollfd的问题
        //* 1.定义为全局变量
        //* 2.定义一个reactor结构体
        epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
        close(fd);
    }
    connlist[fd].idx += count;

    // 设置事件
    struct epoll_event ev;
    ev.events = EPOLLOUT;
    ev.data.fd = fd;
    epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);

    // send(fd, buffer, count, 0);
    return count;
}

int send_cb(int fd)
{
    char *buffer = connlist[fd].buffer;
    int idx = connlist[fd].idx;

    int count = send(fd, buffer, idx, 0);

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);

    return count;
}

// tcp
int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serveraddr;

    memset(&serveraddr, 0, sizeof(struct sockaddr_in));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(2048);

    // bind()_绑定套接字与地址。将套接字关联到特定的IP地址和端口，使客户端能通过该地址访问服务。
    if (-1 == bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr)))
    {
        perror("bind");
        return -1;
    }

    // 将套接字设置为被动模式，准备接受客户端的连接请求。
    listen(sockfd, 10);

    // create只要参数不为0和负即可
    epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

    struct epoll_event events[1024] = {0};
    while (1)
    {
        int nready = epoll_wait(epfd, events, 1024, -1);
        int i = 0;
        for (i = 0; i < nready; ++i)
        {
            int connfd = events[i].data.fd;
            if (sockfd == connfd)
            {
                int clientfd = accept_cb(sockfd);
                printf("sockfd: %d\n", clientfd);
            }
            else if (events[i].events & EPOLLIN)
            {
                int count = recv_cb(connfd);
                printf("recv <-- buffer:%s\n", connlist[connfd].buffer);
            }
            else if (events[i].events & EPOLLOUT)
            {
                int count = send_cb(connfd);
                printf("send --> buffer:%s\n", connlist[connfd].buffer);
            }
        }
    }

    // ==============================================================================
    getchar();
    // 不要将关闭交给服务端，否则另一边端口不会释放，会一直报Timeout
    close(sockfd);
}
