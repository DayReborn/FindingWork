#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <ucontext.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <assert.h>
#include <arpa/inet.h> // 添加inet_ntop支持

// 协程状态枚举
typedef enum {
    STATE_READY,
    STATE_WAIT,
    STATE_SLEEP
} coroutine_state;

// 协程结构体
typedef struct coroutine {
    ucontext_t ctx;          // 上下文
    char stack[128 * 1024];    // 128KB栈
    int fd;                  // 关联的文件描述符
    coroutine_state state;   // 当前状态
    struct timespec expire;  // 唤醒时间戳
    struct coroutine *prev, *next; // 链表指针
} coroutine;

// 调度器结构体
typedef struct scheduler {
    int epfd;                  // epoll实例
    struct epoll_event *events; // epoll事件数组
    int event_size;            // 事件数组大小
    
    coroutine *ready_head;     // 就绪队列头
    coroutine *ready_tail;     // 就绪队列尾
    coroutine *wait_root;      // 等待树根节点
    coroutine *sleep_root;     // 睡眠树根节点
    coroutine *current;        // 当前运行的协程
    ucontext_t sched_ctx;      // 调度器上下文
} scheduler;

// 全局调度器实例
scheduler global_sched;

// 系统函数Hook声明
typedef ssize_t (*read_t)(int fd, void *buf, size_t count);
typedef ssize_t (*write_t)(int fd, const void *buf, size_t count);
typedef int (*accept_t)(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
typedef int (*close_t)(int fd);
typedef int (*poll_t)(struct pollfd *fds, nfds_t nfds, int timeout);

read_t sys_read = NULL;
write_t sys_write = NULL;
accept_t sys_accept = NULL;
close_t sys_close = NULL;
poll_t sys_poll = NULL;

// 链表操作函数
void add_to_ready(coroutine *co) {
    co->state = STATE_READY;
    co->next = NULL;
    if (!global_sched.ready_head) {
        global_sched.ready_head = co;
        global_sched.ready_tail = co;
        co->prev = NULL;
    } else {
        global_sched.ready_tail->next = co;
        co->prev = global_sched.ready_tail;
        global_sched.ready_tail = co;
    }
}

coroutine *pop_ready() {
    if (!global_sched.ready_head) return NULL;
    
    coroutine *co = global_sched.ready_head;
    global_sched.ready_head = co->next;
    if (global_sched.ready_head) {
        global_sched.ready_head->prev = NULL;
    } else {
        global_sched.ready_tail = NULL;
    }
    co->prev = co->next = NULL;
    return co;
}

void remove_from_ready(coroutine *co) {
    if (co->prev) co->prev->next = co->next;
    if (co->next) co->next->prev = co->prev;
    if (co == global_sched.ready_head) global_sched.ready_head = co->next;
    if (co == global_sched.ready_tail) global_sched.ready_tail = co->prev;
    co->prev = co->next = NULL;
}

void insert_wait(coroutine *co) {
    co->state = STATE_WAIT;
    if (!global_sched.wait_root) {
        global_sched.wait_root = co;
        co->prev = NULL;
        co->next = NULL;
    } else {
        co->next = global_sched.wait_root;
        global_sched.wait_root->prev = co;
        global_sched.wait_root = co;
        co->prev = NULL;
    }
}

void remove_wait(coroutine *co) {
    if (co->prev) co->prev->next = co->next;
    if (co->next) co->next->prev = co->prev;
    if (co == global_sched.wait_root) global_sched.wait_root = co->next;
    co->prev = co->next = NULL;
}

void coroutine_yield() {
    coroutine *curr = global_sched.current;
    swapcontext(&curr->ctx, &global_sched.sched_ctx);
}

// 协程挂起并等待指定fd可读
void wait_for_read(int fd) {
    global_sched.current->fd = fd;
    insert_wait(global_sched.current);
    
    // 添加到epoll监听
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = fd;
    epoll_ctl(global_sched.epfd, EPOLL_CTL_ADD, fd, &ev);
    
    coroutine_yield();
}

// 创建新协程
coroutine *create_coroutine(void (*func)(void *), void *arg) {
    coroutine *co = malloc(sizeof(coroutine));
    if (!co) return NULL;
    
    if (getcontext(&co->ctx) == -1) {
        perror("getcontext");
        free(co);
        return NULL;
    }
    
    co->ctx.uc_stack.ss_sp = co->stack;
    co->ctx.uc_stack.ss_size = sizeof(co->stack);
    co->ctx.uc_link = &global_sched.sched_ctx;
    
    makecontext(&co->ctx, (void (*)(void))func, 1, arg);
    
    co->fd = -1;
    add_to_ready(co);
    
    return co;
}

// Hook的读函数
ssize_t read(int fd, void *buf, size_t count) {
    if (!sys_read) sys_read = dlsym(RTLD_NEXT, "read");
    
    while (1) {
        ssize_t n = sys_read(fd, buf, count);
        if (n >= 0) return n;
        
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // 数据未就绪，挂起协程
            wait_for_read(fd);
        } else {
            return -1;
        }
    }
}

