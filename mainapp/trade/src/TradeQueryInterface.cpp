#include "stdafx.h"
#include "TradeQueryInterface.h"
#include "ShareFun.h"
#include "coding.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRespTradeNotify::~CRespTradeNotify()
{
	m_aCacheHoldDetail.clear();
	m_aCacheEntrust.clear();
	m_aCacheDeal.clear();
	m_aCacheCommInfo.clear();
	m_aCacheQuotation.clear();
	m_aCacheTraderID.clear();
}

// 连接成功
void CRespTradeNotify::OnConnected()
{
	PostMessage(m_notifyHwnd, WM_RESP_TRADE_NOTIFY, (WPARAM)EMT_CONNECTED, 0);
}

// 连接断开
void CRespTradeNotify::OnDisConnected()
{
	PostMessage(m_notifyHwnd, WM_RESP_TRADE_NOTIFY, (WPARAM)EMT_DISCONNECTED, 0);
}

// 查验证码回包
void CRespTradeNotify::OnRespQueryCode(CClientRespCode* pResp)
{
	m_stCacheCode = *pResp;
	PostMessage(m_notifyHwnd, WM_RESP_TRADE_NOTIFY, (WPARAM)EMT_QUERYCODE, 0);
}

// 登录成功
void CRespTradeNotify::OnLogInOK(CClientRespLogIn* pResp)
{
	m_stCacheLogIn = *pResp;
// 	string strName = "d:\\trace.txt";
// 	FILE* pFile = fopen(strName.c_str(), "w");
// 	if (NULL != pFile)
// 	{
// 		fclose(pFile);
// 	}

//	DEL(pResp);
	PostMessage(m_notifyHwnd, WM_RESP_TRADE_NOTIFY, (WPARAM)EMT_LOGINOK, 0);
}

// 登出成功
void CRespTradeNotify::OnLogOutOK(CClientRespLogOut* pResp)
{
	m_stCacheError = *pResp;
//	DEL(pResp);
	PostMessage(m_notifyHwnd, WM_RESP_TRADE_NOTIFY, (WPARAM)EMT_LOGOUTOK, 0);
}

// 客户信息回包
void CRespTradeNotify::OnRespUserInfo(CClientRespUserInfo* pResp)
{
	m_stCacheUserInfo = *pResp;
//	DEL(pResp);
	PostMessage(m_notifyHwnd, WM_RESP_TRADE_NOTIFY, (WPARAM)EMT_USERINFO, 0);
}

// 商品信息回包
void CRespTradeNotify::OnRespMerchInfo(CClientRespMerchInfo* pResp)
{
	m_aCacheCommInfo.clear();
	m_aCacheCommInfo = pResp->m_aMerchInfos;
//	DEL(pResp);
	PostMessage(m_notifyHwnd, WM_RESP_TRADE_NOTIFY, (WPARAM)EMT_MERCHINFO, 0);
}

// 委托买卖回包
void CRespTradeNotify::OnRespEntrust(CClientRespEntrust* pResp)
{
	m_stCacheReqEntrust = *pResp;
//	DEL(pResp);
	PostMessage(m_notifyHwnd, WM_RESP_TRADE_NOTIFY, (WPARAM)EMT_ENTRUST, 0);
}

// 撤销委托单回包
void CRespTradeNotify::OnRespCancelEntrust(CClientRespCancelEntrust* pResp)
{
	m_stCacheReqCancelEntrust = *pResp;
//	DEL(pResp);
	PostMessage(m_notifyHwnd, WM_RESP_TRADE_NOTIFY, (WPARAM)EMT_CANCELENTRUST, 0);
}

// 查询可撤单回包
void CRespTradeNotify::OnRespQueryCancelEntrust(CClientRespQueryCancelEntrust* pResp)
{
	m_aCacheEntrust.clear();
	m_aCacheEntrust = pResp->m_aQureyEntrust;
	//	DEL(pResp);
	PostMessage(m_notifyHwnd, WM_RESP_TRADE_NOTIFY, (WPARAM)EMT_QUERYENTRUST, 0);
}

// 查询委托单回包
void CRespTradeNotify::OnRespQueryEntrust(CClientRespQueryEntrust* pResp)
{
	m_aCacheEntrust.clear();
	m_aCacheEntrust = pResp->m_aQureyEntrust;
//	DEL(pResp);
	PostMessage(m_notifyHwnd, WM_RESP_TRADE_NOTIFY, (WPARAM)EMT_QUERYENTRUST, 0);
}

// 查询成交回包
void CRespTradeNotify::OnRespQueryDeal(CClientRespQueryDeal* pResp)
{
	m_aCacheDeal.clear();
	m_aCacheDeal = pResp->m_aQueryDeal;
//	DEL(pResp);
	PostMessage(m_notifyHwnd, WM_RESP_TRADE_NOTIFY, (WPARAM)EMT_QUERYDEAL, 0);
}

