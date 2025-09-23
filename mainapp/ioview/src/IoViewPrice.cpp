#include "stdafx.h"
#include "memdc.h"
#include "IoViewManager.h"
#include "IoViewPrice.h"
#include "GridCellSymbol.h"
#include "facescheme.h"
#include "ReportScheme.h"
#include "ShareFun.h"
#include "IoViewTimeSale.h"
#include "MPIChildFrame.h"
#include "GridCellLevel2.h"
#include "MerchManager.h"
#include "FontFactory.h"
#include "DlgRecentMerch.h"
#include "MPIChildFrame.h"
#include "BalloonMsg.h"
#include "GGTongView.h"
#include "XmlShare.h"
#include "BuySellPriceExchange.h"

#define   GRIDZOOMPARAM		1.15
#define	  ERTSHOWBUYSELL	50

static const TCHAR KStrPriceShowFutureCnTick[]	=	_T("ShowFutureCnTick");

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
IMPLEMENT_DYNCREATE(CIoViewPrice, CIoViewBase) 
///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewPrice, CIoViewBase)
//{{AFX_MSG_MAP(CIoViewPrice)
ON_WM_PAINT()
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_MOUSEMOVE()
ON_WM_CREATE()
ON_WM_SIZE()
ON_WM_TIMER()
ON_WM_ERASEBKGND()
ON_WM_DESTROY()
ON_WM_GETMINMAXINFO()
//}}AFX_MSG_MAP
ON_NOTIFY(NM_RCLICK,20205,OnGridRButtonDown)
ON_NOTIFY(NM_CLICK,20205,OnGridLButtonDown)
ON_NOTIFY(NM_DBLCLK,20205,OnGridLDBClick)
ON_NOTIFY(GVN_BEGINLABELEDIT,20205,OnGridEditBegin)
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////
const int32		KTimerSnapshotElapseFirst  = 10;
const int32		KTimerSnapshotElapseNormal = 3000;
const int32		KShowTimeSaleCount		 = 10000;		// 最大的分笔数

// 指数红绿柱的显示高度
static const int32 KiFixedGeneralShowHeight   = 30;

// 这个区域默认纵向显示 5 个小矩形
static const int32 KiHeightCounts = 5;

// 1档和5档之间的间隙宽度
static const int32 KiCenterSkip = 3;

// 左边空出来的小空隙
static const int32 KiLeftSkip = 2;


#define   MIX_SCREEN_HEIGHT 900


///////////////////////////////////////////////////////////////////////////////
// 
CIoViewPrice::CIoViewPrice()
:CIoViewBase()
{
	m_eMerchKind	= ERTStockCn;
	m_bFromXml		= false;
	m_iRowHeight	= -1;
	m_iTitleHeight	= -1;
	m_aSeparatorLine.RemoveAll();
	m_aMarketIdArray.RemoveAll();
	m_RectView		= CRect(0,0,0,0);
	m_RectIndexRnG	= CRect(0,0,0,0);
	m_bIsShowBuySell = false;
	m_iBuySellCnt = 1;

	m_iScreenHeight = GetSystemMetrics(SM_CYSCREEN); 

	//
	m_RectSpecialBuy.SetRectEmpty();
	m_RectSpecialSell.SetRectEmpty();

	//
	ResetFutureCnData();
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewPrice::~CIoViewPrice()
{

}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
void CIoViewPrice::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if ( !IsWindowVisible() )
	{
		return;
	}

	if ( GetParentGGTongViewDragFlag() || m_bDelByBiSplitTrack )
	{
		LockRedraw();
		return;
	}

	//
	UnLockRedraw();
	
	if ( m_GridCtrl.GetSafeHwnd() )
	{
		m_GridCtrl.RedrawWindow();
	}
	
	COLORREF clrBk = GetIoViewColor(ESCBackground);

	CMemDC dcMem(&dc, m_rcTitleHead);
	dcMem->FillSolidRect(m_rcTitleHead, clrBk);
	//DrawCorner(dcMem, m_rcTitleHead);

	if ( m_CtrlTitle.GetSafeHwnd() )
	{
		m_CtrlTitle.RedrawWindow();			
	}
}

BOOL CIoViewPrice::OnEraseBkgnd(CDC* pDC)
{
	CRect rectClient;
	GetClientRect(&rectClient);
	
	COLORREF clrGridLine = GetIoViewColor(ESCChartAxisLine);
	
	CBrush brush(clrGridLine);
	
	CBrush* pOldBrush = pDC->SelectObject(&brush);
	CRect rectDraw = m_RectView;
	rectDraw.left -= 1;
	pDC->SelectObject(pOldBrush);
	brush.DeleteObject();

	if ( m_RectView.right < rectClient.right)
	{
		CRect RectRight = rectClient;
		RectRight.left	= m_RectView.right;

		pDC->FillSolidRect(&RectRight, GetIoViewColor(ESCBackground));
	}

	if ( m_RectView.bottom < rectClient.bottom)
	{
		CRect RectBottom = rectClient;
		RectBottom.top = m_RectView.bottom;

		CRect RectRiseFall(0,0,0,0);
		m_GridExpRiseFall.GetWindowRect(RectRiseFall);
		ScreenToClient(&RectRiseFall);
		if ( m_GridExpRiseFall.IsWindowVisible() && RectBottom.top < RectRiseFall.bottom )
		{
			RectBottom.top = RectRiseFall.bottom;
		}

		pDC->FillSolidRect(&RectBottom, GetIoViewColor(ESCBackground));
	}

	//
	COLORREF clrSell = GetIoViewColor(ESCFall);
	COLORREF clrBuy  = GetIoViewColor(ESCRise);
	COLORREF clrBk	 = GetIoViewColor(ESCBackground);
	COLORREF clrText = GetIoViewColor(ESCVolume);
	
	COLORREF clrTextBk;
	//
	if (BeSpecialBuySell() && !m_RectSpecialBuy.IsRectEmpty() && !m_RectSpecialSell.IsRectEmpty())
	{
		//
		pDC->FillSolidRect(m_RectSpecialBuy, clrBk);
		pDC->FillSolidRect(m_RectSpecialSell, clrBk);
		
		if (NULL != m_pMerchXml && NULL != m_pMerchXml->m_pRealtimePrice)
		{
			//
			CFont* pFontOld = pDC->SelectObject(&m_fontBuySell);
			
			int32 iSaveDec = m_pMerchXml->m_MerchInfo.m_iSaveDec;
			CString strBuy = L"买";
			CString strSell = L"卖";
			if (BeERTMony())
			{
				CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
				strBuy = pApp->m_pConfigInfo->m_strBuy;
				strSell = pApp->m_pConfigInfo->m_strSell;
			}
			
			/////////////////////////////////////////////////
			// 卖出价
			CString StrPriceSell = Float2String(m_pMerchXml->m_pRealtimePrice->m_astSellPrices[0].m_fPrice, iSaveDec);				
			clrTextBk = pDC->SetTextColor(clrSell);
			pDC->DrawText(strSell, m_RectSpecialBuy, DT_VCENTER | DT_LEFT | DT_SINGLELINE);

			CRect rtBuy = m_RectSpecialBuy;
			CSize szBuy = pDC->GetTextExtent(strSell);
			rtBuy.left += szBuy.cx;

			pDC->DrawText(StrPriceSell, rtBuy, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
			/////////////////////////////////////////////////
			// 买入价 
			CString StrPriceBuy = Float2String(m_pMerchXml->m_pRealtimePrice->m_astBuyPrices[0].m_fPrice, iSaveDec);	
			clrTextBk = pDC->SetTextColor(clrBuy);

			pDC->DrawText(strBuy, m_RectSpecialSell, DT_VCENTER | DT_RIGHT | DT_SINGLELINE);
			
			CRect rtSell = m_RectSpecialSell;
			rtSell.right -= szBuy.cx;

			//
			pDC->DrawText(StrPriceBuy, rtSell, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
			
			//
			CPen penNow(PS_SOLID, 1, GetIoViewColor(ESCSpliter));
			CPen* penOld = pDC->SelectObject(&penNow);
			CPoint ptLB(m_RectSpecialBuy.left, m_RectSpecialBuy.bottom - 1);
			CPoint ptRB(m_RectSpecialSell.right, m_RectSpecialBuy.bottom - 1);
			
			pDC->MoveTo(ptLB);
			pDC->LineTo(ptRB);
			
			pDC->SelectObject(penOld);
			pDC->SetTextColor(clrTextBk);
			pDC->SelectObject(pFontOld);

			return TRUE;
		}		
	}

	// 画指数的红绿柱
	T_GeneralNormal stData;
	bool32 bH;
	if ( !BeGeneralIndex(bH) )
	{
		return TRUE;
	}

	if ( m_RectIndexRnG.IsRectEmpty() )
	{
		return TRUE;
	};

	pDC->FillSolidRect(m_RectIndexRnG, clrBk);
	pDC->SetBkMode(TRANSPARENT);

	CPen PenLine(PS_SOLID, 1, GetIoViewColor(ESCChartAxisLine));
	CPen* pOldPen = pDC->SelectObject(&PenLine);
	pDC->MoveTo(m_RectIndexRnG.left, m_RectIndexRnG.bottom - 1);
	pDC->LineTo(m_RectIndexRnG.right, m_RectIndexRnG.bottom -1);
	pDC->SelectObject(pOldPen);
	PenLine.DeleteObject();

	//
	if ( NULL == m_pAbsCenterManager )
	{
		return TRUE;
	}

	int32 iBuyFlag  = 0;
	int32 iSellFlag = 0;

	const CAbsCenterManager::GeneralNormalArray* paGeneralNormal = NULL;
	
	if ( bH )
	{
		paGeneralNormal = &m_pAbsCenterManager->GetGeneralNormalArrayH();		
		m_pAbsCenterManager->GetGeneralNormalHFlag(iBuyFlag, iSellFlag);
	}
	else
	{
		paGeneralNormal = &m_pAbsCenterManager->GetGeneralNormalArrayS();		
		m_pAbsCenterManager->GetGeneralNormalSFlag(iBuyFlag, iSellFlag);
	}

	if ( (*paGeneralNormal).GetSize() <= 0 )
	{
		return TRUE;
	}
	
	stData = (*paGeneralNormal).GetAt((*paGeneralNormal).GetSize() - 1);

	//
	float fBuy1  = stData.m_fBuy1Amount;		
	float fBuy5	 = stData.m_fBuy5Amount;
	float fSell1 = stData.m_fSell1Amount;
	float fSell5 = stData.m_fSell5Amount;
	
	if ( fBuy1 <= 0. || fBuy5 <= 0. || fSell1 <= 0. || fSell5 <= 0. )
	{
		return TRUE;
	}
	
	float fMax = (fBuy1 + fBuy5) >= (fSell1 + fSell5) ? (fBuy1 + fBuy5) : (fSell1 + fSell5);

	// 文字的宽度
	CString StrTest = L"88.8亿";
	/*CFont* pFontOld = */pDC->SelectObject(GetIoViewFontObject(ESFSmall));
	CSize size = pDC->GetTextExtent(StrTest);
	int32 iTextWidth = size.cx + 50;

	// 箭头的宽度
	int32 iArrowWidth = 20;

	//
	int32 iRectHeigh = m_RectIndexRnG.Height() / KiHeightCounts;

	//
	int32 iLeftAlign = m_RectIndexRnG.left + KiLeftSkip;

	// 卖
	{
		CRect RectSell1;
		CRect RectSell5;
		CRect rectText;
		
		// 卖 1 的矩形
		int32 iWidth1		= (int32)((m_RectIndexRnG.Width() - iTextWidth - iArrowWidth - KiLeftSkip - KiCenterSkip) * (fSell1 / fMax));
		RectSell1.top		= m_RectIndexRnG.top + iRectHeigh;
		RectSell1.bottom	= RectSell1.top + iRectHeigh;
		RectSell1.left		= iLeftAlign;
		RectSell1.right		= RectSell1.left + iWidth1;
		
		// 填充卖 1 小矩形
		pDC->FillSolidRect(RectSell1, clrSell);
		
		// 卖 5 的矩形
		int32 iWidth2		= (int32)((m_RectIndexRnG.Width() - iTextWidth - iArrowWidth - KiLeftSkip - KiCenterSkip) * (fSell5 / fMax));
		RectSell5.top		= RectSell1.top;
		RectSell5.bottom	= RectSell1.bottom;
		RectSell5.left		= RectSell1.right + KiCenterSkip;
		RectSell5.right		= RectSell5.left + iWidth2;
		
		// 填充卖 5 小矩形
		pDC->FillSolidRect(RectSell5, clrSell);
		
		// 写字
		rectText.top	= m_RectIndexRnG.top;
		rectText.bottom	= RectSell1.bottom + 3;		
		rectText.right	= m_RectIndexRnG.right - iArrowWidth;
		rectText.left	= rectText.right - iTextWidth;
		
		//
		CString StrText = Float2String(fSell1 + fSell5, 1, true);
		clrTextBk = pDC->SetTextColor(clrText);		
		pDC->DrawText(StrText, rectText, DT_SINGLELINE | DT_RIGHT | DT_BOTTOM);
		pDC->SetTextColor(clrTextBk);

		// 小箭头
		if ( 0 != iSellFlag )
		{
			CString StrArrow = L"▲";
			
			if ( -1 == iSellFlag )
			{
				// 向下箭头
				StrArrow = L"▼";
			}
			
			CRect RectArrow;
			RectArrow.top	= rectText.top;
			RectArrow.bottom= RectSell1.bottom + 1;
			RectArrow.left	= rectText.right;
			RectArrow.right	= m_RectIndexRnG.right;
			
			//					
			clrTextBk = pDC->SetTextColor(RGB(0, 128, 0));
			pDC->DrawText(StrArrow, RectArrow, DT_SINGLELINE | DT_CENTER | DT_BOTTOM);
			pDC->SetTextColor(clrTextBk);
		}
	}

	// 买
	{
		CRect RectBuy1;
		CRect RectBuy5;
		CRect rectText;
		
		// 买 1 矩形
		int32 iWidth1	= (int32)((m_RectIndexRnG.Width() - iTextWidth - iArrowWidth - KiLeftSkip - KiCenterSkip) * (fBuy1 / fMax));
		RectBuy1.top	= m_RectIndexRnG.top + iRectHeigh * 3;
		RectBuy1.bottom	= RectBuy1.top + iRectHeigh;
		RectBuy1.left	= iLeftAlign;
		RectBuy1.right	= RectBuy1.left + iWidth1;
		
		// 填充买 1 矩形
		pDC->FillSolidRect(RectBuy1, clrBuy);
		
		// 买 5 矩形
		int32 iWidth2	= (int32)((m_RectIndexRnG.Width() - iTextWidth - iArrowWidth - KiLeftSkip - KiCenterSkip) * (fBuy5 / fMax));
		RectBuy5.top	= RectBuy1.top;
		RectBuy5.bottom	= RectBuy1.bottom;
		RectBuy5.left	= RectBuy1.right + KiCenterSkip;
		RectBuy5.right	= RectBuy5.left + iWidth2;
		
		// 填充卖 5 小矩形
		pDC->FillSolidRect(RectBuy5, clrBuy);
		
		// 写字
		rectText.top	= m_RectIndexRnG.top + iRectHeigh * 2;
		rectText.bottom	= RectBuy1.bottom + 3;		
		rectText.right	= m_RectIndexRnG.right - iArrowWidth;
		rectText.left	= rectText.right - iTextWidth;
		
		//
		CString StrText = Float2String(fBuy1 + fBuy5, 1, true);
		clrTextBk = pDC->SetTextColor(clrText);
		pDC->DrawText(StrText, rectText, DT_SINGLELINE | DT_RIGHT | DT_BOTTOM);
		pDC->SetTextColor(clrTextBk);

		// 小箭头
		if ( 0 != iBuyFlag )
		{
			CString StrArrow = L"▲";
			
			if ( -1 == iBuyFlag )
			{
				// 向下箭头
				StrArrow = L"▼";
			}
			
			CRect RectArrow;
			RectArrow.top	= RectBuy1.top;
			RectArrow.bottom= RectBuy1.bottom + 3;
			RectArrow.left	= rectText.right;
			RectArrow.right	= m_RectIndexRnG.right;
			
			//		
			clrTextBk = pDC->SetTextColor(RGB(255, 0, 0));			
			pDC->DrawText(StrArrow, RectArrow, DT_SINGLELINE | DT_CENTER | DT_BOTTOM);
			pDC->SetTextColor(clrTextBk);
		}
	}

	return TRUE;
}		

///////////////////////////////////////////////////////////////////////////////
// Draw
void CIoViewPrice::Draw()
{	
	
}
///////////////////////////////////////////////////////////////////////////////
// OnLButtonDown
void CIoViewPrice::OnLButtonDown(UINT nFlags, CPoint point)
{	
// 	if ( m_RectIndexRnG.PtInRect(point) )
// 	{
// 		CString StrPrompt = L"绿色小条: 市场挂卖一的总金额\n绿色长条: 市场五档卖盘总金额\n红色小条: 市场挂买一的总金额\n红色长条: 市场五档买盘总金额\n▲▼标志: 比上一笔数据的涨跌";
// 				
// 		//
// 		CPoint pt(m_RectIndexRnG.left + 15, m_RectIndexRnG.bottom - 8);
// 		ClientToScreen(&pt);
// 		CBalloonMsg::RequestCloseAll();
// 		CBalloonMsg::Show(L"大盘多空阵线", StrPrompt, (HICON)1, &pt);
// 	}
// 	else
// 	{
// 		m_TipWnd.Hide();
// 	}
	CStatic::OnLButtonDown(nFlags, point);
}

//OnRButtonDown linhc 20100904添加右击消息响应。
void CIoViewPrice::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_TipWnd.Hide();
	CIoViewBase::OnRButtonDown(nFlags,point);
}
///////////////////////////////////////////////////////////////////////////////
// OnLButtonUp
void CIoViewPrice::OnLButtonUp(UINT nFlags, CPoint point)
{
	CStatic::OnLButtonUp(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnMouseMove
void CIoViewPrice::OnMouseMove(UINT nFlags, CPoint point)
{	
	CRect rcMark = m_CtrlTitle.GetDrewMarkRect();
	m_CtrlTitle.ClientToScreen(&rcMark);
	ScreenToClient(&rcMark);
	T_MarkData MarkData;
	if ( NULL != m_pMerchXml
		&& rcMark.PtInRect(point)
		&& CMarkManager::Instance().QueryMark(m_pMerchXml, MarkData)
		&& EMT_Text == MarkData.m_eType )
	{
		CString Str;
		Str.Format(_T("%s(%s)  CTRL+Q修改标记文本")
			, m_pMerchXml->m_MerchInfo.m_StrMerchCnName.GetBuffer()
			, m_pMerchXml->m_MerchInfo.m_StrMerchCode.GetBuffer());
		m_TipWnd.Show(point, MarkData.m_StrTip, Str);
	}
	else
	{
		m_TipWnd.Hide();
	}

	CStatic::OnMouseMove(nFlags, point);
}

void CIoViewPrice::OnGridMouseMove(CPoint pt)
{
}

int CIoViewPrice::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CIoViewBase::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	// 获得颜色和字体
	InitialIoViewFace(this);

	LOGFONT fontBuySell;
	memset(&fontBuySell, 0, sizeof(fontBuySell));
	_tcscpy(fontBuySell.lfFaceName, gFontFactory.GetExistFontName(_T("微软雅黑")));///
	fontBuySell.lfHeight  = -20;
   	fontBuySell.lfOutPrecision = 3;
	fontBuySell.lfClipPrecision = 2;
	fontBuySell.lfQuality = 1;
	fontBuySell.lfPitchAndFamily = 34;

	//
	m_fontBuySell.CreateFontIndirect(&fontBuySell);

	m_TipWnd.Create(this);
		 
	// 创建当前商品对应的表格
 	E_ReportType eMerchKind = GetMerchKind(m_pMerchXml);
	CreateTable(eMerchKind);

  	if ( m_eMerchKind != eMerchKind )
  	{
  		if (CreateTable(eMerchKind))
  		{
  			// 保存当前表格类型
  			m_eMerchKind = eMerchKind;
  		}
  	}
 
	//
	if ( m_CtrlTitle.GetSafeHwnd() )
	{
		m_CtrlTitle.SetDrawActiveParams(m_iActiveXpos, m_iActiveXpos, m_ColorActive);
	}

	// 标记监听
	CMarkManager::Instance().AddMerchMarkChangeListener(this);

 	// 尝试设置表格内容
 	OnVDataRealtimePriceUpdate(m_pMerchXml);	
	return 0;
}

void CIoViewPrice::OnSize(UINT nType, int cx, int cy) 
{		
	CIoViewBase::OnSize(nType,cx,cy);	
	SetSize();
	Invalidate();
	m_GridCtrl.Refresh();
}

void CIoViewPrice::SetSize()
{
	// 行高固定， 列宽设最小值，可以更宽
	int32 iViewWidth;
	float fCharWidth;

	iViewWidth = GetProperSize(&fCharWidth).cx;
	if (iViewWidth < m_rectClient.Width())
		iViewWidth = m_rectClient.Width();
	//
	if (m_GridCtrl.GetColumnCount() == 2)
	{
		int32 iWidth0 = iViewWidth / 2;
		m_GridCtrl.SetColumnWidth(0,iWidth0);
		m_GridCtrl.SetColumnWidth(1,iViewWidth-iWidth0);		
	}
	else if (m_GridCtrl.GetColumnCount() == 4)
	{
		// 四列,一三列三个字符宽,二四列三个字符宽
		// xl 1202 1,3 2.5字符宽 2,4 3.5字符宽
		int32 iHalfWidth = iViewWidth / 2;
		// 		int32 iWidth0 = iHalfWidth / 2 + 15;
		// 		int32 iWidth1 = iHalfWidth - iWidth0 + 3;
		int32 iWidth0 = (int32)(iHalfWidth / 2.0 - fCharWidth/2.0);
		//int32 iWidth1 = iHalfWidth - iWidth0 + 3;	// 余下
		int32 iWidth1 = iHalfWidth - iWidth0; // 2014-01-21 by cym
		int32 iWidth2 = (int32)((iViewWidth - iHalfWidth) /2.0 - fCharWidth/2.0);
		//int32 iWidth3 = iViewWidth - iWidth0 - iWidth1 - iWidth2;
		int32 iWidth3 = iHalfWidth - iWidth2; //2014-01-21 by cym
        //linhc	20100908调整了显示的列宽，因为有个别字段太长无法显示。
		/*
		m_GridCtrl.SetColumnWidth(0,iWidth3);
		m_GridCtrl.SetColumnWidth(1,iWidth1);
		m_GridCtrl.SetColumnWidth(2,iWidth2);
		m_GridCtrl.SetColumnWidth(3,iWidth0);
		*/
		//调整显示列宽，使一二和三四列的间隔相同 2014-01-21 by cym
		m_GridCtrl.SetColumnWidth(0,iWidth0);
		m_GridCtrl.SetColumnWidth(1,iWidth1);
		m_GridCtrl.SetColumnWidth(2,iWidth2);
		m_GridCtrl.SetColumnWidth(3,iWidth3);
	}

	if ( m_GridExpRiseFall.GetColumnCount() == 4 )
	{
		// 四列,一三列三个字符宽,二四列三个字符宽
		// xl 1202 1,3 2.5字符宽 2,4 3.5字符宽
		int32 iHalfWidth = iViewWidth / 2;
		// 		int32 iWidth0 = iHalfWidth / 2 + 15;
		// 		int32 iWidth1 = iHalfWidth - iWidth0 + 3;
		int32 iWidth0 = (int32)(iHalfWidth / 2.0 + fCharWidth);
		int32 iWidth1 = (int32)(iHalfWidth - iWidth0);	// 余下
		int32 iWidth2 = (int32)((iViewWidth - iHalfWidth) /2.0 + fCharWidth);
		int32 iWidth3 = (int32)(iViewWidth - iWidth0 - iWidth1 - iWidth2);
        //linhc	20100908调整了显示的列宽，因为有个别字段太长无法显示。
		m_GridExpRiseFall.SetColumnWidth(0,iWidth0);
		m_GridExpRiseFall.SetColumnWidth(1,iWidth1);
		m_GridExpRiseFall.SetColumnWidth(2,iWidth2);
		m_GridExpRiseFall.SetColumnWidth(3,iWidth3);
	}

	CRect RectTitle;
	RectTitle.top		= m_rectClient.top;
	RectTitle.bottom	= RectTitle.top + 62;//m_iTitleHeight;
	RectTitle.left		= m_rectClient.left;
	RectTitle.right		= RectTitle.left+ iViewWidth;
	m_CtrlTitle.MoveWindow(RectTitle);
	m_CtrlTitle.SetDrawCornerFlag(false);
	
	// 红绿柱
	bool32 bH = false;
	bool32 bRedGreenExp = BeGeneralIndex(bH);
	if ( bRedGreenExp )
	{
		m_RectIndexRnG = m_rectClient;
		m_RectIndexRnG.top = RectTitle.bottom;
		m_RectIndexRnG.bottom = m_RectIndexRnG.top + KiFixedGeneralShowHeight;
	}
	else
	{
		m_RectIndexRnG.SetRectEmpty();
	}

	// 自画的买卖价:
	bool32 bSpecialBuySell = BeSpecialBuySell();
	if (bSpecialBuySell && !m_bIsShowBuySell)	// 加一个判断是否 是指定要显示买卖5档的商品
	{
		m_RectSpecialBuy = m_rectClient;
		m_RectSpecialBuy.top = RectTitle.bottom;
		m_RectSpecialBuy.bottom = m_RectSpecialBuy.top + m_iTitleHeight + 10;
		
		m_RectSpecialSell = m_RectSpecialBuy;
		m_RectSpecialBuy.right = m_RectSpecialBuy.left + m_RectSpecialBuy.Width() / 2;

		m_RectSpecialSell.left = m_RectSpecialBuy.right;
	}
	else
	{
		m_RectSpecialBuy.SetRectEmpty();
		m_RectSpecialSell.SetRectEmpty();
	}
	//
	CRect RectClient;
	GetClientRect(RectClient);

	CRect RectGrid = RectClient;
	if ( m_RectIndexRnG.IsRectEmpty() )
	{
		RectGrid.top = RectTitle.bottom;
	}
	else
	{
		RectGrid.top = m_RectIndexRnG.bottom;
	}

	if (m_RectSpecialBuy.IsRectEmpty())
	{
		RectGrid.top = RectTitle.bottom;
	}
	else
	{
		RectGrid.top = m_RectSpecialBuy.bottom;
	}

	//
	int32 iGridVH = m_GridCtrl.GetRowCount()*21;
	int32 iTmpHeight = RectGrid.Height();
	iGridVH = min(iGridVH, iTmpHeight);
	RectGrid.bottom = RectGrid.top + iGridVH;

	CRect RectRiseFall = RectClient;
	RectRiseFall.top = RectGrid.bottom;
	if ( RectRiseFall.Height() < 0 )
	{
		RectRiseFall.bottom = RectRiseFall.top;
	}
	//if ( bRedGreenExp )
	//{
	//	// 显示红绿线则涨跌同行，显示涨跌表格
	//}
	//else
	//{
	//	// 涨跌与xx同表格
	//}
	
	m_RectView.top		= m_rectClient.top ;
	m_RectView.bottom	= RectGrid.bottom;
	m_RectView.left		= m_rectClient.left;
 	m_RectView.right	= RectGrid.right + 1;

	// 
	m_GridCtrl.MoveWindow(RectGrid);
	m_GridExpRiseFall.MoveWindow(RectRiseFall);

	m_GridExpRiseFall.AutoSizeRows();
	m_GridCtrl.AutoSizeRows();
}

void CIoViewPrice::SetRowHeightAccordingFont()
{	
	CFont * pFontNormal = GetIoViewFontObject(ESFText);

	CClientDC dc(this);	
	CFont* pOldFont = dc.SelectObject(pFontNormal); // font为要得其高宽的字体	
	CSize size = dc.GetTextExtent(L"一二三四");
	dc.SelectObject(pOldFont);

	m_iRowHeight = size.cy + size.cy / 6;
	int32 iMarginHeight = size.cy / 6;
	if (iMarginHeight < 4)
		iMarginHeight = 6;

	m_iRowHeight += iMarginHeight;

	E_ReportType eMerchKind = ERTCount;
	if (NULL != m_pMerchXml)
	{
		eMerchKind = GetMerchKind(m_pMerchXml);
	}

	int32 i = 0;
	for ( i = 0 ; i < m_GridCtrl.GetRowCount() ; i++)
	{
		m_GridCtrl.SetRowHeight(i,m_iRowHeight);
	}

	// 
	m_iTitleHeight = 26/*m_iRowHeight*/; 

	for ( i=0; i < m_GridExpRiseFall.GetRowCount() ; i++ )
	{
		m_GridExpRiseFall.SetRowHeight(i, m_iRowHeight);
	}

	// 
	SetSize();
}

BOOL CIoViewPrice::TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return FALSE;
}

// 通知视图改变关注的商品
void CIoViewPrice::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch || NULL == pMerch)
		return;

	// 如果商品发生了类型切换，则通知frame进行rect的重计算
	CMerch *pMerchOld = m_pMerchXml;
	bool32 bH;
	bool32 bGeneralIndexOld = BeGeneralIndex(bH);
	
	// 修改当前查看的商品
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;

	if(m_pMerchXml != NULL)
	{
		m_bIsShowBuySell = BeShowBuySell(m_pMerchXml->m_MerchInfo.m_iMarketId);
		m_iBuySellCnt = GetShowBuySellItem(m_pMerchXml->m_MerchInfo.m_iMarketId);	// 获取显示iItem档
	}

	// 设置关注的商品信息
	m_aSmartAttendMerchs.RemoveAll();
	
	CSmartAttendMerch SmartAttendMerch;
	SmartAttendMerch.m_pMerch = pMerch;
	SmartAttendMerch.m_iDataServiceTypes = EDSTPrice;
    //linhc 20100914添加关注数据类型
    SmartAttendMerch.m_iDataServiceTypes |= EDSTGeneral;
	m_aSmartAttendMerchs.Add(SmartAttendMerch);

	if ( IsFutureCn(pMerch) )
	{
		InitFutureCnTradingDay();
		ResetFutureCnData();
	}
	
	// 在CreateTable前完成此类型初始化
	m_aExpMerchs.RemoveAll();
	E_MerchExpType eMET = EMET_Count;
	if ( (eMET = GetMerchExpType(pMerch)) != EMET_Count )	// 写死 上证指 与深证指
	{
// 		上海、深证指数买卖盘增加显示字段：
// 			主要目的：1、填充股票和指数切换时的买卖盘与分笔之间空隙
// 			2、增加显示内容，帮助客户了解大盘概况
// 			深证指数和上海指数分别增加如下八个字段，分为两栏显示，后面显示成交金额
// 			沪深A股                      沪深A股 深市399101+399102+399107+沪市000002
// 			沪深B股                      沪深B股 深市399108+沪市000003
// 			沪深基金                      沪深基金  深市399305+沪市000011
// 			中 小 板                      中 小 板  深市399101
// 			创 业 板                      创 业 板  深市399102
// 			------------                       ------------     
// 			深证A股 399101+399102+399107 上证A股  000002 
// 			深证B股  399108              上证B股  000003
		bool32 bOk = GetAppendExpMerchArray(eMET, m_aExpMerchs);
		ASSERT( bOk );
		if ( bOk )
		{
			for ( int i=0; i < m_aExpMerchs.GetSize() ; i++ )
			{
				const ExpMerchArray &ExpAry = m_aExpMerchs[i].second;
				//for ( ExpMerchArray::const_iterator it=ExpAry.begin(); it != ExpAry.end(); it++  )
				for (int k = 0;k < ExpAry.size();++k)
				{
					if ( ExpAry[k] != NULL )
					{
						CSmartAttendMerch TmpSmartAttendMerch;
						TmpSmartAttendMerch.m_pMerch = ExpAry[k];
						TmpSmartAttendMerch.m_iDataServiceTypes = EDSTPrice;	// 仅需要Price就可以了
						m_aSmartAttendMerchs.Add(TmpSmartAttendMerch);
					}
				}
			}
		}
	}
	
	// 尝试更新标题
	T_MarkData MarkNowData;
	GV_DRAWMARK_ITEM markItem;
	markItem.eDrawMarkFlag = EDMF_None;
	if ( CMarkManager::Instance().QueryMark(iMarketId, StrMerchCode, MarkNowData) )
	{	
		InitDrawMarkItem(MarkNowData, markItem);
	}
	m_CtrlTitle.SetDrawMarkItem(markItem);
	m_CtrlTitle.Invalidate();
	CString StrTitle;
	StrTitle.Format(L"%s(%s)", pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer());
	CString StrSubTitle = L"";
    CRealtimePrice* pRealPrice = m_pMerchXml->m_pRealtimePrice;
	if(pRealPrice && abs(pRealPrice->m_fPriceNew) >  0.000001f)
	{
		// 现价
		float fPrevReferPrice = pRealPrice->m_fPricePrevClose;
		int iSaveDec = m_pMerchXml->m_MerchInfo.m_iSaveDec;
		CString StrPriceNew = Float2SymbolString( pRealPrice->m_fPriceNew , fPrevReferPrice, iSaveDec);	
	
		//  涨跌
		CString StrRiseValue = L"";
		if (0. != pRealPrice->m_fPriceNew)
		{
			StrRiseValue = Float2SymbolString(pRealPrice->m_fPriceNew - fPrevReferPrice, 0, iSaveDec, false, false);	
		}
	
		//  涨幅
		CString StrPriceRisePercent = L"";
		if (0. != pRealPrice->m_fPriceNew && 0. != fPrevReferPrice)
		{
			float fRisePercent = ((pRealPrice->m_fPriceNew - fPrevReferPrice) / fPrevReferPrice) * 100.;
			StrPriceRisePercent = Float2SymbolString(fRisePercent, 0, 2, false, false, true);
		}

		StrSubTitle.Format(L"%s|%s|%s", StrPriceNew, StrRiseValue, StrPriceRisePercent);
	}
	
	m_CtrlTitle.SetTitle(StrTitle, StrSubTitle);
	
	// 创建当前商品对应的表格
	// fangz 1015 for bohai
	E_ReportType eMerchKind = GetMerchKind(pMerch);
	// if ( m_eMerchKind != eMerchKind )
	{
		if (CreateTable(eMerchKind))
		{
			// 保存当前表格类型
			m_eMerchKind = eMerchKind;
		}
	}
	
	//
	if ( m_eMerchKind == ERTExp )
	{
		SetTimer(SNAPSHOTTIMER, KTimerSnapshotElapseNormal, NULL);
	}
	else
	{
		KillTimer(SNAPSHOTTIMER);
	}

	// 尝试设置表格内容
	OnVDataRealtimePriceUpdate(pMerch);	

	if ( IsFutureCn(pMerch) && IsNeedShowFutureCnTick() )
	{
		UpdateFutureCnTick();
	}
	
	// 上涨下跌的处理
	m_aMarketIdArray.RemoveAll();
	int32 iRiseCounts = CalcRiseFallCounts(true);
	int32 iFallCounts = CalcRiseFallCounts(false);
	if ( iRiseCounts + iFallCounts > 0 )
	{
		ShowRiseFallCounts(iRiseCounts, iFallCounts);
	}	
 	//
 	SetSize();
	
	CClientDC dc(this);
	OnEraseBkgnd(&dc);

	m_TipWnd.Hide();

	bool32 bGeneralIndexNow = BeGeneralIndex(bH);
	if ( NULL == pMerchOld 
		|| pMerchOld->m_Market.m_MarketInfo.m_eMarketReportType != m_pMerchXml->m_Market.m_MarketInfo.m_eMarketReportType
		|| (bGeneralIndexOld ? !bGeneralIndexNow : bGeneralIndexNow) )
	{
		// 商品类型变更了 或者商品为特殊指数变换了
		if ( NULL != pMerchOld )
		{
			CRect rcNow(0,0,0,0);
			GetClientRect(rcNow);
			m_mapMerchHeight[pMerchOld->m_Market.m_MarketInfo.m_eMarketReportType] = rcNow.Height();
		}

		CMPIChildFrame *pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
		if ( NULL != pFrame )
		{
			pFrame->RecalcLayoutAsync();	// 重新计算面积的布局
			return ;
		}
	}
}

