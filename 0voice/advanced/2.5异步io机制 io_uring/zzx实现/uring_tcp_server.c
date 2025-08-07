
#include <stdio.h>
#include <liburing.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

//#define DEBUG printf
#define DEBUG 

#define EVENT_ACCEPT 0
#define EVENT_READ 1
#define EVENT_WRITE 2

#define ENTRIES_LENGTH 1024
#define BUFFER_LENGTH 1024

typedef struct conn_info
{
    int fd;
    int event;
} conn_info_t;

int init_server(unsigned short port)
{

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(struct sockaddr_in));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(port);

    if (-1 == bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr)))
    {
        perror("bind");
        return -1;
    }

    listen(sockfd, 10);

    return sockfd;
}

int set_event_recv(struct io_uring *ring, int sockfd,
                   void *buf, size_t len, int flags)
{
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);

    struct conn_info accept_info = {
        .fd = sockfd,
        .event = EVENT_READ,
    };

    io_uring_prep_recv(sqe, sockfd, buf, len, flags);
    memcpy(&sqe->user_data, &accept_info, sizeof(struct conn_info));
}

int set_event_send(struct io_uring *ring, int sockfd,
                   void *buf, size_t len, int flags)
{

    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);

    struct conn_info accept_info = {
        .fd = sockfd,
        .event = EVENT_WRITE,
    };

    io_uring_prep_send(sqe, sockfd, buf, len, flags);
    memcpy(&sqe->user_data, &accept_info, sizeof(struct conn_info));
}

int set_event_accept(struct io_uring *ring, int sockfd, struct sockaddr *addr,
                     socklen_t *addrlen, int flags)
{

    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);

    struct conn_info accept_info = {
        .fd = sockfd,
        .event = EVENT_ACCEPT,
    };

    io_uring_prep_accept(sqe, sockfd, (struct sockaddr *)addr, addrlen, flags);
    memcpy(&sqe->user_data, &accept_info, sizeof(struct conn_info));
}

int main(int argc, char *argv[])
{

    unsigned short port = 9999;
    int sockfd = init_server(port);
    struct io_uring_params params;
    memset(&params, 0, sizeof(params));

    struct io_uring ring;
    io_uring_queue_init_params(ENTRIES_LENGTH, &ring, &params);

    struct sockaddr_in clientaddr;
    socklen_t len = sizeof(clientaddr);
    set_event_accept(&ring, sockfd, (struct sockaddr *)&clientaddr, &len, 0);

    // 初始化缓冲区
    char buffer[BUFFER_LENGTH] = {0};

    while (1)
    {
        io_uring_submit(&ring);

        DEBUG("submitted accept request\n");
        DEBUG("waiting for cqe\n");
        // 等待完成事件
        struct io_uring_cqe *cqe;
        io_uring_wait_cqe(&ring, &cqe);

        // 一次性从io_uring中获取多个完成事件
        // 这里可以处理cqe->res来获取结果
        struct io_uring_cqe *cqes[128];
        int nready = io_uring_peek_batch_cqe(&ring, cqes, 128);

        int i = 0;
        for (i = 0; i < nready; i++)
        {
            struct io_uring_cqe *entries = cqes[i];

            conn_info_t result;
            memcpy(&result, &entries->user_data, sizeof(conn_info_t));
            DEBUG("\ncqe[%d]: fd=%d, event=%d, res=%d\n", i, result.fd, result.event, entries->res);

            if (result.event == EVENT_ACCEPT)
            {
                set_event_accept(&ring, sockfd, (struct sockaddr *)&clientaddr, &len, 0);
                // 处理接受事件
                DEBUG("Accepted connection on fd %d\n", result.fd);
                int connfd = entries->res;

                set_event_recv(&ring, connfd, buffer, BUFFER_LENGTH, 0);
            }
            else if (result.event == EVENT_READ)
            {
                int ret = entries->res;

                if (ret == 0)
                {
                    buffer[0] = '\0'; // 清空缓冲区
                    DEBUG("Connection closed by client on fd %d\n", result.fd);
                    close(result.fd);
                }
                else if (ret > 0)
                {
                    // 处理读取事件
                    DEBUG("set_event_recv ret: %d, %s\n", ret, buffer); //
                    set_event_send(&ring, result.fd, buffer, ret, 0);
                }
            }
            else if (result.event == EVENT_WRITE)
            {
                // 处理发送事件
                int ret = entries->res;
                DEBUG("set_event_send ret: %d, %s\n", ret, buffer);

                set_event_recv(&ring, result.fd, buffer, BUFFER_LENGTH, 0);
            }
        }
        io_uring_cq_advance(&ring, nready);
    }
}
