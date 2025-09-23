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

#include "IoViewTrendAnalysis.h"

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


#define  FORMULAR_ANALYSISI  L"大势研判"
#define  WORK_LINE		L"工作线"

#define  TITLE_TOP		50;

#define  PAGE_STR_TITLE				 "大势研判"
// #define  PAGE_STR_STRATEGY_TITLE     "推荐策略："
// #define  PAGE_STR_STRATEGY_LONG			 "龙战于天"
// #define  PAGE_STR_STRATEGY_TIAN			 "天眼破兵"
// #define  PAGE_STR_STRATEGY_DUO			 "多空挪移"

#define  RGB_COLOR_RED		RGB(0xff,0x38,0x37)
#define  RGB_COLOR_YELLOW	RGB(0xb9,0xfc,0x00)
#define  RGB_COLOR_WHITE	RGB(0xcb,0xeb,0xfa)





const ST_SpecialMarket g_aSpecialMarket[] = 
{
	ST_SpecialMarket(0,   _T("上证指数") , _T("000001")),
	ST_SpecialMarket(1000, _T("深圳指数"), _T("399001")),
	ST_SpecialMarket(1000, _T("创业指数"), _T("399006")),
	ST_SpecialMarket(1000, _T("沪深300"),  _T("399300"))
};

 




/////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CIoViewTrendAnalysis, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewTrendAnalysis, CIoViewBase)
	//{{AFX_MSG_MAP(CIoViewTrendAnalysis)
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
CIoViewTrendAnalysis::CIoViewTrendAnalysis()
:CIoViewBase()
{
	m_bActive			= false;
	m_RectWindows = CRect(0,0,0,0);
	m_iDataServiceType  = EDSTKLine;	// General为收到除权信号
	m_pDrawImage = NULL;
// 	m_strDisStrategy = _T("");
	m_iTrendMarketId = 0;
	m_strTrendMerchCode = _T("000001");
	m_vSpecialMarket.clear();

	m_pImageXiongshi = NULL;
	m_pImageNiushi = NULL;
	m_pImageZhengDangshi = NULL;

	m_Font14.CreateFont(14, 0,0,0,FW_NORMAL, 0,0,0,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"宋体");

	m_Font12.CreateFont(12, 0,0,0,FW_NORMAL, 0,0,0,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"宋体");
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewTrendAnalysis::~CIoViewTrendAnalysis()
{
	m_Font14.DeleteObject();
	m_Font12.DeleteObject();

	DEL(m_pImageXiongshi);
	DEL(m_pImageNiushi);
	DEL(m_pImageZhengDangshi);
}


///////////////////////////////////////////////////////////////////////////////
// OnPaint

void CIoViewTrendAnalysis::OnPaint()
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
	COLORREF clrText = RGB_COLOR_WHITE;
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
	//dcMem->SetBkColor(clrBk);
	dcMem->FillSolidRect(rcClient, clrBk);

	//DrawCorner(dcMem, rcClient);


	//// 划线
	//CBrush cbr;
	//cbr.CreateStockObject(NULL_BRUSH); //创建一个空画刷
	//CBrush *pOldBr = dcMem->SelectObject(&cbr);
	//CPen cpen;
	//cpen.CreatePen(PS_SOLID, 1, RGB(200, 200, 200)); //创建一个红色的画笔
	//CPen *pOldPen = dcMem->SelectObject(&cpen);
	////CRect rcTemp(rcClient);

	////rcTemp.top  += 100;
	//dcMem->RoundRect(rcClient, CPoint(10, 10));
	//dcMem->SelectObject(pOldPen);
	//cbr.DeleteObject();
	//dcMem->SelectObject(pOldBr);
	//cbr.DeleteObject();



	CFont *pFontNormal = &m_Font14;
	CFont *pFontSmall  = &m_Font12;
	CFont *pFontOld	= NULL;
	// 无权限不绘制
	if ( !CPluginFuncRight::Instance().IsUserHasRight(CPluginFuncRight::FuncCapitalFlow, false) )
	{
		pFontOld = dcMem->SelectObject(pFontSmall);
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
//		if ( !m_RectTitle.IsRectEmpty())
//		{
//			CString StrTitle = CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;
//			pFontOld = dcMem->SelectObject(pFontNormal);
//			dcMem->SetTextColor(clrTitle);
//			CRect rcTmp(0,0,0,0);
//			//int32 iHeight = dcMem->DrawText(StrTitle, rcTmp, DT_SINGLELINE |DT_CALCRECT |DT_VCENTER |DT_LEFT);
//			//CRect rcTitle(m_RectTitle);
//			dcMem->DrawText(StrTitle, m_RectTitle, DT_SINGLELINE |DT_VCENTER |DT_LEFT);
//			//rcTitle.left += rcTmp.Width();
//
//// 			dcMem->SelectObject(pFontSmall);
//// 			CString strStrategyTitle = PAGE_STR_STRATEGY_TITLE;
//// 			dcMem->SetTextColor(clrText);
//// 			dcMem->DrawText(strStrategyTitle, m_RectStrategyTitle, DT_SINGLELINE |DT_VCENTER |DT_RIGHT);
//// 			CString strStrategy = m_strDisStrategy;
//// 			dcMem->SetTextColor(clrStrategy);
//// 			dcMem->DrawText(strStrategy, m_RectStrategy, DT_SINGLELINE |DT_VCENTER |DT_LEFT);
//			dcMem->SelectObject(pFontOld);
//		}

		if (m_pDrawImage)
		{
			Graphics GraphicImage(dcMem->GetSafeHdc());
			DrawImage(GraphicImage, m_pDrawImage, m_RectImage, 1, 0, false);
		} 
	}
	dc.BitBlt(0,0, rcClient.Width(),rcClient.Height(),&dcMem,0,0,SRCCOPY);  
	//绘图完成后的清理  
	MemBitmap.DeleteObject();  
	dcMem->RestoreDC(iSaveDC);

}


