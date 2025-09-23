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

#include "IoViewKLineArbitrage.h"
#include "DlgChooseStockVar.h"
#include "BalloonMsg.h"
#include "sharestructnews.h"
#include "XmlShare.h"
#include "XLTimerTrace.h"
#include "DlgMenuUserCycleSet.h"

using std::set;

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
							//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern void TestKLine(CArray<CKLine,CKLine>& aKLines,const char * pFile,CString StrMerchCode, int32 iMarketId);
extern void TestNodes(CArray<CNodeData,CNodeData&>& aNodes,const char * pFile,CString StrMerchCode, int32 iMarketId);

extern CString MakeMenuString(const CString& StrName, const CString& StrExplain, bool32 bAppendSpace=true);

bool32	GetArbitrageMerchs(IN const CArbitrage &arb, OUT CIoViewBase::MerchArray &aMerchs);
CString ArbEnumToString(E_Arbitrage eType);
CString	ArbShowTypeToString(E_ArbitrageChartShowType eType);

//////////////////////////////////////////////////////////////////////////
namespace
{
	const int32 KTimerIdCalcArbitrage		= 10086;		// 计算套利
	const int32 KTimerPeriodCalcArbitrage	= 10;


	const int32 KSpaceRightCount			= 4;			// 右边空出的K线条数

	const int32 KMinKLineNumPerScreen		= 10;			// 最少显示K线条数, 包括KSpaceRightCount位置
	const int32 KMaxKLineNumPerScreen		= 5000;		// 最多显示K线条数
	const int32 KDefaultKLineNumPerScreen	= 200;

	const int32 KDefaultCtrlMoveSpeed		= 5;			// 在Ctrl键按下时，默认平移的单位

	const int32 KMinRequestKLineNumber		= 300;			// 一次请求最少数据量
	const int32 KRequestKLineNumberScale	= 2;			// 默认第一次请求K线数量
	const int32 KDefaultKLineWidth			= 15;			// 根据视图大小请求K 线的时候,K 线节点默认宽度

	// 
	const int32 KMaxMinuteKLineCount		= 10000;		// 最大允许的分钟K线数据， 不加限制的话， 连续开着软件会无限增长

	//
	const int32 KiStatisticaKLineNums		= 200;			// 统计的K 线个数

	// xml
	static const char KXMLAttriIoViewKLineArbPickSubRegionIndex[] = "PickSubRegion";	// 最后选择的副图
	static const char KXMLAttriIoViewKLineArbShowType[]				= "ArbShowType";	// 套利显示类型
}

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CIoViewKLineArbitrage, CIoViewChart)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewKLineArbitrage, CIoViewChart)
	//{{AFX_MSG_MAP(CIoViewKLineArbitrage)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
 	ON_COMMAND_RANGE(IDM_CHART_BEGIN, IDM_CHART_END, OnMenu)
	ON_COMMAND_RANGE(ID_CHART_INTERVALSTATISTIC, ID_KLINE_HISTORYTREND, OnMenu)
	ON_COMMAND_RANGE(ID_KLINE_AXISYNORMAL, ID_KLINE_AXISYLOG, OnMenu)
 	ON_COMMAND_RANGE(IDM_IOVIEWKLINE_ALLINDEX_BEGIN, IDM_IOVIEWKLINE_ALLINDEX_END, OnAllIndexMenu)
 	ON_COMMAND_RANGE(IDM_IOVIEWKLINE_OFTENINDEX_BEGIN, IDM_IOVIEWKLINE_OFTENINDEX_END, OnOftenIndexMenu)
 	ON_COMMAND_RANGE(IDM_IOVIEWKLINE_OWNINDEX_BEGIN, IDM_IOVIEWKLINE_OWNINDEX_END, OnOwnIndexMenu)	
 	ON_COMMAND_RANGE(IDT_SD_BEGIN, IDT_SD_END,OnMenu)	
	ON_COMMAND_RANGE(ID_ARBITRAGE_SETTING, ID_ARBITRAGE_SETTING, OnMenu)
	ON_COMMAND_RANGE(IDM_ARBTTRAGE_SELECT, IDM_ARBTTRAGE_SELECT, OnMenu)
	ON_COMMAND_RANGE(ID_ARBKLINE_CLOSEDIFF, ID_ARBKLINE_END, OnMenu)
	ON_MESSAGE(UM_IoViewTitle_Button_LButtonDown, OnMessageTitleButton)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// 
void CIoViewKLineArbitrage::OnTest()
{
	m_StrText = L"";
}

CIoViewKLineArbitrage::CIoViewKLineArbitrage()
:CIoViewChart(),CChartRegionViewParam(NULL)
{
	m_pParent			= NULL;
	m_rectClient		= CRect(-1,-1,-1,-1);
	m_iDataServiceType  = EDSTKLine | EDSTPrice ;
	m_StrText			=_T("IoViewKLineArbitrage");
	m_pRegionViewData	= this;
	m_pRegionDrawNotify = this;
	SetParentWnd(this);
	m_iChartType		= 4;
	m_bForceUpdate		= false;

	m_bRequestNodesByRectView	= false;
	m_aAllFormulaNames.RemoveAll();

	//
	m_iCurCrossKLineIndex	= -1;
	m_iNodeCountPerScreen	= KDefaultKLineNumPerScreen;	
	memset((void*)(&m_KLineCrossNow), 0, sizeof(m_KLineCrossNow));

	
	//
	InitialImageResource();	

	m_iLastPickSubRegionIndex = 0;	// 默认第一个副图

	m_eArbitrageShow = EACST_CloseDiff;	// 收盘线
	m_pCuveOtherArbLeg = NULL;
}

CIoViewKLineArbitrage::~CIoViewKLineArbitrage()
{
	CArbitrageManage::Instance()->DelNotify(this);
}

///////////////////////////////////////////////////////////////////////////////
int CIoViewKLineArbitrage::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CIoViewChart::OnCreate(lpCreateStruct) == -1)
		return -1;

	CArbitrageManage::Instance()->AddNotify(this);
	
	return 0;
}

BOOL CIoViewKLineArbitrage::TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ( NULL != m_pIoViewManager && this != m_pIoViewManager->GetActiveIoView() )
	{
		return FALSE;
	}
	
	if ( nChar == VK_F8 )
	{
		OnKeyF8();
		return TRUE;
	}

// 	if ( nChar == VK_F7 )
// 	{
// 		TestSetArb(TRUE);
// 		return TRUE;
// 	}

	return CIoViewChart::TestKeyDown(nChar,nRepCnt,nFlags);
}

void CIoViewKLineArbitrage::InitCtrlFloat( CGridCtrl* pGridCtrl )
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

void CIoViewKLineArbitrage::CreateRegion()
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

void CIoViewKLineArbitrage::SplitRegion()
{
	CIoViewChart::SplitRegion();
	UpdateGDIObject();
}

int32 CIoViewKLineArbitrage::GetSubRegionTitleButtonNums()
{
	return 0;
}

const T_FuctionButton* CIoViewKLineArbitrage::GetSubRegionTitleButton(int32 iIndex)
{
	if( iIndex < 0 || iIndex >= GetSubRegionTitleButtonNums() )
	{
		return NULL;
	}

	ASSERT( 0 );
	return NULL;
}

void CIoViewKLineArbitrage::OnDestroy() 
{
	m_pCuveOtherArbLeg = NULL;

	CArbitrageManage::Instance()->DelNotify(this);

	ReleaseMemDC();

	CIoViewChart::OnDestroy();
}

E_NodeTimeInterval CIoViewKLineArbitrage::GetTimeInterval()
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

bool32 CIoViewKLineArbitrage::GetTimeIntervalInfo(E_NodeTimeInterval eNodeTimeInterval, E_NodeTimeInterval &eNodeTimeIntervalCompare, E_KLineTypeBase &eKLineTypeCompare,int32 &iScale)
{
	eNodeTimeIntervalCompare = ENTIDay;
	iScale = 1;

	// 
	switch (eNodeTimeInterval)
	{
	case ENTIMinute:
		{
			eNodeTimeIntervalCompare	= ENTIMinute;
			eKLineTypeCompare			= EKTBMinute;
			iScale						= 1;
		}
		break;
	case ENTIMinute5:
		{
			eNodeTimeIntervalCompare	= ENTIMinute5;
			eKLineTypeCompare			= EKTB5Min;
			iScale						= 1;
		}
		break;
	case ENTIMinute15:
		{
			eNodeTimeIntervalCompare	= ENTIMinute5;
			eKLineTypeCompare			= EKTB5Min;
			iScale						= 3;
		}			
		break;
	case ENTIMinute30:
		{
			eNodeTimeIntervalCompare	= ENTIMinute5;
			eKLineTypeCompare			= EKTB5Min;
			iScale						= 6;
		}
		break;
	case ENTIMinute60:
		{
			eNodeTimeIntervalCompare	= ENTIMinute60;
			eKLineTypeCompare			= EKTBHour;
			iScale						= 1;
		}
		break;
    case ENTIMinute180:
        {
            eNodeTimeIntervalCompare	= ENTIMinute60;
            eKLineTypeCompare			= EKTBHour;
            iScale						= 3;
        }
        break;
    case ENTIMinute240:
        {
            eNodeTimeIntervalCompare	= ENTIMinute60;
            eKLineTypeCompare			= EKTBHour;
            iScale						= 4;
        }
        break;
	case ENTIMinuteUser:			// 自定义周期
		{
			if ( m_MerchParamArray.GetSize() <= 0)
			{
				return false;
			}

			T_MerchNodeUserData *pData = m_MerchParamArray.GetAt(0);

			if ( NULL == pData )
			{
				return false;
			}

			if ( pData->m_iTimeUserMultipleMinutes <= 0)		// 错误的周期
				return false;

			if ( pData->m_iTimeUserMultipleMinutes % 60 == 0)
			{
				eNodeTimeIntervalCompare= ENTIMinute60;
				eKLineTypeCompare		= EKTBHour;
				iScale					= pData->m_iTimeUserMultipleMinutes / 60;
			}
			else if ( pData->m_iTimeUserMultipleMinutes % 5 == 0)
			{
				eNodeTimeIntervalCompare= ENTIMinute5;
				eKLineTypeCompare		= EKTB5Min;
				iScale					= pData->m_iTimeUserMultipleMinutes / 5;
			}
			else
			{
				eNodeTimeIntervalCompare= ENTIMinute;
				eKLineTypeCompare		= EKTBMinute;
				iScale					= pData->m_iTimeUserMultipleMinutes;
			}
		}
		break;
	case ENTIDay:
		{
			eNodeTimeIntervalCompare	= ENTIDay;
			eKLineTypeCompare			= EKTBDay;
			iScale						= 1;
		}
		break;
	case ENTIWeek:
		{
			eNodeTimeIntervalCompare	= ENTIDay;
			eKLineTypeCompare			= EKTBDay;
			iScale						= 6;
		}
		break;
	case ENTIMonth:
		{
			eNodeTimeIntervalCompare	= ENTIMonth;
			eKLineTypeCompare			= EKTBMonth;
			iScale						= 1;
		}
		break;
	case ENTIQuarter:
		{
			eNodeTimeIntervalCompare	= ENTIMonth;
			eKLineTypeCompare			= EKTBMonth;
			iScale						= 3;
		}
		break;
	case ENTIYear:
		{
			eNodeTimeIntervalCompare	= ENTIMonth;
			eKLineTypeCompare			= EKTBMonth;
			iScale						= 12;
		}
		break;
	case ENTIDayUser:
		{
			if ( m_MerchParamArray.GetSize() <= 0)
			{
				return false;
			}
			
			T_MerchNodeUserData *pData = m_MerchParamArray.GetAt(0);
			
			if ( NULL == pData )
			{
				return false;
			}

			if ( pData->m_iTimeUserMultipleDays <= 0)		// 错误的周期
				return false;
			
			eNodeTimeIntervalCompare	= ENTIDay;
			eKLineTypeCompare			= EKTBDay;
			iScale						= pData->m_iTimeUserMultipleDays;
		}
		break;		
	default:
		{
			return false;
		}
		break; 
	}

	return true;
}

void CIoViewKLineArbitrage::SetTimeInterval(T_MerchNodeUserData &MerchNodeUserData, E_NodeTimeInterval eNodeTimeInterval)
{
	MerchNodeUserData.m_eTimeIntervalFull		= ENTIMinute;
	E_NodeTimeInterval eNodeTimeIntervalCompare = ENTIDay;
	E_KLineTypeBase	   eKLineTypeCompare		= EKTBDay;

	int32 iScale = 1;
	if (!GetTimeIntervalInfo(eNodeTimeInterval, eNodeTimeIntervalCompare, eKLineTypeCompare, iScale))
	{
		//ASSERT(0);	// 不应该出现这种情况
		return;
	}

	ASSERT( MerchNodeUserData.bMainMerch );	// 必须为主
	MerchNodeUserData.m_eTimeIntervalFull = eNodeTimeInterval;
	MerchNodeUserData.m_eTimeIntervalCompare = eNodeTimeIntervalCompare;
	for ( int32 i=1; i < m_MerchParamArray.GetSize() ; ++i )
	{
		ResetTimeInterval(m_MerchParamArray[i], eNodeTimeInterval, eNodeTimeIntervalCompare);
		ResetMerchUserData(m_MerchParamArray[i]);	// 要将数据清除重新合成
	}

	ResetNodeScreenCount();	// 重置屏幕点数

	CalcArbitrageAsyn();
}

void CIoViewKLineArbitrage::ResetTimeInterval(T_MerchNodeUserData* pData,IN E_NodeTimeInterval eNodeTimeInterval,IN E_NodeTimeInterval eNodeTimeIntervalCompare)
{
	pData->m_eTimeIntervalFull    = eNodeTimeInterval;
	pData->m_eTimeIntervalCompare = eNodeTimeIntervalCompare;

	//

	// ...fangz0828 这里不要清空数据,会导致多余的请求( 原来有数据的,清空了又重新请求), 用本地数据先画出来,如不够在请求,用请求回来的数据覆盖重画当前的
	if ( pData->bMainMerch )
	{
		UpdateMainMerchKLine(*pData);
	}
	else
	{
		UpdateSubMerchKLine(*pData);
	}
}

// 通知视图改变关注的商品
void CIoViewKLineArbitrage::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	// 事实上套利并不关心商品的切换，仅关心套利内容的变更
	_LogCheckTime("[Client==>] CIoViewKLineArbitrage::OnVDataMerchChanged", g_hwndTrace);

	if (m_pMerchXml == pMerch || NULL == pMerch )
		return;
	
	// 修改当前查看的商品
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
}

// 叠加视图相关
bool32 CIoViewKLineArbitrage::OnVDataAddCompareMerch(IN CMerch *pMerch)
{
	return true;
}

void CIoViewKLineArbitrage::SetCurveTitle ( T_MerchNodeUserData* pData )
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
			StrTitle = ArbShowTypeToString(m_eArbitrageShow);
			if ( EACST_TowLegCmp == m_eArbitrageShow )
			{
				StrTitle += _T("A");	// 主线必然是A
			}
		}
		else
		{
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
		
		StrTitle += _T("(") + TimeInterval2String(pData->m_eTimeIntervalFull, pData->m_iTimeUserMultipleMinutes, pData->m_iTimeUserMultipleDays) + _T(") ");
		
		m_pRegionMain->SetTitle(StrTitle);
	}
}

void CIoViewKLineArbitrage::OnVDataForceUpdate()
{
	if ( NULL == m_pRegionMain )
	{
		return;
	}

	m_bForceUpdate = true;

	CIoViewChart::OnVDataForceUpdate();
	RequestViewData(true);

	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	PostMessage(WM_PAINT);

	m_bForceUpdate = false;
}

void CIoViewKLineArbitrage::OnVDataFormulaChanged ( E_FormulaUpdateType eUpdateType, CString StrName )
{
	CIoViewChart::OnVDataFormulaChanged(eUpdateType, StrName);
}

void CIoViewKLineArbitrage::OnVDataRealtimePriceUpdate(IN CMerch *pMerch)
{
}

bool32 CIoViewKLineArbitrage::IsShowNewestKLine(OUT CGmtTime& TimeStartInFullList, OUT CGmtTime &TimeEndInFullList)
{
	TimeStartInFullList = TimeEndInFullList = CGmtTime(0);
	if (m_MerchParamArray.GetSize() > 0)
	{
		T_MerchNodeUserData *pMainData = m_MerchParamArray[0];
		if (NULL != pMainData)
		{
			int32 iShowPosStart = pMainData->m_iShowPosInFullList;
			int32 iShowPosEnd = iShowPosStart + pMainData->m_iShowCountInFullList - 1;

			if (iShowPosStart <= iShowPosEnd &&
				iShowPosStart >= 0 && iShowPosStart < pMainData->m_aKLinesFull.GetSize() &&
				iShowPosEnd >= 0 && iShowPosEnd < pMainData->m_aKLinesFull.GetSize())	// 在正常的显示范围内
			{
				// 照样拷贝时间
				TimeStartInFullList		= CGmtTime(pMainData->m_aKLinesFull[iShowPosStart].m_TimeCurrent);
				TimeEndInFullList		= CGmtTime(pMainData->m_aKLinesFull[iShowPosEnd].m_TimeCurrent);
				if (iShowPosEnd < pMainData->m_aKLinesFull.GetSize() - 1)
				{
					return false;
				}
			}
		}
	}	
	
	return true;
}

void CIoViewKLineArbitrage::OnVDataPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType)
{
	return;
	// ...fangz1117  目前 UpdateMainMerchKLine 中是每次都 bUpdate = false 全部数据重算,
	// 优化以后,要处理除权等数据,不要因为在收盘后没有K 线重算的时机而导致没有更新

	if (NULL == pMerch || pMerch != m_pMerchXml)
		return;
	
	if (EPFTWeight != ePublicFileType)	// 仅处理除权数据
		return;

	// 挨个商品更新所有数据
}

void CIoViewKLineArbitrage::ClearLocalData(bool32 bClearAll/* = true*/)
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

	if ( NULL != m_pCuveOtherArbLeg )
	{
		if ( NULL != m_pCuveOtherArbLeg->GetNodes() )
		{
			m_pCuveOtherArbLeg->GetNodes()->GetNodes().RemoveAll();
		}
	}
}

void CIoViewKLineArbitrage::OnVDataMerchKLineUpdate(IN CMerch *pMerch)
{
	if (NULL == pMerch/* || pMerch != m_pMerchXml*/)
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
			UpdateSubMerchKLine(*pData, false);
		}
	}
}

bool32 CIoViewKLineArbitrage::UpdateMainMerchKLine(T_MerchNodeUserData &MerchNodeUserData, bool32 bForceUpdate)
{
	CalcArbitrage();	// 立即开始计算套利数据
	return true;
}

