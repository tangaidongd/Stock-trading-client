#include "stdafx.h"
#include "memdc.h"
#include "IoViewManager.h"
#include "IoViewKLine.h"
#include "facescheme.h"
#include "ShareFun.h"
#include "MPIChildFrame.h"
#include "MerchManager.h"
#include "IoViewBase.h"
#include "IoViewChouMa.h"
#include "ColorStep.h"
#include "ChengBenFenBu.h"
#include "IoViewShare.h"
#include "IoViewKLine.h"
#include "ChartRegion.h"
#include "ChartCurve.h"
#include "DlgChouMaSetting.h"
#include "PathFactory.h"
#include "tinyxml.h"
#include "XmlShare.h"
#include "ShareFun.h"
#include "coding.h"
#include <string>
#include <algorithm>
using std::string;

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
CIoViewChouMa::T_CBFBParam CIoViewChouMa::s_cbfbParam;

IMPLEMENT_DYNCREATE(CIoViewChouMa, CIoViewBase)
///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewChouMa, CIoViewBase)
//{{AFX_MSG_MAP(CIoViewChouMa)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////

//
CIoViewChouMa::CIoViewChouMa()
:CIoViewBase()
{
	m_RectTitle.SetRectEmpty();
	m_RectChart.SetRectEmpty();

	m_mapNodes[0];

	m_bNeedCalc = false;

	m_pIoViewKlineFocus = NULL;
	m_RectKlineCurve.SetRectEmpty();
	m_fKlineMaxY = -FLT_MAX;
	m_fKlineMinY = FLT_MAX;
	m_uTimeEnd = 0;

	CRect rcBtn(0, 0, 0, 0);
	m_RectBtns.Add(rcBtn);	// 4按钮 
	m_RectBtns.Add(rcBtn);
	m_RectBtns.Add(rcBtn);
	m_RectBtns.Add(rcBtn);

	m_iCurBtn = 1;	// 红黄图

	m_ImageList.Create(IDB_BITMAP_CHOUMA, 16, 0, ILC_COLOR);

	m_fLastPrice = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////
// 

//lint --e{1540}
CIoViewChouMa::~CIoViewChouMa()
{

}

// OnPaint
void CIoViewChouMa::OnPaint()
{
	CPaintDC dcPaint(this); 

	if ( !IsWindowVisible() )
	{
		return;
	}

	//
	if ( GetParentGGTongViewDragFlag() ||  m_bDelByBiSplitTrack )
	{
		LockRedraw();
		return;
	}

	// 锁定绘图
	UnLockRedraw();

	//DWORD dwTime = timeGetTime();
	
	//
	CRect rectClient;
	GetClientRect(&rectClient);

	COLORREF clrBk = GetIoViewColor(ESCBackground);
	CMemDC dc(&dcPaint, rectClient);
	dc->FillSolidRect(rectClient, clrBk);
	//DrawCorner(dc, rectClient);

	// 标题
	{
		// 名称  [平均图] [红黄] [蓝绿] [设置]
		//if ( m_bActive )
		//{
		//	// 修改，为了画边角，不显示激活点
		//	dc.FillSolidRect(3, 3, 2, 2, GetIoViewColor(ESCVolume));
		//}

		if ( NULL != m_pMerchXml )
		{
			dc->SetBkMode(TRANSPARENT);
			dc->SetTextColor(GetIoViewColor(ESCVolume));
			dc->SelectObject(GetIoViewFontObject(ESFSmall));
			CRect rcTitleText(m_RectTitle);
			rcTitleText.left += 5;
			rcTitleText.top += 2;
			dc->DrawText(m_pMerchXml->m_MerchInfo.m_StrMerchCode, rcTitleText, DT_SINGLELINE |DT_LEFT |DT_TOP );
		}

		ASSERT( m_RectBtns.GetSize() == 4 );
		m_ImageList.SetBkColor(clrBk);
		
		int32 i = 0;
		for ( i=0; i < m_RectBtns.GetSize() ; i++ )
		{
			if ( m_iCurBtn == i )
			{
				CRect rcBtn(m_RectBtns[i]);
				rcBtn.InflateRect(1, 1);
				dc->FillSolidRect(rcBtn, GetIoViewColor((ESCChartAxisLine)));
			}
			CPoint pt = m_RectBtns[i].TopLeft();
			ImageList_DrawEx(m_ImageList.GetSafeHandle(), i, dc->GetSafeHdc(), pt.x, pt.y, m_RectBtns[i].Width(), m_RectBtns[i].Height(),
				clrBk, CLR_DEFAULT, ILD_IMAGE );
		}
			
	}

	if ( m_bNeedCalc )
	{
		CalcShowData();
		if ( m_bNeedCalc )
		{
			return;	// 没有计算结果
		}
	}

	const float fRange = m_fKlineMaxY - m_fKlineMinY;
	if ( fRange <= 0.0f || m_mapNodes.size() <= 0 || m_mapNodes[0].size() <= 0 )
	{
		return;	// 此时不适合绘制
	}

	COLORREF clrYellow = GetIoViewColor(ESCGuideLine2);
	COLORREF clrYellowOrg = RGB(255,255,0);
	COLORREF clrWhite = GetIoViewColor(ESCGuideLine1);
	COLORREF clrRed	   = RGB(225, 0, 0);
	COLORREF clrBlue   = RGB(0, 0, 255);
	COLORREF clrGreen  = RGB(64, 255, 0);

	const ChengBenFenBuNodeMap &mapNodes = m_mapNodes;
	const ChengBenFenBuNodeArray &aNodes = m_mapNodes[0];

	T_CBFBParam paramCBFB;
	GetCBFBParam(paramCBFB);

	float fVolMax = -FLT_MAX;
	float fVolMin = FLT_MAX;
	float fVolTotal = 1e-6;
	float fVolOtherTotal = 0.0f;
	//const int32 iBottom = m_RectChart.bottom;
	const int32 iTop = m_RectChart.top;
	const int32 iLeft = m_RectChart.left;
	const int32 iWith = m_RectChart.Width() - 8;
	float fPriceClose = 0.0f;
	float fWinRateClose = 0.0f;
	float fPriceHalfVol = 0.0f;
	int32 iPriceCloseY = 0;
	int32 iPriceHalfVolX = 0;
	int32 iPriceHalfVol = 0;
	typedef map<int32, float> VMap;
	VMap mapSub;
	{
		for ( ChengBenFenBuNodeMap::const_iterator it=mapNodes.begin(); it != mapNodes.end() ; it++ )
		{
			mapSub[ it->first ] = 0.0;
			ChengBenFenBuNodeArray::const_reverse_iterator itSub = it->second.rbegin();	// 高低
			for ( ; itSub != it->second.rend() ; itSub++ )
			{
				//const T_ChengBenFenBuCalcNode &node = *itSub;
				if ( it->first == 0 )
				{
					// 计算总成交量参数
					if ( itSub->fPrice >= m_fKlineMinY && itSub->fPrice <= m_fKlineMaxY  )
					{
						if ( itSub->fVol > fVolMax )
						{
							fVolMax = itSub->fVol;		// 最大成交量 - 都是属于0的
						}
						if ( itSub->fVol < fVolMin )
						{
							fVolMin = itSub->fVol;
						}
					}
					fVolTotal += itSub->fVol;		// 所有成交量总和 - 都是属于0的
					ASSERT( !_isnan(fVolTotal) && _finite(fVolTotal) && fVolTotal >= 0.0 );
				}
				else
				{
					fVolOtherTotal += itSub->fVol;	// 其它的总量
				}
				mapSub[it->first] += itSub->fVol;		// 每个分的成交量和
			}
		}

		// 查找收盘价获利盘位置
		if ( m_KLines.GetSize() > 0 )
		{
			// 最后一天的收盘价
			int32 iPosEnd = CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(m_KLines, CGmtTime((time_t)m_uTimeEnd));
			if ( iPosEnd < 0 || iPosEnd >= m_KLines.GetSize() )
			{
				iPosEnd = m_KLines.GetUpperBound();
			}
			const CKLine &kline = m_KLines[iPosEnd];
			fPriceClose = kline.m_fPriceClose;
			if ( CReportScheme::IsFuture(GetMerchKind(m_pMerchXml)) )
			{
				fPriceClose = kline.m_fPriceAvg;
			}
			float fVol = 0.0;
			for ( ChengBenFenBuNodeArray::const_iterator itSub = aNodes.begin() ; itSub != aNodes.end(); itSub++ )
			{
				if ( itSub->fPrice <= fPriceClose )
				{
					fVol += itSub->fVol;
				}
				else
				{
					break;
				}
			}
			fWinRateClose = fVol / fVolTotal;
			
			iPriceCloseY = iTop + (int32)((m_fKlineMaxY-fPriceClose)*m_RectChart.Height()/fRange);
		}

		// 查找50%对应获利盘位置
		if ( fVolTotal > 0.0 )
		{
			float fVol = 0.0;
			for ( ChengBenFenBuNodeArray::const_reverse_iterator itSub = aNodes.rbegin() ; itSub != aNodes.rend(); itSub++ )
			{
				fVol += itSub->fVol;
				float fWinRate = fVol / fVolTotal;
				if ( fWinRate >= 0.50 )
				{
					fPriceHalfVol = itSub->fPrice;
					iPriceHalfVol = iTop + (int32)((m_fKlineMaxY-fPriceHalfVol)*m_RectChart.Height()/fRange);
					if ( fVolMax > fVolMin )
					{
						iPriceHalfVolX = iLeft + (int32)(iWith*(itSub->fVol-fVolMin)/(fVolMax-fVolMin));
					}
					else
					{
						iPriceHalfVolX = iLeft + iWith/2;
					}
					break;
				}
			}
		}
		
	}

	// 图形
	// 当前统计用线条表示，如果有红黄或者蓝绿，则叠加他们
	// 然后在绘制收盘价获利盘，50%对应获利盘
	if ( !m_RectChart.IsRectEmpty() )
	{
		CRgn rgnClip;
		rgnClip.CreateRectRgnIndirect(m_RectChart);
		dc->SelectClipRgn(&rgnClip);
		// 先从新的开始画
		if ( fVolMax > fVolMin )	// 不是一个单纯的柱形图
		{	
			CPen penWhite;
			penWhite.CreatePen(PS_SOLID, 0, clrWhite);
			CColorStep step(clrRed, clrYellowOrg, paramCBFB.m_aCBFBRedYellowCycles.size()-2);
			CPen *pPenOld = dc->SelectObject(&penWhite);
			CBrush brhWhite;
			brhWhite.CreateSolidBrush(clrWhite);
			//int32 i = 0;
			switch (m_iCurBtn)
			{
			case 0: // 获利比例图
				{
					DrawBasicNodesChart(dc, aNodes, fVolMin, fVolMax, fPriceClose, iPriceHalfVol, iPriceHalfVolX);
				}
				break;
			case 1:	// 红黄图
				{
					if ( fVolOtherTotal <= 0.0f )
					{
						// 不足则仅绘制基本图形
						DrawBasicNodesChart(dc, aNodes, fVolMin, fVolMax, fPriceClose, iPriceHalfVol, iPriceHalfVolX);
					}
					else
					{
						// 白色的不画区域，仅画线
						DrawBasicNodesChart(dc, aNodes, fVolMin, fVolMax, fPriceClose, iPriceHalfVol, iPriceHalfVolX, false);

						for ( ChengBenFenBuNodeMap::const_iterator it=mapNodes.begin(); it != mapNodes.end() ; it++ )
						{
							CPen penMy;
							CBrush brhMy;
							// 貌似map二叉，小-》大
							
							if ( it->first == 0 )
							{
								dc->SelectObject(&penWhite);
								dc->SelectObject(&brhWhite);
								continue;
							}
							else
							{
								penMy.CreatePen(PS_SOLID, 0, step.GetColor());
								brhMy.CreateSolidBrush(step.NextColor());
								//dc->SelectObject(&penMy);
								dc->SelectObject(&penWhite);
								dc->SelectObject(&brhMy);
							}
							CArray<CPoint, CPoint> aPts;
							aPts.SetSize(it->second.size()+10);
							CPoint *pPts = aPts.GetData();
							pPts[0] = m_RectChart.TopLeft();
							int32 iPtIndex = 1;	// 第一点预留
							ChengBenFenBuNodeArray::const_reverse_iterator itSub = it->second.rbegin();	// 高低
							for ( ; itSub != it->second.rend() ; itSub++ )
							{
								// 开始绘制
								//const T_ChengBenFenBuCalcNode &node = *itSub;
								ASSERT( itSub->fVol >= 0.0 );
								if ( itSub->fPrice >= m_fKlineMinY && itSub->fPrice <= m_fKlineMaxY )
								{
									//pPts[iPtIndex].y = iTop +  (m_fKlineMaxY-itSub->fPrice)*(iBottom-iTop)/fRange;
									pPts[iPtIndex].y = PriceYToClientY(itSub->fPrice);
									pPts[iPtIndex].x = iLeft + (int32)(iWith*(itSub->fVol-fVolMin)/(fVolMax-fVolMin));
									iPtIndex++;
								}
							}
							if ( iPtIndex > 1 )
							{
								// 增加2点+回绕
								pPts[iPtIndex].y = pPts[iPtIndex-1].y;
								pPts[iPtIndex].x = iLeft;		// 回绕到左边
								pPts[0].y = pPts[1].y;		// 第一个点指向左边第一个y
								pPts[++iPtIndex] = pPts[0];	// 绕回左边
								
								iPtIndex++;	// 数目
								
								dc->BeginPath();
								dc->Polyline(pPts, iPtIndex);
								dc->EndPath();
								dc->StrokeAndFillPath();
							}
						}
					}
				}
				break;
			case 2:	// 蓝绿图
				{
					if ( fVolOtherTotal <= 0.0f )
					{
						DrawBasicNodesChart(dc, aNodes, fVolMin, fVolMax, fPriceClose, iPriceHalfVol, iPriceHalfVolX);
					}
					else
					{
						// 先绘制当前成本
						// 白色的不画区域，仅画线
						DrawBasicNodesChart(dc, aNodes, fVolMin, fVolMax, fPriceClose, iPriceHalfVol, iPriceHalfVolX, false);
						bool bRun = false;
						if ( bRun )
						{
							dc->SelectObject(&penWhite);
							dc->SelectObject(&brhWhite);
							
							CArray<CPoint, CPoint> aPts;
							aPts.SetSize(aNodes.size()+10);
							CPoint *pPts = aPts.GetData();
							pPts[0] = m_RectChart.TopLeft();
							int32 iPtIndex = 1;	// 第一点预留
							ChengBenFenBuNodeArray::const_reverse_iterator itSub = aNodes.rbegin();	// 高低
							for ( ; itSub != aNodes.rend() ; itSub++ )
							{
								// 开始绘制
								//const T_ChengBenFenBuCalcNode &node = *itSub;
								ASSERT( itSub->fVol >= 0.0 );
								if ( itSub->fPrice >= m_fKlineMinY && itSub->fPrice <= m_fKlineMaxY )
								{
									//pPts[iPtIndex].y = iTop +  (m_fKlineMaxY-itSub->fPrice)*(iBottom-iTop)/fRange;
									pPts[iPtIndex].y = PriceYToClientY(itSub->fPrice);
									pPts[iPtIndex].x = iLeft + (int32)(iWith*(itSub->fVol-fVolMin)/(fVolMax-fVolMin));
									iPtIndex++;
								}
							}
							if ( iPtIndex > 1 )
							{
								// 增加2点+回绕
								pPts[iPtIndex].y = pPts[iPtIndex-1].y;
								pPts[iPtIndex].x = iLeft;		// 回绕到左边
								pPts[0].y = pPts[1].y;		// 第一个点指向左边第一个y
								pPts[++iPtIndex] = pPts[0];	// 绕回左边
								
								iPtIndex++;	// 数目
								
								dc->BeginPath();
								dc->Polyline(pPts, iPtIndex);
								dc->EndPath();
								dc->StrokeAndFillPath();
							}
						}
						
						
						// 在绘制后续的，大-》小
						step.InitColorRange(clrGreen, clrBlue, paramCBFB.m_aCBFBBlueGCycles.size()-2);
						for ( ChengBenFenBuNodeMap::const_reverse_iterator it=mapNodes.rbegin(); it != mapNodes.rend() ; it++ )
						{
							CPen penMy;
							CBrush brhMy;
							// 貌似map二叉，小-》大
							
							if ( it->first == 0 )
							{
								continue;
							}
							else
							{
								penMy.CreatePen(PS_SOLID, 0, step.GetColor());
								brhMy.CreateSolidBrush(step.NextColor());
								//dc->SelectObject(&penMy);
								dc->SelectObject(&penWhite);
								dc->SelectObject(&brhMy);
							}
							CArray<CPoint, CPoint> aPts;
							aPts.SetSize(it->second.size()+10);
							CPoint *pPts = aPts.GetData();
							pPts[0] = m_RectChart.TopLeft();
							int32 iPtIndex = 1;	// 第一点预留
							ChengBenFenBuNodeArray::const_reverse_iterator itSub = it->second.rbegin();	// 高低
							for ( ; itSub != it->second.rend() ; itSub++ )
							{
								// 开始绘制
								//const T_ChengBenFenBuCalcNode &node = *itSub;
								ASSERT( itSub->fVol >= 0.0 );
								if ( itSub->fPrice >= m_fKlineMinY && itSub->fPrice <= m_fKlineMaxY )
								{
									//pPts[iPtIndex].y = iTop +  (m_fKlineMaxY-itSub->fPrice)*(iBottom-iTop)/fRange;
									pPts[iPtIndex].y = PriceYToClientY(itSub->fPrice);
									pPts[iPtIndex].x = iLeft + (int32)(iWith*(itSub->fVol-fVolMin)/(fVolMax-fVolMin));
									iPtIndex++;
								}
							}
							if ( iPtIndex > 1 )
							{
								// 增加2点+回绕
								pPts[iPtIndex].y = pPts[iPtIndex-1].y;
								pPts[iPtIndex].x = iLeft;		// 回绕到左边
								pPts[0].y = pPts[1].y;		// 第一个点指向左边第一个y
								pPts[++iPtIndex] = pPts[0];	// 绕回左边
								
								iPtIndex++;	// 数目
								
								dc->BeginPath();
								dc->Polyline(pPts, iPtIndex);
								dc->EndPath();
								dc->StrokeAndFillPath();
							}
						}
					}
				}
				break;
			default:
				ASSERT( 0 );
			}
			
			dc->SelectObject(pPenOld);
		}
		else
		{
			//ASSERT( mapNodes[0].size() == 0 );
		}
		dc->SelectClipRgn(NULL);
	}

	{
		// 绘制文字
		// 从右底下开始
		CRect rcText(rectClient);
		rcText.left += 3;
		CColorStep step(clrYellowOrg, clrRed, paramCBFB.m_aCBFBRedYellowCycles.size()-2);
		if ( m_iCurBtn == 2 )
		{
			step.InitColorRange(clrGreen, clrBlue, paramCBFB.m_aCBFBBlueGCycles.size()-2);
		}
		dc->SetBkMode(TRANSPARENT);
		dc->SetTextColor(GetIoViewColor(ESCText));

		dc->SelectObject(GetIoViewFontObject(ESFSmall));
		const CSize sizeText = dc->GetTextExtent(_T("测试高度")) + CSize(0, 3);

		int32 iSaveDec = 2;
		if ( NULL != m_pMerchXml )
		{
			iSaveDec = m_pMerchXml->m_MerchInfo.m_iSaveDec;
		}

		float fTotalVol = fVolTotal;
		
		if ( fTotalVol < 1.0 )
		{
			fTotalVol = 1.0;
		}

		// 成本分布 日期: xxxx/xx/xx
		// 获利比例: |xx.x%| |
		// xx 处获利盘: xx.x% 
		// 平均价格: xx.x
		// 90%成本分布于 xx-xx 集中度 xx
		// 70%成本分布于 xx-xx 集中度 xx

		// 平均成本
		{
			ChengBenFenBuNodeMap::const_iterator it = mapNodes.find(0);
			ASSERT( it != mapNodes.end() );
			CArray<float, float> aPrices;
			CArray<float, float> aJiZhongDu;
			CArray<float, float> aPec;
			aPec.Add(0.7f);
			aPec.Add(0.9f);
			aPrices.SetSize(aPec.GetSize()*2);
			aJiZhongDu.SetSize(aPec.GetSize());
			int32 i;
			for ( i=0; i < aPec.GetSize() ; i++ )
			{
				aPrices[i] = aPrices[i+1] = 0.0;
				aJiZhongDu[i] = 0.0;
			}
			if ( it != mapNodes.end() )
			{
				// 从高价加到低价，加到 (100-n)/2，就是顶价，从低价往高价加，加到(100-n)/2就是低价
				for ( i=0; i < aPec.GetSize() ; i++ )
				{
					float fVol = 0.0;
					float fPec = aPec[i];
					for ( ChengBenFenBuNodeArray::const_reverse_iterator itSub = it->second.rbegin() ; itSub != it->second.rend(); itSub++ )
					{
						fVol += itSub->fVol;
						if ( fVol/fTotalVol >= (1-fPec)/2 )
						{
							aPrices[i*2] = itSub->fPrice;
							break;
						}
					}
					
					fVol = 0.0;
					for ( ChengBenFenBuNodeArray::const_iterator itSub2 = it->second.begin() ; itSub2 != it->second.end(); itSub2++ )
					{
						fVol += itSub2->fVol;
						if ( fVol/fTotalVol >= (1-fPec)/2 )
						{
							aPrices[i*2+1] = itSub2->fPrice;
							break;
						}
					}

					ASSERT( aPrices[i*2] >= aPrices[i*2+1] );
					

					//aJiZhongDu[i] = (aPrices[i*2]-aPrices[i*2+1]) /  (it->second.back().fPrice- it->second.front().fPrice);
					// SCR公式 (A1-A2)/(A1+A2)*100
					aJiZhongDu[i] = (aPrices[i*2]-aPrices[i*2+1]) /  ((aPrices[i*2]+aPrices[i*2+1])) *100;
				}
			}

			dc->SetTextColor(GetIoViewColor(ESCText));
			for ( i = 0 ; i < aPec.GetSize() ; i++ )
			{
				CString Str;
				CString StrFmt;
				StrFmt.Format(_T("%%0.0f%%%%成本%%0.%df-%%0.%df 集中%%0.1f%%%%"), iSaveDec, iSaveDec);
				Str.Format(StrFmt, aPec[i]*100, aPrices[i*2+1], aPrices[i*2], aJiZhongDu[i]);

				dc->DrawText(Str, rcText, DT_LEFT |DT_SINGLELINE |DT_BOTTOM);
				rcText.bottom -= sizeText.cy;
			}

			// 平均价格
			float fPriceAvg = 0.0;
			if ( m_KLines.GetSize() > 0 && it != mapNodes.end() )
			{
				// 获利盘50%的点
				float fVol = 0.0;
				for ( ChengBenFenBuNodeArray::const_reverse_iterator itSub = it->second.rbegin() ; itSub != it->second.rend(); itSub++ )
				{
					fVol += itSub->fVol;
					float fWinRate = fVol / fTotalVol;
					if ( fWinRate >= 0.50 )
					{
						fPriceAvg = itSub->fPrice;
						break;
					}
				}
				
			}
			{
				CString Str;
				CString StrFmt;
				StrFmt.Format(_T("平均成本: %%0.%df"), iSaveDec);
				Str.Format(StrFmt, fPriceAvg);
				
				dc->DrawText(Str, rcText, DT_LEFT |DT_SINGLELINE |DT_BOTTOM);
				rcText.bottom -= sizeText.cy;
			}

			// Y除获利盘 TODO
			float fYWinRate = 0.0f;
			float fYPrice = m_fLastPrice;
			if ( m_KLines.GetSize() > 0 && it != mapNodes.end() )
			{
				// 等于Y值的点的成交
				//const int32 iBottom = m_RectChart.bottom;
				float fVol = 0.0;
				for ( ChengBenFenBuNodeArray::const_iterator itSub = it->second.begin() ; itSub != it->second.end(); itSub++ )
				{
					if ( itSub->fPrice > fYPrice )
					{
						fYWinRate = fVol/fTotalVol;
						//fYPrice = itSub->fPrice;
						break;
					}
					else
					{
						fVol += itSub->fVol;
					}
				}
				
			}
			{
				CString Str;
				CString StrFmt;
				StrFmt.Format(_T("%%0.%df处获利盘: %%0.2f%%%%"), iSaveDec);
				Str.Format(StrFmt, fYPrice, fYWinRate*100);
				
				dc->DrawText(Str, rcText, DT_LEFT |DT_SINGLELINE |DT_BOTTOM);
				rcText.bottom -= sizeText.cy;
			}

			// 获利比例 
			float fWinRate = 0.0;
			if ( m_KLines.GetSize() > 0 && it != mapNodes.end() )
			{
				// 最后一天的收盘价
				fWinRate = fWinRateClose;
			}

			{
				// 获利比例: [___|___xxxx_____]
				CString Str(_T("获利比例: "));
				dc->DrawText(Str, rcText, DT_LEFT |DT_SINGLELINE |DT_BOTTOM);
				

				CSize sizeHL = dc->GetTextExtent(Str);
				CRect rcFrame(rcText);
				rcFrame.top = rcFrame.bottom - sizeText.cy;
				rcFrame.left += sizeHL.cx;
				rcFrame.right -= 10;
				CBrush brh;
				brh.CreateSolidBrush(clrWhite);
				dc->FrameRect(rcFrame, &brh);
				brh.DeleteObject();

				brh.CreateSolidBrush(clrYellow);
				CRect rcTmp(rcFrame);
				rcTmp.right = (int)(rcFrame.left + rcFrame.Width()*fWinRate + 1);
				dc->FrameRect(rcTmp, &brh);

				Str.Format(_T("%0.1f%%"), fWinRate*100);
				dc->DrawText(Str, rcFrame, DT_CENTER |DT_SINGLELINE |DT_VCENTER);

				rcText.bottom -= sizeText.cy;
			}
		}

		// 时间
		{
			// 成本分布, 日期: 2010/01/01
			CTime TimeEnd((time_t)m_uTimeEnd);
			CString Str;
			Str.Format(_T("成本分布, 日期: %04d/%02d/%02d"), TimeEnd.GetYear(), TimeEnd.GetMonth(), TimeEnd.GetDay());
			
			dc->DrawText(Str, rcText, DT_LEFT |DT_SINGLELINE |DT_BOTTOM);
			rcText.bottom -= sizeText.cy;
		}

		// 条状文字
		for ( ChengBenFenBuNodeMap::const_reverse_iterator it=mapNodes.rbegin(); it != mapNodes.rend() && 0 != m_iCurBtn ; it++ )
		{
			if ( it->first == 0 )
			{
				continue;
			}

			CRect rcBk(rcText);
			CSize sizeBk = dc->GetTextExtent(_T(" 999日前比率: 100.0% "));
			sizeBk += CSize(10, 0);
			rcBk.top = rcBk.bottom - sizeText.cy;
			rcBk.top += 1;
			rcBk.bottom -= 1;
			rcBk.right = rcBk.right - 10;
			rcBk.left = rcBk.right - sizeBk.cx;
			dc->FillSolidRect(rcBk, step.NextColor());
			dc->SetBkMode(TRANSPARENT);
			dc->SetTextColor(GetIoViewColor(ESCBackground));
			rcBk.right -= 3;

			// 开始绘制
				CString Str;
				float fVol = mapSub[it->first];
				if ( m_iCurBtn == 2 )
				{
					Str.Format(_T("%d日内成本: %0.2f%%"), it->first,  fVol/fTotalVol *100);
				}
				else
				{
					Str.Format(_T("%d日前成本: %0.2f%%"), it->first,  fVol/fTotalVol *100);
				}
				dc->DrawText(Str, rcBk, DT_RIGHT |DT_BOTTOM |DT_SINGLELINE);
				rcText.bottom -= sizeText.cy;
			
		}
	}

	// TRACE(_T("成本分布图绘制: %d ms\r\n"), timeGetTime() -dwTime);
}

void CIoViewChouMa::DrawMyText( CDC *pDC )
{
	const float fRange = m_fKlineMaxY - m_fKlineMinY;
	if ( fRange <= 0.0f || m_mapNodes.size() <= 0 || m_mapNodes[0].size() <= 0 )
	{
		return;	// 此时不适合绘制
	}

	CRect rectClient;
	GetClientRect(rectClient);

	bool32 bDelDC = false;
	if ( NULL == pDC )
	{
		pDC = new CClientDC(this);
		bDelDC = true;
	}

	COLORREF clrText = GetIoViewColor(ESCText);

	const ChengBenFenBuNodeMap &mapNodes = m_mapNodes;

	T_CBFBParam paramCBFB;
	GetCBFBParam(paramCBFB);

	if ( m_fKlineMaxY - m_fKlineMinY > 0.0 ) 
	{
		// 绘制文字
		// 从右底下开始
		CRect rcText(rectClient);
		rcText.left += 3;
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(clrText);

		pDC->SelectObject(GetIoViewFontObject(ESFSmall));
		const CSize sizeText = pDC->GetTextExtent(_T("测试高度")) + CSize(0, 3);

		int32 iSaveDec = 2;
		if ( NULL != m_pMerchXml )
		{
			iSaveDec = m_pMerchXml->m_MerchInfo.m_iSaveDec;
		}

		float fTotalVol = 1e-6;
		typedef map<int32, float> VMap;
		VMap mapSub;
		{
			for ( ChengBenFenBuNodeMap::const_iterator it=mapNodes.begin(); it != mapNodes.end() ; it++ )
			{
				mapSub[ it->first ] = 0.0;
				ChengBenFenBuNodeArray::const_reverse_iterator itSub = it->second.rbegin();	// 高低
				for ( ; itSub != it->second.rend() ; itSub++ )
				{
					//const T_ChengBenFenBuCalcNode &node = *itSub;
					if ( it->first == 0 )
					{
						fTotalVol += itSub->fVol;
					}
					ASSERT( !_isnan(fTotalVol) && _finite(fTotalVol) && fTotalVol >= 0.0 );
					mapSub[it->first] += itSub->fVol;
				}
			}
		}
		if ( fTotalVol < 1.0 )
		{
			fTotalVol = 1.0;
		}

		// 成本分布 日期: xxxx/xx/xx
		// 获利比例: |xx.x%| |
		// xx 处获利盘: xx.x% 
		// 平均价格: xx.x
		// 90%成本分布于 xx-xx 集中度 xx
		// 70%成本分布于 xx-xx 集中度 xx

		rcText.bottom -= sizeText.cy * 3;
		ChengBenFenBuNodeMap::const_iterator it = mapNodes.find(0);

		// Y除获利盘 TODO
		float fYWinRate = 0.0f;
		float fVol = 0.0;
		float fYPrice = m_fLastPrice;
		if ( m_KLines.GetSize() > 0 && it != mapNodes.end() )
		{
			// 等于Y值的点的成交
			for ( ChengBenFenBuNodeArray::const_iterator itSub = it->second.begin() ; itSub != it->second.end(); itSub++ )
			{
				if ( itSub->fPrice > fYPrice )
				{
					fYWinRate = fVol/fTotalVol;
					break;
				}
				fVol += itSub->fVol;
			}

		}
		{
			CString Str;
			CString StrFmt;
			StrFmt.Format(_T("%%0.%df处获利盘: %%0.2f%%%%"), iSaveDec);
			Str.Format(StrFmt, fYPrice, fYWinRate*100);

			pDC->FillSolidRect(rcText.left, rcText.bottom-sizeText.cy, rcText.Width(), sizeText.cy, GetIoViewColor(ESCBackground));
			pDC->DrawText(Str, rcText, DT_LEFT |DT_SINGLELINE |DT_BOTTOM);
			rcText.bottom -= sizeText.cy;
		}
	}

	if ( bDelDC )
	{
		DEL(pDC);
	}
}

void CIoViewChouMa::OnSize(UINT nType, int cx, int cy) 
{		 
	CIoViewBase::OnSize(nType,cx,cy);

	if ( nType != SIZE_MINIMIZED )
	{
		RecalcLayout();
	}

	if ( nType != SIZE_MINIMIZED && m_pIoViewKlineFocus == NULL )
	{
		// 此时应当找一个存在的给与自己
		InitializeFirstKline();
	}
}

void CIoViewChouMa::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	// 通知视图改变关注的商品
	if (NULL == pMerch || m_pMerchXml == pMerch)
		return;
	
	if (NULL != pMerch)
	{
		// 设置关注的商品信息
		m_aSmartAttendMerchs.RemoveAll();
		
		CSmartAttendMerch SmartAttendMerch;
		SmartAttendMerch.m_pMerch = pMerch;
		SmartAttendMerch.m_iDataServiceTypes = EDSTKLine |EDSTGeneral;
		m_aSmartAttendMerchs.Add(SmartAttendMerch);
	}

	// 修改当前查看的商品
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;

	OnVDataMerchKLineUpdate(pMerch);	// 尝试计算
	Invalidate();
}