void CIoViewTrendAnalysis::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();

	Invalidate();
}



void CIoViewTrendAnalysis::OnVDataMerchKLineUpdate(IN CMerch *pMerch)
{
	if (!pMerch || !m_pMerchXml)
	{
		return;
	}

	E_KLineTypeBase    eKLineType			    = EKTBDay;
	int32 iPosFound;
	CMerchKLineNode* pKLineRequest = NULL;


	ST_SpecialMarket stSpecialMarket;
	stSpecialMarket.iMarketId = m_pMerchXml->m_MerchInfo.m_iMarketId;
	stSpecialMarket.strMercCode = m_pMerchXml->m_MerchInfo.m_StrMerchCode;
	vector<ST_SpecialMarket>::iterator iter;
	iter = std::find(m_vSpecialMarket.begin(), m_vSpecialMarket.end(), stSpecialMarket);
	if (iter == m_vSpecialMarket.end())
	{
		CGGTongDoc *pDoc = AfxGetDocument();
		CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
		if (!pAbsCenterManager)
		{
			return;
		}

		CMerch *pMerchFound = NULL;
		pAbsCenterManager->GetMerchManager().FindMerch(m_strTrendMerchCode, m_iTrendMarketId, pMerchFound);
		if (!pMerchFound)
		{	
			return;
		}
		pMerchFound->FindMerchKLineNode(eKLineType, iPosFound, pKLineRequest);
	}
	else
	{
		m_pMerchXml->FindMerchKLineNode(eKLineType, iPosFound, pKLineRequest);
	}

	// 根本找不到K线数据， 那就不需要显示了
	if (NULL != pKLineRequest && 0 != pKLineRequest->m_KLines.GetSize())	
	{
		int iIndex = pKLineRequest->m_KLines.GetSize();
		CArray<CKLine, CKLine> aKlineTemp;
		aKlineTemp.Add(pKLineRequest->m_KLines.GetAt(iIndex - 1));

		// 取大势判研的工作线
		CFormularContent* pContent = CFormulaLib::instance()->GetFomular(FORMULAR_ANALYSISI);
		if ( NULL != pContent )
		{
			float fRed		= 0.0;
			float fGreen	= 0.0;
			float fYelloew  = 0.0;
			T_IndexOutArray* pOut = formula_index(pContent, pKLineRequest->m_KLines);
			if (pOut && pOut->iIndexNum >= ANALYSISI_COUNT)
			{
				float fLast  = 0.0f;
				float fYLast = 0.0f;	
				float fMax   = 60.0f;
				CString strTemp ;
				int aiValidData[ANALYSISI_COUNT] = {0};

				for (int i = 0; i < pOut->iIndexNum; i ++ )
				{
					if (pOut->index[i].iPointNum > 0)
					{
						int32 iPointNum = pOut->index[i].iPointNum;
						int32 iShowCount = 0;
						for (int j = iPointNum - 1; j >= 0; j--)
						{


							float fY = pOut->index[i].pPoint[j];
							if (fY != FLT_MAX &&  fY > 1e-7)
							{
								aiValidData[i] = j;
								break;
							}
						}
					}
				}

				int iValidData = 0;
				int iDataIndex = 0;
				for (int i = ANALYSISI_RED; i < ANALYSISI_COUNT; ++i)
				{
					if (iValidData < aiValidData[i])
					{
						iValidData = aiValidData[i];
						iDataIndex = i;
					}
				}

				// 红色
 				if (ANALYSISI_RED == iDataIndex)
 				{
 					m_pDrawImage = m_pImageNiushi;
 					/*m_strDisStrategy =  PAGE_STR_STRATEGY_LONG;*/
 				}
 
 				if (ANALYSISI_GREEN == iDataIndex)
 				{
 					m_pDrawImage = m_pImageXiongshi;
 					/*m_strDisStrategy =  PAGE_STR_STRATEGY_TIAN;*/
 				}
 
 				if (ANALYSISI_YELLOEW == iDataIndex)
 				{
 					m_pDrawImage = m_pImageZhengDangshi;
 				/*	m_strDisStrategy =  PAGE_STR_STRATEGY_DUO;*/
 				}
 
 				if (ANALYSISI_YELLOEW_TWO == iDataIndex)
 				{
 					m_pDrawImage = m_pImageZhengDangshi;
 					/*m_strDisStrategy =  PAGE_STR_STRATEGY_DUO;*/
 				}

				// 其实没必要做刷新
				SetSize();
				Invalidate();
				UpdateWindow();
			}
		}			
	}
}



