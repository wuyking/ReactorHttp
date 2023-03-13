#pragma once
#include "channel.h"
#include "EventLoop.h"
#include<string>
#include<poll.h>
#include"Dispatcher.h"
using namespace std;


class pollDispatcher : public Dispatcher
{
public:
	pollDispatcher(EventLoop* evLoop);
	~pollDispatcher();
	//添加
	int add() override;

	//删除
	int remove() override;

	//修改
	int modify() override;

	//事件监测
	int dispatcher(int timeout = 2) override;


private:
	int m_maxfd;
	struct pollfd *m_fds;
	const int m_maxNode = 1024;

};