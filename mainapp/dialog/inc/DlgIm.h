#if !defined(AFX_DLGIm_H__43B85636_28F1_42FE_A0F4_B7C7EA7E3088__INCLUDED_)
#define AFX_DLGIm_H__43B85636_28F1_42FE_A0F4_B7C7EA7E3088__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgwait.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgIm dialog
#include <map>

#define  WM_IM_NOTIFY   (WM_USER+1100)  //IM发送消息

class CDlgIm : public CDialog
{
// Construction
public:
	CDlgIm(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgIm)
	enum { IDD = IDD_DIALOG_IM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
private:
	bool    m_blShow;
	bool    m_blExeExist;
	CRect   m_RectWindow;
	CString m_strName;
	CString m_strPwd;
	CString m_strFullPath;
	CWnd   *m_ImTrayWnd;
	int     m_iWnd;

	//
	 map<CString, CString> m_mapAccount;
public:
	void	ShowIm(bool blShow);
	void    MoveIm(CRect rect);
	//void  SetWinRect(const CRect *pRect);
	//void	GetWinRect(CRect &rect);
	//void  ChangeSize();
	void    OnLogin(CString strName);
	void    OpenMessageWindow();//发送双击消息给托盘窗体
	bool    IsExist();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgIm)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL KillProcessFromName(LPCSTR lpProcessName);
	unsigned int LastLoginCode();

protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgIm)
	afx_msg void OnPaint();
	afx_msg void OnClose();	
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg	LRESULT OnImNotify(WPARAM wParam, LPARAM lParam); //2013-10-29  add by cym IM消息处理

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGIm_H__43B85636_28F1_42FE_A0F4_B7C7EA7E3088__INCLUDED_)
