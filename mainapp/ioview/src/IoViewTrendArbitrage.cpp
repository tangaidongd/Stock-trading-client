#include "stdafx.h"

#include <set>

#include "GridCtrlSys.h"
#include "GridCellSys.h"
#include "GridCellSymbol.h"
#include "guidrawlayer.h"
#include "NewMenu.h"
#include "memdc.h"
#include "MerchManager.h"

#include "facescheme.h"
#include "IoViewManager.h"
#include "ShareFun.h"
#include "Region.h"
#include "ChartRegion.h"
#include "CFormularContent.h"
#include "float.h"

#include "ChartDrawer.h"
#include "IoViewShare.h"
#include "dlgtrendindexstatistics.h"
#include "dlgindexprompt.h"
#include "GGTongView.h"

#include "IoViewTrendArbitrage.h"
#include "DlgChooseStockVar.h"

#include "BalloonMsg.h"
#include "sharestructnews.h"
#include "XmlShare.h"

#include "XLTimerTrace.h"
#include <map>

using std::set;
using std::multimap;

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
							//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define _MYABSF(f) (f<0.0?-f:f)

extern void TestKLine(CArray<CKLine,CKLine>& aKLines,const char * pFile,CString StrMerchCode, int32 iMarketId);
extern void TestNodes(CArray<CNodeData,CNodeData&>& aNodes,const char * pFile,CString StrMerchCode, int32 iMarketId);

extern CString MakeMenuString(const CString& StrName, const CString& StrExplain, bool32 bAppendSpace=true);

extern bool32	GetArbitrageMerchs(IN const CArbitrage &arb, OUT CIoViewBase::MerchArray &aMerchs);
extern CString ArbEnumToString(E_Arbitrage eType);

//////////////////////////////////////////////////////////////////////////
const int32 KTimerIdCalcArbitrage		= 10086;		// 计算套利
const int32 KTimerPeriodCalcArbitrage	= 10;

const int32 KDefaultCtrlMoveSpeed		= 5;			// 在Ctrl键按下时，默认平移的单位

// 
const int32 KMaxMinuteKLineCount		= 10000;		// 最大允许的分钟K线数据， 不加限制的话， 连续开着软件会无限增长

// xml
static const char KXMLAttriIoViewTrendSubIndex[]				= "SubIndex";	// 副图指标名称

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CIoViewTrendArbitrage, CIoViewChart)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewTrendArbitrage, CIoViewChart)
	//{{AFX_MSG_MAP(CIoViewTrendArbitrage)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
 	ON_COMMAND_RANGE(IDM_CHART_BEGIN, IDM_CHART_END, OnMenu)
 	ON_COMMAND_RANGE(IDM_IOVIEWKLINE_ALLINDEX_BEGIN, IDM_IOVIEWKLINE_ALLINDEX_END, OnAllIndexMenu)
 	ON_COMMAND_RANGE(IDM_IOVIEWKLINE_OFTENINDEX_BEGIN, IDM_IOVIEWKLINE_OFTENINDEX_END, OnOftenIndexMenu)
 	ON_COMMAND_RANGE(IDM_IOVIEWKLINE_OWNINDEX_BEGIN, IDM_IOVIEWKLINE_OWNINDEX_END, OnOwnIndexMenu)	
 	ON_COMMAND_RANGE(IDT_SD_BEGIN, IDT_SD_END,OnMenu)	
	ON_COMMAND_RANGE(ID_ARBITRAGE_SETTING, ID_ARBITRAGE_SETTING, OnMenu)
	ON_MESSAGE(UM_IoViewTitle_Button_LButtonDown, OnMessageTitleButton)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewTrendArbitrage::CIoViewTrendArbitrage()
:CIoViewChart(),CChartRegionViewParam(NULL)
{
	m_pParent			= NULL;
	m_rectClient		= CRect(-1,-1,-1,-1);
	m_iDataServiceType  = EDSTKLine | EDSTPrice ;
	m_StrText			=_T("IoViewTrendArbitrage");
	m_pRegionViewData	= this;
	m_pRegionDrawNotify = this;
	SetParentWnd(this);
	m_iChartType		= 5;

	m_aAllFormulaNames.RemoveAll();

	//
	memset(&m_KLineCrossNow, 0, sizeof(m_KLineCrossNow));
}

CIoViewTrendArbitrage::~CIoViewTrendArbitrage()
{
	//
	CArbitrageManage::Instance()->DelNotify(this);
}

///////////////////////////////////////////////////////////////////////////////
int CIoViewTrendArbitrage::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CIoViewChart::OnCreate(lpCreateStruct) == -1)
		return -1;

	CArbitrageManage::Instance()->AddNotify(this);
	
	return 0;
}

BOOL CIoViewTrendArbitrage::TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ( NULL != m_pIoViewManager && this != m_pIoViewManager->GetActiveIoView() )
	{
		return FALSE;
	}

// 	if ( nChar == VK_F7 )
// 	{
// 		TestSetArb(TRUE);
// 		return TRUE;
// 	}

	return CIoViewChart::TestKeyDown(nChar,nRepCnt,nFlags);
}

void CIoViewTrendArbitrage::InitCtrlFloat( CGridCtrl* pGridCtrl )
{
	if ( m_bInitialFloat )
	{
		return;
	}
	else
	{
		m_bInitialFloat = true;
	}

	pGridCtrl->SetRowCount(10);

	CGridCellSys *pCell;
	// 	 pCell = (CGridCellSys *)pGridCtrl->GetCell(0, 0);
	// 	 pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	// 	 pCell->SetText(_T(" 时间"));

	pCell = (CGridCellSys *)pGridCtrl->GetCell(2, 0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(_T(" 数值"));

	pCell = (CGridCellSys *)pGridCtrl->GetCell(4, 0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(_T(" 开盘"));

	pCell = (CGridCellSys *)pGridCtrl->GetCell(6, 0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(_T(" 收盘"));

	pCell = (CGridCellSys *)pGridCtrl->GetCell(8, 0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(_T(" 均价"));
	 
	 //
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
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
	pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

	pGridCtrl->SetCellType(9,0, RUNTIME_CLASS(CGridCellSymbol));
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(9,0);
	pCellSymbol->SetDefaultFont(ESFSmall);
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
	pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	

	pGridCtrl->ShowWindow(SW_SHOW);
	SetGridFloatInitPos();
}

void CIoViewTrendArbitrage::CreateRegion()
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
	m_pRegionMain->SetBottomSkip(16);	
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

void CIoViewTrendArbitrage::SplitRegion()
{
	CIoViewChart::SplitRegion();
	UpdateGDIObject();
}

int32 CIoViewTrendArbitrage::GetSubRegionTitleButtonNums()
{
	return 0;
}

const T_FuctionButton* CIoViewTrendArbitrage::GetSubRegionTitleButton(int32 iIndex)
{
	if( iIndex < 0 || iIndex >= GetSubRegionTitleButtonNums() )
	{
		return NULL;
	}

	ASSERT( 0 );
	return NULL;
}

void CIoViewTrendArbitrage::OnDestroy() 
{
	CArbitrageManage::Instance()->DelNotify(this);

	ReleaseMemDC();

	CIoViewChart::OnDestroy();
}

E_NodeTimeInterval CIoViewTrendArbitrage::GetTimeInterval()
{
	if (m_MerchParamArray.GetSize() > 0)
	{
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
		if (NULL != pData)
		{
			return (pData->m_eTimeIntervalFull);
		}
	}
	
	return ENTICount;
}

// 通知视图改变关注的商品
void CIoViewTrendArbitrage::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	// 事实上套利并不关心商品的切换，仅关心套利内容的变更
	_LogCheckTime("[Client==>] CIoViewTrendArbitrage::OnVDataMerchChanged", g_hwndTrace);

	if (m_pMerchXml == pMerch || NULL == pMerch )
		return;
	
	// 修改当前查看的商品
	//CMerch *pMerchXmlOld        = m_pMerchXml;	// 原来关注的
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
	
	if (NULL != pMerch)
	{		
	}
	else
	{
		// zhangbo 20090824 #待补充， 当心商品不存在时，清空当前显示数据
		//...
	}
}

// 叠加视图相关
bool32 CIoViewTrendArbitrage::OnVDataAddCompareMerch(IN CMerch *pMerch)
{
	return true;
}

void CIoViewTrendArbitrage::SetCurveTitle ( T_MerchNodeUserData* pData )
{
	CString StrTitle;
	CMerch* pMerch = pData->m_pMerchNode;
	if ( NULL == pMerch )
	{
		if ( pData->bMainMerch )
		{
			StrTitle = _T(" ") + TimeInterval2String(pData->m_eTimeIntervalFull) + _T(" (无)");				
		}
		else
		{
			StrTitle.Empty();
		}
	}
	else
	{
					
		if ( pData->bMainMerch )
		{
			StrTitle = _T("收盘价差");
		}
		else
		{
			ASSERT( 0 );
			StrTitle = _T("[") + pMerch->m_MerchInfo.m_StrMerchCnName + _T("] ");
		}
	}
	
	pData->m_pKLinesShow->SetName(StrTitle);

	if ( NULL != m_pRegionMain )
	{
		// 两腿套利 浦发银行/白云机场(日线)
		StrTitle = _T("") + ArbEnumToString(m_Arbitrage.m_eArbitrage);
		StrTitle += _T(" ");
		
		if ( m_Arbitrage.m_MerchA.m_pMerch != NULL )
		{
			StrTitle += m_Arbitrage.GetShowName();
		}
		
		m_pRegionMain->SetTitle(StrTitle);
	}
}

void CIoViewTrendArbitrage::OnVDataForceUpdate()
{
	if ( NULL == m_pRegionMain )
	{
		return;
	}

	// 重新计算交易时间信息
	if ( NULL != GetMainData() )
	{
		UpdateAllMerchDataTradeTimes();
		CombineMainDataTradeTime();
	}

	CIoViewChart::OnVDataForceUpdate();
	RequestViewData();

	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	PostMessage(WM_PAINT);
}

void CIoViewTrendArbitrage::OnVDataFormulaChanged ( E_FormulaUpdateType eUpdateType, CString StrName )
{
	CIoViewChart::OnVDataFormulaChanged(eUpdateType, StrName);
}

void CIoViewTrendArbitrage::OnVDataRealtimePriceUpdate(IN CMerch *pMerch)
{
	T_MerchNodeUserData *pMerchData = GetMerchData(pMerch);
	if ( NULL != pMerchData )
	{
		ASSERT( pMerchData->m_TrendTradingDayInfo.m_bInit );
		float fPricePre = pMerchData->m_TrendTradingDayInfo.GetPrevReferPrice();
		pMerchData->m_TrendTradingDayInfo.RecalcPrice(*pMerchData->m_pMerchNode);
		if ( fPricePre != pMerchData->m_TrendTradingDayInfo.GetPrevReferPrice() )
		{
			// 变更了中轴值
			ReDrawAysnc();
		}
	}
}

void CIoViewTrendArbitrage::OnVDataPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType)
{
	// ...fangz1117  目前 UpdateMainMerchKLine 中是每次都 bUpdate = false 全部数据重算,
	// 优化以后,要处理除权等数据,不要因为在收盘后没有K 线重算的时机而导致没有更新

	if (NULL == pMerch || pMerch != m_pMerchXml)
		return;
	
	if (EPFTWeight != ePublicFileType)	// 仅处理除权数据
		return;

	// 挨个商品更新所有数据
}

void CIoViewTrendArbitrage::ClearLocalData(bool32 bClearAll/* = true*/)
{
	if ( bClearAll )
	{
		for (int32 i = 0; i < m_MerchParamArray.GetSize(); i++)
		{
			T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(i);
			pData->m_aKLinesCompare.RemoveAll();
			pData->m_aKLinesFull.RemoveAll();
			
			if (NULL != pData->m_pKLinesShow)
				pData->m_pKLinesShow->RemoveAll();
			
			RemoveIndexsNodesData(pData);
		}
	}
	else
	{
		// 仅清除mainnode
		for (int32 i = 0; i < m_MerchParamArray.GetSize(); i++)
		{
			T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(i);
			if ( pData->bMainMerch )
			{
				pData->m_aKLinesCompare.RemoveAll();
				pData->m_aKLinesFull.RemoveAll();
				
				if (NULL != pData->m_pKLinesShow)
					pData->m_pKLinesShow->RemoveAll();
				
				RemoveIndexsNodesData(pData);
				break;
			}
		}
	}
}

void CIoViewTrendArbitrage::OnVDataMerchKLineUpdate(IN CMerch *pMerch)
{
	if (NULL == pMerch)
		return;
	
	if (m_MerchParamArray.GetSize() <= 0)
	{
		return;
	}
	
	// 挨个商品更新所有数据 - 从实际数据开始
	for (int32 i = 1; i < m_MerchParamArray.GetSize(); i ++)
	{
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(i);
		
		// main实际并没有关联商品
		if (pData->m_pMerchNode == pMerch && !pData->bMainMerch )
		{
			if (UpdateSubMerchKLine(*pData, false))
			{
				
			}
		}
	}
}

bool32 CIoViewTrendArbitrage::UpdateMainMerchKLine(T_MerchNodeUserData &MerchNodeUserData, bool32 bForceUpdate)
{
	CalcArbitrage();	// 立即开始计算套利数据
	return true;
}

bool32 CIoViewTrendArbitrage::UpdateSubMerchKLine(T_MerchNodeUserData &MerchNodeUserData, bool32 bForceUpdate/*=true*/)
{
	if (m_MerchParamArray.GetSize() <= 0 || m_MerchParamArray[0] == &MerchNodeUserData)
	{
		//ASSERT(0);
		return false;
	}
	
	// 用来更新关联商品
	T_MerchNodeUserData *pMainData = GetMainData(true);
	if ( NULL == pMainData )
	{
		ASSERT( 0 );
		return false;
	}

	CMerch* pMerch = MerchNodeUserData.m_pMerchNode;
	if (NULL == pMerch)
	{
		return false;
	}

	// zhangbo 20090714 #待处理
	//...
	_LogCheckTime("[==>Client] CIoViewTrendArbitrage::UpdateSubMerchKLine 开始", g_hwndTrace);
	float fPricePrevClose = MerchNodeUserData.m_TrendTradingDayInfo.GetPrevReferPrice();
	MerchNodeUserData.m_TrendTradingDayInfo.RecalcPrice(*pMerch);
	if (fPricePrevClose != MerchNodeUserData.m_TrendTradingDayInfo.GetPrevReferPrice())
	{
		// zhangbo 20090708 #待补充, 更新中轴值
		//...		
		ReDrawAysnc();	// 需要重新计算中轴
	}

	CGmtTime TimeStartInFullList, TimeEndInFullList;
	if ( !MerchNodeUserData.m_TrendTradingDayInfo.m_bInit )
	{
		ASSERT( 0 );
		ResetMerchUserData(&MerchNodeUserData);
		CalcArbitrageAsyn();
		return true;
	}
	TimeStartInFullList = MerchNodeUserData.m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeOpen.m_Time;
	TimeEndInFullList = MerchNodeUserData.m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeClose.m_Time;

	// 更新数据
	// 先获取对应的K线
	int32 iPosFound;
	CMerchKLineNode* pKLineRequest = NULL;
	pMerch->FindMerchKLineNode(EKTBMinute, iPosFound, pKLineRequest);
	
	// 根本找不到K线数据， 那就不需要显示了
	if (NULL == pKLineRequest || 0 == pKLineRequest->m_KLines.GetSize())	
	{
		ResetMerchUserData(&MerchNodeUserData);
		CalcArbitrageAsyn();
		return true;
	}

	// 获取区间
	int32 iStartPos, iEndPos;
	iStartPos = iEndPos = 0;
	iStartPos = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pKLineRequest->m_KLines, TimeStartInFullList);
	iEndPos	  = CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(pKLineRequest->m_KLines, TimeEndInFullList);
	if ( iStartPos < 0 || iStartPos > iEndPos || iEndPos >= pKLineRequest->m_KLines.GetSize() )
	{
		// 无有效数据
		ResetMerchUserData(&MerchNodeUserData);
		CalcArbitrageAsyn();
		return true;
	}

	const int32 iCount = iEndPos-iStartPos+1;

	CKLine *pKLineSrc = (CKLine *)pKLineRequest->m_KLines.GetData();
	
	CKLine *pKLineCmp = (CKLine *)MerchNodeUserData.m_aKLinesCompare.GetData();
	int32 iCountCmp   = MerchNodeUserData.m_aKLinesCompare.GetSize();

	int32 iCmpResult = bForceUpdate ? 1 : CompareKLinesChange(pKLineCmp, iCountCmp, pKLineSrc+iStartPos, iCount); // 原始数据比较

	CArray<CKLine, CKLine> aTmpCmpKlines;
	
	// 有可能周期或者权息信息变更，导致需要重新合成k线
	if ( iCmpResult != 0 )
	{
		// 需要更新合成K线
		// 清除原始&显示数据
		aTmpCmpKlines.SetSize(iCount);
		memcpyex(aTmpCmpKlines.GetData(), pKLineSrc+iStartPos, iCount*sizeof(CKLine));
		
		// 实际数据K线合成完成
		// 有可能K线没有均价数据，所以自己合成
		// 分钟K线的均价实际是日线的，而且分时图形并不显示均价相关
		for ( int32 i=aTmpCmpKlines.GetSize()-1; i >= 0 ; --i )
		{
			CKLine &kline = aTmpCmpKlines[i];
			// 			if ( kline.m_fPriceAvg == 0.0f )
			// 			{
			// 				// 没有则尝试自己运算下
			// 				if ( kline.m_fVolume >= 0.9f )
			// 				{
			// 					float fAvg = kline.m_fAmount / kline.m_fVolume / 100;
			// 					if ( fAvg > kline.m_fPriceHigh || fAvg < kline.m_fPriceLow )
			// 					{
			// 						// 超过了限制，不要算了
			// 						aTmpCmpKlines.RemoveAt(i);
			// 						continue;
			// 					}
			// 					kline.m_fPriceAvg = fAvg;
			// 				}
			// 			}
			kline.m_fPriceAvg = (kline.m_fPriceClose+kline.m_fPriceHigh+kline.m_fPriceLow+kline.m_fPriceOpen)/4.0f;
		}
	}
	// compareK线与fullK线数据准备好了
	// 现在Full已经是要求的K线了
	bool32	bUpdateShowData = false;
	if ( 0 != iCmpResult )
	{
		// FullK线要求更新
		MerchNodeUserData.m_aKLinesCompare.Copy(aTmpCmpKlines);
		bUpdateShowData = true;	// 必须更新显示数据
	}

	// 商品数据计算完毕，计算套利
	if ( bUpdateShowData )
	{
		CalcArbitrageAsyn();
	}
	
	return true;
}

bool32 CIoViewTrendArbitrage::OnCanPanLeft ( CChartRegion* pRegion, CNodeSequence* pNodes, int32 id, int32 iNum )
{
	CGmtTime Time(id);
	T_MerchNodeUserData* pData = (T_MerchNodeUserData*)pNodes->GetUserData();
	
	if ( m_pRegionMain == pRegion || m_pRegionXBottom	== pRegion )		 
	{
		int32 iSearch = HalfSearch(pData->m_aKLinesCompare,id);

		if ( iSearch > 1 )
		{
			return true;
		}
	}

	return false;
}

bool32 CIoViewTrendArbitrage::OnCanPanRight ( CChartRegion* pRegion, CNodeSequence* pNodes, int32 id, int32 iNum )
{
	CGmtTime Time(id);
	T_MerchNodeUserData* pData = (T_MerchNodeUserData*)pNodes->GetUserData();
	if ( m_pRegionMain == pRegion ||
		 m_pRegionXBottom	== pRegion )
	{
		int32 iSearch = HalfSearch(pData->m_aKLinesCompare,id);
		if ( iSearch < pData->m_aKLinesCompare.GetSize()-1 )
		{
			return true;
		}
	}
	return false;
}

bool32 CIoViewTrendArbitrage::GetTips(IN CPoint pt, OUT CString& StrTips, OUT CString &StrTitle)
{
	// 得到Tips
	StrTips = L"";

	
	return false;
}

void CIoViewTrendArbitrage::OnNodesRelease ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes )
{
	//T_MerchNodeUserData* pData = (T_MerchNodeUserData*)pNodes->GetUserData();
	//pData->pMerch已经不被这个Curve使用了,如果没有其它Curve使用,则考虑清除数据,并且从AttendMerch中删除.
}

