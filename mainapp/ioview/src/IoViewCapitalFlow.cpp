#include "stdafx.h"
#include "memdc.h"
#include "MPIChildFrame.h"
#include "IoViewManager.h"
#include "MerchManager.h"
#include "GridCellSymbol.h"
#include "facescheme.h"
#include "ShareFun.h"
#include "ColorStep.h"
#include "PlugInStruct.h"
#include "IoViewCapitalFlow.h"
#include "PluginFuncRight.h"
#include "ConfigInfo.h"

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
							//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////
static const int32 KShowTimeSaleCount			= 10000;
static const int32 KNormalShowTimeSaleCount		= 100;
static const int32 KChartCycleDiameter			= 150;			// 饼图直径不超过
static const int32 KTimerIdPushDataReq			= 1002;
static const int32 KTimerPeriodPushDataReq		= 1000*60*5;	// 请求数据

/////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CIoViewCapitalFlow, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewCapitalFlow, CIoViewBase)
	//{{AFX_MSG_MAP(CIoViewCapitalFlow)
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
CIoViewCapitalFlow::CIoViewCapitalFlow()
:CIoViewBase()
{
	m_bActive = false;

	m_RectTitle.SetRectEmpty();
	m_RectButton.SetRectEmpty();
	m_RectChartRate.SetRectEmpty();
	m_RectChartVol.SetRectEmpty();
	m_RectChartAmount.SetRectEmpty();

	m_eCycle = Cycle_OneDay;
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewCapitalFlow::~CIoViewCapitalFlow()
{
}


///////////////////////////////////////////////////////////////////////////////
// OnPaint

void CIoViewCapitalFlow::OnPaint()
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
	COLORREF clrRise = GetIoViewColor(ESCRise);
	COLORREF clrFall = GetIoViewColor(ESCFall);
	COLORREF clrText = GetIoViewColor(ESCText);
	COLORREF clrAxis = GetIoViewColor(ESCChartAxisLine);

	dc.SetBkColor(clrBk);
	CMemDC dcMem(&dc, rcClient);

	dcMem->FillSolidRect(rcClient, clrBk);

	int iSaveDC = dcMem->SaveDC();

	if ( m_bActive )
	{
		//dcMem->FillSolidRect(3, 3, 2, 2, clrVol);		// 激活标志
	}

	dcMem->SetBkMode(TRANSPARENT);
	dcMem->SetBkColor(clrBk);

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
		CPen penAxis;
		penAxis.CreatePen(PS_SOLID, 0, clrAxis);
		CPen penBk;
		penBk.CreatePen(PS_SOLID, 0, clrBk);
		
		CRect rcDraw(rcClient);
		rcDraw.InflateRect(-2, -2);	// 边界空白
		
		if ( !m_RectTitle.IsRectEmpty() )
		{
			// 标题 商品名
			CString StrTitle = CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;
			dcMem->SelectObject(pFontNormal);
			dcMem->SetTextColor(clrVol);
			CRect rcTmp(0,0,0,0);
			dcMem->DrawText(StrTitle, rcTmp, DT_SINGLELINE |DT_CALCRECT |DT_VCENTER |DT_LEFT);
			CRect rcTitle(m_RectTitle);
			dcMem->DrawText(StrTitle, rcTitle, DT_SINGLELINE |DT_VCENTER |DT_LEFT);
			rcTitle.left += rcTmp.Width();
			
			CString StrMerch;
			if ( NULL != m_pMerchXml )
			{
				// 			StrMerch = m_pMerchXml->m_MerchInfo.m_StrMerchCnName;
				// 			dcMem->SetTextColor(clrText);
				// 			//dcMem->SelectObject(pFontSmall);
				// 			dcMem->DrawText(StrMerch, rcTitle, DT_SINGLELINE |DT_VCENTER |DT_CENTER);
				DrawMerchName(dcMem, m_pMerchXml, rcTitle, false);
			}
		}
		
		// 按钮
		if ( !m_RectButton.IsRectEmpty() )
		{
			CRect rcBtn(m_RectButton);
			rcBtn.right = rcBtn.left + rcBtn.Width()/2;
			COLORREF clr1, clr2;
			clr1 = clr2 = clrText;
			if ( Cycle_OneDay == m_eCycle )
			{
				clr1 = clrVol;
			}
			else
			{
				clr2 = clrVol;
			}
			dcMem->SetTextColor(clr1);
			//dcMem->SelectObject(pFontSmall);
			dcMem->DrawText(_T("一日"), rcBtn, DT_SINGLELINE |DT_VCENTER |DT_CENTER); 
			
			rcBtn.left = rcBtn.right;
			rcBtn.right = m_RectButton.right;
			dcMem->SetTextColor(clr2);
			dcMem->DrawText(_T("五日"), rcBtn, DT_SINGLELINE |DT_VCENTER |DT_CENTER);
		}
		
		dcMem->SelectObject(&penAxis);
		dcMem->MoveTo(rcClient.left, m_RectButton.bottom);
		dcMem->LineTo(rcClient.right, m_RectButton.bottom);
		
		// 饼图
		if ( !m_RectChartRate.IsRectEmpty() )
		{
			float fSell = m_capData.GetSellVol();
			float fBuy  = m_capData.GetBuyVol();
			float fOrgBuy, fOrgSell;
			fOrgBuy = fBuy;
			fOrgSell = fSell;
			float fUnit = 1.0;		// 服务器传过来的数据单位原始
			if ( fSell >= 10000.0 )
			{
				fUnit = 10000.0;
			}
			
			float fTotal = fSell + fBuy;
			if ( fTotal == 0.0 )
			{
				fBuy = fSell = 1.0;		// 画一个对分图
				fTotal = 2.0;
			}
			
			dcMem->SetTextColor(clrText);
			dcMem->SelectObject(pFontNormal);
			CString StrTitle;
			StrTitle.Format(_T("%s买卖对比(%s股)"), GetCycleString().GetBuffer(), fUnit <= 1.0 ? _T(""):_T("万"));
			//CSize sizeText = dcMem->GetTextExtent(StrTitle);
			
			CRect rcTitle(m_RectChartRate);
			rcTitle.bottom = rcTitle.bottom - KChartCycleDiameter;
			//ASSERT( rcTitle.Height() > sizeText.cy );
			
			dcMem->DrawText(StrTitle, rcTitle, DT_CENTER |DT_SINGLELINE |DT_VCENTER);
			
			CRect rcRate(m_RectChartRate);
			rcRate.top = rcTitle.bottom;
			CPoint pt = rcRate.CenterPoint();
			int32 iRadius = KChartCycleDiameter / 2;
			rcRate.SetRect(pt.x - iRadius, pt.y - iRadius, pt.x + iRadius, pt.y + iRadius);
			//rcRate.InflateRect(-40, -40);
			
			CBrush brhFall, brhRise;
			brhFall.CreateSolidBrush(clrFall);
			brhRise.CreateSolidBrush(clrRise);
			dcMem->SelectObject(&penBk);
			dcMem->SelectObject(&brhFall);	// 先绘制整个圆的fall
			dcMem->RoundRect(rcRate, CPoint(rcRate.Width(), rcRate.Height()));
			
			int iSave2 = dcMem->SaveDC();
			CPoint ptCenter = rcRate.CenterPoint();
			dcMem->BeginPath();		// 单独绘制rise
			dcMem->MoveTo(rcRate.right, ptCenter.y);	// 右侧中间开始
			dcMem->AngleArc(ptCenter.x, ptCenter.y, rcRate.Width()/2, 0.0, 360.0 * fBuy/fTotal);
			dcMem->LineTo(ptCenter);
			dcMem->EndPath();
			dcMem->SelectClipPath(RGN_AND);
			dcMem->SelectObject(&brhRise);
			dcMem->RoundRect(rcRate, CPoint(rcRate.Width(), rcRate.Height()));
			dcMem->RestoreDC(iSave2);
			
			// 绘制说明文字
			dcMem->SetTextColor(RGB(255,255,255));
			dcMem->SelectObject(pFontSmall);
			
			CString StrRise, StrFall;
			StrRise.Format(_T("%0.0f(%0.0f%%)"), fOrgBuy/fUnit, fOrgBuy*100/fTotal);
			StrFall.Format(_T("%0.0f(%0.0f%%)"), fOrgSell/fUnit, fOrgSell*100/fTotal);
			CRect rcRiseText, rcFallText;
			rcRiseText.SetRect(ptCenter.x+10, rcRate.top, rcRate.right, ptCenter.y - 4);
			rcFallText.SetRect(ptCenter.x+10, ptCenter.y+4, rcRate.right, rcRate.bottom);
			dcMem->DrawText(StrRise, rcRiseText, DT_SINGLELINE |DT_BOTTOM |DT_LEFT |DT_NOCLIP);
			dcMem->DrawText(StrFall, rcFallText, DT_SINGLELINE |DT_TOP |DT_LEFT |DT_NOCLIP);
		}
		
		// 成交量横图
		if ( !m_RectChartVol.IsRectEmpty() && m_RectChartVol.Height() > 0 )
		{
			CArray<float, float> aVolValues;
			m_capData.GetVolValueArray(aVolValues);
			ASSERT( aVolValues.GetSize() == 8 );
			
			float fMax = 1.0;	// 以最大的作为单位标准
			
			int i=0;
			for ( i=0 ; i < aVolValues.GetSize() ; i++ )
			{
				fMax = MAX(fMax, aVolValues[i]);
			}
			
			float fUnit = 1.0f;
			if ( fMax >= 10000.0 )
			{
				fUnit = 10000.0f;
			}
			if ( fMax == 0.0 )
			{
				fMax = 1.0f;
			}
			
			CStringArray aNames;
			aNames.Add(_T("超大单"));
			aNames.Add(_T("(机构)"));
			aNames.Add(_T("大单"));
			aNames.Add(_T("(大户)"));
			aNames.Add(_T("中单"));
			aNames.Add(_T("(中户)"));
			aNames.Add(_T("小单"));
			aNames.Add(_T("(小户)"));
			ASSERT( aNames.GetSize() == aVolValues.GetSize() );
			
			// 计算区域
			dcMem->SelectObject(pFontSmall);
			CSize sizeName = dcMem->GetTextExtent(_T("最小高度"));
			dcMem->SelectObject(pFontNormal);
			CSize sizeTitle = dcMem->GetTextExtent(_T("最小高度"));
			
			int32 iMinHeight = sizeTitle.cy + 10 + (sizeName.cy+2) * aNames.GetSize();
			int32 iLeftHeight = m_RectChartVol.Height() - iMinHeight;
			
			int32 iSeperatorHeight = iLeftHeight / 5;		// 分为5个空格 标1 图4
			
			int32 iItemHeight = m_RectChartVol.Height() / 14;	// 分为14分 标2 图8 空4
			
			int32 iTitleRealHeight=0, iItemRealHeight=0, iSeperatorRealHeight=0;	// 实际每个高度
			
			if ( (iItemHeight < sizeName.cy+2 || iItemHeight*2 < sizeTitle.cy+10) && iSeperatorHeight > 0 )		// 等分分配方法不适用, 最小的还有空余, 使用最小满足, 压缩空行
			{
				iTitleRealHeight = sizeTitle.cy + 10 + iSeperatorHeight;
				iItemRealHeight = sizeName.cy + 2;
				iSeperatorRealHeight = iSeperatorHeight;
			}
			else
			{	// 过大或者过小都使用等分保持美观
				iTitleRealHeight = iItemHeight*2;
				iItemRealHeight = iItemHeight;
				iSeperatorRealHeight = iItemHeight;
			}
			
			// 标题
			dcMem->SetTextColor(clrText);
			
			CString StrTitle;
			StrTitle.Format(_T("%s成交明细(%s股)"), GetCycleString().GetBuffer(), fUnit <= 1.0 ? _T(""):_T("万"));
			
			CRect rcTitle(m_RectChartVol);
			rcTitle.bottom = rcTitle.top + iTitleRealHeight;
			ASSERT( rcTitle.Height() > 0 );
			dcMem->DrawText(StrTitle, rcTitle, DT_CENTER |DT_SINGLELINE |DT_VCENTER);
			
			CRect rectDraw(m_RectChartVol);
			rectDraw.top = rcTitle.bottom;
			for ( i=0; i < 4 ; i++ )		// 4大行
			{
				CRect rcItem(rectDraw);
				rcItem.bottom = rcItem.top + iItemRealHeight;
				DrawHorizontalVolItem(dcMem, rcItem, aNames[i*2], aVolValues[i*2]/fUnit, fMax/fUnit, clrRise);	// 买量
				
				rcItem.top += iItemRealHeight;
				rcItem.bottom = rcItem.top + iItemRealHeight;
				DrawHorizontalVolItem(dcMem, rcItem, aNames[i*2+1], aVolValues[i*2+1]/fUnit, fMax/fUnit, clrFall);	// 卖
				
				rectDraw.top = rcItem.bottom + iSeperatorRealHeight;	// 跳过一个
			}		
			
		}
		
		// 成交金额竖图
		if ( !m_RectChartAmount.IsRectEmpty() && m_RectChartAmount.Height() > 0 )
		{
			CArray<float, float> aAmountValues;
			m_capData.GetAmoutValueArray(aAmountValues);
			ASSERT( aAmountValues.GetSize() == 8 );
			
			float fMaxBuy = 0.0, fMaxSell = 0.0;	// 以最大的作为单位标准
			
			CArray<float, float> aValues;
			int i=0;
			for ( i=0 ; i < aAmountValues.GetSize() /2 ; i++ )
			{
				aValues.Add( aAmountValues[2*i] - aAmountValues[2*i+1] );	// buy - sell
			}
			
			for ( i=0; i < aValues.GetSize() ; i++ )
			{
				if ( aValues[i] < 0.0 )
				{
					fMaxSell = MIN(fMaxSell, aValues[i]);
				}
				else
				{
					fMaxBuy = MAX(fMaxBuy, aValues[i]);
				}
			}
			
			float fUnit = 1.0f;
			float fMax = MAX(fMaxBuy, -fMaxSell);		// 绝对最大值
			if ( fMax >= 10000.0 )
			{
				fUnit = 10000.0f;
			}
			fMax = fMaxBuy - fMaxSell;		// 总跨度
			if ( fMax == 0.0 )
			{
				ASSERT( fMaxBuy==0.0 && fMaxSell == 0.0 ); // 两个都为0才导致跨度为0
				fMax = 1.0f;
			}
			
			int32 iItemWidth = m_RectChartAmount.Width() / 2;	// 分为8列 有5个空格 4个柱子
			int32 iItemSeperator = iItemWidth / 5;	// 5个空
			iItemWidth /= 4;		// 4个柱子
			
			// 标题
			CString StrTitle;
			StrTitle.Format(_T("%s买卖净额(%s元)"), GetCycleString().GetBuffer(), fUnit <= 1.0 ? _T(""):_T("万"));
			
			CRect rctDraw(m_RectChartAmount);
			CRect rcTitle(rctDraw);
			ASSERT( rcTitle.Height() > 0 );
			dcMem->SetTextColor(clrText);
			dcMem->SelectObject(pFontNormal);
			CSize sizeTitle = dc.GetTextExtent(StrTitle);
			rcTitle.bottom = rcTitle.top + sizeTitle.cy + 6;
			dcMem->DrawText(StrTitle, rcTitle, DT_CENTER |DT_SINGLELINE |DT_VCENTER);
			
			rctDraw.top = rcTitle.bottom + 4;
			
			CStringArray aNames;
			aNames.Add(_T("超大单"));
			aNames.Add(_T("大单"));
			aNames.Add(_T("中单"));
			aNames.Add(_T("小单"));
			ASSERT( aNames.GetSize() == aValues.GetSize() );
			
			// 留点给文字 上&下
			dcMem->SelectObject(pFontSmall);
			CSize sizeText = dcMem->GetTextExtent(_T("123超大单"));
			
			int32 iTextHeightInDraw = 2 * sizeText.cy + 6;
			
			rctDraw.InflateRect(0, -iTextHeightInDraw/2);
			if ( rctDraw.Height() > 0 )	// 必须要能画文字
			{
				int32 iCenter = 0;
				iCenter = (int32)(rctDraw.top + rctDraw.Height() * fMaxBuy / fMax);
				if ( rctDraw.top == iCenter )
				{
					iCenter += 1;	// 多留像素
				}
				
				dcMem->SelectObject(&penAxis);
				dcMem->MoveTo(rctDraw.left, iCenter);
				dcMem->LineTo(rctDraw.right, iCenter);
				
				CRect rcItem(rctDraw);
				for ( i=0; i < aNames.GetSize() && i < aValues.GetSize() ; i++ )
				{
					rcItem.left += iItemSeperator;	// 留出空格
					rcItem.right = rcItem.left + iItemWidth;
					
					float fV = aValues[i];
					COLORREF clr;
					if ( fV < 0.0 )
					{
						fMax = fMaxSell;
						clr = clrFall;
					}
					else
					{
						fMax = fMaxBuy;
						clr = clrRise;
					}
					
					//draw
					DrawVerticalAmountItem(dcMem, rcItem, iCenter, aNames[i], fV/fUnit, fMax/fUnit, clr);
					
					rcItem.left = rcItem.right;
				}
			}
		}
	}

	dcMem->RestoreDC(iSaveDC);
}

