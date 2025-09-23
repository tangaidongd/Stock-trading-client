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

#include "dllexport.h"
#ifndef XSCROLLBAR_H
#define XSCROLLBAR_H



/////////////////////////////////////////////////////////////////////////////
// CXScrollBar

class CONTROL_EXPORT CXScrollBar : public CStatic
{
// Construction
public:
	CXScrollBar();
	virtual ~CXScrollBar();
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
	void	UseAppRes(BOOL bUserAppRes = TRUE){m_bUserAppRes = bUserAppRes;}
	int		GetScrollPos() const { return m_nPos; }
	int		SetScrollPos(int nPos, BOOL bRedraw = TRUE);
	
	void	SetScrollRange(int nMinPos, int nMaxPos, BOOL bRedraw = TRUE);
	void	GetScrolRange(int& iMin, int& iMax) { iMin = m_nMinPos; iMax = m_nMaxPos;}

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

	bool	m_bIsStone;	// 特殊标识，为ture是时，会有一些特殊的处理
	
	// lcq add 动态设置滚动条各图片(未设置垂直滚动条)
	BOOL m_bUserAppRes;//是否使用外部图片和下面的顺序对应
	int m_IDB_LeftArrowH;
	int m_IDB_RightArrowH;
	int m_IDB_ChannelH;
	int m_IDB_ThumbH;
	int m_IDB_ThumbNoColorH;
	
	int m_IDB_DownArrowV;
	int m_IDB_UpArrowV;
	int m_IDB_ChannelV;
	int m_IDB_ThumbV;
	int m_IDB_ThumbNoColorV;
	
	void SetScrollBarLeftArrowH(const int idb);	
	void SetScrollBarRightArrowH(const int idb);	
	void SetScrollBarChannelH(const int idb);	
	void SetScrollBarThumbH(const int idb);
	void SetScrollBarThumbNoColorH(const int idb);
	
	void SetScrollBarDownArrowV(const int idb);	
	void SetScrollBarUpArrowV(const int idb);	
	void SetScrollBarChannelV(const int idb);	
	void SetScrollBarThumbV(const int idb);
	void SetScrollBarThumbNoColorV(const int idb);

	void SetBorderColor(COLORREF clrBorder);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXScrollBar)
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

	int			m_nThumbPos;			// 鼠标点击时，保存鼠标在thumb中的相对Y方向位置

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
	COLORREF		m_clrBorder;

	int             m_nAlphaDefine;

	CPoint          m_ptBakMouseMove;
	// Generated message map functions
protected:
	//{{AFX_MSG(CXScrollBar)
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

#endif //XSCROLLBAR_H
