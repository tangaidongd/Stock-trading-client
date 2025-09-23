// PNGButton.cpp : 实现文件
//

#include "stdafx.h"
#include "PNGButton.h"

#include <GdiPlus.h>

using namespace Gdiplus;


// 从hinstance中加载对应的图片资源
static bool BitmapFromIDResource(UINT nID, LPCTSTR sTR,Bitmap * &pBitmap)
{
	HINSTANCE hInst = AfxGetResourceHandle();
	HRSRC hRsrc = ::FindResource (hInst,MAKEINTRESOURCE(nID),sTR); // type

	if (!hRsrc)
	{
		return FALSE;
	}

	// load resource into memory
	DWORD len = SizeofResource(hInst, hRsrc);
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
	if (!lpRsrc)
	{
		return FALSE;
	}

	// Allocate global memory on which to create stream
	HGLOBAL hMem = GlobalAlloc(GMEM_FIXED, len);
	if (NULL != hMem)
	{
		BYTE* pmem = (BYTE*)GlobalLock(hMem);
		memmove(pmem,lpRsrc,len);
		IStream* pstm;
		CreateStreamOnHGlobal(hMem,TRUE,&pstm);

		// load from stream
		pBitmap=Gdiplus::Bitmap::FromStream(pstm);
		// free/release stuff
		GlobalUnlock(hMem);
		pstm->Release();
	}	
	FreeResource(lpRsrc);

	return TRUE;
}

// 自动加载gpi+资源类
class Helper_CAutoLoadGdiPlus{
public:
	~Helper_CAutoLoadGdiPlus(){
		GdiplusShutdown(m_token);
	}

	static int LoadGdiPlus(){
		static Helper_CAutoLoadGdiPlus s_plus;
		return 1;
	}

private:
	Helper_CAutoLoadGdiPlus(){
		GdiplusStartupInput gdipInput;
		Status s = GdiplusStartup(&m_token, &gdipInput, NULL);
		if ( Ok != s )
		{
			TRACE(_T("ERROR: GdiplusStartup fail(%d)\r\n"), (int)s);
			AfxThrowNotSupportedException();
		}
	}
	Helper_CAutoLoadGdiPlus(const Helper_CAutoLoadGdiPlus &h);

	ULONG_PTR m_token;
};

int _initializeGdiPlus = Helper_CAutoLoadGdiPlus::LoadGdiPlus();

// CPNGButton

static const TCHAR KPngButtonClassName[] = _T("PNGButton");

IMPLEMENT_DYNAMIC(CPNGButton, CBitmapButton)

CPNGButton::CPNGButton()
{
	m_hCursor = NULL;
	m_bOnButton	=	false;

	// 4 种状态
	m_statusData[ ODS_DISABLED ];
	m_statusData[ ODS_FOCUS    ]; 
	m_statusData[ ODS_SELECTED ]; // press
	m_statusData[ 0 ];		// Normal

	SetTextColor();
	SetBackgroundColor();
	SetOuterFrameColor();
	SetFocusFrameColor();
	SetOuterFrameWidth();
	SetFocusFrameWidth();

	SetShowMode();

	SetDrawRectOnButton();
	SetDrawRectOnButtonColorAndWidth();

	SetDrawDefaultRect();
}

CPNGButton::~CPNGButton()
{
	m_hCursor =NULL;
}


BEGIN_MESSAGE_MAP(CPNGButton, CBitmapButton)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()



// CPNGButton 消息处理程序



BOOL CPNGButton::Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: 在此添加专用代码和/或调用基类

	DWORD dwStyle2 = dwStyle | BS_OWNERDRAW;

	return CBitmapButton::Create(lpszCaption, dwStyle2, rect, pParentWnd, nID);
}

