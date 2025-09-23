#ifndef _DLG_ACCOUNT_01_27_
#define _DLG_ACCOUNT_01_27_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CDlgTodayComment.h : header file
//
#include "DialogEx.h"
#include "WndCef.h"


#define IDC_WEB_EXPLORER	1949	// 浏览器控件的ID

/////////////////////////////////////////////////////////////////////////////
// CDlgAccount dialog
//用户信息结构体
struct T_USERINFO
{
    CString         m_UserType;
    CString         m_StrUserAccount;//账号
    CString         m_StrOtherName;//昵称
    CString         m_StrPhone;    //手机
    CString         m_StrMail;     //邮箱
    CString         m_StrQQ;       //QQ
    CString         m_StrImagePath;
};

class CDlgAccount : public CDialog
{
    // Construction
public:
    CDlgAccount(CWnd* pParent = NULL);   // standard constructor
    ~CDlgAccount();
	virtual void OnClickClose();

protected:
	CWndCef *m_pWndCef;

private:
	CString m_strUrl;
	CString m_strTitle;
	CWnd   *m_pParent;
	CRect	m_parentBtnRect;
	Image           *m_pImgBk;

public:
	void SetUserDlgInfo(CRect &parentRct);
	void ShowDlgIEWithSize( const CString &StrTitle, const CString &StrUrl, const CRect& rect, int bFlag = 0);
	void ShowDlgIESpecialSize(const CString &StrTitle,const CString &StrUrl,int bFlag = 0);
	void SetTitleUrl( const CString &StrTitle, const CString &StrUrl);
	void ShowWeb(const CRect& rc);	// 显示网页浏览器
protected:
	//void SetTitleUrl( const CString &StrTitle, const CString &StrUrl);
public:
// Dialog Data
	//{{AFX_DATA(CDlgAccount)
    enum { IDD = IDD_DIALOG_USER_ACCOUNT};
	//}}AFX_DATA
	
// Overrides
	// ClassWizard generated virtual function overrides 
	//{{AFX_VIRTUAL(CDlgAccount)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAccount)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSize( UINT nType, int cx, int cy );
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//afx_msg BOOL PreTranslateMessage(MSG* pMsg);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 