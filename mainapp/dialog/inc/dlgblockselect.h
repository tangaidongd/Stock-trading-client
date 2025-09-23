#if !defined(AFX_DLGBLOCKSELECT_H__7B7D5F42_B820_4B77_A7C1_840B2CF17B6D__INCLUDED_)
#define AFX_DLGBLOCKSELECT_H__7B7D5F42_B820_4B77_A7C1_840B2CF17B6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgblockselect.h : header file
//

//#include "MerchBlocks.h"
#include "IoViewReport.h"
#include "propertysheetex.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgBlockSelect dialog
class CBlockSelect;

class CDlgBlockSelect : public CPropertyPage
{
// Construction
public:
	CDlgBlockSelect();   // standard constructor

	void			UpdateUserBlock();
	void			UpdateLogicBlock();		// 废弃
	void			UpdateSysBlock();
	void			UpdateServerLogicBlock();	// 现有的依据服务器传递的逻辑板块

	// Dialog Data
	//{{AFX_DATA(CDlgBlockSelect)
	enum { IDD = IDD_DIALOG_BLOCK_SEL };
	CTreeCtrl	m_Tree;
	//}}AFX_DATA

	CImageList			m_ImageList;
	CBlockSelect*		m_pParent;
	
	int32 m_iType;		//同T_BlockDesc.iType

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBlockSelect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnSetActive();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgBlockSelect)
	afx_msg void OnSelchangedTreeBlock(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CBlockSelect : public CPropertySheetEX
{
	DECLARE_DYNAMIC(CBlockSelect)

// Construction
public:
	CBlockSelect(T_BlockDesc BlockDesc, LPCTSTR pszCaption = _T(""), CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	CDlgBlockSelect m_DialogSys;
	CDlgBlockSelect m_DialogLogic;
	CDlgBlockSelect m_DialogUser;
	
	T_BlockDesc		m_BlockDesc;
// Operations
public:
	void AddPages();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSheet1)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBlockSelect();
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	// Generated message map functions
protected:
	//{{AFX_MSG(CSheet1)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBLOCKSELECT_H__7B7D5F42_B820_4B77_A7C1_840B2CF17B6D__INCLUDED_)
