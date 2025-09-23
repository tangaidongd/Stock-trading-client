// NewDateTimeCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "NewDateTimeCtrl.h"
#include "FontFactory.h"
#include "resource.h"

#define COLOR_BORDER	RGB(98,104,111)
#define COLOR_TEXT		RGB(200,200,200)
#define COLOR_BACK		RGB(39,44,49)
#define LEN_BTN	17

// CNewDateTimeCtrl

IMPLEMENT_DYNAMIC(CNewDateTimeCtrl, CDateTimeCtrl)

CNewDateTimeCtrl::CNewDateTimeCtrl()
{
	m_bOver = FALSE;
	m_font.CreateFont(-14,0,0,0,0,0,0,0,0,0,0,0,0,gFontFactory.GetExistFontName(L"微软雅黑"));

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_bmapArrow.LoadBitmap(IDB_BITMAP_CMTARROW);
}

CNewDateTimeCtrl::~CNewDateTimeCtrl()
{
	if ( m_font.m_hObject != NULL )
	{
		m_font.DeleteObject();
	}
}


BEGIN_MESSAGE_MAP(CNewDateTimeCtrl, CDateTimeCtrl)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE( WM_MOUSELEAVE,OnMouseLeave )
	ON_NOTIFY_REFLECT(DTN_DATETIMECHANGE, OnDatetimeChange)
END_MESSAGE_MAP()



// CNewDateTimeCtrl 消息处理程序
void CNewDateTimeCtrl::OnDatetimeChange(NMHDR*   pNMHDR,   LRESULT*   pResult) 
{
	Invalidate();
}

void CNewDateTimeCtrl::OnPaint()
{
	CPaintDC dc(this);

	CDC dMemDC;
	dMemDC.CreateCompatibleDC(&dc);
	dMemDC.SetMapMode(dc.GetMapMode());

	CRect rcClient;
	GetClientRect(&rcClient);
	CRect rtContent(rcClient);
	rtContent.right = rcClient.right - LEN_BTN;

	CRect rtPic(rcClient);
	rtPic.left = rtPic.right - LEN_BTN;

	dMemDC.SelectObject(&m_bmapArrow);
	if (m_bOver)
	{
		dc.BitBlt(rtPic.left,(rtPic.Height()-12)/2, 16 , 12 , &dMemDC,0, 13,SRCCOPY);
	}
	else
	{
		dc.BitBlt(rtPic.left,(rtPic.Height()-12)/2, 16 , 12 , &dMemDC,0, 0,SRCCOPY);
	}

	dc.FillSolidRect(rtContent, COLOR_BACK);

	CPen Pen;
	Pen.CreatePen(PS_SOLID, 2, COLOR_BORDER);
	CPen *pOld =  dc.SelectObject(&Pen);
	dc.MoveTo(rcClient.left, rcClient.top);
	dc.LineTo(rcClient.right, rcClient.top);
	dc.MoveTo(rcClient.right, rcClient.top);
	dc.LineTo(rcClient.right, rcClient.bottom);
	dc.MoveTo(rcClient.left, rcClient.bottom);
	dc.LineTo(rcClient.right, rcClient.bottom);
	dc.MoveTo(rcClient.left, rcClient.bottom);
	dc.LineTo(rcClient.left, rcClient.top);
	dc.SelectObject(pOld);
	DeleteObject(&Pen);

	CTime time;
	GetTime(time);
	CString StrTime;
	StrTime.Format(L"%04d/%02d/%02d", time.GetYear(), time.GetMonth(), time.GetDay());

	CFont* pOldFont = dc.SelectObject( &m_font );
	dc.SetTextColor(COLOR_TEXT);
	dc.DrawText(StrTime, rtContent, DT_CENTER|DT_SINGLELINE | DT_VCENTER );
	dc.SelectObject( pOldFont );
}

void CNewDateTimeCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if ( m_bOver == FALSE )
	{
		m_bOver = TRUE;

		//更新当前区域
		UpdateRect();

		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof( TRACKMOUSEEVENT );
		tme.dwFlags = TME_LEAVE;
		tme.dwHoverTime = 0;
		tme.hwndTrack = m_hWnd;

		_TrackMouseEvent( &tme );
	}
	CDateTimeCtrl::OnMouseMove(nFlags, point);
}

LRESULT CNewDateTimeCtrl::OnMouseLeave( WPARAM wParam,LPARAM lParam )
{
	if ( m_bOver )
	{
		m_bOver = FALSE;
	}

	UpdateRect();

	return 1;
}

void CNewDateTimeCtrl::UpdateRect(void)
{
	CRect rcClient;
	GetWindowRect( &rcClient );
	rcClient.left = rcClient.right - LEN_BTN;
	GetParent()->ScreenToClient( &rcClient);
	GetParent()->InvalidateRect( &rcClient, FALSE );

	return;
}