// Hook的写函数
ssize_t write(int fd, const void *buf, size_t count) {
    if (!sys_write) sys_write = dlsym(RTLD_NEXT, "write");
    
    while (1) {
        ssize_t n = sys_write(fd, buf, count);
        if (n >= 0) return n;
        
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // 缓冲区满，挂起协程
            global_sched.current->fd = fd;
            insert_wait(global_sched.current);
            
            // 添加到epoll监听
            struct epoll_event ev;
            ev.events = EPOLLOUT | EPOLLET;
            ev.data.fd = fd;
            epoll_ctl(global_sched.epfd, EPOLL_CTL_ADD, fd, &ev);
            
            coroutine_yield();
        } else {
            return -1;
        }
    }
}

// Hook的accept函数
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    if (!sys_accept) sys_accept = dlsym(RTLD_NEXT, "accept");
    
    while (1) {
        int clientfd = sys_accept(sockfd, addr, addrlen);
        if (clientfd >= 0) {
            // 设置非阻塞
            fcntl(clientfd, F_SETFL, fcntl(clientfd, F_GETFL) | O_NONBLOCK);
            return clientfd;
        }
        
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // 无连接，挂起协程
            wait_for_read(sockfd);
        } else {
            perror("accept failed");
            return -1;
        }
    }
}

// Hook的close函数
int close(int fd) {
    if (!sys_close) sys_close = dlsym(RTLD_NEXT, "close");
    
    // 从epoll监听移除
    epoll_ctl(global_sched.epfd, EPOLL_CTL_DEL, fd, NULL);
    
    // 从等待树移除
    coroutine *curr = global_sched.wait_root;
    while (curr) {
        if (curr->fd == fd) {
            remove_wait(curr);
            add_to_ready(curr);
            break;
        }
        curr = curr->next;
    }
    
    return sys_close(fd);
}

// Hook的poll函数
int poll(struct pollfd *fds, nfds_t nfds, int timeout) {
    if (!sys_poll) sys_poll = dlsym(RTLD_NEXT, "poll");
    
    // 简化处理：使用非阻塞轮询
    return sys_poll(fds, nfds, 0);
}

// 初始化调度器
void init_scheduler() {
    global_sched.epfd = epoll_create1(0);
    if (global_sched.epfd == -1) {
        perror("epoll_create1");
        exit(1);
    }
    
    global_sched.event_size = 1024;
    global_sched.events = malloc(global_sched.event_size * sizeof(struct epoll_event));
    if (!global_sched.events) {
        perror("malloc");
        exit(1);
    }
    
    global_sched.ready_head = NULL;
    global_sched.ready_tail = NULL;
    global_sched.wait_root = NULL;
    global_sched.sleep_root = NULL;
    global_sched.current = NULL;
    
    if (getcontext(&global_sched.sched_ctx) == -1) {
        perror("getcontext");
        exit(1);
    }
}

