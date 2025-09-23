#if !defined(AFX_DLGSYSTEMPARAMETER_H__BCC77889_9F0D_4ADF_8AE3_12D6910F4261__INCLUDED_)
#define AFX_DLGSYSTEMPARAMETER_H__BCC77889_9F0D_4ADF_8AE3_12D6910F4261__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgsystemparameter.h : header file
//
#include "DialogEx.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgSystemParameter dialog

class CDlgSystemParameter : public CDialogEx
{
// Construction
public:
	CDlgSystemParameter(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSystemParameter)
	enum { IDD = IDD_DIALOG_SYSPARAMETER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSystemParameter)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSystemParameter)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSYSTEMPARAMETER_H__BCC77889_9F0D_4ADF_8AE3_12D6910F4261__INCLUDED_)
