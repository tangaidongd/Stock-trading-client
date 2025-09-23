// XScrollBar.cpp  Version 1.1
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// History
//     Version 1.1 - 2004 September 21
//     - Update with Steve Mayfield's vertical scrollbar support
//
//     Version 1.0 - 2004 September 9
//     - Initial public release
//
// Acknowledgements:
//     Thanks to Greg Ellis for his CSkinHorizontalScrollbar class,
//     which I used as the starting point for CXScrollBar:
//         http://www.codeproject.com/listctrl/skinlist.asp
//
//     Thanks to Christian Rodemeyer for his CColor class:
//         http://www.codeproject.com/bitmap/ccolor.asp
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

#include "stdafx.h"
//#include "../../main/res/resource.h"
#include "XScrollBar.h"
#include "memdc.h"
#include "Color.h"
#include "resource.h"
//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
							//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef IDC_HAND
#define IDC_HAND MAKEINTRESOURCE(32649)		// From WINUSER.H
#endif

///////////////////////////////////////////////////////////////////////////////
// timer defines

#define TIMER_MOUSE_OVER_BUTTON		1	// mouse is over an arrow button, and
										// left button is down

#define TIMER_LBUTTON_PRESSED		2	// mouse is over an arrow button, and
										// left button has just been pressed

#define TIMER_MOUSE_OVER_THUMB		3	// mouse is over thumb

///////////////////////////////////////////////////////////////////////////////
// color defines

#define THUMB_MASK_COLOR					RGB(0,0,1)
#define THUMB_GRIPPER_MASK_COLOR			RGB(0,0,2)
#define THUMB_LEFT_TRANSPARENT_MASK_COLOR	RGB(0,0,3)
#define THUMB_RIGHT_TRANSPARENT_MASK_COLOR	RGB(0,0,4)
#define THUMB_UP_TRANSPARENT_MASK_COLOR		THUMB_LEFT_TRANSPARENT_MASK_COLOR
#define THUMB_DOWN_TRANSPARENT_MASK_COLOR	THUMB_RIGHT_TRANSPARENT_MASK_COLOR
#define THUMB_GRIPPER_COLOR					RGB(91,91,91)	// dark gray
#define THUMB_HOVER_COLOR					RGB(255,204,0)	// orange
#define FRAME_COLOR							RGB(19,22,24)	// dark gray					

namespace
{
	void	MyRotateRect(const CRect &rcSrc, CRect &rcDst)
	{
		rcDst.left	= rcSrc.top;
		rcDst.right	= rcSrc.bottom;
		rcDst.top	= rcSrc.left;
		rcDst.bottom	= rcSrc.right;
	}
}

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CXScrollBar, CStatic)
	//{{AFX_MSG_MAP(CXScrollBar)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_SETCURSOR()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// ctor
CXScrollBar::CXScrollBar()
{
	m_pParent              = NULL;
	m_bHorizontal          = TRUE;
	m_hCursor              = NULL;
	m_bMouseDown           = FALSE;
	m_bMouseDownArrowLeft  = FALSE;
	m_bMouseDownArrowRight = FALSE;
	m_bMouseDownArrowUp    = FALSE;
	m_bMouseDownArrowDown  = FALSE;
	m_bDragging            = FALSE;
	m_nPos                 = 0;
	m_nMinPos              = 0;
	m_nMaxPos              = 0;
	m_nRange               = 0;
	m_ThumbColor           = RGB(0,0,0);
	m_ThumbHoverColor      = THUMB_HOVER_COLOR;
	m_bChannelColor        = TRUE;
	m_bThumbColor          = FALSE;
	m_bThumbGripper        = TRUE;
	m_bThumbHover          = FALSE;
	m_rectThumb            = CRect(-1,-1,-1,-1);
	m_rectClient           = CRect(-1,-1,-1,-1);
	m_rectLeftArrow		   = CRect(-1,-1,-1,-1);
	m_rectRightArrow	   = CRect(-1,-1,-1,-1);
	m_nThumbLeft           = -1;
	m_nThumbTop            = 25;	
	m_nBitmapHeight        = 12;

	m_clrFaceCheme         = RGB(255,255,255);
	m_clrBorder			   = RGB(0xE6, 0xE6, 0xE6);
	m_nAlphaDefine		   = 160;

	
	m_IDB_LeftArrowH = IDB_HORIZONTAL_SCROLLBAR_LEFTARROW;
	m_IDB_RightArrowH = IDB_HORIZONTAL_SCROLLBAR_RIGHTARROW;
	m_IDB_ChannelH = IDB_HORIZONTAL_SCROLLBAR_CHANNEL;
	m_IDB_ThumbH = IDB_HORIZONTAL_SCROLLBAR_THUMB;
	m_IDB_ThumbNoColorH = IDB_HORIZONTAL_SCROLLBAR_THUMB_NO_COLOR;
	
	m_IDB_DownArrowV = IDB_VERTICAL_SCROLLBAR_DOWNARROW;
	m_IDB_UpArrowV = IDB_VERTICAL_SCROLLBAR_UPARROW;
	m_IDB_ChannelV = IDB_VERTICAL_SCROLLBAR_CHANNEL;
	m_IDB_ThumbV = IDB_VERTICAL_SCROLLBAR_THUMB;
	m_IDB_ThumbNoColorV = IDB_VERTICAL_SCROLLBAR_THUMB_NO_COLOR;
	m_bUserAppRes = FALSE;
	m_bIsStone = false;	// 初始化
	m_nThumbPos = 0;
}

///////////////////////////////////////////////////////////////////////////////
// dtor
CXScrollBar::~CXScrollBar()
{
	if (m_hCursor)
		DestroyCursor(m_hCursor);
	m_hCursor = NULL;
}

///////////////////////////////////////////////////////////////////////////////
//
// CreateFromStatic
//
// Purpose:     Create the CXScrollBar control from STATIC placeholder
//
// Parameters:  dwStyle    - the scroll bar抯 style. Typically this will be
//                           SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE.
//              pParentWnd - the scroll bar抯 parent window, usually a CDialog
//                           object. It must not be NULL.
//              nIdStatic  - the resource id of the placeholder STATIC
//              nId        - the resource id of the CXScrollBar control
//
// Returns:     BOOL       - TRUE = success
//
// Notes:       Hides the STATIC placeholder.  Also loads hand cursor, and
//              sets the thumb bitmap size.

