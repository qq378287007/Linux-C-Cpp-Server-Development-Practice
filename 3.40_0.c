#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

typedef struct
{
    void (*function)(void *arg);
    void *arg;
} Task;

typedef struct
{
    Task *taskQ;       // 任务队列
    int queueCapacity; // 容量
    int queueSize;     // 当前任务个数
    int queueFront;    // 队头 -> 取数据
    int queueRear;     // 队尾 -> 放数据

    pthread_t managerID;       // 管理者线程ID
    pthread_t *threadIDs;      // 工作的线程ID
    int minNum;                // 最小线程数量
    int maxNum;                // 最大线程数量
    int busyNum;               // 忙的线程的个数
    int liveNum;               // 存活的线程的个数
    int exitNum;               // 要销毁（闲置）的线程个数，= liveNum - busyNum
    pthread_mutex_t mutexPool; // 锁整个的线程池
    pthread_mutex_t mutexBusy; // 锁busyNum变量
    pthread_cond_t notFull;    // 任务队列是不是满了
    pthread_cond_t notEmpty;   // 任务队列是不是空了

    int shutdown; // 是不是要销毁线程池, 销毁为1, 不销毁为0
} ThreadPool;

// 当前工作线程个数
int threadPoolBusyNum(ThreadPool *pool)
{
    pthread_mutex_lock(&pool->mutexBusy);
    int busyNum = pool->busyNum;
    pthread_mutex_unlock(&pool->mutexBusy);

    return busyNum;
}

// 当前活着的线程的个数
int threadPoolAliveNum(ThreadPool *pool)
{
    pthread_mutex_lock(&pool->mutexPool);
    int aliveNum = pool->liveNum;
    pthread_mutex_unlock(&pool->mutexPool);

    return aliveNum;
}

// 线程池退出
void threadExit(ThreadPool *pool)
{
    pthread_t tid = pthread_self();

    pthread_mutex_lock(&pool->mutexPool);
    for (int i = 0; i < pool->maxNum; i++)
    {
        if (pool->threadIDs[i] == tid)
        {
            pool->threadIDs[i] = 0;
            pool->liveNum--;
            printf("threadExit() called, %ld exiting...\n", tid);
            break;
        }
    }
    pthread_mutex_unlock(&pool->mutexPool);

    pthread_exit(NULL);
}

// 工作的线程(消费者线程)任务函数
void *worker(void *arg)
{
    ThreadPool *pool = (ThreadPool *)arg;
    while (1)
    {
        pthread_mutex_lock(&pool->mutexPool);

        // 当前任务队列是否为空
        while (pool->queueSize == 0 && !pool->shutdown)
        {
            // 阻塞工作线程
            pthread_cond_wait(&pool->notEmpty, &pool->mutexPool);

            // 判断是否用销毁线程
            if (pool->exitNum > 0) // 启动线程冗余
            {
                pool->exitNum--;
                if (pool->liveNum > pool->minNum)
                {
                    pool->liveNum--;
                    pthread_mutex_unlock(&pool->mutexPool);
                    threadExit(pool);
                }
            }
        }

        // 判断线程池是否被关闭
        if (pool->shutdown)
        {
            pthread_mutex_unlock(&pool->mutexPool);
            threadExit(pool);
        }

        // 从任务队列中取出一个任务
        Task task;
        task.function = pool->taskQ[pool->queueFront].function;
        task.arg = pool->taskQ[pool->queueFront].arg;

        // 移动头节点
        pool->queueFront = (pool->queueFront + 1) % pool->queueCapacity;
        pool->queueSize--;

        // 解锁
        pthread_mutex_unlock(&pool->mutexPool);
        pthread_cond_signal(&pool->notFull); // 通知线程池可以添加任务

        printf("thread %ld start working...\n", pthread_self());

        pthread_mutex_lock(&pool->mutexBusy);
        pool->busyNum++;
        pthread_mutex_unlock(&pool->mutexBusy);

        // 执行
        task.function(task.arg);
        free(task.arg);
        task.arg = NULL;

        pthread_mutex_lock(&pool->mutexBusy);
        pool->busyNum--;
        pthread_mutex_unlock(&pool->mutexBusy);

        printf("thread %ld end working...\n", pthread_self());
    }

    threadExit(pool);
    // return NULL;
}