void CIoViewPrice::RequestViewData()
{
	if ( !IsWindowVisible() )
	{
		return;
	}

	//
	if (NULL == m_pMerchXml)
		return;

	// 请求该商品的实时Price数据
	if (NULL != m_pAbsCenterManager)
	{
		CMmiReqRealtimePrice Req;
		Req.m_iMarketId = m_MerchXml.m_iMarketId;
		Req.m_StrMerchCode = m_MerchXml.m_StrMerchCode;

		CMerchKey mKey;
		for ( int32 i=1; i < m_aSmartAttendMerchs.GetSize() ; i++ )	// 过滤了第一个merchxml
		{
			mKey.m_iMarketId = m_aSmartAttendMerchs[i].m_pMerch->m_MerchInfo.m_iMarketId;
			mKey.m_StrMerchCode = m_aSmartAttendMerchs[i].m_pMerch->m_MerchInfo.m_StrMerchCode;
			Req.m_aMerchMore.Add(mKey);
		}

		DoRequestViewData(Req);

		CMmiReqPublicFile	reqF10;
		reqF10.m_ePublicFileType = EPFTF10;
		reqF10.m_iMarketId		 = m_MerchXml.m_iMarketId;
		reqF10.m_StrMerchCode	 = m_MerchXml.m_StrMerchCode;
		DoRequestViewData(reqF10);

		//
		bool32 bH;
		//if ( BeGeneralIndex(bH) )
		// 沪深都是用general值
		if ( 0 == m_MerchXml.m_iMarketId 
			|| 1000 == m_MerchXml.m_iMarketId )
		{
			CMmiReqGeneralNormal ReqGeneralNormal;
			CMmiReqAddPushGeneralNormal ReqPush;
			
			bH = 0 == m_MerchXml.m_iMarketId;
			if ( bH )
			{
				ReqGeneralNormal.m_iMarketId = 0;
				ReqPush.m_iMarketId = 0;
			}
			else
			{
				ReqGeneralNormal.m_iMarketId = 1000;
				ReqPush.m_iMarketId = 1000;
			}

			//
			DoRequestViewData(ReqGeneralNormal);
			DoRequestViewData(ReqPush);
		}

		// 国内期货 - 请求分笔统计信息
		if ( IsFutureCn(m_pMerchXml) && IsNeedShowFutureCnTick() )
		{
			RequestFutureCnTicks();
		}
	}
}

//
void CIoViewPrice::OnVDataForceUpdate()
{
	if ( IsFutureCn(m_pMerchXml) )
	{
		InitFutureCnTradingDay();
	}

	RequestViewData();	
}

void CIoViewPrice::OnIoViewFontChanged()
{
	// 设置字体值		
	CIoViewBase::OnIoViewFontChanged();

	SetRowHeightAccordingFont();
	m_GridCtrl.Invalidate();
	m_GridCtrl.Refresh();
}

void CIoViewPrice::OnVDataGeneralNormalUpdate(CMerch* pMerch)
{
	// 画画
	bool32 bH;
	if ( !BeGeneralIndex(bH) )
	{
		return;
	}

	//
	RedrawWindow();
}

void CIoViewPrice::OnVDataMarketSnapshotUpdate(int32 iMarketId)
{
	if (NULL == m_pMerchXml || iMarketId != m_pMerchXml->m_MerchInfo.m_iMarketId)
		return;

	// 验证合法性
	bool32 bValid = false;
	for (int32 i = 0 ; i<m_aMarketIdArray.GetSize(); i++)
	{
		if (iMarketId == m_aMarketIdArray[i])
		{
			bValid = true;
			break;
		}
	}

	if (!bValid)
	{
		return;
	}
	
	// 计算上涨下跌家数
	int32 iRiseCounts = CalcRiseFallCounts(true);
	int32 iFallCounts = CalcRiseFallCounts(false);

	// 显示
	ShowRiseFallCounts(iRiseCounts,iFallCounts);
}




float StrTranslateToFloat(CString StrVolume)
{
	float fResult = 0.0;

	if (-1 != StrVolume.Find(L"亿"))
	{
		fResult =  fabsf(_tstof(StrVolume)) * 100000000;
	}
	else if (-1 != StrVolume.Find(L"万"))
	{
		fResult = fabsf(_tstof(StrVolume)) * 10000;
	}
	else
	{
		fResult = fabsf(_tstof(StrVolume));
	}

	// 好调试
	return fResult;
}



CString CIoViewPrice::CalculatefVolumnBuy(float fTotalValue, float fBuyNum, float fSellNum, float fRefer,bool32 bTranslate,bool32 bZeroAsHLine, bool32 bAddPercent, bool32 bShowNegative)
{
	if ( !BeValidFloat(fBuyNum) )
	{
		return L"";
	}

	CString StrFloat;
	CString StrSymbol;


	// 用实际的总数来判断符号，避免外部的内外盘传的错误
	if (0. == fBuyNum)
	{
		// 0值表示没有值， 用" -"代替
		StrFloat = L"-";
	}
	else
	{
		// 
		CString StrSellVolume = Float2SymbolString(fSellNum, -100000, 0, true);
		CString StrVolume = Float2String(fTotalValue, 0, true);

		// 统一计量单位
		float fValue = StrTranslateToFloat(StrVolume) - StrTranslateToFloat(StrSellVolume);
		StrFloat = Float2SymbolString(fValue, 100000000, 0, true);
	}


	return StrFloat;
}


