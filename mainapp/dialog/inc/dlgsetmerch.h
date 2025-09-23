#if !defined(AFX_DLGSETMERCH_H__)
#define AFX_DLGSETMERCH_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgsetmerch.h : header file
//
#include "ReportScheme.h"
#include "DialogEx.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgSetMerch dialog

class CDlgSetMerch : public CDialog
{
// Construction
public:
	CDlgSetMerch(CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CDlgSetMerch)
	enum { IDD = IDD_DIALOG_MERCH };
	CSpinButtonCtrl	m_SpinNum;
	CSpinButtonCtrl	m_SpinWidth;
	CListBox	m_CtrlListAll;
	CListBox	m_CtrlListExist;
	CString	m_StrName;
	UINT	m_uiWidth;
	UINT	m_uiNum;
	//}}AFX_DATA
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSetMerch)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
public:
	void		Show(E_ReportType  eReportType = ERTFuturesCn);
	bool32		BeModify();
	void		GetOwnHeadInfo(CArray<T_HeadInfo,T_HeadInfo>&	aHeadInfoList,int32 iFixCol);
public:
	CString		 m_StrTitle;

	int32		 m_iFixCol;
	CArray<T_HeadInfo,T_HeadInfo>	m_aHeadInfoList;
	
	int32		 m_iFixColBk;
	CArray<T_HeadInfo,T_HeadInfo>	m_aHeadInfoListBk;	
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgSetMerch)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDel();
	afx_msg void OnButtonUp();
	afx_msg void OnButtonDown();
	afx_msg void OnDeltaposSpinWidth(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeListExist();
	afx_msg void OnChangeEditWidth();
	afx_msg void OnChangeEditNum();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSETMERCH_H__)
