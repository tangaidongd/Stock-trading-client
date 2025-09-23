#if !defined(_H_MDICLINETEX_)
#define _H_MDICLINETEX_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// mdiclientex.h : header file
//
#include "GdiPlusTS.h"

/////////////////////////////////////////////////////////////////////////////
// CMDIClientEx window

class CMDIClientEx : public CWnd
{
// Construction
public:
	CMDIClientEx();

// Attributes
public:
	WNDPROC* GetSuperWndProcAddr();
// Operations
public:
	Image*	m_pImageBkGround;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMDIClientEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMDIClientEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMDIClientEx)
	// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg void OnNcPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_H_MDICLINETEX_)
