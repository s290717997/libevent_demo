#include "XFtpList.h"


#include <string>
using namespace std;

void XFtpList::Write(struct bufferevent* bev)
{
	cout << "XFtpList:: Write" << endl;
	ResCMD("226 Transfer complete.\r\n");
	//关闭链接
	Close();
}



void XFtpList::Event(struct bufferevent *bev, short what)
{
	if (what & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT))
	{
		cout << " what = "<< what << " BEV_EVENT_EOF | BEV_EVENT_ERROR |BEV_EVENT_TIMEOUT" << endl;
		Close();
	}
	else if(what&BEV_EVENT_CONNECTED)
	{
		cout << "XFtpLIST BEV_EVENT_CONNECTED" << endl;
	}
}

	//解析协议
void XFtpList::Parse(std::string type, std::string msg)
{
	cout << "XFtpList:: Parse type = " << type << " msg = " << msg << endl;
	string remsg = "";
	if(type == "PWD")
	{
		remsg += "257 \"";
		remsg += cmdTask->curDir;
		remsg += "\" is current dir.\n";
		ResCMD(remsg);	
	}else if(type == "LIST"){
	
		//1 150 2 发送目录数据通道 3 发送完成226 4 关闭链接
		//1链接数据通道
		ConnectPORT();
		//2 150回应
		ResCMD("150 Here comes the directory listing.\r\n");
		string listdata = "-rwxrwxrwx 1 root group 64463 Mar 14 09:53 101.jpg.\r\n";
		//回复消息， 使用数据通道发送目录
		//链接数据通道
		cout << listdata << endl;
		Send(listdata);
	}
}
