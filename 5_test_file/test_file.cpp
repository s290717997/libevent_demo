/*
 * 使用echo “hello world” >> test.txt进行测试
 *
 **/
#include <iostream>
#include <event2/event.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>
using namespace std;


void read_file(evutil_socket_t fd, short event, void *arg)
{
	char buf[1024] = {0};
	int len = read(fd, buf, sizeof(buf)-1);

	if(len > 0) {
		cout << buf << endl;
	}else{
		cout << "." << flush;
		sleep(1);
	}
}

int main(int argc, const char** argv)
{
	
	
	if(signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		return 1;

	event_config* conf = event_config_new();

	event_config_require_features(conf, EV_FEATURE_FDS);
	event_base* base = event_base_new_with_config(conf);
	event_config_free(conf);

	if(!base)
	{
		cerr << "event_base_new_with_config failed!" << endl;
		return -1;
	}

	int sock = open("test.txt", O_RDONLY|O_NONBLOCK, 0);
	if(sock <= 0)
	{
		cerr << "open test_file.txt failed!" << endl;
		return -2;
	}

	lseek(sock, 0, SEEK_END);


	//监听文件描述符
	event* fev = event_new(base, sock, EV_READ|EV_PERSIST, read_file, 0);
	event_add(fev, NULL);
			



	event_base_dispatch(base);
	event_base_free(base);


	return 0;
}
