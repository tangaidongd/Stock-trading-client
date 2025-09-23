#include "stdafx.h"

#include "facescheme.h"
#include "ChartDrawer.h"
#include "ChartRegion.h"

#include "XLTimerTrace.h"

const int32 KDefaultShowSelectedFlagCount = 10;	// 对于选中线， 默认显示N个点


CChartDrawer::CChartDrawer(IIoViewBase &IoView, E_ChartDrawStyle eChartDrawType)
:m_IoView(IoView)
{
	m_bGetClrFromView = false;
	m_eChartDrawType  = eChartDrawType;

	m_clrSingle = CLR_NONE;	// 无单独颜色
}

void CChartDrawer::DrawNodes(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, IN CChartCurve &ChartCurve, const CArray<CNodeDrawerData, CNodeDrawerData&> &aNodeDrawerDatas)
{
	if (aNodeDrawerDatas.GetSize() <= 0)
		return;

	// 计算当前画选中标志的间隔粒度
	int32 iShowSelectFlagPeriod = (int32)((float)aNodeDrawerDatas.GetSize() / KDefaultShowSelectedFlagCount + 0.5f);
	if (iShowSelectFlagPeriod < 1)
		iShowSelectFlagPeriod = 1;

	// 
	CNodeDrawerData *pNodeDrawerDatas = (CNodeDrawerData *)aNodeDrawerDatas.GetData();
	int32 iNodeCount = aNodeDrawerDatas.GetSize();

	if (EKDSTrendPrice	== m_eChartDrawType ||
		EKDSTrendJunXian== m_eChartDrawType ||
		EKDSClose		== m_eChartDrawType ||
		EKDSTickPrice	== m_eChartDrawType)
	{
		DrawNodesAsCurve(pDC, pPickDC, iShowSelectFlagPeriod, ChartCurve, pNodeDrawerDatas, iNodeCount);
	}
	else
	{
		DrawNodesAsAlone(pDC, pPickDC, iShowSelectFlagPeriod, ChartCurve, pNodeDrawerDatas, iNodeCount);
	}
}

