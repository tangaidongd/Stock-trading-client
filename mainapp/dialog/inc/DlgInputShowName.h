#ifndef _DLGINPUTSHOWNAME_H_
#define _DLGINPUTSHOWNAME_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgindexprompt.h : header file
//

#include "DialogEx.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgInputShowName dialog

class CDlgInputShowName : public CDialogEx
{
	DECLARE_DYNCREATE(CDlgInputShowName)
public:
	static int GetInputString(INOUT CString &StrInput, const CString &StrTip = _T(""), const CString &StrTitle = _T(""));
	// Construction
public:
	CDlgInputShowName(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlgInputShowName)
	enum { IDD = IDD_DIALOG_INPUT_SHOWSTRING };
	CString	m_StrInput;
	CString m_StrTip;
	//}}AFX_DATA
	
private:
	CString	m_StrInputOrg;			// 是否有初始化的字串提示
	CString	m_StrTipOrg;
	CString m_StrTitleOrg;
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgInputShowName)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgInputShowName)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_DLGINPUTSHOWNAME_H_