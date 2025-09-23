#ifndef _DlgChangePwd_H_
#define _DlgChangePwd_H_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTradeQuery.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgTradeQuery dialog
#include "DialogEx.h"
#include "DlgStatic.h"
#include "NCButton.h"
#include "TradeQueryInterface.h"
#include "TradeLoginInterface.h"
#include "ComboBoxCheck.h"

#include <vector>
using std::vector;
#include <set>
using std::set;
#include <map>
using std::map;

#define  WM_MESSAGE_PARENT		WM_USER+1000


class CDlgChangePwd : public CDialog, public iTradeBidNotify
{

public:
	CDlgChangePwd(CWnd* pParent = NULL);   // standard constructor
	~CDlgChangePwd();
	
public:
	virtual void	OnLoginStatusChanged(int iCurStatus, int iOldStatus){};
	virtual bool32	OnInitQuery(CString &strTipMsg){ return true;};
	virtual bool32	OnQueryUserInfo(CString &strTipMsg){ return true;};
	virtual void	OnInitCommInfo(){};

	virtual void	OnQueryHoldDetailResponse(){};
	virtual void	OnQueryHoldSummaryResponse() {};
//	virtual void	OnQueryEntrustResponse();
	virtual void	OnQueryLimitEntrustResponse(){};
	virtual void	OnQueryDealResponse(){};
	virtual void	OnQueryCommInfoResponse(){};
	virtual void	OnQueryTraderIDResponse(){};
	virtual void	OnQueryUserInfoResponse();
	virtual void	OnQueryQuotationResponse(){};
	virtual void	OnDisConnectResponse(){};

	virtual void	OnReqEntrustResponse(bool32 bShowTip=true){};
	virtual void	OnReqCancelEntrustResponse(){};
	virtual void	OnReqSetStopLPResponse(){};
	virtual void	OnReqCancelStopLPResponse(){};
	virtual void    OnReqModifyPwdResponse();


public:
	int				Create(CWnd *pParent) { return CDialog::Create(IDD, pParent); }
	void			SetTradeBid(iTradeBid *pTradeBid);

public:
	T_TradeLoginInfo	*m_pTradeLoginInfo;
	
private:
	void			RecalcLayout(bool bNeedDraw);
	void			CreateControl();


private:
	// Dialog Data
	//{{AFX_DATA(CDlgChangePwd)
	enum { IDD = IDD_DIALOG_CHANGE_PASSWORD };
	//CDlgStatic		&m_StaticAccount;
	//}}AFX_DATA

private:
	//ÃÜÂëÐÞ¸Ä
	CStatic m_StaticPwdType;
	CComboBoxCheck m_DropPwdType;
	CStatic m_StaticOldPwd;
	CStatic m_StaticOldPwdBK;
	CEdit	m_editOldPwd;
	CStatic m_StaticNewPwd;
	CStatic m_StaticNewPwdBK;
	CEdit	m_editNewPwd;
	CStatic m_StaticCheckPwd;
	CStatic m_StaticCheckPwdBK;
	CEdit	m_editCheckPwd;

	CNCButton	m_btnCommit;

	iTradeBid			*m_pTradeBid;
	CClientRespUserInfo		m_QueryUserInfoResult;
	
	CFont	m_pFont;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgChangePwd)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual	BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgTradeQuery)
	afx_msg BOOL		OnEraseBkgnd(CDC* pDC);
	afx_msg void		OnSize(UINT nType, int cx, int cy);
	afx_msg void		OnPaint();
	afx_msg HBRUSH		OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void		OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void		OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void		OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void		OnBtnCommit();

	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!_DlgChangePwd_H_