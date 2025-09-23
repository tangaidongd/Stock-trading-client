#include "stdafx.h"
#include "MPIChildFrame.h"
#include "GridCtrlSys.h"
#include "GridCellSys.h"
#include "GridCellSymbol.h"
#include "guidrawlayer.h"
#include "NewMenu.h"
#include "memdc.h"
#include "MerchManager.h"
#include "facescheme.h"
#include "IoViewKLine.h"
#include "IoViewExpTrend.h"
#include "IoViewManager.h"
#include "ShareFun.h"
#include "Region.h"
#include "ChartRegion.h"
#include "CFormularContent.h"
#include <float.h>
#include "GmtTime.h"
#include "ChartDrawer.h"

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
							//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


///////////////////////////////////////////////////////
// CIoViewExpTrend

const CString KStrTrendFS = _T(" ��ʱ����");
const CString KStrTrendJun= _T("����");
const CString KStrTrendVol= _T("�ɽ���");
const CString KStrTrendHold = _T("�ֲ���");
//////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
const T_FuctionButton s_astTrendSubRegionButtons[] =
{
	T_FuctionButton(L"+",		L"���Ӹ�ͼ",		REGIONTITLEADDBUTTONID,		IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE),
	T_FuctionButton(L"��",		L"ɾ����ǰ��ͼ",	REGIONTITLEDELBUTTONID,		IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE),
};

// ���ܰ�ť����
const int KiTrendSubRegionButtonNums = sizeof(s_astTrendSubRegionButtons)/sizeof(T_FuctionButton);
///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CIoViewExpTrend, CIoViewChart)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewExpTrend, CIoViewChart)
	//{{AFX_MSG_MAP(CIoViewExpTrend)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_COMMAND_RANGE(IDM_CHART_BEGIN,IDM_CHART_END,OnMenu)
	ON_COMMAND_RANGE(IDT_SD_BEGIN,IDT_SD_END,OnMenu)
	ON_MESSAGE(UM_IoViewTitle_Button_LButtonDown,OnMessageTitleButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewExpTrend::CIoViewExpTrend()
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
	m_iChartType		  = 3;

	m_iOpenTimeForNoData  = 0;
	m_iCloseTimeForNoData = 0;
	
	m_bHistoryTrend		  = false;
	m_TimeHistory		  = CGmtTime(1971,1,1,1,1,1);
		
	m_aYAxisDivideBkMain.RemoveAll();
	m_aYAxisDivideBkSub.RemoveAll();

	m_fMinHold = m_fMaxHold = 0.0f;
	m_pHoldNodes = NULL;
}

//lint --e{1540}
CIoViewExpTrend::~CIoViewExpTrend()
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
}
///////////////////////////////////////////////////////////////////////////////
int CIoViewExpTrend::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CIoViewChart::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

void CIoViewExpTrend::InitCtrlFloat ( CGridCtrl* pGridCtrl )
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

void CIoViewExpTrend::OnTimer(UINT nIDEvent) 
{
	if(!m_bShowNow)
	{
		return;
	}

	if (NULL == m_pMerchXml)
		return;
	
	if ( nIDEvent == KExpTrendHeartBeatTimerId &&
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

	CIoViewChart::OnTimer(nIDEvent);
}

void CIoViewExpTrend::CreateRegion()
{
	ASSERT ( NULL == m_pRegionMain );
	CIoViewChart::CreateRegion();
	UpdateGDIObject();

	///////////////////////////////////////////////////////////////////////////////////////////////
	//Trend
	new CChartRegion ( this,m_pRegionMain,_T("Trend"), CChartRegion::KChart|CChartRegion::KCrossAble);
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

void CIoViewExpTrend::SplitRegion()
{
	CIoViewChart::SplitRegion();
	UpdateGDIObject();
}

int32 CIoViewExpTrend::GetSubRegionTitleButtonNums()
{
	return KiTrendSubRegionButtonNums;
}

const T_FuctionButton* CIoViewExpTrend::GetSubRegionTitleButton(int32 iIndex)
{
	if( iIndex < 0 || iIndex >= GetSubRegionTitleButtonNums() )
	{
		return NULL;
	}
	
	return &s_astTrendSubRegionButtons[iIndex];
}

void CIoViewExpTrend::OnDestroy() 
{
	ReleaseMemDC();
	CIoViewChart::OnDestroy();
}

void CIoViewExpTrend::RequestViewData()
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
		
		// ����ָ��ʱ�����K�ߣ� �Ա��ȡ��ǰ��ʾ����ͼ�����ռۣ� �񿪼�
		{
			ASSERT(pData->m_TrendTradingDayInfo.m_bInit);
			
			// 
			// ������ͽ������k������

			// ����
			CGmtTime TimeDay = pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeClose.m_Time;
			SaveDay(TimeDay);
			
			CMmiReqMerchKLine info;
			info.m_eKLineTypeBase	= EKTBDay;
			info.m_iMarketId		= pData->m_pMerchNode->m_MerchInfo.m_iMarketId;
			info.m_StrMerchCode		= pData->m_pMerchNode->m_MerchInfo.m_StrMerchCode;
			info.m_eReqTimeType		= ERTYFrontCount;
			info.m_TimeStart		= TimeDay;
			info.m_iFrontCount			= 2;			
			DoRequestViewData(info);
		}
		
		//�ȷ�RealtimePrice����
		{
			CMmiReqRealtimePrice Req;
			Req.m_iMarketId			= pData->m_pMerchNode->m_MerchInfo.m_iMarketId;
			Req.m_StrMerchCode		= pData->m_pMerchNode->m_MerchInfo.m_StrMerchCode;			
			DoRequestViewData(Req);
		}
		
		// ���ͷ���������
		{
			CMmiReqMerchKLine info;
			info.m_eKLineTypeBase	= EKTBMinute;
			info.m_iMarketId		= pData->m_pMerchNode->m_MerchInfo.m_iMarketId;
			info.m_StrMerchCode		= pData->m_pMerchNode->m_MerchInfo.m_StrMerchCode;
			info.m_eReqTimeType		= ERTYSpecifyTime;
			info.m_TimeStart		= pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time;
			info.m_TimeEnd			= pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeEnd.m_Time;			
			DoRequestViewData(info);
		}
	}
}