// 调度器主循环
void schedule_loop() {
    while (1) {
        // 处理就绪队列
        while (global_sched.ready_head) {
            coroutine *co = pop_ready();
            if (!co) break;
            
            global_sched.current = co;
            swapcontext(&global_sched.sched_ctx, &co->ctx);
            global_sched.current = NULL;
        }
        
        // 无协程可执行时阻塞等待事件
        int n = epoll_wait(global_sched.epfd, global_sched.events, global_sched.event_size, 100);
        if (n < 0) {
            if (errno == EINTR) continue;
            perror("epoll_wait");
            break;
        }
        
        // 处理事件
        for (int i = 0; i < n; i++) {
            int fd = global_sched.events[i].data.fd;
            uint32_t events = global_sched.events[i].events;
            
            // 在等待树中查找对应fd的协程
            coroutine *co = global_sched.wait_root;
            while (co) {
                if (co->fd == fd) {
                    // 从等待树移除
                    remove_wait(co);
                    
                    // 添加到就绪队列
                    add_to_ready(co);
                    
                    // 移除epoll监听
                    epoll_ctl(global_sched.epfd, EPOLL_CTL_DEL, fd, NULL);
                    break;
                }
                co = co->next;
            }
        }
    }
}

// 客户端连接处理协程
void client_handler(void *arg) {
    int clientfd = *(int*)arg;
    free(arg);
    
    char client_ip[INET_ADDRSTRLEN];
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    getpeername(clientfd, (struct sockaddr*)&addr, &addr_len);
    inet_ntop(AF_INET, &addr.sin_addr, client_ip, sizeof(client_ip));
    int client_port = ntohs(addr.sin_port);
    
    printf("New client connected: %s:%d\n", client_ip, client_port);
    
    while (1) {
        char buffer[128] = {0};
        ssize_t count = read(clientfd, buffer, sizeof(buffer));
        if (count <= 0) break;
        
        printf("From %s:%d: %s", client_ip, client_port, buffer);
        write(clientfd, buffer, count);
    }
    
    close(clientfd);
    printf("Client disconnected: %s:%d\n", client_ip, client_port);
}

// 接受连接的主协程
void accept_handler(void *arg) {
    int sockfd = *(int*)arg;
    free(arg);
    
    printf("Accept handler started for socket %d\n", sockfd);
    
    while (1) {
        int clientfd = accept(sockfd, NULL, NULL);
        if (clientfd < 0) {
            perror("accept");
            continue;
        }
        
        // 创建新协程处理客户端
        int *arg = malloc(sizeof(int));
        if (!arg) {
            perror("malloc");
            close(clientfd);
            continue;
        }
        *arg = clientfd;
        
        if (!create_coroutine(client_handler, arg)) {
            perror("create_coroutine");
            close(clientfd);
            free(arg);
        }
    }
}

int main() {
    // 打印启动消息
    printf("Starting coroutine-based TCP server...\n");
    
    // 初始化调度器
    init_scheduler();
    
    // 创建监听socket
    int sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }
    
    // 设置地址重用
    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt");
        close(sockfd);
        exit(1);
    }
    
    // 绑定地址
    struct sockaddr_in serveraddr = {0};
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(9999);
    
    if (bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        perror("bind");
        close(sockfd);
        exit(1);
    }
    
    // 监听
    if (listen(sockfd, 1024) < 0) {
        perror("listen");
        close(sockfd);
        exit(1);
    }
    
    printf("Server listening on port 9999\n");
    printf("IP addresses: ");
    
    // 获取并打印所有本地IP地址
    system("/sbin/ip -o -4 addr list | awk \'{print $4}\' | cut -d\'/\' -f1");
    printf("\n");
    
    // 创建接受连接协程
    int *arg = malloc(sizeof(int));
    if (!arg) {
        perror("malloc");
        close(sockfd);
        exit(1);
    }
    *arg = sockfd;
    
    if (!create_coroutine(accept_handler, arg)) {
        perror("create_coroutine");
        close(sockfd);
        free(arg);
        exit(1);
    }
    
    // 启动调度器
    schedule_loop();
    
    // 清理（正常情况下不会到达这里）
    close(sockfd);
    return 0;
}