void CIoViewTrendArbitrage::OnRegionMenu2 ( CChartRegion* pRegion, CDrawingCurve* pCurve, int32 x, int32 y )
{
	RegionMenu2 ( pRegion, pCurve, x, y );
}

void CIoViewTrendArbitrage::OnRegionIndexMenu ( CChartRegion* pRegion, CDrawingCurve* pCurve, int32 x, int32 y )
{
	RegionIndexMenu ( pRegion, pCurve, x, y );
}

void CIoViewTrendArbitrage::OnRegionIndexBtn(CChartRegion* pRegion, CDrawingCurve* pCurve, int32 iID )
{
	RegionIndexBtn (pRegion, pCurve, iID);
}

bool32 CIoViewTrendArbitrage::LoadAllIndex(CNewMenu* pMenu)
{
	// 把所有的指标都加载到菜单里:

	if ( NULL == pMenu )
	{
		return false;
	}
	
	//
	if ( NULL == m_pRegionPick )
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
					
					bool32 bShow = CFormulaLib::BeIndexShow(pContent->name);	// 是否在界面上显示出来
					
					if ( bShow )
					{
						CString StrIndexFull = MakeMenuString(StrText, StrExp);
						
						pSubMenu->AppendODMenu(StrIndexFull, MF_STRING, IDM_IOVIEWKLINE_ALLINDEX_BEGIN + iIndex );
						
						iIndex += 1;
						m_aAllFormulaNames.Add(StrText);
					}					
				}

				if ( 0 == pSubMenu->GetMenuItemCount() )
				{
					// 没有子菜单的,把这项删掉
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
					
					CString StrIndexFull = MakeMenuString(StrText, StrExp);
					
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

	pMenu->RemoveMenu(0, MF_BYPOSITION);
	
	return true;
}


void CIoViewTrendArbitrage::GetOftenFormulars(IN CChartRegion* pRegion, OUT CStringArray& aFormulaNames, OUT int32& iSeperatorIndex, bool32 bDelSame /*= true*/)
{
	iSeperatorIndex = 0;
	aFormulaNames.RemoveAll();
	
	if ( NULL == pRegion )
	{
		return;
	}
	
	//
	if ( m_MerchParamArray.GetSize() <= 0 )
	{
		return;
	}
	
	T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
	
	if ( NULL == pData || NULL == m_pMerchXml )
	{
		return;
	}
	
	//
	uint32 iFlag = NodeTimeIntervalToFlag(pData->m_eTimeIntervalFull);
	
	if ( pRegion == m_pRegionMain )
	{
		AddFlag(iFlag, CFormularContent::KAllowMain);
	}
	else
	{
		AddFlag(iFlag, CFormularContent::KAllowSub);
	}
	
	//
	E_IndexStockType eIST = EIST_None;
	
	CFormulaLib::instance()->GetAllowNames(iFlag, aFormulaNames, eIST);
	
	if ( bDelSame )
	{
		CStringArray Formulas;
		FindRegionFormula(pRegion, Formulas);
		
		DeleteSameString (aFormulaNames, Formulas);
		Formulas.RemoveAll();	
	}
	
	// 按成交量类型的排序, 加上分隔符
	SortIndexByVol(aFormulaNames, iSeperatorIndex);
}

int32 CIoViewTrendArbitrage::LoadOftenIndex(CNewMenu* pMenu)
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
	
	if ( NULL == pData )
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
	
	E_IndexStockType eIST = EIST_Index;
	
	CFormulaLib::instance()->GetAllowNames(iFlag,m_FormulaNames, eIST);
	CStringArray Formulas;
	FindRegionFormula(m_pRegionPick,Formulas);
	DeleteSameString ( m_FormulaNames,Formulas);
	Formulas.RemoveAll();	
	
	bool32 bIsExp = false;
	
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

void CIoViewTrendArbitrage::OnRegionMenu ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes,CNodeData* pNodeData, int32 x, int32 y)
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

	m_pRegionPick= pRegion;
	m_pCurvePick = NULL;
	m_pNodesPick = NULL;
	int32 iSize, iRegion = -1;
	if ( pRegion == m_pRegionMain )
	{
		iRegion = 0;
	}
	//如果选中了其它坐标轴
	iSize = m_SubRegions.GetSize();
	for ( int32 i=0; i < iSize ; ++i )
	{
		if ( m_SubRegions[i].m_pSubRegionMain == pRegion )
		{
			iRegion = i+1;
			break;
		}
	}
	
	if ( iRegion < 0 )
	{
		return;
	}

	m_pCurvePick	= (CChartCurve*)pCurve;
	if ( NULL == m_pCurvePick )
	{
		m_pCurvePick = GetCurIndexCurve(pRegion);	// 使用region的第一个指标作为当前指标
	}
	m_pNodesPick	= pNodes;
	m_NodePick		= CNodeData(*pNodeData);

	// NodePick用来保存上次右键点击处的点
	if ( pRegion == m_pRegionMain )
	{
		CChartCurve *pDep = pRegion->GetDependentCurve();
		if ( NULL != pDep )
		{
			int32 iX = x;
			pRegion->ClientXToRegion(iX);
			int32 iPos;
			if ( pDep->RegionXToCurvePos(iX, iPos) )
			{
				CNodeSequence *pTmpNodes = pDep->GetNodes();
				if ( NULL != pTmpNodes )
				{
					pTmpNodes->GetAt(iPos, m_NodePick);
				}
			}
		}
	}

	CPoint pt(x,y);
	ClientToScreen(&pt);

	CChartCurve *pCurveTmp = NULL;
	if ( iRegion == 1
		&& NULL != (pCurveTmp=m_SubRegions[0].m_pSubRegionMain->GetCurve(0))
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
	
	//
	CNewMenu menu;						  
	//
	menu.LoadMenu(IDR_MENU_TRENDARB);
	menu.LoadToolBar(g_awToolBarIconIDs);

	CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
	CMenu* pTempMenu = NULL;
	//

	//////////////////////////////////////////////////////////////////////////
	

	
	// 插入内容:
	pTempMenu = pPopMenu->GetSubMenu(L"插入内容");
	ASSERT(NULL!=pTempMenu);
	CNewMenu * pIoViewPopMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );
	AppendIoViewsMenu(pIoViewPopMenu, IsLockedSplit());

	// 图标:
	pPopMenu->ModifyODMenu(L"自画线",   L"自画线",   IDB_TOOLBAR_OWNDRAW);

	// 如果处在锁定分割状态，需要删除一些按钮
	CMPIChildFrame *pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pChildFrame && pChildFrame->IsLockedSplit() )
	{
		pChildFrame->RemoveSplitMenuItem(*pPopMenu);
	}

	pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, AfxGetMainWnd());
	menu.DestroyMenu();
}
  
void CIoViewTrendArbitrage::OnAllIndexMenu(UINT nID)
{
	// 与所有指标对应的处理过程.跟以前的菜单分开.方便调试.不需要的时候删除也方便
	if ( NULL == m_pRegionPick )
	{	
		return;
	}

	int32 iIndex = nID - IDM_IOVIEWKLINE_ALLINDEX_BEGIN;

	if ( iIndex < 0 || iIndex >= m_aAllFormulaNames.GetSize() )
	{
		return;
	}

	CString StrName = m_aAllFormulaNames.GetAt(iIndex);
	
	// 清除现在的
	DelCurrentIndex();
	m_aAllFormulaNames.RemoveAll();
	//
	
	T_IndexParam* pIndex = AddIndex(m_pRegionPick,StrName);
	
	if ( NULL == pIndex )
	{
		return;
	}
	
	T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
	int32 iNodeBegin,iNodeEnd;
	GetNodeBeginEnd(iNodeBegin,iNodeEnd);
	g_formula_compute ( this, this, (CChartRegion*)pIndex->pRegion, pData, pIndex, iNodeBegin, iNodeEnd);

	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	UpdateSelfDrawCurve();
	Invalidate();
}

void CIoViewTrendArbitrage::OnOftenIndexMenu(UINT nID)
{
	// 常用指标
	int32 iIndex = nID - IDM_IOVIEWKLINE_OFTENINDEX_BEGIN;

	if ( iIndex < 0 || iIndex >= m_FormulaNames.GetSize() )
	{
		return;
	}

	CString StrName = m_FormulaNames.GetAt(iIndex);

	// 清除现在的
	DelCurrentIndex();
	m_FormulaNames.RemoveAll();

	//
	T_IndexParam* pIndex = AddIndex(m_pRegionPick, StrName);
	
	if ( NULL == pIndex )
	{
		return;
	}
	
	T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
	int32 iNodeBegin,iNodeEnd;
	GetNodeBeginEnd(iNodeBegin,iNodeEnd);
	g_formula_compute ( this, this, (CChartRegion*)pIndex->pRegion, pData, pIndex, iNodeBegin,iNodeEnd );
	
	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	UpdateSelfDrawCurve();
	Invalidate();
}


bool32 CIoViewTrendArbitrage::AddShowIndex( const CString &StrIndexName, bool32 bDelRegionAllIndex /*= false*/, bool32 bChangeStockByIndex /*= false*/, bool32 bShowUserRightDlg/*=false*/ )
{
	T_MerchNodeUserData* pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == m_pRegionMain || NULL == pData )
	{
		ASSERT( 0 );
		return false;
	}

	const CFormularContent *pFormulaContent = CFormulaLib::instance()->GetFomular(StrIndexName);
	if ( NULL == pFormulaContent )
	{
		ASSERT( 0 );
		return false;	// 无此指标
	}

	if ( NULL == m_pMerchXml )
	{
		bChangeStockByIndex = true;	// 无商品，切换
	}

	
	{
		if ( CheckFlag(pFormulaContent->flag, CFormularContent::KAllowMain) )
		{
			if ( bDelRegionAllIndex )
			{
				ClearRegionIndex(m_pRegionMain, true);	// 清除主图指标
			}
			else
			{
				
			}
			ChangeIndexToMainRegion(StrIndexName);
		}
		else if ( CheckFlag(pFormulaContent->flag, CFormularContent::KAllowSub) )
		{
			// 变更特定副图 - 副图无其它指标需要清除，所有指标都是替换策略
			{
				AddIndexToSubRegion(StrIndexName, true, 0);	// 没有发现有
			}
		}
	}
	return true;
}


void CIoViewTrendArbitrage::ReplaceIndex( const CString &StrOldIndex, const CString &StrNewIndex, bool32 bDelRegionAllIndex/* = false*/, bool32 bChangeStock/* = false*/ )
{
	// 指标是否有效
	const CFormularContent *pFormulaContentOld = CFormulaLib::instance()->GetFomular(StrOldIndex);
	if ( NULL == pFormulaContentOld )
	{
		ASSERT( 0 );
		return;	// 无此指标
	}
	const CFormularContent *pFormulaContentNew = CFormulaLib::instance()->GetFomular(StrNewIndex);
	if ( NULL == pFormulaContentNew )
	{
		ASSERT( 0 );
		return;
	}
	
	{
		int iMainOld = CheckFlag(pFormulaContentOld->flag, CFormularContent::KAllowMain) ? 1 : 0;
		int iMainNew = CheckFlag(pFormulaContentNew->flag, CFormularContent::KAllowMain) ? 1 : 0;
		
		ASSERT( iMainNew == iMainOld );	// 严格类型应该相等，但是用户可能修改类型，所以允许变更，虽然不一定是替换
		
		if ( iMainNew != 0 )
		{
			AddShowIndex(StrNewIndex, bDelRegionAllIndex, bChangeStock);
		}
		else
		{
			// 副图，替换所有副图的指标
			// 找出原指标存在哪个region	
			CArray<CChartRegion *, CChartRegion *> aRgnDst;
			for ( int32 j=0; j < m_MerchParamArray.GetSize() ; j++ )
			{
				T_MerchNodeUserData *pData = m_MerchParamArray[j];
				if ( NULL == pData )
				{
					continue;
				}
				
				for ( int32 i = pData->aIndexs.GetSize()-1 ; i >= 0 ; i-- )
				{
					T_IndexParam *pParam = pData->aIndexs[i];
					if ( NULL != pParam && pParam->strIndexName == StrOldIndex )
					{
						if ( NULL != pParam->pRegion )
						{
							bool32 bAdded = false;
							for ( int32 k=0; k < aRgnDst.GetSize() ; k++ )
							{
								if ( aRgnDst[k] == pParam->pRegion )
								{
									bAdded = true;
									break;
								}
							}
							if ( !bAdded )
							{
								aRgnDst.Add((CChartRegion*)pParam->pRegion);
							}
						}
					}
				}
			}
			if ( aRgnDst.GetSize() < 1 )
			{
				return;	// 无原来的指标，那就可以不操作了
			}
			
			for ( int32 i=0; i < aRgnDst.GetSize() ; i++ )
			{
				if ( aRgnDst[i] != m_pRegionMain )
				{
					AddIndexToSubRegion(StrNewIndex, aRgnDst[i] );
				}
			}
		}
	}
}

