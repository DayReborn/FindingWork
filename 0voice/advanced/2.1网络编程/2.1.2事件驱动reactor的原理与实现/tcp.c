#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/select.h> // 主要头文件（包含fd_set及相关函数）
#include <sys/poll.h>   // 包含poll函数和相关结构体
#include <sys/epoll.h>  // 包含epoll函数和相关结构体
#include <sys/time.h>   // 包含时间相关的结构体和函数

void *client_thread(void *arg)
{
    int clientfd = *(int *)arg; // 从参数中获取客户端文件描述符

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    while (1)
    {
        // 接下来会阻塞在recv函数上，等待客户端发送数据
        printf("[Clientfd:%d]-Recving data...\n", clientfd);
        int count = recv(clientfd, buffer, 1024, 0);
        if (count <= 0)
        {
            printf("Client disconnected or error occurred.\n");
            close(clientfd); // 关闭客户端连接
            printf("[Clientfd:%d]-Connection closed.\n", clientfd);
            pthread_exit(NULL); // 退出线程
        }
        printf("[Clientfd:%d]-recv count: %d\n", clientfd, count);
        printf("[Clientfd:%d]-recv data: %s\n", clientfd, buffer);

        count = send(clientfd, "Hello, client!", 15, 0);
        printf("[Clientfd:%d]-send count: %d\n", clientfd, count);
        printf("[Clientfd:%d]-send data: Hello, client!\n", clientfd);
    }
}

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // 监听所有接口——0.0.0.0
    servaddr.sin_port = htons(4646);              // 监听端口4646——0-1023端口为系统保留端口
    if (-1 == bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)))
    {
        printf("Bind error: %s\n", strerror(errno));
        return -1;
    }

    listen(sockfd, 10); // 监听队列长度为10
    printf("Listen finished!\n");

    struct sockaddr_in clientaddr;
    socklen_t len = sizeof(clientaddr);

    // 当我们运行代码的时候首先会阻塞在这个地方，而不是阻塞在下面的getchar();

    int clientfd;

#if 0
    printf("accepting...\n");
    clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);
    printf("Accept finished!\n");
    printf("[Sockfd:%d, Clientfd:%d]\n", sockfd, clientfd);

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    // 接下来会阻塞在recv函数上，等待客户端发送数据
    printf("Recv data...\n");
    int count = recv(clientfd, buffer, 1024, 0);
    printf("-recv count: %d\n", count);
    printf("-recv data: %s\n", buffer);

    count = send(clientfd, "Hello, client!", 15, 0);
    printf("-send count: %d\n", count);
    printf("-send data: Hello, client!\n");

#elif 0
    while (1)
    {
        printf("accepting...\n");
        clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);
        printf("Accept finished!\n");
        printf("[Sockfd:%d, Clientfd:%d]\n", sockfd, clientfd);

        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));

        // 接下来会阻塞在recv函数上，等待客户端发送数据
        printf("Recv data...\n");
        int count = recv(clientfd, buffer, 1024, 0);
        if (count <= 0)
        {
            printf("Client disconnected or error occurred.\n");
            break;
        }
        printf("-recv count: %d\n", count);
        printf("-recv data: %s\n", buffer);

        count = send(clientfd, "Hello, client!", 15, 0);
        printf("-send count: %d\n", count);
        printf("-send data: Hello, client!\n");
    }
#elif 0
    while (1)
    {
        printf("accepting...\n");
        clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);
        printf("Accept finished!\n");
        printf("[Sockfd:%d, Clientfd:%d]\n", sockfd, clientfd);

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, client_thread, &clientfd) != 0)
        {
            printf("Failed to create thread: %s\n", strerror(errno));
            close(clientfd);
            continue; // 如果线程创建失败，继续等待下一个连接
        }
    }

