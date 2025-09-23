#if !defined(AFX_DLGHOSTSET_H__D74E76CD_8047_4DB7_B1DA_77DDB70D0FDA__INCLUDED_)
#define AFX_DLGHOSTSET_H__D74E76CD_8047_4DB7_B1DA_77DDB70D0FDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlghostset.h : header file
//
#include "DialogEx.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgHostSet dialog

class CDlgHostSet : public CDialogEx
{
// Construction 
public:
	CDlgHostSet(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgHostSet)
	enum { IDD = IDD_DIALOG_HOSTSET };
	CString	m_StrHostAddress;
	CString	m_StrHostName;
	UINT	m_uiHostPort;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgHostSet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgHostSet)
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGHOSTSET_H__D74E76CD_8047_4DB7_B1DA_77DDB70D0FDA__INCLUDED_)
