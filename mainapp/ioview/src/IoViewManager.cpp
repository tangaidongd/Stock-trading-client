#include "stdafx.h"

#include "memdc.h"
#include "ShareFun.h"
#include "GGTongView.h"
#include "IoViewBase.h"
#include "IoViewChart.h"
#include "IoViewKLine.h"
#include "IoViewTrend.h"
#include "IoViewTick.h"
#include "IoViewPrice.h"
#include "IoViewLevel2.h"
#include "IoViewF10.h"
#include "IoViewNews.h" 
#include "IoViewTextNews.h"
#include "IoViewReport.h"
#include "IoViewTimeSale.h"
#include "IoViewManager.h"
#include "IoViewRelative.h"
#include "IoViewDetail.h"
#include "IoViewStarry.h"
#include "IoViewCapitalFlow.h"
#include "IoViewBlockReport.h"
#include "IoViewMainTimeSale.h"
#include "IoViewReportRank.h"
#include "IoViewReportSelect.h"
#include "IoViewDaDanPaiHang.h"
#include "IoViewValue.h"
#include "IoViewExpTrend.h"
#include "IoViewFenJiaBiao.h"
#include "BiSplitterWnd.h"
#include "IoViewChouMa.h"
#include "IoViewPhaseSort.h"
#include "IoViewDuoGuTongLie.h"
#include "IoViewReportArbitrage.h"
#include "IoViewKLineArbitrage.h"
#include "IoViewTrendArbitrage.h"
#include "IoViewQRCode.h"
#include "IoViewPositions.h"
#include "IoViewTrendAnalysis.h"
#include "IoViewDKMoney.h"
#include "IoViewMainCost.h"
#include "IoViewSuggestPosition.h"
#include "IoViewMultiRank.h"
#include "IoViewMultiCycleKline.h"
#include "IoViewWebCapitalFlow.h"
#include "IoViewAddUserBlock.h"
#include "IoViewDuanXianJianKong.h"
#include "IoViewSmartChooseStock.h"
#include "IoViewNewStockReport.h"

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
							//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CIoViewManager* CIoViewManager::m_pThis = NULL;

// tab����ͼ��ָ����Ŀ��, ֻ������top �� bottom
#define TAB_SPLITER_WIDTH	1


#define TOPBAR_BEGINID	33000
#define TOPBAR_ENDID		33100
#define INVALID_ID			-1


// �������ص�xml�����������Ϣ
static const char *KStrElementValue				= "IoViewManager";
static const char *KStrElementAttrViewTab		= "ViewTab";
static const char *KStrElementAttrCurView		= "CurView";
static const char *KStrElementAttrAutoHide		= "AutoHide";
static const char *KStrElementAttrGroupId		= "GroupId";
static const char *KStrElementAttrShowManagerTopBar		= "ShowManagerTopBar";	// �Ƿ���ʾ����������
static const char *KStrElementAttrbIsHasTopBarByLayout	= "bIsHasTopBarByLayout";	// ����Ƿ��ж�����ǩ��
static const char *KStrElementAttrbIsHasBottomBarByLayout	= "bIsHasBottomBarByLayout";	// ����Ƿ��еײ���ǩ��

static const char *KStrElementAttrManagerTopBtn		= "ManagerTopBtn";				// �����ඥ����ǩ
static const char *KStrElementAttrManagerBarHight	= "ManagerBarHight";			// ������ǩ�߶�
static const char *KStrElementAttrManagerBarBkColor	= "ManagerBarBkColor";			// ������ǩ������ɫ
static const char *KStrElementAttrtabbarbtnWidth	= "tabbarbtnWidth";				// ��ǩ���İ�ť�Ŀ��
static const char *KStrElementAttrManagerBarType	= "ManagerBarType";				// ��ǩ������
static const char *KStrElementAttrRightimagePath	= "RightimagePath";				// �Ҳఴť����ͼƬ
static const char *KStrElementAttrtabbar			= "tabbar";						// ��ǩ��
static const char *KStrElementAttrbtnType			= "btnType";					// ��ǩ�������
static const char *KStrElementAttrname				= "name";						// ��ǩ����ʾ����
static const char *KStrElementAttrmarketid			= "marketid";					// �г�ID
static const char *KStrElementAttrMerchCode			= "MerchCode";					// ��Ʒ����
static const char *KStrElementAttrcfmname			= "cfmname";					// ��������
static const char *KStrElementAttrShrinkSize		= "ShrinkSize";					// ��Ч����������������
static const char *KStrElementAttrisSelected		= "isSelected";					// �Ƿ�Ϊѡ��״̬
static const char *KStrElementAttrAlignmente		= "Alignment";					// ���뷽ʽ�������ʾ���ұ���ʾ���м���ʾ
static const char *KStrElementAttrframeColor		= "frameColor";					// �ؼ��߿���ɫ


static const char *KStrElementAttrTabBkClr		= "TabBkClr";			// tab��������ɫ
static const char *KStrElementAttrTabBkNorClr	= "TabBkNorClr";		// tab����������ɫ
static const char *KStrElementAttrTabBkSelClr	= "TabBkSelClr";		// tabѡ�б�����ɫ
static const char *KStrElementAttrTabTextNorClr	= "TabTextNorClr";		// tab����������ɫ
static const char *KStrElementAttrTabTextSelClr	= "TabTextSelClr";		// tabѡ��������ɫ
static const char *KStrElementAttrTabSpliterClr	= "TabSpliterClr";		// tab����ͼ��ָ�����ɫ
static const char *KStrElementAttrTabFixWidth   = "TabFixWidth";        // tab��ǩÿһ���Ƿ�̶����

// ����ҵ����ͼ�б�, �½���ҵ����ͼһ��Ҫ������ǩ���� �Żᱻϵͳ��ʶ
// �� resource.h ��ID �����˳��Ҫһ��
static const T_IoViewObject s_astIoViewObjects[] = 
{
	T_IoViewObject(ID_PIC_TREND			,			EIBSNone,		EIMCSFalse,		L"��ʱ����",		L"��",		L"",					L"IoViewTrend",				E_IVGTChart,		RUNTIME_CLASS(CIoViewTrend), false),
	T_IoViewObject(ID_PIC_KLINE			,			EIBSNone,		EIMCSFalse,		L"���߷���",		L"K",		L"",		L"IoViewKLine",				E_IVGTChart,		RUNTIME_CLASS(CIoViewKLine), false),
	T_IoViewObject(ID_PIC_TICK			,			EIBSNone,		EIMCSFalse,		L"����ͼ",			L"��",		L"",					L"IoViewTick",				E_IVGTChart,		RUNTIME_CLASS(CIoViewTick), false),
	T_IoViewObject(ID_PIC_PRICE			,			EIBSNone,		EIMCSFalse,		L"������",			L"��",		L"",	    				L"IoViewPrice",				E_IVGTPrice,		RUNTIME_CLASS(CIoViewPrice), false),
	T_IoViewObject(ID_PIC_TIMESALE		,			EIBSNone,		EIMCSFalse,		L"�ֱʳɽ�",		L"��",		L"",					L"IoViewTimeSale",			E_IVGTOther,		RUNTIME_CLASS(CIoViewTimeSale), false),
	T_IoViewObject(ID_PIC_NEWS			,			EIBSNone,		EIMCSFalse,		L"������Ѷ",		L"Ѷ",		L"",					L"IoViewNews",				E_IVGTOther,		RUNTIME_CLASS(CIoViewNews), false),
//	T_IoViewObject(ID_PIC_NEWS			,			EIBSNone,		EIMCSTrue,		L"������Ѷ",		L"Ѷ",		L"",					L"IoViewTextNews",			E_IVGTOther,		RUNTIME_CLASS(CIoViewTextNews), false),
    T_IoViewObject(ID_PIC_DETAIL		,			EIBSNone,		EIMCSFalse,		L"�Ƹ�׷��",		L"��",		L"�Ƹ�׷��ָ��",				L"IoViewDetail",			E_IVGTOther,		RUNTIME_CLASS(CIoViewDetail), false),	
	T_IoViewObject(ID_PIC_REPORT		,			EIBSNone,		EIMCSTrue,		L"�����б�",		L"��",		L"",		L"IoViewReport",			E_IVGTReport,		RUNTIME_CLASS(CIoViewReport), true),
    T_IoViewObject(ID_PIC_STARRY		,			EIBSNone,		EIMCSFalse,		L"�ǿ�ͼ",		    L"��",		L"�ȵ��ǿ�ͼ",		            L"IoViewStarry",			E_IVGTOther,		RUNTIME_CLASS(CIoViewStarry), false, EMMT_StockCn),	
    T_IoViewObject(ID_PIC_CAPITALFLOW	,			EIBSHorizontal,	EIMCSFalse,		L"�ʽ�����",		L"��",		L"�����ʽ�����ͼ",		        L"IoViewCapitalFlow",		E_IVGTOther,		RUNTIME_CLASS(CIoViewCapitalFlow), false, EMMT_StockCn),	
    T_IoViewObject(ID_PIC_BLOCKREPORT	,			EIBSNone,		EIMCSFalse,		L"����б�",		L"��",		L"����б�",			        L"IoViewBlockReport",		E_IVGTOther,		RUNTIME_CLASS(CIoViewBlockReport), false, EMMT_StockCn),	
    T_IoViewObject(ID_PIC_MAIN_TIMESALE	,			EIBSHorizontal,	EIMCSFalse,		L"�������",		L"��",		L"�������",			        L"IoViewMainTimeSale",		E_IVGTOther,		RUNTIME_CLASS(CIoViewMainTimeSale), false, EMMT_StockCn),	
	T_IoViewObject(ID_PIC_REPORT_RANK	,			EIBSNone,		EIMCSFalse,		L"��������",		L"��",		L"��������",			        L"IoViewReportRank",		E_IVGTOther,		RUNTIME_CLASS(CIoViewReportRank), false, EMMT_StockCn),	
    //T_IoViewObject(ID_PIC_REPORT_SELECT	,			EIBSNone,		EIMCSTrue,		L"����ѡ��",		L"ѡ",		L"����ѡ�ɱ�������",	        L"IoViewReportSelect",		E_IVGTOther,		RUNTIME_CLASS(CIoViewReportSelect), false, CViewData::EMMT_StockCn),	
    T_IoViewObject(ID_PIC_DADANPAIHANG	,			EIBSNone,		EIMCSTrue,		L"�����",		L"��",		L"�����",					L"IoViewDaDanPaiHang",		E_IVGTOther,		RUNTIME_CLASS(CIoViewDaDanPaiHang), false, EMMT_StockCn),	
    T_IoViewObject(ID_PIC_VALUE			,			EIBSHorizontal,		EIMCSFalse,		L"�����ֵ",		L"ֵ",		L"�����ֵ",					L"IoViewValue",				E_IVGTOther,		RUNTIME_CLASS(CIoViewValue), false, EMMT_StockCn),	
    T_IoViewObject(ID_PIC_EXPTREND		,			EIBSNone,		EIMCSFalse,		L"ָ������",		L"ָ",		L"��Ʒ��Ӧָ�����Ʒ���",		L"IoViewExpTrend",			E_IVGTOther,		RUNTIME_CLASS(CIoViewExpTrend), false, EMMT_StockCn),	
	T_IoViewObject(ID_PIC_FENJIABIAO	,			EIBSNone,		EIMCSFalse,		L"�ּ۱�",			L"��",		L"�ּ۱�",				        L"IoViewFenJiaBiao",		E_IVGTOther,		RUNTIME_CLASS(CIoViewFenJiaBiao), false),	
    T_IoViewObject(ID_PIC_CHOUMA		,			EIBSHorizontal, EIMCSFalse,		L"����ֲ�",		L"��",		L"����ֲ�",			        L"IoViewChouMa",			E_IVGTOther,		RUNTIME_CLASS(CIoViewChouMa), false),	
	T_IoViewObject(ID_PIC_PHASESORT		,			EIBSNone,		EIMCSFalse,		L"�׶�����",		L"��",		L"",			        L"IoViewPhaseSort",			E_IVGTOther,		RUNTIME_CLASS(CIoViewPhaseSort), false),	
	T_IoViewObject(ID_PIC_DUOGUTONGLIE	,			EIBSNone,		EIMCSFalse,		L"���Լͬ��",		L"��",		L"",			        L"IoViewDuoGuTongLie",		E_IVGTOther,		RUNTIME_CLASS(CIoViewDuoGuTongLie), false),	
	T_IoViewObject(ID_PIC_KLINEARBITRAGE	,		EIBSNone,		EIMCSFalse,		L"����K��ͼ",		L"��",		L"",			        L"IoViewKLineArbitrage",	E_IVGTOther,		RUNTIME_CLASS(CIoViewKLineArbitrage), false),	
	T_IoViewObject(ID_PIC_TRENDARBITRAGE	,		EIBSNone,		EIMCSFalse,		L"������ʱͼ",		L"��",		L"",			    L"IoViewTrendArbitrage",	E_IVGTOther,		RUNTIME_CLASS(CIoViewTrendArbitrage), false),	
	//T_IoViewObject(ID_PIC_LEVEL2	,	L"����",			L"��",		L"����������",			 		L"IoViewLevel2",		E_IVGTOther,		RUNTIME_CLASS(CIoViewLevel2)),
	//T_IoViewObject(ID_PIC_F10		,	L"����",			L"��",		L"F10��Ѷ",					 	L"IoViewF10",			E_IVGTOther,		RUNTIME_CLASS(CIoViewF10)),
	//T_IoViewObject(ID_PIC_RELATIVE,	L"���",			L"��",		L"�����Ʒ",	    			L"IoViewRelative",		E_IVGTRelative,		RUNTIME_CLASS(CIoViewRelative)),
	T_IoViewObject(ID_PIC_REPORT_ARBITRAGE,			EIBSNone,		EIMCSTrue,		L"�������۱�",		L"��",		L"",			        L"IoViewReportArbitrage",	E_IVGTOther,		RUNTIME_CLASS(CIoViewReportArbitrage), false),		
	T_IoViewObject(ID_PIC_WEB_NEWS,					EIBSNone,		EIMCSTrue,		L"��ҳ",			L"��",		L"",					L"IoViewNews",			E_IVGTOther,		RUNTIME_CLASS(CIoViewNews), false),
	T_IoViewObject(ID_PIC_QR_CODE,					EIBSNone,		EIMCSTrue,		L"��ά��",			L"��",		L"",						L"IoViewQRCode",			E_IVGTReport,		RUNTIME_CLASS(CIoViewQRCode), false),	
	T_IoViewObject(ID_PIC_MULTI_RANK,				EIBSNone,		EIMCSFalse,		L"������ͬ��",	    L"��",		L"������ͬ��",				    L"IoViewMultiRank",		    E_IVGTOther,		RUNTIME_CLASS(CIoViewMultiRank), false),	
	T_IoViewObject(ID_PIC_DUANXIANJIANKONG,			EIBSNone,		EIMCSFalse,		L"���߼��",	    L"��",		L"���߼��",				    L"IoViewDuanXianJianKong",  E_IVGTOther,		RUNTIME_CLASS(CIoViewDuanXianJianKong), false),	
	T_IoViewObject(ID_PIC_MULTI_CYCLE_KLINE,	    EIBSNone,		EIMCSFalse,		L"��K��ͬ��",	    L"��",		L"��K��ͬ��",				    L"IoViewMultiCycleKline",   E_IVGTOther,		RUNTIME_CLASS(CIoViewMultiCycleKline), false),	
	T_IoViewObject(ID_PIC_POSITIONS,				EIBSNone,		EIMCSFalse,		L"�ֲֲ�λ",		L"��",		L"�ֲֲ�λ",					L"IoViewPositions",			E_IVGTOther,		RUNTIME_CLASS(CIoViewPositions), false, EMMT_StockCn),
	T_IoViewObject(ID_PIC_TREND_ANALYSIS,			EIBSNone,		EIMCSFalse,		L"��������",		L"��",		L"��������",					L"IoViewTrendAnalysis",		E_IVGTOther,		RUNTIME_CLASS(CIoViewTrendAnalysis), false, EMMT_StockCn),
	T_IoViewObject(ID_PIC_DK_MONEY,					EIBSNone,		EIMCSFalse,		L"����ʽ�",		L"��",		L"����ʽ�",					L"IoViewDKMoney",			E_IVGTOther,		RUNTIME_CLASS(CIoViewDKMoney), false, EMMT_StockCn),
	T_IoViewObject(ID_PIC_MAIN_COST,				EIBSNone,		EIMCSFalse,		L"�����ɱ�",		L"��",		L"�����ɱ�",					L"IoViewMainCost",			E_IVGTOther,		RUNTIME_CLASS(CIoViewMainCost), false, EMMT_StockCn),
	T_IoViewObject(ID_PIC_SUGGEST_POSITION,			EIBSNone,		EIMCSFalse,		L"�����λ",		L"��",		L"�����λ",					L"IoViewSuggestPosition",	E_IVGTOther,		RUNTIME_CLASS(CIoViewSuggestPosition), false, EMMT_StockCn),
	T_IoViewObject(ID_PIC_WEB_CAPITAL_FLOW,			EIBSHorizontal,	EIMCSFalse,		L"�ʽ�����",		L"��",		L"�ʽ�����",					L"IoViewWebCapitalFlow",	E_IVGTOther,		RUNTIME_CLASS(CIoViewWebCapitalFlow), false, EMMT_StockCn),
	T_IoViewObject(ID_PIC_ADD_USER_BLOCK,			EIBSNone,		EIMCSFalse,		L"��ѡ���",		L"ѡ",		L"��ѡ���",					L"IoViewAddUserBlock",	    E_IVGTOther,		RUNTIME_CLASS(CIoViewAddUserBlock), false, EMMT_StockCn),
	T_IoViewObject(ID_PIC_TIMESALE_RANK,			EIBSNone,		EIMCSFalse,		L"������",		L"��",		L"������",					L"IoViewTimeSaleRank",	    E_IVGTOther,		RUNTIME_CLASS(CIoViewTimeSaleRank), false, EMMT_StockCn),
	T_IoViewObject(ID_PIC_TIMESALE_STATISTIC,		EIBSNone,		EIMCSFalse,		L"��ͳ��",		L"ͳ",		L"��ͳ��",					L"IoViewTimeSaleStatistic",	E_IVGTOther,		RUNTIME_CLASS(CIoViewTimeSaleStatistic), false, EMMT_StockCn),
	T_IoViewObject(ID_PIC_SMART_CHOOSE_STOCK,		EIBSNone,		EIMCSFalse,		L"����ѡ��",		L"ѡ",		L"����ѡ��",					L"IoViewSmartChooseStock",	E_IVGTOther,		RUNTIME_CLASS(CIoViewSmartChooseStock), false),
    T_IoViewObject(ID_PIC_REPORT_NEWSTOCK,		    EIBSNone,		EIMCSFalse,		L"�¹ɱ��۱�",		L"��",		L"�¹ɱ��۱�",					L"IoViewNewStockReport",	E_IVGTOther,		RUNTIME_CLASS(CIoViewNewStockReport), false),
};