void CPNGButton::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类

	// 检查 BS_OWNERDRAW
	
	CString StrClassName;
	if ( !GetClassName(GetSafeHwnd(), StrClassName.GetBuffer(100), 100) )
	{
		StrClassName.ReleaseBuffer();
		TRACE(_T("ERROR: Invalid hwnd class(%d)\r\n"), GetLastError());
		ASSERT( 0 );
	}
	else
	{
		StrClassName.ReleaseBuffer();
		if ( StrClassName.CompareNoCase(_T("button")) != 0 )
		{
			TRACE(_T("ERROR: Only BUTTON class wnd should be subclass[%s]"), StrClassName.GetBuffer());
			ASSERT( 0 );
		}
		else if ( !(GetStyle() & BS_OWNERDRAW) )
		{
			TRACE(_T("ERROR: Only BS_OWNERDRAW style button should be subclass"));
			ASSERT( 0 );
		}
	}

	
	// 字体初始化
	InitialFont();
	

	CBitmapButton::PreSubclassWindow();

}


void CPNGButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	ASSERT( ODT_BUTTON == lpDrawItemStruct->CtlType );
	ASSERT( GetDlgCtrlID() == lpDrawItemStruct->CtlID );

	int iSave = SaveDC(lpDrawItemStruct->hDC);

	HDC &hDC = lpDrawItemStruct->hDC;
	CRect rc = lpDrawItemStruct->rcItem;

	// 画背景
	COLORREF clrBG, clrText, clrOuterFrame, clrFocusFrame;
	SetStatusColor(0, clrText, clrBG, clrOuterFrame, clrFocusFrame);
	UINT state = lpDrawItemStruct->itemState;
	if ( state & ODS_DISABLED )
	{
		// DISABLE -> PRESS -> FOCUS ->Normal
		SetStatusColor(ODS_DISABLED, clrText, clrBG, clrOuterFrame, clrFocusFrame);
	}
	else if ( state & ODS_SELECTED )
	{
		SetStatusColor(ODS_SELECTED, clrText, clrBG, clrOuterFrame, clrFocusFrame);
	}
	else if ( state & ODS_FOCUS )
	{
		SetStatusColor(ODS_FOCUS, clrText, clrBG, clrOuterFrame, clrFocusFrame);
	}

	FillMySolidRect(hDC, rc, clrBG);

	// 图片 - 是否考虑如果图片无，则显示文字？
	if ( m_nShowMode & EShowImage && (NULL != m_bitmap.m_hObject) )
	{
		CBitmapButton::DrawItem(lpDrawItemStruct);
	}

	if ( EShowImage == m_nShowMode )
	{
		// 只需要画图片，那就可以什么都不干了
	}
	else
	{
		// 绘制边框

		if ( m_nShowMode & EShowOuterFrame )
		{
			FrameMyRect(hDC, rc, clrOuterFrame, m_iOuterFrameWidth, m_iOuterFrameWidth);

			rc.InflateRect(-m_iOuterFrameWidth, -m_iOuterFrameWidth);
		}

		if ( (m_nShowMode & EShowFocusFrame) && (state & ODS_FOCUS) )
		{
			// 绘制聚焦 - 如果聚焦
			FrameMyRect(hDC, rc, clrFocusFrame, m_iFocusFrameWidth, m_iFocusFrameWidth);

			rc.InflateRect(-m_iFocusFrameWidth, -m_iFocusFrameWidth);
		}

		if ( m_nShowMode & EShowText )
		{
			// 文字
			CString StrText;
			GetWindowText(StrText);
			::SetTextColor(hDC, clrText);
			::SetBkMode(hDC, TRANSPARENT);
			DrawText(hDC, StrText, StrText.GetLength(), &rc, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		}
	}

	if ( m_bNeedDrawRectOnButton && m_bOnButton )
	{
		// 绘制鼠标在按钮上的标记
		CRect rcM = lpDrawItemStruct->rcItem;
		rcM.InflateRect(-1, -1);
		FrameMyRect(hDC, rcM, m_clrDrawRectOnButton, m_iWidthDrawRectOnButton, m_iWidthDrawRectOnButton);
	}

	if ( m_bNeedDrawDefaultRect && (this == GetFocus()) )
	{
		CRect rcF = lpDrawItemStruct->rcItem;
		rcF.InflateRect(-2,-2);
		DrawFocusRect(hDC, rcF);
	}
	
	

	RestoreDC(lpDrawItemStruct->hDC, iSave);
}