CGmtTime CIoViewExpTrend::GetRequestNodeTime(IN CMerch* pMerch)
{
	if(NULL == m_pAbsCenterManager)
	{
		return CGmtTime(0);
	}

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
void CIoViewExpTrend::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if ( NULL == pMerch || m_pMerchReal == pMerch )
	{
		return;
	}

	//
	m_pMerchReal = pMerch;

	CMerch* pExpMerch = GetMerchAccordExpMerch(pMerch);
	if ( NULL == pExpMerch )
	{
		return;
	}

	//
	if (m_pMerchXml == pExpMerch)
		return;

	//
	m_pMerchXml = pExpMerch;

	if (NULL != m_pMerchXml)
	{
		//////////////////////////////////////////////////////////////////////////		
		m_aSmartAttendMerchs.RemoveAll();
		
		// 
		CSmartAttendMerch AttendMerch;
		AttendMerch.m_pMerch = m_pMerchXml;
		AttendMerch.m_iDataServiceTypes = m_iDataServiceType;
		m_aSmartAttendMerchs.Add(AttendMerch);

		//
		ASSERT(NULL!=m_pRegionMain);
		if ( NULL == m_pAbsCenterManager )
		{
			return;
		}

		bool32 bInhert;
		T_MerchNodeUserData* pData = NewMerchData(m_pMerchXml, true, bInhert);

		// 
		pData->m_eTimeIntervalFull = ENTIMinute;

		// ��ȡ����Ʒ������ص�����
		CMarketIOCTimeInfo RecentTradingDay;
		if ( !m_bHistoryTrend )
		{
			if (m_pMerchXml->m_Market.GetRecentTradingDay(m_pAbsCenterManager->GetServerTime(), RecentTradingDay, pMerch->m_MerchInfo))
			{
				pData->m_TrendTradingDayInfo.Set(m_pMerchXml, RecentTradingDay);
				pData->m_TrendTradingDayInfo.RecalcPrice(*m_pMerchXml);
				pData->m_TrendTradingDayInfo.RecalcHold(*m_pMerchXml);
				
				// zhangbo 20090708 #�����䣬 �������
				//...
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
		m_pRegionMain->SetTitle(m_pMerchXml->m_MerchInfo.m_StrMerchCnName);
		if ( NULL != m_pCell1 )
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
		
		// 
		if ( !bInhert )
		{
			CNodeSequence* pNodes = CreateNodes();
			pNodes->SetUserData(pData);
			pData->m_pKLinesShow = pNodes;

			CChartCurve* pCurvePrice = m_pRegionMain->CreateCurve(CChartCurve::KDependentCurve|CChartCurve::KRequestCurve|CChartCurve::KTypeTrend|CChartCurve::KYTransformByClose);
			pCurvePrice->SetTitle(KStrTrendFS);//��ʱ����
			CChartDrawer* pTrendDrawer = new CChartDrawer(*this, CChartDrawer::EKDSTrendPrice);
			pCurvePrice->m_clrTitleText = GetIoViewColor(ESCGridLine);
			m_pTrendDrawer = pTrendDrawer;
			
			pCurvePrice->AttatchDrawer(pTrendDrawer);
			pCurvePrice->AttatchNodes(pNodes);

			CChartCurve *pCurveAvg = m_pRegionMain->CreateCurve(CChartCurve::KTypeTrend | CChartCurve::KYTransformByAvg);
			pCurveAvg->SetTitle(KStrTrendJun);
			pCurveAvg->m_clrTitleText  = GetIoViewColor(ESCVolume);				
			CChartDrawer* pJunDrawer   = new CChartDrawer(*this, CChartDrawer::EKDSTrendJunXian);

			pCurveAvg->AttatchDrawer(pJunDrawer);
			//DEL(pJunDrawer);
			pCurveAvg->AttatchNodes(pNodes);
		}

		if ( m_IndexPostAdd.id >= 0 )
		{
			m_IndexPostAdd.id = -1;
			AddIndex(m_IndexPostAdd.pRegion,m_IndexPostAdd.StrIndexName);
		}

		//////////////////////////////////////////////////////////////////////////
		// merchklineupdate	
		OnVDataMerchKLineUpdate(m_pMerchXml);

		//////////////////////////////////////////////////////////////////////////
		KillTimer(KExpTrendHeartBeatTimerId);
		SetTimer(KExpTrendHeartBeatTimerId, 1000*60, NULL);
	}
	else
	{
		// zhangbo 20090824 #�����䣬 ������Ʒ������ʱ����յ�ǰ��ʾ����
		//...
	}
}

// ������ͼ���
bool32 CIoViewExpTrend::OnVDataAddCompareMerch(IN CMerch *pMerch)
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

void CIoViewExpTrend::SetCurveTitle ( T_MerchNodeUserData* pData )
{
	CString StrTitle;
	if ( !pData->bMainMerch )
	{
		StrTitle = pData->m_pMerchNode->m_MerchInfo.m_StrMerchCode + _T("(��)");
		pData->m_pKLinesShow->SetName(StrTitle);
	}
	CMerch* pMerch = pData->m_pMerchNode;
	if ( NULL == pMerch )
	{
		return;
	}
	if ( !pData->bMainMerch)
	{
		StrTitle = pMerch->m_MerchInfo.m_StrMerchCnName + _T("(����)");
		pData->m_pKLinesShow->SetName(StrTitle);
	}
	else
	{
		pData->m_pKLinesShow->SetName(pMerch->m_MerchInfo.m_StrMerchCnName);
	}
}

void CIoViewExpTrend::OnVDataForceUpdate()
{
	if ( NULL == m_pRegionMain || NULL == m_pAbsCenterManager)
	{
		return;
	}

	// zhangbo 20090707 #�����䣬 ���¼��㽻���գ� �п����Ǹ����ʼ���� ��Ҫ���»�����ʾ��
	//...
	if ( m_MerchParamArray.GetSize() > 0 )
	{		
		T_MerchNodeUserData * pData = m_MerchParamArray.GetAt(0);
		if (NULL == pData)
		{
			return;
		}

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
			OnVDataMerchKLineUpdate(m_pMerchXml);
		}
	}

	//
	CIoViewChart::OnVDataForceUpdate();
	RequestViewData();

	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	ReDrawAysnc();
}

bool32 CIoViewExpTrend::ServerTimeInOpenCloseTime ( CMerch* pMerch )
{
	bool32 bFound = false;
	if ( pMerch && m_pAbsCenterManager)
	{
		CGmtTime ServerTime = m_pAbsCenterManager->GetServerTime();
		T_OpenCloseTime tOctime;
		int32 i,iSize = pMerch->m_Market.m_MarketInfo.m_OpenCloseTimes.GetSize();
		for ( i = 0; i < iSize; i ++ )
		{
			tOctime = pMerch->m_Market.m_MarketInfo.m_OpenCloseTimes.GetAt(i);
			int32 iHourOpen = tOctime.m_iTimeOpen/60;
			int32 iMinuteOpen = tOctime.m_iTimeOpen%60;
			int32 iHourClose = tOctime.m_iTimeClose/60;
			int32 iMinuteClose = tOctime.m_iTimeClose%60;
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

void CIoViewExpTrend::OnVDataFormulaChanged ( E_FormulaUpdateType eUpdateType, CString StrName )
{
	CIoViewChart::OnVDataFormulaChanged ( eUpdateType, StrName );
	OnVDataMerchKLineUpdate (m_pMerchXml);
}

void CIoViewExpTrend::OnIoViewActive()
{ 
	CIoViewChart::OnIoViewActive();
}

void CIoViewExpTrend::OnKeyHome()
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

void CIoViewExpTrend::OnKeyEnd()
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

void CIoViewExpTrend::OnKeyLeft()
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

void CIoViewExpTrend::OnKeyRight()
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

void CIoViewExpTrend::OnVDataMerchKLineUpdate(IN CMerch *pMerch)
{
	if (NULL == pMerch || pMerch != m_pMerchXml)
		return;

	//if (!IsWindowVisible())
	//{
	//	// 0002149 xl 0705 ����ڼ���󣬶�ʱ���ڱ�����GGTongView��F7�������Ϊ���أ��ᵼ�»�ԭʱ��������δ���¶�������ݵ���ʾ������ȡ�����Ż�
 //		AddFlag(m_uiFlag,CIoViewChart::KMerchKLineUpdate);
	//	ClearLocalData();
 //		return;
	//}
	
	// ������Ʒ������������
	for (int32 i = 0; i < m_MerchParamArray.GetSize(); i ++)
	{
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(i);
		SetCurveTitle(pData);
		
		if (pData->m_pMerchNode != pMerch)
			continue;
		
		if (0 == i)	// ����Ʒ
		{
			if (UpdateMainMerchKLine(*pData))
			{
				ReDrawAysnc();	
			}
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

void CIoViewExpTrend::OnVDataRealtimePriceUpdate(IN CMerch *pMerch)
{
	if (NULL == pMerch || pMerch != m_pMerchXml)
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
		SetCurveTitle(pData);
		
		if (pData->m_pMerchNode != pMerch)
			continue;
		
		// ��Ҫ�������ռ�
		if (0. == pData->m_TrendTradingDayInfo.m_fPricePrevClose && 0. == pData->m_TrendTradingDayInfo.m_fPricePrevAvg)
		{
			CMerch* pTmpMerch = pData->m_pMerchNode;
			if (NULL == pTmpMerch)
			{
				//ASSERT(0);
				continue;
			}
			
			// ���Ը���������صı�������
			float fPricePrevClose = pData->m_TrendTradingDayInfo.GetPrevReferPrice();
			pData->m_TrendTradingDayInfo.RecalcPrice(*pTmpMerch);
			if (fPricePrevClose != pData->m_TrendTradingDayInfo.GetPrevReferPrice())
			{
				// zhangbo 20090708 #������, ��������ֵ
				//...				
			}
		}
	}
}

void CIoViewExpTrend::OnRequestPreNodeData ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes, int32 id, int32 iNum, bool32 bSendRequest )
{

}

void CIoViewExpTrend::OnRequestNextNodeData ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes, int32 id, int32 iNum, bool32 bSendRequest )
{

}

void CIoViewExpTrend::OnNodesRelease ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes )
{
	/*T_MerchNodeUserData* pData = (T_MerchNodeUserData*)pNodes->GetUserData();*/
	//pData->pMerch�Ѿ��������Curveʹ����,���û������Curveʹ��,�����������,���Ҵ�AttendMerch��ɾ��.
}

void CIoViewExpTrend::OnRegionMenu2 ( CChartRegion* pRegion, CDrawingCurve* pCurve, int32 x, int32 y )
{
	RegionMenu2 ( pRegion, pCurve, x, y );
}

void CIoViewExpTrend::OnRegionIndexMenu ( CChartRegion* pRegion, CDrawingCurve* pCurve, int32 x, int32 y )
{
	RegionIndexMenu ( pRegion, pCurve, x, y );
}

void CIoViewExpTrend::OnRegionIndexBtn(CChartRegion* pRegion, CDrawingCurve* pCurve, int32 iID )
{
	RegionIndexBtn (pRegion, pCurve, iID);
}

void CIoViewExpTrend::OnRegionMenu ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes,CNodeData* pNodeData, int32 x, int32 y)
{
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
			iRegion = 1;
		}
	}
	if ( iRegion < 0 )
	{
		return;
	}

	m_pCurvePick	= (CChartCurve*)pCurve;
	m_pNodesPick	= pNodes;
	m_NodePick		= CNodeData(*pNodeData);

	DoShowStdPopupMenu();

// 	CPoint pt(x,y);
// 	ClientToScreen(&pt);
// 
// 	CNewMenu menu;
// 	menu.LoadMenu(IDR_MENU_TICK);
// 	menu.LoadToolBar(g_awToolBarIconIDs);
// 
// 	CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
// 	CMenu *pTempMenu = NULL;
// 	
// 	//////////////////////////////////////////////////////////////////////////
// 	// ��������:
// 	pTempMenu = pPopMenu->GetSubMenu(L"��������");
// 	CNewMenu * pIoViewPopMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );
// 	ASSERT(NULL != pIoViewPopMenu );
// 	AppendIoViewsMenu(pIoViewPopMenu);
// 	//
// 
// 	// ������������ָ�״̬����Ҫɾ��һЩ��ť
// 	CMPIChildFrame *pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
// 	if ( NULL != pChildFrame && pChildFrame->IsLockedSplit() )
// 	{
// 		pChildFrame->RemoveSplitMenuItem(*pPopMenu);
// 	}
// 
// 	pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, AfxGetMainWnd());
// 	menu.DestroyMenu();
}

