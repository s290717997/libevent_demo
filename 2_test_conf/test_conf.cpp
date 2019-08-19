/*
 * linux环境：
 * 该程序为了设置libevent的特征和网络模型
 * 1.支持的网络模型
 *  1) epoll
 *  2) poll
 *  3) select
 *  默认是使用epoll，可以通过api来设置为其他，如果设置了特征为FDS，则会该为poll，FDS不支持epoll
 *	windows环境还支持一种iocp，暂不考虑win环境
 *
 * 2.支持的特征
 *  1) EV_FEATURE_ET = 0x01
 *  2) EV_FEATURE_O1 = 0x02
 *  3) EV_FEATURE_FDS = 0x04
 *  4) EV_FEATURE_EARLY_CLOSE = 0x08
 *	linux下默认为支持前2种，可以通过api更改
 *
 * */


#include <event2/event.h>
#include <event2/listener.h>
#include <iostream>
#include <signal.h>
#include <string.h>

using namespace std;
#define SPORT 5001

void listen_cb(struct evconnlistener* e, evutil_socket_t s, struct sockaddr* a, int socklen, void* arg)
{
	cout << "listen_cb " << endl;
}

int main()
{

	//忽略管道信号，发送数据给以关闭的
	if(signal(SIGPIPE, SIG_IGN) == SIG_ERR)
	{
		return 1;
	}

	//初始化配置libevent上下文
	

	//配置上下文
	event_config* conf = event_config_new();


	//get支持的网络模式
	cout << "supported_methods: " << endl;
	const char** methods = event_get_supported_methods();
	for(int i = 0; methods[i] != NULL; i++)
	{
		cout << methods[i] << endl;
	}

	//set特征,设置了EV_FEATURE_FDS后其他特征就无法设置
	//event_config_require_features(conf, EV_FEATURE_ET | EV_FEATURE_FDS);
	//event_config_require_features(conf, EV_FEATURE_FDS);


	/*通过‘避免’来取消epoll和poll的支持*/
	event_config_avoid_method(conf, "epoll");
	event_config_avoid_method(conf, "poll");



	//初始化配置libevent上下文
	event_base* base = event_base_new_with_config(conf);

	//释放掉配置的指针
	event_config_free(conf);

	



	if(!base)
	{
		cerr << "event_base_new_with_config failed! " << endl;
	}else{
		/*获取当前网络模型*/
		cout << "current method is " << event_base_get_method(base) << endl;
	



		/*查看set的特征是否生效*/
		int f =	event_base_get_features(base);
		if(f & EV_FEATURE_ET)
			cout << "EV_FEATURE_ET events are supported." << endl;
		else
			cout << "EV_FEATURE_ET events are not supported." << endl;

		if(f & EV_FEATURE_O1)
			cout << "EV_FEATURE_01 events are supported." << endl;
		else
			cout << "EV_FEATURE_01 events are not supported." << endl;

		if(f & EV_FEATURE_FDS)
			cout << "EV_FEATURE_FDS events are supported." << endl;
		else
			cout << "EV_FEATURE_FDS events are not supported." << endl;

		if(f & EV_FEATURE_EARLY_CLOSE)
			cout << "EV_FEATURE_EARLY_CLOSE events are supported." << endl;
		else
			cout << "EV_FEATURE_EARLY_CLOSE events are not supported." << endl;


		cout << "event_base_new_with_config success!" << endl;

		sockaddr_in sin;
		memset(&sin, 0, sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_port = htons(SPORT);

		evconnlistener* ev = evconnlistener_new_bind(base, listen_cb, base, 10, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, (sockaddr*)&sin, sizeof(sin)); 

		if(base)
			event_base_dispatch(base);
		if(ev)
			evconnlistener_free(ev);
		if(base)
			event_base_free(base);
	}
	return 0;



}