bool CPNGButton::SetBitmaps( UINT nBmpNormal, UINT nBmpPress /*= 0*/, UINT nBmpFocus /*= 0*/, UINT nBmpDisable /*= 0*/ )
{
	return LoadBitmaps(nBmpNormal, nBmpPress, nBmpFocus, nBmpDisable) ? true : false;
}

bool CPNGButton::SetImages( LPCTSTR lpszResSection, UINT nImageNormal, UINT nImagePress /*= 0*/, UINT nImageFocus /*= 0*/, UINT nImageDisable /*= 0*/ )
{
	// clear
	m_bitmap.DeleteObject();
	m_bitmapSel.DeleteObject();
	m_bitmapFocus.DeleteObject();
	m_bitmapDisabled.DeleteObject();

	// 加载对应的资源 -> 转成Bitmap句柄
	Bitmap	*pBmpN = NULL, *pBmpP= NULL, *pBmpF= NULL, *pBmpD= NULL;
	bool bAllLoad = true;
	if ( 0 != nImageDisable && !BitmapFromIDResource(nImageDisable, lpszResSection, pBmpD) )
	{
		TRACE(_T("Fail to load Disable image\r\n"));
		bAllLoad = false;
	}
	if ( NULL != pBmpD )
	{
		HBITMAP hb;
		Status s = pBmpD->GetHBITMAP(Color(), &hb) ;
		ASSERT( Ok == s );
		m_bitmapDisabled.Attach(hb);
	}

	if ( 0 != nImageFocus && !BitmapFromIDResource(nImageFocus, lpszResSection, pBmpF) )
	{
		TRACE(_T("Fail to load focus image\r\n"));
		bAllLoad = false;
	}
	if ( NULL != pBmpF )
	{
		HBITMAP hb;
		Status s = pBmpF->GetHBITMAP(Color(), &hb) ;
		ASSERT( Ok == s );
		m_bitmapFocus.Attach(hb);
	}

	if ( 0 != nImagePress && !BitmapFromIDResource(nImagePress, lpszResSection, pBmpP) )
	{
		TRACE(_T("Fail to load select/press image\r\n"));
		bAllLoad = false;
	}
	if ( NULL != pBmpP )
	{
		HBITMAP hb;
		Status s = pBmpP->GetHBITMAP(Color(), &hb) ;
		ASSERT( Ok == s );
		m_bitmapSel.Attach(hb);
	}

	if ( !BitmapFromIDResource(nImageNormal, lpszResSection, pBmpN) )
	{
		TRACE(_T("Fail to load normal image\r\n"));
		bAllLoad = false;
	}
	if (NULL != pBmpN)	
	{
		HBITMAP hb;
		Status s = pBmpN->GetHBITMAP(Color(), &hb) ;
		ASSERT( Ok == s );
		m_bitmap.Attach(hb);
	}

	return bAllLoad;
}


void CPNGButton::SetCursorOnButton( HCURSOR hCursor )
{
	m_hCursor = hCursor;

	if ( m_hCursor && m_bOnButton )
	{
		SetCursor(m_hCursor);
	}
}

void CPNGButton::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	// track
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_LEAVE;
	tme.hwndTrack = m_hWnd;
	
	_TrackMouseEvent(&tme);

	
	if ( m_hCursor && !m_bOnButton )
	{
		::SetCursor(m_hCursor);
	}

	if ( !m_bOnButton && m_bNeedDrawRectOnButton )
	{
		// 绘制鼠标标记
		Invalidate(TRUE);
	}
	m_bOnButton	=	true;
	
	CBitmapButton::OnMouseMove(nFlags, point);
}