void CIoViewPrice::OnVDataRealtimePriceUpdate(IN CMerch *pMerch)
{
	if ( m_bDelByBiSplitTrack )
	{
		LockRedraw();
		return;
	}
	else
	{
		UnLockRedraw();
	}

	if ( GetParentGGTongViewDragFlag() )
	{
		LockRedraw();	
		return;
	}
	else
	{
		UnLockRedraw();		
	}

	if (NULL == pMerch || NULL == m_pMerchXml)	// 不能空视图
		return;

	if(pMerch != m_pMerchXml)
	{
		return;
	}

	{
		// 如果是相关栏目更新也需要更新表格
		bool32 bMyData = false;
		for ( int32 i=0; i < m_aSmartAttendMerchs.GetSize() ; i++ )
		{
			if ( m_aSmartAttendMerchs[i].m_pMerch == pMerch )
			{
				bMyData = true;
				break;
			}
		}
		if ( !bMyData )
		{
			return;
		}
	}

	if (!::IsWindow(m_GridCtrl.GetSafeHwnd()))
	{
		return; 
	}
	
	CRealtimePrice  Realtimeprice;
	CFinanceData    FinanceData;
	CMerchExtendData ExtendData;
	int iSaveDec = m_pMerchXml->m_MerchInfo.m_iSaveDec;
	
	// 所有数据都需要使用 m_pMerchXml的数据
	CRealtimePrice * pRealtimePrice = m_pMerchXml->m_pRealtimePrice;	
	if (NULL != pRealtimePrice)
	{
		Realtimeprice = * pRealtimePrice;
	}
	

	if (NULL != m_pMerchXml->m_pFinanceData)
	{
        FinanceData = (*m_pMerchXml->m_pFinanceData);
	}
	// 
	CMerchExtendData * pExtendData = m_pMerchXml->m_pMerchExtendData;
	if (NULL != pExtendData)
	{
		ExtendData = * pExtendData;
	}
		
	// 获取昨收价
	float fPricePrevClose = Realtimeprice.m_fPricePrevClose;
	float fPricePrevAvg = Realtimeprice.m_fPricePrevAvg;
	float fPrevReferPrice = fPricePrevClose;

	//
	if ( ERTFuturesCn == m_pMerchXml->m_Market.m_MarketInfo.m_eMarketReportType )
	{
		// 仅国内期货需要昨结
		fPrevReferPrice = fPricePrevAvg;
	}
	
	// 根据不同市场类型,表格显示不同内容;
	E_ReportType eMerchKind = GetMerchKind(m_pMerchXml);	
	if (m_eMerchKind != eMerchKind)
	{		
		if (CreateTable(eMerchKind))
		{
			// 保存当前表格类型
			m_eMerchKind = eMerchKind;
		}
	}

	// 设置表格内容
	// ... 显示
	float fVolBuySum = 0.;
	float fVolSellSum = 0.;
	for (int32 i = 0; i < 5; i++)
	{
		fVolBuySum += Realtimeprice.m_astBuyPrices[i].m_fVolume;
		fVolSellSum += Realtimeprice.m_astSellPrices[i].m_fVolume;
	}

	// 收益
	if ( eMerchKind == ERTStockCn)
	{
		CString StrSeason;
		if ( 0 == Realtimeprice.m_uiSeason )
		{
			StrSeason = L"收益㈠";
		}
		else if ( 1 == Realtimeprice.m_uiSeason )
		{
			StrSeason = L"收益㈡";
		}
		else if ( 2 == Realtimeprice.m_uiSeason )
		{
			StrSeason = L"收益㈢";
		}
		else if ( 3 == Realtimeprice.m_uiSeason )
		{
			StrSeason = L"收益㈣";
		}
		else
		{
			//ASSERT(0);
		}

		//		
		CGridCellSys* pCell = (CGridCellSys *)m_GridCtrl.GetCell(18, 0);
		if ( NULL != pCell )
		{
			CString StrTextNow  = pCell->GetText();
			if ( StrTextNow != StrSeason )
			{
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
 				pCell->SetText(StrSeason);
			}			
		}		
	}

	bool32 bH;
	bool32 bRedGreenExp = BeGeneralIndex(bH);

	LOGFONT fontNumber;
	memset(&fontNumber, 0, sizeof(fontNumber));
	_tcscpy(fontNumber.lfFaceName, gFontFactory.GetExistFontName(_T("Arial")));///
	fontNumber.lfHeight  = -14;
	fontNumber.lfWeight = 560;

	for (int i =0; i < m_GridCtrl.GetRowCount(); ++i)
	{
		for (int j =0; j < m_GridCtrl.GetColumnCount(); ++j)
		{				
			CGridCellBase* pCell = m_GridCtrl.GetCell(i, j);
			if (NULL != pCell)
			{
				if (0 == j)
				{
					pCell->SetTextPadding(CRect(15,0,0,0));
				}

				if(2 == j)
				{
					pCell->SetTextPadding(CRect(40,0,0,0));
				}

				if (m_GridCtrl.GetColumnCount()- 1 == j)
				{
					pCell->SetTextPadding(CRect(0,0,15,0));
				}
			}
		}
	}

	switch( eMerchKind )
	{
	case ERTStockCn:
	case ERTStockHk:	
		{
			/////////////////////////////////////////////////
			// 委比	
			m_GridCtrl.SetCellType(0, 1, RUNTIME_CLASS(CGridCellSymbol));
			CGridCellSymbol *pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(0, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

			float fBidRatio = 0.;
			if (fVolBuySum + fVolSellSum != 0.)
				fBidRatio = (fVolBuySum - fVolSellSum) / (fVolBuySum + fVolSellSum) * 100;
			CString StrBidRatio = Float2SymbolString(fBidRatio, 0, 2);
			StrBidRatio += L"%";
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrBidRatio);
			
			/////////////////////////////////////////////////
			//  卖5价
			m_GridCtrl.SetCellType(1, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(1, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrPriceSell5 = Float2SymbolString( Realtimeprice.m_astSellPrices[4].m_fPrice, fPrevReferPrice, iSaveDec);	
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceSell5);
			
			
			/////////////////////////////////////////////////
			//  卖4
			m_GridCtrl.SetCellType(2, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(2, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrPriceSell4 = Float2SymbolString(Realtimeprice.m_astSellPrices[3].m_fPrice, fPrevReferPrice, iSaveDec);	
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceSell4);
			
			/////////////////////////////////////////////////
			//  卖3
			m_GridCtrl.SetCellType(3, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(3, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrPriceSell3 = Float2SymbolString(Realtimeprice.m_astSellPrices[2].m_fPrice, fPrevReferPrice, iSaveDec);
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceSell3);
			
			/////////////////////////////////////////////////
			//  卖2
			m_GridCtrl.SetCellType(4, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(4, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrPriceSell2 = Float2SymbolString(Realtimeprice.m_astSellPrices[1].m_fPrice, fPrevReferPrice, iSaveDec);	
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceSell2);
			
			/////////////////////////////////////////////////
			//  卖1
			m_GridCtrl.SetCellType(5, 1, RUNTIME_CLASS(CGridCellSymbol));
		    pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(5, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrPriceSell1 = Float2SymbolString(Realtimeprice.m_astSellPrices[0].m_fPrice, fPrevReferPrice, iSaveDec);
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceSell1);
						
			/////////////////////////////////////////////////
			//  买1
			m_GridCtrl.SetCellType(6, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(6, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrPriceBuy1 = Float2SymbolString(Realtimeprice.m_astBuyPrices[0].m_fPrice , fPrevReferPrice, iSaveDec);
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceBuy1);
		
			/////////////////////////////////////////////////
			//  买2
			m_GridCtrl.SetCellType(7, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(7, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrPriceBuy2 = Float2SymbolString(Realtimeprice.m_astBuyPrices[1].m_fPrice , fPrevReferPrice, iSaveDec);
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceBuy2);
			
			/////////////////////////////////////////////////
			//  买3
			m_GridCtrl.SetCellType(8, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(8, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrPriceBuy3 = Float2SymbolString(Realtimeprice.m_astBuyPrices[2].m_fPrice , fPrevReferPrice, iSaveDec);	
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceBuy3);
			
			/////////////////////////////////////////////////
			//  买4
			m_GridCtrl.SetCellType(9, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(9, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrPriceBuy4 = Float2SymbolString(Realtimeprice.m_astBuyPrices[3].m_fPrice , fPrevReferPrice, iSaveDec);	
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceBuy4);
			
			/////////////////////////////////////////////////
			//  买5 价
			m_GridCtrl.SetCellType(10, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(10, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrPriceBuy5 = Float2SymbolString(Realtimeprice.m_astBuyPrices[4].m_fPrice , fPrevReferPrice, iSaveDec);	
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceBuy5);
			
			/////////////////////////////////////////////////
			//  现价,最新价
			m_GridCtrl.SetCellType(11, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(11, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrPriceNow = Float2SymbolString( Realtimeprice.m_fPriceNew , fPrevReferPrice, iSaveDec);
			//StrPriceNow.Append(L"  ");
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceNow);
			
			/////////////////////////////////////////////////
			//  涨跌
			m_GridCtrl.SetCellType(12, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(12, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCellSymbol->SetFont(&fontNumber);
			
			CString StrRiseValue = L" -";
			if (0. != Realtimeprice.m_fPriceNew)
			{
				StrRiseValue = Float2SymbolString(Realtimeprice.m_fPriceNew - fPrevReferPrice, 0, iSaveDec, false, false);	
			}
			//StrRiseValue.Append(L"  ");
			pCellSymbol->SetText(StrRiseValue);
			
			/////////////////////////////////////////////////
			//  涨幅
			m_GridCtrl.SetCellType(13, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(13, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCellSymbol->SetFont(&fontNumber);
					
			CString StrPriceRisePercent = L" -";
			if (0. != Realtimeprice.m_fPriceNew && 0. != fPrevReferPrice)
			{
				float fRisePercent = ((Realtimeprice.m_fPriceNew - fPrevReferPrice) / fPrevReferPrice) * 100.;
				StrPriceRisePercent = Float2SymbolString(fRisePercent, 0, 2, false, false, true);
			}
			//StrPriceRisePercent.Append(L"  ");
			pCellSymbol->SetText(StrPriceRisePercent);
			
			
			/////////////////////////////////////////////////
			// 总量, 黄色
			CGridCellSys * pCell = (CGridCellSys *)m_GridCtrl.GetCell(14, 1);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCVolume);
			CString StrVolume;	
			StrVolume = Float2String( Realtimeprice.m_fVolumeTotal, 0, true);
			//StrVolume.Append(L"  ");
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolume);
			
			/////////////////////////////////////////////////
			//  外盘,内盘一定是描述为涨， 外盘一定描述为跌,一个跟一个超小值比， 一个跟超大值比,显示不同颜色
			m_GridCtrl.SetCellType(15, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(15, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);		
			// 增加修改项，去掉小数
			CString StrSellVolume = Float2SymbolString(Realtimeprice.m_fSellVolume,-100000,0, true);
			//StrSellVolume.Append(L"  ");
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrSellVolume);    

			/////////////////////////////////////////////////
			//  换手, 灰色
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(16, 1);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCText);
			CString StrVolumeTRate = Float2String( Realtimeprice.m_fTradeRate , 2, true);
			//pCell->SetText(L"fan换手");
			StrVolumeTRate += L"%";
			//StrVolumeTRate.Append(L"  ");
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolumeTRate);
			
			/////////////////////////////////////////////////
			//  净资, 灰色
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(17, 1);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCText);
			CString StrVolumePure = Float2String( FinanceData.m_fPerPureAsset , 2, true);
			//StrVolumePure.Append(L"  ");
			//pCell->SetText(L"fan净资");
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolumePure);

			/////////////////////////////////////////////////
			//  收益, 灰色
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(18, 1);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCText);
			CString StrVolumeIncome;// = Float2String( FinanceData.m_fPerIncomeYear , 3, true);
			Float2StringLimitMaxLength(StrVolumeIncome, FinanceData.m_fPerIncomeYear, 3, 6, true, true);
			//StrVolumeIncome.Append(L"  ");
			//pCell->SetText(L"fan收益");
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolumeIncome);
			
			/////////////////////////////////////////////////
			//  委差
			m_GridCtrl.SetCellType(0, 3, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(0, 3);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrBidDifference = Float2SymbolString(fVolBuySum - fVolSellSum, 0, 0);	
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrBidDifference);
			
			/////////////////////////////////////////////////
			//  卖5, 黄色 , 量
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(1, 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCVolume);
			CString StrVolumeSell5 = Float2String( Realtimeprice.m_astSellPrices[4].m_fVolume, 0, false);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolumeSell5);
			
			/////////////////////////////////////////////////
			//  卖4, 黄色
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(2, 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCVolume);
			CString StrVolumeSell4 = Float2String( Realtimeprice.m_astSellPrices[3].m_fVolume, 0, false);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolumeSell4);
			
			/////////////////////////////////////////////////
			//  卖3, 黄色
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(3, 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCVolume);
			CString StrVolumeSell3 = Float2String( Realtimeprice.m_astSellPrices[2].m_fVolume, 0, false);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolumeSell3);
			
			/////////////////////////////////////////////////
			//  卖2, 黄色
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(4, 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCVolume);
			CString StrVolumeSell2 = Float2String( Realtimeprice.m_astSellPrices[1].m_fVolume, 0, false);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolumeSell2);
			
			/////////////////////////////////////////////////
			//  卖1, 黄色
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(5, 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCVolume);
			CString StrVolumeSell1 = Float2String( Realtimeprice.m_astSellPrices[0].m_fVolume, 0, false);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolumeSell1);
			
			/////////////////////////////////////////////////
			//  买1, 黄色
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(6, 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCVolume);
			CString StrVolumeBuy1 = Float2String( Realtimeprice.m_astBuyPrices[0].m_fVolume, 0, false);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolumeBuy1);
			
			/////////////////////////////////////////////////
			//  买2, 黄色
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(7, 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCVolume);
			CString StrVolumeBuy2 = Float2String( Realtimeprice.m_astBuyPrices[1].m_fVolume, 0, false);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolumeBuy2);
			
			/////////////////////////////////////////////////
			//  买3, 黄色
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(8, 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCVolume);
			CString StrVolumeBuy3 = Float2String( Realtimeprice.m_astBuyPrices[2].m_fVolume, 0, false);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolumeBuy3);
			
			/////////////////////////////////////////////////
			//  买4, 黄色
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(9, 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCVolume);
			CString StrVolumeBuy4 = Float2String( Realtimeprice.m_astBuyPrices[3].m_fVolume, 0, false);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolumeBuy4);
			
			/////////////////////////////////////////////////
			//  买5, 黄色 量
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(10, 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCVolume);
			CString StrVolumeBuy5 = Float2String( Realtimeprice.m_astBuyPrices[4].m_fVolume, 0, false);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolumeBuy5);
			
			/////////////////////////////////////////////////
			//  今开价
			m_GridCtrl.SetCellType(11, 3, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(11, 3);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrPriceOpen = Float2SymbolString( Realtimeprice.m_fPriceOpen , fPrevReferPrice, iSaveDec);	
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceOpen);
			
			/////////////////////////////////////////////////
			//  最高价
			m_GridCtrl.SetCellType(12, 3, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(12, 3);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrPriceHigh = Float2SymbolString( Realtimeprice.m_fPriceHigh , fPrevReferPrice, iSaveDec);	
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceHigh);
			
			/////////////////////////////////////////////////
			//  最低价 
 			m_GridCtrl.SetCellType(13, 3, RUNTIME_CLASS(CGridCellSymbol));
 			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(13, 3);
 			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
 			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
 			CString StrPriceLow = Float2SymbolString( Realtimeprice.m_fPriceLow , fPrevReferPrice, iSaveDec);
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceLow);
			
			/////////////////////////////////////////////////
			//  量比, 灰色
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(14, 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCText);
			CString StrVolumeRate = Float2String(Realtimeprice.m_fVolumeRate, 2, false);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolumeRate);
			
			/////////////////////////////////////////////////
			//  内盘,内盘一定是描述为涨， 外盘一定描述为跌,一个跟一个超小值比， 一个跟超大值比,显示不同颜色
			
			m_GridCtrl.SetCellType(15, 3, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(15, 3);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCellSymbol->SetFont(&fontNumber);
			
			// 内盘为总量减去外盘
			CString StrBuyVolumn = CalculatefVolumnBuy(Realtimeprice.m_fVolumeTotal, Realtimeprice.m_fBuyVolume,Realtimeprice.m_fSellVolume, 100000000, true);
			pCellSymbol->SetText(StrBuyVolumn);
			
			/////////////////////////////////////////////////
			//  股本, 灰色
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(16, 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCText);
			float fValueAll = FinanceData.m_fAllCapical;
			//fValueAll /= 10000.0;
			CString StrVolumeAll; // = Float2String(  fValueAll, 0, true);
			Float2StringLimitMaxLength(StrVolumeAll, fValueAll, 0, 6, true, true);
			//pCell->SetText(L"fan 股本");
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolumeAll);
			
			/////////////////////////////////////////////////
			//  流通, 灰色
		
 			pCell = (CGridCellSys *)m_GridCtrl.GetCell(17, 3);
 			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
 			pCell->SetDefaultTextColor(ESCText);
			float fValueCir = FinanceData.m_fCircAsset;
			//fValueCir /= 10000.0;
 			CString StrVolumeCir;// = Float2String( fValueCir , 0, true);
			Float2StringLimitMaxLength(StrVolumeCir, fValueCir, 0, 6, true, true);
 			//pCell->SetText(L"fan流通");
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolumeCir);

			/////////////////////////////////////////////////
			//  市盈, 灰色
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(18, 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCText);
			CString StrVolumePeRate; // = Float2String( Realtimeprice.m_fPeRate , 2, true);
			Float2StringLimitMaxLength(StrVolumePeRate, Realtimeprice.m_fPeRate, 2, 6, true, true);
			//pCell->SetText(L"fan市盈");
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolumePeRate);

			for (int32 i = 0; i < m_GridCtrl.GetColumnCount(); i++)
			{
				CGridCellSys * pItemCell = (CGridCellSys *)m_GridCtrl.GetCell(0,i);
				pItemCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
				
				pItemCell = (CGridCellSys *)m_GridCtrl.GetCell(5,i);
				pItemCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
				
				pItemCell = (CGridCellSys *)m_GridCtrl.GetCell(10,i);
				pItemCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
				
				pItemCell = (CGridCellSys *)m_GridCtrl.GetCell(14,i);
				pItemCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
				
				pItemCell = (CGridCellSys *)m_GridCtrl.GetCell(15,i);
				pItemCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
				
				pItemCell = (CGridCellSys *)m_GridCtrl.GetCell(18,i);
				pItemCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
			}
		}		
		break;
	case ERTExp:
	case ERTExpForeign:
		{
			if ( m_aExpMerchs.GetSize() > 0 )
			{
				// 新增7个字段 = 11 + 7
				//ASSERT( m_aExpMerchs.GetSize() + 11 == m_GridCtrl.GetRowCount() );
				int32 iRow = 0;
				CGridCellSymbol *pCellSymbol = NULL;
				for ( iRow=0; iRow < m_aExpMerchs.GetSize() ; iRow++ )
				{
					float fValue = GetAppendExpAmout(m_aExpMerchs[iRow]);
					CGridCellSys * 	pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 1);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCell->SetDefaultTextColor(ESCVolume);			
					CString StrAmount = Float2String(fValue, 0, true);
					pCell->SetFont(&fontNumber);
					pCell->SetText(StrAmount);
				}


				// 原来的
				m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow++, 1);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);			
				CString   StrPriceNew   = Float2SymbolString(Realtimeprice.m_fPriceNew,fPrevReferPrice,iSaveDec);
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPriceNew);
				
				m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow++, 1);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				CString		StrPriceOpen = Float2SymbolString(Realtimeprice.m_fPriceOpen,fPrevReferPrice,iSaveDec);		
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPriceOpen);
				
				m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow++, 1);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);			
				CString		StrPriceClose = Float2SymbolString(Realtimeprice.m_fPricePrevClose,fPrevReferPrice,iSaveDec);
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPriceClose);
				
				m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow++, 1);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				CString		StrRiseFall = Float2SymbolString( (Realtimeprice.m_fPriceNew - fPrevReferPrice),0,iSaveDec);
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrRiseFall);
				
				// 
				m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow++, 1);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				
				float fRisePercent = 0.;
				if (0. != fPrevReferPrice)
				{
					fRisePercent = ((Realtimeprice.m_fPriceNew - fPrevReferPrice) / fPrevReferPrice) * 100.;
				}
				
				CString StrPriceRisePercent = Float2SymbolString(fRisePercent, 0, 2,false,true,true);
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPriceRisePercent);
				
				// 
				CGridCellSys * 	pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow++, 1);
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				pCell->SetDefaultTextColor(ESCVolume);
				CString StrVolume = Float2String(Realtimeprice.m_fVolumeTotal, 0, true);
				pCell->SetFont(&fontNumber);
				pCell->SetText(StrVolume);
				
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow++, 1);
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				pCell->SetDefaultTextColor(ESCVolume);			
				CString StrAmount = Float2String(Realtimeprice.m_fAmountTotal,0,true);
				pCell->SetFont(&fontNumber);
				pCell->SetText(StrAmount);
				
				m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow++, 1);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				CString StrPriceHigh = Float2SymbolString( Realtimeprice.m_fPriceHigh , fPrevReferPrice, iSaveDec);	
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPriceHigh);
				
				m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow++, 1);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				CString StrPriceLow = Float2SymbolString( Realtimeprice.m_fPriceLow , fPrevReferPrice, iSaveDec);
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPriceLow);
				
				int32 i = 0;
				for ( i = 0; i < m_GridCtrl.GetColumnCount(); i++)
				{
					const int32 iAppend = m_aExpMerchs.GetSize();

					CGridCellSys * pTmpCell = (CGridCellSys *)m_GridCtrl.GetCell(iAppend-1, i);
					pTmpCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
					
					pTmpCell = (CGridCellSys *)m_GridCtrl.GetCell(8 + iAppend, i);
					pTmpCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
					
					if ( !bRedGreenExp )
					{
						pTmpCell = (CGridCellSys *)m_GridCtrl.GetCell(9 + iAppend, i);	// 9上涨, 10下跌
						pTmpCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
						
						pTmpCell = (CGridCellSys *)m_GridCtrl.GetCell(10 + iAppend, i);
						pTmpCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
					}
  				}
				for ( i=0; i < m_GridExpRiseFall.GetColumnCount() ; i++ )
				{
					pCell = (CGridCellSys *)m_GridExpRiseFall.GetCell(0, i);	// 0,0上涨, 0,2下跌
					pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
				}
			}
			else
			{
				m_GridCtrl.SetCellType(0, 1, RUNTIME_CLASS(CGridCellSymbol));
				CGridCellSymbol *pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(0, 1);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);			
				CString   StrPriceNew   = Float2SymbolString(Realtimeprice.m_fPriceNew,fPrevReferPrice,iSaveDec);
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPriceNew);
				
				m_GridCtrl.SetCellType(1, 1, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(1, 1);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				CString		StrPriceOpen = Float2SymbolString(Realtimeprice.m_fPriceOpen,fPrevReferPrice,iSaveDec);		
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPriceOpen);
				
				m_GridCtrl.SetCellType(2, 1, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(2, 1);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);			
				CString		StrPriceClose = Float2SymbolString(Realtimeprice.m_fPricePrevClose,fPrevReferPrice,iSaveDec);
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPriceClose);
				
				m_GridCtrl.SetCellType(3, 1, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(3, 1);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				CString		StrRiseFall = Float2SymbolString( (Realtimeprice.m_fPriceNew - fPrevReferPrice),0,iSaveDec);
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrRiseFall);
				
				// 
				m_GridCtrl.SetCellType(4, 1, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(4, 1);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				
				float fRisePercent = 0.;
				if (0. != fPrevReferPrice)
				{
					fRisePercent = ((Realtimeprice.m_fPriceNew - fPrevReferPrice) / fPrevReferPrice) * 100.;
				}
				
				CString StrPriceRisePercent = Float2SymbolString(fRisePercent, 0, 2,false,true,true);
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPriceRisePercent);
				
				// 
				CGridCellSys * 	pCell = (CGridCellSys *)m_GridCtrl.GetCell(5, 1);
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				pCell->SetDefaultTextColor(ESCVolume);
				CString StrVolume = Float2String(Realtimeprice.m_fVolumeTotal, 0, true);
				pCell->SetFont(&fontNumber);
				pCell->SetText(StrVolume);
				
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(6, 1);
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				pCell->SetDefaultTextColor(ESCVolume);			
				CString StrAmount = Float2String(Realtimeprice.m_fAmountTotal,0,true);
				pCell->SetFont(&fontNumber);
				pCell->SetText(StrAmount);
				
				m_GridCtrl.SetCellType(7, 1, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(7, 1);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				CString StrPriceHigh = Float2SymbolString( Realtimeprice.m_fPriceHigh , fPrevReferPrice, iSaveDec);	
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPriceHigh);
				
				m_GridCtrl.SetCellType(8, 1, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(8, 1);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				CString StrPriceLow = Float2SymbolString( Realtimeprice.m_fPriceLow , fPrevReferPrice, iSaveDec);	
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPriceLow);
				
				int32 i = 0;
				for ( i = 0;i<m_GridCtrl.GetColumnCount(); i++)
				{
					CGridCellSys * pCurCell = (CGridCellSys *)m_GridCtrl.GetCell(0, i);
					pCurCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
					
					pCurCell = (CGridCellSys *)m_GridCtrl.GetCell(8, i);
					pCurCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
					
					if ( !bRedGreenExp )
					{
						pCurCell = (CGridCellSys *)m_GridCtrl.GetCell(9, i);	// 9上涨, 10下跌
						pCurCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
						
						pCurCell = (CGridCellSys *)m_GridCtrl.GetCell(10, i);
						pCurCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
					}
  				}
				for ( i=0; i < m_GridExpRiseFall.GetColumnCount() ; i++ )
				{
					pCell = (CGridCellSys *)m_GridExpRiseFall.GetCell(0, i);	// 0,0上涨, 0,2下跌
					pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
				}
			}
		}
		break;
	case ERTWarrantCn:
	case ERTWarrantHk:
		{
			/////////////////////////////////////////////////
			// 委比 
			m_GridCtrl.SetCellType(0, 1, RUNTIME_CLASS(CGridCellSymbol));
			CGridCellSymbol *pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(0, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			
			//			
			float fRate = 0.0;
			int32 iBuyVolums  = 0;
			int32 iSellVolums = 0;
			
			int32 i = 0;
			for ( i = 0 ; i < 5 ; i++)
			{
				iBuyVolums  += (int32)(Realtimeprice.m_astBuyPrices[i].m_fVolume);
				iSellVolums += (int32)(Realtimeprice.m_astSellPrices[i].m_fVolume);
			}
			
			if ( 0 != (iBuyVolums + iSellVolums) )
			{
				fRate = (float)(iBuyVolums - iSellVolums) / (iBuyVolums + iSellVolums);
			}
			
			CString StrRate = Float2SymbolString(fRate, 0.0, 2, false, true, true);
			
			//
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrRate);
			
			/////////////////////////////////////////////////
			//  卖5 价
			m_GridCtrl.SetCellType(1, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(1, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrPriceSell5 = Float2SymbolString( Realtimeprice.m_astSellPrices[4].m_fPrice, fPrevReferPrice, iSaveDec);	
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceSell5);
			
			
			/////////////////////////////////////////////////
			//  卖4
			m_GridCtrl.SetCellType(2, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(2, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrPriceSell4 = Float2SymbolString(Realtimeprice.m_astSellPrices[3].m_fPrice, fPrevReferPrice, iSaveDec);	
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceSell4);
			
			/////////////////////////////////////////////////
			//  卖3
			m_GridCtrl.SetCellType(3, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(3, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrPriceSell3 = Float2SymbolString(Realtimeprice.m_astSellPrices[2].m_fPrice, fPrevReferPrice, iSaveDec);	
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceSell3);
			
			/////////////////////////////////////////////////
			//  卖2
			m_GridCtrl.SetCellType(4, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(4, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrPriceSell2 = Float2SymbolString(Realtimeprice.m_astSellPrices[1].m_fPrice, fPrevReferPrice, iSaveDec);	
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceSell2);
			
			/////////////////////////////////////////////////
			//  卖1
			m_GridCtrl.SetCellType(5, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(5, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrPriceSell1 = Float2SymbolString(Realtimeprice.m_astSellPrices[0].m_fPrice, fPrevReferPrice, iSaveDec);	
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceSell1);
			
			/////////////////////////////////////////////////
			//  买1
			m_GridCtrl.SetCellType(6, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(6, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrPriceBuy1 = Float2SymbolString(Realtimeprice.m_astBuyPrices[0].m_fPrice , fPrevReferPrice, iSaveDec);	
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceBuy1);
			
			/////////////////////////////////////////////////
			//  买2
			m_GridCtrl.SetCellType(7, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(7, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrPriceBuy2 = Float2SymbolString(Realtimeprice.m_astBuyPrices[1].m_fPrice , fPrevReferPrice, iSaveDec);	
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceBuy2);
			
			/////////////////////////////////////////////////
			//  买3
			m_GridCtrl.SetCellType(8, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(8, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrPriceBuy3 = Float2SymbolString(Realtimeprice.m_astBuyPrices[2].m_fPrice , fPrevReferPrice, iSaveDec);	
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceBuy3);
			
			/////////////////////////////////////////////////
			//  买4
			m_GridCtrl.SetCellType(9, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(9, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrPriceBuy4 = Float2SymbolString(Realtimeprice.m_astBuyPrices[3].m_fPrice , fPrevReferPrice, iSaveDec);	
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceBuy4);
			
			/////////////////////////////////////////////////
			//  买5 价
			m_GridCtrl.SetCellType(10, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(10, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrPriceBuy5 = Float2SymbolString(Realtimeprice.m_astBuyPrices[4].m_fPrice , fPrevReferPrice, iSaveDec);	
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceBuy5);
			
			/////////////////////////////////////////////////
			//  现价,最新价
			m_GridCtrl.SetCellType(11, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(11, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrPriceNow = Float2SymbolString( Realtimeprice.m_fPriceNew , fPrevReferPrice, iSaveDec);	
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceNow);
			
			/////////////////////////////////////////////////
			//  涨跌
			m_GridCtrl.SetCellType(12, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(12, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

			CString StrRiseValue = L" -";
			if (0. != Realtimeprice.m_fPriceNew)
			{
				StrRiseValue = Float2SymbolString(Realtimeprice.m_fPriceNew - fPrevReferPrice, 0, iSaveDec, false, false);	
			}
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrRiseValue);
			
			
			/////////////////////////////////////////////////
			//  涨幅
			m_GridCtrl.SetCellType(13, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(13, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			
			CString StrPriceRisePercent = L" -";
			if (0. != Realtimeprice.m_fPriceNew && 0. != fPrevReferPrice)
			{
				float fRisePercent = ((Realtimeprice.m_fPriceNew - fPrevReferPrice) / fPrevReferPrice) * 100.;
				StrPriceRisePercent = Float2SymbolString(fRisePercent, 0, 2, false, false, true);
			}
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceRisePercent);
			
			/////////////////////////////////////////////////
			//  总量, 黄色
			CGridCellSys * pCell = (CGridCellSys *)m_GridCtrl.GetCell(14, 1);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCVolume);
			CString StrVolume;
			StrVolume = Float2String( Realtimeprice.m_fVolumeTotal, 0, true);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolume);
			
			/////////////////////////////////////////////////
			//  外盘,内盘一定是描述为涨， 外盘一定描述为跌,一个跟一个超小值比， 一个跟超大值比,显示不同颜色
			m_GridCtrl.SetCellType(15, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(15, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrSellVolume = Float2SymbolString(Realtimeprice.m_fSellVolume, -100000, 0, true);
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrSellVolume);   
			
			/////////////////////////////////////////////////
			//  涨跌
			m_GridCtrl.SetCellType(16, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(16, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

			StrRiseValue = L" -";
			if (0. != Realtimeprice.m_fPriceNew)
			{
				StrRiseValue = Float2SymbolString(Realtimeprice.m_fPriceNew - fPrevReferPrice, 0, iSaveDec, false, false);	
			}
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrRiseValue);
			
			
			/////////////////////////////////////////////////
			//  换手, 灰色
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(17, 1);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCText);
			//CString StrVolume = Float2String( Realtimeprice.m_fVolumeTotal , 0, true);
			//pCell->SetText(L"fan换手");
			pCell->SetFont(&fontNumber);
			pCell->SetText(L"-");
			
			/////////////////////////////////////////////////
			//  价值, 灰色
//			pCell = (CGridCellSys *)m_GridCtrl.GetCell(18, 1);
//			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
//			pCell->SetDefaultTextColor(ESCText);
//			//CString StrVolume = Float2String( Realtimeprice.m_fVolumeTotal , 0, true);
//			pCell->SetText(L"fan价值");
			/////////////////////////////////////////////////
			//  行权日
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(19, 1);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCText);
			//CString StrVolume = Float2String( Realtimeprice.m_fVolumeTotal , 0, true);
			//pCell->SetText(L"fan 行权日");
			pCell->SetFont(&fontNumber);
			pCell->SetText(L"-");	
			/////////////////////////////////////////////////
			//  行权比
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(20, 1);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCText);
			//CString StrVolume = Float2String( Realtimeprice.m_fVolumeTotal , 0, true);
			//pCell->SetText(L"fan 行权比");
			pCell->SetFont(&fontNumber);
			pCell->SetText(L"-");
			/////////////////////////////////////////////////
			//  委差
			m_GridCtrl.SetCellType(0, 3, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(0, 3);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			//CString StrPriceNow = Float2SymbolString( Realtimeprice.m_fPriceNew , fPrevReferPrice, iSaveDec);	
			//pCellSymbol->SetText(L"fan委差");
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(L"-");
			/////////////////////////////////////////////////
			//  卖5, 黄色 , 量
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(1, 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCVolume);
			CString StrVolumeSell5 = Float2String( Realtimeprice.m_astSellPrices[4].m_fVolume, 0, false);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolumeSell5);
			
			/////////////////////////////////////////////////
			//  卖4, 黄色
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(2, 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCVolume);
			CString StrVolumeSell4 = Float2String( Realtimeprice.m_astSellPrices[3].m_fVolume, 0, false);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolumeSell4);
			
			/////////////////////////////////////////////////
			//  卖3, 黄色
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(3, 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCVolume);
			CString StrVolumeSell3 = Float2String( Realtimeprice.m_astSellPrices[2].m_fVolume, 0, false);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolumeSell3);
			
			/////////////////////////////////////////////////
			//  卖2, 黄色
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(4, 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCVolume);
			CString StrVolumeSell2 = Float2String( Realtimeprice.m_astSellPrices[1].m_fVolume, 0, false);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolumeSell2);
			
			/////////////////////////////////////////////////
			//  卖1, 黄色
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(5, 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCVolume);
			CString StrVolumeSell1 = Float2String( Realtimeprice.m_astSellPrices[0].m_fVolume, 0, false);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolumeSell1);
			
			/////////////////////////////////////////////////
			//  买1, 黄色
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(6, 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCVolume);
			CString StrVolumeBuy1 = Float2String( Realtimeprice.m_astBuyPrices[0].m_fVolume, 0, false);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolumeBuy1);
			
			/////////////////////////////////////////////////
			//  买2, 黄色
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(7, 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCVolume);
			CString StrVolumeBuy2 = Float2String( Realtimeprice.m_astBuyPrices[1].m_fVolume, 0, false);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolumeBuy2);
			
			/////////////////////////////////////////////////
			//  买3, 黄色
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(8, 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCVolume);
			CString StrVolumeBuy3 = Float2String( Realtimeprice.m_astBuyPrices[2].m_fVolume, 0, false);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolumeBuy3);
			
			/////////////////////////////////////////////////
			//  买4, 黄色
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(9, 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCVolume);
			CString StrVolumeBuy4 = Float2String( Realtimeprice.m_astBuyPrices[3].m_fVolume, 0, false);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolumeBuy4);
			
			/////////////////////////////////////////////////
			//  买5, 黄色 量
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(10, 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCVolume);
			CString StrVolumeBuy5 = Float2String( Realtimeprice.m_astBuyPrices[4].m_fVolume, 0, false);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolumeBuy5);
			
			/////////////////////////////////////////////////
			//  今开价
			m_GridCtrl.SetCellType(11, 3, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(11, 3);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrPriceOpen = Float2SymbolString( Realtimeprice.m_fPriceOpen , fPrevReferPrice, iSaveDec);	
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceOpen);
			
			/////////////////////////////////////////////////
			//  最高价
			m_GridCtrl.SetCellType(12, 3, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(12, 3);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrPriceHigh = Float2SymbolString( Realtimeprice.m_fPriceHigh , fPrevReferPrice, iSaveDec);	
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceHigh);
			
			/////////////////////////////////////////////////
			//  最低价
			m_GridCtrl.SetCellType(13, 3, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(13, 3);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrPriceLow = Float2SymbolString( Realtimeprice.m_fPriceLow , fPrevReferPrice, iSaveDec);	
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPriceLow);
			
			/////////////////////////////////////////////////
			//  量比, 灰色
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(14, 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCText);
			CString StrVolumeRate = Float2String(Realtimeprice.m_fVolumeRate, 2, false);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolumeRate);

			/////////////////////////////////////////////////
			//  内盘,内盘一定是描述为涨， 外盘一定描述为跌,一个跟一个超小值比， 一个跟超大值比,显示不同颜色
			
			m_GridCtrl.SetCellType(15, 3, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(15, 3);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			// 内盘为总量减去外盘	
			CString StrBuyVolumn = CalculatefVolumnBuy(Realtimeprice.m_fVolumeTotal, Realtimeprice.m_fBuyVolume,Realtimeprice.m_fSellVolume, 100000000, true);
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrBuyVolumn);

			/////////////////////////////////////////////////
			//  股本, 灰色
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(16, 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCText);
			//CString StrVolume = Float2String( Realtimeprice.m_fVolumeTotal , 0, true);
			//pCell->SetText(L"fan跌停");
			pCell->SetFont(&fontNumber);
			pCell->SetText(L"-");

			/////////////////////////////////////////////////
			//  流通, 灰色
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(17, 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCText);
			//CString StrVolume = Float2String( Realtimeprice.m_fVolumeTotal , 0, true);
			//pCell->SetText(L"fan杠杆");
			pCell->SetFont(&fontNumber);
			pCell->SetText(L"-");
			
			/////////////////////////////////////////////////
			//  收益, 灰色
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(18, 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			pCell->SetDefaultTextColor(ESCText);
			//CString StrVolume = Float2String( Realtimeprice.m_fVolumeTotal , 0, true);
			//pCell->SetText(L"fan溢价");
			pCell->SetFont(&fontNumber);
			pCell->SetText(L"-");


			/////////////////////////////////////////////////
			//  认购,认沽			
			m_GridCtrl.SetCellType(19, 3, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(19, 3);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			//CString StrPriceNow = Float2SymbolString( Realtimeprice.m_fPriceNew , fPrevReferPrice, iSaveDec,true);最后一个参数,是否自动转换单位	
			//pCellSymbol->SetText(L"fan认购/认沽");
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(L"-");


			/////////////////////////////////////////////////
			//  行权价			
			m_GridCtrl.SetCellType(20, 3, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(20, 3);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			//CString StrPriceNow = Float2SymbolString( Realtimeprice.m_fPriceNew , fPrevReferPrice, iSaveDec,true);最后一个参数,是否自动转换单位	
			//pCellSymbol->SetText(L"fan行权价");
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(L"-");

 
 			for (i = 0;i<m_GridCtrl.GetColumnCount(); i++)
 			{
 				CGridCellSys * pItemCell = (CGridCellSys *)m_GridCtrl.GetCell(0,i);
 				pItemCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
 				
 				pItemCell = (CGridCellSys *)m_GridCtrl.GetCell(5,i);
 				pItemCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
 				
 				pItemCell = (CGridCellSys *)m_GridCtrl.GetCell(10,i);
 				pItemCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
 				
 				pItemCell = (CGridCellSys *)m_GridCtrl.GetCell(14,i);
 				pItemCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
 				
 				pItemCell = (CGridCellSys *)m_GridCtrl.GetCell(15,i);
 				pItemCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
 				
 				pItemCell = (CGridCellSys *)m_GridCtrl.GetCell(18,i);
 				pItemCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);

				pItemCell = (CGridCellSys *)m_GridCtrl.GetCell(20,i);
 				pItemCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
 			}
		}  
		break;
	case ERTFuturesCn:
	case ERTFuturesForeign:
	case ERTFutureRelaMonth:
	case ERTFutureSpot:
	case ERTFutureGold:
	case ERTMony:
		{
			if ( BeSpecial() )
			{
				/////////////////////////////////////////////////
				//  卖5价
				m_GridCtrl.SetCellType(0, 1, RUNTIME_CLASS(CGridCellSymbol));
				CGridCellSymbol* pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(0, 1);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				CString StrPriceSell5 = Float2SymbolString( Realtimeprice.m_astSellPrices[4].m_fPrice, fPrevReferPrice, iSaveDec);	
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPriceSell5);				
				
				/////////////////////////////////////////////////
				//  卖4
				m_GridCtrl.SetCellType(1, 1, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(1, 1);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				CString StrPriceSell4 = Float2SymbolString(Realtimeprice.m_astSellPrices[3].m_fPrice, fPrevReferPrice, iSaveDec);	
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPriceSell4);
				
				/////////////////////////////////////////////////
				//  卖3
				m_GridCtrl.SetCellType(2, 1, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(2, 1);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				CString StrPriceSell3 = Float2SymbolString(Realtimeprice.m_astSellPrices[2].m_fPrice, fPrevReferPrice, iSaveDec);	
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPriceSell3);
				
				/////////////////////////////////////////////////
				//  卖2
				m_GridCtrl.SetCellType(3, 1, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(3, 1);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				CString StrPriceSell2 = Float2SymbolString(Realtimeprice.m_astSellPrices[1].m_fPrice, fPrevReferPrice, iSaveDec);	
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPriceSell2);
				
				/////////////////////////////////////////////////
				//  卖1
				m_GridCtrl.SetCellType(4, 1, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(4, 1);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				CString StrPriceSell1 = Float2SymbolString(Realtimeprice.m_astSellPrices[0].m_fPrice, fPrevReferPrice, iSaveDec);	
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPriceSell1);
				
				/////////////////////////////////////////////////
				//  买1
				m_GridCtrl.SetCellType(5, 1, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(5, 1);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				CString StrPriceBuy1 = Float2SymbolString(Realtimeprice.m_astBuyPrices[0].m_fPrice , fPrevReferPrice, iSaveDec);	
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPriceBuy1);
				
				/////////////////////////////////////////////////
				//  买2
				m_GridCtrl.SetCellType(6, 1, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(6, 1);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				CString StrPriceBuy2 = Float2SymbolString(Realtimeprice.m_astBuyPrices[1].m_fPrice , fPrevReferPrice, iSaveDec);	
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPriceBuy2);
				
				/////////////////////////////////////////////////
				//  买3
				m_GridCtrl.SetCellType(7, 1, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(7, 1);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				CString StrPriceBuy3 = Float2SymbolString(Realtimeprice.m_astBuyPrices[2].m_fPrice , fPrevReferPrice, iSaveDec);	
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPriceBuy3);
				
				/////////////////////////////////////////////////
				//  买4
				m_GridCtrl.SetCellType(8, 1, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(8, 1);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				CString StrPriceBuy4 = Float2SymbolString(Realtimeprice.m_astBuyPrices[3].m_fPrice , fPrevReferPrice, iSaveDec);	
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPriceBuy4);
				
				/////////////////////////////////////////////////
				//  买5 价
				m_GridCtrl.SetCellType(9, 1, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(9, 1);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				CString StrPriceBuy5 = Float2SymbolString(Realtimeprice.m_astBuyPrices[4].m_fPrice , fPrevReferPrice, iSaveDec);	
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPriceBuy5);
				
				/////////////////////////////////////////////////
				//  卖5, 黄色 , 量
				CGridCellSys* pCell = (CGridCellSys *)m_GridCtrl.GetCell(0, 3);
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				pCell->SetDefaultTextColor(ESCVolume);
				CString StrVolumeSell5 = Float2String( Realtimeprice.m_astSellPrices[4].m_fVolume, 0, false);
				pCell->SetFont(&fontNumber);
				pCell->SetText(StrVolumeSell5);
				
				/////////////////////////////////////////////////
				//  卖4, 黄色
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(1, 3);
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				pCell->SetDefaultTextColor(ESCVolume);
				CString StrVolumeSell4 = Float2String( Realtimeprice.m_astSellPrices[3].m_fVolume, 0, false);
				pCell->SetFont(&fontNumber);
				pCell->SetText(StrVolumeSell4);
				
				/////////////////////////////////////////////////
				//  卖3, 黄色
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(2, 3);
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				pCell->SetDefaultTextColor(ESCVolume);
				CString StrVolumeSell3 = Float2String( Realtimeprice.m_astSellPrices[2].m_fVolume, 0, false);
				pCell->SetFont(&fontNumber);
				pCell->SetText(StrVolumeSell3);
				
				/////////////////////////////////////////////////
				//  卖2, 黄色
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(3, 3);
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				pCell->SetDefaultTextColor(ESCVolume);
				CString StrVolumeSell2 = Float2String( Realtimeprice.m_astSellPrices[1].m_fVolume, 0, false);
				pCell->SetFont(&fontNumber);
				pCell->SetText(StrVolumeSell2);
				
				/////////////////////////////////////////////////
				//  卖1, 黄色
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(4, 3);
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				pCell->SetDefaultTextColor(ESCVolume);
				CString StrVolumeSell1 = Float2String( Realtimeprice.m_astSellPrices[0].m_fVolume, 0, false);
				pCell->SetFont(&fontNumber);
				pCell->SetText(StrVolumeSell1);
				
				/////////////////////////////////////////////////
				//  买1, 黄色
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(5, 3);
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				pCell->SetDefaultTextColor(ESCVolume);
				CString StrVolumeBuy1 = Float2String( Realtimeprice.m_astBuyPrices[0].m_fVolume, 0, false);
				pCell->SetFont(&fontNumber);
				pCell->SetText(StrVolumeBuy1);
				
				/////////////////////////////////////////////////
				//  买2, 黄色
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(6, 3);
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				pCell->SetDefaultTextColor(ESCVolume);
				CString StrVolumeBuy2 = Float2String( Realtimeprice.m_astBuyPrices[1].m_fVolume, 0, false);
				pCell->SetFont(&fontNumber);
				pCell->SetText(StrVolumeBuy2);
				
				/////////////////////////////////////////////////
				//  买3, 黄色
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(7, 3);
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				pCell->SetDefaultTextColor(ESCVolume);
				CString StrVolumeBuy3 = Float2String( Realtimeprice.m_astBuyPrices[2].m_fVolume, 0, false);
				pCell->SetFont(&fontNumber);
				pCell->SetText(StrVolumeBuy3);
				
				/////////////////////////////////////////////////
				//  买4, 黄色
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(8, 3);
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				pCell->SetDefaultTextColor(ESCVolume);
				CString StrVolumeBuy4 = Float2String( Realtimeprice.m_astBuyPrices[3].m_fVolume, 0, false);
				pCell->SetFont(&fontNumber);
				pCell->SetText(StrVolumeBuy4);
				
				/////////////////////////////////////////////////
				//  买5, 黄色 量
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(9, 3);
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				pCell->SetDefaultTextColor(ESCVolume);
				CString StrVolumeBuy5 = Float2String( Realtimeprice.m_astBuyPrices[4].m_fVolume, 0, false);
				pCell->SetFont(&fontNumber);
				pCell->SetText(StrVolumeBuy5);

				/////////////////////////////////////////////////
				// 最新价 
				m_GridCtrl.SetCellType(10, 1, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(10, 1);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				CString StrPriceNew = Float2SymbolString(Realtimeprice.m_fPriceNew,fPrevReferPrice,iSaveDec);
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPriceNew);
				
				/////////////////////////////////////////////////
				// 涨跌 
				m_GridCtrl.SetCellType(11, 1, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(11, 1);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				
				CString StrRiseValue = L" -";
				if (0. != Realtimeprice.m_fPriceNew)
				{
					StrRiseValue = Float2SymbolString(Realtimeprice.m_fPriceNew - fPrevReferPrice, 0, iSaveDec, false, false);	
				}
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrRiseValue);
				
				/////////////////////////////////////////////////
				// 涨幅 
				m_GridCtrl.SetCellType(12, 1, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(12, 1);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				
				CString StrPriceRisePercent = L" -";
				if (0. != Realtimeprice.m_fPriceNew && 0. != fPrevReferPrice)
				{
					float fRisePercent = ((Realtimeprice.m_fPriceNew - fPrevReferPrice) / fPrevReferPrice) * 100.;
					StrPriceRisePercent = Float2SymbolString(fRisePercent, 0, 2, false, false, true);
				}
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPriceRisePercent);
				
				/////////////////////////////////////////////////
				//  总手, 黄色
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(13, 1);
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				pCell->SetDefaultTextColor(ESCVolume);
				CString StrVolume = Float2String(Realtimeprice.m_fVolumeTotal, 0, true);
				pCell->SetFont(&fontNumber);
				pCell->SetText(StrVolume );
				
				/////////////////////////////////////////////////
				//  现手, 黄色
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(14, 1);
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				pCell->SetDefaultTextColor(ESCVolume);
				
				CString StrVolumeNow = Float2String(Realtimeprice.m_fVolumeCur, 0, false);
				pCell->SetFont(&fontNumber);
				pCell->SetText(StrVolumeNow );
				
				/////////////////////////////////////////////////
				// 持仓, 黄色
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(15, 1);
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				pCell->SetDefaultTextColor(ESCVolume);
				CString StrVolumeHold = Float2String( Realtimeprice.m_fHoldTotal , 0, true);
				pCell->SetFont(&fontNumber);
				pCell->SetText(StrVolumeHold);
				
				/////////////////////////////////////////////////
				// 外盘
				m_GridCtrl.SetCellType(16, 1, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol*)m_GridCtrl.GetCell(16, 1);	
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				CString StrSellVolume = Float2SymbolString(Realtimeprice.m_fSellVolume, -100000, 0, true);
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrSellVolume);         
				
				/////////////////////////////////////////////////
				// 均价 
				m_GridCtrl.SetCellType(10, 3, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(10, 3);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				CString StrPriceAvg = Float2SymbolString(Realtimeprice.m_fPriceAvg , fPrevReferPrice, iSaveDec);	
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPriceAvg);
				
				/////////////////////////////////////////////////
				// 昨结 
				m_GridCtrl.SetCellType(11, 3, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(11, 3);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				CString StrPricePreClose = Float2SymbolString(fPricePrevAvg, fPrevReferPrice, iSaveDec);	
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPricePreClose);
				
				/////////////////////////////////////////////////
				// 开盘 
				m_GridCtrl.SetCellType(12, 3, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(12, 3);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				CString StrPriceOpen = Float2SymbolString( Realtimeprice.m_fPriceOpen , fPrevReferPrice, iSaveDec);	
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPriceOpen);
				
				/////////////////////////////////////////////////
				// 最高 
				m_GridCtrl.SetCellType(13, 3, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(13, 3);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				CString StrPriceHigh = Float2SymbolString( Realtimeprice.m_fPriceHigh , fPrevReferPrice, iSaveDec);	
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPriceHigh);
				
				/////////////////////////////////////////////////
				// 最低 
				m_GridCtrl.SetCellType(14, 3, RUNTIME_CLASS(CGridCellSymbol));
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(14, 3);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				CString StrPriceLow = Float2SymbolString( Realtimeprice.m_fPriceLow, fPrevReferPrice, iSaveDec);	
				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrPriceLow);
				
				/////////////////////////////////////////////////
				// 仓差 
				m_GridCtrl.SetCellType(15, 3, RUNTIME_CLASS(CGridCellSys));
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(15, 3);
				pCell->SetDefaultTextColor(ESCVolume);			
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);			
				CString StrDifference = Float2String(Realtimeprice.m_fHoldTotal - Realtimeprice.m_fHoldPrev, 0, true);
				pCell->SetFont(&fontNumber);
				pCell->SetText(StrDifference);
				
				/////////////////////////////////////////////////
				// 内盘------ 与一个非常大的数字比较
				m_GridCtrl.SetCellType(16, 3, RUNTIME_CLASS(CGridCellSymbol));
				
				pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(16, 3);
				pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);
				pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
				// 内盘为总量减去外盘
				CString StrBuyVolumn = CalculatefVolumnBuy(Realtimeprice.m_fVolumeTotal, Realtimeprice.m_fBuyVolume,Realtimeprice.m_fSellVolume, 100000000, true);

				pCellSymbol->SetFont(&fontNumber);
				pCellSymbol->SetText(StrBuyVolumn);
				
				for (int i = 0;i<m_GridCtrl.GetColumnCount(); i++)
				{
					CGridCellSys * pRecentCell = (CGridCellSys *)m_GridCtrl.GetCell(4,i);
					pRecentCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
					
					pRecentCell = (CGridCellSys *)m_GridCtrl.GetCell(9,i);
					pRecentCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
					
					pRecentCell = (CGridCellSys *)m_GridCtrl.GetCell(13,i);
					pRecentCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
					
					pRecentCell = (CGridCellSys *)m_GridCtrl.GetCell(14,i);
					pRecentCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
					
					pRecentCell = (CGridCellSys *)m_GridCtrl.GetCell(16,i);
					pRecentCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
				}
			}
			else
			{
				bool32 bShowTranslate = false;
				CGridCellSymbol *pCellSymbol = NULL;
				CGridCellSys * pCell = NULL;
				bool32 bIsFutureCn = ERTFuturesCn == eMerchKind;

				int32 iRow = 0;
				if ( bIsFutureCn )
				{
			//		ASSERT( IsNeedShowFutureCnTick() ? m_GridCtrl.GetRowCount() == 14 : m_GridCtrl.GetRowCount() == 10 );
					// 委比-委差
					// 委比
					{
						float fRate = 0.0;
						
						if ( 0 != (fVolBuySum + fVolSellSum) )
						{
							fRate = (float)(fVolBuySum - fVolSellSum)*(float)100 / (fVolBuySum + fVolSellSum);
						}
						m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
						pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 1);
						pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);
						pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
						CString Str = Float2SymbolString(fRate, 0.0, 2, false, false, true);	
						pCellSymbol->SetFont(&fontNumber);
						pCellSymbol->SetText(Str);
					}
					// 委差
					{
						float fDiff = 0.0;
						fDiff = (float)(fVolBuySum - fVolSellSum);
						
						m_GridCtrl.SetCellType(iRow, 3, RUNTIME_CLASS(CGridCellSymbol));
						pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 3);
						pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);
						pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
						CString Str = Float2SymbolString(fDiff, 0.0, 0, false, false, false);	
						pCellSymbol->SetFont(&fontNumber);
						pCellSymbol->SetText(Str);
					}
					++iRow;

					/////////////////////////////////////////////////
					if (m_bIsShowBuySell)
					{
						int32 iItem = m_iBuySellCnt;
						// 卖
						for(int i = 0; i < iItem; i++)
						{
							m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
							pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 1);
							pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);
							pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
							CString StrPriceSell = Float2SymbolString( Realtimeprice.m_astSellPrices[(iItem-1)-i].m_fPrice, fPrevReferPrice, iSaveDec);	
							pCellSymbol->SetFont(&fontNumber);
							pCellSymbol->SetText(StrPriceSell);

							CGridCellSys *pGetCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 3);
							pGetCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
							pGetCell->SetDefaultTextColor(ESCVolume);
							CString StrVolumeSell = Float2String( Realtimeprice.m_astSellPrices[(iItem-1)-i].m_fVolume, 0, false);
							pGetCell->SetFont(&fontNumber);
							pGetCell->SetText(StrVolumeSell);
							++iRow;
						}
						// 买
						for(int i = 0; i < iItem; i++)
						{
							m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
							pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 1);
							pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
							pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
							CString StrPriceBuy = Float2SymbolString(Realtimeprice.m_astBuyPrices[i].m_fPrice , fPrevReferPrice, iSaveDec);	
							pCellSymbol->SetFont(&fontNumber);
							pCellSymbol->SetText(StrPriceBuy);

							// 黄色
							CGridCellSys *pMyCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 3);
							pMyCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
							pMyCell->SetDefaultTextColor(ESCVolume);
							CString StrVolumeBuy = Float2String( Realtimeprice.m_astBuyPrices[i].m_fVolume, 0, false);
							pMyCell->SetFont(&fontNumber);
							pMyCell->SetText(StrVolumeBuy);
							++iRow;
						}	
					}
					else
					{
						// 卖出价 
						// 0
						m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
						pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 1);
						pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
						pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
						CString StrPriceSell = Float2SymbolString(Realtimeprice.m_astSellPrices[0].m_fPrice, fPrevReferPrice, iSaveDec);				
						pCellSymbol->SetText(StrPriceSell);					
						pCellSymbol->SetFont(&fontNumber);
						pCellSymbol->SetOwnCursor(NULL);


						/////////////////////////////////////////////////
						// 卖出量, 黄色
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 3);
						pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
						pCell->SetDefaultTextColor(ESCVolume);
						CString StrVolumeSell0 = Float2String( Realtimeprice.m_astSellPrices[0].m_fVolume, 0, 0, bShowTranslate);
						pCell->SetFont(&fontNumber);
						pCell->SetText(StrVolumeSell0);
						++iRow;


						/////////////////////////////////////////////////
						// 买入价 
						// 1
						m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
						pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 1);
						pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
						pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
						CString StrPriceBuy = Float2SymbolString(Realtimeprice.m_astBuyPrices[0].m_fPrice, fPrevReferPrice, iSaveDec);	
						pCellSymbol->SetFont(&fontNumber);
						pCellSymbol->SetText(StrPriceBuy);
						pCellSymbol->SetOwnCursor(NULL);

						/////////////////////////////////////////////////
						// 买入量, 黄色
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 3);
						pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
						pCell->SetDefaultTextColor(ESCVolume);
						CString StrVolumeBuy0 = Float2String( Realtimeprice.m_astBuyPrices[0].m_fVolume, 0, bShowTranslate);
						pCell->SetFont(&fontNumber);
						pCell->SetText(StrVolumeBuy0);
						++iRow;
					}
					
					/////////////////////////////////////////////////
					// 最新价 
					// 2
					m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 1);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					CString StrPriceNew = Float2SymbolString(Realtimeprice.m_fPriceNew,fPrevReferPrice,iSaveDec);
					pCellSymbol->SetFont(&fontNumber);
					pCellSymbol->SetText(StrPriceNew);
					/////////////////////////////////////////////////
					// 均价 
					m_GridCtrl.SetCellType(iRow, 3, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 3);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					CString StrPriceAvg = Float2SymbolString(Realtimeprice.m_fPriceAvg , fPrevReferPrice, iSaveDec);	
					pCellSymbol->SetFont(&fontNumber);
					pCellSymbol->SetText(StrPriceAvg);
					++iRow;
					
					/////////////////////////////////////////////////
					// 涨跌 
					// 3
					m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 1);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					CString StrRiseValue = L" -";
					if (0. != Realtimeprice.m_fPriceNew)
					{
						StrRiseValue = Float2SymbolString(Realtimeprice.m_fPriceNew - fPrevReferPrice, 0, iSaveDec, false, false);	
					}
					pCellSymbol->SetFont(&fontNumber);
					pCellSymbol->SetText(StrRiseValue);
					/////////////////////////////////////////////////
					// 昨结 
					m_GridCtrl.SetCellType(iRow, 3, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 3);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					CString StrPricePreClose = Float2SymbolString(fPricePrevAvg, fPrevReferPrice, iSaveDec);	
					pCellSymbol->SetFont(&fontNumber);
					pCellSymbol->SetText(StrPricePreClose);
					++iRow;
					
					/////////////////////////////////////////////////
					// 涨幅 
					// 4
					m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 1);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					
					CString StrPriceRisePercent = L" -";
					if (0. != Realtimeprice.m_fPriceNew && 0. != fPrevReferPrice)
					{
						float fRisePercent = ((Realtimeprice.m_fPriceNew - fPrevReferPrice) / fPrevReferPrice) * 100.;
						StrPriceRisePercent = Float2SymbolString(fRisePercent, 0, 2, false, false, true);
					}
					pCellSymbol->SetFont(&fontNumber);
					pCellSymbol->SetText(StrPriceRisePercent);
					/////////////////////////////////////////////////
					// 开盘 
					m_GridCtrl.SetCellType(iRow, 3, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 3);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					CString StrPriceOpen = Float2SymbolString( Realtimeprice.m_fPriceOpen , fPrevReferPrice, iSaveDec);	
					pCellSymbol->SetFont(&fontNumber);
					pCellSymbol->SetText(StrPriceOpen);
					++iRow;
					
					/////////////////////////////////////////////////
					//  总手, 黄色
					// 5
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 1);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCell->SetDefaultTextColor(ESCVolume);
					CString StrVolume = Float2String(Realtimeprice.m_fVolumeTotal, 0, bShowTranslate);
					pCell->SetFont(&fontNumber);
					pCell->SetText(StrVolume );
					/////////////////////////////////////////////////
					// 最高 
					m_GridCtrl.SetCellType(iRow, 3, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 3);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					CString StrPriceHigh = Float2SymbolString( Realtimeprice.m_fPriceHigh , fPrevReferPrice, iSaveDec);	
					pCellSymbol->SetFont(&fontNumber);
					pCellSymbol->SetText(StrPriceHigh);
					++iRow;
					
					/////////////////////////////////////////////////
					//  现手, 黄色
					// 6
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 1);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCell->SetDefaultTextColor(ESCVolume);
					
					CString StrVolumeNow = Float2String(Realtimeprice.m_fVolumeCur, 0, bShowTranslate);
					pCell->SetFont(&fontNumber);
					pCell->SetText(StrVolumeNow );
					/////////////////////////////////////////////////
					// 最低 
					m_GridCtrl.SetCellType(iRow, 3, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 3);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					CString StrPriceLow = Float2SymbolString( Realtimeprice.m_fPriceLow, fPrevReferPrice, iSaveDec);	
					pCellSymbol->SetFont(&fontNumber);
					pCellSymbol->SetText(StrPriceLow);
					++iRow;
					
					/////////////////////////////////////////////////
					// 持仓, 黄色
					// 7
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 1);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCell->SetDefaultTextColor(ESCVolume);
					CString StrVolumeHold = Float2String( Realtimeprice.m_fHoldTotal , 0, bShowTranslate);
					pCell->SetFont(&fontNumber);
					pCell->SetText(StrVolumeHold);
					/////////////////////////////////////////////////
					// 仓差 
					m_GridCtrl.SetCellType(iRow, 3, RUNTIME_CLASS(CGridCellSys));
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 3);
					pCell->SetDefaultTextColor(ESCVolume);			
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);			
					CString StrDifference = Float2String(Realtimeprice.m_fHoldTotal - Realtimeprice.m_fHoldPrev, 0, bShowTranslate);
					pCell->SetFont(&fontNumber);
					pCell->SetText(StrDifference);
					++iRow;
					
					/////////////////////////////////////////////////
					// 外盘
					// 8
					m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol*)m_GridCtrl.GetCell(iRow, 1);	
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					CString StrSellVolume = Float2SymbolString(Realtimeprice.m_fSellVolume, -100000, 0, bShowTranslate);
					pCellSymbol->SetFont(&fontNumber);
					pCellSymbol->SetText(StrSellVolume); 
					/////////////////////////////////////////////////
					// 内盘------ 与一个非常大的数字比较
					m_GridCtrl.SetCellType(iRow, 3, RUNTIME_CLASS(CGridCellSymbol));
					
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 3);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					// 内盘为总量减去外盘	
					CString StrBuyVolumn = CalculatefVolumnBuy(Realtimeprice.m_fVolumeTotal, Realtimeprice.m_fBuyVolume,Realtimeprice.m_fSellVolume, 100000000, true);

					pCellSymbol->SetFont(&fontNumber);
					pCellSymbol->SetText(StrBuyVolumn);
				}
				else if (ERTFutureSpot == eMerchKind || ERTMony == eMerchKind)
				{
					if (m_bIsShowBuySell)
					{
						int32 iItem = m_iBuySellCnt;
						// 卖
						for(int i = 0; i < iItem; i++)
						{
							m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
							pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 1);
							pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);
							pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
							CString StrPriceSell = Float2SymbolString( Realtimeprice.m_astSellPrices[(iItem-1)-i].m_fPrice, fPrevReferPrice, iSaveDec);	
							pCellSymbol->SetFont(&fontNumber);
							pCellSymbol->SetText(StrPriceSell);

							CGridCellSys *pCurrentCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 3);
							pCurrentCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
							pCurrentCell->SetDefaultTextColor(ESCVolume);
							CString StrVolumeSell = Float2String( Realtimeprice.m_astSellPrices[(iItem-1)-i].m_fVolume, 0, false);
							pCurrentCell->SetFont(&fontNumber);
							pCurrentCell->SetText(StrVolumeSell);
							++iRow;
						}
						// 买
						for(int i = 0; i < iItem; i++)
						{
							m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
							pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 1);
							pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
							pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
							CString StrPriceBuy = Float2SymbolString(Realtimeprice.m_astBuyPrices[i].m_fPrice , fPrevReferPrice, iSaveDec);	
							pCellSymbol->SetFont(&fontNumber);
							pCellSymbol->SetText(StrPriceBuy);

							// 黄色
							CGridCellSys *pSingleCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 3);
							pSingleCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
							pSingleCell->SetDefaultTextColor(ESCVolume);
							CString StrVolumeBuy = Float2String( Realtimeprice.m_astBuyPrices[i].m_fVolume, 0, false);
							pSingleCell->SetFont(&fontNumber);
							pSingleCell->SetText(StrVolumeBuy);
							++iRow;
						}	
					}

					/////////////////////////////////////////////////
					// 最新价 
					// 2
					m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 1);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					CString StrPriceNew = Float2SymbolString(Realtimeprice.m_fPriceNew,fPrevReferPrice,iSaveDec);
					pCellSymbol->SetFont(&fontNumber);
					pCellSymbol->SetText(StrPriceNew);
					/////////////////////////////////////////////////
					// 均价 
					m_GridCtrl.SetCellType(iRow, 3, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 3);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					CString StrPriceAvg = Float2SymbolString(Realtimeprice.m_fPriceAvg , fPrevReferPrice, iSaveDec);	
					pCellSymbol->SetFont(&fontNumber);
					pCellSymbol->SetText(StrPriceAvg);
					++iRow;
					
					/////////////////////////////////////////////////
					// 涨跌 
					// 3
					m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 1);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					CString StrRiseValue = L" -";
					if (0. != Realtimeprice.m_fPriceNew)
					{
						StrRiseValue = Float2SymbolString(Realtimeprice.m_fPriceNew - fPrevReferPrice, 0, iSaveDec, false, false);	
					}
					pCellSymbol->SetFont(&fontNumber);
					pCellSymbol->SetText(StrRiseValue);
					/////////////////////////////////////////////////
					// 昨结 
					m_GridCtrl.SetCellType(iRow, 3, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 3);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					// zhangtao
					CString StrPricePreClose;
					if (ERTMony == eMerchKind)// 如果是“做市商”
					{
						BUY_SELL_PLATE nType = CConfigInfo::Instance()->m_nBuySellPlate;
						if (YESTERDAY_SETTLEMENT == nType)// 如果是昨结
							StrPricePreClose = Float2SymbolString(fPricePrevAvg, fPrevReferPrice, iSaveDec);
						else if (YESTERDAY_CLOSE == nType)// 如果是昨收
							StrPricePreClose = Float2SymbolString(fPrevReferPrice, fPricePrevAvg, iSaveDec);
					}
					else
					{
						StrPricePreClose = Float2SymbolString(fPricePrevAvg, fPrevReferPrice, iSaveDec);
					}
					pCellSymbol->SetFont(&fontNumber);
					pCellSymbol->SetText(StrPricePreClose);
					++iRow;
					
					/////////////////////////////////////////////////
					// 涨幅 
					// 4
					m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 1);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					
					CString StrPriceRisePercent = L" -";
					if (0. != Realtimeprice.m_fPriceNew && 0. != fPrevReferPrice)
					{
						float fRisePercent = ((Realtimeprice.m_fPriceNew - fPrevReferPrice) / fPrevReferPrice) * 100.;
						StrPriceRisePercent = Float2SymbolString(fRisePercent, 0, 2, false, false, true);
					}
					pCellSymbol->SetFont(&fontNumber);
					pCellSymbol->SetText(StrPriceRisePercent);
					/////////////////////////////////////////////////
					// 开盘 
					m_GridCtrl.SetCellType(iRow, 3, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 3);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					CString StrPriceOpen = Float2SymbolString( Realtimeprice.m_fPriceOpen , fPrevReferPrice, iSaveDec);	
					pCellSymbol->SetFont(&fontNumber);
					pCellSymbol->SetText(StrPriceOpen);
					++iRow;
					
					// 最高 
					m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 1);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					CString StrPriceHigh = Float2SymbolString( Realtimeprice.m_fPriceHigh , fPrevReferPrice, iSaveDec);	
					pCellSymbol->SetFont(&fontNumber);
					pCellSymbol->SetText(StrPriceHigh);
			
					// 最低 
					m_GridCtrl.SetCellType(iRow, 3, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 3);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					CString StrPriceLow = Float2SymbolString( Realtimeprice.m_fPriceLow, fPrevReferPrice, iSaveDec);	
					pCellSymbol->SetFont(&fontNumber);
					pCellSymbol->SetText(StrPriceLow);
					++iRow;

					if(m_bIsShowBuySell)
					{
						int32 iItem = m_iBuySellCnt;
						for (int i = 0; i < m_GridCtrl.GetColumnCount(); i++)
						{
							pCell = NULL;
							pCell = (CGridCellSys *)m_GridCtrl.GetCell(iItem - 1, i);
							pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);

							pCell = (CGridCellSys *)m_GridCtrl.GetCell(2*iItem - 1, i);
							pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);

						}
					}
					else
					{
						for (int i = 0;i<m_GridCtrl.GetColumnCount(); i++)
						{
							iRow = 0;
							pCell = NULL;

							pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow,i);
							pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);

							pCell = (CGridCellSys *)m_GridCtrl.GetCell(1+iRow,i);
							pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);

							pCell = (CGridCellSys *)m_GridCtrl.GetCell(2+iRow,i);
							pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);

							pCell = (CGridCellSys *)m_GridCtrl.GetCell(3+iRow,i);
							pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
						}	
					}
					
					RedrawWindow(m_RectSpecialBuy);
					RedrawWindow(m_RectSpecialSell);
				}
				else
				{
					/////////////////////////////////////////////////
					if (m_bIsShowBuySell)
					{
						int32 iItem = m_iBuySellCnt;
						// 卖
						for(int i = 0; i < iItem; i++)
						{
							m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
							pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 1);
							pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);
							pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
							CString StrPriceSell = Float2SymbolString( Realtimeprice.m_astSellPrices[(iItem-1)-i].m_fPrice, fPrevReferPrice, iSaveDec);	
							pCellSymbol->SetFont(&fontNumber);
							pCellSymbol->SetText(StrPriceSell);

							CGridCellSys *pCurCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 3);
							pCurCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
							pCurCell->SetDefaultTextColor(ESCVolume);
							CString StrVolumeSell = Float2String( Realtimeprice.m_astSellPrices[(iItem-1)-i].m_fVolume, 0, false);
							pCurCell->SetFont(&fontNumber);
							pCurCell->SetText(StrVolumeSell);
							++iRow;
						}
						// 买
						for(int i = 0; i < iItem; i++)
						{
							m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
							pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 1);
							pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
							pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
							CString StrPriceBuy = Float2SymbolString(Realtimeprice.m_astBuyPrices[i].m_fPrice , fPrevReferPrice, iSaveDec);	
							pCellSymbol->SetFont(&fontNumber);
							pCellSymbol->SetText(StrPriceBuy);

							// 黄色
							CGridCellSys *pTmpCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 3);
							pTmpCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
							pTmpCell->SetDefaultTextColor(ESCVolume);
							CString StrVolumeBuy = Float2String( Realtimeprice.m_astBuyPrices[i].m_fVolume, 0, false);
							pTmpCell->SetFont(&fontNumber);
							pTmpCell->SetText(StrVolumeBuy);
							++iRow;
						}	
					}
					else
					{
						// 卖出价 
						// 0
						m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
						pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 1);
						pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
						pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
						CString StrPriceSell = Float2SymbolString(Realtimeprice.m_astSellPrices[0].m_fPrice, fPrevReferPrice, iSaveDec);				
						pCellSymbol->SetFont(&fontNumber);
						pCellSymbol->SetText(StrPriceSell);
						pCellSymbol->SetOwnCursor(NULL);

						/////////////////////////////////////////////////
						// 买入价 
						// 1
						m_GridCtrl.SetCellType(iRow, 3, RUNTIME_CLASS(CGridCellSymbol));
						pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 3);
						pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
						pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
						CString StrPriceBuy = Float2SymbolString(Realtimeprice.m_astBuyPrices[0].m_fPrice, fPrevReferPrice, iSaveDec);	
						pCellSymbol->SetFont(&fontNumber);
						pCellSymbol->SetText(StrPriceBuy);
						pCellSymbol->SetOwnCursor(NULL);
						++iRow;
					}
										
					/////////////////////////////////////////////////
					// 最新价 
					// 2
					m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 1);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					CString StrPriceNew = Float2SymbolString(Realtimeprice.m_fPriceNew,fPrevReferPrice,iSaveDec);
					pCellSymbol->SetFont(&fontNumber);
					pCellSymbol->SetText(StrPriceNew);
					/////////////////////////////////////////////////
					// 均价 
					m_GridCtrl.SetCellType(iRow, 3, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 3);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					CString StrPriceAvg = Float2SymbolString(Realtimeprice.m_fPriceAvg , fPrevReferPrice, iSaveDec);	
					pCellSymbol->SetFont(&fontNumber);
					pCellSymbol->SetText(StrPriceAvg);
					++iRow;
					
					/////////////////////////////////////////////////
					// 涨跌 
					// 3
					m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 1);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					CString StrRiseValue = L" -";
					if (0. != Realtimeprice.m_fPriceNew)
					{
						StrRiseValue = Float2SymbolString(Realtimeprice.m_fPriceNew - fPrevReferPrice, 0, iSaveDec, false, false);	
					}
					pCellSymbol->SetFont(&fontNumber);
					pCellSymbol->SetText(StrRiseValue);
					/////////////////////////////////////////////////
					// 昨结 
					m_GridCtrl.SetCellType(iRow, 3, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 3);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					CString StrPricePreClose = Float2SymbolString(fPricePrevAvg, fPrevReferPrice, iSaveDec);	
					pCellSymbol->SetFont(&fontNumber);
					pCellSymbol->SetText(StrPricePreClose);
					++iRow;
					
					/////////////////////////////////////////////////
					// 涨幅 
					// 4
					m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 1);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					
					CString StrPriceRisePercent = L" -";
					if (0. != Realtimeprice.m_fPriceNew && 0. != fPrevReferPrice)
					{
						float fRisePercent = ((Realtimeprice.m_fPriceNew - fPrevReferPrice) / fPrevReferPrice) * 100.0f;
						StrPriceRisePercent = Float2SymbolString(fRisePercent, 0, 2, false, false, true);
					}
					pCellSymbol->SetFont(&fontNumber);
					pCellSymbol->SetText(StrPriceRisePercent);
					/////////////////////////////////////////////////
					// 开盘 
					m_GridCtrl.SetCellType(iRow, 3, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 3);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					CString StrPriceOpen = Float2SymbolString( Realtimeprice.m_fPriceOpen , fPrevReferPrice, iSaveDec);	
					pCellSymbol->SetFont(&fontNumber);
					pCellSymbol->SetText(StrPriceOpen);
					++iRow;
					
					// 最高 
					m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 1);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					CString StrPriceHigh = Float2SymbolString( Realtimeprice.m_fPriceHigh , fPrevReferPrice, iSaveDec);	
					pCellSymbol->SetFont(&fontNumber);
					pCellSymbol->SetText(StrPriceHigh);
			
					// 最低 
					m_GridCtrl.SetCellType(iRow, 3, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 3);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					CString StrPriceLow = Float2SymbolString( Realtimeprice.m_fPriceLow, fPrevReferPrice, iSaveDec);	
					pCellSymbol->SetFont(&fontNumber);
					pCellSymbol->SetText(StrPriceLow);
				}

				if ( bIsFutureCn && IsNeedShowFutureCnTick() )
				{
					// 增仓 x 性质 x
					// 开仓 x 平仓 x
					// 多开 x 多平 x
					// 空开 x 空平 x
					iRow = 10;
					//ASSERT( m_GridCtrl.GetRowCount() == 10 );
					if(m_bIsShowBuySell)
					{
						int32 iItem = m_iBuySellCnt;
						iRow += (iItem-1)*2;
					}

					// 增仓
					{
						m_GridCtrl.SetCellType(iRow, 1, RUNTIME_CLASS(CGridCellSymbol));
						pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, 1);
						pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);
						pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
						CString Str = Float2SymbolString(Realtimeprice.m_fHoldCur, 0.0, 0, false, false, false);	
						pCellSymbol->SetFont(&fontNumber);
						pCellSymbol->SetText(Str);
					}
					// 性质
					{
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 3);
						pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
						pCell->SetFont(&fontNumber);
						pCell->SetText(m_StrFutureCnProperty);
						pCell->SetDefaultTextColor((E_SysColor)m_dwFutureCnProperty);
					}
					++iRow;

					// 开仓
					float fCangCha = Realtimeprice.m_fHoldTotal - Realtimeprice.m_fHoldPrev;
					{
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 1);
						pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
						CString Str = Float2String((Realtimeprice.m_fVolumeTotal+fCangCha)/2, 0, true, false, false);	
						pCell->SetFont(&fontNumber);
						pCell->SetText(Str);
						pCell->SetTextClr(GetIoViewColor(ESCVolume));
					}
					// 平仓
					{
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 3);
						pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
						CString Str = Float2String((Realtimeprice.m_fVolumeTotal-fCangCha)/2, 0, true, false, false);	
						pCell->SetFont(&fontNumber);
						pCell->SetText(Str);
						pCell->SetTextClr(GetIoViewColor(ESCVolume));
					}
					ShowFutureCnData(false);
				}

				int i;
				if(m_bIsShowBuySell)
				{
					int32 iItem = m_iBuySellCnt;
					if (bIsFutureCn)
					{
						for ( i = 0;i<m_GridCtrl.GetColumnCount(); i++)
						{
							iRow = 0;
							pCell = NULL;

							pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow,i);
							pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
							iRow = 1+(iItem-1)*2;

							pCell = (CGridCellSys *)m_GridCtrl.GetCell(iItem,i);
							pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);

							pCell = (CGridCellSys *)m_GridCtrl.GetCell(1+iRow,i);
							pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);

							pCell = (CGridCellSys *)m_GridCtrl.GetCell(6+iRow,i);
							pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);

							pCell = (CGridCellSys *)m_GridCtrl.GetCell(7+iRow,i);
							pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);

							pCell = (CGridCellSys *)m_GridCtrl.GetCell(8+iRow,i);
							pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);

							if ( IsNeedShowFutureCnTick() )
							{
								pCell = (CGridCellSys *)m_GridCtrl.GetCell(13+(iItem-1)*2,i);
								pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
							}
						}
					}
					else
					{
						for (i = 0; i < m_GridCtrl.GetColumnCount(); i++)
						{
							pCell = NULL;
							pCell = (CGridCellSys *)m_GridCtrl.GetCell(iItem - 1, i);
							pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);

							pCell = (CGridCellSys *)m_GridCtrl.GetCell(2*iItem - 1, i);
							pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);

						}
					}
				}
				else
				{
					if (bIsFutureCn)
					{
						for ( i = 0;i<m_GridCtrl.GetColumnCount(); i++)
						{
							iRow = 0;
							pCell = NULL;

							pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow,i);
							pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
							iRow = 1;

							pCell = (CGridCellSys *)m_GridCtrl.GetCell(1+iRow,i);
							pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);

							pCell = (CGridCellSys *)m_GridCtrl.GetCell(6+iRow,i);
							pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);

							pCell = (CGridCellSys *)m_GridCtrl.GetCell(7+iRow,i);
							pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);

							pCell = (CGridCellSys *)m_GridCtrl.GetCell(8+iRow,i);
							pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);

							if ( IsNeedShowFutureCnTick() )
							{
								pCell = (CGridCellSys *)m_GridCtrl.GetCell(13,i);
								pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
							}
						}
					}
					else
					{
						for (i = 0;i<m_GridCtrl.GetColumnCount(); i++)
						{
							iRow = 0;
							pCell = NULL;

							pCell = (CGridCellSys *)m_GridCtrl.GetCell(1+iRow,i);
							pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);

							pCell = (CGridCellSys *)m_GridCtrl.GetCell(3+iRow,i);
							pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);

							if (iRow + 4 >= m_GridCtrl.GetRowCount())
							{
								continue;
							}
							pCell = (CGridCellSys *)m_GridCtrl.GetCell(4+iRow,i);
							pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
						}
					}
				}				
			}		
		}
		break;	
	default:
		break;
	}
	m_GridCtrl.Refresh();

	CString StrTitle;
	StrTitle.Format(L"%s(%s)", pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer());
	CString StrSubTitle = L"";
	CRealtimePrice* pRealPrice = m_pMerchXml->m_pRealtimePrice;
	if(pRealPrice && abs(pRealPrice->m_fPriceNew) >  0.000001f)
	{
		// 现价
		float fPrevReferPrice = pRealPrice->m_fPricePrevClose;
		int iSaveDec = m_pMerchXml->m_MerchInfo.m_iSaveDec;
		CString StrPriceNew = Float2SymbolString( pRealPrice->m_fPriceNew , fPrevReferPrice, iSaveDec);	
		StrPriceNew = StrPriceNew.Right(StrPriceNew.GetLength() - 1);

		//  涨跌
		CString StrRiseValue = L"";
		if (0. != pRealPrice->m_fPriceNew)
		{
			StrRiseValue = Float2SymbolString(pRealPrice->m_fPriceNew - fPrevReferPrice, 0, iSaveDec, false, false);
			if(-1 == StrRiseValue.Find('-'))
			{
				StrRiseValue = StrRiseValue.Right(StrRiseValue.GetLength() - 1);
			}
		}

		//  涨幅
		CString StrPriceRisePercent = L"";
		if (0. != pRealPrice->m_fPriceNew && 0. != fPrevReferPrice)
		{
			float fRisePercent = ((pRealPrice->m_fPriceNew - fPrevReferPrice) / fPrevReferPrice) * 100.;
			StrPriceRisePercent = Float2SymbolString(fRisePercent, 0, 2, false, false, true);
			if(-1 == StrPriceRisePercent.Find('-'))
			{
				StrPriceRisePercent = StrPriceRisePercent.Right(StrPriceRisePercent.GetLength() - 1);
			}
		}

		StrSubTitle.Format(L"%s|%s|%s", StrPriceNew, StrRiseValue, StrPriceRisePercent);
	}

	m_CtrlTitle.SetTitle(StrTitle, StrSubTitle);
}

