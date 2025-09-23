#include "stdafx.h"
#include "WebClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//小型动态缓冲区
template<typename T>
DynamicBuffer<T>::DynamicBuffer(const T *pData /*=NULL*/, size_t nData /*=0*/)
{
	ASSERT(!(!pData && nData != 0));
	if (nData > DEFAULT_SIZE)
		m_nBuffer = nData + GROW_SIEZ;
	else
		m_nBuffer = DEFAULT_SIZE;
	m_pData = (T*)malloc(m_nBuffer * sizeof(T));
	memset(m_pData, 0, m_nBuffer * sizeof(T));
	if (pData)
		memcpy_s(m_pData, m_nBuffer * sizeof(T), pData, nData * sizeof(T));
	m_nData = nData;
}

//允许用右值构造
template<typename T>
DynamicBuffer<T>::DynamicBuffer(const DynamicBuffer &RValue)
{
	m_pData = RValue.m_pData;
	m_nData = RValue.m_nData;
	m_nBuffer = RValue.m_nBuffer;
}

//允许用右值赋值
template<typename T>
DynamicBuffer<T>& DynamicBuffer<T>::operator = (const DynamicBuffer &RValue)
{
	if (m_pData)
		free(m_pData);
	m_pData = RValue.m_pData;
	m_nData = RValue.m_nData;
	m_nBuffer = RValue.m_nBuffer;
	return *this;
}

template<typename T>
DynamicBuffer<T>::~DynamicBuffer()
{
	Release();
}

//追加数据
template<typename T>
void DynamicBuffer<T>::Append(const T *pData, size_t nData)
{
	ASSERT(!(pData == NULL && nData != 0));
	if (nData > m_nBuffer - m_nData)
	{
		//需要重新分配内存
		size_t nNewBuffer = 0;
		if (nData > GROW_SIEZ)
			nNewBuffer = m_nData + nData + GROW_SIEZ;
		else
			nNewBuffer = m_nBuffer + GROW_SIEZ;
		m_pData = (T*)realloc(m_pData, nNewBuffer * sizeof(T));
		memset(m_pData + m_nBuffer, 0, (nNewBuffer - m_nBuffer) * sizeof(T));
		m_nBuffer = nNewBuffer;
	}
	memcpy_s(m_pData + m_nData, (m_nBuffer - m_nData) * sizeof(T), pData, nData * sizeof(T));
	m_nData += nData;
}

//释放所有内存，一般在不再使用时才调用
template<typename T>
bool DynamicBuffer<T>::Release()
{
	if (m_pData)
	{
		free(m_pData);
		m_pData = NULL;
		m_nData = 0;
		m_nBuffer = 0;
		return true;
	}
	else
		return false;
}

//清空所有数据,恢复默认构造后的状态
template<typename T>
void DynamicBuffer<T>::Clear()
{
	if (m_pData && m_nBuffer == DEFAULT_SIZE)
	{
		//已经是默认构造后的大小，清空下数据即可
		memset(m_pData, 0, m_nBuffer);
		m_nData = 0;
		return;
	}
	if (m_pData)
		free(m_pData);
	m_pData = (T*)malloc(DEFAULT_SIZE * sizeof(T));
	memset(m_pData, 0, DEFAULT_SIZE * sizeof(T));
	m_nBuffer = DEFAULT_SIZE;
	m_nData = 0;
}



WebClient::WebClient(LPCWSTR wszUserAgent /*= L"Mozilla/5.0"*/)
{
	ASSERT(wszUserAgent != NULL);
	m_CharSet = UTF8;
	m_strUserAgent = wszUserAgent;
	m_hSession = WinHttpOpen(m_strUserAgent, WINHTTP_ACCESS_TYPE_NO_PROXY,
		WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, NULL);
	//自动跟踪所有重定向
	DWORD dwOption = WINHTTP_OPTION_REDIRECT_POLICY_ALWAYS;
	BOOL bSuccessSet = WinHttpSetOption(m_hSession, WINHTTP_OPTION_REDIRECT_POLICY, &dwOption, sizeof(DWORD));

	ASSERT(m_hSession != NULL && bSuccessSet);
}

WebClient::~WebClient()
{
	if (m_hSession)
		WinHttpCloseHandle(m_hSession);
}

