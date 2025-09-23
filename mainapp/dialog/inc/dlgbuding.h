#if !defined(AFX_DLGBUDING_H__C65DA70D_5C64_462A_A49B_C4D1E6BF3741__INCLUDED_)
#define AFX_DLGBUDING_H__C65DA70D_5C64_462A_A49B_C4D1E6BF3741__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgbuding.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgbuding dialog
class CDlgTrace;

class CDlgbuding : public CDialog
{
public:
	CDlgTrace* m_pDlgTrace;
// Construction
public:
	CDlgbuding(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgbuding)
	enum { IDD = IDD_DIALOG_BUDING };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgbuding)
	protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgbuding)
		// NOTE: the ClassWizard will add member functions here
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBUDING_H__C65DA70D_5C64_462A_A49B_C4D1E6BF3741__INCLUDED_)
