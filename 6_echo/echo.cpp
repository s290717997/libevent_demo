/* 1个事件用来监听用户的链接，1个事件用来处理用户的数据，注意清理这2个事件的资源
 * 
 * 水平触发和边缘触发：
 * 		水平触发LT：用户数据没有处理完会一直触发
 * 		边缘触发ET：数据变化的那一刻才会进入处理一次，不管是否还有数据没处理完
 *
 **/
#include <iostream>
#include <event2/event.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <error.h>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>
using namespace std;

#define SPORT  5001

//正常断开链接和超时都会进入
void client_cb(evutil_socket_t s, short w, void* arg)
{	
	//水平触发没有处理会一直触发
	//cout << "." << flush;
	//return ;
	event* ev = (event*)arg;
	//超时判断
	if(w&EV_TIMEOUT)
	{
		cout << "timeout " << endl;
		event_free(ev);
		evutil_closesocket(s);
		return ;
	}
	


	char buf[10] = {0};
	int len = recv(s, buf, sizeof(buf)-1, 0);
	if (len > 0)
	{
		cout << buf << endl;
		send(s, "ok", 2, 0);
	}else{
		cout << "." << flush;
		event_free(ev);
		evutil_closesocket(s);
	}
}

void listen_cb(evutil_socket_t s, short w, void* arg)
{
	cout << "listen cb" << endl;

	sockaddr_in sin;
	socklen_t size = sizeof(sin);
	//读取链接信息， 
	evutil_socket_t client = accept(s, (sockaddr*)&sin, &size);
	char ip[16];
	evutil_inet_ntop(AF_INET, &sin.sin_addr, ip, sizeof(ip));
	cout << "client ip is " << ip << endl;
	
	/*客户端数据读取事件*/
	event_base* base = (event_base*)arg;
	//默认水平触发
	event* ev = event_new(base, client, EV_READ|EV_PERSIST, client_cb, event_self_cbarg());
	//EV_ET代表边缘触发
	//event* ev = event_new(base, client, EV_READ|EV_PERSIST|EV_ET, client_cb, event_self_cbarg());
	
	timeval t = {10, 0};
	event_add(ev, &t);
	


}


int main(int argc, const char** argv)
{
	
	
	if(signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		return 1;


	cout << "test event server" << endl;

	event_base* base = event_base_new();

	if(!base)
	{
		cout << "event_base_new failed!" << endl;
		return -1;
	}

	evutil_socket_t sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock <= 0)
	{
		cerr << "socket error: " << strerror(errno) << endl;
		return -1;
	}

	//设置地址复用和非阻塞
	evutil_make_socket_nonblocking(sock);
	evutil_make_listen_socket_reuseable(sock);


	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SPORT);
	//绑定端口和地址
	int re = ::bind(sock, (sockaddr*)&sin, sizeof(sin));
	if(re != 0)
	{
		cerr << "bind error:" << strerror(errno) << endl;
		return -1;
	}
	//开始监听
	listen(sock, 10);


	//开始接收链接事件, 默认水平触发
	event* ev = event_new(base, sock, EV_READ|EV_PERSIST, listen_cb, base);
	event_add(ev, 0);



	event_base_dispatch(base);
	evutil_closesocket(sock);
	event_base_free(base);


	return 0;
}
