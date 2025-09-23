#if !defined(AFX_NEWSPINBUTTONCTRL_H__DD26CA3C_1BAD_4458_AA3C_E7F0FAC8AAB4__INCLUDED_)
#define AFX_NEWSPINBUTTONCTRL_H__DD26CA3C_1BAD_4458_AA3C_E7F0FAC8AAB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewSpinButtonCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewSpinButtonCtrl window
#include "GdiPlusTS.h"
#include "dllexport.h"

class CONTROL_EXPORT CNewSpinButtonCtrl : public CSpinButtonCtrl
{
// Construction
public:
	CNewSpinButtonCtrl();

// Attributes
public:
	void CalculateRect();

private:
	bool m_bTopDown;
	bool m_bBottomDown;
	CRect m_rctTop;
	CRect m_rctBottom;

	Image	*m_pImgTop;
	Image	*m_pImgBottom;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewSpinButtonCtrl)
	public:
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNewSpinButtonCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNewSpinButtonCtrl)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWSPINBUTTONCTRL_H__DD26CA3C_1BAD_4458_AA3C_E7F0FAC8AAB4__INCLUDED_)
