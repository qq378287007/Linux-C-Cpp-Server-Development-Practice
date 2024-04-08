#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <mutex>
#include <thread>
using namespace std;

#define testout(msg) cout << msg << endl

class XTask
{
public:
    virtual ~XTask() {}

    // 初始化任务
    virtual void Init(int arg) = 0;

    // 一客户端一个base
    // struct event_base *base = 0;

    // 线程池id
    // int thread_id = 0;
};

class CMyTask : public XTask
{
public:
    void Init(int arg)
    {
        for (long long c = 0; c < 1000; c++)
            for (long long i = 0; i < 1000; i++)
                ;

        printf("%d---------%d--------\n", arg, arg);
    }
};

class XThread
{
public:
    void Main()
    {
        cout << id << " thread::Main() begin" << endl;

        cout << id << " thread::Main() end" << endl;
    }

    void Start()
    {
        testout(id << " thread At Start()");

        thread th(&XThread::Main, this);
        th.detach();
    }

    void Activate(int arg)
    {
        testout(id << " thread At Activate()");

        mtx.lock();

        if (tasks.empty())
        {
            mtx.unlock();
            return;
        }

        XTask *t = tasks.front();
        tasks.pop_front();

        mtx.unlock();

        t->Init(arg);
    }

    void AddTask(XTask *t)
    {
        if (t)
        {
            // t->base = base;
            mtx.lock();
            tasks.push_back(t);
            mtx.unlock();
        }
    }

    // void Notify();
    int id = 0;

private:
    // event_base *base = 0;
    list<XTask *> tasks;
    mutex mtx;
};

class XThreadPool
{
public:
    void Init(int _threadCount)
    {
        testout("main thread At XThreadPoll::Init()");

        threadCount = _threadCount;
        lastThread = -1;
        for (int i = 0; i < threadCount; i++)
        {
            cout << "Create thread " << i << endl;
            XThread *t = new XThread();
            t->id = i;
            t->Start();
            threads.push_back(t);

            this_thread::sleep_for(chrono::milliseconds(10));
        }
    }

    void Dispatch(XTask *task, int arg)
    {
        testout("main thread At XThreadPoll::dispathch()");

        if (task)
        {
            int tid = (lastThread + 1) % threadCount;
            lastThread = tid;

            XThread *t = threads[tid];
            t->AddTask(task);
            t->Activate(arg);
        }
    }

private:
    int threadCount;
    int lastThread = -1;
    vector<XThread *> threads;
};

int main()
{
    {
#define N 10
        XThreadPool threadpool;
        threadpool.Init(N); // 初始化线程池

        CMyTask task[N];

        for (int i = 0; i < N; i++)
            threadpool.Dispatch(&task[i], i); // 分配任务
#undef N
    }

    return 0;
}
