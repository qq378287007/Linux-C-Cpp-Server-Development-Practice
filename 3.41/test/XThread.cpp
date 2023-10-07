#include <thread>
#include <iostream>
using namespace std;
#include <unistd.h>
#include "testUtil.h"
#include "XThread.h"
#include "XTask.h"


void XThread::Start() {
	testout(id << " thread At Start()");
	thread th(&XThread::Main, this);
	th.detach();
}


void XThread::Main() {
	cout << id << " thread::Main() begin" << endl;

	cout << id << " thread::Main() end" << endl;
}



void XThread::Activate(int arg) {
	testout(id << " thread At Activate()");

	XTask *t = NULL;
	tasks_mutex.lock();
	if (tasks.empty()) {
		tasks_mutex.unlock();
		return;
	}
	t = tasks.front();
	tasks.pop_front();
	tasks_mutex.unlock();
	t->Init(arg);
}



void XThread::AddTack(XTask *t) {
	if (!t) return;

	t->base = this->base;
	tasks_mutex.lock();
	tasks.push_back(t);
	tasks_mutex.unlock();
}

XThread::XThread() {
}

XThread::~XThread() { 

}
