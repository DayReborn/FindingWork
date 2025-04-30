#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define LIST_INSERT(item, list)    \
    do                             \
    {                              \
        if ((list) != NULL)        \
            (list)->prev = (item); \
        (item)->prev = NULL;       \
        (item)->next = (list);     \
        (list) = (item);           \
    } while (0)

#define LIST_REMOVE(item, list)            \
    do                                     \
    {                                      \
        if (item->prev != NULL)            \
            item->prev->next = item->next; \
        if (item->next != NULL)            \
            item->next->prev = item->prev; \
        if (list == item)                  \
            list = item->next;             \
        item->prev = NULL;                 \
        item->next = NULL;                 \
    } while (0)

typedef struct nTask
{
    void (*task_func)(void *arg);
    void *user_data;

    nTask *next;
    nTask *prev;
} nTask;

typedef struct nWorker
{
    pthread_t threadid;

    nWorker *next;
    nWorker *prev;
} nWorker;

typedef struct nManager
{
    nTask *tasks;
    nWorker *workers;

    pthread_mutex_t mutex;
    pthread_cond_t cond;
} ThreadPool, nManager;

// 创建一个线程池
void *nThreadPoolCallback(void *arg)
{
}

int nThreadPoolCreate(ThreadPool *pool, int nWorker)
{
    if(pool == NULL)
        return -1;
    if(nWorker<1) nWorker = 1;
    
    // function

    // return

}

int nThreadPoolDestroy(ThreadPool *pool, int nWorker)
{
}

int nThreadPoolPushTask(ThreadPool *pool, nTask *task)
{
}