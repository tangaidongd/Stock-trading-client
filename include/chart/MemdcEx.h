#ifndef _MEMDC_EX_H_
#define _MEMDC_EX_H_

class CDrawingCurve;
#include "typedef.h"
#include "chartexport.h"
/////////////////////////////////////////////////////////////////////////
//(1)支持CMemDC的缓冲区swap
//(2)支持简单的Feedback拾取
//(3)支持常用绘制功能,尽量把所有Chart中绘制功能底层封装到此处,便于以后移植?
class CHART_EXPORT CMemDCEx : public CDC
{
public:
	CMemDCEx(CDC* pDC,const CRect& rcBounds,bool32 bPickDC = false );
	~CMemDCEx();
	void			Swap( );
	void			Swap( CRect Rect );
	void			_FillSolidRect(CRect Rect,COLORREF clr);
	void			_DrawLine ( int32 x1, int32 y1, int32 x2, int32 y2 );
	void			_DrawLine ( CPoint pt1, CPoint pt2 );
	void			_DrawDotLine ( int32 x1, int32 y1, int32 x2, int32 y2, int32 iSkip, COLORREF clr );
	void			_DrawDotLine ( CPoint pt1, CPoint pt2, int32 iSkip, COLORREF clr );
	void			_DrawEdge(CPoint pt1,CPoint pt2, COLORREF clr1,COLORREF clr2);
	void			_DrawEdge(int32 x1,int32 y1,int32 x2,int32 y2, COLORREF clr1,COLORREF clr2);
	void			_DrawRect ( CRect Rect );
	void			_DrawRect ( CRect Rect,COLORREF clr );
			
	bool32			IsPickDC();
	void			BeginDraw ( CDrawingCurve* pDrawingCurve,int32 iPenWidth );
	void			EndDraw ( );
	void			_Clear(CRect Rect,COLORREF clr);
		
	CMemDCEx* operator->() {return this;}
    operator CMemDCEx*() {return this;}

protected:
	bool32			m_bPickDC;
	CPen			m_PenPick;
	CPen			*m_pOldPenPick;
	CBrush			m_BrushPick;
	CBrush			*m_pOldBrushPick;
	COLORREF		m_ColorPick;
public:
	COLORREF		m_ColorDot;
	int32			m_iDotSkip;
	int32			m_iCount;
	int32			m_iCurCount;
	CRect			m_RectClipDiff;
	COLORREF		m_Color1;
	COLORREF		m_Color2;
public:
    CDC				*m_pDC;					// Saves CDC passed in constructor
    CRect			m_Rect;					// Rectangle of drawing area.
	HBITMAP			m_hBmp;
	HBITMAP			m_hOldBmp;
};


#endif


