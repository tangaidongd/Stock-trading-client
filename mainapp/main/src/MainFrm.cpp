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
#define TITLE_HEIGHT			40			// �������߶�

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
static	CLoadLibHelperT<HtmlHelpWFunc> s_libHelper(_T("")/*_T("hhctrl.ocx")*/, "HtmlHelpW");	// ����htmlhelpew
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

// TOOLBAR ���������õ���ͼ��
WORD g_awToolBarIconIDs[] =
{
	IDB_BITMAP_MENUICO,
	20, 16,
	IDM_IOVIEWBASE_RETURN,			// ��������	
	ID_TOOL_ICON_NOUSE,				// ����ȫ��	
	ID_TOOL_ICON_NOUSE,				// ���ڻظ�	
	IDM_IOVIEWBASE_FONT_BIGGER,		// ����Ŵ�		
	IDM_IOVIEWBASE_FONT_SMALLER,	// ������С	
	NULL
};

/*
WORD g_awToolBarIconIDs[] =
{
//  ����ͼƬ�е�ÿ��һСͼ�� ID ��.û�õ�����ʱ�� ID_TOOL_ICON_NOUSE ���
IDR_TOOLBAR_GGTONG,
16, 16,

ID_PIC_TREND,			// ��ʱ		

ID_PIC_TICK,			// ����

ID_PIC_KLINE,			// K ��		

ID_PIC_PRICE,			// ������

ID_PIC_TIMESALE,		// �ֱ�

ID_PIC_NEWS,			// �Ƹ���Ѷ

ID_TOOL_ICON_NOUSE,		// F10

ID_PIC_DETAIL,			// �Ƹ�׷��

ID_PIC_REPORT,			// ���۱�

ID_TOOL_ICON_NOUSE,		// K ������

ID_TOOL_ICON_NOUSE,		// ѡ��ָ��

ID_FORMULA_MAG,			// ��ʽ����

ID_GRIDHEAD_SET,		// ��Ŀ����	

ID_BLOCK_SET,			// �������		

ID_SETTING,				// �������		

ID_LAYOUT_ADJUST,		// ���沼��		

ID_VIEW_SDBAR,			// �Ի���	

ID_UPDATE,				// �������

ID_APP_ABOUT,			// ���߰���

ID_WINDOW_NEW,			// �½�����

ID_TOOL_ICON_NOUSE,//ID_NEW_WORKSPACE,		// �½�����		

ID_SAVE_WORKSPACE,		// �������

IDM_IOVIEWBASE_RETURN,	// ��������			

IDM_IOVIEWBASE_FONT_BIGGER,		// ����Ŵ�		

IDM_IOVIEWBASE_FONT_SMALLER,	// ������С		

ID_TOOL_ICON_NOUSE,		// PAGE_UP

ID_TOOL_ICON_NOUSE,		// PAGE_DOWN

ID_TOOL_ICON_NOUSE,		// ˮƽ����	

ID_TOOL_ICON_NOUSE,		// ˮƽ����		

ID_TOOL_ICON_NOUSE,		// ��ֱ����

ID_TOOL_ICON_NOUSE,		// ��ֱ����

ID_TOOL_ICON_NOUSE,		// ����ȫ��		

ID_TOOL_ICON_NOUSE,		// ���ڻظ�

IDM_IOVIEWBASE_F7,		// ���ڷָ�	(���ͼ����ȫ���ָ��Ļ��ÿ���)	

ID_DISCONNECT_SERVER,	// �Ͽ�������

ID_CONNECT_SERVER,		// ����

ID_APP_EXIT,			// �˳�

ID_OPEN_WORKSPACE,		// �򿪰���

//ID_PIC_STARRY,			// �ǿ�ͼ

//ID_PIC_CAPITALFLOW,		// �ʽ�����



//ID_PIC_MAIN_TIMESALE,	// �������

ID_PIC_BLOCKREPORT,		// ���

ID_PIC_REPORT_RANK,		// ��������

ID_PIC_REPORT_SELECT,	// ��������

ID_VIEW_CALC,			// ������

ID_NEW_WORKSPACE,		// �½�����
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

// ����������ʱ��,�ж����ӳɹ�

// ��������
const UINT KuiTimesToWaitForConnect		  = (1000/500)*60*3;

// ��ʱ��ID
const UINT KTimerIdCheckConnectServer	  = 123456;
const UINT KTimerPeriodCheckConnectServer = 500;

const UINT KTimerIdCheckNewsConnectServer = 123457;
const UINT KTimerPeriodCheckNewsConnectServer = 500;

const UINT KTimerIdAutoUpdateCheck		  = 123461;			// ��̨�Զ�����
const UINT KTimerPeriodAutoUpdateCheck	  = 5000;

const UINT KTimerIdReqPushJinping		  = 123458;			// ��������
const UINT KTimerPeriodReqPushJinping	  = 1000 * 30;

const UINT KTimerIdIndexShow			  = 123459;	
const UINT KTimerPeriodIndexShow		  = 5;

// ��������,��Ϣ��ʾ����ʱ��
const UINT KTimerInfoCenter				  = 1000110;
const UINT KTimerIntervalTime			  = 1;

// ����ѡ��״̬��ʱ��
const UINT KTimerPickModelStatus		  = 1000111;
const UINT KTimerPickModelStatusTime	  = 1000*10;

// 10����ˢ��token
const UINT KTimerIdRefreshToken			  = 123462;	
const UINT KTimerPeriodRefreshToken		  = 10;

// 1���ӵ�һ���û���Ϊ��¼
const UINT KTimerIdUserDealRecord		  = 123463;	
const UINT KTimerPeriodUserDealRecord	  = 1;

const UINT KTimerIdDapanState             = 123466;			// ����״̬
const UINT KTimerPeriodDapanState         = 1000 ;			// 1 min ����һ�δ���״̬

const UINT KTimerIdChooseStockState       = 123468;			// ѡ��״̬
const UINT KTimerPeriodChooseStockState   = 1 ;			    // 1 ms ����һ��ѡ��״̬

const UINT KTimerStartUpLogin		      = 123469;	        // ������¼
const UINT KTimerPeriodStartUpLogin		  = 100 ;		    // 0.1s��������¼

const UINT KTimerCheckToken		          = 123470;	        // ���token�Ƿ���Ч
const UINT KTimerPeriodCheckToken		  = 1000*60 ;		// ���1min

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
	TCHAR   szCfmName[12];		// ҳ������
	bool32	bCreateIfNotExist;	// �����ڸ�ҳ���Ƿ񴴽�
	UINT	nSystemPicId;			// �Ƿ���ϵͳ���ҳ��, �������ϵͳ���Pic��������0
	UINT	nFunc;				// ����Ҫ��Ȩ��
};
static T_PicCfmInfo s_aPicCfmInfos[] = 
{
	{ID_PIC_TREND, _T("��ʱ����"), true, ID_PIC_TREND, 0},
	{ID_PIC_KLINE, _T("K�߷���"), true, ID_PIC_KLINE, 0},
	{ID_PIC_TICK, _T("����ͼ"), true, 0, 0},
	{ID_PIC_TIMESALE, _T("K�߷���"), false, ID_PIC_TREND, 0},		// Ĭ�ϼ��ط�ʱͼ����ķֱ�
	{ID_PIC_STARRY, _T("�ȵ��ھ�"), false, 0, 0},
	{ID_PIC_CAPITALFLOW, _T("K�߷���"), false, ID_PIC_KLINE, CPluginFuncRight::FuncCapitalFlow},
	{ID_PIC_MAIN_TIMESALE, _T("K�߷���"), false, ID_PIC_KLINE, 0},
	{ID_PIC_FENJIABIAO, _T("K�߷���"), false, ID_PIC_KLINE, 0},
	{ID_PIC_CHOUMA, _T("K�߷���"), false, ID_PIC_KLINE, 0},
	{ID_PIC_PHASESORT, _T("�׶�����"), true, 0, 0},					// �׶���������Ҫ���⴦���
	{ID_PIC_REPORT_ARBITRAGE, _T("�������۱�"), true, ID_PIC_REPORT_ARBITRAGE, 0}					// �������۱�
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
	ON_UPDATE_COMMAND_UI(IDM_WINDOW_CLOSE, OnUpdateCloseChildFrame)					// ����һϵ�д��ڷָ��
	ON_UPDATE_COMMAND_UI(ID_SAVE_CUR_CHILDFRAME, OnUpdateSaveChildFrame)
	ON_UPDATE_COMMAND_UI(ID_SAVEAS_CHILDFRAME, OnUpdateSaveChildFrame)
	ON_UPDATE_COMMAND_UI(ID_LOCK_CUR_CHILDFRAME, OnUpdateLockChildFrame)
	ON_UPDATE_COMMAND_UI_RANGE(ID_DEL_VIEW, ID_ADD_RIGHTVIEW, OnUpdateViewSplitCmd)
	ON_UPDATE_COMMAND_UI_RANGE(IDM_IOVIEWBASE_SPLIT_TOP, IDM_IOVIEWBASE_CLOSECUR, OnUpdateViewSplitCmd)
	ON_UPDATE_COMMAND_UI_RANGE(ID_WINDOW_SPLIT_ADD_RIGHT, ID_WINDOW_SPLIT_ADD_BOTTOM, OnUpdateViewSplitCmd)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_SPLIT_DEL, OnUpdateViewSplitCmd)
	ON_UPDATE_COMMAND_UI_RANGE(ID_PIC_BEGIN, ID_PIC_END, OnUpdatePicMenu)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MAIN_MENU_START, ID_MAIN_MENU_END, OnUpdateMainMenu)	// �˵������ȡʲô���ֺ���
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
	ON_MESSAGE(UM_ViewData_OnNewsTitleUpdate, OnMsgViewDataOnNewsTitleUpdate)//�Ƹ���Ѷ2013
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
	//// ����ѡ������ذ�
	//ON_MESSAGE(UM_PickModelTypeInfo,OnMsgPickModelTypeInfo)
	//ON_MESSAGE(UM_PickModelTypeStatus,OnMsgPickModelTypeStatus)
	//ON_MESSAGE(UM_PickModelTypeEvent,OnMsgPickModelTypeEvent)

	

	ON_MESSAGE(UM_WEB_KEY_DOWN, OnWebkeyDown)
	ON_MESSAGE(UM_Right_ShowTip,OnShowRightTip)
	ON_MESSAGE(UM_User_Deal_Record,OnAddUserDealRecord)
	ON_MESSAGE(UM_HasIndexRight,OnIsUserHasIndexRight)
	// OnJumpToTradingSoftware
	ON_MESSAGE(UM_JumpToTradingSoftware,OnJumpToTradingSoftware)
	// ��Ѷ
	ON_MESSAGE(UM_ViewData_OnNewsResponse, OnMsgViewDataOnNewsResponse)
	ON_MESSAGE(UM_ViewData_OnNewsServerConnected, OnMsgViewDataNewsServerConnected)
	ON_MESSAGE(UM_ViewData_OnNewsServerDisConnected, OnMsgViewDataNewsServerDisConnected)

	ON_MESSAGE(UM_ViewData_OnAuthFail, OnMsgViewDataOnAuthFail)

	ON_WM_SETTINGCHANGE()
	ON_COMMAND(ID_NEW_WORKSPACE, OnNewWorkspace)
	ON_COMMAND(ID_NEW_CHILDFRAME, OnNewCfm)		// �½�ҳ���ļ�
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
	// ����
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

	//�ָ�ϵͳҳ��
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
	//�����˵�ͼƬ
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

	m_dwUserConnectCmdFlag = (DWORD)(~0);	// ���б�־��

	m_uiConnectNewsServerTimers = 0;

	m_pTradeContainer		= NULL;// lcq add
	m_pSimulateTrade		= NULL;

	if ( s_libHelper.m_pProg != NULL )	// ��ʼ��������Դ
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
	DEL(m_pDlgAdvertise); //������RealeseGdiPlus ֮ǰ���

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
	//�����˵�ͼƬ
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

	CIoViewSyncExcel::UnInitialExcelResource();	// �ͷ�û���ͷŵ�Excelͬ����Դ

	// ɾ�����������Դ
	CBlockConfig::DeleteInstance();
	//CSysBlockManager::DelInstance();
	//CUserBlockManager::DelInstance();

	if (NULL != m_pDlgbelong)
	{
		m_pDlgbelong = NULL;
	}

	if ( s_libHelper.m_pProg != NULL )	// �ͷŰ�����Դ
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

#ifdef _DEBUG  // debug���ͷ�doc��Դ����֪��Ϊʲôapp��exitinstance����ȥ
	{
		// 		CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
		// 		if ( NULL != pApp )
		// 		{
		// 			pApp->m_pDocManager->CloseAllDocuments(TRUE);		// ���doc��ϵ���ͷ�doc��Դ
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

	//�����˵�ͼƬ
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

	// ���οͲ��������˿���true, �����û���Ϊ��¼
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


	// ���໯.�ػ�MainFrame �ı���
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

	// ʹStatusBar ���½�û��SIZEGRIP,�Է�ֹ���ʱ�ܹ��϶�
	ModifyStyle(WS_THICKFRAME,0);  

	ASSERT( NULL == m_pMyControlBar );
	m_pMyControlBar = new CMyContainerBar;
	m_pMyControlBar->Create(_T("������"), WS_CHILD|CBRS_ALIGN_BOTTOM|WS_VISIBLE|CBRS_SIZE_DYNAMIC, this);

	// ��ʼ������Ԥ��
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
	// m_wndStatusBar.ModifyStyle(SBARS_SIZEGRIP,0); ״̬��ȥ�������־. û��Ч��?
	ModifyStyle(0,WS_THICKFRAME);  


	//���������� 2013-11-12 by cym 
	m_wndCommentSBar.Create(this);
	//m_wndCommentSBar.SetStatusTimer();

	if (pApp->m_bOffLine)
	{
		ShowControlBar(&(m_wndCommentSBar), FALSE, 0);
	}
	m_DefaultNewMenu.LoadToolBar(g_awToolBarIconIDs);

	m_pMyControlBar->EnableDocking(CBRS_ALIGN_BOTTOM);
	{
		// ��ȡ�ȽϺ��ʵĸ߶�
		CSize sizeDef(700, 330);
		CRect rcWorkArea(0,0,0,0);
		if ( SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0) )
		{
			sizeDef.cy	= 330;	// Ĭ����330���ظ�
			sizeDef.cx	= int((float)sizeDef.cy * 2.618f);
		}		
		m_pMyControlBar->SetSizeDefault(sizeDef);
	}
	EnableMyDocking(CBRS_ALIGN_ANY);
	DockControlBar(m_pMyControlBar);
	ShowControlBar(m_pMyControlBar, FALSE, TRUE);	// Ĭ�����ظ�����

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

	//// ���ò����״̬
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
	m_pTradeContainer = new CTradeContainerWnd(ETT_TRADE_FIRM); // ʵ�̽��� 
	m_pTradeContainer->Create(_T("trade"), WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN, CRect(0,0,0,0), this, 0x1258);
	AddDockSubWnd(m_pTradeContainer);

	m_pSimulateTrade = new CTradeContainerWnd(ETT_TRADE_SIMULATE);	// ģ�⽻��
	m_pSimulateTrade->Create(_T("STradeInfo"), WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN, CRect(0,0,0,0), this, 0x1259);
	AddDockSubWnd(m_pSimulateTrade);

	LoadPicMenu();
	SendMessage(WM_NCPAINT, 0, 0);

	//2013-12-10	
	//if (NULL == m_pDlgIm && pApp->m_pConfigInfo->m_bImVersion)	�Ѿ�ȡ������ʦ������
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

	// �����˵���
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
	m_strTitle = AfxGetApp()->m_pszAppName;		// �������

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

	// �����û�ѡ���־
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
		SetUserConnectCmdFlag(EUCCF_ConnectDataServer, 0);	// �Ƴ����ӱ�־
		pDoc->m_pAbsCenterManager->DisconnectQuoteServer();
	}
}

void CMainFrame::OnUpdate()
{
	/*
	if ( m_bHaveUpdated )
	{
	MessageBox(L"ϵͳ�Ѹ���.�´ν���ʱ��Ч.", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
	return;
	}
	*/
	// �����Զ����³���:
	TCHAR TStrDir[MAX_PATH];	
	::GetCurrentDirectory(MAX_PATH, TStrDir);

	STARTUPINFO StartInfo = {sizeof(StartInfo)};
	PROCESS_INFORMATION ProcessInfo;

	if( !::CreateProcess(L"AutoUpdate.exe", L"GGTong", NULL, NULL, false, 0, NULL, TStrDir, &StartInfo, &ProcessInfo) )
	{
		MessageBox(L"�����Զ����³���ʧ��!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
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

	// ֹͣ�״����
	CMarketRadarCalc::Instance().StopCalc();
	return;

	// ɾ���˵�������Ŀitem data
	CMenu* pMenu = this->GetMenu();
	if ( NULL != pMenu )
	{
		// �Ƴ������˵�
		const int32 iMenuCount = pMenu->GetMenuItemCount();
		for ( int32 i=0; i < iMenuCount ; i++ )
		{
			CString StrMenu;
			pMenu->GetMenuString(i, StrMenu, MF_BYPOSITION);
			if ( StrMenu == _T("����(&F)") )
			{
				pMenu->RemoveMenu(i, MF_BYPOSITION);	// ��menu������ɾ��������ֻҪremove
				break;
			}
		}
	}
	CNewMenu* pFrameMenu = DYNAMIC_DOWNCAST(CNewMenu, pMenu);
	if ( NULL != pFrameMenu )
	{
		// ���˵�
		pMenu = pFrameMenu->GetSubMenu(_T("���(&B)"));
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


		// 3: �����˵� - �����˵�ʹ�õ��ǰ���е�ָ��

		//CMenu * pSubMenuF = pFrameMenu->GetSubMenu(L"����(&F)");
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
		// 				// ���۱��������е�ɾ��
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

	// ��ʼ��xml
	TiXmlElement *pChildEle = NULL;
	TiXmlDocument myDocument;
	{
		// ʹ��Ĭ��
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

	//  �Ӵ������,��fromxml ���Ѿ������,���ﲻ��Ҫ�ڴ���
	if (m_bShowHistory && NULL == GetHistoryChild())
	{
		pChildFrame->CenterWindow();
	}
	else
	{
		// �򿪵��Ӵ��ھ����������
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

	// �����ڲ��� 
	// ��������.
	int32 iWindowCounts = 0;
	const char *pcAttrValue = pRootElement->Attribute(KStrWindowCounts);
	if (NULL != pcAttrValue)
		iWindowCounts = atoi(pcAttrValue);

	// ��������
	WINDOWPLACEMENT aWp[256];
	memset(aWp, 0, sizeof(aWp));

	aWp[0].length = sizeof(WINDOWPLACEMENT);

	pcAttrValue	= pRootElement->Attribute("flag");
	if (NULL != pcAttrValue)		aWp[0].flags = atoi(pcAttrValue);

	pcAttrValue	= pRootElement->Attribute("showCmd");
	if (NULL != pcAttrValue)		aWp[0].showCmd = atoi(pcAttrValue);

	// ��������ʱ��Ӧ��ȫ���������ٴδ򿪹������涼��Ӧ���޸ĵ�ǰ���ڵĴ�С
	// �޸Ĳ��ԣ���һ��ȫ���������򿪰��治�ڵ���SetWindowPlacement
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
	// XL0006 - ����ǰ��false��Ǽ���
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
	// ������󻯱�־,����XML ����,��Ӧ��ͬ�ֱ�����Ļ���������

	CRect rectMax;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rectMax,0);	

	// 	if ( 1 == iMaxFlag)
	// 	{
	// 		// ��������,��ô�����еĻ����϶�Ӧ�������
	// 		CRect rectNow;		
	// 		GetClientRect(&rectNow);
	// 		MoveWindow(&rectMax);		
	// 		m_rectLast = rectNow;
	// 		m_bMaxForSysSettingChange = true;		
	// 	}
	// 	else
	// 	{
	// 		// ����������,��������������ĳߴ���ڱ�������Ļ�ߴ�,������Ϊ���
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
		// ����������,��������������ĳߴ���ڱ�������Ļ�ߴ�,������Ϊ���
		if ( (aWp[0].rcNormalPosition.right - aWp[0].rcNormalPosition.left) > rectMax.Width() || (aWp[0].rcNormalPosition.bottom - aWp[0].rcNormalPosition.top) > rectMax.Height())
		{
			aWp[0].showCmd = SW_SHOWMAXIMIZED;
		}					
	}

	// �Ƿ���Ҫ�����أ�Ȼ��ȴ���ʼ����ɺ���ʾ��

	// ֻ��ϵͳ��һ�δ�ʱ����Ҫ���ã�������Ӧ���ٴ�Ӱ���û������ĳ����С
	if ( m_bFirstDisplayFromXml )
	{
		SetWindowPlacement(&aWp[0]);
		m_bFirstDisplayFromXml = false;
	}

	//////////////////////////////////////////////////////////////////////////
	// ���߲���С����
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

			// ����z ֵ�Ӵ�С��˳��,�����Ӵ���
			const char * StrChildZindex = pElementChildFrame->Attribute("zindex");
			int32     iChildZindex = atoi(StrChildZindex);

			if ( iChildZindex == iZindex )
			{
				CMPIChildFrame *pChildFrame = pApp->m_pDocTemplate->CreateMPIFrame(IDR_MAINFRAME);
				if (NULL == pChildFrame)
					return false;

				if (!pChildFrame->FromXml(pElementChildFrame))
					return false;

				// ����Ĭ�ϵļ�����ͼ,�����Tab ҳ,��ô��ʾTab ҳ�ļ�����ͼ.����,��Ĭ�ϵ�
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
		//m_pDlgWait->ShowWindow(SW_HIDE); // ����ᵼ��MainFrame�����showNormal,max֮���л��Ļ���һ��hide - -
		m_pDlgWait->MoveWindow(CRect(0,0,0,0));
		m_pDlgWait->PostMessage(WM_CLOSE,0,0);
		m_pDlgWait = NULL;
		ShowCursor(true);

		// ���������Ϣջ�п����кܶ���Ϣ��������ʾ���ң����û�ͼ��һ����˵
		UpdateWindow();
	}

	SetHotkeyTarget(NULL);
	m_bFromXml = false;

	if ( NULL != m_pActiveGGTViewFromXml )
	{
		// �����Ժ���ÿ�.

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
	// �����ļ�
	CMPIChildFrame* pChild = (CMPIChildFrame*)GetActiveFrame();
	if ( (CNewMDIFrameWnd*)pChild != (CNewMDIFrameWnd*)this )
	{
		CGGTongView* pView = (CGGTongView*)pChild->GetActiveView();
		if ( NULL != pView )
		{
			// �������һ�������GGTongView 
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

	// xml�������Ӵ���, �����б����ҳ�����ô������
	// ��id�Ĳ����浽������
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

	// xml �е���ʾ����Ĭ��Ϊ�ļ���(�û����Ϊ�ͳ���������,������̫����)
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

	// �����XML �İ汾��.��Ҫ��
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

	// �����������

	CString StrMainFrameAttribute;	
	CString StrName, StrZIndex, StrFlag, StrShowCmd;
	CString StrPtMinPosX, StrPtMinPosY, StrPtMaxPosX, StrPtMaxPosY;
	CString StrNomPosleft, StrNomPosright, StrNomPostop, StrNomPosbottom;


	WINDOWPLACEMENT aWp[256];
	GetWindowPlacement(&aWp[0]);

	aWp[0].length = sizeof(WINDOWPLACEMENT);

	StrFlag.Format(L"%d", aWp[0].flags);
	StrShowCmd.Format(L"%d", aWp[0].showCmd);
	//  �����Ƿ���������־	XL0006 ������ʵ��showCmd
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

	// �����ÿ�
	SetActiveGGTongViewXml(NULL);

	return StrThis;
}

CString CMainFrame::GetWspFileShowName(const char* KpcFilePath)
{	
	// �õ��������ļ���������������ʾ���ļ���		
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

	// ���ֽ�ת��Ϊ���ֽ�
	wchar_t awcFileName[1024];
	memset(awcFileName, 0, sizeof(awcFileName));

	MultiCharCoding2Unicode(EMCCUtf8, StrFileShowName, strlen(StrFileShowName), awcFileName, sizeof(awcFileName) / sizeof(wchar_t));
	CString StrReturn = awcFileName;

	return StrReturn;
}

CString CMainFrame::GetWspFileVersion(const char* KpcFilePath)
{
	// �õ��������ļ��������İ汾��
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

	// ���ֽ�ת��Ϊ���ֽ�
	wchar_t awcFileVersion[1024];
	memset(awcFileVersion, 0, sizeof(awcFileVersion));

	MultiCharCoding2Unicode(EMCCUtf8, StrFileVersion, strlen(StrFileVersion), awcFileVersion, sizeof(awcFileVersion) / sizeof(wchar_t));
	CString StrReturn = awcFileVersion;

	return StrReturn;
}

bool32 CMainFrame::IsWspReadOnly( const char *KpcFilePath )
{
	// �õ��������ļ��������İ汾��
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
	// �õ��������ļ��������İ汾��
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

	// �����������ڵĶ���
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
	// ����ռ�, �ÿռ��С������ԭ���ȵ�2��, ��ΪUnicode������ռ2�ֽ�, Utf8������ռ3-7�ֽ�, ƽ������, һ������ᳬ��2��
	int32 iOutBufferSize = StrContent.GetLength();
	iOutBufferSize *= 2;
	iOutBufferSize += 100;		// ��ֹStrContent����Ϊ0
	char *pcOutBuffer = new char[iOutBufferSize];
	if (NULL != pcOutBuffer)
	{
		// ��ת��д���ļ�������
		memset(pcOutBuffer, 0, iOutBufferSize);
		Unicode2MultiCharCoding(EMCCUtf8, StrContent, StrContent.GetLength(), pcOutBuffer, iOutBufferSize);

		// ת���ļ���
		wchar_t awcFileName[MAX_PATH];
		memset(awcFileName, 0, sizeof(awcFileName));
		MultiCharCoding2Unicode(EMCCSystem, KpcFileName, strlen(KpcFileName), awcFileName, sizeof(awcFileName) / sizeof(wchar_t));

		CFile File;
		if (File.Open(awcFileName, CFile::modeCreate | CFile::modeWrite))
		{
			File.Write(pcOutBuffer, strlen(pcOutBuffer));
			File.Close();
		}

		// ��Ҫ�����ͷ�
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
		m_dwHotKeyTypeFlag = EHKTCount;		// Ĭ��Ϊ����
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
	// MainFrame �з������̰����¼� 
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
				// xl 1102 ʹ��showdlg�滻
				ShowHotkeyDlg(pMsg, m_pKBParent, (E_HotKeyType)m_dwHotKeyTypeFlag);
				// 				CGGTongApp * pApp = (CGGTongApp *)AfxGetApp();
				// 				CGGTongDoc *pDoc = pApp->m_pDocument;
				// 				ASSERT(NULL != pDoc);
				// 
				// 				// ����С��������Ի���
				// 				if ( NULL == m_pKBParent || !IsWindow(m_pKBParent->GetSafeHwnd()))	// ����������!
				// 				{
				// 					m_pKBParent = this;
				// 					m_dwHotKeyTypeFlag = EHKTCount;
				// 					SetForceFixHotkeyTarget(false);
				// 				}
				// 
				// 				if ( m_dwHotKeyTypeFlag != EHKTCount )
				// 				{
				// 					// ��֧���ض�һ�����Ŀǰ��û��֧�ֶ�����
				// 					m_pDlgKeyBoard = new CKeyBoardDlg((E_HotKeyType)m_dwHotKeyTypeFlag, m_pKBParent);
				// 				}
				// 				else
				// 				{
				// 					m_pDlgKeyBoard = new CKeyBoardDlg(&pDoc->m_pAbsCenterManager->m_HotKeyList, m_pKBParent);
				// 				}
				// 				ASSERT(NULL != m_pDlgKeyBoard);
				// 				
				// 				// ���水����Ϣ,�������������顱�Ի�����
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
	// ����ǻ�ԭ����϶���F7��GGTongView
	bool32 bF7 = true;
	CMPIChildFrame *pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, pActiveGGTongView->GetParentFrame());
	if ( NULL != pParentFrame && pParentFrame->IsF7AutoLock() )
	{
		CGGTongView *pF7View = pParentFrame->GetF7GGTongView();
		ASSERT( NULL != pF7View );
		if ( NULL != pF7View )
		{
			pActiveGGTongView = pF7View;	// ��ȻҪ��ԭ�������յ�f7����
		}
		bF7 = false;	// ��ԭ����
	}

	// �����F7���������ڼ���ڱ������ˣ�������Ҫ�����趨�����
	// ���ｻ���ⲿ����������

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

				pActiveGGTongView = (CGGTongView*)pParent;	// ע�����ﲢ����ggtong view
			}

			pParent = pParent->GetParent();
		}
	}

	// ��ԭ��ʱ��, ������ͼ�Ĵ���
	if ( !bF7 && pActiveGGTongViewBK->m_IoViewManager.GetBeyondShowFlag() )
	{
		// Ҫ�ٴγŴ���ʾ
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
		// ��ݼ�����
		if (WM_SYSKEYDOWN == pMsg->message || WM_KEYDOWN == pMsg->message)
		{
			// �رճ���
			if ( VK_F4 == pMsg->wParam && IsAltPressed() )
			{
				CloseWorkSpace();
				return TRUE;					 
			}

			// �ر��Ӵ���			
			CMPIChildFrame * pChild = (CMPIChildFrame*)MDIGetActive();
			if ( (NULL != pChild) && (*pChild != *this) && pChild != CCfmManager::Instance().GetUserDefaultCfmFrame() )
			{			
				if ( VK_F4 == pMsg->wParam && IsCtrlPressed() )
				{
					OnCloseChildFrame();	// �رյ�ǰ����
					return TRUE;
				}			
			}

			if ( VK_F10 == pMsg->wParam )
			{
				// ���� F10				
				if ( DoF10() )
				{
					return TRUE;
				}
			}

			if ( VK_F11 == pMsg->wParam )
			{
				// ���� F10				
				if ( DoF11() )
				{
					return TRUE;
				}
			}

			// debug�°�F12�����DbgBreakIn
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
				// ĳ�������ESC��ת����������
				OnEscBackFrame();
				return TRUE;	// ���������Ϣ
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
	// ...fangz0411 �����л��ͼ
	CIoViewBase * pIoView = FindActiveIoView();

	if (pWnd && pIoView)
	{
		CDlgSystemSetting  DlgSystemSetting;
		DlgSystemSetting.m_DlgSystemFace.SetActiveIoView(pIoView);
		DlgSystemSetting.DoModal();	
	}
	else
	{
		MessageBox(L"��ǰû�п����õ���ͼ�򴰿�!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
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
	// 	���水����Ϣ,�������������顱�Ի�����		
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
	// �ҵ�ǰ�����GGTongView

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

	// ö�����е�ҵ����ͼ, �ҵ���ͬ����ģ� ȡ����Чֵ
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

	// û����ͬ����ģ��͸�����ָ֤��(������Ŀ����)
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
		// zhangbo 0515 #������
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

	// �����ҵ���ǰҵ����ͼ
	//	CIoViewBase *pIoView = NULL;

	CMerch *pMerch = NULL;
	if (!pAbsCenterManager->GetMerchManager().FindMerch(StrMerchCode, iMarketId, pMerch))
	{
		return;
	}

	// �������� ֻ�����Ǽ�����Ʒ֮���л���������Ʒ��ת����ʱ����
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

	// �����ҵ���ǰҵ����ͼ
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
	//linhc 20100917���Ĭ�Ͻ����һ����Ʒ
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
	// �˴�Ӧ����Ȩ�޿���
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
		if ( NULL == pIoViewKLine->GetMerchXml() )	// ���һ��Ĭ����Ʒ����
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

			// û����ƷҲ���ܸ���ָ��- -
		}
		if(0 >= pIoViewKLine->m_pRegionMain->m_RectView.Height())
		{
			m_hotKey = hotKey;
			SetTimer(KTimerIdIndexShow, KTimerPeriodIndexShow, NULL);
			return;
		}
		if ( NULL != pIoViewKLine->GetMerchXml() )
		{
			pIoViewKLine->AddShowIndex(hotKey.m_StrParam1, false, true);	// ����ʵ�����Ʒ
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
		// ��ǰҳ���ޱ��۱��������������Ĭ�ϱ��۱����
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
				return;	// ����������е��ˣ���������ǰ�Ĳ���
			}
		}
	}

	if (NULL == pIoViewReport)
	{
		pIoViewReport = FindIoViewReport(false);
	}

	if (NULL == pIoViewReport)
	{	
		// zhangbo 0702 #bug, ��Ҫ�����յı��۱� Ȼ�������һ��������Ϣ
		NewEmptyReportChildFrame();
		CIoViewBase *pActiveIoView = FindActiveIoView();
		ASSERT(pActiveIoView->IsKindOf(RUNTIME_CLASS(CIoViewReport)));

		pIoViewReport = (CIoViewReport *)pActiveIoView;
		pIoViewReport->OpenBlock(BlockDesc, (E_MerchReportField)iMerchReportField, bDescSort);
		return;
	}

	// �ҵ����IoViewReport �Ժ�,����Ϊ��ǰ�ɼ�
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

	// ��ʾ���
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
		// ��ǰҳ���ޱ��۱��������������Ĭ�ϱ��۱����
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
				return;	// ����������е��ˣ���������ǰ�Ĳ���
			}
		}
	}

	if (NULL == pIoViewReport)
	{
		pIoViewReport = FindIoViewReport(false);
	}

	if (NULL == pIoViewReport)
	{	
		// zhangbo 0702 #bug, ��Ҫ�����յı��۱� Ȼ�������һ��������Ϣ
		NewEmptyReportChildFrame();
		CIoViewBase *pActiveIoView = FindActiveIoView();
		ASSERT(pActiveIoView->IsKindOf(RUNTIME_CLASS(CIoViewReport)));

		pIoViewReport = (CIoViewReport *)pActiveIoView;
		pIoViewReport->OpenBlock(BlockDesc, (E_MerchReportField)iMerchReportField, bDescSort);
		return;
	}

	// �ҵ����IoViewReport �Ժ�,����Ϊ��ǰ�ɼ�
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

	// ��ʾ���
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
			// ͬ������ͼ

			// ����ÿ��������ͼ��Ӧ�ķ������
			T_IoViewTypeFaceObject IoViewTypeFaceObject;

			IoViewTypeFaceObject.m_pIoViewClass = pIoViewSrc->GetRuntimeClass();

			if ( bFont)
			{
				CFaceScheme::Instance()->GetIoViewTypeFaceObject(pIoViewSrc,IoViewTypeFaceObject);
				// ֻ�ı�����,��ɫ����
				CFaceScheme::Instance()->GetNowUseFonts(IoViewTypeFaceObject.m_aFonts);
			}
			else
			{
				CFaceScheme::Instance()->GetIoViewTypeFaceObject(pIoViewSrc,IoViewTypeFaceObject);			
				// ֻ�ı���ɫ,���岻��
				CFaceScheme::Instance()->GetNowUseColors(IoViewTypeFaceObject.m_aColors);
			}


			CFaceScheme::Instance()->UpdateIoViewTypeFaceObjectList(IoViewTypeFaceObject);

			// ����ͬ������ͼ,��Ӧ�����Ϣ

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
			// ͬ������ͼ
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
			// ͬ��ǩҳ��ͼ
			for (int32 i=0 ; i<m_IoViewsPtr.GetSize(); i++)
			{	
				CIoViewBase * pIoView = m_IoViewsPtr[i];
				if (NULL == pIoView)
				{
					continue;
				}

				// ��IoViewSrc �ĸ����Ƿ��� TabSplitWnd:
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
					// Դ��ͼ��û���������,ֱ�ӷ���
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
			// ��ͬ�Ӵ���

			// ��������Ӵ��ڵķ������:

			T_ChildFrameFaceObject ChildFrameObject;

			ChildFrameObject.m_pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, pIoViewSrc->GetParentFrame());
			if ( NULL == ChildFrameObject.m_pChildFrame )
			{
				return; // ��ֱ�ӷ���
			}
			CFaceScheme::Instance()->GetNowUseColors(ChildFrameObject.m_aColors);
			CFaceScheme::Instance()->GetNowUseFonts(ChildFrameObject.m_aFonts);

			CFaceScheme::Instance()->UpdateChildFrameFaceObjectList(ChildFrameObject);

			// �ҵ���Ӧ����ͼ,��Ӧ�����Ϣ

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
	// ���巢���仯
	CIoViewBase * pIoViewSrc = (CIoViewBase *)wParam;
	ASSERT(NULL != pIoViewSrc);

	OnIoViewFaceChange(true,pIoViewSrc);

	return 0;
}

