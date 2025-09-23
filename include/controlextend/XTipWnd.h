#ifndef _X_TIPWND_H_
#define _X_TIPWND_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "dllexport.h"

// XTipWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CXTipWnd window

class CONTROL_EXPORT CXTipWnd : public CWnd 
{
public:
	enum E_ShowTitle {ESTAuto = 0, ESTShow = 1, ESTNotShow = 2};

public:
	CXTipWnd();
	virtual ~CXTipWnd();
	
public:
	virtual BOOL Create(CWnd *pParentWnd);
	void Show(CPoint point, LPCTSTR lpszContentText, LPCTSTR lpszTitleText = NULL, E_ShowTitle eShowTile = ESTAuto);
	void Hide();
	
protected:
	int CalcSize();
	void SetString(CString& s);
	
protected:
	CPoint m_pointOld;
	CWnd *m_pParentWnd;
	
	
private:
	CFont			m_FontNormal;
	int				m_fontSize;
	int				m_iLineHeight;

	COLORREF		m_clrTitleBackgroud;
	COLORREF		m_clrContentBackgroud;
	COLORREF		m_clrTitleText;
	COLORREF		m_clrContentText;

	CPoint			m_pOld;

	int				m_iWidth;
	int				m_iHeight;
	BOOL			m_bShowTitle;

	CString			m_strTitle;
	CStringList		m_stringList;
public:
	bool			m_bWindowsPosition;

////////////////////////////////////
////////////////////////////////////
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXTipWnd)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CXTipWnd)
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // _X_TIPWND_H_
