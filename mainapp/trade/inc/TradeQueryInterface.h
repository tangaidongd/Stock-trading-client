#ifndef _TRADEQUERYINTERFACE_H_
#define _TRADEQUERYINTERFACE_H_


#include "SocketClient.h"
#include "tradenginexport.h"

enum E_TradeLoginStatus
{
	ETLS_NotLogin = 0,		// 未登录
	ETLS_Logining,			// 正登录
	ETLS_Logined,			// 已登录
	ETLS_Locked,
};

enum E_TipType
{
	ETT_NONE = 0,
	ETT_SUC,		// 成功
	ETT_ERR,		// 错误
	ETT_TIP,		// 提示
	ETT_WAR,		// 警告
};

enum E_MsgType
{
	EMT_NONE = 0,
	EMT_CONNECTED,		// 连接成功
	EMT_DISCONNECTED,	// 连接断开
	EMT_QUERYCODE,		// 查验证码
	EMT_LOGINOK,		// 登录成功
	EMT_LOGOUTOK,		// 登出成功
	EMT_USERINFO,		// 客户信息
	EMT_MERCHINFO,		// 商品信息
	EMT_ENTRUST,		// 委托买卖
	EMT_CANCELENTRUST,	// 撤销委托单
	EMT_QUERYENTRUST,	// 查询委托单
	EMT_QUERYDEAL,		// 查询成交
	EMT_QUERYHOLD,		// 查询持仓
	EMT_QUERYHOLDSUMMARY,// 查持仓汇总
	EMT_QUOTE,			// 行情
	EMT_QUERYTRADERID,	// 查对方交易员ID
	EMT_SETSTOPLP,		// 设置止盈止损
	EMT_CANCELSTOPLP,	// 取消止盈止损
	EMT_MODIFY_PWD,		// 修改密码
	EMT_ERROR,			// 出错了
};

#define WM_ENTRUSTTYPE_CLOSE	WM_USER+1000	//平仓
#define WM_HOLDDETAIL_CLICK		WM_USER+1001	//选中单个持仓
#define WM_CLOSETRADE			WM_USER+1002	//关闭交易通知
#define WM_NETWORKSTATUS_CHANGE	WM_USER+1003	//网络状态改变通知
#define WM_HIDETRADEWND			WM_USER+1004	//隐藏交易窗口

#define WM_RESP_TRADE_NOTIFY	WM_USER+1005	//回包通知消息

enum E_TradeType
{
	ECT_Market = 0,		// 市价
	ECT_Limit,			// 指价
};

enum E_CloseBtnStatus
{
	ECBS_Enable = 0,
	ECBS_UnEnable,
};

#include <vector>
using std::vector;

 //
 typedef vector<T_RespQueryHold>	 QueryHoldDetailResultVector;  // 查询持仓
 typedef vector<T_RespQueryHoldTotal> QueryHoldSummaryResultVector;// 查持仓汇总
 typedef vector<T_RespQueryEntrust>  QueryEntrustResultVector;     //查询委托单
 typedef vector<T_RespQueryDeal>	 QueryDealResultVector;		 // 查询成交
 typedef vector<T_TradeMerchInfo>	 QueryCommInfoResultVector;    // 商品信息
 typedef vector<T_RespQuote>		 QueryQuotationResultVector;	 // 行情
 typedef vector<T_RespTraderID>		 QueryTraderIDResultVector;	 // 查对方交易ID

class iTradeBidNotify;
class iTradeBid
{
public:
	virtual	void	AddTradeBidNotify(iTradeBidNotify *pNotify) = 0;
	virtual void	RemoveTradeBidNotify(iTradeBidNotify *pNotify) = 0;
	
	// query方法
	// 请求客户信息
	virtual bool32 ReqUserInfo(const CClientReqUserInfo& stReq, CString &strErrMsg) = 0;
	// 请求商品信息
	virtual bool32 ReqMerchInfo(const CClientReqMerchInfo& stReq, CString &strErrMsg) = 0;
	// 查询对方交易ID
	virtual bool32 ReqQueryTraderID(const CClientQueryTraderID& stReq, CString &strErrMsg) = 0;
	// 请求委托买卖
	virtual bool32 ReqEntrust(const CClientReqEntrust& stReq, CString &strTipMsg) = 0;
	// 请求撤销委托单
	virtual bool32 ReqCancelEntrust(const CClientReqCancelEntrust& stReq, CString &strTipMsg) = 0;
	// 请求查询委托单
	virtual bool32 ReqQueryEntrust(const CClientReqQueryEntrust& stReq, CString &strErrMsg) = 0;
	// 请求查询可撤单
	virtual bool32 ReqQueryCancelEntrust(const CClientReqQueryCancelEntrust& stReq, CString &strErrMsg) = 0;
	// 请求查询成交
	virtual bool32 ReqQueryDeal(const CClientReqQueryDeal& stReq, CString &strErrMsg) = 0;
	// 请求查询持仓
	virtual bool32 ReqQueryHold(const CClientReqQueryHold& stReq, CString &strErrMsg) = 0;
	//请求查持仓汇总
	virtual bool32 ReqQueryHoldSummary(const CClientReqQueryHoldTotal& stReq, CString &strErrMsg) = 0;
	// 请求行情
	virtual bool32 ReqQute(const CClientReqQuote& stReq, CString &strErrMsg) = 0;
	// 设置止盈止损
	virtual bool32 ReqSetStopLP(const CClientReqSetStopLP& stReq, CString &strTipMsg) = 0;
	// 取消止盈止损
	virtual bool32 ReqCancelStopLP(const CClientReqCancelStopLP& stReq, CString &strTipMsg) = 0;
	// 请求修改密码
	virtual bool32 ReqModifyPwd(const CClientReqModifyPwd& stReq, CString &strTipMsg) = 0;

