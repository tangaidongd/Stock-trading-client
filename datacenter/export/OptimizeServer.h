// OptimizeServer.h: interface for the COptimizeServer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _OPTIMIZESERVER_H_
#define _OPTIMIZESERVER_H_
#include "DataCenterExport.h"

#include "communication.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class TiXmlDocument;
class TiXmlElement;
class CViewData;


// ping 的定时器  间隔 5 s
const int32	KiTimerIdPing				= 61215891;				
const int32	KiTimerPeriodPing			= 1500;	


typedef CArray<CServerState, CServerState&> arrServer;

// 保存服务器 ping 的信息结构
typedef struct T_ServerSortPing
{
	int32			m_iReqID;				// 请求 ID
	bool32			m_bCompeleted;			// 是否完成
	bool32			m_bTimeOut;				// 是否超时

	CServerState	m_stServer;
	DWORD			m_TimeReq;				// 发请求的时间
	DWORD			m_TimeResp;				// 请求回来的时间

	T_ServerSortPing()
	{
		m_iReqID		= -1;
		m_bCompeleted	= false;
		m_bTimeOut		= false;

		m_TimeReq		= -1;
		m_TimeResp		= -1;
	}

}T_ServerSortPing;

class DATACENTER_DLL_EXPORT COptimizeServerNotify
{
public: 
	virtual void  OnFinishSortServer(){}								// 排序完成的时机, 
	virtual void  OnFinishAllPing(){}									// ping 完成的时机. 	
	virtual void  OnFinishPingServer(CServerState stServer){}			// ping 完一台服务器
};

// 优选主站
class DATACENTER_DLL_EXPORT COptimizeServer
{
public:
	enum E_NetType
	{
		ENTDianxin = 1,
		ENTWangtong,
		ENTYidong,
		ENTLiantong,
		ENTOther,

		ENTCount,
	};

private:
	COptimizeServer();

public:
	void			Construct();
	virtual ~COptimizeServer();

public:
	// 单例模式
	static COptimizeServer* Instance();
	
	static void				DelInstance();

	// 设置所有者
	void			SetOwner(HWND hWnd) { if (NULL == hWnd)return; m_hOwnWnd = hWnd; }

	// 增加观察者
	void			AddNotify(COptimizeServerNotify* pNotify);
	
	// 删除观察者
	void			RemoveNotify(COptimizeServerNotify* pNotify);
	
	// 设置网络类型
	void			SetNetType(E_NetType eNetType);

	// 设置是否手动排序
	void			SetSortMode(bool32 bAuto);

	// 得到排序模式
	bool32			BeAutoSortMode();

	// 设置排序的参数
	void			SetSortParams(const T_ServerInfo* pServerFromAuth, int32 iServerCounts);

	// 剔除连接不上的行情服务器
	void			DelNonConnectServer(const T_ServerInfo* pServerNonConnect, int32 iServerCounts);

	// 给行情服务器算分
	int32			CalcServerScore(const CServerState& stServer);			

	// 给行情服务器排序
	bool32			SortQuoteServers();								

	// 用ping 来排序服务器
	bool32			SortQuoteServersByPing();	

	// 对话框手动排序的结果
	void			SetManualSortResult(IN const arrServer& aServers);

	// 设置 ping 的标志 (是否手动优选的界面强制重ping)
	void			SetForcePingFlag(bool32 bForcePing);

	// 得到 ping 的标志
	bool32			GetForcePingFlag();

	// 发送 ping 请求
	bool32			RequestNetTest(IN const CMmiReqNetTest *pMmiReqNetTest, IN int32 iCommunicationID, OUT int32& iReqID);

	// 收到 ping 的回包
	void			OnDataRespNetTest(int32 iMmiReqId, IN const CMmiRespNetTest *pMmiRespNetTest);

	// 测速中有服务器断线了- -，证明该服务器的连接可能不稳定
	void			OnDataServerDisconnected(int32 iCommunicationId);
	
	// 手动界面的 重ping
	void			ForceRePingServer();

	// 得到ping 的最大值
	int32			GetMaxPingValue();
	