static const int32 KIoViewObjectCount = sizeof(s_astIoViewObjects) / sizeof(T_IoViewObject);
//
static const T_IoViewGroupObject s_astIoViewGroupObjects[] = 
{
	T_IoViewGroupObject(L"ͼ��",	L"ͼ",		E_IVGTChart),
	T_IoViewGroupObject(L"���۱�",	L"��",		E_IVGTReport),
	//T_IoViewGroupObject(L"���",	L"��",		E_IVGTRelative),
	T_IoViewGroupObject(L"������",	L"��",		E_IVGTPrice),
	T_IoViewGroupObject(L"����",	L"��",		E_IVGTOther),
};

static const int32 KIoViewGroupObjectCount = sizeof(s_astIoViewGroupObjects) / sizeof(T_IoViewGroupObject);
// Ĭ�ϵ�ҵ����ͼ
CRuntimeClass *CIoViewManager::m_pDefaultIoViewClass = RUNTIME_CLASS(CIoViewTrend);	// �޸�����


IMPLEMENT_DYNAMIC(CIoViewManager, CControlBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewManager, CStatic)
	//{{AFX_MSG_MAP(CIoViewManager)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_NOTIFY(TCN_SELCHANGE, 0x9999, OnSelChange)
	ON_MESSAGE(UM_IoViewTitle_Button_LButtonDown,OnIoViewTitleButtonLButtonDown)	
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// ctor

CIoViewManager::CIoViewManager()
{
	m_pParent              = NULL;
	m_rectClient           = CRect(-1,-1,-1,-1);

	//
	m_eViewTab  = EVTNone;
	m_iGroupId  = 1;			// Ĭ�ϴ�Ҷ���1����
	m_bAutoHide = false;

	m_pThis		= this;

	m_bFromXmlNeedInit = false;
	m_bBeyondShow = false;

	m_bDelIoView = false;

	m_bIsShowManagerTopbar = false;
	m_bIsHasTopBarByLayout = false;
	m_bIsHasBottomBarByLayout = false;
	m_rcManagerTopBtn.SetRect(0,0,0,0);
	m_rcGuiTabWnd.SetRect(0,0,0,0);

	m_clrTabBk		= RGB(44, 44, 52);
	m_clrTabBkNor	= RGB(44, 44, 52);
	m_clrTabBkSel	= RGB(42,46,52);
	m_clrTabTextNor	= RGB(182,183,185);
	m_clrTabTextSel	= RGB(230,70,70);
	m_clrTabSpliter	= RGB(30,30,30);

	m_bNeedFixWidth = false;
	m_iTabFixWidth  = 0;

	m_LastPressTopBtnId = -1;
}

///////////////////////////////////////////////////////////////////////////////
// dtor

CIoViewManager::~CIoViewManager()
{
	for (int32 i = 0; i < m_IoViewsPtr.GetSize(); i++)
		DEL(m_IoViewsPtr[i]);

	m_IoViewsPtr.RemoveAll();
}


void CIoViewManager::DrawManagerTopBar(CPaintDC &dc)
{
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bmp;

	CRect rtClient;
	GetClientRect(rtClient);
	CRect rcManagerTopBtn = rtClient;
	rcManagerTopBtn.bottom = m_rcManagerTopBtn.bottom;

	bmp.CreateCompatibleBitmap(&dc, rcManagerTopBtn.Width(), rcManagerTopBtn.Height());
	memDC.SelectObject(&bmp);
	memDC.FillSolidRect(m_rcManagerTopBtn.left, m_rcManagerTopBtn.top, m_rcManagerTopBtn.Width(), m_rcManagerTopBtn.Height(), m_cManagerTopBarInfo.m_colBarBKColor);
	memDC.SetBkMode(TRANSPARENT);
	Gdiplus::Graphics graphics(memDC.GetSafeHdc());

	CRect rcPaint;
	dc.GetClipBox(&rcPaint);
	CRect rcControl;

	map<int32, CNCButton>::iterator iter;
	for (iter = m_cManagerTopBarInfo.m_mapTopbarBtnLeft.begin(); iter != m_cManagerTopBarInfo.m_mapTopbarBtnLeft.end(); iter++)
	{
		CNCButton &ncButton = iter->second;
		ncButton.GetRect(rcControl);
		// �жϵ�ǰ��ť�Ƿ���Ҫ�ػ�
		if (!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;
		}
		ncButton.DrawButton(&graphics);
	}

	// �Ҳఴť
	for (iter = m_cManagerTopBarInfo.m_mapTopbarBtnRight.begin(); iter != m_cManagerTopBarInfo.m_mapTopbarBtnRight.end(); iter++)
	{
		CNCButton &ncButton = iter->second;
		ncButton.GetRect(rcControl);
		// �жϵ�ǰ��ť�Ƿ���Ҫ�ػ�
		if (!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;
		}
		ncButton.DrawButton(&graphics);
	}
	// �в���ť
	for (iter = m_cManagerTopBarInfo.m_mapTopbarBtnCenter.begin(); iter != m_cManagerTopBarInfo.m_mapTopbarBtnCenter.end(); iter++)
	{
		CNCButton &ncButton = iter->second;
		ncButton.GetRect(rcControl);
		// �жϵ�ǰ��ť�Ƿ���Ҫ�ػ�
		if (!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;
		}
		ncButton.DrawButton(&graphics);
	}

	CBrush cbr;
	CPen cpenbr;
	cpenbr.CreatePen(PS_SOLID, 1, RGB(1,1,5)); 
	CPen *pOldPen = memDC.SelectObject(&cpenbr);
	cbr.CreateStockObject(NULL_BRUSH); //����һ���ջ�ˢ
	CBrush *pOldBr = memDC.SelectObject(&cbr);
	memDC.RoundRect(m_rcManagerTopBtn, CPoint(0, 0));
	cbr.DeleteObject();
	memDC.SelectObject(pOldBr);
	memDC.SelectObject(pOldPen);

	dc.BitBlt(m_rcManagerTopBtn.left,m_rcManagerTopBtn.top, rcManagerTopBtn.Width(), rcManagerTopBtn.Height(), &memDC, m_rcManagerTopBtn.left, m_rcManagerTopBtn.top, SRCCOPY);
	dc.SelectClipRgn(NULL);
	memDC.DeleteDC();
	bmp.DeleteObject();
}
///////////////////////////////////////////////////////////////////////////////
// OnPaint

void CIoViewManager::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rtClient;
	GetClientRect(rtClient);
	dc.FillSolidRect(rtClient, RGB(42,42,50));

	CRect rcDrawLine(rtClient);
	rcDrawLine.DeflateRect(1,1,1,1);


	// tab����ͼ��ָ�������, ֻ������top �� bottom
	if (EVTTop == m_eViewTab)
	{
		CRect rcDraw;
		m_GuiTabWnd.GetClientRect(rcDraw);
		rcDraw.top = rcDraw.bottom;
		rcDraw.bottom += TAB_SPLITER_WIDTH;	

		if(m_bNeedFixWidth)
		{
			dc.FillSolidRect(rtClient, RGB(42,42,50));
		}
		else
		{
			dc.FillSolidRect(rcDraw, m_clrTabSpliter);
		}
	}
	else if (EVTBottom == m_eViewTab)
	{
		CRect rcDraw;
		m_GuiTabWnd.GetClientRect(rcDraw);
		rcDraw.bottom = rtClient.bottom-rcDraw.Height();
		rcDraw.top = rcDraw.bottom - TAB_SPLITER_WIDTH;	
		if(m_bNeedFixWidth)
		{
			dc.FillSolidRect(rtClient, RGB(42,42,50));
		}
		else
		{
			dc.FillSolidRect(rcDraw, m_clrTabSpliter);
		}
	}

	if ( NULL != GetActiveIoView() )
	{
		if (GetActiveIoView()->IsKindOf(RUNTIME_CLASS(CIoViewChart)))
		{
			//DrawNoChartRgn();
		}		
	}

	/*if (m_bIsHasBottomBarByLayout)
	{
		rcDrawLine.bottom -= 1;
	}*/


	if (EVTBottom == m_eViewTab)
	{
		CRect rcDraw;
		rcDraw = rcDrawLine;
		rcDraw.bottom = m_rcGuiTabWnd.bottom;
		rcDraw.top = m_rcGuiTabWnd.top;
		dc.MoveTo(rcDraw.left, rcDraw.top);  
		dc.LineTo(rcDraw.right, rcDraw.top);  
	}

	dc.MoveTo(rcDrawLine.left, rcDrawLine.top);  
	/*if (m_bIsHasTopBarByLayout)
	{
		dc.MoveTo(rcDrawLine.right, rcDrawLine.top);  
	}
	else*/
	{
		dc.MoveTo(rcDrawLine.left, rcDrawLine.top);  
		dc.LineTo(rcDrawLine.right, rcDrawLine.top);  
	}
	dc.LineTo(rcDrawLine.right, rcDrawLine.bottom); 
	dc.LineTo(rcDrawLine.left, rcDrawLine.bottom); 
	dc.LineTo(rcDrawLine.left, rcDrawLine.top); 


	if (m_cManagerTopBarInfo.m_mapTopbarGroup.size() > 0)
	{
		DrawManagerTopBar(dc);
	}
	// ����ֻҪ����ʾ��client rect��Ӱ��ķ�Χ�ڣ�ϵͳ�ᰴ��Z�ᷢ��WM_PAINT?
// 	for (int32 i = 0 ; i < m_IoViewsPtr.GetSize(); i++)
// 	{
// 		CIoViewBase* pIoView = m_IoViewsPtr[i];
// 
// 		if ( NULL ==  pIoView )
// 		{
// 			continue;
// 		}
// 		
// 		if ( !BeTestValid() )
// 		{
// 			OnTabSelChange();
// 		}
// 		//
// 		if ( i == m_GuiTabWnd.GetCurtab() )
// 		{			
// 			// ����Ҫ֪ͨ��ϵͳò�����Զ�������ռ����֪ͨ�ػ�
// // 			CRect rcNeedDraw;
// // 			int32 iRcClipMode = dc.GetClipBox(&rcNeedDraw);
// // 			if ( ERROR != iRcClipMode && NULLREGION != iRcClipMode )
// // 			{
// // 				// ֻ������Ҫ���ƵĲ��֣�����������˸
// // 				pIoView->RedrawWindow(&rcNeedDraw, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);
// // 			}
// // 			else
// // 			{
// // 				pIoView->RedrawWindow();		
// // 			}		
// 		}		
// 	}
}

BOOL CIoViewManager::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

int CIoViewManager::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if (!m_GuiTabWnd.Create(WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,0x9999))
		return -1;
	m_GuiTabWnd.SetUserCB(this);


	// ���÷�����Ϣ
	CWnd* pWnd = GetParent();
	while (pWnd)
	{
		if ( pWnd->IsKindOf(RUNTIME_CLASS(CTabSplitWnd)))
		{
			CTabSplitWnd* pParent = (CTabSplitWnd*)pWnd;
			m_iGroupId = pParent->GetGroupID();
			break;
		}
		
		if ( pWnd->IsKindOf(RUNTIME_CLASS(CMPIChildFrame)))
		{
			CMPIChildFrame* pParent = (CMPIChildFrame*)pWnd;
			m_iGroupId = pParent->GetGroupID();
			break;
		}

		pWnd = pWnd->GetParent();
	}

	//
	m_GuiTabWnd.SetTipTitle(L"ҵ����ͼ");	
/*
	//--- wangyongxue ��ʱ���趨�̶���С
	m_GuiTabWnd.SetTabLength(80);

	m_GuiTabWnd.SetTabTextColor(m_clrTabTextNor, m_clrTabTextSel);
	m_GuiTabWnd.SetTabBkColor(m_clrTabBkNor, m_clrTabBkSel);
	m_GuiTabWnd.SetBkGround(false, m_clrTabBk,0,0);
	m_GuiTabWnd.SetTabFrameColor(m_clrTabBkSel);
*/
	// m_GuiTabWnd.SetImageList(IDB_TAB, 16, 16, 0x0000ff);
	