bool32 CIoViewKLineArbitrage::UpdateSubMerchKLine(T_MerchNodeUserData &MerchNodeUserData, bool32 bForceUpdate/*=true*/)
{
	if (m_MerchParamArray.GetSize() <= 0 || m_MerchParamArray[0] == &MerchNodeUserData)
	{
		return false;
	}
	
	// 用来更新关联商品
	T_MerchNodeUserData *pMainData = GetMainData(true);
	if ( NULL == pMainData )
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
	_LogCheckTime("[==>Client] CIoViewKLineArbitrage::UpdateSubMerchKLine 开始", g_hwndTrace);

	//
	E_NodeTimeInterval eNodeTimeIntervalCompare = ENTIDay;
	E_KLineTypeBase    eKLineType			    = EKTBDay;
	
	int32 iScale = 1;
	if (!GetTimeIntervalInfo(pMainData->m_eTimeIntervalCompare, eNodeTimeIntervalCompare, eKLineType, iScale))
	{
		//ASSERT(0);	// 不应该出现这种情况
		return false;
	}
	
	// 判断当前显示， 是否显示最新价
	CGmtTime TimeStartInFullList, TimeEndInFullList;

	IsShowNewestKLine(TimeStartInFullList, TimeEndInFullList);

	// 先获取对应的K线
	int32 iPosFound;
	CMerchKLineNode* pKLineRequest = NULL;
	pMerch->FindMerchKLineNode(eKLineType, iPosFound, pKLineRequest);
	
	// 根本找不到K线数据， 那就不需要显示了
	if (NULL == pKLineRequest || 0 == pKLineRequest->m_KLines.GetSize())	
	{
		ResetMerchUserData(&MerchNodeUserData);
		CalcArbitrageAsyn();
		return true;
	}

	// 优化：绝大多数情况下， 该事件都是由于历史数据更新引发的， 对这种情况做特别判断处理	

	int32 iCmpResult = bForceUpdate ? 1 : CompareKLinesChange(pKLineRequest->m_KLines, MerchNodeUserData.m_aKLinesCompare); // 原始数据比较

	CArray<CKLine, CKLine> aFullKlines;
	
	// 有可能周期或者权息信息变更，导致需要重新合成k线
	if ( iCmpResult != 0 )
	{
		// 需要更新合成K线
		// 清除原始&显示数据
		MerchNodeUserData.m_aKLinesCompare.Copy(pKLineRequest->m_KLines);	// 复制一份原始数据

		// 是否保持与K线相同的复权选择？
		//////////////////////////////////////////////////////////////////////////
		// 除权复权:
		CArray<CKLine, CKLine> aSrcKLines;
		aSrcKLines.Copy(MerchNodeUserData.m_aKLinesCompare);
		
		// 看是否要做复权处理
		CArray<CKLine, CKLine> aWeightedKLines;
		
		if ( CIoViewKLine::EWTAft == m_eWeightType )
		{
			// 后复权
			if ( CMerchKLineNode::WeightKLine(aSrcKLines, pMerch->m_aWeightDatas, false, aWeightedKLines) )
			{
				aSrcKLines.Copy(aWeightedKLines);
			}		
		}
		else if ( CIoViewKLine::EWTPre == m_eWeightType )  
		{
			// 前复权
			if ( CMerchKLineNode::WeightKLine(aSrcKLines, pMerch->m_aWeightDatas, true, aWeightedKLines) )
			{
				aSrcKLines.Copy(aWeightedKLines);
			}
		}						
		
		//
		CMarketIOCTimeInfo MarketIOCTimeInfo;
		if (!pMerch->m_Market.GetRecentTradingDay(m_pAbsCenterManager->GetServerTime(), MarketIOCTimeInfo, pMerch->m_MerchInfo))
		{
			//ASSERT(0);
			return false;
		}

		//////////////////////////////////////////////////////////////////////////			
		// 处理数据， 压缩数据
		switch (MerchNodeUserData.m_eTimeIntervalFull)
		{
		case ENTIMinute:
			{
				aFullKlines.SetSize(MerchNodeUserData.m_aKLinesCompare.GetSize());	
				memcpyex(aFullKlines.GetData(), aSrcKLines.GetData(), sizeof(CKLine) * aSrcKLines.GetSize());						
			}
			break;
		case ENTIMinute5:
			{
				aFullKlines.SetSize(aSrcKLines.GetSize());	
				memcpyex(aFullKlines.GetData(), aSrcKLines.GetData(), sizeof(CKLine) * aSrcKLines.GetSize());
			}
			break;					
		case ENTIMinute15:
			{
				if (!CMerchKLineNode::CombinMinuteN(pMerch, 15, aSrcKLines, aFullKlines))					
				{
					//ASSERT(0);
				}	
			}
			break;
		case ENTIMinute30:
			{
				if (!CMerchKLineNode::CombinMinuteN(pMerch, 30, aSrcKLines, aFullKlines))					
				{
					//ASSERT(0);
				}
			}
			break;
		case ENTIMinute60:
			{	
				aFullKlines.SetSize(aSrcKLines.GetSize());	
				memcpyex(aFullKlines.GetData(), aSrcKLines.GetData(), sizeof(CKLine) * aSrcKLines.GetSize());
			}	
			break;
        case ENTIMinute180:
            {
                if (!CMerchKLineNode::CombinMinuteN(pMerch, 180, aSrcKLines, aFullKlines))						
                {
                    //ASSERT(0);
                }
            }
            break;
        case ENTIMinute240:
            {
                if (!CMerchKLineNode::CombinMinuteN(pMerch, 240, aSrcKLines, aFullKlines))						
                {
                    //ASSERT(0);
                }
            }
            break;
		case ENTIMinuteUser:
			{					
				if (!CMerchKLineNode::CombinMinuteN(pMerch, MerchNodeUserData.m_iTimeUserMultipleMinutes, aSrcKLines, aFullKlines))					
				{
					//ASSERT(0);
				}
			}	
			break;
		case ENTIDay:
			{
				aFullKlines.SetSize(aSrcKLines.GetSize());	
				memcpyex(aFullKlines.GetData(), aSrcKLines.GetData(), sizeof(CKLine) * aSrcKLines.GetSize());
			}
			break;
		case ENTIWeek:
			{
				if (!CMerchKLineNode::CombinWeek(aSrcKLines, aFullKlines))
				{
					//ASSERT(0);
				}
			}
			break;
		case ENTIDayUser:
			{
				if (!CMerchKLineNode::CombinDayN(aSrcKLines, MerchNodeUserData.m_iTimeUserMultipleDays, aFullKlines))
				{
					//ASSERT(0);
				}
			}
			break;
		case ENTIMonth:
			{
				aFullKlines.SetSize(aSrcKLines.GetSize());	
				memcpyex(aFullKlines.GetData(), aSrcKLines.GetData(), sizeof(CKLine) * aSrcKLines.GetSize());
			}
			break;			
			
		case ENTIQuarter:
			{
				if (!CMerchKLineNode::CombinMonthN(aSrcKLines, 3, aFullKlines))
				{
					//ASSERT(0);
				}
			}
			break;
		case ENTIYear:
			{
				if (!CMerchKLineNode::CombinMonthN(aSrcKLines, 12, aFullKlines))
				{
					//ASSERT(0);
				}
			}
			break;
		default:
			//ASSERT(0);
			break;
		}
		
		// 实际数据K线合成完成
		// 有可能K线没有均价数据，所以自己合成
		for ( int32 i=aFullKlines.GetSize()-1; i >= 0 ; --i )
		{
			CKLine &kline = aFullKlines[i];

			if (ENTIDay == MerchNodeUserData.m_eTimeIntervalFull)
			{
				if ( kline.m_fPriceAvg == 0.0f )
				{
					// 没有则尝试自己运算下
					// 					if ( kline.m_fVolume >= 0.9f )
					// 					{
					// 						float fAvg = kline.m_fAmount / kline.m_fVolume / 100;
					// 						if ( fAvg > kline.m_fPriceHigh || fAvg < kline.m_fPriceLow )
					// 						{
					// 							// 超过了限制，不要算了
					// 							aFullKlines.RemoveAt(i);
					// 							continue;
					// 						}
					// 						kline.m_fPriceAvg = fAvg;
					// 					}
					kline.m_fPriceAvg = (kline.m_fPriceClose+kline.m_fPriceHigh+kline.m_fPriceLow+kline.m_fPriceOpen)/4.0f;
				}
				if ( kline.m_fPriceAvg > kline.m_fPriceHigh*1.20f || kline.m_fPriceAvg < kline.m_fPriceLow*0.80f )
				{
					// 相差太大，服务器可能有错误均价数据发过来?
					aFullKlines.RemoveAt(i);
					continue;
				}
			}
			else
			{
				kline.m_fPriceAvg = (kline.m_fPriceClose+kline.m_fPriceHigh+kline.m_fPriceLow+kline.m_fPriceOpen)/4.0f;
			}
		}
	}
	// compareK线与fullK线数据准备好了
	// 现在Full已经是要求的K线了
	bool32	bUpdateShowData = false;
	if ( 0 != iCmpResult )
	{
		iCmpResult = CompareKLinesChange(aFullKlines, MerchNodeUserData.m_aKLinesFull);
		if ( 0 != iCmpResult )
		{
			// FullK线要求更新
			MerchNodeUserData.m_aKLinesFull.Copy(aFullKlines);
			bUpdateShowData = true;	// 必须更新显示数据
		}
	}

	// 商品数据计算完毕，计算套利
	if ( bUpdateShowData )
	{
		CalcArbitrageAsyn();
	}
	
	return true;
}

bool32 CIoViewKLineArbitrage::OnCanPanLeft ( CChartRegion* pRegion, CNodeSequence* pNodes, int32 id, int32 iNum )
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

bool32 CIoViewKLineArbitrage::OnCanPanRight ( CChartRegion* pRegion, CNodeSequence* pNodes, int32 id, int32 iNum )
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

bool32 CIoViewKLineArbitrage::GetTips(IN CPoint pt, OUT CString& StrTips, OUT CString &StrTitle)
{
	// 得到Tips
	StrTips = L"";
	return false;
}

void CIoViewKLineArbitrage::OnNodesRelease ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes )
{
	//T_MerchNodeUserData* pData = (T_MerchNodeUserData*)pNodes->GetUserData();
	//pData->pMerch已经不被这个Curve使用了,如果没有其它Curve使用,则考虑清除数据,并且从AttendMerch中删除.
}

void CIoViewKLineArbitrage::OnRegionMenu2 ( CChartRegion* pRegion, CDrawingCurve* pCurve, int32 x, int32 y )
{
	RegionMenu2 ( pRegion, pCurve, x, y );
}

void CIoViewKLineArbitrage::OnRegionIndexMenu ( CChartRegion* pRegion, CDrawingCurve* pCurve, int32 x, int32 y )
{
	RegionIndexMenu ( pRegion, pCurve, x, y );
}

void CIoViewKLineArbitrage::OnRegionIndexBtn(CChartRegion* pRegion, CDrawingCurve* pCurve, int32 iID )
{
	RegionIndexBtn (pRegion, pCurve, iID);
}

bool32 CIoViewKLineArbitrage::LoadAllIndex(CNewMenu* pMenu)
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

void CIoViewKLineArbitrage::GetOftenFormulars(IN CChartRegion* pRegion, OUT CStringArray& aFormulaNames, OUT int32& iSeperatorIndex, bool32 bDelSame /*= true*/)
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

int32 CIoViewKLineArbitrage::LoadOftenIndex(CNewMenu* pMenu)
{
	if ( NULL == pMenu )
	{
		return -1;
	}

	//
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
	int32 iSeperatorIndex = 0;
	GetOftenFormulars(m_pRegionPick, m_FormulaNames, iSeperatorIndex);

	int32 iIndexNums = 0;
	
	for (int32 i = 0; i < m_FormulaNames.GetSize(); i ++ )
	{
		if ( i >= (IDM_IOVIEWKLINE_OFTENINDEX_END - IDM_IOVIEWKLINE_OFTENINDEX_BEGIN) ) break;
		
		if ( -1 != iSeperatorIndex && i == iSeperatorIndex && 0 != iSeperatorIndex)
		{
			pMenu->AppendODMenu(L"",MF_SEPARATOR);
		}
		
		iIndexNums += 1;
		
		CString StrName = m_FormulaNames.GetAt(i);
		CFormularContent* pFormular = CFormulaLib::instance()->GetFomular(StrName);
		
		CString StrText = pFormular->name;		
		CString StrexplainBrief = pFormular->explainBrief;

		CString StrIndexFull = MakeMenuString(StrText, StrexplainBrief);
		
		pMenu->AppendODMenu(StrIndexFull, MF_STRING, IDM_IOVIEWKLINE_OFTENINDEX_BEGIN + i);	
 	}

	return iIndexNums;
}

void CIoViewKLineArbitrage::OnRegionMenu ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes,CNodeData* pNodeData, int32 x, int32 y)
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
			iRegion = i+1;
			int32 iOld = m_iLastPickSubRegionIndex;
			m_iLastPickSubRegionIndex = i;	// 设定最后一个选定的region
			if ( iOld != m_iLastPickSubRegionIndex )
			{
				m_pRegionMain->SetDrawFlag(m_pRegionMain->GetDrawFlag() | CRegion::KDrawFull);
				Invalidate();
			}
		}
	}
	if ( pRegion == m_pRegionYLeft )
	{
		// 左Y轴，换坐标
		CNewMenu menu;
		menu.CreatePopupMenu();

		const UINT uBase = 100;
		menu.AppendMenu(MF_SEPARATOR);

		menu.AppendMenu(MF_STRING, uBase+CPriceToAxisYObject::EAYT_Normal, _T("普通坐标"));
		menu.AppendMenu(MF_STRING, uBase+CPriceToAxisYObject::EAYT_Pecent, _T("百分比坐标"));
		//menu.AppendMenu(MF_STRING, uBase+CPriceToAxisYObject::EAYT_Log,	   _T("对数坐标"));

		menu.CheckMenuItem(uBase+GetMainCurveAxisYType(), MF_BYCOMMAND |MF_CHECKED);

		menu.DeleteMenu(0, MF_BYPOSITION);

		CPoint pt(x,y);
		ClientToScreen(&pt);

		UINT uRet = menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL |TPM_NONOTIFY |TPM_RETURNCMD,pt.x, pt.y, AfxGetMainWnd());
		// 不发送给mainframe了，自己处理
		if ( uRet >= uBase )
		{
			SetMainCurveAxisYType((CPriceToAxisYObject::E_AxisYType)(uRet-uBase));	// 变更坐标类型
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
	
	//
	CNewMenu menu;						  
	//
	menu.LoadMenu(IDR_MENU_KLINEARB);
	menu.LoadToolBar(g_awToolBarIconIDs);

	CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
	CMenu* pTempMenu = NULL;
	CNewMenu* pSubMenu = NULL;
	//

	// 套利
	pTempMenu = pPopMenu->GetSubMenu(_T("套利价差类型"));
	ASSERT( NULL != pTempMenu );
	if ( NULL != pTempMenu )
	{
		pTempMenu->CheckMenuItem(ID_ARBKLINE_CLOSEDIFF+m_eArbitrageShow, MF_BYCOMMAND|MF_CHECKED);
	}

	//////////////////////////////////////////////////////////////////////////
	pTempMenu = pPopMenu->GetSubMenu(_T("所有指标"));
	ASSERT(NULL!=pTempMenu);

	CNewMenu* pAllIndexMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );	
	LoadAllIndex(pAllIndexMenu);

	//
	pTempMenu = pPopMenu->GetSubMenu(_T("常用指标"));
	ASSERT(NULL!=pTempMenu);
	
	pSubMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );
	pSubMenu->RemoveMenu(0,MF_BYPOSITION|MF_SEPARATOR);

	int32 iIndexNums = LoadOftenIndex(pSubMenu);

	if ( 0 == iIndexNums )
	{
		// 没有指标的时候
		pPopMenu->DeleteMenu((UINT)pSubMenu->GetSafeHmenu(), MF_BYCOMMAND);
		pTempMenu = NULL;
		pSubMenu = NULL;
	}

	// 主图坐标
	if ( m_pRegionPick == m_pRegionMain )
	{
		// 打勾
		pTempMenu = pPopMenu->GetSubMenu(L"主图坐标");
		
		if ( NULL != pTempMenu )
		{
			CNewMenu* pMainAxisYMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu);
			
			pMainAxisYMenu->CheckMenuItem(ID_KLINE_AXISYNORMAL+GetMainCurveAxisYType(), MF_CHECKED|MF_BYCOMMAND);
		}
	}
	else
	{
		// 删除该菜单项
		pTempMenu = pPopMenu->GetSubMenu(L"主图坐标");
		if ( NULL != pTempMenu )
		{
			pPopMenu->DeleteMenu((UINT)pTempMenu->GetSafeHmenu(), MF_BYCOMMAND);
			pTempMenu = NULL;
		}
	}

	// 插入内容:
	pTempMenu = pPopMenu->GetSubMenu(L"插入内容");
	ASSERT(NULL!=pTempMenu);
	CNewMenu * pIoViewPopMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );
	AppendIoViewsMenu(pIoViewPopMenu, IsLockedSplit());

	// 分析周期
	pTempMenu = pPopMenu->GetSubMenu(_T("选择周期"));
	ASSERT(NULL!=pTempMenu);
	pSubMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );
	
	i = IDM_CHART_KMINUTE + pData->m_eTimeIntervalFull;
	pSubMenu->CheckMenuItem(i,MF_BYCOMMAND|MF_CHECKED); 

	//如果不是主Chart,则不显示"下方增加区域"
	if ( m_pRegionPick != m_pRegionMain)
	{	
		//menu.RemoveMenu(IDM_CHART_INSERT, MF_BYCOMMAND);		// 同时显示
		//如果子区域达到6个,则不再允许增加
		if ( m_SubRegions.GetSize() > 5 )
		{			
			menu.RemoveMenu(IDM_CHART_INSERT, MF_BYCOMMAND);
		}
	}
	//如果是主chart
	else
	{
		//如果子区域达到6个,则不再允许增加
		if ( m_SubRegions.GetSize() > 5 )
		{			
			menu.RemoveMenu(IDM_CHART_INSERT, MF_BYCOMMAND);
		}
		//不显示"删除区域"
		//menu.RemoveMenu(IDM_CHART_DELETE, MF_BYCOMMAND);	// 同时显示
		//如果子区域达到6个,则不再允许增加
		if ( m_iLastPickSubRegionIndex < 0 || m_iLastPickSubRegionIndex >= m_SubRegions.GetSize() )
		{			
			menu.RemoveMenu(IDM_CHART_DELETE, MF_BYCOMMAND);	// 无选中subregion
		}
	}

 	if ( NULL != m_pCurvePick && CheckFlag(m_pCurvePick->m_iFlag,CChartCurve::KTypeIndex))
 	{
 		//NULL;
 	}
 	else
	{
		pTempMenu = pPopMenu->GetSubMenu(L"当前指标");
		if ( NULL != pTempMenu )
		{
			pPopMenu->DeleteMenu((UINT)pTempMenu->GetSafeHmenu(), MF_BYCOMMAND);
			pTempMenu = NULL;
		}
	}

	// 图标:
	pPopMenu->ModifyODMenu(L"选择周期", L"选择周期", IDB_TOOLBAR_KLINECYLE);
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
  
