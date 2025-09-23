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

#define TREND_MIN_DAY_COUNT   (1)					// ��С������
#define TREND_MAX_DAY_COUNT	  (20)					// ��������

#define TREND_BOTTOM_HEIGHT (25)					// �ײ���ť����߶�
#define TREND_BTN_HEIGHT	(19)					// �ײ���ť�߶�

#define INVALID_ID -1
#define BTN_ID_BEGIN			3000				// �ײ���ťID��ʼֵ

const int32 iIndexShift = 0;						// ָ�갴ť, ƽ���ı���ƫ����
const int32 KiIndexSpace				= 2;	//	ָ���ڿؼ������еļ��

extern const int32 KiMenuIDIntervalStatistic;

#define IDM_CHART_INDEX_BEGIN   (33388)
#define IDM_CHART_INDEX_END     (33888)

enum E_TrendSpecialCurve
{
	ETSC_Start = 0,				// ��ʼλ��
	ETSC_DuoKongCurve,			// ��� һ����
	ETSC_RiseFall_RiseCurve,	// �ǵ��� ����
	ETSC_RiseFall_FallCurve,	// �ǵ��� ����
	ETSC_RiseFall_DiffCurve,	// �ǵ��� ��ֵ��
	ETSC_VolRate,				// ���� һ����
	ETSC_BuySell_BuyCurve,		// �������� ����
	ETSC_BuySell_SellCurve,		// �������� ����
	ETSC_BuySell_DiffCurve,		// �������� ��ֵ��

	ETSC_Count
};


///////////////////////////////////////////////////////
// CIoViewTrend

const int32 KTimerIdTrendInit		= 10023;		// ��ʱͼ��ʼ��timer��FromXmlʱ�������һЩ��ʼ���������������
const int32 KTimerPeriodTrendInit	= 20;			// n ms��ʼ

const int32 KTimerIdTrendReqPlusPushData = 10024;
const int32 KTimerPeriodTrendReqPlusPushData = 60*2*1000;	// ÿ������ ms����һ�η�ʱ�������������

const int32 KTimerIdTrendReqInfoMine = 10025;	// ÿ������ ms������Ϣ��������
const int32 KTimerPeriodTrendReqInfoMine = 60*1000*10;	

const int32 KTimerIdTrendPushMinuteBS = 10026;			// ����ÿ������������Ϣ��������
const int32 KTimerPeriodTrendPushMinuteBS = 60*1000*3;	//

const int32 KTimerIdTrendEconoData = 10027;				// ����ƾ�����
const int32 KTimerPeriodTrendEconoData = 30*1000;		// 30s����һ��

const CString KStrTrendFS = _T(" ��ʱ����");
const CString KStrTrendJun= _T("����");
const CString KStrTrendVol= _T("�ɽ���");
const CString KStrTrendHold = _T("�ֲ���");

const LPCWSTR  KStrXMLShowRiseFallMax = L"ShowRiseFallMax";
const LPCWSTR  KStrXMLShowAvgPrice = L"ShowAvgPrice";

//////////////////////////////////////////////////////////////////////////

// xml
const char KStrXMLEleTrendMerchType[]			= "TrendMerchType";				// ��Ʒ����Ԫ��
const char KStrXMLAttriTrendMerchType[]			= "type";						// ����
const char KStrXMLAttriTrendBtnIndex[]			= "BtnIndex";					// ��ť
const char KStrXMLAttriTrendIndexName[]			= "IndexName";					// ָ������
const char KStrXMLEleTrendRegions[]				= "TrendRegions";				// ��ʱregionԪ��
const char KStrXMLEleTrendRegion[]				= "TrendRegion";				// ��ʱregionԪ��

///////////////////////////////////////////////////////////////////////////////
const T_FuctionButton s_astTrendSubRegionButtons[] =
{
	T_FuctionButton(L"+",		L"���Ӹ�ͼ",		REGIONTITLEADDBUTTONID,		IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE),
	T_FuctionButton(L"��",		L"ɾ����ǰ��ͼ",	REGIONTITLEDELBUTTONID,		IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE),
};