bool32 CIoViewPrice::FromXml(TiXmlElement * pElement)
{
	//
	SetFontsFromXml(pElement);
	SetColorsFromXml(pElement);
	SetRowHeightAccordingFont();
	//
	m_bFromXml = true;
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
	return true;
}

CString CIoViewPrice::ToXml()
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
			StrMarketId.GetBuffer());

	CString StrFace;
	StrFace  = SaveColorsToXml();
	StrFace += SaveFontsToXml();
	//
	StrThis += StrFace;
	//
	StrThis += L">\n";
	//
	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";

	return StrThis;
} 

CString CIoViewPrice::GetDefaultXML()
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

bool32 CIoViewPrice::CreateTable(E_ReportType eMerchKind)
{
	// 尝试创建表格
	if (NULL == m_GridCtrl.GetSafeHwnd())
	{
		if (!m_CtrlTitle.Create(_T("60000"),SS_LEFT,CRect(0, 0, 0, 0), this))
			return false;
		m_CtrlTitle.ShowWindow(SW_SHOW);
		
		if (!m_GridCtrl.Create(CRect(0, 0, 0, 0), this, 20205))
			return false;
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

		if (!m_GridExpRiseFall.Create(CRect(0, 0, 0, 0), this, (UINT)IDC_STATIC, WS_CHILD |WS_TABSTOP))
			return false;
		m_GridExpRiseFall.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
		m_GridExpRiseFall.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
		m_GridExpRiseFall.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	}

	// 清空表格内容
	m_GridCtrl.DeleteAllItems();
	//
	m_GridCtrl.EnableSelection(false);
	//

	m_GridExpRiseFall.DeleteAllItems();
	m_GridExpRiseFall.ShowWindow(SW_HIDE);	// 除非是要显示红绿柱，否则隐藏
	m_GridExpRiseFall.EnableSelection(FALSE);
	m_GridExpRiseFall.SetHideTextFlag(true);


	bool32 bH;
	bool32 bRedGreenExp = BeGeneralIndex(bH);

	bool32 bBuySellPriceExchange = IsBuySellPriceExchangeMerch(m_pMerchXml);

	LOGFONT fontChinese;
	memset(&fontChinese, 0, sizeof(fontChinese));
	_tcscpy(fontChinese.lfFaceName, gFontFactory.GetExistFontName(_T("宋体")));///
	fontChinese.lfHeight  = -14;
	COLORREF clrChinese = RGB(160,160,160);


	// 根据不同类别创建不同的表格
	switch( eMerchKind )
	{
	case ERTStockCn:
	case ERTStockHk:		
		{
			if (!m_GridCtrl.SetColumnCount(4))
				return false;
			if (!m_GridCtrl.SetRowCount(19))
				return false;

			
			for (int32 i=0; i<m_GridCtrl.GetRowCount(); i++)
			{		
				if (m_iRowHeight <= 0)
				{
					m_iRowHeight = IOVIEWPRICEROWHEIGHT;
				}
				m_GridCtrl.SetRowHeight(i,m_iRowHeight);
			}
			
			// 设置表格图标
			// m_ImageList.Create(MAKEINTRESOURCE(IDB_GRID_REPORT), 16, 1, RGB(255,255,255));
			// m_GridCtrl.SetImageList(&m_ImageList);
			
			// 设置相互之间的关联			
			// m_XSBHorz.SetOwner(&m_GridCtrl);
			// m_XSBVert.SetOwner(&m_GridCtrl);
			// m_GridCtrl.SetScrollBar(&m_XSBHorz, &m_XSBVert);						
			///////////////////////////////////////////////// 
		
			CGridCellSys *pCell = (CGridCellSys *)m_GridCtrl.GetCell(0, 0);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
			pCell->SetText(L"委比");
			

			pCell = (CGridCellSys *)m_GridCtrl.GetCell(1, 0);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
			pCell->SetText(L"卖五");
			
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(2, 0);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
			pCell->SetText(L"卖四");
			
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(3, 0);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
			pCell->SetText( L"卖三");
			
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(4, 0);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
			pCell->SetText( L"卖二");
			
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(5, 0);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
			pCell->SetText(L"卖一");
			
			
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(6, 0);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
			pCell->SetText(L"买一");
			
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(7, 0);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
			pCell->SetText( L"买二");
			
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(8, 0);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
			pCell->SetText( L"买三");
			
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(9, 0);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
			pCell->SetText(L"买四");
			
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(10, 0);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
			pCell->SetText(L"买五");
			
			
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(11, 0);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
			pCell->SetText(L"现价");
			
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(12, 0);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
			pCell->SetText(L"涨跌");
			
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(13, 0);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
			pCell->SetText(L"涨幅");
			
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(14, 0);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
			pCell->SetText(L"总量");
			
			
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(15, 0);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
			pCell->SetText(L"外盘");
			
			
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(16, 0);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
			//pCell->SetText(L"PE(动)");
			pCell->SetText(L"换手");
			
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(17, 0);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
			//pCell->SetText(L"换手");
			pCell->SetText(L"净资");
			
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(18, 0);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
			//pCell->SetText(L"净资");
			pCell->SetText(L"收益");
			
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(0, 2);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
			pCell->SetText(L"委差");
			
			
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(11, 2);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
			pCell->SetText(L" 今开");
			
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(12, 2);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
			pCell->SetText(L" 最高");
			
			
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(13, 2);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
			pCell->SetText(L" 最低");
			
			
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(14, 2);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
			pCell->SetText(L" 量比");
			
			
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(15, 2);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
			pCell->SetText(L" 内盘");
			
			
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(16, 2);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
			pCell->SetText(L" 股本");
			
			pCell = (CGridCellSys *)m_GridCtrl.GetCell(17, 2);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
			pCell->SetText(L" 流通");
			
 			pCell = (CGridCellSys *)m_GridCtrl.GetCell(18, 2);
 			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
			pCell->SetFont(&fontChinese);
			pCell->SetTextClr(clrChinese);
 			//pCell->SetText(L"收益");
			pCell->SetText(L" PE(动)");
			}			
			break;
		case ERTExp:
		case ERTExpForeign:
			{
				if ( m_aExpMerchs.GetSize() > 0 )
				{
					// 新增7行 = 11 + 7 
					if (!m_GridCtrl.SetColumnCount(2))
						return false;
					if (!m_GridCtrl.SetRowCount(m_aExpMerchs.GetSize() + 11))
						return false;

					if ( bRedGreenExp )
					{
						// 画红绿柱的商品 上涨下跌另外表格
						m_GridCtrl.SetRowCount(m_GridCtrl.GetRowCount()-2);
						m_GridExpRiseFall.SetColumnCount(4);
						m_GridExpRiseFall.SetRowCount(1);
					}


					CGridCellSys * pCell = (CGridCellSys *)m_GridCtrl.GetCell(0, 0);

					int32 iRow = 0;
					for ( iRow=0; iRow < m_aExpMerchs.GetSize() ; iRow++ )
					{
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText( m_aExpMerchs[iRow].first );
					}

					// 原来的
					
					//CGridCellSys * pCell = (CGridCellSys *)m_GridCtrl.GetCell(0, 0);
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow++, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText( L"最新指数");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow++, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"今日开盘");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow++, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"昨日收盘");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow++, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"指数涨跌");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow++, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"指数涨幅");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow++, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"总成交量");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow++, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"总成交额");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow++, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"最高指数");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow++, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"最低指数");				
					
					if ( iRow < m_GridCtrl.GetRowCount() )
					{
						// 可能不需要显示~
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow++, 0);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"上涨家数");
						
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow++, 0);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"下跌家数");
					}
					else
					{
						pCell = (CGridCellSys *)m_GridExpRiseFall.GetCell(0, 0);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"上涨家数");
						
						pCell = (CGridCellSys *)m_GridExpRiseFall.GetCell(0, 2);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"下跌家数");
					}
				}
				else
				{
					if (!m_GridCtrl.SetColumnCount(2))
						return false;
					if (!m_GridCtrl.SetRowCount(11))
						return false;

					if ( bRedGreenExp )
					{
						// 画红绿柱的商品 上涨下跌另外表格
						m_GridCtrl.SetRowCount(m_GridCtrl.GetRowCount()-2);
						m_GridExpRiseFall.SetColumnCount(4);
						m_GridExpRiseFall.SetRowCount(1);
					}
					
					// 设置表格图标
					// m_ImageList.Create(MAKEINTRESOURCE(IDB_GRID_REPORT), 16, 1, RGB(255,255,255));
					// m_GridCtrl.SetImageList(&m_ImageList);
					
					// 设置相互之间的关联
					
					// m_XSBHorz.SetOwner(&m_GridCtrl);
					// m_XSBVert.SetOwner(&m_GridCtrl);
					// m_GridCtrl.SetScrollBar(&m_XSBHorz, &m_XSBVert);
					
					CGridCellSys * pCell = (CGridCellSys *)m_GridCtrl.GetCell(0, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText( L"最新指数");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(1, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"今日开盘");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(2, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"昨日收盘");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(3, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"指数涨跌");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(4, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"指数涨幅");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(5, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"总成交量");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(6, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"总成交额");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(7, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"最高指数");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(8, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"最低指数");				
					
					if ( !bRedGreenExp )
					{
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(9, 0);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"上涨家数");
						
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(10, 0);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"下跌家数");
					}
					else
					{
						pCell = (CGridCellSys *)m_GridExpRiseFall.GetCell(0, 0);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"上涨家数");
						
						pCell = (CGridCellSys *)m_GridExpRiseFall.GetCell(0, 2);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"下跌家数");
					}
				}
			}
			break;
		case ERTWarrantCn:
		case ERTWarrantHk:			
			{
				if (!m_GridCtrl.SetColumnCount(4))
					return false;
				if (!m_GridCtrl.SetRowCount(21))
					return false;

				
				// 设置表格图标
				// m_ImageList.Create(MAKEINTRESOURCE(IDB_GRID_REPORT), 16, 1, RGB(255,255,255));
				// m_GridCtrl.SetImageList(&m_ImageList);
				
				// 设置相互之间的关联
				
				// m_XSBHorz.SetOwner(&m_GridCtrl);
				// m_XSBVert.SetOwner(&m_GridCtrl);
				// m_GridCtrl.SetScrollBar(&m_XSBHorz, &m_XSBVert);
								
				///////////////////////////////////////////////// 
				
				CGridCellSys *pCell = (CGridCellSys *)m_GridCtrl.GetCell(0, 0);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText(L"委比");
				
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(1, 0);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText(L"卖五");
				
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(2, 0);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText(L"卖四");
				
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(3, 0);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText( L"卖三");
				
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(4, 0);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText( L"卖二");
				
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(5, 0);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText(L"卖一");				
				
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(6, 0);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText(L"买一");
				
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(7, 0);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText( L"买二");
				
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(8, 0);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText( L"买三");
				
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(9, 0);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText(L"买四");
				
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(10, 0);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText(L"买五");
				
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(11, 0);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText(L"现价");
				
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(12, 0);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText(L"涨跌");
				
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(13, 0);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText(L"涨幅");
				
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(14, 0);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText(L"总量");
				
				
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(15, 0);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText(L"外盘");
				
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(16, 0);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText(L"涨停");
				
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(17, 0);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText(L"换手");
				
				//pCell = (CGridCellSys *)m_GridCtrl.GetCell(18, 0);
				//pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				//pCell->SetText(L"价值");

				pCell = (CGridCellSys *)m_GridCtrl.GetCell(19, 0);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText(L"到期日");

				pCell = (CGridCellSys *)m_GridCtrl.GetCell(20, 0);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText(L"行权比");
							
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(0, 2);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText(L"委差");
				
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(11, 2);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText(L"今开");
				
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(12, 2);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText(L"最高");
								
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(13, 2);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText(L"最低");
							
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(14, 2);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText(L"量比");
				
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(15, 2);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText(L"内盘");
				
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(16, 2);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText(L"跌停");
				
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(17, 2);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText(L"杠杆");
				
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(18, 2);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText(L"溢价");
				
				pCell = (CGridCellSys *)m_GridCtrl.GetCell(20, 2);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
				pCell->SetFont(&fontChinese);
				pCell->SetTextClr(clrChinese);
				pCell->SetText(L"行权价");
				
			}
			break;
		case ERTFuturesCn:
		case ERTFuturesForeign:
		case ERTFutureRelaMonth:
		case ERTFutureSpot:				
		case ERTFutureGold:
		case ERTMony:  //外汇改为和现货一样
			{				
				if ( BeSpecial() )	
				{
					if (!m_GridCtrl.SetColumnCount(4))
						return false;
					if (!m_GridCtrl.SetRowCount(17))
						return false;			
					
					CGridCellSys* pCell = (CGridCellSys *)m_GridCtrl.GetCell(0, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"卖五");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(1, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"卖四");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(2, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText( L"卖三");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(3, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText( L"卖二");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(4, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"卖一");
					
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(5, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"买一");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(6, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText( L"买二");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(7, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText( L"买三");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(8, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"买四");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(9, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"买五");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(10, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"最新");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(11, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText( L"涨跌");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(12, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText( L"幅度");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(13, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"总手");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(14, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"现手");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(15, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText( L"持仓");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(16, 0);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"外盘");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(10, 2);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"均价");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(11, 2);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"昨结");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(12, 2);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"开盘");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(13, 2);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"最高");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(14, 2);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"最低");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(15, 2);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"仓差");
					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(16, 2);
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
					pCell->SetFont(&fontChinese);
					pCell->SetTextClr(clrChinese);
					pCell->SetText(L"内盘");
				}
				else
				{
					int32 iRow = 0;
					CGridCellSys *pCell = NULL;

					if (!m_GridCtrl.SetColumnCount(4))
						return false;

					if ( ERTFuturesCn == eMerchKind )
					{
						int32 iItem = m_iBuySellCnt;
						if (m_bIsShowBuySell)
						{
							if ( IsNeedShowFutureCnTick() )
							{
								m_GridCtrl.SetRowCount(7+5+iItem*2);
							}
							else
							{
								m_GridCtrl.SetRowCount(7+1+iItem*2);
							}
						}
						else
						{
							if ( IsNeedShowFutureCnTick() )
							{
								m_GridCtrl.SetRowCount(9+5);
							}
							else
							{
								m_GridCtrl.SetRowCount(9+1);
							}
						}


						// 委比-委差
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"委比");
						
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 2);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"委差");
						++iRow;

						if (m_bIsShowBuySell)
						{
							if ( bBuySellPriceExchange )
							{
								TCHAR szBuy[5][5] = { L"买一", L"买二", L"买三", L"买四", L"买五"};
								for(int i = 0; i < iItem; i++)
								{
									pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow++, 0);
									pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
									pCell->SetFont(&fontChinese);
									pCell->SetTextClr(clrChinese);
									pCell->SetText(szBuy[(iItem - 1) - i]);
								}	
							}
							else
							{
								TCHAR szSell[5][5] = { L"卖一", L"卖二", L"卖三", L"卖四", L"卖五"};
								for(int i = 0; i < iItem; i++)
								{
									pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow++, 0);
									pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
									pCell->SetFont(&fontChinese);
									pCell->SetTextClr(clrChinese);
									pCell->SetText(szSell[(iItem - 1) - i]);
								}
							}
						
							if ( bBuySellPriceExchange )
							{
								TCHAR szSell[5][5] = { L"卖一", L"卖二", L"卖三", L"卖四", L"卖五"};
								for(int i = 0; i < iItem; i++)
								{
									pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow++, 0);
									pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
									pCell->SetFont(&fontChinese);
									pCell->SetTextClr(clrChinese);
									pCell->SetText(szSell[i]);
								}	
							}
							else
							{
								TCHAR szBuy[5][5] = { L"买一", L"买二", L"买三", L"买四", L"买五"};
								for(int i = 0; i < iItem; i++)
								{
									pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow++, 0);
									pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
									pCell->SetFont(&fontChinese);
									pCell->SetTextClr(clrChinese);
									pCell->SetText(szBuy[i]);
								}
							}
						}
						else
						{
							// 0
							pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
							pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
							pCell->SetFont(&fontChinese);
							pCell->SetTextClr(clrChinese);
							if ( bBuySellPriceExchange )
							{
								pCell->SetText(L"买入");
							}
							else
							{
								pCell->SetText(L"卖出");
							}
							++iRow;

							// 1
							pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
							pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
							pCell->SetFont(&fontChinese);
							pCell->SetTextClr(clrChinese);
							if ( bBuySellPriceExchange )
							{
								pCell->SetText(L"卖出");
							}
							else
							{
								pCell->SetText(L"买入");
							}
							++iRow;
						}
						
						// 2
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"最新");
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 2);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"均价");
						++iRow;
						
						// 3
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText( L"涨跌");
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 2);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"昨结");
						++iRow;
						
						// 4
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText( L"幅度");
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 2);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"开盘");
						++iRow;
						
						// 5
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"总手");
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 2);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"最高");
						++iRow;
						
						// 6
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"现手");
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 2);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"最低");
						++iRow;
						
						// 7
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText( L"持仓");
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 2);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"仓差");
						++iRow;
						
						// 				pCell = (CGridCellSys *)m_GridCtrl.GetCell(8, 0);
						// 				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						// 				pCell->SetText( L"委比");
						
						// 8
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"外盘");
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 2);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"内盘");
						++iRow;
					}
					else if(ERTFutureSpot == eMerchKind || ERTMony == eMerchKind)
					{
						int32 iItem = m_iBuySellCnt;
						if (m_bIsShowBuySell)
						{
							if (!m_GridCtrl.SetRowCount(4+iItem*2))
								return false;
						}
						else
						{
							if (!m_GridCtrl.SetRowCount(4))
								return false;
						}
				
						///////////////////////////////////////////////// 
						if (m_bIsShowBuySell)
						{
							TCHAR szSell[5][5] = { L"卖一", L"卖二", L"卖三", L"卖四", L"卖五"};
							for(int i = 0; i < iItem; i++)
							{
								pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow++, 0);
								pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
								pCell->SetFont(&fontChinese);
								pCell->SetTextClr(clrChinese);
								pCell->SetText(szSell[(iItem - 1) - i]);
							}

							TCHAR szBuy[5][5] = { L"买一", L"买二", L"买三", L"买四", L"买五"};
							for(int i = 0; i < iItem; i++)
							{
								pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow++, 0);
								pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
								pCell->SetFont(&fontChinese);
								pCell->SetTextClr(clrChinese);
								pCell->SetText(szBuy[i]);
							}
						}

						CString strNewPrice=L"最新";
						if(ERTMony == eMerchKind)
						{
							CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
							strNewPrice = pApp->m_pConfigInfo->m_strNewPrice;
						}
						// 1
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(strNewPrice);
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 2);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"均价");
						++iRow;
						
						// 2
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText( L"涨跌");
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 2);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						// zhangtao
						E_ReportType eTmpMerchKind = GetMerchKind(m_pMerchXml);
						if (ERTMony == eTmpMerchKind)// 判断是否是“做市商”
						{
							BUY_SELL_PLATE nType = CConfigInfo::Instance()->m_nBuySellPlate;
							if (YESTERDAY_SETTLEMENT == nType)
								pCell->SetText(L"昨结");
							else if (YESTERDAY_CLOSE == nType)
								pCell->SetText(L"昨收");
						}
						else
						{
							pCell->SetText(L"昨结");
						}
						++iRow;
						
						// 3
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText( L"幅度");
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 2);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"开盘");
						++iRow;
						
						//4
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"最高");
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 2);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"最低");
						++iRow;						
					}
					else 
					{
						int32 iItem = m_iBuySellCnt;
						if (m_bIsShowBuySell)
						{
							if (!m_GridCtrl.SetRowCount(4+iItem*2))
								return false;
						}
						else
						{
							if (!m_GridCtrl.SetRowCount(5))
								return false;
						}
						///////////////////////////////////////////////// 
						
						// 0

						if (m_bIsShowBuySell)
						{
							if ( bBuySellPriceExchange )
							{
								TCHAR szBuy[5][5] = { L"买一", L"买二", L"买三", L"买四", L"买五"};
								for(int i = 0; i < iItem; i++)
								{
									pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow++, 0);
									pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
									pCell->SetFont(&fontChinese);
									pCell->SetTextClr(clrChinese);
									pCell->SetText(szBuy[(iItem - 1) - i]);
								}	
							}
							else
							{
								TCHAR szSell[5][5] = { L"卖一", L"卖二", L"卖三", L"卖四", L"卖五"};
								for(int i = 0; i < iItem; i++)
								{
									pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow++, 0);
									pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
									pCell->SetFont(&fontChinese);
									pCell->SetTextClr(clrChinese);
									pCell->SetText(szSell[(iItem - 1) - i]);
								}
							}

							if ( bBuySellPriceExchange )
							{
								TCHAR szSell[5][5] = { L"卖一", L"卖二", L"卖三", L"卖四", L"卖五"};
								for(int i = 0; i < iItem; i++)
								{
									pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow++, 0);
									pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
									pCell->SetFont(&fontChinese);
									pCell->SetTextClr(clrChinese);
									pCell->SetText(szSell[i]);
								}	
							}
							else
							{
								TCHAR szBuy[5][5] = { L"买一", L"买二", L"买三", L"买四", L"买五"};
								for(int i = 0; i < iItem; i++)
								{
									pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow++, 0);
									pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
									pCell->SetFont(&fontChinese);
									pCell->SetTextClr(clrChinese);
									pCell->SetText(szBuy[i]);
								}
							}
						}
						else
						{
							pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
							pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
							pCell->SetFont(&fontChinese);
							pCell->SetTextClr(clrChinese);
							if ( bBuySellPriceExchange )
							{
								pCell->SetText(L"买入");
							}
							else
							{
								pCell->SetText(L"卖出");
							}

							pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 2);
							pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
							pCell->SetFont(&fontChinese);
							pCell->SetTextClr(clrChinese);
							if ( bBuySellPriceExchange )
							{
								pCell->SetText(L"卖出");
							}
							else
							{
								pCell->SetText(L"买入");
							}
							++iRow;
						}
												
						// 1
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"最新");
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 2);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"均价");
						++iRow;
						
						// 2
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText( L"涨跌");
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 2);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"昨结");
						++iRow;
						
						// 3
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText( L"幅度");
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 2);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"开盘");
						++iRow;

						//4
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"最高");
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 2);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"最低");
					}

					if ( ERTFuturesCn == eMerchKind  && IsNeedShowFutureCnTick() )
					{
						// 增仓 x 性质 x
						// 开仓 x 平仓 x
						// 多开 x 多平 x
						// 空开 x 空平 x
						int32 iTmpRow = 10;
						if (m_bIsShowBuySell)
						{
							int32 iItem = m_iBuySellCnt;
							iTmpRow += (iItem-1)*2; 
						}

						// 增仓 性质
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iTmpRow, 0);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"增仓");
						
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iTmpRow, 2);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"性质");
						++iTmpRow;

						// 开仓 x 平仓 x
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iTmpRow, 0);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"开仓");
						
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iTmpRow, 2);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"平仓");
						++iTmpRow;

						// 多开 x 空开 x
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iTmpRow, 0);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"多开");
						
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iTmpRow, 2);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"多平");
						++iTmpRow;

						// 多平 x 空平 x
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iTmpRow, 0);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"空开");
						
						pCell = (CGridCellSys *)m_GridCtrl.GetCell(iTmpRow, 2);
						pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
						pCell->SetFont(&fontChinese);
						pCell->SetTextClr(clrChinese);
						pCell->SetText(L"空平");
						++iTmpRow;

						ASSERT( iTmpRow == m_GridCtrl.GetRowCount() );
					}
				}				
			}
			break;
		default:
			{
				return false;
			}
			break;
	}
