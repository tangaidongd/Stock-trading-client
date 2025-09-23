#ifndef _DATA_MANAGER_H_
#define _DATA_MANAGER_H_

#include "typedef.h"
#include "sharestruct.h"
#include "synch.h"
//#include "CommonInterface.h"
#include "ShareFun.h"
#include "commmanager.h"
//#include "AbsOfflineDataManager.h"

#include "XTimer.h"
#include "DataCenterExport.h"

class CAbsCommManager;
class CCommManager;
class CAbsOfflineDataManager;
////////////////////////////////////////////////////////////////////////
// 数据源
enum E_DataSource
{
	EDSAuto = 0,							// 默认方式， 对于有可能存在脱机数据的请求， 优先查找脱机数据
	EDSVipData,								// 仅从临时数据中取
	EDSCommunication,						// 仅从网络取数据
	EDSOfflineData,							// 仅从脱机文件中取 (EDSVipData EDSOfflineData 要合并, 历史原因, 比较混乱)	

	EDSCounts,
};

// 每笔业务请求记录
class DATACENTER_DLL_EXPORT CMmiReqWaitingNode
{
public:
	CMmiReqWaitingNode();
	~CMmiReqWaitingNode();

public:
	// 记录下原始请求
	CMmiCommBase	*m_pMmiCommReq;			// 保留原始的数据请求
	E_DataSource	m_eDataSource;			// 保留请求数据源
	int32			m_iCommunicationId;		// 通讯服务器id

public:
	uint32			m_uiTimeRequest;		// 发出请求的时间,  每个请求都必须经过一定的时间才能处理
};

// 每笔业务请求记录
class DATACENTER_DLL_EXPORT CMmiReqNode
{
public:
	CMmiReqNode();
	~CMmiReqNode();

public:
	// 记录下原始请求
	CMmiCommBase	*m_pMmiCommReq;			// 保留原始的数据请求
	E_DataSource	m_eDataSource;			// 保留请求数据源

	// 
	int32			m_iMmiReqId;			// 请求应答号
	int32			m_iCommSerialNo;		// 该请求流水号， 仅对EDSCommunication有效
	int32			m_iCommunicationId;		// 通讯号
	
public:
	uint32			m_uiTimeRequest;		// 发出请求的时间， 用于超时处理， 对于超出一定时长没有应答的请求， 我们将该不再等待其应答
};

// 每笔业务应答记录
class DATACENTER_DLL_EXPORT CMmiRespNode
{
public:
	CMmiRespNode();
	~CMmiRespNode();

public:
	// 记录下应答数据
	CMmiCommBase	*m_pMmiCommResp;

	//
	int32			m_iMmiRespId;			// 应答号

public:
	int32			m_iCommunicationId;		// 服务器id
	uint32			m_uiTimeResponse;		// 该服务器时间
};


// 写脱机K 线的参数结构
typedef struct T_OffLineKLineParam
{
	friend class CDataManager;

public:
	T_OffLineKLineParam()
	{
		m_iMarketId		= -1;
		m_StrMerchCode	= L"";
		m_eKLineTypeBase= EKTBDay;
		m_aKLines.RemoveAll();
	}

	T_OffLineKLineParam(int32 iMarketId, const CString& StrMerchCode, E_KLineTypeBase eKLineTypeBase, const CArray<CKLine, CKLine>& aKLines)
	{
		m_iMarketId		= iMarketId;
		m_StrMerchCode	= StrMerchCode;
		m_eKLineTypeBase= eKLineTypeBase;
		
		m_aKLines.SetSize(aKLines.GetSize());
		
		if ( aKLines.GetSize() > 0 )
		{
			memcpyex((CKLine*)m_aKLines.GetData(), (const CKLine*)aKLines.GetData(), aKLines.GetSize() * sizeof(CKLine));
		}		
	}

	T_OffLineKLineParam& T_OffLineKLineParam::operator = (const T_OffLineKLineParam& stOffLineKLineParam)
	{
		if ( &stOffLineKLineParam == this )
		{
			return *this;
		}

		m_iMarketId		= stOffLineKLineParam.m_iMarketId;
		m_StrMerchCode	= stOffLineKLineParam.m_StrMerchCode;
		m_eKLineTypeBase= stOffLineKLineParam.m_eKLineTypeBase;
		
		m_aKLines.SetSize(stOffLineKLineParam.m_aKLines.GetSize());
		
		if ( stOffLineKLineParam.m_aKLines.GetSize() > 0 )
		{
			memcpyex((CKLine*)m_aKLines.GetData(), (const CKLine*)stOffLineKLineParam.m_aKLines.GetData(), stOffLineKLineParam.m_aKLines.GetSize() * sizeof(CKLine));
		}

		return *this;
	}

private:
	int32			m_iMarketId;
	CString			m_StrMerchCode;
	E_KLineTypeBase m_eKLineTypeBase;
	CArray<CKLine, CKLine> m_aKLines;

}T_OffLineKLineParam;


