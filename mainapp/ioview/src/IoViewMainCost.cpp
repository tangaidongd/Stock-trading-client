#include "stdafx.h"
#include "memdc.h"
#include <math.h>
#include "MPIChildFrame.h"
#include "IoViewManager.h"
#include "MerchManager.h"
#include "GridCellSymbol.h"
#include "facescheme.h"
#include "ShareFun.h"
#include "ColorStep.h"
#include "PlugInStruct.h"
#include "IoViewMainCost.h"
#include "PluginFuncRight.h"
#include "ConfigInfo.h"
#include "formulaengine.h"

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
							//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////
static const int32 KTimerIdPushDataReq = 1002;
static const int32 KTimerPeriodPushDataReq = 1000*60*5;	// 请求数据


CPoint			m_PointUlShortTermCost;	   // 超短期成本
CPoint			m_PointShortTermCost;	   // 短期成本
CPoint			m_PointMediumermCost;	   // 中期成本
CPoint			m_PointLongTermCost;	   // 长期成本


#define  FORMULAR_MAIN_COST  L"个股机构成本"
#define  LINE_UL_SHORT_COST		L"超短期"
#define  LINE_SHORT_COST		L"短期"
#define  LINE_MEDIUMERM_COST	L"中期"
#define  LINE_LONG_COST			L"长期"

#define  TITLE_TOP		50;

#define  PAGE_STR_TITLE				 "多空资金"
#define  PAGE_STR_STRATEGY_TITLE    "推荐策略："
#define  PAGE_STR_STRATEGY			"龙战于天"

#define  RGB_COLOR_RED		RGB(0xff,0x38,0x37)
#define  RGB_COLOR_YELLOW	RGB(0xb9,0xfc,0x00)
#define  FONT_SPACING		1.2				// 字体间隔
#define  BIG_ROUND_SIZE     5				// 大圆的大小
#define  SMALL_ROUND_SIZE   2				// 小圆的大小
#define  SMALL_ROUND_GAP	6				// 小圆的间隔

/////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CIoViewMainCost, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewMainCost, CIoViewBase)
	//{{AFX_MSG_MAP(CIoViewMainCost)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewMainCost::CIoViewMainCost()