LRESULT CMainFrame::OnMsgColorChange(WPARAM wParam, LPARAM lParam)
{
	// ��ɫ�����仯
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

	_MYTRACE(L"MainFrame�յ�ViewData ���͵ķ����� %d ���ӳɹ���Ϣ, ת��ViewData����:", (int32)wParam);
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

	// �����ǰ��Ϣջ�������һ������
	// 	MSG msg;
	// 	int32 iProc = 0;
	// 	while ( PeekMessage(&msg, m_hWnd, UM_ViewData_OnDataCommResponse, UM_ViewData_OnDataCommResponse, PM_REMOVE) )
	// 	{
	// 		++iProc;
	// 	}
	// 	if ( iProc > 0 )
	// 	{
	// 		UpdateWindow();
	// 		TRACE(_T("���������ȡ�ذ�����֪ͨ: %d�� \n"), iProc);
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

	// ������������Ի����ע����Ʒ����		
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

	// �����ǰ��Ϣջ�������һ������
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
				// �Ѿ��������˵���Ʒ���Ͳ��ظ���
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
	// ���ܴ����ȫ�������������
	for ( i=iNeedProc; i < aMsgs.GetSize() ; ++i )
	{
		PostMessage(aMsgs[i].message, aMsgs[i].wParam, aMsgs[i].lParam);
	}

	//if ( iNeedProc > 0 )
	//{
	//	TRACE(_T("�������RealtimePrice֪ͨ(%d/%d): %d�� ת�Ƶ���β: %d��\n"), aMsgs.GetSize(), iProc, iNeedProc, aMsgs.GetSize()-iNeedProc);
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
		MessageBox(StrPrompt, L"ǿ���˳�", MB_OK | MB_ICONINFORMATION);
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
//	//mapPickModelInfo.insert(std::make_pair(11002, T_SmartStockPickItem(11002, L"һ������", L"׷����ͣ�����ٳɶ࣬�������������", 0)));
//	//mapPickModelInfo.insert(std::make_pair(11003, T_SmartStockPickItem(11003, L"����ֱ��", L"������������׽�������������˵�Ʒ��", 0)));
//	//mapPickModelInfo.insert(std::make_pair(11004, T_SmartStockPickItem(11004, L"������ˮ", L"����ϴ�̽��������ǵ�����ʱ��", 0)));
//	//mapPickModelInfo.insert(std::make_pair(11005, T_SmartStockPickItem(11005, L"��������", L"���̴���һ��죬���������ʾ�����ռ�", 0)));
//	//result = find( m_vPickModelTypeGroup.begin( ), m_vPickModelTypeGroup.end( ), 100); //����100
//	//if (result != m_vPickModelTypeGroup.end())
//	//{
//	//	m_mapPickModelTypeGroup.insert(std::make_pair(100, mapPickModelInfo));
//	//}
//
//	//mapPickModelInfo.clear();
//	//mapPickModelInfo.insert(std::make_pair(11006, T_SmartStockPickItem(11006, L"һ������", L"׷����ͣ�����ٳɶ࣬�������������", 0)));
//	//mapPickModelInfo.insert(std::make_pair(11007, T_SmartStockPickItem(11007, L"����ֱ��", L"������������׽�������������˵�Ʒ��", 0)));
//	//mapPickModelInfo.insert(std::make_pair(11008, T_SmartStockPickItem(11008, L"������ˮ", L"����ϴ�̽��������ǵ�����ʱ��", 0)));
//	//mapPickModelInfo.insert(std::make_pair(11009, T_SmartStockPickItem(11009, L"��������", L"���̴���һ��죬���������ʾ�����ռ�", 0)));
//	//result = find( m_vPickModelTypeGroup.begin( ), m_vPickModelTypeGroup.end( ), 1002); //����100
//	//if (result != m_vPickModelTypeGroup.end())
//	//{
//	//	m_mapPickModelTypeGroup.insert(std::make_pair(1002, mapPickModelInfo));
//	//}
//		
//	// ���ж��Ƿ���Ҫ�ػ棬��������
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
//	// ����ѡ��״̬m_mapPickModelStatusGroup
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
// ���˵�
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

	// ����TechIndexd.dll����Ҫ������  zhangtao
	SetAbsCenterManager(pAbsCenterManager);
	//SetUserName(pAbsCenterManager->GetUserName());
	//SetServerTime(pAbsCenterManager->GetServerTime());

	//������ݼ�
	BuildHotKeyList();

	//pAbsCenterManager->SaveUserRightInfo();

	// ���ù�������Ϣ
	if ( m_bNeedSetWspAfterInitialMarket )
	{
		const CString StrFileNameStock  = L"system0";
		const CString StrFileNameFuture = L"system1";
		const CString StrFileNameAll	= L"system2";

		// ���û�,�Զ�ѡ��Ĭ�ϵĹ�����:

		// 1: �õ��û�����:
		CGGTongApp::E_UserRightType eUserRightType = pApp->GetUserRightType();
		CString StrFileName = StrFileNameAll;

		// 2: ѡ��Ĭ�ϵĹ�����
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
			// �����ʱ��,ѡ��һ��
			if ( pApp->m_aWspFileInfo.GetSize() > 0 )
			{
				pApp->m_StrCurWsp = pApp->m_aWspFileInfo.GetAt(0).m_StrFilePath;
			}
		}
	}

	//
	AdjustByOfflineMode();

	// ɾ���ϴεĴ����ļ�
	COptimizeServer::Instance()->ClearErrServer();

	// ������Զ�ģʽ, ���ұ��غ���֤���ص�һ��. ��¼�ɹ�����ping һ��, ���������ļ�
	if ( COptimizeServer::Instance()->BeLocalAndAuthServersIdentical() && COptimizeServer::Instance()->BeAutoSortMode() )
	{
		COptimizeServer::Instance()->SortQuoteServersByPing();
	}

	// �˵�
	T_BlockMenuInfo  BlockMenuInfoToAdd;
	int32 i, iSize = pAbsCenterManager->GetMerchManager().m_BreedListPtr.GetSize();		

	//
	if (iSize >0)
	{
		bool32 bShangHaiZhengQuan = (pAbsCenterManager->GetMerchManagerType() & EMMT_StockCn) != 0;

		// 		CMenu* pMenu = this->GetMenu();
		// 	
		// 		CNewMenu* pFrameMenu = DYNAMIC_DOWNCAST(CNewMenu, pMenu);
		// 		pMenu = pFrameMenu->GetSubMenu(_T("���(&B)"));
		// 		if(NULL == pMenu)
		// 			return 0;
		// 		CNewMenu* pRootMenu = DYNAMIC_DOWNCAST(CNewMenu, pMenu);	
		// 
		// 		{
		// 			int32 iCount = pRootMenu->GetMenuItemCount();
		// 			for ( int32 iDel=0; iDel < iCount ; iDel++  )
		// 			{
		// 				pRootMenu->RemoveMenu(0, MF_BYPOSITION);		// �����ǰ�Ĳ˵���Ŀ
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
			// 			if (StrBreedName == L"���֤ȯ" || StrBreedName == L"�¼����ڻ�" || StrBreedName == L"ȫ��ָ��")
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
				// delete ָ��.
				// 				T_BlockDesc*  pBlockDesc = new T_BlockDesc;
				// 				pBlockDesc->m_eType			= T_BlockDesc::EBTPhysical;
				// 				pBlockDesc->m_iMarketId		= pMarket->m_MarketInfo.m_iMarketId;
				// 				pBlockDesc->m_StrBlockName	= pMarket->m_MarketInfo.m_StrCnName;
				// 				pBlockDesc->m_pMarket		= pMarket;

				// 				SubPop->SetItemData(IDM_BLOCKMENU_BEGIN+iIncrement,(DWORD)(pBlockDesc));

				// ������Ϣ
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
		// 			pRootMenu->AppendMenu(MF_SEPARATOR);	// ���û��separator�����һ��
		// 		}

		// �����߼����
		// 		UINT uCmdBlock = IDM_BLOCKMENU_BEGIN4;
		// 		if ( bShangHaiZhengQuan && CConfigInfo::FlagCaiFuJinDunQiHuo != CConfigInfo::Instance()->GetVersionFlag() )
		// 		{
		// 			CBlockConfig::IdArray aColIds;
		// 			CBlockConfig::Instance()->GetCollectionIdArray(aColIds);
		//  		CNewMenu *pLogicMenu = NULL;
		// 			if ( aColIds.GetSize() > 0 )
		// 			{
		//  			pLogicMenu = pRootMenu->AppendODPopupMenu(_T("��Ʊ���"));
		// 				ASSERT( NULL != pLogicMenu );
		// 			}

		// 			for ( int i=0; i < aColIds.GetSize() && pLogicMenu != NULL ; i++ )
		// 			{
		// 				CBlockCollection *pCol = CBlockConfig::Instance()->GetBlockCollectionById(aColIds[i]);
		// 				CBlockConfig::BlockArray aBlocks;
		// 				pCol->GetValidBlocks(aBlocks);
		// 				if ( aBlocks.GetSize() > 0 
		// 					&& 
		// 					(aBlocks[0]->m_blockInfo.GetBlockType() == CBlockInfo::typeNormalBlock				// ��ͨ || ����
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

		// �ڻ���飬������ڵĻ�
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
		// 					&& aBlocks[0]->m_blockInfo.GetBlockType() == CBlockInfo::typeFutureClassBlock				// �ڻ� )
		// 					)
		// 				{
		// 					bFuture = true;
		// 					break;
		// 				}
		// 			}
		// 			if ( bFuture && CConfigInfo::FlagCaiFuJinDunGuPiao != CConfigInfo::Instance()->GetVersionFlag() )
		// 			{
		// 				pLogicMenu = pRootMenu->AppendODPopupMenu(_T("�ڻ����"));
		// 				ASSERT( NULL != pLogicMenu );
		// 				for ( int i=0; i < aColIds.GetSize() && pLogicMenu != NULL ; i++ )
		// 				{
		// 					CBlockCollection *pCol = CBlockConfig::Instance()->GetBlockCollectionById(aColIds[i]);
		// 					CBlockConfig::BlockArray aBlocks;
		// 					pCol->GetValidBlocks(aBlocks);
		// 					if ( aBlocks.GetSize() > 0 
		// 						&& aBlocks[0]->m_blockInfo.GetBlockType() == CBlockInfo::typeFutureClassBlock				// �ڻ� )
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
		// 		// �û����
		// 		AppendUserBlockMenu(pRootMenu);
	}

	// PIC �˵�����:
	//AppendPicMenu();

	//// ���������汾�Ĳ˵����� xl 100928
	//{
	//	// ����˵��еĲ˵�������
	//	// ���� ���ѡ��
	//	{
	//		// ����
	//		CMenu* pMenu = this->GetMenu();
	//		CNewMenu* pFrameMenu = DYNAMIC_DOWNCAST(CNewMenu, pMenu);
	//		pMenu = pFrameMenu->GetSubMenu(_T("����(&H)"));
	//		CNewMenu* pRootMenu = DYNAMIC_DOWNCAST(CNewMenu, pMenu);
	//		if ( NULL != pRootMenu )
	//		{
	//			CString StrAbout;
	//			StrAbout.Format(_T("���� %s(&A)..."), AfxGetApp()->m_pszAppName);
	//			pRootMenu->SetMenuText(ID_APP_ABOUT, StrAbout, MF_BYCOMMAND);
	//		}
	//	}
	//}

	// ������ɫ�˵�����
	InitSpecialMenu();

	// ���Ӵ��뱣��cfmѡ��

	// ��ʼ���г��״��������
	CMarketRadarCalc::Instance().InitAndStartIf();

	// ��ʼ������Ԥ��
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


	// ��ʼ������
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

	// �ڻ��汾Ĭ��������ɫ������
	if ( CConfigInfo::FlagCaiFuJinDunQiHuo == CConfigInfo::Instance()->GetVersionFlag()
		|| CConfigInfo::FlagCaiFuJinDunFree == CConfigInfo::Instance()->GetVersionFlag() )
	{
		OnViewSpecialBar();
	}
	//---wangyongxue Ŀǰ��ʱδʹ��
	//	//״̬������ 2013-7-22
	// 	{
	// 		//�������ű���
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
	// 		if ( !bViewExist ) //��ͼ�����ڣ���������
	// 		{			
	// 			CMmmiNewsReqAddPush NewsReqAddPushJinPing;
	// 			NewsReqAddPushJinPing.m_iIndexID = pAbsCenterManager->GetJinpingID();
	// 
	// 			//
	// 			pAbsCenterManager->RequestNews(&NewsReqAddPushJinPing);
	// 		}
	// 	}

	// �Զ�����timer
	SetTimer(KTimerIdAutoUpdateCheck, KTimerPeriodAutoUpdateCheck, NULL);

	// ����
	// 	SetTimer(KTimerIdReqPushJinping, KTimerPeriodReqPushJinping, NULL);

	// 1s����һ�δ���״̬
	SetTimer(KTimerIdDapanState, KTimerPeriodDapanState, NULL);

	// 1min���token�Ƿ���Ч
	SetTimer(KTimerCheckToken, KTimerPeriodCheckToken, NULL);
	// ���⽻��ʱ������
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
		case en_heartbeat:				// ����
			{
				//printf("�յ�������.\n");
				handleHeartbeat(vData);
			}
			break;
		case en_set_device_id:			// �����豸��
			{
			}
			break;
		case en_query_info_type_list:	// ��ѯ��Ѷ�����б�
			{
			}
			break;
		case en_query_msg_list:			// ��ѯ��Ϣ�б�
			{
				//
			}
			break;
		case en_query_info_content:		// ��ѯ��Ѷ����
			{
				handleInfoContentQuery(vData);
			}
			break;
		case en_info_push:				// ������Ѷ
			{
				handlePushInfo(vData);
			}
			break;
		case en_add_info:				// �����Ѷ
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
		// ��Ϣ���ķ��������ӳ���
	}
	else if (en_connect_success == eConnNotify)// ��Ϣ���ķ��������ӳɹ�
	{
	}
}