	// Get方法
	virtual QueryHoldDetailResultVector				&GetCacheHoldDetail() = 0;
	virtual const QueryHoldSummaryResultVector		&GetCacheHoldSummary() = 0;
	virtual QueryEntrustResultVector				&GetCacheEntrust() = 0;
	virtual const QueryDealResultVector				&GetCacheDeal() = 0 ;
	virtual const QueryCommInfoResultVector			&GetCacheCommInfo() = 0 ;
	virtual const QueryTraderIDResultVector			&GetCacheTraderID() = 0 ;
	virtual const CClientRespUserInfo			    &GetCacheUserInfo() = 0;
	virtual QueryQuotationResultVector				&GetCacheQuotation() = 0 ;
	virtual const CClientRespError					&GetCacheError() = 0;
	virtual const CClientRespLogIn					&GetCacheLogIn() = 0;

	virtual const CClientRespEntrust				&GetCacheReqEntrust() = 0;
	virtual const CClientRespCancelEntrust			&GetCacheReqCancelEntrust() = 0;
	virtual const CClientRespSetStopLP				&GetCacheReqSetStopLP() = 0;
	virtual const CClientRespCancelStopLP			&GetCacheReqCancelStopLP() = 0;
	virtual const CClientRespModifyPwd				&GetCacheReqModifyPwd() = 0;

	// 心跳
	virtual int	HeartBeating(CString &strTipMsg) = 0;

	virtual void	WndMaxMin() = 0;
	virtual void	WndClose(int iMode) = 0;
	virtual BOOL	GetWndStatus() = 0;

	//登录状态查询
	virtual E_TradeLoginStatus GetTradeLoginStatus() = 0;

	QueryHoldDetailResultVector	m_aCacheHoldDetail;
	QueryEntrustResultVector 	m_aCacheEntrust;
	QueryDealResultVector 		m_aCacheDeal;
	QueryCommInfoResultVector	m_aCacheCommInfo;
	QueryQuotationResultVector	m_aCacheQuotation;
	QueryTraderIDResultVector	m_aCacheTraderID;
	CClientRespError			m_stCacheError;	// 错误包
	CClientRespLogIn			m_stCacheLogIn;	// 登陆
	CClientRespUserInfo			m_stCacheUserInfo; // 客户信息
	CClientRespCode				m_stCacheCode; // 验证码

	CClientRespEntrust			m_stCacheReqEntrust; // 委托买卖回包
	CClientRespCancelEntrust	m_stCacheReqCancelEntrust; 
	CClientRespSetStopLP		m_stCacheReqSetStopLP; 
	CClientRespCancelStopLP		m_stCacheReqCancelStopLP; 
	CClientRespModifyPwd		m_stCacheReqModifyPwd; 
	bool						m_bReqQuick;	// 闪电下单
};

class iTradeBidNotify
{
public:
	
	// 登录连接状态变更 status = E_TradeLoginStatus
	virtual void	OnLoginStatusChanged(int iCurStatus, int iOldStatus) = 0;
	virtual bool32	OnInitQuery(CString &strTipMsg) = 0;
	virtual bool32	OnQueryUserInfo(CString &strTipMsg) = 0;
	virtual void	OnInitCommInfo() = 0;
	
	virtual void	OnQueryHoldDetailResponse() = 0;
	virtual void	OnQueryHoldSummaryResponse() = 0;
//	virtual void	OnQueryEntrustResponse() = 0;
	virtual void	OnQueryLimitEntrustResponse() = 0;
	virtual void	OnQueryDealResponse() = 0;
	virtual void	OnQueryCommInfoResponse() = 0;
	virtual void	OnQueryTraderIDResponse() = 0;
	virtual void	OnQueryUserInfoResponse() = 0;
	virtual void	OnQueryQuotationResponse() = 0;
	virtual void	OnDisConnectResponse() = 0;

	virtual void	OnReqEntrustResponse(bool32 bShowTip=true) = 0;
	virtual void	OnReqCancelEntrustResponse() = 0;
	virtual void	OnReqSetStopLPResponse() = 0;
	virtual void	OnReqCancelStopLPResponse() = 0;
	virtual void	OnReqModifyPwdResponse() = 0;
};

class CRespTradeNotify : public CTradeNotify
{
public:
	CRespTradeNotify(HWND m_hwnd)
	{
		m_notifyHwnd = m_hwnd;
	}

	~CRespTradeNotify();
	
