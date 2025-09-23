#include "stdafx.h"
#include "GridCtrlSys.h"
#include "GridCellSys.h"
#include "GridCellSymbol.h"
#include "guidrawlayer.h"
#include "NewMenu.h"
#include "memdc.h"
#include "MerchManager.h"
#include "facescheme.h"
#include "IoViewTick.h"
#include "IoViewManager.h"
#include "ShareFun.h"
#include "Region.h"
#include "ChartRegion.h"
#include "CFormularContent.h"
#include "float.h"
#include "ChartDrawer.h"
#include "IoViewShare.h"
#include "dlgtrendindexstatistics.h"
#include "GGTongView.h"
//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
							//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

const int32 KMinTickNumPerScreen		= 10;			// ������ʾK������, ����KSpaceRightCountλ��
const int32 KMaxTickNumPerScreen		= 10000;		// �����ʾK������
const int32 KDefaultTickNumPerScreen	= 200;
const int32 KDefaultTickCtrlMoveSpeed	= 5;			// Ĭ��Ctrl�ƶ��ٶ�����

///////////////////////////////////////////////////////////////////////////////

//
IMPLEMENT_DYNCREATE(CIoViewTick, CIoViewChart)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewTick, CIoViewChart)
	//{{AFX_MSG_MAP(CIoViewTick)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//ON_WM_TIMER()
	ON_COMMAND_RANGE(IDM_CHART_BEGIN,IDM_CHART_END,OnMenu)
	ON_COMMAND_RANGE(IDT_SD_BEGIN,IDT_SD_END,OnMenu)
	ON_COMMAND_RANGE(ID_TICK_CANCELZOOM, ID_TICK_CANCELZOOM, OnMenu)
	ON_COMMAND_RANGE(ID_TICK_SHOWTREND, ID_TICK_SHOWTREND, OnMenu)
	ON_COMMAND_RANGE(ID_TICK_DAY1, ID_TICK_DAY10, OnMenu)
	ON_COMMAND_RANGE(ID_TREND_SHOWVOLCOLOR, ID_TREND_SHOWVOLCOLOR, OnMenu)
	ON_MESSAGE(UM_IoViewTitle_Button_LButtonDown,OnMessageTitleButton)
	ON_MESSAGE_VOID(UM_SettingChanged, OnMySettingChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewTick::CIoViewTick()
:CIoViewChart(),CChartRegionViewParam(NULL)
{
	m_pParent			= NULL;
	m_rectClient		= CRect(-1,-1,-1,-1);
	m_iDataServiceType  = EDSTTimeSale | EDSTTick |EDSTKLine;
	m_StrText			=_T("IoViewTick");
	m_pRegionViewData	= this;
	m_pRegionDrawNotify = this;
	SetParentWnd(this);
	m_iChartType		= 2;

	m_bRequestNodesByRectView = false;
	
	//
	m_iCurCrossTickIndex = -1;
	m_iNodeCountPerScreen = KDefaultTickNumPerScreen;
	
	memset((void*)(&m_TickCrossNow), 0, sizeof(m_TickCrossNow));

	m_eMainCurveAxisYType = CPriceToAxisYObject::EAYT_Pecent;	// ����ͼ��֧����һ��

	m_TimeZoomStart = m_TimeZoomEnd = CGmtTime(0);
	m_iTickMultiDay = 1;
}

CIoViewTick::~CIoViewTick()
{

}

///////////////////////////////////////////////////////////////////////////////
int CIoViewTick::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CIoViewChart::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL CIoViewTick::TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ( NULL != m_pIoViewManager && this != m_pIoViewManager->GetActiveIoView() )
	{
		return FALSE;
	}
	return CIoViewChart::TestKeyDown(nChar,nRepCnt,nFlags);
}

void CIoViewTick::InitCtrlFloat ( CGridCtrl* pGridCtrl )
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
	pGridCtrl->SetCellType(0,0, RUNTIME_CLASS(CGridCellSymbol));	// ����
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(0,0);
	pCellSymbol->SetDefaultFont(ESFSmall);
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
	pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	
	pGridCtrl->SetCellType(1,0, RUNTIME_CLASS(CGridCellSymbol));	// ʱ��
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(1,0);
	pCellSymbol->SetDefaultFont(ESFSmall);
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
	pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	
	pGridCtrl->SetCellType(3,0, RUNTIME_CLASS(CGridCellSymbol));	// �۸�
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(3,0);
	pCellSymbol->SetDefaultFont(ESFSmall);
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
	pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

	pGridCtrl->SetCellType(5,0, RUNTIME_CLASS(CGridCellSymbol));	// ����
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(5,0);
	pCellSymbol->SetDefaultFont(ESFSmall);
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
	pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	
	pGridCtrl->SetCellType(7,0, RUNTIME_CLASS(CGridCellSymbol));
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(7,0);		// �ǵ�
	pCellSymbol->SetDefaultFont(ESFSmall);
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	
	pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	
	pGridCtrl->SetCellType(9,0, RUNTIME_CLASS(CGridCellSymbol));
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(9,0);		// �Ƿ�
	pCellSymbol->SetDefaultFont(ESFSmall);
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	
	pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	
	pCell = (CGridCellSys *)pGridCtrl->GetCell(11, 0);				// �ɽ���
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetDefaultTextColor(ESCVolume);
	
	pCell = (CGridCellSys *)pGridCtrl->GetCell(13, 0);				// ���
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetDefaultTextColor(ESCAmount);

	pGridCtrl->ShowWindow(SW_SHOW);
	SetGridFloatInitPos();
}

void CIoViewTick::CreateRegion()
{
	ASSERT ( NULL == m_pRegionMain );
	CIoViewChart::CreateRegion();
	UpdateGDIObject();

	///////////////////////////////////////////////////////////////////////////////////////////////
	//��K
	new CChartRegion ( this, m_pRegionMain, _T("k��"),
//		CRegion::KBottomDragAble|
		CChartRegion::KChart|
		CChartRegion::KCrossAble|
		CChartRegion::KPanAble|
		CChartRegion::KHScaleAble|
		CChartRegion::KRectZoomOut);
	m_pRegionMain->SetTopSkip(18);
	m_pRegionMain->SetBottomSkip(0);
	m_pRegionMain->AddDrawNotify(this,ERDNBeforeDrawCurve);
	m_pRegionMain->AddDrawNotify(this,ERDNAfterDrawCurve);
	m_pRegionMain->AddDrawNotify(this,ERDNBeforeTransformAllRegion);

	//X��
	new CChartRegion ( this,m_pRegionXBottom, _T("x��"), 
		CRegion::KFixHeight|
		CChartRegion::KXAxis|
		CChartRegion::KDragXAxisPanAble);

	m_pRegionXBottom->AddDrawNotify(this,ERDNBeforeDrawCurve);
	m_pRegionXBottom->SetDataSourceRegion(m_pRegionMain);

	//����������
	new CChartRegion ( this,m_pRegionSeparator, _T("split"), 
		CChartRegion::KUserChart);
	m_pRegionSeparator->AddDrawNotify(this,ERDNAfterDrawCurve);

	new CChartRegion ( this,m_pRegionLeftBottom, _T("���½�"),
		CChartRegion::KUserChart|
		CRegion::KFixHeight|
		CRegion::KFixWidth);
	m_pRegionLeftBottom->AddDrawNotify(this,ERDNBeforeDrawCurve);
	m_pRegionLeftBottom->AddDrawNotify(this,ERDNAfterDrawCurve);
	
	new CChartRegion ( this,m_pRegionRightBottom, _T("���½�"),
		CChartRegion::KUserChart|
		CRegion::KFixHeight|
		CRegion::KFixWidth);
	m_pRegionRightBottom->AddDrawNotify(this,ERDNBeforeDrawCurve);
	m_pRegionRightBottom->AddDrawNotify(this,ERDNAfterDrawCurve);

	new CChartRegion ( this,m_pRegionYLeft, _T("��y��"),
//		CRegion::KBottomDragAble|
		CRegion::KFixWidth|
		CChartRegion::KYAxis|
		CChartRegion::KActiveFlag);
	m_pRegionYLeft->SetTopSkip(18);
	m_pRegionYLeft->SetBottomSkip(0);
	m_pRegionMain->AddYDependentRegion(m_pRegionYLeft);
	m_pRegionYLeft->AddDrawNotify(this,ERDNBeforeDrawCurve);
	m_pRegionYLeft->AddDrawNotify(this,ERDNAfterDrawCurve);
	m_pRegionYLeft->SetDataSourceRegion(m_pRegionMain);

	new CChartRegion ( this,m_pRegionYRight, _T("��y��"),
		//		CRegion::KBottomDragAble|
		CRegion::KFixWidth|
		CChartRegion::KYAxis);
	m_pRegionYRight->SetTopSkip(18);
	m_pRegionYRight->SetBottomSkip(0);
	m_pRegionMain->AddYDependentRegion(m_pRegionYRight);
	m_pRegionYRight->AddDrawNotify(this,ERDNBeforeDrawCurve);
	m_pRegionYRight->AddDrawNotify(this,ERDNAfterDrawCurve);
	m_pRegionYRight->SetDataSourceRegion(m_pRegionMain);
}

void CIoViewTick::SplitRegion()
{
	CIoViewChart::SplitRegion();
	UpdateGDIObject();
}

void CIoViewTick::OnDestroy() 
{
	ReleaseMemDC();
	CIoViewChart::OnDestroy();
}

// ֪ͨ��ͼ�ı��ע����Ʒ
//lint --e{429}
void CIoViewTick::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch)
		return;
	
	CIoViewChart::OnVDataMerchChanged(iMarketId, StrMerchCode, pMerch);
	
	if (NULL != pMerch)
	{
		//////////////////////////////////////////////////////////////////////////
		
		ASSERT(NULL!=m_pRegionMain);
		if ( NULL == m_pAbsCenterManager )
		{
			return;
		}

		if ( IsTickZoomed() )
		{
			CancelZoom();	// ȡ������������Ϣ
		}

		// ��ʼ������Ľ�������Ϣ
		m_aTickMultiDayIOCTimes.RemoveAll();
		CMarketIOCTimeInfo IOCTime;
		CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();
		pMerch->m_Market.GetRecentTradingDay(TimeNow, IOCTime, pMerch->m_MerchInfo);
		m_aTickMultiDayIOCTimes.Add(IOCTime);	// ���뵱��Ŀ�����Ϣ
		
		bool32 bInhert;
		T_MerchNodeUserData* pData = NewMerchData(pMerch, true, bInhert);

		// 
		pData->m_eTimeIntervalFull = ENTIMinute;
		CMarketIOCTimeInfo RecentTradingDay;
		if (pMerch->m_Market.GetRecentTradingDay(m_pAbsCenterManager->GetServerTime(), RecentTradingDay, pMerch->m_MerchInfo))
		{
			pData->m_TrendTradingDayInfo.Set(pMerch, RecentTradingDay);
			pData->m_TrendTradingDayInfo.RecalcPrice(*pMerch);
			pData->m_TrendTradingDayInfo.RecalcHold(*pMerch);
			
			// zhangbo 20090708 #�����䣬 �������
			//...
		}
		
		if ( !bInhert )
		{
			CNodeSequence* pNodes = CreateNodes();
			pNodes->SetUserData(pData);
			pData->m_pKLinesShow = pNodes;
			
			//new 1 curves/nodes/...
			CChartCurve* pCurve = m_pRegionMain->CreateCurve(CChartCurve::KDependentCurve|
															 CChartCurve::KRequestCurve|
															 CChartCurve::KTypeTick|
															 CChartCurve::KYTransformByLowHigh|
															 CChartCurve::KDonotPick
															 /*CChartCurve::KUseNodesNameAsTitle*/);
			pCurve->SetTitle(_T("����ͼ "));
			CChartDrawer* pTickDrawer = new CChartDrawer(*this, CChartDrawer::EKDSTickPrice);
			
			// 		if ( -1 != m_iDrawTypeXml )
			// 		{
			// 			pTickDrawer->m_eChartDrawType = (CChartDrawer::E_ChartDrawStyle)m_iDrawTypeXml;
			// 			m_iDrawTypeXml = 0;
			// 		}
			
			pCurve->AttatchDrawer(pTickDrawer);
			pCurve->AttatchNodes(pNodes);

			// ��������
			CChartCurve *pCurveAvg = m_pRegionMain->CreateCurve(CChartCurve::KTypeTick | CChartCurve::KYTransformByAvg |CChartCurve::KDonotPick);
			pCurveAvg->SetTitle(_T("���� "));
			pCurveAvg->m_clrTitleText  = GetIoViewColor(ESCVolume2);				
			CChartDrawer* pJunDrawer   = new CChartDrawer(*this, CChartDrawer::EKDSTrendJunXian);
			
			pCurveAvg->AttatchDrawer(pJunDrawer);
			pCurveAvg->AttatchNodes(pNodes);
			
			//new 2 curve
			pCurve = m_pRegionXBottom->CreateCurve(CChartCurve::KDependentCurve|
				CChartCurve::KRequestCurve|
				CChartCurve::KInVisible|
				CChartCurve::KTypeTick|
				CChartCurve::KYTransformByLowHigh);
			pCurve->AttatchNodes(pNodes);
		}
		
		// 
		CMerch* pMerchNew = pData->m_pMerchNode;
		
		if (NULL != pMerchNew)
		{
			if ( NULL != m_pCell1 )
			{
				if (CReportScheme::IsFuture(pMerchNew->m_Market.m_MarketInfo.m_eMarketReportType))
				{
					m_pCell1->SetText(_T(" �ֲ���"));
				}
				else
				{
					m_pCell1->SetText(_T(" ���"));
				}
			}			
		}
		
		if ( m_IndexPostAdd.id >= 0 )
		{
			m_IndexPostAdd.id = -1;
			AddIndex(m_IndexPostAdd.pRegion,m_IndexPostAdd.StrIndexName);
		}
		
		UpdateTickMultiDayTitle();
		// 
		OnVDataMerchTimeSalesUpdate(pMerch);
		
		// 
		// RequestViewData();
		SetCurveTitle(pData);
		UpdateAxisSize();
		UpdateSelfDrawCurve();
		m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
		PostMessage(WM_PAINT);
	}
	else
	{
		// zhangbo 20090824 #�����䣬 ������Ʒ������ʱ����յ�ǰ��ʾ����
		//...
	}
}

// ������ͼ���
bool32 CIoViewTick::OnVDataAddCompareMerch(IN CMerch *pMerch)
{
	// ...fangz 0526
	/*
	if (NULL == pMerch)
		return false;

	StartTime(201);
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
	T_MerchNodeUserData* pData = NewMerchData(pMerch, false, bInhert);
	if ( NULL == pData )
	{
		return false;
	}

	CNodeSequence* pNodes = CreateNodes();
	pNodes->SetUserData(pData);
	pData->m_pKLinesShow = pNodes;


	//new 1 curves/nodes/...
	CChartCurve* pCurve = m_pRegionMain->CreateCurve(CChartCurve::KRequestCurve|
													 CChartCurve::KYTransformToAlignDependent|
													 CChartCurve::KTypeTick |
													 CChartCurve::KYTransformByLowHigh|
													 CChartCurve::KUseNodesNameAsTitle);

	CChartDrawer* pCompareTickDrawer = new CChartDrawer(*this, CChartDrawer::EKDSTickPrice);
	CChartCurve *pCurveDependent = m_pRegionMain->GetDependentCurve();
	if (NULL == pCurveDependent)
		return false;

	CChartDrawer* pTickDrawer = (CChartDrawer*)pCurveDependent->GetDrawer();
	pCompareTickDrawer->m_eChartDrawType = pTickDrawer->m_eChartDrawType;
	pCurve->AttatchDrawer(pCompareTickDrawer);
	pCurve->AttatchNodes(pNodes);

	RequestViewData();
	
	// ������Ʒʱ��Ϊ׼���������Ʒ����
	{
		T_MerchNodeUserData *pData0 = m_MerchParamArray[0];
		if (NULL != pData0 && pData0->m_aTicksCompare.GetSize() > 0)
		{
			// ������Ʒ��������Ʒ��ʱ��Ϊ׼����
			CGmtTime TimeStart = pData->m_aTicksCompare[0].m_TimeCurrent.m_Time;
			CGmtTime TimeEnd = pData->m_aTicksCompare[pData->m_aTicksCompare.GetSize() - 1].m_TimeCurrent.m_Time;

			// ָ��ʱ�������
			CMmiReqMerchTimeSales info;
			info.m_iMarketId = pData->m_pMerchNode->m_MerchInfo.m_iMarketId;
			info.m_StrMerchCode = pData->m_pMerchNode->m_MerchInfo.m_StrMerchCode;
			
			info.m_eReqTimeType = ERTYSpecifyTime;
			info.m_TimeStart	= TimeStart;
			info.m_TimeEnd		= TimeEnd;
			
			RequestViewData(m_pAbsCenterManager,(CMmiCommBase*)&info);
		}
	}


	// 
	SetCurveTitle(pData);
// 	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
// 	PostMessage(WM_PAINT);

	KillTimer(103);
	SetTimer(103,5,NULL);

	EndTime(201,_T("CIoViewTick::OnVDataAddCompareMerch"));	
	*/

	return true;
}

void CIoViewTick::SetCurveTitle ( T_MerchNodeUserData* pData )
{
	CString StrTitle;
	CMerch* pMerch = pData->m_pMerchNode;
	if ( NULL == pMerch )
	{
		if ( pData->bMainMerch )
		{
			StrTitle = pData->m_pMerchNode->m_MerchInfo.m_StrMerchCode + _T(" ") + _T(" (��)");
		}
		else
		{
			StrTitle = pData->m_pMerchNode->m_MerchInfo.m_StrMerchCode + _T("(��)");
		}
	}
	else
	{
		if ( pData->bMainMerch )
		{
			StrTitle = pMerch->m_MerchInfo.m_StrMerchCnName + _T("  ") + _T(" ");
		}
		else
		{
			StrTitle = pMerch->m_MerchInfo.m_StrMerchCnName + _T(" ���� ");
		}
	}
	pData->m_pKLinesShow->SetName(StrTitle);
}
//
void CIoViewTick::OnVDataForceUpdate()
{
	if ( NULL == m_pRegionMain )
	{
		return;
	}

	// zhangbo 20090707 #�����䣬 ���¼��㽻���գ� �п����Ǹ����ʼ���� ��Ҫ���»�����ʾ��
	//...
	if ( m_pAbsCenterManager && (m_MerchParamArray.GetSize() > 0) )
	{		
		T_MerchNodeUserData * pData = m_MerchParamArray.GetAt(0);
		ASSERT(NULL != pData);
		
		CGmtTime TimeInit	= pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time;
		CGmtTime TimeEnd	= TimeInit + CGmtTimeSpan(0, 23, 59, 59);
		
		CGmtTime TimeServer = m_pAbsCenterManager->GetServerTime();
		
		if (TimeServer < TimeInit || TimeServer > TimeEnd)	// ����ͬһ�죬 ��Ҫ������ʱ������ռ۵�
		{
			// �����������
			ClearLocalData();
			
			// ��ȡ����Ʒ������ص�����
			CMarketIOCTimeInfo RecentTradingDay;
			bool32 bRecentTradingDayOk = false;
			bRecentTradingDayOk = m_pMerchXml->m_Market.GetRecentTradingDay(m_pAbsCenterManager->GetServerTime(), RecentTradingDay, m_pMerchXml->m_MerchInfo);
			if ( bRecentTradingDayOk )
			{
				pData->m_TrendTradingDayInfo.Set(m_pMerchXml, RecentTradingDay);
				pData->m_TrendTradingDayInfo.RecalcPrice(*m_pMerchXml);
			}
			//else
			//{
			//	ASSERT(0);
			//}
			
			m_aTickMultiDayIOCTimes.RemoveAll();	// ���ʱ�����Ϣ
			if ( pData->m_TrendTradingDayInfo.m_bInit )
			{
				m_aTickMultiDayIOCTimes.Add(pData->m_TrendTradingDayInfo.m_MarketIOCTime);		// �����һ��ĳ�ʼ��ʱ��
			}

			// 
			OnVDataMerchKLineUpdate(m_pMerchXml);
			OnVDataMerchTimeSalesUpdate(m_pMerchXml);
		}
		//else
		//{
		//	NULL;
		//}
	}

	CIoViewChart::OnVDataForceUpdate();
	RequestViewData();
	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	PostMessage(WM_PAINT);
}

void CIoViewTick::OnVDataFormulaChanged ( E_FormulaUpdateType eUpdateType, CString StrName )
{
	CIoViewChart::OnVDataFormulaChanged ( eUpdateType, StrName );
	OnVDataMerchTimeSalesUpdate(m_pMerchXml);
}

void CIoViewTick::OnVDataRealtimePriceUpdate(IN CMerch *pMerch)
{
	if (NULL == pMerch || pMerch != m_pMerchXml)
		return;

	// ������Ʒ������������
	for (int32 i = 0; i < m_MerchParamArray.GetSize(); i ++)
	{
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(i);
		if (pData->m_pMerchNode != pMerch)
			continue;
		
		// ��Ҫ�������ռ�
		if (0. == pData->m_TrendTradingDayInfo.m_fPricePrevClose && 0. == pData->m_TrendTradingDayInfo.m_fPricePrevAvg)
		{
			// ���Ը���������صı�������
			float fPricePrevClose = pData->m_TrendTradingDayInfo.GetPrevReferPrice();
			pData->m_TrendTradingDayInfo.RecalcPrice(*pMerch);
			if (fPricePrevClose != pData->m_TrendTradingDayInfo.GetPrevReferPrice())
			{
				// zhangbo 20090708 #������, ��������ֵ
				//...				
				ReDrawAysnc();
			}
		}
	}

	CalcTodayRise(pMerch);
}

bool32 CIoViewTick::IsShowNewestTick(OUT CGmtTime& TimeStartInFullList, OUT CGmtTime &TimeEndInFullList)
{
	GetShowTimeRange(TimeStartInFullList, TimeEndInFullList);
	CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();
	if ( TimeNow >= TimeStartInFullList && TimeNow <= TimeEndInFullList )
	{
		return true;
	}
	
	return false;
}

void CIoViewTick::OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch)
{
	if (NULL == pMerch || pMerch != m_pMerchXml)
		return;

	//if (!IsWindowVisible())
	//{
	//	// 0002149 xl 0705 ����ڼ���󣬶�ʱ���ڱ�����GGTongView��F7�������Ϊ���أ��ᵼ�»�ԭʱ��������δ���¶�������ݵ���ʾ������ȡ�����Ż�
 //		AddFlag(m_uiFlag,CIoViewChart::KMerchTimeSalesUpdate);
 //		ClearLocalData();
 //		return;
	//}
	
	if (m_MerchParamArray.GetSize() <= 0)
	{
		return;
	}
	
	// ������Ʒ������������
	for (int32 i = 0; i < m_MerchParamArray.GetSize(); i ++)
	{
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(i);
		SetCurveTitle(pData);
		
		if (pData->m_pMerchNode == pMerch)
		{
			if (0 == i)
			{
				if (UpdateMainMerchTick2(*pData))
				{
					RequestSequenceTickNext();
					ReDrawAysnc();	
				}
			}
			else
			{
				if (UpdateSubMerchTick(*pData))
				{
					ReDrawAysnc();
				}
			}
		}
	}
}

