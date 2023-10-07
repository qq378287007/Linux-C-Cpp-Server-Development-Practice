#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <mutex>
#include <thread>
using namespace std;

// #define TEST

#ifdef TEST
#define testout(msg) cout << msg << endl \
                          << flush
#else
#define testout(msg)
#endif

class XTask
{
public:
    virtual ~XTask() {}
    // struct event_base *base = 0;
    // int thread_id = 0;
    virtual bool Init(int arg) = 0;
};

class CMyTask : public XTask
{
public:
    bool Init(int arg)
    {
        long long i = 0;
        long long c = 0;
        for (; c < 10000000; c++)
            while (i < 1000000000)
                i++;
        printf("%d---------%lld--------\n", arg, c);
        return true;
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
    std::list<XTask *> tasks;
    std::mutex mtx;
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
            cout << "Create thread" << i << endl;
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
    std::vector<XThread *> threads;
};

static XThreadPool threadpool;

int main()
{
    CMyTask task[10];

    threadpool.Init(10);
    for (int i = 0; i < 10; i++)
        threadpool.Dispatch(&task[i], i);

    return 0;
}