#elif 0
    // select方法
    /**
     * @param maxfd: 设置内部循环的最大fd编号
     * @param rset: fd_set 本质是 1024 位的位图，用来标志这个io是否可读
     * @param wset: fd_set，用来标志这个io是否可写
     * @param eset: fd_set，用来标志这个io是否可读
     * @param timeout: 超时控制
     * int nready = select(maxfd, rset, wset,eset,timeout)这个函数需要五个参数
     **/

    fd_set rfds, rset;
    FD_ZERO(&rfds);
    FD_SET(sockfd, &rfds);

    int maxfd = sockfd; // 最大文件描述符
    while (1)
    {
        rset = rfds; // rset是rfds的副本，select会修改rset，所以每次循环都需要重新赋值
        // nready表示有多少个文件描述符就绪, +1是因为select的第一个参数是最大文件描述符加1
        // select会阻塞，直到有文件描述符就绪或者超时
        // timeout参数为NULL表示无限等待，直到有文件描述符就绪
        printf("Waiting for select...\n");
        int nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
        if (FD_ISSET(sockfd, &rset))
        {
            struct sockaddr_in clientaddr;
            socklen_t len = sizeof(clientaddr);
            memset(&clientaddr, 0, len); // 清空客户端地址结构体

            int clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);

            FD_SET(clientfd, &rfds);
            printf("New connection accepted: [Sockfd:%d, Clientfd:%d]\n", sockfd, clientfd);

            if (maxfd < clientfd)
                maxfd = clientfd;
        }

        int i = 0;
        for (i = sockfd + 1; i <= maxfd; ++i)
        {
            if (FD_ISSET(i, &rset))
            {
                char buffer[1024] = {0};
                int count = recv(i, buffer, 1024, 0);
                if (count == 0)
                {
                    // 客户端断开连接
                    printf("Clientfd:%d disconnected\n", i);
                    // !不关闭事件会一直循环循环
                    // close(i);
                    FD_CLR(i, &rfds);
                    close(i);

                    continue;
                }
                send(i, buffer, count, 0);
                printf("Clientfd %d\ncount: %d\nbuffer: %s\n", i, count, buffer);
            }
        }
    }

#elif 0
    // poll方法
    struct pollfd fds[1024] = {0};

    fds[sockfd].fd = sockfd;
    fds[sockfd].events = POLLIN;
    int maxfd = sockfd;
    while (1)
    {
        int nready = poll(fds, maxfd + 1, -1);
        if (fds[sockfd].revents & POLLIN)
        {
            struct sockaddr_in clientaddr;
            socklen_t len = sizeof(clientaddr);
            int clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);

            printf("New connection accepted: [Sockfd:%d, Clientfd:%d]\n", sockfd, clientfd);
            fds[clientfd].fd = clientfd;
            fds[clientfd].events = POLLIN;

            maxfd = clientfd;
        }
        int i = 0;
        for (i = sockfd + 1; i <= maxfd; ++i)
        {
            if (fds[i].revents & POLLIN)
            {
                char buffer[1024] = {0};
                int count = recv(i, buffer, 1024, 0);
                if (count == 0)
                {
                    printf("Clientfd:%d disconnected\n", i);
                    // !不关闭事件会一直循环循环
                    fds[i].fd = -1;
                    fds[i].events = 0;
                    close(i);

                    continue;
                }
                send(i, buffer, count, 0);
                printf("clientfd: %d\ncount: %d\nbuffer: %s\n", i, count, buffer);
            }
        }
    }

#elif 1
    // 使用epoll方法
    // create只要参数不为0和负即可
    int epfd = epoll_create(1);

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);  // 将sockfd添加到epoll中
    printf("Epoll created and sockfd added!\n");
    printf("Epollfd: %d\n", epfd);
    printf("Sockfd: %d\n", sockfd);
    printf("Waiting for connections...\n");

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
                struct sockaddr_in clientaddr;
                socklen_t len = sizeof(clientaddr);

                int clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);
                printf("New connection accepted: [Sockfd:%d, Clientfd:%d]\n", sockfd, clientfd);

                ev.events = EPOLLIN;
                ev.data.fd = clientfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &ev);

                printf("sockfd: %d\n", clientfd);
            }
            else if (events[i].events & EPOLLIN)
            {
                char buffer[128] = {0};
                int count = recv(connfd, buffer, 128, 0);
                if (count == 0)
                {
                    printf("Clientfd:%d disconnected\n", connfd);
                    // !不关闭事件会一直循环循环

                    epoll_ctl(epfd, EPOLL_CTL_DEL, connfd, NULL);// 从epoll中删除事件
                    close(connfd);

                    continue;
                }
                send(connfd, buffer, count, 0);
                printf("[Clientfd: %d, Count: %d]\nbuffer: %s\n", connfd, count, buffer);
            }
        }
    }

#endif

    getchar(); // 等待用户输入以便查看结果

    close(clientfd);
    close(sockfd);

    printf("Exit!\n");
    return 0;
}