// 写脱机 Tick 的参数结构
typedef struct T_OffLineTickParam
{
	friend class CDataManager;

public:
	T_OffLineTickParam()
	{
		m_iMarketId		= -1;
		m_StrMerchCode	= L"";
		m_aTicks.RemoveAll();
	}
	
	T_OffLineTickParam(int32 iMarketId, const CString& StrMerchCode, const CArray<CTick, CTick>& aTicks)
	{
		m_iMarketId		= iMarketId;
		m_StrMerchCode	= StrMerchCode;
		
		m_aTicks.SetSize(aTicks.GetSize());
		
		if ( aTicks.GetSize() > 0 )
		{
			memcpyex((CTick*)m_aTicks.GetData(), (const CTick*)aTicks.GetData(), aTicks.GetSize() * sizeof(CTick));
		}		
	}

	T_OffLineTickParam& T_OffLineTickParam::operator= (const T_OffLineTickParam& stOffLineTickParam)
	{
		if ( this == &stOffLineTickParam )
		{
			return *this;
		}

		m_iMarketId		= stOffLineTickParam.m_iMarketId;
		m_StrMerchCode	= stOffLineTickParam.m_StrMerchCode;
		m_aTicks.Copy(stOffLineTickParam.m_aTicks);

		m_aTicks.SetSize(stOffLineTickParam.m_aTicks.GetSize());
		
		if ( stOffLineTickParam.m_aTicks.GetSize() > 0 )
		{
			memcpyex((CTick*)m_aTicks.GetData(), (const CTick*)stOffLineTickParam.m_aTicks.GetData(), stOffLineTickParam.m_aTicks.GetSize() * sizeof(CTick));
		}

		return *this;
	}
private:
	int32		m_iMarketId;
	CString		m_StrMerchCode;	
	CArray<CTick, CTick> m_aTicks;

}T_OffLineTickParam;

// 脱机行情数据结构
typedef struct T_OffLineRealtimePriceParam
{
	friend class CDataManager;

public:
	T_OffLineRealtimePriceParam()
	{
	
	}

	~T_OffLineRealtimePriceParam()
	{		
	}

	//
	T_OffLineRealtimePriceParam(const CRealtimePrice& RealtimePrice)
	{
		m_RealtimePrice = RealtimePrice;
	}

	//
	T_OffLineRealtimePriceParam& T_OffLineRealtimePriceParam::operator=(T_OffLineRealtimePriceParam& stSrc)
	{
		if ( this == &stSrc )
		{
			return *this;
		}

		//
		m_RealtimePrice = stSrc.m_RealtimePrice;
		return *this;
	}

private:
	CRealtimePrice m_RealtimePrice;

}T_OffLineRealtimePriceParam;
////////////////////////////////////////////////////////////////////////////////////
// 数据管理通知
class DATACENTER_DLL_EXPORT CDataManagerNotify
{
public:
	virtual void	OnDataServiceConnected(int32 iServiceId) = 0;
	virtual void	OnDataServiceDisconnected(int32 iServiceId) = 0;
	virtual void	OnDataRequestTimeOut(int32 iMmiReqId) = 0;
	virtual void	OnDataCommResponse() = 0;
	virtual void	OnDataServerConnected(int32 iCommunicationId) = 0;
	virtual void	OnDataServerDisconnected(int32 iCommunicationId) = 0;
	virtual void	OnDataSyncServerTime(CGmtTime &TimeServerNow) = 0;
	virtual void	OnDataServerLongTimeNoRecvData(int32 iCommunicationId, E_ReConnectType eType) = 0;
};


// 数据管理基类， 描述了对外提供的功能接口
class DATACENTER_DLL_EXPORT CAbsDataManager
{
public:
	// 请求数据， 该数据可能在脱机数据中， 也可能存在于服务器
	// 返会处理结果， <0 表示请求失败 =0 丢入队列中 >0 已经丢入发送队列
	virtual int32	RequestData(CMmiCommBase *pMmiCommReq, OUT CArray<CMmiReqNode*, CMmiReqNode*> &aMmiReqNodes, E_DataSource eDataSource = EDSAuto, int32 iCommunicationId = -1) = 0;
	
	//
	virtual int32	GetRespCount() = 0;
	virtual CMmiRespNode*	PeekAndRemoveFirstResp() = 0;

	//
	virtual void	AddDataManagerNotify(CDataManagerNotify *pDataManagerNotify) = 0;
	virtual void	RemoveDataManagerNotify(CDataManagerNotify *pDataManagerNotify) = 0;

