#include "StdAfx.h"
#include "ChartCurve.h"
#include "ChartRegion.h"
#include "IndexDrawer.h"
#include "coding.h"
#include "XLTimerTrace.h"
#include "TabSplitWnd.h"
#include "../res/resource.h"

const int32 KDefaultShowSelectedFlagCount = 10;	// 对于选中线， 默认显示N个点


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
	if (m_lColor < 0 || m_lColor > 0xffffff)	// 未指定颜色, 从自定义指标线颜色队列中取色
	{
		int32 iColorLoop = aNodeDrawerDatas[0].m_iCurveLoop % 16;
		E_SysColor eColor = (E_SysColor)(ESCGuideLine1 + iColorLoop);
		clrIndex = m_IoView.GetIoViewColor(eColor);
		ChartCurve.SetOccupyIndexLineClr(true);	// 占用了此颜色
	}
	else
	{
		clrIndex = m_lColor;
		ChartCurve.SetOccupyIndexLineClr(false);
	}

	// 设置书画文字颜色
	ChartCurve.m_clrTitleText = clrIndex;

	// 计算当前画选中标志的间隔粒度
	int32 iShowSelectFlagPeriod = (int32)((float)aNodeDrawerDatas.GetSize() / KDefaultShowSelectedFlagCount + 0.5f);
	if (iShowSelectFlagPeriod < 1)
		iShowSelectFlagPeriod = 1;

	// 不能在前面return, 有些颜色设置的会影响到DrawTitle 里的文字颜色
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
		// 单个分离节点
		DrawNodesAsAlone(pDC, pPickDC, clrIndex, iShowSelectFlagPeriod, ChartCurve, pNodeDrawerDatas, iNodeCount);
	}
	else if ( EIDSDrawText == m_eIndexDrawType )
	{
		// 画文字(在指定位置画出文字)
		DrawNodesAsText(pDC, pPickDC, clrIndex, iShowSelectFlagPeriod, ChartCurve, pNodeDrawerDatas, iNodeCount);
	}
	else if ( EIDSDrawNum == m_eIndexDrawType )
	{
		// 画数字
		DrawNodesAsNumText(pDC, pPickDC, clrIndex, iShowSelectFlagPeriod, ChartCurve, pNodeDrawerDatas, iNodeCount);
	}
	else if (EIDSDrawTextEx == m_eIndexDrawType)
	{
		// 画文字EX(在指定位置画出值)
		DrawNodesAsTextEx(pDC, pPickDC, clrIndex, iShowSelectFlagPeriod, ChartCurve, pNodeDrawerDatas, iNodeCount);
	}
	else if ( EIDSDrawIcon == m_eIndexDrawType )
	{
		// 画图标
		DrawNodesAsIcon(pDC, pPickDC, clrIndex, iShowSelectFlagPeriod, ChartCurve, pNodeDrawerDatas, iNodeCount);
	}
	else if ( EIDSDrawLine == m_eIndexDrawType )
	{
		// 画线
		DrawNodesAsLine(pDC, pPickDC, clrIndex, iShowSelectFlagPeriod, ChartCurve, pNodeDrawerDatas, iNodeCount);
	}
	else if ( EIDSStickLine == m_eIndexDrawType )
	{
		// 柱状线
		DrawNodesAsStickLine(pDC, pPickDC, clrIndex, iShowSelectFlagPeriod, ChartCurve, pNodeDrawerDatas, iNodeCount);
	}
	else if ( EIDSDrawBand == m_eIndexDrawType)
	{
		// 柱状线
		DrawNodesAsBand(pDC, pPickDC, clrIndex, iShowSelectFlagPeriod, ChartCurve, pNodeDrawerDatas, iNodeCount);
	}
	else if ( EIDSDrawKLine == m_eIndexDrawType && EIKDSClose != m_eIndexKlineDrawStyle )
	{
		// 画 K 线
		DrawNodesAsKLine(pDC, pPickDC, clrIndex, iShowSelectFlagPeriod, ChartCurve, pNodeDrawerDatas, iNodeCount);
	}	
	else if (EIDSDot == m_eIndexDrawType)
	{
		// 画曲线
		DrawNodesAsCurve(pDC, pPickDC, clrIndex, iShowSelectFlagPeriod, ChartCurve, pNodeDrawerDatas, iNodeCount, true);		
	}
	else
	{
		// 画曲线
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

// 优化后代码

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
	//xiali XL0005 不画重叠的
	CArray<CPoint, CPoint &>	aDrawPoints;
	aDrawPoints.SetSize(iNodeCount);
	CPoint		*pDrawPoints	=	aDrawPoints.GetData();
	int32		iDrawPointsCount	=	0;
	int32		iLastY	=	-1;
	int32		iLastValidPos = -1;	// 最后一个有效节点

	// 去掉了DRAWNULL 的这些点的记录
	map<int, CPoint> mapPointReal;

	//
	for (int32 i = 0; i < iNodeCount; i++)
	{		
		//可以做重叠判断？
		if (CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid))
		{
			// 非法节点跳过的话,会导致指标线断掉
			
			// 如果是非法的,就往前找到一个合法的节点,延续这个点的Y坐标
			// 如果前合法节点为分割节点，则空置该非法节点
			if ( -1 != iLastY && iLastValidPos >= 0 )
			{
				pDrawPoints[ iDrawPointsCount ].x	=	pNodeDrawerDatas[i].m_iX;
				pDrawPoints[ iDrawPointsCount ].y	=	iLastY;
				
				if ( pDrawPoints[iDrawPointsCount-1]!=pDrawPoints[iDrawPointsCount] )
				{
					if (!CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KDrawNULL))
					{
						// 如果是不画的节点
						mapPointReal[iDrawPointsCount] = pDrawPoints[iDrawPointsCount];
					}	

					iDrawPointsCount++;
				}
			}

			iLastValidPos = CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueDivide) ? -1 : iLastValidPos;	// 改点后面的点不能延续值
		}
		else
		{
			//合法节点
			iLastValidPos = CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueDivide) ? -1 : i;	// 改点后面的点不能延续值

			pDrawPoints[ iDrawPointsCount ].x	=	pNodeDrawerDatas[i].m_iX;
			pDrawPoints[ iDrawPointsCount ].y	=	pNodeDrawerDatas[i].m_iClose;
			if ( iDrawPointsCount ==0 || pDrawPoints[iDrawPointsCount-1]!=pDrawPoints[iDrawPointsCount] )
			{
				//第一个 || 相邻不相等的点
				iLastY	=	pDrawPoints[ iDrawPointsCount ].y;

				if (!CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KDrawNULL))
				{
					// 如果是不画的节点
					mapPointReal[iDrawPointsCount] = pDrawPoints[iDrawPointsCount];
				}	

				iDrawPointsCount++;
			}
		}	
	}

	if (mapPointReal.size() == iDrawPointsCount)
	{
		// 正常情况
		pDC->Polyline(pDrawPoints, iDrawPointsCount);
	}
	else
	{
		// 有需要跳空的节点, 分段画.	
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
					// 是延续上一个点的, 继续数组增加
					aPointNow.Add(pt);
				}
				else
				{
					// 断开了, 把前面一段画出来
					if (1 == aPointNow.GetSize())
					{
						// 画点
						pDC->SetPixel(aPointNow[0], clrIndex);
					}
					else
					{
						// 画线
						CPoint* pPtNow = (CPoint*)aPointNow.GetData();
						pDC->Polyline(pPtNow, aPointNow.GetSize());
					}
					
					// 清空
					aPointNow.RemoveAll();

					// 加上当前点
					aPointNow.Add(pt);
				}
			}

			//
			iIndexLast = iIndexNow;
		}

		// 还有多的, 画出来
		CPoint* pPtNow = (CPoint*)aPointNow.GetData();
		pDC->Polyline(pPtNow, aPointNow.GetSize());
		
		aPointNow.RemoveAll();
	}

	// 由于PickDC pen为3，所以在polyLine时，如果超出特定值()，会有性能上的损失，只有麻烦点了
	if ( iDrawPointsCount > 0 )
	{
		// 暂300吧
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

	// 画选中标志
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
 		float dashValues[] = {2, 4};  //像素长度为2，间距长度为4 的虚线
 		penNow.SetDashPattern(dashValues, 2);
		//penNow.SetDashStyle(DashStyleDot);
	}

	Gdiplus::Graphics graphics(pDC->GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);

	pPickDC->BeginDraw(&ChartCurve, 3);

	// 
	//xiali XL0005 不画重叠的
	CArray<Point, Point&>	aDrawPoints;
	aDrawPoints.SetSize(iNodeCount);
	Point		*pDrawPoints	=	aDrawPoints.GetData();
	int32		iDrawPointsCount	=	0;
	int32		iLastY	=	-1;
	int32		iLastValidPos = -1;	// 最后一个有效节点

	// 去掉了DRAWNULL 的这些点的记录
	map<int, Point> mapPointReal;

	//
	for (int32 i = 0; i < iNodeCount; i++)
	{		
		//可以做重叠判断？
		if (CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid))
		{
			// 非法节点跳过的话,会导致指标线断掉
			
			// 如果是非法的,就往前找到一个合法的节点,延续这个点的Y坐标
			// 如果前合法节点为分割节点，则空置该非法节点
			if ( -1 != iLastY && iLastValidPos >= 0 )
			{
				pDrawPoints[ iDrawPointsCount ].X	=	pNodeDrawerDatas[i].m_iX;
				pDrawPoints[ iDrawPointsCount ].Y	=	iLastY;
				
				if ( !pDrawPoints[iDrawPointsCount-1].Equals(pDrawPoints[iDrawPointsCount]) )
				{
					if (!CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KDrawNULL))
					{
						// 如果是不画的节点
						mapPointReal[iDrawPointsCount] = pDrawPoints[iDrawPointsCount];
					}	

					iDrawPointsCount++;
				}
			}

			iLastValidPos = CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueDivide) ? -1 : iLastValidPos;	// 改点后面的点不能延续值
		}
		else
		{
			//合法节点
			iLastValidPos = CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueDivide) ? -1 : i;	// 改点后面的点不能延续值

			pDrawPoints[ iDrawPointsCount ].X	=	pNodeDrawerDatas[i].m_iX;
			pDrawPoints[ iDrawPointsCount ].Y	=	pNodeDrawerDatas[i].m_iClose;
			if ( iDrawPointsCount ==0 || !(pDrawPoints[iDrawPointsCount-1].Equals(pDrawPoints[iDrawPointsCount])) )
			{
				//第一个 || 相邻不相等的点
				iLastY	=	pDrawPoints[ iDrawPointsCount ].Y;

				if (!CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KDrawNULL))
				{
					// 如果是不画的节点
					mapPointReal[iDrawPointsCount] = pDrawPoints[iDrawPointsCount];
				}	

				iDrawPointsCount++;
			}
		}	
	}

	if (mapPointReal.size() == iDrawPointsCount)
	{
		// 正常情况
	//	graphics.DrawCurve(&penNow, pDrawPoints, iDrawPointsCount);
		graphics.DrawLines(&penNow, pDrawPoints, iDrawPointsCount);
	}
	else
	{
		// 有需要跳空的节点, 分段画.	
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
					// 是延续上一个点的, 继续数组增加
					aPointNow.Add(pt);
				}
				else
				{
					// 断开了, 把前面一段画出来
					if (1 == aPointNow.GetSize())
					{
						// 画点
						pDC->SetPixel(CPoint(aPointNow[0].X, aPointNow[0].Y), clrIndex);
						
					}
					else
					{
						// 画线
						Point* pPtNow = (Point*)aPointNow.GetData();
					//	graphics.DrawCurve(&penNow, pPtNow, aPointNow.GetSize());
						graphics.DrawLines(&penNow, pPtNow, aPointNow.GetSize());
					}
					
					// 清空
					aPointNow.RemoveAll();

					// 加上当前点
					aPointNow.Add(pt);
				}
			}

			//
			iIndexLast = iIndexNow;
		}

		// 还有多的, 画出来
		Point* pPtNow = (Point*)aPointNow.GetData();
