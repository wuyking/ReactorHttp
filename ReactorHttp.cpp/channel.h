#pragma once
#include <functional>
//定义函数指针
//typedef int (*handleFunc)(void* arg);
//using handleFunc = int(*)(void*);
using namespace std;
enum class fdEvent
{
	Timeout = 0x01,
	ReadEvent = 0x02,
	WriteEvent = 0x04

};

//可调用对象包装器 是什么  ？     1. 函数指针。。。 2. 可调用对象（可以像函数一样使用）
//最终得到一个地址 ，但没有调用

class Channel
{
public:

	//初始化一个Channel
	using handleFunc = function <int(void*)>;
	Channel(int fd, fdEvent events, handleFunc readFunc, handleFunc writeFunc, handleFunc destroyFunc, void* arg);
	//回调函数 
	handleFunc readCallBack;
	handleFunc writeCallBack;
	handleFunc destroyCallBack;


	//修改fd的写事件（检测 or 不检测）
	void writeEventEnable(bool falg);
	//判断是否需要检测文件描述符的写事件
	bool isWriteEventEnable();


	//取出私有成员变量
	inline int getSocket()
	{
		return m_fd;
	}

	inline int getEvent()
	{
		return m_events;
	}

	inline const void* getArg()
	{
		return m_arg;
	}

private:
	//文件描述符
	int m_fd;
	//事件
	int m_events;

	//回调函数参数
	void* m_arg;

};


