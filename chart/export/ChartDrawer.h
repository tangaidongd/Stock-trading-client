#ifndef _CHART_DRAWER_H_
#define _CHART_DRAWER_H_

#include "ChartCurve.h"
#include "IRegionData.h"
#include "chartexport.h"
class CIoViewBase;

class CHART_EXPORT CChartDrawer : public CNodeDrawer
{
public:
	enum E_ChartDrawStyle
	{
		// 走势
		EKDSTrendPrice		= 0,
		EKDSTrendJunXian,			// 走势均线

		// K线
		EKDSNormal,					// 空心线
		EKDSFillRect,				// 实心
		
		EKDSClose,					// 收盘
		
		EKDSAmerican,				// 美国线

		// Tick线
		EKDSTickPrice,				// Tick线
		
		EKDSTower,					// k-宝塔线

		EKDSArbitrageKline,			// 套利K线
	};

public:
	CChartDrawer(IIoViewBase &IoView, E_ChartDrawStyle eChartDrawType);
	~CChartDrawer() {}

	// from CNodeDrawer
public:
	virtual void	DrawNodes(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, IN CChartCurve &ChartCurve, const CArray<CNodeDrawerData, CNodeDrawerData&> &aNodeDrawerDatas);

private:
	void			DrawNodesAsCurve(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount);	// 串起来，连成曲线
	void			DrawNodesAsAlone(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount);	// 独立的个体显示

public:
	void			SetColors(CArray<COLORREF, COLORREF>& aColors);
	void			SetColorFlag(bool32 bFromView)	{ m_bGetClrFromView = bFromView; }

	// 使用单独的指定颜色, CLR_NONE 为不使用单独颜色
	void			SetSingleColor(COLORREF clr=CLR_NONE){ m_clrSingle = clr; }
	COLORREF		GetSingleColor() const { return m_clrSingle; }
public:
	E_ChartDrawStyle	m_eChartDrawType;	

private:	
	IIoViewBase		&m_IoView;

	// 曲线的颜色可以由 View 强制设置. (类似五彩 K 线的需求)
private:
	bool32			m_bGetClrFromView;	
	CArray<COLORREF, COLORREF> m_aColorsFromView;

	COLORREF		m_clrSingle;	// 使用单独的指定颜色, CLR_NONE 为不使用单独颜色
};

#endif