/*
	CIoViewBase *pIoView = (CIoViewBase *)m_pDefaultIoViewClass->CreateObject();
	const T_IoViewObject * pstIoViewObject = FindIoViewObjectByIoViewPtr(pIoView);
	ASSERT(NULL != pstIoViewObject);

	pIoView->SetViewData(pAbsCenterManager);
	pIoView->SetIoViewManager(this);
	pIoView->Create(WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10204);
	
	// ������ͼ����
	m_IoViewsPtr.Add(pIoView);
	
	// ���Ӷ�Ӧ�ı�ǩҳ
	m_GuiTabWnd.Addtab(pstIoViewObject->m_StrLongName, pstIoViewObject->m_StrShortName, pstIoViewObject->m_StrTipMsg);				
	
	// ����ͼ��
	int32 iTabCount = m_GuiTabWnd.GetCount();
	m_GuiTabWnd.SetTabIcon(iTabCount - 1, 0);
	
	// ������ʾ��һ��
	for (int i = 0; i < m_IoViewsPtr.GetSize(); i++)
	{
		CIoViewBase *pIoViewBase = m_IoViewsPtr[i];
		if (NULL == pIoViewBase)
			continue;

		if (0 == i)
		{
			pIoViewBase->ShowWindow(SW_SHOW);
			pIoViewBase->OnIoViewActive();
			pIoViewBase->OnVDataForceUpdate();	// ǿ��ˢ��
		}
		else
		{
			pIoViewBase->ShowWindow(SW_HIDE);
			pIoViewBase->OnIoViewDeactive();
		}
	}

	m_GuiTabWnd.SetCurtab(0);
		//
*/
	return 0;
}

void CIoViewManager::ManagerTopBarSizeChange(const CRect& rect)
{
	// ����������Ϊ�����ұߣ�����ߺ��м䡣�ռ䲻���Ͳ�������
	CRect rcAreaOffset(1,1,1,1);
	int iClientWidth = rect.Width();
	int iBtnWidth = m_cManagerTopBarInfo.m_iTopBarBtnWidth;		// ��ť���Ϊ80������
	int iBtnWidthRight = 110;
	if (m_cManagerTopBarInfo.m_pImageBk)
	{
		iBtnWidthRight = m_cManagerTopBarInfo.m_pImageBk->GetWidth();
	}
	
	int iLeftWidthCount		= m_cManagerTopBarInfo.m_mapTopbarBtnLeft.size()*iBtnWidth;
	int iRihgtWidthCount	= m_cManagerTopBarInfo.m_mapTopbarBtnRight.size()*iBtnWidthRight;
	int iCenterWidthCount	= m_cManagerTopBarInfo.m_mapTopbarBtnCenter.size()*iBtnWidth;

	// ����ߵ��Ȼ�����˵
	m_rcManagerTopBtn = rect;
	m_rcManagerTopBtn.bottom = m_rcManagerTopBtn.top + m_cManagerTopBarInfo.m_iBarHight;

	// ��ఴť
	CRect rcLeftBtn = m_rcManagerTopBtn;
	rcLeftBtn.right = rcLeftBtn.left;
	map<int32, CNCButton>::iterator iterleft;
	for (iterleft = m_cManagerTopBarInfo.m_mapTopbarBtnLeft.begin(); iterleft != m_cManagerTopBarInfo.m_mapTopbarBtnLeft.end(); iterleft++)
	{
		CNCButton &ncButton = iterleft->second;
		rcLeftBtn.left = rcLeftBtn.right;
		rcLeftBtn.right += iBtnWidth;

		// ��ռ�����Ҳఴťλ��
		if (iClientWidth - rcLeftBtn.right < iRihgtWidthCount)
		{
			break;;
		}
		// ����С��������
		if (m_cManagerTopBarInfo.m_mapTopbarGroup.count(iterleft->first) > 0)
		{
			int iShrinkSize = m_cManagerTopBarInfo.m_mapTopbarGroup[iterleft->first].m_iShrinkSize;
			rcLeftBtn.DeflateRect(iShrinkSize,iShrinkSize,iShrinkSize,iShrinkSize);
		}

		ncButton.SetRect(rcLeftBtn);

		// ��Ч�����ȥ1����ΪҪ�����ص�
		ncButton.SetAreaOffset(rcAreaOffset);
		rcLeftBtn.right -= 1;
	}

	// �Ҳఴť
	CRect rcRightBtn = m_rcManagerTopBtn;
	rcRightBtn.left = rcRightBtn.right;
	map<int32, CNCButton>::iterator iterRight;
	for (iterRight = m_cManagerTopBarInfo.m_mapTopbarBtnRight.begin(); iterRight != m_cManagerTopBarInfo.m_mapTopbarBtnRight.end(); iterRight++)
	{
		CNCButton &ncButton = iterRight->second;

		rcRightBtn.right = rcRightBtn.left;
		rcRightBtn.left  -= iBtnWidthRight;

		// ���ߵĲ������ƣ�������ť����
		if (rcRightBtn.left < 0)
		{
			break;
		}

		// ����С��������
		if (m_cManagerTopBarInfo.m_mapTopbarGroup.count(iterRight->first) > 0)
		{
			int iShrinkSize = m_cManagerTopBarInfo.m_mapTopbarGroup[iterRight->first].m_iShrinkSize;
			rcRightBtn.DeflateRect(iShrinkSize,iShrinkSize,iShrinkSize,iShrinkSize);
		}

		ncButton.SetRect(rcRightBtn);

		// ��Ч�����ȥ1����ΪҪ�����ص�
		ncButton.SetAreaOffset(rcAreaOffset);
		rcRightBtn.left += 1;
	}

	// �в���ť
	int iCenterStartPos = (iClientWidth - iCenterWidthCount)/2;
	if (iCenterStartPos > iLeftWidthCount  &&   (iCenterStartPos + iCenterWidthCount) < (iClientWidth - iRihgtWidthCount))
	{
		CRect rcCenterBtn = m_rcManagerTopBtn;
		rcCenterBtn.left = iCenterStartPos;
		rcCenterBtn.right = rcCenterBtn.left;
		map<int32, CNCButton>::iterator iterCenter;
		for (iterCenter = m_cManagerTopBarInfo.m_mapTopbarBtnCenter.begin(); iterCenter != m_cManagerTopBarInfo.m_mapTopbarBtnCenter.end(); iterCenter++)
		{
			CNCButton &ncButton = iterCenter->second;	
			rcCenterBtn.left  = rcCenterBtn.right;
			rcCenterBtn.right = rcCenterBtn.left + iBtnWidth;

			// ����С��������
			if (m_cManagerTopBarInfo.m_mapTopbarGroup.count(iterCenter->first) > 0)
			{
				int iShrinkSize = m_cManagerTopBarInfo.m_mapTopbarGroup[iterCenter->first].m_iShrinkSize;
				rcCenterBtn.DeflateRect(iShrinkSize,iShrinkSize,iShrinkSize,iShrinkSize);
			}

			ncButton.SetRect(rcCenterBtn);

			// ��Ч�����ȥ1����ΪҪ�����ص�
			ncButton.SetAreaOffset(rcAreaOffset);
			rcCenterBtn.right -= 1;
		}
	}
}

void CIoViewManager::OnSizeChange(const CRect& rect)
{
	//
	CRect RectTab(rect);
	CRect RectIoView(rect);

	RectTab.DeflateRect(2,2,2,2);
	RectIoView.DeflateRect(2,2,2,1);
	
	int32 iTabBarHorW = 22;//m_GuiTabWnd.GetFitHorW();
	BOOL bShowTabBar = TRUE;
	
	if ( 1 == m_IoViewsPtr.GetSize() && m_bAutoHide )
	{
		bShowTabBar = FALSE;
	}
	else if (EVTLeft == m_eViewTab)
	{
		// m_XTabBar.SetHorz(FALSE);
		RectTab.right = RectTab.left + iTabBarHorW;
		RectIoView.left = RectTab.right;
		if(m_bNeedFixWidth)
		{
			RectTab.bottom = m_iTabFixWidth * m_IoViewsPtr.GetSize() + 10;
		}
		m_GuiTabWnd.SetALingTabs(CGuiTabWnd::ALN_LEFT);
	}
	else if (EVTRight == m_eViewTab)
	{
		//		m_XTabBar.SetHorz(FALSE);
		RectTab.left = RectTab.right - iTabBarHorW;	
		RectIoView.right = RectTab.left;
		if(m_bNeedFixWidth)
		{
			RectTab.bottom = m_iTabFixWidth * m_IoViewsPtr.GetSize() + 10;
		}
		m_GuiTabWnd.SetALingTabs(CGuiTabWnd::ALN_RIGHT);
	}
	else  if (EVTTop == m_eViewTab)
	{
		// m_XTabBar.SetHorz(TRUE);
		RectTab.bottom = RectTab.top + iTabBarHorW;	
		RectIoView.top = RectTab.bottom + TAB_SPLITER_WIDTH;
		if(m_bNeedFixWidth)
		{
			RectTab.right = m_iTabFixWidth * m_IoViewsPtr.GetSize() + 10;
		}
		m_GuiTabWnd.SetALingTabs(CGuiTabWnd::ALN_TOP);
	}
	else  if (EVTBottom == m_eViewTab)
	{
		// m_XTabBar.SetHorz(TRUE);
		RectTab.DeflateRect(-1,0,0,0);
		RectTab.top = RectTab.bottom - iTabBarHorW;	
		RectTab.OffsetRect(0,2);
		RectIoView.bottom = RectTab.top - TAB_SPLITER_WIDTH;
		if(m_bNeedFixWidth)
		{
			RectTab.right = m_iTabFixWidth * m_IoViewsPtr.GetSize() + 10;
		}
		m_GuiTabWnd.SetALingTabs(CGuiTabWnd::ALN_BOTTOM);
	}
	else// if (EVTNone == m_eViewTab)
	{
		bShowTabBar = FALSE;
	}	

	if ( bShowTabBar )
	{
		// �����ǰ��F7ģʽ�£�����ʾ�ײ���ť��  ��Ҫ���÷�
		CMPIChildFrame *pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
		if ( NULL != pFrame && pFrame->IsF7AutoLock() )
		{
			bShowTabBar = FALSE;
			RectIoView = rect;
		}
	}
	
	//
	if (bShowTabBar)
	{
		m_rcGuiTabWnd = RectTab;
		m_GuiTabWnd.ShowWindow(SW_SHOW);	
		m_GuiTabWnd.MoveWindow(RectTab);
	}
	else
	{
		m_GuiTabWnd.ShowWindow(SW_HIDE);
	}


	if (m_cManagerTopBarInfo.m_mapTopbarGroup.size() > 0)
	{
		CRect rcTopBar(RectIoView);
		rcTopBar.top -= 1;
		rcTopBar.left -= 1;
		rcTopBar.right += 2;
		ManagerTopBarSizeChange(rcTopBar);
		RectIoView.top  = rcTopBar.top + m_cManagerTopBarInfo.m_iBarHight;
	}	

	if (m_bIsHasTopBarByLayout)
	{
		RectIoView.top -= 1;
	}
	if (m_bIsHasBottomBarByLayout)
	{
		RectIoView.bottom += 1;
	}
	
	//RectIoView.DeflateRect(2,1,2,2);
	//
	// �������ҪҪ����XML�г�ʼ�����״ε�ÿһ����ͼһ��OnSize���ᣬ��������������OnSize��Ϣ�ܹ������� xl ��ʱ�޸�
	CIoViewBase *pActiveIoView = GetActiveIoView();
	if ( !rect.IsRectEmpty() )
	{
		for ( int i=0; i < m_IoViewsPtr.GetSize() && m_bFromXmlNeedInit ; i++ )
		{
			// ����Size��Ϣ
			if ( m_IoViewsPtr[i] != pActiveIoView )
			{
				::MoveWindow(m_IoViewsPtr[i]->GetSafeHwnd(), RectIoView.left, RectIoView.top, RectIoView.Width(), RectIoView.Height(), TRUE);
				::MoveWindow(m_IoViewsPtr[i]->GetSafeHwnd(), 0, 0, 0, 0, TRUE);
			}
		}
		
		m_bFromXmlNeedInit = false;
	}
	
	
	if ( NULL != GetActiveIoView() )
	{
		if (GetActiveIoView()->IsKindOf(RUNTIME_CLASS(CIoViewChart)))
		{
			//RectIoView.DeflateRect(KiChartBorderLeft,KiChartBorderTop,KiChartBorderRight,KiChartBorderBottom);
		}		
		
		GetActiveIoView()->MoveWindow(RectIoView);
	}
	
	CRect rectRRR,rectClient;
	GetClientRect(&rectClient);
	m_GuiTabWnd.GetClientRect(&rectRRR);


	// ���ð�ť
	RestTopBtnStatus(m_LastPressTopBtnId);
}

void CIoViewManager::OnSize(UINT nType, int cx, int cy) 
{	
	CStatic::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CRect rect(0, 0, cx, cy);
	OnSizeChange(rect);
}

void CIoViewManager::DealTabKey()
{
	// �ӵ�ǰtab����ѡ����һ��
	int iCurSel = m_GuiTabWnd.GetCurtab();
	iCurSel++;
	if (iCurSel >= m_GuiTabWnd.GetCount())
		iCurSel = 0;
	
	m_GuiTabWnd.SetCurtab(iCurSel);
}

BOOL CIoViewManager::TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (VK_TAB == nChar || VK_F5 == nChar)
	{
		if ( VK_F5 == nChar )
		{
			DealF5Key();
		}
		else
		{
			// �ӵ�ǰtab����ѡ����һ��  
			if ( GetAsyncKeyState(VK_CONTROL) & 0x8000 )
			{
				return FALSE;
			}
			DealTabKey();
		}
		return TRUE;
	}

	// ����Ϣ���������ҵ����ͼ
	for (int i = 0; i < m_IoViewsPtr.GetSize(); i++)
	{
		CIoViewBase *pIoViewBase = m_IoViewsPtr[i];
		if (NULL != pIoViewBase)
		{
			if (pIoViewBase->TestKeyDown(nChar, nRepCnt, nFlags))
				return TRUE;
		}
	}

	return FALSE;
}

BOOL CIoViewManager::TestChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	for (int i = 0; i < m_IoViewsPtr.GetSize(); i++)
	{
		CIoViewBase *pIoViewBase = m_IoViewsPtr[i];
		if (NULL != pIoViewBase)
		{
			if (pIoViewBase->TestChar(nChar, nRepCnt, nFlags))
				return TRUE;
		}
	}

	return FALSE;
}

BOOL CIoViewManager::TestKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	for (int i = 0; i < m_IoViewsPtr.GetSize(); i++)
	{
		CIoViewBase *pIoViewBase = m_IoViewsPtr[i];
		if (NULL != pIoViewBase)
		{
			if (pIoViewBase->TestKeyUp(nChar, nRepCnt, nFlags))
				return TRUE;
		}
	}

	return FALSE;
}