:CIoViewBase()
{
	m_bActive			= false;
	m_iDataServiceType  = EDSTKLine;	// General为收到除权信号

	m_stMainCostData.fPriceNow        = 16.66;
	m_stMainCostData.fUlShortTermCost =16.20;
	m_stMainCostData.fShortTermCost = 17.52;
	m_stMainCostData.fMediumermCost = 14.20;
	m_stMainCostData.fLongTermCost  = 19.36;

	m_Font14.CreateFont(14, 0,0,0,FW_NORMAL, 0,0,0,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"宋体");
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewMainCost::~CIoViewMainCost()
{
	m_Font14.DeleteObject();
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint

void CIoViewMainCost::OnPaint()
{
	CPaintDC dc(this); // device context for painting	

	if ( GetParentGGTongViewDragFlag() || m_bDelByBiSplitTrack )
	{
		LockRedraw();
		return;
	}
	//
	UnLockRedraw();

	CRect rcClient;
	GetClientRect(rcClient);

	COLORREF clrBk = GetIoViewColor(ESCBackground);
	COLORREF clrVol = GetIoViewColor(ESCVolume);
	COLORREF clrText = GetIoViewColor(ESCText);
	COLORREF clrHighValue = RGB(185,250,0);
	COLORREF clrLowValue =  RGB(255,17,50);
	COLORREF clrTitle = RGB_COLOR_RED;


	dc.SetBkColor(clrBk);
	CMemDC dcMem(&dc, rcClient);

	CBitmap MemBitmap;
	MemBitmap.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height()); 
	CBitmap *pOldBit=dcMem.SelectObject(&MemBitmap);  

	// 激活标志
	dcMem->FillSolidRect(rcClient.left + 1, rcClient.top + 1, rcClient.right - 1, rcClient.bottom -1, clrBk);
	int iSaveDC = dcMem->SaveDC();
	if ( m_bActive )
	{
		//dcMem->FillSolidRect(3, 3, 2, 2, clrVol);		
	}
	dcMem->SetBkMode(TRANSPARENT);
	dcMem->FillSolidRect(rcClient, clrBk);

	//DrawCorner(dcMem, rcClient);

	//// 划线
	//CBrush cbr;
	//CPen cpenbr;
	//cpenbr.CreatePen(PS_SOLID, 1, clrVol); 
	//CPen *pOldPen = dcMem->SelectObject(&cpenbr);
	//cbr.CreateStockObject(NULL_BRUSH); //创建一个空画刷
	//CBrush *pOldBr = dcMem->SelectObject(&cbr);
	//dcMem->RoundRect(rcClient, CPoint(10, 10));
	//cbr.DeleteObject();
	//dcMem->SelectObject(pOldBr);
	//dcMem->SelectObject(pOldPen);

	CFont *pFontNormal = GetIoViewFontObject(ESFNormal);
	CFont *pFontSmall  = GetIoViewFontObject(ESFSmall);
	// 无权限不绘制
	if ( !CPluginFuncRight::Instance().IsUserHasRight(CPluginFuncRight::FuncCapitalFlow, false) )
	{
		dcMem->SelectObject(pFontSmall);
		dcMem->SetTextColor(clrText);
		CString StrRight = CPluginFuncRight::Instance().GetUserRightName(CPluginFuncRight::FuncCapitalFlow);
		CString StrShow = _T("不具备[")+StrRight+_T("]权限, ") + CConfigInfo::Instance()->GetPrompt();
		CRect rcDraw(rcClient);
		rcDraw.top = rcClient.CenterPoint().y;
		rcDraw.top -= 15;
		rcDraw.InflateRect(-2, 0);
		dcMem->DrawText(StrShow, rcDraw, DT_CENTER |DT_WORDBREAK );
	}
	else
	{
		// 标题
		if ( !m_RectTitle.IsRectEmpty() )
		{
			CString StrTitle = CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;
			CFont *pOldPen = dcMem->SelectObject(&m_Font14);
			dcMem->SetTextColor(clrTitle);
			CRect rcTmp(0,0,0,0);
			dcMem->DrawText(StrTitle, m_RectTitle, DT_SINGLELINE |DT_VCENTER |DT_LEFT);
			dcMem->SelectObject(pOldPen);
		}

		DrawLineAndPriceNow(dcMem, clrHighValue, clrLowValue);
		DrawBigRound(dcMem, clrHighValue,  clrLowValue);
		DrawSmallRound(dcMem, clrHighValue,  clrLowValue);
		DrawMainText(dcMem, RGB(185,252,0), RGB(255,71,90));
	}
	
	dc.BitBlt(0,0, rcClient.Width(),rcClient.Height(),&dcMem,0,0,SRCCOPY);  

	//绘图完成后的清理  
	MemBitmap.DeleteObject();  
	dcMem->RestoreDC(iSaveDC);
}

// 画线与当前价
void CIoViewMainCost::DrawLineAndPriceNow(CDC &dc,COLORREF clrHighValue, COLORREF clrLowValue)
{
	int iSaveDC = dc.SaveDC();
	dc.SetBkMode(TRANSPARENT);
	dc.SetBkColor(GetIoViewColor(ESCBackground));

	// 画线
	COLORREF clrVol = GetIoViewColor(ESCVolume);
	CPen penLine(PS_SOLID, 1, clrVol);
	CPen *pOldPen;
	pOldPen = dc.SelectObject(&penLine);
	CPoint pStart(m_RectLine.left, m_RectLine.top);
	CPoint pEnd(m_RectLine.right, m_RectLine.top);
	dc.MoveTo(pStart);
	dc.LineTo(pEnd);
	dc.SelectObject(pOldPen);

	// 当前价;
	CString strPriceNow;
	strPriceNow.Format(_T("%0.2f"), m_stMainCostData.fPriceNow);

	CPen penMain(PS_SOLID, 1, clrHighValue), penOther(PS_SOLID, 1, clrLowValue), penBlack(PS_SOLID, 1, RGB(0,0,0));
	pOldPen = dc.SelectObject(&penMain);
	CFont *pFontSmall = GetIoViewFontObject(ESFSmall);
	CFont *pOldFont;
	pOldFont = dc.SelectObject(pFontSmall);
	CSize sizeWidth = dc.GetTextExtent(strPriceNow);

	CString strPrice;
	CRect rcPrice;
	rcPrice.top = m_RectLine.top;
	rcPrice.bottom = rcPrice.top + sizeWidth.cy + 2*4;
	rcPrice.left = 0;
	rcPrice.right = rcPrice.left + sizeWidth.cx*1.3;

	dc.SetTextColor(clrHighValue);
	if (m_aPointTerm[BIG_ULSHORT_TERM].y > m_RectLine.top)
	{
		rcPrice.OffsetRect(CPoint(0,  - rcPrice.Height()));
		dc.SetTextColor(clrLowValue);
	}
	dc.DrawText(strPriceNow, rcPrice, DT_SINGLELINE |DT_VCENTER |DT_RIGHT);


	CRect rcNowPrice(0,0,0,0);
	dc.DrawText(_T(" 最新价"), rcNowPrice, DT_SINGLELINE |DT_CALCRECT |DT_VCENTER |DT_LEFT);
	rcPrice.left = rcPrice.right;
	rcPrice.right = rcPrice.left + rcNowPrice.Width();
	dc.DrawText(_T(" 最新价"), rcPrice, DT_SINGLELINE  |DT_VCENTER |DT_LEFT);

	dc.SelectObject(pOldFont);
	dc.SelectObject(pOldPen);
}

// 画主文字
void CIoViewMainCost::DrawMainText(CDC &dc,COLORREF clrHighValue, COLORREF clrLowValue)
{
	int iSaveDC = dc.SaveDC();
	dc.SetBkMode(TRANSPARENT);
	dc.SetBkColor(GetIoViewColor(ESCBackground));

	m_RectMainText;
	CSize sizeWidth = dc.GetTextExtent(_T("字"));
	CRect rcTextLeft, rcTextRight;
	// 计算字体中心点
	rcTextLeft.top = m_RectMainText.top;
	rcTextLeft.left = m_RectMainText.left;
	rcTextLeft.right = rcTextLeft.left + m_RectMainText.Width()/5;
	rcTextLeft.bottom = rcTextLeft.top + sizeWidth.cy + 2*4;

	rcTextRight = rcTextLeft;
	rcTextRight.top = rcTextRight.top + (rcTextRight.Height() - sizeWidth.cy)/2;
	rcTextRight.bottom = rcTextRight.top + sizeWidth.cy;
	rcTextRight.left = rcTextLeft.right ;
	rcTextRight.right = rcTextRight.left + m_RectMainText.Width()/5;

	float aMainData[BIG_TERM_COUNT];
	aMainData[BIG_ULSHORT_TERM]		= m_stMainCostData.fUlShortTermCost;
	aMainData[BIG_SHORT_TERM]		= m_stMainCostData.fShortTermCost;
	aMainData[BIG_MEDIUMERM_TERM]	= m_stMainCostData.fMediumermCost;
	aMainData[BIG_LONG_TERM]		= m_stMainCostData.fLongTermCost;

	CPen penMain(PS_SOLID, 1, clrHighValue), penOther(PS_SOLID, 1, clrLowValue), penBlack(PS_SOLID, 1, RGB(0,0,0));
	CBrush brMain(clrHighValue), brOther(clrLowValue);
	CPen   *pOldPen = dc.SelectObject(&penMain);
	CBrush *pOldBrush = dc.SelectObject(&brMain);

	CFont *pFontSmall  = GetIoViewFontObject(ESFSmall);
	CFont *pOldFont = NULL;
	pOldFont = dc.SelectObject(pFontSmall);

	for (int i = BIG_ULSHORT_TERM; i < BIG_TERM_COUNT; ++i)
	{
		CRect rcLeftTemp = rcTextLeft;
		CRect rcRightTemp = rcTextRight;

		CString strCostName;
		CString strCostValue;
		if (BIG_ULSHORT_TERM == i)
		{
			strCostName = _T("超短期");
			
		}
		else if (BIG_SHORT_TERM == i)
		{
			strCostName = _T("短期");
			rcLeftTemp.OffsetRect(CPoint(m_RectMainText.Width()/2, 0));
			rcRightTemp.OffsetRect(CPoint(m_RectMainText.Width()/2, 0));	
		}
		else if (BIG_MEDIUMERM_TERM == i)
		{
			strCostName = _T("中期");
			rcLeftTemp.OffsetRect(CPoint(0, rcLeftTemp.Height()));
			rcRightTemp.OffsetRect(CPoint(0, rcLeftTemp.Height()));	
		}
		else if (BIG_LONG_TERM == i)
		{
			strCostName = _T("长期");
			rcLeftTemp.OffsetRect(CPoint(m_RectMainText.Width()/2, rcLeftTemp.Height()));
			rcRightTemp.OffsetRect(CPoint(m_RectMainText.Width()/2, rcLeftTemp.Height()));
		}

		if (aMainData[i] >= m_stMainCostData.fPriceNow)
		{
			dc.SelectObject(&brMain);
			dc.SelectObject(&penMain);
			dc.SetTextColor(clrHighValue);
		}
		else
		{
			dc.SelectObject(&penOther);
			dc.SelectObject(&brOther);
			dc.SetTextColor(clrLowValue);
		}
		
		dc.DrawText(strCostName, rcLeftTemp, DT_SINGLELINE |DT_VCENTER |DT_CENTER);
		dc.Rectangle(rcRightTemp);

		strCostValue.Format(_T("%0.2f"), aMainData[i]);
		dc.SetTextColor(RGB(0,0,0));
		dc.DrawText(strCostValue, rcRightTemp, DT_SINGLELINE |DT_VCENTER |DT_CENTER);
	}		

	dc.SelectObject(pOldBrush);
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldFont);
}

