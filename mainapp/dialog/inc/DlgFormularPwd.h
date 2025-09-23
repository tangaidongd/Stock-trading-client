#if !defined(AFX_DLGFORMULARPWD_H__BE7313D6_A294_40C0_B14C_82F11DC6475A__INCLUDED_)
#define AFX_DLGFORMULARPWD_H__BE7313D6_A294_40C0_B14C_82F11DC6475A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFormularPwd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgFormularPwd dialog
#include "DialogEx.h"

class CDlgFormularPwd : public CDialogEx
{
public:
	CString		GetPwd();
	CString		m_StrPwdMy;
// Construction
public:
	CDlgFormularPwd(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgFormularPwd)
	enum { IDD = IDD_DIALOG_FORMULAR_PWS };
	CString	m_StrPwd;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFormularPwd)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgFormularPwd)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFORMULARPWD_H__BE7313D6_A294_40C0_B14C_82F11DC6475A__INCLUDED_)
