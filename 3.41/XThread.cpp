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
	mtx.lock();
	if (tasks.empty()) {
		mtx.unlock();
		return;
	}
	t = tasks.front();
	tasks.pop_front();
	mtx.unlock();
	t->Init(arg);
}



void XThread::AddTack(XTask *t) {
	if (!t) return;

	t->base = this->base;
	mtx.lock();
	tasks.push_back(t);
	mtx.unlock();
}

XThread::XThread() {
}

XThread::~XThread() { 

}