void CIoViewTrendArbitrage::ChangeIndexToMainRegion( const CString &StrIndexName )
{
	T_MerchNodeUserData* pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == m_pRegionMain || NULL == pData )
	{
		ASSERT( 0 );
		return;
	}

	// 得到能添加的所有指标名称
	const CFormularContent *pFormulaContent = CFormulaLib::instance()->GetFomular(StrIndexName);
	if ( NULL == pFormulaContent || !CheckFlag(pFormulaContent->flag, CFormularContent::KAllowMain) )
	{
		ASSERT( 0 );
		return;	// 无此指标或非主图指标， 不处理
	}

// 	m_pRegionPick = m_pRegionMain;	// delcurrent需要
// 	DelCurrentIndex();
	ClearRegionIndex(m_pRegionMain); 

	T_IndexParam *pIndex = AddIndex(m_pRegionMain, StrIndexName);
	if ( NULL == pIndex )
	{
		ASSERT( 0 );		// 怎么挽救？？
		return;
	}

		
	int32 iNodeBegin,iNodeEnd;
	GetNodeBeginEnd(iNodeBegin,iNodeEnd);
	g_formula_compute ( this, this, (CChartRegion*)pIndex->pRegion, pData, pIndex, iNodeBegin,iNodeEnd );
	
	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	UpdateSelfDrawCurve();
	Invalidate();
}


void CIoViewTrendArbitrage::AddIndexToSubRegion( const CString &StrIndexName, bool32 bAddSameIfExist/* = true*/, int32 iSubRegionIndex/* = -1*/ )
{
	T_MerchNodeUserData* pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == m_pRegionMain || NULL == pData )
	{
		ASSERT( 0 );
		return;
	}

	int32 i = 0;
	
	const CFormularContent *pFormulaContent = CFormulaLib::instance()->GetFomular(StrIndexName);
	if ( NULL == pFormulaContent || !CheckFlag(pFormulaContent->flag, CFormularContent::KAllowSub) )
	{
		ASSERT( 0 );
		return;	// 无此指标或非副图指标， 不处理
	}
	
	// 如果不需要添加重复副图，则检查是否已经存在该副图指标
	if ( !bAddSameIfExist )
	{
		for ( i=0; i < pData->aIndexs.GetSize() ; i++ )	// 使用的是副图指标判断
		{
			if ( NULL == pData->aIndexs.GetAt(i)->pRegion || pData->aIndexs.GetAt(i)->pRegion == m_pRegionMain)
			{
				continue;
			}
			if ( pData->aIndexs[i]->strIndexName == StrIndexName )
			{
				return;	// 已经存在该副图, 不需要继续添加了
			}
		}
	}
	

	CChartRegion *pSubRegion = NULL; // 由于不会在此过程中变更subregion，所以应当可以
	if ( iSubRegionIndex >= 0 && iSubRegionIndex <= m_SubRegions.GetSize()-1 )
	{
		pSubRegion = m_SubRegions[iSubRegionIndex].m_pSubRegionMain;
	}
	else
	{
		// 不存在，在尾部添加一个SubRegion
		if ( !AddSubRegion(false) || m_SubRegions.GetSize() < 1 )
		{
			return;
		}
		pSubRegion = m_SubRegions[m_SubRegions.GetSize()-1].m_pSubRegionMain;
	}

	AddIndexToSubRegion(StrIndexName, pSubRegion);
}

void CIoViewTrendArbitrage::AddIndexToSubRegion( const CString &StrIndexName, CChartRegion *pSubRegion )
{
	T_MerchNodeUserData* pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == m_pRegionMain || NULL == pData )
	{
		ASSERT( 0 );
		return;
	}

	if ( NULL == pSubRegion )
	{
		ASSERT( 0 );
		return;
	}
	
	// 删除当前SubRegion的指标
	ClearRegionIndex(pSubRegion);
	
	T_IndexParam* pIndex = AddIndex(pSubRegion, StrIndexName);
	if ( NULL == pIndex )
	{
		ASSERT( 0 );
		return;
	}				
	int32 iNodeBegin,iNodeEnd;
	GetNodeBeginEnd(iNodeBegin,iNodeEnd);
	g_formula_compute ( this, this, pSubRegion, pData, pIndex, iNodeBegin,iNodeEnd );
	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	UpdateSelfDrawCurve();
	
	RedrawWindow();
}

void CIoViewTrendArbitrage::RemoveShowIndex( const CString &StrIndexName )
{
	bool32 bRemove = false;
	for ( int32 j=0; j < m_MerchParamArray.GetSize() ; j++ )
	{
		T_MerchNodeUserData *pData = m_MerchParamArray[j];
		if ( NULL == pData )
		{
			continue;;
		}
		
		for ( int32 i = pData->aIndexs.GetSize()-1 ; i >= 0 ; i-- )
		{
			T_IndexParam *pParam = pData->aIndexs[i];
			if ( NULL != pParam && pParam->strIndexName == StrIndexName )	// 清除该名称指标
			{
				CArray<CChartCurve*,CChartCurve*> Curves;
				FindCurvesByIndexParam(pParam,Curves);	
				while ( Curves.GetSize() > 0 )
				{
					CChartCurve *pCurve = Curves.GetAt(0);
					Curves.RemoveAt(0);
					pCurve->GetChartRegion().RemoveCurve(pCurve);
				}
				
				DeleteIndexParamData ( pParam );	// 此时aIndexs的size已经变更

				bRemove = true;
			}
		}
	}

	// 还有什么指标吗？
	if ( bRemove )
	{
		ReDrawAysnc();
	}
}

void CIoViewTrendArbitrage::ClearRegionIndex( CChartRegion *pRegion, bool32 bDelExpertTip/* = false*/ )
{
	// 删除已有的指标
	if( NULL == pRegion )
	{
		ASSERT( 0 );
		return;
	}

	for ( int32 j=0; j < m_MerchParamArray.GetSize() ; j++ )
	{
		T_MerchNodeUserData *pData = m_MerchParamArray[j];
		if ( NULL == pData )
		{
			continue;;
		}
		
		for ( int32 i = pData->aIndexs.GetSize()-1 ; i >= 0 ; i-- )
		{
			T_IndexParam *pParam = pData->aIndexs[i];
			if ( NULL != pParam && pParam->pRegion == pRegion )	// 清除该region下所有指标
			{
				CArray<CChartCurve*,CChartCurve*> Curves;
				FindCurvesByIndexParam(pParam,Curves);	
				while ( Curves.GetSize() > 0 )
				{
					CChartCurve *pCurve = Curves.GetAt(0);
					Curves.RemoveAt(0);
					pCurve->GetChartRegion().RemoveCurve(pCurve);
				}
				
				DeleteIndexParamData ( pParam );	// 此时aIndexs的size已经变更
			}
		}
	
	}
	
	if ( pRegion == m_pRegionMain )
	{
	}
	
	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	UpdateSelfDrawCurve();
	Invalidate();
}

void CIoViewTrendArbitrage::OnOwnIndexMenu(UINT nID)
{
	//
	DelCurrentIndex();

	
}

void CIoViewTrendArbitrage::OnMenu ( UINT nID )
{
	CChartCurve *pCurveDependent = m_pRegionMain->GetDependentCurve();
	if (NULL == pCurveDependent)
		return;

	CChartDrawer* pKLineDrawer = (CChartDrawer*)pCurveDependent->GetDrawer();
	if (NULL == pKLineDrawer)
		return;

	bool32 bNeedReDraw = false;

	switch(nID)
	{
	case IDM_CHART_HIDE_INDEX:
		DelCurrentIndex();
		break;
	case IDM_CHART_INSERT:
		MenuAddRegion();
		break;
	case IDM_CHART_DELETE:
		MenuDelRegion();
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
	default:
		break;
	}

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
	else if ( ID_ARBITRAGE_SETTING == nID )
	{
		// 套利设置，没有实现
		ASSERT( 0 );
	}
	else if ( IDM_ARBTTRAGE_SELECT == nID )
	{
		// 选择套利，TODO
		ASSERT( 0 );
	}

	if ( bNeedReDraw )
	{
		m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
		Invalidate();
	}
}


LRESULT CIoViewTrendArbitrage::OnMessageTitleButton(WPARAM wParam,LPARAM lParam)
{
	//
	UINT uID = (UINT)wParam;
	//
	
	if ( 0 == m_MerchParamArray.GetSize() )
	{
		return 0 ;
	}
	
// 	if ( NULL == m_pRegionPick)		// xl 由于TBWnd上的弹出菜单使m_pRegionPick为Null，实际这个只在某些选项需要判断
// 	{								//    
// 		return 0;
// 	}	
	
	if ( 1 == uID )
	{
		// 指标菜单
		int32 iIndexNums = 0;

		m_pRegionPick = m_pRegionMain;	
		m_FormulaNames.RemoveAll();
		CNewMenu menu;
		menu.LoadMenu(IDR_MENU_KLINE);
		menu.LoadToolBar(g_awToolBarIconIDs);

		CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(0));

		CMenu* pTempMenu = pPopMenu->GetSubMenu(_T("常用指标"));
		ASSERT( NULL != pTempMenu);
		CNewMenu* pSubMenu = DYNAMIC_DOWNCAST(CNewMenu,pTempMenu);

		pSubMenu->RemoveMenu(0,MF_BYPOSITION|MF_SEPARATOR);//删除尾巴
		

		iIndexNums = LoadOftenIndex(pSubMenu);

		if ( 0 == iIndexNums )
		{
			MessageBox(L"当前无可用指标,请在指标公式中设置!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		}
		else
		{
			CPoint pt;
			GetCursorPos(&pt);
			pSubMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, this);
			menu.DestroyMenu();
		}
	}

	return 0;
}

void CIoViewTrendArbitrage::OnPickNode ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes, CNodeData* pNodeData,int32 x,int32 y,int32 iFlag )
{
	if ( NULL != pNodeData )
	{
		CString StrText;
		T_MerchNodeUserData* pData = (T_MerchNodeUserData*)pNodes->GetUserData();
		ASSERT(NULL!=pData);
		CString StrTime = Time2String ( CGmtTime(pNodeData->m_iID), pData->m_eTimeIntervalFull);
		int32 iSaveDec = m_Arbitrage.m_iSaveDec;
		
		if ( CheckFlag(pCurve->m_iFlag,CChartCurve::KTypeKLine) )
		{
			StrText.Format(_T("时: %s \n开: %s\n收: %s\n均: %s\n"),
				StrTime.GetBuffer(),
				Float2String(pNodeData->m_fOpen,iSaveDec,true).GetBuffer(),
				Float2String(pNodeData->m_fClose,iSaveDec,true).GetBuffer(),
				Float2String(pNodeData->m_fAvg,iSaveDec,true).GetBuffer()
				);
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

void CIoViewTrendArbitrage::MenuAddRegion()
{
	if ( !AddSubRegion(false))
	{
		return;
	}

	// 得到主商品数据
	T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
	if ( NULL == pData || NULL == m_pMerchXml )
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

	E_IndexStockType eIST = EIST_None;

	// 得到能添加的所有指标名称
	uint32 uiFlag = NodeTimeIntervalToFlag( pData->m_eTimeIntervalFull);	
	AddFlag(uiFlag,CFormularContent::KAllowSub);

	m_FormulaNames.RemoveAll();
	CFormulaLib::instance()->GetAllowNames(uiFlag,m_FormulaNames, eIST);
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

void CIoViewTrendArbitrage::SetXmlSource ()
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

		CChartDrawer* pKLineDrawer = (CChartDrawer*)pCurveDependent->GetDrawer();
		if (NULL == pKLineDrawer)
			return;

		m_iDrawTypeXml = (int)pKLineDrawer->m_eChartDrawType;
	}
}

void CIoViewTrendArbitrage::DelCurrentIndex()
{
	// 删除已有的指标
	if( NULL == m_pRegionPick )
	{
		return;
	}

	ClearRegionIndex(m_pRegionPick);
}

void CIoViewTrendArbitrage::MenuAddIndex ( int32 id )
{

}

void CIoViewTrendArbitrage::MenuDelRegion()
{
	ASSERT( 0 );	// 仅有一个副图
	// 同时存在删除与添加，so需要变动regionpick
	if ( m_pRegionPick == m_pRegionMain || NULL == m_pRegionPick )
	{
		// 变为响应的副图
		ASSERT( 0 );
		return;	// 没有选中的副图
	}
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
	m_pRegionPick = NULL;

	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	RedrawWindow();
}

void CIoViewTrendArbitrage::MenuDelCurve()
{
	if ( NULL == m_pCurvePick || NULL == m_pRegionPick )
	{
		return;
	}
	CChartRegion &Region = m_pCurvePick->GetChartRegion();
	DeleteIndexCurve ( m_pCurvePick );
	if ( Region.GetCurveNum() <= 0 && &Region != m_pRegionMain )
	{
		DelSubRegion(&Region);	// 删除该region
	}

	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	UpdateSelfDrawCurve();
	Invalidate();
}

void CIoViewTrendArbitrage::OnDropCurve ( CChartRegion* pSrcRegion,CChartCurve* pSrcCurve,CChartRegion* pDestChart)
{
	if ( NULL == pDestChart )
	{
		DeleteIndexCurve(pSrcCurve);
	}
	else
	{
		//DragDropCurve( pSrcCurve,pDestChart);
	}

	UpdateSelfDrawCurve();

	CalcArbitrage();
	ReDrawAysnc();
}
void CIoViewTrendArbitrage::OnRegionCurvesNumChanged ( CChartRegion* pRegion,int32 iNum )
{
	
}
void CIoViewTrendArbitrage::OnCrossData ( CChartRegion* pRegion,int32 iPos,CNodeData& NodeData, float fPricePrevClose, float fYValue, bool32 bShow)
{
	if ( m_MerchParamArray.GetSize() < 1 ) return;
	
	if ( bShow )
	{
		T_MerchNodeUserData* pData = GetMainData();
		int32 iSaveDec = m_Arbitrage.m_iSaveDec;
		
		CKLine KLine; 
		NodeData2KLine(NodeData, KLine);
		
		if ( pRegion == m_pRegionMain )
		{
			// 记录下当前这根K线
			m_KLineCrossNow = KLine;
		}
		
		CString StrTimeLine1 = L"";
		CString StrTimeLine2 = L"";
		
		Time2String(KLine.m_TimeCurrent,pData->m_eTimeIntervalFull,StrTimeLine1,StrTimeLine2);
		
		if ( pRegion == m_pRegionMain )
		{
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
		CRect rect(0,0,0,0);
		EnableClipDiff(false,rect);
	}
}
void CIoViewTrendArbitrage::OnCrossNoData(CString StrTime,bool32 bShow)
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
		EnableClipDiff(false,CRect(0,0,0,0));
	}
}

void CIoViewTrendArbitrage::SetFloatData ( CGridCtrl* pGridCtrl,float fCursorValue,int32 iSaveDec)
{
	//return;

	if ( !m_bInitialFloat )
	{
		InitCtrlFloat(&m_GridCtrlFloat);
	}

	AdjustCtrlFloatContent();

	CString StrValue;
	CGridCellSymbol* pCellSymbol;

	//数值
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(3,0);
	//StrValue = Float2String(fCursorValue, iSaveDec, true) + _T(" ");
	Float2StringLimitMaxLength(StrValue, fCursorValue, iSaveDec, 7, true, true);
	pCellSymbol->SetText(StrValue);
	
	pGridCtrl->RedrawWindow();
}

void CIoViewTrendArbitrage::SetFloatData ( CGridCtrl* pGridCtrl,CKLine& KLine,float fPricePrevClose,float fCursorValue,CString StrTimeLine1,CString StrTimeLine2,int32 iSaveDec)
{
	if ( !m_bInitialFloat )
	{
		InitCtrlFloat(&m_GridCtrlFloat);
	}

	/*T_MerchNodeUserData* pDate =*/ GetMainData();

	// 调整详情窗口的显示
	AdjustCtrlFloatContent();

	// xl 0001762 清除所有的空格显示，限制数字显示长度，详情窗口的宽度比YLeft能多显示一个数字 - -
	int32 iYLeftShowChar = GetYLeftShowCharWidth();
	iYLeftShowChar += 1;
	// 
	CString StrValue;
	CGridCellSymbol* pCellSymbol;
	CGridCellSys   * pCellSys;
	// 时间
	pCellSys = (CGridCellSys *)pGridCtrl->GetCell(0,0);
	pCellSys->SetText(StrTimeLine1 + _T(""));

	pCellSys = (CGridCellSys *)pGridCtrl->GetCell(1,0);
	pCellSys->SetText(StrTimeLine2 + _T(""));
	
	//数值
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(3,0);	
	//StrValue = Float2SymbolString(fCursorValue, fCursorValue, iSaveDec) + _T(" ");
	Float2SymbolStringLimitMaxLength(StrValue, fCursorValue, fCursorValue, iSaveDec, iYLeftShowChar, true, false, true, false, true);
	pCellSymbol->SetText(StrValue);

	//开盘价
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(5,0);
	//StrValue = Float2SymbolString(KLine.m_fPriceOpen, fPricePrevClose, iSaveDec) + _T(" ");
	Float2SymbolStringLimitMaxLength(StrValue, KLine.m_fPriceOpen, fPricePrevClose, iSaveDec, iYLeftShowChar, true, false, true, false, true);
	pCellSymbol->SetText(StrValue);

	//收盘价
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(7,0);
	//StrValue = Float2SymbolString(KLine.m_fPriceHigh, fPricePrevClose, iSaveDec) + _T(" ");
	Float2SymbolStringLimitMaxLength(StrValue, KLine.m_fPriceClose, fPricePrevClose, iSaveDec, iYLeftShowChar, true, false, true, false, true);
	pCellSymbol->SetText(StrValue);

	//均价
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(9,0);
	//StrValue = Float2SymbolString(KLine.m_fPriceHigh, fPricePrevClose, iSaveDec) + _T(" ");
	Float2SymbolStringLimitMaxLength(StrValue, KLine.m_fPriceAvg, fPricePrevClose, iSaveDec, iYLeftShowChar, true, false, true, false, true);
	pCellSymbol->SetText(StrValue);

	pGridCtrl->RedrawWindow();

	// 
	if ( !pGridCtrl->IsWindowVisible() )
	{
		pGridCtrl->ShowWindow(SW_SHOW|SW_SHOWNOACTIVATE);
		
		CRect Rect;
		pGridCtrl->GetClientRect(&Rect);
		
		OffsetRect(&Rect,m_PtGridCtrl.x,m_PtGridCtrl.y);
		EnableClipDiff(true,Rect);
		
		pGridCtrl->RedrawWindow();					
	}	

}
void CIoViewTrendArbitrage::ClipGridCtrlFloat (CRect& Rect)
{
	EnableClipDiff(true,Rect);
}
CString CIoViewTrendArbitrage::OnTime2String ( CGmtTime& Time )
{
	CString StrTime = Time2String ( Time,ENTIMinute );
	if ( m_MerchParamArray.GetSize() > 0 )
	{
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
		StrTime = Time2String(Time,pData->m_eTimeIntervalFull);
	}
	return StrTime;
}

CString CIoViewTrendArbitrage::OnFloat2String ( float fValue, bool32 bZeroAsHLine, bool32 bNeedTerminate/* = false*/ )
{
	int32 iSaveDec = m_Arbitrage.m_iSaveDec;
	
	// xl 0001762 如果需要截断
	if ( bNeedTerminate )
	{
		CString StrValue;
		// 这个是游标调用的，其它默认不会调用true
		Float2StringLimitMaxLength(StrValue, fValue, iSaveDec, GetYLeftShowCharWidth(0), true, true, bZeroAsHLine);
		return StrValue;
	}
	return Float2String(fValue,iSaveDec,true,bZeroAsHLine);
}

void CIoViewTrendArbitrage::OnCalcXAxis(CChartRegion* pRegion, CDC* pDC, CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aXAxisDivide)
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
		// CalcMainRegionXAxis(aAxisNodes, aXAxisDivide);	

		// 复制，应该将X轴点单独出来，大家一起公用
		if ( NULL != m_pRegionMain )
		{
			aAxisNodes.Copy(m_pRegionMain->m_aXAxisNodes);
			aXAxisDivide.Copy(m_pRegionMain->m_aXAxisDivide);
		}
	}
}
	
