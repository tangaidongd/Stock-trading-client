#ifndef _DLG_CONTENT__
#define _DLG_CONTENT__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CDlgWebContent.h : header file
//

#include "WndCef.h"


#define IDC_WEB_EXPLORER	1949	// ä¯ÀÀÆ÷¿Ø¼þµÄID

/////////////////////////////////////////////////////////////////////////////
// CDlgWebContent dialog


class CDlgWebContent : public CDialog
{
    // Construction
public:
    CDlgWebContent(CWnd* pParent = NULL);   // standard constructor
    ~CDlgWebContent();
	virtual void OnClickClose();

protected:
	CWndCef *m_pWndCef;

private:
	CString m_strUrl;
	CString m_strTitle;
	CWnd   *m_pParent;
	CRect	m_parentBtnRect;

public:
	void SetUserDlgInfo(CRect &parentRct);
	void ShowDlgIEWithSize( const CString &StrTitle, const CString &StrUrl, const CRect& rect, int bFlag = 0);
	void ShowDlgIESpecialSize(const CString &StrTitle,const CString &StrUrl,int bFlag = 0);
	void SetTitleUrl( const CString &StrTitle, const CString &StrUrl);
	void ShowWeb(const CRect& rc);	// ÏÔÊ¾ÍøÒ³ä¯ÀÀÆ÷
protected:
	//void SetTitleUrl( const CString &StrTitle, const CString &StrUrl);
public:
// Dialog Data
	//{{AFX_DATA(CDlgWebContent)
    enum { IDD = IDD_DIALOG_WEB_CONTENT};
	//}}AFX_DATA
	
// Overrides
	// ClassWizard generated virtual function overrides 
	//{{AFX_VIRTUAL(CDlgWebContent)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgWebContent)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSize( UINT nType, int cx, int cy );
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
// 	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//afx_msg BOOL PreTranslateMessage(MSG* pMsg);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 