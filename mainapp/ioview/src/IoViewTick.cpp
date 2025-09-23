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

const int32 KMinTickNumPerScreen		= 10;			// 最少显示K线条数, 包括KSpaceRightCount位置
const int32 KMaxTickNumPerScreen		= 10000;		// 最多显示K线条数
const int32 KDefaultTickNumPerScreen	= 200;
const int32 KDefaultTickCtrlMoveSpeed	= 5;			// 默认Ctrl移动速度周期

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

	m_eMainCurveAxisYType = CPriceToAxisYObject::EAYT_Pecent;	// 闪电图仅支持这一个

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
	pGridCtrl->SetCellType(0,0, RUNTIME_CLASS(CGridCellSymbol));	// 日期
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(0,0);
	pCellSymbol->SetDefaultFont(ESFSmall);
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
	pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	
	pGridCtrl->SetCellType(1,0, RUNTIME_CLASS(CGridCellSymbol));	// 时间
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(1,0);
	pCellSymbol->SetDefaultFont(ESFSmall);
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
	pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	
	pGridCtrl->SetCellType(3,0, RUNTIME_CLASS(CGridCellSymbol));	// 价格
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(3,0);
	pCellSymbol->SetDefaultFont(ESFSmall);
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
	pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

	pGridCtrl->SetCellType(5,0, RUNTIME_CLASS(CGridCellSymbol));	// 均价
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(5,0);
	pCellSymbol->SetDefaultFont(ESFSmall);
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
	pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	
	pGridCtrl->SetCellType(7,0, RUNTIME_CLASS(CGridCellSymbol));
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(7,0);		// 涨跌
	pCellSymbol->SetDefaultFont(ESFSmall);
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	
	pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	
	pGridCtrl->SetCellType(9,0, RUNTIME_CLASS(CGridCellSymbol));
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(9,0);		// 涨幅
	pCellSymbol->SetDefaultFont(ESFSmall);
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	
	pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	
	pCell = (CGridCellSys *)pGridCtrl->GetCell(11, 0);				// 成交量
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetDefaultTextColor(ESCVolume);
	
	pCell = (CGridCellSys *)pGridCtrl->GetCell(13, 0);				// 金额
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
	//主K
	new CChartRegion ( this, m_pRegionMain, _T("k线"),
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

	//X轴
	new CChartRegion ( this,m_pRegionXBottom, _T("x轴"), 
		CRegion::KFixHeight|
		CChartRegion::KXAxis|
		CChartRegion::KDragXAxisPanAble);

	m_pRegionXBottom->AddDrawNotify(this,ERDNBeforeDrawCurve);
	m_pRegionXBottom->SetDataSourceRegion(m_pRegionMain);

	//增加新区域处
	new CChartRegion ( this,m_pRegionSeparator, _T("split"), 
		CChartRegion::KUserChart);
	m_pRegionSeparator->AddDrawNotify(this,ERDNAfterDrawCurve);

	new CChartRegion ( this,m_pRegionLeftBottom, _T("左下角"),
		CChartRegion::KUserChart|
		CRegion::KFixHeight|
		CRegion::KFixWidth);
	m_pRegionLeftBottom->AddDrawNotify(this,ERDNBeforeDrawCurve);
	m_pRegionLeftBottom->AddDrawNotify(this,ERDNAfterDrawCurve);
	
	new CChartRegion ( this,m_pRegionRightBottom, _T("右下角"),
		CChartRegion::KUserChart|
		CRegion::KFixHeight|
		CRegion::KFixWidth);
	m_pRegionRightBottom->AddDrawNotify(this,ERDNBeforeDrawCurve);
	m_pRegionRightBottom->AddDrawNotify(this,ERDNAfterDrawCurve);

	new CChartRegion ( this,m_pRegionYLeft, _T("左y轴"),
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

	new CChartRegion ( this,m_pRegionYRight, _T("右y轴"),
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

// 通知视图改变关注的商品
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
			CancelZoom();	// 取消所有缩放信息
		}

		// 初始化当天的交易日信息
		m_aTickMultiDayIOCTimes.RemoveAll();
		CMarketIOCTimeInfo IOCTime;
		CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();
		pMerch->m_Market.GetRecentTradingDay(TimeNow, IOCTime, pMerch->m_MerchInfo);
		m_aTickMultiDayIOCTimes.Add(IOCTime);	// 加入当天的开收信息
		
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
			
			// zhangbo 20090708 #待补充， 中轴更新
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
			pCurve->SetTitle(_T("闪电图 "));
			CChartDrawer* pTickDrawer = new CChartDrawer(*this, CChartDrawer::EKDSTickPrice);
			
			// 		if ( -1 != m_iDrawTypeXml )
			// 		{
			// 			pTickDrawer->m_eChartDrawType = (CChartDrawer::E_ChartDrawStyle)m_iDrawTypeXml;
			// 			m_iDrawTypeXml = 0;
			// 		}
			
			pCurve->AttatchDrawer(pTickDrawer);
			pCurve->AttatchNodes(pNodes);

			// 新增均线
			CChartCurve *pCurveAvg = m_pRegionMain->CreateCurve(CChartCurve::KTypeTick | CChartCurve::KYTransformByAvg |CChartCurve::KDonotPick);
			pCurveAvg->SetTitle(_T("均线 "));
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
					m_pCell1->SetText(_T(" 持仓量"));
				}
				else
				{
					m_pCell1->SetText(_T(" 金额"));
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
		// zhangbo 20090824 #待补充， 当心商品不存在时，清空当前显示数据
		//...
	}
}