void CIoViewTrendArbitrage::OnCalcYAxis(CChartRegion* pRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide)
{
	DWORD dwB = timeGetTime();

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

	DWORD dwE = timeGetTime();
	
	CString StrTrace;
	StrTrace.Format(L"%s   CalcYAxis 耗时: %d ms \r\n", pRegion->m_StrName.GetBuffer(), (dwE - dwB));
	// TRACE(StrTrace);
}

///////////////////////////////////////////////////
//
void CIoViewTrendArbitrage::OnRegionDrawNotify(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC,CRegion* pRegion,E_RegionDrawNotifyType eType)
{
	//CMPIChildFrame* pFrame = (CMPIChildFrame*)GetParentFrame();
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

		if ( m_SubRegions.GetSize() > 0
			&& m_SubRegions[0].m_pSubRegionMain == pChartRegion )
		{
			
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
			
		}
		else if ( pChartRegion == m_pRegionYLeft )
		{
			m_pRegionMain->GetRectCurves();
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
			m_pRegionMain->GetRectCurves();
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
				
				// 
				pDC->SetTextColor(AxisDivide.m_DivideText2.m_lTextColor);
				pDC->SetBkMode(TRANSPARENT);
				pDC->DrawText(AxisDivide.m_DivideText2.m_StrText, &Rect, AxisDivide.m_DivideText2.m_uiTextAlign);
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
				int32 i;
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
						pDC->SetTextColor(pAxisDivide[i].m_DivideText1.m_lTextColor);
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
			
			if (NULL != pData )
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
				pDC->SetTextColor(0xffffff);	// zhangbo 20090706 #暂时用白色
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

///////////////////////////////////////////////////////////////////////////////
void CIoViewTrendArbitrage::OnSliderId ( int32& id, int32 iJump )
{
	SliderId ( id, iJump );
}
CString CIoViewTrendArbitrage::OnGetChartGuid ( CChartRegion* pRegion )
{
	return GetChartGuid(pRegion );
}

void CIoViewTrendArbitrage::DoFromXml()
{
	if (m_bShowTopMerchBar || (GetIoViewManager() && GetIoViewManager()->GetManagerTopbarStatus()))
	{
		m_iTopButtonHeight = TOP_BUTTON_HEIHGT - 1;	
	}
	SetViewToolBarHeight(m_iTopButtonHeight+m_iTopMerchHeight);
	BShowViewToolBar(m_bShowIndexToolBar, m_bShowTopToolBar);
	if ( m_bFromXml )
	{
		
		CIoViewChart::DoFromXml();

		// 有副图否
		bool32 bAddIndex = true;
		CChartRegion *pSub = NULL;
		if ( m_SubRegions.GetSize() > 0 )
		{
			pSub = m_SubRegions[0].m_pSubRegionMain;

			T_MerchNodeUserData *pMainData = GetMainData();
			if ( NULL != pMainData )
			{
				for ( int32 i=0; i < pMainData->aIndexs.GetSize() ; ++i )
				{
					T_IndexParam *pIndex = pMainData->aIndexs[i];
					if ( NULL != pIndex && pIndex->pRegion == pSub )
					{
						bAddIndex = false;	// 有指标了
						break;
					}
				}
			}
		}
		if ( bAddIndex )
		{
			if ( m_SubRegions.GetSize() == 0 )
			{
				AddSubRegion();
				m_pRegionMain->NestSizeAll();
				pSub = m_SubRegions.GetSize() > 0 ? m_SubRegions[0].m_pSubRegionMain : NULL;
			}
			if ( NULL != pSub )
			{
				MerchArray aMerchs;
				if ( GetArbitrageMerchs(m_Arbitrage, aMerchs) )
				{
					AddIndex(pSub,_T("MACD"));
				}	
				else
				{
					m_IndexPostAdd.id = 0;
					m_IndexPostAdd.pRegion = pSub;
					m_IndexPostAdd.StrIndexName = _T("MACD");
				}
			}
		}
		
		// 自定义周期值:
		if ( m_MerchParamArray.GetSize() > 0)
		{
			T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
			if ( NULL != pData)
			{
				pData->m_iTimeUserMultipleMinutes = m_uiUserMinutesXml;
				pData->m_iTimeUserMultipleDays	  = m_uiUserDaysXml;
			}			
		}
		return;
	}

	UpdateAxisSize(false);	// 更新区域预留位置

	AddSubRegion();
	m_pRegionMain->NestSizeAll();
	T_SubRegionParam SubParam = m_SubRegions.GetAt(0);

	MerchArray aMerchs;
	if ( GetArbitrageMerchs(m_Arbitrage, aMerchs) )
	{
		AddIndex(SubParam.m_pSubRegionMain,_T("MACD"));
	}	
	else
	{
		m_IndexPostAdd.id = 0;
		m_IndexPostAdd.pRegion = SubParam.m_pSubRegionMain;
		m_IndexPostAdd.StrIndexName = _T("MACD");
	}
}

IChartBrige CIoViewTrendArbitrage::GetRegionParentIoView()
{
	IChartBrige ChartRegion;
	ChartRegion.pWnd = this;
	ChartRegion.pIoViewBase = this;
	ChartRegion.pChartRegionData = this;
	return ChartRegion;
}


void CIoViewTrendArbitrage::SetChildFrameTitle()
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

void CIoViewTrendArbitrage::GetCrossKLine(OUT CKLine & KLineIn)
{
	 memset(&KLineIn,0,sizeof(Kline));
	 memcpyex(&KLineIn,&m_KLineCrossNow,sizeof(Kline));
}

void CIoViewTrendArbitrage::OnIoViewActive()
{
	CIoViewChart::OnIoViewActive();
}

void CIoViewTrendArbitrage::OnIoViewDeactive()
{
	CIoViewChart::OnIoViewDeactive();
	m_TipWnd.Hide();
}

void CIoViewTrendArbitrage::CalcLayoutRegions(bool bOnlyUpdateMainRegion)
{
	if (m_MerchParamArray.GetSize() <= 0)
		return;

	T_MerchNodeUserData* pMainData = GetMainData();
	if ( NULL == pMainData )
	{
		return;
	}

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

		double iSaveDec = m_Arbitrage.m_iSaveDec;
		
		ASSERT( iSaveDec >= 0 );
		//float fPricePrevClose = pMerchData->m_TrendTradingDayInfo.GetPrevReferPrice();	// 昨收
		float fPricePrevClose = GetTrendPrevClose();
		if ( iSaveDec <= 0 || _MYABSF(fPricePrevClose) >= 1000.0f )	// 昨收高于1000由于宽度原因将不能显示全小数位
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
						// 最小刻度不能比实际有效值太大，免得成为一条直线
						// 比较是否比最小刻度小，小则取最小刻度
						float fReal = max(fCurveYMax-fAxisPreClose, fAxisPreClose-fCurveYMin);
						float fReal2 = max(fAxisMaxY-fAxisPreClose, fAxisPreClose-fAxisMinY);
						if ( fReal2 < fReal*3.5f )
						{
							fCurveYMax = max(fCurveYMax, fAxisMaxY);
							fCurveYMin = min(fCurveYMin, fAxisMinY);
						}
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

					//TRACE(_T("分时最大最小: %d %f-%f\n"), bValidCurveYMinMax, fCurveYMin, fCurveYMax);
				}
			}		
		}
	}
}

