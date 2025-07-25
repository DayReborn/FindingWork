#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>

#define MAX_MSG_LENGTH       1024
#define TIME_SUB_MS(tv1, tv2)  ((tv1.tv_sec - tv2.tv_sec) * 1000 + (tv1.tv_usec - tv2.tv_usec) / 1000)

// 发送消息到服务器
int send_msg(int connfd, char *msg, int length) {
    int res = send(connfd, msg, length, 0);
    if (res < 0) {
        perror("send");
        exit(1);
    }
    return res;
}

// 从服务器接收消息
int recv_msg(int connfd, char *msg, int length) {
    int res = recv(connfd, msg, length, 0);
    if (res < 0) {
        perror("recv");
        exit(1);
    }
    msg[res] = '\0';  // 确保字符串正确终止
    return res;
}

// 测试用例执行函数
void testcase(int connfd, char *msg, char *pattern, char *casename) {
    if (!msg || !pattern || !casename) return;

    printf("执行测试: %s\n", casename);
    printf("发送命令: %s\n", msg);
    
    send_msg(connfd, msg, strlen(msg));

    char result[MAX_MSG_LENGTH] = {0};
    recv_msg(connfd, result, MAX_MSG_LENGTH);
    
    printf("期望结果: %s\n", pattern);
    printf("实际结果: %s\n", result);

    if (strcmp(result, pattern) == 0) {
        printf("==> \033[32mPASS\033[0m ->  %s\n\n", casename);
    } else {
        printf("==> \033[31mFAILED\033[0m -> %s, '%s' != '%s' \n\n", 
               casename, result, pattern);
        exit(1);
    }
}

// 连接到TCP服务器
int connect_tcpserver(const char *ip, unsigned short port) {
    int connfd = socket(AF_INET, SOCK_STREAM, 0);
    if (connfd < 0) {
        perror("socket creation failed");
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_in));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    // 设置连接超时
    struct timeval timeout;
    timeout.tv_sec = 1;  // 1秒超时
    timeout.tv_usec = 0;
    setsockopt(connfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(connfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    if (connect(connfd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_in)) != 0) {
        perror("connect failed");
        close(connfd);
        return -1;
    }
    
    return connfd;
}

// 基本功能测试用例
void array_testcase(int connfd) {
    printf("\n=== 开始基本功能测试 ===\n\n");

    testcase(connfd, "SET Teacher King", "OK\r\n", "SET-Teacher");
    testcase(connfd, "GET Teacher", "King\r\n", "GET-Teacher");
    testcase(connfd, "MOD Teacher Darren", "OK\r\n", "MOD-Teacher");
    testcase(connfd, "GET Teacher", "Darren\r\n", "GET-Teacher");
    testcase(connfd, "EXIST Teacher", "EXIST\r\n", "EXIST-Teacher");
    testcase(connfd, "DEL Teacher", "OK\r\n", "DEL-Teacher");
    testcase(connfd, "GET Teacher", "NOT FOUND\r\n", "GET-Teacher-NotExist");
    testcase(connfd, "MOD Teacher KING", "NOT FOUND\r\n", "MOD-Teacher-NotExist");
    testcase(connfd, "EXIST Teacher", "NOT EXIST\r\n", "EXIST-Teacher-NotExist");

    printf("基本功能测试全部通过！\n");
}

// 压力测试用例 - 1万次操作
void array_testcase_1w(int connfd) {
    printf("\n=== 开始压力测试（1万次） ===\n\n");

    int count = 10000;
    int i = 0;
    int success_ops = 0;
    int failed_ops = 0;

    struct timeval tv_begin;
    gettimeofday(&tv_begin, NULL);

    for (i = 0; i < count; i++) {
        // 定义测试数据
        char set_cmd[128], get_cmd[128], mod_cmd[128], exist_cmd[128], del_cmd[128];
        char value[32], new_value[32];
        
        snprintf(set_cmd, sizeof(set_cmd), "SET key%d value%d", i, i);
        snprintf(get_cmd, sizeof(get_cmd), "GET key%d", i);
        snprintf(mod_cmd, sizeof(mod_cmd), "MOD key%d newvalue%d", i, i);
        snprintf(exist_cmd, sizeof(exist_cmd), "EXIST key%d", i);
        snprintf(del_cmd, sizeof(del_cmd), "DEL key%d", i);
        snprintf(value, sizeof(value), "value%d\r\n", i);
        snprintf(new_value, sizeof(new_value), "newvalue%d\r\n", i);

        // 执行测试序列
        try {
            testcase(connfd, set_cmd, "OK\r\n", "SET");
            testcase(connfd, get_cmd, value, "GET");
            testcase(connfd, mod_cmd, "OK\r\n", "MOD");
            testcase(connfd, get_cmd, new_value, "GET-After-MOD");
            testcase(connfd, exist_cmd, "EXIST\r\n", "EXIST");
            testcase(connfd, del_cmd, "OK\r\n", "DEL");

            success_ops += 6;
        } catch(...) {
            failed_ops += 1;
            printf("测试序列在第 %d 轮失败\n", i);
        }

        // 每100轮显示进度
        if (i % 100 == 0) {
            printf("进度: %d/%d (成功: %d, 失败: %d)\n", 
                   i, count, success_ops, failed_ops);
        }
    }

    struct timeval tv_end;
    gettimeofday(&tv_end, NULL);

    int time_used = TIME_SUB_MS(tv_end, tv_begin); // ms

    printf("\n压力测试完成：\n");
    printf("- 总操作数: %d\n", count * 6);
    printf("- 成功操作: %d\n", success_ops);
    printf("- 失败操作: %d\n", failed_ops);
    printf("- 总耗时: %d 毫秒\n", time_used);
    printf("- QPS: %d\n", (success_ops * 1000) / time_used);
    printf("- 平均响应时间: %.3f 毫秒\n", (float)time_used / (success_ops + failed_ops));
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("使用方法: %s <ip> <port> <mode>\n", argv[0]);
        printf("mode: 0 - 基本功能测试\n");
        printf("      1 - 压力测试(1w次操作)\n");
        return -1;
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);
    int mode = atoi(argv[3]);

    printf("连接到服务器 %s:%d ...\n", ip, port);
    int connfd = connect_tcpserver(ip, port);
    if (connfd < 0) {
        printf("连接服务器失败！\n");
        return -1;
    }
    printf("连接成功！\n");

    switch(mode) {
        case 0:
            array_testcase(connfd);
            break;
        case 1:
            array_testcase_1w(connfd);
            break;
        default:
            printf("不支持的测试模式: %d\n", mode);
            break;
    }

    close(connfd);
    return 0;
}
