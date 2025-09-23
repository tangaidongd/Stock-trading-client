#ifndef _DlgTradeQuery_H_
#define _DlgTradeQuery_H_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTradeQuery.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CDlgTradeQuery dialog
#include "DialogEx.h"
#include "GridCtrlNormal.h"
#include "XScrollBarBaCai.h"
#include "DlgStatic.h"
#include "XScrollBar.h"

//#include "TradeBidInterface.h"
#include "TradeQueryInterface.h"
#include "TradeLoginInterface.h"
#include "synch.h"

#include "ExportMT2DispatchThread.h"
#include "MyGroupBoxButton.h"
#include "SimulationHomePage.h"


#include "ComboBoxCheck.h"

#include <vector>
using std::vector;
#include <set>
using std::set;
#include <map>
using std::map;

#define  WM_MESSAGE_PARENT		WM_USER+1000

enum E_QueryType
{
	EQT_QueryHoldDetail  = 0x650,
	EQT_QueryHoldSummary,
	EQT_QueryEntrust,
	EQT_QueryLimitEntrust,
	EQT_QueryDeal,
	EQT_QueryUserInfo,
	EQT_QueryCommInfo,
	EQT_QueryQuote,
	EQT_QueryHome,
	EQT_QueryCancel,
	EQT_QueryNone,
};

class CDlgTradeQuery : public CDialog, public iTradeBidNotify, public iExportMT2DispatchThreadProcessor
{
private:
	enum E_StopLossProfitType
	{
		EST_StopLoss = 0,
		EST_StopProfit,
		EST_StopLossProfit,
	};

	typedef vector<T_CommUserInfoList>		CommUserInfoListVector;
	CommUserInfoListVector m_CommInfoVector;

	// Construction
public:
	CDlgTradeQuery(CWnd* pParent = NULL);   // standard constructor
	~CDlgTradeQuery();
	
public:
	virtual void	DispatchThreadProcess(bool bSucDisp, LPARAM l);
	
	void IncDTPNeedProcessFlag();
	void DecDTPNeedProcessFlag();
	void DoQueryAsy(E_QueryType eType);
	volatile long		m_lWaitTradeDTPProcess;
	
	LockSingle m_LockThread;
	LockSingle m_Lock_HoldDetail;
	LockSingle m_Lock_Entrust;
	LockSingle m_Lock_Deal;
	LockSingle m_Lock_TraderID;
	LockSingle m_Lock_UserInfo;
	LockSingle m_Lock_CommInfo;
	LockSingle m_Lock_Quotation;
	bool m_bInitCommInfo;

	CString m_strMidCode;
	CFont m_Font;

	CSimulationHomePage		m_dlgHomePage;	// 主页
///	CDialogChangePassword	m_dlgChangePW;	// 修改密码
private:
	BYTE	m_bAccountInfo;	// 是否是账户信息（如果不是帐户信息，也要更新首页或者3个参数的信息）
public:	// iTradeBidNotify
 	virtual void	OnLoginStatusChanged(int iCurStatus, int iOldStatus);
	virtual bool32	OnInitQuery(CString &strTipMsg);
	virtual bool32	OnQueryUserInfo(CString &strTipMsg);
	virtual void	OnInitCommInfo();

	virtual void	OnQueryHoldDetailResponse();
	virtual void	OnQueryHoldSummaryResponse();
//	virtual void	OnQueryEntrustResponse();
	virtual void	OnQueryLimitEntrustResponse();
	virtual void	OnQueryDealResponse();
	virtual void	OnQueryCommInfoResponse();
	virtual void	OnQueryTraderIDResponse();
	virtual void	OnQueryUserInfoResponse();
	virtual void	OnQueryQuotationResponse();
	virtual void	OnDisConnectResponse();

	virtual void	OnReqEntrustResponse(bool32 bShowTip=true);
	virtual void	OnReqCancelEntrustResponse();
	virtual void	OnReqSetStopLPResponse();
	virtual void	OnReqCancelStopLPResponse();
	virtual void	OnReqModifyPwdResponse();
public:
	// 持仓明细
	void ReloadGridHoldDetailMain();
	void UpdateOneHoldDetailMainRow( int iRow, const T_RespQueryHold &stOut);
	// 持仓汇总
	void ReloadGridHoldSummary();
	void UpdateOneHoldSummaryRow( int iRow, const T_RespQueryHoldTotal &stOut);
	// 查委托
	void ReloadGridEntrustMain();
	void UpdateOneEntrustMainRow( int iRow, const T_RespQueryEntrust &stOut);
	// 指价委托
	void ReloadGridEntrust();
	void UpdateOneEntrustRow( int iRow, const T_RespQueryEntrust &stOut);
	// 查成交
	void ReloadGridDeal();
	void UpdateOneDealRow( int iRow, const T_RespQueryDeal &stOut);
	// 查商品信息
	void ReloadGridCommInfo();
	void UpdateOneCommInfoRow( int iRow, const T_CommUserInfoList lst);
	// 查账户
	void ReloadGridUserInfo();
	void UpdateOneUserInfoRow( int iRow, const CClientRespUserInfo &stOut);
	void ReloadGridUserInfoMain();
	void UpdateOneUserInfoMainRow( int iRow, const T_CommUserInfoList lst);

public:
	int				Create(CWnd *pParent) { return CDialog::Create(IDD, pParent); }

