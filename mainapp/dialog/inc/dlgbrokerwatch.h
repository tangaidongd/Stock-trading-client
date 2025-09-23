#if !defined(AFX_DLGBROKERWATCH_H__A2574406_936A_4E08_9E5B_67B53992A287__INCLUDED_)
#define AFX_DLGBROKERWATCH_H__A2574406_936A_4E08_9E5B_67B53992A287__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgbrokerwatch.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgBrokerWatch dialog
#include "GridCtrlSys.h"
#include "XScrollBar.h"
#include "DialogEx.h"
class CDlgBrokerWatch : public CDialogEx
{
// Construction
public:
	CDlgBrokerWatch(CWnd* pParent = NULL);   // standard constructor
public:
	bool32	CreateGrid();

protected:
	CGridCtrlSys    m_GridCtrlLeft;
	CGridCtrlSys    m_GridCtrlRight;
	CImageList		m_ImageList;
	CXScrollBar		m_XSBVertLeft;
	CXScrollBar		m_XSBVertRight;

// Dialog Data
	//{{AFX_DATA(CDlgBrokerWatch)
	enum { IDD = IDD_DIALOG_BROKERWATCH };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBrokerWatch)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();

	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgBrokerWatch)
		// NOTE: the ClassWizard will add member functions here
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridSelChanged(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnClose();	

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBROKERWATCH_H__A2574406_936A_4E08_9E5B_67B53992A287__INCLUDED_)
