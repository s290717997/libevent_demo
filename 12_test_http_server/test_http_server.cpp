#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/keyvalq_struct.h>
#include <iostream>
#include <signal.h>
#include <string.h>

using namespace std;

#define SPORT 5001


#define WEBROOT	"."
#define DEFAULTINDEX "index.html"

void evhttp_cb(struct evhttp_request* request, void *arg)
{
	cout << "http_cb" << endl;
	//1 获取浏览器的请求信息
	//uri
	const char* uri = evhttp_request_get_uri(request);
	cout << "uri: " << uri << endl;
	//请求行
	string cmdtype;
	switch(evhttp_request_get_command(request))
	{
		case EVHTTP_REQ_GET:
			cmdtype = "GET";
			break;
		case EVHTTP_REQ_POST:
			cmdtype = "POST";
			break;

	}
	cout << "cmdtype " <<  cmdtype << endl;
	//请求消息报头
	evkeyvalq* headers = evhttp_request_get_input_headers(request);
	cout << "=========headers===========\n" << endl;
	for(evkeyval* p = headers->tqh_first; p != NULL; p = p->next.tqe_next)
	{
		cout << p->key << ":" << p->value << endl;
	}		
	//请求正文（GET为空， POST有表单信息）
	evbuffer* inbuf = evhttp_request_get_input_buffer(request);
	char buf[1024] = {0};
	cout << "========input data=========" << endl;
	while(evbuffer_get_length(inbuf))
	{
		int n =	evbuffer_remove(inbuf, buf, sizeof(buf) - 1);
		if(n > 0)
		{
			buf[n] = '\0';
			cout << buf << endl;
		}

	}
	//通过uri分析出请求的文件
	//设置根目录 WEBROOT
	string filepath = WEBROOT;
	filepath += uri;
	if(strcmp(uri, "/") == 0)
	{
		filepath += DEFAULTINDEX;
	}
	//消息报头
	evkeyvalq* outhead = evhttp_request_get_output_headers(request);
	//要支持图片，js，css，下载zip文件
	//获取文件的后缀
	int pos = filepath.rfind('.');
	string postfix = filepath.substr(pos+1, filepath.size() - (pos + 1));
	if(postfix == "jpg" || postfix == "gif" || postfix == "png")
	{
		string tmp = "image/" + postfix;
		evhttp_add_header(outhead, "Content-Type", tmp.c_str());
	}else if(postfix == "zip"){
		evhttp_add_header(outhead, "Content-Type", "application/zip");
	//}else if(postfix == "html"){
		//evhttp_add_header(outhead, "Content-Type", "test/html");
	}else if(postfix == "css"){
		evhttp_add_header(outhead, "Content-Type", "text/css");
	}



	FILE* fp = fopen(filepath.c_str(), "rb");
	if(!fp)
	{
		evhttp_send_reply(request, HTTP_NOTFOUND, "", 0);
		return ;
	}
	
	//2 回复浏览器
	//状态行， 消息报头， 响应正文
	evbuffer* outbuf = evhttp_request_get_output_buffer(request);
	for(;;)
	{
		int len = fread(buf, 1, sizeof(buf), fp);
		if(len <= 0) break;
		evbuffer_add(outbuf, buf, len);
	}
	fclose(fp);
	evhttp_send_reply(request, HTTP_OK, "", outbuf);
	
	

}


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
	//设置监听的端口和地址
	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SPORT);


	//http 服务器
	//创建evhttp上下文
	evhttp* evh = evhttp_new(base);
		
	//2 绑定端口和ip
	if(evhttp_bind_socket(evh, "0.0.0.0", 8080) != 0)
	{
		cout << "evhttp_bind_socket failed!" << endl;
	}

	//3设定回调函数
	evhttp_set_gencb(evh, evhttp_cb, 0);

	



	if(base)
		event_base_dispatch(base);
	if(base)
		event_base_free(base);
	if(evh)
		evhttp_free(evh);

	return 0;


}
