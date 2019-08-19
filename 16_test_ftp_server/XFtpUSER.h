#pragma once

#include "XFtpTask.h"
#include <string>
class XFtpUSER:public XFtpTask
{
public:
	//解析协议
	virtual void Parse(std::string type, std::string msg);
	XFtpUSER();
	~XFtpUSER();	
};
