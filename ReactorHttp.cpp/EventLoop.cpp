#include "EventLoop.h"
#include<assert.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include"EpollDispatcher.h"
#include"PollDispatcher.h"
#include"SelectDispatcher.h"
#include"Log.h"


EventLoop::EventLoop() : EventLoop(string())
{
}

EventLoop::EventLoop(const string threadName)
{
	m_isQuit = true; //默认没有启动
	m_threadID = this_thread::get_id();
	m_threadName = threadName == string() ? "Mainthread" : threadName;
	m_dispatcher = new EpollDispatcher(this);
	//map
	m_channelMap.clear();
	int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, m_socketPair);
	if (ret == -1)
	{
		perror("socketpair");  
		exit(0);
	}

#if 0
	//制定规则，socketpair[0]发送数据, socketpair[1]接收数据
	Channel* channel =new Channel(m_socketPair[1], fdEvent::ReadEvent, readLocalMessage, nullptr, nullptr, this);
#else
	//绑定 bind
	auto obj = bind(&EventLoop::readMessage, this);
	Channel* channel = new Channel(m_socketPair[1], fdEvent::ReadEvent, obj, nullptr, nullptr, this);

#endif
	//添加channel到任务队列
	addTask(channel,ElemType::ADD);
	//Debug("evloop初始化成功");

}


EventLoop::~EventLoop() 
{
}

int EventLoop::run()
{
	m_isQuit = false; 
	if (m_threadID != this_thread::get_id())
	{
		return -1;
	}
	//循环处理事件
	while (!m_isQuit)
	{  
		m_dispatcher->dispatcher();
		processTaskQ();
	}
	return 0;
}

int EventLoop::eventActive(int fd, int event)
{

	if (fd < 0)
	{
		return -1;
	}
	//取出channel
	Channel* channel =m_channelMap[fd];
	assert(channel->getSocket() == fd);
	if (event & (int)fdEvent::ReadEvent && channel->readCallBack)
	{
		channel->readCallBack(const_cast<void*>(channel->getArg()));
	}
	if (event & (int)fdEvent::WriteEvent && channel->writeCallBack)
	{
		channel->writeCallBack(const_cast<void*>(channel->getArg()));
	}
	return 0;
}

int EventLoop::addTask(Channel* channel, ElemType type)
{
	//加锁， 保护共享资源
	m_mutex.lock();
	//创建新节点
	ChannelElement* node = new ChannelElement;
	node->channel = channel;
	node->type = type;
	m_taskQ.push(node);
	
	m_mutex.unlock();

	//处理节点
	/*
	细节：
	1、 对于链表节点的添加，可能是当前线程也可能是其他线程（主线程）发起的
			1.修改fd的事件是子线程发起的 由当前子线程自己处理
			2.添加的新fd的操作是由主线程发起的
	1、不能让主线程处理任务队列，要由当前子线程去处理
	*/

	if (m_threadID == this_thread::get_id())
	{
		//是当前子线程
		processTaskQ();
	}
	else
	{
		//是主线程，通知子线程处理任务队列中的任务
		//1.子线程在工作， 2.子线程被阻塞：select，poll，epoll，处理方法
		//在三个函数中加入一个自己的fd 如果有任务到来，给到这个fd， 函数中检测到这个fd有事件到来，立马就被唤醒阻塞
		taskWakeup();
		//Debug("唤醒成功");
	}
	return 0;
}

int EventLoop::processTaskQ()
{

	
	//取出头节点
	
	while (!m_taskQ.empty())
	{
		m_mutex.lock();
		ChannelElement* node = m_taskQ.front();
		m_taskQ.pop();  //取出后删除头部节点
		m_mutex.unlock();
		Channel* channel = node->channel;
		if (node->type == ElemType::ADD)
		{
			//添加
			//Debug("添加任务处理中。。。");
			add(channel);
		}

		else if (node->type == ElemType::DELETE)
		{
			//删除
			//Debug("删除任务处理中。。。");
			remove(channel);
		}

		else if (node->type == ElemType::MODIFY)
		{
			//修改

			modify(channel);
		}

		delete node;

	}

	//Debug("任务处理完成。。。");
	return 0;

}

int EventLoop::add(Channel* channel)
{
	int fd = channel->getSocket();
	
	//找到对应的fd元素位置，并存储
	if (m_channelMap.find(fd) == m_channelMap.end())
	{
		m_channelMap.insert(make_pair(fd, channel));
		m_dispatcher->setChannel(channel);
		int ret = m_dispatcher->add();
		return ret;
	}

	return -1;  //......
}

int EventLoop::remove(Channel* channel)
{
	int fd = channel->getSocket();
	if (m_channelMap.find(fd) == m_channelMap.end())
	{
		return -1;
	}
	m_dispatcher->setChannel(channel);
	int ret =m_dispatcher->remove();
	return ret;
}


int EventLoop::modify(Channel* channel)
{
	int fd = channel->getSocket();
	
	if (m_channelMap.find(fd) == m_channelMap.end())
	{
		return -1;
	}
	m_dispatcher->setChannel(channel);
	int ret = m_dispatcher->modify();
	return ret;
}

int EventLoop::freeChannel(Channel* channel)
{
	//删除channel和fd的对应关系 
	auto it = m_channelMap.find(channel->getSocket());
	if (it != m_channelMap.end())
	{
		m_channelMap.erase(it);
		close(channel->getSocket());
		delete channel;
	}
	return 0;
}

int EventLoop::readLocalMessage(void* arg)
{
	EventLoop* evLoop = static_cast<EventLoop*>(arg);
	char buf[256];
	read(evLoop->m_socketPair[1], buf, sizeof(buf));
	return 0;
}

int EventLoop::readMessage()
{
	char buf[256];
	read(m_socketPair[1], buf, sizeof(buf));
	return 0;
}

void EventLoop::taskWakeup()
{
	const char* msg = "我是代码狂人！！！";
	write(m_socketPair[0], msg, strlen(msg));
}