// 查询持仓回包
void CRespTradeNotify::OnRespQueryHold(CClientRespQueryHold* pResp)
{
	m_aCacheHoldDetail.clear();
	m_aCacheHoldDetail = pResp->m_aQueryHold;
//	DEL(pResp);
	PostMessage(m_notifyHwnd, WM_RESP_TRADE_NOTIFY, (WPARAM)EMT_QUERYHOLD, 0);
}

// 查询持仓汇总回包
void CRespTradeNotify::OnRespQueryHoldTotal(CClientRespQueryHoldTotal* pResp)
{
	m_aCacheHoldSummary.clear();
	m_aCacheHoldSummary = pResp->m_aQueryHoldTotal;
	PostMessage(m_notifyHwnd, WM_RESP_TRADE_NOTIFY, (WPARAM)EMT_QUERYHOLDSUMMARY, 0);
}

// 行情回包
void CRespTradeNotify::OnRespQuote(CClientRespQuote* pResp)
{
	m_aCacheQuotation.clear();
	m_aCacheQuotation = pResp->m_aQuote;

// 	int isize = m_aCacheQuotation.size();
// 	string strName = "d:\\trace.txt";
// 	FILE* pFile = fopen(strName.c_str(), "at+");
// 	for( int i=0; i<isize; i++)
// 	{
// 		TRACE(_T("商品代码: %s "), m_aCacheQuotation[i].stock_code);
// 		TRACE(_T("最高价: %.2f "), m_aCacheQuotation[i].up_price);
// 		TRACE(_T("最低价: %.2f "), m_aCacheQuotation[i].down_price);
// 		TRACE(_T("最新价: %.2f "), m_aCacheQuotation[i].last_price);
// 		TRACE(_T("买价: %.2f "), m_aCacheQuotation[i].buy_price);
// 		TRACE(_T("卖价: %.2f "), m_aCacheQuotation[i].sell_price);
// 		TRACE(_T("报价时间: %s\n"), m_aCacheQuotation[i].quote_time);
// 		
// 		CString str;
// 		string s;
// 		str.Format(_T("商品代码: %s ,最高价: %.2f ,最低价: %.2f ,最新价: %.2f ,买价: %.2f ,卖价: %.2f ,报价时间: %s\n"), 
// 			m_aCacheQuotation[i].stock_code,m_aCacheQuotation[i].up_price,m_aCacheQuotation[i].down_price,
// 			m_aCacheQuotation[i].last_price,m_aCacheQuotation[i].buy_price,m_aCacheQuotation[i].sell_price,m_aCacheQuotation[i].quote_time);
// 
// 		UnicodeToUtf8(str, s);
// 		
// 		if (NULL != pFile)
// 		{
// 			fprintf(pFile, s.c_str());
// 		}
// 	}
// 	if (NULL != pFile)
// 		fclose(pFile);

//	DEL(pResp);
	PostMessage(m_notifyHwnd, WM_RESP_TRADE_NOTIFY, (WPARAM)EMT_QUOTE, 0);
}

// 对方交易员ID回包
void CRespTradeNotify::OnRespQueryTraderID(CClientRespTraderID* pResp)
{
	m_aCacheTraderID.clear();
	m_aCacheTraderID = pResp->m_aQueryTraderID;

	PostMessage(m_notifyHwnd, WM_RESP_TRADE_NOTIFY, (WPARAM)EMT_QUERYTRADERID, 0);
}

// 设置止盈止损回包
void CRespTradeNotify::OnRespSetStopLP(CClientRespSetStopLP* pResp)
{
	m_stCacheReqSetStopLP = *pResp;
//	DEL(pResp);
	PostMessage(m_notifyHwnd, WM_RESP_TRADE_NOTIFY, (WPARAM)EMT_SETSTOPLP, 0);
}

// 取消止盈止损回包
void CRespTradeNotify::OnRespCancelStopLP(CClientRespCancelStopLP* pResp)
{
	m_stCacheReqCancelStopLP = *pResp;
//	DEL(pResp);
	PostMessage(m_notifyHwnd, WM_RESP_TRADE_NOTIFY, (WPARAM)EMT_CANCELSTOPLP, 0);
}

// 修改密码回包
void CRespTradeNotify::OnRespModifyPwd(CClientRespModifyPwd* pResp)
{
	m_stCacheReqModifyPwd = *pResp;
	//	DEL(pResp);
	PostMessage(m_notifyHwnd, WM_RESP_TRADE_NOTIFY, (WPARAM)EMT_MODIFY_PWD, 0);
}

// 出错了
void CRespTradeNotify::OnError(CClientRespError* pResp)
{
	if(pResp)
	{
		m_stCacheError = *pResp;	
		PostMessage(m_notifyHwnd, WM_RESP_TRADE_NOTIFY, (WPARAM)EMT_ERROR, 0);
	}
//	DEL(pResp);
}