// ���ܰ�ť����
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

	m_eMainCurveAxisYType = CPriceToAxisYObject::EAYT_Pecent;	// ��ʱͼ��֧����һ��

	m_bShowRiseFallMax = true;
	m_bRiseFallAxis = false;

	m_bShowAvgPrice = false;

	m_bEnableRightYRegion = true;	// ��ʾ��Y��

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

	// �Ѿ���region��ɾ����
	//delete m_pRedGreenCurve;

	//m_pEconoHandle->Release();

	delete m_pCompetePriceNodes;	// ��������ɾ��
}
///////////////////////////////////////////////////////////////////////////////
int CIoViewTrend::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CIoViewChart::OnCreate(lpCreateStruct) == -1)
		return -1;

	// ��ʼ��һ����ʾ��ͣ�Ĳ���
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

	InitBtnIndex();	// ��һ�γ�ʼ����ť��ָ��, ����xml��ȡ

	// ���ͬ�в���Ҫ��������Ķ���
	if (m_bMoreView)
	{
		return 0;
	}

	// �Ƿ���Ҫ����ƾ�����
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if (pDoc && pDoc->IsReqEcono())
	{
		m_dlgEconomicTip.Create(CDlgEconomicTip::IDD, this);

		// �ж��Ƿ��Ѿ������ƾ���������
		CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
		if (pMainFrame)
		{
			if (!pMainFrame->m_bStartEconoReq)
			{
				// ����ƾ���������
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
	pCell->SetText(_T(" �۸�"));

	pCell = (CGridCellSys *)pGridCtrl->GetCell(4, 0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(_T(" ����"));

	pCell = (CGridCellSys *)pGridCtrl->GetCell(6, 0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(_T(" �ǵ�"));

	pCell = (CGridCellSys *)pGridCtrl->GetCell(8, 0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(_T(" �Ƿ�"));

	pCell = (CGridCellSys *)pGridCtrl->GetCell(10, 0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(_T("�ɽ���"));

	pCell = (CGridCellSys *)pGridCtrl->GetCell(12, 0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(_T(" ���"));
	m_pCell1 = pCell;
	if ( NULL != m_pMerchXml )
	{
		if (CReportScheme::IsFuture(m_pMerchXml->m_Market.m_MarketInfo.m_eMarketReportType))
		{
			m_pCell1->SetText(_T(" �ֲ���"));
		}
		else
		{
			m_pCell1->SetText(_T(" ���"));
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
		//�Ƿ���OpenCloseTime֮��
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
			if ( !RequestTrendPlusPushData() )	// ÿ������ʱ��������������
			{
				KillTimer(nIDEvent); // ���������ݾͿ��Բ�Ҫ��ʱ��
			}
		}
		else
		{
			// no Kill����Ϊ���ŵ��ž͵��������ݵ�ʱ����
		}
	}
	if ( KTimerIdTrendReqInfoMine == nIDEvent && !m_bHistoryTrend )
	{
		// ÿ������ʱ��Σ�������Ϣ����
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
				reqMine.m_iTimeType = 2;	// ������ǰ200��
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

			// ��ȡ�ƾ�������������ַ
			pDoc->GetEconoServerURL(strURL);

			CString StrUrl = strURL.c_str();

			// ��ʼ������Ĳƾ���������
			SYSTEMTIME sys; 
			GetLocalTime( &sys );

			char szTime[32] = {0};
			sprintf(szTime, "%d-%d-%d", sys.wYear, sys.wMonth, sys.wDay);

			// ����ƾ���������
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

	//X��
	new CChartRegion ( this,m_pRegionXBottom, _T("x��"), CRegion::KFixHeight|CChartRegion::KXAxis);
	m_pRegionXBottom->AddDrawNotify(this,ERDNBeforeDrawCurve);
	m_pRegionXBottom->SetDataSourceRegion(m_pRegionMain);

	//����������
	new CChartRegion ( this,m_pRegionSeparator, _T("split"), CChartRegion::KUserChart);
	m_pRegionSeparator->AddDrawNotify(this,ERDNAfterDrawCurve);

	//Y��
	new CChartRegion ( this,m_pRegionYLeft, _T("��y��"),CRegion::KFixWidth|CChartRegion::KYAxis|CChartRegion::KActiveFlag|CChartRegion::KYAxisSpecifyTextColor);
	m_pRegionYLeft->SetTopSkip(18);
	m_pRegionYLeft->SetBottomSkip(0);
	m_pRegionMain->AddYDependentRegion(m_pRegionYLeft);
	m_pRegionYLeft->AddDrawNotify(this,ERDNBeforeDrawCurve);
	m_pRegionYLeft->SetDataSourceRegion(m_pRegionMain);

	//Y��
	new CChartRegion ( this,m_pRegionYRight, _T("��y��"),CRegion::KFixWidth|CChartRegion::KYAxis|CChartRegion::KYAxisSpecifyTextColor);
	m_pRegionYRight->SetTopSkip(18);
	m_pRegionYRight->SetBottomSkip(0);
	m_pRegionMain->AddYDependentRegion(m_pRegionYRight);
	m_pRegionYRight->AddDrawNotify(this,ERDNBeforeDrawCurve);
	m_pRegionYRight->SetDataSourceRegion(m_pRegionMain);

	new CChartRegion ( this,m_pRegionLeftBottom, _T("���½�"),CChartRegion::KUserChart|CRegion::KFixHeight|CRegion::KFixWidth);
	m_pRegionLeftBottom->AddDrawNotify(this,ERDNBeforeDrawCurve);
	m_pRegionLeftBottom->AddDrawNotify(this,ERDNAfterDrawCurve);

	new CChartRegion ( this,m_pRegionRightBottom, _T("���½�"),CChartRegion::KUserChart|CRegion::KFixHeight|CRegion::KFixWidth);
	m_pRegionRightBottom->AddDrawNotify(this,ERDNBeforeDrawCurve);
	m_pRegionRightBottom->AddDrawNotify(this,ERDNAfterDrawCurve);
}

void CIoViewTrend::SplitRegion()
{
	if ( ParentIsIoView() )
	{
		m_bEnableRightYRegion = false;	// ����ͼ����½�ֹ��Y��
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

	// ������regionɾ��
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

		//�ȷ�RealtimePrice����
		{
			CMmiReqRealtimePrice Req;
			Req.m_iMarketId			= pData->m_pMerchNode->m_MerchInfo.m_iMarketId;
			Req.m_StrMerchCode		= pData->m_pMerchNode->m_MerchInfo.m_StrMerchCode;			
			DoRequestViewData(Req);
		}

		// ����ָ��ʱ�����K�ߣ� �Ա��ȡ��ǰ��ʾ����ͼ�����ռۣ� �񿪼�
		{
			ASSERT(pData->m_TrendTradingDayInfo.m_bInit);

			// 
			// ������ͽ������k������
			if ( m_iTrendDayCount <= TREND_MIN_DAY_COUNT-1 )
			{
				ASSERT( 0 );
				m_iTrendDayCount = TREND_MIN_DAY_COUNT;
			}
			ASSERT( m_iTrendDayCount <= TREND_MAX_DAY_COUNT );

			// ����
			CGmtTime TimeDay = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeClose.m_Time;
			SaveDay(TimeDay);

			CMmiReqMerchKLine info;
			info.m_eKLineTypeBase	= EKTBDay;
			info.m_iMarketId		= pData->m_pMerchNode->m_MerchInfo.m_iMarketId;
			info.m_StrMerchCode		= pData->m_pMerchNode->m_MerchInfo.m_StrMerchCode;
			info.m_eReqTimeType		= ERTYFrontCount;
			info.m_TimeStart		= TimeDay;
			info.m_iFrontCount			= 1 + m_iTrendDayCount;			// ����n+1 !!�����Ӱ�쵽��ʷ��ʱ��
			DoRequestViewData(info);
		}

		// ���ͷ���������
		// ����Ƕ��շ�ʱ����Ҫ�ȵ���k�߻�������ȷ��Ҫ�������ʲôʱ��εķ���k��
		{
			RequestMinuteKLineData(*pData);	// �������k��
		}

		// ������Ϣ����
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
			reqMine.m_iTimeType = 2;	// ������ǰ200��
			reqMine.m_iIndexInfo = EITLandmine;

			m_pAbsCenterManager->RequestNews(&reqMine);
		}
	}

	// �����������
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
		// zhangbo 20090626 #������
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
	m_mapLandMine.clear();		// �������

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

		// ��ȡ����Ʒ������ص�����
		CMarketIOCTimeInfo RecentTradingDay;
		if ( !m_bHistoryTrend )
		{
			if (pMerch->m_Market.GetRecentTradingDay(m_pAbsCenterManager->GetServerTime(), RecentTradingDay, pMerch->m_MerchInfo))
			{
				pData->m_TrendTradingDayInfo.Set(pMerch, RecentTradingDay);
				pData->m_TrendTradingDayInfo.RecalcPrice(*pMerch);
				pData->m_TrendTradingDayInfo.RecalcHold(*pMerch);

				// zhangbo 20090708 #�����䣬 �������
				//...
			}
		}
		else
		{
			// ��ʷ��ʱ - �趨��������Ϣ(��ʷ��ʱ�ϸ������ϲ������л���Ʒ����Ϊÿ����Ʒ�Ľ�������Ϣ������ͬ)
			if ( pMerch->m_Market.GetSpecialTradingDayTime(m_TimeHistory, RecentTradingDay, pMerch->m_MerchInfo) )
			{
				pData->m_TrendTradingDayInfo.Set(pMerch, RecentTradingDay);
				pData->m_TrendTradingDayInfo.RecalcPrice(*pMerch);
				pData->m_TrendTradingDayInfo.RecalcHold(*pMerch);
			}
		}

		// ������Ʒ��־Ϊ��Ҫnews
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

		m_aTrendMultiDayIOCTimes.RemoveAll();	// ���ʱ�����Ϣ

		if ( pData->m_TrendTradingDayInfo.m_bInit )
		{
			m_aTrendMultiDayIOCTimes.Add(pData->m_TrendTradingDayInfo.m_MarketIOCTime);		// �����һ��ĳ�ʼ��ʱ��
		}

		if ( NULL != m_pCell1 )
		{
			if (CReportScheme::IsFuture(pMerch->m_Market.m_MarketInfo.m_eMarketReportType))
			{
				m_pCell1->SetText(_T(" �ֲ���"));
			}
			else
			{
				m_pCell1->SetText(_T(" ���"));
			}
		}


		// 
		if ( !bInhert )
		{
			CNodeSequence* pNodes = CreateNodes();
			pNodes->SetUserData(pData);
			pData->m_pKLinesShow = pNodes;

			CChartCurve* pCurvePrice = m_pRegionMain->CreateCurve(CChartCurve::KDependentCurve|CChartCurve::KRequestCurve|CChartCurve::KTypeTrend|CChartCurve::KYTransformByClose|CChartCurve::KDonotPick);
			pCurvePrice->SetTitle(KStrTrendFS);//��ʱ����
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

		// �����޷�ȷ�еĿ϶�ÿ����Ʒ�Ľ�������Ϣ�����ܾ������������������ǿ����գ�ͬʱҲ����һЩ����
		//SetTrendMultiDay(1, true, false);

		// ���ڲ���������Ʒ���ʺϰ볡������������Ҫ�ж�

		// ��ʼ������Ʒ�İ�ť��Ϣ
		InitMerchBtns();	
		
		InitNoShowBtns();
		

		RecalcBtnRect(true);

		if ( CheckFlag(m_uiFlag,CIoViewChart::KRegionSplited) )
		{
			// û�зָ�Ļ���������Ҳû��
			// ��һ�ε��ý�����Timer��xml��ɺ����
			OnTrendBtnSelChanged(false);	// ���ݲ�Ҫ�����
		}

		UpdateTrendMultiDayChange();	// ���ı���

		ShowProperTrendPlusCurve();	// ��ʾ�������ط�ʱ��������

		//////////////////////////////////////////////////////////////////////////
		// merchklineupdate	
		OnVDataMerchKLineUpdate(pMerch);

		m_mapLandMine = pMerch->m_mapLandMine;	// ��ֵ��ʼ����Ϣ����

		//////////////////////////////////////////////////////////////////////////
		KillTimer(KTrendHeartBeatTimerId);
		SetTimer(KTrendHeartBeatTimerId, 1000*60, NULL);

		SetTimer(KTimerIdTrendReqPlusPushData, KTimerPeriodTrendReqPlusPushData, NULL);	// ��ʼ���͵Ķ�ʱ��
		if ( !m_bHistoryTrend )
		{
			SetTimer(KTimerIdTrendReqInfoMine, KTimerPeriodTrendReqInfoMine, NULL);	// ��ʼ��Ϣ���׶�ʱ����
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
		// zhangbo 20090824 #�����䣬 ������Ʒ������ʱ����յ�ǰ��ʾ����
		//...
	}

	//2014-03-10 �ֻ�����Ʒ�� ��ʾ���Ի��� by cym
	CGGTongApp *ggApp = (CGGTongApp *)AfxGetApp();

	if ( ggApp->m_pConfigInfo->m_bIsVip )
	{
		//���ù�������view
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

		//��Ʒ�л�ʱ�������Ͼ�Ҫ��������
		if ( m_bOwnAdvertise )
		{
			//	if ( m_bMatchMerCode )
			//	{
			RedrawAdvertise();  // 2014-04-01 by cym �����Ǻ��پ����Ƿ�Ҫ����
			/*
			}
			else
			{
			HideAdvDlg(); //���ع��Ի���
			}
			*/
		}
	}

}

// ������ͼ���
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
		// ��ʾ�����¼� TODO
		float fPreClose = pData->m_TrendTradingDayInfo.GetPrevReferPrice();
		if ( 0.0f == fPreClose || NULL == pMerch->m_pRealtimePrice )
		{
			StrTitle = _T("[") + pMerch->m_MerchInfo.m_StrMerchCnName + _T(" ����] ");
			pData->m_pKLinesShow->SetName(StrTitle);
		}
		else
		{
			float fPrice = pMerch->m_pRealtimePrice->m_fPriceNew;
			CString StrSign;
			if ( fPrice < fPreClose ) // ?
			{
				StrSign = _T("��");
			}
			else if (fPrice > fPreClose)
			{
				StrSign = _T("��");
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

	// zhangbo 20090707 #�����䣬 ���¼��㽻���գ� �п����Ǹ����ʼ���� ��Ҫ���»�����ʾ��
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
			TimeServer = m_TimeHistory;		// ��ʷʱ��
		}

		if (TimeServer < TimeInit || TimeServer > TimeEnd)	// ����ͬһ�죬 ��Ҫ������ʱ������ռ۵�
		{
			// �����������
			ClearLocalData();

			// ��ȡ����Ʒ������ص�����
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
			m_aTrendMultiDayIOCTimes.RemoveAll();	// ���ʱ�����Ϣ
			if ( pData->m_TrendTradingDayInfo.m_bInit )
			{
				m_aTrendMultiDayIOCTimes.Add(pData->m_TrendTradingDayInfo.m_MarketIOCTime);		// �����һ��ĳ�ʼ��ʱ��
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

	// ����ǵ�ǰ��ťָ�꣬���滻��ťָ��
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

	// 1:��λ��	
	m_pRegionMain->m_iNodeCross = 0;

	if ( !m_pRegionMain->IsActiveCross() )
	{
		// �����ұ߿�ʼ
		m_pRegionMain->ActiveCross(true);
		for(int32 i = 0 ; i < m_SubRegions.GetSize(); i++)
		{
			m_SubRegions.GetAt(i).m_pSubRegionMain->ActiveCross(true);
		}
	}		

	// 2:������
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
		// �����ұ߿�ʼ
		m_pRegionMain->ActiveCross(true);
		for(int32 i = 0 ; i < m_SubRegions.GetSize(); i++)
		{
			m_SubRegions.GetAt(i).m_pSubRegionMain->ActiveCross(true);
		}
	}		

	// 2:������
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
	// ���ƶ�

	// 1:��λ��
	int32 iNodeCross = m_pRegionMain->m_iNodeCross;

	if ( !m_pRegionMain->IsActiveCross() )
	{
		// �����ұ߿�ʼ
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
			// �Ѿ����������,�������ұߵķǷ�����. �����ұ�ȥ
			iNodeCross = m_pRegionMain->GetDependentCurve()->GetNodes()->GetNodes().GetSize() - 1;		
		}
		else
		{
			iNodeCross -= 1;
		}			 
	}

	m_pRegionMain->m_iNodeCross = iNodeCross;

	// 2:������
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
	// ���ƶ�

	// 1:��λ��
	int32 iNodeCross = m_pRegionMain->m_iNodeCross;

	if ( !m_pRegionMain->IsActiveCross() )
	{
		// ������߿�ʼ
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
			// �Ѿ������ұ���,�������ȥ
			iNodeCross = 0;		
		}
		else
		{
			iNodeCross += 1;
		}			 
	}

	m_pRegionMain->m_iNodeCross = iNodeCross;

	// 2:������
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
		// ��������ͳ�ƶԻ���
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

		// �����
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

		// ׼������һ�ε�ͳ������
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
	//	// 0002149 xl 0705 ����ڼ���󣬶�ʱ���ڱ�����GGTongView��F7�������Ϊ���أ��ᵼ�»�ԭʱ��������δ���¶�������ݵ���ʾ������ȡ�����Ż�
	//	AddFlag(m_uiFlag,CIoViewChart::KMerchKLineUpdate);
	//	ClearLocalData();
	// 	return;
	//}

	// ������Ʒ������������
	for (int32 i = 0; i < m_MerchParamArray.GetSize(); i ++)
	{
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(i);

		if (pData->m_pMerchNode != pMerch)
			continue;

		SetCurveTitle(pData);

		if (0 == i)	// ����Ʒ
		{
			if (UpdateMainMerchKLine(*pData))
			{
				ReDrawAysnc();	
			}
			// ����һ�鸽������
			CalcTrendPlusSubRegionData();
		}
		else		// ������Ʒ
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
	//	// 0002149 xl 0705 ����ڼ���󣬶�ʱ���ڱ�����GGTongView��F7�������Ϊ���أ��ᵼ�»�ԭʱ��������δ���¶�������ݵ���ʾ������ȡ�����Ż�
	//	return;
	//}

	// ������Ʒ������������
	for (int32 i = 0; i < m_MerchParamArray.GetSize(); i ++)
	{
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(i);
		if (pData->m_pMerchNode != pMerch)
			continue;

		// ���������Ʒ�������Ƿ�����
		//if ( pData->bMainMerch )
		//{
		//}

		// ���Ƿ���Ҫ�������ռ�
		{
			// 
			float fPricePrevClose = pData->m_TrendTradingDayInfo.GetPrevReferPrice();
			pData->m_TrendTradingDayInfo.RecalcPrice(*pMerch);
			if (fPricePrevClose != pData->m_TrendTradingDayInfo.GetPrevReferPrice())
			{
				// zhangbo 20090708 #������, ��������ֵ
				ReDrawAysnc();
			}
		}

		// ����Ǿ���ͼ�����ھ���ʱ�䣬����Ʒʵʱ����仯�������󾺼�����
		E_TrendBtnType eBtn = GetCurTrendBtnType();
		if ( pData->bMainMerch
			&& NULL != pMerch->m_pRealtimePrice
			&& ETBT_CompetePrice == eBtn
			&& IsTimeInCallAuction(pMerch->m_pRealtimePrice->m_TimeCurrent.m_Time))
		{
			// ����ʱ��� ���󾺼�����
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
			// ͣ����Ʒ���������KlineUpdate
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
	//pData->pMerch�Ѿ��������Curveʹ����,���û������Curveʹ��,�����������,���Ҵ�AttendMerch��ɾ��.
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

// ��ʱ��ͼ���Ҽ��˵� zhangtao
void CIoViewTrend::OnRegionMenu ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes,CNodeData* pNodeData, int32 x, int32 y)
{
	bool bMulitTimeSection = false;// ��¼�Ƿ��ж������ʱ��

	if ( m_bHistoryTrend )
	{
		return;	// ��ʷ���˵�
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

	// �ж��Ƿ��Ƕ�ʱ�ν�����Ʒ
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
	//���ѡ��������������
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
		&& NULL != FindIndexParamByCurve(pCurveTmp) )	// �޸�chartҪ�õ���pick curve����
	{
		// ����Ҫ��һ��ָ��
		// ����ڷǳɽ�����ͼ���� && ָ�갴ť
		// ���� ָ�갴ť�˵�������
		m_pCurvePick = pCurveTmp;	// �޸�pick����

		CNewMenu menu;
		menu.CreatePopupMenu();
		menu.LoadToolBar(g_awToolBarIconIDs);

		// ����ָ�� �÷�ע�� ����ָ����� �޸�ָ�깫ʽ ����ָ��
		CNewMenu *pAllIndexMenu = menu.AppendODPopupMenu(_T("���з�ʱָ��"));
		if ( !LoadAllIndex(pAllIndexMenu)
			|| pAllIndexMenu->GetMenuItemCount() <= 0 )
		{
			menu.DeleteMenu((UINT)pAllIndexMenu->GetSafeHmenu(), MF_BYCOMMAND);
			pAllIndexMenu = NULL;
		}

		menu.AppendMenu(MF_STRING, IDM_CHART_INDEX_HELP,  _T("ָ���÷�ע��"));
		menu.AppendMenu(MF_STRING, IDM_CHART_INDEX_PARAM, _T("����ָ�����"));
		menu.AppendMenu(MF_STRING, IDM_CHART_INDEX,		  _T("�༭ָ�깫ʽ"));

		menu.AppendMenu(MF_SEPARATOR);
		int32 iOften = LoadOftenIndex(&menu);		// Ҫcheck����
		if ( iOften <= 0 )
		{
			menu.RemoveMenu(menu.GetMenuItemCount()-1, MF_BYPOSITION);	// ɾ�����һ���ָ���
		}

		menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL, pt.x, pt.y, AfxGetMainWnd());
		return;
	}
	CNewMenu menu;
	menu.LoadMenu(IDR_MENU_TREND);
	menu.LoadToolBar(g_awToolBarIconIDs);

	CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));

	//�����޸�Ϊ�Ի�ģʽ
	// 	for(int i = 0;i < pPopMenu->GetMenuItemCount();++i)
	// 	{
	// 		UINT nID=pPopMenu->GetMenuItemID(i);
	// 		pPopMenu->ModifyMenu(i,MF_BYPOSITION|MF_OWNERDRAW,nID);
	// 	}
	CMenu* pTempMenu = NULL;

	/////////////////////////////////////////////////////////////////////////////////////////

	// ����볡 - ���ղ˵�
	CNewMenu *pNewMenu = NULL;
	{
		// ����
		pNewMenu = DYNAMIC_DOWNCAST(CNewMenu, pPopMenu->GetSubMenu(_T("���շ�ʱ")));
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
				// �볡���Ʋ�����1��
				pNewMenu->CheckMenuItem(ID_TREND_MULTIDAY1, MF_UNCHECKED |MF_BYCOMMAND);
			}
		}
		pPopMenu->CheckMenuItem(ID_TREND_SINGLESECTION, (m_bSingleSectionTrend ? MF_CHECKED:MF_UNCHECKED) |MF_BYCOMMAND);
	}

	// �ɽ�����ɫ
	if ( IsShowVolBuySellColor() )
	{
		pPopMenu->CheckMenuItem(ID_TREND_SHOWVOLCOLOR, MF_BYCOMMAND|MF_CHECKED);
	}

	// �Զ����Ӵ���
	{
		CNewMenu *pSubMenu = DYNAMIC_DOWNCAST(CNewMenu, pPopMenu->GetSubMenu(_T("����Ʒ��")));
		ASSERT( pSubMenu != NULL );
		if ( NULL != pSubMenu )
		{
			pSubMenu->CheckMenuItem(ID_KLINE_AUTOADDEXPCMPMERCH, (IsAutoAddExpCmpMerch()?MF_CHECKED:MF_UNCHECKED) |MF_BYCOMMAND);
		}
	}

	// �ǵ�ͣ���
	{
		float fRise, fFall;
		if ( CalcRiseFallMaxPrice(fRise, fFall) )
		{
			pPopMenu->CheckMenuItem(ID_TREND_SHOWRFMAX, (m_bShowRiseFallMax ? MF_CHECKED : MF_UNCHECKED) |MF_BYCOMMAND);
			pPopMenu->CheckMenuItem(ID_TREND_RFMAXAXIS, (m_bRiseFallAxis ? MF_CHECKED : MF_UNCHECKED) |MF_BYCOMMAND);
		}
		else
		{
			// ɾ�������˵�
			pPopMenu->DeleteMenu(ID_TREND_SHOWRFMAX, MF_BYCOMMAND);
			pPopMenu->DeleteMenu(ID_TREND_RFMAXAXIS, MF_BYCOMMAND);
		}

		pPopMenu->CheckMenuItem(ID_TREND_SHOWPERCENT, (IsEnableRightYRegion() ? MF_CHECKED : MF_UNCHECKED) |MF_BYCOMMAND);
	}

	// ���۵�ǰ�����
	{
		if ( NULL != GetMerchXml()  && GetMerchXml()->m_pRealtimePrice)
		{
			pPopMenu->CheckMenuItem(ID_TREND_SHOWAVG_PRICE, (m_bShowAvgPrice ? MF_CHECKED : MF_UNCHECKED) |MF_BYCOMMAND);
		}
		else
		{
			// ɾ�������˵�
			pPopMenu->DeleteMenu(ID_TREND_SHOWAVG_PRICE, MF_BYCOMMAND);
		}
	}

	// ����ͳ�� ������ɾ��
	if ( 1 != m_iTrendDayCount )
	{
		pPopMenu->RemoveMenu(ID_CHART_INTERVALSTATISTIC, MF_BYCOMMAND);
	}


	//////////////////////////////////////////////////////////////////////////
	m_aUserBlockNames.RemoveAll();

	CArray<T_Block, T_Block&> aBlocks;
	CUserBlockManager::Instance()->GetBlocks(aBlocks);

	//////////////////////////////////////////////////////////////////////////
	//---wangyongxue �Ѿ���ӵ���ѡ�ģ������뵽��ѡ����Ϊ��ɾ����ѡ�ɡ�
	CString StrBlockName = CUserBlockManager::Instance()->GetServerBlockName();

	if (StrBlockName.IsEmpty())
	{
		StrBlockName = _T("�ҵ���ѡ");
	}

	bool32 bZixuan= CUserBlockManager::Instance()->BeMerchInBlock(m_pMerchXml, StrBlockName);

	if (bZixuan)
	{
		menu.ModifyODMenu(ID_BLOCK_ADDTOFIRST,MF_BYCOMMAND|MF_STRING,ID_BLOCK_DELETEFROMFIRST,L"ɾ����ѡ��");
	}

	// ��������:
	pTempMenu = pPopMenu->GetSubMenu(L"��������");
	CNewMenu * pIoViewPopMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );
	ASSERT(NULL != pIoViewPopMenu );
	AppendIoViewsMenu(pIoViewPopMenu, IsLockedSplit());
	//

	// ������������ָ�״̬����Ҫɾ��һЩ��ť
	CMPIChildFrame *pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pChildFrame && pChildFrame->IsLockedSplit() )
	{
		pChildFrame->RemoveSplitMenuItem(*pPopMenu);
	}

	// ������Ƕ�ʱ�ν�����Ʒ����ôɾ�����볡ʮ�֡��˵�
	if (!bMulitTimeSection)
		pPopMenu->DeleteMenu(ID_TREND_SINGLESECTION, MF_BYCOMMAND);

	// ���ǹ�Ʊ����ɾ������ʱ����ͳ�ơ��͡���ɫ���ֳɽ�����ʾ�������Ӳ˵���
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
			// �л�����ʱͼ
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
			// �л���ָ�긱ͼ
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
			// �л���ָ�긱ͼ
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
		// ��ӵ�����Ʒ
		// �Ƿ�����Զ�����
		if ( !SetAutoAddExpCmpMerch(false, true) )
		{
			return;	// �û�����ȡ���Զ�����
		}
		ASSERT( !IsAutoAddExpCmpMerch() );

		MerchArray aMerchs;
		CDlgChooseStockVar::ChooseStockVar(aMerchs, false, m_pMerchXml);
		if ( aMerchs.GetSize() > 0 )
		{
			if ( AddCmpMerch(aMerchs[0], true, false) )
			{
				RequestSingleCmpMerchViewData(aMerchs[0]);	// ��������
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
		// ɾ�����е�����Ʒ
		SetAutoAddExpCmpMerch(false, false);
		RemoveAllCmpMerch();
	}

	if ( ID_TREND_SHOWRFMAX == nID )
	{
		// ��ʾ�ǵ�ͣ�۸�
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
		// ��ʾ�ǵ�ͣ�۸�
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
		// �ǵ�ͣ������ʾ
		m_bRiseFallAxis = !m_bRiseFallAxis;
		if ( NULL != m_pRegionMain )
		{
			m_pRegionMain->SetDrawFlag(CChartRegion::KDrawFull);
			Invalidate();
		}
	}
	else if ( ID_TREND_SHOWPERCENT == nID )
	{
		// ��ʾ�ٷֱ�����
		EnableRightYRegion( !IsEnableRightYRegion() );
		if ( NULL != m_pRegionMain )
		{
			m_pRegionMain->SetDrawFlag(CChartRegion::KDrawFull);
			Invalidate();
		}
	}
	else if ( ID_CHART_INTERVALSTATISTIC == nID )
	{
		// �˵�����ͳ��
		// �����
		m_aKLineInterval.RemoveAll();

		if ( 1 == m_iTrendDayCount )
		{
			// ��֧�ֵ��� && �볡
			T_MerchNodeUserData *pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
			if ( NULL != pData )
			{
				int32 iNodeBeginNow, iNodeEndNow;
				GetNodeBeginEnd(iNodeBeginNow, iNodeEndNow);
				if ( iNodeBeginNow < iNodeEndNow && pData->m_aKLinesFull.GetSize() > 0 )
				{
					// ׼������һ�ε�ͳ������
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
// 		// ָ��:
// 		// ָ��˵�
// /*		int32 iIndexNums = 0;
// 
// 		m_pRegionPick = m_pRegionMain;	
// 		m_FormulaNames.RemoveAll();
// 		CNewMenu menu;
// 		menu.LoadMenu(IDR_MENU_TREND);
// 		CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(0));
// 		
// 		CMenu* pTempMenu = pPopMenu->GetSubMenu(_T("����ָ��"));
// 		ASSERT( NULL != pTempMenu);
// 		CNewMenu* pSubMenu = DYNAMIC_DOWNCAST(CNewMenu,pTempMenu);
// 		
// 		pSubMenu->RemoveMenu(0,MF_BYPOSITION|MF_SEPARATOR);//ɾ��β��
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
// 			AfxMessageBox(L"��ǰ�޿���ָ��,����ָ�깫ʽ������!");
// 		}
// */
// 	}
// 	else if ( REGIONTITLELEFTBUTTONID == uID)
// 	{
// 		// ǰһ��ָ��
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
// 			// ��һ��:
// 			iIndex = 0; // ǰһ����ID��
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
// 						iIndex = i-1; // ǰһ����ID��
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
// 		// ��Region ��ָ�갴ť
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
// 		CMenu* pTempMenu = pPopMenu->GetSubMenu(_T("����ָ��"));
// 		ASSERT( NULL != pTempMenu);
// 		CNewMenu* pSubMenu = DYNAMIC_DOWNCAST(CNewMenu,pTempMenu);
// 		
// 		pSubMenu->RemoveMenu(0,MF_BYPOSITION|MF_SEPARATOR);//ɾ��β��
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
// 		// ��һ��ָ��
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
// 			// ��һ��:
// 			iIndex = 0; // ǰһ����ID��
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
// 						iIndex = i+1; // ǰһ����ID��
// 					}
// 				}
// 			}
// 		}
// 		
// 		MenuAddIndex(iIndex);
// 	}
// 	else if ( REGIONTITLEADDBUTTONID == uID)
// 	{
// 		// ���Ӹ�ͼ
// 		MenuAddRegion();
// 	}
// 	else if ( REGIONTITLEDELBUTTONID == uID)
// 	{	
// 		// �ر���Region
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
				StrText.Format(_T("ʱ: %s \n��: %s\n"),
					StrTime.GetBuffer(),
					Float2String(pNodeData->m_fVolume,iSaveDec,true).GetBuffer());
			}
			else if ( pCurve->GetTitle() == KStrTrendJun )
			{
				StrText.Format(_T("ʱ: %s \n��: %s\n"),
					StrTime.GetBuffer(),
					Float2String(pNodeData->m_fClose,iSaveDec,true).GetBuffer());
			}
			else if ( pCurve->GetTitle() == KStrTrendVol )
			{
				StrText.Format(_T("ʱ: %s \n��: %s\n"),
					StrTime.GetBuffer(),
					Float2String(pNodeData->m_fVolume,iSaveDec,true).GetBuffer());
			}
			else
			{
				StrText.Format(_T("ʱ: %s \n��: %s\n"),
					StrTime.GetBuffer(),
					Float2String(pNodeData->m_fVolume,iSaveDec,true).GetBuffer());
				StrTitle = pCurve->GetTitle();
			}
		}
		else if ( CheckFlag(pCurve->m_iFlag,CChartCurve::KTypeIndex) )
		{
			StrText.Format(_T("ʱ: %s\n%s: %s\n"),
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
	// �˵����ָ���ʱ��,��ɾ�����е�ָ��,����Ӧ�����滻�����ǵ���	
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
	//����
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

	// xl 0001762 ������еĿո���ʾ������������ʾ���ȣ����鴰�ڵĿ�ȱ�YLeft�ܶ���ʾһ������ - -
	int32 iYLeftShowChar = GetYLeftShowCharWidth();
	iYLeftShowChar += 1;

	//ʱ��
	CGridCellSys* pCellSys = (CGridCellSys*)pGridCtrl->GetCell(0,0);
	pCellSys->SetText(StrTimeLine1 + _T(""));

	pCellSys = (CGridCellSys*)pGridCtrl->GetCell(1,0);
	pCellSys->SetText(StrTimeLine2 + _T(""));

	//��λ
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(3,0);
	// StrValue = Float2String(KLine.m_fPriceClose, iSaveDec, true) + _T(" ");
	// pCellSymbol->SetText(StrValue);

	//StrValue = Float2SymbolString(KLine.m_fPriceClose,fPricePrevClose,iSaveDec) + _T(" ");
	Float2SymbolStringLimitMaxLength(StrValue, KLine.m_fPriceClose, fPricePrevClose, iSaveDec, iYLeftShowChar, true, false, true, false, true);
	pCellSymbol->SetText(StrValue);

	// �ж��Ƿ��ǺϷ�ֵ

	// �ڶ����Ǿ���
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

	// ����
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(5, 0);
	//StrValue = Float2SymbolString(KLine.m_fPriceAvg, fPricePrevClose, iSaveDec) + _T(" ");
	Float2SymbolStringLimitMaxLength(StrValue, KLine.m_fPriceAvg, fPricePrevClose, iSaveDec, iYLeftShowChar, true, false, true, false, true);
	pCellSymbol->SetText(StrValue);

	//�ǵ�=����-����
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(7, 0);

	StrValue = L" -";
	if (0. != KLine.m_fPriceClose)
	{
		//StrValue = Float2SymbolString(KLine.m_fPriceClose - fPricePrevClose, 0.0f, iSaveDec) + _T("");
		Float2SymbolStringLimitMaxLength(StrValue, KLine.m_fPriceClose - fPricePrevClose, 0.0f, iSaveDec, iYLeftShowChar, true, false, false);
	}
	pCellSymbol->SetText(StrValue);

	//�ǵ���(����-����)/����*100%
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(9, 0);

	StrValue = L" -";
	if (0. != KLine.m_fPriceClose && 0. != fPricePrevClose)
	{
		float fRisePercent = ((KLine.m_fPriceClose - fPricePrevClose) / fPricePrevClose) * 100.;
		StrValue = Float2SymbolString(fRisePercent, 0, 2, false, false, true) + _T("");
	}	
	pCellSymbol->SetText(StrValue);		

	//�ɽ���
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(11,0);	
	//StrValue = Float2String(KLine.m_fVolume, 0, true) + _T(" ");
	Float2StringLimitMaxLength(StrValue, KLine.m_fVolume, 0, GetYLeftShowCharWidth(1), true, true);
	pCellSymbol->SetText(StrValue);	

	// �ɽ���/�ֲ�
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

	if ( pRegion == m_pRegionMain )	// ��ͼ��Y����ʾ
	{
		CalcMainRegionXAxis(aAxisNodes, aXAxisDivide);
	}
	else
	{
		// zhangbo 20090710 #���Ż��� ���ڶ�������region������һ�飬 ��ʵ�����㣬 ���ƾͿ�����
		CalcMainRegionXAxis(aAxisNodes, aXAxisDivide);	
	}
}

void CIoViewTrend::OnCalcYAxis (CChartRegion* pRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide)
{
	aYAxisDivide.SetSize(0);

	// 
	if ( pRegion == m_pRegionMain )	// ��ͼ��Y����ʾ
	{
		CalcMainRegionYAxis(pDC, aYAxisDivide);
	}
	else	// ��ͼ��Y����ʾ
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

	// ������Ԥ���ľ�����:
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
	// �������Ʒ			
	if ( 1 == AlarmMerch.m_iConditon || 2 == AlarmMerch.m_iConditon)
	{
	// �Ǽ۸����ƻ������Ƶ�����:				
	float fPrice = AlarmMerch.m_fParam;

	// zhangbo 20090703 #��ʱ�ܵ�
	// 				// ������۸��ڲ�����Сֵ�����ֵ֮��.�ǵĻ��ͻ���:
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
	// 					// ��ʼ�������߿�: ȡ��Y ��̶ȵ����ֵ����Сֵ
	// 
	// 					if ( m_pRegionYLeft->m_AxisTexts.GetSize() > 0 )
	// 					{						
	// 						// ��������ߵ�Y ����:
	// 						
	// 						CRect Rect = m_pRegionYLeft->GetRectCurves();
	// 						
	// 						int32 iY = Rect.Height()*(fPrice - fMin)/(fMax - fMin);
	// 						iY = Rect.bottom - iY;
	// 						
	// 						// ����:
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
	// 						// д��:
	// 
	// 						CString StrAlarm;
	// 
	// 						StrAlarm.Format(L"��:%.3f",fPrice);
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

	// �����¼۵���:
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

		// ������
		CPen penDraw;
		penDraw.CreatePen(PS_SOLID, 1, clrLine);
		CPen* pOldPen = (CPen*)pDC->SelectObject(&penDraw);;
		CRect rtRegion = m_pRegionMain->m_RectView;

		//
		pDC->MoveTo(iX, iYPos);
		pDC->LineTo(rtRegion.right, iYPos);		

		// ��һ������
		CBrush br(clrShow);
		CBrush* pOldBrush = pDC->SelectObject(&br);

		CRect rtCircle(CPoint(ptCenter.x - 3, ptCenter.y - 3), CPoint(ptCenter.x + 3, ptCenter.y + 3));
		pDC->Ellipse(rtCircle);

		// ���ּۺ��ǵ���
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
			// ������ݿ�����, ̫�����Ҳ�, �Ͱ��ּۺ��ǵ�����������������
			m_bDrawYRigthPrice = true;
		}
		else
		{
			m_bDrawYRigthPrice = false;
		}

		// �߶�̫С�Ͳ�Ҫ����
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

	// �߶�̫С�Ͳ�Ҫ����
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
		//��ͼ
		if (pChartRegion == m_pRegionMain )
		{
			if(NULL != m_pMerchXml)
			{
				// �Ǵ�����Ʒ���Ż��Ʋƾ�����СԲ��
				if (m_pMerchXml->m_Market.m_MarketInfo.m_eMarketReportType == ERTMony)
				{
					// ���Ʋƾ�����СԲ��
					DrawCircleDot(pDC);
				}
			}	
			DrawTitle1(pDC);

			// ������Ԥ����
			DrawAlarmMerchLine();

			// ��Ϣ����
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

		// ����ͼ
		if ( ETBT_CompetePrice == GetCurTrendBtnType() )
		{
			DrawCompetePriceChart(pDC, *pChartRegion);
		}

		if ( m_SubRegions.GetSize() > 0
			&& m_SubRegions[0].m_pSubRegionMain == pChartRegion )
		{
			// ��һ����ͼ�����ֲֻ���ȥ xl 1102
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

		// ��ͼ
		if (pChartRegion == m_pRegionMain)	// ��ͼ
		{
			CRect RectMain = m_pRegionMain->GetRectCurves();

			// ��ˮƽ��
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

					// ����
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

			// ����ֱ��
			{
				CAxisDivide *pAxisDivide = (CAxisDivide *)m_pRegionMain->m_aXAxisDivide.GetData();
				// ��һ����Ҫ����,�غ��˲��ÿ������һ��Ҳ������
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
							// ���շ�ʱ����ʵ��
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
					// û���ݵ�ʱ��,������
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

			// �����һ��������
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

			// ��������
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

			// ��������
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
		else if (pChartRegion == m_pRegionXBottom)	// ��������
		{
			// ���´���Ϊ�����������ʱ������ӵ��������
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

			// �����ȼ�����
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

			// ����������֣� �������ּ��ص�
			CArray<CRect, CRect&> aRectSpace;	// ��������
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

				// ����Ǵ�����Ʒ����ô�������Ʒ��ʱͼ��ʱ��̶�
				if (m_pMerchXml->m_Market.m_MarketInfo.m_eMarketReportType == ERTMony)
				{
					SaveTimeDegreeScale(Rect.left + Rect.Width()/2, pAxisDivide[i].m_DivideText1.m_StrText);
				}


				// �ӿ��пռ��в��ң� �����Ƿ������ʾ
				int32 iPosFindInSpaceRects = -1;

				CRect *pRectSpace = (CRect *)aRectSpace.GetData();
				for (int32 iIndexSpace = 0; iIndexSpace < aRectSpace.GetSize(); iIndexSpace++)
				{
					if (Rect.left < pRectSpace[iIndexSpace].left || Rect.right > pRectSpace[iIndexSpace].right)
						continue;

					iPosFindInSpaceRects = iIndexSpace;
					break;
				}

				// ��ֿ�������
				if (iPosFindInSpaceRects < 0)
					continue;
				else
				{
					// ������
					// ���ư汾��K��������������ɫ��ʱ���óɹ̶�ֵ
					pDC->SetTextColor(RGB(125, 124, 129));
					//pDC->SetTextColor(pAxisDivide[i].m_DivideText1.m_lTextColor);
					pDC->SetBkMode(TRANSPARENT);
					pDC->DrawText(pAxisDivide[i].m_DivideText1.m_StrText, &Rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

					// ��ֿ�������
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

			// ����ʱͼ���ڵĴ�С�ı��ˣ���ôҪ���¼���ƾ��������ݵ�СԲ������
			if (m_bSizeChange && !m_bMoreView)
			{
				if (!m_vecXAxisInfo.empty())
				{
					ParseEonomicData(m_strEconoData.c_str());
					m_bSizeChange = false;
				}
			}
		}
		else if ( pChartRegion == m_pRegionLeftBottom )	// ��ʾ��ǰ��ʾ��������
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
				//����ʱ��
				pDC->SetTextColor(RGB(125, 124, 129));	//���ư汾����ʱ�ù̶�ɫֵ
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
		else	// ��ͼ
		{
			for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
			{
				T_SubRegionParam &SubRegion = m_SubRegions[i];

				if (pChartRegion == SubRegion.m_pSubRegionMain)
				{
					if ( 0 == i )
					{
						// ��һ����ͼ��Ȼ�ǳɽ���ͼ, ��ͼ����ʾ�ߵ�����
						AddFlag(pChartRegion->m_iFlag, CChartRegion::KDonotShowCurveDataText);
					}
					CRect RectSubMain = SubRegion.m_pSubRegionMain->GetRectCurves();

					{
						// ���
						if (  pChartRegion->GetTopSkip() > iHalfHeight+6 )		// �ɽ�����ͼ��topskip�����٣����Ȼ���Ȧ )
						{
							pt1 = RectSubMain.TopLeft();
							pt2 = CPoint(RectSubMain.right, RectSubMain.top);
							pDC->_DrawLine(pt1, pt2);
						}

						pt1 = CPoint(RectSubMain.left, RectSubMain.bottom);
						pt2 = CPoint(RectSubMain.right, RectSubMain.bottom);
						pDC->_DrawLine(pt1, pt2);						
					}

					// ��ˮƽ��
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
								// �ɽ�����ͼ�ĵ�һ�����߸߶�С�ڴ�
								continue;
							}

							// ����
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

					// ����ֱ��
					{						
						CAxisDivide *pAxisDivide = (CAxisDivide *)m_pRegionMain->m_aXAxisDivide.GetData();
						// ��һ����Ҫ����,�غ��˲��ÿ������һ������
						for (int32 i = 1; i < m_pRegionMain->m_aXAxisDivide.GetSize()-1; i++)
						{
							CAxisDivide &AxisDivide = pAxisDivide[i];
							pt1.x = pt2.x = AxisDivide.m_iPosPixel;
							pt1.y = RectSubMain.top;
							pt2.y = RectSubMain.bottom;

							if ( pChartRegion->GetTopSkip() < iHalfHeight+6 )
							{
								pt1.y = pChartRegion->m_RectView.top;	// �ɽ�����ͼֱ�Ӵ���
							}

							if (CAxisDivide::ELSSolid == AxisDivide.m_eLineStyle)
							{
								if ( m_aTrendMultiDayIOCTimes.GetSize() > 1 )
								{
									// ���շ�ʱ����ʵ��
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
							// û���ݵ�ʱ��,������
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

					// ��������
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

					// ��������
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

		//����ָ��ˮƽ��
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
			// ������,�����������.
			m_TimeHistory -= Span;
			m_TimeHistory -= Span;
		}
		else if ( 7 == m_TimeHistory.GetDayOfWeek())
		{
			// ������,�����������.
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
			// �����ǰ��ʾ�ľ��ǽ����,��ô������ͺ���
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
	// fangz0401 ��ʱ���Ƴɽ�����ʱ����ʾ,�Ӵ��ڱ���.��������.
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

		// �÷�ʱͼ��region�߶Ȼ�ԭ
		UpdateAxisSize(false);	// ��Ҫ����֮ǰ���ú��ʵ�x������߶�
		// ����⼸���������
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
			// α��xmlregion����
			double dfRatio = m_aSubRegionHeightRatios[i];
			m_aRegionHeightXml.Add( (int32)(KIViewHeight*dfRatio) );
			m_aRegionHeightRadioXml.Add( dfRatio );
			m_IndexNameXml.Add(_T(""));
			m_IndexRegionXml.Add(1+i);
		}
		// ����Ӹ�ͼ
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
		OnTrendBtnSelChanged();	// ��ť��ʼ��, ����ᶪʧԭ���ĵڶ���ͼ�ĸ߶�
	}
	else
	{
		SetTimer(KTimerIdTrendInit, KTimerPeriodTrendInit, NULL);	// ��ʱ��ʼ����ť
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
	case 1:	 StrWeek = L"��";
		break;
	case 2:	 StrWeek = L"һ";
		break;
	case 3:	 StrWeek = L"��";
		break;
	case 4:	 StrWeek = L"��";
		break;
	case 5:	 StrWeek = L"��";
		break;
	case 6:	 StrWeek = L"��";
		break;
	case 7:	 StrWeek = L"��";
		break;
	}

	//StrTime.Format(L"��ʷ��ʱ����: %s %s %d��%d��%d�� ����%s  CTRL+ ���ҷ�����鿴ǰ��һ���ʱ,�����л���Ʒ",
	StrTime.Format(L"��ʷ��ʱ: %s(%s) %d��%d��%d�� ����%s  PageUp/Down���鿴ǰ��һ���ʱ",
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
	// ������ʱ����ֻʣ��merchchangedʱ��
}

void CIoViewTrend::ClearLocalData(bool32 bClearAll/* = true*/)
{
	for (int32 i = 0; i < m_MerchParamArray.GetSize(); i++)
	{
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(i);
		if ( !bClearAll && !pData->bMainMerch )
		{
			continue;	// ������Ʒ�����
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
		// �����߲�������ͬx�ᣬ����
		m_pCompetePriceNodes->RemoveAll();
	}
}

void CIoViewTrend::OnRectIntervalStatistics(int32 iNodeBegin, int32 iNodeEnd)
{
	// �����
	m_aKLineInterval.RemoveAll();

	// �������:
	if (m_MerchParamArray.GetSize() <= 0)
	{
		return;
	}

	// ����ͳ�� �����²�����
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

	// ׼������һ�ε�ͳ������
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

	// �����˵�:
	CPoint pt;
	GetCursorPos(&pt);

	//
	//CNewMenu Menu;
	//Menu.CreatePopupMenu();
	//Menu.AppendODMenu(L"����ͳ��", MF_STRING, KiMenuIDIntervalStatistic);	
	//Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, this);

	//OnMenuIntervalStatistic();

	// �ػ���ͼ
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

	// ���Ը���������صı�������
	if (0. == MerchNodeUserData.m_TrendTradingDayInfo.m_fPricePrevClose && 0. == MerchNodeUserData.m_TrendTradingDayInfo.m_fPricePrevAvg)
	{
		float fPricePrevClose = MerchNodeUserData.m_TrendTradingDayInfo.GetPrevReferPrice();
		MerchNodeUserData.m_TrendTradingDayInfo.RecalcPrice(*pMerch);
		if (fPricePrevClose != MerchNodeUserData.m_TrendTradingDayInfo.GetPrevReferPrice())
		{
			// zhangbo 20090708 #������, ��������ֵ
			//...		
		}
	}
	if ( 0.0 == MerchNodeUserData.m_TrendTradingDayInfo.GetPrevHold() )
	{
		MerchNodeUserData.m_TrendTradingDayInfo.RecalcHold(*pMerch);
	}

	// ����������շ���k��
	if ( m_aTrendMultiDayIOCTimes.GetSize() < m_iTrendDayCount )
	{
		TryReqMoreMultiDayMinuteKLineData();
	}

	CUpdateShowDataChangeHelper helperChange(this, &MerchNodeUserData);	// ������仯

	// ��������
	int32 iPosFound;
	CMerchKLineNode* pKLineRequest = NULL;
	pMerch->FindMerchKLineNode(EKTBMinute, iPosFound, pKLineRequest);
	if (NULL == pKLineRequest || 0 == pKLineRequest->m_KLines.GetSize())	// �����Ҳ���K�����ݣ� �ǾͲ���Ҫ��ʾ��
	{
		MerchNodeUserData.m_aKLinesCompare.RemoveAll();
		MerchNodeUserData.m_aKLinesFull.RemoveAll();

		if (NULL != MerchNodeUserData.m_pKLinesShow)	
			MerchNodeUserData.m_pKLinesShow->RemoveAll();

		if ( IsSuspendedMerch() )
		{
			CreateSuspendedVirtualNoOpenKlineData();	// ͣ����Ʒ����������
		}

		return true;
	}

	// �Ż��������������£� ���¼���������ʵʱ���ݸ��������ģ� ������������ر��жϴ���

	CGmtTime TimeStart, TimeEnd;
	if ( !GetTrendStartEndTime(TimeStart, TimeEnd) )
	{
		ASSERT( 0 );	// ��Чʱ��Σ���

		return false;
	}

	if ( m_bSingleSectionTrend )
	{
		UpdateTrendMultiDayChange();	// ���ܰ볡ʱ����Ѿ������˱������Ҫ���±�����ʾ, �������Ǹ��±�����ʾ
	}

	// ��ȡK�������У� ������ʾ�Ĳ���
	int32 iStartSrc	= CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pKLineRequest->m_KLines, TimeStart);
	if (iStartSrc < 0 || iStartSrc >= pKLineRequest->m_KLines.GetSize())
	{
		// �Ҳ�����Ҫ��ʾʱ���ڵķ���K��
		MerchNodeUserData.m_aKLinesCompare.RemoveAll();
		MerchNodeUserData.m_aKLinesFull.RemoveAll();

		if (NULL != MerchNodeUserData.m_pKLinesShow)
			MerchNodeUserData.m_pKLinesShow->RemoveAll();

		ClearLocalData(false);	// �����������Ҫ���

		if ( IsSuspendedMerch() )
		{
			CreateSuspendedVirtualNoOpenKlineData();	// ͣ����Ʒ����������
		}

		return true;
	}

	int32 iEndSrc = CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(pKLineRequest->m_KLines, TimeEnd);

	// 
	CKLine *pKLineSrc = (CKLine *)pKLineRequest->m_KLines.GetData() + iStartSrc;
	int32 iCountSrc = iEndSrc - iStartSrc + 1;

	CKLine *pKLineCmp = (CKLine *)MerchNodeUserData.m_aKLinesCompare.GetData();

	// �����������
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
		// ���¸�ֵcmpָ��&����
		// src�����ں��治Ӧ����ʹ��
		pKLineCmp = pData->m_aKLinesCompare.GetData();
		iCountSrc = pData->m_aKLinesCompare.GetSize();
	}

	{
		// ��ͼ��������
		if (iCountSrc == 0)
		{
			return true;		// ����û�����ݣ� �Ͳ�������
		}

		// 

		// ����Full����
		FillTrendNodes(*pData, m_pAbsCenterManager->GetServerTime(), pKLineCmp, iCountSrc);

		// ������ͼ�����ָ�� - ������ٵ��������⣬δ���
		for (int32 j = 0; j < pData->aIndexs.GetSize(); j++)
		{
			T_IndexParam* pIndex = pData->aIndexs.GetAt(j);
			g_formula_compute(this, this, (CChartRegion*)pIndex->pRegion, pData, pIndex, 0, pData->m_aKLinesFull.GetSize());
		}

		// ������ʾ����
		if (!pData->UpdateShowData(0, pData->m_aKLinesFull.GetSize()))
		{
			//ASSERT(0);
			return false;
		}

		// ����ֻ�е��콻���յ����ݣ�����ֻ���ڵ�������
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
							&& pIndex->m_aIndexLines.GetSize() > 0 )	// ���⴦��
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
									continue;	// �޷���λ��������
								}
								// ������ȫ������ݷ�Χ������bsֻ��һ���֣�bsû�е�ȫ���趨ƽ��
								ASSERT( iNodeEnd == aNodes.GetSize()-1 );
								int32 m = 0;
								for ( m=iNodeStart; m < aNodes.GetSize() && m <= iNodeEnd ; ++m )
								{
									bool32 bMatch = false;
									if ( iTmpMinutePos < iMinuteBSCount )
									{
										int32 iMBS = aBSData[iTmpMinutePos].m_lTime/60;
										int32 iMNode = aNodes[m].m_iID/60;
										while ( iMNode > iMBS )// nodeʱ����֮��, �ƶ�BS�ڵ�
										{
											++iTmpMinutePos;
											if ( iTmpMinutePos >= iMinuteBSCount )
											{
												break;	// �����ƶ�bs�ڵ�
											}
											iMBS = aBSData[iTmpMinutePos].m_lTime/60;
										}
										if ( iMBS > iMNode )
										{
											// nodeʱ����֮ǰ,��node��Ϊƽ��
										}
										else
										{
											bMatch = iTmpMinutePos < iMinuteBSCount;	// ��Чƥ����
										}
									}
									if ( bMatch )
									{
										ASSERT( iTmpMinutePos < iMinuteBSCount );
										float fDiff = aBSData[iTmpMinutePos].m_fBuyVolume - aBSData[iTmpMinutePos].m_fSellVolume;
										++iTmpMinutePos;
										if ( fDiff > 0.1f )
										{
											// ��>�⣬����
											aNodes[m].m_bClrValid = true;
											aNodes[m].m_clrNode = (unsigned long)(CNodeData::ENCFColorFall);
										}
										else if ( fDiff < -0.1f )
										{
											// ��>�� ����
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
										// û��bsֵ������ƽ��
										aNodes[m].m_bClrValid = true;
										aNodes[m].m_clrNode = (unsigned long)(CNodeData::ENCFColorKeep);
									}
								}

								for ( ; m < aNodes.GetSize() ; ++m )
								{
									// ʣ�µ�û���ݵģ�ȫ��ƽ��
									aNodes[m].m_bClrValid = true;
									aNodes[m].m_clrNode = (unsigned long)(CNodeData::ENCFColorKeep);
								}
							}//for ( int32 k=0; k < pIndex->m_aIndexLines.GetSize() ; ++k )
						}//if ( pIndex->strIndexName == CString(_T("VOLFS"))
					}//for (int32 j = 0; j < pData->aIndexs.GetSize(); j++)
				}//if ( iMinuteBSPos < iMinuteBSCount && aBSData[iMinuteBSPos].m_lTime <= tEnd )
			}//if ( iTodayStart >=0 && iTodayEnd >= iTodayStart && iTodayEnd < pData->m_aKLinesFull.GetSize() )
		}//if ( 1 == m_iTrendDayCount && pData->m_pMerchNode->m_aMinuteBS.GetSize() > 0 && pData->m_aKLinesFull.GetSize() > 0 )

		// ������ֲ���������ݣ���ʾ�����н�������ݱ�Ȼ�����������
		// ����������ݵ�ָ������invalid
		// ����ָ�궼��������ݣ�������Щ���ݵ�flagΪinvalid
		if ( iCountSrc != iCountSrcOldCmp )
		{
			ASSERT( IsSuspendedMerch() );
			TrimSuspendedIndexData();
		}

		// ���¼������� Ϊʲô������������ᵼ�� " 0001683 3 ���ڡ�[0423] �л���Ʒʱ, ������ͼ���²���ʱ? " �����Ѿ�����˰�.
		// UpdateAxisSize();

		// ��ʾˢ��
		m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
		for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
		{
			m_SubRegions[i].m_pSubRegionMain->SetDrawFlag(CRegion::KDrawFull);
		}	

		// ���¸�ͼ����
		// zhangbo 20090714 #������...
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

	// zhangbo 20090714 #������
	//...
	// ���Ը���������صı�������

	RemoveCmpMerchFromNeedUpdate(pMerch);

	if (0. == MerchNodeUserData.m_TrendTradingDayInfo.m_fPricePrevClose && 0. == MerchNodeUserData.m_TrendTradingDayInfo.m_fPricePrevAvg)
	{
		float fPricePrevClose = MerchNodeUserData.m_TrendTradingDayInfo.GetPrevReferPrice();
		MerchNodeUserData.m_TrendTradingDayInfo.RecalcPrice(*pMerch);
		if (fPricePrevClose != MerchNodeUserData.m_TrendTradingDayInfo.GetPrevReferPrice())
		{
			// zhangbo 20090708 #������, ��������ֵ
			//...		
		}
	}

	// ��������
	int32 iPosFound;
	CMerchKLineNode* pKLineRequest = NULL;
	pMerch->FindMerchKLineNode(EKTBMinute, iPosFound, pKLineRequest);
	if (NULL == pKLineRequest || 0 == pKLineRequest->m_KLines.GetSize())	// �����Ҳ���K�����ݣ� �ǾͲ���Ҫ��ʾ��
	{
		MerchNodeUserData.m_aKLinesCompare.RemoveAll();
		MerchNodeUserData.m_aKLinesFull.RemoveAll();

		if (NULL != MerchNodeUserData.m_pKLinesShow)	
			MerchNodeUserData.m_pKLinesShow->RemoveAll();

		MerchNodeUserData.m_iShowPosInFullList = MerchNodeUserData.m_iShowCountInFullList = 0;

		return true;
	}

	// ʱ���������Ʒ��Ϊ׼
	CGmtTime TimeStart, TimeEnd;
	if ( !GetTrendStartEndTime(TimeStart, TimeEnd) )
	{
		ASSERT( 0 );	// ��Чʱ��Σ���
		return false;
	}

	// ��ȡK�������У� ������ʾ�Ĳ���
	int32 iStartSrc	= CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pKLineRequest->m_KLines, TimeStart);
	if (iStartSrc < 0 || iStartSrc >= pKLineRequest->m_KLines.GetSize())
	{
		// �Ҳ�����Ҫ��ʾʱ���ڵķ���K��
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

	// ���Ƚ���ȫ��ͬ�����, ��ȫ��ͬ��������û����
	bool32 bSameCmp = iCountCmp == iCountSrc && 0 == memcmp(pKLineCmp, pKLineSrc, iCountCmp*sizeof(CKLine));

	bool32 bNeedUpdate = false;
	if ( !bSameCmp )
	{
		// ��ͼ��������
		if (iCountSrc == 0)
		{
			MerchNodeUserData.m_aKLinesCompare.RemoveAll();
			MerchNodeUserData.m_aKLinesFull.RemoveAll();

			if (NULL != MerchNodeUserData.m_pKLinesShow)
				MerchNodeUserData.m_pKLinesShow->RemoveAll();

			MerchNodeUserData.m_iShowPosInFullList = MerchNodeUserData.m_iShowCountInFullList = 0;
			return true;		// ����û�����ݣ� �Ͳ�������
		}

		{
			T_MerchNodeUserData* pData = &MerchNodeUserData;	// ������Ʒ����
			pData->m_aKLinesCompare.SetSize(iCountSrc, 100);

			pKLineCmp = (CKLine *)pData->m_aKLinesCompare.GetData();
			memcpyex(pKLineCmp, pKLineSrc, iCountSrc * sizeof(CKLine));
		}
		bNeedUpdate = true;
	}

	if ( !bNeedUpdate )
	{
		// cmp������ͬ����full�����бȰ����㹻����Ϣ����ʾ
		// ʹ�ÿ�ʼ����ʱ���ȡһ��������
		// ���п������л���Ʒ�������ƣ�ȡ��
		bNeedUpdate = true;	// ����Ҫ����Full
		// 		CArray<CKLine, CKLine> aTmpKline;
		// 		const int32 iFullSize = MerchNodeUserData.m_aKLinesFull.GetSize();
		// 		int32 iFullStartNew = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(MerchNodeUserData.m_aKLinesFull, TimeStart);
		// 		int32 iFullEndNew = CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(MerchNodeUserData.m_aKLinesFull, TimeEnd);
		// 		if ( iFullStartNew >= 0
		// 			&& iFullEndNew >= iFullStartNew 
		// 			&& iFullEndNew < iFullSize )
		// 		{
		// 			// ��ȡ
		// 			int32 iPatchCount = iFullEndNew - iFullStartNew+1;
		// 			aTmpKline.SetSize(iPatchCount);
		// 			memcpyex(aTmpKline.GetData(), MerchNodeUserData.m_aKLinesFull.GetData()+iFullStartNew, iPatchCount*sizeof(CKLine));
		// 			memcpyex(MerchNodeUserData.m_aKLinesFull.GetData(), aTmpKline.GetData(), iPatchCount*sizeof(CKLine));
		// 			MerchNodeUserData.m_aKLinesFull.SetSize(iPatchCount);
		// 
		// 			// ���ڷ�ʱ�����˱�־λ�����Ա�־λҲ��Ҫ����
		// 			// ���Full��������
		// 		}
		// 		else
		// 		{
		// 			bNeedUpdate = true;	// ����ԭʼ����
		// 		}
	}

	if ( bNeedUpdate )
	{
		// ����Full����
		FillTrendNodes(MerchNodeUserData, m_pAbsCenterManager->GetServerTime(), MerchNodeUserData.m_aKLinesCompare.GetData(), iCountSrc);
	}

	// ������ʾ����
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
	// 		ASSERT( 0 );	// ��֧��0�۸���Ʒ
	// 		return false;
	// 	}

	// �ȳ�ʼ����һ������
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
	pKLineFull[iCountNode].m_fHold			= MerchNodeUserData.m_TrendTradingDayInfo.GetPrevHold();	// ȡ����ֲ�
	bool32 bFirstNodeHold = true;
	if ( pKLineFull[iCountNode].m_fHold <= 0.0 )
	{
		// ��ʱ�Ѿ������ˣ�������һ����Ч����ֵ��Ϊ�������
		bFirstNodeHold = false;
		pKLineFull[iCountNode].m_fHold = pKLines[iCountNode].m_fHold;
	}

	bool32 bFoundFirstNodeData = false;		// �Ƿ��˵�һ����Ч����

	// ����ֶδ���
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
						ASSERT(MerchNodeUserData.m_pMerchNode != m_pMerchXml);		// ������Ʒ���ܳ���,��Ӧ�ó����������
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
						memcpyex(&pKLineFull[iCountNode], &pKLines[iKLineIndex], sizeof(CKLine));	// һ��һ��ֵ
						bFoundFirstNodeData = true;	// ���������Ѿ� ��������������

						//
						iCountNode++;
						iKLineIndex++;
					}
					else // ������һ��ֵ
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
				else	// ������һ��ֵ
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
		// ��������
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
							//ASSERT(!MerchNodeUserData.bMainMerch || MerchNodeUserData.m_pMerchNode != m_pMerchXml);		// ������Ʒ���ܳ���,��Ӧ�ó����������
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
							memcpyex(&pKLineFull[iCountNode], &pKLines[iKLineIndex], sizeof(CKLine));	// һ��һ��ֵ
							bFoundFirstNodeData = true;	// ���������Ѿ� ��������������

							//
							iCountNode++;
							iKLineIndex++;
						}
						else // ������һ��ֵ
						{
							ASSERT( pKLines[iKLineIndex].m_TimeCurrent.GetTime() >= uiTime );
							if (iCountNode > 0)
							{
								if ( iLastDayPos>=0 && iLastDayPos == iCountNode-1 )
								{
									// ��������ݣ����޸����ռ�&���ۣ������۸�ȫ��Ϊ0
									CKLine kline;
									kline.m_fPriceAvg = kline.m_fPriceClose = pKLineFull[iCountNode-1].m_fPriceClose;
									memcpyex(&pKLineFull[iCountNode], &kline, sizeof(CKLine));
								}
								else
								{
									// �����̳���һ��ֵ
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
								// �ڶ��շ�ʱ�£���һ����Ч����ǰ���������ݶ��ǷǷ��ڵ�
								pFlags[iCountNode] = CNodeData::KValueInvalid;	// ֵ�Ƿ�
							}
							iCountNode++;
						}
					}
					else	// ������һ��ֵ
					{
						if (iCountNode > 0)
						{
							if ( iLastDayPos>=0 && iLastDayPos == iCountNode-1 )
							{
								// ��������ݣ����޸����ռ�&���ۣ������۸�ȫ��Ϊ0
								CKLine kline;
								kline.m_fPriceAvg = kline.m_fPriceClose = pKLineFull[iCountNode-1].m_fPriceClose;
								memcpyex(&pKLineFull[iCountNode], &kline, sizeof(CKLine));
							}
							else
							{
								// �����̳���һ��ֵ
								memcpyex(&pKLineFull[iCountNode], &pKLineFull[iCountNode - 1], sizeof(CKLine));
							}
							pKLineFull[iCountNode].m_TimeCurrent= uiTime;
							pKLineFull[iCountNode].m_fVolume	= 0.;
							pKLineFull[iCountNode].m_fAmount	= 0.;
						}
						if ( m_aTrendMultiDayIOCTimes.GetSize() > 1 && !bFoundFirstNodeData )
						{
							// �ڶ��շ�ʱ�£���һ����Ч����ǰ���������ݶ��ǷǷ��ڵ�
							pFlags[iCountNode] = CNodeData::KValueInvalid;	// ֵ�Ƿ�
						}
						iCountNode++;
					}
				}
			}

			// ���շ�ʱ�£�ÿ�յ����һ���ڵ�����һ��Ľڵ㲻����
			if ( iCountNode > 0 && m_aTrendMultiDayIOCTimes.GetSize() > 1 && bFoundFirstNodeData && iDay != 0 )
			{
				iLastDayPos = iCountNode-1;
				pFlags[iCountNode-1] |= CNodeData::KValueDivide;
			}
		}
	}


	if ( !bFirstNodeHold )
	{
		TRACE(_T("��ʱ����ֲ����ݿ��ܻ᲻����ȷ��ֵ!\r\n"));
	}

	// ��������
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

	const int32 kiSeperatorValve = 8*60-2;		// >�������ߵľͰ���1Сʱ�ָ����
	int32 kiTimeSeperator = 1800;				// Ĭ�ϰ�Сʱ�ָ��

	// 
	CRect Rect = m_pRegionMain->GetRectCurves();
	Rect.DeflateRect(1, 1, 1, 1);
	if ( m_bSingleSectionTrend )
	{
		// �볡����
		int32 iMaxTrendUnitCount = TrendTradingDay.m_MarketIOCTime.GetMaxTrendUnitCount();
		CGmtTime TimeOpen, TimeClose;
		if ( !GetTrendSingleSectionOCTime(TimeOpen, TimeClose) )
		{
			ASSERT( 0 ); // ��֪����ô���䣬Ĭ����ʾ��
		}
		else
		{
			iMaxTrendUnitCount = ((TimeClose.GetTime() - TimeOpen.GetTime()) / 60 + 1);
		}

		int32 iHalfTimeSeperator = kiTimeSeperator;	// Ĭ�ϰ�Сʱ�ָ���
		if ( iMaxTrendUnitCount > kiSeperatorValve )
		{
			int i2Multi = (1+iMaxTrendUnitCount/kiSeperatorValve);
			i2Multi = i2Multi - i2Multi%2;
			iHalfTimeSeperator *= i2Multi;
		}
		int32 iFullTimeSeperator = iHalfTimeSeperator*2;	// ����ָ��

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
			AxisNode.m_iTimeId = uiTime;		// ʱ��ڵ�

			/////////////////////////////////////////////////////////////////////////
			// �ڵ�
			CTime Time(uiTime);	// ���ǵ���ʱ����ʾ
			// ...fangz0815 ��ʱ�Ķ�. Ϊ��X �Ử����ʾ��ʱ��Ҫ��ʾ�·ݺ�����
			// AxisNode.m_StrSliderText1.Format(L"%02d/%02d %02d:%02d", Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute());
			AxisNode.m_StrSliderText1.Format(L"%02d:%02d", Time.GetHour(), Time.GetMinute());

			// �ڵ��� 
			if (0 == iIndexNode)
			{
				AxisNode.m_iStartPixel = Rect.left;
			}
			else
			{
				AxisNode.m_iStartPixel = pAxisNode[iIndexNode - 1].m_iEndPixel + 1;
			}

			// �ڵ���
			if (iMaxTrendUnitCount - 1 == iIndexNode)
			{
				AxisNode.m_iEndPixel = Rect.right;		// xl 0623 �����ұ���һ���߿��������ڲ��������
			}
			else
			{
				AxisNode.m_iEndPixel = Rect.left + (int32)((iIndexNode + 0.5) * fPixelWidthPerUnit + 0.5);
			}

			//
			if (AxisNode.m_iStartPixel > AxisNode.m_iEndPixel)
				AxisNode.m_iStartPixel = AxisNode.m_iEndPixel;

			// �нڵ�
			if (0 == iIndexNode)
				AxisNode.m_iCenterPixel = AxisNode.m_iStartPixel;
			else if (iMaxTrendUnitCount - 1 == iIndexNode)
				AxisNode.m_iCenterPixel = AxisNode.m_iEndPixel;
			else
				AxisNode.m_iCenterPixel = (AxisNode.m_iStartPixel + AxisNode.m_iEndPixel) / 2;

			iIndexNode++;

			//////////////////////////////////////////////////////////
			// �ָ���
			if (uiTime == TimeOpen.GetTime())	// ÿ��С���׶ε���ʼʱ��
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
			else if (uiTime == TimeClose.GetTime())	// �ý���������ʱ��
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
			else if (uiTime % (iFullTimeSeperator) == 0)		// һСʱ
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
			else if (uiTime % iHalfTimeSeperator == 0)		// ��Сʱ
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
		// ����
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

		// �ָ����������Ϊ���׼, ������ΪС��׼
		// ������䲻�࣬����ʹ��Сʱ����Ϊ�ָ��
		int32 iMaxAxisDivideSize = iMaxTrendUnitCount / (20);
		if (iMaxAxisDivideSize < 20)	iMaxAxisDivideSize = 20;
		aAxisDivide.SetSize(iMaxAxisDivideSize);
		bool32 bHourIsDivide = iMaxTrendIOCCount < 8; // ������ʱ��θ���С��8, ����뿪��nСʱ����Ϊһ���ָ��
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

				CTime Time1(uiTime);	// ���ǵ���ʱ����ʾ
				CTime Time2(TimeClose.GetTime());

				CString StrTimeOpen;
				CString StrTimeClose;

				StrTimeOpen.Format(L"%02d:%02d", Time1.GetHour(), Time1.GetMinute());
				StrTimeClose.Format(L"%02d:%02d", Time2.GetHour(), Time2.GetMinute());

				for (uiTime = TimeOpen.GetTime(); uiTime <= TimeClose.GetTime(); uiTime += 60)
				{
					CAxisNode &AxisNode = pAxisNode[iIndexNode];
					AxisNode.m_iTimeId = uiTime;		// ʱ��ڵ�

					/////////////////////////////////////////////////////////////////////////
					// �ڵ�
					CTime Time(uiTime);	// ���ǵ���ʱ����ʾ
					// ...fangz0815 ��ʱ�Ķ�. Ϊ��X �Ử����ʾ��ʱ��Ҫ��ʾ�·ݺ�����
					// AxisNode.m_StrSliderText1.Format(L"%02d/%02d %02d:%02d", Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute());
					AxisNode.m_StrSliderText1.Format(L"%02d:%02d", Time.GetHour(), Time.GetMinute());

					// �ڵ��� 
					if (0 == iIndexNode)
					{
						AxisNode.m_iStartPixel = Rect.left;
					}
					else
					{
						AxisNode.m_iStartPixel = pAxisNode[iIndexNode - 1].m_iEndPixel + 1;
					}

					// �ڵ���
					if (iMaxTrendUnitCount - 1 == iIndexNode)
					{
						AxisNode.m_iEndPixel = Rect.right;		// xl 0623 �����ұ���һ���߿��������ڲ��������
					}
					else
					{
						AxisNode.m_iEndPixel = Rect.left + (int32)((iIndexNode + 0.5) * fPixelWidthPerUnit + 0.5);
					}

					//
					if (AxisNode.m_iStartPixel > AxisNode.m_iEndPixel)
						AxisNode.m_iStartPixel = AxisNode.m_iEndPixel;

					// �нڵ�
					if (0 == iIndexNode)
						AxisNode.m_iCenterPixel = AxisNode.m_iStartPixel;
					else if (iMaxTrendUnitCount - 1 == iIndexNode)
						AxisNode.m_iCenterPixel = AxisNode.m_iEndPixel;
					else
						AxisNode.m_iCenterPixel = (AxisNode.m_iStartPixel + AxisNode.m_iEndPixel) / 2;

					iIndexNode++;

					if ( uiTime == TimeOpen.GetTime() )	// ���̵�
					{
						if ( iIndexOC == 0 )
						{
							// ����� - ����ʵ��, ��ʾ����
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
								pAxisDivide->m_DivideText1.m_StrText.Format(L"%02d/%02d  [ǰ%d ��]", curTmpTime.GetMonth(), curTmpTime.GetDay(), iDay);
							}

							if ( iDay == m_aTrendMultiDayIOCTimes.GetSize()-1 )	// ��һ��������
							{
								pAxisDivide->m_eLevelText		= CAxisDivide::EDLLevel1;
								pAxisDivide->m_DivideText1.m_uiTextAlign = DT_LEFT | DT_VCENTER | DT_SINGLELINE;
							}
							else	// �����������
							{				
								pAxisDivide->m_eLevelText		= CAxisDivide::EDLLevel2;
								pAxisDivide->m_DivideText1.m_uiTextAlign = DT_LEFT| DT_VCENTER | DT_SINGLELINE;
							}

							pAxisDivide->m_DivideText1.m_lTextColor = m_pRegionMain->GetAxisTextColor();
							pAxisDivide->m_DivideText2 = pAxisDivide->m_DivideText1;

							// 
							iIndexDivide++;
							pAxisDivide++;		// �������¸��ڵ�
						}
						else
						{
							// ����С���̵� - Сʱ����Ϊʵ����Сʱ��Ϊ��, ��ʾʱ��
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
						// ���뿪������, �飬��ʾʱ��
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
						// ���һ�������� ʵ����ʾʱ��
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
						pAxisDivide++;		// �������¸��ڵ�
					}
				}
			}
		}


		//  
		aAxisNodes.SetSize(iIndexNode);
		aAxisDivide.SetSize(iIndexDivide);
	}
	else	// ��ͨ״̬��
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
			Rect.left += (int32)fPixelWidthPerUnit;	// ���ƫ��һ��С�ո����һ�����̼�
		}

		int32 iHalfTimeSeperator = kiTimeSeperator;	// Ĭ�ϰ�Сʱ�ָ���
		if ( iMaxTrendUnitCount > kiSeperatorValve )
		{
			int i2Multi = (1+iMaxTrendUnitCount/kiSeperatorValve);
			i2Multi = i2Multi - i2Multi%2;
			iHalfTimeSeperator *= i2Multi;
		}
		int32 iFullTimeSeperator = iHalfTimeSeperator*2;	// ����ָ��

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

			CTime Time1(uiTime);	// ���ǵ���ʱ����ʾ
			CTime Time2(TimeClose.GetTime());

			CString StrTimeOpen;
			CString StrTimeClose;

			StrTimeOpen.Format(L"%02d:%02d", Time1.GetHour(), Time1.GetMinute());
			StrTimeClose.Format(L"%02d:%02d", Time2.GetHour(), Time2.GetMinute());

			for (uiTime = TimeOpen.GetTime(); uiTime <= TimeClose.GetTime(); uiTime += 60)
			{
				CAxisNode &AxisNode = pAxisNode[iIndexNode];
				CAxisDivide &AxisDivide = pAxisDivide[iIndexDivide];
				AxisNode.m_iTimeId = uiTime;		// ʱ��ڵ�

				/////////////////////////////////////////////////////////////////////////
				// �ڵ�
				CTime Time(uiTime);	// ���ǵ���ʱ����ʾ
				// ...fangz0815 ��ʱ�Ķ�. Ϊ��X �Ử����ʾ��ʱ��Ҫ��ʾ�·ݺ�����
				// AxisNode.m_StrSliderText1.Format(L"%02d/%02d %02d:%02d", Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute());
				AxisNode.m_StrSliderText1.Format(L"%02d:%02d", Time.GetHour(), Time.GetMinute());

				// �ڵ��� 
				if (0 == iIndexNode)
				{
					AxisNode.m_iStartPixel = Rect.left;
				}
				else
				{
					AxisNode.m_iStartPixel = pAxisNode[iIndexNode - 1].m_iEndPixel + 1;
				}

				// �ڵ���
				if (iMaxTrendUnitCount - 1 == iIndexNode)
				{
					AxisNode.m_iEndPixel = Rect.right;		// xl 0623 �����ұ���һ���߿��������ڲ��������
				}
				else
				{
					AxisNode.m_iEndPixel = Rect.left + (int32)((iIndexNode + 0.5) * fPixelWidthPerUnit + 0.5);
				}

				//
				if (AxisNode.m_iStartPixel > AxisNode.m_iEndPixel)
					AxisNode.m_iStartPixel = AxisNode.m_iEndPixel;

				// �нڵ�, ����ʾ���̼�����£��õ��ǲ��������
				if (0 == iIndexNode)
					AxisNode.m_iCenterPixel = AxisNode.m_iStartPixel;
				else if (iMaxTrendUnitCount - 1 == iIndexNode)
					AxisNode.m_iCenterPixel = AxisNode.m_iEndPixel;
				else
					AxisNode.m_iCenterPixel = (AxisNode.m_iStartPixel + AxisNode.m_iEndPixel) / 2;

				iIndexNode++;

				//////////////////////////////////////////////////////////
				// �ָ���
				if (uiTime == TimeOpen.GetTime())	// ÿ��С���׶ε���ʼʱ��
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
					if (0 == iIndexOC)	// �ý����տ���ʱ��
					{
						AxisDivide.m_eLevelText		= CAxisDivide::EDLLevel1;
						AxisDivide.m_DivideText1.m_uiTextAlign = DT_LEFT | DT_VCENTER | DT_SINGLELINE;
					}
					else	// һ��С�ڵĿ���ʱ��
					{				
						AxisDivide.m_eLevelText		= CAxisDivide::EDLLevel2;
						AxisDivide.m_DivideText1.m_uiTextAlign = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
					}

					AxisDivide.m_DivideText1.m_lTextColor = m_pRegionMain->GetAxisTextColor();
					AxisDivide.m_DivideText2 = AxisDivide.m_DivideText1;

					// 
					iIndexDivide++;
				}
				else if (uiTime == TimeClose.GetTime() && iIndexOC == TrendTradingDay.m_MarketIOCTime.m_aOCTimes.GetSize() - 2)	// �ý���������ʱ��
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
				else if (((int32)uiTime) % iFullTimeSeperator == 0)		// һСʱ
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
				else if (((int32)uiTime) % iHalfTimeSeperator == 0)		// ��Сʱ
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

	// ����Y����������ɶ��ٸ��̶�
	CRect Rect = m_pRegionMain->GetRectCurves();

	CFont* pOldFont = pDC->SelectObject(GetIoViewFontObject(ESFSmall));					
	int32 iHeightText = pDC->GetTextExtent(_T("����ֵ")).cy;
	pDC->SelectObject(pOldFont);

	// int32 iHeightText = pDC->GetTextExtent(_T("����ֵ")).cy;
	int32 iHeightSub = iHeightText + 12;
	int32 iNum = Rect.Height() / iHeightSub + 1;
	if ( iNum % 2 != 0 )	iNum--;	// ż��
	int32 iHalfNum = iNum / 2;

	// ��ȡ��ǰ��ʾС����λ
	int32 iSaveDec = 0;
	T_MerchNodeUserData* pData = NULL;
	if ( m_MerchParamArray.GetSize() > 0 )
	{
		pData = m_MerchParamArray.GetAt(0);
		CMerch* pMerch = pData->m_pMerchNode;
		if ( NULL != pMerch )
			iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;
	}

	// ��ȡ�������ϼ�����Ҫ��ֵ
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
			// ȡ���������Сֵ,��������ƽ��һ��:
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

				// ����
				AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;		// 

				if ( i == iHalfNum )
				{		
					AxisDivide.m_eLineStyle = CAxisDivide::ELSDoubleSolid;	// 2ʵ��
				}
				else
				{
					AxisDivide.m_eLineStyle = CAxisDivide::ELSSolid;		// ʵ��
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
			m_aYAxisDivideBkMain.RemoveAll();	// ��ձ���
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
			ASSERT( b );	// ��ת��Ӧ���ܳɹ��ģ���Ϊmin��max�Ѿ�����������
		}
	}

	{
		// 		float fBaseAxisY, fBase;
		// 		pCurveDependent->GetAxisBaseY(fBaseAxisY);
		// 		pCurveDependent->AxisYToPriceY(fBaseAxisY, fBase);
		// 		TRACE(_T("����������min max base: %f,%f %f\r\n"), fYMin, fYMax, fBase);
	}

	if (fabs(fPricePrevClose - fYMid) > 0.000001)
	{
		// //ASSERT(0);	// ��ֵӦ�þ������ռ�
		pCurveDependent->GetYMinMax(fYMin, fYMax);
	}

	// ���㸡�����̶ȳ���.
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

		// xl 0001762 ����YLeft��ʾ�ִ����� - ����һ���ո�
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
			StrPercent.TrimLeft(_T('-'));	// ��Ҫ-
		}

		// 
		CAxisDivide &AxisDivide = pAxisDivide[iCount];
		AxisDivide.m_iPosPixel	= y;

		// ����
		AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;		// 
		if (i == iHalfNum)
		{		
			AxisDivide.m_eLineStyle = CAxisDivide::ELSDoubleSolid;	// 2ʵ��
		}
		else
		{
			AxisDivide.m_eLineStyle = CAxisDivide::ELSSolid;		// ʵ��
		}

		// ����
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

	// ����Y����������ɶ��ٸ��̶�
	CRect Rect = pChartRegion->GetRectCurves();
	int32 iHeightText = pDC->GetTextExtent(_T("����ֵ")).cy;
	int32 iHeightSub  = iHeightText + 12;
	int32 iHeightHalf = iHeightSub/2;
	int32 iNum = Rect.Height()/iHeightSub + 1;

	// 	int32 iNum = Rect.Height() / iHeightSub + 1;
	// 	if ( iNum % 2 != 0 )	iNum--;	// ż��
	if (iNum > 3)	iNum = 3;
	if (iNum < 2)	iNum = 2;

	// ��ȡ��ǰ��ʾС����λ
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

	// ��ȡ�������ϼ�����Ҫ��ֵ
	CChartCurve *pCurveDependent = pChartRegion->GetDependentCurve();

	CPriceToAxisYObject AxisObj;
	bool32 bValidAxis = false;
	// �п��ܾ���ͼ��û�гɽ����ݣ������о�������
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
			// ȡ���������Сֵ,��������ƽ��һ��:
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

				// ����
				AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;		// 				
				AxisDivide.m_eLineStyle = CAxisDivide::ELSSolid;		// ʵ��

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

	// ���㸡�����̶ȳ���.
	float fHeightSub = (fYMax - fYMin) / iNum;
	int32 y;
	float fStart = fYMin;
	int32 iCount = 0;
	aYAxisDivide.SetSize(iNum + 1);
	CAxisDivide *pAxisDivide = (CAxisDivide *)aYAxisDivide.GetData();

	// �Ƿ�������������
	bool32 bTran = false;
	/*	
	if ( 0 == iSaveDec && 3 == iNum)
	{
	// ������,�ҷ�Χ����,�����´���:
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
	// ��֤��Сֵ�Ϸ�
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
	// ��֤���ֵ�Ϸ�
	aiYAxis[3] = iMax;
	}

	// ��ֵ:
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

	// ����
	AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;		// 
	AxisDivide.m_eLineStyle = CAxisDivide::ELSSolid;		// ʵ��

	// ����
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

			// 0001762 - �ɽ����ȴ�ָ�겻��Ҫ��ʾС���� > 1000 
			if ( fStart >= 1000 || fStart <= -1000  )
			{
				iSaveDec = 0;
			}

			// xl 0001762 ����YLeft��ʾ�ִ����� - ����һ���ո�
			//CString StrValue	= Float2String(fStart, iSaveDec, true, false, false);
			CString StrValue;
			Float2StringLimitMaxLength(StrValue, fStart, iSaveDec, GetYLeftShowCharWidth()-1, true, false, false, false);

			// 
			CAxisDivide &AxisDivide = pAxisDivide[iCount];
			AxisDivide.m_iPosPixel	= y;

			// ����
			AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;		// 
			AxisDivide.m_eLineStyle = CAxisDivide::ELSDot;			// ʵ��

			// ����
			AxisDivide.m_eLevelText						= CAxisDivide::EDLLevel1;

			AxisDivide.m_DivideText1.m_StrText			= StrValue;
			AxisDivide.m_DivideText2.m_StrText			= StrValue;

			if ( iNum == i && pChartRegion->GetTopSkip() < iHeightHalf )
			{
				// ���һ�����û��TopSkip�򶥲�����
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

	m_eMainCurveAxisYType = CPriceToAxisYObject::EAYT_Pecent;	// ��֧�ְٷֱ�

	// ����Y����������ɶ��ٸ��̶�
	float fMinYRange = 0.0f;
	if ( NULL != m_pMerchXml )
	{
		CRect Rect = m_pRegionMain->GetRectCurves();

		CClientDC dc(this);
		CFont* pOldFont = dc.SelectObject(GetIoViewFontObject(ESFSmall));					
		int32 iHeightText = dc.GetTextExtent(_T("����ֵ")).cy;
		dc.SelectObject(pOldFont);

		int32 iHeightSub = iHeightText + 12;
		int32 iDivideCount = Rect.Height() / iHeightSub + 1;
		if ( iDivideCount % 2 != 0 )	iDivideCount--;	// ż��
		iDivideCount /= 2;	// ȡһ��

		double iSaveDec = m_pMerchXml->m_MerchInfo.m_iSaveDec;
		ASSERT( iSaveDec >= 0 );
		//float fPricePrevClose = pMerchData->m_TrendTradingDayInfo.GetPrevReferPrice();	// ����
		float fPricePrevClose = GetTrendPrevClose();
		if ( iSaveDec <= 0 || fPricePrevClose >= 1000.0f )	// ���ո���1000���ڿ��ԭ�򽫲�����ʾȫС��λ
		{
			fMinYRange = iDivideCount;	// ��С��λ1
		}
		else
		{
			fMinYRange = iDivideCount * 1/(float)(pow(10.0, iSaveDec));		// 2Ӧ��Ϊ0.01
		}
	}

	// ������region������ֵ�� �Գ�����
	{
		if (NULL != m_pRegionMain)
		{
			bool32 bValidCurveYMinMax = false;
			float fCurveYMin = 0., fCurveYMax = 0.;

			// �ٷֱȼ���baseֵ
			// �������Сδȷ��ʱ����Ҫʹ��curve��PriceToAxis�ȼ��㷽��
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
				// �����������ߵ����±߽�ֵ
				for (int32 i = 0; i < m_pRegionMain->m_Curves.GetSize(); i++)
				{
					float fYMin = 0., fYMax = 0.;

					CChartCurve *pChartCurve = m_pRegionMain->m_Curves[i];
					pChartCurve->SetAxisYType(m_eMainCurveAxisYType);	// ���аٷֱ�����
					if ( CheckFlag(pChartCurve->m_iFlag, CChartCurve::KYTransformToAlignDependent) )
					{
						// ������Ʒ���Լ���
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
						continue;		// �����߲��������
					}

					if ( CheckFlag(pChartCurve->m_iFlag, CChartCurve::KInVisible) )
					{
						continue;	// ���ص��߲��������
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
					// �ǵ�ͣ����
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

				// ͬһ�������� ��/�� �۱仯�����, ��Ҫ����������� - ����ʷ��ʱ����Ҫ
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
					// ����С�̶��޶�
					float fPricePrevClose = fBase;	// ����
					float fAxisMaxY = fPricePrevClose + fMinYRange;
					float fAxisMinY = fPricePrevClose - fMinYRange;
					float fAxisPreClose;
					bool32 b = CPriceToAxisYObject::PriceYToAxisYByBase(m_eMainCurveAxisYType, fBase, fPricePrevClose, fAxisPreClose);
					b = CPriceToAxisYObject::PriceYToAxisYByBase(m_eMainCurveAxisYType, fBase, fAxisMaxY, fAxisMaxY) && b;
					b = CPriceToAxisYObject::PriceYToAxisYByBase(m_eMainCurveAxisYType, fBase, fAxisMinY, fAxisMinY) && b;
					ASSERT( b );
					if ( bValidCurveYMinMax )
					{
						// �Ƚ��Ƿ����С�̶�С��С��ȡ��С�̶�
						fCurveYMax = max(fCurveYMax, fAxisMaxY);
						fCurveYMin = min(fCurveYMin, fAxisMinY);
					}
					else
					{
						// û����Чֵ��������С�޶�ֵ��Ϊ��Чֵ, ��������Ϊ���߻���
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
				// �����ռ���Ϊ��ֵ�� ���¶ԳƷ�
				if (bValidCurveYMinMax)
				{
					//float fPricePrevClose = pMerchData->m_TrendTradingDayInfo.GetPrevReferPrice();	// ����
					float fPricePrevClose = fBase;	// ����

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

			// �������е���region�е�curve��������ֵ
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

	// ���������ͼ�µ�curveȡֵ
	if (!bOnlyUpdateMainRegion)
	{
		// ������ͼ
		for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
		{
			CChartRegion *pSubRegionMain = m_SubRegions[i].m_pSubRegionMain;
			const bool32 bVolSub = 0 == i;

			if (NULL != pSubRegionMain)
			{
				bool32 bValidCurveYMinMax = false;
				float fCurveYMin = 0., fCurveYMax = 0.;

				// �����������ߵ����±߽�ֵ
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
					// ����
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

				// �������е���region�е�curve��������ֵ
				for (int32 m = 0; m < pSubRegionMain->m_Curves.GetSize(); m++)
				{
					CChartCurve *pChartCurve = pSubRegionMain->m_Curves[m];
					pChartCurve->SetYMinMax(fCurveYMin, fCurveYMax, bValidCurveYMinMax);

					//TRACE(_T("��ʱ�����С: %d %f-%f\n"), bValidCurveYMinMax, fCurveYMin, fCurveYMax);
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
		// ȡ��right - Ӧ����ȫ�� - ��ռʱ����right����ûʵ��
		// 		int32 iRightSkip = m_pRegionMain->GetRightSkip();
		// 		if ( iRightSkip < 1 )
		// 		{
		// 			m_pRegionMain->SetRightSkip(1);
		// 		}
	}

	// ���ٳɽ�����ͼ��topSkip
	if ( m_SubRegions.GetSize() > 0 )
	{
		T_SubRegionParam &SubVol = m_SubRegions[0];
		SubVol.m_pSubRegionYLeft->SetTopSkip(20);		// ��һ����϶��ĸ�Ӧ������
		SubVol.m_pSubRegionYRight->SetTopSkip(20);
		SubVol.m_pSubRegionMain->SetTopSkip(20);
	}

	int32 iLeftSkip = 0;
	if ( ETBT_CompetePrice == GetCurTrendBtnType()
		&& NULL != m_pMerchXml )
	{
		// ����ͼ ���� ͼҪ�Ƴ����ֿռ�
		iLeftSkip = 100;	// �̶�����
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
			// ��ʷ��ʱ�İ볡������
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
	//ASSERT( iSize == pRegion->m_aXAxisNodes.GetSize() );		// �п���û������!
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

		// ����Ʒ��������գ����ӵĲ�����
		bool32 bIsMain = pData->bMainMerch;	// ����Ʒ

		// 
		CMerch* pMerch = pData->m_pMerchNode;
		if ( NULL == pMerch )
		{
			return;
		}

		ASSERT(pData->m_TrendTradingDayInfo.m_bInit);

		ASSERT( m_iTrendDayCount <= TREND_MAX_DAY_COUNT && m_iTrendDayCount >= TREND_MIN_DAY_COUNT );
		// ���ͷ���������
		// ����Ƕ��շ�ʱ����Ҫ�ȵ���k�߻�������ȷ��Ҫ�������ʲôʱ��εķ���k��
		CGmtTime TimeStart, TimeEnd;
		CGmtTime TimeLastClose;
		if ( bIsMain )
		{
			// ����Ʒ�ж���ļ�¼
			// ��Ѱ����Ʒ��k������, ��������ʵ��k����������������
			m_aTrendMultiDayIOCTimes.RemoveAll();	// �����ǰ������
			m_aTrendMultiDayIOCTimes.Add(pData->m_TrendTradingDayInfo.m_MarketIOCTime);	// ����������һ���

			if ( m_iTrendDayCount > 1 )
			{
				// ����Ʒ�Ҷ���ͼ����Ҫ���������
				TimeStart = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time;
				TimeEnd   = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeEnd.m_Time;
				TimeLastClose = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeClose.m_Time;

				int32 iPosFound;
				CMerchKLineNode* pKLineDayRequest = NULL;
				pMerch->FindMerchKLineNode(EKTBDay, iPosFound, pKLineDayRequest);
				if (NULL != pKLineDayRequest)	// �ҵ���Ӧ����
				{
					// ��Ѱ��k�ߣ�������
					//int32 iPos = CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(pKLineDayRequest->m_KLines, TimeEnd);
					// ��K���Կ���ʱ��Ϊ��ĵ�λ
					int32 iPos = CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(pKLineDayRequest->m_KLines, TimeLastClose);
					if ( iPos >= 0 && iPos < pKLineDayRequest->m_KLines.GetSize() )
					{
						// �п��ܳ��ָ�����û�е��յ���K������(�ڳ�ʼ��ʱ��ʱ), ����iPos����һ���ǵ�������
						// �ж������k�߿�����, ������Ҫ��������k��
						int32 iEndPos = iPos-1;
						CGmtTime TimeToday(TimeLastClose);
						SaveDay(TimeToday);
						if ( pKLineDayRequest->m_KLines[iPos].m_TimeCurrent < TimeToday )
						{
							++iEndPos;	// posָ��ĵ㲻�ǵ��յ�, Ҫ�����õ�
						}
						if ( iEndPos >= 0 )
						{
							int32 iStartPos = (iEndPos+1) - (m_iTrendDayCount-1);	// (��ʼ�����)(���յ㲻����)
							iStartPos = max(iStartPos, 0);	// ���û����ô�࣬�;������
							for ( int32 iIoc = iEndPos ; iIoc >= iStartPos ; iIoc-- )	// ���еĶ��ӽ�ȥ��������k��������������
							{
								CGmtTime TimeKLineStart = pKLineDayRequest->m_KLines[iIoc].m_TimeCurrent;		// �����ʱ��ȡ�ĸ���ȷ��
								CMarketIOCTimeInfo MIOCTime;
								if ( pData->m_pMerchNode->m_Market.GetSpecialTradingDayTime(TimeKLineStart, MIOCTime, pData->m_pMerchNode->m_MerchInfo) )
								{
									TimeStart = MIOCTime.m_TimeInit.m_Time;	// ȡ�����k�ߵ�ֵ
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
							ASSERT( m_aTrendMultiDayIOCTimes.GetSize() > 1 ); // ����������һ��
							UpdateTrendMultiDayChange();	// �������
						}
					}
				}
			}
			else 
			{
				// ������ָ�����ڵ�(�߼��ϵĵ���)
				TimeStart = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time;
				TimeEnd   = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeEnd.m_Time;
			}
		}
		else
		{
			// ������Ʒ, ������Ʒ������϶����ں����, ������Ʒ���е��������ݶ���������ƷΪ׼���ж����������
			if ( !GetTrendStartEndTime(TimeStart, TimeEnd) )
			{
				// ������ָ�����ڵ�(�߼��ϵĵ���), ����Ʒʧ�ܣ��ǾͿ��Բ���������
				TimeStart = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time;
				TimeEnd   = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeEnd.m_Time;

				return;	// ��������
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
	// �����������ǰ���͵ĸ����Ҫ��Ķ���k������
	// ���������k�߻���û��ǰ�淢�͵Ķ࣬�򲻷�������
	if ( m_MerchParamArray.GetSize() <= 0 )
	{
		ASSERT( 0 );
		return;
	}

	if ( m_iTrendDayCount == m_aTrendMultiDayIOCTimes.GetSize() )
	{
		return;	// �Ѿ�������������
	}
	// �п������л���Ʒʱֱ�ӵ���update�����Ǵ�ʱ��û��requestdata��ʼ���ģ���������Ҳ��Ӧ����������
	ASSERT( m_aTrendMultiDayIOCTimes.GetSize() < m_iTrendDayCount );

	T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);	// ����������Ʒ
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
	// ���ͷ���������
	// ����Ƕ��շ�ʱ����Ҫ�ȵ���k�߻�������ȷ��Ҫ�������ʲôʱ��εķ���k��
	{
		// ��Ѱ����Ʒ��k������, ��������ʵ��k����������������
		CGmtTime TimeStart, TimeEnd;
		CGmtTime TimeLastClose;
		if ( m_iTrendDayCount > 1 )		// �Ƕ��첻��Ҫ���Ⳣ������
		{
			// ����Ʒ�Ҷ���ͼ����Ҫ���������
			TimeStart = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time;
			TimeEnd   = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeEnd.m_Time;
			TimeLastClose = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeClose.m_Time;

			int32 iPosFound;
			CMerchKLineNode* pKLineDayRequest = NULL;
			pMerch->FindMerchKLineNode(EKTBDay, iPosFound, pKLineDayRequest);
			if (NULL != pKLineDayRequest)	// �ҵ���Ӧ����
			{
				//const int32 iPos = CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(pKLineDayRequest->m_KLines, TimeEnd);
				// ��K���Կ���ʱ��Ϊ��ĵ�λ
				const	int32 iPos = CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(pKLineDayRequest->m_KLines, TimeLastClose);
				if ( iPos >= 0 && iPos < pKLineDayRequest->m_KLines.GetSize() )
				{
					int32 iEndPos = iPos-1;
					CGmtTime TimeToday(TimeLastClose);
					SaveDay(TimeToday);
					if ( pKLineDayRequest->m_KLines[iPos].m_TimeCurrent < TimeToday )
					{
						++iEndPos;	// posָ��ĵ㲻�ǵ��յ�, Ҫ�����õ�
					}
					if ( iEndPos >= 0 )
					{
						int32 iStartPos = (iEndPos+1) - (m_iTrendDayCount-1);	// (��ʼ�����)(���յ㲻����)
						ASSERT( iStartPos <= iEndPos );
						iStartPos = min(iEndPos, iStartPos);
						//int32 iStartPos = iPos - (m_iTrendDayCount - 1);
						iStartPos = max(iStartPos, 0);	// ���û����ô�࣬�;������
						int32 iOldPos = iEndPos+1 - (m_aTrendMultiDayIOCTimes.GetSize() -1);
						ASSERT( iOldPos <= iEndPos+1 );
						if ( iStartPos < iOldPos )
						{
							// ����ǰû�з���������յķ���k��
							int32 iIoc = iOldPos-1;
							ASSERT( iIoc <= iEndPos );
							iIoc = min(iIoc, iEndPos);
							for ( ; iIoc >= iStartPos ; iIoc-- )
							{
								CGmtTime TimeKLineStart = pKLineDayRequest->m_KLines[iIoc].m_TimeCurrent;
								CMarketIOCTimeInfo MIOCTime;
								if ( pData->m_pMerchNode->m_Market.GetSpecialTradingDayTime(TimeKLineStart, MIOCTime, pData->m_pMerchNode->m_MerchInfo) )
								{
									TimeStart = MIOCTime.m_TimeInit.m_Time;	// ȡ�����k�ߵ�ֵ
									// ������û�з���, ��k�������Ѿ������ˣ��´ε���requestviewdataʱ���ҵ����ʵ�
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
							UpdateTrendMultiDayChange();	// �������

							// ������ƷҲ������һ������
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
		m_bSingleSectionTrend = false;	// ȡ���볡���Ʊ�־
		if ( ETBT_CompetePrice == GetCurTrendBtnType()
			&& iDay != 1 )
		{
			// ȡ������ͼ
			m_iBtnCur = 0;
			SendMessage(WM_SETREDRAW, (WPARAM)FALSE, 0);
			OnTrendBtnSelChanged();			// ע�ⲻҪ�ݹ����
			ASSERT( ETBT_CompetePrice != GetCurTrendBtnType() );	// ע��˴���д����ȡλ��0
			SendMessage(WM_SETREDRAW, (WPARAM)TRUE, 0);
		}

		if ( bClearData )
		{
			// ���ԭ������������
			m_aTrendMultiDayIOCTimes.RemoveAll();
			if ( m_MerchParamArray.GetSize() > 0 
				&& m_MerchParamArray[0] != NULL
				&& m_MerchParamArray[0]->m_TrendTradingDayInfo.m_bInit )
			{
				m_aTrendMultiDayIOCTimes.Add( m_MerchParamArray[0]->m_TrendTradingDayInfo.m_MarketIOCTime );	// ������һ��Ĭ�ϵ�
			}
		}
		else
		{
			if ( m_aTrendMultiDayIOCTimes.GetSize() > iDay )
			{
				m_aTrendMultiDayIOCTimes.SetSize(iDay);	// �ض�һ����
			}
			else if ( m_aTrendMultiDayIOCTimes.GetSize() <= 0 )
			{
				// ���볢�Ը�һ��Ĭ�ϵ�
				if ( m_MerchParamArray.GetSize() > 0 
					&& m_MerchParamArray[0] != NULL
					&& m_MerchParamArray[0]->m_TrendTradingDayInfo.m_bInit )
				{
					m_aTrendMultiDayIOCTimes.Add( m_MerchParamArray[0]->m_TrendTradingDayInfo.m_MarketIOCTime );	// ������һ��Ĭ�ϵ�
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
		ASSERT( 0 );	// ������
	}
}

void CIoViewTrend::UpdateTrendMultiDayChange()
{
	bool32 bValidMerch = NULL != m_pRegionMain && NULL != m_pMerchXml;
	if ( m_bSingleSectionTrend && bValidMerch )
	{
		// �볡���Ʊ���
		CGmtTime TimeOpen, TimeClose;
		if ( GetTrendSingleSectionOCTime(TimeOpen, TimeClose) )
		{
			CTime tOpen(TimeOpen.GetTime());
			CTime tClose(TimeClose.GetTime());
			CString StrName;
			StrName.Format(_T("%s ��ʱ��� %02d:%02d--%02d:%02d��"), 
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
		// ���շ�ʱ���� ��Ʒ[���n�շ�ʱ] , ��ʱ��Ʒ�ȱز�ΪNULL
		CString StrName;
		StrName.Format(_T("%s�����%d�շ�ʱ��"), m_pMerchXml->m_MerchInfo.m_StrMerchCnName.GetBuffer(), m_aTrendMultiDayIOCTimes.GetSize());

		if (m_bShowTopToolBar)
		{
			//
			CString StrCaption;
			StrCaption.Format(L"%d �շ�ʱ", m_aTrendMultiDayIOCTimes.GetSize());
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
			//m_mapTrendRightBtn[TOP_TREND_MORE_TREND].SetCaption(L"���շ�ʱ");
		}
		/*else*/
		{
			m_pRegionMain->SetTitle(m_pMerchXml->m_MerchInfo.m_StrMerchCnName);
		}
	}
}