BOOL CXScrollBar::CreateFromStatic(DWORD dwStyle,
								   CWnd* pParentWnd,
								   UINT nIdStatic,
								   UINT nId)
{
	// TRACE(_T("in CXScrollBar::CreateFromStatic\n"));


	m_pParent = pParentWnd;
	ASSERT(m_pParent);
	if (NULL == m_pParent)
	{
		return FALSE;
	}
	ASSERT(::IsWindow(pParentWnd->GetDlgItem(nIdStatic)->m_hWnd));
	if (!::IsWindow(pParentWnd->GetDlgItem(nIdStatic)->m_hWnd))
	{
		return FALSE;
	}
	CRect rect;
	pParentWnd->GetDlgItem(nIdStatic)->GetWindowRect(&rect);
	pParentWnd->ScreenToClient(&rect);

	// hide placeholder STATIC
	pParentWnd->GetDlgItem(nIdStatic)->ShowWindow(SW_HIDE);

	// load hand cursor
	SetDefaultCursor();

	m_bHorizontal = (dwStyle & SBS_VERT) ? FALSE : TRUE;

	BOOL bResult = 	CStatic::Create(_T(""), dwStyle, rect, pParentWnd, nId);

	if (bResult)
	{
		CBitmap bitmap;
		BOOL bLoad =  TRUE;
		if(m_bUserAppRes == FALSE)
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			bLoad = bitmap.LoadBitmap(m_bHorizontal ? 
				/*IDB_HORIZONTAL_SCROLLBAR_THUMB*/m_IDB_ThumbH : 
			/*IDB_VERTICAL_SCROLLBAR_THUMB*/m_IDB_ThumbV);
		}
		else
		{
			bLoad = bitmap.LoadBitmap(m_bHorizontal ? 
				/*IDB_HORIZONTAL_SCROLLBAR_THUMB*/m_IDB_ThumbH : 
			/*IDB_VERTICAL_SCROLLBAR_THUMB*/m_IDB_ThumbV);
		}
		// we assume that width of thumb is same as width of arrows
		if (bLoad)
		{
			BITMAP bm;
			bitmap.GetBitmap(&bm);
			
			m_nBitmapHeight = bm.bmHeight;

			// TRACE(_T("m_nBitmapWidth=%d  m_nBitmapHeight=%d\n"),
			//	m_nBitmapWidth, m_nBitmapHeight);

			GetClientRect(&m_rectClient);
			// TRACE(_T("m_rectClient:  %d, %d, %d, %d\n"),
			//	m_rectClient.left, m_rectClient.top, 
			//	m_rectClient.right, m_rectClient.bottom);

			if (bitmap.GetSafeHandle())
				bitmap.DeleteObject();
		}
		else
		{
			// TRACE(_T("ERROR - failed to load thumb bitmap\n"));
			ASSERT(FALSE);
		}
	}
	else
	{
		// TRACE(_T("ERROR - failed to create CXScrollBar\n"));
		ASSERT(FALSE);
	}

	return bResult;
}



BOOL CXScrollBar::Create(DWORD dwStyle,
						 CWnd* pParentWnd,
						 LPRECT lpRect,
						 UINT nId)
{
	// TRACE(_T("in CXScrollBar::Create\n"));

	if (NULL == pParentWnd)
	{
		return FALSE;
	}
	m_pParent = pParentWnd;
	ASSERT(m_pParent);
	CRect rect(lpRect);
	pParentWnd->ScreenToClient(&rect);

	// load hand cursor
	SetDefaultCursor();

	m_bHorizontal = (dwStyle & SBS_VERT) ? FALSE : TRUE;

	BOOL bResult = 	CStatic::Create(_T(""), dwStyle, rect, pParentWnd, nId);
	if (bResult)
	{
		CBitmap bitmap;
		BOOL bLoad = TRUE;
		if(m_bUserAppRes == FALSE)
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			bLoad = bitmap.LoadBitmap(m_bHorizontal ? 
				/*IDB_HORIZONTAL_SCROLLBAR_THUMB*/m_IDB_ThumbH : 
			/*IDB_VERTICAL_SCROLLBAR_THUMB*/m_IDB_ThumbV);
		}
		else
		{
			bLoad = bitmap.LoadBitmap(m_bHorizontal ? 
				/*IDB_HORIZONTAL_SCROLLBAR_THUMB*/m_IDB_ThumbH : 
			/*IDB_VERTICAL_SCROLLBAR_THUMB*/m_IDB_ThumbV);
		}
		// we assume that width of thumb is same as width of arrows
		if (bLoad)
		{
			BITMAP bm;
			bitmap.GetBitmap(&bm);
			
			m_nBitmapHeight = bm.bmHeight;

			// TRACE(_T("m_nBitmapWidth=%d  m_nBitmapHeight=%d\n"),
			//	m_nBitmapWidth, m_nBitmapHeight);

			GetClientRect(&m_rectClient);
			// TRACE(_T("m_rectClient:  %d, %d, %d, %d\n"),
			//	m_rectClient.left, m_rectClient.top, 
			//	m_rectClient.right, m_rectClient.bottom);

			if (bitmap.GetSafeHandle())
				bitmap.DeleteObject();
		}
		else
		{
			// TRACE(_T("ERROR - failed to load thumb bitmap\n"));
			ASSERT(FALSE);
		}

#define LOAD(BMP,RES)\
		if(m_bUserAppRes == FALSE)\
		{\
			AFX_MANAGE_STATE(AfxGetStaticModuleState());\
			BOOL bRet = BMP##.SetBitmap(/*IDB_HORIZONTAL_SCROLLBAR_LEFTARROW*/RES);\
			ASSERT( bRet );\
		}\
		else\
		{\
			BOOL bRet = BMP##.SetBitmap(/*IDB_HORIZONTAL_SCROLLBAR_LEFTARROW*/RES);\
			ASSERT( bRet );\
		}\

		LOAD(m_LeftArrow,m_IDB_LeftArrowH)
// 		bLoad = m_RightArrow.SetBitmap(/*IDB_HORIZONTAL_SCROLLBAR_RIGHTARROW*/m_IDB_RightArrowH);
// 		ASSERT( bLoad );
		LOAD(m_RightArrow,m_IDB_RightArrowH)
// 		bLoad = m_ThumbNoColor.SetBitmap(/*IDB_HORIZONTAL_SCROLLBAR_THUMB_NO_COLOR*/m_IDB_ThumbNoColorH);
// 		ASSERT( bLoad );
		LOAD(m_ThumbNoColor,m_IDB_ThumbNoColorH)
// 		bLoad = m_ThumbHColor.SetBitmap(/*IDB_HORIZONTAL_SCROLLBAR_THUMB*/m_IDB_ThumbH);
// 		ASSERT( bLoad );
		LOAD(m_ThumbHColor,m_IDB_ThumbH)
// 		bLoad = m_ChannelColor.SetBitmap(/*IDB_HORIZONTAL_SCROLLBAR_CHANNEL*/m_IDB_ChannelH);
// 		ASSERT( bLoad );
		LOAD(m_ChannelColor,m_IDB_ChannelH)
	}
	else
	{
		// TRACE(_T("ERROR - failed to create CXScrollBar\n"));
		ASSERT(FALSE);
	}

	return bResult;
}

BOOL CXScrollBar::IsHorz()
{
	return m_bHorizontal;
}

int CXScrollBar::GetFitHorW()
{
	if (m_bHorizontal)
		return m_nBitmapHeight;
	
	return 16;
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
void CXScrollBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	Draw();
}

