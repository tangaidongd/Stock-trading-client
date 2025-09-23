#include "StdAfx.h"
#include "ChartCurve.h"
#include "ChartRegion.h"
#include "IndexDrawer.h"
#include "coding.h"
#include "XLTimerTrace.h"
#include "TabSplitWnd.h"
#include "../res/resource.h"

const int32 KDefaultShowSelectedFlagCount = 10;	// ����ѡ���ߣ� Ĭ����ʾN����


CIndexDrawer::CIndexDrawer(IIoViewBase &IoView, E_IndexDrawStyle eIndexDrawType, E_IndexDrawStyleEx eIndexDrawTypeEx, int32 iThick, long lColor, E_IndexKlineDrawStyle eIndexKlineDrawStyle/*=EIKDSNormal*/)
:m_IoView(IoView)
{
	m_pImageUp	   = NULL;
	m_pImageDown  = NULL;

	m_pImageUp2   = NULL;
	m_pImageDown2 = NULL;

	m_eIndexDrawType	= eIndexDrawType;
	m_eIndexDrawTypeEx	= eIndexDrawTypeEx;
	m_iThick			= iThick;
	if ( -1 == iThick )
	{
		m_iThick = 1;
	}

	m_lColor			= lColor;
	m_eIndexKlineDrawStyle = eIndexKlineDrawStyle;
	if ( m_eIndexKlineDrawStyle < 0 || m_eIndexKlineDrawStyle >= EIKDSCount )//lint !e568
	{
		m_eIndexKlineDrawStyle = EIKDSNormal;
	}
}

CIndexDrawer::~CIndexDrawer()
{
	DEL(m_pImageUp);
	DEL(m_pImageDown);

	DEL(m_pImageUp2);
	DEL(m_pImageDown2);
}
 
void CIndexDrawer::DrawNodes(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, IN CChartCurve &ChartCurve, const CArray<CNodeDrawerData, CNodeDrawerData&> &aNodeDrawerDatas)
{
	if (aNodeDrawerDatas.GetSize() <= 0)
		return;
	
	// 
	COLORREF clrIndex = m_lColor;
	if (m_lColor < 0 || m_lColor > 0xffffff)	// δָ����ɫ, ���Զ���ָ������ɫ������ȡɫ
	{
		int32 iColorLoop = aNodeDrawerDatas[0].m_iCurveLoop % 16;
		E_SysColor eColor = (E_SysColor)(ESCGuideLine1 + iColorLoop);
		clrIndex = m_IoView.GetIoViewColor(eColor);
		ChartCurve.SetOccupyIndexLineClr(true);	// ռ���˴���ɫ
	}
	else
	{
		clrIndex = m_lColor;
		ChartCurve.SetOccupyIndexLineClr(false);
	}

	// �����黭������ɫ
	ChartCurve.m_clrTitleText = clrIndex;

	// ���㵱ǰ��ѡ�б�־�ļ������
	int32 iShowSelectFlagPeriod = (int32)((float)aNodeDrawerDatas.GetSize() / KDefaultShowSelectedFlagCount + 0.5f);
	if (iShowSelectFlagPeriod < 1)
		iShowSelectFlagPeriod = 1;

	// ������ǰ��return, ��Щ��ɫ���õĻ�Ӱ�쵽DrawTitle ���������ɫ
	if ( EIDSENoDraw == m_eIndexDrawTypeEx )
	{
		return;
	}

	// 
	CNodeDrawerData *pNodeDrawerDatas = (CNodeDrawerData *)aNodeDrawerDatas.GetData();
	int32 iNodeCount = aNodeDrawerDatas.GetSize();

	if (EIDSVolStick	== m_eIndexDrawType ||
		EIDSCircle		== m_eIndexDrawType ||
		EIDSCrossDot	== m_eIndexDrawType ||
		EIDSStick		== m_eIndexDrawType ||
		EIDSColorStick  == m_eIndexDrawType
	   )
	{
		// ��������ڵ�
		DrawNodesAsAlone(pDC, pPickDC, clrIndex, iShowSelectFlagPeriod, ChartCurve, pNodeDrawerDatas, iNodeCount);
	}
	else if ( EIDSDrawText == m_eIndexDrawType )
	{
		// ������(��ָ��λ�û�������)
		DrawNodesAsText(pDC, pPickDC, clrIndex, iShowSelectFlagPeriod, ChartCurve, pNodeDrawerDatas, iNodeCount);
	}
	else if ( EIDSDrawNum == m_eIndexDrawType )
	{
		// ������
		DrawNodesAsNumText(pDC, pPickDC, clrIndex, iShowSelectFlagPeriod, ChartCurve, pNodeDrawerDatas, iNodeCount);
	}
	else if (EIDSDrawTextEx == m_eIndexDrawType)
	{
		// ������EX(��ָ��λ�û���ֵ)
		DrawNodesAsTextEx(pDC, pPickDC, clrIndex, iShowSelectFlagPeriod, ChartCurve, pNodeDrawerDatas, iNodeCount);
	}
	else if ( EIDSDrawIcon == m_eIndexDrawType )
	{
		// ��ͼ��
		DrawNodesAsIcon(pDC, pPickDC, clrIndex, iShowSelectFlagPeriod, ChartCurve, pNodeDrawerDatas, iNodeCount);
	}
	else if ( EIDSDrawLine == m_eIndexDrawType )
	{
		// ����
		DrawNodesAsLine(pDC, pPickDC, clrIndex, iShowSelectFlagPeriod, ChartCurve, pNodeDrawerDatas, iNodeCount);
	}
	else if ( EIDSStickLine == m_eIndexDrawType )
	{
		// ��״��
		DrawNodesAsStickLine(pDC, pPickDC, clrIndex, iShowSelectFlagPeriod, ChartCurve, pNodeDrawerDatas, iNodeCount);
	}
	else if ( EIDSDrawBand == m_eIndexDrawType)
	{
		// ��״��
		DrawNodesAsBand(pDC, pPickDC, clrIndex, iShowSelectFlagPeriod, ChartCurve, pNodeDrawerDatas, iNodeCount);
	}
	else if ( EIDSDrawKLine == m_eIndexDrawType && EIKDSClose != m_eIndexKlineDrawStyle )
	{
		// �� K ��
		DrawNodesAsKLine(pDC, pPickDC, clrIndex, iShowSelectFlagPeriod, ChartCurve, pNodeDrawerDatas, iNodeCount);
	}	
	else if (EIDSDot == m_eIndexDrawType)
	{
		// ������
		DrawNodesAsCurve(pDC, pPickDC, clrIndex, iShowSelectFlagPeriod, ChartCurve, pNodeDrawerDatas, iNodeCount, true);		
	}
	else
	{
		// ������
		DrawNodesAsCurve(pDC, pPickDC, clrIndex, iShowSelectFlagPeriod, ChartCurve, pNodeDrawerDatas, iNodeCount, false);		
	}
}

bool32 CIndexDrawer::BeDrawTitleText()
{
	if ( EIDSENoTitle == m_eIndexDrawTypeEx )
	{
		return false;
	}

	return true;
}

bool32 CIndexDrawer::BeNoDraw()
{
	if ( EIDSENoDraw == m_eIndexDrawTypeEx )
	{
		return true;
	}

	return false;
}

// �Ż������

/*
void CIndexDrawer::DrawNodesAsCurve(IN CMemDCEx* pDC, IN CMemDCEx* pPickDC, COLORREF clrIndex, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount, bool32 bDot)
{
	ASSERT(NULL != pNodeDrawerDatas && iNodeCount > 0);

	CPen Pen,*pOldPen = NULL;
	if(bDot)
	{
		Pen.CreatePen(PS_DOT, m_iThick, clrIndex);
	}
	else
	{
		Pen.CreatePen(PS_SOLID, m_iThick, clrIndex);
	}

	
	pOldPen = (CPen*)pDC->SelectObject(&Pen);

	pPickDC->BeginDraw(&ChartCurve, 3);

	// 
	//xiali XL0005 �����ص���
	CArray<CPoint, CPoint &>	aDrawPoints;
	aDrawPoints.SetSize(iNodeCount);
	CPoint		*pDrawPoints	=	aDrawPoints.GetData();
	int32		iDrawPointsCount	=	0;
	int32		iLastY	=	-1;
	int32		iLastValidPos = -1;	// ���һ����Ч�ڵ�

	// ȥ����DRAWNULL ����Щ��ļ�¼
	map<int, CPoint> mapPointReal;

	//
	for (int32 i = 0; i < iNodeCount; i++)
	{		
		//�������ص��жϣ�
		if (CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid))
		{
			// �Ƿ��ڵ������Ļ�,�ᵼ��ָ���߶ϵ�
			
			// ����ǷǷ���,����ǰ�ҵ�һ���Ϸ��Ľڵ�,����������Y����
			// ���ǰ�Ϸ��ڵ�Ϊ�ָ�ڵ㣬����ø÷Ƿ��ڵ�
			if ( -1 != iLastY && iLastValidPos >= 0 )
			{
				pDrawPoints[ iDrawPointsCount ].x	=	pNodeDrawerDatas[i].m_iX;
				pDrawPoints[ iDrawPointsCount ].y	=	iLastY;
				
				if ( pDrawPoints[iDrawPointsCount-1]!=pDrawPoints[iDrawPointsCount] )
				{
					if (!CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KDrawNULL))
					{
						// ����ǲ����Ľڵ�
						mapPointReal[iDrawPointsCount] = pDrawPoints[iDrawPointsCount];
					}	

					iDrawPointsCount++;
				}
			}

			iLastValidPos = CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueDivide) ? -1 : iLastValidPos;	// �ĵ����ĵ㲻������ֵ
		}
		else
		{
			//�Ϸ��ڵ�
			iLastValidPos = CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueDivide) ? -1 : i;	// �ĵ����ĵ㲻������ֵ

			pDrawPoints[ iDrawPointsCount ].x	=	pNodeDrawerDatas[i].m_iX;
			pDrawPoints[ iDrawPointsCount ].y	=	pNodeDrawerDatas[i].m_iClose;
			if ( iDrawPointsCount ==0 || pDrawPoints[iDrawPointsCount-1]!=pDrawPoints[iDrawPointsCount] )
			{
				//��һ�� || ���ڲ���ȵĵ�
				iLastY	=	pDrawPoints[ iDrawPointsCount ].y;

				if (!CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KDrawNULL))
				{
					// ����ǲ����Ľڵ�
					mapPointReal[iDrawPointsCount] = pDrawPoints[iDrawPointsCount];
				}	

				iDrawPointsCount++;
			}
		}	
	}

	if (mapPointReal.size() == iDrawPointsCount)
	{
		// �������
		pDC->Polyline(pDrawPoints, iDrawPointsCount);
	}
	else
	{
		// ����Ҫ���յĽڵ�, �ֶλ�.	
		CArray<CPoint, CPoint> aPointNow;

		//
		int32 iIndexLast = -1;
		for (map<int, CPoint>::iterator it = mapPointReal.begin(); it != mapPointReal.end(); ++it)
		{
			int32 iIndexNow = it->first;
			CPoint pt = it->second;

			//
			if (-1 == iIndexLast)
			{	
				aPointNow.Add(pt);
			}
			else
			{
				if (iIndexNow == iIndexLast + 1)
				{
					// ��������һ�����, ������������
					aPointNow.Add(pt);
				}
				else
				{
					// �Ͽ���, ��ǰ��һ�λ�����
					if (1 == aPointNow.GetSize())
					{
						// ����
						pDC->SetPixel(aPointNow[0], clrIndex);
					}
					else
					{
						// ����
						CPoint* pPtNow = (CPoint*)aPointNow.GetData();
						pDC->Polyline(pPtNow, aPointNow.GetSize());
					}
					
					// ���
					aPointNow.RemoveAll();

					// ���ϵ�ǰ��
					aPointNow.Add(pt);
				}
			}

			//
			iIndexLast = iIndexNow;
		}

		// ���ж��, ������
		CPoint* pPtNow = (CPoint*)aPointNow.GetData();
		pDC->Polyline(pPtNow, aPointNow.GetSize());
		
		aPointNow.RemoveAll();
	}

	// ����PickDC penΪ3��������polyLineʱ����������ض�ֵ()�����������ϵ���ʧ��ֻ���鷳����
	if ( iDrawPointsCount > 0 )
	{
		// ��300��
		const int32 iPickDrawPer = 300;
		const int32 iPickDrawLoop = iDrawPointsCount / iPickDrawPer;

		pPickDC->MoveTo(pDrawPoints[0]);

		int32 iPickDrawed = 0;
		for ( ; iPickDrawed < iPickDrawLoop; iPickDrawed++)
		{
			pPickDC->PolylineTo(pDrawPoints + iPickDrawed * iPickDrawPer, iPickDrawPer);
		}

		pPickDC->PolylineTo(pDrawPoints + iPickDrawed * iPickDrawPer, iDrawPointsCount % iPickDrawPer);
	}
	
	//pPickDC->Polyline(pDrawPoints, iDrawPointsCount);
	pPickDC->EndDraw();

	// ��ѡ�б�־
	if (ChartCurve.IsSelect())
	{
		int32 iROPBack = pDC->SetROP2(R2_XORPEN);
		for (int32 i = 0; i < iNodeCount; i += iShowSelectFlagPeriod)
		{
			if (CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid))
			{
				continue;
			}
			
			int32 x1 = pNodeDrawerDatas[i].m_iX		- 4;	
			int32 x2 = pNodeDrawerDatas[i].m_iX		+ 4;
			int32 y1 = pNodeDrawerDatas[i].m_iClose - 4;	
			int32 y2 = pNodeDrawerDatas[i].m_iClose + 4;
			
			pDC->_FillSolidRect(CRect(x1, y1, x2, y2), 0xffffff);
		}
		pDC->SetROP2(iROPBack);
	}
	
	// 
	if (NULL != pOldPen)
		pDC->SelectObject(pOldPen);

	Pen.DeleteObject();	
}
*/

