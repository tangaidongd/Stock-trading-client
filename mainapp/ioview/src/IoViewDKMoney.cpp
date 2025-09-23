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
#include "IoViewDKMoney.h"
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
static const int32 KShowTimeSaleCount		 = 10000;
static const int32 KNormalShowTimeSaleCount = 100;

static const int32 KChartCycleDiameter = 150;		// 饼图直径不超过

static const int32 KTimerIdPushDataReq = 1002;
static const int32 KTimerPeriodPushDataReq = 1000*60*5;	// 请求数据


#define  FORMULAR_NAME  L"仓位统计"
#define  WORK_LINE		L"工作线"

#define  TITLE_TOP		50;

// #define  PAGE_STR_TITLE				 "多空资金"
// #define  PAGE_STR_STRATEGY_TITLE    "推荐策略："
// #define  PAGE_STR_STRATEGY			"龙战于天"

#define  RGB_COLOR_RED		RGB(0xff,0x38,0x37)
#define  RGB_COLOR_YELLOW	RGB(0xb9,0xfc,0x00)
#define  RGB_COLOR_WHITE	RGB(0xcb,0xeb,0xfa)
#define  FONT_SPACING		1.2				// 字体间隔

/////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CIoViewDKMoney, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewDKMoney, CIoViewBase)
	//{{AFX_MSG_MAP(CIoViewDKMoney)
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
CIoViewDKMoney::CIoViewDKMoney()
:CIoViewBase()
{
	m_bActive = false;
	m_RectWindows = CRect(0,0,0,0);
	m_iDataServiceType  = EDSTKLine;	// General为收到除权信号

	m_Font14.CreateFont(-14, 0,0,0,FW_NORMAL, 0,0,0,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"宋体");

	m_Font12.CreateFont(-14, 0,0,0,FW_BOLD, 0,0,0,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewDKMoney::~CIoViewDKMoney()
{
	m_Font14.DeleteObject();
	m_Font12.DeleteObject();
}


///////////////////////////////////////////////////////////////////////////////
// OnPaint

void CIoViewDKMoney::OnPaint()
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
	COLORREF clrTitle = RGB_COLOR_RED;
	COLORREF clrStrategy = RGB_COLOR_YELLOW;

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
		/*if ( !m_RectTitle.IsRectEmpty() )
		{
			CString StrTitle = CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;
			CFont *pOldFont = NULL;
			pOldFont = dcMem->SelectObject(&m_Font14);
			dcMem->SetTextColor(clrTitle);
			CRect rcTmp(0,0,0,0);
			dcMem->DrawText(StrTitle, m_RectTitle, DT_SINGLELINE |DT_VCENTER |DT_LEFT);
			dcMem->SelectObject(pOldFont);
		}*/

		// 图片
		float fRisePercent = 100;
		float fFallPercent = 35;
		float fValue	 = 500000;

		CFont *pFontOld = NULL; 
		if (NULL != m_pMerchXml)
		{
			pFontOld  = dcMem.SelectObject(&m_Font12);
			CSize sizeWidthNormal = dc.GetTextExtent(_T("字"));
			m_rcPercentPosRed = m_RectRedPillar;
			m_rcPercentPosRed.bottom =  m_RectRedPillar.bottom - m_RectRedPillar.Height() *m_RespDKMoney.m_fMultSide / 100;
			m_rcPercentPosRed.right = m_RectRedPillar.left - 5;
			m_rcPercentPosRed.left = m_rcPercentPosRed.right - 20;
			m_rcPercentPosRed.top = m_rcPercentPosRed.bottom - sizeWidthNormal.cy* FONT_SPACING;

			m_rcPercentPosGreen = m_RectGreenPillar;
			m_rcPercentPosGreen.bottom = m_RectGreenPillar.bottom - m_RectGreenPillar.Height() *m_RespDKMoney.m_fEmpSide / 100;
			m_rcPercentPosGreen.left = m_RectGreenPillar.right + 5;
			m_rcPercentPosGreen.right = m_rcPercentPosGreen.left + sizeWidthNormal.cx*2;
			m_rcPercentPosGreen.top = m_rcPercentPosGreen.bottom -  sizeWidthNormal.cy  * FONT_SPACING;

			CRect rcRed = m_RectImage;
			rcRed.right = m_RectImage.left + 100;
			DrawPillar(dcMem, m_RectRedPillar, m_RespDKMoney.m_fITBF, m_RespDKMoney.m_fMultSide, m_rcPercentPosRed, RGB(246,48,62), RGB(255,76,34));

			CRect rcGreen = m_RectImage;
			rcGreen.left = rcRed.right + 0;
			rcGreen.right = rcGreen.left + 100;
			DrawPillar(dcMem, m_RectGreenPillar, m_RespDKMoney.m_fITSF, m_RespDKMoney.m_fEmpSide, m_rcPercentPosGreen, RGB(20,188,76), RGB(50,204,102));

			// 画线
			CPen cpen;
			cpen.CreatePen(PS_SOLID, 1, RGB(48,48,54)); 
			CPen *pOldPen = dcMem->SelectObject(&cpen);
			POINT pStart, pEnd;
			pStart.x = rcClient.Width() / 10;
			pStart.y = m_RectRedPillar.bottom;
			pEnd.x   = rcClient.Width() / 10 * 9;
			pEnd.y   = m_RectRedPillar.bottom;
			dcMem.MoveTo(pStart);
			dcMem.LineTo(pEnd);
			dcMem->SelectObject(pOldPen);
			dcMem.SelectObject(pFontOld);
		}

		// 画下半部分字体和百分比
		//CRect rcDrawBottom(rcClient);
		//rcDrawBottom.top = m_RectRedPillar.bottom;
		if(NULL != m_pMerchXml)
		{
			pFontOld  = dcMem.SelectObject(&m_Font14);
			DrawBottomText(dcMem, m_RectText, m_RespDKMoney.m_fMultSide, m_RespDKMoney.m_fEmpSide, RGB(230,70,70), RGB(51,204,102));
			dcMem.SelectObject(&pFontOld);
		}
		
	}
	
	dc.BitBlt(0,0, rcClient.Width(),rcClient.Height(),&dcMem,0,0,SRCCOPY);  

	//绘图完成后的清理  
	MemBitmap.DeleteObject();  
	dcMem->RestoreDC(iSaveDC);
}

