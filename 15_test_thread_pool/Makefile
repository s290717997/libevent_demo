test_thread_pool:test_thread_pool.cpp XThread.cpp XThreadPool.cpp XFtpServerCMD.cpp
	g++ $^ -o $@ -levent -std=c++11
	./$@
clean:
	rm -rf test_thread_pool
	rm -rf *.o