void CIoViewMainCost::DrawBigRound(CDC &dc,COLORREF clrHighValue, COLORREF clrLowValue)
{
	int iSaveDC = dc.SaveDC();
	dc.SetBkMode(TRANSPARENT);
	dc.SetBkColor(GetIoViewColor(ESCBackground));

	// 画圆
	CPen penMain(PS_SOLID, 1, clrHighValue), penOther(PS_SOLID, 1, clrLowValue);
	CPen *pOldPen;
	CBrush brMain(clrHighValue), brOther(clrLowValue);
	pOldPen = dc.SelectObject(&penMain);
	CBrush *pOldBrush = dc.SelectObject(&brMain);
	for (int i = BIG_ULSHORT_TERM; i < BIG_TERM_COUNT; ++i)
	{
		
		if (m_aPointTerm[i].y <= m_RectLine.bottom)
		{
			CRect rcDot(-BIG_ROUND_SIZE, -BIG_ROUND_SIZE, BIG_ROUND_SIZE, BIG_ROUND_SIZE);
			rcDot.OffsetRect(m_aPointTerm[i]);
			dc. Ellipse(rcDot);
		}
	}

	dc.SelectObject(&penOther);
	dc.SelectObject(&brOther);
	for (int i = BIG_ULSHORT_TERM; i < BIG_TERM_COUNT; ++i)
	{
		if (m_aPointTerm[i].y >  m_RectLine.bottom)
		{
			CRect rcDot(-BIG_ROUND_SIZE, -BIG_ROUND_SIZE, BIG_ROUND_SIZE, BIG_ROUND_SIZE);
			rcDot.OffsetRect(m_aPointTerm[i]);
			dc. Ellipse(rcDot);
		}
	}
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
}

