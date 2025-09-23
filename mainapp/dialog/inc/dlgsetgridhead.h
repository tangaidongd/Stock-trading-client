#if !defined(AFX_DLGSETGRIDHEAD_H__)
#define AFX_DLGSETGRIDHEAD_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgsetgridhead.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSetGridHead dialog
#include "tabsheet.h"
#include "dlgsetmerch.h"
#include "ReportScheme.h"
#include "DialogEx.h"

class CDlgSetGridHead;

class CTabSetGridHeadCallback : public CTabDialogPreCreateCallback
{
public:
	CDlgSetGridHead* m_pParent;
	void PreCreate(CString StrTitle,int32 i);
};
class CDlgSetGridHead: public CDialogEx
{
public:
	int32		   m_iTabSelLast;
	CDlgSetMerch   m_DlgSetMerch[ERTCount];
	
	CArray<T_HeadInfo,T_HeadInfo>	m_aHeadInfoList;
	CTabSetGridHeadCallback			m_Callback;
	// Construction	
public:
	CDlgSetGridHead(CWnd* pParent = NULL);   // standard constructor
	// Dialog Data
	//{{AFX_DATA(CDlgSetGridHead)
	enum { IDD = IDD_DIALOG_GRIDHEAD };
	CTabSheet	m_TabSheet;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSetGridHead)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgSetGridHead)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTabGridhead(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonConfirm();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSETGRIDHEAD_H__
