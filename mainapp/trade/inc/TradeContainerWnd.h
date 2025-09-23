#ifndef _TRADECONTAINERWND_H_
#define _TRADECONTAINERWND_H_

#include "WndRectButton.h"
#include "TradeLoginInterface.h"
#include "DlgTradeBidContainer.h"
#include "DlgTradeLogin.h"
#include "TradeQueryInterface.h"
#include "ExportMT2DispatchThread.h"
#include "AuthoPlugin.h"
#include "DlgQuickOrder.h"
#include "synch.h"

#include <vector>
using std::vector;
#include <list>
using std::list;
#include <map>
using std::map;
#include <set>
using std::set;

class CMyContainerBar;
class CDlgTradeLogin;
class CDlgQuickOrder;
class CViewData;

class CTradeContainerWnd 
: public CRectButtonWndSimpleImpl
 , public iTradeLogin
 , public iTradeBid
{
	DECLARE_DYNCREATE(CTradeContainerWnd);

public:
	CTradeContainerWnd(E_TradeLoginType eTradeType);
	CTradeContainerWnd(){}
	~CTradeContainerWnd();

	int	 Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext  = NULL );

	bool Login();
	void LoginOK();
	// CRectButtonWndSimpleImpl
public:
	virtual	void	GetAllBtns(WndRectButtonVector &aBtns);
	virtual void	OnDraw(CDC &dc);			// 绘制时，继承对象调用此接口绘制按钮
	virtual	void	RecalcLayout(bool32 bNeedPaint = true);			// 分配各个按钮，子控件size

// 	// iTradeLogin
 public:
	virtual bool    Connect(const T_TradeLoginInfo &stLoginInfo, iTradeLoginNotify *pNotify);
	virtual void	DisConnect(int iMode=0);
 	virtual void	StopLogin(int iMode=0);
 	virtual void	OnLoginDlgCancel();		// 登录界面要关闭了
// 
	// iTradeBid
 public:
	 // 查询
	virtual	void	AddTradeBidNotify(iTradeBidNotify *pNotify);
	virtual void	RemoveTradeBidNotify(iTradeBidNotify *pNotify);

	// 请求客户信息
	virtual bool32 ReqUserInfo(const CClientReqUserInfo& stReq, CString &strErrMsg);
	// 请求商品信息
	virtual bool32 ReqMerchInfo(const CClientReqMerchInfo& stReq, CString &strErrMsg);
	// 请求查询交易员ID
	virtual bool32 ReqQueryTraderID(const CClientQueryTraderID& stReq, CString &strErrMsg);
	// 请求委托买卖
	virtual bool32 ReqEntrust(const CClientReqEntrust& stReq, CString &strTipMsg);
	// 请求撤销委托单
	virtual bool32 ReqCancelEntrust(const CClientReqCancelEntrust& stReq, CString &strTipMsg);
	// 请求查询委托单
	virtual bool32 ReqQueryEntrust(const CClientReqQueryEntrust& stReq, CString &strErrMsg);
	// 请求查询可撤单
	virtual bool32 ReqQueryCancelEntrust(const CClientReqQueryCancelEntrust& stReq, CString &strErrMsg);
	// 请求查询成交
	virtual bool32 ReqQueryDeal(const CClientReqQueryDeal& stReq, CString &strErrMsg);
	// 请求查询持仓
	virtual bool32 ReqQueryHold(const CClientReqQueryHold& stReq, CString &strErrMsg);
	// 请求查询持仓汇总
	virtual bool32 ReqQueryHoldSummary(const CClientReqQueryHoldTotal& stReq, CString &strErrMsg);
	// 请求行情
	virtual bool32 ReqQute(const CClientReqQuote& stReq, CString &strErrMsg);
	// 设置止盈止损
	virtual bool32 ReqSetStopLP(const CClientReqSetStopLP& stReq, CString &strTipMsg);
	// 取消止盈止损
	virtual bool32 ReqCancelStopLP(const CClientReqCancelStopLP& stReq, CString &strTipMsg);
	// 请求修改密码
	virtual bool32 ReqModifyPwd(const CClientReqModifyPwd& stReq, CString &strTipMsg);

	// Get
	QueryHoldDetailResultVector				&GetCacheHoldDetail(){ return m_aCacheHoldDetail;}
	const QueryHoldSummaryResultVector		&GetCacheHoldSummary() { return m_aCacheHoldSummary; }
	QueryEntrustResultVector				&GetCacheEntrust(){ return m_aCacheEntrust;}
	const QueryDealResultVector				&GetCacheDeal(){ return m_aCacheDeal;}
	const QueryCommInfoResultVector			&GetCacheCommInfo(){ return m_aCacheCommInfo;}
	const CClientRespUserInfo				&GetCacheUserInfo(){ return m_stCacheUserInfo;}
	QueryQuotationResultVector				&GetCacheQuotation(){ return m_aCacheQuotation;}
	const QueryTraderIDResultVector			&GetCacheTraderID(){ return m_aCacheTraderID;}
	const CClientRespError					&GetCacheError(){ return m_stCacheError;}
	const CClientRespLogIn					&GetCacheLogIn(){ return m_stCacheLogIn;}
	const CClientRespCode					&GetCacheCode(){ return m_stCacheCode;}

	const CClientRespEntrust				&GetCacheReqEntrust(){ return m_stCacheReqEntrust;}
	const CClientRespCancelEntrust			&GetCacheReqCancelEntrust(){ return m_stCacheReqCancelEntrust;}
	const CClientRespSetStopLP				&GetCacheReqSetStopLP(){ return m_stCacheReqSetStopLP;}
	const CClientRespCancelStopLP			&GetCacheReqCancelStopLP(){ return m_stCacheReqCancelStopLP;}
	const CClientRespModifyPwd				&GetCacheReqModifyPwd(){ return m_stCacheReqModifyPwd;}

	// 心跳
	virtual int	HeartBeating(CString &strTipMsg);

	//界面大小化
	virtual void	WndMaxMin();
	virtual BOOL	GetWndStatus() {return m_bWndMaxMinStatus;};
	virtual void	WndClose(int iMode);	//0-重新登录1-界面注销2-程序注销3-关闭整个界面
	virtual E_TradeLoginStatus GetTradeLoginStatus();