//
void CIoViewChouMa::OnVDataForceUpdate()
{
	RequestViewData();	
}

void CIoViewChouMa::OnIoViewFontChanged()
{
	// 设置字体值	
	CIoViewBase::OnIoViewFontChanged();

	RecalcLayout();
}

void CIoViewChouMa::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();
}

void CIoViewChouMa::OnVDataRealtimePriceUpdate(IN CMerch *pMerch)
{

}

bool32 CIoViewChouMa::FromXml(TiXmlElement * pElement)
{
	//
	SetFontsFromXml(pElement);
	SetColorsFromXml(pElement);
	//
	if (NULL == pElement)
		return false;

	// 判读是不是IoView的节点
	const char *pcValue = pElement->Value();
	if (NULL == pcValue || strcmp(GetXmlElementValue(), pcValue) != 0)
		return false;

	// 判断是不是描述自己这个业务视图的节点
	const char *pcAttrValue = pElement->Attribute(GetXmlElementAttrIoViewType());
	if (NULL == pcAttrValue || CIoViewManager::GetIoViewString(this).Compare(CString(pcAttrValue)) != 0)	// 不是描述自己的业务节点
		return false;

	// 读取本业务视图特有的内容
	int32 iMarketId			= -1;
	CString StrMerchCode	= L"";

	pcAttrValue = pElement->Attribute(GetXmlElementAttrMarketId());
	if (NULL != pcAttrValue)
	{
		iMarketId = atoi(pcAttrValue);
	}

	pcAttrValue = pElement->Attribute(GetXmlElementAttrMerchCode());
	if (NULL != pcAttrValue)
	{
		StrMerchCode = pcAttrValue;
	}

	// 
	CMerch *pMerchFound = NULL;
	if (!m_pAbsCenterManager->GetMerchManager().FindMerch(StrMerchCode, iMarketId, pMerchFound))
	{
		pMerchFound = NULL;
	}

	// 商品发生改变
	OnVDataMerchChanged(iMarketId, StrMerchCode, pMerchFound);
	
	{
		InitializeFirstKline();
	}
	return true;
}

