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

// tab与视图间分割条的宽度, 只处理了top 和 bottom
#define TAB_SPLITER_WIDTH	1


#define TOPBAR_BEGINID	33000
#define TOPBAR_ENDID		33100
#define INVALID_ID			-1


// 与该类相关的xml配置中相关信息
static const char *KStrElementValue				= "IoViewManager";
static const char *KStrElementAttrViewTab		= "ViewTab";
static const char *KStrElementAttrCurView		= "CurView";
static const char *KStrElementAttrAutoHide		= "AutoHide";
static const char *KStrElementAttrGroupId		= "GroupId";
static const char *KStrElementAttrShowManagerTopBar		= "ShowManagerTopBar";	// 是否显示顶部工具栏
static const char *KStrElementAttrbIsHasTopBarByLayout	= "bIsHasTopBarByLayout";	// 板块是否有顶部标签项
static const char *KStrElementAttrbIsHasBottomBarByLayout	= "bIsHasBottomBarByLayout";	// 板块是否有底部标签项

static const char *KStrElementAttrManagerTopBtn		= "ManagerTopBtn";				// 管理类顶部标签
static const char *KStrElementAttrManagerBarHight	= "ManagerBarHight";			// 顶部标签高度
static const char *KStrElementAttrManagerBarBkColor	= "ManagerBarBkColor";			// 顶部标签背景颜色
static const char *KStrElementAttrtabbarbtnWidth	= "tabbarbtnWidth";				// 标签栏的按钮的宽度
static const char *KStrElementAttrManagerBarType	= "ManagerBarType";				// 标签栏类型
static const char *KStrElementAttrRightimagePath	= "RightimagePath";				// 右侧按钮背景图片
static const char *KStrElementAttrtabbar			= "tabbar";						// 标签项
static const char *KStrElementAttrbtnType			= "btnType";					// 标签项功能类型
static const char *KStrElementAttrname				= "name";						// 标签项显示名称
static const char *KStrElementAttrmarketid			= "marketid";					// 市场ID
static const char *KStrElementAttrMerchCode			= "MerchCode";					// 商品代码
static const char *KStrElementAttrcfmname			= "cfmname";					// 版面名称
static const char *KStrElementAttrShrinkSize		= "ShrinkSize";					// 有效区域内缩多少像素
static const char *KStrElementAttrisSelected		= "isSelected";					// 是否为选中状态
static const char *KStrElementAttrAlignmente		= "Alignment";					// 对齐方式，左边显示，右边显示，中间显示
static const char *KStrElementAttrframeColor		= "frameColor";					// 控件边框颜色


static const char *KStrElementAttrTabBkClr		= "TabBkClr";			// tab条背景颜色
static const char *KStrElementAttrTabBkNorClr	= "TabBkNorClr";		// tab正常背景颜色
static const char *KStrElementAttrTabBkSelClr	= "TabBkSelClr";		// tab选中背景颜色
static const char *KStrElementAttrTabTextNorClr	= "TabTextNorClr";		// tab正常文字颜色
static const char *KStrElementAttrTabTextSelClr	= "TabTextSelClr";		// tab选中文字颜色
static const char *KStrElementAttrTabSpliterClr	= "TabSpliterClr";		// tab与视图间分割条颜色
static const char *KStrElementAttrTabFixWidth   = "TabFixWidth";        // tab标签每一项是否固定宽度

