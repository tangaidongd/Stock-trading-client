#ifndef _DATA_ENGINE_STRUCT_H_
#define _DATA_ENGINE_STRUCT_H_

#include "typedef.h"
#include "sharestruct.h"
#include "SocketClient.h"
#include "SharePubStruct.h"

//#define  MAX_NAME_LEN	64

enum E_ServerTypeByEngine
{
	enEngineSTNone		= 0,
	enEngineSTQuote		= 1, 	// 行情服务器
	enEngineSTTrade		= 2, 	// 交易服务器
	enEngineSTNews		= 4,	// 消息服务器
	enEngineSTWarn		= 8,	// 预警
	enEngineSTInfo		= 32,   // 资讯服务器
};

// 数据源
enum E_DataSource
{
	EDSAuto = 0,							// 默认方式， 对于有可能存在脱机数据的请求， 优先查找脱机数据
	EDSVipData,								// 仅从临时数据中取
	EDSCommunication,						// 仅从网络取数据
	EDSOfflineData,							// 仅从脱机文件中取 (EDSVipData EDSOfflineData 要合并, 历史原因, 比较混乱)	

	EDSCounts,
};


// 认证状态
enum E_AuthState
{
	EASAuthSuccessed = 0,		// 成功
	EASAuthing,					// 正在认证
	EASAuthFailed,				// 

	EASAuthCount
};

// 连接状态
enum E_Connect_Status
{
	EG_CNT_STATUS_CONNECT  = 1,
	EG_CNT_STATUS_DIS_CONNECT,
	EG_CNT_STATUS_COUNT
};


enum E_COMMSTAGE
{
	ECSTAGE_LoginInit = 0,		// 处在登录初始化场景中
	ECSTAGE_QuoteSupplier,		// 行情提供中
	ECSTAGE_COUNT
};

//
//// 代理服务器信息
//class  CProxyInfo
//{
//public:
//	// 代理类型
//	enum E_ProxyType
//	{
//		EPTNone = 0,	// 直连
//		EPTHttp,		// http代理
//		EPTSock4,		// SOCK4代理
//		EPTSock5,		// SOCK5代理
//		EPTCount
//	};
//public:
//	CProxyInfo() { m_eProxyType = EPTNone; }
//	~CProxyInfo() { }
//public:
//	bool32 IsUseProxy()
//	{
//		if (m_eProxyType <= EPTNone || m_eProxyType >= EPTCount)
//			return false;
//		return true;
//	}
//public:
//	E_ProxyType		m_eProxyType;
//	CString			m_StrProxyAddr;
//	int32			m_iProxyPort;
//	CString			m_StrProxyUser;
//	CString			m_StrProxyPassword;
//};



// 每笔业务请求记录
class  AFX_API_EXPORT CMmiReqNode
{
public:
	CMmiReqNode();
	~CMmiReqNode();
public:
	// 记录下原始请求
	CMmiCommBase	*m_pMmiCommReq;			// 保留原始的数据请求
	E_DataSource	m_eDataSource;			// 保留请求数据源
	int32			m_iMmiReqId;			// 请求应答号
	int32			m_iCommSerialNo;		// 该请求流水号， 仅对EDSCommunication有效
	int32			m_iCommunicationId;		// 通讯号
public:
	uint32			m_uiTimeRequest;		// 发出请求的时间， 用于超时处理， 对于超出一定时长没有应答的请求， 我们将该不再等待其应答
};

// 每笔业务应答记录
class  AFX_API_EXPORT CMmiRespNode
{
public:
	CMmiRespNode();
	~CMmiRespNode();
public:
	// 记录下应答数据
	CMmiCommBase	*m_pMmiCommResp;
	int32			m_iMmiRespId;			// 应答号
public:
	int32			m_iCommunicationId;		// 服务器id
	uint32			m_uiTimeResponse;		// 该服务器时间
};





// 服务器状态结构体
class  CServerState
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

	int32		m_iCommunicationID;		// m_iCommunicationID
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
class  CAbsCommunicationShow
{
public:
	virtual void	GetServerSummary(OUT CString &StrAddr, OUT uint32 &uiPort, OUT CProxyInfo &ProxyInfo) = 0;
	virtual void	GetServerState(OUT CServerState &ServerState) = 0;
};

//
//// 服务器信息
//typedef struct _ServerInfo
//{
//	int				iID;						// 服务器ID
//	E_ServerTypeByEngine	enType;				// 服务器类型
//	wchar_t			wszName[MAX_NAME_LEN];			// 名字
//	wchar_t			wszAddr[32];				// IP
//	int				iPort;						// 端口
//	int				iHttpPort;					// HTTP端口
//	int				iOnLineCount;				// 在线数
//	int				iMaxCount;					// 最大在线数
//	int				iSerNetID;					// 服务器的网络类型ID
//	wchar_t			wszSerKey[MAX_NAME_LEN];
//	int				iGroup;						// 所在组
//
//	_ServerInfo()
//	{
//		iID					= 0;
//		enType				= enEngineSTNone;
//		wszName[0]			= 0;
//		wszAddr[0]			= 0;
//		wszSerKey[0]		= 0;
//		iPort				= 0;
//		iHttpPort			= 0;
//		iOnLineCount		= 0;
//		iMaxCount			= 0;
//		iSerNetID			= 0;
//		iGroup				= 1;
//	};
//
//	_ServerInfo(const _ServerInfo& serverinfo)
//	{
//		iID			= serverinfo.iID;
//		enType		= serverinfo.enType;
//		memcpy(wszName, serverinfo.wszName, sizeof(wchar_t)*MAX_NAME_LEN);
//		memcpy(wszAddr, serverinfo.wszAddr, sizeof(wchar_t)*32);
//		memcpy(wszSerKey, serverinfo.wszSerKey, sizeof(wchar_t)*MAX_NAME_LEN);
//		iPort		= serverinfo.iPort;
//		iHttpPort	= serverinfo.iHttpPort;
//		iOnLineCount = serverinfo.iOnLineCount;
//		iMaxCount	= serverinfo.iMaxCount;
//		iSerNetID	= serverinfo.iSerNetID;
//		iGroup		= serverinfo.iGroup;	
//	}
//
//	_ServerInfo& operator=(const _ServerInfo& serverinfo)//重载运算符  
//	{  
//		if ( this == &serverinfo )
//		{
//			return *this;
//		}
//
//		iID			= serverinfo.iID;
//		enType		= serverinfo.enType;
//		memcpy(wszName, serverinfo.wszName, sizeof(wchar_t)*MAX_NAME_LEN);
//		memcpy(wszAddr, serverinfo.wszAddr, sizeof(wchar_t)*32);
//		memcpy(wszSerKey, serverinfo.wszSerKey, sizeof(wchar_t)*MAX_NAME_LEN);
//		iPort		= serverinfo.iPort;
//		iHttpPort	= serverinfo.iHttpPort;
//		iOnLineCount = serverinfo.iOnLineCount;
//		iMaxCount	= serverinfo.iMaxCount;
//		iSerNetID	= serverinfo.iSerNetID;
//		iGroup		= serverinfo.iGroup;
//
//		return *this;
//	} 
//
//}T_ServerInfo;


#endif