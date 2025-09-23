// DlgWelcome.h : header file
//

#if !defined(AFX_234DLG_H__32304215_15E0_4784_85DC_5626320532D4__INCLUDED_)
#define AFX_234DLG_H__32304215_15E0_4784_85DC_5626320532D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "DialogEx.h"
#include "WndCef.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgWelcome dialog

class CDlgWelcome : public CDialogEx
{
// Construction
public:
	CDlgWelcome(CWnd* pParent = NULL);	// standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlgWelcome)
	enum { IDD = IDD_DIALOG_WELCOME };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgWelcome)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	HICON m_hIcon;
	
	// Generated message map functions
	//{{AFX_MSG(CDlgWelcome)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnPaint();
	virtual void PostNcDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CWndCef		m_wndCef;
	bool32		m_bShowUrl;
	Image      *m_pImgWelcom;
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_234DLG_H__32304215_15E0_4784_85DC_5626320532D4__INCLUDED_)