// 所有业务视图列表, 新建的业务视图一定要在这里签名， 才会被系统认识
// 跟 resource.h 中ID 定义的顺序要一致
static const T_IoViewObject s_astIoViewObjects[] = 
{
	T_IoViewObject(ID_PIC_TREND			,			EIBSNone,		EIMCSFalse,		L"分时走势",		L"分",		L"",					L"IoViewTrend",				E_IVGTChart,		RUNTIME_CLASS(CIoViewTrend), false),
	T_IoViewObject(ID_PIC_KLINE			,			EIBSNone,		EIMCSFalse,		L"Ｋ线分析",		L"K",		L"",		L"IoViewKLine",				E_IVGTChart,		RUNTIME_CLASS(CIoViewKLine), false),
	T_IoViewObject(ID_PIC_TICK			,			EIBSNone,		EIMCSFalse,		L"闪电图",			L"闪",		L"",					L"IoViewTick",				E_IVGTChart,		RUNTIME_CLASS(CIoViewTick), false),
	T_IoViewObject(ID_PIC_PRICE			,			EIBSNone,		EIMCSFalse,		L"买卖盘",			L"盘",		L"",	    				L"IoViewPrice",				E_IVGTPrice,		RUNTIME_CLASS(CIoViewPrice), false),
	T_IoViewObject(ID_PIC_TIMESALE		,			EIBSNone,		EIMCSFalse,		L"分笔成交",		L"笔",		L"",					L"IoViewTimeSale",			E_IVGTOther,		RUNTIME_CLASS(CIoViewTimeSale), false),
	T_IoViewObject(ID_PIC_NEWS			,			EIBSNone,		EIMCSFalse,		L"新闻资讯",		L"讯",		L"",					L"IoViewNews",				E_IVGTOther,		RUNTIME_CLASS(CIoViewNews), false),
//	T_IoViewObject(ID_PIC_NEWS			,			EIBSNone,		EIMCSTrue,		L"新闻资讯",		L"讯",		L"",					L"IoViewTextNews",			E_IVGTOther,		RUNTIME_CLASS(CIoViewTextNews), false),
    T_IoViewObject(ID_PIC_DETAIL		,			EIBSNone,		EIMCSFalse,		L"财富追踪",		L"财",		L"财富追踪指标",				L"IoViewDetail",			E_IVGTOther,		RUNTIME_CLASS(CIoViewDetail), false),	
	T_IoViewObject(ID_PIC_REPORT		,			EIBSNone,		EIMCSTrue,		L"报价列表",		L"报",		L"",		L"IoViewReport",			E_IVGTReport,		RUNTIME_CLASS(CIoViewReport), true),
    T_IoViewObject(ID_PIC_STARRY		,			EIBSNone,		EIMCSFalse,		L"星空图",		    L"星",		L"热点星空图",		            L"IoViewStarry",			E_IVGTOther,		RUNTIME_CLASS(CIoViewStarry), false, EMMT_StockCn),	
    T_IoViewObject(ID_PIC_CAPITALFLOW	,			EIBSHorizontal,	EIMCSFalse,		L"资金流向",		L"资",		L"个股资金流向图",		        L"IoViewCapitalFlow",		E_IVGTOther,		RUNTIME_CLASS(CIoViewCapitalFlow), false, EMMT_StockCn),	
    T_IoViewObject(ID_PIC_BLOCKREPORT	,			EIBSNone,		EIMCSFalse,		L"板块列表",		L"板",		L"板块列表",			        L"IoViewBlockReport",		E_IVGTOther,		RUNTIME_CLASS(CIoViewBlockReport), false, EMMT_StockCn),	
    T_IoViewObject(ID_PIC_MAIN_TIMESALE	,			EIBSHorizontal,	EIMCSFalse,		L"主力监控",		L"主",		L"主力监控",			        L"IoViewMainTimeSale",		E_IVGTOther,		RUNTIME_CLASS(CIoViewMainTimeSale), false, EMMT_StockCn),	
	T_IoViewObject(ID_PIC_REPORT_RANK	,			EIBSNone,		EIMCSFalse,		L"报价排行",		L"排",		L"报价排行",			        L"IoViewReportRank",		E_IVGTOther,		RUNTIME_CLASS(CIoViewReportRank), false, EMMT_StockCn),	
    //T_IoViewObject(ID_PIC_REPORT_SELECT	,			EIBSNone,		EIMCSTrue,		L"智能选股",		L"选",		L"条件选股报价排行",	        L"IoViewReportSelect",		E_IVGTOther,		RUNTIME_CLASS(CIoViewReportSelect), false, CViewData::EMMT_StockCn),	
    T_IoViewObject(ID_PIC_DADANPAIHANG	,			EIBSNone,		EIMCSTrue,		L"大单组合",		L"单",		L"大单组合",					L"IoViewDaDanPaiHang",		E_IVGTOther,		RUNTIME_CLASS(CIoViewDaDanPaiHang), false, EMMT_StockCn),	
    T_IoViewObject(ID_PIC_VALUE			,			EIBSHorizontal,		EIMCSFalse,		L"相关数值",		L"值",		L"相关数值",					L"IoViewValue",				E_IVGTOther,		RUNTIME_CLASS(CIoViewValue), false, EMMT_StockCn),	
    T_IoViewObject(ID_PIC_EXPTREND		,			EIBSNone,		EIMCSFalse,		L"指数走势",		L"指",		L"商品对应指数走势分析",		L"IoViewExpTrend",			E_IVGTOther,		RUNTIME_CLASS(CIoViewExpTrend), false, EMMT_StockCn),	
	T_IoViewObject(ID_PIC_FENJIABIAO	,			EIBSNone,		EIMCSFalse,		L"分价表",			L"价",		L"分价表",				        L"IoViewFenJiaBiao",		E_IVGTOther,		RUNTIME_CLASS(CIoViewFenJiaBiao), false),	
    T_IoViewObject(ID_PIC_CHOUMA		,			EIBSHorizontal, EIMCSFalse,		L"筹码分布",		L"筹",		L"筹码分布",			        L"IoViewChouMa",			E_IVGTOther,		RUNTIME_CLASS(CIoViewChouMa), false),	
	T_IoViewObject(ID_PIC_PHASESORT		,			EIBSNone,		EIMCSFalse,		L"阶段排行",		L"阶",		L"",			        L"IoViewPhaseSort",			E_IVGTOther,		RUNTIME_CLASS(CIoViewPhaseSort), false),	
	T_IoViewObject(ID_PIC_DUOGUTONGLIE	,			EIBSNone,		EIMCSFalse,		L"多合约同列",		L"股",		L"",			        L"IoViewDuoGuTongLie",		E_IVGTOther,		RUNTIME_CLASS(CIoViewDuoGuTongLie), false),	
	T_IoViewObject(ID_PIC_KLINEARBITRAGE	,		EIBSNone,		EIMCSFalse,		L"套利K线图",		L"利",		L"",			        L"IoViewKLineArbitrage",	E_IVGTOther,		RUNTIME_CLASS(CIoViewKLineArbitrage), false),	
	T_IoViewObject(ID_PIC_TRENDARBITRAGE	,		EIBSNone,		EIMCSFalse,		L"套利分时图",		L"利",		L"",			    L"IoViewTrendArbitrage",	E_IVGTOther,		RUNTIME_CLASS(CIoViewTrendArbitrage), false),	
	//T_IoViewObject(ID_PIC_LEVEL2	,	L"经纪",			L"纪",		L"经纪买卖盘",			 		L"IoViewLevel2",		E_IVGTOther,		RUNTIME_CLASS(CIoViewLevel2)),
	//T_IoViewObject(ID_PIC_F10		,	L"资料",			L"资",		L"F10资讯",					 	L"IoViewF10",			E_IVGTOther,		RUNTIME_CLASS(CIoViewF10)),
	//T_IoViewObject(ID_PIC_RELATIVE,	L"相关",			L"关",		L"相关商品",	    			L"IoViewRelative",		E_IVGTRelative,		RUNTIME_CLASS(CIoViewRelative)),
	T_IoViewObject(ID_PIC_REPORT_ARBITRAGE,			EIBSNone,		EIMCSTrue,		L"套利报价表",		L"利",		L"",			        L"IoViewReportArbitrage",	E_IVGTOther,		RUNTIME_CLASS(CIoViewReportArbitrage), false),		
	T_IoViewObject(ID_PIC_WEB_NEWS,					EIBSNone,		EIMCSTrue,		L"网页",			L"网",		L"",					L"IoViewNews",			E_IVGTOther,		RUNTIME_CLASS(CIoViewNews), false),
	T_IoViewObject(ID_PIC_QR_CODE,					EIBSNone,		EIMCSTrue,		L"二维码",			L"二",		L"",						L"IoViewQRCode",			E_IVGTReport,		RUNTIME_CLASS(CIoViewQRCode), false),	
	T_IoViewObject(ID_PIC_MULTI_RANK,				EIBSNone,		EIMCSFalse,		L"多排名同列",	    L"名",		L"多排名同列",				    L"IoViewMultiRank",		    E_IVGTOther,		RUNTIME_CLASS(CIoViewMultiRank), false),	
	T_IoViewObject(ID_PIC_DUANXIANJIANKONG,			EIBSNone,		EIMCSFalse,		L"短线监控",	    L"监",		L"短线监控",				    L"IoViewDuanXianJianKong",  E_IVGTOther,		RUNTIME_CLASS(CIoViewDuanXianJianKong), false),	
	T_IoViewObject(ID_PIC_MULTI_CYCLE_KLINE,	    EIBSNone,		EIMCSFalse,		L"多K线同列",	    L"列",		L"多K线同列",				    L"IoViewMultiCycleKline",   E_IVGTOther,		RUNTIME_CLASS(CIoViewMultiCycleKline), false),	
	T_IoViewObject(ID_PIC_POSITIONS,				EIBSNone,		EIMCSFalse,		L"持仓仓位",		L"持",		L"持仓仓位",					L"IoViewPositions",			E_IVGTOther,		RUNTIME_CLASS(CIoViewPositions), false, EMMT_StockCn),
	T_IoViewObject(ID_PIC_TREND_ANALYSIS,			EIBSNone,		EIMCSFalse,		L"大势研判",		L"势",		L"大势研判",					L"IoViewTrendAnalysis",		E_IVGTOther,		RUNTIME_CLASS(CIoViewTrendAnalysis), false, EMMT_StockCn),
	T_IoViewObject(ID_PIC_DK_MONEY,					EIBSNone,		EIMCSFalse,		L"多空资金",		L"空",		L"多空资金",					L"IoViewDKMoney",			E_IVGTOther,		RUNTIME_CLASS(CIoViewDKMoney), false, EMMT_StockCn),
	T_IoViewObject(ID_PIC_MAIN_COST,				EIBSNone,		EIMCSFalse,		L"主力成本",		L"主",		L"主力成本",					L"IoViewMainCost",			E_IVGTOther,		RUNTIME_CLASS(CIoViewMainCost), false, EMMT_StockCn),
	T_IoViewObject(ID_PIC_SUGGEST_POSITION,			EIBSNone,		EIMCSFalse,		L"建议仓位",		L"仓",		L"建议仓位",					L"IoViewSuggestPosition",	E_IVGTOther,		RUNTIME_CLASS(CIoViewSuggestPosition), false, EMMT_StockCn),
	T_IoViewObject(ID_PIC_WEB_CAPITAL_FLOW,			EIBSHorizontal,	EIMCSFalse,		L"资金流向",		L"流",		L"资金流向",					L"IoViewWebCapitalFlow",	E_IVGTOther,		RUNTIME_CLASS(CIoViewWebCapitalFlow), false, EMMT_StockCn),
	T_IoViewObject(ID_PIC_ADD_USER_BLOCK,			EIBSNone,		EIMCSFalse,		L"自选添加",		L"选",		L"自选添加",					L"IoViewAddUserBlock",	    E_IVGTOther,		RUNTIME_CLASS(CIoViewAddUserBlock), false, EMMT_StockCn),
	T_IoViewObject(ID_PIC_TIMESALE_RANK,			EIBSNone,		EIMCSFalse,		L"大单排行",		L"排",		L"大单排行",					L"IoViewTimeSaleRank",	    E_IVGTOther,		RUNTIME_CLASS(CIoViewTimeSaleRank), false, EMMT_StockCn),
	T_IoViewObject(ID_PIC_TIMESALE_STATISTIC,		EIBSNone,		EIMCSFalse,		L"大单统计",		L"统",		L"大单统计",					L"IoViewTimeSaleStatistic",	E_IVGTOther,		RUNTIME_CLASS(CIoViewTimeSaleStatistic), false, EMMT_StockCn),
	T_IoViewObject(ID_PIC_SMART_CHOOSE_STOCK,		EIBSNone,		EIMCSFalse,		L"智能选股",		L"选",		L"智能选股",					L"IoViewSmartChooseStock",	E_IVGTOther,		RUNTIME_CLASS(CIoViewSmartChooseStock), false),
    T_IoViewObject(ID_PIC_REPORT_NEWSTOCK,		    EIBSNone,		EIMCSFalse,		L"新股报价表",		L"新",		L"新股报价表",					L"IoViewNewStockReport",	E_IVGTOther,		RUNTIME_CLASS(CIoViewNewStockReport), false),
};

