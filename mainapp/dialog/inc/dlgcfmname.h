#if !defined(AFX_DLGCFMNAME_H__E6E02034_ACFE_4D64_8374_4317C3A800DC__INCLUDED_)
#define AFX_DLGCFMNAME_H__E6E02034_ACFE_4D64_8374_4317C3A800DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgcfmname.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgCfmName dialog

#include "listboxex.h"
#include "DialogEx.h"

struct T_CfmFileInfo;

class CDlgCfmName : public CDialogEx
{
// Construction
public:
	CDlgCfmName(CWnd* pParent = NULL);   // standard constructor

	enum E_DoneAction		// 做的操作 - 并
	{
		DoneNone = 0,				// 无任何操作
		DoneAdd = 1,				// 添加了新的项
		DoneDelete = 2,				// 删除了项
	};

public:

	DWORD			m_dwDoneAction;		// 该对话框曾进行了什么操作
	

protected:
	bool32	GetSelectCfm(OUT T_CfmFileInfo &cfm);

	void	LoadCfmList();
	
public:
// Dialog Data
	//{{AFX_DATA(CDlgCfmName)
	enum { IDD = IDD_DIALOG_WSPNAME };
	CRichEditCtrl	m_edit;
	CListBoxEx	m_ListExist;
	CString	m_StrName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCfmName)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgCfmName)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListExist();	
	afx_msg void OnButtonDel();
	afx_msg void OnSelchangeListShow();
	virtual void OnOK();	
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCFMNAME_H__E6E02034_ACFE_4D64_8374_4317C3A800DC__INCLUDED_)
