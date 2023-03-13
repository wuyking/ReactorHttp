#include "ThreadPool.h"
#include<assert.h>
#include<stdlib.h>


ThreadPool::ThreadPool(EventLoop* mainLoop, int count)
{
	
	m_index = 0;
	m_threadNum = count;
	m_isStart = false;
	m_mainLoop = mainLoop;
	m_workerThreads.clear();

}

ThreadPool::~ThreadPool()
{
	for (auto item : m_workerThreads)
	{
		delete item;
	}
}

void ThreadPool::run()
{
	assert(!m_isStart);
	if (m_mainLoop->getThreadID() != this_thread::get_id())
	{
		exit(0);
	}

	m_isStart = true;
	//如果threanum大于0，说明有子线程，那么我们对其进行初始化
	if (m_threadNum > 0)
	{
		for (int i = 0; i < m_threadNum; ++i)
		{
			WorkerThread* subThread = new WorkerThread(i);
			subThread->run();
			m_workerThreads.push_back(subThread);
		}
	}
}

EventLoop* ThreadPool::takeWorkerEventLoop()
{
	assert(m_isStart);
	if (m_mainLoop->getThreadID() != this_thread::get_id())
	{
		exit(0);
	}


	//冲线程池里拿出一个子线程，然后取出里边的反应堆实例
	//先把反应堆实例指向主线程，防止没有子线程的情况，只能主线程的evloop顶上
	EventLoop* evLoop = m_mainLoop;
	//有子线程的情况下，我们就直接拿出子线程的evloop
	if (m_threadNum > 0)
	{
		evLoop = m_workerThreads[m_index]->getEventLoop();
		//负载均衡
		m_index = ++m_index % m_threadNum;
	}
	return evLoop;
}