void CIoViewTrend::SetSingleSection( bool32 bSingle )
{
	// ��ǰʱ�����ڵĿ�����ʱ��, init-��һ���������ڵ�һ������
	//		���һ���ճ���Endʱ���������һ��������
	// �������ڹ�Ʊ&ָ���ṩ
	bool32 bOld = m_bSingleSectionTrend;
	if ( bSingle && IsEnableSingleSection() )
	{
		// Ҫ�����ð볡���ƣ�ȡ��������ģʽ

		// ȡ�����շ�ʱ, ע��Ҫ��ȡ�����շ�ʱ, �����ñ�־
		SetTrendMultiDay(1, true, false);

		m_bSingleSectionTrend = true;
	}
	else if ( bSingle )
	{
		// Ҫ�����ð볡���ƣ��������ڵ�ģʽ������
		ASSERT( 0 );
		m_bSingleSectionTrend = false;
	}
	else
	{
		// ȡ���볡����
		m_bSingleSectionTrend = false;
	}

	if ( (bOld && !m_bSingleSectionTrend)
		|| (!bOld && m_bSingleSectionTrend) )
	{
		// �����

		UpdateTrendMultiDayChange();
		OnVDataMerchKLineUpdate(m_pMerchXml);	// K�����ݱ��
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
		ASSERT( 0 );	// ��ʷ��ʱ�£�������
		//return false;
	}

	//if ( pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_aOCTimes.GetSize() != 4 )
	//{
	//	// ���ܷ�Ϊ�ϡ��°볡
	//	return false;
	//}

	//if ( m_aTrendMultiDayIOCTimes.GetSize() > 1 )
	//{
	//	// �ִ��ڶ��շ�ʱ��
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
		// �ǰ볡����
		TimeOpen = m_MerchParamArray[0]->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeOpen.m_Time;
		TimeClose   = m_MerchParamArray[0]->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeClose.m_Time;
		// ����ȡ��һ���
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
		// ����ǰʱ�������ĸ�ʱ���
		CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();
		const CMarketIOCTimeInfo &IOCTime = m_MerchParamArray[0]->m_TrendTradingDayInfo.m_MarketIOCTime;

		// �п��ܽ���ǿ����գ���ioctimeʵ����Ϊ�ϸ���������Ϣ
		for ( int32 i=0; i < IOCTime.m_aOCTimes.GetSize() ; i+=2 )
		{
			CGmtTime TimeOpenInner;
			CGmtTime TimeCloseInner;

			if ( 0 == i )
			{
				// ��һ�����̵�, ȡ��һ������ʱ��
				TimeOpenInner = IOCTime.m_TimeOpen.m_Time;
			}
			else
			{
				TimeOpenInner = IOCTime.m_aOCTimes[i];
			}

			if ( i >= IOCTime.m_aOCTimes.GetSize() - 2 )
			{
				// ���һ������ ��������ʣ��ʱ��
				TimeCloseInner = IOCTime.m_TimeEnd.m_Time;
				if ( (TimeNow >= TimeOpenInner && TimeNow <= TimeCloseInner)
					|| TimeNow > TimeCloseInner )		// �ǿ�������Ϣ���䵽���һ�������յ����̶�
				{
					TimeOpen = TimeOpenInner;
					TimeClose   = IOCTime.m_TimeClose.m_Time;	// ����ʱ��
					return true;	// ȡ���������ʱ����
				}
			}
			else
			{
				// �м�Ŀ��գ�����ʱ��ȡ����һ�����̵�
				TimeCloseInner = IOCTime.m_aOCTimes[i+2];
				if ( TimeNow >= TimeOpenInner && TimeNow < TimeCloseInner )	// ��һ�����̵㲻����
				{
					TimeOpen = TimeOpenInner;
					TimeClose   = TimeCloseInner;
					return true;	// ȡ���������ʱ����
				}
			}

		}
	}

	ASSERT( 0 );
	return false;	// ��Ӧ�õ��õ�
}

