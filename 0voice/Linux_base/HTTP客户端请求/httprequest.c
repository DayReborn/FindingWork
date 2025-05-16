#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include <netdb.h>
#include <fcntl.h>

#define HTTP_VERSION "HTTP/1.1"
#define CONNECTION_TYPE "Connection: close\r\n"

#define BUFFER_SIZE 4096

/**
 * @brief 将主机名解析为点分十进制 IP 字符串
 *
 * 通过 gethostbyname() 实现 DNS 解析，将返回的网络字节序地址
 * 转换成可读的点分十进制字符串。
 *
 * @param hostname 要解析的主机名（如 "www.example.com"）
 * @return 成功时返回静态缓冲区中的 IP 字符串（如 "93.184.216.34"），
 *         失败时返回 NULL
 */
char *host_to_ip(const char *hostname)
{
    struct hostent *host_entry = gethostbyname(hostname);
    if (host_entry != NULL)
    {
        return inet_ntoa(*(struct in_addr *)*host_entry->h_addr_list);
    }
    return NULL;
}

/**
 * @brief 创建一个与指定 IP、端口 80 的 TCP 非阻塞 socket 并连接
 *
 * @param ip 点分十进制的 IP 地址字符串
 * @return 成功时返回 socket 描述符，失败时返回 -1
 */
int http_create_socket(char *ip)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in sin = {0};
    sin.sin_family = AF_INET;
    sin.sin_port = htons(80);
    sin.sin_addr.s_addr = inet_addr(ip);

    if (0 != connect(sockfd, (struct sockaddr *)&sin, sizeof(struct sockaddr_in)))
    {
        perror("connect");
        close(sockfd);
        return -1;
    }
    // 设置为非阻塞模式
    fcntl(sockfd, F_SETFL, O_NONBLOCK);
    return sockfd;
}

/**
 * @brief 向指定主机和资源发送 HTTP GET 请求并接收完整响应
 *
 * @param hostname 目标主机名（用于 Host 头）
 * @param resource 要请求的资源路径（如 "/index.html"）
 * @return 返回包含 HTTP 响应的完整字符串，调用者需 free()；失败时返回 NULL
 */
char * http_send_request(const char *hostname, const char *resource)
{
    char *ip = host_to_ip(hostname);
    int sockfd = http_create_socket(ip);
    if (sockfd < 0)
    {
        return NULL;
    }

    char buffer[BUFFER_SIZE] = {0};
    // 构造 GET 请求报文
    sprintf(buffer,
            "GET %s %s\r\n"
            "Host: %s\r\n"
            "Connection: %s\r\n\r\n",
            resource, HTTP_VERSION,
            hostname, CONNECTION_TYPE);

    send(sockfd, buffer, strlen(buffer), 0);

    // 使用 select 等待可读
    fd_set fdread;
    FD_ZERO(&fdread);
    FD_SET(sockfd, &fdread);

    struct timeval tv;
    tv.tv_sec = 5;    /**< 等待最久 5 秒 */
    tv.tv_usec = 0;

    // 初始分配少量空间
    char *result = malloc(sizeof(int));
    memset(result, 0, sizeof(int));

    while (1)
    {
        int selection = select(sockfd + 1, &fdread, NULL, NULL, &tv);
        if (!selection || !FD_ISSET(sockfd, &fdread))
        {
            printf("timeout\n");
            break;
        }
        else
        {
            memset(buffer, 0, BUFFER_SIZE);
            int len = recv(sockfd, buffer, BUFFER_SIZE, 0);
            if (len == 0)
            {
                // 对端关闭
                break;
            }
            // 扩展缓冲区并追加
            result = realloc(result, (strlen(result) + len + 1) * sizeof(char));
            strncat(result, buffer, len);
        }
    }
    close(sockfd);
    return result;
}

/**
 * @brief 程序入口
 *
 * @param argc 参数个数，至少 3（程序名、主机名、资源路径）
 * @param argv 参数数组，argv[1]=hostname，argv[2]=resource
 * @return 成功返回 0，失败返回 -1
 */
int main(int argc, char *argv[])
{
    if (argc < 3)
        return -1;

    char *response = http_send_request(argv[1], argv[2]);
    printf("response :%s\n", response);
    free(response);
    return 0;
}
