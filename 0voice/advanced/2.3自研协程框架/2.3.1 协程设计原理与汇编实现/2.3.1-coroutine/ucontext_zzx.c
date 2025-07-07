#define _GNU_SOURCE

#include <dlfcn.h> // 用于动态链接库函数的声明

#include <stdio.h>    // 用于标准输入输出函数
#include <ucontext.h> // 用于上下文切换相关的函数和结构体
#include <string.h>   // 用于字符串处理函数
#include <unistd.h>   // 用于POSIX操作系统API函数
#include <fcntl.h>    // 用于文件控制操作函数

ucontext_t ctx[3];
ucontext_t main_ctx;

int count = 0;

void func1(void)
{
	while (count ++ < 30) {
		printf("1\n");
		swapcontext(&ctx[0], &ctx[1]);
		//swapcontext(&ctx[0], &main_ctx);
		printf("4\n");
	}
}

void func2(void)
{
	while (count ++ < 30) {
		printf("2\n");
		swapcontext(&ctx[1], &ctx[2]);
		//swapcontext(&ctx[1], &main_ctx);
		printf("5\n");
	}
}

void func3(void)
{
	while (count ++ < 30) {
		printf("3\n");
		swapcontext(&ctx[2], &ctx[0]);
		//swapcontext(&ctx[2], &main_ctx);
		printf("6\n");
	}
}


int main()
{
    char stack1[2048];
    char stack2[2048];
    char stack3[2048];

    // 初始化上下文
    memset(&main_ctx, 0, sizeof(main_ctx));

    // 创建协程1
    memset(stack1, 0, sizeof(stack1));
    getcontext(&ctx[0]);
    ctx[0].uc_stack.ss_sp = stack1; // 设置协程1的栈空间
    ctx[0].uc_stack.ss_size = sizeof(stack1); // 设置协程1的栈大小
    ctx[0].uc_link = &main_ctx; // 设置协程1的链接上下文
    // makecontext函数用于将func1函数与ctx[0]关联
    makecontext(&ctx[0], func1, 0);

    // 创建协程2
    memset(stack2, 0, sizeof(stack2));
    getcontext(&ctx[1]);
    ctx[1].uc_stack.ss_sp = stack2;
    ctx[1].uc_stack.ss_size = sizeof(stack2);
    ctx[1].uc_link = &main_ctx;
    makecontext(&ctx[1], func2, 0);

    // 创建协程3
    memset(stack3, 0, sizeof(stack3));
    getcontext(&ctx[2]);
    ctx[2].uc_stack.ss_sp = stack3;
    ctx[2].uc_stack.ss_size = sizeof(stack3);
    ctx[2].uc_link = &main_ctx;
    makecontext(&ctx[2], func3, 0);

    // 启动协程1
    printf("main start\n");
    swapcontext(&main_ctx, &ctx[0]);
    printf("main end\n");
    return 0;
}