bool32 CIoViewTrend::GetTrendStartEndTime( OUT CGmtTime &TimeStart, OUT CGmtTime &TimeEnd )
{
	bool32 bValidTrend = m_MerchParamArray.GetSize() > 0 
		&& m_MerchParamArray[0] != NULL
		&& m_MerchParamArray[0]->m_TrendTradingDayInfo.m_bInit;

	if ( !m_bSingleSectionTrend 
		&& bValidTrend )
	{
		// �ǰ볡����
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
		// ����ǰʱ�������ĸ�ʱ���
		CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();
		const CMarketIOCTimeInfo &IOCTime = m_MerchParamArray[0]->m_TrendTradingDayInfo.m_MarketIOCTime;

		// �п��ܽ���ǿ����գ���ioctimeʵ����Ϊ�ϸ���������Ϣ
		for ( int32 i=0; i < IOCTime.m_aOCTimes.GetSize() ; i+=2 )
		{
			CGmtTime TimeOpenInner;
			CGmtTime TimeCloseInner;

			if ( 0 == i )
			{
				// ��һ�����̵�, ȡ�г���ʼ��ʱ��
				TimeOpenInner = IOCTime.m_TimeInit.m_Time;
			}
			else
			{
				TimeOpenInner = IOCTime.m_aOCTimes[i];
			}

			if ( i >= IOCTime.m_aOCTimes.GetSize() - 2 )
			{
				// ���һ������ ��������ʣ��ʱ��
				TimeCloseInner = IOCTime.m_TimeEnd.m_Time;
				if ( (TimeNow >= TimeOpenInner && TimeNow <= TimeCloseInner)
					|| TimeNow > TimeCloseInner )		// �ǿ�������Ϣ���䵽���һ�������յ����̶�
				{
					TimeStart = TimeOpenInner;
					TimeEnd   = TimeCloseInner;
					return true;	// ȡ���������ʱ����
				}
			}
			else
			{
				// �м�Ŀ��գ�����ʱ��ȡ����һ�����̵�
				TimeCloseInner = IOCTime.m_aOCTimes[i+2];
				if ( TimeNow >= TimeOpenInner && TimeNow < TimeCloseInner )	// ��һ�����̵㲻����
				{
					TimeStart = TimeOpenInner;
					TimeEnd   = TimeCloseInner;
					return true;	// ȡ���������ʱ����
				}
			}

		}
	}

	ASSERT( 0 );
	return false;	// ��Ӧ�õ��õ�
}

