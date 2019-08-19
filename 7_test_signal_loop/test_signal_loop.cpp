/*
 * 事件循环有2种方式
 * 	1）event_base_dispatch(base);普通的方式，无法设置属性
 * 	2）event_base_loop(base, EV_ONCE);
 *			EVLOOP_ONCE等待一个事件运行，直到没有活动事件则推出
 *			EVLOOP_NONBLOCK有活动事件就处理，没有就返回
 *			EVLOOP_NO_EXIT_ON_EMPTY没有注册事件也不返回，用于事件后期多线程添加
 *	事件退出循环也有对应的2种方式
 *	1) event_base_loopbreak(base);  执行完当前的事件任务就退出
 *	2) timeval t = {3,0};
 *	   event_base_loopexit(base, &t)  运行完所有的事件在退出，事件循环没有运行时也要运行一次在退出，该写法表示至少运行3秒才退出
 * 
 */
#include <iostream>
#include <event2/event.h>
#include <signal.h>

using namespace std;

bool isexit = false;

/* sock 文件描述符
 * which 事件类型
 * arg 传递的参数*/
static void Ctrl_C(int sock, short which, void* arg)
{
	cout << "ctrl + c" << endl;	
	event_base* base = (event_base*)arg;
	//执行完当前的事件函数就退出
	//event_base_loopbreak(base);

	//运行完所有的事件在退出,事件循环没有运行时也要等运行一次在退出, 至少运行3秒才退出
	timeval t = {3,0};
	event_base_loopexit(base, &t);
}

static void Kill(int sock, short which, void* arg)
{
	cout << "Kill" << endl;

	event* ev = (event*)arg;	
	//如果处于非待决状态，则再次进入
	if(!evsignal_pending(ev, NULL))
	{
		event_del(ev);
		event_add(ev, NULL);
	}
}


int main(int argc, const char** argv)
{
	event_base* base = event_base_new();
	
#if 1
	//添加信号ctrl+C 信号事件，处于no pending
	//evsignal_new隐藏的状态 EV_SIGNAL|EV_PERSIST
	event* csig = evsignal_new(base, SIGINT, Ctrl_C, base);

	if(!csig)
	{
		cerr << "SIGINT evsignale_new failed!" << endl;
		return -1;
	}

	//使事件处于pending状态
	if(event_add(csig, 0) != 0)
	{
		cerr << "SIGINT event_add failed!" << endl;
		return -1;
	}

	
	//添加kill信号
	//非持久信号，只进入一次, event_self_cbarg()传递当前的event
	event* ksig = event_new(base, SIGTERM, EV_SIGNAL, Kill, event_self_cbarg());


	if(!ksig)
	{
		cerr << "SIGINT evsignale_new failed!" << endl;
		return -1;
	}

	//使事件处于pending状态
	if(event_add(ksig, 0) != 0)
	{
		cerr << "SIGINT event_add failed!" << endl;
		return -1;
	}

#endif
	//进入事件主循环
	//event_base_dispatch(base);
		
	//EVLOOP_ONCE等待一个事件运行，直到没有活动事件则推出
	//EVLOOP_NONBLOCK有活动事件就处理，没有就返回
	//while(!isexit){
	//	event_base_loop(base, EVLOOP_NONBLOCK);
	//}
	
	//EVLOOP_NO_EXIT_ON_EMPTY没有注册事件也不返回，用于事件后期多线程添加
	event_base_loop(base, EVLOOP_NO_EXIT_ON_EMPTY);
		



	cout << "end!" << endl;
	//event_free(csig);
	event_base_free(base);


	return 0;
}
