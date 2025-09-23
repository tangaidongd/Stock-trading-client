#include "stdafx.h"

#include <math.h>

#include "DrawingCurve.h"

///////////////////////////////////////////////////////////////////////////////
void DrawArrow ( CDC* pDC, CPoint pt1,CPoint pt2, float fArrowSize)
{
	double dslopy;
	float  cosy , siny;
	if ( fArrowSize < 8.0f ) fArrowSize = 8.0f;
	if ( fArrowSize > 30.0f ) fArrowSize = 30.0f;
	
	dslopy = atan2( (double)(pt1.y - pt2.y),(double)(pt1.x - pt2.x) );
	cosy = cosf( (float)dslopy );
	siny = sinf( (float)dslopy );
	
	POINT pt[3];
	pt[0].x = pt2.x + int( fArrowSize * cosy - ( fArrowSize / 2.0 * siny ) );
	pt[0].y = pt2.y + int( fArrowSize * siny + ( fArrowSize / 2.0 * cosy ) );
	pt[1].x = pt2.x;
	pt[1].y = pt2.y;
	pt[2].x = pt2.x + int( fArrowSize * cosy + fArrowSize / 2.0 * siny );
	pt[2].y = pt2.y - int( fArrowSize / 2.0 * cosy - fArrowSize * siny );
	
	pDC->Polyline(pt,3);	
}
COLORREF Get2Colors ( COLORREF clr1,COLORREF clr2 )
{
	int32 r1 = GetRValue(clr1);
	int32 g1 = GetGValue(clr1);
	int32 b1 = GetBValue(clr1);
	
	int32 r2 = GetRValue(clr2);
	int32 g2 = GetGValue(clr2);
	int32 b2 = GetBValue(clr2);
	
	int32 rx = abs(r1-r2)/3;
	int32 gx = abs(g1-g2)/3;
	int32 bx = abs(b1-b2)/3;
	
	return RGB(r1>r2 ? r1-rx:r1+rx,
		g1>g2 ? g1-gx:g1+gx,
		b1>b2 ? b1-bx:b1+bx);
}
///////////////////////////////////////////////////////////////////////////////
RBIndex::RBIndex()
{
	
}
RBIndex::~RBIndex()
{
	RemoveAll();
}
bool32 RBIndex::Add (const int32 key,void* obj )
{
	//	ASSERT(NULL!=obj);
	_data[key] = obj;
	return true;
}
bool32 RBIndex::Remove ( const int32 key )
{
	if ( _data.empty() ) return false;
	RBTREE::iterator i = _data.find ( key );
	if ( i == _data.end() ) return false;
	_data.erase ( i );
	return true;
}

void* RBIndex::Get( const int32 key )
{
	if ( _data.empty() ) return NULL;
	RBTREE::iterator i = _data.find ( key );
	if ( i == _data.end() ) return NULL;
	return i->second;
}

void RBIndex::RemoveAll()
{
	if ( !_data.empty())
	{
		_data.clear();
	}
}





////////////////////////////////////////////////////////////////////////////////
uint32 CDrawingCurve::g_uiPickColor = 0x999999;
RBIndex CDrawingCurve::g_MapIndex;





uint32 CDrawingCurve::KSelect					= 0x40;
CDrawingCurve* CDrawingCurve::GetCurve ( uint32 iPickColor )
{
	void* p = g_MapIndex.Get(iPickColor);
	return (CDrawingCurve*)p;
}
CDrawingCurve::CDrawingCurve()
{
	m_uiPickColor = CDrawingCurve::g_uiPickColor;
	CDrawingCurve::g_uiPickColor ++;
	CDrawingCurve::g_MapIndex.Add(m_uiPickColor,this);
	m_bSelfDraw			= false;
	m_iFlag = 0;
}
CDrawingCurve::~CDrawingCurve()
{
	CDrawingCurve::g_MapIndex.Remove(m_uiPickColor);
}
