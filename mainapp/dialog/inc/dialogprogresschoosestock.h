#if !defined(AFX_DIALOGPROGRESSCHOOSESTOCK_H__)
#define AFX_DIALOGPROGRESSCHOOSESTOCK_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dialogprogresschoosestock.h : header file
//
#include "DialogEx.h"
/////////////////////////////////////////////////////////////////////////////
// CDialogProgressChooseStock dialog

class CDialogProgressChooseStock : public CDialogEx
{
private:
	//int32		m_iProgress;
// Construction
public:
	CDialogProgressChooseStock(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogProgressChooseStock)
	enum { IDD = IDD_DIALOG_PROGRESS_CHOOSE_STOCK };
	CProgressCtrl	m_Progress;
	CString	m_StrMsg;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogProgressChooseStock)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual	BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogProgressChooseStock)
		// NOTE: the ClassWizard will add member functions here
	afx_msg	LRESULT OnMsgStepProgress(WPARAM wParam , LPARAM lParam);
	//}}AFX_MSG	
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGPROGRESSCHOOSESTOCK_H__)
