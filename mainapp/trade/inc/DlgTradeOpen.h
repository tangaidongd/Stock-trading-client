// #if !defined(AFX_TRADE_H__57B8CCC7_22B9_46E2_8CDD_37DA8F62F230__INCLUDED_)
// #define AFX_TRADE_H__57B8CCC7_22B9_46E2_8CDD_37DA8F62F230__INCLUDED_
#ifndef _DLGTRADEOPEN_H_
#define _DLGTRADEOPEN_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CDlgTradeOpen.h : header file
//
#include "ExportMT2DispatchThread.h"
#include "TradeQueryInterface.h"
#include "TradeLoginInterface.h"
#include <deque>
#include "synch.h"
#include <string.h>

#include "DlgTradeQuery.h"
#include "NCButton.h"
#include "ComboBoxCheck.h"
#include "NewSpinButtonCtrl.h"
#include "GdiPlusTS.h"
#include "FloatEdit.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgTradeOpen dialog

class CDlgTradeOpen : public CDialog, public iTradeBidNotify
{
// Construction
public:
	CDlgTradeOpen(CWnd* pParent = NULL);   // standard constructor
	~CDlgTradeOpen();

// Dialog Data
	//{{AFX_DATA(CTrade)
	enum { IDD = IDD_DIALOG_OPEN };
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
//	void CreateControl(int newVersion);
	void InitControl();
	void ResetInitControl(CString strCommID,bool bBuy);
	void GetLimitPrice(double &upPrice, double &downPrice); // 限价的取值范围
	void SetPrice(bool32 bChangePrice=false);
	void SetRange();
	void RecalcLayout( bool bNeedDraw );
	void ShowHideControl_LimitPrice(int nCmdShow);
	void ShowHideControl_MarketPrice(int nCmdShow);
	bool FindOtherFirm( int iIndex, T_CommInfoType &commInfoType);
	bool FindCommInfoType( int iIndex, T_CommInfoType &commInfoType);
	int GetPointCount(const double dSpread);
	void SetTradeBid( iTradeBid *pTradeBid );
	void UpdataStopLossProfitShow();
	void UpdataMarginTip();
	int WarningMsg(CString &strWarn);
	void LoadTradeOpenInfo();
	void SaveTradeOpenInfo();
	int GetCommInfoTypeIndex(const char* pValue);

	int GetHaveHoldNum(); // 获得给定商品已有持仓数

	void TradeTypeChange(E_TradeType  tradeType);
	int OpenMaxVol(); // 建仓的最大委托量

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
	bool m_bInitTradeID;

	CComboBoxCheck m_DropCommInfo;		// 商品
	CStatic	m_StaticCommInfo;
	CComboBox m_DropHand;		// 手数
	CStatic	m_StaticHand;
	CEdit m_EditHand;
	CNewSpinButtonCtrl m_SpinHand;
	CStatic m_StaticRange;

	// 指价建仓控件
	CNCButton m_TradeOk;
	CNCButton m_RadioBuy;		// 买卖
	CNCButton m_RadioSell;
	CNCButton m_BtnMax;
	CStatic m_StaticBuySell;
	CEdit m_EditBuyPrice;
	CEdit m_EditSellPrice;
	CStatic m_StaticPrice;
	CFloatEdit m_EditPrice;
	CNewSpinButtonCtrl m_SpinPrice;
	CStatic m_StaticMarginTip;
	CNCButton m_CheckStopLoss;
	CStatic m_StaticCheckStopLoss;
	CFloatEdit m_EditStopLoss1;// 止损
	CEdit m_EditStopLoss2;// 止盈
	CNewSpinButtonCtrl m_SpinStopLoss;
	CStatic m_StaticStopLoss;
	CNCButton m_CheckStopProfit;
	CStatic m_StaticCheckStopProfit;
	CFloatEdit m_EditStopProfit1;
	CEdit m_EditStopProfit2;
	CNewSpinButtonCtrl m_SpinStopProfit;
	CStatic m_StaticStopProfit;
	// 市价建仓
	CEdit m_EditPriceMarketPrice;
	CStatic m_StaticMarginTipMarketPrice;
	CNCButton m_CheckAllowPoint;
	CStatic m_StaticCheckAllowPoint;
	CEdit m_EditPoint;
	CNewSpinButtonCtrl m_SpinPoint;
	CNCButton m_BuyOK;		// 买卖
	CNCButton m_SellOK;

	CStatic m_StaticAfterOk;	// 处理中
	CStatic	m_StaticBKHand;		// 手数编辑框的底色
	CStatic m_StaticBKPoint;
	CStatic m_StaticBKStopProfit1;
	CStatic m_StaticBKStopProfit2;
	CStatic m_StaticBKStopLoss1;
	CStatic m_StaticBKStopLoss2;
	CStatic m_StaticBKPrice;

	//下单前确认
	CNCButton m_CheckBeforeOrder;
	CStatic m_StaticCheckBeforeOrder;
	CStatic m_StaticTip;
	