void CIndexDrawer::DrawNodesAsCurve(IN CMemDCEx* pDC, IN CMemDCEx* pPickDC, COLORREF clrIndex, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount, bool32 bDot)
{
	if(NULL == pNodeDrawerDatas || iNodeCount <= 0)
	{
		return;
	}	
	ASSERT(NULL != pNodeDrawerDatas && iNodeCount > 0);

	Color clr(GetRValue(clrIndex), GetGValue(clrIndex), GetBValue(clrIndex));
	Pen penNow(clr, (float)m_iThick);

	if (bDot)
	{
 		float dashValues[] = {2, 4};  //���س���Ϊ2����೤��Ϊ4 ������
 		penNow.SetDashPattern(dashValues, 2);
		//penNow.SetDashStyle(DashStyleDot);
	}

	Gdiplus::Graphics graphics(pDC->GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);

	pPickDC->BeginDraw(&ChartCurve, 3);

	// 
	//xiali XL0005 �����ص���
	CArray<Point, Point&>	aDrawPoints;
	aDrawPoints.SetSize(iNodeCount);
	Point		*pDrawPoints	=	aDrawPoints.GetData();
	int32		iDrawPointsCount	=	0;
	int32		iLastY	=	-1;
	int32		iLastValidPos = -1;	// ���һ����Ч�ڵ�

	// ȥ����DRAWNULL ����Щ��ļ�¼
	map<int, Point> mapPointReal;

	//
	for (int32 i = 0; i < iNodeCount; i++)
	{		
		//�������ص��жϣ�
		if (CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid))
		{
			// �Ƿ��ڵ������Ļ�,�ᵼ��ָ���߶ϵ�
			
			// ����ǷǷ���,����ǰ�ҵ�һ���Ϸ��Ľڵ�,����������Y����
			// ���ǰ�Ϸ��ڵ�Ϊ�ָ�ڵ㣬����ø÷Ƿ��ڵ�
			if ( -1 != iLastY && iLastValidPos >= 0 )
			{
				pDrawPoints[ iDrawPointsCount ].X	=	pNodeDrawerDatas[i].m_iX;
				pDrawPoints[ iDrawPointsCount ].Y	=	iLastY;
				
				if ( !pDrawPoints[iDrawPointsCount-1].Equals(pDrawPoints[iDrawPointsCount]) )
				{
					if (!CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KDrawNULL))
					{
						// ����ǲ����Ľڵ�
						mapPointReal[iDrawPointsCount] = pDrawPoints[iDrawPointsCount];
					}	

					iDrawPointsCount++;
				}
			}

			iLastValidPos = CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueDivide) ? -1 : iLastValidPos;	// �ĵ����ĵ㲻������ֵ
		}
		else
		{
			//�Ϸ��ڵ�
			iLastValidPos = CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueDivide) ? -1 : i;	// �ĵ����ĵ㲻������ֵ

			pDrawPoints[ iDrawPointsCount ].X	=	pNodeDrawerDatas[i].m_iX;
			pDrawPoints[ iDrawPointsCount ].Y	=	pNodeDrawerDatas[i].m_iClose;
			if ( iDrawPointsCount ==0 || !(pDrawPoints[iDrawPointsCount-1].Equals(pDrawPoints[iDrawPointsCount])) )
			{
				//��һ�� || ���ڲ���ȵĵ�
				iLastY	=	pDrawPoints[ iDrawPointsCount ].Y;

				if (!CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KDrawNULL))
				{
					// ����ǲ����Ľڵ�
					mapPointReal[iDrawPointsCount] = pDrawPoints[iDrawPointsCount];
				}	

				iDrawPointsCount++;
			}
		}	
	}

	if (mapPointReal.size() == iDrawPointsCount)
	{
		// �������
	//	graphics.DrawCurve(&penNow, pDrawPoints, iDrawPointsCount);
		graphics.DrawLines(&penNow, pDrawPoints, iDrawPointsCount);
	}
	else
	{
		// ����Ҫ���յĽڵ�, �ֶλ�.	
		CArray<Point, Point> aPointNow;

		//
		int32 iIndexLast = -1;
		for (map<int, Point>::iterator it = mapPointReal.begin(); it != mapPointReal.end(); ++it)
		{
			int32 iIndexNow = it->first;
			Point pt = it->second;

			//
			if (-1 == iIndexLast)
			{	
				aPointNow.Add(pt);
			}
			else
			{
				if (iIndexNow == iIndexLast + 1)
				{
					// ��������һ�����, ������������
					aPointNow.Add(pt);
				}
				else
				{
					// �Ͽ���, ��ǰ��һ�λ�����
					if (1 == aPointNow.GetSize())
					{
						// ����
						pDC->SetPixel(CPoint(aPointNow[0].X, aPointNow[0].Y), clrIndex);
						
					}
					else
					{
						// ����
						Point* pPtNow = (Point*)aPointNow.GetData();
					//	graphics.DrawCurve(&penNow, pPtNow, aPointNow.GetSize());
						graphics.DrawLines(&penNow, pPtNow, aPointNow.GetSize());
					}
					
					// ���
					aPointNow.RemoveAll();

					// ���ϵ�ǰ��
					aPointNow.Add(pt);
				}
			}

			//
			iIndexLast = iIndexNow;
		}

		// ���ж��, ������
		Point* pPtNow = (Point*)aPointNow.GetData();
//		graphics.DrawCurve(&penNow, pPtNow, aPointNow.GetSize());
		graphics.DrawCurve(&penNow, pPtNow, aPointNow.GetSize());
		
		aPointNow.RemoveAll();
	}

	// ����PickDC penΪ3��������polyLineʱ����������ض�ֵ()�����������ϵ���ʧ��ֻ���鷳����
	if ( iDrawPointsCount > 0 )
	{
		// ��300��
		const int32 iPickDrawPer = 300;
		const int32 iPickDrawLoop = iDrawPointsCount / iPickDrawPer;

		pPickDC->MoveTo(pDrawPoints[0].X,pDrawPoints[0].Y);

		CArray<CPoint, CPoint&> aPoints;
		aPoints.SetSize(iNodeCount);
		CPoint	*pPoints = aPoints.GetData();
		int32 iLength = aDrawPoints.GetSize();
		for (int32 iPt=0; iPt<iLength; iPt++)
		{
			pPoints[iPt].x = pDrawPoints[iPt].X;
			pPoints[iPt].y = pDrawPoints[iPt].Y;
		}

		int32 iPickDrawed = 0;
		for ( ; iPickDrawed < iPickDrawLoop; iPickDrawed++)
		{
			pPickDC->PolylineTo(pPoints + iPickDrawed * iPickDrawPer, iPickDrawPer);
		}

		pPickDC->PolylineTo(pPoints + iPickDrawed * iPickDrawPer, iDrawPointsCount % iPickDrawPer);
	}
	
	pPickDC->EndDraw();

	// ��ѡ�б�־
	if (ChartCurve.IsSelect())
	{
		int32 iROPBack = pDC->SetROP2(R2_XORPEN);
		for (int32 i = 0; i < iNodeCount; i += iShowSelectFlagPeriod)
		{
			if (CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid))
			{
				continue;
			}
			
			int32 x1 = pNodeDrawerDatas[i].m_iX		- 4;	
			int32 x2 = pNodeDrawerDatas[i].m_iX		+ 4;
			int32 y1 = pNodeDrawerDatas[i].m_iClose - 4;	
			int32 y2 = pNodeDrawerDatas[i].m_iClose + 4;
			
			pDC->_FillSolidRect(CRect(x1, y1, x2, y2), 0xffffff);
		}
		pDC->SetROP2(iROPBack);
	}
}

//��ͬX�����ص��һ�����ڸ���ǰһ�����ڵķ���������ʵ�����軭�ĵ�
//ÿ������һ��������ͼ�Σ���ͼ�α����ڵ�xLeft, xRight, yBottom, iValue�����ƣ������ڴ�����ʱ�򣬻��ص���ͬһ������X���ص���
//����ÿ��ͼ�Σ���ȡ��iValue���ܻ�������ͬ����������ͼ����ȡ�Ķ���m_iVolumn�������ص������ѡ��������
//�ȼ��� ��Ҫ���ľ�������Ӧ��pNodeDrawerDatas �� ���������������и��ǿ���
//����ʵ����Ҫ���ĸ��� - ʵ�ʷ��صľ��κ�ָ�����Դ���ͼ�ұߵ���ߵ�˳������ģ�����ʱ��Ҫ�Ӻ��滭��ǰ��
static int32	CalcNeedCalcNodesOverlapVol(IN CNodeDrawerData *pNodeDrawerDatas, IN int32 iNodeCount, IN CChartCurve &ChartCurve, 
								 OUT CArray<CRect, CRect &>	&aNeedDrawRects, OUT CArray<CNodeDrawerData *, CNodeDrawerData *>& aNeedDrawNodeDrawerDatas)
{
	int32	iNeedDrawNodeCount	=	0;
	if ( NULL==pNodeDrawerDatas || iNodeCount < 1 )
	{	
		return	0;
	}


	CRect	rectDraw	=	ChartCurve.GetChartRegion().GetRectCurves();

	aNeedDrawRects.SetSize( iNodeCount );
	aNeedDrawNodeDrawerDatas.SetSize( iNodeCount );

	ASSERT( ChartCurve.GetChartRegion().m_aXAxisNodes.GetSize() > 0 );

	if ( ChartCurve.GetChartRegion().m_aXAxisNodes[0].m_fPixelWidthPerUnit < 1.0 )
	{
		//����1.0�ģ�û��Ҫ�����ص����㣬����С��1.0���ظ���
		for (int32 i = iNodeCount - 1; i >= 0 ; i--)
		{
			if (CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid))
			{
				continue;
			}
			
			//ͬX�����ǰ���û�����ڵĸ�(��һ��)����ǰ��ı�����ĸ��ǣ�������ǰ��Ѱ���Ƿ���ͬX��ģ��ڱȽ�
			//		���ǰ��ıȺ���ĸߣ������ı���ӣ�����Ҫ��ǰ������
			//�������ڴӺ��濪ʼ���㣬��任����ǰ��ָǰ���¼�ģ���С��ָfValue�Ĵ�С�������������Ͼ���ԽTop(YֵԽС)��Խ��:
			//				1. û��ǰ��󣬿��Ժ��Լ�����
			//				2. ��ǰ��Ĵ���ӽ�ȥ
			//����û����������ͼ��k��ͼ֮�����������ͼ�����ֻ������Խ�һ���Ż�
			//��Ϊm_iVolumn
			
			CNodeDrawerData	&node	=	pNodeDrawerDatas[i];
			//������
			if ( 0 == iNeedDrawNodeCount || aNeedDrawNodeDrawerDatas[iNeedDrawNodeCount-1]->m_iX != node.m_iX
				|| aNeedDrawNodeDrawerDatas[iNeedDrawNodeCount-1]->m_iVolumn > node.m_iVolumn )
			{
				//û����ӹ� || ����ͬX��� || ��ǰ���¼���Ǹ�Ҫ�󣨲����ȣ�(m_iVolumnԽ�󣬾���Խbottom��������value��Խ��)
				aNeedDrawNodeDrawerDatas[ iNeedDrawNodeCount ]	=	pNodeDrawerDatas + i;
				
				//��������
				int32 x1 = 0, x2 = 0, x3 = 0;
				pNodeDrawerDatas[i].CalcXPos(x1, x2, x3);
				
				int32 y1 = 0, y2 = 0;
				
				y1 = rectDraw.bottom;
				y2 = pNodeDrawerDatas[i].m_iVolumn;
				
				aNeedDrawRects[ iNeedDrawNodeCount ].left	=	x1;
				aNeedDrawRects[ iNeedDrawNodeCount ].top		=	y2;
				aNeedDrawRects[ iNeedDrawNodeCount ].right	=	x3;
				aNeedDrawRects[ iNeedDrawNodeCount ].bottom	=	y1;
				
				iNeedDrawNodeCount++;
			}
		}
	}
	else
	{
		//ֱ�Ӹ��ƣ��ų�Invalid
		for (int32 i = iNodeCount - 1; i >= 0 ; i--)
		{
			if (CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid))
			{
				continue;
			}
			
		
			//������ - ��ǰ�����
			
			
			aNeedDrawNodeDrawerDatas[ iNeedDrawNodeCount ]	=	pNodeDrawerDatas + i;
			//��������
			int32 x1 = 0, x2 = 0, x3 = 0;
			pNodeDrawerDatas[i].CalcXPos(x1, x2, x3);
			
			int32 y1 = 0, y2 = 0;
			
			y1 = rectDraw.bottom;
			y2 = pNodeDrawerDatas[i].m_iVolumn;
			
			aNeedDrawRects[ iNeedDrawNodeCount ].left	=	x1;
			aNeedDrawRects[ iNeedDrawNodeCount ].top		=	y2;
			aNeedDrawRects[ iNeedDrawNodeCount ].right	=	x3;
			aNeedDrawRects[ iNeedDrawNodeCount ].bottom	=	y1;
			
			iNeedDrawNodeCount++;
		}
	}
	
	aNeedDrawRects.SetSize(iNeedDrawNodeCount);
	aNeedDrawNodeDrawerDatas.SetSize(iNeedDrawNodeCount);
	return	iNeedDrawNodeCount;
}