bool32 CIoViewTrend::GetTrendSingleSectionOCTime( OUT CGmtTime &TimeOpen, OUT CGmtTime &TimeClose )
{
	bool32 bValidTrend = m_MerchParamArray.GetSize() > 0 
		&& m_MerchParamArray[0] != NULL
		&& m_MerchParamArray[0]->m_TrendTradingDayInfo.m_bInit;
	if ( m_bSingleSectionTrend && bValidTrend )
	{
		// ����ǰʱ�������ĸ�ʱ���
		CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();
		const CMarketIOCTimeInfo &IOCTime = m_MerchParamArray[0]->m_TrendTradingDayInfo.m_MarketIOCTime;

		for ( int32 i=0; i < IOCTime.m_aOCTimes.GetSize() ; i+=2 )
		{
			CGmtTime TimeOpenInner;
			CGmtTime TimeCloseInner;

			if ( 0 == i )
			{
				// ��һ�����̵�, ȡ�г���ʼ��ʱ��
				TimeOpenInner = IOCTime.m_TimeInit.m_Time;
			}
			else
			{
				TimeOpenInner = IOCTime.m_aOCTimes[i];
			}

			if ( i >= IOCTime.m_aOCTimes.GetSize() - 2 )
			{
				// ���һ������ ��������ʣ��ʱ��
				TimeCloseInner = IOCTime.m_TimeEnd.m_Time;
				if ( TimeNow >= TimeOpenInner && TimeNow <= TimeCloseInner
					|| TimeNow > TimeCloseInner )
				{
					TimeOpen = IOCTime.m_aOCTimes[i];
					TimeClose = IOCTime.m_aOCTimes[i+1];
					return true;	// ȡ���������ʱ����
				}
			}
			else
			{
				// �м�Ŀ��գ�����ʱ��ȡ����һ�����̵�
				TimeCloseInner = IOCTime.m_aOCTimes[i+2];
				if ( TimeNow >= TimeOpenInner && TimeNow < TimeCloseInner )	// ��һ�����̵㲻����
				{
					TimeOpen = IOCTime.m_aOCTimes[i];
					TimeClose = IOCTime.m_aOCTimes[i+1];
					return true;	// ȡ���������ʱ����
				}
			}

		}
	}

	return false;
}

//lint --e{429}
bool32 CIoViewTrend::AddCmpMerch( CMerch *pMerchToAdd, bool32 bPrompt, bool32 bReqData )
{
	// ����Ƿ��Ѵ���
	ASSERT( m_MerchParamArray.GetSize() > 0 );
	ASSERT( m_pMerchXml != NULL );
	if ( m_pMerchXml == NULL || m_MerchParamArray.GetSize() <= 0 )
	{
		return false;
	}

	// ��ʱͼֻ�аٷֱ�����
	ASSERT( GetMainCurveAxisYType() == CPriceToAxisYObject::EAYT_Pecent );


	for ( int32 i=0; i < m_MerchParamArray.GetSize() ; i++ )
	{
		if ( m_MerchParamArray[i]->m_pMerchNode == pMerchToAdd )
		{
			if ( bPrompt )
			{
				MessageBox(_T("����Ʒ���Ѿ�����"), _T("��ʾ"));
			}
			return false;
		}
	}

	if ( bPrompt )
	{
		// ��鲢ѯ���Ƿ�Ҫ�������ٷֱ�����
	}

	CArray<COLORREF, COLORREF> aClrs;
	aClrs.SetSize(3); 
	aClrs[2] = RGB(127,127,127);
	aClrs[1] = RGB(128,64,0);
	aClrs[0] = RGB(0,150, 84);
	if ( m_MerchParamArray.GetSize() >= 4 )
	{
		// �������ֵ��1+3
		if ( bPrompt )
		{
			MessageBox(_T("�Ѿ������������������Ʒ��!"), _T("������ʾ"));
		}
		return false;
	}

	COLORREF clrLine = aClrs[ (m_MerchParamArray.GetSize()-1)%aClrs.GetSize() ];

	// ������Ʒ�б������ע�����������Ʒ���ݣ��Ƿ���Ҫ��Ȩ
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

				// zhangbo 20090708 #�����䣬 �������
				//...
			}
		}
		else
		{
			// ��ʷ��ʱ - �趨��������Ϣ(��ʷ��ʱ�ϸ������ϲ����������Ʒ)
			if ( pMerchToAdd->m_Market.GetSpecialTradingDayTime(m_TimeHistory, RecentTradingDay, pMerchToAdd->m_MerchInfo) )
			{
				pData->m_TrendTradingDayInfo.Set(pMerchToAdd, RecentTradingDay);
				pData->m_TrendTradingDayInfo.RecalcPrice(*pMerchToAdd);
				pData->m_TrendTradingDayInfo.RecalcHold(*pMerchToAdd);
			}
		}
		if ( !bInherit )
		{
			// ������Ҫ���� ����
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

		// �����ע
		CSmartAttendMerch AttendMerch;
		AttendMerch.m_pMerch = pMerchToAdd;
		AttendMerch.m_iDataServiceTypes = m_iDataServiceType;
		m_aSmartAttendMerchs.Add(AttendMerch);

		if ( bReqData )
		{
			RequestSingleCmpMerchViewData(pMerchToAdd);
		}

		return true;

		// ��ʱ����û�����ñ����
	}

	return false;
}