// 管理者线程任务函数
// 动态增删线程数
void *manager(void *arg)
{
    ThreadPool *pool = (ThreadPool *)arg;
    while (!pool->shutdown)
    {
        // 3s检查一次
        sleep(3);

        // 取出线程池任务的数量和当前线程的数量
        pthread_mutex_lock(&pool->mutexPool);
        int queueSize = pool->queueSize;
        int liveNum = pool->liveNum;
        printf("liveNum = %d\n", liveNum);
        pthread_mutex_unlock(&pool->mutexPool);

        //  取出忙的线程的数量
        pthread_mutex_lock(&pool->mutexBusy);
        int busyNum = pool->busyNum;
        printf("busyNum = %d\n", busyNum);
        pthread_mutex_unlock(&pool->mutexBusy);

        // 添加线程
        // 存活的线程个数 < 任务的个数 && 存活的线程数 < 最大线程数
        if (liveNum < queueSize && liveNum < pool->maxNum)
        {
            pthread_mutex_lock(&pool->mutexPool);
            for (int i = 0; i < pool->maxNum && pool->liveNum < pool->maxNum; i++)
            {
                if (pool->threadIDs[i] == 0)
                {
                    pthread_create(&pool->threadIDs[i], NULL, worker, pool); // 创建新线程
                    pool->liveNum++;
                }
            }
            pthread_mutex_unlock(&pool->mutexPool);
        }

        // 销毁线程
        // 存活的线程数 < 忙的线程*2&& 存活的线程 > 最小线程数
        if (liveNum < busyNum * 2 && liveNum > pool->minNum)
        {
            pthread_mutex_lock(&pool->mutexPool);
            int NUMBER = pool->liveNum - pool->busyNum;
            pool->exitNum = NUMBER;
            pthread_mutex_unlock(&pool->mutexPool);

            // 发信号让线程自杀，具体实现就是每个线程的有一个销毁判断
            // 每次销毁一个，pool->exitNum--,直到为0
            for (int i = 0; i < NUMBER; i++)
                pthread_cond_signal(&pool->notEmpty);
        }
    }

    pthread_cond_broadcast(&pool->notEmpty);
    return NULL;
}

// 创建线程池并初始化
ThreadPool *threadPoolCreate(int min, int max, int queueSize)
{
    ThreadPool *pool = (ThreadPool *)malloc(sizeof(ThreadPool));
    do
    {
        if (pool == NULL)
        {
            printf("malloc ThreadPool failed\n");
            break;
        }

        pool->threadIDs = (pthread_t *)malloc(sizeof(pthread_t) * max);
        if (pool->threadIDs == NULL)
        {
            printf("malloc threadIDs failed\n");
            break;
        }
        // memset(pool->threadIDs, 0, sizeof(pthread_t) * max);
        for (int i = 0; i < max; i++)
            pool->threadIDs[i] = 0;

        pool->minNum = min;
        pool->maxNum = max;
        pool->busyNum = 0;
        pool->liveNum = min;
        pool->exitNum = 0;

        if (pthread_mutex_init(&pool->mutexPool, NULL) != 0 ||
            pthread_mutex_init(&pool->mutexBusy, NULL) != 0 ||
            pthread_cond_init(&pool->notEmpty, NULL) != 0 ||
            pthread_cond_init(&pool->notFull, NULL) != 0)
        {
            printf("mutex or cond init failed\n");
            break;
        }

        // 任务队列
        pool->taskQ = (Task *)malloc(sizeof(Task) * queueSize);
        if (pool->taskQ == NULL)
        {
            printf("malloc taskQ failed\n");
            break;
        }
        pool->queueCapacity = queueSize;
        pool->queueSize = 0;
        pool->queueFront = 0;
        pool->queueRear = 0;
        pool->shutdown = 0;

        if (pthread_create(&pool->managerID, NULL, manager, pool))
        {
            perror("create pthread_create managerID failed");
            break;
        }

        for (int i = 0; i < min; i++)
        {
            if (pthread_create(&pool->threadIDs[i], NULL, worker, pool))
            {
                perror("create pthread_create threadIDs failed");
                break;
            }
        }

        return pool;
    } while (0);

    // 初始化失败，释放资源
    if (pool && pool->threadIDs)
        free(pool->threadIDs);
    if (pool && pool->taskQ)
        free(pool->taskQ);
    if (pool)
        free(pool);
    return NULL;
}

