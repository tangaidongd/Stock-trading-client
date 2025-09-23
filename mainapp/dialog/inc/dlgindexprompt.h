#if !defined(_DLGINDEXPROMPT_H_)
#define _DLGINDEXPROMPT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgindexprompt.h : header file
//
#include "DialogEx.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgIndexPrompt dialog

class CDlgIndexPrompt : public CDialogEx
{
public:

	void  ShowMessage(const CString& StrShow);
// Construction
public:
	CDlgIndexPrompt(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgIndexPrompt)
	enum { IDD = IDD_DIALOG_INDEX_PROMPT };
	CString	m_StrShow;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgIndexPrompt)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgIndexPrompt)
	afx_msg void OnClose();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_DLGINDEXPROMPT_H_)
