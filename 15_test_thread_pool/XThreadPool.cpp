#include "XThreadPool.h"
#include "XTask.h"
#include "XThread.h"
#include <thread>
#include <iostream>

using namespace std;
void XThreadPool::Dispatch(XTask* task)
{
	if(!task) return ;
	int tid = (lastThread + 1) % threadCount;
	lastThread = tid;
	XThread* t = threads[tid];

	t->AddTask(task);
	//激活线程
	t->Active();
}


void XThreadPool::Init(int threadCount)
{
	this->threadCount = threadCount;
	lastThread = -1;
	for(int i = 0; i < threadCount; i++)
	{
		XThread* t = new XThread();
		cout << "Create thread " << i << endl;
		t->id = i+1;
		t->Start();
		threads.push_back(t);
		this_thread::sleep_for(chrono::milliseconds(10));
	}
}
