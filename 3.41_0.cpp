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

    virtual void *Run() = 0;

protected:
    void *data = NULL;
};

class CMyTask : public XTask
{
public:
    void *Run()
    {
        int n = *(int *)data;
        cout << "Task " << n << " start" << endl;

        for (int c = 0; c < 1000; c++)
            for (int i = 0; i < 1000; i++)
                ;

        cout << "Task " << n << " end" << endl;
        return NULL;
    }
};

class XThread
{
public:
    void Main()
    {
        std::thread::id id = this_thread::get_id();
        while (true)
        {
            XTask *t;
            {
                std::unique_lock<std::mutex> lk(mtx);
                while (tasks.empty() && !quit)
                    cv.wait(lk);

                if (quit)
                {
                    cout << "Thread " << id << " quit" << endl;
                    return;
                }
                t = tasks.front();
                tasks.pop_front();
            }

            cout << "Thread " << id << " start" << endl;
            t->Run();
            cout << "Thread " << id << " end" << endl;
        }
    }

    void AddTask(XTask *t)
    {
        mtx.lock();
        cout << "AddTask" << endl;
        tasks.push_back(t);
        cout << tasks.size() << endl;
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
    std::deque<XTask *> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    bool quit;
    thread t;
};

class XThreadPool
{
public:
    XThreadPool(int _threadCount = 10) : threadCount(_threadCount)
    {
        lastThread = -1;
        for (int i = 0; i < threadCount; i++)
        {
            threads.push_back(new XThread());
            this_thread::sleep_for(chrono::milliseconds(10));
        }
    }
    ~XThreadPool()
    {
        for (int i = 0; i < threadCount; i++)
            delete threads[i];
    }

    void Dispatch(XTask *task)
    {
        lastThread += 1;
        lastThread %= threadCount;

        threads[lastThread]->AddTask(task);
    }

private:
    int threadCount;
    int lastThread;
    std::vector<XThread *> threads;
};

int main()
{
    /*
        {
            CMyTask task;
            int data = 1;
            task.setData((void *)&data);

            XThread t;
            t.AddTask(&task);
        }
    */

    /*
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
    */

    {
#define N 100
        CMyTask task[10];
        int data[10];

        XThreadPool threadpool;

        for (int i = 0; i < 10; i++)
        {
            data[i] = i;
            task[i].setData((void *)&data[i]);
            threadpool.Dispatch(&task[i]);
        }
#undef N
    }

    return 0;
}