void CIoViewExpTrend::OnMenu ( UINT nID )
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
	default:
		break;
	}
	if ( nID >= IDC_CHART_ADD2BLOCK_BEGIN && nID < IDC_CHART_ADD2BLOCK_END )
	{
		CString StrBlockName = m_aUserBlockNames.GetAt(nID-IDC_CHART_ADD2BLOCK_BEGIN);
		Add2Block(StrBlockName);
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
}

LRESULT CIoViewExpTrend::OnMessageTitleButton(WPARAM wParam,LPARAM lParam)
{
	int32 uID =(int32)wParam;

	if ( 0 == m_MerchParamArray.GetSize() )
	{
		return 0 ;
	}
	
	if ( NULL == m_pRegionPick)
	{
		return 0;
	}

	if ( 0 == uID)
	{
		// ָ��:
		// ָ��˵�
/*		int32 iIndexNums = 0;

		m_pRegionPick = m_pRegionMain;	
		m_FormulaNames.RemoveAll();
		CNewMenu menu;
		menu.LoadMenu(IDR_MENU_TREND);
		CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(0));
		
		CMenu* pTempMenu = pPopMenu->GetSubMenu(_T("����ָ��"));
		ASSERT( NULL != pTempMenu);
		CNewMenu* pSubMenu = DYNAMIC_DOWNCAST(CNewMenu,pTempMenu);
		
		pSubMenu->RemoveMenu(0,MF_BYPOSITION|MF_SEPARATOR);//ɾ��β��
		
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
		
		if (NULL == pData)
		{
			return 0;				
		}
		
		uint32 iFlag = CFormularContent::KAllowTrend;
		AddFlag ( iFlag, CFormularContent::KAllowMain );
		
		CFormulaLib::instance()->GetAllowNames(iFlag,m_FormulaNames);
		CStringArray Formulas;
		FindRegionFormula(m_pRegionPick,Formulas);
		DeleteSameString ( m_FormulaNames,Formulas);
		Formulas.RemoveAll();

		int32 iSeperatorIndex = 0;	
		SortIndexByVol(m_FormulaNames,iSeperatorIndex);

		for (int32 i = 0; i < m_FormulaNames.GetSize(); i ++ )
		{
			if ( i >=(IDM_CHART_INDEX_END-IDM_CHART_INDEX_BEGIN) ) break;

			if ( -1 != iSeperatorIndex && i == iSeperatorIndex && 0 != iSeperatorIndex)
			{
				pSubMenu->AppendODMenu(L"",MF_SEPARATOR);
			}
			iIndexNums += 1;
			pSubMenu->AppendODMenu(m_FormulaNames.GetAt(i),MF_STRING,IDM_CHART_INDEX_BEGIN+i);
		}
		
		CPoint pt;
		GetCursorPos(&pt);
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, this);
		menu.DestroyMenu();

		if ( 0 == iIndexNums )
		{
			AfxMessageBox(L"��ǰ�޿���ָ��,����ָ�깫ʽ������!");
		}
*/
	}
	else if ( REGIONTITLELEFTBUTTONID == uID)
	{
		// ǰһ��ָ��
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
		
		if (NULL == pData)
		{
			return 0;				
		}

		uint32 iFlag = NodeTimeIntervalToFlag ( pData->m_eTimeIntervalFull );
		AddFlag ( iFlag, CFormularContent::KAllowSub );
		
		CFormulaLib::instance()->GetAllowNames(iFlag,m_FormulaNames);
		CStringArray Formulas;
		FindRegionFormula(m_pRegionPick,Formulas);
		
		int32 iIndex = 0;
		if ( 0 == Formulas.GetSize())
		{
			// ��һ��:
			iIndex = 0; // ǰһ����ID��
		}
		else
		{
			CString StrIndexNow = Formulas.GetAt(0);

			for ( int32 i = 0 ; i < m_FormulaNames.GetSize(); i++)
			{
				if ( StrIndexNow == m_FormulaNames.GetAt(i))
				{
					if ( 0 == i )
					{
						return 0;
					}
					else
					{
						iIndex = i-1; // ǰһ����ID��
					}
				}
			}
		}
		
		MenuAddIndex(iIndex);
		
	}
	else if ( REGIONTITLEFOMULARBUTTONID == uID)
	{
		// ��Region ��ָ�갴ť
/*		if ( NULL == m_pRegionPick )
		{
			return 0;
		}

		m_FormulaNames.RemoveAll();
		CNewMenu menu;
		menu.LoadMenu(IDR_MENU_TREND);
		CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(0));
		
		CMenu* pTempMenu = pPopMenu->GetSubMenu(_T("����ָ��"));
		ASSERT( NULL != pTempMenu);
		CNewMenu* pSubMenu = DYNAMIC_DOWNCAST(CNewMenu,pTempMenu);
		
		pSubMenu->RemoveMenu(0,MF_BYPOSITION|MF_SEPARATOR);//ɾ��β��
		
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
		
		if (NULL == pData)
		{
			return 0;				
		}
		
		uint32 iFlag = NodeTimeIntervalToFlag ( pData->m_eTimeIntervalFull );
		AddFlag ( iFlag, CFormularContent::KAllowSub );
		
		CFormulaLib::instance()->GetAllowNames(iFlag,m_FormulaNames);
		CStringArray Formulas;
		FindRegionFormula(m_pRegionPick,Formulas);
		DeleteSameString ( m_FormulaNames,Formulas);
		Formulas.RemoveAll();

		int32 iSeperatorIndex = 0;	
		SortIndexByVol(m_FormulaNames,iSeperatorIndex);

		for (int32 i = 0; i < m_FormulaNames.GetSize(); i ++ )
		{
			if ( i >=(IDM_CHART_INDEX_END-IDM_CHART_INDEX_BEGIN) ) break;
			
			if ( -1 != iSeperatorIndex && i == iSeperatorIndex && 0 != iSeperatorIndex)
			{
				pSubMenu->AppendODMenu(L"",MF_SEPARATOR);
			}

			CString StrName = m_FormulaNames.GetAt(i);
			pSubMenu->AppendODMenu(m_FormulaNames.GetAt(i),MF_STRING,IDM_CHART_INDEX_BEGIN+i);
		}
		
		CPoint pt;
		GetCursorPos(&pt);
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, this);
		menu.DestroyMenu();
*/
	}
	else if ( REGIONTITLERIGHTBUTTONID == uID)
	{
		// ��һ��ָ��

		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
		
		if (NULL == pData)
		{
			return 0;				
		}
		
		uint32 iFlag = NodeTimeIntervalToFlag ( pData->m_eTimeIntervalFull );
		AddFlag ( iFlag, CFormularContent::KAllowSub );
		
		CFormulaLib::instance()->GetAllowNames(iFlag,m_FormulaNames);
		CStringArray Formulas;
		FindRegionFormula(m_pRegionPick,Formulas);
		
		int32 iIndex = 0;
		if ( 0 == Formulas.GetSize())
		{
			// ��һ��:
			iIndex = 0; // ǰһ����ID��
		}
		else
		{
			CString StrIndexNow = Formulas.GetAt(0);
			
			for ( int32 i = 0 ; i < m_FormulaNames.GetSize(); i++)
			{
				if ( StrIndexNow == m_FormulaNames.GetAt(i))
				{
					if ( m_FormulaNames.GetSize() -1 == i )
					{
						return 0;
					}
					else
					{
						iIndex = i+1; // ǰһ����ID��
					}
				}
			}
		}
		
		MenuAddIndex(iIndex);
	}
	else if ( REGIONTITLEADDBUTTONID == uID)
	{
		// ���Ӹ�ͼ
		MenuAddRegion();
	}
	else if ( REGIONTITLEDELBUTTONID == uID)
	{	
		// �ر���Region
		if ( NULL != m_pRegionPick )
		{
			MenuDelRegion();
		}		
	}
	else
	{
		//NULL;
	}

	return 0;
}

