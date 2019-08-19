#include "XTask.h"
#include <vector>
class XThread;



class XThreadPool{
public:
	//单例模式
	static XThreadPool* Get()
	{
		static XThreadPool p;
		return &p;
	}
	//初始化所有线程并启动线程
	void Init(int threadCount);
	
	//分发线程
	void Dispatch(XTask* task);

private:
	//线程池数量
	int threadCount = 0;
	int lastThread = -1;
	//线程池线程
	std:: vector<XThread *> threads;
	XThreadPool(){};
};