///////////////////////////////////////////////////////////////////////////////
// Draw
void CXScrollBar::Draw()
{
	if (m_bHorizontal)
	{
		if( m_clrFaceCheme == RGB(255,255,255) )
			DrawHorizontal();
		else
		{
			DrawHorizontalExt();
		}
	}
	else
	{
		if( m_clrFaceCheme == RGB(255,255,255) )
			DrawVertical();
		else
		{
			DrawVerticalExt();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// DrawHorizontal
void CXScrollBar::DrawHorizontal(CDC *pDC/*=NULL*/)
{
	// TRACE(_T("in CXScrollBar::DrawHorizontal\n"));
	CClientDC dc(this);
	if ( NULL==pDC )
	{
		pDC = &dc;
	}
	CMemDC memDC(pDC, &m_rectClient);

	CBrush brushFrame(FRAME_COLOR);

	CDC bitmapDC;
	bitmapDC.CreateCompatibleDC(pDC);
	CBitmap bitmap;
	
	// =====  draw left arrow  =====
	CBitmap* pOldBitmap;
	CRect rectLeftArrow;
	if (-1!=m_IDB_LeftArrowH)
	{
		if(m_bUserAppRes == FALSE)
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			VERIFY(bitmap.LoadBitmap(/*IDB_HORIZONTAL_SCROLLBAR_LEFTARROW*/m_IDB_LeftArrowH));
		}
		else
		{
			VERIFY(bitmap.LoadBitmap(/*IDB_HORIZONTAL_SCROLLBAR_LEFTARROW*/m_IDB_LeftArrowH));
		}

		pOldBitmap = bitmapDC.SelectObject(&bitmap);
		
		//
		BITMAP bmpLeftArrow;
		bitmap.GetBitmap(&bmpLeftArrow);
		
		//
		rectLeftArrow.left = m_rectClient.left;
		rectLeftArrow.top = m_rectClient.top;
		rectLeftArrow.right = m_rectClient.left + bmpLeftArrow.bmWidth;
		rectLeftArrow.bottom = m_rectClient.bottom;
		
		memDC.StretchBlt(rectLeftArrow.left, rectLeftArrow.top,
			rectLeftArrow.Width(), rectLeftArrow.Height(),
			&bitmapDC, 0, 0, bmpLeftArrow.bmWidth, bmpLeftArrow.bmHeight, SRCCOPY);
		
		memDC.FrameRect(&rectLeftArrow, &brushFrame);
		
		
		
		if (pOldBitmap)
			bitmapDC.SelectObject(pOldBitmap);
		if (bitmap.GetSafeHandle())
			bitmap.DeleteObject();
		pOldBitmap = NULL;
		m_rectLeftArrow = rectLeftArrow;
	}
	else
	{
		rectLeftArrow.left = m_rectClient.left;
		rectLeftArrow.top = m_rectClient.top;
		rectLeftArrow.right = m_rectClient.left;
		rectLeftArrow.bottom = m_rectClient.bottom;
		m_rectLeftArrow = rectLeftArrow;
	}
	if(m_bUserAppRes == FALSE)
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		VERIFY(bitmap.LoadBitmap(/*IDB_HORIZONTAL_SCROLLBAR_CHANNEL*/m_IDB_ChannelH));
	}
	else
	{
		VERIFY(bitmap.LoadBitmap(/*IDB_HORIZONTAL_SCROLLBAR_CHANNEL*/m_IDB_ChannelH));
	}

	pOldBitmap = bitmapDC.SelectObject(&bitmap);

	BITMAP bmpChannel;
	bitmap.GetBitmap(&bmpChannel);
	
	CRect rectChannel(m_rectClient.left + rectLeftArrow.Width(), m_rectClient.top,
		m_rectClient.right - rectLeftArrow.Width(), m_rectClient.bottom);
	
	memDC.StretchBlt(rectChannel.left, rectChannel.top,
		rectChannel.Width(), rectChannel.Height(),
		&bitmapDC, 0, 0, bmpChannel.bmWidth, bmpChannel.bmHeight, SRCCOPY);
	
	if (pOldBitmap)
		bitmapDC.SelectObject(pOldBitmap);
	if (bitmap.GetSafeHandle())
		bitmap.DeleteObject();
	pOldBitmap = NULL;

	// =====  draw right arrow  =====
	CRect rectRightArrow;
	if (-1!=m_IDB_RightArrowH)
	{
		if(m_bUserAppRes == FALSE)
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			VERIFY(bitmap.LoadBitmap(/*IDB_HORIZONTAL_SCROLLBAR_RIGHTARROW*/m_IDB_RightArrowH));
		}
		else
		{
			VERIFY(bitmap.LoadBitmap(/*IDB_HORIZONTAL_SCROLLBAR_RIGHTARROW*/m_IDB_RightArrowH));
		}
		pOldBitmap = bitmapDC.SelectObject(&bitmap);
		
		BITMAP bmpRightArrow;
		bitmap.GetBitmap(&bmpRightArrow);
		
		rectRightArrow.left = m_rectClient.right - bmpRightArrow.bmWidth;
		rectRightArrow.top = m_rectClient.top;
		rectRightArrow.right = m_rectClient.right;
		rectRightArrow.bottom = m_rectClient.bottom;
		
		memDC.StretchBlt(rectRightArrow.left, rectRightArrow.top,
			rectRightArrow.Width(), rectRightArrow.Height(),
			&bitmapDC, 0, 0, bmpRightArrow.bmWidth, bmpRightArrow.bmHeight, SRCCOPY);
		
		memDC.FrameRect(&rectRightArrow, &brushFrame);
		
		if (pOldBitmap)
			bitmapDC.SelectObject(pOldBitmap);
		if (bitmap.GetSafeHandle())
			bitmap.DeleteObject();
		pOldBitmap = NULL;
	}
	else
	{
		rectRightArrow.left = m_rectClient.right;
		rectRightArrow.top = m_rectClient.top;
		rectRightArrow.right = m_rectClient.right;
		rectRightArrow.bottom = m_rectClient.bottom;
	}
	m_rectRightArrow = rectRightArrow;

	// If there is nothing to scroll then don't show the thumb
	if (m_nRange)
	{
		// =====  draw thumb  =====
		if(m_bUserAppRes == FALSE)
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			if (m_bThumbColor)
				VERIFY(bitmap.LoadBitmap(/*IDB_HORIZONTAL_SCROLLBAR_THUMB*/m_IDB_ThumbH));
			else
				VERIFY(bitmap.LoadBitmap(/*IDB_HORIZONTAL_SCROLLBAR_THUMB_NO_COLOR*/m_IDB_ThumbNoColorH));
		}
		else
		{
			if (m_bThumbColor)
				VERIFY(bitmap.LoadBitmap(/*IDB_HORIZONTAL_SCROLLBAR_THUMB*/m_IDB_ThumbH));
			else
				VERIFY(bitmap.LoadBitmap(/*IDB_HORIZONTAL_SCROLLBAR_THUMB_NO_COLOR*/m_IDB_ThumbNoColorH));
		}
	

		pOldBitmap = bitmapDC.SelectObject(&bitmap);
		
		BITMAP bmpThumb;
		bitmap.GetBitmap(&bmpThumb);

		if ( -1 == m_nThumbLeft )
		{
			m_nThumbLeft = m_rectLeftArrow.right;
		}

		m_rectThumb.left   = m_rectClient.left + m_nThumbLeft;
		m_rectThumb.right  = m_rectThumb.left + bmpThumb.bmWidth;
		m_rectThumb.top    = m_rectClient.top;
		m_rectThumb.bottom = m_rectThumb.top + m_rectClient.Height();

		// add desired color to thumb
		memDC.StretchBlt(m_rectThumb.left, m_rectThumb.top,
			m_rectThumb.Width(), m_rectThumb.Height(),
			&bitmapDC, 0, 0, bmpThumb.bmWidth, bmpThumb.bmHeight, SRCCOPY);

		if (pOldBitmap)
			bitmapDC.SelectObject(pOldBitmap);
		if (bitmap.GetSafeHandle())
			bitmap.DeleteObject();
		pOldBitmap = NULL;
	}
	else
	{
		m_rectThumb = CRect(-1,-1,-1,-1);
	}

	if ((-1==m_IDB_LeftArrowH) && (-1==m_IDB_RightArrowH))
	{
		CPen * pOldpen,penBoard;
		penBoard.CreatePen(PS_SOLID, 1, m_clrBorder);
		pOldpen = memDC.SelectObject(&penBoard);
		memDC.MoveTo(m_rectClient.left, m_rectClient.top);
		memDC.LineTo(m_rectClient.left, m_rectClient.bottom-1);
		memDC.MoveTo(m_rectClient.left, m_rectClient.bottom-1);
		memDC.LineTo(m_rectClient.right, m_rectClient.bottom-1);
		if (m_bHorizontal)
		{
			memDC.MoveTo(m_rectClient.right-1, m_rectClient.bottom);
			memDC.LineTo(m_rectClient.right-1, m_rectClient.top-1);
		}
		memDC.SelectObject(pOldpen);
		penBoard.DeleteObject();
	}
	else
	{
		memDC.FrameRect(&m_rectClient, &brushFrame);
	}
	
	bitmapDC.DeleteDC();
}

