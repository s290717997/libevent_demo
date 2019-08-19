#pragma once

#include "XTask.h"
class XFtpServerCMD:public XTask
{
public:
	virtual bool Init();
	XFtpServerCMD();
	~XFtpServerCMD();
};