CString CIoViewChouMa::ToXml()
{
	CString StrThis;

	CString StrMarketId;
	StrMarketId.Format(L"%d", m_MerchXml.m_iMarketId);

	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" ", 
			CString(GetXmlElementValue()).GetBuffer(), 
			CString(GetXmlElementAttrIoViewType()).GetBuffer(),
			CIoViewManager::GetIoViewString(this).GetBuffer(),
			CString(GetXmlElementAttrShowTabName()).GetBuffer(), 
			m_StrTabShowName.GetBuffer(),
			CString(GetXmlElementAttrMerchCode()).GetBuffer(), 
			m_MerchXml.m_StrMerchCode.GetBuffer(),
			CString(GetXmlElementAttrMarketId()).GetBuffer(),
			StrMarketId.GetBuffer()
			);

	CString StrFace;
	StrFace  = SaveColorsToXml();
	StrFace += SaveFontsToXml();
	//
	StrThis += StrFace;
	//
	StrThis += L">\n";
	//
	//
	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";

	return StrThis;
} 

CString CIoViewChouMa::GetDefaultXML()
{
	CString StrThis;

	// 
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" >\n", 
		CString(GetXmlElementValue()).GetBuffer(), 
		CString(GetXmlElementAttrIoViewType()).GetBuffer(), 
		CIoViewManager::GetIoViewString(this).GetBuffer(),
		CString(GetXmlElementAttrMerchCode()).GetBuffer(), 
		L"",
		CString(GetXmlElementAttrMarketId()).GetBuffer(), 
		L"-1");

	//
	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";

	return StrThis;
}