//清理Cookie，重置会话
void WebClient::ClearAll()
{
	if (m_hSession)
		WinHttpCloseHandle(m_hSession);
	m_hSession = WinHttpOpen(m_strUserAgent, WINHTTP_ACCESS_TYPE_NO_PROXY,
		WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, NULL);
	//自动跟踪所有重定向
	DWORD dwOption = WINHTTP_OPTION_REDIRECT_POLICY_ALWAYS;
	BOOL bSuccessSet = WinHttpSetOption(m_hSession, WINHTTP_OPTION_REDIRECT_POLICY, &dwOption, sizeof(DWORD));

	ASSERT(m_hSession != NULL && bSuccessSet);
}


//发起GET请求，获取页面HTML
WebClient::RetCode WebClient::Get(LPCWSTR wszUrl, CStringW &strHtml)
{
	return GetEx(wszUrl, L"", strHtml);
}

WebClient::RetCode WebClient::GetEx(LPCWSTR wszUrl, const CStringW &strHead, CStringW &strHtml)
{
	std::vector<CStringW> vec_strHead;
	if (!strHead.IsEmpty())
		vec_strHead.push_back(strHead);
	return GetEx(wszUrl, vec_strHead, strHtml);
}

WebClient::RetCode WebClient::GetEx(LPCWSTR wszUrl, const std::vector<CStringW> &vec_strHead, CStringW &strHtml)
{
	ASSERT(wszUrl != NULL);
	//解析URL
	int iProtocal;
	WORD nPort;
	CStringW strHostName, strUrlPath;
	if (!ParseUrl(wszUrl, iProtocal, nPort, strHostName, strUrlPath))
		return INVALID_URL;

	HINTERNET hConnect = WinHttpConnect(m_hSession, strHostName, nPort, NULL);
	if (!hConnect)
		return WINHTTP_ERROR;
	//不接受缓存
	DWORD dwFlags = WINHTTP_FLAG_REFRESH;
	if (iProtocal == INTERNET_SCHEME_HTTPS)
		dwFlags |= WINHTTP_FLAG_SECURE;
	HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", strUrlPath, NULL,
		WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, dwFlags);
	if (!hRequest)
	{
		WinHttpCloseHandle(hConnect);
		return WINHTTP_ERROR;
	}
	//添加请求头
	for (size_t i=0; i<vec_strHead.size(); i++)
	{
		if (!vec_strHead[i].IsEmpty())
		{
			WinHttpAddRequestHeaders(hRequest, vec_strHead[i],
				(DWORD)-1L, WINHTTP_ADDREQ_FLAG_ADD|WINHTTP_ADDREQ_FLAG_REPLACE);
		}
	}
	//发送请求
	if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, NULL, 
		WINHTTP_NO_REQUEST_DATA, NULL, NULL, NULL))
	{
		DWORD dwErrorCode = GetLastError();
		RetCode retcode;
		if (dwErrorCode == ERROR_WINHTTP_NAME_NOT_RESOLVED || dwErrorCode == ERROR_WINHTTP_CANNOT_CONNECT ||
			dwErrorCode == ERROR_WINHTTP_INVALID_SERVER_RESPONSE || dwErrorCode == ERROR_WINHTTP_CONNECTION_ERROR)
			retcode = INTERNET_ERROR;
		else if (dwErrorCode == ERROR_WINHTTP_TIMEOUT)
			retcode = TIMEOUT_ERROR;
		else
			retcode = WINHTTP_ERROR;
		WinHttpCloseHandle(hRequest);
		WinHttpCloseHandle(hConnect);
		return retcode;
	}
	//接收内容
	if (!WinHttpReceiveResponse(hRequest, NULL))
	{
		DWORD dwErrorCode = GetLastError();
		RetCode retcode;
		if (dwErrorCode == ERROR_WINHTTP_NAME_NOT_RESOLVED || dwErrorCode == ERROR_WINHTTP_CANNOT_CONNECT ||
			dwErrorCode == ERROR_WINHTTP_INVALID_SERVER_RESPONSE || dwErrorCode == ERROR_WINHTTP_CONNECTION_ERROR)
			retcode = INTERNET_ERROR;
		else if (dwErrorCode == ERROR_WINHTTP_TIMEOUT)
			retcode = TIMEOUT_ERROR;
		else
			retcode = WINHTTP_ERROR;
		WinHttpCloseHandle(hRequest);
		WinHttpCloseHandle(hConnect);
		return retcode;
	}
	//读出内容
	DynamicBuffer<char> DyBuffData;
	char *pTempBuffer = new char[TEMP_BUFFER_SIZE];
	DWORD dwRead = 0;
	do 
	{
		memset(pTempBuffer, 0, TEMP_BUFFER_SIZE);
		if (!WinHttpReadData(hRequest, pTempBuffer, TEMP_BUFFER_SIZE, &dwRead))
		{
			DWORD dwErrorCode = GetLastError();
			RetCode retcode;
			if (dwErrorCode == ERROR_WINHTTP_NAME_NOT_RESOLVED || dwErrorCode == ERROR_WINHTTP_CANNOT_CONNECT ||
				dwErrorCode == ERROR_WINHTTP_INVALID_SERVER_RESPONSE || dwErrorCode == ERROR_WINHTTP_CONNECTION_ERROR)
				retcode = INTERNET_ERROR;
			else if (dwErrorCode == ERROR_WINHTTP_TIMEOUT)
				retcode = TIMEOUT_ERROR;
			else
				retcode = WINHTTP_ERROR;
			delete [] pTempBuffer;
			WinHttpCloseHandle(hRequest);
			WinHttpCloseHandle(hConnect);
			return retcode;
		}
		DyBuffData.Append(pTempBuffer, dwRead);

	} while (dwRead > 0);

	delete [] pTempBuffer;
	WinHttpCloseHandle(hRequest);
	WinHttpCloseHandle(hConnect);
	//进行编码转换
	if (m_CharSet == UTF8)
	{
		strHtml = UTF8ToUTF16(DyBuffData, DyBuffData.Size());
	}
	else
	{
		strHtml = AnsiToUTF16(DyBuffData, DyBuffData.Size());
	}

	return SUCCESS;
}

