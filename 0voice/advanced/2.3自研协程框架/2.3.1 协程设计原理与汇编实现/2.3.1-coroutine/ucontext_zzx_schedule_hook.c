#define _GNU_SOURCE

#include <dlfcn.h> // 用于动态链接库函数的声明

#include <stdio.h>    // 用于标准输入输出函数
#include <ucontext.h> // 用于上下文切换相关的函数和结构体
#include <string.h>   // 用于字符串处理函数
#include <unistd.h>   // 用于POSIX操作系统API函数
#include <fcntl.h>    // 用于文件控制操作函数

#define BUFFER_LENGTH 128

ucontext_t ctx[3];
ucontext_t main_ctx;

int count = 0;

// hook
typedef ssize_t (*read_t)(int fd, void *buf, size_t count);
read_t read_f;

typedef ssize_t (*write_t)(int fd, const void *buf, size_t count);
write_t write_f;

ssize_t read(int fd, void *buf, size_t count)
{
    ssize_t res = read_f(fd, buf, count);

    printf("read: %s\n", (const char *)buf);

    return res;
}

ssize_t write(int fd, const void *buf, size_t count)
{
    ssize_t res = write_f(fd, buf, count);

    printf("write: %s\n", (const char *)buf);

    return res;
}

void init_hook(void)
{
    if (!read_f)
    {
        read_f = dlsym(RTLD_NEXT, "read");
    }
    if (!write_f)
    {
        write_f = dlsym(RTLD_NEXT, "write");
    }
}

void func1(void)
{
    while (count++ < 30)
    {
        printf("1\n");
        // swapcontext(&ctx[0], &ctx[1]);
        swapcontext(&ctx[0], &main_ctx);
        printf("4\n");
    }
}

void func2(void)
{
    while (count++ < 30)
    {
        printf("2\n");
        // swapcontext(&ctx[1], &ctx[2]);
        swapcontext(&ctx[1], &main_ctx);
        printf("5\n");
    }
}

void func3(void)
{
    while (count++ < 30)
    {
        printf("3\n");
        // swapcontext(&ctx[2], &ctx[0]);
        swapcontext(&ctx[2], &main_ctx);
        printf("6\n");
    }
}

// schedule
int main()
{
    init_hook();
    int fd = open("a.txt", O_CREAT | O_RDWR);
    if (fd < 0)
    {
        return -1;
    }
    char *str = "1234567890";
    write(fd, str, strlen(str));

    char buffer[BUFFER_LENGTH];
    memset(buffer, 0, BUFFER_LENGTH);

    read(fd, buffer, BUFFER_LENGTH);

    printf("buffer is: %s\n", buffer);
}