void CIoViewMainCost::DrawSmallRound(CDC &dc,COLORREF clrHighValue, COLORREF clrLowValue)
{
	int iSaveDC = dc.SaveDC();
	dc.SetBkMode(TRANSPARENT);
	dc.SetBkColor(GetIoViewColor(ESCBackground));

	// 画圆
	CPen penMain(PS_SOLID, 1, clrHighValue), penOther(PS_SOLID, 1, clrLowValue);
	CPen *pOldPen;
	CBrush brMain(clrHighValue), brOther(clrLowValue);
	pOldPen = dc.SelectObject(&penMain);
	CBrush *pOldBrush = dc.SelectObject(&brMain);

	CPoint PointStart , PointEnd;
	PointStart.x = m_RectLine.left;
	PointStart.y = m_RectLine.bottom;

	for (int i = BIG_ULSHORT_TERM; i < BIG_TERM_COUNT; ++i)
	{
		PointEnd = m_aPointTerm[i];
		int iSmallRoundSize = 0;
		iSmallRoundSize = (PointEnd.x - PointStart.x)/(SMALL_ROUND_SIZE + SMALL_ROUND_GAP);

		// 正玄定理忘记了，直接用加减法
		float fGapX = 0;
		float fGapY = 0;
		fGapX = (fabsf(PointEnd.x - PointStart.x)) / iSmallRoundSize;
		fGapY = (fabsf(PointEnd.y - PointStart.y)) / iSmallRoundSize;

		float fPosX = PointStart.x;
		float fPosY = PointStart.y;
		for (int j = 0; j < iSmallRoundSize; ++j)
		{
			// 当可提升值小于1的时候，也必须填充,所以再次填充一个浮点型
			// 上升则加，下降则减
			if (PointStart.y > PointEnd.y)
			{
				fPosX += fGapX;
				fPosY -= fGapY;
			}
			else
			{
				fPosX  += fGapX;
				fPosY  += fGapY;
			}

			PointStart.x = fPosX;
			PointStart.y = fPosY;

			// 特殊处理，与X轴想重叠的点，用大点来判断
			if (PointStart.y == m_RectLine.bottom)
			{
				if (PointEnd.y <=  m_RectLine.bottom)
				{
					CRect rcDot(-SMALL_ROUND_SIZE, -SMALL_ROUND_SIZE, SMALL_ROUND_SIZE, SMALL_ROUND_SIZE);
					rcDot.OffsetRect(PointStart);
					dc. Ellipse(rcDot);
				}
			}
			else
			{
				if (PointStart.y <=  m_RectLine.bottom)
				{
					CRect rcDot(-SMALL_ROUND_SIZE, -SMALL_ROUND_SIZE, SMALL_ROUND_SIZE, SMALL_ROUND_SIZE);
					rcDot.OffsetRect(PointStart);
					dc. Ellipse(rcDot);
				}
			}
		}
		PointStart = m_aPointTerm[i];
	}

	dc.SelectObject(&penOther);
	dc.SelectObject(&brOther);
	PointStart.x = m_RectLine.left;
	PointStart.y = m_RectLine.bottom;
	for (int i = BIG_ULSHORT_TERM; i < BIG_TERM_COUNT; ++i)
	{
		PointEnd = m_aPointTerm[i];
		int iSmallRoundSize = 0;
		iSmallRoundSize = (PointEnd.x - PointStart.x)/(SMALL_ROUND_SIZE + SMALL_ROUND_GAP);

		// 正玄定理忘记了，直接用加减法
		float fGapX = 0;
		float fGapY = 0;
		fGapX = (fabsf(PointEnd.x - PointStart.x)) / iSmallRoundSize;
		fGapY = (fabsf(PointEnd.y - PointStart.y)) / iSmallRoundSize;

		float fPosX = PointStart.x;
		float fPosY = PointStart.y;
		for (int j = 0; j < iSmallRoundSize; ++j)
		{
			// 当可提升值小于1的时候，也必须填充,所以再次填充一个浮点型
			// 上升则加，下降则减
			if (PointStart.y > PointEnd.y)
			{
				fPosX += fGapX;
				fPosY -= fGapY;
			}
			else
			{
				fPosX  += fGapX;
				fPosY  += fGapY;
			}

			PointStart.x = fPosX;
			PointStart.y = fPosY;

			// 特殊处理，与X轴想重叠的点，用大点来判断
			if (PointStart.y == m_RectLine.bottom)
			{
				if (PointEnd.y >  m_RectLine.bottom)
				{
					CRect rcDot(-SMALL_ROUND_SIZE, -SMALL_ROUND_SIZE, SMALL_ROUND_SIZE, SMALL_ROUND_SIZE);
					rcDot.OffsetRect(PointStart);
					dc. Ellipse(rcDot);
				}
			}
			else
			{
				if (PointStart.y >  m_RectLine.bottom)
				{
					CRect rcDot(-SMALL_ROUND_SIZE, -SMALL_ROUND_SIZE, SMALL_ROUND_SIZE, SMALL_ROUND_SIZE);
					rcDot.OffsetRect(PointStart);
					dc. Ellipse(rcDot);
				}
			}
		}
		PointStart = m_aPointTerm[i];
	}

	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
}