//下载文件
WebClient::RetCode WebClient::DownloadFile(LPCWSTR wszUrl, LPCWSTR wszFilePath)
{
	ASSERT(wszUrl != NULL && wszFilePath != NULL);
	//解析URL
	int iProtocal;
	WORD nPort;
	CStringW strHostName, strUrlPath;
	if (!ParseUrl(wszUrl, iProtocal, nPort, strHostName, strUrlPath))
		return INVALID_URL;

	HINTERNET hConnect = WinHttpConnect(m_hSession, strHostName, nPort, NULL);
	if (!hConnect)
		return WINHTTP_ERROR;
	//不接受缓存
	DWORD dwFlags = WINHTTP_FLAG_REFRESH;
	if (iProtocal == INTERNET_SCHEME_HTTPS)
		dwFlags |= WINHTTP_FLAG_SECURE;
	HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", strUrlPath, NULL,
		WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, dwFlags);
	if (!hRequest)
	{
		WinHttpCloseHandle(hConnect);
		return WINHTTP_ERROR;
	}
	//发送请求
	if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, NULL, 
		WINHTTP_NO_REQUEST_DATA, NULL, NULL, NULL))
	{
		DWORD dwErrorCode = GetLastError();
		RetCode retcode;
		if (dwErrorCode == ERROR_WINHTTP_NAME_NOT_RESOLVED || dwErrorCode == ERROR_WINHTTP_CANNOT_CONNECT ||
			dwErrorCode == ERROR_WINHTTP_INVALID_SERVER_RESPONSE || dwErrorCode == ERROR_WINHTTP_CONNECTION_ERROR)
			retcode = INTERNET_ERROR;
		else if (dwErrorCode == ERROR_WINHTTP_TIMEOUT)
			retcode = TIMEOUT_ERROR;
		else
			retcode = WINHTTP_ERROR;
		WinHttpCloseHandle(hRequest);
		WinHttpCloseHandle(hConnect);
		return retcode;
	}
	//接收内容
	if (!WinHttpReceiveResponse(hRequest, NULL))
	{
		DWORD dwErrorCode = GetLastError();
		RetCode retcode;
		if (dwErrorCode == ERROR_WINHTTP_NAME_NOT_RESOLVED || dwErrorCode == ERROR_WINHTTP_CANNOT_CONNECT ||
			dwErrorCode == ERROR_WINHTTP_INVALID_SERVER_RESPONSE || dwErrorCode == ERROR_WINHTTP_CONNECTION_ERROR)
			retcode = INTERNET_ERROR;
		else if (dwErrorCode == ERROR_WINHTTP_TIMEOUT)
			retcode = TIMEOUT_ERROR;
		else
			retcode = WINHTTP_ERROR;
		WinHttpCloseHandle(hRequest);
		WinHttpCloseHandle(hConnect);
		return retcode;
	}
	//读出内容，写入到文件
	CFile File;
	if (!File.Open(wszFilePath, CFile::modeCreate|CFile::modeWrite|CFile::shareDenyWrite))
	{
		WinHttpCloseHandle(hRequest);
		WinHttpCloseHandle(hConnect);
		return CREATE_FILE_ERROR;
	}
	char *pTempBuffer = new char[TEMP_BUFFER_SIZE];
	DWORD dwRead = 0;
	do 
	{
		memset(pTempBuffer, 0, TEMP_BUFFER_SIZE);
		if (!WinHttpReadData(hRequest, pTempBuffer, TEMP_BUFFER_SIZE, &dwRead))
		{
			DWORD dwErrorCode = GetLastError();
			RetCode retcode;
			if (dwErrorCode == ERROR_WINHTTP_NAME_NOT_RESOLVED || dwErrorCode == ERROR_WINHTTP_CANNOT_CONNECT ||
				dwErrorCode == ERROR_WINHTTP_INVALID_SERVER_RESPONSE || dwErrorCode == ERROR_WINHTTP_CONNECTION_ERROR)
				retcode = INTERNET_ERROR;
			else if (dwErrorCode == ERROR_WINHTTP_TIMEOUT)
				retcode = TIMEOUT_ERROR;
			else
				retcode = WINHTTP_ERROR;
			delete [] pTempBuffer;
			WinHttpCloseHandle(hRequest);
			WinHttpCloseHandle(hConnect);
			//未下载完，网络异常，删除文件
			File.Close();
			::DeleteFileW(wszFilePath);
			return retcode;
		}
		File.Write(pTempBuffer, dwRead);

	} while (dwRead > 0);

	delete [] pTempBuffer;
	WinHttpCloseHandle(hRequest);
	WinHttpCloseHandle(hConnect);
	File.Close();

	return SUCCESS;
}

