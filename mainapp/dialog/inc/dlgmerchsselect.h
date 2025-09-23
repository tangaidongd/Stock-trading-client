#if !defined(AFX_DLGMERCHSSELECT_H__FEDF7EB8_4D9D_4CED_B4F3_E48194BEC9C7__INCLUDED_)
#define AFX_DLGMERCHSSELECT_H__FEDF7EB8_4D9D_4CED_B4F3_E48194BEC9C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgmerchsselect.h : header file
//
#include "DialogEx.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgMerchsSelect dialog

class CDlgMerchsSelect : public CDialogEx
{
// Construction
public:
	CDlgMerchsSelect(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgMerchsSelect)
	enum { IDD = IDD_DIALOG_MERCH_SEL };
	CTreeCtrl	m_Tree;
	CListCtrl	m_List;
	//}}AFX_DATA

	CImageList	 m_ImageList;
	CStringArray m_SelectNames;
	CStringArray m_SelectCodes;
	CArray<int32,int32> m_SelectIds;

private:
	CStringArray m_MerchNames;
	CStringArray m_MerchCodes;
	CArray<int32,int32> m_MerchIds;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMerchsSelect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgMerchsSelect)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedTreeBlock(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMERCHSSELECT_H__FEDF7EB8_4D9D_4CED_B4F3_E48194BEC9C7__INCLUDED_)
