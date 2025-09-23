// StatusBarEx.cpp : implementation file
//

#include "stdafx.h"
#include "dlgspirite.h"
#include "dlgnetwork.h"
#include "DlgRecentMerch.h"
#include "dlgindexprompt.h"
#include "DlgAlarm.h"

#include "memdc.h"



#include "IoViewReport.h"
#include "IoViewManager.h"
#include "GdiPlusTS.h"
#include "FontFactory.h"

#include "NewsEngineManager.h"
#include "StatusBarEx.h"
#include "BalloonMsg.h"

#include "XmlShare.h"
//#include "tinystr.h"
#include "tinyxml.h"

#include "afxconv.h"

#include "PathFactory.h"
#include "DlgInputShowName.h"


#include "Proxy_auth_client_base.h"
#include "CCodeFile.h"
#include "XmlShare.h"
#include "coding.h"
#include "DlgIE.h"
#include "ConfigInfo.h"
#include "EngineCenterBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef IDC_HAND
#define IDC_HAND MAKEINTRESOURCE(32649)		// From WINUSER.H
#endif


#define  IDC_SEARCH_EDIT_ID 5066
#define  SEARCH_EDIT_WIDTH  221

#define BUTTON_SIDEBAR		60005
#define BUTTON_SCTOOLBAR	60006



// 
const int32 KTimeNetPintPaint			= 452136;		// 定时刷新
const int32 KTimePeriodPaint			= 55;			// 每100毫秒刷一次

const int32 KUpdateTimeCount			= 1;			// 每3个定时器更新一次时间
const int32 KUpdateNetworkPicCount		= 10;			// 每10个定时器更新一次网络图标显示状态
const int32 KUpdateNetworkDetailCount	= 30;			// 每30个定时器更新一次网络详情状态

const int32 KTimerIdSBUpateOrg			= 452137;		// 刷新跑马灯新闻
const int32 KTimerPeriodSBUpdateOrg		= 1000*60*9;	

const int32 KTimerIdTodayComment        = 452138;       //金评
const int32 KTimerPeriodTodayComment    = 1500;
//IM
const int32 KTimerIdImId                = 452150;       //金评
const int32 KTimerPeriodIm              = 500;

const int32 KIBBSSeparate = 50;
const int32 KIBBSGroupSeparate = 150;

const COLORREF KColorText = RGB(170, 170, 170);			// 文字颜色

const int32 KIDuoKongSmallRectCount = 6;			// 多空阵线下方每个市场多少个小矩形

// xml 
static const TCHAR KStrXmlFileName[] = _T("StatusBarMerch.xml");
static const char  KStrXmlMerch[]	  = "merch";
static const char  KStrXmlMerchName[] = "name";
static const char  KStrXmlMerchCode[] = "code";
static const char  KStrXmlMarketId[]  = "market";
static const char  KStrXmlOrderId[]	  = "order";

static CStatusBarEx::T_BarMerch	KMerchKeyDefault[KMaxNumStatus] = { CStatusBarEx::T_BarMerch(_T("上证"), 0, _T("000001")),
																	CStatusBarEx::T_BarMerch(_T("深证"), 1000, _T("399001")),
																	/*CStatusBarEx::T_BarMerch(_T("沪深300"), 1000, _T("399300")),*/
																	CStatusBarEx::T_BarMerch(_T("创业板"), 1000, _T("399006"))};
#define DIS_CAPTION_NUM	2

CString g_aStrSidebarCaption[DIS_CAPTION_NUM][DIS_CAPTION_NUM] = {
																{_T("显示侧边栏"),_T("隐藏侧边栏")},
																{_T("开启工具栏"),_T("隐藏工具栏")}
																							};

/////////////////////////////////////////////////////////////////////////////
// CStatusBarEx

CStatusBarEx::CStatusBarEx()
:m_ciBarMerchCount(sizeof(m_BarMerchKind)/sizeof(m_BarMerchKind[0]))
{
	m_pDlgKeyBoard = NULL;
	m_iHeight = 41;
	m_iCurSel = -1;

	int32 iYCenter = 16;
	
	ASSERT( sizeof(m_BarMerchKind)/sizeof(T_BarMerch) == sizeof(KMerchKeyDefault)/sizeof(T_BarMerch) );	// 数目要相等
	int32 iMinBarCount = min(sizeof(m_BarMerchKind)/sizeof(T_BarMerch), sizeof(KMerchKeyDefault)/sizeof(T_BarMerch));
	{
		for ( int32 i=0; i < iMinBarCount ; i++ )
		{
			m_BarMerchKind[i] = KMerchKeyDefault[i];
		}
	}
	m_iBarMerchCur = -1;
	ASSERT( m_ciBarMerchCount == sizeof(m_BarMerchKind)/sizeof(T_BarMerch) );

	//第一排
	m_iMerchKindNumber = 4;
	

	m_RectMerchKind[0].m_iMinWidth = iYCenter;
	m_RectMerchKind[0].m_StrName = L"上证";
	m_RectMerchKind[0].m_StrNow  = L"-";
	m_RectMerchKind[0].m_StrCompare = L"-";
	m_RectMerchKind[0].m_StrAmount  = L"-";
	
	m_RectMerchKind[1].m_iMinWidth = iYCenter;
	m_RectMerchKind[1].m_StrName = L"深证";
	m_RectMerchKind[1].m_StrNow  = L"-";
	m_RectMerchKind[1].m_StrCompare = L"-";
	m_RectMerchKind[1].m_StrAmount  = L"-";
		
	m_RectMerchKind[2].m_iMinWidth = iYCenter;
	m_RectMerchKind[2].m_StrName = L"创业板";
	m_RectMerchKind[2].m_StrNow  = L"-";
	m_RectMerchKind[2].m_StrCompare = L"-";
	m_RectMerchKind[2].m_StrAmount  = L"-";

	m_RectMerchKind[3].m_iMinWidth = iYCenter;
	m_RectMerchKind[3].m_StrName = L"沪深300";
	m_RectMerchKind[3].m_StrNow  = L"-";
	m_RectMerchKind[3].m_StrCompare = L"-";
	m_RectMerchKind[3].m_StrAmount  = L"-";

	//
	m_RectRecentMerch.m_iMinWidth = 35;
	m_RectRecentMerch.m_StrName   = L"最近";

	// 第二排

	m_RectTitle.m_iMinWidth = 60;
	m_RectTitle.m_StrName	= AfxGetApp()->m_pszAppName;

	m_RectBBS.m_iMinWidth	= 20;
	m_RectBBS.m_StrName		= L"";
	
	m_RectAlarm.m_iMinWidth = 20;
	m_RectAlarm.m_StrName	= L"";

	m_RectShowTips.m_iMinWidth = 360;
	m_RectShowTips.m_StrName	= L"本软件所载观点仅做参考，不可直接作为投资依据。";

	m_RectNetwork.m_iMinWidth = iYCenter;
	m_RectNetwork.m_StrName	  = L"N";

	CTime curtime = CTime::GetCurrentTime();
	m_StrCurTime.Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), curtime.GetYear(), curtime.GetMonth(), curtime.GetDay(), curtime.GetHour(), curtime.GetMinute(), curtime.GetSecond());
	m_RectTime.m_iMinWidth = 180;
	m_RectTime.m_StrName=m_StrCurTime;

	m_bShowNetworDetailNotity = false;
	//
	m_bConnect	= false;
	m_bConnectNews  = false;
	m_iShowPic1 = 0;

	m_iUpdateTimeCounter = 0;
	m_iUpdateNetworkPicCounter = 0;
	m_iUpdateNetworkDetailCounter = 0;

	//m_iNetworkNotifyCount = 0;

	m_bMouseTrack = FALSE;
	m_ptLastMouseMove = CPoint(-1, -1);

	


	
	m_StrTips	= L"";

	m_mapNetworkNotifyCount[EUCCF_ConnectDataServer] = 0;
	m_mapNetworkNotifyCount[EUCCF_ConnectNewsServer] = 0;



	
	VERIFY(m_EditFont.CreateFont(
		16,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		L"微软雅黑"));		   // lpszFacename	//...



	//	
	
	// 图片资源加载

	// 背景图(用主标题栏一样的背景)
	m_pImageBkGround = NULL;

	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_STATUSBAR, L"PNG", m_pImageBkGround))
	{
		ASSERT(0);
		m_pImageBkGround = NULL;
	}

	// 条件预警开启的图
	m_pImageAlarmOn = NULL;
	
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_ALARM_ON, L"PNG", m_pImageAlarmOn))
	{
		ASSERT(0);
		m_pImageAlarmOn = NULL;
	}
	
	// 条件预警关闭的图
	m_pImageAlarmOff = NULL;
	
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_ALARM_OFF, L"PNG", m_pImageAlarmOff))
	{
		ASSERT(0);
		m_pImageAlarmOff = NULL;
	}

	// 网络正常的图

	m_pImageNetOn1 = NULL;
	
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_NET_ON1, L"PNG", m_pImageNetOn1))
	{
		ASSERT(0);
		m_pImageNetOn1 = NULL;
	}

	m_pImageNetOn2 = NULL;
	
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_NET_ON2, L"PNG", m_pImageNetOn2))
	{
		ASSERT(0);
		m_pImageNetOn2 = NULL;
	}

	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_NET_ON3, L"PNG", m_pImageNetOn3))
	{
		ASSERT(0);
		m_pImageNetOn3 = NULL;
	}
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_NET_ON4, L"PNG", m_pImageNetOn4))
	{
		ASSERT(0);
		m_pImageNetOn4 = NULL;
	}

	// 网络断开的图

	m_pImageNetOff = NULL;
	
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_NET_OFF, L"PNG", m_pImageNetOff))
	{
		ASSERT(0);
		m_pImageNetOff = NULL;
	}

	// 最近商品:

	m_pImageRecentMerch = NULL;
	
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_RECENT_MERCH, L"PNG", m_pImageRecentMerch))
	{
		ASSERT(0);
		m_pImageRecentMerch = NULL;  
	}

	// 跑马灯的背景:
	m_pImageBkBBS = NULL;

	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_BkBBS, L"PNG", m_pImageBkBBS))
	{
		ASSERT(0);
		m_pImageBkBBS = NULL;
	}

	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_BkBBSLEFT, L"PNG", m_pImageBkBBSLeft))
	{
		ASSERT(0);
		m_pImageBkBBSLeft = NULL;
	}

	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_BkBBSRIGHT, L"PNG", m_pImageBkBBSRight))
	{
		ASSERT(0);
		m_pImageBkBBSRight = NULL;
	}
	//金评
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_BULB_OFF, L"PNG", m_pImageTodayCommentOff) )
	{
		ASSERT(0);
		m_pImageTodayCommentOff = NULL;
	}


	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_BULB_BLUE, L"PNG", m_pImageTodayCommentBlue) )
	{
		ASSERT(0);
		m_pImageTodayCommentBlue = NULL;
	}

	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_BLUE_RED, L"PNG", m_pImageTodayCommentRed) )
	{
		ASSERT(0);
		m_pImageTodayCommentRed = NULL;
		
	}
	m_pImageTodayCommentOn = m_pImageTodayCommentRed;

	// IM消息
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_MESSAGE,L"PNG", m_pImageMessageShow) )
	{
		ASSERT(0);
		m_pImageMessageShow = NULL;
			
	}

	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_MESSAGE_HIDE,L"PNG", m_pIMageMessageHide) )
	{
		// ASSERT(0);
		m_pIMageMessageHide = NULL;
		
	}

	m_pImageMessage = m_pImageMessageShow;
	m_bMember = true;

	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDB_PNG_SEARCH, L"PNG", m_pImageSearch))
	{
		ASSERT(0);
		m_pImageRecentMerch = NULL;  
	}

	m_pImageSearch_BK = NULL;
	m_pImageSearch_BK = Image::FromFile(L".//image//SearchEdit.png");	

	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDB_PNG_LEFTBAR, L"PNG", m_pImageSCToolBar))
	{
		ASSERT(0);
		m_pImageSCToolBar = NULL;  
	}


	// 多空小图
	m_pImageDuoKongSmallBmp = Bitmap::FromResource(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP_STATUSBAR_DUOKONG));
	ASSERT( NULL != m_pImageDuoKongSmallBmp && m_pImageDuoKongSmallBmp->GetLastStatus() == Ok );
	m_aDuoKongImageH.SetSize(KIDuoKongSmallRectCount);
	m_aDuoKongImageS.SetSize(KIDuoKongSmallRectCount);
	for ( int32 i=0; i < KIDuoKongSmallRectCount ; i++ )
	{
		m_aDuoKongImageH[i] = m_aDuoKongImageS[i] = 4;		// 白平 默认
	}
}

void CStatusBarEx::UpdateRect ( int32 iWidth, int32 iHeight )
{
	CRect rect ;
	GetClientRect( & rect );

	iWidth = rect.Width();
	iHeight = rect.Height();

	//const int32 iDuoKongWidth = 100;		// 多空固定宽度

	int32 iPicRectWidth = 25;	
	int32 iBtnSidebarWidth = 110;
	// 从后往前排版:
	// 如果没有足够的区域，那么给跑马灯的就是0
	
	// 时间:
	m_RectTime = rect;
	m_RectTime.right = rect.right - 5;
	m_RectTime.left = rect.right - 180;//70; // 时间不要
	
	//工具栏
	m_RectScToolBar = rect;
	m_RectScToolBar.left = rect.right;
	//m_RectScToolBar.right = m_RectTime.left - 5;
	//m_RectScToolBar.left  = m_RectScToolBar.right - iPicRectWidth;

	// 网络:
	m_RectNetwork = rect;
	m_RectNetwork.top = (rect.Height() - 18) / 2;
	m_RectNetwork.bottom = m_RectNetwork.top + 18;
	m_RectNetwork.right = m_RectTime.left;
	m_RectNetwork.left  = m_RectNetwork.right - 24;

	// 预警:
	m_RectAlarm = rect;
	m_RectAlarm.top = (rect.Height() - 18) / 2;
	m_RectAlarm.bottom = m_RectNetwork.top + 18;
	m_RectAlarm.right = m_RectNetwork.left -16;
	m_RectAlarm.left  = m_RectAlarm.right - 16;

	// 最近:
	m_RectRecentMerch = rect;
	m_RectRecentMerch.top = (rect.Height() - 18) / 2;
	m_RectRecentMerch.bottom = m_RectNetwork.top + 18;
	m_RectRecentMerch.right = m_RectAlarm.left -16;
	m_RectRecentMerch.left  = m_RectRecentMerch.right - 19;

	int iSearchEditWidth = SEARCH_EDIT_WIDTH;
	int iSearchEditHight = 25;
	if (m_pImageSearch_BK)
	{
		iSearchEditWidth = m_pImageSearch_BK->GetWidth();
		iSearchEditHight = m_pImageSearch_BK->GetHeight();
	}
	m_KeyBoarEditRect = rect;
	m_KeyBoarEditRect.right = m_RectRecentMerch.left - 16;			//减3，给文本框留位置，显得美观
	m_KeyBoarEditRect.left = m_KeyBoarEditRect.right - iSearchEditWidth;
	if (iSearchEditHight <  m_KeyBoarEditRect.Height())
	{
		m_KeyBoarEditRect.top = (m_KeyBoarEditRect.Height() - iSearchEditHight ) /2 +1;
		m_KeyBoarEditRect.bottom = m_KeyBoarEditRect.top + iSearchEditHight;
	}
	
	

	//今评
	m_RectTodayComment = rect;
	//if (m_bMember)
	//{
	//	m_RectTodayComment.right = m_RectRecentMerch.left;
	//	//m_RectTodayComment.left = m_RectTodayComment.right - iDuoKongWidth;
	//	m_RectTodayComment.left = m_RectTodayComment.right - iPicRectWidth;
	//}
	//else
	{
		m_RectTodayComment.right = m_KeyBoarEditRect.left;
		m_RectTodayComment.left = m_RectTodayComment.right - 0;
	}
	//消息
	LONG offsetLeft = m_RectTodayComment.left;

	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if ( pApp->m_pConfigInfo->m_bImVersion )
	{
		m_RectMessage = rect;
		m_RectMessage.right = m_RectTodayComment.left;
		m_RectMessage.left = m_RectMessage.right - iPicRectWidth;
		offsetLeft = m_RectMessage.left;
	}
	else
	{
		m_RectMessage = CRect(-1, -1, -1, -1);
	}


	int iSpacing = 29;
	// 初步预留200给单个商品显示, 预留100给跑马灯
	//m_RectShowTips.right - 332;
	int32 iWidthPer	= 225;
	const int32 iShowTips = 332;
	const int32 iLeftWidth = 20;// 左侧间隔
	const int32 iMerchWidth = 30;// 商品之间的间隔
	const int32 KiMerchWidth = (m_KeyBoarEditRect.left  - iLeftWidth - iShowTips - iSpacing*2 - iMerchWidth*(KMaxNumStatus-1) + 5)/KMaxNumStatus;
	iWidthPer = max(KiMerchWidth, iWidthPer);	// 取能够分配的小的值

	// 上证
	m_RectMerchKind[0].top		= rect.top;	
	m_RectMerchKind[0].bottom	= rect.bottom;
	m_RectMerchKind[0].left		= rect.left + iLeftWidth;
	m_RectMerchKind[0].right	= m_RectMerchKind[0].left + iWidthPer;
	for (int i = 1; i < KMaxNumStatus; i++)
	{

		m_RectMerchKind[i].top		= rect.top;		
		m_RectMerchKind[i].bottom	= rect.bottom;	
		m_RectMerchKind[i].right  	= m_RectMerchKind[i-1].right + iMerchWidth + iWidthPer;
		if (m_RectMerchKind[i].right  > (m_KeyBoarEditRect.left - iSpacing))
		{
			m_RectMerchKind[i].right = m_RectMerchKind[i-1].right;
			m_RectMerchKind[i].left  = m_RectMerchKind[i-1].right;
		}
		else
		{
			m_RectMerchKind[i].left	= m_RectMerchKind[i-1].right + iMerchWidth;	
		}
	}
	// 提示文字
	m_RectShowTips = rect;
	m_RectShowTips.right = m_KeyBoarEditRect.left - iSpacing;		
	// 235*KMaxNumStatus指数信息，20是左侧留出值，29与右侧最小距离，332是提示信息的
	if (KiMerchWidth >= iWidthPer )
	{
		m_RectShowTips.left  = m_RectShowTips.right - 332;
	}
	else
	{
		m_RectShowTips.left = m_RectShowTips.right;
	}
	
}