void CIoViewCapitalFlow::DrawVerticalAmountItem(CDC &dc, const CRect &rcItem, int32 iCenter, const CString &StrHeader, float fValue, float fMax, COLORREF clr)
{
	int iSaveDC = dc.SaveDC();
	dc.SetBkMode(TRANSPARENT);
	dc.SetBkColor(GetIoViewColor(ESCBackground));
	
	CFont *pFontSmall  = GetIoViewFontObject(ESFSmall);
	
	dc.SelectObject(pFontSmall);
	
	if ( fMax == 0.0 )
	{
		ASSERT( 0.0 == fValue );
		fMax = 1.0f;
	}

	ASSERT( fabsf(fValue) <= fabsf(fMax) );

	CString StrValue;
	StrValue.Format(_T("%0.0f"), (float)(fabsf(fValue)));
	CSize sizeText = dc.GetTextExtent(StrValue);
	int32 iTextHeight = sizeText.cy + 4;

	CRect rcDraw(rcItem);
	CRect rcText(rcItem);

	if ( fValue < 0.0 )
	{
		ASSERT( fMax < 0.0 );	// 卖出，向下
		
		int32 iChartLen = (int32)MAX(1, ((rcItem.bottom - iCenter) * fValue / fMax));

		rcDraw.SetRect(rcItem.left, iCenter+1, rcItem.right, iCenter + iChartLen+1);
		rcText.SetRect(rcItem.left, iCenter - iTextHeight, rcItem.right, iCenter);	// 文字出现在上方
	}
	else
	{
		ASSERT( fMax >=0.0 );	// 买入，向上

		int32 iChartLen = (int32)MAX(1, ((iCenter - rcItem.top) * fValue / fMax));
		rcDraw.SetRect(rcItem.left, iCenter - iChartLen, rcItem.right, iCenter);
		rcText.SetRect(rcItem.left, rcDraw.top - iTextHeight, rcItem.right, rcDraw.top);
	}
	
	CColorStep step;
	step.InitColorRange(clr, RGB(255,255,255), rcDraw.Width()/2);
	for ( int i=rcDraw.left ; i < rcDraw.right ; i++ )
	{
		dc.FillSolidRect(i, rcDraw.top, 1, rcDraw.Height(), step.NextColor());
	}
	
	dc.DrawText(StrValue, rcText, DT_SINGLELINE |DT_CENTER |DT_NOCLIP |DT_VCENTER);

	rcText.SetRect(rcItem.left, rcItem.bottom, rcItem.right, rcItem.bottom + iTextHeight);
	dc.DrawText(StrHeader, rcText, DT_SINGLELINE |DT_CENTER |DT_NOCLIP |DT_VCENTER);

	dc.RestoreDC(iSaveDC);
}

