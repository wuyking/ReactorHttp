#include"Dispatcher.h"
#include<poll.h>
#include<stdlib.h>
#include<stdio.h>
#include "PollDispatcher.h"



pollDispatcher::pollDispatcher(EventLoop* evLoop) : Dispatcher(evLoop)
{
	
	m_maxfd = 0;
	m_fds = new struct pollfd[m_maxNode];
	for (int i = 0; i < m_maxNode; ++i)
	{
		m_fds[i].fd = -1;
		m_fds[i].events = 0;
		m_fds[i].revents = 0;
	}
	m_name = "Poll";
}

pollDispatcher::~pollDispatcher()
{
	delete[]m_fds;
}

int pollDispatcher::add()
{

	int events = 0;
	if (m_channel->getEvent() & (int)fdEvent::ReadEvent)
	{
		events |= POLLIN;
	}
	if (m_channel->getEvent() & (int)fdEvent::WriteEvent)
	{
		events |= POLLOUT;
	}
	int i = 0;
	for (; i < m_maxNode; ++i)
	{
		if (m_fds[i].fd == -1)   //..................................................
		{
			m_fds[i].events = events;
			m_fds[i].fd = m_channel->getSocket();
			m_maxfd = i < m_maxfd ?m_maxfd : i;
			break;
		}
	}
	if (i >= m_maxNode)
	{
		return -1;
	}
	return 0;
}

int pollDispatcher::remove()
{
	int i = 0;
	for (; i < m_maxNode; ++i)
	{
		if (m_fds[i].fd == m_channel->getSocket())
		{
			m_fds[i].events = 0;
			m_fds[i].revents = 0;
			m_fds[i].fd = -1;
			break;
		}
	}
	//置空后通过channel释放tcpconnection内存资源
	m_channel->destroyCallBack(const_cast<void*>(m_channel->getArg()));
	if (i >= m_maxNode)
	{
		return -1;
	}
	return 0;
}

int pollDispatcher::modify()
{

	int events = 0;
	if (m_channel->getEvent() & (int)fdEvent::ReadEvent)
	{
		events |= POLLIN;
	}
	if (m_channel->getEvent() & (int)fdEvent::WriteEvent)
	{
		events |= POLLOUT;
	}
	int i = 0;
	for (; i < m_maxNode; ++i)
	{
		if (m_fds[i].fd == m_channel->getSocket())
		{
			m_fds[i].events = events;
			break;
		}
	}
	if (i >= m_maxNode)
	{
		return -1;
	}
	return 0;
}

int pollDispatcher::dispatcher(int timeout)
{
	
	int count = poll(m_fds, m_maxfd + 1, timeout * 1000);
	if (count == -1)
	{
		perror("poll");
		exit(0);
	}
	for (int i = 0; i <= m_maxfd + 1; ++i)
	{

		if (m_fds[i].fd == -1)
		{
			continue;
		}
		if (m_fds[i].revents & POLLIN)
		{
			m_evLoop->eventActive(m_fds[i].fd, (int)fdEvent::ReadEvent);
		}
		if (m_fds[i].revents & POLLOUT)
		{
			m_evLoop->eventActive(m_fds[i].fd, (int)fdEvent::WriteEvent);
		}
	}
	return 0;
}