// 叠加视图相关
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
	
	// 以主商品时间为准请求叠加商品数据
	{
		T_MerchNodeUserData *pData0 = m_MerchParamArray[0];
		if (NULL != pData0 && pData0->m_aTicksCompare.GetSize() > 0)
		{
			// 所有商品都以主商品的时间为准请求
			CGmtTime TimeStart = pData->m_aTicksCompare[0].m_TimeCurrent.m_Time;
			CGmtTime TimeEnd = pData->m_aTicksCompare[pData->m_aTicksCompare.GetSize() - 1].m_TimeCurrent.m_Time;

			// 指定时间段请求
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
			StrTitle = pData->m_pMerchNode->m_MerchInfo.m_StrMerchCode + _T(" ") + _T(" (无)");
		}
		else
		{
			StrTitle = pData->m_pMerchNode->m_MerchInfo.m_StrMerchCode + _T("(无)");
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
			StrTitle = pMerch->m_MerchInfo.m_StrMerchCnName + _T(" 叠加 ");
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

	// zhangbo 20090707 #待补充， 重新计算交易日， 有可能是隔天初始化， 需要重新换天显示等
	//...
	if ( m_pAbsCenterManager && (m_MerchParamArray.GetSize() > 0) )
	{		
		T_MerchNodeUserData * pData = m_MerchParamArray.GetAt(0);
		ASSERT(NULL != pData);
		
		CGmtTime TimeInit	= pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time;
		CGmtTime TimeEnd	= TimeInit + CGmtTimeSpan(0, 23, 59, 59);
		
		CGmtTime TimeServer = m_pAbsCenterManager->GetServerTime();
		
		if (TimeServer < TimeInit || TimeServer > TimeEnd)	// 不是同一天， 需要重新算时间和昨收价等
		{
			// 清除所有数据
			ClearLocalData();
			
			// 获取该商品走势相关的数据
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
			
			m_aTickMultiDayIOCTimes.RemoveAll();	// 清除时间点信息
			if ( pData->m_TrendTradingDayInfo.m_bInit )
			{
				m_aTickMultiDayIOCTimes.Add(pData->m_TrendTradingDayInfo.m_MarketIOCTime);		// 给与第一天的初始化时间
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

	// 挨个商品更新所有数据
	for (int32 i = 0; i < m_MerchParamArray.GetSize(); i ++)
	{
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(i);
		if (pData->m_pMerchNode != pMerch)
			continue;
		
		// 需要更新昨收价
		if (0. == pData->m_TrendTradingDayInfo.m_fPricePrevClose && 0. == pData->m_TrendTradingDayInfo.m_fPricePrevAvg)
		{
			// 尝试更新走势相关的报价数据
			float fPricePrevClose = pData->m_TrendTradingDayInfo.GetPrevReferPrice();
			pData->m_TrendTradingDayInfo.RecalcPrice(*pMerch);
			if (fPricePrevClose != pData->m_TrendTradingDayInfo.GetPrevReferPrice())
			{
				// zhangbo 20090708 #待补充, 更新中轴值
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
	//	// 0002149 xl 0705 如果在激活后，短时间内被其它GGTongView的F7最大设置为隐藏，会导致还原时由于数据未更新而清空数据的显示错误，先取消该优化
 //		AddFlag(m_uiFlag,CIoViewChart::KMerchTimeSalesUpdate);
 //		ClearLocalData();
 //		return;
	//}
	
	if (m_MerchParamArray.GetSize() <= 0)
	{
		return;
	}
	
	// 挨个商品更新所有数据
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
			continue;	// 叠加商品不清空
		}
		pData->m_aTicksCompare.RemoveAll();
		pData->m_aKLinesFull.RemoveAll();
		pData->m_aKLinesShowDataFlag.RemoveAll();
		
		if (NULL != pData->m_pKLinesShow)
			pData->m_pKLinesShow->RemoveAll();
		
		RemoveIndexsNodesData(pData);
	}

	m_aAllXTimeIds.RemoveAll();	// 清空所有的时间轴节点
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

	// 判断当前显示， 是否显示最新价
	// 计算时使用所有可能使用的数据，显示时，仅使用部分要显示的数据
	CGmtTime TimeStartInFullList, TimeEndInFullList;
	if ( !GetTickMultiDayTimeRange(TimeStartInFullList, TimeEndInFullList) )
	{
		return false;
	}

	// 先获取对应的K线
	CMerchTimeSales* pTickRequest = pMerch->m_pMerchTimeSales;

	// 根本找不到K线数据， 那就不需要显示了
	if (NULL == pTickRequest || 0 == pTickRequest->m_Ticks.GetSize())	
	{
		MerchNodeUserData.m_aTicksCompare.RemoveAll();
		MerchNodeUserData.m_aKLinesFull.RemoveAll();

		if (NULL != MerchNodeUserData.m_pKLinesShow)		
			MerchNodeUserData.m_pKLinesShow->RemoveAll();
		
		return true;
	}

	// 
	// 优化：绝大多数情况下， 该事件都是由于历史数据更新引发的， 对这种情况做特别判断处理
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
// 		if (memcmp(pTickSrc, pTickCmp, (iCountCmp - 1) * sizeof(CTick)) == 0)	// 由于很可能是更新最后一笔， 不止是增加这一种情况
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
// 					// 增加了几笔数据
// 					bAddSome = true;
// 				}
// 			}
// 			else	// 最后一笔数据不一致
// 			{
// 				bModifyLast = true;
// 				if (0 == iCountAdd)
// 				{
// 					// 仅更新最后一笔数据
// 					bAddSome = false;
// 				}
// 				else
// 				{
// 					// 更新了最后一笔数据并且还增加了几笔数据
// 					bAddSome = true;
// 				}
// 			}
// 		}
// 	}
// 		
// 	if ( bUpdateLast && !bModifyLast && !bAddSome )
// 	{
// 		// 什么都没变, 返回咯, 可能是其它变了，总是需要重新计算
// 		//return true;
// 	}
	//////////////////////////////////////////////////////////////////////
	// 针对更新最新价和更新历史分别处理

	// ...fangz0805 暂时没处理优化
	bUpdateLast = false;

	if (bUpdateLast)	// 更新最新的几笔数据
	{
		if ( false == bModifyLast )
		{
			// 现有的最后一笔与更新来的对应位置那笔数据相同

			if ( false == bAddSome )
			{
				// 没有新增. 完全相同
				return false;
			}
			else
			{
				// 前面都相同, 最后新加的几笔数据:

			}
		}
		else
		{
			// 现有的最后一笔与更新来的对应位置那笔数据不同
			if ( false == bAddSome)
			{
				// 没有新增,仅仅最后一笔不同,类似于最新价发生变化
			}
			else
			{
				// 最后一笔不同而且还新增了节点
			}
		}		
	}
	else	// 全部更新
	{
		// 清除所有数据
		ClearLocalData();

		// 主图更新数据
		if (iCountSrc == 0)
		{
			return true;		// 现在没有数据， 就不处理了
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
						
			// 处理数据
			pData->m_aKLinesFull.SetSize(pData->m_aTicksCompare.GetSize());
			CTick *pTicksCompare = (CTick *)pData->m_aTicksCompare.GetData();
			CKLine *pKLinesFull = (CKLine *)pData->m_aKLinesFull.GetData();
			for (int32 iIndexExchange = 0; iIndexExchange < pData->m_aTicksCompare.GetSize(); iIndexExchange++)
			{
				Tick2KLine(pTicksCompare[iIndexExchange], pKLinesFull[iIndexExchange]); 
			}

			CalcKLineAvgValue(pData->m_aKLinesFull);	// 计算均价

			// 调整显示区间 交给2处理
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
// 				// 没有有效数据
// 				iShowPosInFullList = 0;	
// 				iShowCountInFullList = 0;
// 			}
// 
// 			//********************************************************************************************************************************************
// 			// 更新主图下面的指标
// 			// 交给2处理
// 			for (int32 j = 0; j < pData->aIndexs.GetSize(); j++)
// 			{
// 				T_IndexParam* pIndex = pData->aIndexs.GetAt(j);
// 				g_formula_compute(this, this, pIndex->pRegion, pData, pIndex, iShowPosInFullList, iShowPosInFullList + iShowCountInFullList);
// 			}
// 
// 			// 生成当前显示的数据
// 			if (!pData->UpdateShowData(iShowPosInFullList, iShowCountInFullList))
// 			{
// 				//ASSERT(0);
// 				return false;
// 			}

			// 根据当前生成的实际数据，填充虚拟数据
		}

		// zhangbo 20090714 #待补充， 叠加商品数据更新
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
	
	// zhangbo 20090714 #待处理
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
	//pData->pMerch已经不被这个Curve使用了,如果没有其它Curve使用,则考虑清除数据,并且从AttendMerch中删除.
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

	//如果选中了其它坐标轴
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

	// 取消缩放
	{
		pPopMenu->EnableMenuItem(ID_TICK_CANCELZOOM, MF_BYCOMMAND |(IsTickZoomed()?MF_ENABLED:(MF_GRAYED|MF_DISABLED)));
	}

	// 成交量颜色
	if ( IsShowVolBuySellColor() )
	{
		pPopMenu->CheckMenuItem(ID_TREND_SHOWVOLCOLOR, MF_BYCOMMAND|MF_CHECKED);
	}

	// 多日闪电图
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
// 	pTempMenu = pPopMenu->GetSubMenu(L"插入内容");
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
// 					StrExport = _T("导出[") + pMerchi->m_MerchInfo.m_StrMerchCnName + _T("]到Excel");
// 				}
// 			}
// 		}
// 	}
// 	if ( 0 == StrExport.GetLength() )
// 	{
// 		m_pExportNodes = m_pRegionMain->GetDependentCurve()->GetNodes();
// 		StrExport = _T("导出[") + pMerch->m_MerchInfo.m_StrMerchCnName + _T("]到Excel");
// 	}
// 	menu.ModifyODMenu(IDM_CHART_PROP,MF_BYCOMMAND|MF_STRING,IDM_CHART_PROP,StrExport);
// 
// 	// 如果处在锁定分割状态，需要删除一些按钮
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
		// 指标:
		// 指标菜单
/*		m_pRegionPick = m_pRegionMain;	

		m_FormulaNames.RemoveAll();
		CNewMenu menu;
		menu.LoadMenu(IDR_MENU_KLINE);
		CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(0));
		
		CMenu* pTempMenu = pPopMenu->GetSubMenu(_T("常用指标"));
		ASSERT( NULL != pTempMenu);
		CNewMenu* pSubMenu = DYNAMIC_DOWNCAST(CNewMenu,pTempMenu);
		
		pSubMenu->RemoveMenu(0,MF_BYPOSITION|MF_SEPARATOR);//删除尾巴
		
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
		// 前一个指标
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
			// 第一个:
			iIndex = 0; // 前一个的ID号
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
						iIndex = i-1; // 前一个的ID号
					}
				}
			}
		}
		
		MenuAddIndex(iIndex);
		
	}
	else if ( REGIONTITLEFOMULARBUTTONID == uID)
	{
		// 子Region 的指标按钮
/*		if ( NULL == m_pRegionPick )
		{
			return 0;
		}

		m_FormulaNames.RemoveAll();
		CNewMenu menu;
		menu.LoadMenu(IDR_MENU_KLINE);
		CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(0));
		
		CMenu* pTempMenu = pPopMenu->GetSubMenu(_T("常用指标"));
		ASSERT( NULL != pTempMenu);
		CNewMenu* pSubMenu = DYNAMIC_DOWNCAST(CNewMenu,pTempMenu);
		
		pSubMenu->RemoveMenu(0,MF_BYPOSITION|MF_SEPARATOR);//删除尾巴
		
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
		// 后一个指标

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
			// 第一个:
			iIndex = 0; // 前一个的ID号
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
						iIndex = i+1; // 前一个的ID号
					}
				}
			}
		}
		
		MenuAddIndex(iIndex);
	}
	else if ( REGIONTITLEADDBUTTONID == uID)
	{
		// 增加副图
		MenuAddRegion();
	}
	else if ( REGIONTITLEDELBUTTONID == uID)
	{	
		// 关闭子Region
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
			StrText.Format(_T("时: %s \n开: %s\n高: %s\n低: %s\n收: %s\n量: %s\n额: %s\n"),
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
			StrText.Format(_T("时: %s\n%s: %s\n"),
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

	// 得到主商品数据
	T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
	if ( NULL == pData)
	{
		return;
	}

	// 得到新加的ChartRegion
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

	// 得到能添加的所有指标名称
	uint32 uiFlag = NodeTimeIntervalToFlag( pData->m_eTimeIntervalFull);	
	AddFlag(uiFlag,CFormularContent::KAllowSub);

	m_FormulaNames.RemoveAll();
	CFormulaLib::instance()->GetAllowNames(uiFlag,m_FormulaNames);
	if ( 0 == m_FormulaNames.GetSize())
	{
		return;
	}	
	
	// 找不到符合条件的,就用这个:
	CString StrDefaultIndex = m_FormulaNames.GetAt(0);
	// 副图已经存在的指标
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

	// 删去重复的,剩下的就是能添加的指标:
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
			// 记录下当前这根K线
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

	//数值
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

	// xl 0001762 清除所有的空格显示，限制数字显示长度，详情窗口的宽度比YLeft能多显示一个数字 - -
	int32 iYLeftShowChar = GetYLeftShowCharWidth();
	iYLeftShowChar += 1;

	CString StrValue;

	//时间
	CGridCellSys* pCellSys = (CGridCellSys* )pGridCtrl->GetCell(0,0);
	pCellSys->SetText(StrTimeLine1 + _T(""));

	pCellSys = (CGridCellSys*)pGridCtrl->GetCell(1,0);
	pCellSys->SetText(StrTimeLine2 + _T(""));	

	//价格
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(3,0);

	//StrValue = Float2SymbolString(Tick.m_fPrice,fPricePrevClose,iSaveDec) + _T(" ");
	Float2SymbolStringLimitMaxLength(StrValue, Tick.m_fPrice, fPricePrevClose, iSaveDec, iYLeftShowChar, true);
	pCellSymbol->SetText(StrValue);	

	//均价
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(5,0);
	pCellSymbol->SetText(_T(" -"));	

	//涨跌=最新-昨收
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(7,0);
	
	StrValue = L" -";
	if (0. != Tick.m_fPrice)
	{
		//StrValue = Float2SymbolString(Tick.m_fPrice - fPricePrevClose, 0.0f, iSaveDec) + _T(" ");
		Float2SymbolStringLimitMaxLength(StrValue, Tick.m_fPrice - fPricePrevClose, 0.0f, iSaveDec, iYLeftShowChar, true, false, false);
	}
	pCellSymbol->SetText(StrValue);

	//涨跌幅(最新-昨收)/昨收*100%
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(9,0);
	
	StrValue = L" -";
	if (0. != Tick.m_fPrice && 0. != fPricePrevClose)
	{
		float fRisePercent = ((Tick.m_fPrice - fPricePrevClose) / fPricePrevClose) * 100.;
		StrValue = Float2SymbolString(fRisePercent, 0, 2, false, false, true) + _T("");
	}	
	pCellSymbol->SetText(StrValue);	

	//成交量
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(11,0);
 	CString StrVolume = Float2String(Tick.m_fVolume, 0, true) + _T(" ");
 	pCellSymbol->SetText(StrVolume);	
	Float2StringLimitMaxLength(StrValue, Tick.m_fVolume, 0, GetYLeftShowCharWidth(1), true, true);
	pCellSymbol->SetText(StrValue);
	
	//
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(13,0);	
	if ( bFutures )
	{
		// 持仓
		pCellSymbol->SetDefaultTextColor(ESCVolume);
// 		CString StrHold = Float2String(Tick.m_fHold, 0, true) + _T(" ");
// 		pCellSymbol->SetText(StrHold);
		Float2StringLimitMaxLength(StrValue, Tick.m_fHold, 0, GetYLeftShowCharWidth(1), true, true);
		pCellSymbol->SetText(StrValue);
	}
	else
	{
		// 金额
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

	// xl 0001762 清除所有的空格显示，限制数字显示长度，详情窗口的宽度比YLeft能多显示一个数字 - -
	int32 iYLeftShowChar = GetYLeftShowCharWidth();
	iYLeftShowChar += 1;

	CString StrValue;

	//时间
	CGridCellSys* pCellSys = (CGridCellSys* )pGridCtrl->GetCell(0,0);
	pCellSys->SetText(StrTimeLine1 + _T(""));

	pCellSys = (CGridCellSys*)pGridCtrl->GetCell(1,0);
	pCellSys->SetText(StrTimeLine2 + _T(""));	

	//价格
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(3,0);

	//StrValue = Float2SymbolString(Tick.m_fPrice,fPricePrevClose,iSaveDec) + _T(" ");
	Float2SymbolStringLimitMaxLength(StrValue, KLine.m_fPriceClose, fPricePrevClose, iSaveDec, iYLeftShowChar, true);
	pCellSymbol->SetText(StrValue);	

	//均价
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(5,0);
	Float2SymbolStringLimitMaxLength(StrValue, KLine.m_fPriceAvg, fPricePrevClose, iSaveDec, iYLeftShowChar, true);
	pCellSymbol->SetText(StrValue);	

	//涨跌=最新-昨收
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(7,0);
	
	StrValue = L" -";
	if (0. != KLine.m_fPriceClose)
	{
		//StrValue = Float2SymbolString(Tick.m_fPrice - fPricePrevClose, 0.0f, iSaveDec) + _T(" ");
		Float2SymbolStringLimitMaxLength(StrValue, KLine.m_fPriceClose - fPricePrevClose, 0.0f, iSaveDec, iYLeftShowChar, true, false, false);
	}
	pCellSymbol->SetText(StrValue);

	//涨跌幅(最新-昨收)/昨收*100%
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(9,0);
	
	StrValue = L" -";
	if (0. != KLine.m_fPriceClose && 0. != fPricePrevClose)
	{
		float fRisePercent = ((KLine.m_fPriceClose - fPricePrevClose) / fPricePrevClose) * 100.;
		StrValue = Float2SymbolString(fRisePercent, 0, 2, false, false, true) + _T("");
	}	
	pCellSymbol->SetText(StrValue);	

	//成交量
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(11,0);
 	CString StrVolume = Float2String(KLine.m_fVolume, 0, true) + _T(" ");
 	pCellSymbol->SetText(StrVolume);	
	Float2StringLimitMaxLength(StrValue, KLine.m_fVolume, 0, GetYLeftShowCharWidth(1), true, true);
	pCellSymbol->SetText(StrValue);
	
	//
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(13,0);	
	if ( bFutures )
	{
		// 持仓
		pCellSymbol->SetDefaultTextColor(ESCVolume);
// 		CString StrHold = Float2String(Tick.m_fHold, 0, true) + _T(" ");
// 		pCellSymbol->SetText(StrHold);
		Float2StringLimitMaxLength(StrValue, KLine.m_fHold, 0, GetYLeftShowCharWidth(1), true, true);
		pCellSymbol->SetText(StrValue);
	}
	else
	{
		// 金额
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
	
	if ( pRegion == m_pRegionMain )	// 主图的Y轴显示
	{
		CalcMainRegionXAxis(aAxisNodes, aXAxisDivide);
	}
	else
	{
		// zhangbo 20090710 #待优化， 现在都按照主region重新算一遍， 其实不用算， 复制就可以了
		//CalcMainRegionXAxis(aAxisNodes, aXAxisDivide);	

		// 副图Copy
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
		// 主图的Y轴显示
		CalcMainRegionYAxis(pDC, aYAxisDivide);
	}
	else	
	{
		// 子图的Y轴显示
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
		//主图
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

		// 主图
		if (pChartRegion == m_pRegionMain)	// 主图
		{			
			CRect RectMain = m_pRegionMain->GetRectCurves();
		
			// 画水平线
			{
				CAxisDivide *pAxisDivide = (CAxisDivide *)m_pRegionMain->m_aYAxisDivide.GetData();
				for (int32 i = 0; i < m_pRegionMain->m_aYAxisDivide.GetSize(); i++)
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
				for (int32 i = 0; i < m_pRegionMain->m_aXAxisDivide.GetSize(); i++)
				{
					CAxisDivide &AxisDivide = pAxisDivide[i];
					pt1.x = pt2.x = AxisDivide.m_iPosPixel;
					pt1.y = RectMain.top;
					pt2.y = RectMain.bottom;

					if ( AxisDivide.m_iPosPixel <= RectMain.left+1
						|| AxisDivide.m_iPosPixel >= RectMain.right-1 )
					{
						continue;	// 过于靠近坐标边界的不画
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

			// 画左文字
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

			// 画右文字
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
		else if (pChartRegion == m_pRegionXBottom)	// 画横坐标
		{
// 			CRect RectXBottom	= m_pRegionXBottom->GetRectCurves();
// 			RectXBottom.DeflateRect(2, 0, 2, 0);
// 
// 			// 画横坐标文字
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

			// 以下代码为避免绘制文字时互相叠加的情况发生
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

				// 按优先级排序
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

				// 挨个输出文字， 避免文字间重叠
				CArray<CRect, CRect&> aRectSpace;	// 空闲区域
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

					// 从空闲空间中查找， 看看是否可以显示
					int32 iPosFindInSpaceRects = -1;

					CRect *pRectSpace = (CRect *)aRectSpace.GetData();
					for (int32 iIndexSpace = 0; iIndexSpace < aRectSpace.GetSize(); iIndexSpace++)
					{
						if (rt.left < pRectSpace[iIndexSpace].left || rt.right > pRectSpace[iIndexSpace].right)
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
						pDC->SetTextColor(pAxisDivide[i].m_DivideText1.m_lTextColor);
						pDC->SetBkMode(TRANSPARENT);
						pDC->DrawText(pAxisDivide[i].m_DivideText1.m_StrText, &rt, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

						// 拆分空闲区域
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
		else if ( pChartRegion == m_pRegionLeftBottom )	// 显示当前显示的年月日
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
				//完整时间
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
		else	// 副图
		{
			for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
			{
				T_SubRegionParam &SubRegion = m_SubRegions[i];
				if (pChartRegion == SubRegion.m_pSubRegionMain)
				{
					CRect RectSubMain = SubRegion.m_pSubRegionMain->GetRectCurves();
					
					// 画水平线				
					{
						CAxisDivide *pAxisDivide = (CAxisDivide *)SubRegion.m_pSubRegionMain->m_aYAxisDivide.GetData();
					    //解决副图没有数据时，subregionmain顶部没有虚线2013-7-25
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
						for (int32 i = 0; i < m_pRegionMain->m_aXAxisDivide.GetSize(); i++)
						{
							CAxisDivide &AxisDivide = pAxisDivide[i];
							pt1.x = pt2.x = AxisDivide.m_iPosPixel;
							pt1.y = RectSubMain.top;
							pt2.y = RectSubMain.bottom;

							if ( AxisDivide.m_iPosPixel <= RectSubMain.left+1
								|| AxisDivide.m_iPosPixel >= RectSubMain.right-1 )
							{
								continue;	// 过于靠近坐标边界的不画
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
					
					// 画左文字
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
		//绘制指标水平线
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

	m_eMainCurveAxisYType = CPriceToAxisYObject::EAYT_Pecent;	// 仅支持百分比

	// 计算Y方向可以容纳多少个刻度
	float fMinYRange = 0.0f;
	if ( NULL != m_pMerchXml )
	{
		CRect rect = m_pRegionMain->GetRectCurves();
		
		CClientDC dc(this);
		CFont* pOldFont = dc.SelectObject(GetIoViewFontObject(ESFSmall));					
		int32 iHeightText = dc.GetTextExtent(_T("测试值")).cy;
		dc.SelectObject(pOldFont);
		
		int32 iHeightSub = iHeightText + 12;
		int32 iDivideCount = rect.Height() / iHeightSub + 1;
		if ( iDivideCount % 2 != 0 )	iDivideCount--;	// 偶数
		iDivideCount /= 2;	// 取一半

		double iSaveDec = m_pMerchXml->m_MerchInfo.m_iSaveDec;
		ASSERT( iSaveDec >= 0 );
		//float fPricePrevClose = pMerchData->m_TrendTradingDayInfo.GetPrevReferPrice();	// 昨收
		float fPricePrevClose = GetTickPrevClose();
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

				// 设置所有的主region中的curve的上下限值
				for (int32 m = 0; m < pSubRegionMain->m_Curves.GetSize(); m++)
				{
					CChartCurve *pChartCurve = pSubRegionMain->m_Curves[m];
					pChartCurve->SetYMinMax(fCurveYMin, fCurveYMax, bValidCurveYMinMax);
				}
			}		
		}
	}
}

// 视图优化 - 不初始化字串
bool32 CIoViewTick::CalcMainRegionXAxis(CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aAxisDivide)
{ 
	ASSERT(NULL != m_pRegionMain);

	aAxisNodes.SetSize(0);
	aAxisDivide.SetSize(0);

	// 
	if (m_MerchParamArray.GetSize() <= 0)
	{
		// 可能存在新建的K 线视图,没有数据.
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
	else	// 不应该出现的类型
	{
		return false;
	}

	// 分割线分割时间单位
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
		// 以小时为刻度单位显示
		// eDivideUnitType = EDUTByHour;
		// ...fangz0811
		eDivideUnitType = EDUTByDay;
	}
	else if (ENTIMinute30 == eTimeInterval || ENTIMinute60 == eTimeInterval)
	{
		// 以天为刻度单位显示
		eDivideUnitType = EDUTByDay;
	}
	else if (ENTIDay == eTimeInterval || ENTIWeek == eTimeInterval || ENTIDayUser == eTimeInterval)	
	{
		// 以月为刻度单位显示
		eDivideUnitType = EDUTByMonth;
	}
	else if (ENTIMonth == eTimeInterval || ENTIQuarter == eTimeInterval)
	{
		// 以年为刻度单位显示
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
		return false;	// 由于在updatemain中所有数据都已经准备好了，所以不该出现这种，要不就干脆没商品
	}
	

	// 另一种处理:
	// 先按照时间计算有多少个点
	// 对于在该分钟内无数据的，认为该分钟拥有一个空数据点，有坐标，无数据
	// 然后再计算每个点该拥有多少像素宽度
	// 最后根据多日单日计算分割点位置与文字
	// 实行另一种
	// 根据开收盘时间, 从最先的时间到晚的时间
	// 需要在UpdateMain中处理好，有些点需要穿过，有些点需要忽略
	

	// 
	CRect rect = m_pRegionMain->GetRectCurves();
	rect.DeflateRect(2, 1, 1, 1);
	//int32 iTickUnitCount = m_iNodeCountPerScreen;
	int32 iTickUnitCount = iNodeDataCount;
	if ( iTickUnitCount <= 0 )
	{
		ASSERT( 0 );
		return false;	// 没有显示的数据
	}
	float fPixelWidthPerUnit = (rect.Width() - 2) / (float)iTickUnitCount;	// 线性直接小数点吧
	
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
	
	// 根据开收盘时间, 从最先的时间到晚的时间
	int32 i = 0;
	for ( i=m_aTickMultiDayIOCTimes.GetUpperBound(); i >=0 && iIndexNode <iTickUnitCount ; --i )
	{
		const CMarketIOCTimeInfo &IOCTime = m_aTickMultiDayIOCTimes[i];
		if ( IOCTime.m_TimeEnd.m_Time < TimeStart )
		{
			continue;	// 这天的结束时间比显示开始小，不必显示这天
		}
		if ( TimeEnd < IOCTime.m_TimeInit.m_Time )
		{
			break;	// 这天的开始时间比显示结束大，不必继续显示了
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
				break;	// 没有该数据点了
			}
			
			if ( iTimeId < IOCTime.m_TimeInit.m_Time.GetTime()
				|| iTimeId > IOCTime.m_TimeEnd.m_Time.GetTime() )
			{
				break;	// 不是这个时间段的, 到下个时间段去看吧
			}
			
			CAxisNode &AxisNode = pAxisNode[iIndexNode];
			
			AxisNode.m_iTimeId = iTimeId;
			AxisNode.m_fPixelWidthPerUnit = fPixelWidthPerUnit;
			
			// 节点左 
			if (0 == iIndexNode)
			{
				AxisNode.m_iStartPixel = rect.left;		// 总是离左边框空出一个像素
			}
			else
			{
				AxisNode.m_iStartPixel = pAxisNode[iIndexNode - 1].m_iEndPixel + 1;
			}
			
			AxisNode.m_iEndPixel = rect.left + (int32)(((iIndexNode + 1) * fPixelWidthPerUnit + 0.5)) - 1;
			
			//
			if (AxisNode.m_iStartPixel > AxisNode.m_iEndPixel)
				AxisNode.m_iStartPixel = AxisNode.m_iEndPixel;
			
			// 中节点
			AxisNode.m_iCenterPixel = (AxisNode.m_iStartPixel + AxisNode.m_iEndPixel) / 2;
			iIndexNode++;
			// 单个节点处理完毕
		} // iIndexNode < iTickUnitCount
		
	}//for ( int32 i=m_aTickMultiDayIOCTimes.GetUpperBound(); i >=0 && iIndexNode <iTickUnitCount ; --i )

	// 单个节点处理完毕，开始分割点的处理
	// 分割点处理
	// 单日:
	//		30分钟一条dot分割线, 半场线实线, 显示30分钟时间&半场
	// 多日:
	//		<=3日，60分钟一条dot分割线, 半场实线, 天实, 显示半场&天
	//		>3日，半场dot分割线, 一天实, 显示半场&天
	// 如果在分割点时没有数据，则在前后两个点之间的中点作为分割点的位置
	// 在缩放状态下，根据保存的AllTimeId判断是否要显示天数与开收盘
	bool32 bZoomed = IsTickZoomed();
	int32 iNodePos = 0;
	for ( i=m_aTickMultiDayIOCTimes.GetUpperBound(); i >=0 && iNodePos<iIndexNode ; --i )
	{
		const CMarketIOCTimeInfo &IOCTime = m_aTickMultiDayIOCTimes[i];
		if ( IOCTime.m_TimeEnd.m_Time < TimeStart )
		{
			continue;	// 这天的结束时间比显示开始小，不必显示这天
		}
		if ( TimeEnd < IOCTime.m_TimeInit.m_Time )
		{
			break;	// 这天的开始时间比显示结束大，不必继续显示了
		}

		// 找到这天的起点数据
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
		int32 iDayPos = iNodePos;		// 新天的数据的起点
		

		// 多日新天的天分割线数据 - 该天只要有开收盘信息则必然有该天的第一个数据起点
		const int32 iDaySecCount = (3600*24);
		if ( bMultiDay && pAxisNode[iDayPos].m_iTimeId/iDaySecCount == IOCTime.m_TimeInit.m_Time.GetTime()/iDaySecCount )
		{
			int32 iCurTime = pAxisNode[iDayPos].m_iTimeId;
			bool32 bIsDayStart = true;
			if ( bZoomed )
			{
				// 缩放下，检查全部的坐标点，看该点是否是该天的第一个点
				// 如果是，则增加该天的日分割线(如果有同秒数据会出现误差)
				bIsDayStart = false;
				int32 iPosInAll;
				if ( FindPosInAllTimeIds(iCurTime, iPosInAll) )
				{
					if ( iPosInAll > 0 )
					{
						// 找到比这个小的数据位置，一般同秒数据最多就那么10来个
						while ( (--iPosInAll) >= 0 )
						{
							if ( m_aAllXTimeIds[iPosInAll] < iCurTime )
							{
								if ( m_aAllXTimeIds[iPosInAll]/iDaySecCount != iCurTime/iDaySecCount )
								{
									bIsDayStart = true;	// 不是同一天，那就认为这个点是这一天的第一个数据吧，不好区别同秒数据
								}
								break;
							}
						}
					}
					else
					{
						bIsDayStart = false;	// 数据第一天没有天分割点数据
					}
				}
			}
			else if ( bMultiDay && m_aTickMultiDayIOCTimes.GetUpperBound() == i )
			{
				bIsDayStart = false;	// 多日第一天没有天分割点数据
			}

			if ( bIsDayStart )
			{
				// 天分割点数据
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
		
		// 在坐标轴位置必须有点
		// 如果这整个开收盘内分笔数据总数小于1/2分钟线数，则填充分钟点，凑足数量
		for ( int32 j=0; j <= IOCTime.m_aOCTimes.GetSize()-2 ; j+=2 )
		{
			// 开收 小时 半小时
			CGmtTime TimeOpen	= IOCTime.m_aOCTimes[j];
			CGmtTime TimeClose	= IOCTime.m_aOCTimes[j+1];

			TimeOpen	= max(TimeOpen, TimeStart);
			SaveMinute(TimeOpen);
			TimeClose	= min(TimeClose, TimeEnd);
			SaveMinute(TimeClose);
			TimeClose += CGmtTimeSpan(0, 0, 0, 59);	// 精确度只能到这里了

			int32 iOCPos = iNodePos;	// 新开盘数据的起点
			// 开盘线数据
			if ( j > 0 || (!bMultiDay&&!bZoomed) )
			{
				// 多日下第一个开盘被视为天数的分割，不记入开盘分割
				// 缩放状态下，不绘制单日的第一个开盘分割点
				int32 tK=IOCTime.m_aOCTimes[j].GetTime();
				if ( tK >= TimeOpen.GetTime() )
				{
					// 只能插入在这个显示区间内的开盘点
					bool32 bExist = false;
					int32 iPos = iOCPos;
					for ( ; iPos < iIndexNode ; ++iPos )
					{
						if ( pAxisNode[iPos].m_iTimeId/60 == tK/60 )
						{
							bExist = true;
							// 开盘分割点数据
							CTime Time(pAxisNode[iPos].m_iTimeId);
							CAxisDivide &AxisDivide = pAxisDivide[iIndexDivide];
							AxisDivide.m_iPosPixel		= pAxisNode[iPos].m_iCenterPixel;
							
							//
							AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel2;
							if ( 0!=iIndexDivide && (!bMultiDay || bShowHourInMultiDay) )
							{
								// 在有更低等线的情况下显示实线
								AxisDivide.m_eLineStyle = CAxisDivide::ELSSolid;
							}
							else
							{
								AxisDivide.m_eLineStyle = CAxisDivide::ELSDot;
							}
							
							// 在有更低等线的情况下显示文字
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
				// 如果是最后一个收盘, 还要标志最后一个收盘时间	
				int32 tK=IOCTime.m_aOCTimes[j+1].GetTime();//TimeClose.GetTime();
				bool32 bExist = false;
				int32 iPos = iOCPos;
				for ( ; iPos < iIndexNode ; ++iPos )
				{
					if ( pAxisNode[iPos].m_iTimeId/60 == tK/60 )
					{
						bExist = true;
						// 收盘结束点数据
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

			// 小分割点数据
			if ( !bMultiDay )
			{
				// 半小时数据, 至少要距离开/收盘时间半小时, 可能出现断层点
				for ( int32 tK=IOCTime.m_aOCTimes[j].GetTime()+1800; tK < IOCTime.m_aOCTimes[j+1].GetTime() ; tK+=1800 )
				{
					if ( tK < TimeOpen.GetTime() )
					{
						continue;	// 不能插入在显示区段外的
					}
					if ( tK > TimeClose.GetTime() )
					{
						break;	// 不能插入显示外的
					}

					int32 tHalfM = tK/60;//(tK/1800)*1800/60;
					bool32 bExist = false;
					int32 iPos = iOCPos;
					for ( ; iPos < iIndexNode ; ++iPos )
					{
						if ( pAxisNode[iPos].m_iTimeId/60 == tHalfM )
						{
							bExist = true;
							// 半小时数据点在这里
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
							break;	// 需要在这个位置插入一个半小时点
						}
					}//for ( ; iPos < iIndexNode ; ++iPos )

					ASSERT( pAxisNode[iPos-1].m_iTimeId/60 <= tHalfM );
				}//for ( int32 tK=TimeOpen.GetTime()+1800; tK <= TimeClose.GetTime()-1740 ; tK+=1800 )
			}//if ( !bMultiDay )
			else if ( bShowHourInMultiDay )
			{
				// 小时数据, 至少要距离开/收盘时间半小时
				for ( int32 tK=IOCTime.m_aOCTimes[j].GetTime()+3600; tK <= IOCTime.m_aOCTimes[j+1].GetTime() ; tK+=3600 )
				{
					if ( tK < TimeOpen.GetTime() )
					{
						continue;	// 不能插入在显示区段外的
					}
					if ( tK > TimeClose.GetTime() )
					{
						break;	// 不能插入显示外的
					}

					int32 tHalfM = tK/60;//(tK/3600)*3600/60;
					bool32 bExist = false;
					int32 iPos = iOCPos;
					for ( ; iPos < iIndexNode ; ++iPos )
					{
						if ( pAxisNode[iPos].m_iTimeId/60 == tHalfM )
						{
							bExist = true;
							// 小时分割点
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

	// 计算Y方向可以容纳多少个刻度
	CRect rect = m_pRegionMain->GetRectCurves();

	CFont* pOldFont = pDC->SelectObject(GetIoViewFontObject(ESFSmall));					
	int32 iHeightText = pDC->GetTextExtent(_T("测试值")).cy;
	pDC->SelectObject(pOldFont);

	// int32 iHeightText = pDC->GetTextExtent(_T("测试值")).cy;
	int32 iHeightSub = iHeightText + 12;
	int32 iNum = rect.Height() / iHeightSub + 1;
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
}

void CIoViewTick::CalcSubRegionYAxis(CChartRegion* pChartRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide)
{									  
	ASSERT(NULL != pChartRegion);
	aYAxisDivide.SetSize(0);
	
	// 计算Y方向可以容纳多少个刻度
	CRect rect = pChartRegion->GetRectCurves();
	int32 iHeightText = pDC->GetTextExtent(_T("测试值")).cy;
	int32 iHeightSub  = iHeightText + 12;
	int32 iNum = rect.Height()/iHeightSub + 1;

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
	
	// 获取纵坐标上几个主要的值
	CChartCurve *pCurveDependent = pChartRegion->GetDependentCurve();
	if (NULL == pCurveDependent)
		return;
	
	float fYMin = 0., fYMax = 0.;
	if (!pCurveDependent->GetYMinMax(fYMin, fYMax))
		return;
	
	// 计算浮点数刻度长度.
	float fHeightSub = (fYMax - fYMin) / iNum;
	int32 y;
	float fStart = fYMin;
	int32 iCount = 0;
	aYAxisDivide.SetSize(iNum + 1);
	CAxisDivide *pAxisDivide = (CAxisDivide *)aYAxisDivide.GetData();
	
	// 是否用算整数坐标
	bool32 bTran = false;
	
// 	if ( 0 == iSaveDec && 3 == iNum)
// 	{
// 		// 是整数,且范围够大,做如下处理:
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
// 					// 保证最小值合法
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
// 				// 保证最大值合法
// 				aiYAxis[3] = iMax;
// 			}
// 			
// 			// 赋值:
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
// 				// 格线
// 				AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;		// 
// 				AxisDivide.m_eLineStyle = CAxisDivide::ELSDot;			// 实线
// 				
// 				// 文字
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
			
			AxisDivide.m_DivideText1.m_uiTextAlign		= DT_VCENTER | DT_RIGHT | DT_SINGLELINE;
			AxisDivide.m_DivideText2.m_uiTextAlign		= DT_VCENTER | DT_LEFT | DT_SINGLELINE;
			
			// zhangbo 20090927 #暂时写死颜色
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
	// 取主图信息
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
		return;	// 非缩放状态下不能平移
	}
	
	// 取主图信息
	T_MerchNodeUserData *pMainData = m_MerchParamArray.GetSize()>0?m_MerchParamArray[0]:NULL;
	if ( NULL == pMainData || NULL == m_pRegionMain )
	{
		return;
	}

	if ( m_pRegionMain->m_aXAxisNodes.GetSize() <= 0 )
	{
		return;
	}

	// 坐标点的移动与数据无关
	// 找到当前坐标点在alltime中的位置，至少移动1秒
	int32 iStartInAll;	// 开始位置
	int32 iNodeCount = m_pRegionMain->m_aXAxisNodes.GetSize();
	ASSERT( iNodeCount < m_aAllXTimeIds.GetSize() );	// 缩放状态下必小于
	if ( !FindPosInAllTimeIds(m_TimeZoomStart.GetTime(), iStartInAll) )
	{
		ASSERT( 0 );	// 无法定位开始位置
		return;
	}
	if ( m_aAllXTimeIds[iStartInAll+iNodeCount-1] != m_TimeZoomEnd.GetTime() )
	{
		// 开始处有同秒数据，利用尾部数据进行定位，如果尾部数据也定位失败，则以开始的为准
		int32 iEndInAll;
		if ( !FindPosInAllTimeIds(m_TimeZoomEnd.GetTime(), iEndInAll) )
		{
			// 无法定位位置
			return;
		}
		if ( m_aAllXTimeIds[iEndInAll-iNodeCount+1] == m_TimeZoomStart.GetTime() )
		{
			// 头尾匹配，比较接近事实了吧
			iStartInAll = iEndInAll-iNodeCount+1;
		}
	}
	
	// 
	if ( iStartInAll <= 0 )	// 到最左边了， 不需要请求数据， 除非用户按上下键放缩
	{
		ASSERT( iStartInAll==0 );
		return;
	}
	else
	{	
		// 根据showPos指示，在alltime里面找到前面的时间点，按照若干个时间点移动
		// 至少移动一秒钟
		int32 iMove = iRepCnt * 5;	// 移动的时间点数
		int32 iShowPosInFullList = iStartInAll - iMove;	// 右侧移动
		int32 iStartTime = m_TimeZoomStart.GetTime();
		// 调整最右边的数据
		if ( iShowPosInFullList < 0 )
		{
			iShowPosInFullList	=	0;
		}
		else
		{
			// 保证要至少移动一秒，如果实在没有，那就多跳n条数据吧，应该n不会超过10多条
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
		
		// 同秒数据会引发误差
		m_TimeZoomStart = CGmtTime(m_aAllXTimeIds[iShowPosInFullList]);
		m_TimeZoomEnd	= CGmtTime(m_aAllXTimeIds[iShowPosInFullList+iNodeCount-1]);
		
		OnVDataMerchTimeSalesUpdate(m_pMerchXml);
		
		// zhangbo 20090720 #待补充， 叠加商品数据更新
		//...
		
		// 重新计算坐标
		UpdateAxisSize();
		
		// 显示刷新
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
// 	// Home 键.跳到第一根K 线
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

	// 增加显示线
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

	// 减少显示线
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

		// 判断一下iNodeCross 是否在非法区域: 最右边空白没数据的地方
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
			
			// 引发向左漫游:
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
	// 取主图信息
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
		return;	// 非缩放状态下不能平移
	}
	
	// 取主图信息
	T_MerchNodeUserData *pMainData = m_MerchParamArray.GetSize()>0?m_MerchParamArray[0]:NULL;
	if ( NULL == pMainData || NULL == m_pRegionMain )
	{
		return;
	}

	if ( m_pRegionMain->m_aXAxisNodes.GetSize() <= 0 )
	{
		return;
	}

	// 坐标点的移动与数据无关
	// 找到当前坐标点在alltime中的位置，至少移动1秒
	int32 iStartInAll;	// 开始位置
	int32 iNodeCount = m_pRegionMain->m_aXAxisNodes.GetSize();
	ASSERT( iNodeCount < m_aAllXTimeIds.GetSize() );	// 缩放状态下必小于
	if ( !FindPosInAllTimeIds(m_TimeZoomStart.GetTime(), iStartInAll) )
	{
		ASSERT( 0 );	// 无法定位开始位置
		return;
	}
	if ( m_aAllXTimeIds[iStartInAll+iNodeCount-1] != m_TimeZoomEnd.GetTime() )
	{
		// 开始处有同秒数据，利用尾部数据进行定位，如果尾部数据也定位失败，则以开始的为准
		int32 iEndInAll;
		if ( !FindPosInAllTimeIds(m_TimeZoomEnd.GetTime(), iEndInAll) )
		{
			// 无法定位位置
			return;
		}
		if ( m_aAllXTimeIds[iEndInAll-iNodeCount+1] == m_TimeZoomStart.GetTime() )
		{
			// 头尾匹配，比较接近事实了吧
			iStartInAll = iEndInAll-iNodeCount+1;
		}
	}
	
	// 
	if ( iStartInAll+iNodeCount >= m_aAllXTimeIds.GetSize() )	// 到最右边了， 不需要请求数据， 除非用户按上下键放缩
	{
		ASSERT( iStartInAll+iNodeCount == m_aAllXTimeIds.GetSize() );
		return;
	}
	else
	{	
		// 根据showPos指示，在alltime里面找到前面的时间点，按照若干个时间点移动
		// 至少移动一秒钟
		int32 iMove = iRepCnt * 5;	// 移动的时间点数
		int32 iShowPosInFullList = iStartInAll + iMove;	// 右侧移动
		int32 iStartTime = m_TimeZoomStart.GetTime();
		// 调整最右边的数据
		if ( iShowPosInFullList+iNodeCount > m_aAllXTimeIds.GetSize() )
		{
			iShowPosInFullList	=	m_aAllXTimeIds.GetSize() - iNodeCount;
		}
		else
		{
			// 保证要至少移动一秒，如果实在没有，那就多跳n条数据吧，应该n不会超过10多条
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
		
		// 同秒数据会引发误差
		m_TimeZoomStart = CGmtTime(m_aAllXTimeIds[iShowPosInFullList]);
		m_TimeZoomEnd	= CGmtTime(m_aAllXTimeIds[iShowPosInFullList+iNodeCount-1]);
		
		OnVDataMerchTimeSalesUpdate(m_pMerchXml);
		
		// zhangbo 20090720 #待补充， 叠加商品数据更新
		//...
		
		// 重新计算坐标
		UpdateAxisSize();
		
		// 显示刷新
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
			
			// 引发向右漫游:
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

// 缩小显示单位， 显示多一些K线
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
		return false;	// 非缩放状态下不用缩小了
	}
	
	// 如果有十字光标，则以当前十字光标为中线两边等比例放缩
	// 如果没有十字光标，则以当前显示的最后一个(不论是否有效)数据点作为最右边的线，对左边进行放缩
	int32 iDepPos = m_pRegionMain->m_aXAxisNodes.GetUpperBound();
	const int32 iNodeSize = m_pRegionMain->m_aXAxisNodes.GetSize();
	if ( iNodeSize >= m_aAllXTimeIds.GetSize() )
	{
		ASSERT( iNodeSize == m_aAllXTimeIds.GetSize() );
		//return false;	// 无法继续放大了
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

		// 需要保证FullKline与AllTimeId始终是一致的, 怎样保证呢
		int32 iCrossPos = m_pRegionMain->m_iNodeCross;
		int32 iTimeId = m_pRegionMain->m_aXAxisNodes[iCrossPos].m_iTimeId;
		int32 iPosInAll;
		if ( !FindPosInAllTimeIds(iTimeId, iPosInAll) )
		{
			return false;
		}
		ASSERT( m_aAllXTimeIds[iPosInAll] == iTimeId );
		
		// 两边按照比例增加部分数据的显示
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
			CancelZoom(); // 相当于取消缩放了
		}
		else
		{
			m_TimeZoomStart = m_aAllXTimeIds[iNewStartPos];
			m_TimeZoomEnd   = m_aAllXTimeIds[iNewEndPos];
		}
		
		int32 iNewCrossPos = iCrossPos + (iNewSize-iNodeSize-iRightAdd) ;
		ASSERT( iNewCrossPos >=0 && iNewCrossPos <iNewSize );
		
		// 数据必须重新计算&显示
		// 经过update与redraw，AllTimeId与xnodes可能都已经改变，不能仅依靠上次计算的位置来确定实际的点在哪里
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

		// 此时根据上次计算出来的iNewCrossPos可能并不是现在有效的值了，需要做矫正
		// 后面尽量不要使用前面计算出来的结果
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
			iNewCrossPos = iHalf;	// 此时获得是一个等于或者接近指定时间的值
			if ( iNewCrossPos <0
				|| iNewCrossPos >= m_pRegionMain->m_aXAxisNodes.GetSize() )
			{
				ASSERT( 0 );	// 无法获得有效值
				return false;
			}
			CGmtTime cgTmpTime(m_pRegionMain->m_aXAxisNodes[iNewCrossPos].m_iTimeId);
			TRACE(_T("增加tick cross矫正到: %s\n")
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

			TRACE(_T("数据增加: %s->%s   %s\n")
				, Time2String(cgTime0, ENTIMinute).GetBuffer()
				, Time2String(cgTime1, ENTIMinute).GetBuffer()
				, Time2String(cgTime2, ENTIMinute).GetBuffer());
		}
#endif

		// 重置cross, Y不重置 TODO
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
		
		// 左边添加数据
		int32 iNewStartPos = iPosInAll - (iNewSize-1);
		if ( iNewStartPos < 0 )
		{
			// 左边不够，往右边弹
			iPosInAll -= iNewStartPos;
			iNewStartPos -= iNewStartPos;
		}
		ASSERT( iPosInAll-iNewStartPos+1 == iNewSize );
		ASSERT( iNewStartPos <= iPosInAll && iPosInAll <= m_aAllXTimeIds.GetUpperBound() );
		if ( iNewSize >= m_aAllXTimeIds.GetSize() )
		{
			ASSERT( iNewSize == m_aAllXTimeIds.GetSize() );
			CancelZoom(); // 相当于取消缩放了
		}
		else
		{
			m_TimeZoomStart = m_aAllXTimeIds[iNewStartPos];
			m_TimeZoomEnd	= m_aAllXTimeIds[iPosInAll];
		}
		
		// 数据必须重新计算&显示
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

// 显示单位放大， 显示少一些K线
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
	// 如果有十字光标，则以当前十字光标为中线两边等比例放缩
	// 如果没有十字光标，则以当前显示的最后一个(不论是否有效)数据点作为最右边的线，对左边进行放缩
	int32 iDepPos = m_pRegionMain->m_aXAxisNodes.GetUpperBound();
	const int32 iNodeSize = m_pRegionMain->m_aXAxisNodes.GetSize();
	const int32 KIMinNodeSize = 20;
	if ( iNodeSize <= KIMinNodeSize )
	{
		return false;	// 无法继续缩小了
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

		// 两边按照比例剔除部分数据的显示
		float fRightRatio = (iNodeSize-iCrossPos)/(float)iNodeSize;
		int32 iRightDel = (int32)((iNodeSize-iNewSize)*fRightRatio);
		int32 iNewEndPos = iNodeSize-1-iRightDel;
		iNewEndPos = max(iNewEndPos, iCrossPos);	// 右边
		int32 iNewStartPos = iNewEndPos-iNewSize+1;
		ASSERT( iNewStartPos <= iCrossPos && iCrossPos <= iNewEndPos );

		m_TimeZoomStart = CGmtTime(m_pRegionMain->m_aXAxisNodes[iNewStartPos].m_iTimeId);
		m_TimeZoomEnd   = CGmtTime(m_pRegionMain->m_aXAxisNodes[iNewEndPos].m_iTimeId);

		int32 iNewCrossPos = iCrossPos-iNewStartPos;
		ASSERT( iNewCrossPos >=0 && iNewCrossPos <iNewSize );
		
		// 数据必须重新计算&显示
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

		// 此时根据上次计算出来的iNewCrossPos可能并不是现在有效的值了，需要做矫正
		// 后面尽量不要使用前面计算出来的结果
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
			iNewCrossPos = iHalf;	// 此时获得是一个等于或者接近指定时间的值
			if ( iNewCrossPos <0
				|| iNewCrossPos >= m_pRegionMain->m_aXAxisNodes.GetSize() )
			{
				ASSERT( 0 );	// 无法获得有效值
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
			TRACE(_T("数据减少: %s->%s\n"), Time2String(cgTime1, ENTIMinute).GetBuffer(), Time2String(cgTime2, ENTIMinute).GetBuffer());
		}
#endif

		// 重置cross, Y不重置 TODO
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

		// 左边剔除所有需要剔除的数据
		int32 iNewStartPos = iDepPos -(iNewSize-1);
		ASSERT( iDepPos-iNewStartPos+1 == iNewSize );
		m_TimeZoomStart = CGmtTime(m_pRegionMain->m_aXAxisNodes[iNewStartPos].m_iTimeId);
		m_TimeZoomEnd   = CGmtTime(m_pRegionMain->m_aXAxisNodes[iDepPos].m_iTimeId);
		
		// 数据必须重新计算&显示
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

	// 初始化时间 -> [o, c]...[o,c] ->结束时间
	// 按照分段取数据, 第一段为 初始化->第一个开收的收 最后一段为最后一个开收的开到结束时间
	// 从最后一段发请求直至第一段
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

		TRACE(_T("闪电ReqStart: %s->%s\n")
			, Time2String(reqTick.m_TimeStart, ENTIMinute, true, false).GetBuffer()
				, Time2String(reqTick.m_TimeEnd, ENTIMinute, true, false).GetBuffer());

		if ( ERTYFrontCount != reqTick.m_eReqTimeType )
		{
			reqTick.m_eReqTimeType	= ERTYFrontCount;
			reqTick.m_TimeSpecify	= m_MmiReqTimeSales.m_TimeEnd;
			reqTick.m_iFrontCount	= 50;
			DoRequestViewData(reqTick);
		}

		// 避免无数据时回应慢？额外发送一个数据量的请求
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
		return;	// 不是同一个商品~~
	}

	// 由于并不知道上一个发出去的时间段内数据请求有没有回来或者根本就是0数据回来
	// 所以只能不管上一个结果，继续发送下一个，如果数据请求被过滤会调用forceupdate的

	// 初始化时间 -> [o, c]...[o,c] ->结束时间
	// 按照分段取数据, 第一段为 初始化->第一个开收的收 最后一段为最后一个开收的开到结束时间
	// 从最后一段发请求直至第一段
	m_MmiReqTimeSales.m_TimeEnd			= m_MmiReqTimeSales.m_TimeStart;	// start必为开盘或者初始化时间
	int32 i = 0;
	for ( i=0; i < m_aTickMultiDayIOCTimes.GetSize() ; ++i )
	{
		const CMarketIOCTimeInfo &IOCTime = m_aTickMultiDayIOCTimes[i];
		if ( m_MmiReqTimeSales.m_TimeEnd <= IOCTime.m_TimeOpen.m_Time )
		{
			continue;	// <=这日最初开盘时间，则必然是要请求前面的数据
		}

		m_MmiReqTimeSales.m_TimeStart = IOCTime.m_TimeInit.m_Time;	// 开始时间

		int32 iSize = IOCTime.m_aOCTimes.GetSize();
		for ( int32 j=iSize-2; j > 1 ; j-=2 )	// 第一个开收取初始化时间, 不比较
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
		return;	// 没有数据要请求了
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
			TRACE(_T("闪电ReqNext: %s->%s\n")
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

		// 需要请求n+1条日k线 && 已初始化好的时间段内的tick数据
		// 分节奏请求 TODO
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

			DoRequestViewData(reqKline);	// 请求n日k线, 过滤 TODO
		}
		
		// 请求实时数据
		CMmiReqRealtimePrice MmiReqRealtimePrice;
		MmiReqRealtimePrice.m_iMarketId		= pData->m_pMerchNode->m_MerchInfo.m_iMarketId;
		MmiReqRealtimePrice.m_StrMerchCode	= pData->m_pMerchNode->m_MerchInfo.m_StrMerchCode;		
		DoRequestViewData(MmiReqRealtimePrice);
	}

	// 当前需要请求多少日的数据，从没有的地方开始请求若干条，等待数据回来接着请求
	RequestSequenceTickStart();
}

bool32 CIoViewTick::GetChartXAxisSliderText1( OUT CString &StrSlider, CAxisNode &AxisNode )
{
	StrSlider.Empty();
	// 
	if (m_MerchParamArray.GetSize() <= 0)
	{
		// 可能存在新建的K 线视图,没有数据.
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
	else	// 不应该出现的类型
	{
		return false;
	}

	if ( NULL == m_pRegionMain )
	{
		// 不应该在RegionMain为空时就调用到了的 或者其它的情况
		return false;
	}

	int32 iTimeId = AxisNode.m_iTimeId;

	if ( 0 == iTimeId )
	{
		// 根据X坐标查找
		int32	iRegionX = AxisNode.m_iCenterPixel;
		m_pRegionMain->ClientXToRegion(iRegionX);
		if( NULL == m_pRegionMain->GetDependentCurve()
			|| ! m_pRegionMain->GetDependentCurve()->RegionXToCurveID(iRegionX, iTimeId) )
		{
			// 不应该在RegionMain为空时就调用到了的 或者其它的情况 - 有可能X ID会失败
			return false;
		}
	}
	
	CTime Time(iTimeId);		// 总是当地时间显示
	
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
		CancelZoom();	// 总是要清除
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
			m_aTickMultiDayIOCTimes.SetSize(m_iTickMultiDay);	// 截断多余的数据
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
	// 获取日K线数据
	if ( pMerch != m_pMerchXml
		|| NULL == m_pMerchXml )
	{
		return;
	}

	// 尝试更新走势相关的报价数据

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
					// zhangbo 20090708 #待补充, 更新中轴值
					//...		
					ReDrawAysnc();
				}
			}
			break;
		}
	}
	

	if ( m_iTickMultiDay == m_aTickMultiDayIOCTimes.GetSize() )
	{
		return;	// 所有日k线已经返回，无需额外从这里处理
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
	// 实际有可能出现在初始化时没有该日k线生成，所以只能假设所有日k线都是连续的，最新日k线要不就没出现，出现则必然在最后一根
	//int32 iPosToday = CMerchKLineNode::QuickFindKLineByTime(pDayKline->m_KLines, TimeToday);
	int32 iPosToday = CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(pDayKline->m_KLines, TimeToday);
	if ( iPosToday < m_aTickMultiDayIOCTimes.GetSize()-1
		|| iPosToday >= pDayKline->m_KLines.GetSize() )
	{
		return;// 没有多余的天数或者没有找到
	}

	if ( pDayKline->m_KLines[iPosToday].m_TimeCurrent < TimeToday )
	{
		// 最后一根线不是今日线，则认为最后一根是昨日线，将iPosToday+1
		++iPosToday;	// 此时iPosToday实际是一个无效位置
	}

	// 假设所有日K线都是连续的，最新时间在最后面
	// tick多日里面存储的最新日期在最前面
	int32 iPosLast = iPosToday-m_aTickMultiDayIOCTimes.GetSize();		// 最后的一把数据量
	for ( i=iPosLast; i >= 0 && m_aTickMultiDayIOCTimes.GetSize() < m_iTickMultiDay ; --i )
	{
		const CKLine &kline = pDayKline->m_KLines[i];
		CGmtTime TimeDay = kline.m_TimeCurrent;
		SaveDay(TimeDay);
		CMarketIOCTimeInfo IOCTime;
		if ( m_pMerchXml->m_Market.GetSpecialTradingDayTime(TimeDay, IOCTime, m_pMerchXml->m_MerchInfo) )
		{
			TRACE(_T("添加tick日k线: %s\n"), Time2String(TimeDay, ENTIMinute, true, false).GetBuffer());
			m_aTickMultiDayIOCTimes.Add(IOCTime);
		}
		else
		{
			ASSERT( 0 );
		}
	}

	OnVDataMerchTimeSalesUpdate(pMerch);	// 重新计算分笔数据
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

	const CNodeData *pSrcData = aSrc.GetData();	// 可能为NULL
	const int32		iSrcDataCount = aSrc.GetSize();

	CGmtTime TimeFirstNeedAddNode(TimeStart);		// 第一个需要添加的时间
	if ( iSrcDataCount > 0 )
	{
		TimeFirstNeedAddNode = CGmtTime(pSrcData[iSrcDataCount-1].m_iID) + CGmtTimeSpan(0,0,1,0);
		SaveMinute(TimeFirstNeedAddNode);
	}

	// 添加要添加的尾部数据, 由于是按照一分钟一个点来处理，所以必然会添加需要的分割点
	// 从时间开始到结束[早->晚]
	for ( int32 i=m_aTickMultiDayIOCTimes.GetUpperBound(); i >=0 ; --i )
	{
		const CMarketIOCTimeInfo &IOCTime = m_aTickMultiDayIOCTimes[i];
		if ( IOCTime.m_TimeEnd.m_Time < TimeFirstNeedAddNode )
		{
			ASSERT( aNewNodes.GetSize() == iSrcDataCount );	// 处于应用前
			continue;	// 这天的结束时间比需要显示开始小，不必显示这天
		}
		if ( TimeEnd < IOCTime.m_TimeInit.m_Time )
		{
			break;	// 这天的开始时间比显示结束大，不必继续显示了
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

		// 换天
		if ( iDayPos > 0 
			&& iDayPos < aNewNodes.GetSize()
			&& aNewNodes[iDayPos-1].m_iID/(3600*24) != aNewNodes[iDayPos].m_iID/(3600*24) )
		{
			aNewNodes[iDayPos-1].m_iFlag |= CNodeData::KValueDivide;
			CGmtTime cPreTime(aNewNodes[iDayPos-1].m_iID);
			CGmtTime cCurTime(aNewNodes[iDayPos].m_iID);
			TRACE(_T("天数分割线: %s->%s\n")
				, Time2String(cPreTime, ENTIMinute, true, false).GetBuffer()
				, Time2String(cCurTime, ENTIMinute, true, false).GetBuffer());
		}
	}//for ( int32 i=m_aTickMultiDayIOCTimes.GetUpperBound(); i >=0 ; --i )
}

void CIoViewTick::CalcKLineAvgValue( INOUT CArray<CKLine, CKLine> &aKlines )
{
	// 传入的应当是所有得到的数据
	const int32 iKlineCount = aKlines.GetSize();
	CKLine *pKlines	= aKlines.GetData();	// 可能为NULL
	float fAvgSum	= 0.0f;
	int32 iAvgCount = 0;
	int32 tTimeLastDay = 0;	// 换天应当重新计算avg
	ASSERT( NULL != m_pMerchXml );
	for ( int32 i=0; i < iKlineCount ; ++i )
	{
		if ( pKlines[i].m_TimeCurrent.GetTime()/(3600*24) != tTimeLastDay )
		{
			// 换天，数据重新计算
			fAvgSum		= 0.0f;
			iAvgCount	= 0;
			tTimeLastDay = pKlines[i].m_TimeCurrent.GetTime()/(3600*24);
		}
		
		fAvgSum += pKlines[i].m_fPriceClose;
		++iAvgCount;
		pKlines[i].m_fPriceAvg = fAvgSum/iAvgCount;

		// 取savedec的有效位数
	}
}

void CIoViewTick::FillMainVirtualKLineData(const CArray<CKLine, CKLine> &aSrc, OUT CArray<CKLine, CKLine> &aDst, OUT CArray<DWORD, DWORD> &aDstFlag)
{
	// 往fullData中填充n多虚假数据
	// 不填充尾部数据
	CGmtTime TimeStart, TimeEnd;
	GetTickMultiDayTimeRange(TimeStart, TimeEnd);

	aDst.SetSize(0, 1500);
	aDstFlag.SetSize(0, 1500);

	int32 iShowPosStart = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(aSrc, TimeStart);
	int32 iShowPosEnd	= CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(aSrc, TimeEnd);
	int32 iSrcDataCount = 0;
	const CKLine *pSrcData = aSrc.GetData();	// 可能为NULL
	if ( iShowPosStart >=0 && iShowPosEnd>=iShowPosEnd && iShowPosEnd < aSrc.GetSize() )
	{
		iSrcDataCount = iShowPosEnd-iShowPosStart+1;
	}
	else
	{
		// 无数据
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
			ASSERT( 0 == iIndex && aDst.GetSize()==0 );	// 处于应用前
			continue;	// 这天的结束时间比显示开始小，不必显示这天
		}
		if ( TimeEnd < IOCTime.m_TimeInit.m_Time )
		{
			break;	// 这天的开始时间比显示结束大，不必继续显示了
		}

		int32 iDayPos = aDst.GetSize();		// 新天的数据的起点
		
		// 在坐标轴位置必须有点
		// 如果这整个开收盘内分笔数据总数小于分钟线数，则填充分钟点，凑足数量
		for ( int32 j=0; j <= IOCTime.m_aOCTimes.GetSize()-2 ; j+=2 )
		{
			// 开收 小时 半小时
			CGmtTime TimeOpen	= IOCTime.m_aOCTimes[j];
			CGmtTime TimeClose	= IOCTime.m_aOCTimes[j+1];

			// 夸大实际开收盘时间，来包括更多的实际点
			if ( 0 == j )
			{
				TimeOpen = IOCTime.m_TimeInit.m_Time;	// 第一个开盘以市场初始化时间为开始
			}
			else if ( IOCTime.m_aOCTimes.GetSize()-2==j )
			{
				TimeClose = IOCTime.m_TimeEnd.m_Time;	// 最后一个收盘以结束时间为结束
			}
			TimeOpen	= max(TimeOpen, TimeStart);
			SaveMinute(TimeOpen);
			TimeClose	= min(TimeClose, TimeEnd);
			SaveMinute(TimeClose);
			TimeClose += CGmtTimeSpan(0, 0, 0, 59);	// 精确度只能到这里了
			//int32	 iMinuteCount = (TimeClose.GetTime()-TimeOpen.GetTime())/60 +1;

			int32 iOCPos = aDst.GetSize();	// 新开盘数据的起点
			
			int32 iPosOpen = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(aSrc, TimeOpen);
			int32 iPosClose = CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(aSrc, TimeClose);
			bool32 bNeedFill = iPosOpen>=0&&iPosClose>=iPosOpen ? ((iPosClose-iPosOpen+1) <= 0) : true;

			// 还原实际存在的开收盘时间, 用来计算分割点
			TimeOpen	= max(IOCTime.m_aOCTimes[j], TimeStart);
			SaveMinute(TimeOpen);
			TimeClose	= min(IOCTime.m_aOCTimes[j+1], TimeEnd);
			SaveMinute(TimeClose);
			TimeClose += CGmtTimeSpan(0, 0, 0, 59);	// 精确度只能到这里了

			if ( bNeedFill )
			{
				// 每分钟至少一个点, 保证了即使最小分割点仍然有至少一个点
				// 现在整段开盘无数据，数据不连续
				DWORD dwFlag = iPosClose-iPosOpen+1 <=0 ? CNodeData::KValueInvalid : CNodeData::KValueInhert|CNodeData::KValueInvalid;
				for ( int32 tK=TimeOpen.GetTime(); tK < TimeClose.GetTime() ; tK+=60 )
				{
					bool32 bAdded = false;

					if(pSrcData)
					{
						// 添加属于本次分钟点前的任何有效数据
						for ( 
							; iIndex < iSrcDataCount && (pSrcData[iIndex].m_TimeCurrent.GetTime()<tK)
							; ++iIndex)
						{
							// 实际数据
							aDst.Add(pSrcData[iIndex]);
							aDstFlag.Add(0);
						}

						// 填充属于本分钟点的数据
						for ( 
							; iIndex < iSrcDataCount && (pSrcData[iIndex].m_TimeCurrent.GetTime()>=tK&& pSrcData[iIndex].m_TimeCurrent.GetTime()<tK+60)
							; ++iIndex)
						{
							// 实际数据
							aDst.Add(pSrcData[iIndex]);
							aDstFlag.Add(0);
							bAdded = true;
						}
						if ( !bAdded )
						{
							// 虚假的点
							CKLine kline;
							kline.m_TimeCurrent = CGmtTime(tK);
							aDst.Add(kline);
							aDstFlag.Add(dwFlag);
						}
					}
					// 分钟点后的数据留给下个分钟点或者最后添加
				}
			}// if ( bNeedFill )
			else
			{
				// 照这数据填点就可以了

				// 填充属于本开收段的数据, 小于本次收盘时间的都属于这个开收段
				if(pSrcData)
				{
					for ( 
						; iIndex < iSrcDataCount && (pSrcData[iIndex].m_TimeCurrent<TimeClose)
						; ++iIndex)
					{
						// 实际数据
						aDst.Add(pSrcData[iIndex]);
						aDstFlag.Add(0);
					}
				}


				// 最后尾部没有数据的位置需要按照1分钟一个点来填充
				// 最后的点必然不能连续
				if ( iIndex >= iSrcDataCount )
				{
					ASSERT( iSrcDataCount >0 && pSrcData != NULL );	// 能进入这条分支则必然有数据
					CGmtTime TimeLastData = pSrcData[iIndex-1].m_TimeCurrent;
					TimeLastData = max(TimeLastData, TimeOpen);
					SaveMinute(TimeLastData);
					TimeLastData += CGmtTimeSpan(0,0,1,0);
					for ( int32 tK=TimeLastData.GetTime(); tK < TimeClose.GetTime() ; tK+=60 )
					{	
						// 虚假的点
						CKLine kline;
						kline.m_TimeCurrent = CGmtTime(tK);
						aDst.Add(kline);
						aDstFlag.Add(CNodeData::KValueInvalid);	
					}
				}
			}// if ( bNeedFill ) else

			// 产生分割点数据
			// 如果这些分割点数据后面没有实际的数据了，则点不能继承上一个有效点
			if ( !bMultiDay )
			{
				// 半小时数据, 至少要距离开/收盘时间半小时, 可能出现断层点
				// 认为距离开盘30分钟的半小时线
				int32 iPos = iOCPos;
				for ( int32 tK=IOCTime.m_aOCTimes[j].GetTime()+1800; tK < IOCTime.m_aOCTimes[j+1].GetTime(); tK+=1800 )
				{
					if ( tK < TimeOpen.GetTime() )
					{
						continue;	// 不能插入在显示区段外的
					}
					if ( tK > TimeClose.GetTime() )
					{
						break;	// 不能插入显示外的
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
							break;	// 需要在这个位置插入一个半小时点
						}
					}
					if ( !bExist )
					{
						ASSERT( iPos < aDst.GetSize() );
						// 虚假的点
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
				// 小时数据, 至少要距离开/收盘时间半小时
				// 认为距离开盘为60分钟的为小时线
				int32 iPos = iOCPos;
				for ( int32 tK=IOCTime.m_aOCTimes[j].GetTime()+3600; tK < IOCTime.m_aOCTimes[j+1].GetTime() ; tK+=3600 )
				{
					if ( tK < TimeOpen.GetTime() )
					{
						continue;	// 不能插入在显示区段外的
					}
					if ( tK > TimeClose.GetTime() )
					{
						break;	// 不能插入显示外的
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
							break;	// 需要在这个位置插入一个小时点
						}
					}
					if ( !bExist )
					{
						ASSERT( iPos < aDst.GetSize() );
						// 虚假的点
						CKLine kline;
						int64 tmpTime = int64(tHalfM*60);
						kline.m_TimeCurrent = CGmtTime((LONGLONG)tmpTime);
						aDst.InsertAt(iPos, kline);
						aDstFlag.InsertAt(iPos, CNodeData::KValueInvalid|CNodeData::KValueInhert);
					}
				}
			}
			
			// 开盘线数据
			if ( j > 0 || !bMultiDay )
			{
				// 多日下第一个开盘被视为天数的分割，不记入开盘分割
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
						break;	// 需要在这个位置插入一个开盘点
					}
				}
				if ( !bExist )
				{
					ASSERT( iPos < aDst.GetSize() );
					// 虚假的点
					CKLine kline;
					kline.m_TimeCurrent = CGmtTime(tK);
					aDst.InsertAt(iPos, kline);
					aDstFlag.InsertAt(iPos, CNodeData::KValueInvalid|CNodeData::KValueInhert);
				}
			}

			if ( (0 == i && IOCTime.m_aOCTimes.GetSize()-1 == j+1) )
			{
				// 如果是最后一个收盘，还要标志最后一个收盘
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
						break;	// 需要在这个位置插入一个开盘点
					}
				}
				if ( !bExist )
				{
					// 最后一个收盘点有可能并没有相应的数据点
					ASSERT( iPos <= aDst.GetSize() );
					// 虚假的点
					CKLine kline;
					kline.m_TimeCurrent = CGmtTime(tK);
					aDst.InsertAt(iPos, kline);
					aDstFlag.InsertAt(iPos, CNodeData::KValueInvalid|CNodeData::KValueInhert);
				}
			}
		}//for ( int32 j=0; j < IOCTime.m_aOCTimes.GetSize()-2 ; j+=2 )

		// 添加收盘后->结束时间的有效数据
		// 添加属于本天前的任何有效数据
		CGmtTime TimeLastEnd(IOCTime.m_TimeEnd.m_Time);
		TimeLastEnd = min(TimeLastEnd, TimeEnd);
		if(pSrcData)
		{
			for ( 
				; iIndex < iSrcDataCount && (pSrcData[iIndex].m_TimeCurrent<TimeLastEnd)
				; ++iIndex)
			{
				// 实际数据
				aDst.Add(pSrcData[iIndex]);
				aDstFlag.Add(0);
			}
		}

		// 新天的天分割线数据 - 该天只要有开收盘信息则必然有该天的第一个数据起点
		ASSERT( iDayPos < aDst.GetSize() && aDst.GetSize()==aDstFlag.GetSize() );
		if ( iDayPos > 0 )
		{
			aDstFlag[iDayPos-1] |= CNodeData::KValueDivide; // 处理这个标志与datainvalid的冲突 TODO
// 			TRACE(_T("天数分割线: %s->%s, 均价%f-%f\n")
// 				, Time2String(aDst[iDayPos-1].m_TimeCurrent, ENTIMinute, true, false)
// 				, Time2String(aDst[iDayPos].m_TimeCurrent, ENTIMinute, true, false)
// 				, aDst[iDayPos-1].m_fPriceAvg, aDst[iDayPos].m_fPriceAvg);
		}
		else
		{
// 			TRACE(_T("忽略天数分割: ->%s\n")
// 				, Time2String(aDst[iDayPos].m_TimeCurrent, ENTIMinute, true, false)); 
		}
		// 每天的新数据要断开线的连接
	}//for ( int32 i=m_aTickMultiDayIOCTimes.GetUpperBound(); i >=0 ; --i )

	ASSERT( aDst.GetSize()==aDstFlag.GetSize() /*&& aDst.GetSize() >= aSrc.GetSize()*/ );

	// 保存当前所有的数据到所有x轴时间节点中
	m_aAllXTimeIds.SetSize(0, aDst.GetSize());
	for ( i=0; i < aDst.GetSize() ; ++i )
	{
		m_aAllXTimeIds.Add(aDst[i].m_TimeCurrent.GetTime());
	}

	// 剔除掉尾部附加的多余数据，这些多余坐标点会在calcx时自动补足
	for ( int32 iDelEnd=aDstFlag.GetUpperBound(); iDelEnd >=0; --iDelEnd )
	{
		if ( !CheckFlag(aDstFlag[iDelEnd], CNodeData::KValueInvalid) )
		{
			break;
		}
		aDstFlag.RemoveAt(iDelEnd);
		aDst.RemoveAt(iDelEnd);
	}

	// 分割点，数据连续等的处理

	// 分割点处理
	// 单日:
	//		30分钟一条dot分割线(距离开盘30分钟), 半场线实线, 显示30分钟时间&半场
	// 多日:
	//		<=3日，60分钟一条dot分割线(距离开盘60分钟), 半场实线, 天实, 显示半场&天
	//		>3日，半场dot分割线, 一天实, 显示半场&天
	// 如果在分割点时没有数据，则在前后两个点之间的中点作为分割点的位置
	// 连续: 每天的数据必须单独一条线

	// 可以考虑以分割点来判断两分割点之间至少应当有多少点 TODO
}

void CIoViewTick::FillMainVirtualShowData()
{
	// 不要单独调用这个函数
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

	// 建立在Full数据已经计算好了基础上
	
	int32 iShowPosInFullList = 0;
	int32 iShowCountInFullList = 0;
	// 调整显示区间
	int32 iShowPosStart = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pData->m_aKLinesFull, TimeStart);
	int32 iShowPosEnd	= CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(pData->m_aKLinesFull, TimeEnd);
	if (iShowPosStart >= 0 && iShowPosStart < pData->m_aKLinesFull.GetSize()
		&& iShowPosEnd >= iShowPosStart && iShowPosEnd < pData->m_aKLinesFull.GetSize() )
	{
		iShowPosInFullList = iShowPosStart;
		iShowCountInFullList = iShowPosEnd-iShowPosStart+1;
		
		// 填充所有在约定n天内的虚拟数据
		CArray<CKLine, CKLine> aTmp;
		aTmp.Copy(pData->m_aKLinesFull);

		FillMainVirtualKLineData(aTmp, pData->m_aKLinesFull, pData->m_aKLinesShowDataFlag);

		CGmtTime TimeShowStart, TimeShowEnd;
		GetShowTimeRange(TimeShowStart, TimeShowEnd);
		iShowPosStart	= CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pData->m_aKLinesFull, TimeShowStart);
		iShowPosEnd		= CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(pData->m_aKLinesFull, TimeShowEnd);
		// 有可能根本就没有适合显示的k线数据
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
		// 空数据
		ClearLocalData(false);

		CArray<CKLine, CKLine> aTmp;
		FillMainVirtualKLineData(aTmp, pData->m_aKLinesFull, pData->m_aKLinesShowDataFlag);	// 必须用它来初始化所有时间轴信息
		pData->m_aKLinesFull.RemoveAll();
		iShowPosInFullList = 0;
		iShowCountInFullList = 0;
		//iShowCountInFullList = pData->m_aKLinesFull.GetSize();
	}

	// 将标志与实际数据同步
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
	// 更新主图下面的指标
	//bool32 bShowVolColor = IsShowVolBuySellColor();
	for (int32 j = 0; j < pData->aIndexs.GetSize(); j++)
	{
		T_IndexParam* pIndex = pData->aIndexs.GetAt(j);
		g_formula_compute(this, this, (CChartRegion*)pIndex->pRegion, pData, pIndex, iShowPosInFullList, iShowPosInFullList + iShowCountInFullList);

		// 闪电分时，由于Kline数据中不含内外盘数据，so在这里需要处理成交量的红绿信息
		// Full数据与compare数据并不一定严格对等，但是full数据中的实际数据一定在compare中存在
		if ( pIndex->strIndexName == _T("VOLFS") )
		{
			for ( int32 k=iShowPosInFullList; k < iShowPosInFullList+iShowCountInFullList ; ++k )
			{
				
			}
		}
	}
	
	// 生成当前显示的数据
	bool32 bUpdate = pData->UpdateShowData(iShowPosInFullList, iShowCountInFullList);
	// 还原fullk线
	if (!bUpdate)
	{
		return;
	}
}

void CIoViewTick::OnRectZoomOut( int32 iNodeBegin, int32 iNodeEnd )
{
	// 错误情况:
	if (m_MerchParamArray.GetSize() <= 0)
	{
		return;
	}
	
	T_MerchNodeUserData *pMainData = m_MerchParamArray[0];
	if (NULL == pMainData)
	{
		return;
	}

	// iNodeBegin与end表示的点在FullKline中的位置，在tick图中，有可能这两个点都是虚的
	// 根本就不在fullkline中
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
		return;	// 数据太少了
	}
	
	// 设置缩放范围
	m_TimeZoomStart = CGmtTime(m_pRegionMain->m_aXAxisNodes[iNodeBegin].m_iTimeId);
	m_TimeZoomEnd	= CGmtTime(m_pRegionMain->m_aXAxisNodes[iNodeEnd].m_iTimeId);

	OnVDataMerchTimeSalesUpdate(m_pMerchXml);
	
	// 显示刷新
	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	
	for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
	{
		m_SubRegions[i].m_pSubRegionMain->SetDrawFlag(CRegion::KDrawFull);
	}
	
	// 重画视图
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
			// 多日分时名称 商品[最近n日分时] , 此时商品等必不为NULL
			CString StrName;
			StrName.Format(_T("%s【最近%d日】"), m_pMerchXml->m_MerchInfo.m_StrMerchCnName.GetBuffer(), m_aTickMultiDayIOCTimes.GetSize());
			m_pRegionMain->SetTitle(StrName);
		}
		else if ( bValidMerch )
		{
			m_pRegionMain->SetTitle(m_pMerchXml->m_MerchInfo.m_StrMerchCnName + _T(" "));
		}
		else
		{
			m_pRegionMain->SetTitle(_T("闪电图"));
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
	// 有没有0的昨收呢
	// 对于有实时报价但是不是今天的实时报价做特殊处理
	if ( fPriceBase == 0.0f
		&& NULL != pData->m_pMerchNode
		&& NULL != pData->m_pMerchNode->m_pRealtimePrice )
	{
		if (ERTFuturesCn == pData->m_pMerchNode->m_Market.m_MarketInfo.m_eMarketReportType)	// 仅国内期货按昨结算价算
		{
			fPriceBase = pData->m_pMerchNode->m_pRealtimePrice->m_fPricePrevAvg;	// 取昨天的结算
		}
		else
		{
			fPriceBase = pData->m_pMerchNode->m_pRealtimePrice->m_fPricePrevClose;	// 取昨天的收盘
		}
	}
	return fPriceBase;
}

bool32 CIoViewTick::CalcKLinePriceBaseValue( CChartCurve *pCurve, OUT float &fPriceBase )
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
		ASSERT( 0 );	// 不支持其它任何类型
	}
	
	return false;
}

bool32 CIoViewTick::FindPosInAllTimeIds( int32 iTimeId, OUT int32 &iPos )
{
	ASSERT( m_aAllXTimeIds.GetSize() > 0 );
	iPos = -1;
	int32 iLow  = 0;
	int32 iHigh = m_aAllXTimeIds.GetUpperBound();
	// time 小->到
	// 对于那种同一秒内有n比数据的就悲剧了，定位不了
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
