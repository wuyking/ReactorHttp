#pragma once
#include"EventLoop.h"
#include"ThreadPool.h"

using namespace std;

class TcpServer
{    
public:
	TcpServer(unsigned short port, int threadNum);
	~TcpServer();

	//初始化监听套接字
	void setListener();
	//启动服务器
	void run();
	static int acceptConnection(void* arg);

private:
	EventLoop* m_mainLoop;
	ThreadPool* m_threadPool;
	int m_threadNum;

	int m_lfd;
	unsigned short m_port;
};


