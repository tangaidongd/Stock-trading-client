#ifndef _SELF_DRAW_H_2008_08_
#define _SELF_DRAW_H_2008_08_

/*#include "ChartRegion.h"*/
#include "typedef.h"
#include "DrawingCurve.h"
#include "chartexport.h"

class TiXmlElement;
class TiXmlDocument;
class CMerch;

extern const COLORREF KSelfDrawDefaultClr;

typedef enum E_SelfDrawType
{
	ESDTLine = 0,// �߶�
	ESDTLineEx,// ֱ��
	ESDTJgtd,// �۸�ͨ����
	ESDTPxzx,// ƽ��ֱ��
	ESDTYhx,// Բ����
	ESDTHjfg,// �ƽ�ָ���
	ESDTBfb,// �ٷֱ���
	ESDTBd,// ������
	ESDTJx,// ����
	ESDTJxhgd,// ���λع��
	ESDTYcxxhgd,// �ӳ����ͻع��
	ESDTXxhgd,// ���ͻع��
	ESDTZq,// ������
	ESDTFblq,// �Ѳ�������
	ESDTZs,// ������
	ESDTJejd,// �����Ƕ���
	ESDTZ45UP,// 45��
	ESDTZ45DOWN,// 45 ��
	ESDTZVertical,// ����
	ESDTZHorizontal,//ˮƽ��
	ESDTZbj,// �Ǳ��
	ESDTDbj,// �����
	ESDTText,// ����ע��
	ESDTColor,// ��ɫ����
	ESDTInvalid

}E_SelfDrawType;

class CSelfDrawCurve;
class CChartRegion;
class CHART_EXPORT CSelfDrawNode
{
public:
	CSelfDrawNode();
	~CSelfDrawNode();

	void _ToXml();
	void _FromXml ( TiXmlElement * pNode );

	void HitTest ( CChartRegion* pRegion, int32 x,int32 y, int32& hit );
	void Moving ( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC, int32 x,int32 y,int32 hit,int32 hitx,int32 hity );
	void Move ( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC, int32 x,int32 y,int32 hit,int32 hitx,int32 hity );

	void Draw ( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC );

	void DrawLine( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC );
		
	void DrawLineEx( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC );

	void DrawJgtd( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC );
		
	void DrawPxzx( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC );
		
	void DrawYhx( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC );
		
	void DrawHjfg( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC );
		
	void DrawBfb( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC );
		
	void DrawBd( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC );
		
	void DrawJx( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC );
		
	void DrawJxhgd( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC );
		
	void DrawYcxxhgd( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC );
		
	void DrawXxhgd( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC );
		
	void DrawZq( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC );
		
	void DrawFblq( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC );
		
	void DrawZs( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC );
		
	void DrawJejd( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC );
		
	void DrawZbj( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC );
		
	void DrawDbj( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC );
		
	void Draw45Degree(CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC, bool32 bUp);

	void DrawHorizontal(CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC);

	void DrawVertical(CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC);

	void DrawText( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC );
		
	void DrawDel( CChartRegion* pRegion,CSelfDrawCurve* pCurve,CMemDCEx* pDC, CMemDCEx* pPickDC );

	int32 t;
	E_SelfDrawType m_eType;
	int32 m_id1;//start time(x axis)
	int32 m_id2;//end time(x axis)
	int32 m_ide;
	float m_fVal1;//y1
	float m_fVal2;//y2
	float m_fVale;
	COLORREF m_clr;
	CString m_StrText;
	CString m_StrXml;

	// ������ص�����
	int32	m_iInternal;
	int32	m_iUserMin;
	int32	m_iUserDay;
	CMerch*	m_pMerch;
};

class CHART_EXPORT CSelfDrawCurve : public CDrawingCurve
{
public:
	CSelfDrawCurve(CChartRegion* p);
	~CSelfDrawCurve();
	