void CIoViewTick::ClearLocalData(bool32 bClearAll /*= true*/)
{
	for (int32 i = 0; i < m_MerchParamArray.GetSize(); i++)
	{
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(i);
		if ( !bClearAll && !pData->bMainMerch )
		{
			continue;	// ������Ʒ�����
		}
		pData->m_aTicksCompare.RemoveAll();
		pData->m_aKLinesFull.RemoveAll();
		pData->m_aKLinesShowDataFlag.RemoveAll();
		
		if (NULL != pData->m_pKLinesShow)
			pData->m_pKLinesShow->RemoveAll();
		
		RemoveIndexsNodesData(pData);
	}

	m_aAllXTimeIds.RemoveAll();	// ������е�ʱ����ڵ�
}

bool32 CIoViewTick::UpdateMainMerchTick(T_MerchNodeUserData &MerchNodeUserData)
{
	if (m_MerchParamArray.GetSize() <= 0 || m_MerchParamArray[0] != &MerchNodeUserData)
	{
		return false;
	}
	
	//
	CMerch* pMerch = MerchNodeUserData.m_pMerchNode;
	if (NULL == pMerch)
	{
		return false;
	}

	// �жϵ�ǰ��ʾ�� �Ƿ���ʾ���¼�
	// ����ʱʹ�����п���ʹ�õ����ݣ���ʾʱ����ʹ�ò���Ҫ��ʾ������
	CGmtTime TimeStartInFullList, TimeEndInFullList;
	if ( !GetTickMultiDayTimeRange(TimeStartInFullList, TimeEndInFullList) )
	{
		return false;
	}

	// �Ȼ�ȡ��Ӧ��K��
	CMerchTimeSales* pTickRequest = pMerch->m_pMerchTimeSales;

	// �����Ҳ���K�����ݣ� �ǾͲ���Ҫ��ʾ��
	if (NULL == pTickRequest || 0 == pTickRequest->m_Ticks.GetSize())	
	{
		MerchNodeUserData.m_aTicksCompare.RemoveAll();
		MerchNodeUserData.m_aKLinesFull.RemoveAll();

		if (NULL != MerchNodeUserData.m_pKLinesShow)		
			MerchNodeUserData.m_pKLinesShow->RemoveAll();
		
		return true;
	}

	// 
	// �Ż��������������£� ���¼�����������ʷ���ݸ��������ģ� ������������ر��жϴ���
	bool32 bUpdateLast	= false;
	bool32 bModifyLast	= false;
	bool32 bAddSome		= false;

	CTick *pTickSrc = (CTick *)pTickRequest->m_Ticks.GetData();
	int32 iCountSrc   = pTickRequest->m_Ticks.GetSize();

	CTick *pTickCmp = (CTick *)MerchNodeUserData.m_aTicksCompare.GetData();
	
// 	int32 iCountAdd = iCountSrc - iCountCmp;
// 
// 	if (iCountCmp > 1 && (iCountAdd >= 0 && iCountAdd < 3))
// 	{
// 		if (memcmp(pTickSrc, pTickCmp, (iCountCmp - 1) * sizeof(CTick)) == 0)	// ���ںܿ����Ǹ������һ�ʣ� ��ֹ��������һ�����
// 		{
// 			bUpdateLast = true;
// 
// 			if (memcmp(pTickSrc + iCountCmp - 1, pTickCmp + iCountCmp - 1, sizeof(CTick)) == 0)
// 			{
// 				bModifyLast = false;
// 				if (0 == iCountAdd)
// 				{					
// 					bAddSome = false;
// 				}
// 				else
// 				{
// 					// �����˼�������
// 					bAddSome = true;
// 				}
// 			}
// 			else	// ���һ�����ݲ�һ��
// 			{
// 				bModifyLast = true;
// 				if (0 == iCountAdd)
// 				{
// 					// ���������һ������
// 					bAddSome = false;
// 				}
// 				else
// 				{
// 					// ���������һ�����ݲ��һ������˼�������
// 					bAddSome = true;
// 				}
// 			}
// 		}
// 	}
// 		
// 	if ( bUpdateLast && !bModifyLast && !bAddSome )
// 	{
// 		// ʲô��û��, ���ؿ�, �������������ˣ�������Ҫ���¼���
// 		//return true;
// 	}
	//////////////////////////////////////////////////////////////////////
	// ��Ը������¼ۺ͸�����ʷ�ֱ���

	// ...fangz0805 ��ʱû�����Ż�
	bUpdateLast = false;

	if (bUpdateLast)	// �������µļ�������
	{
		if ( false == bModifyLast )
		{
			// ���е����һ����������Ķ�Ӧλ���Ǳ�������ͬ

			if ( false == bAddSome )
			{
				// û������. ��ȫ��ͬ
				return false;
			}
			else
			{
				// ǰ�涼��ͬ, ����¼ӵļ�������:

			}
		}
		else
		{
			// ���е����һ����������Ķ�Ӧλ���Ǳ����ݲ�ͬ
			if ( false == bAddSome)
			{
				// û������,�������һ�ʲ�ͬ,���������¼۷����仯
			}
			else
			{
				// ���һ�ʲ�ͬ���һ������˽ڵ�
			}
		}		
	}
	else	// ȫ������
	{
		// �����������
		ClearLocalData();

		// ��ͼ��������
		if (iCountSrc == 0)
		{
			return true;		// ����û�����ݣ� �Ͳ�������
		}
		
		// 
		if (m_MerchParamArray.GetSize() > 0)
		{
			T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
			pData->m_aTicksCompare.SetSize(iCountSrc, 100);
			
			pTickCmp = (CTick *)pData->m_aTicksCompare.GetData();
			memcpyex(pTickCmp, pTickSrc, iCountSrc * sizeof(CTick));
			
			//
			int32 iTimeInitPerDay = 0, iTimeOpenPerDay = 0, iTimeClosePerDay = 0;
			
			CMerch *pTmpMerch = pData->m_pMerchNode;
			if (NULL == pTmpMerch)
				return false;
			
			// 
			pTmpMerch->m_Market.m_MarketInfo.GetTimeInfo(iTimeInitPerDay, iTimeOpenPerDay, iTimeClosePerDay);				
						
			// ��������
			pData->m_aKLinesFull.SetSize(pData->m_aTicksCompare.GetSize());
			CTick *pTicksCompare = (CTick *)pData->m_aTicksCompare.GetData();
			CKLine *pKLinesFull = (CKLine *)pData->m_aKLinesFull.GetData();
			for (int32 iIndexExchange = 0; iIndexExchange < pData->m_aTicksCompare.GetSize(); iIndexExchange++)
			{
				Tick2KLine(pTicksCompare[iIndexExchange], pKLinesFull[iIndexExchange]); 
			}

			CalcKLineAvgValue(pData->m_aKLinesFull);	// �������

			// ������ʾ���� ����2����
// 			int32 iShowPosStart = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pData->m_aKLinesFull, TimeStartInFullList);
// 			int32 iShowPosEnd	= CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(pData->m_aKLinesFull, TimeEndInFullList);
// 			if (iShowPosStart >= 0 && iShowPosStart < pData->m_aKLinesFull.GetSize()
// 				&& iShowPosEnd >= iShowPosStart && iShowPosEnd < pData->m_aKLinesFull.GetSize() )
// 			{
// 				iShowPosInFullList = iShowPosStart;
// 				iShowCountInFullList = iShowPosEnd-iShowPosStart+1;
// 			}
// 			else
// 			{
// 				// û����Ч����
// 				iShowPosInFullList = 0;	
// 				iShowCountInFullList = 0;
// 			}
// 
// 			//********************************************************************************************************************************************
// 			// ������ͼ�����ָ��
// 			// ����2����
// 			for (int32 j = 0; j < pData->aIndexs.GetSize(); j++)
// 			{
// 				T_IndexParam* pIndex = pData->aIndexs.GetAt(j);
// 				g_formula_compute(this, this, pIndex->pRegion, pData, pIndex, iShowPosInFullList, iShowPosInFullList + iShowCountInFullList);
// 			}
// 
// 			// ���ɵ�ǰ��ʾ������
// 			if (!pData->UpdateShowData(iShowPosInFullList, iShowCountInFullList))
// 			{
// 				//ASSERT(0);
// 				return false;
// 			}

			// ���ݵ�ǰ���ɵ�ʵ�����ݣ������������
		}

		// zhangbo 20090714 #�����䣬 ������Ʒ���ݸ���
		//...

		return true;
	}

	return false;
}

bool32 CIoViewTick::UpdateSubMerchTick(T_MerchNodeUserData &MerchNodeUserData)
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

bool32 CIoViewTick::OnCanPanLeft ( CChartRegion* pRegion, CNodeSequence* pNodes, int32 id, int32 iNum )
{
	CGmtTime Time(id);
	T_MerchNodeUserData* pData = (T_MerchNodeUserData*)pNodes->GetUserData();
	if ( m_pRegionMain == pRegion ||
		 m_pRegionXBottom	== pRegion )
	{
		int32 iSearch = HalfSearch(pData->m_aTicksCompare,id);
		if ( iSearch > 1 )
		{
			return true;
		}
	}
	return false;
}
bool32 CIoViewTick::OnCanPanRight ( CChartRegion* pRegion, CNodeSequence* pNodes, int32 id, int32 iNum )
{
	CGmtTime Time(id);
	T_MerchNodeUserData* pData = (T_MerchNodeUserData*)pNodes->GetUserData();
	if ( m_pRegionMain == pRegion ||
		 m_pRegionXBottom	== pRegion )
	{
		int32 iSearch = HalfSearch(pData->m_aTicksCompare,id);
		if ( iSearch < pData->m_aTicksCompare.GetSize()-1 )
		{
			return true;
		}
	}
	return false;
}

void CIoViewTick::OnNodesRelease ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes )
{
	//T_MerchNodeUserData* pData = (T_MerchNodeUserData*)pNodes->GetUserData();
	//pData->pMerch�Ѿ��������Curveʹ����,���û������Curveʹ��,�����������,���Ҵ�AttendMerch��ɾ��.
}

void CIoViewTick::OnRegionMenu2 ( CChartRegion* pRegion, CDrawingCurve* pCurve, int32 x, int32 y )
{
	RegionMenu2 ( pRegion, pCurve, x, y );
}

void CIoViewTick::OnRegionIndexMenu ( CChartRegion* pRegion, CDrawingCurve* pCurve, int32 x, int32 y )
{
	RegionIndexMenu ( pRegion, pCurve, x, y );
}

void CIoViewTick::OnRegionIndexBtn(CChartRegion* pRegion, CDrawingCurve* pCurve, int32 iID )
{
	RegionIndexBtn (pRegion, pCurve, iID);
}

void CIoViewTick::OnRegionMenu ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes,CNodeData* pNodeData, int32 x, int32 y)
{
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

	

 	CPoint pt(x,y);
 	ClientToScreen(&pt);
 	CNewMenu menu;
 	menu.LoadMenu(IDR_MENU_TICK);
 	menu.LoadToolBar(g_awToolBarIconIDs);
 
 	CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
 	pPopMenu->LoadToolBar(g_awToolBarIconIDs);

	// ȡ������
	{
		pPopMenu->EnableMenuItem(ID_TICK_CANCELZOOM, MF_BYCOMMAND |(IsTickZoomed()?MF_ENABLED:(MF_GRAYED|MF_DISABLED)));
	}

	// �ɽ�����ɫ
	if ( IsShowVolBuySellColor() )
	{
		pPopMenu->CheckMenuItem(ID_TREND_SHOWVOLCOLOR, MF_BYCOMMAND|MF_CHECKED);
	}

	// ��������ͼ
	{
		pPopMenu->CheckMenuItem(ID_TICK_DAY1+m_iTickMultiDay-1, MF_BYCOMMAND|MF_CHECKED);
	}

	AppendStdMenu(pPopMenu);
	pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, AfxGetMainWnd());
// 
// 	/////////////////////////////////////////////////////////////////////////////////////////
// 	//////////////////////////////////////////////////////////////////////////
// 	m_aUserBlockNames.RemoveAll();
// 	
// 	CArray<T_Block, T_Block&> aBlocks;
// 	CUserBlockManager::Instance()->GetBlocks(aBlocks);
// 	int32 iSizeBlock = aBlocks.GetSize();
// 	//////////////////////////////////////////////////////////////////////////
// 
// 	pPopMenu->EnableMenuItem(IDM_CHART_CNP,MF_BYCOMMAND | MF_GRAYED);
// 	
// 	//
// 	pTempMenu = pPopMenu->GetSubMenu(L"��������");
// 	CNewMenu * pIoViewPopMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );
// 	ASSERT(NULL != pIoViewPopMenu );
// 	AppendIoViewsMenu(pIoViewPopMenu);
// 
// 	m_pExportNodes = NULL;
// 	CString StrExport;
// 	if ( NULL != m_pCurvePick && CheckFlag(m_pCurvePick->m_iFlag,CChartCurve::KTypeKLine))
// 	{
// 		CNodeSequence* pNodesi = m_pCurvePick->GetNodes();
// 		if ( NULL != pNodesi )
// 		{
// 			m_pExportNodes = pNodesi;
// 			T_MerchNodeUserData* pDatai = (T_MerchNodeUserData*)pNodesi->GetUserData();
// 			if ( NULL != pDatai )
// 			{
// 				CMerch* pMerchi = pDatai->m_pMerchNode;
// 				if ( NULL != pMerchi )
// 				{
// 					StrExport = _T("����[") + pMerchi->m_MerchInfo.m_StrMerchCnName + _T("]��Excel");
// 				}
// 			}
// 		}
// 	}
// 	if ( 0 == StrExport.GetLength() )
// 	{
// 		m_pExportNodes = m_pRegionMain->GetDependentCurve()->GetNodes();
// 		StrExport = _T("����[") + pMerch->m_MerchInfo.m_StrMerchCnName + _T("]��Excel");
// 	}
// 	menu.ModifyODMenu(IDM_CHART_PROP,MF_BYCOMMAND|MF_STRING,IDM_CHART_PROP,StrExport);
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

void CIoViewTick::UpdateTickStyle()
{
	CChartCurve *pCurveDependent = m_pRegionMain->GetDependentCurve();
	if (NULL == pCurveDependent)
		return;

	CChartDrawer* pTickDrawer = (CChartDrawer*)pCurveDependent->GetDrawer();
	if (NULL == pTickDrawer)
		return;

	CChartDrawer::E_ChartDrawStyle eDrawStyle = pTickDrawer->m_eChartDrawType;
	int32 i, iSize = m_pRegionMain->GetCurveNum();
	for ( i = 0; i < iSize; i ++ )
	{
		CChartCurve* pCurve = m_pRegionMain->GetCurve(i);
		if ( CheckFlag(pCurve->m_iFlag,CChartCurve::KTypeTick))
		{
			CChartDrawer* pDrawer = (CChartDrawer*)pCurve->GetDrawer();
			pDrawer->m_eChartDrawType = eDrawStyle;
		}
	}
}

