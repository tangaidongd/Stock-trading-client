#include "StdAfx.h"
#include "DrawPolygon.h"

static const int KiPolygonW = 12;
static const int KiPolygonH = 12;

void DrawPolygonBorder(CDC* pDC, CRect rectWnd, bool bLeft /*= true,  */,COLORREF clrFill /* = RGB(234, 23, 23)*/, COLORREF clrBorder /*= RGB(234, 23, 23)*/)
{
	if (NULL == pDC)
	{
		return;
	}

	CRect rect(rectWnd);
	
	if (bLeft)
	{
		rect.right = rect.left + KiPolygonW;
		rect.bottom = rect.top + KiPolygonH;
	}
	else
	{
		rect.right -=1;
		rect.left = rect.right - KiPolygonW;
		rect.bottom = rect.top + KiPolygonH;
	}
	

	CPen penBlue(PS_SOLID, 1, clrBorder);
	CPen* pOldPen = pDC->SelectObject(&penBlue);
	CBrush brushRed(clrFill);
	CBrush* pOldBrush = pDC->SelectObject(&brushRed);
	//	离left,top边界的高度
	int iCornerBorderH =1;

	CPoint pts[7];
	if (bLeft)
	{
		pts[0].x = rect.left;
		pts[0].y = rect.top;

		pts[1].x = rect.right;
		pts[1].y = rect.top;

		pts[2].x = rect.right;
		pts[2].y = rect.top + iCornerBorderH;

		pts[3].x = rect.left + rect.Width()/2;
		pts[3].y = rect.top + iCornerBorderH;


		pts[4].x =rect.left + iCornerBorderH;
		pts[4].y = rect.top + rect.Height()/2;


		pts[5].x = rect.left + iCornerBorderH;
		pts[5].y = rect.bottom;

		pts[6].x = rect.left;
		pts[6].y = rect.left + rect.Height();
	}
	else
	{
		pts[0].x = rect.left;
		pts[0].y = rect.top;

		pts[1].x = rect.right;
		pts[1].y = rect.top;

		pts[2].x = rect.right;
		pts[2].y = rect.bottom;

		pts[3].x = rect.right - iCornerBorderH;
		pts[3].y = rect.bottom ;


		pts[4].x =rect.right - iCornerBorderH;
		pts[4].y = rect.top + (float)rect.Height()/2;


		pts[5].x = rect.right - (float)rect.Width()/2;
		pts[5].y = rect.top + iCornerBorderH;

		pts[6].x = rect.left;
		pts[6].y = rect.top + iCornerBorderH;		
	}


	// we specified.
	pDC->Polygon(pts, 7);

	// Put back the old objects.
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);

}

