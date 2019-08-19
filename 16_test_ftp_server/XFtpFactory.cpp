#include "XFtpFactory.h"
#include "XFtpPORT.h"
#include "XFtpList.h"
#include "XFtpServerCMD.h"

#include "XFtpUSER.h"
XTask* XFtpFactory::CreateTask()
{
	XFtpServerCMD* x = new XFtpServerCMD();

	x->Reg("USER", new XFtpUSER());
	//注册ftp消息处理对象
	XFtpList* list = new XFtpList();
	x->Reg("PWD", list);
	x->Reg("LIST", list);
	x->Reg("PORT", new XFtpPORT());
	return x;
}



XFtpFactory::XFtpFactory()
{

}