CStatusBarEx::~CStatusBarEx()
{
	DEL(m_pImageBkGround);
	DEL(m_pImageAlarmOn);	
	DEL(m_pImageAlarmOff);
	DEL(m_pImageNetOn1);
	DEL(m_pImageNetOn2);
	DEL(m_pImageNetOn3);
	DEL(m_pImageNetOn4);
	DEL(m_pImageNetOff);
	DEL(m_pImageRecentMerch);
	DEL(m_pImageBkBBS);
	DEL(m_pImageBkBBSLeft);
	DEL(m_pImageBkBBSRight);
	DEL(m_pImageTodayCommentOff);
	DEL(m_pImageTodayCommentRed);
	DEL(m_pImageTodayCommentBlue);
	DEL(m_pImageMessageShow);
	DEL(m_pIMageMessageHide);
	DEL(m_pImageDuoKongSmallBmp);
	DEL(m_pImageSearch);
	DEL(m_pImageSearch_BK);
	DEL(m_pImageSCToolBar);

	m_EditFont.DeleteObject();

	

	auth::iProxyAuthClient *pAuth = GetAuthManager();
	if ( NULL!=pAuth )
	{
		//pAuth->SetUserDataNotify(NULL);
	}

	CAlarmCenter* pAlarmCenter = GetAlarmCenter();
	if ( NULL != pAlarmCenter )
	{
		pAlarmCenter->DelNotify(this);
	}
}

BEGIN_MESSAGE_MAP(CStatusBarEx, CStatusBar)
	//{{AFX_MSG_MAP(CStatusBarEx)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(UM_HOTKEY, OnMsgHotKey)
	ON_MESSAGE(UM_Dlghotkey_Close,OnMsgDlgHotKeyClose)
	ON_MESSAGE(WM_USER_EDIT_SEARCH_CLICKED,SearchEditClick)
	ON_WM_DESTROY()
	ON_WM_SETCURSOR()
	//ON_COMMAND_RANGE(IDM_STATUS_BEGIN,IDM_STATUS_END,OnMenu)  
	ON_UPDATE_COMMAND_UI(IDM_STATUS_SHOW2, OnUpdateShow2)
	ON_MESSAGE_VOID(UM_DO_ORGDATAUPDATE, DoOnOrgDataUpdate)
	ON_EN_CHANGE(IDC_SEARCH_EDIT_ID, OnChangeEdit)
	ON_EN_SETFOCUS(IDC_SEARCH_EDIT_ID, OnSetEditFocus)
	ON_EN_KILLFOCUS(IDC_SEARCH_EDIT_ID, OnKillEditFocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatusBarEx message handlers

BOOL CStatusBarEx::OnEraseBkgnd(CDC* pDC) 
{	
	return TRUE;
}

void CStatusBarEx::GetClientRect(LPRECT pRect)
{
	CStatusBar::GetClientRect(pRect);
	pRect->bottom = pRect->top + m_iHeight;
}

void CStatusBarEx::OnSize(UINT nType, int cx, int cy) 
{
	if ( cx > 0 && cy > 0 )
	{
		UpdateRect ( cx, cy );

		m_KeyBoarEdit.SetEditArea(30, 4, 15, 1, m_KeyBoarEditRect.Width()-2, m_KeyBoarEditRect.Height()-2);
		m_KeyBoarEdit.MoveWindow(m_KeyBoarEditRect);

	}
	CStatusBar::OnSize(nType,cx,cy);
}

int CStatusBarEx::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatusBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_TipWnd.Create(this))
		return -1;	

	CStatusBar::GetStatusBarCtrl().SetMinHeight(m_iHeight);
	
	// 注意,这里设置父指针.不要有空指针!

	//第一排	

	for ( int32 i = 0; i < m_ciBarMerchCount; i ++ )
	{
		m_RectMerchKind[i].m_pParent = this;
	}

	m_RectRecentMerch.m_pParent = this;

	//第二排

	m_RectTitle.m_pParent	     = this;
	m_RectBBS.m_pParent		     = this;
	m_RectTodayComment.m_pParent = this;
	m_RectMessage.m_pParent      = this;
	m_RectNetwork.m_pParent      = this;
	m_RectAlarm.m_pParent	     = this;
	m_RectTime.m_pParent	     = this;
	m_RectDuoKong.m_pParent      = this;
	m_RectScToolBar.m_pParent	 = this;


	// 初始化商品列表
	LoadFromXml();

	if ( NULL == AfxGetDocument() || NULL == AfxGetDocument()->m_pAbsCenterManager || NULL == AfxGetDocument()->m_pAbsCenterManager->GetServiceDispose() )
	{
		return 0;
	}

	//m_KeyBoarEditRect.m_pParent = this;     
	  

	m_KeyBoarEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,m_KeyBoarEditRect,this,  IDC_SEARCH_EDIT_ID);
	m_KeyBoarEdit.SetImage(m_pImageSearch_BK);
	m_KeyBoarEdit.SetButtonClickedMessageId(WM_USER_EDIT_SEARCH_CLICKED);
	m_KeyBoarEdit.SetOwner(this);
	m_KeyBoarEdit.SetTipText(L"股票代码/首字母/板块");
	m_KeyBoarEdit.SetTipColor(RGB(0x89, 0x8e, 0x94));
	m_KeyBoarEdit.SetBkColorref(RGB(25,24,30));
	m_bKeyBoardEditClick = false;
	
	m_KeyBoarEdit.SetFont(&m_EditFont);

	return 0;
}


void	CStatusBarEx::UnInit()
{
	//	网络链接失败详情通知显示
	m_bShowNetworDetailNotity = false;

	KillTimer(KTimerIdImId);//IM


	KillTimer(KTimerIdSBUpateOrg);

	KillTimer(KTimerIdTodayComment);

	CGGTongDoc *pDocument = AfxGetDocument();	
	if (NULL == pDocument || NULL == pDocument->m_pAbsCenterManager)
	{
		return;
	}

	pDocument->m_pAbsCenterManager->RemoveAttendMerch(EA_StatucBar);

	//for (int i =0; i < sizeof(m_RectMerchKind)/sizeof(m_RectMerchKind[0]); ++i)
	//{
	//	m_RectMerchKind[i].m_StrAmount = L"";
	//	m_RectMerchKind[i].m_StrCompare = L"";
	//	m_RectMerchKind[i].m_StrNow = L"";
	//}

	//for(int i =0; i < sizeof(m_BarMerchKind)/sizeof(m_BarMerchKind[0]); ++i)
	//{
	//	m_BarMerchKind[i].m_pMerch = NULL;
	//}
	////	跑马灯文本
	m_aOrgAdvs.RemoveAll();		
}
void	CStatusBarEx::InitStatusEx()
{
	//	网络链接失败详情通知显示
	m_bShowNetworDetailNotity = true;
	// 请求数据
	MerchArray aMerchs; 
	GetStatusBarMerchs(aMerchs);
	{
		for ( int i=aMerchs.GetSize()-1; i >= 0 ; i-- )
		{
			if ( NULL == aMerchs[i] )
			{
				aMerchs.RemoveAt(i);
			}
		}
		SendStatusBarMerchs(aMerchs);
	}

	// 机构私有数据 跑马灯新闻
	OnOrgDataUpdate();
	auth::iProxyAuthClient *pAuth = GetAuthManager();
	if ( NULL!=pAuth )
	{
		//		pAuth->SetUserDataNotify(this);
		SetTimer(KTimerIdSBUpateOrg, KTimerPeriodSBUpdateOrg, NULL);
	}



	// 报警通知
	CAlarmCenter* pAlarmCenter = GetAlarmCenter();
	if ( NULL != pAlarmCenter )
	{
		pAlarmCenter->AddNotify(this);
	}

	SetTimer(KTimerIdImId, KTimerPeriodIm, NULL);//IM

	SetTimer(KTimeNetPintPaint, KTimePeriodPaint, NULL);
}

void CStatusBarEx::RefreshSpecifiedArea()
{
	CRect rect ; 
	GetClientRect(&rect);
	
	CRect RectLeft(rect), RectRight(rect);

	RectLeft.right = m_KeyBoarEditRect.left - 10;
	RectRight.left  = m_KeyBoarEditRect.right  + 10;
	
	InvalidateRect(RectLeft);
	InvalidateRect(RectRight);
}

void CStatusBarEx::OnPaint() 
{
	CPaintDC pdc(this); 

	CRect Rect11;
	GetClientRect(&Rect11);

	CMemDC dc(&pdc, Rect11);
	int32 iOldMode = dc.SetBkMode(TRANSPARENT);
	dc->FillSolidRect(Rect11, RGB(33,34, 40));
	
	// 字色,字体	
	dc.SetTextColor( KColorText );
	LOGFONT FontStatus;
	InitStatusBBSLogFont(FontStatus);
	
	CFont FontSts;
	
	BOOL bOK = FontSts.CreateFontIndirect(&FontStatus);
	if ( !bOK)
	{
		return;
	}

	CFont* pOldFont = dc.SelectObject(&FontSts);
	//

	for (int32 i = 0; i < m_iMerchKindNumber; i ++ )
	{
		m_RectMerchKind[i].DrawRect(&dc,m_RectMerchKind[i]);
	}
	 
	COLORREF clrActive = RGB(251, 206, 3);//CFaceScheme::Instance()->GetActiveColor();
	COLORREF clrBefore = dc.SetTextColor(clrActive);
	dc.SetTextColor(clrBefore);

	m_RectDuoKong.DrawRectDuoKongZhenXian(&dc, m_RectDuoKong);
 
	CPen pen1,*pOldPen1;
	pen1.CreatePen(PS_SOLID,1,RGB(77,77,77)/*CFaceScheme::Instance()->GetSysColor(ESCText)*/);
	pOldPen1 = (CPen*)dc.SelectObject(&pen1);
	


	//m_RectBBS.DrawRectBBS(&dc,m_RectBBS, m_RectShow);
	m_RectNetwork.DrawRectNetWork(&dc,m_RectNetwork);
	m_RectTime.DrawRectRow2(&dc,m_RectTime);
	m_RectAlarm.DrawRectAlarmMerch(&dc,m_RectAlarm);
	m_RectRecentMerch.DrawRectRecentMerch(&dc,m_RectRecentMerch);
	//m_RectScToolBar.DrawRectLeftBar(&dc,m_RectScToolBar);
	m_RectShowTips.DrawShowTips(&dc,m_RectShowTips);

	//dc->MoveTo(m_RectScToolBar.left -1, m_RectScToolBar.top +5);
	//dc->LineTo(m_RectScToolBar.left -1, m_RectScToolBar.bottom -5);

	//if (m_bMember)
	//{
	//	m_RectTodayComment.DrawRectTodayComment(&dc, m_RectTodayComment);
	//}

	m_KeyBoarEdit.OnlyOnceRefresh();


	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if ( pApp->m_pConfigInfo->m_bImVersion )
		m_RectMessage.DrawRectMessage(&dc, m_RectMessage);//显示消息标志



	CPen pen,*pOldPen;
	pen.CreatePen(PS_SOLID,2,RGB(4,4,5)/*CFaceScheme::Instance()->GetSysColor(ESCText)*/);
	pOldPen = (CPen*)dc.SelectObject(&pen);
	dc->MoveTo(Rect11.left, Rect11.top);
	dc->LineTo(Rect11.right, Rect11.top);



	dc.SelectObject(pOldPen);
	dc.SetBkMode(iOldMode);
	dc.SelectObject(pOldFont);
	
	pen.DeleteObject();
	FontSts.DeleteObject();
}

void CStatusBarEx::SetMKNumber ( int32 iNumber )
{
	if ( iNumber < 1 || iNumber > KMaxNumStatus ) return;
	m_iMerchKindNumber = iNumber;
	CRect Rect;
	GetClientRect(&Rect);
	UpdateRect(Rect.Width(),Rect.Height());

	m_KeyBoarEdit.MoveWindow(m_KeyBoarEditRect);
	//Invalidate();
	RefreshSpecifiedArea();


}

int32  CStatusBarEx::GetMKNumber()
{
	return m_iMerchKindNumber;
}

void CStatusBarEx::HitTestRectData ( CPoint pt, int32 iType )
{	
	CMainFrame *pMainFrm = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	
	if (!pMainFrm)
	{
		return;
	}

	if (m_RectNetwork.PtInRect(pt))
	{
#ifdef DEBUG
		CreateNetWorkDialog();
#endif
		return;
	}
	else if ( m_RectRecentMerch.PtInRect(pt))
	{
		CreateRecentMerchDialog();
		return;
	}	
	else if ( m_RectAlarm.PtInRect(pt))
	{
		CreateAlarmDialog();
		return;
	}
	else if ( m_RectTodayComment.PtInRect(pt) ) 
	{
		CreateNewsView();
		return;
	}
	else if ( m_RectMessage.PtInRect(pt) )
	{
		ShowImMessage();
		return;
	}
	
	else if ( m_RectScToolBar.PtInRect(pt) )
	{
		// 切换状态
		//pMainFrm->OnMenuShowToolBar();
		//return;
	}

	this->SetFocus();
	
	int32 i = 0;
	for ( i = 0; i < m_iMerchKindNumber; i ++ )
	{
		if ( m_RectMerchKind[i].HitTest(pt,iType))
		{
			m_iCurSel = i;
			return;
		}
	}

	// 是否在广告上面
	for ( i=0; i < m_aOrgAdvRects.GetSize() ; i++ )
	{
		if ( m_aOrgAdvRects[i].PtInRect(pt) )
		{
			// 打开该广告
			const int32 iIndex = m_aOrgAdvRects[i].iOrgIndex;
			ASSERT( iIndex >= 0 && iIndex < m_aOrgAdvs.GetSize() );
			if ( iIndex >= 0 && iIndex < m_aOrgAdvs.GetSize() )
			{
				CDlgIE::ShowDlgIE(_T("系统公告"), m_aOrgAdvs[iIndex].m_StrUrl, m_aOrgAdvs[iIndex].m_StrContent);
			}
			return;
		}
	}


}
void CStatusBarEx::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_TipWnd.Hide();
	HitTestRectData ( point, 0 );	
	CStatusBar::OnLButtonDown(nFlags, point);
}
void CStatusBarEx::OnRButtonDown(UINT nFlags, CPoint point) 
{
	m_TipWnd.Hide();

	// 如果是在左下角商品列表，则弹出选项
	{
		const T_BarMerch *pBar = NULL;
		const T_BarMerch *pBarDefault = NULL;
		int i=0;
		for ( i=0; i < m_ciBarMerchCount ; i++ )
		{
			if ( m_RectMerchKind[i].PtInRect(point) )
			{
				pBar = m_BarMerchKind + i;
				break;
			}
		}
		if ( NULL != pBar )
		{
			ASSERT( i >=0 && i < sizeof(KMerchKeyDefault)/sizeof(KMerchKeyDefault[0]) );
			m_iBarMerchCur = i;

			pBarDefault = KMerchKeyDefault + i;
			CNewMenu menu;
			menu.CreatePopupMenu();
			
			CString StrName = pBarDefault->m_StrName;
			CString StrDefault;
			StrDefault.Format(_T("还原[%s]"), StrName.GetBuffer());
			menu.AppendMenu(MF_SEPARATOR);
			menu.AppendMenu(MF_STRING, 1, _T("更换商品"));
			menu.AppendMenu(MF_STRING, 2, StrDefault);
			menu.RemoveMenu(0, MF_BYPOSITION);

			CPoint ptMenu(point);
			ClientToScreen(&ptMenu);

			CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
			ASSERT( NULL != pMainFrame );

			int iRet = menu.TrackPopupMenu(TPM_BOTTOMALIGN |TPM_LEFTALIGN |TPM_NONOTIFY |TPM_RETURNCMD, ptMenu.x, ptMenu.y, pMainFrame);
			switch (iRet)
			{
			case 1:	// 变更 调出keyboard 等待输入 输入完成后要取消标志
				{
					pMainFrame->ShowHotkeyDlg(NULL, this, EHKTMerch);
				}				
				break;
			case 2:	// 还原成默认的
				{
					UpdateStatusBarMerch(i, *pBarDefault);
				}
				break;
			default:
				;
			}
		}
	}

	CStatusBar::OnRButtonDown(nFlags, point);
}
void CStatusBarEx::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	m_TipWnd.Hide();
	CStatusBar::OnLButtonDblClk(nFlags, point);
	
	for (int32 i=0 ; i<m_iMerchKindNumber;i++)
	{
		if ( m_RectMerchKind[i].PtInRect(point))
		{
			ShowMerchData(i);
			return;
		}
	}
}
void CStatusBarEx::ShowMerchData(int32 iMerchIndex)
{
	int32 iMarketId;
	CString StrMerchCode;
	
// 	if( 1 == iMerchIndex)
// 	{		
// 		iMarketId = 1000;
// 		StrMerchCode = L"399001";
// 	}
// 	else if ( 2 == iMerchIndex)
// 	{
// 		iMarketId = 1000;
// 		StrMerchCode = L"399300";
// 	}
// 	else
// 	{
// 		iMarketId = 0;
// 		StrMerchCode = L"000001";
// 	}	
	if ( iMerchIndex < 0 || iMerchIndex >= m_ciBarMerchCount )
	{
		return;
	}
	iMarketId = m_BarMerchKind[iMerchIndex].m_iMarket;
	StrMerchCode = m_BarMerchKind[iMerchIndex].m_StrCode;

	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd(); 
	//pMainFrame->OnHotKeyMerch(iMarketId,StrMerchCode);
	pMainFrame->OnShowMerchInChart(iMarketId, StrMerchCode, ID_PIC_TREND);
}
void CStatusBarEx::SetRectData(int32 iCurSel,CRectData RectData)
{	
	// m_RectMerchKind[iCurSel].m_StrName = RectData.m_StrName;
	m_RectMerchKind[iCurSel].m_StrNow  = RectData.m_StrNow;
	m_RectMerchKind[iCurSel].m_StrCompare = RectData.m_StrCompare;
	m_RectMerchKind[iCurSel].m_StrAmount = RectData.m_StrAmount;	
	//Invalidate();
	RefreshSpecifiedArea();
	UpdateWindow();
}
void CStatusBarEx::GetRectData(int32 iCurSel,OUT CRectData& RectData)
{
	RectData.m_StrName		= m_RectMerchKind[iCurSel].m_StrName;
	RectData.m_StrNow		= m_RectMerchKind[iCurSel].m_StrNow;	
	RectData.m_StrCompare	= m_RectMerchKind[iCurSel].m_StrCompare;
	RectData.m_StrAmount	= m_RectMerchKind[iCurSel].m_StrAmount;
	RectData.m_pParent		= m_RectMerchKind[iCurSel].m_pParent;
	RectData.m_iMinWidth	= m_RectMerchKind[iCurSel].m_iMinWidth;
	//memcpyex(&RectData,&m_RectMerchKind[iCurSel],sizeof(m_RectMerchKind));							
}