void CIoViewKLineArbitrage::UpdateKLineStyle()
{
	CChartCurve *pCurveDependent = m_pRegionMain->GetDependentCurve();
	if (NULL == pCurveDependent)
		return;

	CChartDrawer* pKLineDrawer = (CChartDrawer*)pCurveDependent->GetDrawer();
	if (NULL == pKLineDrawer)
		return;

	CChartDrawer::E_ChartDrawStyle eDrawStyle = pKLineDrawer->m_eChartDrawType;
	int32 i, iSize = m_pRegionMain->GetCurveNum();
	for ( i = 0; i < iSize; i ++ )
	{
		CChartCurve* pCurve = m_pRegionMain->GetCurve(i);
		if ( CheckFlag(pCurve->m_iFlag,CChartCurve::KTypeKLine))
		{
			CChartDrawer* pDrawer = (CChartDrawer*)pCurve->GetDrawer();
			pDrawer->m_eChartDrawType = eDrawStyle;
		}
	}
}

void CIoViewKLineArbitrage::OnUserCircleChanged(E_NodeTimeInterval eNodeInterval, int32 iValue)
{
	T_MerchNodeUserData *pData = GetMainData();
	if ( NULL != pData )
	{
        SetTopButtonStatus(eNodeInterval, iValue);

		if ( ENTIMinuteUser == eNodeInterval)
		{
			if ( iValue >= USERSET_MINUET_MIN && iValue <= USERSET_MINUET_MAX )
			{
				pData->m_iTimeUserMultipleMinutes = iValue;
			}
		}
		else if (ENTIDayUser == eNodeInterval)
		{
			if ( iValue >= USERSET_DAY_MIN && iValue <= USERSET_DAY_MAX )
			{
				pData->m_iTimeUserMultipleDays = iValue;
			}
		}
		
		CalcArbitrage();
		SetCurveTitle(pData);
	}
	
	ReDrawAysnc();
}

void CIoViewKLineArbitrage::OnAllIndexMenu(UINT nID)
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

void CIoViewKLineArbitrage::OnOftenIndexMenu(UINT nID)
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

bool32 CIoViewKLineArbitrage::AddShowIndex( const CString &StrIndexName, bool32 bDelRegionAllIndex /*= false*/, bool32 bChangeStockByIndex /*= false*/, bool32 bShowUserRightDlg/*=false*/ )
{
	T_MerchNodeUserData* pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == m_pRegionMain || NULL == pData )
	{
		return false;
	}

	const CFormularContent *pFormulaContent = CFormulaLib::instance()->GetFomular(StrIndexName);
	if ( NULL == pFormulaContent )
	{
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
				AddIndexToSubRegion(StrIndexName, true, m_iLastPickSubRegionIndex);	// 没有发现有
			}
		}
	}

	return true;
}

