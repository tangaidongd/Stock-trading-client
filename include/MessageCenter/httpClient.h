// SocketClient.h: interface for the CHttpClient class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HTTP_CLIENT_H_
#define _HTTP_CLIENT_H_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxsock.h>
#include <vector>
//#include "CommonInterface.h"
#include "DataStream.h"
#define UM_MSGLOG 0x459
using std::vector;

namespace http_client
{
	// 与消息中心服务器的连接状态
	enum E_CONN_NOTIFY
	{
		en_connect_error,	// 连接出错
		en_connect_success, // 连接成功
	};

	typedef void (*CALL_HTTP_DATA)(std::string strHttpData);
	typedef void (*CALL_HTTP_CONN_NOTIFY)(E_CONN_NOTIFY eConnNotify);

// 代理服务器信息
class AFX_EXT_CLASS CProxyInfo
{
public:
	// 代理类型
	enum E_ProxyType
	{
		EPTNone = 0,	// 直连
		EPTHttp,		// http代理
		EPTSock4,		// SOCK4代理
		EPTSock5,		// SOCK5代理

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
// socket事件通知接口
class AFX_EXT_CLASS CSocketNotify
{
public:
	virtual void	OnSocketConnected()		= 0;	// 当服务器连接成功
	virtual void	OnSocketRecvOneOrMore() = 0;	// 当收到数据时， 一有数据即会通知
	virtual void	OnSocketError()			= 0;	// 当发生错误， 可能是连接过程中， 也可能是连接成功后
};


///////////////////////////////////////////////////////////////

// socketclient
// socket客户端通讯类， 不处理服务器相关的bind和listen之类的操作
// 支持代理和直连的方式
class AFX_EXT_CLASS CHttpClient  
{
public:
	// 网络状态
	enum E_SocketState 
	{
		ESSIdle = 0,			// 未连接
		ESSDnsLooking,			// 域名解析中
        ESSConnecting,			// 连接中
        ESSConnected			// 连接成功
	};

	enum E_SocketError
	{
		ESENone = 0,			// 正常

		// 打开socket出错 
		ESEOpenSocketFail,	
		ESESocketGeneral,

		// 网络报错
		ESESocketConnectError,	// 连接过程错误
		ESESocketSendError,		// 发送数据错误
		ESESocketRecvError,		// 接收数据错误
		ESESocketConnectClose,	// 连接被服务器关闭
		
		// 超时
		ESEConnectTimeOut,		// 连接超时
		ESESendDataTimeOut,		// 发送数据超时
		ESERecvDataTimeOut,		// 接收数据超时

		// 代理
		ESEProxyError,			

		// 流错误
		ESEStreamSendError,	// 写入inputstream错误
		ESEStreamRecvError		// 读取outputstream错误
	};


public:
	CHttpClient(/*CSocketNotify &SocketNotify*/);
	~CHttpClient();

public:
	bool32			Construct();

	static CHttpClient* GetHttpClient();
	
public:
	/*
	 * UnpackHttpPacket
	 * 解析服务器发送过来的http数据包
	 * @param strHttpData 保存解析后的http数据
	 */
	void UnpackHttpPacket(OUT std::string &strHttpData);

	/*
	 * PackHttpPacket
	 * 封装要向服务器发送的http数据包
	 * @param strHttpData 保存封装后的http数据包
	 */
	void PackHttpPacket(IN std::string strJson);

	/*
	 * SetTokenAndUserID
	 * 在动态库中保存UserID和token
	 * @param strToken token值
	 * @param strUserID UserID
	 */
	void SetTokenAndUserID(std::string strToken, std::string strUserID);

	/*
	 * QueryInfoContent
	 * 查询资讯内容
	 * @param strUserID 用户ID
	 * @param strInfoID 资讯ID
	 */
	void QueryInfoContent(std::string strUserID, std::string strInfoID);

	/*
	 * SetCallHttpData
	 * 设置客户端接收http数据的回调函数
	 * @param fnCallHttpData 要设置的回调函数
	 */
	void SetCallHttpData(CALL_HTTP_DATA fnCallHttpData);

	/*
	 * SetCallHttpConn
	 * 设置连接状态通知的回调函数
	 * @param fnCallHttpConn 要设置的回调函数
	 */
	void SetCallHttpConn(CALL_HTTP_CONN_NOTIFY fnCallHttpConn);
	
public:
	// 连接、收、发线程
	static void		CallbackThreadConnect(LPVOID lpParam);
	static void		CallbackThreadRW(LPVOID lpParam);
	static void		CallbackThreadMain(LPVOID lpParam);

	// 心跳线程
	static void		CallbackThreadHeartBeat(LPVOID lpParam);

	// 消息中心服务器重连线程
	static void		CallbackReConnectMsgCenter(LPVOID lpParam);

private:
	// 以下函数在线程回调中处理
	void			CallbackThreadConnect();
	void			CallbackThreadRW();
	void			CallbackThreadMain();