//��ͬX�����ص������ľ��θ�������С�ľ��� - ��������ͼ�ͷ�ʱ��Vol������ֻ����ͬһX����ߵ��Ǹ�
//�����뷵��ֵ�뿼���ص����Ǹ���ͬ
//����ʵ����Ҫ���ĸ��� - ʵ�ʷ��صľ��κ�ָ�����Դ���ͼ�ұߵ���ߵ�˳������ģ�����ʱ��Ҫ�Ӻ��滭��ǰ��
static int32	CalcNeedCalcNodesCoverVol(IN CNodeDrawerData *pNodeDrawerDatas, IN int32 iNodeCount, IN CChartCurve &ChartCurve, 
								 OUT CArray<CRect, CRect &>	&aNeedDrawRects, OUT CArray<CNodeDrawerData *, CNodeDrawerData *>& aNeedDrawNodeDrawerDatas)
{
	int32	iNeedDrawNodeCount	=	0;
	if ( NULL==pNodeDrawerDatas || iNodeCount < 1 )
	{
		
		return	0;
	}


	CRect	rectDraw	=	ChartCurve.GetChartRegion().GetRectCurves();

	aNeedDrawRects.SetSize( iNodeCount );
	aNeedDrawNodeDrawerDatas.SetSize( iNodeCount );

	ASSERT( ChartCurve.GetChartRegion().m_aXAxisNodes.GetSize() > 0 );

	if ( ChartCurve.GetChartRegion().m_aXAxisNodes[0].m_fPixelWidthPerUnit < 1.0 )
	{
		//����1.0�ģ�û��Ҫ�����ص����㣬����С��1.0���ظ���
		for (int32 i = iNodeCount - 1; i >= 0 ; i--)
		{
			if (CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid))
			{
				continue;
			}
			
			//����ͼ�ͷ�ʱ��vol��ɫ��ͬ����ͬ�ģ���������Ҳò�Ʋ��ܸı䣬���Բ��ÿ��Ǻ�����ص�ǰ��ģ�ֱ��ֻ����ߵ��Ǹ��Ϳ�����
			//��Ϊm_iVolumn
			
			CNodeDrawerData	&node	=	pNodeDrawerDatas[i];
			//������
			//�ص��ļ��㷽��
			// 			if ( 0 == iNeedDrawNodeCount || aNeedDrawNodeDrawerDatas[iNeedDrawNodeCount-1]->m_iX != node.m_iX
			// 				|| aNeedDrawNodeDrawerDatas[iNeedDrawNodeCount-1]->m_iVolumn > node.m_iVolumn )
			// 			{
			// 				//û����ӹ� || ����ͬX��� || ��ǰ���¼���Ǹ�Ҫ�󣨲����ȣ�(m_iVolumnԽ�󣬾���Խbottom��������value��Խ��)
			// 				aNeedDrawNodeDrawerDatas[ iNeedDrawNodeCount ]	=	pNodeDrawerDatas + i;
			// 				
			// 				//��������
			// 				int32 x1 = 0, x2 = 0, x3 = 0;
			// 				pNodeDrawerDatas[i].CalcXPos(x1, x2, x3);		//������������ͼ��������������Ժ���- -��ֻ��Ҫһ��top��bottom�Ϳ�����
			// 				
			// 				int32 y1 = 0, y2 = 0;
			// 				
			// 				y1 = rectDraw.bottom;
			// 				y2 = pNodeDrawerDatas[i].m_iVolumn;
			// 				
			// 				aNeedDrawRects[ iNeedDrawNodeCount ].left	=	x1;
			// 				aNeedDrawRects[ iNeedDrawNodeCount ].top		=	y2;
			// 				aNeedDrawRects[ iNeedDrawNodeCount ].right	=	x3;
			// 				aNeedDrawRects[ iNeedDrawNodeCount ].bottom	=	y1;
			// 				
			// 				iNeedDrawNodeCount++;
			// 			}

			//ֱ��ѡȡTop
			if ( 0 == iNeedDrawNodeCount || aNeedDrawNodeDrawerDatas[iNeedDrawNodeCount-1]->m_iX != node.m_iX )
			{
				//û����ӹ� || ����ͬX��� 
				//���
				aNeedDrawNodeDrawerDatas[ iNeedDrawNodeCount ]	=	pNodeDrawerDatas + i;
				
				//��������
				int32 x1 = 0, x2 = 0, x3 = 0;
				pNodeDrawerDatas[i].CalcXPos(x1, x2, x3);		//������������ͼ��������������Ժ���- -��ֻ��Ҫһ��top��bottom�Ϳ�����
				
			
				
				aNeedDrawRects[ iNeedDrawNodeCount ].left	=	x1;
				aNeedDrawRects[ iNeedDrawNodeCount ].top		=	pNodeDrawerDatas[i].m_iVolumn;
				aNeedDrawRects[ iNeedDrawNodeCount ].right	=	x3;
				aNeedDrawRects[ iNeedDrawNodeCount ].bottom	=	rectDraw.bottom;
				
				iNeedDrawNodeCount++;
			}
			else if  ( aNeedDrawNodeDrawerDatas[iNeedDrawNodeCount-1]->m_iVolumn > node.m_iVolumn )
			{
				//��ǰ���¼���Ǹ�ͬXҪ�󣨲����ȣ�(m_iVolumnԽ�󣬾���Խbottom��������value��Խ��)
				//�޸�ǰһ����¼Ϊtopһ����¼�¼
				aNeedDrawNodeDrawerDatas[ iNeedDrawNodeCount - 1 ]	=	pNodeDrawerDatas + i;
				
				//��������
				int32 x1 = 0, x2 = 0, x3 = 0;
				pNodeDrawerDatas[i].CalcXPos(x1, x2, x3);		//������������ͼ��������������Ժ���- -��ֻ��Ҫһ��top��bottom�Ϳ�����
				
				aNeedDrawRects[ iNeedDrawNodeCount - 1 ].left	=	x1;
				aNeedDrawRects[ iNeedDrawNodeCount - 1 ].top		=	pNodeDrawerDatas[i].m_iVolumn;
				aNeedDrawRects[ iNeedDrawNodeCount - 1 ].right	=	x3;
				aNeedDrawRects[ iNeedDrawNodeCount - 1 ].bottom	=	rectDraw.bottom;
				
			}
		}
	}
	else
	{
		//ֱ�Ӹ��ƣ��ų�Invalid
		for (int32 i = iNodeCount - 1; i >= 0 ; i--)
		{
			if (CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid))
			{
				continue;
			}
			
			
			//������ - ��ǰ�����
			
			
			aNeedDrawNodeDrawerDatas[ iNeedDrawNodeCount ]	=	pNodeDrawerDatas + i;
			//��������
			int32 x1 = 0, x2 = 0, x3 = 0;
			pNodeDrawerDatas[i].CalcXPos(x1, x2, x3);
			
			aNeedDrawRects[ iNeedDrawNodeCount ].left	=	x1;
			aNeedDrawRects[ iNeedDrawNodeCount ].top		=	pNodeDrawerDatas[i].m_iVolumn;
			aNeedDrawRects[ iNeedDrawNodeCount ].right	=	x3;
			aNeedDrawRects[ iNeedDrawNodeCount ].bottom	=	rectDraw.bottom;


			iNeedDrawNodeCount++;
		}
	}
	
	aNeedDrawRects.SetSize(iNeedDrawNodeCount);
	aNeedDrawNodeDrawerDatas.SetSize(iNeedDrawNodeCount);
	
	return	iNeedDrawNodeCount;
}