void CStatusBarEx::OnMenu(UINT nID )
{
	ASSERT( 0 );		// 应该是被抛弃的应用
	//... fangz 0512# 具体数据未得到.
	CRectData  RectDatain;
	switch(nID)
	{
	case IDM_STATUS_SHOW2:
		SetMKNumber(2);		
		break;
	case IDM_STATUS_SHOW3:
		SetMKNumber(3);
		break;     
	case IDM_STATUS_SHOW4:
		SetMKNumber(4);
		break; 	
    case IDM_STATUS_STOCK_SH:
		RectDatain.m_StrName = L"沪";
			SetRectData(m_iCurSel,RectDatain);
		break;		
	case IDM_STATUS_SZ:
		RectDatain.m_StrName = L"深";
			SetRectData(m_iCurSel,RectDatain);
		break;
	case IDM_STATUS_FUTURESCN:
		RectDatain.m_StrName = L"金";
			SetRectData(m_iCurSel,RectDatain);
		break;
	case IDM_STATUS_HK:
		RectDatain.m_StrName = L"港";
			SetRectData(m_iCurSel,RectDatain);
		break;
	case IDM_STATUS_GOLD:
		RectDatain.m_StrName = L"黄金";
			SetRectData(m_iCurSel,RectDatain);
		break;
	case IDM_STATUS_FUTURES_SH:
		RectDatain.m_StrName = L"上期";
			SetRectData(m_iCurSel,RectDatain);
		break;
	case IDM_STATUS_ZZ:
		RectDatain.m_StrName = L"郑商";
			SetRectData(m_iCurSel,RectDatain);
		break;
	case IDM_STATUS_DL:
		RectDatain.m_StrName = L"大商";
			SetRectData(m_iCurSel,RectDatain);
		break;
	case IDM_STATUS_FIX:
		RectDatain.m_StrName = L"固收";
			SetRectData(m_iCurSel,RectDatain);
		break;
	case IDM_STATUS_OTHER:
		RectDatain.m_StrName = L"其他";
			SetRectData(m_iCurSel,RectDatain);		
		break;		
	default:
		break;
	}
}

void CStatusBarEx::OnUpdateShow2(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(0);
}

void CStatusBarEx::OnXTimer(int32 iTimerId)
{
	if ( !IsWindowVisible()) return;
	if ( KTimeNetPintPaint == iTimerId)
	{
     	CClientDC dc1(this);		
//		CMemDC dc(&dc1,&m_RectBBS);

		HDC hdc = CreateCompatibleDC(dc1.GetSafeHdc());
		HBITMAP hBmp = ::CreateCompatibleBitmap(hdc, m_RectBBS.right + 10, m_RectBBS.bottom + 10);
		HBITMAP hOldBmp = (HBITMAP)SelectObject(hdc, hBmp);
		CDC dc;
		dc.Attach(hdc);
		
		m_RectBBS.DrawRectBBS(&dc,m_RectBBS, m_RectShow);

		//
		//int w = m_RectBBS.Width();
		//int h = m_RectBBS.Height();
		BitBlt(dc1.GetSafeHdc(), m_RectBBS.left + 1, m_RectBBS.top + 1, m_RectBBS.Width() - 2, m_RectBBS.Height() - 2,
                      dc, m_RectBBS.left + 1, m_RectBBS.top + 1, SRCCOPY);

		SelectObject(dc, hOldBmp);
		DeleteObject(hBmp);

		DeleteDC(hdc);
	}
}


void CStatusBarEx::UpdateTime()
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
    ASSERT(NULL != pApp);
	CGGTongDoc *pDocument = pApp->m_pDocument;	
	ASSERT(NULL != pDocument);
	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL != pAbsCenterManager);

	CTime curtime(pAbsCenterManager->GetServerTime().GetTime());
	m_StrCurTime.Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), curtime.GetYear(), curtime.GetMonth(), curtime.GetDay(), curtime.GetHour(), curtime.GetMinute(), curtime.GetSecond());
	m_RectTime.m_StrName = m_StrCurTime;
}

void CStatusBarEx::UpdateNetworkPic()
{
	m_iShowPic1 = 3;
	// m_iShowPic1 = (++m_iShowPic1)%4;
}

CString CStatusBarEx::GetTime(UINT uiSeconds)
{
	CString StrTime = L"";

	if ( uiSeconds <= 0 )
	{
		return StrTime;	
	}

	//	
	uint32 uiDay = uiSeconds / (24*60*60) ;
	
	uiSeconds -= uiDay*24*60*60;
	
	uint32 uiHour = uiSeconds / (60*60) ;
	
	uiSeconds -= uiHour*60*60;
	
	uint32 uiMiniute = uiSeconds / 60;
	
	uiSeconds -= uiMiniute*60;
	
	//
	StrTime.Format(L"%i 天 %i 小时 %i 分 %i 秒",uiDay,uiHour,uiMiniute,uiSeconds);

	return StrTime;
}

void CStatusBarEx::UpdateNetworkDetail()
{
	CGGTongDoc * pDoc = (CGGTongDoc *)AfxGetDocument();
	ASSERT( NULL!= pDoc );

	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (NULL == pMainFrame)
	{
		// ASSERT( 0 );
		return;
	}	

	m_StrTips = L"";
	
	if ( pDoc->m_pAbsCenterManager->GetKickOutState())
	{
		return;
	}
	
	
	// 行情服务器:
	CArray<CAbsCommunicationShow *,CAbsCommunicationShow *> aQuoteServers;
	if (!pDoc->m_pAbsDataManager)
	{
		return;
	}
	pDoc->m_pAbsDataManager->GetCommnunicaionList(aQuoteServers);

	bool32 bAllQuoteNonConnect = true;

	int32 i = 0;
	for ( i = 0 ;i < aQuoteServers.GetSize() ; i++)
	{		
		CString StrTemp;
		CServerState ServerState;
		aQuoteServers[i]->GetServerState(ServerState);	
		
		if (ServerState.m_bConnected)
		{
			if ( pDoc->m_pAbsCenterManager->BeAuthError() )
			{
				StrTemp.Format(L"连接行情: %s:%i 认证错误, 请重新登录!\n", ServerState.m_StrServerAddr.GetBuffer(), ServerState.m_uiServerPort);
				m_StrTips += StrTemp;
				bAllQuoteNonConnect = false;
				break;
			}
			else
			{
				if ( EASAuthSuccessed == ServerState.m_eAuthState )
				{
					StrTemp.Format(L"连接行情: %s:%i 认证成功!\n连接时长: %s \n", ServerState.m_StrServerAddr.GetBuffer(), ServerState.m_uiServerPort, GetTime(ServerState.m_uiKeepSecond).GetBuffer());
				}
				else if ( EASAuthing == ServerState.m_eAuthState )
				{
					StrTemp.Format(L"连接行情: %s:%i 认证中...\n", ServerState.m_StrServerAddr.GetBuffer(), ServerState.m_uiServerPort);
				}
				else if ( EASAuthFailed == ServerState.m_eAuthState )
				{
					StrTemp.Format(L"连接行情: %s:%i 未认证!\n", ServerState.m_StrServerAddr.GetBuffer(), ServerState.m_uiServerPort);
				}
			}
			
			m_StrTips += StrTemp;	
			bAllQuoteNonConnect = false;
		}		
	}

	m_bConnect = !bAllQuoteNonConnect;

	if ( aQuoteServers.GetSize() <= 0 )
	{
		ASSERT(0);
		m_StrTips = L"未连接行情服务器";
	}

	// 咨询服务器:
	CArray<CAbsNewsCommnunicationShow*, CAbsNewsCommnunicationShow*> aNewsServers;
	pDoc->m_pNewsManager->GetCommnunicaionList(aNewsServers);

	bool32 bAllNewsNonConnect = true;

	for ( i = 0; i < aNewsServers.GetSize(); i++ )
	{
		CString StrTemp;
		CAbsNewsCommnunicationShow* pCommunication = aNewsServers[i];
		if( NULL == pCommunication )
		{
			continue;
		}

		bool32  bConnected;
		CString StrAddress;
		uint32 uiPort;
		uint32 uiKeepSeconds;

		pCommunication->GetNewsServerSummary(bConnected, StrAddress, uiPort, uiKeepSeconds);
		
		CNewsServerState NewsServerState;
		pCommunication->GetNewsServerState(NewsServerState);

		if (bConnected) 
		{
			StrTemp.Format(L"连接资讯: %s:%i \n连接时长: %s \n", StrAddress.GetBuffer(), uiPort, GetTime(uiKeepSeconds).GetBuffer());
// 			StrAddress.ReleaseBuffer();
// 			GetTime(uiKeepSeconds).ReleaseBuffer();

			m_StrTips += StrTemp;	
			bAllNewsNonConnect = false;
		}
	}

	m_bConnectNews = !bAllNewsNonConnect;

	//if ( aNewsServers.GetSize() <= 0 )
	//{
	//	m_StrTips += L"未连接资讯服务器\n";
	//}

	WINDOWPLACEMENT wp;	
	::GetWindowPlacement(AfxGetMainWnd()->GetSafeHwnd(), &wp);
	if ( SW_SHOWMINIMIZED == wp.showCmd || SW_MINIMIZE == wp.showCmd )
	{
		return;
	}

	CPoint pt = m_RectNetwork.TopLeft();
	ClientToScreen(&pt);

	const DWORD dwEUCCFs = pMainFrame->GetUserConnectCmdFlag();
	bool32 bUserConnectDataServer = CheckFlag(dwEUCCFs, EUCCF_ConnectDataServer);
	bool32 bUserConnectNewsServer = CheckFlag(dwEUCCFs, EUCCF_ConnectNewsServer);
	
	if ( aNewsServers.GetSize() > 0 && bAllNewsNonConnect &&
		 aQuoteServers.GetSize() > 0 && bAllQuoteNonConnect )
	{
		if ( !bUserConnectDataServer && !bUserConnectNewsServer )
		{
			m_StrTips = L"所有服务器连接已经中断!";		// 用户强制要求的
			if ( m_mapNetworkNotifyCount[EUCCF_ConnectDataServer] < 3 )
			{
				m_mapNetworkNotifyCount[EUCCF_ConnectDataServer] = 3;		// 如果以前没通知过，则通知一次
			}
			if ( m_mapNetworkNotifyCount[EUCCF_ConnectNewsServer] < 3 )
			{
				m_mapNetworkNotifyCount[EUCCF_ConnectNewsServer] = 3;
			}
		}
		else
		{
			m_StrTips = L"所有服务器连接中断, 请检查网络状况!";
		}

		// 通知次数取最小值
		const int32 iNotifyCount = min(m_mapNetworkNotifyCount[EUCCF_ConnectDataServer], m_mapNetworkNotifyCount[EUCCF_ConnectNewsServer]);
		
		if ( iNotifyCount <= 3 )
		{		
			m_mapNetworkNotifyCount[EUCCF_ConnectDataServer]++;
			m_mapNetworkNotifyCount[EUCCF_ConnectNewsServer]++;

			CBalloonMsg::RequestCloseAll();
			if (m_bShowNetworDetailNotity)			
			{
				CBalloonMsg::Show(L"连接中断", m_StrTips, (HICON)3, &pt);
			}
			
		}
	}
	else if ( aQuoteServers.GetSize() > 0 && bAllQuoteNonConnect )
	{
		if ( !bUserConnectDataServer )
		{
			m_StrTips = L"所有行情服务器连接已经中断!";
			if ( m_mapNetworkNotifyCount[EUCCF_ConnectDataServer] < 3 )
			{
				m_mapNetworkNotifyCount[EUCCF_ConnectDataServer] = 3;	// 最多通知一次
			}
		}
		else
		{
			m_StrTips = L"所有行情服务器连接中断, 请检查网络状况!";
		}

		if ( aNewsServers.GetSize() > 0 && bAllNewsNonConnect  )
		{
			// 此时资讯连接上了，则资讯记数清零
			m_mapNetworkNotifyCount[EUCCF_ConnectNewsServer] = 0;
		}

		const int32 iNotifyCount = m_mapNetworkNotifyCount[EUCCF_ConnectDataServer];
		if ( iNotifyCount <= 3 )
		{
			m_mapNetworkNotifyCount[EUCCF_ConnectDataServer]++;

			CBalloonMsg::RequestCloseAll();
			if (m_bShowNetworDetailNotity)
			{
				CBalloonMsg::Show(L"行情连接中断", m_StrTips, (HICON)3, &pt);
			}
		
		}
	}
	else if ( aNewsServers.GetSize() > 0 && bAllNewsNonConnect )
	{
		// 此时行情必连接上了
		m_mapNetworkNotifyCount[EUCCF_ConnectDataServer] = 0;

		if ( !bUserConnectNewsServer )
		{
			m_StrTips = L"所有资讯服务器连接已经中断!";
			if ( m_mapNetworkNotifyCount[EUCCF_ConnectNewsServer] < 3 )
			{
				m_mapNetworkNotifyCount[EUCCF_ConnectNewsServer] = 3;	// 最多通知一次
			}
		}
		else
		{
			m_StrTips = L"所有资讯服务器连接中断, 请检查网络状况!";
		}
		

		if ( m_mapNetworkNotifyCount[EUCCF_ConnectNewsServer] <= 3 )
		{
			m_mapNetworkNotifyCount[EUCCF_ConnectNewsServer]++;
			CBalloonMsg::RequestCloseAll();
			if (m_bShowNetworDetailNotity)
			{
				CBalloonMsg::Show(L"资讯连接中断", m_StrTips, (HICON)3, &pt);		
			}
		
		}
	}
	else
	{
		// 无报警清0
		m_mapNetworkNotifyCount[EUCCF_ConnectDataServer] = 0;
		m_mapNetworkNotifyCount[EUCCF_ConnectNewsServer] = 0;
	}

#ifdef _DEBUG
	{
		CGmtTime TimeCur = pDoc->m_pAbsCenterManager->GetServerTime();
		tm tmLocal, tmGmt;
		TimeCur.GetGmtTm(&tmGmt);
		TimeCur.GetLocalTm(&tmLocal);
		CString StrTime;
		StrTime.Format(_T("")
			_T("本地时间：%04d-%02d-%02d %02d:%02d:%02d\r\n")
			_T("格林威治：%04d-%02d-%02d %02d:%02d:%02d"),
			1900 + tmLocal.tm_year, tmLocal.tm_mon + 1, tmLocal.tm_mday, tmLocal.tm_hour, tmLocal.tm_min, tmLocal.tm_sec
			, 1900 + tmGmt.tm_year, tmGmt.tm_mon + 1, tmGmt.tm_mday, tmGmt.tm_hour, tmGmt.tm_min, tmGmt.tm_sec
			);
		m_StrTips += StrTime;
	}
#endif
}

