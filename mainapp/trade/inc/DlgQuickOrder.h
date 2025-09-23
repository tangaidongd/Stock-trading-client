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
	void SetOpen(BOOL bOpen);// bOpenΪtrue��ʾ���֣�Ϊfalse��ƽ��
	void EnableTradeButton(BOOL bEnable);
	


public:
	BOOL m_bOpen;						// ��¼�ǽ��ֻ���ƽ�֣�TRUE:����   FALSE:ƽ�֣�

	COptionButton m_btnOpen;			// ����
	COptionButton m_btnClose;			// ƽ��

	CButtonSTMain m_btnTitleExit;		// �رհ�ť

	CNCButton m_btnTitleBK;				// ����������
	CNCButton m_btnIcon;				// ���Ͻ�ͼ�갴ť
	CNCButton m_btnTitleText;			// �������ı�
	CNCButton m_btnSub;					// ��
	CNCButton m_btnAdd;					// ��
	CNCButton m_btnLeft;				// ���װ�ť
	CNCButton m_btnRight;				// �ҽ��װ�ť
	CNCButton m_btnGoodsNameTip;
	CStaticEx m_btnGoodsName;
	CNCButton m_btnHandTip;				// ����
	CCheckBox m_btnCheckbox;
	//CNCButton m_btnCheckbox;
	CNCButton m_btnEditBK;
	CEdit m_editHand;					// ����

	CStaticEx m_staticHandingEmpty;		// �ֿ�����
	CStaticEx m_staticHandingMany;		// �ֶ�����

	Image *m_imgTitleBK;				// ����ͼ
	Image *m_imgTitleIcon;				// �������Ͻ�ͼ��
	Image *m_imgTitleText;				// �ı�����ɫ
	Image *m_imgTitleExit;				// �رհ�ťͼ��
	Image *m_imgStatic;
	Image *m_imgSub;					// ����ťͼ
	Image *m_imgAdd;					// �Ӱ�ťͼ
	Image *m_imgLeftbtn;				// ���װ�ťͼ
	Image *m_imgRightbtn;				// �ҽ��װ�ťͼ

	CString m_strCommID;// �洢�ҵ�����ƷID
	BOOL m_bTradeComm;// �Ƿ��ǽ�����Ʒ
	CFont m_font;

	CFont m_fontEdit;

	int m_iHand;
	int m_iHandMax;// ���ί����
	int m_iHandMin;// ��Сί����
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

	CBitmap m_bmpTitleIcon;// ���Ͻ�ͼ��
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

	double m_dCurrentMargin;	//�����ñ�֤��
	double m_dMarginMarket;		//���豣֤��(�м�)
	int	m_iHodingEmptyMax;		// �ֿ��������
	int	m_iHodingManyMax;		// �ֶ��������

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
	void ReadCheck();// ���ļ��ж�ȡ���ֺ�ƽ��checkbox��״̬
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