void CIoViewMainCost::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();

	Invalidate();
}

void CIoViewMainCost::OnVDataMerchKLineUpdate(IN CMerch *pMerch)
{
	if (NULL == pMerch)
	{
		return;
	}
	// 等于当前商品才更新K线等数据
	if (pMerch == m_pMerchXml)
	{
		E_KLineTypeBase    eKLineType			    = EKTBDay;

		int32 iPosFound;
		CMerchKLineNode* pKLineRequest = NULL;
		pMerch->FindMerchKLineNode(eKLineType, iPosFound, pKLineRequest);

		pMerch->m_MerchKLineNodesPtr;
		// 根本找不到K线数据， 那就不需要显示了
		if (NULL != pKLineRequest && 0 != pKLineRequest->m_KLines.GetSize())	
		{
			int iIndex = pKLineRequest->m_KLines.GetSize();
			CArray<CKLine, CKLine> aKlineTemp;
			aKlineTemp.Add(pKLineRequest->m_KLines.GetAt(iIndex - 1));

			// 取大势判研的工作线
			CFormularContent* pContent = CFormulaLib::instance()->GetFomular(FORMULAR_MAIN_COST);
			if ( NULL != pContent )
			{
				float fRed		= 0.0;
				float fGreen	= 0.0;
				float fYelloew  = 0.0;
				T_IndexOutArray* pOut = formula_index(pContent, pKLineRequest->m_KLines);
				if (pOut && pOut->iIndexNum > 0)
				{
					float fUlShortCost	 = 0.0;
					float fShortCost	 = 0.0;
					float fMediumermCost = 0.0;
					float fLongCost		 = 0.0;
					for (int i = 0; i < pOut->iIndexNum; i ++ )
					{
						if (0 == pOut->index[i].StrName.CompareNoCase(LINE_UL_SHORT_COST))
						{
							if (pOut->index[i].iPointNum > 0)
							{
								int32 iPointNum = pOut->index[i].iPointNum;

								m_stMainCostData.fUlShortTermCost  =  pOut->index[i].pPoint[iPointNum - 1];
							}											
						}

						if (0 == pOut->index[i].StrName.CompareNoCase(LINE_SHORT_COST))
						{
							if (pOut->index[i].iPointNum > 0)
							{
								int32 iPointNum = pOut->index[i].iPointNum;

								m_stMainCostData.fShortTermCost  =  pOut->index[i].pPoint[iPointNum - 1];
							}											
						}

						if (0 == pOut->index[i].StrName.CompareNoCase(LINE_MEDIUMERM_COST))
						{
							if (pOut->index[i].iPointNum > 0)
							{
								int32 iPointNum = pOut->index[i].iPointNum;

								m_stMainCostData.fMediumermCost  =  pOut->index[i].pPoint[iPointNum - 1];
							}											
						}


						if (0 == pOut->index[i].StrName.CompareNoCase(LINE_LONG_COST))
						{
							if (pOut->index[i].iPointNum > 0)
							{
								int32 iPointNum = pOut->index[i].iPointNum;

								m_stMainCostData.fLongTermCost  =  pOut->index[i].pPoint[iPointNum - 1];
							}											
						}
					}

					// 所有数据都需要使用 m_pMerchXml的数据
					CRealtimePrice * pRealtimePrice = m_pMerchXml->m_pRealtimePrice;	
					if (NULL != pRealtimePrice)
					{
						m_stMainCostData.fPriceNow = pRealtimePrice->m_fPriceNew;
						if (fabs(m_stMainCostData.fPriceNow) < 1e-6)
						{
							m_stMainCostData.fPriceNow = pRealtimePrice->m_fPricePrevClose;
						}

					}

					// 其实没必要调用这个
					SetSize();
					Invalidate();
					UpdateWindow();
				}
			}
		}		
	}
}

