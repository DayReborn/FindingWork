#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

/// 链表插入宏（头插法）
#define LIST_INSERT(item, list) \
    do { \
        if ((list) != NULL) (list)->prev = (item); \
        (item)->prev = NULL; \
        (item)->next = (list); \
        (list) = (item); \
    } while (0)

/// 链表移除宏
#define LIST_REMOVE(item, list) \
    do { \
        if (item->prev != NULL) item->prev->next = item->next; \
        if (item->next != NULL) item->next->prev = item->prev; \
        if (list == item) list = item->next; \
        item->prev = NULL; \
        item->next = NULL; \
    } while (0)

/// @brief 任务结构体
typedef struct nTask {
    void (*task_func)(void *arg); ///< 任务函数指针
    void *user_data;              ///< 用户自定义数据参数

    struct nTask *next, *prev;
} nTask;

/// @brief 工作线程结构体
typedef struct nWorker {
    pthread_t threadid;           ///< 线程 ID
    int terminate;                ///< 是否终止线程

    struct nManager *manager;     ///< 指向线程池结构体

    struct nWorker *next, *prev;
} nWorker;

/// @brief 线程池结构体
typedef struct nManager {
    struct nTask *tasks;          ///< 任务链表
    struct nWorker *workers;      ///< 线程链表

    pthread_mutex_t mutex;        ///< 互斥锁
    pthread_cond_t cond;          ///< 条件变量
} ThreadPool, nManager;

/**
 * @brief 工作线程函数
 * 
 * 每个线程启动后，进入循环，等待并执行任务。
 * 
 * @param arg 指向 nWorker 的指针
 * @return void* 
 */
void *nThreadPoolCallback(void *arg) {
    nWorker *worker = (nWorker *)arg;

    while (1) {
        pthread_mutex_lock(&worker->manager->mutex);

        // 如果没有任务，等待条件变量
        while (worker->manager->tasks == NULL) {
            if (worker->terminate) break;
            pthread_cond_wait(&worker->manager->cond, &worker->manager->mutex);
        }

        if (worker->terminate) {
            pthread_mutex_unlock(&worker->manager->mutex);
            break;
        }

        // 从队列中获取任务
        nTask *task = worker->manager->tasks;
        LIST_REMOVE(task, worker->manager->tasks);

        pthread_mutex_unlock(&worker->manager->mutex);

        task->task_func(task); // 执行任务
    }

    free(worker);
    return NULL;
}

/**
 * @brief 创建线程池
 * 
 * 初始化线程池并启动若干工作线程
 * 
 * @param pool 线程池结构体指针
 * @param numWorkers 线程数量
 * @return int 0 成功，非0失败
 */
int nThreadPoolCreate(ThreadPool *pool, int numWorkers) {
    if (pool == NULL) return -1;
    if (numWorkers < 1) numWorkers = 1;

    memset(pool, 0, sizeof(ThreadPool));

    // 初始化条件变量和互斥锁
    pthread_cond_t blank_cond = PTHREAD_COND_INITIALIZER;
    memcpy(&pool->cond, &blank_cond, sizeof(pthread_cond_t));

    pthread_mutex_t blank_mutex = PTHREAD_MUTEX_INITIALIZER;
    memcpy(&pool->mutex, &blank_mutex, sizeof(pthread_mutex_t));

    // 创建工作线程
    for (int i = 0; i < numWorkers; i++) {
        nWorker *worker = (nWorker *)malloc(sizeof(nWorker));
        if (!worker) return -2;

        memset(worker, 0, sizeof(nWorker));
        worker->manager = pool;

        if (pthread_create(&worker->threadid, NULL, nThreadPoolCallback, worker) != 0) {
            free(worker);
            return -3;
        }

        LIST_INSERT(worker, pool->workers);
    }

    return 0;
}

/**
 * @brief 销毁线程池
 * 
 * 设置终止标志，并唤醒所有线程
 * 
 * @param pool 线程池指针
 * @return int 0 成功
 */
int nThreadPoolDestroy(ThreadPool *pool, int numWorkers) {
    for (nWorker *worker = pool->workers; worker != NULL; worker = worker->next) {
        worker->terminate = 1;
    }

    pthread_mutex_lock(&pool->mutex);
    pthread_cond_broadcast(&pool->cond); // 唤醒所有线程
    pthread_mutex_unlock(&pool->mutex);

    pool->workers = NULL;
    pool->tasks = NULL;

    return 0;
}

/**
 * @brief 将任务推入线程池
 * 
 * @param pool 线程池
 * @param task 新任务
 * @return int 0 成功
 */
int nThreadPoolPushTask(ThreadPool *pool, nTask *task) {
    pthread_mutex_lock(&pool->mutex);
    LIST_INSERT(task, pool->tasks);
    pthread_cond_signal(&pool->cond); // 唤醒一个线程
    pthread_mutex_unlock(&pool->mutex);
    return 0;
}

/// 测试常量
#define THREAD_INIT_COUNT 20
#define TASK_INIT_SIZE 1000

/**
 * @brief 任务入口函数
 * 
 * @param arg 指向任务结构体
 */
void task_entry(void *arg) {
    nTask *task = (nTask *)arg;
    int idx = *(int *)task->user_data;
    printf("任务编号：%d\n", idx);
    free(task->user_data);
    free(task);
}

/**
 * @brief 主函数入口
 * 
 * 初始化线程池并推送多个任务
 * 
 * @return int 
 */
int main(void) {
    ThreadPool pool;
    nThreadPoolCreate(&pool, THREAD_INIT_COUNT);

    for (int i = 0; i < TASK_INIT_SIZE; ++i) {
        nTask *task = (nTask *)malloc(sizeof(nTask));
        memset(task, 0, sizeof(nTask));

        task->task_func = task_entry;
        task->user_data = malloc(sizeof(int));
        *(int *)task->user_data = i;

        nThreadPoolPushTask(&pool, task);
    }

    getchar(); // 阻塞防止线程退出
    return 0;
}
