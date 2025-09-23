#if !defined(AFX_RICHEDITFORMULAR_H__1A1A72C1_0498_11D7_9690_D53117962B72__INCLUDED_)
#define AFX_RICHEDITFORMULAR_H__1A1A72C1_0498_11D7_9690_D53117962B72__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RichEditFormular.h : header file
//
//#include "MyTipWin.h"
#include "AutoRichEditCtrl.h"
#include "XTipWnd.h"
/////////////////////////////////////////////////////////////////////////////
// CRichEditFormular window
struct dataFunc;
class CRichEditFormular : public CAutoRichEditCtrl
{
// Construction
public:
	CRichEditFormular();
	CString GetWordByOffset(int nOffset);
	void SetDataFunc(dataFunc *stritemsArr,int nItemCountText);
// Attributes
public:
	CXTipWnd m_TitleTip;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRichEditFormular)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRichEditFormular();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRichEditFormular)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	dataFunc *m_pstritemsArr;
	int m_nItemCountText;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RICHEDITFORMULAR_H__1A1A72C1_0498_11D7_9690_D53117962B72__INCLUDED_)