void CIoViewMainCost::OnIoViewFontChanged()
{	
	CIoViewBase::OnIoViewFontChanged();
	// 字体调整 - 视图会变得不协调
	//RecalcRectSize();
	Invalidate();
}

void CIoViewMainCost::LockRedraw()
{
	if ( m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = true;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
}

void CIoViewMainCost::UnLockRedraw()
{
	if ( !m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = false;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
}

int CIoViewMainCost::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	int iRet = CIoViewBase::OnCreate(lpCreateStruct);
	if ( -1 == iRet )
	{
		return -1;
	}

	InitialIoViewFace(this);
	SetTimer(KTimerIdPushDataReq, KTimerPeriodPushDataReq, NULL);

	return iRet;
}

void CIoViewMainCost:: GetMaxGapAndPriceLow(const ST_MAIN_COST_DATA& stMainCostData, float &fMaxGap, float &fPriceLow)
{
	float  fGap = 0; 
	fMaxGap = fabsf(stMainCostData.fUlShortTermCost - stMainCostData.fPriceNow);

	fGap = fabsf(stMainCostData.fShortTermCost - stMainCostData.fPriceNow);
	if (fGap > fMaxGap)
	{
		fMaxGap = fGap;
	}
	fGap = fabsf(stMainCostData.fMediumermCost - stMainCostData.fPriceNow);
	if (fGap > fMaxGap)
	{
		fMaxGap = fGap;
	}
	fGap = fabsf(stMainCostData.fLongTermCost - stMainCostData.fPriceNow);
	if (fGap > fMaxGap)
	{
		fMaxGap = fGap;
	}
	fMaxGap = fMaxGap * 2;


	fPriceLow = stMainCostData.fUlShortTermCost;
	if (stMainCostData.fShortTermCost < fPriceLow)
	{
		fPriceLow = stMainCostData.fShortTermCost;
	}

	if (stMainCostData.fMediumermCost < fPriceLow)
	{
		fPriceLow = stMainCostData.fMediumermCost;
	}

	if (stMainCostData.fLongTermCost < fPriceLow)
	{
		fPriceLow = stMainCostData.fLongTermCost;
	}
}

float CIoViewMainCost::GetHeightPercent(float fPriceNow, float fCalPrice, float fMaxGap)
{
	float fPosIndex = 0;
	float fPrice;
	fPrice = fCalPrice - fPriceNow;

	if (fPrice < 1e-7 && fPrice > -1e-7)
	{
		fPosIndex = 0.5; 
	}
	else if (fPrice > 1e-7)
	{	
		fPrice = fMaxGap/2 - fabsf(fPrice);
		fPosIndex = fPrice / fMaxGap;
	}
	else
	{
		fPrice	= fabsf(fPrice) + fMaxGap/2;
		fPosIndex = fPrice / fMaxGap;
	}

	return fPosIndex;
}

void CIoViewMainCost::SetSize()
{
	CRect rcWindows;
	GetClientRect(&rcWindows);
	m_RectTitle = rcWindows;
	m_RectTitle.InflateRect(-10, -10);	// 空隙

	CClientDC dc(this);

	CString StrTitleWidth   = CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;;

	CSize sizeWidth = dc.GetTextExtent(_T("字"));
	m_RectTitle.right = m_RectTitle.left + sizeWidth.cx* StrTitleWidth.GetLength() *FONT_SPACING;	// 依据字体，固定宽绘制
	m_RectTitle.bottom = m_RectTitle.top + sizeWidth.cy + 2*4;

	// 去掉标题的高度，并且预留一个字的高度
	rcWindows.top  += m_RectTitle.bottom + sizeWidth.cy;

	// 显示文字位置，用正常汉字来算坐标，到时候用小字来填就不用计算间隔
	CFont *pFontNormal = GetIoViewFontObject(ESFNormal);
	CFont *pFontSmall  = GetIoViewFontObject(ESFSmall);
	CFont *pOldFont = NULL;
	pOldFont = dc.SelectObject(pFontNormal);
	CSize sizeWidthNormal = dc.GetTextExtent(_T("字"));
	dc.SelectObject(pFontSmall);
	CSize sizeWidthSmall = dc.GetTextExtent(_T("字"));
	dc.SelectObject(pOldFont);
	m_RectMainText = rcWindows;
	m_RectMainText.bottom = m_RectMainText.bottom - 10;		// 避免顶到底框
	m_RectMainText.top = m_RectMainText.bottom -  sizeWidthNormal.cy *3 + 3*4 ;	// 预留一个大圆的空间

	m_RectMainRound = rcWindows;
	m_RectMainRound.top    = m_RectTitle.bottom + sizeWidth.cy;
	m_RectMainRound.bottom = m_RectMainText.top - 10; 

	int iMainRoundHeight = m_RectMainRound.Height();
	m_RectLine.top = m_RectMainRound.top + iMainRoundHeight/2;
	m_RectLine.left = m_RectMainRound.left + m_RectMainRound.Width()/10;
	m_RectLine.right = m_RectMainRound.right - m_RectMainRound.Width()/20;
	m_RectLine.bottom = m_RectMainRound.top + iMainRoundHeight/2;

	// 因为效果图的原因,第一个点为1/3,第二个点1/2，第三个点2/3，第四个点4/4
	float fMaxGap = 0;
	float fPriceLow = 0;
	float fPos;

	GetMaxGapAndPriceLow(m_stMainCostData, fMaxGap, fPriceLow);
	fPos = GetHeightPercent(m_stMainCostData.fPriceNow, m_stMainCostData.fUlShortTermCost, fMaxGap);
	m_PointUlShortTermCost.x = m_RectMainRound.left + m_RectMainRound.Width()/3;
	m_PointUlShortTermCost.y = m_RectMainRound.top + iMainRoundHeight * fPos;
	m_aPointTerm[BIG_ULSHORT_TERM] = m_PointUlShortTermCost;

	fPos = GetHeightPercent(m_stMainCostData.fPriceNow, m_stMainCostData.fShortTermCost, fMaxGap);
	m_PointShortTermCost.x = m_RectMainRound.left + m_RectMainRound.Width()/2;
	m_PointShortTermCost.y = m_RectMainRound.top + iMainRoundHeight * fPos;
	m_aPointTerm[BIG_SHORT_TERM] = m_PointShortTermCost;

	fPos = GetHeightPercent(m_stMainCostData.fPriceNow, m_stMainCostData.fMediumermCost, fMaxGap);
	m_PointMediumermCost.x = m_RectMainRound.left + m_RectMainRound.Width()/10 * 7;
	m_PointMediumermCost.y = m_RectMainRound.top + iMainRoundHeight * fPos;
	m_aPointTerm[BIG_MEDIUMERM_TERM] = m_PointMediumermCost;

	fPos = GetHeightPercent(m_stMainCostData.fPriceNow, m_stMainCostData.fLongTermCost, fMaxGap);
	m_PointLongTermCost.x = m_RectMainRound.left + m_RectMainRound.Width()/10*9;
	m_PointLongTermCost.y = m_RectMainRound.top + iMainRoundHeight * fPos;
	m_aPointTerm[BIG_LONG_TERM] = m_PointLongTermCost;
}

void CIoViewMainCost::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);
	SetSize();
}

