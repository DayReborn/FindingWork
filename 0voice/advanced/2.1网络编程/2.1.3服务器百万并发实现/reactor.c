#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/epoll.h> // 包含epoll函数和相关结构体
#include <sys/time.h>  // 包含时间相关的结构体和函数

#define BUFFER_LENGTH 1024

// RCALLBACK 是一个回调函数类型的定义
// 表示一个接受一个整数参数 fd 并返回整数的函数指针类型。
typedef int (*RCALLBACK)(int fd); 
int epfd = 0; // epoll文件描述符

typedef struct conn
{
    int fd; // 文件描述符

    char rbuffer[BUFFER_LENGTH];
    int rlength; // 接收数据长度

    char wbuffer[BUFFER_LENGTH];
    int wlength; // 发送数据长度

    RCALLBACK recv_cb; // 接收回调函数
    RCALLBACK send_cb; // 发送回调函数
    RCALLBACK accept_cb; // 接受连接回调函数

} conn_t;


// 函数声明
int init_server(unsigned short port); // 初始化服务器，监听指定端口
int set_event(int fd, int event);     // 设置事件，将文件描述符添加到epoll中



// 函数实现
int init_server(unsigned short port)
{
    if (port < 1024 || port > 65535)
    {
        printf("Port number must be between 1024 and 65535.\n");
        return -1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // 监听所有接口——0.0.0.0
    servaddr.sin_port = htons(port);              // 监听端口——0-1023端口为系统保留端口

    if (-1 == bind(sockfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr)))
    {
        printf("Bind error: %s\n", strerror(errno));
        return -2;
    }

    listen(sockfd, 10); // 监听队列长度为10
    printf("Listen finished!\n");

    return sockfd;
}

int set_event(int fd, int event)
{
    struct epoll_event ev;
    ev.events = event;
    ev.data.fd = fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev); // 将sockfd添加到epoll中
}

int main()
{
    unsigned short port = 4646; // 端口号
    int sockfd = init_server(port);
    if (sockfd < 0)
    {
        printf("Failed to initialize server on port %d.\n", port);
        return -1;
    }

    epfd = epoll_create(1);

    if (epfd < 0)
    {
        printf("Failed to create epoll instance: %s\n", strerror(errno));
        close(sockfd);
        return -2;
    }

    set_event(sockfd, EPOLLIN); // 设置sockfd为可读事件
}