/*	
	CRect rect;
	GetClientRect(&rect); 	 
	//int32 iPerfectHeight = (m_GridCtrl.GetRowCount()*IOVIEWPRICEROWHEIGHT); 
	int32 iPerfectHeight = (m_GridCtrl.GetRowCount()*m_iRowHeight); 
	if (rect.Height() > iPerfectHeight)
	{
		m_GridCtrl.ExpandColumnsToFit();
		m_GridCtrl.ExpandToFit();
	}
*/

	// 指数类的左右边距调整
	int iColumn = m_GridCtrl.GetColumnCount();
	ASSERT(iColumn > 0);
	for (int32 i=0; i<m_GridCtrl.GetRowCount(); i++)
	{		
		CGridCellBase* pCell1 = m_GridCtrl.GetCell(i, 0);
		if (NULL != pCell1)
		{
			pCell1->SetTextPadding(CRect(5,0,0,0));
		}

		pCell1 = m_GridCtrl.GetCell(i, iColumn - 1);
		if (NULL != pCell1)
		{
			pCell1->SetTextPadding(CRect(0,0,5,0));
		}
	}



	if (m_iScreenHeight < MIX_SCREEN_HEIGHT)
	{	
		LOGFONT *pFontSmall = GetIoViewFont(ESFSmall);
		int32 iRowInit = 0;
		for ( iRowInit=0; iRowInit < m_GridCtrl.GetRowCount() ; iRowInit++ )
		{
			for (int ColuInit = 0; ColuInit < m_GridCtrl.GetColumnCount(); ColuInit++)
			{
				CGridCellSys * pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRowInit, ColuInit);
				pCell->SetFont(pFontSmall);
			}	
		}
	}

	// linhc 0908 
	// 设置标志， 使表格隐藏的时候还要画线
	m_GridCtrl.SetHideTextFlag(false);

	// 设置行高
	SetRowHeightAccordingFont();

	m_GridExpRiseFall.ShowWindow(bRedGreenExp ? SW_SHOW : SW_HIDE);
	m_GridCtrl.SetUserCB(NULL);

	return true;
}

