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
		// ����
		EKDSTrendPrice		= 0,
		EKDSTrendJunXian,			// ���ƾ���

		// K��
		EKDSNormal,					// ������
		EKDSFillRect,				// ʵ��
		
		EKDSClose,					// ����
		
		EKDSAmerican,				// ������

		// Tick��
		EKDSTickPrice,				// Tick��
		
		EKDSTower,					// k-������

		EKDSArbitrageKline,			// ����K��
	};

public:
	CChartDrawer(IIoViewBase &IoView, E_ChartDrawStyle eChartDrawType);
	~CChartDrawer() {}

	// from CNodeDrawer
public:
	virtual void	DrawNodes(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, IN CChartCurve &ChartCurve, const CArray<CNodeDrawerData, CNodeDrawerData&> &aNodeDrawerDatas);

private:
	void			DrawNodesAsCurve(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount);	// ����������������
	void			DrawNodesAsAlone(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount);	// �����ĸ�����ʾ

public:
	void			SetColors(CArray<COLORREF, COLORREF>& aColors);
	void			SetColorFlag(bool32 bFromView)	{ m_bGetClrFromView = bFromView; }

	// ʹ�õ�����ָ����ɫ, CLR_NONE Ϊ��ʹ�õ�����ɫ
	void			SetSingleColor(COLORREF clr=CLR_NONE){ m_clrSingle = clr; }
	COLORREF		GetSingleColor() const { return m_clrSingle; }
public:
	E_ChartDrawStyle	m_eChartDrawType;	

private:	
	IIoViewBase		&m_IoView;

	// ���ߵ���ɫ������ View ǿ������. (������� K �ߵ�����)
private:
	bool32			m_bGetClrFromView;	
	CArray<COLORREF, COLORREF> m_aColorsFromView;

	COLORREF		m_clrSingle;	// ʹ�õ�����ָ����ɫ, CLR_NONE Ϊ��ʹ�õ�����ɫ
};

#endif