LRESULT CPNGButton::OnMouseLeave( WPARAM w, LPARAM l )
{
	m_bOnButton = false;

	if ( m_bNeedDrawRectOnButton )
	{
		// 清除鼠标标志
		Invalidate(TRUE);
	}

	return Default();
}

BOOL CPNGButton::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此添加专用代码和/或调用基类
	// 创建自己的Class
	return CBitmapButton::PreCreateWindow(cs);
}

BOOL CPNGButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	if ( m_hCursor )
	{
		SetCursor(m_hCursor);
		return TRUE;
	}

	return CBitmapButton::OnSetCursor(pWnd, nHitTest, message);
}

void CPNGButton::SetFontSize( int iFontSize )
{
	if ( iFontSize < 4 )
	{
		iFontSize = 4;
	}
	else if ( iFontSize > 1000 )
	{
		iFontSize = 1000;
	}

	// 初始化字体为窗口字体->父窗口字体->System
	InitialFont();
	

	ASSERT( NULL != m_font.m_hObject );

	LOGFONT lf;
	m_font.GetLogFont(&lf);
	lf.lfHeight = iFontSize;

	m_font.DeleteObject();
	m_font.CreateFontIndirect(&lf);
	ASSERT( NULL != m_font.m_hObject );

	SetFont(&m_font, TRUE);
}

bool CPNGButton::InitialFont()
{
	CFont *pFontWnd = GetFont();
	if ( m_font.GetSafeHandle() != pFontWnd->GetSafeHandle() )
	{
		// 不是同一个字体，应当设置为窗体的字体，因为类并没有单独提供m_font的facename设置方法
		m_font.DeleteObject();
	}

	if ( NULL == m_font.m_hObject)
	{
		// 字体没有初始化
		if ( NULL == pFontWnd )
		{
			// 父字体
			if ( IsWindow(GetParent()->GetSafeHwnd()) )
			{
				pFontWnd = GetParent()->GetFont();
			}

			if ( NULL == pFontWnd )
			{
				// 系统字体
				pFontWnd = CFont::FromHandle((HFONT)GetStockObject(SYSTEM_FONT));
				ASSERT( pFontWnd );
			}
		}

		// LOGFONT
		LOGFONT lf;
		pFontWnd->GetLogFont(&lf);
		m_font.CreateFontIndirect(&lf);
	}

	ASSERT( NULL != m_font.m_hObject );

	// 强制设置窗体字体与现在字体相同
	SetFont(&m_font, TRUE);

	return NULL != m_font.m_hObject;
}

int CPNGButton::SetOuterFrameWidth( int iNewWidth /*= 1*/ )
{
	int i = m_iOuterFrameWidth;
	m_iOuterFrameWidth = iNewWidth;
	return i;
}

int CPNGButton::SetFocusFrameWidth( int iNewWidth /*= 2*/ )
{
	int i = m_iFocusFrameWidth;
	m_iFocusFrameWidth = iNewWidth;
	return i;
}

void CPNGButton::SetTextColor( COLORREF clrNormal /*= EBlack*/, COLORREF clrPress /*= EBlack*/, COLORREF clrFocus /*= EBlack*/, COLORREF clrDisable /*= EBlack*/ )
{
	m_statusData[ 0 ].m_clrText = clrNormal;
	m_statusData[ ODS_SELECTED ].m_clrText = clrPress;
	m_statusData[ ODS_FOCUS ].m_clrText = clrFocus;
	m_statusData[ ODS_DISABLED ].m_clrText = clrDisable;
}

void CPNGButton::SetBackgroundColor( COLORREF clrNormal /*= EWhite*/, COLORREF clrPress /*= EWhite*/, COLORREF clrFocus /*= EWhite*/, COLORREF clrDisable /*= EWhite*/ )
{
	m_statusData[ 0 ].m_clrBackground = clrNormal;
	m_statusData[ ODS_SELECTED ].m_clrBackground = clrPress;
	m_statusData[ ODS_FOCUS ].m_clrBackground = clrFocus;
	m_statusData[ ODS_DISABLED ].m_clrBackground = clrDisable;
}

