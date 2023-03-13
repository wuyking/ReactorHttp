#include "TcpServer.h"
#include<arpa/inet.h>
#include"TcpConnection.h"
#include<stdio.h>
#include<stdlib.h>
#include "Log.h"




int TcpServer::acceptConnection(void* arg)
{
	TcpServer* server = static_cast<TcpServer*>(arg);
	//和客户端建立连接
	int cfd = accept(server->m_lfd,NULL,NULL);
	//建立好连接之后，得到了一个用于通信的fd，就要用子线程去处理了
	EventLoop* evLoop = server->m_threadPool->takeWorkerEventLoop();
	//把cfd封装进TcpConnect模块中
	new TcpConnection(cfd, evLoop);
	return 0;


}

TcpServer::TcpServer(unsigned short port, int threadNum)
{
	m_port = port;
	m_mainLoop = new EventLoop;
	m_threadNum = threadNum;
	m_threadPool = new ThreadPool(m_mainLoop, threadNum);
	setListener();

}
TcpServer::~TcpServer()
{
}

void TcpServer::setListener()
{
	
	//1.创建监听的fd
	m_lfd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_lfd == -1)
	{
		perror("socket create error");
		return;
	}
	//2.设置端口复用
	int opt = 1;
	int ret = setsockopt(m_lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
	if (ret == -1)
	{
		perror("setsockopt error");
		return;
	}
	//3.绑定
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(m_port);
	addr.sin_addr.s_addr = INADDR_ANY;
	ret = bind(m_lfd, (struct sockaddr*)&addr, sizeof addr);
	if (ret == -1)
	{
		perror("bind error");
		return;
	}
	//4.设置监听
	ret = listen(m_lfd, 128);
	if (ret == -1)
	{
		perror("listen error");
		return;
	}
}

void TcpServer::run()
{
	Debug("服务器程序已经启动了...");
//服务器启动起来，线程池也要启动
	m_threadPool->run();

	//初始化一个反应堆模型
	Channel* channel = new Channel(m_lfd, fdEvent::ReadEvent, acceptConnection, nullptr, nullptr, this);
	//添加检测任务
	m_mainLoop->addTask(channel, ElemType::ADD);
	//启动主线程反应堆模型
	m_mainLoop->run();

}