void CIoViewCapitalFlow::DrawHorizontalVolItem(CDC &dc, const CRect &rcItem, const CString &StrHeader, float fValue, float fMax, COLORREF clr)
{	
	int iSaveDC = dc.SaveDC();
	dc.SetBkMode(TRANSPARENT);
	dc.SetBkColor(GetIoViewColor(ESCBackground));
	
	CFont *pFontSmall  = GetIoViewFontObject(ESFSmall);

	dc.SelectObject(pFontSmall);
	
	if ( fMax == 0.0 )
	{
		ASSERT( 0.0 == fValue );
		fMax = 1.0f;
	}
	
	int32 iItemWidth = rcItem.Width() / 4;	// 4列 - 字1 图2 字1	
	int32 iChartLen = 0;
	
	CRect rcDraw(rcItem);
	rcDraw.right = rcDraw.left + iItemWidth - 5;
	dc.DrawText(StrHeader, rcDraw, DT_SINGLELINE |DT_VCENTER |DT_LEFT);	// 超大单..

	rcDraw.left += iItemWidth;
	rcDraw.right = rcItem.right;
	iChartLen = (int32)(iItemWidth*2 * fValue / fMax);
	iChartLen = MAX(iChartLen, 1);

	CRect rcHorz(rcDraw);
	rcHorz.InflateRect(0, -1);
	rcHorz.NormalizeRect();
	CColorStep step;
	step.InitColorRange(clr, RGB(255,255,255), rcHorz.Height()/2);
	for ( int i=rcHorz.top ; i < rcHorz.bottom ; i++ )
	{
		dc.FillSolidRect(rcHorz.left, i, iChartLen, 1, step.NextColor());
	}

	CString StrValue;
	StrValue.Format(_T("%0.0f"), fValue);
	rcDraw.left += iChartLen + 5;
	dc.DrawText(StrValue, rcDraw, DT_SINGLELINE |DT_VCENTER |DT_LEFT);	// 0.00
	
	dc.RestoreDC(iSaveDC);
}

