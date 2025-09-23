#include "stdafx.h"
#include "WebClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//С�Ͷ�̬������
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

//��������ֵ����
template<typename T>
DynamicBuffer<T>::DynamicBuffer(const DynamicBuffer &RValue)
{
	m_pData = RValue.m_pData;
	m_nData = RValue.m_nData;
	m_nBuffer = RValue.m_nBuffer;
}

//��������ֵ��ֵ
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

//׷������
template<typename T>
void DynamicBuffer<T>::Append(const T *pData, size_t nData)
{
	ASSERT(!(pData == NULL && nData != 0));
	if (nData > m_nBuffer - m_nData)
	{
		//��Ҫ���·����ڴ�
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

//�ͷ������ڴ棬һ���ڲ���ʹ��ʱ�ŵ���
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

//�����������,�ָ�Ĭ�Ϲ�����״̬
template<typename T>
void DynamicBuffer<T>::Clear()
{
	if (m_pData && m_nBuffer == DEFAULT_SIZE)
	{
		//�Ѿ���Ĭ�Ϲ����Ĵ�С����������ݼ���
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
	//�Զ����������ض���
	DWORD dwOption = WINHTTP_OPTION_REDIRECT_POLICY_ALWAYS;
	BOOL bSuccessSet = WinHttpSetOption(m_hSession, WINHTTP_OPTION_REDIRECT_POLICY, &dwOption, sizeof(DWORD));

	ASSERT(m_hSession != NULL && bSuccessSet);
}

WebClient::~WebClient()
{
	if (m_hSession)
		WinHttpCloseHandle(m_hSession);
}

//����Cookie�����ûỰ
void WebClient::ClearAll()
{
	if (m_hSession)
		WinHttpCloseHandle(m_hSession);
	m_hSession = WinHttpOpen(m_strUserAgent, WINHTTP_ACCESS_TYPE_NO_PROXY,
		WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, NULL);
	//�Զ����������ض���
	DWORD dwOption = WINHTTP_OPTION_REDIRECT_POLICY_ALWAYS;
	BOOL bSuccessSet = WinHttpSetOption(m_hSession, WINHTTP_OPTION_REDIRECT_POLICY, &dwOption, sizeof(DWORD));

	ASSERT(m_hSession != NULL && bSuccessSet);
}


//����GET���󣬻�ȡҳ��HTML
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
	//����URL
	int iProtocal;
	WORD nPort;
	CStringW strHostName, strUrlPath;
	if (!ParseUrl(wszUrl, iProtocal, nPort, strHostName, strUrlPath))
		return INVALID_URL;

	HINTERNET hConnect = WinHttpConnect(m_hSession, strHostName, nPort, NULL);
	if (!hConnect)
		return WINHTTP_ERROR;
	//�����ܻ���
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
	//�������ͷ
	for (size_t i=0; i<vec_strHead.size(); i++)
	{
		if (!vec_strHead[i].IsEmpty())
		{
			WinHttpAddRequestHeaders(hRequest, vec_strHead[i],
				(DWORD)-1L, WINHTTP_ADDREQ_FLAG_ADD|WINHTTP_ADDREQ_FLAG_REPLACE);
		}
	}
	//��������
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
	//��������
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
	//��������
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
	//���б���ת��
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

//�����ļ�
WebClient::RetCode WebClient::DownloadFile(LPCWSTR wszUrl, LPCWSTR wszFilePath)
{
	ASSERT(wszUrl != NULL && wszFilePath != NULL);
	//����URL
	int iProtocal;
	WORD nPort;
	CStringW strHostName, strUrlPath;
	if (!ParseUrl(wszUrl, iProtocal, nPort, strHostName, strUrlPath))
		return INVALID_URL;

	HINTERNET hConnect = WinHttpConnect(m_hSession, strHostName, nPort, NULL);
	if (!hConnect)
		return WINHTTP_ERROR;
	//�����ܻ���
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
	//��������
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
	//��������
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
	//�������ݣ�д�뵽�ļ�
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
			//δ�����꣬�����쳣��ɾ���ļ�
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

//����POST���󣬻�ȡ���ص�HTML
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
	//����URL
	int iProtocal;
	WORD nPort;
	CStringW strHostName, strUrlPath;
	if (!ParseUrl(wszUrl, iProtocal, nPort, strHostName, strUrlPath))
		return INVALID_URL;

	HINTERNET hConnect = WinHttpConnect(m_hSession, strHostName, nPort, NULL);
	if (!hConnect)
		return WINHTTP_ERROR;
	//�����ܻ���
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
	//���POST�ر�������ͷ
	WinHttpAddRequestHeaders(hRequest, L"Content-Type: application/x-www-form-urlencoded",
		(DWORD)-1L, WINHTTP_ADDREQ_FLAG_ADD|WINHTTP_ADDREQ_FLAG_REPLACE);
	//�����������ͷ
	for (size_t i=0; i<vec_strHead.size(); i++)
	{
		if (!vec_strHead[i].IsEmpty())
		{
			WinHttpAddRequestHeaders(hRequest, vec_strHead[i],
				(DWORD)-1L, WINHTTP_ADDREQ_FLAG_ADD|WINHTTP_ADDREQ_FLAG_REPLACE);
		}
	}
	//ת������
	CStringA strPostData_utf8 = UTF16ToUTF8(strPostData);
	//��������
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
	//��������
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
	//��������
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

	//���б���ת��
	if (m_CharSet == UTF8)
		strHtml = UTF8ToUTF16(DyBuffData, DyBuffData.Size());
	else
		strHtml = AnsiToUTF16(DyBuffData, DyBuffData.Size());

	return SUCCESS;	
}


//����URL
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

//����ת��
CStringW WebClient::UTF8ToUTF16(LPCSTR szUTF8, int nLen /*= -1*/)
{
	//�Ȼ�ȡת������Ҫ�Ļ�������С
	DWORD nWszLen = MultiByteToWideChar(CP_UTF8, NULL, szUTF8, nLen, NULL, NULL);

	CStringW strUTF16;
	nWszLen = MultiByteToWideChar(CP_UTF8, NULL, szUTF8, nLen, strUTF16.GetBuffer(nWszLen), nWszLen);
	strUTF16.ReleaseBuffer(nWszLen);

	return strUTF16;
}

CStringW WebClient::AnsiToUTF16(LPCSTR szAnsi, int nLen /*= -1*/)
{
	//�Ȼ�ȡת������Ҫ�Ļ�������С
	DWORD nWszLen = MultiByteToWideChar(CP_ACP, NULL, szAnsi, nLen, NULL, NULL);

	CStringW strUTF16;
	nWszLen = MultiByteToWideChar(CP_ACP, NULL, szAnsi, nLen, strUTF16.GetBuffer(nWszLen), nWszLen);
	strUTF16.ReleaseBuffer(nWszLen);

	return strUTF16;
}

CStringA WebClient::UTF16ToUTF8(LPCWSTR wszUTF16, int nLen /*= -1*/)
{
	//�Ȼ�ȡת������Ҫ�Ļ�������С
	DWORD nszLen = WideCharToMultiByte(CP_UTF8, NULL, wszUTF16, nLen, NULL, NULL, NULL, NULL);

	CStringA strUTF8;
	nszLen = WideCharToMultiByte(CP_UTF8, NULL, wszUTF16, nLen, strUTF8.GetBuffer(nszLen), nszLen, NULL, NULL);
	strUTF8.ReleaseBuffer(nszLen);

	return strUTF8;
}

//���ó�ʱֵ
bool WebClient::SetTimeouts(DWORD dwConnectTimeout, DWORD dwTransmissionTimeout)
{
	return true;
	if (WinHttpSetTimeouts(m_hSession, 0, dwConnectTimeout, dwTransmissionTimeout, dwTransmissionTimeout))
		return true;
	else
		return false;
}

//���ô���
bool WebClient::SetProxy(const CStringW &strProxy)
{
	WINHTTP_PROXY_INFO ProxyInfo = {WINHTTP_ACCESS_TYPE_NAMED_PROXY,
		(LPWSTR)(LPCWSTR)strProxy, (LPWSTR)(LPCWSTR)strProxy};
	if (WinHttpSetOption(m_hSession, WINHTTP_OPTION_PROXY, &ProxyInfo, sizeof(WINHTTP_PROXY_INFO)))
		return true;
	else
		return false;
}

//ȡ������
bool WebClient::RemoveProxy()
{
	WINHTTP_PROXY_INFO ProxyInfo = {WINHTTP_ACCESS_TYPE_NO_PROXY, NULL, NULL};
	if (WinHttpSetOption(m_hSession, WINHTTP_OPTION_PROXY, &ProxyInfo, sizeof(WINHTTP_PROXY_INFO)))
		return true;
	else
		return false;
}

//����Ŀ����վ����
bool WebClient::CharSet(int iCharSet)
{
	if (iCharSet != ANSI && iCharSet != UTF8)
		return false;
	m_CharSet = iCharSet;
	return true;
}