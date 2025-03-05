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

#if 0
    // ==============================================================================
    // 补充接受数据部分
    struct sockaddr_in clientaddr;
    socklen_t len = sizeof(clientaddr);
    // 当我们运行代码的时候首先会阻塞在这个地方，而不是阻塞在下面的getchar();
    int clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);

    // 具体来写接受部分代码
    printf("accept\n");

#if 0
    char buffer[128] = {0};
    int count = recv(clientfd, buffer, 128, 0);
    send(clientfd, buffer, count, 0);

    printf("sockfd:%d\nclientfd:%d\ncount:%d\nbuffer:%s", sockfd, clientfd, count, buffer);

#else
    while (1)
    {
        char buffer[128] = {0};
        int count = recv(clientfd, buffer, 128, 0);
        if(count == 0){
            close(clientfd);
            break;
        }
        send(clientfd, buffer, count, 0);
        printf("sockfd:%d\nclientfd:%d\ncount:%d\nbuffer:%s\n", sockfd, clientfd, count, buffer);
    }
#endif

#elif 0
    while (1)
    {
        struct sockaddr_in clientaddr;
        socklen_t len = sizeof(clientaddr);
        int clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);
        int *clientfd_ptr = malloc(sizeof(int)); // 动态分配内存
        *clientfd_ptr = clientfd;                // 复制文件描述符的值
        pthread_t thid;
        if (pthread_create(&thid, NULL, client_thread, clientfd_ptr) != 0)
        {
            perror("pthread_create failed");
            free(clientfd_ptr);
            close(clientfd);
        }
        else
        {
            pthread_detach(thid); // 分离线程
        }
    }
#elif 0
    // @param maxfd: 设置内部循环的最大fd编号
    // @param rset: fd_set 本质是 1024 位的位图，用来标志这个io是否可读
    // @param wset: fd_set，用来标志这个io是否可写
    // @param eset: fd_set，用来标志这个io是否可读
    // @param timeout: 超时控制
    // *int nready = select(maxfd, rset, wset,eset,timeout)这个函数需要五个参数

    fd_set rfds, rset;
    FD_ZERO(&rfds);
    FD_SET(sockfd, &rfds);

    int maxfd = sockfd;

    printf("loop\n");
    while (1)
    {
        rset = rfds;
        int nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
        if (FD_ISSET(sockfd, &rset))
        {
            struct sockaddr_in clientaddr;
            socklen_t len = sizeof(clientaddr);
            int clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);

            printf("sockfd: %d\n", clientfd);

            FD_SET(clientfd, &rfds);
            maxfd = clientfd;
        }
        int i = 0;
        for (i = sockfd + 1; i <= maxfd; ++i)
        {
            if (FD_ISSET(i, &rset))
            {
                char buffer[128] = {0};
                int count = recv(i, buffer, 128, 0);
                if (count == 0)
                {
                    printf("disconnect\n");
                    // !不关闭事件会一直循环循环
                    // close(i);
                    FD_CLR(i, &rfds);
                    close(i);

                    continue;
                }
                send(i, buffer, count, 0);
                printf("clientfd:%d\ncount:%d\nbuffer:%s\n", i, count, buffer);
            }
        }
    }

#elif 0
    struct pollfd fds[1024] = {0};

    fds[sockfd].fd = sockfd;
    fds[sockfd].events = POLLIN;
    int maxfd = sockfd;
    while (1)
    {
        // *其实会遍历整个数组，这里使用maxfd+1是为了简化操作
        int nready = poll(fds, maxfd + 1, -1);
        if (fds[sockfd].revents & POLLIN)
        {
            struct sockaddr_in clientaddr;
            socklen_t len = sizeof(clientaddr);
            int clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);

            printf("sockfd: %d\n", clientfd);
            fds[clientfd].fd = clientfd;
            fds[clientfd].events = POLLIN;

            maxfd = clientfd;
        }
        int i = 0;
        for (i = sockfd + 1; i <= maxfd; ++i)
        {
            if (fds[i].revents & POLLIN)
            {
                char buffer[128] = {0};
                int count = recv(i, buffer, 128, 0);
                if (count == 0)
                {
                    printf("disconnect\n");
                    // !不关闭事件会一直循环循环
                    fds[i].fd = -1;
                    fds[i].events = 0;
                    close(i);

                    continue;
                }
                send(i, buffer, count, 0);
                printf("clientfd:%d\ncount:%d\nbuffer:%s\n", i, count, buffer);
            }
        }
    }
# else
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
                
                printf("sockfd: %d\n", clientfd);
            }
            else if(events[i].events & EPOLLIN)
            {
                // char buffer[128] = {0};  
                // int count = recv(connfd, buffer, 128, 0);
                char buffer[15] = {0};  // 设置为15测试水平和边沿
                int count = recv(connfd, buffer, 15, 0);
                if (count == 0)
                {
                    printf("disconnect\n");
                    // !不关闭事件会一直循环循环
                    
                    epoll_ctl(epfd,EPOLL_CTL_DEL,connfd,NULL);
                    close(connfd);

                    continue;
                }
                send(connfd, buffer, count, 0);
                printf("clientfd:%d\ncount:%d\nbuffer:%s\n",connfd, count, buffer);
            }
        }
    }

#endif

    // ==============================================================================
    getchar();
    // 不要将关闭交给服务端，否则另一边端口不会释放，会一直报Timeout
    close(sockfd);
}