void CIoViewCapitalFlow::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();
	Invalidate();
}

void CIoViewCapitalFlow::OnIoViewFontChanged()
{	
	CIoViewBase::OnIoViewFontChanged();
	// 字体调整 - 视图会变得不协调
	RecalcRectSize();
	Invalidate();
}

void CIoViewCapitalFlow::LockRedraw()
{
	if ( m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = true;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
}

void CIoViewCapitalFlow::UnLockRedraw()
{
	if ( !m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = false;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
}

int CIoViewCapitalFlow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

void CIoViewCapitalFlow::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);
	
	RecalcRectSize();
}

void CIoViewCapitalFlow::RecalcRectSize()
{
	m_RectTitle.SetRectEmpty();
	m_RectButton.SetRectEmpty();
	m_RectChartRate.SetRectEmpty();
	m_RectChartVol.SetRectEmpty();
	m_RectChartAmount.SetRectEmpty();

	if ( !CPluginFuncRight::Instance().IsUserHasRight(CPluginFuncRight::FuncCapitalFlow, false) )
	{
		return;
	}
	
	CRect rcClient;
	GetClientRect(rcClient);
	
	CRect rcDraw(rcClient);
	rcDraw.InflateRect(-2, -2);	// 空隙
	
	if ( rcDraw.Width() <=0 || rcDraw.Height() <= 0 )
	{
		return;
	}

	CClientDC dc(this);
	
	CString StrWidth(_T("资金流向  固定商品标  一日  五日 "));
	CSize sizeWidth = dc.GetTextExtent(StrWidth);
	
	rcDraw.right = rcDraw.left + sizeWidth.cx;		// 依据字体，固定宽绘制
	
	CSize sizeText = dc.GetTextExtent(_T(" 一日 "));		// 以该宽度作为按钮的宽度
	m_RectButton = rcDraw;
	m_RectButton.bottom = m_RectButton.top + sizeText.cy + 2*4;
	m_RectButton.left = m_RectButton.right - 2*sizeText.cx;
	
	if ( m_RectButton.Width() < rcDraw.Width() )
	{
		m_RectTitle.left = rcDraw.left;
		m_RectTitle.right = m_RectButton.left;
		m_RectTitle.top = m_RectButton.top;
		m_RectTitle.bottom = m_RectButton.bottom;
	}
	
	rcDraw.top = m_RectButton.bottom;
	rcDraw.top += 10;	// 分割线下方留一点空格
	
	if ( rcDraw.Height() <= 0 )
	{
		return;
	}
	
	int32 iChartDefautHeight = KChartCycleDiameter + sizeText.cy + 10;		// 默认饼图高度
	
	m_RectChartRate = rcDraw;
	m_RectChartRate.bottom = rcDraw.top + iChartDefautHeight;	// 饼图固定高度
	
	rcDraw.top = m_RectChartRate.bottom;
	if ( rcDraw.Height() <= 0 )
	{
		return;
	}
	
	int32 iChartRealHeight = rcDraw.Height() / 2;		// 实际剩余高度平均 - 给成交量与金额
	iChartRealHeight = MAX(iChartDefautHeight, iChartRealHeight);	// 其高度只要要跟饼图一样高
	
	m_RectChartVol = rcDraw;
	m_RectChartVol.bottom = rcDraw.top + iChartRealHeight;
	
	rcDraw.top = m_RectChartVol.bottom;
	if ( rcDraw.Height() <= 0 )
	{
		return;
	}
	
	m_RectChartAmount = rcDraw;
	//m_RectChartAmount.bottom = MAX(rcDraw.bottom, (rcDraw.top + iChartRealHeight));	// 余下的全部给金额图
}