bool32 CIoViewTrendArbitrage::CalcMainRegionXAxis(CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aAxisDivide)
{	
	ASSERT(NULL != m_pRegionMain);

	aAxisNodes.SetSize(0);
	aAxisDivide.SetSize(0);

	// 
	T_MerchNodeUserData* pData = GetMainData();
	if (NULL == pData)
		return false;

	// 
	CTrendTradingDayInfo &TrendTradingDay = pData->m_TrendTradingDayInfo;
	if (!TrendTradingDay.m_bInit)
	{
		////ASSERT(0);
		return false;
	}

	// 
	CRect Rect = m_pRegionMain->GetRectCurves();
	Rect.DeflateRect(1, 1, 1, 1);
	// 普通状态下
	{
		int32 iMaxTrendUnitCount = TrendTradingDay.m_MarketIOCTime.GetMaxTrendUnitCount();
		bool32 bShowFirstOpen = false;
		
		ASSERT( iMaxTrendUnitCount > 1 );
		if ( iMaxTrendUnitCount <= 1 )
		{
			iMaxTrendUnitCount	=	2;
		}
		float fPixelWidthPerUnit = Rect.Width() / (float)(iMaxTrendUnitCount - 1);
		if ( bShowFirstOpen )
		{
			fPixelWidthPerUnit = Rect.Width()/(float)iMaxTrendUnitCount;
			Rect.left += (int32)(fPixelWidthPerUnit);	// 左侧偏移一个小空格给第一个开盘价
		}
		
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
					
					AxisDivide.m_DivideText1.m_lTextColor = m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
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
					AxisDivide.m_DivideText1.m_lTextColor = m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
					
					AxisDivide.m_DivideText2 = AxisDivide.m_DivideText1;
					
					//
					iIndexDivide++;
				}
				else if (uiTime % 3600 == 0)		// 一小时
				{
					AxisDivide.m_iPosPixel		= AxisNode.m_iCenterPixel;
					
					//
					AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;
					AxisDivide.m_eLineStyle = CAxisDivide::ELSSolid;
					
					//
					AxisDivide.m_eLevelText		= CAxisDivide::EDLLevel3;
					AxisDivide.m_DivideText1.m_StrText.Format(L"%02d:%02d", Time.GetHour(), Time.GetMinute());
					AxisDivide.m_DivideText1.m_uiTextAlign = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
					AxisDivide.m_DivideText1.m_lTextColor = m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
					
					AxisDivide.m_DivideText2 = AxisDivide.m_DivideText1;
					
					//
					iIndexDivide++;
				}
				else if (uiTime % 1800 == 0)		// 半小时
				{
					AxisDivide.m_iPosPixel		= AxisNode.m_iCenterPixel;
					
					//
					AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;
					AxisDivide.m_eLineStyle = CAxisDivide::ELSDot;
					
					//
					AxisDivide.m_eLevelText		= CAxisDivide::EDLLevel4;
					AxisDivide.m_DivideText1.m_StrText.Format(L"%02d:%02d", Time.GetHour(), Time.GetMinute());
					AxisDivide.m_DivideText1.m_uiTextAlign = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
					AxisDivide.m_DivideText1.m_lTextColor = m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
					
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

void CIoViewTrendArbitrage::CalcMainRegionYAxis(CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide)
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
	
	T_MerchNodeUserData* pData = GetMainData();
	if ( NULL == pData )
	{
		return;
	}
	// 获取当前显示小数点位
	int32 iSaveDec = m_Arbitrage.m_iSaveDec;

	// 获取纵坐标上几个主要的值
	CChartCurve *pCurveDependent = m_pRegionMain->GetDependentCurve();
	if (NULL == pCurveDependent)
		return;

	float fYMin = 0., fYMax = 0., fYMid = 0.;

	if (!pCurveDependent->GetYMinMax(fYMin, fYMax))
	{
		CGmtTime TimeInit	= pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time;
		CGmtTime TimeOpen	= pData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeOpen.m_Time;
		if(!m_pAbsCenterManager)
		{
			return;	
		}
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

			float fHeightSub = rectMainRegion.Height() / (iNum * 1.0);
			
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
			fPricePrevClose = GetTrendPrevClose();
			float fTmp = fPricePrevClose;
			bool32 b = pCurveDependent->PriceYToAxisY(fTmp, fPricePrevClose);
			ASSERT( b );	// 该转换应当能成功的，因为min，max已经正常生成了
		}
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
		Float2StringLimitMaxLength(StrValue, fPriceCur, iSaveDec, GetYLeftShowCharWidth()-1, true, true, false, false);
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

void CIoViewTrendArbitrage::CalcSubRegionYAxis(CChartRegion* pChartRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide)
{									  
	ASSERT(NULL != pChartRegion);
	aYAxisDivide.SetSize(0);
	
	// 计算Y方向可以容纳多少个刻度
	CRect Rect = pChartRegion->GetRectCurves();
	int32 iHeightText = pDC->GetTextExtent(_T("测试值")).cy;
	int32 iHeightSub  = iHeightText + 12;
	int32 iNum = Rect.Height()/iHeightSub + 1;

	if (iNum > 3)	iNum = 3;
	if (iNum < 2)	iNum = 2;
	
	// 获取当前显示小数点位
	int32 iSaveDec = m_Arbitrage.m_iSaveDec;
	T_MerchNodeUserData* pData = GetMainData();
	if ( NULL == pData )
	{
		return;
	}
	
	// 获取纵坐标上几个主要的值
	CChartCurve *pCurveDependent = pChartRegion->GetDependentCurve();
	if (NULL == pCurveDependent)
		return;
	
	float fYMin = 0., fYMax = 0.;
	if (!pCurveDependent->GetYMinMax(fYMin, fYMax))
		return;

	// 是否使用副图指标的水平线
	bool32 bUserFormularLine = false;
	
	// 找到这个副图的指标
	CFormularContent* pFormular= NULL;
	
	if ( NULL != pData )
	{
		for ( int32 i = 0; i < pData->aIndexs.GetSize(); i++ )
		{
			T_IndexParam* pIndex = pData->aIndexs.GetAt(i);
			if ( NULL != pIndex && pIndex->pRegion == pChartRegion )
			{
				// 看这个指标是否需要画水平线
				if ( NULL != pIndex->pContent && pIndex->pContent->numLine > 0 )
				{
					pFormular = pIndex->pContent;
					bUserFormularLine = true;
				}
				
				//
				break;
			}
		}						
	}

	//
	if ( bUserFormularLine && pFormular)
	{
		//
		for ( int32 i = 0; i < pFormular->numLine; i++ )
		{
			float fValue = pFormular->line[i];
			if ( fValue < fYMin || fValue > fYMax )
			{
				// 过滤非法值
				continue;
			}

			//
			int32 iVal = 0;
			pCurveDependent->PriceYToRegionY(fValue, iVal);
			pChartRegion->RegionYToClient(iVal);

			//
			CAxisDivide AxisDivide;
			AxisDivide.m_iPosPixel	= iVal;
			
			// 格线
			AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;		// 
			AxisDivide.m_eLineStyle = CAxisDivide::ELSDot;			// 实线
			
			// 文字
			AxisDivide.m_eLevelText						= CAxisDivide::EDLLevel1;
			
			//
			CString StrValue = Float2String(fValue, iSaveDec, true, false, false);

			AxisDivide.m_DivideText1.m_StrText			= StrValue;
			AxisDivide.m_DivideText2.m_StrText			= StrValue;
			
			AxisDivide.m_DivideText1.m_uiTextAlign		= DT_VCENTER | DT_RIGHT | DT_SINGLELINE;
			AxisDivide.m_DivideText2.m_uiTextAlign		= DT_VCENTER | DT_LEFT | DT_SINGLELINE;
			
			// zhangbo 20090927 #暂时写死颜色
			AxisDivide.m_DivideText1.m_lTextColor	= GetIoViewColor(ESCVolume);//GetIoViewColor(ESCChartAxisLine);
			AxisDivide.m_DivideText2.m_lTextColor	= GetIoViewColor(ESCVolume);//GetIoViewColor(ESCChartAxisLine);

			//
			aYAxisDivide.Add(AxisDivide);
		}

		//
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
	bool32 bTran = true;
	
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
			
			int32 i;
			for ( i = 0 ; i <= iNum; i++)
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
				if (!pCurveDependent->AxisYToRegionY(float(aiYAxis[i]), y))
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
	}
	else
	{
		bTran = false;
	}

	if ( !bTran )
	{
		for (int32 i = 0; i <= iNum; i++, fStart += fHeightSub)
		{
			if (i == iNum)
				fStart = fYMax;
			
			// 
			if (!pCurveDependent->AxisYToRegionY(fStart, y))
			{
				//
				//CNodeSequence* pNodes = pCurveDependent->GetNodes();
				//CArray<CNodeData, CNodeData&>& NodeDatas = pNodes->GetNodes();
				
				// TestNodes(NodeDatas, "c:\\Nodes.txt", m_pMerchXml->m_MerchInfo.m_StrMerchCode, m_pMerchXml->m_MerchInfo.m_iMarketId);
				//ASSERT(0);
				break;
			}
			pChartRegion->RegionYToClient(y);
			
			// 指标大数据时不用显示小数点了
			if ( fStart >= 1000.0 || fStart <= -1000.0 )
			{
				iSaveDec = 0;
			}
			// xl 0001762 限制副Y轴显示字符长度 - 右侧留一个空格
			//CString StrValue = Float2String(fStart, iSaveDec, true, false, false);
			CString StrValue;
			Float2StringLimitMaxLength(StrValue, fStart, iSaveDec, GetYLeftShowCharWidth()-1, true, true, false, false);
			
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

void CIoViewTrendArbitrage::OnKeyDown()
{
	//UINT uiTimeStart = timeGetTime();

	if (OnZoomOut())
	{
		m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
		for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
		{
			T_SubRegionParam &SubRegion = m_SubRegions[i];
			SubRegion.m_pSubRegionMain->SetDrawFlag(CRegion::KDrawFull);
		}
	}

	//UINT uiTimeEnd = timeGetTime();
}

void CIoViewTrendArbitrage::OnKeyUp()
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

void CIoViewTrendArbitrage::OnKeyHome()
{
	if ( NULL == m_pRegionMain )
	{
		return;
	}

	//
	if ( IsCtrlPressed() || m_pRegionMain->IsActiveCross() )
	{
		/*
		// 按着Ctrl 跳到第一根
		if ( NULL == m_pRegionMain->GetDependentCurve() )
		{
			return;
		}

		//
		SetAccurateZoomFlag(true);		

		// Home 键.跳到第一根K 线
		if ( !m_pRegionMain->IsActiveCross() )
		{
			m_pRegionMain->ActiveCross(true);
			
			for(int32 i = 0 ; i < m_SubRegions.GetSize(); i++)
			{
				m_SubRegions.GetAt(i).m_pSubRegionMain->ActiveCross(true);
			}
		}		
	
		//
		m_pRegionMain->m_iNodeCross = 0;						
		
		CKLine KLine; 
		CNodeData NodeData;
		
		m_pRegionMain->GetDependentCurve()->GetNodes()->GetAt(m_pRegionMain->m_iNodeCross, NodeData);
		NodeData2KLine(NodeData, KLine);
		
		// 记录下当前这根K线
		m_KLineCrossNow = KLine;
		
		OnKeyLeftRightAdjustIndex();			
		
		// 显示刷新
		m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
		for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
		{
			m_SubRegions[i].m_pSubRegionMain->SetDrawFlag(CRegion::KDrawFull);
		}
		
		RedrawWindow();
		*/
		// 跳到最后一根
		if ( NULL == m_pRegionMain->GetDependentCurve() )
		{
			return;
		}
	
		//		
		if ( !m_pRegionMain->IsActiveCross() )
		{
			m_pRegionMain->ActiveCross(true);
			
			for(int32 i = 0 ; i < m_SubRegions.GetSize(); i++)
			{
				m_SubRegions.GetAt(i).m_pSubRegionMain->ActiveCross(true);
			}
		}	
	
		m_pRegionMain->m_iNodeCross = 0;						
		
		int32 x = 0;
		m_pRegionMain->GetDependentCurve()->CurvePosToRegionX(m_pRegionMain->m_iNodeCross,x);
		m_pRegionMain->RegionXToClient(x);
		m_pRegionMain->BeginOverlay(false);			
		m_pRegionMain->DrawCross(m_pRegionMain->GetViewData()->GetOverlayDC(),x,0,CChartRegion::EDCLeftRightKey);
		m_pRegionMain->EndOverlay();
		
		OnKeyLeftRightAdjustIndex();			
	}
	else
	{
		// 选择上一个指标
		ChangeToNextIndex(true);
	}	
}

void CIoViewTrendArbitrage::OnKeyEnd()
{
	if ( NULL == m_pRegionMain )
	{
		return;
	}

	if ( IsCtrlPressed() || m_pRegionMain->IsActiveCross() )
	{
		// 跳到最后一根
		if ( NULL == m_pRegionMain->GetDependentCurve() )
		{
			return;
		}

		//		
		if ( !m_pRegionMain->IsActiveCross() )
		{
			m_pRegionMain->ActiveCross(true);
			
			for(int32 i = 0 ; i < m_SubRegions.GetSize(); i++)
			{
				m_SubRegions.GetAt(i).m_pSubRegionMain->ActiveCross(true);
			}
		}	
		
		m_pRegionMain->m_iNodeCross = m_pRegionMain->GetDependentCurve()->GetNodes()->GetNodes().GetSize() - 1;						
		
		int32 x = 0;
		m_pRegionMain->GetDependentCurve()->CurvePosToRegionX(m_pRegionMain->m_iNodeCross,x);
		m_pRegionMain->RegionXToClient(x);
		m_pRegionMain->BeginOverlay(false);			
		m_pRegionMain->DrawCross(m_pRegionMain->GetViewData()->GetOverlayDC(),x,0,CChartRegion::EDCLeftRightKey);
		m_pRegionMain->EndOverlay();
		
		OnKeyLeftRightAdjustIndex();			
	}
	else
	{
		// 选择下一个指标
		ChangeToNextIndex(false);
	}
}

void CIoViewTrendArbitrage::OnKeyLeftAndCtrl()
{
	OnKeyLeftAndCtrl(1);
}

void CIoViewTrendArbitrage::OnKeyLeftAndCtrl( int32 iRepCnt )
{
	// 取主图信息
	if (m_MerchParamArray.GetSize() <= 0)
		return;
	
	T_MerchNodeUserData *pMainData = m_MerchParamArray[0];
	if (pMainData->m_aKLinesFull.GetSize() <= 0)
		return;
	
	if ( iRepCnt < 1 )
	{
		iRepCnt = 1;
	}
	// 
	{
		int32 iShowStart = pMainData->m_iShowPosInFullList;
		int32 iShowEnd = pMainData->m_iShowPosInFullList + pMainData->m_iShowCountInFullList - 1;
		ASSERT(iShowStart <= iShowEnd);
		ASSERT(iShowStart >= 0 && iShowStart < pMainData->m_aKLinesFull.GetSize());
		ASSERT(iShowEnd >= 0 && iShowEnd < pMainData->m_aKLinesFull.GetSize());
	}
	
	// 
	if (pMainData->m_iShowPosInFullList <= 0)	// 到最左边了， 不需要请求数据， 除非用户按上下键放缩
	{
		return;
	}
	else
	{	
		// 更新主图显示数据
		// 最少要移动一个像素
		ASSERT( m_pRegionMain->m_aXAxisNodes[0].m_fPixelWidthPerUnit > 0.0 );
		
		int32 iMove = iRepCnt;
		
		int32 iUnitPerPixel = (int32)(1 / m_pRegionMain->m_aXAxisNodes[0].m_fPixelWidthPerUnit);
		if ( iUnitPerPixel > 1 )
		{
			// 1个像素有若干个单位
			iMove = iRepCnt * iUnitPerPixel;
		}

		// 左边移动，如果能移动，就会造成调整右侧空格的局面
		
		int32 iShowPosInFullList = pMainData->m_iShowPosInFullList - iMove;
		int32 iShowCountInFullList = pMainData->m_iShowCountInFullList;
		// 调整最左边的数据
		if ( iShowPosInFullList < 0 )
		{
			iShowPosInFullList	=	0;
		}
		
		if (!ChangeMainDataShowData(*pMainData, iShowPosInFullList, iShowCountInFullList))
		{
			//ASSERT(0);
			return;
		}

		// 显示刷新
		m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
		for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
		{
			m_SubRegions[i].m_pSubRegionMain->SetDrawFlag(CRegion::KDrawFull);
		}
		
		RedrawWindow();
	}	
}

void CIoViewTrendArbitrage::OnKeyLeft()
{
	if (NULL == m_pRegionMain->GetDependentCurve())
	{
		return;
	}

	int32 iMove = 1;
	if ( IsCtrlPressed() )
	{
		iMove = KDefaultCtrlMoveSpeed;
	}

// 	if ( IsCtrlPressed())
// 	{
// 		OnKeyLeftAndCtrl(KDefaultCtrlMoveSpeed);
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
				
				if ( !m_pRegionMain->FlyCross(iNodeCross, true))
				{
					return;
				}

				//
				int32 x = 0;
				m_pRegionMain->GetDependentCurve()->CurvePosToRegionX(m_pRegionMain->m_iNodeCross,x);
				m_pRegionMain->RegionXToClient(x);
				m_pRegionMain->BeginOverlay(false);			
				m_pRegionMain->DrawCross(m_pRegionMain->GetViewData()->GetOverlayDC(),x,0,CChartRegion::EDCLeftRightKey);
				m_pRegionMain->EndOverlay();

			}

			RedrawWindow();
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
			m_pRegionMain->DrawCross(m_pRegionMain->GetViewData()->GetOverlayDC(),x,0,CChartRegion::EDCLeftRightKey);
			m_pRegionMain->EndOverlay();
		}
		
		OnKeyLeftRightAdjustIndex();		
	}
}
				   
void CIoViewTrendArbitrage::OnKeyRightAndCtrl()
{
	// 取主图信息
	OnKeyRightAndCtrl(1);
}

void CIoViewTrendArbitrage::OnKeyRightAndCtrl( int32 iRepCnt )
{
	// 取主图信息
	if (m_MerchParamArray.GetSize() <= 0)
		return;
	
	T_MerchNodeUserData *pMainData = m_MerchParamArray[0];
	if (pMainData->m_aKLinesFull.GetSize() <= 0)
		return;

	if ( iRepCnt < 1 )
	{
		iRepCnt = 1;
	}

	// 
	{
		int32 iShowStart = pMainData->m_iShowPosInFullList;
		int32 iShowEnd = pMainData->m_iShowPosInFullList + pMainData->m_iShowCountInFullList - 1;
		ASSERT(iShowStart <= iShowEnd);
		ASSERT(iShowStart >= 0 && iShowStart < pMainData->m_aKLinesFull.GetSize());
		ASSERT(iShowEnd >= 0 && iShowEnd < pMainData->m_aKLinesFull.GetSize());
	}
	
	// 
	if (pMainData->m_iShowPosInFullList + 1 + pMainData->m_iShowCountInFullList - 1 >= pMainData->m_aKLinesFull.GetSize())	// 到最右边了， 不需要请求数据， 除非用户按上下键放缩
	{
		return;
	}
	else
	{
		// 更新主图显示数据
		// 最少要移动一个像素
		ASSERT( m_pRegionMain->m_aXAxisNodes[0].m_fPixelWidthPerUnit > 0.0 );
		
		int32 iMove = iRepCnt;

		int32 iUnitPerPixel = (int32)(1 / m_pRegionMain->m_aXAxisNodes[0].m_fPixelWidthPerUnit);
		if ( iUnitPerPixel > 1 )
		{
			// 1个像素有若干个单位
			iMove = iRepCnt * iUnitPerPixel;
		}

		int32 iShowPosInFullList = pMainData->m_iShowPosInFullList + iMove;
		int32 iShowCountInFullList = pMainData->m_iShowCountInFullList;
		// 调整最右边的数据
		if ( iShowCountInFullList + iShowPosInFullList > pMainData->m_aKLinesFull.GetSize() )
		{
			iShowPosInFullList	=	pMainData->m_aKLinesFull.GetSize() - iShowCountInFullList;
		}

		if (!ChangeMainDataShowData(*pMainData, iShowPosInFullList, iShowCountInFullList))
		{
			//ASSERT(0);
			return;
		}
		
		// 显示刷新
		m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
		for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
		{
			m_SubRegions[i].m_pSubRegionMain->SetDrawFlag(CRegion::KDrawFull);
		}
		
		RedrawWindow();
	}
}

void CIoViewTrendArbitrage::OnKeyRight()
{
	if ( NULL == m_pRegionMain->GetDependentCurve() )
	{
		return;
	}

	int32 iMove = 1;
	if ( IsCtrlPressed() )
	{
		iMove = KDefaultCtrlMoveSpeed;
	}

// 	if ( IsCtrlPressed() )
// 	{
// 		OnKeyRightAndCtrl(KDefaultCtrlMoveSpeed);
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
				
				if ( ! m_pRegionMain->FlyCross ( iNodeCross, true))
				{
					return;
				}
			
				int32 x = 0;
				m_pRegionMain->GetDependentCurve()->CurvePosToRegionX(m_pRegionMain->m_iNodeCross,x);
				m_pRegionMain->RegionXToClient(x);
				m_pRegionMain->BeginOverlay(false);			
				m_pRegionMain->DrawCross(m_pRegionMain->GetViewData()->GetOverlayDC(),x,0,CChartRegion::EDCLeftRightKey);
				m_pRegionMain->EndOverlay();
			}
		}
		else
		{
			if ( ! m_pRegionMain->FlyCross (iNodeCross, true))
			{
				return;
			}
		
			int32 x = 0;
			m_pRegionMain->GetDependentCurve()->CurvePosToRegionX(m_pRegionMain->m_iNodeCross,x);
			m_pRegionMain->RegionXToClient(x);
			m_pRegionMain->BeginOverlay(false);			
			m_pRegionMain->DrawCross(m_pRegionMain->GetViewData()->GetOverlayDC(),x,0,CChartRegion::EDCLeftRightKey);
			m_pRegionMain->EndOverlay();
		}
		
		OnKeyLeftRightAdjustIndex();		
	}
}

void CIoViewTrendArbitrage::OnKeySpace()
{
	CIoViewChart::OnKeySpace();
	
	
}