void CIoViewPrice::OnIoViewActive()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();

	if ( NULL == pMainFrame || pMainFrame->BeFromXml() )
	{
		return;
	}

	m_bActive = IsActiveInFrame();

	RedrawWindow();
	//OnVDataForceUpdate();

	RequestLastIgnoredReqData();

	if (NULL != m_GridCtrl.GetSafeHwnd())
	{
		m_GridCtrl.SetFocus();
	}

	SetChildFrameTitle();

	m_CtrlTitle.SetActiveFlag(true);	
	m_CtrlTitle.RedrawWindow();
}

void CIoViewPrice::OnIoViewDeactive()
{   
	if ( m_bActive )
	{
		Invalidate(TRUE);
	}
	m_bActive = false;

	m_CtrlTitle.SetActiveFlag(false);
	m_CtrlTitle.RedrawWindow();
}

void CIoViewPrice::LockRedraw()
{
	if ( m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = true;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
	::SendMessage(m_GridCtrl.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
	::SendMessage(m_CtrlTitle.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
	::SendMessage(m_GridExpRiseFall.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
}

void CIoViewPrice::UnLockRedraw()
{
	if ( !m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = false;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
	::SendMessage(m_GridCtrl.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
	::SendMessage(m_CtrlTitle.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
	::SendMessage(m_GridExpRiseFall.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
}

void CIoViewPrice::OnTimer(UINT nIDEvent)
{
	if(!m_bShowNow)
	{
		return;
	}

	if (nIDEvent == SNAPSHOTTIMER)
	{		
		if ( IsWindowVisible() )
		{
			if (NULL == m_pMerchXml)
				return;
			
			m_aMarketIdArray.RemoveAll();
			
			if (m_eMerchKind == ERTExp)
			{	
				CMarket &Market = m_pMerchXml->m_Market;
				if ( 0 == Market.m_MarketInfo.m_iMarketId
					|| 1000 == Market.m_MarketInfo.m_iMarketId  )
				{
					// 请求general推送
					CMmiReqAddPushGeneralNormal reqGPush;
					reqGPush.m_iMarketId = Market.m_MarketInfo.m_iMarketId;
					DoRequestViewData(reqGPush);	// 若干分钟还需要重新补充推送请求~~
				}
				else
				{
					// 其它指数
					for (int32 i = 0; i < Market.m_Breed.m_MarketListPtr.GetSize(); i++)
					{
						if (Market.m_Breed.m_MarketListPtr[i]->m_MarketInfo.m_eMarketReportType != ERTExp)
						{
							CMmiReqMarketSnapshot Req;
							Req.m_iMarketId	= Market.m_Breed.m_MarketListPtr[i]->m_MarketInfo.m_iMarketId;
							m_aMarketIdArray.Add(Req.m_iMarketId);
							DoRequestViewData(Req);
						}				
					}
				}
				
				int32 iRiseCounts = CalcRiseFallCounts(true);
				int32 iFallCounts = CalcRiseFallCounts(false);
				ShowRiseFallCounts(iRiseCounts,iFallCounts);
			}
		}
	}
	CIoViewBase::OnTimer(nIDEvent);
}
	
int32 CIoViewPrice::CalcRiseFallCounts(bool32 bRise)
{
	int32 iCount = 0;
	if ( NULL == m_pMerchXml || NULL == m_pAbsCenterManager)
	{
		return 0;
	}
	
	CMarket &Market = m_pMerchXml->m_Market;
	if ( 0 == Market.m_MarketInfo.m_iMarketId
		|| 1000 == Market.m_MarketInfo.m_iMarketId  )
	{
		// 沪深general
		const CAbsCenterManager::GeneralNormalArray* paGeneralNormal = NULL;
		if ( 0 == Market.m_MarketInfo.m_iMarketId )
		{
			paGeneralNormal = &m_pAbsCenterManager->GetGeneralNormalArrayH();
		}
		else
		{
			paGeneralNormal = &m_pAbsCenterManager->GetGeneralNormalArrayS();
		}
		if ( paGeneralNormal != NULL && paGeneralNormal->GetSize() > 0 )
		{
			if ( bRise )
			{
				iCount = (*paGeneralNormal)[paGeneralNormal->GetUpperBound()].m_uiRiseCounts;
			}
			else
			{
				iCount = (*paGeneralNormal)[paGeneralNormal->GetUpperBound()].m_uiFallCounts;
			}
		}
	}
	else
	{
		// 其它指数
		for (int32 i =0 ;i < m_aMarketIdArray.GetSize(); i++)
		{
			CMarket  * pMarket = NULL ;
			m_pAbsCenterManager->GetMerchManager().FindMarket(m_aMarketIdArray[i],pMarket);
			
			if (NULL == pMarket)
			{
				continue;
			}
			
			if (-1 == pMarket->m_MarketSnapshotInfo.m_iMarketId)
			{
				continue;
			}
			
			if (bRise)
			{
				if ( -1 == pMarket->m_MarketSnapshotInfo.m_iRiseCount )
				{
					continue;
				}
			}
			else
			{
				if ( -1 == pMarket->m_MarketSnapshotInfo.m_iFallCount )
				{
					continue;
				}			
			}
			
			if (bRise)
			{
				iCount += pMarket->m_MarketSnapshotInfo.m_iRiseCount;
			}
			else
			{
				iCount += pMarket->m_MarketSnapshotInfo.m_iFallCount;
			}		
		}
	}
	
	return iCount;
}

void CIoViewPrice::ShowRiseFallCounts(int32 iRiseCounts,int32 iFallCounts)
{
	if (iRiseCounts < 0 || iFallCounts < 0)
	{
		return;
	}

	bool32 bH;
	bool32 bRedGreenExp = BeGeneralIndex(bH);
	
	// 显示
	if ( !bRedGreenExp )
	{
		const int32 iAppend = m_aExpMerchs.GetSize(); // 新增指数行
		E_ReportType eType = m_pMerchXml->m_Market.m_MarketInfo.m_eMarketReportType;
		bool32 bExp = NULL != m_pMerchXml && (ERTExp==eType||ERTExpForeign==eType);
		if ( m_GridCtrl.GetRowCount() > 10+iAppend && bExp )
		{
			m_GridCtrl.SetCellType(9 + iAppend, 1, RUNTIME_CLASS(CGridCellSymbol));
			CGridCellSymbol * pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(9 + iAppend, 1);	
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrRiseCount = Float2SymbolString( (float)iRiseCounts, -1000.00, 0);	
			pCellSymbol->SetText(StrRiseCount);			
			pCellSymbol->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
			//
			m_GridCtrl.SetCellType(10 + iAppend, 1, RUNTIME_CLASS(CGridCellSymbol));
			pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(10 + iAppend, 1);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			CString StrFallCount = Float2SymbolString( (float)iFallCounts, 10000.00, 0);
			pCellSymbol->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
			pCellSymbol->SetText(StrFallCount);
		}
		else
		{
			ASSERT( 0 );	// 上涨下跌表格未初始化
		}
	}
	else
	{
		// 显示在另外的表格中
		m_GridExpRiseFall.SetCellType(0, 1, RUNTIME_CLASS(CGridCellSymbol));
		CGridCellSymbol * pCellSymbol = (CGridCellSymbol *)m_GridExpRiseFall.GetCell(0, 1);	
		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		CString StrRiseCount = Float2SymbolString( (float)iRiseCounts, -1000.00, 0);	
		StrRiseCount += _T(" ");
		pCellSymbol->SetText(StrRiseCount);			
		pCellSymbol->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
		//
		m_GridExpRiseFall.SetCellType(0, 3, RUNTIME_CLASS(CGridCellSymbol));
		pCellSymbol = (CGridCellSymbol *)m_GridExpRiseFall.GetCell(0, 3);
		pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
		pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		CString StrFallCount = Float2SymbolString( (float)iFallCounts, 10000.00, 0);
		pCellSymbol->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
		pCellSymbol->SetText(StrFallCount);
	}
    
	if ( !m_bLockRedraw )
	{
		m_GridCtrl.RedrawWindow();		
	}
}

void CIoViewPrice::SetChildFrameTitle()
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

//右键买卖盘列表菜单显示 linhc20100904
void CIoViewPrice::OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult)
{ 
	CNewMenu menu;	
	menu.LoadMenu(IDR_MENU_PRICE);
	
	menu.LoadToolBar(g_awToolBarIconIDs);
	CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));

	if (NULL == pPopMenu)
	{
		return;
	}
	CMenu* pTempMenu = pPopMenu->GetSubMenu(L"插入内容");

	if (NULL == pTempMenu)
	{
		return;
	}
	CNewMenu * pIoViewPopMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );
	ASSERT(NULL != pIoViewPopMenu );
	if (NULL == pIoViewPopMenu)
	{
		return;
	}
	AppendIoViewsMenu(pIoViewPopMenu, IsLockedSplit());
	
	// 自选股

	// 如果处在锁定分割状态，需要删除一些按钮
	CMPIChildFrame *pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pChildFrame && pChildFrame->IsLockedSplit() )
	{
		pChildFrame->RemoveSplitMenuItem(*pPopMenu);

		// 锁定大小暂时不处理
	}
	
	
	CPoint pt;
	GetCursorPos(&pt);
	pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, AfxGetMainWnd());	
	pPopMenu->DestroyMenu();			
}

void CIoViewPrice::OnGridLButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	//NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	//ASSERT(NULL != pItem);
}

//linhc 20100914 添加服务响应
void CIoViewPrice::OnVDataPublicFileUpdate( IN CMerch *pMerch, E_PublicFileType ePublicFileType )
{
   	if ( EPFTF10 != ePublicFileType )
	{
		return;
	}
	
	if ( NULL == pMerch || NULL == pMerch->m_pFinanceData )
	{
		ASSERT( 0 );
		return;
	} 	
	OnVDataRealtimePriceUpdate(pMerch);
}

// fangz 1015 for bohai
bool32 CIoViewPrice::BeSpecial()
{
	if ( NULL == m_pMerchXml )
	{
		return false;
	}

	if ( m_pMerchXml->m_MerchInfo.m_StrMerchCode == L"BCK" 
	  || m_pMerchXml->m_MerchInfo.m_StrMerchCode == L"BCO" 
	  || m_pMerchXml->m_MerchInfo.m_StrMerchCode == L"BSC" 
	  || m_pMerchXml->m_MerchInfo.m_StrMerchCode == L"BRBW" 
	  || m_pMerchXml->m_MerchInfo.m_StrMerchCode == L"BRC" 
	   )
	{
		return true;
	}

	return false;
}

CIoViewPrice::E_MerchExpType CIoViewPrice::GetMerchExpType( CMerch *pMerch )
{
	if ( NULL != pMerch )
	{
		if ( GetMerchKind(pMerch) == ERTExp )
		{
			if ( 0 == pMerch->m_MerchInfo.m_iMarketId )			// 写死上证指数与深证指数市场
			{
				return EMET_ShangZhengZhiShu;
			}
			else if ( 1000 == pMerch->m_MerchInfo.m_iMarketId )
			{
				return EMET_ShenZhenZhiShu;
			}
		}
	}
	return EMET_Count;
}

bool32 CIoViewPrice::GetAppendExpMerchArray( E_MerchExpType eMET, OUT AppendExpMerchArray &aExpMerchs )
{
// 		上海、深证指数买卖盘增加显示字段：
// 		主要目的：1、填充股票和指数切换时的买卖盘与分笔之间空隙
// 		2、增加显示内容，帮助客户了解大盘概况
// 		深证指数和上海指数分别增加如下八个字段，分为两栏显示，后面显示成交金额
// 		沪深A股                      沪深A股 深市399101+399102+399107+沪市000002
// 		沪深B股                      沪深B股 深市399108+沪市000003
// 		沪深基金                      沪深基金  深市399305+沪市000011
// 		中 小 板                      中 小 板  深市399101
// 		创 业 板                      创 业 板  深市399102
// 		------------                       ------------     
// 		深证A股 399101+399102+399107 上证A股  000002 
// 		深证B股  399108              上证B股  000003
// 		
	aExpMerchs.RemoveAll();

	if(NULL == m_pAbsCenterManager)
	{
		return false;
	}

	if ( EMET_ShangZhengZhiShu == eMET || EMET_ShenZhenZhiShu == eMET )
	{
		CMerch *pMerch = NULL;
		ExpMerchPair emp;
		emp.first = _T("沪深Ａ股");
		if ( m_pAbsCenterManager->GetMerchManager().FindMerch(_T("399101"), 1000, pMerch) )
		{
			emp.second.push_back(pMerch);
		}
		if ( m_pAbsCenterManager->GetMerchManager().FindMerch(_T("399102"), 1000, pMerch) )
		{
			emp.second.push_back(pMerch);
		}
		if ( m_pAbsCenterManager->GetMerchManager().FindMerch(_T("399107"), 1000, pMerch) )
		{
			emp.second.push_back(pMerch);
		}
		if ( m_pAbsCenterManager->GetMerchManager().FindMerch(_T("000002"), 0, pMerch) )
		{
			emp.second.push_back(pMerch);
		}
		aExpMerchs.Add(emp);
		
		emp.second.clear();
		emp.first = _T("沪深Ｂ股");
		if ( m_pAbsCenterManager->GetMerchManager().FindMerch(_T("399108"), 1000, pMerch) )
		{
			emp.second.push_back(pMerch);
		}
		if ( m_pAbsCenterManager->GetMerchManager().FindMerch(_T("000003"), 0, pMerch) )
		{
			emp.second.push_back(pMerch);
		}
		aExpMerchs.Add(emp);
		
		emp.second.clear();
		emp.first = _T("沪深基金");
		if ( m_pAbsCenterManager->GetMerchManager().FindMerch(_T("399105"), 1000, pMerch) )
		{
			emp.second.push_back(pMerch);
		}
		if ( m_pAbsCenterManager->GetMerchManager().FindMerch(_T("000011"), 0, pMerch) )
		{
			emp.second.push_back(pMerch);
		}
		aExpMerchs.Add(emp);
		
		emp.second.clear();
		emp.first = _T("中 小 板");
		if ( m_pAbsCenterManager->GetMerchManager().FindMerch(_T("399101"), 1000, pMerch) )
		{
			emp.second.push_back(pMerch);
		}
		aExpMerchs.Add(emp);
		
		emp.second.clear();
		emp.first = _T("创 业 板");
		if ( m_pAbsCenterManager->GetMerchManager().FindMerch(_T("399102"), 1000, pMerch) )
		{
			emp.second.push_back(pMerch);
		}
		aExpMerchs.Add(emp);

		// 私有部分
		if ( EMET_ShangZhengZhiShu == eMET )
		{
			emp.second.clear();
			emp.first = _T("上证Ａ股");
			if ( m_pAbsCenterManager->GetMerchManager().FindMerch(_T("000002"), 0, pMerch) )
			{
				emp.second.push_back(pMerch);
			}
			aExpMerchs.Add(emp);

			emp.second.clear();
			emp.first = _T("上证Ｂ股");
			if ( m_pAbsCenterManager->GetMerchManager().FindMerch(_T("000003"), 0, pMerch) )
			{
				emp.second.push_back(pMerch);
			}
			aExpMerchs.Add(emp);
		}
		else
		{
			emp.second.clear();
			emp.first = _T("深证Ａ股");
			if ( m_pAbsCenterManager->GetMerchManager().FindMerch(_T("399101"), 1000, pMerch) )
			{
				emp.second.push_back(pMerch);
			}
			if ( m_pAbsCenterManager->GetMerchManager().FindMerch(_T("399102"), 1000, pMerch) )
			{
				emp.second.push_back(pMerch);
			}
			if ( m_pAbsCenterManager->GetMerchManager().FindMerch(_T("399107"), 1000, pMerch) )
			{
				emp.second.push_back(pMerch);
			}
			aExpMerchs.Add(emp);

			emp.second.clear();
			emp.first = _T("深证Ｂ股");
			if ( m_pAbsCenterManager->GetMerchManager().FindMerch(_T("399108"), 1000, pMerch) )
			{
				emp.second.push_back(pMerch);
			}
			aExpMerchs.Add(emp);
		}

		ASSERT( aExpMerchs.GetSize() == 7 );
		return true;
	}

	ASSERT( 0 );
	return false;
}

float CIoViewPrice::GetAppendExpAmout( const ExpMerchPair &expPair )
{
	float fRet = 0.0f;
	const ExpMerchArray &ExpAry = expPair.second;
	for (size_t it = 0; it < ExpAry.size();it++ )
	{
		if ( ExpAry[it] != NULL )
		{
			CMerch *pMerch = ExpAry[it];
			if ( NULL != pMerch->m_pRealtimePrice )
			{
				fRet += pMerch->m_pRealtimePrice->m_fAmountTotal;
			}
		}
		else
		{
			ASSERT( 0 );
		}
	}
	return fRet;
}

void CIoViewPrice::OnMerchMarkChanged( int32 iMarketId, const CString &StrMerchCode, const T_MarkData &MarkOldData )
{
	if ( NULL != m_pMerchXml
		&& m_pMerchXml->m_MerchInfo.m_iMarketId == iMarketId
		&& m_pMerchXml->m_MerchInfo.m_StrMerchCode == StrMerchCode )
	{
		T_MarkData MarkNowData;
		if ( !CMarkManager::Instance().QueryMark(iMarketId, StrMerchCode, MarkNowData)
			|| MarkNowData.m_eType != MarkOldData.m_eType
			|| EMT_Text == MarkNowData.m_eType )
		{
			// 类型变化&文本变化 才要显示更新
			GV_DRAWMARK_ITEM markItem;
			InitDrawMarkItem(MarkNowData, markItem);
			m_CtrlTitle.SetDrawMarkItem(markItem);
			m_CtrlTitle.Invalidate();
		}
	}
}

void CIoViewPrice::OnDestroy()
{
	CMarkManager::Instance().RemoveMerchMarkChangeListener(this);
	m_TipWnd.DestroyWindow();
	CIoViewBase::OnDestroy();
}

BOOL CIoViewPrice::PreTranslateMessage( MSG* pMsg )
{
	if ( m_TipWnd.IsWindowVisible() )
	{
		m_TipWnd.PreTranslateMessage(pMsg);
	}

	return CIoViewBase::PreTranslateMessage(pMsg);
}

bool32 CIoViewPrice::BeGeneralIndex(OUT bool32& bH)
{
	bH = false;
	return false;
	
	//
	if ( NULL == m_pMerchXml )
	{
		return false;
	}
	
	if ( 0 == m_pMerchXml->m_MerchInfo.m_iMarketId && L"000001" == m_pMerchXml->m_MerchInfo.m_StrMerchCode )
	{
		bH = true;
		return true;
	}
	else if ( 1000 == m_pMerchXml->m_MerchInfo.m_iMarketId && L"399001" == m_pMerchXml->m_MerchInfo.m_StrMerchCode )
	{
		bH = false;
		return true;
	}
	else if ( 14001 == m_pMerchXml->m_MerchInfo.m_iMarketId )
	{
		bH = false;
		return true;
	}
	else if ( 0x000036b0 == m_pMerchXml->m_MerchInfo.m_iMarketId )
	{
		return true;
	}
	else if ( 0x00000fa1 == m_pMerchXml->m_MerchInfo.m_iMarketId || 0x00000fa0 ==  m_pMerchXml->m_MerchInfo.m_iMarketId )
	{
		return true;
	}
	else if ( 0x000032cb == m_pMerchXml->m_MerchInfo.m_iMarketId  )
	{
		return true;
	}
// 	if(ERTFutureSpot==m_pMerchXml->m_Market.m_MarketInfo.m_eMarketReportType)
// 	{
// 		return true;
// 	}
	return false;
}

void CIoViewPrice::OnGetMinMaxInfo( MINMAXINFO* lpMMI )
{
	CIoViewBase::OnGetMinMaxInfo(lpMMI);

	CGGTongView *pView = GetParentGGtongView();
	if ( NULL!=lpMMI && NULL != pView && pView->IsLockMinMaxInfo() )
	{
		// 是使用缓存的大小还是使用当前计算出来的大小
		// 使用计算的, 必须在这个之前保证对应的setseize已经调用了
		CSize size = GetProperSize();
		size.cy += 5;
		if ( size.cx > 0 && size.cy > 0 )
		{
			lpMMI->ptMinTrackSize.y = lpMMI->ptMaxTrackSize.y = size.cy;
			lpMMI->ptMinTrackSize.x = size.cx;
			// 不锁定最大宽度
		}
	}
}

CSize CIoViewPrice::GetProperSize(OUT float *pfCharWidth/*=NULL*/)
{
	CClientDC dc(this);
	
	int32 iRowCount = m_GridCtrl.GetRowCount();
	int32 iColCount = m_GridCtrl.GetColumnCount();
	if ( 0 == iRowCount || 0 == iColCount)
	{
		return CSize(0, 0);
	}
	
	//
	CFont * pFontNormal = GetIoViewFontObject(ESFNormal);
	CFont *pOldFont = dc.SelectObject(pFontNormal); // font为要得其高宽的字体	
	CSize size = dc.GetTextExtent(L"一二三");
	dc.SelectObject(pOldFont);
	const float fCharWidth = size.cx / 3.0f;

	int32 iViewWidth = int32(size.cx * 4.0f * GRIDZOOMPARAM);
	size.cx = iViewWidth;
	if ( NULL != pfCharWidth )
	{
		*pfCharWidth = fCharWidth;
	}

	// 高度
	size.cy = 62.;
	bool32 bH;
	if (BeSpecialBuySell() && !m_bIsShowBuySell)
	{
		size.cy += KiFixedGeneralShowHeight;	// 红绿条高度
		size.cy += m_GridExpRiseFall.GetVirtualHeight();	// 红绿条显示时，下面单独的显示涨跌家数的高度
	}
	else if (BeGeneralIndex(bH))
	{
		size.cy += m_GridExpRiseFall.GetVirtualHeight();// 显示涨跌家数的高度
	}

	size.cy += m_GridCtrl.GetRowCount()*21;//m_GridCtrl.GetVirtualHeight();
	return size;
}

bool32 CIoViewPrice::IsFutureCn( CMerch *pMerch )
{
	if ( NULL != pMerch )
	{
		return pMerch->m_Market.m_MarketInfo.m_eMarketReportType == ERTFuturesCn;
	}
	return false;
}

bool32 CIoViewPrice::GetFutureCnStartEndTime( CMerch *pMerch, OUT CGmtTime &TimeStart, CGmtTime &TimeEnd )
{
	if ( NULL != pMerch && NULL != m_pAbsCenterManager)
	{
		CMarketIOCTimeInfo IOCInfo;
		if ( pMerch->m_Market.GetRecentTradingDay(m_pAbsCenterManager->GetServerTime(), IOCInfo, pMerch->m_MerchInfo) )
		{
			TimeStart	= IOCInfo.m_TimeInit.m_Time;
			TimeEnd		= IOCInfo.m_TimeEnd.m_Time;
			return true;
		}
	}
	return false;
}

bool32 CIoViewPrice::GetFutureCnStartEndTime( CMerch *pMerch, OUT CMsTime &TimeStart, CMsTime &TimeEnd )
{
	if ( NULL != pMerch && NULL != m_pAbsCenterManager)
	{
		CMarketIOCTimeInfo IOCInfo;
		if ( pMerch->m_Market.GetRecentTradingDay(m_pAbsCenterManager->GetServerTime(), IOCInfo, pMerch->m_MerchInfo) )
		{
			TimeStart	= IOCInfo.m_TimeInit;
			TimeEnd		= IOCInfo.m_TimeEnd;
			return true;
		}
	}
	return false;
}

E_TickProperty CIoViewPrice::AnalyzeTickProperty( const CTick &tick, float fPreHold )
{
	float fHoldAdd		= tick.m_fHold - fPreHold;	// 单笔仓差
	
	// 计算性质显示内容
	E_TickProperty eProperty = ETP_Count;
	if (fHoldAdd == 0.)	// 记录换手
	{
		if (CTick::ETKSell == tick.m_eTradeKind)	
		{
			//m_StrProperty = L"多换";	// 如果当前的成交为外盘，则记录当前开仓为多头换手　（界面上简称为“多换”）
			eProperty = ETP_DuoHuan;
		}
		else if (CTick::ETKBuy == tick.m_eTradeKind)	
		{
			//m_StrProperty = L"空换";	// 如果当前的成交为内盘，则记录当前开仓为空头换手　（界面上简称为“空换”）
			eProperty = ETP_KongHuan;
		}
	}
	else if (tick.m_fVolume == fHoldAdd)	
	{
		//m_StrProperty = L"双开";		// 单笔成交量=单笔仓差，则记录为双边开仓　（界面上简称为“双开”）
		eProperty = ETP_ShuangKai;
	}
	else if (tick.m_fVolume == -fHoldAdd)
	{
		//m_StrProperty = L"双平";		// 单笔成交量=-单笔仓差，则记录为双边平仓　（界面上简称为“双平”）
		eProperty = ETP_ShuangPing;
	}
	else if (fHoldAdd < tick.m_fVolume)
	{
		if (fHoldAdd > 0)
		{
			if (CTick::ETKSell == tick.m_eTradeKind)	
			{
				//m_StrProperty = L"多开";	// 如果当前的成交为外盘，则记录当前开仓为多头开仓　（界面上简称为“多开”）
				eProperty = ETP_DuoKai;
			}
			else if (CTick::ETKBuy == tick.m_eTradeKind)	
			{
				//m_StrProperty = L"空开";	// 如果当前的成交为内盘，则记录当前开仓为空头开仓　（界面上简称为“空开”）
				eProperty = ETP_KongKai;
			}
		}
		else if (fHoldAdd < 0)
		{
			if (CTick::ETKSell == tick.m_eTradeKind)	
			{
				//m_StrProperty = L"空平";	// 如果当前的成交为外盘，则记录为空头平仓　（界面上简称为“空平”）
				eProperty = ETP_KongPing;
			}
			else if (CTick::ETKBuy == tick.m_eTradeKind)	
			{
				//m_StrProperty = L"多平";	// 如果当前的成交为内盘，则记录为多头平仓　（界面上简称为“多平”）
				eProperty = ETP_DuoPing;
			}
		}
	}
	return eProperty;
}

void CIoViewPrice::InitFutureCnTradingDay()
{
	ASSERT( IsFutureCn(m_pMerchXml) );
	ASSERT( NULL != m_pMerchXml );
	CMarketIOCTimeInfo RecentTradingDay;
	bool32 bTradingDayOk = false;
	bTradingDayOk = m_pMerchXml->m_Market.GetRecentTradingDay(m_pAbsCenterManager->GetServerTime(), RecentTradingDay, m_pMerchXml->m_MerchInfo);
	
	if ( bTradingDayOk )
	{
		m_TrendTradingDayInfo.Set(m_pMerchXml, RecentTradingDay);
		m_TrendTradingDayInfo.RecalcPrice(*m_pMerchXml);
		m_TrendTradingDayInfo.RecalcHold(*m_pMerchXml);
	}
}

void CIoViewPrice::RequestFutureCnTicks()
{
	ASSERT( IsFutureCn(m_pMerchXml) );
	ASSERT( IsNeedShowFutureCnTick() );
	ASSERT( NULL != m_pMerchXml );

	CGmtTime TimeStart, TimeEnd;
	if ( !GetFutureCnStartEndTime(m_pMerchXml, TimeStart, TimeEnd) )
	{
		return;
	}

	// 请求指定时间的日K线， 以便获取当前显示走势图的昨收价， 今开价
	{
		ASSERT(m_TrendTradingDayInfo.m_bInit);
		
		// 
		CGmtTime TimeDay = m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeClose.m_Time;
		SaveDay(TimeDay);
		
		CMmiReqMerchKLine info;
		info.m_eKLineTypeBase	= EKTBDay;
		info.m_iMarketId		= m_pMerchXml->m_MerchInfo.m_iMarketId;
		info.m_StrMerchCode		= m_pMerchXml->m_MerchInfo.m_StrMerchCode;
		info.m_eReqTimeType		= ERTYFrontCount;		// 为防止跨天，请求昨天&今天
		info.m_TimeStart		= TimeDay;
		//info.m_TimeEnd			= TimeDay;
		info.m_iFrontCount			= 2;			
		DoRequestViewData(info);
	}

	// 发RealtimePrice请求, 为取昨收价 - 已经发送过了
// 	{
// 		CMmiReqRealtimePrice Req;
// 		Req.m_iMarketId			= m_pMerchXml->m_MerchInfo.m_iMarketId;
// 		Req.m_StrMerchCode		= m_pMerchXml->m_MerchInfo.m_StrMerchCode;
// 		DoRequestViewData(Req);
// 	}
	
	// 请求该商品的数据
	{
		// 普通情况下，申请最新100个数据
		CMmiReqMerchTimeSales info;
		info.m_iMarketId	= m_pMerchXml->m_MerchInfo.m_iMarketId;
		info.m_StrMerchCode = m_pMerchXml->m_MerchInfo.m_StrMerchCode;
		
		info.m_eReqTimeType	= ERTYSpecifyTime;
		info.m_TimeStart	= TimeStart;
		info.m_TimeEnd		= TimeEnd;
		
		DoRequestViewData(info);
	}
	
	// 发RealtimeTick请求
	{
		CMmiReqRealtimeTick Req;
		Req.m_iMarketId			= m_pMerchXml->m_MerchInfo.m_iMarketId;
		Req.m_StrMerchCode		= m_pMerchXml->m_MerchInfo.m_StrMerchCode;
		DoRequestViewData(Req);
	}

	for ( int32 i=0; i < m_aSmartAttendMerchs.GetSize() ; ++i )
	{
		CSmartAttendMerch &smart = m_aSmartAttendMerchs[i];
		if ( smart.m_pMerch == m_pMerchXml )
		{
			smart.m_iDataServiceTypes |= EDSTTimeSale | EDSTTick | EDSTKLine | EDSTPrice;
			break;
		}
	}
}

void CIoViewPrice::OnVDataMerchTimeSalesUpdate( IN CMerch *pMerch )
{
	if ( NULL == pMerch || pMerch != m_pMerchXml || !IsFutureCn(pMerch) || !IsNeedShowFutureCnTick() )
	{
		return;
	}
	
	DWORD dwTime = timeGetTime();
	bool32 bUpdate = UpdateFutureCnTick();
	TRACE(_T("买卖盘-更新分笔: %d ms\r\n"), timeGetTime()-dwTime);
	if ( bUpdate )
	{
		ShowFutureCnData();
	}
}

bool32 CIoViewPrice::UpdateFutureCnTick()
{
	ASSERT( NULL != m_pMerchXml );
	ASSERT( IsFutureCn(m_pMerchXml) );

	if ( NULL == m_pMerchXml )
	{
		return false;
	}
	// 
	CMerch &Merch = *m_pMerchXml;
	if (NULL == Merch.m_pMerchTimeSales)
	{
		ResetFutureCnData();
		return true;
	}
	
	if ( 0 == Merch.m_pMerchTimeSales->m_Ticks.GetSize() )
	{
		ResetFutureCnData();
		return true;
	}

	CMsTime TimeStart, TimeEnd;
	if ( !GetFutureCnStartEndTime(m_pMerchXml, TimeStart, TimeEnd) )
	{
		ResetFutureCnData();
		return false;
	}
	
	// TestRecordData(Merch.m_pMerchTimeSales->m_Ticks);
	
	// 比较数据变化， 对于仅更后面的数据做判断
	int32 iPosDayStart = CMerchTimeSales::QuickFindTickWithBigOrEqualReferTime(Merch.m_pMerchTimeSales->m_Ticks, TimeStart);
	int32 iPosDayEnd = CMerchTimeSales::QuickFindTickWithSmallOrEqualReferTime(Merch.m_pMerchTimeSales->m_Ticks, TimeEnd);
	if (iPosDayStart < 0 || iPosDayStart > iPosDayEnd || iPosDayEnd >= Merch.m_pMerchTimeSales->m_Ticks.GetSize() )
	{
		ResetFutureCnData();
		return true;
	}
	
	// 优化，绝大多数情况下， 该事件都是由于实时分笔数据更新引起的， 对这种情况做特殊判断
	
	CTick *pTickShow	= (CTick *)m_aFutureCnTicks.GetData();
	int32 iCountShow	= m_aFutureCnTicks.GetSize();
	
	CTick *pTickSrc		= (CTick *)Merch.m_pMerchTimeSales->m_Ticks.GetData();
	pTickSrc		   += iPosDayStart;
	int32 iCountSrc		= iPosDayEnd - iPosDayStart + 1;
	ASSERT( iCountSrc > 0 );

	int32 iCalcStartPos = 0;
	if ( iCountShow > 20 && (iCountSrc>=KShowTimeSaleCount || iCountSrc >= iCountShow) )
	{
		// 这段数据是否已经计算过了，这段数据必须完整的出现在现在的数据中
		// 客户端数据处理可能剔除头部数据，所以从尾部开始判断
		CMsTime TimeShowEnd = pTickShow[iCountShow-1].m_TimeCurrent;
		int32 iEndShowPosInSrc = CMerchTimeSales::QuickFindTickWithSmallOrEqualReferTime(pTickSrc, iCountSrc, TimeShowEnd);
		if ( iEndShowPosInSrc >= 15 && iEndShowPosInSrc < iCountSrc )	// 要有足够的数据匹配验证
		{
			// 找到最后一个<=这个时间点的
			while (iEndShowPosInSrc < iCountSrc-1)
			{
				if ( pTickSrc[iEndShowPosInSrc+1].m_TimeCurrent > TimeShowEnd )
				{
					break;
				}
				iEndShowPosInSrc++;
			}

			int32 iMaxTry = 20;
			iMaxTry = min(iMaxTry, iEndShowPosInSrc+1);
			for ( int32 i=0; i < iMaxTry ; ++i )
			{
				int32 iEndNewSrc = iEndShowPosInSrc-i;
				if ( iEndNewSrc >= iCountShow )
				{
					continue;
				}
				const CTick *pTickShowTmp = pTickShow + iCountShow -1 - iEndNewSrc;
				if ( 0 == memcmp((void*)pTickShowTmp, (void*)pTickSrc, (iEndNewSrc+1)*sizeof(CTick)) )
				{
					iCalcStartPos = iEndNewSrc+1;	// 这段数据与开始完全相同，就认为是计算过了
					break;
				}
			}
		}
	}
	
	pTickShow = NULL;
	iCountShow = 0;

	float fPreHold = 0.0f;
	if ( 0 >= iCalcStartPos )
	{
		ResetFutureCnData();	// 导致show数据无效
		fPreHold = m_TrendTradingDayInfo.GetPrevHold();
	}
	else
	{
		fPreHold = pTickSrc[iCalcStartPos-1].m_fHold;
	}
	float fVolT = 0.0f;
	for ( int32 i=iCalcStartPos; i < iCountSrc ; ++i )
	{
		AnalyzeTickData(pTickSrc[i], fPreHold);
		fPreHold = pTickSrc[i].m_fHold;
		fVolT += pTickSrc[i].m_fVolume;
	}
	TRACE(_T("商品：%s 统计分笔成交量: %f\r\n"), m_pMerchXml->m_MerchInfo.m_StrMerchCnName.GetBuffer(), fVolT);

	// 性质
	E_TickProperty eProperty;
	fPreHold	= pTickSrc[iCountSrc-2].m_fHold;
	eProperty	= AnalyzeTickProperty(pTickSrc[iCountSrc-1], fPreHold);
	m_StrFutureCnProperty	= TickPropertyToString(eProperty);
	m_dwFutureCnProperty	= (ESCKeep);
	if (CTick::ETKBuy == pTickSrc[iCountSrc-1].m_eTradeKind)
		m_dwFutureCnProperty = (ESCFall);
	else if (CTick::ETKSell == pTickSrc[iCountSrc-1].m_eTradeKind)
		m_dwFutureCnProperty = (ESCRise);

	m_aFutureCnTicks.SetSize(iCountSrc);
	memcpyex(m_aFutureCnTicks.GetData(), pTickSrc, iCountSrc*sizeof(CTick));

	return true;
}

void CIoViewPrice::OnVDataMerchKLineUpdate( IN CMerch *pMerch )
{
	if ( NULL == pMerch || pMerch != m_pMerchXml || !IsFutureCn(pMerch) )
	{
		return;
	}

	// 需要更新昨收价
	if (0. == m_TrendTradingDayInfo.m_fHoldPrev)
	{
		float fBackupPricePrevHold	= m_TrendTradingDayInfo.GetPrevHold();
		
		m_TrendTradingDayInfo.RecalcHold(*pMerch);
		m_TrendTradingDayInfo.RecalcPrice(*pMerch);
		
		if (fBackupPricePrevHold != m_TrendTradingDayInfo.GetPrevHold())
		{
			// 需要重新计算了？
			ResetFutureCnData();
			OnVDataMerchTimeSalesUpdate(pMerch);
		}
	}
}

void CIoViewPrice::ResetFutureCnData()
{
	m_StrFutureCnProperty.Empty();
	m_dwFutureCnProperty = (ESCKeep);

	m_fVolDuoKai	= 0.0f;
	m_fVolDuoPing	= 0.0f;
	m_fVolKongKai	= 0.0f;
	m_fVolKongPing	= 0.0f;

	m_aFutureCnTicks.RemoveAll();
}

bool32 CIoViewPrice::AnalyzeTickData( const CTick &tick, float fPreHold )
{
	E_TickProperty eProperty = AnalyzeTickProperty(tick, fPreHold);
	if ( eProperty == ETP_Count )
	{
		// 	没有开仓性质的成交判断：
		// 	1、现手=增仓
		// 			与“双开”相同处理
		// 	2、现手=-增仓
		// 			与“双平”相同处理
		// 	3、增仓=0
		// 			A、现价>=昨结   与“多换”相同
		// 			B、现价<昨结    与“空换”相同
		// 	4、增仓>0 且 现手>增仓
		// 			A、现价>=昨结   与“多开”相同
		// 			B、现价<昨结    与“空开”相同
		// 	5、增仓<0 且现手>|增仓|
		// 			A、现价>=昨结   与“空平”相同
		// 			B、现价<昨结    与“多平”相同
		{
			float fDiff = tick.m_fHold - fPreHold;
			int32 iCmp = CompareFloat(fDiff, tick.m_fVolume, 1);
			int32 iCmpNeg = CompareFloat(fDiff, -tick.m_fVolume, 1);
			float fPreAvg = m_TrendTradingDayInfo.GetPrevReferPrice();
			if ( 0 == iCmp )
			{
				// 双开
				eProperty = ETP_ShuangKai;
			}
			else if ( 0 == iCmpNeg )
			{
				// 双平
				eProperty = ETP_ShuangPing;
			}
			else if ( 0.9f >= fDiff && fDiff >= -0.9f )
			{
				if ( tick.m_fPrice >= fPreAvg )
				{
					eProperty = ETP_DuoHuan;
				}
				else
				{
					eProperty = ETP_KongHuan;
				}
			}
			else if ( fDiff > 0.0f )
			{
				if ( tick.m_fPrice >= fPreAvg )
				{
					eProperty = ETP_DuoKai;
				}
				else
				{
					eProperty = ETP_KongKai;
				}
			}
			else
			{
				if ( tick.m_fPrice >= fPreAvg )
				{
					eProperty = ETP_KongPing;
				}
				else
				{
					eProperty = ETP_DuoPing;
				}
			}
		}
	}
	switch ( eProperty )
	{
	case ETP_DuoKai:
	case ETP_DuoPing:
		{
			float fHoldAdd	= tick.m_fHold-fPreHold;
			m_fVolDuoKai	+= (tick.m_fVolume+fHoldAdd)/2;
			m_fVolDuoPing	+= (tick.m_fVolume-fHoldAdd)/2;
		}
		break;
	case ETP_KongKai:
	case ETP_KongPing:
		{
			float fHoldAdd	= tick.m_fHold-fPreHold;
			m_fVolKongKai	+= (tick.m_fVolume+fHoldAdd)/2;
			m_fVolKongPing	+= (tick.m_fVolume-fHoldAdd)/2;
		}
		break;
	case ETP_KongHuan:
		{
			m_fVolKongKai	+= (tick.m_fVolume)/2;
			m_fVolKongPing	+= (tick.m_fVolume)/2;
		}
		break;
	case ETP_ShuangKai:
		{
			m_fVolDuoKai	+= (tick.m_fVolume)/2;
			m_fVolKongKai	+= (tick.m_fVolume)/2;
		}
		break;
	case ETP_ShuangPing:
		{
			m_fVolDuoPing	+= (tick.m_fVolume)/2;
			m_fVolKongPing	+= (tick.m_fVolume)/2;
		}
		break;
	case ETP_DuoHuan:
		{
			m_fVolDuoKai	+= (tick.m_fVolume)/2;
			m_fVolDuoPing	+= (tick.m_fVolume)/2;
		}
		break;
	default:
		// 判断不出
		ASSERT( 0 );
		return false;
	}
	return true;
}

void CIoViewPrice::ShowFutureCnData(bool32 bDraw/*=true*/)
{
	
	if (NULL == m_pMerchXml)	// 不能空视图
		return;
	
	if (!::IsWindow(m_GridCtrl.GetSafeHwnd()))
	{
		return; 
	}

	if ( !IsFutureCn(m_pMerchXml) || !IsNeedShowFutureCnTick() )
	{
		return;
	}

	// 增仓 x *性质 x
	// 开仓 x 平仓 x
	// *多开 x *多平 x
	// *空开 x *空平 x
	int32 iRow = 10;
//	ASSERT( m_GridCtrl.GetRowCount() == 14 );
	if(m_bIsShowBuySell)
	{
		int32 iItem = m_iBuySellCnt;
		iRow += (iItem-1)*2;
	}
	CGridCellSys	*pCell = NULL;

	// 增仓行
	// 性质
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 3);
	if ( NULL != pCell )
	{
		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		pCell->SetText(m_StrFutureCnProperty);
		pCell->SetDefaultTextColor((E_SysColor)m_dwFutureCnProperty);
	}
	++iRow;

	// 开仓行
	++iRow;

	// 多开 多平
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 1);
	if ( NULL != pCell )
	{
		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		CString Str = Float2String(m_fVolDuoKai, 0, true, false);
		pCell->SetText(Str);
		pCell->SetDefaultTextColor(ESCVolume);
	}
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 3);
	if ( NULL != pCell )
	{
		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		CString Str = Float2String(m_fVolDuoPing, 0, true, false);
		pCell->SetText(Str);
		pCell->SetDefaultTextColor(ESCVolume);
	}
	++iRow;

	// 空开 空平
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 1);
	if ( NULL != pCell )
	{
		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		CString Str = Float2String(m_fVolKongKai, 0, true, false);
		pCell->SetText(Str);
		pCell->SetDefaultTextColor(ESCVolume);
	}
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 3);
	if ( NULL != pCell )
	{
		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		CString Str = Float2String(m_fVolKongPing, 0, true, false);
		pCell->SetText(Str);
		pCell->SetDefaultTextColor(ESCVolume);
	}
	++iRow;

	if(m_bIsShowBuySell)
	{
		int32 iItem = m_iBuySellCnt;
		for (int32 i = 0;i<m_GridCtrl.GetColumnCount(); i++)
		{
			CGridCellSys * pCurCell = (CGridCellSys *)m_GridCtrl.GetCell(13+(iItem-1)*2,i);
			pCurCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
		}
	}
	else
	{
		for (int32 i = 0;i<m_GridCtrl.GetColumnCount(); i++)
		{
			CGridCellSys * pTmpCell = (CGridCellSys *)m_GridCtrl.GetCell(13,i);
			pTmpCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD);
		}
	}

	if ( bDraw )
	{
		if ( m_bDelByBiSplitTrack )
		{
			LockRedraw();
			return;
		}
		else
		{
			UnLockRedraw();
		}
		
		if ( GetParentGGTongViewDragFlag() )
		{
			LockRedraw();	
			return;
		}
		else
		{
			UnLockRedraw();		
		}
	
		m_GridCtrl.Invalidate();
	}
}


