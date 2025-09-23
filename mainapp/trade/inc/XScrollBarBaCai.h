// XScrollBar.h  Version 1.1
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// License:
//     This software is released into the public domain.  You are free to use
//     it in any way you like, except that you may not sell this source code.
//
//     This software is provided "as is" with no expressed or implied warranty.
//     I accept no liability for any damage or loss of business that this
//     software may cause.
//
///////////////////////////////////////////////////////////////////////////////
#include "DIBSectionLite.h"


#include "iXScrollBarForGrid.h"

#ifndef XSCROLLBARBACAI_H
#define XSCROLLBARBACAI_H

/////////////////////////////////////////////////////////////////////////////
// CXScrollBarBaCai

class CXScrollBarBaCai : public CStatic, public iXScrollBarForGrid
{
// Construction
public:
	CXScrollBarBaCai();
	virtual ~CXScrollBarBaCai();
	BOOL CreateFromStatic(DWORD dwStyle,
						  CWnd* pParentWnd,
						  UINT nIdStatic,
						  UINT nId);

	BOOL Create(DWORD dwStyle,
						 CWnd* pParentWnd,
						 LPRECT lpRect,
						 UINT nId);
// Attributes
public:
	void	EnableChannelColor(BOOL bEnable) { m_bChannelColor = bEnable; }
	void	EnableThumbColor(BOOL bEnable) { m_bThumbColor = bEnable; }
	void	EnableThumbGripper(BOOL bGripper) { m_bThumbGripper = bGripper; }

	// iXScrollBarForGrid
public:
	virtual int		GetScrollPos() const { return m_nPos; }
	virtual int		SetScrollPos(int nPos, BOOL bRedraw = TRUE);
	virtual void	SetScrollRange(int nMinPos, int nMaxPos, BOOL bRedraw = TRUE);
	virtual void	GetScrolRange(int& iMin, int& iMax) { iMin = m_nMinPos; iMax = m_nMaxPos;}

	virtual BOOL	EnableBarWindow(BOOL bEnable) { return EnableWindow(bEnable); }
	virtual BOOL	IsSafeValid() { return NULL==this ? FALSE : GetSafeHwnd()!=NULL; }

	void	SetThumbColor(COLORREF rgb) { m_ThumbColor = rgb; m_bThumbColor = TRUE; }
	void	SetThumbHoverColor(COLORREF rgb) { m_ThumbHoverColor = rgb; }
	
	// zhangbo 0311 #add
	void	SetSBRect(LPRECT lpRect, BOOL bDraw = TRUE);
	BOOL	IsHorz();
	BOOL	GetFitHorW();

	// xl
	void	AddMsgListener(HWND hwnd);
	void	RemoveMsgListener(HWND hwnd);
	
// Operations
public:
	void	ScrollLeft();
	void	ScrollRight();
	void	ScrollUp();
	void	ScrollDown();

	void    SetFaceChemeColor(const COLORREF &clr);

	void	FireMsg(UINT msgId, WPARAM w, LPARAM l);		// 发送给Owner和listener
	
	void	FireListener(UINT msgId, WPARAM w, LPARAM l);	// 通知listener，pos或者range发生变化

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXScrollBarBaCai)
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWnd *		m_pParent;				// control parent 
	int			m_nThumbLeft;			// left margin of thumb
	int			m_nThumbTop;			// top margin of thumb
	int			m_nBitmapHeight;		// height of arrow and thumb bitmaps
//	int			m_nBitmapWidth;			// width of arrow and thumb bitmaps
	int			m_nPos;					// current thumb position in scroll units
	int			m_nMinPos;				// minimum scrolling position
	int			m_nMaxPos;				// maximum scrolling position
	int			m_nRange;				// absolute value of max - min pos
	CRect		m_rectThumb;			// current rect for thumb
	CRect		m_rectLeftArrow;
	CRect		m_rectRightArrow;	
	CRect		m_rectClient;			// control client rect
    HCURSOR		m_hCursor;				// hand cursor
	COLORREF	m_ThumbColor;			// thumb color
	COLORREF	m_ThumbHoverColor;		// thumb color when mouse hovers
	BOOL		m_bThumbColor;			// TRUE = display thumb with color
	BOOL		m_bChannelColor;		// TRUE = display color in channel
	BOOL		m_bThumbGripper;		// TRUE = display thumb gripper
	BOOL		m_bThumbHover;			// TRUE = mouse is over thumb
	BOOL		m_bMouseDownArrowRight;	// TRUE = mouse over right arrow & left button down
	BOOL		m_bMouseDownArrowLeft;	// TRUE = mouse over left arrow & left button down
	BOOL		m_bMouseDownArrowUp;	// TRUE = mouse over left arrow & left button down
	BOOL		m_bMouseDownArrowDown;	// TRUE = mouse over right arrow & left button down
	BOOL		m_bMouseDown;			// TRUE = mouse over thumb & left button down
	BOOL		m_bDragging;			// TRUE = thumb is being dragged
	BOOL		m_bHorizontal;			// TRUE = horizontal scroll bar

	CArray<HWND, HWND>		m_aHwndListener;		// 所有消息都将send这个里面的一份，主要是方便Report更新滚动位置

	void		Draw();
	void		DrawHorizontal(CDC *pDC=NULL);
	void		DrawVertical();
	void		LimitThumbPosition();
	void		SetDefaultCursor();
	void		SetPositionFromThumb();
	void		UpdateThumbPosition();
private:
	//lxp add
	void		DrawHorizontalExt(CDC *pDC=NULL);
    COLORREF    GetMarkColor(const COLORREF &clr);
	void DrawVerticalExt();
private:
	CDIBSectionLite m_LeftArrow;
	CDIBSectionLite m_RightArrow;
	CDIBSectionLite m_ThumbNoColor;
	CDIBSectionLite m_ThumbHColor;
	CDIBSectionLite m_FaceChemeColor;
	CDIBSectionLite m_ChannelColor;

	COLORREF		m_clrFaceCheme;

	int             m_nAlphaDefine;

	CPoint          m_ptBakMouseMove;
	// Generated message map functions
protected:
	//{{AFX_MSG(CXScrollBarBaCai)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //XSCROLLBARBACAI_H
