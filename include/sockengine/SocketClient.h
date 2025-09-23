// SocketClient.h: interface for the CSocketClient class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SOCKET_CLIENT_H_
#define _SOCKET_CLIENT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxsock.h>
#include <vector>
//#include "CommonInterface.h"
#include "DataStream.h"
#define UM_MSGLOG 0x459
using std::vector;

// �����������Ϣ
class AFX_EXT_CLASS CProxyInfo
{
public:
	// ��������
	enum E_ProxyType
	{
		EPTNone = 0,	// ֱ��
		EPTHttp,		// http����
		EPTSock4,		// SOCK4����
		EPTSock5,		// SOCK5����

		//
		EPTCount
	};

public:
	CProxyInfo() { m_eProxyType = EPTNone; }
	~CProxyInfo() { }

public:
	bool32 IsUseProxy()
	{
		if (m_eProxyType <= EPTNone || m_eProxyType >= EPTCount)
			return false;

		return true;
	}

public:
	E_ProxyType		m_eProxyType;

	CString			m_StrProxyAddr;
	int32			m_iProxyPort;
	
	CString			m_StrProxyUser;
	CString			m_StrProxyPassword;
};

//////////////////////////////////////////////////////////////////////
// socket�¼�֪ͨ�ӿ�
class AFX_EXT_CLASS CSocketNotify
{
public:
	virtual void	OnSocketConnected()		= 0;	// �����������ӳɹ�
	virtual void	OnSocketRecvOneOrMore() = 0;	// ���յ�����ʱ�� һ�����ݼ���֪ͨ
	virtual void	OnSocketError()			= 0;	// ���������� ���������ӹ����У� Ҳ���������ӳɹ���
};


///////////////////////////////////////////////////////////////

// socketclient
// socket�ͻ���ͨѶ�࣬ �������������ص�bind��listen֮��Ĳ���
// ֧�ִ����ֱ���ķ�ʽ
class AFX_EXT_CLASS CSocketClient  
{
public:
	// ����״̬
	enum E_SocketState 
	{
		ESSIdle = 0,			// δ����
		ESSDnsLooking,			// ����������
        ESSConnecting,			// ������
        ESSConnected			// ���ӳɹ�
	};

	enum E_SocketError
	{
		ESENone = 0,			// ����

		// ��socket���� 
		ESEOpenSocketFail,	
		ESESocketGeneral,

		// ���籨��
		ESESocketConnectError,	// ���ӹ��̴���
		ESESocketSendError,		// �������ݴ���
		ESESocketRecvError,		// �������ݴ���
		ESESocketConnectClose,	// ���ӱ��������ر�
		
		// ��ʱ
		ESEConnectTimeOut,		// ���ӳ�ʱ
		ESESendDataTimeOut,		// �������ݳ�ʱ
		ESERecvDataTimeOut,		// �������ݳ�ʱ

		// ����
		ESEProxyError,			

		// ������
		ESEStreamSendError,	// д��inputstream����
		ESEStreamRecvError		// ��ȡoutputstream����
	};


public:
	CSocketClient(CSocketNotify &SocketNotify);
	~CSocketClient();

public:
	bool32			Construct();
	
public:
	
	
public:
	// ���ӡ��ա����߳�
	static void		CallbackThreadConnect(LPVOID lpParam);
	static void		CallbackThreadRW(LPVOID lpParam);
	static void		CallbackThreadMain(LPVOID lpParam);

private:
	// ���º������̻߳ص��д���
	void			CallbackThreadConnect();
	void			CallbackThreadRW();
	void			CallbackThreadMain();

	void			CallbackThreadConnectByHttpProxy();		// http����ʽ����
	void			CallbackThreadConnectBySock4Proxy();	// sock4����
	void			CallbackThreadConnectBySock5Proxy();	// sock5����
	void			CallbackThreadConnectDirect();			// ֱ��
	