	// 连接成功
	virtual void	OnConnected();
	
	// 连接断开
	virtual void	OnDisConnected();

	// 查验证码回包
	virtual void	OnRespQueryCode(CClientRespCode* pResp);
	
	// 登录成功
	virtual void	OnLogInOK(CClientRespLogIn* pResp);
	
	// 登出成功
	virtual void	OnLogOutOK(CClientRespLogOut* pResp);
	
	// 客户信息回包
	virtual void	OnRespUserInfo(CClientRespUserInfo* pResp);
	
	// 商品信息回包
	virtual void	OnRespMerchInfo(CClientRespMerchInfo* pResp);

	// 对方交易员ID回包
	virtual void	OnRespQueryTraderID(CClientRespTraderID* pResp);
	
	// 委托买卖回包
	virtual void	OnRespEntrust(CClientRespEntrust* pResp);
	
	// 撤销委托单回包
	virtual void	OnRespCancelEntrust(CClientRespCancelEntrust* pResp);
	
	// 查询查询可撤单回包
	virtual void	OnRespQueryCancelEntrust(CClientRespQueryCancelEntrust* pResp);

	// 查询委托单回包
	virtual void	OnRespQueryEntrust(CClientRespQueryEntrust* pResp);
	
	// 查询成交回包
	virtual void	OnRespQueryDeal(CClientRespQueryDeal* pResp);
	
	// 查询持仓回包
	virtual void	OnRespQueryHold(CClientRespQueryHold* pResp);

	// 查询持仓汇总回包
	virtual void	OnRespQueryHoldTotal(CClientRespQueryHoldTotal* pResp);
	
	// 行情回包
	virtual void	OnRespQuote(CClientRespQuote* pResp);
	
	// 设置止盈止损回包
	virtual void	OnRespSetStopLP(CClientRespSetStopLP* pResp);
	
	// 取消止盈止损回包
	virtual void	OnRespCancelStopLP(CClientRespCancelStopLP* pResp);

	// 修改密码回包
	virtual void	OnRespModifyPwd(CClientRespModifyPwd* pResp);
	
	// 出错了
	virtual void	OnError(CClientRespError* pResp);

	// Get方法
	const QueryHoldDetailResultVector		&GetCacheHoldDetail(){ return m_aCacheHoldDetail;} ;
	const QueryHoldSummaryResultVector		&GetCacheHoldSummary() { return m_aCacheHoldSummary; }
	const QueryEntrustResultVector			&GetCacheEntrust(){ return m_aCacheEntrust;}
	const QueryDealResultVector				&GetCacheDeal(){ return m_aCacheDeal;}
	const QueryCommInfoResultVector			&GetCacheCommInfo(){ return m_aCacheCommInfo;}
	const CClientRespUserInfo				&GetCacheUserInfo(){ return m_stCacheUserInfo;}
	const QueryQuotationResultVector		&GetCacheQuotation(){ return m_aCacheQuotation;}
	const QueryTraderIDResultVector			&GetCacheTraderID(){ return m_aCacheTraderID;}
	const CClientRespError					&GetCacheError(){ return m_stCacheError;}
	const CClientRespLogIn					&GetCacheLogIn(){ return m_stCacheLogIn;}
	const CClientRespCode					&GetCacheCode(){ return m_stCacheCode;}

	const CClientRespEntrust				&GetCacheReqEntrust(){ return m_stCacheReqEntrust;}
	const CClientRespCancelEntrust			&GetCacheReqCancelEntrust(){ return m_stCacheReqCancelEntrust;}
	const CClientRespSetStopLP				&GetCacheReqSetStopLP(){ return m_stCacheReqSetStopLP;}
	const CClientRespCancelStopLP			&GetCacheReqCancelStopLP(){ return m_stCacheReqCancelStopLP;}
	const CClientRespModifyPwd				&GetCacheReqModifyPwd(){ return m_stCacheReqModifyPwd;}

private:
	HWND m_notifyHwnd;

	QueryHoldDetailResultVector	m_aCacheHoldDetail;
	QueryHoldSummaryResultVector m_aCacheHoldSummary;
	QueryEntrustResultVector 	m_aCacheEntrust;
	QueryDealResultVector 		m_aCacheDeal;
	QueryCommInfoResultVector	m_aCacheCommInfo;
	QueryQuotationResultVector	m_aCacheQuotation;
	QueryTraderIDResultVector	m_aCacheTraderID;
	CClientRespError			m_stCacheError;	// 错误包
	CClientRespLogIn			m_stCacheLogIn;	// 登陆
	CClientRespUserInfo			m_stCacheUserInfo;// 客户信息
	CClientRespCode				m_stCacheCode;	// 验证码

	CClientRespEntrust			m_stCacheReqEntrust; // 委托买卖回包
	CClientRespCancelEntrust	m_stCacheReqCancelEntrust; 
	CClientRespSetStopLP		m_stCacheReqSetStopLP; 
	CClientRespCancelStopLP		m_stCacheReqCancelStopLP; 
	CClientRespModifyPwd		m_stCacheReqModifyPwd; 
};
#endif