// 绘制水平实心箭头+文字，文字的字体使用的是dc中select的，箭头宽1，颜色为参数，类似  XXX→
static void DrawArrowTextInRect(CDC &dc, const CRect &RectDest, int32 iArrowX, int32 iArrowY, const CString &StrText, COLORREF clrText)
{
	static const float fPI = 3.14159f;
	static const int32 iConstArrowLen = 15; // 箭头水平X长度 →，含头部
	static const int32 iConstArrowHeaderLen = 6; // 头部
	//static const int32 iConstArrowEndX = iConstArrowHeaderLen * cos(25 / 360.0);
	//static const int32 iConstArrowEndY = iConstArrowHeaderLen * sin(25 / 360.0);
	static const float fConstArrowInnerAngle = fPI/8; // 22.5 O
	
	CPoint pointDst(iArrowX, iArrowY);
	if ( !RectDest.PtInRect(pointDst) )
	{
		//////ASSERT(0);	
		return; // 暂时不支持指向矩形外面的操作	
	}
	
	TEXTMETRIC tmText;
	if ( !dc.GetTextMetrics(&tmText) )
	{
		return;
	}
	
	int32 iTextHeight = tmText.tmHeight + tmText.tmInternalLeading;
	
	float fArrowAngle = 0;/*atan( (float)iTextHeight/2.0 / iConstArrowLen )*/;
	int32 iArrowTopHalfX, iArrowTopHalfY;
	int32 iArrowBottomHalfX, iArrowBottomHalfY;
	if ( iArrowX > RectDest.left + RectDest.Width()/2 )
	{
		// 点在右半边，则画在左边
		
	}
	else
	{
		// 画在右边
		fArrowAngle = fPI - fArrowAngle;
	}
	
	if ( iArrowY > RectDest.top + RectDest.Height()/2 )
	{
		// 点在下半边，画在上边
		fArrowAngle = -fArrowAngle;
	}
	else
	{
		// 画在下边
		
	}
	int32 iArrowLen   = (int32)(sqrt( pow(iTextHeight/2.0f, 2.0f) + pow((float)iConstArrowLen, 2.0f) ));
	int32 iArrowEndX  = iArrowX - (int32)(iArrowLen * cos( fArrowAngle )); // 是否需要考虑+-四舍五入
	int32 iArrowEndY  = iArrowY + (int32)(iArrowLen * sin( fArrowAngle ));
	iArrowTopHalfX    = iArrowX - (int32)(iConstArrowHeaderLen * cos( fArrowAngle - fConstArrowInnerAngle ));
	iArrowBottomHalfX = iArrowX - (int32)(iConstArrowHeaderLen * cos( fArrowAngle + fConstArrowInnerAngle ));
	iArrowTopHalfY    = iArrowY + (int32)(iConstArrowHeaderLen * sin( fArrowAngle - fConstArrowInnerAngle ));
	iArrowBottomHalfY = iArrowY + (int32)(iConstArrowHeaderLen * sin( fArrowAngle + fConstArrowInnerAngle ));
	
	// 水平箭头，文字竖直居中
	
	int32 iMaxTextWidth = dc.GetTextExtent(StrText).cx + 1;
	CRect RectText(iArrowEndX, (int32)(iArrowY - iTextHeight/2.0f), iArrowEndX + iMaxTextWidth, (int32)(iArrowY + (iTextHeight/2.0f + 0.5f)));
	if ( iArrowEndX < iArrowX )
	{
		// 文字左边
		RectText.OffsetRect(-iMaxTextWidth, 0);
	}
	
	CRect RectDraw;
	//BOOL bUnion = RectDraw.UnionRect(RectText, CRect(iArrowX, iArrowY, iArrowX+1, iArrowY+1));
	
	
	int32 iBkMode = dc.SetBkMode(TRANSPARENT);
	COLORREF clrOldText = dc.SetTextColor(clrText);
	CPen  penLine(PS_SOLID, 1, clrText);
	CPen  *pOldPen = dc.SelectObject(&penLine);
	CBrush brush;
	brush.CreateSolidBrush(clrText);
	CBrush *pOldBrush = dc.SelectObject(&brush);
	
	POINT  ptArrows[3];
	ptArrows[0].x = iArrowX;
	ptArrows[0].y = iArrowY;
	ptArrows[1].x = iArrowTopHalfX;
	ptArrows[1].y = iArrowTopHalfY;
	ptArrows[2].x = iArrowBottomHalfX;
	ptArrows[2].y = iArrowBottomHalfY;
	dc.Polygon(ptArrows, 3);
 	dc.MoveTo(iArrowX, iArrowY);
 	dc.LineTo(iArrowEndX, iArrowEndY);
// 	dc.MoveTo(iArrowX, iArrowY);
// 	dc.LineTo(iArrowTopHalfX, iArrowTopHalfY);
// 	dc.MoveTo(iArrowX, iArrowY);
// 	dc.LineTo(iArrowBottomHalfX, iArrowBottomHalfY);
	
	dc.DrawText(StrText, RectText, DT_SINGLELINE | DT_BOTTOM);
	
	dc.SelectObject(&pOldBrush);
	dc.SelectObject(pOldPen);
	dc.SetBkMode(iBkMode);
	dc.SetTextColor(clrOldText);
}