void CIoViewTrend::RemoveCmpMerch( CMerch *pMerch )
{
	// �Ӷ�����ɾ�����ӹ�ע��ɾ��
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
	// ɾ�����е��ӵ���Ʒ
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
			ASSERT( !pThisData->bMainMerch );	// ��������Ƹ�������Ʒ��һ�����������õ�
			break;
		}
	}

	if ( NULL == pThisData )
	{
		ASSERT( 0 );
		return;
	}

	//�ȷ�RealtimePrice����
	{
		CMmiReqRealtimePrice Req;
		Req.m_iMarketId			= pMerch->m_MerchInfo.m_iMarketId;
		Req.m_StrMerchCode		= pMerch->m_MerchInfo.m_StrMerchCode;
		DoRequestViewData(Req);
	}

	// һ������ʱ��������ƷΪ׼
	{
		ASSERT(pMainData->m_TrendTradingDayInfo.m_bInit);
		ASSERT(pThisData->m_TrendTradingDayInfo.m_bInit);

		// ʱ�����������Ϊ׼, �����������ڵ�������Ҳû��
		// Ҫ�ǵ��ӵ���ͼ�������ݲ���ͬ����ʱ�䣬��ʹ�д�����ʾ��Ӧ��Ҳ����Ҫ����ʱ�Ȳ�����
		// ������ͽ������k������
		if ( m_iTrendDayCount <= TREND_MIN_DAY_COUNT-1 )
		{
			ASSERT( 0 );
			m_iTrendDayCount = TREND_MIN_DAY_COUNT;
		}
		ASSERT( m_iTrendDayCount <= TREND_MAX_DAY_COUNT );

		// ����
		CGmtTime TimeDay = pMainData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeClose.m_Time;
		SaveDay(TimeDay);

		CMmiReqMerchKLine info;
		info.m_eKLineTypeBase	= EKTBDay;
		info.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
		info.m_StrMerchCode		= pMerch->m_MerchInfo.m_StrMerchCode;
		info.m_eReqTimeType		= ERTYFrontCount;
		info.m_TimeStart		= TimeDay;
		info.m_iFrontCount			= 1 + m_iTrendDayCount;			// ����n+1 !!�����Ӱ�쵽��ʷ��ʱ��
		DoRequestViewData(info);
	}

	// ���ͷ���������
	// ����Ƕ��շ�ʱ����Ҫ�ȵ���k�߻�������ȷ��Ҫ�������ʲôʱ��εķ���k��
	{
		RequestMinuteKLineData(*pThisData);	// �������k��
	}
}

bool32 CIoViewTrend::CalcKLinePriceBaseValue( CChartCurve *pCurve, OUT float &fPriceBase )
{
	// �����������ʹ�������Լ��Ļ���ֵ��ָ����ʹ����������ͬ�Ļ���ֵ
	fPriceBase = CChartCurve::GetInvalidPriceBaseYValue();
	if ( NULL == pCurve )
	{
		return false;
	}
	// ʹ����ͼ���������ͣ��������ߵ��������ͼ���
	CPriceToAxisYObject::E_AxisYType eType = GetMainCurveAxisYType();
	if ( CPriceToAxisYObject::EAYT_Pecent == eType )
	{
		// �ٷֱȣ���Ʒ�����ռۻ��������
		CNodeData node;
		// �����ָ���ߣ��������ߵ�
		if ( CheckFlag(pCurve->m_iFlag, CChartCurve::KTypeIndex) )
		{
			ASSERT( 0 );	// ��Ӧ���ظ������
			if ( NULL == m_pRegionMain || (pCurve=m_pRegionMain->GetDependentCurve()) == NULL )
			{
				return false;
			}
		}

		// �ҵ����curve��Ӧ��merchdData
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
		ASSERT( 0 );	// ��֧�������κ�����
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
		ASSERT( pData->bMainMerch );	// ��ͼ����
		// ��������Ʒ�ĸ��¶��мӽ���ʱ������
		// ��ʾʱ����Ѿ����������������ʱ���µ���K��
		// ���е�����Ʒ������
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
		// �ǹ��ڹ�Ʊ&ָ�����ʺ��ǵ�ͣ 
		return false;
	}

	/*
	�ǵ�ͣ���㣺
	1�����еĹ�Ʊ+ָ��+����  
	��ͣ�ۣ� ����+����*10%�����һλ�������룩
	��ͣ�ۣ� ����-����*10%�����һλ�������룩
	2�����Ƶ�һ����ĸ��"N "
	�������ǵ�ͣ
	3������ǰ��λ��XR��XD��DR���Ĺ�Ʊ�ǳ�Ȩ��Ϣ�ģ������̱����ñ��۱��е������̼��㣬������K��ͼ�е����ռ���
	��ͣ�ۣ� ����+����*10%�����һλ�������룩
	��ͣ�ۣ� ����-����*10%�����һλ�������룩
	4�����ư�����ST��(ST��*ST�� SST��S*ST)
	��ͣ�ۣ� ����+����*5%�����һλ�������룩
	��ͣ�ۣ� ����-����*5%�����һλ�������룩
	*/

	float fRate		= 0.1f;
	float fClosePre	= m_pMerchXml->m_pRealtimePrice->m_fPricePrevClose;

	//
	if ( 'N' == m_pMerchXml->m_MerchInfo.m_StrMerchCnName[0] )
	{
		// �¹� ���ü����ǵ�ͣ, ���Ϊ���ǵ�ͣ
		return false;
	}


	//
	if ( -1 != m_pMerchXml->m_MerchInfo.m_StrMerchCnName.Find(L"ST") )
	{
		fRate = 0.05f;
	}

	// 
	float fTmp = fClosePre * fRate;

	// ��������, ������λС��
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

		// ����Ԥ��Ϊ��ťԤ��
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

	// �����Լ���һ����ť
	DrawBottomBtns();
}

//lint --e{429}
bool32 CIoViewTrend::InitRedGreenCurve()
{
	if ( NULL == m_pRedGreenCurve && m_MerchParamArray.GetSize() > 0 && m_MerchParamArray[0] != NULL )
	{
		// �½�һ��������״�ߣ�ֻ��ָ��ʱ�Ÿ�ֵ������ʱ�򲻸�ֵ, ����ֵ���̼�
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

	CalcNoWeightExpLine();	// ���㲻��Ȩ��, һ��

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
		return;	// ��ʷ����ʾ����
	}

	CalcLayoutRegions(true);	// �˴��������ͼ�����Сֵ����

	ASSERT( GetMainCurveAxisYType() == CPriceToAxisYObject::EAYT_Pecent );	// ���ǰٷֱ�����

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
		// ָ�����������ݼ���
		const float fMax = fBaseAxisY + (fMaxAxisY-fBaseAxisY)/3;	// 1/3���߶�
		//const float fMin = fBaseAxisY - (fBaseAxisY-fMinAxisY)/3;

		//CMarket &Market = pData->m_pMerchNode->m_Market;

		// �����ֵӳ�䵽���귶Χ��
		const int32 iSize = pGeneralNormals->GetSize();
		pNodes->GetNodes().SetSize(iSize);
		CNodeData *pNodeDatas = pNodes->GetNodes().GetData();
		const T_GeneralNormal *pSrcData = pGeneralNormals->GetData();
		float fCurveMax = -FLT_MAX;
		float fCurveMin = FLT_MAX;
		int32 i = 0;
		for ( i=0; i < iSize ; i++ )
		{
			// ȫ������ֵ��close��
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

		// �ض� - �ض�Դ����
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

		// ӳ��
		if ( fCurveMax >= fCurveMin )
		{
			// �Գ�
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
				float fBasePrice;		// ͼ���еĻ����۸�
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
				// ȫ������һ��ֱ��- -
				// ASSERT( 0 );
				float fBasePrice;
				m_pRedGreenCurve->AxisYToPriceY(fBaseAxisY, fBasePrice);
				for ( int32 i=0; i < iSize ; i++ )
				{
					pNodeDatas[i].m_fClose = fBasePrice;	// ȫ�����ǻ����۸�
				}
			}

			// �ض�
			TrimPlusShowNodeData(pNodes);
		}
		else
		{
			// û����Чֵ, ���
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
			m_iBtnCur = iBtnIndex;	// ʹ�ô洢��
		}
	}
	if ( eMerchType != ERTStockCn
		&& eMerchType != ERTExp
		&& eMerchType != ERTFuturesCn )
	{
		eMerchType = ERTStockCn;	// û����ȷָ�������ͣ�ͳһĬ��Ϊ��Ʊ
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
		m_iBtnCur = 0;	// ��һ��
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
	// ��ʱ��ǰѡ���btnӦ���Ǳ����, ���ڵ���ʱ����ͬ�����ﲻ����
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
			if (L"�羯��" != StrTypeIndexName)
			{
				m_StrBtnIndexName = StrTypeIndexName;	// ����и����͵�ָ��洢�ˣ�ʹ�ô洢�ģ�����̳���һ������
			}
		}
		if ( InitBtnIndex() )	// ��ʼ��ָ��
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
		// �������ָ��
		//AddIndexToSubRegion(_T("����"));
		//AddIndexToSubRegion(_T("MACD"));
		AddSpecialCurveToSubRegion();
		if ( bReqData )
		{
			RequestTrendPlusData();
		}
	}
	else if ( ETBT_BuySellForce == eBtnCur )
	{
		// ��������ָ��
		//AddIndexToSubRegion(_T("��������"));
		//AddIndexToSubRegion(_T("VOL"));
		AddSpecialCurveToSubRegion();
		if ( bReqData )
		{
			RequestTrendPlusData();
		}
	}
	else if ( ETBT_CompetePrice == eBtnCur )
	{
		ASSERT( eRT == ERTStockCn );	// ���ڹ�Ʊ ��֤ ��֤,
		if ( m_iTrendDayCount != 1 )
		{
			// ����շ�ʱ��ͻ
			SetTrendMultiDay(1, false, false);	// ֻ���1�󣬲���Ҫ������������
			OnVDataMerchKLineUpdate(m_pMerchXml);	// �ض�����
		}

		// ����ͼ
		RemoveSubRegion();
		// ���þ���ͼ������~~
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
		CString StrIndexName = L"�羯��";

		RemoveSubRegion();
		m_pRegionMain->NestSizeAll();
		UpdateAxisSize( );
		m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);

		AddIndexToSubRegion(StrIndexName);

		Invalidate();
	}
	else if ( ETBT_DuoKong == eBtnCur )
	{
		// ���ָ��
		//AddIndexToSubRegion(_T("���"));
		//AddIndexToSubRegion(_T("DMI"));
		AddSpecialCurveToSubRegion();
		if ( bReqData )
		{
			RequestTrendPlusData();
		}
	}
	else if ( ETBT_RiseFallRate == eBtnCur )
	{
		// �ǵ���ָ��
		//AddIndexToSubRegion(_T("�ǵ���"));
		//AddIndexToSubRegion(_T("DMA"));
		AddSpecialCurveToSubRegion();
		if ( bReqData )
		{
			RequestTrendPlusData();
		}
	}
	else if ( ETBT_PriceModel == eBtnCur )
	{
		// �ڻ��۸�ģ�ͣ�������
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
		// ָ����ʾ�ɽ��û����ȷ�Ķ���������
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

	CRect rc, rcClient;		// ��ť����rect
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

	// ����ɫ
	if (!rcTemp.IsRectEmpty() )
	{
		memDC.FillSolidRect(rcClient.left, rcTemp.top -1, iBtnBkLength + 1 + 1, rcClient.bottom, RGB(0,0,0));	// �ָ���
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
		m_StrBtnIndexName = aNames[0];	// Ĭ�ϸ���һ��
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
		// ��ʷ&&��Ϊ����ioview������ͼ����ʾ���
		rc.top = rc.bottom - TREND_BTN_HEIGHT;
	}
	else
	{
		rc.top = rc.bottom;
	}

	// ��ʱ������Щ����
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
		// ��ť
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
				// ÿ����Ʒ���Ͷ�Ӧ�İ�ť����
				m_mapMerchTypeBtnIndex[ iMerchType ] = atol(pszAttri);				
			}

			pszAttri = pEle->Attribute(KStrXMLAttriTrendIndexName);
			if ( NULL != pszAttri )
			{
				// ÿ����Ʒ���Ͷ�Ӧ��ָ������
				TCHAR *pwszIndexName = CEtcXmlConfig::MultiToWide(pszAttri, CP_UTF8);
				if ( NULL != pwszIndexName )
				{
					m_mapMerchTypeIndexName[ iMerchType ] = pwszIndexName;
					delete []pwszIndexName;
				}
			}
		}
		// ��ͼ�߶ȱ���
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

	// ÿ����Ʒ���Ͷ�Ӧѡ���ָ��
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
	// ��ǰregion��ռ�ĸ߶ȷ������, ����trend��region�󲿷ֶ�û����Ӧ��ָ�꣬so���Լ�do it
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
	//	��ѯ�Ƿ���ھ��۲˵�
	int iBtnIndex = -1;
	for (unsigned i =0; i < m_aTrendBtnCur.GetSize(); ++i)
	{		
		if (ETBT_CompetePrice + BTN_ID_BEGIN == m_aTrendBtnCur[i].m_iID)
		{
			iBtnIndex = i;
			break;
		}
	}

	//	����Ѿ�ѡ�У����������
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

	m_iBtnCur = iBtnIndex;	//	���þ��۱�־λ
	if (-1 == m_iBtnCur)
	{
		return;	//	�����ھ��۲˵�
	}

	E_ReportType eRT = GetMerchKind(m_pMerchXml);
	m_mapMerchTypeBtnIndex[ eRT ] = m_iBtnCur;
	ASSERT( eRT == ERTStockCn );	// ���ڹ�Ʊ ��֤ ��֤,
	if ( m_iTrendDayCount != 1 )
	{
		// ����շ�ʱ��ͻ
		SetTrendMultiDay(1, false, false);	// ֻ���1�󣬲���Ҫ������������
		OnVDataMerchKLineUpdate(m_pMerchXml);	// �ض�����
	}

	// ����ͼ
	RemoveSubRegion();
	// ���þ���ͼ������~~
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
		// ��Ҫ��ӷǳɽ�����ͼ
		if ( !AddSubRegion(false) || m_SubRegions.GetSize() == 1 )
		{
			ASSERT( 0 );
			return false;;	// �޷�������ͼ
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
		return false;; // Ȩ�޲���
	}

	//
	for ( int32 i=0; i < pData->aIndexs.GetSize() ; i++ )
	{
		// ����ʵ�ʴ���indexҲ�Ǹ���name��������so���ж�����+region
		if ( pData->aIndexs[i]->strIndexName == StrIndexName && pSubRegion == pData->aIndexs[i]->pRegion )
		{
			return false;;
		}
	}

	//
	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);

	// pSubRegion�ѱ�ɾ������Ӧ������
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
		// ͣ�����ݵ�ָ����Ҫ�ض��������
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
	// ɾ�������region��ص�ָ���ߵ�
	int32 iDel = 0;
	int32 iSizeOld = pData->aIndexs.GetSize();
	for ( int32 i=pData->aIndexs.GetUpperBound(); i >= 0 ; --i )
	{
		if ( pData->aIndexs[i]->pRegion == pSubRegion )
		{
			DeleteIndexParamData(pData->aIndexs[i]);	// �Ѿ���removeAtɾ����
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
	// �����е�ָ�궼���ص� ָ���� ����ָ�� �˵���:
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

	//ϵͳָ��(sys_index.xml)
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

					// ����ָ�깫ʽ��, ��
					if ( EFTNormal != pContent->m_eFormularType )
					{
						continue;
					}

					if ( bMainRegion && !CheckFlag(pContent->flag, CFormularContent::KAllowMain))
					{
						// ��ǰ����ͼ,���ǹ�ʽû��������ͼ�ı�־.
						continue;
					}

					if ( !bMainRegion && !CheckFlag(pContent->flag, CFormularContent::KAllowSub))
					{
						// ��ǰ�Ǹ�ͼ,���ǹ�ʽû������ͼ�ı�־.
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

					bool32 bShow = CFormulaLib::BeIndexShow(pContent->name);	// �Ƿ��ڽ�������ʾ����

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
					// û���Ӳ˵���,������ɾ��, �������Ƶķ�����������һ����popup�˵�
					pMenu->DeleteMenu((UINT)pSubMenu->GetSafeHmenu(), MF_BYCOMMAND);
				}
			}						
		}
	}

	//�û��Ա�
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
						// ��ǰ����ͼ,���ǹ�ʽû��������ͼ�ı�־.
						continue;
					}

					if ( !bMainRegion && !CheckFlag(pContent->flag, CFormularContent::KAllowSub))
					{
						// ��ǰ�Ǹ�ͼ,���ǹ�ʽû������ͼ�ı�־.
						continue;
					}

					if( !CFormulaLib::BeIndexShow(pContent->name) )
					{
						// �����ڽ�������ʾ
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
				// û���Ӳ˵���,������ɾ��
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

		// ���һ���ǲ��ǿ�����ӵ�[���̸���]ָ��
		if ( pFormular->m_byteIndexStockType == EIST_Index && !bIsExp )
		{
			// ����ָ���ָ���Ʊ
			continue;
		}
		else if ( pFormular->m_byteIndexStockType == EIST_Stock && bIsExp )
		{
			// ����ָ�� ָ���Ʊ
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

	// ��Ҫ���������� & ͼ��
	// �������� �۸�&�ɽ��� Ҳ��Ӱ������Yֵ
	T_MerchNodeUserData *pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == pDC
		|| NULL == m_pCompetePriceNodes
		|| NULL == pData )
	{
		return;
	}

	TRACE(_T("��������: %d\n"), m_pCompetePriceNodes->GetSize());

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
		// �Ҷ�
		CColorStep step;
		step.InitColorRange(clrBk, 32.0f, 32.0f, 32.0f);
		clrBk = step.GetColor(1);
	}

	clrBk =  RGB(41, 40, 46);

	if ( &ChartRegion == m_pRegionMain )
	{
		// ��ͼ����
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

			// ����ɫ
			pDC->FillSolidRect(rcChart, clrBk);

			pDC->_DrawDotLine(pt1, pt2, 2, clrAxis);	// �м�����

			pt1.x = rcChart.right;
			pt2.x = pt1.x;
			pDC->_DrawLine(pt1, pt2);		// �Ҳ�����

			// ˮƽ��
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

				// ����
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

			// ��������
			COLORREF clrDataLine = GetIoViewColor(ESCGuideLine1);

			CArray<CPoint, const CPoint&>	aPts;
			const CArray<CNodeData, CNodeData &> &aNodes = m_pCompetePriceNodes->GetNodes();
			CChartCurve *pDep = ChartRegion.GetDependentCurve();
			int32 iInit = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time.GetTime();
			int32 iOpen = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeOpen.m_Time.GetTime();

			// �̶�ʱ�䵱��[9:15-9:25], ������ǳ�ʼ��������ʱ����
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

				// ����ÿ���㣬��СԲ����ʾ
				CBrush brh;
				brh.CreateSolidBrush(clrDataLine);
				CBrush *pOldBrush = (CBrush *)pDC->SelectObject(&brh);
				for ( int i=0; i < aPts.GetSize() ; i++ )
				{
					CRect rc(aPts[i], aPts[i]);
					rc.InflateRect(2, 2);
					pDC->RoundRect(rc, aPts[i]);
				}

				// ����Ҫ�����̼۵������������̼۲��������һ���㣬�����ǣһ���ߵ����̼�
				// ò�ƹ��˵����һ�������Ժ���޴�������
			}
		}
	}
	else if ( 
		m_SubRegions.GetSize() > 0
		&& m_SubRegions[0].m_pSubRegionMain != NULL
		&& &ChartRegion == m_SubRegions[0].m_pSubRegionMain )
	{
		// �ɽ�����ͼ
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

			// ����ɫ
			pDC->FillSolidRect(rcChart.left, rcChart.top+1, rcChart.Width(), rcChart.Height()-1, clrBk);

			pDC->_DrawDotLine(pt1, pt2, 2, clrAxis);	// �м�����

			pt1.x = rcChart.right;
			pt2.x = pt1.x;
			pDC->_DrawLine(pt1, pt2);		// �Ҳ�����

			CRect RectSubMain = rcChart;

			// ˮƽ��
			{
				// ���
				if (  ChartRegion.GetTopSkip() > 6 )		// �ɽ�����ͼ��topskip�����٣����Ȼ���Ȧ )
				{
					pt1 = RectSubMain.TopLeft();
					pt2 = CPoint(RectSubMain.right, RectSubMain.top);
					pDC->_DrawLine(pt1, pt2);
				}

				pt1 = CPoint(RectSubMain.left, RectSubMain.bottom);
				pt2 = CPoint(RectSubMain.right, RectSubMain.bottom);
				pDC->_DrawLine(pt1, pt2);						
			}

			// ��ˮƽ��
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
						// �ɽ�����ͼ�ĵ�һ�����߸߶�С�ڴ�
						continue;
					}

					// ����
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

			// ��������
			COLORREF clrDataLine = GetIoViewColor(ESCGuideLine1);

			CArray<CPoint, const CPoint&>	aPtsRed, aPtsGreen, aPtsWhite;
			const CArray<CNodeData, CNodeData &> &aNodes = m_pCompetePriceNodes->GetNodes();
			CChartCurve *pDep = ChartRegion.GetDependentCurve();
			int32 iInit = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time.GetTime();
			int32 iOpen = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeOpen.m_Time.GetTime();
			//float fPriceBase = pData->m_TrendTradingDayInfo.GetPrevReferPrice();
			float fPriceBase = GetTrendPrevClose();

			// �̶�ʱ�䵱��[9:15-9:25], ������ǳ�ʼ��������ʱ����
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
							aPtsGreen.Add(pt);							// ����
							aPtsGreen.Add(CPoint(pt.x, rcChart.bottom));	// �׵�
						}
						else if ( 2.0f == node.m_fOpen )
						{
							aPtsRed.Add(pt);							// ����
							aPtsRed.Add(CPoint(pt.x, rcChart.bottom));	// �׵�
						}
						else
						{
							ASSERT( 0.0f == node.m_fOpen );
							aPtsWhite.Add(pt);							// ����
							aPtsWhite.Add(CPoint(pt.x, rcChart.bottom));	// �׵�
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
		return;	// ������
	}
	ASSERT( NULL != m_pCompetePriceNodes );
	if ( !pData->m_TrendTradingDayInfo.m_bInit )
	{
		m_pCompetePriceNodes->RemoveAll();	// δ��ʼ��
		return;
	}

	TRACE(_T("�������ݼ��㿪ʼ: %d\n"), m_pCompetePriceNodes->GetSize());
	m_pCompetePriceNodes->RemoveAll();	// δ��ʼ��

	CGmtTime TimeStart, TimeEnd;
	//float fPriceBase = pData->m_TrendTradingDayInfo.GetPrevReferPrice();
	float fPriceBase = GetTrendPrevClose();
	if ( GetCallAuctionTime(TimeStart, TimeEnd)
		&& fPriceBase > 0.0f )
	{
		// ��ȡ��Ʒ��������
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
				TRACE(_T("�ص���������: ʱ��%u �۸�%f\r\n"), pAuction[i].m_lTime, pAuction->m_fBuyPrice);
				continue;
			}
			pNodesData[iNodePos].m_fClose = pAuction[i].m_fBuyPrice;
			pNodesData[iNodePos].m_fVolume = pAuction[i].m_fBuyVolume/iVolScale;
			pNodesData[iNodePos].m_iID = pAuction[i].m_lTime;			// �������ݲ��ط��Ӷ���
			pNodesData[iNodePos].m_fOpen = pAuction[i].m_cFlag - '0';	// '0'û�� 1�� 2��
			iNodePos++;
		}

		m_pCompetePriceNodes->GetNodes().SetSize(iNodePos);	// ʵ�����ݳ���
		// ���������Ի�ģ����ô����
		// ��Ҫ�ضϷ�ʱ����ھ�������
	}
	else
	{
		m_pCompetePriceNodes->RemoveAll();
	}

	// 	{
	// 		// ���Ծ�������
	// 		CStdioFile file;
	// 		file.Open(_T("��������-��ʱ.txt"), CFile::modeCreate |CFile::modeWrite |CFile::modeNoTruncate);
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
	// 			Str.Format(_T("%02d:%02d:%02d �۸�:%f ��־:%.0f �ɽ���:%f\n")
	// 				, pt->tm_hour, pt->tm_min, pt->tm_sec
	// 				, pNodes[i].m_fClose, pNodes[i].m_fOpen, pNodes[i].m_fVolume);
	// 			int iLen = WideCharToMultiByte(CP_ACP, 0, Str, -1, szBuf, sizeof(szBuf), NULL, NULL);
	// 			file.Write(szBuf, iLen > 0 ? iLen-1: 0);
	// 		}
	// 		file.Close();
	// 	}

	TRACE(_T("�������ݼ������: %d\n"), m_pCompetePriceNodes->GetSize());
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
	// �������ߵļ�����������������Сֵ, һ��Ҫ�������Ѿ����ú���֮����øú���
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

		// ��ͼ��Ҫӳ��
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
			fYMin = min(0.0f, NodeData.m_fVolume);	// ��ͼ��Сֵ0
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
		return false;	// ��֧���������ͻ�
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
		fYMin = min(0.0f, NodeData.m_fVolume); // ��ͼ��Сֵ0
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
				ASSERT( pDep->GetNodes()==NULL || pDep->GetNodes()->GetSize()==0 );	// ��ͼ������Ӧ����׼���õĶ����Ǳؼ����˵�
				// �����Լ��㾺��ͼ��ͼ�������Сֵ
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
				// ��ͼ�����Сֵ����, ��ͼ����ʱ�ż���
				//ASSERT( NULL == pDep );	// ��û׼���ø�ͼ��, ��ʱ��ȷʵû��׼���� �絥������RedrawAll
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
	// ��û��0��������
	// ������ʵʱ���۵��ǲ��ǽ����ʵʱ���������⴦��
	if ( fPriceBase == 0.0f
		&& NULL != pData->m_pMerchNode
		&& NULL != pData->m_pMerchNode->m_pRealtimePrice )
	{
		if (ERTFuturesCn == pData->m_pMerchNode->m_Market.m_MarketInfo.m_eMarketReportType)	// �������ڻ�����������
		{
			fPriceBase = pData->m_pMerchNode->m_pRealtimePrice->m_fPricePrevAvg;	// ȡ���������
		}
		else
		{
			fPriceBase = pData->m_pMerchNode->m_pRealtimePrice->m_fPricePrevClose;	// ȡ���������
		}
	}
	return fPriceBase;
}

