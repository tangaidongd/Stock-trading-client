// #if !defined(AFX_TRADE_H__57B8CCC7_22B9_46E2_8CDD_37DA8F62F230__INCLUDED_)
// #define AFX_TRADE_H__57B8CCC7_22B9_46E2_8CDD_37DA8F62F230__INCLUDED_
#ifndef _DLGTRADECLOSE_H_
#define _DLGTRADCLOSE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CDlgTradeClose.h : header file
//
#include "TradeLoginInterface.h"
#include "TradeQueryInterface.h"
#include <deque>
#include "synch.h"

#include "DlgTradeQuery.h"
#include "NCButton.h"
#include "NewSpinButtonCtrl.h"
#include "FloatEdit.h"
using namespace std;
/////////////////////////////////////////////////////////////////////////////
// CDlgTradeClose dialog

class CDlgTradeClose : public CDialog, public iTradeBidNotify
{
// Construction
public:
	CDlgTradeClose(CWnd* pParent = NULL);   // standard constructor
	~CDlgTradeClose();
// Dialog Data
	//{{AFX_DATA(CTrade)
	enum { IDD = IDD_DIALOG_CLOSE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrade)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

public:
	int	Create(CWnd *pParent) { return CDialog::Create(IDD, pParent); }

	void CreateControl();
	void InitControl();
	void SetPrice();
	void SetHandRange();
	bool FindCommInfoType( int iIndex, T_CommInfoType &commInfoType);
	int GetCommIndex(const char* pCommID);
	void RecalcLayout( bool bNeedDraw );
	void ShowHideControl_MarketPrice(int nCmdShow);
	void ShowHideControl_LimitPrice(int nCmdShow);
	
	void SetTradeBid( iTradeBid *pTradeBid );
	int GetPointCount(const double dSpread);
	void ResetInitControl(E_TradeType eType);
	void UpdataStopLossProfitShow();
	int WarningMsg(CString &strWarn);
	void LoadTradeCloseInfo();
	void SaveTradeCloseInfo();
	bool FindOtherFirm( int iIndex, T_CommInfoType &commInfoType);

	void TradeTypeChange(E_TradeType  tradeType);
	bool32 IsBackOpen(CString strCode, double dPrice, int iHand);	// 是否能反手建仓
	// iTradeBidNotify
public:
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
	CDlgTradeQuery *m_pTradeQuery;
	iTradeBid *m_pTradeBid;
	bool m_bInitCommInfo;
	_CommInfoType m_TradeID;
	LockSingle m_LockTradeID;
	T_CommInfoType m_OtherID;

	CStatic	m_StaticEntrustType;
	CComboBox m_DropCommInfo;		// 商品
	CEdit m_EditCommInfo;			// 商品
	CStatic	m_StaticCommInfo;
	CStatic	m_StaticBuySell;

	CComboBox m_DropHand;		// 手数
	CStatic	m_StaticHand;
	CEdit m_EditHand;
	CNewSpinButtonCtrl m_SpinHand;

	CStatic m_StaticDetail;		// 明细
	CStatic m_EditDetail;

	// 指价平仓控件
	CEdit m_EditBuyPrice;
	CEdit m_EditSellPrice;
	CStatic m_StaticPrice;
	CFloatEdit m_EditPrice;
	CNewSpinButtonCtrl m_SpinPrice;
	CStatic m_StaticMarginTip;
	CFloatEdit m_EditStopLoss1;
	CEdit m_EditStopLoss2;
	CNewSpinButtonCtrl m_SpinStopLoss;
	CStatic m_StaticStopLoss;
	CFloatEdit m_EditStopProfit1;
	CEdit m_EditStopProfit2;
	CNewSpinButtonCtrl m_SpinStopProfit;
	CStatic m_StaticStopProfit;
	// 市价平仓
	CStatic m_StaticMarginTipMarketPrice;
	CEdit m_EditPoint;
	CNewSpinButtonCtrl m_SpinPoint;
	CEdit m_EditBack;
	CNewSpinButtonCtrl m_SpinBack;
	BOOL m_bCheckStopLoss;
	BOOL m_bCheckStopProfit;

	CStatic m_StaticAfterOk;	// 处理中
	CStatic	m_StaticBKHand;		// 手数编辑框的底色
	CStatic m_StaticBKPoint;
	CStatic m_StaticBKBack;
	CStatic m_StaticBKStopProfit1;
	CStatic m_StaticBKStopProfit2;
	CStatic m_StaticBKStopLoss1;
	CStatic m_StaticBKStopLoss2;

