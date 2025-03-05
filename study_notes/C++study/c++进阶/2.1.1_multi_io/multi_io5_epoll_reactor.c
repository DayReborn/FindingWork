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

#define BUFFER_LENGTH 128


struct conn_item
{
    int fd;
    char buffer[BUFFER_LENGTH];
    int idx;
};


struct conn_item connlist[1024] = {0};


// *这是一种阻塞的连接方式（block）
// *缺点就是有1w个科幻段
void *client_thread(void *arg)
{
    int clientfd = *(int *)arg;
    while (1)
    {
        char buffer[128] = {0};
        int count = recv(clientfd, buffer, 128, 0);
        if (count == 0)
        {
            break;
        }
        send(clientfd, buffer, count, 0);
        printf("clientfd:%d\ncount:%d\nbuffer:%s\n", clientfd, count, buffer);
    }
    close(clientfd);
    return NULL;
}

// tcp
int main()
{
    // AF_INET：IPv4地址族。
    // SOCK_STREAM：面向连接的TCP协议。
    // 0：自动选择协议（对于TCP，实际是IPPROTO_TCP）。
    // sockfd：套接字描述符，失败时返回-1。
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serveraddr;

    // 将serveraddr结构体的所有字节置为0，避免残留数据干扰后续操作。
    // 网络编程中结构体可能存在填充字节，未清零可能导致bind()失败
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
    int epfd = epoll_create(1);
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);


    struct epoll_event events[1024] = {0}; 
    while(1){
        int nready = epoll_wait(epfd,events, 1024,-1);
        int i = 0;
        for(i = 0;i <nready;++i){
            int connfd = events[i].data.fd;
            if(sockfd == connfd){
                struct sockaddr_in clientaddr;
                socklen_t len = sizeof(clientaddr);
                
                int clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);
                
                // * ev.events = EPOLLIN;  // * 水平触发状态，数据没读完就一直触发触发触发！！！
                ev.events = EPOLLIN | EPOLLET;  // * 边沿触发状态，数据只触发一次！
                ev.data.fd = clientfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &ev);
                
                // ! 初始化部分
                connlist[clientfd].fd = clientfd;
                memset(connlist[clientfd].buffer,0,BUFFER_LENGTH);
                connlist[clientfd].idx = 0;  

                printf("sockfd: %d\n", clientfd);
            }
            else if(events[i].events & EPOLLIN)
            {
                // ! 实现拼接
                char *buffer = connlist[connfd].buffer;
                int idx = connlist[connfd].idx;
                int count = recv(connfd, buffer+idx, BUFFER_LENGTH-idx, 0);
                
                if (count == 0)
                {
                    printf("disconnect\n");
                    // !不关闭事件会一直循环循环
                    
                    epoll_ctl(epfd,EPOLL_CTL_DEL,connfd,NULL);
                    close(connfd);

                    continue;
                }
                connlist[connfd].idx += count;

                send(connfd, buffer, count, 0);
                printf("clientfd:%d\ncount:%d\nbuffer:%s\n",connfd, count, buffer);
            }
        }
    }


    // ==============================================================================
    getchar();
    // 不要将关闭交给服务端，否则另一边端口不会释放，会一直报Timeout
    close(sockfd);
}