//发起POST请求，获取返回的HTML
WebClient::RetCode WebClient::Post(LPCWSTR wszUrl, const CStringW &strPostData, CStringW &strHtml)
{
	return PostEx(wszUrl, strPostData, L"", strHtml);
}

WebClient::RetCode WebClient::PostEx(LPCWSTR wszUrl, const CStringW &strPostData, const CStringW &strHead, CStringW &strHtml)
{
	std::vector<CStringW> vec_strHead;
	if (!strHead.IsEmpty())
		vec_strHead.push_back(strHead);
	return PostEx(wszUrl, strPostData, vec_strHead, strHtml);
}

WebClient::RetCode WebClient::PostEx(LPCWSTR wszUrl, const CStringW &strPostData, const std::vector<CStringW> &vec_strHead, CStringW &strHtml)
{
	ASSERT(wszUrl != NULL);
	//解析URL
	int iProtocal;
	WORD nPort;
	CStringW strHostName, strUrlPath;
	if (!ParseUrl(wszUrl, iProtocal, nPort, strHostName, strUrlPath))
		return INVALID_URL;

	HINTERNET hConnect = WinHttpConnect(m_hSession, strHostName, nPort, NULL);
	if (!hConnect)
		return WINHTTP_ERROR;
	//不接受缓存
	DWORD dwFlags = WINHTTP_FLAG_REFRESH;
	if (iProtocal == INTERNET_SCHEME_HTTPS)
		dwFlags |= WINHTTP_FLAG_SECURE;
	HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", strUrlPath, NULL,
		WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, dwFlags);
	if (!hRequest)
	{
		WinHttpCloseHandle(hConnect);
		return WINHTTP_ERROR;
	}
	//添加POST必备的请求头
	WinHttpAddRequestHeaders(hRequest, L"Content-Type: application/x-www-form-urlencoded",
		(DWORD)-1L, WINHTTP_ADDREQ_FLAG_ADD|WINHTTP_ADDREQ_FLAG_REPLACE);
	//添加其它请求头
	for (size_t i=0; i<vec_strHead.size(); i++)
	{
		if (!vec_strHead[i].IsEmpty())
		{
			WinHttpAddRequestHeaders(hRequest, vec_strHead[i],
				(DWORD)-1L, WINHTTP_ADDREQ_FLAG_ADD|WINHTTP_ADDREQ_FLAG_REPLACE);
		}
	}
	//转换编码
	CStringA strPostData_utf8 = UTF16ToUTF8(strPostData);
	//发送请求
	if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, NULL, 
		(LPSTR)(LPCSTR)strPostData_utf8, strPostData_utf8.GetLength(), strPostData_utf8.GetLength(), NULL))
	{
		DWORD dwErrorCode = GetLastError();
		RetCode retcode;
		if (dwErrorCode == ERROR_WINHTTP_NAME_NOT_RESOLVED || dwErrorCode == ERROR_WINHTTP_CANNOT_CONNECT ||
			dwErrorCode == ERROR_WINHTTP_INVALID_SERVER_RESPONSE || dwErrorCode == ERROR_WINHTTP_CONNECTION_ERROR)
			retcode = INTERNET_ERROR;
		else if (dwErrorCode == ERROR_WINHTTP_TIMEOUT)
			retcode = TIMEOUT_ERROR;
		else
			retcode = WINHTTP_ERROR;
		WinHttpCloseHandle(hRequest);
		WinHttpCloseHandle(hConnect);
		return retcode;
	}
	//接收内容
	if (!WinHttpReceiveResponse(hRequest, NULL))
	{
		DWORD dwErrorCode = GetLastError();
		RetCode retcode;
		if (dwErrorCode == ERROR_WINHTTP_NAME_NOT_RESOLVED || dwErrorCode == ERROR_WINHTTP_CANNOT_CONNECT ||
			dwErrorCode == ERROR_WINHTTP_INVALID_SERVER_RESPONSE || dwErrorCode == ERROR_WINHTTP_CONNECTION_ERROR)
			retcode = INTERNET_ERROR;
		else if (dwErrorCode == ERROR_WINHTTP_TIMEOUT)
			retcode = TIMEOUT_ERROR;
		else
			retcode = WINHTTP_ERROR;
		WinHttpCloseHandle(hRequest);
		WinHttpCloseHandle(hConnect);
		return retcode;
	}
	//读出内容
	DynamicBuffer<char> DyBuffData;
	char *pTempBuffer = new char[TEMP_BUFFER_SIZE];
	DWORD dwRead = 0;
	do 
	{
		memset(pTempBuffer, 0, TEMP_BUFFER_SIZE);
		if (!WinHttpReadData(hRequest, pTempBuffer, TEMP_BUFFER_SIZE, &dwRead))
		{
			DWORD dwErrorCode = GetLastError();
			RetCode retcode;
			if (dwErrorCode == ERROR_WINHTTP_NAME_NOT_RESOLVED || dwErrorCode == ERROR_WINHTTP_CANNOT_CONNECT ||
				dwErrorCode == ERROR_WINHTTP_INVALID_SERVER_RESPONSE || dwErrorCode == ERROR_WINHTTP_CONNECTION_ERROR)
				retcode = INTERNET_ERROR;
			else if (dwErrorCode == ERROR_WINHTTP_TIMEOUT)
				retcode = TIMEOUT_ERROR;
			else
				retcode = WINHTTP_ERROR;
			delete [] pTempBuffer;
			WinHttpCloseHandle(hRequest);
			WinHttpCloseHandle(hConnect);
			return retcode;
		}
		DyBuffData.Append(pTempBuffer, dwRead);

	} while (dwRead > 0);

	delete [] pTempBuffer;
	WinHttpCloseHandle(hRequest);
	WinHttpCloseHandle(hConnect);

	//进行编码转换
	if (m_CharSet == UTF8)
		strHtml = UTF8ToUTF16(DyBuffData, DyBuffData.Size());
	else
		strHtml = AnsiToUTF16(DyBuffData, DyBuffData.Size());

	return SUCCESS;	
}


