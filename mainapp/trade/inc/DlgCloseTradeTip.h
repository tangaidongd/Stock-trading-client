#ifndef _DLGCLOSETRADETIP_H_
#define _DLGCLOSETRADETIP_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CDlgTip.h : header file
//
#include "TradeQueryInterface.h"

#include "BeautifulStatic.h"
#include "GdiPlusTS.h"
#include "NCButton.h"
#include "BtnST.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgTip dialog

class CDlgCloseTradeTip : public CDialog
{
// Construction
public:
	CDlgCloseTradeTip(CWnd* pParent = NULL);   // standard constructor
	~CDlgCloseTradeTip();

// Dialog Data
	//{{AFX_DATA(CTrade)
	enum { IDD = IDD_DIALOG_CLOSETRADETIP };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
public:
	E_TipType m_eTipType;
	CString m_strTipMsg;
	CStatic m_CtrlPic;

	CBeautifulStatic m_staticTip;
	CWnd *m_pCenterWnd;
	CNCButton	m_buttonOk;
	CNCButton	m_buttonCancel;
	CButtonSTMain	*m_pBtnCloseWnd;		//¹Ø±Õ
private:
	void AdjustWindSize();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrade)
protected:
	HICON m_hIcon;
	HICON m_hIconDlg;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CTrade)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRADE_H__57B8CCC7_22B9_46E2_8CDD_37DA8F62F230__INCLUDED_)