void CIoViewTick::OnMenu ( UINT nID )
{
	CChartCurve *pCurveDependent = m_pRegionMain->GetDependentCurve();
	if (NULL == pCurveDependent)
		return;

	CChartDrawer* pTickDrawer = (CChartDrawer*)pCurveDependent->GetDrawer();
	if (NULL == pTickDrawer)
		return;

	bool32 bNeedReDraw = false;
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
	case IDM_CHART_INDEX:
		ShowIndex();
		break;
	case IDM_CHART_INDEX_PARAM:
		ShowIndexParam();
		break;
	case IDM_CHART_INDEX_HELP:
		ShowIndexHelp();
		break;
	case IDM_CHART_CANCEL:
		break;
	case IDM_CHART_KSTYLE1:
		pTickDrawer->m_eChartDrawType = CChartDrawer::EKDSTickPrice;
		UpdateTickStyle();
		bNeedReDraw = true;
		break;
	case IDM_CHART_KSTYLE2:
		pTickDrawer->m_eChartDrawType = CChartDrawer::EKDSTickPrice;
		UpdateTickStyle();
		bNeedReDraw = true;
		break;
	case ID_TICK_SHOWTREND:
		{
			if ( NULL != m_pMerchXml )
			{
				CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
				if ( NULL != pMainFrame )
				{
					pMainFrame->OnShowMerchInChart(m_pMerchXml->m_MerchInfo.m_iMarketId
						, m_pMerchXml->m_MerchInfo.m_StrMerchCode
						, ID_PIC_TREND);
				}
			}
		}
		break;
	case ID_TICK_CANCELZOOM:
		{
			if ( IsTickZoomed() )
			{
				CancelZoom();
				OnVDataMerchTimeSalesUpdate(m_pMerchXml);
				bNeedReDraw = true;
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

	if ( nID >= ID_TICK_DAY1 && nID <= ID_TICK_DAY10 )
	{
		SetTickMulitDay(nID-ID_TICK_DAY1+1, true);
	}

	if ( bNeedReDraw )
	{
		m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
		RedrawWindow();
	} 
}

LRESULT CIoViewTick::OnMessageTitleButton(WPARAM wParam,LPARAM lParam)
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
/*		m_pRegionPick = m_pRegionMain;	

		m_FormulaNames.RemoveAll();
		CNewMenu menu;
		menu.LoadMenu(IDR_MENU_KLINE);
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
		
		uint32 iFlag = CFormularContent::KAllowTick;
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

			pSubMenu->AppendODMenu(m_FormulaNames.GetAt(i),MF_STRING,IDM_CHART_INDEX_BEGIN+i);
		}
		
		CPoint pt;
		GetCursorPos(&pt);
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, this);
		menu.DestroyMenu();
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
		menu.LoadMenu(IDR_MENU_KLINE);
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

void CIoViewTick::OnPickNode ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes, CNodeData* pNodeData,int32 x,int32 y,int32 iFlag )
{
	if ( NULL != pNodeData )
	{
		CString StrText;
		T_MerchNodeUserData* pData = (T_MerchNodeUserData*)pNodes->GetUserData();
		ASSERT(NULL!=pData);
		CGmtTime cgTime(pNodeData->m_iID);
		CString StrTime = Time2String ( cgTime, pData->m_eTimeIntervalFull);
		int32 iSaveDec = 0;
		CMerch* pMerch = pData->m_pMerchNode;
		if ( NULL != pMerch )
		{
			iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;
		}
		if ( CheckFlag(pCurve->m_iFlag,CChartCurve::KTypeTick) )
		{
			StrText.Format(_T("ʱ: %s \n��: %s\n��: %s\n��: %s\n��: %s\n��: %s\n��: %s\n"),
				StrTime.GetBuffer(),
				Float2String(pNodeData->m_fOpen,iSaveDec,true).GetBuffer(),
				Float2String(pNodeData->m_fHigh,iSaveDec,true).GetBuffer(),
				Float2String(pNodeData->m_fLow,iSaveDec,true).GetBuffer(),
				Float2String(pNodeData->m_fClose,iSaveDec,true).GetBuffer(),
				Float2String(pNodeData->m_fVolume, iSaveDec, true).GetBuffer(),
				Float2String(pNodeData->m_fAmount,iSaveDec,true).GetBuffer());
		}
		else if ( CheckFlag(pCurve->m_iFlag,CChartCurve::KTypeIndex) )
		{
			StrText.Format(_T("ʱ: %s\n%s: %s\n"),
				           StrTime.GetBuffer(),
						   pNodes->GetName().GetBuffer(),
						   Float2String(pNodeData->m_fClose,iSaveDec,true).GetBuffer());
		}
		m_TipWnd.Show(m_PointMouse, StrText, pNodes->GetName());
	}
	else
	{
		m_TipWnd.Hide();
	}
}

void CIoViewTick::MenuAddRegion()
{
	if ( !AddSubRegion(false))
	{
		return;
	}

	// �õ�����Ʒ����
	T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
	if ( NULL == pData)
	{
		return;
	}

	// �õ��¼ӵ�ChartRegion
	int32 iSize = m_SubRegions.GetSize();
	if ( iSize <= 0)
	{
		return;
	}

	CChartRegion* pRegionAdd = m_SubRegions.GetAt(iSize-1).m_pSubRegionMain;
	
	if (NULL == pRegionAdd)
	{
		return;
	}

	// �õ�����ӵ�����ָ������
	uint32 uiFlag = NodeTimeIntervalToFlag( pData->m_eTimeIntervalFull);	
	AddFlag(uiFlag,CFormularContent::KAllowSub);

	m_FormulaNames.RemoveAll();
	CFormulaLib::instance()->GetAllowNames(uiFlag,m_FormulaNames);
	if ( 0 == m_FormulaNames.GetSize())
	{
		return;
	}	
	
	// �Ҳ�������������,�������:
	CString StrDefaultIndex = m_FormulaNames.GetAt(0);
	// ��ͼ�Ѿ����ڵ�ָ��
	CStringArray StrExist;
	StrExist.RemoveAll();
	
	int32 i = 0;
	for ( i = 0 ; i < pData->aIndexs.GetSize(); i++)
	{
		if ( NULL == pData->aIndexs.GetAt(i)->pRegion || NULL == m_pRegionMain)
		{
			continue;
		}

		if ( pData->aIndexs.GetAt(i)->pRegion != m_pRegionMain )
		{
			StrExist.Add(pData->aIndexs.GetAt(i)->pContent->name);
		}		
	}

	// ɾȥ�ظ���,ʣ�µľ�������ӵ�ָ��:
	DeleteSameString(m_FormulaNames,StrExist);
	//
	CString StrIndexName = L"";
	bool32 bFindMACD = false;
	bool32 bFindKDJ  = false;

	for ( i = 0 ; i < m_FormulaNames.GetSize() ; i++)
	{
		if ( L"MACD" == m_FormulaNames.GetAt(i))
		{
			bFindMACD = true;
			break;
		}

		if (L"KDJ" == m_FormulaNames.GetAt(i))
		{
			bFindKDJ = true;
		}
	}

	if ( bFindMACD)
	{
		StrIndexName = L"MACD";			
	}
	else
	{
		if ( bFindKDJ)
		{
			StrIndexName = L"KDJ";
		}
		else
		{
			if ( m_FormulaNames.GetSize() > 0 )
			{
				StrIndexName = m_FormulaNames.GetAt(0);
			}			
		}
	}
	
	if ( 0 == StrIndexName.GetLength())
	{
		StrIndexName = StrDefaultIndex;
	}

	T_IndexParam* pIndex = AddIndex(pRegionAdd,StrIndexName);
	if ( NULL == pIndex )
	{
		return;
	}				
	int32 iNodeBegin,iNodeEnd;
	GetNodeBeginEnd(iNodeBegin,iNodeEnd);
	g_formula_compute ( this, this, pRegionAdd, pData, pIndex, iNodeBegin,iNodeEnd );
	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	UpdateSelfDrawCurve();
	RedrawWindow();
}

void CIoViewTick::SetXmlSource ()
{
	T_MerchNodeUserData* pData = NULL;
	m_iIntervalXml = -1;
	m_iDrawTypeXml = -1;
	m_iNodeNumXml = 0;
	
	if ( m_MerchParamArray.GetSize() > 0 )
	{
		pData = m_MerchParamArray.GetAt(0);
		m_iIntervalXml = (int32)pData->m_eTimeIntervalFull;

		m_iNodeNumXml = pData->m_pKLinesShow->GetSize();

		CChartCurve *pCurveDependent = m_pRegionMain->GetDependentCurve();
		if (NULL == pCurveDependent)
			return;

		CChartDrawer* pTickDrawer = (CChartDrawer*)pCurveDependent->GetDrawer();
		if (NULL == pTickDrawer)
			return;

		m_iDrawTypeXml = (int)pTickDrawer->m_eChartDrawType;
	}
}

void CIoViewTick::MenuAddIndex ( int32 id)
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
	RedrawWindow();
}
void CIoViewTick::MenuDelRegion()
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
	RedrawWindow();
}
void CIoViewTick::MenuDelCurve()
{
	if ( NULL == m_pCurvePick || NULL == m_pRegionPick )
	{
		return;
	}
	DeleteIndexCurve ( m_pCurvePick );
	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	UpdateSelfDrawCurve();
	RedrawWindow();
}

void CIoViewTick::OnDropCurve ( CChartRegion* pSrcRegion,CChartCurve* pSrcCurve,CChartRegion* pDestChart)
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

	OnVDataMerchTimeSalesUpdate(m_pMerchXml);
}

void CIoViewTick::OnRegionCurvesNumChanged ( CChartRegion* pRegion,int32 iNum )
{
	
}

void CIoViewTick::OnCrossData ( CChartRegion* pRegion,int32 iPos,CNodeData& NodeData, float fPricePrevClose, float fYValue, bool32 bShow)
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
		
		CTick Tick;
		NodeData2Tick(NodeData,Tick);
		CKLine KLine;
		NodeData2KLine(NodeData, KLine);
		
		// int32 iPosAll = pData->m_iShowPosInFullList + iPos;
		// memcpyex(&Tick, (pData->m_aTicksCompare.GetData()+iPosAll), sizeof(CTick));

		if ( pRegion == m_pRegionMain )
		{
			// ��¼�µ�ǰ���K��
			m_TickCrossNow = Tick;
		}
		CString StrTimeLine1,StrTimeLine2;
		Time2String(Tick.m_TimeCurrent.m_Time, ENTIMinute, StrTimeLine1, StrTimeLine2);
		if ( pRegion == m_pRegionMain )
		{
			fPricePrevClose = GetTickPrevClose();
			SetFloatData(&m_GridCtrlFloat,KLine,fPricePrevClose,fYValue,StrTimeLine1,StrTimeLine2,iSaveDec);
		}
		else
		{
			if ( fYValue > 0.0f )
			{
				SetFloatData(&m_GridCtrlFloat,fYValue,iSaveDec);
			}
		}
	}
	else
	{
		m_GridCtrlFloat.ShowWindow(SW_HIDE);
		CRect rct(0,0,0,0);
		EnableClipDiff(false,rct);
	}
}

void CIoViewTick::OnCrossNoData(CString StrTime,bool32 bShow)
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

void CIoViewTick::SetFloatData ( CGridCtrl* pGridCtrl,float fCursorValue,int32 iSaveDec)
{
	if ( !m_bInitialFloat )
	{
		InitCtrlFloat(&m_GridCtrlFloat);
	}

	CString StrValue;
	CGridCellSymbol* pCellSymbol;

	//��ֵ
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(3,0);
	StrValue = Float2String(fCursorValue, iSaveDec, true) + _T(" ");
	pCellSymbol->SetText(StrValue);
	pGridCtrl->RedrawWindow();
}

void CIoViewTick::SetFloatData ( CGridCtrl* pGridCtrl,CTick& Tick,float fPricePrevClose,float fCursorValue,CString StrTimeLine1,CString StrTimeLine2,int32 iSaveDec)
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
	CGridCellSymbol* pCellSymbol;

	// xl 0001762 ������еĿո���ʾ������������ʾ���ȣ����鴰�ڵĿ�ȱ�YLeft�ܶ���ʾһ������ - -
	int32 iYLeftShowChar = GetYLeftShowCharWidth();
	iYLeftShowChar += 1;

	CString StrValue;

	//ʱ��
	CGridCellSys* pCellSys = (CGridCellSys* )pGridCtrl->GetCell(0,0);
	pCellSys->SetText(StrTimeLine1 + _T(""));

	pCellSys = (CGridCellSys*)pGridCtrl->GetCell(1,0);
	pCellSys->SetText(StrTimeLine2 + _T(""));	

	//�۸�
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(3,0);

	//StrValue = Float2SymbolString(Tick.m_fPrice,fPricePrevClose,iSaveDec) + _T(" ");
	Float2SymbolStringLimitMaxLength(StrValue, Tick.m_fPrice, fPricePrevClose, iSaveDec, iYLeftShowChar, true);
	pCellSymbol->SetText(StrValue);	

	//����
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(5,0);
	pCellSymbol->SetText(_T(" -"));	

	//�ǵ�=����-����
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(7,0);
	
	StrValue = L" -";
	if (0. != Tick.m_fPrice)
	{
		//StrValue = Float2SymbolString(Tick.m_fPrice - fPricePrevClose, 0.0f, iSaveDec) + _T(" ");
		Float2SymbolStringLimitMaxLength(StrValue, Tick.m_fPrice - fPricePrevClose, 0.0f, iSaveDec, iYLeftShowChar, true, false, false);
	}
	pCellSymbol->SetText(StrValue);

	//�ǵ���(����-����)/����*100%
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(9,0);
	
	StrValue = L" -";
	if (0. != Tick.m_fPrice && 0. != fPricePrevClose)
	{
		float fRisePercent = ((Tick.m_fPrice - fPricePrevClose) / fPricePrevClose) * 100.;
		StrValue = Float2SymbolString(fRisePercent, 0, 2, false, false, true) + _T("");
	}	
	pCellSymbol->SetText(StrValue);	

	//�ɽ���
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(11,0);
 	CString StrVolume = Float2String(Tick.m_fVolume, 0, true) + _T(" ");
 	pCellSymbol->SetText(StrVolume);	
	Float2StringLimitMaxLength(StrValue, Tick.m_fVolume, 0, GetYLeftShowCharWidth(1), true, true);
	pCellSymbol->SetText(StrValue);
	
	//
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(13,0);	
	if ( bFutures )
	{
		// �ֲ�
		pCellSymbol->SetDefaultTextColor(ESCVolume);
// 		CString StrHold = Float2String(Tick.m_fHold, 0, true) + _T(" ");
// 		pCellSymbol->SetText(StrHold);
		Float2StringLimitMaxLength(StrValue, Tick.m_fHold, 0, GetYLeftShowCharWidth(1), true, true);
		pCellSymbol->SetText(StrValue);
	}
	else
	{
		// ���
		pCellSymbol->SetDefaultTextColor(ESCAmount);
// 		CString StrAmount = Float2String(Tick.m_fPrice*Tick.m_fVolume, 0, true) + _T(" ");
// 		pCellSymbol->SetText(StrAmount);
		Float2StringLimitMaxLength(StrValue, Tick.m_fPrice*Tick.m_fVolume, 0, GetYLeftShowCharWidth(1), true, true);
		pCellSymbol->SetText(StrValue);
	}	
	
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

void CIoViewTick::SetFloatData( CGridCtrl* pGridCtrl,CKLine& KLine,float fPricePrevClose,float fCursorValue,CString StrTimeLine1,CString StrTimeLine2,int32 iSaveDec )
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
	CGridCellSymbol* pCellSymbol;

	// xl 0001762 ������еĿո���ʾ������������ʾ���ȣ����鴰�ڵĿ�ȱ�YLeft�ܶ���ʾһ������ - -
	int32 iYLeftShowChar = GetYLeftShowCharWidth();
	iYLeftShowChar += 1;

	CString StrValue;

	//ʱ��
	CGridCellSys* pCellSys = (CGridCellSys* )pGridCtrl->GetCell(0,0);
	pCellSys->SetText(StrTimeLine1 + _T(""));

	pCellSys = (CGridCellSys*)pGridCtrl->GetCell(1,0);
	pCellSys->SetText(StrTimeLine2 + _T(""));	

	//�۸�
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(3,0);

	//StrValue = Float2SymbolString(Tick.m_fPrice,fPricePrevClose,iSaveDec) + _T(" ");
	Float2SymbolStringLimitMaxLength(StrValue, KLine.m_fPriceClose, fPricePrevClose, iSaveDec, iYLeftShowChar, true);
	pCellSymbol->SetText(StrValue);	

	//����
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(5,0);
	Float2SymbolStringLimitMaxLength(StrValue, KLine.m_fPriceAvg, fPricePrevClose, iSaveDec, iYLeftShowChar, true);
	pCellSymbol->SetText(StrValue);	

	//�ǵ�=����-����
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(7,0);
	
	StrValue = L" -";
	if (0. != KLine.m_fPriceClose)
	{
		//StrValue = Float2SymbolString(Tick.m_fPrice - fPricePrevClose, 0.0f, iSaveDec) + _T(" ");
		Float2SymbolStringLimitMaxLength(StrValue, KLine.m_fPriceClose - fPricePrevClose, 0.0f, iSaveDec, iYLeftShowChar, true, false, false);
	}
	pCellSymbol->SetText(StrValue);

	//�ǵ���(����-����)/����*100%
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(9,0);
	
	StrValue = L" -";
	if (0. != KLine.m_fPriceClose && 0. != fPricePrevClose)
	{
		float fRisePercent = ((KLine.m_fPriceClose - fPricePrevClose) / fPricePrevClose) * 100.;
		StrValue = Float2SymbolString(fRisePercent, 0, 2, false, false, true) + _T("");
	}	
	pCellSymbol->SetText(StrValue);	

	//�ɽ���
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(11,0);
 	CString StrVolume = Float2String(KLine.m_fVolume, 0, true) + _T(" ");
 	pCellSymbol->SetText(StrVolume);	
	Float2StringLimitMaxLength(StrValue, KLine.m_fVolume, 0, GetYLeftShowCharWidth(1), true, true);
	pCellSymbol->SetText(StrValue);
	
	//
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(13,0);	
	if ( bFutures )
	{
		// �ֲ�
		pCellSymbol->SetDefaultTextColor(ESCVolume);
// 		CString StrHold = Float2String(Tick.m_fHold, 0, true) + _T(" ");
// 		pCellSymbol->SetText(StrHold);
		Float2StringLimitMaxLength(StrValue, KLine.m_fHold, 0, GetYLeftShowCharWidth(1), true, true);
		pCellSymbol->SetText(StrValue);
	}
	else
	{
		// ���
		pCellSymbol->SetDefaultTextColor(ESCAmount);
// 		CString StrAmount = Float2String(Tick.m_fPrice*Tick.m_fVolume, 0, true) + _T(" ");
// 		pCellSymbol->SetText(StrAmount);
		Float2StringLimitMaxLength(StrValue, KLine.m_fPriceClose*KLine.m_fVolume, 0, GetYLeftShowCharWidth(1), true, true);
		pCellSymbol->SetText(StrValue);
	}	
	
	if ( !pGridCtrl->IsWindowVisible() )
	{
		pGridCtrl->ShowWindow(SW_SHOW|SW_SHOWNOACTIVATE);
		CRect rct;
		pGridCtrl->GetClientRect(&rct);
		OffsetRect(&rct,m_PtGridCtrl.x,m_PtGridCtrl.y);
		EnableClipDiff(true,rct);
	}

	pGridCtrl->RedrawWindow();
}

void CIoViewTick::ClipGridCtrlFloat (CRect& rect)
{
	EnableClipDiff(true,rect);
}

CString CIoViewTick::OnTime2String ( CGmtTime& Time )
{
	CString StrTime = Time2String ( Time,ENTIMinute );
	if ( m_MerchParamArray.GetSize() > 0 )
	{
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
		StrTime = Time2String(Time,pData->m_eTimeIntervalFull);
	}
	return StrTime;
}

CString CIoViewTick::OnFloat2String ( float fValue, bool32 bZeroAsHLine, bool32 bNeedTerminate/* = false*/ )
{
	return BaseFloat2String(fValue,bZeroAsHLine, bNeedTerminate);
}

void CIoViewTick::OnCalcXAxis(CChartRegion* pRegion, CDC* pDC, CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aXAxisDivide)
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
		//CalcMainRegionXAxis(aAxisNodes, aXAxisDivide);	

		// ��ͼCopy
		ASSERT( NULL != m_pRegionMain );
		if ( NULL != m_pRegionMain )
		{
			aAxisNodes.Copy(m_pRegionMain->m_aXAxisNodes);
			aXAxisDivide.Copy(m_pRegionMain->m_aXAxisDivide);
		}
	}
}
	
void CIoViewTick::OnCalcYAxis(CChartRegion* pRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide)
{
	aYAxisDivide.SetSize(0);
	
	if ( pRegion == m_pRegionMain )
	{
		// ��ͼ��Y����ʾ
		CalcMainRegionYAxis(pDC, aYAxisDivide);
	}
	else	
	{
		// ��ͼ��Y����ʾ
		for ( int32 i = 0 ; i < m_SubRegions.GetSize(); i++)
		{
			if ( pRegion == m_SubRegions.GetAt(i).m_pSubRegionMain )
			{
				CalcSubRegionYAxis(pRegion,pDC,aYAxisDivide);
			}
		}		
	}
}