public:

// 	// 交易状态
 	bool			IsLogin();
 	void			TryLogin();		// 如果没登录，则弹出登录框登录，已经登录则显示交易界面
	void			LoginErr();
public:
	// Bar操作
	CMyContainerBar *GetParentBar() const;
	
	// 放大缩小
	BOOL			Maximize();
	BOOL			IsMaximized() const;
	BOOL			RestoreSize();
	BOOL			Minimize();
	BOOL			IsMinimized();
	void			DoMyToggleDock();

	// 颜色
	void			SetBtnColors();

	// 
	void			DoOnLoginSuc();
	void			DoStopTrade();
	void			CheckLoginSuc(int eRecvPackType);

	void			ShowProperChild();
	void			SetLoginStatus( int eStatus );


	void			OnTradeLogIn(int32 iFlag);  // 实盘交易
	void			OnTradeQuotationPush(const CClientRespQuote *pAns, const int iCount);
	void			OnSimulateTradeLogIn(int32 iFlag, CString strQuoteFlag=_T("0")); // 模拟交易
	void			OnMsgSimulateTradeQuotePush(IN CMerch *pMerch); // 模拟交易商品行情更新了

	CMerch*			TradeCode2ClientMerch(const CString& StrTradeCode);
	CString			ClientMerch2TradeCode(const CMerch* pMerch);
public:

	CTradeClient *m_tradeClient;
	CRespTradeNotify *m_respTradeNotify;

	QueryHoldDetailResultVector	m_aCacheHoldDetail;
	QueryHoldSummaryResultVector m_aCacheHoldSummary;
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
	CClientRespModifyPwd	    m_stCacheReqModifyPwd;
	
	BOOL m_bWndMaxMinStatus;	//窗口当前状态 0-最小 1-最大
	CSize m_MaxSize;			// 缩小之前的大小

public:
	void DoOnQueryHoldDetailResponse();
	void DoOnQueryHoldSummaryResponse();
	void DoOnQueryEntrustResponse();
	void DoOnQueryDealResponse();
	void DoOnQueryCommInfoResponse();
	void DoOnQueryTraderIDResponse();
	void DoOnQueryUserInfoResponse();
	void DoOnQueryQuotationResponse();
	void DoOnErrorResponse();
	void DoOnDisConnectResponse();
	void DoOnQueryCodeResponse();

	void DoOnReqEntrustResponse();
	void DoOnReqCancelEntrustResponse();
	void DoOnReqSetStopLPResponse();
	void DoOnReqCancelStopLPResponse();
	void DoOnReqModifyPwdResponse();
	void SetQuoteValue();

public:
	volatile int			m_eLoginStatus;
	CDlgTradeLogin			*m_pDlgLogin;
	CDlgTradeBidContainer	*m_pDlgTradeBidParent;

	int32 iRespCnt;
	int32 m_MaxRespCnt;

private:
	CWndRectButton		*m_pBtnClose;
	CWndRectButton		*m_pBtnMax;
	CWndRectButton		*m_pBtnMin;
	CButton				m_BtnLogin;

 	iTradeLoginNotify	*m_pLoginNotify;
 	T_TradeLoginInfo	m_stTradeLoginInfo;
 	char				m_szAccountCur[100];

	volatile bool		m_bLoginSuc;
	volatile long		m_lWaitTradeDTPProcess;
	typedef vector<iTradeBidNotify *> iTradeBidNotifyVector;
	iTradeBidNotifyVector	m_aTradeBidNotify;
	iTradeBidNotify		*m_pTradeBidNotify;

	bool32				m_bRecvMsg;

	// 缓存 END

	DECLARE_MESSAGE_MAP();
	// Generated message map functions
	//{{AFX_MSG(CTradeContainerWnd)
	afx_msg	int			OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void		OnDestroy();
	afx_msg void		OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void		OnTradeClose();
	afx_msg void		OnTradeMax();
	afx_msg void		OnTradeMin();
	afx_msg void		OnBtnClick(NMHDR *pHdr, LRESULT *pResult);
	afx_msg void		OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void		OnTimer(UINT nIDEvent);
	afx_msg	LRESULT		OnMsgRespTradeNotify(WPARAM w, LPARAM l);
	LRESULT				OnMsgHideTradeWnd( WPARAM w, LPARAM l );

	afx_msg	void		OnBtnTradeLogin();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_MSG
};

#endif //!_TRADECONTAINERWND_H_