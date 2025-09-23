#if !defined(AFX_STATICSD_H__1DBC2684_34F0_41CF_82B6_121FCC6C19FA__INCLUDED_)
#define AFX_STATICSD_H__1DBC2684_34F0_41CF_82B6_121FCC6C19FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StaticSD.h : header file
//

#include "GridCellBase.h"		// 标记的头在这里

/////////////////////////////////////////////////////////////////////////////
// CStaticSD window

class CStaticSD : public CStatic
{
// Construction
public:
	CStaticSD();
	virtual ~CStaticSD();

public:
	void			SetTitle(const CString &StrTitle, CString StrSubTitle = L"");
	CString			GetTitle()						  { return m_StrTitle;}	
	void			SetActiveFlag(bool32 bActive)	  { m_bActive = bActive; }
	void			SetDrawCornerFlag(bool32 bDrawFlag)	  { m_IsDrawCorner = bDrawFlag; }
	void			SetDrawActiveParams(int32 iXpos, int32 iYpos, COLORREF clrActive);	  

	void			SetDrawMarkItem(const GV_DRAWMARK_ITEM &markItem) { m_DrawMarkItem = markItem; };
	const CRect		&GetDrewMarkRect() const {return m_RectMark;}

private:
	CString			m_StrTitle;
	CString         m_StrSubTitle;
	int32			m_iXposActive;
	int32			m_iYposActive;
	COLORREF		m_ColorActive;
	bool32			m_bActive;
	
	GV_DRAWMARK_ITEM	m_DrawMarkItem;
	CRect			m_RectMark;
	bool			m_IsDrawCorner;

private:
	void GetPolygon(CRect rect, CPoint ptsLeft[7] ,CPoint ptsRight[7], int iMaxLength = 12, int iMinLength = 8, int iCornerBorderH = 1);
	void DrawPolygonBorder(CPaintDC *pPaintDC, CRect rect, COLORREF clrFill = RGB(234, 23, 23), COLORREF clrBorder = RGB(234, 23, 23));

////////////////////////////////////////////////////////////////////////
// 
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticSD)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CStaticSD)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG


	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATICSD_H__1DBC2684_34F0_41CF_82B6_121FCC6C19FA__INCLUDED_)