CString TickPropertyToString( E_TickProperty eProperty )
{
	switch(eProperty)
	{
	case ETP_DuoKai:
		return _T("多开");
		break;
	case ETP_DuoPing:
		return _T("多平");
		break;
	case ETP_DuoHuan:
		return _T("多换");
		break;
	case ETP_KongKai:
		return _T("空开");
		break;
	case ETP_KongPing:
		return _T("空平");
		break;
	case ETP_KongHuan:
		return _T("空换");
		break;
	case ETP_ShuangKai:
		return _T("双开");
		break;
	case ETP_ShuangPing:
		return _T("双平");
		break;
	}
	return _T("");
}

void CIoViewPrice::OnGridEditBegin( NMHDR *pHdr, LRESULT *pResult )
{
	*pResult = -1;	
}

bool32 CIoViewPrice::OnEditEndCB( int32 iRow,int32 iCol, const CString &StrOld, INOUT CString &StrNew )
{	
	return false;
}

void CIoViewPrice::OnGridLDBClick( NMHDR *pHdr, LRESULT *pResult )
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pHdr;
	ASSERT(NULL != pItem);
}

bool32 CIoViewPrice::IsNeedShowFutureCnTick()
{
	// 是否需要分笔统计
	CString StrValue;
	CString StrName = CIoViewManager::GetIoViewString(this);
	if ( CEtcXmlConfig::Instance().ReadEtcConfig(StrName, KStrPriceShowFutureCnTick, NULL, StrValue) )
	{
		return _ttoi(StrValue) != 0;
	}
	return true;
}