	BOOL m_bCheckAllowPoint;
	BOOL m_bCheckBeforeOrder;
	BOOL m_bCheckStopLoss;
	BOOL m_bCheckStopProfit;
	E_EntrustType m_EntrustType;

	E_TradeType	  m_eTradeType;
// 
// 	deque<char*> m_DropInfo_CommInfo;
// 	deque<char*> m_DropInfo_TradeID;
// 	LockSingle m_LockDrop;
	_CommInfoType m_CommInfoType;
	LockSingle m_LockCommInfoType;
	_CommInfoType m_TradeID;
	LockSingle m_LockTradeID;
	T_CommInfoType m_CommInfo;
	T_CommInfoType m_OtherID;

	int m_iDefQtyMax;
	int m_iQty;
	int m_iQtyMax;
	int m_iQtyMin;
	int m_iPoint;
	int m_iPointMax;
	int m_iPointMin;
	int m_iPointDef;
	CString m_strEditHand;

//	T_EnrustParameters m_EnrustParameters;
	
	string m_strQty;
	string m_strPrice;
	string m_strPoint;
	string m_strStopLoss;
	string m_strStopProfit;
	string m_strBuySell;
	CString m_strBuyPrice;
	CString m_strSellPrice;

	double m_dPrice;	// 价格(指价)
	double m_dStopLoss1;	// 止损1
	double m_dStopProfit1;	// 止盈1
	double m_dPriceStep;	// 最小价格波动
	int m_iPointCount;		// 小数点保留几位
	
	T_TradeLoginInfo *m_pTradeLoginInfo;
	int m_iBuySellSpace;
	int m_iBuyRadioOffset;

	CClientRespUserInfo		m_QueryUserInfoResult;
	QueryCommInfoResultVector m_QueryCommInfoResultVector;
	QueryQuotationResultVector m_QuotationResultVector;
	QueryHoldDetailResultVector m_QueryHoldResultVector;
	QueryHoldSummaryResultVector m_QueryHoldSummaryResultVector;
	QueryTraderIDResultVector	m_QueryTraderIDVector;

	// 字体
	CFont m_fontSmall;
	CFont m_fontBig;
	CFont m_fontEditSmall;
	CFont m_fontEditBig;
	CFont m_fontEditMid;

	double m_dCurrentFund;		//当前资金
	double m_dCurrentMargin;	//可以用保证金
	double m_dMarginMarket;		//所需保证金(市价)
	double m_dMarginLimit;		//所需保证金(限价)

	int iMerchType;  //商品类型
	int m_maxHold;   //商品的最大持仓数

	Image *m_imgCheck;
	Image *m_imgUncheck;
	Image *m_pImgTradeMax;
	Image *m_pImgBuyOK;
	Image *m_pImgRadioBuy;
	Image *m_pImgSellOK;
	Image *m_pImgRadioSell;
// Implementation
protected:
	HICON m_hIcon;
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH	OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void	OnSpinBtnHand( NMHDR *pNotify, LRESULT *pRes );
	afx_msg void	OnSpinBtnPoint( NMHDR *pNotify, LRESULT *pRes );
	afx_msg void	OnSpinBtnStopLoss( NMHDR *pNotify, LRESULT *pRes );
	afx_msg void	OnSpinBtnStopProfit( NMHDR *pNotify, LRESULT *pRes );
	afx_msg void	OnSpinBtnPrice( NMHDR *pNotify, LRESULT *pRes );
	afx_msg void	OnBnCheckAllowPoint();
	afx_msg void	OnBnCheckBeforeOrder();
	afx_msg void	OnBnCheckStopLoss();
	afx_msg void	OnBnCheckStopProfit();
	afx_msg void	OnBnRadioBuy();
	afx_msg void	OnBnRadioSell();
	afx_msg void	OnBtnMax();
	afx_msg void	OnBtnOk();
	afx_msg void	OnBtnDropCommInfo();
//	afx_msg void	OnBtnDropEntrustType();
	afx_msg void OnTimer(UINT/*_PTR*/ nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void	OnEnChangeEditPrice();
	afx_msg void	OnEnChangeEditPoint();
	afx_msg void	OnEnChangeEditStopLoss1();
	afx_msg void	OnEnChangeEditStopProfit1();
	afx_msg void	OnEnKillFocusEditPrice();
	afx_msg void	OnEnKillFocusEditStopLoss1();
	afx_msg void	OnEnKillFocusEditStopProfit1();
	afx_msg void	OnSetFocusEditHand();
	afx_msg void	OnEnKillFocusEditHand();
	afx_msg void	OnSetFocusEditPoint();
	afx_msg void	OnSetFocusEditBuyPrice();
	afx_msg void	OnSetFocusEditSellPrice();
	afx_msg void	OnSetFocusEditStopLoss2();
	afx_msg void	OnSetFocusEditStopProfit2();
	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);
	// Generated message map functions
	//{{AFX_MSG(CTrade)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRADE_H__57B8CCC7_22B9_46E2_8CDD_37DA8F62F230__INCLUDED_)
