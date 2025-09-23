#if !defined(AFX_DLGEDITREPORT_H__CFD9136C_49FB_40BC_AD9F_06D48AF1C765__INCLUDED_)
#define AFX_DLGEDITREPORT_H__CFD9136C_49FB_40BC_AD9F_06D48AF1C765__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgEditReport.h : header file
//
#include "DialogEx.h"
#include "SelectStockStruct.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgEditReport dialog

class CDlgEditReport : public CDialogEx
{
// Construction
public:
	CDlgEditReport(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgEditReport)
	enum { IDD = IDD_DIALOG_EDIT_REPORT };
		// NOTE: the ClassWizard will add data members here
	CListBox	m_ListMerchs;
	//}}AFX_DATA
public:
	void InitialListMerchs();
	void SaveListMerchs();

	// 所有商品列表框
	void FillListBox();

	CArray<CMerch*, CMerch*> m_aMerch;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgEditReport)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgEditReport)
		// NOTE: the ClassWizard will add member functions here
		afx_msg void OnButtonAdd();
		afx_msg void OnButtonDel();
		afx_msg void OnButtonUp();
		afx_msg void OnButtonDown();
		afx_msg void OnButtonClear();
		afx_msg void OnButtonOK();
		afx_msg void OnButtonCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEDITREPORT_H__CFD9136C_49FB_40BC_AD9F_06D48AF1C765__INCLUDED_)
