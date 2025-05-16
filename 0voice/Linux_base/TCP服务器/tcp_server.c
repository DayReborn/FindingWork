#include <stdio.h>  // 标准输入输出函数
#include <string.h> // 字符串操作函数
#include <stdlib.h> // 标准库函数

#include <unistd.h> // UNIX标准函数（如read、write、close）
#include <errno.h>  // 错误码定义（如errno）
#include <fcntl.h>  // 文件控制相关函数和数据结构

#include <sys/socket.h>  // 套接套接字操作相关函数和数据结构
#include <netinet/in.h>  // IPv4地址结构sockaddr_in定义
#include <netinet/tcp.h> // TCP协议相关函数和数据结构
#include <arpa/inet.h>   // inet_pton函数
#include <pthread.h>     // 线程相关函数和数据结构
#include <sys/epoll.h>   // epoll相关函数和数据结构

#define BUFFER_LENGTH 1024
#define EPOLL_SIZE 1024




void *client_routine(void *arg)
{
    int clientfd = *(int *)arg;
    printf("client %d thread start\n", clientfd);
    while (1)
    {
        char buffer[BUFFER_LENGTH] = {0};
        int len = recv(clientfd, buffer, BUFFER_LENGTH, 0); // 接收数据
        if (len < 0)
        {
            // 在阻塞的IO当中不会存在下面的情况
            // if(errno == EAGAIN || errno == EWOULDBLOCK){
            //     printf("recv timeout\n");
            // }
            close(clientfd);
            break;
        }
        else if (len == 0)
        {
            printf("client close\n");
            close(clientfd);
            break;
        }
        else
        {
            printf("recv: %s, bytes: %d\n", buffer, len);
            send(clientfd, buffer, len, 0); // 回送数据
        }
    }
    printf("client %d thread exit\n", clientfd);
    close(clientfd);
    pthread_exit(NULL); // 线程退出
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Param Error!\n");
        return -1;
    }
    int port = atoi(argv[1]);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET; // IPv4
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY; // 绑定到所有可用的地址

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0)
    {
        perror("bind");
        return -2;
    }

    if (listen(sockfd, 5) < 0)
    {
        perror("listen");
        return -3;
    } // 监听队列长度为5

#if 0
    while (1)
    {
        struct sockaddr_in client_addr;
        memset(&client_addr, 0, sizeof(struct sockaddr_in));
        socklen_t addr_len = sizeof(struct sockaddr_in);

        int clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len); // 阻塞等待连接
        pthread_t thead_id;
        pthread_create(&thead_id, NULL, client_routine, &clientfd); // 创建线程处理连接
    }

    return 0;

#elif 1
    int epfd = epoll_create(1);
    struct epoll_event events[EPOLL_SIZE] = {0};
    struct epoll_event ev;
    ev.events = EPOLLIN; // 读事件
    ev.data.fd = sockfd; // 监听套接字
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev); // 添加监听套接字到epoll

    while(1){
        int nready = epoll_wait(epfd, events, EPOLL_SIZE, -1); // 等待事件发生
        // 这边-1表示阻塞等待
        // 如果设置为5表示超时5秒，最终返回-1
        int i = 0;
        for(i = 0; i<nready;++i){
            if(events->data.fd == sockfd){
                struct sockaddr_in client_addr;
                memset(&client_addr, 0, sizeof(struct sockaddr_in));
                socklen_t addr_len = sizeof(struct sockaddr_in);
                int clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len); // 阻塞等待连接
                ev.events = EPOLLIN; // 读事件
                ev.data.fd = clientfd; // 客户端套接字
                epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &ev); // 添加客户端套接字到epoll
            }else{
                int clientfd = events[i].data.fd;
                char buffer[BUFFER_LENGTH] = {0};
                int len = recv(clientfd, buffer, BUFFER_LENGTH, 0); // 接收数据
                if (len < 0)
                {  
                    close(clientfd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, clientfd, NULL); // 删除客户端套接字

                    break;
                }
                else if (len == 0)
                {
                    printf("client close\n");
                    close(clientfd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, clientfd, NULL); // 删除客户端套接字
                    break;
                }
                else
                {
                    printf("recv: %s, bytes: %d\n", buffer, len);
                    send(clientfd, buffer, len, 0); // 回送数据
                }
            }   

        }
    }

#endif

}