void CIoViewTrendAnalysis::OnIoViewFontChanged()
{	
	CIoViewBase::OnIoViewFontChanged();
	// 字体调整 - 视图会变得不协调
	//RecalcRectSize();
	Invalidate();
}

void CIoViewTrendAnalysis::LockRedraw()
{
	if ( m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = true;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
}

void CIoViewTrendAnalysis::UnLockRedraw()
{
	if ( !m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = false;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
}

int CIoViewTrendAnalysis::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	int iRet = CIoViewBase::OnCreate(lpCreateStruct);
	if ( -1 == iRet )
	{
		return -1;
	}

	//
	InitialIoViewFace(this);

	/*m_Meter.Create(L"",ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,CRect(100,100,100,100),this,  2222);
	m_Meter.SetValueDecimals(0);
	m_Meter.UpdateNeedle(0) ;
	m_Meter.ShowWindow(SW_SHOW);*/

	m_pImageNiushi = Image::FromFile(_T("image//TrendAnalysis//niushi.png"));
	m_pImageXiongshi = Image::FromFile(_T("image//TrendAnalysis//xiongshi.png"));
	m_pImageZhengDangshi = Image::FromFile(_T("image//TrendAnalysis//zhengdangshi.png"));


	m_vSpecialMarket.clear();
	for (int i = 0; i < sizeof(g_aSpecialMarket) / sizeof(ST_SpecialMarket); ++i)
	{
		m_vSpecialMarket.push_back(g_aSpecialMarket[i]);
	}

	SetTimer(KTimerIdPushDataReq, KTimerPeriodPushDataReq, NULL);
	//
	return iRet;
}


void CIoViewTrendAnalysis::SetSize()
{
	CRect rcWindows;
	GetClientRect(&rcWindows);

	m_RectTitle = rcWindows;
	m_RectTitle.InflateRect(-10, -10);	// 空隙



	CClientDC dc(this);

	CString StrTitleWidth   = CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;;
// 	CString StrStrategyTitleWidth = PAGE_STR_STRATEGY_TITLE;
// 	CString StrStarategyWidth   = PAGE_STR_STRATEGY_LONG;

	/*if (dc.GetTextExtent(StrTitleWidth + StrStrategyTitleWidth + StrStarategyWidth).cx >  rcWindows.Width())
	{
		m_RectTitle = NULL;
	}*/

	// 显示文字坐标
	CFont *pFontNormal = &m_Font14;
	CFont *pFontSmall  = &m_Font12;
	CFont *pOldFont = NULL;
	pOldFont = dc.SelectObject(pFontNormal);
	CSize sizeWidthNormal = dc.GetTextExtent(_T("字"));
	dc.SelectObject(pFontSmall);
	CSize sizeWidthSmall = dc.GetTextExtent(_T("字"));
	dc.SelectObject(pOldFont);


	m_RectTitle.right = m_RectTitle.left + sizeWidthNormal.cx* StrTitleWidth.GetLength() *1.2;	// 依据字体，固定宽绘制
	m_RectTitle.bottom = m_RectTitle.top + sizeWidthNormal.cy + 2*4;

// 	m_RectStrategy = m_RectTitle;
// 	m_RectStrategy.right = rcWindows.right; //
// 	m_RectStrategy.left = m_RectStrategy.right - sizeWidthSmall.cx* StrStarategyWidth.GetLength() *1.2;


	m_RectStrategyTitle = m_RectTitle;
	/*m_RectStrategyTitle.right  =  m_RectStrategy.left;*/
	/*m_RectStrategyTitle.left = m_RectStrategyTitle.right -  sizeWidthSmall.cx* StrStrategyTitleWidth.GetLength() *1.2*/;



	// 计算居中起始位置
	// 计算显示表盘出正方形，避免变形
	if (!m_pImageZhengDangshi)
	{
		return;
	}

	rcWindows.top  +=  5; 
	int iLengthDif= rcWindows.Height() - rcWindows.Width();
	if (iLengthDif > 0)
	{
		rcWindows.top     += iLengthDif/2;
		rcWindows.bottom  -= iLengthDif/2;
	}
	else if (iLengthDif < 0)
	{
		iLengthDif = abs(iLengthDif);
		rcWindows.left   += iLengthDif/2;
		rcWindows.right  -= iLengthDif/2;
	}


	int iStartWidth =  m_pImageZhengDangshi->GetWidth();
	int iStartHeight =  m_pImageZhengDangshi->GetHeight();
	if (rcWindows.Width() < iStartWidth)
	{
		iStartWidth = rcWindows.Width();
	}
	if (rcWindows.Height() < iStartHeight)
	{
		iStartHeight = rcWindows.Height();
	}

	m_RectImage.top = rcWindows.top + (rcWindows.Height() - iStartHeight - 10)/2; // 多减10个像素，避免图片贴着底框
	m_RectImage.bottom = m_RectImage.top + iStartHeight;

	m_RectImage.left = rcWindows.left + (rcWindows.Width() - iStartWidth)/2;
	m_RectImage.right = m_RectImage.left + iStartWidth;
}
void CIoViewTrendAnalysis::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);
	
	SetSize();
	//RecalcRectSize();
}