void CIoViewTrendArbitrage::RequestViewData()
{
	//
	T_MerchNodeUserData *pMainData = GetMainData();
	if (NULL == pMainData)
		return;

	// 不考虑main商品数据的请求，只请求实际的商品数据
	// 由于是合成的数据，并不清楚需要满足当前要显示数量的点需要多少数据，只能猜了
	for (int32 i = 1; i < m_MerchParamArray.GetSize(); i++)
	{
		//计算是否应当申请数据 - 数据量比较单位使用基本的 K线 周期单位
		T_MerchNodeUserData *pMerchData = m_MerchParamArray[i];
		if (NULL == pMerchData || NULL == pMerchData->m_pMerchNode)
			continue;

		if ( !pMerchData->m_TrendTradingDayInfo.m_bInit )
		{
			ASSERT( 0 );	// 没有初始化时间
			continue;
		}
		
		//
		{
			//
			CMmiReqMerchKLine info; 
			info.m_eKLineTypeBase	= EKTBMinute;								// 使用原始K线周期
			info.m_iMarketId		= pMerchData->m_pMerchNode->m_MerchInfo.m_iMarketId;
			info.m_StrMerchCode		= pMerchData->m_pMerchNode->m_MerchInfo.m_StrMerchCode;
			
			info.m_eReqTimeType		= ERTYSpecifyTime;
			info.m_TimeStart		= pMerchData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time;
			info.m_TimeEnd			= pMerchData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeEnd.m_Time;
			
			DoRequestViewData(info);
		}
		
		// 请求实时数据
		CMmiReqRealtimePrice MmiReqRealtimePrice;
		MmiReqRealtimePrice.m_iMarketId		= pMerchData->m_pMerchNode->m_MerchInfo.m_iMarketId;
		MmiReqRealtimePrice.m_StrMerchCode	= pMerchData->m_pMerchNode->m_MerchInfo.m_StrMerchCode;		
		DoRequestViewData(MmiReqRealtimePrice);

	}
}

void CIoViewTrendArbitrage::OnWeightTypeChange()
{
	
}

bool32 CIoViewTrendArbitrage::GetChartXAxisSliderText1( OUT CString &StrSlider, CAxisNode &AxisNode )
{
	return CIoViewChart::GetChartXAxisSliderText1(StrSlider, AxisNode);
}

bool32 CIoViewTrendArbitrage::FromXmlInChild( TiXmlElement *pTiXmlElement )
{
	// 初始化默认副图索引
	ASSERT( NULL != pTiXmlElement );

	const char *pszAttri = pTiXmlElement->Attribute(KXMLAttriIoViewTrendSubIndex);
	if ( NULL != pszAttri )
	{
		
	}

	TiXmlElement* pArbitrageElement = pTiXmlElement->FirstChildElement("Arbitrage");
	
	if (NULL != pArbitrageElement)
	{
		bool32 bOK = m_Arbitrage.FromXml(pArbitrageElement,m_pAbsCenterManager);
		if (!bOK)
		{
			//ASSERT(0);
		}
		else
		{
			// 防止不同步
			CArbitrageManage::Instance()->AddArbitrage(m_Arbitrage);
		}
	}

	return true;
}

CString CIoViewTrendArbitrage::ToXmlInChild()
{
	CString StrRet;

	// 保存副图指标

	return StrRet;
}

CString CIoViewTrendArbitrage::ToXmlEleInChild()
{
	CString StrRet = m_Arbitrage.ToXml();
	return StrRet;
}

void CIoViewTrendArbitrage::GetCurrentIndexNameArray( OUT CStringArray &aIndexNames )
{
	aIndexNames.RemoveAll();

	for ( int32 j=0; j < m_MerchParamArray.GetSize() ; j++ )
	{
		T_MerchNodeUserData *pData = m_MerchParamArray[j];
		if ( NULL == pData )
		{
			continue;;
		}
		
		for ( int32 i = pData->aIndexs.GetSize()-1 ; i >= 0 ; i-- )
		{
			T_IndexParam *pParam = pData->aIndexs[i];
			aIndexNames.Add(pParam->strIndexName);
		}
	}
}

void CIoViewTrendArbitrage::ChangeToNextIndex(bool32 bPre)
{	
	if ( m_SubRegions.GetSize() <= 0 )
	{
		return;
	}

	if ( m_MerchParamArray.GetSize() <= 0 )
	{
		return;
	}

	T_MerchNodeUserData* pData = m_MerchParamArray[0];
	if ( NULL == pData )
	{
		return;
	}
	
	T_SubRegionParam Param = m_SubRegions.GetAt(0);
	CChartRegion* pRegionActive = Param.m_pSubRegionMain;
	if ( NULL == pRegionActive )
	{
		return;
	}

	// 常用指标的集合
	CStringArray aFormularNames;
	int32 iPos;
	GetOftenFormulars(pRegionActive, aFormularNames, iPos, false);
	
	if ( aFormularNames.GetSize() <= 0 )
	{
		return;			
	}
	// 当前的指标名称
	CString StrFormularPre;
	
	int32 i = 0;
	for ( i = 0; i < pData->aIndexs.GetSize(); i++ )
	{
		if ( pData->aIndexs[i]->pRegion == pRegionActive )
		{
			StrFormularPre = pData->aIndexs[i]->pContent->name;
			break;
		}				
	}
	
	//
	CString StrFormularNow;
	int32 iFindPos = -1;

	for ( i = 0; i < aFormularNames.GetSize(); i++ )
	{
		if ( StrFormularPre == aFormularNames.GetAt(i) )
		{
			iFindPos = i;
			break;
		}
	}

	// 先清空当前的
	ClearRegionIndex(pRegionActive);

	//
	int32 iPosNow = 0;
	if ( -1 == iFindPos )
	{
		iPosNow = 0;
	}
	else
	{
		if ( bPre )
		{
			iPosNow = iFindPos - 1;
		}
		else
		{
			iPosNow = iFindPos + 1;
		}
	}
	
	//
	if ( iPosNow < 0 )
	{
		iPosNow = aFormularNames.GetSize() - 1;
	}

	if ( iPosNow >= aFormularNames.GetSize() )
	{
		iPosNow = 0;
	}

	//
	StrFormularNow = aFormularNames.GetAt(iPosNow);

	//
	AddIndexToSubRegion(StrFormularNow, pRegionActive);
}

void CIoViewTrendArbitrage::OnChartDBClick( CPoint ptClick, int32 iNodePos )
{
	T_MerchNodeUserData *pMainData = GetMainData();
	if ( NULL == pMainData )
	{
		return;
	}
	
	if ( NULL == m_pRegionMain || m_pRegionMain->m_aXAxisNodes.GetSize() < 1)
	{
		return ;
	}
	
	CChartCurve* pDependCurve = m_pRegionMain->GetDependentCurve();
	
	if ( NULL == pDependCurve)
	{
		return ;
	}
	
	// 	
	CNodeSequence* pNode = pDependCurve->GetNodes();	
	
	if ( NULL == pNode)
	{
		return;
	}
	
	if ( m_pRegionMain->m_aXAxisNodes.GetSize() < pNode->GetNodes().GetSize() )
	{
		ASSERT( 0 );
		return;
	}

	if ( m_pRegionMain->m_aXAxisNodes.GetSize() <= iNodePos || iNodePos < 0 )
	{
		ASSERT( 0 );
		return;
	}
	
	
}

bool32 CIoViewTrendArbitrage::ChangeMainDataShowData( T_MerchNodeUserData &MainMerchNode, int32 iShowPosInFullList, int32 iShowCountInFullList )
{
	if (!MainMerchNode.UpdateShowData(iShowPosInFullList, iShowCountInFullList))
	{
		//ASSERT(0);
		return false;
	}
	
	
	{
		// 重新计算坐标
		UpdateAxisSize();
		
	}

	return true;
}