// 通知视图改变关注的商品
void CIoViewMainCost::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch)
		return;
	
	// 当前商品不为空的时候
	if (NULL != pMerch)
	{
		// 修改当前查看的商品
		m_pMerchXml					= pMerch;
		m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
		m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;

		// 设置关注的商品信息
		m_aSmartAttendMerchs.RemoveAll();

		CSmartAttendMerch SmartAttendMerch;
		SmartAttendMerch.m_pMerch = pMerch;
		SmartAttendMerch.m_iDataServiceTypes = EDSTKLine; //EDSTTimeSale | EDSTTick; // 关注的数据类型
		m_aSmartAttendMerchs.Add(SmartAttendMerch);
		
		// 
		OnVDataMerchTimeSalesUpdate(pMerch);	// 有就显示, 无则清空显示
	}
	else
	{
		// zhangbo 20090824 #待补充， 当心商品不存在时，清空当前显示数据
		//...
	}
	
	//InvalidateRect(m_RectTitle);	// 标题部分需要修改，其它无
}

//
void CIoViewMainCost::OnVDataForceUpdate()
{
	RequestViewData();
}

bool32	CIoViewMainCost::IsAttendData(IN CMerch *pMerch, E_DataServiceType eDataServiceType)
{
	if (eDataServiceType & m_iDataServiceType)
	{
		return TRUE;
	}

	return false;
}

