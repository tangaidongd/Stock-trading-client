#if !defined(AFX_STATICSETCOLOR_H__D2E7AF96_E5D4_47CC_B9C9_EA4B4DDB2EAC__INCLUDED_)
#define AFX_STATICSETCOLOR_H__D2E7AF96_E5D4_47CC_B9C9_EA4B4DDB2EAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// staticsetcolor.h : header file
//
#include "dllexport.h"
/////////////////////////////////////////////////////////////////////////////
// CStaticSetColor window
#include "typedef.h"
class CONTROL_EXPORT CStaticSetColor : public CStatic
{
// Construction
public:
	enum E_ColorType		// 表示的颜色类型，影响GetColor的获取颜色方式
	{
		ECT_SingleColor,
		ECT_MultiColor,
	};
	CStaticSetColor();
// Attributes
public:
	COLORREF      m_color;    
	bool32		  m_bNeedDraw;
	bool32		  m_bParentDlgFace;
	E_ColorType	  m_ectColorType;
// Operations
public:
	void         SetColor(COLORREF clr);
	void		 SetText (COLORREF clr);
	void		 SetDefaultText();
	COLORREF	 GetColor(CPoint pt);

	void		SetColorType(E_ColorType ect) { m_ectColorType = ect; }
	E_ColorType	GetColorType() const { return m_ectColorType; }

	void		DrawMyStatic(CDC *pDC = NULL);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticSetColor)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStaticSetColor();
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CStaticSetColor)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATICSETCOLOR_H__D2E7AF96_E5D4_47CC_B9C9_EA4B4DDB2EAC__INCLUDED_)
