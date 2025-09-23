#if !defined(AFX_DLGTABNAME_H__67703BFF_6C21_4E4E_A5B3_9A107B4E6605__INCLUDED_)
#define AFX_DLGTABNAME_H__67703BFF_6C21_4E4E_A5B3_9A107B4E6605__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgtabname.h : header file
//

#include "DialogEx.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgTabName dialog

class CDlgTabName : public CDialogEx
{

public:
	CString GetName();
	void    SetName(CString StrName);
// Construction
public:
	CDlgTabName(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgTabName)
	enum { IDD = IDD_DIALOG_TABNAME };
	CString	m_StrName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTabName)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTabName)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTABNAME_H__67703BFF_6C21_4E4E_A5B3_9A107B4E6605__INCLUDED_)