	//
	virtual void	RemoveHistoryReqOfSpecifyServer(int32 iCommunicationId) = 0;

	//
	virtual void	ForceRequestType(E_DataSource eDataSource) = 0;
};

//
class DATACENTER_DLL_EXPORT CDataManager : public CCommManagerNotify, public CAbsDataManager, public CXTimerAgent
{
public:
	CDataManager(CAbsCommManager *pCommManager, CAbsOfflineDataManager &OfflineDataManager);
	~CDataManager();

public:
	void GetLocalTime(CString &strTime);
	bool32			Construct();
	
	// from CAbsDataManager
public:
	// 请求数据， 该数据可能在脱机数据中， 也可能存在于服务器
	// 返会处理结果， <0 表示请求失败 =0 丢入队列中 >0 已经丢入发送队列
	virtual int32	RequestData(CMmiCommBase *pMmiCommReq, OUT CArray<CMmiReqNode*, CMmiReqNode*> &aMmiReqNodes, E_DataSource eDataSource = EDSAuto, int32 iCommunicationId = -1);
	
	virtual int32	GetRespCount();
	virtual CMmiRespNode*	PeekAndRemoveFirstResp();

	//
	void			AddDataManagerNotify(CDataManagerNotify *pDataManagerNotify);
	void			RemoveDataManagerNotify(CDataManagerNotify *pDataManagerNotify);

	// 查找已发送队列, 删除对应服务器的已发请求
	void			RemoveHistoryReqOfSpecifyServer(int32 iCommunicationId);

	//
	void			ForceRequestType(E_DataSource eDataSource);

	// from CCommManagerNotify
public:
	virtual void	OnCommDataServiceConnected(int32 iServiceId);
	virtual void	OnCommDataServiceDisconnected(int32 iServiceId);
	virtual void	OnCommResponse(int32 iCommSerialNo, int32 iCommunicationId, IN CMmiCommBase *pMmiCommResp);
	virtual void	OnCommServerConnected(int32 iCommunicationId);
	virtual void	OnCommServerDisconnected(int32 iCommunicationId);
	virtual void	OnCommServerLongTimeNoRecvData(int32 iCommunicationId, E_ReConnectType eType);	

	// from CXTimerAgent
public:
	virtual	void	OnXTimer(int32 iTimerId);

public:
	void			SetCommManager(CCommManager *pCommManager) { m_pCommManager = (CAbsCommManager *)pCommManager; }

	int32			GetMmiReqId()	{ return m_iLastMmiReqId++; }


private:
	// 定时清理长时间没有回应的请求包
	void			OnTimerClearMmiReqList();
	
	// 定时处理脱机数据请求包, 实现异步通知的目的
	void			OnTimerProcessOfflineReq();

	// 统计每个communication 有多少个超时的请求
	void			DealTimeOutInfomations(CArray<CPoint, CPoint>& aTimeOutReqPairs, CMmiReqNode* pReqNode);

	// 定时处理待发队列
	void			OnTimerProcessMmiReqWaitingList();	// 尽可能的优化
	
	// 挨个通知事件发生
	void			NotifyDataServiceConnected(int32 iServiceId);
	void			NotifyDataServiceDisconnected(int32 iServiceId);
	void			NotifyDataRequestTimeOut(int32 iMmiReqId);
	void			NotifyDataCommResponse();
	void			NotifyServerConnected(int32 iCommunicationId);
	void			NotifyServerDisconnected(int32 iCommunicationId);
	void			NotifyDataSyncServerTimer(CGmtTime &TimeServer);
	void			NotifyServerLongTimeNoRecvData(int32 iCommunicationId, E_ReConnectType eType);		

	// 基本请求包
	// 请求数据， 该数据可能在脱机数据中， 也可能存在于服务器
	// 返会业务通讯号， <0 表示请求失败
	void			RequestDataImmediately(CMmiCommBase *pMmiCommReq, OUT CArray<CMmiReqNode*, CMmiReqNode*> &aMmiReqNodes, E_DataSource eDataSource = EDSAuto, int32 iCommunicationId = -1);

	void			PushReqIntoWaitingQueue(CMmiCommBase *pMmiCommReq, E_DataSource eDataSource, int32 iCommunicationId);
	void			AppendWaitingReq(CMmiCommBase *pMmiCommReq, E_DataSource eDataSource, int32 iCommunicationId);
	bool32			RequestDataOnlyFromCommunication(CMmiCommBase *pMmiCommReq, OUT CMmiReqNode*& pMmiReqNodeOut, int32 iCommunicationId = -1);
	bool32			RequestDataOnlyFromVipData(CMmiCommBase *pMmiCommReq, OUT CMmiReqNode*& pMmiReqNodeOut);
	bool32			RequestDataOnlyFromOfflineData(CMmiCommBase *pMmiCommReq, OUT CMmiReqNode*& pMmiReqNodeOut);
	
