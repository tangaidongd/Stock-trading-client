#ifndef _GGTONG_COMMUNICATION_H_
#define _GGTONG_COMMUNICATION_H_

#include "typedef.h"
#include "SocketClient.h"
#include "sharestruct.h"
#include "synch.h"
#include "proxy_auth_client_base.h"
#include "XTimer.h"

using namespace auth;

class CDataService;
class CCommManager;

// ��֤״̬
enum E_AuthState
{
	EASAuthSuccessed = 0,		// �ɹ�
	EASAuthing,					// ������֤
	EASAuthFailed,				// 
		
	EASAuthCount
};

class AFX_EXT_CLASS CServerState
{
public:
	bool32		m_bConnected;			// �Ƿ���������
	E_AuthState	m_eAuthState;			// ��֤״̬

	uint32		m_uiKeepSecond;			// �������Ͽ�ʼ������ά��ʱ�䣨�룩
	uint32		m_uiSendBytes;			// ��������
	uint32		m_uiRecvBytes;			// ��������
	uint32		m_uiPingValue;			// pingֵ�� ��Ӧ��̨���������ٶ�

	CString		m_StrServerName;		// ����
	CString		m_StrServerAddr;		// Ip ��ַ
	CString		m_StrServerSummary;		// ����������
	
	int32		m_iServerID;			// ID
	uint32		m_uiServerPort;			// �˿�
	uint32		m_uiServerHttpPort;		// HTTP�˿�
	
	int32		m_iOnLineCount;			// ������
	int32		m_iMaxCount;			// ���������
	int32		m_iGroup;				// ������
	int32		m_iNetType;				// ����������������
	
	uint32		m_iSortIndex;			// ��������
	int32		m_iSortScore;			// ����÷�

};

// ͨѶ����࣬ �����˶��ⲿģ����ṩ�Ĺ��ܽӿ�
class AFX_EXT_CLASS CAbsCommunicationShow
{
public:
	virtual void	GetServerSummary(OUT CString &StrAddr, OUT uint32 &uiPort, OUT CString &StrSummary, OUT CProxyInfo &ProxyInfo) = 0;
	virtual void	GetServerState(OUT CServerState &ServerState) = 0;
	virtual bool32	IsSupportService(int32 iMarketId, E_DataServiceType eDataServiceType) = 0;
};

class AFX_EXT_CLASS CCommunication : public CAbsCommunicationShow, public CSocketNotify, public CXTimerAgent
{
	

	friend class CCommManager;

public:
	CCommunication(int32 iCommunicationId, const T_ServerInfo& stServerInfo, CCommManager &CommManager);
	~CCommunication();

public:
	bool32			Construct();

	// from XTimer
public:
	virtual void	OnXTimer(int32 nIdEvent);

	// from CSocketNotify
public: 
	virtual void	OnSocketConnected();
	virtual void	OnSocketRecvOneOrMore();
	virtual void	OnSocketError();
//	virtual void	OnSocketLongTimeNoRecvData();

	// from CAbsCommunicationShow
public:
	virtual void	GetServerSummary(OUT CString &StrAddr, OUT uint32 &uiPort, OUT CString &StrSummary, OUT CProxyInfo &ProxyInfo);
	virtual void	GetServerState(OUT CServerState &ServerState);
	virtual bool32	IsSupportService(int32 iMarketId, E_DataServiceType eDataServiceType);

public:
	void			StartService();
	void			StopService();

	// ��������������ݣ� ����<=0 ��ʾ����
	int32			RequestNow(IN CMmiCommBase *pMmiCommBase);

public:
	bool32			IsConnected()							{ return m_bConnected; }
	int32			GetCommunicationId()					{ return m_iCommunicationId; }

	bool32			IsAuthSuccessed()						{ return (EASAuthSuccessed == m_eAuthState); }
	
	E_AuthState		GetAuthState()							{ return m_eAuthState; }
	void			SetAuthState(E_AuthState eAuthState)	{ m_eAuthState = eAuthState; }

protected:
	void			AddDataService(CDataService *pDataService);

protected:
	// ����ı�ţ������������ӳɹ���ʧ��ʱ�� ����׼ȷ֪������̨�����������
	int32			m_iCommunicationId;

	// ��ͨѶ��Ӧ�ķ�������Ϣ(����֤������ȡ��)
	CString			m_StrServerName;		// ����
	CString			m_StrServerAddr;		// Ip ��ַ
	CString			m_StrServerSummary;		// ����������

	int32			m_iServerID;			// ID
	uint32			m_uiServerPort;			// �˿�
	uint32			m_uiServerHttpPort;		// HTTP�˿�

	int32			m_iOnLineCount;			// ������
	int32			m_iMaxCount;			// ���������
	int32			m_iGroup;				// ������
	int32			m_iNetType;				// ����������������
	
	bool32			m_bSomethingWrong;

	// ��ͨѶ�Ĵ�����Ϣ
	CProxyInfo		m_ProxyInfo;			// ������Ϣ

	// 
	bool32			m_bConnected;			// �Ƿ����ӳɹ�				
	E_AuthState		m_eAuthState;			// ��֤״̬

	// ͨѶ����
	CSocketClient	*m_pSocketClient;

public:
	uint32			m_uiPingValue;			// PING ֵ
	uint32			m_iSortIndex;			// ��������
	int32			m_iSortScore;			// ����÷�

private:
	CArray<CDataService*, CDataService*> m_DataServiceList;	// ��¼����ЩDataService���ĸ�����
	LockSingle		m_LockServiceList;

private:
	CCommManager	&m_CommManager;

private:
	bool32			m_bLongTimeNoRecvData;
};

#endif



