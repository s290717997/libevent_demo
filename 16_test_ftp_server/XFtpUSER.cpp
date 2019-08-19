#include "XFtpUSER.h"

#include <iostream>

using namespace std;

void XFtpUSER::Parse(std::string type, std::string msg)
{
	cout << "XFtpUSER::Parse " << type << " msg = " << msg << endl;
	ResCMD("230 Login successful!\n");
}

XFtpUSER::XFtpUSER()
{

}

XFtpUSER::~XFtpUSER()
{

}
