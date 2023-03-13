#include"Dispatcher.h"
#include<sys/select.h>
#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
#include "SelectDispatcher.h"


SelectDispatcher::SelectDispatcher(EventLoop* evLoop) : Dispatcher(evLoop)
{
	
	FD_ZERO(&m_readSet);
	FD_ZERO(&m_writeSet);
	m_name = "Select";

}

SelectDispatcher::~SelectDispatcher()
{
}

int SelectDispatcher::add()
{
	
	if (m_channel->getSocket() >= m_maxSize)
	{
		return -1;
	}
	setFdSet();
	return 0;;
}

int SelectDispatcher::remove()
{
	
	clearFdSet();
	//置空后通过channel释放tcpconnection内存资源
	m_channel->destroyCallBack(const_cast<void*>(m_channel->getArg()));
	return 0;
}

int SelectDispatcher::modify()
{
	
	setFdSet();
	clearFdSet();
	return 0;
}

int SelectDispatcher::dispatcher(int timeout)
{
	struct timeval val;
	val.tv_sec = timeout;
	val.tv_usec = 0;
	fd_set rdtmp = m_readSet;
	fd_set wrtmp = m_writeSet;
	int count = select(m_maxSize, &rdtmp, &wrtmp, NULL, &val);
	if (count == -1)
	{
		perror("select");
		exit(0);
	}
	for (int i = 0; i < m_maxSize; ++i)
	{

		if (FD_ISSET(i, &rdtmp))
		{
			m_evLoop->eventActive(i, (int)fdEvent::ReadEvent);
		}
		if (FD_ISSET(i, &wrtmp))
		{
			m_evLoop->eventActive(i, (int)fdEvent::WriteEvent);
		}

	}
	return 0;
}

void SelectDispatcher::setFdSet()
{
	if (m_channel->getEvent() & (int)fdEvent::ReadEvent)
	{
		FD_SET(m_channel->getSocket(), &m_readSet);
	}
	if (m_channel->getEvent() & (int)fdEvent::WriteEvent)
	{
		FD_SET(m_channel->getSocket(), &m_writeSet);
	}
}

void SelectDispatcher::clearFdSet()
{
	if (m_channel->getEvent() & (int)fdEvent::ReadEvent)
	{
		FD_CLR(m_channel->getSocket(), &m_readSet);
	}
	if (m_channel->getEvent() & (int)fdEvent::WriteEvent)
	{
		FD_CLR(m_channel->getSocket(), &m_writeSet);
	}
}
