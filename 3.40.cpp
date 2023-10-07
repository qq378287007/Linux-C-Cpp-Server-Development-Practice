#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include <vector>
#include <string>
using namespace std;

/*执行任务的类：设置任务数据并执行*/
class CTask
{
protected:
	string m_strTaskName; // 任务的名称
	void *m_ptrData;	  // 要执行的任务的具体数据

public:
	CTask() = default;
	CTask(const string &taskName, void *ptrData = NULL) : m_strTaskName(taskName), m_ptrData(ptrData) {}
	virtual ~CTask() {}

	void setData(void *ptrData) { m_ptrData = ptrData; } // 设置任务数据
	virtual int Run() = 0;
};

class CMyTask : public CTask
{
public:
	int Run()
	{
		printf("%s\n", (char *)m_ptrData);
		sleep(rand() % 4 + 1);
		return 0;
	}
};

/*线程池管理类*/
class CThreadPool
{
private:
	static vector<CTask *> m_vecTaskList;  // 任务列表
	static bool shutdown;				   // 线程退出标志
	static pthread_mutex_t m_pthreadMutex; // 线程同步锁
	static pthread_cond_t m_pthreadCond;   // 线程同步条件变量

	int m_iThreadNum; // 线程池中启动的线程数
	pthread_t *pthread_id;

protected:
	static void *ThreadFunc(void *threadData); // 新线程的线程回调函数
	//static int MoveToIdle(pthread_t tid);	   // 线程执行结束后，把自己放入空闲线程中
	//static int MoveToBusy(pthread_t tid);	   // 移入到忙碌线程中去

	// 创建线程
	int Create() // 创建线程池中的线程
	{
		pthread_id = new pthread_t[m_iThreadNum];
		for (int i = 0; i < m_iThreadNum; i++)
			pthread_create(&pthread_id[i], NULL, ThreadFunc, NULL);

		return 0;
	}

public:
	CThreadPool(int threadNum) // 线程管理类构造函数
	{
		m_iThreadNum = threadNum;
		printf("I will create %d threads.\n", threadNum);
		Create();
	}

	// 往任务队列里添加任务并发出线程同步信号
	int AddTask(CTask *task) // 把任务添加到任务队列中
	{
		pthread_mutex_lock(&m_pthreadMutex);
		m_vecTaskList.push_back(task);
		pthread_mutex_unlock(&m_pthreadMutex);

		pthread_cond_signal(&m_pthreadCond);
		return 0;
	}

	// 停止所有线程
	int StopAll() // 使线程池中的所有线程退出
	{
		// 避免重复调用
		if (shutdown)
			return -1;
		printf("Now I will end all threads!\n\n");

		// 唤醒所有等待进程，线程池也要销毁了
		shutdown = true;
		pthread_cond_broadcast(&m_pthreadCond);

		// 清楚僵尸
		for (int i = 0; i < m_iThreadNum; i++)
			pthread_join(pthread_id[i], NULL);

		delete[] pthread_id;
		pthread_id = NULL;

		// 销毁互斥量和条件变量
		//  pthread_mutex_destroy(&m_pthreadMutex);
		//  pthread_mutex_destroy(&m_pthreadCond);

		return 0;
	}
	static int getTaskSize(); // 获取当前任务队列中的任务数
};

// 静态成员初始化
vector<CTask *> CThreadPool::m_vecTaskList;
bool CThreadPool::shutdown = false;
pthread_mutex_t CThreadPool::m_pthreadMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t CThreadPool::m_pthreadCond = PTHREAD_COND_INITIALIZER;

// 获取当前队列中的任务数
int CThreadPool::getTaskSize()
{
	return m_vecTaskList.size();
}

// 线程回调函数
void *CThreadPool::ThreadFunc(void *threadData)
{
	pthread_t tid = pthread_self();
	while (1)
	{
		pthread_mutex_lock(&m_pthreadMutex);

		// 如果队列为空，等待新任务进入任务队列
		while (getTaskSize() == 0 && !shutdown)
			pthread_cond_wait(&m_pthreadCond, &m_pthreadMutex);

		// 关闭线程
		if (shutdown)
		{
			pthread_mutex_unlock(&m_pthreadMutex);
			printf("[tid: %lu]\texit\n", tid);
			pthread_exit(NULL);
		}

		// 取出一个任务并处理之
		printf("[tid: %lu]\trun: ", tid);
		vector<CTask *>::iterator iter = m_vecTaskList.begin();
		CTask *task = *iter;
		m_vecTaskList.erase(iter);

		pthread_mutex_unlock(&m_pthreadMutex);

		task->Run(); // 执行任务
		printf("[tid: %lu]\tidle\n", tid);
	}

	return (void *)0;
}

int main()
{
	CMyTask taskObj;
	char szTmp[] = "hello!";
	taskObj.setData((void *)szTmp);
	CThreadPool threadpool(5);

	for (int i = 0; i < 10; i++)
		threadpool.AddTask(&taskObj);

	while (1)
	{
		printf("There are still %d tasks need to handle\n", threadpool.getTaskSize());

		if (CThreadPool::getTaskSize() == 0)
		{
			if (threadpool.StopAll() == -1)
			{
				printf("Thread pool clear, exit.\n");
				exit(0);
			}
		}
		sleep(2);
		printf("2 seconds later...\n");
	}

	return 0;
}
