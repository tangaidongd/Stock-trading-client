#if !defined(_CDLGTODAYCOMMENT_H__)
#define _CDLGTODAYCOMMENT_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CDlgTodayComment.h : header file
//
#include "DialogEx.h"
#include "WndCef.h"


#define IDC_WEB_EXPLORER	1949	// ä¯ÀÀÆ÷¿Ø¼þµÄID

/////////////////////////////////////////////////////////////////////////////
// CCDlgTodayComment dialog

class CDlgTodayCommentIE : public CDialogEx
{
// Construction
public:
	CDlgTodayCommentIE(CWnd* pParent = NULL);   // standard constructor
	~CDlgTodayCommentIE();
	
	virtual void OnClickClose();

protected:
	CWndCef *m_pWndCef;

private:
	CString  m_strUrl;
	CString  m_strTitle;

public:
	static void ShowDlgIEWithSize( const CString &StrTitle, const CString &StrUrl, const CRect& rect, int bFlag = 0, Color clrCefBk=RGB(30,36,40));
	static void ShowDlgIESpecialSize(const CString &StrTitle,const CString &StrUrl,int bFlag = 0);
	void SetTitleUrl( const CString &StrTitle, const CString &StrUrl);
	void ShowWeb(const CRect& rc, Color clrCefBk=RGB(35,34,40));	// ÏÔÊ¾ÍøÒ³ä¯ÀÀÆ÷
protected:
	//void SetTitleUrl( const CString &StrTitle, const CString &StrUrl);
public:
// Dialog Data
	//{{AFX_DATA(CCDlgTodayComment)
	enum { IDD = IDD_DIALOG_TODAYCOMMENT_IE};
	//}}AFX_DATA
	
// Overrides
	// ClassWizard generated virtual function overrides 
	//{{AFX_VIRTUAL(CCDlgTodayComment)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCDlgTodayComment)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSize( UINT nType, int cx, int cy );
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//afx_msg BOOL PreTranslateMessage(MSG* pMsg);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 