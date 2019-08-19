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

void http_client(struct evhttp_request* req, void* ctx)
{
	cout << "http_client_cb" << endl;
	event_base* bev = (event_base*)ctx;
	//服务器端响应错误
	if(req == NULL)
	{
		int errcode = EVUTIL_SOCKET_ERROR();
		cout << "socket error " << evutil_socket_error_to_string(errcode) << endl;
		return ;
	}
	//获取path
	const char* path = evhttp_request_get_uri(req);
	cout << "request path is " << path << endl;


	string filepath = ".";
	filepath += path;
	cout << "filepath is " << filepath << endl;
	//如果路径种有目录，需要分析出目录，并创建
	FILE* fp = fopen(filepath.c_str(), "wb");
	if(!fp)
	{
		cout << "open file" << filepath << "failed!" << endl; 
	}

	//获取返回的code eg:200 404
	cout << "Response:" << evhttp_request_get_response_code(req); //200
	cout << " " << evhttp_request_get_response_code_line(req) << endl;	//OK
	
	char buf[1024];
	evbuffer* input = evhttp_request_get_input_buffer(req);
	for(;;){
		int len = evbuffer_remove(input, buf, sizeof(buf)-1);
		if(len <= 0) break;
		buf[len] = 0;
		if(!fp)
			continue;
		fwrite(buf, 1, len, fp);
		//cout << buf << flush;
	}

	if(fp)
		fclose(fp);
	event_base_loopbreak(bev);
}


int TestPostHttp(){

	std:: cout << "test client!\n";
	//创建libevent的上下文
	event_base* base = event_base_new();
	if(base)
	{
		cout << "event_base_new success!" << endl;
	}

	//生成请求信息GET
	//string httl_url = "http://ffmpeg.club/index.html";
	string http_url = "http://127.0.0.1:8080/index.html";
	//uri
	evhttp_uri* uri = evhttp_uri_parse(http_url.c_str());
	//http https
	const char* scheme = evhttp_uri_get_scheme(uri);
	if(!scheme)
	{
		cerr << "scheme is null" << endl;
	}

	//解析port	
	int port = evhttp_uri_get_port(uri);
	if(port < 0)
	{
		if(strcmp(scheme, "http") == 0)
			port = 80;
	}
			
	cout << "port is " << port << endl;



	//host ffmpeg.club
	const char* host = evhttp_uri_get_host(uri);
	if(!host)
	{
		cerr << "host is null" << endl;
		return -1;
	}

	cout << "host is " << host << endl;
		
	const char* path = evhttp_uri_get_path(uri);
	if(!path || strlen(path) == 0)
	{
		path = "/";
	}
	if(path)
		cout << "path is " << path << endl;
	
	//解析query
	const char* query = evhttp_uri_get_query(uri);
	if(query)
	{
		cout << "query is " << query << endl;
	}else{
		cout << "query is NULL" << endl;
	}

	//bufferevent 链接http服务器
	bufferevent* bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
	evhttp_connection* evcon = evhttp_connection_base_bufferevent_new(base, NULL, bev, host,port);

	//http_client 请求 回调函数设置
	evhttp_request* req = evhttp_request_new(http_client, base);
	
	//设置请求的head信息
	evkeyvalq* output_headers = evhttp_request_get_output_headers(req);
	evhttp_add_header(output_headers, "Host", host);


	//发送post数据
	evbuffer* output = evhttp_request_get_output_buffer(req);
	evbuffer_add_printf(output, "tzj=%d&b=%d", 1, 2);

	//发送请求
	evhttp_make_request(evcon, req, EVHTTP_REQ_POST, path);
	
 

	if(base)
		event_base_dispatch(base);
	if(base)
		event_base_free(base);

}

int TestGetHttp(){

	std:: cout << "test client!\n";
	//创建libevent的上下文
	event_base* base = event_base_new();
	if(base)
	{
		cout << "event_base_new success!" << endl;
	}

	//生成请求信息GET
	//string httl_url = "http://ffmpeg.club/index.html";
	string http_url = "http://ffmpeg.club/101.jpg";
	//uri
	evhttp_uri* uri = evhttp_uri_parse(http_url.c_str());
	//http https
	const char* scheme = evhttp_uri_get_scheme(uri);
	if(!scheme)
	{
		cerr << "scheme is null" << endl;
	}

	//解析port	
	int port = evhttp_uri_get_port(uri);
	if(port < 0)
	{
		if(strcmp(scheme, "http") == 0)
			port = 80;
	}
			
	cout << "port is " << port << endl;



	//host ffmpeg.club
	const char* host = evhttp_uri_get_host(uri);
	if(!host)
	{
		cerr << "host is null" << endl;
		return -1;
	}

	cout << "host is " << host << endl;
		
	const char* path = evhttp_uri_get_path(uri);
	if(!path || strlen(path) == 0)
	{
		path = "/";
	}
	if(path)
		cout << "path is " << path << endl;
	
	//解析query
	const char* query = evhttp_uri_get_query(uri);
	if(query)
	{
		cout << "query is " << query << endl;
	}else{
		cout << "query is NULL" << endl;
	}

	//bufferevent 链接http服务器
	bufferevent* bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
	evhttp_connection* evcon = evhttp_connection_base_bufferevent_new(base, NULL, bev, host,port);

	//http_client 请求 回调函数设置
	evhttp_request* req = evhttp_request_new(http_client, base);
	
	//设置请求的head信息
	evkeyvalq* output_headers = evhttp_request_get_output_headers(req);
	evhttp_add_header(output_headers, "Host", host);

	//发送请求
	evhttp_make_request(evcon, req, EVHTTP_REQ_GET, path);
	
 

	if(base)
		event_base_dispatch(base);
	if(base)
		event_base_free(base);

}

int main()
{

	//忽略管道信号，发送数据给以关闭的
	if(signal(SIGPIPE, SIG_IGN) == SIG_ERR)
	{
		return 1;
	}
	
	TestGetHttp();
	TestPostHttp();


	return 0;

}

