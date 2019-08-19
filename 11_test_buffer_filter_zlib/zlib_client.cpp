#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <event2/buffer.h>
#include <iostream>
#include <signal.h>
#include <string.h>

using namespace std;

#define SPORT 5001	

#define FILEPATH "test1.txt"



void client_read_cb(bufferevent* bev, void *arg)
{
	cout << "[client R]" << flush;
	char data[1024] = {0};
	//002 接收服务器端返回的ok
	bufferevent_read(bev, data, sizeof(data) - 1);
	if(strcmp(data, "OK") == 0)
	{
		cout << data << endl;
		//开始发送文件, 触发写入回调
		bufferevent_trigger(bev, EV_WRITE, 0);
	}else{
		bufferevent_free(bev);
	}
}


void client_write_cb(bufferevent* bev, void *arg)
{
	cout << "[client W]" << flush;
	FILE* fp = (FILE*)arg;
	if (!fp) return ;
	cout << "client_write_cb" << endl;
	//读取文件
	char data[1024] = {0};
	int len = fread(data, 1, sizeof(data), fp);
	if(len <= 0)
	{
		fclose(fp);

		bufferevent_free(bev);
		return ;
	}
	
	//发送文件
	bufferevent_write(bev, data, len);
}

bufferevent_filter_result filter_out(evbuffer* s, evbuffer* d, ev_ssize_t limit, 
					bufferevent_flush_mode mode, void* arg)
{
	cout << "filter_out" << endl;
	char data[1024] = {0};
	int len = evbuffer_remove(s, data, sizeof(data));
	evbuffer_add(d, data, len);
	return BEV_OK;



}

void client_event_cb(bufferevent* be, short events, void* arg)
{
	cout << "client_event_cb "<< events << endl;
	if(events & BEV_EVENT_CONNECTED)
	{
		//001 发送文件名
		cout << "BEV_EVENT_CONNECTED" << endl;
		bufferevent_write(be, FILEPATH, strlen(FILEPATH));

		bufferevent* bev_filter = bufferevent_filter_new(be, 0, filter_out,
					   						BEV_OPT_CLOSE_ON_FREE, 0, 0);

		FILE* fp = fopen(FILEPATH, "rb");
		if(!fp)
		{
			cout << "open file" << FILEPATH << " failed!" << endl;
		}
		
		


		bufferevent_setcb(bev_filter, client_read_cb, client_write_cb, client_event_cb, fp);
		bufferevent_enable(bev_filter, EV_READ | EV_WRITE);
	}
}





void Client(event_base* base)
{
	//链接服务器
	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SPORT);
	evutil_inet_pton(AF_INET, "127.0.0.1", &sin.sin_addr.s_addr);
	bufferevent* bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
	
	//只绑定事件回调， 用来确认链接成功
	
	bufferevent_enable(bev, EV_READ | EV_WRITE);
	bufferevent_setcb(bev, 0, 0, client_event_cb, 0);
	
	bufferevent_socket_connect(bev, (sockaddr*)&sin, sizeof(sin));
	//
	//接收回复确认ok
}
