// MainFrm.cpp : implementation of the CMainFrame class
//
#include "stdafx.h"
#include "MPIDocTemplate.h"
#include "coding.h"
#include "GGTongView.h"
#include "MPIChildFrame.h"
#include "DlgTrace.h"
#include "AdjustViewLayoutDlg.h"
#include "dlgsystemsetting.h"
#include "dlgworkspaceset.h"
#include "dlgbrokerwatch.h"
#include "DlgRecentMerch.h"
#include "keyboarddlg.h"
#include "StatusBarEx.h"
#include "dlgsetgridhead.h"
#include "dlgwspname.h"
#include "dlgnetflow.h"
#include "BiSplitterWnd.h"
#include "userblockmanager.h"
#include "dlgformula.h"
#include "hotkey.h"
#include "dlgmanual.h"

#include "sharefun.h"
#include "GmtTime.h" 
#include "IoViewTrend.h"
#include "IoViewTick.h"
#include "IoViewKLine.h"
#include "IoViewDetail.h"
#include "ShareFun.h"
#include "ShortCut.h"
#include "PathFactory.h"
#include "dlgwait.h"
#include "DlgAlarm.h"
#include "dlgchoosestock.h"
#include "dlgindexchoosestock.h"
#include "GdiPlusTS.h"

#include "dlgbuding.h"
#include "OptimizeServer.h"
#include "dlgloginnotify.h"
#include "dlgf10.h"
#include "dlgdownload.h"
#include  "tinyxml.h"
#include "AlarmCenter.h"
#include "DlgSyncExcel.h"
#include "IoViewSyncExcel.h"
#include "IoViewBlockReport.h"
#include "FontFactory.h"
#include "StockSelectManager.h"
#include "IoViewStarry.h"
#include "ColorStep.h"
#include "XmlShare.h"
#include "dlgtabname.h"
#include "XLTraceFile.h"

#include "dlgformularmanager.h"
#include "DlgHistoryTrend.h"
#include "BlockConfig.h"
#include "PluginFuncRight.h"
#include "ConfigInfo.h"

#include "UserNoteInfo.h"
#include "DlgNotePad.h"
#include "dlgconnectset.h"
#include "DlgChooseStockVar.h"
#include "dlgcustomstock.h"

#include "dlgcfmname.h"
#include "dlgworksheet.h"
#include "wspmanager.h"

#include "DlgPhaseSort.h"
#include "IoViewPhaseSort.h"
#include "IoViewDuoGuTongLie.h"

#include "DlgMarketRadarList.h"
#include "DlgMarkManager.h"
#include "DlgInputShowName.h"
#include "DlgMainTimeSale.h"
#include "DlgReportRank.h"
#include "BJSpecialStringImage.h"
#include "TraceLog.h"
#include "DlgArbitrage.h"
#include "DlgWelcome.h"
#include "DlgTip.h"

#include "ArbitrageFormula.h" 
#include "ArbitrageManage.h"
#include "IoViewKLineArbitrage.h"
#include "IoViewTrendArbitrage.h"
#include "IoViewReportArbitrage.h"
#include "MyDlgBar.h"
#include "StatusBarEx.h"
#include "GetCommandLine.h"

#include "IoViewTextNews.h"

#include "DlgRestoreWkSpace.h"
#include "DlgQuickOrder.h"
#include "FontFactory.h"
//
#include "CommentStatusBar.h"
#include "CRecordDataCenter.h"
#include "SelectStockCenter.h"
#include "CFormularContent.h"
#include "Pinyin.h"
#include "DlgTodayComment.h"
#include "DlgSloganExit.h"
#include "DlgNews.h"
#include "UtilTool.h"
#include "client_handler.h"
#include "UrlParser.h"
#include "WebClient.h"
#include "IoViewCowBear.h"
#include "IoViewChooseStock.h"
#include "IoViewReportSelect.h"
#include "DlgTradeSetting.h"
#include "DlgImportUserBlockBySoft.h"
#include "IoViewNewStock.h"
#include "EngineCenterExport.h"

#include <vector>
using	std::vector;
#include <map>
using	std::map;

#ifdef TIXML_USE_STL
#include <iostream>
#include <sstream>
using namespace std;
#else
#include <stdio.h>
#endif

#if defined( WIN32 ) && defined( TUNE )
#include <crtdbg.h>
_CrtMemState startMemState;
_CrtMemState endMemState;
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define INVALID_ID				-1
#define TITLE_HEIGHT			40			// 标题栏高度

#define MSG_SCTOOLBAR_MENU	    54300

#define SMARTSTOCK_PICKMODEL_ID_BEGIN 26000

CMainFrame *g_pMainFrame = NULL;


// #ifdef _DEBUG
// #pragma comment(lib, "tinyxmld.lib")
// #else
// #pragma comment(lib, "tinyxml.lib")
// #endif

#pragma comment(lib, "msimg32.lib")

typedef HWND  (WINAPI *HtmlHelpWFunc)(HWND /*hwndCaller*/, LPCWSTR /*pszFile*/, UINT /*uCommand*/, DWORD /*dwData*/);
static	CLoadLibHelperT<HtmlHelpWFunc> s_libHelper(_T("")/*_T("hhctrl.ocx")*/, "HtmlHelpW");	// 加载htmlhelpew
static  DWORD						   s_dwCookie = 0;

//
//void DataCenterFree(void* pData)
//{
//	if (pData)
//	{
//		delete pData;
//		pData = NULL;
//	}
//}


void OnConnNotify(E_CONN_NOTIFY eConnNotify)
{
	if (g_pMainFrame)
	{
		g_pMainFrame->ConnNotify(eConnNotify);
	}
}

void RecvMsgCenterData(std::string str)
{
	if (g_pMainFrame)
	{
		g_pMainFrame->handleMsgCenterRes(str);
	}
}

// TOOLBAR 工具栏中用到的图标
WORD g_awToolBarIconIDs[] =
{
	IDB_BITMAP_MENUICO,
	20, 16,
	IDM_IOVIEWBASE_RETURN,			// 操作返回	
	ID_TOOL_ICON_NOUSE,				// 窗口全屏	
	ID_TOOL_ICON_NOUSE,				// 窗口回复	
	IDM_IOVIEWBASE_FONT_BIGGER,		// 字体放大		
	IDM_IOVIEWBASE_FONT_SMALLER,	// 字体缩小	
	NULL
};

/*
WORD g_awToolBarIconIDs[] =
{
//  按照图片中的每个一小图给 ID 号.没用到的暂时用 ID_TOOL_ICON_NOUSE 替代
IDR_TOOLBAR_GGTONG,
16, 16,

ID_PIC_TREND,			// 分时		

ID_PIC_TICK,			// 闪电

ID_PIC_KLINE,			// K 线		

ID_PIC_PRICE,			// 买卖盘

ID_PIC_TIMESALE,		// 分笔

ID_PIC_NEWS,			// 财富资讯

ID_TOOL_ICON_NOUSE,		// F10

ID_PIC_DETAIL,			// 财富追踪

ID_PIC_REPORT,			// 报价表

ID_TOOL_ICON_NOUSE,		// K 线周期

ID_TOOL_ICON_NOUSE,		// 选择指标

ID_FORMULA_MAG,			// 公式管理

ID_GRIDHEAD_SET,		// 栏目设置	

ID_BLOCK_SET,			// 板块设置		

ID_SETTING,				// 风格设置		

ID_LAYOUT_ADJUST,		// 版面布局		

ID_VIEW_SDBAR,			// 自画线	

ID_UPDATE,				// 软件升级

ID_APP_ABOUT,			// 在线帮助

ID_WINDOW_NEW,			// 新建窗口

ID_TOOL_ICON_NOUSE,//ID_NEW_WORKSPACE,		// 新建版面		

ID_SAVE_WORKSPACE,		// 保存版面

IDM_IOVIEWBASE_RETURN,	// 操作返回			

IDM_IOVIEWBASE_FONT_BIGGER,		// 字体放大		

IDM_IOVIEWBASE_FONT_SMALLER,	// 字体缩小		

ID_TOOL_ICON_NOUSE,		// PAGE_UP

ID_TOOL_ICON_NOUSE,		// PAGE_DOWN

ID_TOOL_ICON_NOUSE,		// 水平收缩	

ID_TOOL_ICON_NOUSE,		// 水平拉伸		

ID_TOOL_ICON_NOUSE,		// 垂直收缩

ID_TOOL_ICON_NOUSE,		// 垂直拉伸

ID_TOOL_ICON_NOUSE,		// 窗口全屏		

ID_TOOL_ICON_NOUSE,		// 窗口回复

IDM_IOVIEWBASE_F7,		// 窗口分割	(这个图标做全屏恢复的还好看点)	

ID_DISCONNECT_SERVER,	// 断开服务器

ID_CONNECT_SERVER,		// 重连

ID_APP_EXIT,			// 退出

ID_OPEN_WORKSPACE,		// 打开版面

//ID_PIC_STARRY,			// 星空图

//ID_PIC_CAPITALFLOW,		// 资金流向



//ID_PIC_MAIN_TIMESALE,	// 主力监控

ID_PIC_BLOCKREPORT,		// 板块

ID_PIC_REPORT_RANK,		// 报价排行

ID_PIC_REPORT_SELECT,	// 条件报价

ID_VIEW_CALC,			// 计算器

ID_NEW_WORKSPACE,		// 新建版面
NULL
};
*/
//XML 
static const char * KStrElementAttriFileShowName	= "FileShowName";
static const char * KStrElementValue				= "MainFrame";
static const char * KStrWindowCounts				= "WindowCounts";

static const char * KStrElementAttriFlag			= "flag";
static const char * KStrElementAttriShowCmd			= "showCmd";
static const char * KStrElementAttriPtMinX			= "ptMinPosition.x";
static const char * KStrElementAttriPtMinY			= "ptMinPosition.y";
static const char * KStrElementAttriPtMaxX			= "ptMaxPosition.x";
static const char * KStrElementAttriPtMaxY			= "ptMaxPosition.y";

static const char * KStrElementAttriPtNorLeft		= "NormalPosition.left";
static const char * KStrElementAttriPtNorRight		= "NormalPosition.right";
static const char * KStrElementAttriPtNorTop		= "NormalPosition.top";							
static const char * KStrElementAttriPtNorBottom		= "NormalPosition.bottom";

static const char * KStrElementAttriFaceEffectRange = "FaceEffectRange";

//
const char * KStrRootElementValue			= "AlarmMerch";
const char * KStrRootElementAttriAlarmFlag	= "AlarmFlag";
const char * KStrRootElementAttriAlarmPrompt= "AlarmPrompt";
const char * KStrChildElementValue			= "Merch";
const char * KStrChildElementAttriMarket	= "Market";
const char * KStrChildElementAttriCode		= "Code";
const char * KStrChildElementAttriName		= "Name";
const char * KStrChildElementAttriCondition	= "Condition";
const char * KStrChildElementAttriParam		= "Param";
// 
const CString KStrWorkSpaceDir			  =  L"./WorkSpace/";
static const CString KStrXmlRootElementAttrDataWorkSpace		= L"WorkSpace";
//
extern const UINT KMsgChildFrameMaxSize   = WM_USER + 2103;

// 重连服务器时候,判断连接成功

// 等三分钟
const UINT KuiTimesToWaitForConnect		  = (1000/500)*60*3;

// 定时器ID
const UINT KTimerIdCheckConnectServer	  = 123456;
const UINT KTimerPeriodCheckConnectServer = 500;

const UINT KTimerIdCheckNewsConnectServer = 123457;
const UINT KTimerPeriodCheckNewsConnectServer = 500;

const UINT KTimerIdAutoUpdateCheck		  = 123461;			// 后台自动升级
const UINT KTimerPeriodAutoUpdateCheck	  = 5000;

const UINT KTimerIdReqPushJinping		  = 123458;			// 金评推送
const UINT KTimerPeriodReqPushJinping	  = 1000 * 30;

const UINT KTimerIdIndexShow			  = 123459;	
const UINT KTimerPeriodIndexShow		  = 5;

// 引导升级,消息提示请求定时器
const UINT KTimerInfoCenter				  = 1000110;
const UINT KTimerIntervalTime			  = 1;

// 智能选股状态定时器
const UINT KTimerPickModelStatus		  = 1000111;
const UINT KTimerPickModelStatusTime	  = 1000*10;

// 10分钟刷新token
const UINT KTimerIdRefreshToken			  = 123462;	
const UINT KTimerPeriodRefreshToken		  = 10;

// 1分钟调一次用户行为记录
const UINT KTimerIdUserDealRecord		  = 123463;	
const UINT KTimerPeriodUserDealRecord	  = 1;

const UINT KTimerIdDapanState             = 123466;			// 大盘状态
const UINT KTimerPeriodDapanState         = 1000 ;			// 1 min 请求一次大盘状态

const UINT KTimerIdChooseStockState       = 123468;			// 选股状态
const UINT KTimerPeriodChooseStockState   = 1 ;			    // 1 ms 请求一次选股状态

const UINT KTimerStartUpLogin		      = 123469;	        // 启动登录
const UINT KTimerPeriodStartUpLogin		  = 100 ;		    // 0.1s后启动登录

const UINT KTimerCheckToken		          = 123470;	        // 检查token是否有效
const UINT KTimerPeriodCheckToken		  = 1000*60 ;		// 间隔1min

//////////////////////////////////////////////////////////////////////////
const TCHAR KStrTBWndSection[]		= _T("TBWnd");
const TCHAR KStrTBWndTradeKey[]		= _T("trade");
//////////////////////////////////////////////////////////////////////////
const int     KSysMenuWidth = 62;
const int     iPushDlgWidth = 300;
const int     iPushDlgHeight = 200;
const int     iPushDlgRightSpace = 25;
const int     iPushDlgBottomSpace = 72;

const int     iTopSpace = 7;
/////////////////////////////////////////////////////////////////////////////
//
// CMainFrame
struct T_PicCfmInfo
{
	UINT	nPicId;				// pic id
	TCHAR   szCfmName[12];		// 页面名称
	bool32	bCreateIfNotExist;	// 不存在该页面是否创建
	UINT	nSystemPicId;			// 是否是系统相关页面, 是则加载系统相关Pic，否则填0
	UINT	nFunc;				// 所需要的权限
};
static T_PicCfmInfo s_aPicCfmInfos[] = 
{
	{ID_PIC_TREND, _T("分时走势"), true, ID_PIC_TREND, 0},
	{ID_PIC_KLINE, _T("K线分析"), true, ID_PIC_KLINE, 0},
	{ID_PIC_TICK, _T("闪电图"), true, 0, 0},
	{ID_PIC_TIMESALE, _T("K线分析"), false, ID_PIC_TREND, 0},		// 默认加载分时图里面的分笔
	{ID_PIC_STARRY, _T("热点挖掘"), false, 0, 0},
	{ID_PIC_CAPITALFLOW, _T("K线分析"), false, ID_PIC_KLINE, CPluginFuncRight::FuncCapitalFlow},
	{ID_PIC_MAIN_TIMESALE, _T("K线分析"), false, ID_PIC_KLINE, 0},
	{ID_PIC_FENJIABIAO, _T("K线分析"), false, ID_PIC_KLINE, 0},
	{ID_PIC_CHOUMA, _T("K线分析"), false, ID_PIC_KLINE, 0},
	{ID_PIC_PHASESORT, _T("阶段排行"), true, 0, 0},					// 阶段排行是需要特殊处理的
	{ID_PIC_REPORT_ARBITRAGE, _T("套利报价表"), true, ID_PIC_REPORT_ARBITRAGE, 0}					// 套利报价表
};
static const int KPicCfmInfosCount = sizeof(s_aPicCfmInfos)/sizeof(T_PicCfmInfo);

class CAutoShowWindowHelper
{
public:
	CAutoShowWindowHelper(HWND hWnd, BOOL bHide=FALSE)
	{
		m_hWnd = hWnd;
		if ( bHide )
		{
			ShowWindow(m_hWnd, SW_HIDE);
		}
	}
	~CAutoShowWindowHelper()
	{
		if ( m_hWnd != NULL )
		{
			ShowWindow(m_hWnd, SW_SHOW);
		}
	}

	HWND m_hWnd;
};


IMPLEMENT_DYNAMIC(CMainFrame, CNewMDIFrameWnd)

//lint --e{578}
BEGIN_MESSAGE_MAP(CMainFrame, CNewMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_NCCREATE()
	ON_WM_ERASEBKGND()
	ON_WM_ENTERIDLE()
	ON_COMMAND(ID_WINDOW_NEW, OnWindowNew)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_NEW, OnUpdateWindowNew)
	ON_UPDATE_COMMAND_UI(IDM_WINDOW_CLOSE, OnUpdateCloseChildFrame)					// 锁定一系列窗口分割动作
	ON_UPDATE_COMMAND_UI(ID_SAVE_CUR_CHILDFRAME, OnUpdateSaveChildFrame)
	ON_UPDATE_COMMAND_UI(ID_SAVEAS_CHILDFRAME, OnUpdateSaveChildFrame)
	ON_UPDATE_COMMAND_UI(ID_LOCK_CUR_CHILDFRAME, OnUpdateLockChildFrame)
	ON_UPDATE_COMMAND_UI_RANGE(ID_DEL_VIEW, ID_ADD_RIGHTVIEW, OnUpdateViewSplitCmd)
	ON_UPDATE_COMMAND_UI_RANGE(IDM_IOVIEWBASE_SPLIT_TOP, IDM_IOVIEWBASE_CLOSECUR, OnUpdateViewSplitCmd)
	ON_UPDATE_COMMAND_UI_RANGE(ID_WINDOW_SPLIT_ADD_RIGHT, ID_WINDOW_SPLIT_ADD_BOTTOM, OnUpdateViewSplitCmd)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_SPLIT_DEL, OnUpdateViewSplitCmd)
	ON_UPDATE_COMMAND_UI_RANGE(ID_PIC_BEGIN, ID_PIC_END, OnUpdatePicMenu)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MAIN_MENU_START, ID_MAIN_MENU_END, OnUpdateMainMenu)	// 菜单变更，取什么名字好呢
	ON_UPDATE_COMMAND_UI_RANGE(ID_PIC_CFM_MULTIINTERVAL, ID_PIC_CFM_MULTIMERCH, OnUpdateMainMenu)
	ON_UPDATE_COMMAND_UI_RANGE(ID_ANALYSIS_CJMX, ID_ANALYSIS_TLKX, OnUpdateMainMenu)
	ON_UPDATE_COMMAND_UI_RANGE(ID_USE_HELP, ID_USER_SERVICE, OnUpdateMainMenu)
	ON_WM_GETMINMAXINFO()
	ON_WM_CLOSE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOVING()
	ON_WM_MOVE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_SETTING, OnSetting)
	ON_COMMAND(ID_GRIDHEAD_SET, OnGridheadSet)
	ON_COMMAND(ID_BLOCK_SET, OnBlockSet)
	ON_COMMAND(IDM_BELONG_BLOCK, OnBelongBlockMsg)
	ON_COMMAND(ID_MERCH_NOTEPAD, OnMerchNotePad)
	ON_COMMAND(UM_FIRM_TRADE_SHOW, OnFirmTradeShow)
	ON_COMMAND(UM_SIMULATE_TRADE_SHOW, OnSimulateTradeShow)
	ON_COMMAND(UM_SIMULATE_TRADE_OPENACCOUNT, OnSimulateOpenAccount)
	ON_COMMAND(UM_SIMULATE_TRADE_HQREG, OnSimulateHQRegister)
	ON_COMMAND(UM_SIMULATE_TRADE_HQLOGIN, OnSimulateHQLogin)
	ON_MESSAGE(UM_SIMULATE_TRADE_IMGURL, OnSimulateImageUrl)
	ON_COMMAND(UM_HQ_REGISTER_SUCCED, OnHQRegisterSucced)
	ON_MESSAGE(UM_ReLoginPC, OnWebReLoginPC)
	ON_MESSAGE(UM_PersonCenterRsp, OnWebPersonCenterRsp)
	ON_MESSAGE(UM_WEB_COMMAND, OnWebCommand)
	ON_MESSAGE(UM_WEB_CALL_BACK, OnWebCallBack)
	ON_MESSAGE(UM_NativeOpenCfm, OnWebOpenCfm)
	ON_MESSAGE(UM_LinkageWebTrend, OnWebOpenTrend)
	ON_MESSAGE(UM_ShowWebDlg, OnShowWebDlg)
	ON_MESSAGE(UM_OpenOutWeb, OnOpenOutWeb)
	ON_MESSAGE(UM_HOTKEY, OnMsgHotKey)
	ON_MESSAGE(UM_GridHeader_Change, OnMsgGridHeaderChange)
	ON_MESSAGE(UM_Font_Change, OnMsgFontChange)
	ON_MESSAGE(UM_Color_Change,OnMsgColorChange)
	ON_MESSAGE(UM_MainFrame_OnStatusBarDataUpdate, OnMsgMainFrameOnStatusBarDataUpdate)
	ON_MESSAGE(UM_ViewData_OnTimerFreeMemory, OnMsgViewDataOnTimerFreeMemory)
	ON_MESSAGE(UM_ViewData_OnTimerSyncPushing, OnMsgViewDataOnTimerSyncPushing)
	ON_MESSAGE(UM_ViewData_OnTimerInitializeMarket, OnMsgViewDataOnTimerInitializeMarket)
	ON_MESSAGE(UM_ViewData_OnDataServiceConnected, OnMsgViewDataOnDataServiceConnected)
	ON_MESSAGE(UM_ViewData_OnDataServiceDisconnected, OnMsgViewDataOnDataServiceDisconnected)
	ON_MESSAGE(UM_ViewData_OnDataRequestTimeOut, OnMsgViewDataOnDataRequestTimeOut)
	ON_MESSAGE(UM_ViewData_OnDataCommResponse, OnMsgViewDataOnDataCommResponse)
	ON_MESSAGE(UM_ViewData_OnDataServerConnected, OnMsgViewDataOnDataServerConnected)
	ON_MESSAGE(UM_ViewData_OnDataServerDisconnected, OnMsgViewDataOnDataServerDisconnected)
	ON_MESSAGE(UM_ViewData_OnKLineNodeUpdate, OnMsgViewDataOnKLineNodeUpdate)
	ON_MESSAGE(UM_IndexChsMideCore_KLineUpdate_Resp, OnMsgIndexChsMideCoreKLineUpdate)
	ON_MESSAGE(UM_ViewData_OnTimesaleUpdate, OnMsgViewDataOnTimesaleUpdate)
	ON_MESSAGE(UM_ViewData_OnNewsListUpdate, OnMsgViewDataOnNewsListUpdate)
	ON_MESSAGE(UM_ViewData_OnF10Update, OnMsgViewDataOnF10Update)
	ON_MESSAGE(UM_ViewData_OnLandMineUpdate, OnMsgViewDataOnLandMineUpdate)
	ON_MESSAGE(UM_ViewData_OnNewsTitleUpdate, OnMsgViewDataOnNewsTitleUpdate)//财富资讯2013
	ON_MESSAGE(UM_ViewData_OnNewsContentUpdate, OnMsgViewDataOnNewsContentUpdate)
	ON_MESSAGE(UM_ViewData_OnCodeIndexUpdate, OnMsgViewDataOnCodeIndexUpdate)
	ON_MESSAGE(UM_ViewData_OnNewsPushTitle, OnMsgViewDataOnNewsPushTitle)
	ON_MESSAGE(UM_ViewData_OnPublicFileUpdate, OnMsgViewDataOnPublicFileUpdate)
	ON_MESSAGE(UM_ViewData_OnRealtimePriceUpdate, OnMsgViewDataOnRealtimePriceUpdate)
	ON_MESSAGE(UM_ViewData_OnRealtimeLevel2Update, OnMsgViewDataOnRealtimeLevel2Update)
	ON_MESSAGE(UM_ViewData_OnRealtimeTickUpdate, OnMsgViewDataOnRealtimeTickUpdate)
	ON_MESSAGE(UM_ViewData_OnAllMarketInitializeSuccess, OnMsgViewDataOnAllMarketInitializeSuccess)
	ON_MESSAGE(UM_ViewData_OnDataServerLongTimeNoRecvData, OnMsgViewDataOnDataServerLongTimeNoRecvData)
	ON_MESSAGE(UM_ViewData_OnGeneralNormalUpdate, OnMsgViewDataOnGeneralNormalUpdate)
	ON_MESSAGE(UM_ViewData_OnGeneralFinanaceUpdate, OnMsgViewDataOnGeneralFinanaceUpdate)
	ON_MESSAGE(UM_ViewData_OnMerchTrendIndexUpdate, OnMsgViewDataOnMerchTrendIndexUpdate)
	ON_MESSAGE(UM_ViewData_OnMerchAuctionUpdate, OnMsgViewDataOnMerchAuctionUpdate)
	ON_MESSAGE(UM_ViewData_OnMerchMinuteBSUpdate, OnMsgViewDataOnMerchMinuteBSUpdate)
	ON_MESSAGE(UM_ViewData_OnClientTradeTimeUpdate, OnMsgViewDataOnClientTradeTimeUpdate)
	ON_MESSAGE(0x999, OnMsgDllTraceLog)
	ON_MESSAGE(UM_ViewData_OnAuthPlugInHeart, OnMsgViewDataOnAuthPlugInHeart)
	ON_MESSAGE(UM_Package_LongTime,OnMsgPackageLongTime)
	ON_MESSAGE(UM_Package_TimeOut,OnMsgPackageTimeOut)
	ON_MESSAGE(UM_MainFrame_KickOut,OnMsgMainFrameKickOut)
	ON_MESSAGE(UM_MainFrame_ManualOptimize,OnMsgMainFrameManualOptimize)
	ON_MESSAGE(UM_UpdateSelfDrawBar,OnUpdateSelfDrawBar)
	ON_MESSAGE(UM_MainFrame_ChooseStock_Resp,OnChooseStockResp)
	ON_MESSAGE(UM_MainFrame_DapanState_Resp,OnDapanStateResp)
	ON_MESSAGE(UM_MainFrame_ChooseStockStatus_Resp,OnChooseStockStatusResp)
	ON_MESSAGE(UM_MainFrame_NewStock_Resp, OnNewStockResp)
	
	/////////////////////////////////////////////////////
	//===================CODE CLEAN======================
	//////////////////////////////////////////////////////
	//// 智能选股请求回包
	//ON_MESSAGE(UM_PickModelTypeInfo,OnMsgPickModelTypeInfo)
	//ON_MESSAGE(UM_PickModelTypeStatus,OnMsgPickModelTypeStatus)
	//ON_MESSAGE(UM_PickModelTypeEvent,OnMsgPickModelTypeEvent)

	

	ON_MESSAGE(UM_WEB_KEY_DOWN, OnWebkeyDown)
	ON_MESSAGE(UM_Right_ShowTip,OnShowRightTip)
	ON_MESSAGE(UM_User_Deal_Record,OnAddUserDealRecord)
	ON_MESSAGE(UM_HasIndexRight,OnIsUserHasIndexRight)
	// OnJumpToTradingSoftware
	ON_MESSAGE(UM_JumpToTradingSoftware,OnJumpToTradingSoftware)
	// 资讯
	ON_MESSAGE(UM_ViewData_OnNewsResponse, OnMsgViewDataOnNewsResponse)
	ON_MESSAGE(UM_ViewData_OnNewsServerConnected, OnMsgViewDataNewsServerConnected)
	ON_MESSAGE(UM_ViewData_OnNewsServerDisConnected, OnMsgViewDataNewsServerDisConnected)

	ON_MESSAGE(UM_ViewData_OnAuthFail, OnMsgViewDataOnAuthFail)

	ON_WM_SETTINGCHANGE()
	ON_COMMAND(ID_NEW_WORKSPACE, OnNewWorkspace)
	ON_COMMAND(ID_NEW_CHILDFRAME, OnNewCfm)		// 新建页面文件
	ON_COMMAND(ID_CLOSE_ALL_WINDOW, OnCloseAllWindow)
	ON_COMMAND(ID_OPEN_CHILDFRAME, OnOpenChildframe)
	ON_COMMAND(ID_SAVE_CUR_CHILDFRAME, OnSaveChildFrame)
	ON_COMMAND(ID_LOCK_CUR_CHILDFRAME, OnLockChildFrame)
	ON_COMMAND(ID_OPEN_WORKSPACE, OnOpenWorkspace)
	ON_COMMAND(ID_SAVEAS_CHILDFRAME, OnSaveasChildframe)
	ON_COMMAND(ID_SAVEAS_WORKSPACE, OnSaveasWorkspace)
	ON_COMMAND(ID_SAVE_WORKSPACE, OnSaveWorkspace)
	ON_COMMAND(ID_FORMULA_MAG, OnFormulaMag)
	ON_COMMAND(ID_ADD_UPVIEW, OnAddUpView)
	ON_COMMAND(ID_ADD_DOWNVIEW, OnAddDownView)
	ON_COMMAND(ID_ADD_LEFTVIEW, OnAddLeftView)
	ON_COMMAND(ID_ADD_RIGHTVIEW, OnAddRightView)
	ON_COMMAND(ID_DEL_VIEW, OnDelView)
	ON_COMMAND(IDM_WINDOW_CLOSE, OnCloseChildFrame)
	//ON_COMMAND(ID_DISCONNECT_SERVER, OnDisConnectServer)
	ON_COMMAND(ID_UPDATE, OnUpdate)
	//ON_COMMAND(ID_CONNECT_SERVER, OnConnectServer)
	ON_COMMAND(ID_DOWNLOAD, OnDownLoad)
	ON_COMMAND(ID_CLASS_ROOM, OnMenuClassRoom)
	ON_COMMAND_RANGE(ID_TEACHER_TEAM, ID_LIVE_VIDEO, OnMenuInvestmentAdviser)
	ON_COMMAND_RANGE(ID_NEWS_TRANSFER, ID_RESEARCH_REPORT, OnMenuNews)
	ON_COMMAND_RANGE(ID_DELEGATE_TRADE, ID_TRADE_SETTING, OnMenuTrade)
	ON_COMMAND(IDC_INTERVAL_SORT, OnMenuPhaseSort)
	ON_COMMAND(ID_SYS_AUTO_RUN, OnAutoRun)
	ON_COMMAND(ID_ARBITRAGE, OnArbitrage)
	//ON_COMMAND(ID_CONNECT_NEWSSERVER, OnConnectNewsServer)
	//ON_COMMAND(ID_DISCONNECT_NEWSSERVER, OnDisConnectNewsServer)
	ON_UPDATE_COMMAND_UI_RANGE(ID_DISCONNECT_SERVER, ID_CONNECT_SERVER, OnUpdateConnectServer)
	ON_UPDATE_COMMAND_UI_RANGE(ID_DISCONNECT_NEWSSERVER, ID_CONNECT_NEWSSERVER, OnUpdateConnectServer)
	ON_COMMAND(ID_CONNECT_SETTING, OnConnectSetting)
	ON_COMMAND(ID_CHOOSESIMPLESTOCK, OnChooseSimpleStock)
	ON_COMMAND(ID_SHOW_SHORTCUT_BAR, OnShowShortcutBar)
	ON_WM_TIMER()
	ON_WM_NCPAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_SIZE()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONUP()
	ON_WM_NCHITTEST()
	ON_WM_NCACTIVATE()
	ON_WM_NCMOUSEMOVE()
	ON_WM_NCLBUTTONDBLCLK()
	ON_MESSAGE(GGT_WM_NCMOUSELEAVE, OnNcMouseLeave)
	ON_COMMAND(ID_INFO_NEWS, OnInfoNews)
	ON_COMMAND(ID_VIEW_SDBAR, OnSelfDraw)
	ON_COMMAND(IDR_NET_FLOW, OnNetFlow)
	ON_COMMAND(ID_DLGSYNCEXCEL, OnDlgSyncExcel)
	ON_COMMAND(ID_INFO_SH_BOURSE, OnInfoShBourse)
	ON_COMMAND(ID_INFO_SZ_BOURSE, OnInfoSzBourse)
	ON_COMMAND(ID_INFO_HK_BOURSE, OnInfoHkBourse)
	ON_COMMAND(ID_INFO_F10, OnInfoF10)
	ON_COMMAND(ID_BROKER_WATCH, OnBrokerWatch)
	ON_COMMAND(ID_SELECT_STOCK,OnSelectStock)
	ON_COMMAND(ID_CUSTOM_STOCK,OnCustomStock)
	ON_COMMAND(ID_VIEW_CALC, OnViewCalc)
	ON_COMMAND(ID_INDEX_CHOOSE_STOCK, OnIndexChooseStock)
	ON_COMMAND(ID_VEDIO_TRAINNING, OnVedioTrainning)
	ON_COMMAND(ID_MARKETRADAR, OnMarketRadar)
	ON_COMMAND(ID_MARKETRADAR_ADDRESULT, OnMarketRadarAddResult)
	ON_COMMAND(ID_IWANTBUY, OnIWantBuy)
	ON_UPDATE_COMMAND_UI(ID_MARKETRADAR, OnUpdateMarketRadar)
	ON_MESSAGE(UM_CHECK_CFM_HIDE, OnCheckCfmHide)
	// For Menu
	ON_COMMAND_RANGE(IDM_BLOCKMENU_BEGIN,IDM_BLOCKMENU_END,OnMenuBlock)  
	ON_COMMAND_RANGE(ID_REPORT_TYPEBEGIN,ID_REPORT_TYPEEND,OnMenuBlock)  
	ON_COMMAND_RANGE(BTN_ID_QUOTE_HSAG,BTN_ID_QUOTE_WHSC,OnMenuQuoteMarket)  
	ON_COMMAND_RANGE(ID_PIC_BEGIN,ID_PIC_END,OnPictureMenu)
	ON_COMMAND_RANGE(IDM_STD_BEGIN,IDM_STD_END,OnIoViewMenu)  
	ON_COMMAND_RANGE(IDM_IOVIEWBASE_BEGIN, IDM_IOVIEWBASE_END, OnIoViewMenu)
	ON_COMMAND_RANGE(IDC_CHART_ADD2BLOCK_BEGIN, IDC_CHART_ADD2BLOCK_END, OnIoViewMenu)  
	ON_COMMAND_RANGE(ID_BLOCK_ADDTOFIRST, ID_BLOCK_ADDTO, OnIoViewMenu)
	ON_COMMAND_RANGE(ID_MARK_CANCEL, ID_MARK_MANAGER, OnIoViewMenu)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MARK_CANCEL, ID_MARK_MANAGER, OnUpdateIoViewMenu)
	ON_COMMAND_RANGE(ID_MAIN_MENU_START, ID_MAIN_MENU_END, OnMainMenu)
	ON_COMMAND_RANGE(ID_PIC_CFM_MULTIINTERVAL, ID_PIC_CFM_MULTIMERCH, OnMainMenu)
	ON_COMMAND_RANGE(ID_ANALYSIS_CJMX, ID_ANALYSIS_TLKX, OnMainMenu)
	ON_COMMAND_RANGE(ID_CHART_INTERVALSTATISTIC, ID_CHART_CHANGETAB, OnIoViewMenu)
	ON_COMMAND_RANGE(ID_ZHONGCANGCHIGU, ID_ZHONGCANGCHIGU, OnIoViewMenu)
	ON_COMMAND_RANGE(IDM_SETUPALARM, IDM_SETUPALARM, OnIoViewMenu)
	ON_COMMAND_RANGE(ID_INVESTMENT_DIARY, ID_INVESTMENT_DIARY, OnIoViewMenu)
	ON_COMMAND_RANGE(ID_TRADEEXE_SETTING, ID_TRADEEXE_SETTING, OnMainMenu)
	ON_COMMAND_RANGE(ID_USE_HELP, ID_USER_SERVICE, OnMainMenu)
	ON_COMMAND_RANGE(ID_SPECIAL_INFO_JP, ID_SPECIAL_FUNC_ARBITRAGE,OnMainMenu)
	// IoViewReportMenu
	ON_COMMAND_RANGE(IDM_IOVIEWREPORT_BEGIN,IDM_IOVIEWREPORT_END,OnMenuIoViewReport)
	ON_COMMAND_RANGE(IDM_BLOCK_BEGIN,IDM_BLOCK_END,OnMenuIoViewReport)
	ON_COMMAND_RANGE(IDM_USER1,IDM_USER4,OnMenuIoViewReport)
	ON_COMMAND_RANGE(ID_IOVIEWREPORT_NEW_BEGIN, ID_IOVIEWREPORT_NEW_END, OnMenuIoViewReport)
	ON_COMMAND_RANGE(ID_IOVIEWREPORT_NEW_TOP_BEGIN, ID_IOVIEWREPORT_NEW_TOP_END, OnMenuIoViewReport)
	ON_COMMAND_RANGE(ID_REPORT_AUTOPAGE,ID_REPORT_AUTOPAGESETTING, OnMenuIoViewReport)
	ON_COMMAND_RANGE(ID_REPORT_FOREX_BEGIN, ID_REPORT_FOREX_END, OnMenuIoViewReport)

	// IoViewChart
	ON_COMMAND_RANGE(IDM_CHART_BEGIN, IDM_CHART_END, OnMenuIoViewChart)
	ON_COMMAND_RANGE(IDT_SD_BEGIN, IDT_SD_END,OnMenuIoViewChart)
	ON_COMMAND_RANGE(ID_TICK_CANCELZOOM, ID_TICK_CANCELZOOM, OnMenuIoViewChart)
	ON_COMMAND_RANGE(ID_TICK_SHOWTREND, ID_TICK_SHOWTREND, OnMenuIoViewChart)
	ON_COMMAND_RANGE(ID_TICK_DAY1, ID_TICK_DAY10, OnMenuIoViewChart)
	ON_COMMAND_RANGE(ID_TREND_SHOWVOLCOLOR, ID_TREND_SHOWVOLCOLOR, OnMenuIoViewChart)

	// IoViewKLineMenu
	ON_COMMAND_RANGE(IDM_IOVIEWKLINE_ALLINDEX_BEGIN, IDM_IOVIEWKLINE_ALLINDEX_END, OnMenuIoViewChart)
	ON_COMMAND_RANGE(IDM_IOVIEWKLINE_OFTENINDEX_BEGIN, IDM_IOVIEWKLINE_OFTENINDEX_END, OnMenuIoViewChart)
	ON_COMMAND_RANGE(IDM_IOVIEWKLINE_OWNINDEX_BEGIN, IDM_IOVIEWKLINE_OWNINDEX_END, OnMenuIoViewChart)
	ON_COMMAND_RANGE(IDM_IOVIEWKLINE_EXPERT_BEGIN, IDM_IOVIEWKLINE_EXPERT_END, OnMenuIoViewChart)
	ON_COMMAND_RANGE(IDM_PRE_WEIGHT, IDM_NONE_WEIGHT, OnMenuIoViewChart)
	ON_COMMAND_RANGE(ID_KLINE_ADDCMPMERCH, ID_KLINE_REMOVECMPMERCH, OnMenuIoViewChart)
	ON_COMMAND_RANGE(ID_KLINE_AXISYNORMAL, ID_KLINE_AXISYLOG, OnMenuIoViewChart)
	ON_COMMAND(IDM_COLOR_KLINE_PROMPT, OnAddClrKLine)
	ON_COMMAND(IDM_TRADE_PROMPT, OnAddTrade)
	ON_COMMAND(IDM_REMOVE_ALL_PROMPT, OnDelAllPrompt)
	// IoViewDetail
	ON_COMMAND_RANGE(IDM_IOVIEW_DETAIL_TREND, IDM_IOVIEW_DETAIL_SANE, OnMenuIoViewDetail)
	// IoViewStarry
	ON_COMMAND_RANGE(IDM_IOVIEW_STARRY_START, IDM_IOVIEW_STARRY_END, OnMenuIoViewStarry)
	ON_UPDATE_COMMAND_UI_RANGE(IDM_IOVIEW_STARRY_START, IDM_IOVIEW_STARRY_END, OnMenuUIIoViewStarry)
	// IoViewTrend
	ON_COMMAND_RANGE(ID_TREND_SINGLESECTION, ID_TREND_MULTIDAY10, OnMenuIoViewChart)
	ON_COMMAND_RANGE(ID_TREND_SHOWRFMAX, ID_TREND_SHOWPERCENT, OnMenuIoViewChart)
	ON_COMMAND_RANGE(ID_TREND_SHOWAVG_PRICE, ID_TREND_SHOWAVG_PRICE, OnMenuIoViewChart)
	ON_COMMAND_RANGE(ID_TREND_SHOWTICK, ID_TREND_SHOWTICK, OnMenuIoViewChart)
	// 套利
	ON_COMMAND_RANGE(ID_ARBITRAGE_SETTING, ID_ARBITRAGE_SETTING, OnMenuIoViewChart)
	ON_COMMAND_RANGE(ID_ARBKLINE_CLOSEDIFF, ID_ARBKLINE_END, OnMenuIoViewChart)
	ON_COMMAND_RANGE(IDM_ARBTTRAGE_SELECT, IDM_ARBTTRAGE_SELECT, OnMenuIoViewChart)
	ON_COMMAND_RANGE(IDM_ARBITRAGE_SHOWNEXTCHART, IDM_ARBITRAGE_SHOWNEXTCHART, OnIoViewMenu)
	ON_COMMAND_RANGE(IDM_ARBITRAGE_BEGIN, IDM_ARBITRAGE_END, OnIoViewMenu)
	//
	ON_COMMAND(ID_VIEW_STATUS_BAR, OnViewStatusBar)	
	ON_UPDATE_COMMAND_UI(ID_VIEW_STATUS_BAR, OnUpdateViewStatusBar)	
	//2013-11-14 
	ON_COMMAND(ID_VIEW_COMMENT_BAR, OnViewCommentStatusBar)	
	ON_UPDATE_COMMAND_UI(ID_VIEW_COMMENT_BAR, OnUpdateViewCommentStatusBar)

	ON_COMMAND(ID_VIEW_TB_SPECIAL, OnViewSpecialBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TB_SPECIAL, OnUpdateViewSpecialBar)
	ON_COMMAND_RANGE(ID_SPECIAL_START, ID_SPECIAL_END, OnStockSpecial)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SPECIAL_START, ID_SPECIAL_END, OnStockSpecialUpdateUI)


	// Text
	ON_MESSAGE(WM_SETTEXT, OnSetWindowText)
	ON_MESSAGE(WM_GETTEXT, OnGetWindowText)
	ON_MESSAGE(WM_GETTEXTLENGTH, OnGetWindowTextLength)

	ON_MESSAGE(WM_SHOW_JPNEWS, OnShowJPNews)
	ON_MESSAGE(WM_BROWER_PTR, OnBrowerPtr)
	ON_MESSAGE(WM_JPNEWS_TITLE, OnJPNewsTitle)

	ON_MESSAGE(UM_CLOSE_DLG, OnWebCloseDlg)

	ON_MESSAGE(UM_CloseOnlineServDlg, OnCloseOnlineServDlg)

	//恢复系统页面
	ON_COMMAND(ID_RESTORE_VIEWSP, OnRestoreWorkSpace)

	ON_MESSAGE(UM_FindGGTongView,OnFindGGTongView)
	ON_MESSAGE(UM_TAB_RENAME,OnTabRename)
	ON_MESSAGE(UM_GetNowUseColor,OnGetNowUseColor)
	ON_MESSAGE(UM_GetSysColor,OnGetSysColor)
	ON_MESSAGE(UM_GetSysFontObject,OnGetSysFontObject)
	ON_MESSAGE(UM_GetCenterManager,OnGetCenterManager)
	ON_MESSAGE(UM_GetIDRMainFram,OnGetIDRMainFram)
	ON_MESSAGE(UM_GetStaticMainKlineDrawStyle,OnGetStaticMainKlineDrawStyle)
	ON_MESSAGE(UM_IsShowVolBuySellColor,OnIsShowVolBuySellColor)
	//
	ON_COMMAND(ID_MENU_AUTOCONNECT, OnMenuAutoConnect)
	ON_UPDATE_COMMAND_UI(ID_MENU_AUTOCONNECT, OnUpdateMenuAutoConnect)
	ON_COMMAND(MSG_SCTOOLBAR_MENU, OnMenuShowToolBar)
	ON_UPDATE_COMMAND_UI(MSG_SCTOOLBAR_MENU, OnUpdateMenuShowToolBarStatus)
	ON_COMMAND(ID_EXPORT_DATA,OnExportData)
	ON_MESSAGE(UM_InfoCenter, OnReqInfoCenter)
	//
	ON_MESSAGE(UM_ThirdLoginRsp, OnWebThirdLoginRsp)
	ON_MESSAGE(UM_ToolbarCommand, OnToolbarCommand)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction
bool32 CMainFrame::m_bNeedSetWspAfterInitialMarket = false;

CMainFrame::CMainFrame()
//: m_wndStatusBar(*new CStatusBarEx()),m_wndCommentSBar(*new CCommentStatusBar())
{
	CTabSplitWnd::BindMainFram(this);
	m_wndStatusBar.m_bAutoDelete = FALSE;
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	g_pMainFrame = this;

	m_pDlgbelong            = NULL;
	m_pDlgTrace				= NULL;
	m_pDlgAdjustViewLayout	= NULL;
	m_pDlgKeyBoard			= NULL;
	m_pDlgSpirite			= NULL;
	m_pDlgNetWork			= NULL;
	m_pDlgBrokerWatch		= NULL;
	m_pDlgAlarm				= NULL;
	m_pDlgRecentMerch		= NULL;
	m_pDlgIndexPrompt		= NULL;
	m_pDlgWait				= NULL;
	m_pDlgIm                = NULL;
	m_pDlgIndexChooseStock	= NULL;
	// m_pDlgLoginNotify		= NULL;
	m_pDlgNetFlow			= NULL;
	m_pDlgF10				= NULL;

	m_pActiveGGTViewFromXml = NULL;
	m_pEmptyGGTongView		= NULL;
	m_pHistoryChild			= NULL;
	m_pHistoryTrend			= NULL;

	m_pRecordDataCenter		= NULL;

	m_pMyControlBar			= NULL;

	m_pDlgJPNews			= NULL;

	m_pKBParent				= this;
	m_dwHotKeyTypeFlag		= EHKTCount;	
	m_bShowHistory			= false;

	m_rectCaption			= CRect(0,0,0,0);
	m_rectMin				= CRect(0,0,0,0);
	m_rectMax				= CRect(0,0,0,0);
	m_rectClose				= CRect(0,0,0,0);
	m_ptLastNCLButtonDown   = CPoint(-1,-1);

	m_rectLast.left			= 127;
	m_rectLast.right		= 800;
	m_rectLast.top			= 138;
	m_rectLast.bottom		= 776;

	m_iRandIndex			= 0;
	m_aiRandomNums.RemoveAll();

	m_uiConnectServerTimers = 0;
	GenerateRandomNums();
	if(pApp->m_pConfigInfo->m_firstLogin)
		SetAutoRun(TRUE);
	else
		GetAutoRunKeyValue();
	m_bForceFixKeyBoard		= false;
	m_bHaveUpdated			= false;

	m_bFirstDisplayFromXml	= true;
	m_bQuickOrderDn = false;

	m_pNewWndTB = NULL;
	m_pNewCaptionWndTB = NULL;

	m_pImgLogo = NULL;	
	m_pImgLogoLeft = NULL;	
	m_pImgMenuBtn1 = NULL;
	m_pImgSysBtn = NULL;
	m_pImgHideBtn = NULL;
	m_pImgShowBtn = NULL;
	m_pImgSpliter = NULL;
	m_pImageCaption = NULL;
	m_pImageUserAccount = NULL;
	m_pImageLogin = NULL;
	m_pImgUserPhoto = NULL;
	m_pImgMenuExit = NULL;
	m_pImgMenuHelp = NULL;
	m_pImgMenuAlarm = NULL;
	m_pImgMenuSelStock = NULL;
	m_pImgMenuSystem = NULL;
	m_pImgMenuTrade = NULL;	
	m_pImgLoginSpliter = NULL;
	//下拉菜单图片
	 m_pImgCenterSplitBottom = NULL;
	 m_pImgCenterSplitTop = NULL;
	 m_pImgHelpBottom = NULL;
	 m_pImgHelpTop = NULL;
	 m_pImgItemArrow = NULL;
	 m_pImgItemSimple = NULL;
	 m_pImgSysBottom = NULL;
	 m_pImgSysTop = NULL;
	 m_pImgTradeTop = NULL;

	InitialImageResource();

	m_bFromXml	= false;


	m_bWindowTextValid = false;
	m_bManualReconnect = false;

	m_dwUserConnectCmdFlag = (DWORD)(~0);	// 所有标志打开

	m_uiConnectNewsServerTimers = 0;

	m_pTradeContainer		= NULL;// lcq add
	m_pSimulateTrade		= NULL;

	if ( s_libHelper.m_pProg != NULL )	// 初始化帮助资源
	{
		(*s_libHelper.m_pProg)(NULL, NULL, 0x1C/*HH_INITIALIZE*/, (DWORD)&s_dwCookie);
	}

	m_RectOld = CRect(0, 0, 0, 0);

	//
	m_pDlgAdvertise = NULL;
	m_iXMenuBtnHovering = INVALID_ID;
	m_iXSysBtnHovering	= INVALID_ID;
	m_pDlgAccount = NULL;
	m_pDlgWebContent = NULL;
	m_pDlgPushMsg = NULL;
	m_pDlgRegister = NULL;
	//m_pMenuBar	= NULL;
	m_LastTimeSet = 0;

	m_nFrameWidth = 0;
	m_nFrameHeight = 0;
	m_nTitleLength = 0;
	m_bStartEconoReq = false;
	m_nReqTime = 0;
	m_pDlgComment = NULL;
	m_bDoLogin = false;
	m_bLoginSucc = false;
	m_bShowLoginDlg = false;

	m_pDlgPullDown	   = NULL;
	m_mapMenuCollect.clear();

	m_fontMenuItem.m_StrName = L"SimSun";
	m_fontMenuItem.m_Size = 8.7f;
	m_fontMenuItem.m_iStyle = FontStyleRegular;
	m_mapPullDownBtns.clear();

	/////////////////////////////////////////////////////
	//===================CODE CLEAN======================
	//////////////////////////////////////////////////////
	/*m_mapPickModelTypeGroup.clear();
	m_vPickModelTypeGroup.clear();
	m_mapPickModelStatusGroup.clear();*/
	m_iSlectModelId = INVALID_ID;

	InitMapMenuItem();
}

CMainFrame::~CMainFrame()
{

	XLTraceFile::GetXLTraceFile(_T("")).Trace(_T(""));

	//
	DEL(m_pDlgAdvertise); //必须在RealeseGdiPlus 之前广告

	DEL(m_pImgMenuBtn1);
	DEL(m_pImgSysBtn);  
	DEL(m_pImgLogo);	  
	DEL(m_pImgLogoLeft);
	DEL(m_pImgHideBtn); 
	DEL(m_pImgShowBtn); 
	DEL(m_pImgSpliter); 
	DEL(m_pImageUserAccount);
	DEL(m_pImageLogin); 
	DEL(m_pImageCaption);
	DEL(m_pImgUserPhoto);
	DEL(m_pImgMenuExit);
	DEL(m_pImgMenuHelp);
	DEL(m_pImgMenuAlarm);
	DEL(m_pImgMenuSelStock);
	DEL(m_pImgMenuSystem);
	DEL(m_pImgMenuTrade);
    DEL(m_pImgLoginSpliter);
	//下拉菜单图片
	DEL(m_pImgCenterSplitBottom);
	DEL(m_pImgCenterSplitTop);
	DEL(m_pImgHelpBottom);
	DEL(m_pImgHelpTop);
	DEL(m_pImgItemArrow);
	DEL(m_pImgItemSimple);
	DEL(m_pImgSysBottom);
	DEL(m_pImgSysTop);
	DEL(m_pImgTradeTop);


	CGGTongApp * pApp = (CGGTongApp*)AfxGetApp();
	pApp->RealeseGdiPlus();
	//
	if ( NULL != m_pSubject )
	{
		m_pSubject->DelObserver(this);
	}

	if ( NULL != m_pRecordDataCenter )
	{
		m_pRecordDataCenter->StopWork();
		DEL(m_pRecordDataCenter);
	}

	CIoViewSyncExcel::UnInitialExcelResource();	// 释放没有释放的Excel同步资源

	// 删除板块配置资源
	CBlockConfig::DeleteInstance();
	//CSysBlockManager::DelInstance();
	//CUserBlockManager::DelInstance();

	if (NULL != m_pDlgbelong)
	{
		m_pDlgbelong = NULL;
	}

	if ( s_libHelper.m_pProg != NULL )	// 释放帮助资源
	{
		(*s_libHelper.m_pProg)(NULL, NULL, 0x1D/*HH_UNINITIALIZE*/, (DWORD)&s_dwCookie);
	}

	DEL(m_pMyControlBar);
	DEL(m_pDlgJPNews);
	DEL(m_pTradeContainer);
	DEL(m_pSimulateTrade);
	DEL(m_pNewWndTB);
	DEL(m_pDlgAccount);
	DEL(m_pDlgWebContent);
	DEL(m_pDlgPushMsg);
	DEL(m_pDlgF10);
	//DEL(m_pMenuBar);
	DEL(m_pDlgComment);

	DEL(m_pDlgPullDown);
	DEL(m_pImgMenu);
	DEL(m_pImgArrowMenu);
	DEL(m_pNewCaptionWndTB);
	CFormulaLib::DelInstance();

#ifdef _DEBUG  // debug下释放doc资源，不知道为什么app的exitinstance进不去
	{
		// 		CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
		// 		if ( NULL != pApp )
		// 		{
		// 			pApp->m_pDocManager->CloseAllDocuments(TRUE);		// 解除doc关系，释放doc资源
		// 			delete pApp->m_pDocument;
		// 			pApp->m_pDocument = NULL;
		// 			delete pApp->m_pDocManager;
		// 			pApp->m_pDocManager = NULL;
		// 		}
	}
#endif
}

void CMainFrame::InitialImageResource()
{
	m_eCurrentShowState		= ECSSNormal;
	m_pImageCaption			= NULL;

	m_pImgMenuBtn1 = Image::FromFile(L"image//menu_btn1.png");
	m_pImgSysBtn   = Image::FromFile(L"image//Tab_MainSysMenu.png");
	m_pImgLogo	   = Image::FromFile(L"image//Captionlogo.png");
	m_pImgLogoLeft = Image::FromFile(L"image//logo.png");
	m_pImgHideBtn  = Image::FromFile(L"image//hide.png");
	m_pImgShowBtn  = Image::FromFile(L"image//show.png");
	m_pImgSpliter  = Image::FromFile(L"image//spliter.png");
	m_pImageUserAccount = Image::FromFile(L"image//Attention.png");
	m_pImageLogin = Image::FromFile(L"image//MainRegister.png");
    m_pImgMenuExit = Image::FromFile(L"image//menuExit.png");
	m_pImgMenuHelp = Image::FromFile(L"image//menuHelp.png");
	m_pImgMenuAlarm = Image::FromFile(L"image//menuAlarm.png");
	m_pImgMenuSelStock = Image::FromFile(L"image//menuSelectStock.png");
	m_pImgMenuSystem = Image::FromFile(L"image//menuSystem.png");
	m_pImgMenuTrade = Image::FromFile(L"image//menuTrade.png");
	m_pImgMenu = Image::FromFile(L"image//menuItem.png");
	m_pImgArrowMenu = Image::FromFile(L"image//menuArrowItem.png");
	m_pImgLoginSpliter = Image::FromFile(L"image//loginSpliter.png");

	//下拉菜单图片
	m_pImgCenterSplitBottom = Image::FromFile(L"image//mainMenu//CenterSplitBottom.png");
	m_pImgCenterSplitTop = Image::FromFile(L"image//mainMenu//CenterSplitTop.png");
	m_pImgHelpBottom = Image::FromFile(L"image//mainMenu//HelpBottom.png");
	m_pImgHelpTop = Image::FromFile(L"image//mainMenu//HelpTop.png");
	m_pImgItemArrow = Image::FromFile(L"image//mainMenu//ItemArrow.png");
	m_pImgItemSimple = Image::FromFile(L"image//mainMenu//ItemSimple.png");
	m_pImgSysBottom = Image::FromFile(L"image//mainMenu//SysBottom.png");
	m_pImgSysTop = Image::FromFile(L"image//mainMenu//SysTop.png");
	m_pImgTradeTop = Image::FromFile(L"image//mainMenu//TradeTop.png");

}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CNewMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 非游客并且配置了开关true, 启动用户行为记录
	CGGTongApp *pApp = (CGGTongApp*) AfxGetApp();	
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();

	//
	m_sysMenu.LoadMenu(IDR_MAINFRAME);
	//CreateAdvertise();
	//
	EnableToolTips(TRUE);


	if(NULL == m_pDlgPushMsg)
	{
		m_pDlgPushMsg = new CDlgPushMessage(this);
		m_pDlgPushMsg->Create(IDD_DIALOG_PUSH_MESSAGE, this);	
	}

	if(NULL == m_pDlgComment)
	{
		m_pDlgComment = new CDlgNewCommentIE(this);
		m_pDlgComment->Create(IDD_DIALOG_NEW_COMMENT, this);
	}


	// 子类化.重绘MainFrame 的背景
	if (!m_wndMDIClient.SubclassWindow (m_hWndMDIClient))	// Add
	{														// Add
		TRACE0("Failed to subclass MDI client window\n");   // Add
		return (-1);                                        // Add
	} 

	if ( !m_wndReBar.Create(this, RBS_VARHEIGHT ) )
	{
		TRACE0("Failed to create rebar\n");
		return -1;      // fail to create
	}

	//if ( !m_wndReBar2.Create(this, RBS_VARHEIGHT ) )
	//{
	//	TRACE0("Failed to create rebar\n");
	//	return -1;      // fail to create
	//}
	//	ModifyStyle(WS_CAPTION,0);

	// 使StatusBar 右下角没有SIZEGRIP,以防止最大化时能够拖动
	ModifyStyle(WS_THICKFRAME,0);  

	ASSERT( NULL == m_pMyControlBar );
	m_pMyControlBar = new CMyContainerBar;
	m_pMyControlBar->Create(_T("浮动栏"), WS_CHILD|CBRS_ALIGN_BOTTOM|WS_VISIBLE|CBRS_SIZE_DYNAMIC, this);

	// 初始化条件预警
	CGGTongDoc *pDocument = pApp->m_pDocument;	
	ASSERT(NULL!= pDocument);
	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return -1;
	}
	CAlarmCenter* pAlarmCenter = pDocument->m_pAarmCneter;
	if (NULL == pAlarmCenter)
	{
		pDocument->m_pAarmCneter = new CAlarmCenter(pAbsCenterManager);
		pAlarmCenter = pDocument->m_pAarmCneter;
	}
	// 
	if (!m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	// m_wndStatusBar.ModifyStyle(SBARS_SIZEGRIP,0); 状态栏去掉这个标志. 没有效果?
	ModifyStyle(0,WS_THICKFRAME);  


	//金评滚动条 2013-11-12 by cym 
	m_wndCommentSBar.Create(this);
	//m_wndCommentSBar.SetStatusTimer();

	if (pApp->m_bOffLine)
	{
		ShowControlBar(&(m_wndCommentSBar), FALSE, 0);
	}
	m_DefaultNewMenu.LoadToolBar(g_awToolBarIconIDs);

	m_pMyControlBar->EnableDocking(CBRS_ALIGN_BOTTOM);
	{
		// 获取比较合适的高度
		CSize sizeDef(700, 330);
		CRect rcWorkArea(0,0,0,0);
		if ( SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0) )
		{
			sizeDef.cy	= 330;	// 默认是330像素高
			sizeDef.cx	= int((float)sizeDef.cy * 2.618f);
		}		
		m_pMyControlBar->SetSizeDefault(sizeDef);
	}
	EnableMyDocking(CBRS_ALIGN_ANY);
	DockControlBar(m_pMyControlBar);
	ShowControlBar(m_pMyControlBar, FALSE, TRUE);	// 默认隐藏辅助区

	//
	CString StrIconPath = CPathFactory::GetImageMainIcon32Path();

	HICON hIcon = (HICON)LoadImage(AfxGetInstanceHandle(), StrIconPath, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE|LR_LOADFROMFILE);

	if ( NULL == hIcon )
	{
		hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
		SetIcon(hIcon,TRUE);
		SetIcon(hIcon,FALSE);
	}
	else
	{
		SetIcon(hIcon,TRUE);
		SetIcon(hIcon,FALSE);
		SetClassLong(m_hWnd, GCL_HICON, (LONG)hIcon);
	}

	//
	CString StrHwnd;
	StrHwnd.Format(L"%d", (long)GetSafeHwnd());
	::WritePrivateProfileString(L"TRACE2", L"HWND",	StrHwnd, L"./trace.ini");
	_MYTRACE(L"MainFrame::m_hWnd: %d", GetSafeHwnd());

	int iToolBarHeight = 0;
	if(pApp)
	{
		iToolBarHeight = pApp->m_pConfigInfo->m_toolBarData.m_iHeight;
	}

	m_pNewWndTB = new CNewTBWnd(*this);
	m_pNewWndTB->Create(NULL, _T("tb"), WS_CHILD | WS_VISIBLE, CRect(0,0,iToolBarHeight,iToolBarHeight), this, 0x1234);
	m_wndReBar.AddBar(m_pNewWndTB,NULL,NULL,RBBS_NOGRIPPER|~CBRS_BORDER_LEFT|~CBRS_BORDER_RIGHT|~CBRS_BORDER_ANY);

	REBARBANDINFO rbbi = {0};
	rbbi.cbSize = sizeof(rbbi);
	rbbi.fMask = RBBIM_CHILD | RBBIM_COLORS | RBBIM_HEADERSIZE | RBBIM_STYLE  |RBBIM_CHILDSIZE;
	rbbi.hwndChild = m_pNewWndTB->GetSafeHwnd();
	rbbi.clrBack = RGB(255,0,0);
	rbbi.clrFore = RGB(0,0,0);
	rbbi.cxHeader = (UINT)-2;
	rbbi.fStyle = RBBS_NOGRIPPER /*|RBBS_FIXEDSIZE*/ ;   //RBBS_HIDETITLE
	rbbi.cxMinChild = 0;
	rbbi.cyMinChild = iToolBarHeight;//29;//24;
	m_wndReBar.GetReBarCtrl().SetBandInfo(0, &rbbi);
	m_wndReBar.SetBarStyle(m_wndReBar.GetBarStyle() & ~(CBRS_BORDER_TOP)); 

	m_pNewCaptionWndTB = new CNewCaptionTBWnd(*this);
	m_pNewCaptionWndTB->Create(NULL, _T("funtb"), WS_CHILD | WS_VISIBLE, CRect(0,0,30,30), this, 0x2345);
	//m_wndReBar2.AddBar(m_pNewCaptionWndTB,NULL,NULL,RBBS_NOGRIPPER|~CBRS_BORDER_LEFT|~CBRS_BORDER_RIGHT|~CBRS_BORDER_ANY);
	////m_wndReBar.AddBar(m_pNewFunWndTB,NULL,NULL,RBBS_NOGRIPPER);
	////m_wndReBar.SetBarStyle(m_wndReBar.GetBarStyle() & ~(CBRS_BORDER_TOP)); 
	//rbbi.hwndChild = m_pNewCaptionWndTB->GetSafeHwnd();
	//rbbi.fStyle = RBBS_NOGRIPPER |RBBS_BREAK ;
	//rbbi.cyMinChild = 20;
	//m_wndReBar2.GetReBarCtrl().SetBandInfo(0, &rbbi);	
	//m_wndReBar2.SetBarStyle(m_wndReBar2.GetBarStyle() & ~(CBRS_BORDER_TOP)); 


	//if (!m_ShortCutMenuToolBar.CreateEx(this, TBSTYLE_FLAT , WS_CHILD | CBRS_LEFT
	//	| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	//	!m_ShortCutMenuToolBar.LoadToolBar(IDR_TOOLBAR_SHORTCUT))
	//{
	//	TRACE0("Failed to create toolbar\n");
	//	DWORD dwRet = GetLastError();
	//	return -1;      // fail to create
	//}

	//// 设置侧边栏状态
	//if (pDoc->m_bShowSCToolBar)
	//{
	//	ShowControlBar(&m_ShortCutMenuToolBar, TRUE , TRUE);
	//}
	//else
	//{
	//	ShowControlBar(&m_ShortCutMenuToolBar, false , TRUE);
	//}

	if (!m_leftToolBar.CreateEx(this, TBSTYLE_FLAT , WS_CHILD | CBRS_LEFT
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_leftToolBar.LoadToolBar(IDR_TOOLBAR_LEFT))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}


	int	iRightBarHigh = 223;
	m_leftToolBar.SetSizes(CSize(iRightBarHigh,7),CSize(1,1));




	// TradeContainer// lcq add
	m_pTradeContainer = new CTradeContainerWnd(ETT_TRADE_FIRM); // 实盘交易 
	m_pTradeContainer->Create(_T("trade"), WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN, CRect(0,0,0,0), this, 0x1258);
	AddDockSubWnd(m_pTradeContainer);

	m_pSimulateTrade = new CTradeContainerWnd(ETT_TRADE_SIMULATE);	// 模拟交易
	m_pSimulateTrade->Create(_T("STradeInfo"), WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN, CRect(0,0,0,0), this, 0x1259);
	AddDockSubWnd(m_pSimulateTrade);

	LoadPicMenu();
	SendMessage(WM_NCPAINT, 0, 0);

	//2013-12-10	
	//if (NULL == m_pDlgIm && pApp->m_pConfigInfo->m_bImVersion)	已经取消分析师功能了
	//{
	//	m_pDlgIm = new CDlgIm;
	//	m_pDlgIm->Create(IDD_DIALOG_IM,this);
	//}

	if (NULL != pApp && !pApp->m_pConfigInfo->m_bMember)
	{
		OnViewCommentStatusBar();
	}

	m_nFrameWidth = GetSystemMetrics(SM_CXFRAME);// - GetSystemMetrics(SM_CXBORDER);
	m_nFrameHeight = GetSystemMetrics(SM_CYFRAME);// - GetSystemMetrics(SM_CYBORDER);

	// 创建菜单栏
	//CRect rcWnd, rcMenu;
	//int32 iXPos = 0;
	//if (NULL != m_pImgLogoLeft)
	//{
	//	iXPos = (m_pImgLogoLeft->GetWidth()>TITLE_HEIGHT)?m_pImgLogoLeft->GetWidth():TITLE_HEIGHT;
	//}
	//iXPos += (m_nFrameWidth + m_nTitleLength + 5);
	//int iWidth = 46;
	//int iHeight = 18;
	//if(m_pImgMenuBtn1 && m_pImgMenuBtn1->GetLastStatus() == Ok)
	//{
	//	iWidth = m_pImgMenuBtn1->GetWidth();
	//	iHeight = m_pImgMenuBtn1->GetHeight() / 3;
	//}
	//GetWindowRect(&rcWnd);
	//rcWnd.OffsetRect(-rcWnd.left, -rcWnd.top);
	//rcMenu.left   = rcWnd.left + iXPos + 220;
	//rcMenu.top    = m_nFrameWidth + (TITLE_HEIGHT - iHeight) / 2 + 1;
	//rcMenu.right  = rcMenu.left + iWidth;
	//rcMenu.bottom = rcMenu.top + iHeight;
	//m_pMenuBar = new CMenuBar();
	//m_pMenuBar->CreateMenuBar(this, IDR_MAINFRAME, rcMenu, NULL, m_pImgMenuBtn1);


	ElapseLogin(KTimerPeriodStartUpLogin);	

	if (NULL == m_pDlgPullDown)
	{
		m_pDlgPullDown = new CDlgPullDown(this);
		m_pDlgPullDown->Create(IDD_DIALOG_PULLDOWN, this);
	}

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	m_strTitle = AfxGetApp()->m_pszAppName;		// 重设标题

	cs.style &= ~FWS_ADDTOTITLE &~WS_SYSMENU;
	//cs.style |= FWS_PREFIXTITLE;
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	// TODO: Modify the Window class or styles here by modifying
	// the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}
void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnConnectServer()
{
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if ( NULL == pDoc || !pDoc->m_pAbsDataManager || !pDoc->m_pAbsCenterManager)
	{
		return;
	}

	// 设置用户选择标志
	SetUserConnectCmdFlag(0, EUCCF_ConnectDataServer);
	pDoc->m_pAbsCenterManager->ConnectQuoteServer(pDoc->m_eNetType, this);
}

void CMainFrame::OnDownLoad()
{
	CDlgDownLoad Dlg(false);
	Dlg.DoModal();
}

void CMainFrame::OnDisConnectServer()
{
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();

	if ( NULL != pDoc && NULL!=pDoc->m_pAbsCenterManager )
	{
		SetUserConnectCmdFlag(EUCCF_ConnectDataServer, 0);	// 移出连接标志
		pDoc->m_pAbsCenterManager->DisconnectQuoteServer();
	}
}

void CMainFrame::OnUpdate()
{
	/*
	if ( m_bHaveUpdated )
	{
	MessageBox(L"系统已更新.下次进入时生效.", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
	return;
	}
	*/
	// 启动自动更新程序:
	TCHAR TStrDir[MAX_PATH];	
	::GetCurrentDirectory(MAX_PATH, TStrDir);

	STARTUPINFO StartInfo = {sizeof(StartInfo)};
	PROCESS_INFORMATION ProcessInfo;

	if( !::CreateProcess(L"AutoUpdate.exe", L"GGTong", NULL, NULL, false, 0, NULL, TStrDir, &StartInfo, &ProcessInfo) )
	{
		MessageBox(L"启动自动更新程序失败!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		m_bHaveUpdated = false;
		return;
	}

	//m_bHaveUpdated = true;
	DEL_HANDLE(ProcessInfo.hThread);
	DEL_HANDLE(ProcessInfo.hProcess);
}

void CMainFrame::OnClose()
{
	//
	CloseWorkSpace();
}

void CMainFrame::OnDestroy()
{

	CIoViewSyncExcel::DelInstance();

	if (NULL != m_pDlgTrace)
	{
		m_pDlgTrace->DestroyWindow();
		DEL(m_pDlgTrace);
	}

	// 停止雷达计算
	CMarketRadarCalc::Instance().StopCalc();
	return;

	// 删除菜单报价项目item data
	CMenu* pMenu = this->GetMenu();
	if ( NULL != pMenu )
	{
		// 移出分析菜单
		const int32 iMenuCount = pMenu->GetMenuItemCount();
		for ( int32 i=0; i < iMenuCount ; i++ )
		{
			CString StrMenu;
			pMenu->GetMenuString(i, StrMenu, MF_BYPOSITION);
			if ( StrMenu == _T("分析(&F)") )
			{
				pMenu->RemoveMenu(i, MF_BYPOSITION);	// 有menu对象负责删除，这里只要remove
				break;
			}
		}
	}
	CNewMenu* pFrameMenu = DYNAMIC_DOWNCAST(CNewMenu, pMenu);
	if ( NULL != pFrameMenu )
	{
		// 板块菜单
		pMenu = pFrameMenu->GetSubMenu(_T("板块(&B)"));
		CNewMenu* pRootMenu = DYNAMIC_DOWNCAST(CNewMenu, pMenu);	
		if ( NULL != pRootMenu )
		{
			const int32 iMenuDirCount = pRootMenu->GetMenuItemCount();
			for ( int32 i=0; i < iMenuDirCount ; i++ )
			{
				CNewMenu *pSubMenu = DYNAMIC_DOWNCAST(CNewMenu, pRootMenu->GetSubMenu(i));
				if ( NULL == pSubMenu )
				{
					continue;
				}

				DeleteBlockReportItemData(pSubMenu);
			}
		}


		// 3: 分析菜单 - 分析菜单使用的是板块中的指针

		//CMenu * pSubMenuF = pFrameMenu->GetSubMenu(L"分析(&F)");
		// 		CMenu * pSubMenuF = GetAnalysisMenu(false);
		// 		ASSERT( NULL != pSubMenuF );
		// 		CNewMenu * pRootMenuF = DYNAMIC_DOWNCAST(CNewMenu,pSubMenuF);
		// 		ASSERT( NULL != pRootMenuF );
		// 		if ( NULL != pRootMenuF )
		// 		{
		// 			CString StrMenuName = CIoViewManager::FindIoViewObjectByRuntimeClass(RUNTIME_CLASS(CIoViewReport))->m_StrLongName;
		// 			CMenu* pSubMenuReport = pRootMenuF->GetSubMenu(StrMenuName);
		// 			ASSERT(NULL != pSubMenuReport);
		// 			CNewMenu* pRootMenuReport = DYNAMIC_DOWNCAST(CNewMenu, pSubMenuReport);
		// 			ASSERT(NULL != pRootMenuReport);
		// 			if ( NULL != pRootMenuReport )
		// 			{
		// 				// 报价表下面所有的删除
		// 				for ( int32 i = 0; i < pRootMenuReport->GetMenuItemCount(); i++ )
		// 				{
		// 					CNewMenu *pDelUserMenu = DYNAMIC_DOWNCAST(CNewMenu, pRootMenuReport->GetSubMenu(i));
		// 					DeleteBlockReportItemData(pDelUserMenu);
		// 				}
		// 			}
		// 		}

		//

	}

	CMDIFrameWnd::OnDestroy();
}

void CMainFrame::NewDefaultChildFrame(bool32 /*bBlank*/)
{   
	OnWindowNew();
}

void CMainFrame::OnWindowNew() 
{
	//////////////////////////////////////////////////////////////////////////
	bool32 bMax = false;
	CWnd * pChild = CWnd ::FromHandle(m_hWndMDIClient)->GetWindow(GW_CHILD);
	if (pChild)
	{
		CMPIChildFrame * pChilds =(CMPIChildFrame * )GetActiveFrame();

		if (pChilds->GetMaxSizeFlag())
		{
			bMax = true;
		}		
	}

	//////////////////////////////////////////////////////////////////////////
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();

	POSITION   pos  = pApp->m_pDocTemplate->GetFirstDocPosition();

	if ( NULL == pos)
	{
		pApp->m_pDocument = (CGGTongDoc *)pApp->m_pDocTemplate->CreateNewDocument();
		pos  = pApp->m_pDocTemplate->GetFirstDocPosition();
	}


	CRect RectActive = CalcActiveChildSize();

	CMPIChildFrame * pChildFrame = pApp->m_pDocTemplate->CreateMPIFrame(IDR_MAINFRAME);

	// 初始化xml
	TiXmlElement *pChildEle = NULL;
	TiXmlDocument myDocument;
	{
		// 使用默认
		CString StrDefault = pChildFrame->GetDefaultXML(true, RectActive);

		//USES_CONVERSION;
		//const char * KStrDefault = W2A(StrDefault);

		//fangz0714No.3
		char KStrDefault[10240];
		UNICODE_2_MULTICHAR(EMCCUtf8,StrDefault,KStrDefault);
		myDocument.Parse(KStrDefault);
		pChildEle = myDocument.FirstChildElement();
	}
	ASSERT( NULL != pChildEle );

	pChildFrame->FromXml(pChildEle);	
	pChildFrame->SetActiveGGTongView();
	pChildFrame->SetChildFrameTitle(AfxGetApp()->m_pszAppName);

	//  子窗口最大化,在fromxml 中已经处理过,这里不需要在处理
	if (m_bShowHistory && NULL == GetHistoryChild())
	{
		pChildFrame->CenterWindow();
	}
	else
	{
		// 打开的子窗口尽量保持最大化
		if(bMax && !pChildFrame->GetMaxSizeFlag())
		{
			pChildFrame->PostMessage(KMsgChildFrameMaxSize,0,0);
		}
	}
}

void CMainFrame::OnUpdateWindowNew(CCmdUI* pCmdUI) 
{

} 

CString CMainFrame::GetDefaultXML()
{
	CString StrRootElement;
	StrRootElement  = L"<?xml version =\"1.0\" encoding=\"UTF-8\"?> \n";
	StrRootElement += L"<XMLDATA version=\"1.0\" app = \"ggtong\" data = \"WorkSpace\"> \n";

	CString StrDefaultXML;
	CString StrWinCounts, StrZIndex, StrFlag, StrShowCmd;
	CString StrPtMinPosX, StrPtMinPosY, StrPtMaxPosX, StrPtMaxPosY;
	CString StrNomPosleft, StrNomPosright, StrNomPostop, StrNomPosbottom;

	StrWinCounts		= L"1";
	StrFlag				= L"1";
	StrShowCmd			= L"1";
	StrPtMinPosX		= L"-1";
	StrPtMinPosY		= L"-1";
	StrPtMaxPosX		= L"-1";
	StrPtMaxPosY		= L"-1";  
	StrNomPosleft.Format(_T("%d"),KMainFrameDefaultLeft);
	StrNomPosright.Format(_T("%d"),KMainFrameDefaultRight);
	StrNomPostop.Format(_T("%d"),KMainFrameDefaultTop);
	StrNomPosbottom.Format(_T("%d"),KMainFrameDefaultBottom);
	//lint -e{437}
	StrDefaultXML.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\"  %s=\"%s\">\n",
		CString(KStrElementValue),
		CString(KStrWindowCounts), 
		StrWinCounts,
		CString(KStrElementAttriFlag),
		StrFlag,
		CString(KStrElementAttriShowCmd),
		StrShowCmd,			  
		CString(KStrElementAttriPtMinX),
		StrPtMinPosX,
		CString(KStrElementAttriPtMinY),
		StrPtMinPosY,
		CString(KStrElementAttriPtMaxX),
		StrPtMaxPosX,
		CString(KStrElementAttriPtMaxY),
		StrPtMaxPosY,
		CString(KStrElementAttriPtNorLeft),
		StrNomPosleft,
		CString(KStrElementAttriPtNorRight),
		StrNomPosright,
		CString(KStrElementAttriPtNorTop),
		StrNomPostop,
		CString(KStrElementAttriPtNorBottom),
		StrNomPosbottom);

	StrDefaultXML += CMPIChildFrame::GetDefaultXML(false,CRect(0,0,0,0));

	StrDefaultXML += L"</";
	StrDefaultXML += CString(KStrElementValue);
	StrDefaultXML += L">\n";

	StrRootElement += StrDefaultXML;
	StrRootElement += L"</XMLDATA>";

	return StrRootElement;
}

bool32 CMainFrame::FromXml( const char * pKcXml )
{
	m_bFromXml	 = true;

	if (NULL == m_pDlgWait)
	{
		m_pDlgWait = new CDlgWait;
		m_pDlgWait->Create(IDD_DIALOG_WAIT,this);
	}

	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	if (NULL == pApp)
		return false;

	TiXmlDocument myDocument = TiXmlDocument(pKcXml);
	if (!myDocument.LoadFile())
		return false;


	TiXmlElement *pRootElement = myDocument.RootElement();
	ASSERT(NULL!=pRootElement);
	pRootElement = pRootElement->FirstChildElement();
	if (NULL == pRootElement)
		return false;

	const char * pcValue = pRootElement->Value();
	if (NULL == pcValue || strcmp(KStrElementValue, pcValue) != 0)
		return false;

	// 主窗口参数 
	// 窗口数量.
	int32 iWindowCounts = 0;
	const char *pcAttrValue = pRootElement->Attribute(KStrWindowCounts);
	if (NULL != pcAttrValue)
		iWindowCounts = atoi(pcAttrValue);

	// 自身属性
	WINDOWPLACEMENT aWp[256];
	memset(aWp, 0, sizeof(aWp));

	aWp[0].length = sizeof(WINDOWPLACEMENT);

	pcAttrValue	= pRootElement->Attribute("flag");
	if (NULL != pcAttrValue)		aWp[0].flags = atoi(pcAttrValue);

	pcAttrValue	= pRootElement->Attribute("showCmd");
	if (NULL != pcAttrValue)		aWp[0].showCmd = atoi(pcAttrValue);

	// 程序启动时，应当全屏，后续再次打开工作版面都不应当修改当前窗口的大小
	// 修改策略：第一次全屏，后续打开版面不在调用SetWindowPlacement
	aWp[0].showCmd = SW_SHOWMAXIMIZED;

	pcAttrValue	= pRootElement->Attribute("ptMinPosition.x");
	if (NULL != pcAttrValue)		aWp[0].ptMinPosition.x = atol(pcAttrValue);

	pcAttrValue	= pRootElement->Attribute("ptMinPosition.y");
	if (NULL != pcAttrValue)		aWp[0].ptMinPosition.y = atol(pcAttrValue);

	pcAttrValue	= pRootElement->Attribute("ptMaxPosition.x");
	if (NULL != pcAttrValue)		aWp[0].ptMaxPosition.x = atol(pcAttrValue);

	pcAttrValue	= pRootElement->Attribute("ptMaxPosition.y");
	if (NULL != pcAttrValue)		aWp[0].ptMaxPosition.y = atol(pcAttrValue);

	pcAttrValue	= pRootElement->Attribute("NormalPosition.left");
	if (NULL != pcAttrValue)		aWp[0].rcNormalPosition.left = atol(pcAttrValue);

	pcAttrValue	= pRootElement->Attribute("NormalPosition.right");
	if (NULL != pcAttrValue)		aWp[0].rcNormalPosition.right = atol(pcAttrValue);

	pcAttrValue	= pRootElement->Attribute("NormalPosition.top");
	if (NULL != pcAttrValue)		aWp[0].rcNormalPosition.top = atol(pcAttrValue);

	pcAttrValue	= pRootElement->Attribute("NormalPosition.bottom");
	if (NULL != pcAttrValue)		aWp[0].rcNormalPosition.bottom = atol(pcAttrValue);

	//
	//int32 iMaxFlag = aWp[0].showCmd;
	// XL0006 - 与以前的false标记兼容
	if ( 0 == aWp[0].showCmd )
	{
		aWp[0].showCmd = SW_SHOWNORMAL;
	}
	//aWp[0].showCmd = SW_SHOW;
	//
	//	int32 iMaxFlag = aWp[0].showCmd;

	//	

	// FaceSchema
	pcAttrValue = pRootElement->Attribute(KStrElementAttriFaceEffectRange);
	if (NULL != pcAttrValue)	   CFaceScheme::Instance()->m_eFaceEffectRange = (CFaceScheme::E_FaceEffectRange)atoi(pcAttrValue);

	//

	//////////////////////////////////////////////////////////////////////////
	// 设置最大化标志,根据XML 配置,适应不同分辨率屏幕的最大化设置

	CRect rectMax;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rectMax,0);	

	// 	if ( 1 == iMaxFlag)
	// 	{
	// 		// 如果是最大化,那么在所有的机器上都应该是最大化
	// 		CRect rectNow;		
	// 		GetClientRect(&rectNow);
	// 		MoveWindow(&rectMax);		
	// 		m_rectLast = rectNow;
	// 		m_bMaxForSysSettingChange = true;		
	// 	}
	// 	else
	// 	{
	// 		// 如果不是最大化,但是这个工作区的尺寸大于本机的屏幕尺寸,则设置为最大化
	// 		if ( (aWp[0].rcNormalPosition.right - aWp[0].rcNormalPosition.left) > rectMax.Width() || (aWp[0].rcNormalPosition.bottom - aWp[0].rcNormalPosition.top) > rectMax.Height())
	// 		{
	// 			MoveWindow(&rectMax);
	// 			CRect recttemp = rectMax;
	// 			recttemp.top	-= 100;
	// 			recttemp.bottom -= 100;
	// 			recttemp.left	-= 100;
	// 			recttemp.right	-= 100;
	// 			m_rectLast = recttemp;
	// 			m_bMaxForSysSettingChange = true;		
	// 		}					
	// 	}
	if ( SW_SHOWNORMAL == aWp[0].showCmd )
	{
		// 如果不是最大化,但是这个工作区的尺寸大于本机的屏幕尺寸,则设置为最大化
		if ( (aWp[0].rcNormalPosition.right - aWp[0].rcNormalPosition.left) > rectMax.Width() || (aWp[0].rcNormalPosition.bottom - aWp[0].rcNormalPosition.top) > rectMax.Height())
		{
			aWp[0].showCmd = SW_SHOWMAXIMIZED;
		}					
	}

	// 是否需要先隐藏，然后等待初始化完成后显示？

	// 只有系统第一次打开时才需要调用，后续不应当再次影响用户看到的程序大小
	if ( m_bFirstDisplayFromXml )
	{
		SetWindowPlacement(&aWp[0]);
		m_bFirstDisplayFromXml = false;
	}

	//////////////////////////////////////////////////////////////////////////
	// 遮羞布大小设置
	if ( NULL != m_pDlgWait)
	{
		CRect RectMain(0,0,0,0);
		::GetWindowRect(m_hWndMDIClient, RectMain);
	}

	//////////////////////////////////////////////////////////////////////////
	for(int32 iZindex = iWindowCounts-1; iZindex>=0 ; iZindex--)
	{
		TiXmlElement *pElementChildFrame = pRootElement->FirstChildElement();
		while (NULL != pElementChildFrame)
		{
			pcValue = pElementChildFrame->Value();

			if ( strcmp(CMPIChildFrame::GetXmlElementValue(), pcValue) != 0)
				continue;

			// 按照z 值从大到小的顺序,创建子窗口
			const char * StrChildZindex = pElementChildFrame->Attribute("zindex");
			int32     iChildZindex = atoi(StrChildZindex);

			if ( iChildZindex == iZindex )
			{
				CMPIChildFrame *pChildFrame = pApp->m_pDocTemplate->CreateMPIFrame(IDR_MAINFRAME);
				if (NULL == pChildFrame)
					return false;

				if (!pChildFrame->FromXml(pElementChildFrame))
					return false;

				// 设置默认的激活视图,如果有Tab 页,那么显示Tab 页的激活视图.否则,用默认的
				if (!pChildFrame->m_bHaveTab)
				{
					// pChildFrame->SetActiveGGTongView();
				}				   					   
			}
			pElementChildFrame = pElementChildFrame->NextSiblingElement();
		}
	}

	if ( NULL != m_pDlgWait )
	{
		//m_pDlgWait->ShowWindow(SW_HIDE); // 这个会导致MainFrame如果在showNormal,max之间切换的话，一起hide - -
		m_pDlgWait->MoveWindow(CRect(0,0,0,0));
		m_pDlgWait->PostMessage(WM_CLOSE,0,0);
		m_pDlgWait = NULL;
		ShowCursor(true);

		// 加载完后，消息栈中可能有很多消息，导致显示混乱，先让绘图画一下在说
		UpdateWindow();
	}

	SetHotkeyTarget(NULL);
	m_bFromXml = false;

	if ( NULL != m_pActiveGGTViewFromXml )
	{
		// 用完以后就置空.

		CMPIChildFrame* pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, m_pActiveGGTViewFromXml->GetParentFrame());

		pParentFrame->SetActiveView(NULL);
		pParentFrame->SetActiveView(m_pActiveGGTViewFromXml);

		SetActiveGGTongViewXml(NULL);		
	}

	if (m_pNewWndTB)
	{
		m_pNewWndTB->InitialCurrentWSP();
	}

	RedrawWindow(NULL, NULL, RDW_FRAME|RDW_NOERASE|RDW_INVALIDATE);
	return true;
}

void CMainFrame::PostUpdateWnd()
{
	CWnd *pWnd = CWnd ::FromHandle(m_hWndMDIClient)->GetWindow(GW_CHILD); 

	while ( NULL != pWnd )   
	{
		((CMPIChildFrame*)pWnd)->PostUpdateWnd();
		pWnd = pWnd->GetWindow(GW_HWNDNEXT);
	}
}

/*
Warning -- Passing struct 'CStringT' to ellipsis
*/
//lint -e{437}
CString CMainFrame::ToXml(const char * KpcFileName, CString StrFileTitle /*= L""*/, CString StrVersion /*= L""*/)
{
	// 保存文件
	CMPIChildFrame* pChild = (CMPIChildFrame*)GetActiveFrame();
	if ( (CNewMDIFrameWnd*)pChild != (CNewMDIFrameWnd*)this )
	{
		CGGTongView* pView = (CGGTongView*)pChild->GetActiveView();
		if ( NULL != pView )
		{
			// 设置最后一个激活的GGTongView 
			SetActiveGGTongViewXml(pView);
		}		
	}

	//
	CString StrThis;
	CString StrWindowCounts;

	CString StrChildFame  = "";
	int32   iWindowCounts = 0;
	int32   iZindex       = 0;

	CWnd *pWnd = CWnd ::FromHandle(m_hWndMDIClient)->GetWindow(GW_CHILD); 

	// xml化所有子窗口, 对于有保存的页面的怎么处理呢
	// 有id的不保存到工作区
	while ( NULL != pWnd )   
	{ 
		CMPIChildFrame *pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, pWnd);
		if ( NULL != pChildFrame && pChildFrame->GetIdString().IsEmpty() )
		{
			StrChildFame += pChildFrame->ToXml(iZindex,NULL);
			iWindowCounts++;
			iZindex++;
		}
		pWnd = pWnd->GetWindow(GW_HWNDNEXT);
	}

	// 
	CString StrFileShowName = AfxGetApp()->m_pszAppName;
	bool32	bReadOnly		= false;

	// xml 中的显示名称默认为文件名(用户另存为和出错的情况下,不至于太离谱)
	if ( StrFileTitle.GetLength() > 0 )
	{
		StrFileShowName = StrFileTitle;
	}

	const T_WspFileInfo* pWspFileInfo = NULL;

	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();

	if ( NULL != pApp )
	{
		pWspFileInfo = pApp->GetWspFileInfo(KpcFileName);	

		if ( NULL != pWspFileInfo )
		{
			StrVersion		= pWspFileInfo->m_StrFileVersion;
			StrFileShowName = pWspFileInfo->m_StrFileXmlName;
			bReadOnly		= pWspFileInfo->m_bReadOnly;
		}
	}

	CString StrHead = L"";

	// 这个是XML 的版本号.不要改
	StrHead.Format(L"<?xml version =\"%s\" encoding=\"utf-8\" ?> \n", L"1.0");
	StrThis += StrHead;

	StrHead.Format(L"<XMLDATA version=\"%s\" app = \"ggtong\" data = \"WorkSpace\" %s = \"%s\" %s = \"%s\" >\n",
		StrVersion, CString(KStrElementAttriFileShowName), StrFileShowName,
		CString(GetXmlRootElementAttrReadOnly()), bReadOnly ? _T("1") : _T("0")
		);
	StrThis += StrHead;

	//if ( StrFileShowName == L"CfjdSystem" )
	//{
	//	ASSERT(0);
	//}

	StrThis += "<";
	StrThis += KStrElementValue;

	StrWindowCounts.Format(L"%s=\"%d\"", CString(KStrWindowCounts), iWindowCounts); 

	StrThis += " ";
	StrThis += StrWindowCounts;

	// 添加自身属性

	CString StrMainFrameAttribute;	
	CString StrName, StrZIndex, StrFlag, StrShowCmd;
	CString StrPtMinPosX, StrPtMinPosY, StrPtMaxPosX, StrPtMaxPosY;
	CString StrNomPosleft, StrNomPosright, StrNomPostop, StrNomPosbottom;


	WINDOWPLACEMENT aWp[256];
	GetWindowPlacement(&aWp[0]);

	aWp[0].length = sizeof(WINDOWPLACEMENT);

	StrFlag.Format(L"%d", aWp[0].flags);
	StrShowCmd.Format(L"%d", aWp[0].showCmd);
	//  保存是否最大化这个标志	XL0006 保存真实的showCmd
	//	StrShowCmd.Format(L"%d", BeMaxSize());
	StrPtMinPosX.Format(L"%i", aWp[0].ptMinPosition.x);
	StrPtMinPosY.Format(L"%i", aWp[0].ptMinPosition.y);
	StrPtMaxPosX.Format(L"%i", aWp[0].ptMaxPosition.x);
	StrPtMaxPosY.Format(L"%i", aWp[0].ptMaxPosition.y);
	StrNomPosleft.Format(L"%i",aWp[0].rcNormalPosition.left);
	StrNomPosright.Format(L"%i",aWp[0].rcNormalPosition.right);
	StrNomPostop.Format(L"%i",aWp[0].rcNormalPosition.top);
	StrNomPosbottom.Format(L"%i",aWp[0].rcNormalPosition.bottom);
	/*
	Warning -- Passing struct 'CStringT' to ellipsis
	*/
	//lint -e{437}
	StrMainFrameAttribute.Format(L" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\"  %s=\"%s\" ",/*>\n*/
		CString(KStrElementAttriFlag),
		StrFlag,			
		CString(KStrElementAttriShowCmd),
		StrShowCmd,			  
		CString(KStrElementAttriPtMinX),
		StrPtMinPosX,	 
		CString(KStrElementAttriPtMinY),
		StrPtMinPosY,
		CString(KStrElementAttriPtMaxX),
		StrPtMaxPosX,	 
		CString(KStrElementAttriPtMaxY),
		StrPtMaxPosY,
		CString(KStrElementAttriPtNorLeft),
		StrNomPosleft,
		CString(KStrElementAttriPtNorRight),
		StrNomPosright,
		CString(KStrElementAttriPtNorTop),
		StrNomPostop,	 
		CString(KStrElementAttriPtNorBottom),
		StrNomPosbottom);

	StrThis += StrMainFrameAttribute;
	//
	CString StrFace;
	StrFace.Format(L"%s = \"%d\" ",CString(KStrElementAttriFaceEffectRange),CFaceScheme::Instance()->m_eFaceEffectRange);
	StrThis += StrFace;
	//
	StrThis += L">\n";
	//
	StrThis += StrChildFame;
	StrThis += "</";
	StrThis += KStrElementValue;
	StrThis += L"> \n";					
	StrThis += L"</XMLDATA>";

	SaveXmlFile(KpcFileName,StrThis);

	// 用完置空
	SetActiveGGTongViewXml(NULL);

	return StrThis;
}

CString CMainFrame::GetWspFileShowName(const char* KpcFilePath)
{	
	// 得到工作区文件中描述的用于显示的文件名		
	if ( NULL == KpcFilePath )
	{
		return L"";
	}

	TiXmlDocument MyDocument = TiXmlDocument(KpcFilePath);
	if ( !MyDocument.LoadFile(KpcFilePath) )
	{
		return L"";
	}

	// <XMLDATA>
	TiXmlElement* pRootElement = MyDocument.FirstChildElement();

	if ( NULL == pRootElement)
	{
		return L"";
	}

	const char* StrFileShowName = pRootElement->Attribute(KStrElementAttriFileShowName);
	if ( NULL == StrFileShowName )
	{
		return L"";
	}

	// 多字节转化为宽字节
	wchar_t awcFileName[1024];
	memset(awcFileName, 0, sizeof(awcFileName));

	MultiCharCoding2Unicode(EMCCUtf8, StrFileShowName, strlen(StrFileShowName), awcFileName, sizeof(awcFileName) / sizeof(wchar_t));
	CString StrReturn = awcFileName;

	return StrReturn;
}

CString CMainFrame::GetWspFileVersion(const char* KpcFilePath)
{
	// 得到工作区文件中描述的版本号
	// <?xml version ="1.0" encoding="utf-8" ?> 

	if ( NULL == KpcFilePath )
	{
		return L"";
	}

	TiXmlDocument MyDocument = TiXmlDocument(KpcFilePath);
	if ( !MyDocument.LoadFile(KpcFilePath) )
	{
		return L"";
	}

	// <XMLDATA>
	TiXmlElement* pRootElement = MyDocument.RootElement();

	if ( NULL == pRootElement)
	{
		return L"";
	}

	const char* StrFileVersion = pRootElement->Attribute(GetXmlRootElementAttrVersion());
	if ( NULL == StrFileVersion )
	{
		return L"";
	}

	// 多字节转化为宽字节
	wchar_t awcFileVersion[1024];
	memset(awcFileVersion, 0, sizeof(awcFileVersion));

	MultiCharCoding2Unicode(EMCCUtf8, StrFileVersion, strlen(StrFileVersion), awcFileVersion, sizeof(awcFileVersion) / sizeof(wchar_t));
	CString StrReturn = awcFileVersion;

	return StrReturn;
}

bool32 CMainFrame::IsWspReadOnly( const char *KpcFilePath )
{
	// 得到工作区文件中描述的版本号
	// <?xml version ="1.0" encoding="utf-8" ?> 

	if ( NULL == KpcFilePath )
	{
		return false;
	}

	TiXmlDocument MyDocument = TiXmlDocument(KpcFilePath);
	if ( !MyDocument.LoadFile(KpcFilePath) )
	{
		return false;
	}

	// <XMLDATA>
	TiXmlElement* pRootElement = MyDocument.RootElement();

	if ( NULL == pRootElement)
	{
		return false;
	}

	const char* StrReadOnly = pRootElement->Attribute(GetXmlRootElementAttrReadOnly());
	if ( NULL == StrReadOnly )
	{
		return false;
	}

	return atoi(StrReadOnly) > 0;
}

bool32 CMainFrame::SetWspFileReadOnly( const char *KpcFilePath, bool32 bReadOnly )
{
	// 得到工作区文件中描述的版本号
	// <?xml version ="1.0" encoding="utf-8" ?> 

	if ( NULL == KpcFilePath )
	{
		return false;
	}

	TiXmlDocument MyDocument = TiXmlDocument(KpcFilePath);
	if ( !MyDocument.LoadFile(KpcFilePath) )
	{
		return false;
	}

	// <XMLDATA>
	TiXmlElement* pRootElement = MyDocument.RootElement();

	// 不创建不存在的东西
	if ( NULL == pRootElement)
	{
		return false;
	}

	pRootElement->SetAttribute(GetXmlRootElementAttrReadOnly(), bReadOnly ? "1" : "0");
	MyDocument.SaveFile(KpcFilePath);

	return true;
}


void CMainFrame::SaveXmlFile(const char * KpcFileName, const CString &StrContent)
{	
	if ( NULL == KpcFileName || 0 == StrContent.GetLength())
	{
		return;
	}
	// 申请空间, 该空间大小至多是原长度的2倍, 因为Unicode中中文占2字节, Utf8中中文占3-7字节, 平均下来, 一般绝不会超过2倍
	int32 iOutBufferSize = StrContent.GetLength();
	iOutBufferSize *= 2;
	iOutBufferSize += 100;		// 防止StrContent长度为0
	char *pcOutBuffer = new char[iOutBufferSize];
	if (NULL != pcOutBuffer)
	{
		// 先转换写入文件的内容
		memset(pcOutBuffer, 0, iOutBufferSize);
		Unicode2MultiCharCoding(EMCCUtf8, StrContent, StrContent.GetLength(), pcOutBuffer, iOutBufferSize);

		// 转换文件名
		wchar_t awcFileName[MAX_PATH];
		memset(awcFileName, 0, sizeof(awcFileName));
		MultiCharCoding2Unicode(EMCCSystem, KpcFileName, strlen(KpcFileName), awcFileName, sizeof(awcFileName) / sizeof(wchar_t));

		CFile File;
		if (File.Open(awcFileName, CFile::modeCreate | CFile::modeWrite))
		{
			File.Write(pcOutBuffer, strlen(pcOutBuffer));
			File.Close();
		}

		// 不要忘记释放
		DEL_ARRAY(pcOutBuffer);
	}
}

void CMainFrame::SetHotkeyTarget(CWnd* pParent, E_HotKeyType eHKT)
{
	if ( m_bForceFixKeyBoard )
	{
		return;
	}

	if ( NULL == pParent )
	{
		m_pKBParent = this;
		m_dwHotKeyTypeFlag = EHKTCount;		// 默认为所有
	}
	else
	{
		m_pKBParent = pParent;
		m_dwHotKeyTypeFlag = eHKT;
	}
}

static bool32 IsParent(HWND hParent,HWND hWnd)
{
	while ( hWnd )
	{
		if ( hWnd == hParent )
		{
			return true;
		}
		hWnd = ::GetParent(hWnd);
	}

	return false;
}

void CMainFrame::ProcessHotkey(MSG* pMsg)
{
	// MainFrame 中发生键盘按下事件 
	if ( WM_KEYDOWN == pMsg->message && !IsCtrlPressed() && IsParent(GetSafeHwnd(), pMsg->hwnd))
	{
		if ( NULL == m_pDlgKeyBoard )
		{
			int32 nVirtKey = (int32) pMsg->wParam;
			int32 i = 0;
			bool32 bFound = false;
			while ( true )
			{
				if ( s_nKeySpiriteWinAgainst[i] == VK_KEYCOUNT ) break;
				if ( s_nKeySpiriteWinAgainst[i] == nVirtKey )
				{
					bFound = true;
					break;
				}
				i ++;
			}

			if ( !bFound )
			{
				// xl 1102 使用showdlg替换
				ShowHotkeyDlg(pMsg, m_pKBParent, (E_HotKeyType)m_dwHotKeyTypeFlag);
				// 				CGGTongApp * pApp = (CGGTongApp *)AfxGetApp();
				// 				CGGTongDoc *pDoc = pApp->m_pDocument;
				// 				ASSERT(NULL != pDoc);
				// 
				// 				// 创建小键盘输入对话框
				// 				if ( NULL == m_pKBParent || !IsWindow(m_pKBParent->GetSafeHwnd()))	// 非正常窗口!
				// 				{
				// 					m_pKBParent = this;
				// 					m_dwHotKeyTypeFlag = EHKTCount;
				// 					SetForceFixHotkeyTarget(false);
				// 				}
				// 
				// 				if ( m_dwHotKeyTypeFlag != EHKTCount )
				// 				{
				// 					// 仅支持特定一个类别，目前并没有支持多个类别
				// 					m_pDlgKeyBoard = new CKeyBoardDlg((E_HotKeyType)m_dwHotKeyTypeFlag, m_pKBParent);
				// 				}
				// 				else
				// 				{
				// 					m_pDlgKeyBoard = new CKeyBoardDlg(&pDoc->m_pAbsCenterManager->m_HotKeyList, m_pKBParent);
				// 				}
				// 				ASSERT(NULL != m_pDlgKeyBoard);
				// 				
				// 				// 保存按键消息,传至“按键精灵”对话框中
				// 				CopyMemory(&m_pDlgKeyBoard->m_Msg, pMsg, sizeof(MSG)); 
				// 
				// 				m_pDlgKeyBoard->Create(IDD_KEYBOARD, m_pKBParent);
				// 				m_pDlgKeyBoard->ShowWindow(SW_SHOW);
			}
		}
	}
}

bool32 CMainFrame::OnProcessF7(CGGTongView* pGGTonView /*= NULL*/)
{	
	CGGTongView* pActiveGGTongView  = NULL; 

	if ( NULL == pGGTonView )
	{
		pActiveGGTongView = (CGGTongView*)FindGGTongView();
	}
	else
	{
		pActiveGGTongView = pGGTonView;
	}

	if ( NULL == pActiveGGTongView )
	{
		return false;
	}

	if(!pActiveGGTongView->m_bCanF7)
	{
		return false;
	}
	// 如果是还原，则肯定是F7的GGTongView
	bool32 bF7 = true;
	CMPIChildFrame *pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, pActiveGGTongView->GetParentFrame());
	if ( NULL != pParentFrame && pParentFrame->IsF7AutoLock() )
	{
		CGGTongView *pF7View = pParentFrame->GetF7GGTongView();
		ASSERT( NULL != pF7View );
		if ( NULL != pF7View )
		{
			pActiveGGTongView = pF7View;	// 显然要还原的是最终的f7窗口
		}
		bF7 = false;	// 还原操作
	}

	// 如果是F7操作，由于激活窗口被隐藏了，所以需要重新设定激活窗口
	// 这里交给外部来进行设置

	CGGTongView* pActiveGGTongViewBK= pActiveGGTongView;

	bool32 bValid = false;

	if ( NULL != pActiveGGTongView )
	{
		CWnd * pParent = pActiveGGTongView->GetParent();

		while(pParent)
		{
			if ( pParent->IsKindOf(RUNTIME_CLASS(CTabSplitWnd)) || pParent->IsKindOf(RUNTIME_CLASS(CMPIChildFrame)) )
			{				
				break;
			}

			if ( pParent->IsKindOf(RUNTIME_CLASS(CBiSplitterWnd)) )
			{
				bValid = true;

				CBiSplitterWnd * pBiSplit = (CBiSplitterWnd*)pParent;
				double dRadio = pBiSplit->GetF9Ratio(pActiveGGTongView);
				pBiSplit->SetRadio(dRadio, true);

				pActiveGGTongView = (CGGTongView*)pParent;	// 注意这里并不是ggtong view
			}

			pParent = pParent->GetParent();
		}
	}

	// 还原的时候, 特殊视图的处理
	if ( !bF7 && pActiveGGTongViewBK->m_IoViewManager.GetBeyondShowFlag() )
	{
		// 要再次撑大显示
		pActiveGGTongViewBK->m_IoViewManager.ForceBeyondShowIoView();
	}

	if (!bF7)
	{
		CMPIChildFrame* pFrame = (CMPIChildFrame*)pActiveGGTongView->GetParentFrame();

		for ( int32 i = 0; i < pFrame->m_aTest.GetSize(); i++ )
		{
			if (NULL != pFrame->m_aTest[i] )
			{
				((CGGTongView*)(pFrame->m_aTest[i]))->RedrawWindow();
			}
		}

		pFrame->RedrawWindow();	
	}

	// 	if (m_pNewWndTB)
	// 	{
	// 		m_pNewWndTB->SetF7(bF7);
	// 	}

	return bValid;
}

bool32 CMainFrame::OnProcessCFF7( CMPIChildFrame *pChildFrame /*= NULL*/ )
{
	if ( NULL == pChildFrame )
	{
		pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, MDIGetActive());
	}

	if ( NULL != pChildFrame )
	{
		CGGTongView *pView = DYNAMIC_DOWNCAST(CGGTongView, pChildFrame->GetActiveView());
		if ( NULL != pView )
		{
			return OnProcessF7(pView);
		}
	}

	return false;
}

bool32	CMainFrame::IsFullScreen(CGGTongView* pGGTonView /*= NULL*/)
{
	CGGTongView* pActiveGGTongView  = NULL; 

	if ( NULL == pGGTonView )
	{
		pActiveGGTongView = (CGGTongView*)FindGGTongView();
	}
	else
	{
		pActiveGGTongView = pGGTonView;
	}

	if ( NULL == pActiveGGTongView )
	{
		return false;
	}

	CMPIChildFrame *pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, pActiveGGTongView->GetParentFrame());
	if ( NULL != pParentFrame && pParentFrame->IsF7AutoLock() )
	{
		return true;
	}

	return false;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	FilterToolTipMessage(pMsg);

	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();

	if ( WM_SIZE == pMsg->message && m_pNewWndTB)
	{
		m_pNewWndTB->OnHideIM();
	}

	if ( pApp->m_bBindHotkey && pApp->m_iBindHotkey < 1 )
	{
		// 快捷键处理
		if (WM_SYSKEYDOWN == pMsg->message || WM_KEYDOWN == pMsg->message)
		{
			// 关闭程序
			if ( VK_F4 == pMsg->wParam && IsAltPressed() )
			{
				CloseWorkSpace();
				return TRUE;					 
			}

			// 关闭子窗口			
			CMPIChildFrame * pChild = (CMPIChildFrame*)MDIGetActive();
			if ( (NULL != pChild) && (*pChild != *this) && pChild != CCfmManager::Instance().GetUserDefaultCfmFrame() )
			{			
				if ( VK_F4 == pMsg->wParam && IsCtrlPressed() )
				{
					OnCloseChildFrame();	// 关闭当前窗口
					return TRUE;
				}			
			}

			if ( VK_F10 == pMsg->wParam )
			{
				// 处理 F10				
				if ( DoF10() )
				{
					return TRUE;
				}
			}

			if ( VK_F11 == pMsg->wParam )
			{
				// 处理 F10				
				if ( DoF11() )
				{
					return TRUE;
				}
			}

			// debug下按F12会产生DbgBreakIn
			if ( VK_F12 == pMsg->wParam )
			{
				ShowRealTradeDlg();
				return TRUE;
			}

			//
			UINT nChar = (UINT)pMsg->wParam;

			if ( IsCtrlPressed() && 'F' == nChar )
			{
				CDlgFormularManager Dlg;
				Dlg.DoModal();

				return TRUE; 
			}

			if ( VK_ESCAPE == nChar )
			{
				// 某种情况下ESC流转到这里来了
				OnEscBackFrame();
				return TRUE;	// 吃了这个消息
			}

#ifdef TRACE_DLG
			if (VK_F11 == nChar)
			{
				CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();

				if (NULL != pDoc->m_pDlgbuding->m_pDlgTrace)
				{
					if (NULL != pDoc->m_pDlgbuding->m_pDlgTrace->GetSafeHwnd())
					{
						pDoc->m_pDlgbuding->m_pDlgTrace->ShowWindow(SW_SHOW);
						pDoc->m_pDlgbuding->m_pDlgTrace->BringWindowToTop();

						/*
						if (pDoc->m_pDlgbuding->m_pDlgTrace->IsWindowVisible())
						{							
						pDoc->m_pDlgbuding->m_pDlgTrace->ShowWindow(SW_HIDE);							
						}
						else
						{
						pDoc->m_pDlgbuding->m_pDlgTrace->ShowWindow(SW_SHOW);
						}
						*/
						return TRUE;
					}
				}
			}
#endif		
			if (PreTransGlobalHotKey(nChar))
				return TRUE;
		}

		//////////////////////////////////////////////////////////////////////////
		ProcessHotkey(pMsg);
	}
	return CNewMDIFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::OnSetting() 
{	
	CWnd *pWnd = CWnd ::FromHandle(m_hWndMDIClient)->GetWindow(GW_CHILD); 
	// ...fangz0411 必须有活动视图
	CIoViewBase * pIoView = FindActiveIoView();

	if (pWnd && pIoView)
	{
		CDlgSystemSetting  DlgSystemSetting;
		DlgSystemSetting.m_DlgSystemFace.SetActiveIoView(pIoView);
		DlgSystemSetting.DoModal();	
	}
	else
	{
		MessageBox(L"当前没有可设置的视图或窗口!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return;
	}	
}

void CMainFrame::OnGridheadSet() 
{
	CDlgSetGridHead DlgSetGridHead;
	DlgSetGridHead.DoModal();
}


void CMainFrame::OnBlockSet() 
{
	// 	CDlgUserBlockManager* pDlgBlockManager;
	// 	pDlgBlockManager = new CDlgUserBlockManager();
	// 	保存按键消息,传至“按键精灵”对话框中		
	// 	pDlgBlockManager->Create(IDD_DIALOG_BLOCK_MAG,this);
	// 	pDlgBlockManager->CenterWindow();
	// 	pDlgBlockManager->ShowWindow(SW_SHOW);
	CDlgUserBlockManager dlg;
	dlg.DoModal();
}

void CMainFrame::OnFormulaMag() 
{
	CDlgFormularManager dlg;
	dlg.DoModal();
}

////////////////////////////////////////////////////////
CGGTongView* CMainFrame::FindGGTongView(CWnd *pWnd, CGGTongView *pGGTongViewDst)
{
	if (NULL == pWnd)
		return NULL;

	// 
	if (pWnd->IsKindOf(RUNTIME_CLASS(CGGTongView)))
	{
		if (NULL == pGGTongViewDst)
		{
			return (CGGTongView *)pWnd;
		}
		else
		{
			if (pGGTongViewDst == pWnd)
			{
				return (CGGTongView *)pWnd;
			}
			else
			{
				return NULL;
			}
		}
	}

	// 
	CWnd *pWndChild = pWnd->GetNextWindow(GW_CHILD);
	while (NULL != pWndChild)
	{
		//
		CGGTongView *pGGTongViewFound = FindGGTongView(pWndChild, pGGTongViewDst);
		if (NULL != pGGTongViewFound)
			return pGGTongViewFound;

		//
		pWndChild = pWndChild->GetNextWindow(GW_HWNDNEXT);
	}

	return NULL;
}

CView* CMainFrame::FindGGTongView ()
{
	// 找当前激活的GGTongView

	CMPIChildFrame * pActiveChildFrame = ( CMPIChildFrame * )GetActiveFrame();
	if (NULL == pActiveChildFrame)
		return NULL;

	CView *pViewActive = pActiveChildFrame->GetActiveView();
	if (NULL == pViewActive)
		return NULL;

	ASSERT(pViewActive->IsKindOf(RUNTIME_CLASS(CGGTongView)));

	CGGTongView *pGGTongView = FindGGTongView(this, (CGGTongView *)pViewActive);

	return pGGTongView;
}

CIoViewBase* CMainFrame::FindActiveIoView()
{
	CMPIChildFrame * pActiveChildFrame = (CMPIChildFrame *)GetActiveFrame();
	if (NULL == pActiveChildFrame)
		return NULL;

	CView *pViewActive = pActiveChildFrame->GetActiveView();
	if (NULL == pViewActive)
		return NULL;

	ASSERT(pViewActive->IsKindOf(RUNTIME_CLASS(CGGTongView)));

	CGGTongView *pGGTongView = FindGGTongView(this, (CGGTongView *)pViewActive);
	if (NULL != pGGTongView)
	{
		return pGGTongView->m_IoViewManager.GetActiveIoView();
	}

	return NULL;
}

bool32 CMainFrame::GetMerchInSpecifyIoViewGroup(int32 iIoViewGroupId, IN CMerch *&pMerch)
{	
	pMerch = NULL;

	// 
	if (iIoViewGroupId <= 0) 
		return false;

	// 枚举所有的业务视图, 找到相同分组的， 取其有效值
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDoc = pApp->m_pDocument;
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return false;
	}

	for (int32 i = 0; i < m_IoViewsPtr.GetSize(); i++)
	{
		CIoViewBase *pIoView = m_IoViewsPtr[i];
		int32 iViewGroupID = pIoView->GetIoViewGroupId();
		if (iViewGroupID & iIoViewGroupId/*pIoView->GetIoViewGroupId() == iIoViewGroupId*/)
		{
			if (NULL != pIoView->m_pMerchXml)
			{
				pMerch = pIoView->m_pMerchXml;
				return true;
			}
		}
	}

	// 没有相同分组的，就赋予上证指数(北京项目特例)
	pAbsCenterManager->GetMerchManager().FindMerch(_T("000001"), 0, pMerch);

	return NULL!=pMerch;
}

LRESULT CMainFrame::OnMsgHotKey(WPARAM wParam, LPARAM lParam)
{
	CHotKey *pHotKey = (CHotKey *)wParam;
	ASSERT(pHotKey);

	CHotKey HotKey = *pHotKey;
	DEL(pHotKey);

	// 
	if (EHKTMerch == HotKey.m_eHotKeyType)
	{
		OnHotKeyMerch(HotKey.m_iParam1, HotKey.m_StrParam1);
	}
	else if (EHKTShortCut == HotKey.m_eHotKeyType)
	{
		OnHotKeyShortCut(HotKey.m_StrKey);
	}
	else if (EHKTBlock == HotKey.m_eHotKeyType)
	{
		OnHotKeyBlock(HotKey.m_StrParam1, HotKey.m_iParam1);
	}
	else if (EHKTIndex == HotKey.m_eHotKeyType)
	{
		// zhangbo 0515 #待补充
		//...
		OnHotKeyIndex(HotKey);	
	}
	else if ( EHKTLogicBlock == HotKey.m_eHotKeyType )
	{   
		OnHotKeyLogicBlock(HotKey);
	}

	return 0;
}

void CMainFrame::OnHotKeyMerch(int32 iMarketId, const CString &StrMerchCode)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	ASSERT(NULL != pApp);

	CGGTongDoc *pDoc = pApp->m_pDocument;
	ASSERT(NULL != pDoc);

	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	ASSERT(NULL != pAbsCenterManager);
	if (!pAbsCenterManager)
	{
		return;
	}

	// 尝试找到当前业务视图
	//	CIoViewBase *pIoView = NULL;

	CMerch *pMerch = NULL;
	if (!pAbsCenterManager->GetMerchManager().FindMerch(StrMerchCode, iMarketId, pMerch))
	{
		return;
	}

	// 大势研判 只能在那几个商品之间切换，其它商品跳转到分时走势
	CIoViewBase *pIoViewSrc = FindActiveIoView();
	CIoViewBase *pChart = NULL;
	pChart = FindChartIoViewInSameFrame(pIoViewSrc);
	if ( NULL != pChart )
	{
		if (pChart->IsKindOf(RUNTIME_CLASS(CIoViewKLine)))
		{
			CIoViewKLine *pKLine = (CIoViewKLine *)pChart;
			if (!pKLine->BeInSpecialMerchArray(pMerch) && pKLine->GetShowExpBtn())
			{
				LoadSystemDefaultCfm(ID_PIC_TREND, pMerch);
				return;
			}
		}
	}

	OnShowMerchInChart(pMerch, NULL);
}

void CMainFrame::OnHotKeyMerch( const CString &StrMerchCode )
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	ASSERT(NULL != pApp);

	CGGTongDoc *pDoc = pApp->m_pDocument;
	ASSERT(NULL != pDoc);

	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	ASSERT(NULL != pAbsCenterManager);
	if (!pAbsCenterManager)
	{
		return;
	}

	// 尝试找到当前业务视图
	//	CIoViewBase *pIoView = NULL;

	CMerch *pMerch = NULL;
	if (!pAbsCenterManager->GetMerchManager().FindMerch(StrMerchCode, pMerch))
	{
		return;
	}

	OnShowMerchInChart(pMerch, NULL);
}

void CMainFrame::OnHotKeyBlock(const CString &StrBlockName, int32 iBlockType)
{
	T_BlockDesc BlockDesc;
	BlockDesc.m_eType		 = (T_BlockDesc::E_BlockType)iBlockType;
	BlockDesc.m_StrBlockName = StrBlockName;
	BlockDesc.m_iMarketId	 = -1;
	//linhc 20100917添加默认焦点第一行商品
	CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(BlockDesc.m_StrBlockName);
	if ( NULL != pBlock && pBlock->m_blockInfo.m_aSubMerchs.GetSize() > 0 )
	{   
		BlockDesc.m_aMerchs.Copy(pBlock->m_blockInfo.m_aSubMerchs);
	}

	OnSpecifyBlock(BlockDesc);
}

void CMainFrame::OnHotKeyShortCut(const CString &StrKey)
{
	const T_ShortCutObject *pstShortCutObject = FindShortCutObjectByKey(StrKey);
	if (NULL == pstShortCutObject)
		return;

	pstShortCutObject->m_pfnShortCutCallBack();
}

void CMainFrame::OnHotKeyLogicBlock( const CHotKey &hotKey )
{
	T_BlockDesc BlockDesc;
	BlockDesc.m_eType		 = (T_BlockDesc::E_BlockType)hotKey.m_iParam1;
	ASSERT( BlockDesc.m_eType == T_BlockDesc::EBTBlockLogical );
	BlockDesc.m_StrBlockName = hotKey.m_StrParam1;
	BlockDesc.m_iMarketId	 = hotKey.m_iParam2;
	CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(BlockDesc.m_iMarketId);
	if ( NULL != pBlock )
	{
		BlockDesc.m_aMerchs.Copy( pBlock->m_blockInfo.m_aSubMerchs );
		OnSpecifyBlock(BlockDesc);
	}
}

void CMainFrame::OnHotKeyIndex( const CHotKey &hotKey )
{
	// 此处应当有权限控制
	if ( !CPluginFuncRight::Instance().IsUserHasIndexRight(hotKey.m_StrParam1, true) )
	{
		return;
	}
	CIoViewKLine *pIoViewKLine = (CIoViewKLine *)GetIoViewByPicMenuIdAndActiveIoView(ID_PIC_KLINE, false, true, false, true, false, true);
	if ( NULL == pIoViewKLine )
	{
		LoadSystemDefaultCfm(ID_PIC_KLINE, NULL, true);
		pIoViewKLine = (CIoViewKLine *)FindIoViewInFrame(ID_PIC_KLINE, NULL, false, false);
	}
	if ( NULL != pIoViewKLine )
	{
		if ( NULL == pIoViewKLine->GetMerchXml() )	// 填充一个默认商品给他
		{
			CMerch *pMerch = NULL;
			CAbsCenterManager *pAbsCenterManager = pIoViewKLine->GetCenterManager();
			if ( NULL != pAbsCenterManager
				&& pAbsCenterManager->GetMerchManager().m_BreedListPtr.GetSize() > 0
				&& pAbsCenterManager->GetMerchManager().m_BreedListPtr[0]->m_MarketListPtr.GetSize() > 0
				&& pAbsCenterManager->GetMerchManager().m_BreedListPtr[0]->m_MarketListPtr[0]->m_MerchsPtr.GetSize() > 0)
			{
				pMerch = pAbsCenterManager->GetMerchManager().m_BreedListPtr[0]->m_MarketListPtr[0]->m_MerchsPtr[0];
				OnViewMerchChanged(pIoViewKLine, pMerch);
			}

			// 没有商品也不能赋予指标- -
		}
		if(0 >= pIoViewKLine->m_pRegionMain->m_RectView.Height())
		{
			m_hotKey = hotKey;
			SetTimer(KTimerIdIndexShow, KTimerPeriodIndexShow, NULL);
			return;
		}
		if ( NULL != pIoViewKLine->GetMerchXml() )
		{
			pIoViewKLine->AddShowIndex(hotKey.m_StrParam1, false, true);	// 变更适当的商品
		}
		pIoViewKLine->BringToTop();
	}
}

bool32 CMainFrame::PreTransGlobalHotKey(int32 iVirtualKeyCode)
{
	const T_ShortCutObject *pstShortCutObject = FindShortCutObjectByVirtualKeyCode(iVirtualKeyCode);
	if (NULL == pstShortCutObject)
		return false;

	pstShortCutObject->m_pfnShortCutCallBack();
	return true;
}

void CMainFrame::OnSpecifyMarketAndFieldReport(int32 iMarketId, int32 iMerchReportField, bool32 bDescSort)
{
	T_BlockDesc BlockDesc;

	BlockDesc.m_iMarketId = iMarketId;
	BlockDesc.m_eType	  = (T_BlockDesc::E_BlockType)0;

	////////////////////////////////////////////////////
	CIoViewReport * pIoViewReport = FindIoViewReport(true);	
	CMPIChildFrame *pActiveFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, MDIGetActive());
	if ( NULL != pActiveFrame && (NULL == pIoViewReport && pActiveFrame->IsLockedSplit()) )
	{
		// 当前页面无报价表，且锁定，则调出默认报价表操作
		bool32 bLoad = LoadSystemDefaultCfm(ID_PIC_REPORT, NULL);
		ASSERT( bLoad );
		if ( bLoad )
		{
			pIoViewReport = FindIoViewReport(true);
			ASSERT( NULL != pIoViewReport );
			if ( NULL != pIoViewReport )
			{
				pIoViewReport->OpenBlock(BlockDesc, (E_MerchReportField)iMerchReportField, bDescSort);
				pIoViewReport->BringToTop();
				return;	// 如果不能运行到此，则允许以前的操作
			}
		}
	}

	if (NULL == pIoViewReport)
	{
		pIoViewReport = FindIoViewReport(false);
	}

	if (NULL == pIoViewReport)
	{	
		// zhangbo 0702 #bug, 需要创建空的报价表， 然后再添加一个报价信息
		NewEmptyReportChildFrame();
		CIoViewBase *pActiveIoView = FindActiveIoView();
		ASSERT(pActiveIoView->IsKindOf(RUNTIME_CLASS(CIoViewReport)));

		pIoViewReport = (CIoViewReport *)pActiveIoView;
		pIoViewReport->OpenBlock(BlockDesc, (E_MerchReportField)iMerchReportField, bDescSort);
		return;
	}

	// 找到这个IoViewReport 以后,设置为最前可见
	CMPIChildFrame * pChildFrameToShow = DYNAMIC_DOWNCAST(CMPIChildFrame, pIoViewReport->GetParentFrame());
	pChildFrameToShow->BringWindowToTop();

	CIoViewManager * pIoViewManger =(CIoViewManager *)pIoViewReport->GetParent();
	for (int32 i = 0 ;i< pIoViewManger->m_IoViewsPtr.GetSize(); i++)
	{
		CIoViewBase * pIoView = pIoViewManger->m_IoViewsPtr[i];
		if (pIoView->IsKindOf(RUNTIME_CLASS(CIoViewReport)))
		{
			pIoViewManger->m_GuiTabWnd.SetCurtab(i);
			break;
		}
	}

	// 显示板块
	pIoViewReport->OpenBlock(BlockDesc, (E_MerchReportField)iMerchReportField, bDescSort);
}


void CMainFrame::OnSpecifyBlockIdAndFieldReport( int32 iBlockId, int32 iMerchReportField, bool32 bDescSort )
{
	T_BlockDesc BlockDesc;

	BlockDesc.m_iMarketId = iBlockId;
	BlockDesc.m_eType	  = (T_BlockDesc::E_BlockType)T_BlockDesc::EBTBlockLogical;

	CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(iBlockId);
	if ( NULL != pBlock )
	{
		BlockDesc.m_aMerchs.Copy(pBlock->m_blockInfo.m_aSubMerchs);
		BlockDesc.m_StrBlockName = pBlock->m_blockInfo.m_StrBlockName;
	}
	else
	{
		return;
	}

	////////////////////////////////////////////////////
	CIoViewReport * pIoViewReport = FindIoViewReport(true);	
	CMPIChildFrame *pActiveFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, MDIGetActive());
	if ( NULL != pActiveFrame && (NULL == pIoViewReport && pActiveFrame->IsLockedSplit()) )
	{
		// 当前页面无报价表，且锁定，则调出默认报价表操作
		bool32 bLoad = LoadSystemDefaultCfm(ID_PIC_REPORT, NULL);
		ASSERT( bLoad );
		if ( bLoad )
		{
			pIoViewReport = FindIoViewReport(true);
			ASSERT( NULL != pIoViewReport );
			if ( NULL != pIoViewReport )
			{
				pIoViewReport->OpenBlock(BlockDesc, (E_MerchReportField)iMerchReportField, bDescSort);
				pIoViewReport->BringToTop();
				return;	// 如果不能运行到此，则允许以前的操作
			}
		}
	}

	if (NULL == pIoViewReport)
	{
		pIoViewReport = FindIoViewReport(false);
	}

	if (NULL == pIoViewReport)
	{	
		// zhangbo 0702 #bug, 需要创建空的报价表， 然后再添加一个报价信息
		NewEmptyReportChildFrame();
		CIoViewBase *pActiveIoView = FindActiveIoView();
		ASSERT(pActiveIoView->IsKindOf(RUNTIME_CLASS(CIoViewReport)));

		pIoViewReport = (CIoViewReport *)pActiveIoView;
		pIoViewReport->OpenBlock(BlockDesc, (E_MerchReportField)iMerchReportField, bDescSort);
		return;
	}

	// 找到这个IoViewReport 以后,设置为最前可见
	CMPIChildFrame * pChildFrameToShow = DYNAMIC_DOWNCAST(CMPIChildFrame, pIoViewReport->GetParentFrame());
	pChildFrameToShow->BringWindowToTop();

	CIoViewManager * pIoViewManger =(CIoViewManager *)pIoViewReport->GetParent();
	for (int32 i = 0 ;i< pIoViewManger->m_IoViewsPtr.GetSize(); i++)
	{
		CIoViewBase * pIoView = pIoViewManger->m_IoViewsPtr[i];
		if (pIoView->IsKindOf(RUNTIME_CLASS(CIoViewReport)))
		{
			pIoViewManger->m_GuiTabWnd.SetCurtab(i);
			break;
		}
	}

	// 显示板块
	pIoViewReport->OpenBlock(BlockDesc, (E_MerchReportField)iMerchReportField, bDescSort);
}

LRESULT CMainFrame::OnMsgGridHeaderChange(WPARAM wParam, LPARAM lParam)
{
	E_ReportType eReportType = (E_ReportType)wParam;


	OnViewGridHeaderChanged(eReportType);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnIoViewFaceChange(bool32 bFont,CIoViewBase * pIoViewSrc)
{
	if ( NULL == pIoViewSrc)
	{
		return;
	}

	CGGTongApp *pApp = (CGGTongApp*)AfxGetApp();
	CGGTongDoc *pDoc = pApp->m_pDocument;
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	if (NULL == pAbsCenterManager)
	{
		return;
	}

	switch (CFaceScheme::Instance()->m_eFaceEffectRange)
	{
	case CFaceScheme::EFERSingleIoView:
		{
			if(bFont)
			{
				pIoViewSrc->OnIoViewFontChanged();
			}
			else
			{
				pIoViewSrc->OnIoViewColorChanged();
			}
			return;
		}
		break;
	case CFaceScheme::EFERSameTypeIoView:
		{
			// 同类型视图

			// 设置每种类型视图对应的风格数据
			T_IoViewTypeFaceObject IoViewTypeFaceObject;

			IoViewTypeFaceObject.m_pIoViewClass = pIoViewSrc->GetRuntimeClass();

			if ( bFont)
			{
				CFaceScheme::Instance()->GetIoViewTypeFaceObject(pIoViewSrc,IoViewTypeFaceObject);
				// 只改变字体,颜色不变
				CFaceScheme::Instance()->GetNowUseFonts(IoViewTypeFaceObject.m_aFonts);
			}
			else
			{
				CFaceScheme::Instance()->GetIoViewTypeFaceObject(pIoViewSrc,IoViewTypeFaceObject);			
				// 只改变颜色,字体不变
				CFaceScheme::Instance()->GetNowUseColors(IoViewTypeFaceObject.m_aColors);
			}


			CFaceScheme::Instance()->UpdateIoViewTypeFaceObjectList(IoViewTypeFaceObject);

			// 找相同类型视图,响应这个消息

			for (int32 i=0 ; i<m_IoViewsPtr.GetSize(); i++)
			{
				CIoViewBase * pIoView = m_IoViewsPtr[i];
				if (NULL == pIoView)
				{
					continue;
				}
				if ( pIoView->GetRuntimeClass() == pIoViewSrc->GetRuntimeClass() )
				{
					if(bFont)
					{
						pIoView->OnIoViewFontChanged();
					}
					else
					{
						pIoView->OnIoViewColorChanged();
					}								
				}
			}
		}
		break;
	case CFaceScheme::EFERSameGroupIoView:
		{
			// 同分组视图
			for (int32 i=0 ; i<m_IoViewsPtr.GetSize(); i++)
			{	
				CIoViewBase * pIoView = m_IoViewsPtr[i];
				if (NULL == pIoView)
				{
					continue;
				}
				int32 iSrcGroupID = pIoViewSrc->GetIoViewGroupId();
				int32 iViewGroupID = pIoViewSrc->GetIoViewGroupId();
				//if ( pIoViewSrc->GetIoViewGroupId() == pIoView->GetIoViewGroupId())
				if(iSrcGroupID & iViewGroupID)
				{
					if(bFont)
					{
						pIoView->OnIoViewFontChanged();
					}
					else
					{
						pIoView->OnIoViewColorChanged();
					}
				}					
			}
		}
		break;
	case CFaceScheme::EFERSameTabSplitWnd:
		{
			// 同标签页视图
			for (int32 i=0 ; i<m_IoViewsPtr.GetSize(); i++)
			{	
				CIoViewBase * pIoView = m_IoViewsPtr[i];
				if (NULL == pIoView)
				{
					continue;
				}

				// 找IoViewSrc 的父亲是否有 TabSplitWnd:
				CWnd * pParentSrc = pIoViewSrc->GetParent();
				CTabSplitWnd * pParentSrcSptWnd = NULL;

				while( NULL != pParentSrc)
				{
					if ( pParentSrc->IsKindOf(RUNTIME_CLASS(CTabSplitWnd)))
					{
						pParentSrcSptWnd = (CTabSplitWnd*)pParentSrc;
						break;
					}
					pParentSrc = pParentSrc->GetParent();
				}

				if ( NULL == pParentSrcSptWnd)
				{
					// 源视图就没有这个父亲,直接返回
					return;
				}
				else
				{
					CWnd * pParentIoView = pIoView->GetParent();
					CTabSplitWnd * pParentIoViewSptWnd = NULL;

					while(pParentIoView)
					{
						if (pParentIoView->IsKindOf(RUNTIME_CLASS(CTabSplitWnd)))
						{
							pParentIoViewSptWnd = (CTabSplitWnd*)pParentIoView;
							break;
						}
						pParentIoView = pParentIoView->GetParent();
					}

					if (NULL != pParentIoViewSptWnd)
					{
						if ( pParentSrcSptWnd->GetIoViewPage(pIoViewSrc) == pParentIoViewSptWnd->GetIoViewPage(pIoView))
						{
							if (bFont)
							{
								pIoView->OnIoViewFontChanged();
							}
							else
							{
								pIoView->OnIoViewColorChanged();
							}
						}
					}

				}
			}
		}
		break;
	case CFaceScheme::EFERSameChildFrmae:
		{
			// 相同子窗口

			// 设置这个子窗口的风格数据:

			T_ChildFrameFaceObject ChildFrameObject;

			ChildFrameObject.m_pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, pIoViewSrc->GetParentFrame());
			if ( NULL == ChildFrameObject.m_pChildFrame )
			{
				return; // 无直接返回
			}
			CFaceScheme::Instance()->GetNowUseColors(ChildFrameObject.m_aColors);
			CFaceScheme::Instance()->GetNowUseFonts(ChildFrameObject.m_aFonts);

			CFaceScheme::Instance()->UpdateChildFrameFaceObjectList(ChildFrameObject);

			// 找到相应的视图,响应这个消息

			for (int32 i=0 ; i<m_IoViewsPtr.GetSize(); i++)
			{
				if ( pIoViewSrc->GetParentFrame() == m_IoViewsPtr[i]->GetParentFrame())
				{					
					if(bFont)
					{
						m_IoViewsPtr[i]->OnIoViewFontChanged();	
					}
					else
					{
						m_IoViewsPtr[i]->OnIoViewColorChanged(); 
					}
				}
			}
		}
		break;
	default:
		break;
	}
}

LRESULT CMainFrame::OnMsgFontChange(WPARAM wParam, LPARAM lParam)
{
	// 字体发生变化
	CIoViewBase * pIoViewSrc = (CIoViewBase *)wParam;
	ASSERT(NULL != pIoViewSrc);

	OnIoViewFaceChange(true,pIoViewSrc);

	return 0;
}

LRESULT CMainFrame::OnMsgColorChange(WPARAM wParam, LPARAM lParam)
{
	// 颜色发生变化
	CIoViewBase * pIoViewSrc = (CIoViewBase *)wParam;
	// ASSERT(NULL != pIoViewSrc);

	OnIoViewFaceChange(false, pIoViewSrc);

	CWnd *pWnd = CWnd ::FromHandle(m_hWndMDIClient)->GetWindow(GW_CHILD); 

	while ( NULL != pWnd )   
	{ 
		pWnd->RedrawWindow();
		pWnd = pWnd->GetWindow(GW_HWNDNEXT);		
	}

	return 0;
}

LRESULT CMainFrame::OnMsgMainFrameOnStatusBarDataUpdate(WPARAM wParam, LPARAM lParam)
{
	m_wndStatusBar.OnRealTimePriceUpdate();

	return 1;
}

LRESULT CMainFrame::OnMsgViewDataOnTimerFreeMemory(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;

	ASSERT(NULL!= pAbsCenterManager);

	pAbsCenterManager->OnMsgTypeResponse(EMSG_EMSGOnMsgViewDataOnTimerFreeMemory, (WPARAM)wParam, lParam);

	return 0;
}

LRESULT CMainFrame::OnMsgViewDataOnTimerSyncPushing(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	pAbsCenterManager->OnMsgTypeResponse(EMSG_OnMsgViewDataOnTimerSyncPushing, (WPARAM)wParam, lParam);

	return 0;
}


LRESULT CMainFrame::OnMsgViewDataOnTimerInitializeMarket(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	pAbsCenterManager->OnMsgTypeResponse(EMSG_OnMsgViewDataOnTimerInitializeMarket, (WPARAM)wParam, lParam);

	return 0;
}

LRESULT CMainFrame::OnMsgViewDataOnDataServiceConnected(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	pAbsCenterManager->OnMsgServiceConnected((int32)wParam);

	return 0;
}

LRESULT CMainFrame::OnMsgViewDataOnDataServiceDisconnected(WPARAM wParam, LPARAM lParam)
{


	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	pAbsCenterManager->OnMsgServiceDisconnected((int32)wParam);

	return 0;
}


LRESULT CMainFrame::OnMsgViewDataOnDataServerConnected(WPARAM wParam, LPARAM lParam)
{

	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	_MYTRACE(L"MainFrame收到ViewData 发送的服务器 %d 连接成功消息, 转回ViewData处理:", (int32)wParam);
	pAbsCenterManager->OnMsgServerConnected((int32)wParam);

	//
	if ( NULL != m_pDlgNetFlow )
	{
		m_pDlgNetFlow->OnServerReConnected((int32)wParam);
	}

	return 0;
}

LRESULT CMainFrame::OnMsgViewDataOnDataServerDisconnected(WPARAM wParam, LPARAM lParam)
{

	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	pAbsCenterManager->OnMsgServerDisconnected((int32)wParam);

	return 0;
}

LRESULT CMainFrame::OnMsgViewDataOnDataServerLongTimeNoRecvData(WPARAM wParam, LPARAM lParam)
{

	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	pAbsCenterManager->OnMsgServerLongTimeNoRecvData((int32)wParam, (E_ReConnectType)lParam);

	return 0;
}

LRESULT CMainFrame::OnMsgViewDataOnGeneralNormalUpdate(WPARAM wParam, LPARAM lParam)
{
	if (m_wndStatusBar.GetSafeHwnd())
	{
		m_wndStatusBar.OnDuoKongGeneralNormalUpdate();
	}

	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	pAbsCenterManager->OnMsgTypeResponse(EMSG_OnMsgOnGeneralNormalUpdate, (WPARAM)wParam, lParam);

	return 0;
}

LRESULT CMainFrame::OnMsgViewDataOnGeneralFinanaceUpdate(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	pAbsCenterManager->OnMsgTypeResponse(EMSG_OnMsgOnGeneralFinanaceUpdate, (WPARAM)wParam, lParam);

	return 0;
}

LRESULT CMainFrame::OnMsgViewDataOnMerchTrendIndexUpdate(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	pAbsCenterManager->OnMsgTypeResponse(EMSG_OnMsgOnMerchTrendIndexUpdate, (WPARAM)wParam, lParam);

	return 0;
}

LRESULT CMainFrame::OnMsgViewDataOnMerchAuctionUpdate(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	pAbsCenterManager->OnMsgTypeResponse(EMSG_OnMsgOnMerchAuctionUpdate, (WPARAM)wParam, lParam);

	return 0;
}

LRESULT CMainFrame::OnMsgViewDataOnMerchMinuteBSUpdate(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	pAbsCenterManager->OnMsgTypeResponse(EMSG_OnMsgOnMerchMinuteBSUpdate, (WPARAM)wParam, lParam);

	return 0;
}

LRESULT CMainFrame::OnMsgViewDataOnAuthPlugInHeart(WPARAM wParam, LPARAM lParam)
{
	CAbsCenterManager *pAbsCenterManager = AfxGetDocument()->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	pAbsCenterManager->OnMsgTypeResponse(EMSG_OnMsgAuthPlugInHeartResp, wParam, lParam);

	return 0;
}

LRESULT CMainFrame::OnMsgViewDataOnDataRequestTimeOut(WPARAM wParam, LPARAM lParam)
{

	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	pAbsCenterManager->OnMsgRequestTimeOut((int32)wParam);

	return 0;
}


LRESULT CMainFrame::OnMsgViewDataOnDataCommResponse(WPARAM wParam, LPARAM lParam)
{

	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	ASSERT(NULL != pApp);
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	// 如果当前消息栈还有这个一起处理了
	// 	MSG msg;
	// 	int32 iProc = 0;
	// 	while ( PeekMessage(&msg, m_hWnd, UM_ViewData_OnDataCommResponse, UM_ViewData_OnDataCommResponse, PM_REMOVE) )
	// 	{
	// 		++iProc;
	// 	}
	// 	if ( iProc > 0 )
	// 	{
	// 		UpdateWindow();
	// 		TRACE(_T("处理额外提取回包队列通知: %d个 \n"), iProc);
	// 	}

	pAbsCenterManager->OnMsgCommResponse();

	return 0;
}

LRESULT CMainFrame::OnMsgViewDataOnKLineNodeUpdate(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	ASSERT(pApp);
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	CMerch *pMerch = (CMerch *)wParam;
	if (pMerch != NULL)
	{
		pAbsCenterManager->OnMsgTypeResponse(EMSG_OnMsgKLineNodeUpdate, wParam, lParam);
	}

	return 0;
}

LRESULT CMainFrame::OnMsgIndexChsMideCoreKLineUpdate(WPARAM wParam, LPARAM lParam)
{
	int32 iReqId  = int32(lParam);

	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	ASSERT(pApp);
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);


	CMerch *pMerch = (CMerch *)wParam;
	if (pMerch != NULL)
	{
		CIndexChsStkMideCore *pIndexChsStkMideCore = pAbsCenterManager->GetIndexChsStkMideCore();
		if (pIndexChsStkMideCore && pIndexChsStkMideCore->BeIndexChsStkReqID(iReqId))
		{
			bool bFinish = 	pAbsCenterManager->IsRevFinishByIndexChsStk();
			pIndexChsStkMideCore->OnRecvKLineData(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, bFinish);
		}
	}

	return 0;
}

LRESULT CMainFrame::OnMsgViewDataOnTimesaleUpdate(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	CMerch *pMerch = (CMerch *)wParam;
	if (pMerch != NULL)
	{
		pAbsCenterManager->OnMsgTypeResponse(EMSG_OnMsgTimesaleUpdate, wParam, lParam);
	}

	return 0;
}

LRESULT CMainFrame::OnMsgViewDataOnNewsListUpdate(WPARAM wParam, LPARAM lParam)
{

	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	pAbsCenterManager->OnMsgTypeResponse(EMSG_OnMsgNewsListUpdate, wParam, lParam);

	return 0;
}

LRESULT CMainFrame::OnMsgViewDataOnF10Update(WPARAM wParam, LPARAM lParam)
{

	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	CMerch *pMerch = (CMerch *)wParam;
	if (NULL == pMerch)
	{
		return 0;
	}

	//
	pAbsCenterManager->OnMsgTypeResponse(EMSG_OnMsgF10Update, wParam, lParam);

	return 0;
}

LRESULT CMainFrame::OnMsgViewDataOnLandMineUpdate(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	CMerch *pMerch = (CMerch *)wParam;
	if (pMerch != NULL)
	{
		pAbsCenterManager->OnMsgTypeResponse(EMSG_OnMsgLandMineUpdate, wParam, lParam);
	}

	return 0;
}

LRESULT CMainFrame::OnMsgViewDataOnPublicFileUpdate(WPARAM wParam, LPARAM lParam)
{
	E_PublicFileType ePublicFileType = (E_PublicFileType)(lParam);

	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	CMerch *pMerch = (CMerch *)wParam;
	if (NULL == pMerch != NULL)
	{
		pAbsCenterManager->OnMsgTypeResponse(EMSG_OnMsgPublicFileUpdate, wParam, lParam);
	}

	return 0;
}

LRESULT CMainFrame::OnMsgViewDataOnRealtimePriceUpdate(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	CMerch *pMerch = (CMerch *)wParam;
	if (pMerch != NULL)
	{
		pAbsCenterManager->OnMsgTypeResponse(EMSG_OnMsgRealtimePriceUpdate, wParam, lParam);
		if ((NULL!=m_pSimulateTrade) && (ETLS_Logined==m_pSimulateTrade->m_eLoginStatus))
		{
			m_pSimulateTrade->OnMsgSimulateTradeQuotePush(pMerch);
		}
	}

	// 如果是最近浏览对话框关注的商品数据		
	if (NULL != m_pDlgRecentMerch )
	{
		for (int i = 0 ; i < m_aRecentMerchList.GetSize(); i++)
		{
			CMerch* pMerchTmp = m_aRecentMerchList[i].m_pMerch;

			if ( NULL == pMerchTmp )
			{
				continue;
			}

			//
			if ( NULL != pMerch && pMerch->m_pRealtimePrice->m_iMarketId == pMerchTmp->m_MerchInfo.m_iMarketId 
				&&  pMerch->m_pRealtimePrice->m_StrMerchCode.CompareNoCase(pMerchTmp->m_MerchInfo.m_StrMerchCode) == 0 )
			{
				m_pDlgRecentMerch->PostMessage(UM_RecentMerch_Update, (WPARAM)pMerchTmp, 0);															   
			}
		}
	}

	// 如果当前消息栈还有这个一起处理了
	MSG msg;

	int32 iProc = 0;
	CArray<MSG, const MSG &> aMsgs;
	set<CMerch *> merchsInMsg;
	while ( PeekMessage(&msg, m_hWnd, UM_ViewData_OnRealtimePriceUpdate, UM_ViewData_OnRealtimePriceUpdate, PM_REMOVE) )
	{
		pMerch = (CMerch *)msg.wParam;
		if (pMerch != NULL)
		{
			if ( merchsInMsg.count(pMerch) )
			{
				// 已经被保存了的商品，就不重复了
			}
			else
			{
				merchsInMsg.insert(pMerch);
				aMsgs.Add(msg);
			}
		}
		++iProc;
		if ( iProc >= 50 )
		{
			break;
		}
	}
	int32 i = 0;
	int32 iMsgSize = aMsgs.GetSize();
	const int32 iNeedProc = min(10, iMsgSize);
	for (i=0; i < iNeedProc ; ++i )
	{
		pAbsCenterManager->OnMsgTypeResponse(EMSG_OnMsgRealtimePriceUpdate, aMsgs[i].wParam, lParam);

		if ((NULL!=m_pSimulateTrade) && (ETLS_Logined==m_pSimulateTrade->m_eLoginStatus))
		{
			m_pSimulateTrade->OnMsgSimulateTradeQuotePush(pMerch);
		}
	}
	// 不能处理的全部丢到队列最后
	for ( i=iNeedProc; i < aMsgs.GetSize() ; ++i )
	{
		PostMessage(aMsgs[i].message, aMsgs[i].wParam, aMsgs[i].lParam);
	}

	//if ( iNeedProc > 0 )
	//{
	//	TRACE(_T("处理额外RealtimePrice通知(%d/%d): %d个 转移到队尾: %d个\n"), aMsgs.GetSize(), iProc, iNeedProc, aMsgs.GetSize()-iNeedProc);
	//}

	return 0;
}

LRESULT CMainFrame::OnMsgViewDataOnRealtimeLevel2Update(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	CMerch *pMerch = (CMerch *)wParam;
	if (pMerch != NULL)
	{
		pAbsCenterManager->OnMsgTypeResponse(EMSG_OnMsgRealtimeLevel2Update, wParam, lParam);
	}
	return 0;
}

LRESULT CMainFrame::OnMsgViewDataOnRealtimeTickUpdate(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	CMerch *pMerch = (CMerch *)wParam;
	if (pMerch != NULL)
	{
		pAbsCenterManager->OnMsgTypeResponse(EMSG_OnMsgRealtimeTickUpdate, wParam, lParam);
	}

	return 0;
}

LRESULT CMainFrame::OnMsgViewDataOnClientTradeTimeUpdate(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);
	pAbsCenterManager->OnMsgTypeResponse(EMSG_OnMsgClientTradeTimeUpdate, wParam, lParam);

	return 0;
}

LRESULT CMainFrame::OnMsgPackageLongTime(WPARAM wParam, LPARAM lParam)
{
	if( m_wndStatusBar.GetSafeHwnd() )
	{
		m_wndStatusBar.PromptLongTimeNoRecvData();
	}
	return 0;
}

LRESULT CMainFrame::OnMsgPackageTimeOut(WPARAM wParam, LPARAM lParam)
{
	if( m_wndStatusBar.GetSafeHwnd() )
	{
		m_wndStatusBar.PromptSomePackOutTime();
	}
	return 0;
}

LRESULT CMainFrame::OnUpdateSelfDrawBar(WPARAM wParam, LPARAM lParam)
{
	UpdateSelfDrawBar();
	return TRUE;
}

LRESULT CMainFrame::OnMsgMainFrameKickOut(WPARAM wParam, LPARAM lParam)
{
	CString* pStr = (CString*)wParam;
	ASSERT(NULL != pStr);

	if (NULL != pStr)
	{
		CString StrPrompt = _T("");
		StrPrompt =  *pStr;	
		delete pStr;
		MessageBox(StrPrompt, L"强制退出", MB_OK | MB_ICONINFORMATION);
		m_bShowLoginDlg = true;
		PostMessage(WM_CLOSE);		
	}

	return 0;
}


/////////////////////////////////////////////////////
//===================CODE CLEAN======================
//////////////////////////////////////////////////////
//LRESULT CMainFrame::OnMsgPickModelTypeInfo(WPARAM wParam, LPARAM lParam)
//{
//	CString* pStr = (CString*)wParam;
//	ASSERT(NULL != pStr);
//	CString StrPrompt;
//	if (NULL != pStr)
//	{
//		StrPrompt =  *pStr;	
//		delete pStr;
//	}
//
//	if (!StrPrompt.IsEmpty())
//	{
//		string strUnPack;
//		strUnPack = "";
//		strUnPack = CStringA(StrPrompt);
//		UnPackJsonPickModelTypeInfo(strUnPack);
//	}
//
//	m_mapPickModelTypeGroup;
//	
//
//	//vector<int>::iterator result ;
//	//m_mapPickModelTypeGroup.clear();
//	//map<int32,T_SmartStockPickItem> mapPickModelInfo;
//	//mapPickModelInfo.clear();
//	//mapPickModelInfo.insert(std::make_pair(11002, T_SmartStockPickItem(11002, L"一剑飞仙", L"追击涨停，积少成多，分享敢死队收益", 0)));
//	//mapPickModelInfo.insert(std::make_pair(11003, T_SmartStockPickItem(11003, L"青云直上", L"暴力拉升，捕捉即将进入主升浪的品种", 0)));
//	//mapPickModelInfo.insert(std::make_pair(11004, T_SmartStockPickItem(11004, L"神龙吸水", L"主力洗盘结束，正是低吸好时机", 0)));
//	//mapPickModelInfo.insert(std::make_pair(11005, T_SmartStockPickItem(11005, L"升龙逆天", L"万绿丛中一点红，逆势飙升揭示上升空间", 0)));
//	//result = find( m_vPickModelTypeGroup.begin( ), m_vPickModelTypeGroup.end( ), 100); //查找100
//	//if (result != m_vPickModelTypeGroup.end())
//	//{
//	//	m_mapPickModelTypeGroup.insert(std::make_pair(100, mapPickModelInfo));
//	//}
//
//	//mapPickModelInfo.clear();
//	//mapPickModelInfo.insert(std::make_pair(11006, T_SmartStockPickItem(11006, L"一剑飞仙", L"追击涨停，积少成多，分享敢死队收益", 0)));
//	//mapPickModelInfo.insert(std::make_pair(11007, T_SmartStockPickItem(11007, L"青云直上", L"暴力拉升，捕捉即将进入主升浪的品种", 0)));
//	//mapPickModelInfo.insert(std::make_pair(11008, T_SmartStockPickItem(11008, L"神龙吸水", L"主力洗盘结束，正是低吸好时机", 0)));
//	//mapPickModelInfo.insert(std::make_pair(11009, T_SmartStockPickItem(11009, L"升龙逆天", L"万绿丛中一点红，逆势飙升揭示上升空间", 0)));
//	//result = find( m_vPickModelTypeGroup.begin( ), m_vPickModelTypeGroup.end( ), 1002); //查找100
//	//if (result != m_vPickModelTypeGroup.end())
//	//{
//	//	m_mapPickModelTypeGroup.insert(std::make_pair(1002, mapPickModelInfo));
//	//}
//		
//	// 不判断是否需要重绘，后期增加
//	return 0;
//}
//
//LRESULT CMainFrame::OnMsgPickModelTypeStatus(WPARAM wParam, LPARAM lParam)
//{
//	CString* pStr = (CString*)wParam;
//	ASSERT(NULL != pStr);
//	CString StrPrompt;
//	if (NULL != pStr)
//	{
//		StrPrompt =  *pStr;	
//		delete pStr;
//	}
//
//	if (!StrPrompt.IsEmpty())
//	{
//		string strUnPack;
//		strUnPack = "";
//		strUnPack = CStringA(StrPrompt);
//		UnPackJsonPickModelTypeStatus(strUnPack);
//	}
//
//	bool bIsUpdateLeftBarStatus = false;
//	// 更新选股状态m_mapPickModelStatusGroup
//	map<int32, map<int32,T_SmartStockPickItem>>::iterator iterStatus;
//	for (iterStatus = m_mapPickModelStatusGroup.begin(); iterStatus != m_mapPickModelStatusGroup.end(); ++iterStatus)
//	{
//		int iTypeCode = iterStatus->first;
//		if (m_mapPickModelTypeGroup.count(iTypeCode) > 0)
//		{
//			map<int32,T_SmartStockPickItem>::iterator iterType;
//			for (iterType = iterStatus->second.begin(); iterType != iterStatus->second.end(); ++iterType)
//			{
//				int iModeId = iterType->first;
//				map<int32,T_SmartStockPickItem> &mapPickModel = m_mapPickModelTypeGroup[iTypeCode];
//				if (mapPickModel.count(iModeId) > 0)
//				{
//					if (mapPickModel[iModeId].m_ihasSmartStock != iterType->second.m_ihasSmartStock &&  iTypeCode == m_iSlectModelId)
//					{
//						bIsUpdateLeftBarStatus = true;
//					}
//
//					mapPickModel[iModeId].m_ihasSmartStock = iterType->second.m_ihasSmartStock;
//				}
//			}
//		}
//	}
//
//	if (bIsUpdateLeftBarStatus)
//	{
//		m_leftToolBar.ChangePickModelStatus();
//	}
//
//	return 0;
//}
//
//
//
//LRESULT CMainFrame::OnMsgPickModelTypeEvent(WPARAM wParam, LPARAM lParam)
//{
//	int32 iEventID = (int)wParam;
//	CString strModeCode;
//	CString strEventId;
//	strEventId.Format(_T("%d"), iEventID);
//
//	int iSelectModeId  =  m_iSlectModelId;
//	if (iSelectModeId != INVALID_ID)
//	{
//		if (m_mapPickModelTypeGroup.count(iSelectModeId) > 0)
//		{
//			map<int32,T_SmartStockPickItem>::iterator iter;		
//			for (iter = m_mapPickModelTypeGroup[iSelectModeId].begin(); iter != m_mapPickModelTypeGroup[iSelectModeId].end(); ++iter)
//			{
//				T_SmartStockPickItem &stSmartStock = iter->second;
//				if (stSmartStock.m_iModeId == iEventID)
//				{
//					strModeCode = stSmartStock.m_strmodeCode;
//					break;
//				}
//			}
//		}
//	}
//	
//	if (!strModeCode.IsEmpty() && iEventID - SMARTSTOCK_PICKMODEL_ID_BEGIN > 0)
//	{
//		CIoViewNews *pIoViewNews = FindIoViewNews(true);
//		if (pIoViewNews)
//		{	
//			CString strModeId;
//			strModeId.Format(_T("%d"), iEventID - SMARTSTOCK_PICKMODEL_ID_BEGIN );
//
//			pIoViewNews->SetCustomUrlQueryValue(_T("modeCode"), strModeCode);
//			pIoViewNews->SetCustomUrlQueryValue(_T("modeId"), strModeId);
//
//			UrlParser strUrlParser = pIoViewNews->GetCustomUrl();
//			CString str = strUrlParser.GetUrl();
//			pIoViewNews->RefreshWeb(str);
//		}
//	}
//	
//	return 0;
//}



LRESULT CMainFrame::OnMsgMainFrameManualOptimize(WPARAM wParam, LPARAM lParam)
{
	CDlgManual Dlg;
	typedef CArray<CServerState, CServerState&> arrServer;
	arrServer *paServersAftSort= (arrServer *)lParam;
	Dlg.SetInitialParams(*paServersAftSort);
	Dlg.DoModal();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 板块菜单
LRESULT CMainFrame::OnMsgViewDataOnAllMarketInitializeSuccess(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;	
	ASSERT(NULL!= pDocument);
	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	if ( NULL == pAbsCenterManager )
	{
		return 0;
	}

	// 设置TechIndexd.dll中需要的数据  zhangtao
	SetAbsCenterManager(pAbsCenterManager);
	//SetUserName(pAbsCenterManager->GetUserName());
	//SetServerTime(pAbsCenterManager->GetServerTime());

	//创建快捷键
	BuildHotKeyList();

	//pAbsCenterManager->SaveUserRightInfo();

	// 设置工作区信息
	if ( m_bNeedSetWspAfterInitialMarket )
	{
		const CString StrFileNameStock  = L"system0";
		const CString StrFileNameFuture = L"system1";
		const CString StrFileNameAll	= L"system2";

		// 新用户,自动选择默认的工作区:

		// 1: 得到用户类型:
		CGGTongApp::E_UserRightType eUserRightType = pApp->GetUserRightType();
		CString StrFileName = StrFileNameAll;

		// 2: 选择默认的工作区
		if ( CGGTongApp::EURTStock == eUserRightType )
		{
			StrFileName = StrFileNameStock;			
		}
		else if ( CGGTongApp::EURTFuture == eUserRightType )
		{
			StrFileName = StrFileNameFuture;
		}
		else if ( CGGTongApp::EURTAll == eUserRightType )
		{
			//	NULL;
		}	
		else 
		{
			//ASSERT(0);
		}

		bool32 bFind = false;

		for ( int32 i = 0; i < pApp->m_aWspFileInfo.GetSize(); i++ )
		{
			T_WspFileInfo WspFileInfo = pApp->m_aWspFileInfo.GetAt(i);		

			if ( WspFileInfo.m_StrFileName == StrFileName )
			{
				bFind = true;
				pApp->m_StrCurWsp = WspFileInfo.m_StrFilePath;
				break;
			}
		}

		if ( !bFind )
		{
			// 出错的时候,选第一个
			if ( pApp->m_aWspFileInfo.GetSize() > 0 )
			{
				pApp->m_StrCurWsp = pApp->m_aWspFileInfo.GetAt(0).m_StrFilePath;
			}
		}
	}

	//
	AdjustByOfflineMode();

	// 删除上次的错误文件
	COptimizeServer::Instance()->ClearErrServer();

	// 如果是自动模式, 而且本地和认证返回的一样. 登录成功后再ping 一次, 保存配置文件
	if ( COptimizeServer::Instance()->BeLocalAndAuthServersIdentical() && COptimizeServer::Instance()->BeAutoSortMode() )
	{
		COptimizeServer::Instance()->SortQuoteServersByPing();
	}

	// 菜单
	T_BlockMenuInfo  BlockMenuInfoToAdd;
	int32 i, iSize = pAbsCenterManager->GetMerchManager().m_BreedListPtr.GetSize();		

	//
	if (iSize >0)
	{
		bool32 bShangHaiZhengQuan = (pAbsCenterManager->GetMerchManagerType() & EMMT_StockCn) != 0;

		// 		CMenu* pMenu = this->GetMenu();
		// 	
		// 		CNewMenu* pFrameMenu = DYNAMIC_DOWNCAST(CNewMenu, pMenu);
		// 		pMenu = pFrameMenu->GetSubMenu(_T("板块(&B)"));
		// 		if(NULL == pMenu)
		// 			return 0;
		// 		CNewMenu* pRootMenu = DYNAMIC_DOWNCAST(CNewMenu, pMenu);	
		// 
		// 		{
		// 			int32 iCount = pRootMenu->GetMenuItemCount();
		// 			for ( int32 iDel=0; iDel < iCount ; iDel++  )
		// 			{
		// 				pRootMenu->RemoveMenu(0, MF_BYPOSITION);		// 清除以前的菜单项目
		// 			}
		// 		}

		int32 iIncrement = 0;

		// 		ASSERT(NULL != pRootMenu);
		for (int32 iIndex = 0 ;iIndex<iSize; iIndex++ )
		{
			CString StrBreedName= pAbsCenterManager->GetMerchManager().m_BreedListPtr[iIndex]->m_StrBreedCnName;
			// 			CNewMenu* SubPop =pRootMenu->AppendODPopupMenu(StrBreedName);
			//////////////////////////////////////////////////////////////////////////
			BlockMenuInfoToAdd.StrBlockMenuNames = StrBreedName;	
			BlockMenuInfoToAdd.iBlockMenuIDs     =  0;
			m_aBlockMenuInfo.Add(BlockMenuInfoToAdd);
			//////////////////////////////////////////////////////////////////////////
			// 			if (StrBreedName == L"香港证券" || StrBreedName == L"新加坡期货" || StrBreedName == L"全球指数")
			// 			{
			// 				pRootMenu->AppendODMenu(L"",MF_SEPARATOR);
			// 			}

			CBreed* pBreed = pAbsCenterManager->GetMerchManager().m_BreedListPtr.GetAt(iIndex);
			ASSERT( NULL != pBreed);

			for (i = 0; i<pBreed->m_MarketListPtr.GetSize(); i++)
			{
				CMarket * pMarket = pBreed->m_MarketListPtr.GetAt(i);
				ASSERT( NULL!= pMarket); 

				// 				CString StrName = pMarket->m_MarketInfo.m_StrCnName;
				// 				SubPop->AppendODMenu(StrName,MF_STRING,IDM_BLOCKMENU_BEGIN+iIncrement);
				// delete 指针.
				// 				T_BlockDesc*  pBlockDesc = new T_BlockDesc;
				// 				pBlockDesc->m_eType			= T_BlockDesc::EBTPhysical;
				// 				pBlockDesc->m_iMarketId		= pMarket->m_MarketInfo.m_iMarketId;
				// 				pBlockDesc->m_StrBlockName	= pMarket->m_MarketInfo.m_StrCnName;
				// 				pBlockDesc->m_pMarket		= pMarket;

				// 				SubPop->SetItemData(IDM_BLOCKMENU_BEGIN+iIncrement,(DWORD)(pBlockDesc));

				// 保存信息
				BlockMenuInfoToAdd.StrBlockMenuNames = pMarket->m_MarketInfo.m_StrCnName;
				BlockMenuInfoToAdd.iBlockMenuIDs	 = IDM_BLOCKMENU_BEGIN+iIncrement;
				m_aBlockMenuInfo.Add(BlockMenuInfoToAdd);
				iIncrement++;
				// 				if (pBlockDesc)
				// 				{
				// 					delete pBlockDesc;
				// 					pBlockDesc = NULL;
				// 				}
			}	
		}	

		// 		CArray<T_Block, T_Block&> aSysBlocks;
		// 		CSysBlockManager::Instance()->GetBlocks(aSysBlocks);

		// 		if ( pRootMenu->GetMenuItemCount() > 1 
		// 			&& pRootMenu->GetMenuItemID(pRootMenu->GetMenuItemCount()-1) != 0 )
		// 		{
		// 			pRootMenu->AppendMenu(MF_SEPARATOR);	// 如果没有separator，添加一个
		// 		}

		// 增加逻辑板块
		// 		UINT uCmdBlock = IDM_BLOCKMENU_BEGIN4;
		// 		if ( bShangHaiZhengQuan && CConfigInfo::FlagCaiFuJinDunQiHuo != CConfigInfo::Instance()->GetVersionFlag() )
		// 		{
		// 			CBlockConfig::IdArray aColIds;
		// 			CBlockConfig::Instance()->GetCollectionIdArray(aColIds);
		//  		CNewMenu *pLogicMenu = NULL;
		// 			if ( aColIds.GetSize() > 0 )
		// 			{
		//  			pLogicMenu = pRootMenu->AppendODPopupMenu(_T("股票板块"));
		// 				ASSERT( NULL != pLogicMenu );
		// 			}

		// 			for ( int i=0; i < aColIds.GetSize() && pLogicMenu != NULL ; i++ )
		// 			{
		// 				CBlockCollection *pCol = CBlockConfig::Instance()->GetBlockCollectionById(aColIds[i]);
		// 				CBlockConfig::BlockArray aBlocks;
		// 				pCol->GetValidBlocks(aBlocks);
		// 				if ( aBlocks.GetSize() > 0 
		// 					&& 
		// 					(aBlocks[0]->m_blockInfo.GetBlockType() == CBlockInfo::typeNormalBlock				// 普通 || 分类
		// 					|| aBlocks[0]->m_blockInfo.GetBlockType() == CBlockInfo::typeMarketClassBlock )
		// 					)
		// 				{
		// 					CNewMenu *pColMenu = pLogicMenu->AppendODPopupMenu(pCol->m_StrName);
		// 					ASSERT( NULL != pColMenu );
		// 					for ( int iBlock=0; iBlock < aBlocks.GetSize() ; iBlock++ )
		// 					{
		// 						ASSERT( uCmdBlock <= IDM_BLOCKMENU_END4 );
		// 						CString StrItem = aBlocks[iBlock]->m_blockInfo.m_StrBlockName;
		// 						if ( StrItem.GetLength() > 7 )
		// 						{
		// 							StrItem = StrItem.Left(6) + _T("...");
		// 						}
		// 						pColMenu->AppendODMenu(StrItem, MF_STRING, uCmdBlock);
		// 						pColMenu->SetItemData(uCmdBlock, aBlocks[iBlock]->m_blockInfo.m_iBlockId);
		// 
		// 						uCmdBlock++;
		// 					}
		// 					MultiColumnMenu(*pColMenu, 20);
		// 				}
		// 			}
		// 		}

		// 期货板块，如果存在的话
		// 		{
		// 			CBlockConfig::IdArray aColIds;
		// 			CBlockConfig::Instance()->GetCollectionIdArray(aColIds);
		// 			CNewMenu *pLogicMenu = NULL;
		// 			bool32 bFuture = FALSE;
		// 			for ( int i=0; i < aColIds.GetSize() ; ++i )
		// 			{
		// 				CBlockCollection *pCol = CBlockConfig::Instance()->GetBlockCollectionById(aColIds[i]);
		// 				CBlockConfig::BlockArray aBlocks;
		// 				pCol->GetValidBlocks(aBlocks);
		// 				if ( aBlocks.GetSize() > 0 
		// 					&& aBlocks[0]->m_blockInfo.GetBlockType() == CBlockInfo::typeFutureClassBlock				// 期货 )
		// 					)
		// 				{
		// 					bFuture = true;
		// 					break;
		// 				}
		// 			}
		// 			if ( bFuture && CConfigInfo::FlagCaiFuJinDunGuPiao != CConfigInfo::Instance()->GetVersionFlag() )
		// 			{
		// 				pLogicMenu = pRootMenu->AppendODPopupMenu(_T("期货板块"));
		// 				ASSERT( NULL != pLogicMenu );
		// 				for ( int i=0; i < aColIds.GetSize() && pLogicMenu != NULL ; i++ )
		// 				{
		// 					CBlockCollection *pCol = CBlockConfig::Instance()->GetBlockCollectionById(aColIds[i]);
		// 					CBlockConfig::BlockArray aBlocks;
		// 					pCol->GetValidBlocks(aBlocks);
		// 					if ( aBlocks.GetSize() > 0 
		// 						&& aBlocks[0]->m_blockInfo.GetBlockType() == CBlockInfo::typeFutureClassBlock				// 期货 )
		// 						)
		// 					{
		// 						CNewMenu *pColMenu = pLogicMenu->AppendODPopupMenu(pCol->m_StrName);
		// 						ASSERT( NULL != pColMenu );
		// 						for ( int iBlock=0; iBlock < aBlocks.GetSize() ; iBlock++ )
		// 						{
		// 							ASSERT( uCmdBlock <= IDM_BLOCKMENU_END4 );
		// 							CString StrItem = aBlocks[iBlock]->m_blockInfo.m_StrBlockName;
		// 							if ( StrItem.GetLength() > 7 )
		// 							{
		// 								StrItem = StrItem.Left(6) + _T("...");
		// 							}
		// 							pColMenu->AppendODMenu(StrItem, MF_STRING, uCmdBlock);
		// 							pColMenu->SetItemData(uCmdBlock, aBlocks[iBlock]->m_blockInfo.m_iBlockId);
		// 							
		// 							uCmdBlock++;
		// 						}
		// 						MultiColumnMenu(*pColMenu, 20);
		// 					}
		// 				}
		// 			}
		// 		}
		// 
		// 		// 用户板块
		// 		AppendUserBlockMenu(pRootMenu);
	}

	// PIC 菜单更新:
	//AppendPicMenu();

	//// 新增其它版本的菜单处理 xl 100928
	//{
	//	// 变更菜单中的菜单项名称
	//	// 关于 金盾选股
	//	{
	//		// 关于
	//		CMenu* pMenu = this->GetMenu();
	//		CNewMenu* pFrameMenu = DYNAMIC_DOWNCAST(CNewMenu, pMenu);
	//		pMenu = pFrameMenu->GetSubMenu(_T("帮助(&H)"));
	//		CNewMenu* pRootMenu = DYNAMIC_DOWNCAST(CNewMenu, pMenu);
	//		if ( NULL != pRootMenu )
	//		{
	//			CString StrAbout;
	//			StrAbout.Format(_T("关于 %s(&A)..."), AfxGetApp()->m_pszAppName);
	//			pRootMenu->SetMenuText(ID_APP_ABOUT, StrAbout, MF_BYCOMMAND);
	//		}
	//	}
	//}

	// 更改特色菜单名称
	InitSpecialMenu();

	// 增加打开与保存cfm选项

	// 初始化市场雷达，尝试启动
	CMarketRadarCalc::Instance().InitAndStartIf();

	// 初始化条件预警
	CAlarmCenter* pAlarmCenter = pDocument->m_pAarmCneter;
	if (NULL == pAlarmCenter)
	{
		pDocument->m_pAarmCneter = new CAlarmCenter(pAbsCenterManager);
		pAlarmCenter = pDocument->m_pAarmCneter;
	}

	if ( NULL != pAlarmCenter )
	{
		pAlarmCenter->FromXml();
	}


	// 初始化套利
	CArbitrageManage::Instance()->Initialize(pAbsCenterManager,pAlarmCenter);

	//
	m_wndStatusBar.CreateAlarmDialog(FALSE);


	if ( NULL != m_pDlgAlarm )
	{
		m_pDlgAlarm->SetCheckFlag();
	}

	if ( 0 == _taccess(CRecordDataCenter::GetSqlConfigName(), 0) )
	{
		if ( NULL == m_pRecordDataCenter )
		{
			m_pRecordDataCenter = new CRecordDataCenter(pAbsCenterManager);
			if ( NULL != m_pRecordDataCenter )
			{
				m_pRecordDataCenter->StartWork();
			}
		}
	}

	// 期货版本默认隐藏特色工具条
	if ( CConfigInfo::FlagCaiFuJinDunQiHuo == CConfigInfo::Instance()->GetVersionFlag()
		|| CConfigInfo::FlagCaiFuJinDunFree == CConfigInfo::Instance()->GetVersionFlag() )
	{
		OnViewSpecialBar();
	}
	//---wangyongxue 目前暂时未使用
	//	//状态栏今评 2013-7-22
	// 	{
	// 		//请求新闻标题
	// 		bool  bViewExist = false;
	// 
	// 		int32 iMenuCounts = CIoViewManager::GetIoViewObjectCount();
	// 
	// 		for ( int32 i=0 ; i<iMenuCounts ;i++)
	// 		{
	// 			if ( CIoViewManager::GetIoViewObject(i)->m_pIoViewClass == RUNTIME_CLASS(CIoViewTextNews) )
	// 				bViewExist = true;
	// 		}
	// 
	// 		if ( !bViewExist ) //视图不存在，则发送请求
	// 		{			
	// 			CMmmiNewsReqAddPush NewsReqAddPushJinPing;
	// 			NewsReqAddPushJinPing.m_iIndexID = pAbsCenterManager->GetJinpingID();
	// 
	// 			//
	// 			pAbsCenterManager->RequestNews(&NewsReqAddPushJinPing);
	// 		}
	// 	}

	// 自动升级timer
	SetTimer(KTimerIdAutoUpdateCheck, KTimerPeriodAutoUpdateCheck, NULL);

	// 金评
	// 	SetTimer(KTimerIdReqPushJinping, KTimerPeriodReqPushJinping, NULL);

	// 1s请求一次大盘状态
	SetTimer(KTimerIdDapanState, KTimerPeriodDapanState, NULL);

	// 1min检查token是否有效
	SetTimer(KTimerCheckToken, KTimerPeriodCheckToken, NULL);
	// 特殊交易时间请求
	CMmiReqTradeTime ReqTradeTime;
	ReqTradeTime.m_iTradeTimeID = 0;

	//
	pAbsCenterManager->RequestViewData(&ReqTradeTime);
	return 0;
}

//////////////////////////////////////////////////////////////////////////

void CMainFrame::handleMsgCenterRes(std::string strJson)
{
	if(strJson.empty())
	{
		return;
	}

	Json::Value MsgRoot;
	Json::Reader MsgReader;
	if(MsgReader.parse(strJson, MsgRoot))
	{
		if(MsgRoot.isNull())
		{
			return;
		}

		std::string strFC = MsgRoot["fc"].asString();
		int iFC = IsCombineByNum(strFC)?atoi(strFC.c_str()):0;

		Json::Value vData = MsgRoot["data"];

		switch (iFC)
		{
		case en_heartbeat:				// 心跳
			{
				//printf("收到心跳包.\n");
				handleHeartbeat(vData);
			}
			break;
		case en_set_device_id:			// 设置设备号
			{
			}
			break;
		case en_query_info_type_list:	// 查询资讯类型列表
			{
			}
			break;
		case en_query_msg_list:			// 查询消息列表
			{
				//
			}
			break;
		case en_query_info_content:		// 查询资讯内容
			{
				handleInfoContentQuery(vData);
			}
			break;
		case en_info_push:				// 推送资讯
			{
				handlePushInfo(vData);
			}
			break;
		case en_add_info:				// 添加资讯
			{
			}
			break;
		default:
			break;
		}
	}
}

void CMainFrame::ShowInfoCenterMsg(T_pushMsg stMsg)
{
	CRect rcWindow;
	GetWindowRect(&rcWindow);

	CRect rectMsg;
	rectMsg.right = rcWindow.right-10;
	rectMsg.left = rcWindow.right - iPushDlgWidth-10;
	rectMsg.bottom = rcWindow.bottom - iPushDlgBottomSpace;
	rectMsg.top = rectMsg.bottom - iPushDlgHeight;
	if (m_pDlgPushMsg)
	{
		m_pDlgPushMsg->SetMsg(stMsg);
		m_pDlgPushMsg->MoveWindow(&rectMsg);
		m_pDlgPushMsg->ShowDialog();
	}
}

void CMainFrame::ConnNotify(E_CONN_NOTIFY eConnNotify)
{
	if (en_connect_error == eConnNotify)
	{
		// 消息中心服务器连接出错
	}
	else if (en_connect_success == eConnNotify)// 消息中心服务器连接成功
	{
	}
}

// 资讯
LRESULT CMainFrame::OnMsgViewDataOnNewsResponse(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	pAbsCenterManager->OnMsgTypeResponse(EMSG_OnMsgNewsResponse, wParam, lParam);

	return 0;
}

LRESULT CMainFrame::OnMsgViewDataNewsServerConnected(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	pAbsCenterManager->OnMsgTypeResponse(EMSG_OnMsgNewsServerConnected, wParam, lParam);
	if ( NULL != m_pDlgNetFlow )
	{
		m_pDlgNetFlow->OnNewsServerReConnected((int32)wParam);
	}

	return 0;
}

LRESULT CMainFrame::OnMsgViewDataNewsServerDisConnected(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	pAbsCenterManager->OnMsgTypeResponse(EMSG_OnMsgNewsServerDisconnected, (WPARAM)wParam, lParam);

	return 0;
}

LRESULT CMainFrame::OnMsgDllTraceLog(WPARAM w, LPARAM l)
{
	CString* pStrLog = (CString*)l;

	CString StrLog = *pStrLog;
	_MYTRACE(StrLog);

	DEL(pStrLog);

	return 0;
}

//
CIoViewReport* CMainFrame::OnSpecifyBlock(T_BlockDesc &BlockDesc, bool32 bOpenExist )
{
	CIoViewReport *pIoViewReport = (bOpenExist ? FindIoViewReport(true): NULL);
	CMPIChildFrame *pActiveFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, MDIGetActive());
	if ( NULL != pActiveFrame && ((NULL == pIoViewReport && pActiveFrame->IsLockedSplit()) || !bOpenExist) )
	{
		// 当前页面无报价表，且锁定，则调出默认报价表操作
		bool32 bLoad = LoadSystemDefaultCfm(ID_PIC_REPORT, NULL);
		ASSERT( bLoad );
		if ( bLoad )
		{
			pIoViewReport = FindIoViewReport(true);
			ASSERT( NULL != pIoViewReport );
			if ( NULL != pIoViewReport )
			{
				pIoViewReport->OpenBlock(BlockDesc);
				pIoViewReport->BringToTop();
				return pIoViewReport;	// 如果不能运行到此，则允许以前的操作
			}
		}
	}

	// 虽然还是有可能创建报价表，但是是在空白窗口下，是可以允许的
	//if ( NULL != GetEmptyGGTongView())
	//{
	//	// 有空白窗口:
	//	CIoViewReport * pReport = (CIoViewReport *)CreateIoViewByPicMenuID(ID_PIC_REPORT, true);
	//	
	//	if (NULL != pReport)
	//	{
	//		pReport->OpenBlock(BlockDesc);
	//		CMPIChildFrame * pChildFrameToShow = DYNAMIC_DOWNCAST(CMPIChildFrame, pReport->GetParentFrame());
	//		// 如果这个frame最小化了，则还原 0001770 添加
	//		if ( pChildFrameToShow->IsIconic() )
	//		{
	//			pChildFrameToShow->ShowWindow(SW_SHOWNORMAL);
	//		}
	//		pChildFrameToShow->BringWindowToTop();
	//		//linhc 20100917添加设置显示板块视图为焦点视图并默认灰色条选中第一个商品
	//		pReport->BringToTop(BlockDesc.m_StrBlockName);
	//	}		
	//	return pReport;
	//}
	//
	//CIoViewReport * pIoViewReport = NULL;
	//pIoViewReport = NULL;
	CIoViewBase* pIoView = FindActiveIoView();

	if ( NULL != pIoView && pIoView->IsKindOf(RUNTIME_CLASS(CIoViewReport)))
	{
		// 如果是锁定的报价表就不要替换了
		if (!((CIoViewReport*)pIoView)->m_bLockReport)
		{
			pIoViewReport = (CIoViewReport*)pIoView;
		}
	}

	if ( NULL == pIoViewReport )
	{
		if ( NULL != pIoView && pIoView->GetIoViewGroupId() > 0 ) // 存在激活业务视图时，查找同分组的同frame的report - 允许report子类
		{
			pIoViewReport = (CIoViewReport *)FindIoViewByPicMenuId(ID_PIC_REPORT, pIoView, true, false, true, true, true); // 独立视图怎么处理？？
		}
		else
		{
			// 没有激活窗口或者独立视图，交给以前的处理吧
			pIoViewReport = FindIoViewReport(true);	 // 这个寻找到的有可能不是同一个分组的report，希望得到的是与当前激活ioview同分组的report
		}

		//		if (NULL == pIoViewReport)
		//			pIoViewReport = FindIoViewReport(false);

		if (NULL == pIoViewReport)
		{						
			CIoViewReport * pReport = (CIoViewReport *)CreateIoViewByPicMenuID(ID_PIC_REPORT, true);

			if (NULL != pReport)
			{
				pReport->OpenBlock(BlockDesc);
				// 如果这个frame最小化了，则还原
				CMPIChildFrame * pChildFrameToShow = DYNAMIC_DOWNCAST(CMPIChildFrame, pReport->GetParentFrame());
				if ( pChildFrameToShow->IsIconic() )
				{
					pChildFrameToShow->ShowWindow(SW_SHOWNORMAL);
				}
				pChildFrameToShow->BringWindowToTop();
				//linhc 20100917添加设置显示板块视图为焦点视图并默认灰色条选中第一个商品
				pReport->BringToTop(BlockDesc.m_StrBlockName);
			}			
			return pReport;
		}
	}

	// 找到这个IoViewReport 以后,设置为最前可见

	CMPIChildFrame * pChildFrameToShow = DYNAMIC_DOWNCAST(CMPIChildFrame, pIoViewReport->GetParentFrame());
	// 如果这个frame最小化了，则还原
	if ( pChildFrameToShow->IsIconic() )
	{
		pChildFrameToShow->ShowWindow(SW_SHOWNORMAL);
	}
	pChildFrameToShow->BringWindowToTop();

	// fangz1028#如果这个childframe 有 TabSplitWnd ,得到这个IoViewReport 所在Tab 页.设置为当前

	CTabSplitWnd * pTabSplitWnd = NULL;
	CWnd * pWnd = pIoViewReport->GetParent();

	while (pWnd)
	{
		pWnd = pWnd->GetParent();
		if (pWnd->IsKindOf(RUNTIME_CLASS(CTabSplitWnd)))
		{
			pTabSplitWnd = (CTabSplitWnd *)pWnd;
			break;
		}
		if ( pWnd == pChildFrameToShow )
		{
			break;
		}
	}

	if (pTabSplitWnd)
	{
		pTabSplitWnd->SetPageByIoView(pIoViewReport);
	}

	// xl 0607 找到所属的GGTongView，如果F7了，且与当前activeIoView同frame/tabWnd，则必须先还原F7
	CIoViewBase	*pIoViewActive = FindActiveIoView();
	if ( NULL != pIoViewActive 
		&& pIoViewActive != pIoViewReport
		&& pIoViewActive->GetParentFrame() == pIoViewReport->GetParentFrame()					   // 在同一个Frame下
		&& pIoViewActive->GetParentGGtongView() != pIoViewReport->GetParentGGtongView()			   // 不属于同一个GGTongView
		&& pIoViewActive->GetParentGGtongView()->GetMaxF7Flag()									   // F7最大化了
		)
	{
		bool32 bNeedF7Restore = true;
		if ( pTabSplitWnd )
		{
			if ( pTabSplitWnd->GetIoViewPage(pIoViewActive) == pTabSplitWnd->GetIoViewPage(pIoViewReport) )
			{
				// 同一个tab页下，需要还原
			}
			else
			{
				bNeedF7Restore = false;  // 不同页面，不相干
			}
		}

		if ( bNeedF7Restore )
		{
			OnProcessF7(pIoViewActive->GetParentGGtongView());
		}
	}

	//////////////////////////////////////////////////////////////////////////
	CIoViewManager * pIoViewManger =(CIoViewManager *)pIoViewReport->GetParent();
	for (int32 i = 0 ;i< pIoViewManger->m_IoViewsPtr.GetSize(); i++)
	{
		CIoViewBase * pIoView = pIoViewManger->m_IoViewsPtr[i];
		if (pIoView->IsKindOf(RUNTIME_CLASS(CIoViewReport)) && pIoView == pIoViewReport)
		{
			pIoViewManger->m_GuiTabWnd.SetCurtab(i);
			//				pIoViewManger->OnTabSelChange();
			break;
		}
	}

	//pIoViewReport->SetTabByBlockName(BlockDesc.m_StrBlockName);
	pIoViewReport->AddBlock(BlockDesc);
	//pIoViewReport->ReCreateTabWnd();
	//linhc 20100917添加设置显示板块视图为焦点视图并默认灰色条选中第一个商品
	pIoViewReport->BringToTop(BlockDesc.m_StrBlockName);
	return pIoViewReport;
}

void CMainFrame::OpenSpecifyMarketOrBlock(int32 iMarketID)
{
	CAbsCenterManager* pAbsCenterManager = AfxGetDocument()->m_pAbsCenterManager;
	if (NULL == pAbsCenterManager)
	{
		return;
	}

	// 是不是物理市场
	CMarket* pMarket = NULL;
	if (pAbsCenterManager->GetMerchManager().FindMarket(iMarketID, pMarket) && NULL != pMarket)
	{
		T_BlockDesc BlockDesc;

		BlockDesc.m_eType			= T_BlockDesc::EBTPhysical;
		BlockDesc.m_iMarketId		= pMarket->m_MarketInfo.m_iMarketId;
		BlockDesc.m_StrBlockName	= pMarket->m_MarketInfo.m_StrCnName;
		BlockDesc.m_pMarket		= pMarket;		

		OnSpecifyBlock(BlockDesc);
		return;
	}

	// 是不是逻辑板块
	CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(iMarketID);
	if ( NULL != pBlock )
	{
		T_BlockDesc desc;
		desc.m_StrBlockName = pBlock->m_blockInfo.m_StrBlockName;
		desc.m_eType		= T_BlockDesc::EBTBlockLogical;
		desc.m_iMarketId	= iMarketID;
		desc.m_aMerchs.Copy(pBlock->m_blockInfo.m_aSubMerchs);
		OnSpecifyBlock(desc);
	}
}

//////////////////////////////////////////////////////////////////////////
// 分析菜单, 保证resource.h 中定义的ID 与 IoViewManager 数组的顺序一致

void CMainFrame::LoadPicMenu()
{
	CMenu * pMenu = this->GetMenu();	
	CNewMenu * pNewMenu = DYNAMIC_DOWNCAST(CNewMenu,pMenu);
	pNewMenu->LoadToolBar(g_awToolBarIconIDs);

	//CMenu * pSubMenu = pNewMenu->GetSubMenu(L"分析(&F)");
	CMenu * pSubMenu = GetAnalysisMenu(false);	// 非锁定菜单
	ASSERT( NULL != pSubMenu );
	CNewMenu * pRootMenu = DYNAMIC_DOWNCAST(CNewMenu,pSubMenu);
	ASSERT( NULL != pRootMenu );

	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;	
	ASSERT(NULL!= pDocument);
	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT( NULL != pAbsCenterManager );

	int lli =0;
	lli++;

	DWORD dwMerchManagerType = pAbsCenterManager->GetMerchManagerType();
	//ASSERT( 0 != dwMerchManagerType );
	int32 iMenuCounts = CIoViewManager::GetIoViewObjectCount();

	int32 i = 0;
	//pSubMenu->AppendMenu(MF_SEPARATOR);
	for ( i=0 ; i<iMenuCounts ;i++)
	{
		CString MenuName = CIoViewManager::GetIoViewObject(i)->m_StrLongName;

		// 先不加 "报价表" 这个菜单: 等板块信息读取完成后再加上完整的报价表:
		const T_IoViewObject *pIoViewObj = CIoViewManager::GetIoViewObject(i);

		if ( pIoViewObj->m_uID == ID_PIC_PHASESORT )
		{
			continue;		// 阶段排行插入到报价表后面
		}

		if ( pIoViewObj->m_pIoViewClass != RUNTIME_CLASS(CIoViewReport))
		{
			// if ( CIoViewManager::GetIoViewObject(i)->m_uID < ID_PIC_HIDE_INMENU )		// 只有小于这个id才能在菜单中显示 - Main中要出现
			// 更具当前初始化的商品列表灵活选择合适的出现 xl 1020
			if ( (dwMerchManagerType & pIoViewObj->m_dwAttendMerchType) != 0 )
			{
				//pRootMenu->AppendODMenu(MenuName,MF_STRING,ID_PIC_BEGIN + i);
				pRootMenu->AppendODMenu(MenuName,MF_STRING, pIoViewObj->m_uID);
			}
		}		
	}

	//pRootMenu->RemoveMenu(0, MF_BYPOSITION|MF_SEPARATOR);


	// 锁定菜单
	pRootMenu = GetAnalysisMenu(true);
	pSubMenu =  pRootMenu;
	//pSubMenu->AppendMenu(MF_SEPARATOR);
	for ( i=0; i < KPicCfmInfosCount ; i++ )
	{
		const T_PicCfmInfo &info = s_aPicCfmInfos[i];
		const T_IoViewObject *pIoViewObj = CIoViewManager::FindIoViewObjectByPicMenuID(info.nPicId);

		if ( NULL != pIoViewObj )
		{
			pRootMenu->AppendMenu(MF_STRING, info.nPicId, pIoViewObj->m_StrLongName);
		}
	}
	//pSubMenu->RemoveMenu(0, MF_BYPOSITION);
	// 附加综合排名 行情/财务栏目切换 热门板块分析 大盘走势
	pRootMenu->AppendMenu(MF_STRING, ID_ZongHePaiMing, _T("综合排名"));
	//pRootMenu->AppendMenu(MF_STRING, ID_HangQingCaiWu, _T("行情/财务栏目切换"));
	//pRootMenu->AppendMenu(MF_STRING, ID_ReMenBanKuai,  _T("热门板块分析"));
	//pRootMenu->AppendMenu(MF_STRING, ID_DaPanZouShi,   _T("大盘走势"));

	pRootMenu->AppendMenu(MF_SEPARATOR);
	pRootMenu->AppendMenu(MF_STRING, ID_PIC_CFM_MULTIINTERVAL, _T("多周期同列"));
	pRootMenu->AppendMenu(MF_STRING, ID_PIC_CFM_MULTIMERCH,	   _T("多合约同列")); //2013-8-8 modify cym

	ChangeAnalysisMenu(false);	// 默认载入非锁定菜单
}

void CMainFrame::AppendPicMenu()
{
	// 加上报价表	
	//	CMenu * pMenu = this->GetMenu();
	//	CNewMenu *pNewMenu = DYNAMIC_DOWNCAST(CNewMenu,pMenu);
	//CMenu * pSubMenu = pNewMenu->GetSubMenu(L"分析(&F)");
	CMenu * pSubMenu = GetAnalysisMenu(false);
	ASSERT( NULL != pSubMenu );
	CNewMenu * pRootMenu = DYNAMIC_DOWNCAST(CNewMenu,pSubMenu);
	ASSERT( NULL != pRootMenu );

	CString StrMenuName = CIoViewManager::FindIoViewObjectByRuntimeClass(RUNTIME_CLASS(CIoViewReport))->m_StrLongName;			
	pRootMenu->LoadToolBar(g_awToolBarIconIDs);

	//
	CNewMenu * pMenuReport = pRootMenu->AppendODPopupMenu(StrMenuName);
	pRootMenu->ModifyODMenu(StrMenuName, StrMenuName, IDB_TOOLBAR_REPORT);

	// 报价表下面加子菜单
	if ( NULL != pMenuReport )
	{	
		// 变成 默认报价表类型 股票类型 期货类型...
		// 这个菜单本身就是处于非锁定的
		const SimpleTabClassMap &simMap = CIoViewReport::GetSimpleTabClassConfig();
		for ( SimpleTabClassMap::const_iterator it=simMap.begin(); it != simMap.end() ; ++it )
		{
			CString StrItem = it->second.StrName;
			if ( StrItem.IsEmpty() )
			{
				if ( it->second.iId == SIMPLETABINFOCLASS_DEFAULTID )
				{
					StrItem = _T("默认报价表类型");
				}
				else
				{
					StrItem.Format(_T("报价表类型%d"), it->second.iId);
				}
			}
			UINT nId = it->second.iId+ID_REPORT_TYPEBEGIN;
			ASSERT( nId >= ID_REPORT_TYPEBEGIN && nId <= ID_REPORT_TYPEEND );
			pMenuReport->AppendODMenu(StrItem, MF_STRING|MF_BYCOMMAND, nId);
		}
	}

	// 报价表下面再添加一个 阶段排行菜单
	{
		const T_IoViewObject *pIoViewObj = CIoViewManager::FindIoViewObjectByPicMenuID(ID_PIC_PHASESORT);

		if ( NULL != pIoViewObj )
		{
			pRootMenu->AppendMenu(MF_STRING, ID_PIC_PHASESORT, pIoViewObj->m_StrLongName);
		}
	}
}

void CMainFrame::NewDefaultIoViewChildFrame(int32 iIoViewGroupId)
{	
	// 新建一个子窗口, 内包含默认的ioview
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();

	POSITION   pos  = pApp->m_pDocTemplate->GetFirstDocPosition();
	if ( NULL == pos)
	{
		pApp->m_pDocument = (CGGTongDoc *)pApp->m_pDocTemplate->CreateNewDocument();
		pos  = pApp->m_pDocTemplate->GetFirstDocPosition();
	}

	CRect RectActive = CalcActiveChildSize();

	CMPIChildFrame * pChildFrame = pApp->m_pDocTemplate->CreateMPIFrame(IDR_MAINFRAME);

	CString StrXML;
	StrXML = pChildFrame->GetDefaultXML(false,RectActive);

	//fangz0714no5
	char KStrXML[10240];
	UNICODE_2_MULTICHAR(EMCCUtf8,StrXML,KStrXML);

	TiXmlDocument doc("");
	doc.Parse(KStrXML);
	TiXmlElement *pElement = doc.FirstChildElement(); 
	pChildFrame->FromXml(pElement);	
	pChildFrame->SetActiveGGTongView();

	// 查找到该唯一的业务视图， 设置其业务分组编号
	CGGTongView *pGGTongViewDefault = FindGGTongView(pChildFrame, NULL);
	ASSERT(NULL != pGGTongViewDefault);

	pGGTongViewDefault->m_IoViewManager.SetGroupID(iIoViewGroupId);
}

// 
CIoViewBase* CMainFrame::FindNotReportSameGroupIdVisibleIoview(CIoViewBase *pActiveIoView, bool32 bOnlyInSameChildFrame)
{	

	// 查找指定的业务视图属哪个childframe
	CFrameWnd *pReportParentFrame = NULL;
	if (NULL != pActiveIoView)
	{
		if (pActiveIoView->GetIoViewGroupId() <= 0)		// 独立视图， 没有关联视图
			return NULL;

		pReportParentFrame = pActiveIoView->GetParentFrame();
	}
	else
	{
		if (bOnlyInSameChildFrame)	// 根本不可能有同一个父窗口
			return NULL;
	}

	// 挨个查找各个业务视图， 是否符合条件
	for (int32 i = 0; i < m_IoViewsPtr.GetSize(); i++)
	{
		CIoViewBase *pIoView = m_IoViewsPtr[i];

		// not report
		if (pIoView->IsKindOf(RUNTIME_CLASS(CIoViewReport)))
			continue;

		// same groupid
		if (NULL != pActiveIoView)
		{
			int32 iViewGroupID = pIoView->GetIoViewGroupId();
			int32 iACtiveViewGroupID = pActiveIoView->GetIoViewGroupId();

			//if (pIoView->GetIoViewGroupId() != pActiveIoView->GetIoViewGroupId())
			if(!(iViewGroupID & iACtiveViewGroupID ))
				continue;
		}

		// same childframe
		if (bOnlyInSameChildFrame)
		{
			if (pIoView->GetParentFrame() != pReportParentFrame)
				continue;
		}


		// visible
		if (!pIoView->IsWindowVisible())
			continue;

		bool32 bVisible = false;
		CWnd* pParentWnd =  pIoView->GetParent();  
		while (pParentWnd != NULL)
		{
			if (!pParentWnd->IsWindowVisible())
				break;

			if (pParentWnd->IsFrameWnd())
			{
				bVisible = true;
				break;
			}
			pParentWnd = pParentWnd->GetParent();
		}

		if (!bVisible)
			continue;

		// 总算找到了合适的了 
		return pIoView;
	}

	return NULL;
}


CIoViewBase * CMainFrame::FindIoViewByPicMenuId( UINT nPicID, CIoViewBase *pGroupIoView, bool32 bInSameGroup, bool32 bMustVisible, bool32 bOnlyInSameChildFrame, bool32 bNotInSameManager, bool32 bEnableSonObject/* = false*/ )
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDoc = (CGGTongDoc *)pApp->m_pDocument;
	ASSERT( NULL != pDoc );
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	ASSERT( NULL != pAbsCenterManager );
	if (!pAbsCenterManager)
	{
		return NULL;
	}

	const T_IoViewObject *pObjToFind = CIoViewManager::FindIoViewObjectByPicMenuID(nPicID);		// 根据类来查找，不使用id绝对查找
	if ( NULL == pObjToFind || NULL == pObjToFind->m_pIoViewClass )
	{
		return NULL;
	}

	// 检验参数
	// 查找指定的业务视图属哪个childframe
	CFrameWnd *pReportParentFrame = NULL;
	if ( IsWindow(pGroupIoView->GetSafeHwnd()) )
	{
		if ( pGroupIoView->GetIoViewGroupId() <= 0 && bInSameGroup )		// 独立视图， 没有关联视图
		{
			// 如果本身就是要查找的，返回自己
			if ( !bEnableSonObject )
			{
				return pObjToFind->m_pIoViewClass==pGroupIoView->GetRuntimeClass() ? pGroupIoView : NULL;
			}
			return  pGroupIoView->IsKindOf(pObjToFind->m_pIoViewClass) ? pGroupIoView : NULL ;
		}

		pReportParentFrame = pGroupIoView->GetParentFrame();
	}
	else
	{
		pGroupIoView = NULL;
		if ( bOnlyInSameChildFrame || bInSameGroup || bNotInSameManager )	// 必须要求指定有效pGroupIoView
		{
			ASSERT( 0 );
			return NULL;
		}
	}

	// 以前还是有没有想到的，还是需要改善 TODO
	// 查找顺序 
	//			-> 参数视图本身	0 -> 同manager下页面 1
	//			-> 激活页面 2-> 同manager下页面 3 (如果不是参数视图,不与其同manager且与参数视图同childframe, 否则取消该行)
	//			-> 同组同childframe下视图 可见 不可见4
	//			-> 同childframe下可见的页面 5 -> 同childframe下不可见页面 6
	//			-> 不同childfr下可见页面 7 -> 不同childframe下不可见页面 8
	typedef	vector<CIoViewBase *>	IoViewArray;
	typedef vector<IoViewArray >	IoViewArrayArray;
	typedef map<CIoViewBase *, int>	IoViewMap;
	IoViewArrayArray	aIoViewsToSearch;
	IoViewMap			mapIoViewsSrc;
	{
		for ( int i=0; i < m_IoViewsPtr.GetSize() ; i++ )
		{
			CIoViewBase *pIoView = m_IoViewsPtr[i];
			if ( !IsWindow(pIoView->GetSafeHwnd()) )
			{
				ASSERT( 0 );
				continue;
			}

			mapIoViewsSrc[ pIoView ] = 1;
		}
	}

	// 慢慢找哈, 有重复的-- 
	IoViewArray	aIoViewsTmp;

	// 序列化
	if ( NULL != pGroupIoView )
	{
		aIoViewsTmp.push_back(pGroupIoView);
		mapIoViewsSrc.erase(pGroupIoView);
		CIoViewManager *pManager = NULL;
		if ( IsWindow((pManager=pGroupIoView->GetIoViewManager())->GetSafeHwnd()) )
		{
			for ( int i=0; i < pManager->m_IoViewsPtr.GetSize() ; i++ )
			{
				CIoViewBase *pBase = pManager->m_IoViewsPtr[i];
				if ( pBase != pGroupIoView && IsWindow(pBase->GetSafeHwnd()) )
				{
					aIoViewsTmp.push_back(pBase);
					mapIoViewsSrc.erase(pBase);
				}
			}
		}
	}

	CIoViewBase *pActive = FindActiveIoView();
	if ( NULL != pActive 
		&& pActive != pGroupIoView
		&& pActive->GetIoViewManager() != NULL
		&& pActive->GetIoViewGroupId() >= 0
		)
	{
		aIoViewsTmp.push_back(pActive);
		mapIoViewsSrc.erase(pActive);

		bool32 bAddLine = true;
		if ( NULL != pGroupIoView )
		{
			bool32 bSameManager =  pActive->GetIoViewManager() == pGroupIoView->GetIoViewManager();
			//bool32 bSameGroup	=  pActive->GetIoViewGroupId() == pGroupIoView->GetIoViewGroupId();
			bool32 bSameChildFrame = pActive->GetParentFrame() == pGroupIoView->GetParentFrame();
			bAddLine = !bSameManager && bSameChildFrame;
		}
		if ( bAddLine )
		{
			CIoViewManager *pManager = pActive->GetIoViewManager();
			for ( int i=0; i < pManager->m_IoViewsPtr.GetSize() ; i++ )
			{
				CIoViewBase *pBase = pManager->m_IoViewsPtr[i];
				if ( pBase != pActive && IsWindow(pBase->GetSafeHwnd()) )
				{
					aIoViewsTmp.push_back(pBase);
					mapIoViewsSrc.erase(pBase);
				}
			}
		}
	}

	if ( NULL != pGroupIoView )
	{
		// 同组同frame
		IoViewArray aShow, aHide;
		IoViewMap mapTmp = mapIoViewsSrc;
		for ( IoViewMap::iterator it = mapTmp.begin() ; it != mapTmp.end() ; it++ )
		{
			CIoViewBase *pIoView = it->first;
			if ( !IsWindow(pIoView->GetSafeHwnd()) )
			{
				ASSERT( 0 );
				continue;
			}
			int32 iViewGroupID = pIoView->GetIoViewGroupId();
			int32 iACtiveGroupID = pGroupIoView->GetIoViewGroupId();
			if ( (iViewGroupID & iACtiveGroupID)/*pIoView->GetIoViewGroupId() == pGroupIoView->GetIoViewGroupId()*/
				&& pIoView->GetParentFrame() == pGroupIoView->GetParentFrame() )
			{
				if ( pIoView->IsWindowVisible() )
				{
					aShow.push_back(pIoView);
				}
				else
				{
					aHide.push_back(pIoView);
				}
				mapIoViewsSrc.erase(pIoView);
			}
		}
		aIoViewsTmp.insert(aIoViewsTmp.end(), aShow.begin(), aShow.end());
		aIoViewsTmp.insert(aIoViewsTmp.end(), aHide.begin(), aHide.end());
	}

	{
		// 同frame
		IoViewArray aShow, aHide;
		IoViewMap mapTmp = mapIoViewsSrc;
		for ( IoViewMap::iterator it = mapTmp.begin() ; it != mapTmp.end() ; it++ )
		{
			CIoViewBase *pIoView = it->first;
			if ( !IsWindow(pIoView->GetSafeHwnd()) )
			{
				ASSERT( 0 );
				continue;
			}

			if (NULL != pGroupIoView && NULL != pIoView && (pIoView->GetParentFrame() == pGroupIoView->GetParentFrame()) )
			{
				if ( pIoView->IsWindowVisible() )
				{
					aShow.push_back(pIoView);
				}
				else
				{
					aHide.push_back(pIoView);
				}
				mapIoViewsSrc.erase(pIoView);
			}
		}
		aIoViewsTmp.insert(aIoViewsTmp.end(), aShow.begin(), aShow.end());
		aIoViewsTmp.insert(aIoViewsTmp.end(), aHide.begin(), aHide.end());
	}

	{
		// 剩下的
		IoViewArray aShow, aHide;
		IoViewMap mapTmp = mapIoViewsSrc;
		for ( IoViewMap::iterator it = mapTmp.begin() ; it != mapTmp.end() ; it++ )
		{
			CIoViewBase *pIoView = it->first;
			if ( !IsWindow(pIoView->GetSafeHwnd()) )
			{
				ASSERT( 0 );
				continue;
			}

			if ( pIoView->IsWindowVisible() )
			{
				aShow.push_back(pIoView);
			}
			else
			{
				aHide.push_back(pIoView);
			}
			//mapIoViewsSrc.erase(pIoView);

		}
		aIoViewsTmp.insert(aIoViewsTmp.end(), aShow.begin(), aShow.end());
		aIoViewsTmp.insert(aIoViewsTmp.end(), aHide.begin(), aHide.end());
	}

	// 挨个查找各个业务视图， 是否符合条件
	for ( IoViewArray::iterator it = aIoViewsTmp.begin(); it != aIoViewsTmp.end(); it++)
	{
		CIoViewBase *pIoView = *it;
		if ( !IsWindow(pIoView->GetSafeHwnd()) )
		{
			ASSERT( 0 );
			continue;
		}

		// pic id
		const T_IoViewObject *pIoObj = CIoViewManager::FindIoViewObjectByIoViewPtr(pIoView);
		if ( !bEnableSonObject )
		{
			if ( NULL == pIoObj || pIoObj->m_uID != nPicID )
			{
				continue;
			}
		}
		else
		{
			if ( pIoObj == NULL  || pIoObj->m_pIoViewClass == NULL || !pIoObj->m_pIoViewClass->IsDerivedFrom(pObjToFind->m_pIoViewClass) )
			{
				// 使用类型判断，如果是要查找的子类，也把它翻出来
				continue;
			}
		}

		// same group id
		if ( NULL != pGroupIoView && bInSameGroup )
		{
			int32 iViewGroupID = pIoView->GetIoViewGroupId();
			int32 iACtiveGroupID = pGroupIoView->GetIoViewGroupId();

			//if (pIoView->GetIoViewGroupId() != pGroupIoView->GetIoViewGroupId())
			if(!(iViewGroupID & iACtiveGroupID ))
				continue;
		}

		// must visible
		if ( bMustVisible )
		{
			if ( !pIoView->IsWindowVisible() )
			{
				continue;
			}
		}

		// same childframe
		if ( bOnlyInSameChildFrame )
		{
			if (pIoView->GetParentFrame() != pReportParentFrame)
				continue;
		}

		// not in same manager
		if ( bNotInSameManager )
		{
			if ( NULL != pGroupIoView && (pIoView->GetIoViewManager() == pGroupIoView->GetIoViewManager()) )
			{
				continue;
			}
		}

		// 总算找到了合适的了 
		return pIoView;
	}

	return NULL;
}


CIoViewBase * CMainFrame::GetIoViewByPicMenuIdAndActiveIoView( UINT nPicId, bool32 bEnableCreate, bool32 bInSameGroup, bool32 bMustVisible, bool32 bOnlyInSameChildFrame, bool32 bCanNotInSameManager, bool32 bEnableSonObject /*= false*/ )
{
	CIoViewBase *pIoViewActive = FindActiveIoView();
	CIoViewBase *pIoViewGet = NULL;

	if ( NULL != pIoViewActive )
	{
		pIoViewGet = FindIoViewByPicMenuId(nPicId, pIoViewActive, bInSameGroup, bMustVisible, bOnlyInSameChildFrame, bCanNotInSameManager, bEnableSonObject);
	}

	if ( NULL == pIoViewGet && bEnableCreate )
	{
		pIoViewGet = CreateIoViewByPicMenuID(nPicId, true, NULL);
		ASSERT( IsWindow(pIoViewGet->GetSafeHwnd()) );
	}

	return pIoViewGet;
}

//	简化查询算法，旧的的似乎太复杂了。 add by weng.cx
BOOL CMainFrame::FindIoViewInFrame(OUT vector<CIoViewBase*>& outVt,  UINT nPicID, CMDIChildWnd *pChildFrame, bool32 bMustVisible/* = false*/, bool32 bEnableInherit/*=true*/, int32 iGroupId/*=-1*/ )
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDoc = (CGGTongDoc *)pApp->m_pDocument;
	ASSERT( NULL != pDoc );
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	ASSERT( NULL != pAbsCenterManager );
	if (!pAbsCenterManager)
	{
		return false;
	}

	// 检验参数
	// 查找指定的业务视图属哪个childframe
	if ( NULL == pChildFrame )
	{
		pChildFrame = MDIGetActive();
		if ( NULL == pChildFrame )
		{
			return FALSE;	// 无打开的页面
		}
	}

	CIoViewBase *pIoViewActive =  NULL;
	CGGTongView *pActiveView = DYNAMIC_DOWNCAST(CGGTongView, pChildFrame->GetActiveView());
	if ( NULL != pActiveView )
	{
		pIoViewActive = pActiveView->m_IoViewManager.GetActiveIoView();
	}
	else
	{
		return FALSE;
	}
	
	outVt.clear();
	const T_IoViewObject *pObjToFind = CIoViewManager::FindIoViewObjectByPicMenuID(nPicID);
	if(NULL == pObjToFind)
	{
		return FALSE;
	}

	for ( int i=0; i < m_IoViewsPtr.GetSize() ; i++ )
	{
		CIoViewBase *pIoView = m_IoViewsPtr[i];
		if ( !IsWindow(pIoView->GetSafeHwnd()) )
		{
			ASSERT( 0 );
			continue;
		}

		if ( pIoView->GetParentFrame() != pChildFrame )
		{
			continue;
		}

		//	是否需要显示
		if (bMustVisible)
		{
			if (!pIoView->IsWindowVisible())
			{
				continue;
			}
		}

		if (bEnableInherit)
		{
			const T_IoViewObject *pIoObj = CIoViewManager::FindIoViewObjectByIoViewPtr(pIoView);
			if(!pIoObj->m_pIoViewClass->IsDerivedFrom(pObjToFind->m_pIoViewClass))
			{
				continue;
			}
		}

		if (iGroupId > 0)
		{
			if (iGroupId == pIoView->GetIoViewGroupId())
			{
				outVt.push_back(pIoView);
			}			
		}
		else
		{
			outVt.push_back(pIoView);
		}		
	}
	return TRUE;
}

CIoViewBase * CMainFrame::FindIoViewInFrame( UINT nPicID, CMDIChildWnd *pChildFrame, bool32 bMustVisible/* = false*/, bool32 bEnableInherit/*=true*/, int32 iGroupId/*=-1*/ )
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDoc = (CGGTongDoc *)pApp->m_pDocument;
	ASSERT( NULL != pDoc );
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	ASSERT( NULL != pAbsCenterManager );
	if (!pAbsCenterManager)
	{
		return NULL;
	}

	// 检验参数
	// 查找指定的业务视图属哪个childframe
	if ( NULL == pChildFrame )
	{
		pChildFrame = MDIGetActive();
		if ( NULL == pChildFrame )
		{
			return NULL;	// 无打开的页面
		}
	}

	CIoViewBase *pIoViewActive =  NULL;
	CGGTongView *pActiveView = DYNAMIC_DOWNCAST(CGGTongView, pChildFrame->GetActiveView());
	if ( NULL != pActiveView )
	{
		pIoViewActive = pActiveView->m_IoViewManager.GetActiveIoView();
	}


	// 查找顺序 
	//			-> 激活页面	0 -> 任何可视页面 1
	//			-> 激活页面manager下 2-> 任意同frame页面
	typedef	vector<CIoViewBase *>	IoViewArray;
	typedef vector<IoViewArray >	IoViewArrayArray;
	typedef map<CIoViewBase *, int>	IoViewMap;
	IoViewArrayArray	aIoViewsToSearch;
	IoViewMap			mapIoViewsSrc;
	{
		for ( int i=0; i < m_IoViewsPtr.GetSize() ; i++ )
		{
			CIoViewBase *pIoView = m_IoViewsPtr[i];
			if ( !IsWindow(pIoView->GetSafeHwnd()) )
			{
				ASSERT( 0 );
				continue;
			}

			if ( pIoView->GetParentFrame() != pChildFrame )
			{
				continue;
			}
			// 同窗口下的
			mapIoViewsSrc[ pIoView ] = 1;
		}
	}

	const T_IoViewObject *pObjToFind = CIoViewManager::FindIoViewObjectByPicMenuID(nPicID);
	if ( NULL == pObjToFind )
	{
		ASSERT( 0 );
		return NULL;	// 没有这个pic
	}

	// 慢慢找哈, 有重复的-- 
	IoViewArray	aIoViewsTmp;

	// 序列化
	if ( NULL != pIoViewActive )
	{
		// 激活的
		aIoViewsTmp.push_back(pIoViewActive);
		mapIoViewsSrc.erase(pIoViewActive);
	}

	// 同组的
	if ( iGroupId >= 0 )
	{
		IoViewMap mapTmp = mapIoViewsSrc;
		for ( IoViewMap::iterator it = mapTmp.begin() ; it != mapTmp.end() ; it++ )
		{
			CIoViewBase *pIoView = it->first;
			if ( !IsWindow(pIoView->GetSafeHwnd()) )
			{
				ASSERT( 0 );
				continue;
			}

			int32 iViewGroupID = pIoView->GetIoViewGroupId();
			if(iViewGroupID & iGroupId)
				//if ( pIoView->GetIoViewGroupId() == iGroupId )
			{
				aIoViewsTmp.push_back(pIoView);
				mapIoViewsSrc.erase(pIoView);
			}
		}
	}

	{
		// 同frame
		IoViewArray aShow, aHide;
		IoViewMap mapTmp = mapIoViewsSrc;
		for ( IoViewMap::iterator it = mapTmp.begin() ; it != mapTmp.end() ; it++ )
		{
			CIoViewBase *pIoView = it->first;
			if ( !IsWindow(pIoView->GetSafeHwnd()) )
			{
				ASSERT( 0 );
				continue;
			}


			if ( pIoView->IsWindowVisible() )
			{
				aShow.push_back(pIoView);
			}
			else
			{
				aHide.push_back(pIoView);
			}
			mapIoViewsSrc.erase(pIoView);
		}
		aIoViewsTmp.insert(aIoViewsTmp.end(), aShow.begin(), aShow.end());
		aIoViewsTmp.insert(aIoViewsTmp.end(), aHide.begin(), aHide.end());
	}

	ASSERT( mapIoViewsSrc.empty() );	// 应当不剩下了

	// 挨个查找各个业务视图， 是否符合条件
	for ( IoViewArray::iterator it = aIoViewsTmp.begin(); it != aIoViewsTmp.end(); it++)
	{
		CIoViewBase *pIoView = *it;

		if ( bMustVisible )
		{
			if ( !pIoView->IsWindowVisible() )
			{
				continue;	// 不可见的忽略, 其实可以退出了，因为后面的都是不可见的
			}
		}

		// pic id
		const T_IoViewObject *pIoObj = CIoViewManager::FindIoViewObjectByIoViewPtr(pIoView);
		if ( pIoObj != NULL  && pIoObj->m_pIoViewClass != NULL )
		{
			if ( pIoObj->m_pIoViewClass == pObjToFind->m_pIoViewClass
				||(bEnableInherit  && pIoObj->m_pIoViewClass->IsDerivedFrom(pObjToFind->m_pIoViewClass)) )
			{
				return pIoView;	// 相等或者允许子类
			}
		}
	}

	return NULL;
}

CIoViewBase * CMainFrame::FindIoViewInFrame(UINT nPicID, int32 iGroupId)
{
	CMDIChildWnd *pChildFrame = NULL;
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDoc = (CGGTongDoc *)pApp->m_pDocument;
	ASSERT( NULL != pDoc );
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	ASSERT( NULL != pAbsCenterManager );
	if (!pAbsCenterManager)
	{
		return NULL;
	}

	// 查找指定的业务视图属哪个childframe
	if ( NULL == pChildFrame )
	{
		pChildFrame = MDIGetActive();
		if ( NULL == pChildFrame )
		{
			return NULL;	// 无打开的页面
		}
	}

	CIoViewBase *pIoViewActive =  NULL;
	CGGTongView *pActiveView = DYNAMIC_DOWNCAST(CGGTongView, pChildFrame->GetActiveView());
	if ( NULL != pActiveView )
	{
		pIoViewActive = pActiveView->m_IoViewManager.GetActiveIoView();

		CIoViewManager *pManager = pIoViewActive->GetIoViewManager();
		int32 iCnt = pManager->m_IoViewsPtr.GetSize();
		for ( int32 i = 0 ; i < iCnt ; i++)
		{
			const T_IoViewObject *pIoObj = CIoViewManager::FindIoViewObjectByIoViewPtr(pManager->m_IoViewsPtr[i]);

			if( pIoObj && (pIoObj->m_uID==nPicID))
			{
				return pManager->m_IoViewsPtr[i];
			}
		}
	}


	// 查找顺序 
	//			-> 激活页面	0 -> 任何可视页面 1
	//			-> 激活页面manager下
	typedef	vector<CIoViewBase *>	IoViewArray;
	typedef vector<IoViewArray >	IoViewArrayArray;
	typedef map<CIoViewBase *, int>	IoViewMap;
	IoViewArrayArray	aIoViewsToSearch;
	IoViewMap			mapIoViewsSrc;
	{
		for ( int i=0; i < m_IoViewsPtr.GetSize() ; i++ )
		{
			CIoViewBase *pIoView = m_IoViewsPtr[i];
			if ( !IsWindow(pIoView->GetSafeHwnd()) )
			{
				ASSERT( 0 );
				continue;
			}

			if ( pIoView->GetParentFrame() != pChildFrame )
			{
				continue;
			}
			// 同窗口下的
			mapIoViewsSrc[ pIoView ] = 1;
		}
	}

	const T_IoViewObject *pObjToFind = CIoViewManager::FindIoViewObjectByPicMenuID(nPicID);
	if ( NULL == pObjToFind )
	{
		return NULL;	// 没有这个pic
	}

	// 慢慢找哈, 有重复的-- 
	IoViewArray	aIoViewsTmp;

	// 序列化
	if ( NULL != pIoViewActive )
	{
		// 激活的
		aIoViewsTmp.push_back(pIoViewActive);
		mapIoViewsSrc.erase(pIoViewActive);
	}

	// 同组的
	if ( iGroupId >= 0 )
	{
		IoViewMap mapTmp = mapIoViewsSrc;
		IoViewArray aShow, aHide;
		for ( IoViewMap::iterator it = mapTmp.begin() ; it != mapTmp.end() ; it++ )
		{
			CIoViewBase *pIoView = it->first;
			if ( !IsWindow(pIoView->GetSafeHwnd()) )
			{
				ASSERT( 0 );
				continue;
			}

			int32 iViewGroupID = pIoView->GetIoViewGroupId();
			if(iViewGroupID & iGroupId)
				/*if ( pIoView->GetIoViewGroupId() & iGroupId )*/
			{
				if ( pIoView->IsWindowVisible() )
				{
					aShow.push_back(pIoView);
				}
				else
				{
					aHide.push_back(pIoView);
				}

				mapIoViewsSrc.erase(pIoView);
			}
		}
		aIoViewsTmp.insert(aIoViewsTmp.end(), aHide.begin(), aHide.end());
		aIoViewsTmp.insert(aIoViewsTmp.end(), aShow.begin(), aShow.end());
	}

	//ASSERT( mapIoViewsSrc.empty() );	// 应当不剩下了

	// 挨个查找各个业务视图， 是否符合条件
	for ( IoViewArray::iterator it = aIoViewsTmp.begin(); it != aIoViewsTmp.end(); it++)
	{
		CIoViewBase *pIoView = *it;

		// pic id
		const T_IoViewObject *pIoObj = CIoViewManager::FindIoViewObjectByIoViewPtr(pIoView);
		if ( pIoObj != NULL  && pIoObj->m_pIoViewClass != NULL )
		{
			if ( pIoObj->m_pIoViewClass == pObjToFind->m_pIoViewClass
				||(pIoObj->m_pIoViewClass->IsDerivedFrom(pObjToFind->m_pIoViewClass)) )
			{
				return pIoView;	// 相等或者允许子类
			}
		}
	}

	return NULL;
}

CIoViewReport*	CMainFrame::FindIoViewReport(bool32 bInTopChildFrame)
{
	CGGTongDoc * pDoc =  AfxGetDocument();
	ASSERT(NULL != pDoc);

	CAbsCenterManager * pAbsCenterManager = pDoc->m_pAbsCenterManager;
	ASSERT(NULL != pAbsCenterManager);
	if (!pAbsCenterManager)
	{
		return NULL;
	}

	CIoViewReport* pFirstFind = NULL;				// 找到的第一个符合条件的报价视图.
	//	CIoViewReport* pFindInTabSplitWnd = NULL;		// 在标签页中找到的报价视图.

	// 当前的活动 ChildFrame
	CMPIChildFrame * pChildFrame =(CMPIChildFrame *)GetActiveFrame();
	if ( NULL == pChildFrame)
	{
		// 如果当前没有子窗口
		return NULL;
	}

	int iViewSize = m_IoViewsPtr.GetSize()-1;

	for (int32 i=iViewSize; i>=0; i--)
	{
		CIoViewBase * pIoView = m_IoViewsPtr[i];

		bool32 b = pIoView->IsKindOf(RUNTIME_CLASS(CIoViewReport));

		if (!b)
		{
			continue;
		}

		// 锁定的报价表不处理
		if (((CIoViewReport*)pIoView)->m_bLockReport)
		{
			continue;
		}

		if (bInTopChildFrame)
		{
			CMPIChildFrame * pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, pIoView->GetParentFrame());
			if ( pFrame!= pChildFrame)
			{
				continue;
			}
		}

		// 如果ChildFrame 有 TabSplitWnd ,需要增加判断一下

		CWnd* pWnd = pChildFrame->GetWindow(GW_CHILD);

		if ( pWnd->IsKindOf(RUNTIME_CLASS(CTabSplitWnd)))
		{
			CTabSplitWnd* pTabSplitWnd = (CTabSplitWnd*)pWnd;

			int32 iCurPage = pTabSplitWnd->m_nCurPage + 1;

			if(iCurPage ==  pTabSplitWnd->GetIoViewPage(pIoView))
			{
				// 返回当前Tab 页的报价表.
				return (CIoViewReport*)pIoView;
			}
			else
			{
				// 继续找
				// 另外一个Tab也是否考虑分组？
				continue;
			}
		}

		// 尽量在同一个tab页面中创建，所以先进行tab的判断，如果不在tab中，不处理，是否加入分组相关？0001770
		// 分组调整貌似不能影响到 pAbsCenterManager 中的指针？
		if ( NULL == pFirstFind)
		{
			pFirstFind = (CIoViewReport*)pIoView;
		}
	}	
	//
	return pFirstFind;
}

CIoViewNews   * CMainFrame::FindIoViewNews( bool32 bInTopChildFram )
{
	CGGTongDoc * pDoc =  AfxGetDocument();
	ASSERT(NULL != pDoc);

	CAbsCenterManager * pAbsCenterManager = pDoc->m_pAbsCenterManager;
	ASSERT(NULL != pAbsCenterManager);
	if (!pAbsCenterManager)
	{
		return NULL;
	}

	CIoViewNews* pFirstFind = NULL;				// 找到的第一个符合条件的报价视图.
	//	CIoViewNews* pFindInTabSplitWnd = NULL;		// 在标签页中找到的报价视图.

	// 当前的活动 ChildFrame
	CMPIChildFrame * pChildFrame =(CMPIChildFrame *)GetActiveFrame();
	if ( NULL == pChildFrame)
	{
		// 如果当前没有子窗口
		return NULL;
	}

	for (int32 i=0; i<m_IoViewsPtr.GetSize(); i++)
	{
		CIoViewBase * pIoView = m_IoViewsPtr[i];

		bool32 b = pIoView->IsKindOf(RUNTIME_CLASS(CIoViewNews));

		if (!b)
		{
			continue;
		}

		if (bInTopChildFram)
		{
			CMPIChildFrame * pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, pIoView->GetParentFrame());
			if ( pFrame!= pChildFrame)
			{
				continue;
			}
		}

		// 如果ChildFrame 有 TabSplitWnd ,需要增加判断一下

		CWnd* pWnd = pChildFrame->GetWindow(GW_CHILD);

		if ( pWnd->IsKindOf(RUNTIME_CLASS(CTabSplitWnd)))
		{
			CTabSplitWnd* pTabSplitWnd = (CTabSplitWnd*)pWnd;

			int32 iCurPage = pTabSplitWnd->m_nCurPage + 1;

			if(iCurPage ==  pTabSplitWnd->GetIoViewPage(pIoView))
			{
				// 返回当前Tab 页的报价表.
				return (CIoViewNews*)pIoView;
			}
			else
			{
				// 继续找
				// 另外一个Tab也是否考虑分组？
				continue;
			}
		}

		// 尽量在同一个tab页面中创建，所以先进行tab的判断，如果不在tab中，不处理，是否加入分组相关？0001770
		// 分组调整貌似不能影响到 pAbsCenterManager 中的指针？
		if ( NULL == pFirstFind)
		{
			pFirstFind = (CIoViewNews*)pIoView;
		}
	}	

	return pFirstFind;
}

CIoViewDuoGuTongLie* CMainFrame::FindIoViewDuoGuTongLie( bool32 bInTopChildFram )
{
	CGGTongDoc * pDoc =  AfxGetDocument();
	ASSERT(NULL != pDoc);

	CAbsCenterManager * pAbsCenterManager = pDoc->m_pAbsCenterManager;
	ASSERT(NULL != pAbsCenterManager);
	if (!pAbsCenterManager)
	{
		return NULL;
	}

	CIoViewDuoGuTongLie* pFirstFind = NULL;				// 找到的第一个符合条件的视图.
	CIoViewDuoGuTongLie* pFindInTabSplitWnd = NULL;		// 在标签页中找到的视图.

	// 当前的活动 ChildFrame
	CMPIChildFrame * pChildFrame =(CMPIChildFrame *)GetActiveFrame();
	if ( NULL == pChildFrame)
	{
		// 如果当前没有子窗口
		return NULL;
	}

	for (int32 i=0; i<m_IoViewsPtr.GetSize(); i++)
	{
		CIoViewBase * pIoView = m_IoViewsPtr[i];

		bool32 b = pIoView->IsKindOf(RUNTIME_CLASS(CIoViewDuoGuTongLie));

		if (!b)
		{
			continue;
		}

		if (bInTopChildFram)
		{
			CMPIChildFrame * pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, pIoView->GetParentFrame());
			if ( pFrame!= pChildFrame)
			{
				continue;
			}
		}

		// 如果ChildFrame 有 TabSplitWnd ,需要增加判断一下

		CWnd* pWnd = pChildFrame->GetWindow(GW_CHILD);

		if ( pWnd->IsKindOf(RUNTIME_CLASS(CTabSplitWnd)))
		{
			CTabSplitWnd* pTabSplitWnd = (CTabSplitWnd*)pWnd;

			int32 iCurPage = pTabSplitWnd->m_nCurPage + 1;

			if(iCurPage ==  pTabSplitWnd->GetIoViewPage(pIoView))
			{
				// 返回当前Tab 页的报价表.
				return (CIoViewDuoGuTongLie*)pIoView;
			}
			else
			{
				// 继续找
				// 另外一个Tab也是否考虑分组？
				continue;
			}
		}

		// 尽量在同一个tab页面中创建，所以先进行tab的判断，如果不在tab中，不处理，是否加入分组相关？0001770
		// 分组调整貌似不能影响到 pAbsCenterManager 中的指针？
		if ( NULL == pFirstFind)
		{
			pFirstFind = (CIoViewDuoGuTongLie*)pIoView;
		}
	}	

	return pFirstFind;
}

void CMainFrame::OnAddUpView()
{
	OnIoViewMenu(IDM_IOVIEWBASE_SPLIT_TOP);
}

void CMainFrame::OnAddDownView()
{
	OnIoViewMenu(IDM_IOVIEWBASE_SPLIT_BOTTOM);
}

void CMainFrame::OnAddLeftView()
{
	OnIoViewMenu(IDM_IOVIEWBASE_SPLIT_LEFT);
}

void CMainFrame::OnAddRightView()
{
	OnIoViewMenu(IDM_IOVIEWBASE_SPLIT_RIGHT);
}

void CMainFrame::OnDelView()
{
	OnIoViewMenu(IDM_IOVIEWBASE_CLOSE);
}


void CMainFrame::OnUpdateCloseChildFrame( CCmdUI *pCmdUI )
{
	CMDIChildWnd *pChild = MDIGetActive();
	CMPIChildFrame *pDefault = CCfmManager::Instance().GetUserDefaultCfmFrame();
	if ( NULL == MDIGetActive() || pChild == pDefault )		// 默认页面不提供关闭按钮
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(TRUE);
	}
}

void CMainFrame::OnCloseChildFrame()
{
	CMPIChildFrame* pActiveChild = (CMPIChildFrame*)MDIGetActive();

	if ( NULL != pActiveChild && this != (CWnd*)pActiveChild && pActiveChild->IsKindOf(RUNTIME_CLASS(CMPIChildFrame)) )
	{
		CMPIChildFrame *pDefault = CCfmManager::Instance().GetUserDefaultCfmFrame();
		if ( pDefault == pActiveChild )
		{
			ASSERT( 0 );
			return;	// 默认页面不能通过此方法关闭
		}
		if ( !pActiveChild->GetIdString().IsEmpty() )
		{
			CCfmManager::Instance().SaveCfm(pActiveChild->GetIdString(), pActiveChild);	// 自动保存页面
		}
		pActiveChild->PostMessage(WM_CLOSE, 0, 0);
		OnBackToUpCfm();
	}
	else if ( this == (CWnd*)pActiveChild )
	{
		MessageBox(L"当前无可关闭窗口", AfxGetApp()->m_pszAppName, MB_ICONWARNING);		
	}
}

void CMainFrame::GetBlockMenuInfo(CArray<T_BlockMenuInfo, T_BlockMenuInfo&>& aBlockMenuInfo)
{
	aBlockMenuInfo.Copy(m_aBlockMenuInfo);
}

void CMainFrame::OnTimer(UINT nIDEvent)
{
	if (KTimerStartUpLogin == nIDEvent)
	{
		KillTimer(nIDEvent);	
		Login(true, false);
	}
	if ( nIDEvent == KTimerIdCheckConnectServer )
	{
		m_uiConnectServerTimers += 1;

		if( m_uiConnectServerTimers >= KuiTimesToWaitForConnect )
		{
			KillTimer(KTimerIdCheckConnectServer);
			m_uiConnectServerTimers = 0;

			// 超时了,提示一下
			MessageBox(L"服务器不能正常提供服务, 请检查网络, 稍后重连...", AfxGetApp()->m_pszAppName);
		}
		else
		{
			// 判断是否认证成功了:
			CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
			if ( NULL == pDoc || !pDoc->m_pAbsDataManager)
			{
				return;
			}

			CArray<CAbsCommunicationShow *,CAbsCommunicationShow *> aServers;

			aServers.RemoveAll();
			pDoc->m_pAbsDataManager->GetCommnunicaionList(aServers);

			for (int32 i = 0 ;i < aServers.GetSize() ; i++)
			{			
				CString StrTemp,StrAddr;
				uint32 uiPort;
				CProxyInfo ProxyInfo;
				CServerState ServerState;

				// 得到服务器信息
				aServers[i]->GetServerState(ServerState);	
				aServers[i]->GetServerSummary(StrAddr, uiPort, ProxyInfo);

				if ( (ServerState.m_bConnected) && (EASAuthSuccessed == ServerState.m_eAuthState) )
				{
					KillTimer(KTimerIdCheckConnectServer);
					m_wndStatusBar.HideTips();
					m_uiConnectServerTimers = 0;

					// 连上了服务器:
					MessageBox(L"连接成功!", AfxGetApp()->m_pszAppName);
					return;						
				}
			}
		}
	}
	else if (KiTimerIdSaveData == nIDEvent )
	{
		if ( NULL != m_pRecordDataCenter )
		{
			m_pRecordDataCenter->OnRecordDataCenterTimer(nIDEvent);
		}
	}
	else if ( KiTimerIdPing == nIDEvent )
	{
		// Ping 的定时器
		COptimizeServer::Instance()->OnMyTimer(nIDEvent);
	}
	else if ( KTimerIdCheckNewsConnectServer == nIDEvent )
	{
		// 检查资讯服务器是否连接上的
		m_uiConnectNewsServerTimers++;

		// 资讯服务器是否连接上了
		CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
		bool32  bConnected = false;
		if ( NULL != pDoc  && pDoc->m_pNewsManager)
		{
			// 咨询服务器:
			CArray<CAbsNewsCommnunicationShow*, CAbsNewsCommnunicationShow*> aNewsServers;
			pDoc->m_pNewsManager->GetCommnunicaionList(aNewsServers);

			//			bool32 bAllNewsNonConnect = true;

			for ( int32 i = 0; i < aNewsServers.GetSize(); i++ )
			{
				CString StrTemp;
				CAbsNewsCommnunicationShow* pCommunication = aNewsServers[i];
				if( NULL == pCommunication )
				{
					continue;
				}

				CString StrAddress;
				uint32 uiPort;
				uint32 uiKeepSeconds;

				pCommunication->GetNewsServerSummary(bConnected, StrAddress, uiPort, uiKeepSeconds);

				CNewsServerState NewsServerState;
				pCommunication->GetNewsServerState(NewsServerState);

				if (bConnected) 
				{
					// 只要连接上了一个都算
					break;
				}
			}
		}

		if ( bConnected )
		{
			m_uiConnectNewsServerTimers = 0;
			KillTimer(nIDEvent);

			AfxMessageBox(_T("连接资讯服务器成功!"));
		}
		else if( m_uiConnectNewsServerTimers++ >= KuiTimesToWaitForConnect  )
		{
			m_uiConnectNewsServerTimers = 0;
			KillTimer(nIDEvent);

			AfxMessageBox(_T("连接资讯服务器超时！"));
		}
	}
	else if ( KTimerIdAutoUpdateCheck == nIDEvent )
	{
		KillTimer(nIDEvent);

		// 是否指定了update
		StringArray aCmd;
		const TCHAR *ptszCmdLine = ::GetCommandLine();
		GetMyCommandLine(ptszCmdLine, aCmd);
		MyStringType strCmdUpdateHeader(_T("Update="));
		for (unsigned int iCmd=0; iCmd<aCmd.size(); ++iCmd )
		{
			const MyStringType &str = aCmd[iCmd];
			if ( !str.empty() && _tcsnicmp(str.c_str(), strCmdUpdateHeader.c_str(), strCmdUpdateHeader.length()) == 0 )
			{
				// 启动自动更新程序:
				TCHAR TStrDir[MAX_PATH];	
				::GetCurrentDirectory(MAX_PATH, TStrDir);
				int32 iTStrDirLen = _tcslen(TStrDir);
				if ( iTStrDirLen>0 && TStrDir[iTStrDirLen-1]!=_T('\\') )
				{
					++iTStrDirLen;
					TStrDir[iTStrDirLen-1] = _T('\\');
					TStrDir[iTStrDirLen] = '\0';
				}

				STARTUPINFO StartInfo = {sizeof(StartInfo)};
				PROCESS_INFORMATION ProcessInfo;

				TCHAR szExe[MAX_PATH*2];
				_stprintf(szExe, _T("%sAutoUpdate.exe"), TStrDir);
				TCHAR szParam[MAX_PATH*3];
				_stprintf(szParam, _T("\"%s\" BackMode"), szExe, szParam);

				if( !::CreateProcess(L"AutoUpdate.exe", szParam, NULL, NULL, false, 0, NULL, TStrDir, &StartInfo, &ProcessInfo) )
				{
					// 不需要提示
				}
				else
				{
					DEL_HANDLE(ProcessInfo.hThread);
					DEL_HANDLE(ProcessInfo.hProcess);
				}

				break;
			}
		}
	}
	else if (nIDEvent == KTimerIdReqPushJinping)
	{
		CGGTongDoc * pDoc =  AfxGetDocument();
		ASSERT(NULL != pDoc);

		CAbsCenterManager * pAbsCenterManager = pDoc->m_pAbsCenterManager;
		if (!pAbsCenterManager)
		{
			return;
		}

		CMmmiNewsReqUpdatePush NewsReqUpdatePushJinPing;
		NewsReqUpdatePushJinPing.m_iIndexID = pAbsCenterManager->GetJinpingID();

		//
		AfxGetDocument()->m_pAbsCenterManager->RequestNews(&NewsReqUpdatePushJinPing);
	}
	else if (KTimerIdDapanState == nIDEvent)		//--- wangyongxue 2016/12/06  大盘状态目前先放在这里处理吧
	{
		CGGTongDoc * pDoc =  AfxGetDocument();
		ASSERT(NULL != pDoc);

		CAbsCenterManager * pAbsCenterManager = pDoc->m_pAbsCenterManager;
		if (!pAbsCenterManager)
		{
			return;
		}
		CMmiReqDapanState reqDapanState;
		pAbsCenterManager->RequestViewData(&reqDapanState);

		KillTimer(KTimerIdDapanState);
	}
	else if (KTimerIdChooseStockState == nIDEvent)		//--- wangyongxue 2017/03/01  选股状态
	{
		CGGTongDoc * pDoc =  AfxGetDocument();
		ASSERT(NULL != pDoc);

		CAbsCenterManager * pAbsCenterManager = pDoc->m_pAbsCenterManager;
		CMmiReqChooseStockStatus reqChooseStockState;
		pAbsCenterManager->RequestViewData(&reqChooseStockState);

		KillTimer(KTimerIdChooseStockState);
	}
	else if(nIDEvent == KTimerIdIndexShow)
	{
		CIoViewKLine *pIoViewKLine = (CIoViewKLine *)GetIoViewByPicMenuIdAndActiveIoView(ID_PIC_KLINE, false, true, false, true,	false, true);
		if ( NULL == pIoViewKLine )
		{
			LoadSystemDefaultCfm(ID_PIC_KLINE, NULL, true);
			pIoViewKLine = (CIoViewKLine *)FindIoViewInFrame(ID_PIC_KLINE, NULL, false, false);
		}
		if ( NULL != pIoViewKLine )
		{
			if ( NULL == pIoViewKLine->GetMerchXml() )	// 填充一个默认商品给他
			{
				CMerch *pMerch = NULL;
				CAbsCenterManager *pAbsCenterManager = pIoViewKLine->GetCenterManager();
				if ( NULL != pAbsCenterManager
					&& pAbsCenterManager->GetMerchManager().m_BreedListPtr.GetSize() > 0
					&& pAbsCenterManager->GetMerchManager().m_BreedListPtr[0]->m_MarketListPtr.GetSize() > 0
					&& pAbsCenterManager->GetMerchManager().m_BreedListPtr[0]->m_MarketListPtr[0]->m_MerchsPtr.GetSize() > 0)
				{
					pMerch = pAbsCenterManager->GetMerchManager().m_BreedListPtr[0]->m_MarketListPtr[0]->m_MerchsPtr[0];
					OnViewMerchChanged(pIoViewKLine, pMerch);
				}

				// 没有商品也不能赋予指标- -
			}
			if ( NULL != pIoViewKLine->GetMerchXml() )
			{
				if(0 < pIoViewKLine->m_pRegionMain->m_RectView.Height())
				{
					KillTimer(KTimerIdIndexShow);
					pIoViewKLine->AddShowIndex(m_hotKey.m_StrParam1, false, true);	// 变更适当的商品
				}
				else
				{
					return;
				}
			}
			pIoViewKLine->BringToTop();
		}
	}
	else if (KTimerInfoCenter == nIDEvent)
	{
		CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
		if (!pDoc->m_pAutherManager)
		{
			return;
		}

		m_nReqTime += 1;

		if (
			1 == m_nReqTime ||
			3 == m_nReqTime ||
			5 == m_nReqTime || 
			10 == m_nReqTime || 
			15 == m_nReqTime || 
			30 == m_nReqTime)
		{
			TCHAR szTime[32]={0};
			wsprintf(szTime, L"%d", m_nReqTime);
			pDoc->m_pAutherManager->GetInterface()->ReqMsgTip(szTime);
		}

		if (30 == m_nReqTime)
		{
			KillTimer(KTimerInfoCenter);
		}
	}
	else if (KTimerIdRefreshToken == nIDEvent)
	{
		// 刷新token
		CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
		if (!pDoc->m_pAutherManager)
		{
			return;
		}
		bool32 bRet = pDoc->m_pAutherManager->GetInterface()->RefreshToken();

		// 失败，重试一次
		if (!bRet)
		{
			pDoc->m_pAutherManager->GetInterface()->RefreshToken();
		}
	}
	else if (KTimerIdUserDealRecord == nIDEvent)
	{
		// 上传用户行为记录
		if (m_vUserDealRecord.size() > 0)
		{
			CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
			if (!pDoc->m_pAutherManager)
			{
				return;
			}
			pDoc->m_pAutherManager->GetInterface()->UserDealRecord(m_vUserDealRecord);
			m_vUserDealRecord.clear();
		}
	}
	else if (KTimerCheckToken == nIDEvent)
	{	
		CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
		if (pDoc)
		{
			if (!pDoc->m_pAutherManager)
			{
				return;
			}
			pDoc->m_pAutherManager->GetInterface()->ReqCheckToken();
		}
	}
	/////////////////////////////////////////////////////
	//===================CODE CLEAN======================
	//////////////////////////////////////////////////////
	/*else if (KTimerPickModelStatus == nIDEvent)
	{	
		QueryPickModelTypeStatus();
	}*/
	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 菜单项操作
void CMainFrame::OnNewWorkspace() 
{
	NewWorkSpace();	
}

void CMainFrame::NewWorkSpace()
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	bool32 bNewUser = false;
	bool32 bNeedInitTBWnd = false;

	if (!pDoc->m_pAbsCenterManager)
	{
		return;
	}

	if ( NULL != pApp && NULL != pDoc)
	{				
		pApp->ScanAvaliableWspFile(pDoc->m_pAbsCenterManager->GetUserName(), bNewUser);

		if( bNewUser )
		{
			CGGTongApp::CopyPublicWCVFilesToPrivate(pDoc->m_pAbsCenterManager->GetUserName());
			bool32 bUserPseudo;									// 重新扫描一次，以更改wsp中文件路径表示方式
			pApp->ScanAvaliableWspFile(pDoc->m_pAbsCenterManager->GetUserName(), bUserPseudo);

			bNeedInitTBWnd = true;
		}

	}

	// 新建工作区
	CDlgWspName dlgWspName;
	AfxInitRichEdit();

	dlgWspName.SetNewFileType(CDlgWorkSpaceSet::EFTWorkSpace);

	if ( IDOK == dlgWspName.DoModal() && NULL != pApp && NULL != pDoc)  // 有可能在这个对话框里面死命删除- -
	{
		// 关闭当前的
		ClearCurrentWorkSpace();

		//
		const T_WspFileInfo* pWspFile = pApp->GetWspFileInfo(pApp->m_StrCurWsp);
		CString StrFileVersion = L"";

		if ( NULL != pWspFile )
		{
			StrFileVersion = pWspFile->GetNormalWspFileVersion();
		}

		// 保存一下当前的
		pApp->m_StrCurWsp = GetWorkSpaceDir() + dlgWspName.m_StrName + L".wsp";	
		if ( !dlgWspName.m_StrSelectedWspPath.IsEmpty() )
		{
			pApp->m_StrCurWsp = dlgWspName.m_StrSelectedWspPath;	// 使用替换的原文件名，而不是新建一个文件，避免重新得到系统默认工作区
		}

		CString StrAbsPath = CPathFactory::ComPathToAbsPath(pApp->m_StrCurWsp);
		if ( StrAbsPath.GetLength() > 0 )
		{	
			pApp->m_StrCurWsp = StrAbsPath;
		}

		SaveWorkSpace(dlgWspName.m_StrName, StrFileVersion);		

		//
		pApp->ScanAvaliableWspFile(pDoc->m_pAbsCenterManager->GetUserName(), bNewUser);

		// 默认新建一个子窗口
		PostMessage(WM_COMMAND, ID_WINDOW_NEW, 0);
		bNeedInitTBWnd = true;
	}
	else if ( (dlgWspName.m_dwDoneAction & CDlgWspName::DoneDelete) != 0 )	// 有删除操作
	{
		if (NULL != pApp && NULL != pDoc)
		{
			pApp->ScanAvaliableWspFile(pDoc->m_pAbsCenterManager->GetUserName(), bNewUser);
		}

		bNeedInitTBWnd = true;
	}
}

void CMainFrame::OnOpenWorkspace() 
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if (!pDoc->m_pAbsCenterManager)
	{
		return;
	}

	CArray<T_WspFileInfo, T_WspFileInfo &> aWspInfos;

	if ( NULL != pApp && NULL != pDoc)
	{
		bool32 bNewUser = false;
		pApp->ScanAvaliableWspFile(pDoc->m_pAbsCenterManager->GetUserName(), bNewUser);

		if ( bNewUser )
		{
			CGGTongApp::CopyPublicWCVFilesToPrivate(pDoc->m_pAbsCenterManager->GetUserName());
			bool32 bUserPseudo;									// 重新扫描一次，以更改wsp中文件路径表示方式
			pApp->ScanAvaliableWspFile(pDoc->m_pAbsCenterManager->GetUserName(), bUserPseudo);
		}

		aWspInfos.Copy(pApp->m_aWspFileInfo);
	}

	// 打开工作区	
	CDlgWorkSpaceSet dlg;	
	CStringArray aStrFileTitle,aStrFileDir;	
	//
	// 	pApp->GetWspFileXmlNameArray(aStrFileTitle);
	// 	pApp->GetWspFilePathArray(aStrFileDir);

	{
		for (int32 i=0; i < aWspInfos.GetSize(); i++)
		{
			if ( aWspInfos[i].IsSpecialWsp() )
			{
				continue;		// 特殊工作区不能出现在这里
			}

			aStrFileTitle.Add(aWspInfos[i].m_StrFileXmlName);
			aStrFileDir.Add(aWspInfos[i].m_StrFilePath);
		}
	}

	//
	dlg.SetFileInfoArray(aStrFileDir,aStrFileTitle);
	dlg.SetNewFileType(CDlgWorkSpaceSet::EFTWorkSpace);

	dlg.DoModal();	
}

void CMainFrame::SaveWorkSpace(CString StrXmlName /*= L""*/, CString StrVersion/* = L""*/)
{
	// 保存颜色设置文件
	CFaceScheme::Instance()->IoViewFaceObjectToXml();

	// 保存表头设置:
	CReportScheme::Instance()->SaveDataToXml();

	// 上传服务器 我的自选
	// 	bool32 bUpload = CUploadUserData::instance()->UploadUserBlock();
	// 	ASSERT( bUpload );

	// 保存自己的工作区

	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();

	CString StrFileName = pApp->m_StrCurWsp;

	string sPath = _Unicode2MultiChar(StrFileName);
	const char* KStrWorkSpaceDir = sPath.c_str();

	bool32 bReadOnly = IsWspReadOnly(KStrWorkSpaceDir);
	if ( bReadOnly )
	{
		TRACE(_T("Save readonly WSP cancelled: %s\r\n"), StrFileName.GetBuffer());
		StrFileName.ReleaseBuffer();
		return;		// 只读工作区 - 不保存
	}

	CFileFind file;
	if (file.FindFile(StrFileName))
	{
		CFile::Remove(StrFileName);
	}	

	if ( StrXmlName.GetLength() <= 0 )
	{
		const T_WspFileInfo* pWspFileInfo = pApp->GetWspFileInfo(StrFileName);

		if ( NULL != pWspFileInfo )
		{
			StrXmlName = pWspFileInfo->m_StrFileXmlName;
		}
	}

	ToXml(KStrWorkSpaceDir, StrXmlName, StrVersion);
	ReNameFile(StrFileName, CMainFrame::ERTWSP);	
}

void CMainFrame::OnSaveWorkspace() 
{
	SaveWorkSpace();
}

void CMainFrame::OnSaveasWorkspace() 
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if (!pDoc->m_pAbsCenterManager)
	{
		return;
	}


	if ( NULL != pApp && NULL != pDoc)
	{
		bool32 bNewUser = false;
		pApp->ScanAvaliableWspFile(pDoc->m_pAbsCenterManager->GetUserName(), bNewUser);

		if ( bNewUser )
		{
			CGGTongApp::CopyPublicWCVFilesToPrivate(pDoc->m_pAbsCenterManager->GetUserName());

			bool32 bUserPseudo;									// 重新扫描一次，以更改wsp中文件路径表示方式
			pApp->ScanAvaliableWspFile(pDoc->m_pAbsCenterManager->GetUserName(), bUserPseudo);
		}
	}

	// 另存为工作区
	CDlgWspName dlg;
	AfxInitRichEdit();
	dlg.SetNewFileType(CDlgWorkSpaceSet::EFTWorkSpace);

	if ( IDOK == dlg.DoModal() )
	{
		// 保存自己的工作区
		// 另存行为:
		//     使用当前工作区内容，新建或者替换指定工作区
		//	   当前工作区重命名

		CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();

		//
		CString StrFileVersion = L"";
		const T_WspFileInfo* pWspFileInfo = pApp->GetWspFileInfo(pApp->m_StrCurWsp);
		if ( NULL != pWspFileInfo )
		{
			StrFileVersion = pWspFileInfo->GetNormalWspFileVersion();
		}

		//
		CString StrFileName = GetWorkSpaceDir() + dlg.m_StrName + L".wsp";	
		if ( !dlg.m_StrSelectedWspPath.IsEmpty() )
		{
			StrFileName = dlg.m_StrSelectedWspPath;	// 使用原来的文件名
		}

		string sPath = _Unicode2MultiChar(StrFileName);
		const char* KStrWorkSpaceDir = sPath.c_str();

		//
		CFileFind file;
		if (file.FindFile(StrFileName))
		{
			CFile::Remove(StrFileName);
		}	

		// 
		ToXml(KStrWorkSpaceDir, dlg.m_StrName, StrFileVersion);
		pApp->m_StrCurWsp = ReNameFile(StrFileName, CMainFrame::ERTWSP);	// 这个是相对路径 - 由于打开wsp使用的是全路径，这里需要做转换
		file.Close();
		if ( file.FindFile(pApp->m_StrCurWsp) )
		{
			file.FindNextFile();
			pApp->m_StrCurWsp = file.GetFilePath();
		}

		if ( NULL != pApp && NULL != pDoc)
		{
			bool32 bNewUser = false;
			pApp->ScanAvaliableWspFile(pDoc->m_pAbsCenterManager->GetUserName(), bNewUser);	
		}

	}
	else if ( (dlg.m_dwDoneAction & CDlgWspName::DoneDelete) != 0 && NULL != pApp		// 有删除操作
		&& NULL != pDoc)	
	{
		bool32 bNewUser = false;
		pApp->ScanAvaliableWspFile(pDoc->m_pAbsCenterManager->GetUserName(), bNewUser);
	}
}

void CMainFrame::ClearCurrentWorkSpace()
{
	// 关闭工作区(关闭所有子窗口)
	OnCloseAllWindow();
}

void	CMainFrame::ClearLogin()
{
	CGGTongApp *pApp = DYNAMIC_DOWNCAST(CGGTongApp, AfxGetApp());
	CGGTongDoc *pDoc = pApp->m_pDocument;
	if (!pApp || !pDoc || !pDoc->m_pAbsCenterManager || !pDoc->m_pAutherManager)
	{
		return ;
	}
	//	如果已经登录过，下载脱机数据
	if(m_bDoLogin)
	{
		// 收盘后提示下载脱机数据		
		if ( NULL != pDoc->m_pAbsCenterManager && !CGGTongApp::m_bOffLine  && !pDoc->m_isDefaultUser && !pDoc->m_pAbsCenterManager->GetUserName().IsEmpty())
		{
			bool32 bPassedCloseTime = true;

			CMerch* pMerch = NULL;
			pDoc->m_pAbsCenterManager->GetMerchManager().FindMerch(L"000001", 0, pMerch);
			if ( NULL != pMerch )
			{
				bPassedCloseTime = CIoViewBase::BePassedCloseTime(pMerch);
			}

			bool b1 = bPassedCloseTime;
			//bool b2 = pDoc->m_pAbsCenterManager->BeLocalOfflineDataEnough(EKTBDay);

			if ( bPassedCloseTime && !pDoc->m_pAbsCenterManager->BeLocalOfflineDataEnough(EKTBDay) )
			{
				if ( IDYES == MessageBox(L"是否补全本地脱机数据?", AfxGetApp()->m_pszAppName, MB_YESNO) )
				{
					CDlgDownLoad dlg(true);
					dlg.DoModal();
				}
			}
		}
	}

	//	如果已经登录过，则需要保存颜色配置文件，表头文件
	if(m_bDoLogin)
	{
		// 保存颜色设置文件
		CFaceScheme::Instance()->IoViewFaceObjectToXml();
		// 保存表头设置:
		CReportScheme::Instance()->SaveDataToXml();
	}

	// 保存一下所有的工作页面
	CWnd *pWnd = CWnd ::FromHandle(m_hWndMDIClient)->GetWindow(GW_CHILD); 
	while ( NULL != pWnd )   
	{ 
		CWnd* pTmp		= pWnd;
		CWnd* pTmpNext	= pTmp->GetWindow(GW_HWNDNEXT);

		CMPIChildFrame *pMPIFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, pWnd);
		if ( !pMPIFrame->GetIdString().IsEmpty() )
		{
			CCfmManager::Instance().SaveCfm(pMPIFrame->GetIdString(), pMPIFrame);	// 自动保存页面
		}

		pWnd = pTmpNext;
	}	


	if ( NULL != pDoc->m_pDlgbuding && NULL != pDoc->m_pDlgbuding->m_pDlgTrace && NULL != pDoc->m_pDlgbuding->m_pDlgTrace->m_pLogFile )
	{
		fclose(pDoc->m_pDlgbuding->m_pDlgTrace->m_pLogFile);		
	}

	//	如果已经登录过，则需要记录用户的操作，同步用户的自选股
	if (m_bDoLogin)
	{

		//SaveWorkSpace();		// 不用保存工作区


		// 退出的时候非游客上传一下用户行为记录
		if (m_vUserDealRecord.size() > 0 && pDoc->m_pAutherManager)
		{
			pDoc->m_pAutherManager->GetInterface()->UserDealRecord(m_vUserDealRecord);
			m_vUserDealRecord.clear();
		}

		// 非游客退出的时候同步下自选股
		if (pDoc->m_pAbsCenterManager && !pApp->m_bCustom)
		{
			pDoc->m_pAbsCenterManager->UploadUserBlock();
		}		

	}

	Uninit();	

}

bool32 CMainFrame::CanClose()
{
	int32 iChoose = IDNO;
	CGGTongApp *pApp = DYNAMIC_DOWNCAST(CGGTongApp, AfxGetApp());
	CGGTongDoc *pDoc = pApp->m_pDocument;
	if (!pApp || !pDoc)
	{
		return false;
	}

	do 
	{
		if (m_bShowLoginDlg)
		{	
			//	显示登录框
			break;
		}

		if (!m_bLoginSucc)
		{
			//	取消登录
			iChoose = IDYES;
			break;
		}

		if (pApp->m_bTradeExit || pApp->m_bUserLoginExit ||pApp->m_HQRegisterExit || !m_bDoLogin)
		{
			iChoose = IDYES;
		}
		else
		{
			//	宣传退出对话框
			CDlgSloganExit dlg;
			int ret = dlg.DoModal();
			if (IDCANCEL == ret )	//	用户选择关闭按钮，取消按钮
			{
				iChoose = IDNO;
			}

			if (IDOK == ret )		//	用户选择退出按钮，重新登录按钮
			{
				if (dlg.m_bReLogin)	//	用户选择重新登录按钮
				{	
					iChoose = IDNO;		
					m_bShowLoginDlg = true;
				}
				else
				{
					iChoose = IDYES;				
				}
			}	
		}
	} while (0);

	if (m_bShowLoginDlg)
	{
		ClearLogin();
		Login(true, false);
		iChoose = IDNO;
		m_bShowLoginDlg = false;		
	}
	return IDYES == iChoose ? true : false;
}


void CMainFrame::CloseWorkSpace()
{		
	if (CanClose())
	{				
		ClearLogin();
		CNewMDIFrameWnd::OnClose();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 页面
void CMainFrame::OnNewChildframe() 
{
	// 新建一个窗口
	OnWindowNew();  
}

void CMainFrame::OnNewCfm()
{
	CDlgCfmName dlg;
	if ( dlg.DoModal() == IDOK )
	{
		// 创建新窗口
		OnWindowNew();
		CCfmManager::Instance().CurCfmSaveAs(dlg.m_StrName);	// 另存为该名字
	}
}

void CMainFrame::OnOpenChildframe() 
{	
	CDlgOpenWorksheet dlg(this);
	if ( dlg.DoModal() == IDOK )
	{
		ASSERT( !dlg.m_StrSelCfm.IsEmpty() );
		CMPIChildFrame *pChildFrame = CCfmManager::Instance().GetCfmFrame(dlg.m_StrSelCfm);
		if ( NULL != pChildFrame )
		{
			CString StrPrompt;
			StrPrompt.Format(_T("您已经打开了页面: %s\r\n是否重新加载？"), dlg.m_StrSelCfm.GetBuffer());
			dlg.m_StrSelCfm.ReleaseBuffer();
			if ( AfxMessageBox(StrPrompt, MB_YESNO | MB_ICONQUESTION |MB_DEFBUTTON2 ) == IDNO )
			{
				MDIActivate(pChildFrame);
				return;
			}
		}
		CCfmManager::Instance().LoadCfm(dlg.m_StrSelCfm, true);	// 重新打开
	}
}

void CMainFrame::OnSaveasChildframe() 
{
	// 另存为一个窗口	
	if ( NULL != MDIGetActive() )
	{
		CDlgCfmName dlg;
		if ( dlg.DoModal() == IDOK )
		{
			CCfmManager::Instance().CurCfmSaveAs(dlg.m_StrName);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
///
CString CMainFrame::GetWorkSpaceDir()
{
	// 工作目录	

	CString StrPath;
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if (!pDoc->m_pAbsCenterManager)
	{
		return StrPath;
	}
	StrPath = CPathFactory::GetPrivateWorkspacePath(pDoc->m_pAbsCenterManager->GetUserName());

	return	StrPath;
} 

CString CMainFrame::GetLastWspFile(CString StrUserName, bool32 bNewUser/* = false*/)
{
	// 如果是新用户，不允许特殊工作区作为latest工作区
	// 得到最近一次工程文件
	CStringArray aStrFileDir;
	CArray<CTime, CTime> FileWriteTime;

	CString StrDir;

	FileWriteTime.RemoveAll();
	aStrFileDir.RemoveAll();

	if ( 0 == StrUserName.GetLength())
	{
		StrDir = GetWorkSpaceDir() ;	
	}
	else
	{
		StrDir = CPathFactory::GetPrivateWorkspacePath(StrUserName);																
	}

	if(StrDir.Right(1) != "/")
		StrDir += "/";
	StrDir += "*.wsp";

	CFileFind file;
	BOOL bContinue = file.FindFile(StrDir);

	//
	if (!bContinue)
	{
		return L"";
	}

	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CArray<T_WspFileInfo, T_WspFileInfo &>	aWspInfos;
	aWspInfos.Copy(pApp->m_aWspFileInfo);	// 这里有可能是.\格式

	while (bContinue)
	{
		CTime writetime;
		bContinue = file.FindNextFile();
		CString StrfileDir = file.GetFilePath();
		file.GetLastWriteTime(writetime);

		// 直接判断文件名
		bool32 bAdd = true;
		for ( int i=0; i < aWspInfos.GetSize()/* && bNewUser*/ ; i++ )	// 都不能作为latest
		{
			if ( aWspInfos[i].IsSpecialWsp() && aWspInfos[i].m_StrFileName == file.GetFileTitle() )
			{
				// 不能作为新用户的latest
				bAdd = false;
				break;
			}
		}
		if ( !bAdd )
		{
			continue;
		}

		aStrFileDir.Add(StrfileDir);
		FileWriteTime.Add(writetime);
	}

	if ( FileWriteTime.GetSize() < 1 )
	{
		return _T("");
	}

	CTime temptime = FileWriteTime[0];

	int32 i = 0;
	for (i=0;i<FileWriteTime.GetSize();i++)
	{
		temptime = temptime>=FileWriteTime[i]?temptime:FileWriteTime[i];
	}

	for ( i=0;i<FileWriteTime.GetSize();i++)
	{
		if (temptime == FileWriteTime[i])
		{
			StrDir = aStrFileDir[i];
		}
	}

	return StrDir;
} 

CString CMainFrame::ReNameFile(CString StrOldName,E_RenameType eReNameType)
{
	if (eReNameType < ERTXML || eReNameType >= ERTVMG)
	{
		return L"";
	}
	// 重命名文件
	int32   iLength = StrOldName.GetLength();
	CString StrNewName;

	if (ERTXML == eReNameType)   // ... 改成枚举
	{
		StrNewName = StrOldName.Left(iLength-4) + L".xml";
	}
	else if (ERTWSP == eReNameType)  
	{
		StrNewName = StrOldName.Left(iLength-4) + L".wsp";
	}
	else if ( ERTCFM == eReNameType)
	{
		StrNewName = StrOldName.Left(iLength-4) + L".cfm";		
	}
	else if ( ERTVMG == eReNameType)
	{
		StrNewName = StrOldName.Left(iLength-4) + L".vmg";		
	}
	TRY
	{
		CFile::Rename(StrOldName,StrNewName);
	}
	CATCH( CFileException, e )
	{
#ifdef _DEBUG
		afxDump << L"File " << StrOldName << L" not found, cause = "
			<< e->m_cause << L"\n";
#endif
	}
	END_CATCH

		return StrNewName;
}

CString CMainFrame::GetMoudleAppName()
{
	TCHAR szModule[_MAX_PATH];  
	GetModuleFileName(NULL, szModule, _MAX_PATH);
	CString StrExeName, StrTmpDir;
	StrTmpDir = szModule;

	int iPos = StrTmpDir.ReverseFind('\\');
	int32 iLen = StrTmpDir.GetLength();
	StrExeName = StrTmpDir.Right(iLen-iPos-1);
	StrExeName.Replace(L".exe",L"");

	return StrExeName;
}
CString CMainFrame::CreatDefaultWorkSpace()
{
	// 异常情况时创建默认工作区
	USES_CONVERSION;
	CString StrDefaultWorkSpaceFile;
	StrDefaultWorkSpaceFile  = GetWorkSpaceDir();
	StrDefaultWorkSpaceFile += AfxGetApp()->m_pszAppName + CString(".xml");

	char KStrFile[MAX_PATH];
	strcpy(KStrFile,_W2A(StrDefaultWorkSpaceFile));

	//USES_CONVERSION;
	CString StrDefault = GetDefaultXML();	
	char acTest[10240];	
	UNICODE_2_MULTICHAR(EMCCUtf8,StrDefault,acTest);

	CString StrDir	   = GetWorkSpaceDir();

	// Notic:  _tcheck_if_mkdir  的路径参数,需要以"/"结尾,否则最后一部分不会当成目录
	_tcheck_if_mkdir(StrDir.LockBuffer());
	StrDir.UnlockBuffer();


	//fangz0714no
	//const char * acTest = W2A(StrDefault);
	//const char * KStrFile = W2A(StrDefaultWorkSpaceFile);

	TiXmlDocument doc(KStrFile);
	doc.Parse(acTest);
	doc.Print();
	doc.SaveFile();

	return StrDefaultWorkSpaceFile;
}

void CMainFrame::NewEmptyReportChildFrame()
{
	// 新建一个子窗口,根据ID 添加报价视图的板块 

	//////////////////////////////////////////////////////////////////////////
	bool32 bMax = false;
	CWnd * pChild = CWnd ::FromHandle(m_hWndMDIClient)->GetWindow(GW_CHILD);
	if (pChild)
	{
		CMPIChildFrame * pChilds =(CMPIChildFrame * )GetActiveFrame();

		if (pChilds->GetMaxSizeFlag())
		{
			bMax = true;
		}		
	}

	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();

	POSITION   pos  = pApp->m_pDocTemplate->GetFirstDocPosition();
	if ( NULL == pos)
	{
		pApp->m_pDocument = (CGGTongDoc *)pApp->m_pDocTemplate->CreateNewDocument();
		pos  = pApp->m_pDocTemplate->GetFirstDocPosition();
	}

	CRect RectActive = CalcActiveChildSize();

	CMPIChildFrame * pChildFrame = pApp->m_pDocTemplate->CreateMPIFrame(IDR_MAINFRAME);

	CString StrXML;
	CIoViewReport *pIoViewReport = new CIoViewReport;
	StrXML = pChildFrame->GetDefaultXML(false, RectActive ,pIoViewReport);
	DEL(pIoViewReport);

	//USES_CONVERSION;
	//const char * KStrXML = W2A(StrXML);


	//fangz0714no
	char KStrXML[10240];
	UNICODE_2_MULTICHAR(EMCCUtf8,StrXML,KStrXML);

	TiXmlDocument doc("");
	doc.Parse(KStrXML);
	TiXmlElement *pElement = doc.FirstChildElement();
	pChildFrame->FromXml(pElement);	
	pChildFrame->SetActiveGGTongView();

	if (bMax && !pChildFrame->GetMaxSizeFlag())
	{
		pChildFrame->PostMessage(KMsgChildFrameMaxSize,0,0);
	}
}

void CMainFrame::AppendUserBlockMenu(CNewMenu * pMenu, bool32 bAddToSave /*= true*/)
{
	// 	CArray<T_Block, T_Block&> aUserBlocks;
	// 	CUserBlockManager::Instance()->GetBlocks(aUserBlocks);
	// 
	// 	if (NULL == pMenu || aUserBlocks.GetSize() <= 0 )
	// 	{
	// 		return;
	// 	}
	// 		
	// 	// 用户板块
	// 	CStringArray aUserBlockNameList;
	// 	CStringArray aUserBlockKeyList;
	// 
	// 	CNewMenu* PopUserBlock = pMenu->AppendODPopupMenu(L"用户板块");
	// 	
	// 	T_BlockMenuInfo  BlockMenuInfoToAdd;
	// 
	// 	BlockMenuInfoToAdd.StrBlockMenuNames = L"用户板块";
	// 	BlockMenuInfoToAdd.iBlockMenuIDs	 = 0;
	// 
	// 	if ( bAddToSave )
	// 	{
	// 		m_aBlockMenuInfo.Add(BlockMenuInfoToAdd);
	// 	}
	// 
	// 	for ( int32 i = 0; i < aUserBlocks.GetSize(); i++ )
	// 	{
	// 		CString StrBlockName = aUserBlocks.GetAt(i).m_StrName;
	// 		CString StrKey = aUserBlocks.GetAt(i).m_StrHotKey;
	// 
	// 		aUserBlockNameList.Add(StrBlockName);
	// 		aUserBlockKeyList.Add(StrKey);
	// 		
	// 		PopUserBlock->AppendODMenu(StrBlockName,MF_STRING,IDM_BLOCKMENU_BEGIN2+i);
	// 		
	// 		T_BlockDesc*  pBlockUser = new T_BlockDesc;	
	// 		pBlockUser->m_StrBlockName = StrBlockName;
	// 		pBlockUser->m_iMarketId	 = 0;
	// 		pBlockUser->m_eType		 = (T_BlockDesc::E_BlockType)1;
	// 		
	// 		PopUserBlock->SetItemData( IDM_BLOCKMENU_BEGIN2+i ,(DWORD)pBlockUser);
	// 		
	// 		if ( bAddToSave )
	// 		{
	// 			BlockMenuInfoToAdd.StrBlockMenuNames = StrBlockName;
	// 			BlockMenuInfoToAdd.iBlockMenuIDs = IDM_BLOCKMENU_BEGIN2+i;
	// 			m_aBlockMenuInfo.Add(BlockMenuInfoToAdd);
	// 		}	
	// 	}
}

void CMainFrame::OnUserBlockUpdate(CSubjectUserBlock::E_UserBlockUpdate eUpdateType)
{
	//// 更新自定义板块快捷键
	//if ( CSubjectUserBlock::EUBUBlock == eUpdateType )
	//{
	//	CUserBlockManager::Instance()->BuildUserBlockHotKeyList(m_HotKeyList);	
	//}

	// 避免将来被通知者每个都去做更新，删除重新加载
	BuildHotKeyList();

	// 只要更新菜单就行了
	// 	if ( CSubjectUserBlock::EUBUBlock == eUpdateType )
	// 	{
	// 		CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	// 		CGGTongDoc *pDoc = (CGGTongDoc *)pApp->m_pDocument;
	// 		CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	// 		if ( NULL != pAbsCenterManager )
	// 		{
	// 			pAbsCenterManager->BuildUserBlockHotKeyList();
	// 		}		
	// 	}
	// 
	// 	// 1:板块菜单
	// 	CMenu * pMenu = this->GetMenu();
	// 	CNewMenu *pNewMenu = DYNAMIC_DOWNCAST(CNewMenu,pMenu);
	// 	CMenu * pSubMenu = pNewMenu->GetSubMenu(L"板块(&B)");
	// 	ASSERT( NULL != pSubMenu );
	// 	CNewMenu * pRootMenu = DYNAMIC_DOWNCAST(CNewMenu,pSubMenu);
	// 	ASSERT( NULL != pRootMenu );
	// 	
	// 	// 先把用户板块删掉,再重新装载
	// 	for ( int32 j = 0; j < pRootMenu->GetMenuItemCount(); j++ )
	// 	{
	// 		CString StrMenuText;
	// 		pRootMenu->GetMenuText(j, StrMenuText, MF_BYPOSITION);
	// 		
	// 		if ( StrMenuText == L"用户板块" )
	// 		{
	// 			CNewMenu *pDelUserMenu = DYNAMIC_DOWNCAST(CNewMenu, pRootMenu->GetSubMenu(j));
	// 			DeleteBlockReportItemData(pDelUserMenu);
	// 			int32 iPos = pRootMenu->DeleteMenu(j, MF_BYPOSITION);
	// 		}
	// 	}
	// 
	// 	for ( int32 i = m_aBlockMenuInfo.GetSize() - 1 ; i >= 0 ; i--)
	// 	{
	// 		
	// 		T_BlockMenuInfo BlockInfo = m_aBlockMenuInfo.GetAt(i);
	// 		
	// 		if (BlockInfo.iBlockMenuIDs >= IDM_BLOCKMENU_BEGIN2 && BlockInfo.iBlockMenuIDs < IDM_BLOCKMENU_END2)
	// 		{
	// 			m_aBlockMenuInfo.RemoveAt(i);
	// 		}
	// 		
	// 		if ( L"用户板块" == BlockInfo.StrBlockMenuNames)
	// 		{
	// 			m_aBlockMenuInfo.RemoveAt(i);
	// 		}
	// 	}
	// 
	// 	// 2:装载菜单
	// 	AppendUserBlockMenu(pRootMenu);
	// 
	// 	// 3: 分析菜单
	// 	
	// 	//CMenu * pSubMenuF = pNewMenu->GetSubMenu(L"分析(&F)");
	// 	CMenu * pSubMenuF = GetAnalysisMenu(false);
	// 	ASSERT( NULL != pSubMenuF );
	// 	CNewMenu * pRootMenuF = DYNAMIC_DOWNCAST(CNewMenu,pSubMenuF);
	// 	ASSERT( NULL != pRootMenuF );
	// 	
	// 	//
	// 	CString StrMenuName = CIoViewManager::FindIoViewObjectByRuntimeClass(RUNTIME_CLASS(CIoViewReport))->m_StrLongName;
	// 	CMenu* pSubMenuReport = pRootMenuF->GetSubMenu(StrMenuName);
	// 	ASSERT(NULL != pSubMenuReport);
	// 	CNewMenu* pRootMenuReport = DYNAMIC_DOWNCAST(CNewMenu, pSubMenuReport);
	// 	ASSERT(NULL != pRootMenuReport);
	// 	
	// 	for ( i = 0; i < pRootMenuReport->GetMenuItemCount(); i++ )
	// 	{
	// 		CString StrMenuText;
	// 		pRootMenuReport->GetMenuText(i, StrMenuText, MF_BYPOSITION);
	// 		
	// 		if ( StrMenuText == L"用户板块" )
	// 		{
	// 			CNewMenu *pDelUserMenu = DYNAMIC_DOWNCAST(CNewMenu, pRootMenuReport->GetSubMenu(i));
	// 			DeleteBlockReportItemData(pDelUserMenu);
	// 			int32 iPos = pRootMenuReport->DeleteMenu(i, MF_BYPOSITION);
	// 		}
	// 	}
	// 	
	// 	// 4: 装载
	// 	AppendUserBlockMenu(pRootMenuReport, false);
}

void CMainFrame::OnMoving(UINT fwSide, LPRECT pRect) 
{
	CNewMDIFrameWnd::OnMoving(fwSide, pRect);

	// TODO: Add your message handler code here
	{
		CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();	

		if ( NULL != pDoc->m_pAbsCenterManager )
		{
			if ( NULL != m_pDlgF10 )
			{
				m_pDlgF10->AdjustToMaxSize();
			}
		}
	}

	//2013-12-10

	if ( m_RectOld.Width() > 0 )
	{
		m_pNewWndTB->MoveIm(pRect->left - m_RectOld.left, pRect->top - m_RectOld.top, pRect->right - m_RectOld.right, pRect->bottom - m_RectOld.bottom);
	}

	if(NULL != m_pDlgPushMsg && m_pDlgPushMsg->IsWindowVisible())
	{
		CRect rcWindow;
		GetWindowRect(&rcWindow);

		CRect rectMsg;
		rectMsg.right = rcWindow.right - iPushDlgRightSpace;
		rectMsg.left = rectMsg.right - iPushDlgWidth;
		rectMsg.bottom = rcWindow.bottom - iPushDlgBottomSpace;
		rectMsg.top = rectMsg.bottom - iPushDlgHeight;
		m_pDlgPushMsg->MoveWindow(&rectMsg);
	}

	m_RectOld = *pRect;

}

void CMainFrame::OnMouseMove(UINT nFlags, CPoint point)
{
	CNewMDIFrameWnd::OnMouseMove(nFlags, point);
}
// *****************************************************************************************************************************************
// 自画标题栏
// *****************************************************************************************************************************************
void CMainFrame::OnNcPaint()  
{
	if ( m_bFromXml  )
	{
		return;
	}

	CRect wrc;
	GetWindowRect(&wrc);
	wrc.OffsetRect(-wrc.left,-wrc.top);
	int32 val = m_nFrameWidth-3;
	if (0 < val)
	{
		wrc.DeflateRect(val,val,val,val);
	}

	CRgn rgn;
	BOOL bRet=rgn.CreateRectRgnIndirect(&wrc);
	if(bRet) 
	{
		SetWindowRgn(rgn, FALSE);
	}
	rgn.Detach();

	DrawCustomNcClient();
}

//0001681 - start
void CMainFrame::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	TRACKMOUSEEVENT	trackMouse	=	{0};
	trackMouse.cbSize			=	sizeof(trackMouse);
	trackMouse.dwFlags			=	GGT_TME_NONCLIENT | TME_LEAVE ; //TME_NONCLIENT | TME_LEAVE; //TME_NONCLIENT是加上ncclient的意思
	trackMouse.hwndTrack		=	m_hWnd;
	BOOL bMouseTrack			=	_TrackMouseEvent( &trackMouse );
	ASSERT( bMouseTrack );

	if ((HTLEFT <= nHitTest) && (HTBOTTOMRIGHT >= nHitTest || 
		HTCAPTION == nHitTest))
	{
		CNewMDIFrameWnd::OnNcMouseMove(nHitTest, point);
	}

	CRect rcWindow;
	GetWindowRect(&rcWindow);
	point.x -= rcWindow.left;
	point.y -= rcWindow.top;

	int iButton = TSysButtonHitTest(point);
	if (iButton != m_iXSysBtnHovering)
	{
		if (INVALID_ID != m_iXSysBtnHovering)
		{
			m_mapSysBtn[m_iXSysBtnHovering].MouseLeave(FALSE);
			m_iXSysBtnHovering = INVALID_ID;
		}

		if (INVALID_ID != iButton)
		{
			m_iXSysBtnHovering = iButton;
			m_mapSysBtn[m_iXSysBtnHovering].MouseHover(FALSE);
		}
		DrawCustomNcClient();
	}

	iButton = TMenuButtonHitTest(point);
	if (iButton != m_iXMenuBtnHovering)
	{
		if (INVALID_ID != m_iXMenuBtnHovering)
		{
			m_mapMenuBtn[m_iXMenuBtnHovering].MouseLeave(FALSE);
			m_iXMenuBtnHovering = INVALID_ID;
		}

		if (INVALID_ID != iButton)
		{
			m_iXMenuBtnHovering = iButton;
			m_mapMenuBtn[m_iXMenuBtnHovering].MouseHover(FALSE);
		}
		DrawCustomNcClient();
	}

	//m_pMenuBar->OnMouseMove(point);
}

LRESULT CMainFrame::OnNcMouseLeave(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	// 该功能要求使用 Windows 2000 或更高版本。
	// 符号 _WIN32_WINNT 和 WINVER 必须 >= 0x0500。
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	bool32 bFlag = false;
	map<int, CNCButton>::iterator iter;
	for (iter=m_mapSysBtn.begin(); iter!=m_mapSysBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;

		// 按钮是否需要弹起
		if (!btnControl.IsNormal())
		{
			btnControl.MouseLeave(FALSE);
			bFlag = true;
		}
		m_iXSysBtnHovering = INVALID_ID;
	}

	for (iter=m_mapMenuBtn.begin(); iter!=m_mapMenuBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;

		// 按钮是否需要弹起
		if (!btnControl.IsNormal())
		{
			btnControl.MouseLeave(FALSE);
			bFlag = true;
		}
		m_iXMenuBtnHovering = INVALID_ID;
	}

	if (bFlag)
	{
		DrawCustomNcClient();
	}

	return	Default();
}


void CMainFrame::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	CPoint pt = point;
	pt.x -= rcWindow.left;
	pt.y -= rcWindow.top;

	// 系统按钮
	int iButton = TSysButtonHitTest(pt);
	if (INVALID_ID != iButton)
	{
		m_mapSysBtn[iButton].LButtonDown(FALSE);
		DrawCustomNcClient();
		return;
	}

	// 菜单按钮
	iButton = TMenuButtonHitTest(pt);
	if (INVALID_ID != iButton)
	{
		m_mapMenuBtn[iButton].LButtonDown();
		//m_mapMenuBtn[iButton].SetCheck(TRUE, FALSE);
		//
		//map<int, CNCButton>::iterator iter;
		//// 遍历标题栏上菜单按钮
		//for (iter=m_mapMenuBtn.begin(); iter!=m_mapMenuBtn.end(); ++iter)
		//{
		//	CNCButton &btn = iter->second;
		//	if (iButton != iter->first && btn.GetCheck())
		//	{
		//		btn.SetCheckStatus(FALSE, FALSE);
		//	}
		//}
		//DrawCustomNcClient();
		return;
	}

	//if (m_pMenuBar->OnLButtonDown(pt))
	//{
	//	return;
	//}

	if ( HTMINBUTTON == nHitTest || HTMAXBUTTON == nHitTest || HTCLOSE == nHitTest || HTSYSMENU == nHitTest )
	{
		return;
	}

	CNewMDIFrameWnd::OnNcLButtonDown(nHitTest, point);
}

void CMainFrame::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	CPoint pt = point;
	pt.x -= rcWindow.left;
	pt.y -= rcWindow.top;

	int iButton = TSysButtonHitTest(pt);
	if (INVALID_ID != iButton)
	{
		switch (iButton)
		{
		case ID_CLOSE_BTN:
			{
				SendMessage(WM_SYSCOMMAND, SC_CLOSE, 0);
			}
			break;
		case ID_MAX_BTN:
			{
				if (IsZoomed())
				{
					m_mapSysBtn[iButton].MouseLeave(true);
					DrawCustomNcClient();
					SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);

				}
				else
				{
					m_mapSysBtn[iButton].MouseLeave(true);
					DrawCustomNcClient();
					SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

				}
			}
			break;
		case ID_MIN_BTN:
			{
				SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
				m_mapSysBtn[iButton].MouseLeave(FALSE);
			}
			break;
		default:
			{
				m_mapSysBtn[iButton].LButtonUp(FALSE);
			}
			break;
		}
	}
	else
	{
		// 菜单按钮
		iButton = TMenuButtonHitTest(pt);
		if (INVALID_ID != iButton)
		{
			m_mapMenuBtn[iButton].LButtonUp(FALSE);
			DrawCustomNcClient();
		}
		else
		{
			CNewMDIFrameWnd::OnNcLButtonUp(nHitTest, point);
		}
	}

}

void CMainFrame::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp)
{
	//	CNewMDIFrameWnd::OnNcCalcSize(bCalcValidRects, lpncsp);

	if (bCalcValidRects)   
	{   
		CRect &rc  = (CRect&)lpncsp->rgrc[0]; 
		rc.top += (TITLE_HEIGHT + m_nFrameWidth);
		rc.left   += m_nFrameWidth;   
		rc.bottom -= m_nFrameWidth;   
		rc.right  -= m_nFrameWidth;  
	}
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CNewMDIFrameWnd::OnSize(nType, cx, cy);

	if (NULL != m_wndMDIClient.GetSafeHwnd())
	{
		m_wndMDIClient.ArrangeIconicWindows(); // 调整最小化窗口
	}

	// 最近商品对话框
	if ( NULL != m_pDlgRecentMerch)
	{
		m_pDlgRecentMerch->SetPosition();
	}

	// F10
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();	

	if ( NULL != pDoc->m_pAbsCenterManager )
	{
		if ( NULL != m_pDlgF10 )
		{
			m_pDlgF10->AdjustToMaxSize();
		}
	}

	//
	if ( NULL != m_pNewWndTB )
	{
		CRect rect;
		GetClientRect(&rect);

		m_pNewWndTB->SetImBottom(rect.bottom);
	}

	if(NULL != m_pDlgPushMsg && m_pDlgPushMsg->IsWindowVisible())
	{
		CRect rcWindow;
		GetWindowRect(&rcWindow);

		CRect rectMsg;
		rectMsg.right = rcWindow.right - iPushDlgRightSpace;
		rectMsg.left = rectMsg.right - iPushDlgWidth;
		rectMsg.bottom = rcWindow.bottom - iPushDlgBottomSpace;
		rectMsg.top = rectMsg.bottom - iPushDlgHeight;
		m_pDlgPushMsg->MoveWindow(&rectMsg);
	}

	//2013-12-10 by cym
	if ( SIZE_MINIMIZED == nType )
	{
		m_pNewWndTB->OnHideIM();
	}

	//


	//2013-12-10
	GetWindowRect(&m_RectOld);
}

LRESULT CMainFrame::OnNcHitTest(CPoint point) 
{
	LRESULT nHT = OnMyNcHitTest(point);
	if ( nHT != HTNOWHERE )
	{
		if ( nHT != HTCAPTION )
		{
			nHT = 30;	// 只返回给一个系统没有预定义的HT给系统，回来在分析
		}
		return nHT;
	}

	// 看是否是其它的ht
	nHT = CNewMDIFrameWnd::OnNcHitTest(point);
	if ( HTMINBUTTON == nHT || HTMAXBUTTON == nHT || HTCLOSE == nHT || HTSYSMENU == nHT )
	{
		nHT = HTCAPTION;
	}
	return nHT;
}

BOOL CMainFrame::OnNcActivate(BOOL bActive) 
{
	return true;
	BOOL bRet = CNewMDIFrameWnd::OnNcActivate(bActive);
	//由于frame有activ与deactive区别和系统在调用NcActivate时，会绘制ncclient，所以需要覆盖重绘
	DrawCustomNcClient();
	m_wndReBar.RedrawWindow();
	m_wndStatusBar.RedrawWindow();
	//金评滚动条 2013-11-12
	//m_wndCommentSBar.RedrawWindow();	因为改成网页版的了，所以再刷新就不合适了。

	return bRet;
} 

void CMainFrame::OnNcLButtonDblClk(UINT nHitTest, CPoint point) 
{
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	rcWindow.bottom = rcWindow.top + m_rectCaption.bottom;

	// 当前点是否在标题栏上
	if (rcWindow.PtInRect(point))
	{
		point.x -= rcWindow.left;
		point.y -= rcWindow.top;
		int iButton = TMenuButtonHitTest(point);

		// 在按钮上双击,调用单击函数
		if (INVALID_ID != iButton)
		{
			m_mapMenuBtn[iButton].LButtonDown(FALSE);
			DrawCustomNcClient();
			return;
		}

		iButton = TSysButtonHitTest(point);
		if (INVALID_ID != iButton)
		{
			m_mapSysBtn[iButton].LButtonDown(FALSE);
			DrawCustomNcClient();
			return;
		}

		// 双击标题栏上除按钮以外的其它区域
		if (IsZoomed())
		{
			PostMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
		}
		else
		{
			PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
		}

		return;
	}
}

bool32 CMainFrame::BeMaxSize()
{	
	return IsZoomed();
}

CRect CMainFrame::CalcActiveChildSize()
{
	CRect RectActive;

	CFrameWnd * pActiveFrame = GetActiveFrame();

	if ( pActiveFrame != this)
	{
		CMPIChildFrame * pChildActive = (CMPIChildFrame *)pActiveFrame;			
		pChildActive->GetWindowRect(&RectActive);
		ScreenToClient(&RectActive);
	}
	else
	{
		RectActive = CRect(-1*NEWWINDOWOFFSETX,-1*NEWWINDOWOFFSETY,0,0);
	}

	return RectActive;
}

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
	// DESCRIPTION:	先让基类的先计算，然后再修改
	CNewMDIFrameWnd::OnGetMinMaxInfo(lpMMI);

	// 设定窗口的最小尺寸,
	lpMMI->ptMinTrackSize.x=1366;   //宽   
	lpMMI->ptMinTrackSize.y=768;   //高 

	CRect rc;
	BOOL bInfo = SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
	ASSERT( bInfo );
	if ( bInfo && !(GetStyle() & WS_CAPTION) )
	{
		lpMMI->ptMaxPosition.x	+=	rc.left;
		lpMMI->ptMaxPosition.y	+=	rc.top;
	}
	//	lpMMI->ptMaxSize.x	=	GetSystemMetrics(SM_CXMAXIMIZED);
	lpMMI->ptMaxSize.y	=	GetSystemMetrics(SM_CYMAXIMIZED);
	// 给statusBar留出一个frame
	GetSystemMetrics(SM_CYFRAME);
	// 4是XP下默认的frame高，vista下面会比4要大
	lpMMI->ptMaxSize.y	-=	3;
}

//0001681 - end

void CMainFrame::OnSelfDraw()
{
	ShowSelfDrawBar();
}

void CMainFrame::OnNetFlow()
{
	// 流量统计:
	if ( NULL != m_pDlgNetFlow )
	{
		m_pDlgNetFlow->ShowWindow(SW_SHOW);
		return;
	}

	//
	m_pDlgNetFlow = new CDlgNetFlow();
	m_pDlgNetFlow->Create(IDD_DIALOG_NET_FLOW, this);
	m_pDlgNetFlow->ShowWindow(SW_SHOW);
}

void CMainFrame::OnInfoNews()
{
	CString StrUrl;
	GetPrivateProfileStringGGTong(L"info_address", L"news_url", L"", StrUrl, GetInfoAddressFilePath());

	if (StrUrl.GetLength() > 0)
	{
		ShellExecute(0, NULL, StrUrl, NULL,NULL, SW_NORMAL);
	}
}

void CMainFrame::ShowBourseNotification(int32 iBourseId)
{
	CString StrUrl;
	GetPrivateProfileStringGGTong(L"info_address", L"bourse_notification_url", L"", StrUrl, GetInfoAddressFilePath());

	if (StrUrl.GetLength() > 0)
	{
		CString StrUrlParam;
		StrUrlParam.Format(L"?bigmarketid=%d", iBourseId);

		StrUrl += StrUrlParam;

		ShellExecute(0, NULL, StrUrl, NULL,NULL, SW_NORMAL);
	}
}

void CMainFrame::OnInfoShBourse()
{
	ShowBourseNotification(0);
}

void CMainFrame::OnInfoSzBourse()
{
	ShowBourseNotification(1);
}

void CMainFrame::OnInfoHkBourse()
{
	ShowBourseNotification(2);
}

void CMainFrame::OnHotKeyZXG()
{
	// 跳转到自选股板块
	// 1:先找有没有报价表

	CIoViewReport * pIoViewReport = FindIoViewReport(true);

	if (NULL == pIoViewReport)
	{
		//pIoViewReport = FindIoViewReport(false);
		//// 没有的话,新建一个IoViewReport
		//if (NULL == pIoViewReport)	
		//{
		//	pIoViewReport = (CIoViewReport *)CreateIoViewByPicMenuID(ID_PIC_REPORT, true);			
		//}
		bool32 bLoad = LoadSystemDefaultCfm(ID_PIC_REPORT, NULL);
		ASSERT( bLoad );
		if ( bLoad )
		{
			pIoViewReport = FindIoViewReport(true);
		}
	}

	ASSERT(NULL != pIoViewReport);			
	pIoViewReport->GetParentFrame()->BringWindowToTop();
	pIoViewReport->BringToTop();
	//////////////////////////////////////////////////////////////////////////

	CIoViewManager * pIoViewManager = (CIoViewManager *)pIoViewReport->GetParent();
	for (int32 i = 0; i < pIoViewManager->m_IoViewsPtr.GetSize(); i++)
	{
		if ( pIoViewManager->m_IoViewsPtr[i] == pIoViewReport)
		{
			pIoViewManager->m_GuiTabWnd.SetCurtab(i);
			break;
		}
	}

	bool32 bFind = pIoViewReport->SetTabByBlockType(T_BlockDesc::EBTUser);

	if (!bFind)
	{
		// 当前的报价表里面没有自选股板块,新加一个自选股板块进去.
		// 读取 user_blocks.xml 文件 ,得到第一个自选股的节点名,
		CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
		if (!pDoc->m_pAbsCenterManager)
		{
			return;
		}
		CString StrPath = CPathFactory::GetUserBlocksFileFullName(pDoc->m_pAbsCenterManager->GetUserName());

		string sPath = _Unicode2MultiChar(StrPath);
		const char* strFilePath = sPath.c_str();

		TiXmlDocument myDocument = TiXmlDocument(strFilePath);
		if (!myDocument.LoadFile())
			return ;

		TiXmlElement *pRootElement = myDocument.RootElement();
		ASSERT(NULL!=pRootElement);
		pRootElement = pRootElement->FirstChildElement();
		if (NULL == pRootElement)
			return ;

		const char * pcValue = pRootElement->Value();
		if (NULL == pcValue || 0 != strcmp( KStrElementBlockName, pcValue ))
			return ;

		const char *pcAttrValue = pRootElement->Attribute(KStrElementBlockAttriName);
		if ( NULL == pcAttrValue)
			return;

		CString StrUserBlockName = _A2W(pcAttrValue);

		T_BlockDesc BlockUser;
		BlockUser.m_eType = T_BlockDesc::EBTUser;
		BlockUser.m_StrBlockName = StrUserBlockName;

		pIoViewReport->AddBlock(BlockUser);
		pIoViewReport->SetTabByBlockType(T_BlockDesc::EBTUser);
	} 
}

void CMainFrame::OnInfoF10()
{
	// F10功能
	DoF10();
	// 	CIoViewBase *pIoViewBase = FindActiveIoView();
	// 	if (NULL == pIoViewBase)
	// 		return;
	// 
	// 	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	// 	CGGTongDoc *pDoc = (CGGTongDoc *)pApp->m_pDocument;
	// 	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	// 	ASSERT(NULL != pAbsCenterManager);
	// 
	// 	CMerch *pMerch = pIoViewBase->m_pMerchXml;
	// 	if (NULL == pMerch)
	// 	{
	// 		pMerch = pIoViewBase->GetMerchXml();
	// 		if ( NULL == pMerch )
	// 		{
	// 			//ASSERT(0);
	// 			return;
	// 		}
	// 	}
	// 	
	// 	CString StrUrl;
	// 	GetPrivateProfileStringGGTong(L"info_address", L"f10_url", L"", StrUrl, GetInfoAddressFilePath());
	// 
	// 	if (StrUrl.GetLength() > 0)
	// 	{
	// 		CString StrUrlParam;
	// 		StrUrlParam.Format(L"?bigmarketid=%d&symbolname=%s", pMerch->m_Market.m_Breed.m_iBreedId, pMerch->m_MerchInfo.m_StrMerchCode);
	// 
	// 		StrUrl += StrUrlParam;
	// 
	// 		ShellExecute(0, NULL, StrUrl, NULL,NULL, SW_NORMAL);
	// 	}
}

void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	// 最大化时候,当工作区大小变化,自动充满
	if ( m_bMaxForSysSettingChange )
	{	
	}
	CNewMDIFrameWnd::OnSettingChange(uFlags, lpszSection);
}

void CMainFrame::SetEmptyGGTongView(CGGTongView * pView)
{
	//ASSERT(NULL != pView);   // 有可能需要重置该View为NULL xl 0608
	if ( NULL != pView && !IsWindow(pView->GetSafeHwnd()) )
	{
		// 要么就是Null，要么就是窗口
		ASSERT( 0 );
		pView = NULL;
	}
	m_pEmptyGGTongView = pView;
}

void CMainFrame::SetActiveGGTongViewXml(CGGTongView * pView)
{	
	m_pActiveGGTViewFromXml = pView;
}

CGGTongView* CMainFrame::GetActiveGGTongViewXml()
{
	return m_pActiveGGTViewFromXml;
}



CIoViewBase* CMainFrame::CreateIoViewByPicMenuID(UINT nID, bool32 bShowNow, CIoViewManager* pIoViewManagerIn /*= NULL*/)
{
	// 判断是否存在子窗口
	CWnd * pChild = CWnd ::FromHandle(m_hWndMDIClient)->GetWindow(GW_CHILD);	
	if ( NULL == pChild )
	{
		OnWindowNew();		
	}

	//
	CIoViewBase * pIoView = NULL;	
	CString StrXmlName = CIoViewManager::GetIoViewString(nID);

	if ( 0 == StrXmlName.GetLength() )
	{
		return NULL;
	}

	// 存在子窗口的情况下
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd(); 
	CIoViewBase * pIoViewActive =pMainFrame->FindActiveIoView();

	CIoViewManager * pIoViewManager = NULL;

	if ( NULL == pIoViewManagerIn )
	{
		if ( NULL == pIoViewActive )
		{
			// 空白窗口! 新建一个被单击的视图		
			if ( NULL == GetEmptyGGTongView() )
			{
				// 直接从"分析" 菜单中选择视图,这时的m_pEmptyGGTongView 没有设置,为空
				CGGTongView * pView = pMainFrame->FindGGTongView(this,NULL);
				pIoViewManager	    = &(pView->m_IoViewManager);
			}
			else
			{
				// 在空白视图,点右键的情况			
				pIoViewManager = &(GetEmptyGGTongView()->m_IoViewManager);
			}
		}
		else
		{	
			pIoViewManager =(CIoViewManager *)pIoViewActive->GetParent();		
		}
	}
	else
	{
		pIoViewManager = pIoViewManagerIn;
	}

	if( NULL == pIoViewManager || NULL == pIoViewManager->GetSafeHwnd() || !::IsWindow(pIoViewManager->GetSafeHwnd()) )
	{
		if ( NULL != pIoViewManager && !IsWindow(pIoViewManager->GetSafeHwnd()) )
		{
			// IoViewManager不为空，但是又不是窗口，这个是隐患，必须ASSERT
			ASSERT( 0 );
		}
		return NULL;
	}

	int32 iSize = pIoViewManager->m_IoViewsPtr.GetSize();

	//////////////////////////////////////////////////////////////////////////
	//  判断视图中 是否已有这个 IoView,不存在的时候添加,存在的时候显示这个视图
	const T_IoViewObject * pstIoViewObject = pIoViewManager->FindIoViewObjectByXmlName(StrXmlName);
	ASSERT(NULL != pstIoViewObject);				
	ASSERT(NULL != pstIoViewObject->m_pIoViewClass);				
	CRuntimeClass * pRunTimeClasstoAdd = pstIoViewObject->m_pIoViewClass;
	bool32 bAdd = true;
	int32  iExistIndex = -1;

	for (int32 j= 0;j<iSize;j++)
	{
		if (pIoViewManager->m_IoViewsPtr[j]->IsKindOf(pRunTimeClasstoAdd))
		{
			iExistIndex = j;
			bAdd		= false;
			pIoView		= pIoViewManager->m_IoViewsPtr[j];
			break;
		}
	}

	if(m_bShowHistory && NULL == GetHistoryTrend())
	{
		// 历史分时,需要单独新增一个.
		bAdd = true;
	}

	if (bAdd)
	{
		CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
		CGGTongDoc *pDocument = pApp->m_pDocument;			
		ASSERT(NULL!= pDocument);				
		CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
		ASSERT(NULL!= pAbsCenterManager);	

		pIoView = (CIoViewBase *)pstIoViewObject->m_pIoViewClass->CreateObject();
		pIoView->SetCenterManager(pAbsCenterManager);
		pIoView->SetIoViewManager(pIoViewManager);

		if ( pIoView->IsKindOf(RUNTIME_CLASS(CIoViewTrend)))
		{
			CIoViewTrend * pIoViewTrend = (CIoViewTrend *)pIoView;
			pIoViewTrend->m_bFromXml	= false;

			if (m_bShowHistory && NULL == pMainFrame->GetHistoryTrend())
			{
				pIoViewTrend->SetHistoryFlag(true);
			}			
		}

		if ( pIoView->IsKindOf(RUNTIME_CLASS(CIoViewKLine)))
		{
			CIoViewKLine * pIoViewKLine = (CIoViewKLine *)pIoView;
			pIoViewKLine->m_bFromXml	= false;
			pIoViewKLine->SetRequestNodeFlag(true);
		}

		if ( pIoView->IsKindOf(RUNTIME_CLASS(CIoViewTick)))
		{
			CIoViewTick * pIoViewTick = (CIoViewTick *)pIoView;
			pIoViewTick->m_bFromXml	= false;
		}

		if ( pIoView->IsKindOf(RUNTIME_CLASS(CIoViewChart)) )
		{
			CIoViewChart *pChart = (CIoViewChart *)pIoView;
			pChart->m_bFromXml = false;
		}

		if ( pIoView->IsKindOf(RUNTIME_CLASS(CIoViewNews)) )
		{
			CIoViewNews *pNews = (CIoViewNews *)pIoView;
			pNews->m_bFromXml = false;
		}

		CRect rectIoViewManager;
		pIoViewManager->GetClientRect(&rectIoViewManager);

		pIoView->Create(WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE, pIoViewManager, CRect(0,0,0,0), 10204);
		pIoViewManager->m_IoViewsPtr.Add(pIoView);								
		pIoView->MoveWindow(&rectIoViewManager);
		pIoView->SetChildFrameTitle();

		// 设置该分组的商品信息
		CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();

		CMerch *pMerch = NULL;
		if (pMainFrame->GetMerchInSpecifyIoViewGroup(pIoView->GetIoViewGroupId(), pMerch))
		{
			// 当前仅根据已有的商品来change，以后需求可能变更为需要变更分组的关注商品
			pIoView->OnVDataMerchChanged(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, pMerch);
		}
		//
		pIoViewManager->m_GuiTabWnd.Addtab(pstIoViewObject->m_StrLongName, pstIoViewObject->m_StrShortName, pstIoViewObject->m_StrTipMsg);				
		int32 iTabCount = pIoViewManager->m_GuiTabWnd.GetCount();
		pIoViewManager->m_GuiTabWnd.SetTabIcon(iTabCount - 1, 0);	

		if ( bShowNow )
		{					
			// 立刻显示, 确定该显示哪个业务视图
			int32 iCountTab  = pIoViewManager->m_GuiTabWnd.GetCount();
			int32 iCountView = pIoViewManager->m_IoViewsPtr.GetSize();				
			int32 iCurView   = iCountView-1;

			ASSERT(iCountView == iCountTab);

			if (iCurView < 0 || iCurView >= iCountTab)
			{
				iCurView = 0;
			}				

			pIoViewManager->m_GuiTabWnd.SetCurtab(iCurView);	
			pIoViewManager->OnTabSelChange();

			CWnd * pParent = pIoView->GetParent()->GetParent()->GetParent();  // GGTongView 的父亲

			if (pParent->IsKindOf(RUNTIME_CLASS(CMPIChildFrame)))
			{
				((CMPIChildFrame *)pParent)->RecalcLayout();
			}
			else if (pParent->IsKindOf(RUNTIME_CLASS(CBiSplitterWnd)))
			{
				//	CBiSplitterWnd * pBiSplitWnd = (CBiSplitterWnd *)pParent;
				((CBiSplitterWnd *)pParent)->RecalcLayout();			
			}
			else if (pParent->IsKindOf(RUNTIME_CLASS(CTabSplitWnd)))
			{
				((CTabSplitWnd *)pParent)->RecalcLayout();
			}

			pParent->Invalidate();	

			//
			pIoView->GetParentGGtongView()->RedrawWindow();
			//

			for ( int32 i = 0; i < pIoView->GetIoViewManager()->m_IoViewsPtr.GetSize(); i++ )
			{
				CRect RectIoView;
				CIoViewBase* pIoViewBase = pIoView->GetIoViewManager()->m_IoViewsPtr.GetAt(i);

				pIoViewBase->GetClientRect(&RectIoView);
				LPARAM lParam = MAKELPARAM(RectIoView.Width(), RectIoView.Height());
				pIoViewBase->SendMessage(WM_SIZE, 0, lParam);					
				pIoViewBase->RedrawWindow();
			}
		}
	}
	else
	{
		//将已存在的视图设置为当前视图
		pIoViewManager->m_GuiTabWnd.SetCurtab(iExistIndex);			
		pIoViewManager->OnTabSelChange();
	}


	m_pEmptyGGTongView = NULL;  // 每次使用前GGTongView 设置这个值,用完后置空	

	//
	// xl 1119 无论是否显示，都需要在此请求数据, 因为请求数据不在被IoViewActive所控制
	if ( /*bShowNow*/NULL != pIoView )
	{
		pIoView->OnVDataForceUpdate();
	}

	return pIoView;
}

void CMainFrame::OnBrokerWatch() 
{
	if ( NULL == m_pDlgBrokerWatch)
	{
		m_pDlgBrokerWatch = new CDlgBrokerWatch;
		m_pDlgBrokerWatch->Create(IDD_DIALOG_BROKERWATCH,this);
		m_pDlgBrokerWatch->ShowWindow(SW_SHOW);	
	}	
}

void CMainFrame::OnSelectStock()
{
	CDlgChooseStock Dlg;
	Dlg.DoModal();
}

void CMainFrame::OnCustomStock()
{
	CDlgCustomStock Dlg;
	Dlg.DoModal();
}

void CMainFrame::OnIndexChooseStock()
{
	if ( NULL == m_pDlgIndexChooseStock )
	{
		m_pDlgIndexChooseStock = new CDlgIndexChooseStock;
		m_pDlgIndexChooseStock->Create(IDD_DIALOG_INDEX_CHOOSE_STOCK, this);
		m_pDlgIndexChooseStock->CenterWindow();
		m_pDlgIndexChooseStock->ShowWindow(SW_SHOW);
	}
	else
	{
		if ( ::IsWindow(m_pDlgIndexChooseStock->GetSafeHwnd()) )
		{
			m_pDlgIndexChooseStock->ShowWindow(SW_NORMAL);
			m_pDlgIndexChooseStock->SetActiveWindow();
		}
	}
}

void CMainFrame::OnViewCalc() 
{
	WinExec("CALC.EXE",0); 	
}

void CMainFrame::KeepRecentMerch(IN CMerch *pMerch)
{
	T_RecentMerch RecentMerch;

	RecentMerch.m_pMerch	 = pMerch;

	// 保持数组只有10 个.
	int32 iSize = m_aRecentMerchList.GetSize();

	// 从0-9 依次为最新-最旧 的商品数据.如果新插入的商品已有,将这个商品放到第一位
	if ( iSize < RECENTMERCHMAXNUMS)
	{
		for (int32 i = 0 ;i < iSize ; i++)
		{
			T_RecentMerch &RM = m_aRecentMerchList[i];
			if (RM.m_pMerch == pMerch)
			{				
				m_aRecentMerchList.RemoveAt(i);
				break;
			}
		}

		m_aRecentMerchList.InsertAt(0,RecentMerch);
	}
	else if ( iSize == RECENTMERCHMAXNUMS)
	{
		for (int32 i = 0 ;i < iSize ; i++)
		{
			T_RecentMerch &RM = m_aRecentMerchList[i];
			if (RM.m_pMerch == pMerch)
			{
				m_aRecentMerchList.RemoveAt(i);
				break;
			}
		}

		m_aRecentMerchList.InsertAt(0,RecentMerch);

		if (m_aRecentMerchList.GetSize() > USERBLOCKMAXNUMS)
		{
			m_aRecentMerchList.RemoveAt(USERBLOCKMAXNUMS);
		}

	}

	if ( NULL != m_pDlgRecentMerch)
	{
		m_pDlgRecentMerch->UpdateGrid();
	}
}

void CMainFrame::GetRecentMerchList(OUT CArray<T_RecentMerch, T_RecentMerch&> & RecentMerchList)
{
	RecentMerchList.RemoveAll();
	RecentMerchList.Copy(m_aRecentMerchList);
}

bool32 CMainFrame::GetWspFlagAfterInitialMarketFlag()
{
	return m_bNeedSetWspAfterInitialMarket;
}

void CMainFrame::SetWspFlagAfterInitialMarketFlag(bool32 bNeedSet)
{
	m_bNeedSetWspAfterInitialMarket = bNeedSet;
}

void CMainFrame::DealHistoryTrend(CIoViewKLine *pIoViewKLineSrc, const CKLine &KLineNow)
{
	if ( NULL == pIoViewKLineSrc )
	{
		ASSERT( 0 );
		return;
	}

	// 此实现有group一致的麻烦
	ASSERT( 0 );

	m_bShowHistory = true;

	if (NULL == m_pHistoryChild && NULL == m_pHistoryTrend )
	{
		// 应该有ioview参数

		OnWindowNew();
		m_pHistoryTrend = (CIoViewTrend * )CreateIoViewByPicMenuID(ID_PIC_TREND, true);

		if ( NULL == m_pHistoryTrend )
		{
			return;
		}

		m_pHistoryChild = (CMPIChildFrame *)m_pHistoryTrend->GetParentFrame();		
		m_pHistoryChild->SetGroupID(pIoViewKLineSrc->GetIoViewGroupId());
		if ( m_pHistoryTrend->GetIoViewManager() != NULL )
		{
			m_pHistoryTrend->GetIoViewManager()->SetGroupID(pIoViewKLineSrc->GetIoViewGroupId());
		}
		m_pHistoryChild->SetHistoryFlag(true);
		m_pHistoryTrend->SetHistoryTime(KLineNow.m_TimeCurrent);
		OnViewMerchChanged(m_pHistoryTrend, pIoViewKLineSrc->GetMerchXml());



		m_pHistoryTrend->RequestViewData();	
	}
	else if ( NULL == m_pHistoryChild && NULL != m_pHistoryTrend )
	{
		// 不应该出现这种情况
		//ASSERT(0);
	}
	else if ( NULL != m_pHistoryChild && NULL == m_pHistoryTrend )
	{
		// 在这个Child 里新建一个Trend 视图
		m_pHistoryChild->SetActiveWindow();
		m_pHistoryChild->BringWindowToTop();

		m_pHistoryTrend = (CIoViewTrend * )CreateIoViewByPicMenuID(ID_PIC_TREND, true);

		if ( NULL == m_pHistoryTrend )
		{
			return;
		}

		m_pHistoryChild = (CMPIChildFrame *)m_pHistoryTrend->GetParentFrame();		
		m_pHistoryChild->SetGroupID(pIoViewKLineSrc->GetIoViewGroupId());
		if ( m_pHistoryTrend->GetIoViewManager() != NULL )
		{
			m_pHistoryTrend->GetIoViewManager()->SetGroupID(pIoViewKLineSrc->GetIoViewGroupId());
		}
		m_pHistoryChild->SetHistoryFlag(true);
		m_pHistoryTrend->SetHistoryTime(KLineNow.m_TimeCurrent);
		OnViewMerchChanged(m_pHistoryTrend, pIoViewKLineSrc->GetMerchXml());



		m_pHistoryTrend->RequestViewData();	
	}
	else if ( NULL != m_pHistoryChild && NULL != m_pHistoryTrend)
	{
		// 把这个视图显示出来.
		m_pHistoryTrend->BringToTop();

		m_pHistoryTrend->SetHistoryTime(KLineNow.m_TimeCurrent);
		m_pHistoryTrend->RequestViewData();
	}
	else 
	{
		//ASSERT(0);
	}
}


void CMainFrame::DealHistoryTrendDlg( CIoViewKLine *pIoViewKLineSrc, const CKLine &KLineNow )
{
	// 如果是当天的，应当是显示childframe版本的分时，相当于f5
	//	历史的就调用对话框
	if ( NULL != pIoViewKLineSrc && IsWindow(pIoViewKLineSrc->GetSafeHwnd()) )
	{
		bool32 bShowIndexToolbar = FALSE, bShowTopToolbar = FALSE; 
		pIoViewKLineSrc->GetShowViewToolBar(bShowIndexToolbar, bShowTopToolbar);
		pIoViewKLineSrc->BShowViewToolBar(FALSE, FALSE);

		CDlgHistoryTrend::ShowHistoryTrend(pIoViewKLineSrc, KLineNow);

		pIoViewKLineSrc->BShowViewToolBar(bShowIndexToolbar, bShowTopToolbar);
	}
}


void CMainFrame::GenerateRandomNums()
{	
	// 生成随机数字
	unsigned ui = (unsigned)time(NULL);

	srand(ui);

	m_aiRandomNums.RemoveAll();
	m_aiRandomNums.SetSize(RAND_NUMS, 100);

	int32 * pRandomNums = (int32*)m_aiRandomNums.GetData();

	for ( int32 i = 0 ; i < RAND_NUMS; i++)
	{
		pRandomNums[i] = rand();	
	}
}

int32 CMainFrame::GetRandomNum()
{
	// 得到随机数
	if ( m_iRandIndex >= (RAND_NUMS - 1) || m_iRandIndex >= (m_aiRandomNums.GetSize()-1) )
	{
		GenerateRandomNums();
		m_iRandIndex = 0;
		return m_aiRandomNums.GetAt(0);
	}
	else
	{
		m_iRandIndex ++;
		return m_aiRandomNums.GetAt(m_iRandIndex);		
	}
}

//////////////////////////////////////////////////////////////////////////
// 菜单处理:

void CMainFrame::OnMenuBlock(UINT nID)
{
	if (NULL == m_pNewWndTB)
	{
		return;
	}

	if ( nID >= ID_REPORT_TYPEBEGIN && nID <= ID_REPORT_TYPEEND )
	{
		CMPIChildFrame *pActiveFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, MDIGetActive());
		if ( NULL != pActiveFrame && !pActiveFrame->IsLockedSplit() )
		{
			CIoViewReport *pReport = (CIoViewReport *)CreateIoViewByPicMenuID(ID_PIC_REPORT, true);
			// 这个report也有可能不是新建的
			if ( NULL != pReport )
			{
				pReport->ChangeSimpleTabClass(nID-ID_REPORT_TYPEBEGIN);
			}
		}
		else
		{
			ASSERT( 0 );	// 锁定下不允许切换报价表类型
		}
		return;
	}

	//
	int32 iMarketId = m_pNewWndTB->GetMenuMarketID(nID);
	if (iMarketId < 0)
	{
		return;
	}

	//
	OpenSpecifyMarketOrBlock(iMarketId);
}

void CMainFrame::OnIoViewMenu(UINT nID)
{
	// 所有视图公用的菜单项:
	CIoViewBase * pIoViewBase = FindActiveIoView();
	CGGTongView * pGGTongView = NULL;

	if ( NULL == pIoViewBase)
	{
		pGGTongView = (CGGTongView *)FindGGTongView();
	}
	else
	{
		CWnd * pWnd = pIoViewBase->GetParent();

		while (!(pWnd->IsKindOf(RUNTIME_CLASS(CGGTongView))))
		{
			pWnd = pWnd->GetParent();
		}

		pGGTongView =(CGGTongView *)pWnd;	
	}

	if ( NULL == pGGTongView )
	{
		return;
	}

	if ( (nID >= IDC_CHART_ADD2BLOCK_BEGIN && nID <= IDC_CHART_ADD2BLOCK_END)
		|| (nID >= ID_BLOCK_ADDTOFIRST && nID <= ID_BLOCK_ADDTO) )
	{
		// 加入自选股
		if ( NULL != pIoViewBase )
		{
			pIoViewBase->PostMessage(WM_COMMAND, nID, 0);
		}
		return;
	}
	else if (ID_EXPORT_DATA == nID)
	{
		if ( NULL != pIoViewBase )
		{
			pIoViewBase->PostMessage(WM_COMMAND, nID, 0);
		}
	}
	else if ( nID >= ID_MARK_CANCEL && nID <= ID_MARK_MANAGER )
	{
		// 标记
		if ( NULL != pIoViewBase )
		{	
			pIoViewBase->PostMessage(WM_COMMAND, nID, 0);
		}
		else if ( ID_MARK_MANAGER == nID )
		{
			CDlgMarkManager dlg;
			dlg.DoModal();
		}
		return;
	}
	else if ( ID_KLINE_HISTORYTREND == nID )
	{
		// K线历史分时
		if ( NULL != pIoViewBase )
		{
			pIoViewBase->PostMessage(WM_COMMAND, nID, 0);
		}
	}
	else if ( ID_CHART_INTERVALSTATISTIC == nID )
	{
		// K线或者分时的区间统计
		if ( NULL != pIoViewBase )
		{
			pIoViewBase->PostMessage(WM_COMMAND, nID, 0);
		}
	}
	else if ( ID_CHART_QUANTSTATISTIC == nID)
	{
		// K线的量化统计
		if ( NULL != pIoViewBase )
		{
			pIoViewBase->PostMessage(WM_COMMAND, nID, 0);
		}
	}
	else if ( ID_INVESTMENT_DIARY == nID)
	{
		// 投资日记
		if ( NULL != pIoViewBase )
		{
			pIoViewBase->PostMessage(WM_COMMAND, nID, 0);
		}
	}
	else if ( ID_CHART_CHANGETAB == nID )
	{
		if ( NULL != pIoViewBase )
		{
			OnShowMerchInNextChart(pIoViewBase);
		}
	}
	else if ( ID_ZHONGCANGCHIGU == nID || IDM_SETUPALARM == nID)
	{
		if ( NULL != pIoViewBase )
		{
			pIoViewBase->PostMessage(WM_COMMAND, nID, 0);
		}
	}
	else if ( IDM_ARBITRAGE_SHOWNEXTCHART == nID )
	{
		if ( NULL != pIoViewBase )
		{
			CArbitrage arb;
			arb.m_MerchA.m_pMerch = NULL;
			OnArbitrageF5(arb, pIoViewBase);
		}
	}
	else if ( IDM_ARBITRAGE_END >= nID && IDM_ARBITRAGE_BEGIN <= nID )
	{
		if ( NULL != pIoViewBase )
		{
			pIoViewBase->PostMessage(WM_COMMAND, nID, 0);
		}
	}

	switch(nID)
	{	
	case (IDM_IOVIEWBASE_RETURN):
		if ( NULL != pIoViewBase )
		{
			// 返回: 全部都给mainframe处理
			if ( pIoViewBase->IsKindOf(RUNTIME_CLASS(CIoViewChart)) )
			{
				((CIoViewChart*)pIoViewBase)->DealEscKey(false);
			}
			else
			{
				// 其它IoView返回到Report
				pIoViewBase->DealEscKeyReturnToReport();
			}
		}
		break;
	case (IDM_IOVIEWBASE_ADD_IOVIEW):
		{
			// 插入内容:
		}
		break;
	case (IDM_IOVIEWBASE_TAB):
		{
			// 切换内容:
			CIoViewBase* pIoViewBase = FindActiveIoView();

			if ( NULL == pIoViewBase )
			{
				return;
			}
			else
			{
				CIoViewManager* pIoViewManager  = pIoViewBase->GetIoViewManager();
				ASSERT( NULL != pIoViewManager );
				pIoViewManager->DealTabKey();
			}
		}
		break;
	case (IDM_IOVIEWBASE_CLOSECUR):
		{
			// 关闭内容:
			CIoViewBase* pIoViewBase = FindActiveIoView();

			if ( NULL == pIoViewBase )
			{
				return;
			}
			else
			{
				CIoViewManager* pIoViewManager  = pIoViewBase->GetIoViewManager();
				ASSERT( NULL != pIoViewManager );
				pIoViewManager->SendMessage(UM_IoViewTitle_Button_LButtonDown, WPARAM(IOVIEW_TITLE_DEL_BUTTON_ID), 0);
			}
			CGGTongView* pIoViewNext = (CGGTongView*)FindGGTongView();

			if ( NULL != pIoViewNext && 
				pIoViewNext->IsKindOf(RUNTIME_CLASS(CGGTongView) ))
			{
				if ( pIoViewNext->GetEmptyFlag() )
				{   //linhc 20100917修改两个焦点问题
					SetEmptyGGTongView(pIoViewNext);
				}
			}
		}
		break;
	case (IDM_IOVIEWBASE_MERCH_CHANGE):
		{
			if ( NULL == m_pDlgKeyBoard )
			{
				m_pDlgKeyBoard = new CKeyBoardDlg(m_HotKeyList,m_pKBParent);
				ASSERT(NULL != m_pDlgKeyBoard);

				m_pDlgKeyBoard->Create(IDD_KEYBOARD,m_pKBParent);
				m_pDlgKeyBoard->ShowWindow(SW_SHOW);
			}				
		}
		break;
	case (IDM_IOVIEWBASE_F7):
		{
			// 全屏/恢复:
			OnProcessF7();
		}
		break;

	case (IDM_IOVIEWBASE_FONT_BIGGER):
		{
			// 字体放大:
			CIoViewBase* pIoViewBase = FindActiveIoView();

			if ( NULL == pIoViewBase )
			{
				return;
			}
			else
			{
				pIoViewBase->ChangeFontByUpDownKey(true);
			}
		}
		break;
	case (IDM_IOVIEWBASE_FONT_SMALLER):
		{
			// 字体缩小:
			CIoViewBase* pIoViewBase = FindActiveIoView();

			if ( NULL == pIoViewBase )
			{
				return;
			}
			else
			{
				pIoViewBase->ChangeFontByUpDownKey(false);
			}
		}
		break;
	case (IDM_IOVIEWBASE_SPLIT_TOP):
		{
			// 上切
			pGGTongView->AddSplit(CMPIChildFrame::SD_TOP);	
		}
		break;
	case (IDM_IOVIEWBASE_SPLIT_BOTTOM):
		{
			// 下切:
			pGGTongView->AddSplit(CMPIChildFrame::SD_BOTTOM);	
		}
		break;
	case (IDM_IOVIEWBASE_SPLIT_LEFT):
		{
			// 左切:
			pGGTongView->AddSplit(CMPIChildFrame::SD_LEFT);	
		}
		break;
	case (IDM_IOVIEWBASE_SPLIT_RIGHT):
		{
			// 右切:
			pGGTongView->AddSplit(CMPIChildFrame::SD_RIGHT);	
		} 
		break;
	case (IDM_IOVIEWBASE_CLOSE):
		{
			// 关闭:
			CWnd * pWnd = pGGTongView->GetParent();
			while (!(pWnd->IsKindOf(RUNTIME_CLASS(CMPIChildFrame))))
			{
				pWnd = pWnd->GetParent();
			}
			CMPIChildFrame *pChildFrame = (CMPIChildFrame *)pWnd;
			pChildFrame->DelSplit(pGGTongView);		
		}
		break;
	case (ID_SETTING):
		{
			// 风格设置
			PostMessage(WM_COMMAND,ID_SETTING,0);
		}
		break;
	case (ID_LAYOUT_ADJUST):
		{
			// 版面布局
			PostMessage(WM_COMMAND,ID_LAYOUT_ADJUST,0);			
		}
		break;
	case (IDM_BELONG_BLOCK):
		{
			// 所属板块
			PostMessage(WM_COMMAND,IDM_BELONG_BLOCK,0);			
		}
		break;
	default:
		break;
	}
}


void CMainFrame::OnUpdatePicMenu( CCmdUI *pCmdUI )
{
	if ( NULL != pCmdUI )
	{
		// 		if ( pCmdUI->m_nID == ID_PIC_PHASESORT )
		// 		{
		// 			pCmdUI->Enable(TRUE);		// 这个总是允许的
		// 			return;	
		// 		}
		// 		CMPIChildFrame *pActiveFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, MDIGetActive());
		// 		if ( NULL != pActiveFrame && pActiveFrame->IsLockedSplit() )
		// 		{
		// 			pCmdUI->Enable(FALSE);	// 锁定不允许创建视图
		// 		}
		// 		else
		// 		{
		// 			pCmdUI->Enable(TRUE);
		// 		}
	}
}

void CMainFrame::OnPictureMenu(UINT nID)
{
	// 条件选股现在可以任意开，只是没数据- -
	CMPIChildFrame *pActiveFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, MDIGetActive());
	if ( NULL == pActiveFrame || !pActiveFrame->IsLockedSplit() )
	{
		// 非锁定模式下，则可以插入视图
		if ( ID_PIC_DADANPAIHANG == nID )
		{
			CString StrPrompt;
			if ( !CPluginFuncRight::Instance().IsUserHasRight(CPluginFuncRight::FuncTimeSaleRank, true) )
			{
				return;
			}
			CreateIoViewByPicMenuID(nID, true);
		}
		else if ( ID_PIC_PHASESORT == nID )
		{
			// 阶段排行， 先让用户选择，然后找页面，不存在页面则创建一个新的视图
			// 现将阶段排行变成与图标&资讯一样的页面处理
			ShowPhaseSort(NULL);
		}
		else
		{
			// 业务视图菜单项:
			CreateIoViewByPicMenuID(nID, true);
		}
	}
	else
	{
		ASSERT( NULL != pActiveFrame );
		// 锁定模式下打开特定页面，显示特定视图
		OnPicMenuCfm(nID);
	}
}

void CMainFrame::OnMenuIoViewReport(UINT nID)
{
	// 给报价表处理的菜单项
	CIoViewBase * pIoViewBase = FindActiveIoView();

	if ( NULL == pIoViewBase )
	{
		return;
	}

	if ( !pIoViewBase->IsKindOf(RUNTIME_CLASS(CIoViewReport)))
	{
		return;
	}

	CIoViewReport* pIoViewReport = (CIoViewReport*)pIoViewBase;
	pIoViewReport->PostMessage(WM_COMMAND, nID, 0);
}

void CMainFrame::OnMenuIoViewChart(UINT nID)
{	
	// 给K 线处理的菜单项
	CIoViewBase * pIoViewBase = FindActiveIoView();

	if ( NULL == pIoViewBase )
	{
		return;
	}

	bool32 bValidIoView = false;

	if ( nID >= IDM_CHART_CYCLE_BEGIN && nID <= IDM_CHART_CYCLE_END )
	{
		// 指标详情视图也会用到周期选择的菜单项
		if ( pIoViewBase->IsKindOf(RUNTIME_CLASS(CIoViewChart)) || pIoViewBase->IsKindOf(RUNTIME_CLASS(CIoViewDetail)))
		{
			bValidIoView = true;
		}
	}
	else
	{
		if ( pIoViewBase->IsKindOf(RUNTIME_CLASS(CIoViewChart)))
		{
			bValidIoView = true;
		}
	}

	if ( bValidIoView )
	{
		pIoViewBase->PostMessage(WM_COMMAND, nID, 0);
	}
}

void CMainFrame::OnAddClrKLine()
{
	CIoViewBase * pIoViewBase = FindActiveIoView();

	if ( NULL == pIoViewBase )
	{
		return;
	}

	CIoViewKLine* pKLine = (CIoViewKLine*)pIoViewBase;
	pKLine->OnAddClrKLine();
}

void CMainFrame::OnAddTrade()
{
	CIoViewBase * pIoViewBase = FindActiveIoView();

	if ( NULL == pIoViewBase )
	{
		return;
	}

	CIoViewKLine* pKLine = (CIoViewKLine*)pIoViewBase;
	pKLine->OnAddTrade();
}

void CMainFrame::OnDelAllPrompt()
{
	CIoViewBase * pIoViewBase = FindActiveIoView();

	if ( NULL == pIoViewBase )
	{
		return;
	}

	CIoViewKLine* pKLine = (CIoViewKLine*)pIoViewBase;
	pKLine->OnDelAllPrompt();
}

void CMainFrame::OnMenuIoViewDetail(UINT nID)
{
	// 给指标详情处理的菜单项
	CIoViewBase * pIoViewBase = FindActiveIoView();

	if ( NULL == pIoViewBase )
	{
		return;
	}

	if ( !pIoViewBase->IsKindOf(RUNTIME_CLASS(CIoViewDetail)))
	{
		return;
	}

	pIoViewBase->PostMessage(WM_COMMAND, nID, 0);
}

void CMainFrame::ShowWelcome()
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	///--- wangyongxue 目前不直接打开网页，使用本地图片进行显示
	CString& StrWelcomeUrl = pApp->m_pConfigInfo->m_StrWelcomeUrl;
	CString StrAdvsCode = pApp->m_pConfigInfo->m_StrWelcomeAdvsCode;
	StrWelcomeUrl = pApp->m_pConfigInfo->GetAdvsCenterUrlByCode(StrAdvsCode, EAUTPit);
	if (!StrWelcomeUrl.IsEmpty())
	{
		WebClient webClient;
		webClient.SetTimeouts(100, 200);
		webClient.DownloadFile(StrWelcomeUrl,  L"image\\Welcome.png");
	}

	CDlgWelcome* pDlgWelcom = new CDlgWelcome; 
	if ( NULL != pDlgWelcom) 
	{ 
		pDlgWelcom->Create(IDD_DIALOG_WELCOME, this);
		pDlgWelcom->ShowWindow(SW_SHOW);
		pDlgWelcom->BringWindowToTop();
	} 
}

void CMainFrame::ShowUSerRegister()
{
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if(pDoc->m_isDefaultUser)           //账号信息
	{
		m_pDlgRegister = new CDlgDefaultUserRegister;
		if(NULL != m_pDlgRegister)
		{
			m_pDlgRegister->Create(IDD_DIALOG_REGISTER, this);

			CRect rcBtn;
			m_mapSysBtn[ID_TOOLBAR_BTN].GetRect(rcBtn);
			ClientToScreen(&rcBtn);
			int iX = rcBtn.left - 190;
			int iY = rcBtn.bottom - m_rectCaption.bottom + 1;
			CRect rcWnd(iX, iY, iX + 210, iY + 130);
			m_pDlgRegister->MoveWindow(rcWnd);

			m_pDlgRegister->ShowWindow(SW_SHOW);
		}
	}
}


bool32 CMainFrame::ShowNotifyMsg()
{
	// 	if ( NULL != m_pDlgLoginNotify )	
	// 	{
	// 		m_pDlgLoginNotify->CenterWindow();			
	// 		m_pDlgLoginNotify->BringWindowToTop();
	// 		m_pDlgLoginNotify->ShowWindow(SW_SHOW);
	// 		
	// 		return true;
	// 	}
	// 
	// 	CGGTongDoc *pDoc = (CGGTongDoc*)AfxGetDocument();
	// 	if ( NULL == pDoc )
	// 	{
	// 		//ASSERT(0);
	// 		return false;
	// 	}
	// 
	// 	//	
	// 	iServiceDispose* pServiceDispose = pDoc->m_pAutherManager->GetInterface()->GetServiceDispose();
	// 	if ( NULL == pServiceDispose )
	// 	{
	// 		//ASSERT(0);
	// 		return false;
	// 	}
	// 	
	// 	TCHAR* pStrMsg	 = NULL;
	// 	TCHAR* pStrTitle = NULL;
	// 	
	// 	pServiceDispose->GetProductMsg((const TCHAR**)&pStrMsg, (const TCHAR**)&pStrTitle);
	// 	
	// 	if ( lstrlen(pStrMsg) > 0 || lstrlen(pStrTitle) )
	// 	{		
	// 		m_pDlgLoginNotify = new CDlgLoginNotify();		
	// 		if ( NULL == m_pDlgLoginNotify )
	// 		{
	// 			return false;
	// 		} 
	// 		
	// 		m_pDlgLoginNotify->Create(IDD_DIALOG_NOTIFY, GetTopWindow());
	// 		
	// 		m_pDlgLoginNotify->SetNotifyMsg(pStrTitle, pStrMsg);
	// 		// 显示不显示交由DisplayNotify决定？
	// 		m_pDlgLoginNotify->DisplayNotify();
	// 		
	// 		m_pDlgLoginNotify->CenterWindow();
	// 		m_pDlgLoginNotify->BringWindowToTop();
	// 		m_pDlgLoginNotify->ShowWindow(SW_SHOW);
	// 	}
	// 	else
	// 	{
	// 		return false;
	// 	}
	// pServiceDispose->ReleaseData(pStrMsg);
	// pServiceDispose->ReleaseData(pStrTitle);

	CGGTongDoc *pDoc = (CGGTongDoc*)AfxGetDocument();
	if ( NULL == pDoc || !pDoc->m_pAutherManager)
	{
		//ASSERT(0);
		return false;
	}

	//	
	auth::iServiceDispose* pServiceDispose = pDoc->m_pAutherManager->GetInterface()->GetServiceDispose();
	if ( NULL == pServiceDispose )
	{
		//ASSERT(0);
		return false;
	}

	TCHAR* pStrMsg	 = NULL;
	TCHAR* pStrTitle = NULL;

	// chenfj 先注释掉
	//	pServiceDispose->GetProductMsg((const TCHAR**)&pStrMsg, (const TCHAR**)&pStrTitle);

	if ( lstrlen(pStrMsg) > 0 || lstrlen(pStrTitle) )
	{		
		CDlgLoginNotify::DisplayNotification(pStrTitle, pStrMsg, false);
	}
	else
	{
		return false;
	}

	return true;
}


void CMainFrame::OnViewStatusBar()
{
	ShowControlBar(&m_wndStatusBar, !m_wndStatusBar.IsVisible(), 0);
}

//隐藏金评状态栏  2013-11-14 by cym
void CMainFrame::OnViewCommentStatusBar()
{
	ShowControlBar(&m_wndCommentSBar, !m_wndCommentSBar.IsVisible(), 0);
}

//金评状态栏按键状态  2013-11-14 by cym
void CMainFrame::OnUpdateViewStatusBar( CCmdUI *pCmdUI )
{
	pCmdUI->SetCheck( m_wndStatusBar.IsVisible() );
}

void CMainFrame::OnUpdateViewCommentStatusBar( CCmdUI *pCmdUI )
{
	pCmdUI->SetCheck( m_wndCommentSBar.IsVisible() );
}

void CMainFrame::DrawCustomNcClient()
{
	// 全部使用屏幕坐标
	GetWindowRect(&m_rectWindow);
	m_rectWindow.OffsetRect(-m_rectWindow.left, -m_rectWindow.top);
	CRect rcWindow = m_rectWindow;

	int32 iSkipX = m_nFrameWidth;
	int32 iSkipY = m_nFrameHeight;

	m_rectWindow.left	+= iSkipX;
	m_rectWindow.right	-= iSkipX;
	m_rectWindow.top	+= iSkipY;

	m_rectCaption = m_rectWindow;			
	m_rectCaption.bottom = m_rectCaption.top + TITLE_HEIGHT; 

	///////////////////////////////////////////////////////////
	int32 m_nTitleHeight = m_rectCaption.Height() + m_nFrameWidth;

	CWindowDC dc(this);
	CDC memDC;
	CRect rcClient;
	GetClientRect(&rcClient);

	
	// 剪除掉客户区
	rcClient.OffsetRect(m_nFrameWidth, m_nTitleHeight);
	dc.ExcludeClipRect(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);

	memDC.CreateCompatibleDC(&dc);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, rcWindow.Width(), rcWindow.Height());
	memDC.SelectObject(&bmp);
	memDC.SetBkMode(TRANSPARENT);


	COLORREF colorBk;
	colorBk = RGB(255,255,0);
	CBrush brushBK(colorBk);
	memDC.FillRect(rcClient, &brushBK);


	Graphics graphics(memDC.GetSafeHdc());
	RectF destRect;

	// 边框
	Pen pen(Color(255,34,34,40), 2*m_nFrameWidth);
	Pen nPen(Color(255, 51, 56, 63), 2*m_nFrameWidth);
	graphics.DrawLine(&pen, 0, 0, rcWindow.Width(), 0);
	graphics.DrawLine(&nPen, 0, rcWindow.Height(), rcWindow.Width(), rcWindow.Height());
	graphics.DrawLine(&pen, 0, 0, 0, TITLE_HEIGHT + 8);
	graphics.DrawLine(&nPen,0,TITLE_HEIGHT+ 8, 0, rcWindow.Height());
	graphics.DrawLine(&pen, rcWindow.Width(), 0, rcWindow.Width(), TITLE_HEIGHT+ 8);
	graphics.DrawLine(&nPen, rcWindow.Width(), TITLE_HEIGHT+ 8, rcWindow.Width(), rcWindow.Height());

	// 标题栏背景图
	destRect.X = m_nFrameWidth;
	destRect.Y = m_rectCaption.top;
	destRect.Width  = m_rectCaption.Width();
	destRect.Height = m_rectCaption.Height();
	ImageAttributes ImgAtt;
	ImgAtt.SetWrapMode(WrapModeTileFlipXY);
	graphics.DrawImage(m_pImgLogo, destRect, 0, 0, m_pImgLogo->GetWidth(), m_pImgLogo->GetHeight(),	UnitPixel, &ImgAtt);

	// 标题栏左边logo
	int32 iAdjust = 0;
	int32 iLogoWidth = m_pImgLogoLeft->GetWidth();
	destRect.X = m_nFrameWidth + iAdjust;
	destRect.Y = m_rectCaption.top + (m_rectCaption.Height() - m_pImgLogoLeft->GetHeight()) / 2 ;
	destRect.Height = m_pImgLogoLeft->GetHeight();//m_rectCaption.Height() - iAdjust * 2;
	destRect.Width = m_pImgLogoLeft->GetWidth();
	graphics.DrawImage(m_pImgLogoLeft, destRect);

	int32 iTitleLength = 0;
	int32 iSpace = 40; // 距离图标的距离
	// 标题栏文本
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	CString strTitle = pApp->m_pszAppName;
	//获取客服电话号码
	auth::T_TextBannerConfig  CommTextCfg;
	CGGTongDoc* pDoc = pApp->m_pDocument;
	if (!pDoc->m_pAutherManager)
	{
		return;
	}
	pDoc->m_pAutherManager->GetInterface()->GetServiceDispose()->GetTextBanner(CommTextCfg);
	CString strCustServ ;
	strCustServ.Format(_T("热线: %s"),CommTextCfg.wszServiceTel);
	CString strNameTitle = L"您好:";
	//获取用户昵称
	//	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;

	CString strNickName = m_stUserInfo.wszNickName;
	if(strNickName.IsEmpty())
	{
		strNickName = m_stUserInfo.wszMobile;
	}

	if (!pApp->m_pConfigInfo->m_bMember || (EType_Hide==pApp->m_pConfigInfo->m_btnUserInfo.m_eShowType))
	{
		strNameTitle = _T("");
		strNickName = _T("");
	}

	int iNameLen = strNickName.GetLength();

	if (iNameLen > 4)
	{
		strNickName = strNickName.Left(4);
		strNickName += "...";
	}

	RectF grect;
	grect.X = (REAL)40;
	grect.Y = (REAL)m_nFrameHeight - 4;
	grect.Width = (REAL)80;
	grect.Height = m_rectCaption.Height();

	RectF gCustServrect;
	gCustServrect.X = (REAL) 0.5 * m_rectCaption.Width();
	gCustServrect.Y = (REAL)m_nFrameHeight;
	gCustServrect.Width = (REAL)80;
	gCustServrect.Height = m_rectCaption.Height();

	RectF gNameTitlerect;
	gNameTitlerect.X = (REAL) 0.5 * m_rectCaption.Width();
	gNameTitlerect.Y = (REAL)m_nFrameHeight;
	gNameTitlerect.Width = (REAL)80;
	gNameTitlerect.Height = m_rectCaption.Height();

	RectF gNickNamerect;
	gNickNamerect.X = (REAL)40;
	gNickNamerect.Y = (REAL)m_nFrameHeight;
	gNickNamerect.Width = (REAL)80;
	gNickNamerect.Height = m_rectCaption.Height();

	//绘制文字
	StringFormat strFormat;
	strFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	strFormat.SetAlignment(StringAlignmentCenter);
	strFormat.SetLineAlignment(StringAlignmentCenter);

	typedef struct T_NcFont 
	{
	public:
		CString	m_StrName;
		float   m_Size;
		int32	m_iStyle;

	}T_NcFont;
	T_NcFont m_Font1, m_Font2, m_Font3;
	//标题栏文本
	m_Font1.m_StrName = gFontFactory.GetExistFontName(L"微软雅黑");	//...
	m_Font1.m_Size	 = 9.5f;
	m_Font1.m_iStyle	 = FontStyleRegular|FontStyleBold;	
	Gdiplus::FontFamily fontFamily(m_Font1.m_StrName);
	Gdiplus::Font font1(&fontFamily, m_Font1.m_Size, m_Font1.m_iStyle, UnitPoint);

	//客服电话
	m_Font2.m_StrName = gFontFactory.GetExistFontName(L"微软雅黑");	//...
	m_Font2.m_Size	 = 9.5f;
	m_Font2.m_iStyle	 = FontStyleRegular;	
	Gdiplus::Font font2(&fontFamily, m_Font2.m_Size, m_Font2.m_iStyle, UnitPoint);

	//用户昵称
	m_Font3.m_StrName = gFontFactory.GetExistFontName(L"SimSun");	//...
	m_Font3.m_Size	 = 8.7f;
	m_Font3.m_iStyle	 = FontStyleRegular;	
	Gdiplus::Font font3(&fontFamily, m_Font3.m_Size, m_Font3.m_iStyle, UnitPoint);

	RectF rcBound;
	RectF rcCustServ;
	RectF rcNameTitle;
	RectF rcNickName;
	PointF point;

	graphics.MeasureString(strTitle, strTitle.GetLength(), &font1, point, &strFormat, &rcBound);
	grect.Width = rcBound.Width;
	iTitleLength = (long)rcBound.Width;
	m_nTitleLength = iTitleLength;

	graphics.MeasureString(strCustServ, strCustServ.GetLength(), &font2,point, &strFormat, &rcCustServ);
	gCustServrect.Width = rcCustServ.Width;

	graphics.MeasureString(strNameTitle, strNameTitle.GetLength(), &font1,point, &strFormat, &rcNameTitle);
	gNameTitlerect.Width = rcNameTitle.Width;

	graphics.MeasureString(strNickName, strNickName.GetLength(), &font3, point, &strFormat, &rcNickName);
	gNickNamerect.Width = rcNickName.Width;


	// 分割图片
	CRect rcSpliter;
	int iSpliterW = 1;
	int iSpliterH = 14;
	if(m_pImgSpliter && m_pImgSpliter->GetLastStatus() == Ok)
	{
		iSpliterW = m_pImgSpliter->GetWidth();
		iSpliterH = m_pImgSpliter->GetHeight();
	}

	int iBtnTopDistance = (m_rectCaption.Height() - iSpliterH) / 2 + 1;
	int iBegPos = rcWindow.left + m_nFrameWidth + iLogoWidth;    // 横向起始位置
	int iTop = rcWindow.top + m_nFrameWidth + iBtnTopDistance;                             // 纵向起始位置
	int iBottom = iTop + iSpliterH;
	destRect.X = iBegPos;
	destRect.Y = iTop;
	destRect.Width  = iSpliterW;
	destRect.Height = iSpliterH;
	graphics.DrawImage(m_pImgSpliter, destRect, 0, 0, iSpliterW, iSpliterH, UnitPixel);

	int iCustomSpace = 16;     // 分割图片距离系统按钮的位置
	iBegPos += (iSpliterW + iCustomSpace);
	// 绘制系统按钮
	int iBtnWidth = 64;
	int iBtnHeight = 29;
	if(m_pImgMenuSystem && m_pImgMenuSystem->GetLastStatus() == Ok)
	{
		iBtnWidth = m_pImgMenuSystem->GetWidth();
		iBtnHeight = m_pImgMenuSystem->GetHeight() / 3;
		iTop = rcWindow.top + m_nFrameWidth + (TITLE_HEIGHT - iBtnHeight) / 2;
		iBottom = iTop + iBtnHeight;
	}
	CRect rcSystem(iBegPos, iTop, iBegPos + iBtnWidth, iBottom);
	m_mapMenuBtn[ID_MENU_SYSTEM].SetRect(rcSystem);
	m_mapMenuBtn[ID_MENU_SYSTEM].SetTextOffPos(CPoint(-4,0));

	// 绘制公式管理按钮
	iBegPos += iBtnWidth;
	if(m_pImgMenuAlarm && m_pImgMenuAlarm->GetLastStatus() == Ok)
	{
		iBtnWidth = m_pImgMenuAlarm->GetWidth();
		iBottom = iTop + m_pImgMenuAlarm->GetHeight() / 3;
	}
	CRect rcFormulaManage(iBegPos, iTop, iBegPos + iBtnWidth, iBottom);
	m_mapMenuBtn[ID_MENU_FORMULA_MANAGE].SetRect(rcFormulaManage);
	//m_mapMenuBtn[ID_MENU_FORMULA_MANAGE].SetTextOffPos(CPoint(8,0));

	// 绘制条件预警按钮
	iBegPos += iBtnWidth;
	if(m_pImgMenuAlarm && m_pImgMenuAlarm->GetLastStatus() == Ok)
	{
		iBtnWidth = m_pImgMenuAlarm->GetWidth();
		iBottom = iTop + m_pImgMenuAlarm->GetHeight() / 3;
	}
	CRect rcAlarm(iBegPos, iTop, iBegPos + iBtnWidth, iBottom);
	m_mapMenuBtn[ID_MENU_ALARM].SetRect(rcAlarm);

	// 绘制选股按钮
	iBegPos += iBtnWidth;
	if(m_pImgMenuSelStock && m_pImgMenuSelStock->GetLastStatus() == Ok)
	{
		iBtnWidth = m_pImgMenuSelStock->GetWidth();
		iBottom = iTop + m_pImgMenuSelStock->GetHeight() / 3;
	}
	CRect rcSelStock(iBegPos, iTop, iBegPos + iBtnWidth, iBottom);
	m_mapMenuBtn[ID_MENU_SEL_STOCK].SetRect(rcSelStock);
	m_mapMenuBtn[ID_MENU_SEL_STOCK].SetTextOffPos(CPoint(-4,0));

	// 绘制交易按钮
	iBegPos += iBtnWidth;
	if(m_pImgMenuTrade && m_pImgMenuTrade->GetLastStatus() == Ok)
	{
		iBtnWidth = m_pImgMenuTrade->GetWidth();
		iBottom = iTop + m_pImgMenuTrade->GetHeight() / 3;
	}
	CRect rcTrade(iBegPos, iTop, iBegPos + iBtnWidth, iBottom);
	m_mapMenuBtn[ID_MENU_TRADE].SetRect(rcTrade);
	m_mapMenuBtn[ID_MENU_TRADE].SetTextOffPos(CPoint(-4,0));

	// 绘制帮助按钮
	iBegPos += iBtnWidth;
	if(m_pImgMenuHelp && m_pImgMenuHelp->GetLastStatus() == Ok)
	{
		iBtnWidth = m_pImgMenuHelp->GetWidth();
		iBottom = iTop + m_pImgMenuHelp->GetHeight() / 3;
	}
	CRect rcHelp(iBegPos, iTop, iBegPos + iBtnWidth, iBottom);
	m_mapMenuBtn[ID_MENU_HELP].SetRect(rcHelp);
	m_mapMenuBtn[ID_MENU_HELP].SetTextOffPos(CPoint(-4,0));

	// 菜单栏重绘
	//CRect rcMenuBar;
	//if (NULL != m_pMenuBar)
	//{
	//	m_pMenuBar->ReleaseMenuBtn();
	//	m_pMenuBar->OnPaint(&graphics);
	//	m_pMenuBar->GetMenuBarRect(rcMenuBar);
	//}
	int32 iMenuRight = rcHelp.right;

	// 系统按钮位置
	int iBtnWidth2 = 29;
	iTop += 2;    //调整系统按钮位置，使其居中显示
	if(m_pImgSysBtn && m_pImgSysBtn->GetLastStatus() == Ok)
	{
		iBtnWidth2 = m_pImgSysBtn->GetWidth()/4;
		iBottom = iTop + m_pImgSysBtn->GetHeight() / 2;
	}

	iBegPos = rcWindow.right - m_nFrameWidth - iBtnWidth2 - 6;
	CRect rClose(iBegPos, iTop, iBegPos+iBtnWidth2, iBottom );
	m_mapSysBtn[ID_CLOSE_BTN].SetRect(rClose);
	m_rectClose = rClose;

	CRect rMax(iBegPos-iBtnWidth2, iTop, iBegPos, iBottom);
	m_mapSysBtn[ID_MAX_BTN].SetRect(rMax);
	m_rectMax = rMax;

	CRect rMin(iBegPos-iBtnWidth2*2,  iTop, iBegPos-iBtnWidth2, iBottom);
	m_mapSysBtn[ID_MIN_BTN].SetRect(rMin);
	m_rectMin = rMin;


	iBegPos = iBegPos-iBtnWidth2*2 - iSpliterW;


	//收起按钮
	int iHideWidth = 30;
	int iHideHeight = 29;
	if(m_pImgHideBtn && m_pImgHideBtn->GetLastStatus() == Ok)
	{
		iHideWidth = m_pImgHideBtn->GetWidth();
		iHideHeight = m_pImgHideBtn->GetHeight() / 3;
	}
	//CRect rctShow(iBegPos - iHideWidth , rcWindow.top + m_nFrameWidth - 1 , iBegPos, rcWindow.top + m_nFrameWidth -1 + iHideHeight );
	//m_mapSysBtn[ID_TOOLBAR_BTN].SetRect(rctShow);

	CRect rcLeft(0, 0, 0, 0);
	SolidBrush brush((ARGB)Color::White);
	brush.SetColor(Color::Color(227,185,185));
	//iTop += 2;
	iBegPos = iBegPos - 3;
	if(!pDoc->m_isDefaultUser)           //非游客用户显示用户头像，用户昵称，退出按钮
	{
		int iBtnWidthExit = 45;
		if(m_pImgMenuExit && m_pImgMenuExit->GetLastStatus() == Ok)
		{
			iBtnWidthExit = m_pImageLogin->GetWidth();
		}
		int iBegPosExit = iBegPos - iBtnWidthExit - 33;

		//退出
		CRect rExit(iBegPosExit, iTop , iBegPosExit + iBtnWidthExit, iBottom );
		m_mapMenuBtn[ID_MENU_EXIT].SetRect(rExit);

		int iBtnWidthName = 0;
		int iBegPosName = iBegPosExit ;
		if(!strNickName.IsEmpty())
		{
			iBtnWidthName = (int)gNickNamerect.Width + 10;
			iBegPosName = iBegPosName - iBtnWidthName - 10 ;  //调整昵称显示的位置，10个像素是昵称和退出按钮之间的间隙

			rcLeft.SetRect(iBegPosName, iTop, iBegPosName+iBtnWidthName, iBottom);
			if (pApp->m_pConfigInfo->m_bMember && (EType_Hide!=pApp->m_pConfigInfo->m_btnUserInfo.m_eShowType))
			{
				CRect rectName = rcLeft;
				m_mapMenuBtn[ID_MENU_USER_NAME].SetRect(rectName);
				//gNameTitlerect.X = rcLeft.left - rcNameTitle.Width + 3;
				//gNameTitlerect.Y = m_rectCaption.top-2;
				//gNameTitlerect.Height = rcLeft.Height();
				//graphics.DrawString(strNameTitle, strNameTitle.GetLength(), &font2, gNameTitlerect, &strFormat,&brush );    //暂时不需要显示 "您好", 隐藏
			}
		}
		else
		{
			iBegPosName = iBegPosName - 50;
		}

		// 用户头像
		if(m_pImgUserPhoto && m_pImgUserPhoto->GetLastStatus()== Ok)
		{
			iCustomSpace = 20;
			RectF destRect;
			destRect.X = iBegPosName - iCustomSpace -2;
			destRect.Y = iTop - 3;
			destRect.Width  = iCustomSpace;
			destRect.Height = iCustomSpace;

			Gdiplus::GraphicsPath path;
			path.AddEllipse(iBegPosName - iCustomSpace -2 , iTop - 3,  iCustomSpace, iCustomSpace);
			Color clr(255, 255,255,255);
			Gdiplus::Pen pen(clr, 1.00f);
			graphics.SetClip(&path);    
			graphics.DrawImage(m_pImgUserPhoto, destRect, 0, 0, m_pImgUserPhoto->GetWidth(),m_pImgUserPhoto->GetHeight(), UnitPixel);
			graphics.ResetClip();
			graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
			graphics.DrawPath(&pen, &path);

		}
	}
	else            //登录，注册
	{
		int iBtnWidthLogin = m_pImageLogin->GetWidth();
		int iBegPosLogin = iBegPos -  iBtnWidthLogin - 48;

		//登录
		CRect rLogin(iBegPosLogin, iTop , iBegPosLogin+iBtnWidthLogin, iBottom );
		m_mapMenuBtn[ID_MENU_LOGIN].SetRect(rLogin);

		RectF destRect;
		destRect.X = iBegPosLogin - m_pImgLoginSpliter->GetWidth();
		destRect.Y = iTop + 3;
		destRect.Width  = m_pImgLoginSpliter->GetWidth();
		destRect.Height = m_pImgLoginSpliter->GetHeight();
		graphics.DrawImage(m_pImgLoginSpliter, destRect, 0, 0, m_pImgLoginSpliter->GetWidth(),m_pImgLoginSpliter->GetHeight(), UnitPixel);
		

		//注册
		iBegPosLogin = iBegPosLogin - iBtnWidthLogin - m_pImgLoginSpliter->GetWidth() ;
		rcLeft.SetRect(iBegPosLogin, iTop, iBegPosLogin+iBtnWidthLogin, iBottom );
		m_mapMenuBtn[ID_MENU_REGISTER].SetRect(rcLeft);
	}

	// 这两块暂时不显示，予以隐藏
	{
		//绘制标题栏客户电话文本
		gCustServrect.X = rcLeft.left - rcCustServ.Width- 12;
		gCustServrect.Y = 2;

		if (iMenuRight < gCustServrect.X)
		{
			gCustServrect.Height = m_rectCaption.bottom - 1;
			brush.SetColor(Color(0xf0, 0xf0, 0xf0));
			//graphics.DrawString(strCustServ, strCustServ.GetLength(), &font2, gCustServrect,&strFormat, &brush);
		}

		// 在线客服
		//CRect rcOnlienServ ;
		//rcOnlienServ.left = gCustServrect.X - m_pImgOnlineServ->GetWidth() - 16;
		//rcOnlienServ.top = iTop - 8;
		//rcOnlienServ.right = rcOnlienServ.left + m_pImgOnlineServ->GetWidth();
		//rcOnlienServ.bottom = iBottom;
		//m_mapMenuBtn[ID_MENU_ONLINE_SERVICE].SetRect(rcOnlienServ);
	}


	int32 iSysLeft = iBegPos;

	// 绘制标题栏文本
	if (0 == iTitleLength)
	{
		int32 iStrPos = iMenuRight + (iSysLeft - iMenuRight - (long)grect.Width)/2;//(m_rectWindow.Width()-grect.Width)/2;
		grect.X = (REAL)(iStrPos + 2);
		grect.Y = m_rectCaption.top;
		if (grect.Width > (iSysLeft - iMenuRight))
		{
			grect.X = (REAL)iMenuRight;
			grect.Width = iSysLeft - iMenuRight;
		}
		grect.Height = m_rectCaption.Height();
	}

	brush.SetColor(Color::Color(0, 0, 0));
	grect.X = 35;
	grect.Y = m_rectCaption.top ;	// 调整标题文字的Y坐标，居中显示
	grect.Height = m_rectCaption.Height();
	//graphics.DrawString(strTitle, strTitle.GetLength(), &font1, grect, &strFormat, &brush);

	//
	CRect rcPaint;
	dc.GetClipBox(&rcPaint);
	map<int, CNCButton>::iterator iter;

	// 遍历标题栏上菜单按钮
	for (iter=m_mapMenuBtn.begin(); iter!=m_mapMenuBtn.end(); ++iter)
	{

		if(!iter->second.GetCreate())
		{
			continue;;
		}
		CNCButton &btnControl = iter->second;
		CRect rcControl;
		btnControl.GetRect(rcControl);

		// 判断当前按钮是否需要重绘
		if (!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;
		}
		btnControl.DrawButton(&graphics);
	}

	// 遍历标题栏上系统按钮
	for (iter=m_mapSysBtn.begin(); iter!=m_mapSysBtn.end(); ++iter)
	{

		CNCButton &btnControl = iter->second;
		CRect rcControl;
		btnControl.GetRect(rcControl);

		if(!iter->second.GetCreate())
		{
			continue;;
		}
		// 判断当前按钮是否需要重绘
		if (!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;
		}

		int index = 0;
		int iD = btnControl.GetControlId();
		switch (iD) //系统按钮 1 min, 2 max, 3 restore, 4 close
		{
		case ID_MIN_BTN:
			{
				index = 1;
				btnControl.SetSysIndex(index);
				btnControl.DrawSysButton(&graphics, index);
				break;
			}
		case ID_MAX_BTN:
			{
				if (IsZoomed())
				{
					index = 3;
				}
				else
				{
					index = 2;
				}
				btnControl.SetSysIndex(index);
				btnControl.DrawSysButton(&graphics, index);
				break;
			}
		case ID_CLOSE_BTN:
			{
				index = 4;
				btnControl.SetSysIndex(index);
				btnControl.DrawSysButton(&graphics, index);
				break;
			}
		default:
			{
				btnControl.DrawButton(&graphics);
				break;
			}
		}
	}

	dc.BitBlt(0, 0, rcWindow.Width(), rcWindow.Height(), &memDC, 0, 0, SRCCOPY);
	dc.SelectClipRgn(NULL);
	memDC.DeleteDC();
	bmp.DeleteObject();
}

CGGTongView		* CMainFrame::GetEmptyGGTongView()
{
	if ( NULL != m_pEmptyGGTongView && !IsWindow(m_pEmptyGGTongView->GetSafeHwnd()) )
	{
		ASSERT( 0 );
		m_pEmptyGGTongView = NULL;
	}
	return m_pEmptyGGTongView;
}




void CMainFrame::OnChildFrameMaximized( bool32 bMax )
{

}

bool32 CMainFrame::OpenWorkSpace( LPCTSTR lpszPath )
{
	if ( NULL == lpszPath )
	{
		OnNewWorkspace();
		return true;
	}

	bool32 bRet = false;

	CString StrFilePath(lpszPath);
	// 参数是文件的完整路径名
	CFileFind file;
	BOOL bExist = file.FindFile(StrFilePath);	
	ASSERT(bExist);	

	// 转换后缀名
	// xl 0621 为什么要转换后缀？？

	// 	int32 iLength = StrFilePath.GetLength();
	// 	CString StrOldName = StrFilePath;
	// 	CString StrNewName = StrFilePath.Left(iLength-4) + L".xml";
	//	int32 iLength = StrFilePath.GetLength();
	CString StrOldName = StrFilePath;
	CString StrNewName = StrFilePath;

	// 	TRY
	// 	{
	// 		CFile::Rename(StrOldName,StrNewName);
	// 	}
	// 	CATCH( CFileException, e )
	// 	{
	// 		#ifdef _DEBUG
	// 			afxDump << L"File " << StrOldName << L" not found, cause = "
	// 				<< e->m_cause << L"\n";
	// 		#endif
	// 	}
	// 	END_CATCH


	if (bExist)
	{
		USES_CONVERSION;		

		char FilePath[MAX_PATH];
		strcpy(FilePath,_W2A(StrNewName));

		TiXmlDocument myDocument;

		if ( myDocument.LoadFile() == FALSE )
		{
			return false;
		}

		TiXmlElement *  pRootElement = myDocument.RootElement();
		CString StrAttridata = pRootElement->Attribute(GetXmlRootElementAttrData());

		pRootElement = pRootElement->FirstChildElement();

		// 设置当前工程名,为打开的文件名
		if ( 0 == KStrXmlRootElementAttrDataWorkSpace.CompareNoCase(StrAttridata) )
		{	
			CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
			CGGTongApp * pApp = (CGGTongApp *)AfxGetApp();

			pApp->m_StrCurWsp = StrFilePath;
			// 还要先清除一下
			pMainFrame->ClearCurrentWorkSpace();
			pMainFrame->FromXml(FilePath);				
			bRet = true; // 无论是否xml成功，现在都已经无可挽回了
		}
		else
		{
			MessageBox(L"文件解析出错,请选择其他工作区文件!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);		
		}
	}
	else
	{
		MessageBox(L"您所查看的页面文件不存在!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		//... 选择一个默认的		
	}
	return bRet;
}

void CMainFrame::RegisterAcitveIoView( CIoViewBase *pIoView, bool32 bActive )
{
}

bool32 CMainFrame::DoF10()
{
	CIoViewBase* pIoView = FindActiveIoView();
	CMerch* pMerch = NULL;
	if ( NULL == pIoView )
	{
		return false;
	}

	if ( pIoView->IsKindOf(RUNTIME_CLASS(CIoViewReport)) )
	{
		pMerch = ((CIoViewReport*)pIoView)->GetGridSelMerch();
	}
	else
	{
		pMerch = pIoView->GetMerchXml();	// 默认为pMerchXml
	}

	if ( NULL != pIoView && NULL != pMerch )
	{
		if ( NULL != m_pDlgF10 )
		{
			//判断对话框是否有显示
			if (m_pDlgF10->IsWindowVisible())
			{
				if(m_pDlgF10->OnTypeChange(L"002"))
				{
					//对话框内容成功从F11切换到F10
				}
				else
				{
					//对话框内容切换失败，说明本身内容就是F10，故第2次点击就是隐藏对话框
					m_pDlgF10->ShowWindow(SW_HIDE);
				}
			}
			else
			{
				//对话框是隐藏的，重新显示并切换内容
				m_pDlgF10->OnTypeChange(L"002",false);
				m_pDlgF10->OnMerchChange(pMerch);
				m_pDlgF10->ShowWindow(SW_SHOW);
			}
		}
		else
		{ 
			//m_pDlgF10 = new CDlgF10(pIoView, pMerch, this);
			m_pDlgF10 = new CDlgWebF10(pIoView, pMerch, this);
			m_pDlgF10->OnTypeChange(L"002");
			m_pDlgF10->Create(IDD_DIALOG_F10, this);
			m_pDlgF10->ShowWindow(SW_SHOW);
		}

		return true;
	}

	return false;
}

bool32 CMainFrame::DoF11()
{
	//--- wangyongxue 2017/03/04 暂时先屏蔽个股资讯快捷键响应
	if (m_pNewWndTB)
	{
		m_pNewWndTB->DoOpenCfm(L"资讯");
	}

	return true;

	CIoViewBase* pIoView = FindActiveIoView();
	CMerch* pMerch = NULL;
	if ( NULL == pIoView )
	{
		return false;
	}

	if ( pIoView->IsKindOf(RUNTIME_CLASS(CIoViewReport)) )
	{
		pMerch = ((CIoViewReport*)pIoView)->GetGridSelMerch();
	}
	else
	{
		pMerch = pIoView->GetMerchXml();	// 默认为pMerchXml
	}

	if ( NULL != pIoView && NULL != pMerch )
	{
		if ( NULL != m_pDlgF10 )
		{
			//判断对话框是否有显示
			if (m_pDlgF10->IsWindowVisible())
			{
				if(m_pDlgF10->OnTypeChange(L"001"))
				{
					//对话框内容成功从F10切换到F11
				}
				else
				{
					//对话框内容切换失败，说明本身内容就是F11，故第2次点击就是隐藏对话框
					m_pDlgF10->ShowWindow(SW_HIDE);
				}
			}
			else
			{
				//对话框是隐藏的，重新显示并切换内容
				m_pDlgF10->OnTypeChange(L"001",false);
				m_pDlgF10->OnMerchChange(pMerch);
				m_pDlgF10->ShowWindow(SW_SHOW);
			}
		}
		else
		{ 
			m_pDlgF10 = new CDlgWebF10(pIoView, pMerch, this);
			m_pDlgF10->OnTypeChange(L"001");
			m_pDlgF10->Create(IDD_DIALOG_F10, this);
			m_pDlgF10->ShowWindow(SW_SHOW);
		}

		return true;
	}

	return false;
}

void CMainFrame::OnMove( int x, int y )
{
	CNewMDIFrameWnd::OnMove(x, y);

	if ( NULL != m_pDlgF10 )
	{
		m_pDlgF10->AdjustToMaxSize();
	}
}

UINT CMainFrame::OnMyNcHitTest( CPoint point )
{
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	CPoint pt = point;
	pt.x -= rcWindow.left;
	pt.y -= rcWindow.top;

	UINT iHT = HTNOWHERE;
	if ( m_rectClose.PtInRect(pt) )	//最高优先
	{
		iHT =  HTCLOSE;
	}
	else if ( m_rectMax.PtInRect(pt) )
	{
		iHT =  HTMAXBUTTON;
	}
	else if ( m_rectMin.PtInRect(pt) )
	{
		iHT =  HTMINBUTTON;
	}
	else if ( m_rectCaption.PtInRect(pt) )
	{
		iHT =  HTCAPTION;
	}
	return iHT;
}

int CMainFrame::OnToolHitTest( CPoint point, TOOLINFO* pTI ) const
{
	int iRet = CNewMDIFrameWnd::OnToolHitTest(point, pTI);
	if ( -1 == iRet && pTI )
	{
		if ( m_rectMin.PtInRect(point) )
		{
			TCHAR *pszTip = (TCHAR *)malloc(30);
			if ( IsIconic() )
			{
				_tcscpy(pszTip, _T("向上还原"));
			}
			else
			{
				_tcscpy(pszTip, _T("最小化"));
			}
			pTI->uFlags = 0;
			pTI->hwnd = m_hWnd;
			pTI->lpszText = pszTip;
			pTI->rect = m_rectMin;
			pTI->uId = HTMINBUTTON;
			iRet = pTI->uId;
		}
		else if ( m_rectMax.PtInRect(point) )
		{
			TCHAR *pszTip = (TCHAR *)malloc(30);
			if ( IsZoomed() )
			{
				_tcscpy(pszTip, _T("向下还原"));
			}
			else
			{
				_tcscpy(pszTip, _T("最大化"));
			}
			pTI->uFlags = 0;
			pTI->hwnd = m_hWnd;
			pTI->lpszText = pszTip;
			pTI->rect = m_rectMax;
			pTI->uId = HTMAXBUTTON;
			iRet = pTI->uId;
		}
		else if ( m_rectClose.PtInRect(point) )
		{
			TCHAR *pszTip = (TCHAR *)malloc(30);
			_tcscpy(pszTip, _T("退出程序"));
			pTI->uFlags = 0;
			pTI->hwnd = m_hWnd;
			pTI->lpszText = pszTip;
			pTI->rect = m_rectClose;
			pTI->uId = HTCLOSE;
			iRet = pTI->uId;
		}
	}
	else
	{
		return -1;		// 禁止MainFrame的其它控件有tip显示
	}
	return iRet;
}

void CMainFrame::OnDlgSyncExcel()
{
	CIoViewSyncExcel::Instance().UserManageSyncBlock();
}

void CMainFrame::OnMenuIoViewStarry( UINT nID )
{
	// 给指标详情处理的菜单项
	CIoViewBase * pIoViewBase = FindActiveIoView();

	if ( NULL == pIoViewBase )
	{
		return;
	}

	if ( !pIoViewBase->IsKindOf(RUNTIME_CLASS(CIoViewStarry)))
	{
		return;
	}

	pIoViewBase->PostMessage(WM_COMMAND, nID, 0);
}

void CMainFrame::OnMenuUIIoViewStarry( CCmdUI *pCmdUI )
{
	// 给指标详情处理的菜单项
	CIoViewBase * pIoViewBase = FindActiveIoView();

	if ( NULL == pIoViewBase )
	{
		return;
	}

	if ( !pIoViewBase->IsKindOf(RUNTIME_CLASS(CIoViewStarry)))
	{
		return;
	}

	CIoViewStarry *pStarry = (CIoViewStarry *)pIoViewBase;
	pStarry->DoCmdUIUpdate(pCmdUI);
}


void CMainFrame::OnShowTimeSaleDetail()
{
	OnShowF7IoViewInChart(ID_PIC_TIMESALE, 0);
}

bool32 CMainFrame::OnShowF7IoViewInChart( UINT nF7PicId, UINT nChartPicId /*=0*/ )
{
	// 有指定商品没
	CIoViewBase *pIoViewActive = FindActiveIoView();
	CMerch *pMerchXml = NULL;
	if ( NULL != pIoViewActive )
	{
		pMerchXml = pIoViewActive->GetMerchXml();
	}
	CMPIChildFrame *pChildFrame = NULL;
	CMPIChildFrame *pChartFrame = NULL;	// 图表页面
	CIoViewBase *pF7IoView = NULL;
	// 优先于本视图查找指定要F7化视图，如果没有找到，则打开图表视图，再次查找，如在无不处理
	if ( nChartPicId <= 0 )
	{
		pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, MDIGetActive());
		if ( NULL != pChildFrame )
		{
			// 尝试在当前页面找
			pF7IoView = FindIoViewInFrame(nF7PicId, pChildFrame);
		}
	}

	// 新建的页面都是隐藏的，需要显示
	if ( NULL == pF7IoView )
	{
		if ( nChartPicId <= 0 )
		{
			// 打开任意图表视图，尝试调取
			pChartFrame = FindChartIoViewFrame(true, true);
		}
		else
		{
			// 打开指定图表视图，尝试查找
			pChartFrame = FindAimIoViewFrame(nChartPicId, true, true);
		}
		if ( NULL != pChartFrame )
		{
			pF7IoView = FindIoViewInFrame(nF7PicId, pChartFrame);
			if ( NULL == pF7IoView && nChartPicId <= 0 )
			{
				// 当前打开的这个图表视图没有要求的IoView，悲剧~
				ASSERT( 0 );
				// 再次尝试找另外一个图表类型
				CString StrChartXmlName = pChartFrame->GetIdString();
				CCfmManager::E_SystemDefaultCfm eSDC = CCfmManager::Instance().GetESDC(StrChartXmlName);
				if ( eSDC == CCfmManager::ESDC_Trend )
				{
					pChartFrame = FindAimIoViewFrame(ID_PIC_KLINE, true, true);
				}
				else
				{
					pChartFrame = FindAimIoViewFrame(ID_PIC_TREND, true, true);
				}
				if ( NULL != pChartFrame )
				{
					pF7IoView = FindIoViewInFrame(nF7PicId, pChartFrame);
				}
			}
		}

		// 优先的图标窗口没有分笔！！那任意找一个
		if ( NULL == pF7IoView )
		{
			ASSERT( 0 );	// 此时已经处于错误状态，尽量满足用户需求
			pF7IoView = FindIoViewByPicMenuId(nF7PicId, NULL, false, false, false, false, true);
		}
	}

	ASSERT( NULL != pF7IoView );	// 应该找到了，不过也不排除没有设置好页面

	if ( NULL != pF7IoView )
	{
		// 切换商品
		if ( NULL != pMerchXml )
		{
			OnViewMerchChanged(pF7IoView, pMerchXml);
		}

		pF7IoView->BringToTop();	// 置顶

		if ( NULL != pF7IoView->GetIoViewManager() )
		{
			CGGTongView *pView = DYNAMIC_DOWNCAST(CGGTongView, pF7IoView->GetIoViewManager()->GetParent());
			CMPIChildFrame *pF7Frame = DYNAMIC_DOWNCAST(CMPIChildFrame, pF7IoView->GetParentFrame());
			if ( NULL != pF7Frame && pF7Frame->IsF7AutoLock() && pView != pF7Frame->GetF7GGTongView() )
			{
				// 还原
				OnProcessCFF7(pF7Frame);
			}

			if ( NULL != pView && (NULL == pF7Frame || pView != pF7Frame->GetF7GGTongView()) )
			{
				// 最大化
				ASSERT( !pView->GetMaxF7Flag() );
				OnProcessF7(pView);
			}
		}
	}

	if ( NULL != pChartFrame )
	{
		//pChartFrame->ShowWindow(SW_SHOW);	// 如果有创建了图表页面，需要显示它
	}

	// 存储调用链
	CMDIChildWnd *pNowFrame = MDIGetActive();
	if ( NULL != pChildFrame && pNowFrame != pChildFrame )
	{
		AddToEscBackFrame(pChildFrame);

		OnChangeOpenedCfm(nChartPicId);
	}
	return NULL != pF7IoView;
}

void CMainFrame::OnShowMerchInChart( CMerch *pMerch, CIoViewBase *pIoViewSrcOrg )
{
	// 所有视图需要的跳转(含mainframe的hotmsg merch)需要转到这里来进行
	//  1. 同属childframe有chart，则显示该子视图的chart，切换商品，end
	//  2. 无chart，那就只有看是否打开了k 或者 分时的系统页面，如果打开了，则跳转优先分时，后k
	//		都不存在的话，创建分时页面 end
	// 如果发生了页面间跳转，记录上一个源childframe，ESC用
	// 分时或者k线视图的无论分组是否与此相同，都会被跳转到该商品
	if ( NULL == pMerch )
	{
		ASSERT( 0 );
		return;
	}
	if ( NULL == pIoViewSrcOrg )
	{
		pIoViewSrcOrg = FindActiveIoView();
	}

	CIoViewBase *pIoViewSrc = pIoViewSrcOrg;
	CMPIChildFrame *pChildFrame = NULL;

	if ( NULL == pIoViewSrc || NULL == pIoViewSrc->GetIoViewManager() )
	{
		// 无源调用 || 非标准业务视图调用
		// 就是按下了快捷键处理
		pIoViewSrc = FindActiveIoView();	// 找到激活的标准业务视图
		if ( NULL == pIoViewSrc )
		{
			// 无激活视图，则查找分时或者k线页面, 不存在则创建
			pChildFrame = FindChartIoViewFrame(true, true);
			if ( NULL != pChildFrame )
			{
				pIoViewSrc = FindActiveIoViewInFrame(pChildFrame);
			}
		}
		if ( NULL == pIoViewSrc )
		{
			ASSERT( 0 );	// 这都失败了- -
			if ( NULL != pChildFrame )
			{
				//pChildFrame->ShowWindow(SW_SHOW);
			}
			return;
		}
	}
	ASSERT( NULL != pIoViewSrc && NULL != pIoViewSrc->GetIoViewManager() );
	CAbsCenterManager *pAbsCenterManager = pIoViewSrc->GetCenterManager();
	if ( NULL == pAbsCenterManager )
	{
		ASSERT( 0 );
		if ( NULL != pChildFrame )
		{
			//pChildFrame->ShowWindow(SW_SHOW);
		}
		return; 
	}

	// 记录此时的当前frame，以备esc
	CMPIChildFrame *pLastActiveFrame = (CMPIChildFrame *)MDIGetActive();

	// 有了标准业务视图，则看同页面是否有同分组分时或者K线，如果没有的话，则调用分时 K线页面
	// 如果pIoViewSrc就是分时或者K，就不用找了
	// 新的业务需求可能是任何一个单商品视图在该页面存在，都是切换该单商品视图和其分组的商品(通达信貌似这样的)
	CIoViewBase *pChart = NULL;
	pChart = FindChartIoViewInSameFrame(pIoViewSrc);	// 同窗口无结果，则需要新建页面
	if ( NULL == pChart )
	{
		// 变更为如果有任何可见的单商品视图，则显示该单窗口视图？？？
		// 现在对闪电图附加这个补丁
		// 闪电图的补丁去掉
		// pChart = FindIoViewInFrame(ID_PIC_TICK, DYNAMIC_DOWNCAST(CMPIChildFrame, pIoViewSrc->GetParentFrame()), true);
		// 
		// if ( NULL == pChart )
		{
			// 本视图没有， 那打开页面视图吧
			pChildFrame = FindChartIoViewFrame(true, true);
			if ( NULL != pChildFrame )
			{
				CCfmManager::Instance().ChangeIoViewStatus(pChildFrame);

				pIoViewSrc = FindActiveIoViewInFrame(pChildFrame);
				pChart = FindChartIoViewInFrame(pChildFrame);
			}
		}
	}

	CIoViewBase *pIoViewChangeMerch = pChart;
	if ( NULL == pChart )
	{
		// 应当有一个chart图形出来的
		ASSERT( 0 );
		// 与src同组的还是切换商品吧
		pIoViewChangeMerch = pIoViewSrc;	// 没有chart，也要变更找到的被认为是要变更的源的视图的同组商品
	}
	// 此后不应引用pChart
	ASSERT( NULL != pIoViewChangeMerch );
	if ( NULL == pIoViewChangeMerch )
	{
		return;		// 实在是找不到一个合适的了, 没激活也没办法了
	}

	// 有chart了，开始切换
	// chart 视图需要切换，参数源视图如果与其不同分组，也要切换
	OnViewMerchChanged(pIoViewChangeMerch, pMerch);

	if(NULL != pIoViewSrcOrg )
	{
		int32 iSrcOrgGroupID = pIoViewSrcOrg->GetIoViewGroupId();
		int32 iViewChangeGroupID = pIoViewChangeMerch->GetIoViewGroupId();
		if ( pIoViewSrcOrg != pIoViewChangeMerch
			&& !(iSrcOrgGroupID & iViewChangeGroupID )/*pIoViewSrcOrg->GetIoViewGroupId() != pIoViewChangeMerch->GetIoViewGroupId()*/
			)
		{
			OnViewMerchChanged(pIoViewSrcOrg, pMerch);
		}
	}

	pIoViewChangeMerch->BringToTop();		// 最后的视图置顶
	pIoViewChangeMerch->ForceUpdateVisibleIoView();

	if ( NULL != pChildFrame )
	{
		CString StrWspName = pChildFrame->GetIdString();
		CView* pView = pChildFrame->GetActiveView();
		if (pView->IsKindOf(RUNTIME_CLASS(CGGTongView)) && !StrWspName.IsEmpty() )
		{
			CNewCaptionTBWnd::AddPage(StrWspName);
		}
		//pChildFrame->ShowWindow(SW_SHOW);
	}

	// 此时的childframe是否与前记录的不同，如有，则将以前的记录为esc跳转记录
	CMDIChildWnd *pNowActiveFrame = MDIGetActive();
	if ( pNowActiveFrame != pLastActiveFrame && NULL != pLastActiveFrame )
	{
		AddToEscBackFrame(pLastActiveFrame);	// 记录该页面
	}
}

void CMainFrame::OnShowMerchInChart( int32 iMarketId, const CString &StrMerchCode, UINT nPicId/*=0*/ )
{
	CGGTongDoc *pDoc = AfxGetDocument();
	CAbsCenterManager *pAbsCenterManager = NULL != pDoc ? pDoc->m_pAbsCenterManager : NULL;
	if ( NULL == pAbsCenterManager )
	{
		return;
	}

	CMerch *pMerch = NULL;
	if ( !pAbsCenterManager->GetMerchManager().FindMerch(StrMerchCode, iMarketId, pMerch) )
	{
		return;
	}

	if ( 0 == nPicId )
	{
		OnShowMerchInChart(pMerch, NULL);		// 默认规则
		return;
	}

	// 指定chart页面
	CMPIChildFrame *pOldFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, MDIGetActive());
	ASSERT( ID_PIC_KLINE == nPicId || ID_PIC_TREND == nPicId );	// 目前只有这两种页面是支持的
	CMPIChildFrame *pChartFrame = FindAimIoViewFrame(nPicId, true, true);
	CIoViewBase *pIoView = NULL;
	if ( NULL != pChartFrame )
	{
		pIoView = FindIoViewInFrame(nPicId, pChartFrame, false);
	}
	if ( NULL != pIoView )
	{
		OnViewMerchChanged(pIoView, pMerch);
		pIoView->BringToTop();
	}
	else if ( NULL != pChartFrame )
	{
		ASSERT( 0 );
		OnViewMerchChanged(pChartFrame->GetGroupID(), pMerch);
		MDIActivate(pChartFrame);
	}

	if ( NULL != pChartFrame )
	{
		//pChartFrame->ShowWindow(SW_SHOW);	// 如果是新建的话，显示新的页面
	}

	if ( NULL != pOldFrame && pOldFrame != pChartFrame )
	{
		AddToEscBackFrame(pOldFrame);

		OnChangeOpenedCfm(nPicId);
	}
}

void CMainFrame::OnShowMerchInNextChart( CIoViewBase *pIoViewSrcOrg )
{
	if ( NULL == pIoViewSrcOrg )
	{
		// 寻找激活窗口作为源窗口
		pIoViewSrcOrg = FindActiveIoView();
		if ( pIoViewSrcOrg == NULL )
		{
			return;
		}
	}

	CMerch *pMerch = pIoViewSrcOrg->GetMerchXml();
	if ( NULL == pMerch )
	{
		// 无商品不切换
		ASSERT( 0 );
		return;
	}

	CIoViewBase *pIoViewSrc = pIoViewSrcOrg;

	// 规则(必跳转到两种图表页面中的一种，不论分组如何):
	//	当前页面是非图表页面，则跳转到与当前激活窗口同类型的页面(分时或K)，如是其它类型，则分时
	//		上面意思相当于放大
	//	当前页面是图表的一种，则跳到另外一种图表页面中

	CMPIChildFrame *pLastActiveFrame = (CMPIChildFrame *)MDIGetActive();

	// 当前激活的页面是否是图表页面
	CString StrKline, StrTrend;
	CCfmManager::Instance().GetSystemDefaultCfmName(CCfmManager::ESDC_KLine, StrKline);
	CCfmManager::Instance().GetSystemDefaultCfmName(CCfmManager::ESDC_Trend, StrTrend);
	if ( pLastActiveFrame != NULL )
	{
		CString StrId = pLastActiveFrame->GetIdString();
		if ( StrKline == StrId )
		{
			// 跳转到分时
			LoadSystemDefaultCfm(ID_PIC_TREND, pMerch, true);		// 会记录跳转
			return;
		}
		else if ( StrTrend == StrId )
		{
			// 跳转到K线
			LoadSystemDefaultCfm(ID_PIC_KLINE, pMerch, true);
			return;
		}
		// 其它页面 流入下一个流程
	}

	// 当前页面非图表页面
	// 传进来的视图是否为图表视图, 是则创建对应的图表页面, 否则创建分时页面
	if ( !pIoViewSrc->IsKindOf(RUNTIME_CLASS(CIoViewKLine)) )
	{
		// 非k线，则必是要创建分时
		LoadSystemDefaultCfm(ID_PIC_TREND, pMerch, true);
	}
	else
	{
		LoadSystemDefaultCfm(ID_PIC_KLINE, pMerch, true);
	}

	return;	// END
}



void CMainFrame::OnViewSpecialBar()
{
	REBARBANDINFO rbbi = {0};
	rbbi.cbSize = sizeof(rbbi);
	rbbi.fMask = RBBIM_STYLE;
	if ( m_wndReBar.GetReBarCtrl().GetBandInfo(2, &rbbi) )
	{
		m_wndReBar.GetReBarCtrl().ShowBand(2, (rbbi.fStyle & RBBS_HIDDEN) != 0 );
		RecalcLayout();
	}
}

void CMainFrame::OnUpdateViewSpecialBar( CCmdUI *pCmdUI )
{
	REBARBANDINFO rbbi = {0};
	rbbi.cbSize = sizeof(rbbi);
	rbbi.fMask = RBBIM_STYLE;
	if ( m_wndReBar.GetReBarCtrl().GetBandInfo(2, &rbbi) )
	{
		pCmdUI->SetCheck( (rbbi.fStyle & RBBS_HIDDEN) == 0 );
	}
}

void CMainFrame::RecalcLayout( BOOL bNotify /*= TRUE */ )
{
	CNewMDIFrameWnd::RecalcLayout(bNotify);

	//	return;

	if ( (m_wndReBar.m_hWnd != NULL) && (m_pNewWndTB!=NULL))
	{
		// 此时，frame wnd已经recalc layout了 
		CRect rcFrame;
		GetClientRect(rcFrame);
		CRect rcClient;
		m_wndReBar.GetClientRect(rcClient);

		CRect rc;
		if ( ::GetWindowRect(m_pNewWndTB->m_hWnd, &rc) )
		{
			m_wndReBar.ScreenToClient(&rc);
			::MoveWindow(m_pNewWndTB->m_hWnd, rcClient.left, rc.top, rcClient.Width(), rc.Height(), TRUE);
		}
	}

	//if ( (m_wndReBar2.m_hWnd!=NULL) && (m_pNewCaptionWndTB!=NULL) )
	//{
	//	// 此时，frame wnd已经recalc layout了 
	//	CRect rcFrame;
	//	GetClientRect(rcFrame);
	//	CRect rcClient;
	//	m_wndReBar2.GetClientRect(rcClient);

	//	CRect rc;
	//	if ( ::GetWindowRect(m_pNewCaptionWndTB->m_hWnd, &rc) )
	//	{
	//		m_wndReBar2.ScreenToClient(&rc);
	//		::MoveWindow(m_pNewCaptionWndTB->m_hWnd, rcClient.left, rc.top, rcClient.Width(), rc.Height(), TRUE);
	//	}
	//}

	// Rebar中band调整, 减少rebar中预留的header & gripper，不标准，先用
	//if ( m_wndReBar.m_hWnd != NULL )
	//{
	//	// 此时，frame wnd已经recalc layout了 
	//	CRect rcFrame;
	//	GetClientRect(rcFrame);
	//	CRect rcClient;
	//	m_wndReBar.GetClientRect(rcClient);
	//	int iBandCount = m_wndReBar.GetReBarCtrl().GetBandCount();
	//	REBARBANDINFO rbbi = {0};
	//	rbbi.cbSize = sizeof(rbbi);
	//	rbbi.fMask = RBBIM_CHILD;
	//	for ( int i=0; i < iBandCount ; i++ )
	//	{
	//		if ( m_wndReBar.GetReBarCtrl().GetBandInfo(i, &rbbi) && IsWindow(rbbi.hwndChild) )
	//		{
	//			CRect rc;
	//			if ( ::GetWindowRect(rbbi.hwndChild, &rc) )
	//			{
	//				m_wndReBar.ScreenToClient(&rc);
	//				::MoveWindow(rbbi.hwndChild, rcClient.left, rc.top, rcClient.Width(), rc.Height(), TRUE);
	//			}
	//		}
	//	}
	//}
}

void CMainFrame::OnStockSpecialUpdateUI( CCmdUI *pCmdUI )
{
	if ( NULL != pCmdUI )
	{
		// 禁止部分没有权限的按钮
		//pCmdUI->Enable(FALSE);
		pCmdUI->Enable();		// 全部开启，用对话框提示

		pCmdUI->SetCheck(0);
		UINT nId = pCmdUI->m_nID;

		// 根据字串来查询code，通过code查询是否要置灰

		// 根据字串的配置置灰部分按钮
		CString StrMenuItem;
		if ( NULL != pCmdUI->m_pMenu )
		{
			pCmdUI->m_pMenu->GetMenuString(nId, StrMenuItem, MF_BYCOMMAND);
		}
		else if ( NULL != pCmdUI->m_pOther )
		{
			pCmdUI->m_pOther->GetWindowText(StrMenuItem);
		}

		if ( nId >= ID_SPECIAL_INDEX_START && nId <= ID_SPECIAL_INDEX_END )
		{
			// 拐点预测 大盘机构成本 大盘仓位提醒 操盘提醒 个股战略抄底 个股战略逃顶 强弱分界 主图指标或者副图指标
			// 各种指标，判断是否在K线出现
			CString StrIndexName = StrMenuItem;	// 菜单名称就是指标名称
			bool32 bIndex = IsIndexInCurrentKLineFrame(StrIndexName, 0);	// 完全按照指标的设定来

			if ( bIndex )
			{
				pCmdUI->SetCheck(1);
			}
			else
			{
				pCmdUI->SetCheck(0);
			}
		}
		else if ( nId >= ID_SPECIAL_SELECT_START && nId <= ID_SPECIAL_END )
		{
			// 各种选股
			// 各种选股也要按照字串来选择的，TODO
			{
				CStockSelectManager::E_IndexSelectStock eJGLT;
				bool32 b = CStockSelectManager::GetIndexSelectStockEnumByCmdId(nId, eJGLT);
				ASSERT( b );
				const DWORD dwFlag = CStockSelectManager::Instance().GetIndexSelectStockFlag();
				const DWORD dwCur = eJGLT;
				if ( (dwFlag & dwCur) != 0 )
				{
					pCmdUI->SetCheck()	;
				}
			}
		}
		else
		{
			ASSERT( 0 );
		}



	}
}

void CMainFrame::OnStockSpecial( UINT nId )
{
	// 一些参数
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	ASSERT( pApp != NULL );
	CGGTongDoc *pDoc = AfxGetDocument();
	ASSERT( pDoc != NULL );
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	ASSERT( pAbsCenterManager != NULL );
	if (!pAbsCenterManager)
	{
		return;
	}

	CBJSpecialStringImage::Id2StringMap id2Stringmap;
	CBJSpecialStringImage::Instance().GetMenuId2StringMap(id2Stringmap);
	CBJSpecialStringImage::Id2StringMap::iterator it;
	it = id2Stringmap.find(nId);
	CString StrMenuItem;
	if ( it==id2Stringmap.end() )
	{
		ASSERT( 0 );
		return;	// 前面在 Init 特色菜单时没有配置好？
	}
	StrMenuItem = it->second;

	int iFuncRight = CPluginFuncRight::Instance().GetUserRightCode(StrMenuItem);
	if ( iFuncRight >= 0 )
	{
		CPluginFuncRight::E_FuncRight eFunc = (CPluginFuncRight::E_FuncRight)iFuncRight;
		ASSERT( eFunc != CPluginFuncRight::FuncEnd );

		if ( !CPluginFuncRight::Instance().IsUserHasRight(eFunc, true) )
		{
			return;
		}
	}

	if ( nId >= ID_SPECIAL_INDEX_START && nId <= ID_SPECIAL_INDEX_END )
	{
		// 拐点预测 大盘机构成本 大盘仓位提醒 操盘提醒 个股战略抄底 个股战略逃顶 强弱分界 工作区
		// 0914 变更 - // 短 中 --个股战略逃顶 个股战略抄底 强弱分界 当前版面k线主图指标 操盘提醒更换副图指标
		// 指标可以关闭，主图指标变更为ma，副图指标变更为macd
		// 各种指标完全按照名称在K线中出现

		CString StrIndexName = StrMenuItem;
		bool32 bIsOpenIndex = IsIndexInCurrentKLineFrame(StrIndexName);

		// 无论打开指标 or 关闭指标，总是要切到K线
		LoadSystemDefaultCfm(ID_PIC_KLINE);		// 加载K线页面

		CIoViewKLine *pIoKLine = (CIoViewKLine *)GetIoViewByPicMenuIdAndActiveIoView(ID_PIC_KLINE, false, true, false, true, false, true);
		if ( NULL!=pIoKLine )
		{
			if ( !bIsOpenIndex )
			{
				// K线视图会根据指标类型进行预定的切换
				pIoKLine->AddShowIndex(StrIndexName, false, true, true);
			}
			else
			{
				// 替换指定指标
				CFormularContent *pFormula = CFormulaLib::instance()->GetFomular(StrIndexName);
				ASSERT( NULL != pFormula );
				if ( NULL != pFormula )
				{
					if ( CheckFlag(pFormula->flag, CFormularContent::KAllowMain) )
					{
						// 主图
						pIoKLine->ReplaceIndex(StrIndexName, _T("MA"), false);
					}
					else
					{
						pIoKLine->ReplaceIndex(StrIndexName, _T("MACD"), false);
					}
				}
			}
			pIoKLine->BringToTop();
		}
		else
		{
			ASSERT( 0 );	// 没有K线，那没有办法了
		}
	}
	else if ( nId >= ID_SPECIAL_SELECT_START && nId <= ID_SPECIAL_END )
	{
		// 各种选股
		// 按照菜单字串来处理 TODO

		// 主力拉升 主力洗盘 游击涨停 抗跌强庄 超跌反弹 为报价表选股
		CStringArray aNames;
		aNames.Add(_T("智能选股"));
		{
			// 加载指定页面
			LoadSpecialCfm(aNames[0]);
		}
		{
			CStockSelectManager::E_IndexSelectStock eJGLT = CStockSelectManager::StrategyJGLT;
			bool32 b = CStockSelectManager::GetIndexSelectStockEnumByCmdId(nId, eJGLT);
			ASSERT( b );
			DWORD dwNewFlag = eJGLT;
			// 怎么处理这些选股条件呢？互斥，与，并？ TODO

			CStockSelectManager::Instance().SetIndexSelectStockFlag( dwNewFlag );	// 互斥处理
			CStockSelectManager::Instance().SetCurrentReqType(CStockSelectManager::IndexShortMidSelect);
		}
	}
	else
	{
		ASSERT( 0 );
	}
}



//linhc 20100908添加“所属板块”菜单响应函数
void CMainFrame::OnBelongBlockMsg()
{
	CIoViewBase* pIoView = FindActiveIoView();

	if ( NULL != pIoView )
	{  
		CMerch*		  pMerchXml = pIoView->GetMerchXml();

		if ( NULL ==  m_pDlgbelong )
		{
			m_pDlgbelong = new CDlgBelongBlock(pMerchXml);
			m_pDlgbelong->Create(IDD_DIALOG_BELONGBLO,AfxGetMainWnd());
			m_pDlgbelong->CenterWindow();
			m_pDlgbelong->ShowWindow(SW_SHOW);
		}
		else
		{
			ASSERT( IsWindow(m_pDlgbelong->GetSafeHwnd()) );
			m_pDlgbelong->ShowWindow(SW_SHOW);
		}
	}
	else
	{
		//ASSERT(0);
		return;
	}
}

void CMainFrame::OnVedioTrainning()
{
	// 视频培训
}

LRESULT CMainFrame::OnSetWindowText( WPARAM w, LPARAM l )
{
	LPCTSTR pcsz = (LPCTSTR)l;	// 可能为NULL

	m_StrWindowText = pcsz;
	m_bWindowTextValid = true;
	Default();
	return TRUE;
}

LRESULT CMainFrame::OnGetWindowText( WPARAM w, LPARAM l )
{
	if ( m_bWindowTextValid )
	{
		LPTSTR pszBuf = (LPTSTR)l;
		int iLen = (int)w;
		if ( NULL != pszBuf && iLen > 0 )
		{
			_tcsncpy(pszBuf, m_StrWindowText, iLen);
			pszBuf[iLen-1] = _T('\0');
			int32 iTextLen = m_StrWindowText.GetLength();
			return min(iTextLen, iLen-1);
		}
		return 0;
	}
	else
	{
		return Default();
	}
}
LRESULT CMainFrame::OnGetNowUseColor(WPARAM wParam,LPARAM lParam)
{
	COLORREF Clrs[ESCCount] = {0};
	CFaceScheme::Instance()->GetNowUseColors(Clrs);
	COLORREF ClrGap = Clrs[ESCSpliter];
	return ClrGap;
}

LRESULT CMainFrame::OnIsShowVolBuySellColor(WPARAM wParam,LPARAM lParam)
{
	return CIoViewChart::IsShowVolBuySellColor();
}

LRESULT CMainFrame::OnGetStaticMainKlineDrawStyle(WPARAM wParam,LPARAM lParam)
{
	return (LRESULT)CIoViewKLine::GetStaticMainKlineDrawStyle();
}
LRESULT CMainFrame::OnGetIDRMainFram(WPARAM wParam,LPARAM lParam)
{
	return (LRESULT)IDR_MAINFRAME;
}
LRESULT CMainFrame::OnGetCenterManager(WPARAM wParam,LPARAM lParam)
{
	CAbsCenterManager *pAbsCenterManager = AfxGetDocument()->m_pAbsCenterManager;
	return (LRESULT)pAbsCenterManager;
}

LRESULT CMainFrame::OnGetSysFontObject(WPARAM wParam,LPARAM lParam)
{
	CFont * font = CFaceScheme::Instance()->GetSysFontObject ( E_SysFont(wParam));
	return (LRESULT)font;
}
LRESULT CMainFrame::OnGetSysColor(WPARAM wParam,LPARAM lParam)
{
	return CFaceScheme::Instance()->GetSysColor((E_SysColor)wParam);
}
LRESULT CMainFrame::OnTabRename(WPARAM wParam,LPARAM lParam)
{
	CDlgTabName dlg;	
	TCHAR *szNewName = new TCHAR[255];
	memset(szNewName,0,sizeof(TCHAR)*255);
	CString strNewName = (TCHAR*)wParam;
	dlg.SetName((TCHAR*)wParam);
	if ( IDOK == dlg.DoModal())
	{
		strNewName = dlg.GetName();
		lstrcpyn(szNewName,strNewName.GetBuffer(0),255);
	}
	else
	{
		lstrcpyn(szNewName,strNewName.GetBuffer(0),255);
	}
	return (LRESULT)szNewName;
}
LRESULT CMainFrame::OnFindGGTongView(WPARAM wParam,LPARAM lParam)
{
	LRESULT lRes = 0;
	if(wParam == 0 && lParam == 0)
	{
		lRes = (LRESULT)FindGGTongView();
	}
	else
	{
		lRes = (LRESULT)FindGGTongView((CWnd*)wParam,(CGGTongView *)lParam);
	}

	return lRes;
}

LRESULT CMainFrame::OnGetWindowTextLength( WPARAM w, LPARAM l )
{
	if ( m_bWindowTextValid )
	{
		return m_StrWindowText.GetLength();
	}
	else
	{
		return Default();
	}
}

void CMainFrame::ShowHotkeyDlg( MSG *pMsg /*= NULL*/, CWnd *pParent /*= NULL*/, E_HotKeyType eHKT /*= EHKTCount*/ )
{
	CGGTongApp * pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDoc = pApp->m_pDocument;
	ASSERT(NULL != pDoc);

	SetHotkeyTarget(pParent, eHKT);
	ASSERT( IsWindow(m_pKBParent->GetSafeHwnd()) );

	// 创建小键盘输入对话框
	if ( NULL == m_pKBParent || !IsWindow(m_pKBParent->GetSafeHwnd()))	// 非正常窗口!
	{
		m_pKBParent = this;
		m_dwHotKeyTypeFlag = EHKTCount;
		SetForceFixHotkeyTarget(false);
	}

	if ( m_dwHotKeyTypeFlag != EHKTCount )
	{
		// 仅支持特定一个类别，目前并没有支持多个类别
		m_pDlgKeyBoard = new CKeyBoardDlg(m_HotKeyList,m_pKBParent,(E_HotKeyType)m_dwHotKeyTypeFlag);
	}
	else
	{
		m_pDlgKeyBoard = new CKeyBoardDlg(m_HotKeyList,m_pKBParent);
	}
	ASSERT(NULL != m_pDlgKeyBoard);

	// 保存按键消息,传至“按键精灵”对话框中
	if ( NULL != pMsg )
	{
		CopyMemory(&m_pDlgKeyBoard->m_Msg, pMsg, sizeof(MSG)); 
	}

	m_pDlgKeyBoard->Create(IDD_KEYBOARD, m_pKBParent);
	m_pDlgKeyBoard->ShowWindow(SW_SHOW);
}

CMPIChildFrame * CMainFrame::OpenChildFrame( const CString &StrXmlCfmName, bool32 bHideNewCfm/*=false*/ )
{
	// 隐藏要切换商品的页面 - 暂时不应用
	// 太多的地方的优化用到了可见标志，这里不能在初始化时隐藏它
#ifdef _DEBUG
	DWORD dwTime = timeGetTime();
#endif

	m_bFromXml = true;	// 开始进入xml化状态

	//////////////////////////////////////////////////////////////////////////
	bool32 bMax = false;
	CWnd * pChild = CWnd ::FromHandle(m_hWndMDIClient)->GetWindow(GW_CHILD);
	if (pChild)
	{
		CMPIChildFrame * pChilds =(CMPIChildFrame * )GetActiveFrame();

		if (pChilds->GetMaxSizeFlag())
		{
			bMax = true;
		}		
	}

	//////////////////////////////////////////////////////////////////////////
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();

	POSITION   pos  = pApp->m_pDocTemplate->GetFirstDocPosition();

	if ( NULL == pos)
	{
		pApp->m_pDocument = (CGGTongDoc *)pApp->m_pDocTemplate->CreateNewDocument();
		pos  = pApp->m_pDocTemplate->GetFirstDocPosition();
	}


	CRect RectActive = CalcActiveChildSize();

	CMPIChildFrame * pChildFrame = pApp->m_pDocTemplate->CreateMPIFrame(IDR_MAINFRAME);

	// 页面分组: 还是按照页面文件中的分组值，需要确保页面文件的分组值是一致的
	//	 需要修改每个页面文件中具体视图的分组的初始化商品行为, 如果在初始化时
	//	 该分组拥有选择的商品，则不使用页面文件中的商品切换(暂不修改)
	// 使用打开后调用viewdata的merchchanged来替换上面的行为
	// 股大将军等版本的新建窗口，默认分组都属于1

	T_CfmFileInfo cfm;
	if ( CCfmManager::Instance().QueryUserCfm(StrXmlCfmName, cfm) )
	{
		if ( _taccess(cfm.m_StrFilePath, 0) != 0 )
		{
			// 不存在这个文件，只有替换这次打开操作了
			ASSERT( 0 );
			T_CfmFileInfo cfmSys;
			if ( cfm.m_bSystemDirHas
				&& CCfmManager::Instance().QuerySysCfm(StrXmlCfmName, cfmSys) )
			{
				cfm.m_StrFilePath = cfmSys.m_StrFilePath;
				ASSERT( _taccess(cfm.m_StrFilePath, 0) == 0 );	// 已经是最后一次机会了
			}
		}
	}

	// 获取这个时候每个group对应的merch
	typedef map<int32, CMerch *>	GroupMerchMap;
	GroupMerchMap groupMerchMap, groupMerchMapMulti, groupMerchMapOther;
	{
		// 暂时用这种方法初始化吧
		for ( int32 i=0; i < m_IoViewsPtr.GetSize() ; i++ )
		{
			CIoViewBase *pIoView = m_IoViewsPtr[i];
			if ( IsWindow(pIoView->GetSafeHwnd()) )
			{
				int32 iGroupId = pIoView->GetIoViewGroupId();
				if ( iGroupId > 0 )
				{
					CMerch *pMerch = pIoView->GetMerchXml();
					if ( NULL != pMerch && pIoView->GetIoViewType() == CIoViewBase::EIVT_SingleMerch )
					{	
						groupMerchMap[ iGroupId ] = pMerch;
					}
					else if ( NULL != pIoView->m_pMerchXml )
					{
						groupMerchMapMulti[ iGroupId ] = pIoView->m_pMerchXml;
					}
					else
					{
						groupMerchMapOther[ iGroupId ] = pMerch;
					}
				}
			}
		}

		// 当前激活窗口覆盖除了单视窗外的其它数据
		CIoViewBase *pIoViewActive = FindActiveIoView();
		if ( NULL != pIoViewActive && pIoViewActive->GetIoViewGroupId() > 0 )
		{
			groupMerchMapMulti[pIoViewActive->GetIoViewGroupId()] = pIoViewActive->GetMerchXml();
		}

		GroupMerchMap::iterator it;
		// 先处理multi的
		for (  it=groupMerchMapMulti.begin(); it != groupMerchMapMulti.end() ; ++it )
		{
			if ( groupMerchMap.count(it->first) <= 0 )
			{
				groupMerchMap[it->first] = it->second;
			}
		}

		// 在处理剩下
		for ( it=groupMerchMapOther.begin(); it != groupMerchMapOther.end() ; ++it )
		{
			if ( groupMerchMap.count(it->first) <= 0 )
			{
				groupMerchMap[it->first] = it->second;
			}
		}
	}

	// 初始化xml
	TiXmlElement *pChildEle = NULL;
	TiXmlDocument myDocument;
	if ( !cfm.m_StrFilePath.IsEmpty() )
	{
		if ( LoadTiXmlDoc(myDocument, cfm.m_StrFilePath) )
		{
			TiXmlElement *  pRootElement = myDocument.RootElement();
			if ( NULL != pRootElement )
			{
				CString StrAttridata = pRootElement->Attribute(GetXmlRootElementAttrData());
				if ( StrAttridata.CompareNoCase(_T("ChildFrame")) == 0 )
				{
					pChildEle = pRootElement->FirstChildElement();
				}
			}
		}
	}
	if ( NULL == pChildEle )
	{
		// 使用默认
		CString StrDefault = pChildFrame->GetDefaultXML(true, RectActive);

		//USES_CONVERSION;
		//const char * KStrDefault = W2A(StrDefault);

		//fangz0714No.3
		char KStrDefault[10240];
		UNICODE_2_MULTICHAR(EMCCUtf8,StrDefault,KStrDefault);
		myDocument.Parse(KStrDefault);
		pChildEle = myDocument.FirstChildElement();
	}
	ASSERT( NULL != pChildEle );

	// 如果要隐藏，则必须锁定其绘制更新
	//if ( bHideNewCfm )
	{
		// 是否所有工作页面都可以切换到这种模式执行打开操作
		pChildFrame->SetHideMode(true);
	}

	pChildFrame->EnableRequestData(false);	// 禁止从xml中加载时发送数据请求

	pChildFrame->FromXml(pChildEle);
	m_bFromXml = false;	// 离开xml化
#ifdef _DEBUG
	TRACE(_T("打开页面完成xml[%s]: %d ms\r\n"), StrXmlCfmName.operator LPCTSTR(), timeGetTime()-dwTime);
#endif
	pChildFrame->SetActiveGGTongView(GetActiveGGTongViewXml());
	pChildFrame->SetChildFrameTitle(AfxGetApp()->m_pszAppName);

    CCfmManager::Instance().ChangeIoViewStatus(pChildFrame);

	// 版面之间不联动，就不用先前的数据了 modify by tangad
	// 根据FromXml前的列表再次merchchanged, 还原以前的分组商品
	/*{
		for (GroupMerchMap::const_iterator it=groupMerchMap.begin(); it != groupMerchMap.end() ; it++)
		{
			if ( it->second != NULL )
			{
				OnViewMerchChanged(it->first, it->second);
			}
		}
	}*/

	//  
	{
		// 打开的子窗口尽量保持最大化
		// 		if(bMax && !pChildFrame->GetMaxSizeFlag())
		// 		{
		// 			pChildFrame->PostMessage(KMsgChildFrameMaxSize,0,0);
		// 		}
		// 打开的子窗口全部最大化
		pChildFrame->ShowWindow(SW_MAXIMIZE);
	}

	//if ( bHideNewCfm )
	{
		pChildFrame->SetHideMode(false);	// 离开隐藏模式
		PostMessage(UM_CHECK_CFM_HIDE, (WPARAM)pChildFrame->GetSafeHwnd(), NULL);	// 稍候检查它
	}

	// 以后可以发送请求了，由于实际通过IoViewBase响应这个暗示，而ioview是通过timer来开始下一次发送数据
	// 所以实际到下一次具体请求或者下一次timer消息来时，才能引发数据的重发
	// 会不会导致部分请求发送，部分请求没发送呢？
	pChildFrame->EnableRequestData(true);	// 以后可以发请求了
	pChildFrame->RecalcLayoutAsync();		// 要求重新计算区域

#ifdef _DEBUG
	TRACE(_T("打开页面[%s]共: %d ms\r\n"), StrXmlCfmName.operator LPCTSTR(), timeGetTime()-dwTime);
#endif


	return pChildFrame;
}

void CMainFrame::OnUpdateSaveChildFrame( CCmdUI* pCmdUI )
{
	CMPIChildFrame *pChildFrame = (CMPIChildFrame *)MDIGetActive();
	pCmdUI->Enable(FALSE);
	if ( NULL != pChildFrame )
	{
		pCmdUI->Enable(TRUE);	
	}
}

void CMainFrame::OnSaveChildFrame()
{
	CMPIChildFrame *pChildFrame = (CMPIChildFrame *)MDIGetActive();
	if ( NULL != pChildFrame )
	{
		CString StrId = pChildFrame->GetIdString();
		T_CfmFileInfo cfm;
		if ( CCfmManager::Instance().QueryUserCfm(StrId, cfm) )
		{
			CCfmManager::Instance().SaveCurrentCfm();
		}
		else
		{
			OnSaveasChildframe();
		}
	}
}

void CMainFrame::OnUpdateLockChildFrame( CCmdUI* pCmdUI )
{
	return;
	CMPIChildFrame *pChildFrame = (CMPIChildFrame *)MDIGetActive();
	pCmdUI->Enable(FALSE);
	if ( NULL != pChildFrame )
	{
		CString StrId = pChildFrame->GetIdString();
		T_CfmFileInfo cfm;
		BOOL bEnableLock = TRUE;
		if ( CCfmManager::Instance().QueryUserCfm(StrId, cfm) && cfm.m_bLockSplit )
		{
			bEnableLock = FALSE;	// 强制禁止改变lock状态
		}
		if ( pChildFrame->IsF7AutoLock() )
		{
			bEnableLock = FALSE;	// F7不允许改变
		}

		if ( pChildFrame->IsLockedSplit() )
		{
			pCmdUI->SetText(_T("开始页面设置"));
		}
		else
		{
			pCmdUI->SetText(_T("锁定页面设置"));
		}

		pCmdUI->Enable(bEnableLock);
	}
}

void CMainFrame::OnLockChildFrame()
{
	CMPIChildFrame *pChildFrame = (CMPIChildFrame *)MDIGetActive();
	if ( NULL != pChildFrame )
	{
		CString StrId = pChildFrame->GetIdString();
		T_CfmFileInfo cfm;
		//		BOOL bEnableLock = TRUE;
		if ( CCfmManager::Instance().QueryUserCfm(StrId, cfm) && cfm.m_bLockSplit )
		{
			ASSERT( 0 );
			return;
		}

		pChildFrame->SetLockSplit(!pChildFrame->IsLockedSplit());
	}
}

void CMainFrame::OnUpdateViewSplitCmd( CCmdUI *pCmdUI )
{
	return;
	// 如果锁定了childframe，则不能进行分割操作
	pCmdUI->Enable(FALSE);
	CMPIChildFrame *pChildFrame = (CMPIChildFrame *)MDIGetActive();
	if ( NULL != pChildFrame )
	{
		if ( !pChildFrame->IsLockedSplit() )
		{
			pCmdUI->Enable(TRUE);
		}

	}
}

CMPIChildFrame * CMainFrame::FindChartIoViewFrame( bool32 bCreateIfNotExist /*= false*/, bool32 bHideNewCfm/*=false*/ )
{
	// 查找分时 k线 页面，如果有任意一个(分时优先)，返回
	// 如果找不到，设定可以创建，则创建一个分时窗口

	// 按照Z值，上面的优先，如果都不存在则分时优先
	CString StrTrend, StrKline;
	CCfmManager::Instance().GetSystemDefaultCfmName(CCfmManager::ESDC_Trend, StrTrend);
	CCfmManager::Instance().GetSystemDefaultCfmName(CCfmManager::ESDC_KLine, StrKline);
	CWnd *pWnd = CWnd ::FromHandle(m_hWndMDIClient)->GetWindow(GW_CHILD); 

	while ( NULL != pWnd )   
	{ 
		CWnd* pTmp		= pWnd;
		CWnd* pTmpNext	= pTmp->GetWindow(GW_HWNDNEXT);

		if ( pWnd->IsKindOf(RUNTIME_CLASS(CMPIChildFrame)) )
		{
			CMPIChildFrame *pChild = DYNAMIC_DOWNCAST(CMPIChildFrame, pWnd);
			CString StrId = pChild->GetIdString();
			if ( !StrId.IsEmpty() &&
				( StrId == StrTrend || StrId == StrKline )
				)
			{
				return pChild;	// 找到已经存在的了，返回
			}
		}
		pWnd = pTmpNext;
	}

	// 效率低点哈:)
	CMPIChildFrame *pChildFrame = NULL;

	if ( bCreateIfNotExist )
	{
		pChildFrame = FindAimIoViewFrame(ID_PIC_TREND, true, bHideNewCfm);
		return pChildFrame;
	}
	return NULL;
}

CMPIChildFrame * CMainFrame::FindAimIoViewFrame( int32 iPicIdToFind, bool32 bCreateIfNotExist /*= false*/, bool32 bHideNewFrame/*=false*/ )
{
	CCfmManager::E_SystemDefaultCfm esdc = CCfmManager::ESDC_Count;
	switch (iPicIdToFind)
	{
	case ID_PIC_TREND:
		esdc = CCfmManager::ESDC_Trend;
		break;
	case ID_PIC_KLINE:
		esdc = CCfmManager::ESDC_KLine;
		break;
	case ID_PIC_REPORT:
		esdc = CCfmManager::ESDC_Report;
		break;
	case ID_PIC_NEWS:
		esdc = CCfmManager::ESDC_News;
		break;
	case ID_PIC_PHASESORT:
		esdc = CCfmManager::ESDC_PhaseSort;
		break;
	case ID_PIC_KLINEARBITRAGE:
		esdc = CCfmManager::ESDC_KlineArbitrage;
		break;
	case ID_PIC_TRENDARBITRAGE:
		esdc = CCfmManager::ESDC_TrendArbitrage;
		break;
	case ID_PIC_REPORT_ARBITRAGE:
		esdc = CCfmManager::ESDC_ReportArbitrage;
		break;
	default:
		ASSERT( 0 );		// 仅支持4+1种
		return NULL;
	}

	ASSERT( esdc != CCfmManager::ESDC_Count );
	CString StrCfmName;
	if ( !CCfmManager::GetSystemDefaultCfmName(esdc, StrCfmName) )
	{
		ASSERT( 0 );
		return NULL;
	}

	// 看是否有该名称页面打开
	CMPIChildFrame *pChildFrame = CCfmManager::Instance().GetCfmFrame(StrCfmName);
	if ( NULL != pChildFrame
		|| !bCreateIfNotExist )
	{
		// 已经打开 或者 不需要创建
		if(!StrCfmName.IsEmpty())
		{
			CNewCaptionTBWnd::AddPage(StrCfmName);
		}
		return pChildFrame;
	}

	ASSERT( NULL==pChildFrame && bCreateIfNotExist );		// 需要创建的情况
	if ( (pChildFrame = CCfmManager::Instance().LoadCfm(StrCfmName, false, bHideNewFrame)) != NULL )
	{
		return pChildFrame;
	}

	// 看是否存在该页面的配置，如果存在，则证明可能是不能加载或者权限不够引起的
	// 不考虑文件不存在的情况
	T_CfmFileInfo cfm;
	if ( CCfmManager::Instance().QueryUserCfm(StrCfmName, cfm) 
		&& cfm.m_bNeedServerAuth
		&& !CPluginFuncRight::Instance().IsUserHasRight(StrCfmName, false) )
	{
		return NULL;	// 权限不够，没办法
	}

	// 加载也失败了，可能无页面的相关设置
	// 尝试按照理解建立一个空白的页面，不至于很难看
	pChildFrame = CreateNewIoViewFrame(StrCfmName, iPicIdToFind, true);

	return pChildFrame;
}

CIoViewBase		* CMainFrame::FindChartIoViewInSameFrame( CIoViewBase *pIoViewInSameFrameSrc )
{
	CMPIChildFrame *pChildFrame = NULL;
	if ( NULL == pIoViewInSameFrameSrc || NULL==(pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, pIoViewInSameFrameSrc->GetParentFrame())) )
	{
		ASSERT( 0 );
		return NULL;
	}

	// 先看本身视图是不是符合要求的
	// 在找同窗同组的可见的(先分时，后K)
	// 找同窗同组不可见的
	// end

	// 不注重效率了
	CIoViewBase *pChart = NULL;
	//pChart = DYNAMIC_DOWNCAST(CIoViewTrend, pIoViewInSameFrameSrc);
	pChart = pIoViewInSameFrameSrc->GetRuntimeClass() == RUNTIME_CLASS(CIoViewTrend) ? (CIoViewTrend *)pIoViewInSameFrameSrc : NULL;
	if ( NULL != pChart )
	{
		return pChart;
	}
	//pChart = DYNAMIC_DOWNCAST(CIoViewKLine, pIoViewInSameFrameSrc);
	pChart = pIoViewInSameFrameSrc->GetRuntimeClass() == RUNTIME_CLASS(CIoViewKLine) ? (CIoViewKLine *)pIoViewInSameFrameSrc : NULL;
	if ( NULL != pChart )
	{
		return pChart;
	}

	return FindChartIoViewInFrame(pChildFrame, pIoViewInSameFrameSrc);
}

void CMainFrame::GetFrameSortedChartIoViews( OUT CArray<CIoViewBase *, CIoViewBase *> &aIoViews, CMPIChildFrame *pMustInFrame, int32 iGroupId/*=-1*/ )
{
	aIoViews.RemoveAll();
	if ( NULL==pMustInFrame )
	{
		pMustInFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, MDIGetActive());
		if ( NULL == pMustInFrame )
		{
			return;
		}
	}

	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDoc = (CGGTongDoc *)pApp->m_pDocument;
	ASSERT( NULL != pDoc );
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	ASSERT( NULL != pAbsCenterManager );
	if (!pAbsCenterManager)
	{
		return;
	}

	// 顺序:
	//		同组可见分时(iGroupId!=-1)
	//		同组可见K线(iGroupId!=-1)
	//		同组不可见分时(iGroupId!=-1)
	//		同组不可见K线(iGroupId!=-1)
	//		可见分时
	//		可见K线
	//		不可见分时
	//		不可见k线
	typedef	vector<CIoViewBase *>	IoViewArray;
	typedef vector<IoViewArray >	IoViewArrayArray;
	typedef map<CIoViewBase *, int>	IoViewMap;
	IoViewMap			mapIoViewsTrend, mapIoViewsKline;
	{
		for ( int i=0; i < m_IoViewsPtr.GetSize() ; i++ )
		{
			CIoViewBase *pIoView = m_IoViewsPtr[i];


			if ( !IsWindow(pIoView->GetSafeHwnd()) )
			{
				ASSERT( 0 );
				continue;
			}

			if ( pIoView->GetParentFrame() != pMustInFrame )
			{
				continue;
			}

			// 同窗口下的
			if ( pIoView->IsKindOf(RUNTIME_CLASS(CIoViewTrend)) )
			{
				mapIoViewsTrend[pIoView] = 1;
			}
			else if ( pIoView->IsKindOf(RUNTIME_CLASS(CIoViewKLine)) )
			{
				mapIoViewsKline[pIoView] = 1;
			}
		}
	}

	if ( -1 != iGroupId )
	{
		IoViewMap::iterator it;
		IoViewMap mapTmp = mapIoViewsTrend;
		for ( it = mapTmp.begin() ; it != mapTmp.end() ; it++ )
		{
			CIoViewBase *pIoView = it->first;

			int32 iViewGroupID = pIoView->GetIoViewGroupId();
			if ( (iViewGroupID & iGroupId)/*pIoView->GetIoViewGroupId() == iGroupId*/ && pIoView->IsWindowVisible() )
			{
				aIoViews.Add(pIoView);
				mapIoViewsTrend.erase(pIoView);
			}
		}

		mapTmp = mapIoViewsKline;
		for ( it = mapTmp.begin() ; it != mapTmp.end() ; it++ )
		{
			CIoViewBase *pIoView = it->first;

			int32 iViewGroupID = pIoView->GetIoViewGroupId();
			if ( (iViewGroupID & iGroupId)/*pIoView->GetIoViewGroupId() == iGroupId*/ && pIoView->IsWindowVisible() )
			{
				aIoViews.Add(pIoView);
				mapIoViewsKline.erase(pIoView);
			}
		}

		mapTmp = mapIoViewsTrend;
		for ( it = mapTmp.begin() ; it != mapTmp.end() ; it++ )
		{
			CIoViewBase *pIoView = it->first;

			int32 iViewGroupID = pIoView->GetIoViewGroupId();
			if ( (iViewGroupID & iGroupId)/*pIoView->GetIoViewGroupId() == iGroupId*/ )
			{
				aIoViews.Add(pIoView);
				mapIoViewsTrend.erase(pIoView);
			}
		}

		mapTmp = mapIoViewsKline;
		for ( it = mapTmp.begin() ; it != mapTmp.end() ; it++ )
		{
			CIoViewBase *pIoView = it->first;
			int32 iViewGroupID = pIoView->GetIoViewGroupId();
			if ( (iViewGroupID & iGroupId)/*pIoView->GetIoViewGroupId() == iGroupId*/ )
			{
				aIoViews.Add(pIoView);
				mapIoViewsKline.erase(pIoView);
			}
		}
	}

	// 其它分组的不查找
	return;

	// 其它分组的
	{
		IoViewMap::iterator it;
		IoViewMap mapTmp = mapIoViewsTrend;
		for (  it = mapTmp.begin() ; it != mapTmp.end() ; it++ )
		{
			CIoViewBase *pIoView = it->first;

			if ( pIoView->IsWindowVisible() )
			{
				aIoViews.Add(pIoView);
				mapIoViewsTrend.erase(pIoView);
			}
		}

		mapTmp = mapIoViewsKline;
		for ( it = mapTmp.begin() ; it != mapTmp.end() ; it++ )
		{
			CIoViewBase *pIoView = it->first;

			if ( pIoView->IsWindowVisible() )
			{
				aIoViews.Add(pIoView);
				mapIoViewsKline.erase(pIoView);
			}
		}

		mapTmp = mapIoViewsTrend;
		for ( it = mapTmp.begin() ; it != mapTmp.end() ; it++ )
		{
			CIoViewBase *pIoView = it->first;

			aIoViews.Add(pIoView);
		}
		mapIoViewsTrend.clear();

		mapTmp = mapIoViewsKline;
		for ( it = mapTmp.begin() ; it != mapTmp.end() ; it++ )
		{
			CIoViewBase *pIoView = it->first;

			aIoViews.Add(pIoView);
			mapIoViewsKline.erase(pIoView);
		}
		mapIoViewsKline.clear();
	}
}

CIoViewBase		* CMainFrame::FindChartIoViewInFrame( CMPIChildFrame *pChildFrame, CIoViewBase *pIoViewSrc/*=NULL*/ )
{
	// 先看本身视图是不是符合要求的
	// 在找同窗同组的可见的(先分时，后K)
	// 找同窗同组不可见的
	// end
	if ( NULL == pChildFrame )
	{
		pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, MDIGetActive());
		if ( NULL == pChildFrame )
		{
			return NULL;	// 无页面
		}
	}

	int32 iGroupId = -1;
	if ( NULL != pIoViewSrc )
	{
		iGroupId = pIoViewSrc->GetIoViewGroupId();
	}

	CArray<CIoViewBase *, CIoViewBase *> aIoViewCharts;
	GetFrameSortedChartIoViews(aIoViewCharts, pChildFrame, iGroupId);
	if ( aIoViewCharts.GetSize() > 0 )
	{
		return aIoViewCharts[0];
	}
	return NULL;
}

CIoViewBase		* CMainFrame::FindActiveIoViewInFrame( CMPIChildFrame *pChildFrame )
{
	if ( NULL == pChildFrame )
	{
		pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, MDIGetActive());
		if ( NULL == pChildFrame )
		{
			return NULL;
		}
	}

	CGGTongView *pView = DYNAMIC_DOWNCAST(CGGTongView, pChildFrame->GetActiveView());
	if ( NULL != pView )
	{
		return pView->m_IoViewManager.GetActiveIoView();
	}
	return NULL;
}

bool32 CMainFrame::AddToEscBackFrame( CMPIChildFrame *pChildFrame )
{
	// 是否考虑在cf非激活时，自动加入列表？
	if ( NULL == pChildFrame )
	{
		return false;
	}

	// 两个系统默认的图表页面不记入跳转列表
	CString StrTrend, StrKline;
	CCfmManager::Instance().GetSystemDefaultCfmName(CCfmManager::ESDC_Trend, StrTrend);
	CCfmManager::Instance().GetSystemDefaultCfmName(CCfmManager::ESDC_KLine, StrKline);
	CString StrId = pChildFrame->GetIdString();
	if ( StrId == StrTrend || StrId == StrKline )
	{
		return false;
	}

	if ( m_aCfmsReturn.GetSize() == 0 || m_aCfmsReturn[m_aCfmsReturn.GetSize()-1] != pChildFrame->GetSafeHwnd() )
	{
		m_aCfmsReturn.Add(pChildFrame->GetSafeHwnd());	// 仅记录不相同
	}

	return true;
}


void CMainFrame::OnEscBackFrame()
{
	OnBackToUpCfm();
	return;

	CCfmManager::Instance().ReMoveAllOpendCfm();
	// 是否应当关闭现激活页面, 以避免太多的窗口消耗资源
	CMDIChildWnd *pOld = MDIGetActive();
	HWND hWndNow = pOld->GetSafeHwnd();

	// ESC关闭： 非默认，非图表的有cfm的页面
	T_CfmFileInfo cfm;
	CString StrId;
	if ( pOld != NULL )
	{
		CMPIChildFrame *pCF = DYNAMIC_DOWNCAST(CMPIChildFrame, pOld);
		if ( NULL != pCF )
		{
			StrId = pCF->GetIdString();
			if ( !StrId.IsEmpty() && CCfmManager::Instance().QueryUserCfm(StrId, cfm) )
			{
				if ( CCfmManager::Instance().IsReserveCfm(StrId) )
				{
					StrId.Empty();	// 保留页面不关闭
				}
			}
			else
			{
				StrId.Empty();
			}
		}
	}

	bool32	bDone = false;

	while ( m_aCfmsReturn.GetSize() > 0 )
	{
		HWND hWnd = m_aCfmsReturn[ m_aCfmsReturn.GetSize()-1 ];
		m_aCfmsReturn.RemoveAt( m_aCfmsReturn.GetSize()-1 );
		CWnd *pWnd = CWnd::FromHandlePermanent(hWnd);
		if ( IsWindow(hWnd) && NULL != pWnd && hWnd != hWndNow )	// 与当前页面相同则跳转到不同的页面
		{
			pWnd->BringWindowToTop();
			bDone = true;
			break;
		}
	}

	if ( !bDone )
	{
		// 默认页面
		CCfmManager::Instance().LoadCfm(CCfmManager::Instance().GetUserDefaultCfm().m_StrXmlName);
	}

	CMDIChildWnd *pCur = MDIGetActive();
	if ( pCur != pOld
		&& NULL != pOld )
	{
		// 页面已经提到最前面来了,
		// 如果以前的页面是图表页面, 则现在的页面中的视图调用EscBackFrame接口
		CMPIChildFrame *pCurMPI = DYNAMIC_DOWNCAST(CMPIChildFrame, pCur);
		CMPIChildFrame *pOldMPI = DYNAMIC_DOWNCAST(CMPIChildFrame, pOld);
		if ( NULL != pCurMPI
			&& NULL != pOldMPI )
		{
			CMerch *pMerch = NULL;
			CIoViewBase *pIoView = FindChartIoViewInFrame(pOldMPI);
			if ( NULL != pIoView )
			{
				pMerch = pIoView->GetMerchXml();
			}
			if ( NULL != pMerch )
			{
				DoIoViewEscBackFrameInActiveFrame(pCurMPI, pMerch);
			}
		}
	}

	// 关闭esc按下时的页面，现在已经到后台的窗口
	if ( !StrId.IsEmpty() && NULL != pOld && pOld != pCur )
	{
		CMPIChildFrame *pMPIFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, pOld);
		if ( !pMPIFrame->GetIdString().IsEmpty() )
		{
			CCfmManager::Instance().SaveCfm(pMPIFrame->GetIdString(), pMPIFrame);	// 自动保存页面
		}
		pOld->PostMessage(WM_CLOSE, 0, 0);
	}
}

void CMainFrame::OnBackToHome()
{
	//CCfmManager::Instance().ReMoveAllOpendCfm();
	// 是否应当关闭现激活页面, 以避免太多的窗口消耗资源
	CGGTongView* pActiveGGTongView  = (CGGTongView*)FindGGTongView(); 

	if ( NULL != pActiveGGTongView && pActiveGGTongView->GetMaxF7Flag() )
	{
		OnProcessF7(pActiveGGTongView);
	}

	CCfmManager::Instance().LoadCfm(CCfmManager::Instance().GetUserDefaultCfm().m_StrXmlName);
	return;

	CMDIChildWnd *pOld = MDIGetActive();
	//	HWND hWndNow = pOld->GetSafeHwnd();

	// 关闭： 非默认，非图表的有cfm的页面
	T_CfmFileInfo cfm;
	CString StrId;
	if ( pOld != NULL )
	{
		CMPIChildFrame *pCF = DYNAMIC_DOWNCAST(CMPIChildFrame, pOld);
		if ( NULL != pCF )
		{
			StrId = pCF->GetIdString();
			if ( !StrId.IsEmpty() && CCfmManager::Instance().QueryUserCfm(StrId, cfm) )
			{
				if ( CCfmManager::Instance().IsReserveCfm(StrId) )
				{
					StrId.Empty();	// 保留页面不关闭
				}
			}
			else
			{
				StrId.Empty();
			}
		}
	}

	// 默认页面
	CCfmManager::Instance().LoadCfm(CCfmManager::Instance().GetUserDefaultCfm().m_StrXmlName);

	CMDIChildWnd *pCur = MDIGetActive();
	if ( pCur != pOld
		&& NULL != pOld )
	{
		// 页面已经提到最前面来了,
		// 如果以前的页面是图表页面, 则现在的页面中的视图调用EscBackFrame接口
		CMPIChildFrame *pCurMPI = DYNAMIC_DOWNCAST(CMPIChildFrame, pCur);
		CMPIChildFrame *pOldMPI = DYNAMIC_DOWNCAST(CMPIChildFrame, pOld);
		if ( NULL != pCurMPI
			&& NULL != pOldMPI )
		{
			CMerch *pMerch = NULL;
			CIoViewBase *pIoView = FindChartIoViewInFrame(pOldMPI);
			if ( NULL != pIoView )
			{
				pMerch = pIoView->GetMerchXml();
			}
			if ( NULL != pMerch )
			{
				DoIoViewEscBackFrameInActiveFrame(pCurMPI, pMerch);
			}

			CGGTongView* pActiveGGTongView  = (CGGTongView*)FindGGTongView(); 
			if ( NULL != pActiveGGTongView && pActiveGGTongView->GetMaxF7Flag() )
			{
				OnProcessF7(pActiveGGTongView);
			}
		}
	}

	// 关闭esc按下时的页面，现在已经到后台的窗口
	if ( !StrId.IsEmpty() && NULL != pOld && pOld != pCur )
	{
		CMPIChildFrame *pMPIFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, pOld);
		if ( !pMPIFrame->GetIdString().IsEmpty() )
		{
			CCfmManager::Instance().SaveCfm(pMPIFrame->GetIdString(), pMPIFrame);	// 自动保存页面
		}
		pOld->PostMessage(WM_CLOSE, 0, 0);
	}

	CIoViewReport *pReport = FindIoViewReport(true);
	// by tangaidong 避免根据版面设置，把对应市场加载进去
	/*if (pReport)
	{
	pReport->SetTab(0);
	}*/
}

void CMainFrame::OnBackToUpCfm()
{
	CCfmManager::CfmInfoArray aStrOpenCfm;
	CCfmManager::Instance().GetOpenedCfm(aStrOpenCfm);
	int32 iSize = aStrOpenCfm.GetSize();
	if (1 < iSize)
	{
		OnCloseAllNotReserveCfm();
		CCfmManager::Instance().SetOpenedCfm();

		CCfmManager::Instance().GetOpenedCfm(aStrOpenCfm);
		iSize = aStrOpenCfm.GetSize();
		CCfmManager::Instance().LoadCfm(aStrOpenCfm[iSize-1].StrCfmName, false, false, aStrOpenCfm[iSize-1].StrCfmUrl, true);

		iSize = aStrOpenCfm.GetSize();
		if(L"网页资讯" == aStrOpenCfm[iSize - 1].StrCfmName)
		{
			CIoViewNews *pIoViewNews = FindIoViewNews(true);
			pIoViewNews->RefreshWeb(aStrOpenCfm[iSize - 1].StrCfmUrl);
		}


		if (m_pNewWndTB )
		{
			m_pNewWndTB->SetChildCheckStatusByName(aStrOpenCfm[iSize - 1].StrCfmName);
		}
	}
}

void CMainFrame::OnCloseAllNotReserveCfm()
{
	CWnd *pWnd = CWnd ::FromHandle(m_hWndMDIClient)->GetWindow(GW_CHILD); 
	while ( NULL != pWnd )   
	{ 
		CWnd* pTmp		= pWnd;
		CWnd* pTmpNext	= pTmp->GetWindow(GW_HWNDNEXT);

		CMPIChildFrame *pMPIFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, pWnd);
		if (!CCfmManager::Instance().IsReserveCfm(pMPIFrame->GetIdString()))
		{
			CCfmManager::Instance().SaveCfm(pMPIFrame->GetIdString(), pMPIFrame);	// 自动保存页面
			pWnd->SendMessage(WM_CLOSE,0,0);
		}

		pWnd = pTmpNext;
	}	
}

void CMainFrame::OnChangeOpenedCfm(int32 iPicId)
{
	CString StrCfm = _T("");

	switch (iPicId)
	{
	case ID_PIC_KLINE:
		{
			CCfmManager::Instance().GetSystemDefaultCfmName(CCfmManager::ESDC_KLine, StrCfm);
			break;
		}
	case ID_PIC_TREND:
		{
			CCfmManager::Instance().GetSystemDefaultCfmName(CCfmManager::ESDC_Trend, StrCfm);
			break;
		}
	case ID_PIC_REPORT:
		{
			CCfmManager::Instance().GetSystemDefaultCfmName(CCfmManager::ESDC_Report, StrCfm);
			break;
		}
	default:
		break;
	}

	// 不等于默认页面才加入到返回列表中
	CString StrDefCfm = CCfmManager::Instance().GetUserDefaultCfm().m_StrXmlName;
	if (StrDefCfm != StrCfm)
	{
		CCfmManager::Instance().ChangeOpenedCfm(StrCfm);
	}
}

void CMainFrame::OnCloseAllWindow()
{
	// fangz 0527,关闭一个或者主窗口时可能BUG;
	CWnd *pWnd = CWnd ::FromHandle(m_hWndMDIClient)->GetWindow(GW_CHILD); 

	CMPIChildFrame *pDefaultFrame = CCfmManager::Instance().GetUserDefaultCfmFrame();

	while ( NULL != pWnd )   
	{ 
		CWnd* pTmp		= pWnd;
		CWnd* pTmpNext	= pTmp->GetWindow(GW_HWNDNEXT);

		// 默认页面窗口不关闭
		if ( pWnd != pDefaultFrame )
		{
			CMPIChildFrame *pMPIFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, pWnd);
			if ( !pMPIFrame->GetIdString().IsEmpty() )
			{
				CCfmManager::Instance().SaveCfm(pMPIFrame->GetIdString(), pMPIFrame);	// 自动保存页面
			}
			pWnd->SendMessage(WM_CLOSE,0,0);
		}
		pWnd = pTmpNext;
	}	

	// 关闭工作区 需要将按钮重置
	OnChildFrameMaximized(false);

	// 如果有F10窗口，此时应当关闭
	if ( NULL != m_pDlgF10 )
	{
		m_pDlgF10->ShowWindow(SW_HIDE);
		m_pDlgF10->PostMessage(WM_CLOSE, 0, 0);
	}
}

bool32 CMainFrame::LoadSystemDefaultCfm( int32 iPicId, CMerch *pMerchToChange/* = NULL*/, bool32 bMustSeeChart /*= false*/ )
{
	if(ID_PIC_TREND == iPicId)
	{
		return CCfmManager::Instance().LoadCfm(L"分时走势", FALSE) != NULL;	// 普通的
	}
	else if(ID_PIC_KLINE == iPicId)
	{
		return CCfmManager::Instance().LoadCfm(L"K线分析", FALSE) != NULL;	// 普通的
	}

	return LoadSystemDefaultCfmRetFrame(iPicId, pMerchToChange, bMustSeeChart) != NULL;
}

CMPIChildFrame * CMainFrame::LoadSystemDefaultCfmRetFrame( int32 iPicId, CMerch *pMerchToChange /*= NULL*/, bool32 bMustSeeChart /*= false*/ )
{
	// 找到当前激活视图
	CMerch *pMerch = pMerchToChange;
	if ( NULL == pMerch )
	{
		CIoViewBase *pIoView = FindActiveIoView();
		if ( NULL != pIoView )
		{
			pMerch = pIoView->GetMerchXml();
		}
	}

	CMPIChildFrame *pLastActiveFrame = (CMPIChildFrame *)MDIGetActive();

	CMPIChildFrame *pChildFrame = FindAimIoViewFrame(iPicId, true, true);	// 系统的一定要存在
	ASSERT( NULL != pChildFrame );
	if ( NULL != pChildFrame )
	{
		CCfmManager::Instance().ChangeIoViewStatus(pChildFrame);

		// 切换商品
		CIoViewBase *pAimIoView = FindIoViewInFrame(iPicId, pChildFrame, false, false);
		if ( NULL != pMerch && pChildFrame->GetGroupID() > 0 )
		{
			if ( pAimIoView != NULL && pAimIoView->GetMerchXml() != pMerch )
			{
				// 有目标情况下
				OnViewMerchChanged(pChildFrame->GetGroupID(), pMerch);
			}
			else
			{
				// 无目标一样切换
				OnViewMerchChanged(pChildFrame->GetGroupID(), pMerch);
			}
		}

		if ( bMustSeeChart && NULL != pAimIoView )
		{
			pAimIoView->BringToTop();
		}
		else
		{
			// 后激活
			MDIActivate(pChildFrame);
		}
		//pChildFrame->ShowWindow(SW_SHOW);

		if ( pChildFrame != pLastActiveFrame && pLastActiveFrame != NULL )
		{
			AddToEscBackFrame(pLastActiveFrame);

			OnChangeOpenedCfm(iPicId);
		}

		return pChildFrame;
	}

	return NULL;
}

CMPIChildFrame	* CMainFrame::LoadSpecialCfm( const CString &StrCfmName )
{
	if(_T("条件选股") == StrCfmName)
	{
		CMPIChildFrame *pChildFrame = CCfmManager::Instance().LoadCfm(StrCfmName, false, true);	// 可能不存在
		return pChildFrame;
	}

	// 找到当前激活视图
	CMerch *pMerch = NULL;
	//if ( NULL == pMerch )
	{
		CIoViewBase *pIoView = FindActiveIoView();
		if ( NULL != pIoView )
		{
			pMerch = pIoView->GetMerchXml();
		}
	}

	CMPIChildFrame *pLastActiveFrame = (CMPIChildFrame *)MDIGetActive();

	CMPIChildFrame *pChildFrame = CCfmManager::Instance().LoadCfm(StrCfmName, false, true);	// 可能不存在
	if ( NULL != pChildFrame )
	{
		// 切换商品
		if ( NULL != pMerch && pChildFrame->GetGroupID() > 0 )
		{
			OnViewMerchChanged(pChildFrame->GetGroupID(), pMerch);
		}

		// 后激活
		//pChildFrame->ShowWindow(SW_SHOW);
		MDIActivate(pChildFrame);
		if ( pChildFrame != pLastActiveFrame && pLastActiveFrame != NULL )
		{
			AddToEscBackFrame(pLastActiveFrame);

			CCfmManager::Instance().ChangeOpenedCfm(StrCfmName);
		}

		return pChildFrame;
	}

	return NULL;
}




void CMainFrame::EmptyEscBackFrame()
{
	m_aCfmsReturn.RemoveAll();
}


void CMainFrame::OnFinishSortServer()
{
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if ( NULL == pDoc )
	{
		return;
	}

	CAbsDataManager* pAbsDataManager = pDoc->m_pAbsDataManager;
	CAbsCenterManager* pAbsCenterManager = pDoc->m_pAbsCenterManager;
	if ( NULL == pAbsDataManager || NULL == pAbsCenterManager)
	{
		return;
	}

	//
	if ( COptimizeServer::Instance()->GetForcePingFlag() )
	{
		// 重新ping 测速的
		return;
	}

	//
	m_bManualReconnect = false;	

	// 排序完成 1: 重设服务器顺序 2: 发认证请求
	arrServer aServerSorted;
	COptimizeServer::Instance()->GetSortedServers(aServerSorted);

	for ( int32 ii = 0 ; ii < aServerSorted.GetSize() ; ii++ )
	{
		CServerState ss = aServerSorted[ii];			
		TRACE(L"----------- %s  %d \n", ss.m_StrServerAddr.GetBuffer(), ss.m_iSortIndex);
		ss.m_StrServerAddr.ReleaseBuffer();
	}

	//
	pAbsDataManager->ReSetCommunicationIndex(aServerSorted);

	// 关掉其他行情服务器
	pAbsDataManager->StopServiceAfterSortServer();

	//
	for ( int32 i = 0; i < aServerSorted.GetSize(); i++ )
	{
		// 第一台行情服务器发认证请求		
		CServerState stServerState = aServerSorted[i];
		bool32 bFirst = pAbsDataManager->BeFirstCommunication(stServerState.m_StrServerAddr, stServerState.m_uiServerPort);

		if ( bFirst )
		{
			int32 iCommunicationID = pAbsDataManager->GetCommunicationID(stServerState.m_StrServerAddr, stServerState.m_uiServerPort);
			bool32 bSevrConnected  = pAbsDataManager->IsServerConneted(stServerState.m_StrServerAddr, stServerState.m_uiServerPort);

			//
			if ( !bSevrConnected )
			{
				//ASSERT(0);
				continue;
			}

			//
			if ( NULL != pAbsCenterManager->GetServiceDispose() )
			{
				 			//	T_UserInfo stUserInfo;
				 			//	pAbsCenterManager->GetServiceDispose()->GetUserInfo(stUserInfo);

				// 发送认证请求:
				CMmiReqAuth ReqAuth;
				pDoc->m_pAbsCenterManager->GetQuoteAuthJson(ReqAuth.m_StrReqJson);

				pAbsCenterManager->RequestViewData((CMmiCommBase*)&ReqAuth, iCommunicationID);			
			}
			else
			{
				//ASSERT(0);
			}
		}
	}

	// 取消通知
	COptimizeServer::Instance()->RemoveNotify(this);

	// 定时器判断是否连接成功
	SetTimer(KTimerIdCheckConnectServer, KTimerPeriodCheckConnectServer, NULL);
}

bool32 CMainFrame::IsIndexInCurrentKLineFrame( const CString &StrIndexName, int32 iIndexFlag/* = 0*/ )
{
	CMPIChildFrame *pKlineFrame = FindAimIoViewFrame(ID_PIC_KLINE, false);
	if ( NULL == pKlineFrame || MDIGetActive() != pKlineFrame )
	{
		return false;	// 无k线页面加载或者不是当前页面
	}

	CIoViewKLine *pKline = (CIoViewKLine *)FindIoViewInFrame(ID_PIC_KLINE, pKlineFrame, false);
	if ( NULL == pKline )
	{
		ASSERT( 0 );
		return false;	// K线页面无k线视图，这是错误的情况
	}

	CMerch *pMerch = pKline->GetMerchXml();
	if ( NULL == pMerch )
	{
		return false;
	}
	bool32 bMerchIsIndex = ERTExp == pMerch->m_Market.m_MarketInfo.m_eMarketReportType;
	if ( iIndexFlag > 0 )
	{
		if ( !bMerchIsIndex )
		{
			return false;	// 非要求的指数商品 
		}
	}
	else if ( iIndexFlag < 0 )
	{
		if ( bMerchIsIndex )
		{
			return false;	// 非要求的个股商品
		}
	}

	CStringArray aIndexNames;
	pKline->GetCurrentIndexNameArray(aIndexNames);
	for ( int i=0; i < aIndexNames.GetSize() ; i++ )
	{
		if ( aIndexNames[i] == StrIndexName )
		{
			return true;	// 有此指标
		}
	}
	return false;	// 遍历无此指标
}

void CMainFrame::AdjustByOfflineMode()
{
	return;
	if ( !CGGTongApp::m_bOffLine )
	{
		return;
	}

	// 去掉菜单上的 升级客户端, 连接服务器, 重连服务器, 盘后数据下载
	CMenu * pMenu = this->GetMenu();	
	CNewMenu * pNewMenu = DYNAMIC_DOWNCAST(CNewMenu,pMenu);
	pNewMenu->LoadToolBar(g_awToolBarIconIDs);

	CMenu * pSubMenu = pNewMenu->GetSubMenu(L"系统(&S)");
	ASSERT( NULL != pSubMenu );
	CNewMenu * pRootMenu = DYNAMIC_DOWNCAST(CNewMenu,pSubMenu);

	//
	BOOL bOK = FALSE;

	bOK = pRootMenu->RemoveMenu(13, MF_BYPOSITION);
	bOK = pRootMenu->RemoveMenu(12, MF_BYPOSITION);
	bOK = pRootMenu->RemoveMenu(9, MF_BYPOSITION);
	bOK = pRootMenu->RemoveMenu(8, MF_BYPOSITION);
	bOK = pRootMenu->RemoveMenu(7, MF_BYPOSITION);
	bOK = pRootMenu->RemoveMenu(6, MF_BYPOSITION);
	bOK = pRootMenu->RemoveMenu(5, MF_BYPOSITION);
	bOK = pRootMenu->RemoveMenu(4, MF_BYPOSITION);
	bOK = pRootMenu->RemoveMenu(3, MF_BYPOSITION);
	bOK = pRootMenu->RemoveMenu(1, MF_BYPOSITION);
	bOK = pRootMenu->RemoveMenu(0, MF_BYPOSITION);	
}

int32 CMainFrame::DeleteBlockReportItemData( CNewMenu *pPopupMenu )
{
	/*
	int32 iDelCount = 0;
	if ( pPopupMenu == NULL )
	{
	return 0;
	}

	const int32 iMenuItemCount = pPopupMenu->GetMenuItemCount();
	for ( int32 iSub=0; iSub < iMenuItemCount ; iSub++ )
	{
	const UINT uCmdId = pPopupMenu->GetMenuItemID(iSub);
	if ( uCmdId >= IDM_BLOCKMENU_BEGIN && uCmdId <= IDM_BLOCKMENU_END )
	{
	// 物理板块
	T_BlockDesc *pDesc = (T_BlockDesc *)pPopupMenu->GetItemData(uCmdId);
	delete pDesc;
	iDelCount++;
	pPopupMenu->SetItemData(uCmdId, NULL);
	}
	else if ( uCmdId >= IDM_BLOCKMENU_BEGIN3 && uCmdId <= IDM_BLOCKMENU_END3 )
	{
	// 以前的系统板块 - 应当不存在了的
	ASSERT( 0 );
	}
	else if ( uCmdId >= IDM_BLOCKMENU_BEGIN2 && uCmdId <= IDM_BLOCKMENU_END2 )
	{
	// user block 实际已经包含在IDM_BLOCKMENU_BEGIN中
	T_BlockDesc *pDesc = (T_BlockDesc *)pPopupMenu->GetItemData(uCmdId);
	delete pDesc;
	iDelCount++;
	pPopupMenu->SetItemData(uCmdId, NULL);
	}
	else if ( uCmdId >= IDM_BLOCKMENU_BEGIN4 && uCmdId <= IDM_BLOCKMENU_END4 )
	{
	// 逻辑板块
	}
	else if ( uCmdId != (UINT)-1 )
	{
	ASSERT( 0 );	// 不应当有其它菜单
	}
	}

	return iDelCount;
	*/
	return 0;
}

void CMainFrame::OnConnectNewsServer()
{
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if ( NULL == pDoc || NULL == pDoc->m_pNewsManager)
	{
		return;
	}
	// 给与适当的提示 TODO
	SetUserConnectCmdFlag(0, EUCCF_ConnectNewsServer);
	// 资讯服务器暂未处理, 直接重连
	pDoc->m_pNewsManager->StartAllNewsCommunication(true);

	m_uiConnectNewsServerTimers = 0;
	SetTimer(KTimerIdCheckNewsConnectServer, KTimerPeriodCheckNewsConnectServer, NULL);
}

void CMainFrame::OnDisConnectNewsServer()
{
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if (NULL == pDoc || NULL == pDoc->m_pNewsManager)
	{
		return;
	}
	// 给与适当的提示 TODO
	SetUserConnectCmdFlag(EUCCF_ConnectNewsServer, 0);
	pDoc->m_pNewsManager->StopAllNewsCommunication();

	if ( NULL != m_pDlgF10 )
	{
		//m_pDlgF10->OnNewsServerDisConnected(-1);
	}		
}

void CMainFrame::OnConnectSetting()
{
	CDlgConnectSet dlgConnectSet;	

	T_ProxyInfo ProxyInfo;

	dlgConnectSet.ReadProxyInfoFromXml(_T(""));
	if ( dlgConnectSet.DoModal()==IDOK )
	{
		// 更改了网络设置，重新设置网络参数，进行连接？
		// 现在改变网络参数导致的重连步骤比较复杂，仅提示重启后生效
		AfxMessageBox(_T("您所做的网络设置变更将在重启后生效"));
	}
}

void CMainFrame::SetUserConnectCmdFlag( DWORD dwFlagRemove, DWORD dwFlagAdd, DWORD dwMask /*= 0xffffffff*/ )
{
	m_dwUserConnectCmdFlag &= ~(dwFlagRemove&dwMask);
	m_dwUserConnectCmdFlag |= (dwFlagAdd&dwMask);
}

void CMainFrame::OnChooseSimpleStock()
{
	CDlgChooseStockVar::MerchArray aMerchs;
	if ( CDlgChooseStockVar::ChooseStockVar(aMerchs, false) )
	{
		ASSERT( aMerchs.GetSize() > 0 && aMerchs[0] != NULL );
		if ( aMerchs.GetSize() > 0 && aMerchs[0] != NULL )
		{
			OnHotKeyMerch(aMerchs[0]->m_MerchInfo.m_iMarketId, aMerchs[0]->m_MerchInfo.m_StrMerchCode);
		}
	}
}

void CMainFrame::OnMerchNotePad()
{
	// 投资日记
	CDlgNotePad::ShowNotePad(CDlgNotePad::EST_UserNote);
}

void CMainFrame::OnUpdateIoViewMenu( CCmdUI *pCmdUI )
{
	// 所有视图公用的菜单项更新状态
	//	const UINT nID = pCmdUI->m_nID;

	CIoViewBase * pIoViewBase = FindActiveIoView();
	CGGTongView * pGGTongView = NULL;

	if ( NULL == pIoViewBase)
	{
		pGGTongView = (CGGTongView *)FindGGTongView();
	}
	else
	{
		CWnd * pWnd = pIoViewBase->GetParent();

		while (!(pWnd->IsKindOf(RUNTIME_CLASS(CGGTongView))))
		{
			pWnd = pWnd->GetParent();
		}

		pGGTongView =(CGGTongView *)pWnd;	
	}

	if ( NULL == pGGTongView
		|| NULL == pIoViewBase )
	{
		return;
	}

	pIoViewBase->OnUpdateIoViewMenu(pCmdUI);
}


bool32 CMainFrame::OnPicMenuCfm( UINT nPicId )
{
	if ( ID_PIC_PHASESORT == nPicId )
	{
		// 阶段排行， 先让用户选择，然后找页面，不存在页面则创建一个新的视图
		// 现将阶段排行变成与图标&资讯一样的页面处理
		ShowPhaseSort(NULL);
		// 不管处理没
		return true;
	}
	else if ( ID_PIC_TIMESALE == nPicId)
	{
		// 分笔与分价表的特殊情况
		if (!OnShowF7IoViewInChart(nPicId))
		{
			// 失败的时候, 创建一个出来吧	
			CreateIoViewByPicMenuID(nPicId, true);
		}
	}
	else if (ID_PIC_FENJIABIAO == nPicId)
	{
		CIoViewManager* pIoViewManage = NULL;

		CMPIChildFrame*	pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, MDIGetActive());
		if (NULL != pChildFrame)
		{
			CIoViewBase* pView = FindIoViewInFrame(nPicId, pChildFrame);
			if (NULL != pView)
			{
				pIoViewManage = pView->GetIoViewManager();
			}
		}

		//
		CreateIoViewByPicMenuID(nPicId, true, pIoViewManage);
	}
	else
	{
		// 从配置中取相关数值
		T_PicCfmInfo picInfo;
		picInfo.nPicId = 0;
		for ( int32 i=0; i < KPicCfmInfosCount ; i++ )
		{
			if ( s_aPicCfmInfos[i].nPicId == nPicId )
			{
				picInfo = s_aPicCfmInfos[i];
				break;
			}
		}
		if ( picInfo.nPicId == nPicId )
		{
			if ( picInfo.nFunc != 0 && !CPluginFuncRight::Instance().IsUserHasRight((CPluginFuncRight::E_FuncRight)picInfo.nFunc, true) )
			{
				return false;	// 不具备配置的权限
			}
			CString StrCfmName = picInfo.szCfmName;

			CMPIChildFrame *pFrame = NULL;
			if ( picInfo.nSystemPicId > 0 )
			{
				pFrame = LoadSystemDefaultCfmRetFrame((int32)picInfo.nSystemPicId, NULL, false);
			}
			else
			{
				pFrame = LoadSpecialCfm(StrCfmName);
			}

			// 可能特殊页面需要权限，不能打开
			T_CfmFileInfo cfm;
			if ( NULL==pFrame && CCfmManager::Instance().QueryUserCfm(StrCfmName, cfm) )
			{
				ASSERT( cfm.m_bNeedServerAuth );
				return false;
			}

			if ( NULL == pFrame
				&& picInfo.bCreateIfNotExist )
			{
				// 尝试创建该页面视图
				CMPIChildFrame *pOldFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, MDIGetActive());
				CIoViewBase *pIoViewActive = FindActiveIoView();
				CMerch *pMerchActive = NULL == pIoViewActive ? NULL : pIoViewActive->GetMerchXml();
				pFrame = CreateNewIoViewFrame(StrCfmName, nPicId, true, pMerchActive);
				if ( pFrame != NULL )
				{
					AddToEscBackFrame(pOldFrame);

					OnChangeOpenedCfm(nPicId);
				}
			}

			if ( NULL != pFrame )
			{
				// 如果页面在F7状态下，需要取消F7状态, 避免出现异常显示
				if ( pFrame->IsF7AutoLock() )
				{
					// 还原
					OnProcessCFF7(pFrame);
				}
				CIoViewBase *pIoView = FindIoViewInFrame(nPicId, pFrame);
				if ( NULL != pIoView )
				{
					pIoView->BringToTop();
					return true;
				}
			}
		}
	}

	ASSERT( 0 );
	return false;
}

CMPIChildFrame	* CMainFrame::CreateNewIoViewFrame( const CString &StrCfmName, UINT nPicId, bool32 bCfmSecret/*=true*/, CMerch *pMerchToChange/*=NULL*/ )
{
	// 创建新的指定名称的页面，该页面名称必须不存在
	CMPIChildFrame *pChildFrame = NULL;
	T_CfmFileInfo cfm;
	if ( CCfmManager::Instance().QueryUserCfm(StrCfmName, cfm) )
	{
		ASSERT( 0 );
		return NULL;
	}

	OnWindowNew();
	CCfmManager::Instance().CurCfmSaveAs(StrCfmName);
	pChildFrame = CCfmManager::Instance().GetCfmFrame(StrCfmName);
	ASSERT( NULL != pChildFrame );

	//OnPictureMenu(iPicIdToFind);	// 此pic是否还可以调用呢
	CIoViewBase *pIoView = CreateIoViewByPicMenuID(nPicId, true);

	CAbsCenterManager *pAbsCenterManager = NULL;
	if ( NULL != pMerchToChange
		&& NULL != pIoView
		&& NULL != (pAbsCenterManager=pIoView->GetCenterManager())
		&& pIoView->GetMerchXml() != pMerchToChange )
	{
		// 切换商品, 有可能有大量视图的商品需要切换！！！
		OnViewMerchChanged(pIoView, pMerchToChange);
	}

	// 通过这里创建的页面都是偷偷摸摸的页面，要从打开列表中隐藏它
	if ( bCfmSecret 
		&& CCfmManager::Instance().QueryUserCfm(StrCfmName, cfm) )
	{
		cfm.m_bSecret = true;
		CCfmManager::Instance().ModifyUserCfmAttri(cfm);	// 修改成隐藏属性
	}

	CCfmManager::Instance().SaveCurrentCfm();
	return pChildFrame;
}

void CMainFrame::OnUpdateFrameMenu( HMENU hMenuAlt )
{
	CNewMDIFrameWnd::OnUpdateFrameMenu(hMenuAlt);
}

void CMainFrame::ChangeAnalysisMenu( bool32 bLockedFrame )
{
	const CString StrAnalysisMenu(_T("分析(&F)"));
	// 更改菜单
	CNewMenu *pMenu = DYNAMIC_DOWNCAST(CNewMenu, GetMenu());
	if ( NULL != pMenu )
	{
		int32 iPos = -1;
		CMenu *pOldMenuA = NULL;
		for ( int32 i=0; i < int32(pMenu->GetMenuItemCount()) ; i++ )
		{
			CString StrMenu;
			if ( pMenu->GetMenuString(i, StrMenu, MF_BYPOSITION)
				&& StrMenu == StrAnalysisMenu )
			{
				iPos = i;
				pOldMenuA = pMenu->GetSubMenu(i);
				break;
			}
		}
		CNewMenu *pMenuA = GetAnalysisMenu(bLockedFrame);
		if ( iPos >= 0 && NULL != pMenuA && pMenuA != pOldMenuA )
		{
			pMenu->RemoveMenu(iPos, MF_BYPOSITION);
			pMenu->InsertMenu(iPos, MF_BYPOSITION|MF_POPUP, (UINT)pMenuA->GetSafeHmenu(), StrAnalysisMenu);

			if ( m_hWndMDIClient != NULL )
			{
				// 并没有修改可见部分，可以不用刷新
				// 				::SendMessage(m_hWndMDIClient, WM_MDISETMENU,
				// 					(WPARAM)pMenu->GetSafeHmenu(), (LPARAM)GetWindowMenuPopup(pMenu->GetSafeHmenu()));
			}
		}
	}

}

CNewMenu		* CMainFrame::GetAnalysisMenu( bool32 bLockedFrame )
{
	if ( bLockedFrame )
	{
		if ( m_menuAnalysisLocked.m_hMenu == NULL )
		{
			m_menuAnalysisLocked.CreatePopupMenu();
			m_menuAnalysisLocked.LoadToolBar(g_awToolBarIconIDs);
		}
		return &m_menuAnalysisLocked;
	}

	if ( m_menuAnalysisUnLock.m_hMenu == NULL )
	{
		m_menuAnalysisUnLock.CreatePopupMenu();
		m_menuAnalysisUnLock.LoadToolBar(g_awToolBarIconIDs);
	}
	return &m_menuAnalysisUnLock;	
}

void CMainFrame::DoIoViewEscBackFrameInActiveFrame( CMPIChildFrame *pFrame, CMerch *pMerch )
{
	if ( NULL == pFrame || NULL == pMerch )
	{
		ASSERT( 0 );
		return;
	}

	for ( int32 i=0; i < m_IoViewsPtr.GetSize() ; i++ )
	{
		CIoViewBase *pIoView = m_IoViewsPtr[i];
		if ( pIoView->GetParentFrame() == pFrame )
		{
			pIoView->OnEscBackFrameMerch(pMerch);
		}
	}
}

void CMainFrame::OnMarketRadar()
{
	CDlgMarketRadarList::ShowMarketRadarList();
}

void CMainFrame::OnUpdateMarketRadar( CCmdUI *pCmdUI )
{
	pCmdUI->SetCheck( CMarketRadarCalc::Instance().GetRunFlag() != 0 );
}

void CMainFrame::OnMarketRadarAddResult()
{
	T_MarketRadarParameter Param;
	CMarketRadarCalc::Instance().GetParameter(Param);
	switch (Param.m_ParamSub.m_eVoiceType)
	{
	case EMRAVT_None:		// 不做任何事
		break;
	case EMRAVT_System:
		MessageBeep(MB_ICONEXCLAMATION);
		break;
	case EMRAVT_User:
		if ( !Param.m_ParamSub.m_StrVoiceFilePath.IsEmpty() )
		{
			if ( !PlaySound(Param.m_ParamSub.m_StrVoiceFilePath, NULL, SND_ASYNC|SND_FILENAME|SND_NOSTOP) )
			{
				//MessageBeep(MB_ICONEXCLAMATION);
			}
		}
		break;
	default:
		ASSERT( 0 );
		break;
	}

	if ( Param.m_ParamSub.m_bShowDlgListAlarm )
	{
		CWnd *pWnd = AfxGetMainWnd();
		if ( NULL != pWnd )
		{
			::PostMessage(pWnd->GetSafeHwnd(), WM_COMMAND, ID_MARKETRADAR, 0);
		}
	}
}

LRESULT CMainFrame::OnCheckCfmHide( WPARAM w, LPARAM l )
{
	HWND hWnd = (HWND)w;
	if ( !::IsWindowVisible(hWnd) )
	{
		ASSERT( 0 );	// 应当检测所有的出口点，将它显示
		::ShowWindow(hWnd, SW_SHOW);
	}

	CMPIChildFrame *pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, CWnd::FromHandlePermanent(hWnd));
	if ( NULL != pFrame
		&& pFrame->IsHideMode() )
	{
		ASSERT( 0 );	// 应该离开此模式了的, 难道有其它的需求？
	}

	return 1;
}

bool32 CMainFrame::ShowPhaseSort( const T_PhaseOpenBlockParam *pParam /*= NULL*/ )
{
	// 阶段排行， 先让用户选择，然后找页面，不存在页面则创建一个新的视图
	// 现将阶段排行变成与图标&资讯一样的页面处理
	T_PhaseOpenBlockParam BlockParam;
	T_PhaseOpenBlockParam BlockParamOld;
	// 如果没有参数，则使用现在打开的参数或者以前的参数
	if ( NULL == pParam )
	{
		// 使用现在页面的参数
		CMPIChildFrame *pFrame = FindAimIoViewFrame(ID_PIC_PHASESORT, false);
		CIoViewPhaseSort *pPhaseSort = NULL;
		if ( NULL != pFrame )
		{
			pPhaseSort = (CIoViewPhaseSort *)FindIoViewInFrame(ID_PIC_PHASESORT, pFrame);
			if ( NULL != pPhaseSort )
			{
				pPhaseSort->GetPhaseOpenBlockParam(BlockParamOld);	// 初始化
				pParam = &BlockParamOld;
			}
		}
	}
	bool32 bSel = false;
	bSel = CDlgPhaseSort::PhaseSortSetting(BlockParam, pParam);
	if ( bSel )
	{
		CMPIChildFrame *pFrame = FindAimIoViewFrame(ID_PIC_PHASESORT, true);
		CIoViewPhaseSort *pPhaseSort = NULL;
		if ( NULL != pFrame )
		{
			pPhaseSort = (CIoViewPhaseSort *)FindIoViewInFrame(ID_PIC_PHASESORT, pFrame);
		}
		if ( NULL == pPhaseSort )
		{
			pPhaseSort = (CIoViewPhaseSort *)CreateIoViewByPicMenuID(ID_PIC_PHASESORT, true);
		}
		ASSERT( NULL != pPhaseSort );
		if ( NULL != pPhaseSort )
		{
			pPhaseSort->BringToTop();
			pPhaseSort->OpenBlock(BlockParam, true);
			pPhaseSort->ChangeTabToCurrentBlock();
		}
	}

	return bSel;
}

void CMainFrame::OnUpdateMainMenu( CCmdUI *pCmdUI )
{
	UINT nId = pCmdUI->m_nID;
	CGGTongDoc *pDoc = AfxGetDocument();
	if ( NULL == pDoc )
	{
		ASSERT( 0 );
		return;
	}
	CIoViewBase *pActiveIoView = FindActiveIoView();

	if ( ID_AUTOPAGE_SETTING == nId )
	{
		// 弹出对话框设置时间
	}
	else if ( ID_FUNC_CONDITIONALARM == nId )
	{

		// 条件预警
		if (  NULL != pDoc->m_pAarmCneter )
		{
			pCmdUI->SetCheck(pDoc->m_pAarmCneter->GetAlarmFlag());
		}

	}
	/*
	else if ( ID_FUNC_MAINMONITOR == nId )
	{
	// 主力监控
	}
	*/
	else if ( ID_FUNC_ADDTOBLOCK == nId )
	{
		// 添加到板块
	}
	else if ( ID_FUNC_DELFROMBLOCK == nId )
	{
		// 从板块删除
	}
	else if ( ID_AUTOPAGE == nId )
	{
		// 自动翻页
		// 仅支持报价表？
		CIoViewReport *pReport = NULL;
		bool32 bCheck = false;
		if ( NULL != (pReport=DYNAMIC_DOWNCAST(CIoViewReport, pActiveIoView)) )
		{
			bCheck = pReport->IsAutoPageStarted();
		}
		pCmdUI->SetCheck(bCheck);
	}
	else if ( ID_ZongHePaiMing == nId )
	{
		// 综合排名
	}
	/*
	else if ( ID_HangQingCaiWu == nId )
	{
	// 行情、财务 F10
	}
	*/
	/*
	else if ( ID_ReMenBanKuai == nId )
	{
	// 热门板块
	bool32 bCheck = false;
	if ( CCfmManager::Instance().IsCurrentCfm(_T("热门板块")) )
	{
	bCheck = true;
	}
	pCmdUI->SetCheck(bCheck);
	}
	else if ( ID_DaPanZouShi == nId )
	{
	// 大盘走势
	}
	*/
	else if ( ID_InfoMine == nId )
	{
		// 信息地雷
	}
	//else if ( ID_PIC_CFM_MULTIINTERVAL == nId )
	//{
	//	// 多周期同列
	//	pCmdUI->SetCheck(CCfmManager::Instance().IsCurrentCfm(_T("多周期同列")));
	//}
	//else if ( ID_PIC_CFM_MULTIMERCH == nId )
	//{
	//	// 多股同列
	//	pCmdUI->SetCheck(CCfmManager::Instance().IsCurrentCfm(_T("多合约同列")));
	//}
	else if ( ID_TRADEEXE_SETTING == nId )
	{
		// 交易程序设置
	}
}

void CMainFrame::OnMainMenu( UINT nId )
{
	CIoViewBase *pActiveIoView = FindActiveIoView();	// 可能为NULL
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	CGGTongDoc* pDoc = (CGGTongDoc*)pApp->m_pDocument;
	if ( ID_AUTOPAGE_SETTING == nId )
	{
		// 弹出对话框设置时间
		CString StrInput;
		StrInput.Format(_T("%d"), CIoViewReport::GetAutoPageTimerPeriod());
		if ( CDlgInputShowName::GetInputString(StrInput, _T("请输入自动翻页时间[4-60秒]"), _T("自动翻页时间设置")) == IDOK )
		{
			int32 iPeriod = _ttoi(StrInput);
			if ( iPeriod < 4 || iPeriod > 60 )
			{
				MessageBox(_T("您输入的时间不符合要求，被忽略!"));
			}
			else
			{
				CIoViewReport::SetAutoPageTimerPeriod(iPeriod);
			}
		}
	}
	else if ( ID_FUNC_CONDITIONALARM == nId )
	{
		// 条件预警
		m_wndStatusBar.CreateAlarmDialog();
	}

	else if ( ID_FUNC_MAINMONITOR == nId )
	{
		// 主力监控
		CDlgMainTimeSale::ShowPopupMainTimeSale();
	}

	else if ( ID_FUNC_ADDTOBLOCK == nId )
	{
		// 添加到板块
		if ( NULL != pActiveIoView
			&& NULL != pActiveIoView->GetMerchXml() )
		{
			pActiveIoView->PostMessage(WM_COMMAND, ID_BLOCK_ADDTO, NULL);
		}
		else
		{
			MessageBox(_T("没有选定的商品"), _T("提示"));
		}
	}
	else if ( ID_FUNC_DELFROMBLOCK == nId )
	{
		// 从板块删除
		CMerch *pMerch = NULL != pActiveIoView ? pActiveIoView->GetMerchXml() : NULL;
		if ( NULL != pMerch )
		{
			T_Block *pBlock = CUserBlockManager::Instance()->GetBlock(pMerch);
			if ( NULL != pBlock && NULL != pActiveIoView )
			{
				pActiveIoView->PostMessage(WM_COMMAND, ID_BLOCK_REMOVEFROM, NULL);
			}
			else
			{
				MessageBox(_T("该商品不属于任何自选板块"), _T("提示"));
			}
		}
		else
		{
			MessageBox(_T("没有选定的商品"), _T("提示"));
		}
	}
	else if ( ID_AUTOPAGE == nId )
	{
		// 自动翻页
		// 仅支持报价表？
		CIoViewReport *pReport = NULL;
		if ( NULL != (pReport=DYNAMIC_DOWNCAST(CIoViewReport, pActiveIoView)) )
		{
			if ( pReport->IsAutoPageStarted() )
			{
				pReport->StopAutoPage();
			}
			else
			{
				pReport->StartAutoPage(true);				
			}
		}
	}
	//else if ( ID_SYS_AUTO_RUN == nId )
	//{
	//	if (pDoc)
	//	{
	//		pDoc->m_bAutoRun ^= 1;
	//		SetAutoRun(pDoc->m_bAutoRun);
	//	}
	//}
	else if ( ID_ZongHePaiMing == nId )
	{
		// 综合排名
		CDlgReportRank::ShowPopupReportRank(CBlockConfig::GetDefaultMarketlClassBlockPseudoId());	// 上证A股
	}
	/*
	else if ( ID_HangQingCaiWu == nId )
	{
	// 行情、财务 F10
	if ( NULL != pActiveIoView
	&& NULL != pActiveIoView->GetMerchXml() )
	{
	DoF10();
	}
	}
	*/
	/*
	else if ( ID_ReMenBanKuai == nId )
	{
	// 热门板块
	ShowHotBlockReport();
	}
	else if ( ID_DaPanZouShi == nId )
	{
	// 大盘走势
	// 调用上证分时
	OnShowMerchInChart(0, _T("000001"), ID_PIC_TREND);
	}
	*/
	else if ( ID_InfoMine == nId )
	{
		// 信息地雷
		CDlgNotePad::ShowNotePad(CDlgNotePad::EST_InfoMine);
	}
	else if ( ID_PIC_CFM_MULTIINTERVAL == nId )
	{
		// 多周期同列
		CString StrCfm = _T("多周期同列");
		//if ( CCfmManager::Instance().IsCurrentCfm(StrCfm) )
		//{
		//	CCfmManager::Instance().CloseCfmFrame(StrCfm, true);
		//}
		//else
		{
			LoadSpecialCfm(StrCfm);
		}
	}
	else if ( ID_PIC_CFM_MULTIMERCH == nId )
	{
		// 多股同列
		m_leftToolBar.ShowDuoGuTongLie();
	}
	else if ( ID_TRADEEXE_SETTING == nId )
	{
		// 交易程序设置
	}
	else if(ID_ANALYSIS_CJMX == nId)
	{
		LoadSpecialCfm(L"成交明细");
	}
	else if (ID_ANALYSIS_TLKX == nId)
	{
		LoadSpecialCfm(L"套利K线");
	}
	else if (ID_USE_HELP == nId)
	{
		ShowHelpMenu(nId);
	}
	else if (ID_USER_SERVICE == nId)
	{
		ShowHelpMenu(nId);
	}
	else if (ID_SPECIAL_INFO_CJRL == nId)
	{
		CString strNewsIofo = L"";
		int32 iSize = pApp->m_pConfigInfo->m_aWndSize.GetSize();
		int32 iWidth = 900, iHeight = 680;
		for (int32 i=0; i<iSize; i++)
		{
			CWndSize wnd = pApp->m_pConfigInfo->m_aWndSize[i];
			if (L"财经日历" == wnd.m_strID)
			{
				iWidth = wnd.m_iWidth;
				iHeight = wnd.m_iHeight;
				strNewsIofo = wnd.m_strTitle;

				break;
			}
		}

		UrlParser urlParser(L"alias://calendar");
		CDlgTodayCommentIE::ShowDlgIEWithSize(strNewsIofo, urlParser.GetUrl(), CRect(0,0,iWidth,iHeight));

	}
	else if (ID_SPECIAL_TRADE_INTERNAL == nId)
	{
		if ( !CPluginFuncRight::Instance().IsUserHasRight(L"模拟交易", true))
			return ;

		HideAllTradeWnd();
		LoadSpecialCfm(L"模拟交易");
	}
	else if (ID_SPECIAL_TRADE_PLUGIN == nId)
	{

	}
	else if (ID_SPECIAL_LIVE_VIDEO == nId)
	{
		if ( !CPluginFuncRight::Instance().IsUserHasRight(L"视频直播", true))
			return ;

		//视频直播
		LoadSpecialCfm(L"直播大厅");
	}
	else if(ID_SPECIAL_LIVE_TEXT == nId)
	{
		//文字直播
		LoadSpecialCfm(L"文字直播");
	}
	else if(ID_SPECIAL_FUNC_ARBITRAGE == nId)
	{
		if ( !CPluginFuncRight::Instance().IsUserHasRight(L"套利模型", true))
			return ;

		//套利模型
		OnArbitrage();
	}
}

void CMainFrame::OnArbitrage()
{
	// 判断是否具有权限
	if ( !CPluginFuncRight::Instance().IsUserHasRight(_T("套利"), true) )
	{
		return;
	}

	CDlgArbitrage Dlg;
	// 从当前的业务视图中找可能提供套利的
	CIoViewBase *pIoViewActive = FindActiveIoView();
	CArbitrage *pArb = NULL;
	CArbitrage arb;
	bool32 bEmptyArb = true;
	if ( NULL != pIoViewActive )
	{
		if ( pIoViewActive->IsKindOf(RUNTIME_CLASS(CIoViewReportArbitrage)) )
		{
			CIoViewReportArbitrage *pArbReport = DYNAMIC_DOWNCAST(CIoViewReportArbitrage, pIoViewActive);
			pArb = pArbReport->GetCurrentSelArbitrage();
			bEmptyArb = false;
		}
		else if ( pIoViewActive->IsKindOf(RUNTIME_CLASS(CIoViewTrendArbitrage)) )
		{
			CIoViewTrendArbitrage *pArbTrend = (CIoViewTrendArbitrage *)pIoViewActive;
			bEmptyArb = false;
			if ( pArbTrend->GetArbitrage(arb) )
			{
				pArb = &arb;
			}
		}
		else if ( pIoViewActive->IsKindOf(RUNTIME_CLASS(CIoViewKLineArbitrage)) )
		{
			CIoViewKLineArbitrage *pArbKline = (CIoViewKLineArbitrage *)pIoViewActive;
			bEmptyArb = false;
			if ( pArbKline->GetArbitrage(arb) )
			{
				pArb = &arb;
			}
		}
	}
	if ( NULL != pArb )
	{
		Dlg.SetInitParams(false, pArb);
	}
	else
	{
		Dlg.SetInitParams(true, NULL);
	}

	int32 iDoModal = Dlg.DoModal();
	if ( IDOK == iDoModal && NULL == pArb )
	{
		// 自动跳转到套利报价表
		OnPicMenuCfm(ID_PIC_REPORT_ARBITRAGE);
	}
}

void CMainFrame::ShowHotBlockReport( const CGmtTime *pTimeStart/*=NULL*/, const CGmtTime *pTimeEnd/*=NULL*/ )
{
	// 加载这个页面就可以了, 目前没有区间热门板块分析，也没有扩展出排序接口
	UNREFERENCED_PARAMETER(pTimeStart);
	UNREFERENCED_PARAMETER(pTimeEnd);
	LoadSpecialCfm(_T("热门板块"));

	CIoViewBlockReport *pBlockReport = (CIoViewBlockReport *)FindIoViewInFrame(ID_PIC_BLOCKREPORT, NULL, true, true);
	if ( NULL != pBlockReport )
	{
		pBlockReport->SortColumn(2, 1);	// 总是排序第n列
	}
}

void CMainFrame::OnDefaultCfmLoad()
{
	// 加载完默认页面后，初始当前的分组商品

}

void CMainFrame::InitSpecialMenu()
{
	CMenu * pMenu = this->GetMenu();
	CNewMenu *pNewMenu = DYNAMIC_DOWNCAST(CNewMenu,pMenu);
	ASSERT(NULL != pNewMenu);
	CMenu * pSubMenu = pNewMenu->GetSubMenu(L"特色(&T)");
	if (NULL == pSubMenu)
	{
		return;
	}

	//
	CNewMenu * pRootMenu = DYNAMIC_DOWNCAST(CNewMenu,pSubMenu);
	if (NULL == pRootMenu)
	{
		return;
	}

	// 删除特色菜单
	DWORD dw = CConfigInfo::Instance()->GetVersionFlag();
	if ( CConfigInfo::FlagCaiFuJinDunFree == dw )
	{
		if ( NULL!=pRootMenu )
		{
			pNewMenu->DeleteMenu((UINT)pRootMenu->GetSafeHmenu(), MF_BYCOMMAND);
		}

		CNewMenu *pViewMenu = (CNewMenu *)pNewMenu->GetSubMenu(_T("查看"));
		if ( NULL!=pViewMenu )
		{
			pViewMenu->DeleteMenu(ID_VIEW_TB_SPECIAL, MF_BYCOMMAND);
		}
		// 删除显示特色工具栏项
		return;
	}

	if ( NULL == pRootMenu )
	{
		return;
	}

	CBJSpecialStringImage::Id2StringMap strmap;
	CBJSpecialStringImage::Instance().GetMenuId2StringMap(strmap);

	for ( int32 j = 0; j < int32(pRootMenu->GetMenuItemCount()); j++ )
	{
		DWORD dwId = pRootMenu->GetMenuItemID(j);
		CString StrItemText;
		pRootMenu->GetMenuString(j, StrItemText, MF_BYPOSITION);
		CString StrItemNewText;
		StrItemNewText = CBJSpecialStringImage::Instance().GetSpecialString(StrItemText);
		strmap[ dwId ] = StrItemNewText;
		if ( StrItemNewText != StrItemText )
		{
			pRootMenu->SetMenuText(dwId, StrItemNewText, MF_BYCOMMAND);
		}
	}

	CBJSpecialStringImage::Instance().SetMenuId2StringMap(strmap);
}

void CMainFrame::ChangeIoViewKlineTimeInterval( int32 iTimeIntervalId )
{
	CIoViewBase *pIoView = FindIoViewInFrame(ID_PIC_KLINE, NULL, false, true);
	if ( NULL == pIoView )
	{
		CMPIChildFrame *pKlineFrame = FindAimIoViewFrame(ID_PIC_KLINE, true);
		if ( NULL != pKlineFrame )
		{
			pIoView = FindIoViewInFrame(ID_PIC_KLINE, pKlineFrame, false, true);
		}
		if ( NULL == pIoView )
		{
			ASSERT( 0 );	// 无法找到合适的K线视图
			return;
		}
	}

	CIoViewKLine *pKline = DYNAMIC_DOWNCAST(CIoViewKLine, pIoView);
	if ( NULL == pKline )
	{
		ASSERT( 0 );
		return;
	}

	if ( iTimeIntervalId < 0 )
	{
		pKline->OnKeyF8();
	}
	else
	{
		// 指定了周期切换
		//pKline->OnMenu((UINT)iTimeIntervalId);
		pKline->PostMessage(WM_COMMAND, iTimeIntervalId, 0);
	}
	pIoView->BringToTop();
}

void CMainFrame::OnUpdateConnectServer( CCmdUI *pCmdUI )
{
	return;
	bool32 bConnectedData = m_wndStatusBar.m_bConnect;
	bool32 bConnectedNews = m_wndStatusBar.m_bConnectNews;
	if ( NULL != pCmdUI )
	{
		UINT uId = pCmdUI->m_nID;
		switch (uId)
		{
		case ID_CONNECT_SERVER:
			{
				//pCmdUI->Enable(!(GetUserConnectCmdFlag()&EUCCF_ConnectDataServer));
				pCmdUI->Enable(!bConnectedData);
			}
			break;
		case ID_DISCONNECT_SERVER:
			{
				//pCmdUI->Enable((GetUserConnectCmdFlag()&EUCCF_ConnectDataServer));
				pCmdUI->Enable(bConnectedData);
			}
			break;
		case ID_CONNECT_NEWSSERVER:
			{
				//pCmdUI->Enable(!(GetUserConnectCmdFlag()&EUCCF_ConnectNewsServer));
				pCmdUI->Enable(!bConnectedNews);
			}
			break;
		case ID_DISCONNECT_NEWSSERVER:
			{
				//pCmdUI->Enable((GetUserConnectCmdFlag()&EUCCF_ConnectNewsServer));
				pCmdUI->Enable(bConnectedNews);
			}
			break;
		}
	}
}

void CMainFrame::OnIWantBuy()
{
	// 我要购买网站
}

void CMainFrame::OnArbitrageF5( const CArbitrage &arb2, CIoViewBase *pIoViewSrc/*=NULL*/ )
{
	if ( NULL == pIoViewSrc )
	{
		pIoViewSrc = FindActiveIoView();
	}

	UINT nIdPic = ID_PIC_KLINEARBITRAGE;	// 默认套利K
	CArbitrage arb = arb2;
	if ( NULL != pIoViewSrc )
	{
		const T_IoViewObject *pOb = CIoViewManager::FindIoViewObjectByIoViewPtr(pIoViewSrc);
		if ( NULL != pOb )
		{
			switch (pOb->m_uID)
			{
			case ID_PIC_KLINEARBITRAGE:
				{
					nIdPic = ID_PIC_TRENDARBITRAGE;
					if ( arb.m_MerchA.m_pMerch == NULL )
					{
						CIoViewKLineArbitrage *pArbKline = (CIoViewKLineArbitrage *)pIoViewSrc;
						pArbKline->GetArbitrage(arb);
					}
				}
				break;
			case ID_PIC_TRENDARBITRAGE:
				{
					nIdPic = ID_PIC_KLINEARBITRAGE;
					if ( arb.m_MerchA.m_pMerch == NULL )
					{
						CIoViewTrendArbitrage *pArbTrend = (CIoViewTrendArbitrage *)pIoViewSrc;
						pArbTrend->GetArbitrage(arb);
					}
				}
				break;
			case ID_PIC_REPORT_ARBITRAGE:
				{
					if ( arb.m_MerchA.m_pMerch == NULL )
					{
						CIoViewReportArbitrage *pArbReport = (CIoViewReportArbitrage *)pIoViewSrc;
						CArbitrage *pArb = pArbReport->GetCurrentSelArbitrage();
						if ( NULL != pArb )
						{
							arb = *pArb;
						}
					}
				}
				break;
			}
		}
	}

	OnArbitrageF5(nIdPic, arb, pIoViewSrc);
}

void CMainFrame::OnArbitrageF5( UINT nDstArbTypeId, const CArbitrage &arb2, CIoViewBase *pIoViewSrc/*=NULL*/ )
{
	if ( NULL == pIoViewSrc )
	{
		pIoViewSrc = FindActiveIoView();
	}

	UINT nIdPic = ID_PIC_KLINEARBITRAGE;	// 默认套利K
	switch(nDstArbTypeId)
	{
	case ID_PIC_KLINEARBITRAGE:
	case ID_PIC_TRENDARBITRAGE:
	case ID_PIC_REPORT_ARBITRAGE:
		nIdPic = nDstArbTypeId;
		break;
	default:
		//ASSERT(0);
		break;
	}
	CArbitrage arb = arb2;
	if ( NULL != pIoViewSrc )
	{
		const T_IoViewObject *pOb = CIoViewManager::FindIoViewObjectByIoViewPtr(pIoViewSrc);
		if ( NULL != pOb )
		{
			switch (pOb->m_uID)
			{
			case ID_PIC_KLINEARBITRAGE:
				{
					if ( arb.m_MerchA.m_pMerch == NULL )
					{
						CIoViewKLineArbitrage *pArbKline = (CIoViewKLineArbitrage *)pIoViewSrc;
						pArbKline->GetArbitrage(arb);
					}
				}
				break;
			case ID_PIC_TRENDARBITRAGE:
				{
					if ( arb.m_MerchA.m_pMerch == NULL )
					{
						CIoViewTrendArbitrage *pArbTrend = (CIoViewTrendArbitrage *)pIoViewSrc;
						pArbTrend->GetArbitrage(arb);
					}
				}
				break;
			case ID_PIC_REPORT_ARBITRAGE:
				{
					if ( arb.m_MerchA.m_pMerch == NULL )
					{
						CIoViewReportArbitrage *pArbReport = (CIoViewReportArbitrage *)pIoViewSrc;
						CArbitrage *pArb = pArbReport->GetCurrentSelArbitrage();
						if ( NULL != pArb )
						{
							arb = *pArb;
						}
					}
				}
				break;
			}
		}
	}

	CMPIChildFrame *pFrame = NULL;
	CIoViewBase *pIoViewDst = NULL;
	if ( NULL != pIoViewSrc )
	{
		pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, pIoViewSrc->GetParentFrame());
		pIoViewDst = FindIoViewInFrame(nIdPic, pIoViewSrc->GetIoViewGroupId());
	}
	if ( NULL == pFrame )
	{
		pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, MDIGetActive());
		pIoViewDst = FindIoViewInFrame(nIdPic, pFrame);
	}

	if ( NULL == pIoViewDst )
	{
		// 没有指定的视图，那必须加载系统页面了
		pFrame = LoadSystemDefaultCfmRetFrame(nIdPic);
		if ( NULL != pFrame )
		{
			pIoViewDst = FindIoViewInFrame(nIdPic, pFrame);
		}
	}
	if ( NULL != pIoViewDst )
	{
		if ( ID_PIC_KLINEARBITRAGE == nIdPic )
		{
			CIoViewKLineArbitrage *pArb = DYNAMIC_DOWNCAST(CIoViewKLineArbitrage, pIoViewDst);
			ASSERT( NULL != pArb );
			pArb->SetArbitrage(arb);
		}
		else if ( ID_PIC_TRENDARBITRAGE == nIdPic )
		{
			CIoViewTrendArbitrage *pArb = DYNAMIC_DOWNCAST(CIoViewTrendArbitrage, pIoViewDst);
			ASSERT( NULL != pArb );
			pArb->SetArbitrage(arb);
		}
		else
		{
			ASSERT( 0 );
		}

		pIoViewDst->BringToTop();
	}
}

bool32 CMainFrame::OnArbitrageEscReturn( const CArbitrage &arb1, CIoViewBase *pIoViewSrc/*=NULL*/ )
{
	if ( NULL == pIoViewSrc )
	{
		pIoViewSrc = FindActiveIoView();
	}

	UINT nIdPic = ID_PIC_REPORT_ARBITRAGE;
	// 打开套利报价表

	// 优先使用指定的frame，没有则尝试激活frame
	CMPIChildFrame *pFrame = NULL;
	if ( NULL != pIoViewSrc )
	{
		pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, pIoViewSrc->GetParentFrame());
	}
	if ( NULL == pFrame )
	{
		pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, MDIGetActive());
	}

	CIoViewBase *pIoViewDst = NULL;
	pIoViewDst = FindIoViewInFrame(nIdPic, pFrame);
	if ( NULL == pIoViewDst )
	{
		// 没有指定的视图，那必须加载系统页面了
		pFrame = LoadSystemDefaultCfmRetFrame(nIdPic);
		if ( NULL != pFrame )
		{
			pIoViewDst = FindIoViewInFrame(nIdPic, pFrame);
		}
	}
	if ( NULL != pIoViewDst )
	{
		if ( pIoViewDst->IsKindOf(RUNTIME_CLASS(CIoViewReportArbitrage)) )
		{
			CIoViewReportArbitrage *pArbReport = (CIoViewReportArbitrage *)pIoViewDst;
			if ( NULL != arb1.m_MerchA.m_pMerch )
			{
				pArbReport->SetCurrentSelArbitrage(arb1);
			}
		}
		else
		{
			ASSERT( 0 );
		}

		pIoViewDst->BringToTop();
	}

	return NULL != pIoViewDst;
}


// dwDockBarMap
const DWORD dwMyDockBarMap[4][2] =
{
	{ AFX_IDW_DOCKBAR_TOP,      CBRS_TOP    },
	{ AFX_IDW_DOCKBAR_BOTTOM,   CBRS_BOTTOM },
	{ AFX_IDW_DOCKBAR_LEFT,     CBRS_LEFT   },
	{ AFX_IDW_DOCKBAR_RIGHT,    CBRS_RIGHT  },
};

/*
if ( sizeof(CMyDockBar) == sizeof(CDockBar) )
error 506: (Warning -- Constant value Boolean)
*/
//lint -e{506}
void CMainFrame::EnableMyDocking( DWORD dwDockStyle )
{
	// must be CBRS_ALIGN_XXX or CBRS_FLOAT_MULTI only
	ASSERT((dwDockStyle & ~(CBRS_ALIGN_ANY|CBRS_FLOAT_MULTI)) == 0);

	m_pFloatingFrameClass = RUNTIME_CLASS(CMiniDockFrameWnd);
	if ( sizeof(CMyDockBar) == sizeof(CDockBar) )
	{
		m_pFloatingFrameClass = RUNTIME_CLASS(CMyMiniDockFrameWnd);
	}
	for (int i = 0; i < 4; i++)
	{
		if (dwMyDockBarMap[i][1] & dwDockStyle & CBRS_ALIGN_ANY)
		{
			CDockBar* pDock = (CDockBar*)GetControlBar(dwMyDockBarMap[i][0]);
			if (pDock == NULL)
			{
				pDock = new CMyDockBar;
				if (!pDock->Create(this,
					WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_CHILD|WS_VISIBLE |
					dwMyDockBarMap[i][1], dwMyDockBarMap[i][0]))
				{
					AfxThrowResourceException();
				}
			}
		}
	}
}

void CMainFrame::AddDockSubWnd( CWnd *pWnd )
{
	ASSERT( NULL!=pWnd );
	ASSERT( NULL!=m_pMyControlBar );
	if ( !IsExistDockSubWnd(pWnd) )
	{
		m_pMyControlBar->AddWin(pWnd);
	}
}

void CMainFrame::RemoveDockSubWnd( CWnd *pWnd )
{
	ASSERT( NULL!=pWnd );
	ASSERT( NULL!=m_pMyControlBar );
	CWnd *pPreWnd = m_pMyControlBar->GetCurWin();
	m_pMyControlBar->RemoveWin(pWnd);
	CWnd *pCurWnd = m_pMyControlBar->GetCurWin();
	if ( pCurWnd != pPreWnd )
	{
		ShowControlBar(m_pMyControlBar, FALSE, TRUE);
	}
}

void CMainFrame::ShowDockSubWnd( CWnd *pWnd )
{
	ASSERT( NULL!=m_pMyControlBar );
	if ( NULL != pWnd )
	{
		int iCur = m_pMyControlBar->SetCurWin(pWnd);
		if ( -1 != iCur )
		{
			ShowControlBar(m_pMyControlBar, TRUE, FALSE);
			m_pMyControlBar->RedrawWindow();
			return;
		}
	}
	// 如果是没有的或者空窗口，隐藏
	ShowControlBar(m_pMyControlBar, FALSE, TRUE);
	CMDIChildWnd *pFrameActive = MDIGetActive();
	if ( NULL!=pFrameActive )
	{
		pFrameActive->SetFocus();
		pFrameActive->SetActiveWindow();
	}
}

BOOL CMainFrame::IsExistDockSubWnd( CWnd *pWnd )
{
	ASSERT( NULL!=pWnd );
	ASSERT( NULL!=m_pMyControlBar );
	CMyContainerBar::WndPtrArray aWnds;
	m_pMyControlBar->GetSubWndPtrs(aWnds);
	for ( int i=0; i < aWnds.GetSize() ; ++i )
	{
		if ( aWnds[i] == pWnd )
		{
			return TRUE;
		}
	}
	return FALSE;
}

void CMainFrame::DoToggleMyDock()
{
	ASSERT( NULL!=m_pMyControlBar );
	CWnd *pCurWnd = m_pMyControlBar->GetCurWin();
	if ( NULL!=pCurWnd )
	{
		m_pMyControlBar->ToggleMyDock();
	}
}

bool CMainFrame::HideAllTradeWnd()
{
	bool bSimulateTradeShow = m_pSimulateTrade->IsWindowVisible() && !m_pSimulateTrade->IsMinimized();
	if ( bSimulateTradeShow )
	{
		ShowDockSubWnd(NULL);
		if(ETLS_Logined!=m_pSimulateTrade->m_eLoginStatus)//未登陆(应该进不来)
		{	
			m_pSimulateTrade->m_pDlgLogin->ShowWindow(SW_HIDE);
			m_pSimulateTrade->m_pDlgLogin->SetFocus();	
		}
		return true;
	}

	return false;
}

void CMainFrame::OnSimulateTradeShow()
{
	bool bTradeShow = m_pSimulateTrade->IsWindowVisible() && !m_pSimulateTrade->IsMinimized();
	if ( bTradeShow )
	{
		ShowDockSubWnd(NULL);
		if(ETLS_Logined!=m_pSimulateTrade->m_eLoginStatus)//未登陆(应该进不来)
		{	
			m_pSimulateTrade->m_pDlgLogin->ShowWindow(SW_HIDE);
			m_pSimulateTrade->m_pDlgLogin->SetFocus();
		}		
	}
	else
	{
		if(ETLS_Logined!=m_pSimulateTrade->m_eLoginStatus)//未登陆
		{
			ShowDockSubWnd(NULL);
			if(m_pSimulateTrade->m_pDlgLogin->IsWindowVisible())
			{
				m_pSimulateTrade->m_pDlgLogin->ShowWindow(SW_HIDE);
			}
			else
			{
				m_pSimulateTrade->TryLogin();
				m_pSimulateTrade->m_pDlgLogin->ShowWindow(SW_SHOW);
				m_pSimulateTrade->m_pDlgLogin->SetFocus();
			}
		}
		else
		{
			m_pSimulateTrade->m_bWndMaxMinStatus = FALSE;			//如果现在最小化的话也设置最大化
			m_pSimulateTrade->WndMaxMin();
			m_pSimulateTrade->m_pDlgTradeBidParent->SetDefMaxMin();	//界面恢复成显示状态
			m_pSimulateTrade->TryLogin();
			m_pMyControlBar->SetNewSize(m_pMyControlBar->GetSizeDefault());	//显示时候需要设置默认高度
			//		ShowDockSubWnd(NULL);  // 隐藏可能处于显示的子窗口
			ShowDockSubWnd(m_pSimulateTrade);

			CRect rcWnd;
			m_pSimulateTrade->m_pDlgTradeBidParent->GetWindowRect(&rcWnd);
			m_pSimulateTrade->m_pDlgTradeBidParent->OnSize(SIZE_RESTORED, rcWnd.Width(), rcWnd.Height());
			m_pSimulateTrade->m_pDlgTradeBidParent->m_dlgQuery.RecalcLayout(true);
		}
	}
}

void CMainFrame::OnFirmTradeShow()
{
	// lcq add
	bool bTradeShow = m_pTradeContainer->IsWindowVisible() && !m_pTradeContainer->IsMinimized();
	if ( bTradeShow )
	{
		ShowDockSubWnd(NULL);
		if(ETLS_Logined!=m_pTradeContainer->m_eLoginStatus)//未登陆(应该进不来)
		{
			//	m_pTradeContainer->TryLogin();	
			m_pTradeContainer->m_pDlgLogin->ShowWindow(SW_HIDE);
			m_pTradeContainer->m_pDlgLogin->SetFocus();
		}		
	}
	else
	{
		if(ETLS_Logined!=m_pTradeContainer->m_eLoginStatus)//未登陆
		{
			ShowDockSubWnd(NULL);
			if(m_pTradeContainer->m_pDlgLogin->IsWindowVisible())
			{
				m_pTradeContainer->m_pDlgLogin->ShowWindow(SW_HIDE);
			}
			else
			{
				m_pTradeContainer->TryLogin();
				m_pTradeContainer->m_pDlgLogin->ShowWindow(SW_SHOW);
				m_pTradeContainer->m_pDlgLogin->SetFocus();
			}
		}
		else
		{
			if (m_bQuickOrderDn)
			{
				m_bQuickOrderDn = FALSE;
				m_pTradeContainer->m_pDlgLogin->ShowWindow(SW_HIDE); 

				CIoViewBase* p = FindActiveIoView();
				if (NULL != p)
				{
					OnQuickTrade(2, p->GetMerchXml());
				}	
				return;
			}
			m_pTradeContainer->m_bWndMaxMinStatus = FALSE;	//如果现在最小化的话也设置最大化
			m_pTradeContainer->WndMaxMin();
			m_pTradeContainer->m_pDlgTradeBidParent->SetDefMaxMin();	//界面恢复成显示状态
			m_pTradeContainer->TryLogin();
			m_pMyControlBar->SetNewSize(m_pMyControlBar->GetSizeDefault());	//显示时候需要设置默认高度
			//	ShowDockSubWnd(NULL);  // 隐藏可能处于显示的子窗口
			ShowDockSubWnd(m_pTradeContainer);

			CRect rcWnd;
			m_pTradeContainer->m_pDlgTradeBidParent->GetWindowRect(&rcWnd);
			m_pTradeContainer->m_pDlgTradeBidParent->OnSize(SIZE_RESTORED, rcWnd.Width(), rcWnd.Height());
			m_pTradeContainer->m_pDlgTradeBidParent->m_dlgQuery.RecalcLayout(true);
		}
	}
}

void CMainFrame::OnSimulateOpenAccount()
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if (NULL == pApp)
	{
		return;
	}

	bool32 bShowIE = FALSE;
	int32 iWidth = 1024, iHeight = 750;
	int32 iSize = pApp->m_pConfigInfo->m_aWndSize.GetSize();
	CString StrName = L"";
	for (int32 i=0; i<iSize; i++)
	{
		CWndSize wnd = pApp->m_pConfigInfo->m_aWndSize[i];
		if (L"开户" == wnd.m_strID)
		{
			StrName = wnd.m_strTitle;
			iWidth = wnd.m_iWidth;
			iHeight = wnd.m_iHeight;

			if (0 == iWidth || 0 == iHeight)
			{
				bShowIE = TRUE;
			}

			break;
		}
	}

	if (StrName.IsEmpty())
	{
		StrName = pApp->m_pConfigInfo->m_StrBtName;
	}

	UrlParser urlParser(L"alias://openaccount");
	if (bShowIE)
	{
		ShellExecute(0, L"open", urlParser.GetUrl(), NULL, NULL, SW_NORMAL);
	}
	else
	{
		CDlgTodayCommentIE::ShowDlgIEWithSize(StrName, urlParser.GetUrl(), CRect(0, 0, iWidth, iHeight));
	}
}

void CMainFrame::OnSimulateHQRegister()
{
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();

	// 游客登录的时候才显示
	if(pDoc->m_isDefaultUser) 
	{
		ShowRegisterDialog();
	}
}

void CMainFrame::OnSimulateHQLogin()
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	pApp->m_bTradeExit = true;
	OnClose();
}

LRESULT	CMainFrame::OnSimulateImageUrl(WPARAM wParam, LPARAM lParam)
{
	CString* pStrVal = (CString*)wParam;
	CString strUrl = L"TradeLogin";
	if(NULL != pStrVal)
	{
		strUrl =  *pStrVal;
	}

	if(strUrl.IsEmpty())
	{
		return 0;
	}

	if(strUrl == L"TradeLogin")
	{
		OnSimulateTradeShow();
	}
	else if(strUrl.Find(L"http") != -1)
	{
		CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
		if (NULL == pApp)
		{
			return 0;
		}
		CDlgTodayCommentIE::ShowDlgIEWithSize(pApp->m_pszAppName, strUrl, CRect(0,0,1024,750));
	}

	return 0;
}

void CMainFrame::OnHQRegisterSucced()
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	if(NULL == pApp)
	{
		return;
	}

	pApp->m_HQRegisterExit = true;
	OnClose();
}

// js重新登录pc客户端处理
LRESULT	CMainFrame::OnWebReLoginPC(WPARAM wParam, LPARAM lParam)
{
	CString* pStrVal = (CString*)wParam;
	CString StrUserName = _T("");
	if(NULL != pStrVal)
	{
		StrUserName =  *pStrVal;
	}

	// 用户名不为空的时候，保存用户名处理
	if(!StrUserName.IsEmpty())
	{

	}

	// 退出程序
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	if(NULL != pApp)
	{
		m_bShowLoginDlg = true;
		PostMessage(WM_CLOSE);
	}

	return 0;
}

// js个人中心返回信息处理
LRESULT	CMainFrame::OnWebPersonCenterRsp(WPARAM wParam, LPARAM lParam)
{
	CString* pStrVal = (CString*)wParam;
	CString StrNickName = L"";
	if(NULL != pStrVal)
	{
		StrNickName =  *pStrVal;
	}

	if (StrNickName.GetLength() > 4)
	{
		StrNickName = StrNickName.Left(4);
		StrNickName += "...";
	}

	m_mapMenuBtn[ID_MENU_USER_NAME].SetCaption(StrNickName);
	_tcscpy(m_stUserInfo.wszNickName, StrNickName.GetString());
	DrawCustomNcClient();
	return TRUE;
}





void PackJsonType(string &strTransDataUtf8)
{
	Json::Value jsValue;
	CString strToken;
	string strFieldTemp;


	jsValue.clear();
	jsValue["id"]		= "111221212";

	Json::Value jsPayloadValue;
	jsPayloadValue["name"]		= "test";
	jsPayloadValue["params"]	= "22222222222222222222";
	jsValue["payload"] = jsPayloadValue;



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

// web命令请求
LRESULT	CMainFrame::OnWebCommand(WPARAM wParam, LPARAM lParam)
{
	CString* pStrVal = (CString*)wParam;
	CString StrNickName = L"";
	if(NULL == pStrVal)
	{
		return FALSE;
	}
	StrNickName =  *pStrVal;
	CAnalysisWeb cAnalysisWeb;
	cAnalysisWeb.AnalysisJsonCmd(StrNickName);
	if (CAnalysisWeb::COM_OPEN_WEB_PAGE_HTTP == cAnalysisWeb.m_enumComType ||
		CAnalysisWeb::COM_OPEN_WEB_PAGE_HTTPS == cAnalysisWeb.m_enumComType)
	{
		WebComOpenWebPage(cAnalysisWeb);
	}
	else if (CAnalysisWeb::COM_OPEN_LOCAL_FILE == cAnalysisWeb.m_enumComType)
	{
		WebComOpenLocalFile(cAnalysisWeb);
	}
	else if (CAnalysisWeb::COM_OPEN_VIEW == cAnalysisWeb.m_enumComType)
	{
		WebComOpenView(cAnalysisWeb);
	}
	else if (CAnalysisWeb::COM_OPEN_LOCAL_APP == cAnalysisWeb.m_enumComType)
	{
		WebComOpenLocalApp(cAnalysisWeb);
	}
	else if (CAnalysisWeb::COM_OPEN_PASS_MSG == cAnalysisWeb.m_enumComType)
	{
		WebComOpenPassMsg(cAnalysisWeb);
	}
	
	return TRUE;
}


// web回调
LRESULT	CMainFrame::OnWebCallBack(WPARAM wParam, LPARAM lParam)
{
	CString* pStrVal = (CString*)wParam;
	CString StrNickName = L"";
	if(NULL == pStrVal)
	{
		return FALSE;
	}
	StrNickName =  *pStrVal;

	MessageBox(StrNickName, NULL, MB_OK);
	return TRUE;
}


void CMainFrame::WebComOpenWebPage(CAnalysisWeb cAnalysisWeb)
{
	CIoViewNews *pIoviewNes = FindIoViewNews(TRUE);
	if (pIoviewNes)
	{
		UrlParser uParser(cAnalysisWeb.m_strCommand);
		if(cAnalysisWeb.m_multimapParamsInfo.size() > 0)
		{
			multimap<CString,CString>::iterator iter;
			for (iter = cAnalysisWeb.m_multimapParamsInfo.begin(); iter != cAnalysisWeb.m_multimapParamsInfo.end(); iter++)
			{
				pIoviewNes->SetCustomUrlQueryValue(iter->first, iter->second);
				uParser.SetQueryValue(iter->first, iter->second);
			}
		}
	//	pIoviewNes->RefreshWeb(uParser.GetUrl());
		string strPack ;
		PackJsonType(strPack);

	//	CStringA strResult = cAnalysisWeb.excuteJSCallbackPackJsonCmd();
		CStringA strResult = cAnalysisWeb.excuteJS111();
		pIoviewNes->FrameExecuteJavaScript(strResult.GetBuffer());	
	}
}

void CMainFrame::WebComOpenLocalFile(CAnalysisWeb cAnalysisWeb)
{
	
}


void CMainFrame::WebComOpenView(CAnalysisWeb cAnalysisWeb)
{

}

void CMainFrame::WebComOpenLocalApp(CAnalysisWeb cAnalysisWeb)
{

}


void CMainFrame::WebComOpenPassMsg(CAnalysisWeb cAnalysisWeb)
{

}





// js打开指定版面处理
LRESULT	CMainFrame::OnWebOpenCfm(WPARAM wParam, LPARAM lParam)
{
	T_WebOpenCfm *pstParam = (T_WebOpenCfm*)wParam;
	if(NULL == pstParam)
	{
		return 0;
	}

	m_stWebOpenCfm = *pstParam;
	EngineCenterFree(pstParam);

	// 用户行为记录
	AddToUserDealRecord(m_stWebOpenCfm.StrCfmName, EPTFun);

	if (NULL != m_pNewWndTB)
	{
		if (L"个股资讯" == m_stWebOpenCfm.StrCfmName)		// 个股资讯相关处理
		{
			OpenWebWorkspace(m_stWebOpenCfm.StrCfmName, m_stWebOpenCfm.StrFlag);
		}
		else
		{
			m_pNewWndTB->DoOpenCfm(m_stWebOpenCfm.StrCfmName);
		}
	}

	return 0;
}



// js打开分时
LRESULT	CMainFrame::OnWebOpenTrend(WPARAM wParam, LPARAM lParam)
{
	T_WebOpenTrend *pstParam = (T_WebOpenTrend*)wParam;
	if(NULL == pstParam)
	{
		return 0;
	}

	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	if (!pApp)
	{
		return -1;	
	}
	CGGTongDoc *pDoc = pApp->m_pDocument;
	if (!pDoc)
	{
		return -1;
	}
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return -1;
	}

	// 查找商品，并跳转分时页面
	CMerch *pMerch = NULL;
	if (!pAbsCenterManager->GetMerchManager().FindMerch(pstParam->strCode, pstParam->iMarketId, pMerch))
	{
		return -1;
	}


	LoadSystemDefaultCfm(ID_PIC_TREND, pMerch);

	EngineCenterFree(pstParam);

	m_leftToolBar.ChangeLeftBarStatus("");
	return 0;
}


// 点击网页里的链接在外面浏览器打开
LRESULT	CMainFrame::OnOpenOutWeb(WPARAM wParam, LPARAM lParam)
{
	CString* pStrVal = (CString*)wParam;
	CString StrUrl = L"";
	if(NULL != pStrVal)
	{
		StrUrl =  *pStrVal;
	}
	else
	{
		return 0;
	}

	ShellExecute(0, L"open", StrUrl, NULL, NULL, SW_NORMAL);

	return 0;
}

LRESULT	CMainFrame::OnShowWebDlg(WPARAM wParam, LPARAM lParam)
{
	T_ShowWebDlg *pstParam = (T_ShowWebDlg*)wParam;
	if(NULL == pstParam)
	{
		return 0;
	}

	int32 iWidth = _ttoi(pstParam->StrWidth) + 6;		// 加上边框
	int32 iHeight = _ttoi(pstParam->StrHeight) + 33;	// 加上标题栏和边框
	m_pDlgComment->ShowDlgIEWithSize(L"", pstParam->StrUrl, CRect(0, 0, iWidth, iHeight));

	EngineCenterFree(pstParam);
	return 0;
}

void CMainFrame::AddGGTongView(CGGTongView* p)
{
	if ( NULL == p )
	{
		return;
	}

	//
	m_aGGTongViews.Add(p);
}

void CMainFrame::DelGGTongView(CGGTongView* p)
{
	for ( int32 i = m_aGGTongViews.GetSize() - 1; i >= 0; i-- )
	{
		if ( m_aGGTongViews.GetAt(i) == p )
		{
			m_aGGTongViews.RemoveAt(i);
		}
	}
}


void CMainFrame::AddIoView(CIoViewBase* pViewBase)
{
	if (NULL == pViewBase)
		return;

	bool32 bFind = FALSE;
	for (int32 i = 0; i < m_IoViewsPtr.GetSize(); i++)
	{
		if (m_IoViewsPtr[i] == pViewBase)
		{
			bFind = TRUE;
			break;
		}
	}

	if (!bFind)
	{
		m_IoViewsPtr.Add(pViewBase);
	}
}

void CMainFrame::RemoveIoView(CIoViewBase* pViewBase)
{
	if (NULL == pViewBase)
		return;

	// 找到所有的, 剔除（可能有重复的情况）
	while (1)
	{
		int32 iFindPos = -1;
		for (int32 i = 0; i < m_IoViewsPtr.GetSize(); i++)
		{
			if (m_IoViewsPtr[i] == pViewBase)
			{
				iFindPos = i;
				break;
			}
		}

		if (iFindPos >= 0)
		{
			m_IoViewsPtr.RemoveAt(iFindPos, 1);
		}
		else
		{
			break;
		}
	}
}


void CMainFrame::GetAllGGTongViews(OUT CArray<CGGTongView*, CGGTongView*>& aGGTongViews)
{
	aGGTongViews.Copy(m_aGGTongViews);
}


LRESULT CMainFrame::OnMsgViewDataOnNewsTitleUpdate(WPARAM wParam, LPARAM lParam) //财富资讯2013
{

	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	//
	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	listNewsInfo* tmpNewsInfo = (listNewsInfo*)lParam;
	ASSERT(NULL!= tmpNewsInfo);

	//金评状态栏添加当天金评
	m_wndCommentSBar.AddBatchComment(*tmpNewsInfo);

	int newsTypeID = (int)wParam;
	if (newsTypeID == pAbsCenterManager->GetJinpingID())
	{
		//广告框添加特殊今评
		if ( m_pDlgAdvertise ) 
		{
			m_pDlgAdvertise->AddBatchComment(*tmpNewsInfo);
		}
	}

	return 0;
}

LRESULT CMainFrame::OnMsgViewDataOnNewsContentUpdate(WPARAM wParam, LPARAM lParam)
{


	T_NewsInfo* tmpNewsInfo = (T_NewsInfo*)wParam;
	ASSERT(NULL!= tmpNewsInfo);

	m_wndCommentSBar.OnVDataNewsContentUpdate(*tmpNewsInfo);
	if ( m_pDlgAdvertise )
	{
		m_pDlgAdvertise->OnVDataNewsContentUpdate(*tmpNewsInfo);
	}

	return 0;
}

LRESULT CMainFrame::OnMsgViewDataOnNewsPushTitle(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	//
	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	T_NewsInfo* tmpNewsInfo = (T_NewsInfo*)lParam;
	ASSERT(NULL!= tmpNewsInfo);

	//金评状态栏添加当天金评
	m_wndCommentSBar.InsertAtComment(*tmpNewsInfo);

	int newsTypeID = (int)wParam;
	if (newsTypeID == pAbsCenterManager->GetJinpingID())
	{
		pAbsCenterManager->SetTodayFlag(HasValidViewTextNews());

		//广告框添加特殊今评
		if ( m_pDlgAdvertise ) 
		{
			m_pDlgAdvertise->InsertAtComment(*tmpNewsInfo);
		}
	}

	return 0;
}





LRESULT CMainFrame::OnMsgViewDataOnCodeIndexUpdate(WPARAM wParam, LPARAM lParam)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	CString StrParam;
	CString* pStr = (CString*)wParam;
	if ( NULL != pStr )
	{
		StrParam = *pStr;
		DEL(pStr);
	}

	//
	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	pAbsCenterManager->OnMsgCodeIndexUpdate(StrParam);

	return 0;
}

LRESULT CMainFrame::OnShowRightTip(WPARAM wParam, LPARAM lParam)
{
	T_RightPromptCode *pstRightTip = (T_RightPromptCode *)wParam;

	if (NULL != pstRightTip)
	{
		CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
		CGGTongDoc* pDoc = pApp->m_pDocument;

		// 游客账号，没权限显示注册对话框
		if(pDoc->m_isDefaultUser) 
		{
			EngineCenterFree(pstRightTip);
			ShowRegisterDialog();
			return 0;
		}

		// 找到了权限提示信息
		if (pstRightTip->StrMsgTip.IsEmpty())
		{
			CString StrTitle = L"温馨提示";//pstRightTip->StrTitle;
			int32	iWidth = 1024, iHeight = 700;
			CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
			if (NULL != pApp)
			{
				int32 iSize = pApp->m_pConfigInfo->m_aWndSize.GetSize();
				for (int32 i=0; i<iSize; i++)
				{
					CWndSize wnd = pApp->m_pConfigInfo->m_aWndSize[i];
					if (_T("权限提示") == wnd.m_strID)
					{
						iWidth = wnd.m_iWidth;
						iHeight = wnd.m_iHeight;
						break;
					}
				}
			}

			if (!pstRightTip->StrLinkUrl.IsEmpty() && pstRightTip->StrImgUrl.IsEmpty())
			{
				OpenWebWorkspace(L"温馨提示", pstRightTip->StrLinkUrl);
			}
			else if (pstRightTip->StrImgUrl.IsEmpty())
			{
				//无权限提示时，把该功能的名称也带入RUL串里，以背他用 BY HX
				CString strRightName = CPluginFuncRight::Instance().GetUserRightName(pstRightTip->iRightCode);
				UrlParser uParser(pstRightTip->StrLinkUrl);
				uParser.SetQueryValue(_T("func"),strRightName);
				m_pDlgComment->ShowDlgIEWithSize(StrTitle, uParser.GetUrl(), CRect(0,0,iWidth,iHeight));
			}
			else
			{
				m_pDlgComment->ShowDlgIEWithSize(StrTitle, pstRightTip->StrImgUrl, CRect(0,0,iWidth,iHeight));
			}
		}
		else
		{
			// 没有找到权限提示信息
			MessageBox(pstRightTip->StrMsgTip, _T("温馨提示"), MB_OK|MB_ICONWARNING);
		}
	}

	EngineCenterFree(pstRightTip);

	return 0;
}

void CMainFrame::AddToUserDealRecord(CString StrFunName, E_ProductType eType)
{
	// 游客或者开关为false，不记录
	CGGTongApp *pApp = (CGGTongApp*) AfxGetApp();	
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if(pDoc->m_isDefaultUser || !pApp->m_pConfigInfo->m_stUserDealRecord.bRecord) 
	{
		return;
	}

	if (0 != pApp->m_pConfigInfo->m_stUserDealRecord.iType)
	{
		if (eType != pApp->m_pConfigInfo->m_stUserDealRecord.iType)
		{
			return;
		}
	}

	int32 iCnt = m_vUserDealRecord.size();
	bool32 bFind = false;
	for (int32 i=0; i<iCnt; i++)
	{
		T_UserDealRecord &stRecord = m_vUserDealRecord[i];
		CString StrCurrName = stRecord.wszFunName;
		if ( (StrCurrName==StrFunName) && (eType==stRecord.eProductType))
		{
			bFind = true;
			stRecord.iViewCnt++;
			break;
		}
	}

	if (!bFind)
	{
		T_UserDealRecord stRecord;	
		memcpy(stRecord.wszFunName, StrFunName.GetBuffer(), (StrFunName.GetLength() + 1) * sizeof(TCHAR));
		stRecord.eProductType = eType;
		stRecord.iViewCnt = 1;
		m_vUserDealRecord.push_back(stRecord);
	}
}

LRESULT CMainFrame::OnAddUserDealRecord(WPARAM wParam, LPARAM lParam)
{
	CString* pStrVal = (CString*)wParam;
	CString StrFunName = _T("");
	if(NULL != pStrVal)
	{
		StrFunName =  *pStrVal;
		EngineCenterFree(pStrVal);
	}
	else
	{
		return 0;
	}

	AddToUserDealRecord(StrFunName, EPTFun);
	return 0;
}

LRESULT CMainFrame::OnIsUserHasIndexRight(WPARAM wParam, LPARAM lParam)
{
	CString* pStrIndexName = (CString*)wParam;
	bool32 bShowTip = (bool32)lParam;

	CString StrRight;
	bool32 b = CEtcXmlConfig::Instance().ReadEtcConfig(_T("UserRightIndexName"), *pStrIndexName, NULL, StrRight);
	EngineCenterFree(pStrIndexName);
	if ( b )
	{
		return CPluginFuncRight::Instance().IsUserHasRight(StrRight, bShowTip);
	}

	return TRUE; // 如果没有配置要求权限，则默认是有权限打开该指标的
}


LRESULT CMainFrame::OnJumpToTradingSoftware(WPARAM wParam, LPARAM lParam)
{
	ShowRealTradeDlg();
	return TRUE;
}


void CMainFrame::OnRestoreWorkSpace()
{
	CDlgRestoreWkSpace dlg;
	if( dlg.DoModal() == IDOK )
	{
		//恢复当前打开的屏幕
		if( !dlg.m_CurrentCfmInfo.m_StrXmlName.IsEmpty())
		{
			OnOpenChildframeFromWsp( dlg.m_CurrentCfmInfo.m_StrXmlName,true);
		}
	}
}

//lxp 增加  快捷方式打开屏幕
void CMainFrame::OnOpenChildframeFromWsp(CString strXmlName,bool bOpenCur)
{
	ASSERT( !strXmlName.IsEmpty() );
	if( strXmlName.IsEmpty())
		return;

	CMPIChildFrame *pChildFrame = CCfmManager::Instance().GetCfmFrame(strXmlName);
	if ( NULL != pChildFrame && !bOpenCur)
	{
		//CString StrPrompt;
		//StrPrompt.Format(CLanguage::GetInistance().TranslateText(_T("MainFrm_5221_0"),_T("您已经打开了页面: %s\r\n是否重新加载？")), strXmlName);
		//if ( AfxMessageBox(StrPrompt, MB_YESNO | MB_ICONQUESTION |MB_DEFBUTTON2 ) == IDNO )
		{
			MDIActivate(pChildFrame);
			return;
		}
	}
	CCfmManager::Instance().LoadCfm(strXmlName, true);	// 重新打开
}

void CMainFrame::OnShowImMessage()
{
	m_pDlgIm->OpenMessageWindow();
}

void CMainFrame::ControlDisStatus(bool bIsDis)
{
	CIoViewNews *pIoViewNews = NULL;
	CMPIChildFrame *pActiveFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, MDIGetActive());
	if (NULL != pActiveFrame)
	{
		if(pActiveFrame->GetIdString() != L"网页资讯" && m_pNewWndTB)
		{

			bool32 bLoad = m_pNewWndTB->DoOpenCfm(L"网页资讯");
			if ( bLoad )
			{
				pIoViewNews = FindIoViewNews(true);
			}

			if(pIoViewNews)
			{
				pIoViewNews->RefreshWeb(L"alias://pending");
			}
		}
	}
}

///--- 设置开机启动
void CMainFrame::SetAutoRun(BOOL bAutoRun)
{
	HKEY hKey;  
	CString strRegPath = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";  
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDoc = pApp->m_pDocument;
	TCHAR szModule[_MAX_PATH];  
	GetModuleFileName(NULL, szModule, _MAX_PATH);
	CString strModulePath = szModule;
	CString StrExeName = GetMoudleAppName();
	int nLen = _tcslen(szModule);
	if (bAutoRun)  
	{   
		if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, strRegPath, 0, KEY_ALL_ACCESS, &hKey))       
		{  
			RegSetValueEx(hKey,StrExeName, 0, REG_SZ, (const BYTE*)(LPCSTR)szModule, sizeof(TCHAR)*nLen); 
			RegCloseKey(hKey); 
			pDoc->m_bAutoRun = TRUE;
		}  
		else  
		{    
			MessageBox(L"系统参数错误,不能随系统启动",L"提示");     
		}  
	}  
	else  
	{  
		if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, strRegPath, 0, KEY_ALL_ACCESS, &hKey))        
		{  
			RegDeleteValue (hKey,StrExeName);     
			RegCloseKey(hKey);  
			pDoc->m_bAutoRun = FALSE;
		}  
	}  
}

void CMainFrame::GetAutoRunKeyValue()
{
	//	TCHAR    achKey[MAX_PATH];   // buffer for subkey name  
	//	DWORD    cbName;                   // size of name string   
	TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name   
	DWORD    cchClassName = MAX_PATH;  // size of class string   
	DWORD    cSubKeys=0;               // number of subkeys   
	DWORD    cbMaxSubKey;              // longest subkey size   
	DWORD    cchMaxClass;              // longest class string   
	DWORD    cValues;              // number of values for key   
	DWORD    cchMaxValue;          // longest value name   
	DWORD    cbMaxValueData;       // longest value data   
	DWORD    cbSecurityDescriptor; // size of security descriptor   
	FILETIME ftLastWriteTime;      // last write time  

	CString strRegPath = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";  
	CString  StrExeName = GetMoudleAppName();

	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	CGGTongDoc *pDoc = pApp->m_pDocument;	
	ASSERT(NULL!= pDoc);

	DWORD i, dwRet;   
	TCHAR  achValue[MAX_PATH];   
	DWORD cchValue = MAX_PATH;   
	HKEY hKey;

	if(ERROR_SUCCESS == RegOpenKeyEx( HKEY_CURRENT_USER,  
		strRegPath,  
		0,  
		KEY_READ,  
		&hKey)  
		)  
	{  
		// Get the class name and the value count.   
		dwRet = RegQueryInfoKey(  
			hKey,                    // key handle   
			achClass,                // buffer for class name   
			&cchClassName,           // size of class string   
			NULL,                    // reserved   
			&cSubKeys,               // number of subkeys   
			&cbMaxSubKey,            // longest subkey size   
			&cchMaxClass,            // longest class string   
			&cValues,                // number of values for this key   
			&cchMaxValue,            // longest value name   
			&cbMaxValueData,         // longest value data   
			&cbSecurityDescriptor,   // security descriptor   
			&ftLastWriteTime);       // last write time   

		// Enumerate the key values.   

		if (cValues)   
		{  
			for (i=0, dwRet=ERROR_SUCCESS; i<cValues; i++)   
			{   
				cchValue = MAX_PATH;   
				achValue[0] = '\0';   
				dwRet = RegEnumValue(hKey, i,   
					achValue,   
					&cchValue,   
					NULL,   
					NULL,  
					NULL,  
					NULL);  

				if (ERROR_SUCCESS == dwRet && 0 == StrExeName.CompareNoCase(achValue))   
				{   
					pDoc->m_bAutoRun = TRUE;   
				}   
			}  
		}  
	}
	RegCloseKey(hKey); 
}

void CMainFrame::PackUserBlockInfo(string &strUserBlockUtf8)
{
	strUserBlockUtf8.clear();
	// 得到这个板块
	CString StrBlockName = CUserBlockManager::Instance()->GetDefaultServerBlockName();
	T_Block* pBlock	= CUserBlockManager::Instance()->GetBlock(StrBlockName);
	if ( NULL == pBlock )
	{
		return;
	}

	// 得到板块下所有的商品
	CArray<CMerch*, CMerch*> aMerchs;
	if ( !CUserBlockManager::Instance()->GetMerchsInBlock(StrBlockName, aMerchs) )
	{
		return;
	}

	/*if (aMerchs.GetSize() < 1)
	{
		return	
	}*/

	// 填充json字符串
	Json::Value root;
	Json::Value arrayObj;
	Json::Value item;

	root["UserBlockCount"] =  aMerchs.GetSize();

	arrayObj.clear();
	for (int i = 0; i < aMerchs.GetSize(); ++i)
	{
		CMerch *pMerch = aMerchs.GetAt(i);
		if (!pMerch)
			continue;
		string strMerchCode;
		strMerchCode =  CStringA(pMerch->m_MerchInfo.m_StrMerchCode);

		item.clear();
		item["marketid"] = pMerch->m_MerchInfo.m_iMarketId;
		item["merchcode"] = strMerchCode;
		arrayObj.append(item);
	}
	root["UserBlockInfo"] = arrayObj;


	string strWebDisData = root.toStyledString();

	int index = 0;
	if( !strWebDisData.empty())
	{
		while( (index = strWebDisData.find('\n',index)) != string::npos)
		{
			strWebDisData.erase(index,1);
		}
	}

	CString strJson = strWebDisData.c_str();
	strUserBlockUtf8.clear();
	strUserBlockUtf8 = CStringA(strJson);
}


void CMainFrame::AddOrDelUserblockMerch(CString strMerchCode,CString strMarketid, bool bIsDel)
{
	// 获取自选股管理类
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	if (!pApp)
	{
		return;
	}
	CGGTongDoc *pDoc = pApp->m_pDocument;
	if (!pDoc)
	{
		return;
	}
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return;
	}

	CString StrBlockName = CUserBlockManager::Instance()->GetServerBlockName();
	
	if ( !StrBlockName.IsEmpty() && !strMarketid.IsEmpty() && !strMerchCode.IsEmpty())
	{	
		int32 iMarketid = _ttoi(strMarketid);
		CMerch *pMerch = NULL;
		if (!pAbsCenterManager->GetMerchManager().FindMerch(strMerchCode, iMarketid, pMerch))
		{
			return;
		}

		if (bIsDel)
		{
			CUserBlockManager::Instance()->DelMerchFromUserBlock(pMerch, StrBlockName, false);
		}
		else
		{
			CUserBlockManager::Instance()->AddMerchToUserBlock(pMerch, StrBlockName);
		}

		CUserBlockManager::Instance()->SaveXmlFile();
		CUserBlockManager::Instance()->Notify(CSubjectUserBlock::EUBUMerch);
	}
}

void CMainFrame::CreateAdvertise()
{
	// fangz0130 安泰丰的分析师
	CGGTongApp * pApp = (CGGTongApp*)AfxGetApp();
	if ( pApp->m_pConfigInfo->m_bIsVip && NULL == m_pDlgAdvertise )
	{
		m_pDlgAdvertise = new CDlgAdvertise();
		if ( m_pDlgAdvertise )
		{
			m_pDlgAdvertise->Create(IDD_DIALOG_ADVERTISE, this);
			m_pDlgAdvertise->ShowWindow(SW_HIDE);
		}

	}
}

void CMainFrame::SetAdvertiseOwner(CWnd *pWnd)
{	
	if ( m_pDlgAdvertise )
		m_pDlgAdvertise->SetAdvertiseOwner(pWnd);
}

void CMainFrame::RedrawAdvertise()
{
	if ( m_pDlgAdvertise )
	{
		if ( m_pDlgAdvertise->RedrawAdvertise() )
			m_pDlgAdvertise->ShowWindow(SW_NORMAL);
	}
}

void CMainFrame::HideAdvertise()
{
	if ( m_pDlgAdvertise)
	{
		//
		m_pDlgAdvertise->MoveWindow(CRect(0, 0, 0, 0));
		m_pDlgAdvertise->ShowWindow(SW_HIDE);
	}
}

void CMainFrame::NotifyAdvMerchChange()
{	if ( m_pDlgAdvertise )
m_pDlgAdvertise->SetCheckShowTimer();
}

void CMainFrame::SetAdvFont(LOGFONT *pLogFont)
{
	if ( m_pDlgAdvertise )
		m_pDlgAdvertise->SetImageFont(pLogFont);
}


void CMainFrame::OnQuickTrade(int32 iFlag, CMerch* pMerch, CReportScheme::E_ReportHeader eHeader)
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	ASSERT(NULL!= pApp);
	CGGTongDoc *pDoc = pApp->m_pDocument;
	ASSERT(NULL!= pDoc);
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);
	if (!pAbsCenterManager)
	{
		return;
	}

	if (!pApp->m_pConfigInfo->m_bQuickTrade || !pApp->m_pConfigInfo->m_bMember)
	{
		// 交易所版本和外部交易，不允许快手下单
		return; 
	}

	if (!pApp->m_pConfigInfo->m_bQuickTrade || !pApp->m_pConfigInfo->m_bMember)
	{
		// 交易所版本和外部交易，不允许快手下单
		return; 
	}

	// ...fangz 1227 快手下单
	if(NULL==m_pTradeContainer)
	{
		return;
	}

	//不在贵金属界面,隐藏快手下单界面
	if(3==iFlag)
	{
		m_pTradeContainer->m_pDlgTradeBidParent->HideQuickOrder();
		return;
	}

	// 模拟交易不允许快手下单
	bool32 bFlag1 = (ETLS_Logined==m_pSimulateTrade->m_eLoginStatus)&&(ETLS_Logined!=m_pTradeContainer->m_eLoginStatus);
	bool32 bFlag2 = (ETLS_Logined!=m_pSimulateTrade->m_eLoginStatus)&&(ETLS_Logined!=m_pTradeContainer->m_eLoginStatus);
	if(bFlag1 || bFlag2)//未登录
	{
		if(2==iFlag)
		{
			//	MessageBox(_T("交易尚未登录!"),_T("错误"),MB_OK|MB_ICONERROR);
			m_bQuickOrderDn = true;
			OnFirmTradeShow();
		}
		return;
	}
	//  每次显示闪电下单前，从文件中读取checkbox的状态
	m_pTradeContainer->m_pDlgTradeBidParent->GetTradeQuickOrderDlg().ReadCheck();

	if (!m_pTradeContainer->m_pDlgTradeBidParent->GetTradeQuickOrderDlg().SetCommID(m_pTradeContainer->ClientMerch2TradeCode(pMerch)))
	{
		if ((0==iFlag) && !m_pTradeContainer->m_pDlgTradeBidParent->IsVisibleQuickOrder())
		{
			return;
		}
		CString strErr = _T("当前商品不能做交易，商品名称：");
		//strErr.Format(_T("%s"),pMerch->m_MerchInfo.m_StrMerchCnName);
		//strErr += _T(" 不是交易商品");
		CDlgTip dlg;
		//dlg.m_strTipMsg = strErr;
		//dlg.m_eTipType = ETT_WAR;
		//dlg.m_pCenterWnd = this;	// 设置要居中到的窗口指针
		//dlg.DoModal();
		//m_pTradeContainer->m_pDlgTradeBidParent->m_DlgQuickOrder.m_btnGoodsName.SetWindowText(strErr);

		m_pTradeContainer->m_pDlgTradeBidParent->m_DlgQuickOrder.m_btnGoodsName.SetTitleText1(pMerch->m_MerchInfo.m_StrMerchCnName);
		m_pTradeContainer->m_pDlgTradeBidParent->m_DlgQuickOrder.m_btnGoodsName.SetTitleText2(_T(" 不是交易商品"));

		m_pTradeContainer->m_pDlgTradeBidParent->m_DlgQuickOrder.m_btnGoodsName.SetTitleColor1(RGB(100,100,100));
		m_pTradeContainer->m_pDlgTradeBidParent->m_DlgQuickOrder.m_btnGoodsName.SetTitleColor2(RGB(34,124,237));

		m_pTradeContainer->m_pDlgTradeBidParent->m_DlgQuickOrder.m_btnLeft.SetSubCaption(_T("0.0"));
		m_pTradeContainer->m_pDlgTradeBidParent->m_DlgQuickOrder.m_btnRight.SetSubCaption(_T("0.0"));
		m_pTradeContainer->m_pDlgTradeBidParent->m_DlgQuickOrder.AdjustControlStatus(FALSE,FALSE);
		//m_pTradeContainer->m_pDlgTradeBidParent->m_DlgQuickOrder.Invalidate();
		//return;
	}
	else
	{
		m_pTradeContainer->m_pDlgTradeBidParent->m_DlgQuickOrder.m_btnGoodsName.SetTitleColor1(RGB(100,100,100));
		//	m_pTradeContainer->m_pDlgTradeBidParent->m_DlgQuickOrder.SetTipText(L"");
	}
	//开仓
	if(0==iFlag)//单击
	{
		//m_pTradeContainer->m_pDlgTradeBidParent->GetTradeQuickOrderDlg().InitControl();
		m_pTradeContainer->m_pDlgTradeBidParent->GetTradeQuickOrderDlg().SetPrice();
	}
	else if(1==iFlag)//双击
	{
		//判断是买还是卖
		if(CReportScheme::ERHBuyPrice == eHeader)
		{
			m_pTradeContainer->m_pDlgTradeBidParent->TradeOpen(m_pTradeContainer->ClientMerch2TradeCode(pMerch),true);
		}
		else if(CReportScheme::ERHSellPrice == eHeader)
		{
			m_pTradeContainer->m_pDlgTradeBidParent->TradeOpen(m_pTradeContainer->ClientMerch2TradeCode(pMerch),false);
		}		
	}
	else if(2==iFlag)//按钮
	{
		CRect rectWind;
		m_pTradeContainer->GetWindowRect(&rectWind);
		m_pTradeContainer->m_pDlgTradeBidParent->ShowHideQuickOrder();
		m_pTradeContainer->m_pDlgTradeBidParent->GetTradeQuickOrderDlg().SetWindowSize(rectWind.left,rectWind.top);
		m_pTradeContainer->m_pDlgTradeBidParent->GetTradeQuickOrderDlg().SetPrice();
	}
}

BOOL CMainFrame::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;	
}

BOOL CMainFrame::OnNcCreate(LPCREATESTRUCT lpCreateStruct) 
{	
	if (!CMDIFrameWnd::OnNcCreate(lpCreateStruct))
	{
		return FALSE;
	}

	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;	
	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	// 	T_UserInfo stUserInfo;		

	if( (NULL==m_pDlgAccount) && (EType_Normal==pApp->m_pConfigInfo->m_btnUserInfo.m_eShowType))
	{
		m_pDlgAccount = new CDlgAccount(this);
		m_pDlgAccount->Create(IDD_DIALOG_USER_ACCOUNT, this);
	}

	if (NULL == m_pDlgWebContent )
	{
		m_pDlgWebContent = new CDlgWebContent(this);
		m_pDlgWebContent->Create(IDD_DIALOG_WEB_CONTENT, this);
	}

	CRect rcControl(0,0,0,0);

	//添加账号信息，账号或者登录，注册
	if (pApp->m_pConfigInfo->m_bMember && (EType_Hide!=pApp->m_pConfigInfo->m_btnUserInfo.m_eShowType))
	{
		AddTitleMenuButton(rcControl, m_pImageUserAccount, 1, ID_MENU_USER_NAME, L"");
	}

	AddTitleMenuButton(rcControl, m_pImgMenuSystem, 3, ID_MENU_SYSTEM,L"系统");
	AddTitleMenuButton(rcControl, m_pImgMenuAlarm, 3, ID_MENU_FORMULA_MANAGE,L"公式管理");
	AddTitleMenuButton(rcControl, m_pImgMenuAlarm, 3, ID_MENU_ALARM,L"条件预警");
	AddTitleMenuButton(rcControl, m_pImgMenuSelStock, 3, ID_MENU_SEL_STOCK,L"选股");
    AddTitleMenuButton(rcControl, m_pImgMenuTrade, 3, ID_MENU_TRADE,L"交易");
	AddTitleMenuButton(rcControl, m_pImgMenuHelp, 3, ID_MENU_HELP,L"帮助");
	AddTitleMenuButton(rcControl, m_pImageLogin, 3, ID_MENU_LOGIN, L"登录");
	AddTitleMenuButton(rcControl, m_pImageLogin, 3, ID_MENU_REGISTER, L"注册");
	AddTitleMenuButton(rcControl, m_pImgMenuExit, 3, ID_MENU_EXIT, L"退出");
	//CNCButton* pIMButton = AddTitleMenuButton(rcControl, m_pImgMenuBtn1, 3, ID_MENU_ONLINE_SERVICE, L"在线客服");

	// 添加标题栏上的系统按钮
	AddTitleSysButton(rcControl, m_pImgSysBtn, 2, ID_CLOSE_BTN);
	AddTitleSysButton(rcControl, m_pImgSysBtn, 2, ID_MAX_BTN);
	AddTitleSysButton(rcControl, m_pImgSysBtn, 2, ID_MIN_BTN);
	AddTitleSysButton(rcControl, m_pImgHideBtn, 3, ID_TOOLBAR_BTN, L"");

	//UrlParser parser(L"alias://onlineIm");
	//if (!parser.m_bValid)
	//{
	//	pIMButton->SetVisiable(false);
	//}

	return TRUE;
}

int CMainFrame::TMenuButtonHitTest(CPoint point)
{
	map<int, CNCButton>::iterator iter;

	// 遍历标题栏上所有按钮
	for (iter=m_mapMenuBtn.begin(); iter!=m_mapMenuBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;

		// 点point是否在按钮区域内
		if (btnControl.PtInButton(point)  && btnControl.GetCreate())
		{
			return btnControl.GetControlId();
		}
	}

	return INVALID_ID;
}

int CMainFrame::TSysButtonHitTest(CPoint point)
{
	map<int, CNCButton>::iterator iter;

	// 遍历标题栏上所有按钮
	for (iter=m_mapSysBtn.begin(); iter!=m_mapSysBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;

		// 点point是否在按钮区域内
		if (btnControl.PtInButton(point) && btnControl.GetCreate())
		{
			return btnControl.GetControlId();
		}
	}

	return INVALID_ID;
}

CNCButton *CMainFrame::AddTitleMenuButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption)
{

	ASSERT(pImage);
	CNCButton btnControl;
	btnControl.CreateButton(lpszCaption, lpRect, this, pImage, nCount, nID);
	//if (ID_MENU_BACK == nID || IDM_IOVIEWBASE_F7 == nID || ID_MENU_SELF_CHOOSE == nID )
	//{
	//	btnControl.SetTextColor(RGB(0xf1,0xd1,0xd1), RGB(255,255,255), RGB(255,255,255));
	//}
	//else
	{
		btnControl.SetTextColor(RGB(200,200,200), RGB(230,70,70), RGB(230,70,70));
	}

	//btnControl.SetTextBkgColor(0x2f2f2f, 0x2f2f2f, 0x2f2f2f);
	//btnControl.SetTextFrameColor(0x2f2f2f, 0x2f2f2f, 0x2f2f2f);

	CNCButton::T_NcFont stFont;
	CFontFactory fontFactory;
	stFont.m_StrName = fontFactory.GetExistFontName(L"SimSun");	//...
	stFont.m_Size	 =  8.7f;
	stFont.m_iStyle = FontStyleRegular;
 //   if(ID_MENU_USER_NAME == nID)
	//{
	//	stFont.m_StrName = fontFactory.GetExistFontName(L"微软雅黑");	//...
	//	stFont.m_Size	 =  9.5f;
	//	stFont.m_iStyle = FontStyleRegular;
	//}
	btnControl.SetFont(stFont);

	m_mapMenuBtn[nID] = btnControl;
	return &m_mapMenuBtn[nID];
}

CNCButton *CMainFrame::AddTitleSysButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption)
{

	// ASSERT(pImage);
	CNCButton btnControl;
	btnControl.CreateButton(lpszCaption, lpRect, this, pImage, nCount, nID);
	m_mapSysBtn[nID] = btnControl;
	return &m_mapSysBtn[nID];
}

// 菜单栏上按钮的消息响应
BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	int32 iID = (int32)wParam;
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();

	if(iID >= BTN_ID_QUOTE_HSAG && iID <= BTN_ID_QUOTE_WHSC)
	{
		return CMDIFrameWnd::OnCommand(wParam, lParam);
	}
	if (iID >= IDM_BLOCKMENU_BEGIN && iID <= IDM_BLOCKMENU_END)
	{
		return CMDIFrameWnd::OnCommand(wParam, lParam);
	}

	if(ID_TREND_SHOWTICK == iID)
	{
		OnPicMenuCfm(ID_PIC_TICK);
	}

	if(iID == ID_NEW_WORKSPACE || iID == ID_OPEN_WORKSPACE || iID == ID_SAVE_CUR_CHILDFRAME
		||iID == ID_SAVEAS_WORKSPACE || iID == ID_RESTORE_VIEWSP || iID == ID_LOCK_CUR_CHILDFRAME)
	{
		return CMDIFrameWnd::OnCommand(wParam, lParam);
	}

	if (NULL == m_pNewWndTB)
	{
		return CMDIFrameWnd::OnCommand(wParam, lParam);
	}

	//
	switch (iID)
	{
	case ID_MENU_USER_NAME:
		{
			CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
			CString StrUserName = m_stUserInfo.wszUserName;
			if (pDoc && StrUserName.IsEmpty())
			{
				ShowCompleteMaterialDlg();
				break;
			}

			// 根据配置的类型，不同方式展示
			switch(pApp->m_pConfigInfo->m_btnUserInfo.m_eShowType)
			{
			case EType_Normal:
				{
					ShowUserInfoNormal();
					break;
				}
			case EType_Cfm:
				{
					ShowUserInfoCfm();
					break;
				}
			case EType_Dialog:
				{
					ShowUserInfoDialog();
					break;
				}
			default:
				{
					break;
				}
			}
		}
		break;
	//case ID_MENU_ONLINE_SERVICE:
	//	{
	//		ShowOnlineService();
	//	}
	//	break;
	case ID_MENU_LOGIN:
	case ID_MENU_EXIT:
		{

			CGGTongDoc* pDoc = (CGGTongDoc*) AfxGetDocument();
			if (NULL == pApp)
			{
				return CMDIFrameWnd::OnCommand(wParam, lParam);
			}

			if (NULL != pDoc && NULL != pDoc->m_pAbsCenterManager)
			{
				m_bShowLoginDlg = true;
				PostMessage(WM_CLOSE);
			}		
		}
		break;

	case ID_MENU_REGISTER:
		{
			ShowRegisterDialog();

			return CMDIFrameWnd::OnCommand(wParam, lParam);
		}
		break;

	case ID_TOOLBAR_BTN:
		{
			// 显示/隐藏工具栏
			if (m_wndReBar.IsWindowVisible())
			{
				m_wndReBar.ShowWindow(SW_HIDE);
				m_mapSysBtn[ID_TOOLBAR_BTN].SetImage(m_pImgShowBtn,FALSE);
			}
			else
			{
				m_wndReBar.ShowWindow(SW_SHOW);
				m_mapSysBtn[ID_TOOLBAR_BTN].SetImage(m_pImgHideBtn,FALSE);
			}
			RecalcLayout();
			DrawCustomNcClient();
			break;
		}

	case ID_DISCONNECT_SERVER:
		{
			// 断开行情
			if (m_wndStatusBar.m_bConnect)
			{
				OnDisConnectServer();
			}
			else
			{
				OnConnectServer();
			}
			break;
		}
	case ID_DISCONNECT_NEWSSERVER:
		{
			// 断开资讯
			if (m_wndStatusBar.m_bConnectNews)
			{
				OnDisConnectNewsServer();
			}
			else
			{
				OnConnectNewsServer();
			}
			break;
		}
	//case ID_MENU_AUTOCONNECT:
	//	{
	//		
	//		if( pDoc )
	//		{
	//			pDoc->m_bAutoLogin ^=1;
	//			CDlgNewLogin::SetUserInfoStatus(pDoc->m_pAbsCenterManager->GetUserName(), pDoc->m_bAutoLogin);
	//		}
	//		break;
	//	}
	//case ID_SYS_AUTO_RUN:
	//	{
	//		if (pDoc)
	//		{
	//			pDoc->m_bAutoRun ^= 1;
	//			SetAutoRun(pDoc->m_bAutoRun);
	//		}
	//	}
	//	break;

	default:
		{
			if (iID>=ID_MENU_SYSTEM && iID<= ID_MENU_HELP)
			{
				CRect rcNCButton;
				//m_pMenuBar->GetMenuBtnRect(rcNCButton, wParam);
				m_mapMenuBtn[iID].GetRect(rcNCButton);
				rcNCButton.left -= m_nFrameWidth;

				rcNCButton.bottom = 0;
				ClientToScreen(&rcNCButton);
				CString StrText;
				E_MenuItemType eType = EMIT_NONE;
				if(iID == ID_MENU_FORMULA_MANAGE)
				{
					OnFormulaMag();
				}
				else if(iID == ID_MENU_ALARM)
				{
					// 条件预警
					m_wndStatusBar.CreateAlarmDialog();
				}
				else
				{
					T_MenuItemData tmpData;
					tmpData.bShowArrow = false;
					tmpData.strText = StrText;
					if(iID == ID_MENU_SYSTEM)
					{
						StrText = L"行情";
						eType = EMIT_FIR_SYSTEM;
						tmpData.ID = 0;
					}
					else if(iID == ID_MENU_SEL_STOCK)
					{
						StrText = L"选股";
						eType = EMIT_FIR_SEL_STOCK;
						tmpData.ID = 1;
					}
					else if(iID == ID_MENU_TRADE)
					{
						StrText = L"交易";
						eType = EMIT_FIR_TRADE;
						tmpData.ID = 2;
					}
					else if(iID == ID_MENU_HELP)
					{
						StrText = L"帮助";
						eType = EMIT_FIR_HELP;
						tmpData.ID = 3;
					}

	
					ShowMenu(tmpData, eType, rcNCButton);
				}

			}
		}
		break;
	}

	return CMDIFrameWnd::OnCommand(wParam, lParam);
}

void CMainFrame::ShowConditionWarning()
{
	m_wndStatusBar.CreateAlarmDialog();
}

//下次自动登录
void CMainFrame::OnMenuAutoConnect()
{
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if( pDoc && pDoc->m_pAbsCenterManager)
	{
		pDoc->m_bAutoLogin ^=1;
		CDlgNewLogin::SetUserInfoStatus(pDoc->m_pAbsCenterManager->GetUserName(),pDoc->m_bAutoLogin);
	}
}

void CMainFrame::OnUpdateMenuAutoConnect(CCmdUI* pCmdUI)
{
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if( pDoc && pCmdUI )
	{
		pCmdUI->SetCheck(pDoc->m_bAutoLogin);
		pCmdUI->Enable();
	}
}


void CMainFrame::OnMenuShowToolBar()
{
	//CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	//if( pDoc )
	//{
	//	pDoc->m_bShowSCToolBar ^=1;
	//	CDlgNewLogin::SetUserToolBarStatus(pDoc->m_pAbsCenterManager->GetUserName(),pDoc->m_bShowSCToolBar);

	//	if (pDoc->m_bShowSCToolBar)
	//	{
	//		ShowControlBar(&m_ShortCutMenuToolBar, true, false);
	//	}
	//	else
	//	{
	//		ShowControlBar(&m_ShortCutMenuToolBar, false, false);
	//	}
	//}
}

void CMainFrame::OnUpdateMenuShowToolBarStatus(CCmdUI* pCmdUI)
{
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if( pDoc && pCmdUI )
	{
		pCmdUI->SetCheck(pDoc->m_bShowSCToolBar);
		pCmdUI->Enable();
	}
}


void CMainFrame::OnExportData()
{
	OnIoViewMenu(ID_EXPORT_DATA);
}

int32 CMainFrame::GetValidGroupID()
{	
	CArray<int32,int32> aIDs;
	aIDs.RemoveAll();

	int32 i = 0;
	for (i = 1 ; i <= MAXGROUPID; i++)
	{
		aIDs.Add(i);
	}

	//

	for ( i = 0 ; i < m_IoViewsPtr.GetSize(); i++)
	{
		int32 iGroupID = m_IoViewsPtr[i]->GetIoViewGroupId();

		for ( int32 j = 0; j < aIDs.GetSize(); j++)
		{
			if ( iGroupID == aIDs[j])
			{
				aIDs.RemoveAt(j);

				if( 0 == aIDs.GetSize())
				{
					AfxMessageBox(L"当前无可用分组号!此窗口下视图无分组",MB_ICONWARNING);
					return 0;
				}
			}
		}
	}	

	//
	int32 iMinGroupID = aIDs.GetAt(0);
	//

	return iMinGroupID;
}


void CMainFrame::OnViewMerchChanged(IN CIoViewBase *pIoViewSrc, IN CMerch *pMerch)
{
	OnViewMerchChangeBegin(pIoViewSrc, pMerch);
	OnViewMerchChangeEnd(pIoViewSrc, pMerch);
}

void CMainFrame::OnViewMerchChanged( int32 iIoViewGroupId, CMerch *pMerch )
{
	if ( iIoViewGroupId > 0 )
	{
		OnViewMerchChangeBegin(iIoViewGroupId, pMerch);
		OnViewMerchChangeEnd(iIoViewGroupId, pMerch);
	}
}

void CMainFrame::OnViewMerchChangeBegin(IN CIoViewBase *pIoViewSrc, IN CMerch *pMerch)
{
	// 开始切换商品, 通知视图更新控件, 静态数据. 
	if (NULL == pIoViewSrc || NULL == pMerch)
	{
		return;
	}

	// 添加到用户行为记录
	{
		CString StrFunName = pMerch->m_MerchInfo.m_StrMerchCnName;
		StrFunName += L"-";
		StrFunName += pMerch->m_MerchInfo.m_StrMerchCode;

		AddToUserDealRecord(StrFunName, EPTCode);
	}

	int32 iIoViewGroupId = pIoViewSrc->GetIoViewGroupId();
	if (iIoViewGroupId <= 0)	// 独立业务视图， 仅改变一个商品
	{
		pIoViewSrc->OnVDataMerchChanged(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, pMerch);
	}
	else	// 将所有关联业务视图， 全部更换商品， 并且仅当该视图可见时， 通知其更新数据
	{
		
		for (int32 i = 0; i < m_IoViewsPtr.GetSize(); i++)
		{		
			//if(iGroupID & iIoViewGroupId )
			bool32 bIsSameFrame = false;
			if (m_IoViewsPtr[i]->GetParentFrame() == pIoViewSrc->GetParentFrame())
			{
				bIsSameFrame = true;
			}
			
			int32 iGroupId = m_IoViewsPtr[i]->GetIoViewGroupId();
			if (((iGroupId & iIoViewGroupId) || (iGroupId == iIoViewGroupId))
				/*&& bIsSameFrame*/)
			{
				m_IoViewsPtr[i]->OnVDataMerchChanged(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, pMerch);
			}
		}
	}

	// 快手下单
	OnQuickTrade(0, pMerch, CReportScheme::ERHCount);
	KeepRecentMerch(pMerch);		

	CIoViewBase::AddLastCurrentMerch(pMerch);	// 记录当前的商品

	CIoViewBase* pIoViewActive  = FindActiveIoView();
	if ( NULL != pIoViewActive)
	{
		pIoViewActive->SetChildFrameTitle();			
	}
}

void CMainFrame::OnViewMerchChangeBegin( IN int32 iIoViewGroupId, CMerch *pMerch )
{

	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->GetMainWnd();
	CMDIChildWnd *pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
	for (int32 i = 0; i < m_IoViewsPtr.GetSize(); i++)
	{

		bool32 bIsSameFrame = false;
		if (pChild == m_IoViewsPtr[i]->GetParentFrame())
		{
			bIsSameFrame = true;
		}

		int32 iGroupID = m_IoViewsPtr[i]->GetIoViewGroupId();
		//if(iGroupID & iIoViewGroupId )
		if (((iGroupID & iIoViewGroupId) || (iGroupID == iIoViewGroupId))
			/*&& bIsSameFrame*/)
		{
			m_IoViewsPtr[i]->OnVDataMerchChanged(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, pMerch);
		}

	}

	KeepRecentMerch(pMerch);		

	CIoViewBase* pIoViewActive = FindActiveIoView();
	if ( NULL != pIoViewActive)
	{
		pIoViewActive->SetChildFrameTitle();			
	}
}

void CMainFrame::OnViewMerchChangeEnd(IN CIoViewBase *pIoViewSrc, IN CMerch *pMerch)
{
	// 切换完成, 通知视图开始请求数据
	if (NULL == pIoViewSrc || NULL == pMerch)
	{
		return;
	}

	//
	int32 iIoViewGroupId = pIoViewSrc->GetIoViewGroupId();

	if ( iIoViewGroupId <= 0 )	// 独立业务视图， 仅改变一个商品
	{
		pIoViewSrc->ForceUpdateVisibleIoView();


		if ( NULL != m_pDlgF10 )
		{
			if ( m_pDlgF10->GetIoView() == pIoViewSrc && m_pDlgF10->IsWindowVisible())
			{
				m_pDlgF10->OnMerchChange(pMerch);
			}
		}
	}
	else	// 将所有关联业务视图， 全部更换商品， 并且仅当该视图可见时， 通知其更新数据
	{
		OnViewMerchChangeEnd(iIoViewGroupId, pMerch);
	}
}

void CMainFrame::OnViewMerchChangeEnd( IN int32 iIoViewGroupId, CMerch *pMerch )
{
	for (int32 i = 0; i < m_IoViewsPtr.GetSize(); i++)
	{
		int32 iGroupID = m_IoViewsPtr[i]->GetIoViewGroupId();
		if ((iGroupID & iIoViewGroupId) && m_IoViewsPtr[i]->m_bShowNow/*m_IoViewsPtr[i]->GetIoViewGroupId() == iIoViewGroupId*/)
		{
			m_IoViewsPtr[i]->ForceUpdateVisibleIoView();
		}
	}

	//
	if ( NULL != m_pDlgF10 )
	{
		CIoViewBase* pIoView = m_pDlgF10->GetIoView();

		if ( NULL != pIoView && m_pDlgF10->IsWindowVisible() )
		{
			m_pDlgF10->OnMerchChange(pMerch);
		}
	}
}

void CMainFrame::OnViewGridHeaderChanged(E_ReportType eReportType)
{
	for (int32 i = 0; i < m_IoViewsPtr.GetSize(); i++)
	{
		m_IoViewsPtr[i]->OnVDataGridHeaderChanged(eReportType);
	}
}

void CMainFrame::OnViewFormulaChanged ( E_FormulaUpdateType eUpdateType, CString StrName )
{
	for (int32 i = 0; i < m_IoViewsPtr.GetSize(); i++)
	{
		m_IoViewsPtr[i]->OnVDataFormulaChanged(eUpdateType,StrName);
	}
}

CMerch * CMainFrame::GetIoViewGroupMerch(int32 iGroupId)
{
	ASSERT(iGroupId>=0 && iGroupId <=MAXGROUPID);
	if ( iGroupId < 0 )
	{
		return NULL;	// 独立视图的不处理
	}

	int32 i;
	for ( i=0; i < m_IoViewsPtr.GetSize() ; i++ )
	{
		CIoViewBase *pIoView = m_IoViewsPtr[i];
		int32 iViewGroupID = pIoView->GetIoViewGroupId();
		if (  pIoView->GetIoViewType() == CIoViewBase::EIVT_SingleMerch && (iViewGroupID & iGroupId)/*pIoView->GetIoViewGroupId() == iGroupId*/ )
		{
			CMerch *pMerch = pIoView->GetMerchXml();
			if ( NULL != pMerch )
			{
				return pMerch;// 这个实际上集合视图是有可能不同的
			}
		}
	}

	for ( i=0; i < m_IoViewsPtr.GetSize() ; i++ )
	{
		CIoViewBase *pIoView = m_IoViewsPtr[i];
		int32 iViewGroupID = pIoView->GetIoViewGroupId();
		if (  (iViewGroupID & iGroupId)/*pIoView->GetIoViewGroupId() == iGroupId*/ )
		{
			return pIoView->GetMerchXml();	// 没集合视图，那没办法了，随便返回一个关系也不大
		}
	}

	return NULL;
}

bool32 CMainFrame::OnViewAddCompareMerch(IN CIoViewBase *pIoViewSrc, IN CMerch *pMerch)
{
	if (NULL == pIoViewSrc)
	{
		return false;
	}

	if (NULL == pMerch)
	{
		return false;
	}

	int32 iIoViewGroupId = pIoViewSrc->GetIoViewGroupId();

	T_CompareMerchInfo CompareMerchInfo;	
	CompareMerchInfo.m_iGroupId		= iIoViewGroupId;
	CompareMerchInfo.m_pMerch		= pMerch;

	if (iIoViewGroupId <= 0)	// 独立业务视图， 仅通知自己
	{
		bool32 bAddSuccess = false;

		bAddSuccess = pIoViewSrc->OnVDataAddCompareMerch(pMerch);

		if (bAddSuccess)
		{
			m_aCompareMerchList.Add(CompareMerchInfo);			
		}
		return bAddSuccess;
	}
	else	// 将所有关联业务视图， 全部更换商品
	{
		bool32 bAddSuccess = false;

		for (int32 i = 0; i < m_IoViewsPtr.GetSize(); i++)
		{
			int32 iGroupID = m_IoViewsPtr[i]->GetIoViewGroupId();
			if(iGroupID & iIoViewGroupId)
				//if (m_IoViewsPtr[i]->GetIoViewGroupId() == iIoViewGroupId)
			{
				if (m_IoViewsPtr[i]->OnVDataAddCompareMerch(pMerch))
				{
					bAddSuccess = true;
				}
			}
		}

		if (bAddSuccess)
		{
			m_aCompareMerchList.Add(CompareMerchInfo);			
		}

		return bAddSuccess;
	}

	return false;
}

void CMainFrame::OnViewRemoveCompareMerch(IN CIoViewBase *pIoViewSrc, IN CMerch *pMerch)
{
	if (NULL == pIoViewSrc)
	{
		return;
	}

	if (NULL == pMerch)
	{
		return;
	}

	int32 iIoViewGroupId = pIoViewSrc->GetIoViewGroupId();
	if (iIoViewGroupId <= 0)	// 独立业务视图， 仅通知自己
	{
		pIoViewSrc->OnVDataRemoveCompareMerch(pMerch);	
	}
	else	// 将所有关联业务视图， 全部更换商品
	{
		for (int32 i = 0; i < m_IoViewsPtr.GetSize(); i++)
		{
			int32 iGroupID = m_IoViewsPtr[i]->GetIoViewGroupId();
			if(iGroupID & iIoViewGroupId)
				//if (m_IoViewsPtr[i]->GetIoViewGroupId() == iIoViewGroupId)
			{
				m_IoViewsPtr[i]->OnVDataRemoveCompareMerch(pMerch);
			}
		}
	}

	for (int32 i = 0 ; i<m_aCompareMerchList.GetSize(); i++)
	{
		if (m_aCompareMerchList[i].m_iGroupId == iIoViewGroupId && m_aCompareMerchList[i].m_pMerch == pMerch)
		{
			m_aCompareMerchList.RemoveAt(i);
		}
	}	
}

void CMainFrame::OnViewClearCompareMerchs(IN CIoViewBase *pIoViewSrc)
{
	if (NULL == pIoViewSrc)
	{
		return;
	}
	int32 iIoViewGroupId = pIoViewSrc->GetIoViewGroupId();
	if (iIoViewGroupId <= 0)	// 独立业务视图， 仅通知自己
	{
		pIoViewSrc->OnVDataClearCompareMerchs();

	}
	else	// 将所有关联业务视图， 全部更换商品
	{
		for (int32 i = 0; i < m_IoViewsPtr.GetSize(); i++)
		{
			int32 iGroupID = m_IoViewsPtr[i]->GetIoViewGroupId();
			if(iGroupID & iIoViewGroupId)
				//if (m_IoViewsPtr[i]->GetIoViewGroupId() == iIoViewGroupId)
			{
				m_IoViewsPtr[i]->OnVDataClearCompareMerchs();
			}
		}
	}
}

bool CMainFrame::HasValidViewTextNews()
{

	for (int32 i = 0; i < m_IoViewsPtr.GetSize(); i++)
	{
		CIoViewBase* pBase = m_IoViewsPtr[i];
		if (NULL != pBase && pBase->IsKindOf(RUNTIME_CLASS(CIoViewTextNews)) && pBase->IsWindowVisible())
		{
			return true;
		}
	}
	return false;
}

void CMainFrame::RemoveHotKey(E_HotKeyType eHotKeyType)
{
	for (int32 i = m_HotKeyList.GetSize() - 1; i >= 0; i--)
	{
		if (m_HotKeyList[i].m_eHotKeyType == eHotKeyType)
			m_HotKeyList.RemoveAt(i, 1);
	}
}

void CMainFrame::BuildHotKeyList()
{
	m_HotKeyList.RemoveAll();

	// 初始化拼音模块
	InitPy();

	// 创建快捷键列表
	int32 iShortCutCount = GetShortCutObjectCount();
	for (int32 i = 0; i < iShortCutCount; i++)
	{
		const T_ShortCutObject *pstShortCutObject = GetShortCutObject(i);

		// 添加一个
		CHotKey HotKey;	
		HotKey.m_eHotKeyType	= EHKTShortCut;
		HotKey.m_StrKey			= pstShortCutObject->m_StrKey;
		HotKey.m_StrSummary		= pstShortCutObject->m_StrSummary;
		HotKey.m_StrParam1		= pstShortCutObject->m_StrKey;
		m_HotKeyList.Add(HotKey);
	}

	//创建板块快捷键
	CBlockConfig::Instance()->BuildHotKeyList(m_HotKeyList);

	//创建自定义板块快捷键
	CUserBlockManager::Instance()->BuildUserBlockHotKeyList(m_HotKeyList);

	//创建商品快捷键
	CGGTongDoc* pDoc = AfxGetDocument();
	if (pDoc->m_pAbsCenterManager)
	{
		pDoc->m_pAbsCenterManager->BuildMerchHotKeyList(m_HotKeyList);
	}
	
	//创建指标快捷键
	CFormulaLib::instance()->BuildHotKeyList(m_HotKeyList);

	//
}

void CMainFrame::ShowNewsCountent(T_NewsContentInfo& stNewsContentInfo)
{
	static const CString StrWeb1 = "www.";
	static const CString StrWeb2 = "http";

	CString StrContentHead = stNewsContentInfo.m_StrContent.Left(4);

	if ( 0 == StrContentHead.CompareNoCase(StrWeb1) || 0 == StrContentHead.CompareNoCase(StrWeb2) )
	{
		if (NULL == m_pDlgJPNews || NULL == m_pDlgJPNews->m_hWnd)
		{
			m_pDlgJPNews  = new CDlgJPNews; 
			m_pDlgJPNews ->Create(IDD_DIALOG_JP_NEWS, this);
		}

		CGuiTabWnd	*pGuiTopTabWnd = m_pDlgJPNews->GetTabWnd();
		CString strCaption = stNewsContentInfo.m_StrTitle;
		CString strShort = strCaption;
		if(strCaption.GetLength() > 7)
		{
			strShort = strCaption.Left(7) + L"...";
		}
		CString strUrl = stNewsContentInfo.m_StrContent;
		int iTabCnt = m_pDlgJPNews->GetTabCount();

		int iCnt = pGuiTopTabWnd->GetCount();
		for (int i=0; i<iCnt; i++)
		{
			//if (strCaption == pGuiTopTabWnd->GetTabsTitle(i))  // 已经存在
			if(strUrl == pGuiTopTabWnd->GetTabsValue(i))	// 链接相同，表示已经存在
			{
				if ((i+iTabCnt-1) <= iCnt)
				{
					pGuiTopTabWnd->SetShowBeginIndex(i, i+iTabCnt-1);
				}

				pGuiTopTabWnd->SetCurtab(i);
				m_pDlgJPNews ->SetTabSize();
				return;
			}
		}

		pGuiTopTabWnd->Addtab(strCaption, strShort, strCaption, stNewsContentInfo.m_StrContent);
		pGuiTopTabWnd->SetCurtab(pGuiTopTabWnd->GetCount()-1);

		if(m_pDlgJPNews->GetSafeHwnd())
		{
			m_pDlgJPNews->SetTabSize();
			m_pDlgJPNews->ShowWindow(SW_SHOW);
		}
	}
	else
	{
		CDlgNews dlg;
		//	dlg.m_clrText = GetIoViewColor(CFaceScheme::ESCText);
		//	dlg.m_clrBackground = GetIoViewColor(CFaceScheme::ESCBackground);
		dlg.SetTitle(stNewsContentInfo.m_StrTitle );
		dlg.SetNewsText(stNewsContentInfo.m_StrContent);

		CString strShowText = stNewsContentInfo.m_StrTimeUpdate;
		CTime timeTemp;
		CUtilTool::GetInistance().CharToTime(strShowText,timeTemp);
		strShowText = timeTemp.Format(L"%m/%d/%Y %H:%M");
		dlg.SetstrTime(strShowText); 

		dlg.DoModal();
	}
}

void CMainFrame::ChangeUserNickName(CString strNickName)
{
	m_mapMenuBtn[ID_MENU_USER_NAME].SetCaption(strNickName, FALSE);
	DrawCustomNcClient();
}

LRESULT CMainFrame::OnMsgViewDataOnAuthFail(WPARAM wParam, LPARAM lParam)
{
	CString *pStrMsg = (CString *)wParam;
	MessageBox(*pStrMsg);

	return 0;
}

LRESULT	CMainFrame::OnShowJPNews(WPARAM wParam, LPARAM lParam)
{
	T_NewsContentInfo* pNewsContent = (T_NewsContentInfo*)wParam;

	if(pNewsContent)
	{
		ShowNewsCountent(*pNewsContent);
	}

	return 0;
}

LRESULT	CMainFrame::OnBrowerPtr(WPARAM wParam, LPARAM lParam)
{
// 	CefRefPtr<CefBrowser> browser((CefBrowser *)wParam);
// 	if(m_pDlgJPNews)
// 	{
// 		m_pDlgJPNews->SetBrower(browser);
// 	}

	return 0;
}

LRESULT	CMainFrame::OnJPNewsTitle(WPARAM wParam, LPARAM lParam)
{
	if (NULL == m_pDlgJPNews)
	{
		return 0;
	}

	CString strTitle = (LPCTSTR)wParam;
	CString strUrl = (LPCTSTR)lParam;

	CGuiTabWnd	*pGuiTopTabWnd = m_pDlgJPNews->GetTabWnd();
	CString strShort = strTitle;
	if(strTitle.GetLength() > 7)
	{
		strShort = strTitle.Left(7) + L"...";
	}

	int iCnt = pGuiTopTabWnd->GetCount();
	for (int i=0; i<iCnt; i++)
	{
		if (strUrl == pGuiTopTabWnd->GetTabsTitle(i))
		{
			pGuiTopTabWnd->SetTabsTitle(i, strShort);
		}
	}

	return 0;
}

LRESULT	CMainFrame::OnWebCloseDlg(WPARAM wParam, LPARAM lParam)
{
	if (1 == wParam)	// 关闭web对话框
	{
		if (NULL != m_pDlgComment)
		{
			m_pDlgComment->EndDialog(IDOK);
		}
	}
	else				// 退出程序
	{
		if (NULL != m_pDlgComment)
		{
			m_pDlgComment->EndDialog(IDOK);
		}

		CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
		if(NULL != pApp)
		{
			//pApp->m_bUserLoginExit = true;
			//OnClose();
			m_bShowLoginDlg = true;
			PostMessage(WM_CLOSE);
		}
	}

	return 0;
}

LRESULT	CMainFrame::OnCloseOnlineServDlg(WPARAM wParam, LPARAM lParam)
{
	if (NULL != m_pDlgWebContent)
	{
		m_pDlgWebContent->EndDialog(IDOK);
	}
	return 0;
}

void CMainFrame::OnEnterIdle( UINT nWhy, CWnd* pWho )
{
	CMDIFrameWnd::OnEnterIdle(nWhy, pWho);

	//if ((MSGF_MENU==nWhy) &&(NULL!=m_pMenuBar))
	//{
	//	m_pMenuBar->OnEnterIdle();
	//}
}

CMenuBar *CMainFrame::GetMenuBar()
{
	//return m_pMenuBar;

	return NULL;
}

LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (0x125 == message)	// WM_UNINITMENUPOPUP
	{
		HMENU hmenuPopup = (HMENU)wParam;   // 子菜单句柄

		int32 iSize = m_sysMenu.GetMenuItemCount();
		for (int32 i=0; i<iSize; i++)
		{
			if (hmenuPopup == m_sysMenu.GetSubMenu(i)->m_hMenu)
			{ 
				DrawCustomNcClient();
				break;
			}
		}
	}

	return CMDIFrameWnd::WindowProc(message, wParam, lParam);
}

void CMainFrame::ShowHelpMenu(UINT nID)
{
	// 帮助菜单
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if (NULL == pApp)
	{
		return ;
	}

	CMenuConfig Menu = pApp->m_pConfigInfo->m_aHelpMenus[nID - ID_USE_HELP];
	if(!Menu.m_StrName.IsEmpty())
	{
		if ( !Menu.m_StrUrl.IsEmpty() )
		{
			bool32 bShowIE = FALSE;
			int32 iSize = pApp->m_pConfigInfo->m_aWndSize.GetSize();
			int32 iWidth = 1024, iHeight = 750;
			for (int32 i=0; i<iSize; i++)
			{
				CWndSize wnd = pApp->m_pConfigInfo->m_aWndSize[i];
				if (Menu.m_StrName == wnd.m_strID)
				{
					iWidth = wnd.m_iWidth;
					iHeight = wnd.m_iHeight;
					if (0==iWidth || 0== iHeight)
					{
						bShowIE = TRUE;
					}
					break;
				}
			}
			if (bShowIE)
			{
				ShellExecute(0, L"open", Menu.m_StrUrl, NULL, NULL, SW_NORMAL);
			}
			else
			{
				CDlgTodayCommentIE::ShowDlgIEWithSize(Menu.m_StrName, Menu.m_StrUrl, CRect(0, 0, iWidth, iHeight), 0, RGB(255,255,255));
			}
		}
		else
		{
			LoadSpecialCfm(Menu.m_StrName);
		}
	}
}

bool32 CMainFrame::GetTradeExePath(OUT CString &StrExePath, bool32 bEnableReq/*=false*/, bool32 bDirtyOld/*=false*/)
{
	if ( !bDirtyOld && CEtcXmlConfig::Instance().ReadEtcConfig(KStrTBWndSection, KStrTBWndTradeKey, NULL, StrExePath) )
	{
		CFileFind FileFind;
		if ( FileFind.FindFile(StrExePath) )
		{
			return true;
		}
	}
	else
	{
		// 弹出交易设置页面
		CDlgTradeSetting dlg;
		dlg.DoModal();
	}

	if ( !bEnableReq )
	{
		return false;
	}

	return false;
}

void CMainFrame::SetTradeExePath( LPCTSTR lpszExePath )
{
	CEtcXmlConfig::Instance().WriteEtcConfig(KStrTBWndSection, KStrTBWndTradeKey, lpszExePath);
}

//外挂交易
void CMainFrame::DoTradePlugin()
{
	CString StrExePath;
	if ( GetTradeExePath(StrExePath, true)
		&& !StrExePath.IsEmpty() )
	{
		CFileFind FileFind;
		if ( FileFind.FindFile(StrExePath) )
		{
			FileFind.FindNextFile();
			STARTUPINFO stinfo = {0};
			stinfo.cb = sizeof(stinfo);
			PROCESS_INFORMATION pi = {0};

			CString StrDir = FileFind.GetFilePath();
			int iDirPos = StrDir.ReverseFind(_T('\\'));
			if ( iDirPos >= 0 )
			{
				StrDir = StrDir.Left(iDirPos);
			}
			else
			{
				ASSERT( 0 );
				StrDir.Empty();
			}

			LPCTSTR pszDir = StrDir.IsEmpty()? NULL : StrDir.operator LPCTSTR();

			if ( CreateProcess(StrExePath, NULL, NULL, NULL, FALSE, 0, NULL, pszDir, &stinfo, &pi) )
			{
				DEL_HANDLE(pi.hThread);
				DEL_HANDLE(pi.hProcess);
			}
			else
			{
				TCHAR szErr[100];
				_sntprintf(szErr, 100, _T("执行交易程序失败(%u)！"), GetLastError());
				MessageBox(szErr, _T("错误"));
				SetTradeExePath(NULL);	// 清除不能启动的交易配置
			}
		}
		else
		{
			MessageBox(_T("交易程序已不存在！"), _T("错误"));
			SetTradeExePath(NULL);	// 清除不能启动的交易配置
		}
	}
}

LRESULT CMainFrame::OnReqInfoCenter(WPARAM wParam, LPARAM lParam)
{
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if (!pDoc->m_pAutherManager)
	{
		return 0;
	}

	T_MsgTip stMsgTip;

	pDoc->m_pAutherManager->GetInterface()->GetServiceDispose()->GetMsgTip(stMsgTip);

	T_pushMsg stMsg;
	if (wcslen(stMsgTip.wszContent) > 0)
	{
		stMsg.m_StrTitle = stMsgTip.wszTitle;
		stMsg.m_StrContent = stMsgTip.wszContent;
		stMsg.m_StrContentUrl = stMsgTip.wszLinkUrl;
		ShowInfoCenterMsg(stMsg);
	}

	return 0;
}

LRESULT	CMainFrame::OnWebThirdLoginRsp(WPARAM wParam, LPARAM lParam)
{
	if (NULL != m_pDlgComment)
	{
		m_pDlgComment->EndDialog(IDOK);
	}
	return 0;
}

void CMainFrame::handleHeartbeat(Json::Value vData)
{
	// 心跳包不做处理，能收到心跳包就可以了
}

void CMainFrame::handleInfoContentQuery(Json::Value vData)
{
	//
}

void CMainFrame::handlePushInfo(Json::Value vData)
{
	T_pushMsg stMsg;

	int iMsgId = vData["Infotypecode"].asInt();

	if(en_monitor_msg == iMsgId)
	{
		stMsg.m_StrMsgType = L"预警消息";
	}
	else if(en_system_msg == iMsgId)
	{
		stMsg.m_StrMsgType = L"系统消息";
	}
	else if (en_optional_change == iMsgId)
	{
		stMsg.m_StrMsgType = L"自选异动";
	}
	else if (en_strategy_trade == iMsgId)
	{
		stMsg.m_StrMsgType = L"策略消息";
		if (m_pDlgPushMsg)
		{
			m_pDlgPushMsg->SetMsgTypeString(stMsg.m_StrMsgType);
		}
	}
	else if (en_select_stock == iMsgId)
	{
		stMsg.m_StrMsgType = L"智能选股";
	}
	else
	{
		stMsg.m_StrMsgType = L"未知资讯类型消息";
	}

	if(!vData["title"].isNull())
	{
		std::string strTitle = vData["title"].asString();
		stMsg.m_StrTitle = strTitle.c_str();
	}

	if(!vData["content"].isNull())
	{
		std::string strContent = vData["content"].asString();
		stMsg.m_StrContent = strContent.c_str();
	}

	// 如果消息内容为空，那么就不弹出提示框
	if (stMsg.m_StrContent.IsEmpty())
	{
		return;
	}

	stMsg.m_StrContentUrl = vData["url"].asString().c_str();

	if(NULL != m_pDlgPushMsg && !IsIconic())
	{
		if(m_pDlgPushMsg->IsWindowVisible())
		{
			m_pDlgPushMsg->InvalidateMsg(stMsg);
		}
		else
		{
			ShowInfoCenterMsg(stMsg);
		}
	}
}

void CMainFrame::OpenWebWorkspace(const CString &StrWsp, const CString &StrUrl)
{
	CMPIChildFrame *pActiveFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, MDIGetActive());
	CIoViewNews *pIoViewNews = NULL;

	if (NULL != pActiveFrame)
	{
		if(StrWsp != pActiveFrame->GetIdString())
		{
			bool32 bLoadcfm = m_pNewWndTB->DoOpenCfm(StrWsp, StrUrl);

			if ( bLoadcfm )
			{
				pIoViewNews = FindIoViewNews(true);
			}
			if(pIoViewNews)
			{
				pIoViewNews->RefreshWeb(StrUrl);
			}
		}
		else
		{
			pIoViewNews = FindIoViewNews(true);
			CCfmManager::Instance().AddOpenedCfm(StrWsp, StrUrl);

			if(pIoViewNews)
			{
				pIoViewNews->RefreshWeb(StrUrl);
			}
		}
	}
}

//完善资料页面
void CMainFrame::ShowCompleteMaterialDlg()
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if (NULL == pApp)
	{
		return ;
	}

	CString StrUrl = GetCompleteMaterialUrl();

	bool32 bShowIE = FALSE;
	int32 iSize = pApp->m_pConfigInfo->m_aWndSize.GetSize();
	int32 iWidth = 1024, iHeight = 700;
	CString StrName = L"";
	for (int32 i=0; i<iSize; i++)
	{
		CWndSize wnd = pApp->m_pConfigInfo->m_aWndSize[i];
		if (L"完善资料" == wnd.m_strID)
		{
			StrName = wnd.m_strTitle;
			iWidth = wnd.m_iWidth;
			iHeight = wnd.m_iHeight;

			if (0 == iWidth || 0 == iHeight)
			{
				bShowIE = TRUE;
			}
			break;
		}
	}
	if (bShowIE)
	{
		ShellExecute(0, L"open", StrUrl, NULL, NULL, SW_NORMAL);
	}
	else
	{
		//CDlgTodayCommentIE::ShowDlgIEWithSize(StrName, StrUrl, CRect(0, 0, iWidth, iHeight));
		m_pDlgComment->ShowDlgIEWithSize(StrName, StrUrl, CRect(0, 0, iWidth, iHeight));
	}
}

CString CMainFrame::GetCompleteMaterialUrl()
{
	CGGTongApp *pApp  =  (CGGTongApp*) AfxGetApp();	
	CGGTongDoc *pDoc = pApp->m_pDocument;

	CString StrUrl = pApp->m_pConfigInfo->m_StrBindUserUrl;
	CString StrNickName = m_stUserInfo.wszNickName;
	CString StrOpenId   = pDoc->m_StrOpenId;
	CString StrType     = pDoc->m_StrLoginType;
	UrlParser uParser(StrUrl); 

	uParser.SetQueryValue(L"openid", StrOpenId);
	uParser.SetQueryValue(L"nickname", StrNickName);
	uParser.SetQueryValue(L"type", StrType);
	uParser.SetQueryValue(L"add", L"add");

	return uParser.GetUrl();
}

void CMainFrame::ShowUserInfoNormal()
{
	if(m_pDlgAccount != NULL)
	{
		CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
		if (m_pDlgAccount->IsWindowVisible())
		{
			m_pDlgAccount->ShowWindow(FALSE);
			return;
		}
		CRect rcUSer;
		m_mapMenuBtn[ID_MENU_USER_NAME].GetRect(rcUSer);

		int iX = rcUSer.left - 96;
		int iY = rcUSer.bottom - m_rectCaption.bottom;

		CString StrUrl = pApp->m_pConfigInfo->m_btnUserInfo.m_StrUrl;
		UrlParser urlParser(StrUrl);
		urlParser.SetQueryValue(L"regFrom", L"PC");
		StrUrl = urlParser.GetUrl();

		bool32 bShowIE = FALSE;
		int32 iSize = pApp->m_pConfigInfo->m_aWndSize.GetSize();
		int32 iWidth = 1024, iHeight = 700;
		for (int32 i=0; i<iSize; i++)
		{
			CWndSize wnd = pApp->m_pConfigInfo->m_aWndSize[i];
			if (L"用户详细页面" == wnd.m_strID)
			{
				iWidth = wnd.m_iWidth;
				iHeight = wnd.m_iHeight;

				if (0 == iWidth || 0 == iHeight)
				{
					bShowIE = TRUE;
				}
				break;
			}
		}
		CRect rcWnd(iX-104, iY, iX + iWidth-104, iY + iHeight);
		ClientToScreen(&rcWnd);
		if (bShowIE)
		{
			ShellExecute(0, L"open", StrUrl, NULL, NULL, SW_NORMAL);
		}
		else
		{
			m_pDlgAccount->ShowDlgIEWithSize(L"", StrUrl,rcWnd);
			m_pDlgAccount->SetUserDlgInfo(rcUSer);
		}
	}
}

void CMainFrame::ShowUserInfoCfm()
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	CString StrUrl = pApp->m_pConfigInfo->m_btnUserInfo.m_StrUrl;

	CIoViewNews *pIoViewNews = NULL;
	CMPIChildFrame *pActiveFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, MDIGetActive());
	if (NULL != pActiveFrame)
	{
		if(pActiveFrame->GetIdString() != L"网页资讯")
		{
			bool32 bLoadcfm = CCfmManager::Instance().LoadCfm(L"网页资讯", false, false, StrUrl)!=NULL;
			if ( bLoadcfm )
			{
				pIoViewNews = FindIoViewNews(true);
			}
			if(pIoViewNews)
			{
				pIoViewNews->RefreshWeb(StrUrl);
			}
		}
		else
		{
			pIoViewNews = FindIoViewNews(true);
			CCfmManager::Instance().AddOpenedCfm(L"网页资讯", StrUrl);
			if(pIoViewNews)
			{
				pIoViewNews->RefreshWeb(StrUrl);
			}
		}
	}
}

void CMainFrame::ShowUserInfoDialog()
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();

	CString StrUrl = pApp->m_pConfigInfo->m_btnUserInfo.m_StrUrl;
	CString StrName = _T("用户详细页面");

	bool32 bShowIE = FALSE;
	int32 iSize = pApp->m_pConfigInfo->m_aWndSize.GetSize();
	int32 iWidth = 1024, iHeight = 700;
	for (int32 i=0; i<iSize; i++)
	{
		CWndSize wnd = pApp->m_pConfigInfo->m_aWndSize[i];
		if (StrName == wnd.m_strID)
		{
			StrName = wnd.m_strTitle;
			iWidth = wnd.m_iWidth;
			iHeight = wnd.m_iHeight;

			if (0 == iWidth || 0 == iHeight)
			{
				bShowIE = TRUE;
			}
			break;
		}
	}

	if (bShowIE)
	{
		ShellExecute(0, L"open", StrUrl, NULL, NULL, SW_NORMAL);
	}
	else
	{
		m_pDlgComment->ShowDlgIEWithSize(StrName, StrUrl, CRect(0, 0, iWidth, iHeight));
	}
}

void CMainFrame::ShowOnlineService()
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	CGGTongDoc *pDoc  = pApp->m_pDocument;
	if (!pDoc->m_pAutherManager)
	{
		return;
	}

	if (m_pDlgWebContent && m_pDlgWebContent->IsWindowVisible())
	{
		return ;
	}

	if (NULL == pApp || NULL == pDoc)
	{
		return;
	}

	if(!CPluginFuncRight::Instance().IsUserHasRight(L"IM客服", true, true))
	{
		return;
	}

	CString StrUrl = L""; 
	CString StrName = _T("在线客服");
	CString StrToken = pDoc->m_pAutherManager->GetInterface()->GetServiceDispose()->GetToken();

	UrlParser urlPaser(L"alias://customerServiceQQ");
	urlPaser.SetQueryValue(L"token", StrToken);

	string strMac = "";
	GetMacByGetAdaptersAddr(strMac);
	urlPaser.SetQueryValue(L"deviceid",_A2W(strMac.c_str()));

	StrUrl = urlPaser.GetUrl();

	bool32 bShowIE = FALSE;
	int32 iSize = pApp->m_pConfigInfo->m_aWndSize.GetSize();
	int32 iWidth = 1024, iHeight = 700;
	for (int32 i=0; i<iSize; i++)
	{
		CWndSize wnd = pApp->m_pConfigInfo->m_aWndSize[i];
		if (StrName == wnd.m_strID)
		{
			StrName = wnd.m_strTitle;
			iWidth = wnd.m_iWidth;
			iHeight = wnd.m_iHeight;

			if (0 == iWidth || 0 == iHeight)
			{
				bShowIE = TRUE;
			}
			break;
		}
	}

	if (bShowIE)
	{
		ShellExecute(0, L"open", StrUrl, NULL, NULL, SW_NORMAL);
	}
	else
	{
		//m_pDlgWebContent->ShowDlgIEWithSize(StrName, StrUrl,  CRect(0, 0, iWidth, iHeight));
		m_pDlgComment->ShowDlgIEWithSize(StrName, StrUrl,  CRect(0, 0, iWidth, iHeight));
	}
}



void CMainFrame::GetUserInfo(T_UserInfo &stUserInfo)
{
	stUserInfo = m_stUserInfo;
}



void CMainFrame::ShowRegisterDialog()
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if (NULL == pApp)
	{
		return;
	}

	CString StrUrl = pApp->m_pConfigInfo->m_StrRegUrl;
	CString StrName = pApp->m_pConfigInfo->m_StrBtName;
	if (0 >= pApp->m_pConfigInfo->m_StrBtName.GetLength())
	{
		StrName = L"用户注册";
	}

	bool32 bShowIE = FALSE;
	int32 iWidth = 1024, iHeight = 750;
	int32 iSize = pApp->m_pConfigInfo->m_aWndSize.GetSize();
	for (int32 i=0; i<iSize; i++)
	{
		CWndSize wnd = pApp->m_pConfigInfo->m_aWndSize[i];
		if (L"注册" == wnd.m_strID)
		{
			StrName = wnd.m_strTitle;
			iWidth = wnd.m_iWidth;
			iHeight = wnd.m_iHeight;

			if (0==iWidth || 0== iHeight)
			{
				bShowIE = TRUE;
			}

			break;
		}
	}

	if (bShowIE)
	{
		ShellExecute(0, L"open", StrUrl, NULL, NULL, SW_NORMAL);
	}
	else
	{
		m_pDlgComment->ShowDlgIEWithSize(StrName, StrUrl, CRect(0, 0, iWidth, iHeight));
	}
}

LRESULT CMainFrame::OnChooseStockResp( WPARAM wParam, LPARAM lParam )
{
	CMmiRespCRTEStategyChooseStock *pMmiRespChooseStock = (CMmiRespCRTEStategyChooseStock*)wParam;
	if ( ECSTShort == pMmiRespChooseStock->m_uTypes )
	{
		_MYTRACE(L"短线顺序:");
		for ( int32 i = 0; i < pMmiRespChooseStock->m_aMerchs.GetSize(); i++ )
		{
			const CMerchKey& Key = pMmiRespChooseStock->m_aMerchs.GetAt(i);

			_MYTRACE(L"<=== %d %s ===> \n", Key.m_iMarketId, Key.m_StrMerchCode);
		}
	}

	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;	
	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	if(!pAbsCenterManager)
	{
		return 0;
	}

	const CArray<CIoViewListner*, CIoViewListner*> *pstIoViewList = &pAbsCenterManager->GetIoViewList();
	if(pstIoViewList)
	{
		for ( int32 i=0; i < pstIoViewList->GetSize() ; i++ )
		{
			if(((*pstIoViewList)[i])->IsKindOfReportSelect())
			{
				CIoViewReportSelect *pReport = (CIoViewReportSelect*)((*pstIoViewList)[i]);
				pReport->OnChooseStockData(pMmiRespChooseStock);
			} 
		}
	}

	EngineCenterFree(pMmiRespChooseStock);

	return 0;
}

LRESULT CMainFrame::OnDapanStateResp( WPARAM wParam, LPARAM lParam )
{
	CMmiRespDapanState *pMmiRespDapanState = (CMmiRespDapanState*)wParam;
	if (NULL == pMmiRespDapanState)
	{
		return 0;
	}

	CCowBearState::Instance().SetCurrentCowBearState(pMmiRespDapanState->m_eDapanState);

	EngineCenterFree(pMmiRespDapanState);

	return 0;
}

LRESULT CMainFrame::OnChooseStockStatusResp(WPARAM wParam, LPARAM lParam)
{
	CMmiRespChooseStockStatus *pMmiRespChooseStockState = (CMmiRespChooseStockStatus*)wParam;
	if (NULL == pMmiRespChooseStockState)
	{
		return 0;
	}

	CChooseStockState::Instance().SetCurrentChooseStockState(pMmiRespChooseStockState->m_uStatus);

	EngineCenterFree(pMmiRespChooseStockState);

	return 0;
}

LRESULT CMainFrame::OnNewStockResp(WPARAM wParam, LPARAM lParam)
{
	string *pstrRecvData = (string*)wParam;
	bool bListedStock    = (bool)lParam;
	string  strRecvData = *pstrRecvData;
	delete pstrRecvData;
	
	CNewStockManager::Instance().OnNewStockResp(strRecvData.c_str(), bListedStock);
	
	return 0;
}

LRESULT CMainFrame::OnWebkeyDown( WPARAM wParam, LPARAM lParam )
{
	MSG* pMsg = (MSG*)wParam;
	if(pMsg)
	{
		ProcessHotkey(pMsg);
	}

	return 0;
}

void CMainFrame::OnMenuInvestmentAdviser( UINT nId )
{
	CString StrCefName = L"";
	switch(nId)
	{
	case ID_TEACHER_TEAM:
		{
			StrCefName = L"名师战队";
		}
		break;
	case ID_VIP_1V1:
		{
			StrCefName = L"VIP1V1";
		}
		break;
	default:
		break;
	}

	if(m_pNewWndTB && (!StrCefName.IsEmpty()))
	{
		m_pNewWndTB->DoOpenCfm(StrCefName);
	}
}

void CMainFrame::OnMenuNews( UINT nId )
{
	if(m_pNewWndTB)
	{
		CMPIChildFrame *pActiveFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, MDIGetActive());
		CIoViewNews *pIoViewNews = NULL;

		CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
		if(pApp && pApp->m_pConfigInfo->m_vNewsCenterTreeID.size() < 0)
		{
			return;
		}

		CString strParam;
		strParam.Format(L"treeid=%d", pApp->m_pConfigInfo->m_vNewsCenterTreeID[nId - ID_NEWS_TRANSFER].iTreeID);
		CString StrWebUrl = pIoViewNews->GetWebNewsUrl(strParam);

		if (NULL != pActiveFrame)
		{
			if(pActiveFrame->GetIdString() != L"网页资讯")
			{
				bool32 bLoad = m_pNewWndTB->DoOpenCfm(L"网页资讯", StrWebUrl);
				if ( bLoad )
				{
					pIoViewNews = FindIoViewNews(true);
				}
			}
			else
			{
				pIoViewNews = FindIoViewNews(true);
			}
		}

		if(NULL == pIoViewNews)
		{
			return;
		}

		pIoViewNews->RefreshWeb(StrWebUrl);
	}
}

void CMainFrame::OnMenuTrade( UINT nId )
{
	if(ID_DELEGATE_TRADE == nId)
	{
		ShowRealTradeDlg();
	}
	else if(ID_TRADE_SETTING == nId)
	{
		CDlgTradeSetting dlg;
		dlg.DoModal();
	}
}

void CMainFrame::OnMenuClassRoom()
{
	if(m_pNewWndTB)
	{
		//m_pNewWndTB->DoOpenCfm(L"点证课堂");
	}
}

void CMainFrame::OnMenuPhaseSort()
{
	ShowPhaseSort();
}

void CMainFrame::OnAutoRun()
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	CGGTongDoc* pDoc = (CGGTongDoc*)pApp->m_pDocument;
	if (pDoc)
	{
		pDoc->m_bAutoRun ^= 1;
		SetAutoRun(pDoc->m_bAutoRun);
	}
}

void CMainFrame::ShowSystemMenu( CRect rect )
{
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();

	CNewMenu* pPopMenu = NULL;
	pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, m_sysMenu.GetSubMenu(4));

	int32 iCount = pPopMenu->GetMenuItemCount();
	for ( int32 iDel=0; iDel < iCount ; iDel++  )
	{
		pPopMenu->RemoveMenu(0, MF_BYPOSITION);		// 清除以前的菜单项目
	}

	pPopMenu->AppendODMenu(L"数据下载", MF_STRING, ID_DOWNLOAD);
	pPopMenu->AppendODMenu(L"在线升级", MF_STRING, ID_UPDATE);
	if(pDoc->m_bAutoRun)
		pPopMenu->AppendODMenu(L"关闭开机启动", MF_STRING, ID_SYS_AUTO_RUN);
	else
		pPopMenu->AppendODMenu(L"开机启动", MF_STRING, ID_SYS_AUTO_RUN);

	if (pDoc->m_bAutoLogin)
	{
		pPopMenu->AppendODMenu(L"手动登录", MF_STRING, ID_MENU_AUTOCONNECT);
	}
	else
	{
		pPopMenu->AppendODMenu(L"自动登录", MF_STRING, ID_MENU_AUTOCONNECT);
	}

	pPopMenu->AppendODMenu(L"工具栏", MF_STRING, MSG_SCTOOLBAR_MENU);

	bool32 bConnectedData = pMain->m_wndStatusBar.m_bConnect;
	if(bConnectedData)
	{
		pPopMenu->AppendODMenu( L"断开行情", MF_STRING, ID_DISCONNECT_SERVER);
	}
	else
	{
		pPopMenu->AppendODMenu(L"连接行情", MF_STRING, ID_CONNECT_SERVER);
	}

	bool32 bConnectedNews = pMain->m_wndStatusBar.m_bConnectNews;
	if(bConnectedNews)
	{
		pPopMenu->AppendODMenu( L"断开资讯", MF_STRING, ID_DISCONNECT_NEWSSERVER);
	}
	else
	{
		pPopMenu->AppendODMenu(L"连接资讯", MF_STRING, ID_DISCONNECT_NEWSSERVER);
	}


	CNewMenu *childMenu = pPopMenu->AppendODPopupMenu(L"模版设置");
	childMenu->AppendODMenu(L"新建工作页面", MF_STRING, ID_NEW_CHILDFRAME);
	childMenu->AppendODMenu(L"打开工作页面", MF_STRING, ID_OPEN_CHILDFRAME);	
	childMenu->AppendODMenu(L"保存工作页面", MF_STRING, ID_SAVE_CUR_CHILDFRAME);	
	childMenu->AppendODMenu(L"另存工作页面", MF_STRING, ID_SAVEAS_CHILDFRAME);	
	childMenu->AppendODMenu( L"锁定页面设置", MF_STRING, ID_LOCK_CUR_CHILDFRAME);	
	childMenu->AppendODMenu( L"版面布局调整", MF_STRING, ID_LAYOUT_ADJUST);	
	pPopMenu->AppendODMenu(L"栏目设置", MF_STRING, ID_GRIDHEAD_SET);


	pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL, rect.left, rect.bottom , this);
}

void CMainFrame::ShowRealTradeDlg()
{
	CString StrExePath;
	if ( GetTradeExePath(StrExePath, true)
		&& !StrExePath.IsEmpty() )
	{
		CFileFind FileFind;
		if ( FileFind.FindFile(StrExePath) )
		{
			FileFind.FindNextFile();
			STARTUPINFO stinfo = {0};
			stinfo.cb = sizeof(stinfo);
			PROCESS_INFORMATION pi = {0};

			CString StrDir = FileFind.GetFilePath();
			int iDirPos = StrDir.ReverseFind(_T('\\'));
			if ( iDirPos >= 0 )
			{
				StrDir = StrDir.Left(iDirPos);
			}
			else
			{
				ASSERT( 0 );
				StrDir.Empty();
			}

			LPCTSTR pszDir = StrDir.IsEmpty()? NULL : StrDir.operator LPCTSTR();

			if ( CreateProcess(StrExePath, NULL, NULL, NULL, FALSE, 0, NULL, pszDir, &stinfo, &pi) )
			{
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
			}
			else
			{
				TCHAR szErr[100];
				_sntprintf(szErr, 100, _T("执行交易程序失败(%u)！"), GetLastError());
				MessageBox(szErr, _T("错误"));
				SetTradeExePath(NULL);	// 清除不能启动的交易配置
			}
		}
		else
		{
			MessageBox(_T("交易程序已不存在！"), _T("错误"));
			SetTradeExePath(NULL);	// 清除不能启动的交易配置
		}
	}
}

void CMainFrame::ShowQuoteMenu(CRect rect)
{
	CGGTongApp* pApp = DYNAMIC_DOWNCAST(CGGTongApp, AfxGetApp());

	if(NULL == pApp)
	{
		return;
	}

	CNewMenu* pPopMenu = NULL;
	pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, m_sysMenu.GetSubMenu(0));

	int32 iCount = pPopMenu->GetMenuItemCount();
	for ( int32 iDel=0; iDel < iCount ; iDel++  )
	{
		pPopMenu->RemoveMenu(0, MF_BYPOSITION);		// 清除以前的菜单项目
	}

	int32 iSize = pApp->m_pConfigInfo->m_vecQuoteMarket.size();
	if(0 >= iSize)
	{
		return;
	}

	for(int iMarketIndex = 0; iMarketIndex < iSize; iMarketIndex++)
	{
		T_QuoteMarketItem tMarketItem = pApp->m_pConfigInfo->m_vecQuoteMarket[iMarketIndex];
		pPopMenu->AppendODMenu(tMarketItem.StrMarketName, MF_STRING, (BTN_ID_QUOTE_HSAG + iMarketIndex));
	}

	pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL, rect.left, rect.bottom , this);
}

void CMainFrame::OnMenuQuoteMarket( UINT nID )
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	int32 iSize = pApp->m_pConfigInfo->m_vecQuoteMarket.size();
	if(0 >= iSize)
	{
		return;
	}

	if(m_pNewWndTB)
	{
		T_QuoteMarketItem tMarketItem = pApp->m_pConfigInfo->m_vecQuoteMarket[nID - BTN_ID_QUOTE_HSAG];
		if(!tMarketItem.StrMarketCfmName.IsEmpty())
		{
			m_pNewWndTB->DoOpenCfm(tMarketItem.StrMarketCfmName);
		}
	}
}

void CMainFrame::ShowFuncMenu( CRect rect )
{
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();

	CNewMenu* pPopMenu = NULL;
	pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, m_sysMenu.GetSubMenu(0));

	int32 iCount = pPopMenu->GetMenuItemCount();
	for ( int32 iDel=0; iDel < iCount ; iDel++  )
	{
		pPopMenu->RemoveMenu(0, MF_BYPOSITION);		// 清除以前的菜单项目
	}

	if( pDoc )
	{
		if(!pDoc->m_bShowSCToolBar)
		{
			pPopMenu->AppendODMenu(L"显示工具栏", MF_STRING, ID_SHOW_SHORTCUT_BAR);
		}
		else
		{
			pPopMenu->AppendODMenu(L"隐藏工具栏", MF_STRING, ID_SHOW_SHORTCUT_BAR);
		}
	}
	pPopMenu->AppendODMenu(L"公式管理", MF_STRING, ID_FORMULA_MAG);
	CNewMenu *childMenu = pPopMenu->AppendODPopupMenu(L"选股器");
	childMenu->AppendODMenu(L"条件选股器", MF_STRING, ID_SELECT_STOCK);
	childMenu->AppendODMenu(L"定制选股器", MF_STRING, ID_CUSTOM_STOCK);	

	pPopMenu->AppendODMenu(L"条件预警", MF_STRING, ID_FUNC_CONDITIONALARM);
	pPopMenu->AppendODMenu(L"阶段排行", MF_STRING, IDC_INTERVAL_SORT);
	pPopMenu->AppendODMenu(L"闪电图", MF_STRING, ID_TREND_SHOWTICK);
	pPopMenu->AppendODMenu(L"套利", MF_STRING, ID_SPECIAL_FUNC_ARBITRAGE);
	pPopMenu->AppendODMenu(L"多股同列", MF_STRING, ID_PIC_CFM_MULTIMERCH);
	pPopMenu->AppendODMenu(L"多周期同列", MF_STRING, ID_PIC_CFM_MULTIINTERVAL);


	pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL, rect.left, rect.bottom , this);
}

void CMainFrame::OnShowShortcutBar()
{
	OnMenuShowToolBar();
}




/////////////////////////////////////////////////////
//===================CODE CLEAN======================
//////////////////////////////////////////////////////
//void CMainFrame::PackJsonType(string &strTransDataUtf8)
//{
//	Json::Value jsValue;
//	CString strToken;
//	string strFieldTemp;
//
//
//	jsValue.clear();
//	jsValue["typeCode"]		= strTransDataUtf8;
//
//
//	string strWebDisData = jsValue.toStyledString();
//
//	int index = 0;
//	if( !strWebDisData.empty())
//	{
//		while( (index = strWebDisData.find('\n',index)) != string::npos)
//		{
//			strWebDisData.erase(index,1);
//		}
//	}
//
//	CString strJson = strWebDisData.c_str();
//	strTransDataUtf8.clear();
//	strTransDataUtf8= CStringA(strJson);
//	//UnicodeToUtf8(strJson, strTransDataUtf8);
//}
//
//
//void CMainFrame::UnPackJsonPickModelTypeInfo(string strMerchData)
//{
//
//	vector<int>::iterator result ;
//	map<int32,T_SmartStockPickItem> mapPickModelInfo;
//	mapPickModelInfo.clear();
//	
//
//	Json::Reader reader;
//	Json::Value value;
//	if (reader.parse(strMerchData.c_str(), value))
//	{   
//		if (value.size() <= 0)
//		{
//			return;
//		}
//
//		if (!value.isMember("result")) 
//		{
//			return;
//		}
//		
//		int iTypeCode = INVALID_ID;
//		const Json::Value arrayData = value["result"];
//		for (unsigned int i = 0; i < arrayData.size(); i++)
//		{   
//			T_SmartStockPickItem stSmartStockPickItem;
//			CString strGetTemp;
//
//			int32 iModeId = 0;
//			CString strMerchCode;
//			if (!arrayData[i].isMember("modeId")) 
//				continue;
//			iModeId = arrayData[i]["modeId"].asInt();
//			stSmartStockPickItem.m_iModeId = SMARTSTOCK_PICKMODEL_ID_BEGIN + iModeId;
//
//			if (!arrayData[i].isMember("typeCode")) 
//				continue;
//			CString strTypeCode = arrayData[i]["typeCode"].asCString();
//			iTypeCode = _ttoi(strTypeCode);
//
//			if (!arrayData[i].isMember("modeCode")) 
//				continue;
//			stSmartStockPickItem.m_strmodeCode = arrayData[i]["modeCode"].asCString();
//
//			if (!arrayData[i].isMember("modeName")) 
//				continue;
//			stSmartStockPickItem.m_strModeName = arrayData[i]["modeName"].asCString();
//
//			if (!arrayData[i].isMember("modeDetail")) 
//				continue;
//			stSmartStockPickItem.m_strModeDetail = arrayData[i]["modeDetail"].asCString();
//
//			if (INVALID_ID != iTypeCode)
//			{
//				mapPickModelInfo.insert(std::make_pair(stSmartStockPickItem.m_iModeId, stSmartStockPickItem));
//			}
//		}   
//
//		// 缓存选股类型组
//		result = find( m_vPickModelTypeGroup.begin( ), m_vPickModelTypeGroup.end( ), iTypeCode); 
//		if (result != m_vPickModelTypeGroup.end())
//		{
//			m_mapPickModelTypeGroup.insert(std::make_pair(iTypeCode, mapPickModelInfo));
//		}
//	}
//}
//
//void CMainFrame::UnPackJsonPickModelTypeStatus(string strMerchData)
//{
//	vector<int>::iterator result ;
//	m_mapPickModelStatusGroup.clear();
//	map<int32,T_SmartStockPickItem> mapPickModelInfo;
//	mapPickModelInfo.clear();
//
//	Json::Reader reader;
//	Json::Value value;
//	if (reader.parse(strMerchData.c_str(), value))
//	{   
//		if (value.size() <= 0)
//		{
//			return;
//		}
//
//		if (!value.isMember("result")) 
//		{
//			return;
//		}
//
//		int iTypeCode = INVALID_ID;
//		const Json::Value arrayData = value["result"];
//		for (unsigned int i = 0; i < arrayData.size(); i++)
//		{   
//			T_SmartStockPickItem stSmartStockPickItem;
//			CString strGetTemp;
//
//			int32 iModeId = 0;
//			CString strMerchCode;
//			if (!arrayData[i].isMember("modeId")) 
//				continue;
//			iModeId = arrayData[i]["modeId"].asInt();
//			stSmartStockPickItem.m_iModeId = SMARTSTOCK_PICKMODEL_ID_BEGIN + iModeId;
//
//			if (!arrayData[i].isMember("typeCode")) 
//				continue;
//			CString strTypeCode = arrayData[i]["typeCode"].asCString();
//			iTypeCode = _ttoi(strTypeCode);
//
//			if (!arrayData[i].isMember("modeCode")) 
//				continue;
//			stSmartStockPickItem.m_strmodeCode = arrayData[i]["modeCode"].asCString();
//
//			if (!arrayData[i].isMember("num")) 
//				continue;
//			stSmartStockPickItem.m_ihasSmartStock = arrayData[i]["num"].asInt();
//
//			if (INVALID_ID != iTypeCode)
//			{
//				mapPickModelInfo.insert(std::make_pair(stSmartStockPickItem.m_iModeId, stSmartStockPickItem));
//			}
//		}   
//
//		// 缓存选股类型组
//		result = find( m_vPickModelTypeGroup.begin( ), m_vPickModelTypeGroup.end( ), iTypeCode); 
//		if (result != m_vPickModelTypeGroup.end())
//		{
//			m_mapPickModelStatusGroup.insert(std::make_pair(iTypeCode, mapPickModelInfo));
//		}
//	}
//}
//
//void CMainFrame::QueryPickModelTypeInfo()
//{
//	bool bDone = false;
//	string strErr = "";
//	CString strPickModelIp;
//	int		iPort;		
//
//	CGGTongApp *pApp = (CGGTongApp*) AfxGetApp();	
//	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
//	if (pDoc)
//	{
//		strPickModelIp = pDoc->GetPickModelIP();
//		iPort		 =  pDoc->GetPickModelPort();
//	}
//	if (strPickModelIp.IsEmpty())
//	{
//		return;
//	}
//
//	// 请求选股模型数据
//	if (pDoc->m_pAutherManager)
//	{
//		CString strAddrFormat;
//		wstring strAddr;
//		strAddrFormat.Format(_T("%s:%d"), strPickModelIp, iPort);
//		int iIndex = strAddrFormat.Find(_T("http://"));
//		if (-1 == iIndex)
//		{
//			strAddrFormat = _T("http://") + strAddrFormat;
//		}
//		strAddr = strAddrFormat;
//
//		// 取token和组织包头
//		CString strToken;
//		CGGTongApp *pApp  =  (CGGTongApp*) AfxGetApp();
//		CGGTongDoc *pDoc = pApp->m_pDocument;
//		if (NULL != pDoc->m_pAutherManager)
//		{
//			strToken = pDoc->m_pAutherManager->GetInterface()->GetServiceDispose()->GetToken();
//		}
//		
//		CString strFomat;
//		strFomat.Format(_T("Content-Type: application/json;charset:utf-8\r\ntoken:%s"), strToken);
//
//		// 请求之前清空掉所有数据
//		m_mapPickModelTypeGroup.clear();
//		m_mapPickModelStatusGroup.clear();
//		for (int i = 0; i < m_vPickModelTypeGroup.size(); ++i)
//		{
//			string strTypeCode = "";
//			CString strTypeCodeFomat;
//			strTypeCodeFomat.Format(_T("%d"), m_vPickModelTypeGroup.at(i));
//			strTypeCode = CStringA(strTypeCodeFomat);
//			PackJsonType(strTypeCode);
//			pDoc->m_pAutherManager->GetInterface()->ReqPickModelInfo(strAddr.c_str(), L"/stock/query_modes", strTypeCode.c_str(), strFomat);
//		}
//	}
//}
//
//
//void CMainFrame::QueryPickModelTypeStatus()
//{
//	bool bDone = false;
//	string strErr = "";
//	CString strPickModelIp;
//	int		iPort;		
//
//	CGGTongApp *pApp = (CGGTongApp*) AfxGetApp();	
//	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
//	if (pDoc)
//	{
//		strPickModelIp = pDoc->GetPickModelIP();
//		iPort		 =  pDoc->GetPickModelPort();
//	}
//	if (strPickModelIp.IsEmpty())
//	{
//		return;
//	}
//
//	// 请求选股模型数据
//	if (pDoc->m_pAutherManager)
//	{
//		CString strAddrFormat;
//		wstring strAddr;
//		strAddrFormat.Format(_T("%s:%d"), strPickModelIp, iPort);
//		int iIndex = strAddrFormat.Find(_T("http://"));
//		if (-1 == iIndex)
//		{
//			strAddrFormat = _T("http://") + strAddrFormat;
//		}
//		strAddr = strAddrFormat;
//
//		CString strToken;
//		CGGTongApp *pApp  =  (CGGTongApp*) AfxGetApp();
//		CGGTongDoc *pDoc = pApp->m_pDocument;
//		if (NULL != pDoc->m_pAutherManager)
//		{
//			strToken = pDoc->m_pAutherManager->GetInterface()->GetServiceDispose()->GetToken();
//		}
//
//		CString strFomat;
//		strFomat.Format(_T("Content-Type: application/json;charset:utf-8\r\ntoken:%s"), strToken);
//
//		for (int i = 0;  i < m_vPickModelTypeGroup.size(); ++i)
//		{	
//			CString strTypeCode;
//			strTypeCode.Format(_T("%d"),	m_vPickModelTypeGroup.at(i));
//			string strJson = CStringA(strTypeCode);
//			PackJsonType(strJson);
//			// /stock/query_modes
//			pDoc->m_pAutherManager->GetInterface()->ReqPickModelStatus(strAddr.c_str(), L"/stock/query_mode_num", strJson.c_str(), strFomat);
//			
//		}
//		
//	}
//}

// 
LRESULT	CMainFrame::OnToolbarCommand(WPARAM wParam, LPARAM lParam)
{
	UINT uID = (UINT)wParam;
	switch(uID)
	{
	case ID_DELEGATE_TRADE:
		{
			ShowRealTradeDlg();
		}
		break;
	case ID_NEW_CHILDFRAME:
		{
			OnNewCfm();
		}
		break;
		//case ID_TOOLBAR_RIGHT:
		//	{
		//		//
		//		bool32 bShow = m_RightToolBar.IsWindowVisible();

		//		ControlDisStatus(!bShow);
		//		m_wndStatusBar.ChangeSideBtnStatus(!bShow);
		//	}
		//	break;
	default:
		break;
	}

	return 0;
}


void CMainFrame::ElapseLogin(UINT uSecond)
{
	SetTimer(KTimerStartUpLogin, uSecond, NULL);
}

void CMainFrame::Login(bool32 bCloseExit,bool32 bExitQuery)
{
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	ASSERT(NULL!= pDoc);
	if (NULL == pDoc || NULL == pDoc->m_pAbsCenterManager)
	{
		return;
	}

	CString tOldUser = pDoc->m_pAbsCenterManager->GetUserName();
	bool32  bOldDefUser = pDoc->m_isDefaultUser;
	//	开始登录
	bool32	bLoginSuc = pDoc->Login();	
	pDoc->m_pAbsCenterManager->SetFrameHwnd(GetSafeHwnd());		//	登录结束，消息窗体复位	
	m_bLoginSucc = bLoginSuc;
	if(!bLoginSuc)
	{			
		if (bCloseExit)
		{	
			//	退出吧
			PostMessage(WM_CLOSE);
			return;
		}
	}
	else
	{
		//	登录成功，则初始化动作
		Init();														//	初始化
		pDoc->m_pAbsCenterManager->ForceUpdateVisibleAttentIoView();		//	强制刷新关注商品		
	}

}
void	CMainFrame::Uninit()
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if (NULL== pApp || NULL == pDoc || NULL == pDoc->m_pAbsCenterManager)
	{
		return;
	}


	//	清除用户头像
	if (NULL != m_pImgUserPhoto)
	{
		DEL(m_pImgUserPhoto);
	}

	//	标题栏还原为默认状态（显示注册，登录，隐藏用户按钮）按钮
	{

		if (m_mapMenuBtn.end() != m_mapMenuBtn.find(ID_MENU_USER_NAME))
		{
			m_mapMenuBtn[ID_MENU_USER_NAME].SetNotCreate();
		}
		if (m_mapMenuBtn.find(ID_MENU_LOGIN) != m_mapMenuBtn.end())
		{
			m_mapMenuBtn[ID_MENU_LOGIN].SetCreate();
		}
		if (m_mapMenuBtn.find(ID_MENU_EXIT) != m_mapMenuBtn.end())
		{
			m_mapMenuBtn[ID_MENU_EXIT].SetNotCreate();
		}
		if (m_mapMenuBtn.find(ID_MENU_REGISTER) != m_mapMenuBtn.end())
		{
			m_mapMenuBtn[ID_MENU_REGISTER].SetCreate();
		}

		//	标题栏绘制
		DrawCustomNcClient();
	}	

	//	消息中心反初始化
	if(pDoc && pDoc->m_pHttpClient)
	{
		//消息中心后台释放（断开消息中心主线程，重连线程，收发线程）
		DEL(pDoc->m_pHttpClient);
	}


	//	目前先这样处理， 关闭状态栏中的实时监测网络心跳包，关注商品
	m_wndStatusBar.UnInit();

	//权限清掉
	CPluginFuncRight::Instance().Clear();

	//	清空用户行为记录
	m_vUserDealRecord.clear();

	//	viewdata重置(断开通讯链路，大市场管理器重置，定时器重置，登录信息重置，...)
	pDoc->m_pAbsCenterManager->ReSetLogData();

	KillTimer(KTimerIdChooseStockState);	//	删除选股状态定时器
	KillTimer(KTimerPickModelStatus);		//	删除智能选股定时器

	//	退出时显示登录对话框
	m_bShowLoginDlg = false;


}
void CMainFrame::Init()
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if (NULL== pApp || NULL == pDoc || NULL == pDoc->m_pAbsCenterManager || NULL == pDoc->m_pAutherManager)
	{
		return;
	}



	// 更新选股定时器
	KillTimer(KTimerPickModelStatus);

	if (1)
	{
		SetTimer(KTimerPickModelStatus, KTimerPickModelStatusTime, NULL);	//	定时请求智能选股状态
	}
	

	SetTimer(KTimerInfoCenter, KTimerIntervalTime * 60 * 1000, NULL);
	SetTimer(KTimerIdRefreshToken, KTimerPeriodRefreshToken * 60 * 1000, NULL);
	// 1ms请求一次选股状态
	//SetTimer(KTimerIdChooseStockState, KTimerPeriodChooseStockState, NULL);

	if(!pDoc->m_isDefaultUser && pApp->m_pConfigInfo->m_stUserDealRecord.bRecord) 
	{
		SetTimer(KTimerIdUserDealRecord, KTimerPeriodUserDealRecord * 60 * 1000, NULL);
	}

	pDoc->m_pAbsCenterManager->SetFrameHwnd(GetSafeHwnd());
	//	主界面标题栏昵称
	pDoc->m_pAbsCenterManager->GetServiceDispose()->GetUserInfo(m_stUserInfo);
	//	如果是脱机，则直接用登录账户赋值昵称
	if (pDoc->m_pAbsCenterManager->IsOffLineLogin())
	{
		_tcscpy_s(m_stUserInfo.wszUserName, sizeof(m_stUserInfo.wszUserName)/sizeof(m_stUserInfo.wszUserName[0]), pDoc->m_pAbsCenterManager->GetUserName().GetBuffer());
		_tcscpy_s(m_stUserInfo.wszNickName, sizeof(m_stUserInfo.wszNickName)/sizeof(m_stUserInfo.wszUserName[0]), pDoc->m_pAbsCenterManager->GetUserName().GetBuffer());	
	}
	// 创建私有目录
	CString StrPrivatePath;
	StrPrivatePath = CPathFactory::GetPrivateConfigPath(pDoc->m_pAbsCenterManager->GetUserName());
	_tcheck_if_mkdir(StrPrivatePath.GetBuffer(MAX_PATH));
	StrPrivatePath.ReleaseBuffer();
	StrPrivatePath = CPathFactory::GetPrivateWorkspacePath(pDoc->m_pAbsCenterManager->GetUserName());
	_tcheck_if_mkdir(StrPrivatePath.GetBuffer(MAX_PATH));
	StrPrivatePath.ReleaseBuffer();


	//	读取用户列表头，字体
	pDoc->PostInit();
	//	读取用户版面
	CReportScheme::Instance()->SetPrivateFolderName(pDoc->m_pAbsCenterManager->GetUserName());

	// 初始化用户工作页面信息	
	CCfmManager::Instance().Initialize(pDoc->m_pAbsCenterManager->GetUserName());

	T_UserInfo stUserInfo;
	pDoc->m_pAbsCenterManager->GetServiceDispose()->GetUserInfo(stUserInfo);
	CPluginFuncRight::Instance().SetShowTipWnd(GetSafeHwnd(), stUserInfo.iGroupType);


	//	昵称大于4个字符则缩略显示
	int iNameLen = wcslen(m_stUserInfo.wszNickName);
	CString tNickName = m_stUserInfo.wszNickName;
	if(tNickName.IsEmpty())
	{
		tNickName = m_stUserInfo.wszMobile;
	}

	if (iNameLen > 4)
	{
		tNickName = tNickName.Left(4);
		tNickName += "...";
	}

	//	下载并显示用户头像
	if (NULL != m_pImgUserPhoto)
	{
		DEL(m_pImgUserPhoto);
	}

	CString strUserImg = m_stUserInfo.wszImg;
	if (!strUserImg.IsEmpty())
	{
		WebClient webClient;
		webClient.SetTimeouts(100, 200);
		webClient.DownloadFile(strUserImg,  L"image\\userPhoto.png");
	}



	if (!pDoc->m_isDefaultUser)
	{
		//	在非测试账户情况下，则不显示登录，注册按钮
		//	用户按钮设置账户昵称
		if (m_mapMenuBtn.find(ID_MENU_LOGIN) != m_mapMenuBtn.end())
		{
			m_mapMenuBtn[ID_MENU_LOGIN].SetNotCreate();
		}
		if (m_mapMenuBtn.find(ID_MENU_REGISTER) != m_mapMenuBtn.end())
		{
			m_mapMenuBtn[ID_MENU_REGISTER].SetNotCreate();
		}	
		if (m_mapMenuBtn.find(ID_MENU_EXIT) != m_mapMenuBtn.end())
		{
			m_mapMenuBtn[ID_MENU_EXIT].SetCreate();
		}
		if (m_mapMenuBtn.end() != m_mapMenuBtn.find(ID_MENU_USER_NAME))
		{
			m_mapMenuBtn[ID_MENU_USER_NAME].SetCaption(tNickName);
			m_mapMenuBtn[ID_MENU_USER_NAME].SetCreate();
		}
		m_pImgUserPhoto = Image::FromFile(L"image\\userPhoto.png");	
	}
	else
	{
		//	测试账户情况下隐藏用户按钮
		if (m_mapMenuBtn.end() != m_mapMenuBtn.find(ID_MENU_USER_NAME))
		{
			m_mapMenuBtn[ID_MENU_USER_NAME].SetNotCreate();
		}
		if (m_mapMenuBtn.find(ID_MENU_LOGIN) != m_mapMenuBtn.end())
		{
			m_mapMenuBtn[ID_MENU_LOGIN].SetCreate();
		}
		if (m_mapMenuBtn.find(ID_MENU_EXIT) != m_mapMenuBtn.end())
		{
			m_mapMenuBtn[ID_MENU_EXIT].SetNotCreate();
		}
		if (m_mapMenuBtn.find(ID_MENU_REGISTER) != m_mapMenuBtn.end())
		{
			m_mapMenuBtn[ID_MENU_REGISTER].SetCreate();
		}	
	}
	//	重新绘制下主界面标题栏(及时刷新更改内容)
	DrawCustomNcClient();



	//	主界面状态栏需要重新请求关注商品
	m_wndStatusBar.InitStatusEx();
	//	

	if(pDoc && (NULL==pDoc->m_pHttpClient) && pDoc->GetReqMessageInfoStatus())
	{
		pDoc->m_pHttpClient = new http_client::CHttpClient;
		// 设置消息中心响应的回调函数
		pDoc->m_pHttpClient->SetCallHttpData(RecvMsgCenterData);		//	消息中心消息接收处理
		pDoc->m_pHttpClient->SetCallHttpConn((http_client::CALL_HTTP_CONN_NOTIFY)OnConnNotify);		//	消息中心连接处理		//	消息中心连接处理
		pDoc->m_pHttpClient->Construct();
	}

	if(pDoc && pDoc->m_pHttpClient)
	{
		// 将token和userid保存到消息中心动态库中
		std::string strToken = pDoc->m_pAutherManager->GetInterface()->GetServiceDispose()->GetToken();

		char szUserID[32]={0};
		sprintf(szUserID, "%d", m_stUserInfo.iUserId);
		pDoc->m_pHttpClient->SetTokenAndUserID(strToken, szUserID);

		// 连接消息中心服务器
		CString strAddress = pDoc->GetInfoCenterIP();
		int nPort = pDoc->GetInfoCenterPort();
		pDoc->m_pHttpClient->ConnectServer(strAddress, nPort);
	}

	//	加载新的默认子框架，删除旧账号的打开子框架
	if (NULL != m_pNewCaptionWndTB)
	{
		//	删除历史标签版面
		m_pNewCaptionWndTB->CloseAllPage();	
		//	必须重新加载版面，重新从merchmanager中获取数据
		CString StrDefaultCfmName = CCfmManager::Instance().GetUserDefaultCfm().m_StrXmlName;
		if ( !StrDefaultCfmName.IsEmpty() )
		{					
			CCfmManager::Instance().LoadCfm(StrDefaultCfmName, true);
		}
	}

	if(m_pNewWndTB)
	{
		m_pNewWndTB->SetDefaultCfmStatus();
	}

	static bool32 bOnlyInit = true;
	//	第一次登录
	if (bOnlyInit)
	{
		ShowNotifyMsg();
		//
		SendMessage(UM_ViewData_OnAllMarketInitializeSuccess, 0, 0);

		//	禁用框架默认菜单
		SetMenu(NULL);

		//// 保存登录界面左侧的图片
		CString StrLoginCode = CConfigInfo::Instance()->m_StrLoginLeftCode;
		CString StrLoginImg = CConfigInfo::Instance()->GetAdvsCenterUrlByCode(StrLoginCode, EAUTPit);
		//	下载登录窗体宣传图
		if (!StrLoginImg.IsEmpty())
		{
			WebClient webClient;
			webClient.SetTimeouts(100, 200);
			webClient.DownloadFile(StrLoginImg,  L"image\\banner.png");
		}

		//	显示欢迎
		ShowWelcome();

		bOnlyInit = false;
	}

	//	启用定时器(行情实时推送，服务时间，自动清空内存,...)
	pDoc->m_pAbsCenterManager->InitPush(true);

	m_bDoLogin = true;			//	标示是否登录过
	m_bShowLoginDlg = false;	//	退出时显示登录对话框	

	//	指标初始化
	{
		SetAbsCenterManager(pDoc->m_pAbsCenterManager);
		//SetUserName(pDoc->m_pAbsCenterManager->GetUserName());	//	切换账号后，需要更改指标全局中的用户名
		CFormulaLib::instance()->Reload();
	}

	/////////////////////////////////////////////////////
	//===================CODE CLEAN======================
	//////////////////////////////////////////////////////
	//// 选股模型查询
	//if (1)
	//{
	//	QueryPickModelTypeInfo();
	//	QueryPickModelTypeStatus();
	//}
	
	// 请求新股数据
	CNewStockManager::Instance().RequestNewStockData();



	// 登录时同步一次工具栏状态
	// 设置侧边栏状态
	//if (pDoc->m_bShowSCToolBar)
	//{
	//	ShowControlBar(&m_ShortCutMenuToolBar, TRUE , TRUE);
	//}
	//else
	//{
	//	ShowControlBar(&m_ShortCutMenuToolBar, false , TRUE);
	//}
}

void CMainFrame::ShowMenu(T_MenuItemData tData, E_MenuItemType eType, CRect rect)
{
	if (NULL == m_pDlgPullDown)
	{
		//ASSERT(0);
		return;
	}

	if (m_pDlgPullDown->IsWindowVisible())
	{
		m_pDlgPullDown->ShowWindow(FALSE);
		return;
	}

	CGGTongApp *pApp = DYNAMIC_DOWNCAST(CGGTongApp, AfxGetApp());
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();

	if (!pApp)
	{
		return;
	}

	bool bNeedCreateSubMenu = true;

	if (!m_vecDlgSubPullDown.empty())
	{
		if (tData.strText == m_pDlgPullDown->GetParentTitle())
		{
			bNeedCreateSubMenu = false;
		}
	}

	int iHeight = 0;
	int iWidth = 0;
	int iTop = 0;
	int iBottom = 0;

	m_mapPullDownBtns.clear();
	m_mapPullDown.clear();
	if(m_mapMenuCollect.size() <= 0)
	{
		return;
	}
	int32 iMenuItemSize = 0;
	VecMenuContainer vecMenu;
	if(m_mapMenuCollect.count(eType) != 0)
	{
		vecMenu = m_mapMenuCollect[eType];
		iMenuItemSize = vecMenu.size();
	}
	if(iMenuItemSize <= 0)
	{
		return;
	}
	for (int32 i=0; i<iMenuItemSize; i++)
	{
		T_MenuItemData tempData = vecMenu[i];

		CNCButton btnNow;


		if(tempData.pImgData)
		{
			iWidth = tempData.pImgData->GetWidth();
			iHeight = tempData.pImgData->GetHeight()/3;
		}
		else
		{
			iWidth = 90;
			iHeight = 31;
		}

		iBottom += iHeight;
		CRect rtNow(0, iTop, iWidth, iBottom);
		btnNow.CreateButton(tempData.strText,  rtNow, m_pDlgPullDown, tempData.pImgData, 3, tempData.ID);

		btnNow.SetFont(m_fontMenuItem);
		btnNow.SetTextColor(RGB(200,200,200), RGB(230,70,70), RGB(230,70,70));

		// 如果子菜单已经创建，那么只需要调整子菜单的位置即可
		if (!bNeedCreateSubMenu)
		{
			CWnd *pSubMenu = AdjustSubMenuPos(btnNow.GetCaption(), (rect.left+iWidth + 1), i * iHeight + rect.bottom);
			if (pSubMenu)
			{
				btnNow.AddChildMenu(pSubMenu);
			}
		}
		else// 创建子菜单
		{
			E_MenuItemType eChlidtype = (E_MenuItemType)(FindFirstVecIDByMenuID(tempData.ID));
			CWnd *pSubMenu = CreateSubMenu(tempData, eChlidtype, (rect.left + iWidth + 1), i * iHeight + rect.bottom);
			if (pSubMenu)
			{
				btnNow.AddChildMenu(pSubMenu);
			}
		}

		// 处理功能菜单的连接行情主站，断开行情主站，连接资讯主站，断开自选主站、自动登录等按钮的状态
		if(eType == EMIT_FIR_SYSTEM)
		{
			if(ID_DISCONNECT_SERVER == tempData.ID)
			{
				if (m_wndStatusBar.m_bConnect)
				{	
					btnNow.SetCaption(L"断开行情");
				}
				else
				{
					btnNow.SetCaption(L"连接行情");
				}
			}
			else if(ID_DISCONNECT_NEWSSERVER == tempData.ID)
			{
				if (m_wndStatusBar.m_bConnectNews)
				{
					btnNow.SetCaption(L"断开资讯");
				}
				else
				{
					btnNow.SetCaption(L"连接资讯");
				}
			}
			else if(ID_MENU_AUTOCONNECT == tempData.ID)
			{
				if(pDoc->m_bAutoLogin)
				{
					btnNow.SetCaption(_T("手动登录"));		
				}
				else
				{
					btnNow.SetCaption(_T("自动登录"));	
				}
			}
			else if(ID_SYS_AUTO_RUN == tempData.ID)
			{
				if(pDoc->m_bAutoRun)
				{
					btnNow.SetCaption(_T("关闭开机启动"));		
				}
				else
				{
					btnNow.SetCaption(_T("开机启动"));	
				}
			}

		}

		if(i == 0)
		{
			btnNow.SetTextOffPos(CPoint(-35, 4));
		}
		else
		{
			if(ID_MENU_EXIT == tempData.ID)
			{
				btnNow.SetTextOffPos(CPoint(-45, 0));
			}
			else if(IDC_INTERVAL_SORT == tempData.ID)
			{
				btnNow.SetTextOffPos(CPoint(-40, 0));
			}
			else if(ID_RESPONSIBILITY == tempData.ID)
			{
				btnNow.SetTextOffPos(CPoint(-41, 0));
			}
			else if(ID_APP_ABOUT == tempData.ID)
			{
				btnNow.SetTextOffPos(CPoint(-51, 0));
			}
			else if(ID_SYS_AUTO_RUN == tempData.ID)
			{
				if(pDoc->m_bAutoRun)
				{
					btnNow.SetTextOffPos(CPoint(-23, 0));	
				}
				else
				{
					btnNow.SetTextOffPos(CPoint(-35, 0));	
				}
			}
			else
			{
				btnNow.SetTextOffPos(CPoint(-35, 0));
			}
		}

		m_mapPullDownBtns[tempData.ID] = btnNow;
		m_mapPullDown.push_back(btnNow);

		iTop += iHeight;
	}

	int iX = rect.left;
	int iY = rect.bottom;
	CRect rcWnd(iX, iY, iX + iWidth, iY + iBottom);

	m_pDlgPullDown->SetParentTitle(tData.strText);
	m_pDlgPullDown->MoveWindow(rcWnd);
	m_pDlgPullDown->SetShowButtons(m_mapPullDown, rcWnd);

	m_pDlgPullDown->ShowWindow(TRUE);
}

CDlgPullDown* CMainFrame::CreateSubMenu( T_MenuItemData &tmenuData,E_MenuItemType eType, int left, int top )
{
	CDlgPullDown *pDlgPullDown = NULL;

	CGGTongApp *pApp = DYNAMIC_DOWNCAST(CGGTongApp, AfxGetApp());

	if (!pApp)
	{
		return pDlgPullDown;
	}

	int iSize = 0;
	VecMenuContainer vecMenu;
	if(m_mapMenuCollect.count(eType) != 0)
	{
		vecMenu = m_mapMenuCollect[eType];
		iSize = vecMenu.size();
	}

	if (iSize > 0)
	{
		static int index = 0;

		m_mapMorePullDown.clear();

		pDlgPullDown = new CDlgPullDown(m_pDlgPullDown);
		pDlgPullDown->Create(IDD_DIALOG_PULLDOWN, m_pDlgPullDown);
		m_vecDlgSubPullDown.push_back(pDlgPullDown);

		int pos = 0;
		vector<T_MenuItemData>::iterator iterSub;
		int nWidth = 0, nHeight = 0;
        int iTop = 0, iBottom = 0;
		for (iterSub = vecMenu.begin() ; iterSub != vecMenu.end() ; ++iterSub)
		{
			CNCButton btnSub;
	
			if (iterSub->pImgData && Ok == iterSub->pImgData->GetLastStatus())
			{
				nWidth = iterSub->pImgData->GetWidth();
				nHeight = iterSub->pImgData->GetHeight()/3;
				iBottom += nHeight;
				CRect rtNew(0, iTop, nWidth, iBottom);

				btnSub.CreateButton(iterSub->strText, rtNew, pDlgPullDown, iterSub->pImgData, 3, iterSub->ID);	
				btnSub.SetTextColor(RGB(200,200,200), RGB(230,70,70), RGB(230,70,70));
				btnSub.SetFont(m_fontMenuItem);
				if(iterSub == vecMenu.begin())
				{
					btnSub.SetTextOffPos(CPoint(-24, 4));
				}
				else
				{
					btnSub.SetTextOffPos(CPoint(-24, 0));
				}
				
				//btnSub.SetTextOffPos(CPoint(0,2));

				// 设置模板设置里面的开始页面设置(锁定页面设置)的状态
				CMPIChildFrame *pChildFrame = (CMPIChildFrame *)MDIGetActive();
				if(eType == EMIT_SEC_MODULESET)
				{
					if ( NULL != pChildFrame )
					{
						CString StrId = pChildFrame->GetIdString();
						T_CfmFileInfo cfm;
						BOOL bEnableLock = TRUE;
						if ( CCfmManager::Instance().QueryUserCfm(StrId, cfm) && cfm.m_bLockSplit )
						{
							bEnableLock = FALSE;	// 强制禁止改变lock状态
						}
						if ( pChildFrame->IsF7AutoLock() )
						{
							bEnableLock = FALSE;	// F7不允许改变
						}

						if(btnSub.GetControlId() == ID_LOCK_CUR_CHILDFRAME)
						{
							btnSub.EnableButton(bEnableLock);

							if (pChildFrame->IsLockedSplit())
							{
								if(btnSub.GetCaption() == L"锁定页面设置")
								{
									btnSub.SetCaption(_T("开始页面设置"));
								}
							}
							else
							{
								if(btnSub.GetCaption() == L"开始页面设置")
								{
									btnSub.SetCaption(_T("锁定页面设置"));
								}
							}
						}
					}
				}

				m_mapPullDownBtns[iterSub->ID] = btnSub;
				m_mapMorePullDown.push_back(btnSub);
                iTop += nHeight;
				pDlgPullDown->IncrementHeightAndWidth(nHeight, nWidth);
			}
		}

		nHeight = pDlgPullDown->GetHeight();
		CRect rt(left, top, left+nWidth, top+nHeight);
		CRect rcWnd;
		GetWindowRect(&rcWnd);
		if(rt.bottom > rcWnd.bottom)
		{
			rt.bottom = rcWnd.bottom;
		}
		pDlgPullDown->MoveWindow(rt);
		pDlgPullDown->SetShowButtons(m_mapMorePullDown, rt);

		pDlgPullDown->SetParentTitle(tmenuData.strText);
	}

	return pDlgPullDown;
}

CDlgPullDown* CMainFrame::AdjustSubMenuPos( CString strParent, int left, int top )
{
	CDlgPullDown* pDlgPullDown = NULL;

	vector<CDlgPullDown*>::iterator iter;
	for (iter = m_vecDlgSubPullDown.begin() ; iter != m_vecDlgSubPullDown.end() ; ++iter)
	{
		if ((*iter)->GetParentTitle() == strParent)
		{
			int nHeight = (*iter)->GetHeight();

			CRect rt(left, top, left+(*iter)->GetWidth(), top+nHeight);

			(*iter)->MoveWindow(rt);
			pDlgPullDown = *iter;
			break;
		}
	}

	return pDlgPullDown;
}

// 初始化所有的一级和二级菜单
void CMainFrame::InitMapMenuItem()
{
	m_mapMenuCollect.clear();
	VecMenuContainer vecMenu;
	vecMenu.clear();

	// 系统菜单
	T_MenuItemData tMenuData;
	tMenuData.ID = ID_DOWNLOAD;
	tMenuData.strText = L"数据下载";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgSysTop;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_UPDATE;
	tMenuData.strText = L"在线升级";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgItemSimple;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_SYS_AUTO_RUN;
	tMenuData.strText = L"开机启动";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgItemSimple;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_MENU_AUTOCONNECT;
	tMenuData.strText = L"自动登录";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgCenterSplitTop;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_DISCONNECT_SERVER;
	tMenuData.strText = L"断开行情";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgCenterSplitBottom;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_DISCONNECT_NEWSSERVER;
	tMenuData.strText = L"连接资讯";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgItemSimple;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_MODULE_SETTING;
	tMenuData.strText = L"模板设置";
	tMenuData.bShowArrow = true;
	tMenuData.pImgData = m_pImgItemArrow;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_GRIDHEAD_SET;
	tMenuData.strText = L"栏目设置";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgCenterSplitTop;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_MENU_EXIT;
	tMenuData.strText = L"退出";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgSysBottom;
	vecMenu.push_back(tMenuData);

	m_mapMenuCollect[EMIT_FIR_SYSTEM] = vecMenu;

	// 选股菜单
	vecMenu.clear();
	tMenuData.ID = ID_SELECT_STOCK;
	tMenuData.strText = L"条件选股器";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgSysTop;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_CUSTOM_STOCK;
	tMenuData.strText = L"定制选股器";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgItemSimple;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = IDC_INTERVAL_SORT;
	tMenuData.strText = L"阶段排行";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgHelpBottom;
	vecMenu.push_back(tMenuData);

	m_mapMenuCollect[EMIT_FIR_SEL_STOCK] = vecMenu;

	// 交易菜单
	vecMenu.clear();
	tMenuData.ID = ID_DELEGATE_TRADE;
	tMenuData.strText = L"委托交易";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgSysTop;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_TRADE_SETTING;
	tMenuData.strText = L"交易设置";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgHelpBottom;
	vecMenu.push_back(tMenuData);

	m_mapMenuCollect[EMIT_FIR_TRADE] = vecMenu;

	// 帮助菜单
	vecMenu.clear();
	tMenuData.ID = ID_USE_HELP;
	tMenuData.strText = L"帮助说明书";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgHelpTop;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_RESPONSIBILITY;
	tMenuData.strText = L"免责声明";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgCenterSplitBottom;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_APP_ABOUT;
	tMenuData.strText = L"关于";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgHelpBottom;
	vecMenu.push_back(tMenuData);

	m_mapMenuCollect[EMIT_FIR_HELP] = vecMenu;


	// 系统菜单项模板设置菜单
	vecMenu.clear();
	tMenuData.ID = ID_NEW_WORKSPACE;
	tMenuData.strText = L"新建工作页面";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgSysTop;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_OPEN_CHILDFRAME;
	tMenuData.strText = L"打开工作页面";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgItemSimple;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_SAVE_CUR_CHILDFRAME;
	tMenuData.strText = L"保存工作页面";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgItemSimple;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_SAVEAS_WORKSPACE;
	tMenuData.strText = L"另存工作页面";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgItemSimple;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_RESTORE_VIEWSP;
	tMenuData.strText = L"恢复系统页面";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgItemSimple;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_LOCK_CUR_CHILDFRAME;
	tMenuData.strText = L"锁定页面设置";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgItemSimple;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_LAYOUT_ADJUST;
	tMenuData.strText = L"版面布局调整";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgHelpBottom;
	vecMenu.push_back(tMenuData);

	m_mapMenuCollect[EMIT_SEC_MODULESET] = vecMenu;

}

// 根据一级菜单的菜单项ID寻找其二级菜单在整个m_mapMenuCollect中对应的ID，算法有待优化
int32 CMainFrame::FindFirstVecIDByMenuID( int32 iMenuID )
{
	// 有二级菜单的一级菜单项是模板设置, 如果新添加了其他的，需要在这里对应加上
	int32 iRetID = 0;      // 初始化返回值，以防出错
	if(iMenuID == ID_MODULE_SETTING)
	{
		iRetID = EMIT_SEC_MODULESET;
	}

	return iRetID;
}

CNCButton* CMainFrame::GetCNCButton(int iId)
{
	buttonContainerType::iterator it = m_mapPullDown.begin();
	FOR_EACH(m_mapPullDown, it)
	{
		if ((*it).GetControlId() == iId)
		{
			return &(*it);
		}
	}

	buttonContainerType::iterator itor = m_mapMorePullDown.begin();
	FOR_EACH(m_mapMorePullDown, itor)
	{
		if ((*itor).GetControlId() == iId)
		{
			return &(*itor);
		}
	}

	//理论容器中应该存在
	//ASSERT(0);
	return NULL;
}

