#if !defined(AFX_NEWSSHOWVIEW_H__F34A9B4A_E2D0_4195_96E0_3761BB68CC06__INCLUDED_)
#define AFX_NEWSSHOWVIEW_H__F34A9B4A_E2D0_4195_96E0_3761BB68CC06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewsShowView.h : header file
//
#include "GdiPlusTS.h"
#include "NewTextLable.h"

/////////////////////////////////////////////////////////////////////////////
// CNewsShowView view

class CNewsShowView : public CScrollView
{
public:
	CNewsShowView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CNewsShowView)

	virtual ~CNewsShowView();
// Attributes
private:
	CNewTextLable m_NewsText;
public:
	void InitNewsStauts();

	void SetNewsConext(const CString &strTitle,const CString &strTimer,const CString &strText);

	void SetDisclaimer(const CString &strTitle,const CString &strDisc);

	void CalcScrollSize(CDC *pDC);
// Operations
public:
	virtual void OnInitialUpdate( );     // first time after construct
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewsShowView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CNewsShowView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnMouseActivate(CWnd*pDeskopWnd,UINT nHitTest,UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
#if !defined(_WIN32_WCE) && (_MFC_VER >= 0x0421)
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
#endif
	DECLARE_MESSAGE_MAP()
private:
	CSize m_scrollSize;
	int32 m_iHeightImg;  // logoµÄ¸ß¶È
	Image *m_pImgLogo;
	CString	m_strAppName;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWSSHOWVIEW_H__F34A9B4A_E2D0_4195_96E0_3761BB68CC06__INCLUDED_)