	void			SetTradeBid(iTradeBid *pTradeBid);

	void			RecalcLayout(bool bNeedDraw);
public:
	void SetTradeStatus(E_TradeLoginStatus status);
	void SetCurSelToHoldDetail();
	BOOL NetWorkStatusNotify(const int iErrorCode);
private:
	// Dialog Data
	//{{AFX_DATA(CDlgTradeQuery)
	enum { IDD = IDD_DIALOG_TRADEQUERY };
	//CDlgStatic		&m_StaticAccount;
	//}}AFX_DATA

public:
	typedef vector<CGridCtrlNormal *>	GridCtrlVector;
	GridCtrlVector		m_aGrid;

	typedef vector<CXScrollBar *> XScrollBarVector;
	XScrollBarVector	m_aScroll;

	/*	CGridCtrlNormal*/	CGridCtrlNormal		m_GridHold;
	/*CXScrollBarBaCai*/	CXScrollBar	m_XBarHoldH;
	/*CXScrollBarBaCai*/	CXScrollBar	m_XBarHoldV;
	// 表格

	// 概况
	CGridCtrlNormal		m_GridHoldDetail;
	CXScrollBar	m_XBarHoldDetailH;
	CXScrollBar	m_XBarHoldDetailV;
	CGridCtrlNormal		m_GridEntrust;
	CStatic m_staticDisEntrust;
	CXScrollBar	m_XBarEntrustH;
	CXScrollBar	m_XBarEntrustV;
	CGridCtrlNormal		m_GridAccInfo;
	CXScrollBar	m_XBarAccInfoH;
	CXScrollBar	m_XBarAccInfoV;
	// 委托查询
	CGridCtrlNormal		m_GridEntrustMain;
	CXScrollBar	m_XBarEntrustMainH;
	CXScrollBar	m_XBarEntrustMainV;
	// 成交查询
	CGridCtrlNormal		m_GridDeal;
	CXScrollBar	m_XBarDealH;
	CXScrollBar	m_XBarDealV;
	// 持仓明细
	CGridCtrlNormal		m_GridHoldDetailMain;
	CXScrollBar	m_XBarHoldDetailMainH;
	CXScrollBar	m_XBarHoldDetailMainV;
	// 持仓汇总
	CGridCtrlNormal		m_GridHoldSummary;
	CXScrollBar	m_XBarHoldTotalH;
	CXScrollBar	m_XBarHoldTotalV;
	// 用户信息
	CGridCtrlNormal		m_GridAccInfoMain;
	CXScrollBar	m_XBarAccInfoMainH;
	CXScrollBar	m_XBarAccInfoMainV;
	// 商品信息
	CGridCtrlNormal		m_GridCommInfo;
	CXScrollBar	m_XBarCommInfoH;
	CXScrollBar	m_XBarCommInfoV;

	// 事件表格
	CGridCtrlNormal		*m_pGridEvent;
	
	iTradeBid			*m_pTradeBid;
	E_TradeLoginStatus	m_eTradeStatus;

	// 概况
	CMyGroupBoxButton	m_GroupHoldDetail;
	CStatic	m_StaticHoldDetail;
// 	CMyGroupBoxButton	m_GroupEntrust;
// 	CStatic m_StaticEntrust;
	CButton	m_GroupAccInfo;
	CStatic m_StaticAccInfo;

	// 委托查询
	CButton	m_GroupEntrustMain;
	CStatic m_StaticEntrustMain;
	CStatic m_StaticEntrustMainCommInfo;
	CComboBox m_DropEntrustMainCommInfo;
	CStatic m_StaticEntrustMainType;
	CComboBox m_DropEntrustMainType;
	CStatic m_StaticEntrustMainStatus;
	CComboBox m_DropEntrustMainStatus;
	CButton m_BtnEntrustMainQuery;
	CButton m_BtnEntrustMainReset;
	CStatic m_StaticEntrustMainBuySell;
	CComboBox m_DropEntrustMainBuySell;
	CStatic m_StaticEntrustMainOpenClose;
	CComboBox m_DropEntrustMainOpenClose;