// 通知视图改变关注的商品
void CIoViewTrendAnalysis::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
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
		SmartAttendMerch.m_iDataServiceTypes = EDSTGeneral; //EDSTTimeSale | EDSTTick; // 关注的数据类型
		m_aSmartAttendMerchs.Add(SmartAttendMerch);
		
		// 
		OnVDataMerchTimeSalesUpdate(pMerch);	// 有就显示, 无则清空显示
	}
	else
	{
		// zhangbo 20090824 #待补充， 当心商品不存在时，清空当前显示数据
		//...
	}


	ST_SpecialMarket stSpecialMarket;
	stSpecialMarket.iMarketId = pMerch->m_MerchInfo.m_iMarketId;
	stSpecialMarket.strMercCode = pMerch->m_MerchInfo.m_StrMerchCode;
	vector<ST_SpecialMarket>::iterator iter;
	iter = std::find(m_vSpecialMarket.begin(), m_vSpecialMarket.end(), stSpecialMarket);
	if (iter == m_vSpecialMarket.end())
	{
		CGmtTime TimeSpecify		= m_pAbsCenterManager->GetServerTime();

		// 切换商品时，请求一次大盘上证A股数据
		CMmiReqMerchKLine info; 
		info.m_eKLineTypeBase	= EKTBDay;								// 使用原始K线周期
		info.m_iMarketId		= m_iTrendMarketId;
		info.m_StrMerchCode		= m_strTrendMerchCode;

		info.m_eReqTimeType		= ERTYFrontCount;
		info.m_TimeSpecify		= TimeSpecify;
		info.m_iFrontCount		= 300;

		//m_pAbsCenterManager->RequestViewData((CMmiCommBase*)&info);
		DoRequestViewData(info,TRUE);
	}
	//InvalidateRect(m_RectTitle);	// 标题部分需要修改，其它无
}