void CIndexDrawer::DrawNodesAsAlone(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, COLORREF clrIndex, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount)
{
	
	ASSERT(NULL != pNodeDrawerDatas && iNodeCount > 0);
	if (NULL == pNodeDrawerDatas || iNodeCount <= 0)
	{
		return;
	}
	CPen PenRise, PenFall, PenLine, PenTrend, *pOldPen = NULL;
	COLORREF clrRise = m_IoView.GetIoViewColor(ESCKLineRise);
	COLORREF clrFall = m_IoView.GetIoViewColor(ESCKLineFall);
	COLORREF clrTrend= m_IoView.GetIoViewColor(ESCVolume);
	//IChartRegionData *pParent = (IChartRegionData *)(CIoViewChart *)(&m_IoView);
	bool bChart = false;
	int iChartType = m_IoView.GetChartType();
	if(m_IoView.IsKindOfIoViewChart())
		bChart = true;

	if ( bChart != NULL
		&& iChartType == 0
		&& CheckFlag(ChartCurve.m_iFlag, CChartCurve::KTypeTrend) )
	{
		clrRise = m_IoView.GetIoViewColor(ESCRise);	// ��ʱ����ʹ�����ʱ��ͬ���ǵ�ɫ
		clrFall = m_IoView.GetIoViewColor(ESCFall);
		clrTrend = m_IoView.GetIoViewColor(ESCVolume2);
	}
	else if ( bChart != NULL
		&& (iChartType == 0
			|| 2==iChartType) )
	{
		clrTrend = m_IoView.GetIoViewColor(ESCVolume2);
	}
	
	bool32 bOccupy = ChartCurve.GetOccupyIndexLineClr();
	ChartCurve.SetOccupyIndexLineClr(false);	// ֻ�в�������Ҫ�������

	PenRise.CreatePen(PS_SOLID, 1, clrRise);
	PenFall.CreatePen(PS_SOLID, 1, clrFall);
	PenTrend.CreatePen(PS_SOLID,1, clrTrend);
	
	pOldPen = (CPen*)pDC->SelectObject(&PenRise);
	
	// 
	// pPickDC->BeginDraw(&ChartCurve, 10);

	// 
	//xiali XL0005 �����ص���, �����һ��
	//int32	iLastX	=	-1;
	//bool32 bHasMoveTo = false;

	CRect	rectDraw	=	ChartCurve.GetChartRegion().GetRectCurves();

	//ʵ���ϼ�ʹ�ǻ����Σ�Ҳ������X���̫С����ɻ��ߣ��ڴ�������ʱ��������Ҫ��ͬһX��ĺ���ĸ��ǣ����Կ��Ǻ���

	//ÿ������һ��������ͼ�Σ���ͼ�α����ڵ�xLeft, xRight, yBottom, iValue�����ƣ������ڴ�����ʱ�򣬻��ص���ͬһ������X���ص���
	//����ÿ��ͼ�Σ���ȡ��iValue���ܻ�������ͬ����������ͼ����ȡ�Ķ���m_iVolumn�������ص������ѡ��������
	//�ȼ��� ��Ҫ���ľ�������Ӧ��pNodeDrawerDatas �� ���������������и��ǿ��ǣ��洢���ڸ���ɸѡ�����Ľ����������ǰ�Ļ���
	CArray<CRect, CRect &>	aDrawRect;
	CArray<CNodeDrawerData *, CNodeDrawerData *>	aDrawDataPtrs;	//������������--��Ӧ��
	aDrawRect.SetSize(iNodeCount);
	aDrawDataPtrs.SetSize(iNodeCount);
	int32	iNeedDrawNodeCount	=	0;
	
	//�ж����ת�Ƶ�ѭ��������
	if (EIDSVolStick == m_eIndexDrawType)	// �ɽ�����
	{
		//�Ժ������Yֵ��ͬ�ļ��㣬����Ҫ���ص������Ƶ�������
		
		// �ɽ���ͼ��һ����״�Ĳ�ռ��ָ������ɫ
		ChartCurve.SetOccupyIndexLineClr(false);

		//OK, ��������
		
		if ( 1 != ChartCurve.GetChartRegion().GetParentIoViewData().pChartRegionData->m_iChartType )
		{
			// ��ʱ & ���磬��Ҫ�����������ţ��ܷ�ʱ����ʱ������ֻ��һ����ɫ�����Կ��Բ��ÿ��Ǻ��渲������
			//�����ǵ��򣬴Ӻ�����㵽ǰ���
			//�㲻�������ģ�����Ҫһ��һ����
			iNeedDrawNodeCount	=	CalcNeedCalcNodesCoverVol(pNodeDrawerDatas, iNodeCount, ChartCurve, aDrawRect, aDrawDataPtrs);
			ASSERT( iNeedDrawNodeCount >= 0 );

			bool32 bShowVolColor =  CTabSplitWnd::m_pMainFram->SendMessage(UM_IsShowVolBuySellColor);//CIoViewChart::IsShowVolBuySellColor();
			
			// ��ʱ���ơ�����ĸ�ͼbottom row������ʾ����clipRgn��clip��
			//     ԭ�����ڸ�ͼ��Yֵ����vol��MinMaxY��ȷ�������Ե���bottom rowǡ����rectView��bottomһ��
			//     �������ʱ������������ֵ�ļ������Ƶ�rect ���1
			//     TODO�����rectView��CurveView���߿���ƵĹ�ϵ
			CPen penTrendRise, penTrendFall, penTrendKeep;
			penTrendRise.CreatePen(PS_SOLID, 0, m_IoView.GetIoViewColor(ESCRise));
			penTrendFall.CreatePen(PS_SOLID, 0, m_IoView.GetIoViewColor(ESCFall));
			penTrendKeep.CreatePen(PS_SOLID, 0, m_IoView.GetIoViewColor(ESCKeep));
			CArray<CPoint, CPoint> aPtsRise, aPtsFall, aPtsKeep, aPtsDefault;
			aPtsRise.SetSize(0, iNeedDrawNodeCount);
			aPtsFall.SetSize(0, iNeedDrawNodeCount);
			aPtsKeep.SetSize(0, iNeedDrawNodeCount);
			aPtsDefault.SetSize(0, iNeedDrawNodeCount);
			CPoint pt1, pt2;
			if ( bShowVolColor )
			{
				for ( int32 i=iNeedDrawNodeCount-1; i >= 0; i-- )
				{
					if ( (int)aDrawDataPtrs[i]->m_fVolume > 0 )
					{
						aDrawRect[i].OffsetRect(0, -1);
					}
					pt1.x = aDrawDataPtrs[i]->m_iX;
					pt2.x = aDrawDataPtrs[i]->m_iX;
					pt1.y = aDrawRect[i].bottom;
					pt2.y = aDrawRect[i].top;

					//////////////////////////////////////////////////////////////////////////	
					//lint --e{650}
					////lint ENCFColorFall,ENCFColorRiseֵΪ�з��Ÿ�����aDrawDataPtrs[i]->m_bClrValidΪ�޷��ţ���Զ�޷�����case�������޸İ취 modifyer weng.cx
					if ( aDrawDataPtrs[i]->m_bClrValid )
					{
						switch ( aDrawDataPtrs[i]->m_clrNode )
						{
						case CNodeData::ENCFColorRise:
							aPtsRise.Add(pt1);
							aPtsRise.Add(pt2);
							break;
						case CNodeData::ENCFColorFall:
							aPtsFall.Add(pt1);
							aPtsFall.Add(pt2);
							break;
						default:
							aPtsKeep.Add(pt1);
							aPtsKeep.Add(pt2);
							break;
						}
					}
					else
					{
						aPtsDefault.Add(pt1);
						aPtsDefault.Add(pt2);
					}
				}
			}
			else
			{
				for ( int32 i=iNeedDrawNodeCount-1; i >= 0; i-- )
				{
					if ( (int)aDrawDataPtrs[i]->m_fVolume > 0 )
					{
						aDrawRect[i].OffsetRect(0, -1);
					}
					pt1.x = aDrawDataPtrs[i]->m_iX;
					pt2.x = aDrawDataPtrs[i]->m_iX;
					pt1.y = aDrawRect[i].bottom;
					pt2.y = aDrawRect[i].top;
					
					aPtsDefault.Add(pt1);
					aPtsDefault.Add(pt2);
				}
			}
			
			if ( aPtsDefault.GetSize() > 0 )
			{
				pDC->SelectObject(&PenTrend);
				CDWordArray aNodeCounts;
				int32 iSize = aPtsDefault.GetSize();
				ASSERT( iSize%2==0 );
				iSize /= 2;
				aNodeCounts.SetSize(iSize);
				for ( int32 m=0; m < iSize ; ++m )
				{
					aNodeCounts[m] = 2;
				}
				pDC->PolyPolyline(aPtsDefault.GetData(), aNodeCounts.GetData(), iSize);
				// pPickDC->PolyPolyline(aPtsDefault.GetData(), aNodeCounts.GetData(), iSize);
			}

			if ( aPtsRise.GetSize() > 0 )
			{
				pDC->SelectObject(&penTrendRise);
				CDWordArray aNodeCounts;
				aNodeCounts.SetSize(aPtsRise.GetSize());
				int32 iSize = aPtsRise.GetSize();
				ASSERT( iSize%2==0 );
				iSize /= 2;
				aNodeCounts.SetSize(iSize);
				for ( int32 m=0; m < iSize ; ++m )
				{
					aNodeCounts[m] = 2;
				}
				pDC->PolyPolyline(aPtsRise.GetData(), aNodeCounts.GetData(), iSize);
				// pPickDC->PolyPolyline(aPtsRise.GetData(), aNodeCounts.GetData(), iSize);
			}

			if ( aPtsFall.GetSize() > 0 )
			{
				pDC->SelectObject(&penTrendFall);
				CDWordArray aNodeCounts;
				aNodeCounts.SetSize(aPtsFall.GetSize());
				int32 iSize = aPtsFall.GetSize();
				ASSERT( iSize%2==0 );
				iSize /= 2;
				aNodeCounts.SetSize(iSize);
				for ( int32 m=0; m < iSize ; ++m )
				{
					aNodeCounts[m] = 2;
				}
				pDC->PolyPolyline(aPtsFall.GetData(), aNodeCounts.GetData(), iSize);
				// pPickDC->PolyPolyline(aPtsFall.GetData(), aNodeCounts.GetData(), iSize);
			}

			if ( aPtsKeep.GetSize() > 0 )
			{
				pDC->SelectObject(&penTrendKeep);
				CDWordArray aNodeCounts;
				aNodeCounts.SetSize(aPtsKeep.GetSize());
				int32 iSize = aPtsKeep.GetSize();
				ASSERT( iSize%2==0 );
				iSize /= 2;
				aNodeCounts.SetSize(iSize);
				for ( int32 m=0; m < iSize ; ++m )
				{
					aNodeCounts[m] = 2;
				}
				pDC->PolyPolyline(aPtsKeep.GetData(), aNodeCounts.GetData(), iSize);
				// pPickDC->PolyPolyline(aPtsKeep.GetData(), aNodeCounts.GetData(), iSize);
			}
		}
		else
		{
			iNeedDrawNodeCount	=	CalcNeedCalcNodesOverlapVol(pNodeDrawerDatas, iNodeCount, ChartCurve,
																aDrawRect, aDrawDataPtrs);
			ASSERT( iNeedDrawNodeCount > 0 );
			//K�߻���
			for ( int32 i=iNeedDrawNodeCount-1; i >= 0; i-- )
			{
				int32 iRiseFallFlag = 1;
				COLORREF clrCur = clrRise;
				
				// �ǵ�ɫ
				if ( aDrawDataPtrs[i]->m_bRise )
				{
					iRiseFallFlag = 1;
					clrCur = clrRise;
					pDC->SelectObject(&PenRise);
				}
				else 
				{
					iRiseFallFlag = -1;
					clrCur = clrFall;
					pDC->SelectObject(&PenFall);
				}
				// �ײ������϶,����һЩ
				// Ӧ���ڼ��������ʱ���Ҫ�ճ�3�����أ���Ȼ�����������ʾ�����ݵĲ�ƥ�� - �Ѿ���ƥ����- -������ֻ�ǵ���û��ô�ѿ�
				aDrawRect[i].bottom -= 3;
				if ( aDrawRect[i].Height() < 0 )
				{
					aDrawRect[i].top = aDrawRect[i].bottom;
				}
				
				
				
				pDC->_DrawRect( aDrawRect[i] );
				
				if (iRiseFallFlag < 0)	// ��
					pDC->_FillSolidRect(aDrawRect[i], clrCur);				
				//
				CRect RectPick(aDrawDataPtrs[i]->m_iXLeft, rectDraw.bottom, aDrawDataPtrs[i]->m_iXRight, aDrawRect[i].top);
				// pPickDC->_FillSolidRect(RectPick, clrCur);
			}
			
		}
	}//end if EIDSVolStick �ɽ�����
	else if ( EIDSStick == m_eIndexDrawType )
	{
		int32 y2 = 0;
		
		int32	y0	=	0;	//��׼ֵY0
		//ChartCurve.PriceYToRegionY(0.0,y0);
		ChartCurve.AxisYToRegionY(0.0f, y0);
		ChartCurve.GetChartRegion().RegionYToClient(y0);				
		ChartCurve.GetChartRegion().ClipY(y0);
		
		iNeedDrawNodeCount	=	CalcNeedCalcNodesOverlapVol(pNodeDrawerDatas, iNodeCount, ChartCurve,
			aDrawRect, aDrawDataPtrs);
		ASSERT( iNeedDrawNodeCount > 0 );
		
		COLORREF clrCur = clrIndex;
		ChartCurve.SetOccupyIndexLineClr(bOccupy);	// ��Ҫ��ɫ������
				
		CPen penStick(PS_SOLID, m_iThick, clrCur);
		pOldPen = pDC->SelectObject(&penStick);

		for ( int32 i = iNeedDrawNodeCount-1; i >= 0; i-- )
		{
			y2 = aDrawDataPtrs[i]->m_iVolumn;
		
	
			pDC->_DrawLine(aDrawDataPtrs[i]->m_iX, y0, aDrawDataPtrs[i]->m_iX, y2);
			// pPickDC->_DrawLine(aDrawDataPtrs[i]->m_iX, y0, aDrawDataPtrs[i]->m_iX, y2);
		}

		pDC->SelectObject(pOldPen);
		penStick.DeleteObject();
	}
	else if ( EIDSColorStick == m_eIndexDrawType )
	{
		// COLORSTICK �� 0 Ϊ�ֽ���.���»��� ��+ֵ����-ֵ
		// ���ڴ���-ֵ��ʾ���ص��Ż���Ҫ���㣬��ΪͬX����ܴ�������+ֵ��-ֵ����Ҫ��ʾ������ֵһ��Ҫ��ʾ
		// �����ܼ���ʱ��������û�ע����Ч��Ϣ���ȹ���
		
		ChartCurve.SetOccupyIndexLineClr(false);

		int32 y2 = 0;
		
		int32	y0	=	0;	//��׼ֵY0
		//ChartCurve.PriceYToRegionY(0.0,y0);
		float fBase = 0.0f;
		if ( CPriceToAxisYObject::EAYT_Normal != ChartCurve.GetAxisYType() )
		{
			ChartCurve.GetAxisBaseY(fBase);
			ChartCurve.AxisYToPriceY(fBase, fBase);
		}
		else if( CheckFlag(ChartCurve.m_iFlag, CChartCurve::KTypeTrend) )
		{
			// ��������, ��ʱͼ(����ֻ�з�ʱ��ͼ���߻��������)
			ChartCurve.GetAxisBaseY(fBase);
			ChartCurve.AxisYToPriceY(fBase, fBase);
		}
		ChartCurve.PriceYToRegionY(fBase, y0);
		ChartCurve.GetChartRegion().RegionYToClient(y0);				
		ChartCurve.GetChartRegion().ClipY(y0);

		iNeedDrawNodeCount	=	CalcNeedCalcNodesOverlapVol(pNodeDrawerDatas, iNodeCount, ChartCurve,
															aDrawRect, aDrawDataPtrs);
		//ASSERT( iNeedDrawNodeCount > 0 );	// ��Щ����û������

		int32 y1 = y0;
		if ( CheckFlag(ChartCurve.m_iFlag, CChartCurve::KTypeTrend) )
		{
			y1 += 1;
		}
		
		for ( int32 i=iNeedDrawNodeCount-1; i >= 0; i-- )
		{
			y2 = aDrawDataPtrs[i]->m_iVolumn;		
			
			
			if ( y0 < y2 )
			{
				pDC->SelectObject(&PenFall);
				pDC->_DrawLine(aDrawDataPtrs[i]->m_iX, y1, aDrawDataPtrs[i]->m_iX, y2);
				// pPickDC->_DrawLine(aDrawDataPtrs[i]->m_iX, y1, aDrawDataPtrs[i]->m_iX, y2);
			}
			else
			{
				pDC->SelectObject(&PenRise);
				pDC->_DrawLine(aDrawDataPtrs[i]->m_iX, y0, aDrawDataPtrs[i]->m_iX, y2);
				// pPickDC->_DrawLine(aDrawDataPtrs[i]->m_iX, y0, aDrawDataPtrs[i]->m_iX, y2);
			}
		}		
	}
	else if (EIDSCircle	== m_eIndexDrawType)
	{
		// ����	
		ChartCurve.SetOccupyIndexLineClr(false);
	
		iNeedDrawNodeCount	=	CalcNeedCalcNodesOverlapVol(pNodeDrawerDatas, iNodeCount, ChartCurve, aDrawRect, aDrawDataPtrs);

		int32 KIDiameter	= aDrawRect.GetSize()>0? aDrawRect[0].Width() : 0;
		int32 KIRadius		= KIDiameter/2;
		KIRadius	= max(2, KIRadius);
		KIRadius	= min(3, KIRadius);
		KIDiameter	= max(3, KIDiameter);
		KIDiameter	= min(6, KIDiameter);

		CBrush br(clrIndex);
		CPen pen(PS_SOLID, 1, clrIndex);
		CBrush* pOldBrush = pDC->SelectObject(&br);
		pOldPen = pDC->SelectObject(&pen);

		
		for (int32 i = iNeedDrawNodeCount - 1; i >= 0; i--)
		{
			if (CheckFlag(aDrawDataPtrs[i]->m_iFlag, CNodeData::KDrawNULL))
			{
				continue;
			}

			int32 y = aDrawDataPtrs[i]->m_iClose;
	
			//
			CRect rect(CPoint(aDrawDataPtrs[i]->m_iX-KIRadius, y-KIRadius), CSize(KIDiameter, KIDiameter));
			
			pDC->Ellipse(&rect);
			// pPickDC->Ellipse(&rect);
		}
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
		
		br.DeleteObject();
		pen.DeleteObject();
	}
	else if (EIDSCrossDot == m_eIndexDrawType)
	{
		// zhangbo 20090717 #������
		//...
	}
	else 
	{
		////ASSERT(0);
	}
	
	// pPickDC->EndDraw();
	
	// ��ѡ�б�־
	if (ChartCurve.IsSelect())
	{
		int32 iROPBack = pDC->SetROP2(R2_XORPEN);
		for (int32 i = 0; i < iNodeCount; i += iShowSelectFlagPeriod)
		{
			if (CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid))
			{
				continue;
			}
			
			int32 x1 = pNodeDrawerDatas[i].m_iX		- 4;	
			int32 x2 = pNodeDrawerDatas[i].m_iX		+ 4;
			int32 y1 = pNodeDrawerDatas[i].m_iVolumn- 4;	
			int32 y2 = pNodeDrawerDatas[i].m_iVolumn+ 4;
			
			pDC->_FillSolidRect(CRect(x1, y1, x2, y2), 0xffffff);
		}
		pDC->SetROP2(iROPBack);
	}
		
	//
	if (NULL != pOldPen)
		pDC->SelectObject(pOldPen);
	
	PenRise.DeleteObject();	
	PenFall.DeleteObject();	
	PenTrend.DeleteObject();
}

