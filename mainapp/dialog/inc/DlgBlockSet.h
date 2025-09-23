#if !defined(AFX_DLGBLOCKSET_H__)
#define AFX_DLGBLOCKSET_H__
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBlockSet.h : header file
//
#include "DialogEx.h"
#include "staticsetcolor.h"
#include "ReportScheme.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgBlockSet dialog

#define COLORNOTCOUSTOM	CLR_DEFAULT

class CDlgBlockSet : public CDialogEx
{
public:
	COLORREF		GetColor();
	void			SetColor(COLORREF clr);
	LRESULT OnSetColor(WPARAM wParam,LPARAM lParam);
	void			SetHead(E_ReportType eHead);
	E_ReportType GetHead();

	void			SetModifyFlag(bool32 bModify);				
private:	
	COLORREF		m_Color;
	E_ReportType m_eHead;

	bool32			m_bModify;		// 修改和添加都是用的这个对话框
	CString			m_StrNameBK;	// 保存进入对话框时的板块名,当修改板块信息时,用于比较.
// Construction
public:
	CDlgBlockSet(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgBlockSet)
	enum { IDD = IDD_DIALOG_BLOCK_SET };
	CString			m_StrHotKey;
	CString			m_StrName;
	CStaticSetColor m_StaticColor;
	CComboBox		m_ComboHead;
	BOOL			m_bCheck;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBlockSet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgBlockSet)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnChangeName();
	afx_msg void OnCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBLOCKSET_H__CE01408F_5A02_439C_84AA_E853CC2F6779__INCLUDED_)
