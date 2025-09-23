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
//     which I used as the starting point for CXScrollBarBaCai:
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
#include "memdc.h"
#include "Color.h"

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
#define FRAME_COLOR							RGB(76,85,118)	// dark gray

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
BEGIN_MESSAGE_MAP(CXScrollBarBaCai, CStatic)
	//{{AFX_MSG_MAP(CXScrollBarBaCai)
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
CXScrollBarBaCai::CXScrollBarBaCai()
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
	m_nAlphaDefine		   = 160;
}

///////////////////////////////////////////////////////////////////////////////
// dtor
CXScrollBarBaCai::~CXScrollBarBaCai()
{
	if (m_hCursor)
		DestroyCursor(m_hCursor);
	m_hCursor = NULL;
}

///////////////////////////////////////////////////////////////////////////////
//
// CreateFromStatic
//
// Purpose:     Create the CXScrollBarBaCai control from STATIC placeholder
//
// Parameters:  dwStyle    - the scroll bar抯 style. Typically this will be
//                           SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE.
//              pParentWnd - the scroll bar抯 parent window, usually a CDialog
//                           object. It must not be NULL.
//              nIdStatic  - the resource id of the placeholder STATIC
//              nId        - the resource id of the CXScrollBarBaCai control
//
// Returns:     BOOL       - TRUE = success
//
// Notes:       Hides the STATIC placeholder.  Also loads hand cursor, and
//              sets the thumb bitmap size.

BOOL CXScrollBarBaCai::CreateFromStatic(DWORD dwStyle,
								   CWnd* pParentWnd,
								   UINT nIdStatic,
								   UINT nId)
{
	// TRACE(_T("in CXScrollBarBaCai::CreateFromStatic\n"));


	m_pParent = pParentWnd;
	ASSERT(m_pParent);

	ASSERT(::IsWindow(pParentWnd->GetDlgItem(nIdStatic)->m_hWnd));

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

		// we assume that width of thumb is same as width of arrows
		if (bitmap.LoadBitmap(m_bHorizontal ? 
							  IDB_HORIZONTAL_SCROLLBAR_THUMB : 
							  IDB_VERTICAL_SCROLLBAR_THUMB))
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
		// TRACE(_T("ERROR - failed to create CXScrollBarBaCai\n"));
		ASSERT(FALSE);
	}

	return bResult;
}



BOOL CXScrollBarBaCai::Create(DWORD dwStyle,
						 CWnd* pParentWnd,
						 LPRECT lpRect,
						 UINT nId)
{
	// TRACE(_T("in CXScrollBarBaCai::Create\n"));

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

		// we assume that width of thumb is same as width of arrows
		if (bitmap.LoadBitmap(m_bHorizontal ? 
							  IDB_HORIZONTAL_SCROLLBAR_THUMB_BACAI : 
							  IDB_VERTICAL_SCROLLBAR_THUMB))
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

		BOOL bLoad = m_LeftArrow.SetBitmap(IDB_HORIZONTAL_SCROLLBAR_LEFTARROW_BACAI);
		ASSERT( bLoad );

		bLoad = m_RightArrow.SetBitmap(IDB_HORIZONTAL_SCROLLBAR_RIGHTARROW_BACAI);
		ASSERT( bLoad );

		bLoad = m_ThumbNoColor.SetBitmap(IDB_HORIZONTAL_SCROLLBAR_THUMB_NO_COLOR_BACAI);
		ASSERT( bLoad );

		bLoad = m_ThumbHColor.SetBitmap(IDB_HORIZONTAL_SCROLLBAR_THUMB_BACAI);
		ASSERT( bLoad );

		bLoad = m_ChannelColor.SetBitmap(IDB_HORIZONTAL_SCROLLBAR_CHANNEL_BACAI);
		ASSERT( bLoad );
	}
	else
	{
		// TRACE(_T("ERROR - failed to create CXScrollBarBaCai\n"));
		ASSERT(FALSE);
	}

	return bResult;
}

BOOL CXScrollBarBaCai::IsHorz()
{
	return m_bHorizontal;
}

int CXScrollBarBaCai::GetFitHorW()
{
	if (m_bHorizontal)
		return m_nBitmapHeight;
	
	return 16;
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
void CXScrollBarBaCai::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	Draw();
}