void CIndexDrawer::DrawNodesAsText(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, COLORREF clrIndex, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount)
{
	if ( NULL == pDC || NULL == pPickDC || NULL == pNodeDrawerDatas || 0 >= iNodeCount )
	{
	
		return;
	}

	COLORREF clrOld = pDC->GetTextColor();
	COLORREF clrNow = m_lColor;
	ChartCurve.SetOccupyIndexLineClr(false);
	
	CFont* pFont = CFaceScheme::Instance()->GetSysFontObject(ESFSmall);			
	pFont = m_IoView.GetIoViewFontObject(ESFSmall);
	
	if ( -1 == m_lColor )
	{
		clrNow = m_IoView.GetIoViewColor(ESCText);
	}					
	
	CFont* pOldFont = NULL;
	if ( NULL != pFont )
	{
		pOldFont = pDC->SelectObject(pFont);
	}
	
	pDC->SetTextColor(clrNow);

	T_MerchNodeUserData* pUserData = (T_MerchNodeUserData*)ChartCurve.GetNodes()->GetUserData();

	//
	CString StrText = ChartCurve.GetNodes()->GetName();

	//
	if ( NULL != pUserData )
	{
		for ( int32 i = 0; i < pUserData->aIndexs.GetSize(); i++ )
		{
			// �ҵ���� region
			if ( pUserData->aIndexs.GetAt(i)->pRegion == &ChartCurve.GetChartRegion() )
			{
				for ( int32 j = 0; j < pUserData->aIndexs.GetAt(i)->m_aIndexLines.GetSize(); j++ )
				{
					// �ҵ���� churves
					if ( pUserData->aIndexs.GetAt(i)->m_aIndexLines.GetAt(j).m_pCurve == &ChartCurve )
					{
						StrText = pUserData->aIndexs.GetAt(i)->m_aIndexLines.GetAt(j).m_StrExtraData;
						break;
					}
				}
			}
		}		
	}

	for ( int32 i = 0; i < iNodeCount; i++ )
	{
		if ( !CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid) )
		{
			//						
			CSize SizeText = pDC->GetTextExtent(StrText);
			CRect rectText(pNodeDrawerDatas[i].m_iX, pNodeDrawerDatas[i].m_iClose, pNodeDrawerDatas[i].m_iX + SizeText.cx + 5, pNodeDrawerDatas[i].m_iClose + SizeText.cy + 5);
			pDC->DrawText(StrText, &rectText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
	}

	if ( NULL != pOldFont )
	{
		pDC->SelectObject(pOldFont);
	}

	pDC->SetTextColor(clrOld);
}

void CIndexDrawer::DrawNodesAsNumText(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, COLORREF clrIndex, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount)
{
	if ( NULL == pDC || NULL == pPickDC || NULL == pNodeDrawerDatas || 0 >= iNodeCount )
	{
	
		return;
	}
	
	COLORREF clrOld = pDC->GetTextColor();
	COLORREF clrNow = m_lColor;
	ChartCurve.SetOccupyIndexLineClr(false);
	
	CFont* pFont = CFaceScheme::Instance()->GetSysFontObject(ESFSmall);			
	pFont = m_IoView.GetIoViewFontObject(ESFSmall);
	
	if ( -1 == m_lColor )
	{
		clrNow = m_IoView.GetIoViewColor(ESCText);
	}					
	
	CFont* pOldFont = NULL;
	if ( NULL != pFont )
	{
		pOldFont = pDC->SelectObject(pFont);
	}
	
	pDC->SetTextColor(clrNow);
	
	T_MerchNodeUserData* pUserData = (T_MerchNodeUserData*)ChartCurve.GetNodes()->GetUserData();
	
	//
	CIndexNodeList* p = NULL;
	
	//
	if ( NULL != pUserData )
	{
		for ( int32 i = 0; i < pUserData->aIndexs.GetSize(); i++ )
		{
			// �ҵ���� region
			if ( pUserData->aIndexs.GetAt(i)->pRegion == &ChartCurve.GetChartRegion() )
			{
				for ( int32 j = 0; j < pUserData->aIndexs.GetAt(i)->m_aIndexLines.GetSize(); j++ )
				{
					// �ҵ���� churves
					if ( pUserData->aIndexs.GetAt(i)->m_aIndexLines.GetAt(j).m_pCurve == &ChartCurve )
					{
						p = &(pUserData->aIndexs.GetAt(i)->m_aIndexLines[j]);
						break;
					}
				}
			}
		}		
	}

	if (NULL != pUserData && NULL != p)
	{
		for ( int32 i = 0; i < iNodeCount; i++ )
		{
			CString StrText = p->m_aStrExtraData[i + pUserData->m_iShowPosInFullList];

			//
			if ( !CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid) && StrText.GetLength() > 0)
			{
				//										
				CSize SizeText = pDC->GetTextExtent(StrText);
				CRect rectText(pNodeDrawerDatas[i].m_iX, pNodeDrawerDatas[i].m_iClose, pNodeDrawerDatas[i].m_iX + SizeText.cx + 5, pNodeDrawerDatas[i].m_iClose + SizeText.cy + 5);
				pDC->DrawText(StrText, &rectText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
		}	
	}
	
	if ( NULL != pOldFont )
	{
		pDC->SelectObject(pOldFont);
	}
	
	pDC->SetTextColor(clrOld);
}

void CIndexDrawer::DrawNodesAsTextEx(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, COLORREF clrIndex, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount)
{
	if ( NULL == pDC || NULL == pPickDC || NULL == pNodeDrawerDatas || 0 >= iNodeCount )
	{
	
		return;
	}
	
	COLORREF clrOld = pDC->GetTextColor();
	COLORREF clrNow = m_lColor;
	ChartCurve.SetOccupyIndexLineClr(false);
	
	CFont* pFont = CFaceScheme::Instance()->GetSysFontObject(ESFSmall);			
	pFont = m_IoView.GetIoViewFontObject(ESFSmall);
	
	if ( -1 == m_lColor )
	{
		clrNow = m_IoView.GetIoViewColor(ESCText);
	}					
	
	CFont* pOldFont = NULL;
	if ( NULL != pFont )
	{
		pOldFont = pDC->SelectObject(pFont);
	}
	
	pDC->SetTextColor(clrNow);
	
	T_MerchNodeUserData* pUserData = (T_MerchNodeUserData*)ChartCurve.GetNodes()->GetUserData();
	if (NULL == pUserData)
	{
		return;
	}
	//
	CStringArray aTexts;

	//
	if ( NULL != pUserData )
	{
		for ( int32 i = 0; i < pUserData->aIndexs.GetSize(); i++ )
		{
			
			if (!(pUserData->aIndexs.GetAt(i) && pUserData->aIndexs.GetAt(i)->pRegion))
			{
				continue;
			}
			if ( pUserData->aIndexs.GetAt(i)->pRegion == &ChartCurve.GetChartRegion() )
			{
				for ( int32 j = 0; j < pUserData->aIndexs.GetAt(i)->m_aIndexLines.GetSize(); j++ )
				{
					// �ҵ���� churves
					if ( pUserData->aIndexs.GetAt(i)->m_aIndexLines.GetAt(j).m_pCurve == &ChartCurve )
					{
						aTexts.Copy(pUserData->aIndexs.GetAt(i)->m_aIndexLines.GetAt(j).m_aStrExtraData);
						break;
					}
				}
			}
		}		
	}
	
	int32 iSize = aTexts.GetSize();

	//
	float fMaxY = FLT_MAX, fMinY = FLT_MIN;

	CChartCurve *pDependentCurve = ChartCurve.GetChartRegion().GetDependentCurve();
	if (NULL != pDependentCurve)
	{
		if ( !pDependentCurve->GetYMinMax(fMinY, fMaxY) )
		{
			////ASSERT(0);
			// ������ֻ�����óɲ����ܵ�ֵ
			fMinY = FLT_MIN;
			fMaxY = FLT_MAX;
		}
	}

	CString StrMin,StrMax;
	StrMin.Format(L"%g",fMinY);
	StrMax.Format(L"%g",fMaxY);

	for ( int32 i = 0; i < iNodeCount; i++ )
	{
		int32 iIndexText = i + pUserData->m_iShowPosInFullList;

		//
		if ( !CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid) && iIndexText < iSize && iIndexText >= 0)
		{
			//						
			CString StrText = aTexts[iIndexText];

			if (StrText == StrMin || StrText == StrMax)
			{
				;
			}
			else
			{

				bool32 bHigh = false;

				CNodeData NodeTmp;
				pUserData->m_pKLinesShow->GetAt(i,NodeTmp);
				
				CString StrHigh, StrLow;
				StrHigh.Format(L"%g",NodeTmp.m_fHigh);
				StrLow.Format(L"%g",NodeTmp.m_fLow);
				if (0 == StrText.CompareNoCase(StrHigh))
				{
					bHigh = true;
				}

				CString StrDrawText = StrText;
				int iPos = StrText.Find('.');
				if(-1 != iPos)
				{
					CString StrInteger = StrText.Left(iPos);
					CString StrDot = StrText.Right(StrText.GetLength() - iPos - 1);
					if(StrDot.GetLength() == 0)
					{
						StrDrawText.Format(L"%s.00",StrInteger);
					}
					else if(StrDot.GetLength() == 1)
					{
						StrDrawText.Format(L"%s.%s0",StrInteger, StrDot);
					}
					else if(StrDot.GetLength() >= 2)
					{
						StrDrawText.Format(L"%s.%s", StrInteger, StrDot.Left(2));
					}
				}
				else
				{
					StrDrawText.Format(L"%s.00",StrText);
				}
                
				CSize SizeText = pDC->GetTextExtent(StrDrawText);
    //            float fValue = _wtof(StrDrawText);
				//StrText = Float2SymbolString(fValue, fValue, 2);

				if (!bHigh)
				{
					CRect rectText(pNodeDrawerDatas[i].m_iX, pNodeDrawerDatas[i].m_iClose, pNodeDrawerDatas[i].m_iX + SizeText.cx + 5, pNodeDrawerDatas[i].m_iClose + SizeText.cy + 5);
					pDC->DrawText(StrDrawText, &rectText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
				else
				{
					CRect rectText(pNodeDrawerDatas[i].m_iX, pNodeDrawerDatas[i].m_iClose - SizeText.cy - 5, pNodeDrawerDatas[i].m_iX + SizeText.cx + 5, pNodeDrawerDatas[i].m_iClose);
					pDC->DrawText(StrDrawText, &rectText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}				
			}			
		}
	}
	
	if ( NULL != pOldFont )
	{
		pDC->SelectObject(pOldFont);
	}
	
	pDC->SetTextColor(clrOld);
}

void CIndexDrawer::DrawNodesAsIcon(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, COLORREF clrIndex, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount)
{
	if ( NULL == pDC || NULL == pPickDC || NULL == pNodeDrawerDatas || 0 >= iNodeCount )
	{
		////ASSERT(0);
		return;
	}
	
	T_MerchNodeUserData* pUserData = (T_MerchNodeUserData*)ChartCurve.GetNodes()->GetUserData();

	int32 iIndexIcon = 0;

	//
	if ( NULL != pUserData )
	{
		for ( int32 i = 0; i < pUserData->aIndexs.GetSize(); i++ )
		{
			// �ҵ���� region
			if ( pUserData->aIndexs.GetAt(i)->pRegion == &ChartCurve.GetChartRegion() )
			{
				for ( int32 j = 0; j < pUserData->aIndexs.GetAt(i)->m_aIndexLines.GetSize(); j++ )
				{
					// �ҵ���� churves
					if ( pUserData->aIndexs.GetAt(i)->m_aIndexLines.GetAt(j).m_pCurve == &ChartCurve )
					{
						CString StrIndex = pUserData->aIndexs.GetAt(i)->m_aIndexLines.GetAt(j).m_StrExtraData;

						string strIndex;
						Unicode2MultiChar(EMCCUtf8, StrIndex, strIndex);
						iIndexIcon = atoi(strIndex.c_str());

						// 
						if (iIndexIcon < 1 || iIndexIcon > 4)
						{
							iIndexIcon = 1;
						}

						break;
					}
				}
			}
		}		
	}

	//
	for ( int32 i = 0; i < iNodeCount; i++ )
	{
		if ( !CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid) )
		{
			//
			Graphics GraphicImage(pDC->GetSafeHdc());

			Image* pImageToDraw = NULL;

			if ( 1 == iIndexIcon )
			{
				if (NULL == m_pImageUp) 
				{
					AFX_MANAGE_STATE(AfxGetStaticModuleState());
					ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_UP, L"PNG", m_pImageUp);
				}
				
				pImageToDraw = m_pImageUp;
			}
			else if ( 2 == iIndexIcon )
			{
				if ( NULL == m_pImageDown )
				{
					AFX_MANAGE_STATE(AfxGetStaticModuleState());
					ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_DOWN, L"PNG", m_pImageDown);
					
				}
				
				pImageToDraw = m_pImageDown;
			}
			else if ( 3 == iIndexIcon )
			{
				if ( NULL == m_pImageUp2 )
				{
					AFX_MANAGE_STATE(AfxGetStaticModuleState());
					ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TREND_INDEX_DTOC, L"PNG", m_pImageUp2);					
				}
				
				pImageToDraw = m_pImageUp2;
			}
			else if ( 4 == iIndexIcon )
			{
				if ( NULL == m_pImageDown2 )
				{
					AFX_MANAGE_STATE(AfxGetStaticModuleState());
					ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TREND_INDEX_KTOC, L"PNG", m_pImageDown2);					
				}
				
				pImageToDraw = m_pImageDown2;
			}

			//
			int32 iWidth =0, iHeight= 0;
			if (NULL != pImageToDraw)
			{
				iWidth = pImageToDraw->GetWidth() / 2;
				iHeight = pImageToDraw->GetHeight();
			}		

			//
			CRect rectDraw;
			rectDraw.left = pNodeDrawerDatas[i].m_iX - iWidth;
			rectDraw.right = pNodeDrawerDatas[i].m_iX + iWidth;

// 			if (3==iIndexIcon || 4==iIndexIcon) // ������ָͼ�겻�Գƣ�ƫ��3������
// 			{
// 				rectDraw.left += 3;
// 				rectDraw.right += 3;
// 			}

			if ( 1 == iIndexIcon || 3 == iIndexIcon )
			{
				rectDraw.top = pNodeDrawerDatas[i].m_iClose;
				rectDraw.bottom = pNodeDrawerDatas[i].m_iClose + iHeight;
			}
			else if ( 2 == iIndexIcon || 4 == iIndexIcon )
			{
				rectDraw.bottom = pNodeDrawerDatas[i].m_iClose;
				rectDraw.top = pNodeDrawerDatas[i].m_iClose - iHeight;
			}

			//
			DrawImage(GraphicImage, pImageToDraw, rectDraw, 1, 0, true);
		}
	}
}

