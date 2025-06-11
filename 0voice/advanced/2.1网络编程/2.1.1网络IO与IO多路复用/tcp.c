#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

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
        printf("[Clientfd:%d]-send count: %d\n",clientfd, count);
        printf("[Clientfd:%d]-send data: Hello, client!\n",clientfd);
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
#elif 1
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

#endif

    getchar(); // 等待用户输入以便查看结果

    close(clientfd);
    close(sockfd);

    printf("Exit!\n");
    return 0;
}