void CChartDrawer::DrawNodesAsCurve(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount)
{
	if(NULL == pNodeDrawerDatas || iNodeCount <= 0)
	{
		return;
	}
	ASSERT(NULL != pNodeDrawerDatas && iNodeCount > 0);
	
	COLORREF clrLine = 0xffffff;
	if (EKDSTrendPrice == m_eChartDrawType)
		clrLine = m_IoView.GetIoViewColor(ESCKLineKeep);		// 持平颜色
	else if (EKDSTrendJunXian == m_eChartDrawType)
		clrLine = m_IoView.GetIoViewColor(ESCVolume2);
	else if (EKDSClose == m_eChartDrawType)
		clrLine = m_IoView.GetIoViewColor(ESCKLineRise);
	else if (EKDSTickPrice == m_eChartDrawType)
		clrLine = m_IoView.GetIoViewColor(ESCKLineKeep/*ESCVolume*/);
	else
		;////ASSERT(0);

	const bool32 bUseSingleColor = m_clrSingle != CLR_NONE && m_clrSingle != CLR_DEFAULT;
	if ( bUseSingleColor )
	{
		clrLine = m_clrSingle;
	}

	// 设置书画文字颜色
	ChartCurve.m_clrTitleText = clrLine;
	//
	

	CPen pen,*pOldPen = NULL;
	pen.CreatePen(PS_SOLID, 1, clrLine);
	pOldPen = (CPen*)pDC->SelectObject(&pen);


	//过滤掉同一个X坐标像素中重复的部分，统一同一X像素内的点为一个直线，同一X像素内开始一个点和最后一个点一定要记录
	//即如果有同一X像素的点大于4个，则最多有进入点、最高点、最低点、离开点 4点
	//如果这样计算，不知道划得来不
	CArray<CPoint, CPoint &>	aDrawPoints;
	aDrawPoints.SetSize(iNodeCount + 4);		//额外的2个点， TopMost, BottomMost
	CArray<int32, int32>	aTempY;
	CPoint		*pDrawPoints	=	aDrawPoints.GetData();
	int32		iDrawPointsCount	=	0;
	CPoint		pointLast(-1, -1);
	bool32		bTerminated	=	false;
	CNodeDrawerData	*pLastValidDrawNode = NULL;

	//应该同一X点多于4个点数，才使用这种方法压缩数据，否则就是拖慢了计算速度 - 现在貌似速度根本没优化
	for (int32 i = 0; i < iNodeCount; i++)
	{
		CNodeDrawerData	*pNode	=	pNodeDrawerDatas+i;

		if (CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid))
		{
			//原来的实现是希望出现断点，即忽略中间所有的错误点，错误点相关的不画
			//记录错误点前面的最后一个正确点，然后比较错误点后面的第一个正确点，如果X像素相同，则忽略该错误点
			//如果x不同，则绘制前面所有的点（或者存储这些点组，以后在分组绘制，不采用存储，发现X不同且被截断则绘制）

			// 为了避免非法节点截断数据，需要判断是否有继承前一个数据的标志
			// 如果有继承前一个数据的标志，则继承前面最后一个没有被输出的数据
			if ( CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInhert)
				&& iDrawPointsCount > 0
				&& pLastValidDrawNode != NULL )
			{
				pNode = pLastValidDrawNode;
			}
			else
			{
				bTerminated	=	true;
				continue;
			}
		}
		else
		{
			pLastValidDrawNode = pNode;	// 最后一个有效节点保留
		}
	
		if ( pNode->m_iX != pointLast.x || bTerminated )
		{
			//X点不同 或者要求马上输出
			if ( bTerminated )
			{
				if ( iDrawPointsCount > 0 )
				{
					//被截断了,绘制 -	如果记录的最后一个点不是pointLast则将pointLast加入，绘制，不加其实也没关系，反正也不是连续的线
					//TODO 绘制
					pDC->Polyline(pDrawPoints, iDrawPointsCount);
					pPickDC->Polyline(pDrawPoints, iDrawPointsCount);
					
					//还原状态 - 新得到的这个点就好像是新的点一样了
					iDrawPointsCount	=	0;	
				}
				pLastValidDrawNode = NULL;	// 输出完成后就没有有效点了
			}
			else
			{
				//没被截断的话，要将有效的其它点（Top,bottom,last）加入点组
				//由于前面没有进行iDrawPointsCount的操作，所以 iDrawPointsCount-1 指向的是入口点
				if ( iDrawPointsCount > 0 )
				{
					//有效的pointLast
					//iDrawPointsCount -1指向入口点，0 指向topMost，1指向bottomMost，2指向了pointLast
					// 
					//aTempY.SetSize( 0 );
					int32	iTempPos	=	iDrawPointsCount;
					if ( pDrawPoints[iDrawPointsCount-1].y != pDrawPoints[iDrawPointsCount].y )
					{
						//保留最Top，跳过TopMost，TopMost已经赋值，不需要再赋值
						iTempPos++;
						//aTempY.Add( pDrawPoints[iDrawPointsCount].y );
					}

					if ( pDrawPoints[iDrawPointsCount-1] != pDrawPoints[iDrawPointsCount+1] )
					{
						//保留最Bottom，跳过bottomMost，赋值，有可能这个位置是TopMost的
						pDrawPoints[iTempPos++] = pDrawPoints[iDrawPointsCount+1];
						//aTempY.Add( pDrawPoints[iDrawPointsCount+1].y );
					}

					if ( iTempPos > iDrawPointsCount  && pDrawPoints[iTempPos - 1].y != pointLast.y)
					{
						//保留pointLast - 有其它保留，且最后一个保留不为最后的值
						pDrawPoints[iTempPos++]	=	pointLast;
						//aTempY.Add(pointLast.y);
					}
					else
					{
						//如果没有其它保留，则证明pointLast是跟入口点一样的，因为前面x轴相等比较过了
						//如果有其它保留，则pointLast是和最后一个值是相等的
						//除非调整入口点 - top - bottom - 出口点 画点顺序，否则只能节约点到这里了
					}

					iDrawPointsCount	=	iTempPos;

					//赋值保留的值，如果没保留的值，就只保存了一个入口点
// 					for ( int j=0; j < aTempY.GetSize(); j++ ) 
// 					{
// 						pDrawPoints[ iDrawPointsCount++ ].y	= aTempY[j];
// 					}

				}
				
			}

			//新点初始化
			pDrawPoints[ iDrawPointsCount ].x	=	pNode->m_iX;
			pDrawPoints[ iDrawPointsCount ].y	=	pNode->m_iClose;
			pointLast		=	pDrawPoints[ iDrawPointsCount ];
			pDrawPoints[ iDrawPointsCount+1 ]		=	pDrawPoints[ iDrawPointsCount ];	//topMost
			pDrawPoints[ iDrawPointsCount+2 ]		=	pDrawPoints[ iDrawPointsCount ];	//bottomMost
			iDrawPointsCount++;		//实际有效点只有一个
		}
		else
		{
			//X点相同 - 进行最高、最低判断
			// iDrawPointsCount已经指向了TopMost, +1指向了bottomMost
			if ( pDrawPoints[ iDrawPointsCount ].y > pNode->m_iClose )
			{
				//比top的点还top
				pDrawPoints[ iDrawPointsCount ].y	=	pNode->m_iClose;
			}
			else if ( pDrawPoints[ iDrawPointsCount+1 ].y < pNode->m_iClose )
			{
				//比bottom的点还bottom
				pDrawPoints[ iDrawPointsCount+1 ].y	=	pNode->m_iClose;
			}
			pointLast.y		=	pNode->m_iClose;		//只要记录一个Y值就可以了，X肯定相同
		}

		//取消截断点标志
		bTerminated	=	CheckFlag(pNode->m_iFlag, CNodeData::KValueDivide);
		// 如果设置了截断标志，需要添加值后截断绘制
	}

	if (iDrawPointsCount > 0 && iNodeCount > 0)
	{
		//
		if (pDrawPoints[iDrawPointsCount-1].y != pNodeDrawerDatas[iNodeCount-1].m_iClose)
		{
			if (pDrawPoints[iDrawPointsCount-1].x == pNodeDrawerDatas[iNodeCount-1].m_iX)
			{
				pDrawPoints[iDrawPointsCount-1].y = pNodeDrawerDatas[iNodeCount-1].m_iClose;
			}
			else
			{
				iDrawPointsCount += 1;
				
				pDrawPoints[iDrawPointsCount-1].x = pNodeDrawerDatas[iNodeCount-1].m_iX;
				pDrawPoints[iDrawPointsCount-1].y = pNodeDrawerDatas[iNodeCount-1].m_iClose;
			}
		}
	}

	pDC->Polyline(pDrawPoints, iDrawPointsCount);
	// pickDC中的Pen是？
	pPickDC->Polyline(pDrawPoints, iDrawPointsCount);

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

	pen.DeleteObject();	
}