void CIoViewKLineArbitrage::ReplaceIndex( const CString &StrOldIndex, const CString &StrNewIndex, bool32 bDelRegionAllIndex/* = false*/, bool32 bChangeStock/* = false*/ )
{
	// 指标是否有效
	const CFormularContent *pFormulaContentOld = CFormulaLib::instance()->GetFomular(StrOldIndex);
	if ( NULL == pFormulaContentOld )
	{
		return;	// 无此指标
	}
	const CFormularContent *pFormulaContentNew = CFormulaLib::instance()->GetFomular(StrNewIndex);
	if ( NULL == pFormulaContentNew )
	{
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

void CIoViewKLineArbitrage::ChangeIndexToMainRegion( const CString &StrIndexName )
{
	T_MerchNodeUserData* pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == m_pRegionMain || NULL == pData )
	{
		return;
	}
	
	// 得到能添加的所有指标名称
	CFormularContent *pFormulaContent = (CFormularContent *)CFormulaLib::instance()->GetFomular(StrIndexName);
	if (NULL != pFormulaContent && pFormulaContent->BePassedXday())
	{
		MessageBox(L"指标已经过期!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return;
	}
	
	//
	if ( NULL == pFormulaContent || !CheckFlag(pFormulaContent->flag, CFormularContent::KAllowMain) )
	{
		// 还应判断类型为normal TODO
		return;	// 无此指标或非主图指标， 不处理
	}
	
	bool32 bExist = false;
	// 检查是否存在该index - 怎么样判断呢？？
	for ( int32 i=0; i < pData->aIndexs.GetSize() ; i++ )
	{
		// 由于实际创建index也是根据name来创建，so简单判断名字+region
		if ( pData->aIndexs[i]->strIndexName == StrIndexName && m_pRegionMain == pData->aIndexs[i]->pRegion )
		{
			bExist = true;
			break;
		}
	}
	
	// 	m_pRegionPick = m_pRegionMain;	// delcurrent需要
	// 	DelCurrentIndex();
	ClearRegionIndex(m_pRegionMain); 
	
	//
	if (bExist)
	{
		return;
	}
	
	T_IndexParam *pIndex = AddIndex(m_pRegionMain, StrIndexName);
	if ( NULL == pIndex )
	{
		// 怎么挽救？？
		return;
	}
	
	int32 iNodeBegin,iNodeEnd;
	GetNodeBeginEnd(iNodeBegin,iNodeEnd);
	g_formula_compute ( this, this, (CChartRegion*)pIndex->pRegion, pData, pIndex, iNodeBegin,iNodeEnd );
	
	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	UpdateSelfDrawCurve();
	Invalidate();
}

void CIoViewKLineArbitrage::AddIndexToSubRegion( const CString &StrIndexName, bool32 bAddSameIfExist/* = true*/, int32 iSubRegionIndex/* = -1*/ )
{
	T_MerchNodeUserData* pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == m_pRegionMain || NULL == pData )
	{
		return;
	}

	int32 i = 0;

	const CFormularContent *pFormulaContent = CFormulaLib::instance()->GetFomular(StrIndexName);
	if ( NULL == pFormulaContent || !CheckFlag(pFormulaContent->flag, CFormularContent::KAllowSub) )
	{
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

void CIoViewKLineArbitrage::AddIndexToSubRegion( const CString &StrIndexName, CChartRegion *pSubRegion )
{
	T_MerchNodeUserData* pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == m_pRegionMain || NULL == pData )
	{
		return;
	}

	if ( NULL == pSubRegion )
	{
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

void CIoViewKLineArbitrage::RemoveShowIndex( const CString &StrIndexName )
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

void CIoViewKLineArbitrage::ClearRegionIndex( CChartRegion *pRegion, bool32 bDelExpertTip/* = false*/ )
{
	// 删除已有的指标
	if( NULL == pRegion )
	{
		return;
	}

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
	
	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	UpdateSelfDrawCurve();
	Invalidate();
}

void CIoViewKLineArbitrage::OnOwnIndexMenu(UINT nID)
{
	//
	DelCurrentIndex();
}

void CIoViewKLineArbitrage::KLineCycleChange(UINT nID)
{
	if ( nID >= IDM_CHART_KMINUTE && nID <= IDM_CHART_KMINUTE240 )
	{		
		// 只需要设置主的就可以了

		T_MerchNodeUserData* pData = GetMainData();
		if ( NULL != pData )
		{
			E_NodeTimeInterval NodeInterval = (E_NodeTimeInterval)(nID - IDM_CHART_KMINUTE);

			if ( ENTIDayUser == NodeInterval && pData->bMainMerch )
			{
				CDlgMenuUserCycleSet Dlg;
				Dlg.SetIoViewParent(this);
				Dlg.SetInitialParam(ENTIDayUser, pData->m_iTimeUserMultipleDays);				
				if ( IDOK != Dlg.DoModal() )
				{
					return;
				}
			}
			else if ( ENTIMinuteUser == NodeInterval && pData->bMainMerch )
			{
				CDlgMenuUserCycleSet Dlg;
				Dlg.SetIoViewParent(this);
				Dlg.SetInitialParam(ENTIMinuteUser, pData->m_iTimeUserMultipleMinutes);				
				if ( IDOK != Dlg.DoModal() )
				{
					return;
				}
			}

			SetTimeInterval(*pData, NodeInterval);
			SetCurveTitle(pData);
		}
		// 请求数据
		RequestViewData();

		// 
		UpdateSelfDrawCurve();
		m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
		Invalidate();
	}
}

void CIoViewKLineArbitrage::OnMenu ( UINT nID )
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

    if ( nID >= IDM_CHART_KMINUTE && nID <= IDM_CHART_KMINUTE240 )
    {		
        // 只需要设置主的就可以了

        T_MerchNodeUserData* pData = GetMainData();
        if ( NULL != pData )
        {
            E_NodeTimeInterval NodeInterval = (E_NodeTimeInterval)(nID - IDM_CHART_KMINUTE);

            if ( ENTIDayUser == NodeInterval && pData->bMainMerch )
            {
                CDlgMenuUserCycleSet Dlg;
                Dlg.SetIoViewParent(this);
                Dlg.SetInitialParam(ENTIDayUser, pData->m_iTimeUserMultipleDays);				
                if ( IDOK != Dlg.DoModal() )
                {
                    return;
                }
            }
            else if ( ENTIMinuteUser == NodeInterval && pData->bMainMerch )
            {
                CDlgMenuUserCycleSet Dlg;
                Dlg.SetIoViewParent(this);
                Dlg.SetInitialParam(ENTIMinuteUser, pData->m_iTimeUserMultipleMinutes);				
                if ( IDOK != Dlg.DoModal() )
                {
                    return;
                }
            }

            CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
            if(pMain)
            {
                switch(NodeInterval)
                {
                case ENTIMinute:
                    {
                        pMain->m_LastTimeSet = 1;
                    }
                    break;
                case ENTIMinute5:
                    {
                        pMain->m_LastTimeSet = 5;
                    }
                    break;
                case ENTIMinute15:
                    {
                        pMain->m_LastTimeSet = 15;
                    }
                    break;
                case ENTIMinute30:
                    {
                        pMain->m_LastTimeSet = 30;
                    }
                    break;
                case ENTIMinute60:
                    {
                        pMain->m_LastTimeSet = 60;
                    }
                    break;
                case ENTIMinute180:
                    {
                        pMain->m_LastTimeSet = 180;
                    }
                    break;
                case ENTIMinute240:
                    {
                        pMain->m_LastTimeSet = 240;
                    }
                    break;
                case ENTIDay:
                    {
                        pMain->m_LastTimeSet = 1440;
                    }
                    break;
                case ENTIWeek:
                    {
                        pMain->m_LastTimeSet = 1440*7;
                    }
                    break;

                case ENTIMonth:
                    {
                        pMain->m_LastTimeSet = 1440*30;
                    }
                    break;

                case ENTIYear:
                    {
                        pMain->m_LastTimeSet = 1440*365;
                    }
                    break;

                default:
                    {
                    }
                    break;
                }

                SetTimeInterval(*pData, NodeInterval);
                SetCurveTitle(pData);
            }
        }

        //请求数据
        RequestViewData();

        UpdateSelfDrawCurve();
        m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
        Invalidate();
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
	else if ( ID_KLINE_AXISYNORMAL <= nID
		&& ID_KLINE_AXISYLOG >= nID )
	{
		SetMainCurveAxisYType((CPriceToAxisYObject::E_AxisYType)(nID-ID_KLINE_AXISYNORMAL));
	}
	else if ( ID_ARBKLINE_CLOSEDIFF <= nID && ID_ARBKLINE_END > nID )
	{
		// 套利价差类型
		E_ArbitrageChartShowType eArb = (E_ArbitrageChartShowType)(nID-ID_ARBKLINE_CLOSEDIFF);
		if ( eArb != m_eArbitrageShow )
		{
			SetArbShowType(eArb);
			CalcArbitrage();
			bNeedReDraw = true;
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


LRESULT CIoViewKLineArbitrage::OnMessageTitleButton(WPARAM wParam,LPARAM lParam)
{
	//
	int32 uID = (int32)wParam;
	//
	
	if ( 0 == m_MerchParamArray.GetSize() )
	{
		return 0 ;
	}
	
// 	if ( NULL == m_pRegionPick)		// xl 由于TBWnd上的弹出菜单使m_pRegionPick为Null，实际这个只在某些选项需要判断
// 	{								//    
// 		return 0;
// 	}	
	
	if ( 0 == uID)
	{
		m_pRegionPick = m_pRegionMain;		// 周期菜单
		
		CNewMenu menu;
		menu.LoadMenu(IDR_MENU_KLINE);
		menu.LoadToolBar(g_awToolBarIconIDs);

		CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));

		CMenu* pTempMenu = pPopMenu->GetSubMenu(_T("选择周期"));
		ASSERT(NULL!=pTempMenu);
		CNewMenu* pSubMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );
		
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
		
		if (NULL != pData)
		{
			int32 iCurCheck = IDM_CHART_KMINUTE + pData->m_eTimeIntervalFull;
			pSubMenu->CheckMenuItem(iCurCheck,MF_BYCOMMAND|MF_CHECKED); 
		}

		CPoint pt;
		GetCursorPos(&pt);
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, this);
		menu.DestroyMenu();

	}
	else if ( 1 == uID )
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

void CIoViewKLineArbitrage::OnPickNode ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes, CNodeData* pNodeData,int32 x,int32 y,int32 iFlag )
{
	if ( NULL != pNodeData )
	{
		CString StrText;
		T_MerchNodeUserData* pData = (T_MerchNodeUserData*)pNodes->GetUserData();
		ASSERT(NULL!=pData);
		CGmtTime cgTime(pNodeData->m_iID);
		CString StrTime = Time2String ( cgTime, pData->m_eTimeIntervalFull);
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

void CIoViewKLineArbitrage::MenuAddRegion()
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

	m_iLastPickSubRegionIndex = iSize-1;	// 新的region为聚焦副图

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

void CIoViewKLineArbitrage::SetXmlSource ()
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

void CIoViewKLineArbitrage::DelCurrentIndex()
{
	// 删除已有的指标
	if( NULL == m_pRegionPick )
	{
		return;
	}

	ClearRegionIndex(m_pRegionPick);
}

void CIoViewKLineArbitrage::MenuAddIndex ( int32 id )
{

}

void CIoViewKLineArbitrage::MenuDelRegion()
{
	// 同时存在删除与添加，so需要变动regionpick
	if ( m_pRegionPick == m_pRegionMain || NULL == m_pRegionPick )
	{
		// 变为响应的副图
		if ( m_iLastPickSubRegionIndex >= 0 && m_iLastPickSubRegionIndex < m_SubRegions.GetSize() )
		{
			m_pRegionPick = m_SubRegions[m_iLastPickSubRegionIndex].m_pSubRegionMain;
		}
		else
		{
			ASSERT( 0 );
			return;	// 没有选中的副图
		}	
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
	// 更改选中的为下一个region
	if ( m_iLastPickSubRegionIndex >=0 && m_SubRegions.GetSize() > 0 )
	{
		if ( m_iLastPickSubRegionIndex >= m_SubRegions.GetSize() )
		{
			m_iLastPickSubRegionIndex = m_SubRegions.GetUpperBound();
		}
		m_pRegionPick = m_SubRegions[m_iLastPickSubRegionIndex].m_pSubRegionMain;
	}
	else
	{
		m_iLastPickSubRegionIndex = 0;
		if ( m_SubRegions.GetSize() > 0 )
		{
			m_pRegionPick = m_SubRegions[m_iLastPickSubRegionIndex].m_pSubRegionMain;
		}
	}

	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	RedrawWindow();
}

void CIoViewKLineArbitrage::MenuDelCurve()
{
	if ( NULL == m_pCurvePick || NULL == m_pRegionPick )
	{
		return;
	}
	CChartRegion &cRegion = m_pCurvePick->GetChartRegion();
	DeleteIndexCurve ( m_pCurvePick );
	if ( cRegion.GetCurveNum() <= 0 && &cRegion != m_pRegionMain )
	{
		DelSubRegion(&cRegion);	// 删除该region
	}

	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	UpdateSelfDrawCurve();
	Invalidate();
}

void CIoViewKLineArbitrage::OnDropCurve ( CChartRegion* pSrcRegion,CChartCurve* pSrcCurve,CChartRegion* pDestChart)
{
	if ( NULL == pDestChart )
	{
		DeleteIndexCurve(pSrcCurve);
	}
	//else
	//{
	//	DragDropCurve( pSrcCurve,pDestChart);
	//}

	UpdateSelfDrawCurve();

	//OnVDataMerchKLineUpdate(m_pMerchXml);
	CalcArbitrage();
	ReDrawAysnc();
}

void CIoViewKLineArbitrage::OnRegionCurvesNumChanged ( CChartRegion* pRegion,int32 iNum )
{
	
}

void CIoViewKLineArbitrage::OnCrossData ( CChartRegion* pRegion,int32 iPos,CNodeData& NodeData, float fPricePrevClose, float fYValue, bool32 bShow)
{
	if ( m_MerchParamArray.GetSize() < 1 ) return;
	
	if ( bShow )
	{
		T_MerchNodeUserData* pData = GetMainData();
		int32 iSaveDec = m_Arbitrage.m_iSaveDec;

		CKLine kLine; 
		NodeData2KLine(NodeData, kLine);
		
		if ( pRegion == m_pRegionMain )
		{
			// 记录下当前这根K线
			m_KLineCrossNow = kLine;
		}
		
		CString StrTimeLine1 = L"";
		CString StrTimeLine2 = L"";
		
		Time2String(kLine.m_TimeCurrent,pData->m_eTimeIntervalFull,StrTimeLine1,StrTimeLine2);
		
		if ( pRegion == m_pRegionMain )
		{
			SetFloatData(&m_GridCtrlFloat,kLine,fPricePrevClose,fYValue,StrTimeLine1,StrTimeLine2,iSaveDec);
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

void CIoViewKLineArbitrage::OnCrossNoData(CString StrTime,bool32 bShow)
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
		CRect rect(0,0,0,0);
		EnableClipDiff(false,rect);
	}
}

void CIoViewKLineArbitrage::SetFloatData ( CGridCtrl* pGridCtrl,float fCursorValue,int32 iSaveDec)
{
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

void CIoViewKLineArbitrage::SetFloatData ( CGridCtrl* pGridCtrl,CKLine& kLine,float fPricePrevClose,float fCursorValue,CString StrTimeLine1,CString StrTimeLine2,int32 iSaveDec)
{
	if ( !m_bInitialFloat )
	{
		InitCtrlFloat(&m_GridCtrlFloat);
	}

    GetMainData();

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
	Float2SymbolStringLimitMaxLength(StrValue, kLine.m_fPriceOpen, fPricePrevClose, iSaveDec, iYLeftShowChar, true, false, true, false, true);
	pCellSymbol->SetText(StrValue);

	//收盘价
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(7,0);
	//StrValue = Float2SymbolString(KLine.m_fPriceHigh, fPricePrevClose, iSaveDec) + _T(" ");
	Float2SymbolStringLimitMaxLength(StrValue, kLine.m_fPriceClose, fPricePrevClose, iSaveDec, iYLeftShowChar, true, false, true, false, true);
	pCellSymbol->SetText(StrValue);

	//均价
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(9,0);
	//StrValue = Float2SymbolString(KLine.m_fPriceHigh, fPricePrevClose, iSaveDec) + _T(" ");
	Float2SymbolStringLimitMaxLength(StrValue, kLine.m_fPriceAvg, fPricePrevClose, iSaveDec, iYLeftShowChar, true, false, true, false, true);
	pCellSymbol->SetText(StrValue);

	pGridCtrl->RedrawWindow();

	// 
	if ( !pGridCtrl->IsWindowVisible() )
	{
		pGridCtrl->ShowWindow(SW_SHOW|SW_SHOWNOACTIVATE);
		
		CRect rect;
		pGridCtrl->GetClientRect(&rect);
		
		OffsetRect(&rect,m_PtGridCtrl.x,m_PtGridCtrl.y);
		EnableClipDiff(true,rect);
		
		pGridCtrl->RedrawWindow();					
	}	
}

void CIoViewKLineArbitrage::ClipGridCtrlFloat (CRect& rect)
{
	EnableClipDiff(true,rect);
}

CString CIoViewKLineArbitrage::OnTime2String ( CGmtTime& Time )
{
	CString StrTime = Time2String ( Time,ENTIMinute );
	if ( m_MerchParamArray.GetSize() > 0 )
	{
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
		StrTime = Time2String(Time,pData->m_eTimeIntervalFull);
	}
	return StrTime;
}

CString CIoViewKLineArbitrage::OnFloat2String ( float fValue, bool32 bZeroAsHLine, bool32 bNeedTerminate/* = false*/ )
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
	return Float2String(fValue,iSaveDec,false,bZeroAsHLine);
}

void CIoViewKLineArbitrage::OnCalcXAxis(CChartRegion* pRegion, CDC* pDC, CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aXAxisDivide)
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
	
void CIoViewKLineArbitrage::OnCalcYAxis(CChartRegion* pRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide)
{
//	DWORD dwB = timeGetTime();

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

	//DWORD dwE = timeGetTime();
	//CString StrTrace;
	//StrTrace.Format(L"%s   CalcYAxis 耗时: %d ms \r\n", pRegion->m_StrName.GetBuffer(), (dwE - dwB));
	//TRACE(StrTrace);
}

///////////////////////////////////////////////////
//
void CIoViewKLineArbitrage::OnRegionDrawNotify(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC,CRegion* pRegion,E_RegionDrawNotifyType eType)
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
			/*
			{
				CAxisDivide *pAxisDivide = (CAxisDivide *)m_pRegionMain->m_aXAxisDivide.GetData();
				for (int32 i = 0; i < m_pRegionMain->m_aXAxisDivide.GetSize(); i++)
				{
					CAxisDivide &AxisDivide = pAxisDivide[i];
					pt1.x = pt2.x = AxisDivide.m_iPosPixel;
					pt1.y = RectMain.top;
					pt2.y = RectMain.bottom;
					
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
			*/
		}
		else if ( pChartRegion == m_pRegionYLeft)
		{
			CRect RectMain = m_pRegionMain->GetRectCurves();
			CRect RectYLeft = m_pRegionYLeft->GetRectCurves();
			RectYLeft.DeflateRect(3, 3, 3, 3);

			// xl 0607 主图小于一定值，不要画
			if ( RectMain.Height() >= 8 )
			{
				// 画左文字 - 加画左横线
				CAxisDivide *pAxisDivide = (CAxisDivide *)m_pRegionMain->m_aYAxisDivide.GetData();
				for (int32 i = 0; i < m_pRegionMain->m_aYAxisDivide.GetSize(); i++)
				{
					CAxisDivide &AxisDivide = pAxisDivide[i];
					
					CRect rect = RectYLeft;
					
					CRect RectTest(0, 0, 0, 0);
					pDC->DrawText(AxisDivide.m_DivideText1.m_StrText, &RectTest, AxisDivide.m_DivideText1.m_uiTextAlign |DT_CALCRECT);

					int32 iTmpHeight = RectTest.Height();
					int32 iMyHalfHeight = max(iHalfHeight, iTmpHeight/2);
					int32 iMyFontHeight = max(iFontHeight, iTmpHeight);

					if ((AxisDivide.m_DivideText1.m_uiTextAlign & DT_VCENTER) != 0)						
					{
						rect.top	= AxisDivide.m_iPosPixel - iMyHalfHeight;
						rect.bottom = AxisDivide.m_iPosPixel + iMyHalfHeight;
					}
					else if ((AxisDivide.m_DivideText1.m_uiTextAlign & DT_BOTTOM) != 0)
					{
						rect.bottom = AxisDivide.m_iPosPixel;
						rect.top	= rect.bottom - iMyFontHeight;							
					}
					else
					{
						rect.top	= AxisDivide.m_iPosPixel;
						rect.bottom	= rect.top + iMyFontHeight;
					}
					
					// 
					if ( rect.bottom > RectYLeft.bottom )
					{					
						rect.bottom	= RectYLeft.bottom;
						rect.top	= rect.bottom - iMyFontHeight;
					}
					
					pDC->MoveTo(RectMain.left, AxisDivide.m_iPosPixel);
					pDC->LineTo(rect.right-1, AxisDivide.m_iPosPixel);
					pDC->SetTextColor(AxisDivide.m_DivideText1.m_lTextColor);
					pDC->SetBkMode(TRANSPARENT);
					pDC->DrawText(AxisDivide.m_DivideText1.m_StrText, &rect, AxisDivide.m_DivideText1.m_uiTextAlign);
				}
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
			
				CRect rect = RectYRight;
				if ((AxisDivide.m_DivideText2.m_uiTextAlign & DT_VCENTER) != 0)						
				{
					rect.top	= AxisDivide.m_iPosPixel - 50;
					rect.bottom = AxisDivide.m_iPosPixel + 50;
				}
				else if ((AxisDivide.m_DivideText2.m_uiTextAlign & DT_BOTTOM) != 0)
				{
					rect.bottom = AxisDivide.m_iPosPixel;
					rect.top	= rect.bottom - 100;							
				}
				else
				{
					rect.top	= AxisDivide.m_iPosPixel;
					rect.bottom	= rect.top + 100;
				}
				
				// 
				pDC->SetTextColor(AxisDivide.m_DivideText2.m_lTextColor);
				pDC->SetBkMode(TRANSPARENT);
				pDC->DrawText(AxisDivide.m_DivideText2.m_StrText, &rect, AxisDivide.m_DivideText2.m_uiTextAlign);
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
		//	UINT uiTimeStart = timeGetTime();
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

				bool32	bFirstLevelTop = true;
				for (i = 0; i < iAxisDivideCount; i++)
				{
					CSize SizeText = pDC->GetOutputTextExtent(pAxisDivide[i].m_DivideText1.m_StrText);

					CRect rct = RectXBottom;
					if ((pAxisDivide[i].m_DivideText1.m_uiTextAlign & DT_CENTER) != 0)						
					{
						rct.left	= pAxisDivide[i].m_iPosPixel - (SizeText.cx / 2 + iWidthSpace);
						rct.right	= pAxisDivide[i].m_iPosPixel + (SizeText.cx / 2 + iWidthSpace);
					}
					else if ((pAxisDivide[i].m_DivideText1.m_uiTextAlign & DT_RIGHT) != 0)
					{
						rct.right	= pAxisDivide[i].m_iPosPixel + iWidthSpace;
						rct.left	= pAxisDivide[i].m_iPosPixel - (SizeText.cx + iWidthSpace);
					}
					else  // 左对齐的在PosPixel位置加画小竖线，最左侧一个可以不画
					{
						rct.left	= pAxisDivide[i].m_iPosPixel - iWidthSpace;
						rct.right	= pAxisDivide[i].m_iPosPixel + (SizeText.cx + iWidthSpace);
					}

					// 从空闲空间中查找， 看看是否可以显示
					int32 iPosFindInSpaceRects = -1;

					CRect *pRectSpace = (CRect *)aRectSpace.GetData();
					for (int32 iIndexSpace = 0; iIndexSpace < aRectSpace.GetSize(); iIndexSpace++)
					{
						if (rct.left < pRectSpace[iIndexSpace].left || rct.right > pRectSpace[iIndexSpace].right)
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
						// 画个线看
						pDC->MoveTo(pAxisDivide[i].m_iPosPixel, rct.top);
						int32 iBottom = rct.CenterPoint().y;
						if ( pAxisDivide[i].m_eLevelLine == CAxisDivide::EDLLevel1 ) // 貌似是现有k线里最高的
						{
							if ( bFirstLevelTop )		// 第一条最高显示线不画竖线
							{
								iBottom = rct.top;
								bFirstLevelTop = false;
							}
							else
							{
								iBottom = rct.bottom;
							}
						}
						pDC->LineTo(pAxisDivide[i].m_iPosPixel, iBottom);
						pDC->SetTextColor(pAxisDivide[i].m_DivideText1.m_lTextColor);
						pDC->SetBkMode(TRANSPARENT);
						rct.left = pAxisDivide[i].m_iPosPixel + 1; // 跳过那个小竖线，并且设置文字为左对齐
						pDC->DrawText(pAxisDivide[i].m_DivideText1.m_StrText, &rct, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

						// 拆分空闲区域
						CRect RectSubLeft = pRectSpace[iPosFindInSpaceRects];
						CRect RectSubRight = pRectSpace[iPosFindInSpaceRects];

						RectSubLeft.right = rct.left - 1;
						RectSubRight.left = rct.right + 1;

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

			//UINT uiTimeEnd = timeGetTime();
			//CString StrTrace;
			//StrTrace.Format(L"draw::: %dms \n", uiTimeEnd - uiTimeStart);
			//TRACE(StrTrace);
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
				//完整时间
				COLORREF clr = 0xffffff;
				clr = GetIoViewColor(ESCText);
				pDC->SetTextColor(clr);
				int OldMode = pDC->GetBkMode();
				pDC->SetBkMode(TRANSPARENT);

				CRect rect = m_pRegionLeftBottom->GetRectCurves();
				pDC->DrawText(StrDay,rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE );
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
				bool32 bLastPickSub = i == m_iLastPickSubRegionIndex;
				if (pChartRegion == SubRegion.m_pSubRegionMain)
				{
					CRect RectSubMain = SubRegion.m_pSubRegionMain->GetRectCurves();
			
					{
						CAxisDivide *pAxisDivide = (CAxisDivide *)SubRegion.m_pSubRegionMain->m_aYAxisDivide.GetData();
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
					/*
					{
						CAxisDivide *pAxisDivide = (CAxisDivide *)m_pRegionMain->m_aXAxisDivide.GetData();
						for (int32 i = 0; i < m_pRegionMain->m_aXAxisDivide.GetSize(); i++)
						{
							CAxisDivide &AxisDivide = pAxisDivide[i];
							pt1.x = pt2.x = AxisDivide.m_iPosPixel;
							pt1.y = RectSubMain.top;
							pt2.y = RectSubMain.bottom;
							
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
					*/

					DrawInterval(pDC, pChartRegion);
				}
				else if (pChartRegion == SubRegion.m_pSubRegionYLeft)
				{
					CRect RectSubMain = SubRegion.m_pSubRegionMain->GetRectCurves();
					CRect RectYLeft = m_pRegionYLeft->GetRectCurves();
					RectYLeft.DeflateRect(3, 3, 3, 3);
					
					// 画左文字
					// xl 0607 副图小于一定高度，不画坐标
					if ( RectSubMain.Height() >= 8 )
					{
						CAxisDivide *pAxisDivide = (CAxisDivide *)SubRegion.m_pSubRegionMain->m_aYAxisDivide.GetData();
						for (int32 i = 0; i < SubRegion.m_pSubRegionMain->m_aYAxisDivide.GetSize(); i++)
						{
							CAxisDivide &AxisDivide = pAxisDivide[i];
							
							CRect rt = RectYLeft;
							if ((AxisDivide.m_DivideText1.m_uiTextAlign & DT_VCENTER) != 0)						
							{
								rt.top	= AxisDivide.m_iPosPixel - iHalfHeight;
								rt.bottom = AxisDivide.m_iPosPixel + iHalfHeight;
							}
							else if ((AxisDivide.m_DivideText1.m_uiTextAlign & DT_BOTTOM) != 0)
							{
								rt.bottom = AxisDivide.m_iPosPixel;
								rt.top	= rt.bottom - iFontHeight;							
							}
							else
							{
								rt.top	= AxisDivide.m_iPosPixel;
								rt.bottom	= rt.top + iFontHeight;
							}
							
							// 
							if ( rt.bottom > RectSubMain.bottom )
							{
								continue;
							}
							
							pDC->MoveTo(RectSubMain.left, AxisDivide.m_iPosPixel);
							pDC->LineTo(rt.right-1, AxisDivide.m_iPosPixel);
							pDC->SetTextColor(AxisDivide.m_DivideText1.m_lTextColor);
							pDC->SetBkMode(TRANSPARENT);
							pDC->DrawText(AxisDivide.m_DivideText1.m_StrText, &rt, AxisDivide.m_DivideText1.m_uiTextAlign);
						}
					}

					if ( bLastPickSub )
					{
						// 选中副图标志
						CRect RectSel(pChartRegion->m_RectView);
						RectSel.InflateRect(1, -1, -1, -1);
						pDC->_DrawRect(RectSel, color);
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

		//绘制指标额外的水平线
		DrawIndexExtraY(pDC, pChartRegion);
	}

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldFont);
	pen.DeleteObject();	
}

///////////////////////////////////////////////////////////////////////////////
void CIoViewKLineArbitrage::OnSliderId ( int32& id, int32 iJump )
{
	SliderId ( id, iJump );
}

CString CIoViewKLineArbitrage::OnGetChartGuid ( CChartRegion* pRegion )
{
	return GetChartGuid(pRegion );
}

void CIoViewKLineArbitrage::InitialShowNodeNums()
{
	// 由于这个位置总是要显示最新k线的，所以不做调整
	m_iNodeCountPerScreen = m_iNodeNumXml + KSpaceRightCount;

	if ( m_iNodeCountPerScreen < KMinKLineNumPerScreen || m_iNodeCountPerScreen > KMaxKLineNumPerScreen)
	{
		m_iNodeCountPerScreen = KDefaultKLineNumPerScreen;
	}	
}

void CIoViewKLineArbitrage::DoFromXml()
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
			// 如果没有副图就不要添加了
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

                CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
                if(pMain)
                {
                    switch(pData->m_eTimeIntervalFull)
                    {
                    case ENTIMinute:
                        {
                            pMain->m_LastTimeSet = 1;
                        }
                        break;
                    case ENTIMinute5:
                        {
                            pMain->m_LastTimeSet = 5;
                        }
                        break;
                    case ENTIMinute15:
                        {
                            pMain->m_LastTimeSet = 15;
                        }
                        break;
                    case ENTIMinute30:
                        {
                            pMain->m_LastTimeSet = 30;
                        }
                        break;
                    case ENTIMinute60:
                        {
                            pMain->m_LastTimeSet = 60;
                        }
                        break;
                    case ENTIMinute180:
                        {
                            pMain->m_LastTimeSet = 180;
                        }
                        break;
                    case ENTIMinute240:
                        {
                            pMain->m_LastTimeSet = 240;
                        }
                        break;

                    case ENTIDay:
                        {
                            pMain->m_LastTimeSet = 1440;
                        }
                        break;
                    case ENTIWeek:
                        {
                            pMain->m_LastTimeSet = 1440*7;
                        }
                        break;

                    case ENTIMonth:
                        {
                            pMain->m_LastTimeSet = 1440*30;
                        }
                        break;

                    case ENTIYear:
                        {
                            pMain->m_LastTimeSet = 1440*365;
                        }
                        break;

                    default:
                        {
                            pMain->m_LastTimeSet = m_uiUserMinutesXml;
                        }
                        break;
                    }
                }
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

IChartBrige CIoViewKLineArbitrage::GetRegionParentIoView()
{
	IChartBrige ChartRegion;
	ChartRegion.pWnd = this;
	ChartRegion.pIoViewBase = this;
	ChartRegion.pChartRegionData = this;
	return ChartRegion;
}

void CIoViewKLineArbitrage::SetChildFrameTitle()
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

void CIoViewKLineArbitrage::GetCrossKLine(OUT CKLine & KLineIn)
{
	 memset((void*)(&KLineIn),0,sizeof(Kline));
	 memcpyex(&KLineIn,&m_KLineCrossNow,sizeof(Kline));
}

void CIoViewKLineArbitrage::OnIoViewActive()
{
	CIoViewChart::OnIoViewActive();
}

void CIoViewKLineArbitrage::OnIoViewDeactive()
{
	CIoViewChart::OnIoViewDeactive();
	m_TipWnd.Hide();
}

void CIoViewKLineArbitrage::CalcLayoutRegions(bool bOnlyUpdateMainRegion)
{
	if (m_MerchParamArray.GetSize() <= 0)
		return;

	// 计算主region的坐标值， 对称坐标
	{
		if (NULL != m_pRegionMain)
		{
			bool32 bValidCurveYMinMax = false;
			float fCurveYMin = 0., fCurveYMax = 0.;
			int32 i = 0;

			// 如果是非普通类型坐标，需要先计算base值, 并设置, 因为上下边界值依赖于base值
			float fBase = CChartCurve::GetInvalidPriceBaseYValue();
			// 主线
			CChartCurve *pDepCurve = m_pRegionMain->GetDependentCurve();
			if ( NULL == pDepCurve )
			{
				return;
			}

			CPriceToAxisYObject::E_AxisYType eType = GetMainCurveAxisYType();
			pDepCurve->SetAxisYType(eType);	// 设置主线坐标类型
			if ( CalcKLinePriceBaseValue(pDepCurve, fBase) )
			{
				ASSERT( eType == CPriceToAxisYObject::EAYT_Normal || fBase != CChartCurve::GetInvalidPriceBaseYValue() );
				// 设置所有的主region中的curve的基础值
				if ( eType == CPriceToAxisYObject::EAYT_Normal || fBase != CChartCurve::GetInvalidPriceBaseYValue() )
				{
					if ( eType == CPriceToAxisYObject::EAYT_Normal && fBase != CChartCurve::GetInvalidPriceBaseYValue() && EACST_TowLegCmp == m_eArbitrageShow )
					{
						eType = CPriceToAxisYObject::EAYT_Pecent;	// 两腿套利时，切换到等价的百分比坐标
					}
					for (i = 0; i < m_pRegionMain->m_Curves.GetSize(); i++)
					{
						CChartCurve *pCurve = m_pRegionMain->m_Curves[i];
						if ( NULL != pCurve )
						{
							// index与maincurve一样的基础值
							// 叠加的需要单独计算
							pCurve->SetAxisYType(eType);	// 类型
							if ( m_pCuveOtherArbLeg == pCurve )
							{
								// 第二腿线
							}
							if ( CheckFlag(pCurve->m_iFlag, CChartCurve::KYTransformToAlignDependent) )
							{
								float fCmpBase;
								if ( CalcKLinePriceBaseValue(pCurve, fCmpBase) )
								{
									pCurve->SetPriceBaseY(fCmpBase, true);
								}
								else
								{
									// 该线无有效数据？？
									pCurve->SetPriceBaseY(CChartCurve::GetInvalidPriceBaseYValue(), true);	// 无效化
								}
							}
							else
							{
								pCurve->SetPriceBaseY(fBase, true);	// 使用主线的
							}
						}
					}
					
					// 计算所有曲线的上下边界值
					for ( i=0; i < m_pRegionMain->m_Curves.GetSize(); i++)
					{
						float fYMin = 0., fYMax = 0.;
						
						CChartCurve *pChartCurve = m_pRegionMain->m_Curves[i];

						if ( NULL == pChartCurve )
						{
							continue;
						}

						if ( pChartCurve->BeNoDraw() )
						{
							continue;
						}

						if ( CheckFlag(pChartCurve->m_iFlag, CChartCurve::KInVisible) )
						{
							continue;
						}

						//
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
				}
			}
			// 计算主线的基础值成功或者失败

			// 设置所有的主region中的curve的上下限值
			for (i = 0; i < m_pRegionMain->m_Curves.GetSize(); i++)
			{				
				CChartCurve *pChartCurve = m_pRegionMain->m_Curves[i];
				pChartCurve->SetYMinMax(fCurveYMin, fCurveYMax, bValidCurveYMinMax);
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

bool32 CIoViewKLineArbitrage::CalcMainRegionXAxis(CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aAxisDivide)
{	
	ASSERT(NULL != m_pRegionMain);
	
	aAxisNodes.SetSize(0);
	aAxisDivide.SetSize(0);
	
	// 
	if (m_MerchParamArray.GetSize() <= 0)
	{
		return true;
	}
	
	T_MerchNodeUserData* pMainData = m_MerchParamArray.GetAt(0);
	if (NULL == pMainData)
		return false;
	
	bool32 bSaveMinute = false;
	E_NodeTimeInterval eTimeInterval = pMainData->m_eTimeIntervalFull;
	if (ENTIMinute == eTimeInterval || ENTIMinute5 == eTimeInterval || ENTIMinute15 == eTimeInterval || ENTIMinute30 == eTimeInterval || ENTIMinute60 == eTimeInterval|| ENTIMinute180 == eTimeInterval|| ENTIMinute240 == eTimeInterval || ENTIMinuteUser == eTimeInterval)
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
	else if (ENTIMinute30 == eTimeInterval || ENTIMinute60 == eTimeInterval|| ENTIMinute180 == eTimeInterval|| ENTIMinute240 == eTimeInterval)
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
	
	// 
	const CNodeData *pNodeDatas = (CNodeData *)pMainData->m_pKLinesShow->m_aNodes.GetData();
	
	if ( NULL == pNodeDatas)
	{
		return false;
	}
	
	// 
	CRect rcClient;
	GetClientRect(rcClient);
	CRect rect = m_pRegionMain->GetRectCurves();
	rect.DeflateRect(2, 1, 1, 1);
	int32 iKLineUnitCount = pMainData->m_pKLinesShow->m_aNodes.GetSize() + GetSpaceRightCount();
	float fPixelWidthPerUnit = (rect.Width() - 2) / (float)iKLineUnitCount;

	// 4.000 然后放大到 5.999 时，会有一个断差 - 先去掉取整 xl 0601
	//if (fPixelWidthPerUnit >= 5.)
	//	fPixelWidthPerUnit = (int)(fPixelWidthPerUnit);
	// 如果要求完整的对齐，则必然要求开始处某些数据会被丢弃显示或者结束处出现多余的空格
	// 选择让右边出现多余空格，以保证画面显示固定数量k线, 但是这样会出现密集的k线右边反而比稀疏的右边宽
	// 修正，由于右侧有多余空格，拿出来比较一下，如果可以放宽，可去ceil，如果不可以，则floor
	if ( fPixelWidthPerUnit >= 3.0 )
	{
		fPixelWidthPerUnit = floorf(fPixelWidthPerUnit);		// 由于计算K线的宽度单位为4，所以这里以3作为判断，少于3的宽度都是1
	}

	// 
	aAxisNodes.SetSize(iKLineUnitCount);
	
	int32 iMaxAxisDivideSize = iKLineUnitCount / 1;
	if (iMaxAxisDivideSize < 20)	iMaxAxisDivideSize = 20;
	aAxisDivide.SetSize(iMaxAxisDivideSize);
	
	//
	CAxisNode *pAxisNode = (CAxisNode *)aAxisNodes.GetData();
	CAxisDivide *pAxisDivide = (CAxisDivide *)aAxisDivide.GetData();
	
	int32 iIndexNode = 0, iIndexDivide = 0;
	CTime TimePrev;		// 以前是循环内的一个临时变量
	for (int32 i = 0; i < iKLineUnitCount; i++)
	{
		CAxisNode &AxisNode = pAxisNode[iIndexNode];
		CAxisDivide &AxisDivide = pAxisDivide[iIndexDivide];
		
		/////////////////////////////////////////////////////////////////////////
		// 节点
		if ( i < pMainData->m_pKLinesShow->m_aNodes.GetSize() )
		{
			// 保存坐标点精确id
			AxisNode.m_iTimeId = pNodeDatas[i].m_iID;
		}
		
		
		// 节点左 
		if (0 == iIndexNode)
		{
			AxisNode.m_iStartPixel = rect.left;		// 总是离左边框空出一个像素
		}
		else
		{
			AxisNode.m_iStartPixel = pAxisNode[iIndexNode - 1].m_iEndPixel + 1;
		}
		
		AxisNode.m_iEndPixel = rect.left + (int32)((iIndexNode + 1) * fPixelWidthPerUnit + 0.5) - 1;
		AxisNode.m_fPixelWidthPerUnit =	fPixelWidthPerUnit;

		//
		if (AxisNode.m_iStartPixel > AxisNode.m_iEndPixel)
			AxisNode.m_iStartPixel = AxisNode.m_iEndPixel;
		
		// 中节点
		AxisNode.m_iCenterPixel = (AxisNode.m_iStartPixel + AxisNode.m_iEndPixel) / 2;
		iIndexNode++;
		
		//////////////////////////////////////////////////////////
		// 分割线
		if (i < pMainData->m_pKLinesShow->m_aNodes.GetSize())
		{			
			CTime Time(pNodeDatas[i].m_iID);
			if (0 == i || 
				(eDivideUnitType != EDUTNone && TimePrev.GetYear() != Time.GetYear()))	// 第一条或换年，一定要显示
			{
				AxisDivide.m_iPosPixel		= AxisNode.m_iCenterPixel;
				
				//
				AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;
				AxisDivide.m_eLineStyle = CAxisDivide::ELSDot;
				
				//				
				AxisDivide.m_DivideText1.m_StrText.Format(L"%04d/%02d/%02d(%s)", Time.GetYear(), Time.GetMonth(), Time.GetDay(), GetWeekString(Time).GetBuffer());
				AxisDivide.m_eLevelText		= CAxisDivide::EDLLevel1;
				AxisDivide.m_DivideText1.m_uiTextAlign = DT_LEFT | DT_VCENTER | DT_SINGLELINE;
				AxisDivide.m_DivideText1.m_lTextColor = m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
				
				AxisDivide.m_DivideText2 = AxisDivide.m_DivideText1;
				
				// 
				iIndexDivide++;
			}
			else // 以刻度单位显示坐标	
			{
				AxisDivide.m_iPosPixel		= AxisNode.m_iCenterPixel;
				
				//
				AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel2;
				AxisDivide.m_eLineStyle = CAxisDivide::ELSDot;
				
				//
				AxisDivide.m_eLevelText		= CAxisDivide::EDLLevel2;
				
				AxisDivide.m_DivideText1.m_uiTextAlign = DT_LEFT | DT_VCENTER | DT_SINGLELINE;
				AxisDivide.m_DivideText1.m_lTextColor = m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
				
				if (EDUTByHour == eDivideUnitType)
				{					
					if (Time.GetHour() != TimePrev.GetHour())
					{
						AxisDivide.m_DivideText1.m_StrText.Format(L"%02d:%02d", Time.GetHour(), Time.GetMinute());
						iIndexDivide++;
					}
				}
				else if (EDUTByDay == eDivideUnitType)
				{					
					if (Time.GetDay() != TimePrev.GetDay())
					{
						// ...fangz0811
						// AxisDivide.m_DivideText1.m_StrText.Format(L"%02d/%02d", Time.GetMonth(), Time.GetDay());
						AxisDivide.m_DivideText1.m_StrText.Format(L"%02d", Time.GetDay());
						iIndexDivide++;
					}
				}
				else if (EDUTByMonth ==eDivideUnitType)
				{
					if (Time.GetMonth() != TimePrev.GetMonth())
					{
						// ...fangz0811
						// AxisDivide.m_DivideText1.m_StrText.Format(L"%02d/%02d", Time.GetMonth(), Time.GetDay());
						AxisDivide.m_DivideText1.m_StrText.Format(L"%02d", Time.GetMonth());
						iIndexDivide++;
					}
				}
				else if (EDUTByYear ==eDivideUnitType)
				{
					//NULL; 
				}
				
				AxisDivide.m_DivideText2 = AxisDivide.m_DivideText1;				
			}
			
			// 
			TimePrev = Time;
		}
	}
	
	// 
	aAxisNodes.SetSize(iIndexNode);
	aAxisDivide.SetSize(iIndexDivide);
	
	return true;
}

void CIoViewKLineArbitrage::CalcMainRegionYAxis(CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide)
{
	ASSERT(NULL != m_pRegionMain);
	aYAxisDivide.SetSize(0);

	// 获取纵坐标上几个主要的值
	CChartCurve *pCurveDependent = m_pRegionMain->GetDependentCurve();
	if (NULL == pCurveDependent)
		return;
	
	float fYMin = 0., fYMax = 0.;
	if (!pCurveDependent->GetYMinMax(fYMin, fYMax))
		return;

	// 获取当前显示小数点位
	int32 iSaveDec = m_Arbitrage.m_iSaveDec;
	T_MerchNodeUserData* pData = GetMainData();
	if ( NULL == pData )
	{
		return;
	}

	// 计算Y方向可以容纳多少个刻度
	CRect rect = m_pRegionMain->GetRectCurves();
	int32 iHeightText = pDC->GetTextExtent(_T("测试值")).cy;
	int32 iHeightSub = iHeightText + 12;

	CPriceToAxisYObject::E_AxisYType eAxisType = GetMainCurveAxisYType();

	if ( CPriceToAxisYObject::EAYT_Pecent == eAxisType )
	{
		int32 iNum = rect.Height()/2 / iHeightSub + 1;
		float fPecPerDivide = (fYMax - fYMin)/iNum; // 每个刻度可以代表的百分比, 向上取整到1%
		int32 iPecPer = (int32)(fPecPerDivide*100+0.5);
		iPecPer = max(1, iPecPer);	// 至少为1%
		fPecPerDivide = iPecPer/100.0f;

		aYAxisDivide.SetSize(0, iNum);
		
		// 从基点0%开始
		// 向上的百分比, 数量应该不多
		float fStart;
		if ( !pCurveDependent->GetAxisBaseY(fStart) )
		{
			return;
		}
		float fBase = fStart;
		for ( ; fStart <= fYMax ; fStart += fPecPerDivide )
		{
			int32 iY;
			if ( !pCurveDependent->AxisYToRegionY(fStart, iY) )
			{
				continue;
			}
			m_pRegionMain->RegionYToClient(iY);

			CAxisDivide AxisDivide;
			CString StrValue;
			StrValue.Format(_T("%0.0f%%"), (fStart-fBase)*100); //x%
			AxisDivide.m_iPosPixel = iY;
			
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
			AxisDivide.m_DivideText1.m_lTextColor	= m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
			AxisDivide.m_DivideText2.m_lTextColor	= m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);

			aYAxisDivide.Add(AxisDivide);
		}

		// 向下
		fStart = fBase - fPecPerDivide;
		for ( ; fStart >= fYMin ; fStart -= fPecPerDivide )
		{
			int32 iY;
			if ( !pCurveDependent->AxisYToRegionY(fStart, iY) )
			{
				continue;
			}
			m_pRegionMain->RegionYToClient(iY);
			
			CAxisDivide AxisDivide;
			CString StrValue;
			StrValue.Format(_T("%0.0f%%"), (fStart-fBase)*100); //x%
			AxisDivide.m_iPosPixel = iY;
			
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
			AxisDivide.m_DivideText1.m_lTextColor	= m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
			AxisDivide.m_DivideText2.m_lTextColor	= m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
			
			aYAxisDivide.Add(AxisDivide);
		}
	}
	else if ( CPriceToAxisYObject::EAYT_Log == eAxisType )
	{
		// 对数，加一次10%，就是axis里面乘了1个1.1(0.9)
		// 最大最小值为1.1或者0.9的倍数 
		int32 iNum = (int32)(rect.Height()/2.5f / iHeightSub) + 1;
		float fPecPerDivide = (fYMax - fYMin)/iNum; // 每个刻度可以代表的倍数，取整
		int32 iPecPer = (int32)(fPecPerDivide+0.5);
		iPecPer = max(1, iPecPer);	// 至少为1倍即增或减10%
		fPecPerDivide = iPecPer;
		
		aYAxisDivide.SetSize(0, iNum);
		// 从基点0%开始
		// 向上的百分比, 数量应该不多
		float fStart;
		if ( !pCurveDependent->GetAxisBaseY(fStart) )
		{
			return;
		}
		const float fBase = fStart;
		for ( ; fStart <= fYMax ; fStart += fPecPerDivide )
		{
			int32 iY;
			if ( !pCurveDependent->AxisYToRegionY(fStart, iY) )
			{
				continue;
			}
			m_pRegionMain->RegionYToClient(iY);

			float fPrice;
			pCurveDependent->AxisYToPriceY(fStart, fPrice);
			
			CAxisDivide AxisDivide;
			CString StrValue;
			StrValue.Format(_T("%0.0f%%\r\n"), fStart*10); //1 = 10%
			CString StrPrice;
			Float2StringLimitMaxLength(StrPrice, fPrice, iSaveDec, GetYLeftShowCharWidth() -1, true, true, false, false);
			DWORD dwAlign = 0;
			if ( fStart == fBase )
			{
				StrValue = StrPrice;		// 0%处，仅价格
				dwAlign |= DT_SINGLELINE;
			}
			else
			{
				StrValue += StrPrice;		// 两行 10%\r\nxx.xx
			}

			AxisDivide.m_iPosPixel = iY;
			
			// 格线
			AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;		// 
			AxisDivide.m_eLineStyle = CAxisDivide::ELSDot;			// 实线
			
			// 文字
			AxisDivide.m_eLevelText						= CAxisDivide::EDLLevel1;
			
			AxisDivide.m_DivideText1.m_StrText			= StrValue;
			AxisDivide.m_DivideText2.m_StrText			= StrValue;
			
			AxisDivide.m_DivideText1.m_uiTextAlign		= DT_VCENTER | DT_RIGHT |dwAlign;
			AxisDivide.m_DivideText2.m_uiTextAlign		= DT_VCENTER | DT_LEFT  |dwAlign;
			
			// zhangbo 20090927 #暂时写死颜色
			AxisDivide.m_DivideText1.m_lTextColor	= m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
			AxisDivide.m_DivideText2.m_lTextColor	= m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
			
			aYAxisDivide.Add(AxisDivide);
		}
		
		// 向下
		fStart = fBase - fPecPerDivide;
		for ( ; fStart >= fYMin ; fStart -= fPecPerDivide )
		{
			int32 iY;
			if ( !pCurveDependent->AxisYToRegionY(fStart, iY) )
			{
				continue;
			}
			m_pRegionMain->RegionYToClient(iY);
			
			float fPrice;
			pCurveDependent->AxisYToPriceY(fStart, fPrice);
			
			CAxisDivide AxisDivide;
			CString StrValue;
			StrValue.Format(_T("%0.0f%%\r\n"), fStart*10); //x%
			CString StrPrice;
			Float2StringLimitMaxLength(StrPrice, fPrice, iSaveDec, GetYLeftShowCharWidth() -1, true, true, false, false);
			StrValue += StrPrice;		// 两行 10%\r\nxx.xx

			AxisDivide.m_iPosPixel = iY;
			
			// 格线
			AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;		// 
			AxisDivide.m_eLineStyle = CAxisDivide::ELSDot;			// 实线
			
			// 文字
			AxisDivide.m_eLevelText						= CAxisDivide::EDLLevel1;
			
			AxisDivide.m_DivideText1.m_StrText			= StrValue;
			AxisDivide.m_DivideText2.m_StrText			= StrValue;
			
			AxisDivide.m_DivideText1.m_uiTextAlign		= DT_VCENTER | DT_RIGHT;
			AxisDivide.m_DivideText2.m_uiTextAlign		= DT_VCENTER | DT_LEFT;
			
			// zhangbo 20090927 #暂时写死颜色
			AxisDivide.m_DivideText1.m_lTextColor	= m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
			AxisDivide.m_DivideText2.m_lTextColor	= m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
			
			aYAxisDivide.Add(AxisDivide);
		}
	}
	else 
	{
		//ASSERT( CPriceToAxisYObject::EAYT_Normal == pCurveDependent->GetAxisYType() );
		// 普通坐标轴点计算
		int32 iNum = rect.Height() / iHeightSub + 1;
		if (iNum > 3)	iNum = 3;
		if (iNum < 2)	iNum = 2;
			
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
				
				int32 i = 0;
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
						break;
					}
					
					m_pRegionMain->RegionYToClient(y);

					float fPrice = 0.0f;
					pCurveDependent->AxisYToPriceY(float(aiYAxis[i]), fPrice);
					
					CString StrValue	= Float2String(fPrice, iSaveDec, false, false, false);
					
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
					AxisDivide.m_DivideText1.m_lTextColor	= m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
					AxisDivide.m_DivideText2.m_lTextColor	= m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
					
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
					break;
				}
				m_pRegionMain->RegionYToClient(y);

				float fPrice = 0.0f;
				pCurveDependent->AxisYToPriceY(fStart, fPrice);
				
				// xl 0001762 限制主Y轴字串长度 - 右侧留一个空格
				//CString StrValue	= Float2String(fStart, iSaveDec, true, false, false);
				CString StrValue;
				Float2StringLimitMaxLength(StrValue, fPrice, iSaveDec, GetYLeftShowCharWidth() -1, true, false, false, false);
				
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
				AxisDivide.m_DivideText1.m_lTextColor	= m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
				AxisDivide.m_DivideText2.m_lTextColor	= m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
				
				// 
				iCount++;
			}
		}
		// 
		aYAxisDivide.SetSize(iCount);
	}

	CPriceToAxisYObject calcObj;
	pCurveDependent->GetAxisYCalcObject(calcObj);
}

void CIoViewKLineArbitrage::CalcSubRegionYAxis(CChartRegion* pChartRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide)
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
			AxisDivide.m_DivideText1.m_lTextColor	= m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
			AxisDivide.m_DivideText2.m_lTextColor	= m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);

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
			
			int32 i = 0;
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
				AxisDivide.m_DivideText1.m_lTextColor	= m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
				AxisDivide.m_DivideText2.m_lTextColor	= m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
				
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
			AxisDivide.m_DivideText1.m_lTextColor	= m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
			AxisDivide.m_DivideText2.m_lTextColor	= m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
			
			// 
			iCount++;
		}
	}

	// 
	aYAxisDivide.SetSize(iCount);
}

void CIoViewKLineArbitrage::OnKeyF8()
{
	//
	T_MerchNodeUserData *pData = GetMainData();
	if ( NULL != pData )
	{
		switch (pData->m_eTimeIntervalFull)
		{
		case ENTIMinute:		// 1分钟 -> 5分钟
			{
				SetTimeInterval(*pData, ENTIMinute5);
			}
			break;
		case ENTIMinute5:
			{
				SetTimeInterval(*pData, ENTIMinute15);
			}			
			break;
		case ENTIMinute15:
			{
				SetTimeInterval(*pData, ENTIMinute30);
				break;
			}
		case ENTIMinute30:
			{
				SetTimeInterval(*pData, ENTIMinute60);
			}
			break;
        case ENTIMinute60:			
            {
                SetTimeInterval(*pData, ENTIMinute180);
            }
            break;
        case ENTIMinute180:			
            {
                SetTimeInterval(*pData, ENTIMinute240);
            }
            break;
        case ENTIMinute240:			
            {
                SetTimeInterval(*pData, ENTIDay);
            }
            break;
        case ENTIDay:				// 日线
            {
                SetTimeInterval(*pData, ENTIMinuteUser);
            }
            break;
        case ENTIMinuteUser:		// 自定义周期
            {
                SetTimeInterval(*pData, ENTIMinute);
            }
            break;
		case ENTIWeek:
			{
				SetTimeInterval(*pData, ENTIMonth);
			}
			break;
		case ENTIMonth:
			{
				SetTimeInterval(*pData, ENTIQuarter);
			}
			break;
		case ENTIQuarter:
			{
				SetTimeInterval(*pData, ENTIYear);
			}
			break;
		case ENTIYear:
			{
				SetTimeInterval(*pData, ENTIDayUser);
			}
			break;
		case ENTIDayUser:			// 自定义周期
			{
				SetTimeInterval(*pData, ENTIMinute);
			}
			break;	
		default:
			{
				//ASSERT(0);
			}
			break;
		}
		SetCurveTitle(pData);
	}

	// 请求数据
	RequestViewData();

	// 
	UpdateSelfDrawCurve();
	m_pRegionMain->SetDrawFlag(CRegion::KDrawTransform| CRegion::KDrawNotify);
	RedrawWindow();
}

void CIoViewKLineArbitrage::OnKeyDown()
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

void CIoViewKLineArbitrage::OnKeyUp()
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

void CIoViewKLineArbitrage::OnKeyHome()
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

void CIoViewKLineArbitrage::OnKeyEnd()
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

void CIoViewKLineArbitrage::OnKeyLeftAndCtrl()
{
	OnKeyLeftAndCtrl(1);
}

void CIoViewKLineArbitrage::OnKeyLeftAndCtrl( int32 iRepCnt )
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

	bool32 bIsShowNewestOld = false;
	CGmtTime TimeStart, TimeEnd;
	bIsShowNewestOld = IsShowNewestKLine(TimeStart, TimeEnd);
	
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
		
		if (!ChangeMainDataShowData(*pMainData, iShowPosInFullList, iShowCountInFullList, bIsShowNewestOld, TimeStart, TimeEnd))
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

void CIoViewKLineArbitrage::OnKeyLeft()
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
				   
void CIoViewKLineArbitrage::OnKeyRightAndCtrl()
{
	// 取主图信息
	OnKeyRightAndCtrl(1);
}

void CIoViewKLineArbitrage::OnKeyRightAndCtrl( int32 iRepCnt )
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

	bool32 bIsShowNewestOld = false;
	CGmtTime TimeStart, TimeEnd;
	bIsShowNewestOld = IsShowNewestKLine(TimeStart, TimeEnd);
	
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

		if (!ChangeMainDataShowData(*pMainData, iShowPosInFullList, iShowCountInFullList, bIsShowNewestOld, TimeStart, TimeEnd))
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

void CIoViewKLineArbitrage::OnKeyRight()
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

void CIoViewKLineArbitrage::OnKeySpace()
{
	CIoViewChart::OnKeySpace();
}

void CIoViewKLineArbitrage::OnRectZoomOut(int32 iNodeBegin, int32 iNodeEnd)
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

	if ( (iNodeEnd - iNodeBegin + 1) <= KMinKLineNumPerScreen )
	{
		return;
	}

	// 框选放大:
	int32 iNodeCounts		= iNodeEnd - iNodeBegin + 1;
	m_iNodeCountPerScreen	= iNodeCounts + GetSpaceRightCount();

	CGmtTime TimeStart,TimeEnd;
	bool32 bIsShowNewestOld = IsShowNewestKLine(TimeStart, TimeEnd);
	if (!ChangeMainDataShowData(*pMainData, iNodeBegin, iNodeCounts, bIsShowNewestOld, TimeStart, TimeEnd))
	{
		return;
	}

	// 显示刷新
 	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
 	
 	for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
 	{
 		m_SubRegions[i].m_pSubRegionMain->SetDrawFlag(CRegion::KDrawFull);
 	}
 	
 	// 重画视图
 	ReDrawAysnc();
}

bool32 CIoViewKLineArbitrage::OnZoomOut()
{
	// 缩小显示单位， 显示多一些K线
	if ( m_MerchParamArray.GetSize() <= 0 )
		return false;

	T_MerchNodeUserData *pMainData = m_MerchParamArray[0];
	if (NULL == pMainData)
		return false;

	// 当前总共的K 线条数还没有理论上应该有的多, 说明数据不够. 不要再缩小了.
	if (  m_iNodeCountPerScreen >= KMaxKLineNumPerScreen )
	{
		return false;
	}

	// 当前所有商品的原始K线数量小于要求的数据，则不必再请求了
	{
		E_NodeTimeInterval eNodeTimeIntervalCompare = pMainData->m_eTimeIntervalFull;
		E_KLineTypeBase	   eKLineTypeCompare		= EKTB5Min;
		int32 iScale = 1;
		if (!GetTimeIntervalInfo(pMainData->m_eTimeIntervalFull, eNodeTimeIntervalCompare, eKLineTypeCompare, iScale))
		{
			// 不应该出现这种情况
			return false;
		}

		MerchArray aMerchs;
		GetArbitrageMerchs(m_Arbitrage, aMerchs);
		int32 iNeedCount = (GetSubMerchNeedDataCount() - GetSpaceRightCount()) * iScale;
		for ( int32 i=0; i < aMerchs.GetSize() ; ++i )
		{
			T_MerchNodeUserData *pData = GetMerchData(aMerchs[i]);
			ASSERT( NULL != pData );
			if ( NULL != pData )
			{
				if ( pData->m_aKLinesCompare.GetSize() < iNeedCount-100 )
				{
					TRACE(_T("套利数据在等待数据返回，忽略放大: %s %d,%d->%d\r\n"), pData->m_pMerchNode->m_MerchInfo.m_StrMerchCnName.GetBuffer(), m_iNodeCountPerScreen, iNeedCount, pData->m_aKLinesCompare.GetSize());
					return false;	// 实际数据还没有回来了，不处理这次放大
				}
			}
		}
	}
	
	
	int32 iCurShowCountPerScreen = m_iNodeCountPerScreen;
	int32 iBakCurShowCountPerScreen = m_iNodeCountPerScreen;

	iCurShowCountPerScreen = (int32)(iCurShowCountPerScreen / KHZoomFactor + 0.5);

	// 一次最多缩 500 根
  	if ( iCurShowCountPerScreen - iBakCurShowCountPerScreen > 500 )
  	{
  		iCurShowCountPerScreen = iBakCurShowCountPerScreen + 500;
  	}
	
	//
	if (iCurShowCountPerScreen > KMaxKLineNumPerScreen)
		iCurShowCountPerScreen = KMaxKLineNumPerScreen;

	if (iBakCurShowCountPerScreen == iCurShowCountPerScreen)	// 没有变化， 不需要处理
		return false;

	// 锁柱十字光标
	LockCrossInfo(true);

	// 
	m_iNodeCountPerScreen = iCurShowCountPerScreen;

	// 判断当前显示， 是否显示最新价
	CGmtTime TimeStartInFullList, TimeEndInFullList;
	bool32 bShowNewestKLine		= IsShowNewestKLine(TimeStartInFullList, TimeEndInFullList);
	int32 iShowPosInFullList	= pMainData->m_iShowPosInFullList;
	int32 iShowCountInFullList	= pMainData->m_iShowCountInFullList;
	bool32 bIsShowNewestOld		= bShowNewestKLine;

	int32 iSpaceRightCount		= GetSpaceRightCount();
	
	bool32 bActiveCross = false;
	if ( NULL != m_pRegionMain && m_pRegionMain->m_bActiveCross )
	{
		bActiveCross = true;
	}

	//
	CKLine KLineNow;
	if ( bActiveCross )
	{
		if ( 0 == m_iNodeCountPerScreen % 2 )
		{
			// 保证奇数
			m_iNodeCountPerScreen += 1;
		}

		// 以当前十字光标所标识的节点为中心放大:		
		GetCrossKLine(KLineNow);
		
		// K 线个数
		int32 iSizeKLine = pMainData->m_aKLinesFull.GetSize();
		
		// 找到这个K 线所在的位置
		int32 iKLinePos = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pMainData->m_aKLinesFull, KLineNow.m_TimeCurrent);

		// 调整显示区间
DO_CALC_SHOW_RANGE_ACTIVE_CROSS:
		{
			int32 iMaxShowCount = m_iNodeCountPerScreen - iSpaceRightCount;

			if (bShowNewestKLine)	// 总是显示最新价
			{				
				if (iMaxShowCount <= 0)
				{
					//ASSERT(0);
					iMaxShowCount = 1;
				}
				
				if (pMainData->m_aKLinesFull.GetSize() <= 0)
					return true;				// 没有数据要显示
				else
				{
					if (pMainData->m_aKLinesFull.GetSize() <= iMaxShowCount)
					{
						iShowPosInFullList = 0;
						iShowCountInFullList = pMainData->m_aKLinesFull.GetSize();
					}
					else
					{
						iShowCountInFullList = iMaxShowCount;
						iShowPosInFullList = pMainData->m_aKLinesFull.GetSize() - iMaxShowCount;
					}
				}
			}
			else
			{
				// 新的起始和终止位置
				int32 iStartPos = iKLinePos - (m_iNodeCountPerScreen / 2);
				int32 iEndPos	= iKLinePos + (m_iNodeCountPerScreen / 2);
				
				//
				iShowPosInFullList	 = iStartPos;
				iShowCountInFullList = m_iNodeCountPerScreen - iSpaceRightCount;

				//
				if ( iStartPos <= 0 )
				{
					iShowPosInFullList = 0;

					if ( pMainData->m_aKLinesFull.GetSize() <= iMaxShowCount )
					{						
						iShowCountInFullList = pMainData->m_aKLinesFull.GetSize();
					}
				}
				else if ( iEndPos >= iSizeKLine )
				{
					// 
					bShowNewestKLine = true;
					goto DO_CALC_SHOW_RANGE_ACTIVE_CROSS;
				}				
			}
		}
	}
	else
	{
		// 调整显示区间
	DO_CALC_SHOW_RANGE:
		{
			if (bShowNewestKLine)	// 总是显示最新价
			{
				int32 iMaxShowCount = m_iNodeCountPerScreen - iSpaceRightCount;
				if (iMaxShowCount <= 0)
				{
					//ASSERT(0);
					iMaxShowCount = 1;
				}
				
				if (pMainData->m_aKLinesFull.GetSize() <= 0)
					return true;				// 没有数据要显示
				else
				{
					if (pMainData->m_aKLinesFull.GetSize() <= iMaxShowCount)
					{
						iShowPosInFullList = 0;
						iShowCountInFullList = pMainData->m_aKLinesFull.GetSize();
					}
					else
					{
						iShowCountInFullList = iMaxShowCount;
						iShowPosInFullList = pMainData->m_aKLinesFull.GetSize() - iMaxShowCount;
					}
				}
			}
			else
			{
				int32 iShowPosStart = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pMainData->m_aKLinesFull, TimeStartInFullList);
				if (iShowPosStart >= 0 && iShowPosStart < pMainData->m_aKLinesFull.GetSize())
				{
					int32 iMaxShowCountInFullList = pMainData->m_aKLinesFull.GetSize() - iShowPosStart;
					if (iMaxShowCountInFullList > m_iNodeCountPerScreen - iSpaceRightCount)	// 足够显示
					{
						iShowPosInFullList = iShowPosStart;
						iShowCountInFullList = m_iNodeCountPerScreen - iSpaceRightCount;
					}
					else
					{
						bShowNewestKLine = true;
						goto DO_CALC_SHOW_RANGE;
					}
				}
				else
				{
					bShowNewestKLine = true;
					goto DO_CALC_SHOW_RANGE;
				}
			}
		}
	}
	
	// 
	if (!ChangeMainDataShowData(*pMainData, iShowPosInFullList, iShowCountInFullList, bIsShowNewestOld, TimeStartInFullList, TimeEndInFullList))
	{
		return false;
	}
	
	// 显示刷新
	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	for ( int32 i = 0; i < m_SubRegions.GetSize(); i++)
	{
		m_SubRegions[i].m_pSubRegionMain->SetDrawFlag(CRegion::KDrawFull);
	}

	// 看是否有必要请求更多数据
	RequestViewData();

	//
	UpdateMainMerchKLine(*pMainData);

	RedrawWindow();

	return true;
}

bool32 CIoViewKLineArbitrage::OnZoomIn()
{
	// 显示单位放大， 显示少一些K线
	if (m_MerchParamArray.GetSize() <= 0)
		return false;
	
	T_MerchNodeUserData *pMainData = m_MerchParamArray[0];
	if (NULL == pMainData)
		return false;

	// 不要用 m_iNodeCountPerScreen 这个值, 可能缩小缩的太离谱了, 用实际的当前显示的K 线数值
	int32 iCurShowCountPerScreen = pMainData->m_pKLinesShow->GetSize();
	int32 iBakCurShowCountPerScreen = iCurShowCountPerScreen;
	
	iCurShowCountPerScreen = (int32)(iCurShowCountPerScreen * KHZoomFactor + 0.5);
	if (iCurShowCountPerScreen < KMinKLineNumPerScreen)
		iCurShowCountPerScreen = KMinKLineNumPerScreen;
	
	if (iBakCurShowCountPerScreen == iCurShowCountPerScreen)	// 没有变化， 不需要处理
		return false;
	
	// 锁柱十字光标
	LockCrossInfo(true);

	// zhangbo 20090715 #待优化， 避免无限制的放缩
	m_iNodeCountPerScreen = iCurShowCountPerScreen;
	
	// 判断当前显示， 是否显示最新价
	CGmtTime TimeStartInFullList, TimeEndInFullList;
	bool32 bShowNewestKLine		= IsShowNewestKLine(TimeStartInFullList, TimeEndInFullList);
	int32 iShowPosInFullList	= pMainData->m_iShowPosInFullList;
	int32 iShowCountInFullList	= pMainData->m_iShowCountInFullList;
	bool32 bIsShowNewestOld     = bShowNewestKLine;

	int32 iSpaceRightCount = GetSpaceRightCount();

	bool32 bActiveCross = false;
	if ( NULL != m_pRegionMain && m_pRegionMain->m_bActiveCross )
	{
		bActiveCross = true;
	}

	CKLine KLineNow;
	if ( bActiveCross )
	{
		if ( 0 == m_iNodeCountPerScreen % 2 )
		{
			// 保证奇数
			m_iNodeCountPerScreen += 1;
		}

		// 以当前十字光标所标识的节点为中心放大:
		GetCrossKLine(KLineNow);

		// K 线个数
		int32 iSizeKLine = pMainData->m_aKLinesFull.GetSize();
		
		// 找到这个K 线所在的位置
		int32 iKLinePos = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pMainData->m_aKLinesFull, KLineNow.m_TimeCurrent);

		// 新的起始和终止位置
		int32 iStartPos = iKLinePos - (m_iNodeCountPerScreen / 2);
		int32 iEndPos	= iKLinePos + (m_iNodeCountPerScreen / 2);

		//
		if ( iStartPos < 0 )
		{
			// 这时候不应该出现此情况
			ASSERT(iEndPos >=0 && iEndPos < iSizeKLine);

			// 
			iStartPos = 0;
			iEndPos   = iStartPos + m_iNodeCountPerScreen - 1;
		}

		if ( iEndPos >= iSizeKLine )
		{
			ASSERT(iStartPos >= 0 && iStartPos < iSizeKLine);

			//
			iEndPos	  = iSizeKLine - 1;
			iStartPos = iEndPos - m_iNodeCountPerScreen + 1;
		}

		//if ( iStartPos * iEndPos < 0 ||iStartPos >= iSizeKLine || iEndPos >= iSizeKLine )
		//{
		//	ASSERT(0);
		//}

		//
		iShowPosInFullList	 = iStartPos;
		iShowCountInFullList = m_iNodeCountPerScreen;

		//
		// TRACE(L"当前K线总数: %d 屏幕中K线数: %d 居中的K 线是:%d[%d] 显示的第一个是:%d 显示的最后一个是:%d\n", iSizeKLine, m_iNodeCountPerScreen, iKLinePos, KLineNow.m_TimeCurrent, iStartPos, iEndPos);	
	}
	else
	{
		// 调整显示区间
DO_CALC_SHOW_RANGE:
		{
			if (bShowNewestKLine)	// 总是显示最新价
			{
				int32 iMaxShowCount = m_iNodeCountPerScreen - iSpaceRightCount;
				if (iMaxShowCount <= 0)
				{
					//ASSERT(0);
					iMaxShowCount = 1;
				}
				
				if (pMainData->m_aKLinesFull.GetSize() <= 0)
					return true;				// 没有数据要显示
				else
				{
					if (pMainData->m_aKLinesFull.GetSize() <= iMaxShowCount)
					{
						iShowPosInFullList = 0;
						iShowCountInFullList = pMainData->m_aKLinesFull.GetSize();
					}
					else
					{
						iShowCountInFullList = iMaxShowCount;
						iShowPosInFullList = pMainData->m_aKLinesFull.GetSize() - iMaxShowCount;
					}
				}
			}
			else
			{
				int32 iShowPosStart = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pMainData->m_aKLinesFull, TimeStartInFullList);
				if (iShowPosStart >= 0 && iShowPosStart < pMainData->m_aKLinesFull.GetSize())
				{
					int32 iMaxShowCountInFullList = pMainData->m_aKLinesFull.GetSize() - iShowPosStart;
					if (iMaxShowCountInFullList > m_iNodeCountPerScreen - iSpaceRightCount)	// 足够显示
					{
						iShowPosInFullList = iShowPosStart;
						iShowCountInFullList = m_iNodeCountPerScreen - iSpaceRightCount;
					}
					else
					{
						bShowNewestKLine = true;
						goto DO_CALC_SHOW_RANGE;
					}
				}
				else
				{
					bShowNewestKLine = true;
					goto DO_CALC_SHOW_RANGE;
				}
			}
		}
	}

	// 
	if (!ChangeMainDataShowData(*pMainData, iShowPosInFullList, iShowCountInFullList, bIsShowNewestOld, TimeStartInFullList, TimeEndInFullList))
	{
		return false;
	}
	
	// 显示刷新
	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
	{
		m_SubRegions[i].m_pSubRegionMain->SetDrawFlag(CRegion::KDrawFull);
	}
	
	//
	UpdateMainMerchKLine(*pMainData);

	//
	RedrawWindow();

	return true;
}
 
void CIoViewKLineArbitrage::RequestViewData(bool32 bForceReq/*=false*/)
{
	//
	T_MerchNodeUserData *pMainData = m_MerchParamArray.GetSize() > 0? m_MerchParamArray[0] : NULL;
	if (NULL == pMainData)
		return;

	int32 iSpaceRightCount = GetSpaceRightCount();
	// 获取这个商品的周期等信息
	E_NodeTimeInterval eNodeTimeIntervalCompare = pMainData->m_eTimeIntervalFull;
	E_KLineTypeBase	   eKLineTypeCompare		= EKTB5Min;
	int32 iScale = 1;
	if (!GetTimeIntervalInfo(pMainData->m_eTimeIntervalFull, eNodeTimeIntervalCompare, eKLineTypeCompare, iScale))
	{
		// 不应该出现这种情况
		return;
	}

	// 不考虑main商品数据的请求，只请求实际的商品数据
	// 由于是合成的数据，并不清楚需要满足当前要显示数量的点需要多少数据，只能猜了
	for (int32 i = 1; i < m_MerchParamArray.GetSize(); i++)
	{
		//计算是否应当申请数据 - 数据量比较单位使用基本的 K线 周期单位
		T_MerchNodeUserData *pMerchData = m_MerchParamArray[i];
		if (NULL == pMerchData || NULL == pMerchData->m_pMerchNode)
			continue;

		CGmtTime TimeSpecify		= m_pAbsCenterManager->GetServerTime();
		int32 iNeedKLineCountForSane = KMinRequestKLineNumber * iScale;			// 计算金盾指标所需要的最小的NodeCount
		
		// 由于没有办法预知需要请求多少数据，所以只有盲目的*2了
		int32 iTmpCount = (GetSubMerchNeedDataCount() - iSpaceRightCount) * iScale;
		int32 iNeedKLineCount = MAX(iNeedKLineCountForSane, iTmpCount);   // 选取能满足两者需要的最基本KLine数据

		if ( pMerchData->m_pMerchNode!=m_Arbitrage.m_MerchA.m_pMerch
			&& pMerchData->m_pMerchNode!=m_Arbitrage.m_MerchB.m_pMerch
			&& pMerchData->m_pMerchNode!=m_Arbitrage.m_MerchC.m_pMerch )
		{
			// 插入的计算用商品额外取一点数据(估算的)
			iNeedKLineCount += GetSubMerchNeedDataCount()/2;
		}
		
		//
		{
			// 请求吧
			//TRACE(_T("-------------------------------------------请求[%d]\r\n"), iNeedKLineCount);
			if (iNeedKLineCount < KMinRequestKLineNumber)
			{
				iNeedKLineCount = KMinRequestKLineNumber;
			}
			iNeedKLineCount = min(iNeedKLineCount, KMaxMinuteKLineCount);

			// 数据量大的话，假设数据总是连续的
			// 只请求最后一段数据后的若干段
			if ( !bForceReq )
			{
				int32 iSize = pMerchData->m_aKLinesCompare.GetSize();
				if ( iSize > 0 )
				{
					TimeSpecify = pMerchData->m_aKLinesCompare[0].m_TimeCurrent;
					iNeedKLineCount -= iSize-1;
				}
			}
			
			//
			if ( iNeedKLineCount > 0 )
			{
				CMmiReqMerchKLine info; 
				info.m_eKLineTypeBase	= eKLineTypeCompare;								// 使用原始K线周期
				info.m_iMarketId		= pMerchData->m_pMerchNode->m_MerchInfo.m_iMarketId;
				info.m_StrMerchCode		= pMerchData->m_pMerchNode->m_MerchInfo.m_StrMerchCode;
				
				info.m_eReqTimeType		= ERTYFrontCount;
				info.m_TimeSpecify		= TimeSpecify;
				info.m_iFrontCount		= iNeedKLineCount;
			
				DoRequestViewData(info);
			}
		}
		
		// 请求实时数据
		CMmiReqRealtimePrice MmiReqRealtimePrice;
		MmiReqRealtimePrice.m_iMarketId		= pMerchData->m_pMerchNode->m_MerchInfo.m_iMarketId;
		MmiReqRealtimePrice.m_StrMerchCode	= pMerchData->m_pMerchNode->m_MerchInfo.m_StrMerchCode;		
		DoRequestViewData(MmiReqRealtimePrice);

	}

	// 需要显示该信息，so必请求
	//if ( EWTNone != m_eWeightType )
	{
		RequestWeightData();
	}
}

void CIoViewKLineArbitrage::RequestWeightData()
{
	
}

void CIoViewKLineArbitrage::OnWeightTypeChange()
{
	
}

int32 CIoViewKLineArbitrage::CompareKLinesChange(const CArray<CKLine,CKLine>& KLineBef, const CArray<CKLine,CKLine>& KLineAft)
{
	// K 线更新的时候,判断最新的K 线与原来相比有什么变化:
	// 返回值: 0:相同 1:最新一根变化 2:增加了一根 3:除了以上情况,有大的变化

	int32 iReVal = 3;

	CArray<CKLine,CKLine> KLineBefor;
	KLineBefor.RemoveAll();
	KLineBefor.Copy(KLineBef);
	CArray<CKLine,CKLine> KLineAfter;
	KLineAfter.RemoveAll();
	KLineAfter.Copy(KLineAft);

	int32 iSizeBefore = KLineBefor.GetSize();
	int32 iSizeAfter  = KLineAfter.GetSize(); 
	
	char* pBefore = (char*)KLineBefor.GetData();
	char* pAfter  = (char*)KLineAfter.GetData();

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

			KLineAfter.RemoveAt(iSizeAfter-1);
			pAfter  = (char*)KLineAfter.GetData();

			if ( 0 == memcmp(pBefore,pAfter,iSizeBefore*sizeof(CKLine)))
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
			
			KLineBefor.RemoveAt(0);
			KLineAfter.RemoveAt(iSizeAfter-1);
			
			pBefore = (char*)KLineBefor.GetData();
			pAfter  = (char*)KLineAfter.GetData();
			
			if ( 0 == memcmp(pBefore,pAfter,(iSizeBefore - 1)*sizeof(CKLine)))
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
			
			KLineAfter.RemoveAt(iSizeAfter-1);
			KLineBefor.RemoveAt(iSizeBefore-1);
			
			pBefore = (char*)KLineBefor.GetData();
			pAfter  = (char*)KLineAfter.GetData();
			
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

//////////////////////////////////////////////////////////////////////////
void CIoViewKLineArbitrage::InitialImageResource()
{
	
} 

CString CIoViewKLineArbitrage::GetTimeString(CGmtTime Time, E_NodeTimeInterval eTimeInterval, bool32 bRecorEndTime /*= false*/)
{
	CString StrTime;
	
	if ( ENTIDay != eTimeInterval )
	{
		
	}
	else
	{
		CGmtTimeSpan TimeSpan(0, 0, 1, 0);
		Time += TimeSpan;
	}
	
	int32 iHour = Time.GetHour() + 8;
	if ( iHour >= 24 )
	{
		iHour -= 24;
	}
	
	StrTime.Format(L"%04d-%02d-%02d %02d:%02d:%02d", Time.GetYear(), Time.GetMonth(), Time.GetDay(), iHour, Time.GetMinute(), Time.GetSecond());	
	return StrTime;
}

bool32 CIoViewKLineArbitrage::GetChartXAxisSliderText1( OUT CString &StrSlider, CAxisNode &AxisNode )
{
	StrSlider.Empty();
	
	if (m_MerchParamArray.GetSize() <= 0)
	{
		// 可能存在新建的K 线视图,没有数据.	
		return false;
	}
	
	T_MerchNodeUserData* pMainData = m_MerchParamArray.GetAt(0);
	if (NULL == pMainData)
		return false;
	
	bool32 bSaveMinute = false;
	E_NodeTimeInterval eTimeInterval = pMainData->m_eTimeIntervalFull;
	if (ENTIMinute == eTimeInterval || ENTIMinute5 == eTimeInterval || ENTIMinute15 == eTimeInterval || ENTIMinute30 == eTimeInterval || ENTIMinute60 == eTimeInterval || ENTIMinute180 == eTimeInterval|| ENTIMinute240 == eTimeInterval|| ENTIMinuteUser == eTimeInterval)
		bSaveMinute = true;
	else if (ENTIDay == eTimeInterval || ENTIWeek == eTimeInterval || ENTIMonth == eTimeInterval || ENTIQuarter == eTimeInterval || ENTIYear == eTimeInterval || ENTIDayUser == eTimeInterval)
	{
		bSaveMinute = false;
	}
	else	// 不应该出现的类型
	{
		//ASSERT(0);
	}

	if ( NULL == m_pRegionMain )
	{
		// 不应该在RegionMain为空时就调用到了的 或者其它的情况
		ASSERT( 0 );
		return false;
	}
	
	// 作为AxisNode，timeId很重要，应当保存的
	int32	iRegionX = AxisNode.m_iCenterPixel;
	int32	timeId = AxisNode.m_iTimeId;
	if ( 0 != timeId )
	{
		// timeId初始化了，就不需要在找了
	}
	else
	{
		// 根据AxisNode中的X坐标，找到timeID
		m_pRegionMain->ClientXToRegion(iRegionX);
		if( NULL == m_pRegionMain->GetDependentCurve()
			|| ! m_pRegionMain->GetDependentCurve()->RegionXToCurveID(iRegionX, timeId) )
		{
			// 不应该在RegionMain为空时就调用到了的 或者其它的情况 - 有可能X ID会失败
			//ASSERT( 0 );
			//m_pRegionMain->GetDependentCurve()->RegionXToCurveID(iRegionX, timeId);
			return false;
		}
	}
	
	CTime Time(timeId);		// 总是当地时间显示
	
	// 是否使用m_StrSliderText1保存运算结果？
	if (bSaveMinute)
		StrSlider.Format(L"%02d/%02d %02d:%02d", Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute());
	else
		StrSlider.Format(L"%04d/%02d/%02d(%s)", Time.GetYear(), Time.GetMonth(), Time.GetDay(), GetWeekString(Time).GetBuffer());
	
	//TRACE(StrSlider);
	
	return true;
}

int32 CIoViewKLineArbitrage::GetSpaceRightCount()
{
	int32 iSpace = KSpaceRightCount;
	CGmtTime TimeStartInFullList, TimeEndInFullList;
	
	bool32 bShowNewestKLine		= IsShowNewestKLine(TimeStartInFullList, TimeEndInFullList);
	if ( !bShowNewestKLine )
	{
		return 0;
	}
	return iSpace;
}

bool32 CIoViewKLineArbitrage::FromXmlInChild( TiXmlElement *pTiXmlElement )
{
	// 初始化默认副图索引
	ASSERT( NULL != pTiXmlElement );

	// 最后一个副图
	const char *pszPickSubRegionIndex = pTiXmlElement->Attribute(KXMLAttriIoViewKLineArbPickSubRegionIndex);
	if ( NULL != pszPickSubRegionIndex )
	{
		m_iLastPickSubRegionIndex = atoi(pszPickSubRegionIndex);
		// 不考虑越界情况
	}

	// 当前的套利图表类型
	const char *pszArbShowType = pTiXmlElement->Attribute(KXMLAttriIoViewKLineArbShowType);
	if ( NULL != pszArbShowType )
	{
		m_eArbitrageShow = (E_ArbitrageChartShowType)atoi(pszArbShowType);
		if ( m_eArbitrageShow >= EACST_Count )
		{
			ASSERT( 0 );
			m_eArbitrageShow = EACST_CloseDiff;
		}
	}

	TiXmlElement* pArbitrageElement = pTiXmlElement->FirstChildElement("Arbitrage");
	
	if (NULL != pArbitrageElement)
	{
		bool32 bOK = m_Arbitrage.FromXml(pArbitrageElement,m_pAbsCenterManager);
		if (bOK)
		{
			// 防止不同步
			CArbitrageManage::Instance()->AddArbitrage(m_Arbitrage);
		}
	}

	return true;
}

CString CIoViewKLineArbitrage::ToXmlInChild()
{
	CString StrRet;

	if ( m_iLastPickSubRegionIndex >= 0 && m_iLastPickSubRegionIndex < m_SubRegions.GetSize() )
	{
		CString StrIndex;
		StrIndex.Format(_T(" %s=\"%d\" ")
			, CString(KXMLAttriIoViewKLineArbPickSubRegionIndex).GetBuffer(), m_iLastPickSubRegionIndex
			);
		StrRet += StrIndex;
	}

	//
	if ( m_eArbitrageShow != EACST_CloseDiff )
	{
		CString StrIndex;
		StrIndex.Format(_T(" %s=\"%d\" ")
			, CString(KXMLAttriIoViewKLineArbShowType).GetBuffer(), m_eArbitrageShow
			);
		StrRet += StrIndex;
	}

	return StrRet;
}

CString CIoViewKLineArbitrage::ToXmlEleInChild()
{
	CString StrRet = m_Arbitrage.ToXml();
	return StrRet;
}

void CIoViewKLineArbitrage::GetCurrentIndexNameArray( OUT CStringArray &aIndexNames )
{
	aIndexNames.RemoveAll();

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
			aIndexNames.Add(pParam->strIndexName);
		}
	}
}

void CIoViewKLineArbitrage::ChangeToNextIndex(bool32 bPre)
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

	//
	if ( m_iLastPickSubRegionIndex < 0 || m_iLastPickSubRegionIndex >= m_SubRegions.GetSize() )
	{
		m_iLastPickSubRegionIndex = 0;
	}
	
	T_SubRegionParam Param = m_SubRegions.GetAt(m_iLastPickSubRegionIndex);
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

void CIoViewKLineArbitrage::OnChartDBClick( CPoint ptClick, int32 iNodePos )
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
		return;
	}

	if ( m_pRegionMain->m_aXAxisNodes.GetSize() <= iNodePos || iNodePos < 0 )
	{
		return;
	}
}