static const int32 KIoViewObjectCount = sizeof(s_astIoViewObjects) / sizeof(T_IoViewObject);
//
static const T_IoViewGroupObject s_astIoViewGroupObjects[] = 
{
	T_IoViewGroupObject(L"图表",	L"图",		E_IVGTChart),
	T_IoViewGroupObject(L"报价表",	L"报",		E_IVGTReport),
	//T_IoViewGroupObject(L"相关",	L"关",		E_IVGTRelative),
	T_IoViewGroupObject(L"买卖盘",	L"买",		E_IVGTPrice),
	T_IoViewGroupObject(L"其他",	L"资",		E_IVGTOther),
};

static const int32 KIoViewGroupObjectCount = sizeof(s_astIoViewGroupObjects) / sizeof(T_IoViewGroupObject);
// 默认的业务视图
CRuntimeClass *CIoViewManager::m_pDefaultIoViewClass = RUNTIME_CLASS(CIoViewTrend);	// 修改这里


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
	m_iGroupId  = 1;			// 默认大家都是1分组
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
		// 判断当前按钮是否需要重绘
		if (!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;
		}
		ncButton.DrawButton(&graphics);
	}

	// 右侧按钮
	for (iter = m_cManagerTopBarInfo.m_mapTopbarBtnRight.begin(); iter != m_cManagerTopBarInfo.m_mapTopbarBtnRight.end(); iter++)
	{
		CNCButton &ncButton = iter->second;
		ncButton.GetRect(rcControl);
		// 判断当前按钮是否需要重绘
		if (!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;
		}
		ncButton.DrawButton(&graphics);
	}
	// 中部按钮
	for (iter = m_cManagerTopBarInfo.m_mapTopbarBtnCenter.begin(); iter != m_cManagerTopBarInfo.m_mapTopbarBtnCenter.end(); iter++)
	{
		CNCButton &ncButton = iter->second;
		ncButton.GetRect(rcControl);
		// 判断当前按钮是否需要重绘
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
	cbr.CreateStockObject(NULL_BRUSH); //创建一个空画刷
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


	// tab与视图间分割条绘制, 只处理了top 和 bottom
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
	// 窗口只要有显示，client rect在影响的范围内，系统会按照Z轴发送WM_PAINT?
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
// 			// 不需要通知，系统貌似能自动根据所占区域通知重绘
// // 			CRect rcNeedDraw;
// // 			int32 iRcClipMode = dc.GetClipBox(&rcNeedDraw);
// // 			if ( ERROR != iRcClipMode && NULLREGION != iRcClipMode )
// // 			{
// // 				// 只绘制需要绘制的部分，尽量避免闪烁
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


	// 设置分组信息
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
	m_GuiTabWnd.SetTipTitle(L"业务视图");	
/*
	//--- wangyongxue 暂时不设定固定大小
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
	
	// 加入视图队列
	m_IoViewsPtr.Add(pIoView);
	
	// 增加对应的标签页
	m_GuiTabWnd.Addtab(pstIoViewObject->m_StrLongName, pstIoViewObject->m_StrShortName, pstIoViewObject->m_StrTipMsg);				
	
	// 设置图标
	int32 iTabCount = m_GuiTabWnd.GetCount();
	m_GuiTabWnd.SetTabIcon(iTabCount - 1, 0);
	
	// 总是显示第一个
	for (int i = 0; i < m_IoViewsPtr.GetSize(); i++)
	{
		CIoViewBase *pIoViewBase = m_IoViewsPtr[i];
		if (NULL == pIoViewBase)
			continue;

		if (0 == i)
		{
			pIoViewBase->ShowWindow(SW_SHOW);
			pIoViewBase->OnIoViewActive();
			pIoViewBase->OnVDataForceUpdate();	// 强制刷新
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
	// 计算宽度依次为，最右边，最左边和中间。空间不够就不做处理
	CRect rcAreaOffset(1,1,1,1);
	int iClientWidth = rect.Width();
	int iBtnWidth = m_cManagerTopBarInfo.m_iTopBarBtnWidth;		// 按钮宽度为80个像素
	int iBtnWidthRight = 110;
	if (m_cManagerTopBarInfo.m_pImageBk)
	{
		iBtnWidthRight = m_cManagerTopBarInfo.m_pImageBk->GetWidth();
	}
	
	int iLeftWidthCount		= m_cManagerTopBarInfo.m_mapTopbarBtnLeft.size()*iBtnWidth;
	int iRihgtWidthCount	= m_cManagerTopBarInfo.m_mapTopbarBtnRight.size()*iBtnWidthRight;
	int iCenterWidthCount	= m_cManagerTopBarInfo.m_mapTopbarBtnCenter.size()*iBtnWidth;

	// 最左边的先画了再说
	m_rcManagerTopBtn = rect;
	m_rcManagerTopBtn.bottom = m_rcManagerTopBtn.top + m_cManagerTopBarInfo.m_iBarHight;

	// 左侧按钮
	CRect rcLeftBtn = m_rcManagerTopBtn;
	rcLeftBtn.right = rcLeftBtn.left;
	map<int32, CNCButton>::iterator iterleft;
	for (iterleft = m_cManagerTopBarInfo.m_mapTopbarBtnLeft.begin(); iterleft != m_cManagerTopBarInfo.m_mapTopbarBtnLeft.end(); iterleft++)
	{
		CNCButton &ncButton = iterleft->second;
		rcLeftBtn.left = rcLeftBtn.right;
		rcLeftBtn.right += iBtnWidth;

		// 不占用最右侧按钮位置
		if (iClientWidth - rcLeftBtn.right < iRihgtWidthCount)
		{
			break;;
		}
		// 内缩小多少像素
		if (m_cManagerTopBarInfo.m_mapTopbarGroup.count(iterleft->first) > 0)
		{
			int iShrinkSize = m_cManagerTopBarInfo.m_mapTopbarGroup[iterleft->first].m_iShrinkSize;
			rcLeftBtn.DeflateRect(iShrinkSize,iShrinkSize,iShrinkSize,iShrinkSize);
		}

		ncButton.SetRect(rcLeftBtn);

		// 有效区域减去1，因为要画线重叠
		ncButton.SetAreaOffset(rcAreaOffset);
		rcLeftBtn.right -= 1;
	}

	// 右侧按钮
	CRect rcRightBtn = m_rcManagerTopBtn;
	rcRightBtn.left = rcRightBtn.right;
	map<int32, CNCButton>::iterator iterRight;
	for (iterRight = m_cManagerTopBarInfo.m_mapTopbarBtnRight.begin(); iterRight != m_cManagerTopBarInfo.m_mapTopbarBtnRight.end(); iterRight++)
	{
		CNCButton &ncButton = iterRight->second;

		rcRightBtn.right = rcRightBtn.left;
		rcRightBtn.left  -= iBtnWidthRight;

		// 最后边的不足限制，本来按钮就少
		if (rcRightBtn.left < 0)
		{
			break;
		}

		// 内缩小多少像素
		if (m_cManagerTopBarInfo.m_mapTopbarGroup.count(iterRight->first) > 0)
		{
			int iShrinkSize = m_cManagerTopBarInfo.m_mapTopbarGroup[iterRight->first].m_iShrinkSize;
			rcRightBtn.DeflateRect(iShrinkSize,iShrinkSize,iShrinkSize,iShrinkSize);
		}

		ncButton.SetRect(rcRightBtn);

		// 有效区域减去1，因为要画线重叠
		ncButton.SetAreaOffset(rcAreaOffset);
		rcRightBtn.left += 1;
	}

	// 中部按钮
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

			// 内缩小多少像素
			if (m_cManagerTopBarInfo.m_mapTopbarGroup.count(iterCenter->first) > 0)
			{
				int iShrinkSize = m_cManagerTopBarInfo.m_mapTopbarGroup[iterCenter->first].m_iShrinkSize;
				rcCenterBtn.DeflateRect(iShrinkSize,iShrinkSize,iShrinkSize,iShrinkSize);
			}

			ncButton.SetRect(rcCenterBtn);

			// 有效区域减去1，因为要画线重叠
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
		// 如果当前在F7模式下，则不显示底部按钮，  需要配置否？
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
	// 这里必须要要给从XML中初始化的首次的每一个视图一次OnSize机会，以让它们依赖的OnSize消息能够被触发 xl 临时修复
	CIoViewBase *pActiveIoView = GetActiveIoView();
	if ( !rect.IsRectEmpty() )
	{
		for ( int i=0; i < m_IoViewsPtr.GetSize() && m_bFromXmlNeedInit ; i++ )
		{
			// 发送Size消息
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


	// 重置按钮
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
	// 从当前tab条中选择下一个
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
			// 从当前tab条中选择下一个  
			if ( GetAsyncKeyState(VK_CONTROL) & 0x8000 )
			{
				return FALSE;
			}
			DealTabKey();
		}
		return TRUE;
	}

	// 把消息丢给下面的业务视图
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
		// F7 按钮:
		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		if ( NULL != pMainFrame)
		{
			pMainFrame->OnProcessF7();
		}
	}
	else if ( IOVIEW_TITLE_ADD_BUTTON_ID == uID )
	{
		// 弹出显示菜单,添加视图
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
 				// 报价表下面加子菜单
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
 						if (StrBreedName == L"香港证券" || StrBreedName == L"新加坡期货" || StrBreedName == L"全球指数")
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
		// 删除当前视图
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
			((CMainFrame *)AfxGetMainWnd())->SetEmptyGGTongView(pParent);	// 设置空视图
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
		// 左方向键

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
			// 如果到头了,显示最右边一个.循环.
			iTab = m_GuiTabWnd.GetCount() - 1;
		}

		m_GuiTabWnd.SetCurtab(iTab);
		m_IoViewsPtr[iTab]->ShowWindow(SW_SHOW);
	}
	else if ( IOVIEW_TITILE_RIGHT_BUTTON_ID == uID)
	{
		// 右方向键
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
			// 如果到头了,显示最右边一个.循环.
			iTab = 0;
		}
		
		m_GuiTabWnd.SetCurtab(iTab);
		m_IoViewsPtr[iTab]->ShowWindow(SW_SHOW);

	}
	else
	{
		// 单击了其他业务视图
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
			
			// 0001722 在ioViewManager中的onSize会很麻烦，所以所有的非active ioview的size全部在这里恢复和设置
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
			//OnSize(SIZE_RESTORED, rect.Width(), rect.Height());     //0001722 - 让ioViewManager把现在激活的IoView恢复size吧
			OnSizeChange(rect);
			//0001722 -end
			
			pIoViewBase->OnIoViewActive();
			//pIoViewBase->OnVDataForceUpdate();	// 强制刷新 - xl 0904 使用timer检查是否有忽略请求，如有，则在短时间内，窗口可见时强制刷新，不需要此时强制
			
			//#BUG_MARK_BEGIN [NO=0001536 AUTHOR=xiali DATE=2010/04/09]
			//DESCRIPTION:	临时解决方法，主要是IoViewBase的派生类行为不统一，只有让他们重画了
			pIoViewBase->Invalidate(TRUE);	//bug xiali 临时标记 
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
				// 当前激活的是特殊视图, 而且原来不是特殊的
				if ( !m_bBeyondShow )
				{
					BeyondShowIoView(true, pIoViewBase);
				}					
			}
			else
			{
				// 当前不是特殊视图, 而且原来是特殊的
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

	// 上下缩进线
	CString StrTopBottomBar;
	StrTopBottomBar.Format(L"%s=\"%d\" %s=\"%d\" ",
					CString(KStrElementAttrbIsHasTopBarByLayout).GetBuffer(),
					m_bIsHasTopBarByLayout, 
					CString(KStrElementAttrbIsHasBottomBarByLayout).GetBuffer(),
					m_bIsHasBottomBarByLayout);


	
	// manager结束标志
	StrThis += L">\n";
	// 组织各个子业务视图
	CIoViewBase *pIoView = NULL;
	for (int32 i = 0; i < m_IoViewsPtr.GetSize(); i++)
	{
		pIoView = m_IoViewsPtr[i];
		if (NULL == pIoView)
			continue;
		StrThis += pIoView->ToXml();
	}


	// 保存顶部标签属性
	// 保存特殊商品列表
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

			// 显示方式
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

			//// 组合字符串
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

	// 判断是不是描述自己的
	const char *pcValue = pElement->Value();
	if (strcmp(pcValue, KStrElementValue) != 0)
		return false;

	// 后面用的
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;
	if (NULL == pDocument)
		return false;

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	if (NULL == pAbsCenterManager)
		return false;

	// 清掉当前所有的信息
	int32 i = 0;
	for ( i = 0; i < m_IoViewsPtr.GetSize(); i++)
	{
		m_IoViewsPtr[i]->DestroyWindow();
		DEL(m_IoViewsPtr[i]);
	}

	m_IoViewsPtr.RemoveAll();	
	//
	m_GuiTabWnd.DeleteAll();

	// 处理属性
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

	// tab颜色
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


	// 不加判断直接通过有没有配置来初始化
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
		

	// 轮询所有的子节点， 根据属性描述的具体视图创建对应IoView视图
	TiXmlElement *pElementIoView = pElement->FirstChildElement();
	while (pElementIoView)
	{
		pcValue = pElementIoView->Value();
		if (strcmp(CIoViewBase::GetXmlElementValue(), pcValue) == 0)	// 找到业务视图
		{
			const char *pcTmpAttrValue = pElementIoView->Attribute(CIoViewBase::GetXmlElementAttrIoViewType());
			if (NULL == pcTmpAttrValue)	// 找不到具体业务视图类型， 不行！
				return FALSE;

			CString StrIoViewXmlName = pcTmpAttrValue;
			const T_IoViewObject *pstIoViewObject = FindIoViewObjectByXmlName(StrIoViewXmlName);
			if (NULL == pstIoViewObject)	// 找不到对应的业务视图类型，不行！
				return FALSE;

			// 创建业务视图
			if (NULL == pstIoViewObject->m_pIoViewClass)
				return FALSE;

			// tab显示的名字
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
			pIoView->Create(WS_CHILD|SS_LEFT|SS_NOTIFY/*|WS_VISIBLE*/,this,CRect(0,0,0,0), 10204);	// 默认都不可见
			pIoView->m_StrTabShowName = StrTabName;
			