void CIoViewChouMa::OnIoViewActive()
{
	m_bActive = IsActiveInFrame();

	//SetFocus();
	//OnVDataForceUpdate();
	RequestLastIgnoredReqData();

	SetFocus();

	SetChildFrameTitle();
	RedrawWindow();
}

void CIoViewChouMa::OnIoViewDeactive()
{
	m_bActive = false;
	RedrawWindow();
}

void CIoViewChouMa::OnWeightTypeChange()
{
	// 更新所有数据
	OnVDataMerchKLineUpdate(m_pMerchXml);
}

void CIoViewChouMa::LockRedraw()
{
	if ( m_bLockRedraw )
	{
		return;
	}

	m_bLockRedraw = true;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
}

void CIoViewChouMa::UnLockRedraw()
{
	if ( !m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = false;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
}

	
void CIoViewChouMa::SetChildFrameTitle()
{
	CString StrTitle;
	if (NULL == m_pMerchXml)
	{
		StrTitle = CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;
	}
	else
	{		
		StrTitle = CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;
		StrTitle += L" ";
		StrTitle += m_pMerchXml->m_MerchInfo.m_StrMerchCnName;
	}
	
	CMPIChildFrame * pParentFrame = (CMPIChildFrame *)GetParentFrame();
	if ( NULL != pParentFrame)
	{
		pParentFrame->SetChildFrameTitle(StrTitle);
	}
}

//////////////////////////////////////////////////////////////////////////
void CIoViewChouMa::RequestViewData()
{
	if ( NULL == m_pMerchXml )
	{
		return;
	}
		
	// 仅请求财务数据
	CMmiReqPublicFile	ReqFinance;
	ReqFinance.m_ePublicFileType = EPFTF10;
	ReqFinance.m_iMarketId = m_pMerchXml->m_MerchInfo.m_iMarketId;
	ReqFinance.m_StrMerchCode = m_pMerchXml->m_MerchInfo.m_StrMerchCode;
	DoRequestViewData(ReqFinance);

	// K线数据与k线视图使用同一份，自己并不更新k线数据
}

void CIoViewChouMa::OnVDataPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType)
{
	// ...fangz1117  目前 UpdateMainMerchKLine 中是每次都 bUpdate = false 全部数据重算,
	// 优化以后,要处理除权等数据,不要因为在收盘后没有K 线重算的时机而导致没有更新
	
	if (NULL == pMerch || pMerch != m_pMerchXml)
		return;
	
	// 应当跟关联的kline数据同步的
	if (EPFTF10 != ePublicFileType)	// 仅处理除权数据
		return;
	
	// 更新所有数据
	CalcShowData();
	Invalidate();	// 重算数据
}