	// from CDrawingCurve
public:
	virtual void	Draw(IN CMemDCEx* pDC, IN CMemDCEx* pPickDC) { }
	virtual bool32	CalcY(OUT float &fYMin, OUT float &fYMax) { return false; }
	virtual bool32	GetYMinMax(OUT float& fYMin, OUT float& fYMax) { return false; }
	virtual void	SetYMinMax(float fMin, float fMax, bool32 bValid = true) { }

public:
	void SetT ( int32 t );
	void SetNode(CSelfDrawNode* pNode) { if (NULL != pNode)m_pNode=pNode; }
	const CSelfDrawNode* GetNode() const { return m_pNode;}	
	virtual void Draw ( CChartRegion* pRegion, CMemDCEx* pDC, CMemDCEx* pPickDC);
	void Moving ( CChartRegion* pRegion, CMemDCEx* pDC, CMemDCEx* pPickDC, int32 x,int32 y );
	void Move ( CChartRegion* pRegion, CMemDCEx* pDC, CMemDCEx* pPickDC, int32 x,int32 y );

	int32 HitTest ( CChartRegion* pRegion, int32 x,int32 y );

	// �Ƿ��ǵ�ǰ������
	bool32	NeedShow();

	int32 m_t;
private:
	int32 m_hit;//0:head 1:tail 2:body
	int32 m_hitx;
	int32 m_hity;

	CSelfDrawNode* m_pNode;
	CChartRegion* m_pParentRegion;
};
CHART_EXPORT void SDDrawBj ( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x,int32 y,bool32 bRise,COLORREF clr = 0 );
CHART_EXPORT void SDDrawText ( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x,int32 y,CString StrText,COLORREF clr = 0  );

CHART_EXPORT void SDDrawYhx ( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x1,int32 y1,int32 x2,int32 y2,COLORREF clr = 0 );
CHART_EXPORT void SDDrawJx ( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x1,int32 y1,int32 x2,int32 y2,COLORREF clr = 0 );

CHART_EXPORT void SDDrawHjfg ( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x1,int32 y1,int32 x2,int32 y2,COLORREF clr = 0 );
CHART_EXPORT void SDDrawBfb ( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x1,int32 y1,int32 x2,int32 y2,COLORREF clr = 0 );
CHART_EXPORT void SDDrawBd ( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x1,int32 y1,int32 x2,int32 y2,COLORREF clr = 0 );

CHART_EXPORT void SDDrawZq ( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x1,int32 y1,int32 x2,int32 y2,COLORREF clr = 0 );

CHART_EXPORT void SDDrawJgtd ( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x1,int32 y1,int32 x2,int32 y2,int32 x3,int32 y3,COLORREF clr = 0 );
CHART_EXPORT void SDDrawPxzx ( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x1,int32 y1,int32 x2,int32 y2,int32 x3,int32 y3,COLORREF clr = 0 );

CHART_EXPORT void SDDrawFblq ( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x,int32 y,bool32 bRise ,COLORREF clr = 0 );

CHART_EXPORT void SDDrawZs ( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x1,int32 y1,int32 x2,int32 y2,COLORREF clr = 0 );
CHART_EXPORT void SDDrawJejd ( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x1,int32 y1,int32 x2,int32 y2,COLORREF clr = 0 );

CHART_EXPORT void SDDrawXxhgd ( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x1,int32 y1,int32 x2,int32 y2,int32 id1,int32 id2,COLORREF clr = 0 );
CHART_EXPORT void SDDrawJxhgd ( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x1,int32 y1,int32 x2,int32 y2,int32 id1,int32 id2,COLORREF clr = 0 );
CHART_EXPORT void SDDrawYcxxhgd ( CChartRegion* pRegion, CSelfDrawCurve* pCurve, CMemDCEx* pDC,CMemDCEx* pPickDC,int32 x1,int32 y1,int32 x2,int32 y2,int32 id1,int32 id2,COLORREF clr = 0 );

CHART_EXPORT void CalcParallel ( int32 x1,int32 y1,int32 x2,int32 y2, int32 x3,int32 y3,int32& x4,int32& y4 );
CHART_EXPORT void CalcParallel2 ( int32 x1,int32 y1,int32 x2,int32 y2, int32& x3,int32& y3,int32& x4,int32& y4,int32& x5,int32& y5,int32& x6,int32& y6 );

CHART_EXPORT void CaclTwoPoint ( CRect Rect,CPoint p1,CPoint p2,CPoint* pts);

#endif //_SELF_DRAW_H_2008_08_