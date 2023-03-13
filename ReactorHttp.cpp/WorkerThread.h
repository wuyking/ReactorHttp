#pragma once
#include<thread>
#include<mutex>
#include<condition_variable>
#include"EventLoop.h"
using namespace std;

//定义子线程对应的结构体
class WorkerThread
{
public:
	WorkerThread(int index);
	~WorkerThread();
	//启动线程子线程
	void run();
	inline EventLoop* getEventLoop()
	{
		return m_evLoop;
	}
private:
	void Running();

private:
	thread* m_thread; //保存线程实例
	thread::id m_threadID;
	string m_name;
	mutex m_mutex;
	condition_variable m_cond;
	EventLoop* m_evLoop;
};