void CIoViewExpTrend::OnPickNode ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes, CNodeData* pNodeData,int32 x,int32 y,int32 iFlag )
{
	if ( NULL != pNodeData )
	{
		CString StrText;
		CString StrTitle;
		T_MerchNodeUserData* pData = (T_MerchNodeUserData*)pNodes->GetUserData();
		ASSERT(NULL!=pData);
		CGmtTime cgmTime(pNodeData->m_iID);
		CString StrTime = Time2String ( cgmTime, pData->m_eTimeIntervalFull);
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
void CIoViewExpTrend::SetXmlSource ()
{
	m_iIntervalXml = -1;
	m_iDrawTypeXml = -1;
	m_iNodeNumXml = 0;
}
void CIoViewExpTrend::MenuAddRegion()
{
	AddSubRegion(false);
}

void CIoViewExpTrend::MenuAddIndex ( int32 id)
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

void CIoViewExpTrend::MenuDelRegion()
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

void CIoViewExpTrend::MenuDelCurve()
{
	if ( NULL == m_pCurvePick || NULL == m_pRegionPick ) return;
	DeleteIndexCurve ( m_pCurvePick );
	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	UpdateSelfDrawCurve();
	Invalidate();
}

void CIoViewExpTrend::OnDropCurve ( CChartRegion* pSrcRegion,CChartCurve* pSrcCurve,CChartRegion* pDestChart)
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

void CIoViewExpTrend::OnRegionCurvesNumChanged (CChartRegion* pRegion, int32 iNum )
{
}

void CIoViewExpTrend::OnCrossData ( CChartRegion* pRegion,int32 iPos,CNodeData& NodeData, float fPricePrevClose, float fYValue, bool32 bShow)
{
	if ( m_MerchParamArray.GetSize() < 1 ) 
	{
		return;
	}

	if ( bShow )
	{
		T_MerchNodeUserData* pTmpData = m_MerchParamArray.GetAt(0);
		int32 iSaveDec = 0;
		CMerch* pMerch = pTmpData->m_pMerchNode;
		if ( NULL != pMerch )
		{
			iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;
		}

		CKLine cKLine;
		NodeData2KLine(NodeData,cKLine);
		
		CString StrTimeLine1,StrTimeLine2;
		Time2String(cKLine.m_TimeCurrent,ENTIMinute,StrTimeLine1,StrTimeLine2);

// 		if ( CheckFlag(NodeData.m_iFlag,CNodeData::KValueInvalid))
// 		{
// 			NodeData.ClearValue(false,false);
// 			NodeData2KLine(NodeData,KLine);
// 			SetFloatData(&m_GridCtrlFloat,KLine,0,0,StrTimeLine1,StrTimeLine2,iSaveDec);
// 			return;
//		}
		//CChartCurve* pJunCurve = m_pRegionMain->GetCurve(1);
		//CChartDrawer * pTrendDrawer =(CChartDrawer * )pJunCurve->GetDrawer();

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
			SetFloatData(&m_GridCtrlFloat,cKLine,pTmpData->m_TrendTradingDayInfo.GetPrevReferPrice(),fYValue,StrTimeLine1,StrTimeLine2,iSaveDec);
		}
	}
	else
	{
		m_GridCtrlFloat.ShowWindow(SW_HIDE);
		CRect rect(0,0,0,0);
		EnableClipDiff(false,rect);
	}
}

void CIoViewExpTrend::OnCrossNoData(CString StrTime,bool32 bShow)
{
	CGridCellSymbol * pCellSymbol = (CGridCellSymbol *)m_GridCtrlFloat.GetCell(0,1);
	if ( NULL == pCellSymbol)
	{
		return;
	}
	pCellSymbol->SetText(StrTime + _T(" "));

	for (int32 i = 1 ; i < m_GridCtrlFloat.GetRowCount() ; i++)
	{
		CGridCellSymbol * pCellSymb = (CGridCellSymbol *)m_GridCtrlFloat.GetCell(i,1);
		if ( NULL == pCellSymb)
		{
			return;
		}
		pCellSymb->SetText(_T(" "));
	}
	
	if (bShow)
	{
		if ( m_GridCtrlFloat.IsWindowVisible() )
		{
			m_GridCtrlFloat.ShowWindow(SW_SHOW|SW_SHOWNOACTIVATE);
			CRect rect;
			m_GridCtrlFloat.GetClientRect(&rect);
			OffsetRect(&rect,m_PtGridCtrl.x,m_PtGridCtrl.y);
			EnableClipDiff(true,rect);
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

void CIoViewExpTrend::SetFloatData ( CGridCtrl* pGridCtrl,float fCursorValue,int32 iSaveDec)
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

void CIoViewExpTrend::SetFloatData ( CGridCtrl* pGridCtrl,CKLine& kLine,float fPricePrevClose,float fCursorValue,CString StrTimeLine1,CString StrTimeLine2,int32 iSaveDec)
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
	Float2SymbolStringLimitMaxLength(StrValue, kLine.m_fPriceClose, fPricePrevClose, iSaveDec, iYLeftShowChar, true);
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
	Float2SymbolStringLimitMaxLength(StrValue, kLine.m_fPriceAvg, fPricePrevClose, iSaveDec, iYLeftShowChar, true);
	pCellSymbol->SetText(StrValue);
	
	//�ǵ�=����-����
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(7, 0);

	StrValue = L" -";
	if (0. != kLine.m_fPriceClose)
	{
		//StrValue = Float2SymbolString(KLine.m_fPriceClose - fPricePrevClose, 0.0f, iSaveDec) + _T("");
		Float2SymbolStringLimitMaxLength(StrValue, kLine.m_fPriceClose - fPricePrevClose, 0.0f, iSaveDec, iYLeftShowChar, true, false, false);
	}
	pCellSymbol->SetText(StrValue);
	
	//�ǵ���(����-����)/����*100%
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(9, 0);

	StrValue = L" -";
	if (0. != kLine.m_fPriceClose && 0. != fPricePrevClose)
	{
		float fRisePercent = ((kLine.m_fPriceClose - fPricePrevClose) / fPricePrevClose) * 100.;
		StrValue = Float2SymbolString(fRisePercent, 0, 2, false, false, true) + _T("");
	}	
	pCellSymbol->SetText(StrValue);		

	//�ɽ���
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(11,0);	
	//StrValue = Float2String(KLine.m_fVolume, 0, true) + _T(" ");
	Float2StringLimitMaxLength(StrValue, kLine.m_fVolume, 0, GetYLeftShowCharWidth(1), true, true);
	pCellSymbol->SetText(StrValue);	

	// �ɽ���/�ֲ�
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(13,0);
	if ( bFutures)
	{
		pCellSymbol->SetDefaultTextColor(ESCVolume);
		//StrValue = Float2String(KLine.m_fHold, 0, true) + _T(" ");
		Float2StringLimitMaxLength(StrValue, kLine.m_fHold, 0, GetYLeftShowCharWidth(1), true, true);
	}
	else
	{
		pCellSymbol->SetDefaultTextColor(ESCAmount);
		//StrValue = Float2String(KLine.m_fAmount, 0, true) + _T(" ");
		Float2StringLimitMaxLength(StrValue, kLine.m_fAmount, 0, GetYLeftShowCharWidth(1), true, true);
	}

	pCellSymbol->SetText(StrValue);	

	if ( !pGridCtrl->IsWindowVisible() )
	{
		pGridCtrl->ShowWindow(SW_SHOW|SW_SHOWNOACTIVATE);
		CRect rect;
		pGridCtrl->GetClientRect(&rect);
		OffsetRect(&rect,m_PtGridCtrl.x,m_PtGridCtrl.y);
		EnableClipDiff(true,rect);
	}

	pGridCtrl->RedrawWindow();
}

void CIoViewExpTrend::ClipGridCtrlFloat (CRect& rect)
{
	EnableClipDiff(true,rect);
}

CString CIoViewExpTrend::OnTime2String ( CGmtTime& Time )
{
	return Time2String(Time,ENTIMinute,false);
}

CString CIoViewExpTrend::OnFloat2String ( float fValue, bool32 bZeroAsHLine, bool32 bNeedTerminate/* = false*/ )
{
	return BaseFloat2String(fValue, bZeroAsHLine, bNeedTerminate);
}

void CIoViewExpTrend::OnCalcXAxis(CChartRegion* pRegion, CDC* pDC, CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aXAxisDivide)
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

void CIoViewExpTrend::OnCalcYAxis (CChartRegion* pRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide)
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

void CIoViewExpTrend::DrawAlarmMerchLine()
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

void CIoViewExpTrend::ValueYAsNodata(float fYMin, float fYMax, const CRect& rect,int32 iSkip, float fy,int32& iValue)
{
	iValue = 0;
	
	float fRate     =  (fy - fYMin) / ( fYMax - fYMin );
	float fValue	=  fRate * rect.Height();

	iValue			=  (int32)(ceil ( fValue ));
	iValue			=  rect.Height() - iValue;
	iValue		   +=  iSkip;
}

void CIoViewExpTrend::OnRegionDrawNotify(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC,CRegion* pRegion,E_RegionDrawNotifyType eType)
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
			DrawTitle1(pDC);

			// ������Ԥ����
			DrawAlarmMerchLine();
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

		//if ( m_SubRegions.GetSize() > 0
		//	&& m_SubRegions[0].m_pSubRegionMain == pChartRegion )
		//{
		//	// ��һ����ͼ�����ֲֻ���ȥ xl 1102
		//	DrawHoldLine(pDC, pPickDC, pChartRegion);
		//}
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
						pDC->_DrawDotLine(pt1, pt2, 2, color);
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
		}
		else if ( pChartRegion == m_pRegionYLeft )
		{
			/*CRect RectMain =*/ m_pRegionMain->GetRectCurves();
			CRect RectYLeft = m_pRegionYLeft->GetRectCurves();
			RectYLeft.DeflateRect(3, 3, 3, 3);

			// ��������
			CAxisDivide *pAxisDivide = (CAxisDivide *)m_pRegionMain->m_aYAxisDivide.GetData();
			for (int32 i = 0; i < m_pRegionMain->m_aYAxisDivide.GetSize(); i++)
			{
				CAxisDivide &AxisDivide = pAxisDivide[i];

				CRect rect = RectYLeft;

				if ((AxisDivide.m_DivideText1.m_uiTextAlign & DT_VCENTER) != 0)						
				{
					rect.top	= AxisDivide.m_iPosPixel - iHalfHeight;
					rect.bottom = AxisDivide.m_iPosPixel + iHalfHeight;
				}
				else if ((AxisDivide.m_DivideText1.m_uiTextAlign & DT_BOTTOM) != 0)
				{
					rect.bottom = AxisDivide.m_iPosPixel;
					rect.top	= rect.bottom - iFontHeight;							
				}
				else
				{
					rect.top	= AxisDivide.m_iPosPixel;
					rect.bottom	= rect.top + iFontHeight;
				}
				 
				if ( rect.bottom > RectYLeft.bottom )
				{
					continue;
				}
				// 
				pDC->SetTextColor(AxisDivide.m_DivideText1.m_lTextColor);
				pDC->SetBkMode(TRANSPARENT);
				pDC->DrawText(AxisDivide.m_DivideText1.m_StrText, &rect, AxisDivide.m_DivideText1.m_uiTextAlign);
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
			
				CRect rct = RectYRight;
				if ((AxisDivide.m_DivideText2.m_uiTextAlign & DT_VCENTER) != 0)						
				{
					rct.top	= AxisDivide.m_iPosPixel - iHalfHeight;
					rct.bottom = AxisDivide.m_iPosPixel + iHalfHeight;
				}
				else if ((AxisDivide.m_DivideText2.m_uiTextAlign & DT_BOTTOM) != 0)
				{
					rct.bottom = AxisDivide.m_iPosPixel;
					rct.top	= rct.bottom - iFontHeight;							
				}
				else
				{
					rct.top	= AxisDivide.m_iPosPixel;
					rct.bottom	= rct.top + iFontHeight;
				}
				
				// 
				pDC->SetTextColor(AxisDivide.m_DivideText2.m_lTextColor);
				pDC->SetBkMode(TRANSPARENT);
				pDC->DrawText(AxisDivide.m_DivideText2.m_StrText, &rct, AxisDivide.m_DivideText2.m_uiTextAlign);
			}
		}
		else if (pChartRegion == m_pRegionXBottom)	// ��������
		{
// 			CRect RectXBottom	= m_pRegionXBottom->GetRectCurves();
// 			RectXBottom.DeflateRect(2, 0, 2, 0);
// 
// 			// ������������
// 			CAxisDivide *pAxisDivide = (CAxisDivide *)m_pRegionMain->m_aXAxisDivide.GetData();
// 			for (int32 i = 0; i < m_pRegionMain->m_aXAxisDivide.GetSize(); i++)
// 			{
// 				CAxisDivide &AxisDivide = pAxisDivide[i];
// 				
// 				CRect Rect = RectXBottom;
// 				if ((AxisDivide.m_DivideText1.m_uiTextAlign & DT_CENTER) != 0)						
// 				{
// 					Rect.left	= AxisDivide.m_iPosPixel - 500;
// 					Rect.right	= AxisDivide.m_iPosPixel + 500;
// 				}
// 				else if ((AxisDivide.m_DivideText1.m_uiTextAlign & DT_RIGHT) != 0)
// 				{
// 					Rect.right	= AxisDivide.m_iPosPixel;
// 					Rect.left	= Rect.right - 500;							
// 				}
// 				else
// 				{
// 					Rect.left	= AxisDivide.m_iPosPixel;
// 					Rect.right	= Rect.left + 500;
// 				}
// 				
// 				// 
// 				pDC->SetTextColor(AxisDivide.m_DivideText1.m_lTextColor);
// 				pDC->SetBkMode(TRANSPARENT);
// 				pDC->DrawText(AxisDivide.m_DivideText1.m_StrText, &Rect, AxisDivide.m_DivideText1.m_uiTextAlign);
// 			}

			// ���´���Ϊ�����������ʱ������ӵ��������
			{
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
				for ( i = 0; i < iAxisDivideCount - 1; i++)
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
				}

				// ����������֣� �������ּ��ص�
				CArray<CRect, CRect&> aRectSpace;	// ��������
				aRectSpace.SetSize(0, 20);
				aRectSpace.Add(RectXBottom);

				for (i = 0; i < iAxisDivideCount; i++)
				{
					CSize SizeText = pDC->GetOutputTextExtent(pAxisDivide[i].m_DivideText1.m_StrText);

					CRect rect = RectXBottom;
					if ((pAxisDivide[i].m_DivideText1.m_uiTextAlign & DT_CENTER) != 0)						
					{
						rect.left	= pAxisDivide[i].m_iPosPixel - (SizeText.cx / 2 + iWidthSpace);
						rect.right	= pAxisDivide[i].m_iPosPixel + (SizeText.cx / 2 + iWidthSpace);
					}
					else if ((pAxisDivide[i].m_DivideText1.m_uiTextAlign & DT_RIGHT) != 0)
					{
						rect.right	= pAxisDivide[i].m_iPosPixel + iWidthSpace;
						rect.left	= pAxisDivide[i].m_iPosPixel - (SizeText.cx + iWidthSpace);
					}
					else
					{
						rect.left	= pAxisDivide[i].m_iPosPixel - iWidthSpace;
						rect.right	= pAxisDivide[i].m_iPosPixel + (SizeText.cx + iWidthSpace);
					}

					// �ӿ��пռ��в��ң� �����Ƿ������ʾ
					int32 iPosFindInSpaceRects = -1;

					CRect *pRectSpace = (CRect *)aRectSpace.GetData();
					for (int32 iIndexSpace = 0; iIndexSpace < aRectSpace.GetSize(); iIndexSpace++)
					{
						if (rect.left < pRectSpace[iIndexSpace].left || rect.right > pRectSpace[iIndexSpace].right)
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
						pDC->SetTextColor(pAxisDivide[i].m_DivideText1.m_lTextColor);
						pDC->SetBkMode(TRANSPARENT);
						pDC->DrawText(pAxisDivide[i].m_DivideText1.m_StrText, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

						// ��ֿ�������
						CRect RectSubLeft = pRectSpace[iPosFindInSpaceRects];
						CRect RectSubRight = pRectSpace[iPosFindInSpaceRects];

						RectSubLeft.right = rect.left - 1;
						RectSubRight.left = rect.right + 1;

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
			
			if (NULL != pData)
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
				pDC->SetTextColor(0xffffff);	// zhangbo 20090706 #��ʱ�ð�ɫ
				int OldMode = pDC->GetBkMode();
				pDC->SetBkMode(TRANSPARENT);

				CRect rt = m_pRegionLeftBottom->GetRectCurves();
				pDC->DrawText(StrDay,rt,DT_CENTER | DT_VCENTER | DT_SINGLELINE );
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
					CRect RectSubMain = SubRegion.m_pSubRegionMain->GetRectCurves();
					
					{
						// ���
						pt1 = RectSubMain.TopLeft();
						pt2 = CPoint(RectSubMain.right, RectSubMain.top);
						pDC->_DrawLine(pt1, pt2);
						
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
							
							if (CAxisDivide::ELSSolid == AxisDivide.m_eLineStyle)
							{
								pDC->_DrawDotLine(pt1, pt2, 2, color);
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
						
						CRect rect = RectYLeft;
						if ((AxisDivide.m_DivideText1.m_uiTextAlign & DT_VCENTER) != 0)						
						{
							rect.top	= AxisDivide.m_iPosPixel - iHalfHeight;
							rect.bottom = AxisDivide.m_iPosPixel + iHalfHeight;
						}
						else if ((AxisDivide.m_DivideText1.m_uiTextAlign & DT_BOTTOM) != 0)
						{
							rect.bottom = AxisDivide.m_iPosPixel;
							rect.top	= rect.bottom - iFontHeight;							
						}
						else
						{
							rect.top	= AxisDivide.m_iPosPixel;
							rect.bottom	= rect.top + iFontHeight;
						}
												
						// 
						if ( rect.bottom > RectSubMain.bottom )
						{
							continue;
						}

						pDC->SetTextColor(AxisDivide.m_DivideText1.m_lTextColor);
						pDC->SetBkMode(TRANSPARENT);
						pDC->DrawText(AxisDivide.m_DivideText1.m_StrText, &rect, AxisDivide.m_DivideText1.m_uiTextAlign);
					}
				}
				else if (pChartRegion == SubRegion.m_pSubRegionYRight)
				{
					//NULL;
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

void CIoViewExpTrend::OnCtrlLeftRight(bool32 bLeft)
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
		
		if (NULL != pMerch && m_pAbsCenterManager)
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
	// fangz0401 ��ʱ���Ƴɽ������²���ʾ,�Ӵ��ڱ���.��������.
}

void CIoViewExpTrend::OnSliderId ( int32& id, int32 iJump )
{
	SliderId ( id, iJump );
}

CString CIoViewExpTrend::OnGetChartGuid ( CChartRegion* pRegion )
{
	return GetChartGuid(pRegion);
}

///////////////////////////////////////////////////////////////////////////////
CString CIoViewExpTrend::GetDefaultXML()
{
	return CIoViewChart::GetDefaultXML();
// 	CString StrThis;
// 
// 	// 
// 	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s> %s\n",
// 		CString(GetXmlElementValue()), 
// 		CString(GetXmlElementAttrIoViewType()), 
// 		CIoViewManager::GetIoViewString(this),
// 		CString(GetXmlElementAttrMerchCode()),
// 		L"",
// 		CString(GetXmlElementAttrMarketId()), 
// 		L"-1",
// 		L"SubRegionNumber=\"1\" VolRegion=\"0\" KLineInterval=\"-1\" KLineDrawType=\"-1\" NodeNumber=\"0\"",
// 		L"<Indexs><Index name=\"VOLFS\" region=\"1\" /></Indexs>");
// /*
//         <IoView IoViewType="IoViewTrend" MerchCode="00011" MakretId="2002" SubRegionNumber="1" VolRegion="0" KLineInterval="-1" KLineDrawType="-1" NodeNumber="0">
//             <Indexs><Index name="VOLFS" region="1" /></Indexs>
//         </IoView>
// 
// */
// 	//
// 	StrThis += L"</";
// 	StrThis += CString(GetXmlElementValue());
// 	StrThis += L">\n";
// 	return StrThis;
}

void CIoViewExpTrend::DoFromXml()
{
	AddSubRegion();
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

	OnVDataForceUpdate();
}

IChartBrige CIoViewExpTrend::GetRegionParentIoView()
{
	IChartBrige ChartRegion;
	ChartRegion.pWnd = this;
	ChartRegion.pIoViewBase = this;
	ChartRegion.pChartRegionData = this;
	return ChartRegion;
}

void CIoViewExpTrend::GetDrawCrossInfoNoData(int32 &iOpenTime,int32 &iCloseTime)
{
	iOpenTime  = m_iOpenTimeForNoData;
	iCloseTime = m_iCloseTimeForNoData;
}

void CIoViewExpTrend::SetChildFrameTitle()
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

CString CIoViewExpTrend::GetHistroyTitle(CGmtTime Time)
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

void CIoViewExpTrend::SetHistoryTime(CGmtTime TimeHistory)
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

void CIoViewExpTrend::ClearLocalData()
{
	for (int32 i = 0; i < m_MerchParamArray.GetSize(); i++)
	{
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(i);
		pData->m_aKLinesCompare.RemoveAll();
		pData->m_aKLinesFull.RemoveAll();
		
		pData->m_pKLinesShow->RemoveAll();
		
		RemoveIndexsNodesData ( pData );
	}
}

bool32 CIoViewExpTrend::UpdateMainMerchKLine(T_MerchNodeUserData &MerchNodeUserData)
{
	CMerch* pMerch = MerchNodeUserData.m_pMerchNode;
	if (NULL == pMerch)
	{
		return false;
	}

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

		return true;
	}
	
	// �Ż��������������£� ���¼���������ʵʱ���ݸ��������ģ� ������������ر��жϴ���
	bool32 bUpdateLast	= false;
	bool32 bModifyLast	= false;
	bool32 bAddSome		= false;

	// ��ȡK�������У� ������ʾ�Ĳ���
	int32 iStartSrc	= CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pKLineRequest->m_KLines, MerchNodeUserData.m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeOpen.m_Time);
	if (iStartSrc < 0 || iStartSrc >= pKLineRequest->m_KLines.GetSize())
	{
		// �Ҳ�����Ҫ��ʾʱ���ڵķ���K��
		MerchNodeUserData.m_aKLinesCompare.RemoveAll();
		MerchNodeUserData.m_aKLinesFull.RemoveAll();

		if (NULL != MerchNodeUserData.m_pKLinesShow)
			MerchNodeUserData.m_pKLinesShow->RemoveAll();
		
		return true;
	}
	
	int32 iEndSrc = CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(pKLineRequest->m_KLines, MerchNodeUserData.m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeEnd.m_Time);
	
	// 
	CKLine *pKLineSrc = (CKLine *)pKLineRequest->m_KLines.GetData() + iStartSrc;
	int32 iCountSrc = iEndSrc - iStartSrc + 1;
	
	CKLine *pKLineCmp = (CKLine *)MerchNodeUserData.m_aKLinesCompare.GetData();
	int32 iCountCmp = MerchNodeUserData.m_aKLinesCompare.GetSize();
	
	int32 iCountAdd = iCountSrc - iCountCmp;
	if (iCountCmp > 1 && (iCountAdd >= 0 && iCountAdd < 3))
	{
		if (memcmp((void*)pKLineSrc, (void*)pKLineCmp, (iCountCmp - 1) * sizeof(CKLine)) == 0)	// ���ںܿ����Ǹ������һ�ʣ� ��ֹ��������һ�����
		{
			bUpdateLast = true;
			
			if (memcmp((void*)(pKLineSrc + iCountCmp - 1), (void*)(pKLineCmp + iCountCmp - 1), sizeof(CKLine)) == 0)
			{
				bModifyLast = false;
				if (0 == iCountAdd)
				{					
					bAddSome = false;
				}
				else
				{
					// �����˼�������
					bAddSome = true;
				}
			}
			else	// ���һ�����ݲ�һ��
			{
				bModifyLast = true;
				if (0 == iCountAdd)
				{
					// ���������һ������
					bAddSome = false;
				}
				else
				{
					// ���������һ�����ݲ��һ������˼�������
					bAddSome = true;
				}
			}
		}
	}
	
	// ...fangz0805 ��ʱû�����Ż�
	bUpdateLast = false;
	
	//////////////////////////////////////////////////////////////////////
	// ��Ը������¼ۺ͸�����ʷ�ֱ���
	if (bUpdateLast)	// �������µļ�������
	{
		if (false == bModifyLast && false == bAddSome)
		{
			// hehe, I just like it, do nothing ^_^
			return false;
		}

		// zhangbo 20090714 #������
		//...
	}
	else				// ȫ������
	{
		// �����������
		ClearLocalData();

		// ��ͼ��������
		if (0 == iCountSrc)
		{
			return true;		// ����û�����ݣ� �Ͳ�������
		}

		// 
		if (m_MerchParamArray.GetSize() > 0)
		{
			T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
			pData->m_aKLinesCompare.SetSize(iCountSrc, 100);

			pKLineCmp = (CKLine *)pData->m_aKLinesCompare.GetData();
			memcpyex(pKLineCmp, pKLineSrc, iCountSrc * sizeof(CKLine));

			// ����Full����
			FillTrendNodes(*pData, m_pAbsCenterManager->GetServerTime(), pKLineCmp, iCountSrc);

			// ������ͼ�����ָ��
			for (int32 j = 0; j < pData->aIndexs.GetSize(); j++)
			{
				T_IndexParam* pIndex = pData->aIndexs.GetAt(j);
				g_formula_compute(this, this, (CChartRegion*)pIndex->pRegion, pData, pIndex, 0, pData->m_aKLinesFull.GetSize());
			}

			// ������ʾ����
			if (!pData->UpdateShowData(0, pData->m_aKLinesFull.GetSize()))
			{
				return false;
			}

			// ���������ֲֵ����� �����гֲ����ݰ�����ת����avg����
// 			if ( NULL != m_pHoldNodes && m_SubRegions.GetSize() > 0 )
// 			{
// 				float fMin, fMax;
// 				fMin = FLT_MAX;
// 				fMax = FLT_MIN;
// 				CChartCurve *pCurve = m_SubRegions[0].m_pSubRegionMain->GetDependentCurve();
// 			}
// 			else
// 			{
// 				ASSERT( 0 );
// 			}

			// ���¼������� Ϊʲô������������ᵼ�� " 0001683 3 ���ڡ�[0423] �л���Ʒʱ, ������ͼ���²���ʱ? " �����Ѿ�����˰�.
			// UpdateAxisSize();
						
			// ��ʾˢ��
			m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
			for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
			{
				m_SubRegions[i].m_pSubRegionMain->SetDrawFlag(CRegion::KDrawFull);
			}	
		}

		// ���¸�ͼ����
		// zhangbo 20090714 #������...
		//...

		return true;
	}

	return true;
}

bool32 CIoViewExpTrend::UpdateSubMerchKLine(T_MerchNodeUserData &MerchNodeUserData)
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
	
	return false;
}

bool32 CIoViewExpTrend::FillTrendNodes(T_MerchNodeUserData &MerchNodeUserData, const CGmtTime &TimeNow, const CKLine *pKLines, int32 iCountKLine)
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
	int32 iMaxTrendUnitCount = MerchNodeUserData.m_TrendTradingDayInfo.m_MarketIOCTime.GetMaxTrendUnitCount();
	ASSERT(iMaxTrendUnitCount > 0);

	MerchNodeUserData.m_aKLinesFull.SetSize(iMaxTrendUnitCount + 20);
	CKLine *pKLineFull = (CKLine *)MerchNodeUserData.m_aKLinesFull.GetData();
	int iCountNode = 0;
	
	// �ȳ�ʼ����һ������
	pKLineFull[iCountNode].m_TimeCurrent	= MerchNodeUserData.m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeOpen.m_Time.GetTime();

	pKLineFull[iCountNode].m_fPriceOpen		= MerchNodeUserData.m_TrendTradingDayInfo.GetPrevReferPrice();
	pKLineFull[iCountNode].m_fPriceClose	= MerchNodeUserData.m_TrendTradingDayInfo.GetPrevReferPrice();
	pKLineFull[iCountNode].m_fPriceAvg		= MerchNodeUserData.m_TrendTradingDayInfo.GetPrevReferPrice();
	pKLineFull[iCountNode].m_fPriceHigh		= MerchNodeUserData.m_TrendTradingDayInfo.GetPrevReferPrice();
	pKLineFull[iCountNode].m_fPriceLow		= MerchNodeUserData.m_TrendTradingDayInfo.GetPrevReferPrice();
	
 	pKLineFull[iCountNode].m_fVolume		= 0.;
 	pKLineFull[iCountNode].m_fAmount		= 0.;
	pKLineFull[iCountNode].m_fHold			= MerchNodeUserData.m_TrendTradingDayInfo.GetPrevHold();	// ȡ����ֲ�
	bool32 bFirstNodeHold = true;
	if ( pKLineFull[iCountNode].m_fHold <= 0.0 )
	{
		// ��ʱ�Ѿ������ˣ�������һ����Ч����ֵ��Ϊ�������
		bFirstNodeHold = false;
		pKLineFull[iCountNode].m_fHold = pKLines[iCountNode].m_fHold;
	}

	// ����ֶδ���
	int32 iKLineIndex = 0;
	for (int32 iIndexOC = 0; iIndexOC < MerchNodeUserData.m_TrendTradingDayInfo.m_MarketIOCTime.m_aOCTimes.GetSize(); iIndexOC += 2)
	{
		CGmtTime TimeOpen =  MerchNodeUserData.m_TrendTradingDayInfo.m_MarketIOCTime.m_aOCTimes[iIndexOC];
		CGmtTime TimeClose =  MerchNodeUserData.m_TrendTradingDayInfo.m_MarketIOCTime.m_aOCTimes[iIndexOC + 1];

		uint32 uiTime = TimeOpen.GetTime();
		for (uiTime = TimeOpen.GetTime(); uiTime <= TimeClose.GetTime() && uiTime <= TimeNow.GetTime(); uiTime += 60)
		{
DO_SKIP_ONE_KLINE:
 			if (iKLineIndex < iCountKLine)
			{
				if (pKLines[iKLineIndex].m_TimeCurrent.GetTime() < uiTime)
				{
					//ASSERT(0);		// ��Ӧ�ó����������
					iKLineIndex++;
					goto DO_SKIP_ONE_KLINE;
				}
				else if (pKLines[iKLineIndex].m_TimeCurrent.GetTime() == uiTime)
				{
					if ( 0 == iCountNode )
					{
						bFirstNodeHold = true;
					}
					memcpyex(&pKLineFull[iCountNode], &pKLines[iKLineIndex], sizeof(CKLine));	// һ��һ��ֵ

					//
					iCountNode++;
					iKLineIndex++;
				}
				else // ������һ��ֵ
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

	if ( !bFirstNodeHold )
	{
		TRACE(_T("��ʱ����ֲ����ݿ��ܻ᲻����ȷ��ֵ!\r\n"));
	}

	// ��������
	MerchNodeUserData.m_aKLinesFull.SetSize(iCountNode);
	return true;
}

bool32 CIoViewExpTrend::CalcMainRegionXAxis(CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aAxisDivide)
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

	// 
	CRect rect = m_pRegionMain->GetRectCurves();
	rect.DeflateRect(1, 1, 1, 1);
	int32 iMaxTrendUnitCount = TrendTradingDay.m_MarketIOCTime.GetMaxTrendUnitCount();

	ASSERT( iMaxTrendUnitCount > 1 );
	if ( iMaxTrendUnitCount <= 1 )
	{
		iMaxTrendUnitCount	=	2;
	}
	float fPixelWidthPerUnit = rect.Width() / (float)(iMaxTrendUnitCount - 1);

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

			/////////////////////////////////////////////////////////////////////////
			// �ڵ�
			CTime Time(uiTime);	// ���ǵ���ʱ����ʾ
			// ...fangz0815 ��ʱ�Ķ�. Ϊ��X �Ử����ʾ��ʱ��Ҫ��ʾ�·ݺ�����
			// AxisNode.m_StrSliderText1.Format(L"%02d/%02d %02d:%02d", Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute());
			AxisNode.m_StrSliderText1.Format(L"%02d:%02d", Time.GetHour(), Time.GetMinute());

			// �ڵ��� 
			if (0 == iIndexNode)
			{
				AxisNode.m_iStartPixel = rect.left;
			}
			else
			{
				AxisNode.m_iStartPixel = pAxisNode[iIndexNode - 1].m_iEndPixel + 1;
			}

			// �ڵ���
			if (iMaxTrendUnitCount - 1 == iIndexNode)
			{
				AxisNode.m_iEndPixel = rect.right;		// xl 0623 �����ұ���һ���߿��������ڲ��������
			}
			else
			{
				AxisNode.m_iEndPixel = rect.left + (int32)((iIndexNode + 0.5) * fPixelWidthPerUnit + 0.5);
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
				if (0 == iIndexOC)
					AxisDivide.m_eLineStyle = CAxisDivide::ELSSolid;
				else
					//AxisDivide.m_eLineStyle = CAxisDivide::ELSDoubleSolid;
					AxisDivide.m_eLineStyle = CAxisDivide::ELSSolid;

				//
				CTime cTime(uiTime);
				AxisDivide.m_DivideText1.m_StrText.Format(L"%02d:%02d", cTime.GetHour(), cTime.GetMinute());
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
				
				AxisDivide.m_DivideText1.m_lTextColor = GetIoViewColor(ESCChartAxisLine);
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
				AxisDivide.m_DivideText1.m_lTextColor = GetIoViewColor(ESCChartAxisLine);
				
				AxisDivide.m_DivideText2 = AxisDivide.m_DivideText1;
				
				//
				iIndexDivide++;
			}
			else if (uiTime % 3600 == 0)		// һСʱ
			{
				AxisDivide.m_iPosPixel		= AxisNode.m_iCenterPixel;
				
				//
				AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;
				AxisDivide.m_eLineStyle = CAxisDivide::ELSSolid;

				//
				AxisDivide.m_eLevelText		= CAxisDivide::EDLLevel3;
				AxisDivide.m_DivideText1.m_StrText.Format(L"%02d:%02d", Time.GetHour(), Time.GetMinute());
				AxisDivide.m_DivideText1.m_uiTextAlign = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
				AxisDivide.m_DivideText1.m_lTextColor = GetIoViewColor(ESCChartAxisLine);
				
				AxisDivide.m_DivideText2 = AxisDivide.m_DivideText1;
				
				//
				iIndexDivide++;
			}
			else if (uiTime % 1800 == 0)		// ��Сʱ
			{
				AxisDivide.m_iPosPixel		= AxisNode.m_iCenterPixel;
				
				//
				AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;
				AxisDivide.m_eLineStyle = CAxisDivide::ELSDot;

				//
				AxisDivide.m_eLevelText		= CAxisDivide::EDLLevel4;
				AxisDivide.m_DivideText1.m_StrText.Format(L"%02d:%02d", Time.GetHour(), Time.GetMinute());
				AxisDivide.m_DivideText1.m_uiTextAlign = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
				AxisDivide.m_DivideText1.m_lTextColor = GetIoViewColor(ESCChartAxisLine);
				
				AxisDivide.m_DivideText2 = AxisDivide.m_DivideText1;
				
				//
				iIndexDivide++;
			}
		}
	}

	//  
	aAxisNodes.SetSize(iIndexNode);
	aAxisDivide.SetSize(iIndexDivide);

	return true;
}

void CIoViewExpTrend::CalcMainRegionYAxis(CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide)
{
	ASSERT(NULL != m_pRegionMain);
	aYAxisDivide.SetSize(0);

	// ����Y����������ɶ��ٸ��̶�
	CRect rect = m_pRegionMain->GetRectCurves();

	CFont* pOldFont = pDC->SelectObject(GetIoViewFontObject(ESFSmall));					
	int32 iHeightText = pDC->GetTextExtent(_T("����ֵ")).cy;
	pDC->SelectObject(pOldFont);

	// int32 iHeightText = pDC->GetTextExtent(_T("����ֵ")).cy;
	int32 iHeightSub = iHeightText + 12;
	//int32 iHeightHalf = iHeightSub/2;
	int32 iNum = rect.Height() / iHeightSub + 1;
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

			float fHeightSub = static_cast<float>(rectMainRegion.Height() / (1.0 * iNum));
			
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

	float fPricePrevClose = 1e-6;
	if ( NULL != pData )
	{
		if (pData->m_TrendTradingDayInfo.m_bInit)
			fPricePrevClose = pData->m_TrendTradingDayInfo.GetPrevReferPrice();
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

	for (int32 i = 0; i <= iNum; i++, fStart += fHeightSub)
	{
		if (i == iHalfNum)
			fStart = fYMid;

		if (i == iNum)
			fStart = fYMax;
		
		// 
		if (!pCurveDependent->ValueRegionY(fStart, y))
		{
			//ASSERT(0);
			break;
		}
		m_pRegionMain->RegionYToClient(y);

		// xl 0001762 ����YLeft��ʾ�ִ����� - ����һ���ո�
		//CString StrValue	= Float2String(fStart, iSaveDec, true, false, false);
		//CString StrPercent	= Float2String(100.0f*(fStart-fPricePrevClose)/fPricePrevClose,2,false,false,true);
		CString StrValue;
		Float2StringLimitMaxLength(StrValue, fStart, iSaveDec, GetYLeftShowCharWidth()-1, true, true, false, false);
		CString StrPercent;
		Float2StringLimitMaxLength(StrPercent, 100.0f*(fStart-fPricePrevClose)/fPricePrevClose, 2, GetYLeftShowCharWidth()-1, true, false, false, true);
		
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

void CIoViewExpTrend::CalcSubRegionYAxis(CChartRegion* pChartRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide)
{
	ASSERT(NULL != pChartRegion);
	aYAxisDivide.SetSize(0);
	
	// ����Y����������ɶ��ٸ��̶�
	CRect rect = pChartRegion->GetRectCurves();
	int32 iHeightText = pDC->GetTextExtent(_T("����ֵ")).cy;
	int32 iHeightSub  = iHeightText + 12;
	//int32 iHeightHalf = iHeightSub/2;
	int32 iNum = rect.Height()/iHeightSub + 1;

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
	
	// ��ȡ�������ϼ�����Ҫ��ֵ
	CChartCurve *pCurveDependent = pChartRegion->GetDependentCurve();
	
	if ( NULL == pData )
	{
		return;
	}

	// �����ϸ�������ڻ��������ֲֵķ�Χ
	m_fMinHold = m_fMaxHold = 0.0f;
	if ( pData->m_pMerchNode != NULL 
		&& CReportScheme::IsFuture(pData->m_pMerchNode->m_Market.m_MarketInfo.m_eMarketReportType)
		&& NULL != pData->m_pKLinesShow
		&& pData->m_pKLinesShow->GetSize() > 0 )
	{
		m_fMinHold = FLT_MAX;
		m_fMaxHold = FLT_MIN;
		const int32 iSize = pData->m_pKLinesShow->GetSize();
		for ( int32 i=0; i < iSize ; i++ )
		{
			const CNodeData &node = pData->m_pKLinesShow->GetNodes().ElementAt(i);
			if ( node.m_fHold < m_fMinHold )
			{
				m_fMinHold = node.m_fHold;
			}
			if ( node.m_fHold > m_fMaxHold )
			{
				m_fMaxHold = node.m_fHold;
			}
		}
		ASSERT( m_fMinHold >= 0.0 );
	}

	float fYMin = 0., fYMax = 0./*, fYMid = 0.*/;
	if (NULL == pCurveDependent || !pCurveDependent->GetYMinMax(fYMin, fYMax) )
	{
		if(m_pAbsCenterManager)
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

				float fHeightSub = static_cast<float>(rectRegion.Height() / (1.0*iNum));

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
			if (!pCurveDependent->ValueRegionY(fStart, y))
			{
				//
				/*CNodeSequence* pNodes = */pCurveDependent->GetNodes();
				//CArray<CNodeData, CNodeData&>& NodeDatas = pNodes->GetNodes();
				
				// TestNodes(NodeDatas, "c:\\Nodes.txt", m_pMerchXml->m_MerchInfo.m_StrMerchCode, m_pMerchXml->m_MerchInfo.m_iMarketId);
				//ASSERT(0);
				break;
			}
			pChartRegion->RegionYToClient(y);
			
			// 0001762 - �ɽ����ȴ�ָ�겻��Ҫ��ʾС���� > 1000 
			if ( fStart >= 1000 || fStart <= -1000  )
			{
				iSaveDec = 0;
			}

			// xl 0001762 ����YLeft��ʾ�ִ����� - ����һ���ո�
			//CString StrValue	= Float2String(fStart, iSaveDec, true, false, false);
			CString StrValue;
			Float2StringLimitMaxLength(StrValue, fStart, iSaveDec, GetYLeftShowCharWidth()-1, true, true, false, false);

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
			
			AxisDivide.m_DivideText1.m_uiTextAlign		= DT_VCENTER | DT_RIGHT | DT_SINGLELINE;
			AxisDivide.m_DivideText2.m_uiTextAlign		= DT_VCENTER | DT_LEFT | DT_SINGLELINE;
			
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

void CIoViewExpTrend::CalcLayoutRegions(bool bOnlyUpdateMainRegion)
{	
	if (m_MerchParamArray.GetSize() <= 0)
		return;

	T_MerchNodeUserData* pMerchData = m_MerchParamArray.GetAt(0);

	// ������region������ֵ�� �Գ�����
	{
		if (NULL != m_pRegionMain)
		{
			bool32 bValidCurveYMinMax = false;
			float fCurveYMin = 0., fCurveYMax = 0.;

			// �����������ߵ����±߽�ֵ
			int32 i = 0;
			for ( i = 0; i < m_pRegionMain->m_Curves.GetSize(); i++)
			{
				float fYMin = 0., fYMax = 0.;

				CChartCurve *pChartCurve = m_pRegionMain->m_Curves[i];
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

			// ͬһ�������� ��/�� �۱仯�����, ��Ҫ����������� - ����ʷ��ʱ����Ҫ
			if ( NULL != m_pMerchXml 
				&& NULL !=  m_pMerchXml->m_pRealtimePrice 
				&& 0. != m_pMerchXml->m_pRealtimePrice->m_fPriceHigh 
				&& 0. != m_pMerchXml->m_pRealtimePrice->m_fPriceLow
				&& !m_bHistoryTrend )
			{
				float fMinRealTimePrice = m_pMerchXml->m_pRealtimePrice->m_fPriceHigh;
				//float fMaxRealTimePrice = m_pMerchXml->m_pRealtimePrice->m_fPriceLow;
				
				fCurveYMax = fCurveYMax > fMinRealTimePrice ? fCurveYMax : fMinRealTimePrice;
				fCurveYMin = fCurveYMin < fMinRealTimePrice ? fCurveYMin : fMinRealTimePrice;	
			}

			// �����ռ���Ϊ��ֵ�� ���¶ԳƷ�
			if (bValidCurveYMinMax)
			{
				float fPricePrevClose = pMerchData->m_TrendTradingDayInfo.GetPrevReferPrice();	// ����

				float fHeight1 = fCurveYMax - fPricePrevClose;
				float fHeight2 = fPricePrevClose - fCurveYMin;
				if (fHeight1 > fHeight2)
					fCurveYMin = fPricePrevClose - fHeight1;
				else
					fCurveYMax = fPricePrevClose + fHeight2;
			}

			// �������е���region�е�curve��������ֵ
			for (i = 0; i < m_pRegionMain->m_Curves.GetSize(); i++)
			{
				//float fYMin = 0., fYMax = 0.;
				
				CChartCurve *pChartCurve = m_pRegionMain->m_Curves[i];
				pChartCurve->SetYMinMax(fCurveYMin, fCurveYMax, bValidCurveYMinMax);
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

				// �������е���region�е�curve��������ֵ
				for (int32 m = 0; m < pSubRegionMain->m_Curves.GetSize(); m++)
				{
					CChartCurve *pChartCurve = pSubRegionMain->m_Curves[m];
					pChartCurve->SetYMinMax(fCurveYMin, fCurveYMax, bValidCurveYMinMax);
				}
			}		
		}
	}
}

void CIoViewExpTrend::UpdateAxisSize( bool32 bPreDraw /*= true */ )
{
	CIoViewChart::UpdateAxisSize(bPreDraw);
	
	//if ( NULL != m_pRegionMain )
	//{
	//	// ȡ��right - Ӧ����ȫ�� - ��ռʱ����right����ûʵ��
 //		int32 iRightSkip = m_pRegionMain->GetRightSkip();
 //		if ( iRightSkip < 1 )
 //		{
 //			m_pRegionMain->SetRightSkip(1);
 //		}
	//}
}

BOOL CIoViewExpTrend::PreTranslateMessage( MSG* pMsg )
{
	if ( m_bHistoryTrend )
	{
		return CControlBase::PreTranslateMessage(pMsg);
	}
	else
	{
		return CIoViewChart::PreTranslateMessage(pMsg);
	}
}

void CIoViewExpTrend::DrawHoldLine( IN CMemDCEx* pDC, IN CMemDCEx* pPickDC, IN CChartRegion* pRegion )
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
	int32 iXNodeSize = pRegion->m_aXAxisNodes.GetSize();
	iSize = min(iXNodeSize, iSize);
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

CMerch* CIoViewExpTrend::GetMerchAccordExpMerch(CMerch* pMerch)
{
	if ( NULL == pMerch || NULL == m_pAbsCenterManager )
	{
		return NULL;
	}

	// Ĭ�϶�����ָ֤��
	CMerch* pMerchExp = NULL;	
	m_pAbsCenterManager->GetMerchManager().FindMerch(L"000001", 0, pMerchExp);

	//	
	if ( 1 == pMerch->m_Market.m_Breed.m_iBreedId )
	{				
		if ( 1004 == pMerch->m_Market.m_MarketInfo.m_iMarketId )
		{
			// 2��������С�������С��ָ��
			m_pAbsCenterManager->GetMerchManager().FindMerch(/*L"399005"*/L"399101", 1000, pMerchExp);
		}
		else if ( 1008 == pMerch->m_Market.m_MarketInfo.m_iMarketId )
		{
			// 3�����ڴ�ҵ����ô�ҵ��ָ��
			m_pAbsCenterManager->GetMerchManager().FindMerch(/*L"399006"*/L"399102", 1000, pMerchExp);
		}
		else
		{
			// 4�������ȫ���������ָ
			m_pAbsCenterManager->GetMerchManager().FindMerch(L"399001", 1000, pMerchExp);
		}	
	}
	
	//
	if ( NULL == pMerchExp )
	{
		m_pAbsCenterManager->GetMerchManager().FindMerch(L"000001", 0, pMerchExp);
	}

	//
	return pMerchExp;
}