	// 得到排序后的服务器
	void			GetSortedServers(OUT arrServer& aServers);

	// 得到网络类型的字符串
	CString			GetNetTypeString(E_NetType eNetType, bool32 bDetail = false);
	
	// 返回比较结果
	bool32			BeLocalAndAuthServersIdentical();				

	// 比较两个服务器是否一样
	bool32			BeServerIdentical(const CServerState& ServerState1, const CServerState& ServerState2);

	// 是否完成排序
	bool32			BeSortCompleted();

	// 开启定时器
	void			StartMyTimer();

	// 关闭定时器
	void			StopMyTimer();

	// 定时器函数
	void			OnMyTimer(int32 iTimerId);

	// 清除错误服务器信息
	void			ClearErrServer();

	// 错误服务器信息
	bool32			SaveErrServer(CServerState& stServer);

	void			SetViewData(CViewData* pViewData);

private:
	
	// Ping 服务器
	void			PingAllServers();

	// Ping 完了
	void			OnPingCompleted();

	// 排序排好了
	void			OnSortCompleted();

	// 从文件中读取服务器信息
	bool32			GetServerInfoFromXml();							
	
	// 遍历XML 节点获取服务器信息
	bool32			GetServerInfo(const TiXmlElement* pElement, arrServer& aServers);

	// 比较本地的和认证返回的服务器信息
	bool32			JudgeLocalAndAuthServers();		

	// 用配置文件的信息排序服务器
	bool32			SortQuoteServersByXmlFile(IN arrServer& aServerIN, OUT arrServer& aServerOUT);	

	// 得到服务器的xml 字符串
	CString			GetCommunicationXmlString(const CServerState& stServer);
	
	// 将这次的信息保存到文件 
	bool32			SaveServerInfoToXml();	

	// 两种数据类型转换
	void			T_ServerInfo2ServerState(IN const T_ServerInfo& serIn, OUT CServerState& serOut);
	
	// 每次计算的时候, 当前在线数要取最新的, 用认证返回的信息
	void			SetLatestOnlineData(CServerState& stServer);			

	// 最终的排序算法, 先用ping 和type 算分, 再判断服务器空闲率等因素
	bool32			SortServers(IN arrServer& aServerIN, OUT arrServer& aServerOUT);

	// 验证服务器负载情况, 如果太满了, 往后面排, 验证这个服务器是不是错误的服务器
	bool32			FinalCheckServer(INOUT arrServer& aServers);

	// 是否错误的服务器
	bool32			BeErrServer(const CServerState& st);

	// 读取错误服务器信息
	bool32			GetErrServer();



private:
	static	COptimizeServer* m_pThis;									// 指向自己的指针
	TiXmlDocument*			 m_pXmlDoc;									// Xml 的文档类指针
	E_NetType				 m_eNetType;								// 网络类型
	
	bool32					 m_bAutoOptimize;							// 是否自动优选
	bool32					 m_bLocalAndAuthIdentical;					// 本地的服务器信息和认证的信息是否一致
	bool32					 m_bPingCompleted;							// 是否ping 完了
	bool32					 m_bSortCompleted;							// 是否排序完成
	bool32					 m_bForcePing;								// 是否是手动重新测速

	CViewData*				 m_pViewData;

	//
	CArray<COptimizeServerNotify*,COptimizeServerNotify*> m_apNotify;	// 被通知的模块

	arrServer m_aServersFromXmlManual;	// 文件中读取的服务器信息(手动)	
	arrServer m_aServersFromXmlAuto;	// 文件中读取的服务器信息(自动)

	arrServer m_aServersFromAuth;		// 认证返回的服务器信息	
	arrServer m_aServersAftSort;		// 排序后的服务器信息

	arrServer m_aErrServer;				// 优选完, 登录发生错误的服务器

	CArray<T_ServerSortPing, T_ServerSortPing&> m_aServerPingInfo;		// 服务器 ping 的情况	

	HWND					m_hOwnWnd;									// 窗口句柄, 用户定时器
};

#endif // _OPTIMIZESERVER_H_