	// 成交查询
// 	CButton	m_GroupDeal;
// 	CStatic m_StaticDeal;
	CStatic m_StaticDealCommInfo;
	CComboBox m_DropDealCommInfo;
	CStatic m_StaticDealBuySell;
	CComboBox m_DropDealBuySell;
	CStatic m_StaticDealOpenClose;
	CComboBox m_DropDealOpenClose;
	CButton m_BtnDealQuery;
	CButton m_BtnDealReset;
	
	// 持仓汇总
// 	CButton	m_GroupHoldTotal;
// 	CStatic m_StaticHoldTotal;

	// 账户信息
// 	CButton	m_GroupAccInfoMain;
// 	CStatic m_StaticAccInfoMain;
	
	// 商品信息
	CButton	m_GroupCommInfo;
	CStatic m_StaticCommInfo;
	CStatic m_StaticCommInfoType;
	CComboBoxCheck m_DropCommInfoType;

// 	int m_iGroupHoldDetailHeight;	// 持仓明细groupbox高度（概况）
// 	int m_iGroupEntrustHeight;		// 查限价委托groupbox高度（概况）
	BOOL m_bActivaGroupHoldDetail;
	BOOL m_bActivaGroupEntrust;
	
	int m_iAllHoldDetailMainHeight;	//持仓明细总记录高度
	int m_iAllHoldSummaryMainHeight;
	int m_iAllHoldDetailHeight;
	int m_iAllEntrustMainHeight;
	int m_iAllEntrustHeight;
	int m_iAllDealHeight;	
	int m_iAllCommInfoHeight;
	int m_iAllAccInfoHeight;

	BOOL m_bNetWorkStatus;	//0-断开 1-正常
	BOOL m_bWndStatus;
	
	CFont	m_pFont;

	E_QueryType  m_eCurQueryType;

public:
	T_TradeLoginInfo *m_pTradeLoginInfo;
	_CommInfoType m_CommInfoType;
	LockSingle m_LockCommInfoType;
	_CommInfoType m_OtherFirm;
	LockSingle m_LockOtherFirm;

	T_RespQueryHold *m_pHoldDetailSelect;
	T_RespQueryHold *m_pHoldDetailMainSelect;
	T_RespQueryHoldTotal *m_pHoldSummarySelect;
	QueryQuotationResultVector m_QuotationResultVector;
	QueryCommInfoResultVector m_CommInfoResultVector;

	int32	m_iReqEntrustCnt;			// 已经发送请求委托的个数
	int32	m_iRevokeLimitOrderCnt;		// 已经发送请求撤销指价单的个数
	double	m_dFeeScale;				// 手续费比例，模拟交易中写死万5

private:
	// 模拟交易在每次查询行情返回时进行撮合限价单和持仓单中的止损止盈
	void DealEntrustNo();

	// 模拟交易退出时，撤销所有指价单
	void RevokeAllLimitOrder();
	//撤销所有止盈止损
	void RevokeAllProfitLoss();
public:
	void CreateControl();
	void InitComBox(CFont *pFont);
	void InitCommIfo();
	// 委托查询
	void CreateEntrustMainTable(CFont *pFont);
	// 指价委托单
	void CreateEntrustTable(CFont *pFont);
	// 成交查询
	void CreateDealTable(CFont *pFont);
	// 持仓明细
	void CreateHoldDetailMainTable(CFont *pFont);
	// 持仓汇总
	void CreateHoldSummaryMainTable(CFont *pFont);

	// 账户信息
	void CreateUserInfoMainTable(CFont *pFont);
	// 商品信息
	void CreateCommInfoTable(CFont *pFont);

	// 显示/隐藏窗口
	void ShowHideGeneral(int nCmdShow);
	void ShowHideEntrustMain(int nCmdShow);
	void ShowHideDeal(int nCmdShow);
	void ShowHideHoldDetailMain(int nCmdShow);
	void ShowHideHoldSummaryMain(int nCmdShow);
	void ShowHideAccInfoMain(int nCmdShow);
	void ShowHideCommInfo(int nCmdShow);
	void ShowHideLimitEntrust(int nCmdShow);
	void ShowHideHome(int nCmdShow);

