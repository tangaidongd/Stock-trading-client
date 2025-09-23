#include "stdafx.h"

#include "DrawingCurve.h"

#include "MemdcEx.h"


///////////////////////////////////////////////////////////////////////////////
CMemDCEx::CMemDCEx(CDC* pDC,const CRect& rcBounds,bool32 bPickDC )
:CDC()
{
	CreateCompatibleDC(pDC);
	m_bPickDC = bPickDC;

	if ( m_bPickDC )
	{
		void* pData = NULL;
		BITMAPINFO bmi;
		memset(&bmi, 0, sizeof(bmi));
		BITMAPINFOHEADER& bmih = bmi.bmiHeader;
		bmih.biSize        = sizeof(bmih);
		bmih.biWidth       = rcBounds.Width();
		bmih.biHeight      = rcBounds.Height();
		bmih.biPlanes      = 1;
		bmih.biBitCount    = 32;
		bmih.biCompression = BI_RGB;
		m_hBmp = CreateDIBSection( GetSafeHdc(), &bmi, DIB_RGB_COLORS,&pData, NULL, 0);
	}
	else
	{
		m_hBmp = ::CreateCompatibleBitmap(pDC->GetSafeHdc(), rcBounds.Width(), rcBounds.Height());
	}
	m_hOldBmp = (HBITMAP)::SelectObject(GetSafeHdc(),m_hBmp);
	m_pDC = pDC;
	m_Rect = rcBounds;
	m_RectClipDiff = CRect(0,0,0,0);	
	m_pOldPenPick	= NULL;
	m_pOldBrushPick	= NULL;
	m_ColorPick		= 0;
	m_Color1		= 0;
	m_Color2		= 0;
	m_ColorDot		= 0;
	m_iDotSkip		= 0;
	m_iCount		= 0;
	m_iCurCount		= 0;
}
CMemDCEx::~CMemDCEx()
{
	::SelectObject(GetSafeHdc(),m_hOldBmp);
	::DeleteObject(m_hBmp);
 	DeleteDC();
}
void CMemDCEx::Swap()
{
	CRgn rgn;
	if ( m_RectClipDiff.Width() > 0 )
	{
		rgn.CreateRectRgnIndirect(m_RectClipDiff);
		ExtSelectClipRgn(m_pDC->GetSafeHdc(),(HRGN)rgn.GetSafeHandle(),RGN_DIFF);
	}
	m_pDC->BitBlt(m_Rect.left, m_Rect.top, m_Rect.Width(), m_Rect.Height(), 
		this, m_Rect.left, m_Rect.top, SRCCOPY);
	if ( m_RectClipDiff.Width() > 0 )
	{
		ExtSelectClipRgn(m_pDC->GetSafeHdc(),NULL,RGN_COPY);
		rgn.DeleteObject();
	}
}
void CMemDCEx::Swap(CRect Rect)
{
	CRgn rgn;
	if ( m_RectClipDiff.Width() > 0 )
	{
		rgn.CreateRectRgnIndirect(m_RectClipDiff);
		ExtSelectClipRgn(m_pDC->GetSafeHdc(),(HRGN)rgn.GetSafeHandle(),RGN_DIFF);
	}
	m_pDC->BitBlt(Rect.left, Rect.top, Rect.Width(), Rect.Height(), 
		this, Rect.left, Rect.top, SRCCOPY);

	if ( m_RectClipDiff.Width() > 0 )
	{
		ExtSelectClipRgn(m_pDC->GetSafeHdc(),NULL,RGN_COPY);
		rgn.DeleteObject();
	}
}
bool32 CMemDCEx::IsPickDC()
{
	return m_bPickDC;
}
void CMemDCEx::_DrawLine ( int32 x1, int32 y1, int32 x2, int32 y2 )
{
	MoveTo ( x1,y1 );
	LineTo (x2,y2);
}
void CMemDCEx::_DrawLine ( CPoint pt1, CPoint pt2 )
{
	MoveTo(pt1);
	LineTo(pt2);
}
void CALLBACK DDACountProc(int x, int y, LPARAM lParam)
{
	CMemDCEx* pDC = (CMemDCEx*)lParam;
	pDC->m_iCount ++;	
}
void CALLBACK DDADotProc(int x, int y, LPARAM lParam)
{
	CMemDCEx* pDC = (CMemDCEx*)lParam;
	if ( 0 == (pDC->m_iCurCount%pDC->m_iDotSkip) || pDC->m_iCurCount == pDC->m_iCount)
	{
		pDC->SetPixel(x,y,pDC->m_ColorDot);
	}
	pDC->m_iCurCount ++;	
}
void CMemDCEx::_DrawDotLine ( int32 x1, int32 y1, int32 x2, int32 y2, int32 iSkip,COLORREF clr )
{
	m_ColorDot	= clr;
	m_iCurCount = 0;
	m_iCount	= 0;
	m_iDotSkip	= iSkip;
	::LineDDA(x1,y1,x2,y2,(LINEDDAPROC)DDACountProc,(LPARAM)this);
	::LineDDA(x1,y1,x2,y2,(LINEDDAPROC)DDADotProc,(LPARAM)this);
}
void CMemDCEx::_DrawDotLine ( CPoint pt1, CPoint pt2, int32 iSkip, COLORREF clr )
{
	m_ColorDot	= clr;
	m_iCurCount = 0;
	m_iCount	= 0;
	m_iDotSkip	= iSkip;
	::LineDDA(pt1.x,pt1.y,pt2.x,pt2.y,(LINEDDAPROC)DDACountProc,(LPARAM)this);
	::LineDDA(pt1.x,pt1.y,pt2.x,pt2.y,(LINEDDAPROC)DDADotProc,(LPARAM)this);
}
void CALLBACK DDAEdgeProc(int x, int y, LPARAM lParam)
{
	CMemDCEx* pDC = (CMemDCEx*)lParam;
	pDC->SetPixel(x,y,pDC->m_Color1);
	pDC->SetPixel(x-1,y,pDC->m_Color2);
	pDC->SetPixel(x+1,y,pDC->m_Color2);
}
void CMemDCEx::_DrawEdge(CPoint pt1,CPoint pt2, COLORREF clr1,COLORREF clr2)
{
	m_Color1 = clr1;
	m_Color2 = clr2;
	::LineDDA(pt1.x,pt1.y,pt2.x,pt2.y,(LINEDDAPROC)DDAEdgeProc,(LPARAM)this);
}
void CMemDCEx::_DrawEdge(int32 x1,int32 y1,int32 x2,int32 y2, COLORREF clr1,COLORREF clr2)
{
	m_Color1 = clr1;
	m_Color2 = clr2;
	::LineDDA(x1,y1,x2,y2,(LINEDDAPROC)DDAEdgeProc,(LPARAM)this);
}
void CMemDCEx::_Clear(CRect Rect,COLORREF clr)
{
	FillSolidRect(Rect,clr);
}