//分析URL
bool WebClient::ParseUrl(LPCWSTR wszUrl, int &iProtocol, WORD &nPort, CStringW &strHostName, CStringW &strUrlPath)
{
	ASSERT(wszUrl != NULL);

	WCHAR *wszHostName = new WCHAR[MAX_URL_LENGTH+1];
	WCHAR *wszUrlPath = new WCHAR[MAX_URL_LENGTH+1];
	memset(wszHostName, 0 , MAX_URL_LENGTH+1);
	memset(wszUrlPath, 0, MAX_URL_LENGTH+1);
	URL_COMPONENTS urlComp = {sizeof(URL_COMPONENTS)};
	urlComp.lpszHostName = wszHostName;
	urlComp.dwHostNameLength = MAX_URL_LENGTH;
	urlComp.lpszUrlPath = wszUrlPath;
	urlComp.dwUrlPathLength = MAX_URL_LENGTH;
	
	BOOL bSuccessParse = WinHttpCrackUrl(wszUrl, NULL, NULL, &urlComp);
	if (bSuccessParse)
	{
		iProtocol = urlComp.nScheme;
		nPort = urlComp.nPort;
		strHostName = urlComp.lpszHostName;
		strUrlPath = urlComp.lpszUrlPath;
	}

	delete [] wszHostName;
	delete [] wszUrlPath;
	if (bSuccessParse)
		return true;
	else
		return false;
}