void CPNGButton::SetOuterFrameColor( COLORREF clrNormal /*= EGray*/, COLORREF clrPress /*= EGray*/, COLORREF clrFocus /*= EGray*/, COLORREF clrDisable /*= EGray*/ )
{
	m_statusData[ 0 ].m_clrOuterFrame = clrNormal;
	m_statusData[ ODS_SELECTED ].m_clrOuterFrame = clrPress;
	m_statusData[ ODS_FOCUS ].m_clrOuterFrame = clrFocus;
	m_statusData[ ODS_DISABLED ].m_clrOuterFrame = clrDisable;
}

void CPNGButton::SetFocusFrameColor( COLORREF clrNormal /*= ELightBlue*/, COLORREF clrPress /*= ELightBlue*/, COLORREF clrFocus /*= ELightBlue*/, COLORREF clrDisable /*= ELightBlue*/ )
{
	m_statusData[ 0 ].m_clrFocusFrame = clrNormal;
	m_statusData[ ODS_SELECTED ].m_clrFocusFrame = clrPress;
	m_statusData[ ODS_FOCUS ].m_clrFocusFrame = clrFocus;
	m_statusData[ ODS_DISABLED ].m_clrFocusFrame = clrDisable;
}

bool CPNGButton::SetStatusColor( int ODSStatus, COLORREF &clrText, COLORREF &clrBackground, COLORREF &clrOuterFrame, COLORREF &clrFocusFrame )
{
	bool bRet = true;
	if ( m_statusData.count(ODSStatus) < 1 )
	{
		ASSERT( 0 );

		// 使用普通状态代替
		ODSStatus = 0;

		bRet = false;
		//return false;
	}
	clrBackground			= m_statusData[ODSStatus].m_clrBackground;
	clrText					= m_statusData[ODSStatus].m_clrText;
	clrOuterFrame	= m_statusData[ODSStatus].m_clrOuterFrame;
	clrFocusFrame	= m_statusData[ODSStatus].m_clrFocusFrame;

	return bRet;
}

void CPNGButton::FillMySolidRect( HDC hDC, LPRECT pRect, COLORREF clr )
{
	ASSERT( NULL != hDC );

	if (NULL != hDC)
	{
		::SetBkColor(hDC, clr);
		::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, pRect, NULL, 0, NULL);
	}
	
}

UINT32 CPNGButton::SetShowMode( UINT32 esmShow /*= EShowText|EShowOuterFrame|EShowFocusFrame|EShowImage */ )
{
	UINT32 sm = esmShow;
	m_nShowMode = esmShow;
	return sm;
}

void CPNGButton::FrameMyRect( HDC hDC, LPRECT pRect, COLORREF clr, int iWidth, int iHeight )
{
	ASSERT( hDC && pRect );

	if (NULL != hDC && NULL != pRect)
	{
		CBrush brush;
		brush.CreateSolidBrush(clr);
		HGDIOBJ hBrushOld = SelectObject(hDC, brush.GetSafeHandle());

		CRgn rgn;
		rgn.CreateRectRgnIndirect(pRect);
		FrameRgn(hDC, (HRGN)rgn.GetSafeHandle(), (HBRUSH)brush.GetSafeHandle(), iWidth, iHeight);

		SelectObject(hDC, hBrushOld);
	}
	
}

void CPNGButton::SetDrawRectOnButton( bool bNeedDrawRect /*= true*/ )
{
	m_bNeedDrawRectOnButton	=	bNeedDrawRect;
}

void CPNGButton::SetDrawRectOnButtonColorAndWidth( COLORREF clr /*= RGB(255,216,176)*/, int iNewWidth /*= 2*/ )
{
	m_clrDrawRectOnButton = clr;
	m_iWidthDrawRectOnButton = iNewWidth;
}

void CPNGButton::SetDrawDefaultRect( bool bNeedDraw /*= true*/ )
{
	m_bNeedDrawDefaultRect = bNeedDraw;
}
