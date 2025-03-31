/* 编译命令：gcc -o context_switch context_switch.c -Wall -Wextra -ldl */

// 必须定义_GNU_SOURCE宏以使用ucontext相关函数
#define _GNU_SOURCE
#include <stdio.h>
#include <ucontext.h>  // 协程上下文操作库

ucontext_t ctx[3];     // 定义三个协程上下文
ucontext_t main_ctx;   // 主程序上下文
int count = 0;         // 共享计数器（存在竞态条件风险，需原子操作或锁保护）

/*----------------------------------------------------------
  协程1函数：循环打印1和4，切换至协程2
----------------------------------------------------------*/
void fun1() {
    while(count++ < 30) {  // 竞态条件：多个协程共享修改count
        printf("1\n");
        swapcontext(&ctx[0], &ctx[1]); // 保存当前上下文，切换到协程2
        printf("4\n");                 // 恢复执行时继续此处
    }
}

/*----------------------------------------------------------
  协程2函数：循环打印2和5，切换至协程3
----------------------------------------------------------*/
void fun2() {
    while(count++ < 30) {
        printf("2\n");
        swapcontext(&ctx[1], &ctx[2]); // 切换到协程3
        printf("5\n");
    }
}

/*----------------------------------------------------------
  协程3函数：循环打印3和6，切换至协程1
----------------------------------------------------------*/
void fun3() {
    while(count++ < 30) {
        printf("3\n");
        swapcontext(&ctx[2], &ctx[0]); // 切换回协程1
        printf("6\n");
    }
}

/*----------------------------------------------------------
  主函数（初始化协程并启动调度）
----------------------------------------------------------*/
int main() {
    // 为每个协程分配独立的栈空间（已修复栈共享问题）
    char stack1[2048] = {0};
    char stack2[2048] = {0};
    char stack3[2048] = {0};

    // 初始化协程1上下文
    getcontext(&ctx[0]);
    ctx[0].uc_stack.ss_size = sizeof(stack1);
    ctx[0].uc_stack.ss_sp = stack1;
    ctx[0].uc_link = &main_ctx;  // 协程结束后返回主程序
    makecontext(&ctx[0], fun1, 0);

    // 初始化协程2上下文
    getcontext(&ctx[1]);
    ctx[1].uc_stack.ss_size = sizeof(stack2);
    ctx[1].uc_stack.ss_sp = stack2;
    ctx[1].uc_link = &main_ctx;
    makecontext(&ctx[1], fun2, 0);

    // 初始化协程3上下文
    getcontext(&ctx[2]);
    ctx[2].uc_stack.ss_size = sizeof(stack3);
    ctx[2].uc_stack.ss_sp = stack3;
    ctx[2].uc_link = &main_ctx;
    makecontext(&ctx[2], fun3, 0);

    // 启动调度：切换到协程1
    printf("swapcontext\n");
    swapcontext(&main_ctx, &ctx[0]); 

    // 所有协程结束后回到此处
    printf("\n");
    return 0;
}