#if !defined(_DLGCUSTOMCONDITION_H_)
#define _DLGCUSTOMCONDITION_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgcustomcondition.h : header file
//

#include "DialogEx.h"
#include "dlgcustomstock.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgCustomCondition dialog
class CDlgCustomCondition : public CDialogEx
{
public:
	// 设置初始化的参数
	void	SetParmas(bool32 bAdd, const T_CustomCdtCell& stParam, const CString& StrName, const CString& StrPost);

	// 取得结果
	void	GetResult(OUT T_CustomCdtCell& stParam);

private:
	// 是增加还是修改
	bool32	m_bAdd;

	// 参数
	T_CustomCdtCell m_stParma;

// Construction
public:
	CDlgCustomCondition(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgCustomCondition)
	enum { IDD = IDD_DIALOG_CUSTOM_CONDITION };
	int		m_iIndexCmp;
	float	m_fValue;
	CString	m_StrPost;
	CString	m_StrName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCustomCondition)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgCustomCondition)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_DLGCUSTOMCONDITION_H_)
