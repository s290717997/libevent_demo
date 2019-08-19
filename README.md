# libevent_demo
根据夏曹俊老师的课程，整理出来的demo



第11个demo用zlib压缩数据的时候没有写完。

第16个demo的ftp还有bug，目前只实现了USER,PORT，PWD等指令，PORT命令的通过数据通道向客户端发数据的时候套接字已连接但是base会发生错误事件，
查了两天没查出来，消化消化以后在来解决