void CStatusBarEx::PromptLongTimeNoRecvData(bool bErr /*= true*/)
{
	if ( bErr )
	{
		CPoint pt = m_RectNetwork.TopLeft();
		ClientToScreen(&pt);
		
		m_StrTips = L"自动重连中...\n长时间未收到数据, 请检查网络!";
		CBalloonMsg::RequestCloseAll();
		CBalloonMsg::Show(L"网络状况不稳定", m_StrTips, (HICON)3, &pt);
	}
	else
	{
		if ( L"自动重连中...\n长时间未收到数据, 请检查网络!" == m_StrTips )
		{
			CBalloonMsg::RequestCloseAll();
		}
	}	
}

void CStatusBarEx::PromptSomePackOutTime(bool bErr /*= true*/)
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if ( !pApp->m_pConfigInfo->m_bPromptNet )
	{
		return;
	}

	//
	if ( bErr )
	{
		if ( L"与服务器连接不流畅!" != m_StrTips )
		{
			CPoint pt = m_RectNetwork.TopLeft();
			ClientToScreen(&pt);
			
			m_StrTips = L"与服务器连接不流畅!";
			CBalloonMsg::RequestCloseAll();
			CBalloonMsg::Show(L"网络状况不稳定", m_StrTips, (HICON)2, &pt);
		}		
	}
	else
	{
		if ( L"与服务器连接不流畅!" == m_StrTips )
		{			
			CBalloonMsg::RequestCloseAll();		
		}
	}	
}

void CStatusBarEx::HideTips()
{

}

void CStatusBarEx::OnTimer(UINT nIDEvent)
{
	if ( KTimeNetPintPaint == nIDEvent)
	{	
		m_iUpdateTimeCounter--;
		if (m_iUpdateTimeCounter < 0)
		{
			UpdateTime();

			m_iUpdateTimeCounter = KUpdateTimeCount;
		}

		if ( !CGGTongApp::m_bOffLine )
		{
			m_iUpdateNetworkPicCounter--;
			if (m_iUpdateNetworkPicCounter < 0)
			{
				UpdateNetworkPic();
				
				m_iUpdateNetworkPicCounter = KUpdateNetworkPicCount;
			}
			
			m_iUpdateNetworkDetailCounter--;
			if (m_iUpdateNetworkDetailCounter < 0)
			{
				UpdateNetworkDetail();
				
				m_iUpdateNetworkDetailCounter = KUpdateNetworkDetailCount;
			}
		}

		// 刷新
		//Invalidate();
		RefreshSpecifiedArea();
	}
	else if ( KTimerIdSBUpateOrg == nIDEvent )
	{
		//auth::iProxyAuthClient *pAuth = GetAuthManager();
		//if ( NULL!=pAuth )
		//{
		//	USES_CONVERSION;
		//	pAuth->ReqOrgDataUpdate( W2A(CConfigInfo::Instance()->GetOrgKey()) );
		//}
	}
	else if ( KTimerIdTodayComment == nIDEvent ) 
	{
		//金评闪烁
		CGGTongDoc* pDoc = AfxGetDocument();
		if ( pDoc->m_pAbsCenterManager->GetTodayFlag() )
		{
			if ( m_pImageTodayCommentBlue == m_pImageTodayCommentOn )
			{
				m_pImageTodayCommentOn = m_pImageTodayCommentRed;
			}
			else
			{
				m_pImageTodayCommentOn = m_pImageTodayCommentBlue;
			}
		}
	}
	else if ( KTimerIdImId == nIDEvent )
	{
		//IM消息闪烁
		// modify by tangad 今评功能已去掉
		/*CGGTongDoc* pDoc = AfxGetDocument();
		if ( pDoc->m_pAbsCenterManager->GetMessageFlag() )
		{
			if ( m_pIMageMessageHide == m_pImageMessage )
			{
				m_pImageMessage = m_pImageMessageShow;
			}
			else
			{
				m_pImageMessage = m_pIMageMessageHide;
			}
		}*/
	}
}

void CStatusBarEx::CreateSpiriteDialog()
{
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	ASSERT(NULL != pWnd );
	if ( pWnd->m_pDlgSpirite ) return;

	pWnd->m_pDlgSpirite = new CDlgSpirite;
	pWnd->m_pDlgSpirite->Create(IDD_DIALOG_SPIRITE,AfxGetMainWnd());
	pWnd->m_pDlgSpirite->ShowWindow(SW_SHOW);	
}

void CStatusBarEx::CreateAlarmDialog(bool32 bShow /*= true*/)
{
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	if ( NULL == pWnd )
	{
		return;
	}

	//
	if ( pWnd->m_pDlgAlarm )
	{
		if (pWnd->m_pDlgAlarm->IsWindowVisible())
		{
			pWnd->m_pDlgAlarm->ShowWindow(SW_HIDE);
		}
		else
		{
			pWnd->m_pDlgAlarm->ShowWindow(SW_SHOW);		
		}		
	}
	else
	{
		pWnd->m_pDlgAlarm = new CDlgAlarm;
		pWnd->m_pDlgAlarm->Create(IDD_DIALOG_ALARM,AfxGetMainWnd());
		if ( bShow )
		{
			pWnd->m_pDlgAlarm->ShowWindow(SW_SHOW);
		}		
	}
}

void CStatusBarEx::CreateRecentMerchDialog()
{
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	ASSERT(NULL != pWnd );
	if ( pWnd->m_pDlgRecentMerch ) return;
	
	pWnd->m_pDlgRecentMerch = new CDlgRecentMerch;
	pWnd->m_pDlgRecentMerch->Create(IDD_DIALOG_RECENTMERCH,AfxGetMainWnd());
	pWnd->m_pDlgRecentMerch->ShowWindow(SW_SHOW);
}

void CStatusBarEx::CreateIndexPromptDialog()
{
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	ASSERT(NULL != pWnd );
	if ( pWnd->m_pDlgIndexPrompt ) return;
	
	pWnd->m_pDlgIndexPrompt = new CDlgIndexPrompt;
	pWnd->m_pDlgIndexPrompt->Create(IDD_DIALOG_INDEX_PROMPT, AfxGetMainWnd());
	pWnd->m_pDlgIndexPrompt->ShowWindow(SW_SHOW);	
}

void CStatusBarEx::CreateNetWorkDialog()
{
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	ASSERT(NULL != pWnd );
	if ( pWnd->m_pDlgNetWork ) return;

	pWnd->m_pDlgNetWork = new CDlgNetWork;
	pWnd->m_pDlgNetWork->Create(IDD_DIALOG_NETWORK,AfxGetMainWnd());
	pWnd->m_pDlgNetWork->ShowWindow(SW_SHOW);	

}

//如果创建财富资讯，已有就打开 2013-7-23
void CStatusBarEx::CreateNewsView()
{
	CGGTongDoc* pDoc = AfxGetDocument();
	if ( NULL == pDoc || NULL == pDoc->m_pAbsCenterManager )
	{
		return;
	}

	if ( !pDoc->m_pAbsCenterManager->GetTodayFlag() )
		return;

	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	ASSERT(NULL != pWnd );

	pWnd->CreateIoViewByPicMenuID(ID_PIC_NEWS, true);
	pDoc->m_pAbsCenterManager->SetTodayFlag(false);
}

//显示IM 2013-10-31 add by cym
void CStatusBarEx::ShowImMessage()
{
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	if ( NULL != pWnd)
	{
		pWnd->OnShowImMessage();
	}

	return;
}





void CStatusBarEx::OnDestroy() 
{	
	auth::iProxyAuthClient *pAuth = GetAuthManager();
	if ( NULL!=pAuth )
	{
//		pAuth->SetUserDataNotify(NULL);
	}

//	StopTimer(BBSTIMER);	// zhangbo 1028 #//
 	KillTimer(KTimeNetPintPaint);

	SaveToXml();

	m_TipWnd.DestroyWindow();
	CStatusBar::OnDestroy();	
}

int32 CStatusBarEx::GetCurSel()
{
	return m_iCurSel;
}

int32 CStatusBarEx::GetStatusBarHeight()
{
	return m_iHeight;
}

void CStatusBarEx::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ( !m_bMouseTrack )
	{
		TRACKMOUSEEVENT tme = {0};
		tme.cbSize  = sizeof(tme);
		tme.dwFlags = TME_HOVER | TME_LEAVE; // 停留标志 与 离开标志
		tme.dwHoverTime = 400; // 400 ms - 调用Track会重置Hover计时
		tme.hwndTrack = m_hWnd;
		m_bMouseTrack = _TrackMouseEvent(&tme);
	}

	// 检查是否悬浮在广告上面
	bool32 bInAdv = GetAdvIndexByPoint(point) >= 0;
	m_ptLastMouseMove = point;

	SetCursor(LoadCursor(NULL, bInAdv ? MAKEINTRESOURCE(32649)/*IDC_HAND*/ : IDC_ARROW));

	CStatusBar::OnMouseMove(nFlags, point);

	return;
}

LRESULT CStatusBarEx::OnMouseHover( WPARAM nFlags, LPARAM point )
{
	m_bMouseTrack = FALSE;
	CPoint pt;
	pt.y = HIWORD(point);
	pt.x = LOWORD(point);
	if ( m_RectNetwork.PtInRect(pt))
	{
		CPoint ptCorner = m_RectNetwork.TopLeft();
		ClientToScreen(&ptCorner);
		
		WINDOWPLACEMENT wp;	
		::GetWindowPlacement(AfxGetMainWnd()->GetSafeHwnd(), &wp);
		if ( SW_SHOWMINIMIZED == wp.showCmd || SW_MINIMIZE == wp.showCmd )
		{
			//NULL;
		}
		else
		{
			CBalloonMsg::RequestCloseAll();		
			CBalloonMsg::Show(L"网络连接", m_StrTips, HICON(1), &ptCorner);			
		}
	}
	else if ( m_RectTime.PtInRect(pt) )
	{
// #ifdef DEBUG
		// 显示获取的服务器时间
		CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
		CGGTongDoc *pDocument = pApp->m_pDocument;	
		ASSERT(NULL != pDocument);
		if ( NULL != pDocument )
		{
			CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
			ASSERT(NULL != pAbsCenterManager);
			if ( NULL != pAbsCenterManager )
			{
				CGmtTime TimeCur = pAbsCenterManager->GetServerTime();
				tm tmLocal, tmGmt;
				TimeCur.GetGmtTm(&tmGmt);
				TimeCur.GetLocalTm(&tmLocal);
				CString StrTime;
				StrTime.Format(_T("")
							   _T("本地时间：%04d-%02d-%02d %02d:%02d:%02d\r\n")
							   _T("格林威治：%04d-%02d-%02d %02d:%02d:%02d"),
							   1900 + tmLocal.tm_year, tmLocal.tm_mon + 1, tmLocal.tm_mday, tmLocal.tm_hour, tmLocal.tm_min, tmLocal.tm_sec
								, 1900 + tmGmt.tm_year, tmGmt.tm_mon + 1, tmGmt.tm_mday, tmGmt.tm_hour, tmGmt.tm_min, tmGmt.tm_sec
								);
				CBalloonMsg::RequestCloseAll();	
				ClientToScreen(&pt);
				CBalloonMsg::Show(L"时间", StrTime, HICON(1), &pt);
			}
		}
// #endif
	}

	return ::DefWindowProc(m_hWnd, WM_MOUSEHOVER, nFlags, point);
}

LRESULT CStatusBarEx::OnMouseLeave(WPARAM w, LPARAM l)
{
	m_bMouseTrack = FALSE;
	//CBalloonMsg::RequestCloseAll();
	m_TipWnd.Hide();

	m_ptLastMouseMove = CPoint(-1, -1);

	return ::DefWindowProc(m_hWnd, WM_MOUSELEAVE, w, l);
}

void CStatusBarEx::GetStatusBarMerchs( OUT MerchArray &aMerchs )
{
	aMerchs.RemoveAll();
	CGGTongDoc *pDoc = AfxGetDocument();
	if (NULL != pDoc && NULL != pDoc->m_pAbsCenterManager && m_ciBarMerchCount == sizeof(m_BarMerchKind)/sizeof(m_BarMerchKind[0]))
	{
		for ( int32 i=0; i < m_ciBarMerchCount ; i++ )
		{
			T_BarMerch& barMerch = m_BarMerchKind[i];		
			if ( !pDoc->m_pAbsCenterManager->GetMerchManager().FindMerch(barMerch.m_StrCode, barMerch.m_iMarket, barMerch.m_pMerch) )
			{
				ASSERT( 0 );
			}
			aMerchs.Add( m_BarMerchKind[i].m_pMerch ); // 可能为NULL
		}
	}	
}

void CStatusBarEx::SendStatusBarMerchs(IN MerchArray &aMerchs)
{

	CGGTongDoc *pDocument = AfxGetDocument();	
	ASSERT(NULL != pDocument);
	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL != pAbsCenterManager);
	if ( NULL == pAbsCenterManager )
	{
		return;
	}

	pAbsCenterManager->RemoveAttendMerch(EA_StatucBar);
	for ( int i=0; i < aMerchs.GetSize() ; i++ )
	{
		if ( NULL == aMerchs[i] )
		{
			continue;
		}
		CSmartAttendMerch SmartAttendMerch;
		SmartAttendMerch.m_pMerch = aMerchs[i];
		SmartAttendMerch.m_iDataServiceTypes = EDSTPrice;

		pAbsCenterManager->AddAttendMerch(SmartAttendMerch,EA_StatucBar);
	}

	pAbsCenterManager->RequestStatusBarViewData();
}


void CStatusBarEx::LoadFromXml()
{
	CGGTongDoc *pDocument = AfxGetDocument();	
	ASSERT(NULL != pDocument);
	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL != pAbsCenterManager);
	if ( NULL == pAbsCenterManager )
	{
		return;
	}

	CString StrFileName;
	if ( CPathFactory::GetExistConfigFileName(StrFileName, KStrXmlFileName) )
	{
		string sPathName = _Unicode2MultiChar(StrFileName);
		const char* strFilePath = sPathName.c_str();
		TiXmlDocument tiDoc(strFilePath);
		if ( tiDoc.LoadFile() )
		{
			TiXmlElement *pRoot = tiDoc.FirstChildElement(GetXmlRootElement());
			ASSERT( NULL != pRoot );
			if ( NULL != pRoot )
			{
				TiXmlElement *pEle = pRoot->FirstChildElement(KStrXmlMerch);
				while (NULL != pEle)
				{
					const char *pValue = NULL;
					pValue = pEle->Attribute(KStrXmlOrderId);
					int iOrder = -1;
					if ( NULL != pValue)
					{
						iOrder=atoi(pValue);
					}

					if ( iOrder >= m_ciBarMerchCount || iOrder < 0)
					{
						ASSERT( 0 );
						pEle = pEle->NextSiblingElement(KStrXmlMerch);  //移动到下一个节点，避免死循环
						continue;		// 不能接受没有序号的
					}

					const char *pcszCode, *pcszName, *pcszMarket;
					pcszCode = pcszName = pcszMarket = NULL;
					pcszCode = pEle->Attribute(KStrXmlMerchCode);
					pcszName = pEle->Attribute(KStrXmlMerchName);
					pcszMarket = pEle->Attribute(KStrXmlMarketId);

					int32 iMarket = -1;
					if ( NULL == pcszCode || NULL == pcszName || NULL == pcszMarket  )
					{
						ASSERT( 0 );
						pEle = pEle->NextSiblingElement(KStrXmlMerch); //移动到下一个节点，避免死循环
						continue;
					}
					iMarket = atoi(pcszMarket);
					
					// 查找商品
					T_BarMerch &barMerch = m_BarMerchKind[iOrder];
					barMerch.m_StrName = _A2W(pcszName);
					barMerch.m_StrCode = _A2W(pcszCode);
					barMerch.m_iMarket = iMarket;
					barMerch.m_pMerch = NULL;

					pEle = pEle->NextSiblingElement(KStrXmlMerch);
				}
			}
		}
		else
		{
			ASSERT( 0 );
		}
	}

	// 查找商品
	for ( int i=0; i < m_ciBarMerchCount ; i++ )
	{
		T_BarMerch &barMerch = m_BarMerchKind[i];

		m_RectMerchKind[i].m_StrName = barMerch.m_StrName;		// 变更其名称

		if ( !pAbsCenterManager->GetMerchManager().FindMerch(barMerch.m_StrCode, barMerch.m_iMarket, barMerch.m_pMerch) )
		{
			TRACE(_T("Ignore status bar merch: %s{%d-%s}\r\n"), barMerch.m_StrName.GetBuffer(), barMerch.m_iMarket, barMerch.m_StrCode.GetBuffer());
			barMerch.m_StrName.ReleaseBuffer();
			barMerch.m_StrCode.ReleaseBuffer();
		}
	}
}

