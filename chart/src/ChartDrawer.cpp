#include "stdafx.h"

#include "facescheme.h"
#include "ChartDrawer.h"
#include "ChartRegion.h"

#include "XLTimerTrace.h"

const int32 KDefaultShowSelectedFlagCount = 10;	// ����ѡ���ߣ� Ĭ����ʾN����


CChartDrawer::CChartDrawer(IIoViewBase &IoView, E_ChartDrawStyle eChartDrawType)
:m_IoView(IoView)
{
	m_bGetClrFromView = false;
	m_eChartDrawType  = eChartDrawType;

	m_clrSingle = CLR_NONE;	// �޵�����ɫ
}

void CChartDrawer::DrawNodes(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, IN CChartCurve &ChartCurve, const CArray<CNodeDrawerData, CNodeDrawerData&> &aNodeDrawerDatas)
{
	if (aNodeDrawerDatas.GetSize() <= 0)
		return;

	// ���㵱ǰ��ѡ�б�־�ļ������
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

// ����ˮƽʵ�ļ�ͷ+���֣����ֵ�����ʹ�õ���dc��select�ģ���ͷ��1����ɫΪ����������  XXX��
static void DrawArrowTextInRect(CDC &dc, const CRect &RectDest, int32 iArrowX, int32 iArrowY, const CString &StrText, COLORREF clrText)
{
	static const float fPI = 3.14159f;
	static const int32 iConstArrowLen = 15; // ��ͷˮƽX���� ������ͷ��
	static const int32 iConstArrowHeaderLen = 6; // ͷ��
	//static const int32 iConstArrowEndX = iConstArrowHeaderLen * cos(25 / 360.0);
	//static const int32 iConstArrowEndY = iConstArrowHeaderLen * sin(25 / 360.0);
	static const float fConstArrowInnerAngle = fPI/8; // 22.5 O
	
	CPoint pointDst(iArrowX, iArrowY);
	if ( !RectDest.PtInRect(pointDst) )
	{
		//////ASSERT(0);	
		return; // ��ʱ��֧��ָ���������Ĳ���	
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
		// �����Ұ�ߣ��������
		
	}
	else
	{
		// �����ұ�
		fArrowAngle = fPI - fArrowAngle;
	}
	
	if ( iArrowY > RectDest.top + RectDest.Height()/2 )
	{
		// �����°�ߣ������ϱ�
		fArrowAngle = -fArrowAngle;
	}
	else
	{
		// �����±�
		
	}
	int32 iArrowLen   = (int32)(sqrt( pow(iTextHeight/2.0f, 2.0f) + pow((float)iConstArrowLen, 2.0f) ));
	int32 iArrowEndX  = iArrowX - (int32)(iArrowLen * cos( fArrowAngle )); // �Ƿ���Ҫ����+-��������
	int32 iArrowEndY  = iArrowY + (int32)(iArrowLen * sin( fArrowAngle ));
	iArrowTopHalfX    = iArrowX - (int32)(iConstArrowHeaderLen * cos( fArrowAngle - fConstArrowInnerAngle ));
	iArrowBottomHalfX = iArrowX - (int32)(iConstArrowHeaderLen * cos( fArrowAngle + fConstArrowInnerAngle ));
	iArrowTopHalfY    = iArrowY + (int32)(iConstArrowHeaderLen * sin( fArrowAngle - fConstArrowInnerAngle ));
	iArrowBottomHalfY = iArrowY + (int32)(iConstArrowHeaderLen * sin( fArrowAngle + fConstArrowInnerAngle ));
	
	// ˮƽ��ͷ��������ֱ����
	
	int32 iMaxTextWidth = dc.GetTextExtent(StrText).cx + 1;
	CRect RectText(iArrowEndX, (int32)(iArrowY - iTextHeight/2.0f), iArrowEndX + iMaxTextWidth, (int32)(iArrowY + (iTextHeight/2.0f + 0.5f)));
	if ( iArrowEndX < iArrowX )
	{
		// �������
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
		clrLine = m_IoView.GetIoViewColor(ESCKLineKeep);		// ��ƽ��ɫ
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

	// �����黭������ɫ
	ChartCurve.m_clrTitleText = clrLine;
	//
	

	CPen pen,*pOldPen = NULL;
	pen.CreatePen(PS_SOLID, 1, clrLine);
	pOldPen = (CPen*)pDC->SelectObject(&pen);


	//���˵�ͬһ��X�����������ظ��Ĳ��֣�ͳһͬһX�����ڵĵ�Ϊһ��ֱ�ߣ�ͬһX�����ڿ�ʼһ��������һ����һ��Ҫ��¼
	//�������ͬһX���صĵ����4����������н���㡢��ߵ㡢��͵㡢�뿪�� 4��
	//����������㣬��֪����������
	CArray<CPoint, CPoint &>	aDrawPoints;
	aDrawPoints.SetSize(iNodeCount + 4);		//�����2���㣬 TopMost, BottomMost
	CArray<int32, int32>	aTempY;
	CPoint		*pDrawPoints	=	aDrawPoints.GetData();
	int32		iDrawPointsCount	=	0;
	CPoint		pointLast(-1, -1);
	bool32		bTerminated	=	false;
	CNodeDrawerData	*pLastValidDrawNode = NULL;

	//Ӧ��ͬһX�����4����������ʹ�����ַ���ѹ�����ݣ�������������˼����ٶ� - ����ò���ٶȸ���û�Ż�
	for (int32 i = 0; i < iNodeCount; i++)
	{
		CNodeDrawerData	*pNode	=	pNodeDrawerDatas+i;

		if (CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid))
		{
			//ԭ����ʵ����ϣ�����ֶϵ㣬�������м����еĴ���㣬�������صĲ���
			//��¼�����ǰ������һ����ȷ�㣬Ȼ��Ƚϴ�������ĵ�һ����ȷ�㣬���X������ͬ������Ըô����
			//���x��ͬ�������ǰ�����еĵ㣨���ߴ洢��Щ���飬�Ժ��ڷ�����ƣ������ô洢������X��ͬ�ұ��ض�����ƣ�

			// Ϊ�˱���Ƿ��ڵ�ض����ݣ���Ҫ�ж��Ƿ��м̳�ǰһ�����ݵı�־
			// ����м̳�ǰһ�����ݵı�־����̳�ǰ�����һ��û�б����������
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
			pLastValidDrawNode = pNode;	// ���һ����Ч�ڵ㱣��
		}
	
		if ( pNode->m_iX != pointLast.x || bTerminated )
		{
			//X�㲻ͬ ����Ҫ���������
			if ( bTerminated )
			{
				if ( iDrawPointsCount > 0 )
				{
					//���ض���,���� -	�����¼�����һ���㲻��pointLast��pointLast���룬���ƣ�������ʵҲû��ϵ������Ҳ������������
					//TODO ����
					pDC->Polyline(pDrawPoints, iDrawPointsCount);
					pPickDC->Polyline(pDrawPoints, iDrawPointsCount);
					
					//��ԭ״̬ - �µõ��������ͺ������µĵ�һ����
					iDrawPointsCount	=	0;	
				}
				pLastValidDrawNode = NULL;	// �����ɺ��û����Ч����
			}
			else
			{
				//û���ضϵĻ���Ҫ����Ч�������㣨Top,bottom,last���������
				//����ǰ��û�н���iDrawPointsCount�Ĳ��������� iDrawPointsCount-1 ָ�������ڵ�
				if ( iDrawPointsCount > 0 )
				{
					//��Ч��pointLast
					//iDrawPointsCount -1ָ����ڵ㣬0 ָ��topMost��1ָ��bottomMost��2ָ����pointLast
					// 
					//aTempY.SetSize( 0 );
					int32	iTempPos	=	iDrawPointsCount;
					if ( pDrawPoints[iDrawPointsCount-1].y != pDrawPoints[iDrawPointsCount].y )
					{
						//������Top������TopMost��TopMost�Ѿ���ֵ������Ҫ�ٸ�ֵ
						iTempPos++;
						//aTempY.Add( pDrawPoints[iDrawPointsCount].y );
					}

					if ( pDrawPoints[iDrawPointsCount-1] != pDrawPoints[iDrawPointsCount+1] )
					{
						//������Bottom������bottomMost����ֵ���п������λ����TopMost��
						pDrawPoints[iTempPos++] = pDrawPoints[iDrawPointsCount+1];
						//aTempY.Add( pDrawPoints[iDrawPointsCount+1].y );
					}

					if ( iTempPos > iDrawPointsCount  && pDrawPoints[iTempPos - 1].y != pointLast.y)
					{
						//����pointLast - �����������������һ��������Ϊ����ֵ
						pDrawPoints[iTempPos++]	=	pointLast;
						//aTempY.Add(pointLast.y);
					}
					else
					{
						//���û��������������֤��pointLast�Ǹ���ڵ�һ���ģ���Ϊǰ��x����ȱȽϹ���
						//�����������������pointLast�Ǻ����һ��ֵ����ȵ�
						//���ǵ�����ڵ� - top - bottom - ���ڵ� ����˳�򣬷���ֻ�ܽ�Լ�㵽������
					}

					iDrawPointsCount	=	iTempPos;

					//��ֵ������ֵ�����û������ֵ����ֻ������һ����ڵ�
// 					for ( int j=0; j < aTempY.GetSize(); j++ ) 
// 					{
// 						pDrawPoints[ iDrawPointsCount++ ].y	= aTempY[j];
// 					}

				}
				
			}

			//�µ��ʼ��
			pDrawPoints[ iDrawPointsCount ].x	=	pNode->m_iX;
			pDrawPoints[ iDrawPointsCount ].y	=	pNode->m_iClose;
			pointLast		=	pDrawPoints[ iDrawPointsCount ];
			pDrawPoints[ iDrawPointsCount+1 ]		=	pDrawPoints[ iDrawPointsCount ];	//topMost
			pDrawPoints[ iDrawPointsCount+2 ]		=	pDrawPoints[ iDrawPointsCount ];	//bottomMost
			iDrawPointsCount++;		//ʵ����Ч��ֻ��һ��
		}
		else
		{
			//X����ͬ - ������ߡ�����ж�
			// iDrawPointsCount�Ѿ�ָ����TopMost, +1ָ����bottomMost
			if ( pDrawPoints[ iDrawPointsCount ].y > pNode->m_iClose )
			{
				//��top�ĵ㻹top
				pDrawPoints[ iDrawPointsCount ].y	=	pNode->m_iClose;
			}
			else if ( pDrawPoints[ iDrawPointsCount+1 ].y < pNode->m_iClose )
			{
				//��bottom�ĵ㻹bottom
				pDrawPoints[ iDrawPointsCount+1 ].y	=	pNode->m_iClose;
			}
			pointLast.y		=	pNode->m_iClose;		//ֻҪ��¼һ��Yֵ�Ϳ����ˣ�X�϶���ͬ
		}

		//ȡ���ضϵ��־
		bTerminated	=	CheckFlag(pNode->m_iFlag, CNodeData::KValueDivide);
		// ��������˽ضϱ�־����Ҫ���ֵ��ضϻ���
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
	// pickDC�е�Pen�ǣ�
	pPickDC->Polyline(pDrawPoints, iDrawPointsCount);

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

	pen.DeleteObject();	
}


// void CChartDrawer::DrawNodesAsCurve(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC, int32 iShowSelectFlagPeriod, IN CChartCurve &ChartCurve, CNodeDrawerData *pNodeDrawerDatas, int32 iNodeCount)
// {
// 	ASSERT(NULL != pNodeDrawerDatas && iNodeCount > 0);
// 	
// 	COLORREF clrLine = 0xffffff;
// 	if (EKDSTrendPrice == m_eChartDrawType)
// 		clrLine = m_IoView.GetIoViewColor(ESCKLineKeep);		// ��ƽ��ɫ
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
// 	// �����黭������ɫ
// 	ChartCurve.m_clrTitleText = clrLine;
// 
// 	//
// 	Color clr(GetRValue(clrLine), GetGValue(clrLine), GetBValue(clrLine));
// 	Pen penNow(clr, 1);
// 
// 	Gdiplus::Graphics graphics(pDC->GetSafeHdc());
// 	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
// 
// 	//���˵�ͬһ��X�����������ظ��Ĳ��֣�ͳһͬһX�����ڵĵ�Ϊһ��ֱ�ߣ�ͬһX�����ڿ�ʼһ��������һ����һ��Ҫ��¼
// 	//�������ͬһX���صĵ����4����������н���㡢��ߵ㡢��͵㡢�뿪�� 4��
// 	//����������㣬��֪����������
// 	CArray<Point, Point &>	aDrawPoints;
// 	aDrawPoints.SetSize(iNodeCount + 4);		//�����2���㣬 TopMost, BottomMost
// 	CArray<int32, int32>	aTempY;
// 	Point		*pDrawPoints	=	aDrawPoints.GetData();
// 	int32		iDrawPointsCount	=	0;
// 	Point		pointLast(-1, -1);
// 	bool32		bTerminated	=	false;
// 	CNodeDrawerData	*pLastValidDrawNode = NULL;
// 
// 	//Ӧ��ͬһX�����4����������ʹ�����ַ���ѹ�����ݣ�������������˼����ٶ� - ����ò���ٶȸ���û�Ż�
// 	for (int32 i = 0; i < iNodeCount; i++)
// 	{
// 		CNodeDrawerData	*pNode	=	pNodeDrawerDatas+i;
// 
// 		if (CheckFlag(pNodeDrawerDatas[i].m_iFlag, CNodeData::KValueInvalid))
// 		{
// 			//ԭ����ʵ����ϣ�����ֶϵ㣬�������м����еĴ���㣬�������صĲ���
// 			//��¼�����ǰ������һ����ȷ�㣬Ȼ��Ƚϴ�������ĵ�һ����ȷ�㣬���X������ͬ������Ըô����
// 			//���x��ͬ�������ǰ�����еĵ㣨���ߴ洢��Щ���飬�Ժ��ڷ�����ƣ������ô洢������X��ͬ�ұ��ض�����ƣ�
// 
// 			// Ϊ�˱���Ƿ��ڵ�ض����ݣ���Ҫ�ж��Ƿ��м̳�ǰһ�����ݵı�־
// 			// ����м̳�ǰһ�����ݵı�־����̳�ǰ�����һ��û�б����������
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
// 			pLastValidDrawNode = pNode;	// ���һ����Ч�ڵ㱣��
// 		}
// 	
// 		if ( pNode->m_iX != pointLast.X || bTerminated )
// 		{
// 			//X�㲻ͬ ����Ҫ���������
// 			if ( bTerminated )
// 			{
// 				if ( iDrawPointsCount > 0 )
// 				{
// 					//���ض���,���� -	�����¼�����һ���㲻��pointLast��pointLast���룬���ƣ�������ʵҲû��ϵ������Ҳ������������
// 					//TODO ����
// 					graphics.DrawCurve(&penNow, pDrawPoints, iDrawPointsCount);
// 					
// 					//��ԭ״̬ - �µõ��������ͺ������µĵ�һ����
// 					iDrawPointsCount	=	0;	
// 				}
// 				pLastValidDrawNode = NULL;	// �����ɺ��û����Ч����
// 			}
// 			else
// 			{
// 				//û���ضϵĻ���Ҫ����Ч�������㣨Top,bottom,last���������
// 				//����ǰ��û�н���iDrawPointsCount�Ĳ��������� iDrawPointsCount-1 ָ�������ڵ�
// 				if ( iDrawPointsCount > 0 )
// 				{
// 					//��Ч��pointLast
// 					//iDrawPointsCount -1ָ����ڵ㣬0 ָ��topMost��1ָ��bottomMost��2ָ����pointLast
// 					// 
// 					//aTempY.SetSize( 0 );
// 					int32	iTempPos	=	iDrawPointsCount;
// 					if ( pDrawPoints[iDrawPointsCount-1].Y != pDrawPoints[iDrawPointsCount].Y )
// 					{
// 						//������Top������TopMost��TopMost�Ѿ���ֵ������Ҫ�ٸ�ֵ
// 						iTempPos++;
// 						//aTempY.Add( pDrawPoints[iDrawPointsCount].y );
// 					}
// 
// 					if ( !pDrawPoints[iDrawPointsCount-1].Equals(pDrawPoints[iDrawPointsCount+1]) )
// 					{
// 						//������Bottom������bottomMost����ֵ���п������λ����TopMost��
// 						pDrawPoints[iTempPos++] = pDrawPoints[iDrawPointsCount+1];
// 						//aTempY.Add( pDrawPoints[iDrawPointsCount+1].y );
// 					}
// 
// 					if ( iTempPos > iDrawPointsCount  && pDrawPoints[iTempPos - 1].Y != pointLast.Y)
// 					{
// 						//����pointLast - �����������������һ��������Ϊ����ֵ
// 						pDrawPoints[iTempPos++]	=	pointLast;
// 						//aTempY.Add(pointLast.y);
// 					}
// 					else
// 					{
// 						//���û��������������֤��pointLast�Ǹ���ڵ�һ���ģ���Ϊǰ��x����ȱȽϹ���
// 						//�����������������pointLast�Ǻ����һ��ֵ����ȵ�
// 						//���ǵ�����ڵ� - top - bottom - ���ڵ� ����˳�򣬷���ֻ�ܽ�Լ�㵽������
// 					}
// 
// 					iDrawPointsCount	=	iTempPos;
// 
// 					//��ֵ������ֵ�����û������ֵ����ֻ������һ����ڵ�
// // 					for ( int j=0; j < aTempY.GetSize(); j++ ) 
// // 					{
// // 						pDrawPoints[ iDrawPointsCount++ ].y	= aTempY[j];
// // 					}
// 
// 				}
// 				
// 			}
// 
// 			//�µ��ʼ��
// 			pDrawPoints[ iDrawPointsCount ].X	=	pNode->m_iX;
// 			pDrawPoints[ iDrawPointsCount ].Y	=	pNode->m_iClose;
// 			pointLast		=	pDrawPoints[ iDrawPointsCount ];
// 			pDrawPoints[ iDrawPointsCount+1 ]		=	pDrawPoints[ iDrawPointsCount ];	//topMost
// 			pDrawPoints[ iDrawPointsCount+2 ]		=	pDrawPoints[ iDrawPointsCount ];	//bottomMost
// 			iDrawPointsCount++;		//ʵ����Ч��ֻ��һ��
// 		}
// 		else
// 		{
// 			//X����ͬ - ������ߡ�����ж�
// 			// iDrawPointsCount�Ѿ�ָ����TopMost, +1ָ����bottomMost
// 			if ( pDrawPoints[ iDrawPointsCount ].Y > pNode->m_iClose )
// 			{
// 				//��top�ĵ㻹top
// 				pDrawPoints[ iDrawPointsCount ].Y	=	pNode->m_iClose;
// 			}
// 			else if ( pDrawPoints[ iDrawPointsCount+1 ].Y < pNode->m_iClose )
// 			{
// 				//��bottom�ĵ㻹bottom
// 				pDrawPoints[ iDrawPointsCount+1 ].Y	=	pNode->m_iClose;
// 			}
// 			pointLast.Y		=	pNode->m_iClose;		//ֻҪ��¼һ��Yֵ�Ϳ����ˣ�X�϶���ͬ
// 		}
// 
// 		//ȡ���ضϵ��־
// 		bTerminated	=	CheckFlag(pNode->m_iFlag, CNodeData::KValueDivide);
// 		// ��������˽ضϱ�־����Ҫ���ֵ��ضϻ���
// 	}
// 
// 	graphics.DrawCurve(&penNow, pDrawPoints, iDrawPointsCount);
// 	
// 	// pickDC�е�Pen�ǣ�
// 	// pPickDC->Polyline(pDrawPoints, iDrawPointsCount);
// 
// 	// ��ѡ�б�־
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

	// ���ֻ��������K�ߣ���ʹ������ѹ�������������ܻ��������Ҫ������������
	CChartCurve *pDependentCurve = ChartCurve.GetChartRegion().GetDependentCurve();
	if ( NULL == pDependentCurve )
	{
		//ASSERT( 0 );
		return;
	}
	
	// �ڻ��ƹ������ҵ������Сֵ��������Ϻ� ���������С
	float fYMin, fYMax;
	int32 iYMinPos, iYMaxPos;
	fYMin = FLT_MAX;
	fYMax = -FLT_MAX;
	iYMinPos = iYMaxPos = -1;

	const bool32 bDrawMinMax = CheckFlag(ChartCurve.m_iFlag, CChartCurve::KDependentCurve);	// ��ͼ����Ҫ�������Сֵ
	
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
	CArray<CNodeDrawerData *, CNodeDrawerData *>	aDrawDataPtrs;	//������������--��Ӧ��
	int32	iNeedDrawNodeCount	=	0;

	iNeedDrawNodeCount	=	CalcNeedCalcNodesOverlapKLine(pNodeDrawerDatas, iNodeCount, ChartCurve, aDrawRect, aDrawDataPtrs);
	ASSERT( iNeedDrawNodeCount > 0 );

	//
	COLORREF* pColorFromView = (COLORREF*)m_aColorsFromView.GetData();
	int32 iSizeColorFromView = m_aColorsFromView.GetSize();
	CPen* pPenColorFromView  = NULL;

	//K����volstick����
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
		// ��ͼ
		CNodeDrawerData	*pNode	=	aDrawDataPtrs[i];
		
		int32 iRiseFallFlag = 0;
		COLORREF clrCur = clrKeep;
		CRect RectKLine	=	aDrawRect[i];
		RectKLine.top	=	pNode->m_iClose;		//Ĭ�����̼۱ȿ��̼۸�
		RectKLine.bottom	=	pNode->m_iOpen;

		//
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
			// �����黭������ɫ: ���߱������ɫ������һ������ɫ��ͬ
			// �Ż����������Ҫע����û��ʵ�ֺ�
			ChartCurve.m_clrTitleText = clrCur;
			//
		}

		// ����
		if (EKDSNormal == m_eChartDrawType || EKDSFillRect == m_eChartDrawType)	// ������
		{
			pDC->_DrawRect(RectKLine);
		
			if (EKDSNormal == m_eChartDrawType && iRiseFallFlag >= 0)	// ��
				;
 			else
 				pDC->_FillSolidRect(RectKLine, clrCur);

			//
			pDC->_DrawLine(pNode->m_iX, pNode->m_iHigh, pNode->m_iX, RectKLine.top);	//�ϰ������
			pDC->_DrawLine(pNode->m_iX, RectKLine.bottom, pNode->m_iX, pNode->m_iLow);	//�°������
		}
		else if (EKDSAmerican == m_eChartDrawType)
		{
			//����
			pDC->_DrawLine(pNode->m_iX, pNode->m_iHigh, pNode->m_iX, pNode->m_iLow);

			//2����
			pDC->_DrawLine(RectKLine.left, pNode->m_iOpen, pNode->m_iX, pNode->m_iOpen);	//��
			pDC->_DrawLine(RectKLine.right, pNode->m_iClose, pNode->m_iX, pNode->m_iClose);	//����
		}
		else if ( EKDSTower == m_eChartDrawType )
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
		else if ( EKDSArbitrageKline == m_eChartDrawType )
		{
			// ����K��: �����̼ۼ۲���̼ۼ۲���ۼ۲���K�ߣ����̼ۼ۲���ڿ��̼ۼ۲���ߣ���֮������
			// ����ֱ�ߣ����۴�һ��3*2Сʵ�ľ���
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

		// ��������
		if ( bDrawMinMax && pNode->m_fHigh >= fYMax )
		{
			// DrawArrowTextInRect(*pDC, RectDraw, pNode->m_iX, pNode->m_iHigh, StrYMax, clrText);
			// �����λ��
			fYMax = pNode->m_fHigh;
			iYMaxPos = i;
		}

		if ( bDrawMinMax && pNode->m_fLow <= fYMin )
		{
			// ����Ϊ��С��
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

	// �������С��־
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