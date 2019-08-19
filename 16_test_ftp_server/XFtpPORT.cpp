#include "XFtpPORT.h"
#include <vector>
#include <iostream>

using namespace std;


void XFtpPORT::Parse(std::string type, std::string msg)
{
	//PORT 127.0.0.1, 70, 96
	
	//只获取IP和端口，不链接
	
	vector<string> vals;
	string tmp = "";
	for(int i = 0; i < msg.size(); i++)
	{
		if(msg[i] == ','|| msg[i] == '\r')
		{
			vals.push_back(tmp);
			tmp = "";
			continue;
		}
		tmp += msg[i];
	}

	//for(int i = 0; i < vals.size(); i++)
	//{
	//	cout << vals[i] << endl;
	//}
	if(vals.size() != 6)
	{
		//PORT格式有误
		ResCMD("501 Syntax error in parameters or arguments");
	}
	ip = vals[0] + "." +vals[1] + "."+vals[2]+"."+vals[3];
	//port = n5*256
	
	port  = atoi(vals[4].c_str()) * 256 + atoi(vals[5].c_str());
	cout << "Port port ip " << ip << ":" << port  << endl;
	ResCMD("200 PORT command successful.\r\n");

}