void CIoViewChouMa::OnVDataMerchKLineUpdate(IN CMerch *pMerch)
{
	// 不处理
}

void CIoViewChouMa::CalcShowData()
{
	if ( NULL == m_pMerchXml )
	{
		return;
	}

	if ( !IsWindowVisible() )
	{
		m_bNeedCalc = true;	// 现在不可见，可以不用计算，等到重绘时，检查此标志，发现则重计算
		return;
	}

	if ( m_RectChart.Height() <= 0 || m_fKlineMaxY - m_fKlineMinY <= 0.0f )
	{
		// 计算了也无用
		return;
	}
	
	m_bNeedCalc = false;	// 计算过了
	float fCircAsset = 0;
	if ( m_pMerchXml->m_pFinanceData != NULL )
	{
		fCircAsset = m_pMerchXml->m_pFinanceData->m_fCircAsset/100;
	}

	if ( m_KLines.GetSize() <= 0 )
	{
		m_mapNodes.clear();
		m_mapNodes[0];
		return;
	}

	// 找到当前要计算的最后一点
	CKLine *pKline = m_KLines.GetData();
	int32 iKlineCount = m_KLines.GetSize();
	CKLine *pKlineEnd = pKline + iKlineCount-1;
	int32 iPosEnd = CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(m_KLines, CGmtTime((time_t)m_uTimeEnd));
	if ( iPosEnd >= 0 && iPosEnd < iKlineCount )
	{
		pKlineEnd = pKline + iPosEnd;
		iKlineCount = iPosEnd+1;
	}
	else if ( iKlineCount > 0 )
	{
		// 最新的一点
		m_uTimeEnd = pKline[iKlineCount-1].m_TimeCurrent.GetTime();
	}
	// 确定合适的开始位置
	if ( iKlineCount > CHOUMA_MAX_KLINECOUNT )
	{
		iKlineCount = CHOUMA_MAX_KLINECOUNT;
		pKline = pKlineEnd - CHOUMA_MAX_KLINECOUNT +1;
	}
	
	T_ChengBenFenBuCalcParam paramCalc;
	T_CBFBParam paramCBFB;
	GetCBFBParam(paramCBFB);
	paramCalc.eCBFBSuanFa = paramCBFB.m_eChengBenFenBuSuanFa;
	paramCalc.fDefaultTradeRate = paramCBFB.m_fDefaultTradeRate;
	paramCalc.fTradeRateRatio = paramCBFB.m_fTradeRateRatio;
	paramCalc.m_uDivide = paramCBFB.m_uiDivide;
	
	bool32 bCalc = FALSE;
	ChengBenFenBuCycleArray aCycles;
	switch (m_iCurBtn)
	{
	case 0:
		{
			aCycles.push_back(0);	// 只要本身
			bCalc = CalcChengBenFenBuRedY(pKline, iKlineCount, fCircAsset, aCycles, &paramCalc, m_mapNodes);
		}
		break;
	case 1:
		{
			aCycles = paramCBFB.m_aCBFBRedYellowCycles;
			bCalc = CalcChengBenFenBuRedY(pKline, iKlineCount, fCircAsset, aCycles, &paramCalc, m_mapNodes);
		}
		break;
	case 2:
		{
			aCycles = paramCBFB.m_aCBFBBlueGCycles;
			bCalc = CalcChengBenFenBuBlueG(pKline, iKlineCount, fCircAsset, aCycles, &paramCalc, m_mapNodes);
		}
		break;
	}
	ASSERT( bCalc );
}


void CIoViewChouMa::RecalcLayout()
{
	CRect rc;
	GetClientRect(rc);

	CClientDC dc(this);
	CFont *pFontOld = dc.SelectObject(GetIoViewFontObject(ESFSmall));
	const CSize sizeText = dc.GetTextExtent(_T("测试高度")) + CSize(0, 10);
	dc.SelectObject(pFontOld);

	m_RectTitle = rc;
	m_RectTitle.bottom = m_RectTitle.top + max(sizeText.cy, 18);

	CRect rcBtn(m_RectTitle);
	const int32 iBtnWidth = 16;
	rcBtn.InflateRect(0, -(rcBtn.Height()-iBtnWidth)/2);
	rcBtn.right -= iBtnWidth;
	rcBtn.left = rcBtn.right - iBtnWidth;
	for ( int32 i=m_RectBtns.GetUpperBound() ; i >= 0 ; i-- )
	{
		m_RectBtns[i] = rcBtn;
		rcBtn.OffsetRect(-iBtnWidth-1, 0);
	}

	int32 iOldHeight = m_RectChart.Height();
	m_RectChart = rc;
	CRect rcKlineCurve(m_RectKlineCurve);
	ScreenToClient(&rcKlineCurve);

	if ( rcKlineCurve.Height() > 0 && rcKlineCurve.top > 0 &&  rcKlineCurve.top -rc.top <= sizeText.cy*2 )
	{
		// 可以使用k线的y轴坐标
		m_RectChart.top = rcKlineCurve.top;
		m_RectChart.bottom = rcKlineCurve.bottom;
	}
	else
	{
		m_RectChart.top = m_RectTitle.bottom;	// 默认处理
		m_RectChart.bottom = m_RectChart.top + 300;
		//if ( iOldHeight != m_RectChart.Height() )
		//{
		//	CalcShowData();	// 此时计算参数已经变更，重新计算
		//}
	}

	Invalidate();
}

BOOL CIoViewChouMa::PreTranslateMessage( MSG* pMsg )
{
	//if ( pMsg->message == WM_SYSCHAR && NULL != m_pMerchXml )
	//{
	//	if ( pMsg->wParam == _T('+') )
	//	{
	//		return TRUE;
	//	}
	//	else if ( pMsg->wParam == _T('-') )
	//	{
	//		return TRUE;
	//	}
	//}
	return CIoViewBase::PreTranslateMessage(pMsg);
}