///////////////////////////////////////////////////
//
void CIoViewTick::OnRegionDrawNotify(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC,CRegion* pRegion,E_RegionDrawNotifyType eType)
{
	//
	CChartRegion* pChartRegion = (CChartRegion*)pRegion;
	
	CFont* pFont = GetIoViewFontObject(ESFSmall);
	CFont* pOldFont = pDC->SelectObject(pFont);

	if ( eType == ERDNBeforeTransformAllRegion )
	{
		return;
	}

	if ( eType == ERDNAfterDrawCurve )
	{
		//��ͼ
		if ( pChartRegion == m_pRegionMain )
		{
			DrawTitle1(pDC);
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
	}

	CRect rect;
	CPoint pt1,pt2;
	COLORREF color;
	CPen pen,*pOldPen;
	
	color = GetIoViewColor(ESCChartAxisLine);
	pen.CreatePen(PS_SOLID,1,color);
	pOldPen = (CPen*)pDC->SelectObject(&pen);

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
				CAxisDivide *pAxisDivide = (CAxisDivide *)m_pRegionMain->m_aYAxisDivide.GetData();
				for (int32 i = 0; i < m_pRegionMain->m_aYAxisDivide.GetSize(); i++)
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
				for (int32 i = 0; i < m_pRegionMain->m_aXAxisDivide.GetSize(); i++)
				{
					CAxisDivide &AxisDivide = pAxisDivide[i];
					pt1.x = pt2.x = AxisDivide.m_iPosPixel;
					pt1.y = RectMain.top;
					pt2.y = RectMain.bottom;

					if ( AxisDivide.m_iPosPixel <= RectMain.left+1
						|| AxisDivide.m_iPosPixel >= RectMain.right-1 )
					{
						continue;	// ���ڿ�������߽�Ĳ���
					}
					
					if (CAxisDivide::ELSSolid == AxisDivide.m_eLineStyle)
					{
						pDC->_DrawLine(pt1, pt2);
					}
					else if (CAxisDivide::ELS3LineSolid == AxisDivide.m_eLineStyle)
					{
						pt1.x--; pt2.x--;
						pDC->_DrawLine(pt1, pt2);
						
						pt1.x += 2; pt2.x += 2;
						pDC->_DrawLine(pt1, pt2);
						
						pt1.x--; pt2.x--;
						pDC->_DrawLine(pt1, pt2);
					}
					else if (CAxisDivide::ELSDoubleSolid == AxisDivide.m_eLineStyle)
					{
						pt1.x--; pt2.x--;
						pDC->_DrawLine(pt1, pt2);
						
						pt1.x++; pt2.x++;
						pDC->_DrawLine(pt1, pt2);
					}
					else if (CAxisDivide::ELSDot == AxisDivide.m_eLineStyle)
					{
						pDC->_DrawDotLine(pt1, pt2, 2, color);
					}
				}
			}
			
		}
		else if ( pChartRegion == m_pRegionYLeft)
		{
			CRect RectYLeft = m_pRegionYLeft->GetRectCurves();
			RectYLeft.DeflateRect(3, 3, 3, 3);

			// ��������
			CAxisDivide *pAxisDivide = (CAxisDivide *)m_pRegionMain->m_aYAxisDivide.GetData();
			for (int32 i = 0; i < m_pRegionMain->m_aYAxisDivide.GetSize(); i++)
			{
				CAxisDivide &AxisDivide = pAxisDivide[i];

				CRect tmpRct = RectYLeft;
				if ((AxisDivide.m_DivideText1.m_uiTextAlign & DT_VCENTER) != 0)						
				{
					tmpRct.top	= AxisDivide.m_iPosPixel - iHalfHeight;
					tmpRct.bottom = AxisDivide.m_iPosPixel + iHalfHeight;
				}
				else if ((AxisDivide.m_DivideText1.m_uiTextAlign & DT_BOTTOM) != 0)
				{
					tmpRct.bottom = AxisDivide.m_iPosPixel;
					tmpRct.top	= tmpRct.bottom - iFontHeight;							
				}
				else
				{
					tmpRct.top	= AxisDivide.m_iPosPixel;
					tmpRct.bottom	= tmpRct.top + iFontHeight;
				}
				
				//
				if ( tmpRct.bottom > RectYLeft.bottom )
				{
					continue;
				}

				pDC->SetTextColor(AxisDivide.m_DivideText1.m_lTextColor);
				pDC->SetBkMode(TRANSPARENT);
				pDC->DrawText(AxisDivide.m_DivideText1.m_StrText, &tmpRct, AxisDivide.m_DivideText1.m_uiTextAlign);
			}
		}
		else if (pChartRegion == m_pRegionYRight)
		{
			CRect RectYRight= m_pRegionYRight->GetRectCurves();
			RectYRight.DeflateRect(3, 3, 3, 3);

			// ��������
			CAxisDivide *pAxisDivide = (CAxisDivide *)m_pRegionMain->m_aYAxisDivide.GetData();
			for (int32 i = 1; i < m_pRegionMain->m_aYAxisDivide.GetSize(); i++)
			{
				CAxisDivide &AxisDivide = pAxisDivide[i];
			
				CRect tmpRect = RectYRight;
				if ((AxisDivide.m_DivideText2.m_uiTextAlign & DT_VCENTER) != 0)						
				{
					tmpRect.top	= AxisDivide.m_iPosPixel - 50;
					tmpRect.bottom = AxisDivide.m_iPosPixel + 50;
				}
				else if ((AxisDivide.m_DivideText2.m_uiTextAlign & DT_BOTTOM) != 0)
				{
					tmpRect.bottom = AxisDivide.m_iPosPixel;
					tmpRect.top	= tmpRect.bottom - 100;							
				}
				else
				{
					tmpRect.top	= AxisDivide.m_iPosPixel;
					tmpRect.bottom	= tmpRect.top + 100;
				}
				
				// 
				pDC->SetTextColor(AxisDivide.m_DivideText2.m_lTextColor);
				pDC->SetBkMode(TRANSPARENT);
				pDC->DrawText(AxisDivide.m_DivideText2.m_StrText, &tmpRect, AxisDivide.m_DivideText2.m_uiTextAlign);
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

					CRect rt = RectXBottom;
					if ((pAxisDivide[i].m_DivideText1.m_uiTextAlign & DT_CENTER) != 0)						
					{
						rt.left	= pAxisDivide[i].m_iPosPixel - (SizeText.cx / 2 + iWidthSpace);
						rt.right	= pAxisDivide[i].m_iPosPixel + (SizeText.cx / 2 + iWidthSpace);
					}
					else if ((pAxisDivide[i].m_DivideText1.m_uiTextAlign & DT_RIGHT) != 0)
					{
						rt.right	= pAxisDivide[i].m_iPosPixel + iWidthSpace;
						rt.left	= pAxisDivide[i].m_iPosPixel - (SizeText.cx + iWidthSpace);
					}
					else
					{
						rt.left	= pAxisDivide[i].m_iPosPixel - iWidthSpace;
						rt.right	= pAxisDivide[i].m_iPosPixel + (SizeText.cx + iWidthSpace);
					}

					// �ӿ��пռ��в��ң� �����Ƿ������ʾ
					int32 iPosFindInSpaceRects = -1;

					CRect *pRectSpace = (CRect *)aRectSpace.GetData();
					for (int32 iIndexSpace = 0; iIndexSpace < aRectSpace.GetSize(); iIndexSpace++)
					{
						if (rt.left < pRectSpace[iIndexSpace].left || rt.right > pRectSpace[iIndexSpace].right)
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
						pDC->DrawText(pAxisDivide[i].m_DivideText1.m_StrText, &rt, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

						// ��ֿ�������
						CRect RectSubLeft = pRectSpace[iPosFindInSpaceRects];
						CRect RectSubRight = pRectSpace[iPosFindInSpaceRects];

						RectSubLeft.right = rt.left - 1;
						RectSubRight.left = rt.right + 1;

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
			
			CGmtTime TimeStart, TimeEnd;
			if (NULL != pData && 1<=m_aTickMultiDayIOCTimes.GetSize() && GetShowTimeRange(TimeStart, TimeEnd))
			{
				// 
				CTime TimeOpen(TimeStart.GetTime());
				
				int32 iYear = TimeOpen.GetYear();
				iYear -= 2000;
				if (iYear < 0)
					iYear = 100 - iYear;
				
				StrDay.Format(L"%02d/%02d/%02d", iYear, TimeOpen.GetMonth(), TimeOpen.GetDay());
				
			}
				
			if (StrDay.GetLength() > 0)
			{
				//����ʱ��
				COLORREF clr = 0xffffff;
				clr = GetIoViewColor(ESCText);
				pDC->SetTextColor(clr);
				int OldMode = pDC->GetBkMode();
				pDC->SetBkMode(TRANSPARENT);

				CRect rct = m_pRegionLeftBottom->GetRectCurves();
				pDC->DrawText(StrDay,rct,DT_CENTER | DT_VCENTER | DT_SINGLELINE );
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
					
					// ��ˮƽ��				
					{
						CAxisDivide *pAxisDivide = (CAxisDivide *)SubRegion.m_pSubRegionMain->m_aYAxisDivide.GetData();
					    //�����ͼû������ʱ��subregionmain����û������2013-7-25
						{
							CPoint ptTopLeft, ptTopRight;
							
							ptTopLeft.y = RectSubMain.top;
							ptTopRight.y = RectSubMain.top;
							ptTopLeft.x = RectSubMain.left;
							ptTopRight.x = RectSubMain.right;
							pDC->_DrawDotLine(ptTopLeft, ptTopRight, 2, color);
						}
						
						for (int32 i = 0; i < SubRegion.m_pSubRegionMain->m_aYAxisDivide.GetSize(); i++)
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
						for (int32 i = 0; i < m_pRegionMain->m_aXAxisDivide.GetSize(); i++)
						{
							CAxisDivide &AxisDivide = pAxisDivide[i];
							pt1.x = pt2.x = AxisDivide.m_iPosPixel;
							pt1.y = RectSubMain.top;
							pt2.y = RectSubMain.bottom;

							if ( AxisDivide.m_iPosPixel <= RectSubMain.left+1
								|| AxisDivide.m_iPosPixel >= RectSubMain.right-1 )
							{
								continue;	// ���ڿ�������߽�Ĳ���
							}
							
							if (CAxisDivide::ELSSolid == AxisDivide.m_eLineStyle)
							{
								pDC->_DrawLine(pt1, pt2);
							}
							else if (CAxisDivide::ELS3LineSolid == AxisDivide.m_eLineStyle)
							{
								pt1.x--; pt2.x--;
								pDC->_DrawLine(pt1, pt2);
								
								pt1.x += 2; pt2.x += 2;
								pDC->_DrawLine(pt1, pt2);
								
								pt1.x--; pt2.x--;
								pDC->_DrawLine(pt1, pt2);
							}
							else if (CAxisDivide::ELSDoubleSolid == AxisDivide.m_eLineStyle)
							{
								pt1.x--; pt2.x--;
								pDC->_DrawLine(pt1, pt2);
								
								pt1.x++; pt2.x++;
								pDC->_DrawLine(pt1, pt2);
							}
							else if (CAxisDivide::ELSDot == AxisDivide.m_eLineStyle)
							{
								pDC->_DrawDotLine(pt1, pt2, 2, color);
							}
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
						
						CRect tmpRect = RectYLeft;
						if ((AxisDivide.m_DivideText1.m_uiTextAlign & DT_VCENTER) != 0)						
						{
							tmpRect.top	= AxisDivide.m_iPosPixel - iHalfHeight;
							tmpRect.bottom = AxisDivide.m_iPosPixel + iHalfHeight;
						}
						else if ((AxisDivide.m_DivideText1.m_uiTextAlign & DT_BOTTOM) != 0)
						{
							tmpRect.bottom = AxisDivide.m_iPosPixel;
							tmpRect.top	= tmpRect.bottom - iFontHeight;							
						}
						else
						{
							tmpRect.top	= AxisDivide.m_iPosPixel;
							tmpRect.bottom	= tmpRect.top + iFontHeight;
						}
						
						// 
						if ( tmpRect.bottom > RectSubMain.bottom )
						{
							continue;
						}

						pDC->SetTextColor(AxisDivide.m_DivideText1.m_lTextColor);
						pDC->SetBkMode(TRANSPARENT);
						pDC->DrawText(AxisDivide.m_DivideText1.m_StrText, &tmpRect, AxisDivide.m_DivideText1.m_uiTextAlign);
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
		DrawIndexExtraY ( pDC, pChartRegion );

	}
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldFont);
	pen.DeleteObject();	
}

///////////////////////////////////////////////////////////////////////////////
void CIoViewTick::OnSliderId ( int32& id, int32 iJump )
{
	SliderId ( id, iJump );
}

CString CIoViewTick::OnGetChartGuid ( CChartRegion* pRegion )
{
	return GetChartGuid(pRegion );
}

void CIoViewTick::InitialShowNodeNums()
{
	m_iNodeCountPerScreen = m_iNodeNumXml;
	if ( m_iNodeCountPerScreen < KMinTickNumPerScreen || m_iNodeCountPerScreen > KMaxTickNumPerScreen)
	{
		m_iNodeCountPerScreen = KDefaultTickNumPerScreen;
	}
}

void CIoViewTick::DoFromXml()
{
// 	if ( m_bFromXml )
// 	{
// 		CIoViewChart::DoFromXml();
// 		return;
// 	}

	UpdateAxisSize(false);

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
}

IChartBrige CIoViewTick::GetRegionParentIoView()
{
	IChartBrige ChartRegion;
	ChartRegion.pWnd = this;
	ChartRegion.pIoViewBase = this;
	ChartRegion.pChartRegionData = this;
	return ChartRegion;
}

void CIoViewTick::SetChildFrameTitle()
{
	CString StrTitle;
	StrTitle =  CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;

	if (NULL != m_pMerchXml)
	{
		StrTitle  += L" ";
		StrTitle  += m_pMerchXml->m_MerchInfo.m_StrMerchCnName;
	}
	
	CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pParentFrame)
	{
		pParentFrame->SetChildFrameTitle(StrTitle);
	}
}

void CIoViewTick::GetCrossTick(OUT CTick & TickIn)
{
	 memset((void*)(&TickIn),0,sizeof(Kline));
	 memcpyex((void*)(&TickIn),(void*)(&m_TickCrossNow),sizeof(Kline));
}

void CIoViewTick::OnIoViewActive()
{
	CIoViewChart::OnIoViewActive();
}

void CIoViewTick::CalcLayoutRegions(bool bOnlyUpdateMainRegion)
{
	if (m_MerchParamArray.GetSize() <= 0)
		return;

	m_eMainCurveAxisYType = CPriceToAxisYObject::EAYT_Pecent;	// ��֧�ְٷֱ�

	// ����Y����������ɶ��ٸ��̶�
	float fMinYRange = 0.0f;
	if ( NULL != m_pMerchXml )
	{
		CRect rect = m_pRegionMain->GetRectCurves();
		
		CClientDC dc(this);
		CFont* pOldFont = dc.SelectObject(GetIoViewFontObject(ESFSmall));					
		int32 iHeightText = dc.GetTextExtent(_T("����ֵ")).cy;
		dc.SelectObject(pOldFont);
		
		int32 iHeightSub = iHeightText + 12;
		int32 iDivideCount = rect.Height() / iHeightSub + 1;
		if ( iDivideCount % 2 != 0 )	iDivideCount--;	// ż��
		iDivideCount /= 2;	// ȡһ��

		double iSaveDec = m_pMerchXml->m_MerchInfo.m_iSaveDec;
		ASSERT( iSaveDec >= 0 );
		//float fPricePrevClose = pMerchData->m_TrendTradingDayInfo.GetPrevReferPrice();	// ����
		float fPricePrevClose = GetTickPrevClose();
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

// ��ͼ�Ż� - ����ʼ���ִ�
bool32 CIoViewTick::CalcMainRegionXAxis(CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aAxisDivide)
{ 
	ASSERT(NULL != m_pRegionMain);

	aAxisNodes.SetSize(0);
	aAxisDivide.SetSize(0);

	// 
	if (m_MerchParamArray.GetSize() <= 0)
	{
		// ���ܴ����½���K ����ͼ,û������.
		return true;
	}

	T_MerchNodeUserData* pMainData = m_MerchParamArray.GetAt(0);
	if (NULL == pMainData || NULL==pMainData->m_pKLinesShow)
		return false;

	CGmtTime TimeStart, TimeEnd;
	if ( !GetShowTimeRange(TimeStart, TimeEnd) )
	{
		return false;
	}

	bool32 bSaveMinute = false;
	
	// E_NodeTimeInterval eTimeInterval = pMainData->m_eTimeIntervalFull;
	E_NodeTimeInterval eTimeInterval = ENTIMinute;

	if (ENTIMinute == eTimeInterval || ENTIMinute5 == eTimeInterval || ENTIMinute15 == eTimeInterval || ENTIMinute30 == eTimeInterval || ENTIMinute60 == eTimeInterval || ENTIMinuteUser == eTimeInterval)
		bSaveMinute = true;
	else if (ENTIDay == eTimeInterval || ENTIWeek == eTimeInterval || ENTIMonth == eTimeInterval || ENTIQuarter == eTimeInterval || ENTIYear == eTimeInterval || ENTIDayUser == eTimeInterval)
	{
		bSaveMinute = false;
	}
	else	// ��Ӧ�ó��ֵ�����
	{
		return false;
	}

	// �ָ��߷ָ�ʱ�䵥λ
	enum E_DivideUnitType
	{
		EDUTNone	= 0,
		EDUTByHour,
		EDUTByDay,
		EDUTByMonth,
		EDUTByYear,
	};
	
	E_DivideUnitType eDivideUnitType = EDUTNone;

	if (ENTIMinute == eTimeInterval || ENTIMinute5 == eTimeInterval || ENTIMinute15 == eTimeInterval || ENTIMinuteUser == eTimeInterval)
	{
		// ��СʱΪ�̶ȵ�λ��ʾ
		// eDivideUnitType = EDUTByHour;
		// ...fangz0811
		eDivideUnitType = EDUTByDay;
	}
	else if (ENTIMinute30 == eTimeInterval || ENTIMinute60 == eTimeInterval)
	{
		// ����Ϊ�̶ȵ�λ��ʾ
		eDivideUnitType = EDUTByDay;
	}
	else if (ENTIDay == eTimeInterval || ENTIWeek == eTimeInterval || ENTIDayUser == eTimeInterval)	
	{
		// ����Ϊ�̶ȵ�λ��ʾ
		eDivideUnitType = EDUTByMonth;
	}
	else if (ENTIMonth == eTimeInterval || ENTIQuarter == eTimeInterval)
	{
		// ����Ϊ�̶ȵ�λ��ʾ
		eDivideUnitType = EDUTByYear;
	}
	else
	{
		NULL;
	}

	// 
	CArray<CNodeData, CNodeData&> aNewNodes;
	FillAppendMainXPosNode(pMainData->m_pKLinesShow->m_aNodes, aNewNodes);
	const int32 iNodeDataCount = aNewNodes.GetSize();
	CNodeData *pNodeDatas = aNewNodes.GetData();
	if ( NULL == pNodeDatas || iNodeDataCount <= 0 )
	{
		return false;	// ������updatemain���������ݶ��Ѿ�׼�����ˣ����Բ��ó������֣�Ҫ���͸ɴ�û��Ʒ
	}
	

	// ��һ�ִ���:
	// �Ȱ���ʱ������ж��ٸ���
	// �����ڸ÷����������ݵģ���Ϊ�÷���ӵ��һ�������ݵ㣬�����꣬������
	// Ȼ���ټ���ÿ�����ӵ�ж������ؿ��
	// �����ݶ��յ��ռ���ָ��λ��������
	// ʵ����һ��
	// ���ݿ�����ʱ��, �����ȵ�ʱ�䵽���ʱ��
	// ��Ҫ��UpdateMain�д���ã���Щ����Ҫ��������Щ����Ҫ����
	

	// 
	CRect rect = m_pRegionMain->GetRectCurves();
	rect.DeflateRect(2, 1, 1, 1);
	//int32 iTickUnitCount = m_iNodeCountPerScreen;
	int32 iTickUnitCount = iNodeDataCount;
	if ( iTickUnitCount <= 0 )
	{
		ASSERT( 0 );
		return false;	// û����ʾ������
	}
	float fPixelWidthPerUnit = (rect.Width() - 2) / (float)iTickUnitCount;	// ����ֱ��С�����
	
	// 
	aAxisNodes.SetSize(iTickUnitCount);
	
	int32 iMaxAxisDivideSize = iTickUnitCount / 1;
	if (iMaxAxisDivideSize < 20)	iMaxAxisDivideSize = 20;
	aAxisDivide.SetSize(iMaxAxisDivideSize);

	//
	CAxisNode *pAxisNode = (CAxisNode *)aAxisNodes.GetData();
	CAxisDivide *pAxisDivide = (CAxisDivide *)aAxisDivide.GetData();
	
	int32 iIndexNode = 0, iIndexDivide = 0;

	bool32	bMultiDay = m_aTickMultiDayIOCTimes.GetSize() > 1;
	bool32	bShowHourInMultiDay = m_aTickMultiDayIOCTimes.GetSize() <= 3;
	
	// ���ݿ�����ʱ��, �����ȵ�ʱ�䵽���ʱ��
	int32 i = 0;
	for ( i=m_aTickMultiDayIOCTimes.GetUpperBound(); i >=0 && iIndexNode <iTickUnitCount ; --i )
	{
		const CMarketIOCTimeInfo &IOCTime = m_aTickMultiDayIOCTimes[i];
		if ( IOCTime.m_TimeEnd.m_Time < TimeStart )
		{
			continue;	// ����Ľ���ʱ�����ʾ��ʼС��������ʾ����
		}
		if ( TimeEnd < IOCTime.m_TimeInit.m_Time )
		{
			break;	// ����Ŀ�ʼʱ�����ʾ�����󣬲��ؼ�����ʾ��
		}

		for ( ; iIndexNode < iTickUnitCount ; )
		{
			int32 iTimeId = 0;
			if ( iIndexNode < iNodeDataCount )
			{
				iTimeId = pNodeDatas[iIndexNode].m_iID;
			}
			else
			{
				ASSERT( 0 );
				break;	// û�и����ݵ���
			}
			
			if ( iTimeId < IOCTime.m_TimeInit.m_Time.GetTime()
				|| iTimeId > IOCTime.m_TimeEnd.m_Time.GetTime() )
			{
				break;	// �������ʱ��ε�, ���¸�ʱ���ȥ����
			}
			
			CAxisNode &AxisNode = pAxisNode[iIndexNode];
			
			AxisNode.m_iTimeId = iTimeId;
			AxisNode.m_fPixelWidthPerUnit = fPixelWidthPerUnit;
			
			// �ڵ��� 
			if (0 == iIndexNode)
			{
				AxisNode.m_iStartPixel = rect.left;		// ��������߿�ճ�һ������
			}
			else
			{
				AxisNode.m_iStartPixel = pAxisNode[iIndexNode - 1].m_iEndPixel + 1;
			}
			
			AxisNode.m_iEndPixel = rect.left + (int32)(((iIndexNode + 1) * fPixelWidthPerUnit + 0.5)) - 1;
			
			//
			if (AxisNode.m_iStartPixel > AxisNode.m_iEndPixel)
				AxisNode.m_iStartPixel = AxisNode.m_iEndPixel;
			
			// �нڵ�
			AxisNode.m_iCenterPixel = (AxisNode.m_iStartPixel + AxisNode.m_iEndPixel) / 2;
			iIndexNode++;
			// �����ڵ㴦�����
		} // iIndexNode < iTickUnitCount
		
	}//for ( int32 i=m_aTickMultiDayIOCTimes.GetUpperBound(); i >=0 && iIndexNode <iTickUnitCount ; --i )

	// �����ڵ㴦����ϣ���ʼ�ָ��Ĵ���
	// �ָ�㴦��
	// ����:
	//		30����һ��dot�ָ���, �볡��ʵ��, ��ʾ30����ʱ��&�볡
	// ����:
	//		<=3�գ�60����һ��dot�ָ���, �볡ʵ��, ��ʵ, ��ʾ�볡&��
	//		>3�գ��볡dot�ָ���, һ��ʵ, ��ʾ�볡&��
	// ����ڷָ��ʱû�����ݣ�����ǰ��������֮����е���Ϊ�ָ���λ��
	// ������״̬�£����ݱ����AllTimeId�ж��Ƿ�Ҫ��ʾ�����뿪����
	bool32 bZoomed = IsTickZoomed();
	int32 iNodePos = 0;
	for ( i=m_aTickMultiDayIOCTimes.GetUpperBound(); i >=0 && iNodePos<iIndexNode ; --i )
	{
		const CMarketIOCTimeInfo &IOCTime = m_aTickMultiDayIOCTimes[i];
		if ( IOCTime.m_TimeEnd.m_Time < TimeStart )
		{
			continue;	// ����Ľ���ʱ�����ʾ��ʼС��������ʾ����
		}
		if ( TimeEnd < IOCTime.m_TimeInit.m_Time )
		{
			break;	// ����Ŀ�ʼʱ�����ʾ�����󣬲��ؼ�����ʾ��
		}

		// �ҵ�������������
		while ( iNodePos < iIndexNode )
		{
			if ( pAxisNode[iNodePos].m_iTimeId/(3600*24) >= IOCTime.m_TimeInit.m_Time.GetTime()/(3600*24) )
			{
				break;
			}
			++iNodePos;
		}
		ASSERT( iNodePos < iIndexNode );
		if ( iNodePos >= iIndexNode )
		{
			break;
		}
		int32 iDayPos = iNodePos;		// ��������ݵ����
		

		// �����������ָ������� - ����ֻҪ�п�������Ϣ���Ȼ�и���ĵ�һ���������
		const int32 iDaySecCount = (3600*24);
		if ( bMultiDay && pAxisNode[iDayPos].m_iTimeId/iDaySecCount == IOCTime.m_TimeInit.m_Time.GetTime()/iDaySecCount )
		{
			int32 iCurTime = pAxisNode[iDayPos].m_iTimeId;
			bool32 bIsDayStart = true;
			if ( bZoomed )
			{
				// �����£����ȫ��������㣬���õ��Ƿ��Ǹ���ĵ�һ����
				// ����ǣ������Ӹ�����շָ���(�����ͬ�����ݻ�������)
				bIsDayStart = false;
				int32 iPosInAll;
				if ( FindPosInAllTimeIds(iCurTime, iPosInAll) )
				{
					if ( iPosInAll > 0 )
					{
						// �ҵ������С������λ�ã�һ��ͬ������������ô10����
						while ( (--iPosInAll) >= 0 )
						{
							if ( m_aAllXTimeIds[iPosInAll] < iCurTime )
							{
								if ( m_aAllXTimeIds[iPosInAll]/iDaySecCount != iCurTime/iDaySecCount )
								{
									bIsDayStart = true;	// ����ͬһ�죬�Ǿ���Ϊ���������һ��ĵ�һ�����ݰɣ���������ͬ������
								}
								break;
							}
						}
					}
					else
					{
						bIsDayStart = false;	// ���ݵ�һ��û����ָ������
					}
				}
			}
			else if ( bMultiDay && m_aTickMultiDayIOCTimes.GetUpperBound() == i )
			{
				bIsDayStart = false;	// ���յ�һ��û����ָ������
			}

			if ( bIsDayStart )
			{
				// ��ָ������
				CTime Time(pAxisNode[iDayPos].m_iTimeId);
				CAxisDivide &AxisDivide = pAxisDivide[iIndexDivide];
				AxisDivide.m_iPosPixel		= pAxisNode[iDayPos].m_iCenterPixel;
				
				//
				AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;
				AxisDivide.m_eLineStyle = CAxisDivide::ELSSolid;
				
				//				
				AxisDivide.m_DivideText1.m_StrText.Format(L"%02d/%02d", Time.GetMonth(), Time.GetDay());
				AxisDivide.m_eLevelText		= CAxisDivide::EDLLevel1;
				if ( AxisDivide.m_iPosPixel == rect.left )
				{
					AxisDivide.m_DivideText1.m_uiTextAlign = DT_LEFT | DT_VCENTER | DT_SINGLELINE;
				}
				else
				{
					AxisDivide.m_DivideText1.m_uiTextAlign = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
				}
				AxisDivide.m_DivideText1.m_lTextColor = m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
				
				AxisDivide.m_DivideText2 = AxisDivide.m_DivideText1;
				
				// 
				iIndexDivide++;
			} // if (bIsDayStart)
		}//if ( pAxisNode[iDayPos].m_iTimeId/3600 == IOCTime.m_TimeInit.m_Time.GetTime()/3600 )
		else
		{
			ASSERT( pAxisNode[iDayPos].m_iTimeId/(3600*24) == IOCTime.m_TimeInit.m_Time.GetTime()/(3600*24) );
		}
		
		// ��������λ�ñ����е�
		// ����������������ڷֱ���������С��1/2�����������������ӵ㣬��������
		for ( int32 j=0; j <= IOCTime.m_aOCTimes.GetSize()-2 ; j+=2 )
		{
			// ���� Сʱ ��Сʱ
			CGmtTime TimeOpen	= IOCTime.m_aOCTimes[j];
			CGmtTime TimeClose	= IOCTime.m_aOCTimes[j+1];

			TimeOpen	= max(TimeOpen, TimeStart);
			SaveMinute(TimeOpen);
			TimeClose	= min(TimeClose, TimeEnd);
			SaveMinute(TimeClose);
			TimeClose += CGmtTimeSpan(0, 0, 0, 59);	// ��ȷ��ֻ�ܵ�������

			int32 iOCPos = iNodePos;	// �¿������ݵ����
			// ����������
			if ( j > 0 || (!bMultiDay&&!bZoomed) )
			{
				// �����µ�һ�����̱���Ϊ�����ķָ�����뿪�̷ָ�
				// ����״̬�£������Ƶ��յĵ�һ�����̷ָ��
				int32 tK=IOCTime.m_aOCTimes[j].GetTime();
				if ( tK >= TimeOpen.GetTime() )
				{
					// ֻ�ܲ����������ʾ�����ڵĿ��̵�
					bool32 bExist = false;
					int32 iPos = iOCPos;
					for ( ; iPos < iIndexNode ; ++iPos )
					{
						if ( pAxisNode[iPos].m_iTimeId/60 == tK/60 )
						{
							bExist = true;
							// ���̷ָ������
							CTime Time(pAxisNode[iPos].m_iTimeId);
							CAxisDivide &AxisDivide = pAxisDivide[iIndexDivide];
							AxisDivide.m_iPosPixel		= pAxisNode[iPos].m_iCenterPixel;
							
							//
							AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel2;
							if ( 0!=iIndexDivide && (!bMultiDay || bShowHourInMultiDay) )
							{
								// ���и��͵��ߵ��������ʾʵ��
								AxisDivide.m_eLineStyle = CAxisDivide::ELSSolid;
							}
							else
							{
								AxisDivide.m_eLineStyle = CAxisDivide::ELSDot;
							}
							
							// ���и��͵��ߵ��������ʾ����
							if ( !bMultiDay || bShowHourInMultiDay )
							{
								AxisDivide.m_DivideText1.m_StrText.Format(L"%02d:%02d", Time.GetHour(), Time.GetMinute());
							}
							AxisDivide.m_eLevelText		= CAxisDivide::EDLLevel2;
							if ( 0 == iIndexDivide )
							{
								AxisDivide.m_DivideText1.m_uiTextAlign = DT_LEFT | DT_VCENTER | DT_SINGLELINE;
							}
							else
							{
								AxisDivide.m_DivideText1.m_uiTextAlign = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
								
							}
							AxisDivide.m_DivideText1.m_lTextColor = m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
							
							AxisDivide.m_DivideText2 = AxisDivide.m_DivideText1;
							
							// 
							iIndexDivide++;
							break;
						}
						else if ( pAxisNode[iPos].m_iTimeId/60 > tK/60 )
						{
							ASSERT( 0 );
							break;
						}
					}
				}//if ( tK >= TimeOpen.GetTime() )
			}// if(j>0)

			if ( (0 == i && IOCTime.m_aOCTimes.GetSize()-1 == j+1)
				&& !bZoomed )
			{
				// ��������һ������, ��Ҫ��־���һ������ʱ��	
				int32 tK=IOCTime.m_aOCTimes[j+1].GetTime();//TimeClose.GetTime();
				bool32 bExist = false;
				int32 iPos = iOCPos;
				for ( ; iPos < iIndexNode ; ++iPos )
				{
					if ( pAxisNode[iPos].m_iTimeId/60 == tK/60 )
					{
						bExist = true;
						// ���̽���������
						CTime Time(pAxisNode[iPos].m_iTimeId);
						CAxisDivide &AxisDivide = pAxisDivide[iIndexDivide];
						AxisDivide.m_iPosPixel		= pAxisNode[iPos].m_iCenterPixel;
						
						//
						AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;
						AxisDivide.m_eLineStyle = CAxisDivide::ELSDot;
						
						//				
						AxisDivide.m_DivideText1.m_StrText.Format(L"%02d:%02d", Time.GetHour(), Time.GetMinute());
						AxisDivide.m_eLevelText		= CAxisDivide::EDLLevel1;
						AxisDivide.m_DivideText1.m_uiTextAlign = DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
						
						AxisDivide.m_DivideText1.m_lTextColor = m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
						
						AxisDivide.m_DivideText2 = AxisDivide.m_DivideText1;
						
						// 
						iIndexDivide++;
						break;
					}
					else if ( pAxisNode[iPos].m_iTimeId/60 > tK/60 )
					{
						ASSERT( 0 );
						break;
					}
				}
			}// if ( 0 == j )

			// С�ָ������
			if ( !bMultiDay )
			{
				// ��Сʱ����, ����Ҫ���뿪/����ʱ���Сʱ, ���ܳ��ֶϲ��
				for ( int32 tK=IOCTime.m_aOCTimes[j].GetTime()+1800; tK < IOCTime.m_aOCTimes[j+1].GetTime() ; tK+=1800 )
				{
					if ( tK < TimeOpen.GetTime() )
					{
						continue;	// ���ܲ�������ʾ�������
					}
					if ( tK > TimeClose.GetTime() )
					{
						break;	// ���ܲ�����ʾ���
					}

					int32 tHalfM = tK/60;//(tK/1800)*1800/60;
					bool32 bExist = false;
					int32 iPos = iOCPos;
					for ( ; iPos < iIndexNode ; ++iPos )
					{
						if ( pAxisNode[iPos].m_iTimeId/60 == tHalfM )
						{
							bExist = true;
							// ��Сʱ���ݵ�������
							CTime Time(pAxisNode[iPos].m_iTimeId);
							CAxisDivide &AxisDivide = pAxisDivide[iIndexDivide];
							AxisDivide.m_iPosPixel		= pAxisNode[iPos].m_iCenterPixel;
							
							//
							AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel3;
							AxisDivide.m_eLineStyle = CAxisDivide::ELSDot;
							
							//	
							if ( !bMultiDay )
							{
								AxisDivide.m_DivideText1.m_StrText.Format(L"%02d:%02d", Time.GetHour(), Time.GetMinute());
							}
							AxisDivide.m_eLevelText		= CAxisDivide::EDLLevel3;
							AxisDivide.m_DivideText1.m_uiTextAlign = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
							AxisDivide.m_DivideText1.m_lTextColor = m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
							
							AxisDivide.m_DivideText2 = AxisDivide.m_DivideText1;
							
							// 
							iIndexDivide++;
							iNodePos = iPos+1;
							break;
						}
						else if ( pAxisNode[iPos].m_iTimeId/60 > tHalfM )
						{
							ASSERT( 0 );
							break;	// ��Ҫ�����λ�ò���һ����Сʱ��
						}
					}//for ( ; iPos < iIndexNode ; ++iPos )

					ASSERT( pAxisNode[iPos-1].m_iTimeId/60 <= tHalfM );
				}//for ( int32 tK=TimeOpen.GetTime()+1800; tK <= TimeClose.GetTime()-1740 ; tK+=1800 )
			}//if ( !bMultiDay )
			else if ( bShowHourInMultiDay )
			{
				// Сʱ����, ����Ҫ���뿪/����ʱ���Сʱ
				for ( int32 tK=IOCTime.m_aOCTimes[j].GetTime()+3600; tK <= IOCTime.m_aOCTimes[j+1].GetTime() ; tK+=3600 )
				{
					if ( tK < TimeOpen.GetTime() )
					{
						continue;	// ���ܲ�������ʾ�������
					}
					if ( tK > TimeClose.GetTime() )
					{
						break;	// ���ܲ�����ʾ���
					}

					int32 tHalfM = tK/60;//(tK/3600)*3600/60;
					bool32 bExist = false;
					int32 iPos = iOCPos;
					for ( ; iPos < iIndexNode ; ++iPos )
					{
						if ( pAxisNode[iPos].m_iTimeId/60 == tHalfM )
						{
							bExist = true;
							// Сʱ�ָ��
							CTime Time(pAxisNode[iPos].m_iTimeId);
							CAxisDivide &AxisDivide = pAxisDivide[iIndexDivide];
							AxisDivide.m_iPosPixel		= pAxisNode[iPos].m_iCenterPixel;
							
							//
							AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel3;
							AxisDivide.m_eLineStyle = CAxisDivide::ELSDot;
							
							//				
							//AxisDivide.m_DivideText1.m_StrText.Format(L"%02d:%02d", Time.GetHour(), Time.GetMinute());
							AxisDivide.m_eLevelText		= CAxisDivide::EDLLevel3;
							AxisDivide.m_DivideText1.m_uiTextAlign = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
							AxisDivide.m_DivideText1.m_lTextColor = m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
							
							AxisDivide.m_DivideText2 = AxisDivide.m_DivideText1;
							
							// 
							iIndexDivide++;
							iNodePos = iPos+1;
							break;
						}
						else if ( pAxisNode[iPos].m_iTimeId/60 > tHalfM )
						{
							ASSERT( 0 );
							break;	
						}
					}//for ( ; iPos < iIndexNode ; ++iPos )
				}//else if ( bShowHourInMultiDay )
			}
		}//for ( int32 j=0; j < IOCTime.m_aOCTimes.GetSize()-2 ; j+=2 )
		
	}//for ( int32 i=m_aTickMultiDayIOCTimes.GetUpperBound(); i >=0 ; --i )
	
	// 
	aAxisNodes.SetSize(iIndexNode);
	aAxisDivide.SetSize(iIndexDivide);

	return true;
}

void CIoViewTick::CalcMainRegionYAxis(CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide)
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

			float fHeightSub = (float)(rectMainRegion.Height() / (iNum *1.0));
			
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
			
			return;
		}
		else
		{
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
			fPricePrevClose = GetTickPrevClose();
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
}

void CIoViewTick::CalcSubRegionYAxis(CChartRegion* pChartRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide)
{									  
	ASSERT(NULL != pChartRegion);
	aYAxisDivide.SetSize(0);
	
	// ����Y����������ɶ��ٸ��̶�
	CRect rect = pChartRegion->GetRectCurves();
	int32 iHeightText = pDC->GetTextExtent(_T("����ֵ")).cy;
	int32 iHeightSub  = iHeightText + 12;
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
	if (NULL == pCurveDependent)
		return;
	
	float fYMin = 0., fYMax = 0.;
	if (!pCurveDependent->GetYMinMax(fYMin, fYMax))
		return;
	
	// ���㸡�����̶ȳ���.
	float fHeightSub = (fYMax - fYMin) / iNum;
	int32 y;
	float fStart = fYMin;
	int32 iCount = 0;
	aYAxisDivide.SetSize(iNum + 1);
	CAxisDivide *pAxisDivide = (CAxisDivide *)aYAxisDivide.GetData();
	
	// �Ƿ�������������
	bool32 bTran = false;
	
// 	if ( 0 == iSaveDec && 3 == iNum)
// 	{
// 		// ������,�ҷ�Χ����,�����´���:
// 		int32 iMax = int32(fYMax);
// 		int32 iMin = int32(fYMin);
// 		
// 		//
// 		int32 iTemp = (iMax - iMin) / 3;
// 		int32 iStep = GetInter( iTemp );
// 		
// 		if ( -1 == iStep || 0 == iStep || iMax < 0 || iMin < 0)
// 		{
// 			bTran = false;
// 		}
// 		
// 		// 
// 		if( bTran )
// 		{
// 			int32 iBegin = MakeMinBigger(iMin, iStep);
// 			
// 			if ( -1 == iBegin || iBegin < iMin)
// 			{
// 				iBegin = GetInter(iMin);
// 				
// 				while ( iBegin < iMin )
// 				{
// 					// ��֤��Сֵ�Ϸ�
// 					iBegin += iStep;
// 				}
// 			}
// 			
// 			int32 aiYAxis[4] = {0};
// 			
// 			for ( int32 i = 0 ; i <= iNum; i++)
// 			{
// 				aiYAxis[i] = iBegin + i*iStep;
// 			}
// 			
// 			if ( aiYAxis[3] > iMax )
// 			{
// 				// ��֤���ֵ�Ϸ�
// 				aiYAxis[3] = iMax;
// 			}
// 			
// 			// ��ֵ:
// 			for ( i = 0 ; i <= iNum; i++)
// 			{
// 				if (!pCurveDependent->ValueRegionY(float(aiYAxis[i]), y))
// 				{
// 					//ASSERT(0);
// 					break;
// 				}
// 				
// 				pChartRegion->RegionYToClient(y);
// 				
// 				CString StrValue	= Float2String(float(aiYAxis[i]), iSaveDec, true, false, false);
// 				
// 				// 
// 				CAxisDivide &AxisDivide = pAxisDivide[iCount];
// 				AxisDivide.m_iPosPixel	= y;
// 				
// 				// ����
// 				AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;		// 
// 				AxisDivide.m_eLineStyle = CAxisDivide::ELSDot;			// ʵ��
// 				
// 				// ����
// 				AxisDivide.m_eLevelText						= CAxisDivide::EDLLevel1;
// 				
// 				AxisDivide.m_DivideText1.m_StrText			= StrValue;
// 				AxisDivide.m_DivideText2.m_StrText			= StrValue;
// 				
// 				AxisDivide.m_DivideText1.m_uiTextAlign		= DT_VCENTER | DT_RIGHT | DT_SINGLELINE;
// 				AxisDivide.m_DivideText2.m_uiTextAlign		= DT_VCENTER | DT_LEFT | DT_SINGLELINE;
// 				
// 				AxisDivide.m_DivideText1.m_lTextColor	= GetIoViewColor(ESCRise);
// 				AxisDivide.m_DivideText2.m_lTextColor	= GetIoViewColor(ESCRise);
// 				
// 				// 
// 				iCount++;
// 			}
// 		}			
// 	}
// 	else
// 	{
// 		bTran = false;
// 	}

	if ( !bTran )
	{
		for (int32 i = 0; i <= iNum; i++, fStart += fHeightSub)
		{
			if (i == iNum)
				fStart = fYMax;
			
			// 
			if (!pCurveDependent->PriceYToRegionY(fStart, y))
			{
				//
				//CNodeSequence* pNodes = pCurveDependent->GetNodes();
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
			
			AxisDivide.m_DivideText1.m_uiTextAlign		= DT_VCENTER | DT_RIGHT | DT_SINGLELINE;
			AxisDivide.m_DivideText2.m_uiTextAlign		= DT_VCENTER | DT_LEFT | DT_SINGLELINE;
			
			// zhangbo 20090927 #��ʱд����ɫ
			AxisDivide.m_DivideText1.m_lTextColor	= GetIoViewColor(ESCVolume);//GetIoViewColor(ESCChartAxisLine);
			AxisDivide.m_DivideText2.m_lTextColor	= GetIoViewColor(ESCVolume);//GetIoViewColor(ESCChartAxisLine);
			
			// 
			iCount++;
		}
	}

	//  
	aYAxisDivide.SetSize(iCount);
}

void CIoViewTick::OnKeyDown()
{
	if (OnZoomOut())
	{
		m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
		for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
		{
			T_SubRegionParam &SubRegion = m_SubRegions[i];
			SubRegion.m_pSubRegionMain->SetDrawFlag(CRegion::KDrawFull);
		}
	}
}

void CIoViewTick::OnKeyUp()
{
	if (OnZoomIn())
	{
		m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
		for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
		{
			T_SubRegionParam &SubRegion = m_SubRegions[i];
			SubRegion.m_pSubRegionMain->SetDrawFlag(CRegion::KDrawFull);
		}
	}
}

void CIoViewTick::OnKeyLeftAndCtrl()
{
	// ȡ��ͼ��Ϣ
	ASSERT( 0 )	;
	OnKeyLeftAndCtrl(1);
}

void CIoViewTick::OnKeyLeftAndCtrl( int32 iRepCnt )
{
	if ( iRepCnt < 1 )
	{
		iRepCnt = 1;
	}
	
	if ( !IsTickZoomed() )
	{
		return;	// ������״̬�²���ƽ��
	}
	
	// ȡ��ͼ��Ϣ
	T_MerchNodeUserData *pMainData = m_MerchParamArray.GetSize()>0?m_MerchParamArray[0]:NULL;
	if ( NULL == pMainData || NULL == m_pRegionMain )
	{
		return;
	}

	if ( m_pRegionMain->m_aXAxisNodes.GetSize() <= 0 )
	{
		return;
	}

	// �������ƶ��������޹�
	// �ҵ���ǰ�������alltime�е�λ�ã������ƶ�1��
	int32 iStartInAll;	// ��ʼλ��
	int32 iNodeCount = m_pRegionMain->m_aXAxisNodes.GetSize();
	ASSERT( iNodeCount < m_aAllXTimeIds.GetSize() );	// ����״̬�±�С��
	if ( !FindPosInAllTimeIds(m_TimeZoomStart.GetTime(), iStartInAll) )
	{
		ASSERT( 0 );	// �޷���λ��ʼλ��
		return;
	}
	if ( m_aAllXTimeIds[iStartInAll+iNodeCount-1] != m_TimeZoomEnd.GetTime() )
	{
		// ��ʼ����ͬ�����ݣ�����β�����ݽ��ж�λ�����β������Ҳ��λʧ�ܣ����Կ�ʼ��Ϊ׼
		int32 iEndInAll;
		if ( !FindPosInAllTimeIds(m_TimeZoomEnd.GetTime(), iEndInAll) )
		{
			// �޷���λλ��
			return;
		}
		if ( m_aAllXTimeIds[iEndInAll-iNodeCount+1] == m_TimeZoomStart.GetTime() )
		{
			// ͷβƥ�䣬�ȽϽӽ���ʵ�˰�
			iStartInAll = iEndInAll-iNodeCount+1;
		}
	}
	
	// 
	if ( iStartInAll <= 0 )	// ��������ˣ� ����Ҫ�������ݣ� �����û������¼�����
	{
		ASSERT( iStartInAll==0 );
		return;
	}
	else
	{	
		// ����showPosָʾ����alltime�����ҵ�ǰ���ʱ��㣬�������ɸ�ʱ����ƶ�
		// �����ƶ�һ����
		int32 iMove = iRepCnt * 5;	// �ƶ���ʱ�����
		int32 iShowPosInFullList = iStartInAll - iMove;	// �Ҳ��ƶ�
		int32 iStartTime = m_TimeZoomStart.GetTime();
		// �������ұߵ�����
		if ( iShowPosInFullList < 0 )
		{
			iShowPosInFullList	=	0;
		}
		else
		{
			// ��֤Ҫ�����ƶ�һ�룬���ʵ��û�У��ǾͶ���n�����ݰɣ�Ӧ��n���ᳬ��10����
			while ( iShowPosInFullList > 0 )
			{
				if ( m_aAllXTimeIds[iShowPosInFullList] < iStartTime )
				{
					break;
				}
				--iShowPosInFullList;
			}
		}
		
		ASSERT( iShowPosInFullList >=0 && m_aAllXTimeIds.GetSize() >= iShowPosInFullList+iNodeCount );
		
		// ͬ�����ݻ��������
		m_TimeZoomStart = CGmtTime(m_aAllXTimeIds[iShowPosInFullList]);
		m_TimeZoomEnd	= CGmtTime(m_aAllXTimeIds[iShowPosInFullList+iNodeCount-1]);
		
		OnVDataMerchTimeSalesUpdate(m_pMerchXml);
		
		// zhangbo 20090720 #�����䣬 ������Ʒ���ݸ���
		//...
		
		// ���¼�������
		UpdateAxisSize();
		
		// ��ʾˢ��
		m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
		for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
		{
			m_SubRegions[i].m_pSubRegionMain->SetDrawFlag(CRegion::KDrawFull);
		}
		
		RedrawWindow();
	}
}


void CIoViewTick::OnKeyHome()
{
// 	if ( NULL == m_pRegionMain->GetDependentCurve() )
// 	{
// 		return;
// 	}
// 
// 	// Home ��.������һ��K ��
// 	int32 iNodeCross = m_pRegionMain->m_iNodeCross;
// 	
// 	if ( !m_pRegionMain->IsActiveCross() )
// 	{
// 		m_pRegionMain->ActiveCross(true);
// 		
// 		for(int32 i = 0 ; i < m_SubRegions.GetSize(); i++)
// 		{
// 			m_SubRegions.GetAt(i).m_pSubRegionMain->ActiveCross(true);
// 		}
// 	}		
// 	
// 	m_pRegionMain->m_iNodeCross = 0;						
// 	iNodeCross = m_pRegionMain->m_iNodeCross;
// 	
// 	int32 x = 0;
// 	m_pRegionMain->GetDependentCurve()->CurvePosToRegionX(m_pRegionMain->m_iNodeCross,x);
// 	m_pRegionMain->RegionXToClient(x);
// 	m_pRegionMain->BeginOverlay(false);			
// 	m_pRegionMain->DrawCross(m_pRegionMain->GetViewData()->GetOverlayDC(), m_pRegionMain->m_iNodeCross,CChartRegion::EDCLeftRightKey);
// 	m_pRegionMain->EndOverlay();
// 	
// 	OnKeyLeftRightAdjustIndex();

	// ������ʾ��
	OnKeyUp();
	
	return;		
}

void CIoViewTick::OnKeyEnd()
{
// 	if ( NULL == m_pRegionMain->GetDependentCurve() )
// 	{
// 		return;
// 	}
// 
// 	int32 iNodeCross = m_pRegionMain->m_iNodeCross;
// 	
// 	if ( !m_pRegionMain->IsActiveCross() )
// 	{
// 		m_pRegionMain->ActiveCross(true);
// 		
// 		for(int32 i = 0 ; i < m_SubRegions.GetSize(); i++)
// 		{
// 			m_SubRegions.GetAt(i).m_pSubRegionMain->ActiveCross(true);
// 		}
// 	}	
// 	
// 	m_pRegionMain->m_iNodeCross = m_pRegionMain->GetDependentCurve()->GetNodes()->GetNodes().GetSize() - 1;						
// 	iNodeCross = m_pRegionMain->m_iNodeCross;
// 	
// 	int32 x = 0;
// 	m_pRegionMain->GetDependentCurve()->CurvePosToRegionX(m_pRegionMain->m_iNodeCross,x);
// 	m_pRegionMain->RegionXToClient(x);
// 	m_pRegionMain->BeginOverlay(false);			
// 	m_pRegionMain->DrawCross(m_pRegionMain->GetViewData()->GetOverlayDC(), m_pRegionMain->m_iNodeCross,CChartRegion::EDCLeftRightKey);
// 	m_pRegionMain->EndOverlay();
// 	
// 	OnKeyLeftRightAdjustIndex();

	// ������ʾ��
	OnKeyDown();

	return;	
}

void CIoViewTick::OnKeyLeft()
{
	if (NULL == m_pRegionMain->GetDependentCurve())
	{
		return;
	}

	int32 iMove = 1;
	if ( IsCtrlPressed() )
	{
		iMove = KDefaultTickCtrlMoveSpeed;
	}

// 	if ( IsCtrlPressed())
// 	{
// 		OnKeyLeftAndCtrl();
// 	}
//	else
	{
		int32 iNodeCross = m_pRegionMain->m_iNodeCross;

		if ( !m_pRegionMain->IsActiveCross() )
		{
			m_pRegionMain->ActiveCross(true);
			for ( int32 i = 0 ; i < m_SubRegions.GetSize(); i++)
			{
				T_SubRegionParam SubRegion = m_SubRegions.GetAt(i);
				SubRegion.m_pSubRegionMain->ActiveCross(true);
			}
			
			m_pRegionMain->m_iNodeCross = m_pRegionMain->GetDependentCurve()->GetNodes()->GetSize()-1;						
			iNodeCross = m_pRegionMain->m_iNodeCross;
		}
		else
		{
			//iNodeCross --;
			iNodeCross -= iMove;
		}

		// �ж�һ��iNodeCross �Ƿ��ڷǷ�����: ���ұ߿հ�û���ݵĵط�
		int32 iNodeSize = m_pRegionMain->GetDependentCurve()->GetNodes()->GetNodes().GetSize();
		if( iNodeCross >= (iNodeSize - 1) )
		{
			iNodeCross = iNodeSize - 1;
		}	
		//

		if ( iNodeCross < 0 )
		{
			m_pRegionMain->m_iNodeCross = 0;
			iNodeCross = 0;
			
			// ������������:
			{
				OnKeyLeftAndCtrl(iMove);
				
				if ( ! m_pRegionMain->FlyCross ( iNodeCross, true))
				{
					return;
				}
				
				int32 x = 0;
				m_pRegionMain->GetDependentCurve()->CurvePosToRegionX(m_pRegionMain->m_iNodeCross,x);
				m_pRegionMain->RegionXToClient(x);
				m_pRegionMain->BeginOverlay(false);			
				m_pRegionMain->DrawCross(m_pRegionMain->GetViewData()->GetOverlayDC(), m_pRegionMain->m_iNodeCross,CChartRegion::EDCLeftRightKey);
				m_pRegionMain->EndOverlay();
			}
		}
		else
		{
			if ( ! m_pRegionMain->FlyCross ( iNodeCross, true))
			{
				return;
			}
			
			int32 x = 0;
			m_pRegionMain->GetDependentCurve()->CurvePosToRegionX(m_pRegionMain->m_iNodeCross,x);
			m_pRegionMain->RegionXToClient(x);
			m_pRegionMain->BeginOverlay(false);			
			m_pRegionMain->DrawCross(m_pRegionMain->GetViewData()->GetOverlayDC(),m_pRegionMain->m_iNodeCross,CChartRegion::EDCLeftRightKey);
			m_pRegionMain->EndOverlay();
		}

		OnKeyLeftRightAdjustIndex();
		return;		
	}
}

void CIoViewTick::OnKeyRightAndCtrl()
{
	// ȡ��ͼ��Ϣ
	ASSERT( 0 );
	OnKeyRightAndCtrl(1);
}

void CIoViewTick::OnKeyRightAndCtrl( int32 iRepCnt )
{
	if ( iRepCnt < 1 )
	{
		iRepCnt = 1;
	}
	
	if ( !IsTickZoomed() )
	{
		return;	// ������״̬�²���ƽ��
	}
	
	// ȡ��ͼ��Ϣ
	T_MerchNodeUserData *pMainData = m_MerchParamArray.GetSize()>0?m_MerchParamArray[0]:NULL;
	if ( NULL == pMainData || NULL == m_pRegionMain )
	{
		return;
	}

	if ( m_pRegionMain->m_aXAxisNodes.GetSize() <= 0 )
	{
		return;
	}

	// �������ƶ��������޹�
	// �ҵ���ǰ�������alltime�е�λ�ã������ƶ�1��
	int32 iStartInAll;	// ��ʼλ��
	int32 iNodeCount = m_pRegionMain->m_aXAxisNodes.GetSize();
	ASSERT( iNodeCount < m_aAllXTimeIds.GetSize() );	// ����״̬�±�С��
	if ( !FindPosInAllTimeIds(m_TimeZoomStart.GetTime(), iStartInAll) )
	{
		ASSERT( 0 );	// �޷���λ��ʼλ��
		return;
	}
	if ( m_aAllXTimeIds[iStartInAll+iNodeCount-1] != m_TimeZoomEnd.GetTime() )
	{
		// ��ʼ����ͬ�����ݣ�����β�����ݽ��ж�λ�����β������Ҳ��λʧ�ܣ����Կ�ʼ��Ϊ׼
		int32 iEndInAll;
		if ( !FindPosInAllTimeIds(m_TimeZoomEnd.GetTime(), iEndInAll) )
		{
			// �޷���λλ��
			return;
		}
		if ( m_aAllXTimeIds[iEndInAll-iNodeCount+1] == m_TimeZoomStart.GetTime() )
		{
			// ͷβƥ�䣬�ȽϽӽ���ʵ�˰�
			iStartInAll = iEndInAll-iNodeCount+1;
		}
	}
	
	// 
	if ( iStartInAll+iNodeCount >= m_aAllXTimeIds.GetSize() )	// �����ұ��ˣ� ����Ҫ�������ݣ� �����û������¼�����
	{
		ASSERT( iStartInAll+iNodeCount == m_aAllXTimeIds.GetSize() );
		return;
	}
	else
	{	
		// ����showPosָʾ����alltime�����ҵ�ǰ���ʱ��㣬�������ɸ�ʱ����ƶ�
		// �����ƶ�һ����
		int32 iMove = iRepCnt * 5;	// �ƶ���ʱ�����
		int32 iShowPosInFullList = iStartInAll + iMove;	// �Ҳ��ƶ�
		int32 iStartTime = m_TimeZoomStart.GetTime();
		// �������ұߵ�����
		if ( iShowPosInFullList+iNodeCount > m_aAllXTimeIds.GetSize() )
		{
			iShowPosInFullList	=	m_aAllXTimeIds.GetSize() - iNodeCount;
		}
		else
		{
			// ��֤Ҫ�����ƶ�һ�룬���ʵ��û�У��ǾͶ���n�����ݰɣ�Ӧ��n���ᳬ��10����
			int32 iEndPos = m_aAllXTimeIds.GetSize()-iNodeCount;
			while ( iShowPosInFullList < iEndPos )
			{
				if ( m_aAllXTimeIds[iShowPosInFullList] > iStartTime )
				{
					break;
				}
				++iShowPosInFullList;
			}
		}
		
		ASSERT( m_aAllXTimeIds.GetSize() >= iShowPosInFullList+iNodeCount );
		
		// ͬ�����ݻ��������
		m_TimeZoomStart = CGmtTime(m_aAllXTimeIds[iShowPosInFullList]);
		m_TimeZoomEnd	= CGmtTime(m_aAllXTimeIds[iShowPosInFullList+iNodeCount-1]);
		
		OnVDataMerchTimeSalesUpdate(m_pMerchXml);
		
		// zhangbo 20090720 #�����䣬 ������Ʒ���ݸ���
		//...
		
		// ���¼�������
		UpdateAxisSize();
		
		// ��ʾˢ��
		m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
		for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
		{
			m_SubRegions[i].m_pSubRegionMain->SetDrawFlag(CRegion::KDrawFull);
		}
		
		RedrawWindow();
	}
}

void CIoViewTick::OnKeyRight()
{
	if (NULL == m_pRegionMain->GetDependentCurve())
	{
		return;
	}

	int32 iMove = 1;
	if ( IsCtrlPressed() )
	{
		iMove = KDefaultTickCtrlMoveSpeed;
	}

// 	if ( IsCtrlPressed())
// 	{
// 		OnKeyRightAndCtrl();
// 	}
// 	else
	{
		int32 iNodeCross = m_pRegionMain->m_iNodeCross;
		
		if ( !m_pRegionMain->IsActiveCross() )
		{
			m_pRegionMain->ActiveCross(true);
			for(int32 i = 0 ; i < m_SubRegions.GetSize(); i++)
			{
				m_SubRegions.GetAt(i).m_pSubRegionMain->ActiveCross(true);
			}

			m_pRegionMain->m_iNodeCross = 0;			
			iNodeCross = m_pRegionMain->m_iNodeCross;
		}
		else
		{
			//iNodeCross ++;
			iNodeCross += iMove;
		}
		
		int32 iSizeNodes = m_pRegionMain->GetDependentCurve()->GetNodes()->GetSize();

		if (iNodeCross >=  iSizeNodes)
		{
			m_pRegionMain->m_iNodeCross = iSizeNodes-1 ;
			iNodeCross = iSizeNodes;
			
			// ������������:
			{
				OnKeyRightAndCtrl(iMove);
				
				if ( ! m_pRegionMain->FlyCross ( iNodeCross, false))
				{
					return;
				}
				
				int32 x = 0;
				m_pRegionMain->GetDependentCurve()->CurvePosToRegionX(m_pRegionMain->m_iNodeCross,x);
				m_pRegionMain->RegionXToClient(x);
				m_pRegionMain->BeginOverlay(false);			
				m_pRegionMain->DrawCross(m_pRegionMain->GetViewData()->GetOverlayDC(), m_pRegionMain->m_iNodeCross,CChartRegion::EDCLeftRightKey);
				m_pRegionMain->EndOverlay();
			}
		}
		else
		{
			if ( ! m_pRegionMain->FlyCross ( iNodeCross, false))
			{
				return;
			}
			
			int32 x = 0;
			m_pRegionMain->GetDependentCurve()->CurvePosToRegionX(m_pRegionMain->m_iNodeCross,x);
			m_pRegionMain->RegionXToClient(x);
			m_pRegionMain->BeginOverlay(false);			
			m_pRegionMain->DrawCross(m_pRegionMain->GetViewData()->GetOverlayDC(), m_pRegionMain->m_iNodeCross,CChartRegion::EDCLeftRightKey);
			m_pRegionMain->EndOverlay();
		}
		
		OnKeyLeftRightAdjustIndex();
		return;		
	}
}

// ��С��ʾ��λ�� ��ʾ��һЩK��
bool32 CIoViewTick::OnZoomOut()
{
	if (m_MerchParamArray.GetSize() <= 0)
		return false;
	
	T_MerchNodeUserData *pMainData = m_MerchParamArray[0];
	if (NULL == pMainData)
		return false;
	
	if ( NULL == m_pRegionMain || NULL == m_pRegionMain->GetDependentCurve() )
	{
		return false;
	}

	if ( !IsTickZoomed() )
	{
		return false;	// ������״̬�²�����С��
	}
	
	// �����ʮ�ֹ�꣬���Ե�ǰʮ�ֹ��Ϊ�������ߵȱ�������
	// ���û��ʮ�ֹ�꣬���Ե�ǰ��ʾ�����һ��(�����Ƿ���Ч)���ݵ���Ϊ���ұߵ��ߣ�����߽��з���
	int32 iDepPos = m_pRegionMain->m_aXAxisNodes.GetUpperBound();
	const int32 iNodeSize = m_pRegionMain->m_aXAxisNodes.GetSize();
	if ( iNodeSize >= m_aAllXTimeIds.GetSize() )
	{
		ASSERT( iNodeSize == m_aAllXTimeIds.GetSize() );
		//return false;	// �޷������Ŵ���
	}
	int32 iNewSize = iNodeSize*5/4;
	if ( iNewSize > m_aAllXTimeIds.GetSize() )
	{
		iNewSize = m_aAllXTimeIds.GetSize();
	}
	if ( m_pRegionMain->IsActiveCross() )
	{
		iDepPos = m_pRegionMain->m_iNodeCross;
		if ( iDepPos < 0 || iDepPos >= iNodeSize )
		{
			return false;
		}

		// ��Ҫ��֤FullKline��AllTimeIdʼ����һ�µ�, ������֤��
		int32 iCrossPos = m_pRegionMain->m_iNodeCross;
		int32 iTimeId = m_pRegionMain->m_aXAxisNodes[iCrossPos].m_iTimeId;
		int32 iPosInAll;
		if ( !FindPosInAllTimeIds(iTimeId, iPosInAll) )
		{
			return false;
		}
		ASSERT( m_aAllXTimeIds[iPosInAll] == iTimeId );
		
		// ���߰��ձ������Ӳ������ݵ���ʾ
		float fRightRatio = (iNodeSize-iCrossPos)/(float)iNodeSize;
		int32 iRightAdd = (int32)((iNewSize-iNodeSize)*fRightRatio);
		int32 iNewEndPos = iPosInAll+(iNodeSize-1-iCrossPos) + iRightAdd;
		if ( iNewEndPos > m_aAllXTimeIds.GetUpperBound() )
		{
			iRightAdd -= iNewEndPos-m_aAllXTimeIds.GetUpperBound();
			iNewEndPos = m_aAllXTimeIds.GetUpperBound();
		}
		int32 iNewStartPos = iNewEndPos-iNewSize+1;
		if ( iNewStartPos < 0 )
		{
			iRightAdd -= iNewStartPos;
			iNewStartPos = 0;
			iNewEndPos = iNewSize-1;
		}
		ASSERT( 0<=iNewStartPos && iNewStartPos <= iPosInAll && iPosInAll <= iNewEndPos );
		
		if ( iNewSize >= m_aAllXTimeIds.GetSize() )
		{
			ASSERT( iNewSize == m_aAllXTimeIds.GetSize() );
			CancelZoom(); // �൱��ȡ��������
		}
		else
		{
			m_TimeZoomStart = m_aAllXTimeIds[iNewStartPos];
			m_TimeZoomEnd   = m_aAllXTimeIds[iNewEndPos];
		}
		
		int32 iNewCrossPos = iCrossPos + (iNewSize-iNodeSize-iRightAdd) ;
		ASSERT( iNewCrossPos >=0 && iNewCrossPos <iNewSize );
		
		// ���ݱ������¼���&��ʾ
		// ����update��redraw��AllTimeId��xnodes���ܶ��Ѿ��ı䣬���ܽ������ϴμ����λ����ȷ��ʵ�ʵĵ�������
		LockCrossInfo(true);
		AddFlag(m_pRegionMain->m_iFlag,CChartRegion::KLockMouseMove);
		OnVDataMerchTimeSalesUpdate(m_pMerchXml);
		m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
		for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
		{
			m_SubRegions[i].m_pSubRegionMain->SetDrawFlag(CRegion::KDrawFull);
		}
		// 
		RedrawWindow();
		RemoveFlag(m_pRegionMain->m_iFlag,CChartRegion::KLockMouseMove);
		LockCrossInfo(false);

		// ��ʱ�����ϴμ��������iNewCrossPos���ܲ�����������Ч��ֵ�ˣ���Ҫ������
		// ���澡����Ҫʹ��ǰ���������Ľ��
		if ( iNewCrossPos < 0 
			|| iNewCrossPos >= m_pRegionMain->m_aXAxisNodes.GetSize()
			|| m_pRegionMain->m_aXAxisNodes[iNewCrossPos].m_iTimeId != iTimeId )
		{
			int32 iLow	= 0;
			int32 iHigh	= m_pRegionMain->m_aXAxisNodes.GetSize()-1;
			int32 iHalf = 0;
			while ( iLow <= iHigh )
			{
				iHalf = (iLow+iHigh)/2;
				int32 iT = m_pRegionMain->m_aXAxisNodes[iHalf].m_iTimeId;
				if ( iT == iTimeId )
				{
					break;
				}
				else if ( iT > iTimeId )
				{
					iHigh = iHalf-1;
				}
				else
				{
					iLow = iHalf+1;
				}
			}
			iNewCrossPos = iHalf;	// ��ʱ�����һ�����ڻ��߽ӽ�ָ��ʱ���ֵ
			if ( iNewCrossPos <0
				|| iNewCrossPos >= m_pRegionMain->m_aXAxisNodes.GetSize() )
			{
				ASSERT( 0 );	// �޷������Чֵ
				return false;
			}
			CGmtTime cgTmpTime(m_pRegionMain->m_aXAxisNodes[iNewCrossPos].m_iTimeId);
			TRACE(_T("����tick cross������: %s\n")
				, Time2String(cgTmpTime, ENTIMinute).GetBuffer());
		}

#ifdef _DEBUG
		{
			int32 iNewPosInAll;
			if ( !FindPosInAllTimeIds(iTimeId, iNewPosInAll) )
			{
				ASSERT( 0 );
				return false;
			}
			ASSERT( iNewPosInAll == iPosInAll && m_aAllXTimeIds[iNewPosInAll] == iTimeId && m_pRegionMain->m_aXAxisNodes[iNewCrossPos].m_iTimeId == iTimeId );
			CGmtTime cgTime0(iTimeId);
			CGmtTime cgTime1(m_pRegionMain->m_aXAxisNodes[iNewCrossPos].m_iTimeId);
			CGmtTime cgTime2(m_aAllXTimeIds[iNewPosInAll]);

			TRACE(_T("��������: %s->%s   %s\n")
				, Time2String(cgTime0, ENTIMinute).GetBuffer()
				, Time2String(cgTime1, ENTIMinute).GetBuffer()
				, Time2String(cgTime2, ENTIMinute).GetBuffer());
		}
#endif

		// ����cross, Y������ TODO
		m_pRegionMain->m_iNodeCross = iNewCrossPos;
		m_pRegionMain->BeginOverlay(false);			
		m_pRegionMain->DrawCross(m_pRegionMain->GetViewData()->GetOverlayDC(), m_pRegionMain->m_iNodeCross,CChartRegion::EDCLeftRightKey);
		m_pRegionMain->EndOverlay();
	}
	else
	{
		if ( iDepPos < 0 || iDepPos >= iNodeSize )
		{
			return false;
		}
		int32 iTimeId = m_pRegionMain->m_aXAxisNodes[iDepPos].m_iTimeId;
		int32 iPosInAll;
		if ( !FindPosInAllTimeIds(iTimeId, iPosInAll) )
		{
			return false;
		}
		
		// ����������
		int32 iNewStartPos = iPosInAll - (iNewSize-1);
		if ( iNewStartPos < 0 )
		{
			// ��߲��������ұߵ�
			iPosInAll -= iNewStartPos;
			iNewStartPos -= iNewStartPos;
		}
		ASSERT( iPosInAll-iNewStartPos+1 == iNewSize );
		ASSERT( iNewStartPos <= iPosInAll && iPosInAll <= m_aAllXTimeIds.GetUpperBound() );
		if ( iNewSize >= m_aAllXTimeIds.GetSize() )
		{
			ASSERT( iNewSize == m_aAllXTimeIds.GetSize() );
			CancelZoom(); // �൱��ȡ��������
		}
		else
		{
			m_TimeZoomStart = m_aAllXTimeIds[iNewStartPos];
			m_TimeZoomEnd	= m_aAllXTimeIds[iPosInAll];
		}
		
		// ���ݱ������¼���&��ʾ
		OnVDataMerchTimeSalesUpdate(m_pMerchXml);
		m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
		for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
		{
			m_SubRegions[i].m_pSubRegionMain->SetDrawFlag(CRegion::KDrawFull);
		}
		//
		RedrawWindow();
	}
	
	return true;
}

// ��ʾ��λ�Ŵ� ��ʾ��һЩK��
bool32 CIoViewTick::OnZoomIn()
{
	if (m_MerchParamArray.GetSize() <= 0)
		return false;
	
	T_MerchNodeUserData *pMainData = m_MerchParamArray[0];
	if (NULL == pMainData)
		return false;
		
	if ( NULL == m_pRegionMain || NULL == m_pRegionMain->GetDependentCurve() )
	{
		return false;
	}

	//CChartCurve *pDepCurve = m_pRegionMain->GetDependentCurve();
	// �����ʮ�ֹ�꣬���Ե�ǰʮ�ֹ��Ϊ�������ߵȱ�������
	// ���û��ʮ�ֹ�꣬���Ե�ǰ��ʾ�����һ��(�����Ƿ���Ч)���ݵ���Ϊ���ұߵ��ߣ�����߽��з���
	int32 iDepPos = m_pRegionMain->m_aXAxisNodes.GetUpperBound();
	const int32 iNodeSize = m_pRegionMain->m_aXAxisNodes.GetSize();
	const int32 KIMinNodeSize = 20;
	if ( iNodeSize <= KIMinNodeSize )
	{
		return false;	// �޷�������С��
	}
	int32 iNewSize = iNodeSize*3/4;
	if ( iNewSize < KIMinNodeSize )
	{
		iNewSize = KIMinNodeSize;
	}
	if ( m_pRegionMain->IsActiveCross() )
	{
		iDepPos = m_pRegionMain->m_iNodeCross;
		if ( iDepPos < 0 || iDepPos >= iNodeSize )
		{
			return false;
		}
		int32 iCrossPos = m_pRegionMain->m_iNodeCross;
		int32 iTimeId = m_pRegionMain->m_aXAxisNodes[iCrossPos].m_iTimeId;
		int32 iPosInAll;
		if ( !FindPosInAllTimeIds(iTimeId, iPosInAll) )
		{
			return false;
		}

		// ���߰��ձ����޳��������ݵ���ʾ
		float fRightRatio = (iNodeSize-iCrossPos)/(float)iNodeSize;
		int32 iRightDel = (int32)((iNodeSize-iNewSize)*fRightRatio);
		int32 iNewEndPos = iNodeSize-1-iRightDel;
		iNewEndPos = max(iNewEndPos, iCrossPos);	// �ұ�
		int32 iNewStartPos = iNewEndPos-iNewSize+1;
		ASSERT( iNewStartPos <= iCrossPos && iCrossPos <= iNewEndPos );

		m_TimeZoomStart = CGmtTime(m_pRegionMain->m_aXAxisNodes[iNewStartPos].m_iTimeId);
		m_TimeZoomEnd   = CGmtTime(m_pRegionMain->m_aXAxisNodes[iNewEndPos].m_iTimeId);

		int32 iNewCrossPos = iCrossPos-iNewStartPos;
		ASSERT( iNewCrossPos >=0 && iNewCrossPos <iNewSize );
		
		// ���ݱ������¼���&��ʾ
		LockCrossInfo(true);
		AddFlag(m_pRegionMain->m_iFlag,CChartRegion::KLockMouseMove);
		OnVDataMerchTimeSalesUpdate(m_pMerchXml);
		m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
		for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
		{
			m_SubRegions[i].m_pSubRegionMain->SetDrawFlag(CRegion::KDrawFull);
		}
		//
		RedrawWindow();

		RemoveFlag(m_pRegionMain->m_iFlag,CChartRegion::KLockMouseMove);
		LockCrossInfo(false);

		// ��ʱ�����ϴμ��������iNewCrossPos���ܲ�����������Ч��ֵ�ˣ���Ҫ������
		// ���澡����Ҫʹ��ǰ���������Ľ��
		if ( iNewCrossPos < 0 
			|| iNewCrossPos >= m_pRegionMain->m_aXAxisNodes.GetSize()
			|| m_pRegionMain->m_aXAxisNodes[iNewCrossPos].m_iTimeId != iTimeId )
		{
			int32 iLow	= 0;
			int32 iHigh	= m_pRegionMain->m_aXAxisNodes.GetSize()-1;
			int32 iHalf = 0;
			while ( iLow <= iHigh )
			{
				iHalf = (iLow+iHigh)/2;
				int32 iT = m_pRegionMain->m_aXAxisNodes[iHalf].m_iTimeId;
				if ( iT == iTimeId )
				{
					break;
				}
				else if ( iT > iTimeId )
				{
					iHigh = iHalf-1;
				}
				else
				{
					iLow = iHalf+1;
				}
			}
			iNewCrossPos = iHalf;	// ��ʱ�����һ�����ڻ��߽ӽ�ָ��ʱ���ֵ
			if ( iNewCrossPos <0
				|| iNewCrossPos >= m_pRegionMain->m_aXAxisNodes.GetSize() )
			{
				ASSERT( 0 );	// �޷������Чֵ
				return false;
			}
		}

#ifdef _DEBUG
		{
			int32 iNewPosInAll;
			if ( !FindPosInAllTimeIds(iTimeId, iNewPosInAll) )
			{
				ASSERT( 0 );
				return false;
			}
			ASSERT( iNewPosInAll == iPosInAll && m_aAllXTimeIds[iNewPosInAll] == iTimeId && m_pRegionMain->m_aXAxisNodes[iNewCrossPos].m_iTimeId == iTimeId );
			CGmtTime cgTime1(iTimeId);
			CGmtTime cgTime2(m_pRegionMain->m_aXAxisNodes[iNewCrossPos].m_iTimeId);
			TRACE(_T("���ݼ���: %s->%s\n"), Time2String(cgTime1, ENTIMinute).GetBuffer(), Time2String(cgTime2, ENTIMinute).GetBuffer());
		}
#endif

		// ����cross, Y������ TODO
		m_pRegionMain->m_iNodeCross = iNewCrossPos;
		m_pRegionMain->BeginOverlay(false);			
		m_pRegionMain->DrawCross(m_pRegionMain->GetViewData()->GetOverlayDC(), m_pRegionMain->m_iNodeCross,CChartRegion::EDCLeftRightKey);
		m_pRegionMain->EndOverlay();
	}
	else
	{
		if ( iDepPos < 0 || iDepPos >= iNodeSize )
		{
			return false;
		}

		// ����޳�������Ҫ�޳�������
		int32 iNewStartPos = iDepPos -(iNewSize-1);
		ASSERT( iDepPos-iNewStartPos+1 == iNewSize );
		m_TimeZoomStart = CGmtTime(m_pRegionMain->m_aXAxisNodes[iNewStartPos].m_iTimeId);
		m_TimeZoomEnd   = CGmtTime(m_pRegionMain->m_aXAxisNodes[iDepPos].m_iTimeId);
		
		// ���ݱ������¼���&��ʾ
		OnVDataMerchTimeSalesUpdate(m_pMerchXml);
		m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
		for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
		{
			m_SubRegions[i].m_pSubRegionMain->SetDrawFlag(CRegion::KDrawFull);
		}
		//
		RedrawWindow();
	}
	
	return true;
}

void CIoViewTick::RequestSequenceTickStart()
{
	if ( NULL == m_pMerchXml || m_aTickMultiDayIOCTimes.GetSize() <= 0 )
	{
		return;
	}

	// ��ʼ��ʱ�� -> [o, c]...[o,c] ->����ʱ��
	// ���շֶ�ȡ����, ��һ��Ϊ ��ʼ��->��һ�����յ��� ���һ��Ϊ���һ�����յĿ�������ʱ��
	// �����һ�η�����ֱ����һ��
	m_MmiReqTimeSales.m_iMarketId		= m_pMerchXml->m_MerchInfo.m_iMarketId;
	m_MmiReqTimeSales.m_StrMerchCode	= m_pMerchXml->m_MerchInfo.m_StrMerchCode;
	m_MmiReqTimeSales.m_eReqTimeType	= ERTYSpecifyTime;
	m_MmiReqTimeSales.m_TimeEnd			= m_aTickMultiDayIOCTimes[0].m_TimeEnd.m_Time;
	const CMarketIOCTimeInfo &IOCTime = m_aTickMultiDayIOCTimes[0];
	if ( IOCTime.m_aOCTimes.GetSize() > 1 )
	{
		int32 iSize = IOCTime.m_aOCTimes.GetSize();
		m_MmiReqTimeSales.m_TimeStart	= IOCTime.m_aOCTimes[iSize-2];
	}
	else
	{
		ASSERT( 0 );
		m_MmiReqTimeSales.m_TimeStart	= IOCTime.m_TimeInit.m_Time;
	}
	for (int32 i = 0; i < m_MerchParamArray.GetSize(); i++)
	{
		T_MerchNodeUserData *pData = m_MerchParamArray[i];
		if (NULL == pData)
			continue;
		
		CMerch *pMerch = pData->m_pMerchNode;
		if ( NULL == pMerch )
		{
			continue;
		}
		
		CMmiReqMerchTimeSales	reqTick;
		reqTick.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
		reqTick.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
		reqTick.m_eReqTimeType	= m_MmiReqTimeSales.m_eReqTimeType;
		reqTick.m_TimeEnd		= m_MmiReqTimeSales.m_TimeEnd;
		reqTick.m_TimeStart		= m_MmiReqTimeSales.m_TimeStart;
		DoRequestViewData(reqTick);

		TRACE(_T("����ReqStart: %s->%s\n")
			, Time2String(reqTick.m_TimeStart, ENTIMinute, true, false).GetBuffer()
				, Time2String(reqTick.m_TimeEnd, ENTIMinute, true, false).GetBuffer());

		if ( ERTYFrontCount != reqTick.m_eReqTimeType )
		{
			reqTick.m_eReqTimeType	= ERTYFrontCount;
			reqTick.m_TimeSpecify	= m_MmiReqTimeSales.m_TimeEnd;
			reqTick.m_iFrontCount	= 50;
			DoRequestViewData(reqTick);
		}

		// ����������ʱ��Ӧ�������ⷢ��һ��������������
	}
}

void CIoViewTick::RequestSequenceTickNext()
{
	if ( NULL == m_pMerchXml || m_aTickMultiDayIOCTimes.GetSize() <= 0 )
	{
		return;
	}
	if ( m_pMerchXml->m_MerchInfo.m_iMarketId != m_MmiReqTimeSales.m_iMarketId
		|| m_pMerchXml->m_MerchInfo.m_StrMerchCode != m_MmiReqTimeSales.m_StrMerchCode )
	{
		return;	// ����ͬһ����Ʒ~~
	}

	// ���ڲ���֪����һ������ȥ��ʱ���������������û�л������߸�������0���ݻ���
	// ����ֻ�ܲ�����һ�����������������һ��������������󱻹��˻����forceupdate��

	// ��ʼ��ʱ�� -> [o, c]...[o,c] ->����ʱ��
	// ���շֶ�ȡ����, ��һ��Ϊ ��ʼ��->��һ�����յ��� ���һ��Ϊ���һ�����յĿ�������ʱ��
	// �����һ�η�����ֱ����һ��
	m_MmiReqTimeSales.m_TimeEnd			= m_MmiReqTimeSales.m_TimeStart;	// start��Ϊ���̻��߳�ʼ��ʱ��
	int32 i = 0;
	for ( i=0; i < m_aTickMultiDayIOCTimes.GetSize() ; ++i )
	{
		const CMarketIOCTimeInfo &IOCTime = m_aTickMultiDayIOCTimes[i];
		if ( m_MmiReqTimeSales.m_TimeEnd <= IOCTime.m_TimeOpen.m_Time )
		{
			continue;	// <=�����������ʱ�䣬���Ȼ��Ҫ����ǰ�������
		}

		m_MmiReqTimeSales.m_TimeStart = IOCTime.m_TimeInit.m_Time;	// ��ʼʱ��

		int32 iSize = IOCTime.m_aOCTimes.GetSize();
		for ( int32 j=iSize-2; j > 1 ; j-=2 )	// ��һ������ȡ��ʼ��ʱ��, ���Ƚ�
		{
			if ( IOCTime.m_aOCTimes[j] < m_MmiReqTimeSales.m_TimeEnd )
			{
				m_MmiReqTimeSales.m_TimeStart = IOCTime.m_aOCTimes[j];
				break;
			}
		}

		ASSERT(m_MmiReqTimeSales.m_TimeStart < m_MmiReqTimeSales.m_TimeEnd);
		break;
	}

	if ( m_MmiReqTimeSales.m_TimeEnd <= m_MmiReqTimeSales.m_TimeStart )
	{
		return;	// û������Ҫ������
	}
	
	for (i = 0; i < m_MerchParamArray.GetSize(); i++)
	{
		T_MerchNodeUserData *pData = m_MerchParamArray[i];
		if (NULL == pData)
			continue;
		
		CMerch *pMerch = pData->m_pMerchNode;
		if ( NULL == pMerch )
		{
			continue;
		}
		
		CMmiReqMerchTimeSales	reqTick;
		reqTick.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
		reqTick.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
		reqTick.m_eReqTimeType	= m_MmiReqTimeSales.m_eReqTimeType;
		reqTick.m_TimeEnd		= m_MmiReqTimeSales.m_TimeEnd + CGmtTimeSpan(0,0,1,0);
		reqTick.m_TimeStart		= m_MmiReqTimeSales.m_TimeStart;
		bool32 bReq = DoRequestViewData(reqTick);

		if ( bReq )
		{
			TRACE(_T("����ReqNext: %s->%s\n")
				, Time2String(reqTick.m_TimeStart, ENTIMinute, true, false).GetBuffer()
				, Time2String(reqTick.m_TimeEnd, ENTIMinute, true, false).GetBuffer());
		}
	}
}

void CIoViewTick::RequestViewData()
{
	if (m_MerchParamArray.GetSize() <= 0)
		return;
	
	for (int32 i = 0; i < m_MerchParamArray.GetSize(); i++)
	{
		T_MerchNodeUserData *pData = m_MerchParamArray[i];
		if (NULL == pData)
			continue;

		CMerch *pMerch = pData->m_pMerchNode;
		if ( NULL == pMerch )
		{
			continue;
		}

		// ��Ҫ����n+1����k�� && �ѳ�ʼ���õ�ʱ����ڵ�tick����
		// �ֽ������� TODO
		ASSERT(pData->m_TrendTradingDayInfo.m_bInit);
		{
			CGmtTime TimeToday		= pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time;
			SaveDay(TimeToday);
			CMmiReqMerchKLine reqKline;
			reqKline.m_eKLineTypeBase	= EKTBDay;
			reqKline.m_eReqTimeType		= ERTYFrontCount;
			reqKline.m_iFrontCount		= m_iTickMultiDay+1;
			reqKline.m_TimeSpecify		= TimeToday;
			reqKline.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
			reqKline.m_StrMerchCode		= pMerch->m_MerchInfo.m_StrMerchCode;

			DoRequestViewData(reqKline);	// ����n��k��, ���� TODO
		}
		
		// ����ʵʱ����
		CMmiReqRealtimePrice MmiReqRealtimePrice;
		MmiReqRealtimePrice.m_iMarketId		= pData->m_pMerchNode->m_MerchInfo.m_iMarketId;
		MmiReqRealtimePrice.m_StrMerchCode	= pData->m_pMerchNode->m_MerchInfo.m_StrMerchCode;		
		DoRequestViewData(MmiReqRealtimePrice);
	}

	// ��ǰ��Ҫ��������յ����ݣ���û�еĵط���ʼ�������������ȴ����ݻ�����������
	RequestSequenceTickStart();
}

bool32 CIoViewTick::GetChartXAxisSliderText1( OUT CString &StrSlider, CAxisNode &AxisNode )
{
	StrSlider.Empty();
	// 
	if (m_MerchParamArray.GetSize() <= 0)
	{
		// ���ܴ����½���K ����ͼ,û������.
		return true;
	}
	
	T_MerchNodeUserData* pMainData = m_MerchParamArray.GetAt(0);
	if (NULL == pMainData)
		return false;
	
	bool32 bSaveMinute = false;
	
	// E_NodeTimeInterval eTimeInterval = pMainData->m_eTimeIntervalFull;
	E_NodeTimeInterval eTimeInterval = ENTIMinute;
	
	if (ENTIMinute == eTimeInterval || ENTIMinute5 == eTimeInterval || ENTIMinute15 == eTimeInterval || ENTIMinute30 == eTimeInterval || ENTIMinute60 == eTimeInterval || ENTIMinuteUser == eTimeInterval)
		bSaveMinute = true;
	else if (ENTIDay == eTimeInterval || ENTIWeek == eTimeInterval || ENTIMonth == eTimeInterval || ENTIQuarter == eTimeInterval || ENTIYear == eTimeInterval || ENTIDayUser == eTimeInterval)
	{
		bSaveMinute = false;
	}
	else	// ��Ӧ�ó��ֵ�����
	{
		return false;
	}

	if ( NULL == m_pRegionMain )
	{
		// ��Ӧ����RegionMainΪ��ʱ�͵��õ��˵� �������������
		return false;
	}

	int32 iTimeId = AxisNode.m_iTimeId;

	if ( 0 == iTimeId )
	{
		// ����X�������
		int32	iRegionX = AxisNode.m_iCenterPixel;
		m_pRegionMain->ClientXToRegion(iRegionX);
		if( NULL == m_pRegionMain->GetDependentCurve()
			|| ! m_pRegionMain->GetDependentCurve()->RegionXToCurveID(iRegionX, iTimeId) )
		{
			// ��Ӧ����RegionMainΪ��ʱ�͵��õ��˵� ������������� - �п���X ID��ʧ��
			return false;
		}
	}
	
	CTime Time(iTimeId);		// ���ǵ���ʱ����ʾ
	
	if (bSaveMinute)
		StrSlider.Format(L"%02d/%02d %02d:%02d", Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute());
	else
		StrSlider.Format(L"%04d/%02d/%02d(%s)", Time.GetYear(), Time.GetMonth(), Time.GetDay(), GetWeekString(Time).GetBuffer());

	return true;
}

void CIoViewTick::SetTickMulitDay( int32 iMultiDay, bool32 bReqData/* = true*/ )
{
	if ( iMultiDay < 1 )
	{
		iMultiDay = 1;
	}
	if ( iMultiDay > 10 )
	{
		iMultiDay = 10;
	}

	if ( IsTickZoomed() )
	{
		CancelZoom();	// ����Ҫ���
		OnVDataMerchTimeSalesUpdate(m_pMerchXml);
		if ( NULL != m_pRegionMain )
		{
			m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
			Invalidate();
		}
	}

	if ( iMultiDay != m_iTickMultiDay )
	{
		m_iTickMultiDay = iMultiDay;
		if ( m_aTickMultiDayIOCTimes.GetSize() > m_iTickMultiDay )
		{
			m_aTickMultiDayIOCTimes.SetSize(m_iTickMultiDay);	// �ض϶��������
		}
		UpdateTickMultiDayTitle();
		if ( bReqData )
		{
			if ( NULL != m_pRegionMain && NULL != m_pMerchXml )
			{
				OnVDataMerchTimeSalesUpdate(m_pMerchXml);
				RequestViewData();
				m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
				Invalidate();
			}
		}
	}
}

bool32 CIoViewTick::IsTickZoomed()
{
	return m_TimeZoomEnd>=m_TimeZoomStart && m_TimeZoomEnd>CGmtTime(0);
}

void CIoViewTick::CancelZoom()
{
	m_TimeZoomStart = m_TimeZoomEnd = CGmtTime(0);
}

bool32 CIoViewTick::GetShowTimeRange( OUT CGmtTime &TimeStart, OUT CGmtTime &TimeEnd )
{
	if ( IsTickZoomed() )
	{
		return GetZoomedTimeRange(TimeStart, TimeEnd);
	}
	return GetTickMultiDayTimeRange(TimeStart, TimeEnd);
}

void CIoViewTick::OnVDataMerchKLineUpdate( IN CMerch *pMerch )
{
	// ��ȡ��K������
	if ( pMerch != m_pMerchXml
		|| NULL == m_pMerchXml )
	{
		return;
	}

	// ���Ը���������صı�������

	int32 i;
	for ( i=0; i < m_MerchParamArray.GetSize() ; ++i )
	{
		T_MerchNodeUserData *pData = m_MerchParamArray[i];
		if ( NULL != pData && pMerch == pData->m_pMerchNode )
		{
			if (0. == pData->m_TrendTradingDayInfo.m_fPricePrevClose && 0. == pData->m_TrendTradingDayInfo.m_fPricePrevAvg)
			{
				float fPricePrevClose = pData->m_TrendTradingDayInfo.GetPrevReferPrice();
				pData->m_TrendTradingDayInfo.RecalcPrice(*pMerch);
				if (fPricePrevClose != pData->m_TrendTradingDayInfo.GetPrevReferPrice())
				{
					// zhangbo 20090708 #������, ��������ֵ
					//...		
					ReDrawAysnc();
				}
			}
			break;
		}
	}
	

	if ( m_iTickMultiDay == m_aTickMultiDayIOCTimes.GetSize() )
	{
		return;	// ������k���Ѿ����أ������������ﴦ��
	}
	if ( m_aTickMultiDayIOCTimes.GetSize() < 1 )
	{
		return;
	}

	int32 iDayPos;
	CMerchKLineNode *pDayKline = NULL;
	if ( !pMerch->FindMerchKLineNode(EKTBDay, iDayPos, pDayKline)
		|| NULL == pDayKline )
	{
		return;
	}
	CGmtTime TimeToday = m_aTickMultiDayIOCTimes[0].m_TimeInit.m_Time;
	SaveDay(TimeToday);
	// ʵ���п��ܳ����ڳ�ʼ��ʱû�и���k�����ɣ�����ֻ�ܼ���������k�߶��������ģ�������k��Ҫ����û���֣��������Ȼ�����һ��
	//int32 iPosToday = CMerchKLineNode::QuickFindKLineByTime(pDayKline->m_KLines, TimeToday);
	int32 iPosToday = CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(pDayKline->m_KLines, TimeToday);
	if ( iPosToday < m_aTickMultiDayIOCTimes.GetSize()-1
		|| iPosToday >= pDayKline->m_KLines.GetSize() )
	{
		return;// û�ж������������û���ҵ�
	}

	if ( pDayKline->m_KLines[iPosToday].m_TimeCurrent < TimeToday )
	{
		// ���һ���߲��ǽ����ߣ�����Ϊ���һ���������ߣ���iPosToday+1
		++iPosToday;	// ��ʱiPosTodayʵ����һ����Чλ��
	}

	// ����������K�߶��������ģ�����ʱ���������
	// tick��������洢��������������ǰ��
	int32 iPosLast = iPosToday-m_aTickMultiDayIOCTimes.GetSize();		// ����һ��������
	for ( i=iPosLast; i >= 0 && m_aTickMultiDayIOCTimes.GetSize() < m_iTickMultiDay ; --i )
	{
		const CKLine &kline = pDayKline->m_KLines[i];
		CGmtTime TimeDay = kline.m_TimeCurrent;
		SaveDay(TimeDay);
		CMarketIOCTimeInfo IOCTime;
		if ( m_pMerchXml->m_Market.GetSpecialTradingDayTime(TimeDay, IOCTime, m_pMerchXml->m_MerchInfo) )
		{
			TRACE(_T("���tick��k��: %s\n"), Time2String(TimeDay, ENTIMinute, true, false).GetBuffer());
			m_aTickMultiDayIOCTimes.Add(IOCTime);
		}
		else
		{
			ASSERT( 0 );
		}
	}

	OnVDataMerchTimeSalesUpdate(pMerch);	// ���¼���ֱ�����
	UpdateTickMultiDayTitle();
}

bool32 CIoViewTick::GetZoomedTimeRange( OUT CGmtTime &TimeStart, OUT CGmtTime &TimeEnd )
{
	if ( IsTickZoomed() )
	{
		TimeStart = m_TimeZoomStart;
		TimeEnd	  = m_TimeZoomEnd;
		return true;
	}
	return false;
}

bool32 CIoViewTick::GetTickMultiDayTimeRange( OUT CGmtTime &TimeStart, OUT CGmtTime &TimeEnd )
{
	if ( m_aTickMultiDayIOCTimes.GetSize() > 0 )
	{
		TimeStart = m_aTickMultiDayIOCTimes[m_aTickMultiDayIOCTimes.GetUpperBound()].m_TimeInit.m_Time;
		TimeEnd	  = m_aTickMultiDayIOCTimes[0].m_TimeEnd.m_Time;
		return true;
	}

	TimeStart = TimeEnd = CGmtTime::GetCurrentTime();
	return false;
}

void CIoViewTick::FillAppendMainXPosNode( const CArray<CNodeData, CNodeData&> &aSrc, OUT CArray<CNodeData, CNodeData&> &aNewNodes )
{
	CGmtTime TimeStart, TimeEnd;
	GetShowTimeRange(TimeStart, TimeEnd);
	
	aNewNodes.SetSize(0, 1500);
	aNewNodes.Copy(aSrc);

	const CNodeData *pSrcData = aSrc.GetData();	// ����ΪNULL
	const int32		iSrcDataCount = aSrc.GetSize();

	CGmtTime TimeFirstNeedAddNode(TimeStart);		// ��һ����Ҫ��ӵ�ʱ��
	if ( iSrcDataCount > 0 )
	{
		TimeFirstNeedAddNode = CGmtTime(pSrcData[iSrcDataCount-1].m_iID) + CGmtTimeSpan(0,0,1,0);
		SaveMinute(TimeFirstNeedAddNode);
	}

	// ���Ҫ��ӵ�β������, �����ǰ���һ����һ�������������Ա�Ȼ�������Ҫ�ķָ��
	// ��ʱ�俪ʼ������[��->��]
	for ( int32 i=m_aTickMultiDayIOCTimes.GetUpperBound(); i >=0 ; --i )
	{
		const CMarketIOCTimeInfo &IOCTime = m_aTickMultiDayIOCTimes[i];
		if ( IOCTime.m_TimeEnd.m_Time < TimeFirstNeedAddNode )
		{
			ASSERT( aNewNodes.GetSize() == iSrcDataCount );	// ����Ӧ��ǰ
			continue;	// ����Ľ���ʱ�����Ҫ��ʾ��ʼС��������ʾ����
		}
		if ( TimeEnd < IOCTime.m_TimeInit.m_Time )
		{
			break;	// ����Ŀ�ʼʱ�����ʾ�����󣬲��ؼ�����ʾ��
		}

		int32 iDayPos = aNewNodes.GetSize();

		for ( int32 j=0; j <= IOCTime.m_aOCTimes.GetSize()-2 ; j+=2 )
		{
			const CGmtTime &TimeOpen	= IOCTime.m_aOCTimes[j];
			const CGmtTime &TimeClose	= IOCTime.m_aOCTimes[j+1];
			CGmtTime TimeS = max(TimeOpen, TimeFirstNeedAddNode);
			CGmtTime TimeE = min(TimeEnd, TimeClose);

			for ( int32 tK=TimeS.GetTime(); tK <= TimeE.GetTime() ; tK+=60 )
			{
				CNodeData node;
				node.m_iID = tK;
				node.m_iFlag = CNodeData::KValueInvalid;
				aNewNodes.Add(node);
			}
		}//for ( int32 j=0; j <= IOCTime.m_aOCTimes.GetSize()-2 ; j+=2 )

		// ����
		if ( iDayPos > 0 
			&& iDayPos < aNewNodes.GetSize()
			&& aNewNodes[iDayPos-1].m_iID/(3600*24) != aNewNodes[iDayPos].m_iID/(3600*24) )
		{
			aNewNodes[iDayPos-1].m_iFlag |= CNodeData::KValueDivide;
			CGmtTime cPreTime(aNewNodes[iDayPos-1].m_iID);
			CGmtTime cCurTime(aNewNodes[iDayPos].m_iID);
			TRACE(_T("�����ָ���: %s->%s\n")
				, Time2String(cPreTime, ENTIMinute, true, false).GetBuffer()
				, Time2String(cCurTime, ENTIMinute, true, false).GetBuffer());
		}
	}//for ( int32 i=m_aTickMultiDayIOCTimes.GetUpperBound(); i >=0 ; --i )
}

void CIoViewTick::CalcKLineAvgValue( INOUT CArray<CKLine, CKLine> &aKlines )
{
	// �����Ӧ�������еõ�������
	const int32 iKlineCount = aKlines.GetSize();
	CKLine *pKlines	= aKlines.GetData();	// ����ΪNULL
	float fAvgSum	= 0.0f;
	int32 iAvgCount = 0;
	int32 tTimeLastDay = 0;	// ����Ӧ�����¼���avg
	ASSERT( NULL != m_pMerchXml );
	for ( int32 i=0; i < iKlineCount ; ++i )
	{
		if ( pKlines[i].m_TimeCurrent.GetTime()/(3600*24) != tTimeLastDay )
		{
			// ���죬�������¼���
			fAvgSum		= 0.0f;
			iAvgCount	= 0;
			tTimeLastDay = pKlines[i].m_TimeCurrent.GetTime()/(3600*24);
		}
		
		fAvgSum += pKlines[i].m_fPriceClose;
		++iAvgCount;
		pKlines[i].m_fPriceAvg = fAvgSum/iAvgCount;

		// ȡsavedec����Чλ��
	}
}

void CIoViewTick::FillMainVirtualKLineData(const CArray<CKLine, CKLine> &aSrc, OUT CArray<CKLine, CKLine> &aDst, OUT CArray<DWORD, DWORD> &aDstFlag)
{
	// ��fullData�����n���������
	// �����β������
	CGmtTime TimeStart, TimeEnd;
	GetTickMultiDayTimeRange(TimeStart, TimeEnd);

	aDst.SetSize(0, 1500);
	aDstFlag.SetSize(0, 1500);

	int32 iShowPosStart = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(aSrc, TimeStart);
	int32 iShowPosEnd	= CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(aSrc, TimeEnd);
	int32 iSrcDataCount = 0;
	const CKLine *pSrcData = aSrc.GetData();	// ����ΪNULL
	if ( iShowPosStart >=0 && iShowPosEnd>=iShowPosEnd && iShowPosEnd < aSrc.GetSize() )
	{
		iSrcDataCount = iShowPosEnd-iShowPosStart+1;
	}
	else
	{
		// ������
		iShowPosStart = 0;
		iSrcDataCount = 0;
	}

	if ( NULL != pSrcData )
	{
		pSrcData += iShowPosStart;
	}

	bool32	bMultiDay = m_aTickMultiDayIOCTimes.GetSize() > 1;
	bool32	bShowHourInMultiDay = m_aTickMultiDayIOCTimes.GetSize() <= 3;
	int32 iIndex = 0;
	
	/*bool32 bZoomed = */IsTickZoomed();

	int32 i = 0;
	for ( i=m_aTickMultiDayIOCTimes.GetUpperBound(); i >=0 ; --i )
	{
		const CMarketIOCTimeInfo &IOCTime = m_aTickMultiDayIOCTimes[i];
		if ( IOCTime.m_TimeEnd.m_Time < TimeStart )
		{
			ASSERT( 0 == iIndex && aDst.GetSize()==0 );	// ����Ӧ��ǰ
			continue;	// ����Ľ���ʱ�����ʾ��ʼС��������ʾ����
		}
		if ( TimeEnd < IOCTime.m_TimeInit.m_Time )
		{
			break;	// ����Ŀ�ʼʱ�����ʾ�����󣬲��ؼ�����ʾ��
		}

		int32 iDayPos = aDst.GetSize();		// ��������ݵ����
		
		// ��������λ�ñ����е�
		// ����������������ڷֱ���������С�ڷ����������������ӵ㣬��������
		for ( int32 j=0; j <= IOCTime.m_aOCTimes.GetSize()-2 ; j+=2 )
		{
			// ���� Сʱ ��Сʱ
			CGmtTime TimeOpen	= IOCTime.m_aOCTimes[j];
			CGmtTime TimeClose	= IOCTime.m_aOCTimes[j+1];

			// ���ʵ�ʿ�����ʱ�䣬�����������ʵ�ʵ�
			if ( 0 == j )
			{
				TimeOpen = IOCTime.m_TimeInit.m_Time;	// ��һ���������г���ʼ��ʱ��Ϊ��ʼ
			}
			else if ( IOCTime.m_aOCTimes.GetSize()-2==j )
			{
				TimeClose = IOCTime.m_TimeEnd.m_Time;	// ���һ�������Խ���ʱ��Ϊ����
			}
			TimeOpen	= max(TimeOpen, TimeStart);
			SaveMinute(TimeOpen);
			TimeClose	= min(TimeClose, TimeEnd);
			SaveMinute(TimeClose);
			TimeClose += CGmtTimeSpan(0, 0, 0, 59);	// ��ȷ��ֻ�ܵ�������
			//int32	 iMinuteCount = (TimeClose.GetTime()-TimeOpen.GetTime())/60 +1;

			int32 iOCPos = aDst.GetSize();	// �¿������ݵ����
			
			int32 iPosOpen = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(aSrc, TimeOpen);
			int32 iPosClose = CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(aSrc, TimeClose);
			bool32 bNeedFill = iPosOpen>=0&&iPosClose>=iPosOpen ? ((iPosClose-iPosOpen+1) <= 0) : true;

			// ��ԭʵ�ʴ��ڵĿ�����ʱ��, ��������ָ��
			TimeOpen	= max(IOCTime.m_aOCTimes[j], TimeStart);
			SaveMinute(TimeOpen);
			TimeClose	= min(IOCTime.m_aOCTimes[j+1], TimeEnd);
			SaveMinute(TimeClose);
			TimeClose += CGmtTimeSpan(0, 0, 0, 59);	// ��ȷ��ֻ�ܵ�������

			if ( bNeedFill )
			{
				// ÿ��������һ����, ��֤�˼�ʹ��С�ָ����Ȼ������һ����
				// �������ο��������ݣ����ݲ�����
				DWORD dwFlag = iPosClose-iPosOpen+1 <=0 ? CNodeData::KValueInvalid : CNodeData::KValueInhert|CNodeData::KValueInvalid;
				for ( int32 tK=TimeOpen.GetTime(); tK < TimeClose.GetTime() ; tK+=60 )
				{
					bool32 bAdded = false;

					if(pSrcData)
					{
						// ������ڱ��η��ӵ�ǰ���κ���Ч����
						for ( 
							; iIndex < iSrcDataCount && (pSrcData[iIndex].m_TimeCurrent.GetTime()<tK)
							; ++iIndex)
						{
							// ʵ������
							aDst.Add(pSrcData[iIndex]);
							aDstFlag.Add(0);
						}

						// ������ڱ����ӵ������
						for ( 
							; iIndex < iSrcDataCount && (pSrcData[iIndex].m_TimeCurrent.GetTime()>=tK&& pSrcData[iIndex].m_TimeCurrent.GetTime()<tK+60)
							; ++iIndex)
						{
							// ʵ������
							aDst.Add(pSrcData[iIndex]);
							aDstFlag.Add(0);
							bAdded = true;
						}
						if ( !bAdded )
						{
							// ��ٵĵ�
							CKLine kline;
							kline.m_TimeCurrent = CGmtTime(tK);
							aDst.Add(kline);
							aDstFlag.Add(dwFlag);
						}
					}
					// ���ӵ������������¸����ӵ����������
				}
			}// if ( bNeedFill )
			else
			{
				// �����������Ϳ�����

				// ������ڱ����նε�����, С�ڱ�������ʱ��Ķ�����������ն�
				if(pSrcData)
				{
					for ( 
						; iIndex < iSrcDataCount && (pSrcData[iIndex].m_TimeCurrent<TimeClose)
						; ++iIndex)
					{
						// ʵ������
						aDst.Add(pSrcData[iIndex]);
						aDstFlag.Add(0);
					}
				}


				// ���β��û�����ݵ�λ����Ҫ����1����һ���������
				// ���ĵ��Ȼ��������
				if ( iIndex >= iSrcDataCount )
				{
					ASSERT( iSrcDataCount >0 && pSrcData != NULL );	// �ܽ���������֧���Ȼ������
					CGmtTime TimeLastData = pSrcData[iIndex-1].m_TimeCurrent;
					TimeLastData = max(TimeLastData, TimeOpen);
					SaveMinute(TimeLastData);
					TimeLastData += CGmtTimeSpan(0,0,1,0);
					for ( int32 tK=TimeLastData.GetTime(); tK < TimeClose.GetTime() ; tK+=60 )
					{	
						// ��ٵĵ�
						CKLine kline;
						kline.m_TimeCurrent = CGmtTime(tK);
						aDst.Add(kline);
						aDstFlag.Add(CNodeData::KValueInvalid);	
					}
				}
			}// if ( bNeedFill ) else

			// �����ָ������
			// �����Щ�ָ�����ݺ���û��ʵ�ʵ������ˣ���㲻�ܼ̳���һ����Ч��
			if ( !bMultiDay )
			{
				// ��Сʱ����, ����Ҫ���뿪/����ʱ���Сʱ, ���ܳ��ֶϲ��
				// ��Ϊ���뿪��30���ӵİ�Сʱ��
				int32 iPos = iOCPos;
				for ( int32 tK=IOCTime.m_aOCTimes[j].GetTime()+1800; tK < IOCTime.m_aOCTimes[j+1].GetTime(); tK+=1800 )
				{
					if ( tK < TimeOpen.GetTime() )
					{
						continue;	// ���ܲ�������ʾ�������
					}
					if ( tK > TimeClose.GetTime() )
					{
						break;	// ���ܲ�����ʾ���
					}

					int32 tHalfM = tK/60;//(tK/1800)*1800/60;
					bool32 bExist = false;
					
					for ( ; iPos < aDst.GetSize() ; ++iPos )
					{
						if ( aDst[iPos].m_TimeCurrent.GetTime()/60 == tHalfM )
						{
							bExist = true;
							break;
						}
						else if ( aDst[iPos].m_TimeCurrent.GetTime()/60 > tHalfM )
						{
							break;	// ��Ҫ�����λ�ò���һ����Сʱ��
						}
					}
					if ( !bExist )
					{
						ASSERT( iPos < aDst.GetSize() );
						// ��ٵĵ�
						CKLine kline;
						int64 tmpTime = int64(tHalfM*60);
						kline.m_TimeCurrent = CGmtTime((LONGLONG)tmpTime);
						aDst.InsertAt(iPos, kline);
						aDstFlag.InsertAt(iPos, CNodeData::KValueInvalid|CNodeData::KValueInhert);
					}
				}
			}
			else if ( bShowHourInMultiDay )
			{
				// Сʱ����, ����Ҫ���뿪/����ʱ���Сʱ
				// ��Ϊ���뿪��Ϊ60���ӵ�ΪСʱ��
				int32 iPos = iOCPos;
				for ( int32 tK=IOCTime.m_aOCTimes[j].GetTime()+3600; tK < IOCTime.m_aOCTimes[j+1].GetTime() ; tK+=3600 )
				{
					if ( tK < TimeOpen.GetTime() )
					{
						continue;	// ���ܲ�������ʾ�������
					}
					if ( tK > TimeClose.GetTime() )
					{
						break;	// ���ܲ�����ʾ���
					}

					int32 tHalfM = tK/60;//(tK/3600)*3600/60;
					bool32 bExist = false;
					
					for ( ; iPos < aDst.GetSize() ; ++iPos )
					{
						if ( aDst[iPos].m_TimeCurrent.GetTime()/60 == tHalfM )
						{
							bExist = true;
							break;
						}
						else if ( aDst[iPos].m_TimeCurrent.GetTime()/60 > tHalfM )
						{
							break;	// ��Ҫ�����λ�ò���һ��Сʱ��
						}
					}
					if ( !bExist )
					{
						ASSERT( iPos < aDst.GetSize() );
						// ��ٵĵ�
						CKLine kline;
						int64 tmpTime = int64(tHalfM*60);
						kline.m_TimeCurrent = CGmtTime((LONGLONG)tmpTime);
						aDst.InsertAt(iPos, kline);
						aDstFlag.InsertAt(iPos, CNodeData::KValueInvalid|CNodeData::KValueInhert);
					}
				}
			}
			
			// ����������
			if ( j > 0 || !bMultiDay )
			{
				// �����µ�һ�����̱���Ϊ�����ķָ�����뿪�̷ָ�
				int32 tK=IOCTime.m_aOCTimes[j].GetTime();
				bool32 bExist = false;
				int32 iPos = iOCPos;
				for ( ; iPos < aDst.GetSize() ; ++iPos )
				{
					if ( aDst[iPos].m_TimeCurrent.GetTime()/60 == tK/60 )
					{
						bExist = true;
						break;
					}
					else if ( aDst[iPos].m_TimeCurrent.GetTime()/60 > tK/60 )
					{
						break;	// ��Ҫ�����λ�ò���һ�����̵�
					}
				}
				if ( !bExist )
				{
					ASSERT( iPos < aDst.GetSize() );
					// ��ٵĵ�
					CKLine kline;
					kline.m_TimeCurrent = CGmtTime(tK);
					aDst.InsertAt(iPos, kline);
					aDstFlag.InsertAt(iPos, CNodeData::KValueInvalid|CNodeData::KValueInhert);
				}
			}

			if ( (0 == i && IOCTime.m_aOCTimes.GetSize()-1 == j+1) )
			{
				// ��������һ�����̣���Ҫ��־���һ������
				int32 tK=IOCTime.m_aOCTimes[j+1].GetTime();
				bool32 bExist = false;
				int32 iPos = iOCPos;
				for ( ; iPos < aDst.GetSize() ; ++iPos )
				{
					if ( aDst[iPos].m_TimeCurrent.GetTime()/60 == tK/60 )
					{
						bExist = true;
						break;
					}
					else if ( aDst[iPos].m_TimeCurrent.GetTime()/60 > tK/60 )
					{
						break;	// ��Ҫ�����λ�ò���һ�����̵�
					}
				}
				if ( !bExist )
				{
					// ���һ�����̵��п��ܲ�û����Ӧ�����ݵ�
					ASSERT( iPos <= aDst.GetSize() );
					// ��ٵĵ�
					CKLine kline;
					kline.m_TimeCurrent = CGmtTime(tK);
					aDst.InsertAt(iPos, kline);
					aDstFlag.InsertAt(iPos, CNodeData::KValueInvalid|CNodeData::KValueInhert);
				}
			}
		}//for ( int32 j=0; j < IOCTime.m_aOCTimes.GetSize()-2 ; j+=2 )

		// ������̺�->����ʱ�����Ч����
		// ������ڱ���ǰ���κ���Ч����
		CGmtTime TimeLastEnd(IOCTime.m_TimeEnd.m_Time);
		TimeLastEnd = min(TimeLastEnd, TimeEnd);
		if(pSrcData)
		{
			for ( 
				; iIndex < iSrcDataCount && (pSrcData[iIndex].m_TimeCurrent<TimeLastEnd)
				; ++iIndex)
			{
				// ʵ������
				aDst.Add(pSrcData[iIndex]);
				aDstFlag.Add(0);
			}
		}

		// �������ָ������� - ����ֻҪ�п�������Ϣ���Ȼ�и���ĵ�һ���������
		ASSERT( iDayPos < aDst.GetSize() && aDst.GetSize()==aDstFlag.GetSize() );
		if ( iDayPos > 0 )
		{
			aDstFlag[iDayPos-1] |= CNodeData::KValueDivide; // ���������־��datainvalid�ĳ�ͻ TODO
// 			TRACE(_T("�����ָ���: %s->%s, ����%f-%f\n")
// 				, Time2String(aDst[iDayPos-1].m_TimeCurrent, ENTIMinute, true, false)
// 				, Time2String(aDst[iDayPos].m_TimeCurrent, ENTIMinute, true, false)
// 				, aDst[iDayPos-1].m_fPriceAvg, aDst[iDayPos].m_fPriceAvg);
		}
		else
		{
// 			TRACE(_T("���������ָ�: ->%s\n")
// 				, Time2String(aDst[iDayPos].m_TimeCurrent, ENTIMinute, true, false)); 
		}
		// ÿ���������Ҫ�Ͽ��ߵ�����
	}//for ( int32 i=m_aTickMultiDayIOCTimes.GetUpperBound(); i >=0 ; --i )

	ASSERT( aDst.GetSize()==aDstFlag.GetSize() /*&& aDst.GetSize() >= aSrc.GetSize()*/ );

	// ���浱ǰ���е����ݵ�����x��ʱ��ڵ���
	m_aAllXTimeIds.SetSize(0, aDst.GetSize());
	for ( i=0; i < aDst.GetSize() ; ++i )
	{
		m_aAllXTimeIds.Add(aDst[i].m_TimeCurrent.GetTime());
	}

	// �޳���β�����ӵĶ������ݣ���Щ������������calcxʱ�Զ�����
	for ( int32 iDelEnd=aDstFlag.GetUpperBound(); iDelEnd >=0; --iDelEnd )
	{
		if ( !CheckFlag(aDstFlag[iDelEnd], CNodeData::KValueInvalid) )
		{
			break;
		}
		aDstFlag.RemoveAt(iDelEnd);
		aDst.RemoveAt(iDelEnd);
	}

	// �ָ�㣬���������ȵĴ���

	// �ָ�㴦��
	// ����:
	//		30����һ��dot�ָ���(���뿪��30����), �볡��ʵ��, ��ʾ30����ʱ��&�볡
	// ����:
	//		<=3�գ�60����һ��dot�ָ���(���뿪��60����), �볡ʵ��, ��ʵ, ��ʾ�볡&��
	//		>3�գ��볡dot�ָ���, һ��ʵ, ��ʾ�볡&��
	// ����ڷָ��ʱû�����ݣ�����ǰ��������֮����е���Ϊ�ָ���λ��
	// ����: ÿ������ݱ��뵥��һ����

	// ���Կ����Էָ�����ж����ָ��֮������Ӧ���ж��ٵ� TODO
}

void CIoViewTick::FillMainVirtualShowData()
{
	// ��Ҫ���������������
	T_MerchNodeUserData *pData = m_MerchParamArray.GetSize()>0? m_MerchParamArray[0]:NULL;
	if ( NULL == pData )
	{
		return;
	}
	CGmtTime TimeStart, TimeEnd;
	if ( !GetTickMultiDayTimeRange(TimeStart, TimeEnd) )
	{
		return;
	}

	// ������Full�����Ѿ�������˻�����
	
	int32 iShowPosInFullList = 0;
	int32 iShowCountInFullList = 0;
	// ������ʾ����
	int32 iShowPosStart = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pData->m_aKLinesFull, TimeStart);
	int32 iShowPosEnd	= CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(pData->m_aKLinesFull, TimeEnd);
	if (iShowPosStart >= 0 && iShowPosStart < pData->m_aKLinesFull.GetSize()
		&& iShowPosEnd >= iShowPosStart && iShowPosEnd < pData->m_aKLinesFull.GetSize() )
	{
		iShowPosInFullList = iShowPosStart;
		iShowCountInFullList = iShowPosEnd-iShowPosStart+1;
		
		// ���������Լ��n���ڵ���������
		CArray<CKLine, CKLine> aTmp;
		aTmp.Copy(pData->m_aKLinesFull);

		FillMainVirtualKLineData(aTmp, pData->m_aKLinesFull, pData->m_aKLinesShowDataFlag);

		CGmtTime TimeShowStart, TimeShowEnd;
		GetShowTimeRange(TimeShowStart, TimeShowEnd);
		iShowPosStart	= CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pData->m_aKLinesFull, TimeShowStart);
		iShowPosEnd		= CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(pData->m_aKLinesFull, TimeShowEnd);
		// �п��ܸ�����û���ʺ���ʾ��k������
		if ( iShowPosStart <0 || iShowPosEnd<iShowPosStart || iShowPosEnd >= pData->m_aKLinesFull.GetSize() )
		{
			iShowPosStart = 0;
			iShowPosEnd = -1;
		}
		iShowPosInFullList = iShowPosStart;
		iShowCountInFullList = iShowPosEnd-iShowPosStart+1;
	}
	else
	{
		// ������
		ClearLocalData(false);

		CArray<CKLine, CKLine> aTmp;
		FillMainVirtualKLineData(aTmp, pData->m_aKLinesFull, pData->m_aKLinesShowDataFlag);	// ������������ʼ������ʱ������Ϣ
		pData->m_aKLinesFull.RemoveAll();
		iShowPosInFullList = 0;
		iShowCountInFullList = 0;
		//iShowCountInFullList = pData->m_aKLinesFull.GetSize();
	}

	// ����־��ʵ������ͬ��
	ASSERT( pData->m_aKLinesFull.GetSize() == pData->m_aKLinesShowDataFlag.GetSize() );
	if ( iShowPosInFullList+iShowCountInFullList < pData->m_aKLinesShowDataFlag.GetSize() )
	{
		int32 iPos = iShowPosInFullList+iShowCountInFullList;
		pData->m_aKLinesShowDataFlag.RemoveAt(iPos, pData->m_aKLinesShowDataFlag.GetSize()-iPos);
	}
	if ( iShowPosInFullList > 0 )
	{
		pData->m_aKLinesShowDataFlag.RemoveAt(0, iShowPosInFullList);
	}
	ASSERT( pData->m_aKLinesShowDataFlag.GetSize() == iShowCountInFullList );
	
	//********************************************************************************************************************************************
	// ������ͼ�����ָ��
	//bool32 bShowVolColor = IsShowVolBuySellColor();
	for (int32 j = 0; j < pData->aIndexs.GetSize(); j++)
	{
		T_IndexParam* pIndex = pData->aIndexs.GetAt(j);
		g_formula_compute(this, this, (CChartRegion*)pIndex->pRegion, pData, pIndex, iShowPosInFullList, iShowPosInFullList + iShowCountInFullList);

		// �����ʱ������Kline�����в������������ݣ�so��������Ҫ����ɽ����ĺ�����Ϣ
		// Full������compare���ݲ���һ���ϸ�Եȣ�����full�����е�ʵ������һ����compare�д���
		if ( pIndex->strIndexName == _T("VOLFS") )
		{
			for ( int32 k=iShowPosInFullList; k < iShowPosInFullList+iShowCountInFullList ; ++k )
			{
				
			}
		}
	}
	
	// ���ɵ�ǰ��ʾ������
	bool32 bUpdate = pData->UpdateShowData(iShowPosInFullList, iShowCountInFullList);
	// ��ԭfullk��
	if (!bUpdate)
	{
		return;
	}
}

void CIoViewTick::OnRectZoomOut( int32 iNodeBegin, int32 iNodeEnd )
{
	// �������:
	if (m_MerchParamArray.GetSize() <= 0)
	{
		return;
	}
	
	T_MerchNodeUserData *pMainData = m_MerchParamArray[0];
	if (NULL == pMainData)
	{
		return;
	}

	// iNodeBegin��end��ʾ�ĵ���FullKline�е�λ�ã���tickͼ�У��п����������㶼�����
	// �����Ͳ���fullkline��
	iNodeBegin	-= pMainData->m_iShowPosInFullList;
	iNodeEnd	-= pMainData->m_iShowPosInFullList;

	if ( iNodeBegin > iNodeEnd )
	{
		int32 iTmp	= iNodeBegin;
		iNodeBegin	= iNodeEnd;
		iNodeEnd	= iTmp;
	}
	
	if ( iNodeBegin > iNodeEnd 
		|| iNodeBegin < 0
		|| iNodeEnd >= m_pRegionMain->m_aXAxisNodes.GetSize() )
	{
		return;
	}

	if ( iNodeEnd - iNodeBegin +1 < 20 )
	{
		return;	// ����̫����
	}
	
	// �������ŷ�Χ
	m_TimeZoomStart = CGmtTime(m_pRegionMain->m_aXAxisNodes[iNodeBegin].m_iTimeId);
	m_TimeZoomEnd	= CGmtTime(m_pRegionMain->m_aXAxisNodes[iNodeEnd].m_iTimeId);

	OnVDataMerchTimeSalesUpdate(m_pMerchXml);
	
	// ��ʾˢ��
	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	
	for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
	{
		m_SubRegions[i].m_pSubRegionMain->SetDrawFlag(CRegion::KDrawFull);
	}
	
	// �ػ���ͼ
 	RedrawWindow();
}

BOOL CIoViewTick::PreTranslateMessage( MSG* pMsg )
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
			
			SetTickMulitDay(iChar, true);
			return TRUE;
		}
	}
	else if ( WM_KEYDOWN == pMsg->message )
	{
		if ( pMsg->wParam == VK_UP && IsCtrlPressed() )
		{
			int32 iCur = m_iTickMultiDay;
			if ( iCur < 10 )
			{
				SetTickMulitDay(++iCur, true);
			}
			return TRUE;
		}
		else if ( pMsg->wParam == VK_DOWN && IsCtrlPressed() )
		{
			int32 iCur = m_iTickMultiDay;
			if ( iCur > 1 )
			{
				SetTickMulitDay(--iCur, true);
			}
			return TRUE;
		}
		else if ( VK_F8 == pMsg->wParam )
		{
			PostMessage(WM_COMMAND, ID_TICK_SHOWTREND, 0);
			return TRUE;
		}
		else if ( VK_SPACE == pMsg->wParam )
		{
			PostMessage(WM_COMMAND, ID_TICK_CANCELZOOM, 0);
			return TRUE;
		}
	}

	return CIoViewChart::PreTranslateMessage(pMsg);
}