	// 对几个特殊的返回数据做后续加工(需要对照请求包才能补充完整应答包)
	void			RepairResponseData(CMmiCommBase *pMmiCommReq, CMmiCommBase *pMmiCommResp);
	
	// 同步服务器时间
	void			PreTranslateSycnTime(uint32 uiTimeRequest, uint32 uiTimeResponse, uint32 uiTimeServer);

	// 查找已接受队列， 如果存在该服务器推送的实时数据， 则删除， 避免因ServerDisconnect消息与CommResponse消息前后顺序混乱造成上层对实时数据处理的逻辑混乱
	void			RemoveRealtimeDataRespOfSpecifyServer(int32 iCommunicationId);

	// 待发包队列操作
private:
	void			RemoveAllWaitingReq();

	// 请求包队列操作
private:
	void			RemoveAllReq();

	// 应答队列操作
private:
	void			AddResp(CMmiRespNode *pMmiRespNode, bool32 bOfflineData = false);
	void			RemoveAllResp();

private:
	// 启动处理脱机数据的线程
	bool32			BeginThreadProcessOffLineData();
	
	// 脱机 K 线
	static void	    ThreadProcessOffLineKLine(LPVOID lParam);			
	void			ThreadProcessOffLineKLine();			
	
	void			PushOffLineKLineList(T_OffLineKLineParam& stOffLineKLineParam);
	
	// 脱机 Tick
	static void		ThreadProcessOffLineTick(LPVOID lParam);
	void			ThreadProcessOffLineTick();

	void			PushOffLineTickList(T_OffLineTickParam& stOffLineTickParam);

	// 脱机 RealtimePrice
	static void		ThreadProcessOffLineRealtimePrice(LPVOID lParam);
	void			ThreadProcessOffLineRealtimePrice();
	
	void			PushOffLineRealtimePriceList(T_OffLineRealtimePriceParam& stOffLineRealtimePriceParam);

private:
	bool32			Lock(HANDLE hMutex);
	void			UnLock(HANDLE hMutex);

	
//////////////////////////////////////////////////////////
// 变量
private:
	uint32			m_iLastMmiReqId;

private:
	CArray<CMmiReqWaitingNode*, CMmiReqWaitingNode*>	m_aMmiReqWaitingListPtr;	// 待发队列， 这里仅保存历史请求做这种处理
	CArray<CMmiReqNode*, CMmiReqNode*>					m_MmiReqListPtr;			// 已发请求队列
	//lint -sem(m_MmiRespListPtr::Add, custodial(1))
	CArray<CMmiRespNode*, CMmiRespNode*>				m_MmiRespListPtr;			// 应答队列

private:
	CAbsCommManager	*m_pCommManager;				// CommManager操作对象
	CAbsOfflineDataManager	&m_OfflineDataManager;	// 脱机数据操作对象

private:
	CArray<CDataManagerNotify*, CDataManagerNotify*> m_pDataManagerNotifyPtrList;

private:
	E_DataSource	m_eForceDataSource;				// 强制用某种方式请求数据

private:
	LockSingle		m_LockReqWaitingList;
	LockSingle		m_LockReqList;
	LockSingle		m_LockAnsList;

	HANDLE			m_hMutexReqWaitingList;
	HANDLE			m_hMutexReqList;
	HANDLE			m_hMutexRespList;

private:
	LockSingle		m_LockOffLineKLine;
	LockSingle		m_LockOffLineTick;
	LockSingle		m_LockOffLineRealtimePrice;

	HANDLE			m_hThreadOffLineKLine;
	HANDLE			m_hThreadOffLineTick;
	HANDLE			m_hThreadOffLineRealtimePrice;

	bool32			m_bThreadExit;
	// 等待ThreadProcessOffLineTick线程退出
	HANDLE			m_hEventThreadOffLineTick;

	// 等待ThreadProcessOffLineKLine线程退出
	HANDLE			m_hEventThreadOffLineKLine;

	// 等待ThreadProcessOffLineRealtimePrice线程退出
	HANDLE m_hThreadProcessOffLineRealtimePrice;

	CArray<T_OffLineKLineParam, T_OffLineKLineParam&>	m_aOffLineKLines;
	CArray<T_OffLineTickParam, T_OffLineTickParam&>		m_aOffLineTicks;
	CArray<T_OffLineRealtimePriceParam, T_OffLineRealtimePriceParam&> m_aOffLineRealtimePrice;

private:
	bool32			m_bHasSyncServerTime;
	vector<CString> m_vecLogInfo;

private:
	// 1022 #for trace
	HWND			m_hwndTrace;
};


#endif