//
void CIoViewTrendAnalysis::OnVDataForceUpdate()
{
	RequestViewData();
}

bool32	CIoViewTrendAnalysis::IsAttendData(IN CMerch *pMerch, E_DataServiceType eDataServiceType)
{
	if (eDataServiceType & m_iDataServiceType)
	{
		return TRUE;
	}

	return false;
}


void CIoViewTrendAnalysis::OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch)
{
	int a = 10;
}


bool32 CIoViewTrendAnalysis::FromXml(TiXmlElement * pElement)
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

CString CIoViewTrendAnalysis::ToXml()
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

CString CIoViewTrendAnalysis::GetDefaultXML()
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


void CIoViewTrendAnalysis::OnIoViewActive()
{
	SetFocus();
	m_bActive = IsActiveInFrame();

	//OnVDataForceUpdate();
	RequestLastIgnoredReqData();

	SetChildFrameTitle();
	Invalidate();
}

void CIoViewTrendAnalysis::OnIoViewDeactive()
{
	m_bActive = false;

	Invalidate();
}

void  CIoViewTrendAnalysis::SetChildFrameTitle()
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

void CIoViewTrendAnalysis::RequestViewData()
{
	if (NULL == m_pMerchXml)
		return;

	// 请求大盘数据，从当前时间获取以前的300天就行了，因为只是赋值初始值。
	CGmtTime TimeSpecify		= m_pAbsCenterManager->GetServerTime();
	CMmiReqMerchKLine info; 
			//
	ST_SpecialMarket stSpecialMarket;
	stSpecialMarket.iMarketId = m_pMerchXml->m_MerchInfo.m_iMarketId;
	stSpecialMarket.strMercCode = m_pMerchXml->m_MerchInfo.m_StrMerchCode;
	vector<ST_SpecialMarket>::iterator iter;
	iter = std::find(m_vSpecialMarket.begin(), m_vSpecialMarket.end(), stSpecialMarket);
	if (iter == m_vSpecialMarket.end())
	{
		info.m_iMarketId		= m_iTrendMarketId;
		info.m_StrMerchCode		= m_strTrendMerchCode;
	}
	else
	{
		info.m_iMarketId		= m_pMerchXml->m_MerchInfo.m_iMarketId;;
		info.m_StrMerchCode		= m_pMerchXml->m_MerchInfo.m_StrMerchCode;;
	}

	info.m_eKLineTypeBase	= EKTBDay;								// 使用原始K线周期
	info.m_eReqTimeType		= ERTYFrontCount;
	info.m_TimeSpecify		= TimeSpecify;
	info.m_iFrontCount		= 300;
	//
	DoRequestViewData(info);

}

BOOL CIoViewTrendAnalysis::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;
}



void CIoViewTrendAnalysis::OnVDataPluginResp( const CMmiCommBase *pResp )
{
	if ( NULL == pResp )
	{
		return;
	}
}

void CIoViewTrendAnalysis::OnTimer( UINT nIDEvent )
{
	if ( KTimerIdPushDataReq == nIDEvent )
	{
		RequestViewData();
	}
	CIoViewBase::OnTimer(nIDEvent);
}