bool32 CIoViewKLineArbitrage::ChangeMainDataShowData( T_MerchNodeUserData &MainMerchNode, int32 iShowPosInFullList, int32 iShowCountInFullList, bool32 bPreShowNewest, const CGmtTime &TimePreStart, const CGmtTime &TimePreEnd )
{
	if (!MainMerchNode.UpdateShowData(iShowPosInFullList, iShowCountInFullList))
	{
		return false;
	}
	
	CGmtTime TimeNowStart, TimeNowEnd;
	bool32	bIsShowNewestNow = IsShowNewestKLine(TimeNowStart, TimeNowEnd);
	if ( bIsShowNewestNow != bPreShowNewest )	// 管制最新状态变更， 严格讲，这样判断是会出错的
	{
		UpdateMainMerchKLine(MainMerchNode, true);
	}
	else
	{
		// 重新计算坐标
		UpdateAxisSize();
	}

	return true;
}

bool32 CIoViewKLineArbitrage::CalcKLinePriceBaseValue( CChartCurve *pCurve, OUT float &fPriceBase )
{
	// 主线与叠加线使用他们自己的基础值，指标线使用与主线相同的基础值
	fPriceBase = CChartCurve::GetInvalidPriceBaseYValue();
	if ( NULL == pCurve )
	{
		return false;
	}
	// 使用视图的坐标类型，而不是线的坐标类型计算
	CPriceToAxisYObject::E_AxisYType eType = GetMainCurveAxisYType();
	if ( CPriceToAxisYObject::EAYT_Normal == eType )
	{
		// 普通类型不需要基础值
		// 这里按照百分比取一个基础值
		CNodeData node;
		// 如果是指标线，则用主线的
		if ( CheckFlag(pCurve->m_iFlag, CChartCurve::KTypeIndex) )
		{
			ASSERT( 0 );	// 不应该重复计算的
			if ( NULL == m_pRegionMain || (pCurve=m_pRegionMain->GetDependentCurve()) == NULL )
			{
				return true;
			}
		}
		
		if ( pCurve->GetValidFrontNode(0, node) )
		{
			fPriceBase = node.m_fOpen;
			return true;
		}
		return true;
	}
	else if ( CPriceToAxisYObject::EAYT_Pecent == eType )
	{
		// 百分比，取线的第一根开盘价
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

		if ( pCurve->GetValidFrontNode(0, node) )
		{
			fPriceBase = node.m_fOpen;
			return true;
		}
	}
	else if ( CPriceToAxisYObject::EAYT_Log == eType )
	{
		// 对数，取线的倒数第二根的收盘价或者唯一一根的开盘价
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
		
		if ( pCurve->GetValidBackNode(1, node) )
		{
			fPriceBase = node.m_fClose;	// 倒数第二天的收盘
			return true;
		}
		else if ( pCurve->GetValidBackNode(0, node) )
		{
			fPriceBase = node.m_fOpen; // 唯一一天的开盘
			return true;
		}
	}
	else
	{
		ASSERT( 0 );	// 不支持类型
	}

	
	return false;
}

