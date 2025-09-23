#ifndef _DLGIE_H_
#define _DLGIE_H_

// 浏览器对话框
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgnotepad.h : header file
//

#include "DialogEx.h"
#include "WndCef.h"
#include "OleRichEditCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgIE dialog

class CDlgIE : public CDialogEx
{
public:
	
	static void ShowDlgIE(const CString &StrTitle, const CString &StrUrl, const CString &StrContent);	// 浏览指定页面, 或者显示指定内容
	
private:
	
	// Construction
protected:
	CDlgIE(CWnd* pParent = NULL);   // standard constructor
	
	void		PromptErrorInput(const CString &StrPrompt, CWnd *pWndFocus = NULL);
	void		SetTitleUrl(const CString &StrTitle, const CString &StrUrl, const CString &StrContent);

	void		RecalcLayout();
	
	CString		m_StrTitle;
	CString		m_StrUrl;
	CString     m_StrContent;

	CWndCef		m_wndCef;
	
	// Dialog Data
	//{{AFX_DATA(CDlgIE)
	enum { IDD = IDD_DIALOG_IE };
	COleRichEditCtrl m_RichEdit;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgIE)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:	
	// Generated message map functions
	//{{AFX_MSG(CDlgIE)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRichEditExLink(NMHDR* in_pNotifyHeader, LRESULT* out_pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //_DLGIE_H_