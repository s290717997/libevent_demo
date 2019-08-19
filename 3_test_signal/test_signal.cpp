#include <iostream>
#include <event2/event.h>
#include <signal.h>

using namespace std;



/* sock 文件描述符
 * which 事件类型
 * arg 传递的参数*/
static void Ctrl_C(int sock, short which, void* arg)
{
	cout << "ctrl + c" << endl;	
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



	event_base_dispatch(base);
	event_free(csig);
	event_base_free(base);


	return 0;
}
