#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include <vector>
#include <string>
using namespace std;

class CTask
{
public:
	CTask(void *ptrData = nullptr) : m_ptrData(ptrData) {}
	virtual ~CTask()
	{
		if (m_ptrData != nullptr)
			free(m_ptrData);
		m_ptrData = nullptr;
	}

	virtual void Run() = 0;

protected:
	void *m_ptrData;
};

class CMyTask : public CTask
{
public:
	CMyTask(void *ptrData = nullptr) : CTask(ptrData) {}

	void Run()
	{
		printf("%s\n", (char *)m_ptrData);
		sleep(rand() % 4 + 1);
	}
};

class CThreadPool
{
private:
	static vector<CTask *> m_vecTaskList;
	static bool shutdown;
	static pthread_mutex_t m_pthreadMutex;
	static pthread_cond_t m_pthreadCond;

	int m_iThreadNum;
	pthread_t *pthread_id;

protected:
	static void *ThreadFunc(void *threadData);
	// static int MoveToIdle(pthread_t tid);
	// static int MoveToBusy(pthread_t tid);

public:
	static int getTaskSize();

	CThreadPool(int threadNum) : m_iThreadNum(threadNum)
	{
		printf("I will create %d threads.\n", threadNum);

		pthread_id = new pthread_t[m_iThreadNum];
		for (int i = 0; i < m_iThreadNum; i++)
			pthread_create(&pthread_id[i], NULL, ThreadFunc, NULL);
	}
	~CThreadPool()
	{
		/*
		while (CThreadPool::getTaskSize() == 0)
		{
			if (StopAll() == -1)
			{
				printf("Thread pool clear, exit.\n");
				break;
			}
		}
		*/
	}

	void AddTask(CTask *task)
	{
		pthread_mutex_lock(&m_pthreadMutex);
		m_vecTaskList.push_back(task);
		pthread_mutex_unlock(&m_pthreadMutex);

		pthread_cond_signal(&m_pthreadCond);
	}

	int StopAll()
	{
		if (shutdown)
			return -1;

		printf("Now I will end all threads!\n\n");

		pthread_mutex_lock(&m_pthreadMutex);
		shutdown = true;
		pthread_mutex_unlock(&m_pthreadMutex);

		pthread_cond_broadcast(&m_pthreadCond);

		for (int i = 0; i < m_iThreadNum; i++)
			pthread_join(pthread_id[i], NULL);

		delete[] pthread_id;
		pthread_id = NULL;

		//  pthread_mutex_destroy(&m_pthreadMutex);
		//  pthread_mutex_destroy(&m_pthreadCond);

		return 0;
	}
};

vector<CTask *> CThreadPool::m_vecTaskList;
bool CThreadPool::shutdown = false;
pthread_mutex_t CThreadPool::m_pthreadMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t CThreadPool::m_pthreadCond = PTHREAD_COND_INITIALIZER;

int CThreadPool::getTaskSize()
{
	return m_vecTaskList.size();
}

void *CThreadPool::ThreadFunc(void *threadData)
{
	pthread_t tid = pthread_self();
	while (true)
	{
		pthread_mutex_lock(&m_pthreadMutex);

		while (getTaskSize() == 0 && !shutdown)
			pthread_cond_wait(&m_pthreadCond, &m_pthreadMutex);

		if (shutdown)
		{
			pthread_mutex_unlock(&m_pthreadMutex);
			printf("[tid: %lu]\texit\n", tid);
			pthread_exit(NULL);
		}

		printf("[tid: %lu]\trun: ", tid);
		vector<CTask *>::iterator iter = m_vecTaskList.begin();
		CTask *task = *iter;
		m_vecTaskList.erase(iter);

		pthread_mutex_unlock(&m_pthreadMutex);

		task->Run();
		printf("[tid: %lu]\tidle\n", tid);
	}

	return (void *)0;
}

int main()
{
	CThreadPool threadpool(5);
	for (int i = 0; i < 10; i++)
	{
		char *tmp = (char *)malloc(64);
		strcpy(tmp, (string("hello ") + to_string(i)).c_str());
		threadpool.AddTask(new CMyTask((void *)(tmp)));
	}

	while (true)
	{
		printf("There are still %d tasks need to handle\n", CThreadPool::getTaskSize());

		if (CThreadPool::getTaskSize() == 0)
		{
			if (threadpool.StopAll() == -1)
			{
				printf("Thread pool clear, exit.\n");
				break;
			}
		}

		sleep(2);
		printf("2 seconds later...\n");
	}

	return 0;
}