LRESULT CIoViewManager::OnIoViewTitleButtonLButtonDown(WPARAM wParam,LPARAM lParam)
{
	int32 uID = (int32)wParam;

	if ( IOVIEW_TITLE_F7_BUTTON_ID == uID)
	{
		// F7 ��ť:
		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		if ( NULL != pMainFrame)
		{
			pMainFrame->OnProcessF7();
		}
	}
	else if ( IOVIEW_TITLE_ADD_BUTTON_ID == uID )
	{
		// ������ʾ�˵�,�����ͼ
		CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
		
		CNewMenu menu;
		menu.CreatePopupMenu();
//		menu.LoadToolBar(g_awToolBarIconIDs);

 		int32 iMenuCounts = CIoViewManager::GetIoViewObjectCount();
 		
 		for ( int32 i=0 ; i<iMenuCounts ;i++)
 		{
 			CString MenuName = CIoViewManager::GetIoViewObject(i)->m_StrLongName;
 			
 			if ( CIoViewManager::GetIoViewObject(i)->m_pIoViewClass == RUNTIME_CLASS(CIoViewReport))
 			{
 				// ���۱�������Ӳ˵�
 				CNewMenu * pNewMenu = menu.AppendODPopupMenu(MenuName);
 				if ( NULL != pNewMenu )
 				{
 					CArray<T_BlockMenuInfo ,T_BlockMenuInfo&> aBlockMenuInfo; 
 					pMainFrame->GetBlockMenuInfo(aBlockMenuInfo);
 					CNewMenu * pMenuMerch = NULL;
 					
 					for (int32 i = 0; i<aBlockMenuInfo.GetSize(); i++)
 					{			
 						if (aBlockMenuInfo[i].iBlockMenuIDs == 0)
 						{
 							pMenuMerch = pNewMenu->AppendODPopupMenu(aBlockMenuInfo[i].StrBlockMenuNames);
 						}
 						
 						CString StrBreedName = aBlockMenuInfo[i].StrBlockMenuNames;
 						if (StrBreedName == L"���֤ȯ" || StrBreedName == L"�¼����ڻ�" || StrBreedName == L"ȫ��ָ��")
 						{
 							pNewMenu->AppendODMenu(L"",MF_SEPARATOR);
 						}
						if(pMenuMerch)
						{
							pMenuMerch->AppendODMenu(aBlockMenuInfo[i].StrBlockMenuNames,MF_STRING,aBlockMenuInfo[i].iBlockMenuIDs);
						}
 					}
 				}
 			}
 			else
 			{
 				menu.AppendODMenu(MenuName,MF_STRING,ID_PIC_BEGIN + i);
 			}			
 		}

		CPoint pt;
		GetCursorPos(&pt);
		menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y,AfxGetMainWnd());			
		menu.DestroyMenu();	
	}
	else if ( IOVIEW_TITLE_DEL_BUTTON_ID == uID )
	{
		// ɾ����ǰ��ͼ
		int32 iCurTab = m_GuiTabWnd.GetCurtab();
		if ( iCurTab < 0 || iCurTab >= m_GuiTabWnd.GetCount())
		{
			return 0;
		}

		CIoViewBase* pIoView = m_IoViewsPtr.GetAt(iCurTab);
		
		if (NULL == pIoView)
		{
			return 0;
		}

		pIoView->DestroyWindow();
		DEL(pIoView);
		m_IoViewsPtr.RemoveAt(iCurTab);
		m_GuiTabWnd.DeleteTab(iCurTab);

		int32 iCountTab  = m_GuiTabWnd.GetCount();
		int32 iCountView = m_IoViewsPtr.GetSize();
		
		if ( 0 == iCountTab || 0 == iCountView )
		{			
			CGGTongView* pParent = (CGGTongView*)GetParent();
			pParent->RedrawWindow();
			RegisterActiveIoViewToMainFrame(false);
			((CMainFrame *)AfxGetMainWnd())->SetEmptyGGTongView(pParent);	// ���ÿ���ͼ
			return 0;
		}
		
		int32 iCurView   = iCountView -1;
		
		m_GuiTabWnd.SetCurtab(iCurView);
		m_IoViewsPtr[iCurView]->ShowWindow(SW_SHOW);

	//	m_IoViewsPtr[iCurView]->onshow(BOOL bshow);

		for ( int32 i = 0; i < iCountView; i++ )
		{
			CIoViewBase* pTmpIoView = m_IoViewsPtr[i];
			if ( NULL != pTmpIoView )
			{
				CRect RectIoView;
				
				pTmpIoView->GetClientRect(&RectIoView);
				LPARAM lTmpParam = MAKELPARAM(RectIoView.Width(), RectIoView.Height());

				pTmpIoView->PostMessage(WM_SIZE, 0, lTmpParam);
			}
		}

		//
		CGGTongView* pParent = (CGGTongView*)GetParent();
		if ( NULL != pParent )
		{
			CRect RectParent;
			pParent->GetClientRect(&RectParent);
			
			int32 iHeight = RectParent.Height();
			int32 iWidth  = RectParent.Width();
			
			LPARAM lpRarm = MAKELPARAM(iWidth, iHeight);

			pParent->PostMessage(WM_SIZE, 0, lpRarm);
		}

		m_bDelIoView = true;
		OnTabSelChange();
		
		SetTimer(1, 10, NULL);
	}
	else if ( IOVIEW_TITILE_LEFT_BUTTON_ID == uID )
	{
		// �����

		int32 iCurTab = m_GuiTabWnd.GetCurtab();
		if ( iCurTab < 0 || iCurTab >= m_GuiTabWnd.GetCount())
		{
			return 0;
		}
		
		CIoViewBase* pIoView = m_IoViewsPtr.GetAt(iCurTab);
		
		if (NULL == pIoView)
		{
			return 0;
		}

		int32 iTab = iCurTab - 1;
		
		if ( iTab < 0 )
		{
			// �����ͷ��,��ʾ���ұ�һ��.ѭ��.
			iTab = m_GuiTabWnd.GetCount() - 1;
		}

		m_GuiTabWnd.SetCurtab(iTab);
		m_IoViewsPtr[iTab]->ShowWindow(SW_SHOW);
	}
	else if ( IOVIEW_TITILE_RIGHT_BUTTON_ID == uID)
	{
		// �ҷ����
		int32 iCurTab = m_GuiTabWnd.GetCurtab();
		if ( iCurTab < 0 || iCurTab >= m_GuiTabWnd.GetCount())
		{
			return 0;
		}
		
		CIoViewBase* pIoView = m_IoViewsPtr.GetAt(iCurTab);
		
		if (NULL == pIoView)
		{
			return 0;
		}
		
		int32 iTab = iCurTab + 1;
		
		if ( iTab >= m_GuiTabWnd.GetCount() )
		{
			// �����ͷ��,��ʾ���ұ�һ��.ѭ��.
			iTab = 0;
		}
		
		m_GuiTabWnd.SetCurtab(iTab);
		m_IoViewsPtr[iTab]->ShowWindow(SW_SHOW);

	}
	else
	{
		// ����������ҵ����ͼ
		for ( int32 i = 0 ; i < m_IoViewsPtr.GetSize(); i++)
		{
			if ( m_IoViewsPtr[i]->GetGuid() == uID )
			{
				m_GuiTabWnd.SetCurtab(i);
				m_IoViewsPtr[i]->ShowWindow(SW_SHOW);
				return 0;
			}
		}

	}

	return 0;
}

void CIoViewManager::OnSelChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// handle notification from the tab control
	
	OnTabSelChange();

	*pResult = 0;
}

void CIoViewManager::OnTabSelChange()
{
	int iCurSel = m_GuiTabWnd.GetCurtab();
	
	for (int i = 0; i < m_IoViewsPtr.GetSize(); i++)
	{
		CIoViewBase *pIoViewBase = m_IoViewsPtr[i];
		if (NULL == pIoViewBase)
			continue;
		
		if (i != iCurSel)
		{
			pIoViewBase->m_bShowNow = false;
			pIoViewBase->SetKillerTimer(false);

			pIoViewBase->OnIoViewTabHide();
			pIoViewBase->ShowWindow(SW_HIDE);
			pIoViewBase->OnIoViewDeactive();
			
			// 0001722 ��ioViewManager�е�onSize����鷳���������еķ�active ioview��sizeȫ��������ָ�������
			pIoViewBase->MoveWindow(CRect(0,0,0,0), FALSE);  //0001722
		}
		else
		{
			pIoViewBase->m_bShowNow = true;
			pIoViewBase->SetHasIgnoreLastViewDataReqFlag(true);
			pIoViewBase->SetKillerTimer(true);

			//
			pIoViewBase->OnIoViewTabShow();
			pIoViewBase->ShowWindow(SW_SHOW);
			
			//0001722
			CRect	rect;
			GetClientRect(&rect);
			//OnSize(SIZE_RESTORED, rect.Width(), rect.Height());     //0001722 - ��ioViewManager�����ڼ����IoView�ָ�size��
			OnSizeChange(rect);
			//0001722 -end
			
			pIoViewBase->OnIoViewActive();
			//pIoViewBase->OnVDataForceUpdate();	// ǿ��ˢ�� - xl 0904 ʹ��timer����Ƿ��к����������У����ڶ�ʱ���ڣ����ڿɼ�ʱǿ��ˢ�£�����Ҫ��ʱǿ��
			
			//#BUG_MARK_BEGIN [NO=0001536 AUTHOR=xiali DATE=2010/04/09]
			//DESCRIPTION:	��ʱ�����������Ҫ��IoViewBase����������Ϊ��ͳһ��ֻ���������ػ���
			pIoViewBase->Invalidate(TRUE);	//bug xiali ��ʱ��� 
			//#BUG_MARK_END [NO=0001536]
		}
	}

	//
	if ( !m_bDelIoView )
	{
		CIoViewBase* pIoViewBase = m_IoViewsPtr[iCurSel];
		const T_IoViewObject* pIoViewObj = FindIoViewObjectByIoViewPtr(pIoViewBase);
		
		if ( NULL != pIoViewObj )
		{
			if ( EIBSNone != pIoViewObj->m_eBeyondShow )
			{
				// ��ǰ�������������ͼ, ����ԭ�����������
				if ( !m_bBeyondShow )
				{
					BeyondShowIoView(true, pIoViewBase);
				}					
			}
			else
			{
				// ��ǰ����������ͼ, ����ԭ���������
				if ( m_bBeyondShow )
				{
					BeyondShowIoView(false, pIoViewBase);
				}
			}				
		}
	}
	
	//
	RegisterActiveIoViewToMainFrame(true);
} 

CString	CIoViewManager::GetViewTabString(E_ViewTab eViewTab)
{
	CString StrViewTab = L"none";
	if (EVTLeft == eViewTab)
		StrViewTab = L"left";
	else if (EVTRight == eViewTab)
		StrViewTab = L"right";
	else if (EVTTop == eViewTab)
		StrViewTab = L"top";
	else if (EVTBottom == eViewTab)
		StrViewTab = L"bottom";

	return StrViewTab;
}

CIoViewManager::E_ViewTab CIoViewManager::GetViewTab(const char *StrViewTab)
{
	E_ViewTab eViewTab = EVTNone;
	
	if (strcmp(StrViewTab,"left") ==0 )
		eViewTab = EVTLeft;
	else if (strcmp(StrViewTab,"right") ==0 )
		eViewTab = EVTRight;
	else if (strcmp(StrViewTab,"top") ==0 )
		eViewTab = EVTTop;
	else if (strcmp(StrViewTab,"bottom") ==0 )
		eViewTab = EVTBottom;
	return eViewTab;
}

void CIoViewManager::SetTabAllign(E_ViewTab eViewTab)
{
	if ( m_eViewTab == eViewTab )
	{
		return;
	}

	//
	m_eViewTab = eViewTab;

	//
	CRect rect;
	GetClientRect(rect);
	OnSizeChange(rect);
}

CString CIoViewManager::GetIoViewString(CIoViewBase *pIoView)
{
	CString StrIoViewXmlName;

	const T_IoViewObject *pstIoViewObject = FindIoViewObjectByIoViewPtr(pIoView);
	
	if (NULL != pstIoViewObject)
		StrIoViewXmlName = pstIoViewObject->m_StrXmlName;

	return StrIoViewXmlName;
}

CString CIoViewManager::GetIoViewString(UINT uID)
{
	CString StrIoViewXmlName = L"";

	for ( int32 i = 0 ; i < KIoViewObjectCount ; i++)
	{
		if ( uID == s_astIoViewObjects[i].m_uID)
		{
			StrIoViewXmlName = s_astIoViewObjects[i].m_StrXmlName;
			break;
		}  
	}

	return StrIoViewXmlName;
}

E_IoViewGroupType CIoViewManager::GetIoViewGroupType(const CIoViewBase* pIoView)
{
	if ( NULL == pIoView)
	{
		return E_IVGTNone;
	}

	int32 i = 0;
	for ( i = 0 ; i < KIoViewObjectCount; i++)
	{
		if (pIoView->GetRuntimeClass() == s_astIoViewObjects[i].m_pIoViewClass)
		{
			return s_astIoViewObjects[i].m_eGroupType;
		}	
	}

	for ( i = 0 ; i < KIoViewObjectCount; i++)
	{
		if (pIoView->IsKindOf(s_astIoViewObjects[i].m_pIoViewClass))
		{
			return s_astIoViewObjects[i].m_eGroupType;
		}	
	}

	return E_IVGTNone;
} 

CString CIoViewManager::GetIoViewGroupName(const CIoViewBase* pIoView)
{
	if ( NULL == pIoView)
	{
		return L"";
	}
	
	E_IoViewGroupType eType = GetIoViewGroupType(pIoView);

	for ( int32 i = 0 ; i < KIoViewGroupObjectCount; i++)
	{
		if ( eType == s_astIoViewGroupObjects[i].m_eIoViewGroupType)
		{
			return s_astIoViewGroupObjects[i].m_StrLongName;				 
		}
	}

	return L"";
}

CString CIoViewManager::ToXmlForWorkSpace(const char * KpcFileName)
{
	CString StrThis;
	StrThis  = L"<?xml version =\"1.0\" encoding=\"UTF-8\"?> \n";	
	StrThis += L"<XMLDATA version=\"1.0\" app = \"ggtong\" data = \"IoViewManger\">";
	StrThis += ToXml();
	StrThis += L"</XMLDATA>";

	SaveXmlFile(KpcFileName,StrThis);
	
	return StrThis;
}