	CStatic m_StaticCheckStopLoss;
	CStatic m_StaticCheckStopProfit;
	CStatic m_StaticCheckAllowPoint;
	CStatic m_StaticCheckBeforeOrder;
	CStatic m_StaticCheckBack;

	//下单前确认
	CStatic m_StaticTip;

	int m_iCloseChannel;	// 平仓途径 0-按钮平 1-右键菜单平
	E_EntrustType m_EntrustType;

	_CommInfoType m_CommInfoType;
	LockSingle m_LockCommInfoType;
	T_CommInfoType m_CommInfo;
	QueryCommInfoResultVector m_QueryCommInfoResultVector;
	QueryQuotationResultVector m_QuotationResultVector;
	CClientRespUserInfo		m_QueryUserInfoResult;

	BOOL m_bCheckAllowPoint;
	BOOL m_bCheckBack;
	BOOL m_bCheckBeforeOrder;
	int m_iQty;	// 手数
	int m_iQtyMax;	// 可填范围 大
	int m_iQtyMin;	// 可填范围 小
	int m_iPoint;
	int m_iBack;
	int m_iStaticValidity;	// 有效期所占高度(市价平仓时要放在价格上方)

	int m_iOpenQtyMax;
	int m_iOpenQtyMin;
	int m_iPointMax;
	int m_iPointMin;
	int m_iPointDef;

 	string m_strQty;
 	string m_strPrice;
 	string m_strPoint;
 	string m_strBack;
	string m_strStopLoss;
	string m_strStopProfit;
	T_TradeLoginInfo *m_pTradeLoginInfo;

	double m_dPriceStep;
	int m_iPointCount;
	
	// 字体
	CFont m_fontSmall;
	CFont m_fontBig;
	CFont m_fontEditSmall;
	CFont m_fontEditBig;
	CFont m_fontEditMid;

	E_TradeType m_eCloseType;
	T_RespQueryHold *m_pHoldDetailQueryOut;
	QueryTraderIDResultVector	m_QueryTraderIDVector;

	double m_dStopLoss1;
	double m_dStopProfit1;

	double m_dBuyPrice;
	double m_dSellPrice;
	int	   iMerchType; // 商品类型

	bool m_bInitImg;// 是否初始化了图像资源

	Image *m_pImgCheck;
	Image *m_pImgUncheck;
	Image *m_pImgMax;
	Image *m_pImgSell;
	Image *m_pImgBuy;
	Image *m_pImgOrder;
private:
	void OnBtnOk();
	void DrawButtons(CDC* pDC);
	int TButtonHitTest(CPoint point);
	void AddButton(LPRECT lpRect, Image *pImage, UINT nCount=3, UINT nID=0, LPCTSTR lpszCaption=NULL);
	void InitImgResource();

	std::map<int, CNCButton> m_mapButton;
	int	m_iXButtonHovering;			// 标识鼠标进入按钮区域

protected:
	HICON m_hIcon;
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void		OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void		OnSpinBtnHand( NMHDR *pNotify, LRESULT *pRes );
	afx_msg void		OnSpinBtnPoint( NMHDR *pNotify, LRESULT *pRes );
	afx_msg void		OnSpinBtnBack( NMHDR *pNotify, LRESULT *pRes );
	afx_msg void	OnBnCheckAllowPoint();
	afx_msg void	OnBnCheckAllowBack();
	afx_msg void	OnBnCheckBeforeOrder();
	afx_msg void	OnBtnMax();
	afx_msg void	OnBtnDropCommInfo();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSetFocusEditHand();
	afx_msg void	OnEnKillFocusEditHand();
	afx_msg void	OnBnCheckStopLoss();
	afx_msg void	OnBnCheckStopProfit();
	afx_msg void	OnSpinBtnStopLoss( NMHDR *pNotify, LRESULT *pRes );
	afx_msg void	OnSpinBtnStopProfit( NMHDR *pNotify, LRESULT *pRes );
	afx_msg void	OnEnChangeEditPoint();
	afx_msg void	OnEnChangeEditStopLoss1();
	afx_msg void	OnEnChangeEditStopProfit1();
	afx_msg void	OnEnKillFocusEditStopLoss1();
	afx_msg void	OnEnKillFocusEditStopProfit1();
	afx_msg void	OnSetFocusEditPoint();
	afx_msg void	OnSetFocusEditBack();
	afx_msg void	OnSetFocusPrice();
	afx_msg void	OnSetFocusEditStopLoss2();
	afx_msg void	OnSetFocusEditStopProfit2();
	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);
	// Generated message map functions
	//{{AFX_MSG(CTrade)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRADE_H__57B8CCC7_22B9_46E2_8CDD_37DA8F62F230__INCLUDED_)