	// 请求各列表信息
	void QueryListData(E_QueryType eQueryType);
	bool32 QueryHoldDetail();	// 查持仓明细
	bool32 QueryHoldSummary();// 查持仓汇总
//	bool32 QueryEntrust();	// 查委托
	bool32 QueryLimitEntrust();	// 指价委托
	bool32 QueryDeal();	// 查成交
	bool32 QueryUserInfo();	// 查账户信息
	bool32 QueryCommInfo();	// 查商品信息
	bool32 QueryQuotation();	// 查行情
	bool32 QueryTraderID();	// 查交易员ID
	bool32 RevokeLimitOrder();
	bool32 RevokeLimitOrder(CString strNo); // 按照委托单号撤销指价单
	bool32 SetLossProfit(const CClientReqSetStopLP *pSet,CString &strTipMsg);
	bool32 RevokeLossProfit(E_StopLossProfitType eType,bool bMain);	// bMain 是不是主菜单
	bool32 ReqEntrust(const CClientReqEntrust& stReq, CString &strTipMsg); // 委托买卖
	const QueryCommInfoResultVector GetCommInfo();

	void GetRiseFallColor( COLORREF &clrRise, COLORREF &clrFall, COLORREF &clrKeep );
	bool FindCommInfoType( int iIndex, T_CommInfoType &commInfoType);
	int GetCommInfoTypeSize();
	char* GetCommInfoTypeName(int iIndex);
	bool FindOtherFirm( int iIndex, T_CommInfoType &commInfoType);
	int GetOtherFirmSize();
	char* GetOtherFirmName(int iIndex);

	const _CommInfoType GetCommInfoType();
	const _CommInfoType GetOtherFirm();
	/*const char**/CString GetCommName(const char* pCommID);
	LockSingle m_LockGetName;

	void RefreshList();	//刷新列表
	void SetGridHeadColor();	// 设置表头的颜色 
	// 右键选中某一项
	const T_RespQueryEntrust* GetLimitOrderSelect();
	const T_RespQueryHold* GetHoldDetailSelect();
	const T_RespQueryHold* GetHoldDetailMainSelect();
	const T_RespQueryHoldTotal* GetHoldSummarySelect();

	void GetBuySellPrice(const char* pCommID,double &dBuyPrice,double &dSellPrice,CString &dTradeUnit,double &dMarginratio);
	double GetFloatPL();
	long GetTotalQty();
	void GetTodayFloatPL(OUT double &dFloatPL, OUT double &dFee);		// 当日平仓盈亏合计,当日手续费合计
	LockSingle m_LockGetPrice;
	LockSingle m_LockFloatPL;
	LockSingle m_LockTotalQty;

public:
	const CString GetBuySellShow(const CString sBuySell);
	const CString GetEntrustStatusShow(const CString sStatus);
	const char* GetEntrustTypeShow(const short sType);
	const CString GetOpenCloseShow(const CString sType);
	const CString GetDealTypeShow(const CString sType);
	const char* GetDealOperateTypeShow(const int iType);
	const char* UtcTOLocalTime(time_t t,char *pTime);
	const CString GetUserInfoStatus(const CString chStatus);
	const char* GetCommInfoStatus(const short sStatus);
	const char* GetFeeType(const int iType);
	const char* GetFeeGetType(const int iType);
	const char* GetMarginType(const int iType);
	const char* GetDeferType(const int iType);
	void EntrustResultNotify(bool bSuc=false);
	void WndMaxMin();
	void WndClose(int iMode);
	void StopTrade(int iMode);

	void ShowQueryChange(E_QueryType eQueryType);

	void GridClickHoldDetailSelect();
	void GridClickHoldSummarySelect();
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTradeQuery)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgTradeQuery)
	afx_msg BOOL		OnEraseBkgnd(CDC* pDC);
	afx_msg void		OnSize(UINT nType, int cx, int cy);
	afx_msg void		OnBtnEntrustMainReset();
	afx_msg void		OnBtnEntrustMainQuery();
	afx_msg void		OnBtnDealReset();
	afx_msg void		OnBtnDealQuery();
	afx_msg void		OnCbnSelchangeCommInfoType();
	afx_msg void		OnPaint();
	afx_msg HBRUSH		OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void		OnTimer(UINT nIDEvent);
	afx_msg void		OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg	void		OnGridRDBButtonDown( NMHDR *pNotifyStruct, LRESULT* pResult );
	afx_msg void		OnGridSelChange(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void		OnLButtonUp(UINT nFlags, CPoint point);
	LRESULT				OnMsgUpdataCommIfo( WPARAM w, LPARAM l );
	LRESULT				OnMsgWndClose( WPARAM w, LPARAM l );
	LRESULT				OnMsgGroupBoxPosChanged( WPARAM w, LPARAM l );
	virtual				BOOL	PreTranslateMessage(MSG* pMsg);
	afx_msg LRESULT		OnDisEntrust(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!_DlgTradeQuery_H_