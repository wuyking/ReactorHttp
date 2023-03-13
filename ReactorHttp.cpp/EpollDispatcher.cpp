#include"Dispatcher.h"
#include<sys/epoll.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdio.h> 
#include "EpollDispatcher.h"


EpollDispatcher::EpollDispatcher(EventLoop* evLoop) : Dispatcher(evLoop)
{

	m_epfd = epoll_create(10);
	if (m_epfd == -1)
	{
		perror("epoll_create");
		exit(0);
	}
	m_events = new struct epoll_event[m_maxNode];
	m_name = "Epoll";

}

EpollDispatcher::~EpollDispatcher()
{
	close(m_epfd);
	delete[]m_events;
}

int EpollDispatcher::add()
{
	int ret = epollCtl(EPOLL_CTL_ADD);
	if (ret == -1)
	{
		perror("epoll_ctl add");
		exit(0);
	}
	return ret;
}

int EpollDispatcher::remove()
{
	int ret = epollCtl(EPOLL_CTL_DEL);
	if (ret == -1)
	{
		perror("epoll_ctl_del");
		exit(0);
	}
	//置空后通过channel释放tcpconnection内存资源
	m_channel->destroyCallBack(const_cast<void*>(m_channel->getArg()));
	return ret;
}

int EpollDispatcher::modify()
{
	int ret = epollCtl(EPOLL_CTL_MOD);
	if (ret == -1)
	{
		perror("epoll_ctl_mod");
		exit(0);
	}
	return ret;
}

int EpollDispatcher::dispatcher(int timeout)
{
	
	int count = epoll_wait(m_epfd, m_events, m_maxNode, timeout * 1000);
	for (int i = 0; i < count; ++i)
	{
		int events = m_events[i].events;
		int fd = m_events[i].data.fd;
		if (events & EPOLLERR || events & EPOLLHUP)
		{
			//对方断开了连接，删除fd
			//epoll
			continue;
		}
		if (events & EPOLLIN)
		{
			m_evLoop->eventActive(fd, (int)fdEvent::ReadEvent);
		}
		if (events & EPOLLOUT)
		{
			m_evLoop->eventActive(fd, (int)fdEvent::WriteEvent);
		}
	}
	return 0;
}

int EpollDispatcher::epollCtl(int op)
{
	struct epoll_event ev;
	ev.data.fd = m_channel->getSocket();
	int events = 0;
	if (m_channel->getEvent() & static_cast<int>(fdEvent::ReadEvent))//..................
	{
		events |= EPOLLIN;
	}
	if (m_channel->getEvent() & static_cast<int>(fdEvent::WriteEvent))//...............
	{
		events |= EPOLLOUT;
	}
	ev.events = events;
	int ret = epoll_ctl(m_epfd, op, m_channel->getSocket(), &ev);
	return ret;
}
