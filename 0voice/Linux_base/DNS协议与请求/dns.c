#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define DNS_SERVER_PORT 53
#define DNS_SERVER_IP   "114.114.114.114"

#define DNS_HOST  0x01
#define DNS_CNAME 0x05

/**
 * DNS 报文头结构体
 */
typedef struct dns_header
{
    unsigned short id;         ///< 报文标识
    unsigned short flags;      ///< 标志位
    unsigned short questions;  ///< 问题数
    unsigned short answers;    ///< 回答数
    unsigned short authority;  ///< 授权记录数
    unsigned short additional; ///< 附加记录数
} dns_header_t;

typedef struct dns_question
{
    int length;           ///< 域名部分长度（字节数）
    unsigned short qtype; ///< 查询类型
    unsigned short qclass;///< 查询类
    unsigned char *name;  ///< 查询的域名（以 DNS 格式存储）
} dns_question_t;

typedef struct dns_item
{
    char *domain; ///< 解析到的域名
    char *ip;     ///< 对应 IP 地址
} dns_item_t;

/**
 * @brief 初始化 DNS 报文头
 * @param[out] header 指向待初始化的 dns_header_t 结构
 * @return 0 成功；-1 参数无效
 */
int dns_create_header(dns_header_t *header)
{
    if (header == NULL)
    {
        printf("Invalid arguments\n");
        return -1;
    }
    memset(header, 0, sizeof(dns_header_t));

    srandom(time(NULL));
    header->id        = random() % 65536;  // 随机事务 ID
    header->flags     = htons(0x0100);     // 标准查询
    header->questions = htons(1);          // 一个问题
    header->answers   = htons(0);
    header->authority = htons(0);
    header->additional= htons(0);
    return 0;
}

/**
 * @brief 构造 DNS 查询部分
 * @param[out] question 指向待填充的 dns_question_t 结构
 * @param[in] hostname 要查询的主机名（如 "www.example.com"）
 * @return 0 成功；-1 参数无效；-2 内存分配失败
 */
int dns_create_question(dns_question_t *question, const char *hostname)
{
    if (question == NULL || hostname == NULL)
    {
        printf("Invalid arguments\n");
        return -1;
    }
    memset(question, 0, sizeof(dns_question_t));
    question->length = strlen(hostname) + 2;
    question->name   = (unsigned char *)malloc(question->length);
    if (question->name == NULL)
    {
        printf("malloc() failed\n");
        return -2;
    }
    question->qtype  = htons(1);  // A 记录
    question->qclass = htons(1);  // IN 类

    const char delim[2] = ".";
    unsigned char *qname = question->name;
    char *hostname_dup = strdup(hostname);
    char *token = strtok(hostname_dup, delim);

    while (token != NULL)
    {
        size_t len = strlen(token);
        *qname++ = len;                       // 标签长度
        strncpy((char *)qname, token, len);   // 复制标签内容
        qname += len;
        token = strtok(NULL, delim);
    }
    free(hostname_dup);
    return 0;
}

/**
 * @brief 构建整个 DNS 请求报文
 * @param[in] header 已填充的报文头
 * @param[in] question 已填充的查询段
 * @param[out] request 输出缓冲区，用于存放完整请求
 * @return 请求长度（字节数）；-1 参数无效
 */
int dns_build_requestion(dns_header_t *header, dns_question_t *question, char *request)
{
    if (header == NULL || question == NULL || request == NULL)
    {
        printf("Invalid arguments\n");
        return -1;
    }
    memset(request, 0, sizeof(request));

    memcpy(request, header, sizeof(dns_header_t));
    int offset = sizeof(dns_header_t);

    memcpy(request + offset, question->name, question->length);
    offset += question->length;

    memcpy(request + offset, &question->qtype, sizeof(question->qtype));
    offset += sizeof(question->qtype);

    memcpy(request + offset, &question->qclass, sizeof(question->qclass));
    offset += sizeof(question->qclass);

    return offset;
}

/**
 * @brief 判断当前字节是否为指针标志
 * @param[in] in 当前字节
 * @return 非零表示是指针
 */
static int is_pointer(int in)
{
    return ((in & 0xC0) == 0xC0);
}

