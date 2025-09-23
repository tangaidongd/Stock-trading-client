#ifndef _FOREX_EXPORT_H_
#define _FOREX_EXPORT_H_

#include "ForexStruct.h"
#include "synch.h"

//
#ifndef _FOREX_DLL
#define FOREXDLL _declspec(dllimport)
#else
#define FOREXDLL _declspec(dllexport)
#endif

class CForexMerch;
class CForexDataCenter;
class COrderInfo;

// 网络通知
class FOREXDLL CAbsForexNotify
{
public:
	// 网络相关

	// 错误消息
	virtual void	OnErrorMsg(E_ErrorCode eTypeErr) = 0;

	// 心跳包
	virtual void	OnHeart(IN const CString& StrTime) = 0;

	// 登录成功
	virtual void	OnLoginSuccess() = 0;

	// 重定向服务器
	virtual void	OnReLogServer(IN const CServer& Server) = 0;
	
	// 收到帐户信息
	virtual void	OnAccountInfo(IN const CAccountInfo& AccountInfo) = 0;
	
	// 收到货币信息
	virtual void	OnCurrencyInfo(IN const CArray<CCurrency, CCurrency&>& aCurrency) = 0;
	
	// 收到基本商品信息
	virtual void	OnMerchInfo(IN const CArray<CForexMerch, CForexMerch&>& aMerchs) = 0;
	
	// 收到成交信息
	virtual void	OnBargainInfo(IN const CArray<CBargain, CBargain&>& aBargains) = 0;
	
	// 收到委托单信息
	virtual void	OnOrderInfo(IN const CArray<COrderInfo, COrderInfo&>& aOrderInfos) = 0;
	
	// 收到已结算平仓单
	virtual void	OnHistoryOrderInfo(IN const CArray<CHistoryOrderInfo, CHistoryOrderInfo&>& aHistoryOrderInfos) = 0;
	
	// 收到组信息
	virtual void	OnGroupInfo(IN const CGroup& Group) = 0;
	
	// 收到基本信息
	virtual void	OnBaseInfo(IN const CBaseInfo& BaseInfo) = 0;
	
	// 收到公告信息
	virtual void	OnBroadCast(IN const CBroadCast& BroadCast) = 0;	

	// 行情推送
	virtual void	OnPushRealtimePrice(const CForexMerch& Merch) = 0;

	// 与服务器的网络连接出错了
	virtual void	OnNetError(int iError) = 0;

	/* ------------------------------ 交易相关 ------------------------------
	/*
	/* 收到这个通知的时候, 数据中心内的所有单据信息都是已经更新过的
	/* 类似平仓或下单成功, 可以使用接口返回的数据结构中手续费和利息等字段
	/*
	/* ---------------------------------------------------------------------*/

	// 下即时单成功
	virtual void	OnCommitImmiOK(const CBargain& Bargain)	= 0;

	// 下即时单失败
	virtual void	OnCommitImmiFail(const COrderInfo& OrderInfo) = 0;

	// 平仓即时单成功
	virtual void	OnCommitCloseOK(const CBargain& Bargain) = 0;

	// 平仓即时单失败
	virtual void	OnCommitCloseFail(const COrderInfo& OrderInfo) = 0;
	
	// 下委托单成功
	virtual void	OnCommitEntryOK(const COrderInfo& OrderInfo) = 0;

	// 下委托单失败
	virtual void	OnCommitEntryFail(const COrderInfo& OrderInfo) = 0;

	// 修改委托单成功
	virtual void	OnCommitModifyEntryOK(const COrderInfo& OrderInfo) = 0;
	
	// 修改委托单失败
	virtual void	OnCommitModifyEntryFail(const COrderInfo& OrderInfo) = 0;

	// 删除委托单成功
	virtual void	OnCommitDelEntryOK(const COrderInfo& OrderInfo) = 0;
	
	// 删除委托单失败
	virtual void	OnCommitDelEntryFail(const COrderInfo& OrderInfo) = 0;

	// 记录更新了
	virtual	void	OnForexLogInfo(const CArray<CForexLogInfo, const CForexLogInfo &> &logInfo) = 0;
};

