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
	const int32 KTimerIdCalcArbitrage		= 10086;		// ��������
	const int32 KTimerPeriodCalcArbitrage	= 10;


	const int32 KSpaceRightCount			= 4;			// �ұ߿ճ���K������

	const int32 KMinKLineNumPerScreen		= 10;			// ������ʾK������, ����KSpaceRightCountλ��
	const int32 KMaxKLineNumPerScreen		= 5000;		// �����ʾK������
	const int32 KDefaultKLineNumPerScreen	= 200;

	const int32 KDefaultCtrlMoveSpeed		= 5;			// ��Ctrl������ʱ��Ĭ��ƽ�Ƶĵ�λ

	const int32 KMinRequestKLineNumber		= 300;			// һ����������������
	const int32 KRequestKLineNumberScale	= 2;			// Ĭ�ϵ�һ������K������
	const int32 KDefaultKLineWidth			= 15;			// ������ͼ��С����K �ߵ�ʱ��,K �߽ڵ�Ĭ�Ͽ��

	// 
	const int32 KMaxMinuteKLineCount		= 10000;		// �������ķ���K�����ݣ� �������ƵĻ��� ���������������������

	//
	const int32 KiStatisticaKLineNums		= 200;			// ͳ�Ƶ�K �߸���

	// xml
	static const char KXMLAttriIoViewKLineArbPickSubRegionIndex[] = "PickSubRegion";	// ���ѡ��ĸ�ͼ
	static const char KXMLAttriIoViewKLineArbShowType[]				= "ArbShowType";	// ������ʾ����
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

	m_iLastPickSubRegionIndex = 0;	// Ĭ�ϵ�һ����ͼ

	m_eArbitrageShow = EACST_CloseDiff;	// ������
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
	pCell->SetText(_T(" ��ֵ"));

	pCell = (CGridCellSys *)pGridCtrl->GetCell(4, 0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(_T(" ����"));

	pCell = (CGridCellSys *)pGridCtrl->GetCell(6, 0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(_T(" ����"));

	pCell = (CGridCellSys *)pGridCtrl->GetCell(8, 0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(_T(" ����"));
	 
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
	//��K
	new CChartRegion ( this, m_pRegionMain, _T("k��"),
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
	case ENTIMinuteUser:			// �Զ�������
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

			if ( pData->m_iTimeUserMultipleMinutes <= 0)		// ���������
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

			if ( pData->m_iTimeUserMultipleDays <= 0)		// ���������
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
		//ASSERT(0);	// ��Ӧ�ó����������
		return;
	}

	ASSERT( MerchNodeUserData.bMainMerch );	// ����Ϊ��
	MerchNodeUserData.m_eTimeIntervalFull = eNodeTimeInterval;
	MerchNodeUserData.m_eTimeIntervalCompare = eNodeTimeIntervalCompare;
	for ( int32 i=1; i < m_MerchParamArray.GetSize() ; ++i )
	{
		ResetTimeInterval(m_MerchParamArray[i], eNodeTimeInterval, eNodeTimeIntervalCompare);
		ResetMerchUserData(m_MerchParamArray[i]);	// Ҫ������������ºϳ�
	}

	ResetNodeScreenCount();	// ������Ļ����

	CalcArbitrageAsyn();
}

void CIoViewKLineArbitrage::ResetTimeInterval(T_MerchNodeUserData* pData,IN E_NodeTimeInterval eNodeTimeInterval,IN E_NodeTimeInterval eNodeTimeIntervalCompare)
{
	pData->m_eTimeIntervalFull    = eNodeTimeInterval;
	pData->m_eTimeIntervalCompare = eNodeTimeIntervalCompare;

	//

	// ...fangz0828 ���ﲻҪ�������,�ᵼ�¶��������( ԭ�������ݵ�,���������������), �ñ��������Ȼ�����,�粻��������,��������������ݸ����ػ���ǰ��
	if ( pData->bMainMerch )
	{
		UpdateMainMerchKLine(*pData);
	}
	else
	{
		UpdateSubMerchKLine(*pData);
	}
}

// ֪ͨ��ͼ�ı��ע����Ʒ
void CIoViewKLineArbitrage::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	// ��ʵ����������������Ʒ���л����������������ݵı��
	_LogCheckTime("[Client==>] CIoViewKLineArbitrage::OnVDataMerchChanged", g_hwndTrace);

	if (m_pMerchXml == pMerch || NULL == pMerch )
		return;
	
	// �޸ĵ�ǰ�鿴����Ʒ
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
}

// ������ͼ���
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
			StrTitle = _T(" ") + TimeInterval2String(pData->m_eTimeIntervalFull) + _T(" (��)");				
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
				StrTitle += _T("A");	// ���߱�Ȼ��A
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
		// �������� �ַ�����/���ƻ���(����)
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
				iShowPosEnd >= 0 && iShowPosEnd < pMainData->m_aKLinesFull.GetSize())	// ����������ʾ��Χ��
			{
				// ��������ʱ��
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
	// ...fangz1117  Ŀǰ UpdateMainMerchKLine ����ÿ�ζ� bUpdate = false ȫ����������,
	// �Ż��Ժ�,Ҫ�����Ȩ������,��Ҫ��Ϊ�����̺�û��K �������ʱ��������û�и���

	if (NULL == pMerch || pMerch != m_pMerchXml)
		return;
	
	if (EPFTWeight != ePublicFileType)	// �������Ȩ����
		return;

	// ������Ʒ������������
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
		// �����mainnode
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
	
	// ������Ʒ������������ - ��ʵ�����ݿ�ʼ
	for (int32 i = 1; i < m_MerchParamArray.GetSize(); i ++)
	{
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(i);
		
		// mainʵ�ʲ�û�й�����Ʒ
		if (pData->m_pMerchNode == pMerch && !pData->bMainMerch )
		{
			UpdateSubMerchKLine(*pData, false);
		}
	}
}

bool32 CIoViewKLineArbitrage::UpdateMainMerchKLine(T_MerchNodeUserData &MerchNodeUserData, bool32 bForceUpdate)
{
	CalcArbitrage();	// ������ʼ������������
	return true;
}