bool32 CIoViewTrendArbitrage::CalcKLinePriceBaseValue( CChartCurve *pCurve, OUT float &fPriceBase )
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
		if ( NULL == pData || !pData->m_TrendTradingDayInfo.m_bInit )
		{
			//ASSERT( 0 );
			return false;
		}
		
		if ( pData->bMainMerch )
		{
			fPriceBase = GetTrendPrevClose();
		}
		else
		{
			ASSERT( 0 );
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

void CIoViewTrendArbitrage::OnShowDataTimeRangeChanged( T_MerchNodeUserData *pData )
{
	if ( m_MerchParamArray.GetSize() <= 0 || m_MerchParamArray[0] == NULL )
	{
		return;
	}

	if ( pData == m_MerchParamArray[0] )
	{
	}
}


CChartCurve			* CIoViewTrendArbitrage::GetCurIndexCurve( CChartRegion *pRegionParent )
{
	if ( NULL == pRegionParent )
	{
		pRegionParent = m_pRegionMain;
	}
	if ( NULL == pRegionParent )
	{
		return NULL;
	}

	int32 i, iSize = pRegionParent->GetCurveNum();
	for ( i = 0; i < iSize; i ++ )
	{
		CChartCurve* pCurve = pRegionParent->GetCurve(i);
		if ( CheckFlag(pCurve->m_iFlag,CChartCurve::KTypeIndex))
		{
			return pCurve;	// 返回找到的第一条，后面的不考虑
		}
	}
	return NULL;
}

void CIoViewTrendArbitrage::AdjustCtrlFloatContent()
{
	
}

void CIoViewTrendArbitrage::OnArbitrageAdd( const CArbitrage& stArbitrage )
{
	
}

void CIoViewTrendArbitrage::OnArbitrageDel( const CArbitrage& stArbitrage )
{
	// 这删除了能做什么？
}

void CIoViewTrendArbitrage::OnArbitrageModify( const CArbitrage& stArbitrageOld, const CArbitrage& stArbitrageNew )
{
	if ( m_Arbitrage == stArbitrageOld )
	{
		SetArbitrage(stArbitrageNew);
	}
}

void CIoViewTrendArbitrage::OnPickChart( CChartRegion *pRegion, int32 x, int32 y, int32 iFlag )
{
	
}

bool32 CIoViewTrendArbitrage::SetArbitrage( IN const CArbitrage &arb )
{
	if ( !m_bFromXml && m_Arbitrage == arb )
	{
		return true;	// 一模一样的套利就不用了
	}

	MerchArray aMerchs;
	bool32 bMerch = GetArbitrageMerchs(arb, aMerchs);
	ASSERT( bMerch );

	// 清除原有商品的数据，并设立关注，请求数据
	
	// 清除所有商品data，重建data
	int32 i;
	for ( i=m_MerchParamArray.GetSize()-1; i >= 1 ; --i )
	{
		DeleteMerchData(i);
	}
	m_aSmartAttendMerchs.RemoveAll();
	for (i=0; i < aMerchs.GetSize() ; ++i)
	{
		T_MerchNodeUserData *pData = GetMerchData(aMerchs[i], true);
		ASSERT( NULL != pData && !pData->bMainMerch );
		CSmartAttendMerch att;
		att.m_pMerch = aMerchs[i];
		att.m_iDataServiceTypes = m_iDataServiceType;
		m_aSmartAttendMerchs.Add(att);
	}

	m_Arbitrage = arb;

	UpdateAllMerchDataTradeTimes();
	CombineMainDataTradeTime();

	if ( m_IndexPostAdd.id >= 0 )
	{
		m_IndexPostAdd.id = -1;
		AddIndex(m_IndexPostAdd.pRegion,m_IndexPostAdd.StrIndexName);
	}

	for ( i=0; i < aMerchs.GetSize() ; ++i )
	{
		OnVDataMerchKLineUpdate(aMerchs[i]);
	}

	CalcArbitrage();
	SetCurveTitle(GetMainData());
	ReDrawAysnc();

	RequestViewData();
	
	return true;
}

void CIoViewTrendArbitrage::OnTimer( UINT nIDEvent )
{
	if ( KTimerIdCalcArbitrage == nIDEvent )
	{
		// 计算套利，因为套利有多个商品的更新，所有全部都是延时计算
		CalcArbitrage();
		KillTimer(nIDEvent);
		if ( NULL != m_pRegionMain )
		{
			m_pRegionMain->SetDrawFlag(CChartRegion::KDrawFull);
			Invalidate();
		}
	}
	CIoViewChart::OnTimer(nIDEvent);
}

void CIoViewTrendArbitrage::CalcArbitrageAsyn()
{
	SetTimer(KTimerIdCalcArbitrage, KTimerPeriodCalcArbitrage, NULL);
}

void CIoViewTrendArbitrage::CancelCalcArbAsyn()
{
	KillTimer(KTimerIdCalcArbitrage);
}

bool32 CIoViewTrendArbitrage::CalcArbitrage()
{
	CancelCalcArbAsyn();

	T_MerchNodeUserData *pMainData = GetMainData(true);
	if ( NULL == pMainData )
	{
		ASSERT( 0 );
		return false;
	}

	MerchArray aMerchs;
	if ( !GetArbitrageMerchs(m_Arbitrage, aMerchs) || !pMainData->m_TrendTradingDayInfo.m_bInit )
	{
		// 不能获取商品信息，清除显示数据
		// 没有初始化好的
		ClearLocalData(false);

		return true;	// 算是计算成功了:)，将整个视图带到了一个起始阶段~~
	}

	int32 i=0;
	CArray<T_MerchNodeUserData *, T_MerchNodeUserData *> aDatas;
	for ( i=0; i < aMerchs.GetSize() ; ++i )
	{
		T_MerchNodeUserData *pData = GetMerchData(aMerchs[i]);
		if ( NULL != pData )
		{
			aDatas.Add(pData);
		}
		else
		{
			ASSERT( 0 );
		}
	}
	if ( aDatas.GetSize() != aMerchs.GetSize() )
	{
		ASSERT( 0 );

		ClearLocalData(false);
		return true; //// 算是计算成功了:)，将整个视图带到了一个起始阶段~~
	}

	// 处理分时K线，补充没分钟点
	// 由于存在某些K线没有更新分钟点，所以总是更新所有的K线
	DWORD dwTime1 = timeGetTime();
	DWORD dwTime2;

	CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();
	for ( i=0; i < aDatas.GetSize() ; ++i )
	{
		T_MerchNodeUserData *pData = aDatas[i];
		FillSubMerchTrendNodes(*pData, TimeNow, pData->m_aKLinesCompare.GetData(), pData->m_aKLinesCompare.GetSize());
	}

	dwTime2 = timeGetTime();
	TRACE(_T("套利分时: 组成Full %d ms\r\n"), dwTime2-dwTime1);
	dwTime1 = dwTime2;

	bool32 bOk = true;
	CArray<CKLine, CKLine> aKlineLegA, aKlineLegB, aKlineFull;

	CArbitrageManage::Merch2KLineMap mapKLinePtrs;
	for ( i=0; i < aDatas.GetSize() ; ++i )
	{
		mapKLinePtrs[ aDatas[i]->m_pMerchNode ] = &aDatas[i]->m_aKLinesFull;
	}

	if ( !m_Arbitrage.IsNeedCMerch() && aDatas.GetSize() >= 2 )
	{
		// 两腿
		CArray<CKLine, CKLine> aKlineC;
		if ( aDatas[0]->m_aKLinesFull.GetSize() > 0 && aDatas[1]->m_aKLinesFull.GetSize() > 0 )
		{
			bOk = CArbitrageManage::Instance()->CombineArbitrageKLine(m_Arbitrage
											, mapKLinePtrs
											, aKlineLegA
											, aKlineLegB
											, aKlineFull);
		}
		else
		{
			pMainData->m_aKLinesFull.RemoveAll();
			bOk = true;
		}
	}
	else if ( aDatas.GetSize() >= 3 )
	{
		// 蝶式
		if ( aDatas[0]->m_aKLinesFull.GetSize() > 0 && aDatas[1]->m_aKLinesFull.GetSize() > 0 && aDatas[2]->m_aKLinesFull.GetSize() > 0 )
		{
			bOk = CArbitrageManage::Instance()->CombineArbitrageKLine(m_Arbitrage
											, mapKLinePtrs
											, aKlineLegA
											, aKlineLegB
											, aKlineFull);
		}
		else
		{
			pMainData->m_aKLinesFull.RemoveAll();
			bOk = true;
		}
	}
	else
	{
		ASSERT( 0 );
		bOk = false;
	}

	dwTime2 = timeGetTime();
	TRACE(_T("套利分时: 合并 %d ms\r\n"), dwTime2-dwTime1);
	dwTime1 = dwTime2;

	if ( bOk )
	{
		// 修理最高最低价
		for ( int32 i=0; i < aKlineFull.GetSize() ; ++i )
		{
			CKLine &kline = aKlineFull[i];
			kline.m_fPriceHigh	= max(kline.m_fPriceOpen, kline.m_fPriceClose);
			kline.m_fPriceHigh	= max(kline.m_fPriceHigh, kline.m_fPriceAvg);
			kline.m_fPriceLow	= min(kline.m_fPriceOpen, kline.m_fPriceClose);
			kline.m_fPriceLow	= min(kline.m_fPriceLow, kline.m_fPriceAvg);
		}

		bOk = FillTrendNodes(*pMainData, m_pAbsCenterManager->GetServerTime(), aKlineFull.GetData(), aKlineFull.GetSize());

		if ( bOk )
		{
			// 计算公式等
			//********************************************************************************************************************************************
			// 更新的指标
			for (int32 j = 0; j < pMainData->aIndexs.GetSize(); j++)
			{
				T_IndexParam* pIndex = pMainData->aIndexs.GetAt(j);
				bool32 b = g_formula_compute(this, this, (CChartRegion*)pIndex->pRegion, pMainData, pIndex, 0, pMainData->m_aKLinesFull.GetSize()); 				
				if ( !b )
				{
					// 源数据是空的，清除原来的数据
					if ( NULL != pIndex )
					{		
						for (int32 j = 0; j < pIndex->m_aIndexLines.GetSize(); j++)
						{
							CIndexNodeList &IndexNodeList = pIndex->m_aIndexLines[j];
							IndexNodeList.m_aNodesFull.RemoveAll();
							((CNodeSequence*)IndexNodeList.m_pNodesShow)->m_aNodes.SetSize(0);
						}
					}
				}
			}
			
			
			// 生成当前显示的数据
			if (!pMainData->UpdateShowData(0, pMainData->m_aKLinesFull.GetSize()))
			{
				//ASSERT(0);
				bOk = false;
			}
		}
	}

	if ( !bOk )
	{
		ClearLocalData(false);
	}

	dwTime2 = timeGetTime();
	TRACE(_T("套利分时: 其它 %d ms\r\n"), dwTime2-dwTime1);
	dwTime1 = dwTime2;

	// 线的后缀数字需要更新下
	if ( NULL != m_pRegionMain )
	{
		if ( m_pRegionMain->IsActiveCross() )
		{
			CPoint pt = m_pRegionMain->GetPtCross();
			m_pRegionMain->DrawIndexValueByMouseMove(pt.x, pt.y, false);
		}
		else
		{
			m_pRegionMain->DrawIndexValueByMouseMove(0, 0, true);
		}
	}

	return bOk;
}

T_MerchNodeUserData * CIoViewTrendArbitrage::GetMerchData( CMerch *pMerch, bool32 bCreateIfNotExist/*=false*/ )
{
	if ( NULL == pMerch )
	{
		return NULL;
	}

	T_MerchNodeUserData *pMainData = GetMainData(true);
	if ( NULL == pMainData )
	{
		ASSERT( 0 );
		return NULL;
	}

	// 只在非main中取数据
	for ( int32 i=1; i < m_MerchParamArray.GetSize() ; ++i )
	{
		ASSERT( NULL != m_MerchParamArray[i] );
		if ( m_MerchParamArray[i]->m_pMerchNode == pMerch )
		{
			ASSERT( !m_MerchParamArray[i]->bMainMerch );
			return m_MerchParamArray[i];
		}
	}
	
	if ( bCreateIfNotExist )
	{
		// 要创建这个节点了
		if ( pMainData->m_pMerchNode == pMerch )
		{
			// main中的MerchNode不能与现在要求的相同，否则chart会有冲突
			pMainData->m_pMerchNode = CIoViewBase::GetNextMerch(pMerch, true);
			if ( pMainData->m_pMerchNode == pMerch )
			{
				// 还是相同，那只有循环取了
				for ( int32 i=0; i < m_pAbsCenterManager->GetMerchManager().m_BreedListPtr.GetSize() ; ++i )
				{
					CBreed *pBreed = m_pAbsCenterManager->GetMerchManager().m_BreedListPtr[i];
					for ( int32 j=0; j < pBreed->m_MarketListPtr.GetSize() ; ++j )
					{
						CMarket *pMarket = pBreed->m_MarketListPtr[j];
						for ( int32 k=0; k < pMarket->m_MerchsPtr.GetSize() ; ++k )
						{
							if ( pMerch != pMarket->m_MerchsPtr[k] )
							{
								pMainData->m_pMerchNode = pMarket->m_MerchsPtr[k];
								break;
							}
						}
					}
				}
			}
			if ( pMainData->m_pMerchNode == pMerch )
			{
				ASSERT( 0 );
				return NULL;	// 整个只有一个商品，显然是比较不了的
			}
		}
		bool32 bInherit;
		T_MerchNodeUserData *pData = NewMerchData(pMerch, false, bInherit);
		ASSERT( NULL != pData );
		if ( NULL != pData )
		{
			if ( !bInherit )
			{
				// 创建需要的线 画等
				// 暂不需要线，两腿叠加的使用额外的线
			}

			// 初始化交易时间信息
			// 获取该商品走势相关的数据
			UpdateMerchDataTradeTime(pData);
			
			
			// 加入关注 - 在SetArb时就设置好关注的商品
			
			return pData;
			// 此时还是没有设置标题的
		}
	}
	return NULL;
}

bool32 CIoViewTrendArbitrage::ResetMerchUserData( T_MerchNodeUserData *pMerchData )
{
	if ( NULL == pMerchData || pMerchData->bMainMerch )
	{
		ASSERT( 0 );
		return false;
	}

	const T_MerchNodeUserData *pMainData = GetMainData(true);
	if ( NULL == pMainData )
	{
		ASSERT( 0 );
		return false;
	}

	// 清除数据
	pMerchData->m_aTicksCompare.RemoveAll();
	pMerchData->m_aKLinesCompare.RemoveAll();
	pMerchData->m_aKLinesFull.RemoveAll();
	
	ASSERT( NULL == pMerchData->m_pKLinesShow );
	ASSERT( 0 == pMerchData->aIndexs.GetSize() );

	// 设置参数 
	pMerchData->m_eTimeIntervalCompare = pMainData->m_eTimeIntervalCompare;
	pMerchData->m_eTimeIntervalFull		= pMainData->m_eTimeIntervalFull;
	pMerchData->m_iShowPosInFullList	= 0;
	pMerchData->m_iShowCountInFullList	= 0;
	pMerchData->m_iTimeUserMultipleDays	= pMainData->m_iTimeUserMultipleDays;
	pMerchData->m_iTimeUserMultipleMinutes	= pMainData->m_iTimeUserMultipleMinutes;

	return true;
}

//lint --e{429}
bool32 CIoViewTrendArbitrage::InitMainUserData()
{
	T_MerchNodeUserData *pMainData = m_MerchParamArray.GetSize()>0? m_MerchParamArray[0] : NULL;
	if ( NULL == pMainData && NULL != m_pRegionMain )
	{
		// 创建一个
		ASSERT( m_MerchParamArray.GetSize() == 0 );
		bool32 bInhert;
		// 任意选一个商品，mainData中的商品对于这个视图无意义
		CMerch *pMerch = NULL;
		if ( NULL != m_Arbitrage.m_MerchA.m_pMerch )
		{
			pMerch = m_Arbitrage.m_MerchA.m_pMerch;
		}
		else
		{
			pMerch = m_pAbsCenterManager->GetMerchManager().m_BreedListPtr[0]->m_MarketListPtr[0]->m_MerchsPtr[0];
			// 如果这个商品没有的话，那么即使这里不出异常，软件也不会表现正常
		}
		T_MerchNodeUserData* pData = NewMerchData(pMerch, true, bInhert);
		
		// 保持默认分钟线
		pData->m_eTimeIntervalCompare = pData->m_eTimeIntervalFull = ENTIMinute;
		
		if ( !bInhert )
		{
			CNodeSequence* pNodes = CreateNodes();
			pNodes->SetUserData(pData);
			pData->m_pKLinesShow = pNodes;

// 			默认调出收盘价价差图，副图默认MACD
// 				1、	收盘价价差图：用收盘价价差做线性图
// 				2、	均价价差图：用均价价差做线性图
// 				3、	套利K线图：用收盘价价差、开盘价价差、均价价差做K线，收盘价价差大于开盘价价差画红线，反之画绿线。
// 				4、	两腿叠加图：将套利模型中的商品的收盘价分别用线性图画出，商品A显示价格，其余的商品用百分比坐标显示
			// 这个图的主线拥有收盘价差，均价差，套利K，两腿叠加线的画法区别

			// new 1 curves/nodes/...
			CChartCurve* pCurve = m_pRegionMain->CreateCurve(CChartCurve::KDependentCurve|CChartCurve::KRequestCurve|CChartCurve::KTypeTrend|CChartCurve::KYTransformByClose|CChartCurve::KUseNodesNameAsTitle/*|CChartCurve::KDonotPick*/);		 
			CChartDrawer* pKLineDrawer = new CChartDrawer(*this, (CChartDrawer::E_ChartDrawStyle)CChartDrawer::EKDSTrendPrice);
			
			if ( -1 != m_iDrawTypeXml )
			{
				//pKLineDrawer->m_eChartDrawType = (CChartDrawer::E_ChartDrawStyle)m_iDrawTypeXml;	// 不需要从xml中取值，共享全局值
				m_iDrawTypeXml = 0;
			}
			pCurve->AttatchDrawer(pKLineDrawer);
			pCurve->AttatchNodes(pNodes);
			
			// new 2 curve
			pCurve = m_pRegionXBottom->CreateCurve(CChartCurve::KDependentCurve|CChartCurve::KRequestCurve|CChartCurve::KInVisible|CChartCurve::KTypeKLine|CChartCurve::KYTransformByLowHigh);
			pCurve->AttatchNodes(pNodes);
		}
	}
	return true;
}

T_MerchNodeUserData	* CIoViewTrendArbitrage::GetMainData(bool32 bCreateIfNotExist/*=false*/)
{
	if ( m_MerchParamArray.GetSize() <= 0 && bCreateIfNotExist )
	{
		InitMainUserData();
	}
	return m_MerchParamArray.GetSize()>0? m_MerchParamArray[0] : NULL;
}

void CIoViewTrendArbitrage::OnIoViewColorChanged()
{
	CIoViewChart::OnIoViewColorChanged();
}

bool32 CIoViewTrendArbitrage::FromXml( TiXmlElement * pTiXmlElement )
{
	bool32 bXml = CIoViewChart::FromXml(pTiXmlElement);
	if ( bXml )
	{
	}

	//
	SetArbitrage(m_Arbitrage);

	return bXml;
}

CMerch * CIoViewTrendArbitrage::GetMerchXml()
{
	if ( NULL != m_Arbitrage.m_MerchA.m_pMerch )
	{
		return m_Arbitrage.m_MerchA.m_pMerch;
	}
	CMerch *pMerch = CIoViewChart::GetMerchXml();
	return pMerch;
}

void CIoViewTrendArbitrage::TestSetArb(bool32 bAsk/*=false*/)
{
	CArbitrage arb;
	CMerch *pMerch = NULL;
	m_pAbsCenterManager->GetMerchManager().FindMerch(_T("600000"), pMerch);
	arb.m_MerchA.m_pMerch = pMerch;
	m_pAbsCenterManager->GetMerchManager().FindNextMerch(pMerch, arb.m_MerchB.m_pMerch);
	if ( bAsk )
	{
		MerchArray aMerchs;
		bool32 bOk = false;
		if ( CDlgChooseStockVar::ChooseStockVar(aMerchs, false) )
		{
			arb.m_MerchA.m_pMerch = aMerchs[0];
			if ( CDlgChooseStockVar::ChooseStockVar(aMerchs, false) )
			{
				arb.m_MerchB.m_pMerch = aMerchs[0];
				bOk = true;
			}
		}
		if ( !bOk )
		{
			return;
		}
	}
	arb.m_eArbitrage = EABTwoMerch;
	arb.m_eArbPrice = EAPDiv;
	SetArbitrage(arb);
}

bool32 CIoViewTrendArbitrage::OnGetCurveTitlePostfixString( CChartCurve *pCurve, CPoint ptClient, bool32 bHideCross, OUT CString &StrPostfix )
{
	// 仅主图需要额外处理这些

	if ( pCurve == NULL
		|| &pCurve->GetChartRegion() != m_pRegionMain
		)
	{
		return false;
	}


	// 找到这个node
	CNodeSequence * pNodes = ((CChartCurve*)pCurve)->GetNodes();
	
	if ( NULL == pNodes)
	{
		return false;
	}
	if ( pNodes->GetName().GetLength() <= 0 )
	{
		return false;	// 没有名称的node不适合
	}
	
	return false;
}

void CIoViewTrendArbitrage::UpdateMerchDataTradeTime( T_MerchNodeUserData *pMerchData )
{
	if ( NULL == pMerchData || NULL == pMerchData->m_pMerchNode )
	{
		ASSERT( 0 );
		return;
	}
	ASSERT( !pMerchData->bMainMerch );	// 非main的更新

	CMarketIOCTimeInfo RecentTradingDay;
	if (pMerchData->m_pMerchNode->m_Market.GetRecentTradingDay(m_pAbsCenterManager->GetServerTime(), RecentTradingDay, pMerchData->m_pMerchNode->m_MerchInfo))
	{
		pMerchData->m_TrendTradingDayInfo.Set(pMerchData->m_pMerchNode, RecentTradingDay);
		pMerchData->m_TrendTradingDayInfo.RecalcPrice(*pMerchData->m_pMerchNode);
		
		// zhangbo 20090708 #待补充， 中轴更新
		//...
	}
}

void CIoViewTrendArbitrage::UpdateAllMerchDataTradeTimes()
{
	for ( int32 i=1; i < m_MerchParamArray.GetSize() ; ++i )
	{
		UpdateMerchDataTradeTime(m_MerchParamArray[i]);
	}
}

void CIoViewTrendArbitrage::CombineMainDataTradeTime()
{
	typedef set<CMarket *> MarketSet;
	MarketSet mset;

	// 夜盘相关的处理, 如果是夜盘商品, 不能取市场的交易时间, 取这个商品的特殊交易时间
	typedef map<CMarket*, CMerch*> SpecialMerchMap;
	SpecialMerchMap mapSpecialMerch;

	T_MerchNodeUserData *pMainData = GetMainData();
	if ( NULL == pMainData )
	{
		ASSERT( 0 );
		return;
	}

	pMainData->m_TrendTradingDayInfo.m_bInit = false;

	for (int32 i=1; i < m_MerchParamArray.GetSize() ; ++i )
	{
		T_MerchNodeUserData *pData = m_MerchParamArray[i];
		ASSERT( NULL != pData && pData->m_TrendTradingDayInfo.m_bInit && NULL != pData->m_pMerchNode );
		mset.insert( &pData->m_pMerchNode->m_Market );
		mapSpecialMerch[&pData->m_pMerchNode->m_Market] = pData->m_pMerchNode;
	}

	if ( mset.size() > 0 )
	{
		// 需要处理合并
		MarketSet::iterator it=mset.begin();
		CMarketIOCTimeInfo &IOCMain = pMainData->m_TrendTradingDayInfo.m_MarketIOCTime;
		IOCMain.m_aOCTimes.RemoveAll();

		if ( it != mset.end() )
		{
			CMarketIOCTimeInfo RecentTradingDay;
			CMarket *pMarket = *(it);
			CMerch* pSpecialMerch = mapSpecialMerch[pMarket];

			if (pMarket->GetRecentTradingDay(m_pAbsCenterManager->GetServerTime(), RecentTradingDay, pSpecialMerch->m_MerchInfo))
			{
				pMainData->m_TrendTradingDayInfo.Set(pMainData->m_pMerchNode, RecentTradingDay);
				// 昨收等不要计算
			}
			++it;
		}

		for ( ; it != mset.end() ; ++it )
		{
			CMarketIOCTimeInfo RecentTradingDay;
			CMarket *pMarket = *(it);
			CMerch* pSpecialMerch = mapSpecialMerch[pMarket];
			if (pMarket->GetRecentTradingDay(m_pAbsCenterManager->GetServerTime(), RecentTradingDay, pSpecialMerch->m_MerchInfo))
			{
				// 做合并呀~~
				IOCMain.m_TimeInit = max(IOCMain.m_TimeInit, RecentTradingDay.m_TimeInit);	// 最晚的初始化
				IOCMain.m_TimeEnd  = min(IOCMain.m_TimeEnd, RecentTradingDay.m_TimeEnd);	// 最早的结束
				IOCMain.m_TimeOpen  = max(IOCMain.m_TimeOpen, RecentTradingDay.m_TimeOpen);	// 最晚的开盘
				IOCMain.m_TimeClose  = min(IOCMain.m_TimeClose, RecentTradingDay.m_TimeClose);	// 最早的收盘

				// 小段小段的
				// 从最后一段开始，每接着的2段为一个开收盘时间，有效域为[开，收]
				ASSERT( IOCMain.m_aOCTimes.GetSize()%2 == 0 && RecentTradingDay.m_aOCTimes.GetSize()%2==0 );
				CArray<CGmtTime, const CGmtTime &> aTimeCombine;

				int32 iSub = 0;
				CGmtTime TimeOpen(IOCMain.m_TimeOpen.m_Time);
				CGmtTime TimeClose(IOCMain.m_TimeClose.m_Time);
				for ( int32 i=0; i < IOCMain.m_aOCTimes.GetSize()-1 ; i+=2 )
				{
					CGmtTime TimeOpen1 = IOCMain.m_aOCTimes[i];
					CGmtTime TimeClose1 = IOCMain.m_aOCTimes[i+1];
					
					TimeOpen = max(TimeOpen, TimeOpen1);
					TimeClose = TimeClose1;
					for ( ; iSub < RecentTradingDay.m_aOCTimes.GetSize()-1; )
					{
						CGmtTime TimeOpen2 = RecentTradingDay.m_aOCTimes[iSub];
						CGmtTime TimeClose2 = RecentTradingDay.m_aOCTimes[iSub+1];
						if ( TimeClose2 < TimeOpen )
						{
							iSub += 2;	// 在共同的开盘价前面，找下一个开收
							continue;
						}
						else if ( TimeOpen2 > TimeClose )
						{
							break;	// 与指定开盘价没有交集，让main下一个开收
						}
						else
						{
							// TimeOpen <= TimeClose2 || TimeOpen2 <= TimeClose
							// 应该是有交集出现的
							CGmtTime TimeOpen3 = max(TimeOpen, TimeOpen2);
							CGmtTime TimeClose3 = min(TimeClose, TimeClose2);
							if ( TimeClose3 >= TimeOpen3 )
							{
								aTimeCombine.Add(TimeOpen3);
								aTimeCombine.Add(TimeClose3);
							}
							else
							{
								ASSERT( 0 );	// 没有交集？？
							}
							TimeOpen = TimeOpen3;	// 看下一个开收与指定的main的收盘有交集没
							if ( TimeClose2 > TimeClose1 )
							{
								// 跨度比main大，则让main下一个开收
								break;
							}
							else
							{
								// 没有main广，则进行下一个开收判断
								iSub += 2;
							}
							continue;
						}
					}
				}
				
				// 排序，检查能组成开收的段，插入 end
// 				typedef std::multimap<CGmtTime, int32> SortMap;
// 				SortMap mSort;
// 				for ( int32 i=0; i < IOCMain.m_aOCTimes.GetSize() ; ++i )
// 				{
// 					mSort.insert(SortMap::value_type(IOCMain.m_aOCTimes[i], i));
// 				}
// 				for ( i=0; i < RecentTradingDay.m_aOCTimes.GetSize() ; ++i )
// 				{
// 					mSort.insert(SortMap::value_type(RecentTradingDay.m_aOCTimes[i], i));
// 				}
// 				// 时间小的从开盘找起
// 				bool32 bFindOpen = true;
// 				CGmtTime TimeOpen(0);
// 				for ( SortMap::iterator itSort=mSort.begin(); itSort != mSort.end() ; ++itSort )
// 				{
// 					if ( bFindOpen )
// 					{
// 						// 找开盘
// 						int32 iIndex = itSort->second;
// 						CGmtTime Time = itSort->first;
// 						if ( iIndex%2 == 0 && Time > TimeOpen )
// 						{
// 							// 必须要比前面的开盘大
// 							TimeOpen = Time;
// 							bFindOpen = false;
// 						}
// 						else
// 						{
// 							// 碰到一个没有开盘匹配的收盘或者已经匹配了的开盘，无视
// 						}
// 					}
// 					else
// 					{
// 						// 找收盘
// 						int32 iIndex = itSort->second;
// 						CGmtTime Time = itSort->first;
// 						if ( iIndex%2 == 1 )
// 						{
// 							aTimeCombine.Add(TimeOpen);	// 开收对找到了
// 							aTimeCombine.Add(Time);
// 
// 							bFindOpen = true;
// 						}
// 						else
// 						{
// 							// 碰到了更接近收盘价的开盘价，替换
// 							TimeOpen = Time;
// 						}
// 					}
// 				}

				// 必须在规定的时间内
				for (int  q=aTimeCombine.GetSize()-1; q >0 ; q-=2 )
				{
					CGmtTime tmpTimeOpen = aTimeCombine[q-1];
					CGmtTime tmpTimeClose = aTimeCombine[q];
					if ( tmpTimeClose > IOCMain.m_TimeClose.m_Time )
					{
						// 比合集收盘更晚，t
						ASSERT( 0 );
						aTimeCombine.RemoveAt(q-1, 2);
					}
					else if ( tmpTimeOpen < IOCMain.m_TimeOpen.m_Time )
					{
						// 比合集开盘更早, t
						ASSERT( 0 );
						aTimeCombine.RemoveAt(q-1, 2);
					}
				}

				IOCMain.m_aOCTimes.Copy(aTimeCombine);
			}
			else
			{
				ASSERT( 0 );	// 一个不能处理，所有的都不能
				IOCMain.m_aOCTimes.RemoveAll();
				pMainData->m_TrendTradingDayInfo.m_bInit = false;
				break;
			}
		}

		if ( IOCMain.m_aOCTimes.GetSize() <= 0 )
		{
			pMainData->m_TrendTradingDayInfo.m_bInit = false;	// 没有合集
		}
		else
		{
			ASSERT( (IOCMain.m_aOCTimes.GetSize() > 0 && IOCMain.m_aOCTimes.GetSize()%2==0) );
			ASSERT( IOCMain.m_TimeClose >= IOCMain.m_TimeOpen );
			ASSERT( pMainData->m_TrendTradingDayInfo.m_bInit );
		}
	}
	else
	{
		// 根本没有任何市场
		pMainData->m_TrendTradingDayInfo.m_bInit = false;
	}

	if ( !pMainData->m_TrendTradingDayInfo.m_bInit )
	{
		pMainData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeClose = pMainData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeOpen;
		pMainData->m_TrendTradingDayInfo.m_MarketIOCTime.m_aOCTimes.RemoveAll();
		pMainData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeEnd = pMainData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeClose;
	}
}

bool32 CIoViewTrendArbitrage::GetTrendOpenCloseTime( OUT CGmtTime &TimeStart, OUT CGmtTime &TimeEnd )
{
	T_MerchNodeUserData *pMainData = GetMainData();
	if ( NULL == pMainData )
	{
		ASSERT( 0 );
		return false;
	}

	if ( !pMainData->m_TrendTradingDayInfo.m_bInit )
	{
		return false;
	}

	TimeStart	= pMainData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeOpen.m_Time;
	TimeEnd		= pMainData->m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeClose.m_Time;
	return true;
}

int32 CIoViewTrendArbitrage::CompareKLinesChange(const CKLine *pKLineBef, int32 iBefCount, const CKLine *pKLineAft, int32 iAftCount )
{
	// K 线更新的时候,判断最新的K 线与原来相比有什么变化:
	// 返回值: 0:相同 1:最新一根变化 2:增加了一根 3:除了以上情况,有大的变化

	int32 iReVal = 3;

	int32 iSizeBefore = iBefCount;
	int32 iSizeAfter  = iAftCount;
	
	char* pBefore = (char*)pKLineBef;
	char* pAfter  = (char*)pKLineAft;

	if ( NULL == pBefore || NULL == pAfter)
	{
		return 3;
	}

	if ( iSizeBefore != iSizeAfter)
	{
		// 节点数不同,可能只更新了一个节点.这时需要仔细判断,可能更新了多个节点个数大不相同,直接返回3;

		if ( iSizeAfter == iSizeBefore + 1)
		{
			// 多了一个节点.判断一下是不是除了最后一个节点以外,其他的都相同,如果是的话,那就是 2
			if ( 0 == memcmp(pBefore, pAfter+sizeof(CKLine),iSizeBefore*sizeof(CKLine)))
			{
				// 除了最后一个,其他都一样,就是新增了一根
				return 2;
			}
			else
			{
				// 不一样.
				return 3;
			}			
		}
		else
		{
			return 3;
		}
	}
	else
	{
		// 节点个数相同:
		if ( iSizeBefore == KMaxMinuteKLineCount )
		{
			// 满了, 1W 个节点了: 新来的数据插到第一个,把最后一个踢掉.所以拿 After 的前 0~9999(去尾) 和 Before 的 1~10000 比较(去头)
			
			if ( 0 == memcmp(pBefore,pAfter,iSizeBefore*sizeof(CKLine)))
			{
				// 完全相同
				return 0;
			}
			
			if ( 0 == memcmp(pBefore +sizeof(CKLine), pAfter, (iSizeBefore - 1)*sizeof(CKLine)))
			{
				// 除了最后一个,其他都一样,最新一根发生变化
				return 1;
			}
			else
			{
				// 不一样
				return 3;
			}
		}
		else
		{
			// 正常情况
			if ( 0 == memcmp(pBefore,pAfter,iSizeBefore*sizeof(CKLine)))
			{
				// 完全相同
				return 0;
			}
			
			//看除了最后一个前面是不是一样的.
			
			if ( 0 == memcmp(pBefore,pAfter,(iSizeBefore - 1)*sizeof(CKLine)))
			{
				// 除了最后一个,其他都一样,最新一根发生变化
				return 1;
			}
			else
			{
				// 不一样.
				return 3;
			}
		}			
	}
	
	return iReVal;
}

bool32 CIoViewTrendArbitrage::FillSubMerchTrendNodes( T_MerchNodeUserData &MerchNodeUserData, const CGmtTime &TimeNow, const CKLine *pKLines, int32 iCountKLine )
{
	ASSERT( !MerchNodeUserData.bMainMerch );	// 只能副

	// 
	MerchNodeUserData.m_aKLinesFull.SetSize(0);

	// pKline & iCount可能为NULL
	if ( NULL == pKLines )
	{
		return false;
		//if ( iCountKLine > 0 )
		//{
		//	ASSERT( 0 );
		//	return false;	// 必须<=0
		//}
	}

	// 
	if (!MerchNodeUserData.m_TrendTradingDayInfo.m_bInit)
	{
		//ASSERT(0);
		return false;
	}

	// 
	int32 iMaxTrendUnitCount = 0;
	iMaxTrendUnitCount = MerchNodeUserData.m_TrendTradingDayInfo.m_MarketIOCTime.GetMaxTrendUnitCount();
	if ( iMaxTrendUnitCount <= 0 )
	{
		// 没有数据
		ASSERT( 0 );
		return false;
	}

	float fPricePrev = GetMerchTrendPrevClose(MerchNodeUserData);
	if ( fPricePrev == 0.0f )
	{
		ASSERT( 0 );	// 不支持0价格商品的填充
		return false;
	}
	// 先初始化第一笔数据
	MerchNodeUserData.m_aKLinesFull.SetSize(iMaxTrendUnitCount + 20);
	CKLine *pKLineFull = (CKLine *)MerchNodeUserData.m_aKLinesFull.GetData();
	int iCountNode = 0;
	pKLineFull[iCountNode].m_TimeCurrent	= MerchNodeUserData.m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeOpen.m_Time.GetTime();
	pKLineFull[iCountNode].m_fPriceOpen		= fPricePrev;
	pKLineFull[iCountNode].m_fPriceClose	= fPricePrev;
	pKLineFull[iCountNode].m_fPriceAvg		= fPricePrev;
	pKLineFull[iCountNode].m_fPriceHigh		= fPricePrev;
	pKLineFull[iCountNode].m_fPriceLow		= fPricePrev;
	
 	pKLineFull[iCountNode].m_fVolume		= 0.;
 	pKLineFull[iCountNode].m_fAmount		= 0.;
 	pKLineFull[iCountNode].m_fHold			= 0.;
	
	bool32 bFoundFirstNodeData = false;		// 是否到了第一个有效数据

	// 逐个分段处理
	int32 iKLineIndex = 0;

	{
		// 完整数据
		int32 iLastDayPos = -1;
		const CMarketIOCTimeInfo &IOCTime = MerchNodeUserData.m_TrendTradingDayInfo.m_MarketIOCTime;
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
					iCountNode++;
				}
			}
		}	
	}

	// 修正长度
	MerchNodeUserData.m_aKLinesFull.SetSize(iCountNode);
	return true;
}