CString CIoViewManager::ToXml()
{
	CString StrTabBkClr, StrTabBkNorClr, StrTabBkSelClr, StrTabTextNorClr, StrTabTextSelClr, StrTabSpliter;	
	StrTabBkClr = ColorRefToStr(m_clrTabBk);
	StrTabBkNorClr = ColorRefToStr(m_clrTabBkNor);	
	StrTabBkSelClr = ColorRefToStr(m_clrTabBkSel);
	StrTabTextNorClr = ColorRefToStr(m_clrTabTextNor);
	StrTabTextSelClr = ColorRefToStr(m_clrTabTextSel);	
	StrTabSpliter = ColorRefToStr(m_clrTabSpliter);	

	CString StrThis;
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%d\"",
							CString(KStrElementValue).GetBuffer(), 
							CString(KStrElementAttrViewTab).GetBuffer(), 
							GetViewTabString(m_eViewTab).GetBuffer(),
							CString(KStrElementAttrTabBkClr).GetBuffer(),
							StrTabBkClr.GetBuffer(),
							CString(KStrElementAttrTabBkNorClr).GetBuffer(),
							StrTabBkNorClr.GetBuffer(),
							CString(KStrElementAttrTabBkSelClr).GetBuffer(),
							StrTabBkSelClr.GetBuffer(),
							CString(KStrElementAttrTabTextNorClr).GetBuffer(),
							StrTabTextNorClr.GetBuffer(),
							CString(KStrElementAttrTabTextSelClr).GetBuffer(),
							StrTabTextSelClr.GetBuffer(),
							CString(KStrElementAttrTabSpliterClr).GetBuffer(),
							StrTabSpliter.GetBuffer(),
							CString(KStrElementAttrCurView).GetBuffer(), 
							m_GuiTabWnd.GetCurtab(),
							CString(KStrElementAttrAutoHide).GetBuffer(),
							m_bAutoHide,
							CString(KStrElementAttrGroupId).GetBuffer(),
							m_iGroupId,
							CString(KStrElementAttrTabFixWidth).GetBuffer(),
							m_iTabFixWidth);	

	// ����������
	CString StrTopBottomBar;
	StrTopBottomBar.Format(L"%s=\"%d\" %s=\"%d\" ",
					CString(KStrElementAttrbIsHasTopBarByLayout).GetBuffer(),
					m_bIsHasTopBarByLayout, 
					CString(KStrElementAttrbIsHasBottomBarByLayout).GetBuffer(),
					m_bIsHasBottomBarByLayout);


	
	// manager������־
	StrThis += L">\n";
	// ��֯������ҵ����ͼ
	CIoViewBase *pIoView = NULL;
	for (int32 i = 0; i < m_IoViewsPtr.GetSize(); i++)
	{
		pIoView = m_IoViewsPtr[i];
		if (NULL == pIoView)
			continue;
		StrThis += pIoView->ToXml();
	}


	// ���涥����ǩ����
	// ����������Ʒ�б�
	if (m_cManagerTopBarInfo.m_mapTopbarGroup.size() > 0)
	{
		CString StrText;
		StrText.Format(_T("<%s  %s=\"%d\" %s=\"%d\" %s=\"%s\" %s=\"%d\" %s=\"%s\" >\n"),
				CString(KStrElementAttrManagerTopBtn).GetBuffer(), 
				CString(KStrElementAttrManagerBarHight).GetBuffer(),m_cManagerTopBarInfo.m_iBarHight,
				CString(KStrElementAttrtabbarbtnWidth).GetBuffer(),m_cManagerTopBarInfo.m_iTopBarBtnWidth,
				CString(KStrElementAttrManagerBarBkColor).GetBuffer(),ColorRefToStr(m_cManagerTopBarInfo.m_colBarBKColor).GetBuffer(),
				CString(KStrElementAttrManagerBarType).GetBuffer(),m_cManagerTopBarInfo.m_iTopBarType,
				CString(KStrElementAttrRightimagePath).GetBuffer(),m_cManagerTopBarInfo.m_strImagePath.GetBuffer());
		StrThis += StrText;
		map<int32, T_ManagerTopBar>::iterator iter;
		for (iter = m_cManagerTopBarInfo.m_mapTopbarGroup.begin(); iter != m_cManagerTopBarInfo.m_mapTopbarGroup.end(); iter++)
		{
			T_ManagerTopBar stManagerTopBar = iter->second;

			// ��ʾ��ʽ
			CString strAlignment;
			if (ALIGNMENT_LEFT == stManagerTopBar.m_iAlignmentType)
			{
				strAlignment = L"left";
			}
			else if (ALIGNMENT_RIGHT == stManagerTopBar.m_iAlignmentType)
			{
				strAlignment = L"right";
			}
			else if (ALIGNMENT_CENTER == stManagerTopBar.m_iAlignmentType)
			{
				strAlignment = L"right";
			}

			//// ����ַ���
			CString StrTopText;
			StrTopText.Format(_T("<%s %s=\"%d\" %s=\"%s\" %s=\"%d\"  %s=\"%s\"  %s=\"%s\"  %s=\"%d\"  \
				%s=\"%d\"  %s=\"%s\"  %s=\"%s\"  %s=\"%s\"  %s=\"%s\"  %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" />\n"),
				CString(KStrElementAttrtabbar).GetBuffer(),
				CString(KStrElementAttrbtnType).GetBuffer(), stManagerTopBar.m_iBtnType,
				CString(KStrElementAttrname).GetBuffer(), stManagerTopBar.m_StrShowName,
				CString(KStrElementAttrmarketid).GetBuffer(), stManagerTopBar.m_iMarketID,
				CString(KStrElementAttrMerchCode).GetBuffer(), stManagerTopBar.m_strMerchCode,
				CString(KStrElementAttrcfmname).GetBuffer(), stManagerTopBar.m_strCfmName,
				CString(KStrElementAttrShrinkSize).GetBuffer(), stManagerTopBar.m_iShrinkSize,
				CString(KStrElementAttrisSelected).GetBuffer(), stManagerTopBar.m_bSelected,
				CString(KStrElementAttrAlignmente).GetBuffer(), strAlignment.GetBuffer(),
				CString(KIoViewChartIndexNomarlBkColor).GetBuffer(), ColorRefToStr(stManagerTopBar.m_colNoarml).GetBuffer(),
				CString(KIoViewChartIndexPressBkColor).GetBuffer(), ColorRefToStr(stManagerTopBar.m_colPress).GetBuffer(),
				CString(KIoViewChartIndexHotBkColor).GetBuffer(),   ColorRefToStr(stManagerTopBar.m_colHot).GetBuffer(),
				CString(KIoViewChartIndexNomarlTextColor).GetBuffer(), ColorRefToStr(stManagerTopBar.m_colTextNoarml).GetBuffer(),
				CString(KIoViewChartIndexPressTextColor).GetBuffer(),  ColorRefToStr(stManagerTopBar.m_colTextPress).GetBuffer(),
				CString(KIoViewChartIndexHotTextColor).GetBuffer(), ColorRefToStr(stManagerTopBar.m_colTextHot).GetBuffer(),
				CString(KStrElementAttrframeColor).GetBuffer(), ColorRefToStr(stManagerTopBar.m_colTextframe).GetBuffer());
			StrThis += StrTopText;
		}
		StrText.Format(_T("</%s>\n"),CString(KStrElementAttrManagerTopBtn).GetBuffer());
		StrThis += StrText;
	}

	//
	StrThis += L"</";
	StrThis += KStrElementValue;
	StrThis += L">\n";
	//
	return StrThis;
}

bool32 CIoViewManager::FromXml(TiXmlElement *pElement)
{
#ifdef _DEBUG
	DWORD dwTime = timeGetTime();
#endif
	if (NULL == pElement)
		return false;

	// �ж��ǲ��������Լ���
	const char *pcValue = pElement->Value();
	if (strcmp(pcValue, KStrElementValue) != 0)
		return false;

	// �����õ�
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;
	if (NULL == pDocument)
		return false;

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	if (NULL == pAbsCenterManager)
		return false;

	// �����ǰ���е���Ϣ
	int32 i = 0;
	for ( i = 0; i < m_IoViewsPtr.GetSize(); i++)
	{
		m_IoViewsPtr[i]->DestroyWindow();
		DEL(m_IoViewsPtr[i]);
	}

	m_IoViewsPtr.RemoveAll();	
	//
	m_GuiTabWnd.DeleteAll();

	// ��������
	E_ViewTab eViewTab = EVTNone;
	int32	  iCurView = 0;

	const char *pcAttrValue = pElement->Attribute(KStrElementAttrViewTab);
	if (NULL != pcAttrValue)
		eViewTab = GetViewTab(pcAttrValue);

	pcAttrValue = pElement->Attribute(KStrElementAttrCurView);
	if (NULL != pcAttrValue)
		iCurView = atoi(pcAttrValue);

	pcAttrValue = pElement->Attribute(KStrElementAttrAutoHide);
	if (NULL != pcAttrValue)
		m_bAutoHide = atoi(pcAttrValue);

	pcAttrValue = pElement->Attribute(KStrElementAttrGroupId);
	if (NULL != pcAttrValue)
		m_iGroupId = atoi(pcAttrValue);

	pcAttrValue = pElement->Attribute(KStrElementAttrTabFixWidth);
	if (NULL != pcAttrValue)
	{
		m_iTabFixWidth = atoi(pcAttrValue);
		if(m_iTabFixWidth != 0)
		{
			m_bNeedFixWidth = true;
		}
		else
		{
			m_bNeedFixWidth = false;
		}
	}
	else
	{
		m_bNeedFixWidth = false;
	}

	//
	if (EVTLeft == eViewTab)
	{
		m_GuiTabWnd.SetALingTabs(CGuiTabWnd::ALN_LEFT);
	}
	else if (EVTRight == eViewTab)
	{
		m_GuiTabWnd.SetALingTabs(CGuiTabWnd::ALN_RIGHT);
	}
	else  if (EVTTop == eViewTab)
	{
		m_GuiTabWnd.SetALingTabs(CGuiTabWnd::ALN_TOP);
	}
	else  if (EVTBottom == eViewTab)
	{
		m_GuiTabWnd.SetALingTabs(CGuiTabWnd::ALN_BOTTOM);
	}

	// tab��ɫ
	{
		pcAttrValue = pElement->Attribute(KStrElementAttrTabBkClr);
		if ((NULL!=pcAttrValue) && strlen(pcAttrValue) > 0)
		{
			m_clrTabBk = StrToColorRef(pcAttrValue);
		}

		pcAttrValue = pElement->Attribute(KStrElementAttrTabBkNorClr);
		if ((NULL!=pcAttrValue) && strlen(pcAttrValue) > 0)
		{
			m_clrTabBkNor = StrToColorRef(pcAttrValue);
		}

		pcAttrValue = pElement->Attribute(KStrElementAttrTabBkSelClr);
		if ((NULL!=pcAttrValue) && strlen(pcAttrValue) > 0)
		{
			m_clrTabBkSel = StrToColorRef(pcAttrValue);
		}

		pcAttrValue = pElement->Attribute(KStrElementAttrTabTextNorClr);
		if ((NULL!=pcAttrValue) && strlen(pcAttrValue) > 0)
		{
			m_clrTabTextNor = StrToColorRef(pcAttrValue);
		}

		pcAttrValue = pElement->Attribute(KStrElementAttrTabTextSelClr);
		if ((NULL!=pcAttrValue) && strlen(pcAttrValue) > 0)
		{
			m_clrTabTextSel = StrToColorRef(pcAttrValue);
		}

		pcAttrValue = pElement->Attribute(KStrElementAttrTabSpliterClr);
		if ((NULL!=pcAttrValue) && strlen(pcAttrValue) > 0)
		{
			m_clrTabSpliter = StrToColorRef(pcAttrValue);
		}

		m_GuiTabWnd.SetTabTextColor(m_clrTabTextNor, m_clrTabTextSel);
		m_GuiTabWnd.SetTabBkColor(m_clrTabBkNor, m_clrTabBkSel);
		m_GuiTabWnd.SetBkGround(false, m_clrTabBk,0,0);
		m_GuiTabWnd.SetTabFrameColor(m_clrTabBkSel);
	}


	// �����ж�ֱ��ͨ����û����������ʼ��
	if (FromXmlTopBtn(pElement))
	{	
		InitTopBtn();
	}

	pcAttrValue = pElement->Attribute(KStrElementAttrbIsHasTopBarByLayout);
	if (NULL != pcAttrValue )
	{
		m_bIsHasTopBarByLayout = atol ( pcAttrValue);
	}

	pcAttrValue = pElement->Attribute(KStrElementAttrbIsHasBottomBarByLayout);
	if (NULL != pcAttrValue )
	{
		m_bIsHasBottomBarByLayout = atol ( pcAttrValue);
	}
		

	// ��ѯ���е��ӽڵ㣬 �������������ľ�����ͼ������ӦIoView��ͼ
	TiXmlElement *pElementIoView = pElement->FirstChildElement();
	while (pElementIoView)
	{
		pcValue = pElementIoView->Value();
		if (strcmp(CIoViewBase::GetXmlElementValue(), pcValue) == 0)	// �ҵ�ҵ����ͼ
		{
			const char *pcTmpAttrValue = pElementIoView->Attribute(CIoViewBase::GetXmlElementAttrIoViewType());
			if (NULL == pcTmpAttrValue)	// �Ҳ�������ҵ����ͼ���ͣ� ���У�
				return FALSE;

			CString StrIoViewXmlName = pcTmpAttrValue;
			const T_IoViewObject *pstIoViewObject = FindIoViewObjectByXmlName(StrIoViewXmlName);
			if (NULL == pstIoViewObject)	// �Ҳ�����Ӧ��ҵ����ͼ���ͣ����У�
				return FALSE;

			// ����ҵ����ͼ
			if (NULL == pstIoViewObject->m_pIoViewClass)
				return FALSE;

			// tab��ʾ������
			CString StrTabName = _T("");
			const char *pcAttrTabName = pElementIoView->Attribute(CIoViewBase::GetXmlElementAttrShowTabName());
			if ( (NULL!=pcAttrTabName) && (strlen(pcAttrTabName)>0))
			{
				StrTabName = _A2W(pcAttrTabName);
			}
			else
			{
				StrTabName = pstIoViewObject->m_StrLongName;
			}

#ifdef _DEBUG
			DWORD dwTmpTime = timeGetTime();
#endif

			CIoViewBase *pIoView = (CIoViewBase *)pstIoViewObject->m_pIoViewClass->CreateObject();
			pIoView->SetCenterManager(pAbsCenterManager);
			pIoView->SetIoViewManager(this);
			pIoView->Create(WS_CHILD|SS_LEFT|SS_NOTIFY/*|WS_VISIBLE*/,this,CRect(0,0,0,0), 10204);	// Ĭ�϶����ɼ�
			pIoView->m_StrTabShowName = StrTabName;
			
#ifdef _DEBUG
			TRACE(_T("��ͼ[%s]����: %d ms\r\n"), StrIoViewXmlName.GetBuffer(), timeGetTime()-dwTmpTime);
			dwTmpTime = timeGetTime();
#endif

			if (!pIoView->FromXml(pElementIoView))
				return FALSE;
			
#ifdef _DEBUG
			TRACE(_T("��ͼ[%s]FromXml: %d ms\r\n"), StrIoViewXmlName.GetBuffer(), timeGetTime()-dwTmpTime);
			dwTmpTime = timeGetTime();
#endif
			// ��ÿ��IoViewһ�γ�ʼ����ͼ����Դ(OnSize)�Ļ���
		
			// ������ͼ����
			m_IoViewsPtr.Add(pIoView);

			// ���Ӷ�Ӧ�ı�ǩҳ
			m_GuiTabWnd.Addtab(StrTabName, pstIoViewObject->m_StrShortName, pstIoViewObject->m_StrTipMsg);
			
			// ����ͼ��
			int32 iTabCount = m_GuiTabWnd.GetCount();
			m_GuiTabWnd.SetTabIcon(iTabCount - 1, 0);

			// �����ʼ������
//			pIoView->OnVDataForceUpdate();
//#ifdef _DEBUG
//			TRACE(_T("��ͼ[%s]OnVDataForceUpdate: %d ms\r\n"), StrIoViewXmlName.GetBuffer(), timeGetTime()-dwTmpTime);
//			dwTmpTime = timeGetTime();
//#endif
		}

		//
		pElementIoView = pElementIoView->NextSiblingElement();
	}
	//////////////////////////////////////////////
	// ȷ������ʾ�ĸ�ҵ����ͼ
	int32 iCountTab = m_GuiTabWnd.GetCount();
	int32 iCountView = m_IoViewsPtr.GetSize();
	if (iCountView != iCountTab)
		return FALSE;

	if (iCurView < 0 || iCurView >= iCountTab)
	{
		iCurView = 0;
	}

	for (i = 0; i < iCountView; i++)
	{
		CIoViewBase *pIoViewBase = m_IoViewsPtr[i];
		if (NULL == pIoViewBase)
			continue;

		if (iCurView == i)
		{
			pIoViewBase->ShowWindow(SW_SHOW);
			//pIoViewBase->OnVDataForceUpdate();	// ǿ��ˢ��
		}
		else
		{
			pIoViewBase->ShowWindow(SW_HIDE);			
		}
	}

	// �ı�tab����
	m_eViewTab = eViewTab;

	//
	m_GuiTabWnd.SetCurtab(iCurView);
	
	// XL0006 ���ò��Ҫ�ֶ����ã�������û�����notify��
	// OnTabSelChange();

	// ��XML�м����ˣ���Ҫ��ʼ��
	m_bFromXmlNeedInit = true;

#ifdef _DEBUG
	TRACE(_T("IoViewManager���FromXml: %d ms\r\n"), timeGetTime()-dwTime);
#endif

	return true;
}



