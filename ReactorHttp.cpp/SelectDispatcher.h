#pragma once
#include "channel.h"
#include "EventLoop.h"
#include<string>
#include<sys/select.h>
#include"Dispatcher.h"
using namespace std;


class SelectDispatcher : public Dispatcher
{
public:
	SelectDispatcher(EventLoop* evLoop);
	~SelectDispatcher();
	//添加
	int add() override;

	//删除
	int remove() override;

	//修改
	int modify() override;

	//事件监测
	int dispatcher(int timeout = 2) override;

private:
	void setFdSet();
	void clearFdSet();

private:
	fd_set m_readSet;
	fd_set m_writeSet;
	const int m_maxSize = 1024;
};