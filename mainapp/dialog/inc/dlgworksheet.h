#if !defined(AFX_DLGWORKSHEET_H__)
#define AFX_DLGWORKSHEET_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgWorksheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgWorksheet dialog
#include "GGTongView.h"
#include "listboxex.h"
#include "DialogEx.h"


struct T_CfmFileInfo;

class CDlgOpenWorksheet : public CDialogEx
{
public:
// Construction 
public:
	CDlgOpenWorksheet(CWnd* pParent = NULL);   // standard constructor
	
public:
	CString	m_StrSelCfm;
protected:
	
	bool32	GetSelectCfm(OUT T_CfmFileInfo &cfm);

public:	

// Dialog Data
	//{{AFX_DATA(CDlgWorksheet)
	enum { IDD = IDD_DIALOG_WORKSPACE };
	CListBoxEx  	m_ListShow;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgWorksheet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgWorksheet)
	afx_msg void OnDblclkListShow();
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnPaint();
	afx_msg void OnButtonDefault();
	afx_msg void OnSelchangeListShow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGWORKSHEET_H__)