void CIoViewDKMoney::DrawBottomText(CDC &dc, const CRect &rcItem, float fRise, float fFall, COLORREF clrRise, COLORREF clrFall)
{
	int iSaveDC = dc.SaveDC();
	dc.SetBkMode(TRANSPARENT);
	dc.SetBkColor(GetIoViewColor(ESCBackground));
	CBrush brushRed(RGB(255, 0, 0));
	CBrush* pOldBrush = dc.SelectObject(&brushRed);
	//dc.RoundRect(rcItem, CPoint(17, 17));
	dc.SelectObject(pOldBrush);

	// 多方与空方居中显示
	CFont *pFontNormal = GetIoViewFontObject(ESFNormal);
	CFont *pFontSmall = GetIoViewFontObject(ESFSmall);
	CFont *pOldFont;

	pOldFont = dc.SelectObject(pFontNormal);
	CSize sizeWidthNormal = dc.GetTextExtent(_T("字"));
	dc.SelectObject(pFontSmall);
	CSize sizeWidthSmall = dc.GetTextExtent(_T("字"));
	dc.SelectObject(pOldFont);
	
	dc.SetTextColor(clrRise);
	CRect rcRise = rcItem;


	// 借用红绿柱画中心位置
	int iTestWidth = sizeWidthSmall.cx*FONT_SPACING *2 + sizeWidthSmall.cx*FONT_SPACING + 10;
	int iTextRiseRight = m_RectRedPillar.right - m_RectRedPillar.Width()/2 + iTestWidth/2;
	int iTextFallRight = m_RectGreenPillar.right - m_RectGreenPillar.Width()/2 + iTestWidth/2;

	CRect rcTextRise = rcItem;
	rcTextRise.top  = rcItem.top ;
	rcTextRise.right = iTextRiseRight ;	// 两个字乘以2
	rcTextRise.left = rcTextRise.right - sizeWidthSmall.cx*FONT_SPACING *2;//(rcItem.Width()/2 - sizeWidthSmall.cx*FONT_SPACING)/2;
	rcTextRise.bottom = rcTextRise.top + sizeWidthSmall.cy + 4;

	CRect rcTextFall = rcTextRise;
	rcTextFall.right =  iTextFallRight;
	rcTextFall.left  = rcTextFall.right - sizeWidthSmall.cx*FONT_SPACING *2;//rcItem.Width()/2 + (rcItem.Width()/2 - sizeWidthSmall.cx*FONT_SPACING *2)/2;
	

	CString strDis;
	COLORREF clrText = RGB(220,220,220);
	dc.SelectObject(pFontSmall);
	dc.SetTextColor(clrText);
	dc.DrawText(_T("多方"), rcTextRise, DT_SINGLELINE |DT_VCENTER |DT_LEFT);
	dc.DrawText(_T("空方"), rcTextFall, DT_SINGLELINE |DT_VCENTER |DT_LEFT);
	
	if (rcTextRise.left - sizeWidthSmall.cx*FONT_SPACING >  0)
	{
		// 圆点的的样式为10*10，圆角为5
		CRect rcRedRound = rcTextRise;
		rcRedRound.left = rcTextRise.left - sizeWidthSmall.cx*FONT_SPACING;
		rcRedRound.right = rcRedRound.left + 10;
		rcRedRound.top   = rcRedRound.top +  (rcRedRound.Height() - 10) /2;
		rcRedRound.bottom   = rcRedRound.top +  10;
		
		CRect rcGreenRound = rcRedRound;
		rcGreenRound.left = rcTextFall.left  - sizeWidthSmall.cx*FONT_SPACING;
		rcGreenRound.right = rcGreenRound.left + 10;


		CPen* pOldPen;
		CPen cpenRise, cpenFall;
		cpenRise.CreatePen(PS_SOLID, 1, clrRise); 
		pOldPen = dc.SelectObject(&cpenRise);
		CBrush brushRiseColr(clrRise);
		CBrush* pOldBrush = dc.SelectObject(&brushRiseColr);
		dc.RoundRect(rcRedRound, CPoint(5,5));
		dc.SelectObject(pOldBrush);
		dc.SelectObject(pOldPen);

		cpenFall.CreatePen(PS_SOLID, 1, clrFall);
		pOldPen = dc.SelectObject(&cpenFall);
		CBrush brushFallColr(clrFall);
		pOldBrush = dc.SelectObject(&brushFallColr);
		dc.RoundRect(rcGreenRound, CPoint(5,5));
		dc.SelectObject(pOldBrush);
		dc.SelectObject(pOldPen);
	}

	dc.SelectObject(pOldFont);
}



