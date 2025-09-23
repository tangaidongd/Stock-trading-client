#if !defined(AFX_DLGWAIT_H__43B85636_28F1_42FE_A0F4_B7C7EA7E3084__INCLUDED_)
#define AFX_DLGWAIT_H__43B85636_28F1_42FE_A0F4_B7C7EA7E3084__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgwait.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgWait dialog

class CDlgWait : public CDialog
{
// Construction
public:
	CDlgWait(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgWait)
	enum { IDD = IDD_DIALOG_WAIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

public:
	void		DrawShowText();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgWait)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgWait)
	afx_msg void OnPaint();
	afx_msg void OnClose();	
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGWAIT_H__43B85636_28F1_42FE_A0F4_B7C7EA7E3084__INCLUDED_)
