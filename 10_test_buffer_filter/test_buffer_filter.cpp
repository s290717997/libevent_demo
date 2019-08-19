#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <event2/buffer.h>
#include <iostream>
#include <signal.h>
#include <string.h>

using namespace std;

#define SPORT 5001


bufferevent_filter_result filter_in(evbuffer* s, evbuffer* d, ev_ssize_t limit, 
					bufferevent_flush_mode mode, void* arg)
{
	//cout << "filter_in" << endl;
	//读取并清理原数据
	char data[1024] = {0};
	
	int len = evbuffer_remove(s, data, sizeof(data) - 1);

	for(int i = 0; i < len; i++)
	{
		data[i] = toupper(data[i]);
	}

	evbuffer_add(d, data, len);

	return BEV_OK;
}


bufferevent_filter_result filter_out(evbuffer* s, evbuffer* d, ev_ssize_t limit, 
					bufferevent_flush_mode mode, void* arg)
{
	cout << "filter_out" << endl;
	char data[1024] = {0};
	//读取并清理原数据
	int len = evbuffer_remove(s, data, sizeof(data) - 1);
	string str = "";
	str += "==============\n";
	str += data;
	str += "==============\n";

	evbuffer_add(d, str.c_str(), str.size());

	return BEV_OK;
}




void event_cb(bufferevent* be, short events, void *arg)
{
	cout << "[E]" << flush;
	//超时事件发生时，读取事件停止
	if(events & BEV_EVENT_TIMEOUT && events & BEV_EVENT_READING)
	{
		cout << "BEV_EVENT_TIMEOUT BEV_EVENT_READING" << endl;
	//	bufferevent_enable(be, EV_READ);   //再次开启读事件
		bufferevent_free(be);			//超时清理掉链接
	}else if(events & BEV_EVENT_ERROR){
		cout << "BEV_EVENT_ERROR" << endl;
		bufferevent_free(be);	
	}else{
		cout << "OTHERS" << endl;
	}
}


void write_cb(bufferevent* be, void *arg)
{
	cout << "[W]" << flush;
}

void read_cb(bufferevent* be, void *arg)
{
	cout << "[R]" << flush;
	char data[1024] = {0};
	//读取缓冲数据并且发送
	int len = bufferevent_read(be, data, sizeof(data) - 1);
	cout << "["<< data <<"]" << flush;
	if(len <= 0) return ;
	if(strstr(data, "quit") != NULL)
	{
		cout << "quit" << endl;
		//退出并关闭socket
		bufferevent_free(be);
	}
	bufferevent_write(be, data, len);
}


void listen_cb(struct evconnlistener* e, evutil_socket_t s, struct sockaddr* a, int socklen, void* arg)
{
	cout << "listen_cb " << endl;

	event_base* base = (event_base*)arg;
	
	//创建bufferevent 
	bufferevent* bev = bufferevent_socket_new(base, s, BEV_OPT_CLOSE_ON_FREE);
	//绑定bufferevent filter
	bufferevent* bev_filter = bufferevent_filter_new(bev, 
								filter_in,  	//输入过滤函数
								filter_out,		//输出过滤函数
								BEV_OPT_CLOSE_ON_FREE,	//关闭filter同时关闭bufferevent
								0,				//清理的回调函数 
								0);				//传递给回调函数
	//设置bufferevent的回调
	bufferevent_setcb(bev_filter, read_cb, write_cb, event_cb, NULL);

	bufferevent_enable(bev_filter, EV_READ|EV_WRITE);


}



int main()
{

	//忽略管道信号，发送数据给以关闭的
	if(signal(SIGPIPE, SIG_IGN) == SIG_ERR)
	{
		return 1;
	}

	std:: cout << "test server!\n";

	event_base* base = event_base_new();
	if(base)
	{
		cout << "event_base_new success!" << endl;
	}
	//设置监听的端口和地址
	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SPORT);

	//监听端口
	evconnlistener* ev = evconnlistener_new_bind(base, //libevent的上下文
							listen_cb,	//接收到连接的回调函数
							base,		//回调函数获取的参数arg
							LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,	//地址重用，evconnlistener关闭同时关闭socket
							10,						//连接队列的大小，对应listen函数
							(sockaddr*)&sin, 		//绑定的地址和端口
							sizeof(sin));

	if(base)
		event_base_dispatch(base);
	if(ev)
		evconnlistener_free(ev);

	if(base)
		event_base_free(base);


	return 0;


}
