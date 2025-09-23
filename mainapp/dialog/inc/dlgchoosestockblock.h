#if !defined(AFX_DLGCHOOSESTOCKBLOCK_H__)
#define AFX_DLGCHOOSESTOCKBLOCK_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgchoosestockblock.h : header file
//


#include "IoViewReport.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgChooseStockBlock dialog

class CDlgChooseStockBlock : public CDialogEx
{
private:
	void		FillListBox();

public:
	T_BlockDesc			m_BlockFinal;
private:
	int32				m_iUserBlockNums;
	CBindHotkey			m_BindHotkey;

// Construction
public:
	CDlgChooseStockBlock(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgChooseStockBlock)
	enum { IDD = IDD_DIALOG_BLOCK_CHOOSE_STOCK };
	CListBox	m_ListBlock;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgChooseStockBlock)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgChooseStockBlock)
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDel();
	afx_msg void OnClose();
	afx_msg void OnSelchangeList();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCHOOSESTOCKBLOCK_H__)
