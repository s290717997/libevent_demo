#pragma once
#include "XFtpTask.h"

class XFtpPORT:public XFtpTask
{
public:
	void Parse(std::string type, std::string msg);
};