//		graphics.DrawCurve(&penNow, pPtNow, aPointNow.GetSize());
		graphics.DrawCurve(&penNow, pPtNow, aPointNow.GetSize());
		
		aPointNow.RemoveAll();
	}

	// 由于PickDC pen为3，所以在polyLine时，如果超出特定值()，会有性能上的损失，只有麻烦点了
	if ( iDrawPointsCount > 0 )
	{
		// 暂300吧
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

	// 画选中标志
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

//用同X轴像素点后一个周期覆盖前一个周期的方法，计算实际所需画的点
//每个周期一个单独的图形，该图形被周期的xLeft, xRight, yBottom, iValue所限制，在周期大量的时候，会重叠在同一个坐标X像素点上
//但是每种图形，所取的iValue可能会有所不同，现在两种图形所取的都是m_iVolumn，所以重叠计算就选在了外面
//先计算 需要画的矩形所对应的pNodeDrawerDatas 与 面积，根据面积进行覆盖考虑
//返回实际需要画的个数 - 实际返回的矩形和指针是以从视图右边到左边的顺序排序的，画的时候要从后面画到前面
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
		//大于1.0的，没必要进行重叠计算，计算小于1.0的重复点
		for (int32 i = iNodeCount - 1; i >= 0 ; i--)
		{
			if (CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid))
			{
				continue;
			}
			
			//同X，如果前面的没有现在的高(含一样)，则前面的被后面的覆盖，并再往前面寻找是否有同X轴的，在比较
			//		如果前面的比后面的高，则后面的被添加，不需要往前面找了
			//由于现在从后面开始计算，则变换规则（前面指前面记录的，大小是指fValue的大小，体现在坐标上就是越Top(Y值越小)的越大）:
			//				1. 没有前面大，可以忽略计算了
			//				2. 比前面的大，添加进去
			//现在没有区别闪电图、k线图之间的区别，闪电图的那种画法可以进一步优化
			//高为m_iVolumn
			
			CNodeDrawerData	&node	=	pNodeDrawerDatas[i];
			//倒计算
			if ( 0 == iNeedDrawNodeCount || aNeedDrawNodeDrawerDatas[iNeedDrawNodeCount-1]->m_iX != node.m_iX
				|| aNeedDrawNodeDrawerDatas[iNeedDrawNodeCount-1]->m_iVolumn > node.m_iVolumn )
			{
				//没有添加过 || 不是同X轴的 || 比前面记录的那个要大（不含等）(m_iVolumn越大，就是越bottom，级它的value就越下)
				aNeedDrawNodeDrawerDatas[ iNeedDrawNodeCount ]	=	pNodeDrawerDatas + i;
				
				//计算坐标
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
		//直接复制，排除Invalid
		for (int32 i = iNodeCount - 1; i >= 0 ; i--)
		{
			if (CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid))
			{
				continue;
			}
			
		
			//倒计算 - 与前面兼容
			
			
			aNeedDrawNodeDrawerDatas[ iNeedDrawNodeCount ]	=	pNodeDrawerDatas + i;
			//计算坐标
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


//用同X轴像素点中最大的矩形覆盖其它小的矩形 - 用于闪电图和分时的Vol画法，只保留同一X下最高的那个
//参数与返回值与考虑重叠的那个相同
//返回实际需要画的个数 - 实际返回的矩形和指针是以从视图右边到左边的顺序排序的，画的时候要从后面画到前面
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
		//大于1.0的，没必要进行重叠计算，计算小于1.0的重复点
		for (int32 i = iNodeCount - 1; i >= 0 ; i--)
		{
			if (CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid))
			{
				continue;
			}
			
			//闪电图和分时的vol颜色相同是相同的，而且现在也貌似不能改变，所以不用考虑后面的重叠前面的，直接只画最高的那个就可以了
			//高为m_iVolumn
			
			CNodeDrawerData	&node	=	pNodeDrawerDatas[i];
			//倒计算
			//重叠的计算方法
			// 			if ( 0 == iNeedDrawNodeCount || aNeedDrawNodeDrawerDatas[iNeedDrawNodeCount-1]->m_iX != node.m_iX
			// 				|| aNeedDrawNodeDrawerDatas[iNeedDrawNodeCount-1]->m_iVolumn > node.m_iVolumn )
			// 			{
			// 				//没有添加过 || 不是同X轴的 || 比前面记录的那个要大（不含等）(m_iVolumn越大，就是越bottom，级它的value就越下)
			// 				aNeedDrawNodeDrawerDatas[ iNeedDrawNodeCount ]	=	pNodeDrawerDatas + i;
			// 				
			// 				//计算坐标
			// 				int32 x1 = 0, x2 = 0, x3 = 0;
			// 				pNodeDrawerDatas[i].CalcXPos(x1, x2, x3);		//根据现有闪电图画法，这个都可以忽略- -，只需要一个top和bottom就可以了
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

			//直接选取Top
			if ( 0 == iNeedDrawNodeCount || aNeedDrawNodeDrawerDatas[iNeedDrawNodeCount-1]->m_iX != node.m_iX )
			{
				//没有添加过 || 不是同X轴的 
				//添加
				aNeedDrawNodeDrawerDatas[ iNeedDrawNodeCount ]	=	pNodeDrawerDatas + i;
				
				//计算坐标
				int32 x1 = 0, x2 = 0, x3 = 0;
				pNodeDrawerDatas[i].CalcXPos(x1, x2, x3);		//根据现有闪电图画法，这个都可以忽略- -，只需要一个top和bottom就可以了
				
			
				
				aNeedDrawRects[ iNeedDrawNodeCount ].left	=	x1;
				aNeedDrawRects[ iNeedDrawNodeCount ].top		=	pNodeDrawerDatas[i].m_iVolumn;
				aNeedDrawRects[ iNeedDrawNodeCount ].right	=	x3;
				aNeedDrawRects[ iNeedDrawNodeCount ].bottom	=	rectDraw.bottom;
				
				iNeedDrawNodeCount++;
			}
			else if  ( aNeedDrawNodeDrawerDatas[iNeedDrawNodeCount-1]->m_iVolumn > node.m_iVolumn )
			{
				//比前面记录的那个同X要大（不含等）(m_iVolumn越大，就是越bottom，级它的value就越下)
				//修改前一个记录为top一点的新记录
				aNeedDrawNodeDrawerDatas[ iNeedDrawNodeCount - 1 ]	=	pNodeDrawerDatas + i;
				
				//计算坐标
				int32 x1 = 0, x2 = 0, x3 = 0;
				pNodeDrawerDatas[i].CalcXPos(x1, x2, x3);		//根据现有闪电图画法，这个都可以忽略- -，只需要一个top和bottom就可以了
				
				aNeedDrawRects[ iNeedDrawNodeCount - 1 ].left	=	x1;
				aNeedDrawRects[ iNeedDrawNodeCount - 1 ].top		=	pNodeDrawerDatas[i].m_iVolumn;
				aNeedDrawRects[ iNeedDrawNodeCount - 1 ].right	=	x3;
				aNeedDrawRects[ iNeedDrawNodeCount - 1 ].bottom	=	rectDraw.bottom;
				
			}
		}
	}
	else
	{
		//直接复制，排除Invalid
		for (int32 i = iNodeCount - 1; i >= 0 ; i--)
		{
			if (CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid))
			{
				continue;
			}
			
			
			//倒计算 - 与前面兼容
			
			
			aNeedDrawNodeDrawerDatas[ iNeedDrawNodeCount ]	=	pNodeDrawerDatas + i;
			//计算坐标
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
		clrRise = m_IoView.GetIoViewColor(ESCRise);	// 分时类型使用与分时相同的涨跌色
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
	ChartCurve.SetOccupyIndexLineClr(false);	// 只有部分线需要这个参数

	PenRise.CreatePen(PS_SOLID, 1, clrRise);
	PenFall.CreatePen(PS_SOLID, 1, clrFall);
	PenTrend.CreatePen(PS_SOLID,1, clrTrend);
	
	pOldPen = (CPen*)pDC->SelectObject(&PenRise);
	
	// 
	// pPickDC->BeginDraw(&ChartCurve, 10);

	// 
	//xiali XL0005 不画重叠的, 画最后一个
	//int32	iLastX	=	-1;
	//bool32 bHasMoveTo = false;

	CRect	rectDraw	=	ChartCurve.GetChartRegion().GetRectCurves();

	//实际上即使是画矩形，也会由于X间距太小而变成画线，在大数据量时，反正是要被同一X点的后面的覆盖，可以考虑忽略

	//每个周期一个单独的图形，该图形被周期的xLeft, xRight, yBottom, iValue所限制，在周期大量的时候，会重叠在同一个坐标X像素点上
	//但是每种图形，所取的iValue可能会有所不同，现在两种图形所取的都是m_iVolumn，所以重叠计算就选在了外面
	//先计算 需要画的矩形所对应的pNodeDrawerDatas 与 面积，根据面积进行覆盖考虑，存储，在根据筛选出来的结果，进行以前的绘制
	CArray<CRect, CRect &>	aDrawRect;
	CArray<CNodeDrawerData *, CNodeDrawerData *>	aDrawDataPtrs;	//这两个数组是--对应的
	aDrawRect.SetSize(iNodeCount);
	aDrawDataPtrs.SetSize(iNodeCount);
	int32	iNeedDrawNodeCount	=	0;
	
	//判断语句转移到循环外面来
	if (EIDSVolStick == m_eIndexDrawType)	// 成交量线
	{
		//以后如果有Y值不同的计算，可能要将重叠计算移到这里来
		
		// 成交量图第一个柱状的不占用指标线颜色
		ChartCurve.SetOccupyIndexLineClr(false);

		//OK, 计算完了
		
		if ( 1 != ChartCurve.GetChartRegion().GetParentIoViewData().pChartRegionData->m_iChartType )
		{
			// 分时 & 闪电，主要是闪电有缩放，很费时，分时和闪电只有一个颜色，所以可以不用考虑后面覆盖问题
			//由于是倒序，从后面计算到前面吧
			//点不是连续的，必须要一个一个画
			iNeedDrawNodeCount	=	CalcNeedCalcNodesCoverVol(pNodeDrawerDatas, iNodeCount, ChartCurve, aDrawRect, aDrawDataPtrs);
			ASSERT( iNeedDrawNodeCount >= 0 );

			bool32 bShowVolColor =  CTabSplitWnd::m_pMainFram->SendMessage(UM_IsShowVolBuySellColor);//CIoViewChart::IsShowVolBuySellColor();
			
			// 分时走势、闪电的副图bottom row不能显示：被clipRgn给clip了
			//     原因：由于副图的Y值根据vol的MinMaxY来确定，所以导致bottom row恰好与rectView的bottom一样
			//     解决（临时）：将所有有值的即将绘制的rect 提高1
			//     TODO：解决rectView与CurveView，边框绘制的关系
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
					////lint ENCFColorFall,ENCFColorRise值为有符号负数，aDrawDataPtrs[i]->m_bClrValid为无符号，永远无法进入case中暂无修改办法 modifyer weng.cx
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
			//K线画法
			for ( int32 i=iNeedDrawNodeCount-1; i >= 0; i-- )
			{
				int32 iRiseFallFlag = 1;
				COLORREF clrCur = clrRise;
				
				// 涨跌色
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
				// 底部留点空隙,美观一些
				// 应该在计算坐标的时候就要空出3个像素，不然到这里会有显示与数据的不匹配 - 已经不匹配了- -，现在只是调整没那么难看
				aDrawRect[i].bottom -= 3;
				if ( aDrawRect[i].Height() < 0 )
				{
					aDrawRect[i].top = aDrawRect[i].bottom;
				}
				
				
				
				pDC->_DrawRect( aDrawRect[i] );
				
				if (iRiseFallFlag < 0)	// 跌
					pDC->_FillSolidRect(aDrawRect[i], clrCur);				
				//
				CRect RectPick(aDrawDataPtrs[i]->m_iXLeft, rectDraw.bottom, aDrawDataPtrs[i]->m_iXRight, aDrawRect[i].top);
				// pPickDC->_FillSolidRect(RectPick, clrCur);
			}
			
		}
	}//end if EIDSVolStick 成交量线
	else if ( EIDSStick == m_eIndexDrawType )
	{
		int32 y2 = 0;
		
		int32	y0	=	0;	//基准值Y0
		//ChartCurve.PriceYToRegionY(0.0,y0);
		ChartCurve.AxisYToRegionY(0.0f, y0);
		ChartCurve.GetChartRegion().RegionYToClient(y0);				
		ChartCurve.GetChartRegion().ClipY(y0);
		
		iNeedDrawNodeCount	=	CalcNeedCalcNodesOverlapVol(pNodeDrawerDatas, iNodeCount, ChartCurve,
			aDrawRect, aDrawDataPtrs);
		ASSERT( iNeedDrawNodeCount > 0 );
		
		COLORREF clrCur = clrIndex;
		ChartCurve.SetOccupyIndexLineClr(bOccupy);	// 需要颜色参数的
				
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
		// COLORSTICK 以 0 为分界线.上下画线 有+值，与-值
		// 由于存在-值显示，重叠优化需要重算，因为同X轴可能存在最大的+值与-值都需要显示，最后的值一定要显示
		// 但是密集的时候很难有用户注意有效信息，先公用
		
		ChartCurve.SetOccupyIndexLineClr(false);

		int32 y2 = 0;
		
		int32	y0	=	0;	//基准值Y0
		//ChartCurve.PriceYToRegionY(0.0,y0);
		float fBase = 0.0f;
		if ( CPriceToAxisYObject::EAYT_Normal != ChartCurve.GetAxisYType() )
		{
			ChartCurve.GetAxisBaseY(fBase);
			ChartCurve.AxisYToPriceY(fBase, fBase);
		}
		else if( CheckFlag(ChartCurve.m_iFlag, CChartCurve::KTypeTrend) )
		{
			// 线性坐标, 分时图(现在只有分时副图的线会进入这里)
			ChartCurve.GetAxisBaseY(fBase);
			ChartCurve.AxisYToPriceY(fBase, fBase);
		}
		ChartCurve.PriceYToRegionY(fBase, y0);
		ChartCurve.GetChartRegion().RegionYToClient(y0);				
		ChartCurve.GetChartRegion().ClipY(y0);

		iNeedDrawNodeCount	=	CalcNeedCalcNodesOverlapVol(pNodeDrawerDatas, iNodeCount, ChartCurve,
															aDrawRect, aDrawDataPtrs);
		//ASSERT( iNeedDrawNodeCount > 0 );	// 有些线是没有数据

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
		// 画点	
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
		// zhangbo 20090717 #待补充
		//...
	}
	else 
	{
		////ASSERT(0);
	}
	
	// pPickDC->EndDraw();
	
	// 画选中标志
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
			// 找到这个 region
			if ( pUserData->aIndexs.GetAt(i)->pRegion == &ChartCurve.GetChartRegion() )
			{
				for ( int32 j = 0; j < pUserData->aIndexs.GetAt(i)->m_aIndexLines.GetSize(); j++ )
				{
					// 找到这个 churves
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
			// 找到这个 region
			if ( pUserData->aIndexs.GetAt(i)->pRegion == &ChartCurve.GetChartRegion() )
			{
				for ( int32 j = 0; j < pUserData->aIndexs.GetAt(i)->m_aIndexLines.GetSize(); j++ )
				{
					// 找到这个 churves
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
					// 找到这个 churves
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
			// 不处理，只是设置成不可能的值
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
			// 找到这个 region
			if ( pUserData->aIndexs.GetAt(i)->pRegion == &ChartCurve.GetChartRegion() )
			{
				for ( int32 j = 0; j < pUserData->aIndexs.GetAt(i)->m_aIndexLines.GetSize(); j++ )
				{
					// 找到这个 churves
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

// 			if (3==iIndexIcon || 4==iIndexIcon) // 现在手指图标不对称，偏移3个像素
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

	// 画线
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

	// 画选中标志
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

	// 这两个的交集是左边小三角形的区域
	HRGN hRgnLeft = CreateRectRgn(1,1,1,1);
	CombineRgn(hRgnLeft, hRgn1, hRgn2, RGN_AND);
	pDC->FillRgn(CRgn::FromHandle(hRgnLeft), &BrushLeft);

	// 右边的
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

	// 这两个的交集是右边小三角形的区域
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
			// 找到这个 region
			if ( pUserData->aIndexs.GetAt(i)->pRegion == &ChartCurve.GetChartRegion() )
			{
				for ( int32 j = 0; j < pUserData->aIndexs.GetAt(i)->m_aIndexLines.GetSize(); j++ )
				{
					// 找到这个 churves
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

					// 倒着插入
					apt2.InsertAt(0, pt2);
				}
				else
				{
					// rgn 填充	
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
					
					// 要画出来了
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

			// 结束了, 画出来
			if (i == iNodeCount - 1)
			{
				// 要画出来了
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
			// 找到这个 region
			if ( pUserData->aIndexs.GetAt(i)->pRegion == &ChartCurve.GetChartRegion() )
			{
				for ( int32 j = 0; j < pUserData->aIndexs.GetAt(i)->m_aIndexLines.GetSize(); j++ )
				{
					// 找到这个 churves
					if ( pUserData->aIndexs.GetAt(i)->m_aIndexLines.GetAt(j).m_pCurve == &ChartCurve )
					{
						StrText = pUserData->aIndexs.GetAt(i)->m_aIndexLines.GetAt(j).m_StrExtraData;
						break;
					}
				}
			}
		}		
	}
	
	// 宽为十分之x，empty为0则实心，-1则虚线空心柱，否则实线空心柱
	// 是否空心的 StickLine
	bool32 bEmpty = false;
	bool32 bEmptyDotLine = false;
	bool32 bWidthLikeKline = false;

	// 宽度
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
				fWidth = 0.0f;	// 负数则自动变宽
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
				// 画空心的 stickline 或者实心的时候矩形为空(上下相同或者左右相同)时
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
			// 涨
			iRiseFlag = 1;
			clr = clrRise;
		}
		else if ( pNodeDrawerDatas[i].m_fClose == pNodeDrawerDatas[i].m_fOpen )
		{
			// 平
			iRiseFlag = 0;
			clr = clrKeep;
		}
		else
		{
			// 跌
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

		// 画上下影线
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
	// 这个只会用来画K线，即使有数据压缩，最高与最低总会出现在需要画的数组里面
	CChartCurve *pDependentCurve = ChartCurve.GetChartRegion().GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		ASSERT( 0 );
		return;
	}
	
	// 于绘制过程中找到最大，最小值，绘制完毕后 画出最大最小
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
	CArray<CNodeDrawerData *, CNodeDrawerData *>	aDrawDataPtrs;	//这两个数组是--对应的
	int32	iNeedDrawNodeCount	=	0;

	iNeedDrawNodeCount	=	CalcNeedCalcNodesOverlapKLine(pNodeDrawerDatas, iNodeCount, ChartCurve, aDrawRect, aDrawDataPtrs);


	ASSERT( iNeedDrawNodeCount > 0 );

	//K线与volstick类似
	E_IndexKlineDrawStyle eKlineDrawStyle = m_eIndexKlineDrawStyle;
	if ( EIKDSByMainKline == eKlineDrawStyle )
	{
		eKlineDrawStyle = (E_IndexKlineDrawStyle)CTabSplitWnd::m_pMainFram->SendMessage(UM_GetStaticMainKlineDrawStyle);
		//eKlineDrawStyle = (E_IndexKlineDrawStyle)CIoViewKLine::GetStaticMainKlineDrawStyle();
		if ( eKlineDrawStyle < 0 || eKlineDrawStyle >= EIKDSByMainKline )//lint !e568
		{
			eKlineDrawStyle = EIKDSNormal;	// 无效值使用普通线
		}
	}

	for (int32 i = iNeedDrawNodeCount-1; i >= 0; i--)
	{
		// 画图
		CNodeDrawerData	*pNode	=	aDrawDataPtrs[i];
		
		int32 iRiseFallFlag = 0;
		COLORREF clrCur = clrKeep;
		CRect RectKLine	=	aDrawRect[i];
		RectKLine.top	=	pNode->m_iClose;		//默认收盘价比开盘价高
		RectKLine.bottom	=	pNode->m_iOpen;
		if (pNode->m_fClose == pNode->m_fOpen)
		{
			iRiseFallFlag = 0;
			clrCur = clrKeep;
			pDC->SelectObject(&PenKeep);
		}
		else if (pNode->m_fClose >= pNode->m_fOpen)	// 涨
		{
			iRiseFallFlag = 1;
			clrCur = clrRise;
			pDC->SelectObject(&PenRise);
		}
		else	// 跌
		{
			iRiseFallFlag = -1;
			clrCur = clrFall;
			
			RectKLine.top	=	pNode->m_iOpen;				//开盘价比收盘价高
			RectKLine.bottom	=	pNode->m_iClose;
			pDC->SelectObject(&PenFall);
		}

		if ( i == 0 )
		{			
			// 设置书画文字颜色: 曲线标题的颜色和最新一根的颜色相同
			// 优化后这个功能要注意有没有实现好
			ChartCurve.m_clrTitleText = clrCur;
			//
		}

		// 画吧
		if (EIKDSNormal == eKlineDrawStyle || EIKDSFill == eKlineDrawStyle)	// 空心|实心线
		{
			pDC->_DrawRect(RectKLine);
			
			if (EIKDSNormal == eKlineDrawStyle && iRiseFallFlag >= 0)	// 涨
				;
			else
				pDC->_FillSolidRect(RectKLine, clrCur);
			
			//
			pDC->_DrawLine(pNode->m_iX, pNode->m_iHigh, pNode->m_iX, RectKLine.top);	//上半截竖线
			pDC->_DrawLine(pNode->m_iX, RectKLine.bottom, pNode->m_iX, pNode->m_iLow);	//下半截竖线
		}
		else if (EIKDSAmerican == eKlineDrawStyle)
		{
			//竖线
			pDC->_DrawLine(pNode->m_iX, pNode->m_iHigh, pNode->m_iX, pNode->m_iLow);
			
			//2横线
			pDC->_DrawLine(RectKLine.left, pNode->m_iOpen, pNode->m_iX, pNode->m_iOpen);	//左开
			pDC->_DrawLine(RectKLine.right, pNode->m_iClose, pNode->m_iX, pNode->m_iClose);	//右收
		}
		else if ( EIKDSTower == eKlineDrawStyle )
		{
			// 以收盘价作为比较
			// 			1、当股价上涨时，用白色（或红色）空心棒体表示，而当股价下跌时，用黑色（或绿色）实心棒体表示。
			// 			2、以某一日的收盘价作为基准价，每日依次将股票收盘价的涨跌画于图表上。
			// 			3、如果上一日股价是涨升的白色棒体，而次日下跌，股价未跌破上一日白色棒体低点的那一部分跌幅仍用白色棒体，跌破的部分用黑色棒体表示。
			// 			4、如果上一日股价为下跌的黑色棒体，而次日上涨，股价未涨过上一日黑色棒体高点的那一部分涨幅仍用黑色棒体，涨过的部分用白色棒体表示。
			// 　　		5、未来的日行情画法依次类推。
			// 点序是逆的
			if ( i < iNeedDrawNodeCount-2 )	// 至少第3天，需要前两天数据比较
			{
				const CNodeDrawerData &NodePre = *aDrawDataPtrs[i+1];	// 虽然缩的太厉害时数据不连贯，但是应该不影响图形
				const CNodeDrawerData &NodePrePre = *aDrawDataPtrs[i+2];
				ASSERT( NodePre.m_iID < pNode->m_iID );
				if ( NodePre.m_fClose >= NodePrePre.m_fClose )
				{
					// 上一日涨, 股价未跌破上一图形低点(=前前收)的红，跌破绿
					if ( pNode->m_fClose >= NodePrePre.m_fClose )
					{
						// 今日仍然是涨的, 画前收-今日收红
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
						// 比前前收要低了，将前收-前前收画红，前前收-今收绿
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
					// 上一日跌, 股价未涨破上一图形高点(=前前收)的率，涨破红
					if ( pNode->m_fClose <= NodePrePre.m_fClose )
					{
						// 今日仍然是跌的, 画前收-今日收绿
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
						// 比前前收要高了，将前收-前前收画绿，前前收-今收红
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
// 			if (iRiseFallFlag >= 0)	// 涨
// 				NULL;
//  			else
//  				pDC->_FillSolidRect(RectKLine, clrCur);
// 
// 			//
// 			pDC->_DrawLine(pNode->m_iX, pNode->m_iHigh, pNode->m_iX, RectKLine.top);	//上半截竖线
// 			pDC->_DrawLine(pNode->m_iX, RectKLine.bottom, pNode->m_iX, pNode->m_iLow);	//下半截竖线
// 		}

		// 画最高最低
		if ( pNode->m_fHigh >= fYMax )
		{
			// DrawArrowTextInRect(*pDC, RectDraw, pNode->m_iX, pNode->m_iHigh, StrYMax, clrText);
			// 保存该位置
			fYMax = pNode->m_fHigh;
			iYMaxPos = i;
		}

		if ( pNode->m_fLow <= fYMin )
		{
			// 设置为最小的
			// DrawArrowTextInRect(*pDC, RectDraw,  pNode->m_iX, pNode->m_iLow, StrYMin, clrText);
			fYMin = pNode->m_fLow;
			iYMinPos = i;
		}
	}
	
	// 画选中标志
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