void CIndexDrawer::DrawNodesAsLine(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, COLORREF clrIndex, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount)
{
	if ( NULL == pDC || NULL == pPickDC || NULL == pNodeDrawerDatas || 0 >= iNodeCount )
	{
		////ASSERT(0);
		return;
	}

	ChartCurve.SetOccupyIndexLineClr(false);

	// ����
	pPickDC->BeginDraw(&ChartCurve, 3);

	COLORREF clr = m_lColor;	
	if ( -1 == m_lColor )	//	modify by weng.cx, form -1 == clr to -1 == m_lColor
	{
		clr = m_IoView.GetIoViewColor(ESCGuideLine1);			
	}
	
	CPen pen(PS_SOLID, 1, clr);
	CPen* pOldPen = (CPen*)pDC->SelectObject(&pen);

	bool32 bLineTo = false;
	
	for ( int32 i = 0; i < iNodeCount; i++ )
	{
		if ( !CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid) )
		{							
			CPoint pt(pNodeDrawerDatas[i].m_iX, pNodeDrawerDatas[i].m_iClose);
				
			if (pNodeDrawerDatas[i].m_iClose == FLT_EPSILON )
			{
				;
			}

			if (bLineTo)
			{
				pDC->LineTo(pt);
				pPickDC->LineTo(pt);
			}
			else
			{
				pDC->MoveTo(pt);	
				pPickDC->LineTo(pt);
			}

			bLineTo = !bLineTo;

			//
			CRect rectPoint(pNodeDrawerDatas[i].m_iX - 2, pNodeDrawerDatas[i].m_iClose - 2, pNodeDrawerDatas[i].m_iX + 2, pNodeDrawerDatas[i].m_iClose + 2);
			pDC->FillSolidRect(rectPoint, clr);
			pPickDC->FillSolidRect(rectPoint, clr);
		}
	}

	//
	pPickDC->EndDraw();

	// ��ѡ�б�־
	if (ChartCurve.IsSelect())
	{
		int32 iROPBack = pDC->SetROP2(R2_XORPEN);
		for (int32 i = 0; i < iNodeCount; i += iShowSelectFlagPeriod)
		{
			if (CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid))
			{
				continue;
			}
			
			int32 x1 = pNodeDrawerDatas[i].m_iX		- 4;	
			int32 x2 = pNodeDrawerDatas[i].m_iX		+ 4;
			int32 y1 = pNodeDrawerDatas[i].m_iClose - 4;	
			int32 y2 = pNodeDrawerDatas[i].m_iClose + 4;
			
			pDC->_FillSolidRect(CRect(x1, y1, x2, y2), 0xffffff);
		}

		pDC->SetROP2(iROPBack);
	}

	//
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}

void FillCrossRgn(CMemDCEx* pDC, CPoint p1, CPoint p2, CPoint p3, CPoint p4, COLORREF clr1, COLORREF clr2)
{
	if (NULL == pDC)
	{
		return;
	}

	//
	ASSERT(p1.x == p2.x && p3.x == p4.x);

	//
	CPoint pt1,pt2,pt3,pt4;

	pt1 = p1.y <= pt2.y ? p1 : p2;
	pt2 = p1.y <= pt2.y ? p2 : p1;

	//
	pt3 = p3.y <= pt4.y ? p3 : p4;
	pt4 = p3.y <= pt4.y ? p4 : p3;

	//
	CPoint aPts[3];
	aPts[0] = pt1;
	aPts[1] = pt2;
	aPts[2] = pt3;
	
	//
	HRGN hRgn1 = CreatePolygonRgn(&aPts[0], 3, ALTERNATE);

	aPts[0] = pt1;
	aPts[1] = pt2;
	aPts[2] = pt4;
	
	HRGN hRgn2 = CreatePolygonRgn(&aPts[0], 3, ALTERNATE);

	CBrush BrushLeft;
	BrushLeft.CreateSolidBrush(clr1);

	// �������Ľ��������С�����ε�����
	HRGN hRgnLeft = CreateRectRgn(1,1,1,1);
	CombineRgn(hRgnLeft, hRgn1, hRgn2, RGN_AND);
	pDC->FillRgn(CRgn::FromHandle(hRgnLeft), &BrushLeft);

	// �ұߵ�
	CPoint aPtsAnother[3];
	aPtsAnother[0] = pt3;
	aPtsAnother[1] = pt4;
	aPtsAnother[2] = pt2;
	
	HRGN hRgn3 = CreatePolygonRgn(&aPtsAnother[0], 3, ALTERNATE);

	//
	aPtsAnother[0] = pt3;
	aPtsAnother[1] = pt4;
	aPtsAnother[2] = pt1;
	HRGN hRgn4 = CreatePolygonRgn(&aPtsAnother[0], 3, ALTERNATE);

	CBrush BrushRight;
	BrushRight.CreateSolidBrush(clr2);

	// �������Ľ������ұ�С�����ε�����
	HRGN hRgnRight = CreateRectRgn(1,1,1,1);
	CombineRgn(hRgnRight, hRgn3, hRgn4, RGN_AND);
	pDC->FillRgn(CRgn::FromHandle(hRgnRight), &BrushRight);
	
	::DeleteObject(hRgn1);
	::DeleteObject(hRgn2);
	::DeleteObject(hRgn3);
	::DeleteObject(hRgn4);
	::DeleteObject(hRgnLeft);
	::DeleteObject(hRgnRight);
	BrushLeft.DeleteObject();
	BrushRight.DeleteObject();
}


static int32 CStringToInt(TCHAR* pStrNum)
{
	if ( NULL == pStrNum )
	{
		return -1;
	}
	
	for ( int i = 0; i < (int)wcslen(pStrNum); i++ )
	{
		if ( !iswdigit(pStrNum[i]) )
		{
			return -1;
		}
	}
	
	//		
	std::string StrNumA;
	Unicode2MultiChar(CP_ACP, pStrNum, StrNumA);
	int iReval = atoi(StrNumA.c_str());
	
	return iReval;
}

static int32 CStringToInt(const CString& StrNum)
{
	CString StrNumTmp = StrNum;
	
	TCHAR* pStrNum = StrNumTmp.LockBuffer();
	
	int32 iReval = CStringToInt(pStrNum);
	
	StrNumTmp.UnlockBuffer();
	
	return iReval;
}

COLORREF GetBandClr(CString& StrClr)
{
	COLORREF clrRet = RGB(255,0,0);

	//
	if (StrClr.GetLength() != 9)
	{
		return clrRet;
	}

	//
	CString StrR = StrClr.Mid(0,3);
	CString StrG = StrClr.Mid(3,3);
	CString StrB = StrClr.Mid(6,3);

	int32 iR = CStringToInt(StrR);
	int32 iG = CStringToInt(StrG);
	int32 iB = CStringToInt(StrB);

	clrRet = RGB(iR, iG, iB);

	return clrRet;
}

