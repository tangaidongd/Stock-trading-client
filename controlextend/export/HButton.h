#pragma once
#include "dllexport.h"
#define HS_NONE 0
#define HS_ALWAYS 1
#define HS_HOVER 2

#define TRANS_BACK MAXUINT
// CHButton

class CONTROL_EXPORT CHButton : public CButton
{
	DECLARE_DYNAMIC(CHButton)

public:
	CHButton();
	virtual ~CHButton();
	
protected:
	COLORREF m_clrBkColor;
	HCURSOR m_Cursor;
	BOOL m_bCursor;
	CToolTipCtrl m_ToolTip;
    BOOL m_bMouseOver;
	COLORREF m_clrHoverColor;
	BOOL m_bIcon;
	BOOL m_bHoverColor ;
	COLORREF m_clrTextColor;
	UINT m_nUnderline;
	BOOL m_bUnderline;
	void OnMouseMove(UINT nFlags,CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//
	//afx_msg HBRUSH OnCtlColor(CDC *pDC,CWnd *pWnd,UINT nCtlColor);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) ;
	void PreSubclassWindow(void);
	BOOL PreTranslateMessage(MSG* pMsg) ;
 	DECLARE_MESSAGE_MAP()
public:
	void SetToolTip(LPCTSTR strText, BOOL bActive = TRUE);
	void SetHoverTextColor(COLORREF clrTextColor,BOOL bShowColor = TRUE);
	void SetCursor(HCURSOR curCursor);
	void SetUnderline(UINT nUnderline);
	void SetTextColor(COLORREF clrColor);
	UINT m_nIconSize;
	HICON m_Icon;
	void SetIcon(HICON Icon);

    void SetButtonFont(const TCHAR *pFontName, int nSize = 12);
private:
    CFont	m_font;
	CBrush	m_Brush;
	COLORREF m_BackColor;
	COLORREF m_TextColor;
};


