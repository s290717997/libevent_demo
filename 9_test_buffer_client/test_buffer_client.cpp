/**
 * 包含了一个客户的和一个服务器,分别使用了bufferevent
 * 客户的发送本文件给服务器，接收完成后完成发送
 *
 */
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <iostream>
#include <signal.h>
#include <string.h>

using namespace std;

#define SPORT 5001

static string recvStr = "";
static int recvCount = 0;
static int sendCount = 0;



void event_cb(bufferevent* be, short events, void *arg)
{
	cout << "[E]" << flush;
	//超时事件发生时，读取事件停止
	if(events & BEV_EVENT_TIMEOUT && events & BEV_EVENT_READING)
	{
		cout << "BEV_EVENT_TIMEOUT BEV_EVENT_READING" << endl;

		//因为设置了水位所以可能缓冲区中还有低于水位的值没有读取, 所以超时后在读取一次缓冲区
		char data[1024] = {0};
		int len = bufferevent_read(be, data, sizeof(data) - 1);
		if(len > 0)
		{
			recvCount += len;
			recvStr += data;
		}


	//	bufferevent_enable(be, EV_READ);   //再次开启读事件
		bufferevent_free(be);			//超时清理掉链接
		cout << recvStr << endl;
		cout << "recvCount = " << recvCount << endl;
		cout << "sendCount = " << sendCount << endl;
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
	//cout << "["<< data <<"]" << endl;
	cout << data << flush;
	if(len <= 0) return ;
	recvStr += data;
	recvCount += len;
}

void client_event_cb(bufferevent* be, short events, void* arg)
{
		
	cout << "[client E]" << flush;
	//超时事件发生时，读取事件停止
	if(events & BEV_EVENT_TIMEOUT && events & BEV_EVENT_READING)
	{
		cout << "BEV_EVENT_TIMEOUT BEV_EVENT_READING" << endl;
	//	bufferevent_enable(be, EV_READ);   //再次开启读事件
		bufferevent_free(be);			//超时清理掉链接
		return ;
	}else if(events & BEV_EVENT_ERROR){
		cout << "BEV_EVENT_ERROR" << endl;
		bufferevent_free(be);	
		return ;
	}

	//服务端的关闭事件
	if(events & BEV_EVENT_EOF)
	{
		cout << "BEV_EVENT_EOF" << endl;
		bufferevent_free(be);
	}

	if(events & BEV_EVENT_CONNECTED)
	{
		cout << "BEV_EVENT_CONNECTED" << endl;
		bufferevent_trigger(be, EV_WRITE, 0);
	}
}



void client_write_cb(bufferevent* be, void *arg)
{
	cout << "[client W]" << flush;
	FILE* fp = (FILE*) arg;
	char data[1024] = {0};

	int len = fread(data, 1, sizeof(data)-1, fp);
	if(len <= 0)
	{
		//读到结尾或在文件出错
		fclose(fp);
		//立刻清理可能会造成缓冲数据没有发送结束
		//	bufferevent_free(be);
		bufferevent_disable(be, EV_WRITE);
		return ;
	}
	sendCount += len;
	//写入buffer
	bufferevent_write(be, data, len);
	
	
}

void client_read_cb(bufferevent* be, void *arg)
{
	cout << "[client R]" << flush;
}


void listen_cb(struct evconnlistener* e, evutil_socket_t s, struct sockaddr* a, int socklen, void* arg)
{
	cout << "listen_cb " << endl;

	event_base* base = (event_base*)arg;
	//创建bufferevent上下文， BEV_OPT_CLOSE_ON_FREE清理bufferevent时关闭socket
	bufferevent* bev = bufferevent_socket_new(base, s, BEV_OPT_CLOSE_ON_FREE);
	//添加监控事件
	bufferevent_enable(bev, EV_READ | EV_WRITE);

	//设置水位
	
	//读取水位
	bufferevent_setwatermark(bev, EV_READ, 
							5,		//低水位, 默认是0
						   	10);		//高水位，0为无限制

	//写水位
	bufferevent_setwatermark(bev, EV_WRITE, 
							5,		//低水位, 默认是0, 缓冲数据低于5才会发送
						   	0); 	//高水位暂时无效

	//超时时间设置
	timeval t1 = {0, 500000};
	bufferevent_set_timeouts(bev, &t1, 0);


	//设置回调函数
	bufferevent_setcb(bev, read_cb, write_cb, event_cb, base);
	
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

	{

	//调用客户端代码
	bufferevent* bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(5001);
	evutil_inet_pton(AF_INET, "127.0.0.1", &sin.sin_addr.s_addr);
	
	FILE* fp = fopen("test_buffer_client.cpp", "rb");
	//设置回调函数
	bufferevent_setcb(bev, client_read_cb, client_write_cb, client_event_cb, fp);

	bufferevent_enable(bev, EV_READ | EV_WRITE);

	int re = bufferevent_socket_connect(bev, (sockaddr*)&sin, sizeof(sin));
	if(re == 0)
	{
		cout << "connected" << endl;
	}

	}
	if(base)
		event_base_dispatch(base);
	if(ev)
		evconnlistener_free(ev);

	if(base)
		event_base_free(base);


	return 0;


}