// 通知视图改变关注的商品
void CIoViewCapitalFlow::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch)
		return;
	
	// 修改当前查看的商品
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
	
	// 当前商品不为空的时候
	if (NULL != pMerch)
	{
		// 设置关注的商品信息
		m_aSmartAttendMerchs.RemoveAll();

		CSmartAttendMerch SmartAttendMerch;
		SmartAttendMerch.m_pMerch = pMerch;
		SmartAttendMerch.m_iDataServiceTypes = EDSTGeneral; //EDSTTimeSale | EDSTTick; // 关注的数据类型
		m_aSmartAttendMerchs.Add(SmartAttendMerch);
		OnVDataMerchTimeSalesUpdate(pMerch);	// 有就显示, 无则清空显示
	}
	else
	{
		// zhangbo 20090824 #待补充， 当心商品不存在时，清空当前显示数据
		//...
	}
	
	InvalidateRect(m_RectTitle);	// 标题部分需要修改，其它无
	// 数据清空
	if ( m_aTickExData.GetSize() > 0 )
	{
		m_aTickExData.RemoveAll();
		UpdateShowData();
		Invalidate();		// 刷新所有显示
	}
	//RequestViewData();  // change时不申请，等待viewdata调用force申请	
}

//
void CIoViewCapitalFlow::OnVDataForceUpdate()
{
	RequestViewData();
}

