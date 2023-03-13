#include "TcpConnection.h"
#include "HttpRequest.h"
#include<stdlib.h>
#include<stdio.h>
#include"Httpresponse.h"
#include"Log.h"

int TcpConnection::processRead(void* arg)
{
	TcpConnection* conn = static_cast<TcpConnection*>(arg);
	int socket = conn->m_channel->getSocket();
	//接收数据
	int count = conn->m_readbuf->socketRead(socket);

	Debug("接收到http请求数据： %s", conn->m_readbuf->data());
	if (count > 0)
	{
		//接收到了http的请求  ，解析http请求

#ifdef MSG_SEND_TYPE
		conn->m_channel->writeEventEnable(true);
		conn->m_evLoop->addTask(conn->m_channel, ElemType::MODIFY);
#endif 
		bool flag = conn->m_request->parseHttpRequest(conn->m_readbuf, conn->m_response
			, conn->m_writebuf, socket);
		if (!flag)//解析失败 回复一个信息 非假为真， 为真往下执行
		{
			string errMsg = "http/1.1 400 Bad Request\r\n\r\n";
			conn->m_writebuf->appendString(errMsg);
		}
	}
	else
	{
#ifdef MSG_SEND_TYPE
		//断开连接
		conn->m_evLoop->addTask(conn->m_channel, ElemType::DELETE);
#endif
	}
#ifndef MSG_SEND_TYPE
	//断开连接
	conn->m_evLoop->addTask(conn->m_channel, ElemType::DELETE);
#endif
	return 0;
}

int TcpConnection::processWrite(void* arg)
{
	Debug("开始发送数据....,基于写事件发送");
	TcpConnection* conn = static_cast<TcpConnection*>(arg);
	//发送数据
	int count = conn->m_writebuf->sendData(conn->m_channel->getSocket());
	if (count > 0)
	{
		//判断数据是否全部发送出去
		if (conn->m_writebuf->readableSize() == 0)
		{
			//不在检测 写 事件  修改channel保存的事件  下次要写的时候再设置为true
			conn->m_channel->writeEventEnable(false);
			//修改dispatcher检测集合 添加修改任务 
			conn->m_evLoop->addTask(conn->m_channel, ElemType::MODIFY);
			//删除这个写事件节点
			conn->m_evLoop->addTask(conn->m_channel, ElemType::DELETE);
		}
	}
	return 0;
}


int TcpConnection::tcpConnectionDestroy(void* arg)
{
	TcpConnection* conn = static_cast<TcpConnection*>(arg);
	if (conn != nullptr)
	{
		delete conn;
	}

	return 0;
}

TcpConnection::TcpConnection(int fd, EventLoop* evLoop)
{

	m_evLoop = evLoop;
	m_readbuf = new Buffer(10240);
	m_writebuf = new Buffer(10240);
	m_request = new HttpRequest;
	m_response = new HttpResponse;
	m_name = "Connection-" + to_string(fd);
	m_channel = new Channel(fd, fdEvent::ReadEvent, processRead, processWrite, tcpConnectionDestroy, this);
	evLoop->addTask(m_channel, ElemType::ADD);

	//Debug("和客户端建立连接， threadName：%s,threadID: %s, connName: %s", evLoop->threadName, evLoop->threadID, conn->name);
}

TcpConnection::~TcpConnection()
{

	if (m_readbuf/*/指向一块有效的数据*/ && m_readbuf->readableSize() == 0 &&
		m_writebuf && m_writebuf->readableSize() == 0)
	{
		delete m_readbuf;
		delete m_writebuf;
		delete m_request;
		delete m_response;
		m_evLoop->freeChannel(m_channel);
	}

	Debug("发送完毕 连接断开,释放资源....... connName: %s", m_name);
}