void CIoViewChouMa::OnMouseMove( UINT nFlags, CPoint point )
{
	//SetLastFocusPoint(point);

	CIoViewBase::OnMouseMove(nFlags, point);
}

void CIoViewChouMa::OnIoViewKLineActive( CIoViewKLine *pIoViewKline )
{
	m_pIoViewKlineFocus = pIoViewKline;
	// 更新坐标等值
	OnIoViewKLineMainMerchDataUpdate(pIoViewKline);
	OnIoViewKLineYSizeChange(pIoViewKline);
}

void CIoViewChouMa::OnIoViewKLineYSizeChange( CIoViewKLine *pIoViewKline )
{
	// 更新坐标等
	if ( NULL != pIoViewKline && m_pIoViewKlineFocus == pIoViewKline )
	{
		if ( NULL != pIoViewKline->m_pRegionMain )
		{
			CChartCurve *pCurve = pIoViewKline->m_pRegionMain->GetDependentCurve();
			CPriceToAxisYObject AxisObj;
			if ( NULL != pCurve
				&& pCurve->GetAxisYCalcObject(AxisObj) )
			{
				m_AxisYCalcObject = AxisObj;	// 获取计算对象

				m_RectKlineCurve = pIoViewKline->m_pRegionMain->GetRectCurves();
				//pIoViewKline->MapWindowPoints(this, &m_RectKlineCurve);	// 坐标转换为此坐标, 其实只有Y坐标有用
				pIoViewKline->ClientToScreen(&m_RectKlineCurve);
				// 获取Y轴的坐标的最大最小值
				m_AxisYCalcObject.AxisYToPriceY(m_AxisYCalcObject.GetAxisYMax(), m_fKlineMaxY);
				m_AxisYCalcObject.AxisYToPriceY(m_AxisYCalcObject.GetAxisYMin(), m_fKlineMinY);

				RecalcLayout();	// 重新布局
				CalcShowData();	// 更新数据显示
				//Invalidate();
				RedrawWindow();
			}
			else
			{
				// 没有有效数据，怎么办
				// ASSERT( NULL == pCurve );
			}
		}
	}
	else
	{
		// 不做任何变更
	}
}

void CIoViewChouMa::OnIoViewKLineMainMerchDataUpdate( CIoViewKLine *pIoViewKline )
{
	// 共用相同数据
	if ( NULL != pIoViewKline && pIoViewKline == m_pIoViewKlineFocus && pIoViewKline->m_MerchParamArray.GetSize() > 0 )
	{
		T_MerchNodeUserData *pData = pIoViewKline->m_MerchParamArray[0];
		if ( NULL != pData )
		{
			// 找到显示数据, 拷贝显示k线数据
			m_KLines.Copy( pData->m_aKLinesFull );

			// 此时不计算, 等待后续的变更, K线变更后必定引发重绘，导致x轴与y轴的变更(有可能不足以引发这个变更)
			m_bNeedCalc = true;
			Invalidate();	// 设置需要重绘的标志, 如果k线不足以引发其它的变更，则总会更新值
		}
	}
}

void CIoViewChouMa::OnIoViewKLineXAxisMayChange( CIoViewKLine *pIoViewKline )
{
	if ( pIoViewKline == m_pIoViewKlineFocus && m_bNeedCalc )
	{
		CalcShowData();		// 重算
		RedrawWindow();
	}
}

void CIoViewChouMa::OnKLineMouseMove( CIoViewKLine *pIoViewKline, float fPrice, UINT uTimeId )
{
	if ( pIoViewKline == m_pIoViewKlineFocus && uTimeId > 0 && uTimeId != m_uTimeEnd )
	{
		// 有效time变更，需要重新计算
		m_fLastPrice = fPrice;
		m_uTimeEnd = uTimeId;
		CalcShowData();
		//Invalidate(TRUE);
		RedrawWindow();
	}
	else if ( pIoViewKline == m_pIoViewKlineFocus )
	{
		// 仅更新下坐标值的获利比率
		SetLastFocusPrice(fPrice);
	}
}

void CIoViewChouMa::GetCBFBParam( OUT T_CBFBParam &param )
{
	static bool bInit = false;
	if ( !bInit )
	{
		// 从xml文件中初始化
		ReadParamFromFile(s_cbfbParam);
		bInit = true;
	}
	param = s_cbfbParam;
}

void CIoViewChouMa::SetCBFBParam( const T_CBFBParam &param )
{
	// TODO
	s_cbfbParam = param;

	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	if ( NULL == pMainFrame )
	{
		return;
	}

	for ( int32 i=0; i < pMainFrame->m_IoViewsPtr.GetSize() ; i++ )
	{
		CIoViewBase *pIoView = pMainFrame->m_IoViewsPtr[i];
		CIoViewChouMa *pChouMa = DYNAMIC_DOWNCAST(CIoViewChouMa, pIoView);
		if ( NULL != pChouMa )
		{
			pChouMa->OnCBFBParamChanged();	// 通知其变更了
		}
	}

	SaveParamToFile(s_cbfbParam);	// 保存
}

void CIoViewChouMa::OnCBFBParamChanged()
{
	if ( NULL != m_pMerchXml )
	{
		CalcShowData();
		Invalidate();
	}
}

//////////////////////////////////////////////////////////////////////////
// 参数设置与保存
const char *KXMLStrEleParam	= "param";
const char *KXMLStrAttriTradeRateRatio		= "tradeRateRatio";
const char *KXMLStrAttriChengBenFenBuSuanFa = "chengBenFenBuSuanFa";
const char *KXMLStrAttriDefaultTradeRate	= "defaultTradeRate";
const char *KXMLStrAttriDivide				= "divide";
const char *KXMLStrEleCycleArray	= "cycleArray";
const char *KXMLStrAttriCycleArrayId	= "id";
const char *KXMLStrEleCycleArrayCycle	= "cycle";
const char *KXMLStrAttriCycleArrayCycle	= "cycle";

const int   KRedYellowId = 0;	// 红黄的id
const int   KBlueGreenId = 1;	// 蓝绿的id


void CIoViewChouMa::SaveParamToFile( const T_CBFBParam &param )
{
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	CString StrPath = CPathFactory::GetChouMaParamFullName(pDoc->m_pAbsCenterManager->GetUserName());

	USES_CONVERSION;
	char* pszPath = _W2A((CString)StrPath);

	TiXmlDocument tiDoc(pszPath);
	TiXmlElement *pRoot = ConstructGGTongAppXmlDocHeader(tiDoc, "ChouMaFenBu", NULL, NULL, NULL);
	if ( NULL == pRoot )
	{
		return;
	}
	
	TiXmlElement eleRoot(KXMLStrEleParam);
	char buf[100];
	ZeroMemory(buf, sizeof(buf));
	_snprintf(buf, sizeof(buf), "%f", param.m_fTradeRateRatio);
	eleRoot.SetAttribute(KXMLStrAttriTradeRateRatio, buf);
	_snprintf(buf, sizeof(buf), "%d", param.m_eChengBenFenBuSuanFa);
	eleRoot.SetAttribute(KXMLStrAttriChengBenFenBuSuanFa, buf);
	_snprintf(buf, sizeof(buf), "%d", param.m_uiDivide);
	eleRoot.SetAttribute(KXMLStrAttriDivide, buf);
	_snprintf(buf, sizeof(buf), "%f", param.m_fDefaultTradeRate);
	eleRoot.SetAttribute(KXMLStrAttriDefaultTradeRate, buf);

	pRoot = (TiXmlElement *)pRoot->InsertEndChild(eleRoot);
	if ( NULL == pRoot )
	{
		return;
	}

	// 数组了
	TiXmlElement eleArray1(KXMLStrEleCycleArray);
	_snprintf(buf, sizeof(buf), "%d", KRedYellowId);
	eleArray1.SetAttribute(KXMLStrAttriCycleArrayId, buf);
	ChengBenFenBuCycleArray::const_iterator it;
	for ( it = param.m_aCBFBRedYellowCycles.begin(); it != param.m_aCBFBRedYellowCycles.end() ; it++ )
	{
		TiXmlElement eleCycle(KXMLStrEleCycleArrayCycle);
		_snprintf(buf, sizeof(buf), "%d", *it);
		eleCycle.SetAttribute(KXMLStrAttriCycleArrayCycle, buf);
		eleArray1.InsertEndChild(eleCycle);
	}
	pRoot->InsertEndChild(eleArray1);

	TiXmlElement eleArray2(KXMLStrEleCycleArray);
	_snprintf(buf, sizeof(buf), "%d", KBlueGreenId);
	eleArray2.SetAttribute(KXMLStrAttriCycleArrayId, buf);
	for ( it = param.m_aCBFBBlueGCycles.begin(); it != param.m_aCBFBBlueGCycles.end() ; it++ )
	{
		TiXmlElement eleCycle(KXMLStrEleCycleArrayCycle);
		_snprintf(buf, sizeof(buf), "%d", *it);
		eleCycle.SetAttribute(KXMLStrAttriCycleArrayCycle, buf);
		eleArray2.InsertEndChild(eleCycle);
	}
	pRoot->InsertEndChild(eleArray2);

	tiDoc.SaveFile();	// 收工
}