void CStatusBarEx::SaveToXml()
{
	CString StrFileName = CPathFactory::GetSaveConfigFileName(KStrXmlFileName);
	TiXmlDocument tiDoc;
	TiXmlElement *pRoot = ConstructGGTongAppXmlDocHeader(tiDoc, "StatusBarMerch", NULL, NULL, NULL);
	if ( NULL != pRoot )
	{
		char buf[1024]; // - -
		for ( int i=0; i < m_ciBarMerchCount ; i++ )
		{
			const T_BarMerch &barMerch = m_BarMerchKind[i];
			
			TiXmlElement tiEle(KStrXmlMerch);
			_snprintf(buf, sizeof(buf), "%d", i);
			tiEle.SetAttribute(KStrXmlOrderId, buf);

			tiEle.SetAttribute(KStrXmlMerchName, _W2A(barMerch.m_StrName));

			_snprintf(buf, sizeof(buf), "%d", barMerch.m_iMarket);
			tiEle.SetAttribute(KStrXmlMarketId, buf);
			tiEle.SetAttribute(KStrXmlMerchCode, _W2A(barMerch.m_StrCode));
			pRoot->InsertEndChild(tiEle);
		}
		SaveTiXmlDoc(tiDoc, StrFileName);
	}
}

LRESULT CStatusBarEx::OnMsgHotKey( WPARAM w, LPARAM l )
{
	CHotKey *pHotKey = (CHotKey *)w;
	ASSERT(NULL != pHotKey);
	
	if (  EHKTMerch == pHotKey->m_eHotKeyType && m_iBarMerchCur >= 0 && m_iBarMerchCur < m_ciBarMerchCount )
	{
		T_BarMerch bar;
		T_BarMerch *pBar = &bar;
		pBar->m_StrCode = pHotKey->m_StrKey;
		pBar->m_StrName = pHotKey->m_StrSummary;
		pBar->m_iMarket = pHotKey->m_iParam1;
		pBar->m_pMerch = NULL;
		int32 iCurBar = m_iBarMerchCur;

		// 由于出现对话框导致keydlg消失，发送dlgclose消息过来，所以需要先保存一下barMerchCur
		if ( CDlgInputShowName::GetInputString(pBar->m_StrName, _T("请输入商品显示名称"), _T("状态栏商品名称")) == IDOK )
		{
			// 已经变更
		}
		
		UpdateStatusBarMerch(iCurBar, bar);
	}
	else
	{
		ASSERT( 0 );
		// 现在不知道到哪里了
		CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
		if ( NULL != pMainFrame )
		{
		}
	}
	
	DEL(pHotKey);
	return 1;
}

LRESULT CStatusBarEx::OnMsgDlgHotKeyClose( WPARAM w, LPARAM l )
{
	m_iBarMerchCur = -1;		// 已经失去目标
	// 已经关闭，不需要处理，hotkeydlg已经自己把mainFrame中的清空了
	// 没有清空了- -，自己动手，丰衣足食
	
	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if ( NULL != pMainFrame )
	{
		pMainFrame->SetHotkeyTarget();
	}
	return 1;	
}

void CStatusBarEx::UpdateStatusBarMerch( int32 iIndex, const T_BarMerch &barMerch )
{
	if ( iIndex < 0 || iIndex >= m_ciBarMerchCount )
	{
		ASSERT( 0 );
		return;
	}
	ASSERT( m_ciBarMerchCount == sizeof(m_RectMerchKind)/sizeof(m_RectMerchKind[0]) );

	T_BarMerch *pBar = m_BarMerchKind + iIndex;
	*pBar = barMerch;
	m_RectMerchKind[iIndex].m_StrName = pBar->m_StrName;	// 显示名称
	CGGTongDoc *pDoc = AfxGetDocument();
	ASSERT( NULL != pDoc );
	if ( !pDoc->m_pAbsCenterManager->GetMerchManager().FindMerch(pBar->m_StrCode, pBar->m_iMarket, pBar->m_pMerch) )
	{
		ASSERT( 0 );
	}
	MerchArray aMerchs;
	GetStatusBarMerchs(aMerchs);
	SendStatusBarMerchs(aMerchs);

	OnRealTimePriceUpdate();
	//Invalidate();
	RefreshSpecifiedArea();

	SaveToXml();	// 保存
}

void CStatusBarEx::OnRealTimePriceUpdate()
{
	MerchArray aMerchs;
	GetStatusBarMerchs(aMerchs);
	ASSERT( aMerchs.GetSize() == sizeof(m_RectMerchKind)/sizeof(m_RectMerchKind[0]) );
	for ( int i=0; i < aMerchs.GetSize() ; i++ )
	{
		CRectData &RectData = m_RectMerchKind[i];
		if ( NULL == aMerchs[i] || NULL == aMerchs[i]->m_pRealtimePrice )
		{
			RectData.m_StrNow	  = L"-";
			RectData.m_StrCompare = L"-";
			RectData.m_StrAmount  = L"-";
		}
		else
		{
			CMerch *pMerch = aMerchs[i];
		//	RectData.m_StrNow.Format(L"%.2f",pMerch->m_pRealtimePrice->m_fPriceNew);
			RectData.m_StrNow = Float2SymbolString(pMerch->m_pRealtimePrice->m_fPriceNew,pMerch->m_pRealtimePrice->m_fPriceNew,pMerch->m_MerchInfo.m_iSaveDec);
				
			RectData.m_StrCompare = L" ";
			
			if (0 != pMerch->m_pRealtimePrice->m_fPriceNew && 0 != pMerch->m_pRealtimePrice->m_fPricePrevClose)
			{
				float fCompare		= pMerch->m_pRealtimePrice->m_fPriceNew - pMerch->m_pRealtimePrice->m_fPricePrevClose;
				RectData.m_StrCompare = Float2SymbolString(fCompare,0.0,pMerch->m_MerchInfo.m_iSaveDec);
				
				// 涨跌幅
				float fRisePercent = ((fCompare) / pMerch->m_pRealtimePrice->m_fPricePrevClose) * 100.;
				RectData.m_StrAmount = Float2SymbolString(fRisePercent, 0, 2, false, false, true);
				// 成交额
 				//Float2StringLimitMaxLength(RectData.m_StrAmount
 				//	, pMerch->m_pRealtimePrice->m_fAmountTotal
 				//	, 0
 				//	, 7
 				//	, true
 				//	, true);
			}
			else if ( 0. == pMerch->m_pRealtimePrice->m_fPriceNew && 0. != pMerch->m_pRealtimePrice->m_fPricePrevClose )
			{
				// 开盘之后, 集合竞价之前. 显示昨收价, 其他两个字段为空
				RectData.m_StrCompare = L"-";
				RectData.m_StrAmount  = L"-";

				//
			//	RectData.m_StrNow.Format(L"%.2f", pMerch->m_pRealtimePrice->m_fPricePrevClose);
				RectData.m_StrNow = Float2SymbolString(pMerch->m_pRealtimePrice->m_fPricePrevClose,pMerch->m_pRealtimePrice->m_fPricePrevClose,pMerch->m_MerchInfo.m_iSaveDec);
			}
			
			// TRACE(L"statusbar %s %.2f %.2f\n", pMerch->m_MerchInfo.m_StrMerchCnName, pMerch->m_pRealtimePrice->m_fPriceNew, pMerch->m_pRealtimePrice->m_fPricePrevClose);
		}
	}
}

CSize CStatusBarEx::GetProperBBSSizeShow()
{
	CSize SizeShow(0, 0);
	CClientDC dc(this);
	LOGFONT FontStatus;
	InitStatusBBSLogFont(FontStatus);
	CFont FontSts;
	FontSts.CreateFontIndirect(&FontStatus);
	CFont *pOldFont = dc.SelectObject(&FontSts);
	const int32 iSeparate = KIBBSSeparate;

	int32 i = 0;
	for ( i=0; i < m_aOrgAdvs.GetSize() ; i++ )
	{
		CSize SizeText = dc.GetTextExtent(m_aOrgAdvs[i].m_StrTitle);
		SizeShow.cx += SizeText.cx;
		SizeShow.cx += iSeparate;
	}
	dc.SelectObject(pOldFont);

	if ( i == 0 )
	{
		// 没有公告, holyshit
		return m_RectBBS.Width();	// 返回默认的
	}
	else
	{
		SizeShow.cx -= iSeparate;	// 剔除最后的一个分割
	}

	return SizeShow;
}

void CStatusBarEx::InitStatusBBSLogFont( OUT LOGFONT &lf )
{
	memset(&lf, 0, sizeof(lf));
	
	_tcscpy(lf.lfFaceName , gFontFactory.GetExistFontName(L"宋体"));///
   	lf.lfHeight  = -12;
   	lf.lfWeight  = 400;
   	lf.lfCharSet = 0;
 	lf.lfOutPrecision = 3;
}

BOOL CStatusBarEx::OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message )
{
	if ( message == WM_MOUSEMOVE )
	{
		return TRUE;
	}
	return CStatusBar::OnSetCursor(pWnd, nHitTest, message);
}

int32 CStatusBarEx::GetAdvIndexByPoint( CPoint pt )
{
	for ( int32 i=0; i < m_aOrgAdvRects.GetSize() ; i++ )
	{
		if ( m_aOrgAdvRects[i].PtInRect(pt) )
		{
			return m_aOrgAdvRects[i].iOrgIndex;
		}
	}
	return -1;
}

void CStatusBarEx::OnDuoKongGeneralNormalUpdate()
{
	CAbsCenterManager *pAbsCenterManager = NULL;
	CGGTongDoc *pDoc = AfxGetDocument();
	pAbsCenterManager = NULL != pDoc ? pDoc->m_pAbsCenterManager : NULL;
	if ( NULL == pAbsCenterManager )
	{
		ASSERT( 0 );
		return;
	}
	int32 iIndex = 0;
	// 	多空阵线算法：
	// 		1）红绿方块部分		红色：瞬间上涨家数/(瞬间上涨家数+瞬间下跌家数)	绿色：瞬间下跌家数/(瞬间上涨家数+瞬间下跌家数)
	// 		2）箭头和等号部分
	// 		假设N=当前的涨家数-跌家数，M=上一次的涨家数-跌家数
	// 		涨家数>跌家数时
	// 			如N>M  标记红上  	如N<M  标记红下	如N=M  标记红等
	// 		涨家数<跌家数时
	// 			如N>M  标记绿上     如N<M  标记绿下	如N=M  标记绿等
	// 		涨家数=跌家数时
	// 			如N>M  标记白上 	如N<M  标记白下	如N=M  标记白等

	bool32 bUpdate = false;
	// 上海
	const CAbsCenterManager::GeneralNormalArray &aGNH = pAbsCenterManager->GetGeneralNormalArrayH();
	T_GeneralNormal DataNewH;
	if ( aGNH.GetSize() > 0 )
	{
		DataNewH = aGNH[aGNH.GetUpperBound()];
	}
	int32 iRiseNew, iRiseOld;
	if ( DataNewH.m_lTime > m_LastOldDuoKongH.m_lTime
		|| (DataNewH.m_lTime == m_LastOldDuoKongH.m_lTime
		&& 0 != memcmp(&DataNewH, &m_LastOldDuoKongH, sizeof(DataNewH))))
	{
		// 仅接受比最后时间更新的数据或者时间相同，但是数据不同。 是否有可能新数据的时间精细度不够
		iRiseNew = (int32)DataNewH.m_uiRiseMomentary - (int32)DataNewH.m_uiFallMomentary;
		iRiseOld = (int32)m_LastOldDuoKongH.m_uiRiseMomentary - (int32)m_LastOldDuoKongH.m_uiFallMomentary;
		if ( iRiseNew > 0 )
		{
			iIndex += 6;	// 红
		}
		else if ( 0 == iRiseNew )
		{
			iIndex += 3;	// 白
		}
		
		if ( iRiseNew > iRiseOld )
		{
			iIndex += 2;
		}
		else if ( iRiseNew == iRiseOld )
		{
			iIndex += 1;
		}
		m_aDuoKongImageH.RemoveAt(0);	// 添加新标志
		m_aDuoKongImageH.Add(iIndex);
		m_LastOldDuoKongH = DataNewH;

		bUpdate = true;
	}


	// 深圳
	iIndex = 0;
	const CAbsCenterManager::GeneralNormalArray &aGNS = pAbsCenterManager->GetGeneralNormalArrayS();
	T_GeneralNormal DataNewS;
	if ( aGNS.GetSize() > 0 )
	{
		DataNewS = aGNS[aGNS.GetUpperBound()];
	}
	if ( DataNewS.m_lTime > m_LastOldDuoKongS.m_lTime
		|| (DataNewS.m_lTime == m_LastOldDuoKongS.m_lTime
		&& 0 != memcmp(&DataNewS, &m_LastOldDuoKongS, sizeof(DataNewS))))
	{
		// 仅接受比最后时间更新的数据或者时间相同，但是数据不同。 是否有可能新数据的时间精细度不够
		iRiseNew = (int32)DataNewS.m_uiRiseMomentary - (int32)DataNewS.m_uiFallMomentary;
		iRiseOld = (int32)m_LastOldDuoKongS.m_uiRiseMomentary - (int32)m_LastOldDuoKongS.m_uiFallMomentary;
		if ( iRiseNew > 0 )
		{
			iIndex += 6;	// 红
		}
		else if ( 0 == iRiseNew )
		{
			iIndex += 3;	// 白
		}
		
		if ( iRiseNew > iRiseOld )
		{
			iIndex += 2;
		}
		else if ( iRiseNew == iRiseOld )
		{
			iIndex += 1;
		}
		m_aDuoKongImageS.RemoveAt(0);	// 添加新标志
		m_aDuoKongImageS.Add(iIndex);
		m_LastOldDuoKongS = DataNewS;

		bUpdate = true;
	}

	if ( bUpdate )
	{
		//Invalidate();
		RefreshSpecifiedArea();
	}
}



void CStatusBarEx::GetKeyBoarEditRect(CRect &rc)
{
	rc = m_KeyBoarEditRect;
}

void CStatusBarEx::SetKeyBoarEditText(CString szDisData)
{
	m_KeyBoarEdit.SetWindowText(szDisData);
}



BOOL CStatusBarEx::PreTranslateMessage( MSG* pMsg )
{
	//也将消息同步给键盘精灵处理
	if ( WM_KEYDOWN == pMsg->message)
	{
		CMainFrame *pParent = (CMainFrame*)GetParent();

		if (pParent == NULL || pParent->m_pDlgKeyBoard == NULL)
		{
			return CStatusBar::PreTranslateMessage(pMsg);;
		}

		pParent->m_pDlgKeyBoard->PreTranslateMessage(pMsg);
	}

	return CStatusBar::PreTranslateMessage(pMsg);
}




//////////////////////////////////////////////////////////////////////////
// CRectData 函数:
  
