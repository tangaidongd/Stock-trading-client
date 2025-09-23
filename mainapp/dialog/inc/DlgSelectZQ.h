#if !defined(AFX_DLGSELECTZQ_H__7423845C_6D25_4511_9BEC_B0098EFA5943__INCLUDED_)
#define AFX_DLGSELECTZQ_H__7423845C_6D25_4511_9BEC_B0098EFA5943__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSelectZQ.h : header file
//
#include "DialogEx.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgSelectZQ dialog

class CDlgSelectZQ : public CDialogEx
{
// Construction
public:
	CDlgSelectZQ(CWnd* pParent = NULL);   // standard constructor

	uint32 flag;
// Dialog Data
	//{{AFX_DATA(CDlgSelectZQ)
	enum { IDD = IDD_EDIT_ZHOUQI };
	BOOL	m_bFS;
	BOOL	m_bM1;
	BOOL	m_bM15;
	BOOL	m_bM30;
	BOOL	m_bM5;
	BOOL	m_bM60;
	BOOL	m_bM180;
	BOOL	m_bM240;
	BOOL	m_bDay;
	BOOL	m_bMonth;
	BOOL	m_bQuarter;
	BOOL	m_bTrade;
	BOOL	m_bWeek;
	BOOL	m_bYear;
	BOOL	m_bMUser;
	BOOL	m_bDayUser;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSelectZQ)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSelectZQ)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnAddAll();
	afx_msg void OnDelAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSELECTZQ_H__7423845C_6D25_4511_9BEC_B0098EFA5943__INCLUDED_)