#define OFFSET_X 2 
#define OFFSET_Y 2 
void DrawRoundRectange(Graphics &g,Color pens,int x,int y,int width,int height)  
{  
	//设置画图时的滤波模式为消除锯齿现象  
	g.SetSmoothingMode(SmoothingModeHighQuality);  

	//创建一个红色的画笔  
	Pen pFillPen(pens);

	//画矩形上面的边  
	g.DrawLine(&pFillPen,x+OFFSET_X,y,x + width-OFFSET_X,y);  

	//画矩形下面的边  
	g.DrawLine(&pFillPen,x+OFFSET_X,y+height,x + width-OFFSET_X,y+height);  

	//画矩形左面的边  
	g.DrawLine(&pFillPen,x,y+OFFSET_Y,x,y+height-OFFSET_Y);  

	//画矩形右面的边  
	g.DrawLine(&pFillPen,x+width,y+OFFSET_Y,x+width,y+height-OFFSET_Y);  

	//画矩形左上角的圆角  
	g.DrawArc(&pFillPen,x,y,OFFSET_X*2,OFFSET_Y*2,180,90);  

	//画矩形右下角的圆角  
	//g.DrawArc(pen,x+width-OFFSET_X*2,y+height-OFFSET_Y*2,OFFSET_X*2,OFFSET_Y*2,0,90);  

	//画矩形右上角的圆角  
	g.DrawArc(&pFillPen,x+width-OFFSET_X*2,y,OFFSET_X*2,OFFSET_Y*2,270,90);  

	//画矩形左下角的圆角  
	//g.DrawArc(pen,x,y+height-OFFSET_Y*2,OFFSET_X*2,OFFSET_Y*2,90,90);   
}  

