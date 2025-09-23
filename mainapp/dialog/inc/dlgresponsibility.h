#if !defined(AFX_DLGRESPONSIBILITY_H__B6D6F43A_5470_43CB_BFC0_4FBF6C9C5433__INCLUDED_)
#define AFX_DLGRESPONSIBILITY_H__B6D6F43A_5470_43CB_BFC0_4FBF6C9C5433__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgresponsibility.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgResponsibility dialog
#include "DialogEx.h"
#include "OleRichEditCtrl.h"
#include "facescheme.h"

class CDlgResponsibility : public CDialogEx
{

// Construction
public:
	CDlgResponsibility(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgResponsibility)
	enum { IDD = IDD_DIALOG_RESPONSIBILIYT };
	//CString	m_StrResponsibility;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgResponsibility)
	public:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
private:
	COleRichEditCtrl	m_RichEdit;

protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgResponsibility)
		// NOTE: the ClassWizard will add member functions here
		afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGRESPONSIBILITY_H__B6D6F43A_5470_43CB_BFC0_4FBF6C9C5433__INCLUDED_)
