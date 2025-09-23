#if !defined(AFX_DLGNEWS_H__CF0C074A_210D_49AE_9435_9FAFF4AE3DBA__INCLUDED_)
#define AFX_DLGNEWS_H__CF0C074A_210D_49AE_9435_9FAFF4AE3DBA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgNews.h : header file
//
#include "DialogEx.h"
#include "FontClrLable.h"
#include "facescheme.h"
#include "NewsShowView.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgNews dialog

class CDlgNews : public CDialogEx
{
// Construction
public:
	CDlgNews(CWnd* pParent = NULL);   // standard constructor
public:
	void SetTitle(const CString &strTitle){ m_strTitle = strTitle; };
	CString GetTitle(){ return m_strTitle; };

	void SetNewsText(const CString &strNews){ m_strNewsText = strNews; }

	void SetstrTime(const CString &strTimer){ m_strTime = strTimer; }
public:
	COLORREF m_clrText;
	COLORREF m_clrBackground;
private:
	CString m_strTitle;
	CString m_strTime;
	CFontNode m_FontNode;
	CFont     m_Font;
	CNewsShowView  *m_pView;

	UINT   m_nTimerID;
public:
// Dialog Data
	//{{AFX_DATA(CDlgNews)
	enum { IDD = IDD_DIALOG_NEWSVIEW };
	CEdit	m_ctrlNews;
	CStatic	m_ctrPicPos;
	CString	m_strNewsText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgNews)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgNews)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGNEWS_H__CF0C074A_210D_49AE_9435_9FAFF4AE3DBA__INCLUDED_)
