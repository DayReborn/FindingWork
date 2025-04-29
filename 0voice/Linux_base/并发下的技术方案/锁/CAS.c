#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>

#define THREAD_COUNT 10

// 自定义CAS函数（使用GCC内置原子操作）
int cas(int* ptr, int oldval, int newval) {
    return __atomic_compare_exchange_n(ptr, &oldval, newval, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

void* thread_callback(void* arg) {
    int* pcount = (int*)arg;
    int i = 0;
    while (i++ < 100000) {
        int old_val, new_val;
        do {
            old_val = *pcount;       // 读取当前值
            new_val = old_val + 1;   // 计算新值
        } while (!cas(pcount, old_val, new_val)); // 循环直到CAS成功
        usleep(1);
    }
    return NULL;
}

int main() {
    pthread_t threadid[THREAD_COUNT] = {0};
    int i = 0;
    int count = 0;

    for (i = 0; i < THREAD_COUNT; i++) {
        pthread_create(&threadid[i], NULL, thread_callback, &count);
    }

    // 输出逻辑完全不变
    for (i = 0; i < 100; i++) {
        printf("count = %d\n", count);
        sleep(1);
    }

    return 0;
}