#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>

#define THREAD_COUNT 10

#define WITHOUT_LOCK 0
#define MUTEX_FLAG 0
#define SPINLOCK_FLAG 0
#define INC_FLAG 1

#if MUTEX_FLAG
    pthread_mutex_t mutex;
#elif SPINLOCK_FLAG
    pthread_spinlock_t spinlock;
#endif


int inc(int *value, int add){
    int old;
    __asm__ volatile(
        "lock; xaddl %2, %1"
        : "=a"(old)
        : "m"(*value), "a"(add)
        : "cc", "memory"
    );
}

void *thread_callback(void *arg)
{
    int *pcount = (int *)arg;

    int i = 0;

    while (i++ < 100000)
    {
#if WITHOUT_LOCK
        (*pcount)++;
#elif MUTEX_FLAG
        pthread_mutex_lock(&mutex);
        (*pcount)++;
        pthread_mutex_unlock(&mutex);
#elif SPINLOCK_FLAG
        pthread_spin_lock(&spinlock);
        (*pcount)++;
        pthread_spin_unlock(&spinlock);
#elif INC_FLAG
        inc(pcount, 1);
#endif
        usleep(1);
    }
}

int main()
{
    pthread_t threadid[THREAD_COUNT] = {0};

    int i = 0;
    int count = 0;
#if MUTEX_FLAG
    pthread_mutex_init(&mutex, NULL);
#elif SPINLOCK_FLAG
    pthread_spin_init(&spinlock, PTHREAD_PROCESS_SHARED);
#endif
    for (i = 0; i < THREAD_COUNT; i++)
    {
        pthread_create(&threadid[i], NULL, thread_callback, &count);
    }
    for (i = 0; i < 100; i++)
    {
        printf("count = %d\n", count);
        sleep(1);
    }
    return 0;
}