void CIndexDrawer::DrawNodesAsBand(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, COLORREF clrIndex, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount)
{
	ChartCurve.SetOccupyIndexLineClr(false);

	if ( NULL == pDC || NULL == pPickDC || NULL == pNodeDrawerDatas || 0 >= iNodeCount )
	{
		////ASSERT(0);
		return;
	}
	
	//
	ChartCurve.SetOccupyIndexLineClr(false);

	//
	pPickDC->BeginDraw(&ChartCurve, 3);
	
	COLORREF clr = m_lColor;
	if ( -1 == m_lColor )	//	modify by weng.cx, form -1 == clr to  -1 == m_lColor 
	{
		clr = m_IoView.GetIoViewColor(ESCGuideLine1);			
	}
	CPen penDraw;
	penDraw.CreatePen(PS_SOLID, 1, clr);
	CPen *pPenOld = pDC->SelectObject(&penDraw);
	
	//
	T_MerchNodeUserData* pUserData = (T_MerchNodeUserData*)ChartCurve.GetNodes()->GetUserData();
	
	//
	CString StrText = L"";
	
	//
	if ( NULL != pUserData )
	{
		for ( int32 i = 0; i < pUserData->aIndexs.GetSize(); i++ )
		{
			// �ҵ���� region
			if ( pUserData->aIndexs.GetAt(i)->pRegion == &ChartCurve.GetChartRegion() )
			{
				for ( int32 j = 0; j < pUserData->aIndexs.GetAt(i)->m_aIndexLines.GetSize(); j++ )
				{
					// �ҵ���� churves
					if ( pUserData->aIndexs.GetAt(i)->m_aIndexLines.GetAt(j).m_pCurve == &ChartCurve )
					{
						StrText = pUserData->aIndexs.GetAt(i)->m_aIndexLines.GetAt(j).m_StrExtraData;
						break;
					}
				}
			}
		}		
	}
	
	COLORREF Clr1,Clr2;
	int32 iLen = StrText.GetLength();
	if ( iLen != 19 )
	{
		// ////ASSERT(0);
		Clr1 = RGB(255,0,0);
		Clr2 = RGB(0,255,0);
	}
	else
	{
		CString StrClr1, StrClr2;
		StrClr1 = StrText.Mid(0, 9);
		Clr1 = GetBandClr(StrClr1);
		
		//
		StrClr2 = StrText.Mid(10, 9);
		Clr2 = GetBandClr(StrClr2);
	}

	//
	CArray<CPoint, CPoint> apt1;
	CArray<CPoint, CPoint> apt2;
	bool32 bBig = false;
	bool32 bValid = false;

	CBrush brhClr1;
	brhClr1.CreateSolidBrush(Clr1);

	CBrush brhClr2;
	brhClr2.CreateSolidBrush(Clr2);
	
	CBrush* pOldBrush = NULL;

	//
	for ( int32 i = 0; i < iNodeCount; i++ )
	{
		if ( !CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid) )
		{	 
			bool32 bBigNow = false;
			CPoint pt1, pt2;

			pt1.x = pNodeDrawerDatas[i].m_iX;
			pt2.x = pNodeDrawerDatas[i].m_iX;

			if (pNodeDrawerDatas[i].m_iClose >= pNodeDrawerDatas[i].m_iOpen)
			{
				pt1.y = pNodeDrawerDatas[i].m_iClose;
				pt2.y = pNodeDrawerDatas[i].m_iOpen;
				bBigNow = true;
			}
			else
			{
				pt2.y = pNodeDrawerDatas[i].m_iClose;
				pt1.y = pNodeDrawerDatas[i].m_iOpen;	
			}

			//
			if (bValid)
			{
				if (bBig == bBigNow)
				{
					apt1.Add(pt1);

					// ���Ų���
					apt2.InsertAt(0, pt2);
				}
				else
				{
					// rgn ���	
					CPoint ptPre1, ptPre2;

					ptPre1 = apt1.GetAt(apt1.GetSize() - 1);
					ptPre2 = apt2.GetAt(0);

					//
					if (bBig)
					{
						FillCrossRgn(pDC, ptPre1, ptPre2, pt1, pt2, Clr1, Clr2);
					}
					else
					{
						FillCrossRgn(pDC, ptPre1, ptPre2, pt1, pt2, Clr2, Clr1);
					}
					
					// Ҫ��������
					apt1.Append(apt2);

					//
					if (bBig)
					{
						pOldBrush = pDC->SelectObject(&brhClr1);
					}
					else
					{
						pOldBrush = pDC->SelectObject(&brhClr2);
					}

					//
					CPoint* p = (CPoint*)apt1.GetData();

					pDC->BeginPath();
					pDC->Polyline(p, apt1.GetSize());
					pDC->EndPath();					
					pDC->FillPath();
					
					//
					apt1.RemoveAll();
					apt2.RemoveAll();
					
					//
					apt1.Add(pt1);
					apt2.InsertAt(0, pt2);
				}
			}
			else
			{
				bValid = true;

				apt1.Add(pt1);
				apt2.InsertAt(0, pt2);
			}

			bBig = bBigNow;

			// ������, ������
			if (i == iNodeCount - 1)
			{
				// Ҫ��������
				apt1.Append(apt2);
				
				if (bBigNow)
				{
					pOldBrush = pDC->SelectObject(&brhClr1);
				}
				else
				{
					pOldBrush = pDC->SelectObject(&brhClr2);
				}

				//
				CPoint* p = (CPoint*)apt1.GetData();
				
				pDC->BeginPath();
				pDC->Polyline(p, apt1.GetSize());
				pDC->EndPath();				
				pDC->FillPath();
				
				//
				apt1.RemoveAll();
				apt2.RemoveAll();
			}
 
		}		
	}
	
	pPickDC->EndDraw();	
	pDC->SelectObject(pPenOld);
	pDC->SelectObject(pOldBrush);
	brhClr1.DeleteObject();
	brhClr2.DeleteObject();
}

void CIndexDrawer::DrawNodesAsStickLine(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, COLORREF clrIndex, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount)
{
	ChartCurve.SetOccupyIndexLineClr(false);

	if ( NULL == pDC || NULL == pPickDC || NULL == pNodeDrawerDatas || 0 >= iNodeCount )
	{
		////ASSERT(0);
		return;
	}
	
	//
	ChartCurve.SetOccupyIndexLineClr(false);

	//
	pPickDC->BeginDraw(&ChartCurve, 3);
	
	COLORREF clr = m_lColor;
	if ( -1 == m_lColor )	//	modify by weng.cx, form -1 == clr to -1 == -1 == m_lColor
	{
		clr = m_IoView.GetIoViewColor(ESCGuideLine1);			
	}
	CPen penDraw;
	penDraw.CreatePen(PS_SOLID, 1, clr);
	CPen *pPenOld = pDC->SelectObject(&penDraw);
	
	//
	T_MerchNodeUserData* pUserData = (T_MerchNodeUserData*)ChartCurve.GetNodes()->GetUserData();
	
	//
	CString StrText = L"";
	
	//
	if ( NULL != pUserData )
	{
		for ( int32 i = 0; i < pUserData->aIndexs.GetSize(); i++ )
		{
			// �ҵ���� region
			if ( pUserData->aIndexs.GetAt(i)->pRegion == &ChartCurve.GetChartRegion() )
			{
				for ( int32 j = 0; j < pUserData->aIndexs.GetAt(i)->m_aIndexLines.GetSize(); j++ )
				{
					// �ҵ���� churves
					if ( pUserData->aIndexs.GetAt(i)->m_aIndexLines.GetAt(j).m_pCurve == &ChartCurve )
					{
						StrText = pUserData->aIndexs.GetAt(i)->m_aIndexLines.GetAt(j).m_StrExtraData;
						break;
					}
				}
			}
		}		
	}
	
	// ��Ϊʮ��֮x��emptyΪ0��ʵ�ģ�-1�����߿�����������ʵ�߿�����
	// �Ƿ���ĵ� StickLine
	bool32 bEmpty = false;
	bool32 bEmptyDotLine = false;
	bool32 bWidthLikeKline = false;

	// ���
	float  fWidth = 0.f;

	if ( StrText.GetLength() <= 0 )
	{
		////ASSERT(0);
	}
	else
	{
		if( StrText.GetLength() >= 3)
		{
			int32 iSepPos = StrText.Find(_T(' '));
			CString StrEmpty;
			if ( iSepPos >= 0 )
			{
				StrEmpty = StrText.Mid(iSepPos+1);
			}
			else
			{
				StrEmpty = StrText.Right(1);
			}
			
		
			std::string StrEmptyA;
			Unicode2MultiChar(CP_ACP, StrEmpty, StrEmptyA);
			int iEmpty	= atoi(StrEmptyA.c_str());
			bEmpty		=  0!=iEmpty;
			bEmptyDotLine =  -1 == iEmpty;

			StrText = StrText.Left(StrText.GetLength() - StrEmpty.GetLength());
			std::string StrWidthA;
			Unicode2MultiChar(CP_ACP, StrText, StrWidthA);
		
			double dWidth = atof(StrWidthA.c_str());
			if ( dWidth < 0.0 )
			{
				bWidthLikeKline = true;
				fWidth = 0.0f;	// �������Զ����
			}
			else
			{
				fWidth = (float)dWidth;
			}
		}
	}

	int32 iHalfWidth = 1;
	int32 iNodeWidth = pNodeDrawerDatas[0].m_iXRight - pNodeDrawerDatas[0].m_iXLeft;

	bool32 bAuto = false;

	if ( 0. == fWidth )
	{
		bAuto = true;
		iHalfWidth = iNodeWidth / 2;
	}
	else
	{
		iHalfWidth = (int32)((fWidth * iNodeWidth / 5) / 2);
	}
	
	
	if ( iHalfWidth < 1 )
	{
		iHalfWidth = 1;
	}
	else if ( iHalfWidth > 100 )
	{
		iHalfWidth = 100;
	}

	//
	for ( int32 i = 0; i < iNodeCount; i++ )
	{
		if ( !CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid) )
		{
			//		
			int32 iTop	  = pNodeDrawerDatas[i].m_iClose > pNodeDrawerDatas[i].m_iOpen ? pNodeDrawerDatas[i].m_iOpen : pNodeDrawerDatas[i].m_iClose;
			int32 iBottom = pNodeDrawerDatas[i].m_iClose > pNodeDrawerDatas[i].m_iOpen ? pNodeDrawerDatas[i].m_iClose : pNodeDrawerDatas[i].m_iOpen;
			int32 iLeft   = pNodeDrawerDatas[i].m_iX - iHalfWidth;
			int32 iRight  = pNodeDrawerDatas[i].m_iX + iHalfWidth;

			if ( bAuto )
			{
				iLeft  -= 1;
				iRight += 1;
			}
			if ( bWidthLikeKline )
			{
				int32 iMid = pNodeDrawerDatas[i].m_iX;
				pNodeDrawerDatas[i].CalcXPos(iLeft, iMid, iRight);
			}

			CRect rectStickLine(iLeft, iTop, iRight, iBottom);
		
			if ( bEmpty || rectStickLine.IsRectEmpty() )
			{	
				// �����ĵ� stickline ����ʵ�ĵ�ʱ�����Ϊ��(������ͬ����������ͬ)ʱ
				if ( bEmptyDotLine )
				{
					pDC->_DrawDotLine(iLeft, iTop, iRight, iTop, 2, clr);
					pDC->_DrawDotLine(iRight, iTop, iRight, iBottom, 2, clr);
					pDC->_DrawDotLine(iRight, iBottom, iLeft, iBottom, 2, clr);
					pDC->_DrawDotLine(iLeft, iBottom, iLeft, iTop, 2, clr);
				}
				else
				{
					pDC->MoveTo(iLeft, iTop);
					pDC->LineTo(iRight, iTop);
					pDC->LineTo(iRight, iBottom);
					pDC->LineTo(iLeft, iBottom);
					pDC->LineTo(iLeft, iTop);				
				}
			}
			else
			{
				rectStickLine.right += 1;
				rectStickLine.bottom += 1;
				pDC->FillSolidRect(rectStickLine, clr);	
			}
			
			pPickDC->FillSolidRect(rectStickLine, clr);
		}
	}

	pPickDC->EndDraw();

	pDC->SelectObject(pPenOld);
}

