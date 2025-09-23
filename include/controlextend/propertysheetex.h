#if !defined(AFX_PROPERTYSHEETEX_H_)
#define AFX_PROPERTYSHEETEX_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// propertysheetex.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetEX
#include "dllexport.h"
#include "GdiPlusTS.h"
class CONTROL_EXPORT CPropertySheetEX : public CPropertySheet
{
	DECLARE_DYNAMIC(CPropertySheetEX)
public:
	enum E_CurrentShowState
	{
		ECSSNormal = 0,
			ECSSForcusClose,			
			ECSSPressClose,
			//
			ECSSCount			
	};
public:
	void	ConstructPropertySheet();
	void	CalcNcSize();
	void	SetWindowTextEx(const CString& StrText);

	void	DrawSCButtons(CWindowDC &dc, Graphics *pGraphics);			// ，如果pGra NULL，则绘制文字
	
protected:
	CRect	m_rectClose;
	CRect   m_rectCaption;
	
	CRect   m_rectLeftBoard;
	CRect   m_rectRightBoard;
	CRect   m_rectBottomBoard;
	
	Image*	m_pImageButtons;
	Image*	m_pImageCatptionBig;
	Image*	m_pImageCatptionSmall;
	
	bool32	m_bNcMouseMoving;
	bool32  m_bActive;
	
	E_CurrentShowState m_eCurrentShowState;
	// Construction
public:
	CPropertySheetEX(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPropertySheetEX(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	
	// Implementation
public:
	virtual ~CPropertySheetEX();
	
	//XL0002 - OnNcMouseLeave
	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertySheetEX)
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg	LRESULT OnNcHitTest(CPoint point);
	afx_msg	LRESULT OnNcMouseLeave(WPARAM, LPARAM);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYSHEETEX_H_)
