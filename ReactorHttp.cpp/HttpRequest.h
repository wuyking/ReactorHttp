#pragma once
#include"Buffer.h"
#include<stdbool.h>
#include<stdlib.h>
#include"Httpresponse.h"
#include<map>

using namespace std;
//请求头键值对结构体


//当前解析状态枚举
enum class ProcessState:char
{
	ParseReqLine,
	ParseReqHeads,
	ParseReqBody,
	ParseReqDone
};
//定义http请求结构体
class HttpRequest
{
public:
	HttpRequest();
	~HttpRequest();

	//会反复向httpRequest中写入数据，所以我们要重置结构体
	void reset();

	//获取处理状态
	inline ProcessState getState()
	{
		return m_curState;
	}
	inline void setState(ProcessState state)
	{
		m_curState = state;
	}
	//添加请求头
	void addHeader(const string key, const string value);
	//根据key得到请求头的value
	string getHeader(const string key);
	//解析请求行
	bool parseRequestLine(Buffer* readBuf);
	//解析请求头
	bool parseRequestHeader(Buffer* readBuf);  //多了一个struct.....................


	//解析http请求协议
	bool parseHttpRequest(Buffer* readBuf,
		HttpResponse* response, Buffer* sendBuf, int socket);

	//处理http请求协议
	bool processHttpRequest(HttpResponse* response);
	//解码字符串
	string decodeMsg(string from);
	const string getFileType(const string name);
	static void sendFile(string fileName, Buffer* sendBuf, int cfd);
	static void sendDir(string dirName, Buffer* sendBuf, int cfd);
	inline void setMethod(string method)
	{
		m_method = method;
	}
	inline void setUrl(string url)
	{
		m_url = url;
	}
	inline void setVersion(string version)
	{
		m_version = version;
	}

private:
	char* splitRequestLine(const char* start, const char* end, const char* sub,
		function<void(string)> callback);

	// 将字符转换为整形数
	int hexToDec(char c);


private:

	string m_method;
	string m_url;
	string m_version;
	map<string, string>m_reqHeaders;
	ProcessState m_curState;
};