#ifdef _DEBUG
			TRACE(_T("视图[%s]创建: %d ms\r\n"), StrIoViewXmlName.GetBuffer(), timeGetTime()-dwTmpTime);
			dwTmpTime = timeGetTime();
#endif

			if (!pIoView->FromXml(pElementIoView))
				return FALSE;
			
#ifdef _DEBUG
			TRACE(_T("视图[%s]FromXml: %d ms\r\n"), StrIoViewXmlName.GetBuffer(), timeGetTime()-dwTmpTime);
			dwTmpTime = timeGetTime();
#endif
			// 给每个IoView一次初始化副图等资源(OnSize)的机会
		
			// 加入视图队列
			m_IoViewsPtr.Add(pIoView);

			// 增加对应的标签页
			m_GuiTabWnd.Addtab(StrTabName, pstIoViewObject->m_StrShortName, pstIoViewObject->m_StrTipMsg);
			
			// 设置图标
			int32 iTabCount = m_GuiTabWnd.GetCount();
			m_GuiTabWnd.SetTabIcon(iTabCount - 1, 0);

			// 请求初始化数据
//			pIoView->OnVDataForceUpdate();
//#ifdef _DEBUG
//			TRACE(_T("视图[%s]OnVDataForceUpdate: %d ms\r\n"), StrIoViewXmlName.GetBuffer(), timeGetTime()-dwTmpTime);
//			dwTmpTime = timeGetTime();
//#endif
		}

		//
		pElementIoView = pElementIoView->NextSiblingElement();
	}
	//////////////////////////////////////////////
	// 确定该显示哪个业务视图
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
			//pIoViewBase->OnVDataForceUpdate();	// 强制刷新
		}
		else
		{
			pIoViewBase->ShowWindow(SW_HIDE);			
		}
	}

	// 改变tab布局
	m_eViewTab = eViewTab;

	//
	m_GuiTabWnd.SetCurtab(iCurView);
	
	// XL0006 这个貌似要手动调用，否则是没有这个notify的
	// OnTabSelChange();

	// 从XML中加载了，需要初始化
	m_bFromXmlNeedInit = true;