bool32 CIoViewChouMa::ReadParamFromFile( OUT T_CBFBParam &param )
{
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	CString StrPath = CPathFactory::GetChouMaParamFullName(pDoc->m_pAbsCenterManager->GetUserName());

	USES_CONVERSION;
	char* pszPath = _W2A((CString)StrPath);
	TiXmlDocument tiDoc(pszPath);
	if ( !tiDoc.LoadFile() )
	{
		return false;
	}

	TiXmlElement *pRoot = tiDoc.RootElement();
	if ( NULL == pRoot )
	{
		return false;
	}

	pRoot = pRoot->FirstChildElement(KXMLStrEleParam);
	if ( NULL == pRoot )
	{
		return false;
	}

	// 衰减系数
	const char *pAttri = pRoot->Attribute(KXMLStrAttriTradeRateRatio);
	if ( NULL != pAttri )
	{
		param.m_fTradeRateRatio = (float)atof(KXMLStrAttriTradeRateRatio);
		if ( param.m_fTradeRateRatio < CBFB_MIN_TRADERATERADIO || param.m_fTradeRateRatio > CBFB_MAX_TRADERATERADIO )
		{
			param.m_fTradeRateRatio = 1.0;
		}
	}

	// 算法
	pAttri = pRoot->Attribute(KXMLStrAttriChengBenFenBuSuanFa);
	if ( NULL != pAttri )
	{
		param.m_eChengBenFenBuSuanFa = (E_ChengBenFenBuSuanFa)atoi(pAttri);
		if ( /*param.m_eChengBenFenBuSuanFa < ECBFB_PingJun ||*/ param.m_eChengBenFenBuSuanFa > ECBFB_SanJiao )
		{
			param.m_eChengBenFenBuSuanFa = ECBFB_PingJun;
		}
	}

	// 分成的分数
	pAttri = pRoot->Attribute(KXMLStrAttriDivide);
	if ( NULL != pAttri )
	{
		param.m_uiDivide = (E_ChengBenFenBuSuanFa)atoi(pAttri);
		if ( param.m_uiDivide > CBFB_MAX_DIVIDE || param.m_eChengBenFenBuSuanFa <= ECBFB_SanJiao )
		{
			param.m_uiDivide = (CBFB_MIN_DIVIDE+CBFB_MAX_DIVIDE)/2;
		}
	}

	// 默认的无流通股本下的换手率
	pAttri = pRoot->Attribute(KXMLStrAttriDefaultTradeRate);
	if ( NULL != pAttri )
	{
		param.m_fDefaultTradeRate = (float)atof(pAttri);
		if ( param.m_fDefaultTradeRate < 0.0 || param.m_fDefaultTradeRate > 0.999 )
		{
			param.m_fDefaultTradeRate = 0.001f; // 0.1  %
		}
	}

	TiXmlElement *pEleArray = pRoot->FirstChildElement(KXMLStrEleCycleArray);
	while ( NULL != pEleArray )
	{
		TiXmlElement *pEleThis = pEleArray;
		pEleArray = pEleArray->NextSiblingElement(KXMLStrEleCycleArray);

		pAttri = pEleThis->Attribute(KXMLStrAttriCycleArrayId);
		if ( NULL == pAttri )
		{
			continue;
		}
		
		ChengBenFenBuCycleArray *pArray = NULL;
		switch ( atoi(pAttri) )
		{
		case KRedYellowId:
			pArray = &param.m_aCBFBRedYellowCycles;
			break;
		case KBlueGreenId:
			pArray = &param.m_aCBFBBlueGCycles;
			break;
		default:
			NULL;
		}
		
		if ( NULL != pArray )
		{
			pArray->clear();
			pArray->push_back(0);	// 加入一个0
			// 一个一个小的读
			TiXmlElement *pEleInner = pEleThis->FirstChildElement(KXMLStrEleCycleArrayCycle);
			while (NULL != pEleInner)
			{
				// xml等会在做- -
				pAttri = pEleInner->Attribute(KXMLStrAttriCycleArrayCycle);
				if ( NULL != pAttri )
				{
					int32 iId = atoi(pAttri);
					if ( iId > 0 )
					{
						bool32 bInsert = false;
						for ( ChengBenFenBuCycleArray::iterator it = pArray->begin(); it != pArray->end() ; it++ )
						{
							if ( *it > iId )
							{
								pArray->insert(it, iId);
								bInsert = true;
								break;
							}
							else if ( *it == iId )
							{
								bInsert = true;
								break;
							}
						}
						if ( !bInsert )
						{
							pArray->push_back(iId);
						}
					}
				}
				pEleInner = pEleInner->NextSiblingElement(KXMLStrEleCycleArrayCycle);
			}
		}
	}

	return true;
}

int CIoViewChouMa::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	int iRet = CIoViewBase::OnCreate(lpCreateStruct);
	
	// 获得颜色和字体
	InitialIoViewFace(this);

	return iRet;
}

void CIoViewChouMa::OnLButtonDown( UINT nFlags, CPoint point )
{
	ASSERT( m_RectBtns.GetSize() == 4 );
	int32 iCur = -1;
	for ( int32 i=0; i < m_RectBtns.GetSize() ; i++ )
	{
		if ( m_RectBtns[i].PtInRect(point) )
		{
			iCur = i;
			break;
		}
	}

	if ( iCur == m_iCurBtn )
	{
		CIoViewBase::OnLButtonDown(nFlags, point);
		return;
	}

	switch (iCur)
	{
	case 0:	// 白黄线条, 数据都有
		{
			m_iCurBtn = iCur;
			Invalidate(TRUE);
		}
		break;
	case 1:	// 红黄图
	case 2:	// 蓝绿图
		{
			m_iCurBtn = iCur;
			CalcShowData();
			Invalidate();
		}
		break;
	case 3:	// 设置
		{
			CDlgChouMaSetting::ChouMaSetting();
		}
		break;
	}

	CIoViewBase::OnLButtonDown(nFlags, point);
}

void CIoViewChouMa::DrawBasicNodesChart( CDC &dc, const ChengBenFenBuNodeArray &aNodes, float fVolMin, float fVolMax, float fPriceClose, int32 iPriceHalfVol, int32 iPriceHalfVolX, bool32 bUseYellow /*= true*/ )
{
	const float fRange = m_fKlineMaxY - m_fKlineMinY;
	if ( fRange <= 0.0f 
		|| m_mapNodes.size() <= 0 
		|| m_mapNodes[0].size() <= 0
		|| fVolMax < fVolMin )
	{
		return;	// 此时不适合绘制
	}

	//const int32 iBottom = m_RectChart.bottom;
	const int32 iTop = m_RectChart.top;
	const int32 iLeft = m_RectChart.left;
	const int32 iWith = m_RectChart.Width() - 8;

	COLORREF clrYellow = GetIoViewColor(ESCGuideLine2);
	COLORREF clrWhite = GetIoViewColor(ESCGuideLine1);
	CPen penWhite;
	penWhite.CreatePen(PS_SOLID, 0, clrWhite);
	dc.SelectObject(&penWhite);
	CBrush brhWhite;
	brhWhite.CreateSolidBrush(clrWhite);
	//int32 i = 0;
	// 获利比例图
	{
		brhWhite.DeleteObject();
		brhWhite.CreateHatchBrush(HS_HORIZONTAL, clrWhite);
		
		CArray<CPoint, CPoint> aPtWhites, aPtYellows;
		aPtWhites.SetSize(aNodes.size()+10);
		aPtYellows.SetSize(aNodes.size()+10);
		CPoint *pPtWhites = aPtWhites.GetData();
		CPoint *pPtYellows = aPtYellows.GetData();
		pPtWhites[0] = m_RectChart.TopLeft();
		pPtYellows[0] = m_RectChart.TopLeft();
		int32 iPtWhiteIndex = 1;	// 第一点预留
		int32 iPtYellowIndex = 1;
		ChengBenFenBuNodeArray::const_reverse_iterator itSub = aNodes.rbegin();	// 高低
		for ( ; itSub != aNodes.rend() ; itSub++ )
		{
			// 开始绘制
			//const T_ChengBenFenBuCalcNode &node = *itSub;
			ASSERT( itSub->fVol >= 0.0 );
			if ( itSub->fPrice >= m_fKlineMinY && itSub->fPrice <= m_fKlineMaxY )
			{
				if ( !bUseYellow || itSub->fPrice > fPriceClose )
				{
					//pPtWhites[iPtWhiteIndex].y = iTop +  (m_fKlineMaxY-itSub->fPrice)*(iBottom-iTop)/fRange;
					pPtWhites[iPtWhiteIndex].y = PriceYToClientY(itSub->fPrice);
					pPtWhites[iPtWhiteIndex].x = iLeft + (int32)(iWith*(itSub->fVol-fVolMin)/(fVolMax-fVolMin));
					iPtWhiteIndex++;							
				}
				else
				{
					//pPtYellows[iPtYellowIndex].y = iTop +  (m_fKlineMaxY-itSub->fPrice)*(iBottom-iTop)/fRange;
					pPtYellows[iPtYellowIndex].y = PriceYToClientY(itSub->fPrice);
					pPtYellows[iPtYellowIndex].x = iLeft + (int32)(iWith*(itSub->fVol-fVolMin)/(fVolMax-fVolMin));
					iPtYellowIndex++;
				}
			}
		}
		
		if ( iPtWhiteIndex > 1 )
		{
			// 增加2点+回绕
			pPtWhites[iPtWhiteIndex].y = pPtWhites[iPtWhiteIndex-1].y;
			pPtWhites[iPtWhiteIndex].x = iLeft;		// 回绕到左边
			pPtWhites[0].y = pPtWhites[1].y;		// 第一个点指向左边第一个y
			pPtWhites[++iPtWhiteIndex] = pPtWhites[0];	// 绕回左边
			
			iPtWhiteIndex++;	// 数目
			
			dc.BeginPath();
			dc.Polyline(pPtWhites, iPtWhiteIndex);
			dc.EndPath();
			dc.SelectObject(&brhWhite);
			dc.FillPath();
		}
		if ( iPtYellowIndex > 1 )
		{
			brhWhite.DeleteObject();
			brhWhite.CreateHatchBrush(HS_HORIZONTAL, clrYellow);
			// 增加2点+回绕
			pPtYellows[iPtYellowIndex].y = pPtYellows[iPtYellowIndex-1].y;
			pPtYellows[iPtYellowIndex].x = iLeft;		// 回绕到左边
			pPtYellows[0].y = pPtYellows[1].y;		// 第一个点指向左边第一个y
			pPtYellows[++iPtYellowIndex] = pPtYellows[0];	// 绕回左边
			
			iPtYellowIndex++;	// 数目
			
			dc.BeginPath();
			dc.Polyline(pPtYellows, iPtYellowIndex);
			dc.EndPath();
			dc.SelectObject(&brhWhite);
			dc.FillPath();
		}
		
		if ( iPriceHalfVol > iTop )
		{
			// 绘制50%获利线
			CPen pen;
			pen.CreatePen(PS_SOLID, 0, GetIoViewColor(ESCGuideLine3));
			CPen *pTmpPenOld = dc.SelectObject(&pen);
			
			dc.MoveTo(iLeft, iPriceHalfVol);
			dc.LineTo(iPriceHalfVolX, iPriceHalfVol);
			
			dc.SelectObject(pTmpPenOld);
		}
	}
}

