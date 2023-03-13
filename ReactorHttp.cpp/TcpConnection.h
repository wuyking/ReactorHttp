#pragma once
#include"EventLoop.h"
#include"channel.h"
#include"Buffer.h"
#include"HttpRequest.h"
#include"Httpresponse.h"


class TcpConnection
{
public:
	TcpConnection(int fd, EventLoop* evLoop);
	~TcpConnection();

	static int processRead(void* arg);
	static int processWrite(void* arg);
	static int tcpConnectionDestroy(void* arg);

private:
	string m_name;
	EventLoop* m_evLoop;
	Channel* m_channel;
	Buffer* m_readbuf;
	Buffer* m_writebuf;

	//http
	HttpRequest* m_request;
	HttpResponse* m_response;
};