bool32 CIoViewKLineArbitrage::UpdateSubMerchKLine(T_MerchNodeUserData &MerchNodeUserData, bool32 bForceUpdate/*=true*/)
{
	if (m_MerchParamArray.GetSize() <= 0 || m_MerchParamArray[0] == &MerchNodeUserData)
	{
		return false;
	}
	
	// �������¹�����Ʒ
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

	// zhangbo 20090714 #������
	//...
	_LogCheckTime("[==>Client] CIoViewKLineArbitrage::UpdateSubMerchKLine ��ʼ", g_hwndTrace);

	//
	E_NodeTimeInterval eNodeTimeIntervalCompare = ENTIDay;
	E_KLineTypeBase    eKLineType			    = EKTBDay;
	
	int32 iScale = 1;
	if (!GetTimeIntervalInfo(pMainData->m_eTimeIntervalCompare, eNodeTimeIntervalCompare, eKLineType, iScale))
	{
		//ASSERT(0);	// ��Ӧ�ó����������
		return false;
	}
	
	// �жϵ�ǰ��ʾ�� �Ƿ���ʾ���¼�
	CGmtTime TimeStartInFullList, TimeEndInFullList;

	IsShowNewestKLine(TimeStartInFullList, TimeEndInFullList);

	// �Ȼ�ȡ��Ӧ��K��
	int32 iPosFound;
	CMerchKLineNode* pKLineRequest = NULL;
	pMerch->FindMerchKLineNode(eKLineType, iPosFound, pKLineRequest);
	
	// �����Ҳ���K�����ݣ� �ǾͲ���Ҫ��ʾ��
	if (NULL == pKLineRequest || 0 == pKLineRequest->m_KLines.GetSize())	
	{
		ResetMerchUserData(&MerchNodeUserData);
		CalcArbitrageAsyn();
		return true;
	}

	// �Ż��������������£� ���¼�����������ʷ���ݸ��������ģ� ������������ر��жϴ���	

	int32 iCmpResult = bForceUpdate ? 1 : CompareKLinesChange(pKLineRequest->m_KLines, MerchNodeUserData.m_aKLinesCompare); // ԭʼ���ݱȽ�

	CArray<CKLine, CKLine> aFullKlines;
	
	// �п������ڻ���ȨϢ��Ϣ�����������Ҫ���ºϳ�k��
	if ( iCmpResult != 0 )
	{
		// ��Ҫ���ºϳ�K��
		// ���ԭʼ&��ʾ����
		MerchNodeUserData.m_aKLinesCompare.Copy(pKLineRequest->m_KLines);	// ����һ��ԭʼ����

		// �Ƿ񱣳���K����ͬ�ĸ�Ȩѡ��
		//////////////////////////////////////////////////////////////////////////
		// ��Ȩ��Ȩ:
		CArray<CKLine, CKLine> aSrcKLines;
		aSrcKLines.Copy(MerchNodeUserData.m_aKLinesCompare);
		
		// ���Ƿ�Ҫ����Ȩ����
		CArray<CKLine, CKLine> aWeightedKLines;
		
		if ( CIoViewKLine::EWTAft == m_eWeightType )
		{
			// ��Ȩ
			if ( CMerchKLineNode::WeightKLine(aSrcKLines, pMerch->m_aWeightDatas, false, aWeightedKLines) )
			{
				aSrcKLines.Copy(aWeightedKLines);
			}		
		}
		else if ( CIoViewKLine::EWTPre == m_eWeightType )  
		{
			// ǰ��Ȩ
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
		// �������ݣ� ѹ������
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
		
		// ʵ������K�ߺϳ����
		// �п���K��û�о������ݣ������Լ��ϳ�
		for ( int32 i=aFullKlines.GetSize()-1; i >= 0 ; --i )
		{
			CKLine &kline = aFullKlines[i];

			if (ENTIDay == MerchNodeUserData.m_eTimeIntervalFull)
			{
				if ( kline.m_fPriceAvg == 0.0f )
				{
					// û�������Լ�������
					// 					if ( kline.m_fVolume >= 0.9f )
					// 					{
					// 						float fAvg = kline.m_fAmount / kline.m_fVolume / 100;
					// 						if ( fAvg > kline.m_fPriceHigh || fAvg < kline.m_fPriceLow )
					// 						{
					// 							// ���������ƣ���Ҫ����
					// 							aFullKlines.RemoveAt(i);
					// 							continue;
					// 						}
					// 						kline.m_fPriceAvg = fAvg;
					// 					}
					kline.m_fPriceAvg = (kline.m_fPriceClose+kline.m_fPriceHigh+kline.m_fPriceLow+kline.m_fPriceOpen)/4.0f;
				}
				if ( kline.m_fPriceAvg > kline.m_fPriceHigh*1.20f || kline.m_fPriceAvg < kline.m_fPriceLow*0.80f )
				{
					// ���̫�󣬷����������д���������ݷ�����?
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
	// compareK����fullK������׼������
	// ����Full�Ѿ���Ҫ���K����
	bool32	bUpdateShowData = false;
	if ( 0 != iCmpResult )
	{
		iCmpResult = CompareKLinesChange(aFullKlines, MerchNodeUserData.m_aKLinesFull);
		if ( 0 != iCmpResult )
		{
			// FullK��Ҫ�����
			MerchNodeUserData.m_aKLinesFull.Copy(aFullKlines);
			bUpdateShowData = true;	// ���������ʾ����
		}
	}

	// ��Ʒ���ݼ�����ϣ���������
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
	// �õ�Tips
	StrTips = L"";
	return false;
}

void CIoViewKLineArbitrage::OnNodesRelease ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes )
{
	//T_MerchNodeUserData* pData = (T_MerchNodeUserData*)pNodes->GetUserData();
	//pData->pMerch�Ѿ��������Curveʹ����,���û������Curveʹ��,�����������,���Ҵ�AttendMerch��ɾ��.
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
	// �����е�ָ�궼���ص��˵���:

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
					
					bool32 bShow = CFormulaLib::BeIndexShow(pContent->name);	// �Ƿ��ڽ�������ʾ����
					
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
					// û���Ӳ˵���,������ɾ��
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
					
					CString StrIndexFull = MakeMenuString(StrText, StrExp);
					
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
	
	// ���ɽ������͵�����, ���Ϸָ���
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
	//���ѡ��������������
	iSize = m_SubRegions.GetSize();
	for ( i = 0; i < iSize; i ++ )
	{
		if ( m_SubRegions.GetAt(i).m_pSubRegionMain == pRegion )
		{
			iRegion = i+1;
			int32 iOld = m_iLastPickSubRegionIndex;
			m_iLastPickSubRegionIndex = i;	// �趨���һ��ѡ����region
			if ( iOld != m_iLastPickSubRegionIndex )
			{
				m_pRegionMain->SetDrawFlag(m_pRegionMain->GetDrawFlag() | CRegion::KDrawFull);
				Invalidate();
			}
		}
	}
	if ( pRegion == m_pRegionYLeft )
	{
		// ��Y�ᣬ������
		CNewMenu menu;
		menu.CreatePopupMenu();

		const UINT uBase = 100;
		menu.AppendMenu(MF_SEPARATOR);

		menu.AppendMenu(MF_STRING, uBase+CPriceToAxisYObject::EAYT_Normal, _T("��ͨ����"));
		menu.AppendMenu(MF_STRING, uBase+CPriceToAxisYObject::EAYT_Pecent, _T("�ٷֱ�����"));
		//menu.AppendMenu(MF_STRING, uBase+CPriceToAxisYObject::EAYT_Log,	   _T("��������"));

		menu.CheckMenuItem(uBase+GetMainCurveAxisYType(), MF_BYCOMMAND |MF_CHECKED);

		menu.DeleteMenu(0, MF_BYPOSITION);

		CPoint pt(x,y);
		ClientToScreen(&pt);

		UINT uRet = menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL |TPM_NONOTIFY |TPM_RETURNCMD,pt.x, pt.y, AfxGetMainWnd());
		// �����͸�mainframe�ˣ��Լ�����
		if ( uRet >= uBase )
		{
			SetMainCurveAxisYType((CPriceToAxisYObject::E_AxisYType)(uRet-uBase));	// �����������
		}
	}
	if ( iRegion < 0 )
	{
		return;
	}

	m_pCurvePick	= (CChartCurve*)pCurve;
	if ( NULL == m_pCurvePick )
	{
		m_pCurvePick = GetCurIndexCurve(pRegion);	// ʹ��region�ĵ�һ��ָ����Ϊ��ǰָ��
	}
	m_pNodesPick	= pNodes;
	m_NodePick		= CNodeData(*pNodeData);

	// NodePick���������ϴ��Ҽ�������ĵ�
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

	// ����
	pTempMenu = pPopMenu->GetSubMenu(_T("�����۲�����"));
	ASSERT( NULL != pTempMenu );
	if ( NULL != pTempMenu )
	{
		pTempMenu->CheckMenuItem(ID_ARBKLINE_CLOSEDIFF+m_eArbitrageShow, MF_BYCOMMAND|MF_CHECKED);
	}

	//////////////////////////////////////////////////////////////////////////
	pTempMenu = pPopMenu->GetSubMenu(_T("����ָ��"));
	ASSERT(NULL!=pTempMenu);

	CNewMenu* pAllIndexMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );	
	LoadAllIndex(pAllIndexMenu);

	//
	pTempMenu = pPopMenu->GetSubMenu(_T("����ָ��"));
	ASSERT(NULL!=pTempMenu);
	
	pSubMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );
	pSubMenu->RemoveMenu(0,MF_BYPOSITION|MF_SEPARATOR);

	int32 iIndexNums = LoadOftenIndex(pSubMenu);

	if ( 0 == iIndexNums )
	{
		// û��ָ���ʱ��
		pPopMenu->DeleteMenu((UINT)pSubMenu->GetSafeHmenu(), MF_BYCOMMAND);
		pTempMenu = NULL;
		pSubMenu = NULL;
	}

	// ��ͼ����
	if ( m_pRegionPick == m_pRegionMain )
	{
		// ��
		pTempMenu = pPopMenu->GetSubMenu(L"��ͼ����");
		
		if ( NULL != pTempMenu )
		{
			CNewMenu* pMainAxisYMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu);
			
			pMainAxisYMenu->CheckMenuItem(ID_KLINE_AXISYNORMAL+GetMainCurveAxisYType(), MF_CHECKED|MF_BYCOMMAND);
		}
	}
	else
	{
		// ɾ���ò˵���
		pTempMenu = pPopMenu->GetSubMenu(L"��ͼ����");
		if ( NULL != pTempMenu )
		{
			pPopMenu->DeleteMenu((UINT)pTempMenu->GetSafeHmenu(), MF_BYCOMMAND);
			pTempMenu = NULL;
		}
	}

	// ��������:
	pTempMenu = pPopMenu->GetSubMenu(L"��������");
	ASSERT(NULL!=pTempMenu);
	CNewMenu * pIoViewPopMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );
	AppendIoViewsMenu(pIoViewPopMenu, IsLockedSplit());

	// ��������
	pTempMenu = pPopMenu->GetSubMenu(_T("ѡ������"));
	ASSERT(NULL!=pTempMenu);
	pSubMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );
	
	i = IDM_CHART_KMINUTE + pData->m_eTimeIntervalFull;
	pSubMenu->CheckMenuItem(i,MF_BYCOMMAND|MF_CHECKED); 

	//���������Chart,����ʾ"�·���������"
	if ( m_pRegionPick != m_pRegionMain)
	{	
		//menu.RemoveMenu(IDM_CHART_INSERT, MF_BYCOMMAND);		// ͬʱ��ʾ
		//���������ﵽ6��,������������
		if ( m_SubRegions.GetSize() > 5 )
		{			
			menu.RemoveMenu(IDM_CHART_INSERT, MF_BYCOMMAND);
		}
	}
	//�������chart
	else
	{
		//���������ﵽ6��,������������
		if ( m_SubRegions.GetSize() > 5 )
		{			
			menu.RemoveMenu(IDM_CHART_INSERT, MF_BYCOMMAND);
		}
		//����ʾ"ɾ������"
		//menu.RemoveMenu(IDM_CHART_DELETE, MF_BYCOMMAND);	// ͬʱ��ʾ
		//���������ﵽ6��,������������
		if ( m_iLastPickSubRegionIndex < 0 || m_iLastPickSubRegionIndex >= m_SubRegions.GetSize() )
		{			
			menu.RemoveMenu(IDM_CHART_DELETE, MF_BYCOMMAND);	// ��ѡ��subregion
		}
	}

 	if ( NULL != m_pCurvePick && CheckFlag(m_pCurvePick->m_iFlag,CChartCurve::KTypeIndex))
 	{
 		//NULL;
 	}
 	else
	{
		pTempMenu = pPopMenu->GetSubMenu(L"��ǰָ��");
		if ( NULL != pTempMenu )
		{
			pPopMenu->DeleteMenu((UINT)pTempMenu->GetSafeHmenu(), MF_BYCOMMAND);
			pTempMenu = NULL;
		}
	}

	// ͼ��:
	pPopMenu->ModifyODMenu(L"ѡ������", L"ѡ������", IDB_TOOLBAR_KLINECYLE);
	pPopMenu->ModifyODMenu(L"�Ի���",   L"�Ի���",   IDB_TOOLBAR_OWNDRAW);

	// ������������ָ�״̬����Ҫɾ��һЩ��ť
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
	// ������ָ���Ӧ�Ĵ������.����ǰ�Ĳ˵��ֿ�.�������.����Ҫ��ʱ��ɾ��Ҳ����
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
	
	// ������ڵ�
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
	// ����ָ��
	int32 iIndex = nID - IDM_IOVIEWKLINE_OFTENINDEX_BEGIN;

	if ( iIndex < 0 || iIndex >= m_FormulaNames.GetSize() )
	{
		return;
	}

	CString StrName = m_FormulaNames.GetAt(iIndex);

	// ������ڵ�
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
		return false;	// �޴�ָ��
	}

	if ( NULL == m_pMerchXml )
	{
		bChangeStockByIndex = true;	// ����Ʒ���л�
	}
	
	{
		if ( CheckFlag(pFormulaContent->flag, CFormularContent::KAllowMain) )
		{
			if ( bDelRegionAllIndex )
			{
				ClearRegionIndex(m_pRegionMain, true);	// �����ͼָ��
			}
			else
			{
				
			}
			ChangeIndexToMainRegion(StrIndexName);
		}
		else if ( CheckFlag(pFormulaContent->flag, CFormularContent::KAllowSub) )
		{
			// ����ض���ͼ - ��ͼ������ָ����Ҫ���������ָ�궼���滻����
			{
				AddIndexToSubRegion(StrIndexName, true, m_iLastPickSubRegionIndex);	// û�з�����
			}
		}
	}

	return true;
}