int32 CIoViewChouMa::PriceToChouMaY( float fPrice, int32 iDefault/* = 0*/)
{
	const float fRange = m_fKlineMaxY - m_fKlineMinY;
	if ( m_RectChart.Height() <= 0
		|| m_RectKlineCurve.Height() <= 0
		|| fRange <= 0.0f )
	{
		return iDefault;	// 无法转换
	}

	return m_RectChart.top + (int32)((m_fKlineMaxY-fPrice)*m_RectChart.Height()/fRange);
}
// 
// float CIoViewChouMa::KlineYToPrice(int32 iKlineScreenY, float fDefault /*= 0.0f*/ )
// {
// 	const float fRange = m_fKlineMaxY - m_fKlineMinY;
// 	if ( m_RectChart.Height() <= 0
// 		|| m_RectKlineCurve.Height() <= 0
// 		|| fRange <= 0.0f )
// 	{
// 		return fDefault;	// 无法转换
// 	}
// 
// 	return m_fKlineMaxY - (iKlineScreenY-m_RectKlineCurve.top)*fRange/(float)m_RectKlineCurve.Height();
// }

void CIoViewChouMa::OnChouMaNotify( const T_ChouMaNotify &notify )
{
	if ( !IsWindow(notify.pIoViewKLine->GetSafeHwnd()) )
	{
		return;
	}

	// iflag=0 激活通知 
	//   1=Y轴变化 
	//   2=mousemove wparame=makelong(x,y), lparam=timeid 
	//   3=updateMainMerchData
	//	 4=k线x坐标轴可能变化了

	//DWORD dwTime = timeGetTime();
	CMPIChildFrame *pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, notify.pIoViewKLine->GetParentFrame());
	if ( NULL == pFrame )
	{
		return;
	}
	const int32 iGroupId = notify.pIoViewKLine->GetIoViewGroupId();
	if ( iGroupId < 0 )
	{
		return;
	}
	
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	if ( NULL == pMainFrame )
	{
		return;
	}
	
	CArray<CIoViewChouMa *, CIoViewChouMa *> aChouMas;
	for ( int32 i=0; i < pMainFrame->m_IoViewsPtr.GetSize() ; i++ )
	{
		CIoViewChouMa *pChouMa = DYNAMIC_DOWNCAST(CIoViewChouMa, pMainFrame->m_IoViewsPtr[i]);
		if ( NULL != pChouMa)
		{
			int32 iChouMaGroupID = pChouMa->GetIoViewGroupId();

			if((iChouMaGroupID & iGroupId)/*iGroupId == pChouMa->GetIoViewGroupId()*/
				&& pFrame == pChouMa->GetParentFrame() )
			{
				aChouMas.Add(pChouMa);
			}
		}
	}

	switch (notify.iFlag)
	{
	case 0:	// 激活通知
		{
			for ( int32 i=0; i < aChouMas.GetSize() ; i++ )
			{
				aChouMas[i]->OnIoViewKLineActive(notify.pIoViewKLine);
			}
		}
		break;
	case 1:	// Y轴变更通知
		{
			for ( int32 i=0; i < aChouMas.GetSize() ; i++ )
			{
				aChouMas[i]->OnIoViewKLineYSizeChange(notify.pIoViewKLine);
			}
		}
		break;
	case 2:	// 鼠标移动通知 wparame=makelong(x,y), lparam=timeid
		{
			float fPrice = *((float *)(&notify.wParam));
			UINT uTime = (UINT)notify.lParam;
			for ( int32 i=0; i < aChouMas.GetSize() ; i++ )
			{
				aChouMas[i]->OnKLineMouseMove(notify.pIoViewKLine, fPrice, uTime);
			}
		}
		break;
	case 3:
		{
			for ( int32 i=0; i < aChouMas.GetSize() ; i++ )
			{
				aChouMas[i]->OnIoViewKLineMainMerchDataUpdate(notify.pIoViewKLine);
			}
		}
		break;
	case 4:
		{
			for ( int32 i=0; i < aChouMas.GetSize() ; i++ )
			{
				aChouMas[i]->OnIoViewKLineMainMerchDataUpdate(notify.pIoViewKLine);
			}
		}
		break;
	default:
		ASSERT( 0 );
	}
	
	// TRACE(_T("成本分布图通知: %d ms\r\n"), timeGetTime() -dwTime);
}

void CIoViewChouMa::InitializeFirstKline()
{
	if ( NULL != m_pIoViewKlineFocus )
	{
		return;	// 有了就不必了
	}

	// 找到当前同窗口的k线视图，并以该k线视图注册到本视图
	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if ( NULL != pMainFrame )
	{
		CIoViewKLine *pIo = (CIoViewKLine *)pMainFrame->FindIoViewByPicMenuId(ID_PIC_KLINE, this, true, false, true, true, true);
		if ( NULL == pIo )
		{
			// 在IoViewManager中找找
			CIoViewManager *pManager = GetIoViewManager();
			if ( NULL != pManager )
			{
				pIo = (CIoViewKLine *)pManager->FindIoViewByPicMenu(ID_PIC_KLINE);
			}
		}
		if ( NULL != pIo )
		{
			OnIoViewKLineActive(pIo);	// 初始化一个默认的k线视图
		}
	}
}

int32 CIoViewChouMa::PriceYToClientY( float fPriceY )
{
	// 无错误检查
	float fMin, fMax, fCur;
	fMax = m_fKlineMaxY;
	fMin = m_fKlineMinY;
	fCur = fPriceY;
	int32 iY = -1;
	if ( m_AxisYCalcObject.PriceYToRectY(m_RectChart, fPriceY, iY) )
	{
		return iY;
	}

	return m_RectChart.top + (int32)((fMax-fCur)*(m_RectChart.Height())/(fMax-fMin));
}

void CIoViewChouMa::SetLastFocusPrice( float fPrice )
{
	if ( m_fLastPrice != fPrice )
	{
		m_fLastPrice = fPrice;
		DrawMyText(NULL);
	}
}

//////////////////////////////////////////////////////////////////////////
// 参数
CIoViewChouMa::T_CBFBParam::T_CBFBParam()
{
	m_fTradeRateRatio = 1.0;	// 衰减1
	m_eChengBenFenBuSuanFa = ECBFB_PingJun; // 平均分布
	m_fDefaultTradeRate = 0.003f;	// 0.3% 换手

	m_uiDivide	= (CBFB_MIN_DIVIDE+CBFB_MAX_DIVIDE)/2;	// 默认

	m_aCBFBRedYellowCycles.push_back(0);
	m_aCBFBRedYellowCycles.push_back(5);
	m_aCBFBRedYellowCycles.push_back(10);
	m_aCBFBRedYellowCycles.push_back(20);
	m_aCBFBRedYellowCycles.push_back(30);
	m_aCBFBRedYellowCycles.push_back(60);
	m_aCBFBRedYellowCycles.push_back(100);

	m_aCBFBBlueGCycles = m_aCBFBRedYellowCycles;
}