// 外汇交易协议处理
class FOREXDLL CAbsForexManager
{
public:
	// 删除对象
	virtual void	ReleaseObj() = 0;

	// 设置通知
	virtual void	SetNotify(CAbsForexNotify* pNotify) = 0;

	// 取消通知
	virtual void	UnSetNotify() = 0;

	// 连接	
	virtual bool	ConnectServer(const CServer& Server) = 0;
	
	// 断开连接	
	virtual void	DisConnect() = 0;

	// 请求登录
	virtual	bool	ReqLogin(const CString& StrUserID, const CString& StrPwd) = 0;

	// 请求退出
	virtual	bool	ReqLogOut(const CString& StrUserID, const CString& StrPwd) = 0;

	// 请求下单
	virtual	bool	CommitBill(const COrderInfo &order) = 0;

	// 请求历史记录
	virtual bool	ReqHistoryOrderInfo(time_t tStart, time_t tEnd) = 0;

	// 获取数据中心
	virtual CForexDataCenter* GetDataCenter() = 0;

	// 获取服务器地址
	virtual void	GetServer(CServer& Server) = 0;
};

// 数据中心
class FOREXDLL CForexDataCenter
{
public:
	CForexDataCenter();
	virtual ~CForexDataCenter();
	
public:
	void		SetNotify(CAbsForexNotify* pNotify);
	
	void		UnSetNotify();
	
	void		OnRealtimePriceUpdate(const CForexRealtimePrice& ForexRealtimePrice);

	// 取当前的内部编号:
	int			GetInternalNo();

public:
	
	// 基本信息
	void		GetBaseInfo(CBaseInfo& BaseInfo);

	// 货币信息
	void		GetCurrencysInfo(CArray<CCurrency, CCurrency&>& aCurrency);

	// 取组信息
	void		GetGroupInfo(CGroup& Group);

	// 公告信息
	void		GetBroadCast(CBroadCast& BroadCast);

	// 帐号信息
	void		GetAccountInfo(CAccountInfo& AccountInfo);

	// 商品信息
	void		GetForexMerchs(CArray<CForexMerch, CForexMerch&>& aMerchs);

	// 成交信息
	void		GetBargainsInfo(CArray<CBargain, CBargain&>& aBargain);

	// 委托单信息
	void		GetOrdersInfo(CArray<COrderInfo, COrderInfo&>& aOrdersInfo);

	// 已结算平仓单信息
	void		GetHistoryOrderInfo(CArray<CHistoryOrderInfo, CHistoryOrderInfo&>& aHistoryOrderInfo);

	// 服务器返回的错误
	E_ErrorCode	GetErrorCode();

	//
public:
	//
	CAbsForexNotify* m_pNotify;

private:

	// 基本信息
	CBaseInfo		m_BaseInfo;
	LockSingle		m_LockBaseInfo;

	// 货币信息
	CArray<CCurrency, CCurrency&> m_aCurrency;
	LockSingle		m_LockCurrency;

	// 组信息
	CGroup			m_Group;
	LockSingle		m_LockGroup;

	// 公告信息
	CBroadCast		m_BroadCast;
	LockSingle		m_LockBroadCast;

	// 帐号信息
	CAccountInfo	m_AccountInfo;
	LockSingle		m_LockAccountInfo;

	// 商品信息
	CArray<CForexMerch, CForexMerch&>	m_aMerchsInfo;
	LockSingle		m_LockMerchsInfo;

	// 成交信息
	CArray<CBargain, CBargain&>	m_aBargains;
	LockSingle		m_LockBargains;

	// 委托单信息
	CArray<COrderInfo, COrderInfo&> m_aOrdersInfo; 
	LockSingle		m_LockOrdersInfo;

	// 已结算平仓单信息
	CArray<CHistoryOrderInfo, CHistoryOrderInfo&> m_aHistoryOrdersInfo; 
	LockSingle		m_LockHistoryOrdersInfo;

	// 服务器返回的错误
	E_ErrorCode		m_eErrServer;
	LockSingle		m_LockErrServer;

// 
friend class CPackagePara;
};

// 获取动态库操作指针
extern "C" FOREXDLL CAbsForexManager* GetAbsForexSocket();

#endif	// _FOREX_EXPORT_H_