/**
 * @brief 递归解析 DNS 名称（处理指针压缩）
 * @param[in] chunk DNS 报文起始地址
 * @param[in] ptr 指向当前名称部分
 * @param[out] out 输出缓冲区，存放解析后的名称
 * @param[in,out] len 已解析长度
 */
static void dns_parse_name(unsigned char *chunk, unsigned char *ptr, char *out, int *len)
{
    int flag = 0, n = 0;
    char *pos = out + (*len);

    while (1)
    {
        flag = ptr[0];
        if (flag == 0) break;

        if (is_pointer(flag))
        {
            n = ptr[1];
            ptr = chunk + n;
            dns_parse_name(chunk, ptr, out, len);
            break;
        }
        else
        {
            ptr++;
            memcpy(pos, ptr, flag);
            pos += flag;
            ptr += flag;
            *len += flag;
            if (ptr[0] != 0)
            {
                *pos++ = '.';
                (*len)++;
            }
        }
    }
}

/**
 * @brief 解析 DNS 响应报文中的 Answer 部分
 * @param[in] buffer 接收到的原始报文
 * @param[out] domains 返回动态分配的 dns_item_t 数组
 * @return 解析到的记录数；-1 分配失败
 */
static int dns_parse_response(unsigned char *buffer, struct dns_item **domains)
{
    unsigned char *ptr = buffer;
    ptr += 4;
    int querys  = ntohs(*(unsigned short *)ptr);
    ptr += 2;
    int answers = ntohs(*(unsigned short *)ptr);
    ptr += 6;

    // 跳过 question 段
    for (int i = 0; i < querys; i++)
    {
        while (*ptr != 0) ptr += (*ptr + 1);
        ptr += 5;
    }

    struct dns_item *list = calloc(answers, sizeof(struct dns_item));
    if (!list) return -1;

    int cnt = 0;
    for (int i = 0; i < answers; i++)
    {
        char aname[128] = {0}, ip[20] = {0}, netip[4];
        int len = 0, type, ttl, datalen;

        dns_parse_name(buffer, ptr, aname, &len);
        ptr += 2;

        type = htons(*(unsigned short *)ptr);
        ptr += 4;
        ttl  = htons(*(unsigned short *)ptr);
        ptr += 4;
        datalen = ntohs(*(unsigned short *)ptr);
        ptr += 2;

        if (type == DNS_CNAME)
        {
            char cname[128] = {0};
            len = 0;
            dns_parse_name(buffer, ptr, cname, &len);
            ptr += datalen;
        }
        else if (type == DNS_HOST && datalen == 4)
        {
            memcpy(netip, ptr, datalen);
            inet_ntop(AF_INET, netip, ip, sizeof(ip));
            printf("%s has address %s\n", aname, ip);
            printf("\tTime to live: %d minutes , %d seconds\n", ttl/60, ttl%60);

            list[cnt].domain = strdup(aname);
            list[cnt].ip     = strdup(ip);
            cnt++;
            ptr += datalen;
        }
    }

    *domains = list;
    return cnt;
}

/**
 * @brief 执行一次 DNS 查询并打印结果
 * @param[in] domain 要查询的域名
 * @return 0 成功；-1~其他 失败
 */
int dns_client_commit(const char *domain)
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) { printf("socket() failed\n"); return -1; }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(DNS_SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(DNS_SERVER_IP);

    connect(fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    dns_header_t header  = {0};
    dns_create_header(&header);

    dns_question_t question = {0};
    dns_create_question(&question, domain);

    char request[1024] = {0};
    int length = dns_build_requestion(&header, &question, request);

    int slen = sendto(fd, request, length, 0,
                      (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (slen < 0) { printf("sendto() failed\n"); return -2; }
    printf("sendto() %d bytes\n", slen);

    char response[1024] = {0};
    int n = recvfrom(fd, response, sizeof(response), 0, NULL, NULL);
    if (n < 0) { printf("recvfrom() failed\n"); return -3; }
    printf("recvfrom() %d bytes\n", n);

    struct dns_item *domains = NULL;
    int cnt = dns_parse_response((unsigned char *)response, &domains);
    if (cnt < 0) { printf("dns_parse_response() failed\n"); return -4; }

    printf("cnt = %d\n", cnt);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 2) return -1;
    dns_client_commit(argv[1]);
}