bool32 CIoViewManager::FromXmlTopBtn( TiXmlElement *pTiXmlElement )
{
	if (NULL == pTiXmlElement)
		return false;


	// ��ǰѡ������ѡ��Ĳ���
	m_cManagerTopBarInfo.m_mapTopbarGroup.clear();
	TiXmlElement *pMyEle = pTiXmlElement->FirstChildElement(KStrElementAttrManagerTopBtn);
	if ( NULL != pMyEle )
	{
		// ��ȡ����ɫ�Ϳؼ�����
		const char *pcAttrValue = pMyEle->Attribute(KStrElementAttrManagerBarHight);
		if (pcAttrValue != NULL)
		{
			m_cManagerTopBarInfo.m_iBarHight  = atoi(pcAttrValue);
		}
		pcAttrValue = pMyEle->Attribute(KStrElementAttrManagerBarBkColor);
		if (pcAttrValue != NULL)
		{
			m_cManagerTopBarInfo.m_colBarBKColor  = StrToColorRef(pcAttrValue);
		}

		pcAttrValue = pMyEle->Attribute(KStrElementAttrtabbarbtnWidth);
		if (pcAttrValue != NULL)
		{
			m_cManagerTopBarInfo.m_iTopBarBtnWidth  = atoi(pcAttrValue);
		}

		pcAttrValue = pMyEle->Attribute(KStrElementAttrManagerBarType);
		if (pcAttrValue != NULL)
		{
			m_cManagerTopBarInfo.m_iTopBarType      = atoi(pcAttrValue);
		}

		pcAttrValue = pMyEle->Attribute(KStrElementAttrRightimagePath);
		if (pcAttrValue != NULL)
		{
			CString strImagePath = _A2W(pcAttrValue);
			m_cManagerTopBarInfo.m_pImageBk  = Image::FromFile(strImagePath);
			m_cManagerTopBarInfo.m_strImagePath = strImagePath;
		}

		int iID = 0;
		for(TiXmlNode *pNode = pMyEle->FirstChild(); pNode; pNode = pNode->NextSibling())
		{
			if (!pNode)
			{
				return false;
			}

			const char *pcValue = pNode->Value();
			if (NULL == pcValue || strcmp(KStrElementAttrtabbar, pcValue) != 0)
			{
				continue;
			}

			T_ManagerTopBar stManagerTopbar;

			TiXmlElement* pElement = (TiXmlElement*)pNode;
			const char *pcAttrValue = pElement->Attribute(KStrElementAttrname);
			if (pcAttrValue != NULL)
			{
				stManagerTopbar.m_StrShowName  = _A2W(pcAttrValue);
			}

			pcAttrValue = pElement->Attribute(KStrElementAttrbtnType);
			if (pcAttrValue != NULL)
			{
				stManagerTopbar.m_iBtnType  = atoi(pcAttrValue);
			}

			pcAttrValue = pElement->Attribute(KStrElementAttrmarketid);
			if (pcAttrValue != NULL)
			{
				stManagerTopbar.m_iMarketID  = atoi(pcAttrValue);
			}

			pcAttrValue = pElement->Attribute(KStrElementAttrMerchCode);
			if (pcAttrValue != NULL)
			{
				stManagerTopbar.m_strMerchCode  = _A2W(pcAttrValue);
			}


			pcAttrValue = pElement->Attribute(KStrElementAttrcfmname);
			if (pcAttrValue != NULL)
			{
				stManagerTopbar.m_strCfmName  = _A2W(pcAttrValue);
			}

			pcAttrValue = pElement->Attribute(KStrElementAttrShrinkSize);
			if (pcAttrValue != NULL)
			{
				stManagerTopbar.m_iShrinkSize  = atoi(pcAttrValue);
			}

			pcAttrValue = pElement->Attribute(KStrElementAttrisSelected);
			if (pcAttrValue != NULL)
			{
				stManagerTopbar.m_bSelected = atoi(pcAttrValue) == 1;
			}

			pcAttrValue = pElement->Attribute(KStrElementAttrAlignmente);
			if (pcAttrValue != NULL)
			{
				CString strAlignment  = _A2W(pcAttrValue);
				if (strAlignment.CompareNoCase(L"left") == 0)
				{
					stManagerTopbar.m_iAlignmentType = ALIGNMENT_LEFT;
				}
				if (strAlignment.CompareNoCase(L"right") == 0)
				{
					stManagerTopbar.m_iAlignmentType = ALIGNMENT_RIGHT;
				}
				if (strAlignment.CompareNoCase(L"center") == 0)
				{
					stManagerTopbar.m_iAlignmentType = ALIGNMENT_CENTER;
				}
			}

			//	�ؼ���ʾ����ɫ
			const char* pIndexNomarlBkColor	= pElement->Attribute(KIoViewChartIndexNomarlBkColor);
			if (NULL != pIndexNomarlBkColor)
			{						
				stManagerTopbar.m_colNoarml = StrToColorRef(pIndexNomarlBkColor);					
			}

			const char* pIndexPressBkColor	= pElement->Attribute(KIoViewChartIndexPressBkColor);
			if (NULL != pIndexPressBkColor)
			{						
				stManagerTopbar.m_colPress = StrToColorRef(pIndexPressBkColor);					
			}

			const char* pIndexHotBkColor	= pElement->Attribute(KIoViewChartIndexHotBkColor);
			if (NULL != pIndexHotBkColor)
			{						
				stManagerTopbar.m_colHot = StrToColorRef(pIndexHotBkColor);					
			}

			//	�ؼ��ı�ɫ
			const char* pIndexNomarTextColor	= pElement->Attribute(KIoViewChartIndexNomarlTextColor);
			if (NULL != pIndexNomarTextColor)
			{						
				stManagerTopbar.m_colTextNoarml = StrToColorRef(pIndexNomarTextColor);					
			}

			const char* pIndexPressTextColor	= pElement->Attribute(KIoViewChartIndexPressTextColor);
			if (NULL != pIndexPressTextColor)
			{						
				stManagerTopbar.m_colTextPress = StrToColorRef(pIndexPressTextColor);					
			}

			const char* pIndexHotTextColor	= pElement->Attribute(KIoViewChartIndexHotTextColor);
			if (NULL != pIndexHotTextColor)
			{						
				stManagerTopbar.m_colTextHot = StrToColorRef(pIndexHotTextColor);					
			}

			// �ؼ��߿�ɫ
			const char* pIndexFrameTextColor	= pElement->Attribute(KStrElementAttrframeColor);
			if (NULL != pIndexFrameTextColor)
			{						
				stManagerTopbar.m_colTextframe = StrToColorRef(pIndexFrameTextColor);					
			}
			
			stManagerTopbar.m_iBtnID = TOPBAR_BEGINID + iID++;
			m_cManagerTopBarInfo.m_mapTopbarGroup[stManagerTopbar.m_iBtnID] = stManagerTopbar;
			//// ָ��
			//for(TiXmlNode *pChild = pElement->FirstChild(); pChild; pChild	 = pChild->NextSibling())
			//{
		}
	}

	return true;
}


void CIoViewManager::InitTopBtn()
{
	m_cManagerTopBarInfo.m_mapTopbarBtnLeft.clear();
	m_cManagerTopBarInfo.m_mapTopbarBtnRight.clear();
	m_cManagerTopBarInfo.m_mapTopbarBtnCenter.clear();
	COLORREF colorBk = m_cManagerTopBarInfo.m_colBarBKColor;
	map<int32, T_ManagerTopBar>::iterator iter;
	for (iter = m_cManagerTopBarInfo.m_mapTopbarGroup.begin(); iter != m_cManagerTopBarInfo.m_mapTopbarGroup.end(); iter++)
	{
		T_ManagerTopBar &topTabbar = iter->second;
		CNCButton btnControl;
		CRect rcRect(0,0,0,0);
		btnControl.SetTextColor(topTabbar.m_colTextNoarml, topTabbar.m_colTextHot, topTabbar.m_colTextPress);
		btnControl.SetTextBkgColor(topTabbar.m_colNoarml, topTabbar.m_colHot, topTabbar.m_colPress);
		btnControl.SetTextFrameColor(topTabbar.m_colTextframe, topTabbar.m_colTextframe, topTabbar.m_colTextframe);

		CNCButton::T_NcFont m_Font;
		m_Font.m_StrName = (L"����");
		m_Font.m_Size	 = 10.5;
		m_Font.m_iStyle	 = FontStyleRegular;	
		btnControl.SetFont(m_Font);
		btnControl.SetBtnBkgColor(colorBk);
		
		if (ALIGNMENT_LEFT == topTabbar.m_iAlignmentType)
		{
			btnControl.CreateButton(topTabbar.m_StrShowName, rcRect, this, NULL, 1, topTabbar.m_iBtnID);
			// ����ѡ��
			if (topTabbar.m_bSelected)
			{
				btnControl.SetCheck(TRUE);
				m_LastPressTopBtnId = btnControl.GetControlId();
			}
			m_cManagerTopBarInfo.m_mapTopbarBtnLeft[topTabbar.m_iBtnID] = btnControl;
		}
		else if (ALIGNMENT_RIGHT == topTabbar.m_iAlignmentType)
		{
			btnControl.CreateButton(topTabbar.m_StrShowName, rcRect, this, m_cManagerTopBarInfo.m_pImageBk, 3, topTabbar.m_iBtnID);
			// ����ѡ��
			if (topTabbar.m_bSelected)
			{
				btnControl.SetCheck(TRUE);
				m_LastPressTopBtnId = btnControl.GetControlId();
			}
			m_cManagerTopBarInfo.m_mapTopbarBtnRight[topTabbar.m_iBtnID] = btnControl;
		}
		else if (ALIGNMENT_CENTER == topTabbar.m_iAlignmentType)
		{
			btnControl.CreateButton(topTabbar.m_StrShowName, rcRect, this, NULL, 1, topTabbar.m_iBtnID);
			// ����ѡ��
			if (topTabbar.m_bSelected)
			{
				btnControl.SetCheck(TRUE);
				m_LastPressTopBtnId = btnControl.GetControlId();
			}
			m_cManagerTopBarInfo.m_mapTopbarBtnCenter[topTabbar.m_iBtnID] = btnControl;
		}
		else
		{
			//.....
		}

	}
}


void CIoViewManager::GetIoViewIDs(IN E_IoViewGroupType eGroupType,OUT CArray<int32,int32>& aID)
{
	aID.RemoveAll();
	
	for ( int32 i = 0 ; i < KIoViewObjectCount; i++)
	{
		if ( eGroupType == s_astIoViewObjects[i].m_eGroupType)
		{	
			aID.Add(s_astIoViewObjects[i].m_uID);						
		}
	}
}

const T_IoViewObject* CIoViewManager::FindIoViewObjectByXmlName(const CString &StrXmlName)
{
	for (int32 i = 0; i < KIoViewObjectCount; i++)
	{
		if (s_astIoViewObjects[i].m_StrXmlName == StrXmlName)
		{
			return &s_astIoViewObjects[i];
		}
	}

	return NULL;
}

const T_IoViewObject* CIoViewManager::FindIoViewObjectByIoViewPtr(const CIoViewBase *pIoView)
{
	if (NULL == pIoView)
		return NULL;

	int32 i = 0;
	for ( i = 0; i < KIoViewObjectCount; i++)
	{
		if (pIoView->GetRuntimeClass() == (s_astIoViewObjects[i].m_pIoViewClass))	// �ϸ����
		{
			return &s_astIoViewObjects[i];
		}
	}

	for (i = 0; i < KIoViewObjectCount; i++)
	{
		if (pIoView->IsKindOf(s_astIoViewObjects[i].m_pIoViewClass))
		{
			return &s_astIoViewObjects[i];
		}
	}

	return NULL;
}

const T_IoViewObject* CIoViewManager::FindIoViewObjectByRuntimeClass(CRuntimeClass * pRunTimeClass)
{
	if ( NULL == pRunTimeClass)
	{
		return NULL;
	}
	for ( int32 i = 0 ; i < KIoViewObjectCount ; i++)
	{
		if ( s_astIoViewObjects[i].m_pIoViewClass == pRunTimeClass)
		{
			return &s_astIoViewObjects[i];
		}
	}
	
	return NULL;
}


const T_IoViewObject* CIoViewManager::FindIoViewObjectByPicMenuID( UINT nMenuID )
{
	for ( int32 i = 0 ; i < KIoViewObjectCount ; i++)
	{
		if ( s_astIoViewObjects[i].m_uID == nMenuID)
		{
			return &s_astIoViewObjects[i];
		}
	}
	
	return NULL;
}


const T_IoViewObject* CIoViewManager::GetIoViewObject(int32 iIndex)
{
	ASSERT(iIndex>=0 && iIndex< KIoViewObjectCount );

	return &s_astIoViewObjects[iIndex];
}

const int32 CIoViewManager::GetIoViewObjectCount()
{
	return KIoViewObjectCount;
}

const T_IoViewObject* CIoViewManager::GetIoViewObject()
{
	return s_astIoViewObjects;
}

const T_IoViewGroupObject* CIoViewManager::FindIoViewGroupObjectByLongName(const CString& StrLongName)
{
	for ( int32 i = 0 ; i < KIoViewGroupObjectCount; i++)
	{
		if ( StrLongName == s_astIoViewGroupObjects[i].m_StrLongName)
		{
			return &s_astIoViewGroupObjects[i];
		}
	}

	return NULL;
}

const T_IoViewGroupObject* CIoViewManager::GetIoViewGroupObject(int32 iIndex)
{
	if ( iIndex < 0 || iIndex >= KIoViewGroupObjectCount )
	{
		return NULL;
	}
	return &s_astIoViewGroupObjects[iIndex];
}

const int32 CIoViewManager::GetIoViewGroupObjectCount()
{
	return KIoViewGroupObjectCount;
}