// ��Ѷ
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
		// ��ǰҳ���ޱ��۱��������������Ĭ�ϱ��۱����
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
				return pIoViewReport;	// ����������е��ˣ���������ǰ�Ĳ���
			}
		}
	}

	// ��Ȼ�����п��ܴ������۱��������ڿհ״����£��ǿ��������
	//if ( NULL != GetEmptyGGTongView())
	//{
	//	// �пհ״���:
	//	CIoViewReport * pReport = (CIoViewReport *)CreateIoViewByPicMenuID(ID_PIC_REPORT, true);
	//	
	//	if (NULL != pReport)
	//	{
	//		pReport->OpenBlock(BlockDesc);
	//		CMPIChildFrame * pChildFrameToShow = DYNAMIC_DOWNCAST(CMPIChildFrame, pReport->GetParentFrame());
	//		// ������frame��С���ˣ���ԭ 0001770 ���
	//		if ( pChildFrameToShow->IsIconic() )
	//		{
	//			pChildFrameToShow->ShowWindow(SW_SHOWNORMAL);
	//		}
	//		pChildFrameToShow->BringWindowToTop();
	//		//linhc 20100917���������ʾ�����ͼΪ������ͼ��Ĭ�ϻ�ɫ��ѡ�е�һ����Ʒ
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
		// ����������ı��۱�Ͳ�Ҫ�滻��
		if (!((CIoViewReport*)pIoView)->m_bLockReport)
		{
			pIoViewReport = (CIoViewReport*)pIoView;
		}
	}

	if ( NULL == pIoViewReport )
	{
		if ( NULL != pIoView && pIoView->GetIoViewGroupId() > 0 ) // ���ڼ���ҵ����ͼʱ������ͬ�����ͬframe��report - ����report����
		{
			pIoViewReport = (CIoViewReport *)FindIoViewByPicMenuId(ID_PIC_REPORT, pIoView, true, false, true, true, true); // ������ͼ��ô������
		}
		else
		{
			// û�м���ڻ��߶�����ͼ��������ǰ�Ĵ����
			pIoViewReport = FindIoViewReport(true);	 // ���Ѱ�ҵ����п��ܲ���ͬһ�������report��ϣ���õ������뵱ǰ����ioviewͬ�����report
		}

		//		if (NULL == pIoViewReport)
		//			pIoViewReport = FindIoViewReport(false);

		if (NULL == pIoViewReport)
		{						
			CIoViewReport * pReport = (CIoViewReport *)CreateIoViewByPicMenuID(ID_PIC_REPORT, true);

			if (NULL != pReport)
			{
				pReport->OpenBlock(BlockDesc);
				// ������frame��С���ˣ���ԭ
				CMPIChildFrame * pChildFrameToShow = DYNAMIC_DOWNCAST(CMPIChildFrame, pReport->GetParentFrame());
				if ( pChildFrameToShow->IsIconic() )
				{
					pChildFrameToShow->ShowWindow(SW_SHOWNORMAL);
				}
				pChildFrameToShow->BringWindowToTop();
				//linhc 20100917���������ʾ�����ͼΪ������ͼ��Ĭ�ϻ�ɫ��ѡ�е�һ����Ʒ
				pReport->BringToTop(BlockDesc.m_StrBlockName);
			}			
			return pReport;
		}
	}

	// �ҵ����IoViewReport �Ժ�,����Ϊ��ǰ�ɼ�

	CMPIChildFrame * pChildFrameToShow = DYNAMIC_DOWNCAST(CMPIChildFrame, pIoViewReport->GetParentFrame());
	// ������frame��С���ˣ���ԭ
	if ( pChildFrameToShow->IsIconic() )
	{
		pChildFrameToShow->ShowWindow(SW_SHOWNORMAL);
	}
	pChildFrameToShow->BringWindowToTop();

	// fangz1028#������childframe �� TabSplitWnd ,�õ����IoViewReport ����Tab ҳ.����Ϊ��ǰ

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

	// xl 0607 �ҵ�������GGTongView�����F7�ˣ����뵱ǰactiveIoViewͬframe/tabWnd��������Ȼ�ԭF7
	CIoViewBase	*pIoViewActive = FindActiveIoView();
	if ( NULL != pIoViewActive 
		&& pIoViewActive != pIoViewReport
		&& pIoViewActive->GetParentFrame() == pIoViewReport->GetParentFrame()					   // ��ͬһ��Frame��
		&& pIoViewActive->GetParentGGtongView() != pIoViewReport->GetParentGGtongView()			   // ������ͬһ��GGTongView
		&& pIoViewActive->GetParentGGtongView()->GetMaxF7Flag()									   // F7�����
		)
	{
		bool32 bNeedF7Restore = true;
		if ( pTabSplitWnd )
		{
			if ( pTabSplitWnd->GetIoViewPage(pIoViewActive) == pTabSplitWnd->GetIoViewPage(pIoViewReport) )
			{
				// ͬһ��tabҳ�£���Ҫ��ԭ
			}
			else
			{
				bNeedF7Restore = false;  // ��ͬҳ�棬�����
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
	//linhc 20100917���������ʾ�����ͼΪ������ͼ��Ĭ�ϻ�ɫ��ѡ�е�һ����Ʒ
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

	// �ǲ��������г�
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

	// �ǲ����߼����
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
// �����˵�, ��֤resource.h �ж����ID �� IoViewManager �����˳��һ��

void CMainFrame::LoadPicMenu()
{
	CMenu * pMenu = this->GetMenu();	
	CNewMenu * pNewMenu = DYNAMIC_DOWNCAST(CNewMenu,pMenu);
	pNewMenu->LoadToolBar(g_awToolBarIconIDs);

	//CMenu * pSubMenu = pNewMenu->GetSubMenu(L"����(&F)");
	CMenu * pSubMenu = GetAnalysisMenu(false);	// �������˵�
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

		// �Ȳ��� "���۱�" ����˵�: �Ȱ����Ϣ��ȡ��ɺ��ټ��������ı��۱�:
		const T_IoViewObject *pIoViewObj = CIoViewManager::GetIoViewObject(i);

		if ( pIoViewObj->m_uID == ID_PIC_PHASESORT )
		{
			continue;		// �׶����в��뵽���۱����
		}

		if ( pIoViewObj->m_pIoViewClass != RUNTIME_CLASS(CIoViewReport))
		{
			// if ( CIoViewManager::GetIoViewObject(i)->m_uID < ID_PIC_HIDE_INMENU )		// ֻ��С�����id�����ڲ˵�����ʾ - Main��Ҫ����
			// ���ߵ�ǰ��ʼ������Ʒ�б����ѡ����ʵĳ��� xl 1020
			if ( (dwMerchManagerType & pIoViewObj->m_dwAttendMerchType) != 0 )
			{
				//pRootMenu->AppendODMenu(MenuName,MF_STRING,ID_PIC_BEGIN + i);
				pRootMenu->AppendODMenu(MenuName,MF_STRING, pIoViewObj->m_uID);
			}
		}		
	}

	//pRootMenu->RemoveMenu(0, MF_BYPOSITION|MF_SEPARATOR);


	// �����˵�
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
	// �����ۺ����� ����/������Ŀ�л� ���Ű����� ��������
	pRootMenu->AppendMenu(MF_STRING, ID_ZongHePaiMing, _T("�ۺ�����"));
	//pRootMenu->AppendMenu(MF_STRING, ID_HangQingCaiWu, _T("����/������Ŀ�л�"));
	//pRootMenu->AppendMenu(MF_STRING, ID_ReMenBanKuai,  _T("���Ű�����"));
	//pRootMenu->AppendMenu(MF_STRING, ID_DaPanZouShi,   _T("��������"));

	pRootMenu->AppendMenu(MF_SEPARATOR);
	pRootMenu->AppendMenu(MF_STRING, ID_PIC_CFM_MULTIINTERVAL, _T("������ͬ��"));
	pRootMenu->AppendMenu(MF_STRING, ID_PIC_CFM_MULTIMERCH,	   _T("���Լͬ��")); //2013-8-8 modify cym

	ChangeAnalysisMenu(false);	// Ĭ������������˵�
}

void CMainFrame::AppendPicMenu()
{
	// ���ϱ��۱�	
	//	CMenu * pMenu = this->GetMenu();
	//	CNewMenu *pNewMenu = DYNAMIC_DOWNCAST(CNewMenu,pMenu);
	//CMenu * pSubMenu = pNewMenu->GetSubMenu(L"����(&F)");
	CMenu * pSubMenu = GetAnalysisMenu(false);
	ASSERT( NULL != pSubMenu );
	CNewMenu * pRootMenu = DYNAMIC_DOWNCAST(CNewMenu,pSubMenu);
	ASSERT( NULL != pRootMenu );

	CString StrMenuName = CIoViewManager::FindIoViewObjectByRuntimeClass(RUNTIME_CLASS(CIoViewReport))->m_StrLongName;			
	pRootMenu->LoadToolBar(g_awToolBarIconIDs);

	//
	CNewMenu * pMenuReport = pRootMenu->AppendODPopupMenu(StrMenuName);
	pRootMenu->ModifyODMenu(StrMenuName, StrMenuName, IDB_TOOLBAR_REPORT);

	// ���۱�������Ӳ˵�
	if ( NULL != pMenuReport )
	{	
		// ��� Ĭ�ϱ��۱����� ��Ʊ���� �ڻ�����...
		// ����˵�������Ǵ��ڷ�������
		const SimpleTabClassMap &simMap = CIoViewReport::GetSimpleTabClassConfig();
		for ( SimpleTabClassMap::const_iterator it=simMap.begin(); it != simMap.end() ; ++it )
		{
			CString StrItem = it->second.StrName;
			if ( StrItem.IsEmpty() )
			{
				if ( it->second.iId == SIMPLETABINFOCLASS_DEFAULTID )
				{
					StrItem = _T("Ĭ�ϱ��۱�����");
				}
				else
				{
					StrItem.Format(_T("���۱�����%d"), it->second.iId);
				}
			}
			UINT nId = it->second.iId+ID_REPORT_TYPEBEGIN;
			ASSERT( nId >= ID_REPORT_TYPEBEGIN && nId <= ID_REPORT_TYPEEND );
			pMenuReport->AppendODMenu(StrItem, MF_STRING|MF_BYCOMMAND, nId);
		}
	}

	// ���۱����������һ�� �׶����в˵�
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
	// �½�һ���Ӵ���, �ڰ���Ĭ�ϵ�ioview
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

	// ���ҵ���Ψһ��ҵ����ͼ�� ������ҵ�������
	CGGTongView *pGGTongViewDefault = FindGGTongView(pChildFrame, NULL);
	ASSERT(NULL != pGGTongViewDefault);

	pGGTongViewDefault->m_IoViewManager.SetGroupID(iIoViewGroupId);
}

// 
CIoViewBase* CMainFrame::FindNotReportSameGroupIdVisibleIoview(CIoViewBase *pActiveIoView, bool32 bOnlyInSameChildFrame)
{	

	// ����ָ����ҵ����ͼ���ĸ�childframe
	CFrameWnd *pReportParentFrame = NULL;
	if (NULL != pActiveIoView)
	{
		if (pActiveIoView->GetIoViewGroupId() <= 0)		// ������ͼ�� û�й�����ͼ
			return NULL;

		pReportParentFrame = pActiveIoView->GetParentFrame();
	}
	else
	{
		if (bOnlyInSameChildFrame)	// ������������ͬһ��������
			return NULL;
	}

	// �������Ҹ���ҵ����ͼ�� �Ƿ��������
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

		// �����ҵ��˺��ʵ��� 
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

	const T_IoViewObject *pObjToFind = CIoViewManager::FindIoViewObjectByPicMenuID(nPicID);		// �����������ң���ʹ��id���Բ���
	if ( NULL == pObjToFind || NULL == pObjToFind->m_pIoViewClass )
	{
		return NULL;
	}

	// �������
	// ����ָ����ҵ����ͼ���ĸ�childframe
	CFrameWnd *pReportParentFrame = NULL;
	if ( IsWindow(pGroupIoView->GetSafeHwnd()) )
	{
		if ( pGroupIoView->GetIoViewGroupId() <= 0 && bInSameGroup )		// ������ͼ�� û�й�����ͼ
		{
			// ����������Ҫ���ҵģ������Լ�
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
		if ( bOnlyInSameChildFrame || bInSameGroup || bNotInSameManager )	// ����Ҫ��ָ����ЧpGroupIoView
		{
			ASSERT( 0 );
			return NULL;
		}
	}

	// ��ǰ������û���뵽�ģ�������Ҫ���� TODO
	// ����˳�� 
	//			-> ������ͼ����	0 -> ͬmanager��ҳ�� 1
	//			-> ����ҳ�� 2-> ͬmanager��ҳ�� 3 (������ǲ�����ͼ,������ͬmanager���������ͼͬchildframe, ����ȡ������)
	//			-> ͬ��ͬchildframe����ͼ �ɼ� ���ɼ�4
	//			-> ͬchildframe�¿ɼ���ҳ�� 5 -> ͬchildframe�²��ɼ�ҳ�� 6
	//			-> ��ͬchildfr�¿ɼ�ҳ�� 7 -> ��ͬchildframe�²��ɼ�ҳ�� 8
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

	// �����ҹ�, ���ظ���-- 
	IoViewArray	aIoViewsTmp;

	// ���л�
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
		// ͬ��ͬframe
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
		// ͬframe
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
		// ʣ�µ�
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

	// �������Ҹ���ҵ����ͼ�� �Ƿ��������
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
				// ʹ�������жϣ������Ҫ���ҵ����࣬Ҳ����������
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

		// �����ҵ��˺��ʵ��� 
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

//	�򻯲�ѯ�㷨���ɵĵ��ƺ�̫�����ˡ� add by weng.cx
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

	// �������
	// ����ָ����ҵ����ͼ���ĸ�childframe
	if ( NULL == pChildFrame )
	{
		pChildFrame = MDIGetActive();
		if ( NULL == pChildFrame )
		{
			return FALSE;	// �޴򿪵�ҳ��
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

		//	�Ƿ���Ҫ��ʾ
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

	// �������
	// ����ָ����ҵ����ͼ���ĸ�childframe
	if ( NULL == pChildFrame )
	{
		pChildFrame = MDIGetActive();
		if ( NULL == pChildFrame )
		{
			return NULL;	// �޴򿪵�ҳ��
		}
	}

	CIoViewBase *pIoViewActive =  NULL;
	CGGTongView *pActiveView = DYNAMIC_DOWNCAST(CGGTongView, pChildFrame->GetActiveView());
	if ( NULL != pActiveView )
	{
		pIoViewActive = pActiveView->m_IoViewManager.GetActiveIoView();
	}


	// ����˳�� 
	//			-> ����ҳ��	0 -> �κο���ҳ�� 1
	//			-> ����ҳ��manager�� 2-> ����ͬframeҳ��
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
			// ͬ�����µ�
			mapIoViewsSrc[ pIoView ] = 1;
		}
	}

	const T_IoViewObject *pObjToFind = CIoViewManager::FindIoViewObjectByPicMenuID(nPicID);
	if ( NULL == pObjToFind )
	{
		ASSERT( 0 );
		return NULL;	// û�����pic
	}

	// �����ҹ�, ���ظ���-- 
	IoViewArray	aIoViewsTmp;

	// ���л�
	if ( NULL != pIoViewActive )
	{
		// �����
		aIoViewsTmp.push_back(pIoViewActive);
		mapIoViewsSrc.erase(pIoViewActive);
	}

	// ͬ���
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
		// ͬframe
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

	ASSERT( mapIoViewsSrc.empty() );	// Ӧ����ʣ����

	// �������Ҹ���ҵ����ͼ�� �Ƿ��������
	for ( IoViewArray::iterator it = aIoViewsTmp.begin(); it != aIoViewsTmp.end(); it++)
	{
		CIoViewBase *pIoView = *it;

		if ( bMustVisible )
		{
			if ( !pIoView->IsWindowVisible() )
			{
				continue;	// ���ɼ��ĺ���, ��ʵ�����˳��ˣ���Ϊ����Ķ��ǲ��ɼ���
			}
		}

		// pic id
		const T_IoViewObject *pIoObj = CIoViewManager::FindIoViewObjectByIoViewPtr(pIoView);
		if ( pIoObj != NULL  && pIoObj->m_pIoViewClass != NULL )
		{
			if ( pIoObj->m_pIoViewClass == pObjToFind->m_pIoViewClass
				||(bEnableInherit  && pIoObj->m_pIoViewClass->IsDerivedFrom(pObjToFind->m_pIoViewClass)) )
			{
				return pIoView;	// ��Ȼ�����������
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

	// ����ָ����ҵ����ͼ���ĸ�childframe
	if ( NULL == pChildFrame )
	{
		pChildFrame = MDIGetActive();
		if ( NULL == pChildFrame )
		{
			return NULL;	// �޴򿪵�ҳ��
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


	// ����˳�� 
	//			-> ����ҳ��	0 -> �κο���ҳ�� 1
	//			-> ����ҳ��manager��
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
			// ͬ�����µ�
			mapIoViewsSrc[ pIoView ] = 1;
		}
	}

	const T_IoViewObject *pObjToFind = CIoViewManager::FindIoViewObjectByPicMenuID(nPicID);
	if ( NULL == pObjToFind )
	{
		return NULL;	// û�����pic
	}

	// �����ҹ�, ���ظ���-- 
	IoViewArray	aIoViewsTmp;

	// ���л�
	if ( NULL != pIoViewActive )
	{
		// �����
		aIoViewsTmp.push_back(pIoViewActive);
		mapIoViewsSrc.erase(pIoViewActive);
	}

	// ͬ���
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

	//ASSERT( mapIoViewsSrc.empty() );	// Ӧ����ʣ����

	// �������Ҹ���ҵ����ͼ�� �Ƿ��������
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
				return pIoView;	// ��Ȼ�����������
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

	CIoViewReport* pFirstFind = NULL;				// �ҵ��ĵ�һ�����������ı�����ͼ.
	//	CIoViewReport* pFindInTabSplitWnd = NULL;		// �ڱ�ǩҳ���ҵ��ı�����ͼ.

	// ��ǰ�Ļ ChildFrame
	CMPIChildFrame * pChildFrame =(CMPIChildFrame *)GetActiveFrame();
	if ( NULL == pChildFrame)
	{
		// �����ǰû���Ӵ���
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

		// �����ı��۱�����
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

		// ���ChildFrame �� TabSplitWnd ,��Ҫ�����ж�һ��

		CWnd* pWnd = pChildFrame->GetWindow(GW_CHILD);

		if ( pWnd->IsKindOf(RUNTIME_CLASS(CTabSplitWnd)))
		{
			CTabSplitWnd* pTabSplitWnd = (CTabSplitWnd*)pWnd;

			int32 iCurPage = pTabSplitWnd->m_nCurPage + 1;

			if(iCurPage ==  pTabSplitWnd->GetIoViewPage(pIoView))
			{
				// ���ص�ǰTab ҳ�ı��۱�.
				return (CIoViewReport*)pIoView;
			}
			else
			{
				// ������
				// ����һ��TabҲ�Ƿ��Ƿ��飿
				continue;
			}
		}

		// ������ͬһ��tabҳ���д����������Ƚ���tab���жϣ��������tab�У��������Ƿ���������أ�0001770
		// �������ò�Ʋ���Ӱ�쵽 pAbsCenterManager �е�ָ�룿
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

	CIoViewNews* pFirstFind = NULL;				// �ҵ��ĵ�һ�����������ı�����ͼ.
	//	CIoViewNews* pFindInTabSplitWnd = NULL;		// �ڱ�ǩҳ���ҵ��ı�����ͼ.

	// ��ǰ�Ļ ChildFrame
	CMPIChildFrame * pChildFrame =(CMPIChildFrame *)GetActiveFrame();
	if ( NULL == pChildFrame)
	{
		// �����ǰû���Ӵ���
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

		// ���ChildFrame �� TabSplitWnd ,��Ҫ�����ж�һ��

		CWnd* pWnd = pChildFrame->GetWindow(GW_CHILD);

		if ( pWnd->IsKindOf(RUNTIME_CLASS(CTabSplitWnd)))
		{
			CTabSplitWnd* pTabSplitWnd = (CTabSplitWnd*)pWnd;

			int32 iCurPage = pTabSplitWnd->m_nCurPage + 1;

			if(iCurPage ==  pTabSplitWnd->GetIoViewPage(pIoView))
			{
				// ���ص�ǰTab ҳ�ı��۱�.
				return (CIoViewNews*)pIoView;
			}
			else
			{
				// ������
				// ����һ��TabҲ�Ƿ��Ƿ��飿
				continue;
			}
		}

		// ������ͬһ��tabҳ���д����������Ƚ���tab���жϣ��������tab�У��������Ƿ���������أ�0001770
		// �������ò�Ʋ���Ӱ�쵽 pAbsCenterManager �е�ָ�룿
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

	CIoViewDuoGuTongLie* pFirstFind = NULL;				// �ҵ��ĵ�һ��������������ͼ.
	CIoViewDuoGuTongLie* pFindInTabSplitWnd = NULL;		// �ڱ�ǩҳ���ҵ�����ͼ.

	// ��ǰ�Ļ ChildFrame
	CMPIChildFrame * pChildFrame =(CMPIChildFrame *)GetActiveFrame();
	if ( NULL == pChildFrame)
	{
		// �����ǰû���Ӵ���
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

		// ���ChildFrame �� TabSplitWnd ,��Ҫ�����ж�һ��

		CWnd* pWnd = pChildFrame->GetWindow(GW_CHILD);

		if ( pWnd->IsKindOf(RUNTIME_CLASS(CTabSplitWnd)))
		{
			CTabSplitWnd* pTabSplitWnd = (CTabSplitWnd*)pWnd;

			int32 iCurPage = pTabSplitWnd->m_nCurPage + 1;

			if(iCurPage ==  pTabSplitWnd->GetIoViewPage(pIoView))
			{
				// ���ص�ǰTab ҳ�ı��۱�.
				return (CIoViewDuoGuTongLie*)pIoView;
			}
			else
			{
				// ������
				// ����һ��TabҲ�Ƿ��Ƿ��飿
				continue;
			}
		}

		// ������ͬһ��tabҳ���д����������Ƚ���tab���жϣ��������tab�У��������Ƿ���������أ�0001770
		// �������ò�Ʋ���Ӱ�쵽 pAbsCenterManager �е�ָ�룿
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
	if ( NULL == MDIGetActive() || pChild == pDefault )		// Ĭ��ҳ�治�ṩ�رհ�ť
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
			return;	// Ĭ��ҳ�治��ͨ���˷����ر�
		}
		if ( !pActiveChild->GetIdString().IsEmpty() )
		{
			CCfmManager::Instance().SaveCfm(pActiveChild->GetIdString(), pActiveChild);	// �Զ�����ҳ��
		}
		pActiveChild->PostMessage(WM_CLOSE, 0, 0);
		OnBackToUpCfm();
	}
	else if ( this == (CWnd*)pActiveChild )
	{
		MessageBox(L"��ǰ�޿ɹرմ���", AfxGetApp()->m_pszAppName, MB_ICONWARNING);		
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

			// ��ʱ��,��ʾһ��
			MessageBox(L"���������������ṩ����, ��������, �Ժ�����...", AfxGetApp()->m_pszAppName);
		}
		else
		{
			// �ж��Ƿ���֤�ɹ���:
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

				// �õ���������Ϣ
				aServers[i]->GetServerState(ServerState);	
				aServers[i]->GetServerSummary(StrAddr, uiPort, ProxyInfo);

				if ( (ServerState.m_bConnected) && (EASAuthSuccessed == ServerState.m_eAuthState) )
				{
					KillTimer(KTimerIdCheckConnectServer);
					m_wndStatusBar.HideTips();
					m_uiConnectServerTimers = 0;

					// �����˷�����:
					MessageBox(L"���ӳɹ�!", AfxGetApp()->m_pszAppName);
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
		// Ping �Ķ�ʱ��
		COptimizeServer::Instance()->OnMyTimer(nIDEvent);
	}
	else if ( KTimerIdCheckNewsConnectServer == nIDEvent )
	{
		// �����Ѷ�������Ƿ������ϵ�
		m_uiConnectNewsServerTimers++;

		// ��Ѷ�������Ƿ���������
		CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
		bool32  bConnected = false;
		if ( NULL != pDoc  && pDoc->m_pNewsManager)
		{
			// ��ѯ������:
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
					// ֻҪ��������һ������
					break;
				}
			}
		}

		if ( bConnected )
		{
			m_uiConnectNewsServerTimers = 0;
			KillTimer(nIDEvent);

			AfxMessageBox(_T("������Ѷ�������ɹ�!"));
		}
		else if( m_uiConnectNewsServerTimers++ >= KuiTimesToWaitForConnect  )
		{
			m_uiConnectNewsServerTimers = 0;
			KillTimer(nIDEvent);

			AfxMessageBox(_T("������Ѷ��������ʱ��"));
		}
	}
	else if ( KTimerIdAutoUpdateCheck == nIDEvent )
	{
		KillTimer(nIDEvent);

		// �Ƿ�ָ����update
		StringArray aCmd;
		const TCHAR *ptszCmdLine = ::GetCommandLine();
		GetMyCommandLine(ptszCmdLine, aCmd);
		MyStringType strCmdUpdateHeader(_T("Update="));
		for (unsigned int iCmd=0; iCmd<aCmd.size(); ++iCmd )
		{
			const MyStringType &str = aCmd[iCmd];
			if ( !str.empty() && _tcsnicmp(str.c_str(), strCmdUpdateHeader.c_str(), strCmdUpdateHeader.length()) == 0 )
			{
				// �����Զ����³���:
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
					// ����Ҫ��ʾ
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
	else if (KTimerIdDapanState == nIDEvent)		//--- wangyongxue 2016/12/06  ����״̬Ŀǰ�ȷ������ﴦ���
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
	else if (KTimerIdChooseStockState == nIDEvent)		//--- wangyongxue 2017/03/01  ѡ��״̬
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
			if ( NULL == pIoViewKLine->GetMerchXml() )	// ���һ��Ĭ����Ʒ����
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

				// û����ƷҲ���ܸ���ָ��- -
			}
			if ( NULL != pIoViewKLine->GetMerchXml() )
			{
				if(0 < pIoViewKLine->m_pRegionMain->m_RectView.Height())
				{
					KillTimer(KTimerIdIndexShow);
					pIoViewKLine->AddShowIndex(m_hotKey.m_StrParam1, false, true);	// ����ʵ�����Ʒ
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
		// ˢ��token
		CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
		if (!pDoc->m_pAutherManager)
		{
			return;
		}
		bool32 bRet = pDoc->m_pAutherManager->GetInterface()->RefreshToken();

		// ʧ�ܣ�����һ��
		if (!bRet)
		{
			pDoc->m_pAutherManager->GetInterface()->RefreshToken();
		}
	}
	else if (KTimerIdUserDealRecord == nIDEvent)
	{
		// �ϴ��û���Ϊ��¼
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
/// �˵������
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
			bool32 bUserPseudo;									// ����ɨ��һ�Σ��Ը���wsp���ļ�·����ʾ��ʽ
			pApp->ScanAvaliableWspFile(pDoc->m_pAbsCenterManager->GetUserName(), bUserPseudo);

			bNeedInitTBWnd = true;
		}

	}

	// �½�������
	CDlgWspName dlgWspName;
	AfxInitRichEdit();

	dlgWspName.SetNewFileType(CDlgWorkSpaceSet::EFTWorkSpace);

	if ( IDOK == dlgWspName.DoModal() && NULL != pApp && NULL != pDoc)  // �п���������Ի�����������ɾ��- -
	{
		// �رյ�ǰ��
		ClearCurrentWorkSpace();

		//
		const T_WspFileInfo* pWspFile = pApp->GetWspFileInfo(pApp->m_StrCurWsp);
		CString StrFileVersion = L"";

		if ( NULL != pWspFile )
		{
			StrFileVersion = pWspFile->GetNormalWspFileVersion();
		}

		// ����һ�µ�ǰ��
		pApp->m_StrCurWsp = GetWorkSpaceDir() + dlgWspName.m_StrName + L".wsp";	
		if ( !dlgWspName.m_StrSelectedWspPath.IsEmpty() )
		{
			pApp->m_StrCurWsp = dlgWspName.m_StrSelectedWspPath;	// ʹ���滻��ԭ�ļ������������½�һ���ļ����������µõ�ϵͳĬ�Ϲ�����
		}

		CString StrAbsPath = CPathFactory::ComPathToAbsPath(pApp->m_StrCurWsp);
		if ( StrAbsPath.GetLength() > 0 )
		{	
			pApp->m_StrCurWsp = StrAbsPath;
		}

		SaveWorkSpace(dlgWspName.m_StrName, StrFileVersion);		

		//
		pApp->ScanAvaliableWspFile(pDoc->m_pAbsCenterManager->GetUserName(), bNewUser);

		// Ĭ���½�һ���Ӵ���
		PostMessage(WM_COMMAND, ID_WINDOW_NEW, 0);
		bNeedInitTBWnd = true;
	}
	else if ( (dlgWspName.m_dwDoneAction & CDlgWspName::DoneDelete) != 0 )	// ��ɾ������
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
			bool32 bUserPseudo;									// ����ɨ��һ�Σ��Ը���wsp���ļ�·����ʾ��ʽ
			pApp->ScanAvaliableWspFile(pDoc->m_pAbsCenterManager->GetUserName(), bUserPseudo);
		}

		aWspInfos.Copy(pApp->m_aWspFileInfo);
	}

	// �򿪹�����	
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
				continue;		// ���⹤�������ܳ���������
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
	// ������ɫ�����ļ�
	CFaceScheme::Instance()->IoViewFaceObjectToXml();

	// �����ͷ����:
	CReportScheme::Instance()->SaveDataToXml();

	// �ϴ������� �ҵ���ѡ
	// 	bool32 bUpload = CUploadUserData::instance()->UploadUserBlock();
	// 	ASSERT( bUpload );

	// �����Լ��Ĺ�����

	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();

	CString StrFileName = pApp->m_StrCurWsp;

	string sPath = _Unicode2MultiChar(StrFileName);
	const char* KStrWorkSpaceDir = sPath.c_str();

	bool32 bReadOnly = IsWspReadOnly(KStrWorkSpaceDir);
	if ( bReadOnly )
	{
		TRACE(_T("Save readonly WSP cancelled: %s\r\n"), StrFileName.GetBuffer());
		StrFileName.ReleaseBuffer();
		return;		// ֻ�������� - ������
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

			bool32 bUserPseudo;									// ����ɨ��һ�Σ��Ը���wsp���ļ�·����ʾ��ʽ
			pApp->ScanAvaliableWspFile(pDoc->m_pAbsCenterManager->GetUserName(), bUserPseudo);
		}
	}

	// ���Ϊ������
	CDlgWspName dlg;
	AfxInitRichEdit();
	dlg.SetNewFileType(CDlgWorkSpaceSet::EFTWorkSpace);

	if ( IDOK == dlg.DoModal() )
	{
		// �����Լ��Ĺ�����
		// �����Ϊ:
		//     ʹ�õ�ǰ���������ݣ��½������滻ָ��������
		//	   ��ǰ������������

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
			StrFileName = dlg.m_StrSelectedWspPath;	// ʹ��ԭ�����ļ���
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
		pApp->m_StrCurWsp = ReNameFile(StrFileName, CMainFrame::ERTWSP);	// ��������·�� - ���ڴ�wspʹ�õ���ȫ·����������Ҫ��ת��
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
	else if ( (dlg.m_dwDoneAction & CDlgWspName::DoneDelete) != 0 && NULL != pApp		// ��ɾ������
		&& NULL != pDoc)	
	{
		bool32 bNewUser = false;
		pApp->ScanAvaliableWspFile(pDoc->m_pAbsCenterManager->GetUserName(), bNewUser);
	}
}

void CMainFrame::ClearCurrentWorkSpace()
{
	// �رչ�����(�ر������Ӵ���)
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
	//	����Ѿ���¼���������ѻ�����
	if(m_bDoLogin)
	{
		// ���̺���ʾ�����ѻ�����		
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
				if ( IDYES == MessageBox(L"�Ƿ�ȫ�����ѻ�����?", AfxGetApp()->m_pszAppName, MB_YESNO) )
				{
					CDlgDownLoad dlg(true);
					dlg.DoModal();
				}
			}
		}
	}

	//	����Ѿ���¼��������Ҫ������ɫ�����ļ�����ͷ�ļ�
	if(m_bDoLogin)
	{
		// ������ɫ�����ļ�
		CFaceScheme::Instance()->IoViewFaceObjectToXml();
		// �����ͷ����:
		CReportScheme::Instance()->SaveDataToXml();
	}

	// ����һ�����еĹ���ҳ��
	CWnd *pWnd = CWnd ::FromHandle(m_hWndMDIClient)->GetWindow(GW_CHILD); 
	while ( NULL != pWnd )   
	{ 
		CWnd* pTmp		= pWnd;
		CWnd* pTmpNext	= pTmp->GetWindow(GW_HWNDNEXT);

		CMPIChildFrame *pMPIFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, pWnd);
		if ( !pMPIFrame->GetIdString().IsEmpty() )
		{
			CCfmManager::Instance().SaveCfm(pMPIFrame->GetIdString(), pMPIFrame);	// �Զ�����ҳ��
		}

		pWnd = pTmpNext;
	}	


	if ( NULL != pDoc->m_pDlgbuding && NULL != pDoc->m_pDlgbuding->m_pDlgTrace && NULL != pDoc->m_pDlgbuding->m_pDlgTrace->m_pLogFile )
	{
		fclose(pDoc->m_pDlgbuding->m_pDlgTrace->m_pLogFile);		
	}

	//	����Ѿ���¼��������Ҫ��¼�û��Ĳ�����ͬ���û�����ѡ��
	if (m_bDoLogin)
	{

		//SaveWorkSpace();		// ���ñ��湤����


		// �˳���ʱ����ο��ϴ�һ���û���Ϊ��¼
		if (m_vUserDealRecord.size() > 0 && pDoc->m_pAutherManager)
		{
			pDoc->m_pAutherManager->GetInterface()->UserDealRecord(m_vUserDealRecord);
			m_vUserDealRecord.clear();
		}

		// ���ο��˳���ʱ��ͬ������ѡ��
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
			//	��ʾ��¼��
			break;
		}

		if (!m_bLoginSucc)
		{
			//	ȡ����¼
			iChoose = IDYES;
			break;
		}

		if (pApp->m_bTradeExit || pApp->m_bUserLoginExit ||pApp->m_HQRegisterExit || !m_bDoLogin)
		{
			iChoose = IDYES;
		}
		else
		{
			//	�����˳��Ի���
			CDlgSloganExit dlg;
			int ret = dlg.DoModal();
			if (IDCANCEL == ret )	//	�û�ѡ��رհ�ť��ȡ����ť
			{
				iChoose = IDNO;
			}

			if (IDOK == ret )		//	�û�ѡ���˳���ť�����µ�¼��ť
			{
				if (dlg.m_bReLogin)	//	�û�ѡ�����µ�¼��ť
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
/// ҳ��
void CMainFrame::OnNewChildframe() 
{
	// �½�һ������
	OnWindowNew();  
}

void CMainFrame::OnNewCfm()
{
	CDlgCfmName dlg;
	if ( dlg.DoModal() == IDOK )
	{
		// �����´���
		OnWindowNew();
		CCfmManager::Instance().CurCfmSaveAs(dlg.m_StrName);	// ���Ϊ������
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
			StrPrompt.Format(_T("���Ѿ�����ҳ��: %s\r\n�Ƿ����¼��أ�"), dlg.m_StrSelCfm.GetBuffer());
			dlg.m_StrSelCfm.ReleaseBuffer();
			if ( AfxMessageBox(StrPrompt, MB_YESNO | MB_ICONQUESTION |MB_DEFBUTTON2 ) == IDNO )
			{
				MDIActivate(pChildFrame);
				return;
			}
		}
		CCfmManager::Instance().LoadCfm(dlg.m_StrSelCfm, true);	// ���´�
	}
}

void CMainFrame::OnSaveasChildframe() 
{
	// ���Ϊһ������	
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
	// ����Ŀ¼	

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
	// ��������û������������⹤������Ϊlatest������
	// �õ����һ�ι����ļ�
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
	aWspInfos.Copy(pApp->m_aWspFileInfo);	// �����п�����.\��ʽ

	while (bContinue)
	{
		CTime writetime;
		bContinue = file.FindNextFile();
		CString StrfileDir = file.GetFilePath();
		file.GetLastWriteTime(writetime);

		// ֱ���ж��ļ���
		bool32 bAdd = true;
		for ( int i=0; i < aWspInfos.GetSize()/* && bNewUser*/ ; i++ )	// ��������Ϊlatest
		{
			if ( aWspInfos[i].IsSpecialWsp() && aWspInfos[i].m_StrFileName == file.GetFileTitle() )
			{
				// ������Ϊ���û���latest
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
	// �������ļ�
	int32   iLength = StrOldName.GetLength();
	CString StrNewName;

	if (ERTXML == eReNameType)   // ... �ĳ�ö��
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
	// �쳣���ʱ����Ĭ�Ϲ�����
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

	// Notic:  _tcheck_if_mkdir  ��·������,��Ҫ��"/"��β,�������һ���ֲ��ᵱ��Ŀ¼
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
	// �½�һ���Ӵ���,����ID ��ӱ�����ͼ�İ�� 

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
	// 	// �û����
	// 	CStringArray aUserBlockNameList;
	// 	CStringArray aUserBlockKeyList;
	// 
	// 	CNewMenu* PopUserBlock = pMenu->AppendODPopupMenu(L"�û����");
	// 	
	// 	T_BlockMenuInfo  BlockMenuInfoToAdd;
	// 
	// 	BlockMenuInfoToAdd.StrBlockMenuNames = L"�û����";
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
	//// �����Զ������ݼ�
	//if ( CSubjectUserBlock::EUBUBlock == eUpdateType )
	//{
	//	CUserBlockManager::Instance()->BuildUserBlockHotKeyList(m_HotKeyList);	
	//}

	// ���⽫����֪ͨ��ÿ����ȥ�����£�ɾ�����¼���
	BuildHotKeyList();

	// ֻҪ���²˵�������
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
	// 	// 1:���˵�
	// 	CMenu * pMenu = this->GetMenu();
	// 	CNewMenu *pNewMenu = DYNAMIC_DOWNCAST(CNewMenu,pMenu);
	// 	CMenu * pSubMenu = pNewMenu->GetSubMenu(L"���(&B)");
	// 	ASSERT( NULL != pSubMenu );
	// 	CNewMenu * pRootMenu = DYNAMIC_DOWNCAST(CNewMenu,pSubMenu);
	// 	ASSERT( NULL != pRootMenu );
	// 	
	// 	// �Ȱ��û����ɾ��,������װ��
	// 	for ( int32 j = 0; j < pRootMenu->GetMenuItemCount(); j++ )
	// 	{
	// 		CString StrMenuText;
	// 		pRootMenu->GetMenuText(j, StrMenuText, MF_BYPOSITION);
	// 		
	// 		if ( StrMenuText == L"�û����" )
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
	// 		if ( L"�û����" == BlockInfo.StrBlockMenuNames)
	// 		{
	// 			m_aBlockMenuInfo.RemoveAt(i);
	// 		}
	// 	}
	// 
	// 	// 2:װ�ز˵�
	// 	AppendUserBlockMenu(pRootMenu);
	// 
	// 	// 3: �����˵�
	// 	
	// 	//CMenu * pSubMenuF = pNewMenu->GetSubMenu(L"����(&F)");
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
	// 		if ( StrMenuText == L"�û����" )
	// 		{
	// 			CNewMenu *pDelUserMenu = DYNAMIC_DOWNCAST(CNewMenu, pRootMenuReport->GetSubMenu(i));
	// 			DeleteBlockReportItemData(pDelUserMenu);
	// 			int32 iPos = pRootMenuReport->DeleteMenu(i, MF_BYPOSITION);
	// 		}
	// 	}
	// 	
	// 	// 4: װ��
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
// �Ի�������
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
	trackMouse.dwFlags			=	GGT_TME_NONCLIENT | TME_LEAVE ; //TME_NONCLIENT | TME_LEAVE; //TME_NONCLIENT�Ǽ���ncclient����˼
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
	// �ù���Ҫ��ʹ�� Windows 2000 ����߰汾��
	// ���� _WIN32_WINNT �� WINVER ���� >= 0x0500��
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	bool32 bFlag = false;
	map<int, CNCButton>::iterator iter;
	for (iter=m_mapSysBtn.begin(); iter!=m_mapSysBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;

		// ��ť�Ƿ���Ҫ����
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

		// ��ť�Ƿ���Ҫ����
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

	// ϵͳ��ť
	int iButton = TSysButtonHitTest(pt);
	if (INVALID_ID != iButton)
	{
		m_mapSysBtn[iButton].LButtonDown(FALSE);
		DrawCustomNcClient();
		return;
	}

	// �˵���ť
	iButton = TMenuButtonHitTest(pt);
	if (INVALID_ID != iButton)
	{
		m_mapMenuBtn[iButton].LButtonDown();
		//m_mapMenuBtn[iButton].SetCheck(TRUE, FALSE);
		//
		//map<int, CNCButton>::iterator iter;
		//// �����������ϲ˵���ť
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
		// �˵���ť
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
		m_wndMDIClient.ArrangeIconicWindows(); // ������С������
	}

	// �����Ʒ�Ի���
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
			nHT = 30;	// ֻ���ظ�һ��ϵͳû��Ԥ�����HT��ϵͳ�������ڷ���
		}
		return nHT;
	}

	// ���Ƿ���������ht
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
	//����frame��activ��deactive�����ϵͳ�ڵ���NcActivateʱ�������ncclient��������Ҫ�����ػ�
	DrawCustomNcClient();
	m_wndReBar.RedrawWindow();
	m_wndStatusBar.RedrawWindow();
	//���������� 2013-11-12
	//m_wndCommentSBar.RedrawWindow();	��Ϊ�ĳ���ҳ����ˣ�������ˢ�¾Ͳ������ˡ�

	return bRet;
} 

void CMainFrame::OnNcLButtonDblClk(UINT nHitTest, CPoint point) 
{
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	rcWindow.bottom = rcWindow.top + m_rectCaption.bottom;

	// ��ǰ���Ƿ��ڱ�������
	if (rcWindow.PtInRect(point))
	{
		point.x -= rcWindow.left;
		point.y -= rcWindow.top;
		int iButton = TMenuButtonHitTest(point);

		// �ڰ�ť��˫��,���õ�������
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

		// ˫���������ϳ���ť�������������
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
	// DESCRIPTION:	���û�����ȼ��㣬Ȼ�����޸�
	CNewMDIFrameWnd::OnGetMinMaxInfo(lpMMI);

	// �趨���ڵ���С�ߴ�,
	lpMMI->ptMinTrackSize.x=1366;   //��   
	lpMMI->ptMinTrackSize.y=768;   //�� 

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
	// ��statusBar����һ��frame
	GetSystemMetrics(SM_CYFRAME);
	// 4��XP��Ĭ�ϵ�frame�ߣ�vista������4Ҫ��
	lpMMI->ptMaxSize.y	-=	3;
}

//0001681 - end

void CMainFrame::OnSelfDraw()
{
	ShowSelfDrawBar();
}

void CMainFrame::OnNetFlow()
{
	// ����ͳ��:
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
	// ��ת����ѡ�ɰ��
	// 1:������û�б��۱�

	CIoViewReport * pIoViewReport = FindIoViewReport(true);

	if (NULL == pIoViewReport)
	{
		//pIoViewReport = FindIoViewReport(false);
		//// û�еĻ�,�½�һ��IoViewReport
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
		// ��ǰ�ı��۱�����û����ѡ�ɰ��,�¼�һ����ѡ�ɰ���ȥ.
		// ��ȡ user_blocks.xml �ļ� ,�õ���һ����ѡ�ɵĽڵ���,
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
	// F10����
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
	// ���ʱ��,����������С�仯,�Զ�����
	if ( m_bMaxForSysSettingChange )
	{	
	}
	CNewMDIFrameWnd::OnSettingChange(uFlags, lpszSection);
}

void CMainFrame::SetEmptyGGTongView(CGGTongView * pView)
{
	//ASSERT(NULL != pView);   // �п�����Ҫ���ø�ViewΪNULL xl 0608
	if ( NULL != pView && !IsWindow(pView->GetSafeHwnd()) )
	{
		// Ҫô����Null��Ҫô���Ǵ���
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
	// �ж��Ƿ�����Ӵ���
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

	// �����Ӵ��ڵ������
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd(); 
	CIoViewBase * pIoViewActive =pMainFrame->FindActiveIoView();

	CIoViewManager * pIoViewManager = NULL;

	if ( NULL == pIoViewManagerIn )
	{
		if ( NULL == pIoViewActive )
		{
			// �հ״���! �½�һ������������ͼ		
			if ( NULL == GetEmptyGGTongView() )
			{
				// ֱ�Ӵ�"����" �˵���ѡ����ͼ,��ʱ��m_pEmptyGGTongView û������,Ϊ��
				CGGTongView * pView = pMainFrame->FindGGTongView(this,NULL);
				pIoViewManager	    = &(pView->m_IoViewManager);
			}
			else
			{
				// �ڿհ���ͼ,���Ҽ������			
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
			// IoViewManager��Ϊ�գ������ֲ��Ǵ��ڣ����������������ASSERT
			ASSERT( 0 );
		}
		return NULL;
	}

	int32 iSize = pIoViewManager->m_IoViewsPtr.GetSize();

	//////////////////////////////////////////////////////////////////////////
	//  �ж���ͼ�� �Ƿ�������� IoView,�����ڵ�ʱ�����,���ڵ�ʱ����ʾ�����ͼ
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
		// ��ʷ��ʱ,��Ҫ��������һ��.
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

		// ���ø÷������Ʒ��Ϣ
		CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();

		CMerch *pMerch = NULL;
		if (pMainFrame->GetMerchInSpecifyIoViewGroup(pIoView->GetIoViewGroupId(), pMerch))
		{
			// ��ǰ���������е���Ʒ��change���Ժ�������ܱ��Ϊ��Ҫ�������Ĺ�ע��Ʒ
			pIoView->OnVDataMerchChanged(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, pMerch);
		}
		//
		pIoViewManager->m_GuiTabWnd.Addtab(pstIoViewObject->m_StrLongName, pstIoViewObject->m_StrShortName, pstIoViewObject->m_StrTipMsg);				
		int32 iTabCount = pIoViewManager->m_GuiTabWnd.GetCount();
		pIoViewManager->m_GuiTabWnd.SetTabIcon(iTabCount - 1, 0);	

		if ( bShowNow )
		{					
			// ������ʾ, ȷ������ʾ�ĸ�ҵ����ͼ
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

			CWnd * pParent = pIoView->GetParent()->GetParent()->GetParent();  // GGTongView �ĸ���

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
		//���Ѵ��ڵ���ͼ����Ϊ��ǰ��ͼ
		pIoViewManager->m_GuiTabWnd.SetCurtab(iExistIndex);			
		pIoViewManager->OnTabSelChange();
	}


	m_pEmptyGGTongView = NULL;  // ÿ��ʹ��ǰGGTongView �������ֵ,������ÿ�	

	//
	// xl 1119 �����Ƿ���ʾ������Ҫ�ڴ���������, ��Ϊ�������ݲ��ڱ�IoViewActive������
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

	// ��������ֻ��10 ��.
	int32 iSize = m_aRecentMerchList.GetSize();

	// ��0-9 ����Ϊ����-��� ����Ʒ����.����²������Ʒ����,�������Ʒ�ŵ���һλ
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

	// ��ʵ����groupһ�µ��鷳
	ASSERT( 0 );

	m_bShowHistory = true;

	if (NULL == m_pHistoryChild && NULL == m_pHistoryTrend )
	{
		// Ӧ����ioview����

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
		// ��Ӧ�ó����������
		//ASSERT(0);
	}
	else if ( NULL != m_pHistoryChild && NULL == m_pHistoryTrend )
	{
		// �����Child ���½�һ��Trend ��ͼ
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
		// �������ͼ��ʾ����.
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
	// ����ǵ���ģ�Ӧ������ʾchildframe�汾�ķ�ʱ���൱��f5
	//	��ʷ�ľ͵��öԻ���
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
	// �����������
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
	// �õ������
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
// �˵�����:

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
			// ���reportҲ�п��ܲ����½���
			if ( NULL != pReport )
			{
				pReport->ChangeSimpleTabClass(nID-ID_REPORT_TYPEBEGIN);
			}
		}
		else
		{
			ASSERT( 0 );	// �����²������л����۱�����
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
	// ������ͼ���õĲ˵���:
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
		// ������ѡ��
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
		// ���
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
		// K����ʷ��ʱ
		if ( NULL != pIoViewBase )
		{
			pIoViewBase->PostMessage(WM_COMMAND, nID, 0);
		}
	}
	else if ( ID_CHART_INTERVALSTATISTIC == nID )
	{
		// K�߻��߷�ʱ������ͳ��
		if ( NULL != pIoViewBase )
		{
			pIoViewBase->PostMessage(WM_COMMAND, nID, 0);
		}
	}
	else if ( ID_CHART_QUANTSTATISTIC == nID)
	{
		// K�ߵ�����ͳ��
		if ( NULL != pIoViewBase )
		{
			pIoViewBase->PostMessage(WM_COMMAND, nID, 0);
		}
	}
	else if ( ID_INVESTMENT_DIARY == nID)
	{
		// Ͷ���ռ�
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
			// ����: ȫ������mainframe����
			if ( pIoViewBase->IsKindOf(RUNTIME_CLASS(CIoViewChart)) )
			{
				((CIoViewChart*)pIoViewBase)->DealEscKey(false);
			}
			else
			{
				// ����IoView���ص�Report
				pIoViewBase->DealEscKeyReturnToReport();
			}
		}
		break;
	case (IDM_IOVIEWBASE_ADD_IOVIEW):
		{
			// ��������:
		}
		break;
	case (IDM_IOVIEWBASE_TAB):
		{
			// �л�����:
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
			// �ر�����:
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
				{   //linhc 20100917�޸�������������
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
			// ȫ��/�ָ�:
			OnProcessF7();
		}
		break;

	case (IDM_IOVIEWBASE_FONT_BIGGER):
		{
			// ����Ŵ�:
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
			// ������С:
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
			// ����
			pGGTongView->AddSplit(CMPIChildFrame::SD_TOP);	
		}
		break;
	case (IDM_IOVIEWBASE_SPLIT_BOTTOM):
		{
			// ����:
			pGGTongView->AddSplit(CMPIChildFrame::SD_BOTTOM);	
		}
		break;
	case (IDM_IOVIEWBASE_SPLIT_LEFT):
		{
			// ����:
			pGGTongView->AddSplit(CMPIChildFrame::SD_LEFT);	
		}
		break;
	case (IDM_IOVIEWBASE_SPLIT_RIGHT):
		{
			// ����:
			pGGTongView->AddSplit(CMPIChildFrame::SD_RIGHT);	
		} 
		break;
	case (IDM_IOVIEWBASE_CLOSE):
		{
			// �ر�:
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
			// �������
			PostMessage(WM_COMMAND,ID_SETTING,0);
		}
		break;
	case (ID_LAYOUT_ADJUST):
		{
			// ���沼��
			PostMessage(WM_COMMAND,ID_LAYOUT_ADJUST,0);			
		}
		break;
	case (IDM_BELONG_BLOCK):
		{
			// �������
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
		// 			pCmdUI->Enable(TRUE);		// ������������
		// 			return;	
		// 		}
		// 		CMPIChildFrame *pActiveFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, MDIGetActive());
		// 		if ( NULL != pActiveFrame && pActiveFrame->IsLockedSplit() )
		// 		{
		// 			pCmdUI->Enable(FALSE);	// ��������������ͼ
		// 		}
		// 		else
		// 		{
		// 			pCmdUI->Enable(TRUE);
		// 		}
	}
}

void CMainFrame::OnPictureMenu(UINT nID)
{
	// ����ѡ�����ڿ������⿪��ֻ��û����- -
	CMPIChildFrame *pActiveFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, MDIGetActive());
	if ( NULL == pActiveFrame || !pActiveFrame->IsLockedSplit() )
	{
		// ������ģʽ�£�����Բ�����ͼ
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
			// �׶����У� �����û�ѡ��Ȼ����ҳ�棬������ҳ���򴴽�һ���µ���ͼ
			// �ֽ��׶����б����ͼ��&��Ѷһ����ҳ�洦��
			ShowPhaseSort(NULL);
		}
		else
		{
			// ҵ����ͼ�˵���:
			CreateIoViewByPicMenuID(nID, true);
		}
	}
	else
	{
		ASSERT( NULL != pActiveFrame );
		// ����ģʽ�´��ض�ҳ�棬��ʾ�ض���ͼ
		OnPicMenuCfm(nID);
	}
}

void CMainFrame::OnMenuIoViewReport(UINT nID)
{
	// �����۱���Ĳ˵���
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
	// ��K �ߴ���Ĳ˵���
	CIoViewBase * pIoViewBase = FindActiveIoView();

	if ( NULL == pIoViewBase )
	{
		return;
	}

	bool32 bValidIoView = false;

	if ( nID >= IDM_CHART_CYCLE_BEGIN && nID <= IDM_CHART_CYCLE_END )
	{
		// ָ��������ͼҲ���õ�����ѡ��Ĳ˵���
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
	// ��ָ�����鴦��Ĳ˵���
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
	///--- wangyongxue Ŀǰ��ֱ�Ӵ���ҳ��ʹ�ñ���ͼƬ������ʾ
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
	if(pDoc->m_isDefaultUser)           //�˺���Ϣ
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
	// 		// ��ʾ����ʾ����DisplayNotify������
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

	// chenfj ��ע�͵�
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

//���ؽ���״̬��  2013-11-14 by cym
void CMainFrame::OnViewCommentStatusBar()
{
	ShowControlBar(&m_wndCommentSBar, !m_wndCommentSBar.IsVisible(), 0);
}

//����״̬������״̬  2013-11-14 by cym
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
	// ȫ��ʹ����Ļ����
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

	
	// �������ͻ���
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

	// �߿�
	Pen pen(Color(255,34,34,40), 2*m_nFrameWidth);
	Pen nPen(Color(255, 51, 56, 63), 2*m_nFrameWidth);
	graphics.DrawLine(&pen, 0, 0, rcWindow.Width(), 0);
	graphics.DrawLine(&nPen, 0, rcWindow.Height(), rcWindow.Width(), rcWindow.Height());
	graphics.DrawLine(&pen, 0, 0, 0, TITLE_HEIGHT + 8);
	graphics.DrawLine(&nPen,0,TITLE_HEIGHT+ 8, 0, rcWindow.Height());
	graphics.DrawLine(&pen, rcWindow.Width(), 0, rcWindow.Width(), TITLE_HEIGHT+ 8);
	graphics.DrawLine(&nPen, rcWindow.Width(), TITLE_HEIGHT+ 8, rcWindow.Width(), rcWindow.Height());

	// ����������ͼ
	destRect.X = m_nFrameWidth;
	destRect.Y = m_rectCaption.top;
	destRect.Width  = m_rectCaption.Width();
	destRect.Height = m_rectCaption.Height();
	ImageAttributes ImgAtt;
	ImgAtt.SetWrapMode(WrapModeTileFlipXY);
	graphics.DrawImage(m_pImgLogo, destRect, 0, 0, m_pImgLogo->GetWidth(), m_pImgLogo->GetHeight(),	UnitPixel, &ImgAtt);

	// ���������logo
	int32 iAdjust = 0;
	int32 iLogoWidth = m_pImgLogoLeft->GetWidth();
	destRect.X = m_nFrameWidth + iAdjust;
	destRect.Y = m_rectCaption.top + (m_rectCaption.Height() - m_pImgLogoLeft->GetHeight()) / 2 ;
	destRect.Height = m_pImgLogoLeft->GetHeight();//m_rectCaption.Height() - iAdjust * 2;
	destRect.Width = m_pImgLogoLeft->GetWidth();
	graphics.DrawImage(m_pImgLogoLeft, destRect);

	int32 iTitleLength = 0;
	int32 iSpace = 40; // ����ͼ��ľ���
	// �������ı�
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	CString strTitle = pApp->m_pszAppName;
	//��ȡ�ͷ��绰����
	auth::T_TextBannerConfig  CommTextCfg;
	CGGTongDoc* pDoc = pApp->m_pDocument;
	if (!pDoc->m_pAutherManager)
	{
		return;
	}
	pDoc->m_pAutherManager->GetInterface()->GetServiceDispose()->GetTextBanner(CommTextCfg);
	CString strCustServ ;
	strCustServ.Format(_T("����: %s"),CommTextCfg.wszServiceTel);
	CString strNameTitle = L"����:";
	//��ȡ�û��ǳ�
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

	//��������
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
	//�������ı�
	m_Font1.m_StrName = gFontFactory.GetExistFontName(L"΢���ź�");	//...
	m_Font1.m_Size	 = 9.5f;
	m_Font1.m_iStyle	 = FontStyleRegular|FontStyleBold;	
	Gdiplus::FontFamily fontFamily(m_Font1.m_StrName);
	Gdiplus::Font font1(&fontFamily, m_Font1.m_Size, m_Font1.m_iStyle, UnitPoint);

	//�ͷ��绰
	m_Font2.m_StrName = gFontFactory.GetExistFontName(L"΢���ź�");	//...
	m_Font2.m_Size	 = 9.5f;
	m_Font2.m_iStyle	 = FontStyleRegular;	
	Gdiplus::Font font2(&fontFamily, m_Font2.m_Size, m_Font2.m_iStyle, UnitPoint);

	//�û��ǳ�
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


	// �ָ�ͼƬ
	CRect rcSpliter;
	int iSpliterW = 1;
	int iSpliterH = 14;
	if(m_pImgSpliter && m_pImgSpliter->GetLastStatus() == Ok)
	{
		iSpliterW = m_pImgSpliter->GetWidth();
		iSpliterH = m_pImgSpliter->GetHeight();
	}

	int iBtnTopDistance = (m_rectCaption.Height() - iSpliterH) / 2 + 1;
	int iBegPos = rcWindow.left + m_nFrameWidth + iLogoWidth;    // ������ʼλ��
	int iTop = rcWindow.top + m_nFrameWidth + iBtnTopDistance;                             // ������ʼλ��
	int iBottom = iTop + iSpliterH;
	destRect.X = iBegPos;
	destRect.Y = iTop;
	destRect.Width  = iSpliterW;
	destRect.Height = iSpliterH;
	graphics.DrawImage(m_pImgSpliter, destRect, 0, 0, iSpliterW, iSpliterH, UnitPixel);

	int iCustomSpace = 16;     // �ָ�ͼƬ����ϵͳ��ť��λ��
	iBegPos += (iSpliterW + iCustomSpace);
	// ����ϵͳ��ť
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

	// ���ƹ�ʽ����ť
	iBegPos += iBtnWidth;
	if(m_pImgMenuAlarm && m_pImgMenuAlarm->GetLastStatus() == Ok)
	{
		iBtnWidth = m_pImgMenuAlarm->GetWidth();
		iBottom = iTop + m_pImgMenuAlarm->GetHeight() / 3;
	}
	CRect rcFormulaManage(iBegPos, iTop, iBegPos + iBtnWidth, iBottom);
	m_mapMenuBtn[ID_MENU_FORMULA_MANAGE].SetRect(rcFormulaManage);
	//m_mapMenuBtn[ID_MENU_FORMULA_MANAGE].SetTextOffPos(CPoint(8,0));

	// ��������Ԥ����ť
	iBegPos += iBtnWidth;
	if(m_pImgMenuAlarm && m_pImgMenuAlarm->GetLastStatus() == Ok)
	{
		iBtnWidth = m_pImgMenuAlarm->GetWidth();
		iBottom = iTop + m_pImgMenuAlarm->GetHeight() / 3;
	}
	CRect rcAlarm(iBegPos, iTop, iBegPos + iBtnWidth, iBottom);
	m_mapMenuBtn[ID_MENU_ALARM].SetRect(rcAlarm);

	// ����ѡ�ɰ�ť
	iBegPos += iBtnWidth;
	if(m_pImgMenuSelStock && m_pImgMenuSelStock->GetLastStatus() == Ok)
	{
		iBtnWidth = m_pImgMenuSelStock->GetWidth();
		iBottom = iTop + m_pImgMenuSelStock->GetHeight() / 3;
	}
	CRect rcSelStock(iBegPos, iTop, iBegPos + iBtnWidth, iBottom);
	m_mapMenuBtn[ID_MENU_SEL_STOCK].SetRect(rcSelStock);
	m_mapMenuBtn[ID_MENU_SEL_STOCK].SetTextOffPos(CPoint(-4,0));

	// ���ƽ��װ�ť
	iBegPos += iBtnWidth;
	if(m_pImgMenuTrade && m_pImgMenuTrade->GetLastStatus() == Ok)
	{
		iBtnWidth = m_pImgMenuTrade->GetWidth();
		iBottom = iTop + m_pImgMenuTrade->GetHeight() / 3;
	}
	CRect rcTrade(iBegPos, iTop, iBegPos + iBtnWidth, iBottom);
	m_mapMenuBtn[ID_MENU_TRADE].SetRect(rcTrade);
	m_mapMenuBtn[ID_MENU_TRADE].SetTextOffPos(CPoint(-4,0));

	// ���ư�����ť
	iBegPos += iBtnWidth;
	if(m_pImgMenuHelp && m_pImgMenuHelp->GetLastStatus() == Ok)
	{
		iBtnWidth = m_pImgMenuHelp->GetWidth();
		iBottom = iTop + m_pImgMenuHelp->GetHeight() / 3;
	}
	CRect rcHelp(iBegPos, iTop, iBegPos + iBtnWidth, iBottom);
	m_mapMenuBtn[ID_MENU_HELP].SetRect(rcHelp);
	m_mapMenuBtn[ID_MENU_HELP].SetTextOffPos(CPoint(-4,0));

	// �˵����ػ�
	//CRect rcMenuBar;
	//if (NULL != m_pMenuBar)
	//{
	//	m_pMenuBar->ReleaseMenuBtn();
	//	m_pMenuBar->OnPaint(&graphics);
	//	m_pMenuBar->GetMenuBarRect(rcMenuBar);
	//}
	int32 iMenuRight = rcHelp.right;

	// ϵͳ��ťλ��
	int iBtnWidth2 = 29;
	iTop += 2;    //����ϵͳ��ťλ�ã�ʹ�������ʾ
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


	//����ť
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
	if(!pDoc->m_isDefaultUser)           //���ο��û���ʾ�û�ͷ���û��ǳƣ��˳���ť
	{
		int iBtnWidthExit = 45;
		if(m_pImgMenuExit && m_pImgMenuExit->GetLastStatus() == Ok)
		{
			iBtnWidthExit = m_pImageLogin->GetWidth();
		}
		int iBegPosExit = iBegPos - iBtnWidthExit - 33;

		//�˳�
		CRect rExit(iBegPosExit, iTop , iBegPosExit + iBtnWidthExit, iBottom );
		m_mapMenuBtn[ID_MENU_EXIT].SetRect(rExit);

		int iBtnWidthName = 0;
		int iBegPosName = iBegPosExit ;
		if(!strNickName.IsEmpty())
		{
			iBtnWidthName = (int)gNickNamerect.Width + 10;
			iBegPosName = iBegPosName - iBtnWidthName - 10 ;  //�����ǳ���ʾ��λ�ã�10���������ǳƺ��˳���ť֮��ļ�϶

			rcLeft.SetRect(iBegPosName, iTop, iBegPosName+iBtnWidthName, iBottom);
			if (pApp->m_pConfigInfo->m_bMember && (EType_Hide!=pApp->m_pConfigInfo->m_btnUserInfo.m_eShowType))
			{
				CRect rectName = rcLeft;
				m_mapMenuBtn[ID_MENU_USER_NAME].SetRect(rectName);
				//gNameTitlerect.X = rcLeft.left - rcNameTitle.Width + 3;
				//gNameTitlerect.Y = m_rectCaption.top-2;
				//gNameTitlerect.Height = rcLeft.Height();
				//graphics.DrawString(strNameTitle, strNameTitle.GetLength(), &font2, gNameTitlerect, &strFormat,&brush );    //��ʱ����Ҫ��ʾ "����", ����
			}
		}
		else
		{
			iBegPosName = iBegPosName - 50;
		}

		// �û�ͷ��
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
	else            //��¼��ע��
	{
		int iBtnWidthLogin = m_pImageLogin->GetWidth();
		int iBegPosLogin = iBegPos -  iBtnWidthLogin - 48;

		//��¼
		CRect rLogin(iBegPosLogin, iTop , iBegPosLogin+iBtnWidthLogin, iBottom );
		m_mapMenuBtn[ID_MENU_LOGIN].SetRect(rLogin);

		RectF destRect;
		destRect.X = iBegPosLogin - m_pImgLoginSpliter->GetWidth();
		destRect.Y = iTop + 3;
		destRect.Width  = m_pImgLoginSpliter->GetWidth();
		destRect.Height = m_pImgLoginSpliter->GetHeight();
		graphics.DrawImage(m_pImgLoginSpliter, destRect, 0, 0, m_pImgLoginSpliter->GetWidth(),m_pImgLoginSpliter->GetHeight(), UnitPixel);
		

		//ע��
		iBegPosLogin = iBegPosLogin - iBtnWidthLogin - m_pImgLoginSpliter->GetWidth() ;
		rcLeft.SetRect(iBegPosLogin, iTop, iBegPosLogin+iBtnWidthLogin, iBottom );
		m_mapMenuBtn[ID_MENU_REGISTER].SetRect(rcLeft);
	}

	// ��������ʱ����ʾ����������
	{
		//���Ʊ������ͻ��绰�ı�
		gCustServrect.X = rcLeft.left - rcCustServ.Width- 12;
		gCustServrect.Y = 2;

		if (iMenuRight < gCustServrect.X)
		{
			gCustServrect.Height = m_rectCaption.bottom - 1;
			brush.SetColor(Color(0xf0, 0xf0, 0xf0));
			//graphics.DrawString(strCustServ, strCustServ.GetLength(), &font2, gCustServrect,&strFormat, &brush);
		}

		// ���߿ͷ�
		//CRect rcOnlienServ ;
		//rcOnlienServ.left = gCustServrect.X - m_pImgOnlineServ->GetWidth() - 16;
		//rcOnlienServ.top = iTop - 8;
		//rcOnlienServ.right = rcOnlienServ.left + m_pImgOnlineServ->GetWidth();
		//rcOnlienServ.bottom = iBottom;
		//m_mapMenuBtn[ID_MENU_ONLINE_SERVICE].SetRect(rcOnlienServ);
	}


	int32 iSysLeft = iBegPos;

	// ���Ʊ������ı�
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
	grect.Y = m_rectCaption.top ;	// �����������ֵ�Y���꣬������ʾ
	grect.Height = m_rectCaption.Height();
	//graphics.DrawString(strTitle, strTitle.GetLength(), &font1, grect, &strFormat, &brush);

	//
	CRect rcPaint;
	dc.GetClipBox(&rcPaint);
	map<int, CNCButton>::iterator iter;

	// �����������ϲ˵���ť
	for (iter=m_mapMenuBtn.begin(); iter!=m_mapMenuBtn.end(); ++iter)
	{

		if(!iter->second.GetCreate())
		{
			continue;;
		}
		CNCButton &btnControl = iter->second;
		CRect rcControl;
		btnControl.GetRect(rcControl);

		// �жϵ�ǰ��ť�Ƿ���Ҫ�ػ�
		if (!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;
		}
		btnControl.DrawButton(&graphics);
	}

	// ������������ϵͳ��ť
	for (iter=m_mapSysBtn.begin(); iter!=m_mapSysBtn.end(); ++iter)
	{

		CNCButton &btnControl = iter->second;
		CRect rcControl;
		btnControl.GetRect(rcControl);

		if(!iter->second.GetCreate())
		{
			continue;;
		}
		// �жϵ�ǰ��ť�Ƿ���Ҫ�ػ�
		if (!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;
		}

		int index = 0;
		int iD = btnControl.GetControlId();
		switch (iD) //ϵͳ��ť 1 min, 2 max, 3 restore, 4 close
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
	// �������ļ�������·����
	CFileFind file;
	BOOL bExist = file.FindFile(StrFilePath);	
	ASSERT(bExist);	

	// ת����׺��
	// xl 0621 ΪʲôҪת����׺����

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

		// ���õ�ǰ������,Ϊ�򿪵��ļ���
		if ( 0 == KStrXmlRootElementAttrDataWorkSpace.CompareNoCase(StrAttridata) )
		{	
			CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
			CGGTongApp * pApp = (CGGTongApp *)AfxGetApp();

			pApp->m_StrCurWsp = StrFilePath;
			// ��Ҫ�����һ��
			pMainFrame->ClearCurrentWorkSpace();
			pMainFrame->FromXml(FilePath);				
			bRet = true; // �����Ƿ�xml�ɹ������ڶ��Ѿ��޿������
		}
		else
		{
			MessageBox(L"�ļ���������,��ѡ�������������ļ�!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);		
		}
	}
	else
	{
		MessageBox(L"�����鿴��ҳ���ļ�������!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		//... ѡ��һ��Ĭ�ϵ�		
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
		pMerch = pIoView->GetMerchXml();	// Ĭ��ΪpMerchXml
	}

	if ( NULL != pIoView && NULL != pMerch )
	{
		if ( NULL != m_pDlgF10 )
		{
			//�ж϶Ի����Ƿ�����ʾ
			if (m_pDlgF10->IsWindowVisible())
			{
				if(m_pDlgF10->OnTypeChange(L"002"))
				{
					//�Ի������ݳɹ���F11�л���F10
				}
				else
				{
					//�Ի��������л�ʧ�ܣ�˵���������ݾ���F10���ʵ�2�ε���������ضԻ���
					m_pDlgF10->ShowWindow(SW_HIDE);
				}
			}
			else
			{
				//�Ի��������صģ�������ʾ���л�����
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
	//--- wangyongxue 2017/03/04 ��ʱ�����θ�����Ѷ��ݼ���Ӧ
	if (m_pNewWndTB)
	{
		m_pNewWndTB->DoOpenCfm(L"��Ѷ");
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
		pMerch = pIoView->GetMerchXml();	// Ĭ��ΪpMerchXml
	}

	if ( NULL != pIoView && NULL != pMerch )
	{
		if ( NULL != m_pDlgF10 )
		{
			//�ж϶Ի����Ƿ�����ʾ
			if (m_pDlgF10->IsWindowVisible())
			{
				if(m_pDlgF10->OnTypeChange(L"001"))
				{
					//�Ի������ݳɹ���F10�л���F11
				}
				else
				{
					//�Ի��������л�ʧ�ܣ�˵���������ݾ���F11���ʵ�2�ε���������ضԻ���
					m_pDlgF10->ShowWindow(SW_HIDE);
				}
			}
			else
			{
				//�Ի��������صģ�������ʾ���л�����
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
	if ( m_rectClose.PtInRect(pt) )	//�������
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
				_tcscpy(pszTip, _T("���ϻ�ԭ"));
			}
			else
			{
				_tcscpy(pszTip, _T("��С��"));
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
				_tcscpy(pszTip, _T("���»�ԭ"));
			}
			else
			{
				_tcscpy(pszTip, _T("���"));
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
			_tcscpy(pszTip, _T("�˳�����"));
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
		return -1;		// ��ֹMainFrame�������ؼ���tip��ʾ
	}
	return iRet;
}

void CMainFrame::OnDlgSyncExcel()
{
	CIoViewSyncExcel::Instance().UserManageSyncBlock();
}

void CMainFrame::OnMenuIoViewStarry( UINT nID )
{
	// ��ָ�����鴦��Ĳ˵���
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
	// ��ָ�����鴦��Ĳ˵���
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
	// ��ָ����Ʒû
	CIoViewBase *pIoViewActive = FindActiveIoView();
	CMerch *pMerchXml = NULL;
	if ( NULL != pIoViewActive )
	{
		pMerchXml = pIoViewActive->GetMerchXml();
	}
	CMPIChildFrame *pChildFrame = NULL;
	CMPIChildFrame *pChartFrame = NULL;	// ͼ��ҳ��
	CIoViewBase *pF7IoView = NULL;
	// �����ڱ���ͼ����ָ��ҪF7����ͼ�����û���ҵ������ͼ����ͼ���ٴβ��ң������޲�����
	if ( nChartPicId <= 0 )
	{
		pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, MDIGetActive());
		if ( NULL != pChildFrame )
		{
			// �����ڵ�ǰҳ����
			pF7IoView = FindIoViewInFrame(nF7PicId, pChildFrame);
		}
	}

	// �½���ҳ�涼�����صģ���Ҫ��ʾ
	if ( NULL == pF7IoView )
	{
		if ( nChartPicId <= 0 )
		{
			// ������ͼ����ͼ�����Ե�ȡ
			pChartFrame = FindChartIoViewFrame(true, true);
		}
		else
		{
			// ��ָ��ͼ����ͼ�����Բ���
			pChartFrame = FindAimIoViewFrame(nChartPicId, true, true);
		}
		if ( NULL != pChartFrame )
		{
			pF7IoView = FindIoViewInFrame(nF7PicId, pChartFrame);
			if ( NULL == pF7IoView && nChartPicId <= 0 )
			{
				// ��ǰ�򿪵����ͼ����ͼû��Ҫ���IoView������~
				ASSERT( 0 );
				// �ٴγ���������һ��ͼ������
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

		// ���ȵ�ͼ�괰��û�зֱʣ�����������һ��
		if ( NULL == pF7IoView )
		{
			ASSERT( 0 );	// ��ʱ�Ѿ����ڴ���״̬�����������û�����
			pF7IoView = FindIoViewByPicMenuId(nF7PicId, NULL, false, false, false, false, true);
		}
	}

	ASSERT( NULL != pF7IoView );	// Ӧ���ҵ��ˣ�����Ҳ���ų�û�����ú�ҳ��

	if ( NULL != pF7IoView )
	{
		// �л���Ʒ
		if ( NULL != pMerchXml )
		{
			OnViewMerchChanged(pF7IoView, pMerchXml);
		}

		pF7IoView->BringToTop();	// �ö�

		if ( NULL != pF7IoView->GetIoViewManager() )
		{
			CGGTongView *pView = DYNAMIC_DOWNCAST(CGGTongView, pF7IoView->GetIoViewManager()->GetParent());
			CMPIChildFrame *pF7Frame = DYNAMIC_DOWNCAST(CMPIChildFrame, pF7IoView->GetParentFrame());
			if ( NULL != pF7Frame && pF7Frame->IsF7AutoLock() && pView != pF7Frame->GetF7GGTongView() )
			{
				// ��ԭ
				OnProcessCFF7(pF7Frame);
			}

			if ( NULL != pView && (NULL == pF7Frame || pView != pF7Frame->GetF7GGTongView()) )
			{
				// ���
				ASSERT( !pView->GetMaxF7Flag() );
				OnProcessF7(pView);
			}
		}
	}

	if ( NULL != pChartFrame )
	{
		//pChartFrame->ShowWindow(SW_SHOW);	// ����д�����ͼ��ҳ�棬��Ҫ��ʾ��
	}

	// �洢������
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
	// ������ͼ��Ҫ����ת(��mainframe��hotmsg merch)��Ҫת������������
	//  1. ͬ��childframe��chart������ʾ������ͼ��chart���л���Ʒ��end
	//  2. ��chart���Ǿ�ֻ�п��Ƿ����k ���� ��ʱ��ϵͳҳ�棬������ˣ�����ת���ȷ�ʱ����k
	//		�������ڵĻ���������ʱҳ�� end
	// ���������ҳ�����ת����¼��һ��Դchildframe��ESC��
	// ��ʱ����k����ͼ�����۷����Ƿ������ͬ�����ᱻ��ת������Ʒ
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
		// ��Դ���� || �Ǳ�׼ҵ����ͼ����
		// ���ǰ����˿�ݼ�����
		pIoViewSrc = FindActiveIoView();	// �ҵ�����ı�׼ҵ����ͼ
		if ( NULL == pIoViewSrc )
		{
			// �޼�����ͼ������ҷ�ʱ����k��ҳ��, �������򴴽�
			pChildFrame = FindChartIoViewFrame(true, true);
			if ( NULL != pChildFrame )
			{
				pIoViewSrc = FindActiveIoViewInFrame(pChildFrame);
			}
		}
		if ( NULL == pIoViewSrc )
		{
			ASSERT( 0 );	// �ⶼʧ����- -
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

	// ��¼��ʱ�ĵ�ǰframe���Ա�esc
	CMPIChildFrame *pLastActiveFrame = (CMPIChildFrame *)MDIGetActive();

	// ���˱�׼ҵ����ͼ����ͬҳ���Ƿ���ͬ�����ʱ����K�ߣ����û�еĻ�������÷�ʱ K��ҳ��
	// ���pIoViewSrc���Ƿ�ʱ����K���Ͳ�������
	// �µ�ҵ������������κ�һ������Ʒ��ͼ�ڸ�ҳ����ڣ������л��õ���Ʒ��ͼ����������Ʒ(ͨ����ò��������)
	CIoViewBase *pChart = NULL;
	pChart = FindChartIoViewInSameFrame(pIoViewSrc);	// ͬ�����޽��������Ҫ�½�ҳ��
	if ( NULL == pChart )
	{
		// ���Ϊ������κοɼ��ĵ���Ʒ��ͼ������ʾ�õ�������ͼ������
		// ���ڶ�����ͼ�����������
		// ����ͼ�Ĳ���ȥ��
		// pChart = FindIoViewInFrame(ID_PIC_TICK, DYNAMIC_DOWNCAST(CMPIChildFrame, pIoViewSrc->GetParentFrame()), true);
		// 
		// if ( NULL == pChart )
		{
			// ����ͼû�У� �Ǵ�ҳ����ͼ��
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
		// Ӧ����һ��chartͼ�γ�����
		ASSERT( 0 );
		// ��srcͬ��Ļ����л���Ʒ��
		pIoViewChangeMerch = pIoViewSrc;	// û��chart��ҲҪ����ҵ��ı���Ϊ��Ҫ�����Դ����ͼ��ͬ����Ʒ
	}
	// �˺�Ӧ����pChart
	ASSERT( NULL != pIoViewChangeMerch );
	if ( NULL == pIoViewChangeMerch )
	{
		return;		// ʵ�����Ҳ���һ�����ʵ���, û����Ҳû�취��
	}

	// ��chart�ˣ���ʼ�л�
	// chart ��ͼ��Ҫ�л�������Դ��ͼ������䲻ͬ���飬ҲҪ�л�
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

	pIoViewChangeMerch->BringToTop();		// ������ͼ�ö�
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

	// ��ʱ��childframe�Ƿ���ǰ��¼�Ĳ�ͬ�����У�����ǰ�ļ�¼Ϊesc��ת��¼
	CMDIChildWnd *pNowActiveFrame = MDIGetActive();
	if ( pNowActiveFrame != pLastActiveFrame && NULL != pLastActiveFrame )
	{
		AddToEscBackFrame(pLastActiveFrame);	// ��¼��ҳ��
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
		OnShowMerchInChart(pMerch, NULL);		// Ĭ�Ϲ���
		return;
	}

	// ָ��chartҳ��
	CMPIChildFrame *pOldFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, MDIGetActive());
	ASSERT( ID_PIC_KLINE == nPicId || ID_PIC_TREND == nPicId );	// Ŀǰֻ��������ҳ����֧�ֵ�
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
		//pChartFrame->ShowWindow(SW_SHOW);	// ������½��Ļ�����ʾ�µ�ҳ��
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
		// Ѱ�Ҽ������ΪԴ����
		pIoViewSrcOrg = FindActiveIoView();
		if ( pIoViewSrcOrg == NULL )
		{
			return;
		}
	}

	CMerch *pMerch = pIoViewSrcOrg->GetMerchXml();
	if ( NULL == pMerch )
	{
		// ����Ʒ���л�
		ASSERT( 0 );
		return;
	}

	CIoViewBase *pIoViewSrc = pIoViewSrcOrg;

	// ����(����ת������ͼ��ҳ���е�һ�֣����۷������):
	//	��ǰҳ���Ƿ�ͼ��ҳ�棬����ת���뵱ǰ�����ͬ���͵�ҳ��(��ʱ��K)�������������ͣ����ʱ
	//		������˼�൱�ڷŴ�
	//	��ǰҳ����ͼ���һ�֣�����������һ��ͼ��ҳ����

	CMPIChildFrame *pLastActiveFrame = (CMPIChildFrame *)MDIGetActive();

	// ��ǰ�����ҳ���Ƿ���ͼ��ҳ��
	CString StrKline, StrTrend;
	CCfmManager::Instance().GetSystemDefaultCfmName(CCfmManager::ESDC_KLine, StrKline);
	CCfmManager::Instance().GetSystemDefaultCfmName(CCfmManager::ESDC_Trend, StrTrend);
	if ( pLastActiveFrame != NULL )
	{
		CString StrId = pLastActiveFrame->GetIdString();
		if ( StrKline == StrId )
		{
			// ��ת����ʱ
			LoadSystemDefaultCfm(ID_PIC_TREND, pMerch, true);		// ���¼��ת
			return;
		}
		else if ( StrTrend == StrId )
		{
			// ��ת��K��
			LoadSystemDefaultCfm(ID_PIC_KLINE, pMerch, true);
			return;
		}
		// ����ҳ�� ������һ������
	}

	// ��ǰҳ���ͼ��ҳ��
	// ����������ͼ�Ƿ�Ϊͼ����ͼ, ���򴴽���Ӧ��ͼ��ҳ��, ���򴴽���ʱҳ��
	if ( !pIoViewSrc->IsKindOf(RUNTIME_CLASS(CIoViewKLine)) )
	{
		// ��k�ߣ������Ҫ������ʱ
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
		// ��ʱ��frame wnd�Ѿ�recalc layout�� 
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
	//	// ��ʱ��frame wnd�Ѿ�recalc layout�� 
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

	// Rebar��band����, ����rebar��Ԥ����header & gripper������׼������
	//if ( m_wndReBar.m_hWnd != NULL )
	//{
	//	// ��ʱ��frame wnd�Ѿ�recalc layout�� 
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
		// ��ֹ����û��Ȩ�޵İ�ť
		//pCmdUI->Enable(FALSE);
		pCmdUI->Enable();		// ȫ���������öԻ�����ʾ

		pCmdUI->SetCheck(0);
		UINT nId = pCmdUI->m_nID;

		// �����ִ�����ѯcode��ͨ��code��ѯ�Ƿ�Ҫ�û�

		// �����ִ��������ûҲ��ְ�ť
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
			// �յ�Ԥ�� ���̻����ɱ� ���̲�λ���� �������� ����ս�Գ��� ����ս���Ӷ� ǿ���ֽ� ��ͼָ����߸�ͼָ��
			// ����ָ�꣬�ж��Ƿ���K�߳���
			CString StrIndexName = StrMenuItem;	// �˵����ƾ���ָ������
			bool32 bIndex = IsIndexInCurrentKLineFrame(StrIndexName, 0);	// ��ȫ����ָ����趨��

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
			// ����ѡ��
			// ����ѡ��ҲҪ�����ִ���ѡ��ģ�TODO
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
	// һЩ����
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
		return;	// ǰ���� Init ��ɫ�˵�ʱû�����úã�
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
		// �յ�Ԥ�� ���̻����ɱ� ���̲�λ���� �������� ����ս�Գ��� ����ս���Ӷ� ǿ���ֽ� ������
		// 0914 ��� - // �� �� --����ս���Ӷ� ����ս�Գ��� ǿ���ֽ� ��ǰ����k����ͼָ�� �������Ѹ�����ͼָ��
		// ָ����Թرգ���ͼָ����Ϊma����ͼָ����Ϊmacd
		// ����ָ����ȫ����������K���г���

		CString StrIndexName = StrMenuItem;
		bool32 bIsOpenIndex = IsIndexInCurrentKLineFrame(StrIndexName);

		// ���۴�ָ�� or �ر�ָ�꣬����Ҫ�е�K��
		LoadSystemDefaultCfm(ID_PIC_KLINE);		// ����K��ҳ��

		CIoViewKLine *pIoKLine = (CIoViewKLine *)GetIoViewByPicMenuIdAndActiveIoView(ID_PIC_KLINE, false, true, false, true, false, true);
		if ( NULL!=pIoKLine )
		{
			if ( !bIsOpenIndex )
			{
				// K����ͼ�����ָ�����ͽ���Ԥ�����л�
				pIoKLine->AddShowIndex(StrIndexName, false, true, true);
			}
			else
			{
				// �滻ָ��ָ��
				CFormularContent *pFormula = CFormulaLib::instance()->GetFomular(StrIndexName);
				ASSERT( NULL != pFormula );
				if ( NULL != pFormula )
				{
					if ( CheckFlag(pFormula->flag, CFormularContent::KAllowMain) )
					{
						// ��ͼ
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
			ASSERT( 0 );	// û��K�ߣ���û�а취��
		}
	}
	else if ( nId >= ID_SPECIAL_SELECT_START && nId <= ID_SPECIAL_END )
	{
		// ����ѡ��
		// ���ղ˵��ִ������� TODO

		// �������� ����ϴ�� �λ���ͣ ����ǿׯ �������� Ϊ���۱�ѡ��
		CStringArray aNames;
		aNames.Add(_T("����ѡ��"));
		{
			// ����ָ��ҳ��
			LoadSpecialCfm(aNames[0]);
		}
		{
			CStockSelectManager::E_IndexSelectStock eJGLT = CStockSelectManager::StrategyJGLT;
			bool32 b = CStockSelectManager::GetIndexSelectStockEnumByCmdId(nId, eJGLT);
			ASSERT( b );
			DWORD dwNewFlag = eJGLT;
			// ��ô������Щѡ�������أ����⣬�룬���� TODO

			CStockSelectManager::Instance().SetIndexSelectStockFlag( dwNewFlag );	// ���⴦��
			CStockSelectManager::Instance().SetCurrentReqType(CStockSelectManager::IndexShortMidSelect);
		}
	}
	else
	{
		ASSERT( 0 );
	}
}



//linhc 20100908��ӡ�������顱�˵���Ӧ����
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
	// ��Ƶ��ѵ
}

LRESULT CMainFrame::OnSetWindowText( WPARAM w, LPARAM l )
{
	LPCTSTR pcsz = (LPCTSTR)l;	// ����ΪNULL

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

	// ����С��������Ի���
	if ( NULL == m_pKBParent || !IsWindow(m_pKBParent->GetSafeHwnd()))	// ����������!
	{
		m_pKBParent = this;
		m_dwHotKeyTypeFlag = EHKTCount;
		SetForceFixHotkeyTarget(false);
	}

	if ( m_dwHotKeyTypeFlag != EHKTCount )
	{
		// ��֧���ض�һ�����Ŀǰ��û��֧�ֶ�����
		m_pDlgKeyBoard = new CKeyBoardDlg(m_HotKeyList,m_pKBParent,(E_HotKeyType)m_dwHotKeyTypeFlag);
	}
	else
	{
		m_pDlgKeyBoard = new CKeyBoardDlg(m_HotKeyList,m_pKBParent);
	}
	ASSERT(NULL != m_pDlgKeyBoard);

	// ���水����Ϣ,�������������顱�Ի�����
	if ( NULL != pMsg )
	{
		CopyMemory(&m_pDlgKeyBoard->m_Msg, pMsg, sizeof(MSG)); 
	}

	m_pDlgKeyBoard->Create(IDD_KEYBOARD, m_pKBParent);
	m_pDlgKeyBoard->ShowWindow(SW_SHOW);
}

CMPIChildFrame * CMainFrame::OpenChildFrame( const CString &StrXmlCfmName, bool32 bHideNewCfm/*=false*/ )
{
	// ����Ҫ�л���Ʒ��ҳ�� - ��ʱ��Ӧ��
	// ̫��ĵط����Ż��õ��˿ɼ���־�����ﲻ���ڳ�ʼ��ʱ������
#ifdef _DEBUG
	DWORD dwTime = timeGetTime();
#endif

	m_bFromXml = true;	// ��ʼ����xml��״̬

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

	// ҳ�����: ���ǰ���ҳ���ļ��еķ���ֵ����Ҫȷ��ҳ���ļ��ķ���ֵ��һ�µ�
	//	 ��Ҫ�޸�ÿ��ҳ���ļ��о�����ͼ�ķ���ĳ�ʼ����Ʒ��Ϊ, ����ڳ�ʼ��ʱ
	//	 �÷���ӵ��ѡ�����Ʒ����ʹ��ҳ���ļ��е���Ʒ�л�(�ݲ��޸�)
	// ʹ�ô򿪺����viewdata��merchchanged���滻�������Ϊ
	// �ɴ󽫾��Ȱ汾���½����ڣ�Ĭ�Ϸ��鶼����1

	T_CfmFileInfo cfm;
	if ( CCfmManager::Instance().QueryUserCfm(StrXmlCfmName, cfm) )
	{
		if ( _taccess(cfm.m_StrFilePath, 0) != 0 )
		{
			// ����������ļ���ֻ���滻��δ򿪲�����
			ASSERT( 0 );
			T_CfmFileInfo cfmSys;
			if ( cfm.m_bSystemDirHas
				&& CCfmManager::Instance().QuerySysCfm(StrXmlCfmName, cfmSys) )
			{
				cfm.m_StrFilePath = cfmSys.m_StrFilePath;
				ASSERT( _taccess(cfm.m_StrFilePath, 0) == 0 );	// �Ѿ������һ�λ�����
			}
		}
	}

	// ��ȡ���ʱ��ÿ��group��Ӧ��merch
	typedef map<int32, CMerch *>	GroupMerchMap;
	GroupMerchMap groupMerchMap, groupMerchMapMulti, groupMerchMapOther;
	{
		// ��ʱ�����ַ�����ʼ����
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

		// ��ǰ����ڸ��ǳ��˵��Ӵ������������
		CIoViewBase *pIoViewActive = FindActiveIoView();
		if ( NULL != pIoViewActive && pIoViewActive->GetIoViewGroupId() > 0 )
		{
			groupMerchMapMulti[pIoViewActive->GetIoViewGroupId()] = pIoViewActive->GetMerchXml();
		}

		GroupMerchMap::iterator it;
		// �ȴ���multi��
		for (  it=groupMerchMapMulti.begin(); it != groupMerchMapMulti.end() ; ++it )
		{
			if ( groupMerchMap.count(it->first) <= 0 )
			{
				groupMerchMap[it->first] = it->second;
			}
		}

		// �ڴ���ʣ��
		for ( it=groupMerchMapOther.begin(); it != groupMerchMapOther.end() ; ++it )
		{
			if ( groupMerchMap.count(it->first) <= 0 )
			{
				groupMerchMap[it->first] = it->second;
			}
		}
	}

	// ��ʼ��xml
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
		// ʹ��Ĭ��
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

	// ���Ҫ���أ��������������Ƹ���
	//if ( bHideNewCfm )
	{
		// �Ƿ����й���ҳ�涼�����л�������ģʽִ�д򿪲���
		pChildFrame->SetHideMode(true);
	}

	pChildFrame->EnableRequestData(false);	// ��ֹ��xml�м���ʱ������������

	pChildFrame->FromXml(pChildEle);
	m_bFromXml = false;	// �뿪xml��
#ifdef _DEBUG
	TRACE(_T("��ҳ�����xml[%s]: %d ms\r\n"), StrXmlCfmName.operator LPCTSTR(), timeGetTime()-dwTime);
#endif
	pChildFrame->SetActiveGGTongView(GetActiveGGTongViewXml());
	pChildFrame->SetChildFrameTitle(AfxGetApp()->m_pszAppName);

    CCfmManager::Instance().ChangeIoViewStatus(pChildFrame);

	// ����֮�䲻�������Ͳ�����ǰ�������� modify by tangad
	// ����FromXmlǰ���б��ٴ�merchchanged, ��ԭ��ǰ�ķ�����Ʒ
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
		// �򿪵��Ӵ��ھ����������
		// 		if(bMax && !pChildFrame->GetMaxSizeFlag())
		// 		{
		// 			pChildFrame->PostMessage(KMsgChildFrameMaxSize,0,0);
		// 		}
		// �򿪵��Ӵ���ȫ�����
		pChildFrame->ShowWindow(SW_MAXIMIZE);
	}

	//if ( bHideNewCfm )
	{
		pChildFrame->SetHideMode(false);	// �뿪����ģʽ
		PostMessage(UM_CHECK_CFM_HIDE, (WPARAM)pChildFrame->GetSafeHwnd(), NULL);	// �Ժ�����
	}

	// �Ժ���Է��������ˣ�����ʵ��ͨ��IoViewBase��Ӧ�����ʾ����ioview��ͨ��timer����ʼ��һ�η�������
	// ����ʵ�ʵ���һ�ξ������������һ��timer��Ϣ��ʱ�������������ݵ��ط�
	// �᲻�ᵼ�²��������ͣ���������û�����أ�
	pChildFrame->EnableRequestData(true);	// �Ժ���Է�������
	pChildFrame->RecalcLayoutAsync();		// Ҫ�����¼�������

#ifdef _DEBUG
	TRACE(_T("��ҳ��[%s]��: %d ms\r\n"), StrXmlCfmName.operator LPCTSTR(), timeGetTime()-dwTime);
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
			bEnableLock = FALSE;	// ǿ�ƽ�ֹ�ı�lock״̬
		}
		if ( pChildFrame->IsF7AutoLock() )
		{
			bEnableLock = FALSE;	// F7������ı�
		}

		if ( pChildFrame->IsLockedSplit() )
		{
			pCmdUI->SetText(_T("��ʼҳ������"));
		}
		else
		{
			pCmdUI->SetText(_T("����ҳ������"));
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
	// ���������childframe�����ܽ��зָ����
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
	// ���ҷ�ʱ k�� ҳ�棬���������һ��(��ʱ����)������
	// ����Ҳ������趨���Դ������򴴽�һ����ʱ����

	// ����Zֵ����������ȣ���������������ʱ����
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
				return pChild;	// �ҵ��Ѿ����ڵ��ˣ�����
			}
		}
		pWnd = pTmpNext;
	}

	// Ч�ʵ͵��:)
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
		ASSERT( 0 );		// ��֧��4+1��
		return NULL;
	}

	ASSERT( esdc != CCfmManager::ESDC_Count );
	CString StrCfmName;
	if ( !CCfmManager::GetSystemDefaultCfmName(esdc, StrCfmName) )
	{
		ASSERT( 0 );
		return NULL;
	}

	// ���Ƿ��и�����ҳ���
	CMPIChildFrame *pChildFrame = CCfmManager::Instance().GetCfmFrame(StrCfmName);
	if ( NULL != pChildFrame
		|| !bCreateIfNotExist )
	{
		// �Ѿ��� ���� ����Ҫ����
		if(!StrCfmName.IsEmpty())
		{
			CNewCaptionTBWnd::AddPage(StrCfmName);
		}
		return pChildFrame;
	}

	ASSERT( NULL==pChildFrame && bCreateIfNotExist );		// ��Ҫ���������
	if ( (pChildFrame = CCfmManager::Instance().LoadCfm(StrCfmName, false, bHideNewFrame)) != NULL )
	{
		return pChildFrame;
	}

	// ���Ƿ���ڸ�ҳ������ã�������ڣ���֤�������ǲ��ܼ��ػ���Ȩ�޲��������
	// �������ļ������ڵ����
	T_CfmFileInfo cfm;
	if ( CCfmManager::Instance().QueryUserCfm(StrCfmName, cfm) 
		&& cfm.m_bNeedServerAuth
		&& !CPluginFuncRight::Instance().IsUserHasRight(StrCfmName, false) )
	{
		return NULL;	// Ȩ�޲�����û�취
	}

	// ����Ҳʧ���ˣ�������ҳ����������
	// ���԰�����⽨��һ���հ׵�ҳ�棬�����ں��ѿ�
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

	// �ȿ�������ͼ�ǲ��Ƿ���Ҫ���
	// ����ͬ��ͬ��Ŀɼ���(�ȷ�ʱ����K)
	// ��ͬ��ͬ�鲻�ɼ���
	// end

	// ��ע��Ч����
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

	// ˳��:
	//		ͬ��ɼ���ʱ(iGroupId!=-1)
	//		ͬ��ɼ�K��(iGroupId!=-1)
	//		ͬ�鲻�ɼ���ʱ(iGroupId!=-1)
	//		ͬ�鲻�ɼ�K��(iGroupId!=-1)
	//		�ɼ���ʱ
	//		�ɼ�K��
	//		���ɼ���ʱ
	//		���ɼ�k��
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

			// ͬ�����µ�
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

	// ��������Ĳ�����
	return;

	// ���������
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
	// �ȿ�������ͼ�ǲ��Ƿ���Ҫ���
	// ����ͬ��ͬ��Ŀɼ���(�ȷ�ʱ����K)
	// ��ͬ��ͬ�鲻�ɼ���
	// end
	if ( NULL == pChildFrame )
	{
		pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, MDIGetActive());
		if ( NULL == pChildFrame )
		{
			return NULL;	// ��ҳ��
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
	// �Ƿ�����cf�Ǽ���ʱ���Զ������б�
	if ( NULL == pChildFrame )
	{
		return false;
	}

	// ����ϵͳĬ�ϵ�ͼ��ҳ�治������ת�б�
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
		m_aCfmsReturn.Add(pChildFrame->GetSafeHwnd());	// ����¼����ͬ
	}

	return true;
}


void CMainFrame::OnEscBackFrame()
{
	OnBackToUpCfm();
	return;

	CCfmManager::Instance().ReMoveAllOpendCfm();
	// �Ƿ�Ӧ���ر��ּ���ҳ��, �Ա���̫��Ĵ���������Դ
	CMDIChildWnd *pOld = MDIGetActive();
	HWND hWndNow = pOld->GetSafeHwnd();

	// ESC�رգ� ��Ĭ�ϣ���ͼ�����cfm��ҳ��
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
					StrId.Empty();	// ����ҳ�治�ر�
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
		if ( IsWindow(hWnd) && NULL != pWnd && hWnd != hWndNow )	// �뵱ǰҳ����ͬ����ת����ͬ��ҳ��
		{
			pWnd->BringWindowToTop();
			bDone = true;
			break;
		}
	}

	if ( !bDone )
	{
		// Ĭ��ҳ��
		CCfmManager::Instance().LoadCfm(CCfmManager::Instance().GetUserDefaultCfm().m_StrXmlName);
	}

	CMDIChildWnd *pCur = MDIGetActive();
	if ( pCur != pOld
		&& NULL != pOld )
	{
		// ҳ���Ѿ��ᵽ��ǰ������,
		// �����ǰ��ҳ����ͼ��ҳ��, �����ڵ�ҳ���е���ͼ����EscBackFrame�ӿ�
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

	// �ر�esc����ʱ��ҳ�棬�����Ѿ�����̨�Ĵ���
	if ( !StrId.IsEmpty() && NULL != pOld && pOld != pCur )
	{
		CMPIChildFrame *pMPIFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, pOld);
		if ( !pMPIFrame->GetIdString().IsEmpty() )
		{
			CCfmManager::Instance().SaveCfm(pMPIFrame->GetIdString(), pMPIFrame);	// �Զ�����ҳ��
		}
		pOld->PostMessage(WM_CLOSE, 0, 0);
	}
}

void CMainFrame::OnBackToHome()
{
	//CCfmManager::Instance().ReMoveAllOpendCfm();
	// �Ƿ�Ӧ���ر��ּ���ҳ��, �Ա���̫��Ĵ���������Դ
	CGGTongView* pActiveGGTongView  = (CGGTongView*)FindGGTongView(); 

	if ( NULL != pActiveGGTongView && pActiveGGTongView->GetMaxF7Flag() )
	{
		OnProcessF7(pActiveGGTongView);
	}

	CCfmManager::Instance().LoadCfm(CCfmManager::Instance().GetUserDefaultCfm().m_StrXmlName);
	return;

	CMDIChildWnd *pOld = MDIGetActive();
	//	HWND hWndNow = pOld->GetSafeHwnd();

	// �رգ� ��Ĭ�ϣ���ͼ�����cfm��ҳ��
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
					StrId.Empty();	// ����ҳ�治�ر�
				}
			}
			else
			{
				StrId.Empty();
			}
		}
	}

	// Ĭ��ҳ��
	CCfmManager::Instance().LoadCfm(CCfmManager::Instance().GetUserDefaultCfm().m_StrXmlName);

	CMDIChildWnd *pCur = MDIGetActive();
	if ( pCur != pOld
		&& NULL != pOld )
	{
		// ҳ���Ѿ��ᵽ��ǰ������,
		// �����ǰ��ҳ����ͼ��ҳ��, �����ڵ�ҳ���е���ͼ����EscBackFrame�ӿ�
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

	// �ر�esc����ʱ��ҳ�棬�����Ѿ�����̨�Ĵ���
	if ( !StrId.IsEmpty() && NULL != pOld && pOld != pCur )
	{
		CMPIChildFrame *pMPIFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, pOld);
		if ( !pMPIFrame->GetIdString().IsEmpty() )
		{
			CCfmManager::Instance().SaveCfm(pMPIFrame->GetIdString(), pMPIFrame);	// �Զ�����ҳ��
		}
		pOld->PostMessage(WM_CLOSE, 0, 0);
	}

	CIoViewReport *pReport = FindIoViewReport(true);
	// by tangaidong ������ݰ������ã��Ѷ�Ӧ�г����ؽ�ȥ
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
		if(L"��ҳ��Ѷ" == aStrOpenCfm[iSize - 1].StrCfmName)
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
			CCfmManager::Instance().SaveCfm(pMPIFrame->GetIdString(), pMPIFrame);	// �Զ�����ҳ��
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

	// ������Ĭ��ҳ��ż��뵽�����б���
	CString StrDefCfm = CCfmManager::Instance().GetUserDefaultCfm().m_StrXmlName;
	if (StrDefCfm != StrCfm)
	{
		CCfmManager::Instance().ChangeOpenedCfm(StrCfm);
	}
}

void CMainFrame::OnCloseAllWindow()
{
	// fangz 0527,�ر�һ������������ʱ����BUG;
	CWnd *pWnd = CWnd ::FromHandle(m_hWndMDIClient)->GetWindow(GW_CHILD); 

	CMPIChildFrame *pDefaultFrame = CCfmManager::Instance().GetUserDefaultCfmFrame();

	while ( NULL != pWnd )   
	{ 
		CWnd* pTmp		= pWnd;
		CWnd* pTmpNext	= pTmp->GetWindow(GW_HWNDNEXT);

		// Ĭ��ҳ�洰�ڲ��ر�
		if ( pWnd != pDefaultFrame )
		{
			CMPIChildFrame *pMPIFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, pWnd);
			if ( !pMPIFrame->GetIdString().IsEmpty() )
			{
				CCfmManager::Instance().SaveCfm(pMPIFrame->GetIdString(), pMPIFrame);	// �Զ�����ҳ��
			}
			pWnd->SendMessage(WM_CLOSE,0,0);
		}
		pWnd = pTmpNext;
	}	

	// �رչ����� ��Ҫ����ť����
	OnChildFrameMaximized(false);

	// �����F10���ڣ���ʱӦ���ر�
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
		return CCfmManager::Instance().LoadCfm(L"��ʱ����", FALSE) != NULL;	// ��ͨ��
	}
	else if(ID_PIC_KLINE == iPicId)
	{
		return CCfmManager::Instance().LoadCfm(L"K�߷���", FALSE) != NULL;	// ��ͨ��
	}

	return LoadSystemDefaultCfmRetFrame(iPicId, pMerchToChange, bMustSeeChart) != NULL;
}

CMPIChildFrame * CMainFrame::LoadSystemDefaultCfmRetFrame( int32 iPicId, CMerch *pMerchToChange /*= NULL*/, bool32 bMustSeeChart /*= false*/ )
{
	// �ҵ���ǰ������ͼ
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

	CMPIChildFrame *pChildFrame = FindAimIoViewFrame(iPicId, true, true);	// ϵͳ��һ��Ҫ����
	ASSERT( NULL != pChildFrame );
	if ( NULL != pChildFrame )
	{
		CCfmManager::Instance().ChangeIoViewStatus(pChildFrame);

		// �л���Ʒ
		CIoViewBase *pAimIoView = FindIoViewInFrame(iPicId, pChildFrame, false, false);
		if ( NULL != pMerch && pChildFrame->GetGroupID() > 0 )
		{
			if ( pAimIoView != NULL && pAimIoView->GetMerchXml() != pMerch )
			{
				// ��Ŀ�������
				OnViewMerchChanged(pChildFrame->GetGroupID(), pMerch);
			}
			else
			{
				// ��Ŀ��һ���л�
				OnViewMerchChanged(pChildFrame->GetGroupID(), pMerch);
			}
		}

		if ( bMustSeeChart && NULL != pAimIoView )
		{
			pAimIoView->BringToTop();
		}
		else
		{
			// �󼤻�
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
	if(_T("����ѡ��") == StrCfmName)
	{
		CMPIChildFrame *pChildFrame = CCfmManager::Instance().LoadCfm(StrCfmName, false, true);	// ���ܲ�����
		return pChildFrame;
	}

	// �ҵ���ǰ������ͼ
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

	CMPIChildFrame *pChildFrame = CCfmManager::Instance().LoadCfm(StrCfmName, false, true);	// ���ܲ�����
	if ( NULL != pChildFrame )
	{
		// �л���Ʒ
		if ( NULL != pMerch && pChildFrame->GetGroupID() > 0 )
		{
			OnViewMerchChanged(pChildFrame->GetGroupID(), pMerch);
		}

		// �󼤻�
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
		// ����ping ���ٵ�
		return;
	}

	//
	m_bManualReconnect = false;	

	// ������� 1: ���������˳�� 2: ����֤����
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

	// �ص��������������
	pAbsDataManager->StopServiceAfterSortServer();

	//
	for ( int32 i = 0; i < aServerSorted.GetSize(); i++ )
	{
		// ��һ̨�������������֤����		
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

				// ������֤����:
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

	// ȡ��֪ͨ
	COptimizeServer::Instance()->RemoveNotify(this);

	// ��ʱ���ж��Ƿ����ӳɹ�
	SetTimer(KTimerIdCheckConnectServer, KTimerPeriodCheckConnectServer, NULL);
}

bool32 CMainFrame::IsIndexInCurrentKLineFrame( const CString &StrIndexName, int32 iIndexFlag/* = 0*/ )
{
	CMPIChildFrame *pKlineFrame = FindAimIoViewFrame(ID_PIC_KLINE, false);
	if ( NULL == pKlineFrame || MDIGetActive() != pKlineFrame )
	{
		return false;	// ��k��ҳ����ػ��߲��ǵ�ǰҳ��
	}

	CIoViewKLine *pKline = (CIoViewKLine *)FindIoViewInFrame(ID_PIC_KLINE, pKlineFrame, false);
	if ( NULL == pKline )
	{
		ASSERT( 0 );
		return false;	// K��ҳ����k����ͼ�����Ǵ�������
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
			return false;	// ��Ҫ���ָ����Ʒ 
		}
	}
	else if ( iIndexFlag < 0 )
	{
		if ( bMerchIsIndex )
		{
			return false;	// ��Ҫ��ĸ�����Ʒ
		}
	}

	CStringArray aIndexNames;
	pKline->GetCurrentIndexNameArray(aIndexNames);
	for ( int i=0; i < aIndexNames.GetSize() ; i++ )
	{
		if ( aIndexNames[i] == StrIndexName )
		{
			return true;	// �д�ָ��
		}
	}
	return false;	// �����޴�ָ��
}

void CMainFrame::AdjustByOfflineMode()
{
	return;
	if ( !CGGTongApp::m_bOffLine )
	{
		return;
	}

	// ȥ���˵��ϵ� �����ͻ���, ���ӷ�����, ����������, �̺���������
	CMenu * pMenu = this->GetMenu();	
	CNewMenu * pNewMenu = DYNAMIC_DOWNCAST(CNewMenu,pMenu);
	pNewMenu->LoadToolBar(g_awToolBarIconIDs);

	CMenu * pSubMenu = pNewMenu->GetSubMenu(L"ϵͳ(&S)");
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
	// ������
	T_BlockDesc *pDesc = (T_BlockDesc *)pPopupMenu->GetItemData(uCmdId);
	delete pDesc;
	iDelCount++;
	pPopupMenu->SetItemData(uCmdId, NULL);
	}
	else if ( uCmdId >= IDM_BLOCKMENU_BEGIN3 && uCmdId <= IDM_BLOCKMENU_END3 )
	{
	// ��ǰ��ϵͳ��� - Ӧ���������˵�
	ASSERT( 0 );
	}
	else if ( uCmdId >= IDM_BLOCKMENU_BEGIN2 && uCmdId <= IDM_BLOCKMENU_END2 )
	{
	// user block ʵ���Ѿ�������IDM_BLOCKMENU_BEGIN��
	T_BlockDesc *pDesc = (T_BlockDesc *)pPopupMenu->GetItemData(uCmdId);
	delete pDesc;
	iDelCount++;
	pPopupMenu->SetItemData(uCmdId, NULL);
	}
	else if ( uCmdId >= IDM_BLOCKMENU_BEGIN4 && uCmdId <= IDM_BLOCKMENU_END4 )
	{
	// �߼����
	}
	else if ( uCmdId != (UINT)-1 )
	{
	ASSERT( 0 );	// ��Ӧ���������˵�
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
	// �����ʵ�����ʾ TODO
	SetUserConnectCmdFlag(0, EUCCF_ConnectNewsServer);
	// ��Ѷ��������δ����, ֱ������
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
	// �����ʵ�����ʾ TODO
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
		// �������������ã�������������������������ӣ�
		// ���ڸı�����������µ���������Ƚϸ��ӣ�����ʾ��������Ч
		AfxMessageBox(_T("���������������ñ��������������Ч"));
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
	// Ͷ���ռ�
	CDlgNotePad::ShowNotePad(CDlgNotePad::EST_UserNote);
}

void CMainFrame::OnUpdateIoViewMenu( CCmdUI *pCmdUI )
{
	// ������ͼ���õĲ˵������״̬
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
		// �׶����У� �����û�ѡ��Ȼ����ҳ�棬������ҳ���򴴽�һ���µ���ͼ
		// �ֽ��׶����б����ͼ��&��Ѷһ����ҳ�洦��
		ShowPhaseSort(NULL);
		// ���ܴ���û
		return true;
	}
	else if ( ID_PIC_TIMESALE == nPicId)
	{
		// �ֱ���ּ۱���������
		if (!OnShowF7IoViewInChart(nPicId))
		{
			// ʧ�ܵ�ʱ��, ����һ��������	
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
		// ��������ȡ�����ֵ
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
				return false;	// ���߱����õ�Ȩ��
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

			// ��������ҳ����ҪȨ�ޣ����ܴ�
			T_CfmFileInfo cfm;
			if ( NULL==pFrame && CCfmManager::Instance().QueryUserCfm(StrCfmName, cfm) )
			{
				ASSERT( cfm.m_bNeedServerAuth );
				return false;
			}

			if ( NULL == pFrame
				&& picInfo.bCreateIfNotExist )
			{
				// ���Դ�����ҳ����ͼ
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
				// ���ҳ����F7״̬�£���Ҫȡ��F7״̬, ��������쳣��ʾ
				if ( pFrame->IsF7AutoLock() )
				{
					// ��ԭ
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
	// �����µ�ָ�����Ƶ�ҳ�棬��ҳ�����Ʊ��벻����
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

	//OnPictureMenu(iPicIdToFind);	// ��pic�Ƿ񻹿��Ե�����
	CIoViewBase *pIoView = CreateIoViewByPicMenuID(nPicId, true);

	CAbsCenterManager *pAbsCenterManager = NULL;
	if ( NULL != pMerchToChange
		&& NULL != pIoView
		&& NULL != (pAbsCenterManager=pIoView->GetCenterManager())
		&& pIoView->GetMerchXml() != pMerchToChange )
	{
		// �л���Ʒ, �п����д�����ͼ����Ʒ��Ҫ�л�������
		OnViewMerchChanged(pIoView, pMerchToChange);
	}

	// ͨ�����ﴴ����ҳ�涼��͵͵������ҳ�棬Ҫ�Ӵ��б���������
	if ( bCfmSecret 
		&& CCfmManager::Instance().QueryUserCfm(StrCfmName, cfm) )
	{
		cfm.m_bSecret = true;
		CCfmManager::Instance().ModifyUserCfmAttri(cfm);	// �޸ĳ���������
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
	const CString StrAnalysisMenu(_T("����(&F)"));
	// ���Ĳ˵�
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
				// ��û���޸Ŀɼ����֣����Բ���ˢ��
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
	case EMRAVT_None:		// �����κ���
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
		ASSERT( 0 );	// Ӧ��������еĳ��ڵ㣬������ʾ
		::ShowWindow(hWnd, SW_SHOW);
	}

	CMPIChildFrame *pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, CWnd::FromHandlePermanent(hWnd));
	if ( NULL != pFrame
		&& pFrame->IsHideMode() )
	{
		ASSERT( 0 );	// Ӧ���뿪��ģʽ�˵�, �ѵ�������������
	}

	return 1;
}

bool32 CMainFrame::ShowPhaseSort( const T_PhaseOpenBlockParam *pParam /*= NULL*/ )
{
	// �׶����У� �����û�ѡ��Ȼ����ҳ�棬������ҳ���򴴽�һ���µ���ͼ
	// �ֽ��׶����б����ͼ��&��Ѷһ����ҳ�洦��
	T_PhaseOpenBlockParam BlockParam;
	T_PhaseOpenBlockParam BlockParamOld;
	// ���û�в�������ʹ�����ڴ򿪵Ĳ���������ǰ�Ĳ���
	if ( NULL == pParam )
	{
		// ʹ������ҳ��Ĳ���
		CMPIChildFrame *pFrame = FindAimIoViewFrame(ID_PIC_PHASESORT, false);
		CIoViewPhaseSort *pPhaseSort = NULL;
		if ( NULL != pFrame )
		{
			pPhaseSort = (CIoViewPhaseSort *)FindIoViewInFrame(ID_PIC_PHASESORT, pFrame);
			if ( NULL != pPhaseSort )
			{
				pPhaseSort->GetPhaseOpenBlockParam(BlockParamOld);	// ��ʼ��
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
		// �����Ի�������ʱ��
	}
	else if ( ID_FUNC_CONDITIONALARM == nId )
	{

		// ����Ԥ��
		if (  NULL != pDoc->m_pAarmCneter )
		{
			pCmdUI->SetCheck(pDoc->m_pAarmCneter->GetAlarmFlag());
		}

	}
	/*
	else if ( ID_FUNC_MAINMONITOR == nId )
	{
	// �������
	}
	*/
	else if ( ID_FUNC_ADDTOBLOCK == nId )
	{
		// ��ӵ����
	}
	else if ( ID_FUNC_DELFROMBLOCK == nId )
	{
		// �Ӱ��ɾ��
	}
	else if ( ID_AUTOPAGE == nId )
	{
		// �Զ���ҳ
		// ��֧�ֱ��۱�
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
		// �ۺ�����
	}
	/*
	else if ( ID_HangQingCaiWu == nId )
	{
	// ���顢���� F10
	}
	*/
	/*
	else if ( ID_ReMenBanKuai == nId )
	{
	// ���Ű��
	bool32 bCheck = false;
	if ( CCfmManager::Instance().IsCurrentCfm(_T("���Ű��")) )
	{
	bCheck = true;
	}
	pCmdUI->SetCheck(bCheck);
	}
	else if ( ID_DaPanZouShi == nId )
	{
	// ��������
	}
	*/
	else if ( ID_InfoMine == nId )
	{
		// ��Ϣ����
	}
	//else if ( ID_PIC_CFM_MULTIINTERVAL == nId )
	//{
	//	// ������ͬ��
	//	pCmdUI->SetCheck(CCfmManager::Instance().IsCurrentCfm(_T("������ͬ��")));
	//}
	//else if ( ID_PIC_CFM_MULTIMERCH == nId )
	//{
	//	// ���ͬ��
	//	pCmdUI->SetCheck(CCfmManager::Instance().IsCurrentCfm(_T("���Լͬ��")));
	//}
	else if ( ID_TRADEEXE_SETTING == nId )
	{
		// ���׳�������
	}
}

void CMainFrame::OnMainMenu( UINT nId )
{
	CIoViewBase *pActiveIoView = FindActiveIoView();	// ����ΪNULL
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	CGGTongDoc* pDoc = (CGGTongDoc*)pApp->m_pDocument;
	if ( ID_AUTOPAGE_SETTING == nId )
	{
		// �����Ի�������ʱ��
		CString StrInput;
		StrInput.Format(_T("%d"), CIoViewReport::GetAutoPageTimerPeriod());
		if ( CDlgInputShowName::GetInputString(StrInput, _T("�������Զ���ҳʱ��[4-60��]"), _T("�Զ���ҳʱ������")) == IDOK )
		{
			int32 iPeriod = _ttoi(StrInput);
			if ( iPeriod < 4 || iPeriod > 60 )
			{
				MessageBox(_T("�������ʱ�䲻����Ҫ�󣬱�����!"));
			}
			else
			{
				CIoViewReport::SetAutoPageTimerPeriod(iPeriod);
			}
		}
	}
	else if ( ID_FUNC_CONDITIONALARM == nId )
	{
		// ����Ԥ��
		m_wndStatusBar.CreateAlarmDialog();
	}

	else if ( ID_FUNC_MAINMONITOR == nId )
	{
		// �������
		CDlgMainTimeSale::ShowPopupMainTimeSale();
	}

	else if ( ID_FUNC_ADDTOBLOCK == nId )
	{
		// ��ӵ����
		if ( NULL != pActiveIoView
			&& NULL != pActiveIoView->GetMerchXml() )
		{
			pActiveIoView->PostMessage(WM_COMMAND, ID_BLOCK_ADDTO, NULL);
		}
		else
		{
			MessageBox(_T("û��ѡ������Ʒ"), _T("��ʾ"));
		}
	}
	else if ( ID_FUNC_DELFROMBLOCK == nId )
	{
		// �Ӱ��ɾ��
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
				MessageBox(_T("����Ʒ�������κ���ѡ���"), _T("��ʾ"));
			}
		}
		else
		{
			MessageBox(_T("û��ѡ������Ʒ"), _T("��ʾ"));
		}
	}
	else if ( ID_AUTOPAGE == nId )
	{
		// �Զ���ҳ
		// ��֧�ֱ��۱�
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
		// �ۺ�����
		CDlgReportRank::ShowPopupReportRank(CBlockConfig::GetDefaultMarketlClassBlockPseudoId());	// ��֤A��
	}
	/*
	else if ( ID_HangQingCaiWu == nId )
	{
	// ���顢���� F10
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
	// ���Ű��
	ShowHotBlockReport();
	}
	else if ( ID_DaPanZouShi == nId )
	{
	// ��������
	// ������֤��ʱ
	OnShowMerchInChart(0, _T("000001"), ID_PIC_TREND);
	}
	*/
	else if ( ID_InfoMine == nId )
	{
		// ��Ϣ����
		CDlgNotePad::ShowNotePad(CDlgNotePad::EST_InfoMine);
	}
	else if ( ID_PIC_CFM_MULTIINTERVAL == nId )
	{
		// ������ͬ��
		CString StrCfm = _T("������ͬ��");
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
		// ���ͬ��
		m_leftToolBar.ShowDuoGuTongLie();
	}
	else if ( ID_TRADEEXE_SETTING == nId )
	{
		// ���׳�������
	}
	else if(ID_ANALYSIS_CJMX == nId)
	{
		LoadSpecialCfm(L"�ɽ���ϸ");
	}
	else if (ID_ANALYSIS_TLKX == nId)
	{
		LoadSpecialCfm(L"����K��");
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
			if (L"�ƾ�����" == wnd.m_strID)
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
		if ( !CPluginFuncRight::Instance().IsUserHasRight(L"ģ�⽻��", true))
			return ;

		HideAllTradeWnd();
		LoadSpecialCfm(L"ģ�⽻��");
	}
	else if (ID_SPECIAL_TRADE_PLUGIN == nId)
	{

	}
	else if (ID_SPECIAL_LIVE_VIDEO == nId)
	{
		if ( !CPluginFuncRight::Instance().IsUserHasRight(L"��Ƶֱ��", true))
			return ;

		//��Ƶֱ��
		LoadSpecialCfm(L"ֱ������");
	}
	else if(ID_SPECIAL_LIVE_TEXT == nId)
	{
		//����ֱ��
		LoadSpecialCfm(L"����ֱ��");
	}
	else if(ID_SPECIAL_FUNC_ARBITRAGE == nId)
	{
		if ( !CPluginFuncRight::Instance().IsUserHasRight(L"����ģ��", true))
			return ;

		//����ģ��
		OnArbitrage();
	}
}

void CMainFrame::OnArbitrage()
{
	// �ж��Ƿ����Ȩ��
	if ( !CPluginFuncRight::Instance().IsUserHasRight(_T("����"), true) )
	{
		return;
	}

	CDlgArbitrage Dlg;
	// �ӵ�ǰ��ҵ����ͼ���ҿ����ṩ������
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
		// �Զ���ת���������۱�
		OnPicMenuCfm(ID_PIC_REPORT_ARBITRAGE);
	}
}

void CMainFrame::ShowHotBlockReport( const CGmtTime *pTimeStart/*=NULL*/, const CGmtTime *pTimeEnd/*=NULL*/ )
{
	// �������ҳ��Ϳ�����, Ŀǰû���������Ű�������Ҳû����չ������ӿ�
	UNREFERENCED_PARAMETER(pTimeStart);
	UNREFERENCED_PARAMETER(pTimeEnd);
	LoadSpecialCfm(_T("���Ű��"));

	CIoViewBlockReport *pBlockReport = (CIoViewBlockReport *)FindIoViewInFrame(ID_PIC_BLOCKREPORT, NULL, true, true);
	if ( NULL != pBlockReport )
	{
		pBlockReport->SortColumn(2, 1);	// ���������n��
	}
}

void CMainFrame::OnDefaultCfmLoad()
{
	// ������Ĭ��ҳ��󣬳�ʼ��ǰ�ķ�����Ʒ

}

void CMainFrame::InitSpecialMenu()
{
	CMenu * pMenu = this->GetMenu();
	CNewMenu *pNewMenu = DYNAMIC_DOWNCAST(CNewMenu,pMenu);
	ASSERT(NULL != pNewMenu);
	CMenu * pSubMenu = pNewMenu->GetSubMenu(L"��ɫ(&T)");
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

	// ɾ����ɫ�˵�
	DWORD dw = CConfigInfo::Instance()->GetVersionFlag();
	if ( CConfigInfo::FlagCaiFuJinDunFree == dw )
	{
		if ( NULL!=pRootMenu )
		{
			pNewMenu->DeleteMenu((UINT)pRootMenu->GetSafeHmenu(), MF_BYCOMMAND);
		}

		CNewMenu *pViewMenu = (CNewMenu *)pNewMenu->GetSubMenu(_T("�鿴"));
		if ( NULL!=pViewMenu )
		{
			pViewMenu->DeleteMenu(ID_VIEW_TB_SPECIAL, MF_BYCOMMAND);
		}
		// ɾ����ʾ��ɫ��������
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
			ASSERT( 0 );	// �޷��ҵ����ʵ�K����ͼ
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
		// ָ���������л�
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
	// ��Ҫ������վ
}

void CMainFrame::OnArbitrageF5( const CArbitrage &arb2, CIoViewBase *pIoViewSrc/*=NULL*/ )
{
	if ( NULL == pIoViewSrc )
	{
		pIoViewSrc = FindActiveIoView();
	}

	UINT nIdPic = ID_PIC_KLINEARBITRAGE;	// Ĭ������K
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

	UINT nIdPic = ID_PIC_KLINEARBITRAGE;	// Ĭ������K
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
		// û��ָ������ͼ���Ǳ������ϵͳҳ����
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
	// ���������۱�

	// ����ʹ��ָ����frame��û�����Լ���frame
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
		// û��ָ������ͼ���Ǳ������ϵͳҳ����
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
	// �����û�еĻ��߿մ��ڣ�����
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
		if(ETLS_Logined!=m_pSimulateTrade->m_eLoginStatus)//δ��½(Ӧ�ý�����)
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
		if(ETLS_Logined!=m_pSimulateTrade->m_eLoginStatus)//δ��½(Ӧ�ý�����)
		{	
			m_pSimulateTrade->m_pDlgLogin->ShowWindow(SW_HIDE);
			m_pSimulateTrade->m_pDlgLogin->SetFocus();
		}		
	}
	else
	{
		if(ETLS_Logined!=m_pSimulateTrade->m_eLoginStatus)//δ��½
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
			m_pSimulateTrade->m_bWndMaxMinStatus = FALSE;			//���������С���Ļ�Ҳ�������
			m_pSimulateTrade->WndMaxMin();
			m_pSimulateTrade->m_pDlgTradeBidParent->SetDefMaxMin();	//����ָ�����ʾ״̬
			m_pSimulateTrade->TryLogin();
			m_pMyControlBar->SetNewSize(m_pMyControlBar->GetSizeDefault());	//��ʾʱ����Ҫ����Ĭ�ϸ߶�
			//		ShowDockSubWnd(NULL);  // ���ؿ��ܴ�����ʾ���Ӵ���
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
		if(ETLS_Logined!=m_pTradeContainer->m_eLoginStatus)//δ��½(Ӧ�ý�����)
		{
			//	m_pTradeContainer->TryLogin();	
			m_pTradeContainer->m_pDlgLogin->ShowWindow(SW_HIDE);
			m_pTradeContainer->m_pDlgLogin->SetFocus();
		}		
	}
	else
	{
		if(ETLS_Logined!=m_pTradeContainer->m_eLoginStatus)//δ��½
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
			m_pTradeContainer->m_bWndMaxMinStatus = FALSE;	//���������С���Ļ�Ҳ�������
			m_pTradeContainer->WndMaxMin();
			m_pTradeContainer->m_pDlgTradeBidParent->SetDefMaxMin();	//����ָ�����ʾ״̬
			m_pTradeContainer->TryLogin();
			m_pMyControlBar->SetNewSize(m_pMyControlBar->GetSizeDefault());	//��ʾʱ����Ҫ����Ĭ�ϸ߶�
			//	ShowDockSubWnd(NULL);  // ���ؿ��ܴ�����ʾ���Ӵ���
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
		if (L"����" == wnd.m_strID)
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

	// �ο͵�¼��ʱ�����ʾ
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

// js���µ�¼pc�ͻ��˴���
LRESULT	CMainFrame::OnWebReLoginPC(WPARAM wParam, LPARAM lParam)
{
	CString* pStrVal = (CString*)wParam;
	CString StrUserName = _T("");
	if(NULL != pStrVal)
	{
		StrUserName =  *pStrVal;
	}

	// �û�����Ϊ�յ�ʱ�򣬱����û�������
	if(!StrUserName.IsEmpty())
	{

	}

	// �˳�����
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	if(NULL != pApp)
	{
		m_bShowLoginDlg = true;
		PostMessage(WM_CLOSE);
	}

	return 0;
}

// js�������ķ�����Ϣ����
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

// web��������
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


// web�ص�
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





// js��ָ�����洦��
LRESULT	CMainFrame::OnWebOpenCfm(WPARAM wParam, LPARAM lParam)
{
	T_WebOpenCfm *pstParam = (T_WebOpenCfm*)wParam;
	if(NULL == pstParam)
	{
		return 0;
	}

	m_stWebOpenCfm = *pstParam;
	EngineCenterFree(pstParam);

	// �û���Ϊ��¼
	AddToUserDealRecord(m_stWebOpenCfm.StrCfmName, EPTFun);

	if (NULL != m_pNewWndTB)
	{
		if (L"������Ѷ" == m_stWebOpenCfm.StrCfmName)		// ������Ѷ��ش���
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



// js�򿪷�ʱ
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

	// ������Ʒ������ת��ʱҳ��
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


// �����ҳ��������������������
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

	int32 iWidth = _ttoi(pstParam->StrWidth) + 6;		// ���ϱ߿�
	int32 iHeight = _ttoi(pstParam->StrHeight) + 33;	// ���ϱ������ͱ߿�
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

	// �ҵ����е�, �޳����������ظ��������
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


LRESULT CMainFrame::OnMsgViewDataOnNewsTitleUpdate(WPARAM wParam, LPARAM lParam) //�Ƹ���Ѷ2013
{

	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;			
	ASSERT(NULL!= pDocument);				

	//
	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL!= pAbsCenterManager);

	listNewsInfo* tmpNewsInfo = (listNewsInfo*)lParam;
	ASSERT(NULL!= tmpNewsInfo);

	//����״̬����ӵ������
	m_wndCommentSBar.AddBatchComment(*tmpNewsInfo);

	int newsTypeID = (int)wParam;
	if (newsTypeID == pAbsCenterManager->GetJinpingID())
	{
		//��������������
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

	//����״̬����ӵ������
	m_wndCommentSBar.InsertAtComment(*tmpNewsInfo);

	int newsTypeID = (int)wParam;
	if (newsTypeID == pAbsCenterManager->GetJinpingID())
	{
		pAbsCenterManager->SetTodayFlag(HasValidViewTextNews());

		//��������������
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

		// �ο��˺ţ�ûȨ����ʾע��Ի���
		if(pDoc->m_isDefaultUser) 
		{
			EngineCenterFree(pstRightTip);
			ShowRegisterDialog();
			return 0;
		}

		// �ҵ���Ȩ����ʾ��Ϣ
		if (pstRightTip->StrMsgTip.IsEmpty())
		{
			CString StrTitle = L"��ܰ��ʾ";//pstRightTip->StrTitle;
			int32	iWidth = 1024, iHeight = 700;
			CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
			if (NULL != pApp)
			{
				int32 iSize = pApp->m_pConfigInfo->m_aWndSize.GetSize();
				for (int32 i=0; i<iSize; i++)
				{
					CWndSize wnd = pApp->m_pConfigInfo->m_aWndSize[i];
					if (_T("Ȩ����ʾ") == wnd.m_strID)
					{
						iWidth = wnd.m_iWidth;
						iHeight = wnd.m_iHeight;
						break;
					}
				}
			}

			if (!pstRightTip->StrLinkUrl.IsEmpty() && pstRightTip->StrImgUrl.IsEmpty())
			{
				OpenWebWorkspace(L"��ܰ��ʾ", pstRightTip->StrLinkUrl);
			}
			else if (pstRightTip->StrImgUrl.IsEmpty())
			{
				//��Ȩ����ʾʱ���Ѹù��ܵ�����Ҳ����RUL����Ա����� BY HX
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
			// û���ҵ�Ȩ����ʾ��Ϣ
			MessageBox(pstRightTip->StrMsgTip, _T("��ܰ��ʾ"), MB_OK|MB_ICONWARNING);
		}
	}

	EngineCenterFree(pstRightTip);

	return 0;
}

void CMainFrame::AddToUserDealRecord(CString StrFunName, E_ProductType eType)
{
	// �οͻ��߿���Ϊfalse������¼
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

	return TRUE; // ���û������Ҫ��Ȩ�ޣ���Ĭ������Ȩ�޴򿪸�ָ���
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
		//�ָ���ǰ�򿪵���Ļ
		if( !dlg.m_CurrentCfmInfo.m_StrXmlName.IsEmpty())
		{
			OnOpenChildframeFromWsp( dlg.m_CurrentCfmInfo.m_StrXmlName,true);
		}
	}
}

//lxp ����  ��ݷ�ʽ����Ļ
void CMainFrame::OnOpenChildframeFromWsp(CString strXmlName,bool bOpenCur)
{
	ASSERT( !strXmlName.IsEmpty() );
	if( strXmlName.IsEmpty())
		return;

	CMPIChildFrame *pChildFrame = CCfmManager::Instance().GetCfmFrame(strXmlName);
	if ( NULL != pChildFrame && !bOpenCur)
	{
		//CString StrPrompt;
		//StrPrompt.Format(CLanguage::GetInistance().TranslateText(_T("MainFrm_5221_0"),_T("���Ѿ�����ҳ��: %s\r\n�Ƿ����¼��أ�")), strXmlName);
		//if ( AfxMessageBox(StrPrompt, MB_YESNO | MB_ICONQUESTION |MB_DEFBUTTON2 ) == IDNO )
		{
			MDIActivate(pChildFrame);
			return;
		}
	}
	CCfmManager::Instance().LoadCfm(strXmlName, true);	// ���´�
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
		if(pActiveFrame->GetIdString() != L"��ҳ��Ѷ" && m_pNewWndTB)
		{

			bool32 bLoad = m_pNewWndTB->DoOpenCfm(L"��ҳ��Ѷ");
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

///--- ���ÿ�������
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
			MessageBox(L"ϵͳ��������,������ϵͳ����",L"��ʾ");     
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
	// �õ�������
	CString StrBlockName = CUserBlockManager::Instance()->GetDefaultServerBlockName();
	T_Block* pBlock	= CUserBlockManager::Instance()->GetBlock(StrBlockName);
	if ( NULL == pBlock )
	{
		return;
	}

	// �õ���������е���Ʒ
	CArray<CMerch*, CMerch*> aMerchs;
	if ( !CUserBlockManager::Instance()->GetMerchsInBlock(StrBlockName, aMerchs) )
	{
		return;
	}

	/*if (aMerchs.GetSize() < 1)
	{
		return	
	}*/

	// ���json�ַ���
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
	// ��ȡ��ѡ�ɹ�����
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
	// fangz0130 ��̩��ķ���ʦ
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
		// �������汾���ⲿ���ף�����������µ�
		return; 
	}

	if (!pApp->m_pConfigInfo->m_bQuickTrade || !pApp->m_pConfigInfo->m_bMember)
	{
		// �������汾���ⲿ���ף�����������µ�
		return; 
	}

	// ...fangz 1227 �����µ�
	if(NULL==m_pTradeContainer)
	{
		return;
	}

	//���ڹ��������,���ؿ����µ�����
	if(3==iFlag)
	{
		m_pTradeContainer->m_pDlgTradeBidParent->HideQuickOrder();
		return;
	}

	// ģ�⽻�ײ���������µ�
	bool32 bFlag1 = (ETLS_Logined==m_pSimulateTrade->m_eLoginStatus)&&(ETLS_Logined!=m_pTradeContainer->m_eLoginStatus);
	bool32 bFlag2 = (ETLS_Logined!=m_pSimulateTrade->m_eLoginStatus)&&(ETLS_Logined!=m_pTradeContainer->m_eLoginStatus);
	if(bFlag1 || bFlag2)//δ��¼
	{
		if(2==iFlag)
		{
			//	MessageBox(_T("������δ��¼!"),_T("����"),MB_OK|MB_ICONERROR);
			m_bQuickOrderDn = true;
			OnFirmTradeShow();
		}
		return;
	}
	//  ÿ����ʾ�����µ�ǰ�����ļ��ж�ȡcheckbox��״̬
	m_pTradeContainer->m_pDlgTradeBidParent->GetTradeQuickOrderDlg().ReadCheck();

	if (!m_pTradeContainer->m_pDlgTradeBidParent->GetTradeQuickOrderDlg().SetCommID(m_pTradeContainer->ClientMerch2TradeCode(pMerch)))
	{
		if ((0==iFlag) && !m_pTradeContainer->m_pDlgTradeBidParent->IsVisibleQuickOrder())
		{
			return;
		}
		CString strErr = _T("��ǰ��Ʒ���������ף���Ʒ���ƣ�");
		//strErr.Format(_T("%s"),pMerch->m_MerchInfo.m_StrMerchCnName);
		//strErr += _T(" ���ǽ�����Ʒ");
		CDlgTip dlg;
		//dlg.m_strTipMsg = strErr;
		//dlg.m_eTipType = ETT_WAR;
		//dlg.m_pCenterWnd = this;	// ����Ҫ���е��Ĵ���ָ��
		//dlg.DoModal();
		//m_pTradeContainer->m_pDlgTradeBidParent->m_DlgQuickOrder.m_btnGoodsName.SetWindowText(strErr);

		m_pTradeContainer->m_pDlgTradeBidParent->m_DlgQuickOrder.m_btnGoodsName.SetTitleText1(pMerch->m_MerchInfo.m_StrMerchCnName);
		m_pTradeContainer->m_pDlgTradeBidParent->m_DlgQuickOrder.m_btnGoodsName.SetTitleText2(_T(" ���ǽ�����Ʒ"));

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
	//����
	if(0==iFlag)//����
	{
		//m_pTradeContainer->m_pDlgTradeBidParent->GetTradeQuickOrderDlg().InitControl();
		m_pTradeContainer->m_pDlgTradeBidParent->GetTradeQuickOrderDlg().SetPrice();
	}
	else if(1==iFlag)//˫��
	{
		//�ж���������
		if(CReportScheme::ERHBuyPrice == eHeader)
		{
			m_pTradeContainer->m_pDlgTradeBidParent->TradeOpen(m_pTradeContainer->ClientMerch2TradeCode(pMerch),true);
		}
		else if(CReportScheme::ERHSellPrice == eHeader)
		{
			m_pTradeContainer->m_pDlgTradeBidParent->TradeOpen(m_pTradeContainer->ClientMerch2TradeCode(pMerch),false);
		}		
	}
	else if(2==iFlag)//��ť
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

	//����˺���Ϣ���˺Ż��ߵ�¼��ע��
	if (pApp->m_pConfigInfo->m_bMember && (EType_Hide!=pApp->m_pConfigInfo->m_btnUserInfo.m_eShowType))
	{
		AddTitleMenuButton(rcControl, m_pImageUserAccount, 1, ID_MENU_USER_NAME, L"");
	}

	AddTitleMenuButton(rcControl, m_pImgMenuSystem, 3, ID_MENU_SYSTEM,L"ϵͳ");
	AddTitleMenuButton(rcControl, m_pImgMenuAlarm, 3, ID_MENU_FORMULA_MANAGE,L"��ʽ����");
	AddTitleMenuButton(rcControl, m_pImgMenuAlarm, 3, ID_MENU_ALARM,L"����Ԥ��");
	AddTitleMenuButton(rcControl, m_pImgMenuSelStock, 3, ID_MENU_SEL_STOCK,L"ѡ��");
    AddTitleMenuButton(rcControl, m_pImgMenuTrade, 3, ID_MENU_TRADE,L"����");
	AddTitleMenuButton(rcControl, m_pImgMenuHelp, 3, ID_MENU_HELP,L"����");
	AddTitleMenuButton(rcControl, m_pImageLogin, 3, ID_MENU_LOGIN, L"��¼");
	AddTitleMenuButton(rcControl, m_pImageLogin, 3, ID_MENU_REGISTER, L"ע��");
	AddTitleMenuButton(rcControl, m_pImgMenuExit, 3, ID_MENU_EXIT, L"�˳�");
	//CNCButton* pIMButton = AddTitleMenuButton(rcControl, m_pImgMenuBtn1, 3, ID_MENU_ONLINE_SERVICE, L"���߿ͷ�");

	// ��ӱ������ϵ�ϵͳ��ť
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

	// ���������������а�ť
	for (iter=m_mapMenuBtn.begin(); iter!=m_mapMenuBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;

		// ��point�Ƿ��ڰ�ť������
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

	// ���������������а�ť
	for (iter=m_mapSysBtn.begin(); iter!=m_mapSysBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;

		// ��point�Ƿ��ڰ�ť������
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
	//	stFont.m_StrName = fontFactory.GetExistFontName(L"΢���ź�");	//...
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

// �˵����ϰ�ť����Ϣ��Ӧ
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

			// �������õ����ͣ���ͬ��ʽչʾ
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
			// ��ʾ/���ع�����
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
			// �Ͽ�����
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
			// �Ͽ���Ѷ
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
					// ����Ԥ��
					m_wndStatusBar.CreateAlarmDialog();
				}
				else
				{
					T_MenuItemData tmpData;
					tmpData.bShowArrow = false;
					tmpData.strText = StrText;
					if(iID == ID_MENU_SYSTEM)
					{
						StrText = L"����";
						eType = EMIT_FIR_SYSTEM;
						tmpData.ID = 0;
					}
					else if(iID == ID_MENU_SEL_STOCK)
					{
						StrText = L"ѡ��";
						eType = EMIT_FIR_SEL_STOCK;
						tmpData.ID = 1;
					}
					else if(iID == ID_MENU_TRADE)
					{
						StrText = L"����";
						eType = EMIT_FIR_TRADE;
						tmpData.ID = 2;
					}
					else if(iID == ID_MENU_HELP)
					{
						StrText = L"����";
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

//�´��Զ���¼
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
					AfxMessageBox(L"��ǰ�޿��÷����!�˴�������ͼ�޷���",MB_ICONWARNING);
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
	// ��ʼ�л���Ʒ, ֪ͨ��ͼ���¿ؼ�, ��̬����. 
	if (NULL == pIoViewSrc || NULL == pMerch)
	{
		return;
	}

	// ��ӵ��û���Ϊ��¼
	{
		CString StrFunName = pMerch->m_MerchInfo.m_StrMerchCnName;
		StrFunName += L"-";
		StrFunName += pMerch->m_MerchInfo.m_StrMerchCode;

		AddToUserDealRecord(StrFunName, EPTCode);
	}

	int32 iIoViewGroupId = pIoViewSrc->GetIoViewGroupId();
	if (iIoViewGroupId <= 0)	// ����ҵ����ͼ�� ���ı�һ����Ʒ
	{
		pIoViewSrc->OnVDataMerchChanged(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, pMerch);
	}
	else	// �����й���ҵ����ͼ�� ȫ��������Ʒ�� ���ҽ�������ͼ�ɼ�ʱ�� ֪ͨ���������
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

	// �����µ�
	OnQuickTrade(0, pMerch, CReportScheme::ERHCount);
	KeepRecentMerch(pMerch);		

	CIoViewBase::AddLastCurrentMerch(pMerch);	// ��¼��ǰ����Ʒ

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
	// �л����, ֪ͨ��ͼ��ʼ��������
	if (NULL == pIoViewSrc || NULL == pMerch)
	{
		return;
	}

	//
	int32 iIoViewGroupId = pIoViewSrc->GetIoViewGroupId();

	if ( iIoViewGroupId <= 0 )	// ����ҵ����ͼ�� ���ı�һ����Ʒ
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
	else	// �����й���ҵ����ͼ�� ȫ��������Ʒ�� ���ҽ�������ͼ�ɼ�ʱ�� ֪ͨ���������
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
		return NULL;	// ������ͼ�Ĳ�����
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
				return pMerch;// ���ʵ���ϼ�����ͼ���п��ܲ�ͬ��
			}
		}
	}

	for ( i=0; i < m_IoViewsPtr.GetSize() ; i++ )
	{
		CIoViewBase *pIoView = m_IoViewsPtr[i];
		int32 iViewGroupID = pIoView->GetIoViewGroupId();
		if (  (iViewGroupID & iGroupId)/*pIoView->GetIoViewGroupId() == iGroupId*/ )
		{
			return pIoView->GetMerchXml();	// û������ͼ����û�취�ˣ���㷵��һ����ϵҲ����
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

	if (iIoViewGroupId <= 0)	// ����ҵ����ͼ�� ��֪ͨ�Լ�
	{
		bool32 bAddSuccess = false;

		bAddSuccess = pIoViewSrc->OnVDataAddCompareMerch(pMerch);

		if (bAddSuccess)
		{
			m_aCompareMerchList.Add(CompareMerchInfo);			
		}
		return bAddSuccess;
	}
	else	// �����й���ҵ����ͼ�� ȫ��������Ʒ
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
	if (iIoViewGroupId <= 0)	// ����ҵ����ͼ�� ��֪ͨ�Լ�
	{
		pIoViewSrc->OnVDataRemoveCompareMerch(pMerch);	
	}
	else	// �����й���ҵ����ͼ�� ȫ��������Ʒ
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
	if (iIoViewGroupId <= 0)	// ����ҵ����ͼ�� ��֪ͨ�Լ�
	{
		pIoViewSrc->OnVDataClearCompareMerchs();

	}
	else	// �����й���ҵ����ͼ�� ȫ��������Ʒ
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

	// ��ʼ��ƴ��ģ��
	InitPy();

	// ������ݼ��б�
	int32 iShortCutCount = GetShortCutObjectCount();
	for (int32 i = 0; i < iShortCutCount; i++)
	{
		const T_ShortCutObject *pstShortCutObject = GetShortCutObject(i);

		// ���һ��
		CHotKey HotKey;	
		HotKey.m_eHotKeyType	= EHKTShortCut;
		HotKey.m_StrKey			= pstShortCutObject->m_StrKey;
		HotKey.m_StrSummary		= pstShortCutObject->m_StrSummary;
		HotKey.m_StrParam1		= pstShortCutObject->m_StrKey;
		m_HotKeyList.Add(HotKey);
	}

	//��������ݼ�
	CBlockConfig::Instance()->BuildHotKeyList(m_HotKeyList);

	//�����Զ������ݼ�
	CUserBlockManager::Instance()->BuildUserBlockHotKeyList(m_HotKeyList);

	//������Ʒ��ݼ�
	CGGTongDoc* pDoc = AfxGetDocument();
	if (pDoc->m_pAbsCenterManager)
	{
		pDoc->m_pAbsCenterManager->BuildMerchHotKeyList(m_HotKeyList);
	}
	
	//����ָ���ݼ�
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
			//if (strCaption == pGuiTopTabWnd->GetTabsTitle(i))  // �Ѿ�����
			if(strUrl == pGuiTopTabWnd->GetTabsValue(i))	// ������ͬ����ʾ�Ѿ�����
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
	if (1 == wParam)	// �ر�web�Ի���
	{
		if (NULL != m_pDlgComment)
		{
			m_pDlgComment->EndDialog(IDOK);
		}
	}
	else				// �˳�����
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
		HMENU hmenuPopup = (HMENU)wParam;   // �Ӳ˵����

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
	// �����˵�
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
		// ������������ҳ��
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

//��ҽ���
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
				_sntprintf(szErr, 100, _T("ִ�н��׳���ʧ��(%u)��"), GetLastError());
				MessageBox(szErr, _T("����"));
				SetTradeExePath(NULL);	// ������������Ľ�������
			}
		}
		else
		{
			MessageBox(_T("���׳����Ѳ����ڣ�"), _T("����"));
			SetTradeExePath(NULL);	// ������������Ľ�������
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
	// �����������������յ��������Ϳ�����
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
		stMsg.m_StrMsgType = L"Ԥ����Ϣ";
	}
	else if(en_system_msg == iMsgId)
	{
		stMsg.m_StrMsgType = L"ϵͳ��Ϣ";
	}
	else if (en_optional_change == iMsgId)
	{
		stMsg.m_StrMsgType = L"��ѡ�춯";
	}
	else if (en_strategy_trade == iMsgId)
	{
		stMsg.m_StrMsgType = L"������Ϣ";
		if (m_pDlgPushMsg)
		{
			m_pDlgPushMsg->SetMsgTypeString(stMsg.m_StrMsgType);
		}
	}
	else if (en_select_stock == iMsgId)
	{
		stMsg.m_StrMsgType = L"����ѡ��";
	}
	else
	{
		stMsg.m_StrMsgType = L"δ֪��Ѷ������Ϣ";
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

	// �����Ϣ����Ϊ�գ���ô�Ͳ�������ʾ��
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

//��������ҳ��
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
		if (L"��������" == wnd.m_strID)
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
			if (L"�û���ϸҳ��" == wnd.m_strID)
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
		if(pActiveFrame->GetIdString() != L"��ҳ��Ѷ")
		{
			bool32 bLoadcfm = CCfmManager::Instance().LoadCfm(L"��ҳ��Ѷ", false, false, StrUrl)!=NULL;
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
			CCfmManager::Instance().AddOpenedCfm(L"��ҳ��Ѷ", StrUrl);
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
	CString StrName = _T("�û���ϸҳ��");

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

	if(!CPluginFuncRight::Instance().IsUserHasRight(L"IM�ͷ�", true, true))
	{
		return;
	}

	CString StrUrl = L""; 
	CString StrName = _T("���߿ͷ�");
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
		StrName = L"�û�ע��";
	}

	bool32 bShowIE = FALSE;
	int32 iWidth = 1024, iHeight = 750;
	int32 iSize = pApp->m_pConfigInfo->m_aWndSize.GetSize();
	for (int32 i=0; i<iSize; i++)
	{
		CWndSize wnd = pApp->m_pConfigInfo->m_aWndSize[i];
		if (L"ע��" == wnd.m_strID)
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
		_MYTRACE(L"����˳��:");
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
			StrCefName = L"��ʦս��";
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
			if(pActiveFrame->GetIdString() != L"��ҳ��Ѷ")
			{
				bool32 bLoad = m_pNewWndTB->DoOpenCfm(L"��ҳ��Ѷ", StrWebUrl);
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
		//m_pNewWndTB->DoOpenCfm(L"��֤����");
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
		pPopMenu->RemoveMenu(0, MF_BYPOSITION);		// �����ǰ�Ĳ˵���Ŀ
	}

	pPopMenu->AppendODMenu(L"��������", MF_STRING, ID_DOWNLOAD);
	pPopMenu->AppendODMenu(L"��������", MF_STRING, ID_UPDATE);
	if(pDoc->m_bAutoRun)
		pPopMenu->AppendODMenu(L"�رտ�������", MF_STRING, ID_SYS_AUTO_RUN);
	else
		pPopMenu->AppendODMenu(L"��������", MF_STRING, ID_SYS_AUTO_RUN);

	if (pDoc->m_bAutoLogin)
	{
		pPopMenu->AppendODMenu(L"�ֶ���¼", MF_STRING, ID_MENU_AUTOCONNECT);
	}
	else
	{
		pPopMenu->AppendODMenu(L"�Զ���¼", MF_STRING, ID_MENU_AUTOCONNECT);
	}

	pPopMenu->AppendODMenu(L"������", MF_STRING, MSG_SCTOOLBAR_MENU);

	bool32 bConnectedData = pMain->m_wndStatusBar.m_bConnect;
	if(bConnectedData)
	{
		pPopMenu->AppendODMenu( L"�Ͽ�����", MF_STRING, ID_DISCONNECT_SERVER);
	}
	else
	{
		pPopMenu->AppendODMenu(L"��������", MF_STRING, ID_CONNECT_SERVER);
	}

	bool32 bConnectedNews = pMain->m_wndStatusBar.m_bConnectNews;
	if(bConnectedNews)
	{
		pPopMenu->AppendODMenu( L"�Ͽ���Ѷ", MF_STRING, ID_DISCONNECT_NEWSSERVER);
	}
	else
	{
		pPopMenu->AppendODMenu(L"������Ѷ", MF_STRING, ID_DISCONNECT_NEWSSERVER);
	}


	CNewMenu *childMenu = pPopMenu->AppendODPopupMenu(L"ģ������");
	childMenu->AppendODMenu(L"�½�����ҳ��", MF_STRING, ID_NEW_CHILDFRAME);
	childMenu->AppendODMenu(L"�򿪹���ҳ��", MF_STRING, ID_OPEN_CHILDFRAME);	
	childMenu->AppendODMenu(L"���湤��ҳ��", MF_STRING, ID_SAVE_CUR_CHILDFRAME);	
	childMenu->AppendODMenu(L"��湤��ҳ��", MF_STRING, ID_SAVEAS_CHILDFRAME);	
	childMenu->AppendODMenu( L"����ҳ������", MF_STRING, ID_LOCK_CUR_CHILDFRAME);	
	childMenu->AppendODMenu( L"���沼�ֵ���", MF_STRING, ID_LAYOUT_ADJUST);	
	pPopMenu->AppendODMenu(L"��Ŀ����", MF_STRING, ID_GRIDHEAD_SET);


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
				_sntprintf(szErr, 100, _T("ִ�н��׳���ʧ��(%u)��"), GetLastError());
				MessageBox(szErr, _T("����"));
				SetTradeExePath(NULL);	// ������������Ľ�������
			}
		}
		else
		{
			MessageBox(_T("���׳����Ѳ����ڣ�"), _T("����"));
			SetTradeExePath(NULL);	// ������������Ľ�������
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
		pPopMenu->RemoveMenu(0, MF_BYPOSITION);		// �����ǰ�Ĳ˵���Ŀ
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
		pPopMenu->RemoveMenu(0, MF_BYPOSITION);		// �����ǰ�Ĳ˵���Ŀ
	}

	if( pDoc )
	{
		if(!pDoc->m_bShowSCToolBar)
		{
			pPopMenu->AppendODMenu(L"��ʾ������", MF_STRING, ID_SHOW_SHORTCUT_BAR);
		}
		else
		{
			pPopMenu->AppendODMenu(L"���ع�����", MF_STRING, ID_SHOW_SHORTCUT_BAR);
		}
	}
	pPopMenu->AppendODMenu(L"��ʽ����", MF_STRING, ID_FORMULA_MAG);
	CNewMenu *childMenu = pPopMenu->AppendODPopupMenu(L"ѡ����");
	childMenu->AppendODMenu(L"����ѡ����", MF_STRING, ID_SELECT_STOCK);
	childMenu->AppendODMenu(L"����ѡ����", MF_STRING, ID_CUSTOM_STOCK);	

	pPopMenu->AppendODMenu(L"����Ԥ��", MF_STRING, ID_FUNC_CONDITIONALARM);
	pPopMenu->AppendODMenu(L"�׶�����", MF_STRING, IDC_INTERVAL_SORT);
	pPopMenu->AppendODMenu(L"����ͼ", MF_STRING, ID_TREND_SHOWTICK);
	pPopMenu->AppendODMenu(L"����", MF_STRING, ID_SPECIAL_FUNC_ARBITRAGE);
	pPopMenu->AppendODMenu(L"���ͬ��", MF_STRING, ID_PIC_CFM_MULTIMERCH);
	pPopMenu->AppendODMenu(L"������ͬ��", MF_STRING, ID_PIC_CFM_MULTIINTERVAL);


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
//		// ����ѡ��������
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
//		// ����ѡ��������
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
//	// ����ѡ��ģ������
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
//		// ȡtoken����֯��ͷ
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
//		// ����֮ǰ��յ���������
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
//	// ����ѡ��ģ������
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
	//	��ʼ��¼
	bool32	bLoginSuc = pDoc->Login();	
	pDoc->m_pAbsCenterManager->SetFrameHwnd(GetSafeHwnd());		//	��¼��������Ϣ���帴λ	
	m_bLoginSucc = bLoginSuc;
	if(!bLoginSuc)
	{			
		if (bCloseExit)
		{	
			//	�˳���
			PostMessage(WM_CLOSE);
			return;
		}
	}
	else
	{
		//	��¼�ɹ������ʼ������
		Init();														//	��ʼ��
		pDoc->m_pAbsCenterManager->ForceUpdateVisibleAttentIoView();		//	ǿ��ˢ�¹�ע��Ʒ		
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


	//	����û�ͷ��
	if (NULL != m_pImgUserPhoto)
	{
		DEL(m_pImgUserPhoto);
	}

	//	��������ԭΪĬ��״̬����ʾע�ᣬ��¼�������û���ť����ť
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

		//	����������
		DrawCustomNcClient();
	}	

	//	��Ϣ���ķ���ʼ��
	if(pDoc && pDoc->m_pHttpClient)
	{
		//��Ϣ���ĺ�̨�ͷţ��Ͽ���Ϣ�������̣߳������̣߳��շ��̣߳�
		DEL(pDoc->m_pHttpClient);
	}


	//	Ŀǰ���������� �ر�״̬���е�ʵʱ�����������������ע��Ʒ
	m_wndStatusBar.UnInit();

	//Ȩ�����
	CPluginFuncRight::Instance().Clear();

	//	����û���Ϊ��¼
	m_vUserDealRecord.clear();

	//	viewdata����(�Ͽ�ͨѶ��·�����г����������ã���ʱ�����ã���¼��Ϣ���ã�...)
	pDoc->m_pAbsCenterManager->ReSetLogData();

	KillTimer(KTimerIdChooseStockState);	//	ɾ��ѡ��״̬��ʱ��
	KillTimer(KTimerPickModelStatus);		//	ɾ������ѡ�ɶ�ʱ��

	//	�˳�ʱ��ʾ��¼�Ի���
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



	// ����ѡ�ɶ�ʱ��
	KillTimer(KTimerPickModelStatus);

	if (1)
	{
		SetTimer(KTimerPickModelStatus, KTimerPickModelStatusTime, NULL);	//	��ʱ��������ѡ��״̬
	}
	

	SetTimer(KTimerInfoCenter, KTimerIntervalTime * 60 * 1000, NULL);
	SetTimer(KTimerIdRefreshToken, KTimerPeriodRefreshToken * 60 * 1000, NULL);
	// 1ms����һ��ѡ��״̬
	//SetTimer(KTimerIdChooseStockState, KTimerPeriodChooseStockState, NULL);

	if(!pDoc->m_isDefaultUser && pApp->m_pConfigInfo->m_stUserDealRecord.bRecord) 
	{
		SetTimer(KTimerIdUserDealRecord, KTimerPeriodUserDealRecord * 60 * 1000, NULL);
	}

	pDoc->m_pAbsCenterManager->SetFrameHwnd(GetSafeHwnd());
	//	������������ǳ�
	pDoc->m_pAbsCenterManager->GetServiceDispose()->GetUserInfo(m_stUserInfo);
	//	������ѻ�����ֱ���õ�¼�˻���ֵ�ǳ�
	if (pDoc->m_pAbsCenterManager->IsOffLineLogin())
	{
		_tcscpy_s(m_stUserInfo.wszUserName, sizeof(m_stUserInfo.wszUserName)/sizeof(m_stUserInfo.wszUserName[0]), pDoc->m_pAbsCenterManager->GetUserName().GetBuffer());
		_tcscpy_s(m_stUserInfo.wszNickName, sizeof(m_stUserInfo.wszNickName)/sizeof(m_stUserInfo.wszUserName[0]), pDoc->m_pAbsCenterManager->GetUserName().GetBuffer());	
	}
	// ����˽��Ŀ¼
	CString StrPrivatePath;
	StrPrivatePath = CPathFactory::GetPrivateConfigPath(pDoc->m_pAbsCenterManager->GetUserName());
	_tcheck_if_mkdir(StrPrivatePath.GetBuffer(MAX_PATH));
	StrPrivatePath.ReleaseBuffer();
	StrPrivatePath = CPathFactory::GetPrivateWorkspacePath(pDoc->m_pAbsCenterManager->GetUserName());
	_tcheck_if_mkdir(StrPrivatePath.GetBuffer(MAX_PATH));
	StrPrivatePath.ReleaseBuffer();


	//	��ȡ�û��б�ͷ������
	pDoc->PostInit();
	//	��ȡ�û�����
	CReportScheme::Instance()->SetPrivateFolderName(pDoc->m_pAbsCenterManager->GetUserName());

	// ��ʼ���û�����ҳ����Ϣ	
	CCfmManager::Instance().Initialize(pDoc->m_pAbsCenterManager->GetUserName());

	T_UserInfo stUserInfo;
	pDoc->m_pAbsCenterManager->GetServiceDispose()->GetUserInfo(stUserInfo);
	CPluginFuncRight::Instance().SetShowTipWnd(GetSafeHwnd(), stUserInfo.iGroupType);


	//	�ǳƴ���4���ַ���������ʾ
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

	//	���ز���ʾ�û�ͷ��
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
		//	�ڷǲ����˻�����£�����ʾ��¼��ע�ᰴť
		//	�û���ť�����˻��ǳ�
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
		//	�����˻�����������û���ť
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
	//	���»����������������(��ʱˢ�¸�������)
	DrawCustomNcClient();



	//	������״̬����Ҫ���������ע��Ʒ
	m_wndStatusBar.InitStatusEx();
	//	

	if(pDoc && (NULL==pDoc->m_pHttpClient) && pDoc->GetReqMessageInfoStatus())
	{
		pDoc->m_pHttpClient = new http_client::CHttpClient;
		// ������Ϣ������Ӧ�Ļص�����
		pDoc->m_pHttpClient->SetCallHttpData(RecvMsgCenterData);		//	��Ϣ������Ϣ���մ���
		pDoc->m_pHttpClient->SetCallHttpConn((http_client::CALL_HTTP_CONN_NOTIFY)OnConnNotify);		//	��Ϣ�������Ӵ���		//	��Ϣ�������Ӵ���
		pDoc->m_pHttpClient->Construct();
	}

	if(pDoc && pDoc->m_pHttpClient)
	{
		// ��token��userid���浽��Ϣ���Ķ�̬����
		std::string strToken = pDoc->m_pAutherManager->GetInterface()->GetServiceDispose()->GetToken();

		char szUserID[32]={0};
		sprintf(szUserID, "%d", m_stUserInfo.iUserId);
		pDoc->m_pHttpClient->SetTokenAndUserID(strToken, szUserID);

		// ������Ϣ���ķ�����
		CString strAddress = pDoc->GetInfoCenterIP();
		int nPort = pDoc->GetInfoCenterPort();
		pDoc->m_pHttpClient->ConnectServer(strAddress, nPort);
	}

	//	�����µ�Ĭ���ӿ�ܣ�ɾ�����˺ŵĴ��ӿ��
	if (NULL != m_pNewCaptionWndTB)
	{
		//	ɾ����ʷ��ǩ����
		m_pNewCaptionWndTB->CloseAllPage();	
		//	�������¼��ذ��棬���´�merchmanager�л�ȡ����
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
	//	��һ�ε�¼
	if (bOnlyInit)
	{
		ShowNotifyMsg();
		//
		SendMessage(UM_ViewData_OnAllMarketInitializeSuccess, 0, 0);

		//	���ÿ��Ĭ�ϲ˵�
		SetMenu(NULL);

		//// �����¼��������ͼƬ
		CString StrLoginCode = CConfigInfo::Instance()->m_StrLoginLeftCode;
		CString StrLoginImg = CConfigInfo::Instance()->GetAdvsCenterUrlByCode(StrLoginCode, EAUTPit);
		//	���ص�¼��������ͼ
		if (!StrLoginImg.IsEmpty())
		{
			WebClient webClient;
			webClient.SetTimeouts(100, 200);
			webClient.DownloadFile(StrLoginImg,  L"image\\banner.png");
		}

		//	��ʾ��ӭ
		ShowWelcome();

		bOnlyInit = false;
	}

	//	���ö�ʱ��(����ʵʱ���ͣ�����ʱ�䣬�Զ�����ڴ�,...)
	pDoc->m_pAbsCenterManager->InitPush(true);

	m_bDoLogin = true;			//	��ʾ�Ƿ��¼��
	m_bShowLoginDlg = false;	//	�˳�ʱ��ʾ��¼�Ի���	

	//	ָ���ʼ��
	{
		SetAbsCenterManager(pDoc->m_pAbsCenterManager);
		//SetUserName(pDoc->m_pAbsCenterManager->GetUserName());	//	�л��˺ź���Ҫ����ָ��ȫ���е��û���
		CFormulaLib::instance()->Reload();
	}

	/////////////////////////////////////////////////////
	//===================CODE CLEAN======================
	//////////////////////////////////////////////////////
	//// ѡ��ģ�Ͳ�ѯ
	//if (1)
	//{
	//	QueryPickModelTypeInfo();
	//	QueryPickModelTypeStatus();
	//}
	
	// �����¹�����
	CNewStockManager::Instance().RequestNewStockData();



	// ��¼ʱͬ��һ�ι�����״̬
	// ���ò����״̬
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

		// ����Ӳ˵��Ѿ���������ôֻ��Ҫ�����Ӳ˵���λ�ü���
		if (!bNeedCreateSubMenu)
		{
			CWnd *pSubMenu = AdjustSubMenuPos(btnNow.GetCaption(), (rect.left+iWidth + 1), i * iHeight + rect.bottom);
			if (pSubMenu)
			{
				btnNow.AddChildMenu(pSubMenu);
			}
		}
		else// �����Ӳ˵�
		{
			E_MenuItemType eChlidtype = (E_MenuItemType)(FindFirstVecIDByMenuID(tempData.ID));
			CWnd *pSubMenu = CreateSubMenu(tempData, eChlidtype, (rect.left + iWidth + 1), i * iHeight + rect.bottom);
			if (pSubMenu)
			{
				btnNow.AddChildMenu(pSubMenu);
			}
		}

		// �����ܲ˵�������������վ���Ͽ�������վ��������Ѷ��վ���Ͽ���ѡ��վ���Զ���¼�Ȱ�ť��״̬
		if(eType == EMIT_FIR_SYSTEM)
		{
			if(ID_DISCONNECT_SERVER == tempData.ID)
			{
				if (m_wndStatusBar.m_bConnect)
				{	
					btnNow.SetCaption(L"�Ͽ�����");
				}
				else
				{
					btnNow.SetCaption(L"��������");
				}
			}
			else if(ID_DISCONNECT_NEWSSERVER == tempData.ID)
			{
				if (m_wndStatusBar.m_bConnectNews)
				{
					btnNow.SetCaption(L"�Ͽ���Ѷ");
				}
				else
				{
					btnNow.SetCaption(L"������Ѷ");
				}
			}
			else if(ID_MENU_AUTOCONNECT == tempData.ID)
			{
				if(pDoc->m_bAutoLogin)
				{
					btnNow.SetCaption(_T("�ֶ���¼"));		
				}
				else
				{
					btnNow.SetCaption(_T("�Զ���¼"));	
				}
			}
			else if(ID_SYS_AUTO_RUN == tempData.ID)
			{
				if(pDoc->m_bAutoRun)
				{
					btnNow.SetCaption(_T("�رտ�������"));		
				}
				else
				{
					btnNow.SetCaption(_T("��������"));	
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

				// ����ģ����������Ŀ�ʼҳ������(����ҳ������)��״̬
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
							bEnableLock = FALSE;	// ǿ�ƽ�ֹ�ı�lock״̬
						}
						if ( pChildFrame->IsF7AutoLock() )
						{
							bEnableLock = FALSE;	// F7������ı�
						}

						if(btnSub.GetControlId() == ID_LOCK_CUR_CHILDFRAME)
						{
							btnSub.EnableButton(bEnableLock);

							if (pChildFrame->IsLockedSplit())
							{
								if(btnSub.GetCaption() == L"����ҳ������")
								{
									btnSub.SetCaption(_T("��ʼҳ������"));
								}
							}
							else
							{
								if(btnSub.GetCaption() == L"��ʼҳ������")
								{
									btnSub.SetCaption(_T("����ҳ������"));
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

// ��ʼ�����е�һ���Ͷ����˵�
void CMainFrame::InitMapMenuItem()
{
	m_mapMenuCollect.clear();
	VecMenuContainer vecMenu;
	vecMenu.clear();

	// ϵͳ�˵�
	T_MenuItemData tMenuData;
	tMenuData.ID = ID_DOWNLOAD;
	tMenuData.strText = L"��������";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgSysTop;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_UPDATE;
	tMenuData.strText = L"��������";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgItemSimple;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_SYS_AUTO_RUN;
	tMenuData.strText = L"��������";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgItemSimple;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_MENU_AUTOCONNECT;
	tMenuData.strText = L"�Զ���¼";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgCenterSplitTop;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_DISCONNECT_SERVER;
	tMenuData.strText = L"�Ͽ�����";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgCenterSplitBottom;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_DISCONNECT_NEWSSERVER;
	tMenuData.strText = L"������Ѷ";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgItemSimple;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_MODULE_SETTING;
	tMenuData.strText = L"ģ������";
	tMenuData.bShowArrow = true;
	tMenuData.pImgData = m_pImgItemArrow;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_GRIDHEAD_SET;
	tMenuData.strText = L"��Ŀ����";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgCenterSplitTop;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_MENU_EXIT;
	tMenuData.strText = L"�˳�";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgSysBottom;
	vecMenu.push_back(tMenuData);

	m_mapMenuCollect[EMIT_FIR_SYSTEM] = vecMenu;

	// ѡ�ɲ˵�
	vecMenu.clear();
	tMenuData.ID = ID_SELECT_STOCK;
	tMenuData.strText = L"����ѡ����";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgSysTop;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_CUSTOM_STOCK;
	tMenuData.strText = L"����ѡ����";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgItemSimple;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = IDC_INTERVAL_SORT;
	tMenuData.strText = L"�׶�����";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgHelpBottom;
	vecMenu.push_back(tMenuData);

	m_mapMenuCollect[EMIT_FIR_SEL_STOCK] = vecMenu;

	// ���ײ˵�
	vecMenu.clear();
	tMenuData.ID = ID_DELEGATE_TRADE;
	tMenuData.strText = L"ί�н���";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgSysTop;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_TRADE_SETTING;
	tMenuData.strText = L"��������";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgHelpBottom;
	vecMenu.push_back(tMenuData);

	m_mapMenuCollect[EMIT_FIR_TRADE] = vecMenu;

	// �����˵�
	vecMenu.clear();
	tMenuData.ID = ID_USE_HELP;
	tMenuData.strText = L"����˵����";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgHelpTop;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_RESPONSIBILITY;
	tMenuData.strText = L"��������";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgCenterSplitBottom;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_APP_ABOUT;
	tMenuData.strText = L"����";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgHelpBottom;
	vecMenu.push_back(tMenuData);

	m_mapMenuCollect[EMIT_FIR_HELP] = vecMenu;


	// ϵͳ�˵���ģ�����ò˵�
	vecMenu.clear();
	tMenuData.ID = ID_NEW_WORKSPACE;
	tMenuData.strText = L"�½�����ҳ��";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgSysTop;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_OPEN_CHILDFRAME;
	tMenuData.strText = L"�򿪹���ҳ��";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgItemSimple;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_SAVE_CUR_CHILDFRAME;
	tMenuData.strText = L"���湤��ҳ��";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgItemSimple;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_SAVEAS_WORKSPACE;
	tMenuData.strText = L"��湤��ҳ��";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgItemSimple;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_RESTORE_VIEWSP;
	tMenuData.strText = L"�ָ�ϵͳҳ��";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgItemSimple;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_LOCK_CUR_CHILDFRAME;
	tMenuData.strText = L"����ҳ������";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgItemSimple;
	vecMenu.push_back(tMenuData);

	tMenuData.ID = ID_LAYOUT_ADJUST;
	tMenuData.strText = L"���沼�ֵ���";
	tMenuData.bShowArrow = false;
	tMenuData.pImgData = m_pImgHelpBottom;
	vecMenu.push_back(tMenuData);

	m_mapMenuCollect[EMIT_SEC_MODULESET] = vecMenu;

}

// ����һ���˵��Ĳ˵���IDѰ��������˵�������m_mapMenuCollect�ж�Ӧ��ID���㷨�д��Ż�
int32 CMainFrame::FindFirstVecIDByMenuID( int32 iMenuID )
{
	// �ж����˵���һ���˵�����ģ������, ���������������ģ���Ҫ�������Ӧ����
	int32 iRetID = 0;      // ��ʼ������ֵ���Է�����
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

	//����������Ӧ�ô���
	//ASSERT(0);
	return NULL;
}