void CIoViewTick::UpdateTickMultiDayTitle()
{
	if(NULL != m_pRegionMain)
	{
		bool32 bValidMerch = NULL != m_pMerchXml;
		if ( m_aTickMultiDayIOCTimes.GetSize() > 1 && bValidMerch )
		{
			// ���շ�ʱ���� ��Ʒ[���n�շ�ʱ] , ��ʱ��Ʒ�ȱز�ΪNULL
			CString StrName;
			StrName.Format(_T("%s�����%d�ա�"), m_pMerchXml->m_MerchInfo.m_StrMerchCnName.GetBuffer(), m_aTickMultiDayIOCTimes.GetSize());
			m_pRegionMain->SetTitle(StrName);
		}
		else if ( bValidMerch )
		{
			m_pRegionMain->SetTitle(m_pMerchXml->m_MerchInfo.m_StrMerchCnName + _T(" "));
		}
		else
		{
			m_pRegionMain->SetTitle(_T("����ͼ"));
		}
		if ( NULL != m_pRegionMain )
		{
			m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
			Invalidate();
		}
	}
}

bool32 CIoViewTick::UpdateMainMerchTick2( T_MerchNodeUserData &MerchNodeUserData )
{
	bool32 bRet = UpdateMainMerchTick(MerchNodeUserData);
	FillMainVirtualShowData();
	return bRet;
}

