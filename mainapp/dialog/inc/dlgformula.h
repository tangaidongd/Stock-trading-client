#if !defined(AFX_DLGFORMULA_H__F9EBFB6A_57FB_46B7_850F_5C718AC1BFA0__INCLUDED_)
#define AFX_DLGFORMULA_H__F9EBFB6A_57FB_46B7_850F_5C718AC1BFA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgformula.h : header file
//

#include "CFormularContent.h"
#include "propertysheetex.h"
class CFormulaSelect;
/////////////////////////////////////////////////////////////////////////////
// CDlgFormula dialog
class CDlgFormula : public CPropertyPage
{
	DECLARE_DYNCREATE(CDlgFormula)

// Construction
public:
	CDlgFormula();
	~CDlgFormula();

	CFormulaSelect* m_pMainDlg;
	CImageList	m_ImageList;
	CFormulaLib* m_pLib;
	void	BuildTree();
	void	BuildGroup();//0
	void	BuildOften();//1
	void	BuildUser();//2
	void	UpdateButton ( HTREEITEM hItem);
	bool32	SameCutName ( CString Str1,CString Str2 );
	CString m_StrSelectName;
	HTREEITEM m_hSelectItem;
// Dialog Data
	//{{AFX_DATA(CDlgFormula)
	enum { IDD = IDD_DIALOG_FORMULA };
	CTreeCtrl	m_Tree;
	int32		m_iType;//0:分组;1:常用;2:用户自编
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDlgFormula)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgFormula)
	afx_msg void OnSelchangedTreeBlock(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNew();
	afx_msg void OnDel();
	afx_msg void OnChg();
	afx_msg void OnOften();
	afx_msg void OnImport();
	afx_msg void OnExport();
	afx_msg void OnClose0();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
class CDlgFormula2 : public CPropertyPage
{
	DECLARE_DYNCREATE(CDlgFormula2)

// Construction
public:
	CDlgFormula2();
	~CDlgFormula2();

	CFormulaSelect* m_pMainDlg;
	CImageList	m_ImageList;
	CFormulaLib* m_pLib;
	void	BuildList();
	void	UpdateButton ( int32 iItem );
	bool32	SameCutName ( CString Str1,CString Str2 );
	CString m_StrSelectName;
	int32	m_iSelectItem;

	int32	m_iSortDir;
	int32	m_iSortCol;
	
// Dialog Data
	//{{AFX_DATA(CDlgFormula)
	enum { IDD = IDD_DIALOG_FORMULA2 };
	CListCtrl	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDlgFormula)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgFormula)
//	afx_msg void OnSelchangedTreeBlock(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNew();
	afx_msg void OnDel();
	afx_msg void OnChg();
	afx_msg void OnOften();
	afx_msg void OnMySet();
	afx_msg void OnImport();
	afx_msg void OnExport();
	afx_msg void OnClose0();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

class CFormulaSelect : public CPropertySheetEX
{
	DECLARE_DYNAMIC(CFormulaSelect)

// Construction
public:
//	CBlockSelect(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CFormulaSelect(LPCTSTR pszCaption = _T(""), CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	CDlgFormula2 m_FormulaList;
	CDlgFormula  m_FormulaGroup;
	CDlgFormula  m_FormulaOften;
	CDlgFormula  m_FormulaUser;

	void ReloadXml();
// Operations
public:
	void AddPages();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSheet1)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFormulaSelect();
	virtual BOOL OnInitDialog();
	// Generated message map functions
protected:
	//{{AFX_MSG(CSheet1)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFORMULA_H__F9EBFB6A_57FB_46B7_850F_5C718AC1BFA0__INCLUDED_)
