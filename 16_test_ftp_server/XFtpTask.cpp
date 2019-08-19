#include "XFtpTask.h"
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <iostream>
#include <string.h>
#include <iostream>
using namespace std;

void XFtpTask::Send(std::string data)
{
	Send(data.c_str(), data.size());
}

void XFtpTask::Send(const char*  data, int datasize)
{
	if(!bev) return ;
	
	cout << "XFtpTask::Send, data= " << data << datasize << endl;
	bufferevent_write(bev, data, datasize);
}

void XFtpTask::Close()
{
	if (bev)
	{
		bufferevent_free(bev);
		bev = 0;
	}
	if (fp)
	{
		fclose(fp);
		fp = 0;
	}
}


void XFtpTask::ConnectPORT()
{
	if(ip.empty() || port <= 0 || !base)
	{
		cout << "ConnectPORT failed ip or port is null " << endl;
		return ;
	}
	Close();
	bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	evutil_inet_pton(AF_INET, ip.c_str(), &sin.sin_addr.s_addr);
	//设置回调和权限
	SetCallback(bev);
	//timeval rt = {60, 0};
//	bufferevent_set_timeouts(bev, &rt, 0);
	int ret ;
	ret = bufferevent_socket_connect(bev, (sockaddr*)&sin, sizeof(sin));
	cout << "data channel bev conect ret = " << ret << endl;

}

void XFtpTask::ResCMD(std::string msg)
{
	if(!cmdTask | !cmdTask->bev) return;
	cout << "ResCMD: " << msg << endl;
	bufferevent_write(cmdTask->bev, msg.c_str(), msg.size());

}



void XFtpTask::SetCallback(struct bufferevent* bev)
{
	bufferevent_setcb(bev, ReadCB, WriteCB, EventCB, this);
	bufferevent_enable(bev, EV_READ | EV_WRITE);
}



void XFtpTask::ReadCB(bufferevent* bevent, void* arg)
{
	XFtpTask* t = (XFtpTask*)arg;
	t->Read(bevent);
}

void XFtpTask::WriteCB(bufferevent* bevent, void* arg)
{
	XFtpTask* t = (XFtpTask*)arg;
	cout << "WriteCB11" << endl;
	t->Write(bevent);
}

void XFtpTask::EventCB(bufferevent* bevent, short what, void* arg)
{
	XFtpTask* t = (XFtpTask*)arg;
	t->Event(bevent, what);
}
