#pragma once

#include <winhttp.h>
#pragma comment(lib, "Winhttp.lib")
#include <vector>


//小型动态缓冲区
//仅支持基本数据类型
template<typename T>
class DynamicBuffer
{
	enum {
		DEFAULT_SIZE = 32000,	//默认初始大小
		GROW_SIEZ = 32000,		//每次增长大小
	};
private:
	T *m_pData;
	size_t m_nData;
	size_t m_nBuffer;
public:
	explicit DynamicBuffer(const T *pData = NULL, size_t nData = 0);
	//允许用右值构造
	DynamicBuffer(const DynamicBuffer &RValue);
	//允许用右值赋值
	DynamicBuffer& operator = (const DynamicBuffer &RValue);
	~DynamicBuffer();
	//追加数据
	void Append(const T *pData, size_t nData);
	//清空所有数据,恢复默认构造后的状态
	void Clear();
	//释放所有内存，一般在不再使用时才调用
	bool Release();
	//获取保存的数据大小
	size_t Size()
	{
		return m_nData;
	}
	const T* Data()
	{
		return m_pData;
	}
	operator const T*()
	{
		return m_pData;
	}
};

class WebClient
{
public:
	enum RetCode {
		SUCCESS = 0,
		INVALID_URL = 1,
		WINHTTP_ERROR = 2,
		INTERNET_ERROR = 3,
		TIMEOUT_ERROR = 4,
		CREATE_FILE_ERROR = 5,
	};
	enum {
		ANSI = 1,
		UTF8 = 2,
	};
private:
	HINTERNET m_hSession;
	CStringW m_strUserAgent;
	int m_CharSet;
public:
	explicit WebClient(LPCWSTR wszUserAgent = L"Mozilla/5.0");
	~WebClient();
	//清理Cookie，重置会话
	void ClearAll();
	//发起GET请求，获取页面HTML
	RetCode Get(LPCWSTR wszUrl, CStringW &strHtml);
	RetCode GetEx(LPCWSTR wszUrl, const CStringW &strHead, CStringW &strHtml);
	RetCode GetEx(LPCWSTR wszUrl, const std::vector<CStringW> &vec_strHead, CStringW &strHtml);
	//发起POST请求，获取返回的HTML
	RetCode Post(LPCWSTR wszUrl, const CStringW &strPostData, CStringW &strHtml);
	RetCode PostEx(LPCWSTR wszUrl, const CStringW &strPostData, const CStringW &strHead, CStringW &strHtml);
	RetCode PostEx(LPCWSTR wszUrl, const CStringW &strPostData, const std::vector<CStringW> &vec_strHead, CStringW &strHtml);
	//下载文件
	RetCode DownloadFile(LPCWSTR wszUrl, LPCWSTR wszFilePath);
	//设置超时值
	bool SetTimeouts(DWORD dwConnectTimeout, DWORD dwTransmissionTimeout);
	//设置代理
	bool SetProxy(const CStringW &strProxy);
	//取消代理
	bool RemoveProxy();
	//设置目标网站编码，默认为UTF8
	bool CharSet(int iCharSet);
private:
	enum {
		MAX_URL_LENGTH = 2084,
		TEMP_BUFFER_SIZE = 4000,
	};
	//分析URL
	bool ParseUrl(LPCWSTR wszUrl, int &iProtocol, WORD &nPort, CStringW &strHostName, CStringW &strUrlPath);
	//编码转换
	CStringW UTF8ToUTF16(LPCSTR szUTF8, int nLen = -1);
	CStringW AnsiToUTF16(LPCSTR szAnsi, int nLen = -1);
	CStringA UTF16ToUTF8(LPCWSTR wszUTF16, int nLen = -1);

private:
	WebClient(const WebClient&);
	WebClient& operator = (const WebClient&);
};