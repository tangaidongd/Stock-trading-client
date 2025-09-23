#ifndef _DRAWING_CURVE_H_
#define _DRAWING_CURVE_H_

#include "MemdcEx.h"
#include <map>
#include "chartexport.h"

CHART_EXPORT void DrawArrow ( CDC* pDC, CPoint pt1,CPoint pt2, float fArrowSize = 3.0f );
CHART_EXPORT COLORREF Get2Colors ( COLORREF clr1,COLORREF clr2);



//Red Black Tree Index
typedef std::map<int32,void*> RBTREE;
class CHART_EXPORT RBIndex
{
public:
	RBIndex();
	~RBIndex();
	bool32	Add (const int32,void*);
	bool32	Remove ( const int32 );
	void	RemoveAll();
	void*	Get ( int32 );
	//	void*	GetByPos ( int32 iPos );
	//	int32	GetSize();
private:
	RBTREE _data;
};




/////////////////////////////////////////////////////////////////////////
//在Region中绘制的曲线,提供绘制/坐标转换
class CHART_EXPORT CDrawingCurve
{
public:
	CDrawingCurve();
	virtual ~CDrawingCurve();
	
public:
	// 绘制
	virtual void	Draw(IN CMemDCEx* pDC, IN CMemDCEx* pPickDC) = 0;
	virtual bool32	CalcY(OUT float &fYMin, OUT float &fYMax) = 0;		// 计算y最大最小值,必须在赋值m_Nodes之后调用
	virtual bool32	GetYMinMax(OUT float& fYMin, OUT float& fYMax) = 0;
	virtual void	SetYMinMax(float fMin, float fMax, bool32 bValid = true) = 0;
	
	virtual bool32	RegionXToCurvePos(int32 x, OUT int32& iPos)		{ return false; }
	virtual bool32	CurvePosToRegionX(int32 iPos, OUT int32& x)		{ return false; }
	virtual bool32	RegionXToCurveID(int32 x, OUT int32& id)		{ return false; }

public:
	uint32			GetPickColor()									{ return m_uiPickColor; }
	static CDrawingCurve*	GetCurve(uint32 iPickColor);
	
public:
	bool32			m_bSelfDraw;
	uint32			m_iFlag;	
	static	uint32	KSelect;//被选中(内部使用)
	
private:
	uint32			m_uiPickColor;
	static uint32	g_uiPickColor;
	static RBIndex	g_MapIndex;
};




#endif