	// ���º������̻߳ص��д����� ��ɵ�һ�Ĺ��ܣ� ���Ҳ��������� Ҫ�ܶ��˳���־����ʱ����Ӧ
	bool32			ConnectServerWithExitFlag(CString StrServerAddr, int32 iServerPort, IN bool32 &bExitFlag, bool32 bAllowTimeOut = true);
	int32			SendDataWithExitFlag(const int8 *pcBuf, int32 iNeedSendLen, IN bool32 &bExitFlag, bool32 bAllowTimeOut = true, bool32 bSendOneOrMore = true);
	int32			RecvDataWithExitFlag(int32 iNeedRecvLen, OUT int8 *pcBuf, IN bool32 &bExitFlag, bool32 bAllowTimeOut = true, bool32 bRecvOneOrMore = true);	// ����-1��ʾ����

public:
	void			SetProxy(const CProxyInfo &ProxyInfo) { m_ProxyInfo = ProxyInfo; }
	void			ConnectServer(const CString &StrServerAddr, int32 iServerPort);
	void			DisConnect();
	void			TryToDisConnect();

	// ������
public:
	CDataStream&	GetSendStream() { return m_DataStreamSend; }
	CDataStream&	GetRecvStream() { return m_DataStreamRecv; }
	
	uint32			GetTimeConnected() { return m_uiTimeConnected; }
	uint32			GetTimeLastSend() { return m_uiTimeLastSend; }
	uint32			GetTimeLastRecv() { return m_uiTimeLastRecv; }

public:
	// ״̬
	void			SetSocketState(E_SocketState eSocketState);
	E_SocketState	GetSocketState() { return m_eSocketState; }

	void			SetSocketError(E_SocketError eSocketError);
	E_SocketError	GetSocketError() { return m_eSocketError; }

	void			SetReportString(const CString &StrReport) { m_StrReport = StrReport; }
	CString			GetReportString() { return m_StrReport; }
	
private:
	// �����ա����̣߳� �ڷ������Ͷ�Ӧ���������䴫������
	void			StartConnectThread();
	void			StartRWThread();
	void			StartMainThread();

	void			StopConnectThread();
	void			StopRWThread();
	void			StopMainThread();

	void			TryToStopConnectThread();
	void			TryToStopRWThread();
	void			TryToStopMainThread();
	
	//
	bool32			OpenSocket();
	void			CloseSocket();

private:
	bool32			Lock(HANDLE hMutex);
	void			UnLock(HANDLE hMutex);
	
public:   
	// ���ӡ��ա����߳̾��
	HANDLE			m_hThreadMain, m_hThreadConnect, m_hThreadRW;
	bool32			m_bExitConnectThreadFlag;	// �˳��̱߳�־
	bool32			m_bExitRWThreadFlag;
	bool32			m_bExitMainThreadFlag;		
	
	
	// �ա���������
	CDataStream		m_DataStreamSend;
	CDataStream		m_DataStreamRecv;

	// socket�������
	SOCKET			m_hSocket;

	// ��ǰ���ӵ�ַ
	CString			m_StrServerAddr;	// ���磺202.96.134.134 �� www.google.cn
	int32			m_iServerPort;		// �������˿ڣ� Ĭ��http�˿�80�� ftp�˿�21
	
	// ������Ϣ
	CProxyInfo		m_ProxyInfo;

	// ״̬��Ϣ
	CString			m_StrReport;		// ������ʱ��״̬��Ϣ�ַ���
	E_SocketState	m_eSocketState;
	E_SocketError	m_eSocketError;

	// �¼����ն���
	CSocketNotify	&m_SocketNotify;
	
	// ����ʱ���
private:
	uint32			m_uiTimeConnected;
	uint32			m_uiTimeLastSend;
	uint32			m_uiTimeLastRecv;

	// 
private:
	int8			*m_pcBufRecvIntime;

	HANDLE			m_hMutexMain;
	HANDLE			m_hMutexConnect;
	HANDLE			m_hMutexRW;
	HANDLE			m_hMutexSend;
	
private:
	// 1022 #for trace
	HWND			m_hwndTrace;
	vector<CString> m_vecMsgLog;
};

#endif