void CIoViewCapitalFlow::OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch)
{
	
}

bool32 CIoViewCapitalFlow::FromXml(TiXmlElement * pElement)
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

CString CIoViewCapitalFlow::ToXml()
{
	CString StrThis;

	CString StrMarketId;
	StrMarketId.Format(L"%d", m_MerchXml.m_iMarketId);

	// 
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" ",/*>\n*/ 
		CString(GetXmlElementValue()).GetBuffer(), 
		CString(GetXmlElementAttrIoViewType()).GetBuffer(), 
		CIoViewManager::GetIoViewString(this).GetBuffer(),
		CString(GetXmlElementAttrShowTabName()).GetBuffer(), 
		m_StrTabShowName.GetBuffer(),
		CString(GetXmlElementAttrMerchCode()).GetBuffer(),
		m_MerchXml.m_StrMerchCode.GetBuffer(),
		CString(GetXmlElementAttrMarketId()).GetBuffer(), 
		StrMarketId.GetBuffer());
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

CString CIoViewCapitalFlow::GetDefaultXML()
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


void CIoViewCapitalFlow::OnIoViewActive()
{
	SetFocus();
	m_bActive = IsActiveInFrame();

	//OnVDataForceUpdate();
	RequestLastIgnoredReqData();

	SetChildFrameTitle();
	Invalidate();
}

void CIoViewCapitalFlow::OnIoViewDeactive()
{
	m_bActive = false;

	Invalidate();
}

void  CIoViewCapitalFlow::SetChildFrameTitle()
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

void CIoViewCapitalFlow::RequestViewData()
{
	if (NULL == m_pMerchXml || NULL == m_pAbsCenterManager)
		return;

	// 无权限不请求该数据
	if ( !CPluginFuncRight::Instance().IsUserHasRight(CPluginFuncRight::FuncCapitalFlow, false) )
	{
		return;
	}
	
	if ( Cycle_OneDay == m_eCycle )
	{
		{
			// 普通情况下，申请最新单个商品 - 需要更新以后的周期数据，暂无
			CMmiReqTickEx info;
			info.m_iMarketId	= m_pMerchXml->m_MerchInfo.m_iMarketId;
			info.m_StrMerchCode = m_pMerchXml->m_MerchInfo.m_StrMerchCode;
			
			RequestData(info);
		}
		
		// 发RealtimeTick请求
		{
			CMmiReqPushPlugInMerchData Req;
			Req.m_iMarketID			= m_pMerchXml->m_MerchInfo.m_iMarketId;
			Req.m_StrMerchCode		= m_pMerchXml->m_MerchInfo.m_StrMerchCode;
			Req.m_uType				= ECSTTickEx;
			RequestData(Req);
		}
	}	
	else 
	{
		// 发送历史数据请求
		CMmiReqHistoryTickEx info;
		info.m_iMarketId	= m_pMerchXml->m_MerchInfo.m_iMarketId;
		info.m_StrMerchCode = m_pMerchXml->m_MerchInfo.m_StrMerchCode;
		RequestData(info);

		// 发RealtimeTick请求
		{
			CMmiReqPushPlugInMerchData Req;
			Req.m_iMarketID			= m_pMerchXml->m_MerchInfo.m_iMarketId;
			Req.m_StrMerchCode		= m_pMerchXml->m_MerchInfo.m_StrMerchCode;
			Req.m_uType				= ECSTTickEx;
			RequestData(Req);
		}
	}
}

void CIoViewCapitalFlow::RequestData( CMmiCommBase &req )
{
	// 隐藏时不要发请求，在视图激活时会根据当前数据与视图的关系，发一次请求
	//if ( IsWindowVisible() )
	{
		DoRequestViewData(req);
	}
}

BOOL CIoViewCapitalFlow::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;
}

CString CIoViewCapitalFlow::GetCycleString()
{
	switch (m_eCycle)
	{
	case Cycle_OneDay:
		return CString(_T("一日"));
	case Cycle_FiveDay:
		return CString(_T("五日"));
	}
	ASSERT( 0 );
	return _T("");
}

void CIoViewCapitalFlow::UpdateShowData( const T_CapitalFlowDataPseudo &cap )	
{
	if ( !(m_capData == cap) )
	{
		m_capData = cap;
		Invalidate(TRUE);
	}
}

void CIoViewCapitalFlow::UpdateShowData()
{
	T_CapitalFlowDataPseudo cap;
	if ( m_aTickExData.GetSize() > 0 )
	{
		if ( Cycle_OneDay == m_eCycle )
		{
			cap = m_aTickExData[0];
		}
		else
		{
			// 累加数据
			for ( int i=0; i < m_aTickExData.GetSize() ; i++ )
			{
				cap += m_aTickExData[i];
			}
		}
	}
	UpdateShowData(cap);
}