CString	CIoViewManager::GetDefaultXML(int32 iGroupID,bool32 bBlank, CIoViewBase *pIoView)
{
	CString StrDefaultXML;
 
	CString StrViewTab  = L"none"; 
	CString	StrCurView  = L"0";
	CString	StrAutoHide = L"0";
	CString StrGroupID;

	StrGroupID.Format(L"%d",iGroupID);
 	StrDefaultXML.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\">\n",
		CString(KStrElementValue).GetBuffer(),
		CString(KStrElementAttrViewTab).GetBuffer(),
		StrViewTab.GetBuffer(),
		CString(KStrElementAttrCurView).GetBuffer(),
		StrCurView.GetBuffer(),
		CString(KStrElementAttrAutoHide).GetBuffer(),
		StrAutoHide.GetBuffer(),
		CString(KStrElementAttrGroupId).GetBuffer(),
		StrGroupID.GetBuffer());
	
	// Ĭ��ҵ����ͼ
	if (!bBlank)
	{
		if (NULL != pIoView)
		{
			StrDefaultXML += pIoView->GetDefaultXML();
		}
		else
		{
			CIoViewBase *pIoViewDefault = (CIoViewBase *)m_pDefaultIoViewClass->CreateObject();
			const T_IoViewObject * pstIoViewObject = FindIoViewObjectByIoViewPtr(pIoViewDefault);
			ASSERT(NULL != pstIoViewObject);
			
			{
				StrDefaultXML += pIoViewDefault->GetDefaultXML();
			}

			DEL(pIoViewDefault);
		}
 	}

	StrDefaultXML += L"</";
	StrDefaultXML += KStrElementValue;
	StrDefaultXML += L">\n";

	return StrDefaultXML;
}

const char * CIoViewManager::GetXmlElementValue()
{
	return KStrElementValue;
}

CIoViewBase* CIoViewManager::GetActiveIoView()
{
	// �޸�Ϊ���յ�ǰѡ��ķ���active view�������ƥ�䣬�ڰ���ԭ���ķ��� xl
	int32 iSel = m_GuiTabWnd.GetCurtab();
	if ( iSel >=0 && m_IoViewsPtr.GetSize() > iSel  && m_IoViewsPtr.GetSize() == m_GuiTabWnd.GetCount() )
	{
		return m_IoViewsPtr[iSel];
	}

	for (int32 i = 0; i < m_IoViewsPtr.GetSize(); i++)
	{
		CIoViewBase *pIoView = m_IoViewsPtr[i];
		if (NULL == pIoView)
			continue;

		if (pIoView->IsWindowVisible())
		{
			return pIoView;
		}
	}

	return NULL;
}

void CIoViewManager::OnIoViewManagerActive()
{
	// ������Ϣ����ǰ�ɼ���ҵ����ͼ
	CIoViewBase *pIoView = GetActiveIoView();
	if (NULL != pIoView)
	{
	    pIoView->OnIoViewActive();
	}
// 	if (GetFocus() != pIoView)
// 	{
// 		SetFocus();
// 	}

	// xl  TBWndע��ioview����
	RegisterActiveIoViewToMainFrame(true);
}

void CIoViewManager::OnIoViewManagerDeactive()
{
	// ������Ϣ����ǰ�ɼ���ҵ����ͼ
	for (int32 i = 0; i < m_IoViewsPtr.GetSize(); i++)
	{
		CIoViewBase *pIoView = m_IoViewsPtr[i];
		if (NULL == pIoView)
			continue;

		if (pIoView->IsWindowVisible() || i == m_GuiTabWnd.GetCurtab())	// ��֤��ǰ���������յ���Ϣ
		{
			pIoView->OnIoViewDeactive();
			break;
		}
	}

	RegisterActiveIoViewToMainFrame(false);
}

int32 CIoViewManager::GetGroupID()
{
	return m_iGroupId;
}

void CIoViewManager::SetGroupID(int32 iID)
{
	ASSERT(iID>=0 && iID <=MAXGROUPID);
	if (iID != m_iGroupId)
	{
		m_iGroupId = iID;
		OnViewGroupIdChanged(m_iGroupId);
	}
}

void CIoViewManager::OnViewGroupIdChanged(int32 iNewGroupId)
{	
	// ����Ϣ�����IoViewManager �����е�ҵ����ͼ
	for (int32 i = 0; i<m_IoViewsPtr.GetSize(); i++)
	{
		CIoViewBase * pIoView = m_IoViewsPtr[i];
		if (pIoView)
		{
			pIoView->OnIoViewGroupIdChange(iNewGroupId);
		}
	}
}

void CIoViewManager::OnRButtonDown2(CPoint pt, int32 iTab)
{
	return;

	CNewMenu menu;
	menu.CreatePopupMenu();
//	menu.LoadToolBar(g_awToolBarIconIDs);

// 	if ( m_IoViewsPtr.GetSize() > 1 )
// 	{
// 		menu.AppendODMenu(L"ɾ����ͼ",MF_STRING,ID_IOVIEWTABLE_DEL);
// 	}

	CNewMenu * pSubMenuView = menu.AppendODPopupMenu(L"����ҵ����ͼ");
	
	int32 iMenuCounts = CIoViewManager::GetIoViewObjectCount();
	
	for ( int32 i=0 ; i<iMenuCounts ;i++)
	{
		CString MenuName = CIoViewManager::GetIoViewObject(i)->m_StrLongName;
		pSubMenuView->AppendODMenu(MenuName,MF_STRING,ID_PIC_BEGIN + i);
	}		
	int32 iCurTab = m_GuiTabWnd.GetCurtab();
	CIoViewBase * pIoView = this->m_IoViewsPtr[iCurTab];
	if (pIoView->IsKindOf(RUNTIME_CLASS(CIoViewReport)))
	{
		CNewMenu * pSubMenuBlock = menu.AppendODPopupMenu(L"���Ӱ����ͼ");
		
		CArray<T_BlockMenuInfo ,T_BlockMenuInfo&> aBlockMenuInfo; 
		CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
		pMainFrame->GetBlockMenuInfo(aBlockMenuInfo);
		CNewMenu * pMenuMerch = NULL;
		
		for (int32 i = 0; i<aBlockMenuInfo.GetSize(); i++)
		{			
			if (aBlockMenuInfo[i].iBlockMenuIDs == 0)
			{
				pMenuMerch = pSubMenuBlock->AppendODPopupMenu(aBlockMenuInfo[i].StrBlockMenuNames);
			}

			CString StrBreedName = aBlockMenuInfo[i].StrBlockMenuNames;
			if (StrBreedName == L"���֤ȯ" || StrBreedName == L"�¼����ڻ�" || StrBreedName == L"ȫ��ָ��")
			{
				pSubMenuBlock->AppendODMenu(L"",MF_SEPARATOR);
			}

			if(pMenuMerch)
			{
				pMenuMerch->AppendODMenu(aBlockMenuInfo[i].StrBlockMenuNames,MF_STRING,aBlockMenuInfo[i].iBlockMenuIDs);
			}
		}	
	}	
	m_GuiTabWnd.ClientToScreen(&pt);
	menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, AfxGetMainWnd());	
}

bool32 CIoViewManager::BeTestValid()
{
	int32 iCurTab = m_GuiTabWnd.GetCurtab();

	for ( int32 i = 0 ; i < m_IoViewsPtr.GetSize(); i++ )
	{
		CIoViewBase* pIoView = m_IoViewsPtr[i];

		if ( NULL == pIoView )
		{
			continue;
		}

		if ( i != iCurTab && pIoView->IsWindowVisible() )
		{
			return false;
		}
	}

	return true;
}


void CIoViewManager::RegisterActiveIoViewToMainFrame(bool32 bRegister)
{
	CWnd *pMain = AfxGetMainWnd();
	if ( IsWindow(pMain->GetSafeHwnd()) && pMain->IsKindOf(RUNTIME_CLASS(CMainFrame)) )
	{
		CMainFrame *pMainFrame = (CMainFrame *)pMain;
		CIoViewBase *pView = GetActiveIoView();
		// ������ص���NULL����ע��
		if ( bRegister )
		{
			pMainFrame->RegisterAcitveIoView(pView, IsWindow(pView->GetSafeHwnd()));
		}
		else
		{
			pMainFrame->RegisterAcitveIoView(NULL, false);
		}
	}
}

void CIoViewManager::DealF5Key()
{
	// ���ڵ�ǰmanager���Ҵ��ڵĶ�Ӧͼ�꣬������ڣ����ǵ�ǰҳ��ģ�����mainframe
	// ֻ���ڵ�ǰ��ͼ��ͼ�������²ų���
	CIoViewBase *pIoViewActive = GetActiveIoView();
	if(NULL == pIoViewActive)
	{
		return;
	}
	CMerch *pMerchXml = pIoViewActive->GetMerchXml();

	if ( NULL == pMerchXml )
	{
		return;	// ����Ʒ���ô�����
	}

	if ( pIoViewActive->IsKindOf(RUNTIME_CLASS(CIoViewKLine)) )
	{
		// K���ҷ�ʱ
		CIoViewBase *pTrend = FindIoViewByPicMenu(ID_PIC_TREND, false);
		if ( NULL != pTrend )
		{
			pTrend->BringToTop();
			return;	// �����л���Ʒ��
		}
	}
	else if ( pIoViewActive->IsKindOf(RUNTIME_CLASS(CIoViewTrend)) )
	{
		// ��ʱ��K
		CIoViewBase *pKline = FindIoViewByPicMenu(ID_PIC_KLINE, false);
		if ( NULL != pKline )
		{
			pKline->BringToTop();
			return;
		}
	}

	// �������mainframe����

	// F5����:  ����mainframe
	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if ( NULL != pMainFrame )
	{
		pMainFrame->OnShowMerchInNextChart(pIoViewActive);
	}
}

CIoViewBase* CIoViewManager::FindIoViewByPicMenu( UINT nID, bool32 bCreateWhenNotExist/*=false*/ )
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	if ( NULL == pMainFrame )
	{
		return NULL;
	}
	
	const T_IoViewObject *pIoObj = FindIoViewObjectByPicMenuID(nID);
	if ( NULL == pIoObj )
	{
		return NULL;
	}

	for ( int32 i=0; i < m_IoViewsPtr.GetSize() ; i++ )
	{
		if ( IsWindow(m_IoViewsPtr[i]->GetSafeHwnd()) && m_IoViewsPtr[i]->GetRuntimeClass() == pIoObj->m_pIoViewClass )
		{
			return m_IoViewsPtr[i];
		}
	}
	
	if ( bCreateWhenNotExist )
	{
		return pMainFrame->CreateIoViewByPicMenuID(nID, false, this);
	}

	return NULL;
}

void CIoViewManager::ChangeGroupId( int32 iNewId, bool32 bChangeMerch /*= false*/ )
{
	if ( !bChangeMerch )
	{
		SetGroupID(iNewId);
	}
	else
	{
		ASSERT(iNewId>=0 && iNewId <=MAXGROUPID);
		if (iNewId != m_iGroupId)
		{
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			CMerch *pMerch = pFrame->GetIoViewGroupMerch(iNewId);
			for ( int i=0; i < m_IoViewsPtr.GetSize() && NULL != pMerch ; i++ )
			{
				CIoViewBase *pIoView = m_IoViewsPtr[i];
				if ( IsWindow(pIoView->GetSafeHwnd()) )
				{
					pIoView->OnVDataMerchChanged(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, pMerch);
				}
			}
		}
	}
}

void CIoViewManager::BeyondShowIoView(bool bBeyond, CIoViewBase* pIoView, bool32 bForceShow /*= false*/)
{
	CGGTongView* pGGTongView = pIoView->GetParentGGtongView();
	if ( NULL == pGGTongView )
	{
		return;
	}

	// �����зִ��ڵ�, ���ô�����
	CWnd* pParent = pGGTongView->GetParent();
	if ( !pParent->IsKindOf(RUNTIME_CLASS(CBiSplitterWnd)) )
	{
		return;
	}

	CBiSplitterWnd* pParentSplit = (CBiSplitterWnd*)pParent;

	if ( bBeyond )
	{
		const T_IoViewObject* pIoViewObj = FindIoViewObjectByIoViewPtr(pIoView);
		if ( NULL == pIoViewObj )
		{
			return;
		}

		// �Ƿ���Ҫ����, �����ͼ����ռ���ͺ͵�ǰ���з����Ͳ�ͬ�Ļ��Ͳ��ô�����
		bool32 bNeedDel = false;

		//
		E_IoViewBeyondShow eBeyondShow = pIoViewObj->m_eBeyondShow;
		bool32 bVertical = pParentSplit->BeVSplit();
		
		if ( EIBSBoth == eBeyondShow )
		{
			bNeedDel = true;
		}
		else if ( (EIBSVertical == eBeyondShow) && (bVertical) )
		{
			bNeedDel = true;
		}
		else if ( (EIBSHorizontal == eBeyondShow) && (!bVertical) )
		{
			bNeedDel = true;
		}
		else
		{
			// ���ô�����
			return;
		}

		//		
		if( m_bBeyondShow && !bForceShow )
		{
			// ��һ��Ҳ��������ͼ, ���ù�
			return;
		}
		else
		{
			// ������һ��	
			// �����ǰggtongView�Ѿ���F7״̬������Ҫ�����κδ�����
			CMPIChildFrame *pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
			if ( NULL == pFrame || !pFrame->IsF7AutoLock() )
			{
				int32 iID =	pParentSplit->GetViewPaneID(pGGTongView);
				pParentSplit->HidePane(1 - iID);	
			}
			m_bBeyondShow = true;		// ���õ�����־���Ա�֤F7��ԭʱ�ܹ��ٴε���
		}		
	}
	else
	{
		if ( !m_bBeyondShow && !bForceShow )
		{
			// ԭ���Ͳ���, ���ù�
			return;
		}
		else
		{
			// ��ʾԭ������ͼ
			// ���������F7״̬������Ҫ������λ�ԭ���ȵ��û�F7��ԭʱ����ʾԭ�������ز���
			CMPIChildFrame *pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
			if ( NULL == pFrame || !pFrame->IsF7AutoLock() )
			{
				int32 iID =	pParentSplit->GetViewPaneID(pGGTongView);
				pParentSplit->ShowPane(1 - iID);					
			}
			m_bBeyondShow = false;
		}
	}
}

void CIoViewManager::ForceBeyondShowIoView(CIoViewBase* pIoView /* = NULL */)
{
	if ( NULL == pIoView )
	{
		// Ĭ���ǵ�ǰ��ʾ����ͼ
		int32 iCurTab = m_GuiTabWnd.GetCurtab();
			
		if ( iCurTab < 0 || iCurTab >= m_IoViewsPtr.GetSize() )
		{
			return;
		}
		
		//
		pIoView = m_IoViewsPtr[iCurTab];

		if ( NULL == pIoView )
		{
			return;
		}

		//
		const T_IoViewObject* pIoViewObj = FindIoViewObjectByIoViewPtr(pIoView);
		if ( NULL == pIoViewObj )
		{
			return;
		}

		if ( EIBSNone == pIoViewObj->m_eBeyondShow )
		{
			return;
		}
	}

	//
	BeyondShowIoView(true, pIoView, true);
}

void CIoViewManager::OnTimer(UINT nID)
{	
	if ( nID == 1 )
	{
		KillTimer(1);
		m_bDelIoView = false;
		ForceBeyondShowIoView();
	}
}

void CIoViewManager::RemoveAllIoView()
{
	for ( int32 i = m_IoViewsPtr.GetSize() - 1; i >= 0; i-- )
	{
		CIoViewBase* pIoView = m_IoViewsPtr.GetAt(i);
		if ( NULL == pIoView )
		{
			continue;
		}

		pIoView->DestroyWindow();
		DEL(pIoView);
	}

	//
	m_IoViewsPtr.RemoveAll();
	m_GuiTabWnd.DeleteAll();
	
	//
	CGGTongView* pParent = (CGGTongView*)GetParent();
	if ( NULL != pParent )
	{
		CRect RectParent;
		pParent->GetClientRect(&RectParent);
		
		int32 iHeight = RectParent.Height();
		int32 iWidth  = RectParent.Width();
		
		LPARAM lpRarm = MAKELPARAM(iWidth, iHeight);
		
		pParent->PostMessage(WM_SIZE, 0, lpRarm);
	}
}

