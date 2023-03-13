#include "WorkerThread.h"
#include<stdio.h>


WorkerThread::WorkerThread(int index)
{
	m_evLoop = nullptr;
	m_thread = nullptr;
	m_threadID = thread::id();
	m_name = "SubThread" + to_string(index);

}

WorkerThread::~WorkerThread()
{
	if (m_thread != nullptr)
	{
		delete m_thread;
	}
}

void WorkerThread::run()
{
	//创建子线程
	m_thread = new thread(&WorkerThread::Running, this);
	//阻塞主线程，让函数subThreadRunning中的对evLoop的实例初始话完毕，在结束这个函数
	unique_lock<mutex> locker(m_mutex); 
	while (m_evLoop == nullptr)
	{
		m_cond.wait(locker);
	}
}

//子线程回调函数
void WorkerThread::Running()
{
	m_mutex.lock();
	m_evLoop = new EventLoop(m_name);
	m_mutex.unlock();
	m_cond.notify_one();
	m_evLoop->run();

}


