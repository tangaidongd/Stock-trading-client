#ifndef _INDEX_DRAWER_H_
#define _INDEX_DRAWER_H_

#include "ChartCurve.h"
#include "chartexport.h"
class CIoViewBase;

class CHART_EXPORT CIndexDrawer : public CNodeDrawer
{
public:
	enum E_IndexDrawStyle
	{
		EIDSVolStick			= 1,
		EIDSStick				= 2,
		EIDSCircle				= 3,
		EIDSLineStick			= 4,
		EIDSCrossDot			= 5,
		EIDSColorStick			= 6,
		EIDSStickLingxianFs		= 7,
		EIDSDot					= 8,
		
		// fangz zhibiao Add
		EIDSDrawLine			= 9,
		EIDSPolyLine			= 10,
		EIDSStickLine			= 11,
		EIDSDrawIcon			= 12,
		EIDSDrawText			= 13,
		EIDSDrawKLine			= 14,
		EIDSDrawNum				= 15,		
		EIDSDrawBand			= 20,
		EIDSDrawTextEx			= 21,
	};

	// ��������Կ��Ժ�����ĵ���
	enum E_IndexDrawStyleEx
	{		
		EIDSENone				= 0,
		EIDSENoDraw				= 15,				
		EIDSENoTitle			= 16,			
	};

	// ��ʾK�߻������͵�
	// �˴���enum������CIoViewKLine�е�k�߻��Ʒ���һ��
	enum E_IndexKlineDrawStyle
	{
		EIKDSNormal = 0,	// ��ͨ������
		EIKDSFill	= 1,	// ʵ����
		EIKDSAmerican = 2,	// ������
		EIKDSClose	= 3,	// ������
		EIKDSTower	= 4,	// ������

		EIKDSByMainKline = 0xffff,	// ʹ������ͼ��ͬ��k�߻��Ʒ���
		EIKDSCount			// ������Чֵ
	};

	/*
	enum E_IndexDrawStyle
	{
		EIDSVolStick			= 1,
		EIDSStickVolFs			= 2,
		EIDSCircle				= 3,
		EIDSLineStick			= 4,
		EIDSCrossDot			= 5,
		EIDSStick				= 6,
		EIDSStickLingxianFs		= 7,
		EIDSDot					= 8,
	
		// fangz zhibiao Add
		EIDSDrawLine				= 9,
		EIDSPolyLine				= 10,
		EIDSStickLine				= 11,
		EIDSDrawIcon				= 12,
		EIDSDrawText				= 13,
		EIDSDrawKLine				= 14
	};
	*/

public:
	CIndexDrawer(IIoViewBase &IoView, E_IndexDrawStyle eIndexDrawType, E_IndexDrawStyleEx eIndexDrawTypeEx, int32 iThick, long lColor, E_IndexKlineDrawStyle eIndexKlineDrawStyle=EIKDSNormal);
	~CIndexDrawer();
	
	// from CNodeDrawer
public:
	virtual void	DrawNodes(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, IN CChartCurve &ChartCurve, const CArray<CNodeDrawerData, CNodeDrawerData&> &aNodeDrawerDatas);
	
	virtual bool32	BeDrawTitleText();
	virtual bool32	BeNoDraw();

private:
	void			DrawNodesAsCurve(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, COLORREF clrIndex, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount, bool32 bDot);		// ����������������
	void			DrawNodesAsAlone(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, COLORREF clrIndex, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount);		// �����ĸ�����ʾ
	void			DrawNodesAsText(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, COLORREF clrIndex, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount);		// EIDSDrawText
	void			DrawNodesAsNumText(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, COLORREF clrIndex, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount);		// EIDSDrawText
	void			DrawNodesAsTextEx(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, COLORREF clrIndex, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount);		// EIDSDrawTextEx
	void			DrawNodesAsIcon(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, COLORREF clrIndex, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount);		// EIDSDrawIcon
	void			DrawNodesAsLine(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, COLORREF clrIndex, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount);		// EIDSDrawLine
	void			DrawNodesAsStickLine(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, COLORREF clrIndex, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount);	// EIDSSticKLine	
	void			DrawNodesAsKLine(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, COLORREF clrIndex, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount);		// EIDSDrawKLine
	void			DrawNodesAsBand(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, COLORREF clrIndex, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount);	// EIDSDrawBand

public:
	E_IndexDrawStyle	m_eIndexDrawType;
	E_IndexDrawStyleEx	m_eIndexDrawTypeEx;
	E_IndexKlineDrawStyle	m_eIndexKlineDrawStyle;
	int32				m_iThick;
	long				m_lColor;		

	Image		*m_pImageUp;
	Image		*m_pImageDown;

	Image		*m_pImageUp2;
	Image		*m_pImageDown2;

private:
	IIoViewBase			&m_IoView;
};

#endif

