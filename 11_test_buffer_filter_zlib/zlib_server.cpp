#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <event2/buffer.h>
#include <iostream>
#include <signal.h>
#include <string.h>

#define SPORT 5001

using namespace std;

struct Status{
	bool start = fasle;
	FILE* fp = 0;
}


void event_cb(bufferevent* be, short events, void* arg)
{
	cout << "events_cb" << endl;

}


void read_cb(bufferevent* be, void *arg)
{
	cout << "[server R]" << flush;
	//回复ok
	Status* status = (Status*)arg;
	if(!status->start){
		char data[1024] = {0};
		bufferevent_read(bev, data, sizeof(data) - 1);
		status->filename = data;

			string out = ""
		status->fp = fopen(data, "wb");

		bufferevent_write(be, "OK", 2);

		status->start = true;
	}
}


bufferevent_filter_result filter_in(evbuffer* s, evbuffer* d, ev_ssize_t limit, 
					bufferevent_flush_mode mode, void* arg)
{
	cout << "server filter_in" << endl;
	//接收客户端发送的文件名
	char data[1024] = {0};
	int len = evbuffer_remove(s, data, sizeof(data) - 1);
	cout << "server recv " << data << endl;
	evbuffer_add(d, data, len);
	
	return BEV_OK;
}




void listen_cb(struct evconnlistener* e, evutil_socket_t s, struct sockaddr* a, int socklen, void* arg)
{
	cout << "listen_cb " << endl;

	event_base* base = (event_base*)arg;
	
	//创建bufferevent 
	bufferevent* bev = bufferevent_socket_new(base, s, BEV_OPT_CLOSE_ON_FREE);
	Status* status = new Status();

	//绑定bufferevent filter
	bufferevent* bev_filter = bufferevent_filter_new(bev, 
								filter_in,  	//输入过滤函数
								0,		//输出过滤函数
								BEV_OPT_CLOSE_ON_FREE,	//关闭filter同时关闭bufferevent
								0,				//清理的回调函数 
								0);				//传递给回调函数
	//设置bufferevent的回调
	bufferevent_setcb(bev_filter, read_cb, 0, event_cb, NULL);
	//设置权限
	bufferevent_enable(bev_filter, EV_READ|EV_WRITE);


}

void Server(event_base* base)
{
	cout << "test Server" << endl;
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


	
}