/* 填充颜色 */  
void FillRoundRectangle(Graphics &g,Color color,int x,int y,int width,int height)  
{  
	//矩形填充的步骤：  
	//1、把圆角矩形画分为四个圆角上分成四个同等的扇形外加三个直角矩形  
	//2、先填充三个直角矩形  
	//3、然后填充四个角上的扇形  

	//创建画刷  
	SolidBrush bFillBrush(color);
	//填充三个直角矩形  


	//填充四个角上的扇形区  
	//填充左上角扇形  
#define PIE_OFFSET 0  
	g.FillPie(&bFillBrush,x,y,OFFSET_X*2+PIE_OFFSET,OFFSET_Y*2+PIE_OFFSET,180,90);  

	//填充右下角的扇形  
	//g.FillPie(brush,x+width-(OFFSET_X*2+PIE_OFFSET),y+height-(OFFSET_Y*2+PIE_OFFSET),OFFSET_X*2+PIE_OFFSET,OFFSET_Y*2+PIE_OFFSET,360,90);  

	//填充右上角的扇形  
	g.FillPie(&bFillBrush,x+width-(OFFSET_X*2+PIE_OFFSET),y,(OFFSET_X*2+PIE_OFFSET),(OFFSET_Y*2+PIE_OFFSET),270,90);  

	//填充左下角的扇形  
	//g.FillPie(brush,x,y+height-(OFFSET_X*2+PIE_OFFSET),(OFFSET_X*2+PIE_OFFSET),(OFFSET_Y*2+PIE_OFFSET),90,90);  

	g.FillRectangle(&bFillBrush,x,y+OFFSET_Y -1 ,width,height-OFFSET_Y*2+2);  
	g.FillRectangle(&bFillBrush,x+OFFSET_X -1 ,y,width-OFFSET_X*2 + 2,height-OFFSET_Y*2+2);  
 
}  



void CIoViewDKMoney::DrawPillar(CDC &dc, const CRect &rcItem, float fValue, float fPercentage, CRect rcPercentPos, COLORREF clrStart, COLORREF clrEnd)
{
	int iSaveDC = dc.SaveDC();
	dc.SetBkMode(TRANSPARENT);
	dc.SetBkColor(GetIoViewColor(ESCBackground));


	Graphics graphics(dc.GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeHighQuality);

	CFont *pFontNormal = GetIoViewFontObject(ESFNormal);
	CFont *pFontSmall  = GetIoViewFontObject(ESFSmall);
	dc.SelectObject(pFontSmall);

	CSize sizeText = dc.GetTextExtent(_T("字"));
	int32 iTextHeight = sizeText.cy + 4;

	// 画柱型
	// 负数也当正数用
	if (fPercentage > 100.00f)
	{
		fPercentage = 100.00f;
	}
	if (fPercentage <  1e-5)
	{
		// 也显示1个像素，避免难看
		fPercentage = 1.00f;
	}

	// 计算百分比坐标
	CRect rcPillar(rcItem);
	rcPillar.top = rcPillar.bottom - rcPillar.Height() *fPercentage / 100;

	int iHeight = rcPillar.Height();
	CColorStep step;
	step.InitColorRange(clrStart, clrEnd, rcPillar.Height());
	for ( int i = 0 ; i <   rcPillar.Height() ; ++i)
	{
		COLORREF colorref  = step.NextColor();
		dc.FillSolidRect(rcPillar.left, rcPillar.bottom - i, rcPillar.Width(), 1, colorref);
		// 最后4个像素自己画一个圆角的矩形盖上去
		if (i > iHeight - 4 && iHeight > 4)
		{	
			Color color1(GetRValue(colorref),GetGValue(colorref),GetBValue(colorref));
			FillRoundRectangle(graphics, color1,rcPillar.left, rcPillar.top ,  rcPillar.Width() -1,rcPillar.Height() - i + 1);
			DrawRoundRectange(graphics, color1,rcPillar.left, rcPillar.top ,  rcPillar.Width() -1,rcPillar.Height() - i + 1);
			break;
		}
	}

	// 显示金额
	//CString StrDisValue;
	//float fTrans = fabsf(fValue);
	//if (fTrans >= 100000000)	// 大于1亿， 以亿为单位
	//	StrDisValue.Format(L"%.2f亿", fValue / 100000000);
	//else if ( fTrans >= 100000)	// 大于10万， 以万为单位
	//	StrDisValue.Format(L"%.1f万", fValue/10000);

	CFont *pOldFont = NULL;
	pOldFont = dc.SelectObject(&m_Font12);
	dc.SetTextColor(RGB(220,220,220));
	CString strPercent;
	strPercent.Format(_T("%.2f%%"),	fPercentage);
	dc.DrawText(strPercent, rcPercentPos, DT_SINGLELINE |DT_CENTER |DT_NOCLIP |DT_VCENTER);
	dc.SelectObject(pOldFont);
	dc.RestoreDC(iSaveDC);
}

