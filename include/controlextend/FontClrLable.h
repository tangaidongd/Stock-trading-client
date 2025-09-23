#if !defined(AFX_FONTCLRLABLE_H__F267D77E_A901_4972_A42B_F2137568464A__INCLUDED_)
#define AFX_FONTCLRLABLE_H__F267D77E_A901_4972_A42B_F2137568464A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FontClrLable.h : header file
//
#include "dllexport.h"
/////////////////////////////////////////////////////////////////////////////
// CFontClrLable window
enum FlashType {None, Text, Background };

class CONTROL_EXPORT CFontClrLable : public CStatic
{
	//DECLARE_DYNAMIC(CFontClrLable)
// Construction
public:
	CFontClrLable();

	CFontClrLable& SetBkColor(COLORREF crBkgnd);
	CFontClrLable& SetTextColor(COLORREF crText);
	CFontClrLable& SetText(const CString& strText);
	CFontClrLable& SetFontBold(BOOL bBold);
	CFontClrLable& SetFontName(const CString& strFont);
	CFontClrLable& SetFontUnderline(BOOL bSet);
	CFontClrLable& SetFontItalic(BOOL bSet);
	CFontClrLable& SetFontSize(int nSize);
	CFontClrLable& SetSunken(BOOL bSet);
	CFontClrLable& SetBorder(BOOL bSet);
	CFontClrLable& FlashText(BOOL bActivate);
	CFontClrLable& FlashBackground(BOOL bActivate);
	CFontClrLable& SetLink(BOOL bLink);
	CFontClrLable& SetLinkCursor(HCURSOR hCursor);
// Attributes
public:
	
private:
	void ReconstructFont();
	COLORREF	m_crText;
	HBRUSH		m_hBrush;
	HBRUSH		m_hwndBrush;
	LOGFONT		m_lf;
	CFont		m_font;
	CString		m_strText;
	BOOL		m_bState;
	BOOL		m_bTimer;
	BOOL		m_bLink;
	FlashType	m_Type;
	HCURSOR		m_hCursor; 
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFontClrLable)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFontClrLable();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFontClrLable)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FONTCLRLABLE_H__F267D77E_A901_4972_A42B_F2137568464A__INCLUDED_)
