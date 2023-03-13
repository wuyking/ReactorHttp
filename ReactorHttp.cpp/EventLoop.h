#pragma once
#include"Dispatcher.h"
#include<thread>
#include"channel.h"
#include<queue>
#include<map>
#include<mutex>
using namespace std;


//处理还节点中的channel的方式
enum class ElemType : char { ADD, DELETE, MODIFY };


//定义任务队列的节点
struct ChannelElement
{
	ElemType type;  //如何处理节点中的channel对应的fd事件
	Channel* channel;  //............
};

class Dispatcher;
class EventLoop
{
public:
	EventLoop();
	EventLoop(const string threadName);
	~EventLoop();

	//启动
	int run();
	//处理激活的文件描述符fd
	int eventActive(int fd, int event);
	//添加任务到任务队列
	int addTask(Channel* channel, ElemType type);//..................
	//处理任务队列中的任务
	int processTaskQ();
	//处理dispatcher中的节点
	int add(Channel* channel);
	int remove(Channel* channel);
	int modify(Channel* channel);

	//释放channel 内存
	int freeChannel(Channel* channel);
	int readMessage();
	//返回线程id
	inline thread::id getThreadID()
	{
		return m_threadID;
	}

	inline string getThreadName()  //.................
	{
		return m_threadName;
	}

	static int readLocalMessage(void* arg);
	
private:
	void taskWakeup();
private:
	//是否运行
	bool m_isQuit;
	//该指针指向子类的实例 epoll poll select
	Dispatcher* m_dispatcher;
	//任务队列
	queue<ChannelElement*> m_taskQ;
	//map
	map<int, Channel*> m_channelMap;
	//线程相关
	thread::id m_threadID;
	string m_threadName;
	mutex m_mutex;
	int m_socketPair[2]; //存储本地通信的fd  通过socketpair初始化
};