void CMemDCEx::_FillSolidRect(CRect Rect,COLORREF clr)
{
	if ( m_bPickDC )
	{
		FillSolidRect(Rect,m_ColorPick);
	}
	else
	{
		FillSolidRect(Rect,clr);
	}
}
void CMemDCEx::_DrawRect ( CRect Rect )
{
	if ( Rect.left == Rect.right && Rect.top == Rect.bottom )
	{
		// 只有一个点的时候:
		CPen* pPenNowUse = GetCurrentPen();
		if ( NULL == pPenNowUse )
		{
			return;
		}
			
		LOGPEN logPen;
		pPenNowUse->GetLogPen(&logPen);
		SetPixel(CPoint(Rect.left,Rect.top), logPen.lopnColor);
	}
	else		
	{
		//无需修改Brush的做法
		MoveTo(Rect.left,Rect.top);
		LineTo(Rect.right,Rect.top);
		LineTo(Rect.right,Rect.bottom);
		LineTo(Rect.left,Rect.bottom);
		LineTo(Rect.left,Rect.top);
	}

}
void CMemDCEx::_DrawRect ( CRect Rect,COLORREF clr )
{
	if ( Rect.left == Rect.right && Rect.top == Rect.bottom )
	{
		// 一个点的时候
		SetPixel(CPoint(Rect.left,Rect.top), clr);
	}
	else
	{
		CPen pen,*pOldPen;
		pen.CreatePen(PS_SOLID,1,clr);
		pOldPen = (CPen*)this->SelectObject(&pen);
		
		_DrawRect(Rect);
		
		this->SelectObject(pOldPen);
		pen.DeleteObject();
	}
}

void CMemDCEx::BeginDraw ( CDrawingCurve* pDrawingCurve,int32 iPenWidth )
{
	if ( m_bPickDC )
	{
		m_ColorPick = pDrawingCurve->GetPickColor();
		CPen* pCurrentPen = GetCurrentPen();
		LOGPEN LogPen;
		pCurrentPen->GetLogPen(&LogPen);
		LogPen.lopnColor = pDrawingCurve->GetPickColor();
		LogPen.lopnStyle = PS_SOLID;
		LogPen.lopnWidth = CPoint(iPenWidth,iPenWidth);
		m_PenPick.CreatePenIndirect(&LogPen);
		m_pOldPenPick = (CPen*)SelectObject(&m_PenPick);
		
		CBrush* pCurrentBrush = GetCurrentBrush();
		LOGBRUSH LogBrush;
		pCurrentBrush->GetLogBrush(&LogBrush);
		LogBrush.lbColor = pDrawingCurve->GetPickColor();
		m_BrushPick.CreateBrushIndirect(&LogBrush);
		m_pOldBrushPick = (CBrush*)SelectObject(&m_BrushPick);
	}
}
void CMemDCEx::EndDraw ( )
{
	if ( m_bPickDC )
	{
		SelectObject(m_pOldPenPick);
		SelectObject(m_pOldBrushPick);
		m_PenPick.DeleteObject();
		m_BrushPick.DeleteObject();
	}
}