void CIoViewCapitalFlow::EmptyShowData()
{
	m_aTickExData.RemoveAll();
	UpdateShowData();
}

void CIoViewCapitalFlow::ChangeCycleType( E_Cycle eCycle )
{
	if ( m_eCycle != eCycle )
	{
		m_eCycle = eCycle;
		RequestViewData();
		//EmptyShowData();
		Invalidate(TRUE);
	}
}

void CIoViewCapitalFlow::OnLButtonDown( UINT nFlags, CPoint point )
{
	CRect rcCycle1(m_RectButton);
	CRect rcCycle2(m_RectButton);
	rcCycle1.right = rcCycle2.left = rcCycle1.left + m_RectButton.Width()/2;

	if ( rcCycle1.PtInRect(point) )
	{
		ChangeCycleType(Cycle_OneDay);
	}
	else if ( rcCycle2.PtInRect(point) )
	{
		ChangeCycleType(Cycle_FiveDay);
	}

	CIoViewBase::OnLButtonDown(nFlags, point);
}

void CIoViewCapitalFlow::OnTickExResp( const CMmiRespTickEx *pResp )
{
	CMerch  *pMerch = NULL;
	if ( NULL == pResp || NULL == m_pAbsCenterManager || NULL == m_pMerchXml || pResp->m_aTickEx.GetSize() == 0 )
	{
		return;
	}

	int i = 0;
	for ( i=0; i < pResp->m_aTickEx.GetSize() ; i++ )
	{
		const T_TickEx	&tick = pResp->m_aTickEx[i];
		if ( !m_pAbsCenterManager->GetMerchManager().FindMerch(tick.m_StrMerchCode, tick.m_iMarketId, pMerch) )
		{
			continue;
		}
		if ( pMerch == m_pMerchXml )
		{
			break;
		}
	}
	if ( pMerch != m_pMerchXml || i >= pResp->m_aTickEx.GetSize() )
	{
		return;
	}

	const T_TickEx &tickMy = pResp->m_aTickEx[i];
	
	// 如果此时是5日显示则更新当日数据，再显示，如果是1日数据则直接显示
	TRACE(_T("TickEx %d: %.0f-%.0f\r\n"), i, tickMy.m_fSmallBuyVol, tickMy.m_fSmallSellVol);

	T_CapitalFlowDataPseudo cap;
	TickExToShowData(tickMy, cap);

	if ( m_aTickExData.GetSize() > 0 )
	{
		m_aTickExData[0] = cap;		// 替换当日数据
	}
	else
	{
		m_aTickExData.Add(cap);
	}
	
	UpdateShowData();
}

void CIoViewCapitalFlow::OnTickExPeriodResp(const CMmiRespPeriodTickEx *pResp)
{
	CMerch  *pMerch = NULL;
	if ( NULL == pResp || NULL == m_pAbsCenterManager || NULL == m_pMerchXml || pResp->m_aTickEx.GetSize() == 0 )
	{
		return;
	}

	int i = 0;
	for ( i=0; i < pResp->m_aTickEx.GetSize() ; i++ )
	{
		const T_TickEx	&tick = pResp->m_aTickEx[i];
		if ( !m_pAbsCenterManager->GetMerchManager().FindMerch(tick.m_StrMerchCode, tick.m_iMarketId, pMerch) )
		{
			continue;
		}
		if ( pMerch == m_pMerchXml )
		{
			break;
		}
	}
	if ( pMerch != m_pMerchXml || i >= pResp->m_aTickEx.GetSize() )
	{
		return;
	}

	const T_TickEx &tickMy = pResp->m_aTickEx[i];

	// 如果此时是5日显示则更新当日数据，再显示，如果是1日数据则直接显示
	TRACE(_T("TickEx %d: %.0f-%.0f\r\n"), i, tickMy.m_fSmallBuyVol, tickMy.m_fSmallSellVol);

	T_CapitalFlowDataPseudo cap;
	TickExToShowData(tickMy, cap);

	if ( m_aTickExData.GetSize() > 0 )
	{
		m_aTickExData[0] = cap;		// 替换当日数据
	}
	else
	{
		m_aTickExData.Add(cap);
	}

	UpdateShowData();
}

void CIoViewCapitalFlow::OnTickExHistoryResp( const CMmiRespHistoryTickEx *pResp )
{
	CMerch  *pMerch = NULL;
	if ( NULL == pResp || NULL == m_pAbsCenterManager || NULL == m_pMerchXml || pResp->m_aTickEx.GetSize() == 0 )
	{
		return;
	}
	
	int i = 0;
	CArray<T_TickEx, const T_TickEx &>  aTickExHis;
	for ( i=0; i < pResp->m_aTickEx.GetSize() ; i++ )
	{
		const T_TickEx	&tick = pResp->m_aTickEx[i];
		if ( !m_pAbsCenterManager->GetMerchManager().FindMerch(tick.m_StrMerchCode, tick.m_iMarketId, pMerch) )
		{
			continue;
		}
		if ( pMerch == m_pMerchXml )
		{
			aTickExHis.Add(tick);
			TRACE(_T("TickExHis %d: %.0f-%.0f\r\n"), i, tick.m_fSmallBuyVol, tick.m_fSmallSellVol);
		}
	}
	TRACE(_T("\n"));
	if ( pMerch != m_pMerchXml )	// 不是该商品的数据
	{
		return;
	}
	
	// 转换所有数据
	m_aTickExData.RemoveAll();
	for ( i=0; i < aTickExHis.GetSize() ; i++ )
	{
		T_CapitalFlowDataPseudo cap;
		TickExToShowData(aTickExHis[i], cap);
		m_aTickExData.Add(cap);
	}

	UpdateShowData();
}

