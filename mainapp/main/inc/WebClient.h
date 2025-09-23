#pragma once

#include <winhttp.h>
#pragma comment(lib, "Winhttp.lib")
#include <vector>


//С�Ͷ�̬������
//��֧�ֻ�����������
template<typename T>
class DynamicBuffer
{
	enum {
		DEFAULT_SIZE = 32000,	//Ĭ�ϳ�ʼ��С
		GROW_SIEZ = 32000,		//ÿ��������С
	};
private:
	T *m_pData;
	size_t m_nData;
	size_t m_nBuffer;
public:
	explicit DynamicBuffer(const T *pData = NULL, size_t nData = 0);
	//��������ֵ����
	DynamicBuffer(const DynamicBuffer &RValue);
	//��������ֵ��ֵ
	DynamicBuffer& operator = (const DynamicBuffer &RValue);
	~DynamicBuffer();
	//׷������
	void Append(const T *pData, size_t nData);
	//�����������,�ָ�Ĭ�Ϲ�����״̬
	void Clear();
	//�ͷ������ڴ棬һ���ڲ���ʹ��ʱ�ŵ���
	bool Release();
	//��ȡ��������ݴ�С
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
	//����Cookie�����ûỰ
	void ClearAll();
	//����GET���󣬻�ȡҳ��HTML
	RetCode Get(LPCWSTR wszUrl, CStringW &strHtml);
	RetCode GetEx(LPCWSTR wszUrl, const CStringW &strHead, CStringW &strHtml);
	RetCode GetEx(LPCWSTR wszUrl, const std::vector<CStringW> &vec_strHead, CStringW &strHtml);
	//����POST���󣬻�ȡ���ص�HTML
	RetCode Post(LPCWSTR wszUrl, const CStringW &strPostData, CStringW &strHtml);
	RetCode PostEx(LPCWSTR wszUrl, const CStringW &strPostData, const CStringW &strHead, CStringW &strHtml);
	RetCode PostEx(LPCWSTR wszUrl, const CStringW &strPostData, const std::vector<CStringW> &vec_strHead, CStringW &strHtml);
	//�����ļ�
	RetCode DownloadFile(LPCWSTR wszUrl, LPCWSTR wszFilePath);
	//���ó�ʱֵ
	bool SetTimeouts(DWORD dwConnectTimeout, DWORD dwTransmissionTimeout);
	//���ô���
	bool SetProxy(const CStringW &strProxy);
	//ȡ������
	bool RemoveProxy();
	//����Ŀ����վ���룬Ĭ��ΪUTF8
	bool CharSet(int iCharSet);
private:
	enum {
		MAX_URL_LENGTH = 2084,
		TEMP_BUFFER_SIZE = 4000,
	};
	//����URL
	bool ParseUrl(LPCWSTR wszUrl, int &iProtocol, WORD &nPort, CStringW &strHostName, CStringW &strUrlPath);
	//����ת��
	CStringW UTF8ToUTF16(LPCSTR szUTF8, int nLen = -1);
	CStringW AnsiToUTF16(LPCSTR szAnsi, int nLen = -1);
	CStringA UTF16ToUTF8(LPCWSTR wszUTF16, int nLen = -1);

private:
	WebClient(const WebClient&);
	WebClient& operator = (const WebClient&);
};