// 线程池添加任务
void threadPoolAdd(ThreadPool *pool, void (*func)(void *), void *arg)
{
    pthread_mutex_lock(&pool->mutexPool);

    // 因为任务队列满了或者线程池需要销毁，阻塞生产者线程
    while (pool->queueSize == pool->queueCapacity && !pool->shutdown)
        pthread_cond_wait(&pool->notFull, &pool->mutexPool);

    if (pool->shutdown)
    {
        pthread_mutex_unlock(&pool->mutexPool);
        return;
    }

    // 添加任务
    pool->taskQ[pool->queueRear].function = func;
    pool->taskQ[pool->queueRear].arg = arg;
    pool->queueRear = (pool->queueRear + 1) % pool->queueCapacity;
    pool->queueSize++;

    pthread_mutex_unlock(&pool->mutexPool);
    pthread_cond_signal(&pool->notEmpty);
}

// 销毁线程池, 失败-1，成功0
int threadPoolDestroy(ThreadPool *pool)
{
    if (pool == NULL)
        return -1;

    // 关闭线程池
    // pthread_mutex_lock(&pool->mutexBusy);
    pool->shutdown = -1;
    // pthread_mutex_unlock(&pool->mutexPool);

    // 等待管理者线程结束
    pthread_join(pool->managerID, NULL);
    printf("manager thread quit!\n");

    // 唤醒阻塞的消费者线程（工作线程）
    for (int i = 0; i < pool->liveNum; i++)
        pthread_cond_signal(&pool->notEmpty);

    // 等待工作线程结束
    // pthread_mutex_lock(&pool->mutexPool);
    for (int i = 0; i < pool->maxNum; i++)
    {
        if (pool->threadIDs[i] != 0)
        {
            pthread_join(pool->threadIDs[i], NULL);
            pool->threadIDs[i] = 0;
            pool->liveNum--;
        }
    }
    // pthread_mutex_unlock(&pool->mutexPool);
    printf("work threads quit!\n");

    // 释放堆内存
    if (pool->taskQ)
        free(pool->taskQ);
    if (pool->threadIDs)
        free(pool->threadIDs);

    // 销毁互斥量，条件变量
    pthread_mutex_destroy(&pool->mutexPool);
    pthread_mutex_destroy(&pool->mutexBusy);
    pthread_cond_destroy(&pool->notEmpty);
    pthread_cond_destroy(&pool->notFull);

    free(pool);
    pool = NULL;

    return 0;
}

// 测试案例
void taskFunc(void *arg)
{
    int num = *(int *)arg;
    printf("taskFunc thread %ld is working, number = %d\n", pthread_self(), num);
    // sleep(3);
}

int main()
{
#if 1
#define N 130
    {
        ThreadPool *pool = threadPoolCreate(5, 15, 100);
        for (int i = 0; i < N; i++)
        {
            int *num = (int *)malloc(sizeof(int));
            *num = i + 100;
            threadPoolAdd(pool, taskFunc, num);
        }
        sleep(1);
        threadPoolDestroy(pool);
    }
#undef N
#endif

    printf("Over!\n");
    return 0;
}
