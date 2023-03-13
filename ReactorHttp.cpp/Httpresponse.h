#pragma once
#include "Buffer.h"
#include <map>
#include<functional>
using namespace std;
//定义枚举类型  状态码
enum class StatusCode
{
	Unknown,
	OK = 200,
	MovePermanently = 301,
	MoveTemporarily = 302,
	BadRequest = 400,
	NotFound = 404
};



//定义结构体
class HttpResponse
{
public:
	HttpResponse();
	~HttpResponse();
	
	//添加响应头
	void addHeader(const string key, const string value);
	//组织http响应数据
	void prepareMsg(Buffer* sendBuf, int socket);
	function<void(const string, struct Buffer*, int )> sendDataFunc;  //定义一个函数指针，用来组织回复给客户端

	inline void setFIleName(string name)
	{
		m_fileName = name;
	}
	inline void setStateCode(StatusCode code)
	{
		m_statusCode = code;
	}
private:
	//状态行, 状态码，状态描述
	StatusCode m_statusCode;
	string m_fileName;
	//响应头 键值对
	map<string,string> m_headers;
	//定义状态码和状态描述的对应关系 
	const map<int, string>m_info = {
		{200, "OK"},
		{301, "MovePermanently"},
		{302, "MoveTemporarily"},
		{400, "BadRequest"},
		{404, "NotFound"}
	};

};