void CIndexDrawer::DrawNodesAsKLine(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, COLORREF clrIndex, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount)
{
	/*
	if ( NULL == pDC || NULL == pPickDC || NULL == pNodeDrawerDatas || 0 >= iNodeCount )
	{
		////ASSERT(0);
		return;
	}
	
	//
	pPickDC->BeginDraw(&ChartCurve, 3);

	//
	COLORREF clrRise = m_IoView.GetIoViewColor(ESCKLineRise);
	COLORREF clrKeep = m_IoView.GetIoViewColor(ESCKLineKeep);
	COLORREF clrFall = m_IoView.GetIoViewColor(ESCKLineFall);

	for ( int32 i = 0; i < iNodeCount; i++ )
	{
		int32 iXpos = 0;		
		int32 iLeft, iRight;

		pNodeDrawerDatas[i].CalcXPos(iLeft, iXpos, iRight);
		int32 iTop	  = pNodeDrawerDatas[i].m_iClose > pNodeDrawerDatas[i].m_iOpen ? pNodeDrawerDatas[i].m_iOpen : pNodeDrawerDatas[i].m_iClose;
		int32 iBottom = pNodeDrawerDatas[i].m_iClose > pNodeDrawerDatas[i].m_iOpen ? pNodeDrawerDatas[i].m_iClose : pNodeDrawerDatas[i].m_iOpen;

		CRect rectKLine(iLeft, iTop, iRight, iBottom);

		//
		COLORREF clr = clrRise;
		int32 iRiseFlag = 0;
		
		int32 y1 = pNodeDrawerDatas[i].m_iLow;
		int32 y2 = pNodeDrawerDatas[i].m_iOpen;
		int32 y3 = pNodeDrawerDatas[i].m_iClose;
		int32 y4 = pNodeDrawerDatas[i].m_iHigh;

		if ( pNodeDrawerDatas[i].m_fClose > pNodeDrawerDatas[i].m_fOpen )
		{
			// ��
			iRiseFlag = 1;
			clr = clrRise;
		}
		else if ( pNodeDrawerDatas[i].m_fClose == pNodeDrawerDatas[i].m_fOpen )
		{
			// ƽ
			iRiseFlag = 0;
			clr = clrKeep;
		}
		else
		{
			// ��
			iRiseFlag = -1;
			clr = clrFall;

			y2 = pNodeDrawerDatas[i].m_iClose;
			y3 = pNodeDrawerDatas[i].m_iOpen;
		}

		CPen pen(PS_SOLID, 1, clr);
		CPen* pOldPen = pDC->SelectObject(&pen);

		if ( 1 == iRiseFlag || 0 == iRiseFlag )
		{
			pDC->_DrawRect(rectKLine, clr);
		}
		else 
		{
			pDC->FillSolidRect(rectKLine, clr);
		}

		pPickDC->FillSolidRect(rectKLine, clr);

		// ������Ӱ��
		pDC->_DrawLine(iXpos, y1, iXpos, y2);
		pDC->_DrawLine(iXpos, y3, iXpos, y4);

		// 
		pPickDC->_DrawLine(iXpos, y1, iXpos, y2);
		pPickDC->_DrawLine(iXpos, y3, iXpos, y4);

		pDC->SelectObject(pOldPen);
		pen.DeleteObject();
	}

	//
	pPickDC->EndDraw();
	*/


	ChartCurve.SetOccupyIndexLineClr(false);

	ASSERT(NULL != pNodeDrawerDatas && iNodeCount > 0);
	if(NULL == pNodeDrawerDatas || iNodeCount <= 0)
	{
		return;
	}
	// ���ֻ��������K�ߣ���ʹ������ѹ�������������ܻ��������Ҫ������������
	CChartCurve *pDependentCurve = ChartCurve.GetChartRegion().GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		ASSERT( 0 );
		return;
	}
	
	// �ڻ��ƹ������ҵ������Сֵ��������Ϻ� ���������С
	float fYMin, fYMax;
	//lint --e{438}
	int32 iYMinPos, iYMaxPos;//lint !e438
	fYMin = FLT_MAX;
	fYMax = FLT_MIN;
	iYMinPos = iYMaxPos = -1;

	//
	ChartCurve.SetOccupyIndexLineClr(false);
	
	CPen PenRise, PenFall, PenKeep, *pOldPen = NULL;

	COLORREF clrRise = m_IoView.GetIoViewColor(ESCKLineRise);
	COLORREF clrFall = m_IoView.GetIoViewColor(ESCKLineFall);
	COLORREF clrKeep = m_IoView.GetIoViewColor(ESCKLineKeep);
	

	CFont	fontText, *pOldFont;
	fontText.CreateFontIndirect(m_IoView.GetIoViewFont(ESFSmall));

	PenRise.CreatePen(PS_SOLID, 1, clrRise);
	PenFall.CreatePen(PS_SOLID, 1, clrFall);
	PenKeep.CreatePen(PS_SOLID, 1, clrKeep);
	
	pOldPen = (CPen*)pDC->SelectObject(&PenRise);
	pOldFont = (CFont *)pDC->SelectObject(&fontText);
	
	CArray<CRect, CRect &>	aDrawRect;
	CArray<CNodeDrawerData *, CNodeDrawerData *>	aDrawDataPtrs;	//������������--��Ӧ��
	int32	iNeedDrawNodeCount	=	0;

	iNeedDrawNodeCount	=	CalcNeedCalcNodesOverlapKLine(pNodeDrawerDatas, iNodeCount, ChartCurve, aDrawRect, aDrawDataPtrs);


	ASSERT( iNeedDrawNodeCount > 0 );

	//K����volstick����
	E_IndexKlineDrawStyle eKlineDrawStyle = m_eIndexKlineDrawStyle;
	if ( EIKDSByMainKline == eKlineDrawStyle )
	{
		eKlineDrawStyle = (E_IndexKlineDrawStyle)CTabSplitWnd::m_pMainFram->SendMessage(UM_GetStaticMainKlineDrawStyle);
		//eKlineDrawStyle = (E_IndexKlineDrawStyle)CIoViewKLine::GetStaticMainKlineDrawStyle();
		if ( eKlineDrawStyle < 0 || eKlineDrawStyle >= EIKDSByMainKline )//lint !e568
		{
			eKlineDrawStyle = EIKDSNormal;	// ��Чֵʹ����ͨ��
		}
	}

	for (int32 i = iNeedDrawNodeCount-1; i >= 0; i--)
	{
		// ��ͼ
		CNodeDrawerData	*pNode	=	aDrawDataPtrs[i];
		
		int32 iRiseFallFlag = 0;
		COLORREF clrCur = clrKeep;
		CRect RectKLine	=	aDrawRect[i];
		RectKLine.top	=	pNode->m_iClose;		//Ĭ�����̼۱ȿ��̼۸�
		RectKLine.bottom	=	pNode->m_iOpen;
		if (pNode->m_fClose == pNode->m_fOpen)
		{
			iRiseFallFlag = 0;
			clrCur = clrKeep;
			pDC->SelectObject(&PenKeep);
		}
		else if (pNode->m_fClose >= pNode->m_fOpen)	// ��
		{
			iRiseFallFlag = 1;
			clrCur = clrRise;
			pDC->SelectObject(&PenRise);
		}
		else	// ��
		{
			iRiseFallFlag = -1;
			clrCur = clrFall;
			
			RectKLine.top	=	pNode->m_iOpen;				//���̼۱����̼۸�
			RectKLine.bottom	=	pNode->m_iClose;
			pDC->SelectObject(&PenFall);
		}

		if ( i == 0 )
		{			
			// �����黭������ɫ: ���߱������ɫ������һ������ɫ��ͬ
			// �Ż����������Ҫע����û��ʵ�ֺ�
			ChartCurve.m_clrTitleText = clrCur;
			//
		}

		// ����
		if (EIKDSNormal == eKlineDrawStyle || EIKDSFill == eKlineDrawStyle)	// ����|ʵ����
		{
			pDC->_DrawRect(RectKLine);
			
			if (EIKDSNormal == eKlineDrawStyle && iRiseFallFlag >= 0)	// ��
				;
			else
				pDC->_FillSolidRect(RectKLine, clrCur);
			
			//
			pDC->_DrawLine(pNode->m_iX, pNode->m_iHigh, pNode->m_iX, RectKLine.top);	//�ϰ������
			pDC->_DrawLine(pNode->m_iX, RectKLine.bottom, pNode->m_iX, pNode->m_iLow);	//�°������
		}
		else if (EIKDSAmerican == eKlineDrawStyle)
		{
			//����
			pDC->_DrawLine(pNode->m_iX, pNode->m_iHigh, pNode->m_iX, pNode->m_iLow);
			
			//2����
			pDC->_DrawLine(RectKLine.left, pNode->m_iOpen, pNode->m_iX, pNode->m_iOpen);	//��
			pDC->_DrawLine(RectKLine.right, pNode->m_iClose, pNode->m_iX, pNode->m_iClose);	//����
		}
		else if ( EIKDSTower == eKlineDrawStyle )
		{
			// �����̼���Ϊ�Ƚ�
			// 			1�����ɼ�����ʱ���ð�ɫ�����ɫ�����İ����ʾ�������ɼ��µ�ʱ���ú�ɫ������ɫ��ʵ�İ����ʾ��
			// 			2����ĳһ�յ����̼���Ϊ��׼�ۣ�ÿ�����ν���Ʊ���̼۵��ǵ�����ͼ���ϡ�
			// 			3�������һ�չɼ��������İ�ɫ���壬�������µ����ɼ�δ������һ�հ�ɫ����͵����һ���ֵ������ð�ɫ���壬���ƵĲ����ú�ɫ�����ʾ��
			// 			4�������һ�չɼ�Ϊ�µ��ĺ�ɫ���壬���������ǣ��ɼ�δ�ǹ���һ�պ�ɫ����ߵ����һ�����Ƿ����ú�ɫ���壬�ǹ��Ĳ����ð�ɫ�����ʾ��
			// ����		5��δ���������黭���������ơ�
			// ���������
			if ( i < iNeedDrawNodeCount-2 )	// ���ٵ�3�죬��Ҫǰ�������ݱȽ�
			{
				const CNodeDrawerData &NodePre = *aDrawDataPtrs[i+1];	// ��Ȼ����̫����ʱ���ݲ����ᣬ����Ӧ�ò�Ӱ��ͼ��
				const CNodeDrawerData &NodePrePre = *aDrawDataPtrs[i+2];
				ASSERT( NodePre.m_iID < pNode->m_iID );
				if ( NodePre.m_fClose >= NodePrePre.m_fClose )
				{
					// ��һ����, �ɼ�δ������һͼ�ε͵�(=ǰǰ��)�ĺ죬������
					if ( pNode->m_fClose >= NodePrePre.m_fClose )
					{
						// ������Ȼ���ǵ�, ��ǰ��-�����պ�
						CRect rcUp(RectKLine.left, pNode->m_iClose, RectKLine.right, NodePre.m_iClose);
						if ( rcUp.Height() <= 0 )
						{
							rcUp.bottom = rcUp.top+1;
						}
						if ( rcUp.Width() <= 0 )
						{
							rcUp.right = rcUp.left+1;
						}
						pDC->_FillSolidRect(rcUp, clrRise);
					}
					else
					{
						// ��ǰǰ��Ҫ���ˣ���ǰ��-ǰǰ�ջ��죬ǰǰ��-������
						CRect rcUp(RectKLine.left, NodePre.m_iClose, RectKLine.right, NodePrePre.m_iClose);
						if ( rcUp.Height() <= 0 )
						{
							rcUp.top = rcUp.bottom-1;
						}
						if ( rcUp.Width() <= 0 )
						{
							rcUp.right = rcUp.left+1;
						}
						pDC->_FillSolidRect(rcUp, clrRise);
						
						rcUp.top = NodePrePre.m_iClose;
						rcUp.bottom = pNode->m_iClose;
						if ( rcUp.Height() <= 0 )
						{
							rcUp.bottom = rcUp.top+1;
						}
						pDC->_FillSolidRect(rcUp, clrFall);
					}
				}
				else
				{
					// ��һ�յ�, �ɼ�δ������һͼ�θߵ�(=ǰǰ��)���ʣ����ƺ�
					if ( pNode->m_fClose <= NodePrePre.m_fClose )
					{
						// ������Ȼ�ǵ���, ��ǰ��-��������
						CRect rcUp(RectKLine.left, NodePre.m_iClose, RectKLine.right, pNode->m_iClose);
						if ( rcUp.Height() <= 0 )
						{
							rcUp.bottom = rcUp.top+1;
						}
						if ( rcUp.Width() <= 0 )
						{
							rcUp.right = rcUp.left+1;
						}
						pDC->_FillSolidRect(rcUp, clrFall);
					}
					else
					{
						// ��ǰǰ��Ҫ���ˣ���ǰ��-ǰǰ�ջ��̣�ǰǰ��-���պ�
						CRect rcUp(RectKLine.left, NodePrePre.m_iClose, RectKLine.right, NodePre.m_iClose);
						if ( rcUp.Height() <= 0 )
						{
							rcUp.top = rcUp.bottom-1;
						}
						if ( rcUp.Width() <= 0 )
						{
							rcUp.right = rcUp.left+1;
						}
						pDC->_FillSolidRect(rcUp, clrFall);
						
						rcUp.bottom = NodePrePre.m_iClose;
						rcUp.top	= pNode->m_iClose;
						if ( rcUp.Height() <= 0 )
						{
							rcUp.bottom = rcUp.top+1;
						}
						pDC->_FillSolidRect(rcUp, clrRise);
					}
				}
			}
			
		}
		else 
		{
			////ASSERT(0);
		}
// 		{
// 			pDC->_DrawRect(RectKLine);
// 		
// 			if (iRiseFallFlag >= 0)	// ��
// 				NULL;
//  			else
//  				pDC->_FillSolidRect(RectKLine, clrCur);
// 
// 			//
// 			pDC->_DrawLine(pNode->m_iX, pNode->m_iHigh, pNode->m_iX, RectKLine.top);	//�ϰ������
// 			pDC->_DrawLine(pNode->m_iX, RectKLine.bottom, pNode->m_iX, pNode->m_iLow);	//�°������
// 		}

		// ��������
		if ( pNode->m_fHigh >= fYMax )
		{
			// DrawArrowTextInRect(*pDC, RectDraw, pNode->m_iX, pNode->m_iHigh, StrYMax, clrText);
			// �����λ��
			fYMax = pNode->m_fHigh;
			iYMaxPos = i;
		}

		if ( pNode->m_fLow <= fYMin )
		{
			// ����Ϊ��С��
			// DrawArrowTextInRect(*pDC, RectDraw,  pNode->m_iX, pNode->m_iLow, StrYMin, clrText);
			fYMin = pNode->m_fLow;
			iYMinPos = i;
		}
	}
	
	// ��ѡ�б�־
	if (ChartCurve.IsSelect())
	{
		int32 iROPBack = pDC->SetROP2(R2_XORPEN);
		for (int32 i = 0; i < iNodeCount; i += iShowSelectFlagPeriod)
		{
			if (CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid))
			{
				continue;
			}
			
			int32 x1 = pNodeDrawerDatas[i].m_iX		- 4;	
			int32 x2 = pNodeDrawerDatas[i].m_iX		+ 4;
			int32 y1 = pNodeDrawerDatas[i].m_iClose - 4;	
			int32 y2 = pNodeDrawerDatas[i].m_iClose + 4;
			
			pDC->_FillSolidRect(CRect(x1, y1, x2, y2), 0xffffff);
		}
		pDC->SetROP2(iROPBack);
	}

	//
	if (NULL != pOldPen)
		pDC->SelectObject(pOldPen);
	
	if ( NULL != pOldFont )
	{
		pDC->SelectObject(pOldFont);
	}
	
	PenRise.DeleteObject();	
	PenFall.DeleteObject();	
	PenKeep.DeleteObject();	
}