void CIoViewKLineArbitrage::OnShowDataTimeRangeChanged( T_MerchNodeUserData *pData )
{
	if ( m_MerchParamArray.GetSize() <= 0 || m_MerchParamArray[0] == NULL )
	{
		return;
	}
}

CChartCurve* CIoViewKLineArbitrage::GetCurIndexCurve( CChartRegion *pRegionParent )
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

void CIoViewKLineArbitrage::AdjustCtrlFloatContent()
{
	
}

void CIoViewKLineArbitrage::OnArbitrageAdd( const CArbitrage& stArbitrage )
{
	
}

void CIoViewKLineArbitrage::OnArbitrageDel( const CArbitrage& stArbitrage )
{
	// 这删除了能做什么？
}

void CIoViewKLineArbitrage::OnArbitrageModify( const CArbitrage& stArbitrageOld, const CArbitrage& stArbitrageNew )
{
	if ( m_Arbitrage == stArbitrageOld )
	{
		SetArbitrage(stArbitrageNew);
	}
}

void CIoViewKLineArbitrage::OnPickChart( CChartRegion *pRegion, int32 x, int32 y, int32 iFlag )
{
	
}

bool32 CIoViewKLineArbitrage::SetArbitrage( IN const CArbitrage &arb )
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
	int32 i = 0;
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
	
	// 图形点数量初始化
	// 缩小的很严重时, 切换商品不要记住这个比例
	if ( m_iNodeCountPerScreen > KMinRequestKLineNumber * 1.5 )
	{
		m_iNodeCountPerScreen = (int32)(KMinRequestKLineNumber * 1.5);
	}
	
	// 在m_pMerchXml为NULL时，初始化屏幕应当显示的Node数量，如果是从Xml中初始化，Node数量应该已经初始化好了的
	if ( !m_bFromXml )
	{
		if ( m_bRequestNodesByRectView )
		{
			// 根据当前视图大小,请求合适的K 线个数
			// 因为这时候是先请求数据再计算画图等矩形坐标大小.所以这时候 m_pRegionMain->m_RectView 是0.
			// 不要用这个值来算,用GGTongView 的宽度值来算
			
			ResetNodeScreenCount();
			
			// 一次性的标志,用完就复位
			m_bRequestNodesByRectView = false;
		}
	}

	m_Arbitrage = arb;

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

	OnVDataForceUpdate();
	
	return true;
}