bool32 CIoViewPrice::BeSpecialBuySell()
{
	if (NULL == m_pMerchXml)
	{
		return false;
	} 
	
	//
	E_ReportType eMerchKind = GetMerchKind(m_pMerchXml);	
	if (ERTFutureSpot == eMerchKind || ERTMony == eMerchKind)
	{
		return true;
	}
	
	return false;
}

bool32 CIoViewPrice::BeERTMony()
{
	if (NULL == m_pMerchXml)
	{
		return false;
	} 

	//
	E_ReportType eMerchKind = GetMerchKind(m_pMerchXml);	
	if (ERTMony == eMerchKind)
	{
		return true;
	}

	return false;
}

bool32 CIoViewPrice::BeShowBuySell(int32 Marketid)
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	for(unsigned int i = 0; i < pApp->m_pConfigInfo->m_vShowBuySell.size(); i++)
	{
		if(Marketid == pApp->m_pConfigInfo->m_vShowBuySell[i]->m_showBuySellId)
		{
			return true;
		}
	}
	
	return false;
}

int32 CIoViewPrice::GetShowBuySellItem(int32 Marketid)
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	for(unsigned int i = 0; i < pApp->m_pConfigInfo->m_vShowBuySell.size(); i++)
	{
		if(Marketid == pApp->m_pConfigInfo->m_vShowBuySell[i]->m_showBuySellId)
		{
			return pApp->m_pConfigInfo->m_vShowBuySell[i]->m_showItem;
		}
	}

	return 1;
}