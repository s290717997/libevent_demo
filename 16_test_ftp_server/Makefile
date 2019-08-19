test_ftp_server:test_ftp_server.cpp XThread.cpp XThreadPool.cpp XFtpServerCMD.cpp XFtpFactory.cpp XFtpTask.cpp XFtpUSER.cpp XFtpList.cpp XFtpPORT.cpp
	g++ $^ -o $@ -levent -std=c++11
	./$@
clean:
	rm -rf test_ftp_server
	rm -rf *.o
