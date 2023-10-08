#include <unistd.h>

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <mutex>
#include <thread>
#include <deque>
#include <condition_variable>
using namespace std;

class XTask
{
public:
    virtual ~XTask() {}

    void setData(void *_data)
    {
        data = _data;
    }

    virtual void Run() = 0;

protected:
    void *data = NULL;
};

class CMyTask : public XTask
{
public:
    void Run()
    {
        int n = *(int *)data;
        cout << "Task " << n << " start" << endl;
        for (int c = 0; c < 1000; c++)
            for (int i = 0; i < 1000; i++)
                ;
        cout << "Task " << n << " end" << endl;
    }
};

class XThread
{
public:
    void Main()
    {
        thread::id id = this_thread::get_id();
        cout << "Thread " << id << " start" << endl;
        while (true)
        {
            XTask *t;
            {
                unique_lock<mutex> lk(mtx);
                while (tasks.empty() && !quit)
                    cv.wait(lk);

                if (quit)
                {
                    cout << "Thread " << id << " quit" << endl;
                    break;
                }
                t = tasks.front();
                tasks.pop_front();
            }
            t->Run();
        }
        cout << "Thread " << id << " end" << endl;
    }

    void AddTask(XTask *t)
    {
        mtx.lock();
        cout << "AddTask" << endl;
        tasks.push_back(t);
        cout << "TaskSize: " << tasks.size() << endl;
        mtx.unlock();

        cv.notify_one();
    }

    XThread()
    {
        quit = false;
        t = thread(&XThread::Main, this);
    }

    ~XThread()
    {
        mtx.lock();
        quit = true;
        mtx.unlock();

        cv.notify_one();

        t.join();
    }

private:
    bool quit;
    thread t;

    deque<XTask *> tasks;
    mutex mtx;
    condition_variable cv;
};

class XThreadPool
{
public:
    XThreadPool(int _threadCount = 10) : threadCount(_threadCount)
    {
        for (int i = 0; i < threadCount; i++)
        {
            threads.push_back(new XThread());
            // this_thread::sleep_for(chrono::milliseconds(10));
        }
        lastThread = -1;
    }
    ~XThreadPool()
    {
        for (int i = 0; i < threadCount; i++)
            delete threads[i];
    }

    void Dispatch(XTask *task)
    {
        lastThread = (lastThread + 1) % threadCount;
        threads[lastThread]->AddTask(task);
    }

private:
    int threadCount;
    int lastThread;
    vector<XThread *> threads;
};

int main()
{
    if (false)
    {
#define N 100
        CMyTask task[N];
        int data[N];
        XThread t;

        for (int i = 0; i < N; i++)
        {
            data[i] = i;
            task[i].setData((void *)&data[i]);
            t.AddTask(&task[i]);
        }
        sleep(1);
#undef N
    }

    if (true)
    {
#define N 100
        CMyTask task[N];
        int data[N];
        XThreadPool threadpool;

        for (int i = 0; i < N; i++)
        {
            data[i] = i;
            task[i].setData((void *)&data[i]);
            threadpool.Dispatch(&task[i]);
        }
        sleep(1);
#undef N
    }
    return 0;
}