// void CChartDrawer::DrawNodesAsCurve(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount)
// {
// 	ASSERT(NULL != pNodeDrawerDatas && iNodeCount > 0);
// 	
// 	COLORREF clrLine = 0xffffff;
// 	if (EKDSTrendPrice == m_eChartDrawType)
// 		clrLine = m_IoView.GetIoViewColor(ESCKLineKeep);		// 持平颜色
// 	else if (EKDSTrendJunXian == m_eChartDrawType)
// 		clrLine = m_IoView.GetIoViewColor(ESCVolume2);
// 	else if (EKDSClose == m_eChartDrawType)
// 		clrLine = m_IoView.GetIoViewColor(ESCKLineRise);
// 	else if (EKDSTickPrice == m_eChartDrawType)
// 		clrLine = m_IoView.GetIoViewColor(ESCKLineKeep/*ESCVolume*/);
// 	else
// 		////ASSERT(0);
// 
// 	const bool32 bUseSingleColor = m_clrSingle != CLR_NONE && m_clrSingle != CLR_DEFAULT;
// 	if ( bUseSingleColor )
// 	{
// 		clrLine = m_clrSingle;
// 	}
// 
// 	// 设置书画文字颜色
// 	ChartCurve.m_clrTitleText = clrLine;
// 
// 	//
// 	Color clr(GetRValue(clrLine), GetGValue(clrLine), GetBValue(clrLine));
// 	Pen penNow(clr, 1);
// 
// 	Gdiplus::Graphics graphics(pDC->GetSafeHdc());
// 	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
// 
// 	//过滤掉同一个X坐标像素中重复的部分，统一同一X像素内的点为一个直线，同一X像素内开始一个点和最后一个点一定要记录
// 	//即如果有同一X像素的点大于4个，则最多有进入点、最高点、最低点、离开点 4点
// 	//如果这样计算，不知道划得来不
// 	CArray<Point, Point &>	aDrawPoints;
// 	aDrawPoints.SetSize(iNodeCount + 4);		//额外的2个点， TopMost, BottomMost
// 	CArray<int32, int32>	aTempY;
// 	Point		*pDrawPoints	=	aDrawPoints.GetData();
// 	int32		iDrawPointsCount	=	0;
// 	Point		pointLast(-1, -1);
// 	bool32		bTerminated	=	false;
// 	CNodeDrawerData	*pLastValidDrawNode = NULL;
// 
// 	//应该同一X点多于4个点数，才使用这种方法压缩数据，否则就是拖慢了计算速度 - 现在貌似速度根本没优化
// 	for (int32 i = 0; i < iNodeCount; i++)
// 	{
// 		CNodeDrawerData	*pNode	=	pNodeDrawerDatas+i;
// 
// 		if (CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid))
// 		{
// 			//原来的实现是希望出现断点，即忽略中间所有的错误点，错误点相关的不画
// 			//记录错误点前面的最后一个正确点，然后比较错误点后面的第一个正确点，如果X像素相同，则忽略该错误点
// 			//如果x不同，则绘制前面所有的点（或者存储这些点组，以后在分组绘制，不采用存储，发现X不同且被截断则绘制）
// 
// 			// 为了避免非法节点截断数据，需要判断是否有继承前一个数据的标志
// 			// 如果有继承前一个数据的标志，则继承前面最后一个没有被输出的数据
// 			if ( CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInhert)
// 				&& iDrawPointsCount > 0
// 				&& pLastValidDrawNode != NULL )
// 			{
// 				pNode = pLastValidDrawNode;
// 			}
// 			else
// 			{
// 				bTerminated	=	true;
// 				continue;
// 			}
// 		}
// 		else
// 		{
// 			pLastValidDrawNode = pNode;	// 最后一个有效节点保留
// 		}
// 	
// 		if ( pNode->m_iX != pointLast.X || bTerminated )
// 		{
// 			//X点不同 或者要求马上输出
// 			if ( bTerminated )
// 			{
// 				if ( iDrawPointsCount > 0 )
// 				{
// 					//被截断了,绘制 -	如果记录的最后一个点不是pointLast则将pointLast加入，绘制，不加其实也没关系，反正也不是连续的线
// 					//TODO 绘制
// 					graphics.DrawCurve(&penNow, pDrawPoints, iDrawPointsCount);
// 					
// 					//还原状态 - 新得到的这个点就好像是新的点一样了
// 					iDrawPointsCount	=	0;	
// 				}
// 				pLastValidDrawNode = NULL;	// 输出完成后就没有有效点了
// 			}
// 			else
// 			{
// 				//没被截断的话，要将有效的其它点（Top,bottom,last）加入点组
// 				//由于前面没有进行iDrawPointsCount的操作，所以 iDrawPointsCount-1 指向的是入口点
// 				if ( iDrawPointsCount > 0 )
// 				{
// 					//有效的pointLast
// 					//iDrawPointsCount -1指向入口点，0 指向topMost，1指向bottomMost，2指向了pointLast
// 					// 
// 					//aTempY.SetSize( 0 );
// 					int32	iTempPos	=	iDrawPointsCount;
// 					if ( pDrawPoints[iDrawPointsCount-1].Y != pDrawPoints[iDrawPointsCount].Y )
// 					{
// 						//保留最Top，跳过TopMost，TopMost已经赋值，不需要再赋值
// 						iTempPos++;
// 						//aTempY.Add( pDrawPoints[iDrawPointsCount].y );
// 					}
// 
// 					if ( !pDrawPoints[iDrawPointsCount-1].Equals(pDrawPoints[iDrawPointsCount+1]) )
// 					{
// 						//保留最Bottom，跳过bottomMost，赋值，有可能这个位置是TopMost的
// 						pDrawPoints[iTempPos++] = pDrawPoints[iDrawPointsCount+1];
// 						//aTempY.Add( pDrawPoints[iDrawPointsCount+1].y );
// 					}
// 
// 					if ( iTempPos > iDrawPointsCount  && pDrawPoints[iTempPos - 1].Y != pointLast.Y)
// 					{
// 						//保留pointLast - 有其它保留，且最后一个保留不为最后的值
// 						pDrawPoints[iTempPos++]	=	pointLast;
// 						//aTempY.Add(pointLast.y);
// 					}
// 					else
// 					{
// 						//如果没有其它保留，则证明pointLast是跟入口点一样的，因为前面x轴相等比较过了
// 						//如果有其它保留，则pointLast是和最后一个值是相等的
// 						//除非调整入口点 - top - bottom - 出口点 画点顺序，否则只能节约点到这里了
// 					}
// 
// 					iDrawPointsCount	=	iTempPos;
// 
// 					//赋值保留的值，如果没保留的值，就只保存了一个入口点
// // 					for ( int j=0; j < aTempY.GetSize(); j++ ) 
// // 					{
// // 						pDrawPoints[ iDrawPointsCount++ ].y	= aTempY[j];
// // 					}
// 
// 				}
// 				
// 			}
// 
// 			//新点初始化
// 			pDrawPoints[ iDrawPointsCount ].X	=	pNode->m_iX;
// 			pDrawPoints[ iDrawPointsCount ].Y	=	pNode->m_iClose;
// 			pointLast		=	pDrawPoints[ iDrawPointsCount ];
// 			pDrawPoints[ iDrawPointsCount+1 ]		=	pDrawPoints[ iDrawPointsCount ];	//topMost
// 			pDrawPoints[ iDrawPointsCount+2 ]		=	pDrawPoints[ iDrawPointsCount ];	//bottomMost
// 			iDrawPointsCount++;		//实际有效点只有一个
// 		}
// 		else
// 		{
// 			//X点相同 - 进行最高、最低判断
// 			// iDrawPointsCount已经指向了TopMost, +1指向了bottomMost
// 			if ( pDrawPoints[ iDrawPointsCount ].Y > pNode->m_iClose )
// 			{
// 				//比top的点还top
// 				pDrawPoints[ iDrawPointsCount ].Y	=	pNode->m_iClose;
// 			}
// 			else if ( pDrawPoints[ iDrawPointsCount+1 ].Y < pNode->m_iClose )
// 			{
// 				//比bottom的点还bottom
// 				pDrawPoints[ iDrawPointsCount+1 ].Y	=	pNode->m_iClose;
// 			}
// 			pointLast.Y		=	pNode->m_iClose;		//只要记录一个Y值就可以了，X肯定相同
// 		}
// 
// 		//取消截断点标志
// 		bTerminated	=	CheckFlag(pNode->m_iFlag, CNodeData::KValueDivide);
// 		// 如果设置了截断标志，需要添加值后截断绘制
// 	}
// 
// 	graphics.DrawCurve(&penNow, pDrawPoints, iDrawPointsCount);
// 	
// 	// pickDC中的Pen是？
// 	// pPickDC->Polyline(pDrawPoints, iDrawPointsCount);
// 
// 	// 画选中标志
// 	if (ChartCurve.IsSelect())
// 	{
// 		int32 iROPBack = pDC->SetROP2(R2_XORPEN);
// 		for (int32 i = 0; i < iNodeCount; i += iShowSelectFlagPeriod)
// 		{
// 			if (CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid))
// 			{
// 				continue;
// 			}
// 			
// 			int32 x1 = pNodeDrawerDatas[i].m_iX		- 4;	
// 			int32 x2 = pNodeDrawerDatas[i].m_iX		+ 4;
// 			int32 y1 = pNodeDrawerDatas[i].m_iClose - 4;	
// 			int32 y2 = pNodeDrawerDatas[i].m_iClose + 4;
// 			
// 			pDC->_FillSolidRect(CRect(x1, y1, x2, y2), 0xffffff);
// 		}
// 		pDC->SetROP2(iROPBack);
// 	}
// }