void CIoViewMainCost::OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch)
{

}

bool32 CIoViewMainCost::FromXml(TiXmlElement * pElement)
{
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
	SetFontsFromXml(pElement);
	SetColorsFromXml(pElement);

	//
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

	OnVDataForceUpdate();

	return true;
}

CString CIoViewMainCost::ToXml()
{
	CString StrThis;

	CString StrMarketId;
	StrMarketId.Format(L"%d", m_MerchXml.m_iMarketId);

	// 
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" ",/*>\n*/ 
		CString(GetXmlElementValue()), 
		CString(GetXmlElementAttrIoViewType()), 
		CIoViewManager::GetIoViewString(this),
		CString(GetXmlElementAttrMerchCode()),
		m_MerchXml.m_StrMerchCode,
		CString(GetXmlElementAttrMarketId()), 
		StrMarketId);
	//
	CString StrFace;
	StrFace  = SaveColorsToXml();
	StrFace += SaveFontsToXml();
	
	StrThis += StrFace;
	StrThis += L">\n";
	//
	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";

	return StrThis;
}

CString CIoViewMainCost::GetDefaultXML()
{
	CString StrThis;

	// 
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" >\n", 
		CString(GetXmlElementValue()), 
		CString(GetXmlElementAttrIoViewType()), 
		CIoViewManager::GetIoViewString(this),
		CString(GetXmlElementAttrMerchCode()),
		L"",
		CString(GetXmlElementAttrMarketId()), 
		L"-1");

	//
	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";

	return StrThis;
}

void CIoViewMainCost::OnIoViewActive()
{
	SetFocus();
	m_bActive = IsActiveInFrame();

	//OnVDataForceUpdate();
	RequestLastIgnoredReqData();

	SetChildFrameTitle();
	Invalidate();
}

void CIoViewMainCost::OnIoViewDeactive()
{
	m_bActive = false;
	Invalidate();
}

void  CIoViewMainCost::SetChildFrameTitle()
{
	CString StrTitle;
	StrTitle =  CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;

	if (NULL != m_pMerchXml)
	{	
		StrTitle += L" ";
		StrTitle += m_pMerchXml->m_MerchInfo.m_StrMerchCnName;
	}
	
	CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pParentFrame)
	{
		pParentFrame->SetChildFrameTitle(StrTitle);
	}
}

void CIoViewMainCost::RequestViewData()
{
}

BOOL CIoViewMainCost::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;
}

void CIoViewMainCost::OnVDataPluginResp( const CMmiCommBase *pResp )
{
}

void CIoViewMainCost::OnTimer( UINT nIDEvent )
{
	if ( KTimerIdPushDataReq == nIDEvent )
	{
		RequestViewData();
	}
	CIoViewBase::OnTimer(nIDEvent);
}