#ifdef _DEBUG
	TRACE(_T("IoViewManager完成FromXml: %d ms\r\n"), timeGetTime()-dwTime);
#endif

	return true;
}



bool32 CIoViewManager::FromXmlTopBtn( TiXmlElement *pTiXmlElement )
{
	if (NULL == pTiXmlElement)
		return false;


	// 当前选择，两种选择的参数
	m_cManagerTopBarInfo.m_mapTopbarGroup.clear();
	TiXmlElement *pMyEle = pTiXmlElement->FirstChildElement(KStrElementAttrManagerTopBtn);
	if ( NULL != pMyEle )
	{
		// 获取背景色和控件告诉
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

			//	控件显示背景色
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

			//	控件文本色
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

			// 控件边框色
			const char* pIndexFrameTextColor	= pElement->Attribute(KStrElementAttrframeColor);
			if (NULL != pIndexFrameTextColor)
			{						
				stManagerTopbar.m_colTextframe = StrToColorRef(pIndexFrameTextColor);					
			}
			
			stManagerTopbar.m_iBtnID = TOPBAR_BEGINID + iID++;
			m_cManagerTopBarInfo.m_mapTopbarGroup[stManagerTopbar.m_iBtnID] = stManagerTopbar;
			//// 指标
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
		m_Font.m_StrName = (L"宋体");
		m_Font.m_Size	 = 10.5;
		m_Font.m_iStyle	 = FontStyleRegular;	
		btnControl.SetFont(m_Font);
		btnControl.SetBtnBkgColor(colorBk);
		
		if (ALIGNMENT_LEFT == topTabbar.m_iAlignmentType)
		{
			btnControl.CreateButton(topTabbar.m_StrShowName, rcRect, this, NULL, 1, topTabbar.m_iBtnID);
			// 设置选中
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
			// 设置选中
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
			// 设置选中
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
		if (pIoView->GetRuntimeClass() == (s_astIoViewObjects[i].m_pIoViewClass))	// 严格相等
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
	
	// 默认业务视图
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
	// 修改为按照当前选择的返回active view，如果不匹配，在按照原来的返回 xl
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
	// 发送消息给当前可见的业务视图
	CIoViewBase *pIoView = GetActiveIoView();
	if (NULL != pIoView)
	{
	    pIoView->OnIoViewActive();
	}
// 	if (GetFocus() != pIoView)
// 	{
// 		SetFocus();
// 	}

	// xl  TBWnd注册ioview类型
	RegisterActiveIoViewToMainFrame(true);
}

void CIoViewManager::OnIoViewManagerDeactive()
{
	// 发送消息给当前可见的业务视图
	for (int32 i = 0; i < m_IoViewsPtr.GetSize(); i++)
	{
		CIoViewBase *pIoView = m_IoViewsPtr[i];
		if (NULL == pIoView)
			continue;

		if (pIoView->IsWindowVisible() || i == m_GuiTabWnd.GetCurtab())	// 保证当前窗口总能收到消息
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
	// 发消息给这个IoViewManager 下所有的业务视图
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
// 		menu.AppendODMenu(L"删除视图",MF_STRING,ID_IOVIEWTABLE_DEL);
// 	}

	CNewMenu * pSubMenuView = menu.AppendODPopupMenu(L"增加业务视图");
	
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
		CNewMenu * pSubMenuBlock = menu.AppendODPopupMenu(L"增加板块视图");
		
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
			if (StrBreedName == L"香港证券" || StrBreedName == L"新加坡期货" || StrBreedName == L"全球指数")
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
		// 如果返回的是NULL，则注销
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
	// 现在当前manager下找存在的对应图标，如果存在，则是当前页面的，否则mainframe
	// 只有在当前视图是图表的情况下才成立
	CIoViewBase *pIoViewActive = GetActiveIoView();
	if(NULL == pIoViewActive)
	{
		return;
	}
	CMerch *pMerchXml = pIoViewActive->GetMerchXml();

	if ( NULL == pMerchXml )
	{
		return;	// 无商品不用处理了
	}

	if ( pIoViewActive->IsKindOf(RUNTIME_CLASS(CIoViewKLine)) )
	{
		// K线找分时
		CIoViewBase *pTrend = FindIoViewByPicMenu(ID_PIC_TREND, false);
		if ( NULL != pTrend )
		{
			pTrend->BringToTop();
			return;	// 不用切换商品了
		}
	}
	else if ( pIoViewActive->IsKindOf(RUNTIME_CLASS(CIoViewTrend)) )
	{
		// 分时找K
		CIoViewBase *pKline = FindIoViewByPicMenu(ID_PIC_KLINE, false);
		if ( NULL != pKline )
		{
			pKline->BringToTop();
			return;
		}
	}

	// 其它情况mainframe处理

	// F5功能:  交给mainframe
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

	// 不是切分窗口的, 不用处理了
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

		// 是否需要处理, 如果视图的抢占类型和当前的切分类型不同的话就不用处理了
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
			// 不用处理了
			return;
		}

		//		
		if( m_bBeyondShow && !bForceShow )
		{
			// 上一个也是特殊视图, 不用管
			return;
		}
		else
		{
			// 隐藏另一半	
			// 如果当前ggtongView已经是F7状态，则不需要弹开任何窗口了
			CMPIChildFrame *pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
			if ( NULL == pFrame || !pFrame->IsF7AutoLock() )
			{
				int32 iID =	pParentSplit->GetViewPaneID(pGGTongView);
				pParentSplit->HidePane(1 - iID);	
			}
			m_bBeyondShow = true;		// 设置弹开标志，以保证F7还原时能够再次弹开
		}		
	}
	else
	{
		if ( !m_bBeyondShow && !bForceShow )
		{
			// 原来就不是, 不用管
			return;
		}
		else
		{
			// 显示原来的视图
			// 如果现在在F7状态，则需要忽略这次还原，等到用户F7还原时才显示原来被隐藏部分
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
		// 默认是当前显示的视图
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
	//	绘制顶部工具栏，底部工具栏，边框线，四角圆心
	COLORREF clrEllipse = RGB(214,57,61);
	COLORREF clrBorder = RGB(77,79,84);	
	CRect rcClient, rcBorder;
	GetClientRect(&rcClient);
	rcBorder = rcClient;
	//	裁剪区域（我们仅仅需要非ChartRgn的区域即可）
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
	//	绘制背景色
	memDC.FillSolidRect(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), RGB(33, 36, 41));
	//	绘制边框线，四角圆心
	{
		CPen pen(PS_SOLID, 2, clrBorder);
		HGDIOBJ hOldPen = memDC.SelectObject(&pen);

		int iCorner = 10;	//	四角距离x,y中心点的塌陷长度
		//	边框线位置
		CRect rtBorderInside(rcBorder);
		rtBorderInside.InflateRect(KiChartBorderLeft/4 * 3, KiChartBorderTop/4 * 3, KiChartBorderRight/4 * 3, KiChartBorderBottom/4 * 3);		

		//	左上，左上x,左上y
		CPoint ptLeftTop(rtBorderInside.left, rtBorderInside.top);
		CPoint ptLeftTopX(ptLeftTop);
		ptLeftTopX.x += iCorner;
		CPoint ptLeftTopY(ptLeftTop);
		ptLeftTopY.y += iCorner;

		//	右上，右上x, 右上y
		CPoint ptRightTop(rtBorderInside.right, rtBorderInside.top);
		CPoint ptRightTopX(ptRightTop);
		ptRightTopX.x -= iCorner;
		CPoint ptRightTopY(ptRightTop);
		ptRightTopY.y += iCorner;

		//	左下，左下x,左下y
		CPoint ptLeftBottom(rtBorderInside.left, rtBorderInside.bottom);
		CPoint ptLeftBottomX(ptLeftBottom);
		ptLeftBottomX.x += iCorner;
		CPoint ptLeftBottomY(ptLeftBottom);
		ptLeftBottomY.y -= iCorner;

		//	右下，右下x， 右下y
		CPoint ptRightBottom(rtBorderInside.right, rtBorderInside.bottom);
		CPoint ptRightBottomX(ptRightBottom);
		ptRightBottomX.x -= iCorner;
		CPoint ptRightBottomY(ptRightBottom);
		ptRightBottomY.y -= iCorner;
		//	画顶边
		memDC.MoveTo(ptLeftTopX);
		memDC.LineTo(ptRightTopX);
		//	画左上斜角
		memDC.MoveTo(ptLeftTopX);
		memDC.LineTo(ptLeftTopY);
		//	画右上斜角
		memDC.MoveTo(ptRightTopX);
		memDC.LineTo(ptRightTopY);
		////	画左边
		memDC.MoveTo(ptLeftTopY);
		memDC.LineTo(ptLeftBottomY);
		////	画左下斜角
		memDC.MoveTo(ptLeftBottomX);
		memDC.LineTo(ptLeftBottomY);
		////	画底边
		memDC.MoveTo(ptLeftBottomX);
		memDC.LineTo(ptRightBottomX);
		////	画右下斜角
		memDC.MoveTo(ptRightBottomX);
		memDC.LineTo(ptRightBottomY);
		////	画右边
		memDC.MoveTo(ptRightTopY);
		memDC.LineTo(ptRightBottomY);

		//	画四角圆点
		int iEllipse = 6;
		CBrush br(clrEllipse);
		CPen   ellipsePen(PS_SOLID, 1, clrEllipse);
		HGDIOBJ hOldBr = memDC.SelectObject(&br);
		memDC.SelectObject(&ellipsePen);
		//	画右上圆		
		CRect rtTopLeftEllipse(ptLeftTop.x - iEllipse/2, ptLeftTop.y - iEllipse/2, ptLeftTop.x + iEllipse/2, ptLeftTop.y + iEllipse/2);
		memDC.Ellipse(rtTopLeftEllipse);
		//	画右上圆
		CRect rtRightTopEllipse(ptRightTop.x - iEllipse/2, ptRightTop.y - iEllipse/2, ptRightTop.x + iEllipse/2, ptRightTop.y + iEllipse/2);
		memDC.Ellipse(rtRightTopEllipse);
		//	画左下圆
		CRect rtLeftBottomEllipse(ptLeftBottom.x - iEllipse/2, ptLeftBottom.y - iEllipse/2, ptLeftBottom.x + iEllipse/2, ptLeftBottom.y + iEllipse/2);
		memDC.Ellipse(rtLeftBottomEllipse);
		//	画右下圆
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
/* 函数名称：TButtonHitTest                                             */
/* 功能描述：获取当前鼠标所在的按钮										*/
/* 输入参数：point,当前鼠标位置											*/
/* 返回值：  按钮ID														*/
/************************************************************************/
int CIoViewManager::TButtonHitTest(CPoint point)
{
	map<int32, CNCButton>::iterator iter;
	int iBtnId = INVALID_ID;
	// 遍历工具栏上所有按钮
	for (iter=m_cManagerTopBarInfo.m_mapTopbarBtnLeft.begin(); iter!=m_cManagerTopBarInfo.m_mapTopbarBtnLeft.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		iBtnId   = btnControl.GetControlId();
		// 点point是否在已绘制的按钮区域内
		if (btnControl.PtInButton(point) && btnControl.GetCreate())
		{
			return  iBtnId;
		}
	}

	for (iter=m_cManagerTopBarInfo.m_mapTopbarBtnRight.begin(); iter!=m_cManagerTopBarInfo.m_mapTopbarBtnRight.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		iBtnId   = btnControl.GetControlId();
		// 点point是否在已绘制的按钮区域内
		if (btnControl.PtInButton(point) && btnControl.GetCreate())
		{
			return  iBtnId;
		}
	}

	for (iter=m_cManagerTopBarInfo.m_mapTopbarBtnCenter.begin(); iter!=m_cManagerTopBarInfo.m_mapTopbarBtnCenter.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		iBtnId   = btnControl.GetControlId();
		// 点point是否在已绘制的按钮区域内
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

	//	顶部按钮个数为0
	if (0 == m_cManagerTopBarInfo.m_mapTopbarGroup.size())
	{
		return;
	}

	map<int32, T_ManagerTopBar>::iterator it = m_cManagerTopBarInfo.m_mapTopbarGroup.begin();
	for (; it != m_cManagerTopBarInfo.m_mapTopbarGroup.end(); ++it)
	{
		const T_ManagerTopBar& bar = it->second;
		if (bar.m_strMerchCode == pMerch->m_MerchInfo.m_StrMerchCode)	//	根据商品代码查询对应的按钮ID
		{
			int iBtnId = it->first;
			if (m_cManagerTopBarInfo.m_mapTopbarBtnLeft.find(iBtnId) != m_cManagerTopBarInfo.m_mapTopbarBtnLeft.end())
			{
				SetCheckStatus(iBtnId);	//	设置按钮为check状态
				m_LastPressTopBtnId = iBtnId;
			}
			break;
		}
	}
	

	
}