//编码转换
CStringW WebClient::UTF8ToUTF16(LPCSTR szUTF8, int nLen /*= -1*/)
{
	//先获取转换后需要的缓冲区大小
	DWORD nWszLen = MultiByteToWideChar(CP_UTF8, NULL, szUTF8, nLen, NULL, NULL);

	CStringW strUTF16;
	nWszLen = MultiByteToWideChar(CP_UTF8, NULL, szUTF8, nLen, strUTF16.GetBuffer(nWszLen), nWszLen);
	strUTF16.ReleaseBuffer(nWszLen);

	return strUTF16;
}

CStringW WebClient::AnsiToUTF16(LPCSTR szAnsi, int nLen /*= -1*/)
{
	//先获取转换后需要的缓冲区大小
	DWORD nWszLen = MultiByteToWideChar(CP_ACP, NULL, szAnsi, nLen, NULL, NULL);

	CStringW strUTF16;
	nWszLen = MultiByteToWideChar(CP_ACP, NULL, szAnsi, nLen, strUTF16.GetBuffer(nWszLen), nWszLen);
	strUTF16.ReleaseBuffer(nWszLen);

	return strUTF16;
}

CStringA WebClient::UTF16ToUTF8(LPCWSTR wszUTF16, int nLen /*= -1*/)
{
	//先获取转换后需要的缓冲区大小
	DWORD nszLen = WideCharToMultiByte(CP_UTF8, NULL, wszUTF16, nLen, NULL, NULL, NULL, NULL);

	CStringA strUTF8;
	nszLen = WideCharToMultiByte(CP_UTF8, NULL, wszUTF16, nLen, strUTF8.GetBuffer(nszLen), nszLen, NULL, NULL);
	strUTF8.ReleaseBuffer(nszLen);

	return strUTF8;
}

//设置超时值
bool WebClient::SetTimeouts(DWORD dwConnectTimeout, DWORD dwTransmissionTimeout)
{
	return true;
	if (WinHttpSetTimeouts(m_hSession, 0, dwConnectTimeout, dwTransmissionTimeout, dwTransmissionTimeout))
		return true;
	else
		return false;
}

//设置代理
bool WebClient::SetProxy(const CStringW &strProxy)
{
	WINHTTP_PROXY_INFO ProxyInfo = {WINHTTP_ACCESS_TYPE_NAMED_PROXY,
		(LPWSTR)(LPCWSTR)strProxy, (LPWSTR)(LPCWSTR)strProxy};
	if (WinHttpSetOption(m_hSession, WINHTTP_OPTION_PROXY, &ProxyInfo, sizeof(WINHTTP_PROXY_INFO)))
		return true;
	else
		return false;
}

//取消代理
bool WebClient::RemoveProxy()
{
	WINHTTP_PROXY_INFO ProxyInfo = {WINHTTP_ACCESS_TYPE_NO_PROXY, NULL, NULL};
	if (WinHttpSetOption(m_hSession, WINHTTP_OPTION_PROXY, &ProxyInfo, sizeof(WINHTTP_PROXY_INFO)))
		return true;
	else
		return false;
}

//设置目标网站编码
bool WebClient::CharSet(int iCharSet)
{
	if (iCharSet != ANSI && iCharSet != UTF8)
		return false;
	m_CharSet = iCharSet;
	return true;
}