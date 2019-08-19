#include <thread>
#include <iostream>
#include <event2/bufferevent.h>
#include <event2/event.h>
#include "unistd.h"
#include "XThread.h"

using namespace std;
//激活线程任务的回调函数



static void NotifyCB(evutil_socket_t fd, short which, void* arg)
{
	XThread* t = (XThread*)arg;
	t->Notify(fd, which);
}

void XThread::Notify(evutil_socket_t fd, short which)
{
	//水平触发，只要没有接收完全，会再次进来
	char buf[2] = {0};
	int re = read(fd, buf, 1);
	if(re <= 0)
		return ;
	cout << "id = " << id <<" thread " << buf << endl;
	XTask*task = NULL;
	//获取任务，并初始化任务
	
	task_mutex.lock();
	if(tasks.empty())
	{
		task_mutex.unlock();
		return ;
	}
	task = tasks.front(); 	//先进先出
	tasks.pop_front();
	task_mutex.unlock();
	task->Init();
}

//启动线程
void XThread::Start()
{
	Setup();
	//启动线程
	thread th(&XThread::Main, this);

	//断开与主线程联系
	th.detach();
}
bool XThread::Setup()
{
	int fds[2];
	//创建的管道，不能用send recv读取，用read， write
	if(pipe(fds))
	{
		cerr << "pipe failed!" << endl;
		return false;
	}
	//读取绑定到event事件种
	notify_send_fd = fds[1];

	//创建libevent上下文
	event_config* ev_config = event_config_new();
	event_config_set_flag(ev_config, EVENT_BASE_FLAG_NOLOCK); 
	this->base = event_base_new_with_config(ev_config);
	event_config_free(ev_config);
	if(!base)
	{
		cerr << "event_base_new_with_config failed in thread!" << endl;
		return false;
	}

	//添加管道监听事件，用于激活线程执行任务
	event* ev = event_new(base, fds[0], EV_READ|EV_PERSIST, NotifyCB, this);
	event_add(ev, 0);
}	



//线程入口函数
void XThread::Main()
{
	cout << id << " XThread::Main() begin" << endl;
	event_base_dispatch(base);
	event_base_free(base);
	cout << id << " XThread::Main() end" << endl;
}

void XThread::AddTask(XTask* t)
{
	if(!t) return ;
	t->base = this->base;
	task_mutex.lock();
	tasks.push_back(t);
	task_mutex.unlock();
}



void XThread::Active()
{
	int re = write(this->notify_send_fd, "c", 1);
	if(re <= 0)
	{
		cerr << "XThread::Active() failed!" << endl;
	}
}

XThread::XThread()
{

}


XThread::~XThread()
{

}