///////////////////////////////////////////////////////////////////////////////
// Draw
void CXScrollBarBaCai::Draw()
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
void CXScrollBarBaCai::DrawHorizontal(CDC *pDC/*=NULL*/)
{
	// TRACE(_T("in CXScrollBarBaCai::DrawHorizontal\n"));

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

	VERIFY(bitmap.LoadBitmap(IDB_HORIZONTAL_SCROLLBAR_LEFTARROW_BACAI));
	CBitmap* pOldBitmap = bitmapDC.SelectObject(&bitmap);

	//
	BITMAP bmpLeftArrow;
	bitmap.GetBitmap(&bmpLeftArrow);

	//
	CRect rectLeftArrow(m_rectClient.left, m_rectClient.top,
		m_rectClient.left + bmpLeftArrow.bmWidth, m_rectClient.bottom);

	memDC.StretchBlt(rectLeftArrow.left, rectLeftArrow.top,
					 rectLeftArrow.Width(), rectLeftArrow.Height(),
					 &bitmapDC, 0, 0, bmpLeftArrow.bmWidth, bmpLeftArrow.bmHeight, SRCCOPY);

	memDC.FrameRect(&rectLeftArrow, &brushFrame);

	//int nChannelStart = m_rectClient.left + bmpLeftArrow.bmWidth;
	//int nChannelWidth = m_rectClient.Width() - 2 * bmpLeftArrow.bmWidth;

	if (pOldBitmap)
		bitmapDC.SelectObject(pOldBitmap);
	if (bitmap.GetSafeHandle())
		bitmap.DeleteObject();
	pOldBitmap = NULL;

	m_rectLeftArrow = rectLeftArrow;
	
	VERIFY(bitmap.LoadBitmap(IDB_HORIZONTAL_SCROLLBAR_CHANNEL_BACAI));	
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

	VERIFY(bitmap.LoadBitmap(IDB_HORIZONTAL_SCROLLBAR_RIGHTARROW_BACAI));

	pOldBitmap = bitmapDC.SelectObject(&bitmap);
	
	BITMAP bmpRightArrow;
	bitmap.GetBitmap(&bmpRightArrow);

	CRect rectRightArrow(m_rectClient.right - bmpRightArrow.bmWidth, m_rectClient.top,
		m_rectClient.right, m_rectClient.bottom);

	memDC.StretchBlt(rectRightArrow.left, rectRightArrow.top,
					 rectRightArrow.Width(), rectRightArrow.Height(),
					 &bitmapDC, 0, 0, bmpRightArrow.bmWidth, bmpRightArrow.bmHeight, SRCCOPY);

	memDC.FrameRect(&rectRightArrow, &brushFrame);

	if (pOldBitmap)
		bitmapDC.SelectObject(pOldBitmap);
	if (bitmap.GetSafeHandle())
	{
		bitmap.DeleteObject();
	}
	pOldBitmap = NULL;

	m_rectRightArrow = rectRightArrow;

	// If there is nothing to scroll then don't show the thumb
	if (m_nRange)
	{
		// =====  draw thumb  =====

		if (m_bThumbColor)
		{
			VERIFY(bitmap.LoadBitmap(IDB_HORIZONTAL_SCROLLBAR_THUMB_BACAI));
		}
		else
		{
			VERIFY(bitmap.LoadBitmap(IDB_HORIZONTAL_SCROLLBAR_THUMB_NO_COLOR_BACAI));
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

	memDC.FrameRect(&m_rectClient, &brushFrame);
	//
	bitmapDC.DeleteDC();
}

COLORREF   CXScrollBarBaCai::GetMarkColor(const COLORREF &clr)
{
	if( GetRValue(clr) >190 || GetGValue(clr)>190 || GetBValue(clr)>190)
		return RGB(0,0,0);
	else return RGB(255,255,255);
}

void CXScrollBarBaCai::DrawHorizontalExt(CDC *pDC/*=NULL*/)
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
	CRect rectLeftArrow(m_rectClient.left, m_rectClient.top,
		m_rectClient.left + m_LeftArrow.GetWidth(), m_rectClient.bottom);

	DrawPicColor.Stretch(&memDC,rectLeftArrow.TopLeft(),CSize(rectLeftArrow.Width(),rectLeftArrow.Height()));
	memDC.FrameRect(&rectLeftArrow, &brushFrame);

	//int nChannelStart = m_rectClient.left + m_LeftArrow.GetWidth();
	//int nChannelWidth = m_rectClient.Width() - 2 * m_LeftArrow.GetWidth();

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
void CXScrollBarBaCai::DrawVertical()
{
// xl 20120823 新增竖直绘制方法
// TRACE(_T("in CXScrollBarBaCai::DrawHorizontal\n"));

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
	
	bR = SetWorldTransform(rotateDC.GetSafeHdc(), &stRotate);
	SetGraphicsMode(rotateDC.GetSafeHdc(), GM_COMPATIBLE);
	BOOL bD = dc.StretchBlt(m_rectClient.left, m_rectClient.top, m_rectClient.Width(), m_rectClient.Height(),
		&rotateDC, 0, 0, m_rectClient.Width(), m_rectClient.Height(), SRCCOPY);
	if ( !bD )
	{
		DWORD dw = GetLastError();
		dw = dw;
	}

	//
	rotateDC.DeleteDC();
	roateBmp.DeleteObject();
}

void CXScrollBarBaCai::DrawVerticalExt()
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

	rotateDC.DeleteDC();
	roateBmp.DeleteObject();
}

///////////////////////////////////////////////////////////////////////////////
// OnLButtonDown
void CXScrollBarBaCai::OnLButtonDown(UINT nFlags, CPoint point)
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
// 			CWnd *pOwner = GetOwner();
// 			if (pOwner && ::IsWindow(pOwner->m_hWnd))
// 				pOwner->SendMessage(WM_HSCROLL, MAKELONG(SB_THUMBTRACK, m_nPos),
// 				(LPARAM)m_hWnd);
		}
	}
	else
	{
		CRect rectUpArrow(m_rectLeftArrow);
		CRect rectDownArrow(m_rectRightArrow);
 		CRect rectThumb(m_rectThumb);

		

// 		CRect rectUpArrow(0, 0, m_rectClient.Width(), m_nBitmapHeight);
// 		CRect rectDownArrow(0, m_rectClient.Height() - m_nBitmapHeight, m_rectClient.Width(), m_rectClient.Height());
// 		CRect rectThumb(0, m_nThumbTop, m_rectClient.Width(), m_nThumbTop + m_nBitmapHeight);
// 
		if (rectThumb.PtInRect(point))
		{
			m_bMouseDown = TRUE;
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
			SetPositionFromThumb();
			Draw();

			FireMsg(WM_VSCROLL, MAKELONG(SB_THUMBTRACK, m_nPos), (LPARAM)m_hWnd);
// 			CWnd *pOwner = GetOwner();
// 			if (pOwner && ::IsWindow(pOwner->m_hWnd))
// 				pOwner->SendMessage(WM_VSCROLL, MAKELONG(SB_THUMBTRACK, m_nPos), 
// 				(LPARAM)m_hWnd);
		}
	}

	CStatic::OnLButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnLButtonUp
void CXScrollBarBaCai::OnLButtonUp(UINT nFlags, CPoint point)
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
void CXScrollBarBaCai::OnMouseMove(UINT nFlags, CPoint point)
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
// 			CWnd *pOwner = GetOwner();
// 			if (pOwner && ::IsWindow(pOwner->m_hWnd))
// 				pOwner->SendMessage(WM_HSCROLL, MAKELONG(SB_THUMBTRACK, m_nPos), 
// 					(LPARAM)m_hWnd);
		}
		else
		{
			m_nThumbTop = point.y - m_rectThumb.Height() / 2;
			SetPositionFromThumb();
			FireMsg(WM_VSCROLL, MAKELONG(SB_THUMBTRACK, m_nPos), (LPARAM)m_hWnd);
// 			CWnd *pOwner = GetOwner();
// 			if (pOwner && ::IsWindow(pOwner->m_hWnd))
// 				pOwner->SendMessage(WM_VSCROLL, MAKELONG(SB_THUMBTRACK, m_nPos), 
// 					(LPARAM)m_hWnd);
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
void CXScrollBarBaCai::OnTimer(UINT nIDEvent)
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
void CXScrollBarBaCai::ScrollLeft()
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
void CXScrollBarBaCai::ScrollRight()
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
void CXScrollBarBaCai::ScrollUp()
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
void CXScrollBarBaCai::ScrollDown()
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
void CXScrollBarBaCai::SetPositionFromThumb()
{
	double dMax, dInterval, dPos = 0.0;

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
			dPos = (int)(m_nThumbLeft + m_rectThumb.Width() / 2) * dInterval;
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
			double dWidth = m_rectClient.Height() - (m_rectLeftArrow.Height() + m_rectRightArrow.Height());
			dInterval	  = (m_nMaxPos - m_nMinPos) / dWidth;
			dPos = (int)(m_nThumbTop + m_rectThumb.Height() / 2) * dInterval;
			dPos += m_nMinPos;
			
			m_nPos = (int)dPos;
		}	
		
		return;
// 		dPixels   = m_rectClient.Height() - 3*m_nBitmapHeight;
// 		dInterval = dMax / dPixels;
// 		dPos      = dInterval * (m_nThumbTop - m_nBitmapHeight);
	}

	m_nPos = (int) (dPos);
	if (m_nPos < 0)
		m_nPos = 0;
	if (m_nPos > m_nRange)
		m_nPos = m_nRange;
}

