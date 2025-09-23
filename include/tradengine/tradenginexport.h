#if !defined(_TRADE_EXPORT_H_)
#define _TRADE_EXPORT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// 导出的接口类
#include "tradenginestruct.h"

class AFX_EXT_CLASS CTradeNotify
{
public:
	
	// 连接成功
	virtual void	OnConnected() = 0;

	// 连接断开
	virtual void	OnDisConnected() = 0;

	// 查验证码回包
	virtual void	OnRespQueryCode(CClientRespCode* pResp) = 0;

	// 登录成功
	virtual void	OnLogInOK(CClientRespLogIn* pResp) = 0;

	// 登出成功
	virtual void	OnLogOutOK(CClientRespLogOut* pResp) = 0;

	// 客户信息回包
	virtual void	OnRespUserInfo(CClientRespUserInfo* pResp) = 0;

	// 商品信息回包
	virtual void	OnRespMerchInfo(CClientRespMerchInfo* pResp) = 0;

	// 委托买卖回包
	virtual void	OnRespEntrust(CClientRespEntrust* pResp) = 0;

	// 撤销委托单回包
	virtual void	OnRespCancelEntrust(CClientRespCancelEntrust* pResp) = 0;

	// 查询委托单回包
	virtual void	OnRespQueryEntrust(CClientRespQueryEntrust* pResp) = 0;

	// 查询可撤单回包
	virtual void	OnRespQueryCancelEntrust(CClientRespQueryCancelEntrust* pResp) = 0;

	// 查询成交回包
	virtual void	OnRespQueryDeal(CClientRespQueryDeal* pResp) = 0;

	// 查询持仓回包
	virtual void	OnRespQueryHold(CClientRespQueryHold* pResp) = 0;

	// 查询持仓汇总回包
	virtual void	OnRespQueryHoldTotal(CClientRespQueryHoldTotal* pResp) = 0;

	// 查对方交易员ID回包
	virtual void	OnRespQueryTraderID(CClientRespTraderID* pResp) = 0;	

	// 行情回包
	virtual void	OnRespQuote(CClientRespQuote* pResp) = 0;

	// 设置止盈止损回包
	virtual void	OnRespSetStopLP(CClientRespSetStopLP* pResp) = 0;

	// 取消止盈止损回包
	virtual void	OnRespCancelStopLP(CClientRespCancelStopLP* pResp) = 0;

	// 修改密码回包
	virtual void	OnRespModifyPwd(CClientRespModifyPwd* pResp) = 0;

	// 出错了
	virtual void	OnError(CClientRespError* pResp) = 0;
};

class AFX_EXT_CLASS CTradeClient
{
public:
	CTradeClient()
	{
		m_pNotify = NULL;
	}

	//
	void		  SetNotifyer(CTradeNotify* pNotify) { if (NULL == pNotify)return;	m_pNotify = pNotify; }

	// 连接服务器
	virtual bool32 Connect(const CString & StrServerAdd, const int32 iPort, bool32 bUseProxy, const CProxyInfo& stProxy) = 0;

	// 断开连接
	virtual	void   DisConnect() = 0;

	// 查验证码
	virtual bool32 ReqQueryCode(const CClientQueryCode& stReq) = 0;	

	// 查对方交易员ID
	virtual bool32 ReqQueryTraderID(const CClientQueryTraderID& stReq) = 0;	

	// 登录
	virtual bool32 ReqLogIn(const CClientReqLogIn& stReq) = 0;	

	// 登出
	virtual bool32 ReqLogOut(const CString& StrAccount, const CString& StrSessionID, const CString& StrSessionInfo) = 0;	

	// 请求客户信息
	virtual bool32 ReqUserInfo(const CClientReqUserInfo& stReq) = 0;

	// 请求商品信息
	virtual bool32 ReqMerchInfo(const CClientReqMerchInfo& stReq) = 0;

	// 请求委托买卖
	virtual bool32 ReqEntrust(const CClientReqEntrust& stReq) = 0;

	// 请求撤销委托单
	virtual bool32 ReqCancelEntrust(const CClientReqCancelEntrust& stReq) = 0;

	// 请求查询委托单
	virtual bool32 ReqQueryEntrust(const CClientReqQueryEntrust& stReq) = 0;

	// 请求查询可撤单
	virtual bool32 ReqQueryCancelEntrust(const CClientReqQueryCancelEntrust& stReq)=0;

	// 请求查询成交
	virtual bool32 ReqQueryDeal(const CClientReqQueryDeal& stReq) = 0;

	// 请求查询持仓
	virtual bool32 ReqQueryHold(const CClientReqQueryHold& stReq) = 0;

	// 请求查询持仓汇总
	virtual bool32 ReqQueryHoldTotal(const CClientReqQueryHoldTotal& stReq) = 0;

	// 请求行情
	virtual bool32 ReqQute(const CClientReqQuote& stReq) = 0;

	// 设置止盈止损
	virtual bool32 ReqSetStopLP(const CClientReqSetStopLP& stReq) = 0;

	// 取消止盈止损
	virtual bool32 ReqCancelStopLP(const CClientReqCancelStopLP& stReq) = 0;

	// 请求修改密码
	virtual bool32 ReqModifyPwd(const CClientReqModifyPwd& stReq) = 0;

	//由CreateTradeClient 创建的空间，比须由Destroy释放
	virtual void Destroy() = 0;

	//
	CTradeNotify*	m_pNotify;
};

AFX_EXT_CLASS CTradeClient* CreateTradeClient();


#endif // _TRADE_EXPORT_H_