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

// 认证状态
enum E_AuthState
{
	EASAuthSuccessed = 0,		// 成功
	EASAuthing,					// 正在认证
	EASAuthFailed,				// 
		
	EASAuthCount
};

class AFX_EXT_CLASS CServerState
{
public:
	bool32		m_bConnected;			// 是否已连接上
	E_AuthState	m_eAuthState;			// 认证状态

	uint32		m_uiKeepSecond;			// 从连接上开始到现在维持时间（秒）
	uint32		m_uiSendBytes;			// 上行流量
	uint32		m_uiRecvBytes;			// 下行流量
	uint32		m_uiPingValue;			// ping值， 反应这台服务器的速度

	CString		m_StrServerName;		// 名字
	CString		m_StrServerAddr;		// Ip 地址
	CString		m_StrServerSummary;		// 服务器描述
	
	int32		m_iServerID;			// ID
	uint32		m_uiServerPort;			// 端口
	uint32		m_uiServerHttpPort;		// HTTP端口
	
	int32		m_iOnLineCount;			// 在线数
	int32		m_iMaxCount;			// 最大在线数
	int32		m_iGroup;				// 所在组
	int32		m_iNetType;				// 服务器的网络类型
	
	uint32		m_iSortIndex;			// 排序的序号
	int32		m_iSortScore;			// 排序得分

};

// 通讯库基类， 描述了对外部模块快提供的功能接口
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

	// 向服务器请求数据， 返回<=0 表示出错
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
	// 服务的编号，当服务器连接成功或失败时， 可以准确知道是哪台服务器引起的
	int32			m_iCommunicationId;

	// 该通讯对应的服务器信息(从认证服务器取得)
	CString			m_StrServerName;		// 名字
	CString			m_StrServerAddr;		// Ip 地址
	CString			m_StrServerSummary;		// 服务器描述

	int32			m_iServerID;			// ID
	uint32			m_uiServerPort;			// 端口
	uint32			m_uiServerHttpPort;		// HTTP端口

	int32			m_iOnLineCount;			// 在线数
	int32			m_iMaxCount;			// 最大在线数
	int32			m_iGroup;				// 所在组
	int32			m_iNetType;				// 服务器的网络类型
	
	bool32			m_bSomethingWrong;

	// 该通讯的代理信息
	CProxyInfo		m_ProxyInfo;			// 代理信息

	// 
	bool32			m_bConnected;			// 是否连接成功				
	E_AuthState		m_eAuthState;			// 认证状态

	// 通讯对象
	CSocketClient	*m_pSocketClient;

public:
	uint32			m_uiPingValue;			// PING 值
	uint32			m_iSortIndex;			// 排序的序号
	int32			m_iSortScore;			// 排序得分

private:
	CArray<CDataService*, CDataService*> m_DataServiceList;	// 记录下那些DataService关心该数据
	LockSingle		m_LockServiceList;

private:
	CCommManager	&m_CommManager;

private:
	bool32			m_bLongTimeNoRecvData;
};

#endif



