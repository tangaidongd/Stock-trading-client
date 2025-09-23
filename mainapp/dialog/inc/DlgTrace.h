#if !defined(AFX_DLGTRACE_H__FA957CFC_50C4_40F3_95ED_E439F1DBFF63__INCLUDED_)
#define AFX_DLGTRACE_H__FA957CFC_50C4_40F3_95ED_E439F1DBFF63__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTrace.h : header file
//

#include "XScrollBar.h"
#include "GridCtrl.h"
  

/////////////////////////////////////////////////////////////////////////////
// CDlgTrace dialog

class CDlgTrace : public CDialog
{
// Construction
public:
	CDlgTrace(CWnd* pParent = NULL);   // standard constructor
	~CDlgTrace();
public:
	void			ConstructGrid();
	void			OnSizeChanged();

	void			AddTrace(int iTraceType, UINT uiTime, const CString &StrTrace);


private:
	CXScrollBar		m_XSBVert;
	CXScrollBar		m_XSBHorz;
	CImageList		m_ImageList;	
	CGridCtrl		m_GridTrace;

	int				m_iRowIndex;

	bool			m_bInitlized;
	bool			m_bStop;

public:
	FILE*			m_pLogFile;

//////////////////////////////////////////////////////////////////////
// Dialog Data
	//{{AFX_DATA(CDlgTrace)
	enum { IDD = IDD_TRACE };
	CButton	m_BtnOk;
	//}}AFX_DATA

	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTrace)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTrace)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnMsgAddTrace(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgAddGoodTrace(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgAddBadTrace(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgLog(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgCheckTime(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBtnSaveText();
	afx_msg void OnButtonStop();
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTRACE_H__FA957CFC_50C4_40F3_95ED_E439F1DBFF63__INCLUDED_)
