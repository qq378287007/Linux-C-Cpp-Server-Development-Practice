#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include <vector>
#include <string>
using namespace std;

/*ִ��������ࣺ�����������ݲ�ִ��*/
class CTask
{
protected:
	string m_strTaskName; // ���������
	void *m_ptrData;	  // Ҫִ�е�����ľ�������

public:
	CTask() = default;
	CTask(const string &taskName, void *ptrData = NULL) : m_strTaskName(taskName), m_ptrData(ptrData) {}
	virtual ~CTask() {}

	void setData(void *ptrData) { m_ptrData = ptrData; } // ������������
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

/*�̳߳ع�����*/
class CThreadPool
{
private:
	static vector<CTask *> m_vecTaskList;  // �����б�
	static bool shutdown;				   // �߳��˳���־
	static pthread_mutex_t m_pthreadMutex; // �߳�ͬ����
	static pthread_cond_t m_pthreadCond;   // �߳�ͬ����������

	int m_iThreadNum; // �̳߳����������߳���
	pthread_t *pthread_id;

protected:
	static void *ThreadFunc(void *threadData); // ���̵߳��̻߳ص�����
	//static int MoveToIdle(pthread_t tid);	   // �߳�ִ�н����󣬰��Լ���������߳���
	//static int MoveToBusy(pthread_t tid);	   // ���뵽æµ�߳���ȥ

	// �����߳�
	int Create() // �����̳߳��е��߳�
	{
		pthread_id = new pthread_t[m_iThreadNum];
		for (int i = 0; i < m_iThreadNum; i++)
			pthread_create(&pthread_id[i], NULL, ThreadFunc, NULL);

		return 0;
	}

public:
	CThreadPool(int threadNum) // �̹߳����๹�캯��
	{
		m_iThreadNum = threadNum;
		printf("I will create %d threads.\n", threadNum);
		Create();
	}

	// �����������������񲢷����߳�ͬ���ź�
	int AddTask(CTask *task) // ��������ӵ����������
	{
		pthread_mutex_lock(&m_pthreadMutex);
		m_vecTaskList.push_back(task);
		pthread_mutex_unlock(&m_pthreadMutex);

		pthread_cond_signal(&m_pthreadCond);
		return 0;
	}

	// ֹͣ�����߳�
	int StopAll() // ʹ�̳߳��е������߳��˳�
	{
		// �����ظ�����
		if (shutdown)
			return -1;
		printf("Now I will end all threads!\n\n");

		// �������еȴ����̣��̳߳�ҲҪ������
		shutdown = true;
		pthread_cond_broadcast(&m_pthreadCond);

		// �����ʬ
		for (int i = 0; i < m_iThreadNum; i++)
			pthread_join(pthread_id[i], NULL);

		delete[] pthread_id;
		pthread_id = NULL;

		// ���ٻ���������������
		//  pthread_mutex_destroy(&m_pthreadMutex);
		//  pthread_mutex_destroy(&m_pthreadCond);

		return 0;
	}
	static int getTaskSize(); // ��ȡ��ǰ��������е�������
};

// ��̬��Ա��ʼ��
vector<CTask *> CThreadPool::m_vecTaskList;
bool CThreadPool::shutdown = false;
pthread_mutex_t CThreadPool::m_pthreadMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t CThreadPool::m_pthreadCond = PTHREAD_COND_INITIALIZER;

// ��ȡ��ǰ�����е�������
int CThreadPool::getTaskSize()
{
	return m_vecTaskList.size();
}

// �̻߳ص�����
void *CThreadPool::ThreadFunc(void *threadData)
{
	pthread_t tid = pthread_self();
	while (1)
	{
		pthread_mutex_lock(&m_pthreadMutex);

		// �������Ϊ�գ��ȴ�����������������
		while (getTaskSize() == 0 && !shutdown)
			pthread_cond_wait(&m_pthreadCond, &m_pthreadMutex);

		// �ر��߳�
		if (shutdown)
		{
			pthread_mutex_unlock(&m_pthreadMutex);
			printf("[tid: %lu]\texit\n", tid);
			pthread_exit(NULL);
		}

		// ȡ��һ�����񲢴���֮
		printf("[tid: %lu]\trun: ", tid);
		vector<CTask *>::iterator iter = m_vecTaskList.begin();
		CTask *task = *iter;
		m_vecTaskList.erase(iter);

		pthread_mutex_unlock(&m_pthreadMutex);

		task->Run(); // ִ������
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
