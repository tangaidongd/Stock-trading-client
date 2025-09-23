#include "stdafx.h"

#include <set>
#include <utility>
#include "GridCtrlSys.h"
#include "GridCellSys.h"
#include "GridCellSymbol.h"
#include "guidrawlayer.h"
#include "NewMenu.h"
#include "memdc.h"
#include "MerchManager.h"

#include "facescheme.h"
#include "IoViewKLine.h"
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

#include "IoViewChouMa.h"
#include "UserNoteInfo.h"
#include "DlgNotePad.h"
#include "dlgklineinteval.h"
#include "dlgklinequant.h"
#include "DlgChooseStockVar.h"
#include "PathFactory.h"
#include "BalloonMsg.h"
#include "dlgcolorkline.h"
#include "sharestructnews.h"
#include "XmlShare.h"
#include "XLTimerTrace.h"
#include "PluginFuncRight.h"
#include "DlgAdvertise.h"
#include "formulaengine.h"
#include "dlgformularmanager.h"
#include "DlgMenuUserCycleSet.h"
#include "DlgDialy.h"
//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const CString KStrTradeEnterLong		= L"ENTERLONG";
const CString KStrTradeExitLong			= L"EXITLONG";
const CString KStrTradeEnterShort		= L"ENTERSHORT";
const CString KStrTradeExitShort		= L"EXITSHORT";

extern const int32 KiMenuIDIntervalStatistic;
using std::set;

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/

#define  _MYARGB(a,r,g,b) \
	(((ARGB) ((a) & 0xff) << ALPHA_SHIFT)| \
	((ARGB) ((r) & 0xff) << RED_SHIFT)  | \
	((ARGB) ((g) & 0xff) << GREEN_SHIFT)| \
	((ARGB) ((b) & 0xff) << BLUE_SHIFT))

void TestKLine(CArray<CKLine,CKLine>& aKLines,const char * pFilePath,CString StrMerchCode, int32 iMarketId);
void TestNodes(CArray<CNodeData,CNodeData&>& aNodes,const char * pFile,CString StrMerchCode, int32 iMarketId);

//////////////////////////////////////////////////////////////////////////
const int32 KTimerPeriodUpdateTrendValues	 = 500;

const int32 KTimerIdUpdateSaneValues		 = 489667;

const int32 KSpaceRightCount			= 4;			// �ұ߿ճ���K������

const int32 KMinKLineNumPerScreen		= 10;			// ������ʾK������, ����KSpaceRightCountλ��
const int32 KMaxKLineNumPerScreen		= 10000;		// �����ʾK������
const int32 KDefaultKLineNumPerScreen	= 200;

const int32 KDefaultCtrlMoveSpeed		= 5;			// ��Ctrl������ʱ��Ĭ��ƽ�Ƶĵ�λ

const int32 KMinRequestKLineNumber		= 300;			// һ����������������
const int32 KRequestKLineNumberScale	= 2;			// Ĭ�ϵ�һ������K������
const int32 KDefaultKLineWidth			= 15;			// ������ͼ��С����K �ߵ�ʱ��,K �߽ڵ�Ĭ�Ͽ��

// 
const int32 KMaxMinuteKLineCount		= 10000;		// �������ķ���K�����ݣ� �������ƵĻ��� ���������������������24

const int32 KTimerIdDelayAutoAddExpCmp			 = 45536;  // �Զ����Ӵ��̵���ʱ����
const int32 KTimerPeriodDelayAutoAddExpCmp		 = 50;	// ��ʱ n ms


//
namespace
{
	const int32 KiStatisticaKLineNums		= 200;			// ͳ�Ƶ�K �߸���
}

const int32 KiGridFloatCtrlRowCountForStockCn = 22+2/*22+4*/;	// ���ڹ�Ʊ���鴰������
const int32 KiGridFloatCtrlRowCountForOther   = 22;		// ������Ʒ���鴰������

// XML
static	const char KXMLIoViewKLineClrFormularName[] = "COLOR_FOMULAR";		// ���K������
static const char KXMLAttriIoViewKLinePickSubRegionIndex[] = "PickSubRegion";	// ���ѡ��ĸ�ͼ
static const TCHAR KXMLAttriIoViewKlineKlineDrawStyle[]	   = _T("KlineDrawStyle");	// ��ͼk������

///////////////////////////////////////////////////////////////////////////////
const T_FuctionButton s_astKLineSubRegionButtons[] =
{
	//	T_FuctionButton(L"ǰ",		L"ǰһָ��",		REGIONTITLELEFTBUTTONID,	IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE),
	T_FuctionButton(L"ָ",		L"ѡ��ָ��",		REGIONTITLEFOMULARBUTTONID,	IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE),
	//	T_FuctionButton(L"��",		L"��һָ��",		REGIONTITLERIGHTBUTTONID,	IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE),

	//	T_FuctionButton(L"+",		L"���Ӹ�ͼ",		REGIONTITLEADDBUTTONID,		IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE),
	T_FuctionButton(L"��",		L"ɾ����ǰ��ͼ",	REGIONTITLEDELBUTTONID,		IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE,	IMAGEID_NOUSEIMAGE),
};

// ���ܰ�ť����
const int KiKLineSubRegionButtonNums = sizeof(s_astKLineSubRegionButtons)/sizeof(T_FuctionButton);

// ������ͨ��
bool32	CalcKlineCircAsset(CMerch *pMerch, const CGmtTime &TimeKline, OUT float &fCircAsset);


//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CIoViewKLine, CIoViewChart)

E_MainKlineDrawStyle CIoViewKLine::sm_eMainKlineDrawStyle = EMKDSNormal;

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewKLine, CIoViewChart)
	//{{AFX_MSG_MAP(CIoViewKLine)
	//ON_WM_SHOWWINDOW()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_COMMAND_RANGE(IDM_CHART_BEGIN, IDM_CHART_END, OnMenu)
	ON_COMMAND_RANGE(ID_CHART_INTERVALSTATISTIC, ID_KLINE_HISTORYTREND, OnMenu)
	ON_COMMAND_RANGE(ID_KLINE_AXISYNORMAL, ID_KLINE_AXISYLOG, OnMenu)
	ON_COMMAND_RANGE(IDM_IOVIEWKLINE_ALLINDEX_BEGIN, IDM_IOVIEWKLINE_ALLINDEX_END, OnAllIndexMenu)
	ON_COMMAND_RANGE(IDM_IOVIEWKLINE_OFTENINDEX_BEGIN, IDM_IOVIEWKLINE_OFTENINDEX_END, OnOftenIndexMenu)
	ON_COMMAND_RANGE(IDM_IOVIEWKLINE_OWNINDEX_BEGIN, IDM_IOVIEWKLINE_OWNINDEX_END, OnOwnIndexMenu)	
	ON_COMMAND_RANGE(IDM_IOVIEWKLINE_EXPERT_BEGIN, IDM_IOVIEWKLINE_EXPERT_END, OnMenuExpertTrade)
	ON_COMMAND_RANGE(IDT_SD_BEGIN, IDT_SD_END,OnMenu)	
	ON_COMMAND_RANGE(ID_INVESTMENT_DIARY, ID_INVESTMENT_DIARY, OnMenu)
	ON_COMMAND_RANGE(IDM_PRE_WEIGHT, IDM_NONE_WEIGHT, OnWeightMenu)	
	ON_COMMAND_RANGE(ID_KLINE_ADDCMPMERCH, ID_KLINE_REMOVECMPMERCH,OnMenu)
	ON_COMMAND(KiMenuIDIntervalStatistic, OnMenuIntervalStatistic)
	ON_MESSAGE(UM_IoViewTitle_Button_LButtonDown, OnMessageTitleButton)	
	ON_MESSAGE(TEMP_UM_CHOUMANOTIFY, OnNotifyChouMa)
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// 
void CIoViewKLine::OnTest()
{
	m_StrText = L"";
}

CIoViewKLine::CIoViewKLine() : CIoViewChart(),CChartRegionViewParam(NULL)
{
	m_pParent			= NULL;
	m_rectClient		= CRect(-1,-1,-1,-1);
	m_iDataServiceType  = EDSTKLine | EDSTPrice | EDSTGeneral;	// GeneralΪ�յ���Ȩ�ź�
	m_StrText			=_T("IoViewKLine");
	m_pRegionViewData	= this;
	m_pRegionDrawNotify = this;
	SetParentWnd(this);
	m_iChartType		= 1;
	m_bForceUpdate		= false;

	m_bNeedCnp					= false;
	m_bRequestNodesByRectView	= false;
	m_aAllFormulaNames.RemoveAll();

	//
	m_iCurCrossKLineIndex	= -1;
	m_iNodeCountPerScreen	= KDefaultKLineNumPerScreen;	
	memset((void*)(&m_KLineCrossNow), 0, sizeof(m_KLineCrossNow));

	m_mulmapStategyInfo.clear();

	// Ĭ����ʾ
	bool32 bHasSaneRight = CPluginFuncRight::Instance().IsUserHasSaneIndexRight(false);
	m_bShowSaneIndexNow		= bHasSaneRight ? true : false;
	m_bForceCalcSaneIndex   = false;  // ��UpdateMainKLine�Լ������Ƿ�Ҫ����

	// 
	m_pDlgTrendIndex			= NULL;
	m_fProfit					= 0.0;

	m_aSaneNodeTips.RemoveAll();

	//
	m_pIndexEMAForSane			= NULL;		

	//
	InitialImageResource();	
	ReSetSaneIndex(false);

	m_aZLMMCrossID.RemoveAll();
	m_bZLMMAlarm  = false;

	//
	m_pFormularClrKLine = NULL;
	m_pOutArrayClrKLine = NULL;

	m_pFormularTrade	= NULL;
	m_pOutArrayTrade	= NULL;

	//
	m_pExpertTrade		= NULL;

	m_iLastPickSubRegionIndex = 0;	// Ĭ�ϵ�һ����ͼ
	m_iPickSubRegionIndex = 0;

	m_RectMainCurve.SetRectEmpty();

	//
	m_aKLineInterval.RemoveAll();
	m_fPriceIntervalPreClose = 0.;

	m_eKlineDrawStyle = EMKDSNormal;
	m_bReqStrategy = true;
}

CIoViewKLine::~CIoViewKLine()
{
	DEL(m_pIndexEMAForSane);
	DEL(m_pExpertTrade);
	DEL(m_pOutArrayClrKLine);
	DEL(m_pOutArrayTrade);

	DEL(m_pImageStrongStabilityUp);			// ǿ,�ȶ�,����
	DEL(m_pImageNormalStabilityUp);			// һ��,�ȶ�,����
	DEL(m_pImageWeekStabilityUp);			// ��,�ȶ�,����

	DEL(m_pImageStrongUnStabilityUp);		// ǿ,���ȶ�,����
	DEL(m_pImageNormalUnStabilityUp);		// һ��,���ȶ�,����
	DEL(m_pImageWeekUnStabilityUp);			// ��,���ȶ�,����

	DEL(m_pImageStrongStabilityDown);		// ǿ,�ȶ�,�½�
	DEL(m_pImageNormalStabilityDown);		// һ��,�ȶ�,�½�
	DEL(m_pImageWeekStabilityDown);			// ��,�ȶ�,�½�

	DEL(m_pImageStrongUnStabilityDown);		// ǿ,���ȶ�,�½�
	DEL(m_pImageNormalUnStabilityDown);		// һ��,���ȶ�,�½�
	DEL(m_pImageWeekUnStabilityDown);		// ��,���ȶ�,�½�

	DEL(m_pImageShake);						// ������

	DEL(m_pImageDTOC);						// ��ͷ����
	DEL(m_pImageDTAC);						// ��ͷ�Ӳ�
	DEL(m_pImageDTRC);						// ��ͷ����
	DEL(m_pImageDTCC);						// ��ͷ���

	DEL(m_pImageKTOC);						// ��ͷ����
	DEL(m_pImageKTAC);						// ��ͷ�Ӳ�
	DEL(m_pImageKTRC);						// ��ͷ����
	DEL(m_pImageKTCC);						// ��ͷ���

	DEL(m_pImageDCKO);						// ����տ�
	DEL(m_pImageKCDO);						// ����࿪

	DEL(m_pImageExpert);					// ����࿪

	DEL(m_pImageZLMM);						// ��������

	DEL(m_pImageRiseFallFlag);				// �ǵ����
	//
	ReleaseTradeImage();
}	

///////////////////////////////////////////////////////////////////////////////
int CIoViewKLine::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CIoViewChart::OnCreate(lpCreateStruct) == -1)
		return -1;

	// ��ʼ����ͼ����
	CString StrValue;
	CString StrName = CIoViewManager::GetIoViewString(this);
	if ( CEtcXmlConfig::Instance().ReadEtcConfig(StrName, KXMLAttriIoViewKlineKlineDrawStyle, NULL, StrValue) )
	{
		m_eKlineDrawStyle = (E_MainKlineDrawStyle)_ttoi(StrValue);
		if ( m_eKlineDrawStyle > EMKDSCount )
		{
			m_eKlineDrawStyle = EMKDSNormal;
		}
	}

	static bool sbInit = false;
	if ( !sbInit )
	{
		sbInit = true;
		sm_eMainKlineDrawStyle = m_eKlineDrawStyle;
	}

	//
	m_bIsKLineOrTend = true;
	return 0;
}

BOOL CIoViewKLine::TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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

	// Ȩ�ؿ�ݼ�
	if ( 'V' == nChar && IsCtrlPressed() )
	{
		if ( m_eWeightType == EWTPre )
		{
			OnWeightMenu(IDM_NONE_WEIGHT);	// ��ԭ
		}
		else
		{
			OnWeightMenu(IDM_PRE_WEIGHT);	// ǰ��Ȩ
		}
		return TRUE;
	}	
	else if ( 'B' == nChar && IsCtrlPressed() )
	{
		if ( m_eWeightType == EWTAft )
		{
			OnWeightMenu(IDM_NONE_WEIGHT);	// ��ԭ
		}
		else
		{
			OnWeightMenu(IDM_AFT_WEIGHT);	// ǰ��Ȩ
		}
		return TRUE;
	}	

	if ( nChar == 'K' && IsCtrlPressed() )
	{
		OnAddClrKLine();
		return TRUE;
	}

	if ( nChar == 'H' && IsCtrlPressed() )
	{
		OnDelAllPrompt();
		return TRUE;
	}

	if ( nChar == 'E' && IsCtrlPressed() )
	{
		OnAddTrade();
		return TRUE;
	}

	return CIoViewChart::TestKeyDown(nChar,nRepCnt,nFlags);
}

void CIoViewKLine::ReSetSaneIndex(bool32 bMerchChange)
{
	if ( GetSafeHwnd() )
	{
		KillTimer(KTimerIdUpdateSaneValues);
	}

	if ( !bMerchChange )
	{
		// ��ʼ����ʱ��,������������.�л���Ʒ��ʱ�����������Ʒ�б�,�ж��Ƿ��Ѿ��������ֵ
		m_aMerchsHaveCalcLatestValues.RemoveAll();
	}

	m_bNeedCalcSaneLatestValues = false;
	m_TimeToUpdateSaneIndex		= 0xffffffff;

	m_eSaneIndexState			= ESISNONE;		

	m_iSaneTradeTimes			= 0;		
	m_fSaneAccuracyRate			= 0.0;	
	m_fSaneProfitability		= 0.0;	

	m_aSaneIndexValues.RemoveAll();
}

void CIoViewKLine::InitCtrlFloat( CGridCtrl* pGridCtrl )
{
	if ( m_bInitialFloat )
	{
		return;
	}
	else
	{
		m_bInitialFloat = true;
	}

	//---����Ĭ�ϸ߶�
	pGridCtrl->SetDefCellHeight(22);
	pGridCtrl->SetRowCount(KiGridFloatCtrlRowCountForStockCn);

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
	pCell->SetText(_T(" ���"));


	pCell = (CGridCellSys *)pGridCtrl->GetCell(8, 0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(_T(" ���"));

	pCell = (CGridCellSys *)pGridCtrl->GetCell(10, 0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(_T(" ����"));

	pCell = (CGridCellSys *)pGridCtrl->GetCell(12, 0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(_T(" �ɽ���"));
	m_pCell2 = pCell;

	pCell = (CGridCellSys *)pGridCtrl->GetCell(14, 0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(_T(" ���"));
	m_pCell1 = pCell;

	pCell = (CGridCellSys *)pGridCtrl->GetCell(16, 0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(_T(" �Ƿ�"));

	pCell = (CGridCellSys *)pGridCtrl->GetCell(18, 0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(_T(" ���"));

	// ��Ʊ�������� ��ͨ��
	pCell = (CGridCellSys *)pGridCtrl->GetCell(20, 0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetText(_T("������"));

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

	pGridCtrl->SetCellType(11,0, RUNTIME_CLASS(CGridCellSymbol));
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(11,0);
	pCellSymbol->SetDefaultFont(ESFSmall);
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
	pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

	pCell = (CGridCellSys *)pGridCtrl->GetCell(13, 0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetDefaultTextColor(ESCVolume);

	pCell = (CGridCellSys *)pGridCtrl->GetCell(15, 0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pCell->SetDefaultTextColor(ESCAmount);

	pGridCtrl->SetCellType(17,0, RUNTIME_CLASS(CGridCellSymbol));
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(17,0);	
	pCellSymbol->SetDefaultFont(ESFSmall);
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	
	pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

	pGridCtrl->SetCellType(19,0, RUNTIME_CLASS(CGridCellSymbol));
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(19,0);
	pCellSymbol->SetDefaultFont(ESFSmall);
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	
	pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

	// ������ ��ͨ��
	pCell = (CGridCellSys *)pGridCtrl->GetCell(21,0);
	pCell->SetDefaultFont(ESFSmall);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

	pGridCtrl->ShowWindow(SW_SHOW);
	SetGridFloatInitPos();
}

void CIoViewKLine::CreateRegion()
{
	ASSERT( NULL == m_pRegionMain );
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

void CIoViewKLine::SplitRegion()
{
	CIoViewChart::SplitRegion();
	UpdateGDIObject();
}

int32 CIoViewKLine::GetSubRegionTitleButtonNums()
{
	return KiKLineSubRegionButtonNums;
}

const T_FuctionButton* CIoViewKLine::GetSubRegionTitleButton(int32 iIndex)
{
	if( iIndex < 0 || iIndex >= GetSubRegionTitleButtonNums() )
	{
		return NULL;
	}

	return &s_astKLineSubRegionButtons[iIndex];
}

void CIoViewKLine::OnDestroy() 
{
	ReleaseMemDC();

	MSG msg = {0};
	while ( PeekMessage(&msg, m_hWnd, TEMP_UM_CHOUMANOTIFY, TEMP_UM_CHOUMANOTIFY, PM_REMOVE) )
	{
		T_ChouMaNotify *pNotify = (T_ChouMaNotify *)msg.wParam;
		delete pNotify;
	}

	CIoViewChart::OnDestroy();
}

E_NodeTimeInterval CIoViewKLine::GetTimeInterval()
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

bool32 CIoViewKLine::GetTimeIntervalInfo(E_NodeTimeInterval eNodeTimeInterval, E_NodeTimeInterval &eNodeTimeIntervalCompare, E_KLineTypeBase &eKLineTypeCompare,int32 &iScale)
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

void CIoViewKLine::SetTimeInterval(T_MerchNodeUserData &MerchNodeUserData, E_NodeTimeInterval eNodeTimeInterval)
{
	MerchNodeUserData.m_eTimeIntervalFull		= ENTIMinute;
	E_NodeTimeInterval eNodeTimeIntervalCompare = ENTIDay;
	E_KLineTypeBase	   eKLineTypeCompare		= EKTBDay;

	int32 iScale = 1;
	if (!GetTimeIntervalInfo(eNodeTimeInterval, eNodeTimeIntervalCompare, eKLineTypeCompare, iScale))
	{
		// ��Ӧ�ó����������
		return;
	}

	// ���ʵ�ʲ���Ҫ�����µ�k�ߣ����ڲ����ڽ��ָ�������»ᵼ��ͬ���ڵĶ�XX��XX������ xl 1025
	m_bForceCalcSaneIndex = true;	// ǿ������ - ��ʱ�������
	ResetTimeInterval(&MerchNodeUserData,eNodeTimeInterval,eNodeTimeIntervalCompare);
	if ( !m_bShowSaneIndexNow )
	{
		m_bForceCalcSaneIndex = false;
	}
}

void CIoViewKLine::ResetTimeInterval(T_MerchNodeUserData* pData,IN E_NodeTimeInterval eNodeTimeInterval,IN E_NodeTimeInterval eNodeTimeIntervalCompare)
{
	pData->m_eTimeIntervalFull    = eNodeTimeInterval;
	pData->m_eTimeIntervalCompare = eNodeTimeIntervalCompare;

	//
	m_aMerchsHaveCalcLatestValues.RemoveAll();
	SetUpdateTime();

	m_aSaneIndexValues.RemoveAll();
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




void  CIoViewKLine::AddExpMerch(IN CMerch *pMerch)
{
	CMerch *pMerchXmlOld        = m_pMerchXml;	// ԭ����ע��
	bool32 bIsAddExp = true;

	// �Զ����Ӷ�Ӧָ��
	CMerch *pExpOld = GetMerchAccordExpMerch(pMerchXmlOld);
	CMerch *pExpNow = GetMerchAccordExpMerch(pMerch);
	//if ( m_MerchParamArray.GetSize() > 1 )
	//{
	//	RemoveAllCmpMerch();	// ɾ�����д��ڵĵ�����Ʒ, �Ժ��е���Ȩ֤��ô�죿
	//}
	//AddCmpMerch(pExpNow, false, true, false);
	if ( pExpOld != pExpNow )
	{
		RemoveCmpMerch(pExpOld);
		if ( pExpNow != NULL && pExpNow != pMerch )
		{
			// ����ָ������, �л�ʱ����������
			//AddCmpMerch(pExpNow, false, false);	// ��ʱ��ӣ������ʱ��û�г�ʼ��
			bIsAddExp = true;
		}
	}
	else if ( pExpNow != pMerch )
	{
		// �鿴�Ƿ��ڵ�����Ʒ�У�û�о���Ҫ���
		bool32 bNeedAdd = true;
		for ( int32 i=m_MerchParamArray.GetUpperBound() ; i >= 0 ; i-- )
		{
			if ( m_MerchParamArray[i]->m_pMerchNode == pExpNow && !m_MerchParamArray[i]->bMainMerch )
			{
				bNeedAdd = false;
				break;
			}
		}
		if ( bNeedAdd )
		{
			// ����ָ������, �л�ʱ����������
			// ��ʱ���ԭ������Ʒ�Ǹ�ָ�������Ȼ���ʧ�ܣ������Ҫ��ȫ����Ʒ�л���ϲ������ָ���ĵ���
			//if ( !AddCmpMerch(pExpNow, false, false) )	// �����ʱ����MerchData��û��ʼ�������
			bIsAddExp = true;
		}
	}

	if (bIsAddExp)
	{
		SetTimer(KTimerIdDelayAutoAddExpCmp, KTimerPeriodDelayAutoAddExpCmp,  NULL);
	}
}


// ֪ͨ��ͼ�ı��ע����Ʒ
//lint --e{429}
void CIoViewKLine::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	//	_LogCheckTime("[Client==>] CIoViewKLine::OnVDataMerchChanged", g_hwndTrace);

	if (m_pMerchXml == pMerch)
		return;

	// ��С�ĺ�����ʱ, �л���Ʒ��Ҫ��ס�������
	if ( m_iNodeCountPerScreen > KMinRequestKLineNumber * 1.5 )
	{
		m_iNodeCountPerScreen = (int32)(KMinRequestKLineNumber * 1.5);
	}

	m_mapLandMine.clear();		// �������

	// ��m_pMerchXmlΪNULLʱ����ʼ����ĻӦ����ʾ��Node����������Ǵ�Xml�г�ʼ����Node����Ӧ���Ѿ���ʼ�����˵�
	if ( NULL == m_pMerchXml && !m_bFromXml )
	{
		// Ӧ��û�е��ù�TryGetMore..
		// ��ʱ��Client Size�Ƕ��٣�
		// 		ASSERT( m_bRequestNodesByRectView );
		if ( m_bRequestNodesByRectView )
		{
			// ���ݵ�ǰ��ͼ��С,������ʵ�K �߸���
			// ��Ϊ��ʱ���������������ټ��㻭ͼ�Ⱦ��������С.������ʱ�� m_pRegionMain->m_RectView ��0.
			// ��Ҫ�����ֵ����,��GGTongView �Ŀ��ֵ����

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

			// һ���Եı�־,����͸�λ
			m_bRequestNodesByRectView = false;
		}
	}

	CIoViewChart::OnVDataMerchChanged(iMarketId, StrMerchCode, pMerch);

	OnAQExtDoQueueItem();
	//
	ReSetSaneIndex(true);

	//
	if (NULL != pMerch)
	{		
		if ( NULL == m_pAbsCenterManager || NULL == m_pRegionMain )
		{
			return;
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

		bool32 bInhert;
		T_MerchNodeUserData* pData = NewMerchData(pMerch, true, bInhert);

		if ( -1 != m_iIntervalXml )
		{
			pData->m_eTimeIntervalFull = (E_NodeTimeInterval)m_iIntervalXml;

			E_KLineTypeBase	eKLineTypeCompare = EKTBDay;
			int32 iScale = 0;

			GetTimeIntervalInfo(pData->m_eTimeIntervalFull,pData->m_eTimeIntervalCompare,eKLineTypeCompare,iScale);
			m_iIntervalXml = -1;
		}

		if ( !bInhert )
		{
			CNodeSequence* pNodes = CreateNodes();
			pNodes->SetUserData(pData);
			pData->m_pKLinesShow = pNodes;

			// new 1 curves/nodes/...
			CChartCurve* pCurve = m_pRegionMain->CreateCurve(CChartCurve::KDependentCurve|CChartCurve::KRequestCurve|CChartCurve::KTypeKLine|CChartCurve::KYTransformByLowHigh|CChartCurve::KUseNodesNameAsTitle/*|CChartCurve::KDonotPick*/);		 
			CChartDrawer* pKLineDrawer = new CChartDrawer(*this, (CChartDrawer::E_ChartDrawStyle)GetCurChartKlineDrawType());

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
		}

		// ���鴰����ʾ����
		AdjustCtrlFloatContent();

		// 
		if ( m_IndexPostAdd.id >= 0 )
		{
			m_IndexPostAdd.id = -1;
			AddIndex(m_IndexPostAdd.pRegion,m_IndexPostAdd.StrIndexName);
		}

		// ���Ӵ���ָ������
		AddExpMerch(pMerch);


		// 
		OnVDataMerchKLineUpdate(pMerch);

		m_mapLandMine = pMerch->m_mapLandMine;	// ��ֵ��ʼ������Ϣ����

		//
		// RequestViewData();

		SetCurveTitle(pData);
		UpdateAxisSize();
		UpdateSelfDrawCurve();
		m_pRegionMain->SetDrawFlag(CRegion::KDrawCurves);
		PostMessage(WM_PAINT);
	}
	else
	{
		// zhangbo 20090824 #�����䣬 ������Ʒ������ʱ����յ�ǰ��ʾ����
		//...
	}

	// �����ǰָ�겻�����Ʒƥ�䣬������ָ��
	if ( NULL != m_pMerchXml )
	{
		CStringArray aIndexNames;
		GetCurrentIndexNameArray(aIndexNames);
		bool32 bIsExp = GetMerchKind(m_pMerchXml) == ERTExp;
		for ( int32 i=0; i < aIndexNames.GetSize() ; i++ )
		{
			CFormularContent *pContent = CFormulaLib::instance()->GetFomular(aIndexNames[i]);
			if ( NULL != pContent )
			{
				bool32 bIsMain = CheckFlag(pContent->flag, CFormularContent::KAllowMain);
				if ( pContent->m_byteIndexStockType == EIST_Index && !bIsExp )
				{
					// �Ǵ�����Ʒװ�����˴���ָ��
					if ( bIsMain )
					{
						ReplaceIndex(aIndexNames[i], _T("MA"), true, false);
					}
					else
					{
						ReplaceIndex(aIndexNames[i], _T("MACD"), true, false);
					}
				}
				else if ( pContent->m_byteIndexStockType == EIST_Stock && bIsExp )
				{
					// ������Ʒװ�����˸���ָ��
					if ( bIsMain )
					{
						ReplaceIndex(aIndexNames[i], _T("MA"), true, false);
					}
					else
					{
						ReplaceIndex(aIndexNames[i], _T("MACD"), true, false);
					}
				}
			}
		}
	}

	//
	if(pMerch)
	{
		SetTopCenterTitle(pMerch->m_MerchInfo.m_StrMerchCnName);
	}

	CGGTongApp *ggApp = (CGGTongApp *)AfxGetApp();
	if ( ggApp->m_pConfigInfo->m_bIsVip )
	{
		/* 
		if ( iMarketId == ggApp->m_iMarketID && StrMerchCode == ggApp->m_strMerchCode )
		{
		m_bMatchMerCode = true;
		}
		else
		{
		m_bMatchMerCode = false;
		}
		*/

		//���ù�������view
		//	if ( m_bMatchMerCode && m_bMatchTimer && !ggApp->m_bAdvertiseExist )
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
			//	if ( m_bMatchMerCode && m_bMatchTimer)
			//	{
			RedrawAdvertise(); // 2014-04-01 by cym 
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
bool32 CIoViewKLine::OnVDataAddCompareMerch(IN CMerch *pMerch)
{
	// ...fangz 0526 ��ʱע��

	/*
	if (NULL == pMerch)
	return false;

	StartTime(201);
	CIoViewChart::OnVDataAddCompareMerch(pMerch);

	ASSERT(NULL!=m_pRegionMain);

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
	CChartCurve::KTypeKLine |
	CChartCurve::KYTransformByLowHigh|
	CChartCurve::KUseNodesNameAsTitle);

	CChartDrawer* pCompareKLineDrawer = new CChartDrawer(*this, CChartDrawer::EKDSNormal);
	CChartCurve *pCurveDependent = m_pRegionMain->GetDependentCurve();
	if (NULL == pCurveDependent)
	return false;

	CChartDrawer* pKLineDrawer = (CChartDrawer*)pCurveDependent->GetDrawer();
	pCompareKLineDrawer->m_eChartDrawType = pKLineDrawer->m_eChartDrawType;
	pCurve->AttatchDrawer(pCompareKLineDrawer);
	pCurve->AttatchNodes(pNodes);

	RequestViewData();

	// ������Ʒʱ��Ϊ׼���������Ʒ����
	{
	T_MerchNodeUserData *pData0 = m_MerchParamArray[0];
	if (NULL != pData0 && NULL != pData0->m_pMerchNode && pData0->m_aKLinesCompare.GetSize() > 0)
	{
	// ������Ʒ��������Ʒ��ʱ��Ϊ׼����
	CGmtTime TimeStart = pData->m_aKLinesCompare[0].m_TimeCurrent;
	CGmtTime TimeEnd = pData->m_aKLinesCompare[pData->m_aKLinesCompare.GetSize() - 1].m_TimeCurrent;

	// ָ��ʱ�������
	E_NodeTimeInterval eNodeTimeIntervalCompare = ENTIDay;
	E_KLineTypeBase	   eKLineTypeCompare		= EKTBDay;
	int32 iScale = 1;
	if (!GetTimeIntervalInfo(pData->m_eTimeIntervalFull, eNodeTimeIntervalCompare, eKLineTypeCompare, iScale))
	{
	//ASSERT(0);	// ��Ӧ�ó����������
	return false;
	}

	CMmiReqMerchKLine info;
	info.m_eKLineTypeBase = eKLineTypeCompare;								// ʹ��ԭʼK������
	info.m_iMarketId	  = pData->m_pMerchNode->m_MerchInfo.m_iMarketId;
	info.m_StrMerchCode   = pData->m_pMerchNode->m_MerchInfo.m_StrMerchCode;

	info.m_eReqTimeType   = ERTYSpecifyTime;
	info.m_TimeStart  	  = TimeStart;
	info.m_TimeEnd		  = TimeEnd;

	RequestViewData(m_pAbsCenterManager,(CMmiCommBase*)&info);

	//
	RequestWeightData();
	}
	}

	// 
	SetCurveTitle(pData);
	// 	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	// 	PostMessage(WM_PAINT);

	KillTimer(103);
	SetTimer(103,5,NULL);

	EndTime(201,_T("CIoViewKLine::OnVDataAddCompareMerch"));
	*/
	return true;
}

void CIoViewKLine::SetCurveTitle ( T_MerchNodeUserData* pData )
{
	CString StrTitle  = _T("");
	CString StrWeightTitle = _T("");
	CMerch* pMerch = pData->m_pMerchNode;
	if ( NULL == pMerch )
	{
		if ( pData->bMainMerch )
		{
			StrTitle = _T(" ") + TimeInterval2String(pData->m_eTimeIntervalFull) + _T(" (��)");

			E_ReportType eMerchKind = GetMerchKind(m_pMerchXml);

			if ( !CReportScheme::IsFuture(eMerchKind) )
			{
				if( EWTPre == m_eWeightType )
				{
					StrWeightTitle += L" (ǰ��Ȩ) ";
				}
				else if( EWTAft == m_eWeightType )
				{
					StrWeightTitle += L" (��Ȩ) ";
				}

				pData->m_pKLinesShow->SetWeightName(StrWeightTitle);
			}				
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
			StrTitle = pMerch->m_MerchInfo.m_StrMerchCnName + _T("  ") + TimeInterval2String(pData->m_eTimeIntervalFull, pData->m_iTimeUserMultipleMinutes, pData->m_iTimeUserMultipleDays) + _T(" ");

			E_ReportType eMerchKind = GetMerchKind(m_pMerchXml);

			if ( !CReportScheme::IsFuture(eMerchKind) )
			{
				if( EWTPre == m_eWeightType )
				{
					StrWeightTitle += L" (ǰ��Ȩ) ";
				}
				else if( EWTAft == m_eWeightType )
				{
					StrWeightTitle += L" (��Ȩ) ";
				}

				pData->m_pKLinesShow->SetWeightName(StrWeightTitle);
			}	
		}
		else
		{
			StrTitle = _T("[") + pMerch->m_MerchInfo.m_StrMerchCnName + _T(" ����] ");
		}
	}

	pData->m_pKLinesShow->SetName(StrTitle);

	if (pData->bMainMerch && m_bShowTopToolBar)
	{
		// SetTopCenterTitle(StrTitle);
	}	
}

void CIoViewKLine::OnVDataForceUpdate()
{
	if ( NULL == m_pRegionMain )
	{
		return;
	}

	m_bForceUpdate = true;

	CIoViewChart::OnVDataForceUpdate();
	RequestViewData();

	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	PostMessage(WM_PAINT);

	m_bForceUpdate = false;
}

void CIoViewKLine::OnVDataFormulaChanged ( E_FormulaUpdateType eUpdateType, CString StrName )
{
	if ( EFUTClrKLineUpdate == eUpdateType || EFUTTradeChoseUpdate == eUpdateType )
	{
		if ( m_MerchParamArray.GetSize() > 0 )
		{
			T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
			if ( NULL != pData )
			{
				UpdateMainMerchKLine(*pData, true);
			}
		}		
	}
	else if ( EFUTClrKLineDelete == eUpdateType )
	{
		OnDelClrKLine();
	}
	else if ( EFUTTradeChoseDelete == eUpdateType )
	{
		OnDelTrade();
	}
	else
	{
		CIoViewChart::OnVDataFormulaChanged(eUpdateType, StrName);
	}

	OnVDataMerchKLineUpdate(m_pMerchXml);
}

void CIoViewKLine::OnVDataRealtimePriceUpdate(IN CMerch *pMerch)
{
	if (NULL == pMerch || pMerch != m_pMerchXml)
		return;

	CalcTodayRise(pMerch);
}

bool32 CIoViewKLine::IsShowNewestKLine(OUT CGmtTime& TimeStartInFullList, OUT CGmtTime &TimeEndInFullList)
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

void CIoViewKLine::OnVDataPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType)
{
	// ...fangz1117  Ŀǰ UpdateMainMerchKLine ����ÿ�ζ� bUpdate = false ȫ����������,
	// �Ż��Ժ�,Ҫ�����Ȩ������,��Ҫ��Ϊ�����̺�û��K �������ʱ��������û�и���

	if (NULL == pMerch || pMerch != m_pMerchXml)
		return;

	if (EPFTWeight != ePublicFileType)	// �������Ȩ����
		return;

	//if (!IsWindowVisible())
	//{
	//	// 0002149 xl 0705 ����ڼ���󣬶�ʱ���ڱ�����GGTongView��F7�������Ϊ���أ��ᵼ�»�ԭʱ��������δ���¶�������ݵ���ʾ������ȡ�����Ż�
	//	AddFlag(m_uiFlag,CIoViewChart::KMerchKLineUpdate);
	//	return;
	//}

	if (m_MerchParamArray.GetSize() <= 0)
	{
		return;
	}

	// ������Ʒ������������
	// xl 0610 ��Ȩ���ݷ��أ�Ҫ�����㸴Ȩ&ָ��
	m_bForceCalcSaneIndex = true;
	if ( m_eWeightType != EWTNone )
	{
		for (int32 i = 0; i < m_MerchParamArray.GetSize(); i ++)
		{
			T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(i);
			SetCurveTitle(pData);

			if (pData->m_pMerchNode == pMerch)
			{
				if (0 == i)
				{
					if (UpdateMainMerchKLine(*pData))
					{
						ReDrawAysnc();	
					}
				}
				else
				{
					if (UpdateSubMerchKLine(*pData))
					{
						ReDrawAysnc();
					}
				}
			}
		}
	}
	else
	{
		ReDrawAysnc();
	}
}

void CIoViewKLine::ClearLocalData(bool32 bClearAll/* = true*/)
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
}

void CIoViewKLine::OnVDataMerchKLineUpdate(IN CMerch *pMerch)
{
	if (NULL == pMerch/* || pMerch != m_pMerchXml*/)
		return;

	//if (!IsWindowVisible())
	//{
	//	// 0002149 xl 0705 ����ڼ���󣬶�ʱ���ڱ�����GGTongView��F7�������Ϊ���أ��ᵼ�»�ԭʱ��������δ���¶�������ݵ���ʾ������ȡ�����Ż�
	//	// Ӧ���ṩһ��������ͼ֪ͨ�ӿ�,OnShowIoView(bShow) - �������˶�ʱ���⣬��֪������ʲô�ܹ������������ʱ��
	//		AddFlag(m_uiFlag,CIoViewChart::KMerchKLineUpdate);
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

		if (pData->m_pMerchNode == pMerch)
		{
			SetCurveTitle(pData);
			if (0 == i)
			{
				if (UpdateMainMerchKLine(*pData))
				{
					ReDrawAysnc();	
				}
			}
			else
			{
				if (UpdateSubMerchKLine(*pData, false))
				{
					ReDrawAysnc();
				}
			}
		}
	}
}


typedef struct _ClientKLineResponse
{
	u32		iTime;		// ʱ��
	float	fOpen;		// ����
	float	fHigh;		// ���
	float	fLow;		// ���
	float	fClose;		// ����
	float	fVolume;		//�ɽ���
	float	fAmount;		// �ɽ���
	float	fHold;		// �ֲ�
	float	fAvg;		// �����
	u16		iUpCount;		// ���Ǽ���
	u16		iDownCount;	// �µ�����
	_ClientKLineResponse()
	{
		memset((void*)this, 0, sizeof(_ClientKLineResponse));
	}
} T_ClientKLineResponse;

void SaveKLine(const CArray<CKLine,CKLine> &aSrcKLine, CMerch *pMerch, int iCycle, int iScale)
{
	if ( NULL == pMerch || aSrcKLine.GetSize() <= 0 )
	{
		return;
	}

	TCHAR path[MAX_PATH] = _T("./tmp/");
	_tcheck_if_mkdir(path);

	CString StrName;
	StrName.Format(_T("%s_%s_%d_%d.kline"), pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer(), pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), iCycle, iScale);
	StrName = path + StrName;

	CStdioFile file;
	if ( file.Open(StrName, CFile::typeBinary | CFile::modeCreate |CFile::modeWrite) )
	{
		for ( int i=0; i < aSrcKLine.GetSize() ; i++ )
		{
			T_ClientKLineResponse kline;
			const CKLine &src = aSrcKLine[i];
			kline.iTime = src.m_TimeCurrent.GetTime();
			kline.fOpen = src.m_fPriceOpen;
			kline.fHigh = src.m_fPriceHigh;
			kline.fLow	= src.m_fPriceLow;
			kline.fClose = src.m_fPriceClose;
			kline.fVolume = src.m_fVolume;
			kline.fAmount = src.m_fAmount;
			kline.fHold = src.m_fHold;
			kline.fAvg  = src.m_fPriceAvg;
			kline.iUpCount = src.m_usRiseMerchCount;
			kline.iDownCount = src.m_usFallMerchCount;

			file.Write(&kline, sizeof(kline));
		}
		file.Close();
	}
	else
	{
		ASSERT( 0 );
	}
}

bool32 CIoViewKLine::UpdateMainMerchKLine(T_MerchNodeUserData &MerchNodeUserData, bool32 bForceUpdate)
{
	_LogCheckTime("[==>Client] CIoViewKLine::UpdateMainMerchKLine ��ʼ", g_hwndTrace);

	if (m_MerchParamArray.GetSize() <= 0 || m_MerchParamArray[0] != &MerchNodeUserData)
	{
		return false;
	}

	//
	E_NodeTimeInterval eNodeTimeIntervalCompare = ENTIDay;
	E_KLineTypeBase    eKLineType			    = EKTBDay;

	int32 iScale = 1;
	if (!GetTimeIntervalInfo(MerchNodeUserData.m_eTimeIntervalCompare, eNodeTimeIntervalCompare, eKLineType, iScale))
	{
		// ��Ӧ�ó����������
		return false;
	}

	CMerch* pMerch = MerchNodeUserData.m_pMerchNode;
	if (NULL == pMerch)
	{
		return false;
	}

	CUpdateShowDataChangeHelper showChangeHelper(this, &MerchNodeUserData);	// �����ж�main�Ƿ���ʾ������

	// �жϵ�ǰ��ʾ�� �Ƿ���ʾ���¼�
	CGmtTime TimeStartInFullList, TimeEndInFullList;

	bool32 bShowNewestKLine		= IsShowNewestKLine(TimeStartInFullList, TimeEndInFullList);
	int32 iShowPosInFullList	= MerchNodeUserData.m_iShowPosInFullList;
	int32 iShowCountInFullList	= MerchNodeUserData.m_iShowCountInFullList;

	// �Ȼ�ȡ��Ӧ��K��
	int32 iPosFound;
	CMerchKLineNode* pKLineRequest = NULL;
	pMerch->FindMerchKLineNode(eKLineType, iPosFound, pKLineRequest);

	// �����Ҳ���K�����ݣ� �ǾͲ���Ҫ��ʾ��
	if (NULL == pKLineRequest || 0 == pKLineRequest->m_KLines.GetSize())	
	{
		MerchNodeUserData.m_aKLinesCompare.RemoveAll();
		MerchNodeUserData.m_aKLinesFull.RemoveAll();

		if (NULL != MerchNodeUserData.m_pKLinesShow)		
			MerchNodeUserData.m_pKLinesShow->RemoveAll();

		return true;
	}

	// �Ż��������������£� ���¼�����������ʷ���ݸ��������ģ� ������������ر��жϴ���
	bool32 bUpdateLast	= false;
	bool32 bModifyLast	= false;
	bool32 bAddSome		= false;

	CKLine *pKLineSrc = (CKLine *)pKLineRequest->m_KLines.GetData();
	int32 iCountSrc   = pKLineRequest->m_KLines.GetSize();

	CKLine *pKLineCmp = (CKLine *)MerchNodeUserData.m_aKLinesCompare.GetData();
	int32 iCountCmp   = MerchNodeUserData.m_aKLinesCompare.GetSize();

	// 
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

	if ( bUpdateLast && !bModifyLast && !bAddSome && !m_bForceCalcSaneIndex && !bForceUpdate )
	{
		// ʲô��û��, ���ؿ� (xl ������CalcSaneIndexҪ���²��ܷ���)
		return true;			
	}

	//////////////////////////////////////////////////////////////////////
	// ��Ը������¼ۺ͸�����ʷ�ֱ���

	// ...fangz0729 ����һ��ԭ����K ������. 

	T_MerchNodeUserData* pMainData = m_MerchParamArray.GetAt(0);	

	CArray<CKLine,CKLine> aKLineBeforUpdate;
	aKLineBeforUpdate.RemoveAll();
	aKLineBeforUpdate.Copy(pMainData->m_aKLinesFull);

	//////////////////////////////////////////////////////////////////////////	
	// ...fangz0805 ��ʱû�����Ż�
	// ...fangz1117 �����Ż��Ժ�,�����Ȩ����Ҫ���������. ��Ҫ��ΪK ��û���¶����´���
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
		// �����������, ��������ڵ�����
		ClearLocalData(false);

		// ��ͼ��������
		if (iCountSrc == 0)
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

			//
			int32 iTimeInitPerDay = 0, iTimeOpenPerDay = 0, iTimeClosePerDay = 0;

			CMerch *pTmpMerch = pData->m_pMerchNode;
			if ( NULL == pTmpMerch	)
			{
				return false;					
			}

			pTmpMerch->m_Market.m_MarketInfo.GetTimeInfo(iTimeInitPerDay, iTimeOpenPerDay, iTimeClosePerDay);				

			//////////////////////////////////////////////////////////////////////////
			// ��Ȩ��Ȩ:
			CArray<CKLine, CKLine> aSrcKLines;
			aSrcKLines.Copy(pData->m_aKLinesCompare);

			// ���Ƿ�Ҫ����Ȩ����
			CArray<CKLine, CKLine> aWeightedKLines;

			if ( CIoViewKLine::EWTAft == m_eWeightType )
			{
				// ��Ȩ
				if ( CMerchKLineNode::WeightKLine(aSrcKLines, pTmpMerch->m_aWeightDatas, false, aWeightedKLines, true) )
				{
					aSrcKLines.Copy(aWeightedKLines);
				}		
			}
			else if ( CIoViewKLine::EWTPre == m_eWeightType )  
			{
				// ǰ��Ȩ
				if ( CMerchKLineNode::WeightKLine(aSrcKLines, pTmpMerch->m_aWeightDatas, true, aWeightedKLines, true) )
				{
					aSrcKLines.Copy(aWeightedKLines);
				}
			}						


			//
			CMarketIOCTimeInfo MarketIOCTimeInfo;
			if (!pTmpMerch->m_Market.GetRecentTradingDay(m_pAbsCenterManager->GetServerTime(), MarketIOCTimeInfo, pTmpMerch->m_MerchInfo))
			{
				//ASSERT(0);
				return false;
			}

			//////////////////////////////////////////////////////////////////////////			
			// �������ݣ� ѹ������
			switch (pData->m_eTimeIntervalFull)
			{
			case ENTIMinute:
				{
					pData->m_aKLinesFull.SetSize(pData->m_aKLinesCompare.GetSize());	
					memcpyex(pData->m_aKLinesFull.GetData(), aSrcKLines.GetData(), sizeof(CKLine) * aSrcKLines.GetSize());						
				}
				break;
			case ENTIMinute5:
				{
					pData->m_aKLinesFull.SetSize(aSrcKLines.GetSize());	
					memcpyex(pData->m_aKLinesFull.GetData(), aSrcKLines.GetData(), sizeof(CKLine) * aSrcKLines.GetSize());
				}
				break;					
			case ENTIMinute15:
				{
					if (!CMerchKLineNode::CombinMinuteN(pTmpMerch, 15, aSrcKLines, pData->m_aKLinesFull))						
					{
						//ASSERT(0);
					}	
				}
				break;
			case ENTIMinute30:
				{
					if (!CMerchKLineNode::CombinMinuteN(pTmpMerch, 30, aSrcKLines, pData->m_aKLinesFull))						
					{
						//ASSERT(0);
					}
				}
				break;
			case ENTIMinute60:
				{	
					pData->m_aKLinesFull.SetSize(aSrcKLines.GetSize());	
					memcpyex(pData->m_aKLinesFull.GetData(), aSrcKLines.GetData(), sizeof(CKLine) * aSrcKLines.GetSize());
				}	
				break;
			case ENTIMinute180:
				{
					if (!CMerchKLineNode::CombinMinuteN(pTmpMerch, 180, aSrcKLines, pData->m_aKLinesFull))						
					{
						//ASSERT(0);
					}
				}
				break;
			case ENTIMinute240:
				{
					if (!CMerchKLineNode::CombinMinuteN(pTmpMerch, 240, aSrcKLines, pData->m_aKLinesFull))						
					{
						//ASSERT(0);
					}
				}
				break;
			case ENTIMinuteUser:
				{					
					if (!CMerchKLineNode::CombinMinuteN(pTmpMerch, pData->m_iTimeUserMultipleMinutes, aSrcKLines, pData->m_aKLinesFull))						
					{
						//ASSERT(0);
					}
				}	
				break;
			case ENTIDay:
				{
					pData->m_aKLinesFull.SetSize(aSrcKLines.GetSize());	
					memcpyex(pData->m_aKLinesFull.GetData(), aSrcKLines.GetData(), sizeof(CKLine) * aSrcKLines.GetSize());
				}
				break;
			case ENTIWeek:
				{
					if (!CMerchKLineNode::CombinWeek(aSrcKLines, pData->m_aKLinesFull))
					{
						//ASSERT(0);
					}
				}
				break;
			case ENTIDayUser:
				{
					if (!CMerchKLineNode::CombinDayN(aSrcKLines, pData->m_iTimeUserMultipleDays, pData->m_aKLinesFull))
					{
						//ASSERT(0);
					}
				}
				break;
			case ENTIMonth:
				{
					pData->m_aKLinesFull.SetSize(aSrcKLines.GetSize());	
					memcpyex(pData->m_aKLinesFull.GetData(), aSrcKLines.GetData(), sizeof(CKLine) * aSrcKLines.GetSize());
				}
				break;			

			case ENTIQuarter:
				{
					if (!CMerchKLineNode::CombinMonthN(aSrcKLines, 3, pData->m_aKLinesFull))
					{
						//ASSERT(0);
					}
				}
				break;
			case ENTIYear:
				{
					if (!CMerchKLineNode::CombinMonthN(aSrcKLines, 12, pData->m_aKLinesFull))
					{
						//ASSERT(0);
					}
				}
				break;
			default:
				//ASSERT(0);
				break;
			}

			//
			int32 iSpaceRightCount = GetSpaceRightCount();

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

					if (pData->m_aKLinesFull.GetSize() <= 0)
						return true;				// û������Ҫ��ʾ
					else
					{
						if (pData->m_aKLinesFull.GetSize() <= iMaxShowCount)
						{
							iShowPosInFullList = 0;
							iShowCountInFullList = pData->m_aKLinesFull.GetSize();
						}
						else
						{
							iShowCountInFullList = iMaxShowCount;
							iShowPosInFullList = pData->m_aKLinesFull.GetSize() - iMaxShowCount;
						}
					}
				}
				else	
				{
					int32 iShowPosStart = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pData->m_aKLinesFull, TimeStartInFullList);
					if (iShowPosStart >= 0 && iShowPosStart < pData->m_aKLinesFull.GetSize())
					{
						int32 iMaxShowCountInFullList = pData->m_aKLinesFull.GetSize() - iShowPosStart;
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

			// ���ø���ʱ��:
			// ����һ����ʱ��仯��,����������һ���򼸸�K ��,���� K �߷��ص��������˴�ı仯. Ҫ�������ʱ��:			
			if( aKLineBeforUpdate.GetSize() <= 0 && pMainData->m_aKLinesFull.GetSize() > 0 )
			{
				// ԭ��û����,��������:		
				SetUpdateTime();
			}
			else if ( aKLineBeforUpdate.GetSize() > 0 && pMainData->m_aKLinesFull.GetSize() > 0 )
			{
				// 1: ����һ��K �ߵ�ʱ�䲻ͬ��(����һ�����������б仯)
				CGmtTime TimeKLineBefore = aKLineBeforUpdate.GetAt(aKLineBeforUpdate.GetSize() - 1).m_TimeCurrent;
				CGmtTime TimeKLineNow    = pMainData->m_aKLinesFull.GetAt(pMainData->m_aKLinesFull.GetSize() - 1).m_TimeCurrent;

				if ( TimeKLineBefore != TimeKLineNow )
				{				
					SetUpdateTime();
				}					

				// 2: ����һ��K �ߵ����ںͷ��������ڵ�ʱ��,���ڲ�ͬ��(���ܹ�ҹ)
				// ԭ��: ���߻������ߵ�.��һ���ö�ʱ��,���ָ���ʱ�����������10 ��.���Բ�������ʱ��,���º���һֱû��������ʱ�������ø���ʱ���ʱ��
				CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();

				if ( TimeKLineNow.GetDay() != TimeNow.GetDay() )
				{
					SetUpdateTime();
				}
			}

			//
			bool32 bNeedRedrawAftCalc = false;

			// ����ָ��
			if ( m_bShowSaneIndexNow )
			{
				E_ReportType eReportType = GetMerchKind(m_pMerchXml);
				bool32 bJudgeKTOC = false;

				if (CReportScheme::IsFuture(eReportType))
				{
					bJudgeKTOC = true;
				}

				int32 iReval = CompareKLinesChange(aKLineBeforUpdate, pMainData->m_aKLinesFull);

				// m_bForceCalcSaneIndex Ҫ�ڼ�����Ϻ����
				if ( 0 == m_aSaneIndexValues.GetSize() || m_bForceCalcSaneIndex )
				{
					iReval = 3;
				}

				if ( 2 == iReval )
				{
					SetUpdateTime();
				}

				// ...fangz0705 ������Ҫ�Ż�.��û����,ȫ����������.				
				// һ��Ҫ DEL!! ������ڴ�й©

				if ( 0 != iReval )
				{
					DEL(m_pIndexEMAForSane);
					m_pIndexEMAForSane = CalcIndexEMAForSane(pData->m_aKLinesFull);
				}

				if ( NULL != m_pIndexEMAForSane )
				{	
					bool32 bOK = false;

					if ( 1 == iReval )
					{						
						// ���¼۱仯. ����һ��������, ����һ��������, ����������ָ��:
						if ( m_aSaneIndexValues.GetSize() >= 0 )
						{
							if ( UpDateLatestIndexForSane(m_pIndexEMAForSane, m_aSaneIndexValues) )
							{
								bNeedRedrawAftCalc = true;

								// ����������								
								int32 iGoodTimes,iBadTimes;
								float fPriceLastClose = pMainData->m_aKLinesFull.GetAt(pMainData->m_aKLinesFull.GetSize() - 1).m_fPriceClose;

								StatisticaSaneValues2(KiStatisticaKLineNums, fPriceLastClose, m_iSaneRealKlineNums, m_aSaneIndexValues, m_iSaneTradeTimes, iGoodTimes, iBadTimes, m_fSaneAccuracyRate, m_fSaneProfitability, m_fProfit);							
							}
							else
							{
								//ASSERT(0);
							}
						}						
					}
					else if ( 2 == iReval )
					{						
						// ������һ��
						bool32 bKillTimer = false;

						// 1: ������һ��������
						CArray<CKLine, CKLine> aKLineTemp;
						aKLineTemp.Copy(pData->m_aKLinesFull);
						int32 iSizeTemp = aKLineTemp.GetSize();

						if ( iSizeTemp > 0 )
						{
							aKLineTemp.RemoveAt(iSizeTemp - 1);
							CalcLatestSaneIndex(false, bJudgeKTOC, m_pIndexEMAForSane, aKLineTemp, m_aSaneIndexValues, m_pAbsCenterManager->GetServerTime().GetTime(), bKillTimer);
						}

						// 2: �����µ�һ��,�ӵ����												
						bOK = CalcLatestSaneIndex(true, bJudgeKTOC, m_pIndexEMAForSane, pData->m_aKLinesFull, m_aSaneIndexValues, m_pAbsCenterManager->GetServerTime().GetTime(), bKillTimer);
						ASSERT(!bKillTimer);

						if ( bOK )
						{
							// ����б�
							m_aMerchsHaveCalcLatestValues.RemoveAll();

							int32 iGoodTimes,iBadTimes;
							float fPriceLastClose = pMainData->m_aKLinesFull.GetAt(pMainData->m_aKLinesFull.GetSize() - 1).m_fPriceClose;

							StatisticaSaneValues2(KiStatisticaKLineNums, fPriceLastClose, m_iSaneRealKlineNums, m_aSaneIndexValues, m_iSaneTradeTimes, iGoodTimes, iBadTimes, m_fSaneAccuracyRate, m_fSaneProfitability, m_fProfit);							
						}												
					}
					else if ( 3 == iReval)
					{						
						// �ı�ܴ�,��Ҫȫ������		

						bool32 bPassUpdateTime = true;
						bool32 bNeedTimer	   = false;

						CGmtTime TimeNow = pData->m_aKLinesFull.GetAt(pData->m_aKLinesFull.GetSize() - 1).m_TimeCurrent;
						CGmtTime TimeToUpdate;

						if ( !CIoViewKLine::GetKLineUpdateTime(m_pMerchXml, m_pAbsCenterManager, TimeNow, pData->m_eTimeIntervalFull, pData->m_iTimeUserMultipleMinutes, bNeedTimer, TimeToUpdate, bPassUpdateTime) )
						{
							// //ASSERT(0);
						}

						//
						bool32 bCalcOk = CalcHistorySaneIndex(bJudgeKTOC, bPassUpdateTime, m_pIndexEMAForSane, pData->m_aKLinesFull, m_aSaneIndexValues);

						// ���㽻�״���,׼ȷ��,�����ʵ�ֵ
						if ( bCalcOk )
						{				
							if ( m_bForceCalcSaneIndex )
							{
								m_bForceCalcSaneIndex = false;
								bNeedRedrawAftCalc = true;
							}

							//
							int32 iGoodTimes,iBadTimes;
							float fPriceLastClose = pMainData->m_aKLinesFull.GetAt(pMainData->m_aKLinesFull.GetSize() - 1).m_fPriceClose;

							StatisticaSaneValues2(KiStatisticaKLineNums, fPriceLastClose, m_iSaneRealKlineNums,m_aSaneIndexValues, m_iSaneTradeTimes, iGoodTimes, iBadTimes, m_fSaneAccuracyRate, m_fSaneProfitability, m_fProfit);							
						}												
					}					
				}
			}
			else
			{
				m_bForceCalcSaneIndex = false;	// ����Ҫ���������ñ�־
			}

			//********************************************************************************************************************************************
			// ���µ�ָ��
			for (int32 j = 0; j < pData->aIndexs.GetSize(); j++)
			{
				T_IndexParam* pIndex = pData->aIndexs.GetAt(j);
				if (NULL == pIndex || NULL == pIndex->pContent)
				{
					continue;
				}

				//
				g_formula_compute(this, this, (CChartRegion*)pIndex->pRegion, pData, pIndex, iShowPosInFullList, iShowPosInFullList + iShowCountInFullList); 				
			}

			// �������K ��
			if ( NULL != m_pFormularClrKLine )
			{
				DEL(m_pOutArrayClrKLine);
				m_pOutArrayClrKLine = formula_index(m_pFormularClrKLine, pData->m_aKLinesFull);

				UpdateShowClrKLine(pData, iShowPosInFullList, iShowCountInFullList);				
			}

			// ���½���ϵͳ
			if ( NULL != m_pFormularTrade )
			{
				DEL(m_pOutArrayTrade);
				m_pOutArrayTrade = formula_index(m_pFormularTrade, pData->m_aKLinesFull);
			}

			//
			if ( BeNeedZLMM(pMainData) )
			{
				SpecialForZLMM(pMainData);				
			}

			// ���ɵ�ǰ��ʾ������
			bool32 bUpdateResult =	true;
			if (!pData->UpdateShowData(iShowPosInFullList, iShowCountInFullList))
			{
				//ASSERT(0);
				bUpdateResult = false;
			}

			// ���������Сֵ
			CalcLayoutRegions(false);

			// ר�ҽ���ָ��Ĵ���
			if ( NULL != m_pExpertTrade )
			{
				if ( NULL != m_pMerchXml->m_pFinanceData )
				{
					m_pExpertTrade->SetCircStockValue(m_pMerchXml->m_pFinanceData->m_fCircAsset);
				}

				m_pExpertTrade->UpdateAll(pData->m_aKLinesFull);
			}

			//
			if ( !bUpdateResult )
			{
				return false;
			}

			if ( bNeedRedrawAftCalc )
			{
				ReDrawAysnc();
			}

			FireIoViewChouMa(3, 0, 0);		// ֪ͨ���¼�������Ʒk������
		}

		// zhangbo 20090714 #�����䣬 ������Ʒ���ݸ���
		//...
		// ���������г��ڶ�����õ��ˣ���������TODO

		return true;		
	}

	return false;
}

bool32 CIoViewKLine::UpdateSubMerchKLine(T_MerchNodeUserData &MerchNodeUserData, bool32 bForceUpdate/*=true*/)
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
	_LogCheckTime("[==>Client] CIoViewKLine::UpdateSubMerchKLine ��ʼ", g_hwndTrace);

	{
		// ����ʱ���¶�����ɾ��
		RemoveCmpMerchFromNeedUpdate(pMerch);
	}

	//
	E_NodeTimeInterval eNodeTimeIntervalCompare = ENTIDay;
	E_KLineTypeBase    eKLineType			    = EKTBDay;

	int32 iScale = 1;
	if (!GetTimeIntervalInfo(MerchNodeUserData.m_eTimeIntervalCompare, eNodeTimeIntervalCompare, eKLineType, iScale))
	{
		// ��Ӧ�ó����������
		return false;
	}

	// �жϵ�ǰ��ʾ�� �Ƿ���ʾ���¼�
	CGmtTime TimeStartInFullList, TimeEndInFullList;

	/*bool32 bShowNewestKLine		= */IsShowNewestKLine(TimeStartInFullList, TimeEndInFullList);
	int32 iShowPosInFullList	= MerchNodeUserData.m_iShowPosInFullList;
	int32 iShowCountInFullList	= MerchNodeUserData.m_iShowCountInFullList;

	// �Ȼ�ȡ��Ӧ��K��
	int32 iPosFound;
	CMerchKLineNode* pKLineRequest = NULL;
	pMerch->FindMerchKLineNode(eKLineType, iPosFound, pKLineRequest);

	// �����Ҳ���K�����ݣ� �ǾͲ���Ҫ��ʾ��
	if (NULL == pKLineRequest || 0 == pKLineRequest->m_KLines.GetSize())	
	{
		MerchNodeUserData.m_aKLinesCompare.RemoveAll();
		MerchNodeUserData.m_aKLinesFull.RemoveAll();

		if (NULL != MerchNodeUserData.m_pKLinesShow)		
			MerchNodeUserData.m_pKLinesShow->RemoveAll();

		MerchNodeUserData.m_iShowPosInFullList = MerchNodeUserData.m_iShowCountInFullList = 0;

		return true;
	}

	int32 iCmpResult = bForceUpdate ? 1 : CompareKLinesChange(pKLineRequest->m_KLines, MerchNodeUserData.m_aKLinesCompare); // ԭʼ���ݱȽ�
	CArray<CKLine, CKLine> aFullKlines;

	// �п������ڻ���ȨϢ��Ϣ�����������Ҫ���ºϳ�k��
	if ( iCmpResult != 0 )
	{
		// ��Ҫ���ºϳ�K��
		// ���ԭʼ&��ʾ����
		MerchNodeUserData.m_aKLinesCompare.Copy(pKLineRequest->m_KLines);	// ����һ��ԭʼ����

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
				if (!CMerchKLineNode::CombinMinuteN(pMerch, MerchNodeUserData.m_iTimeUserMultipleMinutes, 
					aSrcKLines, aFullKlines))
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
	// ������ʾ����
	// ��ʱ����K�߼�����������ͼ�Ѿ����������ʾ���䣬Copy��ͼҪ���ʱ�����K�ߵ�������������
	// ��ǰIsShowNewest���ص�ʱ�����Ҫ��ʾ������

	// ����[first, last]
	int32 iFirstNode = 0;
	int32 iLastNode = -1;

	iFirstNode = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(MerchNodeUserData.m_aKLinesFull, TimeStartInFullList);
	iLastNode = CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(MerchNodeUserData.m_aKLinesFull, TimeEndInFullList);

	if ( iLastNode >= iFirstNode
		&& iFirstNode >= 0
		&& iLastNode < MerchNodeUserData.m_aKLinesFull.GetSize() )
	{
		const int32 iShowCount = iLastNode - iFirstNode + 1;

		// ������ʾ����
		if ( bUpdateShowData
			|| iFirstNode != iShowPosInFullList
			|| iShowCount != iShowCountInFullList )
		{
			MerchNodeUserData.UpdateShowData(iFirstNode, iShowCount);
		}

		return true;		
	}
	else
	{
		// �Ѿ�û�и����ε���ʾ������, ������ʾ���ݣ�ʵ�����ݱ���
		//MerchNodeUserData.m_aKLinesCompare.RemoveAll();
		//MerchNodeUserData.m_aKLinesFull.RemoveAll();

		if (NULL != MerchNodeUserData.m_pKLinesShow)		
			MerchNodeUserData.m_pKLinesShow->RemoveAll();

		MerchNodeUserData.m_iShowPosInFullList = MerchNodeUserData.m_iShowCountInFullList = 0;
	}
	// ���۽����Σ���ˢ����ʾ��ʱ�����������Сֵ
	return false;	// ����������ЧK��
}

bool32 CIoViewKLine::OnCanPanLeft ( CChartRegion* pRegion, CNodeSequence* pNodes, int32 id, int32 iNum )
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

bool32 CIoViewKLine::OnCanPanRight ( CChartRegion* pRegion, CNodeSequence* pNodes, int32 id, int32 iNum )
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

bool32 CIoViewKLine::GetTips(IN CPoint pt, OUT CString& StrTips, OUT CString &StrTitle)
{
	// �õ�Tips
	StrTips = L"";
	//bool32 bSaneTip = false;

	{	
		if ( m_aSaneIndexValues.GetSize() < 1 )
		{
			//return false;
		}
		else
		{
			if ( m_bShowSaneIndexNow )
			{
				for ( int32 i = 0; i < m_aSaneNodeTips.GetSize(); i++ )
				{
					const T_SaneNodeTip &NodeTip = m_aSaneNodeTips.GetAt(i);

					if ( NodeTip.m_RectSign.PtInRect(pt) )
					{
						return GetSaneNodeTip(StrTips, NodeTip.m_iSaneIndex);
					}
				}
			}
		}		
	}

	// ��Ȩ
	{
		for ( int32 i=0; i < m_aWeightRects.GetSize() ; i++ )
		{
			if ( m_aWeightRects[i].PtInRect(pt) && NULL != m_pMerchXml && m_pMerchXml->m_aWeightDatas.GetSize() > i )
			{
				const CWeightData &wData = m_pMerchXml->m_aWeightDatas[i];
				if ( IsWeightDataCanShow(wData) )
				{
					CString StrTitleTmp, StrTipsTmp;
					StrTitleTmp = _T("ȨϢ���ϱ䶯");


					StrTipsTmp.Format(_T("��Ȩ,��Ϣ: %04d/%02d/%02d\r\n"), wData.m_TimeCurrent.GetYear(), wData.m_TimeCurrent.GetMonth(), wData.m_TimeCurrent.GetDay());

					// ������Щ������Ҫ�����
					CStringArray aNames;
					CArray<float, float> aValues, aScales;
					aNames.Add(_T("ÿ10���� %.2f ��"));
					aNames.Add(_T("ÿ10���� %.2f ��"));
					aNames.Add(_T("  ��ɼ� %.2f Ԫ"));
					aNames.Add(_T("ÿ10������ %.2f Ԫ"));
					aNames.Add(_T("ÿ10��ת�� %.2f ��"));
					aValues.Add(wData.B03);
					aValues.Add(wData.B04);
					aValues.Add(wData.B02);
					aValues.Add(wData.B01);
					aValues.Add(wData.B05);
					aScales.Add(10000.0f);
					aScales.Add(10000.0f);
					aScales.Add(1000.0f);	//
					aScales.Add(1000.0f);
					aScales.Add(10000.0f);
					ASSERT( aNames.GetSize() == aValues.GetSize() && aScales.GetSize() == aValues.GetSize()   );

					for ( int32 j=0; j < aValues.GetSize() ; j++ )
					{
						if ( aValues[j] > 0.0 )
						{
							CString StrValue;
							StrValue.Format(aNames[j], aValues[j]/aScales[j]);
							StrTips += StrValue + _T("\r\n");
						}
					}

					if ( StrTips.GetLength() > 0 )
					{
						StrTips = StrTipsTmp + StrTips;
						StrTitle = StrTitleTmp;
					}
				}

				return !StrTips.IsEmpty();
			}
		}
	}

	// Ͷ���ռ�
	{
		if ( m_mapUserNotesRect.GetCount() > 0 )
		{
			POSITION pos = m_mapUserNotesRect.GetStartPosition();
			CArray<int32, int32> aIds;
			while ( NULL != pos )
			{
				int32 iUniqueId;
				CRect rc;
				m_mapUserNotesRect.GetNextAssoc(pos, iUniqueId, rc);
				if ( rc.PtInRect(pt) )
				{
					aIds.Add(iUniqueId);
				}
			}

			if ( aIds.GetSize() > 0 )
			{
				CUserNoteInfoManager::UserNotesArray aNotes;
				CUserNoteInfoManager::Instance().GetUserNotesHeaderArray(m_MerchXml, aNotes);

				for ( int32 i=0; i < aNotes.size() ; i++ )
				{
					for ( int32 j=0; j < aIds.GetSize() ; j++ )
					{
						if ( aIds[j] == aNotes[i].m_iUniqueId )
						{
							StrTips += aNotes[i].m_StrTitle + _T("\r\n");
							if ( StrTitle.IsEmpty() )
							{
								CTime Time(aNotes[i].m_Time.GetTime());
								StrTitle.Format(_T("Ͷ���ռ�:%04d/%02d/%02d/%s"), aNotes[i].m_Time.GetYear(), aNotes[i].m_Time.GetMonth(), aNotes[i].m_Time.GetDay(), GetWeekString(Time).GetBuffer());
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
						CString StrTime = Mines.first->first;
						int32 iSep = -1;
						if ( (iSep=StrTime.Find(_T(' ')))>=0 )
						{
							StrTime = StrTime.Left(iSep);
						}
						if ( StrTitle.IsEmpty() )
						{
							//StrTitle.Format(_T("��ʷ��Ϣ����: %s"), StrTime);
							StrTitle = _T("��ʷ��Ϣ����(˫���鿴)");
						}
						// ��������˳������µ���ǰ
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


	// �����ź�
	{
		if ( m_mulmapStategyRects.size() > 0 )
		{
			CStringArray aStrMineTimes;
			for ( StategyRectMulmap::iterator it = m_mulmapStategyRects.begin(); it != m_mulmapStategyRects.end() ; ++it )
			{
				if ( it->second.PtInRect(pt) )
				{
					//  ֻ����һ�����ݵĲ�����Ϣ����һ���ڵ��ж����Ϣ���õ���¼�����
					if (1 == m_mulmapStategyInfo.count(it->first))
					{
						StrTitle	 = _T("���Խ�����Ϣ");
						StategyInfoMulmap::iterator iter = m_mulmapStategyInfo.find(it->first);
						if (iter != m_mulmapStategyInfo.end())
						{
							T_StrategyInfo stStrategyInfo = iter->second;
							CString strPrint;
							strPrint.Format(_T("��������  %s\n"), stStrategyInfo.strStrategyName);
							StrTips += strPrint;
							CTime time(stStrategyInfo.llStrategyTime);
							strPrint.Format(_T("����ʱ��  %02d-%02d %02d:%02d\n"), time.GetMonth(), time.GetDay(),time.GetHour(),time.GetSecond());
							StrTips += strPrint;
							CString strSignalType;
							strSignalType = ((stStrategyInfo.ulSignalType==0)?_T("����"):_T("����"));
							strPrint.Format(_T("���׷���  %s\n"), strSignalType);
							StrTips += strPrint;
							strPrint.Format(_T("����۸�  %.2f\n"), stStrategyInfo.SugPrice);
							StrTips += strPrint;
						}
					}
					return TRUE;
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

void CIoViewKLine::OnNodesRelease ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes )
{
	//T_MerchNodeUserData* pData = (T_MerchNodeUserData*)pNodes->GetUserData();
	//pData->pMerch�Ѿ��������Curveʹ����,���û������Curveʹ��,�����������,���Ҵ�AttendMerch��ɾ��.
}

void CIoViewKLine::OnRegionMenu2 ( CChartRegion* pRegion, CDrawingCurve* pCurve, int32 x, int32 y )
{
	RegionMenu2 ( pRegion, pCurve, x, y );
}

void CIoViewKLine::OnRegionIndexMenu ( CChartRegion* pRegion, CDrawingCurve* pCurve, int32 x, int32 y )
{
	RegionIndexMenu ( pRegion, pCurve, x, y );
}

void CIoViewKLine::OnRegionIndexBtn(CChartRegion* pRegion, CDrawingCurve* pCurve, int32 iID )
{
	RegionIndexBtn (pRegion, pCurve, iID);
}

//
void GetCharacternNums(const CString& StrIn, int32& iEngNums, int32& iChnNums)
{
	iEngNums = 0;
	iChnNums = 0;
	if ( 0 == StrIn.GetLength() )
	{
		return;
	}

	//
	for(int32 i = 0; i < StrIn.GetLength(); i++)   		
	{   	
		TCHAR ch = StrIn.GetAt(i);

		if( ch  <   0x80   )      			
		{   
			//�ַ�
			iEngNums += 1;
		}   
		else   
		{
			//����
			//��λ��i��i++Ϊһ������ 		
			iChnNums+= 1;
		}   		
	}		
}

void FillCStringWithSpace(int32 iSpaceNum, CString& StrValue )
{
	if( iSpaceNum < 0)
		return;

	for ( int32 i = 0; i < iSpaceNum; i++)
	{
		StrValue += L" ";
	}
}

CString MakeMenuString(const CString& StrName, const CString& StrExplain, bool32 bAppendSpace=true)
{
	CString StrReturn = L"";
	// �����ַ���
	const int32 KiNameLen	 = 7;			// ǰ�� 10 �����ֵĿ��
	const int32 KiExplainLen = 7;			// ���� 10 �����ֵĿ��

	//int32 iLenAll = KiNameLen + KiExplainLen;	// �ܹ�

	//int32 iNameLen	= StrName.GetLength();		// ǰ��ĳ���
	//int32 iExplainLen = StrExplain.GetLength();	// ����ĳ���

	//int32 iLenLeft = iLenAll - iExplainLen;		// ʣ�µĳ���

	//
	int32 iNumNameEngChar = 0;
	int32 iNumNameChnChar = 0;
	GetCharacternNums(StrName, iNumNameEngChar, iNumNameChnChar);

	int32 iNameSpace = 2*KiNameLen - ( iNumNameEngChar + 2*iNumNameChnChar);

	if ( iNameSpace <= 0 )
	{
		StrReturn  = StrName + L" ";
		StrReturn += StrExplain;

		return StrReturn;
	}

	int32 iNumExpEngChar = 0;
	int32 iNumExpChnChar = 0;
	GetCharacternNums(StrExplain, iNumExpEngChar, iNumExpChnChar);

	int32 iExpSpace = 2*KiExplainLen - ( iNumExpEngChar + 2*iNumExpChnChar);

	// 
	CString StrNameTemp = StrName;

	if ( bAppendSpace || !StrExplain.IsEmpty() )
	{
		FillCStringWithSpace( ( iNameSpace + iExpSpace ),  StrNameTemp);	
	}

	StrReturn = StrNameTemp + StrExplain;

	return StrReturn;
}

void CIoViewKLine::GetOftenFormulars(IN CChartRegion* pRegion, OUT CStringArray& aFormulaNames, OUT int32& iSeperatorIndex, bool32 bDelSame /*= true*/)
{
	CStringArray subFormulaNames;

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
	/*
	if ( pRegion == m_pRegionMain )
	{
	AddFlag(iFlag, CFormularContent::KAllowMain);
	}
	else
	{
	AddFlag(iFlag, CFormularContent::KAllowSub);
	}
	*/
	//
	AddFlag(iFlag, CFormularContent::KAllowMain);
	E_IndexStockType eIST = GetMerchKind(m_pMerchXml) == ERTExp ? EIST_Index : EIST_Stock;

	CFormulaLib::instance()->GetAllowNames(iFlag, aFormulaNames, eIST);

	//��ȡ��ͼindex 2013-5-4-3
	RemoveFlag(iFlag, CFormularContent::KAllowMain);
	AddFlag(iFlag, CFormularContent::KAllowSub);
	CFormulaLib::instance()->GetAllowNames(iFlag, subFormulaNames, eIST);

	if ( bDelSame )
	{
		CStringArray Formulas;
		FindRegionFormula(pRegion, Formulas);

		DeleteSameString (aFormulaNames, Formulas);
		Formulas.RemoveAll();	
	}

	// ���ɽ������͵�����, ���Ϸָ���
	//SortIndexByVol(aFormulaNames, iSeperatorIndex);

	//SortIndexByVol(subFormulaNames, iSeperatorIndex);

	SortIndexByAlpha(aFormulaNames);
	SortIndexByAlpha(subFormulaNames);

	//����ͼָ��ӵ�aFormulaNames  2013-5-3 add by cym
	if (subFormulaNames.GetSize() > 0)
	{
		aFormulaNames.Add(L"Seperator");
		aFormulaNames.Append(subFormulaNames);
	}
}

int32 CIoViewKLine::LoadOftenIndex(CNewMenu* pMenu)
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

	bool32 bIsExp = false;
	if ( NULL != pData && NULL != pData->m_pMerchNode )
	{
		bIsExp = GetMerchKind(pData->m_pMerchNode) == ERTExp;
	}

	int32 iIndexNums = 0;

	if ( m_pRegionMain == m_pRegionPick )
	{
		// ��ͼ����ʾ:		
		if ( !m_bShowTrendIndexNow )
		{
			//iIndexNums += 1;
			//pMenu->AppendODMenu(L"�������", MF_STRING, IDM_KLINE_TREND_INDEX);	
		}

		if ( !m_bShowSaneIndexNow )
		{
			iIndexNums += 1;
			pMenu->AppendODMenu(GetSaneIndexShowName(), MF_STRING, IDM_KLINE_SANE_INDEX);
		}

		//2013-5-3 ���һ���ͼ����Ĭ��ѡ�е��ǵ�һ����ͼ add by cym
		m_iLastPickSubRegionIndex = 0;

	}

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
		//�˵���ͼ�븱ͼ������
		if (0 == StrName.CompareNoCase(L"Seperator"))
		{
			pMenu->AppendODMenu(L"",MF_SEPARATOR);
			continue;
		}

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

		CString StrIndexFull = MakeMenuString(StrText, StrexplainBrief);

		pMenu->AppendODMenu(StrIndexFull, MF_STRING, IDM_IOVIEWKLINE_OFTENINDEX_BEGIN + i);	
	}

	return iIndexNums;
}

int32 CIoViewKLine::LoadJinDunSpecialIndex( CNewMenu *pMenu )
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
	CFormulaLib::instance()->GetJinDunSpecialIndexNames(m_FormulaNames);
	CStringArray Formulas;
	FindRegionFormula(m_pRegionMain, Formulas);

	DeleteSameString (m_FormulaNames, Formulas);
	Formulas.RemoveAll();

	bool32 bIsExp = false;
	if ( NULL != pData && NULL != pData->m_pMerchNode )
	{
		bIsExp = GetMerchKind(pData->m_pMerchNode) == ERTExp;
	}

	int32 iIndexNums = 0;

	if ( m_pRegionMain == m_pRegionPick )
	{
		// ��ͼ����ʾ:		
		if ( !m_bShowTrendIndexNow )
		{
			//iIndexNums += 1;
			//pMenu->AppendODMenu(L"�������", MF_STRING, IDM_KLINE_TREND_INDEX);	
		}

		if ( !m_bShowSaneIndexNow )
		{
			iIndexNums += 1;
			pMenu->AppendODMenu(GetSaneIndexShowName(), MF_STRING, IDM_KLINE_SANE_INDEX);
		}
	}

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

		CString StrIndexFull = MakeMenuString(StrText, StrexplainBrief);

		pMenu->AppendODMenu(StrIndexFull, MF_STRING, IDM_IOVIEWKLINE_OFTENINDEX_BEGIN + i);	
	}

	return iIndexNums;
}

bool32	CIoViewKLine::KlineLoadAllIndex(CNewMenu* pMenu)
{
	CStringArray subMens;//��ͼ
	CStringArray mainMens;
	map<CString, CString>  subFullNames;;//��ͼ
	map<CString, CString>  mainFullNames;

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

	if ( bMainRegion )
	{	 		
		// ��ͼ����ʾ:		
		//if ( !m_bShowTrendIndexNow )
		//{
		//	pMenu->AppendODMenu(L"�������   ", MF_STRING, IDM_KLINE_TREND_INDEX);	
		//}

		if ( !m_bShowSaneIndexNow )
		{
			pMenu->AppendODMenu(GetSaneIndexShowName(), MF_STRING, IDM_KLINE_SANE_INDEX);
		}

	}

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
			mainMens.RemoveAll();
			subMens.RemoveAll();
			subFullNames.clear();
			mainFullNames.clear();

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

					if ( (!CheckFlag(pContent->flag, CFormularContent::KAllowMain)) && (!CheckFlag(pContent->flag, CFormularContent::KAllowSub)) )
					{
						continue;
					}
					/*
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
					*/
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
						/*
						pSubMenu->AppendODMenu(StrIndexFull, MF_STRING, IDM_IOVIEWKLINE_ALLINDEX_BEGIN + iIndex );

						iIndex += 1;
						m_aAllFormulaNames.Add(StrText);
						*/
						if (CheckFlag(pContent->flag, CFormularContent::KAllowMain))
						{
							mainMens.Add(StrText);
							mainFullNames.insert( std::pair<CString, CString>(StrText, StrIndexFull) );
						}
						else
						{
							subMens.Add(StrText);
							subFullNames.insert( std::pair<CString, CString>(StrText, StrIndexFull) );
						}
					}					
				}

				if (mainMens.GetSize() > 1) //����
				{
					SortIndexByAlpha(mainMens);
				}

				if (subMens.GetSize() > 1)
				{
					SortIndexByAlpha(subMens);
				}

				CString  strIndex;

				for (int m=0; m < mainMens.GetSize(); ++m) //������˵�
				{
					strIndex = mainMens.GetAt(m);
					map<CString,CString>::iterator funIt = mainFullNames.find(strIndex);

					pSubMenu->AppendODMenu(funIt->second, MF_STRING, IDM_IOVIEWKLINE_ALLINDEX_BEGIN + iIndex );

					iIndex += 1;
					m_aAllFormulaNames.Add(strIndex);
				}

				if (mainMens.GetSize() >0 && subMens.GetSize() > 0) //��ӷָ���
				{
					pSubMenu->AppendODMenu(L"",MF_SEPARATOR);;
				}

				for (int n=0; n < subMens.GetSize(); ++n) //��Ӹ��˵�
				{
					strIndex = subMens.GetAt(n);
					map<CString,CString>::iterator funIt = subFullNames.find(strIndex);

					pSubMenu->AppendODMenu(funIt->second, MF_STRING, IDM_IOVIEWKLINE_ALLINDEX_BEGIN + iIndex );

					iIndex += 1;
					m_aAllFormulaNames.Add(strIndex);
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
					if ( (!CheckFlag(pContent->flag, CFormularContent::KAllowMain)) && (!CheckFlag(pContent->flag, CFormularContent::KAllowSub)) )
					{
						continue;
					}

					/*
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
					*/
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

void CIoViewKLine::OnRegionMenu ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes,CNodeData* pNodeData, int32 x, int32 y)
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
	if ( NULL == pMerch)
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
		menu.AppendMenu(MF_STRING, uBase+CPriceToAxisYObject::EAYT_Log,	   _T("��������"));

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
	menu.LoadMenu(IDR_MENU_KLINE);
	menu.LoadToolBar(g_awToolBarIconIDs);

	CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
	CMenu* pTempMenu = NULL;
	CNewMenu* pSubMenu = NULL;

	//////////////////////////////////////////////////////////////////////////
	pTempMenu = pPopMenu->GetSubMenu(_T("����ָ��"));
	ASSERT(NULL!=pTempMenu);

	CNewMenu* pAllIndexMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );	
	KlineLoadAllIndex(pAllIndexMenu);

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
		pSubMenu = NULL;
		pTempMenu = NULL;
	}

	// ר����ʾ
	pTempMenu = pPopMenu->GetSubMenu(L"ר����ʾ");
	if ( NULL != pTempMenu )
	{
		pTempMenu = ((CNewMenu*)pTempMenu)->GetSubMenu(L"ר�ҽ�����ʾ");

		if ( NULL != pTempMenu )
		{
			CNewMenu* pExpertTradeMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu);

			for ( int32 i = (int32)CExpertTrade::EETTJglt; i < (int32)CExpertTrade::EETTCount; i++ )
			{
				CString StrName = CExpertTrade::GetTradeString((CExpertTrade::E_ExpertTradeType)i);
				pExpertTradeMenu->AppendODMenu(StrName, MF_STRING, IDM_IOVIEWKLINE_EXPERT_BEGIN + i);

				if ( NULL != m_pExpertTrade && (int32)m_pExpertTrade->GetTradeType() == i )
				{
					// �����
					pExpertTradeMenu->CheckMenuItem(IDM_IOVIEWKLINE_EXPERT_BEGIN + i, MF_BYCOMMAND | MF_CHECKED);
				}
			}

			pExpertTradeMenu->RemoveMenu(0, MF_BYPOSITION|MF_SEPARATOR);
		}
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

	// ��ͼ����
	if ( m_pRegionPick == m_pRegionMain )
	{
		// ��
		pTempMenu = pPopMenu->GetSubMenu(L"��ͼ����");

		if ( NULL != pTempMenu )
		{
			CNewMenu* pMainKDStyleMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu);

			pMainKDStyleMenu->CheckMenuItem(IDM_CHART_KSTYLE1+GetMainKlineDrawStyle(), MF_CHECKED|MF_BYCOMMAND);
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

	// ��ʷ��ʱ
	if ( ENTIDay != GetTimeInterval() )
	{
		pPopMenu->RemoveMenu(ID_KLINE_HISTORYTREND, MF_BYCOMMAND);
	}

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
	ASSERT(NULL!=pTempMenu);
	CNewMenu * pIoViewPopMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );
	AppendIoViewsMenu(pIoViewPopMenu, IsLockedSplit());

	// ��������
	pTempMenu = pPopMenu->GetSubMenu(_T("ѡ������"));
	ASSERT(NULL!=pTempMenu);
	pSubMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );

	i = IDM_CHART_KMINUTE + pData->m_eTimeIntervalFull;
	pSubMenu->CheckMenuItem(i,MF_BYCOMMAND|MF_CHECKED); 

	// ��Ȩ��Ȩ
	pTempMenu = pPopMenu->GetSubMenu(_T("��Ȩ����"));
	ASSERT(NULL != pTempMenu);
	pSubMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );
	pSubMenu->CheckMenuItem((IDM_PRE_WEIGHT+m_eWeightType), MF_BYCOMMAND|MF_CHECKED);

	if (ERTStockCn != pMerch->m_Market.m_MarketInfo.m_eMarketReportType)
	{
		pPopMenu->DeleteMenu((UINT)pTempMenu->GetSafeHmenu(), MF_BYCOMMAND);
	}

	m_pExportNodes = NULL;
	CString StrExport;
	if ( NULL != m_pCurvePick && CheckFlag(m_pCurvePick->m_iFlag,CChartCurve::KTypeKLine))
	{
		CNodeSequence* pNodesi = m_pCurvePick->GetNodes();
		if ( NULL != pNodesi )
		{
			m_pExportNodes = pNodesi;
			T_MerchNodeUserData* pDatai = (T_MerchNodeUserData*)pNodesi->GetUserData();
			if ( NULL != pDatai )
			{
				CMerch* pMerchi = pDatai->m_pMerchNode;
				if ( NULL != pMerchi )
				{
					StrExport = _T("����[") + pMerchi->m_MerchInfo.m_StrMerchCnName + _T("]��Excel");
				}
			}
		}
	}
	if ( 0 == StrExport.GetLength() )
	{
		CChartCurve *pCurveDependent = m_pRegionMain->GetDependentCurve();
		if (NULL == pCurveDependent)
			return;

		m_pExportNodes = pCurveDependent->GetNodes();
		StrExport = _T("����[") + pMerch->m_MerchInfo.m_StrMerchCnName + _T("]��Excel");
	}
	menu.ModifyODMenu(IDM_CHART_PROP,MF_BYCOMMAND|MF_STRING,IDM_CHART_PROP,StrExport);

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

	// ��ʱ����ָ��༭���� 0001734
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
		// 		menu.RemoveMenu(IDM_CHART_INDEX,MF_BYCOMMAND);
		// 		menu.RemoveMenu(IDM_CHART_INDEX_PARAM,MF_BYCOMMAND);
		// 		menu.RemoveMenu(IDM_CHART_INDEX_HELP,MF_BYCOMMAND);
	}

	//  	if ( NULL != m_pCurvePick &&
	//  		 CheckFlag(m_pCurvePick->m_iFlag,CChartCurve::KTypeKLine) &&
	//  		 m_pCurvePick != m_pRegionMain->GetDependentCurve() )
	//  	{ 		
	//  		menu.ModifyODMenu(IDM_CHART_DELETE_CURVE,MF_BYCOMMAND|MF_STRING,IDM_CHART_DELETE_CURVE,_T("ɾ������"));
	//  	}
	// 	else
	// 	{
	// 		menu.RemoveMenu(IDM_CHART_DELETE_CURVE,MF_BYCOMMAND);
	// 	}


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

void CIoViewKLine::UpdateKLineStyle()
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

void CIoViewKLine::OnUserCircleChanged(E_NodeTimeInterval eNodeInterval, int32 iValue)
{
	if ( m_MerchParamArray.GetSize() <= 0 )
	{
		return;
	}

	SetTopButtonStatus(eNodeInterval, iValue);

	for ( int32 i=0; i < m_MerchParamArray.GetSize() ; ++i )
	{
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(i);

		if ( NULL == pData)
		{
			continue;
		}

		if ( ENTIMinuteUser == eNodeInterval)
		{
			if ( iValue >= USERSET_MINUET_MIN && iValue <= USERSET_MINUET_MAX )
			{
				pData->m_iTimeUserMultipleMinutes = iValue;
			}
		}
		else if (ENTIDayUser == eNodeInterval)
		{
			if ( iValue >= USERSET_DAY_MIN && iValue < USERSET_DAY_MAX )
			{
				pData->m_iTimeUserMultipleDays = iValue;
			}
		}

		if ( pData->bMainMerch )
		{
			UpdateMainMerchKLine(*pData);
		}
		else
		{
			UpdateSubMerchKLine(*pData);
		}
		SetCurveTitle(pData);
	}

	ReDrawAysnc();
}

void CIoViewKLine::OnAllIndexMenu(UINT nID)
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

	AddShowIndex(StrName, false, false, true);
}

void CIoViewKLine::OnOftenIndexMenu(UINT nID)
{
	// ����ָ��
	int32 iIndex = nID - IDM_IOVIEWKLINE_OFTENINDEX_BEGIN;

	if ( iIndex < 0 || iIndex >= m_FormulaNames.GetSize() )
	{
		return;
	}

	CString StrName = m_FormulaNames.GetAt(iIndex);
	AddShowIndex(StrName, false, false, true);
}


bool32 CIoViewKLine::AddShowIndex( const CString &StrIndexName, bool32 bDelRegionAllIndex /*= false*/, bool32 bChangeStockByIndex /*= false*/, bool32 bShowUserRightDlg/*=false*/ )
{
	T_MerchNodeUserData* pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == m_pRegionMain || NULL == pData )
	{
		return false;
	}

	CFormularContent *pFormulaContent = CFormulaLib::instance()->GetFomular(StrIndexName);
	if ( NULL == pFormulaContent )
	{
		ASSERT( 0 );
		return false;	// �޴�ָ��
	}

	if ( !CPluginFuncRight::Instance().IsUserHasIndexRight(StrIndexName, bShowUserRightDlg) )
	{
		return false;	// ��Ȩ��
	}

	if ( NULL == m_pMerchXml )
	{
		bChangeStockByIndex = true;	// ����Ʒ���л�
	}

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

	if ( bChangeStockByIndex && NULL != m_pAbsCenterManager )
	{
		// ��Ҫ������ʵ���Ʒ
		// ����ָ�������ָ�� ����Ĭ����֤A(0-000001)������Ĭ���ַ�(1-600000)
		CMerch *pMerchNow = GetMerchXml();
		if ( pFormulaContent->m_byteIndexStockType == EIST_Index )
		{
			// ����ָ��
			bool32 bNeedChange = true;
			CMerch *pMerchChange = NULL;
			if ( NULL != pMerchNow )
			{
				E_ReportType eMerchType = GetMerchKind(pMerchNow);
				if ( eMerchType == ERTExp )
				{
					bNeedChange = false;
				}
			}
			if ( bNeedChange )
			{
				m_pAbsCenterManager->GetMerchManager().FindMerch(_T("000001"), 0, pMerchChange);
				pFrame->OnViewMerchChanged(this, pMerchChange);
			}
		}
		else if ( pFormulaContent->m_byteIndexStockType == EIST_Stock )
		{
			// ����ָ��
			bool32 bNeedChange = true;
			CMerch *pMerchChange = NULL;
			if ( NULL != pMerchNow )
			{
				E_ReportType eMerchType = GetMerchKind(pMerchNow);
				if ( eMerchType != ERTExp )		// ��ָ���������
				{
					bNeedChange = false;
				}
			}
			if ( bNeedChange )
			{
				m_pAbsCenterManager->GetMerchManager().FindMerch(_T("600000"), 1, pMerchChange);
				pFrame->OnViewMerchChanged(this, pMerchChange);
			}
		}
		else
		{
			// ����������Ʒ���л����ַ�
			if ( NULL == pMerchNow )
			{
				CMerch *pMerchChange = NULL;
				m_pAbsCenterManager->GetMerchManager().FindMerch(_T("600000"), 1, pMerchChange);
				pFrame->OnViewMerchChanged(this, pMerchChange);
			}
		}
	}

	if ( pFormulaContent->m_eFormularType == EFTClrKLine )
	{
		if ( !bDelRegionAllIndex )
		{
			// ���ҵ�ǰָ���Ƿ���drawKline���ܣ�����������
			for ( int32 j=0; j < m_MerchParamArray.GetSize() ; j++ )
			{
				T_MerchNodeUserData *pTmpData = m_MerchParamArray[j];
				if ( NULL == pTmpData )
				{
					continue;;
				}

				for ( int32 i = pTmpData->aIndexs.GetSize()-1 ; i >= 0 ; i-- )
				{
					T_IndexParam *pParam = pTmpData->aIndexs[i];
					if ( NULL != pParam 
						&& pParam->pRegion == m_pRegionMain
						&& pParam->pContent != NULL )	// �����region�����а���DrawKlineָ��
					{
						CString StrFormula = pParam->pContent->formular;
						StrFormula.MakeUpper();
						if ( StrFormula.Find(_T("DRAWKLINE")) != -1 )
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

			}
		}
		if ( bDelRegionAllIndex )
		{
			ClearRegionIndex(m_pRegionMain, true);	// �����ͼָ��
		}
		AddClrKLine(StrIndexName);
	}
	else if ( pFormulaContent->m_eFormularType == EFTTradeChose )
	{
		// ����ָ��
		if ( bDelRegionAllIndex )
		{
			ClearRegionIndex(m_pRegionMain, true);	// �����ͼָ��
		}
		DoAddTrade(pFormulaContent);
	}
	else
	{
		if ( CheckFlag(pFormulaContent->flag, CFormularContent::KAllowMain) )
		{
			if ( bDelRegionAllIndex )
			{
				ClearRegionIndex(m_pRegionMain, true);	// �����ͼָ��
			}
			else
			{
				// �����ָ����drawkline������������ʳ�ͻ������Ҫ�޳�ר����� 
				CString StrFormula = pFormulaContent->formular;
				StrFormula.MakeUpper();
				if ( StrFormula.Find(_T("DRAWKLINE")) != -1 )
				{
					//
					DEL(m_pExpertTrade);

					// ������
					OnDelClrKLine();
				}
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


void CIoViewKLine::ReplaceIndex( const CString &StrOldIndex, const CString &StrNewIndex, bool32 bDelRegionAllIndex/* = false*/, bool32 bChangeStock/* = false*/, bool32 bDonotAddIfNotSameType/*=true*/ )
{
	// ������ھ�ָ�꣬����ɾ���þ�ָ��
	// �����ָ�����ָ�����Ͳ�ͬ(�޾�ָ�꣬����Ϊ��ͬ)����ָ��bDonotAddIfNotSameTypeΪtrue���������ָ��
	// ָ���Ƿ���Ч
	const CFormularContent *pFormulaContentOld = CFormulaLib::instance()->GetFomular(StrOldIndex);
	if ( NULL == pFormulaContentOld )
	{
		// �޾�ָ��, ��������ָ��
		AddShowIndex(StrNewIndex, bDelRegionAllIndex, bChangeStock);
		return;
	}
	ASSERT( NULL!=pFormulaContentOld );

	const CFormularContent *pFormulaContentNew = CFormulaLib::instance()->GetFomular(StrNewIndex);
	if ( NULL == pFormulaContentNew )
	{
		// ����ָ��
		return;
	}
	ASSERT( NULL!=pFormulaContentNew );

	if ( bDonotAddIfNotSameType 
		&& pFormulaContentNew->m_eFormularType != pFormulaContentOld->m_eFormularType
		)
	{
		RemoveShowIndex(StrOldIndex);	// ��ɾ��
		return;	// ���Ͳ� һ��
	}

	if ( !CPluginFuncRight::Instance().IsUserHasIndexRight(StrNewIndex, false) )
	{
		// Ȩ�޲���
		RemoveShowIndex(StrOldIndex);	// ��ɾ��
		return;
	}

	// �Ƿ������k��
	// ������ͼ���߶��Ǹ�ͼ
	if ( pFormulaContentOld->m_eFormularType == EFTClrKLine )
	{
		// ɾ�����
		OnDelClrKLine();
	}

	if ( pFormulaContentOld->m_eFormularType == EFTTradeChose )
	{
		// ɾ������
		OnDelTrade();
	}

	if ( NULL!=pFormulaContentNew )
	{
		if ( pFormulaContentNew->m_eFormularType == EFTClrKLine )
		{
			// �������
			AddShowIndex(StrNewIndex, bDelRegionAllIndex, bChangeStock);
			return;
		}
		else if ( pFormulaContentNew->m_eFormularType == EFTTradeChose )
		{
			// ����
			AddShowIndex(StrNewIndex, bDelRegionAllIndex, bChangeStock);
			return;
		}

		int iMainNew = CheckFlag(pFormulaContentNew->flag, CFormularContent::KAllowMain) ? 1 : 0;
		if ( bDonotAddIfNotSameType )
		{
			// �����Ƿ��ϸ�ƥ��
			int iMainOld = CheckFlag(pFormulaContentOld->flag, CFormularContent::KAllowMain) ? 1 : 0;
			if ( iMainNew != iMainOld )	// �ϸ�����Ӧ����ȣ������û������޸����ͣ���������������Ȼ��һ�����滻
			{
				RemoveShowIndex(StrOldIndex);
				return;
			}
		}

		if ( iMainNew != 0 )
		{
			RemoveShowIndex(StrOldIndex);
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
				// ��ԭ����ָ�꣬������ͼָ��
				AddShowIndex(StrNewIndex, bDelRegionAllIndex, bChangeStock);
			}
			else
			{
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
}

void CIoViewKLine::ChangeIndexToMainRegion( const CString &StrIndexName )
{
	T_MerchNodeUserData* pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == m_pRegionMain || NULL == pData )
	{
		ASSERT( 0 );
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
		ASSERT( 0 );
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

	m_bShowSaneIndexNow = false;
	m_bShowTrendIndexNow = false;
	T_IndexParam *pIndex = AddIndex(m_pRegionMain, StrIndexName);
	if ( NULL == pIndex )
	{
		ASSERT( 0 );		// ��ô��ȣ���
		return;
	}


	int32 iNodeBegin,iNodeEnd;
	GetNodeBeginEnd(iNodeBegin,iNodeEnd);
	g_formula_compute ( this, this, (CChartRegion*)pIndex->pRegion, pData, pIndex, iNodeBegin,iNodeEnd );

	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	UpdateSelfDrawCurve();
	Invalidate();
}


void CIoViewKLine::AddIndexToSubRegion( const CString &StrIndexName, bool32 bAddSameIfExist/* = true*/, int32 iSubRegionIndex/* = -1*/ )
{
	T_MerchNodeUserData* pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == m_pRegionMain || NULL == pData )
	{
		ASSERT( 0 );
		return;
	}

	int32 i = 0;

	CFormularContent *pFormulaContent = (CFormularContent *)CFormulaLib::instance()->GetFomular(StrIndexName);
	if ( NULL == pFormulaContent || !CheckFlag(pFormulaContent->flag, CFormularContent::KAllowSub) )
	{
		ASSERT( 0 );
		return;	// �޴�ָ���Ǹ�ͼָ�꣬ ������
	}

	if (NULL != pFormulaContent && pFormulaContent->BePassedXday())
	{
		MessageBox(L"ָ���Ѿ�����!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return;
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

void CIoViewKLine::AddIndexToSubRegion( const CString &StrIndexName, CChartRegion *pSubRegion )
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

	bool32 bExist = false;
	// ����Ƿ���ڸ�index - ��ô���ж��أ���
	for ( int32 i=0; i < pData->aIndexs.GetSize() ; i++ )
	{
		// ����ʵ�ʴ���indexҲ�Ǹ���name��������so���ж�����+region
		if ( pData->aIndexs[i]->strIndexName == StrIndexName && pSubRegion == pData->aIndexs[i]->pRegion )
		{
			bExist = true;
			break;
		}
	}

	// ɾ����ǰSubRegion��ָ��
	ClearRegionIndex(pSubRegion);

	if (bExist)
	{
		// ���ڵ�, ɾ�������ok ��
		return;
	}

	//
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

	if ( BeNeedZLMM(pData) )
	{
		UpdateMainMerchKLine(*pData);
	}
	else
	{
		m_aZLMMCrossID.RemoveAll();
	}

	RedrawWindow();
}

bool32 CIoViewKLine::CheckIndexExist(const CString &StrIndexName)
{
	T_MerchNodeUserData* pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
	if ( NULL == pData )
	{
		ASSERT( 0 );
		return false;
	}

	bool32 bExist = false;
	// ����Ƿ���ڸ�index 
	for ( int32 i=0; i < pData->aIndexs.GetSize() ; i++ )
	{
		if ( pData->aIndexs[i]->strIndexName == StrIndexName  )
		{
			bExist = true;
			break;
		}
	}
	return bExist;
}

void CIoViewKLine::RemoveShowIndex( const CString &StrIndexName )
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

	// ����ɾ�����ָ�ꣿ

	// ���ָ��
	if ( m_pFormularClrKLine != NULL
		&& m_pFormularClrKLine->name == StrIndexName )
	{
		OnDelClrKLine();
	}

	// ר����ʾ
	if ( NULL != m_pExpertTrade
		&& CExpertTrade::GetTradeString(m_pExpertTrade->GetTradeType()) == StrIndexName)
	{
		DEL(m_pExpertTrade);
		bRemove = true;
	}

	// ����ָ��
	if ( NULL != m_pFormularTrade
		&& m_pFormularTrade->name == StrIndexName)
	{
		OnDelTrade();
	}

	// ����ʲôָ����
	if ( bRemove )
	{
		ReDrawAysnc();
	}
}

void CIoViewKLine::AddIndexGroup(CString strIndexGroupName)
{
	if ( !CPluginFuncRight::Instance().IsUserHasIndexRight(strIndexGroupName, true) )
	{
		return ;	// ��Ȩ��
	}


	UpdateAxisSize(false);	// ��Ҫ����֮ǰ���ú��ʵ�x������߶�

	m_pRegionMain->NestSizeAll();

	vector<T_IndexGroupItem> vIndexGroupItem;
	vIndexGroupItem.clear();
	CConfigInfo::Instance()->GetIndexGroupItem(strIndexGroupName, vIndexGroupItem);

	int32 iSize = vIndexGroupItem.size();
	int32 iRegionSize = m_SubRegions.GetSize();
	for (int32 i = 0; i < iSize && i < (iRegionSize + 1); i ++ )
	{

		CString StrIndexName = vIndexGroupItem.at(i).strIndexName;
		int32 iRegion = vIndexGroupItem.at(i).iRegion;
		CChartRegion* pRegion = NULL;
		if ( iRegion == 0 )
		{
			pRegion = m_pRegionMain;
		}
		else
		{
			T_SubRegionParam SubParam = m_SubRegions.GetAt(iRegion-1);
			pRegion = SubParam.m_pSubRegionMain;
		}

		ClearRegionIndex(pRegion);
		bool32 bMainIndex = vIndexGroupItem.at(i).bRegionMainIndex;
		AddIndex( pRegion, StrIndexName, bMainIndex);
		SetSelectIndexGroupName(strIndexGroupName);
	}

	m_aRegionHeightXml.RemoveAll();
	m_aRegionHeightRadioXml.RemoveAll();
	m_IndexNameXml.RemoveAll();
	m_IndexRegionXml.RemoveAll();


	/*std::map<int, CPolygonButton>::iterator iter;
	for (iter = m_mapBottomBtn.begin(); iter!= m_mapBottomBtn.end(); ++iter)
	{
	if (iter->second.GetCaption() == strIndexGroupName)
	{
	iter->second.SetCheck(TRUE);
	}
	else
	{
	iter->second.SetCheck(false);
	}
	}*/

	// ǿ��ˢ��
	if (NULL != m_pMerchXml)
	{
		OnVDataMerchChanged(m_MerchXml.m_iMarketId, m_MerchXml.m_StrMerchCode, m_pMerchXml);
		OnVDataForceUpdate();
	}

	m_bFromXml = false;
}

void CIoViewKLine::ClearRegionIndex( CChartRegion *pRegion, bool32 bDelExpertTip/* = false*/, bool32 bDelCmpIndex )
{
	// ɾ�����е�ָ��
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
			if ( NULL != pParam && pParam->pRegion == pRegion )	// �����region������ָ��
			{
				if (!bDelCmpIndex && !pParam->bMainIndex)
				{
					continue;
				}

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

	if ( pRegion == m_pRegionMain )
	{
		if ( m_bShowTrendIndexNow )
		{
			m_bShowTrendIndexNow = false;
		}

		if ( m_bShowSaneIndexNow )
		{
			m_bShowSaneIndexNow = false;
		}

		if ( bDelExpertTip )
		{
			//
			DEL(m_pExpertTrade);

			// ������
			OnDelClrKLine();

			OnDelTrade();
		}
	}

	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	UpdateSelfDrawCurve();
	Invalidate();
}

void CIoViewKLine::OnOwnIndexMenu(UINT nID)
{
	if ( IDM_KLINE_SANE_INDEX == nID
		&& !CPluginFuncRight::Instance().IsUserHasSaneIndexRight(true) )
	{
		return;	// û��Ȩ��
	}
	//
	DelCurrentIndex();

	// ��ɫָ��
	if ( IDM_KLINE_TREND_INDEX == nID )
	{
		/*
		// ����ָ��������⴦��,������ ID = IDM_CHART_INDEX_END
		m_bShowTrendIndexNow = true;
		m_bShowSaneIndexNow	 = false;

		// ...fangz0815 ������ʱ������.ͬʱ��ʾGGTongMA
		CString StrName = L"GGTMA";

		m_FormulaNames.RemoveAll();

		T_IndexParam* pIndex = AddIndex(m_pRegionPick,StrName);

		if ( NULL == pIndex )
		{
		return;
		}

		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
		int32 iNodeBegin,iNodeEnd;
		GetNodeBeginEnd(iNodeBegin,iNodeEnd);
		g_formula_compute ( this, this, pIndex->pRegion, pData, pIndex, iNodeBegin,iNodeEnd );

		DrawTrendIndex(m_pMainDC);
		*/
	}
	else if ( IDM_KLINE_SANE_INDEX == nID )
	{
		m_bShowSaneIndexNow = true;
		m_bShowTrendIndexNow= false;

		ReDrawAysnc();

		m_FormulaNames.RemoveAll();
	}	 
}

void CIoViewKLine::OnMenuExpertTrade(UINT nID)
{
	if ( NULL == m_pMerchXml )
	{
		return;
	}

	//
	if ( m_MerchParamArray.GetSize() <= 0 )
	{
		return;
	}

	T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
	if ( NULL == pData )
	{
		return;
	}

	//
	CExpertTrade::E_ExpertTradeType eType = (CExpertTrade::E_ExpertTradeType)(nID - IDM_IOVIEWKLINE_EXPERT_BEGIN);

	if ( NULL == m_pExpertTrade )
	{
		// ��ʼû��, ������
		float fCirStock = 0.;

		if ( NULL != m_pMerchXml->m_pFinanceData )
		{
			fCirStock = m_pMerchXml->m_pFinanceData->m_fCircAsset;
		}

		m_pExpertTrade = new CExpertTrade(eType, fCirStock);	
		UpdateMainMerchKLine(*pData, true);

		// �ػ�
		ReDrawAysnc();
	}
	else
	{
		if ( eType == m_pExpertTrade->GetTradeType() )
		{
			return;
		}

		// ���¼���		
		m_pExpertTrade->UpdateAll(eType);

		// �ػ�		 
		ReDrawAysnc();
	}
}

void CIoViewKLine::KLineCycleChange(UINT nID)
{
	//	OnMenu(nID);
	if ( nID >= IDM_CHART_KMINUTE && nID <= IDM_CHART_KMINUTE240 )
	{		
		int32 i, iSize = m_MerchParamArray.GetSize();

		for ( i = 0; i < iSize; i ++ )
		{
			T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(i);
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

void CIoViewKLine::OnMenu( UINT nID )
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
	case IDM_CHART_PROP:
		OnExport();
		break;
	case IDM_CHART_HIDE_INDEX:
		DelCurrentIndex();
		break;
	case IDM_CHART_ADDCMP_INDEX:
		AddCmpIndex();
		break;
	case IDM_CHART_CNP:
		OnMenuCnp();
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
	case IDM_CHART_KSTYLE1:
		//pKLineDrawer->m_eChartDrawType = CChartDrawer::EKDSNormal;
		//UpdateKLineStyle();
		SetMainKlineDrawStyle(EMKDSNormal);
		bNeedReDraw = true;
		break;
	case IDM_CHART_KSTYLE2:
		// 		pKLineDrawer->m_eChartDrawType = CChartDrawer::EKDSFillRect;
		// 		UpdateKLineStyle();
		SetMainKlineDrawStyle(EMKDSFill);
		bNeedReDraw = true;
		break;
	case IDM_CHART_KSTYLE3:
		// 		pKLineDrawer->m_eChartDrawType = CChartDrawer::EKDSAmerican;
		// 		UpdateKLineStyle();
		SetMainKlineDrawStyle(EMKDSAmerica);
		bNeedReDraw = true;
		break;
	case IDM_CHART_KSTYLE4:
		SetMainKlineDrawStyle(EMKDSClose);
		UpdateKLineStyle();
		bNeedReDraw = true;
		break;
	case IDM_CHART_KSTYLE5:
		SetMainKlineDrawStyle(EMKDSTower);
		UpdateKLineStyle();
		bNeedReDraw = true;
		break;
	default:
		break;
	}

	if ( nID >= IDC_CHART_ADD2BLOCK_BEGIN && nID < IDC_CHART_ADD2BLOCK_END )
	{
		CString StrBlockName = m_aUserBlockNames.GetAt(nID-IDC_CHART_ADD2BLOCK_BEGIN);
		Add2Block(StrBlockName);
	}

	if ( nID >= IDM_CHART_KMINUTE && nID <= IDM_CHART_KMINUTE240 )
	{		
		int32 i, iSize = m_MerchParamArray.GetSize();

		for ( i = 0; i < iSize; i ++ )
		{
			T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(i);
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
			}

			SetTimeInterval(*pData, NodeInterval);
			SetCurveTitle(pData);
		}

		CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
		if(pMain)
		{
			//pMain->m_LastTimeSet = 0;
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
	else if ( nID == ID_KLINE_REMOVECMPMERCH )
	{
		// ɾ�����е�����Ʒ
		SetAutoAddExpCmpMerch(false, false);
		RemoveAllCmpMerch();
	}
	else if ( ID_CHART_INTERVALSTATISTIC == nID )
	{
		// K������ͳ��
		// �����
		m_aKLineInterval.RemoveAll();
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
			CDlgKLineInteval Dlg;
			Dlg.SetKLineData(this, m_pMerchXml, pData->m_eTimeIntervalFull, pData->m_iTimeUserMultipleDays, m_pMerchXml->m_pFinanceData->m_fCircAsset, m_fPriceIntervalPreClose, m_aKLineInterval);
			Dlg.DoModal();	
		}
	}
	else if ( ID_CHART_QUANTSTATISTIC == nID)
	{	
		// ֻ���Ƕ��߼����߲�����Ϊ���ߵ�ʱ��Ŵ�����ͳ�ƽ���
		bool32 bIsDXCPExist = CheckIndexExist(L"���߲���");
		bool32 bIsZXCPExist = CheckIndexExist(L"���߲���");

		if( !bIsZXCPExist && !bIsDXCPExist)
		{
			MessageBox(L"�����л�Ϊ���߲��̻����߲���ָ��!", L"��ʾ");
		}
		else if (ENTIDay !=  GetTimeInterval())
		{
			MessageBox(L"Ŀǰ��֧������ͳ��!", L"��ʾ");
		}
		else
		{
			// K������ͳ��
			// �����
			m_aKLineInterval.RemoveAll();
			T_MerchNodeUserData *pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
			if ( NULL != pData )
			{
				int32 iNodeBeginNow, iNodeEndNow;
				GetNodeBeginEnd(iNodeBeginNow, iNodeEndNow);
				// ׼������һ�ε�ͳ������
				int32 iNodeCounts = 0;
				if ( iNodeBeginNow < iNodeEndNow && pData->m_aKLinesFull.GetSize() > 0 )
				{
					// ׼������һ�ε�ͳ������
					iNodeCounts = iNodeEndNow - iNodeBeginNow;

					m_aKLineInterval.SetSize(iNodeCounts);

					CKLine* pKLineInterval = (CKLine*)m_aKLineInterval.GetData();
					CKLine* pKLineFull	   = (CKLine*)pData->m_aKLinesFull.GetData();

					memcpyex(pKLineInterval, pKLineFull + iNodeBeginNow, sizeof(CKLine) * iNodeCounts);
				}
				//
				CDlgKLineQuant Dlg;

				Dlg.SetKLineData(this, m_pMerchXml, pData->m_eTimeIntervalFull, pData->m_iTimeUserMultipleDays, iNodeBeginNow, iNodeCounts, m_aKLineInterval);
				Dlg.DoModal();	
			}
		}
	}
	else if ( ID_INVESTMENT_DIARY == nID)
	{	
		CDlgDialy::ShowNotePad(CDlgDialy::EST_UserNote);
	}
	else if ( ID_KLINE_HISTORYTREND == nID )
	{
		// ��ʷ��ʱ - ����������
		T_MerchNodeUserData *pData = m_MerchParamArray.GetSize() > 0 ? m_MerchParamArray[0] : NULL;
		if ( NULL != pData 
			&& m_NodePick.m_iID > 0
			&& ENTIDay == GetTimeInterval() )
		{
			CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
			CKLine kline;
			NodeData2KLine(m_NodePick, kline);
			if ( NULL != pMainFrame )
			{
				pMainFrame->DealHistoryTrendDlg(this, kline);
			}
		}
	}
	else if ( ID_KLINE_AXISYNORMAL <= nID
		&& ID_KLINE_AXISYLOG >= nID )
	{
		SetMainCurveAxisYType((CPriceToAxisYObject::E_AxisYType)(nID-ID_KLINE_AXISYNORMAL));
	}

	if ( bNeedReDraw )
	{
		m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
		Invalidate();
	}
}

void CIoViewKLine::OnWeightMenu(UINT nID)
{
	CIoViewKLine::E_WeightType eWeightTypeBk = m_eWeightType;

	//
	if ( IDM_PRE_WEIGHT == nID )
	{
		SetWeightType(CIoViewKLine::EWTPre);
	}
	else if ( IDM_AFT_WEIGHT == nID )
	{
		SetWeightType(CIoViewKLine::EWTAft);
	}
	else if ( IDM_NONE_WEIGHT == nID )
	{
		SetWeightType(CIoViewKLine::EWTNone);
	}

	// ֪ͨ���е���ͼͬʱ��Ӧ����¼�:
	if ( NULL != m_pAbsCenterManager && eWeightTypeBk != m_eWeightType )
	{
		m_pAbsCenterManager->OnWeightTypeChange();
	}
}

void CIoViewKLine::OnMenuIntervalStatistic()
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
	CDlgKLineInteval Dlg;
	Dlg.SetKLineData(this, m_pMerchXml, pMainData->m_eTimeIntervalFull, pMainData->m_iTimeUserMultipleDays, m_pMerchXml->m_pFinanceData->m_fCircAsset, m_fPriceIntervalPreClose, m_aKLineInterval);
	Dlg.DoModal();	
}

void CIoViewKLine::OnAddClrKLine()
{
	CDlgColorKLine Dlg(EFTClrKLine);
	if ( IDOK == Dlg.DoModal() )
	{
		if ( NULL == Dlg.GetFormular() )
		{
			return;
		}

		CFormularContent *pContent = Dlg.GetFormular();
		bool32 bIsExp = false;
		if ( NULL != m_pMerchXml )
		{
			bIsExp = GetMerchKind(m_pMerchXml) == ERTExp;
		}
		if ( pContent->m_byteIndexStockType == EIST_Index && !bIsExp )
		{
			// ����ָ���ָ���Ʊ
			if ( AfxMessageBox(_T("��ѡ����ָ��Ϊ����ר��ָ�꣬�뵱ǰ����Ʒ����ƥ�䣡\r\n��ȷ��Ҫѡ���ָ����"), MB_ICONQUESTION |MB_YESNO)
				== IDNO )
			{
				return;
			}

		}
		else if ( pContent->m_byteIndexStockType == EIST_Stock && bIsExp )
		{
			// ����ָ�� ָ���Ʊ
			if ( AfxMessageBox(_T("��ѡ����ָ��Ϊ����ר��ָ�꣬�뵱ǰ����Ʒ����ƥ�䣡\r\n��ȷ��Ҫѡ���ָ����"), MB_ICONQUESTION |MB_YESNO)
				== IDNO )
			{
				return;
			}
		}

		//DoAddClrKLine(pContent);
		AddShowIndex(pContent->name, false, false, true);
	}
}

void CIoViewKLine::OnDelClrKLine()
{
	if ( NULL != m_pRegionMain )
	{
		CChartCurve* pCurve = m_pRegionMain->GetDependentCurve();
		if ( NULL != pCurve )
		{
			CChartDrawer* pDrawer = (CChartDrawer*)pCurve->GetDrawer();
			if ( NULL != pDrawer )
			{
				pDrawer->SetColorFlag(false);
			}
		}
	}

	//
	m_pFormularClrKLine = NULL;
	DEL(m_pOutArrayClrKLine);

	//
	ReDrawAysnc();
}

void CIoViewKLine::OnDelTrade()
{
	//
	m_pFormularTrade = NULL;
	DEL(m_pOutArrayTrade);

	//
	ReDrawAysnc();
}

void CIoViewKLine::OnDelAllPrompt()
{	
	DEL(m_pExpertTrade);

	OnDelClrKLine();
	OnDelTrade();
}

void CIoViewKLine::OnAddTrade()
{
	CDlgColorKLine Dlg(EFTTradeChose);
	if ( IDOK == Dlg.DoModal() )
	{
		CFormularContent* pFormular = Dlg.GetFormular();

		DoAddTrade(pFormular);
	}
}



void CIoViewKLine::OnTimer(UINT nIDEvent)
{	
	if ( KTimerIdDelayAutoAddExpCmp == nIDEvent )
	{
		KillTimer(nIDEvent);
		CMerch *pExpNow = GetMerchAccordExpMerch(GetMerchXml());
		for ( int32 i=0; i < m_MerchParamArray.GetSize() ; i++ )
		{
			if ( m_MerchParamArray[i]->m_pMerchNode == pExpNow )
			{
				return ;
			}
		}

		// �����ע
		CSmartAttendMerch AttendMerch;
		AttendMerch.m_pMerch = pExpNow;
		AttendMerch.m_iDataServiceTypes = m_iDataServiceType;
		m_aSmartAttendMerchs.Add(AttendMerch);

		RequestSingleCmpMerchViewData(pExpNow);

	}
	CIoViewChart::OnTimer(nIDEvent);
}



void CIoViewKLine::DoAddTrade( CFormularContent *pFormulaTrade )
{
	if ( NULL != m_pFormularTrade )
	{
		OnDelTrade();
	}
	m_pFormularTrade = pFormulaTrade;
	if ( NULL != m_pFormularTrade )
	{
		if ( m_MerchParamArray.GetSize() > 0 && NULL != m_MerchParamArray[0] )
		{
			T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
			UpdateMainMerchKLine(*pData, true);
		}			

		if ( NULL != m_pRegionMain )
		{
			m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
			Invalidate();
		}
	}
}

LRESULT CIoViewKLine::OnMessageTitleButton(WPARAM wParam,LPARAM lParam)
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

		iIndexNums = LoadJinDunSpecialIndex(pSubMenu);

		if ( 0 >= iIndexNums )
		{
			iIndexNums = LoadOftenIndex(pSubMenu);	// ���س���ָ��
		}

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
	else if ( REGIONTITLELEFTBUTTONID == uID)
	{
		// ǰһ��ָ��
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);

		if (NULL == pData || NULL == m_pRegionPick )
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
		if ( NULL == m_pRegionPick )
		{
			return 0;
		}

		m_FormulaNames.RemoveAll();
		CNewMenu menu;
		menu.LoadMenu(IDR_MENU_KLINE);
		menu.LoadToolBar(g_awToolBarIconIDs);

		CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(0));

		CMenu* pTempMenu = pPopMenu->GetSubMenu(_T("����ָ��"));
		ASSERT( NULL != pTempMenu);
		CNewMenu* pSubMenu = DYNAMIC_DOWNCAST(CNewMenu,pTempMenu);

		pSubMenu->RemoveMenu(0,MF_BYPOSITION|MF_SEPARATOR);//ɾ��β��

		/*		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);

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
		*/
		LoadOftenIndex(pSubMenu);

		CPoint pt;
		GetCursorPos(&pt);
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, this);
		menu.DestroyMenu();

	}
	else if ( REGIONTITLERIGHTBUTTONID == uID)
	{
		// ��һ��ָ��

		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);

		if (NULL == pData || NULL == m_pRegionPick )
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

void CIoViewKLine::OnPickNode ( CChartRegion* pRegion, CChartCurve* pCurve, CNodeSequence* pNodes, CNodeData* pNodeData,int32 x,int32 y,int32 iFlag )
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
		if ( CheckFlag(pCurve->m_iFlag,CChartCurve::KTypeKLine) )
		{
			StrText.Format(_T("ʱ: %s \n��: %s\n��: %s\n��: %s\n��: %s\n��: %s\n��: %s\n"),
				StrTime.GetBuffer(),
				Float2String(pNodeData->m_fOpen,iSaveDec,true).GetBuffer(),
				Float2String(pNodeData->m_fHigh,iSaveDec,true).GetBuffer(),
				Float2String(pNodeData->m_fLow,iSaveDec,true).GetBuffer(),
				Float2String(pNodeData->m_fClose,iSaveDec,true).GetBuffer(),
				Float2String(pNodeData->m_fVolume,iSaveDec,true).GetBuffer(),
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

void CIoViewKLine::OnMenuCnp()
{
	CNewMenu menu;
	menu.LoadMenu(IDR_MENU_KLINE);
	menu.LoadToolBar(g_awToolBarIconIDs);

	m_bNeedCnp = !m_bNeedCnp;

	if(m_bNeedCnp)
	{
		menu.CheckMenuItem(IDM_CHART_CNP,MF_BYCOMMAND|MF_CHECKED);
	}
	else
	{
		menu.CheckMenuItem(IDM_CHART_CNP,MF_BYCOMMAND|MF_UNCHECKED);
	}

	ReDrawAysnc();
}

void CIoViewKLine::MenuAddRegion()
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

	E_IndexStockType eIST = GetMerchKind(m_pMerchXml) == ERTExp ? EIST_Index : EIST_Stock;

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

	for ( int32 i = 0 ; i < pData->aIndexs.GetSize(); i++)
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

	for (int32 i = 0 ; i < m_FormulaNames.GetSize() ; i++)
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

	if ( BeNeedZLMM(pData) )
	{
		UpdateMainMerchKLine(*pData);
	}
	else
	{
		m_aZLMMCrossID.RemoveAll();
	}

	RedrawWindow();
}

void CIoViewKLine::SetXmlSource ()
{
	T_MerchNodeUserData* pData = NULL;

	m_iIntervalXml = -1;
	m_iDrawTypeXml = -1;
	m_iNodeNumXml = 0;

	m_bTrendIndex = m_bShowTrendIndexNow;
	m_bSaneIndex  = m_bShowSaneIndexNow;	

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

void CIoViewKLine::DelCurrentIndex()
{
	// ɾ�����е�ָ��
	if( NULL == m_pRegionPick )
	{
		return;
	}

	ClearRegionIndex(m_pRegionPick, FALSE, TRUE);

	// 	for (int32 i = 0 ; i < m_pRegionPick->m_Curves.GetSize(); i++)
	// 	{		
	// 		CChartCurve * pCurve = m_pRegionPick->m_Curves.GetAt(i);		
	// 		T_IndexParam* pParam = FindIndexParamByCurve(pCurve);
	// 		
	// 		if (pParam)
	// 		{
	// 			CArray<CChartCurve*,CChartCurve*> Curves;
	// 			FindCurvesByIndexParam(pParam,Curves);	
	// 			while ( Curves.GetSize() > 0 )
	// 			{
	// 				pCurve = Curves.GetAt(0);
	// 				Curves.RemoveAt(0);
	// 				pCurve->GetChartRegion().RemoveCurve(pCurve);
	// 			}
	// 
	// 			DeleteIndexParamData ( pParam );			
	// 		}	
	// 	}
	// 	
	// 	if ( m_pRegionPick == m_pRegionMain )
	// 	{
	// 		if ( m_bShowTrendIndexNow )
	// 		{
	// 			m_bShowTrendIndexNow = false;
	// 		}
	// 		
	// 		if ( m_bShowSaneIndexNow )
	// 		{
	// 			m_bShowSaneIndexNow = false;
	// 		}
	// 
	// 		//
	// 		DEL(m_pExpertTrade);
	// 	}
	// 
	// 	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	// 	UpdateSelfDrawCurve();
	// 	RedrawWindow();
}

void CIoViewKLine::AddCmpIndex()
{
	// 
	if (NULL == m_pRegionMain)
	{
		//ASSERT(0);
		return;
	}

	//
	T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
	if ( NULL == pData )
	{
		return;
	}

	// ������3����ͼָ��
	int32 iMainIndex = 0;
	for ( int32 i=0; i < pData->aIndexs.GetSize() ; i++ )
	{
		if (m_pRegionMain == pData->aIndexs[i]->pRegion)
		{
			iMainIndex++;
		}
	}
	if (3 <= iMainIndex)
	{
		MessageBox(_T("������3����ͼָ��"), AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return;
	}

	//
	CDlgFormularManager Dlg;
	Dlg.SetToChoose();
	if (IDOK == Dlg.DoModal())
	{
		CFormularContent* pChoose = Dlg.GetChoosedFormular();

		if (NULL == pChoose)
		{
			return;
		}

		if (pChoose->BePassedXday())
		{
			MessageBox(L"ָ���Ѿ�����!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
			return;
		}

		//
		CString StrIndexName = pChoose->name;

		if (StrIndexName.GetLength() <= 0)
		{
			//ASSERT(0);
			return;
		}

		if ( !CPluginFuncRight::Instance().IsUserHasIndexRight(StrIndexName, TRUE) )
		{
			return;	// ��Ȩ��
		}

		bool32 bHasMainIndex = FALSE;

		// ����Ƿ���ڸ�index
		for ( int32 i=0; i < pData->aIndexs.GetSize() ; i++ )
		{
			// 
			if ( pData->aIndexs[i]->strIndexName == StrIndexName && m_pRegionMain == pData->aIndexs[i]->pRegion )
			{
				return;
			}

			if ( m_pRegionMain == pData->aIndexs[i]->pRegion ) // ��ͼָ��
			{
				bHasMainIndex = TRUE;
			}
		}

		// �����ǰ��ͼû��ָ�꣬��������ָ�꣬����ָ�괦��
		if (!bHasMainIndex)
		{
			AddIndex(m_pRegionMain, StrIndexName);
			ReDrawAysnc();
			return;
		}

		//
		T_IndexParam* pIndex = new T_IndexParam;
		pIndex->pRegion = m_pRegionMain;
		_tcscpy (pIndex->strIndexName, StrIndexName.LockBuffer());
		StrIndexName.UnlockBuffer();

		pIndex->pContent = CFormulaLib::instance()->AllocContent(pIndex->id,StrIndexName);

		if ( NULL == pIndex->pContent )
		{
			DEL(pIndex);
			return;
		}

		pIndex->bMainIndex = false;

		pData->aIndexs.Add(pIndex);

		pIndex->m_iIndexShowPosInFullList   = pData->m_iShowPosInFullList;
		pIndex->m_iIndexShowCountInFullList = pData->m_iShowCountInFullList;

		// ����Index��ػ�ͼ��Դ
		// ǰ�������Ѿ���ӣ�����ʵ�ʿ��ܲ�û�и��ӵ����ͼ�Σ�����ͼ�α�ɾ��ʱ
		// ָ����Դ��û�еõ��ͷţ�ͬʱ��ָ������ָ�򸽼�ͼ�Σ���������ʱ����
		// Ӧ����ʧ��ʱ���ͷ�index��Դ������NULL
		int32 iNodeBegin,iNodeEnd;
		GetNodeBeginEnd(iNodeBegin,iNodeEnd);
		if ( !g_formula_compute ( this, m_pRegionViewData, (CChartRegion*)pIndex->pRegion, pData, pIndex, iNodeBegin, iNodeEnd) )
		{
			// û�з���ɹ�
			//ASSERT(0);
		}

		ReDrawAysnc();
	}
}

void CIoViewKLine::MenuAddIndex ( int32 id )
{
	/*	DelCurrentIndex();	
	//
	if ( IDM_KLINE_TREND_INDEX - IDM_CHART_INDEX_BEGIN == id )
	{
	// ����ָ��������⴦��,������ ID = IDM_CHART_INDEX_END
	m_bShowTrendIndexNow = true;
	m_bShowSaneIndexNow	 = false;

	// ...fangz0815 ������ʱ������.ͬʱ��ʾGGTongMA
	CString StrName = L"GGTMA";

	m_FormulaNames.RemoveAll();

	T_IndexParam* pIndex = AddIndex(m_pRegionPick,StrName);

	if ( NULL == pIndex )
	{
	return;
	}

	T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
	int32 iNodeBegin,iNodeEnd;
	GetNodeBeginEnd(iNodeBegin,iNodeEnd);
	g_formula_compute ( this, this, pIndex->pRegion, pData, pIndex, iNodeBegin,iNodeEnd );
	}
	else if ( IDM_KLINE_SANE_INDEX - IDM_CHART_INDEX_BEGIN == id )
	{
	m_bShowSaneIndexNow = true;
	m_bShowTrendIndexNow= false;

	m_FormulaNames.RemoveAll();
	}
	else
	{
	if ( m_pRegionPick == m_pRegionMain)
	{
	m_bShowTrendIndexNow = false;
	m_bShowSaneIndexNow	 = false;
	}

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
	g_formula_compute ( this, this, pIndex->pRegion, pData, pIndex, iNodeBegin,iNodeEnd );
	}

	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	UpdateSelfDrawCurve();
	Invalidate();
	*/
}

void CIoViewKLine::MenuDelRegion()
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

	if ( m_MerchParamArray.GetSize() >= 0 )
	{
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
		if ( BeNeedZLMM(pData) )
		{
			UpdateMainMerchKLine(*pData);
		}
		else
		{
			m_aZLMMCrossID.RemoveAll();
		}
	}

	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	RedrawWindow();
}

void CIoViewKLine::MenuDelCurve()
{
	if ( NULL == m_pCurvePick || NULL == m_pRegionPick )
	{
		return;
	}
	CChartRegion &chartRegion = m_pCurvePick->GetChartRegion();
	DeleteIndexCurve ( m_pCurvePick );
	if ( chartRegion.GetCurveNum() <= 0 && &chartRegion != m_pRegionMain )
	{
		DelSubRegion(&chartRegion);	// ɾ����region
	}

	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	UpdateSelfDrawCurve();
	Invalidate();
}

void CIoViewKLine::OnDropCurve ( CChartRegion* pSrcRegion,CChartCurve* pSrcCurve,CChartRegion* pDestChart)
{
	if ( NULL == pDestChart )
	{
		DeleteIndexCurve(pSrcCurve);
	}
	else
	{
		DragDropCurve( pSrcCurve,pDestChart);
	}

	if ( NULL != pSrcRegion && NULL != m_pRegionMain )
	{
		CChartCurve* pMainCurve = m_pRegionMain->GetDependentCurve();

		if ( pSrcRegion == m_pRegionMain && pSrcCurve == pMainCurve )
		{
			if ( NULL != m_pOutArrayClrKLine && NULL != m_pFormularClrKLine )
			{
				OnDelClrKLine();
			}
		}
	}

	UpdateSelfDrawCurve();

	OnVDataMerchKLineUpdate(m_pMerchXml);
}

void CIoViewKLine::OnRegionCurvesNumChanged ( CChartRegion* pRegion,int32 iNum )
{

}

void CIoViewKLine::OnCrossData ( CChartRegion* pRegion,int32 iPos,CNodeData& NodeData, float fPricePrevClose, float fYValue, bool32 bShow)
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

			// �����Ҳ�ʮ�����ϲ�ļ۸�����
			CChartCurve *pCurveDep = pRegion->GetDependentCurve();
			if ( NULL!=pCurveDep )
			{
				int32 iY = 0;
				if ( pCurveDep->PriceYToRegionY(fYValue, iY) )
				{
					pRegion->RegionYToClient(iY);
					CString	StrValue;
					Float2StringLimitMaxLength(StrValue, fYValue, iSaveDec, INT_MAX, false, false, false);

					// ��������ʱdc���Ƶ��ã����������ֱ��ʹ����ʱdc����swap
					CMemDCEx *pDC = GetOverlayDC();
					CRect rcRgn(0,0,0,0);
					rcRgn = pRegion->GetRectCurves();

					CRect rcText(0,0,0,0);
					rcText.right	= rcRgn.right - 2;
					rcText.bottom	= iY - 1;

					pDC->SelectObject( GetIoViewFontObject(ESFSmall) );
					pDC->SetTextColor( GetIoViewColor(ESCText) );
					pDC->SetBkMode(TRANSPARENT);
					CSize sizeText	= pDC->GetTextExtent(StrValue);
					rcText.left		= rcText.right - sizeText.cx;
					rcText.top		= rcText.bottom - sizeText.cy;
					pDC->DrawText(StrValue, rcText, DT_LEFT|DT_SINGLELINE|DT_BOTTOM);
				}
			}
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

void CIoViewKLine::OnCrossNoData(CString StrTime,bool32 bShow)
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

void CIoViewKLine::SetFloatData ( CGridCtrl* pGridCtrl,float fCursorValue,int32 iSaveDec)
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
	Float2StringLimitMaxLength(StrValue, fCursorValue, iSaveDec, 7, true, true);
	pCellSymbol->SetText(StrValue);

	pGridCtrl->RedrawWindow();
}

void CIoViewKLine::SetFloatData ( CGridCtrl* pGridCtrl,CKLine& kLine,float fPricePrevClose,float fCursorValue,CString StrTimeLine1,CString StrTimeLine2,int32 iSaveDec)
{
	if ( !m_bInitialFloat )
	{
		InitCtrlFloat(&m_GridCtrlFloat);
	}

	bool32 bFutures = false;
	bool32 bStockCn = false;
	T_MerchNodeUserData* pDate = m_MerchParamArray.GetAt(0);

	if ( NULL != pDate)
	{
		CMerch* pMerch = pDate->m_pMerchNode;
		if ( NULL != pMerch)
		{
			E_ReportType eType = pMerch->m_Market.m_MarketInfo.m_eMarketReportType;
			if (CReportScheme::IsFuture(eType))
			{
				bFutures = true; 
			}
			else
			{
				bFutures = false;
			}
			bStockCn = ERTStockCn==eType ? true : false;
		}
	}

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

	//��߼�
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(7,0);
	//StrValue = Float2SymbolString(KLine.m_fPriceHigh, fPricePrevClose, iSaveDec) + _T(" ");
	Float2SymbolStringLimitMaxLength(StrValue, kLine.m_fPriceHigh, fPricePrevClose, iSaveDec, iYLeftShowChar, true, false, true, false, true);
	pCellSymbol->SetText(StrValue);

	//��ͼ�
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(9,0);
	//StrValue = Float2SymbolString(KLine.m_fPriceLow, fPricePrevClose, iSaveDec) + _T(" ");
	Float2SymbolStringLimitMaxLength(StrValue, kLine.m_fPriceLow, fPricePrevClose, iSaveDec, iYLeftShowChar, true, false, true, false, true);
	pCellSymbol->SetText(StrValue);

	//���̼�
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(11,0);
	//StrValue = Float2SymbolString(KLine.m_fPriceClose, fPricePrevClose, iSaveDec) + _T(" ");
	Float2SymbolStringLimitMaxLength(StrValue, kLine.m_fPriceClose, fPricePrevClose, iSaveDec, iYLeftShowChar, true, false, true, false, true);
	pCellSymbol->SetText(StrValue);

	// �ɽ���
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(13,0);	
	//StrValue = Float2String(KLine.m_fVolume, 0, true) + _T(" ");
	Float2StringLimitMaxLength(StrValue, kLine.m_fVolume, 0, GetYLeftShowCharWidth(1), true, true);
	pCellSymbol->SetText(StrValue);

	//�ɽ���/�ֲ�
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(15,0);
	if ( bFutures )
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

	//�ǵ�=����-����
	// 	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(17,0);
	// 	
	// 	StrValue = L" -";
	// 	if (0. != kLine.m_fPriceClose)
	// 	{
	// 		//StrValue = Float2SymbolString(KLine.m_fPriceClose - fPricePrevClose, 0.0f, iSaveDec) + _T(" ");		
	// 		Float2SymbolStringLimitMaxLength(StrValue, kLine.m_fPriceClose - fPricePrevClose, 0.0f, iSaveDec, iYLeftShowChar, true, false, false);
	// 	}
	// 	pCellSymbol->SetText(StrValue);

	//�ǵ���(����-����)/����*100%
	pCellSymbol = (CGridCellSymbol *)pGridCtrl->GetCell(17,0);

	StrValue = L" -";
	if (0. != kLine.m_fPriceClose && 0. != fPricePrevClose)
	{
		float fRisePercent = ((kLine.m_fPriceClose - fPricePrevClose) / fPricePrevClose) * 100.;
		StrValue = Float2SymbolString(fRisePercent, 0, 2, false, false, true);
	}
	pCellSymbol->SetText(StrValue);

	//���=(���-���)/����*100%
	pCellSys = (CGridCellSys *)pGridCtrl->GetCell(19,0);

	StrValue = L" -";
	if ( 0. != kLine.m_fPriceHigh && 0. != kLine.m_fPriceLow && 0. != fPricePrevClose)
	{
		float fValue = (kLine.m_fPriceHigh - kLine.m_fPriceLow) * 100.0f / fPricePrevClose;
		StrValue = Float2SymbolString(fValue, fValue, 2, false, false, true);
	}					
	pCellSys->SetText(StrValue);

	if ( bStockCn && pGridCtrl->GetRowCount() == KiGridFloatCtrlRowCountForStockCn )
	{
		// ������ ��ͨ��
		CString StrTradeRate, StrCircAsset;
		StrTradeRate = StrCircAsset = _T(" -");
		float fCircAsset = 0.0f;
		if ( CalcKlineCircAsset(m_pMerchXml, kLine.m_TimeCurrent, fCircAsset) )
		{
			//ASSERT( fCircAsset > 0.0f );
			Float2StringLimitMaxLength(StrTradeRate, kLine.m_fVolume/fCircAsset*10000, 2, iYLeftShowChar-2, true, false, true, true);
			Float2StringLimitMaxLength(StrCircAsset, fCircAsset, 2, iYLeftShowChar-2, true, true);
		}
		pCellSys = (CGridCellSys *)pGridCtrl->GetCell(21,0);
		pCellSys->SetText(StrTradeRate);
		// 		pCellSys = (CGridCellSys *)pGridCtrl->GetCell(25,0);
		// 		pCellSys->SetText(StrCircAsset);
	}
	else
	{
		ASSERT( pGridCtrl->GetRowCount() == KiGridFloatCtrlRowCountForOther );
	}

	pGridCtrl->RedrawWindow();

	// 
	if ( !pGridCtrl->IsWindowVisible() )
	{
		pGridCtrl->ShowWindow(SW_SHOW|SW_SHOWNOACTIVATE);

		CRect rt;
		pGridCtrl->GetClientRect(&rt);

		OffsetRect(&rt,m_PtGridCtrl.x,m_PtGridCtrl.y);
		EnableClipDiff(true,rt);

		pGridCtrl->RedrawWindow();					
	}	
}

void CIoViewKLine::ClipGridCtrlFloat (CRect& rect)
{
	EnableClipDiff(true,rect);
}

CString CIoViewKLine::OnTime2String ( CGmtTime& Time )
{
	CString StrTime = Time2String ( Time,ENTIMinute );
	if ( m_MerchParamArray.GetSize() > 0 )
	{
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
		StrTime = Time2String(Time,pData->m_eTimeIntervalFull);
	}
	return StrTime;
}

CString CIoViewKLine::OnFloat2String ( float fValue, bool32 bZeroAsHLine, bool32 bNeedTerminate/* = false*/ )
{
	return BaseFloat2String(fValue,bZeroAsHLine, bNeedTerminate);
}

void CIoViewKLine::OnCalcXAxis(CChartRegion* pRegion, CDC* pDC, CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aXAxisDivide)
{
	aAxisNodes.SetSize(0);
	aXAxisDivide.SetSize(0);

	if ( pRegion == m_pRegionMain )	// ��ͼ��Y����ʾ
	{
		CalcMainRegionXAxis(aAxisNodes, aXAxisDivide);

		FireIoViewChouMaAxisChange();	// ʱ����仯�ˣ�֪ͨ����
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

void CIoViewKLine::OnCalcYAxis(CChartRegion* pRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide)
{
	DWORD dwB = timeGetTime();

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

	DWORD dwE = timeGetTime();

	CString StrTrace;
	StrTrace.Format(L"%s   CalcYAxis ��ʱ: %d ms \r\n", pRegion->m_StrName.GetBuffer(), (dwE - dwB));
	// TRACE(StrTrace);
}

///////////////////////////////////////////////////
//
void CIoViewKLine::OnRegionDrawNotify(IN CMemDCEx* pDC,IN CMemDCEx* pPickDC,CRegion* pRegion,E_RegionDrawNotifyType eType)
{
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
			DrawStrategySignal(pDC);
			DrawUserNotes(pDC);
			DrawWeightInfo(pDC);
			DrawTitle1(pDC);
			DrawExpertTradePrompt(pDC);
			DrawSaneIndex(pDC);
			DrawTrade(pDC);
			DrawLandMines(pDC);
			// DrawZLMMIndex(pDC);
			DrawInterval(pDC, pChartRegion);
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

	//CRect Rect;
	CPoint pt1,pt2;
	COLORREF color;
	CPen pen,*pOldPen;

	color = GetIoViewColor(ESCChartAxisLine);
	pen.CreatePen(PS_SOLID,1,color);
	pOldPen = (CPen*)pDC->SelectObject(&pen);

	if ( eType == ERDNBeforeDrawCurve )
	{
		if ( m_bNeedCnp )
		{
			// ���ɽ��ѻ�:
			DrawNcp(pDC);
		}

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

					int32 iTmpHeight = RectTest.Height()/2;
					int32 iTmpWidth = RectTest.Height();
					int32 iMyHalfHeight = max(iHalfHeight, iTmpHeight);
					int32 iMyFontHeight = max(iFontHeight,iTmpWidth);

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

					rect.right -= 10;
					//rect.left -= 10;

					// 
					if ( rect.bottom > RectYLeft.bottom )
					{					
						rect.bottom	= RectYLeft.bottom;
						rect.top	= rect.bottom - iMyFontHeight;
					}

					pDC->MoveTo(RectMain.left, AxisDivide.m_iPosPixel);
					pDC->LineTo(rect.right-1, AxisDivide.m_iPosPixel);

					// ���ư汾��K��������������ɫ��ʱ���óɹ̶�ֵ
					pDC->SetTextColor(RGB(215, 214, 219));
					//pDC->SetTextColor(AxisDivide.m_DivideText1.m_lTextColor);
					pDC->SetBkMode(TRANSPARENT);
					pDC->DrawText(AxisDivide.m_DivideText1.m_StrText, &rect, AxisDivide.m_DivideText1.m_uiTextAlign);
				}
			}
		}
		else if (pChartRegion == m_pRegionYRight)
		{
			//CRect RectMain = m_pRegionMain->GetRectCurves();
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

				rect.left += 10;

				//
				// ���ư汾��K��������������ɫ��ʱ���óɹ̶�ֵ
				pDC->SetTextColor(RGB(215, 214, 219));
				//pDC->SetTextColor(AxisDivide.m_DivideText2.m_lTextColor);
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
			UINT uiTimeStart = timeGetTime();
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
				for (int32 i = 0; i < iAxisDivideCount - 1; i++)
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
				for (int32 i = 0; i < iAxisDivideCount; i++)
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
						// ���ư汾��K��������������ɫ��ʱ���óɹ̶�ֵ
						pDC->SetTextColor(RGB(125, 124, 129));
						//pDC->SetTextColor(pAxisDivide[i].m_DivideText1.m_lTextColor);
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
			UINT uiTimeEnd = timeGetTime();

			CString StrTrace;
			StrTrace.Format(L"draw::: %dms \n", uiTimeEnd - uiTimeStart);

			// TRACE(StrTrace);
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
				//bool32 bLastPickSub = i == m_iLastPickSubRegionIndex;
				m_iPickSubRegionIndex = m_iLastPickSubRegionIndex;
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

					bool32 bTmpLastPickSub = i == m_iLastPickSubRegionIndex;
					if ( bTmpLastPickSub && m_iChartType!=6)	// ������ͬ���е�K�߲���ʾ��ͼ��־
					{
						// ѡ�и�ͼ��־
						CRect RectSel(pChartRegion->m_RectView);
						CBrush brush;
						brush.CreateSolidBrush(RGB(79, 155, 255));
						CBrush * pOldBrush = pDC->SelectObject( &brush );

						POINT points[4];
						points[0].x = RectSel.right - 12; 
						points[0].y = RectSel.bottom; 
						points[1].x = RectSel.right; 
						points[1].y = RectSel.bottom; 
						points[2].x = RectSel.right;
						points[2].y = RectSel.bottom - 12; 
						points[3].x = RectSel.right - 12; 
						points[3].y = RectSel.bottom; 
						pDC->Polygon(points, 4); 
						pDC->SelectObject(pOldBrush);
						brush.DeleteObject();
					}
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

							rect.right -= 10;

							pDC->MoveTo(RectSubMain.left, AxisDivide.m_iPosPixel);
							pDC->LineTo(rect.right-1, AxisDivide.m_iPosPixel);
							// ���ư汾��K��������������ɫ��ʱ���óɹ̶�ֵ
							pDC->SetTextColor(RGB(215, 214, 219));
							//pDC->SetTextColor(AxisDivide.m_DivideText1.m_lTextColor);
							pDC->SetBkMode(TRANSPARENT);
							pDC->DrawText(AxisDivide.m_DivideText1.m_StrText, &rect, AxisDivide.m_DivideText1.m_uiTextAlign);
						}
					}

					//if ( bLastPickSub )
					//{
					//	// ѡ�и�ͼ��־
					//	CRect RectSel(pChartRegion->m_RectView);
					//	RectSel.InflateRect(1, -1, -1, -1);
					//	pDC->_DrawRect(RectSel, color);
					//}
				}
				else if (pChartRegion == SubRegion.m_pSubRegionYRight)
				{
					CRect RectSubMain = SubRegion.m_pSubRegionMain->GetRectCurves();
					CRect RectYRight = m_pRegionYRight->GetRectCurves();
					RectYRight.DeflateRect(3, 3, 3, 3);

					// ��������
					// xl 0607 ��ͼС��һ���߶ȣ���������
					if ( RectSubMain.Height() >= 8 )
					{
						CAxisDivide *pAxisDivide = (CAxisDivide *)SubRegion.m_pSubRegionMain->m_aYAxisDivide.GetData();
						for (int32 i = 0; i < SubRegion.m_pSubRegionMain->m_aYAxisDivide.GetSize(); i++)
						{
							CAxisDivide &AxisDivide = pAxisDivide[i];

							CRect rect = RectYRight;
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

							rect.right -= 10;

							pDC->MoveTo(RectSubMain.left, AxisDivide.m_iPosPixel);
							pDC->LineTo(rect.right-1, AxisDivide.m_iPosPixel);
							// ���ư汾��K��������������ɫ��ʱ���óɹ̶�ֵ
							pDC->SetTextColor(RGB(215, 214, 219));
							//pDC->SetTextColor(AxisDivide.m_DivideText1.m_lTextColor);
							pDC->SetBkMode(TRANSPARENT);
							pDC->DrawText(AxisDivide.m_DivideText1.m_StrText, &rect, AxisDivide.m_DivideText1.m_uiTextAlign);
						}
					}
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
void CIoViewKLine::OnSliderId ( int32& id, int32 iJump )
{
	SliderId ( id, iJump );
}
CString CIoViewKLine::OnGetChartGuid ( CChartRegion* pRegion )
{
	return GetChartGuid(pRegion );
}

void CIoViewKLine::InitialShowNodeNums()
{
	// �������λ������Ҫ��ʾ����k�ߵģ����Բ�������
	m_iNodeCountPerScreen = m_iNodeNumXml + KSpaceRightCount;

	if ( m_iNodeCountPerScreen < KMinKLineNumPerScreen || m_iNodeCountPerScreen > KMaxKLineNumPerScreen)
	{
		m_iNodeCountPerScreen = KDefaultKLineNumPerScreen;
	}	
}

void CIoViewKLine::DoFromXml()
{
	if (m_bShowTopMerchBar || (GetIoViewManager() && GetIoViewManager()->GetManagerTopbarStatus()))
	{
		m_iTopButtonHeight = TOP_BUTTON_HEIHGT - 1;	
	}
	SetViewToolBarHeight(m_iTopButtonHeight+m_iTopMerchHeight);
	BShowViewToolBar(m_bShowIndexToolBar, m_bShowTopToolBar);
	if ( m_bFromXml )
	{
		m_bShowTrendIndexNow = m_bTrendIndex;

		if ( CPluginFuncRight::Instance().IsUserHasSaneIndexRight(false) )
		{
			m_bShowSaneIndexNow	 = m_bSaneIndex;
		}
		else
		{
			m_bShowSaneIndexNow = false;
		}

		CIoViewChart::DoFromXml();

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

		//
		AddSelfDrawNodesFromXML();
		return;
	}

	AddSubRegion();
	m_pRegionMain->NestSizeAll();
	T_SubRegionParam SubParam = m_SubRegions.GetAt(0);

	if (NULL != m_pMerchXml)
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

IChartBrige CIoViewKLine::GetRegionParentIoView()
{
	IChartBrige ChartRegion;
	ChartRegion.pWnd = this;
	ChartRegion.pIoViewBase = this;
	ChartRegion.pChartRegionData = this;
	return ChartRegion;
}

void CIoViewKLine::DrawNcp(IN CMemDCEx* pDC)
{
	if(m_MerchParamArray.GetSize() <= 0)
		return;

	// ������:
	T_MerchNodeUserData * pMainData = m_MerchParamArray.GetAt(0);	
	if (NULL == pMainData)
		return;

	// ��Region
	if ( NULL == m_pRegionMain)
		return;
	CRect RectDraw = m_pRegionMain->GetRectCurves();
	if ( RectDraw.Height() <= 0 || RectDraw.Height() >= 1024 )
		return;
	// pDC->_FillSolidRect(RectDraw,RGB(0,100,25));
	// 

	if (RectDraw.top == RectDraw.bottom)
		return ;
	//


	// ���,С�۸�,ÿ��ˮƽ�ߵļ۸�.
	CChartCurve *pCurveDependent = m_pRegionMain->GetDependentCurve();
	if (NULL == pCurveDependent)
		return;

	float fAxisYMin, fAxisYMax;
	float fYMin = 0., fYMax = 0.;
	if (!pCurveDependent->GetYMinMax(fAxisYMin, fAxisYMax)) // ���Ϊ���������Сֵ
		return;
	pCurveDependent->AxisYToPriceY(fAxisYMax, fYMax);
	pCurveDependent->AxisYToPriceY(fAxisYMin, fYMin);

	float fPricePerLine = (float)(fYMax - fYMin)/RectDraw.Height();

	if (fPricePerLine <= 0)		
		return;
	//

	// ���鱣������ˮƽ���ϵļ۸��Ӧ�ĳɽ����ĺ�

	long pValueSum[1024];
	memset(pValueSum, 0, sizeof(long)*RectDraw.Height());

	int iStart, iEnd, iMaxValue;

	iMaxValue = 0;

	// ����Ӧ���ǵ�ǰ��ʾK �ߵ�����.
	for ( int32 i = 0 ; i < pMainData->m_pKLinesShow->GetSize(); i++)
	{
		CNodeData Node;
		pMainData->m_pKLinesShow->GetAt(i,Node);

		iStart = (int32)((Node.m_fLow  - fYMin)/fPricePerLine);
		iEnd   = (int32)((Node.m_fHigh - fYMin)/fPricePerLine);

		for ( int32 j = iStart ; j <= iEnd ; j++)
		{
			if ( j > (RectDraw.Height() -1))
			{
				continue;
			}

			pValueSum[j] += (int32)(Node.m_fVolume / fPricePerLine);
			if ( iMaxValue < pValueSum[j])
			{
				iMaxValue = pValueSum[j];
			}
		}
	}

	if ( iMaxValue <= 0)
	{
		return;
	}

	CMPIChildFrame * pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());

	if (NULL == pChildFrame)	
		return;

	//COLORREF clrLine = pChildFrame->GetFrameColor(ESCGridLine);
	COLORREF clrLine = RGB(128,255,255);			

	CPen LinePen(PS_SOLID,1,clrLine);
	CPen* oldPen = pDC->SelectObject(&LinePen);


	float fTmp = 0.00;
	CRect rect;
	int   iWidth;

	for (int32 i = 0; i< RectDraw.Height();i++)
	{
		pDC->MoveTo(RectDraw.right, RectDraw.bottom - i);
		fTmp = (float)pValueSum[i] / iMaxValue;
		iWidth = (int)(RectDraw.Width() * fTmp * 2/3);
		rect.top = RectDraw.bottom - i-1;
		rect.bottom = RectDraw.bottom - i;
		rect.left = RectDraw.right - iWidth;
		rect.right = RectDraw.right;
		rect.NormalizeRect( );

		pDC->LineTo(RectDraw.right - iWidth, RectDraw.bottom - i);
	}
	pDC->SelectObject(oldPen);
}

void CIoViewKLine::SetChildFrameTitle()
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

void CIoViewKLine::GetCrossKLine(OUT CKLine & KLineIn)
{
	memset((void*)(&KLineIn),0,sizeof(Kline));
	memcpyex(&KLineIn,&m_KLineCrossNow,sizeof(Kline));
}

void CIoViewKLine::OnIoViewActive()
{
	CIoViewChart::OnIoViewActive();

	FireIoViewChouMa(0, 0, 0);
	//
	m_bViewIsvisible = true;
	RedrawAdvertise();
}

void CIoViewKLine::OnIoViewDeactive()
{
	CIoViewChart::OnIoViewDeactive();
	m_TipWnd.Hide();
}

void CIoViewKLine::CalcLayoutRegions(bool bOnlyUpdateMainRegion)
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

			const CPriceToAxisYObject::E_AxisYType eType = GetMainCurveAxisYType();
			pDepCurve->SetAxisYType(eType);	// ����������������
			if ( CalcKLinePriceBaseValue(pDepCurve, fBase) )
			{
				ASSERT( eType == CPriceToAxisYObject::EAYT_Normal || fBase != CChartCurve::GetInvalidPriceBaseYValue() );
				// �������е���region�е�curve�Ļ���ֵ
				if ( eType == CPriceToAxisYObject::EAYT_Normal || fBase != CChartCurve::GetInvalidPriceBaseYValue() )
				{
					for (i = 0; i < m_pRegionMain->m_Curves.GetSize(); i++)
					{
						CChartCurve *pCurve = m_pRegionMain->m_Curves[i];
						if ( NULL != pCurve )
						{
							// index��maincurveһ���Ļ���ֵ
							// ���ӵ���Ҫ��������
							if (CheckFlag(pCurve->m_iFlag, CChartCurve::KindexCmp))
							{
								// ���ӵ�ָ�겻�������������
								continue;
							}

							//
							pCurve->SetAxisYType(eType);	// ����
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

						if (CheckFlag(pChartCurve->m_iFlag, CChartCurve::KindexCmp))
						{
							// ���ӵ�ָ�겻�������������
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
		for (int32 j = 0; j < m_SubRegions.GetSize(); j++)
		{
			CChartRegion *pSubRegionMain = m_SubRegions[j].m_pSubRegionMain;			

			if (NULL != pSubRegionMain)
			{
				bool32 bValidCurveYMinMax = false;
				float fCurveYMin = 0., fCurveYMax = 0.;

				// �����������ߵ����±߽�ֵ
				for (int32 i = 0; i < pSubRegionMain->m_Curves.GetSize(); i++)
				{
					float fYMin = 0., fYMax = 0.;
					CChartCurve *pChartCurve = pSubRegionMain->m_Curves[i];

					if (CheckFlag(pChartCurve->m_iFlag, CChartCurve::KindexCmp))
					{
						// ���ӵ�ָ�겻�������������
						continue;
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

				// �������е���region�е�curve��������ֵ
				for (int32 i = 0; i < pSubRegionMain->m_Curves.GetSize(); i++)
				{
					CChartCurve *pChartCurve = pSubRegionMain->m_Curves[i];
					pChartCurve->SetYMinMax(fCurveYMin, fCurveYMax, bValidCurveYMinMax);
				}
			}		
		}
	}

	// ����ָ������ֵ��Сֵ����:
	if (m_MerchParamArray.GetSize() > 0)
	{
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
		if(NULL != pData)		
		{
			for (int32 i = 0; i < pData->aIndexs.GetSize(); i++)
			{
				T_IndexParam* p = pData->aIndexs[i];
				if (NULL == p)
				{
					continue;
				}

				// �ҵ����region ��ĵ���ָ��, ����һ�����Լ���һ���ߵ������Сֵ
				if (NULL != p->pRegion && !p->bMainIndex)
				{
					float fMinCmp = FLT_MAX;
					float fMaxCmp = FLT_MIN;

					// ��������Сֵ
					for (int32 j = 0; j < p->m_aIndexLines.GetSize(); j++)
					{
						CChartCurve* pCurveIndex = (CChartCurve*)p->m_aIndexLines[j].m_pCurve;
						if (NULL == pCurveIndex || NULL == pCurveIndex->GetNodes())
						{
							continue;
						}

						int32 iSize = pCurveIndex->GetNodes()->GetSize();
						if (iSize <= 0)
						{
							continue;
						}

						// 
						float fMinTmp, fMaxTmp;
						CNodeData NodeBegin,NodeEnd;

						pCurveIndex->GetNodes()->GetAt(0, NodeBegin);
						pCurveIndex->GetNodes()->GetAt(iSize - 1, NodeEnd);

						if (pCurveIndex->CalcY(fMinTmp, fMaxTmp))
						{
							fMinCmp = fMinCmp <= fMinTmp ? fMinCmp : fMinTmp;
							fMaxCmp = fMaxCmp >= fMaxTmp ? fMaxCmp : fMaxTmp;
						}
					}

					// ���ֵ��Сֵ��ͬ��(����ֱ��) 
					if (fMinCmp == fMaxCmp)
					{			
						// ���ڵ�ʱ��, ������ֻ��һ��ʾ������, ûʲô��������, ���м������
						float fTmp = fMinCmp/2.0;

						fMinCmp -= fTmp;
						fMaxCmp += fTmp;
					}

					// ������һ�������Сֵ
					for (int32 j = 0; j < p->m_aIndexLines.GetSize(); j++)
					{
						CChartCurve* pCurveIndex = (CChartCurve*)p->m_aIndexLines[j].m_pCurve;
						if (NULL == pCurveIndex)
						{
							continue;
						}

						// 					
						pCurveIndex->SetYMinMax(fMinCmp, fMaxCmp);
					}
				}
			}
		}
	}
}

bool32 CIoViewKLine::CalcMainRegionXAxis(CArray<CAxisNode, CAxisNode&> &aAxisNodes, CArray<CAxisDivide, CAxisDivide&> &aAxisDivide)
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
	else
	{
		//NULL;
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
	int32 iKLineUnitCount = m_iNodeCountPerScreen;
	float fPixelWidthPerUnit = (rect.Width() - 2) / (float)iKLineUnitCount;

	// 4.000 Ȼ��Ŵ� 5.999 ʱ������һ���ϲ� - ��ȥ��ȡ�� xl 0601
	//if (fPixelWidthPerUnit >= 5.)
	//	fPixelWidthPerUnit = (int)(fPixelWidthPerUnit);
	// ���Ҫ�������Ķ��룬���ȻҪ��ʼ��ĳЩ���ݻᱻ������ʾ���߽��������ֶ���Ŀո�
	// ѡ�����ұ߳��ֶ���ո��Ա�֤������ʾ�̶�����k��, ��������������ܼ���k���ұ߷�����ϡ����ұ߿�
	// �����������Ҳ��ж���ո��ó����Ƚ�һ�£�������Էſ���ȥceil����������ԣ���floor
	if ( fPixelWidthPerUnit >= 3.0 )
	{
		int32 iRightSpace = GetSpaceRightCount();
		if ( iRightSpace > 0 )
		{
			float fRightSpaceTotal = iRightSpace * fPixelWidthPerUnit;
			if ( (fRightSpaceTotal/iKLineUnitCount + fPixelWidthPerUnit) - ceilf(fPixelWidthPerUnit) > 0.1 )
			{
				//TRACE(_T("����: %f - %f\n"), fPixelWidthPerUnit, ceilf(fPixelWidthPerUnit));
				//fPixelWidthPerUnit = ceilf(fPixelWidthPerUnit);	// �ո��㹻��ȥceil  �ұ߿ո��ж����󣬲�����������

			}
		}
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

		AxisNode.m_iEndPixel = rect.left + (int32)(((iIndexNode + 1) * fPixelWidthPerUnit + 0.5)) - 1;
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
				AxisDivide.m_DivideText1.m_lTextColor = m_pRegionMain->GetAxisTextColor();

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
				AxisDivide.m_DivideText1.m_lTextColor = m_pRegionMain->GetAxisTextColor();

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

//�ֶ��Ż�������������
float CalcStepSize(float fOldStep)
{
	// С�ڵ���0
	if (fOldStep<0.0000001)
	{
		return 0;
	}

	float tempStep = fOldStep;
	float log10Step = log10(tempStep);

	if (log10Step < 1.0)
	{
		return CalcStepSize(fOldStep*10)/10;
	}
	else if (log10Step > 10)
	{
		return CalcStepSize(fOldStep/10)*10;
	}
	else
	{
		float mag =		floor(log10Step);
		float magPow =  pow((float)10, mag);
		float magMsd = tempStep / magPow + 0.5;
		if (magMsd > 7.5)			magMsd = 10.0;
		else if (magMsd > 3.5)		magMsd = 5.0;
		else if (magMsd > 1.5)		magMsd = 2.0;
		else						magMsd = 1.0;
		return magMsd * magPow;
	}
}

void CIoViewKLine::CalcMainRegionYAxis(CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide)
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
	int32 iSaveDec = 0;
	T_MerchNodeUserData* pData = NULL;
	if ( m_MerchParamArray.GetSize() > 0 )
	{
		pData = m_MerchParamArray.GetAt(0);
		CMerch* pMerch = pData->m_pMerchNode;
		if ( NULL != pMerch )
			iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;
	}

	// ����Y����������ɶ��ٸ��̶�
	CRect rect = m_pRegionMain->GetRectCurves();
	int32 iHeightText = pDC->GetTextExtent(_T("����ֵ")).cy;
	int32 iHeightSub = iHeightText + 12;

	if ( CPriceToAxisYObject::EAYT_Pecent == pCurveDependent->GetAxisYType() )
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
			ASSERT( 0 );
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
			AxisDivide.m_DivideText1.m_lTextColor	= m_pRegionMain->GetAxisTextColor();
			AxisDivide.m_DivideText2.m_lTextColor	= m_pRegionMain->GetAxisTextColor();

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
			AxisDivide.m_DivideText1.m_lTextColor	= m_pRegionMain->GetAxisTextColor();
			AxisDivide.m_DivideText2.m_lTextColor	= m_pRegionMain->GetAxisTextColor();

			aYAxisDivide.Add(AxisDivide);
		}
	}
	else if ( CPriceToAxisYObject::EAYT_Log == pCurveDependent->GetAxisYType() )
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
			ASSERT( 0 );
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
			Float2StringLimitMaxLength(StrPrice, fPrice, iSaveDec, GetYLeftShowCharWidth() -1, true, false, false, false);
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
			AxisDivide.m_DivideText1.m_lTextColor	= m_pRegionMain->GetAxisTextColor();
			AxisDivide.m_DivideText2.m_lTextColor	= m_pRegionMain->GetAxisTextColor();

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
			AxisDivide.m_DivideText1.m_lTextColor	= m_pRegionMain->GetAxisTextColor();
			AxisDivide.m_DivideText2.m_lTextColor	= m_pRegionMain->GetAxisTextColor();

			aYAxisDivide.Add(AxisDivide);
		}
	}
	else 
	{
		ASSERT( CPriceToAxisYObject::EAYT_Normal == pCurveDependent->GetAxisYType() );


		int32 iCount     = 8;			//����ķָ��߶��߶���
		if (rect.Height() < 100)
		{
			iCount = 3;
		}
		else if (rect.Height() < 150)
		{
			iCount = 5;
		}



		float diffValue = fYMax - fYMin;		
		float fOldStep = diffValue/iCount;

		//����ָ��߶θ���
		float fNewStep = CalcStepSize(fOldStep);		  //����ֵ
		if (fNewStep>0.0000001)
		{
			iCount = (int)(diffValue / fNewStep)+1;			  //ʵ�ʵ��߶θ���
		}

		//������С��ʼֵ
		float nScale = pow((float)10.0,(float)iSaveDec);
		float tmpMax = (int)(fYMax*nScale);
		int iStepScale = (int)(fNewStep*nScale);
		if (0 != iStepScale)
		{
			tmpMax = tmpMax - (int)(tmpMax)%iStepScale;
		}

		aYAxisDivide.SetSize(iCount);
		CAxisDivide *pAxisDivide = (CAxisDivide *)aYAxisDivide.GetData();

		float fStart = tmpMax/nScale;
		int32 y = 0;
		int32 iNum = 0;

		for (; iNum < iCount; iNum++)
		{
			if (fStart <= fYMin)
			{
				break;
			}

			if (!pCurveDependent->AxisYToRegionY(fStart, y))
			{
				//ASSERT(0);
				break;
			}
			m_pRegionMain->RegionYToClient(y);

			// xl 0001762 ������Y���ִ����� - �Ҳ���һ���ո�
			//CString StrValue	= Float2String(fStart, iSaveDec, true, false, false);
			CString StrValue;
			Float2StringLimitMaxLength(StrValue, fStart, iSaveDec, GetYLeftShowCharWidth() -1, true, false, false, false);

			// 
			CAxisDivide &AxisDivide = pAxisDivide[iNum];
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
			AxisDivide.m_DivideText1.m_lTextColor	= m_pRegionMain->GetAxisTextColor();
			AxisDivide.m_DivideText2.m_lTextColor	= m_pRegionMain->GetAxisTextColor();

			fStart -= fNewStep;
		}

		aYAxisDivide.SetSize(iNum + 1);

	}


	CPriceToAxisYObject calcObj;
	pCurveDependent->GetAxisYCalcObject(calcObj);
	if ( m_RectMainCurve != rect
		|| calcObj != m_AxisYCalcObj )
	{
		m_RectMainCurve = rect;
		m_AxisYCalcObj = calcObj;

		FireIoViewChouMa(1, 0, 0);	// ysize֪ͨ
	}
}

void CIoViewKLine::CalcSubRegionYAxis(CChartRegion* pChartRegion, CDC* pDC, CArray<CAxisDivide, CAxisDivide&> &aYAxisDivide)
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
	if ( bUserFormularLine && pFormular )
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
			AxisDivide.m_DivideText1.m_lTextColor	= m_pRegionMain->GetAxisTextColor();
			AxisDivide.m_DivideText2.m_lTextColor	= m_pRegionMain->GetAxisTextColor();

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
			for (int32 i = 0 ; i <= iNum; i++)
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
				AxisDivide.m_DivideText1.m_lTextColor	= m_pRegionMain->GetAxisTextColor();
				AxisDivide.m_DivideText2.m_lTextColor	= m_pRegionMain->GetAxisTextColor();

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
			AxisDivide.m_DivideText1.m_lTextColor	= m_pRegionMain->GetAxisTextColor();
			AxisDivide.m_DivideText2.m_lTextColor	= m_pRegionMain->GetAxisTextColor();

			// 
			iCount++;
		}
	}

	// 
	aYAxisDivide.SetSize(iCount);
}

void CIoViewKLine::OnKeyF8()
{
	//
	int32 i, iSize = m_MerchParamArray.GetSize();
	for ( i = 0; i < iSize; i ++ )
	{
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(i);
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
				//SetTimeInterval(*pData, ENTIDay);
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
	//	m_pRegionMain->SetDrawFlag(CRegion::KDrawTransform| CRegion::KDrawNotify);
	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	RedrawWindow();
}

void CIoViewKLine::OnKeyDown()
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

void CIoViewKLine::OnKeyUp()
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

void CIoViewKLine::OnKeyHome()
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

void CIoViewKLine::OnKeyEnd()
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

void CIoViewKLine::OnKeyLeftAndCtrl()
{
	OnKeyLeftAndCtrl(1);
}

void CIoViewKLine::OnKeyLeftAndCtrl( int32 iRepCnt )
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
		m_bForbidDrag = TRUE;
		return;
	}
	else
	{	
		m_bForbidDrag = FALSE;

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

void CIoViewKLine::OnKeyLeft()
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

void CIoViewKLine::OnKeyRightAndCtrl()
{
	// ȡ��ͼ��Ϣ
	OnKeyRightAndCtrl(1);
}

void CIoViewKLine::OnKeyRightAndCtrl( int32 iRepCnt )
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
		m_bForbidDrag = TRUE;
		return;
	}
	else
	{
		m_bForbidDrag = FALSE;

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

void CIoViewKLine::OnKeyRight()
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

void CIoViewKLine::OnKeySpace()
{
	CIoViewChart::OnKeySpace();

	//
	if (m_MerchParamArray.GetSize() <= 0)
	{
		return;
	}

	T_MerchNodeUserData *pMainData = m_MerchParamArray[0];
	if (NULL == pMainData)
	{
		return;
	}

	//
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
		CDlgKLineInteval Dlg;
		Dlg.SetKLineData(this, m_pMerchXml, pMainData->m_eTimeIntervalFull, pMainData->m_iTimeUserMultipleDays, m_pMerchXml->m_pFinanceData->m_fCircAsset, m_fPriceIntervalPreClose, m_aKLineInterval);
		Dlg.DoModal();	
	}

	if ( -1 != m_TimeIntevalBegin.GetTime() || -1 != m_TimeIntevalEnd.GetTime() )
	{
		ReDrawAysnc();
	}
}

void CIoViewKLine::OnIntervalTimeChange(const CGmtTime& TimeBegin, const CGmtTime& TimeEnd)
{
	// ʱ��ı���, ����ͼ����ʾ
	if (m_MerchParamArray.GetSize() <= 0)
	{
		return;
	}

	T_MerchNodeUserData *pMainData = m_MerchParamArray[0];
	if (NULL == pMainData)
	{
		return;
	}

	//
	int32 iNodeBeginNow, iNodeEndNow;
	GetNodeBeginEnd(iNodeBeginNow, iNodeEndNow);

	//
	int32 iPosBegin = CMerchKLineNode::QuickFindKLineByTime(pMainData->m_aKLinesFull, TimeBegin);
	if ( iPosBegin < 0  )
	{
		//ASSERT(0);
		return;
	}

	int32 iPosEnd = CMerchKLineNode::QuickFindKLineByTime(pMainData->m_aKLinesFull, TimeBegin);
	if ( iPosEnd < 0  )
	{
		//ASSERT(0);
		return;
	}

	//
	if ( iPosBegin >= iNodeBeginNow && iPosEnd <= iNodeEndNow )
	{
		// �µ�����С��ԭ����, ֱ�����ñ�־, �ػ�������
		SetIntervalBeginTime(TimeBegin);
		SetIntervalEndTime(TimeEnd);

		// ��ʾˢ��
		ReDrawAysnc();
	}
	else
	{
		// ��Ҫ����������ʾ����
		while (1)
		{
			OnZoomOut();

			//
			int32 iNodeBeginNowTime, iNodeEndNowTime;
			GetNodeBeginEnd(iNodeBeginNowTime, iNodeEndNowTime);

			if ( iPosBegin >= iNodeBeginNowTime && iPosEnd <= iNodeEndNowTime )
			{
				// �µ�����С��ԭ����, ֱ�����ñ�־, �ػ�������
				SetIntervalBeginTime(TimeBegin);
				SetIntervalEndTime(TimeEnd);

				// ��ʾˢ��
				ReDrawAysnc();
				break;
			}
		}
	}
}

void CIoViewKLine::OnRectZoomOut(int32 iNodeBegin, int32 iNodeEnd)
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
	if ( iNodeBegin < 0 || iNodeBegin >= pMainData->m_aKLinesCompare.GetSize() )
	{
		return;
	}

	if ( iNodeEnd < 0 || iNodeEnd >= pMainData->m_aKLinesCompare.GetSize() )
	{
		iNodeEnd = pMainData->m_aKLinesCompare.GetSize() -1;
	}

	if ( (iNodeEnd - iNodeBegin + 1) <= KMinKLineNumPerScreen )
	{
		return;
	}

	// ��ѡ�Ŵ�:
	int32 iNodeCounts		= iNodeEnd - iNodeBegin + 1;
	//m_iNodeCountPerScreen	= iNodeCounts + KSpaceRightCount;
	m_iNodeCountPerScreen	= iNodeCounts + GetSpaceRightCount();

	CGmtTime TimeStart,TimeEnd;
	bool32 bIsShowNewestOld = IsShowNewestKLine(TimeStart, TimeEnd);
	if (!ChangeMainDataShowData(*pMainData, iNodeBegin, iNodeCounts, bIsShowNewestOld, TimeStart, TimeEnd))
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

	// �ػ���ͼ
	ReDrawAysnc();
}

//
void CIoViewKLine::OnRectIntervalStatistics(int32 iNodeBegin, int32 iNodeEnd)
{
	// �����
	m_aKLineInterval.RemoveAll();

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
	if ( iNodeBegin < 0 || iNodeBegin >= pMainData->m_aKLinesCompare.GetSize() )
	{
		return;
	}

	if ( iNodeEnd < 0 || iNodeEnd >= pMainData->m_aKLinesCompare.GetSize() )
	{
		iNodeEnd = pMainData->m_aKLinesCompare.GetSize() -1;
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

	OnMenuIntervalStatistic();

	// ��ʾˢ��
	ReDrawAysnc();
}

bool32 CIoViewKLine::OnZoomOut()
{
	// ��С��ʾ��λ�� ��ʾ��һЩK��
	if ( m_MerchParamArray.GetSize() <= 0 )
		return false;

	T_MerchNodeUserData *pMainData = m_MerchParamArray[0];
	if (NULL == pMainData)
		return false;

	// ��ǰ�ܹ���K ��������û��������Ӧ���еĶ�, ˵�����ݲ���. ��Ҫ����С��.
	if ( pMainData->m_aKLinesFull.GetSize() + GetSpaceRightCount() < m_iNodeCountPerScreen )
	{
		return false;
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
		//ASSERT(0);
		return false;
	}

	// ��ʾˢ��
	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
	{
		m_SubRegions[i].m_pSubRegionMain->SetDrawFlag(CRegion::KDrawFull);
	}

	RedrawWindow();

	// ���Ƿ��б�Ҫ�����������
	RequestViewData();

	//
	UpdateMainMerchKLine(*pMainData);

	return true;
}

bool32 CIoViewKLine::OnZoomIn()
{
	// ��ʾ��λ�Ŵ� ��ʾ��һЩK��
	if (m_MerchParamArray.GetSize() <= 0)
		return false;

	T_MerchNodeUserData *pMainData = m_MerchParamArray[0];
	if (NULL == pMainData)
		return false;

	// int32 iCurShowCountPerScreen = m_iNodeCountPerScreen;
	// int32 iBakCurShowCountPerScreen = m_iNodeCountPerScreen;

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

		//
		if ( iStartPos * iEndPos < 0 ||iStartPos >= iSizeKLine || iEndPos >= iSizeKLine )
		{
			//ASSERT(0);
		}

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
		//ASSERT(0);
		return false;
	}

	// ��ʾˢ��
	m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
	for (int32 i = 0; i < m_SubRegions.GetSize(); i++)
	{
		m_SubRegions[i].m_pSubRegionMain->SetDrawFlag(CRegion::KDrawFull);
	}

	//
	RedrawWindow();

	//
	UpdateMainMerchKLine(*pMainData);

	return true;
}

void CIoViewKLine::RequestViewData()
{
	//
	// 	if ( !IsWindowVisible() )
	// 	{
	// 		return;
	// 	}

	if (m_MerchParamArray.GetSize() <= 0)
		return;

	int32 iSpaceRightCount = GetSpaceRightCount();

	for (int32 i = 0; i < m_MerchParamArray.GetSize(); i++)
	{
		T_MerchNodeUserData *pData = m_MerchParamArray[i];
		if (NULL == pData || NULL == pData->m_pMerchNode)
			continue;

		// ��ȡ�����Ʒ�����ڵ���Ϣ
		E_NodeTimeInterval eNodeTimeIntervalCompare = ENTIDay;
		E_KLineTypeBase	   eKLineTypeCompare		= EKTBDay;

		int32 iScale = 1;
		if (!GetTimeIntervalInfo(pData->m_eTimeIntervalFull, eNodeTimeIntervalCompare, eKLineTypeCompare, iScale))
		{
			//ASSERT(0);	// ��Ӧ�ó����������
			return;
		}

		if (eNodeTimeIntervalCompare != pData->m_eTimeIntervalCompare)
		{				
			// �����һ�£�����
			pData->m_eTimeIntervalCompare = eNodeTimeIntervalCompare;				
		}

		//�����Ƿ�Ӧ���������� - �������Ƚϵ�λʹ�û����� K�� ���ڵ�λ
		bool32 bNeedRequestMore		= false;
		CGmtTime TimeSpecify		= m_pAbsCenterManager->GetServerTime();
		// SaveDay(TimeSpecify);
		int32 iNeedKLineCount		= 1;
		int32 iNeedKLineCountForSane = KMinRequestKLineNumber * iScale;			// ������ָ������Ҫ����С��NodeCount

		iNeedKLineCount = MAX(iNeedKLineCountForSane, (m_iNodeCountPerScreen - iSpaceRightCount) * iScale);   // ѡȡ������������Ҫ�������KLine����
		iNeedKLineCount = iNeedKLineCount - pData->m_aKLinesCompare.GetSize();

		//
		if ( m_bForceUpdate )
		{
			// ǿ�Ƹ���, ��Щʱ��(���������) ���ܱ��������Ѿ����� MAX(...) �������ļ��� K ���Ǵ����. ��Ҫǿ�Ƹ���
			bNeedRequestMore = true;
			iNeedKLineCount	 = MAX(iNeedKLineCountForSane, (m_iNodeCountPerScreen - iSpaceRightCount) * iScale);	
		}
		else
		{
			if (iNeedKLineCount > 0)	// ���ݲ���
			{
				if (pData->m_aKLinesCompare.GetSize() < pData->m_pKLinesShow->GetSize())
				{
					//ASSERT(0);		// �Ƚ�����ֻ����࣬ �������
					return;
				}

				// ����Ѿ����ڵ�ǰ��Ҫ��ʾ���ݵ�N�����Ͳ�Ҫ*��
				if ( iNeedKLineCount < m_iNodeCountPerScreen * iScale * KRequestKLineNumberScale )
				{
					iNeedKLineCount *= KRequestKLineNumberScale;
				}

				//
				if (pData->m_pKLinesShow->GetSize() <= 0)	// ��ǰû������
				{
					bNeedRequestMore = true;
				}
				else	// ��ǰ�Ѿ��������ˣ� ���ѵ�ǰ��ʾK�ߵ�һ����ʱ��Ϊ׼�� ��ǰ����
				{
					TimeSpecify = pData->m_aKLinesCompare[0].m_TimeCurrent;

					// zhangbo 20090717 #���Ż��� ���������û���ס�����ŵ��µĴ����ظ�������
					//...
					bNeedRequestMore = true;
				}
			}
			else
			{
				//TRACE(_T("-------------------------------------------��������\r\n"));
			}
		}

		//
		if (bNeedRequestMore)
		{
			// �����
			//TRACE(_T("-------------------------------------------����[%d]\r\n"), iNeedKLineCount);
			if (iNeedKLineCount < KMinRequestKLineNumber)
			{
				iNeedKLineCount = KMinRequestKLineNumber;
			}

			//
			CMmiReqMerchKLine info; 
			info.m_eKLineTypeBase	= eKLineTypeCompare;								// ʹ��ԭʼK������
			info.m_iMarketId		= pData->m_pMerchNode->m_MerchInfo.m_iMarketId;
			info.m_StrMerchCode		= pData->m_pMerchNode->m_MerchInfo.m_StrMerchCode;

			info.m_eReqTimeType		= ERTYFrontCount;
			info.m_TimeSpecify		= TimeSpecify;
			info.m_iFrontCount		= iNeedKLineCount;

			//
			DoRequestViewData(info);
		}

		// ����ʵʱ����
		CMmiReqRealtimePrice MmiReqRealtimePrice;
		MmiReqRealtimePrice.m_iMarketId		= pData->m_pMerchNode->m_MerchInfo.m_iMarketId;
		MmiReqRealtimePrice.m_StrMerchCode	= pData->m_pMerchNode->m_MerchInfo.m_StrMerchCode;		
		DoRequestViewData(MmiReqRealtimePrice);

		if ( !CReportScheme::IsFuture(GetMerchKind(pData->m_pMerchNode)) )
		{
			// ���ڻ�
			// �����������
			CMmiReqPublicFile	reqF10;
			reqF10.m_ePublicFileType = EPFTF10;
			reqF10.m_iMarketId		 = pData->m_pMerchNode->m_MerchInfo.m_iMarketId;
			reqF10.m_StrMerchCode	 = pData->m_pMerchNode->m_MerchInfo.m_StrMerchCode;
			DoRequestViewData(reqF10);

			// ������Ϣ����
			if ( pData->bMainMerch )
			{
				CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();
				MerchReqTimeMap::iterator it = m_mapMerchLandMineHasReqTimes.find(pData->m_pMerchNode);
				if ( it == m_mapMerchLandMineHasReqTimes.end()
					|| TimeNow > it->second+CGmtTimeSpan(0,0,5,0) )
				{
					// û�з��͹����߳����ϴη���n����
					CMmiNewsReqLandMine reqMine;
					reqMine.m_iMarketId		= pData->m_pMerchNode->m_MerchInfo.m_iMarketId;
					reqMine.m_StrMerchCode	= pData->m_pMerchNode->m_MerchInfo.m_StrMerchCode;
					TimeNow +=  CGmtTimeSpan(1,0,0,0);	// +1��
					SaveDay(TimeNow);
					reqMine.m_StrTime1.Format(_T("%04d-%02d-%02d 00:00:00"), TimeNow.GetYear(), TimeNow.GetMonth(), TimeNow.GetDay());
					reqMine.m_iCount = 200;
					reqMine.m_iTimeType = 2;	// ������ǰ200��
					reqMine.m_iIndexInfo = EITLandmine;

					m_pAbsCenterManager->RequestNews(&reqMine);
					{
						// ��¼��η��͵�ʱ��
						m_mapMerchLandMineHasReqTimes[pData->m_pMerchNode] = m_pAbsCenterManager->GetServerTime();
					}
				}
			}
		}

	}

	// ��Ҫ��ʾ����Ϣ��so������
	//if ( EWTNone != m_eWeightType )
	{
		RequestWeightData();
	}
}

void CIoViewKLine::RequestWeightData()
{
	//
	if ( EWTNone == m_eWeightType )
	{
		// �����ǰ�ǲ���Ȩ��,ֱ�ӷ��� - ��Ҫ��Ȩ��Ϣ��ʾ��ʾ
		//return;
	}

	for ( int32 i = 0; i < m_MerchParamArray.GetSize(); i++ )
	{
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(i);

		if ( NULL == pData )
		{
			continue;
		}

		CMerch* pMerch = pData->m_pMerchNode;

		if ( NULL == pMerch )
		{
			continue;
		}

		if ( CReportScheme::Instance()->IsFuture(pMerch->m_Market.m_MarketInfo.m_eMarketReportType) )
		{
			continue;
		}

		// �����
		SetCurveTitle(pData);

		// 1: ��������û�г�Ȩ����
		if ( !pMerch->m_bHaveReadLocalWeightFile && 0 == pMerch->m_TimeLastUpdateWeightDatas.GetTime() )
		{
			// ���ض��ļ�ʧ�ܶ��Ҵӷ����������ݶ�ʧ�ܵ�ʱ��,
			// ��ͬʱ���� 1: �ٴζ������ļ� 2: �ٴη������������
			// �κ�һ��·���ɹ���,�Ͳ����ٶ��ļ���.���ǻ��ᷢ���������(2Сʱ�������)

			// ��һ�ε�ʱ��ӱ���ȡ,���ðѱ��ص�crc ֵ����������
			CArray<CWeightData,CWeightData&> aWeightData;
			UINT uiCrc32 = 0;

			if ( ReadWeightDataFromLocalFile(pMerch, aWeightData, uiCrc32) )
			{				
				pMerch->m_bHaveReadLocalWeightFile = true;

				// ����CMerch ������ֶε�ֵ				
				pMerch->m_uiWeightDataCRC = uiCrc32;

				int32 iSize = aWeightData.GetSize();
				pMerch->m_aWeightDatas.SetSize(iSize);				
				memcpyex(pMerch->m_aWeightDatas.GetData(), aWeightData.GetData(), sizeof(CWeightData) * iSize);					
			}	 
		}	

		// 2: �������������		
		CMmiReqPublicFile info;
		info.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
		info.m_StrMerchCode		= pMerch->m_MerchInfo.m_StrMerchCode;
		info.m_ePublicFileType	= EPFTWeight;
		info.m_uiCRC32			= pMerch->m_uiWeightDataCRC;

		DoRequestViewData(info);
	}
}

void CIoViewKLine::OnWeightTypeChange()
{
	// ֱ�ӽ��������������:
	RequestWeightData();

	// Ҫ����ָ��
	m_bForceCalcSaneIndex = true;
	m_aSaneIndexValues.RemoveAll();	// ��������

	// ����
	for ( int32 i=0; i < m_MerchParamArray.GetSize() ; i++ )
	{
		if ( NULL != m_MerchParamArray[i] && NULL != m_MerchParamArray[i]->m_pMerchNode )
		{
			if ( m_MerchParamArray[i]->bMainMerch )
			{
				UpdateMainMerchKLine(*m_MerchParamArray[i]);
			}
			else
			{
				UpdateSubMerchKLine(*m_MerchParamArray[i]);
			}
		}
	}

	// �ػ�
	ReDrawAysnc();
}

bool32 CIoViewKLine::GetSaneIndexNode(IN int32 iPos, OUT T_SaneIndexNode& Node)
{
	int32 iSize = m_aSaneIndexValues.GetSize();

	if ( iSize <= 0 || iPos >= iSize || iPos < 0)
	{
		return false;
	}

	Node = m_aSaneIndexValues.GetAt(iPos);

	return true;
}

int32 CIoViewKLine::CompareKLinesChange(const CArray<CKLine,CKLine>& KLineBef, const CArray<CKLine,CKLine>& KLineAft)
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

void CIoViewKLine::SetUpdateTime()
{
	// ����ʱ��: 1: ����1 ��K �ߵ�ʱ��. 2: ��ǰ����ߵ����ڷ����仯 3: ��ʱ����,���ڴ����� �Ϸ���10 ����û�и���. ��ʱ����ûɱ��,��������һ�¸���ʱ��	
	if ( !m_bShowSaneIndexNow )
	{	
		m_bNeedCalcSaneLatestValues = false;		
		m_TimeToUpdateSaneIndex	    = 0;
		return;
	}

	if( m_MerchParamArray.GetSize() <= 0)	
	{
		return;
	}

	T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
	if (NULL == pData || NULL == pData->m_pMerchNode)
	{
		return;
	}

	if ( pData->m_aKLinesFull.GetSize() <= 0)
	{
		return;
	}

	CMerch *pMerch = pData->m_pMerchNode;	
	if ( NULL == pMerch )
	{
		return;
	}

	int32 iKLineSize = pData->m_aKLinesFull.GetSize();

	// ���ø���ʱ��:	
	CGmtTime TimeNow;								// ��ǰ����һ��K �ߵ�ʱ��
	CGmtTime TimeClose;								// ��ǰ��Ʒ���������ʱ��
	CGmtTime TimeToUpdate = 0;

	bool32 bNeedTimer = true;						// �Ƿ���Ҫ��ʱ����������ָ��

	// �õ���ʼʱ��:
	TimeNow = pData->m_aKLinesFull.GetAt(iKLineSize - 1).m_TimeCurrent;

	// ʱ�����,����
	if ( TimeNow.GetTime() <= 0 )
		return;

	KillTimer( KTimerIdUpdateSaneValues );

	//
	if ( BePassedCloseTime(pMerch) )
	{
		m_bNeedCalcSaneLatestValues = false;		
		m_TimeToUpdateSaneIndex	    = 0;
		return;
	}

	// 
	bool32 bPassUpdateTime = false;
	if ( !GetKLineUpdateTime(m_pMerchXml, m_pAbsCenterManager, TimeNow, pData->m_eTimeIntervalFull, pData->m_iTimeUserMultipleMinutes, bNeedTimer, TimeToUpdate, bPassUpdateTime) )
	{
		return;
	}
	//

	m_TimeToUpdateSaneIndex = TimeToUpdate;

	if( bNeedTimer )
	{
		m_bNeedCalcSaneLatestValues = true;
		SetTimer(KTimerIdUpdateSaneValues, KTimerPeriodUpdateTrendValues, NULL);		
	}
	else
	{
		// ����Ҫ����ʵʱ����: 
		m_bNeedCalcSaneLatestValues	 = false;					
	}
}

bool32 CIoViewKLine::GetKLineUpdateTime(IN CMerch* pMerch, IN CAbsCenterManager* pAbsCenterManager, IN const CGmtTime& TimeNow, IN E_NodeTimeInterval eTimeInterval, IN int32 iMiniuteUser, OUT bool32& bNeedTimer, OUT CGmtTime& TimeToUpdate, OUT bool32& bPassedUpdateTime, IN bool32 bGetKLineFinishTime)
{	
	// �õ�ĳ��ʱ��K �߶�Ӧ�ĸ���ʱ��: ������, �����ܵõ���ȷ��ʱ��. ������û����. ����, ����, ����, �����жϵ�ǰ���Ƿ�Ϊ����(��, ��, ��)�����һ��������, ���ǵĻ�ֱ�ӷ���
	if ( NULL == pMerch || NULL == pAbsCenterManager )
	{
		return false;
	}

	// ��ʼ����������
	bNeedTimer   = true;
	TimeToUpdate = 0;
	bPassedUpdateTime = false;

	// 
	int32 iMiniutePeriod = 0;
	CGmtTime TimeClose = 0;

	// �����������ڵ�K �߽ڵ�ʱ�䶼�� XX:00 , ����ȡ�����յ�ʱ������. �����K ��ʱ��ת������ʱ����
	CGmtTime TimeNowFixed = TimeNow;

	if ( eTimeInterval == ENTIDay || eTimeInterval == ENTIDayUser || eTimeInterval == ENTIWeek || eTimeInterval == ENTIQuarter || eTimeInterval == ENTIYear )
	{		
		CGmtTimeSpan TimeSpanFix(0, 0, pMerch->m_Market.m_MarketInfo.m_iTimeInitializePerDay, 0);		
		TimeNowFixed += TimeSpanFix;
	}

	// 
	CMarketIOCTimeInfo RecentTradingDayTime;	
	if ( pMerch->m_Market.GetRecentTradingDay(TimeNowFixed, RecentTradingDayTime, pMerch->m_MerchInfo) )
	{	
		TimeClose = RecentTradingDayTime.m_TimeClose.m_Time;
	}
	else
	{
		return false;
	}

	// 		
	TimeToUpdate = TimeClose;

	//
	CGmtTimeSpan TimeSpan(0, 0, 0, SECONDSTOUPDATE);
	switch ( eTimeInterval )
	{
	case ENTIMinute:
		{		
			iMiniutePeriod = 1;
		}
		break;
	case ENTIMinute5:
		{
			iMiniutePeriod = 5;
		}
		break;
	case ENTIMinute15:
		{
			iMiniutePeriod = 15;
		}
		break;
	case ENTIMinute30:
		{
			iMiniutePeriod = 30;
		}
		break;
	case ENTIMinute60:
		{
			iMiniutePeriod = 60;
		}
		break;
	case ENTIMinute180:
		{
			iMiniutePeriod = 180;
		}
		break;
	case ENTIMinute240:
		{
			iMiniutePeriod = 240;
		}
		break;
	case ENTIMinuteUser:
		{			
			iMiniutePeriod = iMiniuteUser;
		}
		break;
	case ENTIDay:
		{
			if ( !bGetKLineFinishTime )
			{
				TimeToUpdate = TimeClose - TimeSpan;
			}			
		}
		break;
	case ENTIDayUser:          
		{
			return false;
		}
		break;
	case ENTIWeek:
		{
			// �����ǰ����������������ָ��. ��һ������ֱ�Ӳ��ø��¾�����
			if ( 6 == TimeNowFixed.GetDayOfWeek())
			{
				// �ȵõ�����ʱ��,�ټ�ȥʮ����		
				if ( !bGetKLineFinishTime )
				{
					TimeToUpdate = TimeClose - TimeSpan;
				}				
			}
			else
			{
				// ����ΪFALSE ,ʵʱ���ü������
				bNeedTimer = false;
			}
		}
		break;
	case ENTIMonth:
		{			
			// ���ж��ǲ�������µ����һ��������,���ǵĻ�ֱ�Ӳ��ø��¾�����
			if ( BeLastTradeDayInaMounth(TimeNowFixed) )
			{
				if ( !bGetKLineFinishTime )
				{
					TimeToUpdate = TimeClose - TimeSpan;
				}				
			}
			else
			{
				// ����ΪFALSE ,ʵʱ���ü������
				bNeedTimer = false;
			}			
		}
		break;
	case ENTIQuarter:
		{
			// ���ж��ǲ�������������һ��������,���ǵĻ�ֱ�Ӳ��ø��¾�����
			if ( BeLastTradeDayInaQuarter(TimeNowFixed) )
			{
				if ( !bGetKLineFinishTime )
				{
					TimeToUpdate = TimeClose - TimeSpan;
				}				
			}
			else
			{
				// ����ΪFALSE ,ʵʱ���ü������
				bNeedTimer = false;
			}
		}
		break;
	case ENTIYear:
		{			
			// ���ж��ǲ�������µ����һ��������,���ǵĻ�ֱ�Ӳ��ø��¾�����
			if ( BeLastTradeDayInaYear(TimeNowFixed) )
			{
				if ( !bGetKLineFinishTime )
				{
					TimeToUpdate = TimeClose - TimeSpan;
				}				
			}
			else
			{
				// ����ΪFALSE ,ʵʱ���ü������
				bNeedTimer = false;
			}			
		}
		break;
	default:
		break;
	}

	//
	CString StrNow = GetTimeString(TimeNowFixed, eTimeInterval);
	CString StrClose = GetTimeString(TimeClose, eTimeInterval);

	CString StrStart, StrFinish;

	if ( 0 != iMiniutePeriod )
	{
		UINT uiTimeStart, uiTimeFinish;		
		bool32 bOK = CMerchKLineNode::GetKLineTimeByMinute(pMerch, TimeNowFixed.GetTime(), iMiniutePeriod, uiTimeStart, uiTimeFinish);

		// �õ��Ľ���ʱ�����Ƿ��ӵĿ�ʼ, ���Լ���60 ��
		uiTimeFinish += 60;

		StrStart  = GetTimeString(CGmtTime(uiTimeStart), eTimeInterval);
		StrFinish = GetTimeString(CGmtTime(uiTimeFinish), eTimeInterval);

		//
		if ( bOK )
		{			
			if ( !bGetKLineFinishTime )
			{
				TimeToUpdate = CGmtTime(uiTimeFinish) - TimeSpan;
			}
			else
			{
				TimeToUpdate = CGmtTime(uiTimeFinish);
			}			
		}
		else
		{
			return false;				 
		}
	}

	if ( pAbsCenterManager->GetServerTime() >= TimeToUpdate )
	{
		bPassedUpdateTime = true;
	}

	return true;
}

void CIoViewKLine::OnTimerUpdateLatestValuse(UINT uID)
{
	if ( m_MerchParamArray.GetSize() < 0 )
	{
		return;
	}

	T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);

	if ( NULL == pData )
	{
		return;
	}

	// �Ƿ��ڻ�
	E_ReportType eReportType = GetMerchKind(m_pMerchXml);
	bool32 bJudgeKTOC = false;

	if (CReportScheme::IsFuture(eReportType))
	{
		bJudgeKTOC = true;
	}

	// �õ���ǰʱ��
	CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();

	// 10 ���ʱ��.
	CGmtTimeSpan TimeSpan(0, 0, 0, SECONDSTOUPDATE);

	// ����ĸ���ʱ��,���������ʱ��Ͳ�����
	CGmtTime TimeLast = m_TimeToUpdateSaneIndex + TimeSpan;  

	// ����ʱ��
	CMerch* pMerch = pData->m_pMerchNode;
	if ( NULL == pMerch )
	{
		return;
	}

	if ( KTimerIdUpdateSaneValues == uID )
	{
		if ( BePassedCloseTime(pMerch) )
		{
			// ������, ����һ������һ��������(��ֹʵʱ����ʷ��һ��)
			bool32 bKillTimer = false;

			CalcLatestSaneIndex(false, bJudgeKTOC, m_pIndexEMAForSane, pData->m_aKLinesFull, m_aSaneIndexValues, m_pAbsCenterManager->GetServerTime().GetTime(), bKillTimer);

			// ...fangz 0531
			ReDrawAysnc();

			//
			if ( bKillTimer )
			{		
				//
				m_bNeedCalcSaneLatestValues = false;
				KillTimer(KTimerIdUpdateSaneValues);				
			}
		}		
		else if ( TimeNow < m_TimeToUpdateSaneIndex )
		{
			// ��û��ʱ��,ʲô������
		}
		else if ( TimeNow >= m_TimeToUpdateSaneIndex && TimeNow <= TimeLast )
		{	
			for ( int32 i = 0 ; i < m_aMerchsHaveCalcLatestValues.GetSize(); i++ )
			{
				if ( m_pMerchXml == m_aMerchsHaveCalcLatestValues.GetAt(i) )
				{
					// �Ѿ����һ����,��Ҫ������
					return;
				}
			}

			// �Ϸ�ʱ��,���°�:
			// CRect RectClient;
			// GetClientRect(&RectClient);
			// m_pMainDC->FillSolidRect(&RectClient, RGB(255,0,0));
			// m_pRegionMain->ReDrawAll();

			// ��������ָ��:			
			bool32 bKillTimer = false;

			if ( CalcLatestSaneIndex(false, bJudgeKTOC, m_pIndexEMAForSane, pData->m_aKLinesFull, m_aSaneIndexValues, m_pAbsCenterManager->GetServerTime().GetTime(), bKillTimer) )
			{
				ASSERT(bKillTimer);
			}

			if ( bKillTimer )
			{		
				//
				m_bNeedCalcSaneLatestValues = false;
				KillTimer(KTimerIdUpdateSaneValues);

				// һ����Ʒֻ����һ��
				m_aMerchsHaveCalcLatestValues.Add(m_pMerchXml);

				//
				ReDrawAysnc();			
			}			
		}
		else if ( TimeNow > TimeLast )
		{
			// ��ʱ��,��Ҫ����.�������ø���ʱ��:
			m_bNeedCalcSaneLatestValues = false;
			KillTimer(KTimerIdUpdateSaneValues);

			SetUpdateTime();
		}		
	}
}

void CIoViewKLine::DrawZLMMIndex(CMemDCEx* pDC)
{
	if ( NULL == m_pMerchXml )
	{
		return;
	}

	if ( m_aZLMMCrossID.GetSize() <= 0 )
	{
		return;
	}

	if ( !CheckFlag( m_pRegionMain->GetDrawFlag(),CRegion::KDrawCurves) )
	{
		// �������϶��߿��ʱ��,��Ҫ����
		return;
	}

	if ( NULL == pDC)
	{
		return;
	}

	// ��������: ��ͷ����,��ͷ����,�𵴻���
	if ( NULL == m_pRegionMain)
	{
		return;
	}

	CChartCurve* pDependCurve = m_pRegionMain->GetDependentCurve();

	if ( NULL == pDependCurve)
	{
		return;
	}
	// 

	CNodeSequence* pNode = pDependCurve->GetNodes();	

	if ( NULL == pNode)
	{
		return;
	}

	int32 iNodeSize = pNode->GetSize();

	//
	Graphics GraphicImage(pDC->GetSafeHdc());

	//
	for ( int32 i = 0 ; i < iNodeSize; i++ )
	{
		uint32 uIDSrc = pNode->m_aNodes.GetAt(i).m_iID;

		for ( int32 j = 0; j < m_aZLMMCrossID.GetSize(); j++ )
		{
			uint32 uIDLocal = m_aZLMMCrossID.GetAt(j);

			if ( uIDLocal == uIDSrc )
			{
				// �����
				int32 iLow,iHigth,iOpen,iClose;

				pDependCurve->PriceYToRegionY(pNode->m_aNodes.GetAt(i).m_fLow, iLow);
				m_pRegionMain->RegionYToClient(iLow);
				m_pRegionMain->ClipY(iLow);

				pDependCurve->PriceYToRegionY(pNode->m_aNodes.GetAt(i).m_fHigh, iHigth);
				m_pRegionMain->RegionYToClient(iHigth);
				m_pRegionMain->ClipY(iHigth);

				pDependCurve->PriceYToRegionY(pNode->m_aNodes.GetAt(i).m_fOpen, iOpen);
				m_pRegionMain->RegionYToClient(iOpen);
				m_pRegionMain->ClipY(iOpen);

				pDependCurve->PriceYToRegionY(pNode->m_aNodes.GetAt(i).m_fClose, iClose);
				m_pRegionMain->RegionYToClient(iClose);
				m_pRegionMain->ClipY(iClose);

				CPoint ptCenter;				
				ptCenter.x = m_pRegionMain->m_aXAxisNodes[i].m_iCenterPixel;
				ptCenter.y = ( iOpen + iClose ) / 2;

				//			
				int32 iImageWidth     = m_pImageDTOC->GetWidth();
				int32 iImageHeight    = m_pImageDTOC->GetHeight();

				int32 iHalfImageWidth = iImageWidth / 2;

				// ��ͷ����
				CRect rectIcon;
				rectIcon.left   = ptCenter.x - iHalfImageWidth;
				rectIcon.right  = rectIcon.left + iImageWidth;			
				rectIcon.top    = iLow + 3;
				rectIcon.bottom = rectIcon.top + iImageHeight;

				DrawImage(GraphicImage, m_pImageZLMM, rectIcon, 1, 0, TRUE);
			}
		}
	}
}

// ��ͼ�Ż�����
void CIoViewKLine::DrawSaneIndex(CMemDCEx* pDC)
{	
	// ���Ƚ���ָ��
	if ( !m_bShowSaneIndexNow)
	{
		return;
	}

	//
	if ( NULL == pDC )
	{
		//ASSERT(0);
		return;
	}

	// ��������: ��ͷ����,��ͷ����,�𵴻���
	if ( NULL == m_pRegionMain)
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

	//
	int32 iSaneIndexSize = m_aSaneIndexValues.GetSize();
	int32 iNodeSize		  = pNode->GetSize();

	//
	if ( iSaneIndexSize <= 0 || iSaneIndexSize < iNodeSize )
	{
		// ����һ��		
		if ( m_MerchParamArray.GetSize() < 0 )
		{
			return;
		}

		T_MerchNodeUserData* pMerchData = m_MerchParamArray.GetAt(0);

		// ��ʹUpdateMainKLine����ָ��
		m_bForceCalcSaneIndex = true; 
		UpdateMainMerchKLine(*pMerchData);

		return;
	}

	CArray<T_DrawTrendParam,T_DrawTrendParam &> aDrawParam;
	aDrawParam.RemoveAll();

	if ( iNodeSize > m_pRegionMain->m_aXAxisNodes.GetSize() )
	{
		T_MerchNodeUserData* pMerchData = m_MerchParamArray.GetAt(0);
		UpdateMainMerchKLine(*pMerchData);		
		return;
	}

	aDrawParam.SetSize(iNodeSize);

	for ( int32 i = 0 ; i < iNodeSize; i++ )
	{
		T_DrawTrendParam &stDrawParam = aDrawParam[i];
		int32 iOpen,iClose;

		// ����̫�� - ���Ż�
		pDependCurve->PriceYToRegionY(pNode->m_aNodes.GetAt(i).m_fLow, stDrawParam.m_iLow);
		m_pRegionMain->RegionYToClient(stDrawParam.m_iLow);
		m_pRegionMain->ClipY(stDrawParam.m_iLow);

		pDependCurve->PriceYToRegionY(pNode->m_aNodes.GetAt(i).m_fHigh, stDrawParam.m_iHigh);
		m_pRegionMain->RegionYToClient(stDrawParam.m_iHigh);
		m_pRegionMain->ClipY(stDrawParam.m_iLow);

		pDependCurve->PriceYToRegionY(pNode->m_aNodes.GetAt(i).m_fOpen, iOpen);
		m_pRegionMain->RegionYToClient(iOpen);
		m_pRegionMain->ClipY(iOpen);

		pDependCurve->PriceYToRegionY(pNode->m_aNodes.GetAt(i).m_fClose, iClose);
		m_pRegionMain->RegionYToClient(iClose);
		m_pRegionMain->ClipY(iClose);

		stDrawParam.m_CenterPoint.x = m_pRegionMain->m_aXAxisNodes[i].m_iCenterPixel;
		stDrawParam.m_CenterPoint.y = ( iOpen + iClose ) / 2;
	}

	if ( aDrawParam.GetSize() < 0)
	{
		return;
	}

	// ͼ��
	Graphics GraphicImage(pDC->GetSafeHdc());

	// ���� gdi:
	CPen PenRiseDT(PS_SOLID, 2, RGB(255, 0, 0));
	CPen PenRiseDH(PS_SOLID, 2, RGB(0, 0, 255));

	CPen PenFallKT(PS_SOLID, 2, RGB(0, 255, 0));
	CPen PenFallKH(PS_SOLID, 2, RGB(255, 255, 0));

	CArray<CPoint, CPoint&> aPointsGdi;

	// �õ������յ�
	int32 iBegin,iEnd;	
	GetNodeBeginEnd(iBegin,iEnd);
	//

	if ( ( iEnd - iBegin ) != iNodeSize )
	{
		return;
		//ASSERT(0);
	}

	int32 iIndexBegin = 0;

	if ( iBegin < 13 )
	{
		iIndexBegin = 13;
	}

	// NodTip����ʼ��
	ASSERT( iNodeSize <= m_aSaneIndexValues.GetSize() );

	aPointsGdi.SetSize(iNodeSize);
	int32 iPointIndex = 0;

	CPen *pOldPen = pDC->SelectObject(&PenRiseDH);

	const T_SaneIndexNode *pLastValidSaneIndexNode = NULL;

	for (int32 i = iIndexBegin ; i < iNodeSize ; i++)
	{
		if ( i + iBegin >= m_aSaneIndexValues.GetSize() )
		{
			if ( i == 0 )	// ֤����Ҫ����
			{
				T_MerchNodeUserData* pMerchData = m_MerchParamArray.GetAt(0);

				// ��ʹUpdateMainKLine����ָ��
				m_bForceCalcSaneIndex = true; 
				UpdateMainMerchKLine(*pMerchData);
			}
			else
			{
				//ASSERT(0);
			}

			return;
		}

		const T_SaneIndexNode &Node    = m_aSaneIndexValues.GetAt(i+iBegin);

		// �ȼ��ϵ�ǰ�������
		CPoint &ptToAdd = aPointsGdi[iPointIndex];
		ptToAdd.x = aDrawParam.GetAt(i).m_CenterPoint.x;

		int32 iY;		
		if ( !pDependCurve->PriceYToRegionY(Node.m_fLineData, iY) )
		{
			// �����������
			continue;
		}

		pLastValidSaneIndexNode = &Node;

		m_pRegionMain->RegionYToClient(iY);
		m_pRegionMain->ClipY(iY);

		ptToAdd.y = iY;
		iPointIndex++;

		//
		if ( i+iBegin+1 < m_aSaneIndexValues.GetSize() )
		{
			const T_SaneIndexNode &NodeAft = m_aSaneIndexValues[i+iBegin+1];			

			if ( Node.m_eSaneMarketInfo == NodeAft.m_eSaneMarketInfo )
			{
				// �ͺ�һ����״̬��ͬ,�Ͳ�����,����
				//NULL;
			}
			else
			{
				// ��ͬ��. ����
				CPen* pDrawPen = NULL;
				if ( ESMIDT == Node.m_eSaneMarketInfo )
				{
					// ����
					pDrawPen = &PenRiseDT;
				}
				else if ( ESMIKT == Node.m_eSaneMarketInfo )
				{
					// �½�
					pDrawPen = &PenFallKT;
				}
				else if ( ESMIDH == Node.m_eSaneMarketInfo )
				{
					// ���
					pDrawPen = &PenRiseDH;
				}
				else if ( ESMIKH == Node.m_eSaneMarketInfo )
				{
					// �ջ�
					pDrawPen = &PenFallKH;
				}

				pDC->SelectObject(pDrawPen);
				pDC->Polyline(aPointsGdi.GetData(), iPointIndex);

				// �ټ��ϵ�ǰ��,��Ϊ��һ�ε����
				aPointsGdi[0] = ptToAdd;
				iPointIndex = 1;
			}	
		}	
	}

	// ���һ��, û�����,��������:
	if ( iPointIndex > 1 && NULL != pLastValidSaneIndexNode )
	{
		CPen* pDrawPen = NULL;
		if ( ESMIDT == pLastValidSaneIndexNode->m_eSaneMarketInfo )
		{					
			pDrawPen = &PenRiseDT;
		}
		else if ( ESMIKT == pLastValidSaneIndexNode->m_eSaneMarketInfo )
		{
			pDrawPen = &PenFallKT;
		}
		else if ( ESMIDH == pLastValidSaneIndexNode->m_eSaneMarketInfo )
		{
			pDrawPen = &PenRiseDH;
		}
		else if ( ESMIKH == pLastValidSaneIndexNode->m_eSaneMarketInfo )
		{
			pDrawPen = &PenFallKH;
		}

		pDC->SelectObject(pDrawPen);
		pDC->Polyline(aPointsGdi.GetData(), iPointIndex);
	}

	pDC->SelectObject(pOldPen);


	// �����:
	int32 iImageWidth  = 15;
	int32 iImageHeight = 20;

	//
	m_aSaneNodeTips.RemoveAll();

	// �������е�ͼ��
	// ���������������У�����̫����
	Rect    rectDrawAll;
	Rect    rectDrawDTOC, rectDrawDTCC, rectDrawKTOC, rectDrawKTCC, rectDrawKCDO, rectDrawDCKO;

	//��ʼ�����־��ε����
	CRect	rectESISDTOC, rectESISDTCC, rectESISKTOC, rectESISKTCC,  rectESISKCDO, rectESISDCKO;

	//--	
	if ( NULL != m_pImageDTOC)
	{
		iImageWidth   = m_pImageDTOC->GetWidth();
		iImageHeight  = m_pImageDTOC->GetHeight();

		rectDrawDTOC.X      = 0;
		rectDrawDTOC.Y      = 0;
		rectDrawDTOC.Width  = iImageWidth;
		rectDrawDTOC.Height = iImageHeight;
	}

	// ��ͷ����
	rectESISDTOC.SetRect(-iImageWidth/2, 0, iImageWidth/2, iImageHeight);
	//--
	if ( NULL != m_pImageDTCC)
	{
		iImageWidth   = m_pImageDTCC->GetWidth();
		iImageHeight  = m_pImageDTCC->GetHeight();

		rectDrawDTCC.X      = rectDrawDTOC.Width;
		rectDrawDTCC.Y      = 0;
		rectDrawDTCC.Width  = iImageWidth;
		rectDrawDTCC.Height = iImageHeight;
	}

	// ��ͷ���
	rectESISDTCC.SetRect(-iImageWidth/2, 0, iImageWidth/2, iImageHeight);
	//--
	if ( NULL != m_pImageKTOC)
	{
		iImageWidth   = m_pImageKTOC->GetWidth();
		iImageHeight  = m_pImageKTOC->GetHeight();

		rectDrawKTOC.X      = rectDrawDTCC.X + rectDrawDTCC.Width;
		rectDrawKTOC.Y      = 0;
		rectDrawKTOC.Width  = iImageWidth;
		rectDrawKTOC.Height = iImageHeight;
	}

	// ��ͷ����
	rectESISKTOC.SetRect(-iImageWidth/2, 0, iImageWidth/2, iImageHeight);
	//--
	if ( NULL != m_pImageKTCC)
	{
		iImageWidth   = m_pImageKTCC->GetWidth();
		iImageHeight  = m_pImageKTCC->GetHeight();

		rectDrawKTCC.X      = rectDrawKTOC.X + rectDrawKTOC.Width;
		rectDrawKTCC.Y      = 0;
		rectDrawKTCC.Width  = iImageWidth;
		rectDrawKTCC.Height = iImageHeight;
	}

	// ��ͷ���
	rectESISKTCC.SetRect(-iImageWidth/2, 0, iImageWidth/2, iImageHeight);
	//--
	if ( NULL != m_pImageKCDO)
	{
		iImageWidth   = m_pImageKCDO->GetWidth();
		iImageHeight  = m_pImageKCDO->GetHeight();

		rectDrawKCDO.X      = rectDrawKTCC.X + rectDrawKTCC.Width;
		rectDrawKCDO.Y      = 0;
		rectDrawKCDO.Width  = iImageWidth;
		rectDrawKCDO.Height = iImageHeight;
	}

	// ����࿪
	rectESISKCDO.SetRect(-iImageWidth/2, 0, iImageWidth/2, iImageHeight);
	//--
	if ( NULL != m_pImageDCKO)
	{
		iImageWidth   = m_pImageDCKO->GetWidth();
		iImageHeight  = m_pImageDCKO->GetHeight();

		rectDrawDCKO.X      = rectDrawKCDO.X + rectDrawKCDO.Width;
		rectDrawDCKO.Y      = 0;
		rectDrawDCKO.Width  = iImageWidth;
		rectDrawDCKO.Height = iImageHeight;
	}

	// ����տ�
	rectESISDCKO.SetRect(-iImageWidth/2, 0, iImageWidth/2, iImageHeight);
	//--

	Rect::Union(rectDrawAll,  rectDrawDTOC, rectDrawDTCC);
	Rect::Union(rectDrawAll,  rectDrawAll,  rectDrawKTOC);
	Rect::Union(rectDrawAll,  rectDrawAll,  rectDrawKTCC);
	Rect::Union(rectDrawAll,  rectDrawAll,  rectDrawKCDO);
	Rect::Union(rectDrawAll,  rectDrawAll,  rectDrawDCKO);


	CDC dcImage;
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, rectDrawAll.Width, rectDrawAll.Height);
	dcImage.CreateCompatibleDC(pDC);
	dcImage.SelectObject(&bmp);
	dcImage.FillSolidRect(rectDrawAll.X, rectDrawAll.Y, rectDrawAll.Width, rectDrawAll.Height, GetIoViewColor(ESCBackground));
	Graphics graTest(dcImage.GetSafeHdc());

	graTest.DrawImage(m_pImageDTOC, rectDrawDTOC, 0, 0, rectDrawDTOC.Width, rectDrawDTOC.Height, UnitPixel);
	graTest.DrawImage(m_pImageDTCC, rectDrawDTCC, 0, 0, rectDrawDTCC.Width, rectDrawDTCC.Height, UnitPixel);
	graTest.DrawImage(m_pImageKTOC, rectDrawKTOC, 0, 0, rectDrawKTOC.Width, rectDrawKTOC.Height, UnitPixel);
	graTest.DrawImage(m_pImageKTCC, rectDrawKTCC, 0, 0, rectDrawKTCC.Width, rectDrawKTCC.Height, UnitPixel);
	graTest.DrawImage(m_pImageKCDO, rectDrawKCDO, 0, 0, rectDrawKCDO.Width, rectDrawKCDO.Height, UnitPixel);
	graTest.DrawImage(m_pImageDCKO, rectDrawDCKO, 0, 0, rectDrawDCKO.Width, rectDrawDCKO.Height, UnitPixel);

	//CRect rcClient;
	//GetClientRect(rcClient);
	//pDC->StretchBlt(rcClient.Width()/2, rcClient.Height()/2, rcClient.Width()/2, rcClient.Height()/2, &dcImage, 0, 0, rectDrawAll.Width, rectDrawAll.Height, SRCCOPY);
	//return;

	int iSaneNodeTipCount	=	0;
	m_aSaneNodeTips.SetSize(iSaneIndexSize);

	Rect rectDrawNeed;
	for (int32 i = 0 ; i < iNodeSize; i++)
	{
		if ( i + iBegin >= iSaneIndexSize )
		{
			break;
		}

		CRect &rectIcon	=	m_aSaneNodeTips[iSaneNodeTipCount].m_RectSign; 
		Image	*pDrawImage	=	0;

		if ( ESISDTOC == m_aSaneIndexValues.GetAt(i+iBegin).m_eSaneIndexState )
		{
			// ��ͷ����
			rectIcon.SetRect(aDrawParam[i].m_CenterPoint.x + rectESISDTOC.left, 
				aDrawParam[i].m_iLow + 3, 
				aDrawParam[i].m_CenterPoint.x + rectESISDTOC.right,
				aDrawParam[i].m_iLow + 3 + rectESISDTOC.Height());

			pDrawImage	=	m_pImageDTOC;
			rectDrawNeed =  rectDrawDTOC;
		}
		else if ( ESISDTCC == m_aSaneIndexValues.GetAt(i+iBegin).m_eSaneIndexState )
		{
			// ��ͷ���
			rectIcon.SetRect(aDrawParam[i].m_CenterPoint.x + rectESISDTCC.left, 
				aDrawParam[i].m_iHigh - 3 - rectESISDTCC.Height(), 
				aDrawParam[i].m_CenterPoint.x + rectESISDTCC.right,
				aDrawParam[i].m_iHigh - 3);

			pDrawImage	=	m_pImageDTCC;
			rectDrawNeed =  rectDrawDTCC;
		}		
		else if ( ESISKTOC == m_aSaneIndexValues.GetAt(i+iBegin).m_eSaneIndexState )
		{

			// ��ͷ����
			rectIcon.SetRect(aDrawParam[i].m_CenterPoint.x + rectESISKTOC.left, 
				aDrawParam[i].m_iHigh - 3 - rectESISKTOC.Height(), 
				aDrawParam[i].m_CenterPoint.x + rectESISKTOC.right,
				aDrawParam[i].m_iHigh - 3);

			pDrawImage	=	m_pImageKTOC;
			rectDrawNeed =  rectDrawKTOC;
		}
		else if ( ESISKTCC == m_aSaneIndexValues.GetAt(i+iBegin).m_eSaneIndexState )
		{

			// ��ͷ���
			rectIcon.SetRect(aDrawParam[i].m_CenterPoint.x + rectESISKTCC.left, 
				aDrawParam[i].m_iLow + 3, 
				aDrawParam[i].m_CenterPoint.x + rectESISKTCC.right,
				aDrawParam[i].m_iLow + 3 + rectESISKTCC.Height());

			pDrawImage	=	m_pImageKTCC;
			rectDrawNeed =  rectDrawKTCC;
		}
		else if ( ESISKCDO == m_aSaneIndexValues.GetAt(i+iBegin).m_eSaneIndexState )
		{

			// ����࿪
			rectIcon.SetRect(aDrawParam[i].m_CenterPoint.x + rectESISKCDO.left, 
				aDrawParam[i].m_iLow + 3, 
				aDrawParam[i].m_CenterPoint.x + rectESISKCDO.right,
				aDrawParam[i].m_iLow + 3 + rectESISKCDO.Height());

			pDrawImage	=	m_pImageKCDO;
			rectDrawNeed =  rectDrawKCDO;
		}
		else if ( ESISDCKO == m_aSaneIndexValues.GetAt(i+iBegin).m_eSaneIndexState)
		{
			// ����տ�
			rectIcon.SetRect(aDrawParam[i].m_CenterPoint.x + rectESISDCKO.left, 
				aDrawParam[i].m_iHigh - 3 - rectESISDCKO.Height(), 
				aDrawParam[i].m_CenterPoint.x + rectESISDCKO.right,
				aDrawParam[i].m_iHigh - 3);

			pDrawImage	=	m_pImageDCKO;
			rectDrawNeed =  rectDrawDCKO;
		}
		else
		{
			pDrawImage	=	NULL;
			if ( !rectIcon.IsRectEmpty() )
			{
				rectIcon.SetRectEmpty();
			}
		}

		if ( NULL != pDrawImage )
		{
			// ��������е���
			//DrawImage(GraphicImage, pDrawImage, rectIcon, 1, 0, FALSE);
			//Rect rc(rectIcon.left, rectIcon.top, rectIcon.Width(), rectIcon.Height());
			//GraphicImage.DrawImage(pDrawImage, rc.X, rc.Y);
			pDC->BitBlt(rectIcon.left, rectIcon.top, rectIcon.Width(), rectIcon.Height(), &dcImage, rectDrawNeed.X, rectDrawNeed.Y, SRCCOPY);

			// ����Ҫ���Ķ���û����ʾ��
			// 
			m_aSaneNodeTips[iSaneNodeTipCount].m_iSaneIndex = i + iBegin;
			ASSERT( m_aSaneNodeTips[iSaneNodeTipCount].m_iSaneIndex >=0 && m_aSaneNodeTips[iSaneNodeTipCount].m_iSaneIndex < m_aSaneIndexValues.GetSize() );
			// 		NodeTip.m_StrTip	= GetNodeTip(bFuture, m_pMerchXml->m_MerchInfo.m_iSaveDec, m_aSaneIndexValues.GetAt(i+iBegin));
			// 		//�������ַ������ַ�����ʵ��Ҫ��ʱ����
			iSaneNodeTipCount++;
		}

	}

	dcImage.DeleteDC();
	bmp.DeleteObject();
	m_aSaneNodeTips.SetSize(iSaneNodeTipCount);
}

void CIoViewKLine::DrawTrade(CMemDCEx* pDC)
{
	// ���Ƚ���ָ��
	if ( NULL == m_pFormularTrade || NULL == m_pOutArrayTrade || NULL == pDC || NULL == m_pRegionMain )
	{
		return;
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

	//
	int32 iNodeSize	= pNode->GetSize();

	CArray<T_DrawTrendParam, T_DrawTrendParam&> aDrawParam;
	aDrawParam.RemoveAll();

	if ( iNodeSize > m_pRegionMain->m_aXAxisNodes.GetSize() )
	{
		T_MerchNodeUserData* pMerchData = m_MerchParamArray.GetAt(0);
		UpdateMainMerchKLine(*pMerchData);		
		return;
	}

	aDrawParam.SetSize(iNodeSize);

	for ( int32 i = 0 ; i < iNodeSize; i++ )
	{
		T_DrawTrendParam &stDrawParam = aDrawParam[i];
		int32 iOpen,iClose;

		// ����̫�� - ���Ż�
		pDependCurve->ValueRegionY(pNode->m_aNodes.GetAt(i).m_fLow, stDrawParam.m_iLow);
		m_pRegionMain->RegionYToClient(stDrawParam.m_iLow);
		m_pRegionMain->ClipY(stDrawParam.m_iLow);

		pDependCurve->ValueRegionY(pNode->m_aNodes.GetAt(i).m_fHigh, stDrawParam.m_iHigh);
		m_pRegionMain->RegionYToClient(stDrawParam.m_iHigh);
		m_pRegionMain->ClipY(stDrawParam.m_iLow);

		pDependCurve->ValueRegionY(pNode->m_aNodes.GetAt(i).m_fOpen, iOpen);
		m_pRegionMain->RegionYToClient(iOpen);
		m_pRegionMain->ClipY(iOpen);

		pDependCurve->ValueRegionY(pNode->m_aNodes.GetAt(i).m_fClose, iClose);
		m_pRegionMain->RegionYToClient(iClose);
		m_pRegionMain->ClipY(iClose);

		stDrawParam.m_CenterPoint.x = m_pRegionMain->m_aXAxisNodes[i].m_iCenterPixel;
		stDrawParam.m_CenterPoint.y = ( iOpen + iClose ) / 2;
	}

	if ( aDrawParam.GetSize() < 0)
	{
		return;
	}

	// ͼ��
	Graphics GraphicImage(pDC->GetSafeHdc());

	// �õ������յ�
	int32 iBegin,iEnd;	
	GetNodeBeginEnd(iBegin,iEnd);

	ASSERT((iEnd - iBegin) == iNodeSize );

	int32 iImageWidth	= 16;
	int32 iImageHeight	= 16;
	CRect rectImage(0,0,0,0);

	Image *pImageTrade = GetTradeImage(0);
	if ( NULL != pImageTrade )
	{
		iImageWidth   = pImageTrade->GetWidth();
		iImageHeight  = pImageTrade->GetHeight();
	}

	for ( int32 iIndexValue = iBegin; iIndexValue < iEnd; iIndexValue++ )
	{
		// ����ͬʱ���ֶ��״̬, ��ʱ��ͼ�겻Ҫ���ص���
		bool32 bSatisfyTop		= false;
		bool32 bSatisfyBottom	= false;

		//
		for ( int32 iIndexLine = 0; iIndexLine < m_pOutArrayTrade->iIndexNum; iIndexLine++ )
		{
			//
			T_IndexOut* pLine = &m_pOutArrayTrade->index[iIndexLine];

			if ( 1 == pLine->pPoint[iIndexValue] )
			{
				// ��������, ׼����ͼ��				
				int32 iIndxeDrawParam = iIndexValue - iBegin;

				//
				if ( 0 == pLine->StrName.CompareNoCase(KStrTradeEnterLong) )
				{
					// ��ͷ����
					rectImage.top	 = aDrawParam[iIndxeDrawParam].m_iLow;
					rectImage.bottom = rectImage.top + iImageHeight;					
					rectImage.left   = aDrawParam[iIndxeDrawParam].m_CenterPoint.x - iImageWidth/2;
					rectImage.right  = aDrawParam[iIndxeDrawParam].m_CenterPoint.x + iImageWidth/2;

					if ( bSatisfyBottom )
					{
						rectImage.top		+= iImageHeight;
						rectImage.bottom	+= iImageHeight;
					}

					//										
					//DrawImage(GraphicImage, m_pImageDTOC, rectImage, 1, 0, false);		
					//DrawImage(GraphicImage, m_pImageRiseFallFlag, rectImage, 6, 1, true);
					// m_ImageListRiseFallFlag.Draw(pDC, 1, rectImage.TopLeft(), ILD_NORMAL);
					DrawTradeImage(GraphicImage, 0, rectImage, true);

					//
					bSatisfyBottom = true;
				}
				else if ( 0 == pLine->StrName.CompareNoCase(KStrTradeExitLong) )
				{
					// ��ͷ����
					rectImage.bottom = aDrawParam[iIndxeDrawParam].m_iHigh;
					rectImage.top	 = rectImage.bottom - iImageHeight;					
					rectImage.left   = aDrawParam[iIndxeDrawParam].m_CenterPoint.x - iImageWidth/2;
					rectImage.right  = aDrawParam[iIndxeDrawParam].m_CenterPoint.x + iImageWidth/2;

					if ( bSatisfyTop )
					{
						rectImage.top		-= iImageHeight;
						rectImage.bottom	-= iImageHeight;
					}

					//					
					// DrawImage(GraphicImage, m_pImageDTCC, rectImage, 1, 0, false);	
					//DrawImage(GraphicImage, m_pImageRiseFallFlag, rectImage, 6, 3, true);
					// m_ImageListRiseFallFlag.Draw(pDC, 3, rectImage.TopLeft(), ILD_NORMAL);
					DrawTradeImage(GraphicImage, 1, rectImage, true);

					//
					bSatisfyTop = true;
				}
				else if ( 0 == pLine->StrName.CompareNoCase(KStrTradeEnterShort) )
				{
					// ��ͷ����
					rectImage.bottom = aDrawParam[iIndxeDrawParam].m_iHigh;
					rectImage.top	 = rectImage.bottom - iImageHeight;					
					rectImage.left   = aDrawParam[iIndxeDrawParam].m_CenterPoint.x - iImageWidth/2;
					rectImage.right  = aDrawParam[iIndxeDrawParam].m_CenterPoint.x + iImageWidth/2;

					if ( bSatisfyTop )
					{
						rectImage.top		-= iImageHeight;
						rectImage.bottom	-= iImageHeight;
					}

					//				
					// DrawImage(GraphicImage, m_pImageKTOC, rectImage, 1, 0, false);
					// DrawImage(GraphicImage, m_pImageRiseFallFlag, rectImage, 6, 1, true);
					// m_ImageListRiseFallFlag.Draw(pDC, 1, rectImage.TopLeft(), ILD_NORMAL);
					DrawTradeImage(GraphicImage, 1, rectImage, false);

					//
					bSatisfyTop = true;
				}
				else if ( 0 == pLine->StrName.CompareNoCase(KStrTradeExitShort) )
				{
					// ��ͷ����
					rectImage.top	 = aDrawParam[iIndxeDrawParam].m_iLow;
					rectImage.bottom = rectImage.top + iImageHeight;					
					rectImage.left   = aDrawParam[iIndxeDrawParam].m_CenterPoint.x - iImageWidth/2;
					rectImage.right  = aDrawParam[iIndxeDrawParam].m_CenterPoint.x + iImageWidth/2;

					if ( bSatisfyBottom )
					{
						rectImage.top		+= iImageHeight;
						rectImage.bottom	+= iImageHeight;
					}

					//					
					// DrawImage(GraphicImage, m_pImageKTCC, rectImage, 1, 0, false);
					// DrawImage(GraphicImage, m_pImageRiseFallFlag, rectImage, 6, 3, true);
					// m_ImageListRiseFallFlag.Draw(pDC, 3, rectImage.TopLeft(), ILD_NORMAL);
					DrawTradeImage(GraphicImage, 0, rectImage, false);

					//
					bSatisfyBottom = true;
				}				
			}			
		}
	}
}

void CIoViewKLine::DrawExpertTradePrompt(CMemDCEx* pDC)
{
	if ( NULL == pDC )
	{
		return;
	}

	//
	if ( NULL == m_pExpertTrade )
	{
		return;
	}

	CArray<UINT, UINT> aTradeTimes;
	m_pExpertTrade->GetTradePoint(aTradeTimes);

	//
	if ( aTradeTimes.GetSize() <= 0 )
	{
		return;
	}

	//
	if ( NULL == m_pRegionMain )
	{
		return;
	}

	//
	CChartCurve* pDependCurve = m_pRegionMain->GetDependentCurve();

	if ( NULL == pDependCurve)
	{
		return;
	}

	// 	
	CNodeSequence* pNode = pDependCurve->GetNodes();	

	if ( NULL == pNode)
	{
		return;
	}

	//
	int32 iNodeSize = pNode->GetSize();

	//
	CNodeData NodeBegin;
	CNodeData NodeEnd;

	pNode->GetAt(0, NodeBegin);
	pNode->GetAt(iNodeSize - 1, NodeEnd);	

	UINT uiTimeBegin = NodeBegin.m_iID;
	UINT uiTimeEnd	 = NodeEnd.m_iID;		

	//
	if ( aTradeTimes.GetAt(0) > uiTimeEnd )
	{
		return;
	}

	if ( aTradeTimes.GetAt(aTradeTimes.GetSize() - 1) < uiTimeBegin )
	{
		return;
	}

	//
	CArray<CPoint, CPoint&> aPointsDraw;

	//
	for ( int32 i = 0; i < iNodeSize; i++ )
	{
		CNodeData NodeNow;
		pNode->GetAt(i, NodeNow);
		UINT uiTimeNow = NodeNow.m_iID;

		//
		for ( int32 j = 0; j < aTradeTimes.GetSize(); j++ )
		{
			UINT uiTimeTrade = aTradeTimes.GetAt(j);

			if ( uiTimeNow == uiTimeTrade )
			{
				// ��¼�������:
				CPoint ptDraw;

				//
				ptDraw.x = m_pRegionMain->m_aXAxisNodes[i].m_iCenterPixel;				

				//
				pDependCurve->ValueRegionY(NodeNow.m_fLow, ptDraw.y);
				m_pRegionMain->RegionYToClient(ptDraw.y);
				m_pRegionMain->ClipY(ptDraw.y);

				//
				aPointsDraw.Add(ptDraw);

				//
				continue;
			}
		}
	}

	//
	Graphics GraphicImage(pDC->GetSafeHdc());

	//
	for (int32 i = 0; i < aPointsDraw.GetSize(); i++ )
	{
		CRect rect;
		rect.left	= aPointsDraw.GetAt(i).x - 8;
		rect.right	= rect.left + 16;
		rect.top    = aPointsDraw.GetAt(i).y + 1;
		rect.bottom = rect.top + 16;

		DrawImage(GraphicImage, m_pImageExpert, rect, 1, 0, false);
	}
}



bool CIoViewKLine::CompareTime(long long Time1, long long Time2)
{
	CTime cTUiTimeTrade(Time1);
	CTime cTUiStrategy(Time2);
	UINT uiTimeTrade;
	UINT uiTimeStrategy;

	uiTimeTrade=cTUiTimeTrade.GetYear();
	uiTimeStrategy =cTUiStrategy.GetYear();
	if (uiTimeTrade != uiTimeStrategy)
		return false;

	uiTimeTrade=cTUiTimeTrade.GetMonth();
	uiTimeStrategy =cTUiStrategy.GetMonth();
	if (uiTimeTrade != uiTimeStrategy)
		return false;

	uiTimeTrade=cTUiTimeTrade.GetDay();
	uiTimeStrategy =cTUiStrategy.GetDay();
	if (uiTimeTrade != uiTimeStrategy)
		return false;

	return true;	
}


void CIoViewKLine::DrawStrategySignal(CMemDCEx* pDC)
{
	if ( NULL == pDC )
	{
		return;
	}


	if (m_mulmapStategyInfo.size() <= 0)
	{
		return;
	}

	//
	if ( NULL == m_pRegionMain )
	{
		return;
	}

	//
	CChartCurve* pDependCurve = m_pRegionMain->GetDependentCurve();

	if ( NULL == pDependCurve)
	{
		return;
	}

	// 	
	CNodeSequence* pNode = pDependCurve->GetNodes();	

	if ( NULL == pNode)
	{
		return;
	}
	//
	int32 iNodeSize = pNode->GetSize();

	//
	CNodeData NodeBegin;
	CNodeData NodeEnd;

	pNode->GetAt(0, NodeBegin);
	pNode->GetAt(iNodeSize - 1, NodeEnd);	

	UINT uiTimeBegin = NodeBegin.m_iID;
	UINT uiTimeEnd	 = NodeEnd.m_iID;		

	m_mulmapStategyRects.clear();
	//
	CArray<CRect, CRect> aRectsDraw;
	for ( int32 i = 0; i < iNodeSize; i++ )
	{
		CNodeData NodeNow;
		pNode->GetAt(i, NodeNow);
		UINT uiTimeNow = NodeNow.m_iID;

		//
		for ( StategyInfoMulmap::iterator it = m_mulmapStategyInfo.begin(); it != m_mulmapStategyInfo.end() ; ++it )
		{
			T_StrategyInfo stStrategyInfo = it->second;

			UINT uiTimeTrade = stStrategyInfo.llStrategyTime;
			// Сϸ�ڣ��ܶ�ʱ�������Ϣ��K����ʾʱ�䲻������ͬ��ʱ���룬����ֻ�жϵ�����
			CTime cTUiTimeTrade(uiTimeNow);
			CTime cTUiStrategy(uiTimeTrade);

			if ( CompareTime(uiTimeNow, uiTimeTrade))
			{
				if (!m_pImageCLBuy)
				{
					return;
				}

				// ��¼�������:
				if (stStrategyInfo.ulSignalType)
				{
					// ���ź�
					CPoint ptDraw;
					ptDraw.x = m_pRegionMain->m_aXAxisNodes[i].m_iCenterPixel;				
					pDependCurve->ValueRegionY(NodeNow.m_fLow, ptDraw.y);
					m_pRegionMain->RegionYToClient(ptDraw.y);
					m_pRegionMain->ClipY(ptDraw.y);

					CRect rect;
					rect.left	= ptDraw.x - m_pImageCLBuy->GetWidth()/2;
					rect.right	= rect.left + m_pImageCLBuy->GetHeight();
					rect.top    = ptDraw.y + 2;
					rect.bottom = rect.top + m_pImageCLBuy->GetHeight();
					aRectsDraw.Add(rect);
					m_mulmapStategyRects.insert(make_pair(uiTimeTrade, rect));	
				}
				else if (!stStrategyInfo.ulSignalType)
				{
					// ���ź�
					CPoint ptDraw;
					ptDraw.x = m_pRegionMain->m_aXAxisNodes[i].m_iCenterPixel;				
					pDependCurve->ValueRegionY(NodeNow.m_fHigh, ptDraw.y);
					m_pRegionMain->RegionYToClient(ptDraw.y);
					m_pRegionMain->ClipY(ptDraw.y);

					CRect rect;
					rect.bottom = ptDraw.y - 1;
					rect.left	= ptDraw.x - m_pImageCLBuy->GetWidth()/2;
					rect.right	= rect.left + m_pImageCLBuy->GetHeight();
					rect.top    = rect.bottom - m_pImageCLBuy->GetHeight();

					aRectsDraw.Add(rect);
					m_mulmapStategyRects.insert(make_pair(uiTimeTrade, rect));
				}
				//
				continue;
			}
		}
	}

	//
	Graphics GraphicImage(pDC->GetSafeHdc());
	SolidBrush solidText(Color(0xff, 255, 255, 255));
	FontFamily  fontFamily(L"����");
	Gdiplus::Font font(&fontFamily, 12, FontStyleBold, UnitPixel);
	//Gdiplus::Font font(&fontFamily, 15, FontStyleRegular, UnitPixel);	//��ʾName������
	StringFormat stringFormat;
	stringFormat.SetLineAlignment(StringAlignmentCenter);	//��ֱ���������ʾ
	stringFormat.SetTrimming(StringTrimmingNone);
	stringFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	for ( StategyRectMulmap::iterator it = m_mulmapStategyRects.begin(); it != m_mulmapStategyRects.end() ; ++it )
	{
		long long llTime = it->first;
		CRect drawRect = it->second;
		CString strCout;
		RectF rectFBuyMul(float(drawRect.left+1), float(drawRect.top+1 + m_pImageCLBuy->GetHeight()/4), float(drawRect.Width()), float(drawRect.Height() - m_pImageCLBuy->GetHeight()/4));
		RectF rectFSell(float(drawRect.left+1), float(drawRect.top-1), float(drawRect.Width()), float(drawRect.Height()));
		if (m_mulmapStategyInfo.count(llTime) > 1)
		{
			DrawImage(GraphicImage, m_pImageCLMul, drawRect, 1, 0, false);

			strCout.Format(_T("%d"), m_mulmapStategyInfo.count(llTime));
			GraphicImage.DrawString(strCout, -1, &font, rectFBuyMul, &stringFormat, &solidText);
		}
		else 
		{
			StategyInfoMulmap::iterator iter   =  m_mulmapStategyInfo.find(llTime);
			if (iter != m_mulmapStategyInfo.end())
			{
				T_StrategyInfo stStrategyInfo = iter->second;
				if (stStrategyInfo.ulSignalType)
				{
					DrawImage(GraphicImage, m_pImageCLBuy, drawRect, 1, 0, false);
					GraphicImage.DrawString(_T("B"), -1, &font, rectFBuyMul, &stringFormat, &solidText);
				}
				else
				{
					DrawImage(GraphicImage, m_pImageCLSell, drawRect, 1, 0, false);
					strCout.Format(_T("%d"), m_mulmapStategyInfo.count(llTime));
					GraphicImage.DrawString(_T("S"), -1, &font, rectFSell, &stringFormat, &solidText);
				}
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////
void CIoViewKLine::InitialImageResource()
{
	// ����
	m_pImageStrongStabilityUp = NULL;
	if( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TREND_INDEX_SSU, L"PNG", m_pImageStrongStabilityUp))
	{
		// ǿ,�ȶ�,����
		m_pImageStrongStabilityUp = NULL;
	}

	m_pImageNormalStabilityUp = NULL;
	if( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TREND_INDEX_NSU, L"PNG", m_pImageNormalStabilityUp))
	{
		// һ��,�ȶ�,����
		m_pImageNormalStabilityUp = NULL;
	}

	m_pImageWeekStabilityUp = NULL;
	if( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TREND_INDEX_WSU, L"PNG", m_pImageWeekStabilityUp))
	{
		// ��,�ȶ�,����
		m_pImageWeekStabilityUp = NULL;
	}

	m_pImageStrongUnStabilityUp = NULL;
	if( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TREND_INDEX_SUSU, L"PNG", m_pImageStrongUnStabilityUp))
	{
		// ǿ,���ȶ�,����
		m_pImageStrongUnStabilityUp = NULL;
	}

	m_pImageNormalUnStabilityUp = NULL;
	if( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TREND_INDEX_NUSU, L"PNG", m_pImageNormalUnStabilityUp))
	{
		// һ��,���ȶ�,����
		m_pImageNormalUnStabilityUp = NULL;
	}

	m_pImageWeekUnStabilityUp = NULL;
	if( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TREND_INDEX_WUSU, L"PNG", m_pImageWeekUnStabilityUp))
	{
		// ��,���ȶ�,����
		m_pImageWeekUnStabilityUp = NULL;
	}


	// �½�
	m_pImageStrongStabilityDown = NULL;
	if( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TREND_INDEX_SSD, L"PNG", m_pImageStrongStabilityDown))
	{
		// ǿ,�ȶ�,�½�
		m_pImageStrongStabilityDown = NULL;
	}

	m_pImageNormalStabilityDown = NULL;
	if( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TREND_INDEX_NSD, L"PNG", m_pImageNormalStabilityDown))
	{
		// һ��,�ȶ�,�½�
		m_pImageNormalStabilityDown = NULL;
	}

	m_pImageWeekStabilityDown = NULL;
	if( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TREND_INDEX_WSD, L"PNG", m_pImageWeekStabilityDown))
	{
		// ��,�ȶ�,�½�
		m_pImageWeekStabilityDown = NULL;
	}

	m_pImageStrongUnStabilityDown = NULL;
	if( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TREND_INDEX_SUSD, L"PNG", m_pImageStrongUnStabilityDown))
	{
		// ǿ,���ȶ�,�½�
		m_pImageStrongUnStabilityDown = NULL;
	}

	m_pImageNormalUnStabilityDown = NULL;
	if( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TREND_INDEX_NUSD, L"PNG", m_pImageNormalUnStabilityDown))
	{
		// һ��,���ȶ�,�½�
		m_pImageNormalUnStabilityDown = NULL;
	}

	m_pImageWeekUnStabilityDown = NULL;
	if( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TREND_INDEX_WUSD, L"PNG", m_pImageWeekUnStabilityDown))
	{
		// ��,���ȶ�,�½�
		m_pImageWeekUnStabilityDown = NULL;
	}

	// ��
	m_pImageShake = NULL;
	if( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TREND_INDEX_SHAKE, L"PNG", m_pImageShake))
	{
		// ��,���ȶ�,�½�
		m_pImageShake = NULL;
	}

	// ��ͷ:
	m_pImageDTOC = NULL;
	if( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TREND_INDEX_DTOC, L"PNG", m_pImageDTOC))
	{
		// ��ͷ����
		m_pImageDTOC = NULL;
	}

	m_pImageDTAC = NULL;
	if( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TREND_INDEX_DTAC, L"PNG", m_pImageDTAC))
	{
		// ��ͷ�Ӳ�
		m_pImageDTAC = NULL;
	}

	m_pImageDTRC = NULL;
	if( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TREND_INDEX_DTRC, L"PNG", m_pImageDTRC))
	{
		// ��ͷ����
		m_pImageDTRC = NULL;
	}

	m_pImageDTCC = NULL;
	if( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TREND_INDEX_DTCC, L"PNG", m_pImageDTCC))
	{
		// ��ͷ���
		m_pImageDTCC = NULL;
	}

	// ��ͷ:
	m_pImageKTOC = NULL;
	if( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TREND_INDEX_KTOC, L"PNG", m_pImageKTOC))
	{
		// ��ͷ����
		m_pImageKTOC = NULL;
	}

	m_pImageKTAC = NULL;
	if( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TREND_INDEX_KTAC, L"PNG", m_pImageKTAC))
	{
		// ��ͷ�Ӳ�
		m_pImageKTAC = NULL;
	}

	m_pImageKTRC = NULL;
	if( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TREND_INDEX_KTRC, L"PNG", m_pImageKTRC))
	{
		// ��ͷ����
		m_pImageKTRC = NULL;
	}

	m_pImageKTCC = NULL;
	if( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TREND_INDEX_KTCC, L"PNG", m_pImageKTCC))
	{
		// ��ͷ���
		m_pImageKTCC = NULL;
	}	

	m_pImageDCKO = NULL;
	if( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TREND_INDEX_DCKO, L"PNG", m_pImageDCKO))
	{
		// ����տ�
		m_pImageDCKO = NULL;
	}	

	m_pImageKCDO = NULL;
	if( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TREND_INDEX_KCDO, L"PNG", m_pImageKCDO))
	{
		// ����࿪
		m_pImageKCDO = NULL;
	}		

	m_pImageZLMM = NULL;
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_ZLMM, L"PNG", m_pImageZLMM))
	{
		m_pImageZLMM = NULL;
	}

	m_pImageExpert = NULL;
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_EXPERT, L"PNG", m_pImageExpert))
	{
		m_pImageExpert = NULL;
	}

	// �������ź�
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDB_PNG_STRATEGY_SELL, L"PNG", m_pImageCLSell))
	{
		m_pImageCLSell = NULL;
	}

	// �������ź�
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDB_PNG_STRATEGY_BUY, L"PNG", m_pImageCLBuy))
	{
		m_pImageCLBuy = NULL;
	}

	// ��������ź�
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDB_PNG_STRATEGY_MUL, L"PNG", m_pImageCLMul))
	{
		m_pImageCLMul = NULL;
	}


	// �ǵ����
	m_pImageRiseFallFlag = NULL;
	if ( NULL == (m_pImageRiseFallFlag = Bitmap::FromResource(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_GRID_REPORT)))
		|| m_pImageRiseFallFlag->GetLastStatus() != Ok )
	{
		DEL(m_pImageRiseFallFlag);
	}

	m_ImageListRiseFallFlag.DeleteImageList();
	m_ImageListRiseFallFlag.Create(IDB_GRID_REPORT, 16, 1, RGB(255,255,255) );
} 

void TestKLine(CArray<CKLine,CKLine>& aKLines,const char * pFilePath,CString StrMerchCode, int32 iMarketId)
{	
	if(NULL == pFilePath)
	{
		return;
	}
	FILE * pfile = fopen(pFilePath,"w+");
	ASSERT( NULL != pFilePath);
	if(NULL == pfile)
	{
		return;
	}

	USES_CONVERSION;
	char* strCode = _W2A(StrMerchCode);
	fprintf(pfile,"%s ,  %d ======================================\r\n", strCode, iMarketId);	

	int32 iSize = aKLines.GetSize();
	bool32 bBug = false;

	for ( int32 i = 0 ; i < iSize; i++)
	{
		CKLine kLine = aKLines.GetAt(i);		
		int32  iTime = kLine.m_TimeCurrent.GetTime();

		if ( i > 0 )
		{
			int32 iTimePre = aKLines.GetAt(i-1).m_TimeCurrent.GetTime();

			if ( iTime < iTimePre )
			{
				fprintf(pfile,"BUG!!!!!!!!!!!!!!!! \r\n");
				//ASSERT(0);
			}
		}

		fprintf(pfile,"iTime = %d ʱ��: %d-%d-%d %d:%d:%d open = %f close = %f high = %f low = %f �ֲ��� = %f �ɽ��� = %f \r\n",
			iTime,
			kLine.m_TimeCurrent.GetYear(),
			kLine.m_TimeCurrent.GetMonth(),
			kLine.m_TimeCurrent.GetDay(),
			kLine.m_TimeCurrent.GetHour(),
			kLine.m_TimeCurrent.GetMinute(),
			kLine.m_TimeCurrent.GetSecond(),
			kLine.m_fPriceOpen,
			kLine.m_fPriceClose,
			kLine.m_fPriceHigh,
			kLine.m_fPriceLow,
			kLine.m_fHold,
			kLine.m_fVolume);

		if ( kLine.m_fPriceOpen <= 0 || kLine.m_fPriceClose <= 0 || kLine.m_fPriceHigh <= 0 || kLine.m_fPriceLow <= 0 || kLine.m_fPriceHigh < kLine.m_fPriceLow)
		{
			bBug = true;
			fprintf(pfile,"********************************************BUG********************************************\r\n");				
			aKLines.RemoveAt(i);
		}
	}						

	fclose(pfile);

	if ( bBug )
	{
		bBug = bBug;
	}
}

void TestNodes(CArray<CNodeData,CNodeData&>& aNodes,const char * pFile,CString StrMerchCode, int32 iMarketId)
{	
	if(NULL == pFile)
	{
		return;
	}
	// ���Լ�¼�ڵ�.
	FILE * pfile = fopen(pFile,"w+");
	ASSERT( NULL != pFile);

	if(NULL == pfile)
	{
		return;
	}

	USES_CONVERSION;
	char* strCode = _W2A(StrMerchCode);
	fprintf(pfile,"%s ,  %d ======================================\r\n", strCode, iMarketId);

	int32 iSize = aNodes.GetSize();
	bool32 bBug = false;

	for ( int32 i = 0 ; i < iSize; i++)
	{
		CNodeData Node = aNodes.GetAt(i);

		int32  iTime = Node.m_iID;

		CGmtTime Time(Node.m_iID);		
		fprintf( pfile,"iTime = %d ʱ��: %d-%d-%d %d:%d:%d open = %f high = %f low = %f close = %f �ֲ��� = %f �ɽ��� = %f \r\n",
			iTime,
			Time.GetYear(),
			Time.GetMonth(),
			Time.GetDay(),
			Time.GetHour(),
			Time.GetMinute(),
			Time.GetSecond(),
			Node.m_fOpen,
			Node.m_fHigh,
			Node.m_fLow,
			Node.m_fClose,
			Node.m_fHold,
			Node.m_fVolume);

		if ( Node.m_fOpen <= 0 || Node.m_fClose <=0 || Node.m_fHigh <= 0 || Node.m_fLow <= 0 || Node.m_fHigh < Node.m_fLow)
		{
			bBug = true;
			fprintf(pfile,"********************************************BUG********************************************\r\n");
			aNodes.RemoveAt(i);				
		}
	}				

	fclose(pfile);
}

bool32 CalcKlineCircAsset( CMerch *pMerch, const CGmtTime &TimeKline, OUT float &fCircAsset )
{
	fCircAsset = 0.0f;
	if ( NULL==pMerch 
		|| NULL == pMerch->m_pFinanceData
		|| 1.0f >= pMerch->m_pFinanceData->m_fCircAsset )
	{
		return false;
	}

	if ( pMerch->m_Market.m_MarketInfo.m_eMarketReportType != ERTStockCn )
	{
		ASSERT( 0 );	// ��֧�ֹ��ڹ�Ʊ
		return false;
	}

	CGmtTime TimeDay = TimeKline;
	SaveDay(TimeDay);

	fCircAsset = pMerch->m_pFinanceData->m_fCircAsset;
	// ��Ȩ����
	int32 iFindPos, iNearSmall, iNearBig;
	int32 iCount = pMerch->m_aWeightDatas.GetSize();
	MYHALF_SEARCH_MEMBER(pMerch->m_aWeightDatas, iCount, TimeDay, iFindPos, iNearSmall, iNearBig, m_TimeCurrent);
	// ��Ȩ��Ϣ�е���ͨ�ɱ���λ ���
	// ָ��ʱ��ǰ(ָ��ʱ������ǳ�Ϣ�գ�ͬ��Ҫȡǰ���)�ĳ�Ȩ��Ϣ�е���ͨ�ɱ�Ϊָ��ʱ�����ͨ�ɱ�
	// ��ǰ�����ͨ�ɱ���Ҫ���ݴκ����ͨ�����͹���Ϣ����
	if ( iNearSmall >=0 && iNearSmall < iCount )
	{
		// ��ǰһ�εĳ�Ȩ��Ϣ
		if ( iNearSmall < iCount-1 )
		{
			fCircAsset = pMerch->m_aWeightDatas[iNearSmall].FlowBase*10000.0f;
		}
		else
		{
			// ���һ�εĳ�Ȩ��Ϣȡ��ǰ�Ĳ������ݵ�ֵ
		}
	}
	else
	{
		// û��ǰһ�εĳ�Ȩ��Ϣ
		int32 iPos = iNearSmall+1;
		if ( iPos >=0 && iPos < iCount )
		{
			// �дκ�ĳ�Ȩ��Ϣ
			// B03��ֵ��λΪ ÿ10�����x���
			const CWeightData &wData = pMerch->m_aWeightDatas[iPos];
			float fCirc = wData.FlowBase*10000.0f;
			const float f10w = 100000.0f;
			fCircAsset  = fCirc / (1.0f + wData.B03/f10w + wData.B04/f10w + wData.B05/f10w);
		}
		else
		{
			ASSERT( iCount == 0 );
			// �����޳�Ȩ��Ϣ�� �Ǿ������ڵ���ͨ�ɱ���
		}
	}
	return true;
}

CString CIoViewKLine::GetTimeString(CGmtTime Time, E_NodeTimeInterval eTimeInterval, bool32 bRecorEndTime /*= false*/)
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

bool32 CIoViewKLine::BeNeedZLMM(T_MerchNodeUserData* pData)
{
	if ( NULL == pData )
	{
		return false;			
	}

	CFileFind filefind;
	if ( filefind.FindFile(L"ZLMM") )
	{
		for (int32 i = 0; i < pData->aIndexs.GetSize(); i++)
		{	
			T_IndexParam* pIndex = pData->aIndexs.GetAt(i);

			if ( NULL == pIndex )
			{
				continue;
			}

			//
			if( 0 == lstrcmp(pIndex->strIndexName, L"ZLMM") )
			{
				return true;					 
			}
		}
	}

	return false;
}

bool32 CIoViewKLine::SpecialForZLMM(T_MerchNodeUserData* pData)
{
	m_aZLMMCrossID.RemoveAll();

	if ( NULL == pData )
	{
		return false;
	}

	if ( pData->m_aKLinesFull.GetSize() <= 0 )
	{
		return false;
	}

	uint32 uIDLast = 0;
	uIDLast = pData->m_aKLinesFull.GetAt(pData->m_aKLinesFull.GetSize() - 1).m_TimeCurrent.GetTime();

	//
	for (int32 i = 0; i < pData->aIndexs.GetSize(); i++)
	{	
		T_IndexParam* pIndex = pData->aIndexs.GetAt(i);

		if ( NULL == pIndex )
		{
			continue;
		}

		//
		if( 0 == lstrcmp(pIndex->strIndexName, L"ZLMM") )
		{
			float fWrite	= 0.;
			float fWritePre	= 0.;

			float fYellow	= 0.;
			float fYellowPre= 0.;

			for ( int32 iIndexPoint = 1; iIndexPoint < ((CNodeSequence*)pIndex->m_aIndexLines[0].m_pNodesShow)->GetSize(); iIndexPoint++ )
			{
				for ( int32 iIndexZLMM = 0; iIndexZLMM < pIndex->m_aIndexLines.GetSize(); iIndexZLMM++ )
				{
					// ����
					if ( ((CNodeSequence*)pIndex->m_aIndexLines[iIndexZLMM].m_pNodesShow)->m_StrName == L"MMS" )
					{								
						CNodeData NodeNow;
						CNodeData NodePre;

						((CNodeSequence*)pIndex->m_aIndexLines[iIndexZLMM].m_pNodesShow)->GetAt(iIndexPoint, NodeNow);
						((CNodeSequence*)pIndex->m_aIndexLines[iIndexZLMM].m_pNodesShow)->GetAt(iIndexPoint - 1, NodePre);

						fWrite		= NodeNow.m_fClose;
						fWritePre	= NodePre.m_fClose;						
					}

					// ����
					if ( ((CNodeSequence*)pIndex->m_aIndexLines[iIndexZLMM].m_pNodesShow)->m_StrName == L"MMM" )
					{								
						CNodeData NodeNow;
						CNodeData NodePre;

						((CNodeSequence*)pIndex->m_aIndexLines[iIndexZLMM].m_pNodesShow)->GetAt(iIndexPoint, NodeNow);
						((CNodeSequence*)pIndex->m_aIndexLines[iIndexZLMM].m_pNodesShow)->GetAt(iIndexPoint - 1, NodePre);

						fYellow		= NodeNow.m_fClose;
						fYellowPre	= NodePre.m_fClose;						
					}
				}

				// �жϻ��ߴ������ϴ�Խ����
				if ( fYellowPre <= fWritePre && fYellow > fWrite )
				{
					// ǰһ�� ����С�ڰ���, ��ǰ����ߴ��ڰ���, ��Խ��
					CNodeData NodeData;
					((CNodeSequence*)pIndex->m_aIndexLines[0].m_pNodesShow)->GetAt(iIndexPoint, NodeData);

					m_aZLMMCrossID.Add(NodeData.m_iID);					

					//					
					if ( NodeData.m_iID == uIDLast )
					{
						CPoint pt = m_pRegionMain->m_RectView.BottomRight();
						ClientToScreen(&pt);

						pt.x -= 3;
						pt.y -= 3;

						CBalloonMsg::RequestCloseAll();
						CBalloonMsg::Show(L"��������Ԥ��", L"������, ��ע��!!!", HICON(2), &pt);
					}
				}
			}					
		}
	}

	return true;
}

bool32 CIoViewKLine::GetChartXAxisSliderText1( OUT CString &StrSlider, CAxisNode &AxisNode )
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
	if (ENTIMinute == eTimeInterval || ENTIMinute5 == eTimeInterval || ENTIMinute15 == eTimeInterval || ENTIMinute30 == eTimeInterval || ENTIMinute60 == eTimeInterval|| ENTIMinute180 == eTimeInterval|| ENTIMinute240 == eTimeInterval || ENTIMinuteUser == eTimeInterval)
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

bool32 CIoViewKLine::GetSaneNodeTip( OUT CString &StrTip, IN int32 iSaneIndex )
{
	StrTip.Empty();

	if ( NULL == m_pMerchXml )
	{
		ASSERT( 0 );
		return false;
	}

	if ( iSaneIndex <0 || iSaneIndex >= m_aSaneIndexValues.GetSize() )
	{
		ASSERT( 0 );
		return false;
	}

	bool32 bFuture = CReportScheme::Instance()->IsFuture(m_pMerchXml->m_Market.m_MarketInfo.m_eMarketReportType);
	StrTip	= GetNodeTip(bFuture, m_pMerchXml->m_MerchInfo.m_iSaveDec, m_aSaneIndexValues.GetAt(iSaneIndex));

	return true;
}

int32 CIoViewKLine::GetSpaceRightCount()
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


void CIoViewKLine::PackJson(string &strTransDataUtf8)
{
	Json::Value jsValue;
	CString strToken;
	string strFieldTemp;

	if (!m_pMerchXml)
	{
		return;
	}

	CGGTongApp *pApp  =  (CGGTongApp*) AfxGetApp();
	CGGTongDoc *pDoc = pApp->m_pDocument;
	if (NULL != pDoc->m_pAutherManager)
	{
		strToken = pDoc->m_pAutherManager->GetInterface()->GetServiceDispose()->GetToken();
	}

	jsValue.clear();
	strFieldTemp = CStringA(strToken);
	jsValue["sessionCode"]		= strFieldTemp;
	jsValue["channel"]			= "PC";
	//jsValue["strategy_code"]	= "qkz_shortstock,qkz_stockpool,long_aggressive_1,long_aggressive_2,long_aggressive_3,long_prudent_1,long_prudent_2,long_prudent_3,long_defensive_1,long_defensive_2,long_defensive_3";
	jsValue["exchange_id"]	= "";
	strFieldTemp =  CStringA(m_pMerchXml->m_MerchInfo.m_StrMerchCode);
	jsValue["code"]	= strFieldTemp;	
	jsValue["market_id"]	= m_pMerchXml->m_MerchInfo.m_iMarketId;
	jsValue["limit_type"]	= "all";
	jsValue["page"]			= 1;
	jsValue["count"]		= 300;	// ��ʱֻ����300�����ݣ�������չ



	//jsValue["limit_type"]		= "all";
	//jsValue["page"]				= 1;

	string strWebDisData = jsValue.toStyledString();

	int index = 0;
	if( !strWebDisData.empty())
	{
		while( (index = strWebDisData.find('\n',index)) != string::npos)
		{
			strWebDisData.erase(index,1);
		}
	}

	CString strJson = strWebDisData.c_str();
	strTransDataUtf8.clear();
	strTransDataUtf8= CStringA(strJson);
	//UnicodeToUtf8(strJson, strTransDataUtf8);
}



void CIoViewKLine::UnPackJson(string strMerchData)
{
	//vector<T_IndustryData> vIndustryData;
	//vIndustryData.clear();

	m_mulmapStategyInfo.clear();

	Json::Value root;
	Json::Value arrayObj;
	Json::Value item;

	Json::Reader reader;
	Json::Value value;
	if (reader.parse(strMerchData.c_str(), value))
	{   
		if (value.size() <= 0)
		{
			return;
		}

		if (!value.isMember("data")) 
		{
			return;
		}
		const Json::Value valueTemp = value["data"];
		if (!valueTemp.isMember("trade_record")) 
		{
			return;
		}

		const Json::Value arrayData = valueTemp["trade_record"];
		for (unsigned int i = 0; i < arrayData.size(); i++)
		{   
			int   nYear,   nMonth,   nDay,   nHour,   nMinite,   nSecond;   
			T_StrategyInfo stStrategy;
			CString strGetTemp;

			int iMarketId = 0;
			CString strMerchCode;
			if (!arrayData[i].isMember("market_id")) 
				continue;
			iMarketId = arrayData[i]["market_id"].asInt();
			if (!arrayData[i].isMember("code")) 
				continue;
			strMerchCode = arrayData[i]["code"].asCString();
			if (m_pMerchXml->m_MerchInfo.m_iMarketId != iMarketId || 0 != m_pMerchXml->m_MerchInfo.m_StrMerchCode.CompareNoCase(strMerchCode))
			{
				return;
			}

			if (!arrayData[i].isMember("strategy_name")) 
				continue;
			stStrategy.strStrategyName = arrayData[i]["strategy_name"].asCString();

			if (!arrayData[i].isMember("price")) 
				continue;
			stStrategy.SugPrice = arrayData[i]["price"].asFloat();

			if (!arrayData[i].isMember("director")) 
				continue;
			strGetTemp = arrayData[i]["director"].asCString();
			stStrategy.ulSignalType = (strGetTemp.CompareNoCase(_T("S"))==0)?0:1;

			if (!arrayData[i].isMember("time")) 
				continue;
			strGetTemp = arrayData[i]["time"].asCString();
			sscanf(CStringA(strGetTemp), "%d-%d-%d   %d:%d:%d",   
				&nYear, &nMonth, &nDay, &nHour, &nMinite, &nSecond);   
			CTime   time(nYear,   nMonth,   nDay,   nHour,   nMinite,   nSecond);
			stStrategy.llStrategyTime = time.GetTime();

			// �������Ĳ����źŲ���ʾ�����������ʱ�䶼�ò������Ͷ�����ʾ��
			CGmtTime gmtTime;
			if (GetNowServerTime(gmtTime))
			{
				CTime Time1(nYear, nMonth, nDay,0,0,0);
				CTime Time2(gmtTime.GetYear(), gmtTime.GetMonth(), gmtTime.GetDay(),0,0,0);
				CTimeSpan timeSpan; 
				timeSpan = Time2 -Time1;
				int iDay = timeSpan.GetDays();
				if (iDay < 6)
				{
					continue;
				}
			}
			else
			{
				continue;
			}
			//strGetTemp += _T("\n");
			//OutputDebugString(strGetTemp);

			m_mulmapStategyInfo.insert(make_pair(stStrategy.llStrategyTime, stStrategy));
		}   
	}
}

void CIoViewKLine::OnAQExtDoQueueItem()
{
	bool bDone = false;
	string strErr = "";
	CString strGateWayIp;
	int		iPort;		

	CGGTongApp *pApp = (CGGTongApp*) AfxGetApp();	
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if (pDoc)
	{
		strGateWayIp = pDoc->GetGateWayIP();
		iPort		 =  pDoc->GetGateWayPort();
	}
	if (strGateWayIp.IsEmpty())
	{
		return;
	}

	// ����ʧ�ܹ����Ͳ�������,����ʧ���˻�ܿ�
	if (!m_bReqStrategy)
	{
		return;
	}


	m_mulmapStategyInfo.clear();

	// ���󱨼۱�ײ��г�
	if (pDoc->m_pAutherManager)
	{
		CString strAddrFormat;
		wstring strAddr;
		strAddrFormat.Format(_T("%s:%d"), strGateWayIp, iPort);
		int iIndex = strAddrFormat.Find(_T("http://"));
		if (-1 == iIndex)
		{
			strAddrFormat = _T("http://") + strAddrFormat;
		}
		strAddr = strAddrFormat;
		string strJson;
		PackJson(strJson);
		pDoc->m_pAutherManager->GetInterface()->ReqStrategyInfo(strAddr.c_str(), L"/statistic/query_strategy_2_stock_trade_record", strJson.c_str());
	}
}


bool32 CIoViewKLine::GetShowClrKLineFlag()
{
	if ( NULL != m_pFormularClrKLine && NULL != m_pOutArrayClrKLine )
	{
		return true;
	}

	return false;
}

bool32 CIoViewKLine::BeSysClrKLine()
{
	if ( NULL == m_pFormularClrKLine || NULL == m_pOutArrayClrKLine )
	{
		return false;
	}

	return m_pFormularClrKLine->m_bSystem;
}

CString CIoViewKLine::GetClrKLineName()
{
	if ( NULL == m_pFormularClrKLine || NULL == m_pOutArrayClrKLine )
	{
		return L"";
	}

	return m_pFormularClrKLine->name;
}

void CIoViewKLine::UpdateShowClrKLine(T_MerchNodeUserData* pData, int32 iShowPosInFullList, int32 iShowCountInFullList)
{	
	if ( NULL == m_pFormularClrKLine || NULL == m_pOutArrayClrKLine || NULL == pData || iShowPosInFullList < 0 || iShowCountInFullList <= 0 )
	{
		return;
	}

	int32 iBegin = iShowPosInFullList;
	int32 iEnd	 = iShowPosInFullList + iShowCountInFullList;
	if ( iBegin < 0 )
	{
		iBegin = 0;
	}

	if ( iEnd > pData->m_aKLinesFull.GetSize() )
	{
		iEnd = pData->m_aKLinesFull.GetSize();
	}

	if ( iBegin > iEnd )
	{
		//ASSERT(0);
	}

	CArray<COLORREF, COLORREF> aColors;
	aColors.SetSize(iShowCountInFullList);
	COLORREF* pColors = (COLORREF*)aColors.GetData();

	int32 iIndexClr = 0;

	for ( int32 i = iBegin; i < iEnd; i++ , iIndexClr++ )
	{
		// Ĭ�����µ�ɫ
		pColors[iIndexClr] = GetIoViewColor(ESCKLineFall);

		// ����ָ���ÿ����. �����
		for ( int32 j = 0; j < m_pOutArrayClrKLine->iIndexNum; j++ )
		{
			if ( m_pOutArrayClrKLine->index[j].pPoint[i] != 0 )
			{
				pColors[iIndexClr] = m_pOutArrayClrKLine->index[j].iColor;
				if ( MAXUINT == pColors[iIndexClr] )
				{
					pColors[iIndexClr] = GetIoViewColor(ESCKLineRise);
				}
			}
		}						
	}

	//
	if ( NULL != m_pRegionMain )
	{
		CChartCurve* pCurve = m_pRegionMain->GetDependentCurve();
		if ( NULL != pCurve )
		{
			CChartDrawer* pDrawer = (CChartDrawer*)pCurve->GetDrawer();
			if ( NULL != pDrawer )
			{
				pDrawer->SetColorFlag(true);
				pDrawer->SetColors(aColors);
			}
		}
	}
}

bool32 CIoViewKLine::FromXmlInChild( TiXmlElement *pTiXmlElement )
{
	// ����ֻ��ʼ����ʹ�ʽ
	// ��ʼ��Ĭ�ϸ�ͼ����
	ASSERT( NULL != pTiXmlElement );

	const char *pClrFormularName = pTiXmlElement->Attribute(KXMLIoViewKLineClrFormularName);
	if ( NULL != pClrFormularName )
	{
		CString StrFormularName;
		::MultiByteToWideChar(CP_UTF8, 0, pClrFormularName, -1, StrFormularName.GetBuffer(2048), 2048);
		StrFormularName.ReleaseBuffer();

		CFormulaLib *pLib = CFormulaLib::instance();
		// ϵͳ
		CIndexGroupClrKLine* pGroupClrKLine = NULL;

		int32 i = 0;
		CFormularContent *pContentFind = NULL;
		if ( NULL != pLib->m_SysIndex.m_pGroupClrKLine )
		{
			pGroupClrKLine = pLib->m_SysIndex.m_pGroupClrKLine;

			for ( i = 0; i < pGroupClrKLine->m_ContentsClrKLine.GetSize(); i++ )
			{
				CFormularContent* pContent = pGroupClrKLine->m_ContentsClrKLine.GetAt(i);
				if ( NULL == pContent )
				{
					continue;
				}

				if ( pContent->name == StrFormularName )
				{
					pContentFind = pContent;
					break;
				}
			}
		}

		// �û�
		if ( NULL != pLib->m_UserIndex.m_pGroupClrKLine && NULL == pContentFind )
		{
			pGroupClrKLine = pLib->m_UserIndex.m_pGroupClrKLine;

			for ( i = 0; i < pGroupClrKLine->m_ContentsClrKLine.GetSize(); i++ )
			{
				CFormularContent* pContent = pGroupClrKLine->m_ContentsClrKLine.GetAt(i);
				if ( NULL == pContent )
				{
					continue;
				}
				if ( pContent->name == StrFormularName )
				{
					pContentFind = pContent;
					break;
				}
			}
		}

		if ( NULL != pContentFind )
		{
			DoAddClrKLine(pContentFind); // ��ʱ�ʺϳ�ʼ������
		}
	}

	// ���һ����ͼ
	const char *pszPickSubRegionIndex = pTiXmlElement->Attribute(KXMLAttriIoViewKLinePickSubRegionIndex);
	if ( NULL != pszPickSubRegionIndex )
	{
		m_iLastPickSubRegionIndex = atoi(pszPickSubRegionIndex);
		// ������Խ�����
	}

	return true;
}

CString CIoViewKLine::ToXmlInChild()
{
	CString StrRet;
	// ���
	if ( NULL != m_pFormularClrKLine )
	{
		StrRet.Format(_T(" %s=\"%s\" "),
			CString(KXMLIoViewKLineClrFormularName).GetBuffer(), m_pFormularClrKLine->name.GetBuffer());
	}

	if ( m_iLastPickSubRegionIndex >= 0 && m_iLastPickSubRegionIndex < m_SubRegions.GetSize() )
	{
		CString StrIndex;
		StrIndex.Format(_T(" %s=\"%d\" "), CString(KXMLAttriIoViewKLinePickSubRegionIndex).GetBuffer(), m_iLastPickSubRegionIndex);
		StrRet += StrIndex;
	}

	return StrRet;
}

void CIoViewKLine::DoAddClrKLine( CFormularContent *pFormularContent )
{
	if ( NULL != m_pFormularClrKLine )
	{
		OnDelClrKLine();
	}
	m_pFormularClrKLine = pFormularContent;

	if ( NULL != m_pFormularClrKLine )
	{
		if ( m_MerchParamArray.GetSize() > 0 && NULL != m_pRegionMain )
		{
			CChartCurve* pCurve = m_pRegionMain->GetDependentCurve();
			if ( NULL != pCurve )
			{
				CChartDrawer* pDrawer = (CChartDrawer*)pCurve->GetDrawer();
				if ( NULL != pDrawer )
				{
					pDrawer->SetColorFlag(true);
				}
			}

			T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(0);
			UpdateMainMerchKLine(*pData, true);
		}			
	}

	if ( NULL != m_pRegionMain )
	{
		m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
		Invalidate();
	}
}

void CIoViewKLine::AddClrKLine( const CString &StrKLineName )
{
	CFormulaLib *pLib = CFormulaLib::instance();
	// ϵͳ
	CIndexGroupClrKLine* pGroupClrKLine = NULL;

	int32 i = 0;
	CFormularContent *pContentFind = NULL;
	if ( NULL != pLib->m_SysIndex.m_pGroupClrKLine )
	{
		pGroupClrKLine = pLib->m_SysIndex.m_pGroupClrKLine;

		for ( i = 0; i < pGroupClrKLine->m_ContentsClrKLine.GetSize(); i++ )
		{
			CFormularContent* pContent = pGroupClrKLine->m_ContentsClrKLine.GetAt(i);
			if ( NULL == pContent )
			{
				continue;
			}

			if ( pContent->name == StrKLineName )
			{
				pContentFind = pContent;
				break;
			}
		}
	}

	// �û�
	if ( NULL != pLib->m_UserIndex.m_pGroupClrKLine && NULL == pContentFind )
	{
		pGroupClrKLine = pLib->m_UserIndex.m_pGroupClrKLine;

		for ( i = 0; i < pGroupClrKLine->m_ContentsClrKLine.GetSize(); i++ )
		{
			CFormularContent* pContent = pGroupClrKLine->m_ContentsClrKLine.GetAt(i);
			if ( NULL == pContent )
			{
				continue;
			}
			if ( pContent->name == StrKLineName )
			{
				pContentFind = pContent;
				break;
			}
		}
	}

	if ( NULL != pContentFind )
	{
		DoAddClrKLine(pContentFind); // ��ʱ�ʺϳ�ʼ������
	}
}

void CIoViewKLine::DrawWeightInfo( CMemDCEx *pDC )
{
	m_aWeightRects.RemoveAll();

	if ( NULL == pDC 
		|| m_MerchParamArray.GetSize() < 1 
		|| m_MerchParamArray[0] == NULL
		|| m_MerchParamArray[0]->m_pMerchNode == NULL )
	{
		return;
	}

	// ����
	//if ( m_MerchParamArray[0]->m_eTimeIntervalFull != ENTIDay )
	//{
	//	// ���еĶ�����
	//	return;
	//}

	// ��������: ��ͷ����,��ͷ����,�𵴻���
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

	T_MerchNodeUserData *pData = m_MerchParamArray[0];
	CMerch *pMerch = pData->m_pMerchNode;
	if ( pMerch->m_aWeightDatas.GetSize() < 1 )
	{
		return;
	}

	if ( m_pRegionMain->m_aXAxisNodes.GetSize() < pNode->GetNodes().GetSize() )
	{
		ASSERT( 0 );
		return;
	}

	// ����x����

	m_aWeightRects.SetSize(0, pMerch->m_aWeightDatas.GetSize());

	time_t tStart, tEnd;
	int32 iNodeSize = m_pRegionMain->m_aXAxisNodes.GetSize();
	CAxisNode	nodeStart = m_pRegionMain->m_aXAxisNodes[0];

	CAxisNode	nodeEnd	  = m_pRegionMain->m_aXAxisNodes[ pNode->GetNodes().GetSize()-1 ];


	tStart = m_pRegionMain->m_aXAxisNodes[0].m_iTimeId;
	tEnd   = nodeEnd.m_iTimeId;
	//const float fRange = tEnd - tStart;
	CRect rcRegion(0,0,0,0);
	rcRegion.top	= m_pRegionMain->GetRectCurves().bottom;
	rcRegion.bottom = m_pRegionMain->m_RectView.bottom;
	rcRegion.left	= 0;
	rcRegion.right	= 15;
	int32 iSaveDC = pDC->SaveDC();
	pDC->SelectObject(CFaceScheme::Instance()->GetSysFontObject(ESFSmall));
	pDC->SetTextColor(GetIoViewColor(ESCRise));
	pDC->SetBkMode(TRANSPARENT);
	int32 iNodeHasFound = 0;	// ˳����� - ����д���˳��ľ����˹�
	for ( int32 i=0; i < pMerch->m_aWeightDatas.GetSize() ; i++ )
	{
		CRect rcNode(0,0,0,0);
		CGmtTime timeNode = pMerch->m_aWeightDatas[i].m_TimeCurrent;
		//SaveDay(timeNode);
		time_t tNode = timeNode.GetTime();
		if ( tNode >= tStart && tNode <= tEnd && IsWeightDataCanShow(pMerch->m_aWeightDatas[i]) )
		{
			int32 iCenter = nodeStart.m_iCenterPixel;


			// һ��һ����
			for ( ; iNodeHasFound < iNodeSize ; iNodeHasFound++ )
			{
				if ( m_pRegionMain->m_aXAxisNodes[iNodeHasFound].m_iTimeId >= tNode )
				{
					CGmtTime time1(m_pRegionMain->m_aXAxisNodes[iNodeHasFound].m_iTimeId);
					CGmtTime time2(tNode);
					SaveDay(time1);
					SaveDay(time2);
					if ( time1.GetTime() != time2.GetTime() )
					{
						iNodeHasFound--;		// ����������һ���ڵ�
						ASSERT( iNodeHasFound >= 0 );
						CGmtTime time3(m_pRegionMain->m_aXAxisNodes[iNodeHasFound].m_iTimeId);
						//						ASSERT( time3.GetDay() == time2.GetDay() );
					}
					break;
				}
			}


			if ( iNodeHasFound >= 0 && iNodeHasFound < iNodeSize )
			{
				iCenter = m_pRegionMain->m_aXAxisNodes[iNodeHasFound].m_iCenterPixel;

				rcNode = rcRegion;
				rcNode.left  += iCenter - 7;
				rcNode.right += iCenter - 7;
				//pDC->FillSolidRect(rcNode, RGB(255,0,0));
				pDC->DrawText(_T("��"), rcNode, DT_CENTER| DT_BOTTOM);	
			}

			//TRACE(_T("WeightNode: %04d-%02d-%02d  %d\r\n"), timeNode.GetYear(), timeNode.GetMonth(), timeNode.GetDay(), iStart);
		}
		m_aWeightRects.Add(rcNode);
	}
	pDC->RestoreDC(iSaveDC);
}

bool32 CIoViewKLine::IsWeightDataCanShow( const CWeightData &wData ) const
{
	return wData.m_TimeCurrent.GetTime() > 0 &&
		( wData.B01 > 0.0 || wData.B02 > 0.0 || wData.B03 > 0.0 || wData.B04 > 0.0 || wData.B05 > 0.0 );
}

void CIoViewKLine::OnPickChart( CChartRegion *pRegion, int32 x, int32 y, int32 iFlag )
{
	for ( int32 i=0; i < m_SubRegions.GetSize() ; i++ )
	{
		if ( pRegion == m_SubRegions[i].m_pSubRegionMain
			|| pRegion == m_SubRegions[i].m_pSubRegionYLeft )
		{
			int32 iOld = m_iLastPickSubRegionIndex;
			m_iLastPickSubRegionIndex = i;
			m_pRegionPick = pRegion;
			if ( iOld != m_iLastPickSubRegionIndex )
			{
				m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
				Invalidate();
			}
			return;
		}
	}
}

void CIoViewKLine::GetCurrentIndexNameArray( OUT CStringArray &aIndexNames )
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

	if ( NULL != m_pFormularClrKLine )
	{
		// ���
		aIndexNames.Add(m_pFormularClrKLine->name);
	}
	if ( NULL != m_pExpertTrade )
	{
		// ��ʱ�Ͱ������ָ�����ƣ�
		//aIndexNames.Add(m_pExpertTrade->GetTradeString(m_pExpertTrade->GetTradeType()));
	}
	if ( NULL != m_pFormularTrade )
	{
		// ����ָ��
		aIndexNames.Add(m_pFormularTrade->name);
	}
}

void CIoViewKLine::ChangeToNextIndex(bool32 bPre)
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

	//
	for ( int32 i = 0; i < pData->aIndexs.GetSize(); i++ )
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

	for (int32 i = 0; i < aFormularNames.GetSize(); i++ )
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

void CIoViewKLine::DrawUserNotes( CMemDCEx *pDC )
{
	m_mapUserNotesRect.RemoveAll();

	if ( NULL == pDC 
		|| m_MerchParamArray.GetSize() < 1 
		|| m_MerchParamArray[0] == NULL
		|| m_MerchParamArray[0]->m_pMerchNode == NULL )
	{
		return;
	}

	T_MerchNodeUserData *pData = m_MerchParamArray[0];

	// ����
	if ( m_MerchParamArray[0]->m_eTimeIntervalFull != ENTIDay )
	{
		return;
	}

	CUserNoteInfoManager::UserNotesArray aUserNotes;
	CMerchKey keyMerch(m_MerchParamArray[0]->m_pMerchNode->m_MerchInfo.m_iMarketId, m_MerchParamArray[0]->m_pMerchNode->m_MerchInfo.m_StrMerchCode);
	CUserNoteInfoManager::Instance().GetUserNotesHeaderArray(keyMerch, aUserNotes);
	if ( aUserNotes.size() <= 0 )
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

	CRect rcRegion(0,0,0,0);
	rcRegion.top	= m_pRegionMain->GetRectCurves().bottom;
	rcRegion.bottom = m_pRegionMain->m_RectView.bottom;
	rcRegion.left	= 0;
	rcRegion.right	= 15;
	int32 iSaveDC = pDC->SaveDC();
	pDC->SelectObject(CFaceScheme::Instance()->GetSysFontObject(ESFSmall));
	pDC->SetTextColor(RGB(255,0,255));
	pDC->SetBkMode(TRANSPARENT);

	// ʱ���Ļ���
	//   �Դ������ʱ��㣬���ǵ�ǰʱ���
	// ����x����

	const int32 iPosStart = 0;
	const int32 iPosCount = pNode->GetNodes().GetSize();
	CAxisNode	nodeStart = m_pRegionMain->m_aXAxisNodes[iPosStart];
	CAxisNode	nodeEnd	  = m_pRegionMain->m_aXAxisNodes[ iPosCount-1 ];

	CUserNoteInfoManager::UserNotesArray::reverse_iterator it = aUserNotes.rbegin();
	// ����ռ�ʱ�����ʾ�����Ҳ��+1�ĵ��ʱ�仹������ʾ���ռǣ�������Ҳ�+1�㣬�򲻿������ռǵĹ���
	CGmtTime timeEnd(nodeEnd.m_iTimeId);
	timeEnd += CGmtTimeSpan(0, 23, 59, 59);
	int32 iPosEndPlus = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pData->m_aKLinesFull, timeEnd);
	if ( iPosEndPlus >= 0 && iPosEndPlus < pData->m_aKLinesFull.GetSize() )
	{
		// ӵ���Ҳ�+1ֵ, ���˵�����>=�Ҳ�ʱ����ֵ
		timeEnd = pData->m_aKLinesFull[iPosEndPlus].m_TimeCurrent;
		while ( it != aUserNotes.rend() && it->m_Time >= timeEnd )
		{
			++it;
		}
	}
	for ( int32 i = iPosCount+iPosStart-1; i >= iPosStart ; i-- )
	{
		bool32 bNeedDraw = true;
		int32 iTimeNow = m_pRegionMain->m_aXAxisNodes[i].m_iTimeId;
		while ( it != aUserNotes.rend() && it->m_Time.GetTime() > iTimeNow )
		{
			// Ͷ���ռǵ�ʱ�����ڸ�ʱ���, �����ڸ�ʱ������
			int32 iCenter = m_pRegionMain->m_aXAxisNodes[i].m_iCenterPixel;

			CRect rcNode(rcRegion);
			rcNode.left  += iCenter - 7;
			rcNode.right += iCenter - 7;

			if ( bNeedDraw )
			{
				pDC->DrawText(_T("��"), rcNode, DT_CENTER| DT_BOTTOM);
				bNeedDraw = false;
			}

			m_mapUserNotesRect[ it->m_iUniqueId ] = rcNode;

			++it;
		}

		if ( it == aUserNotes.rend() )
		{
			break;
		}
	}

	pDC->RestoreDC(iSaveDC);
}

void CIoViewKLine::OnChartDBClick( CPoint ptClick, int32 iNodePos )
{
	if ( m_MerchParamArray.GetSize() <= 0
		|| m_MerchParamArray[0] == NULL
		|| m_MerchParamArray[0]->m_pMerchNode == NULL )
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

	// �Ƿ�˫������Ͷ����־��
	POSITION pos = m_mapUserNotesRect.GetStartPosition();
	CArray<int32, int32> aIds;
	while ( NULL != pos )
	{
		int32 iUniqueId;
		CRect rc;
		m_mapUserNotesRect.GetNextAssoc(pos, iUniqueId, rc);
		if ( rc.PtInRect(ptClick) )
		{
			// ��ʾͶ����־
			// ��ȷ�ĸ���
			CUserNoteInfoManager::UserNotesArray aNotes;
			CUserNoteInfoManager::Instance().GetUserNotesHeaderArray(m_MerchXml, aNotes);
			for (unsigned int i=0; i < aNotes.size() ; i++ )
			{
				if ( aNotes[i].m_iUniqueId == iUniqueId )
				{
					CDlgNotePad::ShowNotePad(CDlgNotePad::EST_UserNote, &aNotes[i].m_Time);
					return;
				}
			}
		}
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
				int32 iHour, iMinute, iSecond;
				iYear = iMonth = iDay = iHour = iMinute = iSecond = 0;
				if (_stscanf(StrMineTime, _T("%d-%d-%d %d:%d:%d"), &iYear, &iMonth, &iDay, &iHour, &iMinute, &iSecond) >= 3)
				{
					CGmtTime TimeClick(iYear, iMonth, iDay, iHour, iMinute, iSecond);
					CDlgNotePad::ShowNotePad(CDlgNotePad::EST_InfoMine, &TimeClick);
					return;
				}
			}	
		}
	}
}

void CIoViewKLine::FireIoViewChouMa( int32 iFlag, WPARAM w, LPARAM l )
{
	T_ChouMaNotify *pNotify = new T_ChouMaNotify;
	pNotify->pIoViewKLine = this;
	pNotify->iFlag = iFlag;
	pNotify->wParam = w;
	pNotify->lParam = l;

	// ��Ҫ��ͬһ����Ϣ�����д����������k�ߵĻ����ӳ�, й©Ӧ�ò���̫���
	PostMessage(TEMP_UM_CHOUMANOTIFY, (WPARAM)pNotify, 0);
}

void CIoViewKLine::FireIoViewChouMaAxisChange()
{
	CPoint ptScreen;
	GetCursorPos(&ptScreen);
	CPoint pt = ptScreen;
	ScreenToClient(&pt);
	CRect rcClient;
	GetClientRect(rcClient);
	if (NULL != m_pRegionMain)
	{
		CChartCurve* pCurveDependent = m_pRegionMain->GetDependentCurve();
		if (NULL == pCurveDependent)
			return;

		int32 rx = pt.x;
		m_pRegionMain->ClientXToRegion(rx);
		int32 iPos = 0;
		if ( pCurveDependent->RegionXToCurvePos(rx, iPos) 
			&& rcClient.PtInRect(pt)
			&& WindowFromPoint(ptScreen) == this )
		{
			ASSERT( sizeof(float) == sizeof(DWORD) );
			int32 iY = pt.y;
			float fPrice = 0.0f;
			m_pRegionMain->ClientYToRegion(iY);
			pCurveDependent->RegionYToPriceY(iY, fPrice);
			FireIoViewChouMa(2, *(WPARAM *)(&fPrice), m_pRegionMain->m_aXAxisNodes[iPos].m_iTimeId);
		}
		else
		{
			FireIoViewChouMa(4, NULL, NULL);	// ������Ҫ�ؼ���
		}
	}
}

void CIoViewKLine::OnMouseMove( UINT nFlags, CPoint point )
{	
	// region֪ͨ������ˣ�ֱ����������ƶ���Ȼ���ڽ���chart����
	if (NULL != m_pRegionMain)
	{
		CChartCurve* pCurveDependent = m_pRegionMain->GetDependentCurve();
		if (NULL == pCurveDependent)
			return;

		CNodeSequence *pNodes = pCurveDependent->GetNodes();

		int32 rx = point.x;
		m_pRegionMain->ClientXToRegion(rx);
		int32 iPos = 0;
		int32 iY = point.y;
		float fPrice = 0.0f;
		m_pRegionMain->ClientYToRegion(iY);
		pCurveDependent->RegionYToPriceY(iY, fPrice);

		if ( pCurveDependent->RegionXToCurvePos(rx, iPos) && m_pRegionMain->m_aXAxisNodes[iPos].m_iTimeId > 0 )
		{
			FireIoViewChouMa(2, *(WPARAM *)(&fPrice), m_pRegionMain->m_aXAxisNodes[iPos].m_iTimeId);
		}
		else if ( pNodes->GetSize() > 0 )
		{
			FireIoViewChouMa(2, *(WPARAM *)(&fPrice), pNodes->GetNodes()[pNodes->GetNodes().GetUpperBound()].m_iID);
		}
		else
		{
			FireIoViewChouMa(2, *(WPARAM *)(&fPrice), 0);
		}
	}

	// chart����
	CIoViewChart::OnMouseMove(nFlags, point);
}

LRESULT CIoViewKLine::OnNotifyChouMa( WPARAM w, LPARAM l )
{
	T_ChouMaNotify *pNotify = (T_ChouMaNotify *)w;
	if ( NULL != w )
	{
		CIoViewChouMa::OnChouMaNotify(*pNotify);
		delete pNotify;	// post��ָ��
	}
	return 1;
}

//lint --e{429}
bool32 CIoViewKLine::AddCmpMerch(CMerch *pMerchToAdd, bool32 bPrompt, bool32 bReqData, bool32 bDrawLine)
{
	// ����Ƿ��Ѵ���
	ASSERT( m_MerchParamArray.GetSize() > 0 );
	ASSERT( m_pMerchXml != NULL );
	if ( m_pMerchXml == NULL || m_MerchParamArray.GetSize() <= 0 )
	{
		return false;
	}

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
		if ( GetMainCurveAxisYType() != CPriceToAxisYObject::EAYT_Pecent 
			&& MessageBox(_T("�Ƿ��л����ٷֱ����ꣿ"), _T("����Ʒ��"), MB_YESNO |MB_ICONQUESTION) == IDYES )
		{
			SetMainCurveAxisYType(CPriceToAxisYObject::EAYT_Pecent);
		}
	}

	CArray<COLORREF, COLORREF> aClrs;
	aClrs.SetSize(3); 
	aClrs[0] = RGB(127,127,127);
	aClrs[1] = RGB(128,64,0);
	aClrs[2] = RGB(0,150, 84);
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
		if ( !bInherit )
		{
			// ������Ҫ���� ����
			CNodeSequence* pNodes = CreateNodes();
			pNodes->SetUserData(pData);
			pData->m_pKLinesShow = pNodes;

			if (bDrawLine)
			{
				// new 1 curves/nodes/...
				CChartCurve* pCurve = m_pRegionMain->CreateCurve(CChartCurve::KRequestCurve|CChartCurve::KTypeKLine|CChartCurve::KYTransformByLowHigh|CChartCurve::KUseNodesNameAsTitle|CChartCurve::KYTransformToAlignDependent);		 
				CChartDrawer* pKLineDrawer = new CChartDrawer(*this, (CChartDrawer::E_ChartDrawStyle)GetCurChartKlineDrawType());
				pKLineDrawer->SetSingleColor(clrLine);

				pCurve->AttatchDrawer(pKLineDrawer);
				pCurve->AttatchNodes(pNodes);
				pCurve->m_clrTitleText = clrLine;
			}
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

void CIoViewKLine::RemoveCmpMerch( CMerch *pMerch )
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
		if ( m_MerchParamArray.GetSize() <= 1 )
		{
			SetMainCurveAxisYType(CPriceToAxisYObject::EAYT_Normal);	// ��ԭ��ͨ����
		}
		if ( NULL != m_pRegionMain )
		{
			m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
			Invalidate();
		}
	}
}

void CIoViewKLine::RemoveAllCmpMerch()
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
		for ( int32 i=m_aSmartAttendMerchs.GetUpperBound(); i >= 0; i-- )
		{
			if ( m_aSmartAttendMerchs[i].m_pMerch != m_pMerchXml )
			{
				m_aSmartAttendMerchs.RemoveAt(i);
			}
		}
		if ( m_MerchParamArray.GetSize() <= 1 )
		{
			SetMainCurveAxisYType(CPriceToAxisYObject::EAYT_Normal);	// ��ԭ��ͨ����
		}
		if ( NULL != m_pRegionMain )
		{
			m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
			Invalidate();
		}
	}
}

bool32 CIoViewKLine::ChangeMainDataShowData( T_MerchNodeUserData &MainMerchNode, int32 iShowPosInFullList, int32 iShowCountInFullList, bool32 bPreShowNewest, const CGmtTime &TimePreStart, const CGmtTime &TimePreEnd )
{
	if (!MainMerchNode.UpdateShowData(iShowPosInFullList, iShowCountInFullList))
	{
		//ASSERT(0);
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
		// ��� K ��
		UpdateShowClrKLine(&MainMerchNode, iShowPosInFullList, iShowCountInFullList);

		// zhangbo 20090720 #�����䣬 ������Ʒ���ݸ���
		//...
		// ��ʾʱ����Ѿ����������������ʱ���µ���K��
		// ���е�����Ʒ������
		if ( TimeNowStart != TimePreStart
			|| TimeNowEnd != TimePreEnd )
		{
			// ��ʾʱ����Ѿ����������������ʱ���µ���K��
			// ���е�����Ʒ������
			OnCmpMerchsNeedUpdate(false);
		}

		// ���¼�������
		UpdateAxisSize();

	}

	// 
	if (bIsShowNewestNow == bPreShowNewest)
	{
		// ǰ��û�и���, �������ڵĵ�ǰ��ʾk �߲�ͬ��. ��������� showh showl ָ��Ļ�, ��Ҫ����
		bool32 bNeedCalcIndex = false;
		for (int32 i = 0; i < MainMerchNode.aIndexs.GetSize(); i++)
		{
			T_IndexParam* p = MainMerchNode.aIndexs[i];
			if (NULL == p)
			{
				continue;
			}

			//
			if (p->bScreenAccroding)
			{
				bNeedCalcIndex = true;
				break;
			}
		}

		if (bNeedCalcIndex)
		{
			UpdateMainMerchKLine(MainMerchNode, true);
		}
	}

	return true;
}

bool32 CIoViewKLine::CalcKLinePriceBaseValue( CChartCurve *pCurve, OUT float &fPriceBase )
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

void CIoViewKLine::RequestSingleCmpMerchViewData( CMerch *pMerch )
{
	if ( NULL == pMerch )
	{
		ASSERT( 0 );
		return;
	}

	T_MerchNodeUserData *pMainData = NULL;
	if ( m_MerchParamArray.GetSize() < 0 || (pMainData=m_MerchParamArray[0]) == NULL )
	{
		ASSERT( 0 );
		return;
	}

	// ��ȡ����Ʒ�����ڵ���Ϣ, ����ʵ�����ݣ�һ����ס��ƷΪ��
	E_NodeTimeInterval eNodeTimeIntervalCompare = ENTIDay;
	E_KLineTypeBase	   eKLineTypeCompare		= EKTBDay;

	int32 iScale = 1;
	if (!GetTimeIntervalInfo(pMainData->m_eTimeIntervalFull, eNodeTimeIntervalCompare, eKLineTypeCompare, iScale))
	{
		//ASSERT(0);	// ��Ӧ�ó����������
		return;
	}

	// ���µ�����Ʒ��TimeInterval

	//�����Ƿ�Ӧ���������� - �������Ƚϵ�λʹ�û����� K�� ���ڵ�λ
	CGmtTime TimeSpecify		= m_pAbsCenterManager->GetServerTime();
	// SaveDay(TimeSpecify);
	int32 iNeedKLineCount		= 1;
	int32 iNeedKLineCountForSane = KMinRequestKLineNumber * iScale;			// ������ָ������Ҫ����С��NodeCount

	const int32 iSpaceRightCount = GetSpaceRightCount();
	iNeedKLineCount = MAX(iNeedKLineCountForSane, (m_iNodeCountPerScreen - iSpaceRightCount) * iScale);   // ѡȡ������������Ҫ�������KLine����

	// ������Ʒ���������е���ʾ��Ҫ��K�����ݣ������Ƕ���
	//
	{
		// �����
		//TRACE(_T("-------------------------------------------����[%d]\r\n"), iNeedKLineCount);
		if (iNeedKLineCount < KMinRequestKLineNumber)
		{
			iNeedKLineCount = KMinRequestKLineNumber;
		}

		//
		CMmiReqMerchKLine info; 
		info.m_eKLineTypeBase	= eKLineTypeCompare;								// ʹ��ԭʼK������
		info.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
		info.m_StrMerchCode		= pMerch->m_MerchInfo.m_StrMerchCode;

		info.m_eReqTimeType		= ERTYFrontCount;
		info.m_TimeSpecify		= TimeSpecify;
		info.m_iFrontCount		= iNeedKLineCount;

		//
		//m_pAbsCenterManager->RequestViewData((CMmiCommBase*)&info);
		DoRequestViewData(info);
	}

	// �������Ʒ�ĳ�Ȩ���ݣ���������
	RequestWeightData();	// ��Ȩ��Ϣ�������ԣ�ȫ������- -Ӧ�����Ӳ���ʲô����
	// ����б�Ҫ���򵥶�
}

void CIoViewKLine::OnShowDataTimeRangeChanged( T_MerchNodeUserData *pData )
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

void CIoViewKLine::OnVDataLandMineUpdate( IN CMerch* pMerch )
{
	// ��Ϣ���������Ѿ�������Ʒ��ص�������
	if ( pMerch == GetMerchXml() && pMerch != NULL )
	{
		bool32 bSame = pMerch->m_mapLandMine.size() == m_mapLandMine.size();
		mapLandMine::iterator it2 = pMerch->m_mapLandMine.begin();
		for ( mapLandMine::iterator it1 = m_mapLandMine.begin(); it1 != m_mapLandMine.end() && bSame ; ++it1 )
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
			it2++;		// ��С��ͬ������+�ıض���ͬ
		}
		if ( !bSame )
		{
			// ����
			m_mapLandMine = pMerch->m_mapLandMine;
			if ( NULL != m_pRegionMain )
			{
				// ��ǰ�Ƿ�Ϊ��K��
				if (
					m_MerchParamArray.GetSize() < 1 
					|| m_MerchParamArray[0] == NULL
					|| m_MerchParamArray[0]->m_pMerchNode == NULL )
				{
					return;
				}

				T_MerchNodeUserData *pData = m_MerchParamArray[0];

				// ����
				if ( pData->m_eTimeIntervalFull != ENTIDay )
				{
					return;
				}

				m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
				Invalidate();	// ������ʾ
			}
		}
	}
}



void CIoViewKLine::OnVDataStrategyData(const char *pszRecvData)
{
	if (!pszRecvData)
	{
		return;
	}

	UnPackJson(pszRecvData);
	m_mulmapStategyInfo;
	if (m_mulmapStategyInfo.size() > 0)
	{
		m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
		Invalidate();	// ������ʾ
	}
}

void CIoViewKLine::DrawLandMines( CMemDCEx *pDC )
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

	// ����
	if ( m_MerchParamArray[0]->m_eTimeIntervalFull != ENTIDay )
	{
		return;
	}

	if ( m_mapLandMine.size() <= 0 )
	{
		return;
	}

	if ( NULL == m_pRegionMain || m_pRegionMain->m_aXAxisNodes.GetSize() <= 0)
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

	if ( NULL == pNode || pNode->GetSize() <= 0)
	{
		return;
	}

	if ( m_pRegionMain->m_aXAxisNodes.GetSize() < pNode->GetNodes().GetSize() )
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

	// x���궨λ����:
	// 1. �����������̺󷢳�����Ϣ���������ڵڶ��콻�׵���Ϣ��Ӧ����Ϊ�ڶ���ķ�������?
	// 2. ���ձ���ʱ���Ӧĳ��ʱ�䷶Χ�����ڸ÷�Χ�ڵģ�Ϊ���գ������ڵ�����ǰ�����
	// 3. ����gmtʱ���Ӧ����ʱ�䷶Χ�����ڸ÷�Χ�ģ�Ϊ���գ������ڵ�����ǰ�����

	// ����2�ŷ�������

	const int32 iPosStart = 0;
	const int32 iPosCount = pNode->GetNodes().GetSize();
	CAxisNode	nodeStart = m_pRegionMain->m_aXAxisNodes[iPosStart];
	CAxisNode	nodeEnd	  = m_pRegionMain->m_aXAxisNodes[ iPosCount-1 ];

	// ʱ���ʽ 2010-01-01 00:00:00

	mapLandMine::reverse_iterator it = m_mapLandMine.rbegin();
	if ( it != m_mapLandMine.rend() )
	{
		// ����ռ�ʱ�����ʾ�����Ҳ��+1�ĵ��ʱ�仹������ʾ���ռǣ�������Ҳ�+1�㣬�򲻿������ռǵĹ���
		CGmtTime timeEnd(nodeEnd.m_iTimeId);
		timeEnd += CGmtTimeSpan(0, 23, 59, 59);
		int32 iPosEndPlus = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pData->m_aKLinesFull, timeEnd);
		if ( iPosEndPlus >= 0 && iPosEndPlus < pData->m_aKLinesFull.GetSize() )
		{
			// ӵ���Ҳ�+1ֵ, ���˵�����>=�Ҳ�ʱ����ֵ
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

	if ( it != m_mapLandMine.rend() )
	{
		for ( int32 i = iPosCount+iPosStart-1; i >= iPosStart ; i-- )
		{
			bool32 bNeedDraw = false;
			int32 iTimeNow = m_pRegionMain->m_aXAxisNodes[i].m_iTimeId;
			CGmtTime TimeNow(iTimeNow);
			CGmtTime TimeDayLocalStart, TimeDayLocalEnd;
			CString StrTime;
			GetLocalDayGmtRange(TimeNow, TimeDayLocalStart, TimeDayLocalEnd);
			StrTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d")
				, TimeDayLocalStart.GetYear(), TimeDayLocalStart.GetMonth(), TimeDayLocalStart.GetDay()
				, TimeDayLocalStart.GetHour(), TimeDayLocalStart.GetMinute(), TimeDayLocalStart.GetSecond());

			CString StrMineTime;
			CRect rcNode(rcRegion);
			int32 iMines = 0;
			CString StrFirst;
			while ( it != m_mapLandMine.rend() && (StrFirst=it->first) >= StrTime )
			{
				// Ͷ���ռǵ�ʱ�����ڸ�ʱ���, �����ڸ�ʱ������
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
	}

	pDC->RestoreDC(iSaveDC);
}

E_MainKlineDrawStyle CIoViewKLine::GetMainKlineDrawStyle()
{
	return m_eKlineDrawStyle;
}

void CIoViewKLine::SetMainKlineDrawStyle( E_MainKlineDrawStyle eStyle )
{
	if ( eStyle >= EMKDSCount )
	{
		eStyle = EMKDSNormal;
	}
	if ( eStyle != m_eKlineDrawStyle )
	{
		// ����һ���������
		CString StrValue;
		StrValue.Format(_T("%d"), eStyle);
		CString StrName = CIoViewManager::GetIoViewString(this);
		CEtcXmlConfig::Instance().WriteEtcConfig(StrName, KXMLAttriIoViewKlineKlineDrawStyle, StrValue);
	}
	NotifyIoViewKlineDrawStyleChange(eStyle);
}

void CIoViewKLine::NotifyIoViewKlineDrawStyleChange( E_MainKlineDrawStyle eNewStyle )
{
	sm_eMainKlineDrawStyle = eNewStyle;

	if ( NULL == m_pAbsCenterManager )
	{
		return;
	}

	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	if ( NULL == pMainFrame )
	{
		return;
	}

	for ( int32 i=0; i < pMainFrame->m_IoViewsPtr.GetSize() ; ++i )
	{
		CIoViewBase *pIoView = pMainFrame->m_IoViewsPtr[i];
		if ( NULL != pIoView
			&& pIoView->IsKindOf(RUNTIME_CLASS(CIoViewKLine)) )
		{
			CIoViewKLine *pKline = (CIoViewKLine *)pIoView;
			pKline->DoIoViewKlineDrawStyleChange(eNewStyle);
		}
	}
}

void CIoViewKLine::DoIoViewKlineDrawStyleChange( E_MainKlineDrawStyle eNewStyle )
{
	if ( m_eKlineDrawStyle != eNewStyle )
	{
		m_eKlineDrawStyle = eNewStyle;
		if ( NULL != m_pRegionMain )
		{
			// ������K��&����������
			CChartDrawer::E_ChartDrawStyle eCDStyle = (CChartDrawer::E_ChartDrawStyle)GetCurChartKlineDrawType();
			for ( int32 i=0; i < m_MerchParamArray.GetSize() ; ++i )
			{
				T_MerchNodeUserData *pData = m_MerchParamArray[i];
				if ( NULL != pData && NULL != pData->m_pKLinesShow )
				{
					CArray<CDrawingCurve *, CDrawingCurve *> &aCurves = pData->m_pKLinesShow->GetAttachedCurves();
					for ( int32 j=0; j < aCurves.GetSize() ; ++j )
					{
						if ( NULL != aCurves[j] 
						&& !aCurves[j]->m_bSelfDraw
							&& CheckFlag(aCurves[j]->m_iFlag, CChartCurve::KTypeKLine))
						{
							CChartCurve *pCurve = (CChartCurve *)aCurves[j];
							CChartDrawer *pDrawer = (CChartDrawer *)pCurve->GetDrawer();
							if ( NULL != pDrawer )
							{
								pDrawer->m_eChartDrawType = eCDStyle;
							}
						}
					}
				}
			}

			m_pRegionMain->SetDrawFlag(CRegion::KDrawFull);
			Invalidate();
		}
	}
}

CChartCurve	* CIoViewKLine::GetCurIndexCurve( CChartRegion *pRegionParent )
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

void CIoViewKLine::AdjustCtrlFloatContent()
{
	if ( NULL != m_pCell1 && NULL != m_pMerchXml )
	{
		E_ReportType eType = m_pMerchXml->m_Market.m_MarketInfo.m_eMarketReportType;
		if (CReportScheme::IsFuture(eType))
		{			
			m_pCell1->SetText(_T(" �ֲ���"));
		}
		else
		{
			m_pCell1->SetText(_T(" ���"));
		}

		// ��grid����cell
		if ( eType == ERTStockCn )
		{
			if ( m_GridCtrlFloat.GetRowCount() != KiGridFloatCtrlRowCountForStockCn )
			{
				ASSERT( m_GridCtrlFloat.GetRowCount() == KiGridFloatCtrlRowCountForOther );
				m_GridCtrlFloat.SetRowCount(KiGridFloatCtrlRowCountForStockCn);

				// ��Ʊ�������� ��ͨ��
				CGridCellSys *pCell;
				CGridCtrlSys *pGridCtrl = &m_GridCtrlFloat;
				pCell = (CGridCellSys *)pGridCtrl->GetCell(20, 0);
				pCell->SetDefaultFont(ESFSmall);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->SetText(_T("������"));

				// 				pCell = (CGridCellSys *)pGridCtrl->GetCell(24, 0);
				// 				pCell->SetDefaultFont(ESFSmall);
				// 				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				// 				pCell->SetText(_T("��ͨ��"));

				pCell = (CGridCellSys *)pGridCtrl->GetCell(21,0);
				pCell->SetDefaultFont(ESFSmall);
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

				pCell = (CGridCellSys *)pGridCtrl->GetCell(23,0);
				pCell->SetDefaultFont(ESFSmall);
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

				AdjustGridFloatSize();
			}
		}
		else
		{
			if ( m_GridCtrlFloat.GetRowCount() != KiGridFloatCtrlRowCountForOther )
			{
				ASSERT( m_GridCtrlFloat.GetRowCount() == KiGridFloatCtrlRowCountForStockCn );
				m_GridCtrlFloat.SetRowCount(KiGridFloatCtrlRowCountForOther);
				AdjustGridFloatSize();
			}
		}
	} // �������
}

CString CIoViewKLine::GetSaneIndexShowName()
{
	return CPluginFuncRight::GetSaneIndexShowName();
}

void CIoViewKLine::ShowJinDunIndex()
{
	m_bShowSaneIndexNow = true;
	m_bShowTrendIndexNow= false;

	ReDrawAysnc();

	m_FormulaNames.RemoveAll(); 
}

bool32 CIoViewKLine::GetAdvDlgPosition(IN int32 iWidth, IN int32 iHeight, OUT CRect& rtPos)
{
	if (iWidth <= 0 || iHeight <= 0)
	{
		return false;
	}

	//
	rtPos.SetRectEmpty();

	//
	float fPriceNew = -1;
	if ( NULL == m_pMerchXml  || NULL == m_pRegionMain || NULL == m_pRegionMain->GetDependentCurve())
	{
		return false;
	}

	// ���ߵ�Y ����
	CRect RectMain = m_pRegionMain->GetRectCurves();	
	int32 iYCenter = RectMain.CenterPoint().y;

	// ���¼۶�Ӧ��Y ����
	bool bNoPriceNew = false;
	if ( m_pMerchXml->m_pRealtimePrice )
	{
		fPriceNew = m_pMerchXml->m_pRealtimePrice->m_fPriceNew;
	}
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
	if ( iYCenter <= iYPrice || bNoPriceNew )
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
bool32 CIoViewKLine::IsAlreadyToShowAdv()
{
	if ( NULL == m_pMerchXml || NULL == m_pMerchXml->m_pRealtimePrice )// || m_pMerchXml->m_pRealtimePrice->m_fPriceNew <= 0.0)
	{
		//	if ( 5 == m_iGetNewPriceCnt ) //5��û�л�ȡ�����¼�
		//		return true;
		return false;
	}
	//
	return true;
}

bool32	CIoViewKLine::GetViewRegionRect(OUT LPRECT pRect)
{
	CWnd *pWnd = GetView();
	if ( NULL != pWnd && NULL != pRect )
	{
		pWnd->GetClientRect(pRect);
		if (m_bShowTopToolBar)
		{
			pRect->top += (m_iTopMerchHeight + m_iTopButtonHeight);
		}
		if (m_bShowIndexToolBar)
		{
			pRect->bottom -= 25;
		}
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
int32 MainKlineDrawStyleToChartDrawType( E_MainKlineDrawStyle eStyle )
{
	int32 iChartDrawType = CChartDrawer::EKDSNormal;
	switch (eStyle)
	{
	case EMKDSNormal:
		break;
	case EMKDSFill:
		iChartDrawType = CChartDrawer::EKDSFillRect;
		break;
	case EMKDSAmerica:
		iChartDrawType = CChartDrawer::EKDSAmerican;
		break;
	case EMKDSClose:
		iChartDrawType = CChartDrawer::EKDSClose;
		break;
	case EMKDSTower:
		iChartDrawType = CChartDrawer::EKDSTower;
		break;
	default:
		ASSERT( 0 );
		break;
	}
	return iChartDrawType;
}

void CIoViewKLine::DragMoveKLine(int32 icnt)
{
	if (NULL == m_pRegionMain->GetDependentCurve())
	{
		return;
	}

	if ( m_pRegionMain->IsActiveCross() )
	{
		OnMouseLeave(0,0); // ȥ��ʮ����
	}

	int32 x = 0, i = 0;
	int32 iMove;
	if (0 < icnt)
	{
		iMove= (int32)(icnt + 0.5);
		if ( 0 <  iMove)
		{
			OnKeyLeftAndCtrl(iMove);

			CPoint pt;
			GetCursorPos(&pt);		
			ScreenToClient(&pt);
			m_pRegionMain->DrawIndexValueByMouseMove(pt.x, pt.y, true);
			for( i = 0 ; i < m_SubRegions.GetSize(); i++ )
			{
				m_SubRegions.GetAt(i).m_pSubRegionMain->DrawIndexValueByMouseMove(pt.x, pt.y, true);
			}
		}
	}
	else
	{
		iMove = (int32)(-icnt + 0.5);
		if ( 0 <  iMove)
		{
			OnKeyRightAndCtrl(iMove);

			CPoint pt;
			GetCursorPos(&pt);		
			ScreenToClient(&pt);
			m_pRegionMain->DrawIndexValueByMouseMove(pt.x, pt.y, true);
			for( i = 0 ; i < m_SubRegions.GetSize(); i++ )
			{
				m_SubRegions.GetAt(i).m_pSubRegionMain->DrawIndexValueByMouseMove(pt.x, pt.y, true);
			}
		}
	}
}

void CIoViewKLine::UpdateKeline(E_NodeTimeInterval NodeInterval)
{
	int32 i, iSize = m_MerchParamArray.GetSize();
	for ( i = 0; i < iSize; i ++ )
	{
		T_MerchNodeUserData* pData = m_MerchParamArray.GetAt(i);
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



void CIoViewKLine::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default


	// ������Ϣ�ĵ���¼�
	if ( m_mulmapStategyRects.size() > 0 )
	{
		CStringArray aStrMineTimes;
		for ( StategyRectMulmap::iterator it = m_mulmapStategyRects.begin(); it != m_mulmapStategyRects.end() ; ++it )
		{
			if ( it->second.PtInRect(point) )
			{
				//  ��һ���ڵ��ж����Ϣ���õ���¼�����
				if (m_mulmapStategyInfo.count(it->first) > 1)
				{
					CDlgStrategyInfomation cStategyInfo(it->first, &m_mulmapStategyInfo);
					cStategyInfo.DoModal();
					//cStategyInfo.SetStrategyInfo(it->first, m_mulmapStategyInfo);
					return;
				}
			}
		}
	}
	__super::OnLButtonDown(nFlags, point);
}