///////////////////////////////////////////////////////////////////////////////
// UpdateThumbPosition
void CXScrollBarBaCai::UpdateThumbPosition()
{
	if ( m_rectLeftArrow.Width() <= 0 || m_rectRightArrow.Width() <= 0 || m_rectThumb.Width() <= 0 )
	{
		return;
	}

	double dMax, dInterval, dPos;

	dMax = m_nRange;
	dPos = m_nPos;

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
			int iPosDistance = m_nMaxPos - m_nMinPos;
			if(iPosDistance != 0)
			{
				dInterval	  = dWidth * (dPos - (double)m_nMinPos) / iPosDistance;
				m_nThumbLeft  = (int)dInterval - m_rectThumb.Width() / 2;
			}
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
		{
			double dWidth = m_rectClient.Height() - (m_rectLeftArrow.Height() + m_rectRightArrow.Height());
			int iPosDis = m_nMaxPos - m_nMinPos;
			if(iPosDis != 0)
			{
				dInterval	  = dWidth * (dPos - (double)m_nMinPos) / iPosDis;
				m_nThumbTop  = (int)dInterval - m_rectThumb.Height() / 2;
			}			
		}
// 		dPixels   = m_rectClient.Height() - 3*m_nBitmapHeight;
// 		dInterval = dPixels / dMax;
// 		double dThumbTop = dPos * dInterval + 0.5;
// 		m_nThumbTop = m_nBitmapHeight + (int)dThumbTop;
	}

	LimitThumbPosition();

	Draw();
}

///////////////////////////////////////////////////////////////////////////////
// LimitThumbPosition
void CXScrollBarBaCai::LimitThumbPosition()
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
		m_nBitmapHeight = m_rectRightArrow.Width();
		if ((m_nThumbTop + m_nBitmapHeight) > (m_rectClient.Height() - m_nBitmapHeight))
			m_nThumbTop = m_rectClient.Height() - 2*m_nBitmapHeight;

		m_nBitmapHeight = m_rectLeftArrow.Width();
		if (m_nThumbTop < (m_rectClient.top + m_nBitmapHeight))
			m_nThumbTop = m_rectClient.top + m_nBitmapHeight;
	}
}

///////////////////////////////////////////////////////////////////////////////
// SetScrollRange
void CXScrollBarBaCai::SetScrollRange(int nMinPos,
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
int CXScrollBarBaCai::SetScrollPos(int nPos, BOOL bRedraw /*= TRUE*/)
{
	int nOldPos = m_nPos;

	m_nPos = nPos;

	UpdateThumbPosition();

	if (bRedraw)
		Invalidate();

	if ( nOldPos != m_nPos )		// 通知侦听，Pos已经改变
	{
		FireListener(UM_POS_CHANGED, (WPARAM)m_hWnd, 0);
	}

	return nOldPos;
}

void CXScrollBarBaCai::SetSBRect(LPRECT lpRect, BOOL bDraw)
{
	MoveWindow(lpRect,bDraw);
	GetClientRect(&m_rectClient);

	DrawHorizontal();	// 更新各个矩形分配
	Draw();
	// 重新调节位置
	SetScrollPos(m_nPos, bDraw);
}

///////////////////////////////////////////////////////////////////////////////
// OnSetCursor
BOOL CXScrollBarBaCai::OnSetCursor(CWnd* /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/)
{
	if (m_bThumbHover && m_hCursor)
		::SetCursor(m_hCursor);
	else
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// SetDefaultCursor - borrowed from XHyperLink.cpp
void CXScrollBarBaCai::SetDefaultCursor()
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
			GetWindowsDirectory(strWndDir.GetBuffer(MAX_PATH), MAX_PATH);
			strWndDir.ReleaseBuffer();

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
	}
}

BOOL CXScrollBarBaCai::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
	return CStatic::OnEraseBkgnd(pDC);
}

void CXScrollBarBaCai::AddMsgListener( HWND hwnd )
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

void CXScrollBarBaCai::RemoveMsgListener( HWND hwnd )
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

void CXScrollBarBaCai::FireMsg( UINT msgId, WPARAM w, LPARAM l )
{
	CWnd *pWnd = GetOwner();
	HWND hwndOwner = pWnd->GetSafeHwnd();
	if ( IsWindow(hwndOwner) )
	{
		::SendMessage(hwndOwner, msgId, w, l);
	}

	FireListener(msgId, w, l);
}

void CXScrollBarBaCai::FireListener( UINT msgId, WPARAM w, LPARAM l )
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

void    CXScrollBarBaCai::SetFaceChemeColor(const COLORREF &clr)
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
