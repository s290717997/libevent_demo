/*
 * 1.定时器有2种调用方法
 *  1）使用宏timer_new，该方法默认事件触发时进入no pending，需要在触发后重新手动添加到pending状态
 *  2) 使用event_new, 参数中添加EV_PERSIST表示持久化（循环触发事件）
 *
 */
#include <iostream>
#include <event2/event.h>
#include <signal.h>
#include <thread>
#include <unistd.h>
using namespace std;


static timeval t1 = {1, 0};

void timer1(int sock, short which, void* arg)
{
	cout << "[timer1]" << flush;
	event* ev = (event*)arg;
	if(!evtimer_pending(ev, &t1))
	{
		evtimer_del(ev);
		evtimer_add(ev, &t1);
	}
}

void timer2(int sock, short which, void* arg)
{
	cout << "[timer2]" << flush;
	sleep(3);
}



void timer3(int sock, short which, void* arg)
{
	cout << "[timer3]" << flush;
}

int main(int argc, const char** argv)
{
	event_base* base = event_base_new();
	
	cout << "test timer" << endl;


	/*对event_new封装, 非持久事件，只进入一次*/
	event* ev1 = evtimer_new(base, timer1, event_self_cbarg());
	if(!ev1)
	{
		cout << "evtimer_new timer1 failed!" << endl;
	}
	evtimer_add(ev1, &t1);


	/*使用参数来定义持久化*/
	static timeval t2;
	t2.tv_sec = 1;
	t2.tv_usec = 200000;
	event* ev2 = event_new(base, -1, EV_PERSIST, timer2,0);
	event_add(ev2, &t2);
	


	/* 超时优化性能优化，默认event用二叉堆存储（完全二叉树）插入删除O(logn)
	 * 优化到双向队列 插入删除O(1)	
	 * */

	event* ev3 = event_new(base, -1, EV_PERSIST, timer3,0);

	static timeval tv_in = {3, 0};
	const timeval* t3;
	t3 = event_base_init_common_timeout(base, &tv_in);
	event_add(ev3, t3);



	event_base_dispatch(base);
	event_base_free(base);


	return 0;
}