bool32 CIoViewTrendArbitrage::FillTrendNodes( T_MerchNodeUserData &MerchNodeUserData, const CGmtTime &TimeNow, const CKLine *pKLines, int32 iCountKLine )
{
	ASSERT( MerchNodeUserData.bMainMerch );	// 只能主

	// 
	MerchNodeUserData.m_aKLinesFull.SetSize(0);

	if (iCountKLine <= 0)
		return true;

	ASSERT(NULL != pKLines);

	// 
	if (!MerchNodeUserData.m_TrendTradingDayInfo.m_bInit)
	{
		//ASSERT(0);
		return false;
	}

	// 
	int32 iMaxTrendUnitCount = 0;
	iMaxTrendUnitCount = MerchNodeUserData.m_TrendTradingDayInfo.m_MarketIOCTime.GetMaxTrendUnitCount();
	if ( iMaxTrendUnitCount <= 0 )
	{
		// 没有数据
		ASSERT( 0 );
		return false;
	}

	MerchNodeUserData.m_aKLinesFull.SetSize(iMaxTrendUnitCount + 20);
	MerchNodeUserData.m_aKLinesShowDataFlag.SetSize(iMaxTrendUnitCount + 20);
	CKLine *pKLineFull = (CKLine *)MerchNodeUserData.m_aKLinesFull.GetData();
	int iCountNode = 0;
	DWORD *pFlags = (DWORD *)MerchNodeUserData.m_aKLinesShowDataFlag.GetData();
	ZeroMemory(pFlags, (iMaxTrendUnitCount+20)*sizeof(DWORD));
	
	// 先初始化第一笔数据
	pKLineFull[iCountNode].m_TimeCurrent	= MerchNodeUserData.m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeOpen.m_Time.GetTime();

	float fPricePrev = MerchNodeUserData.m_TrendTradingDayInfo.GetPrevReferPrice();
	if ( MerchNodeUserData.bMainMerch )
	{
		fPricePrev = GetTrendPrevClose();
	}
	pKLineFull[iCountNode].m_fPriceOpen		= fPricePrev;
	pKLineFull[iCountNode].m_fPriceClose	= fPricePrev;
	pKLineFull[iCountNode].m_fPriceAvg		= fPricePrev;
	pKLineFull[iCountNode].m_fPriceHigh		= fPricePrev;
	pKLineFull[iCountNode].m_fPriceLow		= fPricePrev;
	
 	pKLineFull[iCountNode].m_fVolume		= 0.;
 	pKLineFull[iCountNode].m_fAmount		= 0.;
 	pKLineFull[iCountNode].m_fHold			= 0.;
	
	bool32 bFoundFirstNodeData = false;		// 是否到了第一个有效数据

	// 逐个分段处理
	int32 iKLineIndex = 0;

	{
		// 完整数据
		int32 iLastDayPos = -1;
		const CMarketIOCTimeInfo &IOCTime = MerchNodeUserData.m_TrendTradingDayInfo.m_MarketIOCTime;
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
						ASSERT(!MerchNodeUserData.bMainMerch || MerchNodeUserData.m_pMerchNode != m_pMerchXml);		// 叠加商品可能出现,不应该出现这种情况
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
						if ( !bFoundFirstNodeData )
						{
							// 第一个有效数据前的所有数据都是非法节点
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
					if ( !bFoundFirstNodeData )
					{
						// 第一个有效数据前的所有数据都是非法节点
						pFlags[iCountNode] = CNodeData::KValueInvalid;	// 值非法
					}
					iCountNode++;
				}
			}
		}	
	}

	// 修正长度
	MerchNodeUserData.m_aKLinesFull.SetSize(iCountNode);
	MerchNodeUserData.m_aKLinesShowDataFlag.SetSize(iCountNode);
	return true;
}

float CIoViewTrendArbitrage::GetTrendPrevClose()
{
	MerchArray aMerchs;
	if ( GetArbitrageMerchs(m_Arbitrage, aMerchs) )
	{
		float fPre = m_Arbitrage.GetPrice(EAHPriceDifPreClose);
		if ( fPre == FLT_MIN )
		{
			fPre = 0.0f;
		}
		return fPre;
	}
	return 0.0f;
}

float CIoViewTrendArbitrage::GetMerchTrendPrevClose( T_MerchNodeUserData &MerchNodeUserData )
{
	T_MerchNodeUserData *pData = &MerchNodeUserData;
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

void CIoViewTrendArbitrage::OnF5()
{
	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if ( NULL == pMainFrame )
	{
		return;
	}

	pMainFrame->OnArbitrageF5(m_Arbitrage, this);
}

BOOL CIoViewTrendArbitrage::PreTranslateMessage( MSG* pMsg )
{
	if ( pMsg->message == WM_KEYDOWN )
	{
		if ( VK_F5 == pMsg->wParam )
		{
			OnF5();
			return TRUE;
		}
	}
	return CIoViewChart::PreTranslateMessage(pMsg);
}

bool32 CIoViewTrendArbitrage::GetArbitrage( OUT CArbitrage &arb )
{
	MerchArray aMerchs;
	if ( GetArbitrageMerchs(m_Arbitrage, aMerchs) )
	{
		arb = m_Arbitrage;
		return true;
	}
	return false;
}

bool32 CIoViewTrendArbitrage::OnSpecialEsc()
{
	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if ( NULL == pMainFrame )
	{
		return true;
	}
	
	return !pMainFrame->OnArbitrageEscReturn(m_Arbitrage, this);
}