COLORREF   CXScrollBar::GetMarkColor(const COLORREF &clr)
{
	if( GetRValue(clr) >190 || GetGValue(clr)>190 || GetBValue(clr)>190)
		return RGB(0,0,0);
	else return RGB(255,255,255);
}

void CXScrollBar::DrawHorizontalExt(CDC *pDC/*=NULL*/)
{
	CClientDC dc(this);
	if ( NULL==pDC )
	{
		pDC = &dc;
	}
	CMemDC memDC(pDC, &m_rectClient);

	CBrush brushFrame(FRAME_COLOR);
	//CDIBSectionLite::GetMarkColor(FRAME_COLOR,m_clrFaceCheme,m_nAlphaDefine);

	//CDC bitmapDC;
	//bitmapDC.CreateCompatibleDC(&dc);
	//CBitmap bitmap;

	CDIBSectionLite DrawPicWhite;
	CDIBSectionLite DrawPicColor;
	// =====  draw left arrow  =====
	ASSERT( m_LeftArrow.IsValid() );

	m_FaceChemeColor.Copy( m_LeftArrow );
	m_FaceChemeColor.MakeGray();

	DrawPicWhite.Copy(m_LeftArrow);
	if( m_clrFaceCheme)
	DrawPicWhite.MarkColor(GetMarkColor(m_clrFaceCheme));

	DrawPicColor.Copy(m_LeftArrow);
	DrawPicColor.MarkColor( m_clrFaceCheme );

	DrawPicWhite.CompoundDIB(0,0,&DrawPicColor,&m_FaceChemeColor);
	//
	int nArrowWidth = m_LeftArrow.GetWidth();
	CRect rectLeftArrow(m_rectClient.left, m_rectClient.top,
		m_rectClient.left + nArrowWidth, m_rectClient.bottom);

	DrawPicColor.Stretch(&memDC,rectLeftArrow.TopLeft(),CSize(rectLeftArrow.Width(),rectLeftArrow.Height()));
	memDC.FrameRect(&rectLeftArrow, &brushFrame);

	

	m_rectLeftArrow = rectLeftArrow;

	
	// =====  draw channel  =====
	ASSERT( m_ChannelColor.IsValid() );
	
	m_FaceChemeColor.Copy( m_ChannelColor );
	m_FaceChemeColor.MakeGray();
	
	DrawPicWhite.Copy(m_ChannelColor);
	DrawPicWhite.MarkColor(GetMarkColor(m_clrFaceCheme));
	
	DrawPicColor.Copy(m_ChannelColor);
	DrawPicColor.MarkColor( m_clrFaceCheme );
	
	DrawPicWhite.CompoundDIB(0,0,&DrawPicColor,&m_FaceChemeColor);

	CRect rectChannel(m_rectClient.left + rectLeftArrow.Width(), m_rectClient.top,
		m_rectClient.right - rectLeftArrow.Width(), m_rectClient.bottom);
	
	DrawPicColor.Stretch(&memDC,rectChannel.TopLeft(),CSize(rectChannel.Width(),rectChannel.Height()));
	
	// =====  draw right arrow  =====
	ASSERT( m_RightArrow.IsValid() );

	m_FaceChemeColor.Copy( m_RightArrow );
	m_FaceChemeColor.MakeGray();
	
	DrawPicWhite.Copy(m_RightArrow);
	DrawPicWhite.MarkColor(GetMarkColor(m_clrFaceCheme));
	
	DrawPicColor.Copy(m_RightArrow);
	DrawPicColor.MarkColor( m_clrFaceCheme );
	
	DrawPicWhite.CompoundDIB(0,0,&DrawPicColor,&m_FaceChemeColor);

	CRect rectRightArrow(m_rectClient.right - m_RightArrow.GetWidth(), m_rectClient.top,
		m_rectClient.right, m_rectClient.bottom);

	DrawPicColor.Stretch(&memDC,rectRightArrow.TopLeft(),CSize(rectRightArrow.Width(),rectRightArrow.Height()));

	memDC.FrameRect(&rectRightArrow, &brushFrame);

	m_rectRightArrow = rectRightArrow;

	// If there is nothing to scroll then don't show the thumb
	if (m_nRange)
	{
		// =====  draw thumb  =====

		if ( -1 == m_nThumbLeft )
		{
			m_nThumbLeft = m_rectLeftArrow.right;
		}
		
		m_rectThumb.left   = m_rectClient.left + m_nThumbLeft;
		m_rectThumb.top    = m_rectClient.top;
		m_rectThumb.bottom = m_rectThumb.top + m_rectClient.Height();

		if (m_bThumbColor)
		{
			ASSERT( m_ThumbHColor.IsValid() );
			m_FaceChemeColor.Copy( m_ThumbHColor );
			m_FaceChemeColor.MakeGray();
			
			DrawPicWhite.Copy(m_ThumbHColor);
			DrawPicWhite.MarkColor(GetMarkColor(m_clrFaceCheme));
			
			DrawPicColor.Copy(m_ThumbHColor);
			DrawPicColor.MarkColor( m_clrFaceCheme );
			
			DrawPicWhite.CompoundDIB(0,0,&DrawPicColor,&m_FaceChemeColor);
			m_rectThumb.right  = m_rectThumb.left + m_ThumbHColor.GetWidth();
			DrawPicColor.Stretch(&memDC,m_rectThumb.TopLeft(),CSize(m_rectThumb.Width(),m_rectThumb.Height()));
		}
		else
		{
			ASSERT( m_ThumbNoColor.IsValid() );
			m_FaceChemeColor.Copy( m_ThumbNoColor );
			m_FaceChemeColor.MakeGray();
			
			DrawPicWhite.Copy(m_ThumbNoColor);
			DrawPicWhite.MarkColor(GetMarkColor(m_clrFaceCheme));
			
			DrawPicColor.Copy(m_ThumbNoColor);
			DrawPicColor.MarkColor( m_clrFaceCheme );
			
			DrawPicWhite.CompoundDIB(0,0,&DrawPicColor,&m_FaceChemeColor);
			m_rectThumb.right  = m_rectThumb.left + m_ThumbNoColor.GetWidth();
			DrawPicColor.Stretch(&memDC,m_rectThumb.TopLeft(),CSize(m_rectThumb.Width(),m_rectThumb.Height()));
		}
	}
	else
	{
		m_rectThumb = CRect(-1,-1,-1,-1);
	}

	memDC.FrameRect(&m_rectClient, &brushFrame);
}
///////////////////////////////////////////////////////////////////////////////
// DrawVertical
void CXScrollBar::DrawVertical()
{
// xl 20120823 新增竖直绘制方法
// TRACE(_T("in CXScrollBar::DrawHorizontal\n"));

	// xl 20121026 构造虚拟的水平绘制环境，调用水平绘制
	CClientDC dc(this);
	CMemDC memDC(&dc, &m_rectClient);
	CDC rotateDC;
	CBitmap roateBmp;
	rotateDC.CreateCompatibleDC(&dc);
	roateBmp.CreateCompatibleBitmap(&dc, m_rectClient.Width(), m_rectClient.Height());
	rotateDC.SelectObject(&roateBmp);
	
	// 旋转
	XFORM stRotate= {0};
	stRotate.eDx = 0.0f;
	stRotate.eDy = 0.0f;
	stRotate.eM11 = 0.0f;
	stRotate.eM12 = 1.0f;
	stRotate.eM21 = 1.0f;
	stRotate.eM22 = 0.0f;
	
	BOOL bR;
	SetGraphicsMode(rotateDC.GetSafeHdc(), GM_ADVANCED);
	bR = SetWorldTransform(rotateDC.GetSafeHdc(), &stRotate);
	if ( !bR )
	{
		DWORD dw = GetLastError();
		dw = dw;
	}

	CRect rcClientOld = m_rectClient;
	MyRotateRect(rcClientOld, m_rectClient);

	CRect rcLeftArrowOld = m_rectLeftArrow;
	MyRotateRect(rcLeftArrowOld, m_rectLeftArrow);

	CRect rcRightArrowOld = m_rectRightArrow;
	MyRotateRect(rcRightArrowOld, m_rectRightArrow);

	m_nThumbLeft = m_nThumbTop;
	
	CRect rcThumbOld = m_rectThumb;
	MyRotateRect(rcThumbOld, m_rectThumb);

	// 调用水平绘制
	DrawHorizontal(&rotateDC);

	// 还原
	CRect rcThumbNew = m_rectThumb;
	MyRotateRect(rcThumbNew, m_rectThumb);

	m_nThumbTop = m_nThumbLeft;

	CRect rcRightArrowNew = m_rectRightArrow;
	MyRotateRect(rcRightArrowNew, m_rectRightArrow);

	CRect rcLeftArrowNew = m_rectLeftArrow;
	MyRotateRect(rcLeftArrowNew, m_rectLeftArrow);

	m_rectClient = rcClientOld;

	stRotate.eDx = 0.0f;
	stRotate.eDy = 0.0f;
	stRotate.eM11 = 1.0f;
	stRotate.eM12 = 0.0f;
	stRotate.eM21 = 0.0f;
	stRotate.eM22 = 1.0f;
	
	CBrush brushFrame(FRAME_COLOR);

	bR = SetWorldTransform(rotateDC.GetSafeHdc(), &stRotate);
	SetGraphicsMode(rotateDC.GetSafeHdc(), GM_COMPATIBLE);
	BOOL bD = /*dc*/memDC.StretchBlt(m_rectClient.left, m_rectClient.top, m_rectClient.Width(), m_rectClient.Height(),
		&rotateDC, 0, 0, m_rectClient.Width(), m_rectClient.Height(), SRCCOPY);

	if ((-1==m_IDB_LeftArrowH) && (-1==m_IDB_RightArrowH))
	{
	}
	else
	{
		memDC.FrameRect(&m_rectClient, &brushFrame);
	}

	if ( !bD )
	{
		DWORD dw = GetLastError();
		dw = dw;
	}
	if (roateBmp.GetSafeHandle())
		roateBmp.DeleteObject();

	rotateDC.DeleteDC();
 }

