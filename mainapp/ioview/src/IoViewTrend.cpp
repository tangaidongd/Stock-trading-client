#include "stdafx.h"
#include "MPIChildFrame.h"
#include "GridCellSys.h"
#include "GridCellSymbol.h"
#include "guidrawlayer.h"
#include "NewMenu.h"
#include "memdc.h"
#include "MerchManager.h"
#include "facescheme.h"
#include "IoViewKLine.h"
#include "IoViewTrend.h"
#include "IoViewManager.h"
#include "ShareFun.h"
#include "Region.h"
#include "ChartRegion.h"
#include "CFormularContent.h"
#include <float.h>
#include "GmtTime.h"
#include "ChartDrawer.h"
#include "dlgtrendinterval.h"
#include "TraceLog.h"

#include "ColorStep.h"
#include "IndexDrawer.h"
#include "PluginFuncRight.h"

#include "DlgChooseStockVar.h"
#include "DlgNotePad.h"
#include "XmlShare.h"
#include "PathFactory.h"
#include "GGTongView.h"
#include "coding.h"
#include "FontFactory.h"

#include <string>
using std::string;
#include <set>
using std::set;
#include <fstream>

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TREND_MIN_DAY_COUNT   (1)					// 最小的天数
#define TREND_MAX_DAY_COUNT	  (20)					// 最大的天数

#define TREND_BOTTOM_HEIGHT (25)					// 底部按钮区域高度
#define TREND_BTN_HEIGHT	(19)					// 底部按钮高度

#define INVALID_ID -1
#define BTN_ID_BEGIN			3000				// 底部按钮ID开始值

const int32 iIndexShift = 0;						// 指标按钮, 平行四边形偏移量
const int32 KiIndexSpace				= 2;	//	指标在控件排列中的间隔

extern const int32 KiMenuIDIntervalStatistic;

#define IDM_CHART_INDEX_BEGIN   (33388)
#define IDM_CHART_INDEX_END     (33888)

enum E_TrendSpecialCurve
{
	ETSC_Start = 0,				// 开始位置
	ETSC_DuoKongCurve,			// 多空 一条线
	ETSC_RiseFall_RiseCurve,	// 涨跌率 涨线
	ETSC_RiseFall_FallCurve,	// 涨跌率 跌线
	ETSC_RiseFall_DiffCurve,	// 涨跌率 差值线
	ETSC_VolRate,				// 量比 一条线
	ETSC_BuySell_BuyCurve,		// 买卖力道 买线
	ETSC_BuySell_SellCurve,		// 买卖力道 卖线
	ETSC_BuySell_DiffCurve,		// 买卖力道 差值线

	ETSC_Count
};


///////////////////////////////////////////////////////
// CIoViewTrend

const int32 KTimerIdTrendInit		= 10023;		// 分时图初始化timer，FromXml时不能完成一些初始化，交由这里完成
const int32 KTimerPeriodTrendInit	= 20;			// n ms后开始

const int32 KTimerIdTrendReqPlusPushData = 10024;
const int32 KTimerPeriodTrendReqPlusPushData = 60*2*1000;	// 每个若干 ms请求一次分时特殊的推送数据

const int32 KTimerIdTrendReqInfoMine = 10025;	// 每隔若干 ms请求信息地雷数据
const int32 KTimerPeriodTrendReqInfoMine = 60*1000*10;	

const int32 KTimerIdTrendPushMinuteBS = 10026;			// 请求每分钟内外盘信息推送数据
const int32 KTimerPeriodTrendPushMinuteBS = 60*1000*3;	//

const int32 KTimerIdTrendEconoData = 10027;				// 请求财经日历
const int32 KTimerPeriodTrendEconoData = 30*1000;		// 30s请求一次

const CString KStrTrendFS = _T(" 分时走势");
const CString KStrTrendJun= _T("均线");
const CString KStrTrendVol= _T("成交量");
const CString KStrTrendHold = _T("持仓量");

const LPCWSTR  KStrXMLShowRiseFallMax = L"ShowRiseFallMax";
const LPCWSTR  KStrXMLShowAvgPrice = L"ShowAvgPrice";

//////////////////////////////////////////////////////////////////////////

// xml
const char KStrXMLEleTrendMerchType[]			= "TrendMerchType";				// 商品类型元素
const char KStrXMLAttriTrendMerchType[]			= "type";						// 类型
const char KStrXMLAttriTrendBtnIndex[]			= "BtnIndex";					// 按钮
const char KStrXMLAttriTrendIndexName[]			= "IndexName";					// 指标名称
const char KStrXMLEleTrendRegions[]				= "TrendRegions";				// 分时region元素
const char KStrXMLEleTrendRegion[]				= "TrendRegion";				// 分时region元素

///////////////////////////////////////////////////////////////////////////////
const T_FuctionButton s_astTrendSubRegionButtons[] =
{
	T_FuctionButton(L"+",		L"增加副图",		REGIONTITLEADDBUTTONID,		IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE),
	T_FuctionButton(L"×",		L"删除当前副图",	REGIONTITLEDELBUTTONID,		IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE),
};

// 功能按钮个数
const int KiTrendSubRegionButtonNums = sizeof(s_astTrendSubRegionButtons)/sizeof(T_FuctionButton);
///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CIoViewTrend, CIoViewChart)

CIoViewTrend::TrendBtnArray CIoViewTrend::s_aInitTrendBtns;

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewTrend, CIoViewChart)
	//{{AFX_MSG_MAP(CIoViewTrend)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_COMMAND_RANGE(IDM_CHART_BEGIN,IDM_CHART_END,OnMenu)
	ON_COMMAND_RANGE(IDT_SD_BEGIN,IDT_SD_END,OnMenu)
	ON_COMMAND_RANGE(ID_TREND_SINGLESECTION, ID_TREND_MULTIDAY10, OnMenu)
	ON_COMMAND_RANGE(ID_KLINE_ADDCMPMERCH, ID_KLINE_REMOVECMPMERCH,OnMenu)
	ON_COMMAND_RANGE(ID_TREND_SHOWRFMAX, ID_TREND_SHOWPERCENT, OnMenu)
	ON_COMMAND_RANGE(IDM_IOVIEWKLINE_ALLINDEX_BEGIN, IDM_IOVIEWKLINE_ALLINDEX_END, OnMenu)
	ON_COMMAND_RANGE(IDM_IOVIEWKLINE_OFTENINDEX_BEGIN, IDM_IOVIEWKLINE_OFTENINDEX_END, OnMenu)
	ON_COMMAND_RANGE(ID_CHART_INTERVALSTATISTIC, ID_CHART_INTERVALSTATISTIC, OnMenu)
	ON_COMMAND_RANGE(ID_TREND_SHOWTICK,ID_TREND_SHOWTICK,OnMenu)
	ON_COMMAND_RANGE(ID_TREND_SHOWVOLCOLOR, ID_TREND_SHOWVOLCOLOR, OnMenu)
	ON_COMMAND_RANGE(ID_TREND_SHOWAVG_PRICE, ID_TREND_SHOWAVG_PRICE, OnMenu)
	ON_COMMAND(KiMenuIDIntervalStatistic, OnMenuIntervalStatistic)
	ON_MESSAGE_VOID(UM_SettingChanged, OnMySettingChange)
	ON_COMMAND_RANGE(IDM_INDEXMENU_BEGIN, IDM_INDEXMENU_END, OnTrendIndexMenu)
	ON_UPDATE_COMMAND_UI_RANGE(IDM_INDEXMENU_BEGIN, IDM_INDEXMENU_END, OnUpdateTrendIndexMenu)
	//}}AFX_MSG_MAP
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////


CIoViewTrend::CIoViewTrend()
:CIoViewChart(),CChartRegionViewParam(NULL)
{
	m_pParent			  = NULL;
	m_rectClient		  = CRect(-1,-1,-1,-1);
	m_iDataServiceType	  = EDSTKLine | EDSTPrice;
	m_StrText			  = _T("IoViewTrend");
	m_pTrendDrawer		  = NULL;
	SetParentWnd(this);
	m_pRegionViewData     = this;
	m_pRegionDrawNotify   = this;

	m_CornerTime		  = 0;
	m_iChartType		  = 0;

	m_iOpenTimeForNoData  = 0;
	m_iCloseTimeForNoData = 0;

	m_bHistoryTrend		  = false;
	m_TimeHistory		  = CGmtTime(1971,1,1,1,1,1);

	m_aYAxisDivideBkMain.RemoveAll();
	m_aYAxisDivideBkSub.RemoveAll();

	m_fMinHold = m_fMaxHold = 0.0f;
	m_pHoldNodes = NULL;

	m_iTrendDayCount = 1;
	m_bSingleSectionTrend = false;

	m_eMainCurveAxisYType = CPriceToAxisYObject::EAYT_Pecent;	// 分时图仅支持这一个

	m_bShowRiseFallMax = true;
	m_bRiseFallAxis = false;

	m_bShowAvgPrice = false;

	m_bEnableRightYRegion = true;	// 显示右Y轴

	m_pRedGreenCurve = NULL;
	m_iBtnCur = 0;
	m_pCompetePriceNodes = NULL;

	m_pNoWeightExpCurve = NULL;

	m_aKLineInterval.RemoveAll();
	m_fPriceIntervalPreClose = 0.;

	//
	m_bOwnAdvertise  = false;
	m_bIsKLineOrTend = true;
	m_bDrawYRigthPrice = false;

	m_bTracking = FALSE;
	m_bOver = FALSE;

	m_radius = 0;

	m_bSizeChange = false;
	m_iBtnHovering = INVALID_ID;
}

CIoViewTrend::~CIoViewTrend()
{
	if (m_bHistoryTrend)
	{
		CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
		pMainFrame->SetHistoryTrendNULL();
		if ( NULL != pMainFrame->GetHistoryChild())
		{
			pMainFrame->GetHistoryChild()->SetFixTitleFlag(false);
		}
	}

	// 已经在region中删除了
	//delete m_pRedGreenCurve;

	//m_pEconoHandle->Release();

	delete m_pCompetePriceNodes;	// 竞价数据删除
}
///////////////////////////////////////////////////////////////////////////////
int CIoViewTrend::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CIoViewChart::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 初始化一下显示涨停的参数
	CString StrValue;
	CString StrName = CIoViewManager::GetIoViewString(this);
	if ( CEtcXmlConfig::Instance().ReadEtcConfig(StrName, KStrXMLShowRiseFallMax, NULL, StrValue) )
	{
		m_bShowRiseFallMax = _ttoi(StrValue) != 0;
	}

	CString StrValueAvg;
	CString StrNameAgv = CIoViewManager::GetIoViewString(this);
	if ( CEtcXmlConfig::Instance().ReadEtcConfig(StrNameAgv, KStrXMLShowAvgPrice, NULL, StrValueAvg) )
	{
		m_bShowAvgPrice = _ttoi(StrValueAvg) != 0;
	}

	InitBtnIndex();	// 第一次初始化按钮的指标, 不从xml中取

	// 多股同列不需要创建下面的东西
	if (m_bMoreView)
	{
		return 0;
	}

	// 是否需要请求财经日历
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if (pDoc && pDoc->IsReqEcono())
	{
		m_dlgEconomicTip.Create(CDlgEconomicTip::IDD, this);

		// 判断是否已经启动财经日历请求
		CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
		if (pMainFrame)
		{
			if (!pMainFrame->m_bStartEconoReq)
			{
				// 请求财经日历数据
				StartReqEconoData();
				pMainFrame->m_bStartEconoReq = true;
			}
		}
	}
	
	InitMerchBtns();
	return 0;
}

void CIoViewTrend::InitCtrlFloat ( CGridCtrl* pGridCtrl )
{
	if ( m_bInitialFloat )
	{
		return;
	}
	else
	{
		m_bInitialFloat = true;
	}

	pGridCtrl->SetRowCount(14);
	CGridCellSys *pCell;

	pCell = (CGridCellSys *)pGridCtrl->GetCell(2, 0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(_T(" 价格"));

	pCell = (CGridCellSys *)pGridCtrl->GetCell(4, 0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(_T(" 均价"));

	pCell = (CGridCellSys *)pGridCtrl->GetCell(6, 0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(_T(" 涨跌"));

	pCell = (CGridCellSys *)pGridCtrl->GetCell(8, 0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(_T(" 涨幅"));

	pCell = (CGridCellSys *)pGridCtrl->GetCell(10, 0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(_T("成交量"));

	pCell = (CGridCellSys *)pGridCtrl->GetCell(12, 0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(_T(" 金额"));
	m_pCell1 = pCell;
	if ( NULL != m_pMerchXml )
	{
		if (CReportScheme::IsFuture(m_pMerchXml->m_Market.m_MarketInfo.m_eMarketReportType))
		{
			m_pCell1->SetText(_T(" 持仓量"));
		}
		else
		{
			m_pCell1->SetText(_T(" 金额"));
		}
	}

	CGridCellSymbol* pCellSymbol;
	pGridCtrl->SetCellType(0,0, RUNTIME_CLASS(CGridCellSymbol));
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(0,0);
	pCellSymbol->SetDefaultFont(ESFSmall);
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
	pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

	pGridCtrl->SetCellType(1,0, RUNTIME_CLASS(CGridCellSymbol));
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(1,0);
	pCellSymbol->SetDefaultFont(ESFSmall);
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
	pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

	pGridCtrl->SetCellType(3,0, RUNTIME_CLASS(CGridCellSymbol));
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(3,0);
	pCellSymbol->SetDefaultFont(ESFSmall);
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
	pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

	pGridCtrl->SetCellType(5,0, RUNTIME_CLASS(CGridCellSymbol));
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(5,0);
	pCellSymbol->SetDefaultFont(ESFSmall);
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
	pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

	pGridCtrl->SetCellType(7,0, RUNTIME_CLASS(CGridCellSymbol));
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(7,0);
	pCellSymbol->SetDefaultFont(ESFSmall);
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	
	pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

	pGridCtrl->SetCellType(9,0, RUNTIME_CLASS(CGridCellSymbol));
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(9,0);
	pCellSymbol->SetDefaultFont(ESFSmall);
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	
	pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

	pCell = (CGridCellSys *)pGridCtrl->GetCell(11,0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetDefaultTextColor(ESCVolume);

	pCell = (CGridCellSys *)pGridCtrl->GetCell(13, 0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetDefaultTextColor(ESCAmount);

	pGridCtrl->ShowWindow(SW_SHOW);
	SetGridFloatInitPos();
}

void CIoViewTrend::OnTimer(UINT nIDEvent) 
{
	if(!m_bShowNow)
	{
		return;
	}

	if (NULL == m_pMerchXml)
		return;

	if ( nIDEvent == KTrendHeartBeatTimerId &&
		m_MerchParamArray.GetSize() > 0 &&
		m_pAbsCenterManager != NULL)
	{
		//是否在OpenCloseTime之中
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
		CMerch* pMerch = pData->m_pMerchNode;
		if (ServerTimeInOpenCloseTime(pMerch))
		{
			OnVDataMerchKLineUpdate(pData->m_pMerchNode);
		}	
	}
	if ( nIDEvent == KTimerIdTrendInit )
	{
		T_MerchNodeUserData* pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
		if ( NULL != m_pMerchXml
			&& NULL != m_pRegionMain
			&& NULL != pData )
		{
			OnTrendBtnSelChanged();
			KillTimer(nIDEvent);
		}
	}
	if ( nIDEvent == KTimerIdTrendReqPlusPushData )
	{
		if ( IsNowInTradeTimes(m_pMerchXml, true) )
		{
			if ( !RequestTrendPlusPushData() )	// 每隔若干时间请求推送数据
			{
				KillTimer(nIDEvent); // 无请求数据就可以不要定时了
			}
		}
		else
		{
			// no Kill，因为等着等着就到请求数据的时间了
		}
	}
	if ( KTimerIdTrendReqInfoMine == nIDEvent && !m_bHistoryTrend )
	{
		// 每隔若干时间段，请求信息地雷
		T_MerchNodeUserData *pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
		if ( NULL != pData && NULL != pData->m_pMerchNode && pData->m_TrendTradingDayInfo.m_bInit )
		{
			if ( IsNowInTradeTimes(m_pMerchXml, true) )
			{
				CMmiNewsReqLandMine reqMine;
				reqMine.m_iMarketId		= pData->m_pMerchNode->m_MerchInfo.m_iMarketId;
				reqMine.m_StrMerchCode	= pData->m_pMerchNode->m_MerchInfo.m_StrMerchCode;
				CGmtTime TimeEnd = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeEnd.m_Time + CGmtTimeSpan(1,0,0,0);
				SaveDay(TimeEnd);
				reqMine.m_StrTime1.Format(_T("%04d-%02d-%02d 00:00:00"), TimeEnd.GetYear(), TimeEnd.GetMonth(), TimeEnd.GetDay());
				reqMine.m_iCount = 200;
				reqMine.m_iTimeType = 2;	// 结束往前200个
				reqMine.m_iIndexInfo = EITLandmine;

				m_pAbsCenterManager->RequestNews(&reqMine);
			}
		}
	}
	if ( KTimerIdTrendPushMinuteBS == nIDEvent )
	{
		T_MerchNodeUserData *pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
		if ( NULL != pData && IsShowVolBuySellColor() && IsNowInTradeTimes(m_pMerchXml, true) )
		{
			CMmiReqAddPushMinuteBS reqPush;
			reqPush.m_iMarketId	 = pData->m_pMerchNode->m_MerchInfo.m_iMarketId;
			reqPush.m_StrMerchCode = pData->m_pMerchNode->m_MerchInfo.m_StrMerchCode;
			DoRequestViewData(reqPush);
		}
	}

	if (KTimerIdTrendEconoData == nIDEvent)
	{
		CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
		if (pDoc && pDoc->m_pAutherManager)
		{
			std::string strURL;

			// 读取财经日历服务器地址
			pDoc->GetEconoServerURL(strURL);

			CString StrUrl = strURL.c_str();

			// 开始请求当天的财经日历数据
			SYSTEMTIME sys; 
			GetLocalTime( &sys );

			char szTime[32] = {0};
			sprintf(szTime, "%d-%d-%d", sys.wYear, sys.wMonth, sys.wDay);

			// 请求财经日历数据
			pDoc->m_pAutherManager->GetInterface()->ReqEconoData(StrUrl, szTime);
		}
	}

	CIoViewChart::OnTimer(nIDEvent);
}
void CIoViewTrend::CreateRegion()
{
	ASSERT ( NULL == m_pRegionMain );
	CIoViewChart::CreateRegion();
	UpdateGDIObject();

	///////////////////////////////////////////////////////////////////////////////////////////////
	//Trend
	new CChartRegion ( this,m_pRegionMain,_T("Trend"), CChartRegion::KChart|CChartRegion::KCrossAble|CChartRegion::KRectZoomOut);
	m_pRegionMain->SetLevel(1);
	m_pRegionMain->SetTopSkip(18);
	m_pRegionMain->SetBottomSkip(0);

	m_pRegionMain->AddDrawNotify(this,ERDNBeforeDrawCurve);
	m_pRegionMain->AddDrawNotify(this,ERDNAfterDrawCurve);
	m_pRegionMain->AddDrawNotify(this,ERDNBeforeTransformAllRegion);

	//X轴
	new CChartRegion ( this,m_pRegionXBottom, _T("x轴"), CRegion::KFixHeight|CChartRegion::KXAxis);
	m_pRegionXBottom->AddDrawNotify(this,ERDNBeforeDrawCurve);
	m_pRegionXBottom->SetDataSourceRegion(m_pRegionMain);

	//增加新区域处
	new CChartRegion ( this,m_pRegionSeparator, _T("split"), CChartRegion::KUserChart);
	m_pRegionSeparator->AddDrawNotify(this,ERDNAfterDrawCurve);

	//Y轴
	new CChartRegion ( this,m_pRegionYLeft, _T("左y轴"),CRegion::KFixWidth|CChartRegion::KYAxis|CChartRegion::KActiveFlag|CChartRegion::KYAxisSpecifyTextColor);
	m_pRegionYLeft->SetTopSkip(18);
	m_pRegionYLeft->SetBottomSkip(0);
	m_pRegionMain->AddYDependentRegion(m_pRegionYLeft);
	m_pRegionYLeft->AddDrawNotify(this,ERDNBeforeDrawCurve);
	m_pRegionYLeft->SetDataSourceRegion(m_pRegionMain);

	//Y轴
	new CChartRegion ( this,m_pRegionYRight, _T("右y轴"),CRegion::KFixWidth|CChartRegion::KYAxis|CChartRegion::KYAxisSpecifyTextColor);
	m_pRegionYRight->SetTopSkip(18);
	m_pRegionYRight->SetBottomSkip(0);
	m_pRegionMain->AddYDependentRegion(m_pRegionYRight);
	m_pRegionYRight->AddDrawNotify(this,ERDNBeforeDrawCurve);
	m_pRegionYRight->SetDataSourceRegion(m_pRegionMain);

	new CChartRegion ( this,m_pRegionLeftBottom, _T("左下角"),CChartRegion::KUserChart|CRegion::KFixHeight|CRegion::KFixWidth);
	m_pRegionLeftBottom->AddDrawNotify(this,ERDNBeforeDrawCurve);
	m_pRegionLeftBottom->AddDrawNotify(this,ERDNAfterDrawCurve);

	new CChartRegion ( this,m_pRegionRightBottom, _T("右下角"),CChartRegion::KUserChart|CRegion::KFixHeight|CRegion::KFixWidth);
	m_pRegionRightBottom->AddDrawNotify(this,ERDNBeforeDrawCurve);
	m_pRegionRightBottom->AddDrawNotify(this,ERDNAfterDrawCurve);
}

void CIoViewTrend::SplitRegion()
{
	if ( ParentIsIoView() )
	{
		m_bEnableRightYRegion = false;	// 子视图情况下禁止右Y轴
	}
	CIoViewChart::SplitRegion();
	UpdateGDIObject();
}

int32 CIoViewTrend::GetSubRegionTitleButtonNums()
{
	return KiTrendSubRegionButtonNums;
}

const T_FuctionButton* CIoViewTrend::GetSubRegionTitleButton(int32 iIndex)
{
	if( iIndex < 0 || iIndex >= GetSubRegionTitleButtonNums() )
	{
		return NULL;
	}

	return &s_astTrendSubRegionButtons[iIndex];
}
void CIoViewTrend::OnDestroy() 
{

	ReleaseMemDC();
	CIoViewChart::OnDestroy();

	// 线条由region删除
	m_pRedGreenCurve = NULL;
	m_pNoWeightExpCurve = NULL;
}

void CIoViewTrend::RequestViewData()
{
	// 	if ( !IsWindowVisible() )
	// 	{
	// 		return;
	// 	}

	for ( int32 i = 0; i < m_MerchParamArray.GetSize(); i ++ )
	{
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(i);
		if ( NULL == pData )
		{
			continue;
		}

		// 
		CMerch* pMerch = pData->m_pMerchNode;
		if ( NULL == pMerch )
		{
			return;
		}

		//先发RealtimePrice请求
		{
			CMmiReqRealtimePrice Req;
			Req.m_iMarketId			= pData->m_pMerchNode->m_MerchInfo.m_iMarketId;
			Req.m_StrMerchCode		= pData->m_pMerchNode->m_MerchInfo.m_StrMerchCode;			
			DoRequestViewData(Req);
		}

		// 请求指定时间的日K线， 以便获取当前显示走势图的昨收价， 今开价
		{
			ASSERT(pData->m_TrendTradingDayInfo.m_bInit);

			// 
			// 发昨天和今天的日k线请求
			if ( m_iTrendDayCount <= TREND_MIN_DAY_COUNT-1 )
			{
				ASSERT( 0 );
				m_iTrendDayCount = TREND_MIN_DAY_COUNT;
			}
			ASSERT( m_iTrendDayCount <= TREND_MAX_DAY_COUNT );

			// 今天
			CGmtTime TimeDay = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeClose.m_Time;
			SaveDay(TimeDay);

			CMmiReqMerchKLine info;
			info.m_eKLineTypeBase	= EKTBDay;
			info.m_iMarketId		= pData->m_pMerchNode->m_MerchInfo.m_iMarketId;
			info.m_StrMerchCode		= pData->m_pMerchNode->m_MerchInfo.m_StrMerchCode;
			info.m_eReqTimeType		= ERTYFrontCount;
			info.m_TimeStart		= TimeDay;
			info.m_iFrontCount			= 1 + m_iTrendDayCount;			// 请求n+1 !!这里会影响到历史分时！
			DoRequestViewData(info);
		}

		// 发送分钟线请求
		// 如果是多日分时，需要等到日k线回来才能确定要请求具体什么时间段的分钟k线
		{
			RequestMinuteKLineData(*pData);	// 请求多日k线
		}

		// 请求信息地雷
		if ( pData->bMainMerch
			&& pData->m_TrendTradingDayInfo.m_bInit )
		{
			CMmiNewsReqLandMine reqMine;
			reqMine.m_iMarketId		= pData->m_pMerchNode->m_MerchInfo.m_iMarketId;
			reqMine.m_StrMerchCode	= pData->m_pMerchNode->m_MerchInfo.m_StrMerchCode;
			CGmtTime TimeEnd = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeEnd.m_Time + CGmtTimeSpan(1,0,0,0);
			SaveDay(TimeEnd);
			reqMine.m_StrTime1.Format(_T("%04d-%02d-%02d 00:00:00"), TimeEnd.GetYear(), TimeEnd.GetMonth(), TimeEnd.GetDay());
			reqMine.m_iCount = 200;
			reqMine.m_iTimeType = 2;	// 结束往前200个
			reqMine.m_iIndexInfo = EITLandmine;

			m_pAbsCenterManager->RequestNews(&reqMine);
		}
	}

	// 请求额外数据
	RequestTrendPlusData();
	RequestVolBuySellData();
}

CGmtTime CIoViewTrend::GetRequestNodeTime(IN CMerch* pMerch)
{
	CGmtTimeSpan SpanDay (1,0,0,0);
	CGmtTime ServerTime = m_pAbsCenterManager->GetServerTime();

	CMarketIOCTimeInfo RecentTradingDayTime;
	if (!pMerch->m_Market.GetRecentTradingDay(ServerTime, RecentTradingDayTime, pMerch->m_MerchInfo))
	{
		//ASSERT(0);
		// zhangbo 20090626 #待补充
		//...
	}
	else
	{
		return RecentTradingDayTime.m_TimeClose.m_Time;
	}

	return CGmtTime(0);
}

//lint --e{429}
void CIoViewTrend::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch)
		return;

	E_ReportType eBefore = ERTCount;
	if (NULL != m_pMerchXml)
	{
		eBefore = GetMerchKind(m_pMerchXml);
	}

	//
	m_mapLandMine.clear();		// 地雷清空

	if (NULL != pMerch)
	{
		//////////////////////////////////////////////////////////////////////////		
		CIoViewChart::OnVDataMerchChanged(iMarketId, StrMerchCode, pMerch);

		ASSERT(NULL!=m_pRegionMain);
		if ( NULL == m_pAbsCenterManager )
		{
			return;
		}

		bool32 bInhert;
		T_MerchNodeUserData* pData = NewMerchData(pMerch, true, bInhert);

		// 
		pData->m_eTimeIntervalFull = ENTIMinute;

		// 获取该商品走势相关的数据
		CMarketIOCTimeInfo RecentTradingDay;
		if ( !m_bHistoryTrend )
		{
			if (pMerch->m_Market.GetRecentTradingDay(m_pAbsCenterManager->GetServerTime(), RecentTradingDay, pMerch->m_MerchInfo))
			{
				pData->m_TrendTradingDayInfo.Set(pMerch, RecentTradingDay);
				pData->m_TrendTradingDayInfo.RecalcPrice(*pMerch);
				pData->m_TrendTradingDayInfo.RecalcHold(*pMerch);

				// zhangbo 20090708 #待补充， 中轴更新
				//...
			}
		}
		else
		{
			// 历史分时 - 设定交易日信息(历史分时严格意义上不允许切换商品，因为每个商品的交易日信息并不相同)
			if ( pMerch->m_Market.GetSpecialTradingDayTime(m_TimeHistory, RecentTradingDay, pMerch->m_MerchInfo) )
			{
				pData->m_TrendTradingDayInfo.Set(pMerch, RecentTradingDay);
				pData->m_TrendTradingDayInfo.RecalcPrice(*pMerch);
				pData->m_TrendTradingDayInfo.RecalcHold(*pMerch);
			}
		}

		// 将主商品标志为需要news
		{
			for ( int32 i=0; i < m_aSmartAttendMerchs.GetSize() ; i++ )
			{
				if ( m_aSmartAttendMerchs[i].m_pMerch == pMerch )
				{
					m_aSmartAttendMerchs[i].m_bNeedNews = true;
				}
			}
		}

		// 
		if (m_bShowTopToolBar)
		{
			SetTopCenterTitle(pMerch->m_MerchInfo.m_StrMerchCnName);
		}
		/*else*/
		{
			m_pRegionMain->SetTitle(pMerch->m_MerchInfo.m_StrMerchCnName);
		}

		m_aTrendMultiDayIOCTimes.RemoveAll();	// 清除时间点信息

		if ( pData->m_TrendTradingDayInfo.m_bInit )
		{
			m_aTrendMultiDayIOCTimes.Add(pData->m_TrendTradingDayInfo.m_MarketIOCTime);		// 给与第一天的初始化时间
		}

		if ( NULL != m_pCell1 )
		{
			if (CReportScheme::IsFuture(pMerch->m_Market.m_MarketInfo.m_eMarketReportType))
			{
				m_pCell1->SetText(_T(" 持仓量"));
			}
			else
			{
				m_pCell1->SetText(_T(" 金额"));
			}
		}


		// 
		if ( !bInhert )
		{
			CNodeSequence* pNodes = CreateNodes();
			pNodes->SetUserData(pData);
			pData->m_pKLinesShow = pNodes;

			CChartCurve* pCurvePrice = m_pRegionMain->CreateCurve(CChartCurve::KDependentCurve|CChartCurve::KRequestCurve|CChartCurve::KTypeTrend|CChartCurve::KYTransformByClose|CChartCurve::KDonotPick);
			pCurvePrice->SetTitle(KStrTrendFS);//分时走势
			CChartDrawer* pTrendDrawer = new CChartDrawer(*this, CChartDrawer::EKDSTrendPrice);
			pCurvePrice->m_clrTitleText = GetIoViewColor(ESCGridLine);
			m_pTrendDrawer = pTrendDrawer;

			pCurvePrice->AttatchDrawer(pTrendDrawer);
			pCurvePrice->AttatchNodes(pNodes);

			CChartCurve *pCurveAvg = m_pRegionMain->CreateCurve(CChartCurve::KTypeTrend | CChartCurve::KYTransformByAvg |CChartCurve::KDonotPick);
			pCurveAvg->SetTitle(KStrTrendJun);
			pCurveAvg->m_clrTitleText  = GetIoViewColor(ESCVolume2);				
			CChartDrawer* pJunDrawer   = new CChartDrawer(*this, CChartDrawer::EKDSTrendJunXian);

			pCurveAvg->AttatchDrawer(pJunDrawer);
			pCurveAvg->AttatchNodes(pNodes);
		}

		if ( m_IndexPostAdd.id >= 0 )
		{
			m_IndexPostAdd.id = -1;
			AddIndex(m_IndexPostAdd.pRegion,m_IndexPostAdd.StrIndexName);
		}

		// 由于无法确切的肯定每个商品的交易日信息，不能决定横坐标点数，所以强制清空，同时也避免一些问题
		//SetTrendMultiDay(1, true, false);

		// 由于不是所有商品都适合半场，所以这里需要判断

		// 初始化该商品的按钮信息
		InitMerchBtns();	
		
		InitNoShowBtns();
		

		RecalcBtnRect(true);

		if ( CheckFlag(m_uiFlag,CIoViewChart::KRegionSplited) )
		{
			// 没有分割的化，调用了也没用
			// 第一次调用将会由Timer在xml完成后调用
			OnTrendBtnSelChanged(false);	// 数据不要请求的
		}

		UpdateTrendMultiDayChange();	// 更改标题

		ShowProperTrendPlusCurve();	// 显示或者隐藏分时新增的线

		//////////////////////////////////////////////////////////////////////////
		// merchklineupdate	
		OnVDataMerchKLineUpdate(pMerch);

		m_mapLandMine = pMerch->m_mapLandMine;	// 赋值初始化信息地雷

		//////////////////////////////////////////////////////////////////////////
		KillTimer(KTrendHeartBeatTimerId);
		SetTimer(KTrendHeartBeatTimerId, 1000*60, NULL);

		SetTimer(KTimerIdTrendReqPlusPushData, KTimerPeriodTrendReqPlusPushData, NULL);	// 开始推送的定时器
		if ( !m_bHistoryTrend )
		{
			SetTimer(KTimerIdTrendReqInfoMine, KTimerPeriodTrendReqInfoMine, NULL);	// 开始信息地雷定时请求
		}

		E_ReportType eNow = GetMerchKind(m_pMerchXml);
		AddOrDelVolumeRegion(eBefore, eNow);

		SetCurveTitle(pData);
		UpdateAxisSize(false);
		UpdateSelfDrawCurve();
		m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
		PostMessage(WM_PAINT);
	}
	else
	{
		// zhangbo 20090824 #待补充， 当心商品不存在时，清空当前显示数据
		//...
	}

	//2014-03-10 现货白银品种 显示广告对话框 by cym
	CGGTongApp *ggApp = (CGGTongApp *)AfxGetApp();

	if ( ggApp->m_pConfigInfo->m_bIsVip )
	{
		//设置广告框所属view
		if ( !ggApp->m_pConfigInfo->m_bAdvertiseExist )
		{
			ggApp->m_pConfigInfo->m_bAdvertiseExist = true;
			m_bOwnAdvertise = true;
			SetAdvOwner(this);
			//
			CFont* pFont = GetIoViewFontObject(ESFSmall);
			LOGFONT lf;
			pFont->GetLogFont(&lf);
			SetAdvFont(&lf);
		}

		//商品切换时，不符合就要隐藏起来
		if ( m_bOwnAdvertise )
		{
			//	if ( m_bMatchMerCode )
			//	{
			RedrawAdvertise();  // 2014-04-01 by cym 测试是后再决定是否要保留
			/*
			}
			else
			{
			HideAdvDlg(); //隐藏广告对话框
			}
			*/
		}
	}

}

// 叠加视图相关
bool32 CIoViewTrend::OnVDataAddCompareMerch(IN CMerch *pMerch)
{
	// ...fangz 0526
	/*
	if (NULL == pMerch)
	return false;

	if (m_bHistoryTrend)
	{
	return true;
	}

	CIoViewChart::OnVDataAddCompareMerch(pMerch);

	ASSERT(NULL!=m_pRegionMain);
	if ( NULL == m_pAbsCenterManager )
	{
	return false;
	}
	if ( m_MerchParamArray.GetSize() > 3 )
	{
	return false;
	}
	bool32 bInhert;
	T_MerchNodeUserData* pData = NewMerchData(pMerch,false,bInhert);
	if ( NULL == pData )
	{
	return false;
	}
	// zhangbo 20090629 #
	//	pData->m_TrendInitData.m_pTrend = this;

	CNodeSequence* pNodes = CreateNodes();
	pNodes->SetUserData(pData);
	pData->m_pKLinesShow = pNodes;

	if (NULL == pData->m_pMerchNode)
	{
	return false;
	}

	//new 1 curves/nodes/...
	CChartCurve* pCurve = m_pRegionMain->CreateCurve(CChartCurve::KRequestCurve|
	CChartCurve::KYTransformToAlignDependent|
	CChartCurve::KTypeTrend|
	CChartCurve::KYTransformByClose|
	CChartCurve::KUseNodesNameAsTitle);
	CChartDrawer* pTrendDrawer = new CChartDrawer(*this, CChartDrawer::EKDSTrendPrice);

	pCurve->AttatchDrawer(pTrendDrawer);
	pCurve->AttatchNodes(pNodes);

	RequestViewData(pData);
	SetCurveTitle(pData);

	KillTimer(103);
	SetTimer(103,5,NULL);
	*/
	return true;
}

void CIoViewTrend::SetCurveTitle ( T_MerchNodeUserData* pData )
{
	if ( NULL == pData || NULL == pData->m_pKLinesShow )
	{
		return;
	}

	CString StrTitle;
	CMerch* pMerch = pData->m_pMerchNode;
	if ( NULL == pMerch )
	{
		return;
	}
	if ( !pData->bMainMerch)
	{
		// 显示其最新价 TODO
		float fPreClose = pData->m_TrendTradingDayInfo.GetPrevReferPrice();
		if ( 0.0f == fPreClose || NULL == pMerch->m_pRealtimePrice )
		{
			StrTitle = _T("[") + pMerch->m_MerchInfo.m_StrMerchCnName + _T(" 叠加] ");
			pData->m_pKLinesShow->SetName(StrTitle);
		}
		else
		{
			float fPrice = pMerch->m_pRealtimePrice->m_fPriceNew;
			CString StrSign;
			if ( fPrice < fPreClose ) // ?
			{
				StrSign = _T("▼");
			}
			else if (fPrice > fPreClose)
			{
				StrSign = _T("▲");
			}
			CString StrFmt;
			StrFmt.Format(_T("[%s %%0.%df%s]"), pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), pMerch->m_MerchInfo.m_iSaveDec, StrSign.GetBuffer());
			StrTitle.Format(StrFmt, fPrice);
			pData->m_pKLinesShow->SetName(StrTitle);
		}
	}
	else
	{
		pData->m_pKLinesShow->SetName(pMerch->m_MerchInfo.m_StrMerchCnName);
	}
}

void CIoViewTrend::OnVDataForceUpdate()
{
	if ( NULL == m_pRegionMain )
	{
		return;
	}

	// zhangbo 20090707 #待补充， 重新计算交易日， 有可能是隔天初始化， 需要重新换天显示等
	//...
	if ( m_MerchParamArray.GetSize() > 0 )
	{		
		T_MerchNodeUserData * pData = m_MerchParamArray.GetAt(0);
		ASSERT(NULL != pData);

		CGmtTime TimeInit	= pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time;
		CGmtTime TimeEnd	= TimeInit + CGmtTimeSpan(0, 23, 59, 59);

		CGmtTime TimeServer = m_pAbsCenterManager->GetServerTime();
		if ( m_bHistoryTrend )
		{
			TimeServer = m_TimeHistory;		// 历史时间
		}

		if (TimeServer < TimeInit || TimeServer > TimeEnd)	// 不是同一天， 需要重新算时间和昨收价等
		{
			// 清除所有数据
			ClearLocalData();

			// 获取该商品走势相关的数据
			CMarketIOCTimeInfo RecentTradingDay;
			bool32 bRecentTradingDayOk = false;
			if ( !m_bHistoryTrend )
			{
				bRecentTradingDayOk = m_pMerchXml->m_Market.GetRecentTradingDay(m_pAbsCenterManager->GetServerTime(), RecentTradingDay, m_pMerchXml->m_MerchInfo);
			}
			else
			{
				bRecentTradingDayOk = m_pMerchXml->m_Market.GetSpecialTradingDayTime(m_TimeHistory, RecentTradingDay, m_pMerchXml->m_MerchInfo);
			}
			if ( bRecentTradingDayOk )
			{
				pData->m_TrendTradingDayInfo.Set(m_pMerchXml, RecentTradingDay);
				pData->m_TrendTradingDayInfo.RecalcPrice(*m_pMerchXml);
			}

			// 
			m_aTrendMultiDayIOCTimes.RemoveAll();	// 清除时间点信息
			if ( pData->m_TrendTradingDayInfo.m_bInit )
			{
				m_aTrendMultiDayIOCTimes.Add(pData->m_TrendTradingDayInfo.m_MarketIOCTime);		// 给与第一天的初始化时间
			}

			// 
			OnVDataMerchKLineUpdate(m_pMerchXml);
		}
	}

	//
	CIoViewChart::OnVDataForceUpdate();
	RequestViewData();

	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	ReDrawAysnc();
}

bool32 CIoViewTrend::ServerTimeInOpenCloseTime ( CMerch* pMerch )
{
	bool32 bFound = false;
	if ( pMerch )
	{
		CGmtTime ServerTime = m_pAbsCenterManager->GetServerTime();
		T_OpenCloseTime oct;
		int32 i,iSize = pMerch->m_Market.m_MarketInfo.m_OpenCloseTimes.GetSize();
		for ( i = 0; i < iSize; i ++ )
		{
			oct = pMerch->m_Market.m_MarketInfo.m_OpenCloseTimes.GetAt(i);
			int32 iHourOpen = oct.m_iTimeOpen/60;
			int32 iMinuteOpen = oct.m_iTimeOpen%60;
			int32 iHourClose = oct.m_iTimeClose/60;
			int32 iMinuteClose = oct.m_iTimeClose%60;
			int32 iHour = ServerTime.GetHour();
			int32 iMinute = ServerTime.GetMinute();
			if ( iHour > iHourOpen || ( iHour == iHourOpen && iMinute >= iMinuteOpen))
			{
				if ( iHour < iHourClose || ( iHour == iHourClose && iHour <= iMinuteClose))
				{
					bFound = true;
					break;
				}
			}
		}
	}
	return bFound;
}

void CIoViewTrend::OnVDataFormulaChanged ( E_FormulaUpdateType eUpdateType, CString StrName )
{
	CIoViewChart::OnVDataFormulaChanged ( eUpdateType, StrName );
	OnVDataMerchKLineUpdate (m_pMerchXml);

	// 如果是当前按钮指标，则替换按钮指标
	if ( EFUTParamUpdate == eUpdateType
		|| StrName == m_StrBtnIndexName )
	{
		InitBtnIndex();
	}
}

void CIoViewTrend::OnIoViewActive()
{ 
	CIoViewChart::OnIoViewActive();
	//
	m_bViewIsvisible = true;
	RedrawAdvertise();
}

void CIoViewTrend::OnIoViewDeactive()
{
	CIoViewChart::OnIoViewDeactive();
}

void CIoViewTrend::OnKeyHome()
{
	//
	if ( NULL == m_pRegionMain->GetDependentCurve() )
	{
		return;
	}

	// 1:算位置	
	m_pRegionMain->m_iNodeCross = 0;

	if ( !m_pRegionMain->IsActiveCross() )
	{
		// 从最右边开始
		m_pRegionMain->ActiveCross(true);
		for(int32 i = 0 ; i < m_SubRegions.GetSize(); i++)
		{
			m_SubRegions.GetAt(i).m_pSubRegionMain->ActiveCross(true);
		}
	}		

	// 2:画出来
	int32 ix;

	if ( m_pRegionMain->GetDependentCurve()->CurvePosToRegionX(m_pRegionMain->m_iNodeCross, ix))
	{
		m_pRegionMain->RegionXToClient(ix);
		m_pRegionMain->BeginOverlay(false);			
		m_pRegionMain->DrawCross(m_pRegionMain->GetViewData()->GetOverlayDC(), ix, 0, CChartRegion::EDCLeftRightKey);
		m_pRegionMain->EndOverlay();	
	}	

	OnKeyLeftRightAdjustIndex();
}

void CIoViewTrend::OnKeyEnd()
{
	if ( NULL == m_pRegionMain->GetDependentCurve() )
	{
		return;
	}

	m_pRegionMain->m_iNodeCross = m_pRegionMain->GetDependentCurve()->GetNodes()->GetNodes().GetSize() - 1;

	if ( !m_pRegionMain->IsActiveCross() )
	{
		// 从最右边开始
		m_pRegionMain->ActiveCross(true);
		for(int32 i = 0 ; i < m_SubRegions.GetSize(); i++)
		{
			m_SubRegions.GetAt(i).m_pSubRegionMain->ActiveCross(true);
		}
	}		

	// 2:画出来
	int32 ix;

	if ( m_pRegionMain->GetDependentCurve()->CurvePosToRegionX(m_pRegionMain->m_iNodeCross, ix))
	{
		m_pRegionMain->RegionXToClient(ix);
		m_pRegionMain->BeginOverlay(false);			
		m_pRegionMain->DrawCross(m_pRegionMain->GetViewData()->GetOverlayDC(), ix, 0, CChartRegion::EDCLeftRightKey);
		m_pRegionMain->EndOverlay();	
	}	

	OnKeyLeftRightAdjustIndex();
}

void CIoViewTrend::OnKeyLeft()
{
	if (NULL == m_pRegionMain->GetDependentCurve())
	{
		return;
	}
	// 左移动

	// 1:算位置
	int32 iNodeCross = m_pRegionMain->m_iNodeCross;

	if ( !m_pRegionMain->IsActiveCross() )
	{
		// 从最右边开始
		m_pRegionMain->ActiveCross(true);
		for(int32 i = 0 ; i < m_SubRegions.GetSize(); i++)
		{
			m_SubRegions.GetAt(i).m_pSubRegionMain->ActiveCross(true);
		}

		iNodeCross = m_pRegionMain->GetDependentCurve()->GetNodes()->GetNodes().GetSize() - 1;		
	}
	else
	{
		if( 0 == m_pRegionMain->m_iNodeCross || ((m_pRegionMain->GetDependentCurve()->GetNodes()->GetNodes().GetSize() - 1) < m_pRegionMain->m_iNodeCross) )
		{
			// 已经在最左边了,或者在右边的非法区域. 到最右边去
			iNodeCross = m_pRegionMain->GetDependentCurve()->GetNodes()->GetNodes().GetSize() - 1;		
		}
		else
		{
			iNodeCross -= 1;
		}			 
	}

	m_pRegionMain->m_iNodeCross = iNodeCross;

	// 2:画出来
	int32 ix;
	if ( m_pRegionMain->GetDependentCurve()->CurvePosToRegionX(iNodeCross, ix))
	{
		m_pRegionMain->RegionXToClient(ix);
		m_pRegionMain->BeginOverlay(false);			
		m_pRegionMain->DrawCross(m_pRegionMain->GetViewData()->GetOverlayDC(), ix, 0, CChartRegion::EDCLeftRightKey);
		m_pRegionMain->EndOverlay();	
	}	

	OnKeyLeftRightAdjustIndex();
}

void CIoViewTrend::OnKeyRight()
{
	if (NULL == m_pRegionMain->GetDependentCurve())
	{
		return;
	}
	// 右移动

	// 1:算位置
	int32 iNodeCross = m_pRegionMain->m_iNodeCross;

	if ( !m_pRegionMain->IsActiveCross() )
	{
		// 从最左边开始
		m_pRegionMain->ActiveCross(true);

		for(int32 i = 0 ; i < m_SubRegions.GetSize(); i++)
		{
			m_SubRegions.GetAt(i).m_pSubRegionMain->ActiveCross(true);
		}

		iNodeCross = 0;		
	}
	else
	{
		if( (m_pRegionMain->GetDependentCurve()->GetNodes()->GetNodes().GetSize() - 1) <= m_pRegionMain->m_iNodeCross )
		{ 
			// 已经在最右边了,到最左边去
			iNodeCross = 0;		
		}
		else
		{
			iNodeCross += 1;
		}			 
	}

	m_pRegionMain->m_iNodeCross = iNodeCross;

	// 2:画出来
	int32 ix;
	if ( m_pRegionMain->GetDependentCurve()->CurvePosToRegionX(iNodeCross, ix) )
	{
		m_pRegionMain->RegionXToClient(ix);
		m_pRegionMain->BeginOverlay(false);			
		m_pRegionMain->DrawCross(m_pRegionMain->GetViewData()->GetOverlayDC(), ix, 0, CChartRegion::EDCLeftRightKey);
		m_pRegionMain->EndOverlay();	
	}

	OnKeyLeftRightAdjustIndex();
}

void CIoViewTrend::OnKeySpace()
{
	CIoViewChart::OnKeySpace();

	if (m_MerchParamArray.GetSize() <= 0)
	{
		return;
	}

	T_MerchNodeUserData *pMainData = m_MerchParamArray[0];
	if (NULL == pMainData)
	{
		return;
	}

	if ( -1 != m_TimeIntevalBegin.GetTime() && -1 != m_TimeIntevalEnd.GetTime() )
	{
		// 调用区域统计对话框
		int32 iNodeBegin = CMerchKLineNode::QuickFindKLineByTime(pMainData->m_aKLinesFull, m_TimeIntevalBegin);
		int32 iNodeEnd	 = CMerchKLineNode::QuickFindKLineByTime(pMainData->m_aKLinesFull, m_TimeIntevalEnd);

		if ( iNodeBegin < 0 || iNodeEnd < 0 )
		{
			ClearIntervalTime();
			return;
		}

		if ( iNodeBegin >= iNodeEnd )
		{
			ClearIntervalTime();
			return;
		}

		// 先清空
		m_aKLineInterval.RemoveAll();

		//
		int32 iNodeBeginNow, iNodeEndNow;
		GetNodeBeginEnd(iNodeBeginNow, iNodeEndNow);

		if ( iNodeBegin < iNodeBeginNow )
		{
			iNodeBegin = iNodeBeginNow;
		}

		if ( iNodeEnd > iNodeEndNow )
		{
			iNodeEnd = iNodeEndNow;
		}

		//
		if ( iNodeBegin < 0 || iNodeBegin >= pMainData->m_aKLinesCompare.GetSize() )
		{
			ClearIntervalTime();
			return;
		}

		if ( iNodeEnd < 0 || iNodeEnd >= pMainData->m_aKLinesCompare.GetSize() )
		{
			iNodeEnd = pMainData->m_aKLinesCompare.GetSize() -1;
		}

		if ( (iNodeEnd - iNodeBegin + 1) <= 1 )
		{
			ClearIntervalTime();
			return;
		}

		// 准备好这一段的统计数据
		int32 iNodeCounts = iNodeEnd - iNodeBegin + 1;

		m_aKLineInterval.SetSize(iNodeCounts);

		CKLine* pKLineInterval = (CKLine*)m_aKLineInterval.GetData();
		CKLine* pKLineFull	   = (CKLine*)pMainData->m_aKLinesFull.GetData();

		memcpyex(pKLineInterval, pKLineFull + iNodeBegin, sizeof(CKLine) * iNodeCounts);

		if ( 0 == iNodeBegin )
		{
			m_fPriceIntervalPreClose = pKLineFull[0].m_fPriceClose;
		}
		else
		{
			m_fPriceIntervalPreClose = pKLineFull[iNodeBegin - 1].m_fPriceClose;
		}

		ReDrawAysnc();

		//
		CDlgTrendInterval Dlg;
		Dlg.SetKLineData(this, m_pMerchXml, m_fPriceIntervalPreClose, m_aKLineInterval);
		Dlg.DoModal();	
	}

	if ( -1 != m_TimeIntevalBegin.GetTime() || -1 != m_TimeIntevalEnd.GetTime() )
	{
		ReDrawAysnc();
	}
}

void CIoViewTrend::OnKeyUp()
{
	m_iDayIndex = GetTrendMultiDay();
	++m_iDayIndex;
	m_iDayIndex = m_iDayIndex>10?10:m_iDayIndex;
	SetTrendMultiDay(m_iDayIndex, true, true);
}

void CIoViewTrend::OnKeyDown()
{
	m_iDayIndex = GetTrendMultiDay();
	--m_iDayIndex;
	m_iDayIndex = m_iDayIndex<1?1:m_iDayIndex;
	SetTrendMultiDay(m_iDayIndex, true, true);
}

void CIoViewTrend::OnVDataMerchKLineUpdate(IN CMerch *pMerch)
{
	if (NULL == pMerch /*|| pMerch != m_pMerchXml*/)
		return;

	//if (!IsWindowVisible())
	//{
	//	// 0002149 xl 0705 如果在激活后，短时间内被其它GGTongView的F7最大设置为隐藏，会导致还原时由于数据未更新而清空数据的显示错误，先取消该优化
	//	AddFlag(m_uiFlag,CIoViewChart::KMerchKLineUpdate);
	//	ClearLocalData();
	// 	return;
	//}

	// 挨个商品更新所有数据
	for (int32 i = 0; i < m_MerchParamArray.GetSize(); i ++)
	{
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(i);

		if (pData->m_pMerchNode != pMerch)
			continue;

		SetCurveTitle(pData);

		if (0 == i)	// 主商品
		{
			if (UpdateMainMerchKLine(*pData))
			{
				ReDrawAysnc();	
			}
			// 计算一遍附加数据
			CalcTrendPlusSubRegionData();
		}
		else		// 叠加商品
		{
			if (UpdateSubMerchKLine(*pData))
			{
				ReDrawAysnc();
			}
		}
	}
}

void CIoViewTrend::OnVDataRealtimePriceUpdate(IN CMerch *pMerch)
{
	if (NULL == pMerch /*|| pMerch != m_pMerchXml*/)
		return;

	//if (!IsWindowVisible())
	//{
	//	// 0002149 xl 0705 如果在激活后，短时间内被其它GGTongView的F7最大设置为隐藏，会导致还原时由于数据未更新而清空数据的显示错误，先取消该优化
	//	return;
	//}

	// 挨个商品更新所有数据
	for (int32 i = 0; i < m_MerchParamArray.GetSize(); i ++)
	{
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(i);
		if (pData->m_pMerchNode != pMerch)
			continue;

		// 如果是主商品，则检查是否换天了
		//if ( pData->bMainMerch )
		//{
		//}

		// 看是否需要更新昨收价
		{
			// 
			float fPricePrevClose = pData->m_TrendTradingDayInfo.GetPrevReferPrice();
			pData->m_TrendTradingDayInfo.RecalcPrice(*pMerch);
			if (fPricePrevClose != pData->m_TrendTradingDayInfo.GetPrevReferPrice())
			{
				// zhangbo 20090708 #待补充, 更新中轴值
				ReDrawAysnc();
			}
		}

		// 如果是竞价图，且在竞价时间，主商品实时行情变化重新请求竞价数据
		E_TrendBtnType eBtn = GetCurTrendBtnType();
		if ( pData->bMainMerch
			&& NULL != pMerch->m_pRealtimePrice
			&& ETBT_CompetePrice == eBtn
			&& IsTimeInCallAuction(pMerch->m_pRealtimePrice->m_TimeCurrent.m_Time))
		{
			// 竞价时间段 请求竞价数据
			RequestTrendPlusData();
		}

		if (pData->m_TrendTradingDayInfo.m_bNeedRecalc)
		{
			pData->m_TrendTradingDayInfo.RecalcPrice(*pMerch);	
			ReDrawAysnc();
		}

		//
		SetCurveTitle(pData);

		if ( pData->bMainMerch
			&& IsSuspendedMerch() )
		{
			// 停牌商品在这里调用KlineUpdate
			OnVDataMerchKLineUpdate(pMerch);
		}
	}
}

void CIoViewTrend::OnRequestPreNodeData ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes, int32 id, int32 iNum, bool32 bSendRequest )
{

}

void CIoViewTrend::OnRequestNextNodeData ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes, int32 id, int32 iNum, bool32 bSendRequest )
{

}

void CIoViewTrend::OnNodesRelease ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes )
{
	//T_MerchNodeUserData* pData = (T_MerchNodeUserData*)pNodes->GetUserData();
	//pData->pMerch已经不被这个Curve使用了,如果没有其它Curve使用,则考虑清除数据,并且从AttendMerch中删除.
}

void CIoViewTrend::OnRegionMenu2 ( CChartRegion* pRegion, CDrawingCurve* pCurve, int32 x, int32 y )
{
	RegionMenu2 ( pRegion, pCurve, x, y );
}

void CIoViewTrend::OnRegionIndexMenu ( CChartRegion* pRegion, CDrawingCurve* pCurve, int32 x, int32 y )
{
	RegionIndexMenu ( pRegion, pCurve, x, y );
}

void CIoViewTrend::OnRegionIndexBtn(CChartRegion* pRegion, CDrawingCurve* pCurve, int32 iID )
{
	RegionIndexBtn (pRegion, pCurve, iID);
}

// 分时视图的右键菜单 zhangtao
void CIoViewTrend::OnRegionMenu ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes,CNodeData* pNodeData, int32 x, int32 y)
{
	bool bMulitTimeSection = false;// 记录是否有多个交易时段

	if ( m_bHistoryTrend )
	{
		return;	// 历史不菜单
	}
	m_FormulaNames.RemoveAll();
	if ( NULL != m_pRegionMain )
	{
		m_pRegionMain->MessageAll(KMsgInactiveCross,NULL);
	}
	if ( !IsRegionReady())
	{
		return;
	}
	if ( m_MerchParamArray.GetSize() < 1 )
	{
		return;	
	}
	T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
	if ( NULL == pData )
	{
		return;
	}
	CMerch* pMerch = pData->m_pMerchNode;
	if ( NULL == pMerch )
	{
		return;
	}

	// 判断是否是多时段交易商品
	if (pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_aOCTimes.GetSize() >= 4)
	{
		bMulitTimeSection = true;
	}

	m_pRegionPick= pRegion;
	m_pCurvePick = NULL;
	m_pNodesPick = NULL;

	int32 i, iSize, iRegion = -1;
	if ( pRegion == m_pRegionMain )
	{
		iRegion = 0;
	}
	//如果选中了其它坐标轴
	iSize = m_SubRegions.GetSize();
	for ( i = 0; i < iSize; i ++ )
	{
		if ( m_SubRegions.GetAt(i).m_pSubRegionMain == pRegion )
		{
			iRegion = 1+i;
		}
	}
	if ( iRegion < 0 )
	{
		return;
	}

	m_pCurvePick	= (CChartCurve*)pCurve;
	m_pNodesPick	= pNodes;
	m_NodePick		= CNodeData(*pNodeData);

	CPoint pt(x,y);
	ClientToScreen(&pt);

	CChartCurve *pCurveTmp = NULL;
	if ( iRegion == 2
		&& ETBT_Index == GetCurTrendBtnType()
		&& NULL != (pCurveTmp=m_SubRegions[1].m_pSubRegionMain->GetCurve(0))
		&& NULL != FindIndexParamByCurve(pCurveTmp) )	// 修改chart要用到的pick curve参数
	{
		// 必须要是一个指标
		// 点击在非成交量副图上面 && 指标按钮
		// 弹出 指标按钮菜单并处理
		m_pCurvePick = pCurveTmp;	// 修改pick的线

		CNewMenu menu;
		menu.CreatePopupMenu();
		menu.LoadToolBar(g_awToolBarIconIDs);

		// 所有指标 用法注释 调整指标参数 修改指标公式 常用指标
		CNewMenu *pAllIndexMenu = menu.AppendODPopupMenu(_T("所有分时指标"));
		if ( !LoadAllIndex(pAllIndexMenu)
			|| pAllIndexMenu->GetMenuItemCount() <= 0 )
		{
			menu.DeleteMenu((UINT)pAllIndexMenu->GetSafeHmenu(), MF_BYCOMMAND);
			pAllIndexMenu = NULL;
		}

		menu.AppendMenu(MF_STRING, IDM_CHART_INDEX_HELP,  _T("指标用法注释"));
		menu.AppendMenu(MF_STRING, IDM_CHART_INDEX_PARAM, _T("调整指标参数"));
		menu.AppendMenu(MF_STRING, IDM_CHART_INDEX,		  _T("编辑指标公式"));

		menu.AppendMenu(MF_SEPARATOR);
		int32 iOften = LoadOftenIndex(&menu);		// 要check不？
		if ( iOften <= 0 )
		{
			menu.RemoveMenu(menu.GetMenuItemCount()-1, MF_BYPOSITION);	// 删除最后一个分割条
		}

		menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL, pt.x, pt.y, AfxGetMainWnd());
		return;
	}
	CNewMenu menu;
	menu.LoadMenu(IDR_MENU_TREND);
	menu.LoadToolBar(g_awToolBarIconIDs);

	CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));

	//设置修改为自绘模式
	// 	for(int i = 0;i < pPopMenu->GetMenuItemCount();++i)
	// 	{
	// 		UINT nID=pPopMenu->GetMenuItemID(i);
	// 		pPopMenu->ModifyMenu(i,MF_BYPOSITION|MF_OWNERDRAW,nID);
	// 	}
	CMenu* pTempMenu = NULL;

	/////////////////////////////////////////////////////////////////////////////////////////

	// 插入半场 - 多日菜单
	CNewMenu *pNewMenu = NULL;
	{
		// 多天
		pNewMenu = DYNAMIC_DOWNCAST(CNewMenu, pPopMenu->GetSubMenu(_T("多日分时")));
		ASSERT( NULL != pNewMenu );
		if ( NULL != pNewMenu )
		{
			for ( UINT nMenuId = ID_TREND_MULTIDAY1 ; nMenuId <= ID_TREND_MULTIDAY10 ; nMenuId++ )
			{
				const int32 nPos = nMenuId-ID_TREND_MULTIDAY1+1;
				pNewMenu->CheckMenuItem(nMenuId, (nPos==m_iTrendDayCount ? MF_CHECKED:MF_UNCHECKED) |MF_BYCOMMAND);
			}
			if ( 1 == m_iTrendDayCount && m_bSingleSectionTrend )
			{
				// 半场走势不能算1日
				pNewMenu->CheckMenuItem(ID_TREND_MULTIDAY1, MF_UNCHECKED |MF_BYCOMMAND);
			}
		}
		pPopMenu->CheckMenuItem(ID_TREND_SINGLESECTION, (m_bSingleSectionTrend ? MF_CHECKED:MF_UNCHECKED) |MF_BYCOMMAND);
	}

	// 成交量颜色
	if ( IsShowVolBuySellColor() )
	{
		pPopMenu->CheckMenuItem(ID_TREND_SHOWVOLCOLOR, MF_BYCOMMAND|MF_CHECKED);
	}

	// 自动叠加大盘
	{
		CNewMenu *pSubMenu = DYNAMIC_DOWNCAST(CNewMenu, pPopMenu->GetSubMenu(_T("叠加品种")));
		ASSERT( pSubMenu != NULL );
		if ( NULL != pSubMenu )
		{
			pSubMenu->CheckMenuItem(ID_KLINE_AUTOADDEXPCMPMERCH, (IsAutoAddExpCmpMerch()?MF_CHECKED:MF_UNCHECKED) |MF_BYCOMMAND);
		}
	}

	// 涨跌停相关
	{
		float fRise, fFall;
		if ( CalcRiseFallMaxPrice(fRise, fFall) )
		{
			pPopMenu->CheckMenuItem(ID_TREND_SHOWRFMAX, (m_bShowRiseFallMax ? MF_CHECKED : MF_UNCHECKED) |MF_BYCOMMAND);
			pPopMenu->CheckMenuItem(ID_TREND_RFMAXAXIS, (m_bRiseFallAxis ? MF_CHECKED : MF_UNCHECKED) |MF_BYCOMMAND);
		}
		else
		{
			// 删除两个菜单
			pPopMenu->DeleteMenu(ID_TREND_SHOWRFMAX, MF_BYCOMMAND);
			pPopMenu->DeleteMenu(ID_TREND_RFMAXAXIS, MF_BYCOMMAND);
		}

		pPopMenu->CheckMenuItem(ID_TREND_SHOWPERCENT, (IsEnableRightYRegion() ? MF_CHECKED : MF_UNCHECKED) |MF_BYCOMMAND);
	}

	// 均价当前价相关
	{
		if ( NULL != GetMerchXml()  && GetMerchXml()->m_pRealtimePrice)
		{
			pPopMenu->CheckMenuItem(ID_TREND_SHOWAVG_PRICE, (m_bShowAvgPrice ? MF_CHECKED : MF_UNCHECKED) |MF_BYCOMMAND);
		}
		else
		{
			// 删除两个菜单
			pPopMenu->DeleteMenu(ID_TREND_SHOWAVG_PRICE, MF_BYCOMMAND);
		}
	}

	// 区间统计 多日下删除
	if ( 1 != m_iTrendDayCount )
	{
		pPopMenu->RemoveMenu(ID_CHART_INTERVALSTATISTIC, MF_BYCOMMAND);
	}


	//////////////////////////////////////////////////////////////////////////
	m_aUserBlockNames.RemoveAll();

	CArray<T_Block, T_Block&> aBlocks;
	CUserBlockManager::Instance()->GetBlocks(aBlocks);

	//////////////////////////////////////////////////////////////////////////
	//---wangyongxue 已经添加到自选的，“加入到自选”改为“删除自选股”
	CString StrBlockName = CUserBlockManager::Instance()->GetServerBlockName();

	if (StrBlockName.IsEmpty())
	{
		StrBlockName = _T("我的自选");
	}

	bool32 bZixuan= CUserBlockManager::Instance()->BeMerchInBlock(m_pMerchXml, StrBlockName);

	if (bZixuan)
	{
		menu.ModifyODMenu(ID_BLOCK_ADDTOFIRST,MF_BYCOMMAND|MF_STRING,ID_BLOCK_DELETEFROMFIRST,L"删除自选股");
	}

	// 插入内容:
	pTempMenu = pPopMenu->GetSubMenu(L"插入内容");
	CNewMenu * pIoViewPopMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );
	ASSERT(NULL != pIoViewPopMenu );
	AppendIoViewsMenu(pIoViewPopMenu, IsLockedSplit());
	//

	// 如果处在锁定分割状态，需要删除一些按钮
	CMPIChildFrame *pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pChildFrame && pChildFrame->IsLockedSplit() )
	{
		pChildFrame->RemoveSplitMenuItem(*pPopMenu);
	}

	// 如果不是多时段交易商品，那么删除“半场十分”菜单
	if (!bMulitTimeSection)
		pPopMenu->DeleteMenu(ID_TREND_SINGLESECTION, MF_BYCOMMAND);

	// 不是股票，就删除“分时区间统计”和“颜色区分成交量显示”两个子菜单项
	E_ReportType type = m_pMerchXml->m_Market.m_MarketInfo.m_eMarketReportType;
	if (ERTStockCn != type)
	{
		pPopMenu->DeleteMenu(ID_CHART_INTERVALSTATISTIC, MF_BYCOMMAND);
		pPopMenu->DeleteMenu(ID_TREND_SHOWVOLCOLOR, MF_BYCOMMAND);
	}


	pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, AfxGetMainWnd());
	menu.DestroyMenu();
}

void CIoViewTrend::OnMenu ( UINT nID )
{
	switch(nID)
	{
	case IDM_CHART_PROP:
		OnExport();
		break;
	case IDM_CHART_INSERT:
		//MenuAddRegion();
		break;
	case IDM_CHART_DELETE:
		//MenuDelRegion();
		break;
	case IDM_CHART_DELETE_CURVE:
		MenuDelCurve();
		break;
	case IDM_CHART_CANCEL:
		break;
	case IDM_CHART_INDEX:
		ShowIndex();
		break;
	case IDM_CHART_INDEX_PARAM:
		ShowIndexParam();
		break;
	case IDM_CHART_INDEX_HELP:
		ShowIndexHelp();
		break;
	case ID_TREND_SHOWTICK:
		{
			// 切换到分时图
			CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
			if ( NULL != pMainFrame )
			{
				pMainFrame->OnPicMenuCfm(ID_PIC_TICK);
			}
		}
		break;
	case ID_TREND_SHOWVOLCOLOR:
		{
			SetShowVolBuySellColor(!IsShowVolBuySellColor());
		}
		break;
	default:
		break;
	}
	if ( nID >= IDC_CHART_ADD2BLOCK_BEGIN && nID < IDC_CHART_ADD2BLOCK_END )
	{
		CString StrBlockName = m_aUserBlockNames.GetAt(nID-IDC_CHART_ADD2BLOCK_BEGIN);
		Add2Block(StrBlockName);
	}
	if ( nID >= IDM_IOVIEWKLINE_ALLINDEX_BEGIN && nID < IDM_IOVIEWKLINE_ALLINDEX_END )
	{
		int32 iIndex = nID - IDM_IOVIEWKLINE_ALLINDEX_BEGIN;
		if ( iIndex >= 0 && iIndex < m_aAllFormulaNames.GetSize()
			&& ETBT_Index == GetCurTrendBtnType() )
		{
			// 切换非指标副图
			CString StrIndexName = m_aAllFormulaNames[iIndex];
			AddIndexToSubRegion(StrIndexName, true);
		}
		return;
	}
	else if ( nID >= IDM_IOVIEWKLINE_OFTENINDEX_BEGIN && nID < IDM_IOVIEWKLINE_OFTENINDEX_END )
	{
		int32 iIndex = nID - IDM_IOVIEWKLINE_OFTENINDEX_BEGIN;
		if ( iIndex >= 0 && iIndex < m_FormulaNames.GetSize()
			&& ETBT_Index == GetCurTrendBtnType() )
		{
			// 切换非指标副图
			CString StrIndexName = m_FormulaNames[iIndex];
			AddIndexToSubRegion(StrIndexName, true);
		}
		return;
	}
	// 	if ( nID >= IDM_CHART_INDEX_BEGIN && nID < IDM_CHART_INDEX_END )
	// 	{
	// 		MenuAddIndex(nID-IDM_CHART_INDEX_BEGIN);
	// 	}
	if ( nID >= IDT_SD_BEGIN && nID < IDT_SD_END )
	{
		if ( nID == IDT_SD_DELETE )
		{
			RemoveAllSelfDrawCurve();
		}
		else
		{
			E_SelfDrawType eSelfDrawType = E_SelfDrawType(nID - IDT_SD_BEGIN );
			CChartRegion::SetSelfDrawType(eSelfDrawType, this);
		}
	}

	if ( nID >= ID_TREND_SINGLESECTION && nID <= ID_TREND_MULTIDAY10 )
	{
		if ( ID_TREND_SINGLESECTION == nID )
		{
			SetSingleSection(!m_bSingleSectionTrend);
		}
		else
		{
			SetSingleSection(false);
			SetTrendMultiDay(nID - ID_TREND_MULTIDAY1+1, true, true);
		}
	}

	if ( nID == ID_KLINE_ADDCMPMERCH )
	{
		// 添加叠加商品
		// 是否存在自动叠加
		if ( !SetAutoAddExpCmpMerch(false, true) )
		{
			return;	// 用户否认取消自动叠加
		}
		ASSERT( !IsAutoAddExpCmpMerch() );

		MerchArray aMerchs;
		CDlgChooseStockVar::ChooseStockVar(aMerchs, false, m_pMerchXml);
		if ( aMerchs.GetSize() > 0 )
		{
			if ( AddCmpMerch(aMerchs[0], true, false) )
			{
				RequestSingleCmpMerchViewData(aMerchs[0]);	// 请求数据
				ReDrawAysnc();
			}
		}
	}
	else if ( nID == ID_KLINE_AUTOADDEXPCMPMERCH )
	{
		SetAutoAddExpCmpMerch(!IsAutoAddExpCmpMerch(), false);
	}
	else if ( nID == ID_KLINE_REMOVECMPMERCH )
	{
		// 删除所有叠加商品
		SetAutoAddExpCmpMerch(false, false);
		RemoveAllCmpMerch();
	}

	if ( ID_TREND_SHOWRFMAX == nID )
	{
		// 显示涨跌停价格
		m_bShowRiseFallMax = !m_bShowRiseFallMax;
		{
			CString StrValue;
			StrValue.Format(_T("%d"), m_bShowRiseFallMax);
			CString StrName = CIoViewManager::GetIoViewString(this);
			CEtcXmlConfig::Instance().WriteEtcConfig(StrName, KStrXMLShowRiseFallMax, StrValue);
		}
		if ( NULL != m_pRegionMain )
		{
			m_pRegionMain->SetDrawFlag(CChartRegion::KDrawFull);
			Invalidate();
		}
	}
	if ( ID_TREND_SHOWAVG_PRICE == nID )
	{
		// 显示涨跌停价格
		m_bShowAvgPrice = !m_bShowAvgPrice;
		{
			CString StrValue;
			StrValue.Format(_T("%d"), m_bShowAvgPrice);
			CString StrName = CIoViewManager::GetIoViewString(this);
			CEtcXmlConfig::Instance().WriteEtcConfig(StrName, KStrXMLShowAvgPrice, StrValue);
		}
		if ( NULL != m_pRegionMain )
		{
			m_pRegionMain->SetDrawFlag(CChartRegion::KDrawFull);
			Invalidate();
		}
	}
	else if ( ID_TREND_RFMAXAXIS == nID )
	{
		// 涨跌停坐标显示
		m_bRiseFallAxis = !m_bRiseFallAxis;
		if ( NULL != m_pRegionMain )
		{
			m_pRegionMain->SetDrawFlag(CChartRegion::KDrawFull);
			Invalidate();
		}
	}
	else if ( ID_TREND_SHOWPERCENT == nID )
	{
		// 显示百分比坐标
		EnableRightYRegion( !IsEnableRightYRegion() );
		if ( NULL != m_pRegionMain )
		{
			m_pRegionMain->SetDrawFlag(CChartRegion::KDrawFull);
			Invalidate();
		}
	}
	else if ( ID_CHART_INTERVALSTATISTIC == nID )
	{
		// 菜单区间统计
		// 先清空
		m_aKLineInterval.RemoveAll();

		if ( 1 == m_iTrendDayCount )
		{
			// 仅支持当日 && 半场
			T_MerchNodeUserData *pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
			if ( NULL != pData )
			{
				int32 iNodeBeginNow, iNodeEndNow;
				GetNodeBeginEnd(iNodeBeginNow, iNodeEndNow);
				if ( iNodeBeginNow < iNodeEndNow && pData->m_aKLinesFull.GetSize() > 0 )
				{
					// 准备好这一段的统计数据
					int32 iNodeCounts = iNodeEndNow - iNodeBeginNow;

					m_aKLineInterval.SetSize(iNodeCounts);

					CKLine* pKLineInterval = (CKLine*)m_aKLineInterval.GetData();
					CKLine* pKLineFull	   = (CKLine*)pData->m_aKLinesFull.GetData();

					memcpyex(pKLineInterval, pKLineFull + iNodeBeginNow, sizeof(CKLine) * iNodeCounts);

					if ( 0 == iNodeBeginNow )
					{
						m_fPriceIntervalPreClose = pKLineFull[0].m_fPriceClose;
					}
					else
					{
						m_fPriceIntervalPreClose = pKLineFull[iNodeBeginNow - 1].m_fPriceClose;
					}
				}
				//
				CDlgTrendInterval Dlg;
				Dlg.SetKLineData(this, m_pMerchXml, m_fPriceIntervalPreClose, m_aKLineInterval);
				Dlg.DoModal();	
			}
		}
	}
}
// 
// LRESULT CIoViewTrend::OnMessageTitleButton(WPARAM wParam,LPARAM lParam)
// {
// 	UINT uID =(UINT)wParam;
// 
// 	if ( 0 == m_MerchParamArray.GetSize() )
// 	{
// 		return 0 ;
// 	}
// 	
// 	if ( NULL == m_pRegionPick)
// 	{
// 		return 0;
// 	}
// 
// 	if ( 0 == uID)
// 	{
// 		// 指标:
// 		// 指标菜单
// /*		int32 iIndexNums = 0;
// 
// 		m_pRegionPick = m_pRegionMain;	
// 		m_FormulaNames.RemoveAll();
// 		CNewMenu menu;
// 		menu.LoadMenu(IDR_MENU_TREND);
// 		CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(0));
// 		
// 		CMenu* pTempMenu = pPopMenu->GetSubMenu(_T("常用指标"));
// 		ASSERT( NULL != pTempMenu);
// 		CNewMenu* pSubMenu = DYNAMIC_DOWNCAST(CNewMenu,pTempMenu);
// 		
// 		pSubMenu->RemoveMenu(0,MF_BYPOSITION|MF_SEPARATOR);//删除尾巴
// 		
// 		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
// 		
// 		if (NULL == pData)
// 		{
// 			return 0;				
// 		}
// 		
// 		uint32 iFlag = CFormularContent::KAllowTrend;
// 		AddFlag ( iFlag, CFormularContent::KAllowMain );
// 		
// 		CFormulaLib::instance()->GetAllowNames(iFlag,m_FormulaNames);
// 		CStringArray Formulas;
// 		FindRegionFormula(m_pRegionPick,Formulas);
// 		DeleteSameString ( m_FormulaNames,Formulas);
// 		Formulas.RemoveAll();
// 
// 		int32 iSeperatorIndex = 0;	
// 		SortIndexByVol(m_FormulaNames,iSeperatorIndex);
// 
// 		for (int32 i = 0; i < m_FormulaNames.GetSize(); i ++ )
// 		{
// 			if ( i >=(IDM_CHART_INDEX_END-IDM_CHART_INDEX_BEGIN) ) break;
// 
// 			if ( -1 != iSeperatorIndex && i == iSeperatorIndex && 0 != iSeperatorIndex)
// 			{
// 				pSubMenu->AppendODMenu(L"",MF_SEPARATOR);
// 			}
// 			iIndexNums += 1;
// 			pSubMenu->AppendODMenu(m_FormulaNames.GetAt(i),MF_STRING,IDM_CHART_INDEX_BEGIN+i);
// 		}
// 		
// 		CPoint pt;
// 		GetCursorPos(&pt);
// 		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, this);
// 		menu.DestroyMenu();
// 
// 		if ( 0 == iIndexNums )
// 		{
// 			AfxMessageBox(L"当前无可用指标,请在指标公式中设置!");
// 		}
// */
// 	}
// 	else if ( REGIONTITLELEFTBUTTONID == uID)
// 	{
// 		// 前一个指标
// 		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
// 		
// 		if (NULL == pData)
// 		{
// 			return 0;				
// 		}
// 
// 		uint32 iFlag = NodeTimeIntervalToFlag ( pData->m_eTimeIntervalFull );
// 		AddFlag ( iFlag, CFormularContent::KAllowSub );
// 		
// 		CFormulaLib::instance()->GetAllowNames(iFlag,m_FormulaNames);
// 		CStringArray Formulas;
// 		FindRegionFormula(m_pRegionPick,Formulas);
// 		
// 		int32 iIndex = 0;
// 		if ( 0 == Formulas.GetSize())
// 		{
// 			// 第一个:
// 			iIndex = 0; // 前一个的ID号
// 		}
// 		else
// 		{
// 			CString StrIndexNow = Formulas.GetAt(0);
// 
// 			for ( int32 i = 0 ; i < m_FormulaNames.GetSize(); i++)
// 			{
// 				if ( StrIndexNow == m_FormulaNames.GetAt(i))
// 				{
// 					if ( 0 == i )
// 					{
// 						return 0;
// 					}
// 					else
// 					{
// 						iIndex = i-1; // 前一个的ID号
// 					}
// 				}
// 			}
// 		}
// 		
// 		MenuAddIndex(iIndex);
// 		
// 	}
// 	else if ( REGIONTITLEFOMULARBUTTONID == uID)
// 	{
// 		// 子Region 的指标按钮
// /*		if ( NULL == m_pRegionPick )
// 		{
// 			return 0;
// 		}
// 
// 		m_FormulaNames.RemoveAll();
// 		CNewMenu menu;
// 		menu.LoadMenu(IDR_MENU_TREND);
// 		CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(0));
// 		
// 		CMenu* pTempMenu = pPopMenu->GetSubMenu(_T("常用指标"));
// 		ASSERT( NULL != pTempMenu);
// 		CNewMenu* pSubMenu = DYNAMIC_DOWNCAST(CNewMenu,pTempMenu);
// 		
// 		pSubMenu->RemoveMenu(0,MF_BYPOSITION|MF_SEPARATOR);//删除尾巴
// 		
// 		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
// 		
// 		if (NULL == pData)
// 		{
// 			return 0;				
// 		}
// 		
// 		uint32 iFlag = NodeTimeIntervalToFlag ( pData->m_eTimeIntervalFull );
// 		AddFlag ( iFlag, CFormularContent::KAllowSub );
// 		
// 		CFormulaLib::instance()->GetAllowNames(iFlag,m_FormulaNames);
// 		CStringArray Formulas;
// 		FindRegionFormula(m_pRegionPick,Formulas);
// 		DeleteSameString ( m_FormulaNames,Formulas);
// 		Formulas.RemoveAll();
// 
// 		int32 iSeperatorIndex = 0;	
// 		SortIndexByVol(m_FormulaNames,iSeperatorIndex);
// 
// 		for (int32 i = 0; i < m_FormulaNames.GetSize(); i ++ )
// 		{
// 			if ( i >=(IDM_CHART_INDEX_END-IDM_CHART_INDEX_BEGIN) ) break;
// 			
// 			if ( -1 != iSeperatorIndex && i == iSeperatorIndex && 0 != iSeperatorIndex)
// 			{
// 				pSubMenu->AppendODMenu(L"",MF_SEPARATOR);
// 			}
// 
// 			CString StrName = m_FormulaNames.GetAt(i);
// 			pSubMenu->AppendODMenu(m_FormulaNames.GetAt(i),MF_STRING,IDM_CHART_INDEX_BEGIN+i);
// 		}
// 		
// 		CPoint pt;
// 		GetCursorPos(&pt);
// 		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, this);
// 		menu.DestroyMenu();
// */
// 	}
// 	else if ( REGIONTITLERIGHTBUTTONID == uID)
// 	{
// 		// 后一个指标
// 
// 		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
// 		
// 		if (NULL == pData)
// 		{
// 			return 0;				
// 		}
// 		
// 		uint32 iFlag = NodeTimeIntervalToFlag ( pData->m_eTimeIntervalFull );
// 		AddFlag ( iFlag, CFormularContent::KAllowSub );
// 		
// 		CFormulaLib::instance()->GetAllowNames(iFlag,m_FormulaNames);
// 		CStringArray Formulas;
// 		FindRegionFormula(m_pRegionPick,Formulas);
// 		
// 		int32 iIndex = 0;
// 		if ( 0 == Formulas.GetSize())
// 		{
// 			// 第一个:
// 			iIndex = 0; // 前一个的ID号
// 		}
// 		else
// 		{
// 			CString StrIndexNow = Formulas.GetAt(0);
// 			
// 			for ( int32 i = 0 ; i < m_FormulaNames.GetSize(); i++)
// 			{
// 				if ( StrIndexNow == m_FormulaNames.GetAt(i))
// 				{
// 					if ( m_FormulaNames.GetSize() -1 == i )
// 					{
// 						return 0;
// 					}
// 					else
// 					{
// 						iIndex = i+1; // 前一个的ID号
// 					}
// 				}
// 			}
// 		}
// 		
// 		MenuAddIndex(iIndex);
// 	}
// 	else if ( REGIONTITLEADDBUTTONID == uID)
// 	{
// 		// 增加副图
// 		MenuAddRegion();
// 	}
// 	else if ( REGIONTITLEDELBUTTONID == uID)
// 	{	
// 		// 关闭子Region
// 		if ( NULL != m_pRegionPick )
// 		{
// 			MenuDelRegion();
// 		}		
// 	}
// 	else
// 	{
// 		NULL;
// 	}
// 
// 	return 0;
// }

void CIoViewTrend::OnPickNode ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes, CNodeData* pNodeData,int32 x,int32 y,int32 iFlag )
{
	if ( NULL != pNodeData )
	{
		CString StrText;
		CString StrTitle;
		T_MerchNodeUserData* pData = (T_MerchNodeUserData*)pNodes->GetUserData();
		ASSERT(NULL!=pData);
		CString StrTime = Time2String ( CGmtTime(pNodeData->m_iID), pData->m_eTimeIntervalFull);
		int32 iSaveDec = 0;
		CMerch* pMerch = pData->m_pMerchNode;
		if ( NULL != pMerch )
		{
			iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;
		}
		StrTitle = pNodes->GetName();
		if ( CheckFlag(pCurve->m_iFlag,CChartCurve::KTypeTrend) )
		{
			if ( pCurve->GetTitle() == KStrTrendFS )
			{
				StrText.Format(_T("时: %s \n量: %s\n"),
					StrTime.GetBuffer(),
					Float2String(pNodeData->m_fVolume,iSaveDec,true).GetBuffer());
			}
			else if ( pCurve->GetTitle() == KStrTrendJun )
			{
				StrText.Format(_T("时: %s \n收: %s\n"),
					StrTime.GetBuffer(),
					Float2String(pNodeData->m_fClose,iSaveDec,true).GetBuffer());
			}
			else if ( pCurve->GetTitle() == KStrTrendVol )
			{
				StrText.Format(_T("时: %s \n量: %s\n"),
					StrTime.GetBuffer(),
					Float2String(pNodeData->m_fVolume,iSaveDec,true).GetBuffer());
			}
			else
			{
				StrText.Format(_T("时: %s \n量: %s\n"),
					StrTime.GetBuffer(),
					Float2String(pNodeData->m_fVolume,iSaveDec,true).GetBuffer());
				StrTitle = pCurve->GetTitle();
			}
		}
		else if ( CheckFlag(pCurve->m_iFlag,CChartCurve::KTypeIndex) )
		{
			StrText.Format(_T("时: %s\n%s: %s\n"),
				StrTime.GetBuffer(),
				pNodes->GetName().GetBuffer(),
				Float2String(pNodeData->m_fClose,iSaveDec,true).GetBuffer());
		}
		m_TipWnd.Show(m_PointMouse, StrText, StrTitle);
	}
	else
	{
		m_TipWnd.Hide();
	}
}

void CIoViewTrend::SetXmlSource ()
{
	m_iIntervalXml = -1;
	m_iDrawTypeXml = -1;
	m_iNodeNumXml = 0;
}

void CIoViewTrend::MenuAddRegion()
{
	AddSubRegion(false);
}

void CIoViewTrend::MenuAddIndex ( int32 id)
{
	// 菜单添加指标的时候,先删除已有的指标,这里应该是替换而不是叠加	
	for (int32 i = 0 ; i < m_pRegionPick->m_Curves.GetSize(); i++)
	{		
		CChartCurve * pCurve = m_pRegionPick->m_Curves.GetAt(i);		
		T_IndexParam* pParam = FindIndexParamByCurve(pCurve);

		if (pParam)
		{
			CArray<CChartCurve*,CChartCurve*> Curves;
			FindCurvesByIndexParam(pParam,Curves);	
			while ( Curves.GetSize() > 0 )
			{
				pCurve = Curves.GetAt(0);
				Curves.RemoveAt(0);
				pCurve->GetChartRegion().RemoveCurve(pCurve);
			}
			DeleteIndexParamData ( pParam );			
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	CString StrName = m_FormulaNames.GetAt(id);
	m_FormulaNames.RemoveAll();

	T_IndexParam* pIndex = AddIndex(m_pRegionPick,StrName);
	if ( NULL == pIndex )
	{
		return;
	}
	T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
	int32 iNodeBegin,iNodeEnd;
	GetNodeBeginEnd(iNodeBegin,iNodeEnd);
	g_formula_compute ( this, this, m_pRegionPick, pData, pIndex, iNodeBegin,iNodeEnd );
	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	UpdateSelfDrawCurve();
	Invalidate();
}

void CIoViewTrend::MenuDelRegion()
{
	if ( NULL == m_pRegionPick )
	{
		return;
	}
	DeleteRegionCurves ( m_pRegionPick);
	DelSubRegion(m_pRegionPick);
	if ( m_IndexPostAdd.id >= 0 )
	{
		if  ( m_IndexPostAdd.pRegion == m_pRegionPick )
		{
			m_IndexPostAdd.id = -1;
		}
	}
	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	Invalidate();
}

void CIoViewTrend::MenuDelCurve()
{
	if ( NULL == m_pCurvePick || NULL == m_pRegionPick ) return;
	DeleteIndexCurve ( m_pCurvePick );
	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	UpdateSelfDrawCurve();
	Invalidate();
}

void CIoViewTrend::OnDropCurve ( CChartRegion* pSrcRegion,CChartCurve* pSrcCurve,CChartRegion* pDestChart)
{
	if ( NULL == pDestChart )
	{
		DeleteIndexCurve(pSrcCurve);
	}
	else
	{
		DragDropCurve( pSrcCurve,pDestChart);
	}
	UpdateSelfDrawCurve();
	OnVDataMerchKLineUpdate(m_pMerchXml);
}

void CIoViewTrend::OnRegionCurvesNumChanged (CChartRegion* pRegion, int32 iNum )
{
}

void CIoViewTrend::OnCrossData ( CChartRegion* pRegion,int32 iPos,CNodeData& NodeData, float fPricePrevClose, float fYValue, bool32 bShow)
{
	if ( m_MerchParamArray.GetSize() < 1 ) return;

	if ( bShow )
	{
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
		int32 iSaveDec = 0;
		CMerch* pMerch = pData->m_pMerchNode;
		if ( NULL != pMerch )
		{
			iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;
		}

		CKLine KLine;
		NodeData2KLine(NodeData,KLine);

		CString StrTimeLine1,StrTimeLine2;
		Time2String(KLine.m_TimeCurrent,ENTIMinute,StrTimeLine1,StrTimeLine2);


		// 		if ( CheckFlag(NodeData.m_iFlag,CNodeData::KValueInvalid))
		// 		{
		// 			NodeData.ClearValue(false,false);
		// 			NodeData2KLine(NodeData,KLine);
		// 			SetFloatData(&m_GridCtrlFloat,KLine,0,0,StrTimeLine1,StrTimeLine2,iSaveDec);
		// 			return;
		//		}
		CChartCurve *pCurveDependent = m_pRegionMain->GetDependentCurve();
		if (NULL == pCurveDependent)
			return;

		CNodeSequence* pMainNodes = pCurveDependent->GetNodes();
		CNodeData MainData;
		pMainNodes->GetAt(iPos,MainData);
		if ( iPos >= 0 && !CheckFlag(MainData.m_iFlag,CNodeData::KValueInvalid))
		{
			//			fYValue = pTrendDrawer->m_Closes.GetAt(iPos);
		}
		else
		{
			fYValue = 0.0;
		}
		if ( m_pRegionMain == pRegion )
		{
			//SetFloatData(&m_GridCtrlFloat,KLine,pData->m_TrendTradingDayInfo.GetPrevReferPrice(),fYValue,StrTimeLine1,StrTimeLine2,iSaveDec);
			SetFloatData(&m_GridCtrlFloat,KLine, GetTrendPrevClose(),fYValue,StrTimeLine1,StrTimeLine2,iSaveDec);
		}
	}
	else
	{
		m_GridCtrlFloat.ShowWindow(SW_HIDE);
		CRect rct(0,0,0,0);
		EnableClipDiff(false,rct);
	}

}

void CIoViewTrend::OnCrossNoData(CString StrTime,bool32 bShow)
{
	CGridCellSymbol * pCellSymbol = (CGridCellSymbol *)m_GridCtrlFloat.GetCell(0,1);
	if ( NULL == pCellSymbol)
	{
		return;
	}
	pCellSymbol->SetText(StrTime + _T(" "));

	for (int32 i = 1 ; i < m_GridCtrlFloat.GetRowCount() ; i++)
	{
		CGridCellSymbol * pTmpCellSymbol = (CGridCellSymbol *)m_GridCtrlFloat.GetCell(i,1);
		if ( NULL == pTmpCellSymbol)
		{
			return;
		}
		pTmpCellSymbol->SetText(_T(" "));
	}

	if (bShow)
	{
		if ( m_GridCtrlFloat.IsWindowVisible() )
		{
			m_GridCtrlFloat.ShowWindow(SW_SHOW|SW_SHOWNOACTIVATE);
			CRect Rect;
			m_GridCtrlFloat.GetClientRect(&Rect);
			OffsetRect(&Rect,m_PtGridCtrl.x,m_PtGridCtrl.y);
			EnableClipDiff(true,Rect);
			m_GridCtrlFloat.Refresh();	
		}
		m_GridCtrlFloat.ShowWindow(SW_SHOW|SW_SHOWNOACTIVATE);		
	}
	else
	{
		m_GridCtrlFloat.ShowWindow(SW_HIDE);
		CRect rct(0,0,0,0);
		EnableClipDiff(false,rct);
	}
}

void CIoViewTrend::SetFloatData ( CGridCtrl* pGridCtrl,float fCursorValue,int32 iSaveDec)
{
	if ( !m_bInitialFloat )
	{
		InitCtrlFloat(&m_GridCtrlFloat);
	}

	CString StrValue;
	CGridCellSymbol* pCellSymbol;
	//均价
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(3,0);
	StrValue = Float2SymbolString(fCursorValue, fCursorValue, iSaveDec) + _T(" ");
	pCellSymbol->SetText(StrValue);
	pGridCtrl->RedrawWindow();
}

void CIoViewTrend::SetFloatData ( CGridCtrl* pGridCtrl,CKLine& KLine,float fPricePrevClose,float fCursorValue,CString StrTimeLine1,CString StrTimeLine2,int32 iSaveDec)
{
	if ( !m_bInitialFloat )
	{
		InitCtrlFloat(&m_GridCtrlFloat);
	}

	bool32 bFutures = false;
	T_MerchNodeUserData* pDate = m_MerchParamArray.GetAt(0);
	if ( NULL != pDate)
	{
		CMerch* pMerch = pDate->m_pMerchNode;
		if ( NULL != pMerch)
		{
			if (CReportScheme::IsFuture(pMerch->m_Market.m_MarketInfo.m_eMarketReportType))
			{
				bFutures = true; 
			}
			else
			{
				bFutures = false;
			}
		}
	}

	// 
	CString StrValue;
	CGridCellSymbol* pCellSymbol;

	// xl 0001762 清除所有的空格显示，限制数字显示长度，详情窗口的宽度比YLeft能多显示一个数字 - -
	int32 iYLeftShowChar = GetYLeftShowCharWidth();
	iYLeftShowChar += 1;

	//时间
	CGridCellSys* pCellSys = (CGridCellSys*)pGridCtrl->GetCell(0,0);
	pCellSys->SetText(StrTimeLine1 + _T(""));

	pCellSys = (CGridCellSys*)pGridCtrl->GetCell(1,0);
	pCellSys->SetText(StrTimeLine2 + _T(""));

	//价位
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(3,0);
	// StrValue = Float2String(KLine.m_fPriceClose, iSaveDec, true) + _T(" ");
	// pCellSymbol->SetText(StrValue);

	//StrValue = Float2SymbolString(KLine.m_fPriceClose,fPricePrevClose,iSaveDec) + _T(" ");
	Float2SymbolStringLimitMaxLength(StrValue, KLine.m_fPriceClose, fPricePrevClose, iSaveDec, iYLeftShowChar, true, false, true, false, true);
	pCellSymbol->SetText(StrValue);

	// 判断是否是合法值

	// 第二条是均线
	CChartCurve* pCurve = m_pRegionMain->GetCurve(1);

	int32 pos = m_pRegionMain->m_iNodeCross;

	CNodeSequence * pNodes = pCurve->GetNodes();
	CNodeData NodeData;
	pNodes->GetAt(pos,NodeData);
	bool32 bValidNode = true;

	if (CheckFlag(NodeData.m_iFlag,CNodeData::KValueInvalid))
	{
		bValidNode = false;
	}

	// 均价
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(5, 0);
	//StrValue = Float2SymbolString(KLine.m_fPriceAvg, fPricePrevClose, iSaveDec) + _T(" ");
	Float2SymbolStringLimitMaxLength(StrValue, KLine.m_fPriceAvg, fPricePrevClose, iSaveDec, iYLeftShowChar, true, false, true, false, true);
	pCellSymbol->SetText(StrValue);

	//涨跌=最新-昨收
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(7, 0);

	StrValue = L" -";
	if (0. != KLine.m_fPriceClose)
	{
		//StrValue = Float2SymbolString(KLine.m_fPriceClose - fPricePrevClose, 0.0f, iSaveDec) + _T("");
		Float2SymbolStringLimitMaxLength(StrValue, KLine.m_fPriceClose - fPricePrevClose, 0.0f, iSaveDec, iYLeftShowChar, true, false, false);
	}
	pCellSymbol->SetText(StrValue);

	//涨跌幅(最新-昨收)/昨收*100%
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(9, 0);

	StrValue = L" -";
	if (0. != KLine.m_fPriceClose && 0. != fPricePrevClose)
	{
		float fRisePercent = ((KLine.m_fPriceClose - fPricePrevClose) / fPricePrevClose) * 100.;
		StrValue = Float2SymbolString(fRisePercent, 0, 2, false, false, true) + _T("");
	}	
	pCellSymbol->SetText(StrValue);		

	//成交量
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(11,0);	
	//StrValue = Float2String(KLine.m_fVolume, 0, true) + _T(" ");
	Float2StringLimitMaxLength(StrValue, KLine.m_fVolume, 0, GetYLeftShowCharWidth(1), true, true);
	pCellSymbol->SetText(StrValue);	

	// 成交额/持仓
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(13,0);
	if ( bFutures)
	{
		pCellSymbol->SetDefaultTextColor(ESCVolume);
		//StrValue = Float2String(KLine.m_fHold, 0, true) + _T(" ");
		Float2StringLimitMaxLength(StrValue, KLine.m_fHold, 0, GetYLeftShowCharWidth(1), true, true);
	}
	else
	{
		pCellSymbol->SetDefaultTextColor(ESCAmount);
		//StrValue = Float2String(KLine.m_fAmount, 0, true) + _T(" ");
		Float2StringLimitMaxLength(StrValue, KLine.m_fAmount, 0, GetYLeftShowCharWidth(1), true, true);
	}

	pCellSymbol->SetText(StrValue);	

	if ( !pGridCtrl->IsWindowVisible() )
	{
		pGridCtrl->ShowWindow(SW_SHOW|SW_SHOWNOACTIVATE);
		CRect Rect;
		pGridCtrl->GetClientRect(&Rect);
		OffsetRect(&Rect,m_PtGridCtrl.x,m_PtGridCtrl.y);
		EnableClipDiff(true,Rect);
	}

	pGridCtrl->RedrawWindow();
}

void CIoViewTrend::ClipGridCtrlFloat (CRect& Rect)
{
	EnableClipDiff(true,Rect);
}

CString CIoViewTrend::OnTime2String ( CGmtTime& Time )
{
	return Time2String(Time,ENTIMinute,false);
}

CString CIoViewTrend::OnFloat2String ( float fValue, bool32 bZeroAsHLine, bool32 bNeedTerminate/* = false*/ )
{
	return BaseFloat2String(fValue, bZeroAsHLine, bNeedTerminate);
}

void CIoViewTrend::OnCalcXAxis(CChartRegion* pRegion, CDC* pDC, CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aXAxisDivide)
{
	aAxisNodes.SetSize(0);
	aXAxisDivide.SetSize(0);

	if ( pRegion == m_pRegionMain )	// 主图的Y轴显示
	{
		CalcMainRegionXAxis(aAxisNodes, aXAxisDivide);
	}
	else
	{
		// zhangbo 20090710 #待优化， 现在都按照主region重新算一遍， 其实不用算， 复制就可以了
		CalcMainRegionXAxis(aAxisNodes, aXAxisDivide);	
	}
}

void CIoViewTrend::OnCalcYAxis (CChartRegion* pRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide)
{
	aYAxisDivide.SetSize(0);

	// 
	if ( pRegion == m_pRegionMain )	// 主图的Y轴显示
	{
		CalcMainRegionYAxis(pDC, aYAxisDivide);
	}
	else	// 子图的Y轴显示
	{
		for ( int32 i = 0 ; i < m_SubRegions.GetSize(); i++)
		{
			if ( pRegion == m_SubRegions.GetAt(i).m_pSubRegionMain )
			{
				CalcSubRegionYAxis(pRegion,pDC,aYAxisDivide);
			}
		}
	}
}

void CIoViewTrend::DrawAlarmMerchLine()
{
	/*
	if (NULL == m_pMerchXml)
	return;

	// 画条件预警的警戒线:
	if ( NULL == m_pRegionMain)
	{
	return;
	}

	CMemDCEx * pDC = m_pRegionMain->GetViewParam()->GetMainDC();

	if ( NULL == pDC)
	{
	return;
	}

	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();

	if ( !pMainFrame->m_bAlarm)
	{	
	return;
	}

	CArray<T_AlarmMerch,T_AlarmMerch&> aAlarmMerchList;

	pMainFrame->GetAlarmMerchList(aAlarmMerchList);

	for ( int32 i = 0 ; i < aAlarmMerchList.GetSize(); i++)
	{
	T_AlarmMerch &AlarmMerch = aAlarmMerchList[i];
	if (m_pMerchXml == AlarmMerch.m_pMerch)
	{
	// 有这个商品			
	if ( 1 == AlarmMerch.m_iConditon || 2 == AlarmMerch.m_iConditon)
	{
	// 是价格上破或者下破的条件:				
	float fPrice = AlarmMerch.m_fParam;

	// zhangbo 20090703 #暂时杠掉
	// 				// 看这个价格在不在最小值和最大值之间.是的话就画线:
	// 				
	// 				CString StrMin = m_pRegionYLeft->m_AxisTexts.GetAt(0);
	// 				CString StrMax = m_pRegionYLeft->m_AxisTexts.GetAt(m_pRegionYLeft->m_AxisTexts.GetSize()-1);
	// 				
	// 				TCHAR * test;
	// 				int length = StrMin.GetLength();
	// 				test = StrMin.GetBuffer(length);
	// 				
	// 				char* strtest = _W2A(test);    
	// 				float fMin = atof(strtest);
	// 								
	// 				length = StrMax.GetLength();
	// 				test   = StrMax.GetBuffer(length);
	// 				
	// 				strtest = _W2A(test);
	// 				float fMax = atof(strtest);
	// 				
	// 				if ( fPrice >= fMin && fPrice <= fMax)
	// 				{
	// 					// 开始画警戒线咯: 取出Y 轴刻度的最大值和最小值
	// 
	// 					if ( m_pRegionYLeft->m_AxisTexts.GetSize() > 0 )
	// 					{						
	// 						// 算出警戒线的Y 坐标:
	// 						
	// 						CRect Rect = m_pRegionYLeft->GetRectCurves();
	// 						
	// 						int32 iY = Rect.Height()*(fPrice - fMin)/(fMax - fMin);
	// 						iY = Rect.bottom - iY;
	// 						
	// 						// 画线:
	// 						
	// 						CPoint ptAlarmleft,ptAlarmright;
	// 						
	// 						ptAlarmleft.x  = m_pRegionMain->GetRectCurves().left  + 1;
	// 						ptAlarmright.x = m_pRegionMain->GetRectCurves().right - 1;
	// 						
	// 						ptAlarmleft.y  = iY;
	// 						ptAlarmright.y = iY;
	// 						
	// 						COLORREF color = RGB(0,255,255);
	// 						CPen pen(PS_SOLID,1,color);
	// 						CPen * pOldPen = (CPen*)pDC->SelectObject(&pen);								
	// 						pDC->_DrawDotLine(ptAlarmleft,ptAlarmright,2,RGB(0,255,255));
	// 						
	// 						
	// 						// 写字:
	// 
	// 						CString StrAlarm;
	// 
	// 						StrAlarm.Format(L"警:%.3f",fPrice);
	// 						CFont * pSmallFont = GetIoViewFontObject(ESFSmall);
	// 						CFont * pOldFont   = pDC->SelectObject(pSmallFont);
	// 						CSize sizeAlarm = pDC->GetTextExtent(StrAlarm);
	// 						
	// 						CRect RectText;
	// 						RectText.right  = ptAlarmright.x - 2;
	// 						RectText.bottom = ptAlarmright.y - 2;
	// 						RectText.left   = RectText.right - sizeAlarm.cx - 2;
	// 						RectText.top	= RectText.bottom- sizeAlarm.cy - 2;
	// 
	// 						pDC->SetTextColor(RGB(0,255,255));
	// 						pDC->DrawText(StrAlarm,&RectText,DT_CENTER|DT_SINGLELINE|DT_VCENTER);
	// 						
	// 						pDC->SelectObject(pOldFont);
	// 						pDC->SelectObject(pOldPen);
	// 
	// 						pen.DeleteObject();								
	// 					}							
	// 				}						
	}					
	}
	}
	*/
}

void CIoViewTrend::DrawRealtimePriceLine()
{
	CMemDCEx * pDC = m_pRegionMain->GetViewParam()->GetMainDC();

	//
	if (NULL == pDC || NULL == m_pMerchXml || NULL == m_pMerchXml->m_pRealtimePrice || m_pMerchXml->m_pRealtimePrice->m_fPriceNew <= 0.)
		return;

	// 画最新价的线:
	if ( NULL == m_pRegionMain)
	{
		return;
	}

	CChartCurve* pDependCurve = m_pRegionMain->GetDependentCurve();
	if (NULL == pDependCurve)
	{
		return;
	}

	//
	if (m_MerchParamArray.GetSize() <= 0)
	{
		return;
	}

	//
	int32 iSize = pDependCurve->GetNodes()->GetSize();
	if (iSize <= 0)
	{
		return;
	}

	CNodeData Node;
	if (!pDependCurve->GetNodes()->GetAt(iSize - 1, Node))
	{
		return;
	}

	//
	float fPriceNew = Node.m_fClose;
	float fPricePre = m_MerchParamArray[0]->m_TrendTradingDayInfo.GetPrevReferPrice();

	//
	COLORREF clrShow = GetIoViewColor(ESCRise);
	if (fPriceNew == fPricePre)
	{
		clrShow = GetIoViewColor(ESCKeep);
	}
	else if (fPriceNew < fPricePre)
	{
		clrShow = GetIoViewColor(ESCFall);
	}

	COLORREF clrLine = RGB(100,100,100);

	//
	int32 iX;
	int32 iYPos;

	//
	pDependCurve->PriceYToRegionY(fPriceNew, iYPos);
	m_pRegionMain->RegionYToClient(iYPos);

	TRACE(L"0: price: %.2f pos: %d\r\n", fPriceNew, iYPos);

	//
	if (pDependCurve->CurvePosToRegionX(iSize - 1, iX))
	{	
		m_pRegionMain->RegionXToClient(iX);
		CPoint ptCenter(iX, iYPos);

		// 画横线
		CPen penDraw;
		penDraw.CreatePen(PS_SOLID, 1, clrLine);
		CPen* pOldPen = (CPen*)pDC->SelectObject(&penDraw);;
		CRect rtRegion = m_pRegionMain->m_RectView;

		//
		pDC->MoveTo(iX, iYPos);
		pDC->LineTo(rtRegion.right, iYPos);		

		// 画一个焦点
		CBrush br(clrShow);
		CBrush* pOldBrush = pDC->SelectObject(&br);

		CRect rtCircle(CPoint(ptCenter.x - 3, ptCenter.y - 3), CPoint(ptCenter.x + 3, ptCenter.y + 3));
		pDC->Ellipse(rtCircle);

		// 画现价和涨跌幅
		CString StrNew = OnFloat2String(fPriceNew, false, false);
		CString StrPercent;
		Float2StringLimitMaxLength(StrPercent, 100.0f*(fPriceNew - fPricePre)/fPricePre, 2, GetYLeftShowCharWidth() - 1, true, false, false, true);

		pDC->SelectObject(GetIoViewFontObject(ESFSmall));

		//
		CRect rtNew;
		CRect rtPercent;

		//
		CSize szNew = pDC->GetTextExtent(StrNew);
		CSize szPercent = pDC->GetTextExtent(StrPercent);

		int32 iWidth = max(szNew.cx, szPercent.cx) + 5;
		int32 iHeight= m_pRegionMain->m_iAxisTextMaxHeight;

		if (iX + 10 >= rtRegion.right - iWidth)
		{
			// 如果数据快满了, 太靠近右侧, 就把现价和涨跌幅画到坐标轴上面
			m_bDrawYRigthPrice = true;
		}
		else
		{
			m_bDrawYRigthPrice = false;
		}

		// 高度太小就不要画了
		if (rtRegion.Height() >(2 * iHeight) && !m_bDrawYRigthPrice)
		{
			//
			rtNew.right = rtRegion.right;
			rtNew.left  = rtRegion.right - iWidth;

			rtNew.bottom = iYPos;
			rtNew.top = rtNew.bottom - iHeight;

			rtPercent = rtNew;
			rtPercent.top = iYPos;
			rtPercent.bottom = rtPercent.top + iHeight;

			//
			if (rtNew.top < rtRegion.top)
			{
				rtNew.top = iYPos;
				rtNew.bottom = rtNew.top + iHeight;

				//
				rtPercent = rtNew;
				rtPercent.top = rtNew.bottom;
				rtPercent.bottom = rtPercent.top + iHeight;
			}
			else if (rtPercent.bottom > rtRegion.bottom)
			{
				rtPercent.bottom = iYPos;
				rtPercent.top = rtPercent.bottom - iHeight;

				//
				rtNew.bottom = rtPercent.top;
				rtNew.top = rtNew.bottom - iHeight;
			}

			//
			pDC->SetTextColor(clrShow);

			pDC->FillSolidRect(rtNew, m_pRegionMain->m_clrSliderFill);
			pDC->DrawText(StrNew, rtNew, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
			pDC->MoveTo(rtNew.left, rtNew.bottom);
			pDC->LineTo(rtNew.left, rtNew.top);
			pDC->LineTo(rtNew.right, rtNew.top);

			//
			pDC->FillSolidRect(rtPercent, m_pRegionMain->m_clrSliderFill);
			pDC->DrawText(StrPercent, rtPercent, DT_VCENTER | DT_CENTER | DT_SINGLELINE);			
			pDC->MoveTo(rtPercent.right, rtPercent.bottom);
			pDC->LineTo(rtPercent.left, rtPercent.bottom);
			pDC->LineTo(rtPercent.left, rtPercent.top);
			pDC->LineTo(rtPercent.right, rtPercent.top);
		}

		//
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
	}
}

void CIoViewTrend::DrawRealTimeSlider(CChartRegion* pRegion, CMemDCEx* pDC)
{
	if (!m_bDrawYRigthPrice)
	{
		return;
	}

	if(NULL == pRegion || NULL == pDC)
	{
		return;
	}

	//
	if(NULL == m_pRegionMain || NULL == m_pRegionMain->GetDependentCurve() || NULL == m_pRegionYRight)
	{
		return;
	}

	CChartCurve* pDependentCurve = m_pRegionMain->GetDependentCurve();

	//
	int32 iSize = pDependentCurve->GetNodes()->GetSize();
	if (iSize < 0)
	{
		return;
	}

	if (m_MerchParamArray.GetSize() <= 0)
	{
		return;
	}

	//
	CNodeData Node;
	if (!pDependentCurve->GetNodes()->GetAt(iSize - 1, Node))
	{
		return;
	}

	float fNew = Node.m_fClose;
	float fPre = m_MerchParamArray[0]->m_TrendTradingDayInfo.GetPrevReferPrice();

	if (fNew <= 0. || fPre <= 0.)
	{
		return;
	}

	//
	int32 iYPos;
	if (!pDependentCurve->PriceYToRegionY(fNew, iYPos))
	{
		return;
	}

	m_pRegionMain->RegionYToClient(iYPos);

	COLORREF clrShow = GetIoViewColor(ESCRise);
	if (fNew == fPre)
	{
		clrShow = GetIoViewColor(ESCKeep);
	}
	else if (fNew < fPre)
	{
		clrShow = GetIoViewColor(ESCFall);
	}

	CString StrNew = OnFloat2String(fNew, false, true);
	CString StrPercent;
	Float2StringLimitMaxLength(StrPercent, 100.0f*(fNew - fPre)/fPre, 2, GetYLeftShowCharWidth() - 1, true, false, false, true);

	CFont* pOldFont = pDC->SelectObject(GetIoViewFontObject(ESFSmall));

	//
	CRect rtRegion = m_pRegionYRight->m_RectView;
	CRect rtNew;
	CRect rtPercent;

	//
	int32 iWidth = rtRegion.Width();
	int32 iHeight= m_pRegionYRight->m_iAxisTextMaxHeight;

	// 高度太小就不要画了
	if (rtRegion.Height() >(2 * iHeight))
	{
		//
		rtNew.right = rtRegion.right;
		rtNew.left  = rtRegion.right - iWidth;

		rtNew.bottom = iYPos;
		rtNew.top = rtNew.bottom - iHeight;

		rtPercent = rtNew;
		rtPercent.top = iYPos;
		rtPercent.bottom = rtPercent.top + iHeight;

		//
		if (rtNew.top < rtRegion.top)
		{
			rtNew.top = iYPos;
			rtNew.bottom = rtNew.top + iHeight;

			//
			rtPercent = rtNew;
			rtPercent.top = rtNew.bottom;
			rtPercent.bottom = rtPercent.top + iHeight;
		}
		else if (rtPercent.bottom > rtRegion.bottom)
		{
			rtPercent.bottom = iYPos;
			rtPercent.top = rtPercent.bottom - iHeight;

			//
			rtNew.bottom = rtPercent.top;
			rtNew.top = rtNew.bottom - iHeight;
		}

		//
		pDC->SetTextColor(clrShow);

		pDC->FillSolidRect(rtNew, m_pRegionMain->m_clrSliderFill);
		pDC->DrawText(StrNew, rtNew, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
		pDC->MoveTo(rtNew.left, rtNew.bottom);
		pDC->LineTo(rtNew.left, rtNew.top);
		pDC->LineTo(rtNew.right, rtNew.top);

		//
		pDC->FillSolidRect(rtPercent, m_pRegionMain->m_clrSliderFill);
		pDC->DrawText(StrPercent, rtPercent, DT_VCENTER | DT_CENTER | DT_SINGLELINE);			
		pDC->MoveTo(rtPercent.right, rtPercent.bottom);
		pDC->LineTo(rtPercent.left, rtPercent.bottom);
		pDC->LineTo(rtPercent.left, rtPercent.top);
		pDC->LineTo(rtPercent.right, rtPercent.top);
	}

	pDC->SelectObject(pOldFont);
}

void CIoViewTrend::ValueYAsNodata(float fYMin, float fYMax, const CRect& Rect,int32 iSkip, float fy,int32& iValue)
{
	iValue = 0;

	float fRate     =  (fy - fYMin) / ( fYMax - fYMin );
	float fValue	=  fRate * Rect.Height();

	iValue			=  (int32)(ceil ( fValue ));
	iValue			=  Rect.Height() - iValue;
	iValue		   +=  iSkip;
}

void CIoViewTrend::OnRegionDrawNotify(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC,CRegion* pRegion,E_RegionDrawNotifyType eType)
{
	CChartRegion* pChartRegion = (CChartRegion*)pRegion;

	if ( eType == ERDNBeforeTransformAllRegion )
	{
		return;
	}

	if ( eType == ERDNAfterDrawCurve )
	{
		//主图
		if (pChartRegion == m_pRegionMain )
		{
			if(NULL != m_pMerchXml)
			{
				// 是大宗商品，才绘制财经日历小圆点
				if (m_pMerchXml->m_Market.m_MarketInfo.m_eMarketReportType == ERTMony)
				{
					// 绘制财经日历小圆点
					DrawCircleDot(pDC);
				}
			}	
			DrawTitle1(pDC);

			// 画条件预警线
			DrawAlarmMerchLine();

			// 信息地雷
			DrawLandMines(pDC);

			//
			DrawInterval(pDC, pChartRegion);

			//
			DrawRealtimePriceLine();	
		}
		if ( pChartRegion == m_pRegionSeparator )
		{
			AddDrawBaseRegion(pDC);
		}
		if ( pChartRegion == m_pRegionLeftBottom )
		{
			AddDrawCornerRegion(pDC);
		}
		if ( pChartRegion == m_pRegionRightBottom )
		{
			AddDrawCornerRegion(pDC);
		}

		// 竞价图
		if ( ETBT_CompetePrice == GetCurTrendBtnType() )
		{
			DrawCompetePriceChart(pDC, *pChartRegion);
		}

		if ( m_SubRegions.GetSize() > 0
			&& m_SubRegions[0].m_pSubRegionMain == pChartRegion )
		{
			// 第一个副图新增持仓画进去 xl 1102
		}
	}

	CString StrText;
	CPoint pt1,pt2;
	COLORREF color;
	CPen pen,*pOldPen;

	color = GetIoViewColor(ESCChartAxisLine);
	pen.CreatePen(PS_SOLID,1,color);
	pOldPen = (CPen*)pDC->SelectObject(&pen);	
	CFont* pOldFont = pDC->SelectObject(GetIoViewFontObject(ESFSmall));

	if ( eType == ERDNBeforeDrawCurve )
	{
		// DrawRealtimePriceLine(pDC);

		//
		CSize SizeFont	  = pDC->GetTextExtent(L"123");

		int32 iFontHeight = SizeFont.cy;
		int32 iHalfHeight = iFontHeight / 2;

		// 主图
		if (pChartRegion == m_pRegionMain)	// 主图
		{
			CRect RectMain = m_pRegionMain->GetRectCurves();

			// 画水平线
			{
				int32 iSize = m_pRegionMain->m_aYAxisDivide.GetSize();
				CAxisDivide *pAxisDivide = (CAxisDivide *)m_pRegionMain->m_aYAxisDivide.GetData();

				if ( NULL == pAxisDivide )
				{
					pAxisDivide = (CAxisDivide *)m_aYAxisDivideBkMain.GetData();
					iSize		= m_aYAxisDivideBkMain.GetSize();
				}

				for (int32 i = 0; i < iSize; i++)
				{
					CAxisDivide &AxisDivide = pAxisDivide[i];
					pt1.y = pt2.y = AxisDivide.m_iPosPixel;
					pt1.x = RectMain.left;
					pt2.x = RectMain.right;

					// 画线
					if (CAxisDivide::ELSSolid == AxisDivide.m_eLineStyle)
					{
						pDC->_DrawLine(pt1, pt2);
					}
					else if (CAxisDivide::ELSDoubleSolid == AxisDivide.m_eLineStyle)
					{						
						pDC->_DrawLine(pt1, pt2);

						pt1.y++; pt2.y++;
						pDC->_DrawLine(pt1, pt2);
					}
					else if (CAxisDivide::ELS3LineSolid == AxisDivide.m_eLineStyle)
					{					
						pDC->_DrawLine(pt1, pt2);

						pt1.y ++; pt2.y ++;
						pDC->_DrawLine(pt1, pt2);
					}
					else if (CAxisDivide::ELSDot == AxisDivide.m_eLineStyle)
					{
						pDC->_DrawDotLine(pt1, pt2, 2, color);
					}
				}

				if ( 0 == m_pRegionMain->m_aYAxisDivide.GetSize() && 0 == m_aYAxisDivideBkMain.GetSize())
				{
					pt1.x = RectMain.left;
					pt1.y = RectMain.top;

					pt2.x = RectMain.right;
					pt2.y = RectMain.top;

					pDC->_DrawLine(pt1, pt2);
				}

			}

			// 画垂直线
			{
				CAxisDivide *pAxisDivide = (CAxisDivide *)m_pRegionMain->m_aXAxisDivide.GetData();
				// 第一根不要画了,重合了不好看，最后一个也不画，
				for (int32 i = 1; i < m_pRegionMain->m_aXAxisDivide.GetSize()-1; i++)
				{
					CAxisDivide &AxisDivide = pAxisDivide[i];
					pt1.x = pt2.x = AxisDivide.m_iPosPixel;
					pt1.y = RectMain.top;
					pt2.y = RectMain.bottom;

					if (CAxisDivide::ELSSolid == AxisDivide.m_eLineStyle)
					{
						if ( m_aTrendMultiDayIOCTimes.GetSize() > 1 )
						{
							// 多日分时，则画实线
							pDC->_DrawLine(pt1, pt2);
						}
						else
						{
							pDC->_DrawDotLine(pt1, pt2, 2, color);
						}
					}
					else if (CAxisDivide::ELS3LineSolid == AxisDivide.m_eLineStyle)
					{
						pt1.x--; pt2.x--;
						pDC->_DrawDotLine(pt1, pt2, 2, color);

						pt1.x += 2; pt2.x += 2;
						pDC->_DrawDotLine(pt1, pt2, 2, color);

						pt1.x--; pt2.x--;
						pDC->_DrawDotLine(pt1, pt2, 2, color);
					}
					else if (CAxisDivide::ELSDoubleSolid == AxisDivide.m_eLineStyle)
					{
						pt1.x--; pt2.x--;
						pDC->_DrawDotLine(pt1, pt2, 2, color);

						pt1.x++; pt2.x++;
						pDC->_DrawDotLine(pt1, pt2, 2, color);
					}
					else if (CAxisDivide::ELSDot == AxisDivide.m_eLineStyle)
					{
						pDC->_DrawDotLine(pt1, pt2, 2, color);
					}

					m_vecTimePos.push_back(pt1.x);
				}

				if ( 0 == m_pRegionMain->m_aXAxisDivide.GetSize() )
				{
					// 没数据的时候,左中右
					pt1.x = RectMain.left;
					pt1.y = RectMain.top;

					pt2.x = RectMain.left;
					pt2.y = RectMain.bottom;

					pDC->_DrawLine(pt1, pt2);

					//
					pt1.x = (RectMain.left + RectMain.right) / 2;
					pt1.y = RectMain.top;

					pt2.x = (RectMain.left + RectMain.right) / 2;
					pt2.y = RectMain.bottom;

					pDC->_DrawLine(pt1, pt2);

					//
					pt1.x = RectMain.right;
					pt1.y = RectMain.top;

					pt2.x = RectMain.right;
					pt2.y = RectMain.bottom;

					pDC->_DrawLine(pt1, pt2);
				}
			}

			// 画鬼第一个开盘线
			if ( IsShowTrendFirstOpenPrice() )
			{
				DrawTrendFirstOpenPrice(pDC);
			}
		}
		else if ( pChartRegion == m_pRegionYLeft )
		{
			/*CRect RectMain = */m_pRegionMain->GetRectCurves();
			CRect RectYLeft = m_pRegionYLeft->GetRectCurves();
			RectYLeft.DeflateRect(3, 3, 3, 3);

			// 画左文字
			CAxisDivide *pAxisDivide = (CAxisDivide *)m_pRegionMain->m_aYAxisDivide.GetData();
			for (int32 i = 0; i < m_pRegionMain->m_aYAxisDivide.GetSize(); i++)
			{
				CAxisDivide &AxisDivide = pAxisDivide[i];

				CRect Rect = RectYLeft;

				if ((AxisDivide.m_DivideText1.m_uiTextAlign & DT_VCENTER) != 0)						
				{
					Rect.top	= AxisDivide.m_iPosPixel - iHalfHeight;
					Rect.bottom = AxisDivide.m_iPosPixel + iHalfHeight;
				}
				else if ((AxisDivide.m_DivideText1.m_uiTextAlign & DT_BOTTOM) != 0)
				{
					Rect.bottom = AxisDivide.m_iPosPixel;
					Rect.top	= Rect.bottom - iFontHeight;							
				}
				else
				{
					Rect.top	= AxisDivide.m_iPosPixel;
					Rect.bottom	= Rect.top + iFontHeight;
				}

				Rect.right -= 10;

				if ( Rect.bottom > RectYLeft.bottom )
				{
					continue;
				}
				// 
				pDC->SetTextColor(AxisDivide.m_DivideText1.m_lTextColor);
				pDC->SetBkMode(TRANSPARENT);
				pDC->DrawText(AxisDivide.m_DivideText1.m_StrText, &Rect, AxisDivide.m_DivideText1.m_uiTextAlign);
			}
		}
		else if (pChartRegion == m_pRegionYRight)
		{
			/*CRect RectMain = */m_pRegionMain->GetRectCurves();
			CRect RectYRight= m_pRegionYRight->GetRectCurves();
			RectYRight.DeflateRect(3, 3, 3, 3);

			// 画右文字
			CAxisDivide *pAxisDivide = (CAxisDivide *)m_pRegionMain->m_aYAxisDivide.GetData();
			for (int32 i = 1; i < m_pRegionMain->m_aYAxisDivide.GetSize(); i++)
			{
				CAxisDivide &AxisDivide = pAxisDivide[i];

				CRect Rect = RectYRight;
				if ((AxisDivide.m_DivideText2.m_uiTextAlign & DT_VCENTER) != 0)						
				{
					Rect.top	= AxisDivide.m_iPosPixel - iHalfHeight;
					Rect.bottom = AxisDivide.m_iPosPixel + iHalfHeight;
				}
				else if ((AxisDivide.m_DivideText2.m_uiTextAlign & DT_BOTTOM) != 0)
				{
					Rect.bottom = AxisDivide.m_iPosPixel;
					Rect.top	= Rect.bottom - iFontHeight;							
				}
				else
				{
					Rect.top	= AxisDivide.m_iPosPixel;
					Rect.bottom	= Rect.top + iFontHeight;
				}

				Rect.left += 10;
				// 
				pDC->SetTextColor(AxisDivide.m_DivideText2.m_lTextColor);
				pDC->SetBkMode(TRANSPARENT);
				pDC->DrawText(AxisDivide.m_DivideText2.m_StrText, &Rect, AxisDivide.m_DivideText2.m_uiTextAlign);
			}

			DrawRealTimeSlider(pChartRegion, pDC);
		}
		else if (pChartRegion == m_pRegionXBottom)	// 画横坐标
		{
			// 以下代码为避免绘制文字时互相叠加的情况发生
			int32 iWidthSpace = 5;

			// 
			CRect RectMain		= m_pRegionMain->GetRectCurves();
			CRect RectXBottom	= m_pRegionXBottom->GetRectCurves();
			RectXBottom.left = RectMain.left;
			RectXBottom.right = RectMain.right;
			RectXBottom.InflateRect(iWidthSpace, 0, iWidthSpace, 0);

			CArray<CAxisDivide, CAxisDivide&> aXAxisDivide;
			aXAxisDivide.Copy(m_pRegionMain->m_aXAxisDivide);
			CAxisDivide *pAxisDivide = (CAxisDivide *)aXAxisDivide.GetData();
			int32 iAxisDivideCount = aXAxisDivide.GetSize();

			// 按优先级排序
			int32 i = 0;
			/*for ( i = 0; i < iAxisDivideCount - 1; i++)
			{
			for (int32 j = i + 1; j < iAxisDivideCount; j++)
			{
			if (pAxisDivide[i].m_eLevelText > pAxisDivide[j].m_eLevelText)
			{
			CAxisDivide AxisDivideTemp;
			AxisDivideTemp	= pAxisDivide[i];
			pAxisDivide[i]	= pAxisDivide[j];
			pAxisDivide[j]	= AxisDivideTemp;
			}
			}
			}*/

			// 挨个输出文字， 避免文字间重叠
			CArray<CRect, CRect&> aRectSpace;	// 空闲区域
			aRectSpace.SetSize(0, 20);
			aRectSpace.Add(RectXBottom);

			m_vecXAxisInfo.clear();
			for (i = 0; i < iAxisDivideCount; i++)
			{
				CSize SizeText = pDC->GetOutputTextExtent(pAxisDivide[i].m_DivideText1.m_StrText);

				CRect Rect = RectXBottom;
				if ((pAxisDivide[i].m_DivideText1.m_uiTextAlign & DT_CENTER) != 0)						
				{
					Rect.left	= pAxisDivide[i].m_iPosPixel - (SizeText.cx / 2 + iWidthSpace);
					Rect.right	= pAxisDivide[i].m_iPosPixel + (SizeText.cx / 2 + iWidthSpace);
				}
				else if ((pAxisDivide[i].m_DivideText1.m_uiTextAlign & DT_RIGHT) != 0)
				{
					Rect.right	= pAxisDivide[i].m_iPosPixel + iWidthSpace;
					Rect.left	= pAxisDivide[i].m_iPosPixel - (SizeText.cx + iWidthSpace);
				}
				else
				{
					Rect.left	= pAxisDivide[i].m_iPosPixel - iWidthSpace;
					Rect.right	= pAxisDivide[i].m_iPosPixel + (SizeText.cx + iWidthSpace);
				}

				// 如果是大宗商品，那么保存该商品分时图的时间刻度
				if (m_pMerchXml->m_Market.m_MarketInfo.m_eMarketReportType == ERTMony)
				{
					SaveTimeDegreeScale(Rect.left + Rect.Width()/2, pAxisDivide[i].m_DivideText1.m_StrText);
				}


				// 从空闲空间中查找， 看看是否可以显示
				int32 iPosFindInSpaceRects = -1;

				CRect *pRectSpace = (CRect *)aRectSpace.GetData();
				for (int32 iIndexSpace = 0; iIndexSpace < aRectSpace.GetSize(); iIndexSpace++)
				{
					if (Rect.left < pRectSpace[iIndexSpace].left || Rect.right > pRectSpace[iIndexSpace].right)
						continue;

					iPosFindInSpaceRects = iIndexSpace;
					break;
				}

				// 拆分空闲区域
				if (iPosFindInSpaceRects < 0)
					continue;
				else
				{
					// 画文字
					// 定制版本，K线坐标轴文字颜色暂时设置成固定值
					pDC->SetTextColor(RGB(125, 124, 129));
					//pDC->SetTextColor(pAxisDivide[i].m_DivideText1.m_lTextColor);
					pDC->SetBkMode(TRANSPARENT);
					pDC->DrawText(pAxisDivide[i].m_DivideText1.m_StrText, &Rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

					// 拆分空闲区域
					CRect RectSubLeft = pRectSpace[iPosFindInSpaceRects];
					CRect RectSubRight = pRectSpace[iPosFindInSpaceRects];

					RectSubLeft.right = Rect.left - 1;
					RectSubRight.left = Rect.right + 1;

					aRectSpace.RemoveAt(iPosFindInSpaceRects);

					//
					if (RectSubRight.Width() > iWidthSpace * 2)
					{
						aRectSpace.InsertAt(iPosFindInSpaceRects, RectSubRight);
					}

					if (RectSubLeft.Width() > iWidthSpace * 2)
					{
						aRectSpace.InsertAt(iPosFindInSpaceRects, RectSubLeft);
					}
				}
			}

			// 当分时图窗口的大小改变了，那么要重新计算财经日历数据的小圆点坐标
			if (m_bSizeChange && !m_bMoreView)
			{
				if (!m_vecXAxisInfo.empty())
				{
					ParseEonomicData(m_strEconoData.c_str());
					m_bSizeChange = false;
				}
			}
		}
		else if ( pChartRegion == m_pRegionLeftBottom )	// 显示当前显示的年月日
		{
			CString StrDay;

			// 
			T_MerchNodeUserData* pData = NULL;
			if (m_MerchParamArray.GetSize() > 0)
			{
				pData = m_MerchParamArray.GetAt(0);
			}

			if (NULL != pData && m_aTrendMultiDayIOCTimes.GetSize() <= 1)
			{
				// 
				CTrendTradingDayInfo &TrendTradingDay = pData->m_TrendTradingDayInfo;
				if (TrendTradingDay.m_bInit)
				{
					CTime TimeOpen(TrendTradingDay.m_MarketIOCTime.m_TimeOpen.m_Time.GetTime());

					int32 iYear = TimeOpen.GetYear();
					iYear -= 2000;
					if (iYear < 0)
						iYear = 100 - iYear;

					StrDay.Format(L"%02d/%02d/%02d", iYear, TimeOpen.GetMonth(), TimeOpen.GetDay());
				}
			}

			if (StrDay.GetLength() > 0)
			{
				//完整时间
				pDC->SetTextColor(RGB(125, 124, 129));	//定制版本，暂时用固定色值
				int OldMode = pDC->GetBkMode();
				pDC->SetBkMode(TRANSPARENT);

				CRect Rect = m_pRegionLeftBottom->GetRectCurves();
				pDC->DrawText(StrDay,Rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE );
				pDC->SetBkMode(OldMode);
			}
		}
		else if ( pChartRegion == m_pRegionRightBottom )	// m_pRegionRightBottom
		{
			//NULL;
		}
		else	// 副图
		{
			for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
			{
				T_SubRegionParam &SubRegion = m_SubRegions[i];

				if (pChartRegion == SubRegion.m_pSubRegionMain)
				{
					if ( 0 == i )
					{
						// 第一个副图必然是成交量图, 该图不显示线的数据
						AddFlag(pChartRegion->m_iFlag, CChartRegion::KDonotShowCurveDataText);
					}
					CRect RectSubMain = SubRegion.m_pSubRegionMain->GetRectCurves();

					{
						// 封口
						if (  pChartRegion->GetTopSkip() > iHalfHeight+6 )		// 成交量副图的topskip比这少，不比画回圈 )
						{
							pt1 = RectSubMain.TopLeft();
							pt2 = CPoint(RectSubMain.right, RectSubMain.top);
							pDC->_DrawLine(pt1, pt2);
						}

						pt1 = CPoint(RectSubMain.left, RectSubMain.bottom);
						pt2 = CPoint(RectSubMain.right, RectSubMain.bottom);
						pDC->_DrawLine(pt1, pt2);						
					}

					// 画水平线
					{
						int32 iSize = SubRegion.m_pSubRegionMain->m_aYAxisDivide.GetSize();

						CAxisDivide *pAxisDivide = (CAxisDivide *)SubRegion.m_pSubRegionMain->m_aYAxisDivide.GetData();
						if ( NULL == pAxisDivide )
						{
							pAxisDivide = (CAxisDivide*)m_aYAxisDivideBkSub.GetData();
							iSize = m_aYAxisDivideBkSub.GetSize();
						}

						for (int32 i = 0; i < iSize; i++)
						{
							CAxisDivide &AxisDivide = pAxisDivide[i];
							pt1.y = pt2.y = AxisDivide.m_iPosPixel;
							pt1.x = RectSubMain.left;
							pt2.x = RectSubMain.right;

							if ( pt1.y - RectSubMain.top < iHalfHeight+6 )
							{
								// 成交量副图的第一根虚线高度小于此
								continue;
							}

							// 画线
							if (CAxisDivide::ELSSolid == AxisDivide.m_eLineStyle)
							{
								pDC->_DrawLine(pt1, pt2);
							}
							else if (CAxisDivide::ELS3LineSolid == AxisDivide.m_eLineStyle)
							{
								pt1.y--; pt2.y--;
								pDC->_DrawLine(pt1, pt2);

								pt1.y += 2; pt2.y += 2;
								pDC->_DrawLine(pt1, pt2);

								pt1.y--; pt2.y--;
								pDC->_DrawLine(pt1, pt2);
							}
							else if (CAxisDivide::ELSDot == AxisDivide.m_eLineStyle)
							{
								pDC->_DrawDotLine(pt1, pt2, 2, color);
							}
						}
					}

					// 画垂直线
					{						
						CAxisDivide *pAxisDivide = (CAxisDivide *)m_pRegionMain->m_aXAxisDivide.GetData();
						// 第一根不要画了,重合了不好看，最后一根不画
						for (int32 i = 1; i < m_pRegionMain->m_aXAxisDivide.GetSize()-1; i++)
						{
							CAxisDivide &AxisDivide = pAxisDivide[i];
							pt1.x = pt2.x = AxisDivide.m_iPosPixel;
							pt1.y = RectSubMain.top;
							pt2.y = RectSubMain.bottom;

							if ( pChartRegion->GetTopSkip() < iHalfHeight+6 )
							{
								pt1.y = pChartRegion->m_RectView.top;	// 成交量副图直接穿过
							}

							if (CAxisDivide::ELSSolid == AxisDivide.m_eLineStyle)
							{
								if ( m_aTrendMultiDayIOCTimes.GetSize() > 1 )
								{
									// 多日分时，则画实线
									pDC->_DrawLine(pt1, pt2);
								}
								else
								{
									pDC->_DrawDotLine(pt1, pt2, 2, color);
								}
							}
							else if (CAxisDivide::ELS3LineSolid == AxisDivide.m_eLineStyle)
							{
								pt1.x--; pt2.x--;
								pDC->_DrawDotLine(pt1, pt2, 2, color);

								pt1.x += 2; pt2.x += 2;
								pDC->_DrawDotLine(pt1, pt2, 2, color);

								pt1.x--; pt2.x--;
								pDC->_DrawDotLine(pt1, pt2, 2, color);
							}
							else if (CAxisDivide::ELSDoubleSolid == AxisDivide.m_eLineStyle)
							{
								pt1.x--; pt2.x--;
								pDC->_DrawDotLine(pt1, pt2, 2, color);

								pt1.x++; pt2.x++;
								pDC->_DrawDotLine(pt1, pt2, 2, color);
							}
							else if (CAxisDivide::ELSDot == AxisDivide.m_eLineStyle)
							{
								pDC->_DrawDotLine(pt1, pt2, 2, color);
							}
						}	

						if ( 0 == m_pRegionMain->m_aXAxisDivide.GetSize() )
						{
							// 没数据的时候,左中右
							pt1.x = RectSubMain.left;
							pt1.y = RectSubMain.top;

							pt2.x = RectSubMain.left;
							pt2.y = RectSubMain.bottom;

							pDC->_DrawLine(pt1, pt2);

							//
							pt1.x = (RectSubMain.left + RectSubMain.right) / 2;
							pt1.y = RectSubMain.top;

							pt2.x = (RectSubMain.left + RectSubMain.right) / 2;
							pt2.y = RectSubMain.bottom;

							pDC->_DrawLine(pt1, pt2);

							//
							pt1.x = RectSubMain.right;
							pt1.y = RectSubMain.top;

							pt2.x = RectSubMain.right;
							pt2.y = RectSubMain.bottom;

							pDC->_DrawLine(pt1, pt2);
						}
					}

					//
					DrawInterval(pDC, pChartRegion);
				}
				else if (pChartRegion == SubRegion.m_pSubRegionYLeft)
				{
					CRect RectSubMain = SubRegion.m_pSubRegionMain->GetRectCurves();
					CRect RectYLeft = m_pRegionYLeft->GetRectCurves(); 
					RectYLeft.DeflateRect(3, 3, 3, 3);

					// 画左文字
					CAxisDivide *pAxisDivide = (CAxisDivide *)SubRegion.m_pSubRegionMain->m_aYAxisDivide.GetData();
					for (int32 i = 0; i < SubRegion.m_pSubRegionMain->m_aYAxisDivide.GetSize(); i++)
					{
						CAxisDivide &AxisDivide = pAxisDivide[i];

						CRect Rect = RectYLeft;
						if ((AxisDivide.m_DivideText1.m_uiTextAlign & DT_VCENTER) != 0)						
						{
							Rect.top	= AxisDivide.m_iPosPixel - iHalfHeight;
							Rect.bottom = AxisDivide.m_iPosPixel + iHalfHeight;
						}
						else if ((AxisDivide.m_DivideText1.m_uiTextAlign & DT_BOTTOM) != 0)
						{
							Rect.bottom = AxisDivide.m_iPosPixel;
							Rect.top	= Rect.bottom - iFontHeight;							
						}
						else
						{
							Rect.top	= AxisDivide.m_iPosPixel;
							Rect.bottom	= Rect.top + iFontHeight;
						}

						Rect.right -= 10;
						// 
						if ( Rect.bottom > RectSubMain.bottom )
						{
							continue;
						}

						pDC->SetTextColor(AxisDivide.m_DivideText1.m_lTextColor);
						pDC->SetBkMode(TRANSPARENT);
						pDC->DrawText(AxisDivide.m_DivideText1.m_StrText, &Rect, AxisDivide.m_DivideText1.m_uiTextAlign);
					}
				}
				else if (pChartRegion == SubRegion.m_pSubRegionYRight)
				{
					CRect RectSubMain = SubRegion.m_pSubRegionMain->GetRectCurves();
					CRect RectYRight = SubRegion.m_pSubRegionYRight->GetRectCurves(); 
					RectYRight.DeflateRect(3, 3, 3, 3);

					// 画右文字
					CAxisDivide *pAxisDivide = (CAxisDivide *)SubRegion.m_pSubRegionMain->m_aYAxisDivide.GetData();
					for (int32 i = 0; i < SubRegion.m_pSubRegionMain->m_aYAxisDivide.GetSize(); i++)
					{
						CAxisDivide &AxisDivide = pAxisDivide[i];

						CRect Rect = RectYRight;
						if ((AxisDivide.m_DivideText1.m_uiTextAlign & DT_VCENTER) != 0)						
						{
							Rect.top	= AxisDivide.m_iPosPixel - iHalfHeight;
							Rect.bottom = AxisDivide.m_iPosPixel + iHalfHeight;
						}
						else if ((AxisDivide.m_DivideText1.m_uiTextAlign & DT_BOTTOM) != 0)
						{
							Rect.bottom = AxisDivide.m_iPosPixel;
							Rect.top	= Rect.bottom - iFontHeight;							
						}
						else
						{
							Rect.top	= AxisDivide.m_iPosPixel;
							Rect.bottom	= Rect.top + iFontHeight;
						}

						Rect.left += 10;

						// 
						if ( Rect.bottom > RectSubMain.bottom )
						{
							continue;
						}

						pDC->SetTextColor(AxisDivide.m_DivideText2.m_lTextColor);
						pDC->SetBkMode(TRANSPARENT);
						pDC->DrawText(AxisDivide.m_DivideText2.m_StrText, &Rect, AxisDivide.m_DivideText2.m_uiTextAlign);
					}
				}
				else
				{
					//NULL;
				}
			}
		}

		//绘制指标水平线
		DrawIndexExtraY (pDC, pChartRegion );
	}

	pDC->SelectObject(pOldFont);
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}

void CIoViewTrend::OnCtrlLeftRight(bool32 bLeft)
{
	if (!m_bHistoryTrend)
	{
		return;
	}

	CGmtTimeSpan Span(1,0,0,0);

	if (bLeft)
	{
		m_TimeHistory -= Span;
		if ( 1 == m_TimeHistory.GetDayOfWeek())
		{
			// 星期天,请求星期五的.
			m_TimeHistory -= Span;
			m_TimeHistory -= Span;
		}
		else if ( 7 == m_TimeHistory.GetDayOfWeek())
		{
			// 星期六,请求星期五的.
			m_TimeHistory -= Span;
		}
	}
	else
	{
		T_MerchNodeUserData* pData = NULL;
		CMerch	* pMerch = NULL;

		if ( m_MerchParamArray.GetSize() > 0)
		{
			pData = m_MerchParamArray.GetAt(0);
		}

		if (NULL != pData)
		{
			pMerch = pData->m_pMerchNode;
		}

		CGmtTime CurTime = CGmtTime(1971,1,1,1,1,1);

		if (NULL != pMerch)
		{
			CurTime = m_pAbsCenterManager->GetServerTime();
		}

		if ( CGmtTime(1971,1,1,1,1,1) != CurTime && m_TimeHistory != CurTime )
		{
			// 如果当前显示的就是今天的,那么再请求就忽略
			CGmtTime TimeReq = m_TimeHistory + Span;
			if ( 1 == TimeReq.GetDayOfWeek())
			{
				TimeReq += Span;
			}
			else if ( 7 == TimeReq.GetDayOfWeek() )
			{
				TimeReq += Span;
				TimeReq += Span;
			}

			if ( TimeReq > CurTime )
			{
				return;
			}
			else
			{
				m_TimeHistory = TimeReq;
			}
		}
		else
		{
			return;
		}		
	}

	RequestViewData();
	// fangz0401 分时走势成交量有时不显示,子窗口标题.禁用问题.
}

void CIoViewTrend::OnSliderId ( int32& id, int32 iJump )
{
	SliderId ( id, iJump );
}

CString CIoViewTrend::OnGetChartGuid ( CChartRegion* pRegion )
{
	return GetChartGuid(pRegion);
}

///////////////////////////////////////////////////////////////////////////////
CString CIoViewTrend::GetDefaultXML()
{
	return CIoViewChart::GetDefaultXML();
}

void CIoViewTrend::DoFromXml()
{
	if (m_bShowTopMerchBar || (GetIoViewManager() && GetIoViewManager()->GetManagerTopbarStatus()))
	{
		m_iTopButtonHeight = TOP_BUTTON_HEIHGT - 1;	
	}
	SetViewToolBarHeight(m_iTopButtonHeight+m_iTopMerchHeight);
	BShowViewToolBar(m_bShowIndexToolBar, m_bShowTopToolBar);
	if ( m_bFromXml )
	{
		//	CIoViewChart::DoFromXml();

		// 让分时图的region高度还原
		UpdateAxisSize(false);	// 需要在算之前重置合适的x等区域高度
		// 清除这几个相关数据
		m_aRegionHeightXml.RemoveAll();
		m_aRegionHeightRadioXml.RemoveAll();
		m_IndexNameXml.RemoveAll();
		m_IndexRegionXml.RemoveAll();
		int32 i;
		m_iSubRegionNumXml = m_aSubRegionHeightRatios.GetSize();
		CRect rcView(0,0,0,0);
		GetClientRect(rcView);
		const int32 KIViewHeight = rcView.Height();
		for ( i = 0; i < m_iSubRegionNumXml; i ++ )
		{
			// 伪造xmlregion数据
			double dfRatio = m_aSubRegionHeightRatios[i];
			m_aRegionHeightXml.Add( (int32)(KIViewHeight*dfRatio) );
			m_aRegionHeightRadioXml.Add( dfRatio );
			m_IndexNameXml.Add(_T(""));
			m_IndexRegionXml.Add(1+i);
		}
		// 仅添加副图
		for ( i = 0; i < m_iSubRegionNumXml; i ++ )
		{
			AddSubRegion();
		}

		m_aRegionHeightXml.RemoveAll();
		m_aRegionHeightRadioXml.RemoveAll();
		m_IndexNameXml.RemoveAll();
		m_IndexRegionXml.RemoveAll();

		m_aSubRegionHeightRatios.RemoveAll();
	}
	if ( m_SubRegions.GetSize() == 0 )
	{
		AddSubRegion();
	}
	m_pRegionMain->NestSizeAll();
	T_SubRegionParam SubParam = m_SubRegions.GetAt(0);

	if (NULL != m_pMerchXml)
	{
		AddIndex(SubParam.m_pSubRegionMain,_T("VOLFS"));
	}
	else
	{
		m_IndexPostAdd.id = 0;
		m_IndexPostAdd.pRegion = SubParam.m_pSubRegionMain;
		m_IndexPostAdd.StrIndexName = _T("VOLFS");
	}

	if ( NULL != m_pMerchXml )
	{
		OnTrendBtnSelChanged();	// 按钮初始化, 这里会丢失原来的第二幅图的高度
	}
	else
	{
		SetTimer(KTimerIdTrendInit, KTimerPeriodTrendInit, NULL);	// 延时初始化按钮
	}

	m_bFromXml = false;
	OnVDataForceUpdate();
}

IChartBrige CIoViewTrend::GetRegionParentIoView()
{
	IChartBrige ChartRegion;
	ChartRegion.pWnd = this;
	ChartRegion.pIoViewBase = this;
	ChartRegion.pChartRegionData = this;
	return ChartRegion;
}

void CIoViewTrend::GetDrawCrossInfoNoData(int32 &iOpenTime,int32 &iCloseTime)
{
	iOpenTime  = m_iOpenTimeForNoData;
	iCloseTime = m_iCloseTimeForNoData;
}

void CIoViewTrend::SetChildFrameTitle()
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

CString CIoViewTrend::GetHistroyTitle(CGmtTime Time)
{
	CString StrTime;
	CString StrMerchName = L""; 
	T_MerchNodeUserData* pData = NULL;

	if ( m_MerchParamArray.GetSize() > 0)
	{
		pData = m_MerchParamArray.GetAt(0);
	}
	if (NULL != pData && NULL != pData->m_pMerchNode)
	{
		StrMerchName = pData->m_pMerchNode->m_MerchInfo.m_StrMerchCnName;
	}

	int32 iWeek = Time.GetDayOfWeek();
	CString StrWeek;
	switch(iWeek)
	{
	case 1:	 StrWeek = L"日";
		break;
	case 2:	 StrWeek = L"一";
		break;
	case 3:	 StrWeek = L"二";
		break;
	case 4:	 StrWeek = L"三";
		break;
	case 5:	 StrWeek = L"四";
		break;
	case 6:	 StrWeek = L"五";
		break;
	case 7:	 StrWeek = L"六";
		break;
	}

	//StrTime.Format(L"历史分时走势: %s %s %d年%d月%d日 星期%s  CTRL+ 左右方向键查看前后一天分时,滚轮切换商品",
	StrTime.Format(L"历史分时: %s(%s) %d年%d月%d日 星期%s  PageUp/Down键查看前后一天分时",
		StrMerchName.GetBuffer(), m_MerchXml.m_StrMerchCode.GetBuffer(),			 									
		Time.GetYear(),
		Time.GetMonth(),
		Time.GetDay(),
		StrWeek.GetBuffer()
		);
	return StrTime;
}

void CIoViewTrend::SetHistoryTime(CGmtTime TimeHistory)
{
	m_bHistoryTrend	= true;
	m_TimeHistory	= TimeHistory;
	CMarketIOCTimeInfo RecentTradingDay;
	if ( NULL != m_pMerchXml 
		&& m_MerchParamArray.GetSize() > 0
		&& m_pMerchXml->m_Market.GetSpecialTradingDayTime(m_TimeHistory, RecentTradingDay, m_pMerchXml->m_MerchInfo)
		)
	{
		T_MerchNodeUserData *pData = m_MerchParamArray[0];
		if ( NULL != pData )
		{
			pData->m_TrendTradingDayInfo.Set(m_pMerchXml, RecentTradingDay);
			pData->m_TrendTradingDayInfo.RecalcPrice(*m_pMerchXml);
		}
	}
	// 错过这次时机就只剩下merchchanged时机
}

void CIoViewTrend::ClearLocalData(bool32 bClearAll/* = true*/)
{
	for (int32 i = 0; i < m_MerchParamArray.GetSize(); i++)
	{
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(i);
		if ( !bClearAll && !pData->bMainMerch )
		{
			continue;	// 叠加商品不清空
		}
		pData->m_aKLinesCompare.RemoveAll();
		pData->m_aKLinesFull.RemoveAll();
		pData->m_aKLinesShowDataFlag.RemoveAll();

		pData->m_pKLinesShow->RemoveAll();

		RemoveIndexsNodesData ( pData );
	}

	if ( NULL != m_pRedGreenCurve
		&& NULL != m_pRedGreenCurve->GetNodes())
	{
		m_pRedGreenCurve->GetNodes()->RemoveAll();
	}

	if ( NULL != m_pNoWeightExpCurve
		&& NULL != m_pNoWeightExpCurve->GetNodes() )
	{
		m_pNoWeightExpCurve->GetNodes()->RemoveAll();
	}

	if ( bClearAll 
		&& NULL != m_pCompetePriceNodes )
	{
		// 竞价线不与主线同x轴，单独
		m_pCompetePriceNodes->RemoveAll();
	}
}

void CIoViewTrend::OnRectIntervalStatistics(int32 iNodeBegin, int32 iNodeEnd)
{
	// 先清空
	m_aKLineInterval.RemoveAll();

	// 错误情况:
	if (m_MerchParamArray.GetSize() <= 0)
	{
		return;
	}

	// 区间统计 多日下不允许
	if ( 1 != m_iTrendDayCount )
	{
		return;
	}

	T_MerchNodeUserData *pMainData = m_MerchParamArray[0];
	if (NULL == pMainData)
	{
		return;
	}

	if ( iNodeBegin >= iNodeEnd )
	{
		return;
	}

	int32 iNodeBeginNow, iNodeEndNow;
	GetNodeBeginEnd(iNodeBeginNow, iNodeEndNow);

	if ( iNodeBegin < iNodeBeginNow )
	{
		iNodeBegin = iNodeBeginNow;
	}

	if ( iNodeEnd > iNodeEndNow )
	{
		iNodeEnd = iNodeEndNow;
	}

	//
	if ( iNodeBegin < 0 || iNodeBegin >= pMainData->m_aKLinesFull.GetSize() )
	{
		return;
	}

	if ( iNodeEnd < 0 || iNodeEnd >= pMainData->m_aKLinesFull.GetSize() )
	{
		iNodeEnd = pMainData->m_aKLinesFull.GetSize() -1;
	}

	if ( (iNodeEnd - iNodeBegin + 1) <= 1 )
	{
		return;
	}

	// 准备好这一段的统计数据
	int32 iNodeCounts = iNodeEnd - iNodeBegin + 1;

	m_aKLineInterval.SetSize(iNodeCounts);

	CKLine* pKLineInterval = (CKLine*)m_aKLineInterval.GetData();
	CKLine* pKLineFull	   = (CKLine*)pMainData->m_aKLinesFull.GetData();

	memcpyex(pKLineInterval, pKLineFull + iNodeBegin, sizeof(CKLine) * iNodeCounts);
	ASSERT( pKLineInterval[iNodeCounts-1].m_TimeCurrent.GetTime() > 0 );
	ASSERT( pKLineFull[iNodeBegin+iNodeCounts-1].m_TimeCurrent.GetTime() > 0 );

	if ( 0 == iNodeBegin )
	{
		m_fPriceIntervalPreClose = pKLineFull[0].m_fPriceClose;
	}
	else
	{
		m_fPriceIntervalPreClose = pKLineFull[iNodeBegin - 1].m_fPriceClose;
	}

	// 弹出菜单:
	CPoint pt;
	GetCursorPos(&pt);

	//
	//CNewMenu Menu;
	//Menu.CreatePopupMenu();
	//Menu.AppendODMenu(L"区间统计", MF_STRING, KiMenuIDIntervalStatistic);	
	//Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, this);

	//OnMenuIntervalStatistic();

	// 重画视图
	ReDrawAysnc();
}

void CIoViewTrend::OnMenuIntervalStatistic()
{
	if ( m_MerchParamArray.GetSize() <= 0 )
	{
		return;
	}

	T_MerchNodeUserData *pMainData = m_MerchParamArray[0];
	if ( NULL == pMainData || NULL == m_pMerchXml )
	{
		return;
	}

	int32 iSize = m_aKLineInterval.GetSize();
	if ( iSize <= 0 )
	{
		return;
	}	

	//
	SetIntervalBeginTime(m_aKLineInterval[0].m_TimeCurrent);
	SetIntervalEndTime(m_aKLineInterval[iSize - 1].m_TimeCurrent);
	ReDrawAysnc();

	//
	CDlgTrendInterval Dlg;
	Dlg.SetKLineData(this, m_pMerchXml, m_fPriceIntervalPreClose, m_aKLineInterval);
	Dlg.DoModal();	
}

bool32 CIoViewTrend::UpdateMainMerchKLine(T_MerchNodeUserData &MerchNodeUserData)
{
	CMerch* pMerch = MerchNodeUserData.m_pMerchNode;
	if (NULL == pMerch)
	{
		return false;
	}
	ASSERT( MerchNodeUserData.bMainMerch );

	// 尝试更新走势相关的报价数据
	if (0. == MerchNodeUserData.m_TrendTradingDayInfo.m_fPricePrevClose && 0. == MerchNodeUserData.m_TrendTradingDayInfo.m_fPricePrevAvg)
	{
		float fPricePrevClose = MerchNodeUserData.m_TrendTradingDayInfo.GetPrevReferPrice();
		MerchNodeUserData.m_TrendTradingDayInfo.RecalcPrice(*pMerch);
		if (fPricePrevClose != MerchNodeUserData.m_TrendTradingDayInfo.GetPrevReferPrice())
		{
			// zhangbo 20090708 #待补充, 更新中轴值
			//...		
		}
	}
	if ( 0.0 == MerchNodeUserData.m_TrendTradingDayInfo.GetPrevHold() )
	{
		MerchNodeUserData.m_TrendTradingDayInfo.RecalcHold(*pMerch);
	}

	// 尝试请求多日分钟k线
	if ( m_aTrendMultiDayIOCTimes.GetSize() < m_iTrendDayCount )
	{
		TryReqMoreMultiDayMinuteKLineData();
	}

	CUpdateShowDataChangeHelper helperChange(this, &MerchNodeUserData);	// 辅组检查变化

	// 更新数据
	int32 iPosFound;
	CMerchKLineNode* pKLineRequest = NULL;
	pMerch->FindMerchKLineNode(EKTBMinute, iPosFound, pKLineRequest);
	if (NULL == pKLineRequest || 0 == pKLineRequest->m_KLines.GetSize())	// 根本找不到K线数据， 那就不需要显示了
	{
		MerchNodeUserData.m_aKLinesCompare.RemoveAll();
		MerchNodeUserData.m_aKLinesFull.RemoveAll();

		if (NULL != MerchNodeUserData.m_pKLinesShow)	
			MerchNodeUserData.m_pKLinesShow->RemoveAll();

		if ( IsSuspendedMerch() )
		{
			CreateSuspendedVirtualNoOpenKlineData();	// 停牌商品填充虚假数据
		}

		return true;
	}

	// 优化：绝大多数情况下， 该事件都是由于实时数据更新引发的， 对这种情况做特别判断处理

	CGmtTime TimeStart, TimeEnd;
	if ( !GetTrendStartEndTime(TimeStart, TimeEnd) )
	{
		ASSERT( 0 );	// 无效时间段？？

		return false;
	}

	if ( m_bSingleSectionTrend )
	{
		UpdateTrendMultiDayChange();	// 可能半场时间段已经发生了变更，需要更新标题显示, 现在总是更新标题显示
	}

	// 获取K线序列中， 用于显示的部分
	int32 iStartSrc	= CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pKLineRequest->m_KLines, TimeStart);
	if (iStartSrc < 0 || iStartSrc >= pKLineRequest->m_KLines.GetSize())
	{
		// 找不到需要显示时间内的分钟K线
		MerchNodeUserData.m_aKLinesCompare.RemoveAll();
		MerchNodeUserData.m_aKLinesFull.RemoveAll();

		if (NULL != MerchNodeUserData.m_pKLinesShow)
			MerchNodeUserData.m_pKLinesShow->RemoveAll();

		ClearLocalData(false);	// 主线相关数据要清除

		if ( IsSuspendedMerch() )
		{
			CreateSuspendedVirtualNoOpenKlineData();	// 停牌商品填充虚假数据
		}

		return true;
	}

	int32 iEndSrc = CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(pKLineRequest->m_KLines, TimeEnd);

	// 
	CKLine *pKLineSrc = (CKLine *)pKLineRequest->m_KLines.GetData() + iStartSrc;
	int32 iCountSrc = iEndSrc - iStartSrc + 1;

	CKLine *pKLineCmp = (CKLine *)MerchNodeUserData.m_aKLinesCompare.GetData();

	// 清除所有数据
	ClearLocalData(false);

	T_MerchNodeUserData* pData = &MerchNodeUserData;
	pData->m_aKLinesCompare.SetSize(iCountSrc, 100);
	pKLineCmp = (CKLine *)pData->m_aKLinesCompare.GetData();
	if ( iCountSrc > 0 )
	{
		memcpyex(pKLineCmp, pKLineSrc, iCountSrc * sizeof(CKLine));
	}
	int32 iCountSrcOldCmp = iCountSrc;

	pKLineSrc = NULL;
	if ( IsSuspendedMerch() )
	{
		CreateSuspendedVirtualNoOpenCmpKlineData();
		// 重新赋值cmp指针&长度
		// src数据在后面不应当再使用
		pKLineCmp = pData->m_aKLinesCompare.GetData();
		iCountSrc = pData->m_aKLinesCompare.GetSize();
	}

	{
		// 主图更新数据
		if (iCountSrc == 0)
		{
			return true;		// 现在没有数据， 就不处理了
		}

		// 

		// 生成Full数据
		FillTrendNodes(*pData, m_pAbsCenterManager->GetServerTime(), pKLineCmp, iCountSrc);

		// 更新主图下面的指标 - 存在虚假的数据问题，未解决
		for (int32 j = 0; j < pData->aIndexs.GetSize(); j++)
		{
			T_IndexParam* pIndex = pData->aIndexs.GetAt(j);
			g_formula_compute(this, this, (CChartRegion*)pIndex->pRegion, pData, pIndex, 0, pData->m_aKLinesFull.GetSize());
		}

		// 生成显示数据
		if (!pData->UpdateShowData(0, pData->m_aKLinesFull.GetSize()))
		{
			//ASSERT(0);
			return false;
		}

		// 由于只有当天交易日的数据，所以只能在单日下用
		if ( 1 == m_iTrendDayCount && pData->m_pMerchNode->m_aMinuteBS.GetSize() > 0 && pData->m_aKLinesFull.GetSize() > 0 )
		{
			// 
			CGmtTime TimeTodayStart, TimeTodayEnd;
			TimeTodayStart = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time;
			TimeTodayEnd   = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeEnd.m_Time;
			int32 iTodayStart = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pData->m_aKLinesFull, TimeTodayStart);
			int32 iTodayEnd   = CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(pData->m_aKLinesFull, TimeTodayEnd);
			if ( iTodayStart >=0 && iTodayEnd >= iTodayStart && iTodayEnd < pData->m_aKLinesFull.GetSize() )
			{
				TimeTodayStart  = pData->m_aKLinesFull[iTodayStart].m_TimeCurrent;
				TimeTodayEnd    = pData->m_aKLinesFull[iTodayEnd].m_TimeCurrent;
				int32 tStart	= TimeTodayStart.GetTime();
				int32 tEnd		= TimeTodayEnd.GetTime();
				int32 iMinuteBSPos		= 0;
				int32 iMinuteBSCount	= pData->m_pMerchNode->m_aMinuteBS.GetSize();
				const CArray<T_MinuteBS, T_MinuteBS&> &aBSData = pData->m_pMerchNode->m_aMinuteBS;
				while ( iMinuteBSPos < iMinuteBSCount )
				{
					if ( pData->m_pMerchNode->m_aMinuteBS[iMinuteBSPos].m_lTime >= tStart )
					{
						break;
					}
					++iMinuteBSPos;
				}

				if ( iMinuteBSPos < iMinuteBSCount && aBSData[iMinuteBSPos].m_lTime <= tEnd )
				{
					for (int32 j = 0; j < pData->aIndexs.GetSize(); j++)
					{
						T_IndexParam* pIndex = pData->aIndexs.GetAt(j);
						if ( pIndex->strIndexName == CString(_T("VOLFS"))
							&& pIndex->m_aIndexLines.GetSize() > 0 )	// 特殊处理
						{
							int32 iTmpMinutePos = iMinuteBSPos;	
							for ( int32 k=0; k < pIndex->m_aIndexLines.GetSize() ; ++k )
							{
								CNodeSequence *pNodes = (CNodeSequence*)pIndex->m_aIndexLines[k].m_pNodesShow;
								if ( NULL == pNodes )
								{
									continue;
								}

								CArray<CNodeData, CNodeData&> &aNodes = pNodes->GetNodes();
								int32 iNodeStart, iNodeEnd;
								if ( !pNodes->Lookup(tStart, iNodeStart) 
									|| !pNodes->Lookup(tEnd, iNodeEnd)
									|| iNodeStart < 0
									|| iNodeEnd < iNodeStart )
								{
									ASSERT( 0 );
									continue;	// 无法定位区间数据
								}
								// 这里是全天的数据范围，可能bs只有一部分，bs没有的全部设定平价
								ASSERT( iNodeEnd == aNodes.GetSize()-1 );
								int32 m = 0;
								for ( m=iNodeStart; m < aNodes.GetSize() && m <= iNodeEnd ; ++m )
								{
									bool32 bMatch = false;
									if ( iTmpMinutePos < iMinuteBSCount )
									{
										int32 iMBS = aBSData[iTmpMinutePos].m_lTime/60;
										int32 iMNode = aNodes[m].m_iID/60;
										while ( iMNode > iMBS )// node时间在之后, 移动BS节点
										{
											++iTmpMinutePos;
											if ( iTmpMinutePos >= iMinuteBSCount )
											{
												break;	// 跳出移动bs节点
											}
											iMBS = aBSData[iTmpMinutePos].m_lTime/60;
										}
										if ( iMBS > iMNode )
										{
											// node时间在之前,该node置为平价
										}
										else
										{
											bMatch = iTmpMinutePos < iMinuteBSCount;	// 有效匹配了
										}
									}
									if ( bMatch )
									{
										ASSERT( iTmpMinutePos < iMinuteBSCount );
										float fDiff = aBSData[iTmpMinutePos].m_fBuyVolume - aBSData[iTmpMinutePos].m_fSellVolume;
										++iTmpMinutePos;
										if ( fDiff > 0.1f )
										{
											// 内>外，看跌
											aNodes[m].m_bClrValid = true;
											aNodes[m].m_clrNode = (unsigned long)(CNodeData::ENCFColorFall);
										}
										else if ( fDiff < -0.1f )
										{
											// 外>内 看涨
											aNodes[m].m_bClrValid = true;
											aNodes[m].m_clrNode = (unsigned long)(CNodeData::ENCFColorRise);
										}
										else
										{
											aNodes[m].m_bClrValid = true;
											aNodes[m].m_clrNode = (unsigned long)(CNodeData::ENCFColorKeep);
										}
									}
									else
									{
										// 没有bs值，则置平价
										aNodes[m].m_bClrValid = true;
										aNodes[m].m_clrNode = (unsigned long)(CNodeData::ENCFColorKeep);
									}
								}

								for ( ; m < aNodes.GetSize() ; ++m )
								{
									// 剩下的没数据的，全部平价
									aNodes[m].m_bClrValid = true;
									aNodes[m].m_clrNode = (unsigned long)(CNodeData::ENCFColorKeep);
								}
							}//for ( int32 k=0; k < pIndex->m_aIndexLines.GetSize() ; ++k )
						}//if ( pIndex->strIndexName == CString(_T("VOLFS"))
					}//for (int32 j = 0; j < pData->aIndexs.GetSize(); j++)
				}//if ( iMinuteBSPos < iMinuteBSCount && aBSData[iMinuteBSPos].m_lTime <= tEnd )
			}//if ( iTodayStart >=0 && iTodayEnd >= iTodayStart && iTodayEnd < pData->m_aKLinesFull.GetSize() )
		}//if ( 1 == m_iTrendDayCount && pData->m_pMerchNode->m_aMinuteBS.GetSize() > 0 && pData->m_aKLinesFull.GetSize() > 0 )

		// 如果出现部分虚假数据，显示数据中今天的数据必然都是虚假数据
		// 所有虚假数据的指标数据invalid
		// 所有指标都是虚假数据，设置这些数据的flag为invalid
		if ( iCountSrc != iCountSrcOldCmp )
		{
			ASSERT( IsSuspendedMerch() );
			TrimSuspendedIndexData();
		}

		// 重新计算坐标 为什么这里重算坐标会导致 " 0001683 3 【内】[0423] 切换商品时, 隐藏视图更新不及时? " 数据已经清空了啊.
		// UpdateAxisSize();

		// 显示刷新
		m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
		for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
		{
			m_SubRegions[i].m_pSubRegionMain->SetDrawFlag(CRegion::KDrawFull);
		}	

		// 更新副图数据
		// zhangbo 20090714 #待补充...
		//...

		return true;
	}

	return true;
}

bool32 CIoViewTrend::UpdateSubMerchKLine(T_MerchNodeUserData &MerchNodeUserData)
{
	if (m_MerchParamArray.GetSize() <= 0 || m_MerchParamArray[0] == &MerchNodeUserData)
	{
		return false;
	}

	CMerch* pMerch = MerchNodeUserData.m_pMerchNode;
	if (NULL == pMerch)
	{
		return false;
	}

	// zhangbo 20090714 #待处理
	//...
	// 尝试更新走势相关的报价数据

	RemoveCmpMerchFromNeedUpdate(pMerch);

	if (0. == MerchNodeUserData.m_TrendTradingDayInfo.m_fPricePrevClose && 0. == MerchNodeUserData.m_TrendTradingDayInfo.m_fPricePrevAvg)
	{
		float fPricePrevClose = MerchNodeUserData.m_TrendTradingDayInfo.GetPrevReferPrice();
		MerchNodeUserData.m_TrendTradingDayInfo.RecalcPrice(*pMerch);
		if (fPricePrevClose != MerchNodeUserData.m_TrendTradingDayInfo.GetPrevReferPrice())
		{
			// zhangbo 20090708 #待补充, 更新中轴值
			//...		
		}
	}

	// 更新数据
	int32 iPosFound;
	CMerchKLineNode* pKLineRequest = NULL;
	pMerch->FindMerchKLineNode(EKTBMinute, iPosFound, pKLineRequest);
	if (NULL == pKLineRequest || 0 == pKLineRequest->m_KLines.GetSize())	// 根本找不到K线数据， 那就不需要显示了
	{
		MerchNodeUserData.m_aKLinesCompare.RemoveAll();
		MerchNodeUserData.m_aKLinesFull.RemoveAll();

		if (NULL != MerchNodeUserData.m_pKLinesShow)	
			MerchNodeUserData.m_pKLinesShow->RemoveAll();

		MerchNodeUserData.m_iShowPosInFullList = MerchNodeUserData.m_iShowCountInFullList = 0;

		return true;
	}

	// 时间段以主商品的为准
	CGmtTime TimeStart, TimeEnd;
	if ( !GetTrendStartEndTime(TimeStart, TimeEnd) )
	{
		ASSERT( 0 );	// 无效时间段？？
		return false;
	}

	// 获取K线序列中， 用于显示的部分
	int32 iStartSrc	= CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pKLineRequest->m_KLines, TimeStart);
	if (iStartSrc < 0 || iStartSrc >= pKLineRequest->m_KLines.GetSize())
	{
		// 找不到需要显示时间内的分钟K线
		MerchNodeUserData.m_aKLinesCompare.RemoveAll();
		MerchNodeUserData.m_aKLinesFull.RemoveAll();

		if (NULL != MerchNodeUserData.m_pKLinesShow)
			MerchNodeUserData.m_pKLinesShow->RemoveAll();

		MerchNodeUserData.m_iShowPosInFullList = MerchNodeUserData.m_iShowCountInFullList = 0;

		return true;
	}

	int32 iEndSrc = CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(pKLineRequest->m_KLines, TimeEnd);

	// 
	CKLine *pKLineSrc = (CKLine *)pKLineRequest->m_KLines.GetData() + iStartSrc;
	int32 iCountSrc = iEndSrc - iStartSrc + 1;

	CKLine *pKLineCmp = (CKLine *)MerchNodeUserData.m_aKLinesCompare.GetData();
	int32 iCountCmp = MerchNodeUserData.m_aKLinesCompare.GetSize();

	// 仅比较完全相同的情况, 完全相同，则数据没更新
	bool32 bSameCmp = iCountCmp == iCountSrc && 0 == memcmp(pKLineCmp, pKLineSrc, iCountCmp*sizeof(CKLine));

	bool32 bNeedUpdate = false;
	if ( !bSameCmp )
	{
		// 主图更新数据
		if (iCountSrc == 0)
		{
			MerchNodeUserData.m_aKLinesCompare.RemoveAll();
			MerchNodeUserData.m_aKLinesFull.RemoveAll();

			if (NULL != MerchNodeUserData.m_pKLinesShow)
				MerchNodeUserData.m_pKLinesShow->RemoveAll();

			MerchNodeUserData.m_iShowPosInFullList = MerchNodeUserData.m_iShowCountInFullList = 0;
			return true;		// 现在没有数据， 就不处理了
		}

		{
			T_MerchNodeUserData* pData = &MerchNodeUserData;	// 叠加商品更新
			pData->m_aKLinesCompare.SetSize(iCountSrc, 100);

			pKLineCmp = (CKLine *)pData->m_aKLinesCompare.GetData();
			memcpyex(pKLineCmp, pKLineSrc, iCountSrc * sizeof(CKLine));
		}
		bNeedUpdate = true;
	}

	if ( !bNeedUpdate )
	{
		// cmp数据相同，则full数据中比包含足够的信息来显示
		// 使用开始结束时间截取一部分数据
		// 还有可能是切换商品，不完善，取消
		bNeedUpdate = true;	// 总是要生成Full
		// 		CArray<CKLine, CKLine> aTmpKline;
		// 		const int32 iFullSize = MerchNodeUserData.m_aKLinesFull.GetSize();
		// 		int32 iFullStartNew = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(MerchNodeUserData.m_aKLinesFull, TimeStart);
		// 		int32 iFullEndNew = CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(MerchNodeUserData.m_aKLinesFull, TimeEnd);
		// 		if ( iFullStartNew >= 0
		// 			&& iFullEndNew >= iFullStartNew 
		// 			&& iFullEndNew < iFullSize )
		// 		{
		// 			// 截取
		// 			int32 iPatchCount = iFullEndNew - iFullStartNew+1;
		// 			aTmpKline.SetSize(iPatchCount);
		// 			memcpyex(aTmpKline.GetData(), MerchNodeUserData.m_aKLinesFull.GetData()+iFullStartNew, iPatchCount*sizeof(CKLine));
		// 			memcpyex(MerchNodeUserData.m_aKLinesFull.GetData(), aTmpKline.GetData(), iPatchCount*sizeof(CKLine));
		// 			MerchNodeUserData.m_aKLinesFull.SetSize(iPatchCount);
		// 
		// 			// 由于分时设置了标志位，所以标志位也需要更新
		// 			// 完成Full数据生成
		// 		}
		// 		else
		// 		{
		// 			bNeedUpdate = true;	// 必须原始生成
		// 		}
	}

	if ( bNeedUpdate )
	{
		// 生成Full数据
		FillTrendNodes(MerchNodeUserData, m_pAbsCenterManager->GetServerTime(), MerchNodeUserData.m_aKLinesCompare.GetData(), iCountSrc);
	}

	// 生成显示数据
	if (!MerchNodeUserData.UpdateShowData(0, MerchNodeUserData.m_aKLinesFull.GetSize()))
	{
		return false;
	}

	return true;
}

bool32 CIoViewTrend::FillTrendNodes(T_MerchNodeUserData &MerchNodeUserData, const CGmtTime &TimeNow, const CKLine *pKLines, int32 iCountKLine)
{
	ASSERT(NULL != pKLines);

	// 
	MerchNodeUserData.m_aKLinesFull.SetSize(0);

	if (iCountKLine <= 0)
		return true;

	// 
	if (!MerchNodeUserData.m_TrendTradingDayInfo.m_bInit)
	{
		return false;
	}

	// 
	int32 iMaxTrendUnitCount = 0;
	{
		iMaxTrendUnitCount = 0;
		for ( int32 iDay = m_aTrendMultiDayIOCTimes.GetUpperBound(); iDay >= 0 ; iDay-- )
		{
			iMaxTrendUnitCount += m_aTrendMultiDayIOCTimes[iDay].GetMaxTrendUnitCount();
		}
	}
	if ( iMaxTrendUnitCount <= 0 )
	{
		iMaxTrendUnitCount = MerchNodeUserData.m_TrendTradingDayInfo.m_MarketIOCTime.GetMaxTrendUnitCount();
	}
	ASSERT(iMaxTrendUnitCount > 0);

	float fPricePrev = MerchNodeUserData.m_TrendTradingDayInfo.GetPrevReferPrice();
	if ( MerchNodeUserData.bMainMerch )
	{
		fPricePrev = GetTrendPrevClose();
	}
	// 	if ( fPricePrev == 0.0f )
	// 	{
	// 		ASSERT( 0 );	// 不支持0价格商品
	// 		return false;
	// 	}

	// 先初始化第一笔数据
	MerchNodeUserData.m_aKLinesFull.SetSize(iMaxTrendUnitCount + 20);
	MerchNodeUserData.m_aKLinesShowDataFlag.SetSize(iMaxTrendUnitCount + 20);
	CKLine *pKLineFull = (CKLine *)MerchNodeUserData.m_aKLinesFull.GetData();
	int iCountNode = 0;
	DWORD *pFlags = (DWORD *)MerchNodeUserData.m_aKLinesShowDataFlag.GetData();
	ZeroMemory(pFlags, (iMaxTrendUnitCount+20)*sizeof(DWORD));

	pKLineFull[iCountNode].m_TimeCurrent	= MerchNodeUserData.m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeOpen.m_Time.GetTime();


	pKLineFull[iCountNode].m_fPriceOpen		= fPricePrev;
	pKLineFull[iCountNode].m_fPriceClose	= fPricePrev;
	pKLineFull[iCountNode].m_fPriceAvg		= fPricePrev;
	pKLineFull[iCountNode].m_fPriceHigh		= fPricePrev;
	pKLineFull[iCountNode].m_fPriceLow		= fPricePrev;

	pKLineFull[iCountNode].m_fVolume		= 0.;
	pKLineFull[iCountNode].m_fAmount		= 0.;
	// 	pKLineFull[iCountNode].m_fHold			= 0.;
	pKLineFull[iCountNode].m_fHold			= MerchNodeUserData.m_TrendTradingDayInfo.GetPrevHold();	// 取昨天持仓
	bool32 bFirstNodeHold = true;
	if ( pKLineFull[iCountNode].m_fHold <= 0.0 )
	{
		// 此时已经出错了，换用下一个有效的数值作为虚假数据
		bFirstNodeHold = false;
		pKLineFull[iCountNode].m_fHold = pKLines[iCountNode].m_fHold;
	}

	bool32 bFoundFirstNodeData = false;		// 是否到了第一个有效数据

	// 逐个分段处理
	int32 iKLineIndex = 0;

	if ( m_bSingleSectionTrend )
	{
		CGmtTime TimeOpen, TimeClose;
		if ( !GetTrendSingleSectionOCTime(TimeOpen, TimeClose) )
		{
			ASSERT( 0 );
		}
		else
		{
			uint32 uiTime = TimeOpen.GetTime();
			for (uiTime = TimeOpen.GetTime(); uiTime <= TimeClose.GetTime() && uiTime <= TimeNow.GetTime(); uiTime += 60)
			{
				if (iKLineIndex < iCountKLine)
				{
					if (pKLines[iKLineIndex].m_TimeCurrent.GetTime() < uiTime)
					{
						ASSERT(MerchNodeUserData.m_pMerchNode != m_pMerchXml);		// 叠加商品可能出现,不应该出现这种情况
						while ( pKLines[iKLineIndex].m_TimeCurrent.GetTime() < uiTime && iKLineIndex < iCountKLine )
						{
							iKLineIndex++;
						}
						if ( iKLineIndex >= iCountKLine )
						{
							continue;
						}
					}

					if (pKLines[iKLineIndex].m_TimeCurrent.GetTime() == uiTime)
					{
						if ( 0 == iCountNode )
						{
							bFirstNodeHold = true;
						}
						memcpyex(&pKLineFull[iCountNode], &pKLines[iKLineIndex], sizeof(CKLine));	// 一对一赋值
						bFoundFirstNodeData = true;	// 设置现在已经 有正常的数据了

						//
						iCountNode++;
						iKLineIndex++;
					}
					else // 延续上一个值
					{
						ASSERT( pKLines[iKLineIndex].m_TimeCurrent.GetTime() >= uiTime );
						if (iCountNode > 0)
						{
							memcpyex(&pKLineFull[iCountNode], &pKLineFull[iCountNode - 1], sizeof(CKLine));
							pKLineFull[iCountNode].m_TimeCurrent= uiTime;
							pKLineFull[iCountNode].m_fVolume	= 0.;
							pKLineFull[iCountNode].m_fAmount	= 0.;
						}
						iCountNode++;
					}
				}
				else	// 延续上一个值
				{
					if (iCountNode > 0)
					{
						memcpyex(&pKLineFull[iCountNode], &pKLineFull[iCountNode - 1], sizeof(CKLine));
						pKLineFull[iCountNode].m_TimeCurrent= uiTime;
						pKLineFull[iCountNode].m_fVolume	= 0.;
						pKLineFull[iCountNode].m_fAmount	= 0.;
					}
					iCountNode++;
				}
			}
		}
	}
	else
	{
		// 完整数据
		int32 iLastDayPos = -1;
		for ( int32 iDay = m_aTrendMultiDayIOCTimes.GetUpperBound(); iDay >= 0 ; iDay-- )
		{
			const CMarketIOCTimeInfo &IOCTime = m_aTrendMultiDayIOCTimes[iDay];
			//for (int32 iIndexOC = 0; iIndexOC < MerchNodeUserData.m_TrendTradingDayInfo.m_MarketIOCTime.m_aOCTimes.GetSize(); iIndexOC += 2)
			for (int32 iIndexOC = 0; iIndexOC < IOCTime.m_aOCTimes.GetSize(); iIndexOC += 2)
			{
				CGmtTime TimeOpen =  IOCTime.m_aOCTimes[iIndexOC];
				CGmtTime TimeClose =  IOCTime.m_aOCTimes[iIndexOC + 1];

				uint32 uiTime = TimeOpen.GetTime();
				for (uiTime = TimeOpen.GetTime(); uiTime <= TimeClose.GetTime() && uiTime <= TimeNow.GetTime(); uiTime += 60)
				{
					if (iKLineIndex < iCountKLine)
					{
						if (pKLines[iKLineIndex].m_TimeCurrent.GetTime() < uiTime)
						{
							//ASSERT(!MerchNodeUserData.bMainMerch || MerchNodeUserData.m_pMerchNode != m_pMerchXml);		// 叠加商品可能出现,不应该出现这种情况
							while ( pKLines[iKLineIndex].m_TimeCurrent.GetTime() < uiTime && iKLineIndex < iCountKLine )
							{
								iKLineIndex++;
							}
							if ( iKLineIndex >= iCountKLine )
							{
								continue;
							}
						}

						if (pKLines[iKLineIndex].m_TimeCurrent.GetTime() == uiTime)
						{
							if ( 0 == iCountNode )
							{
								bFirstNodeHold = true;
							}
							memcpyex(&pKLineFull[iCountNode], &pKLines[iKLineIndex], sizeof(CKLine));	// 一对一赋值
							bFoundFirstNodeData = true;	// 设置现在已经 有正常的数据了

							//
							iCountNode++;
							iKLineIndex++;
						}
						else // 延续上一个值
						{
							ASSERT( pKLines[iKLineIndex].m_TimeCurrent.GetTime() >= uiTime );
							if (iCountNode > 0)
							{
								if ( iLastDayPos>=0 && iLastDayPos == iCountNode-1 )
								{
									// 换天的数据，仅修改昨收价&均价，其它价格全部为0
									CKLine kline;
									kline.m_fPriceAvg = kline.m_fPriceClose = pKLineFull[iCountNode-1].m_fPriceClose;
									memcpyex(&pKLineFull[iCountNode], &kline, sizeof(CKLine));
								}
								else
								{
									// 完整继承上一个值
									memcpyex(&pKLineFull[iCountNode], &pKLineFull[iCountNode - 1], sizeof(CKLine));
								}
								pKLineFull[iCountNode].m_TimeCurrent= uiTime;
								pKLineFull[iCountNode].m_fVolume	= 0.;
								pKLineFull[iCountNode].m_fAmount	= 0.;
							}
							else
							{
								pKLineFull[iCountNode].m_TimeCurrent= uiTime;
							}
							if ( m_aTrendMultiDayIOCTimes.GetSize() > 1 && !bFoundFirstNodeData )
							{
								// 在多日分时下，第一个有效数据前的所有数据都是非法节点
								pFlags[iCountNode] = CNodeData::KValueInvalid;	// 值非法
							}
							iCountNode++;
						}
					}
					else	// 延续上一个值
					{
						if (iCountNode > 0)
						{
							if ( iLastDayPos>=0 && iLastDayPos == iCountNode-1 )
							{
								// 换天的数据，仅修改昨收价&均价，其它价格全部为0
								CKLine kline;
								kline.m_fPriceAvg = kline.m_fPriceClose = pKLineFull[iCountNode-1].m_fPriceClose;
								memcpyex(&pKLineFull[iCountNode], &kline, sizeof(CKLine));
							}
							else
							{
								// 完整继承上一个值
								memcpyex(&pKLineFull[iCountNode], &pKLineFull[iCountNode - 1], sizeof(CKLine));
							}
							pKLineFull[iCountNode].m_TimeCurrent= uiTime;
							pKLineFull[iCountNode].m_fVolume	= 0.;
							pKLineFull[iCountNode].m_fAmount	= 0.;
						}
						if ( m_aTrendMultiDayIOCTimes.GetSize() > 1 && !bFoundFirstNodeData )
						{
							// 在多日分时下，第一个有效数据前的所有数据都是非法节点
							pFlags[iCountNode] = CNodeData::KValueInvalid;	// 值非法
						}
						iCountNode++;
					}
				}
			}

			// 多日分时下，每日的最后一个节点与下一天的节点不连续
			if ( iCountNode > 0 && m_aTrendMultiDayIOCTimes.GetSize() > 1 && bFoundFirstNodeData && iDay != 0 )
			{
				iLastDayPos = iCountNode-1;
				pFlags[iCountNode-1] |= CNodeData::KValueDivide;
			}
		}
	}


	if ( !bFirstNodeHold )
	{
		TRACE(_T("分时线昨持仓数据可能会不能正确赋值!\r\n"));
	}

	// 修正长度
	MerchNodeUserData.m_aKLinesFull.SetSize(iCountNode);
	MerchNodeUserData.m_aKLinesShowDataFlag.SetSize(iCountNode);
	return true;
}

bool32 CIoViewTrend::CalcMainRegionXAxis(CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aAxisDivide)
{
	ASSERT(NULL != m_pRegionMain);

	aAxisNodes.SetSize(0);
	aAxisDivide.SetSize(0);

	// 
	T_MerchNodeUserData* pData = NULL;
	if (m_MerchParamArray.GetSize() > 0)
	{
		pData = m_MerchParamArray.GetAt(0);
	}

	if (NULL == pData)
		return false;

	// 
	CTrendTradingDayInfo &TrendTradingDay = pData->m_TrendTradingDayInfo;
	if (!TrendTradingDay.m_bInit)
	{
		//ASSERT(0);
		return false;
	}

	const int32 kiSeperatorValve = 8*60-2;		// >此数据线的就按照1小时分割点算
	int32 kiTimeSeperator = 1800;				// 默认半小时分割点

	// 
	CRect Rect = m_pRegionMain->GetRectCurves();
	Rect.DeflateRect(1, 1, 1, 1);
	if ( m_bSingleSectionTrend )
	{
		// 半场走势
		int32 iMaxTrendUnitCount = TrendTradingDay.m_MarketIOCTime.GetMaxTrendUnitCount();
		CGmtTime TimeOpen, TimeClose;
		if ( !GetTrendSingleSectionOCTime(TimeOpen, TimeClose) )
		{
			ASSERT( 0 ); // 不知道怎么分配，默认显示了
		}
		else
		{
			iMaxTrendUnitCount = ((TimeClose.GetTime() - TimeOpen.GetTime()) / 60 + 1);
		}

		int32 iHalfTimeSeperator = kiTimeSeperator;	// 默认半小时分割线
		if ( iMaxTrendUnitCount > kiSeperatorValve )
		{
			int i2Multi = (1+iMaxTrendUnitCount/kiSeperatorValve);
			i2Multi = i2Multi - i2Multi%2;
			iHalfTimeSeperator *= i2Multi;
		}
		int32 iFullTimeSeperator = iHalfTimeSeperator*2;	// 整点分割点

		ASSERT( iMaxTrendUnitCount > 1 );
		if ( iMaxTrendUnitCount <= 1 )
		{
			iMaxTrendUnitCount	=	2;
		}
		float fPixelWidthPerUnit = Rect.Width() / (float)(iMaxTrendUnitCount - 1);

		//
		aAxisNodes.SetSize(iMaxTrendUnitCount);

		int32 iMaxAxisDivideSize = iMaxTrendUnitCount / (20);
		if (iMaxAxisDivideSize < 20)	iMaxAxisDivideSize = 20;
		aAxisDivide.SetSize(iMaxAxisDivideSize);

		//
		CAxisNode *pAxisNode = (CAxisNode *)aAxisNodes.GetData();
		CAxisDivide *pAxisDivide = (CAxisDivide *)aAxisDivide.GetData();

		int32 iIndexNode = 0, iIndexDivide = 0;
		time_t uiTime = 0;
		for (uiTime = TimeOpen.GetTime(); uiTime <= TimeClose.GetTime(); uiTime += 60)
		{
			CAxisNode &AxisNode = pAxisNode[iIndexNode];
			CAxisDivide &AxisDivide = pAxisDivide[iIndexDivide];
			AxisNode.m_iTimeId = uiTime;		// 时间节点

			/////////////////////////////////////////////////////////////////////////
			// 节点
			CTime Time(uiTime);	// 总是当地时间显示
			// ...fangz0815 暂时改动. 为了X 轴滑块显示的时候不要显示月份和日期
			// AxisNode.m_StrSliderText1.Format(L"%02d/%02d %02d:%02d", Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute());
			AxisNode.m_StrSliderText1.Format(L"%02d:%02d", Time.GetHour(), Time.GetMinute());

			// 节点左 
			if (0 == iIndexNode)
			{
				AxisNode.m_iStartPixel = Rect.left;
			}
			else
			{
				AxisNode.m_iStartPixel = pAxisNode[iIndexNode - 1].m_iEndPixel + 1;
			}

			// 节点右
			if (iMaxTrendUnitCount - 1 == iIndexNode)
			{
				AxisNode.m_iEndPixel = Rect.right;		// xl 0623 由于右边有一个边框，所以现在不画这个线
			}
			else
			{
				AxisNode.m_iEndPixel = Rect.left + (int32)((iIndexNode + 0.5) * fPixelWidthPerUnit + 0.5);
			}

			//
			if (AxisNode.m_iStartPixel > AxisNode.m_iEndPixel)
				AxisNode.m_iStartPixel = AxisNode.m_iEndPixel;

			// 中节点
			if (0 == iIndexNode)
				AxisNode.m_iCenterPixel = AxisNode.m_iStartPixel;
			else if (iMaxTrendUnitCount - 1 == iIndexNode)
				AxisNode.m_iCenterPixel = AxisNode.m_iEndPixel;
			else
				AxisNode.m_iCenterPixel = (AxisNode.m_iStartPixel + AxisNode.m_iEndPixel) / 2;

			iIndexNode++;

			//////////////////////////////////////////////////////////
			// 分割线
			if (uiTime == TimeOpen.GetTime())	// 每个小交易段的起始时间
			{
				if( uiTime % 1800 != 0 )
				{
					continue;
				}

				AxisDivide.m_iPosPixel		= AxisNode.m_iCenterPixel;

				//
				AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;
				AxisDivide.m_eLineStyle = CAxisDivide::ELSSolid;

				//
				CTime tmpTime(uiTime);
				AxisDivide.m_DivideText1.m_StrText.Format(L"%02d:%02d", tmpTime.GetHour(), tmpTime.GetMinute());
				AxisDivide.m_eLevelText		= CAxisDivide::EDLLevel1;
				AxisDivide.m_DivideText1.m_uiTextAlign = DT_LEFT | DT_VCENTER | DT_SINGLELINE;

				AxisDivide.m_DivideText1.m_lTextColor = m_pRegionMain->GetAxisTextColor();
				AxisDivide.m_DivideText2 = AxisDivide.m_DivideText1;

				// 
				iIndexDivide++;
			}
			else if (uiTime == TimeClose.GetTime())	// 该交易日收盘时间
			{
				AxisDivide.m_iPosPixel		= AxisNode.m_iCenterPixel;

				//
				AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;
				AxisDivide.m_eLineStyle = CAxisDivide::ELSSolid;

				//
				AxisDivide.m_eLevelText		= CAxisDivide::EDLLevel1;
				AxisDivide.m_DivideText1.m_StrText.Format(L"%02d:%02d", Time.GetHour(), Time.GetMinute());
				AxisDivide.m_DivideText1.m_uiTextAlign = DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
				AxisDivide.m_DivideText1.m_lTextColor = m_pRegionMain->GetAxisTextColor();

				AxisDivide.m_DivideText2 = AxisDivide.m_DivideText1;

				//
				iIndexDivide++;
			}
			else if (uiTime % (iFullTimeSeperator) == 0)		// 一小时
			{
				AxisDivide.m_iPosPixel		= AxisNode.m_iCenterPixel;

				//
				AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;
				AxisDivide.m_eLineStyle = CAxisDivide::ELSSolid;

				//
				AxisDivide.m_eLevelText		= CAxisDivide::EDLLevel3;
				AxisDivide.m_DivideText1.m_StrText.Format(L"%02d:%02d", Time.GetHour(), Time.GetMinute());
				AxisDivide.m_DivideText1.m_uiTextAlign = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
				AxisDivide.m_DivideText1.m_lTextColor = m_pRegionMain->GetAxisTextColor();

				AxisDivide.m_DivideText2 = AxisDivide.m_DivideText1;

				//
				iIndexDivide++;
			}
			else if (uiTime % iHalfTimeSeperator == 0)		// 半小时
			{
				AxisDivide.m_iPosPixel		= AxisNode.m_iCenterPixel;

				//
				AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;
				AxisDivide.m_eLineStyle = CAxisDivide::ELSDot;

				//
				AxisDivide.m_eLevelText		= CAxisDivide::EDLLevel4;
				AxisDivide.m_DivideText1.m_StrText.Format(L"%02d:%02d", Time.GetHour(), Time.GetMinute());
				AxisDivide.m_DivideText1.m_uiTextAlign = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
				AxisDivide.m_DivideText1.m_lTextColor = m_pRegionMain->GetAxisTextColor();

				AxisDivide.m_DivideText2 = AxisDivide.m_DivideText1;

				//
				iIndexDivide++;
			}
		}


		//  
		aAxisNodes.SetSize(iIndexNode);
		aAxisDivide.SetSize(iIndexDivide);
	}
	else if ( m_aTrendMultiDayIOCTimes.GetSize() > 1 )
	{
		// 多日
		int32 iMaxTrendUnitCount = TrendTradingDay.m_MarketIOCTime.GetMaxTrendUnitCount();
		int32 iMaxTrendIOCCount = 0;
		{
			iMaxTrendUnitCount = 0;
			for ( int32 iDay = m_aTrendMultiDayIOCTimes.GetUpperBound(); iDay >= 0 ; iDay-- )
			{
				iMaxTrendUnitCount += m_aTrendMultiDayIOCTimes[iDay].GetMaxTrendUnitCount();
				iMaxTrendIOCCount += m_aTrendMultiDayIOCTimes[iDay].m_aOCTimes.GetSize()/2;
			}
		}

		ASSERT( iMaxTrendUnitCount > 1 );
		if ( iMaxTrendUnitCount <= 1 )
		{
			iMaxTrendUnitCount	=	2;
		}
		float fPixelWidthPerUnit = Rect.Width() / (float)(iMaxTrendUnitCount - 1);

		//
		aAxisNodes.SetSize(iMaxTrendUnitCount);

		// 分割点变成以天作为大标准, 开收盘为小标准
		// 如果区间不多，则考虑使用小时点作为分割点
		int32 iMaxAxisDivideSize = iMaxTrendUnitCount / (20);
		if (iMaxAxisDivideSize < 20)	iMaxAxisDivideSize = 20;
		aAxisDivide.SetSize(iMaxAxisDivideSize);
		bool32 bHourIsDivide = iMaxTrendIOCCount < 8; // 开收盘时间段个数小于8, 则距离开盘n小时则视为一个分割点
		bool32 bShowOCText = m_aTrendMultiDayIOCTimes.GetSize() < 8;

		//
		CAxisNode *pAxisNode = (CAxisNode *)aAxisNodes.GetData();
		CAxisDivide *pAxisDivide = (CAxisDivide *)aAxisDivide.GetData();

		int32 iIndexNode = 0, iIndexDivide = 0;
		for ( int32 iDay=m_aTrendMultiDayIOCTimes.GetUpperBound(); iDay >= 0; iDay-- )
		{
			const CMarketIOCTimeInfo &IOCTime = m_aTrendMultiDayIOCTimes[iDay];

			for (int32 iIndexOC = 0; iIndexOC < IOCTime.m_aOCTimes.GetSize(); iIndexOC += 2)
			{
				CGmtTime TimeOpen =  IOCTime.m_aOCTimes[iIndexOC];
				CGmtTime TimeClose =  IOCTime.m_aOCTimes[iIndexOC + 1];

				uint32 uiTime = TimeOpen.GetTime();

				CTime Time1(uiTime);	// 总是当地时间显示
				CTime Time2(TimeClose.GetTime());

				CString StrTimeOpen;
				CString StrTimeClose;

				StrTimeOpen.Format(L"%02d:%02d", Time1.GetHour(), Time1.GetMinute());
				StrTimeClose.Format(L"%02d:%02d", Time2.GetHour(), Time2.GetMinute());

				for (uiTime = TimeOpen.GetTime(); uiTime <= TimeClose.GetTime(); uiTime += 60)
				{
					CAxisNode &AxisNode = pAxisNode[iIndexNode];
					AxisNode.m_iTimeId = uiTime;		// 时间节点

					/////////////////////////////////////////////////////////////////////////
					// 节点
					CTime Time(uiTime);	// 总是当地时间显示
					// ...fangz0815 暂时改动. 为了X 轴滑块显示的时候不要显示月份和日期
					// AxisNode.m_StrSliderText1.Format(L"%02d/%02d %02d:%02d", Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute());
					AxisNode.m_StrSliderText1.Format(L"%02d:%02d", Time.GetHour(), Time.GetMinute());

					// 节点左 
					if (0 == iIndexNode)
					{
						AxisNode.m_iStartPixel = Rect.left;
					}
					else
					{
						AxisNode.m_iStartPixel = pAxisNode[iIndexNode - 1].m_iEndPixel + 1;
					}

					// 节点右
					if (iMaxTrendUnitCount - 1 == iIndexNode)
					{
						AxisNode.m_iEndPixel = Rect.right;		// xl 0623 由于右边有一个边框，所以现在不画这个线
					}
					else
					{
						AxisNode.m_iEndPixel = Rect.left + (int32)((iIndexNode + 0.5) * fPixelWidthPerUnit + 0.5);
					}

					//
					if (AxisNode.m_iStartPixel > AxisNode.m_iEndPixel)
						AxisNode.m_iStartPixel = AxisNode.m_iEndPixel;

					// 中节点
					if (0 == iIndexNode)
						AxisNode.m_iCenterPixel = AxisNode.m_iStartPixel;
					else if (iMaxTrendUnitCount - 1 == iIndexNode)
						AxisNode.m_iCenterPixel = AxisNode.m_iEndPixel;
					else
						AxisNode.m_iCenterPixel = (AxisNode.m_iStartPixel + AxisNode.m_iEndPixel) / 2;

					iIndexNode++;

					if ( uiTime == TimeOpen.GetTime() )	// 开盘点
					{
						if ( iIndexOC == 0 )
						{
							// 隔天点 - 总是实线, 显示日期
							pAxisDivide->m_iPosPixel		= AxisNode.m_iCenterPixel;	

							pAxisDivide->m_eLevelLine = CAxisDivide::EDLLevel1;
							pAxisDivide->m_eLineStyle = CAxisDivide::ELSSolid;

							//
							CTime curTmpTime(uiTime);
							if (0 == iDay)
							{
								pAxisDivide->m_DivideText1.m_StrText.Format(L"%02d/%02d", curTmpTime.GetMonth(), curTmpTime.GetDay());
							}
							else
							{
								pAxisDivide->m_DivideText1.m_StrText.Format(L"%02d/%02d  [前%d 天]", curTmpTime.GetMonth(), curTmpTime.GetDay(), iDay);
							}

							if ( iDay == m_aTrendMultiDayIOCTimes.GetSize()-1 )	// 第一个天数点
							{
								pAxisDivide->m_eLevelText		= CAxisDivide::EDLLevel1;
								pAxisDivide->m_DivideText1.m_uiTextAlign = DT_LEFT | DT_VCENTER | DT_SINGLELINE;
							}
							else	// 其余的天数点
							{				
								pAxisDivide->m_eLevelText		= CAxisDivide::EDLLevel2;
								pAxisDivide->m_DivideText1.m_uiTextAlign = DT_LEFT| DT_VCENTER | DT_SINGLELINE;
							}

							pAxisDivide->m_DivideText1.m_lTextColor = m_pRegionMain->GetAxisTextColor();
							pAxisDivide->m_DivideText2 = pAxisDivide->m_DivideText1;

							// 
							iIndexDivide++;
							pAxisDivide++;		// 递增到下个节点
						}
						else
						{
							// 天中小开盘点 - 小时线下为实，非小时线为虚, 显示时间
							pAxisDivide->m_iPosPixel		= AxisNode.m_iCenterPixel;	

							pAxisDivide->m_eLevelLine = CAxisDivide::EDLLevel1;
							if ( bHourIsDivide )
							{
								pAxisDivide->m_eLineStyle = CAxisDivide::ELSSolid;
							}
							else
							{
								pAxisDivide->m_eLineStyle = CAxisDivide::ELSDot;
							}

							//
							CTime cTmpTime(uiTime);
							pAxisDivide->m_eLevelText		= CAxisDivide::EDLLevel4;
							if ( bShowOCText )
							{
								pAxisDivide->m_DivideText1.m_StrText.Format(L"%02d:%02d", cTmpTime.GetHour(), cTmpTime.GetMinute());
							}
							pAxisDivide->m_DivideText1.m_uiTextAlign = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
							pAxisDivide->m_DivideText1.m_lTextColor = m_pRegionMain->GetAxisTextColor();

							pAxisDivide->m_DivideText2 = pAxisDivide->m_DivideText1;

							//
							iIndexDivide++;
							pAxisDivide++;
						}
					}
					else if ( bHourIsDivide && (uiTime - TimeOpen.GetTime())%3600 == 0 )
					{
						// 距离开盘整点, 虚，显示时间
						pAxisDivide->m_iPosPixel		= AxisNode.m_iCenterPixel;	

						pAxisDivide->m_eLevelLine = CAxisDivide::EDLLevel1;
						pAxisDivide->m_eLineStyle = CAxisDivide::ELSDot;

						//
						CTime tpTime(uiTime);
						pAxisDivide->m_eLevelText		= CAxisDivide::EDLLevel4;
						//pAxisDivide->m_DivideText1.m_StrText.Format(L"%02d:%02d", Time.GetHour(), Time.GetMinute());
						pAxisDivide->m_DivideText1.m_uiTextAlign = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
						pAxisDivide->m_DivideText1.m_lTextColor = m_pRegionMain->GetAxisTextColor();

						pAxisDivide->m_DivideText2 = pAxisDivide->m_DivideText1;

						//
						iIndexDivide++;
						pAxisDivide++;
					}
					else if ( 0 == iDay && uiTime == TimeClose.GetTime() && iIndexOC == IOCTime.m_aOCTimes.GetSize()-2 )
					{
						// 最后一根收盘线 实，显示时间
						pAxisDivide->m_iPosPixel		= AxisNode.m_iCenterPixel;	

						pAxisDivide->m_eLevelLine = CAxisDivide::EDLLevel1;
						pAxisDivide->m_eLineStyle = CAxisDivide::ELSSolid;

						//
						CTime tmpTime(uiTime);
						pAxisDivide->m_DivideText1.m_StrText.Format(L"%02d:%02d", tmpTime.GetHour(), tmpTime.GetMinute());
						pAxisDivide->m_eLevelText		= CAxisDivide::EDLLevel1;
						pAxisDivide->m_DivideText1.m_uiTextAlign = DT_RIGHT | DT_VCENTER | DT_SINGLELINE;

						pAxisDivide->m_DivideText1.m_lTextColor = m_pRegionMain->GetAxisTextColor();
						pAxisDivide->m_DivideText2 = pAxisDivide->m_DivideText1;

						// 
						iIndexDivide++;
						pAxisDivide++;		// 递增到下个节点
					}
				}
			}
		}


		//  
		aAxisNodes.SetSize(iIndexNode);
		aAxisDivide.SetSize(iIndexDivide);
	}
	else	// 普通状态下
	{
		int32 iMaxTrendUnitCount = TrendTradingDay.m_MarketIOCTime.GetMaxTrendUnitCount();
		bool32 bShowFirstOpen = IsShowTrendFirstOpenPrice();

		ASSERT( iMaxTrendUnitCount > 1 );
		if ( iMaxTrendUnitCount <= 1 )
		{
			iMaxTrendUnitCount	=	2;
		}
		float fPixelWidthPerUnit = Rect.Width() / (float)(iMaxTrendUnitCount - 1);
		if ( bShowFirstOpen )
		{
			fPixelWidthPerUnit = Rect.Width()/(float)iMaxTrendUnitCount;
			Rect.left += (int32)fPixelWidthPerUnit;	// 左侧偏移一个小空格给第一个开盘价
		}

		int32 iHalfTimeSeperator = kiTimeSeperator;	// 默认半小时分割线
		if ( iMaxTrendUnitCount > kiSeperatorValve )
		{
			int i2Multi = (1+iMaxTrendUnitCount/kiSeperatorValve);
			i2Multi = i2Multi - i2Multi%2;
			iHalfTimeSeperator *= i2Multi;
		}
		int32 iFullTimeSeperator = iHalfTimeSeperator*2;	// 整点分割点

		//
		aAxisNodes.SetSize(iMaxTrendUnitCount);

		int32 iMaxAxisDivideSize = iMaxTrendUnitCount / 20;
		if (iMaxAxisDivideSize < 20)	iMaxAxisDivideSize = 20;
		aAxisDivide.SetSize(iMaxAxisDivideSize);	

		//
		CAxisNode *pAxisNode = (CAxisNode *)aAxisNodes.GetData();
		CAxisDivide *pAxisDivide = (CAxisDivide *)aAxisDivide.GetData();

		int32 iIndexNode = 0, iIndexDivide = 0;

		for (int32 iIndexOC = 0; iIndexOC < TrendTradingDay.m_MarketIOCTime.m_aOCTimes.GetSize(); iIndexOC += 2)
		{
			CGmtTime TimeOpen =  TrendTradingDay.m_MarketIOCTime.m_aOCTimes[iIndexOC];
			CGmtTime TimeClose =  TrendTradingDay.m_MarketIOCTime.m_aOCTimes[iIndexOC + 1];

			uint32 uiTime = TimeOpen.GetTime();

			CTime Time1(uiTime);	// 总是当地时间显示
			CTime Time2(TimeClose.GetTime());

			CString StrTimeOpen;
			CString StrTimeClose;

			StrTimeOpen.Format(L"%02d:%02d", Time1.GetHour(), Time1.GetMinute());
			StrTimeClose.Format(L"%02d:%02d", Time2.GetHour(), Time2.GetMinute());

			for (uiTime = TimeOpen.GetTime(); uiTime <= TimeClose.GetTime(); uiTime += 60)
			{
				CAxisNode &AxisNode = pAxisNode[iIndexNode];
				CAxisDivide &AxisDivide = pAxisDivide[iIndexDivide];
				AxisNode.m_iTimeId = uiTime;		// 时间节点

				/////////////////////////////////////////////////////////////////////////
				// 节点
				CTime Time(uiTime);	// 总是当地时间显示
				// ...fangz0815 暂时改动. 为了X 轴滑块显示的时候不要显示月份和日期
				// AxisNode.m_StrSliderText1.Format(L"%02d/%02d %02d:%02d", Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute());
				AxisNode.m_StrSliderText1.Format(L"%02d:%02d", Time.GetHour(), Time.GetMinute());

				// 节点左 
				if (0 == iIndexNode)
				{
					AxisNode.m_iStartPixel = Rect.left;
				}
				else
				{
					AxisNode.m_iStartPixel = pAxisNode[iIndexNode - 1].m_iEndPixel + 1;
				}

				// 节点右
				if (iMaxTrendUnitCount - 1 == iIndexNode)
				{
					AxisNode.m_iEndPixel = Rect.right;		// xl 0623 由于右边有一个边框，所以现在不画这个线
				}
				else
				{
					AxisNode.m_iEndPixel = Rect.left + (int32)((iIndexNode + 0.5) * fPixelWidthPerUnit + 0.5);
				}

				//
				if (AxisNode.m_iStartPixel > AxisNode.m_iEndPixel)
					AxisNode.m_iStartPixel = AxisNode.m_iEndPixel;

				// 中节点, 在显示开盘价情况下，该点是不会成立的
				if (0 == iIndexNode)
					AxisNode.m_iCenterPixel = AxisNode.m_iStartPixel;
				else if (iMaxTrendUnitCount - 1 == iIndexNode)
					AxisNode.m_iCenterPixel = AxisNode.m_iEndPixel;
				else
					AxisNode.m_iCenterPixel = (AxisNode.m_iStartPixel + AxisNode.m_iEndPixel) / 2;

				iIndexNode++;

				//////////////////////////////////////////////////////////
				// 分割线
				if (uiTime == TimeOpen.GetTime())	// 每个小交易段的起始时间
				{
					if( uiTime % 1800 != 0 )
					{
						continue;
					}

					AxisDivide.m_iPosPixel		= AxisNode.m_iCenterPixel;

					//
					AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;
					if (0 == iIndexOC)
						AxisDivide.m_eLineStyle = CAxisDivide::ELSSolid;
					else
						//AxisDivide.m_eLineStyle = CAxisDivide::ELSDoubleSolid;
						AxisDivide.m_eLineStyle = CAxisDivide::ELSSolid;

					//
					CTime tmpTime(uiTime);
					AxisDivide.m_DivideText1.m_StrText.Format(L"%02d:%02d", tmpTime.GetHour(), tmpTime.GetMinute());
					if (0 == iIndexOC)	// 该交易日开盘时间
					{
						AxisDivide.m_eLevelText		= CAxisDivide::EDLLevel1;
						AxisDivide.m_DivideText1.m_uiTextAlign = DT_LEFT | DT_VCENTER | DT_SINGLELINE;
					}
					else	// 一个小节的开盘时间
					{				
						AxisDivide.m_eLevelText		= CAxisDivide::EDLLevel2;
						AxisDivide.m_DivideText1.m_uiTextAlign = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
					}

					AxisDivide.m_DivideText1.m_lTextColor = m_pRegionMain->GetAxisTextColor();
					AxisDivide.m_DivideText2 = AxisDivide.m_DivideText1;

					// 
					iIndexDivide++;
				}
				else if (uiTime == TimeClose.GetTime() && iIndexOC == TrendTradingDay.m_MarketIOCTime.m_aOCTimes.GetSize() - 2)	// 该交易日收盘时间
				{
					AxisDivide.m_iPosPixel		= AxisNode.m_iCenterPixel;

					//
					AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;
					AxisDivide.m_eLineStyle = CAxisDivide::ELSSolid;

					//
					AxisDivide.m_eLevelText		= CAxisDivide::EDLLevel1;
					AxisDivide.m_DivideText1.m_StrText.Format(L"%02d:%02d", Time.GetHour(), Time.GetMinute());
					AxisDivide.m_DivideText1.m_uiTextAlign = DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
					AxisDivide.m_DivideText1.m_lTextColor = m_pRegionMain->GetAxisTextColor();

					AxisDivide.m_DivideText2 = AxisDivide.m_DivideText1;

					//
					iIndexDivide++;
				}
				else if (((int32)uiTime) % iFullTimeSeperator == 0)		// 一小时
				{
					AxisDivide.m_iPosPixel		= AxisNode.m_iCenterPixel;

					//
					AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;
					AxisDivide.m_eLineStyle = CAxisDivide::ELSSolid;

					//
					AxisDivide.m_eLevelText		= CAxisDivide::EDLLevel3;
					AxisDivide.m_DivideText1.m_StrText.Format(L"%02d:%02d", Time.GetHour(), Time.GetMinute());
					AxisDivide.m_DivideText1.m_uiTextAlign = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
					AxisDivide.m_DivideText1.m_lTextColor = m_pRegionMain->GetAxisTextColor();

					AxisDivide.m_DivideText2 = AxisDivide.m_DivideText1;

					//
					iIndexDivide++;
				}
				else if (((int32)uiTime) % iHalfTimeSeperator == 0)		// 半小时
				{
					AxisDivide.m_iPosPixel		= AxisNode.m_iCenterPixel;

					//
					AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;
					AxisDivide.m_eLineStyle = CAxisDivide::ELSDot;

					//
					AxisDivide.m_eLevelText		= CAxisDivide::EDLLevel4;
					AxisDivide.m_DivideText1.m_StrText.Format(L"%02d:%02d", Time.GetHour(), Time.GetMinute());
					AxisDivide.m_DivideText1.m_uiTextAlign = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
					AxisDivide.m_DivideText1.m_lTextColor = m_pRegionMain->GetAxisTextColor();

					AxisDivide.m_DivideText2 = AxisDivide.m_DivideText1;

					//
					iIndexDivide++;
				}
			}
		}

		//  
		aAxisNodes.SetSize(iIndexNode);
		aAxisDivide.SetSize(iIndexDivide);
	}
	return true;
}

void CIoViewTrend::CalcMainRegionYAxis(CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide)
{
	ASSERT(NULL != m_pRegionMain);
	aYAxisDivide.SetSize(0);

	// 计算Y方向可以容纳多少个刻度
	CRect Rect = m_pRegionMain->GetRectCurves();

	CFont* pOldFont = pDC->SelectObject(GetIoViewFontObject(ESFSmall));					
	int32 iHeightText = pDC->GetTextExtent(_T("测试值")).cy;
	pDC->SelectObject(pOldFont);

	// int32 iHeightText = pDC->GetTextExtent(_T("测试值")).cy;
	int32 iHeightSub = iHeightText + 12;
	int32 iNum = Rect.Height() / iHeightSub + 1;
	if ( iNum % 2 != 0 )	iNum--;	// 偶数
	int32 iHalfNum = iNum / 2;

	// 获取当前显示小数点位
	int32 iSaveDec = 0;
	T_MerchNodeUserData* pData = NULL;
	if ( m_MerchParamArray.GetSize() > 0 )
	{
		pData = m_MerchParamArray.GetAt(0);
		CMerch* pMerch = pData->m_pMerchNode;
		if ( NULL != pMerch )
			iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;
	}

	// 获取纵坐标上几个主要的值
	CChartCurve *pCurveDependent = m_pRegionMain->GetDependentCurve();
	if (NULL == pCurveDependent)
		return;

	if ( NULL == pData )
	{
		return;
	}

	float fYMin = 0., fYMax = 0., fYMid = 0.;

	if (!pCurveDependent->GetYMinMax(fYMin, fYMax))
	{
		CGmtTime TimeInit	= pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time;
		CGmtTime TimeOpen	= pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeOpen.m_Time;

		CGmtTime TimeServer = m_pAbsCenterManager->GetServerTime();

		if ( TimeServer >= TimeInit && TimeServer <= TimeOpen )
		{
			// 取不到最大最小值,把纵坐标平分一下:
			const int32 KiFixedHeight = iHeightSub;
			CRect rectMainRegion	  = m_pRegionMain->GetRectCurves();
			if ( rectMainRegion.Height() <= 0 || rectMainRegion.Width() <= 0 )
			{
				return;
			}

			iNum = rectMainRegion.Height() / KiFixedHeight;
			if ( 0 != (iNum % 2) ) 
			{
				iNum += 1;
			}

			if ( 0 == iNum )
			{
				return;
			}

			float fHeightSub =rectMainRegion.Height() / (iNum * 1.0);

			aYAxisDivide.SetSize(iNum + 1);
			CAxisDivide *pAxisDivide = (CAxisDivide *)aYAxisDivide.GetData();

			float fStart = rectMainRegion.bottom;
			int32 iCount = 0;

			int32 i = 0;
			for ( i = 0; i <= iNum; i++,fStart -= fHeightSub )
			{			
				CAxisDivide &AxisDivide = pAxisDivide[iCount];
				AxisDivide.m_iPosPixel	= (int32)fStart;

				// 格线
				AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;		// 

				if ( i == iHalfNum )
				{		
					AxisDivide.m_eLineStyle = CAxisDivide::ELSDoubleSolid;	// 2实线
				}
				else
				{
					AxisDivide.m_eLineStyle = CAxisDivide::ELSSolid;		// 实线
				}

				AxisDivide.m_eLevelText						= CAxisDivide::EDLLevel1;

				AxisDivide.m_DivideText1.m_StrText			= L"";
				AxisDivide.m_DivideText2.m_StrText			= L"";

				AxisDivide.m_DivideText1.m_uiTextAlign		= DT_VCENTER | DT_RIGHT| DT_SINGLELINE;
				AxisDivide.m_DivideText2.m_uiTextAlign		= DT_VCENTER | DT_LEFT | DT_SINGLELINE;

				iCount++;
			}

			// 
			aYAxisDivide.SetSize(iCount);
			//

			int32 iGap = (rectMainRegion.top - aYAxisDivide[iCount-1].m_iPosPixel);
			if ( iGap < 0 )
			{
				iGap *= (-1);
			}

			if ( iGap > iCount )
			{
				iGap = iCount;
			}

			for ( i = 0; i <= iGap; i++,iGap-- )
			{
				CAxisDivide &AxisDivide = aYAxisDivide[iCount-i-1];
				AxisDivide.m_iPosPixel -= iGap;
			}

			//
			m_aYAxisDivideBkMain.RemoveAll();
			// m_aYAxisDivideBkMain.Copy(aYAxisDivide);

			return;
		}
		else
		{
			m_aYAxisDivideBkMain.RemoveAll();	// 清空备份
		}
		return;
	}

	fYMid = (fYMax + fYMin) / 2;

	float fPricePrevClose = 0.;
	if ( NULL != pData )
	{
		if (pData->m_TrendTradingDayInfo.m_bInit)
		{
			//fPricePrevClose = pData->m_TrendTradingDayInfo.GetPrevReferPrice();
			fPricePrevClose = GetTrendPrevClose();
			float fTmp = fPricePrevClose;
			bool32 b = pCurveDependent->PriceYToAxisY(fTmp, fPricePrevClose);
			//float fAxisYBase;
			//ASSERT( pCurveDependent->GetAxisBaseY(fAxisYBase) && fAxisYBase == fPricePrevClose );
			ASSERT( b );	// 该转换应当能成功的，因为min，max已经正常生成了
		}
	}

	{
		// 		float fBaseAxisY, fBase;
		// 		pCurveDependent->GetAxisBaseY(fBaseAxisY);
		// 		pCurveDependent->AxisYToPriceY(fBaseAxisY, fBase);
		// 		TRACE(_T("计算坐标轴min max base: %f,%f %f\r\n"), fYMin, fYMax, fBase);
	}

	if (fabs(fPricePrevClose - fYMid) > 0.000001)
	{
		// //ASSERT(0);	// 中值应该就是昨收价
		pCurveDependent->GetYMinMax(fYMin, fYMax);
	}

	// 计算浮点数刻度长度.
	float fHeightSub = (fYMax - fYMin) / iNum;
	int32 y;
	float fStart = fYMin;
	int32 iCount = 0;

	aYAxisDivide.SetSize(iNum + 1);
	CAxisDivide *pAxisDivide = (CAxisDivide *)aYAxisDivide.GetData();

	float fBaseAxisY = 0.0f;
	pCurveDependent->GetAxisBaseY(fBaseAxisY);
	for (int32 i = 0; i <= iNum; i++, fStart += fHeightSub)
	{
		if (i == iHalfNum)
			fStart = fYMid;

		if (i == iNum)
			fStart = fYMax;

		// 
		//if (!pCurveDependent->PriceYToRegionY(fStart, y))
		if (!pCurveDependent->AxisYToRegionY(fStart, y))
		{
			//ASSERT(0);
			break;
		}
		m_pRegionMain->RegionYToClient(y);

		// xl 0001762 限制YLeft显示字串长度 - 保留一个空格
		//CString StrValue	= Float2String(fStart, iSaveDec, true, false, false);
		//CString StrPercent	= Float2String(100.0f*(fStart-fPricePrevClose)/fPricePrevClose,2,false,false,true);
		CString StrValue;
		float fPriceCur = 0.0f;
		pCurveDependent->AxisYToPriceY(fStart, fPriceCur);
		Float2StringLimitMaxLength(StrValue, fPriceCur, iSaveDec, GetYLeftShowCharWidth()-1, true, false, false, false);
		CString StrPercent;
		Float2StringLimitMaxLength(StrPercent, 100.0f*(fStart-fBaseAxisY), 2, GetYLeftShowCharWidth()-1, true, false, false, true);
		if ( !StrPercent.IsEmpty() && StrPercent.GetAt(0) == _T('-')  )
		{
			StrPercent.TrimLeft(_T('-'));	// 不要-
		}

		// 
		CAxisDivide &AxisDivide = pAxisDivide[iCount];
		AxisDivide.m_iPosPixel	= y;

		// 格线
		AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;		// 
		if (i == iHalfNum)
		{		
			AxisDivide.m_eLineStyle = CAxisDivide::ELSDoubleSolid;	// 2实线
		}
		else
		{
			AxisDivide.m_eLineStyle = CAxisDivide::ELSSolid;		// 实线
		}

		// 文字
		AxisDivide.m_eLevelText						= CAxisDivide::EDLLevel1;

		AxisDivide.m_DivideText1.m_StrText			= StrValue;
		AxisDivide.m_DivideText2.m_StrText			= StrPercent;

		AxisDivide.m_DivideText1.m_uiTextAlign		= DT_VCENTER | DT_RIGHT | DT_SINGLELINE;
		AxisDivide.m_DivideText2.m_uiTextAlign		= DT_VCENTER | DT_LEFT | DT_SINGLELINE;

		if (i < iHalfNum)
		{
			AxisDivide.m_DivideText1.m_lTextColor	= GetIoViewColor(ESCFall);
			AxisDivide.m_DivideText2.m_lTextColor	= GetIoViewColor(ESCFall);
		}
		else if (i > iHalfNum)
		{
			AxisDivide.m_DivideText1.m_lTextColor	= GetIoViewColor(ESCRise);
			AxisDivide.m_DivideText2.m_lTextColor	= GetIoViewColor(ESCRise);
		}
		else
		{
			AxisDivide.m_DivideText1.m_lTextColor	= GetIoViewColor(ESCKeep);
			AxisDivide.m_DivideText2.m_lTextColor	= GetIoViewColor(ESCKeep);
		}

		// 
		iCount++;
	}

	// 
	aYAxisDivide.SetSize(iCount);
	//
	m_aYAxisDivideBkMain.RemoveAll();
	m_aYAxisDivideBkMain.Copy(aYAxisDivide);
}

void CIoViewTrend::CalcSubRegionYAxis(CChartRegion* pChartRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide)
{
	ASSERT(NULL != pChartRegion);
	aYAxisDivide.SetSize(0);

	// 计算Y方向可以容纳多少个刻度
	CRect Rect = pChartRegion->GetRectCurves();
	int32 iHeightText = pDC->GetTextExtent(_T("测试值")).cy;
	int32 iHeightSub  = iHeightText + 12;
	int32 iHeightHalf = iHeightSub/2;
	int32 iNum = Rect.Height()/iHeightSub + 1;

	// 	int32 iNum = Rect.Height() / iHeightSub + 1;
	// 	if ( iNum % 2 != 0 )	iNum--;	// 偶数
	if (iNum > 3)	iNum = 3;
	if (iNum < 2)	iNum = 2;

	// 获取当前显示小数点位
	int32 iSaveDec = 0;
	T_MerchNodeUserData* pData = NULL;

	if ( m_MerchParamArray.GetSize() > 0 )
	{
		pData = m_MerchParamArray.GetAt(0);
		CMerch* pMerch = pData->m_pMerchNode;
		if ( NULL != pMerch )
			iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;
	}
	if ( NULL == pData )
	{
		return;
	}

	// 获取纵坐标上几个主要的值
	CChartCurve *pCurveDependent = pChartRegion->GetDependentCurve();

	CPriceToAxisYObject AxisObj;
	bool32 bValidAxis = false;
	// 有可能竞价图中没有成交数据，但是有竞价数据
	if ( ETBT_CompetePrice == GetCurTrendBtnType()
		&& m_SubRegions.GetSize() == 1 )
	{
		bValidAxis = GetCompetePriceMinMax(false, AxisObj);
	}
	else if ( NULL != pCurveDependent )
	{
		bValidAxis = pCurveDependent->GetAxisYCalcObject(AxisObj);
	}

	float fYMin = 0., fYMax = 0.;
	fYMin = AxisObj.GetAxisYMin();
	fYMax = AxisObj.GetAxisYMax();
	if ( !bValidAxis
		|| !AxisObj.AxisYToPriceY(AxisObj.GetAxisYMin(), fYMin)
		|| !AxisObj.AxisYToPriceY(AxisObj.GetAxisYMax(), fYMax) )
	{
		CGmtTime TimeInit	= pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time;
		CGmtTime TimeOpen	= pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeOpen.m_Time;

		CGmtTime TimeServer = m_pAbsCenterManager->GetServerTime();

		if ( TimeServer >= TimeInit && TimeServer <= TimeOpen )
		{
			// 取不到最大最小值,把纵坐标平分一下:
			const int32 KiFixedHeight = iHeightSub;
			CRect rectRegion = pChartRegion->GetRectCurves();

			if ( rectRegion.Height() <= 0 || rectRegion.Width() <= 0 )
			{
				return;
			}

			iNum = rectRegion.Height() / KiFixedHeight;

			if ( 0 != (iNum % 2) ) 
			{
				iNum += 1;
			}

			if ( 0 == iNum )
			{
				return;
			}

			float fHeightSub = rectRegion.Height() / (iNum * 1.0);

			aYAxisDivide.SetSize(iNum + 1);
			CAxisDivide *pAxisDivide = (CAxisDivide *)aYAxisDivide.GetData();

			float fStart = rectRegion.bottom;
			int32 iCount = 0;

			int32 i = 0;
			for ( i = 0; i <= iNum; i++,fStart -= fHeightSub )
			{			
				CAxisDivide &AxisDivide = pAxisDivide[iCount];
				AxisDivide.m_iPosPixel	= (int32)fStart;

				// 格线
				AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;		// 				
				AxisDivide.m_eLineStyle = CAxisDivide::ELSSolid;		// 实线

				AxisDivide.m_eLevelText						= CAxisDivide::EDLLevel1;

				AxisDivide.m_DivideText1.m_StrText			= L"";
				AxisDivide.m_DivideText2.m_StrText			= L"";

				AxisDivide.m_DivideText1.m_uiTextAlign		= DT_VCENTER | DT_RIGHT| DT_SINGLELINE;
				AxisDivide.m_DivideText2.m_uiTextAlign		= DT_VCENTER | DT_LEFT | DT_SINGLELINE;

				iCount++;
			}

			// 
			aYAxisDivide.SetSize(iCount);
			//

			int32 iGap = (rectRegion.top - aYAxisDivide[iCount-1].m_iPosPixel);
			if ( iGap < 0 )
			{
				iGap *= (-1);
			}

			if ( iGap > iCount )
			{
				iGap = iCount;
			}

			for ( i = 0; i <= iGap; i++,iGap-- )
			{
				CAxisDivide &AxisDivide = aYAxisDivide[iCount-i-1];
				AxisDivide.m_iPosPixel -= iGap;
			}

			aYAxisDivide.SetSize(iCount);
			//
			m_aYAxisDivideBkSub.RemoveAll();
			return;
		}

		return;
	}

	// 计算浮点数刻度长度.
	float fHeightSub = (fYMax - fYMin) / iNum;
	int32 y;
	float fStart = fYMin;
	int32 iCount = 0;
	aYAxisDivide.SetSize(iNum + 1);
	CAxisDivide *pAxisDivide = (CAxisDivide *)aYAxisDivide.GetData();

	// 是否用算整数坐标
	bool32 bTran = false;
	/*	
	if ( 0 == iSaveDec && 3 == iNum)
	{
	// 是整数,且范围够大,做如下处理:
	int32 iMax = int32(fYMax);
	int32 iMin = int32(fYMin);

	//
	int32 iTemp = (iMax - iMin) / 3;
	int32 iStep = GetInter( iTemp );

	if ( -1 == iStep || 0 == iStep || iMax < 0 || iMin < 0)
	{
	bTran = false;
	}

	// 
	if( bTran )
	{
	int32 iBegin = MakeMinBigger(iMin, iStep);

	if ( -1 == iBegin || iBegin < iMin)
	{
	iBegin = GetInter(iMin);

	while ( iBegin < iMin )
	{
	// 保证最小值合法
	iBegin += iStep;
	}
	}

	int32 aiYAxis[4] = {0};

	for ( int32 i = 0 ; i <= iNum; i++)
	{
	aiYAxis[i] = iBegin + i*iStep;
	}

	if ( aiYAxis[3] > iMax )
	{
	// 保证最大值合法
	aiYAxis[3] = iMax;
	}

	// 赋值:
	for ( i = 0 ; i <= iNum; i++)
	{
	if (!pCurveDependent->ValueRegionY(float(aiYAxis[i]), y))
	{
	//ASSERT(0);
	break;
	}

	pChartRegion->RegionYToClient(y);

	CString StrValue	= Float2String(float(aiYAxis[i]), iSaveDec, true, false, false);

	// 
	CAxisDivide &AxisDivide = pAxisDivide[iCount];
	AxisDivide.m_iPosPixel	= y;

	// 格线
	AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;		// 
	AxisDivide.m_eLineStyle = CAxisDivide::ELSSolid;		// 实线

	// 文字
	AxisDivide.m_eLevelText						= CAxisDivide::EDLLevel1;

	AxisDivide.m_DivideText1.m_StrText			= StrValue;
	AxisDivide.m_DivideText2.m_StrText			= StrValue;

	AxisDivide.m_DivideText1.m_uiTextAlign		= DT_VCENTER | DT_RIGHT | DT_SINGLELINE;
	AxisDivide.m_DivideText2.m_uiTextAlign		= DT_VCENTER | DT_LEFT | DT_SINGLELINE;

	AxisDivide.m_DivideText1.m_lTextColor	= GetIoViewColor(ESCRise);
	AxisDivide.m_DivideText2.m_lTextColor	= GetIoViewColor(ESCRise);

	// 
	iCount++;
	}
	}			
	}
	else
	{
	bTran = false;
	}
	*/
	if ( !bTran )
	{
		for (int32 i = 0; i <= iNum; i++, fStart += fHeightSub)
		{
			if (i == iNum)
				fStart = fYMax;

			// 
			if ( !AxisObj.PriceYToRectY(Rect, fStart, y) )
			{
				ASSERT( 0 );
#ifdef _DEBUG
				if ( NULL != pCurveDependent )
				{
					//CNodeSequence* pNodes = pCurveDependent->GetNodes();
					//CArray<CNodeData, CNodeData&>& NodeDatas = pNodes->GetNodes();

					// TestNodes(NodeDatas, "c:\\Nodes.txt", m_pMerchXml->m_MerchInfo.m_StrMerchCode, m_pMerchXml->m_MerchInfo.m_iMarketId);
					//ASSERT(0);
				}
#endif
			}

			// 0001762 - 成交量等大指标不需要显示小数点 > 1000 
			if ( fStart >= 1000 || fStart <= -1000  )
			{
				iSaveDec = 0;
			}

			// xl 0001762 限制YLeft显示字串长度 - 保留一个空格
			//CString StrValue	= Float2String(fStart, iSaveDec, true, false, false);
			CString StrValue;
			Float2StringLimitMaxLength(StrValue, fStart, iSaveDec, GetYLeftShowCharWidth()-1, true, false, false, false);

			// 
			CAxisDivide &AxisDivide = pAxisDivide[iCount];
			AxisDivide.m_iPosPixel	= y;

			// 格线
			AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;		// 
			AxisDivide.m_eLineStyle = CAxisDivide::ELSDot;			// 实线

			// 文字
			AxisDivide.m_eLevelText						= CAxisDivide::EDLLevel1;

			AxisDivide.m_DivideText1.m_StrText			= StrValue;
			AxisDivide.m_DivideText2.m_StrText			= StrValue;

			if ( iNum == i && pChartRegion->GetTopSkip() < iHeightHalf )
			{
				// 最后一个如果没有TopSkip则顶部对齐
				AxisDivide.m_DivideText1.m_uiTextAlign		= DT_TOP | DT_RIGHT | DT_SINGLELINE;
				AxisDivide.m_DivideText2.m_uiTextAlign		= DT_TOP | DT_LEFT | DT_SINGLELINE;
			}
			else
			{
				AxisDivide.m_DivideText1.m_uiTextAlign		= DT_VCENTER | DT_RIGHT | DT_SINGLELINE;
				AxisDivide.m_DivideText2.m_uiTextAlign		= DT_VCENTER | DT_LEFT | DT_SINGLELINE;
			}

			AxisDivide.m_DivideText1.m_lTextColor	= GetIoViewColor(ESCVolume);
			AxisDivide.m_DivideText2.m_lTextColor	= GetIoViewColor(ESCVolume);

			// 
			iCount++;
		}
	}

	// 
	aYAxisDivide.SetSize(iCount);
	//
	m_aYAxisDivideBkSub.RemoveAll();
	m_aYAxisDivideBkSub.Copy(aYAxisDivide);

}

void CIoViewTrend::CalcLayoutRegions(bool bOnlyUpdateMainRegion)
{	
	if (m_MerchParamArray.GetSize() <= 0)
		return;

	m_eMainCurveAxisYType = CPriceToAxisYObject::EAYT_Pecent;	// 仅支持百分比

	// 计算Y方向可以容纳多少个刻度
	float fMinYRange = 0.0f;
	if ( NULL != m_pMerchXml )
	{
		CRect Rect = m_pRegionMain->GetRectCurves();

		CClientDC dc(this);
		CFont* pOldFont = dc.SelectObject(GetIoViewFontObject(ESFSmall));					
		int32 iHeightText = dc.GetTextExtent(_T("测试值")).cy;
		dc.SelectObject(pOldFont);

		int32 iHeightSub = iHeightText + 12;
		int32 iDivideCount = Rect.Height() / iHeightSub + 1;
		if ( iDivideCount % 2 != 0 )	iDivideCount--;	// 偶数
		iDivideCount /= 2;	// 取一半

		double iSaveDec = m_pMerchXml->m_MerchInfo.m_iSaveDec;
		ASSERT( iSaveDec >= 0 );
		//float fPricePrevClose = pMerchData->m_TrendTradingDayInfo.GetPrevReferPrice();	// 昨收
		float fPricePrevClose = GetTrendPrevClose();
		if ( iSaveDec <= 0 || fPricePrevClose >= 1000.0f )	// 昨收高于1000由于宽度原因将不能显示全小数位
		{
			fMinYRange = iDivideCount;	// 最小单位1
		}
		else
		{
			fMinYRange = iDivideCount * 1/(float)(pow(10.0, iSaveDec));		// 2应该为0.01
		}
	}

	// 计算主region的坐标值， 对称坐标
	{
		if (NULL != m_pRegionMain)
		{
			bool32 bValidCurveYMinMax = false;
			float fCurveYMin = 0., fCurveYMax = 0.;

			// 百分比计算base值
			// 在最大最小未确定时，不要使用curve的PriceToAxis等计算方法
			CChartCurve *pDepCurve = m_pRegionMain->GetDependentCurve();
			float fBase = 0.0f;
			if ( NULL == pDepCurve )
			{
				return;
			}
			pDepCurve->SetAxisYType(m_eMainCurveAxisYType);
			if ( CalcKLinePriceBaseValue(pDepCurve, fBase) )
			{
				ASSERT( fBase != CChartCurve::GetInvalidPriceBaseYValue() );
				// 计算所有曲线的上下边界值
				for (int32 i = 0; i < m_pRegionMain->m_Curves.GetSize(); i++)
				{
					float fYMin = 0., fYMax = 0.;

					CChartCurve *pChartCurve = m_pRegionMain->m_Curves[i];
					pChartCurve->SetAxisYType(m_eMainCurveAxisYType);	// 仅有百分比坐标
					if ( CheckFlag(pChartCurve->m_iFlag, CChartCurve::KYTransformToAlignDependent) )
					{
						// 叠加商品用自己的
						float fCmpBase = CChartCurve::GetInvalidPriceBaseYValue();
						if ( CalcKLinePriceBaseValue(pChartCurve, fCmpBase) )
						{
							pChartCurve->SetPriceBaseY(fCmpBase, true);
						}
						else
						{
							continue;
						}
					}
					else
					{
						pChartCurve->SetPriceBaseY(fBase, true);
					}

					if ( pChartCurve == m_pRedGreenCurve )
					{
						continue;		// 红绿线不加入计算
					}

					if ( CheckFlag(pChartCurve->m_iFlag, CChartCurve::KInVisible) )
					{
						continue;	// 隐藏的线不参与计算
					}

					if (pChartCurve->CalcY(fYMin, fYMax))
					{
						if (bValidCurveYMinMax)
						{
							if (fYMin < fCurveYMin) fCurveYMin = fYMin;
							if (fYMax > fCurveYMax) fCurveYMax = fYMax;
						}
						else
						{
							bValidCurveYMinMax = true;
							fCurveYMin = fYMin;
							fCurveYMax = fYMax;
						}
					}
				}

				if ( m_bRiseFallAxis )
				{
					// 涨跌停坐标
					float fRise, fFall;
					float fAxisRise, fAxisFall;
					if ( CalcRiseFallMaxPrice(fRise, fFall)
						&& CPriceToAxisYObject::PriceYToAxisYByBase(m_eMainCurveAxisYType, fBase, fRise, fAxisRise)
						&& CPriceToAxisYObject::PriceYToAxisYByBase(m_eMainCurveAxisYType, fBase, fFall, fAxisFall))
					{
						if ( bValidCurveYMinMax )
						{
							fCurveYMax = max(fAxisRise, fCurveYMax);
							fCurveYMin = min(fAxisFall, fCurveYMin);
						}
						else
						{
							fCurveYMax = fAxisRise;
							fCurveYMin = fAxisFall;
							bValidCurveYMinMax = true;
						}
					}
				}

				// 同一分钟内最 低/高 价变化的情况, 需要加上这个处理 - 非历史分时才需要
				if ( NULL != m_pMerchXml 
					&& NULL !=  m_pMerchXml->m_pRealtimePrice 
					&& 0. != m_pMerchXml->m_pRealtimePrice->m_fPriceHigh 
					&& 0. != m_pMerchXml->m_pRealtimePrice->m_fPriceLow
					&& !m_bHistoryTrend
					&& bValidCurveYMinMax )
				{
					float fMinRealTimePrice = m_pMerchXml->m_pRealtimePrice->m_fPriceLow;
					float fMaxRealTimePrice = m_pMerchXml->m_pRealtimePrice->m_fPriceHigh;

					float fTmpMax = fMaxRealTimePrice;
					float fTmpMin = fMinRealTimePrice;
					bool32 b = CPriceToAxisYObject::PriceYToAxisYByBase(m_eMainCurveAxisYType, fBase, fTmpMax, fMaxRealTimePrice);
					b = CPriceToAxisYObject::PriceYToAxisYByBase(m_eMainCurveAxisYType, fBase, fTmpMin, fMinRealTimePrice) && b;
					ASSERT( b );

					fCurveYMax = fCurveYMax > fMaxRealTimePrice ? fCurveYMax : fMaxRealTimePrice;
					fCurveYMin = fCurveYMin < fMinRealTimePrice ? fCurveYMin : fMinRealTimePrice;	
				}

				if ( fMinYRange > 0.0f )
				{
					// 有最小刻度限定
					float fPricePrevClose = fBase;	// 昨收
					float fAxisMaxY = fPricePrevClose + fMinYRange;
					float fAxisMinY = fPricePrevClose - fMinYRange;
					float fAxisPreClose;
					bool32 b = CPriceToAxisYObject::PriceYToAxisYByBase(m_eMainCurveAxisYType, fBase, fPricePrevClose, fAxisPreClose);
					b = CPriceToAxisYObject::PriceYToAxisYByBase(m_eMainCurveAxisYType, fBase, fAxisMaxY, fAxisMaxY) && b;
					b = CPriceToAxisYObject::PriceYToAxisYByBase(m_eMainCurveAxisYType, fBase, fAxisMinY, fAxisMinY) && b;
					ASSERT( b );
					if ( bValidCurveYMinMax )
					{
						// 比较是否比最小刻度小，小则取最小刻度
						fCurveYMax = max(fCurveYMax, fAxisMaxY);
						fCurveYMin = min(fCurveYMin, fAxisMinY);
					}
					else
					{
						// 没有有效值，则以最小限定值作为有效值, 以昨收作为中线划分
						fCurveYMax = fAxisMaxY;
						fCurveYMin = fAxisMinY;
						bValidCurveYMinMax = true;
					}
				}

				if ( GetCurTrendBtnType() == ETBT_CompetePrice )
				{
					float fMax, fMin;
					if ( CalcCompetePriceMinMax(pDepCurve, fMin, fMax) )
					{
						if ( bValidCurveYMinMax )
						{
							fCurveYMax = max(fCurveYMax, fMax);
							fCurveYMin = min(fCurveYMin, fMin);
						}
						else
						{
							fCurveYMax = fMax;
							fCurveYMin = fMin;
							bValidCurveYMinMax = true;
						}
					}
				}
				// 以昨收价作为中值， 上下对称分
				if (bValidCurveYMinMax)
				{
					//float fPricePrevClose = pMerchData->m_TrendTradingDayInfo.GetPrevReferPrice();	// 昨收
					float fPricePrevClose = fBase;	// 昨收

					float fTmp = fPricePrevClose;
					bool32 b = CPriceToAxisYObject::PriceYToAxisYByBase(m_eMainCurveAxisYType, fBase, fTmp, fPricePrevClose);
					ASSERT( b );

					float fHeight1 = fCurveYMax - fPricePrevClose;
					float fHeight2 = fPricePrevClose - fCurveYMin;
					if (fHeight1 > fHeight2)
						fCurveYMin = fPricePrevClose - fHeight1;
					else
						fCurveYMax = fPricePrevClose + fHeight2;
				}
			}

			// 设置所有的主region中的curve的上下限值
			for (int32 i = 0; i < m_pRegionMain->m_Curves.GetSize(); i++)
			{				
				CChartCurve *pChartCurve = m_pRegionMain->m_Curves[i];
				pChartCurve->SetYMinMax(fCurveYMin, fCurveYMax, bValidCurveYMinMax);
			}

			{
				//	TRACE(_T("min max base(%d): %f,%f %f\r\n"), bValidCurveYMinMax, fCurveYMin, fCurveYMax, fBase);
			}
		}
	}

	// 计算各个子图下的curve取值
	if (!bOnlyUpdateMainRegion)
	{
		// 各个子图
		for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
		{
			CChartRegion *pSubRegionMain = m_SubRegions[i].m_pSubRegionMain;
			const bool32 bVolSub = 0 == i;

			if (NULL != pSubRegionMain)
			{
				bool32 bValidCurveYMinMax = false;
				float fCurveYMin = 0., fCurveYMax = 0.;

				// 计算所有曲线的上下边界值
				for (int32 q = 0; q < pSubRegionMain->m_Curves.GetSize(); q++)
				{
					float fYMin = 0., fYMax = 0.;

					CChartCurve *pChartCurve = pSubRegionMain->m_Curves[q];

					if (pChartCurve->CalcY(fYMin, fYMax))
					{
						if (bValidCurveYMinMax)
						{
							if (fYMin < fCurveYMin) fCurveYMin = fYMin;
							if (fYMax > fCurveYMax) fCurveYMax = fYMax;
						}
						else
						{
							bValidCurveYMinMax = true;
							fCurveYMin = fYMin;
							fCurveYMax = fYMax;
						}
					}
				}

				if ( bVolSub
					&& ETBT_CompetePrice == GetCurTrendBtnType() )
				{
					float fMax, fMin;
					// 竞价
					CChartCurve *pDep = pSubRegionMain->GetDependentCurve();
					bool32 bMinMax = false;
					if ( NULL != pDep )
					{
						bMinMax = CalcCompetePriceMinMax(pSubRegionMain->GetDependentCurve(), fMin, fMax);
					}
					if ( !bMinMax )
					{
						bMinMax = CalcCompetePriceMinMaxVol(true, fMin, fMax);
					}

					if ( bMinMax )
					{
						if ( bValidCurveYMinMax )
						{
							fCurveYMin = min(fCurveYMin, fMin);
							fCurveYMax = max(fCurveYMax, fMax);
						}
						else
						{
							fCurveYMin = ( fMin);
							fCurveYMax = ( fMax);
							bValidCurveYMinMax = true;
						}
					}
				}

				// 设置所有的主region中的curve的上下限值
				for (int32 m = 0; m < pSubRegionMain->m_Curves.GetSize(); m++)
				{
					CChartCurve *pChartCurve = pSubRegionMain->m_Curves[m];
					pChartCurve->SetYMinMax(fCurveYMin, fCurveYMax, bValidCurveYMinMax);

					//TRACE(_T("分时最大最小: %d %f-%f\n"), bValidCurveYMinMax, fCurveYMin, fCurveYMax);
				}
			}		
		}
	}
}

void CIoViewTrend::UpdateAxisSize( bool32 bPreDraw /*= true */ )
{
	CIoViewChart::UpdateAxisSize(bPreDraw);

	if ( NULL != m_pRegionMain )
	{
		// 取消right - 应该在全屏 - 独占时加入right，现没实现
		// 		int32 iRightSkip = m_pRegionMain->GetRightSkip();
		// 		if ( iRightSkip < 1 )
		// 		{
		// 			m_pRegionMain->SetRightSkip(1);
		// 		}
	}

	// 减少成交量副图的topSkip
	if ( m_SubRegions.GetSize() > 0 )
	{
		T_SubRegionParam &SubVol = m_SubRegions[0];
		SubVol.m_pSubRegionYLeft->SetTopSkip(20);		// 留一点点拖动的感应区域间隔
		SubVol.m_pSubRegionYRight->SetTopSkip(20);
		SubVol.m_pSubRegionMain->SetTopSkip(20);
	}

	int32 iLeftSkip = 0;
	if ( ETBT_CompetePrice == GetCurTrendBtnType()
		&& NULL != m_pMerchXml )
	{
		// 竞价图 主副 图要移出部分空间
		iLeftSkip = 100;	// 固定长度
	}
	if ( NULL != m_pRegionMain )
	{
		m_pRegionMain->SetLeftSkip(iLeftSkip);
	}
	for ( int32 i=0; i < m_SubRegions.GetSize() ; i++ )
	{
		if ( NULL != m_SubRegions[i].m_pSubRegionMain )
		{
			m_SubRegions[i].m_pSubRegionMain->SetLeftSkip(iLeftSkip);
		}
	}
}

BOOL CIoViewTrend::PreTranslateMessage( MSG* pMsg )
{
	if ( WM_SYSCHAR == pMsg->message )
	{
		int iChar = (int)pMsg->wParam - '0';
		if ( iChar >= 0 && iChar <= 9 )
		{
			// alt + 0~9
			if ( iChar == 0 )
			{
				iChar = 10;
			}

			SetSingleSection(false);
			SetTrendMultiDay(iChar, false, true);
			return TRUE;
		}
		else if ( pMsg->wParam == _T('-') && !m_bHistoryTrend )
		{
			// 历史分时的半场无意义
			SetSingleSection(!m_bSingleSectionTrend);
			return TRUE;
		}
	}
	else if ( WM_KEYDOWN == pMsg->message )
	{
		if ( pMsg->wParam == 186 ) //_T(';') )
		{
			float fRise, fFall;
			if ( CalcRiseFallMaxPrice(fRise, fFall) )
			{
				PostMessage(WM_COMMAND, ID_TREND_RFMAXAXIS, 0);
				return TRUE;
			}
		}
		else if ( pMsg->wParam == VK_UP && IsCtrlPressed() )
		{
			int32 iCur = m_iTrendDayCount;
			if ( iCur < 10 )
			{
				SetSingleSection(false);
				SetTrendMultiDay(++iCur, false, true);
			}
			return TRUE;
		}
		else if ( pMsg->wParam == VK_DOWN && IsCtrlPressed() )
		{
			int32 iCur = m_iTrendDayCount;
			if ( iCur > 1 )
			{
				SetSingleSection(false);
				SetTrendMultiDay(--iCur, false, true);
			}
			return TRUE;
		}
		else if ( VK_F8 == pMsg->wParam )
		{
			PostMessage(WM_COMMAND, ID_TREND_SHOWTICK, NULL);
			return TRUE;
		}
	}
	if ( m_bHistoryTrend )
	{
		return CControlBase::PreTranslateMessage(pMsg);
	}
	else
	{
		return CIoViewChart::PreTranslateMessage(pMsg);
	}
}

void CIoViewTrend::DrawHoldLine( IN CMemDCEx* pDC, IN CMemDCEx* pPickDC, IN CChartRegion* pRegion )
{
	T_MerchNodeUserData *pData = NULL;
	if ( m_MerchParamArray.GetSize() > 0 )
	{
		pData = m_MerchParamArray[0];
	}
	if ( NULL == pData || NULL == pData->m_pKLinesShow || NULL == pDC /*|| NULL == pPickDC */|| NULL == pRegion )
	{
		return;
	}
	if ( !CReportScheme::IsFuture(pData->m_pMerchNode->m_Market.m_MarketInfo.m_eMarketReportType) )
	{
		return;
	}

	const float fMax = m_fMaxHold;
	const float fMin = m_fMinHold;
	CRect RectDraw = pRegion->GetRectCurves();

	int32 iSize = pData->m_pKLinesShow->GetSize();
	//ASSERT( iSize == pRegion->m_aXAxisNodes.GetSize() );		// 有可能没有数据!
	if ( iSize < 1 )
	{
		return;
	}
	int32 iNodeSize = pRegion->m_aXAxisNodes.GetSize();
	iSize = min(iNodeSize, iSize);
	CArray<POINT, const POINT &> aPts;
	aPts.SetSize(0, iSize);
	POINT pt = {0};
	for ( int32 i=0; i < iSize ; i++ )
	{
		pt.x = pRegion->m_aXAxisNodes[i].m_iCenterPixel;
		float fVal = pData->m_pKLinesShow->GetNodes().ElementAt(i).m_fHold;
		if ( fMax - fMin < 0.1 )
		{
			pt.y = RectDraw.bottom;
		}
		else
		{
			pt.y = RectDraw.bottom - (int32)(RectDraw.Height() * ( fVal-fMin )/(fMax-fMin));
		}
		if ( pt.y < RectDraw.top || pt.y > RectDraw.bottom )
		{
			ASSERT( 0 );
		}
		else
		{
			aPts.Add(pt);
		}
	}

	if ( aPts.GetSize() > 0 )
	{
		pDC->MoveTo(aPts[0]);
	}

	CPen penLine;
	penLine.CreatePen(PS_SOLID, 0, RGB(128,96,64));
	CPen *pPenOld = pDC->SelectObject(&penLine);

	int32 iSection = aPts.GetSize() / 1000;
	const int32 iLeave = aPts.GetSize() % 1000;
	const POINT *pPts = aPts.GetData();
	while ( iSection-- > 0 )
	{
		pDC->PolylineTo(pPts, 1000);
		pPts += 1000;
	}
	pDC->PolylineTo(pPts, iLeave);

	pDC->SelectObject(pPenOld);
}

void CIoViewTrend::RequestMinuteKLineData(T_MerchNodeUserData &MerchData)
{
	{
		T_MerchNodeUserData* pData = &MerchData;
		if ( NULL == pData || m_MerchParamArray.GetSize() <= 0 )
		{
			return;
		}

		// 主商品可请求多日，叠加的不允许，
		bool32 bIsMain = pData->bMainMerch;	// 主商品

		// 
		CMerch* pMerch = pData->m_pMerchNode;
		if ( NULL == pMerch )
		{
			return;
		}

		ASSERT(pData->m_TrendTradingDayInfo.m_bInit);

		ASSERT( m_iTrendDayCount <= TREND_MAX_DAY_COUNT && m_iTrendDayCount >= TREND_MIN_DAY_COUNT );
		// 发送分钟线请求
		// 如果是多日分时，需要等到日k线回来才能确定要请求具体什么时间段的分钟k线
		CGmtTime TimeStart, TimeEnd;
		CGmtTime TimeLastClose;
		if ( bIsMain )
		{
			// 主商品有额外的记录
			// 找寻该商品日k线数据, 基于现在实际k线数据总是连续的
			m_aTrendMultiDayIOCTimes.RemoveAll();	// 清空以前的数据
			m_aTrendMultiDayIOCTimes.Add(pData->m_TrendTradingDayInfo.m_MarketIOCTime);	// 至少请求了一天的

			if ( m_iTrendDayCount > 1 )
			{
				// 主商品且多日图，则要请求多天了
				TimeStart = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time;
				TimeEnd   = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeEnd.m_Time;
				TimeLastClose = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeClose.m_Time;

				int32 iPosFound;
				CMerchKLineNode* pKLineDayRequest = NULL;
				pMerch->FindMerchKLineNode(EKTBDay, iPosFound, pKLineDayRequest);
				if (NULL != pKLineDayRequest)	// 找到对应日线
				{
					// 找寻日k线，含当天
					//int32 iPos = CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(pKLineDayRequest->m_KLines, TimeEnd);
					// 日K线以开盘时间为天的单位
					int32 iPos = CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(pKLineDayRequest->m_KLines, TimeLastClose);
					if ( iPos >= 0 && iPos < pKLineDayRequest->m_KLines.GetSize() )
					{
						// 有可能出现根本就没有当日的日K线数据(于初始化时间时), 所以iPos并不一定是当日数据
						// 有多余的日k线可以用, 尝试找要求数量的k线
						int32 iEndPos = iPos-1;
						CGmtTime TimeToday(TimeLastClose);
						SaveDay(TimeToday);
						if ( pKLineDayRequest->m_KLines[iPos].m_TimeCurrent < TimeToday )
						{
							++iEndPos;	// pos指向的点不是当日点, 要包含该点
						}
						if ( iEndPos >= 0 )
						{
							int32 iStartPos = (iEndPos+1) - (m_iTrendDayCount-1);	// (开始点包含)(当日点不包含)
							iStartPos = max(iStartPos, 0);	// 如果没有那么多，就尽量多点
							for ( int32 iIoc = iEndPos ; iIoc >= iStartPos ; iIoc-- )	// 把有的都加进去，依赖于k线是线性连续的
							{
								CGmtTime TimeKLineStart = pKLineDayRequest->m_KLines[iIoc].m_TimeCurrent;		// 这里的时间取哪个待确认
								CMarketIOCTimeInfo MIOCTime;
								if ( pData->m_pMerchNode->m_Market.GetSpecialTradingDayTime(TimeKLineStart, MIOCTime, pData->m_pMerchNode->m_MerchInfo) )
								{
									TimeStart = MIOCTime.m_TimeInit.m_Time;	// 取最早的k线的值
									//m_aTrendMultiDayIOCTimes.Add(MIOCTime);
									if ( m_aTrendMultiDayIOCTimes.GetSize() > 0
										&& m_aTrendMultiDayIOCTimes[m_aTrendMultiDayIOCTimes.GetUpperBound()].m_TimeInit > MIOCTime.m_TimeInit )
									{
										m_aTrendMultiDayIOCTimes.Add(MIOCTime);
									}
									else
									{
										ASSERT( 0 );
									}
								}
								else
								{
									ASSERT( 0 );
								}
							}
							ASSERT( m_aTrendMultiDayIOCTimes.GetSize() > 1 ); // 至少新增了一天
							UpdateTrendMultiDayChange();	// 变更标题
						}
					}
				}
			}
			else 
			{
				// 仅请求指定日期的(逻辑上的当天)
				TimeStart = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time;
				TimeEnd   = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeEnd.m_Time;
			}
		}
		else
		{
			// 叠加商品, 叠加商品的请求肯定是在后面的, 叠加商品所有的请求数据都是以主商品为准，有多少请求多少
			if ( !GetTrendStartEndTime(TimeStart, TimeEnd) )
			{
				// 仅请求指定日期的(逻辑上的当天), 主商品失败，那就可以不发请求了
				TimeStart = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time;
				TimeEnd   = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeEnd.m_Time;

				return;	// 不发请求
			}
		}
		CMmiReqMerchKLine info;
		info.m_eKLineTypeBase	= EKTBMinute;
		info.m_iMarketId		= pData->m_pMerchNode->m_MerchInfo.m_iMarketId;
		info.m_StrMerchCode		= pData->m_pMerchNode->m_MerchInfo.m_StrMerchCode;
		info.m_eReqTimeType		= ERTYSpecifyTime;
		info.m_TimeStart		= TimeStart;
		info.m_TimeEnd			= TimeEnd;			
		DoRequestViewData(info);
	}
}

void CIoViewTrend::TryReqMoreMultiDayMinuteKLineData()
{
	// 尝试请求比先前发送的更多的要求的多日k线数据
	// 如果现有日k线还是没有前面发送的多，则不发送请求
	if ( m_MerchParamArray.GetSize() <= 0 )
	{
		ASSERT( 0 );
		return;
	}

	if ( m_iTrendDayCount == m_aTrendMultiDayIOCTimes.GetSize() )
	{
		return;	// 已经满足了需求了
	}
	// 有可能是切换商品时直接调用update，但是此时是没有requestdata初始化的，所以这里也不应当请求数据
	ASSERT( m_aTrendMultiDayIOCTimes.GetSize() < m_iTrendDayCount );

	T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);	// 仅操作主商品
	if ( NULL == pData )
	{
		ASSERT( 0 );
		return;
	}

	// 
	CMerch* pMerch = pData->m_pMerchNode;
	if ( NULL == pMerch )
	{
		ASSERT( 0 );
		return;
	}

	ASSERT(pData->m_TrendTradingDayInfo.m_bInit);
	ASSERT( m_iTrendDayCount <= TREND_MAX_DAY_COUNT && m_iTrendDayCount >= TREND_MIN_DAY_COUNT );
	// 发送分钟线请求
	// 如果是多日分时，需要等到日k线回来才能确定要请求具体什么时间段的分钟k线
	{
		// 找寻该商品日k线数据, 基于现在实际k线数据总是连续的
		CGmtTime TimeStart, TimeEnd;
		CGmtTime TimeLastClose;
		if ( m_iTrendDayCount > 1 )		// 非多天不需要额外尝试请求
		{
			// 主商品且多日图，则要请求多天了
			TimeStart = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time;
			TimeEnd   = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeEnd.m_Time;
			TimeLastClose = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeClose.m_Time;

			int32 iPosFound;
			CMerchKLineNode* pKLineDayRequest = NULL;
			pMerch->FindMerchKLineNode(EKTBDay, iPosFound, pKLineDayRequest);
			if (NULL != pKLineDayRequest)	// 找到对应日线
			{
				//const int32 iPos = CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(pKLineDayRequest->m_KLines, TimeEnd);
				// 日K线以开盘时间为天的单位
				const	int32 iPos = CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(pKLineDayRequest->m_KLines, TimeLastClose);
				if ( iPos >= 0 && iPos < pKLineDayRequest->m_KLines.GetSize() )
				{
					int32 iEndPos = iPos-1;
					CGmtTime TimeToday(TimeLastClose);
					SaveDay(TimeToday);
					if ( pKLineDayRequest->m_KLines[iPos].m_TimeCurrent < TimeToday )
					{
						++iEndPos;	// pos指向的点不是当日点, 要包含该点
					}
					if ( iEndPos >= 0 )
					{
						int32 iStartPos = (iEndPos+1) - (m_iTrendDayCount-1);	// (开始点包含)(当日点不包含)
						ASSERT( iStartPos <= iEndPos );
						iStartPos = min(iEndPos, iStartPos);
						//int32 iStartPos = iPos - (m_iTrendDayCount - 1);
						iStartPos = max(iStartPos, 0);	// 如果没有那么多，就尽量多点
						int32 iOldPos = iEndPos+1 - (m_aTrendMultiDayIOCTimes.GetSize() -1);
						ASSERT( iOldPos <= iEndPos+1 );
						if ( iStartPos < iOldPos )
						{
							// 有以前没有发送请求的日的分钟k线
							int32 iIoc = iOldPos-1;
							ASSERT( iIoc <= iEndPos );
							iIoc = min(iIoc, iEndPos);
							for ( ; iIoc >= iStartPos ; iIoc-- )
							{
								CGmtTime TimeKLineStart = pKLineDayRequest->m_KLines[iIoc].m_TimeCurrent;
								CMarketIOCTimeInfo MIOCTime;
								if ( pData->m_pMerchNode->m_Market.GetSpecialTradingDayTime(TimeKLineStart, MIOCTime, pData->m_pMerchNode->m_MerchInfo) )
								{
									TimeStart = MIOCTime.m_TimeInit.m_Time;	// 取最早的k线的值
									// 不论有没有发送, 日k线数据已经存在了，下次调用requestviewdata时会找到合适的
									if ( m_aTrendMultiDayIOCTimes.GetSize() > 0
										&& m_aTrendMultiDayIOCTimes[m_aTrendMultiDayIOCTimes.GetUpperBound()].m_TimeInit > MIOCTime.m_TimeInit )
									{
										m_aTrendMultiDayIOCTimes.Add(MIOCTime);
									}
									else
									{
										ASSERT( 0 );
									}
								}
								else
								{
									ASSERT( 0 );
								}
							}


							CMmiReqMerchKLine info;
							info.m_eKLineTypeBase	= EKTBMinute;
							info.m_iMarketId		= pData->m_pMerchNode->m_MerchInfo.m_iMarketId;
							info.m_StrMerchCode		= pData->m_pMerchNode->m_MerchInfo.m_StrMerchCode;
							info.m_eReqTimeType		= ERTYSpecifyTime;
							info.m_TimeStart		= TimeStart;
							info.m_TimeEnd			= TimeEnd;			
							DoRequestViewData(info);
							ASSERT( m_aTrendMultiDayIOCTimes.GetSize() > (iPos -iOldPos +1) );
							UpdateTrendMultiDayChange();	// 变更标题

							// 叠加商品也多请求一点数据
							for ( int32 i=0; i < m_MerchParamArray.GetSize() ; i++ )
							{
								if ( NULL != m_MerchParamArray[i] && !m_MerchParamArray[i]->bMainMerch )
								{
									RequestMinuteKLineData(*m_MerchParamArray[i]);
								}
							}
						}
					}
				}
			}
		}
		else
		{
			ASSERT( 0 );
		}
	}
}

void CIoViewTrend::SetTrendMultiDay( int iDay, bool32 bClearData, bool32 bReqData)
{
	if ( iDay >= TREND_MIN_DAY_COUNT && iDay <= TREND_MAX_DAY_COUNT )
	{
		m_iTrendDayCount = iDay;
		m_bSingleSectionTrend = false;	// 取消半场走势标志
		if ( ETBT_CompetePrice == GetCurTrendBtnType()
			&& iDay != 1 )
		{
			// 取消竞价图
			m_iBtnCur = 0;
			SendMessage(WM_SETREDRAW, (WPARAM)FALSE, 0);
			OnTrendBtnSelChanged();			// 注意不要递归调用
			ASSERT( ETBT_CompetePrice != GetCurTrendBtnType() );	// 注意此处是写死的取位置0
			SendMessage(WM_SETREDRAW, (WPARAM)TRUE, 0);
		}

		if ( bClearData )
		{
			// 清除原来的日期数据
			m_aTrendMultiDayIOCTimes.RemoveAll();
			if ( m_MerchParamArray.GetSize() > 0 
				&& m_MerchParamArray[0] != NULL
				&& m_MerchParamArray[0]->m_TrendTradingDayInfo.m_bInit )
			{
				m_aTrendMultiDayIOCTimes.Add( m_MerchParamArray[0]->m_TrendTradingDayInfo.m_MarketIOCTime );	// 仅赋予一个默认的
			}
		}
		else
		{
			if ( m_aTrendMultiDayIOCTimes.GetSize() > iDay )
			{
				m_aTrendMultiDayIOCTimes.SetSize(iDay);	// 截断一部分
			}
			else if ( m_aTrendMultiDayIOCTimes.GetSize() <= 0 )
			{
				// 必须尝试给一个默认的
				if ( m_MerchParamArray.GetSize() > 0 
					&& m_MerchParamArray[0] != NULL
					&& m_MerchParamArray[0]->m_TrendTradingDayInfo.m_bInit )
				{
					m_aTrendMultiDayIOCTimes.Add( m_MerchParamArray[0]->m_TrendTradingDayInfo.m_MarketIOCTime );	// 仅赋予一个默认的
				}
			}
		}
		if ( bReqData )
		{
			RequestViewData();
		}

		UpdateTrendMultiDayChange();
	}
	else
	{
		ASSERT( 0 );	// 无视它
	}
}

void CIoViewTrend::UpdateTrendMultiDayChange()
{
	bool32 bValidMerch = NULL != m_pRegionMain && NULL != m_pMerchXml;
	if ( m_bSingleSectionTrend && bValidMerch )
	{
		// 半场走势标题
		CGmtTime TimeOpen, TimeClose;
		if ( GetTrendSingleSectionOCTime(TimeOpen, TimeClose) )
		{
			CTime tOpen(TimeOpen.GetTime());
			CTime tClose(TimeClose.GetTime());
			CString StrName;
			StrName.Format(_T("%s 【时间段 %02d:%02d--%02d:%02d】"), 
				m_pMerchXml->m_MerchInfo.m_StrMerchCnName.GetBuffer(), 
				tOpen.GetHour(), tOpen.GetMinute(),
				tClose.GetHour(), tClose.GetMinute() );


			//if (m_bShowTopToolBar)
			//{
			//	SetTopCenterTitle(StrName);
			//}
			//else
			{
				m_pRegionMain->SetTitle(StrName);
			}		
		}
		else
		{
			ASSERT( 0 );
		}
	}
	else if ( m_aTrendMultiDayIOCTimes.GetSize() > 1 && bValidMerch )
	{
		// 多日分时名称 商品[最近n日分时] , 此时商品等必不为NULL
		CString StrName;
		StrName.Format(_T("%s【最近%d日分时】"), m_pMerchXml->m_MerchInfo.m_StrMerchCnName.GetBuffer(), m_aTrendMultiDayIOCTimes.GetSize());

		if (m_bShowTopToolBar)
		{
			//
			CString StrCaption;
			StrCaption.Format(L"%d 日分时", m_aTrendMultiDayIOCTimes.GetSize());
		//	m_mapTrendRightBtn[TOP_TREND_MORE_TREND].SetCaption(StrCaption);

			// m_mapTrendRightBtn[TOP_TREND_MORE_TREND].SetCaption(StrName)
			// SetTopCenterTitle(StrName);
		}
		/*else*/
		{
			m_pRegionMain->SetTitle(StrName);
		}
	}
	else if ( bValidMerch )
	{
		if (m_bShowTopToolBar)
		{
			// SetTopCenterTitle(m_pMerchXml->m_MerchInfo.m_StrMerchCnName);
			//m_mapTrendRightBtn[TOP_TREND_MORE_TREND].SetCaption(L"当日分时");
		}
		/*else*/
		{
			m_pRegionMain->SetTitle(m_pMerchXml->m_MerchInfo.m_StrMerchCnName);
		}
	}
}

void CIoViewTrend::SetSingleSection( bool32 bSingle )
{
	// 当前时间所在的开收盘时间, init-第一个开场属于第一个开场
	//		最后一个收场到End时间属于最后一个开场段
	// 仅给国内股票&指数提供
	bool32 bOld = m_bSingleSectionTrend;
	if ( bSingle && IsEnableSingleSection() )
	{
		// 要求设置半场走势，取消其它的模式

		// 取消多日分时, 注意要先取消多日分时, 后设置标志
		SetTrendMultiDay(1, true, false);

		m_bSingleSectionTrend = true;
	}
	else if ( bSingle )
	{
		// 要求设置半场走势，但是现在的模式不允许
		ASSERT( 0 );
		m_bSingleSectionTrend = false;
	}
	else
	{
		// 取消半场走势
		m_bSingleSectionTrend = false;
	}

	if ( (bOld && !m_bSingleSectionTrend)
		|| (!bOld && m_bSingleSectionTrend) )
	{
		// 变更了

		UpdateTrendMultiDayChange();
		OnVDataMerchKLineUpdate(m_pMerchXml);	// K线数据变更
		if ( NULL != m_pRegionMain )
		{			
			if ( m_MerchParamArray.GetSize() > 0 && m_MerchParamArray[0] != NULL && ETBT_CompetePrice == GetCurTrendBtnType() )
			{
				OnVDataMerchAuctionUpdate(m_MerchParamArray[0]->m_pMerchNode );
			}
			m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
			Invalidate();
		}
	}
}

bool32 CIoViewTrend::IsEnableSingleSection()
{
	if ( m_MerchParamArray.GetSize() <= 0 || m_MerchParamArray[0] == NULL || m_MerchParamArray[0]->m_pMerchNode == NULL )
	{
		return false;
	}

	T_MerchNodeUserData *pData = m_MerchParamArray[0];

	ASSERT( pData->m_TrendTradingDayInfo.m_bInit );

	/*E_ReportType eType = */GetMerchKind(pData->m_pMerchNode);

	if ( m_bHistoryTrend )
	{
		ASSERT( 0 );	// 历史分时下，不允许
		//return false;
	}

	//if ( pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_aOCTimes.GetSize() != 4 )
	//{
	//	// 不能分为上、下半场
	//	return false;
	//}

	//if ( m_aTrendMultiDayIOCTimes.GetSize() > 1 )
	//{
	//	// 现处于多日分时下
	//	return false;
	//}

	return true;
}

bool32 CIoViewTrend::GetTrendFirstDayOpenCloseTime( OUT CGmtTime &TimeOpen, OUT CGmtTime &TimeClose )
{
	bool32 bValidTrend = m_MerchParamArray.GetSize() > 0 
		&& m_MerchParamArray[0] != NULL
		&& m_MerchParamArray[0]->m_TrendTradingDayInfo.m_bInit;

	if ( !m_bSingleSectionTrend 
		&& bValidTrend )
	{
		// 非半场走势
		TimeOpen = m_MerchParamArray[0]->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeOpen.m_Time;
		TimeClose   = m_MerchParamArray[0]->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeClose.m_Time;
		// 仅获取第一天的
		// 		if ( m_aTrendMultiDayIOCTimes.GetSize() > 0 )
		// 		{
		// 			TimeOpen	= m_aTrendMultiDayIOCTimes[m_aTrendMultiDayIOCTimes.GetUpperBound()].m_TimeOpen.m_Time;
		// 			TimeClose   = m_aTrendMultiDayIOCTimes[0].m_TimeClose.m_Time;
		// 		}
		return true;
	}
	else if ( m_bSingleSectionTrend
		&& bValidTrend )
	{
		// 看当前时间属于哪个时间段
		CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();
		const CMarketIOCTimeInfo &IOCTime = m_MerchParamArray[0]->m_TrendTradingDayInfo.m_MarketIOCTime;

		// 有可能今天非开盘日，则ioctime实际上为上个开盘日信息
		for ( int32 i=0; i < IOCTime.m_aOCTimes.GetSize() ; i+=2 )
		{
			CGmtTime TimeOpenInner;
			CGmtTime TimeCloseInner;

			if ( 0 == i )
			{
				// 第一个开盘点, 取第一个开盘时间
				TimeOpenInner = IOCTime.m_TimeOpen.m_Time;
			}
			else
			{
				TimeOpenInner = IOCTime.m_aOCTimes[i];
			}

			if ( i >= IOCTime.m_aOCTimes.GetSize() - 2 )
			{
				// 最后一个开收 包括所有剩下时间
				TimeCloseInner = IOCTime.m_TimeEnd.m_Time;
				if ( (TimeNow >= TimeOpenInner && TimeNow <= TimeCloseInner)
					|| TimeNow > TimeCloseInner )		// 非开盘日信息，落到最后一个开盘日的收盘段
				{
					TimeOpen = TimeOpenInner;
					TimeClose   = IOCTime.m_TimeClose.m_Time;	// 收盘时间
					return true;	// 取这个开收盘时间了
				}
			}
			else
			{
				// 中间的开收，结束时间取到下一个开盘点
				TimeCloseInner = IOCTime.m_aOCTimes[i+2];
				if ( TimeNow >= TimeOpenInner && TimeNow < TimeCloseInner )	// 下一个开盘点不包含
				{
					TimeOpen = TimeOpenInner;
					TimeClose   = TimeCloseInner;
					return true;	// 取这个开收盘时间了
				}
			}

		}
	}

	ASSERT( 0 );
	return false;	// 不应该调用的
}

bool32 CIoViewTrend::GetTrendStartEndTime( OUT CGmtTime &TimeStart, OUT CGmtTime &TimeEnd )
{
	bool32 bValidTrend = m_MerchParamArray.GetSize() > 0 
		&& m_MerchParamArray[0] != NULL
		&& m_MerchParamArray[0]->m_TrendTradingDayInfo.m_bInit;

	if ( !m_bSingleSectionTrend 
		&& bValidTrend )
	{
		// 非半场走势
		TimeStart = m_MerchParamArray[0]->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeOpen.m_Time;
		TimeEnd   = m_MerchParamArray[0]->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeEnd.m_Time;
		if ( m_aTrendMultiDayIOCTimes.GetSize() > 0 )
		{
			TimeStart = m_aTrendMultiDayIOCTimes[m_aTrendMultiDayIOCTimes.GetUpperBound()].m_TimeOpen.m_Time;
			TimeEnd   = m_aTrendMultiDayIOCTimes[0].m_TimeEnd.m_Time;
		}
		return true;
	}
	else if ( m_bSingleSectionTrend
		&& bValidTrend )
	{
		// 看当前时间属于哪个时间段
		CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();
		const CMarketIOCTimeInfo &IOCTime = m_MerchParamArray[0]->m_TrendTradingDayInfo.m_MarketIOCTime;

		// 有可能今天非开盘日，则ioctime实际上为上个开盘日信息
		for ( int32 i=0; i < IOCTime.m_aOCTimes.GetSize() ; i+=2 )
		{
			CGmtTime TimeOpenInner;
			CGmtTime TimeCloseInner;

			if ( 0 == i )
			{
				// 第一个开盘点, 取市场初始化时间
				TimeOpenInner = IOCTime.m_TimeInit.m_Time;
			}
			else
			{
				TimeOpenInner = IOCTime.m_aOCTimes[i];
			}

			if ( i >= IOCTime.m_aOCTimes.GetSize() - 2 )
			{
				// 最后一个开收 包括所有剩下时间
				TimeCloseInner = IOCTime.m_TimeEnd.m_Time;
				if ( (TimeNow >= TimeOpenInner && TimeNow <= TimeCloseInner)
					|| TimeNow > TimeCloseInner )		// 非开盘日信息，落到最后一个开盘日的收盘段
				{
					TimeStart = TimeOpenInner;
					TimeEnd   = TimeCloseInner;
					return true;	// 取这个开收盘时间了
				}
			}
			else
			{
				// 中间的开收，结束时间取到下一个开盘点
				TimeCloseInner = IOCTime.m_aOCTimes[i+2];
				if ( TimeNow >= TimeOpenInner && TimeNow < TimeCloseInner )	// 下一个开盘点不包含
				{
					TimeStart = TimeOpenInner;
					TimeEnd   = TimeCloseInner;
					return true;	// 取这个开收盘时间了
				}
			}

		}
	}

	ASSERT( 0 );
	return false;	// 不应该调用的
}

bool32 CIoViewTrend::GetTrendSingleSectionOCTime( OUT CGmtTime &TimeOpen, OUT CGmtTime &TimeClose )
{
	bool32 bValidTrend = m_MerchParamArray.GetSize() > 0 
		&& m_MerchParamArray[0] != NULL
		&& m_MerchParamArray[0]->m_TrendTradingDayInfo.m_bInit;
	if ( m_bSingleSectionTrend && bValidTrend )
	{
		// 看当前时间属于哪个时间段
		CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();
		const CMarketIOCTimeInfo &IOCTime = m_MerchParamArray[0]->m_TrendTradingDayInfo.m_MarketIOCTime;

		for ( int32 i=0; i < IOCTime.m_aOCTimes.GetSize() ; i+=2 )
		{
			CGmtTime TimeOpenInner;
			CGmtTime TimeCloseInner;

			if ( 0 == i )
			{
				// 第一个开盘点, 取市场初始化时间
				TimeOpenInner = IOCTime.m_TimeInit.m_Time;
			}
			else
			{
				TimeOpenInner = IOCTime.m_aOCTimes[i];
			}

			if ( i >= IOCTime.m_aOCTimes.GetSize() - 2 )
			{
				// 最后一个开收 包括所有剩下时间
				TimeCloseInner = IOCTime.m_TimeEnd.m_Time;
				if ( TimeNow >= TimeOpenInner && TimeNow <= TimeCloseInner
					|| TimeNow > TimeCloseInner )
				{
					TimeOpen = IOCTime.m_aOCTimes[i];
					TimeClose = IOCTime.m_aOCTimes[i+1];
					return true;	// 取这个开收盘时间了
				}
			}
			else
			{
				// 中间的开收，结束时间取到下一个开盘点
				TimeCloseInner = IOCTime.m_aOCTimes[i+2];
				if ( TimeNow >= TimeOpenInner && TimeNow < TimeCloseInner )	// 下一个开盘点不包含
				{
					TimeOpen = IOCTime.m_aOCTimes[i];
					TimeClose = IOCTime.m_aOCTimes[i+1];
					return true;	// 取这个开收盘时间了
				}
			}

		}
	}

	return false;
}

//lint --e{429}
bool32 CIoViewTrend::AddCmpMerch( CMerch *pMerchToAdd, bool32 bPrompt, bool32 bReqData )
{
	// 检查是否已存在
	ASSERT( m_MerchParamArray.GetSize() > 0 );
	ASSERT( m_pMerchXml != NULL );
	if ( m_pMerchXml == NULL || m_MerchParamArray.GetSize() <= 0 )
	{
		return false;
	}

	// 分时图只有百分比坐标
	ASSERT( GetMainCurveAxisYType() == CPriceToAxisYObject::EAYT_Pecent );


	for ( int32 i=0; i < m_MerchParamArray.GetSize() ; i++ )
	{
		if ( m_MerchParamArray[i]->m_pMerchNode == pMerchToAdd )
		{
			if ( bPrompt )
			{
				MessageBox(_T("叠加品种已经存在"), _T("提示"));
			}
			return false;
		}
	}

	if ( bPrompt )
	{
		// 检查并询问是否要更换到百分比坐标
	}

	CArray<COLORREF, COLORREF> aClrs;
	aClrs.SetSize(3); 
	aClrs[2] = RGB(127,127,127);
	aClrs[1] = RGB(128,64,0);
	aClrs[0] = RGB(0,150, 84);
	if ( m_MerchParamArray.GetSize() >= 4 )
	{
		// 超过最大值，1+3
		if ( bPrompt )
		{
			MessageBox(_T("已经超过允许的最大叠加商品数!"), _T("叠加提示"));
		}
		return false;
	}

	COLORREF clrLine = aClrs[ (m_MerchParamArray.GetSize()-1)%aClrs.GetSize() ];

	// 加入商品列表，加入关注，请求叠加商品数据，是否需要除权
	bool32 bInherit;
	T_MerchNodeUserData *pData = NewMerchData(pMerchToAdd, false, bInherit);
	ASSERT( NULL != pData );
	if ( NULL != pData )
	{
		CMarketIOCTimeInfo RecentTradingDay;
		if ( !m_bHistoryTrend )
		{
			if (pMerchToAdd->m_Market.GetRecentTradingDay(m_pAbsCenterManager->GetServerTime(), RecentTradingDay, pMerchToAdd->m_MerchInfo))
			{
				pData->m_TrendTradingDayInfo.Set(pMerchToAdd, RecentTradingDay);
				pData->m_TrendTradingDayInfo.RecalcPrice(*pMerchToAdd);
				pData->m_TrendTradingDayInfo.RecalcHold(*pMerchToAdd);

				// zhangbo 20090708 #待补充， 中轴更新
				//...
			}
		}
		else
		{
			// 历史分时 - 设定交易日信息(历史分时严格意义上不允许叠加商品)
			if ( pMerchToAdd->m_Market.GetSpecialTradingDayTime(m_TimeHistory, RecentTradingDay, pMerchToAdd->m_MerchInfo) )
			{
				pData->m_TrendTradingDayInfo.Set(pMerchToAdd, RecentTradingDay);
				pData->m_TrendTradingDayInfo.RecalcPrice(*pMerchToAdd);
				pData->m_TrendTradingDayInfo.RecalcHold(*pMerchToAdd);
			}
		}
		if ( !bInherit )
		{
			// 创建需要的线 画等
			CNodeSequence* pNodes = CreateNodes();
			pNodes->SetUserData(pData);
			pData->m_pKLinesShow = pNodes;

			// new 1 curves/nodes/...
			CChartCurve* pCurve = m_pRegionMain->CreateCurve(CChartCurve::KRequestCurve|CChartCurve::KTypeTrend|CChartCurve::KYTransformByClose|CChartCurve::KUseNodesNameAsTitle|CChartCurve::KYTransformToAlignDependent);		 
			CChartDrawer* pCmpDrawer = new CChartDrawer(*this, CChartDrawer::EKDSTrendPrice);
			pCmpDrawer->SetSingleColor(clrLine);

			pCurve->AttatchDrawer(pCmpDrawer);
			pCurve->AttatchNodes(pNodes);
			pCurve->m_clrTitleText = clrLine;
		}

		// 加入关注
		CSmartAttendMerch AttendMerch;
		AttendMerch.m_pMerch = pMerchToAdd;
		AttendMerch.m_iDataServiceTypes = m_iDataServiceType;
		m_aSmartAttendMerchs.Add(AttendMerch);

		if ( bReqData )
		{
			RequestSingleCmpMerchViewData(pMerchToAdd);
		}

		return true;

		// 此时还是没有设置标题的
	}

	return false;
}

void CIoViewTrend::RemoveCmpMerch( CMerch *pMerch )
{
	// 从队列中删除，从关注中删除
	int32 iDel = 0;
	for ( int32 i=m_MerchParamArray.GetUpperBound(); i >= 0 ; i-- )
	{
		if ( m_MerchParamArray[i] != NULL
			&& !m_MerchParamArray[i]->bMainMerch
			&& m_MerchParamArray[i]->m_pMerchNode == pMerch)
		{
			DeleteMerchData(i);
		}
	}
	if ( iDel > 0 )
	{
		for ( int32 i=m_aSmartAttendMerchs.GetUpperBound(); i >= 0; i-- )
		{
			if ( m_aSmartAttendMerchs[i].m_pMerch == pMerch )
			{
				m_aSmartAttendMerchs.RemoveAt(i);
			}
		}
		if ( NULL != m_pRegionMain )
		{
			m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
			Invalidate();
		}
	}
}

void CIoViewTrend::RemoveAllCmpMerch()
{
	// 删除所有叠加的商品
	int32 iDel = 0;
	for ( int32 i=m_MerchParamArray.GetUpperBound(); i >= 0 ; i-- )
	{
		if ( m_MerchParamArray[i] != NULL
			&& !m_MerchParamArray[i]->bMainMerch )
		{
			DeleteMerchData(i);
			iDel++;
		}
	}

	if ( iDel > 0 )
	{
		//
		UpdateAxisSize(false);

		for ( int32 i=m_aSmartAttendMerchs.GetUpperBound(); i >= 0; i-- )
		{
			if ( m_aSmartAttendMerchs[i].m_pMerch != m_pMerchXml )
			{
				m_aSmartAttendMerchs.RemoveAt(i);
			}
		}
		if ( NULL != m_pRegionMain )
		{
			m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
			Invalidate();
		}
	}
}

void CIoViewTrend::RequestSingleCmpMerchViewData( CMerch *pMerch )
{
	T_MerchNodeUserData *pMainData = NULL;
	if ( 
		pMerch == NULL
		|| m_MerchParamArray.GetSize() <= 0 
		|| (pMainData=m_MerchParamArray[0]) == NULL 
		|| pMainData->m_pMerchNode == NULL )
	{
		return;
	}

	T_MerchNodeUserData *pThisData = NULL;
	for ( int32 i=0; i < m_MerchParamArray.GetSize() ; i++ )
	{
		if ( m_MerchParamArray[i] != NULL && m_MerchParamArray[i]->m_pMerchNode == pMerch  )
		{
			pThisData = m_MerchParamArray[i];
			ASSERT( !pThisData->bMainMerch );	// 是用来设计给叠加商品第一次请求数据用的
			break;
		}
	}

	if ( NULL == pThisData )
	{
		ASSERT( 0 );
		return;
	}

	//先发RealtimePrice请求
	{
		CMmiReqRealtimePrice Req;
		Req.m_iMarketId			= pMerch->m_MerchInfo.m_iMarketId;
		Req.m_StrMerchCode		= pMerch->m_MerchInfo.m_StrMerchCode;
		DoRequestViewData(Req);
	}

	// 一切请求时间以主商品为准
	{
		ASSERT(pMainData->m_TrendTradingDayInfo.m_bInit);
		ASSERT(pThisData->m_TrendTradingDayInfo.m_bInit);

		// 时间点以主数据为准, 不在主数据内的请求了也没用
		// 要是叠加的视图与主数据不是同开盘时间，即使有错误显示，应当也不重要，暂时先不考虑
		// 发昨天和今天的日k线请求
		if ( m_iTrendDayCount <= TREND_MIN_DAY_COUNT-1 )
		{
			ASSERT( 0 );
			m_iTrendDayCount = TREND_MIN_DAY_COUNT;
		}
		ASSERT( m_iTrendDayCount <= TREND_MAX_DAY_COUNT );

		// 今天
		CGmtTime TimeDay = pMainData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeClose.m_Time;
		SaveDay(TimeDay);

		CMmiReqMerchKLine info;
		info.m_eKLineTypeBase	= EKTBDay;
		info.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
		info.m_StrMerchCode		= pMerch->m_MerchInfo.m_StrMerchCode;
		info.m_eReqTimeType		= ERTYFrontCount;
		info.m_TimeStart		= TimeDay;
		info.m_iFrontCount			= 1 + m_iTrendDayCount;			// 请求n+1 !!这里会影响到历史分时！
		DoRequestViewData(info);
	}

	// 发送分钟线请求
	// 如果是多日分时，需要等到日k线回来才能确定要请求具体什么时间段的分钟k线
	{
		RequestMinuteKLineData(*pThisData);	// 请求多日k线
	}
}

bool32 CIoViewTrend::CalcKLinePriceBaseValue( CChartCurve *pCurve, OUT float &fPriceBase )
{
	// 主线与叠加线使用他们自己的基础值，指标线使用与主线相同的基础值
	fPriceBase = CChartCurve::GetInvalidPriceBaseYValue();
	if ( NULL == pCurve )
	{
		return false;
	}
	// 使用视图的坐标类型，而不是线的坐标类型计算
	CPriceToAxisYObject::E_AxisYType eType = GetMainCurveAxisYType();
	if ( CPriceToAxisYObject::EAYT_Pecent == eType )
	{
		// 百分比，商品的昨收价或者昨结算
		CNodeData node;
		// 如果是指标线，则用主线的
		if ( CheckFlag(pCurve->m_iFlag, CChartCurve::KTypeIndex) )
		{
			ASSERT( 0 );	// 不应该重复计算的
			if ( NULL == m_pRegionMain || (pCurve=m_pRegionMain->GetDependentCurve()) == NULL )
			{
				return false;
			}
		}

		// 找到这个curve对应的merchdData
		CNodeSequence *pNodes = pCurve->GetNodes();
		if ( NULL == pNodes )
		{
			ASSERT( 0 );
			return false;
		}

		T_MerchNodeUserData *pData = (T_MerchNodeUserData *)pNodes->GetUserData();
		if ( NULL == pData )
		{
			ASSERT( 0 );
			return false;
		}

		ASSERT( pData->m_TrendTradingDayInfo.m_bInit );
		if ( pData->bMainMerch )
		{
			fPriceBase = GetTrendPrevClose();
		}
		else
		{
			fPriceBase = pData->m_TrendTradingDayInfo.GetPrevReferPrice();
		}
		return fPriceBase != 0.0f;
	}
	else
	{
		ASSERT( 0 );	// 不支持其它任何类型
	}


	return false;
}

void CIoViewTrend::OnShowDataTimeRangeChanged( T_MerchNodeUserData *pData )
{
	if ( m_MerchParamArray.GetSize() <= 0 || m_MerchParamArray[0] == NULL )
	{
		return;
	}

	if ( pData == m_MerchParamArray[0] )
	{
		ASSERT( pData->bMainMerch );	// 主图数据
		// 将叠加商品的更新队列加进定时器更新
		// 显示时间段已经变更，必须设置延时更新叠加K线
		// 所有叠加商品都加入
		OnCmpMerchsNeedUpdateAsync();
	}
}

bool32 CIoViewTrend::CalcRiseFallMaxPrice( OUT float& fRiseMax, OUT float& fFallMax )
{
	fRiseMax = 0.;
	fFallMax = 0.;

	//
	if ( NULL == m_pMerchXml || NULL == m_pMerchXml->m_pRealtimePrice || m_pMerchXml->m_MerchInfo.m_StrMerchCnName.GetLength() <= 0 )
	{
		return false;
	}

	E_ReportType ert = GetMerchKind(m_pMerchXml);
	if ( ert != ERTStockCn
		&& ert != ERTExp )
	{
		// 非国内股票&指数不适合涨跌停 
		return false;
	}

	/*
	涨跌停计算：
	1、所有的股票+指数+基金  
	涨停价： 昨收+昨收*10%（最后一位四舍五入）
	跌停价： 昨收-昨收*10%（最后一位四舍五入）
	2、名称第一个字母是"N "
	不计算涨跌停
	3、名称前两位“XR、XD、DR”的股票是除权除息的，昨收盘必须用报价表中的昨收盘计算，不能用K线图中的昨收计算
	涨停价： 昨收+昨收*10%（最后一位四舍五入）
	跌停价： 昨收-昨收*10%（最后一位四舍五入）
	4、名称包含“ST”(ST、*ST、 SST、S*ST)
	涨停价： 昨收+昨收*5%（最后一位四舍五入）
	跌停价： 昨收-昨收*5%（最后一位四舍五入）
	*/

	float fRate		= 0.1f;
	float fClosePre	= m_pMerchXml->m_pRealtimePrice->m_fPricePrevClose;

	//
	if ( 'N' == m_pMerchXml->m_MerchInfo.m_StrMerchCnName[0] )
	{
		// 新股 不用计算涨跌停, 理解为无涨跌停
		return false;
	}


	//
	if ( -1 != m_pMerchXml->m_MerchInfo.m_StrMerchCnName.Find(L"ST") )
	{
		fRate = 0.05f;
	}

	// 
	float fTmp = fClosePre * fRate;

	// 四舍五入, 保留两位小数
	//fTmp = (int32)((fTmp * 100) + 0.5) / 100.0f;

	//
	fRiseMax = fClosePre + fTmp;
	fFallMax = fClosePre - fTmp;

	return true;
}

bool32 CIoViewTrend::GetViewRegionRect( OUT LPRECT pRect )
{
	if ( NULL != pRect )
	{
		GetClientRect(pRect);
		if (m_bShowTopToolBar)
		{
			pRect->top +=(m_iTopMerchHeight + m_iTopButtonHeight);
		}

		// 下面预留为按钮预留
		if (m_bShowIndexToolBar && m_iChartType!=7)
		{
			pRect->bottom -= m_iTopButtonHeight;
		}

		if ( pRect->bottom < pRect->top )
		{
			pRect->bottom = pRect->top;
		}
		return true;
	}
	return false;
}

void CIoViewTrend::Draw()
{
	if (!IsWindowVisible())
	{
		return;
	}

	//
	CIoViewChart::Draw();

	// 绘制自己的一栏按钮
	DrawBottomBtns();
}

//lint --e{429}
bool32 CIoViewTrend::InitRedGreenCurve()
{
	if ( NULL == m_pRedGreenCurve && m_MerchParamArray.GetSize() > 0 && m_MerchParamArray[0] != NULL )
	{
		// 新建一条红绿柱状线，只有指数时才赋值，其它时候不赋值, 仅赋值收盘价
		CNodeSequence *pNodes = new CNodeSequence();
		pNodes->SetUserData(m_MerchParamArray[0]);
		CChartCurve *pCurveRedGreen = m_pRegionMain->CreateCurve(CChartCurve::KTypeTrend |CChartCurve::KYTransformByClose |CChartCurve::KDonotPick |CChartCurve::KTimeIdAlignToXAxis);
		pCurveRedGreen->SetTitle(_T(""));
		//lint --e(569)
		CIndexDrawer *pRedGreenDrawer = new CIndexDrawer(*this, CIndexDrawer::EIDSColorStick, CIndexDrawer::EIDSENone, 1, CLR_DEFAULT);
		pCurveRedGreen->AttatchDrawer(pRedGreenDrawer);
		pCurveRedGreen->AttatchNodes(pNodes);
		m_pRedGreenCurve = pCurveRedGreen;
		m_pRedGreenCurve->SetAxisYType(GetMainCurveAxisYType());
	}
	return NULL != m_pRedGreenCurve;
}

void CIoViewTrend::CalcRedGreenValue()
{
	if ( !InitRedGreenCurve() )
	{
		ASSERT( 0 );
		return;
	}

	CalcNoWeightExpLine();	// 重算不含权线, 一定

	T_MerchNodeUserData *pData = NULL;
	if ( m_MerchParamArray.GetSize() > 0 )
	{
		pData = m_MerchParamArray[0];
	}
	if ( NULL == pData || NULL == pData->m_pMerchNode )
	{
		return;
	}

	if ( m_bHistoryTrend )
	{
		return;	// 历史不显示红绿
	}

	CalcLayoutRegions(true);	// 此处会更新主图最大最小值！！

	ASSERT( GetMainCurveAxisYType() == CPriceToAxisYObject::EAYT_Pecent );	// 必是百分比坐标

	E_ReportType eMerchType = GetMerchKind(pData->m_pMerchNode);
	CNodeSequence *pNodes = m_pRedGreenCurve->GetNodes();
	float fMaxAxisY, fMinAxisY, fBaseAxisY;
	bool32 bMatchMerch = IsRedGreenMerch(pData->m_pMerchNode);
	const CAbsCenterManager::GeneralNormalArray *pGeneralNormals = m_pAbsCenterManager->GetGeneralNormalArray(pData->m_pMerchNode);
	if ( 
		NULL != pGeneralNormals
		&& ERTExp == eMerchType
		&& bMatchMerch
		&& NULL != pNodes
		&& m_pRedGreenCurve->GetYMinMax(fMinAxisY, fMaxAxisY)
		&& m_pRedGreenCurve->GetAxisBaseY(fBaseAxisY)
		&& fMaxAxisY > fMinAxisY )
	{
		// 指数红绿线数据计算
		const float fMax = fBaseAxisY + (fMaxAxisY-fBaseAxisY)/3;	// 1/3处高度
		//const float fMin = fBaseAxisY - (fBaseAxisY-fMinAxisY)/3;

		//CMarket &Market = pData->m_pMerchNode->m_Market;

		// 将相关值映射到坐标范围内
		const int32 iSize = pGeneralNormals->GetSize();
		pNodes->GetNodes().SetSize(iSize);
		CNodeData *pNodeDatas = pNodes->GetNodes().GetData();
		const T_GeneralNormal *pSrcData = pGeneralNormals->GetData();
		float fCurveMax = -FLT_MAX;
		float fCurveMin = FLT_MAX;
		int32 i = 0;
		for ( i=0; i < iSize ; i++ )
		{
			// 全把他赋值到close中
			float fCur = pSrcData[i].m_fAmountBuy - (float)pSrcData[i].m_fAmountSell;
			float fTotal = pSrcData[i].m_fAmountBuy+pSrcData[i].m_fAmountSell;
			if ( fTotal == 0.0f )
			{
				fCur = 0.0f;
			}
			else
			{
				fCur /= fTotal;
			}
			pNodeDatas[i].m_fClose = fCur;

			CGmtTime Time1(pSrcData[i].m_lTime);
			SaveMinute(Time1);
			pNodeDatas[i].m_iID = Time1.GetTime();
		}

		// 截断 - 截断源数据
		TrimPlusShowNodeData(pNodes);
		for ( i=0; i < iSize ; i++ )
		{
			float fCur = pNodeDatas[i].m_fClose;
			if ( fCur > fCurveMax )
			{
				fCurveMax = fCur;
			}
			if ( fCur < fCurveMin )
			{
				fCurveMin = fCur;
			}
		}

		// 映射
		if ( fCurveMax >= fCurveMin )
		{
			// 对称
			float fTmp1, fTmp2;
			fTmp1 = fabsf(fCurveMax);
			fTmp2 = fabsf(fCurveMin);
			fCurveMax = max(fTmp1, fTmp2);
			fCurveMin = -fCurveMax;
			const float fCurveRange = fCurveMax;
			const float fAxisRange = fMax - fBaseAxisY;
			if ( fCurveRange > 0.0f
				&& fAxisRange > 0.0f )
			{				
				float fBasePrice;		// 图形中的基础价格
				m_pRedGreenCurve->AxisYToPriceY(fBaseAxisY, fBasePrice);
				for ( int32 i=0; i < iSize ; i++ )
				{
					float fCur = pNodeDatas[i].m_fClose;
					fCur = fBaseAxisY + (fCur)/fCurveRange*fAxisRange;
					m_pRedGreenCurve->AxisYToPriceY(fCur, pNodeDatas[i].m_fClose);
				}
			}
			else
			{
				// 全部都是一条直线- -
				// ASSERT( 0 );
				float fBasePrice;
				m_pRedGreenCurve->AxisYToPriceY(fBaseAxisY, fBasePrice);
				for ( int32 i=0; i < iSize ; i++ )
				{
					pNodeDatas[i].m_fClose = fBasePrice;	// 全部都是基础价格
				}
			}

			// 截断
			TrimPlusShowNodeData(pNodes);
		}
		else
		{
			// 没有有效值, 清除
			pNodes->RemoveAll();
		}

	}
	else if ( NULL != pNodes )
	{
		pNodes->RemoveAll();
	}
}

void CIoViewTrend::OnSize( UINT nType, int cx, int cy )
{
	CIoViewChart::OnSize(nType, cx, cy);

	RecalcBtnRect();

	if (m_hWnd && !m_strEconoData.empty())
	{
		m_bSizeChange = true;
	}
}

void CIoViewTrend::InitMerchBtns()
{
	
	if ( s_aInitTrendBtns.GetSize() <= 0 )
	{
		s_aInitTrendBtns.Add(T_TrendBtn(ETBT_Volumn, ERTStockCn, this));
		s_aInitTrendBtns.Add(T_TrendBtn(ETBT_Index, ERTStockCn, this));
		s_aInitTrendBtns.Add(T_TrendBtn(ETBT_VolRate, ERTStockCn, this));
		s_aInitTrendBtns.Add(T_TrendBtn(ETBT_BuySellForce, ERTStockCn, this));
		s_aInitTrendBtns.Add(T_TrendBtn(ETBT_CompetePrice, ERTStockCn, this));
		s_aInitTrendBtns.Add(T_TrendBtn(ETBT_WindWarningLine, ERTStockCn, this));

		//s_aInitTrendBtns.Add(T_TrendBtn(ETBT_Amount, ERTExp, this));
		s_aInitTrendBtns.Add(T_TrendBtn(ETBT_Volumn, ERTExp, this));
		s_aInitTrendBtns.Add(T_TrendBtn(ETBT_Index, ERTExp, this));
		s_aInitTrendBtns.Add(T_TrendBtn(ETBT_DuoKong, ERTExp, this));
		s_aInitTrendBtns.Add(T_TrendBtn(ETBT_RiseFallRate, ERTExp, this));

		s_aInitTrendBtns.Add(T_TrendBtn(ETBT_Volumn, ERTFuturesCn, this));
		s_aInitTrendBtns.Add(T_TrendBtn(ETBT_Index, ERTFuturesCn, this));
		//s_aInitTrendBtns.Add(T_TrendBtn(ETBT_PriceModel, ERTFuturesCn, this));

		s_aInitTrendBtns.Add(T_TrendBtn(ETBT_Volumn, ERTCount, this));
		s_aInitTrendBtns.Add(T_TrendBtn(ETBT_Index, ERTCount, this));
	}


	E_ReportType eMerchType = ERTCount;
	if ( NULL != m_pMerchXml )
	{
		eMerchType = GetMerchKind(m_pMerchXml);

		int32 iBtnIndex;
		if ( m_mapMerchTypeBtnIndex.Lookup(eMerchType, iBtnIndex) )
		{
			m_iBtnCur = iBtnIndex;	// 使用存储的
		}
	}
	if ( eMerchType != ERTStockCn
		&& eMerchType != ERTExp
		&& eMerchType != ERTFuturesCn )
	{
		eMerchType = ERTStockCn;	// 没有明确指出的类型，统一默认为股票
	}

	m_aTrendBtnCur.RemoveAll();
	for ( int32 i=0; i < s_aInitTrendBtns.GetSize() ; i++ )
	{
		if ( eMerchType == s_aInitTrendBtns[i].m_eMerchType )
		{
			s_aInitTrendBtns[i].SetParent(this);
			m_aTrendBtnCur.Add(s_aInitTrendBtns[i]);
		}
	}

	if ( m_iBtnCur >= m_aTrendBtnCur.GetSize()
		|| m_iBtnCur < 0 )
	{
		m_iBtnCur = 0;	// 第一个
	}

	int32 iSize = m_aTrendBtnCur.GetSize();
	for (int32 i=0; i<iSize; i++)
	{
		CPolygonButton &btnControl = m_aTrendBtnCur[i].m_btnPolygon;
		if (btnControl.GetCheck())
		{
			btnControl.MouseLeave(false);
			btnControl.SetCheckStatus(FALSE);
		}
	}

	bool32 bCheck = m_aTrendBtnCur[m_iBtnCur].m_btnPolygon.GetCheck();
	if (!bCheck)
	{
		m_aTrendBtnCur[m_iBtnCur].m_btnPolygon.SetCheckStatus(TRUE,FALSE);
	}
	// 此时当前选择的btn应该是变更了, 由于调用时机不同，这里不处理
}



void CIoViewTrend::InitNoShowBtns()
{
	if (m_mapTrendRightBtn.count(TOP_TREND_BID) > 0)
	{
		bool bIsMerch = false;
		for (unsigned i =0; i < m_aTrendBtnCur.GetSize(); ++i)
		{		
			if (ETBT_CompetePrice + BTN_ID_BEGIN == m_aTrendBtnCur[i].m_iID)
			{
				bIsMerch = true;
				break;
			}
		}
		if (!bIsMerch)
		{
			//m_mapTrendRightBtn.erase(TOP_TREND_BID);
			m_mapTrendRightBtn[TOP_TREND_BID].SetVisiable(FALSE);
		}
		else
		{
			if (m_mapTrendRightBtn[TOP_TREND_BID].IsVisiable())
			{
				return;
			}
			m_mapTrendRightBtn[TOP_TREND_BID].SetVisiable(true);
		}
	}
}

E_TrendBtnType CIoViewTrend::GetCurTrendBtnType()
{
	if (  m_iBtnCur >= 0 && m_iBtnCur < m_aTrendBtnCur.GetSize() )
	{
		return m_aTrendBtnCur[m_iBtnCur].m_eTrendBtnType;
	}
	return ETBT_Count;
}

void CIoViewTrend::OnTrendBtnSelChanged(bool32 bReqData/* = true*/)
{
	// 
	E_TrendBtnType eBtnCur = GetCurTrendBtnType();
	if ( NULL == m_pMerchXml )
	{
		return;
	}

	E_ReportType eRT = GetMerchKind(m_pMerchXml);
	m_mapMerchTypeBtnIndex[ eRT ] = m_iBtnCur;

	if ( ETBT_Index == eBtnCur )
	{
		CString StrTypeIndexName;
		if ( m_mapMerchTypeIndexName.Lookup(eRT, StrTypeIndexName) )
		{
			if (L"风警线" != StrTypeIndexName)
			{
				m_StrBtnIndexName = StrTypeIndexName;	// 如果有该类型的指标存储了，使用存储的，否则继承上一个类型
			}
		}
		if ( InitBtnIndex() )	// 初始化指标
		{
			AddIndexToSubRegion(m_StrBtnIndexName);
		}
		else
		{
			ASSERT( 0 );
			RemoveSubRegion();
		}
	}
	else if ( ETBT_VolRate == eBtnCur )
	{
		// 添加量比指标
		//AddIndexToSubRegion(_T("量比"));
		//AddIndexToSubRegion(_T("MACD"));
		AddSpecialCurveToSubRegion();
		if ( bReqData )
		{
			RequestTrendPlusData();
		}
	}
	else if ( ETBT_BuySellForce == eBtnCur )
	{
		// 买卖力道指标
		//AddIndexToSubRegion(_T("买卖力道"));
		//AddIndexToSubRegion(_T("VOL"));
		AddSpecialCurveToSubRegion();
		if ( bReqData )
		{
			RequestTrendPlusData();
		}
	}
	else if ( ETBT_CompetePrice == eBtnCur )
	{
		ASSERT( eRT == ERTStockCn );	// 国内股票 上证 深证,
		if ( m_iTrendDayCount != 1 )
		{
			// 与多日分时冲突
			SetTrendMultiDay(1, false, false);	// 只会比1大，不需要额外请求数据
			OnVDataMerchKLineUpdate(m_pMerchXml);	// 截断数据
		}

		// 竞价图
		RemoveSubRegion();
		// 设置竞价图参数等~~
		CalcCompetePriceValue();
		m_pRegionMain->NestSizeAll();
		UpdateAxisSize( );
		m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
		Invalidate();
		if ( bReqData )
		{
			RequestTrendPlusData();
		}
	}
	else if (ETBT_WindWarningLine == eBtnCur)
	{
		CString StrIndexName = L"风警线";

		RemoveSubRegion();
		m_pRegionMain->NestSizeAll();
		UpdateAxisSize( );
		m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);

		AddIndexToSubRegion(StrIndexName);

		Invalidate();
	}
	else if ( ETBT_DuoKong == eBtnCur )
	{
		// 多空指标
		//AddIndexToSubRegion(_T("多空"));
		//AddIndexToSubRegion(_T("DMI"));
		AddSpecialCurveToSubRegion();
		if ( bReqData )
		{
			RequestTrendPlusData();
		}
	}
	else if ( ETBT_RiseFallRate == eBtnCur )
	{
		// 涨跌率指标
		//AddIndexToSubRegion(_T("涨跌率"));
		//AddIndexToSubRegion(_T("DMA"));
		AddSpecialCurveToSubRegion();
		if ( bReqData )
		{
			RequestTrendPlusData();
		}
	}
	else if ( ETBT_PriceModel == eBtnCur )
	{
		// 期货价格模型，无数据
		ASSERT( 0 );
	}
	else if ( ETBT_Volumn == eBtnCur )
	{
		if ( m_SubRegions.GetSize() > 1 )
		{
			RemoveSubRegion();
		}
		else
		{
			m_pRegionMain->NestSizeAll();
			UpdateAxisSize( );
			m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
			Invalidate();
		}
	}
	else if ( ETBT_Amount == eBtnCur )
	{
		// 指数显示成交额，没有明确改动，不处理
		ASSERT( 0 );
	}
	else
	{
		ASSERT( 0 );
	}
}

void CIoViewTrend::DrawBottomBtns()
{
	if (7 == m_iChartType || m_bHistoryTrend || !m_bShowIndexToolBar)
	{
		return;
	}

	CClientDC dc(this);

	CRect rc, rcClient;		// 按钮区域rect
	GetClientRect(rcClient);
	rc = rcClient;
	rc.top = rc.bottom - TREND_BOTTOM_HEIGHT;

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bmp;

	bmp.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
	memDC.SelectObject(&bmp);
	memDC.FillSolidRect(rc, GetIoViewColor(ESCBackground));
	memDC.SetBkMode(TRANSPARENT);
	Gdiplus::Graphics graphics(memDC.GetSafeHdc());

	CRect rcControl;
	int iBtnBkLength = 0;
	CRect rcTemp;
	for ( int32 i=0; i < m_aTrendBtnCur.GetSize(); i++ )
	{
		CPolygonButton &btnControl = m_aTrendBtnCur[i].m_btnPolygon;
		btnControl.GetRect(rcControl);

		if (rcControl.right > iBtnBkLength)
		{
			iBtnBkLength = rcControl.right;
			rcTemp = rcControl;
		} 
	}

	// 背景色
	if (!rcTemp.IsRectEmpty() )
	{
		memDC.FillSolidRect(rcClient.left, rcTemp.top -1, iBtnBkLength + 1 + 1, rcClient.bottom, RGB(0,0,0));	// 分割线
		memDC.MoveTo(rcClient.left, rcTemp.top -1);
		memDC.LineTo(rcClient.right, rcTemp.top -1);
		memDC.MoveTo(rcClient.left, rcTemp.bottom +1);
		memDC.LineTo(rcClient.right, rcTemp.bottom +1);
	}

	for ( int32 i=0; i < m_aTrendBtnCur.GetSize(); i++ )
	{
		CPolygonButton &btnControl = m_aTrendBtnCur[i].m_btnPolygon;
		btnControl.SetShift(0);
		btnControl.DrawButton(&graphics);
	}

	dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &memDC, rc.left, rc.top, SRCCOPY);
	dc.SelectClipRgn(NULL);
	memDC.DeleteDC();
	bmp.DeleteObject();
}

BOOL CIoViewTrend::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int32 iID = (int32)wParam;

	int32 iIndex = FindIndexByBtnID(iID);

	if ((iID>=(BTN_ID_BEGIN+ETBT_Volumn)) && (iID<(BTN_ID_BEGIN+ETBT_Count)))
	{
		if ( iIndex != m_iBtnCur )
		{
			m_iBtnCur = iIndex;
			OnTrendBtnSelChanged();
			DrawBottomBtns();
		}
		if(ETBT_Index == m_aTrendBtnCur.GetAt(iIndex).m_eTrendBtnType)
		{
			ShowIndexMenu();
		}
	}

	return CIoViewChart::OnCommand(wParam, lParam);
}

void CIoViewTrend::OnLButtonDown( UINT nFlags, CPoint point )
{
	int32 iButton = TBottomBtnHitTest(point);
	if (INVALID_ID != iButton)
	{
		int32 iSize = m_aTrendBtnCur.GetSize();
		for (int32 i=0; i<iSize; i++)
		{
			CPolygonButton &btnControl = m_aTrendBtnCur[i].m_btnPolygon;
			if (btnControl.GetCheck())
			{
				btnControl.MouseLeave(false);
				btnControl.SetCheckStatus(FALSE);
			}
		}

		bool32 bCheck = m_aTrendBtnCur[iButton].m_btnPolygon.GetCheck();
		if (!bCheck)
		{
			m_aTrendBtnCur[iButton].m_btnPolygon.SetCheckStatus(TRUE,FALSE);
		}
		m_aTrendBtnCur[iButton].m_btnPolygon.LButtonDown();
	}

	CIoViewChart::OnLButtonDown(nFlags, point);
}

void CIoViewTrend::OnLButtonUp(UINT nFlags, CPoint point) 
{
	int32 iButton = TBottomBtnHitTest(point);
	if (INVALID_ID != iButton)
	{
		m_aTrendBtnCur[iButton].m_btnPolygon.LButtonUp();
	}

	CIoViewChart::OnLButtonUp(nFlags, point);
}

bool32 CIoViewTrend::InitBtnIndex()
{
	uint32 iFlag = CFormularContent::KAllowMinute |CFormularContent::KAllowSub;
	E_ReportType eRT = GetMerchKind(m_pMerchXml);
	E_IndexStockType eIST =    (eRT == ERTExp ? EIST_Index : EIST_Stock);
	CStringArray aNames;
	CFormulaLib::instance()->GetAllowNames(iFlag, aNames, eIST);
	bool32 bFind = false;
	for ( int32 i=0; i < aNames.GetSize() ; i++ )
	{
		if ( aNames[i] == m_StrBtnIndexName )
		{
			bFind = true;
			break;
		}
	}
	if ( !bFind && aNames.GetSize() > 0 )
	{
		m_StrBtnIndexName = aNames[0];	// 默认给第一个
		bFind = true;
	}
	if ( !bFind )
	{
		m_StrBtnIndexName.Empty();
	}

	return !m_StrBtnIndexName.IsEmpty();
}

void CIoViewTrend::RecalcBtnRect(bool32 bNestRegion/* = false*/)
{
	if (7 == m_iChartType || m_bHistoryTrend || !m_bShowIndexToolBar)
	{
		return;
	}

	CRect rc;
	GetClientRect(rc);

	if (m_aTrendBtnCur.GetSize() > 0 && !ParentIsIoView())
	{
		// 历史&&作为其它ioview的子视图不显示这个
		rc.top = rc.bottom - TREND_BTN_HEIGHT;
	}
	else
	{
		rc.top = rc.bottom;
	}

	// 暂时屏蔽这些功能
	//rc.top = rc.bottom;

	CClientDC dc(this);
	CFont *pFontOld = dc.SelectObject(GetIoViewFontObject(ESFSmall));

	int32 iWidth = 90; 
	CRect rtBtn(rc);
	rtBtn.bottom -= 1;
	rtBtn.right = rtBtn.left + iWidth;
	for ( int32 i=0; i < m_aTrendBtnCur.GetSize(); i++ )
	{
		m_aTrendBtnCur[i].m_btnPolygon.SetRect(rtBtn);

		rtBtn.left += (iWidth+KiIndexSpace-iIndexShift);
		rtBtn.right = rtBtn.left + iWidth;
	}

	dc.SelectObject(pFontOld);

	if (bNestRegion && NULL != m_pRegionMain )
	{
		m_pRegionMain->ReSizeAll();
	}
}

bool32 CIoViewTrend::FromXmlInChild( TiXmlElement *pTiXmlElement )
{
	if ( NULL != pTiXmlElement )
	{
		// 按钮
		m_mapMerchTypeBtnIndex.RemoveAll();
		m_mapMerchTypeIndexName.RemoveAll();
		for ( TiXmlElement *pEle = pTiXmlElement->FirstChildElement(KStrXMLEleTrendMerchType)
			; pEle != NULL
			; pEle = pEle->NextSiblingElement(KStrXMLEleTrendMerchType) )
		{
			const char *pszAttri = pEle->Attribute(KStrXMLAttriTrendMerchType);
			if ( NULL == pszAttri )
			{
				continue;
			}
			int32 iMerchType = atol(pszAttri);

			pszAttri = pEle->Attribute(KStrXMLAttriTrendBtnIndex);
			if ( NULL != pszAttri )
			{
				// 每个商品类型对应的按钮索引
				m_mapMerchTypeBtnIndex[ iMerchType ] = atol(pszAttri);				
			}

			pszAttri = pEle->Attribute(KStrXMLAttriTrendIndexName);
			if ( NULL != pszAttri )
			{
				// 每个商品类型对应的指标名称
				TCHAR *pwszIndexName = CEtcXmlConfig::MultiToWide(pszAttri, CP_UTF8);
				if ( NULL != pwszIndexName )
				{
					m_mapMerchTypeIndexName[ iMerchType ] = pwszIndexName;
					delete []pwszIndexName;
				}
			}
		}
		// 副图高度比例
		m_aSubRegionHeightRatios.RemoveAll();
		TiXmlElement *pEle = pTiXmlElement->FirstChildElement(KStrXMLEleTrendRegions);
		if ( NULL != pEle )
		{
			for ( TiXmlElement *pEleRegion = pEle->FirstChildElement(KStrXMLEleTrendRegion)
				; pEleRegion != NULL
				; pEleRegion = pEleRegion->NextSiblingElement(KStrXMLEleTrendRegion) )
			{
				const char *pszAttri = pEleRegion->Attribute(KIoViewChartRegionId);
				if ( NULL == pszAttri )
				{
					continue;
				}

				pszAttri = pEleRegion->Attribute(KIoViewChartRegionHeightRatio);
				if ( NULL == pszAttri )
				{
					continue;
				}
				double dfRatio = atof(pszAttri);
				if ( dfRatio < 0.0 || dfRatio >= 1.0 )
				{
					continue;
				}

				m_aSubRegionHeightRatios.Add(dfRatio);
			} // pEleRegion != NULL
		}
	}
	return true;
}

CString CIoViewTrend::ToXmlInChild()
{
	CString StrRet;
	return StrRet;
}


CString CIoViewTrend::ToXmlEleInChild()
{
	CString StrRet;
	CString Str;
	POSITION pos = m_mapMerchTypeBtnIndex.GetStartPosition();
	while ( NULL != pos )
	{
		int32 iMerchType;
		int32 iBtnIndex;
		m_mapMerchTypeBtnIndex.GetNextAssoc(pos, iMerchType, iBtnIndex);
		Str.Format(_T("    <%s %s=\"%d\" %s=\"%d\" />\n")
			, CString(KStrXMLEleTrendMerchType)
			, CString(KStrXMLAttriTrendMerchType), iMerchType
			, CString(KStrXMLAttriTrendBtnIndex), iBtnIndex
			);
		StrRet += Str;
	}

	// 每个商品类型对应选择的指标
	pos = m_mapMerchTypeIndexName.GetStartPosition();
	while ( NULL != pos )
	{
		int32 iMerchType;
		CString StrIndexName;
		m_mapMerchTypeIndexName.GetNextAssoc(pos, iMerchType, StrIndexName);
		Str.Format(_T("    <%s %s=\"%d\" %s=\"%s\" />\n")
			, CString(KStrXMLEleTrendMerchType)
			, CString(KStrXMLAttriTrendMerchType), iMerchType
			, CString(KStrXMLAttriTrendIndexName), StrIndexName
			);
		StrRet += Str;
	}
	// 当前region所占的高度分配比例, 由于trend的region大部分都没有相应的指标，so，自己do it
	if ( NULL != m_pRegionMain && m_SubRegions.GetSize() > 0 )
	{
		CRect rcView(0,0,0,0);
		GetClientRect(rcView);
		const double KFViewHeight = (double)rcView.Height();
		if ( KFViewHeight > 0.0 )
		{
			StrRet += _T("<") + CString(KStrXMLEleTrendRegions) +_T(">\n");
			for ( int32 i=0; i < m_SubRegions.GetSize() ; ++i )
			{
				const int32 KIRegionHeight = m_SubRegions[i].m_pSubRegionMain->m_RectView.Height();
				Str.Format(_T("   <%s %s=\"%d\" %s=\"%0.15f\" />\n")
					, CString(KStrXMLEleTrendRegion).GetBuffer()
					, CString(KIoViewChartRegionId).GetBuffer(), i+1
					, CString(KIoViewChartRegionHeightRatio).GetBuffer(), KIRegionHeight/KFViewHeight
					);
				StrRet += Str;
			}

			StrRet += _T("</") + CString(KStrXMLEleTrendRegions) +_T(">\n");
		}
	}
	return StrRet;
}

bool32 CIoViewTrend::AddShowIndex(const CString &StrIndexName, bool32 bDelRegionAllIndex, bool32 bChangeStockByIndex, bool32 bShowUserRightDlg)
{
	return AddIndexToSubRegion(StrIndexName, bShowUserRightDlg);
}

void	CIoViewTrend::GotoCompletePrice()
{
	//	查询是否存在竞价菜单
	int iBtnIndex = -1;
	for (unsigned i =0; i < m_aTrendBtnCur.GetSize(); ++i)
	{		
		if (ETBT_CompetePrice + BTN_ID_BEGIN == m_aTrendBtnCur[i].m_iID)
		{
			iBtnIndex = i;
			break;
		}
	}

	//	如果已经选中，则清除竞价
	bool bShow = IsCompetePriceShow();
	if (bShow)
	{
		RemoveSubRegion();
		m_pRegionMain->NestSizeAll();
		UpdateAxisSize( );
		m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
		Invalidate();
		if (m_aTrendBtnCur.GetSize() > 0)
		{
			m_iBtnCur = 0;
			E_ReportType eRT = GetMerchKind(m_pMerchXml);
			m_mapMerchTypeBtnIndex[ eRT ] = m_iBtnCur;
		}
		return;
	}

	//for (unsigned i =0; i < m_aTrendBtnCur.GetSize(); ++i)
	//{		
	//	if (ETBT_CompetePrice + BTN_ID_BEGIN == m_aTrendBtnCur[i].m_iID)
	//	{
	//		m_aTrendBtnCur[i].m_btnPolygon.SetCheckStatus(TRUE);
	//	}
	//	else
	//	{
	//		m_aTrendBtnCur[i].m_btnPolygon.SetCheckStatus(FALSE);
	//	}
	//}

	m_iBtnCur = iBtnIndex;	//	设置竞价标志位
	if (-1 == m_iBtnCur)
	{
		return;	//	不存在竞价菜单
	}

	E_ReportType eRT = GetMerchKind(m_pMerchXml);
	m_mapMerchTypeBtnIndex[ eRT ] = m_iBtnCur;
	ASSERT( eRT == ERTStockCn );	// 国内股票 上证 深证,
	if ( m_iTrendDayCount != 1 )
	{
		// 与多日分时冲突
		SetTrendMultiDay(1, false, false);	// 只会比1大，不需要额外请求数据
		OnVDataMerchKLineUpdate(m_pMerchXml);	// 截断数据
	}

	// 竞价图
	RemoveSubRegion();
	// 设置竞价图参数等~~
	CalcCompetePriceValue();
	m_pRegionMain->NestSizeAll();
	UpdateAxisSize( );
	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	Invalidate();
	RequestTrendPlusData();

}

bool32 CIoViewTrend::AddIndexToSubRegion( const CString &StrIndexName, bool32 bShowRightDlg/*=false*/ )
{
	T_MerchNodeUserData* pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == m_pRegionMain || NULL == pData )
	{
		ASSERT( 0 );
		return false;
	}

	int32 iSize = m_SubRegions.GetSize();

	//
	if (1 == iSize)
	{
		// 需要添加非成交量副图
		if ( !AddSubRegion(false) || m_SubRegions.GetSize() == 1 )
		{
			ASSERT( 0 );
			return false;;	// 无法创建副图
		}
	}

	int32 iSizeNow = m_SubRegions.GetSize();

	//
	CChartRegion *pSubRegion = NULL;
	if ( m_SubRegions.GetSize() != 2 )
	{
		ASSERT( 0 );
		return false;
	}
	pSubRegion = m_SubRegions[1].m_pSubRegionMain;

	if ( NULL == pSubRegion )
	{
		ASSERT( 0 );
		return false;;
	}

	if ( !CPluginFuncRight::Instance().IsUserHasIndexRight(StrIndexName, bShowRightDlg) )
	{
		return false;; // 权限不够
	}

	//
	for ( int32 i=0; i < pData->aIndexs.GetSize() ; i++ )
	{
		// 由于实际创建index也是根据name来创建，so简单判断名字+region
		if ( pData->aIndexs[i]->strIndexName == StrIndexName && pSubRegion == pData->aIndexs[i]->pRegion )
		{
			return false;;
		}
	}

	//
	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);

	// pSubRegion已被删除，不应该在用
	DeleteRegionCurves(pSubRegion);

	T_IndexParam* pIndex = AddIndex(pSubRegion, StrIndexName);
	if ( NULL == pIndex )
	{
		ASSERT( 0 );
		return false;;
	}	

	//
	int32 iNodeBegin,iNodeEnd;
	GetNodeBeginEnd(iNodeBegin,iNodeEnd);
	g_formula_compute ( this, this, pSubRegion, pData, pIndex, iNodeBegin,iNodeEnd );

	if ( IsSuspendedMerch() )
	{
		// 停盘数据的指标需要截断虚假数据
		TrimSuspendedIndexData();
	}

	UpdateSelfDrawCurve();
	m_mapMerchTypeIndexName[ GetMerchKind(m_pMerchXml) ] = StrIndexName;
	Invalidate();
	return true;
}

void CIoViewTrend::RemoveSubRegion()
{
	if ( m_SubRegions.GetSize() != 2 )
	{
		ASSERT( m_SubRegions.GetSize() == 1 || m_SubRegions.GetSize() == 0 );
		return;
	}

	T_MerchNodeUserData *pData = m_MerchParamArray.GetSize() > 0?m_MerchParamArray[0]:NULL;
	if ( NULL == pData )
	{
		ASSERT( 0 );
		return;
	}

	CChartRegion *pSubRegion = m_SubRegions[1].m_pSubRegionMain;
	ASSERT( NULL != pSubRegion );
	DeleteRegionCurves ( pSubRegion );
	// 删除与这个region相关的指标线等
	int32 iDel = 0;
	int32 iSizeOld = pData->aIndexs.GetSize();
	for ( int32 i=pData->aIndexs.GetUpperBound(); i >= 0 ; --i )
	{
		if ( pData->aIndexs[i]->pRegion == pSubRegion )
		{
			DeleteIndexParamData(pData->aIndexs[i]);	// 已经被removeAt删除了
			iDel++;
		}
	}
	ASSERT( pData->aIndexs.GetSize() == iSizeOld-iDel );

	DelSubRegion(pSubRegion, false);
	if ( m_IndexPostAdd.id >= 0 )
	{
		if  ( m_IndexPostAdd.pRegion == pSubRegion )
		{
			m_IndexPostAdd.id = -1;
		}
	}
	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	Invalidate();
}

CString CIoViewTrend::GetSubRegionTitle()
{
	if ( m_SubRegions.GetSize() > 1
		&& m_aTrendBtnCur.GetSize() > 0 )
	{
		return m_aTrendBtnCur[m_iBtnCur%m_aTrendBtnCur.GetSize()].m_btnPolygon.GetCaption();
	}
	else if ( ETBT_CompetePrice == GetCurTrendBtnType()
		&& m_aTrendBtnCur.GetSize() > 0 )
	{
		return m_aTrendBtnCur[m_iBtnCur%m_aTrendBtnCur.GetSize()].m_btnPolygon.GetCaption();
	}
	return _T("");
}

extern CString MakeMenuString(const CString& StrName, const CString& StrExplain, bool32 bAppendSpace=true);
bool32 CIoViewTrend::LoadAllIndex( CNewMenu *pMenu )
{
	// 把所有的指标都加载到 指定的 所有指标 菜单里:
	if ( NULL == m_pRegionPick || NULL == pMenu )
	{
		return false;
	}

	//
	CFormulaLib* pLib = CFormulaLib::instance();
	if ( NULL == pLib )
	{
		return false;
	}

	// 
	bool32 bMainRegion = false;

	if ( m_pRegionPick == m_pRegionMain )
	{
		bMainRegion = true;
	}
	//
	int32 iIndex = 0;
	m_aAllFormulaNames.RemoveAll();

	E_IndexStockType eIST = EIST_None;
	if ( NULL != m_pMerchXml )
	{
		eIST = GetMerchKind(m_pMerchXml) == ERTExp ? EIST_Index : EIST_Stock;
	}

	//系统指标(sys_index.xml)
	if ( NULL != pLib->m_SysIndex.m_pGroupNormal )
	{
		CIndexGroupNormal* pGroup1 = pLib->m_SysIndex.m_pGroupNormal;

		int32 iSize2 = pGroup1->m_Group2.GetSize();

		for (int32 j = 0; j < iSize2; j ++ )
		{
			CIndexGroup2* pGroup2 = pGroup1->m_Group2.GetAt(j);

			if ( NULL == pGroup2 )
			{
				continue;
			}

			int32 iSize3 = pGroup2->m_Contents.GetSize();

			if ( iSize3 > 0 )				
			{
				CNewMenu* pSubMenu = pMenu->AppendODPopupMenu(pGroup2->m_StrName);
				bool32 bNeedAppendSpace = false;

				for (int32 k = 0; k < iSize3; k ++ )
				{
					CFormularContent* pContent = pGroup2->m_Contents.GetAt(k);

					if( NULL == pContent)
					{
						continue;
					}

					// 不是指标公式的, 闪
					if ( EFTNormal != pContent->m_eFormularType )
					{
						continue;
					}

					if ( bMainRegion && !CheckFlag(pContent->flag, CFormularContent::KAllowMain))
					{
						// 当前是主图,但是公式没有允许主图的标志.
						continue;
					}

					if ( !bMainRegion && !CheckFlag(pContent->flag, CFormularContent::KAllowSub))
					{
						// 当前是副图,但是公式没有允许副图的标志.
						continue;
					}

					if ( !pContent->IsIndexStockTypeMatched(eIST) )
					{
						continue;
					}

					CString StrText = pContent->name;
					CString StrExp  = pContent->explainBrief;
					if ( !StrExp.IsEmpty() )
					{
						bNeedAppendSpace = true;
					}

					bool32 bShow = CFormulaLib::BeIndexShow(pContent->name);	// 是否在界面上显示出来

					if ( bShow )
					{
						CString StrIndexFull = MakeMenuString(StrText, StrExp, bNeedAppendSpace);

						pSubMenu->AppendODMenu(StrIndexFull, MF_STRING, IDM_IOVIEWKLINE_ALLINDEX_BEGIN + iIndex );

						iIndex += 1;
						m_aAllFormulaNames.Add(StrText);
					}					
				}

				if ( 0 == pSubMenu->GetMenuItemCount() )
				{
					// 没有子菜单的,把这项删掉, 依靠名称的方法必须依赖一个非popup菜单
					pMenu->DeleteMenu((UINT)pSubMenu->GetSafeHmenu(), MF_BYCOMMAND);
				}
			}						
		}
	}

	//用户自编
	if ( NULL != pLib->m_UserIndex.m_pGroupNormal )
	{
		CIndexGroupNormal* pGroup1 = pLib->m_UserIndex.m_pGroupNormal;

		//	
		int32 iSize2 = pGroup1->m_Group2.GetSize();

		for (int32 j = 0; j < iSize2; j ++ )
		{
			CIndexGroup2* pGroup2 = pGroup1->m_Group2.GetAt(j);

			if ( NULL == pGroup2 )
			{
				continue;
			}

			int32 iSize3 = pGroup2->m_Contents.GetSize();
			CNewMenu* pSubMenu1 = pMenu->AppendODPopupMenu(pGroup2->m_StrName);
			bool32 bNeedAppendSpace = false;

			if ( NULL == pSubMenu1 )
			{
				continue;
			}

			if ( iSize3 > 0)
			{
				for (int32 k = 0; k < iSize3; k ++ )
				{
					CFormularContent* pContent = pGroup2->m_Contents.GetAt(k);		 		

					if( NULL == pContent)
					{
						continue;
					}

					if ( bMainRegion && !CheckFlag(pContent->flag, CFormularContent::KAllowMain))
					{
						// 当前是主图,但是公式没有允许主图的标志.
						continue;
					}

					if ( !bMainRegion && !CheckFlag(pContent->flag, CFormularContent::KAllowSub))
					{
						// 当前是副图,但是公式没有允许副图的标志.
						continue;
					}

					if( !CFormulaLib::BeIndexShow(pContent->name) )
					{
						// 不让在界面上显示
						continue;
					}

					if ( !pContent->IsIndexStockTypeMatched(eIST) )
					{
						continue;
					}

					CString StrText = pContent->name;
					CString StrExp  = pContent->explainBrief;
					if ( !StrExp.IsEmpty() )
					{
						bNeedAppendSpace = true;
					}

					CString StrIndexFull = MakeMenuString(StrText, StrExp, bNeedAppendSpace);

					pSubMenu1->AppendODMenu(StrIndexFull, MF_STRING, IDM_IOVIEWKLINE_ALLINDEX_BEGIN + iIndex );

					iIndex += 1;
					m_aAllFormulaNames.Add(StrText);
				}
			}

			if ( 0 == pSubMenu1->GetMenuItemCount() )
			{
				// 没有子菜单的,把这项删掉
				pMenu->DeleteMenu((UINT)pSubMenu1->GetSafeHmenu(), MF_BYCOMMAND);
			}
		}
	}
	return true;
}

int32 CIoViewTrend::LoadOftenIndex( CNewMenu *pMenu )
{
	if ( NULL == pMenu )
	{
		return -1;
	}

	if ( m_MerchParamArray.GetSize() <= 0 )
	{
		return -1;
	}

	T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);

	if ( NULL == pData || NULL == m_pMerchXml )
	{
		return -1;
	}

	//
	uint32 iFlag = NodeTimeIntervalToFlag ( pData->m_eTimeIntervalFull );

	if ( m_pRegionPick == m_pRegionMain )
	{
		AddFlag ( iFlag, CFormularContent::KAllowMain );
	}
	else
	{
		AddFlag ( iFlag, CFormularContent::KAllowSub );
	}

	E_IndexStockType eIST = GetMerchKind(m_pMerchXml) == ERTExp ? EIST_Index : EIST_Stock;

	CFormulaLib::instance()->GetAllowNames(iFlag,m_FormulaNames, eIST);
	CStringArray Formulas;
	FindRegionFormula(m_pRegionPick,Formulas);
	DeleteSameString ( m_FormulaNames,Formulas);
	Formulas.RemoveAll();	

	bool32 bIsExp = false;
	if ( NULL != pData && NULL != pData->m_pMerchNode )
	{
		bIsExp = GetMerchKind(pData->m_pMerchNode) == ERTExp;
	}

	int32 iIndexNums = 0;

	for (int32 i = 0; i < m_FormulaNames.GetSize(); i ++ )
	{
		if ( i >= (IDM_IOVIEWKLINE_OFTENINDEX_END - IDM_IOVIEWKLINE_OFTENINDEX_BEGIN) ) break;

		iIndexNums += 1;

		CString StrName = m_FormulaNames.GetAt(i);
		CFormularContent* pFormular = CFormulaLib::instance()->GetFomular(StrName);

		CString StrText = pFormular->name;		
		CString StrexplainBrief = pFormular->explainBrief;

		// 检查一下是不是可以添加的[大盘个股]指标
		if ( pFormular->m_byteIndexStockType == EIST_Index && !bIsExp )
		{
			// 大盘指标非指标股票
			continue;
		}
		else if ( pFormular->m_byteIndexStockType == EIST_Stock && bIsExp )
		{
			// 个股指标 指标股票
			continue;
		}

		CString StrIndexFull = MakeMenuString(StrText, StrexplainBrief, false);

		pMenu->AppendODMenu(StrIndexFull, MF_STRING, IDM_IOVIEWKLINE_OFTENINDEX_BEGIN+i);	
	}

	return iIndexNums;
}

void CIoViewTrend::DrawCompetePriceChart( CMemDCEx *pDC, CChartRegion &ChartRegion )
{
	ASSERT( ETBT_CompetePrice == GetCurTrendBtnType() );

	// 需要绘制坐标轴 & 图形
	// 竞价数据 价格&成交量 也能影响坐标Y值
	T_MerchNodeUserData *pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == pDC
		|| NULL == m_pCompetePriceNodes
		|| NULL == pData )
	{
		return;
	}

	TRACE(_T("竞价数据: %d\n"), m_pCompetePriceNodes->GetSize());

	CString StrText;
	CPoint pt1,pt2;
	COLORREF color;
	CPen pen,*pOldPen;

	color = GetIoViewColor(ESCChartAxisLine);
	pen.CreatePen(PS_SOLID,1,color);
	pOldPen = (CPen*)pDC->SelectObject(&pen);	
	COLORREF clrBk = GetIoViewColor(ESCBackground);
	if ( clrBk == CFaceScheme::Instance()->GetSysColor(ESCBackground) )
	{
		clrBk = RGB(33, 36, 41);
	}
	else
	{
		// 灰度
		CColorStep step;
		step.InitColorRange(clrBk, 32.0f, 32.0f, 32.0f);
		clrBk = step.GetColor(1);
	}

	clrBk =  RGB(41, 40, 46);

	if ( &ChartRegion == m_pRegionMain )
	{
		// 主图绘制
		int32 iLeftSkip = ChartRegion.GetLeftSkip();
		if ( iLeftSkip > 0 )
		{
			CRect rcChart = ChartRegion.GetRectCurves();
			rcChart.left -= iLeftSkip;
			rcChart.right = rcChart.left + iLeftSkip;

			COLORREF clrAxis = GetIoViewColor(ESCChartAxisLine);
			pt1.x = rcChart.CenterPoint().x;
			pt1.y = rcChart.top;
			pt2.x = pt1.x;
			pt2.y = rcChart.bottom;

			// 背景色
			pDC->FillSolidRect(rcChart, clrBk);

			pDC->_DrawDotLine(pt1, pt2, 2, clrAxis);	// 中间竖线

			pt1.x = rcChart.right;
			pt2.x = pt1.x;
			pDC->_DrawLine(pt1, pt2);		// 右侧竖线

			// 水平线
			int32 iSize = m_pRegionMain->m_aYAxisDivide.GetSize();
			CAxisDivide *pAxisDivide = (CAxisDivide *)m_pRegionMain->m_aYAxisDivide.GetData();

			if ( NULL == pAxisDivide )
			{
				pAxisDivide = (CAxisDivide *)m_aYAxisDivideBkMain.GetData();
				iSize		= m_aYAxisDivideBkMain.GetSize();
			}

			CRect RectMain = rcChart;
			for (int32 i = 0; i < iSize; i++)
			{
				CAxisDivide &AxisDivide = pAxisDivide[i];
				pt1.y = pt2.y = AxisDivide.m_iPosPixel;
				pt1.x = RectMain.left;
				pt2.x = RectMain.right;

				// 画线
				if (CAxisDivide::ELSSolid == AxisDivide.m_eLineStyle)
				{
					pDC->_DrawLine(pt1, pt2);
				}
				else if (CAxisDivide::ELSDoubleSolid == AxisDivide.m_eLineStyle)
				{						
					pDC->_DrawLine(pt1, pt2);

					pt1.y++; pt2.y++;
					pDC->_DrawLine(pt1, pt2);
				}
				else if (CAxisDivide::ELS3LineSolid == AxisDivide.m_eLineStyle)
				{					
					pDC->_DrawLine(pt1, pt2);

					pt1.y ++; pt2.y ++;
					pDC->_DrawLine(pt1, pt2);
				}
				else if (CAxisDivide::ELSDot == AxisDivide.m_eLineStyle)
				{
					pDC->_DrawDotLine(pt1, pt2, 2, clrAxis);
				}
			}

			if ( 0 == m_pRegionMain->m_aYAxisDivide.GetSize() && 0 == m_aYAxisDivideBkMain.GetSize())
			{
				pt1.x = RectMain.left;
				pt1.y = RectMain.top;

				pt2.x = RectMain.right;
				pt2.y = RectMain.top;

				pDC->_DrawLine(pt1, pt2);
			}

			// 画数据线
			COLORREF clrDataLine = GetIoViewColor(ESCGuideLine1);

			CArray<CPoint, const CPoint&>	aPts;
			const CArray<CNodeData, CNodeData &> &aNodes = m_pCompetePriceNodes->GetNodes();
			CChartCurve *pDep = ChartRegion.GetDependentCurve();
			int32 iInit = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time.GetTime();
			int32 iOpen = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeOpen.m_Time.GetTime();

			// 固定时间当天[9:15-9:25], 否则就是初始化到开盘时间鸟
			CGmtTime TimeStart, TimeEnd;
			if ( GetCallAuctionTime(TimeStart, TimeEnd) )
			{
				iInit = TimeStart.GetTime();
				iOpen = TimeEnd.GetTime();
			}

			int32 iRange = iOpen-iInit;
			CPriceToAxisYObject AxisObj;
			if ( iRange > 0 && GetCompetePriceMinMax(true, AxisObj) )
			{
				aPts.SetSize(0, aNodes.GetSize());
				CPen pen1;
				pen1.CreatePen(PS_SOLID, 0, clrDataLine);
				pDC->SelectObject(&pen1);
				for ( int32 i=0; i < aNodes.GetSize() ; i++ )
				{
					const CNodeData &node = aNodes[i];
					CPoint pt;
					pt.x = rcChart.left + (node.m_iID-iInit)/(float)(iRange)*(rcChart.Width());
					if ( AxisObj.PriceYToRectY(rcChart, node.m_fClose, pt.y) )
					{
						aPts.Add(pt);
					}
				}

				pDC->Polyline(aPts.GetData(), aPts.GetSize());

				// 对于每个点，画小圆点显示
				CBrush brh;
				brh.CreateSolidBrush(clrDataLine);
				CBrush *pOldBrush = (CBrush *)pDC->SelectObject(&brh);
				for ( int i=0; i < aPts.GetSize() ; i++ )
				{
					CRect rc(aPts[i], aPts[i]);
					rc.InflateRect(2, 2);
					pDC->RoundRect(rc, aPts[i]);
				}

				// 对于要画开盘价的情况，如果开盘价不等于最后一个点，则额外牵一根线到开盘价
				// 貌似过滤掉最后一个白线以后就无此问题了
			}
		}
	}
	else if ( 
		m_SubRegions.GetSize() > 0
		&& m_SubRegions[0].m_pSubRegionMain != NULL
		&& &ChartRegion == m_SubRegions[0].m_pSubRegionMain )
	{
		// 成交量副图
		int32 iLeftSkip = ChartRegion.GetLeftSkip();
		if ( iLeftSkip > 0 )
		{
			CRect rcChart = ChartRegion.GetRectCurves();
			rcChart.left -= iLeftSkip;
			rcChart.right = rcChart.left + iLeftSkip;

			if ( ChartRegion.GetTopSkip() < 6 )
			{
				rcChart.top = ChartRegion.m_RectView.top;
			}

			COLORREF clrAxis = GetIoViewColor(ESCChartAxisLine);
			pt1.x = rcChart.CenterPoint().x;
			pt1.y = rcChart.top;
			pt2.x = pt1.x;
			pt2.y = rcChart.bottom;

			// 背景色
			pDC->FillSolidRect(rcChart.left, rcChart.top+1, rcChart.Width(), rcChart.Height()-1, clrBk);

			pDC->_DrawDotLine(pt1, pt2, 2, clrAxis);	// 中间竖线

			pt1.x = rcChart.right;
			pt2.x = pt1.x;
			pDC->_DrawLine(pt1, pt2);		// 右侧竖线

			CRect RectSubMain = rcChart;

			// 水平线
			{
				// 封口
				if (  ChartRegion.GetTopSkip() > 6 )		// 成交量副图的topskip比这少，不比画回圈 )
				{
					pt1 = RectSubMain.TopLeft();
					pt2 = CPoint(RectSubMain.right, RectSubMain.top);
					pDC->_DrawLine(pt1, pt2);
				}

				pt1 = CPoint(RectSubMain.left, RectSubMain.bottom);
				pt2 = CPoint(RectSubMain.right, RectSubMain.bottom);
				pDC->_DrawLine(pt1, pt2);						
			}

			// 画水平线
			{
				int32 iSize = ChartRegion.m_aYAxisDivide.GetSize();

				CAxisDivide *pAxisDivide = (CAxisDivide *)ChartRegion.m_aYAxisDivide.GetData();
				if ( NULL == pAxisDivide )
				{
					pAxisDivide = (CAxisDivide*)m_aYAxisDivideBkSub.GetData();
					iSize = m_aYAxisDivideBkSub.GetSize();
				}

				for (int32 i = 0; i < iSize; i++)
				{
					CAxisDivide &AxisDivide = pAxisDivide[i];
					pt1.y = pt2.y = AxisDivide.m_iPosPixel;
					pt1.x = RectSubMain.left;
					pt2.x = RectSubMain.right;

					if ( pt1.y - RectSubMain.top < 6 )
					{
						// 成交量副图的第一根虚线高度小于此
						continue;
					}

					// 画线
					if (CAxisDivide::ELSSolid == AxisDivide.m_eLineStyle)
					{
						pDC->_DrawLine(pt1, pt2);
					}
					else if (CAxisDivide::ELS3LineSolid == AxisDivide.m_eLineStyle)
					{
						pt1.y--; pt2.y--;
						pDC->_DrawLine(pt1, pt2);

						pt1.y += 2; pt2.y += 2;
						pDC->_DrawLine(pt1, pt2);

						pt1.y--; pt2.y--;
						pDC->_DrawLine(pt1, pt2);
					}
					else if (CAxisDivide::ELSDot == AxisDivide.m_eLineStyle)
					{
						pDC->_DrawDotLine(pt1, pt2, 2, color);
					}
				}
			}

			// 画数据线
			COLORREF clrDataLine = GetIoViewColor(ESCGuideLine1);

			CArray<CPoint, const CPoint&>	aPtsRed, aPtsGreen, aPtsWhite;
			const CArray<CNodeData, CNodeData &> &aNodes = m_pCompetePriceNodes->GetNodes();
			CChartCurve *pDep = ChartRegion.GetDependentCurve();
			int32 iInit = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time.GetTime();
			int32 iOpen = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeOpen.m_Time.GetTime();
			//float fPriceBase = pData->m_TrendTradingDayInfo.GetPrevReferPrice();
			float fPriceBase = GetTrendPrevClose();

			// 固定时间当天[9:15-9:25], 否则就是初始化到开盘时间鸟
			CGmtTime TimeStart, TimeEnd;
			if ( GetCallAuctionTime(TimeStart, TimeEnd) )
			{
				iInit = TimeStart.GetTime();
				iOpen = TimeEnd.GetTime();
			}

			int32 iRange = iOpen-iInit;
			CPriceToAxisYObject AxisObj;
			if ( iRange > 0 && fPriceBase > 0.0f && GetCompetePriceMinMax(false, AxisObj) )
			{

				aPtsGreen.SetSize(0, aNodes.GetSize());
				aPtsRed.SetSize(0, aNodes.GetSize());
				aPtsWhite.SetSize(0, aNodes.GetSize());
				for ( int32 i=0; i < aNodes.GetSize() ; i++ )
				{
					const CNodeData &node = aNodes[i];
					CPoint pt;
					pt.x = rcChart.left + (node.m_iID-iInit)/(float)(iRange)*(rcChart.Width());
					if ( AxisObj.PriceYToRectY(rcChart, node.m_fVolume, pt.y) )
					{
						if ( 1.0f == node.m_fOpen )
						{
							aPtsGreen.Add(pt);							// 顶点
							aPtsGreen.Add(CPoint(pt.x, rcChart.bottom));	// 底点
						}
						else if ( 2.0f == node.m_fOpen )
						{
							aPtsRed.Add(pt);							// 顶点
							aPtsRed.Add(CPoint(pt.x, rcChart.bottom));	// 底点
						}
						else
						{
							ASSERT( 0.0f == node.m_fOpen );
							aPtsWhite.Add(pt);							// 顶点
							aPtsWhite.Add(CPoint(pt.x, rcChart.bottom));	// 底点
						}
					}
				}

				if ( aPtsRed.GetSize() > 0 )
				{
					CPen pen1;
					pen1.CreatePen(PS_SOLID, 0, GetIoViewColor(ESCRise));
					pDC->SelectObject(&pen1);
					CArray<DWORD, DWORD> aCounts;
					aCounts.SetSize(aPtsRed.GetSize()/2);
					for ( int32 i=0; i < aCounts.GetSize() ; i++ )
					{
						aCounts[i] = 2;
					}
					pDC->PolyPolyline(aPtsRed.GetData(), aCounts.GetData(), aCounts.GetSize());
				}
				if ( aPtsGreen.GetSize() > 0 )
				{
					CPen pen1;
					pen1.CreatePen(PS_SOLID, 0, GetIoViewColor(ESCFall));
					pDC->SelectObject(&pen1);
					CArray<DWORD, DWORD> aCounts;
					aCounts.SetSize(aPtsGreen.GetSize()/2);
					for ( int32 i=0; i < aCounts.GetSize() ; i++ )
					{
						aCounts[i] = 2;
					}
					pDC->PolyPolyline(aPtsGreen.GetData(), aCounts.GetData(), aCounts.GetSize());
				}
				if ( aPtsWhite.GetSize() > 0 )
				{
					CPen pen1;
					pen1.CreatePen(PS_SOLID, 0, GetIoViewColor(ESCKeep));
					pDC->SelectObject(&pen1);
					CArray<DWORD, DWORD> aCounts;
					aCounts.SetSize(aPtsWhite.GetSize()/2);
					for ( int32 i=0; i < aCounts.GetSize() ; i++ )
					{
						aCounts[i] = 2;
					}
					pDC->PolyPolyline(aPtsWhite.GetData(), aCounts.GetData(), aCounts.GetSize());
				}
			}
		}
	}

	pDC->SelectObject(pOldPen);
}

void CIoViewTrend::CalcCompetePriceValue()
{
	ASSERT( ETBT_CompetePrice == GetCurTrendBtnType() );

	T_MerchNodeUserData *pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == pData 
		|| !InitCompetePriceNodes()
		|| NULL == m_pMerchXml )
	{
		return;	// 无数据
	}
	ASSERT( NULL != m_pCompetePriceNodes );
	if ( !pData->m_TrendTradingDayInfo.m_bInit )
	{
		m_pCompetePriceNodes->RemoveAll();	// 未初始化
		return;
	}

	TRACE(_T("竞价数据计算开始: %d\n"), m_pCompetePriceNodes->GetSize());
	m_pCompetePriceNodes->RemoveAll();	// 未初始化

	CGmtTime TimeStart, TimeEnd;
	//float fPriceBase = pData->m_TrendTradingDayInfo.GetPrevReferPrice();
	float fPriceBase = GetTrendPrevClose();
	if ( GetCallAuctionTime(TimeStart, TimeEnd)
		&& fPriceBase > 0.0f )
	{
		// 获取商品竞价数据
		const int32 iSize = m_pMerchXml->m_aAuctionDatas.GetSize();
		m_pCompetePriceNodes->GetNodes().SetSize(iSize);
		CNodeData *pNodesData = m_pCompetePriceNodes->GetNodes().GetData();
		const T_MerchAuction *pAuction = m_pMerchXml->m_aAuctionDatas.GetData();
		int32 iVolScale = m_pMerchXml->m_Market.m_MarketInfo.m_iVolScale;
		iVolScale = max(1, iVolScale);

		const time_t tStart = TimeStart.GetTime();
		const time_t tEnd = TimeEnd.GetTime();
		int32 iNodePos = 0;
		for ( int32 i=0; i < iSize ; i++ )
		{
			if ( pAuction[i].m_lTime < tStart
				|| pAuction[i].m_lTime > tEnd )
			{
				TRACE(_T("截掉竞价数据: 时间%u 价格%f\r\n"), pAuction[i].m_lTime, pAuction->m_fBuyPrice);
				continue;
			}
			pNodesData[iNodePos].m_fClose = pAuction[i].m_fBuyPrice;
			pNodesData[iNodePos].m_fVolume = pAuction[i].m_fBuyVolume/iVolScale;
			pNodesData[iNodePos].m_iID = pAuction[i].m_lTime;			// 竞价数据不必分钟对齐
			pNodesData[iNodePos].m_fOpen = pAuction[i].m_cFlag - '0';	// '0'没有 1买 2卖
			iNodePos++;
		}

		m_pCompetePriceNodes->GetNodes().SetSize(iNodePos);	// 实际数据长度
		// 这条线是自绘的，不用处理点
		// 需要截断非时间段内竞价数据
	}
	else
	{
		m_pCompetePriceNodes->RemoveAll();
	}

	// 	{
	// 		// 调试竞价数据
	// 		CStdioFile file;
	// 		file.Open(_T("竞价数据-分时.txt"), CFile::modeCreate |CFile::modeWrite |CFile::modeNoTruncate);
	// 		const CNodeData *pNodes  = m_pCompetePriceNodes->GetNodes().GetData();
	// 		file.SeekToEnd();
	// 		char szBuf[1000];
	// 		CString StrName = _T("\n") + pData->m_pMerchNode->m_MerchInfo.m_StrMerchCnName + _T("\n");
	// 		int iLen = WideCharToMultiByte(CP_ACP, 0, StrName, -1, szBuf, sizeof(szBuf), NULL, NULL);
	// 		file.Write(szBuf, iLen > 0 ? iLen-1: 0);
	// 		for ( int32 i=0; i < m_pCompetePriceNodes->GetNodes().GetSize() ; i++ )
	// 		{
	// 			CString Str;
	// 			tm *pt = localtime((const long *)&pNodes[i].m_iID);
	// 			Str.Format(_T("%02d:%02d:%02d 价格:%f 标志:%.0f 成交量:%f\n")
	// 				, pt->tm_hour, pt->tm_min, pt->tm_sec
	// 				, pNodes[i].m_fClose, pNodes[i].m_fOpen, pNodes[i].m_fVolume);
	// 			int iLen = WideCharToMultiByte(CP_ACP, 0, Str, -1, szBuf, sizeof(szBuf), NULL, NULL);
	// 			file.Write(szBuf, iLen > 0 ? iLen-1: 0);
	// 		}
	// 		file.Close();
	// 	}

	TRACE(_T("竞价数据计算结束: %d\n"), m_pCompetePriceNodes->GetSize());
}

bool32 CIoViewTrend::InitCompetePriceNodes()
{
	if ( NULL == m_pCompetePriceNodes && m_MerchParamArray.GetSize() > 0 && m_MerchParamArray[0] != NULL )
	{
		// 
		CNodeSequence *pNodes = new CNodeSequence();
		pNodes->SetUserData(m_MerchParamArray[0]);
		m_pCompetePriceNodes = pNodes;
	}
	return NULL != m_pCompetePriceNodes;
}

bool32 CIoViewTrend::CalcCompetePriceMinMax( CChartCurve *pDepCurve, OUT float &fYMin, OUT float &fYMax )
{
	ASSERT( ETBT_CompetePrice == GetCurTrendBtnType() );
	if ( NULL == pDepCurve 
		|| NULL == m_pCompetePriceNodes
		|| m_pCompetePriceNodes->GetSize() <= 0 )
	{
		return false;
	}
	// 利用主线的计算对象来计算最大最小值, 一定要在主线已经设置好了之后调用该函数
	const int32 iSize = m_pCompetePriceNodes->GetSize();
	CNodeSequence *pNodes = m_pCompetePriceNodes;

	// 
	fYMin	= FLT_MAX;
	fYMax	= -FLT_MAX;

	if ( &pDepCurve->GetChartRegion() == m_pRegionMain )
	{
		float fPricePreClose = GetTrendPrevClose();
		if ( fPricePreClose == 0.0f  )
		{
			return false;
		}
		for (int32 i = 0; i < iSize; i++)
		{
			CNodeData NodeData;
			pNodes->GetAt(i,NodeData);

			if ( CheckFlag(NodeData.m_iFlag,CNodeData::KValueInvalid))
			{
				continue;
			}
			fYMin = min(fYMin, NodeData.m_fClose);
			fYMax = max(fYMax, NodeData.m_fClose);
		}

		// 主图需要映射
		if ( fYMin <= fYMax )
		{
			CPriceToAxisYObject::PriceYToAxisYByBase(CPriceToAxisYObject::EAYT_Pecent, fPricePreClose, fYMin, fYMin);
			CPriceToAxisYObject::PriceYToAxisYByBase(CPriceToAxisYObject::EAYT_Pecent, fPricePreClose, fYMax, fYMax);
		}
	}
	else
	{
		for (int32 i = 0; i < iSize; i++)
		{
			CNodeData NodeData;
			pNodes->GetAt(i,NodeData);

			if ( CheckFlag(NodeData.m_iFlag,CNodeData::KValueInvalid))
			{
				continue;
			}
			fYMin = min(0.0f, NodeData.m_fVolume);	// 副图最小值0
			fYMax = max(fYMax, NodeData.m_fVolume);
		}
	}

	return fYMin <= fYMax;
}

bool32 CIoViewTrend::CalcCompetePriceMinMaxVol( bool32 bVol, OUT float &fYMin, OUT float &fYMax )
{
	ASSERT( ETBT_CompetePrice == GetCurTrendBtnType() );
	if ( !bVol )
	{
		ASSERT( 0 );
		return false;	// 不支持这种类型还
	}
	if ( NULL == m_pCompetePriceNodes )
	{
		return false;
	}

	const int32 iSize = m_pCompetePriceNodes->GetSize();
	CNodeSequence *pNodes = m_pCompetePriceNodes;

	// 
	fYMin	= FLT_MAX;
	fYMax	= -FLT_MAX;
	for (int32 i = 0; i < iSize; i++)
	{
		CNodeData NodeData;
		pNodes->GetAt(i,NodeData);

		if ( CheckFlag(NodeData.m_iFlag,CNodeData::KValueInvalid))
		{
			continue;
		}
		fYMin = min(0.0f, NodeData.m_fVolume); // 副图最小值0
		fYMax = max(fYMax, NodeData.m_fVolume);
	}

	return fYMin <= fYMax;
}

bool32 CIoViewTrend::GetCompetePriceMinMax( bool32 bMainRegion, OUT CPriceToAxisYObject &AxisObj )
{
	ASSERT( ETBT_CompetePrice == GetCurTrendBtnType() );
	if ( bMainRegion )
	{
		if ( NULL != m_pRegionMain )
		{
			CChartCurve *pDep = m_pRegionMain->GetDependentCurve();
			bool32 bValid = false;
			if ( NULL != pDep )
			{
				bValid = pDep->GetAxisYCalcObject(AxisObj);
			}
			if ( !bValid )
			{
				ASSERT( pDep->GetNodes()==NULL || pDep->GetNodes()->GetSize()==0 );	// 主图依赖线应该是准备好的而且是必计算了的
				// 不尝试计算竞价图主图的最大最小值
			}
			return bValid;
		}
		else
		{
			ASSERT( 0 );
		}
	}
	else
	{
		CChartRegion *pVolRegion = NULL;
		if ( m_SubRegions.GetSize() == 1
			&& NULL != (pVolRegion=m_SubRegions[0].m_pSubRegionMain) )
		{
			CChartCurve *pDep = pVolRegion->GetDependentCurve();
			bool32 bValid = false;
			if ( NULL != pDep )
			{
				bValid = pDep->GetAxisYCalcObject(AxisObj);
			}
			if ( !bValid && NULL == pDep )
			{
				// 副图最大最小值计算, 副图无线时才计算
				//ASSERT( NULL == pDep );	// 必没准备好副图线, 有时候确实没有准备好 如单独调用RedrawAll
				float fMin, fMax;
				if ( CalcCompetePriceMinMaxVol(true, fMin, fMax)
					&& AxisObj.SetCalcParam(CPriceToAxisYObject::EAYT_Normal, fMin, fMax, 0.0f) )
				{
					bValid = AxisObj.GetAxisYMax() >= AxisObj.GetAxisYMin();
				}
			}
			return bValid;
		}
	}
	return false;
}

float CIoViewTrend::GetTrendPrevClose()
{
	T_MerchNodeUserData *pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( pData == NULL )
	{
		return 0.0f;
	}
	float fPriceBase = pData->m_TrendTradingDayInfo.GetPrevReferPrice();
	// 有没有0的昨收呢
	// 对于有实时报价但是不是今天的实时报价做特殊处理
	if ( fPriceBase == 0.0f
		&& NULL != pData->m_pMerchNode
		&& NULL != pData->m_pMerchNode->m_pRealtimePrice )
	{
		if (ERTFuturesCn == pData->m_pMerchNode->m_Market.m_MarketInfo.m_eMarketReportType)	// 仅国内期货按昨结算价算
		{
			fPriceBase = pData->m_pMerchNode->m_pRealtimePrice->m_fPricePrevAvg;	// 取昨天的收盘
		}
		else
		{
			fPriceBase = pData->m_pMerchNode->m_pRealtimePrice->m_fPricePrevClose;	// 取昨天的收盘
		}
	}
	return fPriceBase;
}

void CIoViewTrend::OnVDataGeneralNormalUpdate( CMerch *pMerch )
{
	// 指数商品: 红绿柱线 多空 涨跌率
	// 目前所有同市场商品返回的一个一样的数据
	if ( NULL == m_pMerchXml || m_pMerchXml != pMerch )
	{
		return;
	}

	// 仅处理 0-000001 1000-399001，其它商品无数据
	int32 iMarketId		 = m_pMerchXml->m_MerchInfo.m_iMarketId;
	CString StrMerchCode = m_pMerchXml->m_MerchInfo.m_StrMerchCode;
	bool32 bMatchMerch = IsRedGreenMerch(m_pMerchXml);
	if ( !bMatchMerch )
	{
		return;
	}

	// 不做变更优化处理~

	// 红绿柱线是一定有的
	CalcRedGreenValue();

	E_TrendBtnType eBtn = GetCurTrendBtnType();
	if ( ETBT_DuoKong == eBtn )
	{
		// 多空与红绿柱同数据，做比例转换
		CalcDuoKongData();
	}
	else if ( ETBT_RiseFallRate == eBtn )
	{
		// 涨跌率 上涨下跌家数组成的数据
		CalcExpRiseFallRateData();
	}

	// 更新显示部分
	ReDrawAysnc();
}
void CIoViewTrend::OnVDataGeneralFinanaceUpdate( CMerch *pMerch )
{
	// 不需要财务数据
}

void CIoViewTrend::OnVDataMerchTrendIndexUpdate( CMerch *pMerch )
{
	// 个股: 量比 买卖力道
	if ( NULL == m_pMerchXml || m_pMerchXml != pMerch )
	{
		return;
	}
	E_TrendBtnType eBtn = GetCurTrendBtnType();
	E_ReportType eMerchType = GetMerchKind(m_pMerchXml);
	if ( ERTStockCn != eMerchType )
	{
		ASSERT( 0 );
		return;
	}

	if ( ETBT_VolRate == eBtn )
	{
		// 量比数据
		CalcVolRateData();
		ReDrawAysnc();
	}
	else if ( ETBT_BuySellForce == eBtn )
	{
		// 买卖力道数据
		CalcBuySellForceData();
		ReDrawAysnc();
	}
}

void CIoViewTrend::OnVDataMerchAuctionUpdate( CMerch *pMerch )
{
	// 个股: 竞价图
	if ( NULL == m_pMerchXml || m_pMerchXml != pMerch )
	{
		return;
	}
	E_TrendBtnType eBtn = GetCurTrendBtnType();
	E_ReportType eMerchType = GetMerchKind(m_pMerchXml);
	if ( ERTStockCn != eMerchType )
	{
		ASSERT( 0 );
		return;
	}

	if ( ETBT_CompetePrice == eBtn )
	{
		// 集合竞价数据
		CalcCompetePriceValue();		
		ReDrawAysnc();
	}
}

bool32 CIoViewTrend::RequestTrendPlusData()
{
	T_MerchNodeUserData* pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == pData )
	{
		return false;
	}

	// 
	CMerch* pMerch = pData->m_pMerchNode;
	if ( NULL == pMerch )
	{
		return false;
	}

	E_TrendBtnType eBtn = GetCurTrendBtnType();
	E_ReportType eMerchType = GetMerchKind(pMerch);
	int32 iMarketId = pMerch->m_MerchInfo.m_iMarketId;
	CString StrMerchCode = pMerch->m_MerchInfo.m_StrMerchCode;
	// 红绿柱只出现在0-000001 1000-399001上, 多空与红绿柱一起
	// 涨跌率需要一般指数数据中涨跌家数 段
	bool32 bReqGeneral = false; //ETBT_RiseFallRate == eBtn; // 涨跌率数据与红绿柱数据处于同结构中，目前同限制
	bReqGeneral = IsRedGreenMerch(pMerch);

	if ( bReqGeneral )
	{
		CMmiReqGeneralNormal reqG;
		reqG.m_iMarketId = iMarketId;
		DoRequestViewData(reqG);
	}

	if ( ERTStockCn == eMerchType
		&& ETBT_CompetePrice == eBtn )
	{
		// 竞价图
		CMmiReqMerchAution reqAuction;
		reqAuction.m_iMarketId = iMarketId;
		reqAuction.m_StrMerchCode = StrMerchCode;
		DoRequestViewData(reqAuction);

		// 竞价图的实时推送数据通过RealTimePrice合成
	}
	else if ( ERTStockCn == eMerchType
		&& (ETBT_BuySellForce == eBtn
		|| ETBT_VolRate == eBtn ) )
	{
		// 买卖力道 || 量比
		CMmiReqMerchTrendIndex reqTrendIndex;
		reqTrendIndex.m_iMarketId = iMarketId;
		reqTrendIndex.m_StrMerchCode = StrMerchCode;
		DoRequestViewData(reqTrendIndex);
	}

	RequestTrendPlusPushData();	// 注册推送

	return true;
}

bool32 CIoViewTrend::RequestTrendPlusPushData()
{
	T_MerchNodeUserData* pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == pData )
	{
		return false;
	}

	// 
	CMerch* pMerch = pData->m_pMerchNode;
	if ( NULL == pMerch )
	{
		return false;
	}

	bool32 bHasReqPush = false;

	E_TrendBtnType eBtn = GetCurTrendBtnType();
	E_ReportType eMerchType = GetMerchKind(pMerch);
	int32 iMarketId = pMerch->m_MerchInfo.m_iMarketId;
	CString StrMerchCode = pMerch->m_MerchInfo.m_StrMerchCode;
	// 红绿柱只出现在0-000001 1000-399001上, 多空与红绿柱一起
	// 涨跌率需要一般指数数据中涨跌家数 段
	bool32 bReqGeneral = false; //ETBT_RiseFallRate == eBtn; // 涨跌率数据与红绿柱数据处于同结构中，目前同限制
	bReqGeneral = IsRedGreenMerch(pMerch);

	// 此处仅处理推送请求
	if ( bReqGeneral )
	{
		CMmiReqAddPushGeneralNormal reqGPush;
		reqGPush.m_iMarketId = iMarketId;
		DoRequestViewData(reqGPush);	// 若干分钟还需要重新补充推送请求~~

		bHasReqPush = true;
	}
	//else
	//{
	//	// 不能请求删除的，因为其它视图还可能需要这个数据
	//	CMmiReqDelPushGeneralNormal reqGDelPush;
	//	reqGDelPush.m_iMarketId = iMarketId;
	// 	DoRequestViewData(reqGDelPush);
	//}

	// 竞价图仅依靠实时推送来组成竞价推送，这里不处理
	bool32 bReqMerchTrendIndex = false;
	if ( ERTStockCn == eMerchType
		&& (ETBT_BuySellForce == eBtn
		|| ETBT_VolRate == eBtn ) )
	{
		// 买卖力道 || 量比
		// 请求该推送数据
		CMmiReqAddPushMerchTrendIndex reqTrendIndexPush;
		reqTrendIndexPush.m_iMarketId = iMarketId;
		reqTrendIndexPush.m_StrMerchCode = StrMerchCode;
		DoRequestViewData(reqTrendIndexPush);

		bHasReqPush = true;
		bReqMerchTrendIndex = true;
	}

	//if ( !bReqMerchTrendIndex )
	//{
	//	// 不能请求删除的，因为其它视图还可能需要这个数据
	//	CMmiReqDelPushMerchTrendIndex reqDelPush;
	//	reqDelPush.m_iMarketId = iMarketId;
	//	reqDelPush.m_StrMerchCode = StrMerchCode;
	//	DoRequestViewData(reqDelPush);
	//}

	return bHasReqPush;
}

bool32 CIoViewTrend::IsRedGreenMerch( CMerch *pMerch )
{
	if ( NULL == pMerch )
	{
		return false;
	}
	int32 iMarketId = pMerch->m_MerchInfo.m_iMarketId;
	CString StrMerchCode = pMerch->m_MerchInfo.m_StrMerchCode;
	// 红绿柱只出现在0-000001 1000-399001上, 多空与红绿柱一起
	// 涨跌率需要一般指数数据中涨跌家数 段
	// 涨跌率数据与红绿柱数据处于同结构中，目前同限制
	if ( (0 == iMarketId && 0==StrMerchCode.Compare(_T("000001")))
		|| (1000 == iMarketId && 0==StrMerchCode.Compare(_T("399001"))) )
	{
		return true;
	}
	return false;
}

void CIoViewTrend::CalcTrendPlusSubRegionData()
{
	T_MerchNodeUserData *pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( pData == NULL || NULL == pData->m_pMerchNode )
	{
		return;
	}

	if ( IsRedGreenMerch(pData->m_pMerchNode) )
	{
		CalcRedGreenValue();	// 重算红绿线
	}
	else
	{
		CalcNoWeightExpLine();	// 均线可能需要重置
	}

	E_TrendBtnType eBtn = GetCurTrendBtnType();
	switch ( eBtn )
	{
	case ETBT_DuoKong:	// 多空 - 指
		CalcDuoKongData();
		break;
	case ETBT_RiseFallRate:	// 涨跌率 -指
		CalcExpRiseFallRateData();
		break;
	case ETBT_VolRate:	// 量比 - 个
		CalcVolRateData();
		break;
	case ETBT_BuySellForce:	// 买卖力道 - 个
		CalcBuySellForceData();
		break;
	case ETBT_CompetePrice:	// 竞价图
		CalcCompetePriceValue();
		break;
	default:	// 其它数据无需通过此方法计算
		break;
	}
}

CString	GetSpecialCurveTitle(int32 iSpecialId)
{
	static CStringArray s_aNames;
	if ( s_aNames.GetSize() <= 0 )
	{
		s_aNames.Add(_T("多空"));
		s_aNames.Add(_T("涨家"));
		s_aNames.Add(_T("跌家"));
		s_aNames.Add(_T(""));		// 涨跌差值
		s_aNames.Add(_T("量比"));
		s_aNames.Add(_T("委买"));
		s_aNames.Add(_T("委卖"));
		s_aNames.Add(_T(""));		// 买卖力道差值
	}

	if ( iSpecialId > 0 && iSpecialId <= s_aNames.GetSize() )
	{
		return s_aNames[iSpecialId-1];
	}
	return CString();
}

DWORD GetSpecialCurveIndexDrawStyle(int32 iSpecialId)
{
	static CArray<DWORD, DWORD> s_aStyles;
	if ( s_aStyles.GetSize() <= 0 )
	{
		s_aStyles.Add( 0 );	// 多空 0 默认曲线~
		s_aStyles.Add( 0 );  // 涨家
		s_aStyles.Add( 0 );	// 跌家
		s_aStyles.Add( CIndexDrawer::EIDSColorStick );		// 涨跌差值
		s_aStyles.Add( 0 );	// 量比
		s_aStyles.Add( 0 );	// 委买
		s_aStyles.Add( 0 );	// 委卖
		s_aStyles.Add( CIndexDrawer::EIDSColorStick );		// 买卖力道差值
	}
	if ( iSpecialId > 0 && iSpecialId <= s_aStyles.GetSize() )
	{
		return s_aStyles[iSpecialId-1];
	}
	return 0;
}

CChartCurve	* CIoViewTrend::GetSpecialCurve( int32 iSpecailId )
{
	if ( iSpecailId <= ETSC_Start
		|| iSpecailId >= ETSC_Count )
	{
		return NULL;
	}

	uint32 uPickColor;
	CDrawingCurve *pDstCurve = NULL;
	if ( !GetSpecialCurvePickColor(iSpecailId, uPickColor)
		|| NULL == (pDstCurve=CDrawingCurve::GetCurve(uPickColor)) )
	{
		// 已经没有这个线了
		return NULL;
	}

	E_TrendBtnType eBtn = GetCurTrendBtnType();
	ASSERT( ETBT_DuoKong==eBtn || ETBT_RiseFallRate==eBtn || ETBT_VolRate==eBtn || ETBT_BuySellForce==eBtn );

	if ( m_SubRegions.GetSize() != 2 )
	{
		return NULL;
	}

	CChartRegion *pRegion = m_SubRegions[1].m_pSubRegionMain;
	if ( NULL != pRegion )
	{
		for ( int32 i=0; i < pRegion->GetCurveNum() ; i++ )
		{
			CChartCurve *pCurve = pRegion->GetCurve(i);
			if ( pCurve == pDstCurve )
			{
				return pCurve;
			}
		}
	}
	ASSERT( 0 );		// 还存在的线就应该在region中的
	return NULL;
}

void CIoViewTrend::AddSpecialCurveToSubRegion()
{
	T_MerchNodeUserData* pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == m_pRegionMain || NULL == pData )
	{
		ASSERT( 0 );
		return;
	}

	E_TrendBtnType eBtn = GetCurTrendBtnType();
	CArray<int32, int32> aSpecialIds;
	switch (eBtn)
	{
	case ETBT_DuoKong:
		aSpecialIds.Add(ETSC_DuoKongCurve);
		break;
	case ETBT_RiseFallRate:
		aSpecialIds.Add(ETSC_RiseFall_RiseCurve);
		aSpecialIds.Add(ETSC_RiseFall_FallCurve);
		aSpecialIds.Add(ETSC_RiseFall_DiffCurve);
		break;
	case ETBT_VolRate:
		aSpecialIds.Add(ETSC_VolRate);
		break;
	case ETBT_BuySellForce:
		aSpecialIds.Add(ETSC_BuySell_BuyCurve);
		aSpecialIds.Add(ETSC_BuySell_SellCurve);
		aSpecialIds.Add(ETSC_BuySell_DiffCurve);
		break;
	default:
		ASSERT( 0 );
		break;
	}
	if ( aSpecialIds.GetSize() <= 0 )
	{
		return;
	}

	ASSERT( m_SubRegions.GetSize() == 1 || m_SubRegions.GetSize() == 2 );
	if ( m_SubRegions.GetSize() == 1 )
	{
		// 需要添加非成交量副图
		if ( !AddSubRegion(false) || m_SubRegions.GetSize() == 1 )
		{
			ASSERT( 0 );
			return;	// 无法创建副图
		}
	}

	CChartRegion *pSubRegion = NULL;
	if ( m_SubRegions.GetSize() != 2 )
	{
		ASSERT( 0 );
		return;
	}
	pSubRegion = m_SubRegions[1].m_pSubRegionMain;

	if ( NULL == pSubRegion )
	{
		ASSERT( 0 );
		return;
	}

	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);

	DeleteRegionCurves(pSubRegion);		// 移除所有线

	for ( int32 i=0; i < aSpecialIds.GetSize() ; i++ )
	{
		// 
		CString StrCurveTitle = GetSpecialCurveTitle(aSpecialIds[i]);
		DWORD dwDrawStyle = GetSpecialCurveIndexDrawStyle(aSpecialIds[i]);

		// 所有这些线都是时间点对齐的，so 貌似可以不截断了~~
		uint32 iFlag = CChartCurve::KTypeTrend|CChartCurve::KUseNodesNameAsTitle|CChartCurve::KYTransformByClose|CChartCurve::KTimeIdAlignToXAxis |CChartCurve::KDonotPick;

		if ( pSubRegion->GetCurveNum() == 0 )
		{
			AddFlag(iFlag,CChartCurve::KDependentCurve);
		}

		//
		CChartCurve *pCurve = pSubRegion->CreateCurve(iFlag);
		SetSpecialCurvePickColor(aSpecialIds[i], pCurve->GetPickColor());	// 保存特征索引

		pCurve->SetTitle(StrCurveTitle);
		CIndexDrawer* pDrawer = new CIndexDrawer(*this, (CIndexDrawer::E_IndexDrawStyle)dwDrawStyle, CIndexDrawer::EIDSENone, -1, CLR_DEFAULT);
		pCurve->AttatchDrawer(pDrawer);

		//
		CNodeSequence *pNodes = CreateNodes();
		pNodes->SetName(StrCurveTitle);		// 还有一个最后数据点~~悲剧
		pNodes->SetUserData(pData);
		pCurve->AttatchNodes(pNodes);

		// 凑合着搞个颜色
		pCurve->m_clrTitleText = GetIoViewColor((E_SysColor)(ESCGuideLine1+i%16));
	}

	UpdateSelfDrawCurve();

	// 计算数据~~
	CalcTrendPlusSubRegionData();

	Invalidate();
}

void CIoViewTrend::SetSpecialCurvePickColor( int32 iSpecialId, uint32 uPickColor )
{
	m_mapSpecialCurves[iSpecialId] = uPickColor;
}

bool32 CIoViewTrend::GetSpecialCurvePickColor( int32 iSpecialId, OUT uint32 &uPickColor )
{
	SpecialCurvePickColorMap::iterator it = m_mapSpecialCurves.find(iSpecialId);
	if ( it != m_mapSpecialCurves.end() )
	{
		uPickColor = it->second;
		return true;
	}
	return false;
}

void CIoViewTrend::ShowProperAvgLine()
{
	T_MerchNodeUserData *pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == pData
		|| NULL == pData->m_pMerchNode
		|| NULL == m_pRegionMain )
	{
		ASSERT( 0 );
		return;
	}

	bool32 bIsRedGreenExp = IsRedGreenMerch(pData->m_pMerchNode);
	if ( bIsRedGreenExp )
	{
		if ( NULL != m_pNoWeightExpCurve )
		{
			RemoveFlag(m_pNoWeightExpCurve->m_iFlag, CChartCurve::KInVisible);	// 显示不含权线
		}
		for ( int32 i=0; i < m_pRegionMain->m_Curves.GetSize() ; i++ )
		{
			CChartCurve *pCurve = m_pRegionMain->m_Curves[i];
			if ( NULL != pCurve
				&& CheckFlag(pCurve->m_iFlag, CChartCurve::KYTransformByAvg) )
			{
				// 找到普通均线, 隐藏它
				AddFlag(pCurve->m_iFlag, CChartCurve::KInVisible);
				break;
			}
		}
	}
	else
	{
		// 普通均线
		if ( NULL != m_pNoWeightExpCurve )
		{
			if ( NULL != m_pNoWeightExpCurve->GetNodes() )
			{
				m_pNoWeightExpCurve->GetNodes()->RemoveAll();
			}
			AddFlag(m_pNoWeightExpCurve->m_iFlag, CChartCurve::KInVisible);	// 隐藏不含权线
		}

		if ( NULL != m_pRegionMain )
		{
			for ( int32 i=0; i < m_pRegionMain->m_Curves.GetSize() ; i++ )
			{
				CChartCurve *pCurve = m_pRegionMain->m_Curves[i];
				if ( NULL != pCurve
					&& CheckFlag(pCurve->m_iFlag, CChartCurve::KYTransformByAvg) )
				{
					// 找到普通均线, 去掉隐藏
					RemoveFlag(pCurve->m_iFlag, CChartCurve::KInVisible);
					break;
				}
			}
		}
	}
}

//lint --e{429}
void CIoViewTrend::CalcNoWeightExpLine()
{
	T_MerchNodeUserData *pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == pData
		|| NULL == pData->m_pMerchNode
		|| NULL == m_pRegionMain )
	{
		ASSERT( 0 );
		return;
	}

	bool32 bIsRedGreenExp = IsRedGreenMerch(pData->m_pMerchNode);
	if ( bIsRedGreenExp )
	{
		// 不含权均线
		if ( NULL == m_pNoWeightExpCurve )
		{
			// 新建一条红绿柱状线，只有指数时才赋值，其它时候不赋值, 仅赋值收盘价
			CNodeSequence *pNodes = new CNodeSequence();
			pNodes->SetName(_T(""));
			pNodes->SetUserData(m_MerchParamArray[0]);
			CChartCurve *pCurveNoWeightExp = m_pRegionMain->CreateCurve(CChartCurve::KInVisible |CChartCurve::KTypeTrend |CChartCurve::KYTransformByClose |CChartCurve::KTimeIdAlignToXAxis|CChartCurve::KDonotPick);
			pCurveNoWeightExp->SetTitle(_T(""));
			pCurveNoWeightExp->m_clrTitleText  = GetIoViewColor(ESCVolume);				
			CChartDrawer* pJunDrawer   = new CChartDrawer(*this, CChartDrawer::EKDSTrendJunXian);
			pCurveNoWeightExp->AttatchDrawer(pJunDrawer);
			pCurveNoWeightExp->AttatchNodes(pNodes);
			m_pNoWeightExpCurve = pCurveNoWeightExp;
			m_pNoWeightExpCurve->SetAxisYType(GetMainCurveAxisYType());
		}

		ASSERT( NULL != m_pNoWeightExpCurve );
		CNodeSequence *pNodes = m_pNoWeightExpCurve->GetNodes();
		ASSERT( NULL != pNodes );

		// 计算数据, 不含权指数为涨跌幅数据...0.10表示涨0.10%
		pNodes->RemoveAll();
		const CAbsCenterManager::GeneralNormalArray *pGeneralNormals = m_pAbsCenterManager->GetGeneralNormalArray(m_pMerchXml);

		float fPricePreClose = GetTrendPrevClose();
		// 将相关值映射到坐标范围内
		if ( NULL != pGeneralNormals
			&& fPricePreClose > 0.0f )
		{
			const int32 iSize = pGeneralNormals->GetSize();
			pNodes->GetNodes().SetSize(iSize);
			CNodeData *pNodeDatas = pNodes->GetNodes().GetData();
			const T_GeneralNormal *pSrcData = pGeneralNormals->GetData();
			for ( int32 i=0; i < iSize ; i++ )
			{
				// 全把他赋值到close中
				//TRACE(_T("不含权均线: %d - %f%%\n"),pSrcData[i].m_lTime, pSrcData[i].m_fRiseRateNoWeighted);
				//float fCur = pSrcData[i].m_fRiseRateNoWeighted/100 + 0.0f;
				float fCur = pSrcData[i].m_fRiseRateNoWeighted + 0.0f;
				// 转换为价格
				bool32 b = CPriceToAxisYObject::AxisYToPriceYByBase(CPriceToAxisYObject::EAYT_Pecent, fPricePreClose, fCur, fCur);
				ASSERT( b );
				pNodeDatas[i].m_fClose = fCur;

				CGmtTime Time1(pSrcData[i].m_lTime);
				SaveMinute(Time1);
				pNodeDatas[i].m_iID = Time1.GetTime();
			}
		}
		TrimPlusShowNodeData(pNodes);

		RemoveFlag(m_pNoWeightExpCurve->m_iFlag, CChartCurve::KInVisible);	// 显示不含权线
		for ( int32 i=0; i < m_pRegionMain->m_Curves.GetSize() ; i++ )
		{
			CChartCurve *pCurve = m_pRegionMain->m_Curves[i];
			if ( NULL != pCurve
				&& CheckFlag(pCurve->m_iFlag, CChartCurve::KYTransformByAvg) )
			{
				// 找到普通均线, 隐藏它
				AddFlag(pCurve->m_iFlag, CChartCurve::KInVisible);
				break;
			}
		}
	}
	else
	{
		// 普通均线
		if ( NULL != m_pNoWeightExpCurve )
		{
			if ( NULL != m_pNoWeightExpCurve->GetNodes() )
			{
				m_pNoWeightExpCurve->GetNodes()->RemoveAll();
			}
			AddFlag(m_pNoWeightExpCurve->m_iFlag, CChartCurve::KInVisible);	// 隐藏不含权线
		}

		if ( NULL != m_pRegionMain )
		{
			for ( int32 i=0; i < m_pRegionMain->m_Curves.GetSize() ; i++ )
			{
				CChartCurve *pCurve = m_pRegionMain->m_Curves[i];
				if ( NULL != pCurve
					&& CheckFlag(pCurve->m_iFlag, CChartCurve::KYTransformByAvg) )
				{
					// 找到普通均线, 去掉隐藏
					RemoveFlag(pCurve->m_iFlag, CChartCurve::KInVisible);
					break;
				}
			}
		}
	}
}

void CIoViewTrend::CalcDuoKongData()
{
	ASSERT( m_MerchParamArray.GetSize() > 0 && m_pMerchXml != NULL );
	T_MerchNodeUserData *pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == pData )
	{
		return;
	}

	if ( m_SubRegions.GetSize() != 2 )
	{
		return;	// 不存在第二副图，没有初始化准备好，3
	}

	CChartCurve *pCurve = GetSpecialCurve(ETSC_DuoKongCurve);
	CNodeSequence *pNodes = NULL;
	if ( NULL == pCurve
		|| NULL == (pNodes=pCurve->GetNodes()) )
	{
		ASSERT( 0 );
		return;
	}

	pNodes->SetNamePostfix(_T(":0.00"));

	const CAbsCenterManager::GeneralNormalArray *pGeneralNormals = m_pAbsCenterManager->GetGeneralNormalArray(m_pMerchXml);


	// 多空数据: 0-000001 1000-399001
	E_TrendBtnType eBtn = GetCurTrendBtnType();
	if ( ETBT_DuoKong == eBtn
		&& IsRedGreenMerch(m_pMerchXml)
		&& NULL != pGeneralNormals )
	{
		CMarket &Market = pData->m_pMerchNode->m_Market;

		// 将相关值映射到坐标范围内
		const int32 iSize = pGeneralNormals->GetSize();
		pNodes->GetNodes().SetSize(iSize);
		CNodeData *pNodeDatas = pNodes->GetNodes().GetData();
		const T_GeneralNormal *pSrcData = pGeneralNormals->GetData();
		for ( int32 i=0; i < iSize ; i++ )
		{
			// 全把他赋值到close中
			float fCur = pSrcData[i].m_fAmountBuy - pSrcData[i].m_fAmountSell;
			float fTotal = pSrcData[i].m_fAmountBuy+pSrcData[i].m_fAmountSell;
			if ( fTotal == 0.0f )
			{
				fCur = 0.0f;
			}
			else
			{
				fCur /= fTotal;
			}
			pNodeDatas[i].m_fClose = fCur;
			CGmtTime Time1(pSrcData[i].m_lTime);
			SaveMinute(Time1);
			pNodeDatas[i].m_iID = Time1.GetTime();
		}

		TrimPlusShowNodeData(pNodes);

		// 是否将最后的数值显示~~
		if ( pNodes->GetSize() > 0 )
		{
			float fDuoKong = pNodes->GetNodes()[pNodes->GetSize()-1].m_fClose;
			pNodes->SetNamePostfix(_T(":")+Float2String(fDuoKong, 2, true, false));
		}
	}
	else
	{
		pNodes->RemoveAll();
	}
}

void CIoViewTrend::CalcExpRiseFallRateData()
{
	ASSERT( m_MerchParamArray.GetSize() > 0 && m_pMerchXml != NULL );
	T_MerchNodeUserData *pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == pData )
	{
		return;
	}

	if ( m_SubRegions.GetSize() != 2 )
	{
		return;	// 不存在第二副图，没有初始化准备好，3
	}

	// 涨跌家数 3根线
	CChartCurve *pCurveRise = GetSpecialCurve(ETSC_RiseFall_RiseCurve);
	CChartCurve *pCurveFall = GetSpecialCurve(ETSC_RiseFall_FallCurve);
	CChartCurve *pCurveDiff = GetSpecialCurve(ETSC_RiseFall_DiffCurve);
	CNodeSequence *pNodesRise, *pNodesFall, *pNodesDiff;
	if ( NULL == pCurveRise
		|| NULL == (pNodesRise=pCurveRise->GetNodes())
		|| NULL == pCurveFall
		|| NULL == (pNodesFall=pCurveFall->GetNodes())
		|| NULL == pCurveDiff
		|| NULL == (pNodesDiff=pCurveDiff->GetNodes()))
	{
		ASSERT( 0 );
		return;
	}

	pNodesRise->SetNamePostfix(_T(":0"));
	pNodesFall->SetNamePostfix(_T(":0"));

	// 多空数据: 0-000001 1000-399001
	E_TrendBtnType eBtn = GetCurTrendBtnType();
	const CAbsCenterManager::GeneralNormalArray *pGeneralNormals = m_pAbsCenterManager->GetGeneralNormalArray(m_pMerchXml);
	if ( ETBT_RiseFallRate == eBtn
		&& IsRedGreenMerch(m_pMerchXml)
		&& NULL != pGeneralNormals )
	{
		CMarket &Market = pData->m_pMerchNode->m_Market;

		// 将相关值映射到坐标范围内
		const int32 iSize = pGeneralNormals->GetSize();
		pNodesRise->GetNodes().SetSize(iSize);
		pNodesFall->GetNodes().SetSize(iSize);
		pNodesDiff->GetNodes().SetSize(iSize);
		CNodeData *pNodeDatasRise = pNodesRise->GetNodes().GetData();
		CNodeData *pNodeDatasFall = pNodesFall->GetNodes().GetData();
		CNodeData *pNodeDatasDiff = pNodesDiff->GetNodes().GetData();
		const T_GeneralNormal *pSrcData = pGeneralNormals->GetData();
		float fMax = -FLT_MAX;
		float fMin = 0;
		for ( int32 i=0; i < iSize ; i++ )
		{
			// 全把他赋值到close中
			pNodeDatasRise[i].m_fClose = pSrcData[i].m_uiRiseCounts;
			pNodeDatasFall[i].m_fClose = pSrcData[i].m_uiFallCounts;
			float fV = max(pNodeDatasRise[i].m_fClose, pNodeDatasFall[i].m_fClose);
			float fM = min(pNodeDatasRise[i].m_fClose, pNodeDatasFall[i].m_fClose);
			fMax = max(fMax, fV);
			fMin = min(fMin, fM);

			int32 iTime = pSrcData[i].m_lTime;
			CGmtTime Time1(iTime);
			SaveMinute(Time1);
			pNodeDatasRise[i].m_iID = Time1.GetTime();
			pNodeDatasFall[i].m_iID = Time1.GetTime();
			pNodeDatasDiff[i].m_iID = Time1.GetTime();
		}

		if ( fMax == -FLT_MAX )
		{
			fMax = 0.0f;
		}
		const float fHalf = (fMax-fMin)/2;
		// 将diff值映射到[min-max]之间, 中间值0 映射到1/2处
		// diff的最大值不会超过fMax-fMin, 最小值不会小于fMin-fMax
		for (int i=0; i < iSize ; i++ )
		{
			float fV = pNodeDatasRise[i].m_fClose - pNodeDatasFall[i].m_fClose;
			fV /= 2;
			fV += fHalf;	// 加上最大值的偏移值
			pNodeDatasDiff[i].m_fClose = fV;
		}

		pCurveDiff->SetUserPriceMinMaxBaseValue(fMin);	// 设定diff线的用户最小值为0
		pCurveDiff->SetPriceBaseY(fHalf);			// 设定中值，由于是线性坐标，所以设定基础值对坐标无影响

		TrimPlusShowNodeData(pNodesRise);
		TrimPlusShowNodeData(pNodesFall);
		TrimPlusShowNodeData(pNodesDiff);

		if ( pNodesRise->GetSize() > 0 )
		{
			float fValue = pNodesRise->GetNodes()[pNodesRise->GetSize()-1].m_fClose;
			pNodesRise->SetNamePostfix(_T(":")+Float2String(fValue, 0, true, false));
		}
		if ( pNodesFall->GetSize() > 0 )
		{
			float fValue = pNodesFall->GetNodes()[pNodesFall->GetSize()-1].m_fClose;
			pNodesFall->SetNamePostfix(_T(":")+Float2String(fValue, 0, true, false));
		}
	}
	else
	{
		pNodesRise->RemoveAll();
		pNodesFall->RemoveAll();
		pNodesDiff->RemoveAll();
	}
}

void CIoViewTrend::CalcVolRateData()
{
	ASSERT( m_MerchParamArray.GetSize() > 0 && m_pMerchXml != NULL );
	T_MerchNodeUserData *pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == pData )
	{
		return;
	}

	if ( m_SubRegions.GetSize() != 2 )
	{
		return;	// 不存在第二副图，没有初始化准备好，3
	}

	// 量比
	CChartCurve *pCurve = GetSpecialCurve(ETSC_VolRate);
	CNodeSequence *pNodes;
	if ( NULL == pCurve
		|| NULL == (pNodes=pCurve->GetNodes())
		)
	{
		ASSERT( 0 );
		return;
	}

	pNodes->SetNamePostfix(_T(":0.00"));

	// 量比 有数据就算
	E_TrendBtnType eBtn = GetCurTrendBtnType();
	if ( ETBT_VolRate == eBtn )
	{
		CMerch *pMerch = pData->m_pMerchNode;

		// 将相关值映射到坐标范围内
		const int32 iSize = pMerch->m_aMerchTrendIndexs.GetSize();
		pNodes->GetNodes().SetSize(iSize);
		CNodeData *pNodesData = pNodes->GetNodes().GetData();
		const T_MerchTrendIndex *pSrcData = pMerch->m_aMerchTrendIndexs.GetData();
		for ( int32 i=0; i < iSize ; i++ )
		{
			// 全把他赋值到close中
			pNodesData[i].m_fClose = pSrcData[i].m_fVolumeRate;

			CGmtTime Time1(pSrcData[i].m_lTime);
			SaveMinute(Time1);
			pNodesData[i].m_iID = Time1.GetTime();
		}

		TrimPlusShowNodeData(pNodes);

		if ( pNodes->GetSize() > 0 )
		{
			float fValue = pNodes->GetNodes()[pNodes->GetSize()-1].m_fClose;
			pNodes->SetNamePostfix(_T(":")+Float2String(fValue, 2, true, false));
		}
	}
	else
	{
		pNodes->RemoveAll();
	}
}

void CIoViewTrend::CalcBuySellForceData()
{
	ASSERT( m_MerchParamArray.GetSize() > 0 && m_pMerchXml != NULL );
	T_MerchNodeUserData *pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == pData )
	{
		return;
	}

	if ( m_SubRegions.GetSize() != 2 )
	{
		return;	// 不存在第二副图，没有初始化准备好，3
	}

	// 买卖力道 3根线
	CChartCurve *pCurveBuy  = GetSpecialCurve(ETSC_BuySell_BuyCurve);
	CChartCurve *pCurveSell = GetSpecialCurve(ETSC_BuySell_SellCurve);
	CChartCurve *pCurveDiff = GetSpecialCurve(ETSC_BuySell_DiffCurve);
	CNodeSequence *pNodesBuy, *pNodesSell, *pNodesDiff;
	if ( NULL == pCurveBuy
		|| NULL == (pNodesBuy=pCurveBuy->GetNodes())
		|| NULL == pCurveSell
		|| NULL == (pNodesSell=pCurveSell->GetNodes())
		|| NULL == pCurveDiff
		|| NULL == (pNodesDiff=pCurveDiff->GetNodes()))
	{
		ASSERT( 0 );
		return;
	}

	pNodesBuy->SetNamePostfix(_T(":0"));
	pNodesSell->SetNamePostfix(_T(":0"));

	E_TrendBtnType eBtn = GetCurTrendBtnType();
	if ( ETBT_BuySellForce == eBtn )
	{
		CMerch *pMerch = pData->m_pMerchNode;

		// 将相关值映射到坐标范围内
		const int32 iSize = pMerch->m_aMerchTrendIndexs.GetSize();
		pNodesBuy->GetNodes().SetSize(iSize);
		pNodesSell->GetNodes().SetSize(iSize);
		pNodesDiff->GetNodes().SetSize(iSize);
		CNodeData *pNodeDatasBuy  = pNodesBuy->GetNodes().GetData();
		CNodeData *pNodeDatasSell = pNodesSell->GetNodes().GetData();
		CNodeData *pNodeDatasDiff = pNodesDiff->GetNodes().GetData();
		const T_MerchTrendIndex *pSrcData = pMerch->m_aMerchTrendIndexs.GetData();
		int32 iVolScale = pMerch->m_Market.m_MarketInfo.m_iVolScale;
		iVolScale = max(1, iVolScale);

		float fMax = -FLT_MAX;
		float fMin = 0.0f;
		for ( int32 i=0; i < iSize ; i++ )
		{
			// 全把他赋值到close中
			pNodeDatasBuy[i].m_fClose = pSrcData[i].m_fBuyVolumeSum/iVolScale;
			pNodeDatasSell[i].m_fClose = pSrcData[i].m_fSellVolumeSum/iVolScale;
			float fV = max(pNodeDatasSell[i].m_fClose, pNodeDatasBuy[i].m_fClose);
			float fM = min(pNodeDatasSell[i].m_fClose, pNodeDatasBuy[i].m_fClose);
			fMax = max(fV, fMax);
			fMin = min(fM, fMin);

			int32 iTime = pSrcData[i].m_lTime;
			CGmtTime Time1(iTime);
			SaveMinute(Time1);
			iTime = Time1.GetTime();
			pNodeDatasBuy[i].m_iID = iTime;
			pNodeDatasSell[i].m_iID = iTime;
			pNodeDatasDiff[i].m_iID = iTime;
		}

		if ( fMax == -FLT_MAX )
		{
			fMax = 0.0f;
		}
		const float fHalf = (fMax-fMin)/2;
		// 将diff值映射到[min-max]之间, 中间值0 映射到1/2处
		// diff的最大值不会超过fMax-fMin, 最小值不会小于fMin-fMax
		for ( int i=0; i < iSize ; i++ )
		{
			float fV = pNodeDatasBuy[i].m_fClose - pNodeDatasSell[i].m_fClose;
			fV /= 2;
			fV += fHalf;
			pNodeDatasDiff[i].m_fClose = fV;
		}

		pCurveDiff->SetUserPriceMinMaxBaseValue(fMin);	// 设定diff线的用户最小值为0
		pCurveDiff->SetPriceBaseY(fHalf);			// 设定中值，由于是线性坐标，所以设定基础值对坐标无影响

		TrimPlusShowNodeData(pNodesBuy);
		TrimPlusShowNodeData(pNodesSell);
		TrimPlusShowNodeData(pNodesDiff);

		if ( pNodesBuy->GetSize() > 0 )
		{
			float fValue = pNodesBuy->GetNodes()[pNodesBuy->GetSize()-1].m_fClose;
			pNodesBuy->SetNamePostfix(_T(":")+Float2String(fValue, 0, true, false));
		}
		if ( pNodesSell->GetSize() > 0 )
		{
			float fValue = pNodesSell->GetNodes()[pNodesSell->GetSize()-1].m_fClose;
			pNodesSell->SetNamePostfix(_T(":")+Float2String(fValue, 0, true, false));
		}
	}
	else
	{
		pNodesBuy->RemoveAll();
		pNodesSell->RemoveAll();
		pNodesDiff->RemoveAll();
	}
}
void CIoViewTrend::TrimPlusShowNodeData( INOUT CNodeSequence *pNodes )
{
	if ( NULL == pNodes )
	{
		ASSERT( 0 );
		return;
	}
	CGmtTime TimeStart, TimeEnd;
	if ( !GetTrendFirstDayOpenCloseTime(TimeStart, TimeEnd) )
	{
		ASSERT( 0 );	// 无效时间段？？
		return;
	}

	CNodeData *pNodesData = pNodes->GetNodes().GetData();
	const int32 iSize = pNodes->GetSize();
	if ( NULL == pNodesData || iSize <= 0 )
	{
		return;	// 无数据
	}

	int32 iPosStart, iPosEnd;
	if ( !pNodes->LookupEx(TimeStart.GetTime(), iPosStart)
		|| !pNodes->LookupEx(TimeEnd.GetTime(), iPosEnd) 
		|| iPosEnd < iPosStart
		|| iPosStart < 0
		|| iPosEnd >= iSize )
	{
		ASSERT( 0 );
		pNodes->RemoveAll();	// 全截掉
		return;	// 找不到合适的开始&终结点
	}
	// 从两个点位置排除不合适点
	while(iPosStart <= iPosEnd)
	{
		// [开始点 -> 结束点]
		if ( pNodesData[iPosStart].m_iID < TimeStart.GetTime() )
		{
			iPosStart++;
		}
		break;
	}
	while(iPosEnd >= iPosStart)
	{
		// [结束点 -> 开始点]
		if ( pNodesData[iPosEnd].m_iID > TimeEnd.GetTime() )
		{
			iPosEnd--;
		}
		break;
	}
	if ( iPosEnd < iPosStart )
	{
		// 没有合适数据
		pNodes->RemoveAll();
		return;
	}

	if ( iSize > iPosEnd+1 )
	{
		pNodes->GetNodes().RemoveAt(iPosEnd+1, iSize-iPosEnd-1);	// 删除多余尾部数据
	}
	if ( iPosStart > 0 )
	{
		pNodes->GetNodes().RemoveAt(0, iPosStart);	// 删除头部多余数据
	}
}

bool32 CIoViewTrend::OnGetCurveTitlePostfixString( CChartCurve *pCurve, CPoint ptClient, bool32 bHideCross, OUT CString &StrPostfix )
{
	// 仅第二副图需要额外处理这些
	E_TrendBtnType eBtn = GetCurTrendBtnType();
	if ( ETBT_DuoKong != eBtn
		&& ETBT_RiseFallRate != eBtn
		&& ETBT_VolRate != eBtn
		&& ETBT_BuySellForce != eBtn )
	{
		return false;
	}

	// 仅 第二副图需要
	if ( pCurve == NULL
		|| &pCurve->GetChartRegion() == m_pRegionMain
		|| m_SubRegions.GetSize() != 2
		|| m_SubRegions[1].m_pSubRegionMain != &pCurve->GetChartRegion() )
	{
		return false;
	}

	// 找到这个node
	CNodeSequence * pNodes = ((CChartCurve*)pCurve)->GetNodes();

	if ( NULL == pNodes)
	{
		return false;
	}
	if ( pNodes->GetName().IsEmpty() )
	{
		return false;
	}

	CNodeData NodeData;

	int32 iSize = pCurve->GetNodes()->GetSize();
	if (!bHideCross && iSize > 0)
	{		
		// 根据十字光标位置得到NodeData.						
		int32 ix = ptClient.x;
		int32 iPos = -1;

		pCurve->GetChartRegion().ClientXToRegion(ix);
		pCurve->RegionXToCurvePos(ix,iPos);

		// iPos代表的是坐标的位置, 而不是相关node的位置
		// 在普通正常情况下，应该是点点对应的
		// 如果在异常情况下，既然没有显示，那么提示错误也不是很错误
		// 如果以后需要修改，就全部都查询一下
		if ( iPos >= iSize || -1 == iPos )
		{
			const int32 iAxisCount = pCurve->GetChartRegion().m_aXAxisNodes.GetSize();
			if ( iPos >=0 && iPos < iAxisCount )
			{
				CAxisNode *pAxisNode = (CAxisNode *)pCurve->GetChartRegion().m_aXAxisNodes.GetData();
				int32 iTimeId = pAxisNode[iPos].m_iTimeId;
				if ( iTimeId > 0 )
				{
					// 根据实际的数据，找到时间相同点的显示数据
					CNodeData *pNodesData = pNodes->GetNodes().GetData();
					int32 iUp = iSize-1, iDown = 0;
					while ( iUp >= iDown )
					{
						int32 iHalf = (iUp+iDown)/2;
						int32 iId = pNodesData[iHalf].m_iID;
						if ( iId == iTimeId )
						{
							iPos = iHalf;	// 找到了
							break;
						}
						else if ( iId > iTimeId )
						{
							iUp = iHalf-1;
						}
						else
						{
							iDown = iHalf+1;
						}
					}
				}
			}
			if ( iPos >= iSize || -1 == iPos )
			{
				iPos = iSize - 1;
			}
		}

		pNodes->GetAt(iPos,NodeData);							
	}
	else if (iSize > 0)
	{
		// 得到最新的一个指标值											

		pNodes->GetAt(iSize-1,NodeData);
	}
	else
	{
		// 没有数据
		NodeData.m_iFlag |= CNodeData::KValueInvalid;	// 这个值无效
	}

	int32 iSaveDec = 0;
	if ( ETBT_DuoKong == eBtn
		|| ETBT_VolRate == eBtn )
	{
		iSaveDec = 2;
	}

	if (NodeData.m_fClose == FLT_MAX || NodeData.m_fClose == FLT_MIN || CheckFlag(NodeData.m_iFlag,CNodeData::KValueInvalid))
	{
		StrPostfix = _T(":")+ Float2String(0.0,iSaveDec,false, false);
	}
	else
	{
		StrPostfix = _T(":")+ Float2String(NodeData.m_fClose,iSaveDec,true, false);
	}

	return true;
}

bool32 CIoViewTrend::IsTimeInCallAuction( const CGmtTime &TimeNow )
{
	CGmtTime TimeStart, TimeEnd;
	if (ETBT_CompetePrice == GetCurTrendBtnType()
		&& GetCallAuctionTime(TimeStart, TimeEnd) )
	{
		{
			// [9:15 - 9:25] GMT[1:15 - 1:25]
			if ( TimeNow >= TimeStart && TimeNow <= TimeEnd )
			{
				return true;
			}
		}
	}
	return false;
}

bool32 CIoViewTrend::GetCallAuctionTime( OUT CGmtTime &TimeStart, OUT CGmtTime &TimeEnd )
{
	T_MerchNodeUserData *pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( pData != NULL
		&& pData->m_TrendTradingDayInfo.m_bInit
		&& ERTStockCn == GetMerchKind(pData->m_pMerchNode) )
	{
		// [9:15 - 9:25] GMT[1:15 - 1:25]
		CGmtTime Time2(pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time);
		SaveDay(Time2);
		TimeStart = Time2 + CGmtTimeSpan(0, 1, 15, 0);
		TimeEnd = Time2 + CGmtTimeSpan(0, 1, 25, 30);
		return true;
	}
	return false;
}

void CIoViewTrend::OnVDataLandMineUpdate( IN CMerch* pMerch )
{
	if ( pMerch == m_pMerchXml && NULL != pMerch )
	{
		bool32 bSame = pMerch->m_mapLandMine.size() == m_mapLandMine.size();
		mapLandMine::iterator it2 = pMerch->m_mapLandMine.begin();
		for ( mapLandMine::iterator it1 = m_mapLandMine.begin(); it1 != m_mapLandMine.end() && bSame ; ++it1, ++it2 )
		{
			const CLandMineTitle &title1 = it1->second;
			const CLandMineTitle &title2 = it2->second;
			if ( title1.m_iInfoIndex != title2.m_iInfoIndex
				|| title1.m_StrUpdateTime != title2.m_StrUpdateTime
				|| title1.m_StrCrc32 != title2.m_StrCrc32
				|| title1.m_iConLen != title2.m_iConLen )
			{
				// 仅认为这几个 唯一标志了一个地雷列表, 不关注内容的变化
				bSame = false;
				break;
			}
		}
		if ( !bSame )
		{
			// 更新
			m_mapLandMine = pMerch->m_mapLandMine;
			if ( NULL != m_pRegionMain )
			{
				if (
					m_MerchParamArray.GetSize() < 1 
					|| m_MerchParamArray[0] == NULL
					|| m_MerchParamArray[0]->m_pMerchNode == NULL )
				{
					return;
				}

				m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
				Invalidate();	// 更新显示
			}
		}
	}
}

void CIoViewTrend::OnVDataClientTradeTimeUpdate()
{
	if (m_MerchParamArray.GetSize() <= 0 || NULL == m_pMerchXml)
	{
		return;
	}

	// 重新更新一下开收盘时间点信息
	T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
	if (NULL == pData)
	{
		return;
	}

	// 获取该商品走势相关的数据
	CMarketIOCTimeInfo RecentTradingDay;
	if ( !m_bHistoryTrend )
	{
		if (m_pMerchXml->m_Market.GetRecentTradingDay(m_pAbsCenterManager->GetServerTime(), RecentTradingDay, m_pMerchXml->m_MerchInfo))
		{
			pData->m_TrendTradingDayInfo.Set(m_pMerchXml, RecentTradingDay);
			pData->m_TrendTradingDayInfo.RecalcPrice(*m_pMerchXml);
			pData->m_TrendTradingDayInfo.RecalcHold(*m_pMerchXml);
		}
	}
	else
	{
		// 历史分时 - 设定交易日信息(历史分时严格意义上不允许切换商品，因为每个商品的交易日信息并不相同)
		if ( m_pMerchXml->m_Market.GetSpecialTradingDayTime(m_TimeHistory, RecentTradingDay, m_pMerchXml->m_MerchInfo) )
		{
			pData->m_TrendTradingDayInfo.Set(m_pMerchXml, RecentTradingDay);
			pData->m_TrendTradingDayInfo.RecalcPrice(*m_pMerchXml);
			pData->m_TrendTradingDayInfo.RecalcHold(*m_pMerchXml);
		}
	}

	//
	m_aTrendMultiDayIOCTimes.RemoveAll();	// 清除时间点信息
	m_aTrendMultiDayIOCTimes.Add(pData->m_TrendTradingDayInfo.m_MarketIOCTime);		// 给与第一天的初始化时间

	//
	CIoViewChart::OnVDataForceUpdate();
	RequestViewData();

	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	ReDrawAysnc();
}

void CIoViewTrend::DrawLandMines( CMemDCEx *pDC )
{
	m_mapLandMinesRect.clear();

	if ( NULL == pDC 
		|| m_MerchParamArray.GetSize() < 1 
		|| m_MerchParamArray[0] == NULL
		|| m_MerchParamArray[0]->m_pMerchNode == NULL )
	{
		return;
	}

	T_MerchNodeUserData *pData = m_MerchParamArray[0];

	if ( m_mapLandMine.size() <= 0 )
	{
		return;
	}

	if ( NULL == m_pRegionMain || m_pRegionMain->m_aXAxisNodes.GetSize() <= 0)
	{
		return ;
	}

	if ( m_pRegionMain->m_aXAxisNodes.GetSize() <= 0 )
	{
		ASSERT( 0 );
		return;
	}

	// 画在顶上
	CRect rcRegion(0,0,0,0);
	rcRegion.bottom	= m_pRegionMain->GetRectCurves().top - 2;
	rcRegion.top = rcRegion.bottom - 8;		// 这么高的给画地雷
	rcRegion.left	= 0;
	rcRegion.right	= 0;
	const COLORREF clrText = RGB(255,0,255);
	int32 iSaveDC = pDC->SaveDC();
	pDC->SelectObject(CFaceScheme::Instance()->GetSysFontObject(ESFSmall));
	pDC->SetTextColor(clrText);
	pDC->SetBkMode(TRANSPARENT);
	CPen penLine;
	penLine.CreatePen(PS_SOLID, 0, clrText);
	pDC->SelectObject(&penLine);

	// 时间点的划分
	//   以大于左边时间点，则是当前时间点
	// 决定x坐标
	// 分时仅显示当天&&当天后的

	// x坐标定位方法:
	// 1. 按照所有收盘后发出的信息都是有利于第二天交易的信息，应当归为第二天的方法划分?
	// 2. 按照本地时间对应某日时间范围，属于该范围内的，为该日，不属于的属于前或后日
	// 3. 按照gmt时间对应交易时间范围，属于该范围的，为该日，不属于的属于前或后日

	// 依照2号方法处理

	int32 iNodeSize = m_pRegionMain->m_aXAxisNodes.GetSize();
	const int32 iPosStart = 0;
	int32 iPosCount = iNodeSize;
	CAxisNode	nodeStart = m_pRegionMain->m_aXAxisNodes[iPosStart];
	CAxisNode	nodeEnd	  = m_pRegionMain->m_aXAxisNodes[ iPosCount-1 ];
	CGmtTime timeEnd(nodeEnd.m_iTimeId);
	timeEnd += CGmtTimeSpan(0, 0, 0, 59);

	// 时间格式 2010-01-01 00:00:00 - 假设分时信息地雷的时间为 2010-01-01 01:01:01

	mapLandMine::reverse_iterator it = m_mapLandMine.rbegin();
	// 如果日记时间比显示的最右侧点+1的点的时间还晚，则不显示该日记，如果无右侧+1点，则不考虑晚日记的过滤
	if ( it != m_mapLandMine.rend() )
	{
		int32 iPosEndPlus = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pData->m_aKLinesFull, timeEnd);
		if ( iPosEndPlus >= 0 && iPosEndPlus < pData->m_aKLinesFull.GetSize() )
		{
			// 拥有右侧+1值, 过滤掉所有>=右侧时间点的值
			// 由于仅作用于交易，so交易时间过后不会出现重要信息地雷，可忽略
			timeEnd = pData->m_aKLinesFull[iPosEndPlus].m_TimeCurrent;
			CGmtTime TimeLocalStart, TimeLocalEnd;
			GetLocalDayGmtRange(timeEnd, TimeLocalStart, TimeLocalEnd);
			CString StrTime;
			StrTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d")
				, TimeLocalStart.GetYear(), TimeLocalStart.GetMonth(), TimeLocalStart.GetDay()
				, TimeLocalStart.GetHour(), TimeLocalStart.GetMinute(), TimeLocalStart.GetSecond());
			while ( it != m_mapLandMine.rend() && it->first >= StrTime )
			{
				++it;
			}
		}
	}


	CString StrStartTime;
	// 分时图的最左边一个点显示上个交易日结束后到这个交易日开盘时的所有数据
	// 精度原因取上个交易日后的一天作为开始时间, 如果没有上个交易日则今天的00:00:00
	// 仅显示当天&&当天后的信息
	bool32 bOnlyShowTaday = true;
	if (!bOnlyShowTaday)
	{
		// 请求的日k线中包含有上个交易日的信息
		int32 iDayPos =-1;
		CMerchKLineNode *pDayKline = NULL;
		if ( pData->m_pMerchNode->FindMerchKLineNode(EKTBDay, iDayPos, pDayKline)
			&& NULL != pDayKline
			&& pDayKline->m_KLines.GetSize() > 0 )
		{
			CGmtTime TimeCurDay = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time;
			SaveDay(TimeCurDay);
			int32 iPos = CMerchKLineNode::QuickFindKLineByTime(pDayKline->m_KLines, TimeCurDay);
			if ( iPos > 0 && iPos < pDayKline->m_KLines.GetSize() )
			{
				TimeCurDay = pDayKline->m_KLines[iPos-1].m_TimeCurrent;
				CMarketIOCTimeInfo IOC;
				if ( pData->m_pMerchNode->m_Market.GetSpecialTradingDayTime(TimeCurDay, IOC, pData->m_pMerchNode->m_MerchInfo) )
				{
					TimeCurDay = IOC.m_TimeEnd.m_Time + CGmtTimeSpan(0, 0, 1, 0);	// 结束时间后的1分钟划分为前一个交易日的后一天
					SaveDay(TimeCurDay);		// 这一整天的消息都属于开始数据点
					StrStartTime.Format(_T("%04d-%02d-%02d")
						, TimeCurDay.GetYear(), TimeCurDay.GetMonth(), TimeCurDay.GetDay() );
					// 这里并没有按照方法2区别本地时间!!!!
					ASSERT( 0 );
				}
			}
		}
	}
	if ( StrStartTime.IsEmpty() )
	{
		// 当天
		CGmtTime TimeCurDay = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time;
		SaveDay(TimeCurDay);
		CGmtTime TimeLocalStart, TimeLocalEnd;
		GetLocalDayGmtRange(TimeCurDay, TimeLocalStart, TimeLocalEnd);
		StrStartTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d")
			, TimeLocalStart.GetYear(), TimeLocalStart.GetMonth(), TimeLocalStart.GetDay()
			, TimeLocalStart.GetHour(), TimeLocalStart.GetMinute(), TimeLocalStart.GetSecond());

		// 限制最后一个显示的时间为所有可见点的最后一分钟
		timeEnd = CGmtTime(nodeEnd.m_iTimeId);
		timeEnd += CGmtTimeSpan(0, 0, 0, 59);
		CString StrTime;
		StrTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d")
			, timeEnd.GetYear(), timeEnd.GetMonth(), timeEnd.GetDay()
			, timeEnd.GetHour(), timeEnd.GetMinute(), timeEnd.GetSecond() );
		while ( it != m_mapLandMine.rend() && it->first >= StrTime )
		{
			++it;
		}
	}
	CRect rcCurves = m_pRegionMain->GetRectCurves();
	rcCurves.left += 2;
	rcCurves.right -= 2;
	for ( int32 i = iPosCount+iPosStart-1; i >= iPosStart ; i-- )
	{
		bool32 bNeedDraw = false;
		int32 iTimeNow = m_pRegionMain->m_aXAxisNodes[i].m_iTimeId;
		CGmtTime TimeNow(iTimeNow);
		CString StrTime;
		if ( iPosStart == i )
		{
			// 分时图的最左边一个点显示上个交易日结束后到这个交易日开盘时的所有数据
			if ( StrStartTime.IsEmpty() )
			{
				StrTime.Format(_T("%04d-%02d-%02d")
					, TimeNow.GetYear(), TimeNow.GetMonth(), TimeNow.GetDay() );
			}
			else
			{
				StrTime = StrStartTime;
			}
		}
		else
		{
			StrTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d")
				, TimeNow.GetYear(), TimeNow.GetMonth(), TimeNow.GetDay()
				, TimeNow.GetHour(), TimeNow.GetMinute(), TimeNow.GetSecond() );
			if ( bOnlyShowTaday
				&& StrTime < StrStartTime )
			{
				StrTime = StrStartTime;	// 约束仅有今天的显示
			}
		}

		CString StrMineTime;
		CRect rcNode(rcRegion);
		int32 iMines = 0;
		CString StrFirst;
		while ( it != m_mapLandMine.rend() && (StrFirst=it->first) >= StrTime )
		{
			// 信息地雷的时间点大于等于该时间点, 则属于该时间点绘制
			//const CLandMineTitle &title = it->second;
			int32 iCenter = m_pRegionMain->m_aXAxisNodes[i].m_iCenterPixel;

			rcNode.left  = iCenter - 4;
			rcNode.right = iCenter + 4;

			bNeedDraw = true;
			StrMineTime = it->first;
			iMines++;

			m_mapLandMinesRect.insert(make_pair(it->first, rcNode));		// 所有信息地雷的日时间都是一样的

			++it;
		}

		if ( bNeedDraw )
		{
			CPoint ptCenter = rcNode.CenterPoint();
			CRect rcPixel(ptCenter.x-1, ptCenter.y-1, ptCenter.x+1, ptCenter.y+1);
			if ( iMines > 1 )
			{
				// 有多个，画大一点
				rcPixel.top -= 1;
				rcPixel.left -= 1;
			}
			else
			{
				// 画小一点
			}

			if ( rcPixel.left < rcCurves.left )
			{
				rcPixel.OffsetRect(rcCurves.left-rcPixel.left, 0);
			}
			else if ( rcPixel.right > rcCurves.right )
			{
				rcPixel.OffsetRect(rcCurves.right-rcPixel.right, 0);
			}
			pDC->FillSolidRect(rcPixel, clrText);
			pDC->MoveTo(rcPixel.left-1, rcPixel.top-1);
			pDC->LineTo(rcPixel.TopLeft());
			pDC->MoveTo(rcPixel.right, rcPixel.top-1);
			pDC->LineTo(rcPixel.right, rcPixel.top);
			pDC->MoveTo(rcPixel.right, rcPixel.bottom);
			pDC->LineTo(rcPixel.right-1, rcPixel.bottom-1);
			pDC->MoveTo(rcPixel.left-1, rcPixel.bottom);
			pDC->LineTo(rcPixel.left, rcPixel.bottom-1);
			//pDC->DrawText(_T("*"), rcNode, DT_CENTER| DT_BOTTOM);	// 要不要绘制出大小来呢？
		}

		if ( it == m_mapLandMine.rend() )
		{
			break;
		}
	}

	pDC->RestoreDC(iSaveDC);
}

void CIoViewTrend::OnChartDBClick( CPoint ptClick, int32 iNodePos )
{
	if ( m_MerchParamArray.GetSize() <= 0
		|| m_MerchParamArray[0] == NULL
		|| m_MerchParamArray[0]->m_pMerchNode == NULL )
	{
		return;
	}

	// 历史分时双击不响应
	if (m_bHistoryTrend)
	{
		return;
	}

	CGGTongView* pGGTongView = GetParentGGtongView();
	pGGTongView->DealEnterKey();

	//CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	//if ( NULL != pMain )
	//{
	//    pMain->OnProcessF7(NULL, false);
	//}

	//	if ( m_MerchParamArray.GetSize() <= 0
	//		|| m_MerchParamArray[0] == NULL
	//		|| m_MerchParamArray[0]->m_pMerchNode == NULL )
	//	{
	//		return;
	//	}
	//	
	//// 	if ( NULL == m_pRegionMain || m_pRegionMain->m_aXAxisNodes.GetSize() < 1)
	//// 	{
	//// 		return ;
	//// 	}
	//// 	
	//// 	CChartCurve* pDependCurve = m_pRegionMain->GetDependentCurve();
	//// 	
	//// 	if ( NULL == pDependCurve)
	//// 	{
	//// 		return ;
	//// 	}
	//// 	
	//// 	// 	
	//// 	CNodeSequence* pNode = pDependCurve->GetNodes();	
	//// 	
	//// 	if ( NULL == pNode)
	//// 	{
	//// 		return;
	//// 	}
	//	
	//// 	if ( m_pRegionMain->m_aXAxisNodes.GetSize() < pNode->GetNodes().GetSize() )
	//// 	{
	//// 		ASSERT( 0 );
	//// 		return;
	//// 	}
	//	
	if ( m_pRegionMain->m_aXAxisNodes.GetSize() <= iNodePos || iNodePos < 0 )
	{
		//ASSERT( 0 );
		return;
	}

	// 是否双击了信息地雷
	{
		CString StrMineTime;
		for ( LandMineRectMap::reverse_iterator it = m_mapLandMinesRect.rbegin(); it != m_mapLandMinesRect.rend() ; ++it )
		{
			if ( it->second.PtInRect(ptClick) )
			{
				StrMineTime = it->first;
				break;
			}
		}

		if ( !StrMineTime.IsEmpty() )
		{
			LandMineRectMap::_Pairii Mines = m_mapLandMinesRect.equal_range(StrMineTime);

			if ( Mines.first != Mines.second )
			{
				int32 iYear, iMonth, iDay;
				int32 iHour, iMin, iSec;
				if (_stscanf(StrMineTime, _T("%d-%d-%d %d:%d:%d"), &iYear, &iMonth, &iDay, &iHour, &iMin, &iSec) == 6)
				{
					CGmtTime TimeClick(iYear, iMonth, iDay, iHour, iMin, iSec);
					CDlgNotePad::ShowNotePad(CDlgNotePad::EST_InfoMine, &TimeClick);
					return;
				}
			}	
		}
	}
}

bool32 CIoViewTrend::GetTips( IN CPoint pt, OUT CString& StrTips, OUT CString &StrTitle )
{
	// 得到Tips
	StrTips = L"";

	CWnd *p1 = GetParentOwner();
	CWnd *p2 = CWnd::FromHandlePermanent(::GetLastActivePopup(AfxGetMainWnd()->GetSafeHwnd()));
	if ( m_bHistoryTrend
		&& p1 != p2 )
	{
		return false;	// 如果顶层窗口没有active，那么就不用提示了
	}

	// 信息地雷
	{
		if ( m_mapLandMinesRect.size() > 0 )
		{
			CStringArray aStrMineTimes;
			for ( LandMineRectMap::iterator it = m_mapLandMinesRect.begin(); it != m_mapLandMinesRect.end() ; ++it )
			{
				if ( it->second.PtInRect(pt) )
				{
					aStrMineTimes.Add(it->first);
				}
			}

			set<CString> setTimes;
			for ( int32 i=0; i < aStrMineTimes.GetSize() ; ++i )
			{
				const CString &StrMineTime = aStrMineTimes[i];
				if ( !StrMineTime.IsEmpty() && setTimes.count(StrMineTime)==0 )
				{
					setTimes.insert(StrMineTime);
					mapLandMine::_Pairii Mines = m_mapLandMine.equal_range(StrMineTime);

					if ( Mines.first != Mines.second )
					{
						if ( StrTitle.IsEmpty() )
						{
							StrTitle = _T("实时信息地雷(双击查看)");
						}
						// 按照日期顺序否？
						for ( mapLandMine::iterator it=Mines.first; it != Mines.second; ++it )
						{
							const CLandMineTitle &title = it->second;
							StrTips.Insert(0, title.m_StrText + _T("\r\n"));
						}
					}
				}
			}
			if ( !StrTitle.IsEmpty() )
			{
				return true;
			}
		}
	}
	return false;
}

void CIoViewTrend::ShowProperTrendPlusCurve()
{
	T_MerchNodeUserData *pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == pData
		|| NULL == pData->m_pMerchNode
		|| NULL == m_pRegionMain )
	{
		ASSERT( 0 );
		return;
	}

	ShowProperAvgLine();	// 不含权线

	bool32 bIsRedGreenExp = IsRedGreenMerch(pData->m_pMerchNode);
	if ( bIsRedGreenExp )
	{
		// 主图红绿线
		if ( NULL != m_pRedGreenCurve )
		{
			RemoveFlag(m_pRedGreenCurve->m_iFlag, CChartCurve::KInVisible);	// 显示
		}
	}
	else
	{
		if ( NULL != m_pRedGreenCurve )
		{
			AddFlag(m_pRedGreenCurve->m_iFlag, CChartCurve::KInVisible);	// 隐藏
		}
	}
}

bool32 CIoViewTrend::IsShowTrendFirstOpenPrice()
{
	return !m_bHistoryTrend 
		&& m_iTrendDayCount == 1 
		&& !m_bSingleSectionTrend
		&& NULL != m_pMerchXml
		&& (ERTStockCn == m_pMerchXml->m_Market.m_MarketInfo.m_eMarketReportType
		|| IsRedGreenMerch(m_pMerchXml) );	
	// 只有在非历史 单日分时 国内股票下才画
}

void CIoViewTrend::DrawTrendFirstOpenPrice( CMemDCEx *pDC )
{
	CChartCurve *pCurve = NULL;
	if ( NULL != pDC 
		&& IsShowTrendFirstOpenPrice()
		&& NULL != m_pRegionMain
		&& NULL != (pCurve=m_pRegionMain->GetDependentCurve())
		&& m_pRegionMain->m_aXAxisNodes.GetSize() > 0 )
	{
		// 找到显示线的第一根节点
		CNodeSequence *pNodes = pCurve->GetNodes();
		CNodeData *pNodesData = NULL;
		int32 iSize = 0;
		if ( NULL != pNodes )
		{
			iSize = pNodes->GetSize();
			pNodesData = pNodes->GetNodes().GetData();
		}

		if ( iSize >0 && NULL != pNodesData )
		{
			// 绘制第一个的开盘价与收盘价之间的连线
			int32 iXOpen, iYOpen;
			int32 iXClose, iYClose;
			const CNodeData &Node = pNodesData[0];
			if ( !CheckFlag(Node.m_iFlag, CNodeData::KValueInvalid) )
			{
				if ( pCurve->PriceYToRegionY(Node.m_fOpen, iYOpen)
					&& pCurve->PriceYToRegionY(Node.m_fClose, iYClose) )
				{
					iXOpen = m_pRegionMain->GetRectCurves().left+1;
					iXClose = m_pRegionMain->m_aXAxisNodes[0].m_iCenterPixel;

					if (m_bShowTopToolBar)
					{
						iYOpen += m_iTopButtonHeight;
						iYClose += m_iTopButtonHeight;
						if (m_bShowTopMerchBar)
						{
							iYOpen += m_iTopMerchHeight;
							iYClose += m_iTopMerchHeight;
						}
					}
					
					CPen penLine;
					penLine.CreatePen(PS_SOLID, 0, GetIoViewColor(ESCKLineKeep));
					CPen *pPenOld = (CPen *)pDC->SelectObject(&penLine);
					pDC->MoveTo(iXOpen, iYOpen);
					pDC->LineTo(iXClose, iYClose);
					pDC->SelectObject(pPenOld);
				}
			}
		}
	}
}

void CIoViewTrend::RequestVolBuySellData()
{
	T_MerchNodeUserData *pData = m_MerchParamArray.GetSize()>0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == pData || NULL == pData->m_pMerchNode )
	{
		return;
	}

	if ( !IsShowVolBuySellColor() )
	{
		return;
	}

	CMmiReqMinuteBS	reqBS;
	reqBS.m_iMarketId	 = pData->m_pMerchNode->m_MerchInfo.m_iMarketId;
	reqBS.m_StrMerchCode = pData->m_pMerchNode->m_MerchInfo.m_StrMerchCode;
	DoRequestViewData(reqBS);

	if ( IsNowInTradeTimes(pData->m_pMerchNode, true) )
	{
		CMmiReqAddPushMinuteBS reqPush;
		reqPush.m_iMarketId	 = pData->m_pMerchNode->m_MerchInfo.m_iMarketId;
		reqPush.m_StrMerchCode = pData->m_pMerchNode->m_MerchInfo.m_StrMerchCode;
		DoRequestViewData(reqPush);

		SetTimer(KTimerIdTrendPushMinuteBS, KTimerPeriodTrendPushMinuteBS, NULL);
	}
}

void CIoViewTrend::OnMySettingChange()
{
	if ( NULL != m_pMerchXml )
	{
		OnVDataMerchKLineUpdate(m_pMerchXml);
	}

	RequestVolBuySellData();
}

void CIoViewTrend::OnTrendIndexMenu(UINT nId)
{
	m_StrBtnIndexName = m_mapIndexName[nId - IDM_INDEXMENU_BEGIN];
	AddIndexToSubRegion(m_StrBtnIndexName);
}

void CIoViewTrend::OnUpdateTrendIndexMenu(CCmdUI *pCmdUI)
{
	///
}

void CIoViewTrend::OnVDataMerchMinuteBSUpdate( CMerch* pMerch )
{
	if ( NULL == m_pMerchXml || pMerch != m_pMerchXml )
	{
		return;
	}

	// 依赖与指标线数据的更新
	T_MerchNodeUserData *pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL != pData )
	{
		OnVDataMerchKLineUpdate(pMerch);
		ReDrawAysnc();
	}
}

void CIoViewTrend::CreateSuspendedVirtualNoOpenKlineData()
{
	T_MerchNodeUserData *pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == pData )
	{
		return;
	}
	if ( !IsSuspendedMerch() )
	{
		ASSERT( 0 );
		return;
	}

	CreateSuspendedVirtualNoOpenCmpKlineData();

	if ( pData->m_aKLinesCompare.GetSize() <= 0 )
	{
		return;	// 没有数据需要填充
	}

	CKLine *pKLineCmp = (CKLine *)pData->m_aKLinesCompare.GetData();
	int32  iCmpKlineCount = pData->m_aKLinesCompare.GetSize();

#ifdef _DEBUG
	{
		CGmtTime TimeStart, TimeEnd;
		ASSERT( GetTrendStartEndTime(TimeStart, TimeEnd) );
		ASSERT( CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pData->m_aKLinesCompare, TimeStart) == 0 );
		ASSERT( CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(pData->m_aKLinesCompare, TimeEnd) == iCmpKlineCount-1 );
	}
#endif

	// 生成Full数据
	FillTrendNodes(*pData, m_pAbsCenterManager->GetServerTime(), pKLineCmp, iCmpKlineCount);

	// 由于是虚构的数据，所以不需要计算指标数据？
	// 更新主图下面的指标 - 存在虚假的数据问题，未解决
	int32 j = 0;
	for ( j = 0; j < pData->aIndexs.GetSize(); j++)
	{
		T_IndexParam* pIndex = pData->aIndexs.GetAt(j);
		g_formula_compute(this, this, (CChartRegion*)pIndex->pRegion, pData, pIndex, 0, pData->m_aKLinesFull.GetSize());
	}

	// 生成显示数据
	if (!pData->UpdateShowData(0, pData->m_aKLinesFull.GetSize()))
	{
		ASSERT( 0 );
		return;
	}

	// 所有指标都是虚假数据，设置这些数据的flag为invalid
	for ( j = 0; j < pData->aIndexs.GetSize(); j++)
	{
		T_IndexParam* pIndex = pData->aIndexs.GetAt(j);
		for (int32 k = 0; k < pIndex->m_aIndexLines.GetSize(); k++)
		{
			CIndexNodeList &IndexNodeList = pIndex->m_aIndexLines[k];
			if ( NULL == IndexNodeList.m_pNodesShow )
			{
				continue;
			}
			CNodeData *pNodesShow	= ((CNodeSequence*)IndexNodeList.m_pNodesShow)->m_aNodes.GetData();

			for ( int32 iFlagIndex =0; iFlagIndex < ((CNodeSequence*)IndexNodeList.m_pNodesShow)->GetSize() ; iFlagIndex++ )
			{
				// 全部无效
				DWORD dwIndex = pNodesShow[iFlagIndex].m_iFlag;
				pNodesShow[iFlagIndex].m_iFlag = (dwIndex | CNodeData::KValueInvalid);
			}
		}
	}

	// 虚构数据，同样不需要处理分钟vol颜色数据
}

void CIoViewTrend::CreateSuspendedVirtualNoOpenCmpKlineData()
{
	T_MerchNodeUserData *pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == pData || NULL == pData->m_pMerchNode || !pData->m_TrendTradingDayInfo.m_bInit )
	{
		return;
	}

	float fPricePrev = GetTrendPrevClose();
	if ( 0.0f == fPricePrev )
	{
		return;
	}

	CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();

	// 仅填充当天的需要显示的到现在的虚假数据
	// 完整数据
	const CMarketIOCTimeInfo &IOCTime = pData->m_TrendTradingDayInfo.m_MarketIOCTime;
	CGmtTime TimeStart, TimeEnd;
	if ( !GetTrendStartEndTime(TimeStart, TimeEnd) )
	{
		ASSERT( 0 );	// 无效时间段？？	
		return;
	}
	time_t tTimeStart = TimeStart.GetTime();
	time_t tTimeEnd	 = TimeEnd.GetTime();
	CGmtTime TimeTodayStart = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeOpen.m_Time;
	int32 iPos = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pData->m_aKLinesCompare, TimeTodayStart);
	if ( iPos >=0 && iPos < pData->m_aKLinesCompare.GetSize() )
	{
		// 需要填充的数据必然是不存在今天的分钟k线的
		// 这种情况现发现可能是跨天了，没有即时的处理
		//ASSERT( 0 );
		// 只要是存在今天的数据，则不处理
		// 本身在Fill时，就可能存在虚拟的K线，so不提示
		//TRACE(_T("分时图可能存在不需要填充虚拟K线，但是又调用了的问题！\r\n"));
		return;
	}
	pData->m_aKLinesCompare.SetSize(pData->m_aKLinesCompare.GetSize(), 500);

	bool32 bCont = true;
	for (int32 iIndexOC = 0; iIndexOC < IOCTime.m_aOCTimes.GetSize() && bCont; iIndexOC += 2)
	{
		CGmtTime TimeOpen =  IOCTime.m_aOCTimes[iIndexOC];
		CGmtTime TimeClose =  IOCTime.m_aOCTimes[iIndexOC + 1];

		time_t uiTime = TimeOpen.GetTime();
		for (uiTime = TimeOpen.GetTime(); uiTime <= TimeClose.GetTime() && uiTime <= TimeNow.GetTime() && bCont; uiTime += 60)
		{
			if ( uiTime < tTimeStart )
			{
				continue;	// 不在显示区间内的，不填充
			}
			if ( uiTime > tTimeEnd )
			{
				bCont = false;
				break;
			}
			CKLine kline;
			kline.m_fPriceAvg = fPricePrev;
			kline.m_fPriceClose = fPricePrev;
			kline.m_TimeCurrent = CGmtTime(uiTime);
			pData->m_aKLinesCompare.Add(kline);
		}

		if ( uiTime > TimeNow.GetTime() )
		{
			break;	// 超过当前时间，不要继续填充了
		}
	}
}

bool32 CIoViewTrend::IsSuspendedMerch()
{
	T_MerchNodeUserData *pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == pData || NULL == pData->m_pMerchNode )
	{
		ASSERT( 0 );
		return false;
	}

	if ( m_bHistoryTrend )
	{
		return false;	// 历史分时不考虑停牌, 现有日k线则必有同样的分钟k线出现，so不考虑历史
	}

	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	if ( pApp->m_bOffLine )
	{
		return false;	// 离线状态下无停牌概念，就是没数据, shit
	}

	ASSERT( pData->m_TrendTradingDayInfo.m_bInit );
	if ( 0.0f < GetTrendPrevClose()
		&& NULL != pData->m_pMerchNode->m_pRealtimePrice
		&& pData->m_pMerchNode->m_pRealtimePrice->m_fPriceOpen == 0.0f
		&& pData->m_pMerchNode->m_pRealtimePrice->m_fPriceHigh == 0.0f )
	{
		// 有昨收，无开高
		// 如果服务器时间已经过了开盘时间，则认为该商品是停牌商品
		CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();
		CGmtTime TimeOpen = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeOpen.m_Time+CGmtTimeSpan(0,0,0,59);
		if ( TimeNow > TimeOpen )
		{
			return true;	// 超过开盘时间还没有开高数据，则认为它停牌了
		}
	}
	return false;
}

void CIoViewTrend::TrimSuspendedIndexData()
{
	T_MerchNodeUserData *pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == pData || NULL == pData->m_pMerchNode )
	{
		ASSERT( 0 );
		return;
	}

	if ( !IsSuspendedMerch() )
	{
		return;
	}

	CGmtTime TimeTodayStart = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeOpen.m_Time;
	int32 iVirStartPos = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pData->m_aKLinesFull, TimeTodayStart);
	if ( iVirStartPos >=0 && iVirStartPos < pData->m_aKLinesFull.GetSize() )
	{
		ASSERT( pData->m_aKLinesFull.GetSize() == pData->m_iShowPosInFullList+pData->m_iShowCountInFullList );
		for ( int32 j = 0; j < pData->aIndexs.GetSize(); j++)
		{
			T_IndexParam* pIndex = pData->aIndexs.GetAt(j);
			for (int32 k = 0; k < pIndex->m_aIndexLines.GetSize(); k++)
			{
				CIndexNodeList &IndexNodeList = pIndex->m_aIndexLines[k];
				if ( NULL == IndexNodeList.m_pNodesShow )
				{
					continue;
				}
				CNodeData *pNodesShow	= ((CNodeSequence*)IndexNodeList.m_pNodesShow)->m_aNodes.GetData();
				ASSERT( ((CNodeSequence*)IndexNodeList.m_pNodesShow)->GetSize() == pData->m_aKLinesFull.GetSize() );

				for ( int32 iFlagIndex =iVirStartPos; iFlagIndex < ((CNodeSequence*)IndexNodeList.m_pNodesShow)->GetSize() ; iFlagIndex++ )
				{
					// 全部无效
					DWORD dwIndex = pNodesShow[iFlagIndex].m_iFlag;
					pNodesShow[iFlagIndex].m_iFlag = (dwIndex | CNodeData::KValueInvalid);
				}
			}
		}
	}
}

bool32 CIoViewTrend::GetCurrentIOCTime( OUT CMarketIOCTimeInfo &IOCTime )
{
	T_MerchNodeUserData *pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == pData || NULL == pData->m_pMerchNode )
	{
		ASSERT( 0 );
		return false;
	}
	IOCTime = pData->m_TrendTradingDayInfo.m_MarketIOCTime;
	return pData->m_TrendTradingDayInfo.m_bInit;
}

void CIoViewTrend::OnVDataEconoData(const char *pszRecvData)
{
	if (!pszRecvData)
	{
		return;
	}

	// 将新到来的json串的财经日历数据都保存一份（作用：在分时图窗口大小改变时计算要用到）
	m_strEconoData = pszRecvData;

	// 将第一次请求的财经日历数据保存到主窗口类中
	CMainFrame *pMainFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	if (pMainFrame->m_strFirstEconoData.empty())
	{
		pMainFrame->m_strFirstEconoData = pszRecvData;
	}

	// 如果没有保存分时图的时间刻度，不需要解析财经日历数据（就算解析出来了也不会显示的，因为要显示的财经日历数据要依赖分时图的时间刻度）
	if (m_vecXAxisInfo.empty())
	{
		return;
	}

	ParseEonomicData(pszRecvData);
}

void CIoViewTrend::ParseEonomicData(const char *pszData)
{
	if (!pszData)
	{
		return;
	}

	Json::Value vRoot;
	Json::Reader reader;
	if (!reader.parse(pszData, vRoot))
		return;

	int nCode = vRoot["code"].asInt();
	if (0 != nCode)
	{
		return;// 请求失败
	}

	Json::Value vEconomicArray;
	vEconomicArray = vRoot["data"];

	std::vector<T_EconomicData> vecEconomic;
	int size = vEconomicArray.size();
	for (int i = 0 ; i < size ; ++i)
	{
		T_EconomicData stEconomic;
		Json::Value vEconomic = vEconomicArray[i];
		stEconomic.important_level = vEconomic["Importance"].asString();
		stEconomic.forecast_value = vEconomic["PredictValue"].asString();
		stEconomic.nNewID = vEconomic["NewID"].asInt();
		stEconomic.content = vEconomic["Content"].asString();
		stEconomic.former_value = vEconomic["LastValue"].asString();
		stEconomic.country_type = vEconomic["Country"].asString();
		stEconomic.actual_value = vEconomic["PublishValue"].asString();

		std::string strTime = vEconomic["NewTime"].asString();
		int pos = strTime.find(' ');
		strTime = strTime.substr(pos+1, (UINT)(-1));
		stEconomic.release_time = strTime;

		std::string strInfluence = vEconomic["Influence"].asString();
		ParseLDorLK(strInfluence, stEconomic.strLK, stEconomic.strLD, stEconomic.strPlaceholder, stEconomic.bBoth);

		vecEconomic.push_back(stEconomic);
	}

	// 保存分时图上的开始时间（下面for循环用于过滤掉小于该时间的财经日历数据）
	T_XAxisInfo stXAxis;
	if (!m_vecXAxisInfo.empty())
	{
		stXAxis =  *m_vecXAxisInfo.begin();
	}

	std::vector<T_EconomicData>::iterator iter;
	for (iter = vecEconomic.begin() ; iter != vecEconomic.end() ; )
	{
		// 将时间小于分时图中开始时间的财经日历数据过滤掉
		int hour = 0, minute = 0, second = 0;
		sscanf(iter->release_time.c_str(),"%d:%d:%d", &hour, &minute, &second);
		if (stXAxis.hour > hour)
		{
			iter = vecEconomic.erase(iter);
			continue;
		}

		// 去掉重要等级为“低或中”的财经日历数据
		if (iter->important_level.compare("低") == 0)
		{
			iter = vecEconomic.erase(iter);
		}
		else if (iter->important_level.compare("中") == 0)
		{
			iter = vecEconomic.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	if (::IsWindow(m_dlgEconomicTip.m_hWnd))
	{
		if (!vecEconomic.empty())
		{
			std::vector<T_EconomicData>::iterator itBegin, itNext = vecEconomic.begin();

			// 根据时间，合并财经日历数据
			for (itBegin = vecEconomic.begin() ; itBegin != vecEconomic.end() ; )
			{
				std::vector<T_EconomicData> vecMergeEconomic;// 保存合并的数据
				int nSingleCount = 0;// 统计合并的数据中，有多条财经日历数据，只包含力量利多或利空中的一条
				int nBothCount = 0;// 统计合并的数据中，有多条财经日历数据，既包含利多，也包含利空

				int count = 0;// 统计当前循环中，有多少财经日历数据被合并（即：count = nBothCount + nSingleCount）

				vecMergeEconomic.push_back(*itBegin);
				++count;

				if (itBegin->bBoth)
					++nBothCount;
				else
					++nSingleCount;

				int hour1 = 0, minute1 = 0, second = 0;
				sscanf(itBegin->release_time.c_str(),"%d:%d:%d", &hour1, &minute1, &second);


				for (itNext = itBegin + 1 ; itNext != vecEconomic.end() ; ++itNext)
				{
					int hour2 = 0, minute2 = 0;
					sscanf(itNext->release_time.c_str(),"%d:%d:%d", &hour2, &minute2, &second);

					// 如果小时相等，那么就比较分钟数是否超过CONTINUEATION_MINUTES分钟，
					// 将不超过CONTINUEATION_MINUTES分钟的数据合并在一起
					if (hour2 == hour1)
					{
						if (minute2 - minute1 <= CONTINUEATION_MINUTES)
						{
							vecMergeEconomic.push_back(*itNext);
							++count;

							if (itBegin->bBoth)
								++nBothCount;
							else
								++nSingleCount;
						}
						else
						{
							break;
						}
					}
					else if (hour2 - hour1 == 1)
					{
						if (minute2 - minute1 <= CONTINUEATION_MINUTES)
						{
							vecMergeEconomic.push_back(*itNext);
							++count;

							if (itBegin->bBoth)
								++nBothCount;
							else
								++nSingleCount;
						}
						else
						{
							break;
						}
					}
				}

				T_EconoInfo stEconoInfo;
				stEconoInfo.bothCount = nBothCount;
				stEconoInfo.singleCount = nSingleCount;
				stEconoInfo.vecEconomicData = vecMergeEconomic;

				std::vector<T_EconomicData>::iterator itEndEle = vecMergeEconomic.end() - 1;
				std::string strTime = itEndEle->release_time.c_str();//itBegin->release_time.c_str();


				m_dlgEconomicTip.m_mapEconomicData[strTime.c_str()] = stEconoInfo;

				itBegin = itBegin + count;
			}
		}

		// 财经日历数据合并完后，计算小圆点的信息（即x坐标，半径）
		if (!m_dlgEconomicTip.m_mapEconomicData.empty())
		{
			ComputeCircleInfo();
		}
	}
}

void CIoViewTrend::ParseLDorLK(IN std::string &str, OUT std::string &strLK, OUT std::string &strLD, OUT std::string &strPlaceholder, OUT bool &bBoth)
{
	if (str.empty())
	{
		return;
	}

	int pos = str.find(',');
	if (std::string::npos != pos)
	{
		bBoth = true;

		std::string strLeft = str.substr(0, pos);
		std::string strRight = str.substr(pos+1, (UINT)(-1));
		std::string strInfluence = strLeft.substr(0,4);// 取前面两个汉字（要么是利多，要么是利空，以下同理）
		if (strInfluence.compare("利多") == 0)
		{
			strLD = strLeft;
		}
		else if (strInfluence.compare("利空") == 0)
		{
			strLK = strLeft;
		}

		strInfluence = strRight.substr(0, 4);
		if (strInfluence.compare("利多") == 0)
		{
			strLD = strRight;
		}
		else if (strInfluence.compare("利空") == 0)
		{
			strLK = strRight;
		}
	}
	else
	{
		bBoth = false;

		std::string strInfluence = str.substr(0, 4);
		if (strInfluence.compare("利多") == 0)
		{
			strLD = str;
		}
		else if (strInfluence.compare("利空") == 0)
		{
			strLK = str;
		}
		else
		{
			strPlaceholder = str;// 服务器返回了一个无效的字符串（例如：--）
		}
	}
}

void CIoViewTrend::SaveTimeDegreeScale(int xPos, CString strTime)
{
	T_XAxisInfo stCoordInfo;
	stCoordInfo.pt.x = xPos;	// 时间的x坐标
	stCoordInfo.pt.y = 30;		// 小圆点显示时的y坐标

	int pos = strTime.Find(':');
	if(-1 != pos)
	{
		CString strHour = strTime.Left(pos);
		CString strMinutes = strTime.Right(strTime.GetLength() - pos -1);
		stCoordInfo.hour = IsAllNumbers(strHour) ? _wtoi(strHour.GetBuffer()) : 0;
		stCoordInfo.mintues = IsAllNumbers(strMinutes) ? _wtoi(strMinutes.GetBuffer()) : 0;

		m_vecXAxisInfo.push_back(stCoordInfo);
	}
}

void CIoViewTrend::ComputeCircleInfo()
{
	m_CircleInfo.clear();

	// 小圆点的半径
	m_radius = 9;

	if (NULL == m_hWnd)
	{
		return;
	}

	std::map<CString, T_EconoInfo > &mapEconomicData = m_dlgEconomicTip.m_mapEconomicData;
	std::map<CString, T_EconoInfo >::iterator iter;
	for (iter = mapEconomicData.begin() ; iter != mapEconomicData.end() ; ++iter)// 遍历每一个发布时间，以确定该发布时间在x轴的位置
	{
		int hour1 = 0, minute1 = 0, second = 0;
		std::string strTime;
		UnicodeToGbk32(iter->first, strTime);
		sscanf(strTime.c_str(),"%d:%d:%d", &hour1, &minute1, &second);

		// 计算当前财经日历小圆点在x轴的位置
		int posX = -1;
		std::vector<T_XAxisInfo>::iterator it, itNext;
		for (it = m_vecXAxisInfo.begin() ; it != m_vecXAxisInfo.end() ; ++it)
		{
			itNext = it;
			++itNext;

			if (itNext != m_vecXAxisInfo.end())
			{
				// 如果当前财经日历的时间落在该时间段内，就计算该时间段内单位时间长度
				if (hour1 >= it->hour && hour1 < itNext->hour)
				{
					// 当前财经日历时间到it时间，相隔多少小时
					int nHours = hour1 - it->hour;

					// 当前财经日历时间到it时间，相隔多少分钟
					int nMinutes = (int)(nHours * 60 + myFabs(minute1 - it->mintues));

					// 计算itNext时间到it时间，相隔多少分钟
					int nMinutes1 = (int)((itNext->hour - it->hour) * 60 + myFabs(itNext->mintues - it->mintues));

					// 计算itNext时间到it时间，相隔多长
					int nSpacing = itNext->pt.x - it->pt.x;

					if(0 != nMinutes1)
					{
						// 计算1分钟的时间长度
						float unitLen = nSpacing / (nMinutes1*1.0);

						// 财经日历要显示的x坐标（-4的目的：计算有误差，-4就是减少4个长度的误差，备注：只能减小误差，不能消除误差）
						posX = it->pt.x - 4 + (int)(nMinutes * unitLen);
					}
					break;
				}
				else
				{
					continue;
				}
			}
		}

		T_CircleInfo stCircleInfo;

		// 保存小圆点对应的发布时间
		stCircleInfo.strTime = iter->first;

		// 计算每一个发布时间的小圆点的显示区域
		if (it != m_vecXAxisInfo.end())
		{
			stCircleInfo.rt.left = posX;
			stCircleInfo.rt.top = it->pt.y;
			stCircleInfo.rt.right = stCircleInfo.rt.left + m_radius;
			stCircleInfo.rt.bottom = stCircleInfo.rt.top + m_radius;

			m_CircleInfo.push_back(stCircleInfo);
		}
	}
}

void CIoViewTrend::DrawCircleDot(CMemDCEx * pDC)
{
	// 如果是多合约同列界面，就不绘制小圆点
	if (m_bMoreView)
	{
		return;
	}

	if (m_CircleInfo.empty() && !m_vecXAxisInfo.empty())
	{
		CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
		ParseEonomicData(pMainFrame->m_strFirstEconoData.c_str());

		m_strEconoData = pMainFrame->m_strFirstEconoData;
	}

	// 如果是多日分时图，那么不绘制财经日历小圆点
	if (m_iTrendDayCount > 1)
	{
		return;
	}

	int32 iSaveDC = pDC->SaveDC();
	{
		std::vector<T_CircleInfo>::iterator it;
		for (it = m_CircleInfo.begin() ; it != m_CircleInfo.end() ; ++it)
		{
			CBrush br(RGB(85,146,255));
			pDC->SelectObject(&br);
			pDC->Ellipse(it->rt);
		}
	}
	pDC->RestoreDC(iSaveDC);
}

void CIoViewTrend::AddOrDelVolumeRegion(E_ReportType eBefore, E_ReportType eNow)
{
	return;
	/*
	if (m_bFromXml)
	{
	return;
	}

	if (eBefore == eNow)
	{
	return;
	}

	if (ERTMony == eBefore && ERTMony != eNow)
	{
	// 插入成交量副图
	bool32 bNeedAdd = true;

	for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
	{
	CChartRegion* pRegion = m_SubRegions[i].m_pSubRegionMain;

	for (int32 j = 0; j < pRegion->m_Curves.GetSize(); j++)
	{
	CChartCurve* pCurve = pRegion->m_Curves[j];

	if (NULL != pCurve && 0 == pCurve->GetGroupTitle().CompareNoCase(L"VOLFS"))
	{
	bNeedAdd = false;
	break;
	}
	}			
	}		

	if (bNeedAdd)
	{
	// 还要先都tmd 删掉吗? fuck in the hole
	while (m_SubRegions.GetSize() > 0)
	{
	CChartRegion* pRegion = m_SubRegions[0].m_pSubRegionMain;
	if (NULL != pRegion)
	{
	DeleteRegionCurves(pRegion);
	DelSubRegion(pRegion, false);
	}
	}

	//
	AddIndexToSubRegion(L"VOLFS");
	OnTrendBtnSelChanged(false);	
	}
	}

	//
	if (ERTMony != eBefore && ERTMony == eNow)
	{
	// 删除成交量副图
	if (m_SubRegions.GetSize() <= 0)
	{
	return;
	}

	CChartRegion* pRegion = m_SubRegions[0].m_pSubRegionMain;
	if (NULL == pRegion)
	{
	//ASSERT(0);
	return;
	}

	bool32 bNeedDel = false;

	for (int32 i = 0; i < pRegion->m_Curves.GetSize(); i++ )
	{
	CChartCurve* pCurve = pRegion->m_Curves[i];
	if (NULL != pCurve && 0 == pCurve->GetGroupTitle().CompareNoCase(L"VOLFS"))
	{
	bNeedDel = true;
	break;
	}
	}

	//
	if (bNeedDel)
	{
	DeleteRegionCurves(pRegion);
	DelSubRegion(pRegion, false);
	}		

	OnTrendBtnSelChanged(false);	
	}
	*/
}

void CIoViewTrend::SetIndexToRegion(const CString& StrIndexName, CChartRegion* pRegion, bool32 bShowRightDlg)
{
	if (StrIndexName.GetLength() <= 0 || NULL == pRegion)
	{
		return;
	}

	T_MerchNodeUserData* pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == m_pRegionMain || NULL == pData )
	{
		ASSERT( 0 );
		return;
	}

	//	
	if ( !CPluginFuncRight::Instance().IsUserHasIndexRight(StrIndexName, bShowRightDlg) )
	{
		return; // 权限不够
	}

	// 如果当前指标就是现在的指标，则不用操作
	// 不考虑什么大盘指标什么的，无需求，不做这个
	// 检查是否存在该index - 怎么样判断呢？？
	for ( int32 i=0; i < pData->aIndexs.GetSize() ; i++ )
	{
		// 由于实际创建index也是根据name来创建，so简单判断名字+region
		if ( pData->aIndexs[i]->strIndexName == StrIndexName && pRegion == pData->aIndexs[i]->pRegion )
		{
			return;
		}
	}
	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);

	// pSubRegion已被删除，不应该在用
	DeleteRegionCurves(pRegion);

	T_IndexParam* pIndex = AddIndex(pRegion, StrIndexName);
	if ( NULL == pIndex )
	{
		ASSERT( 0 );
		return;
	}				
	int32 iNodeBegin,iNodeEnd;
	GetNodeBeginEnd(iNodeBegin,iNodeEnd);
	g_formula_compute ( this, this, pRegion, pData, pIndex, iNodeBegin,iNodeEnd );

	if ( IsSuspendedMerch() )
	{
		// 停盘数据的指标需要截断虚假数据
		TrimSuspendedIndexData();
	}

	UpdateSelfDrawCurve();
	Invalidate();
}

//获取要显示广告框的位置
bool32 CIoViewTrend::GetAdvDlgPosition(IN int32 iWidth, IN int32 iHeight, OUT CRect& rtPos)
{
	if (iWidth <= 0 || iHeight <= 0)
	{
		//ASSERT(0);
		return false;
	}

	//
	rtPos.SetRectEmpty();

	//
	float fPriceNew = -1;

	if (NULL == m_pMerchXml || NULL == m_pRegionMain || NULL == m_pRegionMain->GetDependentCurve())
	{
		return false;
	}

	// 中线的Y 坐标
	CRect RectMain = m_pRegionMain->GetRectCurves();	
	int32 iYCenter = RectMain.CenterPoint().y;

	// 最新价对应的Y 坐标
	bool bNoPriceNew = false;
	if ( m_pMerchXml->m_pRealtimePrice )
		fPriceNew = m_pMerchXml->m_pRealtimePrice->m_fPriceNew;
	else
	{
		fPriceNew = 0.0;
		bNoPriceNew = true;
	}

	int32 iYPrice;
	CChartCurve* pCurveDependent = m_pRegionMain->GetDependentCurve();



	if ( !pCurveDependent->PriceYToRegionY(fPriceNew, iYPrice) )
	{
		//没有最新价画在最上面
		bNoPriceNew = true;
	}
	else
	{
		m_pRegionMain->RegionYToClient(iYPrice);
	}

	//
	if (iYCenter <= iYPrice || bNoPriceNew)
	{
		// 画上面
		rtPos.top = RectMain.top + RectMain.Height() / 4 - iHeight / 2;		
	}
	else
	{
		// 画下面
		rtPos.top = RectMain.bottom - RectMain.Height() / 4 - iHeight / 2;
	}

	rtPos.bottom = rtPos.top + iHeight;		
	rtPos.right = RectMain.right;
	rtPos.left  = rtPos.right - iWidth;

	ClientToScreen(rtPos);
	return true;
}

//当没有最新价，这些数据时，不用显示广告框
bool32 CIoViewTrend::IsAlreadyToShowAdv()
{
	if ( NULL == m_pMerchXml || NULL == m_pMerchXml->m_pRealtimePrice )//|| m_pMerchXml->m_pRealtimePrice->m_fPriceNew <= 0.0)
		return false;
	//
	return true;
}



void CIoViewTrend::SetAvgPriceEvent()
{
	if (m_bShowAvgPrice)
	{
		m_bShowRiseFallMax = false;
	}
//	else
//	{
	//	m_bShowRiseFallMax = true;
//	}
}

void CIoViewTrend::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
	}
	if (NULL == m_pMerchXml)
	{
		return;
	}

	// 如果不是大宗商品，后面操作都不需要做了
	if ((m_pMerchXml->m_Market.m_MarketInfo.m_eMarketReportType == ERTMony) && !m_bMoreView)
	{
		bool bRectIn = false;
		int size = m_CircleInfo.size();
		for (int i = 0 ; i < size; ++i)
		{
			T_CircleInfo &stCircle = m_CircleInfo.at(i);
			if (stCircle.rt.PtInRect(point))
			{
				bRectIn = true;
				if (!::IsWindowVisible(m_dlgEconomicTip.m_hWnd))
				{
					CPoint pt(stCircle.rt.right, stCircle.rt.bottom);
					ClientToScreen(&pt);

					// 获取分时窗口的大小
					CRect rtTrend;
					GetWindowRect(rtTrend);

					// -45目的：指标显示和时间刻度显示的总高度大概是45，-45就可以让超过的该高度的部分就不用显示了
					rtTrend.bottom -= 45;

					T_EconoInfo &stEconoInfo = m_dlgEconomicTip.m_mapEconomicData[stCircle.strTime];
					int count = stEconoInfo.vecEconomicData.size();
					int nTemp = stEconoInfo.bothCount * CHILD_HEIGHT_V2 + stEconoInfo.singleCount * CHILD_HEIGHT_V1;

					int height = 0;
					if (rtTrend.Height() < nTemp)
					{
						count = rtTrend.Height() / CHILD_HEIGHT_V2;
						height = count * CHILD_HEIGHT_V2;
					}
					else
					{
						height = nTemp;
					}

					if ((rtTrend.right - pt.x) < CHILD_WIDTH)
					{
						pt.x = pt.x - CHILD_WIDTH;
					}

					m_dlgEconomicTip.MoveWindow(pt.x, pt.y, CHILD_WIDTH, height);

					m_dlgEconomicTip.InitChildWindow(count, stEconoInfo.vecEconomicData);// 初始化提示框的子窗口
					m_dlgEconomicTip.ShowWindow(SW_SHOW);
				}
			}
		}

		if (!bRectIn)
		{
			// 鼠标移出小圆点，释放提示框中的子窗口
			m_dlgEconomicTip.ReleaseChildWindow();

			// 隐藏提示框
			::ShowWindow(m_dlgEconomicTip.m_hWnd, SW_HIDE);
		}
	}

	int iButton = TBottomBtnHitTest(point);
	if (iButton != m_iBtnHovering)
	{
		if (INVALID_ID != m_iBtnHovering)
		{
			m_aTrendBtnCur[m_iBtnHovering].m_btnPolygon.MouseLeave();
			m_iBtnHovering = INVALID_ID;
		}

		if (INVALID_ID != iButton)
		{	
			m_iBtnHovering = iButton;
			m_aTrendBtnCur[m_iBtnHovering].m_btnPolygon.MouseHover();
		}
	}

	__super::OnMouseMove(nFlags, point);
}

void CIoViewTrend::OnMouseHover(UINT nFlags, CPoint point)
{
	m_bOver = TRUE;

	//CIoViewTrend::OnMouseHover(nFlags, point);
}

void CIoViewTrend::OnMouseLeave()
{
	if (m_dlgEconomicTip.m_hWnd)
		::ShowWindow(m_dlgEconomicTip.m_hWnd, SW_HIDE);

	m_bOver = FALSE;
	m_bTracking = FALSE;

	if (INVALID_ID != m_iBtnHovering)
	{
		m_aTrendBtnCur[m_iBtnHovering].m_btnPolygon.MouseLeave();
		m_iBtnHovering = INVALID_ID;
	}

	//CIoViewTrend::OnMouseLeave();
}

void CIoViewTrend::StartReqEconoData()
{
	// 设置财经日历服务器的URL地址
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if (pDoc && pDoc->m_pAutherManager)
	{
		std::string strURL;

		// 读取财经日历服务器地址
		pDoc->GetEconoServerURL(strURL);

		CString StrUrl = strURL.c_str();

		// 开始请求当天的财经日历数据
		SYSTEMTIME sys; 
		GetLocalTime( &sys );

		char szTime[32] = {0};
		sprintf(szTime, "%d-%d-%d", sys.wYear, sys.wMonth, sys.wDay);

		// 请求财经日历数据
		pDoc->m_pAutherManager->GetInterface()->ReqEconoData(StrUrl, szTime);

		SetTimer(KTimerIdTrendEconoData, KTimerPeriodTrendEconoData, NULL);
	}
}

double CIoViewTrend::myFabs( double dbNum )
{
	return (dbNum > 0) ? dbNum : (-dbNum);
}

//////////////////////////////////////////////////////////////////////////
//
CString	TrendTypeToCString(E_TrendBtnType eType)
{
	static CStringArray s_aStrs;
	if ( s_aStrs.GetSize() <= 0 )
	{
		s_aStrs.Add(_T("成交量"));
		s_aStrs.Add(_T("成交额"));
		s_aStrs.Add(_T("指标"));
		s_aStrs.Add(_T("量比"));
		s_aStrs.Add(_T("风警线"));
		s_aStrs.Add(_T("买卖力道"));
		s_aStrs.Add(_T("竞价图"));
		s_aStrs.Add(_T("多空指标"));
		s_aStrs.Add(_T("涨跌率"));
		s_aStrs.Add(_T("价格模型"));
	}

	if ( eType >= 0 && eType < s_aStrs.GetSize() )
	{
		return s_aStrs[eType];
	}
	return CString(_T(""));
}

T_TrendBtn::T_TrendBtn()
{
	m_eTrendBtnType = ETBT_Count;
	m_eMerchType = ERTCount;
	m_iID = 0;
	m_btnPolygon.CreateButton(L"", CRect(0,0,0,0), NULL);
}

T_TrendBtn::T_TrendBtn( E_TrendBtnType eBtnType, E_ReportType eMerchType, CWnd *pParent)
{
	m_eTrendBtnType = eBtnType;
	m_eMerchType = eMerchType;
	m_iID = BTN_ID_BEGIN+eBtnType;

	CString StrName = TrendTypeToCString(eBtnType);
	m_btnPolygon.CreateButton(StrName, CRect(0,0,0,0), pParent, m_iID);

	Color nomarlClr, hotClr, pressClr;
	nomarlClr.SetFromCOLORREF(RGB(50, 42,42));
	hotClr.SetFromCOLORREF(RGB(50, 42,42));
	pressClr.SetFromCOLORREF(RGB(50, 42,42));
	Color nomarlTextClr, hotTextClr, presTextsClr;
	nomarlTextClr.SetFromCOLORREF(RGB(179,178,178));
	hotTextClr.SetFromCOLORREF(RGB(255,255,255));
	presTextsClr.SetFromCOLORREF(RGB(255,255,255));
	m_btnPolygon.SetTextColor(RGB(230,230,230), RGB(230,70,70), RGB(230,70,70));
	m_btnPolygon.SetTextBkgColor(nomarlClr, hotClr, pressClr);
	m_btnPolygon.SetTextFrameColor(nomarlClr, nomarlClr, nomarlClr);

	CPoint point(0, 2);
	m_btnPolygon.SetTextOffPos(point);
	

	CPolygonButton::T_NcFont m_Font;
	m_Font.m_StrName = gFontFactory.GetExistFontName(L"宋体");
	m_Font.m_Size	 = 9.7;
	m_Font.m_iStyle	 = FontStyleRegular;	
	m_btnPolygon.SetFont(m_Font);
}

void T_TrendBtn::SetParent(CWnd *pParent)
{
	m_btnPolygon.SetParentWnd(pParent);
}

//--- wangyongxue 显示指标菜单
void CIoViewTrend::ShowIndexMenu()
{
	CNewMenu IndexMenu;
	IndexMenu.CreatePopupMenu();
	int32 iSize = m_ToolBarIndexName.GetSize();
	CString StrIndexName;
	m_mapIndexName.clear();
	for(int32 i=0; i<iSize; i++)
	{
		if(L"VOL" != m_ToolBarIndexName.GetAt(i))
		{
			IndexMenu.AppendODMenu(m_ToolBarIndexName.GetAt(i), MF_STRING, IDM_INDEXMENU_BEGIN + i);
		}
		m_mapIndexName[i] = m_ToolBarIndexName.GetAt(i);

	}
	int32 iCmdIndex = 0;

	for(iCmdIndex=0; iCmdIndex<m_mapIndexName.size(); iCmdIndex++)
	{
		if (m_StrBtnIndexName == m_mapIndexName[iCmdIndex])
		{
			IndexMenu.CheckMenuItem(iCmdIndex+IDM_INDEXMENU_BEGIN, MF_BYCOMMAND|MF_CHECKED );
			break;
		}
	}

	CRect rectIndex(0, 0, 0, 0);
	for(int32 i=0; i<m_aTrendBtnCur.GetSize(); i++)
	{
		if(ETBT_Index == m_aTrendBtnCur.GetAt(i).m_eTrendBtnType)
		{
			m_aTrendBtnCur.GetAt(i).m_btnPolygon.GetRect(rectIndex);
			break;
		}
	}
	ClientToScreen(&rectIndex);
	IndexMenu.TrackPopupMenu(TPM_BOTTOMALIGN | TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL, rectIndex.left, rectIndex.top, this);

	DestroyMenu(IndexMenu);
}

int CIoViewTrend::TBottomBtnHitTest(CPoint point)
{
	map<int, CPolygonButton>::iterator iter;

	int32 iSize = m_aTrendBtnCur.GetSize();
	for (int32 i=0; i<iSize; i++)
	{
		CPolygonButton &btnControl = m_aTrendBtnCur[i].m_btnPolygon;

		// 点point是否在已绘制的按钮区域内
		if (btnControl.PtInButton(point))
		{
			return i;
			//return btnControl.GetControlId();
		}
	}

	return INVALID_ID;
}

int32 CIoViewTrend::FindIndexByBtnID(int32 iID)
{
	int32 iIndex = 0;
	int32 iSize = m_aTrendBtnCur.GetSize();
	for (int32 i=0; i<iSize; i++)
	{
		CPolygonButton &btnControl = m_aTrendBtnCur[i].m_btnPolygon;
		if (btnControl.GetControlId() == iID)
		{
			iIndex = i;
			break;
		}
	}

	return iIndex;
}

bool CIoViewTrend::IsCompetePriceShow()
{
	bool bShow = false;
	//	查询是否存在竞价菜单
	int iBtnIndex = -1;
	for (unsigned i =0; i < m_aTrendBtnCur.GetSize(); ++i)
	{		
		if (ETBT_CompetePrice + BTN_ID_BEGIN == m_aTrendBtnCur[i].m_iID)
		{
			iBtnIndex = i;
			break;
		}
	}

	//	如果已经选中，则清除竞价
	if (m_iBtnCur == iBtnIndex)
	{
		bShow = true;
	}

	return bShow;
}