void CIoViewDKMoney::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();

	Invalidate();
}

void CIoViewDKMoney::OnVDataMerchKLineUpdate(IN CMerch *pMerch)
{
	
}

void CIoViewDKMoney::OnIoViewFontChanged()
{	
	CIoViewBase::OnIoViewFontChanged();
	// 字体调整 - 视图会变得不协调
	//RecalcRectSize();
	Invalidate();
}

void CIoViewDKMoney::LockRedraw()
{
	if ( m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = true;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
}

void CIoViewDKMoney::UnLockRedraw()
{
	if ( !m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = false;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
}

int CIoViewDKMoney::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	int iRet = CIoViewBase::OnCreate(lpCreateStruct);
	if ( -1 == iRet )
	{
		return -1;
	}

	//
	InitialIoViewFace(this);

	m_RespDKMoney.m_fITBF = 0.0;
	m_RespDKMoney.m_fITSF = 0.0;
	m_RespDKMoney.m_fMultSide = 0.0;
	m_RespDKMoney.m_fEmpSide = 0.0;

	SetTimer(KTimerIdPushDataReq, KTimerPeriodPushDataReq, NULL);
	//
	return iRet;
}


void CIoViewDKMoney::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);
	
	CRect rcWindows;
	GetClientRect(&rcWindows);
	
	rcWindows.InflateRect(-10, -10);	// 空隙
	m_RectTitle = rcWindows;

	CClientDC dc(this);

	CString StrTitleWidth   = CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;;
// 	CString StrStrategyTitleWidth = PAGE_STR_STRATEGY_TITLE;
// 	CString StrStarategyWidth   = PAGE_STR_STRATEGY;

	CFont *pFontNormal = GetIoViewFontObject(ESFNormal);
	CFont *pFontSmall  = GetIoViewFontObject(ESFSmall);
	CFont *pOldFont = NULL;

	// 显示文字坐标
	pOldFont = dc.SelectObject(pFontNormal);
	CSize sizeWidthNormal = dc.GetTextExtent(_T("字"));
	dc.SelectObject(pFontSmall);
	CSize sizeWidthSmall = dc.GetTextExtent(_T("字"));
	dc.SelectObject(pOldFont);

	m_RectTitle.right = m_RectTitle.left + sizeWidthNormal.cx* StrTitleWidth.GetLength() *FONT_SPACING;	// 依据字体，固定宽绘制
	m_RectTitle.bottom = m_RectTitle.top + sizeWidthNormal.cy + 2*4;

	m_RectText = rcWindows;
	m_RectText.top = m_RectText.bottom - sizeWidthNormal.cy * 2 - 4;

	// 去掉标题的高度，并且预留一个字的高度
	CRect rcPillar = rcWindows;
	rcPillar.top += sizeWidthNormal.cy + 4;
	rcPillar.bottom = m_RectText.top;

	// 计算柱子的横纵坐标, 
	int iTenContractWidth = 0;
	int iTenContractHeight = 0;

	iTenContractWidth  =  rcPillar.Width() / 10;
	iTenContractHeight = rcPillar.Height();

	m_RectRedPillar.top    = rcPillar.top;
	m_RectRedPillar.left   = rcPillar.left + iTenContractWidth * 3;
	m_RectRedPillar.right  = m_RectRedPillar.left + iTenContractWidth;
	m_RectRedPillar.bottom = rcPillar.top + iTenContractHeight;

	m_RectGreenPillar = m_RectRedPillar;
	m_RectGreenPillar.left = m_RectRedPillar.right + iTenContractWidth*2;
	m_RectGreenPillar.right = m_RectGreenPillar.left + iTenContractWidth;
}

