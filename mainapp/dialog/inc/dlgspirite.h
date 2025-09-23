#if !defined(AFX_DLGSPIRITE_H__)
#define AFX_DLGSPIRITE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgspirite.h : header file
//

#include "GridCtrlSys.h"
#include "XScrollBar.h"


/////////////////////////////////////////////////////////////////////////////
// CDlgSpirite dialog

class CDlgSpirite : public CDialog
{
// Construction
public:
	CDlgSpirite(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSpirite)
	enum { IDD = IDD_DIALOG_SPIRITE };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSpirite)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	CGridCtrlSys    m_GridCtrl;
	CImageList		m_ImageList;
	CXScrollBar		m_XSBVert;
	CXScrollBar		m_XSBHorz;

public:
	bool32	CreateGrid();				
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgSpirite)
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSPIRITE_H__300594E1_5657_41A2_92A0_D7CDDCEC466A__INCLUDED_)