void CIoViewCapitalFlow::OnVDataPluginResp( const CMmiCommBase *pResp )
{
	if ( NULL == pResp )
	{
		return;
	}
	
	if ( pResp->m_eCommType != ECTRespPlugIn )
	{
		ASSERT( 0 );
		return;
	}

	// 无权限不接收该数据
	if ( !CPluginFuncRight::Instance().IsUserHasRight(CPluginFuncRight::FuncCapitalFlow, false) )
	{
		return;
	}
	
	const CMmiCommBasePlugIn *pRespPlugin = (CMmiCommBasePlugIn *)pResp;
	const E_CommTypePlugIn ePlugin = pRespPlugin->m_eCommTypePlugIn;
	if ( ePlugin == ECTPIRespTickEx)
	{
		OnTickExResp((const CMmiRespTickEx *)pResp);
	}
	else if(ePlugin == ECTPIRespAddPushTickEx )
	{
		OnTickExPeriodResp((const CMmiRespPeriodTickEx *)pResp);
	}
	else if ( ePlugin == ECTPIRespHistoryTickEx )
	{
		OnTickExHistoryResp((const CMmiRespHistoryTickEx *)pResp);
	}
}

void CIoViewCapitalFlow::OnTimer( UINT nIDEvent )
{
	if(!m_bShowNow)
	{
		return;
	}

	if ( KTimerIdPushDataReq == nIDEvent )
	{
		RequestViewData();
	}
	CIoViewBase::OnTimer(nIDEvent);
}

void CIoViewCapitalFlow::TickExToShowData( const T_TickEx &tickEx, OUT T_CapitalFlowDataPseudo &showData )
{
	showData.m_fBuyAmountBig = tickEx.m_fBigBuyAmount;
	showData.m_fBuyAmountHuge = tickEx.m_fExBigBuyAmount;
	showData.m_fBuyAmountMid = tickEx.m_fMidBuyAmount;
	showData.m_fBuyAmountSmall = tickEx.m_fSmallBuyAmount;
	
	showData.m_fSellAmountBig = tickEx.m_fBigSellAmount;
	showData.m_fSellAmountHuge = tickEx.m_fExBigSellAmount;
	showData.m_fSellAmountMid  = tickEx.m_fMidSellAmount;
	showData.m_fSellAmountSmall = tickEx.m_fSmallSellAmount;
	
	showData.m_fBuyVolBig	=	tickEx.m_fBigBuyVol;
	showData.m_fBuyVolHuge	=	tickEx.m_fExBigBuyVol;
	showData.m_fBuyVolMid	=	tickEx.m_fMidBuyVol;
	showData.m_fBuyVolSmall	=	tickEx.m_fSmallBuyVol;
	
	showData.m_fSellVolBig	=	tickEx.m_fBigSellVol;
	showData.m_fSellVolHuge	=	tickEx.m_fExBigSellVol;
	showData.m_fSellVolMid	=	tickEx.m_fMidSellVol;
	showData.m_fSellVolSmall	=	tickEx.m_fSmallSellVol;
	
	showData.m_fAllBuyAmount =	tickEx.m_fAllBuyAmount;
	showData.m_fAllSellAmount=	tickEx.m_fAllSellAmount;
	
	showData.m_fAmountPerTrans = tickEx.m_fAmountPerTrans;
	showData.m_fStocksPerTrans = tickEx.m_fStocksPerTrans;	
}

//////////////////////////////////////////////////////////////////////////
//
const T_CapitalFlowDataPseudo & T_CapitalFlowDataPseudo::operator+=( const T_CapitalFlowDataPseudo &cap )
{
	m_fBuyVolHuge	       += cap.m_fBuyVolHuge;
	m_fBuyVolBig		   += cap.m_fBuyVolBig;
	m_fBuyVolMid		   += cap.m_fBuyVolMid		  ;
	m_fBuyVolSmall         += cap.m_fBuyVolSmall    ;
	m_fSellVolHuge	       += cap.m_fSellVolHuge	  ;
	m_fSellVolBig          += cap.m_fSellVolBig     ;
	m_fSellVolMid          += cap.m_fSellVolMid     ;
	m_fSellVolSmall        += cap.m_fSellVolSmall   ;
	m_fBuyAmountHuge	   += cap.m_fBuyAmountHuge	;
	m_fBuyAmountBig	       += cap.m_fBuyAmountBig	  ;
	m_fBuyAmountMid	       += cap.m_fBuyAmountMid	  ;
	m_fBuyAmountSmall      += cap.m_fBuyAmountSmall ;
	m_fSellAmountHuge      += cap.m_fSellAmountHuge ;
	m_fSellAmountBig       += cap.m_fSellAmountBig  ;
	m_fSellAmountMid       += cap.m_fSellAmountMid  ;
	m_fSellAmountSmall     += cap.m_fSellAmountSmall;

	return *this;
}

const T_CapitalFlowDataPseudo T_CapitalFlowDataPseudo::operator+( const T_CapitalFlowDataPseudo &cap )
{
	T_CapitalFlowDataPseudo cap1(*this);
	cap1 += cap;
	return cap1;
}