// 通知视图改变关注的商品
void CIoViewDKMoney::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch)
		return;
	
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

		MemsetDKMoneyData();
		RequestViewData();
		Invalidate(FALSE);
	}
}

//
void CIoViewDKMoney::OnVDataForceUpdate()
{
	RequestViewData();
}

bool32	CIoViewDKMoney::IsAttendData(IN CMerch *pMerch, E_DataServiceType eDataServiceType)
{
	if (eDataServiceType & m_iDataServiceType)
	{
		return TRUE;
	}

	return false;
}

void CIoViewDKMoney::OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch)
{

}

bool32 CIoViewDKMoney::FromXml(TiXmlElement * pElement)
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

CString CIoViewDKMoney::ToXml()
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

CString CIoViewDKMoney::GetDefaultXML()
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


void CIoViewDKMoney::OnIoViewActive()
{
	SetFocus();
	m_bActive = IsActiveInFrame();

	//OnVDataForceUpdate();
	RequestLastIgnoredReqData();

	SetChildFrameTitle();
	Invalidate();

	RequestViewData();
}

void CIoViewDKMoney::OnIoViewDeactive()
{
	m_bActive = false;

	Invalidate();
}

void  CIoViewDKMoney::SetChildFrameTitle()
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

void CIoViewDKMoney::RequestViewData()
{
	if (NULL == m_pMerchXml)
		return;

	CGGTongDoc	*pDoc = AfxGetDocument();
	if ( pDoc != NULL && pDoc->m_pAbsCenterManager != NULL )
	{
		CMmiReqDKMoney	req;
		req.m_iMarketID = m_pMerchXml->m_MerchInfo.m_iMarketId;
		req.m_StrMerchCode = m_pMerchXml->m_MerchInfo.m_StrMerchCode;
		req.m_uType = ECSTMainMonitor;
		pDoc->m_pAbsCenterManager->RequestViewData(&req);
	}	
}

BOOL CIoViewDKMoney::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;
}

void CIoViewDKMoney::OnVDataPluginResp( const CMmiCommBase *pResp )
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

	const CMmiCommBasePlugIn *pRespPlugin = (CMmiCommBasePlugIn *)pResp;
	if ( pRespPlugin->m_eCommTypePlugIn == ECTPIRespDKMoney)
	{
		OnDKMoneyResp((const CMmiRespDKMoney *)pResp);
	}
}

void CIoViewDKMoney::MemsetDKMoneyData()
{
	m_RespDKMoney.m_fITBF		= 0.0;
	m_RespDKMoney.m_fITSF		= 0.0;
	m_RespDKMoney.m_fMultSide	= 0.0;
	m_RespDKMoney.m_fEmpSide	= 0.0;
}

void CIoViewDKMoney::OnDKMoneyResp( const CMmiRespDKMoney *pResp )
{
	if (!pResp)
	{
		return;
	}

	if (fabs(pResp->m_fITBF) < 1e-7 && fabs(pResp->m_fITSF) < 1e-7)
	{
		MemsetDKMoneyData();
	}
	else
	{
		m_RespDKMoney.m_fITBF		= pResp->m_fITBF;
		m_RespDKMoney.m_fITSF		= pResp->m_fITSF;
		m_RespDKMoney.m_fMultSide	= pResp->m_fMultSide;
		m_RespDKMoney.m_fEmpSide	= pResp->m_fEmpSide;
	}
	Invalidate(FALSE);
}

void CIoViewDKMoney::OnTimer( UINT nIDEvent )
{
	if ( KTimerIdPushDataReq == nIDEvent )
	{
		RequestViewData();
	}
	CIoViewBase::OnTimer(nIDEvent);
}

