// #if !defined(AFX_TRADE_H__57B8CCC7_22B9_46E2_8CDD_37DA8F62F230__INCLUDED_)
// #define AFX_TRADE_H__57B8CCC7_22B9_46E2_8CDD_37DA8F62F230__INCLUDED_
#ifndef _DLGQUICKORDER_H_
#define _DLGQUICKORDER_H_

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
#include "resource.h"
#include "DlgTradeQuery.h"
#include "NCButton.h"
#include "BtnST.h"
#include "OptionButton.h"
#include "CheckBox.h"
#include "StaticEx.h"
#include "NCButton.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgTradeOpen dialog

class CDlgQuickOrder : public CDialog, public iTradeBidNotify
{
// Construction
public:
	CDlgQuickOrder(CWnd* pParent = NULL);   // standard constructor
	~CDlgQuickOrder();

// Dialog Data
	//{{AFX_DATA(CTrade)
	enum { IDD = IDD_DIALOG_QUICKORDER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrade)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
	
public:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
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

	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	void SetWindowSize(int nLeft = 0, int nTop=0 ,int nWidth = 220, int nHeight = 180);
	BOOL SetBkTitleBitmap(int nResourceID);
	BOOL SetTitleIconRes(int nRecourceID);
	BOOL SetTitleCloseRes(int nRecourceID);
	void SetOpen(BOOL bOpen);// bOpen为true表示建仓，为false表平仓
	void EnableTradeButton(BOOL bEnable);
	


public:
	BOOL m_bOpen;						// 记录是建仓还是平仓（TRUE:建仓   FALSE:平仓）

	COptionButton m_btnOpen;			// 开仓
	COptionButton m_btnClose;			// 平仓

	CButtonSTMain m_btnTitleExit;		// 关闭按钮

	CNCButton m_btnTitleBK;				// 标题栏背景
	CNCButton m_btnIcon;				// 左上角图标按钮
	CNCButton m_btnTitleText;			// 标题栏文本
	CNCButton m_btnSub;					// 减
	CNCButton m_btnAdd;					// 加
	CNCButton m_btnLeft;				// 左交易按钮
	CNCButton m_btnRight;				// 右交易按钮
	CNCButton m_btnGoodsNameTip;
	CStaticEx m_btnGoodsName;
	CNCButton m_btnHandTip;				// 手数
	CCheckBox m_btnCheckbox;
	//CNCButton m_btnCheckbox;
	CNCButton m_btnEditBK;
	CEdit m_editHand;					// 手数

	CStaticEx m_staticHandingEmpty;		// 持空手数
	CStaticEx m_staticHandingMany;		// 持多手数

	Image *m_imgTitleBK;				// 背景图
	Image *m_imgTitleIcon;				// 窗口左上角图标
	Image *m_imgTitleText;				// 文本背景色
	Image *m_imgTitleExit;				// 关闭按钮图标
	Image *m_imgStatic;
	Image *m_imgSub;					// 减按钮图
	Image *m_imgAdd;					// 加按钮图
	Image *m_imgLeftbtn;				// 左交易按钮图
	Image *m_imgRightbtn;				// 右交易按钮图

	CString m_strCommID;// 存储找到的商品ID
	BOOL m_bTradeComm;// 是否是交易商品
	CFont m_font;

	CFont m_fontEdit;

	int m_iHand;
	int m_iHandMax;// 最大委托量
	int m_iHandMin;// 最小委托量
	int m_iPointDef;
	double m_dPriceStep;
	int m_iPointCount;

	CFont m_fontButtonSmall;
	CFont m_fontButtonBig;
	CFont m_fontEditHand;

	string m_strBuyPrice;
	string m_strSellPrice;
	string m_strFirmID;
	string m_strQty;
	string m_strPoint;

	CBitmap m_bmpbkTitle;
	BITMAP m_bmpbkInfo;

	CBitmap m_bmpTitleIcon;// 左上角图标
	BITMAP m_bmpIconInfo;

	CBitmap m_bmpTitleClose;
	BITMAP m_bmpIconCloseInfo;

	BOOL m_bOpenCheck;
	BOOL m_bCloseCheck;

	QueryTraderIDResultVector	m_QueryTraderIDVector;
	
private:
	enum E_QuickEntrustType
	{
		EET_NONE = 0,

		EET_OPEN_BUY,
		EET_OPEN_SELL,
		EET_CLOSE_BUY,
		EET_CLOSE_SELL,

		EET_END
	};

	double m_dCurrentMargin;	//可以用保证金
	double m_dMarginMarket;		//所需保证金(市价)
	int	m_iHodingEmptyMax;		// 持空最大手数
	int	m_iHodingManyMax;		// 持多最大手数

	HBRUSH m_BrushStaticBack;

public:
	CDlgTradeQuery *m_pTradeQuery;
	T_TradeLoginInfo *m_pTradeLoginInfo;
	iTradeBid *m_pTradeBid;
	char m_chCommID[64];
	QueryQuotationResultVector m_QuotationResultVector;
	QueryCommInfoResultVector m_QueryCommInfoResultVector;
	CClientRespUserInfo		m_QueryUserInfoResult;
	QueryHoldSummaryResultVector	m_QueryHoldSummaryVector;
	
public:
	int	Create(CWnd *pParent) { return CDialog::Create(IDD, pParent); }
	void InitControl();
	void AdjustControlPos();
	void SetVisiableHoding(BOOL bOpen);
	void BuySell(E_QuickEntrustType eEntrustType);
	void SetTradeBid( iTradeBid *pTradeBid );
	bool SetCommID(CString strCommID);
	void SetPrice();
	int GetPointCount(const double dSpread);
	string DoubleToString(double d);
	CString GetCommName(const char* pCommID);
	int WarningMsg(CString &strWarn, E_QuickEntrustType eType);
	CString GetTradeTipMsg(E_QuickEntrustType entrustType);
	void AdjustControlStatus(BOOL bFindMany,BOOL bFindEmpty);
	void ReadCheck();// 从文件中读取开仓和平仓checkbox的状态
protected:
	HICON m_hIcon;
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void		OnBtnSub();
	afx_msg void		OnBtnAdd();
	afx_msg void		OnBtnBuy();
	afx_msg void		OnBtnSell();
	afx_msg void OnEnChangeEditPoint();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg	void		OnBtnClose();
	afx_msg void OnCheckBoxClicked();
	afx_msg void		OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnTabExchange(WPARAM wParam, LPARAM lParam);
	// Generated message map functions
	//{{AFX_MSG(CTrade)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRADE_H__57B8CCC7_22B9_46E2_8CDD_37DA8F62F230__INCLUDED_)
