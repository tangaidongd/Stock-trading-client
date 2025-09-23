#if !defined(AFX_DLGMARKMANAGER_H_)
#define AFX_DLGMARKMANAGER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgnotepad.h : header file
//

#include "DialogEx.h"

#include "MarkManager.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgMarkManager dialog

// 标记文字对话框

class CDlgMarkManager : public CDialogEx
{
public:
	typedef CArray<CMerch *, CMerch *> MerchArray;
	CDlgMarkManager(CWnd* pParent = NULL);   // standard constructor
	
private:
	
	// Construction
protected:
	
	void		PromptErrorInput(const CString &StrPrompt, CWnd *pWndFocus = NULL);

	void		AppendOrDelMarkType(E_MarkType eType, bool32 bApp = true);
	bool32		UpdateRow(CMerch *pMerch, T_MarkData *pMarkData = NULL); // 填充一行表格
	void		RemoveRow(CMerch *pMerch);
	CString		GetMarkTypeName(E_MarkType eType);

	int32		GetCheckedMerchs(OUT MerchArray &aMerchs);
	
	// Dialog Data
	//{{AFX_DATA(CDlgMarkManager)
	enum { IDD = IDD_DIALOG_MARK_MANAGER };
	CListCtrl	m_List;	// 列表框
	//}}AFX_DATA
	
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMarkManager)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgMarkManager)
	afx_msg void OnBtnChecked0();
	afx_msg void OnBtnChecked1();
	afx_msg void OnBtnChecked2();
	afx_msg void OnBtnChecked3();
	afx_msg void OnBtnChecked4();
	afx_msg void OnBtnChecked5();
	afx_msg void OnBtnChecked6();
	afx_msg void OnBtnAddToBlock();
	afx_msg void OnBtnModifyMark();
	afx_msg void OnBtnSelAll();
	afx_msg void OnBtnUnSelAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMARKMANAGER_H_)