void CIoViewManager::DrawNoChartRgn()
{
	//	���ƶ������������ײ����������߿��ߣ��Ľ�Բ��
	COLORREF clrEllipse = RGB(214,57,61);
	COLORREF clrBorder = RGB(77,79,84);	
	CRect rcClient, rcBorder;
	GetClientRect(&rcClient);
	rcBorder = rcClient;
	//	�ü��������ǽ�����Ҫ��ChartRgn�����򼴿ɣ�
	CRgn rgnWhole, rgnChart, rgnNoChart;
	if (m_GuiTabWnd.IsWindowVisible())
	{
		CRect rtTab;
		m_GuiTabWnd.GetClientRect(&rtTab);
		switch(m_eViewTab)
		{
		case EVTTop:
			rcClient.top += rtTab.Height() + TAB_SPLITER_WIDTH;	
			break;
		case EVTRight:
			rcClient.right -= rtTab.Width();	
			break;
		case EVTLeft:
			rcClient.left += rtTab.Width();	
			break;
		case EVTBottom:
			rcClient.bottom -= rtTab.Height() + TAB_SPLITER_WIDTH;	
			break;			
		default:
			ASSERT(0);
			break;
		}
			
	}
	rgnWhole.CreateRectRgn(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);	
	rcBorder.DeflateRect(KiChartBorderLeft, KiChartBorderTop , KiChartBorderRight, KiChartBorderBottom );
	rgnChart.CreateRectRgn(rcClient.left + KiChartBorderLeft, rcClient.top+ KiChartBorderTop , rcClient.right - KiChartBorderRight , rcClient.bottom - KiChartBorderBottom );
	rgnNoChart.CreateRectRgn(0,0,0,0);
	rgnNoChart.CombineRgn(&rgnWhole, &rgnChart, RGN_DIFF);

	CClientDC dc(this);	
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
	memDC.SelectObject(&bmp);
	memDC.SetBkMode(TRANSPARENT);
	//	���Ʊ���ɫ
	memDC.FillSolidRect(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), RGB(33, 36, 41));
	//	���Ʊ߿��ߣ��Ľ�Բ��
	{
		CPen pen(PS_SOLID, 2, clrBorder);
		HGDIOBJ hOldPen = memDC.SelectObject(&pen);

		int iCorner = 10;	//	�ĽǾ���x,y���ĵ�����ݳ���
		//	�߿���λ��
		CRect rtBorderInside(rcBorder);
		rtBorderInside.InflateRect(KiChartBorderLeft/4 * 3, KiChartBorderTop/4 * 3, KiChartBorderRight/4 * 3, KiChartBorderBottom/4 * 3);		

		//	���ϣ�����x,����y
		CPoint ptLeftTop(rtBorderInside.left, rtBorderInside.top);
		CPoint ptLeftTopX(ptLeftTop);
		ptLeftTopX.x += iCorner;
		CPoint ptLeftTopY(ptLeftTop);
		ptLeftTopY.y += iCorner;

		//	���ϣ�����x, ����y
		CPoint ptRightTop(rtBorderInside.right, rtBorderInside.top);
		CPoint ptRightTopX(ptRightTop);
		ptRightTopX.x -= iCorner;
		CPoint ptRightTopY(ptRightTop);
		ptRightTopY.y += iCorner;

		//	���£�����x,����y
		CPoint ptLeftBottom(rtBorderInside.left, rtBorderInside.bottom);
		CPoint ptLeftBottomX(ptLeftBottom);
		ptLeftBottomX.x += iCorner;
		CPoint ptLeftBottomY(ptLeftBottom);
		ptLeftBottomY.y -= iCorner;

		//	���£�����x�� ����y
		CPoint ptRightBottom(rtBorderInside.right, rtBorderInside.bottom);
		CPoint ptRightBottomX(ptRightBottom);
		ptRightBottomX.x -= iCorner;
		CPoint ptRightBottomY(ptRightBottom);
		ptRightBottomY.y -= iCorner;
		//	������
		memDC.MoveTo(ptLeftTopX);
		memDC.LineTo(ptRightTopX);
		//	������б��
		memDC.MoveTo(ptLeftTopX);
		memDC.LineTo(ptLeftTopY);
		//	������б��
		memDC.MoveTo(ptRightTopX);
		memDC.LineTo(ptRightTopY);
		////	�����
		memDC.MoveTo(ptLeftTopY);
		memDC.LineTo(ptLeftBottomY);
		////	������б��
		memDC.MoveTo(ptLeftBottomX);
		memDC.LineTo(ptLeftBottomY);
		////	���ױ�
		memDC.MoveTo(ptLeftBottomX);
		memDC.LineTo(ptRightBottomX);
		////	������б��
		memDC.MoveTo(ptRightBottomX);
		memDC.LineTo(ptRightBottomY);
		////	���ұ�
		memDC.MoveTo(ptRightTopY);
		memDC.LineTo(ptRightBottomY);

		//	���Ľ�Բ��
		int iEllipse = 6;
		CBrush br(clrEllipse);
		CPen   ellipsePen(PS_SOLID, 1, clrEllipse);
		HGDIOBJ hOldBr = memDC.SelectObject(&br);
		memDC.SelectObject(&ellipsePen);
		//	������Բ		
		CRect rtTopLeftEllipse(ptLeftTop.x - iEllipse/2, ptLeftTop.y - iEllipse/2, ptLeftTop.x + iEllipse/2, ptLeftTop.y + iEllipse/2);
		memDC.Ellipse(rtTopLeftEllipse);
		//	������Բ
		CRect rtRightTopEllipse(ptRightTop.x - iEllipse/2, ptRightTop.y - iEllipse/2, ptRightTop.x + iEllipse/2, ptRightTop.y + iEllipse/2);
		memDC.Ellipse(rtRightTopEllipse);
		//	������Բ
		CRect rtLeftBottomEllipse(ptLeftBottom.x - iEllipse/2, ptLeftBottom.y - iEllipse/2, ptLeftBottom.x + iEllipse/2, ptLeftBottom.y + iEllipse/2);
		memDC.Ellipse(rtLeftBottomEllipse);
		//	������Բ
		CRect rtRightBottomEllipse(ptRightBottom.x - iEllipse/2, ptRightBottom.y - iEllipse/2, ptRightBottom.x + iEllipse/2, ptRightBottom.y + iEllipse/2);
		memDC.Ellipse(rtRightBottomEllipse);

		memDC.SelectObject(hOldBr);
		memDC.SelectObject(hOldPen);

	}	

	CRgn rgnOld;
	rgnOld.CreateRectRgn(0,0,0,0);	
	BOOL bRgnOld = GetClipRgn(dc.GetSafeHdc(), (HRGN)rgnOld.m_hObject) > 0;
	dc.SelectClipRgn(&rgnNoChart);
	dc.BitBlt(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), &memDC, rcClient.left, rcClient.top, SRCCOPY);
	if (!bRgnOld)
	{
		dc.SelectClipRgn(NULL);
	}
	else
	{
		dc.SelectClipRgn(&rgnOld);
	}

	memDC.DeleteDC();
	bmp.DeleteObject();
}




/************************************************************************/
/* �������ƣ�TButtonHitTest                                             */
/* ������������ȡ��ǰ������ڵİ�ť										*/
/* ���������point,��ǰ���λ��											*/
/* ����ֵ��  ��ťID														*/
/************************************************************************/
int CIoViewManager::TButtonHitTest(CPoint point)
{
	map<int32, CNCButton>::iterator iter;
	int iBtnId = INVALID_ID;
	// ���������������а�ť
	for (iter=m_cManagerTopBarInfo.m_mapTopbarBtnLeft.begin(); iter!=m_cManagerTopBarInfo.m_mapTopbarBtnLeft.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		iBtnId   = btnControl.GetControlId();
		// ��point�Ƿ����ѻ��Ƶİ�ť������
		if (btnControl.PtInButton(point) && btnControl.GetCreate())
		{
			return  iBtnId;
		}
	}

	for (iter=m_cManagerTopBarInfo.m_mapTopbarBtnRight.begin(); iter!=m_cManagerTopBarInfo.m_mapTopbarBtnRight.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		iBtnId   = btnControl.GetControlId();
		// ��point�Ƿ����ѻ��Ƶİ�ť������
		if (btnControl.PtInButton(point) && btnControl.GetCreate())
		{
			return  iBtnId;
		}
	}

	for (iter=m_cManagerTopBarInfo.m_mapTopbarBtnCenter.begin(); iter!=m_cManagerTopBarInfo.m_mapTopbarBtnCenter.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		iBtnId   = btnControl.GetControlId();
		// ��point�Ƿ����ѻ��Ƶİ�ť������
		if (btnControl.PtInButton(point) && btnControl.GetCreate())
		{
			return  iBtnId;
		}
	}

	return INVALID_ID;
}



void CIoViewManager::SetCheckStatus( int iID )
{
	map<int32, CNCButton>::iterator iter;
	for (iter=m_cManagerTopBarInfo.m_mapTopbarBtnLeft.begin(); iter!=m_cManagerTopBarInfo.m_mapTopbarBtnLeft.end(); ++iter)
	{
		CNCButton &btn = iter->second;

		if(iID == iter->first )
		{
			if (!iter->second.GetCheck())
			{
				m_cManagerTopBarInfo.m_mapTopbarBtnLeft[iID].SetCheckStatus(TRUE);
			}
		}
		else if (btn.GetCheck())
		{
			btn.SetCheckStatus(FALSE);
		}
	}

	for (iter=m_cManagerTopBarInfo.m_mapTopbarBtnRight.begin(); iter!=m_cManagerTopBarInfo.m_mapTopbarBtnRight.end(); ++iter)
	{
		CNCButton &btn = iter->second;

		if(iID == iter->first )
		{
			if (!iter->second.GetCheck())
			{
				m_cManagerTopBarInfo.m_mapTopbarBtnRight[iID].SetCheckStatus(FALSE);
			}
		}
		else if (btn.GetCheck())
		{
			btn.SetCheckStatus(FALSE);
		}
	}


	for (iter=m_cManagerTopBarInfo.m_mapTopbarBtnCenter.begin(); iter!=m_cManagerTopBarInfo.m_mapTopbarBtnCenter.end(); ++iter)
	{
		CNCButton &btn = iter->second;

		if(iID == iter->first )
		{
			if (!iter->second.GetCheck())
			{
				m_cManagerTopBarInfo.m_mapTopbarBtnCenter[iID].SetCheckStatus(TRUE);
			}
		}
		else if (btn.GetCheck())
		{
			btn.SetCheckStatus(FALSE);
		}
	}
}


void CIoViewManager::RestTopBtnStatus(int32 ibtnControlId)
{
	SetCheckStatus(ibtnControlId);
}

void CIoViewManager::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	int iButton = -1;
	iButton = TButtonHitTest(point);
	if (INVALID_ID != iButton)
	{
		CNCButton *pNcButton = NULL;
		if (m_cManagerTopBarInfo.m_mapTopbarBtnLeft.count(iButton) > 0)
		{
			pNcButton = &m_cManagerTopBarInfo.m_mapTopbarBtnLeft[iButton];
			m_LastPressTopBtnId = iButton;
		}
		else if (m_cManagerTopBarInfo.m_mapTopbarBtnRight.count(iButton) > 0)
		{
			pNcButton = &m_cManagerTopBarInfo.m_mapTopbarBtnRight[iButton];
		}
		else if (m_cManagerTopBarInfo.m_mapTopbarBtnCenter.count(iButton) > 0)
		{
			pNcButton = &m_cManagerTopBarInfo.m_mapTopbarBtnCenter[iButton];
			m_LastPressTopBtnId = iButton;
		}

		if (!pNcButton)
		{
			return;
		}

		bool32 bCheck = pNcButton->GetCheck();
		if (!bCheck)
		{
			pNcButton->SetCheck(TRUE);
		}
		SetCheckStatus(iButton);
	}
	__super::OnLButtonDown(nFlags, point);
}

void CIoViewManager::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	int iButton = -1;
	iButton = TButtonHitTest(point);
	if (INVALID_ID != iButton)
	{
		CNCButton *pNcButton = NULL;
		if (m_cManagerTopBarInfo.m_mapTopbarBtnLeft.count(iButton) > 0)
		{
			pNcButton = &m_cManagerTopBarInfo.m_mapTopbarBtnLeft[iButton];
		}
		else if (m_cManagerTopBarInfo.m_mapTopbarBtnRight.count(iButton) > 0)
		{
			pNcButton = &m_cManagerTopBarInfo.m_mapTopbarBtnRight[iButton];
		}
		else if (m_cManagerTopBarInfo.m_mapTopbarBtnCenter.count(iButton) > 0)
		{
			pNcButton = &m_cManagerTopBarInfo.m_mapTopbarBtnCenter[iButton];
		}

		if (!pNcButton)
		{
			return;
		}
		pNcButton->LButtonUp();
	}

	__super::OnLButtonUp(nFlags, point);
}

BOOL CIoViewManager::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	
	if (m_cManagerTopBarInfo.m_mapTopbarGroup.size() > 0)
	{
		int32 iID = (int32)wParam;
		CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
		if (!pMain || !pMain->m_pNewWndTB)
		{
			return FALSE;
		}

		if (m_cManagerTopBarInfo.m_mapTopbarGroup.count(iID) > 0)
		{
			T_ManagerTopBar &ManagerTopBar = m_cManagerTopBarInfo.m_mapTopbarGroup[iID];
			switch(ManagerTopBar.m_iBtnType)
			{
			case MANAGERBAR_CHANGE_MARKET:
				if (ManagerTopBar.m_iMarketID != -1 && !ManagerTopBar.m_strMerchCode.IsEmpty())
				{
					pMain->OnHotKeyMerch(ManagerTopBar.m_iMarketID, ManagerTopBar.m_strMerchCode);
				}
				break;
			case MANAGERBAR_OPEN_CFM:
				if (!ManagerTopBar.m_strCfmName.IsEmpty())
				{
					pMain->m_pNewWndTB->DoOpenCfm(ManagerTopBar.m_strCfmName);
				}
				break;
			default:
				break;
			}
		}
	}
	

	return __super::OnCommand(wParam, lParam);
}

void	CIoViewManager::ForceSelectManagerTop(const CMerch* pMerch)
{
	if (NULL == pMerch)
	{
		return;
	}

	//	������ť����Ϊ0
	if (0 == m_cManagerTopBarInfo.m_mapTopbarGroup.size())
	{
		return;
	}

	map<int32, T_ManagerTopBar>::iterator it = m_cManagerTopBarInfo.m_mapTopbarGroup.begin();
	for (; it != m_cManagerTopBarInfo.m_mapTopbarGroup.end(); ++it)
	{
		const T_ManagerTopBar& bar = it->second;
		if (bar.m_strMerchCode == pMerch->m_MerchInfo.m_StrMerchCode)	//	������Ʒ�����ѯ��Ӧ�İ�ťID
		{
			int iBtnId = it->first;
			if (m_cManagerTopBarInfo.m_mapTopbarBtnLeft.find(iBtnId) != m_cManagerTopBarInfo.m_mapTopbarBtnLeft.end())
			{
				SetCheckStatus(iBtnId);	//	���ð�ťΪcheck״̬
				m_LastPressTopBtnId = iBtnId;
			}
			break;
		}
	}
	

	
}