void CIoViewTrend::OnVDataGeneralNormalUpdate( CMerch *pMerch )
{
	// ָ����Ʒ: �������� ��� �ǵ���
	// Ŀǰ����ͬ�г���Ʒ���ص�һ��һ��������
	if ( NULL == m_pMerchXml || m_pMerchXml != pMerch )
	{
		return;
	}

	// ������ 0-000001 1000-399001��������Ʒ������
	int32 iMarketId		 = m_pMerchXml->m_MerchInfo.m_iMarketId;
	CString StrMerchCode = m_pMerchXml->m_MerchInfo.m_StrMerchCode;
	bool32 bMatchMerch = IsRedGreenMerch(m_pMerchXml);
	if ( !bMatchMerch )
	{
		return;
	}

	// ��������Ż�����~

	// ����������һ���е�
	CalcRedGreenValue();

	E_TrendBtnType eBtn = GetCurTrendBtnType();
	if ( ETBT_DuoKong == eBtn )
	{
		// ����������ͬ���ݣ�������ת��
		CalcDuoKongData();
	}
	else if ( ETBT_RiseFallRate == eBtn )
	{
		// �ǵ��� �����µ�������ɵ�����
		CalcExpRiseFallRateData();
	}

	// ������ʾ����
	ReDrawAysnc();
}
void CIoViewTrend::OnVDataGeneralFinanaceUpdate( CMerch *pMerch )
{
	// ����Ҫ��������
}

void CIoViewTrend::OnVDataMerchTrendIndexUpdate( CMerch *pMerch )
{
	// ����: ���� ��������
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
		// ��������
		CalcVolRateData();
		ReDrawAysnc();
	}
	else if ( ETBT_BuySellForce == eBtn )
	{
		// ������������
		CalcBuySellForceData();
		ReDrawAysnc();
	}
}

void CIoViewTrend::OnVDataMerchAuctionUpdate( CMerch *pMerch )
{
	// ����: ����ͼ
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
		// ���Ͼ�������
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
	// ������ֻ������0-000001 1000-399001��, ����������һ��
	// �ǵ�����Ҫһ��ָ���������ǵ����� ��
	bool32 bReqGeneral = false; //ETBT_RiseFallRate == eBtn; // �ǵ�����������������ݴ���ͬ�ṹ�У�Ŀǰͬ����
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
		// ����ͼ
		CMmiReqMerchAution reqAuction;
		reqAuction.m_iMarketId = iMarketId;
		reqAuction.m_StrMerchCode = StrMerchCode;
		DoRequestViewData(reqAuction);

		// ����ͼ��ʵʱ��������ͨ��RealTimePrice�ϳ�
	}
	else if ( ERTStockCn == eMerchType
		&& (ETBT_BuySellForce == eBtn
		|| ETBT_VolRate == eBtn ) )
	{
		// �������� || ����
		CMmiReqMerchTrendIndex reqTrendIndex;
		reqTrendIndex.m_iMarketId = iMarketId;
		reqTrendIndex.m_StrMerchCode = StrMerchCode;
		DoRequestViewData(reqTrendIndex);
	}

	RequestTrendPlusPushData();	// ע������

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
	// ������ֻ������0-000001 1000-399001��, ����������һ��
	// �ǵ�����Ҫһ��ָ���������ǵ����� ��
	bool32 bReqGeneral = false; //ETBT_RiseFallRate == eBtn; // �ǵ�����������������ݴ���ͬ�ṹ�У�Ŀǰͬ����
	bReqGeneral = IsRedGreenMerch(pMerch);

	// �˴���������������
	if ( bReqGeneral )
	{
		CMmiReqAddPushGeneralNormal reqGPush;
		reqGPush.m_iMarketId = iMarketId;
		DoRequestViewData(reqGPush);	// ���ɷ��ӻ���Ҫ���²�����������~~

		bHasReqPush = true;
	}
	//else
	//{
	//	// ��������ɾ���ģ���Ϊ������ͼ��������Ҫ�������
	//	CMmiReqDelPushGeneralNormal reqGDelPush;
	//	reqGDelPush.m_iMarketId = iMarketId;
	// 	DoRequestViewData(reqGDelPush);
	//}

	// ����ͼ������ʵʱ��������ɾ������ͣ����ﲻ����
	bool32 bReqMerchTrendIndex = false;
	if ( ERTStockCn == eMerchType
		&& (ETBT_BuySellForce == eBtn
		|| ETBT_VolRate == eBtn ) )
	{
		// �������� || ����
		// �������������
		CMmiReqAddPushMerchTrendIndex reqTrendIndexPush;
		reqTrendIndexPush.m_iMarketId = iMarketId;
		reqTrendIndexPush.m_StrMerchCode = StrMerchCode;
		DoRequestViewData(reqTrendIndexPush);

		bHasReqPush = true;
		bReqMerchTrendIndex = true;
	}

	//if ( !bReqMerchTrendIndex )
	//{
	//	// ��������ɾ���ģ���Ϊ������ͼ��������Ҫ�������
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
	// ������ֻ������0-000001 1000-399001��, ����������һ��
	// �ǵ�����Ҫһ��ָ���������ǵ����� ��
	// �ǵ�����������������ݴ���ͬ�ṹ�У�Ŀǰͬ����
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
		CalcRedGreenValue();	// ���������
	}
	else
	{
		CalcNoWeightExpLine();	// ���߿�����Ҫ����
	}

	E_TrendBtnType eBtn = GetCurTrendBtnType();
	switch ( eBtn )
	{
	case ETBT_DuoKong:	// ��� - ָ
		CalcDuoKongData();
		break;
	case ETBT_RiseFallRate:	// �ǵ��� -ָ
		CalcExpRiseFallRateData();
		break;
	case ETBT_VolRate:	// ���� - ��
		CalcVolRateData();
		break;
	case ETBT_BuySellForce:	// �������� - ��
		CalcBuySellForceData();
		break;
	case ETBT_CompetePrice:	// ����ͼ
		CalcCompetePriceValue();
		break;
	default:	// ������������ͨ���˷�������
		break;
	}
}

