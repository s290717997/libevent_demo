#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <event2/buffer.h>
#include <iostream>
#include <signal.h>
#include <string.h>

using namespace std;



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

void Server(event_base* base);
	Server(base);

	cout << "test client" << endl;
void Client(event_base* base);
	Client(base);

	event_base_dispatch(base);


	cout << "end!" << endl;



	return 0;


}
