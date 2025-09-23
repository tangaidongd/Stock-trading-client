#if !defined(AFX_DLGADDTOBLOCK_H_)
#define AFX_DLGADDTOBLOCK_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgaddtoblock.h : header file
//

#include "DialogEx.h"

#include "blockManager.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgAddToBlock dialog

// 添加到自选板块对话框

class CDlgAddToBlock : public CDialogEx
{
public:
	
	static bool32 GetUserSelBlock(OUT T_Block &block); // 弹出对话框 编辑指定note
	
private:
	
	// Construction
protected:
	CDlgAddToBlock(CWnd* pParent = NULL);   // standard constructor

	void	EnableDlgItem(UINT uDlgItemId, BOOL bEnable);
	int		GetCurSelBlock(OUT T_Block *pBlock = NULL);
	void	SelectBlock(const CString &StrBlockName);

	void	UpdateBtnStatus();

	void	FillList();		// 填充列表

	CImageList		m_ImageList;
	T_Block			m_block;
	
	// Dialog Data
	//{{AFX_DATA(CDlgAddToBlock)
	enum { IDD = IDD_DIALOG_ADDTOBLOCK };
	CListCtrl	  m_List;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAddToBlock)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgAddToBlock)
	afx_msg void OnListDBLClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBtnBlockNew();
	afx_msg void OnBtnBlockModify();
	afx_msg void OnBtnBlockDel();
	afx_msg void OnListSelChange(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGADDTOBLOCK_H_)