float CIoViewTick::GetTickPrevClose()
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
			fPriceBase = pData->m_pMerchNode->m_pRealtimePrice->m_fPricePrevAvg;	// ȡ����Ľ���
		}
		else
		{
			fPriceBase = pData->m_pMerchNode->m_pRealtimePrice->m_fPricePrevClose;	// ȡ���������
		}
	}
	return fPriceBase;
}

bool32 CIoViewTick::CalcKLinePriceBaseValue( CChartCurve *pCurve, OUT float &fPriceBase )
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
			return false;
		}
		
		T_MerchNodeUserData *pData = (T_MerchNodeUserData *)pNodes->GetUserData();
		if ( NULL == pData )
		{
			return false;
		}
		
		ASSERT( pData->m_TrendTradingDayInfo.m_bInit );
		if ( pData->bMainMerch )
		{
			fPriceBase = GetTickPrevClose();
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

bool32 CIoViewTick::FindPosInAllTimeIds( int32 iTimeId, OUT int32 &iPos )
{
	ASSERT( m_aAllXTimeIds.GetSize() > 0 );
	iPos = -1;
	int32 iLow  = 0;
	int32 iHigh = m_aAllXTimeIds.GetUpperBound();
	// time С->��
	// ��������ͬһ������n�����ݵľͱ����ˣ���λ����
	while ( iLow <= iHigh )
	{
		int32 iHalf = (iHigh+iLow)/2;
		int32 iT = m_aAllXTimeIds[iHalf];
		if ( iTimeId == iT )
		{
			iPos = iHalf;
			return true;
		}
		else if ( iT > iTimeId )
		{
			iHigh = iHalf-1;
		}
		else
		{
			iLow = iHalf+1;
		}
	}
	return false;
}

void CIoViewTick::OnMySettingChanged()
{
	ReDrawAysnc();
}