void CChartDrawer::DrawNodesAsAlone(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount)
{
	if(NULL == pNodeDrawerDatas || iNodeCount <= 0)
	{
		return;
	}
	ASSERT(NULL != pNodeDrawerDatas && iNodeCount > 0);

	// 这个只会用来画K线，即使有数据压缩，最高与最低总会出现在需要画的数组里面
	CChartCurve *pDependentCurve = ChartCurve.GetChartRegion().GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		//ASSERT( 0 );
		return;
	}
	
	// 于绘制过程中找到最大，最小值，绘制完毕后 画出最大最小
	float fYMin, fYMax;
	int32 iYMinPos, iYMaxPos;
	fYMin = FLT_MAX;
	fYMax = -FLT_MAX;
	iYMinPos = iYMaxPos = -1;

	const bool32 bDrawMinMax = CheckFlag(ChartCurve.m_iFlag, CChartCurve::KDependentCurve);	// 主图才需要画最大最小值
	
	CPen PenRise, PenFall, PenKeep, *pOldPen = NULL;

	COLORREF clrRise = m_IoView.GetIoViewColor(ESCKLineRise);
	COLORREF clrFall = m_IoView.GetIoViewColor(ESCKLineFall);
	COLORREF clrKeep = m_IoView.GetIoViewColor(ESCKLineKeep);
	COLORREF clrText = m_IoView.GetIoViewColor(ESCText);

	const bool32 bUseSingleColor = m_clrSingle != CLR_NONE && m_clrSingle != CLR_DEFAULT;
	if ( bUseSingleColor )
	{
		clrRise = m_clrSingle;
		clrFall = m_clrSingle;
		clrText = m_clrSingle;
	}

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

	//
	COLORREF* pColorFromView = (COLORREF*)m_aColorsFromView.GetData();
	int32 iSizeColorFromView = m_aColorsFromView.GetSize();
	CPen* pPenColorFromView  = NULL;

	//K线与volstick类似
	int32 KIDiameter	= aDrawRect.GetSize()>0? aDrawRect[0].Width() : 0;
	int32 KIRadius		= KIDiameter/2;
	CBrush BrushRise, BrushFall, BrushKeep;
	if ( EKDSArbitrageKline == m_eChartDrawType )
	{
		KIRadius	= max(1, KIRadius);
		KIRadius	= min(5, KIRadius);
		KIDiameter	= max(3, KIDiameter);
		KIDiameter	= min(10, KIDiameter);

		BrushRise.CreateSolidBrush(clrRise);
		BrushFall.CreateSolidBrush(clrFall);
		BrushKeep.CreateSolidBrush(clrKeep);
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

		//
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

		CPen penClrKLine;
		
		if ( m_bGetClrFromView && NULL != pColorFromView )
		{			
			if ( pNode->m_iPos < iSizeColorFromView )
			{
				clrCur = pColorFromView[pNode->m_iPos];				
				penClrKLine.CreatePen(PS_SOLID, 1, clrCur);
				pOldPen = pDC->SelectObject(&penClrKLine);
				pPenColorFromView = &penClrKLine;
			}			
		}
	
		//
		if ( i == 0 )
		{			
			// 设置书画文字颜色: 曲线标题的颜色和最新一根的颜色相同
			// 优化后这个功能要注意有没有实现好
			ChartCurve.m_clrTitleText = clrCur;
			//
		}

		// 画吧
		if (EKDSNormal == m_eChartDrawType || EKDSFillRect == m_eChartDrawType)	// 空心线
		{
			pDC->_DrawRect(RectKLine);
		
			if (EKDSNormal == m_eChartDrawType && iRiseFallFlag >= 0)	// 涨
				;
 			else
 				pDC->_FillSolidRect(RectKLine, clrCur);

			//
			pDC->_DrawLine(pNode->m_iX, pNode->m_iHigh, pNode->m_iX, RectKLine.top);	//上半截竖线
			pDC->_DrawLine(pNode->m_iX, RectKLine.bottom, pNode->m_iX, pNode->m_iLow);	//下半截竖线
		}
		else if (EKDSAmerican == m_eChartDrawType)
		{
			//竖线
			pDC->_DrawLine(pNode->m_iX, pNode->m_iHigh, pNode->m_iX, pNode->m_iLow);

			//2横线
			pDC->_DrawLine(RectKLine.left, pNode->m_iOpen, pNode->m_iX, pNode->m_iOpen);	//左开
			pDC->_DrawLine(RectKLine.right, pNode->m_iClose, pNode->m_iX, pNode->m_iClose);	//右收
		}
		else if ( EKDSTower == m_eChartDrawType )
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
		else if ( EKDSArbitrageKline == m_eChartDrawType )
		{
			// 套利K线: 用收盘价价差、开盘价价差、均价价差做K线，收盘价价差大于开盘价价差画红线，反之画绿线
			// 开收直线，均价处一个3*2小实心矩形
			int32 iHigh = max(pNode->m_iClose, pNode->m_iOpen);
			int32 iLow	= min(pNode->m_iClose, pNode->m_iOpen);
			iHigh	= max(pNode->m_iAvg, iHigh);
			iLow	= min(pNode->m_iAvg, iLow);
			pDC->_DrawLine(pNode->m_iX, iHigh, pNode->m_iX, iLow);
			int32 y = pNode->m_iAvg;
			CRect rcAvg(CPoint(aDrawDataPtrs[i]->m_iX-KIRadius, y-KIRadius), CSize(KIDiameter, KIDiameter));
			
			if ( iRiseFallFlag > 0 )
			{
				pDC->SelectObject(&BrushRise);
			}
			else if ( iRiseFallFlag == 0 )
			{
				pDC->SelectObject(&BrushKeep);
			}
			else
			{
				pDC->SelectObject(&BrushFall);
			}
			pDC->Ellipse(&rcAvg);
		}
		else 
		{
			////ASSERT(0);
		}

		// 画最高最低
		if ( bDrawMinMax && pNode->m_fHigh >= fYMax )
		{
			// DrawArrowTextInRect(*pDC, RectDraw, pNode->m_iX, pNode->m_iHigh, StrYMax, clrText);
			// 保存该位置
			fYMax = pNode->m_fHigh;
			iYMaxPos = i;
		}

		if ( bDrawMinMax && pNode->m_fLow <= fYMin )
		{
			// 设置为最小的
			// DrawArrowTextInRect(*pDC, RectDraw,  pNode->m_iX, pNode->m_iLow, StrYMin, clrText);
			fYMin = pNode->m_fLow;
			iYMinPos = i;
		}

		if ( NULL != pPenColorFromView )
		{
			pDC->SelectObject(pOldPen);
			pPenColorFromView->DeleteObject();
		}
	}

	// 画最大，最小标志
	CString StrYMin, StrYMax;
	StrYMin = ChartCurve.GetChartRegion().GetViewData()->OnFloat2String(fYMin, false);
	StrYMax = ChartCurve.GetChartRegion().GetViewData()->OnFloat2String(fYMax, false);
	CRect   RectDraw = ChartCurve.GetChartRegion().GetRectCurves();
	if ( -1 != iYMaxPos )
	{
		DrawArrowTextInRect(*pDC, RectDraw, aDrawDataPtrs[iYMaxPos]->m_iX, aDrawDataPtrs[iYMaxPos]->m_iHigh, StrYMax, clrText);
	}
	if ( -1 != iYMinPos )
	{
		DrawArrowTextInRect(*pDC, RectDraw, aDrawDataPtrs[iYMinPos]->m_iX, aDrawDataPtrs[iYMinPos]->m_iLow, StrYMin, clrText);
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

void CChartDrawer::SetColors(CArray<COLORREF, COLORREF>& aColors)
{
// 	int32 iSize = aColors.GetSize();
// 	m_aColorsFromView.SetSize(iSize);
// 
// 	if ( iSize <= 0 )
// 	{
// 		return;
// 	}
// 
// 	memcpyex(aColors.GetData(), m_aColorsFromView.GetData(), sizeof(COLORREF) * iSize);

	m_aColorsFromView.Copy(aColors);
}