void CRectData::DrawRect( CDC* pDC,CRectData& rectData )
{
	if ( rectData.Width() <= 0 || rectData.Height() <= 0 )
	{
		return;
	}
	// 填充背景色:
	Graphics GraphicImage(pDC->GetSafeHdc());
	
	if ( NULL == m_pParent )
	{				
		return;
	}

	Image* pImage = m_pParent->m_pImageBkGround;
	/*if ( NULL != pImage )
	{
		DrawImage(GraphicImage, pImage, rectData, 1, 0, true);
	}
	else
	{
		pDC->FillSolidRect(rectData, CFaceScheme::Instance()->GetSysColor(ESCBackground));
	}*/

	// 上证指数:
	CRect RectName,RectNow,RectCompare,RectAmount;
	
	CSize SizeName = pDC->GetTextExtent(m_StrName);
	
	RectName.left   = rectData.left;
	RectName.top    = rectData.top;
	RectName.right  = RectName.left + SizeName.cx+10;
	RectName.bottom = rectData.bottom;
	
 	//pDC->MoveTo(RectName.left-4, RectName.top+1);
 	//pDC->LineTo(RectName.right, RectName.top);
 	//pDC->LineTo(RectName.right, RectName.bottom);
 	//pDC->LineTo(RectName.left-4, RectName.bottom);
 	//pDC->LineTo(RectName.left-1, RectName.top);

	// pDC->Draw3dRect(RectName,GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_HIGHLIGHTTEXT));   

	/*CRect rcTemp = RectName;
	int iStart = (RectName.Height() - RectName.Width())/2 + 1;
	rcTemp.top = iStart;
	rcTemp.bottom = rcTemp.top+RectName.Width();*/
	pDC->SetTextColor(RGB(200,200,200));
	//pDC->FillSolidRect(rcTemp, RGB(70,70,70));
	pDC->DrawText(m_StrName,&RectName,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	
	// 数字的字体

	LOGFONT FontHeavy;
	memset(&FontHeavy, 0, sizeof(FontHeavy));
	
	_tcscpy(FontHeavy.lfFaceName , L"宋体");  
   	FontHeavy.lfHeight		 = -13;
   	FontHeavy.lfWeight	     = 700;
   	FontHeavy.lfCharSet		 = 0;
	FontHeavy.lfOutPrecision = 3;
	
	CFont FontNums;
	CFont* pOldFont = NULL;
	
	BOOL bOK = FontNums.CreateFontIndirect(&FontHeavy);
	if ( bOK )
	{
		pOldFont = pDC->SelectObject(&FontNums);
	}
	
	//
	enum E_State { ESRise = 0, ESFall, ESKeep };
	E_State eState = ESKeep;

	//COLORREF clrFall = RGB(0,128,0);

	// 颜色
	if (rectData.m_StrCompare.Find('-') != -1 && rectData.m_StrCompare.GetLength()>1)
	{
		eState = ESFall;
		// 下跌色
		COLORREF colFall = RGB(51,204,102);
		pDC->SetTextColor(colFall);
	}
	else if (rectData.m_StrCompare.Find('+') != -1)
	{
		eState = ESRise;
		// 上涨色
		COLORREF colRise = RGB(230,70,70);
		pDC->SetTextColor(colRise);
	}
	else
	{
		eState = ESKeep;
		// 平盘色
		COLORREF colKeep = RGB(255,255,255);
		pDC->SetTextColor(colKeep);
	}


	// 最新价:
	CSize sizeNow  = pDC->GetTextExtent(m_StrNow);

	RectNow.left   = RectName.right+5;
	RectNow.right  = RectNow.left + sizeNow.cx;
	RectNow.top    = rectData.top + 2;
	RectNow.bottom = rectData.bottom;
	
	pDC->DrawText(rectData.m_StrNow,&RectNow,DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	
	// 金额:
	//COLORREF colAmount = RGB(0, 160, 160);
	//pDC->SetTextColor(colAmount);
	//CSize sizeAmount = pDC->GetTextExtent(m_StrAmount);
	
	// 涨跌幅,颜色
 	if (rectData.m_StrAmount.Find('-') != -1 && rectData.m_StrAmount.GetLength()>1)
 	{
 		eState = ESFall;
 		// 下跌色
 		COLORREF colFall = RGB(51,204,102);
 		pDC->SetTextColor(colFall);
 	}
 	else if (rectData.m_StrAmount.Find('+') != -1)
 	{
 		eState = ESRise;
 		// 上涨色
 		COLORREF colRise = RGB(230,70,70);
 		pDC->SetTextColor(colRise);
 	}
 	else
 	{
 		eState = ESKeep;
 		// 平盘色
 		COLORREF colKeep = RGB(255,255,255);
 		pDC->SetTextColor(colKeep);
 	}
	
	CString StrAmount =  rectData.m_StrAmount.Right(rectData.m_StrAmount.GetLength()-1);
	CSize sizeAmount = pDC->GetTextExtent(rectData.m_StrAmount);

	if ( rectData.Width() - RectName.Width() - RectNow.Width() < (sizeAmount.cx + 13) )
	{
		// 显示不下了
		if ( NULL != pOldFont )
		{
			pDC->SelectObject(pOldFont);
			FontNums.DeleteObject();
		}
		
		pDC->SetTextColor(KColorText);
		return;
	}
	else
	{		
		RectAmount.right   = rectData.right-5;
		RectAmount.left    = RectAmount.right - sizeAmount.cx - 2;
		RectAmount.top     = rectData.top + 2;
		RectAmount.bottom  = rectData.bottom;	
/*	}

	if ( ESFall == eState )
	{
		COLORREF colFall = clrFall;
		pDC->SetTextColor(colFall);
		pDC->DrawText(Rect.m_StrAmount,&RectAmount,DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	}
	else if ( ESRise == eState )
	{
		COLORREF colRise = RGB(236,27,35);
		pDC->SetTextColor(colRise);*/
		pDC->DrawText(m_StrAmount,&RectAmount,DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		
	}
/*	else if ( ESKeep == eState )
	{
		COLORREF colKeep = CFaceScheme::Instance()->GetSysColor(ESCKLineKeep);
		pDC->SetTextColor(colKeep);
		pDC->DrawText(Rect.m_StrAmount,&RectAmount,DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	}
*/
	CString StrCompare =  rectData.m_StrCompare.Right(rectData.m_StrCompare.GetLength()-1);
	CSize SizeCmp = pDC->GetTextExtent(StrCompare);

	if ( rectData.Width() - RectName.Width() - RectNow.Width() - RectAmount.Width() < (16 + SizeCmp.cx) )
	{
		if ( NULL != pOldFont )
		{
			pDC->SelectObject(pOldFont);
			FontNums.DeleteObject();
		}
		
		pDC->SetTextColor(KColorText);
		
		return;
	}
	else
	{
		RectCompare.left	= RectNow.right;
		RectCompare.right	= RectAmount.left;
		RectCompare.top		= rectData.top + 2;
		RectCompare.bottom	= rectData.bottom;
	}

	if ( ESFall == eState )
	{
		COLORREF colFall = RGB(51,204,102);
		pDC->SetTextColor(colFall);
		pDC->DrawText(rectData.m_StrCompare,&RectCompare,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	else if ( ESRise == eState )
	{
		COLORREF colRise = RGB(230,70,70);
		pDC->SetTextColor(colRise);

		CPoint ptDrawArrow;
		ptDrawArrow.x = (( RectCompare.Width() - SizeCmp.cx )/2)+RectCompare.left-16;
		ptDrawArrow.y = (RectCompare.Height() - SizeCmp.cy)/2;
	
		pDC->DrawText(StrCompare,&RectCompare,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		
	}
	else if ( ESKeep == eState )
	{
		COLORREF colKeep = RGB(255,255,255);
		pDC->SetTextColor(colKeep);

		pDC->DrawText(rectData.m_StrCompare,&RectCompare,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	 	
	if ( NULL != pOldFont )
	{
		pDC->SelectObject(pOldFont);
		FontNums.DeleteObject();
	}
	
	pDC->SetTextColor(KColorText);
}

void CRectData::DrawRectRow2(CDC* pDC,CRectData& rectData )
{
	// 填充背景色:
	// 填充背景色:
	if ( NULL == m_pParent )
	{
		return;
	}

	if ( rectData.Width() <= 0 || rectData.Height() <= 0 )
	{
		return;
	}

	Graphics GraphicImage(pDC->GetSafeHdc());
	/*Image* pImage = m_pParent->m_pImageBkGround;
	if ( NULL != pImage )
	{
		DrawImage(GraphicImage, pImage, rectData, 1, 0, true);
	}
	else
	{
		pDC->FillSolidRect(rectData, CFaceScheme::Instance()->GetSysColor(ESCBackground));
	}*/
	//

	// pDC->SetTextColor(CFaceScheme::Instance()->GetSysColor(ESCText));

	CRect RectName = rectData;
	/*
	if (Rect.m_StrName == L"财富金盾")
	{
		pDC->SetTextColor(RGB(255,0,0));
	}
	else if ( Rect.m_StrName == L"最近商品")
	{
		pDC->SetTextColor(RGB(255,255,0));
	}
	*/
	RectName.left += 5;
	
// 	pDC->MoveTo(Rect.left,Rect.top);
// 	pDC->LineTo(Rect.right,Rect.top);
// 	pDC->LineTo(Rect.right,Rect.bottom -1);
// 	pDC->LineTo(Rect.left,Rect.bottom - 1);
// 	pDC->LineTo(Rect.left,Rect.top);
	 
	//
	CFont font;
	CFontFactory fontFacotry;
	font.CreateFont(-13,0,0,0,600,0,0,0,0,1,2,1,34, fontFacotry.GetExistFontName(L"微软雅黑"));//...
	CFont *pFontOld = pDC->SelectObject(&font);
	pDC->SetTextColor(RGB(200,200,200));
	pDC->DrawText(m_StrName,&RectName,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    pDC->SelectObject(pFontOld);
	pDC->SetTextColor(CFaceScheme::Instance()->GetSysColor(ESCText));
}

void CRectData::DrawRectBBS(CDC* pDC, CRectData& rectData, CRect &RectShow)
{
	if ( NULL == m_pParent )
	{
		return;
	}
	
	// 填充背景色:
	Graphics GraphicImage(pDC->GetSafeHdc());
	Image* pImage = m_pParent->m_pImageBkBBS; 
	Image* pImageLeft = m_pParent->m_pImageBkBBSLeft;
	Image* pImageRight = m_pParent->m_pImageBkBBSRight;

	ASSERT( NULL != pImageLeft && NULL != pImageRight && NULL != pImage );
	CSize sizeLeft(0, 0), sizeRight(0,0), sizeImage(0,0);
	if (NULL != pImageLeft)
	{
		sizeLeft.cx = pImageLeft->GetWidth();
		sizeLeft.cy = pImageLeft->GetHeight();
	}
	if ( NULL != pImageRight )
	{
		sizeRight.cx = pImageRight->GetWidth();
		sizeRight.cy = pImageRight->GetHeight();
	}
	if ( NULL != pImage )
	{
		sizeImage.cx = pImage->GetWidth();
		sizeImage.cy = pImage->GetHeight();
	}

	if ( NULL != pImage )
	{
		CRect rectTest = rectData;
		rectTest.right+= 10;
		
		if (NULL != m_pParent->m_pImageBkGround)
		{
			DrawImage(GraphicImage, m_pParent->m_pImageBkGround, rectTest, 1, 0, true);
		}
		
		CRect rcBkBBS;
		rcBkBBS.top = rectData.top;
		rcBkBBS.bottom = rectData.bottom;
		rcBkBBS.left = rectData.left;
		rcBkBBS.right = rectData.right;
		rcBkBBS.top += 1;

		rcBkBBS.top = (rectData.Height() - m_pParent->m_pImageBkBBS->GetHeight())/2;
		rcBkBBS.bottom = rcBkBBS.top + m_pParent->m_pImageBkBBS->GetHeight();

		CRect rcLeft(rcBkBBS), rcRight(rcBkBBS);
		rcLeft.right = rcLeft.left + sizeLeft.cx;
		if ( rcLeft.Width() > 0 )
		{
			DrawImage(GraphicImage, pImageLeft, rcLeft, 1, 0, true);
			rcBkBBS.left += rcLeft.Width();//-rcLeft.Width()/2;
		}
		rcRight.left = rcRight.right - sizeRight.cx;
		if ( rcRight.Width() > 0 )
		{
			DrawImage(GraphicImage, pImageRight, rcRight, 1, 0, true);
			rcBkBBS.right -= rcRight.Width()-rcRight.Width()/2;
		}
		DrawImage(GraphicImage, pImage, rcBkBBS, 1, 0, true);
	}
	else
	{
		pDC->FillSolidRect(rectData, CFaceScheme::Instance()->GetSysColor(ESCBackground));
	}
	//

// 	pDC->MoveTo(Rect.left,Rect.top);
// 	pDC->LineTo(Rect.right,Rect.top);
// 	pDC->LineTo(Rect.right,Rect.bottom);
// 	pDC->LineTo(Rect.left,Rect.bottom);
// 	pDC->LineTo(Rect.left,Rect.top);
		
//	pDC->MoveTo(Rect.right, Rect.top);
//	pDC->MoveTo(Rect.right,Rect.bottom);
//	pDC->LineTo(Rect.left,Rect.bottom);
	
	pDC->SetTextColor(RGB(0xa7, 0xad, 0xb3));

	CRect RectDraw;
	{
		RectDraw.top = rectData.top;
		RectDraw.bottom = rectData.bottom;
		RectDraw.left = rectData.left;
		RectDraw.right = rectData.right;
	}
	RectDraw.left += 5;
	CRgn rgnOld, rgnNew;
	rgnOld.CreateRectRgn(0,0,0,0);
	rgnNew.CreateRectRgnIndirect(RectDraw);
	BOOL bRgnOld = GetClipRgn(pDC->GetSafeHdc(), (HRGN)rgnOld.m_hObject) > 0;
	pDC->SelectClipRgn(&rgnNew);

	if( RectShow.right > rectData.left)
	{
		// 根据当前广告的数量，从左到右绘制
		// Rect为BBS的rect， RectShow为当前显示最左侧位置
		if ( m_pParent->m_aOrgAdvs.GetSize() <= 0 )
		{
			// 默认绘制
			CRect rect = RectShow;
			rect.right = rectData.right;
			rect.right -= 5;
			rect.top += 1;
			CString StrText;
			StrText.Format(_T("欢迎使用%s"), AfxGetApp()->m_pszAppName);
			pDC->DrawText(StrText, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
			CSize SizeText = pDC->GetTextExtent(StrText);

			RectShow.left -= 1;
			RectShow.right -= 1;

			if ( RectShow.left+SizeText.cx < RectDraw.left )
			{
				const int32 iWidth = RectShow.Width();
				RectShow.left = rectData.right - 10;
				RectShow.right = RectShow.left + iWidth;
			}
		}
		else
		{
			// 广告, Rect可见区域 RectShow总区域
			CRect rect = RectShow;
			rect.right = rectData.right;	// 绘制区域拓展
			rect.right -= 5;
			rect.top += 1;

			const int32 iSeparate = KIBBSSeparate;
			const int32 iAdvCount = m_pParent->m_aOrgAdvs.GetSize();
			m_pParent->m_aOrgAdvRects.SetSize(0, iAdvCount);
// 			while ( rect.left < Rect.right )	// 只要区域能够容纳
// 			{
// 				for ( int32 i=0; i < m_pParent->m_aOrgAdvs.GetSize() ; i++ )
// 				{
// 					CSize SizeText = pDC->GetTextExtent(m_pParent->m_aOrgAdvs[i].m_StrTitle);
// 					const int32 iWidth = SizeText.cx + iSeparate;
// 					if ( rect.left + iWidth > Rect.left )
// 					{
// 						// 于可见范围内
// 						pDC->DrawText(m_pParent->m_aOrgAdvs[i].m_StrTitle, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
// 						
// 						CStatusBarEx::T_OrgAdvRect AdvRect;
// 						AdvRect.SetRect(rect.left, rect.top, rect.left+SizeText.cx, rect.bottom);
// 						AdvRect.iOrgIndex = i;
// 						m_pParent->m_aOrgAdvRects.Add( AdvRect );
// 					}
// 					
// 					rect.left += iWidth;
// 
// 					if ( rect.left >= Rect.right )
// 					{
// 						break;
// 					}
// 				}
// 
// 				rect.left += KIBBSGroupSeparate;	// 大分割
// 			}

			// 这种为一轮一轮的
			for ( int32 i=0; i < iAdvCount ; i++ )
			{
				CSize SizeText = pDC->GetTextExtent(m_pParent->m_aOrgAdvs[i].m_StrTitle);
				const int32 iWidth = SizeText.cx + iSeparate;
				if ( rect.left + iWidth > rectData.left )
				{
					// 于可见范围内
					pDC->DrawText(m_pParent->m_aOrgAdvs[i].m_StrTitle, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
					
					CStatusBarEx::T_OrgAdvRect AdvRect;
					AdvRect.SetRect(max(rect.left, RectDraw.left), rect.top, min(rect.left+SizeText.cx, rect.right), rect.bottom);
					AdvRect.iOrgIndex = i;
					m_pParent->m_aOrgAdvRects.Add( AdvRect );
				}
				
				rect.left += iWidth;
				
				if ( rect.left >= rect.right )
				{
					break;
				}
			}

			if ( m_pParent->GetAdvIndexByPoint(m_pParent->m_ptLastMouseMove) >= 0 )	// 是否鼠标下
			{
				// 鼠标移动进来则不滚动
			}
			else
			{
				RectShow.left -= 1;		
				RectShow.right -= 1;

				// 重定位
				if ( RectShow.right <= rectData.left)
				{
					const int32 iWidth = RectShow.Width();
					RectShow.left = rectData.right - 10;
					RectShow.right = RectShow.left + iWidth;
				}
			}
		}
	}

	if ( bRgnOld )
	{
		pDC->SelectClipRgn(&rgnOld);
	}
	else
	{
		pDC->SelectClipRgn(NULL);
	}

	//pDC->SelectObject(pOldFont);
}


void CRectData::DrawRectLeftBar(CDC * pDC,CRectData& rectData)
{
	if ( NULL == m_pParent )
	{
		return;
	}

	if ( rectData.Width() <= 0 || rectData.Height() <= 0 )
	{
		return;
	}

	Graphics GraphicImage(pDC->GetSafeHdc());
	if (NULL != m_pParent->m_pImageBkGround)
	{
		DrawImage(GraphicImage, m_pParent->m_pImageBkGround, rectData, 1, 0, true);
	}


	CRect RectShow = rectData;

	CSize sizeImage;
	sizeImage.cx = m_pParent->m_pImageSCToolBar->GetWidth();
	sizeImage.cy = m_pParent->m_pImageSCToolBar->GetHeight();

	//sizeImage.cx = 16; //Rect.Width() - 2;			// 缩放图片
	//sizeImage.cy = 16; //Rect.Height() - 2;
	int32 iProper = MIN(sizeImage.cx, sizeImage.cy);
	sizeImage.cx = iProper;
	sizeImage.cy = iProper;

	if ( RectShow.Height() > sizeImage.cy )
	{
		RectShow.top = RectShow.top + (RectShow.Height()-sizeImage.cy)/2;
	}

	//RectShow.top	= 3;
	RectShow.bottom = RectShow.top + sizeImage.cy;

	if(RectShow.Width() > sizeImage.cx)
	{
		int32 iPicWidth = sizeImage.cx;
		int32 iSkip = (RectShow.Width() - iPicWidth)/2;
		RectShow.left += iSkip;
		RectShow.right = RectShow.left + iPicWidth;
	}
	
	DrawImage(GraphicImage, m_pParent->m_pImageSCToolBar, RectShow, 1, 0, true);
}



void CRectData::DrawRectAlarmMerch(CDC * pDC,CRectData & rectData)
{
	if ( NULL == m_pParent )
	{
		return;
	}

	CGGTongDoc* pDoc = AfxGetDocument();
	if ( NULL == pDoc || NULL == pDoc->m_pAbsCenterManager )
	{
		return;
	}


	CAlarmCenter* pAlarmCenter = pDoc->m_pAarmCneter;
	if ( NULL == pAlarmCenter )
	{
		return;
	}

	Graphics GraphicImage(pDC->GetSafeHdc());
	//DrawImage(GraphicImage, m_pParent->m_pImageBkGround, rectData, 1, 0, true);
		
	CRect RectShow = rectData;

	CSize sizeImage;
	sizeImage.cx = m_pParent->m_pImageAlarmOn->GetWidth();
	sizeImage.cy = m_pParent->m_pImageAlarmOn->GetHeight();

	//sizeImage.cx = 16; //Rect.Width() - 2;			// 缩放图片
	//sizeImage.cy = 16; //Rect.Height() - 2;
	int32 iProper = MIN(sizeImage.cx, sizeImage.cy);
	sizeImage.cx = iProper;
	sizeImage.cy = iProper;

	if ( RectShow.Height() > sizeImage.cy )
	{
		RectShow.top = RectShow.top + (RectShow.Height()-sizeImage.cy)/2;
	}
	
	//RectShow.top	= 3;
	RectShow.bottom = RectShow.top + sizeImage.cy;
		
	if(RectShow.Width() > sizeImage.cx)
	{
		int32 iPicWidth = sizeImage.cx;
		int32 iSkip = (RectShow.Width() - iPicWidth)/2;
		RectShow.left += iSkip;
		RectShow.right = RectShow.left + iPicWidth;
	}


	if (pAlarmCenter->GetAlarmFlag())
	{
		ASSERT( NULL != m_pParent->m_pImageAlarmOn);		
		DrawImage(GraphicImage, m_pParent->m_pImageAlarmOn, RectShow, 1, 0, true);	
	}
	else
	{
		ASSERT( NULL != m_pParent->m_pImageAlarmOff);
		DrawImage(GraphicImage, m_pParent->m_pImageAlarmOff, RectShow, 1, 0, true);
	} 
}

void CRectData::DrawRectNetWork(CDC * pDC,CRectData& rectData)
{
	if ( NULL == m_pParent )
	{
		return;
	}

	if ( rectData.Width() <= 0 || rectData.Height() <= 0 )
	{
		return;
	}

	Graphics GraphicImage(pDC->GetSafeHdc());
	//DrawImage(GraphicImage, m_pParent->m_pImageBkGround, rectData, 1, 0, true);

	CSize sizeImage;
	sizeImage.cx = m_pParent->m_pImageNetOn1->GetWidth();
	sizeImage.cy = m_pParent->m_pImageNetOn1->GetHeight();

	//sizeImage.cx = 16; //Rect.Width() - 2;			// 缩放图片
	//sizeImage.cy = 16; //Rect.Height() - 2;
	int32 iProper = MIN(sizeImage.cx, sizeImage.cy);
	sizeImage.cx = iProper;
	sizeImage.cy = iProper;
	
	CRect RectShow = rectData;
	if ( RectShow.Height() > sizeImage.cy )
	{
		RectShow.top = RectShow.top + (RectShow.Height()-sizeImage.cy)/2;
	}
	
	//RectShow.top	= 3;
	RectShow.bottom = RectShow.top + sizeImage.cy;
	
	if(RectShow.Width() > sizeImage.cx)
	{
		int32 iPicWidth = sizeImage.cx;
		int32 iSkip = (RectShow.Width() - iPicWidth)/2;
		RectShow.left += iSkip;
		RectShow.right = RectShow.left + iPicWidth;
	}

// 	int32 iPicWidth  = m_pParent->m_pImageNetOn1->GetWidth();
// 	int32 iPicHeight = m_pParent->m_pImageNetOn1->GetHeight();
// 
// 	CRect RectShow = Rect;
// 	RectShow.top  += 3;
// 	RectShow.bottom= RectShow.top + iPicHeight;
// 
// 	if( RectShow.Width() > iPicWidth )
// 	{
// 		int32 iSkip = (RectShow.Width() - iPicWidth) / 2;
// 		RectShow.left += iSkip;
// 		RectShow.right = RectShow.left + iPicWidth;
// 	}
	
	if (m_pParent->m_bConnect)
	{
		Image *pImage = m_pParent->m_pImageNetOn1;
		switch (m_pParent->m_iShowPic1)
		{
		case 0:
			break;
		case 1:
			ASSERT( NULL != m_pParent->m_pImageNetOn2);
			pImage = m_pParent->m_pImageNetOn2;
			break;
		case 2:
			ASSERT( NULL != m_pParent->m_pImageNetOn3);
			pImage = m_pParent->m_pImageNetOn3;
			break;
		case 3:
			ASSERT( NULL != m_pParent->m_pImageNetOn4);
			pImage = m_pParent->m_pImageNetOn4;
			break;
		default:
			ASSERT( 0 );
		}
		if ( NULL != pImage )
		{
			DrawImage(GraphicImage, pImage, RectShow, 1, 0, true);
		}		
	}
	else	
	{		
		ASSERT( NULL != m_pParent->m_pImageNetOff);
		DrawImage(GraphicImage, m_pParent->m_pImageNetOff, RectShow, 1, 0, true);	
	}

//	pDC->MoveTo(Rect.TopLeft());
//	pDC->LineTo(Rect.right,Rect.top);
}

void CRectData::DrawRectRecentMerch(CDC * pDC,CRectData& rectData)
{
	if ( NULL == m_pParent )
	{
		return;
	}

	if ( rectData.Width() <= 0 || rectData.Height() <= 0 )
	{
		return;
	}
	Graphics GraphicImage(pDC->GetSafeHdc());
	//DrawImage(GraphicImage, m_pParent->m_pImageBkGround, rectData, 1, 0, true);

	CSize sizeImage;
	sizeImage.cx = m_pParent->m_pImageNetOn1->GetWidth();
	sizeImage.cy = m_pParent->m_pImageNetOn1->GetHeight();

	//sizeImage.cx = 16; //Rect.Width() - 2;			// 缩放图片
	//sizeImage.cy = 16; //Rect.Height() - 2;
	int32 iProper = MIN(sizeImage.cx, sizeImage.cy);
	sizeImage.cx = iProper;
	sizeImage.cy = iProper;
	
	CRect RectShow = rectData;
	if ( RectShow.Height() > sizeImage.cy )
	{
		RectShow.top = RectShow.top + (RectShow.Height()-sizeImage.cy)/2;
	}
	
	//RectShow.top	= 3;
	RectShow.bottom = RectShow.top + sizeImage.cy;
	
	if(RectShow.Width() > sizeImage.cx)
	{
		int32 iPicWidth = sizeImage.cx;
		int32 iSkip = (RectShow.Width() - iPicWidth)/2;
		RectShow.left += iSkip;
		RectShow.right = RectShow.left + iPicWidth;
	}
	
// 	int32 iPicWidth = m_pParent->m_pImageRecentMerch->GetWidth();
// 	int32 iPicHeight = m_pParent->m_pImageRecentMerch->GetHeight();
// 
// 	CRect RectShow = Rect;
// 	RectShow.top  += 3;
// 	RectShow.bottom= RectShow.top + iPicHeight;
// 
// 	if( RectShow.Width() > iPicWidth )
// 	{
// 		int32 iSkip = (Rect.Width() - iPicWidth) / 2;
// 		RectShow.left += iSkip;
// 		RectShow.right = RectShow.left + iPicWidth;
// 	}

	DrawImage(GraphicImage, m_pParent->m_pImageRecentMerch, RectShow, 1, 0, true);

//	pDC->MoveTo(Rect.left, Rect.bottom);
//	pDC->LineTo(Rect.left, Rect.top);
//	pDC->LineTo(Rect.right,Rect.top);
//	pDC->LineTo(Rect.right,Rect.bottom);
}


void CRectData::DrawShowTips(CDC *pDC,CRectData& rectData)
{
	if (rectData.left == rectData.right)
	{
		return;
	}
	int iBlackLine = 16;
	int iStartTop = (rectData.Height() - 16)/2; 

	CPen pen,*pOldPen;
	pen.CreatePen(PS_SOLID,2,RGB(23,22,28));
	pOldPen = (CPen*)pDC->SelectObject(&pen);
	pDC->MoveTo(rectData.left+1, rectData.top + iStartTop);
	pDC->LineTo(rectData.left+1, rectData.top + iStartTop + iBlackLine);

	pDC->MoveTo(rectData.right-1, rectData.top + iStartTop);
	pDC->LineTo(rectData.right-1, rectData.top + iStartTop + iBlackLine);

	pDC->SetTextColor(RGB(200,200,200));
	CRect rcDrawTmp = rectData;
	rcDrawTmp.OffsetRect(31,0);
	pDC->DrawText(rectData.m_StrName,&rcDrawTmp,DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	pDC->SelectObject(pOldPen);
}

void CRectData::DrawRectSearch(CDC *pDC,CRectData& rectData, int icltHeight)
{
	Graphics GraphicImage(pDC->GetSafeHdc());

	SolidBrush solidBrush(Color(42, 43, 45));
	GraphicImage.FillRectangle(&solidBrush, rectData.left, 0, rectData.Width(), icltHeight);

	DrawImage(GraphicImage, m_pParent->m_pImageSearch, rectData, 1, 0, true);
}

void CRectData::DrawRectTodayComment(CDC * pDC,CRectData& rectData)
{
	if ( NULL == m_pParent )
	{
		return;
	}
	
	CGGTongDoc* pDoc = AfxGetDocument();
	if ( NULL == pDoc || NULL == pDoc->m_pAbsCenterManager )
	{
		return;
	}

	if ( rectData.Width() <= 0 || rectData.Height() <= 0 )
	{
		return;
	}

	Graphics GraphicImage(pDC->GetSafeHdc());
	DrawImage(GraphicImage, m_pParent->m_pImageBkGround, rectData, 1, 0, true);
	
	CSize sizeImage;
	sizeImage.cx = m_pParent->m_pImageNetOn1->GetWidth();
	sizeImage.cy = m_pParent->m_pImageNetOn1->GetHeight();
	
	sizeImage.cx = 16; 
	sizeImage.cy = 16; 
	int32 iProper = MIN(sizeImage.cx, sizeImage.cy);
	sizeImage.cx = iProper;
	sizeImage.cy = iProper;
	
	CRect RectShow = rectData;
	if ( RectShow.Height() > sizeImage.cy )
	{
		RectShow.top = RectShow.top + (RectShow.Height()-sizeImage.cy)/2;
	}
	
	RectShow.bottom = RectShow.top + sizeImage.cy;
	
	if(RectShow.Width() > sizeImage.cx)
	{
		int32 iPicWidth = sizeImage.cx;
		int32 iSkip = (RectShow.Width() - iPicWidth)/2;
		RectShow.left += iSkip;
		RectShow.right = RectShow.left + iPicWidth;
	}
	
	if ( pDoc->m_pAbsCenterManager->GetTodayFlag() )
	{
		DrawImage(GraphicImage, m_pParent->m_pImageTodayCommentOn, RectShow, 1, 0, true);
	}
	else
	{
		DrawImage(GraphicImage, m_pParent->m_pImageTodayCommentOff, RectShow, 1, 0, true);
	}
	
}
//画消息图标 2013-10-31 add by cym
void CRectData::DrawRectMessage(CDC * pDC,CRectData& rectData)
{
	if ( NULL == m_pParent )
	{
		return;
	}
	
	CGGTongDoc* pDoc = AfxGetDocument();
	if ( NULL == pDoc || NULL == pDoc->m_pAbsCenterManager )
	{
		return;
	}
	
	Graphics GraphicImage(pDC->GetSafeHdc());
	DrawImage(GraphicImage, m_pParent->m_pImageBkGround, rectData, 1, 0, true);
	
	CSize sizeImage;
	sizeImage.cx = m_pParent->m_pImageNetOn1->GetWidth();
	sizeImage.cy = m_pParent->m_pImageNetOn1->GetHeight();
	
	sizeImage.cx = 16; 
	sizeImage.cy = 16; 
	int32 iProper = MIN(sizeImage.cx, sizeImage.cy);
	sizeImage.cx = iProper;
	sizeImage.cy = iProper;
	
	CRect RectShow = rectData;
	if ( RectShow.Height() > sizeImage.cy )
	{
		RectShow.top = RectShow.top + (RectShow.Height()-sizeImage.cy)/2;
	}
	
	RectShow.bottom = RectShow.top + sizeImage.cy;
	
	if(RectShow.Width() > sizeImage.cx)
	{
		int32 iPicWidth = sizeImage.cx;
		int32 iSkip = (RectShow.Width() - iPicWidth)/2;
		RectShow.left += iSkip;
		RectShow.right = RectShow.left + iPicWidth;
	}

	// modify by tangad 今评功能已去掉
	/*if ( pDoc->m_pAbsCenterManager->GetMessageFlag() )
		DrawImage(GraphicImage, m_pParent->m_pImageMessage, RectShow, 1, 0, true);
	else
		DrawImage(GraphicImage, m_pParent->m_pImageMessageShow, RectShow, 1, 0, true);*/
}		

bool CRectData::HitTest ( CPoint pt, int32 iMouseAction )
{
	if (1 == iMouseAction)
	{			
		if ( this->PtInRect(pt))
		{
			if ( NULL == m_pParent )
			{				
				return false;
			}

			CPoint ptMouse;
			GetCursorPos(&ptMouse);
			CNewMenu menu;
			menu.LoadMenu(IDR_MENU_STATUSBAR);
			CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
			pPopMenu->LoadToolBar(IDR_MENU_STATUSBAR);
			//////////////////////////////////////////////////////////////////////////
			//  设置右键菜单的check 状态.
			int32 iCount = m_pParent->GetMKNumber();
//			pPopMenu->CheckMenuItem(IDM_STATUS_SHOW2,MF_BYCOMMAND|MF_CHECKED);
//			pPopMenu->CheckMenuItem(IDM_STATUS_SHOW3,MF_BYCOMMAND|MF_CHECKED);
//			pPopMenu->CheckMenuItem(IDM_STATUS_SHOW4,MF_BYCOMMAND|MF_CHECKED);
  			if (  iCount == 2 )
  			{
  			  	pPopMenu->CheckMenuItem(IDM_STATUS_SHOW2,MF_BYCOMMAND|MF_CHECKED);
  			}
  			else if (iCount == 3 )
  			{
  			  	pPopMenu->CheckMenuItem(IDM_STATUS_SHOW3,MF_BYCOMMAND|MF_CHECKED);
  			}
  			else if (iCount == 4 )
  			{
  			  	pPopMenu->CheckMenuItem(IDM_STATUS_SHOW4,MF_BYCOMMAND|MF_CHECKED);
			}

			CString  StrTest = this->m_StrName;
		             

			pPopMenu->SetMenuTitle(L"", MFT_ROUND|MFT_LINE|MFT_CENTER|MFT_SIDE_TITLE);
			pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,ptMouse.x, ptMouse.y, (CWnd*)m_pParent);
			
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

// 绘制多空的下半的小方块
void DrawDuoKongDownSmallFlag(CDC *pDC, const CRect &rc, int32 iIndex, Graphics &g, Bitmap *pBitmap)
{
	if ( NULL == pDC )
	{
		ASSERT( 0 );
		return;
	}
	if ( NULL == pBitmap
		|| pBitmap->GetLastStatus() != Ok )
	{
		ASSERT( 0 );
		return;
	}

	DrawImage(g, pBitmap, rc, 9, iIndex, true);
}

// 绘制多空的上半的比例
void	DrawDuoKongUpRatio(CDC *pDC, const CRect &rc, float fRatioLeft, float fRatioRight)
{
	// [ ][ ]|[ ][ ]
	CBrush brh;
	brh.CreateStockObject(WHITE_BRUSH);
	const int32 iSmallWidth = rc.Width()/4;
	const int32 iCenter = rc.right - 2*iSmallWidth;
	CRect rc2(rc);
	rc2.left = rc2.right - iSmallWidth;
	// 右边到左边
	for ( int32 i=0; i < 4 ; i++ )
	{
		pDC->FrameRect(rc2, &brh);
		rc2.right = rc2.left;
		rc2.left -= iSmallWidth;
		rc2.right += 1;	// 右侧不绘制
	}

	rc2.InflateRect(0, -1);
	if ( fRatioLeft > 0.0f )
	{
		rc2.left = iCenter - 2*iSmallWidth+1;
		rc2.right = iCenter;
		int32 iLeftWidth = (int32)(fRatioLeft*rc2.Width());
		pDC->FillSolidRect(rc2.right - iLeftWidth, rc2.top, iLeftWidth, rc2.Height(), RGB(255,0,0));
	}
	if ( fRatioRight > 0.0f )
	{
		rc2.left = iCenter+1;
		rc2.right = iCenter +2*iSmallWidth -1;
		int32 iRightWidth = (int32)(fRatioRight*rc2.Width());
		pDC->FillSolidRect(rc2.left, rc2.top, iRightWidth, rc2.Height(), RGB(0,128,0));
	}
}

void CRectData::DrawRectDuoKongZhenXian( CDC *pDC, CRectData &rectData )
{
	if ( rectData.Width() <= 0 || rectData.Height() <= 0 )
	{
		return;
	}

	// 填充背景色:
	Graphics GraphicImage(pDC->GetSafeHdc());
	
	if ( NULL == m_pParent )
	{				
		return;
	}

	Image* pImage = m_pParent->m_pImageBkGround;
	if ( NULL != pImage )
	{
		DrawImage(GraphicImage, pImage, rectData, 1, 0, true);
	}
	else
	{
		pDC->FillSolidRect(rectData, CFaceScheme::Instance()->GetSysColor(ESCBackground));
	}

	CGGTongDoc *pDoc = AfxGetDocument();
	CAbsCenterManager *pAbsCenterManager = NULL != pDoc ? pDoc->m_pAbsCenterManager : NULL;
	if ( NULL == pAbsCenterManager )
	{
		ASSERT( 0 );
		return;
	}

	CRect rc;
	{
		rc.top = rectData.top;
		rc.bottom = rectData.bottom;
		rc.left = rectData.left;
		rc.right = rectData.right;
	}

	rc.InflateRect(-2, 0);
	CRect rcUp(rc);
	rc.top = rc.bottom - rc.Height()/2+1;
	rc.bottom -= 1;
	rcUp.bottom = rc.top -1;
	rcUp.top += 3;
	rcUp.left += 3;

	// 左侧画一竖线
	pDC->MoveTo(rc.left, rectData.top+1);
	pDC->LineTo(rc.left, rectData.bottom);

	// 上海左 1/2位置 
	rc.right = rc.right - rc.Width()/2-2;
	rcUp.right = rc.right;
	
	const int32 iSmallRectWidth = rc.Width()/KIDuoKongSmallRectCount;

	// 绘制上半截红绿[__][___]比例
	{
		T_GeneralNormal DataNewH = m_pParent->m_LastOldDuoKongH;
		float fLeft=0.0f, fRight=0.0f;
		float fTotal = DataNewH.m_uiFallMomentary + DataNewH.m_uiRiseMomentary;
		if ( fTotal > 0.0f )
		{
			fLeft = DataNewH.m_uiRiseMomentary/fTotal;
			fRight = DataNewH.m_uiFallMomentary/fTotal;
		}
		DrawDuoKongUpRatio(pDC, rcUp, fLeft, fRight);
	}
	int32 iIndexPos = m_pParent->m_aDuoKongImageH.GetUpperBound();
	int32 iDrawPos = KIDuoKongSmallRectCount-1;
	ASSERT( iIndexPos == iDrawPos );
	// 绘制下半截[=↑↓] 右边画到左边, 6个 
	
	for ( ; iDrawPos >= 0 ; iDrawPos--, iIndexPos-- )
	{
		int32 iIndex = m_pParent->m_aDuoKongImageH[iIndexPos];
		rc.left = rc.right - iSmallRectWidth +1;	// 留一个像素空
		DrawDuoKongDownSmallFlag(pDC, rc, iIndex,  GraphicImage, m_pParent->m_pImageDuoKongSmallBmp);
		rc.right -= iSmallRectWidth;
	}
	
	// 深证 右 1/2位置 
	rc.left = rcUp.right + 2;
	rc.right = rectData.right-5;
	rcUp.left = rc.left;
	rcUp.right = rc.right;
	
	// 绘制上半截红绿[__][___]比例
	{
		T_GeneralNormal DataNewS = m_pParent->m_LastOldDuoKongS;
		float fLeft=0.0f, fRight=0.0f;
		float fTotal = DataNewS.m_uiFallMomentary + DataNewS.m_uiRiseMomentary;
		if ( fTotal > 0.0f )
		{
			fLeft = DataNewS.m_uiRiseMomentary/fTotal;
			fRight = DataNewS.m_uiFallMomentary/fTotal;
		}
		DrawDuoKongUpRatio(pDC, rcUp, fLeft, fRight);
	}
	iIndexPos = m_pParent->m_aDuoKongImageS.GetUpperBound();
	// 绘制下半截[=↑↓] 左边画到右边, 6个 
	for ( iDrawPos = KIDuoKongSmallRectCount-1 ; iDrawPos >= 0 ; iDrawPos--, iIndexPos-- )
	{
		int32 iIndex = m_pParent->m_aDuoKongImageS[iIndexPos];
		rc.left = rc.right - iSmallRectWidth +1;	// 留一个像素空
		DrawDuoKongDownSmallFlag(pDC, rc, iIndex, GraphicImage, m_pParent->m_pImageDuoKongSmallBmp);
		rc.right -= iSmallRectWidth;
	}
	
	// 最右侧画一竖线
 	// 不画
}

CStatusBarEx::T_BarMerch::T_BarMerch()
{
	m_StrName = _T("上证");
	m_iMarket = 0;
	m_StrCode = _T("000001");
	m_pMerch = NULL;
}

CStatusBarEx::T_BarMerch::T_BarMerch( const CString &StrName, int32 iMarket, const CString &StrCode )
: m_StrName(StrName)
, m_iMarket(iMarket)
, m_StrCode(StrCode)
{
	m_pMerch = NULL;
}

CAlarmCenter* CStatusBarEx::GetAlarmCenter()
{
	CGGTongDoc *pDocument = AfxGetDocument();	
	if ( NULL == pDocument )
	{
		return NULL;
	}
		
	CAlarmCenter* pAlarmCenter = pDocument->m_pAarmCneter;
	return pAlarmCenter;

}



void  CStatusBarEx::CreateAndDisPlayKeyborad()
{
	CMainFrame *pParent = (CMainFrame*)GetParent();

	if (pParent == NULL)
	{
		return;
	}

	if (pParent->m_pDlgKeyBoard == NULL )
	{
		pParent->m_pDlgKeyBoard = new CKeyBoardDlg(pParent->m_HotKeyList, pParent);
		if (!pParent->m_pDlgKeyBoard->Create(IDD_KEYBOARD, pParent))
		{
			return;
		}
		
	}


	CRect rectKey;
	pParent->m_pDlgKeyBoard->GetWindowRect(&rectKey);
	int32 iHeight = rectKey.Height();
	int32 iWidth  = rectKey.Width();

	CRect rt = m_KeyBoarEditRect;
	//int32 iEditHight = m_KeyBoarEditRect.bottom;
	ClientToScreen(rt);
	//::SetWindowPos(m_pDlgKeyBoard->m_hWnd, HWND_TOP, rt.left, rt.bottom - iHeight - iEditHight, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE);

	CRect rtKeyBoard;
	rtKeyBoard.top		= rt.top - iHeight;
	rtKeyBoard.left		= rt.left;

	rtKeyBoard.right	= rtKeyBoard.left + iWidth;
	rtKeyBoard.bottom	= rtKeyBoard.top + iHeight;

	pParent->m_pDlgKeyBoard->MoveWindow(rtKeyBoard);
	pParent->m_pDlgKeyBoard->ShowWindow(SW_SHOW);

	pParent->m_pDlgKeyBoard->SetStatusBarSign(TRUE);
	pParent->m_pDlgKeyBoard->SetActiveByClickSign(false);
	
}





void CStatusBarEx::OnAlarmsChanged(void* pData, CAlarmNotify::E_AlarmNotify eNotify)
{
	CMainFrame *pMainFrm = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());

	// 更换图标
	if ( CAlarmNotify::EANAlarmOn == eNotify )
	{
		RedrawWindow();
	}

	// 更新条件预警对话框复选框状态
	if (pMainFrm && pMainFrm->m_pDlgAlarm)
	{
		pMainFrm->m_pDlgAlarm->SetCheckFlag();
	}
}

void CStatusBarEx::Alarm(CMerch* pMerch, const CString& StrPrompt)
{
	if ( NULL == pMerch )
	{
		return;
	}

	CAlarmCenter* pAlarmCenter = GetAlarmCenter();
	if ( NULL == pAlarmCenter )
	{
		return;
	}

	//
	bool32 bShow = pAlarmCenter->GetAlarmStatusType();

	if ( pAlarmCenter->GetAlarmFlag() && pAlarmCenter->GetPromptFlag() && bShow )
	{
		// 如果是开启警报, 且是状态栏提示的方式. 那么弹一个气球提示
		CString StrAlarm;
		StrAlarm.Format(L"%s [%s]", pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), ((CString)StrPrompt).GetBuffer());
		pMerch->m_MerchInfo.m_StrMerchCnName.ReleaseBuffer();
		((CString)StrPrompt).ReleaseBuffer();

		//
		CPoint pt = m_RectAlarm.TopLeft();
		ClientToScreen(&pt);
		
		//
		CBalloonMsg::RequestCloseAll();
		CBalloonMsg::Show(L"预警", StrAlarm, (HICON)1, &pt);
	}	
}

void CStatusBarEx::AlarmArbitrage(CArbitrage* pArbitrage, const CString& StrPrompt)
{
	if ( NULL == pArbitrage )
	{
		return;
	}
	
	CAlarmCenter* pAlarmCenter = GetAlarmCenter();
	if ( NULL == pAlarmCenter )
	{
		return;
	}
	
	//
	bool32 bShow = pAlarmCenter->GetAlarmStatusType();
	
	if ( pAlarmCenter->GetAlarmFlag() && pAlarmCenter->GetPromptFlag() && bShow )
	{
		// 如果是开启警报, 且是状态栏提示的方式. 那么弹一个气球提示
		CString StrAlarm;
		StrAlarm.Format(L"%s [%s]", pArbitrage->GetShowName().GetBuffer(), ((CString)StrPrompt).GetBuffer());
		pArbitrage->GetShowName().ReleaseBuffer();
		((CString)StrPrompt).ReleaseBuffer();
		
		//
		CPoint pt = m_RectAlarm.TopLeft();
		ClientToScreen(&pt);
		
		//
		CBalloonMsg::RequestCloseAll();
		CBalloonMsg::Show(L"预警", StrAlarm, (HICON)1, &pt);
	}	
}

void CStatusBarEx::OnOrgDataUpdate()
{
	if ( NULL!=GetSafeHwnd() )
	{
		PostMessage(UM_DO_ORGDATAUPDATE);
	}
}


void CStatusBarEx::OnChangeEdit()
{
	m_KeyBoarEdit.SetFocus();
	CMainFrame *pParent = (CMainFrame*)GetParent();
	if (pParent == NULL || pParent->m_pDlgKeyBoard == NULL)
	{
		return;
	}

	CString szEditData ;
	m_KeyBoarEdit.GetWindowText(szEditData);
	pParent->m_pDlgKeyBoard->SetEditText(szEditData);
	return ;
}






LRESULT CStatusBarEx::SearchEditClick( WPARAM w, LPARAM l )
{
	m_bKeyBoardEditClick = true;
	CreateAndDisPlayKeyborad();
	m_KeyBoarEdit.SetFocus();
	return 1;
}





void CStatusBarEx::OnSetEditFocus()
{
	return ;
}



void CStatusBarEx::OnKillEditFocus()
{
	if (m_bKeyBoardEditClick)
	{
		m_bKeyBoardEditClick = false;
		return;
	}

	CMainFrame *pParent = (CMainFrame*)GetParent();


	if (pParent == NULL || pParent->m_pDlgKeyBoard == NULL)
	{
		return;
	}
	
	pParent->m_pDlgKeyBoard->SetStatusBarSign(false);
	pParent->m_pDlgKeyBoard->SetActiveByClickSign(TRUE);

	CString strEditData;
	m_KeyBoarEdit.GetWindowText(strEditData); 
	pParent->m_pDlgKeyBoard->CustomClose(strEditData);
	return ;
}






void CStatusBarEx::DoOnOrgDataUpdate()
{
	// 	iServiceDispose *pDisp = NULL;
	// 	pDisp = GetServiceDispose();
	auth::T_TextBannerConfig CommTextCfg;
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	pDoc->m_pAutherManager->GetInterface()->GetServiceDispose()->GetTextBanner(CommTextCfg);
	// 	if (NULL == pDisp)
	// 	{
	// 		//ASSERT(0);
	// 		return;
	// 	}

	// 机构私有数据 跑马灯新闻
	// 	T_UserInfo userInfo;
	// 	pDisp->GetUserInfo(userInfo);
	// 	int32 iOrgKey = 0;
	// 	//iOrgKey = userInfo.iPSID;
	// 	iOrgKey = _ttol(CConfigInfo::Instance()->GetOrgKey());
	// 	
	//  	const char *pszOrg /*= pDisp->GetOrgPrivateData(iOrgKey)*/;
	// 	if ( NULL != pszOrg )
	// 	{
	// 		const int32 iLen = strlen(pszOrg) + 1;
	// 		char *pszOrgXml = new char[iLen];
	// 		if ( NULL != pszOrgXml )
	// 		{
	// 			// base64 解码
	// 			int32 iDecodeLen = iLen;
	// 			bool32 bRet = CCodeFile::DeCodeString((char *)pszOrg, iLen-1, pszOrgXml, &iDecodeLen, CCodeFile::ECFTBase64);	// 字串尾部不计算
	// 			ASSERT( bRet );	
	// 			
	// 			TiXmlDocument tiDoc;
	// 			tiDoc.Parse(pszOrgXml);
	// 			delete []pszOrgXml;
	// 			pszOrgXml = NULL;
	// 			const char *KStrXmlTitle = "Title";
	// 			const char *KStrXmlUrl	 = "Url";
	// 			const char *KStrXmlContent = "Content";
	// 			const char *KStrXmlAdIndex = "AdIndex";
	T_OrgAdvertisement adv;
	m_aOrgAdvs.RemoveAll();
	m_aOrgAdvRects.RemoveAll();		// 区域清空
	// 			for (TiXmlElement *pEle = tiDoc.FirstChildElement("N"); pEle != NULL ; pEle = pEle->NextSiblingElement("N"))
	// 			{
	// 				// <N Title="xxxxutf8" Url="xxx" Content="xxx" AdIndex="1"/>
	// 				const char *pszAttri = pEle->Attribute(KStrXmlTitle);
	// 				if ( NULL == pszAttri )
	// 				{
	// 					continue;
	// 				}
	// 				wchar_t *pwsz = CEtcXmlConfig::MultiToWide(pszAttri, CP_UTF8);
	adv.m_StrTitle = CommTextCfg.wszStatusContent;
	// 				delete []pwsz;
	// 				
	// 				pszAttri = pEle->Attribute(KStrXmlUrl);
	// 				if ( NULL == pszAttri )
	// 				{
	// 					continue;
	// 				}
	// 				pwsz = CEtcXmlConfig::MultiToWide(pszAttri, CP_UTF8);
	adv.m_StrUrl = L"";
	// 				delete []pwsz;
	// 				
	// 				adv.m_StrContent.Empty();
	// 				pszAttri = pEle->Attribute(KStrXmlContent);
	// 				if ( NULL != pszAttri )
	// 				{
	// 					pwsz = CEtcXmlConfig::MultiToWide(pszAttri, CP_UTF8);
	adv.m_StrContent = CommTextCfg.wszStatusContent;
	// 					delete []pwsz;
	// 				}
	// 				
	// 				pszAttri = pEle->Attribute(KStrXmlAdIndex);
	adv.m_iIndex = 0;
	// 				if ( NULL != pszAttri )
	// 				{
	// 					adv.m_iIndex = atoi(pszAttri);
	// 				}
	// 				
	m_aOrgAdvs.Add(adv);
	// 			}
	// 		}
	// 	}
	// 
	// 	// 重新计算所占大小
	CSize sizeBBS = GetProperBBSSizeShow();
	m_RectShow.right = m_RectShow.left + sizeBBS.cx;	// 更新宽度
	// 	// 马上就会定时绘制的
}

iServiceDispose	* CStatusBarEx::GetServiceDispose()
{
	CGGTongDoc *pDocument = AfxGetDocument();	
	ASSERT(NULL != pDocument);
	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	ASSERT(NULL != pAbsCenterManager);
	iServiceDispose *pDisp = NULL;
	if ( NULL == pAbsCenterManager || NULL == (pDisp=pAbsCenterManager->GetServiceDispose()) )
	{
		return NULL;
	}
	return pDisp;
}

auth::iProxyAuthClient	* CStatusBarEx::GetAuthManager()
{
	CGGTongDoc *pDocument = AfxGetDocument();	
	ASSERT(NULL != pDocument);
	auth::iProxyAuthClient *pAuth = NULL;
	if ( NULL == pDocument || NULL == (pAuth=pDocument->m_pAutherManager->GetInterface()) )
	{
		return NULL;
	}
	return pAuth;
}