void CXScrollBar::DrawVerticalExt()
{
	// xl 20121026 构造虚拟的水平绘制环境，调用水平绘制
	CClientDC dc(this);
	CDC rotateDC;
	CBitmap roateBmp;
	rotateDC.CreateCompatibleDC(&dc);
	roateBmp.CreateCompatibleBitmap(&dc, m_rectClient.Width(), m_rectClient.Height());
	rotateDC.SelectObject(&roateBmp);
	// 旋转
	XFORM stRotate= {0};
	stRotate.eDx = 0.0f;
	stRotate.eDy = 0.0f;
	stRotate.eM11 = 0.0f;
	stRotate.eM12 = 1.0f;
	stRotate.eM21 = 1.0f;
	stRotate.eM22 = 0.0f;
	
	BOOL bR;
	SetGraphicsMode(rotateDC.GetSafeHdc(), GM_ADVANCED);
	bR = SetWorldTransform(rotateDC.GetSafeHdc(), &stRotate);
	if ( !bR )
	{
		DWORD dw = GetLastError();
		dw = dw;
	}
	
	CRect rcClientOld = m_rectClient;
	MyRotateRect(rcClientOld, m_rectClient);
	
	CRect rcLeftArrowOld = m_rectLeftArrow;
	MyRotateRect(rcLeftArrowOld, m_rectLeftArrow);
	
	CRect rcRightArrowOld = m_rectRightArrow;
	MyRotateRect(rcRightArrowOld, m_rectRightArrow);
	
	m_nThumbLeft = m_nThumbTop;
	
	CRect rcThumbOld = m_rectThumb;
	MyRotateRect(rcThumbOld, m_rectThumb);
	
	// 调用水平绘制
	DrawHorizontalExt(&rotateDC);
	
	// 还原
	CRect rcThumbNew = m_rectThumb;
	MyRotateRect(rcThumbNew, m_rectThumb);
	
	m_nThumbTop = m_nThumbLeft;
	
	CRect rcRightArrowNew = m_rectRightArrow;
	MyRotateRect(rcRightArrowNew, m_rectRightArrow);
	
	CRect rcLeftArrowNew = m_rectLeftArrow;
	MyRotateRect(rcLeftArrowNew, m_rectLeftArrow);
	
	m_rectClient = rcClientOld;
	
	stRotate.eDx = 0.0f;
	stRotate.eDy = 0.0f;
	stRotate.eM11 = 1.0f;
	stRotate.eM12 = 0.0f;
	stRotate.eM21 = 0.0f;
	stRotate.eM22 = 1.0f;
	
	bR = SetWorldTransform(rotateDC.GetSafeHdc(), &stRotate);
	SetGraphicsMode(rotateDC.GetSafeHdc(), GM_COMPATIBLE);
	BOOL bD = dc.StretchBlt(m_rectClient.left, m_rectClient.top, m_rectClient.Width(), m_rectClient.Height(),
		&rotateDC, 0, 0, m_rectClient.Width(), m_rectClient.Height(), SRCCOPY);
	if ( !bD )
	{
		DWORD dw = GetLastError();
		dw = dw;
	}

	if (roateBmp.GetSafeHandle())
		roateBmp.DeleteObject();

	rotateDC.DeleteDC();
}
///////////////////////////////////////////////////////////////////////////////
// OnLButtonDown
void CXScrollBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	if (m_bHorizontal)
	{		
		if (m_rectThumb.PtInRect(point))
		{
			m_bMouseDown = TRUE;
		}
		else if (m_rectRightArrow.PtInRect(point))
		{
			m_bMouseDownArrowRight = TRUE;
			SetTimer(TIMER_LBUTTON_PRESSED, 150, NULL);
		}
		else if (m_rectLeftArrow.PtInRect(point))
		{
			m_bMouseDownArrowLeft = TRUE;
			SetTimer(TIMER_LBUTTON_PRESSED, 150, NULL);
		}
		else	// button down in channel
		{
			m_nThumbLeft = point.x - m_rectThumb.Width() / 2;
			SetPositionFromThumb();
			Draw();

			FireMsg(WM_HSCROLL, MAKELONG(SB_THUMBTRACK, m_nPos), (LPARAM)m_hWnd);
		}
	}
	else
	{
		CRect rectUpArrow(m_rectLeftArrow);
		CRect rectDownArrow(m_rectRightArrow);
 		CRect rectThumb(m_rectThumb);

		if (rectThumb.PtInRect(point))
		{
			m_bMouseDown = TRUE;
			m_nThumbPos = point.y - m_rectThumb.top;
		}
		else if (rectDownArrow.PtInRect(point))
		{
			m_bMouseDownArrowDown = TRUE;
			SetTimer(TIMER_LBUTTON_PRESSED, 150, NULL);
		}
		else if (rectUpArrow.PtInRect(point))
		{
			m_bMouseDownArrowUp = TRUE;
			SetTimer(TIMER_LBUTTON_PRESSED, 150, NULL);
		}
		else	// button down in channel
		{
			m_nThumbTop = point.y - m_nBitmapHeight / 2;
			if(m_nThumbTop + m_rectThumb.Height() > m_rectRightArrow.top)
			{
				m_nThumbTop = m_rectRightArrow.top - m_rectThumb.Height();
			}
			SetPositionFromThumb();
			Draw();

			FireMsg(WM_VSCROLL, MAKELONG(SB_THUMBTRACK, m_nPos), (LPARAM)m_hWnd);
		}
	}

	CStatic::OnLButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnLButtonUp
void CXScrollBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	UpdateThumbPosition();
	KillTimer(1);
	ReleaseCapture();

	if (m_bHorizontal)
	{	
		CRect rectThumb(m_nThumbLeft, 0, m_nThumbLeft + m_rectThumb.Width(), 
		m_rectClient.Height());

		if (m_rectLeftArrow.PtInRect(point))
		{
			ScrollLeft();
		}
		else if (m_rectRightArrow.PtInRect(point))
		{
			ScrollRight();
		}
		else if (rectThumb.PtInRect(point))
		{
			m_bThumbHover = TRUE;
			Invalidate();
			SetTimer(TIMER_MOUSE_OVER_THUMB, 50, NULL);
		}

		m_bMouseDownArrowLeft = FALSE;
		m_bMouseDownArrowRight = FALSE;
	}
	else
	{
		CRect rectUpArrow(0, 0, m_rectClient.Width(), m_nBitmapHeight);
		CRect rectDownArrow(0, m_rectClient.Height() - m_nBitmapHeight, m_rectClient.Width(), m_rectClient.Height());
		CRect rectThumb(0, m_nThumbTop, m_rectClient.Width(), m_nThumbTop + m_nBitmapHeight);

		if (rectUpArrow.PtInRect(point))
		{
			ScrollUp();
		}
		else if (rectDownArrow.PtInRect(point))
		{
			ScrollDown();
		}
		else if (rectThumb.PtInRect(point))
		{
			m_bThumbHover = TRUE;
			Invalidate();
			SetTimer(TIMER_MOUSE_OVER_THUMB, 50, NULL);
		}

		m_bMouseDownArrowUp = FALSE;
		m_bMouseDownArrowDown = FALSE;
	}

	m_bMouseDown = FALSE;
	m_bDragging = FALSE;

	CStatic::OnLButtonUp(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnMouseMove
void CXScrollBar::OnMouseMove(UINT nFlags, CPoint point)
{
	BOOL bOldThumbHover = m_bThumbHover;
	m_bThumbHover = FALSE;

	if (m_rectThumb.PtInRect(point))
		m_bThumbHover = TRUE;

	if (m_bMouseDown)
		m_bDragging = TRUE;

	if (m_bDragging)
	{
		if (m_bHorizontal)
		{
			m_nThumbLeft = point.x - m_rectThumb.Width() / 2;

			SetPositionFromThumb();

			FireMsg(WM_HSCROLL, MAKELONG(SB_THUMBTRACK, m_nPos), (LPARAM)m_hWnd);
		}
		else
		{
			m_nThumbTop = point.y - m_nThumbPos;
			if(m_nThumbTop + m_rectThumb.Height() > m_rectRightArrow.top)
			{
				m_nThumbTop = m_rectRightArrow.top - m_rectThumb.Height();
			}
			SetPositionFromThumb();
			FireMsg(WM_VSCROLL, MAKELONG(SB_THUMBTRACK, m_nPos), (LPARAM)m_hWnd);
		}

		Draw();
	}

	if (bOldThumbHover != m_bThumbHover)
	{
		Invalidate();
		SetTimer(TIMER_MOUSE_OVER_THUMB, 50, NULL);
	}

	CStatic::OnMouseMove(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnTimer
void CXScrollBar::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == TIMER_MOUSE_OVER_BUTTON)	// mouse is in an arrow button,
												// and left button is down
	{
		if (m_bMouseDownArrowLeft)
			ScrollLeft();
		if (m_bMouseDownArrowRight)
			ScrollRight();
		if (m_bMouseDownArrowUp)
			ScrollUp();
		if (m_bMouseDownArrowDown)
			ScrollDown();
	}
	else if (nIDEvent == TIMER_LBUTTON_PRESSED)	// mouse is in an arrow button,
												// and left button has just been pressed
	{
		KillTimer(nIDEvent);

		if (m_bMouseDownArrowLeft || 
			m_bMouseDownArrowRight || 
			m_bMouseDownArrowUp || 
			m_bMouseDownArrowDown)
		{
			// debounce left click
			SetTimer(TIMER_MOUSE_OVER_BUTTON, 100, NULL);
		}
	}
	else if (nIDEvent == TIMER_MOUSE_OVER_THUMB)	// mouse is over thumb
	{
		CPoint point;
		::GetCursorPos(&point);
		ScreenToClient(&point);

		if (!m_rectThumb.PtInRect(point))
		{
			// no longer over thumb, restore thumb color
			m_bThumbHover = FALSE;
			KillTimer(nIDEvent);
			::SetCursor(::LoadCursor(NULL, IDC_ARROW));
			Invalidate();
		}
	}

	CStatic::OnTimer(nIDEvent);
}

///////////////////////////////////////////////////////////////////////////////
// ScrollLeft
void CXScrollBar::ScrollLeft()
{
	if (m_nPos > 0)
		m_nPos--;
	FireMsg(WM_HSCROLL, MAKELONG(SB_LINELEFT,0), (LPARAM)m_hWnd);
// 	CWnd *pOwner = GetOwner();
// 	if (pOwner && ::IsWindow(pOwner->m_hWnd))
// 		pOwner->SendMessage(WM_HSCROLL, MAKELONG(SB_LINELEFT,0), (LPARAM)m_hWnd);
	UpdateThumbPosition();
}

///////////////////////////////////////////////////////////////////////////////
// ScrollRight
void CXScrollBar::ScrollRight()
{
	if (m_nPos < m_nRange)
		m_nPos++;
	FireMsg(WM_HSCROLL, MAKELONG(SB_LINERIGHT,0), (LPARAM)m_hWnd);
// 	CWnd *pOwner = GetOwner();
// 	if (pOwner && ::IsWindow(pOwner->m_hWnd))
// 		pOwner->SendMessage(WM_HSCROLL, MAKELONG(SB_LINERIGHT,0), (LPARAM)m_hWnd);
	UpdateThumbPosition();
}

///////////////////////////////////////////////////////////////////////////////
// ScrollUp
void CXScrollBar::ScrollUp()
{
	if (m_nPos > 0)
		m_nPos--;
	FireMsg(WM_VSCROLL, MAKELONG(SB_LINEUP,0), (LPARAM)m_hWnd);
// 	CWnd *pOwner = GetOwner();
// 	if (pOwner && ::IsWindow(pOwner->m_hWnd))
// 		pOwner->SendMessage(WM_VSCROLL, MAKELONG(SB_LINEUP,0), (LPARAM)m_hWnd);
	UpdateThumbPosition();
}

///////////////////////////////////////////////////////////////////////////////
// ScrollDown
void CXScrollBar::ScrollDown()
{
	if (m_nPos < m_nRange)
		m_nPos++;
	FireMsg(WM_VSCROLL, MAKELONG(SB_LINEDOWN,0), (LPARAM)m_hWnd);
// 	CWnd *pOwner = GetOwner();
// 	if (pOwner && ::IsWindow(pOwner->m_hWnd))
// 		pOwner->SendMessage(WM_VSCROLL, MAKELONG(SB_LINEDOWN,0), (LPARAM)m_hWnd);
	UpdateThumbPosition();
}

///////////////////////////////////////////////////////////////////////////////
// SetPositionFromThumb
void CXScrollBar::SetPositionFromThumb()
{
	double  dMax = 0, dInterval = 0, dPos = 0;

	LimitThumbPosition();
	dMax = m_nRange;

	if (m_bHorizontal)
	{
 		if ( m_nThumbLeft >= (m_rectRightArrow.left - m_rectThumb.Width()) )
 		{
 			m_nPos = m_nMaxPos;
 		}
 		else if ( m_nThumbLeft <= m_rectLeftArrow.right )
 		{
 			m_nPos = m_nMinPos;
 		}
 		else
		{
			double dWidth = m_rectClient.Width() - (m_rectLeftArrow.Width() + m_rectRightArrow.Width());
			dInterval	  = (m_nMaxPos - m_nMinPos) / dWidth;
			// modify by tangad 去掉滚动条本身的宽度，可修改回来
			dPos = (int)(m_nThumbLeft) * dInterval;
			dPos += m_nMinPos;

			m_nPos = (int)dPos;
		}	
		
		return;
	}
	else
	{
		if ( m_nThumbTop >= (m_rectRightArrow.top - m_rectThumb.Height()) )
		{
			m_nPos = m_nMaxPos;
		}
		else if ( m_nThumbTop <= m_rectLeftArrow.bottom )
		{
			m_nPos = m_nMinPos;
		}
		else
		{
			if(m_bIsStone)
			{
				double dHeight = m_rectClient.Height() - 
								(m_rectLeftArrow.Height() + m_rectRightArrow.Height() + m_rectThumb.Height());
				dPos = (m_nThumbTop - m_rectLeftArrow.bottom) * (m_nMaxPos - m_nMinPos) / dHeight;
			}
			else
			{
				double dHeight = m_rectClient.Height() - (m_rectLeftArrow.Height() + m_rectRightArrow.Height());
				dInterval	  = (m_nMaxPos - m_nMinPos) / dHeight;
			dPos = (int)(m_nThumbTop + m_rectThumb.Height() / 2) * dInterval;
			}

			dPos += m_nMinPos;
			m_nPos = (int)dPos;
		}	
		
		return;
	}

	m_nPos = (int) (dPos);
	if (m_nPos < 0)
		m_nPos = 0;
	if (m_nPos > m_nRange)
		m_nPos = m_nRange;
}
///////////////////////////////////////////////////////////////////////////////
// UpdateThumbPosition
void CXScrollBar::UpdateThumbPosition()
{
	if ( m_rectLeftArrow.Width() <= 0 || m_rectRightArrow.Width() <= 0 || m_rectThumb.Width() <= 0 )
	{
		//return;
	}

	double  dMax = 0, dInterval = 0, dPos = 0;

	dMax = m_nRange;
	dPos = m_nPos;
	int iStep = m_nMaxPos - m_nMinPos;
	if (m_bHorizontal)
	{
 		if ( m_nPos <= m_nMinPos )
 		{
 			m_nThumbLeft = m_rectLeftArrow.right;
 		}
 		else if ( m_nPos >= m_nMaxPos )
 		{
 			m_nThumbLeft = m_rectRightArrow.left - m_rectThumb.Width();
 		}
 		else
		{
			double dWidth = m_rectClient.Width() - (m_rectLeftArrow.Width() + m_rectRightArrow.Width());
			
			if (0 != iStep )
			{
				dInterval	 = dWidth * (dPos - m_nMinPos) / iStep;
			}		
			// modify by tangad 去掉滚动条本身的宽度，可修改回来
			m_nThumbLeft  = ((int)dInterval);
		}		
	}
	else
	{
		if ( m_nPos <= m_nMinPos )
		{
			m_nThumbTop = m_rectLeftArrow.bottom;
		}
		else if ( m_nPos >= m_nMaxPos )
		{
			m_nThumbTop = m_rectRightArrow.top - m_rectThumb.Height();
		}
		else
		{	// 因为觉得原来的代码对滚动条滑画的位置计算有问题,为了不影响原来的使用,所以写一个标识,控制使用新的计算方法
			if(m_bIsStone)
			{
				double dWidth = m_rectClient.Height() - 
					(m_rectLeftArrow.Height() + m_rectRightArrow.Height() + m_rectThumb.Height());			
				if (0 != iStep)
				{
					dInterval = dWidth * (dPos - m_nMinPos) / iStep;
				}
				
				m_nThumbTop  = int(m_rectLeftArrow.bottom + dInterval);
				if(m_nThumbTop + m_rectThumb.Height() >= m_rectRightArrow.top)
				{
					m_nThumbTop = m_rectRightArrow.top - m_rectThumb.Height();
				}
			}
			else	// 这里原来定理代码
			{
				double dWidth = m_rectClient.Height() - (m_rectLeftArrow.Height() + m_rectRightArrow.Height());				
				if (0 != iStep )
				{
					dInterval	  = dWidth * (dPos - m_nMinPos) / iStep;
				}				
				m_nThumbTop   = ((int)dInterval - m_rectThumb.Height() / 2);
			}
		}
	}

	LimitThumbPosition();

	Draw();
}

///////////////////////////////////////////////////////////////////////////////
// LimitThumbPosition
void CXScrollBar::LimitThumbPosition()
{
	if (m_bHorizontal)
	{
		if ((m_nThumbLeft + m_rectThumb.Width()) > (m_rectClient.right - m_rectRightArrow.Width()))
			m_nThumbLeft = m_rectClient.right - (m_rectThumb.Width() + m_rectRightArrow.Width());

		if (m_nThumbLeft < (m_rectClient.left + m_rectLeftArrow.Width()))
			m_nThumbLeft = m_rectClient.left + m_rectLeftArrow.Width();
	}
	else
	{
		//m_nBitmapHeight = m_rectRightArrow.Width();
		
		if(m_IDB_RightArrowH == -1)
		{
			m_nBitmapHeight = 0;
		}
		else
		{
			m_nBitmapHeight = m_RightArrow.GetWidth();
		}
		if ((m_nThumbTop + m_nBitmapHeight) > (m_rectClient.Height() - m_nBitmapHeight))
		{
			m_nThumbTop = m_rectClient.Height() - 2*m_nBitmapHeight;
		}

		//m_nBitmapHeight = m_rectLeftArrow.Width();
		
		if(m_IDB_LeftArrowH == -1)
		{
			m_nBitmapHeight = 0;
		}
		else
		{
			m_nBitmapHeight = m_LeftArrow.GetWidth();
		}
		if (m_nThumbTop < (m_rectClient.top + m_nBitmapHeight))
		{
			m_nThumbTop = m_rectClient.top + m_nBitmapHeight;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// SetScrollRange
void CXScrollBar::SetScrollRange(int nMinPos,
								 int nMaxPos,
								 BOOL bRedraw /*= TRUE*/)
{
	m_nMinPos = nMinPos;
	m_nMaxPos = nMaxPos;
	if (m_nMinPos < m_nMaxPos)
		m_nRange = m_nMaxPos - m_nMinPos;
	else
		m_nRange = m_nMinPos - m_nMaxPos;

	if( !::IsWindow(m_hWnd) )
		return;

	if (bRedraw && ::IsWindow(m_hWnd) )
		Invalidate();

	FireListener(UM_POS_CHANGED, (WPARAM)m_hWnd, 1);
}

///////////////////////////////////////////////////////////////////////////////
// SetScrollPos
int CXScrollBar::SetScrollPos(int nPos, BOOL bRedraw /*= TRUE*/)
{
	int nOldPos = m_nPos;

	m_nPos = nPos;

	UpdateThumbPosition();

	if (bRedraw)
		Invalidate();

	FireListener(UM_POS_CHANGED, (WPARAM)m_hWnd, 0);

	return nOldPos;
}

void CXScrollBar::SetSBRect(LPRECT lpRect, BOOL bDraw)
{
	MoveWindow(lpRect,bDraw);
	GetClientRect(&m_rectClient);

	//DrawHorizontal();	// 更新各个矩形分配
	Draw();
	// 重新调节位置
	SetScrollPos(m_nPos, bDraw);
}

///////////////////////////////////////////////////////////////////////////////
// OnSetCursor
BOOL CXScrollBar::OnSetCursor(CWnd* /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/)
{
	if (m_bThumbHover && m_hCursor)
		::SetCursor(m_hCursor);
	else
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// SetDefaultCursor - borrowed from XHyperLink.cpp
void CXScrollBar::SetDefaultCursor()
{
	if (m_hCursor == NULL)				// No cursor handle - try to load one
	{
		// First try to load the Win98 / Windows 2000 hand cursor

		// TRACE(_T("loading from IDC_HAND\n"));
		m_hCursor = AfxGetApp()->LoadStandardCursor(IDC_HAND);

		if (m_hCursor == NULL)			// Still no cursor handle -
										// load the WinHelp hand cursor
		{
			// The following appeared in Paul DiLascia's Jan 1998 MSJ articles.
			// It loads a "hand" cursor from the winhlp32.exe module.

			// TRACE(_T("loading from winhlp32\n"));

			// Get the windows directory
			CString strWndDir;
			if (0 != GetWindowsDirectory(strWndDir.GetBuffer(MAX_PATH), MAX_PATH))
			{
				strWndDir += _T("\\winhlp32.exe");

				// This retrieves cursor #106 from winhlp32.exe, which is a hand pointer
				HMODULE hModule = LoadLibrary(strWndDir);
				if (hModule)
				{
					HCURSOR hHandCursor = ::LoadCursor(hModule, MAKEINTRESOURCE(106));
					if (hHandCursor)
						m_hCursor = CopyCursor(hHandCursor);
					FreeLibrary(hModule);
				}
			}			
			strWndDir.ReleaseBuffer();

		}
	}
}

BOOL CXScrollBar::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
	return CStatic::OnEraseBkgnd(pDC);
}

void CXScrollBar::AddMsgListener( HWND hwnd )
{
	if ( !IsWindow(hwnd) )
	{
		ASSERT( 0 );
		return;
	}
	
	for ( int i=0; i < m_aHwndListener.GetSize() ; i++ )
	{
		if ( m_aHwndListener[i] == hwnd )
		{
			return;
		}
	}
	m_aHwndListener.Add(hwnd);
}

void CXScrollBar::RemoveMsgListener( HWND hwnd )
{
	for ( int i=0; i < m_aHwndListener.GetSize() ; i++ )
	{
		if ( m_aHwndListener[i] == hwnd )
		{
			m_aHwndListener.RemoveAt(i);
			return;
		}
	}	
}

void CXScrollBar::FireMsg( UINT msgId, WPARAM w, LPARAM l )
{
	CWnd *pWnd = GetOwner();
	HWND hwndOwner = pWnd->GetSafeHwnd();
	if ( IsWindow(hwndOwner) )
	{
		::SendMessage(hwndOwner, msgId, w, l);
	}

	FireListener(msgId, w, l);
}

void CXScrollBar::FireListener( UINT msgId, WPARAM w, LPARAM l )
{
	HWND hwndOwner = GetOwner()->GetSafeHwnd();
	for ( int i=0; i < m_aHwndListener.GetSize() ; i++ )
	{
		if ( IsWindow(m_aHwndListener[i]) && m_aHwndListener[i] != hwndOwner )
		{
			if ( msgId == UM_POS_CHANGED )
			{
				::PostMessage(m_aHwndListener[i], msgId, w, l);
			}
			else
			{
				::SendMessage(m_aHwndListener[i], msgId, w, l);
			}
		}
		else
		{
			m_aHwndListener.RemoveAt(i);
			i--;	// 该位置的hwnd已经变化，需要重新检测
		}
	}
}

void    CXScrollBar::SetFaceChemeColor(const COLORREF &clr)
{
	if( m_clrFaceCheme != clr )
	{
		m_clrFaceCheme = clr;
		if( m_clrFaceCheme != RGB(255,255,255) )
		{
			m_FaceChemeColor.DeleteObject();
		}
		else
		{
			m_FaceChemeColor.DeleteObject();
		}
	}
}


// lcq add 动态设置滚动条各图片
void CXScrollBar::SetScrollBarLeftArrowH(const int idb)
{
	m_IDB_LeftArrowH = idb;
}

void CXScrollBar::SetScrollBarRightArrowH(const int idb)
{
	m_IDB_RightArrowH = idb;
}

void CXScrollBar::SetScrollBarChannelH(const int idb)
{
	m_IDB_ChannelH = idb;
}

void CXScrollBar::SetScrollBarThumbH(const int idb)
{
	m_IDB_ThumbH = idb;
}

void CXScrollBar::SetScrollBarThumbNoColorH(const int idb)
{
	m_IDB_ThumbNoColorH = idb;
}

void CXScrollBar::SetScrollBarDownArrowV(const int idb)
{
	m_IDB_DownArrowV = idb;
}

void CXScrollBar::SetScrollBarUpArrowV(const int idb)
{
	m_IDB_UpArrowV = idb;
}

void CXScrollBar::SetScrollBarChannelV(const int idb)
{
	m_IDB_ChannelV = idb;
}

void CXScrollBar::SetScrollBarThumbV(const int idb)
{
	m_IDB_ThumbV = idb;
}

void CXScrollBar::SetScrollBarThumbNoColorV(const int idb)
{
	m_IDB_ThumbNoColorV = idb;
}

void CXScrollBar::SetBorderColor(COLORREF clrBorder)
{
	m_clrBorder = clrBorder;
}