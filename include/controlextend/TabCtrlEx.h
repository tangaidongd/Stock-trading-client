#if !defined(AFX_TABCTRLBASE_H__09B78528_AE72_4D7D_B6EA_F4E7BCEE0839__INCLUDED_)
#define AFX_TABCTRLBASE_H__09B78528_AE72_4D7D_B6EA_F4E7BCEE0839__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabCtrlEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTabCtrlEx window
#include "dllexport.h"
class CONTROL_EXPORT CTabCtrlEx : public CTabCtrl
{
// Construction
public:
	CTabCtrlEx();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabCtrlEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTabCtrlEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTabCtrlEx)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABCTRLBASE_H__09B78528_AE72_4D7D_B6EA_F4E7BCEE0839__INCLUDED_)
