#include "channel.h"
#include<stdlib.h>
#include"Log.h"



Channel::Channel(int fd, fdEvent events, handleFunc readFunc, handleFunc writeFunc, handleFunc destroyFunc, void* arg)
{	
	m_fd = fd;
	m_events = (int)events;
	m_arg = arg;
	readCallBack = readFunc;
	writeCallBack = writeFunc;
	destroyCallBack = destroyFunc;	
}

void Channel::writeEventEnable(bool falg)
{
	if (falg)
	{
		//m_events |= (int)fdEvent::WriteEvent;
		m_events |= static_cast<int>(fdEvent::WriteEvent);
	}
	else
	{
		m_events = m_events & ~(int)fdEvent::WriteEvent;
	}

}

bool Channel::isWriteEventEnable()
{
	return m_events & (int)fdEvent::WriteEvent;
}