void CIoViewKLineArbitrage::ReplaceIndex( const CString &StrOldIndex, const CString &StrNewIndex, bool32 bDelRegionAllIndex/* = false*/, bool32 bChangeStock/* = false*/ )
{
	// ָ���Ƿ���Ч
	const CFormularContent *pFormulaContentOld = CFormulaLib::instance()->GetFomular(StrOldIndex);
	if ( NULL == pFormulaContentOld )
	{
		return;	// �޴�ָ��
	}
	const CFormularContent *pFormulaContentNew = CFormulaLib::instance()->GetFomular(StrNewIndex);
	if ( NULL == pFormulaContentNew )
	{
		return;
	}
	
	{
		int iMainOld = CheckFlag(pFormulaContentOld->flag, CFormularContent::KAllowMain) ? 1 : 0;
		int iMainNew = CheckFlag(pFormulaContentNew->flag, CFormularContent::KAllowMain) ? 1 : 0;
		
		ASSERT( iMainNew == iMainOld );	// �ϸ�����Ӧ����ȣ������û������޸����ͣ���������������Ȼ��һ�����滻
		
		if ( iMainNew != 0 )
		{
			AddShowIndex(StrNewIndex, bDelRegionAllIndex, bChangeStock);
		}
		else
		{
			// ��ͼ���滻���и�ͼ��ָ��
			// �ҳ�ԭָ������ĸ�region	
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
				return;	// ��ԭ����ָ�꣬�ǾͿ��Բ�������
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
	
	// �õ�����ӵ�����ָ������
	CFormularContent *pFormulaContent = (CFormularContent *)CFormulaLib::instance()->GetFomular(StrIndexName);
	if (NULL != pFormulaContent && pFormulaContent->BePassedXday())
	{
		MessageBox(L"ָ���Ѿ�����!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return;
	}
	
	//
	if ( NULL == pFormulaContent || !CheckFlag(pFormulaContent->flag, CFormularContent::KAllowMain) )
	{
		// ��Ӧ�ж�����Ϊnormal TODO
		return;	// �޴�ָ������ͼָ�꣬ ������
	}
	
	bool32 bExist = false;
	// ����Ƿ���ڸ�index - ��ô���ж��أ���
	for ( int32 i=0; i < pData->aIndexs.GetSize() ; i++ )
	{
		// ����ʵ�ʴ���indexҲ�Ǹ���name��������so���ж�����+region
		if ( pData->aIndexs[i]->strIndexName == StrIndexName && m_pRegionMain == pData->aIndexs[i]->pRegion )
		{
			bExist = true;
			break;
		}
	}
	
	// 	m_pRegionPick = m_pRegionMain;	// delcurrent��Ҫ
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
		// ��ô��ȣ���
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
		return;	// �޴�ָ���Ǹ�ͼָ�꣬ ������
	}
	
	// �������Ҫ����ظ���ͼ�������Ƿ��Ѿ����ڸø�ͼָ��
	if ( !bAddSameIfExist )
	{
		for ( i=0; i < pData->aIndexs.GetSize() ; i++ )	// ʹ�õ��Ǹ�ͼָ���ж�
		{
			if ( NULL == pData->aIndexs.GetAt(i)->pRegion || pData->aIndexs.GetAt(i)->pRegion == m_pRegionMain)
			{
				continue;
			}
			if ( pData->aIndexs[i]->strIndexName == StrIndexName )
			{
				return;	// �Ѿ����ڸø�ͼ, ����Ҫ���������
			}
		}
	}
	

	CChartRegion *pSubRegion = NULL; // ���ڲ����ڴ˹����б��subregion������Ӧ������
	if ( iSubRegionIndex >= 0 && iSubRegionIndex <= m_SubRegions.GetSize()-1 )
	{
		pSubRegion = m_SubRegions[iSubRegionIndex].m_pSubRegionMain;
	}
	else
	{
		// �����ڣ���β�����һ��SubRegion
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
	
	// ɾ����ǰSubRegion��ָ��
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
			if ( NULL != pParam && pParam->strIndexName == StrIndexName )	// ���������ָ��
			{
				CArray<CChartCurve*,CChartCurve*> Curves;
				FindCurvesByIndexParam(pParam,Curves);	
				while ( Curves.GetSize() > 0 )
				{
					CChartCurve *pCurve = Curves.GetAt(0);
					Curves.RemoveAt(0);
					pCurve->GetChartRegion().RemoveCurve(pCurve);
				}
				
				DeleteIndexParamData ( pParam );	// ��ʱaIndexs��size�Ѿ����

				bRemove = true;
			}
		}
	}

	// ����ʲôָ����
	if ( bRemove )
	{
		ReDrawAysnc();
	}
}

void CIoViewKLineArbitrage::ClearRegionIndex( CChartRegion *pRegion, bool32 bDelExpertTip/* = false*/ )
{
	// ɾ�����е�ָ��
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
			if ( NULL != pParam && pParam->pRegion == pRegion )	// �����region������ָ��
			{
				CArray<CChartCurve*,CChartCurve*> Curves;
				FindCurvesByIndexParam(pParam,Curves);	
				while ( Curves.GetSize() > 0 )
				{
					CChartCurve *pCurve = Curves.GetAt(0);
					Curves.RemoveAt(0);
					pCurve->GetChartRegion().RemoveCurve(pCurve);
				}
				
				DeleteIndexParamData ( pParam );	// ��ʱaIndexs��size�Ѿ����
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
		// ֻ��Ҫ�������ľͿ�����

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
		// ��������
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
        // ֻ��Ҫ�������ľͿ�����

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

        //��������
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
		// �����۲�����
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
		// �������ã�û��ʵ��
		ASSERT( 0 );
	}
	else if ( IDM_ARBTTRAGE_SELECT == nID )
	{
		// ѡ��������TODO
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
	
// 	if ( NULL == m_pRegionPick)		// xl ����TBWnd�ϵĵ����˵�ʹm_pRegionPickΪNull��ʵ�����ֻ��ĳЩѡ����Ҫ�ж�
// 	{								//    
// 		return 0;
// 	}	
	
	if ( 0 == uID)
	{
		m_pRegionPick = m_pRegionMain;		// ���ڲ˵�
		
		CNewMenu menu;
		menu.LoadMenu(IDR_MENU_KLINE);
		menu.LoadToolBar(g_awToolBarIconIDs);

		CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));

		CMenu* pTempMenu = pPopMenu->GetSubMenu(_T("ѡ������"));
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
		// ָ��˵�
		int32 iIndexNums = 0;

		m_pRegionPick = m_pRegionMain;	
		m_FormulaNames.RemoveAll();
		CNewMenu menu;
		menu.LoadMenu(IDR_MENU_KLINE);
		menu.LoadToolBar(g_awToolBarIconIDs);

		CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(0));

		CMenu* pTempMenu = pPopMenu->GetSubMenu(_T("����ָ��"));
		ASSERT( NULL != pTempMenu);
		CNewMenu* pSubMenu = DYNAMIC_DOWNCAST(CNewMenu,pTempMenu);

		pSubMenu->RemoveMenu(0,MF_BYPOSITION|MF_SEPARATOR);//ɾ��β��
		

		iIndexNums = LoadOftenIndex(pSubMenu);

		if ( 0 == iIndexNums )
		{
			MessageBox(L"��ǰ�޿���ָ��,����ָ�깫ʽ������!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
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
			StrText.Format(_T("ʱ: %s \n��: %s\n��: %s\n��: %s\n"),
				StrTime.GetBuffer(),
				Float2String(pNodeData->m_fOpen,iSaveDec,true).GetBuffer(),
				Float2String(pNodeData->m_fClose,iSaveDec,true).GetBuffer(),
				Float2String(pNodeData->m_fAvg,iSaveDec,true).GetBuffer()
				);
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

void CIoViewKLineArbitrage::MenuAddRegion()
{
	if ( !AddSubRegion(false))
	{
		return;
	}

	// �õ�����Ʒ����
	T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
	if ( NULL == pData || NULL == m_pMerchXml )
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

	m_iLastPickSubRegionIndex = iSize-1;	// �µ�regionΪ�۽���ͼ

	E_IndexStockType eIST = EIST_None;

	// �õ�����ӵ�����ָ������
	uint32 uiFlag = NodeTimeIntervalToFlag( pData->m_eTimeIntervalFull);	
	AddFlag(uiFlag,CFormularContent::KAllowSub);

	m_FormulaNames.RemoveAll();
	CFormulaLib::instance()->GetAllowNames(uiFlag,m_FormulaNames, eIST);
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
	// ɾ�����е�ָ��
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
	// ͬʱ����ɾ������ӣ�so��Ҫ�䶯regionpick
	if ( m_pRegionPick == m_pRegionMain || NULL == m_pRegionPick )
	{
		// ��Ϊ��Ӧ�ĸ�ͼ
		if ( m_iLastPickSubRegionIndex >= 0 && m_iLastPickSubRegionIndex < m_SubRegions.GetSize() )
		{
			m_pRegionPick = m_SubRegions[m_iLastPickSubRegionIndex].m_pSubRegionMain;
		}
		else
		{
			ASSERT( 0 );
			return;	// û��ѡ�еĸ�ͼ
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
	// ����ѡ�е�Ϊ��һ��region
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
		DelSubRegion(&cRegion);	// ɾ����region
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
			// ��¼�µ�ǰ���K��
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

	//��ֵ
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

	// �������鴰�ڵ���ʾ
	AdjustCtrlFloatContent();

	// xl 0001762 ������еĿո���ʾ������������ʾ���ȣ����鴰�ڵĿ�ȱ�YLeft�ܶ���ʾһ������ - -
	int32 iYLeftShowChar = GetYLeftShowCharWidth();
	iYLeftShowChar += 1;
	// 
	CString StrValue;
	CGridCellSymbol* pCellSymbol;
	CGridCellSys   * pCellSys;
	// ʱ��
	pCellSys = (CGridCellSys *)pGridCtrl->GetCell(0,0);
	pCellSys->SetText(StrTimeLine1 + _T(""));

	pCellSys = (CGridCellSys *)pGridCtrl->GetCell(1,0);
	pCellSys->SetText(StrTimeLine2 + _T(""));
	
	//��ֵ
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(3,0);	
	//StrValue = Float2SymbolString(fCursorValue, fCursorValue, iSaveDec) + _T(" ");
	Float2SymbolStringLimitMaxLength(StrValue, fCursorValue, fCursorValue, iSaveDec, iYLeftShowChar, true, false, true, false, true);
	pCellSymbol->SetText(StrValue);

	//���̼�
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(5,0);
	//StrValue = Float2SymbolString(KLine.m_fPriceOpen, fPricePrevClose, iSaveDec) + _T(" ");
	Float2SymbolStringLimitMaxLength(StrValue, kLine.m_fPriceOpen, fPricePrevClose, iSaveDec, iYLeftShowChar, true, false, true, false, true);
	pCellSymbol->SetText(StrValue);

	//���̼�
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(7,0);
	//StrValue = Float2SymbolString(KLine.m_fPriceHigh, fPricePrevClose, iSaveDec) + _T(" ");
	Float2SymbolStringLimitMaxLength(StrValue, kLine.m_fPriceClose, fPricePrevClose, iSaveDec, iYLeftShowChar, true, false, true, false, true);
	pCellSymbol->SetText(StrValue);

	//����
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
	
	// xl 0001762 �����Ҫ�ض�
	if ( bNeedTerminate )
	{
		CString StrValue;
		// ������α���õģ�����Ĭ�ϲ������true
		Float2StringLimitMaxLength(StrValue, fValue, iSaveDec, GetYLeftShowCharWidth(0), true, true, bZeroAsHLine);
		return StrValue;
	}
	return Float2String(fValue,iSaveDec,false,bZeroAsHLine);
}

void CIoViewKLineArbitrage::OnCalcXAxis(CChartRegion* pRegion, CDC* pDC, CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aXAxisDivide)
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
		// CalcMainRegionXAxis(aAxisNodes, aXAxisDivide);	

		// ���ƣ�Ӧ�ý�X��㵥�����������һ����
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

	//DWORD dwE = timeGetTime();
	//CString StrTrace;
	//StrTrace.Format(L"%s   CalcYAxis ��ʱ: %d ms \r\n", pRegion->m_StrName.GetBuffer(), (dwE - dwB));
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

			// xl 0607 ��ͼС��һ��ֵ����Ҫ��
			if ( RectMain.Height() >= 8 )
			{
				// �������� - �ӻ������
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

			// ��������
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
					else  // ��������PosPixelλ�üӻ�С���ߣ������һ�����Բ���
					{
						rct.left	= pAxisDivide[i].m_iPosPixel - iWidthSpace;
						rct.right	= pAxisDivide[i].m_iPosPixel + (SizeText.cx + iWidthSpace);
					}

					// �ӿ��пռ��в��ң� �����Ƿ������ʾ
					int32 iPosFindInSpaceRects = -1;

					CRect *pRectSpace = (CRect *)aRectSpace.GetData();
					for (int32 iIndexSpace = 0; iIndexSpace < aRectSpace.GetSize(); iIndexSpace++)
					{
						if (rct.left < pRectSpace[iIndexSpace].left || rct.right > pRectSpace[iIndexSpace].right)
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
						// �����߿�
						pDC->MoveTo(pAxisDivide[i].m_iPosPixel, rct.top);
						int32 iBottom = rct.CenterPoint().y;
						if ( pAxisDivide[i].m_eLevelLine == CAxisDivide::EDLLevel1 ) // ò��������k������ߵ�
						{
							if ( bFirstLevelTop )		// ��һ�������ʾ�߲�������
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
						rct.left = pAxisDivide[i].m_iPosPixel + 1; // �����Ǹ�С���ߣ�������������Ϊ�����
						pDC->DrawText(pAxisDivide[i].m_DivideText1.m_StrText, &rct, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

						// ��ֿ�������
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
		else	// ��ͼ
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
					
					// ��������
					// xl 0607 ��ͼС��һ���߶ȣ���������
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
						// ѡ�и�ͼ��־
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

		//����ָ������ˮƽ��
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
	// �������λ������Ҫ��ʾ����k�ߵģ����Բ�������
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

		// �и�ͼ��
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
						bAddIndex = false;	// ��ָ����
						break;
					}
				}
			}
		}
		if ( bAddIndex )
		{
			// ���û�и�ͼ�Ͳ�Ҫ�����
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
		
		// �Զ�������ֵ:
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

	UpdateAxisSize(false);	// ��������Ԥ��λ��

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

	// ������region������ֵ�� �Գ�����
	{
		if (NULL != m_pRegionMain)
		{
			bool32 bValidCurveYMinMax = false;
			float fCurveYMin = 0., fCurveYMax = 0.;
			int32 i = 0;

			// ����Ƿ���ͨ�������꣬��Ҫ�ȼ���baseֵ, ������, ��Ϊ���±߽�ֵ������baseֵ
			float fBase = CChartCurve::GetInvalidPriceBaseYValue();
			// ����
			CChartCurve *pDepCurve = m_pRegionMain->GetDependentCurve();
			if ( NULL == pDepCurve )
			{
				return;
			}

			CPriceToAxisYObject::E_AxisYType eType = GetMainCurveAxisYType();
			pDepCurve->SetAxisYType(eType);	// ����������������
			if ( CalcKLinePriceBaseValue(pDepCurve, fBase) )
			{
				ASSERT( eType == CPriceToAxisYObject::EAYT_Normal || fBase != CChartCurve::GetInvalidPriceBaseYValue() );
				// �������е���region�е�curve�Ļ���ֵ
				if ( eType == CPriceToAxisYObject::EAYT_Normal || fBase != CChartCurve::GetInvalidPriceBaseYValue() )
				{
					if ( eType == CPriceToAxisYObject::EAYT_Normal && fBase != CChartCurve::GetInvalidPriceBaseYValue() && EACST_TowLegCmp == m_eArbitrageShow )
					{
						eType = CPriceToAxisYObject::EAYT_Pecent;	// ��������ʱ���л����ȼ۵İٷֱ�����
					}
					for (i = 0; i < m_pRegionMain->m_Curves.GetSize(); i++)
					{
						CChartCurve *pCurve = m_pRegionMain->m_Curves[i];
						if ( NULL != pCurve )
						{
							// index��maincurveһ���Ļ���ֵ
							// ���ӵ���Ҫ��������
							pCurve->SetAxisYType(eType);	// ����
							if ( m_pCuveOtherArbLeg == pCurve )
							{
								// �ڶ�����
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
									// ��������Ч���ݣ���
									pCurve->SetPriceBaseY(CChartCurve::GetInvalidPriceBaseYValue(), true);	// ��Ч��
								}
							}
							else
							{
								pCurve->SetPriceBaseY(fBase, true);	// ʹ�����ߵ�
							}
						}
					}
					
					// �����������ߵ����±߽�ֵ
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
			// �������ߵĻ���ֵ�ɹ�����ʧ��

			// �������е���region�е�curve��������ֵ
			for (i = 0; i < m_pRegionMain->m_Curves.GetSize(); i++)
			{				
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
	else if (ENTIMinute30 == eTimeInterval || ENTIMinute60 == eTimeInterval|| ENTIMinute180 == eTimeInterval|| ENTIMinute240 == eTimeInterval)
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

	// 4.000 Ȼ��Ŵ� 5.999 ʱ������һ���ϲ� - ��ȥ��ȡ�� xl 0601
	//if (fPixelWidthPerUnit >= 5.)
	//	fPixelWidthPerUnit = (int)(fPixelWidthPerUnit);
	// ���Ҫ�������Ķ��룬���ȻҪ��ʼ��ĳЩ���ݻᱻ������ʾ���߽��������ֶ���Ŀո�
	// ѡ�����ұ߳��ֶ���ո��Ա�֤������ʾ�̶�����k��, ��������������ܼ���k���ұ߷�����ϡ����ұ߿�
	// �����������Ҳ��ж���ո��ó����Ƚ�һ�£�������Էſ���ȥceil����������ԣ���floor
	if ( fPixelWidthPerUnit >= 3.0 )
	{
		fPixelWidthPerUnit = floorf(fPixelWidthPerUnit);		// ���ڼ���K�ߵĿ�ȵ�λΪ4������������3��Ϊ�жϣ�����3�Ŀ�ȶ���1
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
	CTime TimePrev;		// ��ǰ��ѭ���ڵ�һ����ʱ����
	for (int32 i = 0; i < iKLineUnitCount; i++)
	{
		CAxisNode &AxisNode = pAxisNode[iIndexNode];
		CAxisDivide &AxisDivide = pAxisDivide[iIndexDivide];
		
		/////////////////////////////////////////////////////////////////////////
		// �ڵ�
		if ( i < pMainData->m_pKLinesShow->m_aNodes.GetSize() )
		{
			// ��������㾫ȷid
			AxisNode.m_iTimeId = pNodeDatas[i].m_iID;
		}
		
		
		// �ڵ��� 
		if (0 == iIndexNode)
		{
			AxisNode.m_iStartPixel = rect.left;		// ��������߿�ճ�һ������
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
		
		// �нڵ�
		AxisNode.m_iCenterPixel = (AxisNode.m_iStartPixel + AxisNode.m_iEndPixel) / 2;
		iIndexNode++;
		
		//////////////////////////////////////////////////////////
		// �ָ���
		if (i < pMainData->m_pKLinesShow->m_aNodes.GetSize())
		{			
			CTime Time(pNodeDatas[i].m_iID);
			if (0 == i || 
				(eDivideUnitType != EDUTNone && TimePrev.GetYear() != Time.GetYear()))	// ��һ�����꣬һ��Ҫ��ʾ
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
			else // �Կ̶ȵ�λ��ʾ����	
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

	// ��ȡ�������ϼ�����Ҫ��ֵ
	CChartCurve *pCurveDependent = m_pRegionMain->GetDependentCurve();
	if (NULL == pCurveDependent)
		return;
	
	float fYMin = 0., fYMax = 0.;
	if (!pCurveDependent->GetYMinMax(fYMin, fYMax))
		return;

	// ��ȡ��ǰ��ʾС����λ
	int32 iSaveDec = m_Arbitrage.m_iSaveDec;
	T_MerchNodeUserData* pData = GetMainData();
	if ( NULL == pData )
	{
		return;
	}

	// ����Y����������ɶ��ٸ��̶�
	CRect rect = m_pRegionMain->GetRectCurves();
	int32 iHeightText = pDC->GetTextExtent(_T("����ֵ")).cy;
	int32 iHeightSub = iHeightText + 12;

	CPriceToAxisYObject::E_AxisYType eAxisType = GetMainCurveAxisYType();

	if ( CPriceToAxisYObject::EAYT_Pecent == eAxisType )
	{
		int32 iNum = rect.Height()/2 / iHeightSub + 1;
		float fPecPerDivide = (fYMax - fYMin)/iNum; // ÿ���̶ȿ��Դ���İٷֱ�, ����ȡ����1%
		int32 iPecPer = (int32)(fPecPerDivide*100+0.5);
		iPecPer = max(1, iPecPer);	// ����Ϊ1%
		fPecPerDivide = iPecPer/100.0f;

		aYAxisDivide.SetSize(0, iNum);
		
		// �ӻ���0%��ʼ
		// ���ϵİٷֱ�, ����Ӧ�ò���
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
			AxisDivide.m_DivideText1.m_lTextColor	= m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
			AxisDivide.m_DivideText2.m_lTextColor	= m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);

			aYAxisDivide.Add(AxisDivide);
		}

		// ����
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
			AxisDivide.m_DivideText1.m_lTextColor	= m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
			AxisDivide.m_DivideText2.m_lTextColor	= m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
			
			aYAxisDivide.Add(AxisDivide);
		}
	}
	else if ( CPriceToAxisYObject::EAYT_Log == eAxisType )
	{
		// ��������һ��10%������axis�������1��1.1(0.9)
		// �����СֵΪ1.1����0.9�ı��� 
		int32 iNum = (int32)(rect.Height()/2.5f / iHeightSub) + 1;
		float fPecPerDivide = (fYMax - fYMin)/iNum; // ÿ���̶ȿ��Դ���ı�����ȡ��
		int32 iPecPer = (int32)(fPecPerDivide+0.5);
		iPecPer = max(1, iPecPer);	// ����Ϊ1���������10%
		fPecPerDivide = iPecPer;
		
		aYAxisDivide.SetSize(0, iNum);
		// �ӻ���0%��ʼ
		// ���ϵİٷֱ�, ����Ӧ�ò���
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
				StrValue = StrPrice;		// 0%�������۸�
				dwAlign |= DT_SINGLELINE;
			}
			else
			{
				StrValue += StrPrice;		// ���� 10%\r\nxx.xx
			}

			AxisDivide.m_iPosPixel = iY;
			
			// ����
			AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;		// 
			AxisDivide.m_eLineStyle = CAxisDivide::ELSDot;			// ʵ��
			
			// ����
			AxisDivide.m_eLevelText						= CAxisDivide::EDLLevel1;
			
			AxisDivide.m_DivideText1.m_StrText			= StrValue;
			AxisDivide.m_DivideText2.m_StrText			= StrValue;
			
			AxisDivide.m_DivideText1.m_uiTextAlign		= DT_VCENTER | DT_RIGHT |dwAlign;
			AxisDivide.m_DivideText2.m_uiTextAlign		= DT_VCENTER | DT_LEFT  |dwAlign;
			
			// zhangbo 20090927 #��ʱд����ɫ
			AxisDivide.m_DivideText1.m_lTextColor	= m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
			AxisDivide.m_DivideText2.m_lTextColor	= m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
			
			aYAxisDivide.Add(AxisDivide);
		}
		
		// ����
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
			StrValue += StrPrice;		// ���� 10%\r\nxx.xx

			AxisDivide.m_iPosPixel = iY;
			
			// ����
			AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;		// 
			AxisDivide.m_eLineStyle = CAxisDivide::ELSDot;			// ʵ��
			
			// ����
			AxisDivide.m_eLevelText						= CAxisDivide::EDLLevel1;
			
			AxisDivide.m_DivideText1.m_StrText			= StrValue;
			AxisDivide.m_DivideText2.m_StrText			= StrValue;
			
			AxisDivide.m_DivideText1.m_uiTextAlign		= DT_VCENTER | DT_RIGHT;
			AxisDivide.m_DivideText2.m_uiTextAlign		= DT_VCENTER | DT_LEFT;
			
			// zhangbo 20090927 #��ʱд����ɫ
			AxisDivide.m_DivideText1.m_lTextColor	= m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
			AxisDivide.m_DivideText2.m_lTextColor	= m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
			
			aYAxisDivide.Add(AxisDivide);
		}
	}
	else 
	{
		//ASSERT( CPriceToAxisYObject::EAYT_Normal == pCurveDependent->GetAxisYType() );
		// ��ͨ����������
		int32 iNum = rect.Height() / iHeightSub + 1;
		if (iNum > 3)	iNum = 3;
		if (iNum < 2)	iNum = 2;
			
		// ���㸡�����̶ȳ���.
		float fHeightSub = (fYMax - fYMin) / iNum;
		int32 y;
		float fStart = fYMin;
		int32 iCount = 0;
		aYAxisDivide.SetSize(iNum + 1);
		CAxisDivide *pAxisDivide = (CAxisDivide *)aYAxisDivide.GetData();

		// �Ƿ�������������
		bool32 bTran = true;

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
				
				int32 i = 0;
				for ( i = 0 ; i <= iNum; i++)
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
				
				// xl 0001762 ������Y���ִ����� - �Ҳ���һ���ո�
				//CString StrValue	= Float2String(fStart, iSaveDec, true, false, false);
				CString StrValue;
				Float2StringLimitMaxLength(StrValue, fPrice, iSaveDec, GetYLeftShowCharWidth() -1, true, false, false, false);
				
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
	
	// ����Y����������ɶ��ٸ��̶�
	CRect rect = pChartRegion->GetRectCurves();
	int32 iHeightText = pDC->GetTextExtent(_T("����ֵ")).cy;
	int32 iHeightSub  = iHeightText + 12;
	int32 iNum = rect.Height()/iHeightSub + 1;

	if (iNum > 3)	iNum = 3;
	if (iNum < 2)	iNum = 2;
	
	// ��ȡ��ǰ��ʾС����λ
	int32 iSaveDec = m_Arbitrage.m_iSaveDec;
	T_MerchNodeUserData* pData = GetMainData();
	if ( NULL == pData )
	{
		return;
	}
	
	// ��ȡ�������ϼ�����Ҫ��ֵ
	CChartCurve *pCurveDependent = pChartRegion->GetDependentCurve();
	if (NULL == pCurveDependent)
		return;
	
	float fYMin = 0., fYMax = 0.;
	if (!pCurveDependent->GetYMinMax(fYMin, fYMax))
		return;

	// �Ƿ�ʹ�ø�ͼָ���ˮƽ��
	bool32 bUserFormularLine = false;
	
	// �ҵ������ͼ��ָ��
	CFormularContent* pFormular= NULL;
	
	if ( NULL != pData )
	{
		for ( int32 i = 0; i < pData->aIndexs.GetSize(); i++ )
		{
			T_IndexParam* pIndex = pData->aIndexs.GetAt(i);
			if ( NULL != pIndex && pIndex->pRegion == pChartRegion )
			{
				// �����ָ���Ƿ���Ҫ��ˮƽ��
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
				// ���˷Ƿ�ֵ
				continue;
			}

			//
			int32 iVal = 0;
			pCurveDependent->PriceYToRegionY(fValue, iVal);
			pChartRegion->RegionYToClient(iVal);

			//
			CAxisDivide AxisDivide;
			AxisDivide.m_iPosPixel	= iVal;
			
			// ����
			AxisDivide.m_eLevelLine = CAxisDivide::EDLLevel1;		// 
			AxisDivide.m_eLineStyle = CAxisDivide::ELSDot;			// ʵ��
			
			// ����
			AxisDivide.m_eLevelText						= CAxisDivide::EDLLevel1;
			
			//
			CString StrValue = Float2String(fValue, iSaveDec, true, false, false);

			AxisDivide.m_DivideText1.m_StrText			= StrValue;
			AxisDivide.m_DivideText2.m_StrText			= StrValue;
			
			AxisDivide.m_DivideText1.m_uiTextAlign		= DT_VCENTER | DT_RIGHT | DT_SINGLELINE;
			AxisDivide.m_DivideText2.m_uiTextAlign		= DT_VCENTER | DT_LEFT | DT_SINGLELINE;
			
			// zhangbo 20090927 #��ʱд����ɫ
			AxisDivide.m_DivideText1.m_lTextColor	= m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);
			AxisDivide.m_DivideText2.m_lTextColor	= m_pRegionMain->GetAxisTextColor();//GetIoViewColor(ESCChartAxisLine);

			//
			aYAxisDivide.Add(AxisDivide);
		}

		//
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
	bool32 bTran = true;
	
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
			
			int32 i = 0;
			for ( i = 0 ; i <= iNum; i++)
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
			
			// ָ�������ʱ������ʾС������
			if ( fStart >= 1000.0 || fStart <= -1000.0 )
			{
				iSaveDec = 0;
			}
			// xl 0001762 ���Ƹ�Y����ʾ�ַ����� - �Ҳ���һ���ո�
			//CString StrValue = Float2String(fStart, iSaveDec, true, false, false);
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
			
			// zhangbo 20090927 #��ʱд����ɫ
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
		case ENTIMinute:		// 1���� -> 5����
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
        case ENTIDay:				// ����
            {
                SetTimeInterval(*pData, ENTIMinuteUser);
            }
            break;
        case ENTIMinuteUser:		// �Զ�������
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
		case ENTIDayUser:			// �Զ�������
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

	// ��������
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
		// ����Ctrl ������һ��
		if ( NULL == m_pRegionMain->GetDependentCurve() )
		{
			return;
		}

		//
		SetAccurateZoomFlag(true);		

		// Home ��.������һ��K ��
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
		
		// ��¼�µ�ǰ���K��
		m_KLineCrossNow = KLine;
		
		OnKeyLeftRightAdjustIndex();			
		
		// ��ʾˢ��
		m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
		for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
		{
			m_SubRegions[i].m_pSubRegionMain->SetDrawFlag(CRegion::KDrawFull);
		}
		
		RedrawWindow();
		*/
		// �������һ��
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
		// ѡ����һ��ָ��
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
		// �������һ��
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
		// ѡ����һ��ָ��
		ChangeToNextIndex(false);
	}
}

void CIoViewKLineArbitrage::OnKeyLeftAndCtrl()
{
	OnKeyLeftAndCtrl(1);
}

void CIoViewKLineArbitrage::OnKeyLeftAndCtrl( int32 iRepCnt )
{
	// ȡ��ͼ��Ϣ
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
	if (pMainData->m_iShowPosInFullList <= 0)	// ��������ˣ� ����Ҫ�������ݣ� �����û������¼�����
	{
		return;
	}
	else
	{	
		// ������ͼ��ʾ����
		// ����Ҫ�ƶ�һ������
		ASSERT( m_pRegionMain->m_aXAxisNodes[0].m_fPixelWidthPerUnit > 0.0 );
		
		int32 iMove = iRepCnt;
		
		int32 iUnitPerPixel = (int32)(1 / m_pRegionMain->m_aXAxisNodes[0].m_fPixelWidthPerUnit);
		if ( iUnitPerPixel > 1 )
		{
			// 1�����������ɸ���λ
			iMove = iRepCnt * iUnitPerPixel;
		}

		// ����ƶ���������ƶ����ͻ���ɵ����Ҳ�ո�ľ���
		
		int32 iShowPosInFullList = pMainData->m_iShowPosInFullList - iMove;
		int32 iShowCountInFullList = pMainData->m_iShowCountInFullList;
		// ��������ߵ�����
		if ( iShowPosInFullList < 0 )
		{
			iShowPosInFullList	=	0;
		}
		
		if (!ChangeMainDataShowData(*pMainData, iShowPosInFullList, iShowCountInFullList, bIsShowNewestOld, TimeStart, TimeEnd))
		{
			//ASSERT(0);
			return;
		}

		// ��ʾˢ��
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
	// ȡ��ͼ��Ϣ
	OnKeyRightAndCtrl(1);
}

void CIoViewKLineArbitrage::OnKeyRightAndCtrl( int32 iRepCnt )
{
	// ȡ��ͼ��Ϣ
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
	if (pMainData->m_iShowPosInFullList + 1 + pMainData->m_iShowCountInFullList - 1 >= pMainData->m_aKLinesFull.GetSize())	// �����ұ��ˣ� ����Ҫ�������ݣ� �����û������¼�����
	{
		return;
	}
	else
	{
		// ������ͼ��ʾ����
		// ����Ҫ�ƶ�һ������
		ASSERT( m_pRegionMain->m_aXAxisNodes[0].m_fPixelWidthPerUnit > 0.0 );
		
		int32 iMove = iRepCnt;

		int32 iUnitPerPixel = (int32)(1 / m_pRegionMain->m_aXAxisNodes[0].m_fPixelWidthPerUnit);
		if ( iUnitPerPixel > 1 )
		{
			// 1�����������ɸ���λ
			iMove = iRepCnt * iUnitPerPixel;
		}

		int32 iShowPosInFullList = pMainData->m_iShowPosInFullList + iMove;
		int32 iShowCountInFullList = pMainData->m_iShowCountInFullList;
		// �������ұߵ�����
		if ( iShowCountInFullList + iShowPosInFullList > pMainData->m_aKLinesFull.GetSize() )
		{
			iShowPosInFullList	=	pMainData->m_aKLinesFull.GetSize() - iShowCountInFullList;
		}

		if (!ChangeMainDataShowData(*pMainData, iShowPosInFullList, iShowCountInFullList, bIsShowNewestOld, TimeStart, TimeEnd))
		{
			//ASSERT(0);
			return;
		}
		
		// ��ʾˢ��
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
			
			// ������������:
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

	// ��ѡ�Ŵ�:
	int32 iNodeCounts		= iNodeEnd - iNodeBegin + 1;
	m_iNodeCountPerScreen	= iNodeCounts + GetSpaceRightCount();

	CGmtTime TimeStart,TimeEnd;
	bool32 bIsShowNewestOld = IsShowNewestKLine(TimeStart, TimeEnd);
	if (!ChangeMainDataShowData(*pMainData, iNodeBegin, iNodeCounts, bIsShowNewestOld, TimeStart, TimeEnd))
	{
		return;
	}

	// ��ʾˢ��
 	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
 	
 	for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
 	{
 		m_SubRegions[i].m_pSubRegionMain->SetDrawFlag(CRegion::KDrawFull);
 	}
 	
 	// �ػ���ͼ
 	ReDrawAysnc();
}

bool32 CIoViewKLineArbitrage::OnZoomOut()
{
	// ��С��ʾ��λ�� ��ʾ��һЩK��
	if ( m_MerchParamArray.GetSize() <= 0 )
		return false;

	T_MerchNodeUserData *pMainData = m_MerchParamArray[0];
	if (NULL == pMainData)
		return false;

	// ��ǰ�ܹ���K ��������û��������Ӧ���еĶ�, ˵�����ݲ���. ��Ҫ����С��.
	if (  m_iNodeCountPerScreen >= KMaxKLineNumPerScreen )
	{
		return false;
	}

	// ��ǰ������Ʒ��ԭʼK������С��Ҫ������ݣ��򲻱���������
	{
		E_NodeTimeInterval eNodeTimeIntervalCompare = pMainData->m_eTimeIntervalFull;
		E_KLineTypeBase	   eKLineTypeCompare		= EKTB5Min;
		int32 iScale = 1;
		if (!GetTimeIntervalInfo(pMainData->m_eTimeIntervalFull, eNodeTimeIntervalCompare, eKLineTypeCompare, iScale))
		{
			// ��Ӧ�ó����������
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
					TRACE(_T("���������ڵȴ����ݷ��أ����ԷŴ�: %s %d,%d->%d\r\n"), pData->m_pMerchNode->m_MerchInfo.m_StrMerchCnName.GetBuffer(), m_iNodeCountPerScreen, iNeedCount, pData->m_aKLinesCompare.GetSize());
					return false;	// ʵ�����ݻ�û�л����ˣ���������ηŴ�
				}
			}
		}
	}
	
	
	int32 iCurShowCountPerScreen = m_iNodeCountPerScreen;
	int32 iBakCurShowCountPerScreen = m_iNodeCountPerScreen;

	iCurShowCountPerScreen = (int32)(iCurShowCountPerScreen / KHZoomFactor + 0.5);

	// һ������� 500 ��
  	if ( iCurShowCountPerScreen - iBakCurShowCountPerScreen > 500 )
  	{
  		iCurShowCountPerScreen = iBakCurShowCountPerScreen + 500;
  	}
	
	//
	if (iCurShowCountPerScreen > KMaxKLineNumPerScreen)
		iCurShowCountPerScreen = KMaxKLineNumPerScreen;

	if (iBakCurShowCountPerScreen == iCurShowCountPerScreen)	// û�б仯�� ����Ҫ����
		return false;

	// ����ʮ�ֹ��
	LockCrossInfo(true);

	// 
	m_iNodeCountPerScreen = iCurShowCountPerScreen;

	// �жϵ�ǰ��ʾ�� �Ƿ���ʾ���¼�
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
			// ��֤����
			m_iNodeCountPerScreen += 1;
		}

		// �Ե�ǰʮ�ֹ������ʶ�Ľڵ�Ϊ���ķŴ�:		
		GetCrossKLine(KLineNow);
		
		// K �߸���
		int32 iSizeKLine = pMainData->m_aKLinesFull.GetSize();
		
		// �ҵ����K �����ڵ�λ��
		int32 iKLinePos = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pMainData->m_aKLinesFull, KLineNow.m_TimeCurrent);

		// ������ʾ����
DO_CALC_SHOW_RANGE_ACTIVE_CROSS:
		{
			int32 iMaxShowCount = m_iNodeCountPerScreen - iSpaceRightCount;

			if (bShowNewestKLine)	// ������ʾ���¼�
			{				
				if (iMaxShowCount <= 0)
				{
					//ASSERT(0);
					iMaxShowCount = 1;
				}
				
				if (pMainData->m_aKLinesFull.GetSize() <= 0)
					return true;				// û������Ҫ��ʾ
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
				// �µ���ʼ����ֹλ��
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
		// ������ʾ����
	DO_CALC_SHOW_RANGE:
		{
			if (bShowNewestKLine)	// ������ʾ���¼�
			{
				int32 iMaxShowCount = m_iNodeCountPerScreen - iSpaceRightCount;
				if (iMaxShowCount <= 0)
				{
					//ASSERT(0);
					iMaxShowCount = 1;
				}
				
				if (pMainData->m_aKLinesFull.GetSize() <= 0)
					return true;				// û������Ҫ��ʾ
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
					if (iMaxShowCountInFullList > m_iNodeCountPerScreen - iSpaceRightCount)	// �㹻��ʾ
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
	
	// ��ʾˢ��
	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	for ( int32 i = 0; i < m_SubRegions.GetSize(); i++)
	{
		m_SubRegions[i].m_pSubRegionMain->SetDrawFlag(CRegion::KDrawFull);
	}

	// ���Ƿ��б�Ҫ�����������
	RequestViewData();

	//
	UpdateMainMerchKLine(*pMainData);

	RedrawWindow();

	return true;
}

bool32 CIoViewKLineArbitrage::OnZoomIn()
{
	// ��ʾ��λ�Ŵ� ��ʾ��һЩK��
	if (m_MerchParamArray.GetSize() <= 0)
		return false;
	
	T_MerchNodeUserData *pMainData = m_MerchParamArray[0];
	if (NULL == pMainData)
		return false;

	// ��Ҫ�� m_iNodeCountPerScreen ���ֵ, ������С����̫������, ��ʵ�ʵĵ�ǰ��ʾ��K ����ֵ
	int32 iCurShowCountPerScreen = pMainData->m_pKLinesShow->GetSize();
	int32 iBakCurShowCountPerScreen = iCurShowCountPerScreen;
	
	iCurShowCountPerScreen = (int32)(iCurShowCountPerScreen * KHZoomFactor + 0.5);
	if (iCurShowCountPerScreen < KMinKLineNumPerScreen)
		iCurShowCountPerScreen = KMinKLineNumPerScreen;
	
	if (iBakCurShowCountPerScreen == iCurShowCountPerScreen)	// û�б仯�� ����Ҫ����
		return false;
	
	// ����ʮ�ֹ��
	LockCrossInfo(true);

	// zhangbo 20090715 #���Ż��� ���������Ƶķ���
	m_iNodeCountPerScreen = iCurShowCountPerScreen;
	
	// �жϵ�ǰ��ʾ�� �Ƿ���ʾ���¼�
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
			// ��֤����
			m_iNodeCountPerScreen += 1;
		}

		// �Ե�ǰʮ�ֹ������ʶ�Ľڵ�Ϊ���ķŴ�:
		GetCrossKLine(KLineNow);

		// K �߸���
		int32 iSizeKLine = pMainData->m_aKLinesFull.GetSize();
		
		// �ҵ����K �����ڵ�λ��
		int32 iKLinePos = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pMainData->m_aKLinesFull, KLineNow.m_TimeCurrent);

		// �µ���ʼ����ֹλ��
		int32 iStartPos = iKLinePos - (m_iNodeCountPerScreen / 2);
		int32 iEndPos	= iKLinePos + (m_iNodeCountPerScreen / 2);

		//
		if ( iStartPos < 0 )
		{
			// ��ʱ��Ӧ�ó��ִ����
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
		// TRACE(L"��ǰK������: %d ��Ļ��K����: %d ���е�K ����:%d[%d] ��ʾ�ĵ�һ����:%d ��ʾ�����һ����:%d\n", iSizeKLine, m_iNodeCountPerScreen, iKLinePos, KLineNow.m_TimeCurrent, iStartPos, iEndPos);	
	}
	else
	{
		// ������ʾ����
DO_CALC_SHOW_RANGE:
		{
			if (bShowNewestKLine)	// ������ʾ���¼�
			{
				int32 iMaxShowCount = m_iNodeCountPerScreen - iSpaceRightCount;
				if (iMaxShowCount <= 0)
				{
					//ASSERT(0);
					iMaxShowCount = 1;
				}
				
				if (pMainData->m_aKLinesFull.GetSize() <= 0)
					return true;				// û������Ҫ��ʾ
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
					if (iMaxShowCountInFullList > m_iNodeCountPerScreen - iSpaceRightCount)	// �㹻��ʾ
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
	
	// ��ʾˢ��
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
	// ��ȡ�����Ʒ�����ڵ���Ϣ
	E_NodeTimeInterval eNodeTimeIntervalCompare = pMainData->m_eTimeIntervalFull;
	E_KLineTypeBase	   eKLineTypeCompare		= EKTB5Min;
	int32 iScale = 1;
	if (!GetTimeIntervalInfo(pMainData->m_eTimeIntervalFull, eNodeTimeIntervalCompare, eKLineTypeCompare, iScale))
	{
		// ��Ӧ�ó����������
		return;
	}

	// ������main��Ʒ���ݵ�����ֻ����ʵ�ʵ���Ʒ����
	// �����Ǻϳɵ����ݣ����������Ҫ���㵱ǰҪ��ʾ�����ĵ���Ҫ�������ݣ�ֻ�ܲ���
	for (int32 i = 1; i < m_MerchParamArray.GetSize(); i++)
	{
		//�����Ƿ�Ӧ���������� - �������Ƚϵ�λʹ�û����� K�� ���ڵ�λ
		T_MerchNodeUserData *pMerchData = m_MerchParamArray[i];
		if (NULL == pMerchData || NULL == pMerchData->m_pMerchNode)
			continue;

		CGmtTime TimeSpecify		= m_pAbsCenterManager->GetServerTime();
		int32 iNeedKLineCountForSane = KMinRequestKLineNumber * iScale;			// ������ָ������Ҫ����С��NodeCount
		
		// ����û�а취Ԥ֪��Ҫ����������ݣ�����ֻ��äĿ��*2��
		int32 iTmpCount = (GetSubMerchNeedDataCount() - iSpaceRightCount) * iScale;
		int32 iNeedKLineCount = MAX(iNeedKLineCountForSane, iTmpCount);   // ѡȡ������������Ҫ�������KLine����

		if ( pMerchData->m_pMerchNode!=m_Arbitrage.m_MerchA.m_pMerch
			&& pMerchData->m_pMerchNode!=m_Arbitrage.m_MerchB.m_pMerch
			&& pMerchData->m_pMerchNode!=m_Arbitrage.m_MerchC.m_pMerch )
		{
			// ����ļ�������Ʒ����ȡһ������(�����)
			iNeedKLineCount += GetSubMerchNeedDataCount()/2;
		}
		
		//
		{
			// �����
			//TRACE(_T("-------------------------------------------����[%d]\r\n"), iNeedKLineCount);
			if (iNeedKLineCount < KMinRequestKLineNumber)
			{
				iNeedKLineCount = KMinRequestKLineNumber;
			}
			iNeedKLineCount = min(iNeedKLineCount, KMaxMinuteKLineCount);

			// ��������Ļ���������������������
			// ֻ�������һ�����ݺ�����ɶ�
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
				info.m_eKLineTypeBase	= eKLineTypeCompare;								// ʹ��ԭʼK������
				info.m_iMarketId		= pMerchData->m_pMerchNode->m_MerchInfo.m_iMarketId;
				info.m_StrMerchCode		= pMerchData->m_pMerchNode->m_MerchInfo.m_StrMerchCode;
				
				info.m_eReqTimeType		= ERTYFrontCount;
				info.m_TimeSpecify		= TimeSpecify;
				info.m_iFrontCount		= iNeedKLineCount;
			
				DoRequestViewData(info);
			}
		}
		
		// ����ʵʱ����
		CMmiReqRealtimePrice MmiReqRealtimePrice;
		MmiReqRealtimePrice.m_iMarketId		= pMerchData->m_pMerchNode->m_MerchInfo.m_iMarketId;
		MmiReqRealtimePrice.m_StrMerchCode	= pMerchData->m_pMerchNode->m_MerchInfo.m_StrMerchCode;		
		DoRequestViewData(MmiReqRealtimePrice);

	}

	// ��Ҫ��ʾ����Ϣ��so������
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
	// K �߸��µ�ʱ��,�ж����µ�K ����ԭ�������ʲô�仯:
	// ����ֵ: 0:��ͬ 1:����һ���仯 2:������һ�� 3:�����������,�д�ı仯

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
		// �ڵ�����ͬ,����ֻ������һ���ڵ�.��ʱ��Ҫ��ϸ�ж�,���ܸ����˶���ڵ��������ͬ,ֱ�ӷ���3;

		if ( iSizeAfter == iSizeBefore + 1)
		{
			// ����һ���ڵ�.�ж�һ���ǲ��ǳ������һ���ڵ�����,�����Ķ���ͬ,����ǵĻ�,�Ǿ��� 2

			KLineAfter.RemoveAt(iSizeAfter-1);
			pAfter  = (char*)KLineAfter.GetData();

			if ( 0 == memcmp(pBefore,pAfter,iSizeBefore*sizeof(CKLine)))
			{
				// �������һ��,������һ��,����������һ��
				return 2;
			}
			else
			{
				// ��һ��.
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
		// �ڵ������ͬ:
		if ( iSizeBefore == KMaxMinuteKLineCount )
		{
			// ����, 1W ���ڵ���: ���������ݲ嵽��һ��,�����һ���ߵ�.������ After ��ǰ 0~9999(ȥβ) �� Before �� 1~10000 �Ƚ�(ȥͷ)
			
			if ( 0 == memcmp(pBefore,pAfter,iSizeBefore*sizeof(CKLine)))
			{
				// ��ȫ��ͬ
				return 0;
			}
			
			KLineBefor.RemoveAt(0);
			KLineAfter.RemoveAt(iSizeAfter-1);
			
			pBefore = (char*)KLineBefor.GetData();
			pAfter  = (char*)KLineAfter.GetData();
			
			if ( 0 == memcmp(pBefore,pAfter,(iSizeBefore - 1)*sizeof(CKLine)))
			{
				// �������һ��,������һ��,����һ�������仯
				return 1;
			}
			else
			{
				// ��һ��
				return 3;
			}
		}
		else
		{
			// �������
			if ( 0 == memcmp(pBefore,pAfter,iSizeBefore*sizeof(CKLine)))
			{
				// ��ȫ��ͬ
				return 0;
			}
			
			//���������һ��ǰ���ǲ���һ����.
			
			KLineAfter.RemoveAt(iSizeAfter-1);
			KLineBefor.RemoveAt(iSizeBefore-1);
			
			pBefore = (char*)KLineBefor.GetData();
			pAfter  = (char*)KLineAfter.GetData();
			
			if ( 0 == memcmp(pBefore,pAfter,(iSizeBefore - 1)*sizeof(CKLine)))
			{
				// �������һ��,������һ��,����һ�������仯
				return 1;
			}
			else
			{
				// ��һ��.
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
		// ���ܴ����½���K ����ͼ,û������.	
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
	else	// ��Ӧ�ó��ֵ�����
	{
		//ASSERT(0);
	}

	if ( NULL == m_pRegionMain )
	{
		// ��Ӧ����RegionMainΪ��ʱ�͵��õ��˵� �������������
		ASSERT( 0 );
		return false;
	}
	
	// ��ΪAxisNode��timeId����Ҫ��Ӧ�������
	int32	iRegionX = AxisNode.m_iCenterPixel;
	int32	timeId = AxisNode.m_iTimeId;
	if ( 0 != timeId )
	{
		// timeId��ʼ���ˣ��Ͳ���Ҫ������
	}
	else
	{
		// ����AxisNode�е�X���꣬�ҵ�timeID
		m_pRegionMain->ClientXToRegion(iRegionX);
		if( NULL == m_pRegionMain->GetDependentCurve()
			|| ! m_pRegionMain->GetDependentCurve()->RegionXToCurveID(iRegionX, timeId) )
		{
			// ��Ӧ����RegionMainΪ��ʱ�͵��õ��˵� ������������� - �п���X ID��ʧ��
			//ASSERT( 0 );
			//m_pRegionMain->GetDependentCurve()->RegionXToCurveID(iRegionX, timeId);
			return false;
		}
	}
	
	CTime Time(timeId);		// ���ǵ���ʱ����ʾ
	
	// �Ƿ�ʹ��m_StrSliderText1������������
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
	// ��ʼ��Ĭ�ϸ�ͼ����
	ASSERT( NULL != pTiXmlElement );

	// ���һ����ͼ
	const char *pszPickSubRegionIndex = pTiXmlElement->Attribute(KXMLAttriIoViewKLineArbPickSubRegionIndex);
	if ( NULL != pszPickSubRegionIndex )
	{
		m_iLastPickSubRegionIndex = atoi(pszPickSubRegionIndex);
		// ������Խ�����
	}

	// ��ǰ������ͼ������
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
			// ��ֹ��ͬ��
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

	// ����ָ��ļ���
	CStringArray aFormularNames;
	int32 iPos;
	GetOftenFormulars(pRegionActive, aFormularNames, iPos, false);
	
	if ( aFormularNames.GetSize() <= 0 )
	{
		return;			
	}
	// ��ǰ��ָ������
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

	// ����յ�ǰ��
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
	if ( bIsShowNewestNow != bPreShowNewest )	// ��������״̬����� �ϸ񽲣������ж��ǻ�����
	{
		UpdateMainMerchKLine(MainMerchNode, true);
	}
	else
	{
		// ���¼�������
		UpdateAxisSize();
	}

	return true;
}

bool32 CIoViewKLineArbitrage::CalcKLinePriceBaseValue( CChartCurve *pCurve, OUT float &fPriceBase )
{
	// �����������ʹ�������Լ��Ļ���ֵ��ָ����ʹ����������ͬ�Ļ���ֵ
	fPriceBase = CChartCurve::GetInvalidPriceBaseYValue();
	if ( NULL == pCurve )
	{
		return false;
	}
	// ʹ����ͼ���������ͣ��������ߵ��������ͼ���
	CPriceToAxisYObject::E_AxisYType eType = GetMainCurveAxisYType();
	if ( CPriceToAxisYObject::EAYT_Normal == eType )
	{
		// ��ͨ���Ͳ���Ҫ����ֵ
		// ���ﰴ�հٷֱ�ȡһ������ֵ
		CNodeData node;
		// �����ָ���ߣ��������ߵ�
		if ( CheckFlag(pCurve->m_iFlag, CChartCurve::KTypeIndex) )
		{
			ASSERT( 0 );	// ��Ӧ���ظ������
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
		// �ٷֱȣ�ȡ�ߵĵ�һ�����̼�
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

		if ( pCurve->GetValidFrontNode(0, node) )
		{
			fPriceBase = node.m_fOpen;
			return true;
		}
	}
	else if ( CPriceToAxisYObject::EAYT_Log == eType )
	{
		// ������ȡ�ߵĵ����ڶ��������̼ۻ���Ψһһ���Ŀ��̼�
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
		
		if ( pCurve->GetValidBackNode(1, node) )
		{
			fPriceBase = node.m_fClose;	// �����ڶ��������
			return true;
		}
		else if ( pCurve->GetValidBackNode(0, node) )
		{
			fPriceBase = node.m_fOpen; // Ψһһ��Ŀ���
			return true;
		}
	}
	else
	{
		ASSERT( 0 );	// ��֧������
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
			return pCurve;	// �����ҵ��ĵ�һ��������Ĳ�����
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
	// ��ɾ��������ʲô��
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
		return true;	// һģһ���������Ͳ�����
	}

	MerchArray aMerchs;
	bool32 bMerch = GetArbitrageMerchs(arb, aMerchs);
	ASSERT( bMerch );

	// ���ԭ����Ʒ�����ݣ���������ע����������
	
	// ���������Ʒdata���ؽ�data
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
	
	// ͼ�ε�������ʼ��
	// ��С�ĺ�����ʱ, �л���Ʒ��Ҫ��ס�������
	if ( m_iNodeCountPerScreen > KMinRequestKLineNumber * 1.5 )
	{
		m_iNodeCountPerScreen = (int32)(KMinRequestKLineNumber * 1.5);
	}
	
	// ��m_pMerchXmlΪNULLʱ����ʼ����ĻӦ����ʾ��Node����������Ǵ�Xml�г�ʼ����Node����Ӧ���Ѿ���ʼ�����˵�
	if ( !m_bFromXml )
	{
		if ( m_bRequestNodesByRectView )
		{
			// ���ݵ�ǰ��ͼ��С,������ʵ�K �߸���
			// ��Ϊ��ʱ���������������ټ��㻭ͼ�Ⱦ��������С.������ʱ�� m_pRegionMain->m_RectView ��0.
			// ��Ҫ�����ֵ����,��GGTongView �Ŀ��ֵ����
			
			ResetNodeScreenCount();
			
			// һ���Եı�־,����͸�λ
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
		// ������������Ϊ�����ж����Ʒ�ĸ��£�����ȫ��������ʱ����
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
		// ���ܻ�ȡ��Ʒ��Ϣ�������ʾ����
		ClearLocalData(false);

		return true;	// ���Ǽ���ɹ���:)����������ͼ������һ����ʼ�׶�~~
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
		return true; //// ���Ǽ���ɹ���:)����������ͼ������һ����ʼ�׶�~~
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
		// ����
		ASSERT( aDatas.GetSize() >= 2 );
		CArray<CKLine, CKLine> aKlineC;
		if ( aDatas[0]->m_aKLinesFull.GetSize() > 0 && aDatas[1]->m_aKLinesFull.GetSize() > 0 )
		{
			// ������Ʒ����
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
		// ��ʽ
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
	TRACE(_T("����������ʱ: %d ms\r\n"), dwTime2-dwTime);
	dwTime = dwTime2;

	if ( bOk )
	{
		// �������������
		if ( EACST_TowLegCmp == m_eArbitrageShow )
		{
			// ���Ϊ����
			pMainData->m_aKLinesFull.Copy(aKlineLegA);	// A��
		}
		else
		{
			// ���Ϊ����
		}

		// ���������ͼ�
		for ( int32 i=0; i < pMainData->m_aKLinesFull.GetSize() ; ++i )
		{
			CKLine &kline = pMainData->m_aKLinesFull[i];
			kline.m_fPriceHigh	= max(kline.m_fPriceOpen, kline.m_fPriceClose);
			kline.m_fPriceHigh	= max(kline.m_fPriceHigh, kline.m_fPriceAvg);
			kline.m_fPriceLow	= min(kline.m_fPriceOpen, kline.m_fPriceClose);
			kline.m_fPriceLow	= min(kline.m_fPriceLow, kline.m_fPriceAvg);
		}

		// �����ǰһ�εļ������������ͬ��Ʒ������㣬�򲻻��ٽ����ֻ������
		// ��ͬ��Ʒ������㣬��ֻҪ��֤����ô���Ϳ�����
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
			// ������ʾ�����������
			iShowCountInFullList = m_iNodeCountPerScreen - iRightSpace;
			iShowPosInFullList = pMainData->m_aKLinesFull.GetSize()-iShowCountInFullList;
			if ( iShowPosInFullList < 0 )
			{
				iShowCountInFullList += iShowPosInFullList;
				iShowPosInFullList = 0;
			}
		}

		// ���㹫ʽ��
		//********************************************************************************************************************************************
		// ���µ�ָ��
		for (int32 j = 0; j < pMainData->aIndexs.GetSize(); j++)
		{
			T_IndexParam* pIndex = pMainData->aIndexs.GetAt(j);
			bool32 b = g_formula_compute(this, this, (CChartRegion*)pIndex->pRegion, pMainData, pIndex, iShowPosInFullList, iShowPosInFullList + iShowCountInFullList); 				
			if ( !b )
			{
				// Դ�����ǿյģ����ԭ��������
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

					
		// ���ɵ�ǰ��ʾ������
		if (!pMainData->UpdateShowData(iShowPosInFullList, iShowCountInFullList))
		{
			////ASSERT(0);
			bOk = false;
		}

		// �����ǰΪ����ģʽ���ϳ�B������
		if ( EACST_TowLegCmp == m_eArbitrageShow )
		{
			// �Ƚ��ߵ�����
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
	TRACE(_T("��������������ʱ: %d ms\r\n"), dwTime2-dwTime);

	if ( !bOk )
	{
		ClearLocalData(false);
	}

	// �ߵĺ�׺������Ҫ������
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

	// ֻ�ڷ�main��ȡ����
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
		// Ҫ��������ڵ���
		if ( pMainData->m_pMerchNode == pMerch )
		{
			// main�е�MerchNode����������Ҫ�����ͬ������chart���г�ͻ
			pMainData->m_pMerchNode = CIoViewBase::GetNextMerch(pMerch, true);
			if ( pMainData->m_pMerchNode == pMerch )
			{
				// ������ͬ����ֻ��ѭ��ȡ��
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
				return NULL;	// ����ֻ��һ����Ʒ����Ȼ�ǱȽϲ��˵�
			}
		}
		bool32 bInherit;
		T_MerchNodeUserData *pData = NewMerchData(pMerch, false, bInherit);
		ASSERT( NULL != pData );
		if ( NULL != pData )
		{
			if ( !bInherit )
			{
				// ������Ҫ���� ����
				// �ݲ���Ҫ�ߣ����ȵ��ӵ�ʹ�ö������
			}
			
			// �����ע - ��SetArbʱ�����úù�ע����Ʒ
			
			return pData;
			// ��ʱ����û�����ñ����
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

	// �������
	pMerchData->m_aTicksCompare.RemoveAll();
	pMerchData->m_aKLinesCompare.RemoveAll();
	pMerchData->m_aKLinesFull.RemoveAll();
	
	ASSERT( NULL == pMerchData->m_pKLinesShow );
	ASSERT( 0 == pMerchData->aIndexs.GetSize() );

	// ���ò��� 
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
		// ����һ��
		ASSERT( m_MerchParamArray.GetSize() == 0 );
		bool32 bInhert;
		// ����ѡһ����Ʒ��mainData�е���Ʒ���������ͼ������
		CMerch *pMerch = NULL;
		if ( NULL != m_Arbitrage.m_MerchA.m_pMerch )
		{
			pMerch = m_Arbitrage.m_MerchA.m_pMerch;
		}
		else
		{
			pMerch = m_pAbsCenterManager->GetMerchManager().m_BreedListPtr[0]->m_MarketListPtr[0]->m_MerchsPtr[0];
			// ��������Ʒû�еĻ�����ô��ʹ���ﲻ���쳣�����Ҳ�����������
		}
		T_MerchNodeUserData* pData = NewMerchData(pMerch, true, bInhert);
		
		// ����Ĭ������
		
		if ( !bInhert )
		{
			CNodeSequence* pNodes = CreateNodes();
			pNodes->SetUserData(pData);
			pData->m_pKLinesShow = pNodes;

// 			Ĭ�ϵ������̼ۼ۲�ͼ����ͼĬ��MACD
// 				1��	���̼ۼ۲�ͼ�������̼ۼ۲�������ͼ
// 				2��	���ۼ۲�ͼ���þ��ۼ۲�������ͼ
// 				3��	����K��ͼ�������̼ۼ۲���̼ۼ۲���ۼ۲���K�ߣ����̼ۼ۲���ڿ��̼ۼ۲���ߣ���֮�����ߡ�
// 				4��	���ȵ���ͼ��������ģ���е���Ʒ�����̼۷ֱ�������ͼ��������ƷA��ʾ�۸��������Ʒ�ðٷֱ�������ʾ
			// ���ͼ������ӵ�����̼۲���۲����K�����ȵ����ߵĻ�������

			// new 1 curves/nodes/...
			CChartCurve* pCurve = m_pRegionMain->CreateCurve(CChartCurve::KDependentCurve|CChartCurve::KRequestCurve|CChartCurve::KTypeKLine|CChartCurve::KYTransformByClose|CChartCurve::KUseNodesNameAsTitle/*|CChartCurve::KDonotPick*/);		 
			CChartDrawer* pKLineDrawer = new CChartDrawer(*this, (CChartDrawer::E_ChartDrawStyle)CChartDrawer::EKDSTrendPrice);
			
			if ( -1 != m_iDrawTypeXml )
			{
				//pKLineDrawer->m_eChartDrawType = (CChartDrawer::E_ChartDrawStyle)m_iDrawTypeXml;	// ����Ҫ��xml��ȡֵ������ȫ��ֵ
				m_iDrawTypeXml = 0;
			}
			pCurve->AttatchDrawer(pKLineDrawer);
			//DEL(pKLineDrawer);
			pCurve->AttatchNodes(pNodes);
			
			// new 2 curve
			pCurve = m_pRegionXBottom->CreateCurve(CChartCurve::KDependentCurve|CChartCurve::KRequestCurve|CChartCurve::KInVisible|CChartCurve::KTypeKLine|CChartCurve::KYTransformByLowHigh);
			pCurve->AttatchNodes(pNodes);

			// ����һ�����ߵĳ�ʼ��
			ASSERT( NULL == m_pCuveOtherArbLeg );
			pNodes = CreateNodes();
			pNodes->SetUserData(pData);
			pNodes->SetName(_T("���ȵ���B"));
			pCurve = m_pRegionMain->CreateCurve(CChartCurve::KRequestCurve|CChartCurve::KTypeKLine|CChartCurve::KYTransformByClose|CChartCurve::KUseNodesNameAsTitle|CChartCurve::KYTransformToAlignDependent/*|CChartCurve::KDonotPick*/);		 
			CChartDrawer* pKLineDrawer1 = new CChartDrawer(*this, (CChartDrawer::E_ChartDrawStyle)CChartDrawer::EKDSTrendPrice);
			pKLineDrawer1->SetSingleColor(GetIoViewColor(ESCVolume2));
			pCurve->AttatchDrawer(pKLineDrawer1);
			//DEL(pKLineDrawer);
			pCurve->AttatchNodes(pNodes);
			m_pCuveOtherArbLeg = pCurve;
			m_pCuveOtherArbLeg->m_iFlag |= CChartCurve::KInVisible;	// û������ʱ����

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
						// ����һ�����ڳ�ʼ��ʱ�ͱ���һ����
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
	// ����ͼ��Ҫ���⴦����Щ
	if ( pCurve == NULL || &pCurve->GetChartRegion() != m_pRegionMain)
	{
		return false;
	}

	// �ҵ����node
	CNodeSequence * pNodes = ((CChartCurve*)pCurve)->GetNodes();
	
	if ( NULL == pNodes)
	{
		return false;
	}
	if ( pNodes->GetName().GetLength() <= 0 )
	{
		return false;	// û�����Ƶ�node���ʺ�
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

    // ��������
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
		return _T("���̼ۼ۲�");
		break;
	case EACST_AvgDiff:
		return _T("���ۼ۲�");
		break;
	case EACST_ArbKLine:
		return _T("����K��");
		break;
	case EACST_TowLegCmp:
		return _T("���ȵ���");
		break;
	}
	return _T("δ֪�۲�");
}

CString ArbEnumToString( E_Arbitrage eType )
{
	switch (eType)
	{
	case EABTwoMerch:
		return _T("��������");
		break;
	case EABThreeMerch:
		return _T("��ʽ����");
		break;
	}
	return _T("δ֪����ģ��");
}
