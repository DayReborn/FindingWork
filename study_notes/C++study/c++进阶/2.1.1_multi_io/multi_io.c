#include <sys/socket.h> // 套接套接字操作相关函数和数据结构
#include <errno.h>      // 错误码定义（如errno）
#include <netinet/in.h> // IPv4地址结构sockaddr_in定义

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>
#include <sys/poll.h>
#include <sys/epoll.h>
#include <stdlib.h>  // 添加此行，声明 malloc 和 free

void *client_thread(void *arg){
    int clientfd = *(int *)arg;
    while(1){
        char buffer[128] = {0};
        int count = recv(clientfd, buffer, 128, 0);
        if(count == 0){
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

#else
    while(1){
        struct sockaddr_in clientaddr;
        socklen_t len = sizeof(clientaddr);
        int clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &len);
        int *clientfd_ptr = malloc(sizeof(int));  // 动态分配内存
        *clientfd_ptr = clientfd;                // 复制文件描述符的值
        pthread_t thid;
        if (pthread_create(&thid, NULL, client_thread, clientfd_ptr) != 0) {
            perror("pthread_create failed");
            free(clientfd_ptr);
            close(clientfd);
        } else {
            pthread_detach(thid);  // 分离线程
        }
    }

#endif

    // ==============================================================================
    getchar();
    // 不要将关闭交给服务端，否则另一边端口不会释放，会一直报Timeout
    close(sockfd);
}