CString	GetSpecialCurveTitle(int32 iSpecialId)
{
	static CStringArray s_aNames;
	if ( s_aNames.GetSize() <= 0 )
	{
		s_aNames.Add(_T("���"));
		s_aNames.Add(_T("�Ǽ�"));
		s_aNames.Add(_T("����"));
		s_aNames.Add(_T(""));		// �ǵ���ֵ
		s_aNames.Add(_T("����"));
		s_aNames.Add(_T("ί��"));
		s_aNames.Add(_T("ί��"));
		s_aNames.Add(_T(""));		// ����������ֵ
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
		s_aStyles.Add( 0 );	// ��� 0 Ĭ������~
		s_aStyles.Add( 0 );  // �Ǽ�
		s_aStyles.Add( 0 );	// ����
		s_aStyles.Add( CIndexDrawer::EIDSColorStick );		// �ǵ���ֵ
		s_aStyles.Add( 0 );	// ����
		s_aStyles.Add( 0 );	// ί��
		s_aStyles.Add( 0 );	// ί��
		s_aStyles.Add( CIndexDrawer::EIDSColorStick );		// ����������ֵ
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
		// �Ѿ�û���������
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
	ASSERT( 0 );		// �����ڵ��߾�Ӧ����region�е�
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
		// ��Ҫ��ӷǳɽ�����ͼ
		if ( !AddSubRegion(false) || m_SubRegions.GetSize() == 1 )
		{
			ASSERT( 0 );
			return;	// �޷�������ͼ
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

	DeleteRegionCurves(pSubRegion);		// �Ƴ�������

	for ( int32 i=0; i < aSpecialIds.GetSize() ; i++ )
	{
		// 
		CString StrCurveTitle = GetSpecialCurveTitle(aSpecialIds[i]);
		DWORD dwDrawStyle = GetSpecialCurveIndexDrawStyle(aSpecialIds[i]);

		// ������Щ�߶���ʱ������ģ�so ò�ƿ��Բ��ض���~~
		uint32 iFlag = CChartCurve::KTypeTrend|CChartCurve::KUseNodesNameAsTitle|CChartCurve::KYTransformByClose|CChartCurve::KTimeIdAlignToXAxis |CChartCurve::KDonotPick;

		if ( pSubRegion->GetCurveNum() == 0 )
		{
			AddFlag(iFlag,CChartCurve::KDependentCurve);
		}

		//
		CChartCurve *pCurve = pSubRegion->CreateCurve(iFlag);
		SetSpecialCurvePickColor(aSpecialIds[i], pCurve->GetPickColor());	// ������������

		pCurve->SetTitle(StrCurveTitle);
		CIndexDrawer* pDrawer = new CIndexDrawer(*this, (CIndexDrawer::E_IndexDrawStyle)dwDrawStyle, CIndexDrawer::EIDSENone, -1, CLR_DEFAULT);
		pCurve->AttatchDrawer(pDrawer);

		//
		CNodeSequence *pNodes = CreateNodes();
		pNodes->SetName(StrCurveTitle);		// ����һ��������ݵ�~~����
		pNodes->SetUserData(pData);
		pCurve->AttatchNodes(pNodes);

		// �պ��Ÿ����ɫ
		pCurve->m_clrTitleText = GetIoViewColor((E_SysColor)(ESCGuideLine1+i%16));
	}

	UpdateSelfDrawCurve();

	// ��������~~
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
			RemoveFlag(m_pNoWeightExpCurve->m_iFlag, CChartCurve::KInVisible);	// ��ʾ����Ȩ��
		}
		for ( int32 i=0; i < m_pRegionMain->m_Curves.GetSize() ; i++ )
		{
			CChartCurve *pCurve = m_pRegionMain->m_Curves[i];
			if ( NULL != pCurve
				&& CheckFlag(pCurve->m_iFlag, CChartCurve::KYTransformByAvg) )
			{
				// �ҵ���ͨ����, ������
				AddFlag(pCurve->m_iFlag, CChartCurve::KInVisible);
				break;
			}
		}
	}
	else
	{
		// ��ͨ����
		if ( NULL != m_pNoWeightExpCurve )
		{
			if ( NULL != m_pNoWeightExpCurve->GetNodes() )
			{
				m_pNoWeightExpCurve->GetNodes()->RemoveAll();
			}
			AddFlag(m_pNoWeightExpCurve->m_iFlag, CChartCurve::KInVisible);	// ���ز���Ȩ��
		}

		if ( NULL != m_pRegionMain )
		{
			for ( int32 i=0; i < m_pRegionMain->m_Curves.GetSize() ; i++ )
			{
				CChartCurve *pCurve = m_pRegionMain->m_Curves[i];
				if ( NULL != pCurve
					&& CheckFlag(pCurve->m_iFlag, CChartCurve::KYTransformByAvg) )
				{
					// �ҵ���ͨ����, ȥ������
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
		// ����Ȩ����
		if ( NULL == m_pNoWeightExpCurve )
		{
			// �½�һ��������״�ߣ�ֻ��ָ��ʱ�Ÿ�ֵ������ʱ�򲻸�ֵ, ����ֵ���̼�
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

		// ��������, ����Ȩָ��Ϊ�ǵ�������...0.10��ʾ��0.10%
		pNodes->RemoveAll();
		const CAbsCenterManager::GeneralNormalArray *pGeneralNormals = m_pAbsCenterManager->GetGeneralNormalArray(m_pMerchXml);

		float fPricePreClose = GetTrendPrevClose();
		// �����ֵӳ�䵽���귶Χ��
		if ( NULL != pGeneralNormals
			&& fPricePreClose > 0.0f )
		{
			const int32 iSize = pGeneralNormals->GetSize();
			pNodes->GetNodes().SetSize(iSize);
			CNodeData *pNodeDatas = pNodes->GetNodes().GetData();
			const T_GeneralNormal *pSrcData = pGeneralNormals->GetData();
			for ( int32 i=0; i < iSize ; i++ )
			{
				// ȫ������ֵ��close��
				//TRACE(_T("����Ȩ����: %d - %f%%\n"),pSrcData[i].m_lTime, pSrcData[i].m_fRiseRateNoWeighted);
				//float fCur = pSrcData[i].m_fRiseRateNoWeighted/100 + 0.0f;
				float fCur = pSrcData[i].m_fRiseRateNoWeighted + 0.0f;
				// ת��Ϊ�۸�
				bool32 b = CPriceToAxisYObject::AxisYToPriceYByBase(CPriceToAxisYObject::EAYT_Pecent, fPricePreClose, fCur, fCur);
				ASSERT( b );
				pNodeDatas[i].m_fClose = fCur;

				CGmtTime Time1(pSrcData[i].m_lTime);
				SaveMinute(Time1);
				pNodeDatas[i].m_iID = Time1.GetTime();
			}
		}
		TrimPlusShowNodeData(pNodes);

		RemoveFlag(m_pNoWeightExpCurve->m_iFlag, CChartCurve::KInVisible);	// ��ʾ����Ȩ��
		for ( int32 i=0; i < m_pRegionMain->m_Curves.GetSize() ; i++ )
		{
			CChartCurve *pCurve = m_pRegionMain->m_Curves[i];
			if ( NULL != pCurve
				&& CheckFlag(pCurve->m_iFlag, CChartCurve::KYTransformByAvg) )
			{
				// �ҵ���ͨ����, ������
				AddFlag(pCurve->m_iFlag, CChartCurve::KInVisible);
				break;
			}
		}
	}
	else
	{
		// ��ͨ����
		if ( NULL != m_pNoWeightExpCurve )
		{
			if ( NULL != m_pNoWeightExpCurve->GetNodes() )
			{
				m_pNoWeightExpCurve->GetNodes()->RemoveAll();
			}
			AddFlag(m_pNoWeightExpCurve->m_iFlag, CChartCurve::KInVisible);	// ���ز���Ȩ��
		}

		if ( NULL != m_pRegionMain )
		{
			for ( int32 i=0; i < m_pRegionMain->m_Curves.GetSize() ; i++ )
			{
				CChartCurve *pCurve = m_pRegionMain->m_Curves[i];
				if ( NULL != pCurve
					&& CheckFlag(pCurve->m_iFlag, CChartCurve::KYTransformByAvg) )
				{
					// �ҵ���ͨ����, ȥ������
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
		return;	// �����ڵڶ���ͼ��û�г�ʼ��׼���ã�3
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


	// �������: 0-000001 1000-399001
	E_TrendBtnType eBtn = GetCurTrendBtnType();
	if ( ETBT_DuoKong == eBtn
		&& IsRedGreenMerch(m_pMerchXml)
		&& NULL != pGeneralNormals )
	{
		CMarket &Market = pData->m_pMerchNode->m_Market;

		// �����ֵӳ�䵽���귶Χ��
		const int32 iSize = pGeneralNormals->GetSize();
		pNodes->GetNodes().SetSize(iSize);
		CNodeData *pNodeDatas = pNodes->GetNodes().GetData();
		const T_GeneralNormal *pSrcData = pGeneralNormals->GetData();
		for ( int32 i=0; i < iSize ; i++ )
		{
			// ȫ������ֵ��close��
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

		// �Ƿ�������ֵ��ʾ~~
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
		return;	// �����ڵڶ���ͼ��û�г�ʼ��׼���ã�3
	}

	// �ǵ����� 3����
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

	// �������: 0-000001 1000-399001
	E_TrendBtnType eBtn = GetCurTrendBtnType();
	const CAbsCenterManager::GeneralNormalArray *pGeneralNormals = m_pAbsCenterManager->GetGeneralNormalArray(m_pMerchXml);
	if ( ETBT_RiseFallRate == eBtn
		&& IsRedGreenMerch(m_pMerchXml)
		&& NULL != pGeneralNormals )
	{
		CMarket &Market = pData->m_pMerchNode->m_Market;

		// �����ֵӳ�䵽���귶Χ��
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
			// ȫ������ֵ��close��
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
		// ��diffֵӳ�䵽[min-max]֮��, �м�ֵ0 ӳ�䵽1/2��
		// diff�����ֵ���ᳬ��fMax-fMin, ��Сֵ����С��fMin-fMax
		for (int i=0; i < iSize ; i++ )
		{
			float fV = pNodeDatasRise[i].m_fClose - pNodeDatasFall[i].m_fClose;
			fV /= 2;
			fV += fHalf;	// �������ֵ��ƫ��ֵ
			pNodeDatasDiff[i].m_fClose = fV;
		}

		pCurveDiff->SetUserPriceMinMaxBaseValue(fMin);	// �趨diff�ߵ��û���СֵΪ0
		pCurveDiff->SetPriceBaseY(fHalf);			// �趨��ֵ���������������꣬�����趨����ֵ��������Ӱ��

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
		return;	// �����ڵڶ���ͼ��û�г�ʼ��׼���ã�3
	}

	// ����
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

	// ���� �����ݾ���
	E_TrendBtnType eBtn = GetCurTrendBtnType();
	if ( ETBT_VolRate == eBtn )
	{
		CMerch *pMerch = pData->m_pMerchNode;

		// �����ֵӳ�䵽���귶Χ��
		const int32 iSize = pMerch->m_aMerchTrendIndexs.GetSize();
		pNodes->GetNodes().SetSize(iSize);
		CNodeData *pNodesData = pNodes->GetNodes().GetData();
		const T_MerchTrendIndex *pSrcData = pMerch->m_aMerchTrendIndexs.GetData();
		for ( int32 i=0; i < iSize ; i++ )
		{
			// ȫ������ֵ��close��
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
		return;	// �����ڵڶ���ͼ��û�г�ʼ��׼���ã�3
	}

	// �������� 3����
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

		// �����ֵӳ�䵽���귶Χ��
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
			// ȫ������ֵ��close��
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
		// ��diffֵӳ�䵽[min-max]֮��, �м�ֵ0 ӳ�䵽1/2��
		// diff�����ֵ���ᳬ��fMax-fMin, ��Сֵ����С��fMin-fMax
		for ( int i=0; i < iSize ; i++ )
		{
			float fV = pNodeDatasBuy[i].m_fClose - pNodeDatasSell[i].m_fClose;
			fV /= 2;
			fV += fHalf;
			pNodeDatasDiff[i].m_fClose = fV;
		}

		pCurveDiff->SetUserPriceMinMaxBaseValue(fMin);	// �趨diff�ߵ��û���СֵΪ0
		pCurveDiff->SetPriceBaseY(fHalf);			// �趨��ֵ���������������꣬�����趨����ֵ��������Ӱ��

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
		ASSERT( 0 );	// ��Чʱ��Σ���
		return;
	}

	CNodeData *pNodesData = pNodes->GetNodes().GetData();
	const int32 iSize = pNodes->GetSize();
	if ( NULL == pNodesData || iSize <= 0 )
	{
		return;	// ������
	}

	int32 iPosStart, iPosEnd;
	if ( !pNodes->LookupEx(TimeStart.GetTime(), iPosStart)
		|| !pNodes->LookupEx(TimeEnd.GetTime(), iPosEnd) 
		|| iPosEnd < iPosStart
		|| iPosStart < 0
		|| iPosEnd >= iSize )
	{
		ASSERT( 0 );
		pNodes->RemoveAll();	// ȫ�ص�
		return;	// �Ҳ������ʵĿ�ʼ&�ս��
	}
	// ��������λ���ų������ʵ�
	while(iPosStart <= iPosEnd)
	{
		// [��ʼ�� -> ������]
		if ( pNodesData[iPosStart].m_iID < TimeStart.GetTime() )
		{
			iPosStart++;
		}
		break;
	}
	while(iPosEnd >= iPosStart)
	{
		// [������ -> ��ʼ��]
		if ( pNodesData[iPosEnd].m_iID > TimeEnd.GetTime() )
		{
			iPosEnd--;
		}
		break;
	}
	if ( iPosEnd < iPosStart )
	{
		// û�к�������
		pNodes->RemoveAll();
		return;
	}

	if ( iSize > iPosEnd+1 )
	{
		pNodes->GetNodes().RemoveAt(iPosEnd+1, iSize-iPosEnd-1);	// ɾ������β������
	}
	if ( iPosStart > 0 )
	{
		pNodes->GetNodes().RemoveAt(0, iPosStart);	// ɾ��ͷ����������
	}
}

bool32 CIoViewTrend::OnGetCurveTitlePostfixString( CChartCurve *pCurve, CPoint ptClient, bool32 bHideCross, OUT CString &StrPostfix )
{
	// ���ڶ���ͼ��Ҫ���⴦����Щ
	E_TrendBtnType eBtn = GetCurTrendBtnType();
	if ( ETBT_DuoKong != eBtn
		&& ETBT_RiseFallRate != eBtn
		&& ETBT_VolRate != eBtn
		&& ETBT_BuySellForce != eBtn )
	{
		return false;
	}

	// �� �ڶ���ͼ��Ҫ
	if ( pCurve == NULL
		|| &pCurve->GetChartRegion() == m_pRegionMain
		|| m_SubRegions.GetSize() != 2
		|| m_SubRegions[1].m_pSubRegionMain != &pCurve->GetChartRegion() )
	{
		return false;
	}

	// �ҵ����node
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
		// ����ʮ�ֹ��λ�õõ�NodeData.						
		int32 ix = ptClient.x;
		int32 iPos = -1;

		pCurve->GetChartRegion().ClientXToRegion(ix);
		pCurve->RegionXToCurvePos(ix,iPos);

		// iPos������������λ��, ���������node��λ��
		// ����ͨ��������£�Ӧ���ǵ���Ӧ��
		// ������쳣����£���Ȼû����ʾ����ô��ʾ����Ҳ���Ǻܴ���
		// ����Ժ���Ҫ�޸ģ���ȫ������ѯһ��
		if ( iPos >= iSize || -1 == iPos )
		{
			const int32 iAxisCount = pCurve->GetChartRegion().m_aXAxisNodes.GetSize();
			if ( iPos >=0 && iPos < iAxisCount )
			{
				CAxisNode *pAxisNode = (CAxisNode *)pCurve->GetChartRegion().m_aXAxisNodes.GetData();
				int32 iTimeId = pAxisNode[iPos].m_iTimeId;
				if ( iTimeId > 0 )
				{
					// ����ʵ�ʵ����ݣ��ҵ�ʱ����ͬ�����ʾ����
					CNodeData *pNodesData = pNodes->GetNodes().GetData();
					int32 iUp = iSize-1, iDown = 0;
					while ( iUp >= iDown )
					{
						int32 iHalf = (iUp+iDown)/2;
						int32 iId = pNodesData[iHalf].m_iID;
						if ( iId == iTimeId )
						{
							iPos = iHalf;	// �ҵ���
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
		// �õ����µ�һ��ָ��ֵ											

		pNodes->GetAt(iSize-1,NodeData);
	}
	else
	{
		// û������
		NodeData.m_iFlag |= CNodeData::KValueInvalid;	// ���ֵ��Ч
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
				// ����Ϊ�⼸�� Ψһ��־��һ�������б�, ����ע���ݵı仯
				bSame = false;
				break;
			}
		}
		if ( !bSame )
		{
			// ����
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
				Invalidate();	// ������ʾ
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

	// ���¸���һ�¿�����ʱ�����Ϣ
	T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
	if (NULL == pData)
	{
		return;
	}

	// ��ȡ����Ʒ������ص�����
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
		// ��ʷ��ʱ - �趨��������Ϣ(��ʷ��ʱ�ϸ������ϲ������л���Ʒ����Ϊÿ����Ʒ�Ľ�������Ϣ������ͬ)
		if ( m_pMerchXml->m_Market.GetSpecialTradingDayTime(m_TimeHistory, RecentTradingDay, m_pMerchXml->m_MerchInfo) )
		{
			pData->m_TrendTradingDayInfo.Set(m_pMerchXml, RecentTradingDay);
			pData->m_TrendTradingDayInfo.RecalcPrice(*m_pMerchXml);
			pData->m_TrendTradingDayInfo.RecalcHold(*m_pMerchXml);
		}
	}

	//
	m_aTrendMultiDayIOCTimes.RemoveAll();	// ���ʱ�����Ϣ
	m_aTrendMultiDayIOCTimes.Add(pData->m_TrendTradingDayInfo.m_MarketIOCTime);		// �����һ��ĳ�ʼ��ʱ��

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

	// ���ڶ���
	CRect rcRegion(0,0,0,0);
	rcRegion.bottom	= m_pRegionMain->GetRectCurves().top - 2;
	rcRegion.top = rcRegion.bottom - 8;		// ��ô�ߵĸ�������
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

	// ʱ���Ļ���
	//   �Դ������ʱ��㣬���ǵ�ǰʱ���
	// ����x����
	// ��ʱ����ʾ����&&������

	// x���궨λ����:
	// 1. �����������̺󷢳�����Ϣ���������ڵڶ��콻�׵���Ϣ��Ӧ����Ϊ�ڶ���ķ�������?
	// 2. ���ձ���ʱ���Ӧĳ��ʱ�䷶Χ�����ڸ÷�Χ�ڵģ�Ϊ���գ������ڵ�����ǰ�����
	// 3. ����gmtʱ���Ӧ����ʱ�䷶Χ�����ڸ÷�Χ�ģ�Ϊ���գ������ڵ�����ǰ�����

	// ����2�ŷ�������

	int32 iNodeSize = m_pRegionMain->m_aXAxisNodes.GetSize();
	const int32 iPosStart = 0;
	int32 iPosCount = iNodeSize;
	CAxisNode	nodeStart = m_pRegionMain->m_aXAxisNodes[iPosStart];
	CAxisNode	nodeEnd	  = m_pRegionMain->m_aXAxisNodes[ iPosCount-1 ];
	CGmtTime timeEnd(nodeEnd.m_iTimeId);
	timeEnd += CGmtTimeSpan(0, 0, 0, 59);

	// ʱ���ʽ 2010-01-01 00:00:00 - �����ʱ��Ϣ���׵�ʱ��Ϊ 2010-01-01 01:01:01

	mapLandMine::reverse_iterator it = m_mapLandMine.rbegin();
	// ����ռ�ʱ�����ʾ�����Ҳ��+1�ĵ��ʱ�仹������ʾ���ռǣ�������Ҳ�+1�㣬�򲻿������ռǵĹ���
	if ( it != m_mapLandMine.rend() )
	{
		int32 iPosEndPlus = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pData->m_aKLinesFull, timeEnd);
		if ( iPosEndPlus >= 0 && iPosEndPlus < pData->m_aKLinesFull.GetSize() )
		{
			// ӵ���Ҳ�+1ֵ, ���˵�����>=�Ҳ�ʱ����ֵ
			// ���ڽ������ڽ��ף�so����ʱ����󲻻������Ҫ��Ϣ���ף��ɺ���
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
	// ��ʱͼ�������һ������ʾ�ϸ������ս�������������տ���ʱ����������
	// ����ԭ��ȡ�ϸ������պ��һ����Ϊ��ʼʱ��, ���û���ϸ�������������00:00:00
	// ����ʾ����&&��������Ϣ
	bool32 bOnlyShowTaday = true;
	if (!bOnlyShowTaday)
	{
		// �������k���а������ϸ������յ���Ϣ
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
					TimeCurDay = IOC.m_TimeEnd.m_Time + CGmtTimeSpan(0, 0, 1, 0);	// ����ʱ����1���ӻ���Ϊǰһ�������յĺ�һ��
					SaveDay(TimeCurDay);		// ��һ�������Ϣ�����ڿ�ʼ���ݵ�
					StrStartTime.Format(_T("%04d-%02d-%02d")
						, TimeCurDay.GetYear(), TimeCurDay.GetMonth(), TimeCurDay.GetDay() );
					// ���ﲢû�а��շ���2���𱾵�ʱ��!!!!
					ASSERT( 0 );
				}
			}
		}
	}
	if ( StrStartTime.IsEmpty() )
	{
		// ����
		CGmtTime TimeCurDay = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time;
		SaveDay(TimeCurDay);
		CGmtTime TimeLocalStart, TimeLocalEnd;
		GetLocalDayGmtRange(TimeCurDay, TimeLocalStart, TimeLocalEnd);
		StrStartTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d")
			, TimeLocalStart.GetYear(), TimeLocalStart.GetMonth(), TimeLocalStart.GetDay()
			, TimeLocalStart.GetHour(), TimeLocalStart.GetMinute(), TimeLocalStart.GetSecond());

		// �������һ����ʾ��ʱ��Ϊ���пɼ�������һ����
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
			// ��ʱͼ�������һ������ʾ�ϸ������ս�������������տ���ʱ����������
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
				StrTime = StrStartTime;	// Լ�����н������ʾ
			}
		}

		CString StrMineTime;
		CRect rcNode(rcRegion);
		int32 iMines = 0;
		CString StrFirst;
		while ( it != m_mapLandMine.rend() && (StrFirst=it->first) >= StrTime )
		{
			// ��Ϣ���׵�ʱ�����ڵ��ڸ�ʱ���, �����ڸ�ʱ������
			//const CLandMineTitle &title = it->second;
			int32 iCenter = m_pRegionMain->m_aXAxisNodes[i].m_iCenterPixel;

			rcNode.left  = iCenter - 4;
			rcNode.right = iCenter + 4;

			bNeedDraw = true;
			StrMineTime = it->first;
			iMines++;

			m_mapLandMinesRect.insert(make_pair(it->first, rcNode));		// ������Ϣ���׵���ʱ�䶼��һ����

			++it;
		}

		if ( bNeedDraw )
		{
			CPoint ptCenter = rcNode.CenterPoint();
			CRect rcPixel(ptCenter.x-1, ptCenter.y-1, ptCenter.x+1, ptCenter.y+1);
			if ( iMines > 1 )
			{
				// �ж��������һ��
				rcPixel.top -= 1;
				rcPixel.left -= 1;
			}
			else
			{
				// ��Сһ��
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
			//pDC->DrawText(_T("*"), rcNode, DT_CENTER| DT_BOTTOM);	// Ҫ��Ҫ���Ƴ���С���أ�
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

	// ��ʷ��ʱ˫������Ӧ
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

	// �Ƿ�˫������Ϣ����
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
	// �õ�Tips
	StrTips = L"";

	CWnd *p1 = GetParentOwner();
	CWnd *p2 = CWnd::FromHandlePermanent(::GetLastActivePopup(AfxGetMainWnd()->GetSafeHwnd()));
	if ( m_bHistoryTrend
		&& p1 != p2 )
	{
		return false;	// ������㴰��û��active����ô�Ͳ�����ʾ��
	}

	// ��Ϣ����
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
							StrTitle = _T("ʵʱ��Ϣ����(˫���鿴)");
						}
						// ��������˳���
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

	ShowProperAvgLine();	// ����Ȩ��

	bool32 bIsRedGreenExp = IsRedGreenMerch(pData->m_pMerchNode);
	if ( bIsRedGreenExp )
	{
		// ��ͼ������
		if ( NULL != m_pRedGreenCurve )
		{
			RemoveFlag(m_pRedGreenCurve->m_iFlag, CChartCurve::KInVisible);	// ��ʾ
		}
	}
	else
	{
		if ( NULL != m_pRedGreenCurve )
		{
			AddFlag(m_pRedGreenCurve->m_iFlag, CChartCurve::KInVisible);	// ����
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
	// ֻ���ڷ���ʷ ���շ�ʱ ���ڹ�Ʊ�²Ż�
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
		// �ҵ���ʾ�ߵĵ�һ���ڵ�
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
			// ���Ƶ�һ���Ŀ��̼������̼�֮�������
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

	// ������ָ�������ݵĸ���
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
		return;	// û��������Ҫ���
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

	// ����Full����
	FillTrendNodes(*pData, m_pAbsCenterManager->GetServerTime(), pKLineCmp, iCmpKlineCount);

	// �������鹹�����ݣ����Բ���Ҫ����ָ�����ݣ�
	// ������ͼ�����ָ�� - ������ٵ��������⣬δ���
	int32 j = 0;
	for ( j = 0; j < pData->aIndexs.GetSize(); j++)
	{
		T_IndexParam* pIndex = pData->aIndexs.GetAt(j);
		g_formula_compute(this, this, (CChartRegion*)pIndex->pRegion, pData, pIndex, 0, pData->m_aKLinesFull.GetSize());
	}

	// ������ʾ����
	if (!pData->UpdateShowData(0, pData->m_aKLinesFull.GetSize()))
	{
		ASSERT( 0 );
		return;
	}

	// ����ָ�궼��������ݣ�������Щ���ݵ�flagΪinvalid
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
				// ȫ����Ч
				DWORD dwIndex = pNodesShow[iFlagIndex].m_iFlag;
				pNodesShow[iFlagIndex].m_iFlag = (dwIndex | CNodeData::KValueInvalid);
			}
		}
	}

	// �鹹���ݣ�ͬ������Ҫ�������vol��ɫ����
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

	// ����䵱�����Ҫ��ʾ�ĵ����ڵ��������
	// ��������
	const CMarketIOCTimeInfo &IOCTime = pData->m_TrendTradingDayInfo.m_MarketIOCTime;
	CGmtTime TimeStart, TimeEnd;
	if ( !GetTrendStartEndTime(TimeStart, TimeEnd) )
	{
		ASSERT( 0 );	// ��Чʱ��Σ���	
		return;
	}
	time_t tTimeStart = TimeStart.GetTime();
	time_t tTimeEnd	 = TimeEnd.GetTime();
	CGmtTime TimeTodayStart = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeOpen.m_Time;
	int32 iPos = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pData->m_aKLinesCompare, TimeTodayStart);
	if ( iPos >=0 && iPos < pData->m_aKLinesCompare.GetSize() )
	{
		// ��Ҫ�������ݱ�Ȼ�ǲ����ڽ���ķ���k�ߵ�
		// ��������ַ��ֿ����ǿ����ˣ�û�м�ʱ�Ĵ���
		//ASSERT( 0 );
		// ֻҪ�Ǵ��ڽ�������ݣ��򲻴���
		// ������Fillʱ���Ϳ��ܴ��������K�ߣ�so����ʾ
		//TRACE(_T("��ʱͼ���ܴ��ڲ���Ҫ�������K�ߣ������ֵ����˵����⣡\r\n"));
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
				continue;	// ������ʾ�����ڵģ������
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
			break;	// ������ǰʱ�䣬��Ҫ���������
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
		return false;	// ��ʷ��ʱ������ͣ��, ������k�������ͬ���ķ���k�߳��֣�so��������ʷ
	}

	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	if ( pApp->m_bOffLine )
	{
		return false;	// ����״̬����ͣ�Ƹ������û����, shit
	}

	ASSERT( pData->m_TrendTradingDayInfo.m_bInit );
	if ( 0.0f < GetTrendPrevClose()
		&& NULL != pData->m_pMerchNode->m_pRealtimePrice
		&& pData->m_pMerchNode->m_pRealtimePrice->m_fPriceOpen == 0.0f
		&& pData->m_pMerchNode->m_pRealtimePrice->m_fPriceHigh == 0.0f )
	{
		// �����գ��޿���
		// ���������ʱ���Ѿ����˿���ʱ�䣬����Ϊ����Ʒ��ͣ����Ʒ
		CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();
		CGmtTime TimeOpen = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeOpen.m_Time+CGmtTimeSpan(0,0,0,59);
		if ( TimeNow > TimeOpen )
		{
			return true;	// ��������ʱ�仹û�п������ݣ�����Ϊ��ͣ����
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
					// ȫ����Ч
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

	// ���µ�����json���Ĳƾ��������ݶ�����һ�ݣ����ã��ڷ�ʱͼ���ڴ�С�ı�ʱ����Ҫ�õ���
	m_strEconoData = pszRecvData;

	// ����һ������Ĳƾ��������ݱ��浽����������
	CMainFrame *pMainFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	if (pMainFrame->m_strFirstEconoData.empty())
	{
		pMainFrame->m_strFirstEconoData = pszRecvData;
	}

	// ���û�б����ʱͼ��ʱ��̶ȣ�����Ҫ�����ƾ��������ݣ��������������Ҳ������ʾ�ģ���ΪҪ��ʾ�Ĳƾ���������Ҫ������ʱͼ��ʱ��̶ȣ�
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
		return;// ����ʧ��
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

	// �����ʱͼ�ϵĿ�ʼʱ�䣨����forѭ�����ڹ��˵�С�ڸ�ʱ��Ĳƾ��������ݣ�
	T_XAxisInfo stXAxis;
	if (!m_vecXAxisInfo.empty())
	{
		stXAxis =  *m_vecXAxisInfo.begin();
	}

	std::vector<T_EconomicData>::iterator iter;
	for (iter = vecEconomic.begin() ; iter != vecEconomic.end() ; )
	{
		// ��ʱ��С�ڷ�ʱͼ�п�ʼʱ��Ĳƾ��������ݹ��˵�
		int hour = 0, minute = 0, second = 0;
		sscanf(iter->release_time.c_str(),"%d:%d:%d", &hour, &minute, &second);
		if (stXAxis.hour > hour)
		{
			iter = vecEconomic.erase(iter);
			continue;
		}

		// ȥ����Ҫ�ȼ�Ϊ���ͻ��С��Ĳƾ���������
		if (iter->important_level.compare("��") == 0)
		{
			iter = vecEconomic.erase(iter);
		}
		else if (iter->important_level.compare("��") == 0)
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

			// ����ʱ�䣬�ϲ��ƾ���������
			for (itBegin = vecEconomic.begin() ; itBegin != vecEconomic.end() ; )
			{
				std::vector<T_EconomicData> vecMergeEconomic;// ����ϲ�������
				int nSingleCount = 0;// ͳ�ƺϲ��������У��ж����ƾ��������ݣ�ֻ������������������е�һ��
				int nBothCount = 0;// ͳ�ƺϲ��������У��ж����ƾ��������ݣ��Ȱ������࣬Ҳ��������

				int count = 0;// ͳ�Ƶ�ǰѭ���У��ж��ٲƾ��������ݱ��ϲ�������count = nBothCount + nSingleCount��

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

					// ���Сʱ��ȣ���ô�ͱȽϷ������Ƿ񳬹�CONTINUEATION_MINUTES���ӣ�
					// ��������CONTINUEATION_MINUTES���ӵ����ݺϲ���һ��
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

		// �ƾ��������ݺϲ���󣬼���СԲ�����Ϣ����x���꣬�뾶��
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
		std::string strInfluence = strLeft.substr(0,4);// ȡǰ���������֣�Ҫô�����࣬Ҫô�����գ�����ͬ��
		if (strInfluence.compare("����") == 0)
		{
			strLD = strLeft;
		}
		else if (strInfluence.compare("����") == 0)
		{
			strLK = strLeft;
		}

		strInfluence = strRight.substr(0, 4);
		if (strInfluence.compare("����") == 0)
		{
			strLD = strRight;
		}
		else if (strInfluence.compare("����") == 0)
		{
			strLK = strRight;
		}
	}
	else
	{
		bBoth = false;

		std::string strInfluence = str.substr(0, 4);
		if (strInfluence.compare("����") == 0)
		{
			strLD = str;
		}
		else if (strInfluence.compare("����") == 0)
		{
			strLK = str;
		}
		else
		{
			strPlaceholder = str;// ������������һ����Ч���ַ��������磺--��
		}
	}
}

void CIoViewTrend::SaveTimeDegreeScale(int xPos, CString strTime)
{
	T_XAxisInfo stCoordInfo;
	stCoordInfo.pt.x = xPos;	// ʱ���x����
	stCoordInfo.pt.y = 30;		// СԲ����ʾʱ��y����

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

	// СԲ��İ뾶
	m_radius = 9;

	if (NULL == m_hWnd)
	{
		return;
	}

	std::map<CString, T_EconoInfo > &mapEconomicData = m_dlgEconomicTip.m_mapEconomicData;
	std::map<CString, T_EconoInfo >::iterator iter;
	for (iter = mapEconomicData.begin() ; iter != mapEconomicData.end() ; ++iter)// ����ÿһ������ʱ�䣬��ȷ���÷���ʱ����x���λ��
	{
		int hour1 = 0, minute1 = 0, second = 0;
		std::string strTime;
		UnicodeToGbk32(iter->first, strTime);
		sscanf(strTime.c_str(),"%d:%d:%d", &hour1, &minute1, &second);

		// ���㵱ǰ�ƾ�����СԲ����x���λ��
		int posX = -1;
		std::vector<T_XAxisInfo>::iterator it, itNext;
		for (it = m_vecXAxisInfo.begin() ; it != m_vecXAxisInfo.end() ; ++it)
		{
			itNext = it;
			++itNext;

			if (itNext != m_vecXAxisInfo.end())
			{
				// �����ǰ�ƾ�������ʱ�����ڸ�ʱ����ڣ��ͼ����ʱ����ڵ�λʱ�䳤��
				if (hour1 >= it->hour && hour1 < itNext->hour)
				{
					// ��ǰ�ƾ�����ʱ�䵽itʱ�䣬�������Сʱ
					int nHours = hour1 - it->hour;

					// ��ǰ�ƾ�����ʱ�䵽itʱ�䣬������ٷ���
					int nMinutes = (int)(nHours * 60 + myFabs(minute1 - it->mintues));

					// ����itNextʱ�䵽itʱ�䣬������ٷ���
					int nMinutes1 = (int)((itNext->hour - it->hour) * 60 + myFabs(itNext->mintues - it->mintues));

					// ����itNextʱ�䵽itʱ�䣬����೤
					int nSpacing = itNext->pt.x - it->pt.x;

					if(0 != nMinutes1)
					{
						// ����1���ӵ�ʱ�䳤��
						float unitLen = nSpacing / (nMinutes1*1.0);

						// �ƾ�����Ҫ��ʾ��x���꣨-4��Ŀ�ģ���������-4���Ǽ���4�����ȵ�����ע��ֻ�ܼ�С������������
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

		// ����СԲ���Ӧ�ķ���ʱ��
		stCircleInfo.strTime = iter->first;

		// ����ÿһ������ʱ���СԲ�����ʾ����
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
	// ����Ƕ��Լͬ�н��棬�Ͳ�����СԲ��
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

	// ����Ƕ��շ�ʱͼ����ô�����Ʋƾ�����СԲ��
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
	// ����ɽ�����ͼ
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
	// ��Ҫ�ȶ�tmd ɾ����? fuck in the hole
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
	// ɾ���ɽ�����ͼ
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
		return; // Ȩ�޲���
	}

	// �����ǰָ��������ڵ�ָ�꣬���ò���
	// ������ʲô����ָ��ʲô�ģ������󣬲������
	// ����Ƿ���ڸ�index - ��ô���ж��أ���
	for ( int32 i=0; i < pData->aIndexs.GetSize() ; i++ )
	{
		// ����ʵ�ʴ���indexҲ�Ǹ���name��������so���ж�����+region
		if ( pData->aIndexs[i]->strIndexName == StrIndexName && pRegion == pData->aIndexs[i]->pRegion )
		{
			return;
		}
	}
	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);

	// pSubRegion�ѱ�ɾ������Ӧ������
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
		// ͣ�����ݵ�ָ����Ҫ�ض��������
		TrimSuspendedIndexData();
	}

	UpdateSelfDrawCurve();
	Invalidate();
}

//��ȡҪ��ʾ�����λ��
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

	// ���ߵ�Y ����
	CRect RectMain = m_pRegionMain->GetRectCurves();	
	int32 iYCenter = RectMain.CenterPoint().y;

	// ���¼۶�Ӧ��Y ����
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
		//û�����¼ۻ���������
		bNoPriceNew = true;
	}
	else
	{
		m_pRegionMain->RegionYToClient(iYPrice);
	}

	//
	if (iYCenter <= iYPrice || bNoPriceNew)
	{
		// ������
		rtPos.top = RectMain.top + RectMain.Height() / 4 - iHeight / 2;		
	}
	else
	{
		// ������
		rtPos.top = RectMain.bottom - RectMain.Height() / 4 - iHeight / 2;
	}

	rtPos.bottom = rtPos.top + iHeight;		
	rtPos.right = RectMain.right;
	rtPos.left  = rtPos.right - iWidth;

	ClientToScreen(rtPos);
	return true;
}

//��û�����¼ۣ���Щ����ʱ��������ʾ����
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

	// ������Ǵ�����Ʒ���������������Ҫ����
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

					// ��ȡ��ʱ���ڵĴ�С
					CRect rtTrend;
					GetWindowRect(rtTrend);

					// -45Ŀ�ģ�ָ����ʾ��ʱ��̶���ʾ���ܸ߶ȴ����45��-45�Ϳ����ó����ĸø߶ȵĲ��־Ͳ�����ʾ��
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

					m_dlgEconomicTip.InitChildWindow(count, stEconoInfo.vecEconomicData);// ��ʼ����ʾ����Ӵ���
					m_dlgEconomicTip.ShowWindow(SW_SHOW);
				}
			}
		}

		if (!bRectIn)
		{
			// ����Ƴ�СԲ�㣬�ͷ���ʾ���е��Ӵ���
			m_dlgEconomicTip.ReleaseChildWindow();

			// ������ʾ��
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
	// ���òƾ�������������URL��ַ
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if (pDoc && pDoc->m_pAutherManager)
	{
		std::string strURL;

		// ��ȡ�ƾ�������������ַ
		pDoc->GetEconoServerURL(strURL);

		CString StrUrl = strURL.c_str();

		// ��ʼ������Ĳƾ���������
		SYSTEMTIME sys; 
		GetLocalTime( &sys );

		char szTime[32] = {0};
		sprintf(szTime, "%d-%d-%d", sys.wYear, sys.wMonth, sys.wDay);

		// ����ƾ���������
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
		s_aStrs.Add(_T("�ɽ���"));
		s_aStrs.Add(_T("�ɽ���"));
		s_aStrs.Add(_T("ָ��"));
		s_aStrs.Add(_T("����"));
		s_aStrs.Add(_T("�羯��"));
		s_aStrs.Add(_T("��������"));
		s_aStrs.Add(_T("����ͼ"));
		s_aStrs.Add(_T("���ָ��"));
		s_aStrs.Add(_T("�ǵ���"));
		s_aStrs.Add(_T("�۸�ģ��"));
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
	m_Font.m_StrName = gFontFactory.GetExistFontName(L"����");
	m_Font.m_Size	 = 9.7;
	m_Font.m_iStyle	 = FontStyleRegular;	
	m_btnPolygon.SetFont(m_Font);
}

void T_TrendBtn::SetParent(CWnd *pParent)
{
	m_btnPolygon.SetParentWnd(pParent);
}

//--- wangyongxue ��ʾָ��˵�
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

		// ��point�Ƿ����ѻ��Ƶİ�ť������
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
	//	��ѯ�Ƿ���ھ��۲˵�
	int iBtnIndex = -1;
	for (unsigned i =0; i < m_aTrendBtnCur.GetSize(); ++i)
	{		
		if (ETBT_CompetePrice + BTN_ID_BEGIN == m_aTrendBtnCur[i].m_iID)
		{
			iBtnIndex = i;
			break;
		}
	}

	//	����Ѿ�ѡ�У����������
	if (m_iBtnCur == iBtnIndex)
	{
		bShow = true;
	}

	return bShow;
}