	void			CallbackThreadHeartBeat();
	void			CallbackReConnectMsgCenter();

	

	void			CallbackThreadConnectByHttpProxy();		// http代理方式连接
	void			CallbackThreadConnectBySock4Proxy();	// sock4代理
	void			CallbackThreadConnectBySock5Proxy();	// sock5代理
	void			CallbackThreadConnectDirect();			// 直连
	
	// 以下函数在线程回调中处理理， 完成单一的功能， 并且不能阻塞， 要能对退出标志做及时的响应
	bool32			ConnectServerWithExitFlag(CString StrServerAddr, int32 iServerPort, IN bool32 &bExitFlag, bool32 bAllowTimeOut = true);
	int32			SendDataWithExitFlag(const int8 *pcBuf, int32 iNeedSendLen, IN bool32 &bExitFlag, bool32 bAllowTimeOut = true, bool32 bSendOneOrMore = true);
	int32			RecvDataWithExitFlag(int32 iNeedRecvLen, OUT int8 *pcBuf, IN bool32 &bExitFlag, bool32 bAllowTimeOut = true, bool32 bRecvOneOrMore = true);	// 返回-1表示出错

public:
	void			SetProxy(const CProxyInfo &ProxyInfo) { m_ProxyInfo = ProxyInfo; }
	void			ConnectServer(const CString &StrServerAddr, int32 iServerPort);
	void			DisConnect();
	void			TryToDisConnect();

	// 数据流
public:
	CDataStream&	GetSendStream() { return m_DataStreamSend; }
	CDataStream&	GetRecvStream() { return m_DataStreamRecv; }
	
	uint32			GetTimeConnected() { return m_uiTimeConnected; }
	uint32			GetTimeLastSend() { return m_uiTimeLastSend; }
	uint32			GetTimeLastRecv() { return m_uiTimeLastRecv; }

public:
	// 状态
	void			SetSocketState(E_SocketState eSocketState);
	E_SocketState	GetSocketState() { return m_eSocketState; }

	void			SetSocketError(E_SocketError eSocketError);
	E_SocketError	GetSocketError() { return m_eSocketError; }

	void			SetReportString(const CString &StrReport) { m_StrReport = StrReport; }
	CString			GetReportString() { return m_StrReport; }

	
	
private:
	// 启动收、发线程， 在服务器和对应的数据流间传递数据
	void			StartConnectThread();
	void			StartRWThread();
	void			StartMainThread();

	void			StartHeartbeatThread();
	void			StartReconnectThread();

	void			StopConnectThread();
	void			StopRWThread();
	void			StopMainThread();
	void			StopHeartbeatThread();
	void			StopReconnectThread();

	void			TryToStopConnectThread();
	void			TryToStopRWThread();
	void			TryToStopMainThread();
	
	//
	bool32			OpenSocket();
	void			CloseSocket();

	void			PackHeartbeatPacket();

private:
	bool32			Lock(HANDLE hMutex);
	void			UnLock(HANDLE hMutex);
	
public:   
	// 连接、收、发线程句柄
	HANDLE			m_hThreadMain, m_hThreadConnect, m_hThreadRW;
	HANDLE			m_hThreadHeartbeat;
	HANDLE			m_hThreadReconnect;
	bool32			m_bExitConnectThreadFlag;	// 退出线程标志
	bool32			m_bExitRWThreadFlag;
	bool32			m_bExitMainThreadFlag;	
	bool32			m_bExitHeartbeatThreadFlag;
	bool32			m_bExitReconnectThrad;// 是否退出重连线程

	std::string		m_strUserID;	 // 保存UserID
	std::string		m_strToken;		 // 保存token值
	CALL_HTTP_DATA  m_fnCallHttpData;// 保存向客户端发送http数据的回调函数
	CALL_HTTP_CONN_NOTIFY  m_fnCallConnNotify;// 保存连接状态的回调函数（作用：将与服务器的连接结果通知客户端）


	
	// 收、发数据流
	CDataStream		m_DataStreamSend;
	CDataStream		m_DataStreamRecv;

	std::string		m_strHeartbeat;// 心跳包json串

	// socket操作句柄
	SOCKET			m_hSocket;

	// 当前连接地址
	CString			m_StrServerAddr;	// 形如：202.96.134.134 或 www.google.cn
	int32			m_iServerPort;		// 服务器端口， 默认http端口80， ftp端口21
	
	// 代理信息
	CProxyInfo		m_ProxyInfo;

	// 状态信息
	CString			m_StrReport;		// 描述及时的状态信息字符串
	E_SocketState	m_eSocketState;
	E_SocketError	m_eSocketError;

	// 事件接收对象
	//CSocketNotify	&m_SocketNotify;
	
	// 几个时间点
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
	HANDLE			m_hMutexHeartbeat;
	
private:
	// 1022 #for trace
	HWND			m_hwndTrace;
	vector<CString> m_vecMsgLog;
};

}// namespace http_client
#endif