void CIoViewKLineArbitrage::OnTimer( UINT nIDEvent )
{
	if ( KTimerIdCalcArbitrage == nIDEvent )
	{
		// 计算套利，因为套利有多个商品的更新，所有全部都是延时计算
		/*bool32 b =*/ CalcArbitrage();
		KillTimer(nIDEvent);
		if ( NULL != m_pRegionMain )
		{
			m_pRegionMain->SetDrawFlag(CChartRegion::KDrawFull);
			Invalidate();
		}
	}
	CIoViewChart::OnTimer(nIDEvent);
}

void CIoViewKLineArbitrage::CalcArbitrageAsyn()
{
	SetTimer(KTimerIdCalcArbitrage, KTimerPeriodCalcArbitrage, NULL);
}

void CIoViewKLineArbitrage::CancelCalcArbAsyn()
{
	KillTimer(KTimerIdCalcArbitrage);
}

bool32 CIoViewKLineArbitrage::CalcArbitrage()
{
	CancelCalcArbAsyn();

	T_MerchNodeUserData *pMainData = GetMainData(true);
	if ( NULL == pMainData )
	{
		return false;
	}

	MerchArray aMerchs;
	if ( !GetArbitrageMerchs(m_Arbitrage, aMerchs) )
	{
		// 不能获取商品信息，清除显示数据
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

	CGmtTime TimeStartInFullList, TimeEndInFullList;
	bool32 bShowNewestKLine		= IsShowNewestKLine(TimeStartInFullList, TimeEndInFullList);
	int32 iShowPosInFullList	= pMainData->m_iShowPosInFullList;
	int32 iShowCountInFullList	= pMainData->m_iShowCountInFullList;
	int32 iRightSpace = GetSpaceRightCount();

	DWORD dwTime = timeGetTime();
	DWORD dwTime2;

	bool32 bOk = true;
	CArray<CKLine, CKLine> aKlineLegA, aKlineLegB;
	
	CArbitrageManage::Merch2KLineMap mapKLinePtrs;
	for ( i=0; i < aDatas.GetSize() ; ++i )
	{
		mapKLinePtrs[ aDatas[i]->m_pMerchNode ] = &aDatas[i]->m_aKLinesFull;
	}

	if ( !m_Arbitrage.IsNeedCMerch() && aDatas.GetSize() >= 2 )
	{
		// 两腿
		ASSERT( aDatas.GetSize() >= 2 );
		CArray<CKLine, CKLine> aKlineC;
		if ( aDatas[0]->m_aKLinesFull.GetSize() > 0 && aDatas[1]->m_aKLinesFull.GetSize() > 0 )
		{
			// 插入商品数据
			bOk = CArbitrageManage::Instance()->CombineArbitrageKLine(m_Arbitrage
											, mapKLinePtrs
											, aKlineLegA
											, aKlineLegB
											, pMainData->m_aKLinesFull);
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
		ASSERT( aDatas.GetSize() >= 3 );
		if ( aDatas[0]->m_aKLinesFull.GetSize() > 0 && aDatas[1]->m_aKLinesFull.GetSize() > 0 && aDatas[2]->m_aKLinesFull.GetSize() > 0 )
		{
			bOk = CArbitrageManage::Instance()->CombineArbitrageKLine(m_Arbitrage
								, mapKLinePtrs
								, aKlineLegA
								, aKlineLegB
								, pMainData->m_aKLinesFull);
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
	TRACE(_T("计算套利耗时: %d ms\r\n"), dwTime2-dwTime);
	dwTime = dwTime2;

	if ( bOk )
	{
		// 保留与清除数据
		if ( EACST_TowLegCmp == m_eArbitrageShow )
		{
			// 结果为两腿
			pMainData->m_aKLinesFull.Copy(aKlineLegA);	// A腿
		}
		else
		{
			// 结果为单个
		}

		// 修理最高最低价
		for ( int32 i=0; i < pMainData->m_aKLinesFull.GetSize() ; ++i )
		{
			CKLine &kline = pMainData->m_aKLinesFull[i];
			kline.m_fPriceHigh	= max(kline.m_fPriceOpen, kline.m_fPriceClose);
			kline.m_fPriceHigh	= max(kline.m_fPriceHigh, kline.m_fPriceAvg);
			kline.m_fPriceLow	= min(kline.m_fPriceOpen, kline.m_fPriceClose);
			kline.m_fPriceLow	= min(kline.m_fPriceLow, kline.m_fPriceAvg);
		}

		// 相对于前一次的计算结果，如果是同商品参与计算，则不会少结果，只会增加
		// 不同商品参与计算，则只要保证有这么多点就可以了
		if ( iShowCountInFullList <= 0 )
		{
			iShowCountInFullList = m_iNodeCountPerScreen - iRightSpace;
		}
		if ( !bShowNewestKLine )
		{
			if ( iShowPosInFullList + iShowCountInFullList > pMainData->m_aKLinesFull.GetSize() )
			{
				bShowNewestKLine = true;
			}
		}
		if ( bShowNewestKLine )
		{
			// 总是显示尽量多的数据
			iShowCountInFullList = m_iNodeCountPerScreen - iRightSpace;
			iShowPosInFullList = pMainData->m_aKLinesFull.GetSize()-iShowCountInFullList;
			if ( iShowPosInFullList < 0 )
			{
				iShowCountInFullList += iShowPosInFullList;
				iShowPosInFullList = 0;
			}
		}

		// 计算公式等
		//********************************************************************************************************************************************
		// 更新的指标
		for (int32 j = 0; j < pMainData->aIndexs.GetSize(); j++)
		{
			T_IndexParam* pIndex = pMainData->aIndexs.GetAt(j);
			bool32 b = g_formula_compute(this, this, (CChartRegion*)pIndex->pRegion, pMainData, pIndex, iShowPosInFullList, iShowPosInFullList + iShowCountInFullList); 				
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
		if (!pMainData->UpdateShowData(iShowPosInFullList, iShowCountInFullList))
		{
			////ASSERT(0);
			bOk = false;
		}

		// 如果当前为两腿模式，合成B线数据
		if ( EACST_TowLegCmp == m_eArbitrageShow )
		{
			// 比较线的设置
			ASSERT( NULL != m_pCuveOtherArbLeg );
			CNodeSequence *pNode2 = m_pCuveOtherArbLeg->GetNodes();
			ASSERT( NULL != pNode2 );
			CArray<CNodeData, CNodeData&>& aNodes2 = pNode2->GetNodes();
			aNodes2.RemoveAll();
			int32 iStart, iEnd;
			iStart = iEnd = -1;
			IsShowNewestKLine(TimeStartInFullList, TimeEndInFullList);
			iStart	= CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(aKlineLegB, TimeStartInFullList);
			iEnd	= CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(aKlineLegB, TimeEndInFullList);
			if ( iStart >= 0 && iEnd >= iStart && iEnd < aKlineLegB.GetSize() )
			{
				aNodes2.SetSize(iEnd-iStart+1);
				int32 iNode = 0;
				for ( int32 i=iStart; i <= iEnd ; ++i, ++iNode )
				{
					CNodeData &node = aNodes2[iNode];
					KLine2NodeData(aKlineLegB[i], node);
					
					node.m_fHigh	= max(node.m_fOpen, node.m_fClose);
					node.m_fHigh	= max(node.m_fHigh, node.m_fAvg);
					node.m_fLow		= min(node.m_fOpen, node.m_fClose);
					node.m_fLow		= min(node.m_fLow, node.m_fAvg);
				}
			}
		}
	}

	dwTime2 = timeGetTime();
	TRACE(_T("后续处理套利耗时: %d ms\r\n"), dwTime2-dwTime);

	if ( !bOk )
	{
		ClearLocalData(false);
	}

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

T_MerchNodeUserData * CIoViewKLineArbitrage::GetMerchData( CMerch *pMerch, bool32 bCreateIfNotExist/*=false*/ )
{
	if ( NULL == pMerch )
	{
		return NULL;
	}

	T_MerchNodeUserData *pMainData = GetMainData(true);
	if ( NULL == pMainData )
	{
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
			
			// 加入关注 - 在SetArb时就设置好关注的商品
			
			return pData;
			// 此时还是没有设置标题的
		}
	}
	return NULL;
}

bool32 CIoViewKLineArbitrage::ResetMerchUserData( T_MerchNodeUserData *pMerchData )
{
	if ( NULL == pMerchData || pMerchData->bMainMerch )
	{
		return false;
	}

	const T_MerchNodeUserData *pMainData = GetMainData(true);
	if ( NULL == pMainData )
	{
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
bool32 CIoViewKLineArbitrage::InitMainUserData()
{
	T_MerchNodeUserData *pMainData = m_MerchParamArray.GetSize()>0? m_MerchParamArray[0] : NULL;
	if ( NULL == pMainData && NULL != m_pRegionMain && NULL != m_pAbsCenterManager )
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
		
		// 保持默认日线
		
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
			CChartCurve* pCurve = m_pRegionMain->CreateCurve(CChartCurve::KDependentCurve|CChartCurve::KRequestCurve|CChartCurve::KTypeKLine|CChartCurve::KYTransformByClose|CChartCurve::KUseNodesNameAsTitle/*|CChartCurve::KDonotPick*/);		 
			CChartDrawer* pKLineDrawer = new CChartDrawer(*this, (CChartDrawer::E_ChartDrawStyle)CChartDrawer::EKDSTrendPrice);
			
			if ( -1 != m_iDrawTypeXml )
			{
				//pKLineDrawer->m_eChartDrawType = (CChartDrawer::E_ChartDrawStyle)m_iDrawTypeXml;	// 不需要从xml中取值，共享全局值
				m_iDrawTypeXml = 0;
			}
			pCurve->AttatchDrawer(pKLineDrawer);
			//DEL(pKLineDrawer);
			pCurve->AttatchNodes(pNodes);
			
			// new 2 curve
			pCurve = m_pRegionXBottom->CreateCurve(CChartCurve::KDependentCurve|CChartCurve::KRequestCurve|CChartCurve::KInVisible|CChartCurve::KTypeKLine|CChartCurve::KYTransformByLowHigh);
			pCurve->AttatchNodes(pNodes);

			// 另外一条腿线的初始化
			ASSERT( NULL == m_pCuveOtherArbLeg );
			pNodes = CreateNodes();
			pNodes->SetUserData(pData);
			pNodes->SetName(_T("两腿叠加B"));
			pCurve = m_pRegionMain->CreateCurve(CChartCurve::KRequestCurve|CChartCurve::KTypeKLine|CChartCurve::KYTransformByClose|CChartCurve::KUseNodesNameAsTitle|CChartCurve::KYTransformToAlignDependent/*|CChartCurve::KDonotPick*/);		 
			CChartDrawer* pKLineDrawer1 = new CChartDrawer(*this, (CChartDrawer::E_ChartDrawStyle)CChartDrawer::EKDSTrendPrice);
			pKLineDrawer1->SetSingleColor(GetIoViewColor(ESCVolume2));
			pCurve->AttatchDrawer(pKLineDrawer1);
			//DEL(pKLineDrawer);
			pCurve->AttatchNodes(pNodes);
			m_pCuveOtherArbLeg = pCurve;
			m_pCuveOtherArbLeg->m_iFlag |= CChartCurve::KInVisible;	// 没有两腿时隐藏

			AdjustArbCurveByShowType();
		}
	}
	return true;
}

T_MerchNodeUserData	* CIoViewKLineArbitrage::GetMainData(bool32 bCreateIfNotExist/*=false*/)
{
	if ( m_MerchParamArray.GetSize() <= 0 && bCreateIfNotExist )
	{
		InitMainUserData();
	}
	return m_MerchParamArray.GetSize()>0? m_MerchParamArray[0] : NULL;
}

void CIoViewKLineArbitrage::SetArbShowType( E_ArbitrageChartShowType eType )
{
	if ( eType < EACST_Count )
	{
		m_eArbitrageShow = eType;
		
		AdjustArbCurveByShowType();
	}
}

void CIoViewKLineArbitrage::AdjustArbCurveByShowType()
{
	if ( NULL != m_pRegionMain )
	{
		T_MerchNodeUserData *pMainData = GetMainData(false);
		ASSERT( NULL != pMainData );
		if ( NULL != pMainData )
		{
			CChartCurve *pCurve = m_pRegionMain->GetDependentCurve();
			ASSERT( NULL != pCurve );
			if ( NULL != pCurve )
			{
				CChartDrawer *pDrawer = (CChartDrawer *)pCurve->GetDrawer();
				ASSERT( NULL != pDrawer );
				const uint32 uFlagMask = ~(0x1f0000);
				pCurve->m_iFlag &= uFlagMask;
				ASSERT( NULL != m_pCuveOtherArbLeg );
				m_pCuveOtherArbLeg->m_iFlag |= CChartCurve::KInVisible;
				switch( m_eArbitrageShow )
				{
				case EACST_CloseDiff:
					pCurve->m_iFlag |= CChartCurve::KYTransformByClose;
					pDrawer->m_eChartDrawType = CChartDrawer::EKDSTrendPrice;
					break;
				case EACST_AvgDiff:
					pCurve->m_iFlag |= CChartCurve::KYTransformByAvg;
					pDrawer->m_eChartDrawType = CChartDrawer::EKDSTrendPrice;
					break;
				case EACST_ArbKLine:
					pCurve->m_iFlag |= CChartCurve::KYTransformByLowHigh;
					pDrawer->m_eChartDrawType = CChartDrawer::EKDSArbitrageKline;
					break;
				case EACST_TowLegCmp:
					{
						pCurve->m_iFlag |= CChartCurve::KYTransformByClose;
						pDrawer->m_eChartDrawType = CChartDrawer::EKDSTrendPrice;
						// 另外一条线在初始化时就保持一致了
						m_pCuveOtherArbLeg->m_iFlag &= ~(CChartCurve::KInVisible);
					}
					break;
				default:
					ASSERT( 0 );
					pCurve->m_iFlag |= CChartCurve::KYTransformByClose;
					pDrawer->m_eChartDrawType = CChartDrawer::EKDSTrendPrice;
					break;
				}
			}
			SetCurveTitle(pMainData);
		}
	}
}

void CIoViewKLineArbitrage::OnIoViewColorChanged()
{
	CIoViewChart::OnIoViewColorChanged();
	if ( NULL != m_pCuveOtherArbLeg )
	{
		CChartDrawer *pDrawer = (CChartDrawer *)m_pCuveOtherArbLeg->GetDrawer();
		pDrawer->SetSingleColor(GetIoViewColor(ESCVolume2));
	}
}

bool32 CIoViewKLineArbitrage::FromXml( TiXmlElement * pTiXmlElement )
{
	bool32 bXml = CIoViewChart::FromXml(pTiXmlElement);
	
	//
	SetArbitrage(m_Arbitrage);

	return bXml;
}

CMerch * CIoViewKLineArbitrage::GetMerchXml()
{
	if ( NULL != m_Arbitrage.m_MerchA.m_pMerch )
	{
		return m_Arbitrage.m_MerchA.m_pMerch;
	}
	CMerch *pMerch = CIoViewChart::GetMerchXml();
	return pMerch;
}

void CIoViewKLineArbitrage::TestSetArb(bool32 bAsk/*=false*/)
{
	if(NULL == m_pAbsCenterManager)
	{
		return;
	}

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

bool32 CIoViewKLineArbitrage::OnGetCurveTitlePostfixString( CChartCurve *pCurve, CPoint ptClient, bool32 bHideCross, OUT CString &StrPostfix )
{
	// 仅主图需要额外处理这些
	if ( pCurve == NULL || &pCurve->GetChartRegion() != m_pRegionMain)
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

	int32 iSaveDec = m_Arbitrage.m_iSaveDec;

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

void CIoViewKLineArbitrage::OnF5()
{
	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if ( NULL == pMainFrame )
	{
		return;
	}
	
	pMainFrame->OnArbitrageF5(m_Arbitrage, this);
}

BOOL CIoViewKLineArbitrage::PreTranslateMessage( MSG* pMsg )
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

bool32 CIoViewKLineArbitrage::GetArbitrage( OUT CArbitrage &arb )
{
	MerchArray aMerchs;
	if ( GetArbitrageMerchs(m_Arbitrage, aMerchs) )
	{
		arb = m_Arbitrage;
		return true;
	}
	return false;
}

bool32 CIoViewKLineArbitrage::OnSpecialEsc()
{
	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if ( NULL == pMainFrame )
	{
		return true;
	}
	
	return !pMainFrame->OnArbitrageEscReturn(m_Arbitrage, this);
}

int32 CIoViewKLineArbitrage::GetSubMerchNeedDataCount()
{
	return m_iNodeCountPerScreen*2;
}

void CIoViewKLineArbitrage::ResetNodeScreenCount()
{
	CRect rect(0,0,0,0);			
	CGGTongView* pGGTongView = GetParentGGtongView();
	
	if ( NULL != pGGTongView)
	{
		pGGTongView->GetClientRect(&rect);
		
		int32 iWidth = rect.Width();
		
		int32 iNums	 = iWidth / KDefaultKLineWidth;
		
		if ( iNums < KMinKLineNumPerScreen )
		{
			iNums = KMinKLineNumPerScreen;
		}
		else if ( iNums > KMaxKLineNumPerScreen)
		{
			iNums = KMaxKLineNumPerScreen;
		}
		
		m_iNodeCountPerScreen	  = iNums;
	}
}

void CIoViewKLineArbitrage::UpdateKelineArbitrage( E_NodeTimeInterval NodeInterval )
{
    T_MerchNodeUserData* pData = GetMainData();
    if(NULL != pData)
    {
        SetTimeInterval(*pData, NodeInterval);
        SetCurveTitle(pData);
    }

    // 请求数据
    RequestViewData();

    // 
    UpdateSelfDrawCurve();
    m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
    Invalidate();
}

//////////////////////////////////////////////////////////////////////////
//
bool32 GetArbitrageMerchs( IN const CArbitrage &arb, OUT CIoViewBase::MerchArray &aMerchs )
{
	aMerchs.RemoveAll();
	if ( arb.m_eArbitrage == EABTwoMerch )
	{
		if ( arb.m_MerchA.m_pMerch != NULL && arb.m_MerchB.m_pMerch != NULL )
		{
			CArbitrage::ArbMerchArray aOtherMerch;
			arb.GetOtherMerchs(aOtherMerch, true);
			for ( int i=0; i < aOtherMerch.size() ; ++i )
			{
				aMerchs.Add( aOtherMerch[i] );
			}
			return true;
		}
	}
	else if ( arb.m_eArbitrage == EABThreeMerch )
	{
		if ( arb.m_MerchA.m_pMerch != NULL && arb.m_MerchB.m_pMerch != NULL && arb.m_MerchC.m_pMerch != NULL )
		{
			CArbitrage::ArbMerchArray aOtherMerch;
			arb.GetOtherMerchs(aOtherMerch, true);
			for ( int i=0; i < aOtherMerch.size() ; ++i )
			{
				aMerchs.Add( aOtherMerch[i] );
			}
			
			return true;
		}
	}
	return false;
}

CString ArbShowTypeToString( E_ArbitrageChartShowType eType )
{
	switch (eType)
	{
	case EACST_CloseDiff:
		return _T("收盘价价差");
		break;
	case EACST_AvgDiff:
		return _T("均价价差");
		break;
	case EACST_ArbKLine:
		return _T("套利K线");
		break;
	case EACST_TowLegCmp:
		return _T("两腿叠加");
		break;
	}
	return _T("未知价差");
}

CString ArbEnumToString( E_Arbitrage eType )
{
	switch (eType)
	{
	case EABTwoMerch:
		return _T("两腿套利");
		break;
	case EABThreeMerch:
		return _T("蝶式套利");
		break;
	}
	return _T("未知套利模型");
}
