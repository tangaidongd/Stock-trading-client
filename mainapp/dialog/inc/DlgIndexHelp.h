// DlgWelcome.h : header file
//

#if !defined(AFX_234DLGHELP_H__32304215_15E0_4784_85DC_5626320532D4__INCLUDED_)
#define AFX_234DLGHELP_H__32304215_15E0_4784_85DC_5626320532D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "DialogEx.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgIndexHelp dialog

class CDlgIndexHelp : public CDialogEx
{
// Construction
public:
	CDlgIndexHelp(CWnd* pParent = NULL);	// standard constructor
	 ~CDlgIndexHelp();
public:
	void SetShowImg(CString strImg);
	// Dialog Data
	//{{AFX_DATA(CDlgIndexHelp)
	enum { IDD = IDD_DIALOG_INDEX_HELP };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgIndexHelp)

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	HICON m_hIcon;
	
	// Generated message map functions
	//{{AFX_MSG(CDlgIndexHelp)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnPaint();
	virtual void PostNcDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString		m_strShowImg;
	Image		*m_pImgHelp; 
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_234DLG_H__32304215_15E0_4784_85DC_5626320532D4__INCLUDED_)
