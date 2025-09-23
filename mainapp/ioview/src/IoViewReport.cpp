#include "stdafx.h"
#include "ShareFun.h"
#include "memdc.h"
#include "io.h"
#include "PathFactory.h"
#include "MerchManager.h"
#include "UserBlockManager.h"
#include "MerchManager.h"
#include "facescheme.h"
#include "GGTongView.h"
#include "IoViewManager.h"
#include "IoViewShare.h"
#include "IoViewKLine.h"
#include "IoViewTimeSale.h"
#include "GridCellSymbol.h"
#include "GridCellLevel2.h"
#include "GridCellCheck.h"
#include "DlgBlockSelect.h"
#include "DlgBlockSet.h"
#include "DlgBelongBlock.h"
#include "DlgImportUserBlockBySoft.h"
#include "DlgUserBlockAddMerch.h"
#include "BlockConfig.h"
#include "IoViewReportSelect.h"
#include "IoViewReport.h"
#include "LogFunctionTime.h"
#include "coding.h"
#include "DlgInputShowName.h"
#include "XmlShare.h"
#include "IoViewSyncExcel.h"
#include "MPIChildFrame.h"
#include "BuySellPriceExchange.h"
#include "FontFactory.h"
#include "DlgEditReport.h"
#include "GridCellImage.h"
#include <string>

using std::string;

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MID_BUTTON_ID 20000

#define MID_BUTTON_USER_BLOCK_START 20001
#define MID_BUTTON_USER_BLOCK_ADD	20002
#define MID_BUTTON_USER_BLOCK_DEL	20003
#define MID_BUTTON_USER_BLOCK_CLEN	20004
#define MID_BUTTON_USER_BLOCK_IMP	20005
#define MID_BUTTON_USER_BLOCK_EXP	20006
#define MID_BUTTON_USER_BLOCK_END	20010

#define COLO_NO_NAME_CODE  RGB(245,253,170)


const char* KStrViewReportValueName			= "VALUE";
const char* KStrViewReportTypeName			= "TYPE";
const char* KStrViewReportTabName			= "CURTAB";
const char* KStrViewReportIsReadCurTab		= "ISREADCURTAB";
const char* KStrViewReportTypeSysName		= "SOLID";
const char* KStrViewReportTypeUserName		= "USER";
const char* KStrViewReportTypeLogicName		= "LOGIC";
const char* KStrViewReportTypeSelectName    = "SELECT";
const char* KStrViewReportTypeRecentViewName= "RecentView";
const char* KStrViewReportTypeMerchSortName = "MerchSort";
const char* KStrViewReportTypeBlockLogicName = "BLOCKLOGIC";
const char* KStrViewReportShowGridVertScrool = "SHOWGRIDVERTSCROOL";
const char* KStrViewReportShowTab			= "SHOWTAB";
const char* KStrViewReportShowTabByTabbar	= "SHOWTABBYTABBAR";
const char* KStrViewReportShowGridLine		= "SHOWGRIDLINE";
const char* KStrViewReportShowCustomGrid	= "SHOWCUSTOMGRID";
const char* KStrViewReportShowUserBlock  	= "SHOWUSERBLOCK";
const char* KStrViewReportLock				= "Lock";

const char* KStrXMLReportSection			= "IoViewReport";
const char* KStrXMLReportAutoPagePeriodKey	= "autoPagePeriod";
const char* KStrXMLReportAutoSizeCol		= "autoSizeCol";

const char* KStrViewReportBeEscReport		= "ISESCREPORT";
const char* KStrViewReportEscBlock			= "ESCBLOCK";
const char* KStrViewReportSimpleTabClassId  = "tabClassId";	// SimpleTab��������

const char* KStrViewReportShowTopTabbar  = "ShowTopTabbar";	// ShowTopTabbar�Ƿ���ʾ


const char* KStrViewReportShowMoreColumn    = "showMoreCol";	// �Ƿ���ʾ����������

const int32 KRequestViewDataSortContinueTimerId				= 100001;
const int32 KRequestViewDataSortContinueTimerPeriod			= 5000;

const int32 KRequestViewDataCurrentVisibleRowTimerId		= 100002;					// ��Ҫ���µ�ʱ��, ���¼��㵱ǰ�ɼ�����.����ʵʱ����.(�����ʱ�����ڷ�������ʱ����100ms ������. ��һ���Ե�)
const int32 KRequestViewDataCurrentVisibleRowTimerPeriod	= 50;

const int32 KRequestViewDataSortTimerId						= 100003;
const int32 KRequestViewDataSortTimerPeriod					= 50;

const int32 KUpdateExcelTimerId								= 100004;
const int32 KUpdateExcelTimerPeriod							= 500;

const int32 KUpdatePushMerchsTimerId						= 100005;					// ÿ�� 5 ����, ���¼���һ�ε�ǰ��������Ʒ
const int32 KTimerPeriodPushMerchs							= 1000 * 5;

const int32 KAutoPageTimerId								= 100005;					// ���۱��Զ���ҳ
const int32 KTimerPeriodAutoPage							= 1000 * 5;

const int32 KBudingTimerId									= 100006;					// ����
const int32 KTimerPeriodBuding								= 1000 * 5;

const int32 KiDataNumAddForException						= 1;						// ÿҳ��� 3 �������Ա�����

const int32 KiFixTabCounts									= 10;						// �̶���Tab ��ǩ����
const CString KStrFixTabName								= L"����";					// �̶���ǩ��

// �̶� Tab ��������Ŀ. �����һ��Tab ��Ϊ�����ʾǰ��û�еİ��

static const int32 KiIDTabAll			= -1;   // ȫ��
static const int32 KiIDTabUserOwn		= -2;	// ��ѡ�ɼ���
static const int32 KiIDTabChgable		= -3;	// ���һ��Tab �����Լ��ı��

static const int32 KiIDTabRecentView    = -4;

static const int32 KiIDTabAllExcept		= 2;	// ȫ����ȥ��������

// ��ǩҳ���鶨��
static const T_SimpleTabInfo  s_KaTabInfos[] = 
{
	T_SimpleTabInfo(2,				2003,	0,		L"���з����",		ETITCollection,		T_BlockDesc::EBTBlockLogical),
	T_SimpleTabInfo(KiIDTabAll,		4075,	1,		L"��鼯�ϡ�",		ETITCollection,		T_BlockDesc::EBTBlockLogical),
	T_SimpleTabInfo(KiIDTabUserOwn,	-1,		2,		L"��ѡ����",		ETITCollection,		T_BlockDesc::EBTUser),
	T_SimpleTabInfo(2000,			2000,	3,		L"",				ETITEntity,			T_BlockDesc::EBTBlockLogical),
	T_SimpleTabInfo(2005,			2005,	4,		L"",				ETITEntity,			T_BlockDesc::EBTBlockLogical),
	T_SimpleTabInfo(2006,			2006,	5,		L"",				ETITEntity,			T_BlockDesc::EBTBlockLogical),
	T_SimpleTabInfo(2007,			2007,	6,		L"",				ETITEntity,			T_BlockDesc::EBTBlockLogical),
	T_SimpleTabInfo(2008,			2008,	7,		L"",				ETITEntity,			T_BlockDesc::EBTBlockLogical),
	T_SimpleTabInfo(2010,			2010,	8,		L"",				ETITEntity,			T_BlockDesc::EBTBlockLogical),
	T_SimpleTabInfo(1,				1,		9,		L"",				ETITEntity,			T_BlockDesc::EBTPhysical),
	T_SimpleTabInfo(1001,			1001,	10,		L"",				ETITEntity,			T_BlockDesc::EBTPhysical),		// ����������, ����������һ��
};

//
static const int32 s_KiTabCounts = sizeof(s_KaTabInfos) / sizeof(T_SimpleTabInfo);

bool32 SimpleTabInfoToXml(const T_SimpleTabInfo &tabInfo, OUT TiXmlElement &tiEleThis);
bool32 SimpleTabInfoFromXml(OUT T_SimpleTabInfo &tabInfo, IN TiXmlElement &tiEleThis);


static const CString  s_KaChooseStockTips[ESSCount] =
{
	L"����", L"����", L"����", L"����"
}; 


const CString s_ExcelRowNamesReport[] = { 
	_T("A"), _T("B"), _T("C"), _T("D"), _T("E"), _T("F"), _T("G"), _T("H"), _T("I"), _T("J"), _T("K"), 
	_T("L"), _T("M"), _T("N"), _T("O"), _T("P"), _T("Q"), _T("R"), _T("S"), _T("T"), _T("U"), _T("V"),
	_T("W"), _T("X"), _T("Y"), _T("Z"),
	"AA", "AB", "AC", "AD", "AE", "AF", "AG", "AH", "AI", "AJ", "AK", "AL", "AM", "AN", "AO", "AP", "AQ",
	"AR", "AS", "AT", "AU", "AV", "AW", "AX", "AY", "AZ"
};


IMPLEMENT_DYNCREATE(CIoViewReport, CIoViewBase)

int32	CIoViewReport::s_iAutoPageTimerPeriodSecond = 8;		// �Զ���ҳ���
SimpleTabClassMap	CIoViewReport::sm_mapSimpleTabConfig;



#define NotifyMsg(wNotifyCode,id,memberFxn)\
	if(phdr->code == wNotifyCode && phdr->idFrom == id)\
{\
	memberFxn(phdr,pResult);\
}

#define ID_VERTICAL_REPORT		12345
#define ID_REPORT_GRID          10205


#define TOPTABBAR_BEGINID	23000
#define TOPTABBAR_ENDID		23100
#define INVALID_ID			-1


typedef struct T_UserFont 
{
	E_SysFont	eOwnerType;
	CString		tFontFamily;
	int			iFontHeight;
	int			iFontWeight;
	int			iFontCharset;
	int			iFontOutPrecision;
};

T_UserFont s_arrUserReportFont[] =
{
	{
		ESFSmall, L"΢���ź�", -14,  400, 0, 3
	},
	{
		ESFText, L"΢���ź�", -15,  400, 0, 3
		},
		{
			ESFNormal, L"΢���ź�", -16,  400, 0, 3
		},
		{
			ESFBig, L"΢���ź�", -17,  400, 0, 3
			}
};


///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewReport, CIoViewBase)
	//{{AFX_MSG_MAP(CIoViewReport)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	// ON_MESSAGE(UM_HOTKEY, OnMsgHotKey)
	ON_COMMAND(ID_LAYOUT_ADJUST,OnLayOutAdjust)
	ON_COMMAND_RANGE(IDM_BLOCK_BEGIN,IDM_BLOCK_END, OnMenu)
	ON_COMMAND_RANGE(IDM_USER1,IDM_USER4, OnMenuExport)
	ON_COMMAND_RANGE(IDM_IOVIEWREPORT_BEGIN, IDM_IOVIEWREPORT_END, OnMenuIoViewReport)
	ON_COMMAND_RANGE(IDC_CHART_ADD2BLOCK_BEGIN,IDC_CHART_ADD2BLOCK_END,OnMenuAddOwnSel)
	ON_COMMAND_RANGE(ID_MENU_IOVIEWREPORT_MORE_BGN1, ID_MENU_IOVIEWREPORT_MORE_END1, OnIoViewReportMenuBlockMoreLBtnDown)
	ON_COMMAND_RANGE(ID_MENU_IOVIEWREPORT_MORE_BGN2, ID_MENU_IOVIEWREPORT_MORE_END2, OnIoViewReportMenuBlockMoreRBtnDown)
	ON_COMMAND_RANGE(ID_REPORT_AUTOPAGE, ID_REPORT_AUTOPAGESETTING, OnAutoPage)
	ON_COMMAND_RANGE(ID_IOVIEWREPORT_NEW_BEGIN, ID_IOVIEWREPORT_NEW_END, OnIoViewReportMenuTabCollection)
	ON_COMMAND_RANGE(ID_IOVIEWREPORT_NEW_TOP_BEGIN, ID_IOVIEWREPORT_NEW_TOP_END, OnIoViewReportMenuTabTopCollection)
	ON_MESSAGE_VOID(UM_DO_INITFROMXML, OnDoInitFromXml)
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_IoViewTitle_Button_LButtonDown,OnMessageTitleButton)
	ON_MESSAGE(UM_IoViewReport_SortByRisePercent,OnMessageRisePercent)
	ON_MESSAGE(UM_IoViewReport_Finance,OnMessageFinance)
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////
CIoViewReport::CIoViewReport() : CIoViewBase()
{
	m_pParent		= NULL;
	m_rectClient	= CRect(-1,-1,-1,-1);
	m_RectGrid		= CRect(-1,-1,-1,-1);
	m_RectVert		= CRect(-1,-1,-1,-1);
	m_iCurTab		= -1;
	m_iIsReadCurTab = -1;
	m_iCurTabTmp	= -1;
	m_iPreTab		= -1;
	m_iSelStockTab  = -1;

	m_MmiRequestSys.m_iCount = 0;
	m_MmiRequestSys.m_bDescSort = true;
	m_MmiRequestSys.m_eMerchReportField = EMRFRisePercent;
	m_MmiRequestSys.m_iMarketId = -1;
	m_MmiRequestSys.m_iStart	= 0;

	m_MmiBlockRequestSys.m_eCommType = ECTReqBlockReport;
	m_MmiBlockRequestSys.m_iCount = 0;
	m_MmiBlockRequestSys.m_bDescSort = true;
	m_MmiBlockRequestSys.m_eMerchReportField = EMRFRisePercent;
	m_MmiBlockRequestSys.m_iBlockId = -1;
	m_MmiBlockRequestSys.m_iStart	= 0;

	m_MmiReqPlugInSort.m_iMarketId = -1;
	m_MmiReqPlugInSort.m_iStart = 0;
	m_MmiReqPlugInSort.m_iCount = -1;
	m_MmiReqPlugInSort.m_bDescSort = true;
	m_MmiReqPlugInSort.m_eReportSortType = ERSPIEnd;

	m_iSortColumn		= 0;
	m_bShowGridLine		= false;
	m_bRequestViewSort	= false;
	m_bLockSelChange	= false;
	m_SizeLast			= CSize(0,0);	

	m_aUserBlockNames.RemoveAll();
	m_CellRButtonDown.col = -1;
	m_CellRButtonDown.row = -1;

	m_StrAsyncExcelFileName = L"";
	m_eMarketReportType	= ERTNone;

	m_bIsStockSelectedReport = false;

	m_bAutoPageDirection = true;
	m_iAutoPageTimerCount = 0;
	m_iAutoPageTimerPeriod = INT_MAX;		// ��ֹ�Զ���ҳ

//	m_bInitedFromXml	=	false;	// ��û����ɳ�ʼ��
	m_iMaxGridRowCount	= 0;
	m_iMaxCustomGridRowCount = 0;

	m_bEnableUserBlockChangeResp = true;
	m_bAutoSizeColFromXml = true;
	m_bSuccessAutoSizeColXml = false;

	m_RctSperator = CRect(-1,-1,-1,-1);
	m_RctCustom = CRect(0,0,0,0);
	m_bShowCustomGrid = FALSE;
	m_bShowUserBlock = FALSE;
	m_bIsUserBlock	 = FALSE;
	m_bShowTextTip   = false;
	m_bCustomGrid	  = FALSE;

	m_sortType = 1;
	m_bIsShowGridVertScorll = FALSE;
	m_bShowTab		  = false;
	m_bShowTabByTabbar = false;

	m_bDragZxMerch = false;
	m_bDragMerchLBDown = false;
	m_pDragMerchBegin = NULL;
	m_iRowDragBegin = 0;
	m_iDragGridType = 0;
	m_iMarketID = -1;
	m_bLockReport = false;
	m_iFixCol = -1;

	m_eReportHeadType = ERHT_Count;
	m_bShowMoreColumn = true;

	m_rcTopBtn = CRect(0,0,0,0);
	m_mapTopTabbarGroup.clear();
	m_mapTopTabbarBtn.clear();
	m_bIsShowTopTabbar = FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewReport::~CIoViewReport()
{
	if ( NULL != m_pSubject )
	{
		m_pSubject->DelObserver(this);
	}

	// MainFrame �п�ݼ�
	CMainFrame* pMainFrame =(CMainFrame*)AfxGetMainWnd();
	if ( NULL != pMainFrame )
	{
		pMainFrame->SetHotkeyTarget(NULL);
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
void CIoViewReport::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if ( !IsWindowVisible() )
	{
		return;
	}

	// liuxt ��ע�͵�����֪����ʲô�����ô����ᵼ�������ָ�����ʱ�������ʾ�쳣
	if ( GetParentGGTongViewDragFlag() || m_bDelByBiSplitTrack )
	{
		LockRedraw();
		return;
	}

	//
	UnLockRedraw();

	CRect rectClient;
	GetClientRect(&rectClient);

	CRect RectTab;
	m_GuiTabWnd.GetWindowRect(&RectTab);
	ScreenToClient(RectTab);	

	CRect RectDraw;
	RectDraw = rectClient;

	if ( 0 == m_aTabInfomations.GetSize() )
	{
		m_GuiTabWnd.ShowWindow(SW_HIDE);
		dc.FillSolidRect(&RectTab, GetIoViewColor(ESCBackground));
	}
	else
	{
		m_GuiTabWnd.ShowWindow(SW_SHOW);
	}

	if ( m_GridCtrl.GetSafeHwnd() && m_GridCtrl.IsWindowVisible() )
	{	
		m_GridCtrl.RedrawWindow();
		// ˢ�¿ؼ�����������˿ؼ���ͷ
		m_GridCtrl.Refresh();
	}

	if (m_bShowTab)
	{
		RectDraw.top   = RectTab.top;
		RectDraw.left  = RectTab.right;	
		dc.FillSolidRect(&RectDraw, GetIoViewColor(ESCBackground));
	}

	if (m_bShowCustomGrid)
	{
		dc.SetTextColor(RGB(255,255,255));
		CFont font;
		font.CreateFont(-15,0,0,0,400,0,0,0,0,1,2,1,34, gFontFactory.GetExistFontName(L"΢���ź�"));	///
		CFont *pFontOld = dc.SelectObject(&font);
		dc.DrawText(L"��ѡ��", m_RctSperator, DT_VCENTER| DT_SINGLELINE);
		dc.SelectObject(pFontOld);

		CDC memDC;
		memDC.CreateCompatibleDC(&dc);
		CBitmap bmp;

		CRect rcBtn = m_RctSperator;
		rcBtn.left += 80;
		rcBtn.top += 2;
		rcBtn.right = rcBtn.left + 50;
		rcBtn.bottom = rcBtn.top + 16;

		CRect rcClient;
		GetClientRect(&rcClient);
		bmp.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
		memDC.SelectObject(&bmp);
		memDC.SetBkMode(TRANSPARENT);
		Gdiplus::Graphics graphics(memDC.GetSafeHdc());

		m_BtnMid.SetRect(&rcBtn);
		m_BtnMid.DrawButton(&graphics);

		dc.BitBlt(rcBtn.left, rcBtn.top, rcBtn.Width(), rcBtn.Height(), &memDC, rcBtn.left, rcBtn.top, SRCCOPY);
		dc.SelectClipRgn(NULL);
		font.DeleteObject();
		bmp.DeleteObject();
		memDC.DeleteDC();
	}

	if (m_bIsShowTopTabbar)
	{
		CDC memDC;
		memDC.CreateCompatibleDC(&dc);
		CBitmap bmp;
		dc.FillSolidRect(&m_rcTopBtn, GetIoViewColor(ESCBackground));

		//CRect rcClient;
		//GetClientRect(&rcClient);

		bmp.CreateCompatibleBitmap(&dc, m_rcTopBtn.Width(), m_rcTopBtn.Height());
		memDC.SelectObject(&bmp);
		memDC.SetBkMode(TRANSPARENT);
		Gdiplus::Graphics graphics(memDC.GetSafeHdc());


		map<int32, CNCButton>::iterator iter;
		for (iter = m_mapTopTabbarBtn.begin(); iter != m_mapTopTabbarBtn.end(); iter++)
		{
			iter->second.DrawButton(&graphics);
			CRect rcTemp;
			iter->second.GetRect(rcTemp);
			dc.BitBlt(rcTemp.left, rcTemp.top, rcTemp.Width(), rcTemp.Height(), &memDC, rcTemp.left, rcTemp.top, SRCCOPY);
		}

		dc.SelectClipRgn(NULL);
		bmp.DeleteObject();
		memDC.DeleteDC();

		dc.MoveTo(m_rcTopBtn.left, m_rcTopBtn.bottom - 1);  
		dc.LineTo(m_rcTopBtn.right, m_rcTopBtn.bottom - 1);  

		dc.MoveTo(m_rcTopBtn.left, m_rcTopBtn.top);  
		dc.LineTo(m_rcTopBtn.right, m_rcTopBtn.top);  

	}

	if ( m_bShowTextTip )
	{
		CRect rect(m_RectGrid);
		rect.top = m_RectGrid.top;
		rect.bottom = rect.top + (m_RectGrid.bottom - m_RectGrid.top)/3;
		m_GridCtrl.MoveWindow(rect);

		CRect rcShow(rectClient);
		rcShow.top = rect.bottom ;
		dc.FillSolidRect(&rcShow,GetIoViewColor(ESCBackground));
		dc.SetTextColor(RGB(128,128,128));

		CRect rcText(rcShow);
		rcText.bottom = rcText.top + 30;

		CFont font;
		font.CreateFont(-15,0,0,0,400,0,0,0,0,1,2,1,34, gFontFactory.GetExistFontName(L"����"));	///
		CFont *pFontOld = dc.SelectObject(&font);
		dc.DrawText(L"��Ǹ���������޷��ϸ�ģ�͵Ĺ�Ʊ��", rcText, DT_VCENTER| DT_SINGLELINE|DT_CENTER);
		dc.SelectObject(pFontOld);
	}

	if ( m_GuiTabWnd.GetSafeHwnd() && m_GuiTabWnd.IsWindowVisible() )
	{		
		m_GuiTabWnd.RedrawWindow();
	}

	if (m_bShowUserBlock && m_bIsUserBlock)
	{
		CRect rcClient;
		CRect rcAreaOffset(1, 1, 1, 1);
		GetClientRect(&rcClient);

		CRect rcBkremainder;		// ʣ��û�����ĵط�
		CRect rcXSBHorz;
		CRect rect(m_RectGrid);
		rect.top = m_RectGrid.top;
		CRect RectHScroll(m_RctSperator);
		m_XSBHorz.GetClientRect(rcXSBHorz);
		if (0 != rcXSBHorz.Width())
		{
			RectHScroll.bottom = m_RctSperator.top;
			RectHScroll.top    = RectHScroll.bottom - rcXSBHorz.Height();
			m_XSBHorz.SetSBRect(RectHScroll, TRUE);
			rect.bottom = RectHScroll.top;
		}
		else
		{
			rect.bottom = m_RctSperator.top;
		}
		
		
		m_GridCtrl.MoveWindow(rect);

		// �ײ��������ϲ�������
		CRect rcSperatorOffset = m_RctSperator;
		rcSperatorOffset.DeflateRect(rcAreaOffset);
		dc.FillSolidRect(m_RctSperator, RGB(0,0,5));
		dc.FillSolidRect(rcSperatorOffset, RGB(42,42,50));

		CDC memDC;
		memDC.CreateCompatibleDC(&dc);
		CBitmap bmp;
		bmp.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
		memDC.SelectObject(&bmp);
		memDC.SetBkMode(TRANSPARENT);

		Gdiplus::Graphics graphics(memDC.GetSafeHdc());
		CRect rcDisBtn = m_RctSperator;
		if (m_BtnStockList.GetSize() > 0)
		{
			int iSpacing = 60;
			rcDisBtn.right = rcDisBtn.left + iSpacing;
			for (int i = 0; i < m_BtnStockList.GetSize(); ++i)
			{
				m_BtnStockList.GetAt(i).SetRect(&rcDisBtn);
				m_BtnStockList.GetAt(i).SetAreaOffset(rcAreaOffset);
				m_BtnStockList.GetAt(i).DrawButton(&graphics);
				dc.BitBlt(rcDisBtn.left, rcDisBtn.top, rcDisBtn.Width(), rcDisBtn.Height(), &memDC, rcDisBtn.left, rcDisBtn.top, SRCCOPY);
				rcDisBtn.right -= 1;
				rcDisBtn.left = rcDisBtn.right;
				rcDisBtn.right = rcDisBtn.left + iSpacing;
			}
		}
		
		dc.SelectClipRgn(NULL);
		bmp.DeleteObject();
		memDC.DeleteDC();
	}
 	else 
 	{
		if (0 != m_RctSperator.Width())
		{
			CRect rect(m_RectGrid);
			rect.top = m_RectGrid.top;
			rect.bottom = m_RctSperator.bottom;
			m_GridCtrl.MoveWindow(rect);
		}
 	}
}


void CIoViewReport::TopTabBarHitTest(CPoint point)
{
	map<int32, CNCButton>::iterator iter;
	for (iter=m_mapTopTabbarBtn.begin(); iter!=m_mapTopTabbarBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		btnControl.SetCheckStatus(FALSE);
		// ��point�Ƿ����ѻ��Ƶİ�ť������
		if (btnControl.PtInButton(point))
		{
			btnControl.SetCheck(TRUE);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnLButtonDown
void CIoViewReport::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_bShowCustomGrid && m_BtnMid.PtInButton(point))
	{
		m_BtnMid.LButtonDown();
	}

	if (m_bShowUserBlock && m_bIsUserBlock)
	{
		for (int i = 0; i < m_BtnStockList.GetSize(); ++i)
		{

			if (m_BtnStockList.GetAt(i).PtInButton(point))
			{
				m_BtnStockList.GetAt(i).LButtonDown();
				m_BtnStockList.GetAt(i).MouseHover();
				break;
			}
		}
	}

	if (m_bIsShowTopTabbar)
	{
		TopTabBarHitTest(point);
	}

	CStatic::OnLButtonDown(nFlags, point);
}

void CIoViewReport::OnBtnResponseEvent(UINT nButtonId)
{
	if (MID_BUTTON_USER_BLOCK_ADD == nButtonId)
	{
		CDlgUserBlockAddMerch cDlgAddMerch;
		cDlgAddMerch.DoModal();
	}
	else if (MID_BUTTON_USER_BLOCK_DEL == nButtonId)
	{
		OnIoViewReportMenuDelOwnSel();
	}
	else if (MID_BUTTON_USER_BLOCK_CLEN == nButtonId)
	{
		OnIoViewReportMenuDelALLOwnSel();
	}
	else if (MID_BUTTON_USER_BLOCK_IMP == nButtonId)
	{
		CDlgImportUserBlockBySoft cdlgImportUserBlockBySoft;
		cdlgImportUserBlockBySoft.DoModal();
	}
	else if (MID_BUTTON_USER_BLOCK_EXP == nButtonId)
	{
		int32 iTab;
		T_BlockDesc::E_BlockType eType;
		if ( TabIsValid(iTab, eType))
		{
			T_BlockDesc BlockDesc = m_aTabInfomations.GetAt(iTab).m_Block;
			if (T_BlockDesc::EBTUser == eType)
			{
				ExportUserBlockToExcel(_T("���۱�"), BlockDesc.m_StrBlockName, _T(""), _T(""));
			}
			else
			{
				if (m_bShowCustomGrid)
				{
					ExportGridToExcel(&m_GridCtrl, &m_CustomGridCtrl, _T("���۱�"), BlockDesc.m_StrBlockName, _T(""), _T(""), true);
				}
				else
				{
					ExportGridToExcel(&m_GridCtrl, NULL, _T("���۱�"), BlockDesc.m_StrBlockName, _T(""), _T(""), true);
				}
			}	
		}	
	}

	// ������֮��ȥ������
	if (m_bShowUserBlock && m_bIsUserBlock)
	{
		for (int i = 0; i < m_BtnStockList.GetSize(); ++i)
		{
			m_BtnStockList.GetAt(i).MouseLeave();	
		}
	}
}


void CIoViewReport::OnTopTabBarDisPlateMenu(UINT nButtonId)
{
	if (m_mapTopTabbarGroup.count(nButtonId) < 1)
	{
		return;
	}
	if (m_mapTopTabbarBtn.count(nButtonId) < 1)
	{
		return;
	}

	T_TopTabbar &TopTabbar = m_mapTopTabbarGroup[nButtonId];
	CRect rcBtn;
	m_mapTopTabbarBtn[nButtonId].GetRect(rcBtn);
	ClientToScreen(&rcBtn);

	CPoint ptPos;
	ptPos.x = rcBtn.left;
	ptPos.y = rcBtn.bottom;

	//
	//
	CNewMenu menu;		
	menu.LoadMenu(IDR_MENU_REPORT_NEW);
	menu.LoadToolBar(g_awToolBarIconIDs);

	CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
	
	ASSERT(NULL != pPopMenu);
	pPopMenu->LoadToolBar(g_awToolBarIconIDs);

	if (pPopMenu)
	{
		LOGFONT *pFont = GetIoViewFont(ESFNormal);
		if (pFont)
		{
			//pFont->lfHeight = 
			pPopMenu->SetMenuTitleFont(pFont);
		}
		
	}

	// ��ȡ��ǰѡ��İ��
	T_TabInfo stTabInfo; 
	//if (m_aTabInfomations.GetSize() >  m_iCurTab)
	//{
		//stTabInfo = m_aTabInfomations.GetAt(m_iCurTab);
	//}


	if ( T_BlockDesc::EBTBlockLogical == TopTabbar.m_iBlockType )
	{
		// �߼����˵�
		int32 iID = TopTabbar.m_iBlockid;

		if ( KiIDTabAll == iID )
		{
			// ��ʾȫ��
			CBlockConfig::IdArray aIdAll;
			CBlockConfig::Instance()->GetCollectionIdArray(aIdAll);
			//
			for ( int32 i = 0; i < aIdAll.GetSize(); i++ )
			{
				if ( aIdAll[i] == KiIDTabAllExcept )
				{
					continue;
				}

				//
				CBlockCollection *pCol = CBlockConfig::Instance()->GetBlockCollectionById(aIdAll[i]);
				CBlockConfig::BlockArray aBlocks;
				pCol->GetValidBlocks(aBlocks);

				if ( aBlocks.GetSize() > 0 
					&& ( aBlocks[0]->m_blockInfo.GetBlockType() == CBlockInfo::typeNormalBlock				// ��ͨ || ����
					|| aBlocks[0]->m_blockInfo.GetBlockType() == CBlockInfo::typeMarketClassBlock )					
					)
				{
					CNewMenu *pColMenu = pPopMenu->AppendODPopupMenu(pCol->m_StrName);

					//
					ASSERT(NULL != pColMenu);

					//
					for ( int32 iBlock = 0; iBlock < aBlocks.GetSize(); iBlock++ )
					{						
						if ( NULL == aBlocks[iBlock] )
						{
							//ASSERT(0);
							continue;
						}

						//
						CString StrItem = aBlocks[iBlock]->m_blockInfo.m_StrBlockName;

						if ( StrItem.GetLength() > 7 )
						{
							StrItem = StrItem.Left(6) + _T("...");
						}

						//
						pColMenu->AppendODMenu(StrItem, MF_STRING, ID_IOVIEWREPORT_NEW_TOP_BEGIN + aBlocks[iBlock]->m_blockInfo.m_iBlockId);

						
						
						// ��ǵ�ǰ��ѡ��
						if ( m_CurSelectBlockInfo.m_iMarketID == aBlocks[iBlock]->m_blockInfo.m_iBlockId  && m_CurSelectBlockInfo.m_eType == TopTabbar.m_iBlockType)
						{
							pPopMenu->CheckMenuItem(ID_IOVIEWREPORT_NEW_TOP_BEGIN + aBlocks[iBlock]->m_blockInfo.m_iBlockId, MF_BYCOMMAND | MF_CHECKED );
						}
					}

					MultiColumnMenu(*pColMenu, 20);
				}
			}				
		}
		else 
		{
			//
			CBlockCollection* pBlockCollection = CBlockConfig::Instance()->GetBlockCollectionById(iID);
			if ( NULL == pBlockCollection )
			{
				return;
			}

			//
			for ( int32 i = 0; i < pBlockCollection->m_aBlocks.GetSize(); i++ )
			{
				CBlockLikeMarket* pBlock = pBlockCollection->m_aBlocks[i];
				if ( NULL == pBlock )
				{
					continue;
				}

				//
				pPopMenu->AppendODMenu(pBlock->m_blockInfo.m_StrBlockName, MF_STRING, ID_IOVIEWREPORT_NEW_TOP_BEGIN + pBlock->m_blockInfo.m_iBlockId);

				// ��ǵ�ǰѡ��
				if ( m_CurSelectBlockInfo.m_iMarketID == pBlock->m_blockInfo.m_iBlockId  && m_CurSelectBlockInfo.m_eType == TopTabbar.m_iBlockType )
				{
					pPopMenu->CheckMenuItem(ID_IOVIEWREPORT_NEW_TOP_BEGIN + pBlock->m_blockInfo.m_iBlockId, MF_BYCOMMAND | MF_CHECKED );
				}				

				

				// TRACE(L"�˵���: %s ID: %d \n", pBlock->m_blockInfo.m_StrBlockName, ID_IOVIEWREPORT_NEW_BEGIN + pBlock->m_blockInfo.m_iBlockId);
			}	

			MultiColumnMenu(*pPopMenu, 20);
		}
	}
	else if ( T_BlockDesc::EBTUser == TopTabbar.m_iBlockType)
	{
		// �û����˵�
		CArray<T_Block, T_Block&> aBlocks;
		CUserBlockManager::Instance()->GetBlocks(aBlocks);

		//
		for ( int32 i = 0; i < aBlocks.GetSize(); i++ )
		{
			T_Block stBlock = aBlocks[i];
			pPopMenu->AppendODMenu(stBlock.m_StrName, MF_STRING | MF_BYCOMMAND, ID_IOVIEWREPORT_NEW_TOP_END - i);

			// ��ǵ�ǰѡ��
			if ( m_CurSelectBlockInfo.m_StrBlockName == stBlock.m_StrName && m_CurSelectBlockInfo.m_eType == TopTabbar.m_iBlockType)
			{
				pPopMenu->CheckMenuItem(ID_IOVIEWREPORT_NEW_TOP_END - i, MF_BYCOMMAND | MF_CHECKED);
			}			
		}

		MultiColumnMenu(*pPopMenu, 20);
	}
	else if ( T_BlockDesc::EBTOpenBlockLogical == TopTabbar.m_iBlockType)
	{
		// �߼����		
		CBlockLikeMarket* pBlock = CBlockConfig::Instance()->FindBlock(TopTabbar.m_iBlockid);
		if ( NULL == pBlock )
		{
			return;
		}


		m_iCurTabTmp = 0;
		OnIoViewReportMenuTabCollection(TopTabbar.m_iBlockid + ID_IOVIEWREPORT_NEW_BEGIN);


		return;
	}

	m_CurTopTabbar = TopTabbar;
	// ɾ����һ���ָ���
	pPopMenu->RemoveMenu(0, MF_BYPOSITION | MF_SEPARATOR);

	// ������ù̶�һ���ط������˵�	
	pPopMenu->TrackPopupMenu(TPM_TOPALIGN | TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL, ptPos.x, ptPos.y, AfxGetMainWnd());
	menu.DestroyMenu();	
}


void CIoViewReport::OnTopTabBarResponseEvent(UINT nButtonId)
{
	if (m_mapTopTabbarGroup.empty())
	{
		return;
	}
	
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if (!pMainFrm)
	{
		return;
	}


	if (m_mapTopTabbarGroup.count(nButtonId) < 1)
	{
		return;
	}

	// ֻ�е����ѡ�ɲ���ʾ�ײ���ť
	//m_bShowUserBlock = false;

	T_TopTabbar &TopTabbar = m_mapTopTabbarGroup[nButtonId];
	switch(TopTabbar.m_iBtnType)
	{
	case ETAB_CHANGE_MARKET:
		{
			if(TopTabbar.m_iMarketID != -1)
			 {
				 pMainFrm->OpenSpecifyMarketOrBlock(TopTabbar.m_iMarketID);
			 }

			//
			if (m_bShowTabByTabbar)
			{
				m_bShowTab = false;
				ReCalcSize();
			}
		}
		break;
		case ETAB_OPEN_CFM:
		{
			if(!TopTabbar.m_strCfmName.IsEmpty())
			{
				pMainFrm->LoadSpecialCfm(TopTabbar.m_strCfmName);
			}
		}
		break;
		case ETAB_PLATE_MENU:
		{
			OnTopTabBarDisPlateMenu(nButtonId);
			
			//
			if (m_bShowTabByTabbar)
			{
				m_bShowTab = true;
				ReCalcSize();
			}
		}
		break;
		case ETAB_CUSTOM_ORDER:
		{

		}
		break;
	case ETAB_CHANGE_REPORTHEAD:
		{
			// ���۱��ͷ�л�
			if (ERHT_CapitalFlow == TopTabbar.m_eReportHeadType)		// �ʽ�����
			{
				m_eMarketReportType = ERTCapitalFlow;
			}
			else if (ERHT_FinancialData == TopTabbar.m_eReportHeadType)	// ��������
			{
				m_eMarketReportType = ERTMainMasukura;
			}
			else if (ERHT_MainMasukura == TopTabbar.m_eReportHeadType)	// ��������
			{
				m_eMarketReportType = ERTFinance;
			}
			else
			{
				// 
			}

			//
			m_eReportHeadType = TopTabbar.m_eReportHeadType;
			TabChange();
		}
		break;
	case ETAB_UserBlock:
		{
			// ֻ�ԡ���ѡ�ɡ�������Ч
			CString StrWspName = _T("");
			CMPIChildFrame *pChildFrame = (CMPIChildFrame *)pMainFrm->MDIGetActive();

			if (NULL == pChildFrame)
			{
				return;
			}

			StrWspName = pChildFrame->GetIdString();
			// ˢ������ѡ����Ϣ
			if (L"��ѡ��" != StrWspName)
			{
				return;
			}

			CArray<T_Block, T_Block&> aBlocks;
			CUserBlockManager::Instance()->GetBlocks(aBlocks);

			if ( aBlocks.IsEmpty() )
			{
				return;
			}

			int iSize = aBlocks.GetSize();
			T_Block Block;
			BOOL bIsShowUserblock = FALSE;
			for (int i=0; i<iSize; i++)
			{
				CString StrHotKey = aBlocks.GetAt(i).m_StrHotKey;
				if ( 0 == StrHotKey.CompareNoCase(L"THJY") )
				{
					//
					bIsShowUserblock = TRUE;
					Block = aBlocks.GetAt(i);
					T_BlockDesc BlockDesc;
					BlockDesc.m_eType		 = T_BlockDesc::EBTUser;
					BlockDesc.m_StrBlockName = Block.m_StrName;
					BlockDesc.m_iMarketId	 = -1;
					BlockDesc.m_aMerchs.Copy(Block.m_aMerchs);
					pMainFrm->OnSpecifyBlock(BlockDesc);

					break;
				}
			}

			// ��û�е��У�����ˢ��һ��λ��
			/*if (bIsShowUserblock)
			{
				if (!m_bShowUserBlock)
				{
					m_bShowUserBlock = true;
					ReCalcSize();	
				}
				m_bShowUserBlock = true;

			}*/
		}
		break;
	case ETAB_RecentView:
		{
			// 
			CArray<T_RecentMerch, T_RecentMerch&> aRecentMerchList;	
			pMainFrm->GetRecentMerchList(aRecentMerchList);
			CArray<CMerch*, CMerch*> aMerchs;

			int iSize = aRecentMerchList.GetSize();
			for (int i=0; i<iSize; i++)
			{
				T_RecentMerch &stMerch = aRecentMerchList.GetAt(i);
				if (stMerch.m_pMerch)
				{
					aMerchs.Add(stMerch.m_pMerch);
				}
			}

			T_BlockDesc blockDesc;
			blockDesc.m_eType			= T_BlockDesc::EBTRecentView;
			blockDesc.m_StrBlockName	= _T("������");
			blockDesc.m_iMarketId		= CBlockInfo::GetInvalidId();
			blockDesc.m_aMerchs.Copy(aMerchs);
			//
			OpenBlock(blockDesc);
			
			// ��Ҫˢ���µײ���ť����
			InvalidateRect(m_RctSperator);
		}
		break;
	default:
		break;
	}
}

void CIoViewReport::UserBlockPagejump()
{
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

	if (0 == aMerchs.GetSize())
	{
		CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
		if (pMainFrame)
		{
			CIoViewManager  *pIoViewManager	    = GetIoViewManager();
			if (pIoViewManager->GetActiveIoView() == this && pIoViewManager->m_GuiTabWnd.GetCount() > 1)
			{
				pIoViewManager->DealTabKey();
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnLButtonUp
void CIoViewReport::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bShowCustomGrid && m_BtnMid.PtInButton(point))
	{
		m_BtnMid.LButtonUp();
	}

	if (m_bShowUserBlock && m_bIsUserBlock)
	{
		for (int i = 0; i < m_BtnStockList.GetSize(); ++i)
		{

			if (m_BtnStockList.GetAt(i).PtInButton(point))
			{
				m_BtnStockList.GetAt(i).LButtonUp();
				break;
			}
		}
	}

	CStatic::OnLButtonUp(nFlags, point);
}

void CIoViewReport::CustomGridMouseWheel(short zDelta)
{
	if ( 0 == zDelta )
	{
		return;			
	}

	//linhc 20100910 ��ӱ��浱ǰ������
	CCellID CellFocusBK = m_CustomGridCtrl.GetFocusCell();

	// ���¹�����֤��ʾ��һ�У����Ϲ�����֤��ʾ���һ��
	// �������û��һҳ���򽫰�ҳ���հ�ҳ��ʾ����ҳ����ʽ��ʾ
	// �������û��һҳ���򽫵�һ����Ʒ��Ϊ��һ�У���������Ʒ���γ�һҳ
	// ��Ҫ��֤databeginΪ��һ����ʾ����Ʒ����

	////////////////////////////////////////////
	// 	int32 iTab = m_iCurTab;
	// 	{
	// 		int32 iSize = m_aTabInfomations.GetSize();
	// 		for (int32 i=0; i<iSize; i++)
	// 		{
	// 			if (m_aTabInfomations[i].m_Block.m_eType == T_BlockDesc::EBTUser)
	// 			{
	// 				iTab = i;
	// 				break;
	// 			}
	// 		}
	// 		
	// 		T_TabInfo stTabInfo = m_aTabInfomations.GetAt(iTab);
	// 		const T_SimpleTabInfo *pSimTab = GetSimpleTabInfo(iTab);
	// 		E_TabInfoType eTabInfoType = stTabInfo.m_eTabType;
	// 		T_BlockDesc::E_BlockType eBlockType = stTabInfo.m_Block.m_eType;
	// 		
	// 		int32 iIndex = 1;
	// 		CArray<T_Block, T_Block&> aBlocks;
	// 		CUserBlockManager::Instance()->GetBlocks(aBlocks);
	// 		
	// 		if ( aBlocks.GetSize() <= 0 )
	// 		{
	// 			//ASSERT(0);
	// 			return;
	// 		}
	// 		
	// 		if ( iIndex < 0 || iIndex >= aBlocks.GetSize() )
	// 		{
	// 			//ASSERT(0);
	// 			iIndex = 0;
	// 		}
	// 		
	// 		//
	// 		T_Block Block = aBlocks[iIndex];
	// 		
	// 		T_TabInfo stTabNew;
	// 		stTabNew.m_eTabType				= eTabInfoType;
	// 		stTabNew.m_StrShowName			= stTabInfo.m_StrShowName;
	// 		stTabNew.m_iDataIndexBegin		= stTabInfo.m_iDataIndexBegin;
	// 		stTabNew.m_iDataIndexEnd		= stTabInfo.m_iDataIndexEnd;
	// 		
	// 		stTabNew.m_Block.m_eType		= eBlockType;
	// 		stTabNew.m_Block.m_iMarketId	= 0;
	// 		stTabNew.m_Block.m_StrBlockName	= Block.m_StrName;
	// 		stTabNew.m_Block.m_aMerchs.Copy(Block.m_aMerchs);
	// 		m_aTabInfomations.SetAt(iTab, stTabNew);
	// 	}

	//
	T_TabInfo stTabInfo = m_tabCustom;//m_aTabInfomations.GetAt(iTab);

	// �������������Ϊ m_CustomGridCtrl.EnsureVisible ���䶯. ����һ��
	int32 iHScrolPosBk = m_CustomGridCtrl.GetScrollPos32(SB_HORZ);

	// ����ɼ��У���������EnsureVisible�����ˮƽ����
	CCellRange VisibleCells = m_CustomGridCtrl.GetVisibleNonFixedCellRange(NULL, FALSE, FALSE);
	int32 iVisibleCol = VisibleCells.GetMinCol();

	//int32 iDataNumsPerPage		= stTabInfo.m_iDataIndexEnd - stTabInfo.m_iDataIndexBegin + 1;
	int32 iDataNumsPerPage		= m_iMaxCustomGridRowCount;			// ��ǰ�������ʾ������
	int32 iVisibleNowDataBegin	= stTabInfo.m_iDataIndexBegin;		// ���Ǳ�֤��һ����ʾ����begin����
	int32 iVisibleNowDataEnd	= stTabInfo.m_iDataIndexBegin + VisibleCells.GetRowSpan()-1; // �����ʾ��һ���ɼ�����(��)

	int32 iIndexBeginNow;
	int32 iIndexEndNow;

	int32 iSizeMerchs;

	if ( T_BlockDesc::EBTPhysical == stTabInfo.m_Block.m_eType )
	{
		iSizeMerchs = stTabInfo.m_Block.m_pMarket->m_MerchsPtr.GetSize();
	}
	else
	{
		iSizeMerchs = stTabInfo.m_Block.m_aMerchs.GetSize();
	}

	ASSERT(iSizeMerchs > 0);

	if ( zDelta < 0 )
	{
		// �·�
		if ( iVisibleNowDataEnd >= iSizeMerchs - 1)
		{
			// ��������
			// 			if ( stTabInfo.m_iDataIndexBegin > 0 )
			// 			{
			// 				// ���ݳ��ȳ���һҳ������Ϊ��ѭ��������һҳ�Ķ���
			// 				iIndexBeginNow	= 0;
			// 				iIndexEndNow	= iIndexBeginNow + iDataNumsPerPage-1;
			// 				if ( iIndexEndNow >= iSizeMerchs )
			// 				{
			// 					iIndexEndNow	= iSizeMerchs - 1;		// ����������
			// 				}
			// 			}
			// 			else
			{
				// �������ݲ���һҳ�������������跭ҳ��
				m_CustomGridCtrl.EnsureVisible(m_CustomGridCtrl.GetRowCount() - 1, iVisibleCol);
				return;
			}
		}
		else
		{
			ASSERT( iVisibleNowDataEnd < iSizeMerchs-1 );

			iIndexBeginNow = iVisibleNowDataEnd+1; // �Կɼ��еĺ��1����Ϊ��һ�У��ж�����ʾ����
			iIndexEndNow   = iIndexBeginNow + iDataNumsPerPage - 1;

			//
			if ( iIndexEndNow >= iSizeMerchs )
			{
				iIndexEndNow	= iSizeMerchs - 1;		// ����������
			}
		}

		// ��beginNow��ʼ�����ݷ�Χ�Ѿ�ȷ��, EndNowһ������²����ɼ�
		// ����������beginNowȷ������
		m_CustomGridCtrl.EnsureVisible(m_CustomGridCtrl.GetFixedRowCount(), iVisibleCol);
	}
	else
	{		
		// �����ڿɼ���һ��-1��Ϊ����������һ��
		// ��֤����������һ�пɼ�
		// ��֤��һ��������databegin
		bool32 bMustSeeBottomRow = true;	// Ҫ��֤����пɼ�
		if ( iVisibleNowDataBegin <= 0 )
		{
			// 			if ( iVisibleNowDataEnd < iSizeMerchs-1 )
			// 			{
			// 				// ����û����ʾ��β�����ݣ���Ϊ��ѭ����ҳβ����ҳ��
			// 				iIndexEndNow	= iSizeMerchs-1;
			// 				iIndexBeginNow	= iIndexEndNow -iDataNumsPerPage +1;	// ������ݿ�������Ч��
			// 			}
			// 			else
			{
				// �������ݲ���һҳ���������
				if ( m_CustomGridCtrl.GetRowCount() > m_CustomGridCtrl.GetFixedRowCount() )
				{
					m_CustomGridCtrl.EnsureVisible(m_CustomGridCtrl.GetFixedRowCount(), iVisibleCol);
				}
				return;
			}
		}
		else
		{
			// �Ϸ�
			iIndexEndNow	= iVisibleNowDataBegin-1;
			iIndexBeginNow	= iIndexEndNow - iDataNumsPerPage + 1;		// ������ݿ�������Ч��
			ASSERT( iIndexEndNow <= iSizeMerchs-1 );
		}

		if ( bMustSeeBottomRow )
		{
			// Ҫȷ��ָ����EndNowΪ���һ�пɼ��������¸���BeginNowΪʵ�ʵ�һ�пɼ����ݶ�Ӧ����������
			// ��gridctrl���в���~~�����������У��ٿ��ж������ǿɼ���
			m_CustomGridCtrl.SetRowCount(iDataNumsPerPage+m_CustomGridCtrl.GetFixedRowCount());
			CCellRange rangeTest = m_CustomGridCtrl.GetVisibleNonFixedCellRange(NULL, TRUE, FALSE);
			iIndexBeginNow = iIndexEndNow - rangeTest.GetRowSpan() +1;
			if ( iIndexBeginNow < 0 )
			{
				iIndexBeginNow		= 0;	// ���ܱ仯��ʵ�����ݲ�û����ô��
				iIndexEndNow		= iIndexBeginNow +iDataNumsPerPage-1;
				bMustSeeBottomRow	= false;	// �Ѿ�����ͷ��������Ҫ�ٶ���ײ���
				if ( iIndexEndNow >= iSizeMerchs )
				{
					iIndexEndNow = iSizeMerchs-1;
				}
			}
			m_CustomGridCtrl.SetRowCount((iIndexEndNow-iIndexBeginNow+1)+m_CustomGridCtrl.GetFixedRowCount());
		}
		else
		{
			ASSERT( iIndexBeginNow >= 0 );
			m_CustomGridCtrl.EnsureVisible(m_CustomGridCtrl.GetFixedRowCount(), iVisibleCol);
		}
	}

	if ( iIndexBeginNow == stTabInfo.m_iDataIndexBegin && iIndexEndNow == stTabInfo.m_iDataIndexEnd )
	{
		// û�б仯
		return;
	}

	// TRACE(L"DataPerPage = %d Before: B = %d E = %d, Now: B = %d E = %d \n", iDataNumsPerPage, stTabInfo.m_iDataIndexBegin, stTabInfo.m_iDataIndexEnd, iIndexBeginNow, iIndexEndNow);

	// �������ʵ��������
	m_CustomGridCtrl.SetRowCount((iIndexEndNow-iIndexBeginNow+1)+m_CustomGridCtrl.GetFixedRowCount());

	//
	UpdateLocalDataIndex2(iIndexBeginNow, iIndexEndNow);

	//
	UpdateLocalXScrollPos2(iHScrolPosBk);
	m_CustomGridCtrl.SetScrollPos32(SB_HORZ, iHScrolPosBk);

	//
	ReSetCustomGridCellDataByIndex(0);

	// 	if ( m_bRequestViewSort )
	// 	{
	// 		// ��������, ��������Ժ������ǰ�ɼ���ʵʱ����
	// 		CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	// 		CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	// 		bool32 bCanReqSort = NULL==pApp?false:!pApp->m_bOffLine;
	// 		bCanReqSort = bCanReqSort && (NULL==pMainFrame? false:pMainFrame->GetUserConnectCmdFlag()&EUCCF_ConnectDataServer);
	// 		RequestViewDataSortAsync();
	// 		if ( !bCanReqSort )
	// 		{
	// 			// ������ԶҲ�ز�������ʾ��
	// 			UpdateCustomTableContent(TRUE, NULL, false);
	// 			RequestViewDataCurrentVisibleRowAsync();
	// 		}
	// 	}
	// 	else
	{
		// ����ǰ�ɼ�������
		UpdateCustomTableContent(TRUE, NULL, false);
		RequestViewDataCurrentVisibleRowAsync();
	}

	//linhc 20100910 �������ʱ��Ĭ�Ͻ�����,����֮ǰ����һ��Сbug
	//linhc 20100915�޸�
	CCellRange cellRange = m_CustomGridCtrl.GetSelectedCellRange();
	int32 iCurSel = cellRange.GetMinRow();
	CGridCellSys * pCellToDell = NULL;

	if ( iCurSel > 0 || iCurSel <= m_CustomGridCtrl.GetRowCount() - 1  )
	{   
		pCellToDell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iCurSel,0);
	}
	if ( NULL == pCellToDell )
	{
		m_CustomGridCtrl.SetFocusCell(CellFocusBK);
		//m_CustomGridCtrl.SetSelectedRange(1, 0, 1, m_CustomGridCtrl.GetColumnCount()-1, FALSE, TRUE);
		m_CustomGridCtrl.SetSelectedSingleRow(1);
	}
}

void CIoViewReport::OnMouseWheel(short zDelta, bool32 bMainGrid)
{
	if (!bMainGrid && IsInCustomGrid())
	{
		CustomGridMouseWheel(zDelta);
		return;
	}

	if ( 0 == zDelta )
	{
		return;			
	}

	if ( m_iCurTab < 0 || m_iCurTab >= m_aTabInfomations.GetSize() )
	{
		return;
	}
	//linhc 20100910 ��ӱ��浱ǰ������
	CCellID CellFocusBK = m_GridCtrl.GetFocusCell();

	// ���¹�����֤��ʾ��һ�У����Ϲ�����֤��ʾ���һ��
	// �������û��һҳ���򽫰�ҳ���հ�ҳ��ʾ����ҳ����ʽ��ʾ
	// �������û��һҳ���򽫵�һ����Ʒ��Ϊ��һ�У���������Ʒ���γ�һҳ
	// ��Ҫ��֤databeginΪ��һ����ʾ����Ʒ����

	//
	T_TabInfo stTabInfo = m_aTabInfomations.GetAt(m_iCurTab);

	// �������������Ϊ m_GridCtrl.EnsureVisible ���䶯. ����һ��
	int32 iHScrolPosBk = m_GridCtrl.GetScrollPos32(SB_HORZ);

	// ����ɼ��У���������EnsureVisible�����ˮƽ����
	CCellRange VisibleCells = m_GridCtrl.GetVisibleNonFixedCellRange(NULL, FALSE, FALSE);
	int32 iVisibleCol = VisibleCells.GetMinCol();

	//int32 iDataNumsPerPage		= stTabInfo.m_iDataIndexEnd - stTabInfo.m_iDataIndexBegin + 1;
	int32 iDataNumsPerPage		= m_iMaxGridRowCount;				// ��ǰ�������ʾ������
	int32 iVisibleNowDataBegin	= stTabInfo.m_iDataIndexBegin;		// ���Ǳ�֤��һ����ʾ����begin����
	int32 iVisibleNowDataEnd	= stTabInfo.m_iDataIndexBegin + VisibleCells.GetRowSpan()-1; // �����ʾ��һ���ɼ�����(��)

	int32 iIndexBeginNow;
	int32 iIndexEndNow;

	int32 iSizeMerchs = 0;

	if ( T_BlockDesc::EBTPhysical == stTabInfo.m_Block.m_eType )
	{
		if (NULL != stTabInfo.m_Block.m_pMarket)
		{
			iSizeMerchs = stTabInfo.m_Block.m_pMarket->m_MerchsPtr.GetSize();
		}		
	}
	else
	{
		iSizeMerchs = stTabInfo.m_Block.m_aMerchs.GetSize();
	}

	ASSERT(iSizeMerchs > 0);
	if(iSizeMerchs == 0)
	{

		//	ĳ����Ҳ�����Ʒ
		return;
	}
	

	if ( zDelta < 0 )
	{
		// �·�
		if ( iVisibleNowDataEnd >= iSizeMerchs - 1)
		{
			// ��������
			if ( stTabInfo.m_iDataIndexBegin > 0 )
			{
				// ���ݳ��ȳ���һҳ������Ϊ��ѭ��������һҳ�Ķ���
				iIndexBeginNow	= 0;
				iIndexEndNow	= iIndexBeginNow + iDataNumsPerPage-1;
				if ( iIndexEndNow >= iSizeMerchs )
				{
					iIndexEndNow	= iSizeMerchs - 1;		// ����������
				}
			}
			else
			{
				// ���������������ݲ���һҳ�������������跭ҳ��
				m_GridCtrl.EnsureVisible(m_GridCtrl.GetRowCount() - 1, iVisibleCol);
				SetPosForScroll(VK_NEXT);
				return;
			}	
		}
		else
		{
			ASSERT( iVisibleNowDataEnd < iSizeMerchs - 1 );

			iIndexBeginNow = iVisibleNowDataEnd+1; // �Կɼ��еĺ��1����Ϊ��һ�У��ж�����ʾ����
			iIndexEndNow   = iIndexBeginNow + iDataNumsPerPage - 1;			
			//
			if ( iIndexEndNow >= iSizeMerchs )
			{
				iIndexEndNow	= iSizeMerchs - 1;		// ����������
			}
		}

		// ��beginNow��ʼ�����ݷ�Χ�Ѿ�ȷ��, EndNowһ������²����ɼ�
		// ����������beginNowȷ������
		m_GridCtrl.EnsureVisible(m_GridCtrl.GetFixedRowCount(), iVisibleCol);

		SetPosForScroll(VK_NEXT);
	}
	else	// ���Ϲ���
	{		
		// �����ڿɼ���һ��-1��Ϊ����������һ��
		// ��֤����������һ�пɼ�
		// ��֤��һ��������databegin
		bool32 bMustSeeBottomRow = true;	// Ҫ��֤����пɼ�
		if ( iVisibleNowDataBegin <= 0 )
		{
			if ( iVisibleNowDataEnd < iSizeMerchs-1 )
			{
				// ����û����ʾ��β�����ݣ���Ϊ��ѭ����ҳβ����ҳ��
				iIndexEndNow	= iSizeMerchs-1;
				iIndexBeginNow	= iIndexEndNow -iDataNumsPerPage +1;	// ������ݿ�������Ч��
			}
			else
			{
				// �������ݲ���һҳ���������
				if ( m_GridCtrl.GetRowCount() > m_GridCtrl.GetFixedRowCount() )
				{
					m_GridCtrl.EnsureVisible(m_GridCtrl.GetFixedRowCount(), iVisibleCol);
				}
				SetPosForScroll(VK_PRIOR);
				return;
			}
		}
		else
		{
			// �Ϸ�
			iIndexEndNow	= iVisibleNowDataBegin-1;
			iIndexBeginNow	= iIndexEndNow - iDataNumsPerPage + 1;		// ������ݿ�������Ч��
			ASSERT( iIndexEndNow <= iSizeMerchs-1 );
		}

		if ( bMustSeeBottomRow )
		{
			// Ҫȷ��ָ����EndNowΪ���һ�пɼ��������¸���BeginNowΪʵ�ʵ�һ�пɼ����ݶ�Ӧ����������
			// ��gridctrl���в���~~�����������У��ٿ��ж������ǿɼ���
			m_GridCtrl.SetRowCount(iDataNumsPerPage+m_GridCtrl.GetFixedRowCount());
			CCellRange rangeTest = m_GridCtrl.GetVisibleNonFixedCellRange(NULL, TRUE, FALSE);
			iIndexBeginNow = iIndexEndNow - rangeTest.GetRowSpan() /*+1*/;
			if ( iIndexBeginNow < 0 )
			{
				iIndexBeginNow		= 0;	// ���ܱ仯��ʵ�����ݲ�û����ô��
				iIndexEndNow		= iIndexBeginNow +iDataNumsPerPage-1;
				bMustSeeBottomRow	= false;	// �Ѿ�����ͷ��������Ҫ�ٶ���ײ���
				if ( iIndexEndNow >= iSizeMerchs )
				{
					iIndexEndNow = iSizeMerchs-1;
				}
			}
			m_GridCtrl.SetRowCount((iIndexEndNow-iIndexBeginNow+1)+m_GridCtrl.GetFixedRowCount());
		}
		else
		{
			ASSERT( iIndexBeginNow >= 0 );
			m_GridCtrl.EnsureVisible(m_GridCtrl.GetFixedRowCount(), iVisibleCol);
		}

		SetPosForScroll(VK_PRIOR);
	}

	if ( iIndexBeginNow == stTabInfo.m_iDataIndexBegin && iIndexEndNow == stTabInfo.m_iDataIndexEnd )
	{
		// û�б仯
		return;
	}

	// TRACE(L"DataPerPage = %d Before: B = %d E = %d, Now: B = %d E = %d \n", iDataNumsPerPage, stTabInfo.m_iDataIndexBegin, stTabInfo.m_iDataIndexEnd, iIndexBeginNow, iIndexEndNow);

	// �������ʵ��������
	m_GridCtrl.SetRowCount((iIndexEndNow-iIndexBeginNow+1)+m_GridCtrl.GetFixedRowCount());

	//
	UpdateLocalDataIndex(m_iCurTab, iIndexBeginNow, iIndexEndNow);

	//
	UpdateLocalXScrollPos(m_iCurTab, iHScrolPosBk);
	m_GridCtrl.SetScrollPos32(SB_HORZ, iHScrolPosBk);

	//
	ReSetGridCellDataByIndex();

	if ( m_bRequestViewSort )
	{
		// ��������, ��������Ժ������ǰ�ɼ���ʵʱ����
		CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
		CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
		bool32 bCanReqSort = NULL==pApp?false:!pApp->m_bOffLine;
		bCanReqSort = bCanReqSort && (NULL==pMainFrame? false:pMainFrame->GetUserConnectCmdFlag()&EUCCF_ConnectDataServer);
		RequestViewDataSortAsync();
		if ( !bCanReqSort )
		{
			// ������ԶҲ�ز�������ʾ��
			UpdateTableContent(TRUE, NULL, false);
			RequestViewDataCurrentVisibleRowAsync();
		}
	}
	else
	{
		// ����ǰ�ɼ�������
		UpdateTableContent(TRUE, NULL, false);
		RequestViewDataCurrentVisibleRowAsync();
	}

	//linhc 20100910 �������ʱ��Ĭ�Ͻ�����,����֮ǰ����һ��Сbug
	//linhc 20100915�޸�
	CCellRange cellRange = m_GridCtrl.GetSelectedCellRange();
	int32 iCurSel = cellRange.GetMinRow();
	CGridCellSys * pCellToDell = NULL;

	if ( iCurSel > 0 || iCurSel <= m_GridCtrl.GetRowCount() - 1  )
	{   
		pCellToDell = (CGridCellSys *)m_GridCtrl.GetCell(iCurSel,0);
	}
	if ( NULL == pCellToDell )
	{
		m_GridCtrl.SetFocusCell(CellFocusBK);
		//m_GridCtrl.SetSelectedRange(1, 0, 1, m_GridCtrl.GetColumnCount()-1, FALSE, TRUE);
		m_GridCtrl.SetSelectedSingleRow(1);
	}

	//	��������£�ĳЩcell��Ȼ��ԭ�ˡ���˼������㡣ֻ���������ÿɼ���cell��
	ResetVisibleGridFont();;
}

bool32	CIoViewReport::IsInCustomGrid()
{
	CPoint pt(0,0);
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	if (m_RctCustom.PtInRect(pt))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CIoViewReport::PreTranslateMessage(MSG* pMsg)
{
	if ( WM_KEYDOWN == pMsg->message )
	{
		if ( VK_DELETE == pMsg->wParam )
		{
			// ɾ����ѡ��
			CGGTongApp * pApp = (CGGTongApp *)AfxGetApp();

			int32 iTab;
			T_BlockDesc::E_BlockType eType;

			if ( !TabIsValid(iTab, eType) )
			{
				return TRUE;
			}
			if ( T_BlockDesc::EBTUser != eType )
			{
				return TRUE;
			}

			if ( !pApp->m_bBindHotkey)
			{
				return TRUE;
			}

			OnIoViewReportMenuDelOwnSel();

			return TRUE;
		}		
		else if ( VK_UP == pMsg->wParam  )
		{
			if ( IsShiftPressed() )
			{
				// ����
				AdjustUserMerchPosition(true);
				return TRUE;
			}
			else if ( IsCtrlPressed() )
			{
				return CIoViewBase::PreTranslateMessage(pMsg);
			}

			SetPosForScroll(VK_UP);
		}
		else if ( VK_DOWN == pMsg->wParam )
		{
			if ( IsShiftPressed() )
			{
				// ����
				AdjustUserMerchPosition(false);
				return TRUE;
			}
			else if ( IsCtrlPressed() )
			{
				return CIoViewBase::PreTranslateMessage(pMsg);
			}

			SetPosForScroll(VK_DOWN);
		}
		// VK_RIGHT��VK_LEFT�����¼����ڴ�Return���ǲ��ٷ����Ӵ���
		else if ( VK_RIGHT == pMsg->wParam )
		{
			m_XSBHorz.ScrollRight();
			return TRUE;
		}
		else if ( VK_LEFT == pMsg->wParam )
		{
			m_XSBHorz.ScrollLeft();
			return TRUE;
		}
		else if ( VK_ESCAPE == pMsg->wParam )
		{
			// ������ESC
			// ����
			// 			OnEsc();
			// 			return TRUE;
		}
		else if ( VK_NEXT == pMsg->wParam )
		{
			// ���·�ҳ
			OnMouseWheel(-1);
			return TRUE;
		}
		else if ( VK_PRIOR == pMsg->wParam )
		{
			// ���Ϸ�ҳ
			OnMouseWheel(1);
			return TRUE;
		}
		else if ( 0xBD == pMsg->wParam )		// '-' ��������������� VK_OEM_MINUS 
		{
			PostMessage(WM_COMMAND, ID_REPORT_AUTOPAGE, NULL);
			return TRUE;
		}		
	}
	else if ( WM_MOUSEWHEEL == pMsg->message )
	{  
		// ���ع���
		short zDelta = HIWORD(pMsg->wParam);
		zDelta /= 120;

		// �Լ�����
		OnMouseWheel(zDelta);

		return TRUE;
	}
	else if (WM_LBUTTONDOWN == pMsg->message)
	{
		CPoint ptLBtnDown;
		GetCursorPos(&ptLBtnDown);
		if (BeValidDragArea(ptLBtnDown) && BeUserBlock())
		{
			m_bDragMerchLBDown = true;
			SetDragMerchBegin();
		}
		else
		{
			m_bDragMerchLBDown = false;
			SetDragMerchBegin(true);
		}
	}
	else if ( WM_LBUTTONUP == pMsg->message)
	{
		m_bDragMerchLBDown = false;
		if (m_bDragZxMerch)
		{
			HCURSOR hCur = AfxGetApp()->LoadCursor(IDC_SIZEWE);							
			::SetCursor(hCur);
			DragUserMerchPosition();
			m_bDragZxMerch = false;
		}
	}
	else if (WM_MOUSEMOVE == pMsg->message) 
	{
		if ( m_bDragMerchLBDown )
		{
			m_bDragZxMerch = true;

			HCURSOR hCur = AfxGetApp()->LoadCursor(IDC_CUR_DRAG_RIGHT);							
			::SetCursor(hCur);
		}	
	}

	return CIoViewBase::PreTranslateMessage(pMsg);
}

void CIoViewReport::OnRButtonDown(UINT nFlags, CPoint point)
{	
	CStatic::OnRButtonDown(nFlags, point);
}

void CIoViewReport::OnLayOutAdjust()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CView* pView = pFrame->FindGGTongView();
	if ( NULL != pView )
	{
		pView->PostMessage(WM_COMMAND,ID_LAYOUT_ADJUST,0);
	}
}

void CIoViewReport::OnIoViewReportGridLine()
{
	if (m_bShowGridLine)
	{
		m_GridCtrl.ShowGridLine(false);
		m_CustomGridCtrl.ShowGridLine(false);
	}
	else
	{
		m_GridCtrl.ShowGridLine(true);
		m_CustomGridCtrl.ShowGridLine(true);
	}

	m_bShowGridLine = !m_bShowGridLine;	
}

void CIoViewReport::OnMenuAddOwnSel(UINT nID)
{
	// �ȵõ��Ҽ����е���Ʒ:
	int32 iCol = m_CellRButtonDown.col;
	int32 iRow = m_CellRButtonDown.row;

	if ( 0 == iRow )
	{
		return;
	}

	CMerch *pMerch = NULL;

	CGridCellBase* pBaseCell = m_GridCtrl.GetCell(0,iCol);

	if( NULL == pBaseCell )
	{
		return;
	}

	CGridCellSys *pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
	if ( NULL == pCell )
	{
		return;
	}

	pMerch = (CMerch *)pCell->GetData();

	if (NULL == pMerch)
	{
		return;
	}

	int32 iIndex = nID - IDC_CHART_ADD2BLOCK_BEGIN;

	if ( iIndex < 0 || iIndex >= m_aUserBlockNames.GetSize())
	{
		return;
	}

	CString StrBlockName = m_aUserBlockNames.GetAt(iIndex);	

	//	
	CUserBlockManager::Instance()->AddMerchToUserBlock(pMerch, StrBlockName);

	// �ı������Ʊ����ɫ:
	UpdateTableContent(false, pMerch, true);
}

void StartAsyncExcel ( CGridCtrlSys* pGridCtrl,CString StrGuid ,OUT CString& StrFileName);
void AsyncGridToExcel (CGridCtrlSys* pGridCtrl,CString StrGuid,const CString& StrFileName, int32 iRow,COLORREF clrRise,COLORREF clrFall,COLORREF clrKeep );

///////////////////////////////////////////////////////////////////////////////
// OnMouseMove
void CIoViewReport::OnMouseMove(UINT nFlags, CPoint point)
{	
	if (m_bShowCustomGrid)
	{
		if (m_BtnMid.PtInButton(point))
		{
			m_BtnMid.MouseHover();
		}
		else
		{
			m_BtnMid.MouseLeave();
		}
	}

	CStatic::OnMouseMove(nFlags, point);
}

int CIoViewReport::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CIoViewBase::OnCreate(lpCreateStruct);

	InitialIoViewFace(this);	
	UseSpecialRecordFont();

	//����Tab ��
	m_GuiTabWnd.Create(WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,0x9999);
	m_GuiTabWnd.SetBkGround(false,RGB(42,42,50),0,0);
	m_GuiTabWnd.SetTabTextColor(RGB(182,183,185), RGB(228,229,230));
	m_GuiTabWnd.SetTabBkColor(RGB(42,42,50), RGB(230,70,70));

	m_GuiTabWnd.SetFontByName(_T("΢���ź�"), GB2312_CHARSET);

	int iHeight = -MulDiv(9, GetDeviceCaps(GetDC()->GetSafeHdc(), LOGPIXELSY), 72);

	m_GuiTabWnd.SetFontHeight(iHeight);
	m_GuiTabWnd.SetTabSpace(0);
	m_GuiTabWnd.SetUserCB(this);


	// �����������
	m_XSBVertical.m_bIsStone = true;	// ����stone�����Ĺ�����
	m_XSBVertical.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY,this,CRect(0,0,0,0), ID_VERTICAL_REPORT);
	m_XSBVertical.SetScrollRange(0, 100);

	m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY,this,CRect(0,0,0,0), 10203);
	m_XSBVert.SetScrollRange(0, 10);

	m_XSBHorz.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY,this,CRect(0,0,0,0), 10204);
	m_XSBHorz.SetScrollRange(0, 10);

	m_XSBVertCustom.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY,this,CRect(0,0,0,0), 10203);
	m_XSBVertCustom.SetScrollRange(0, 10);

	m_XSBHorzCustom.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY,this,CRect(0,0,0,0), 10204);
	m_XSBHorzCustom.SetScrollRange(0, 10);

	//m_XSBHorz.SetScrollBarLeftArrowH(-1);
	//m_XSBHorz.SetScrollBarRightArrowH(-1);
	//m_XSBHorz.SetBorderColor(GetIoViewColor(ESCBackground));

	/*m_XSBVertical.SetScrollBarLeftArrowH(-1);
	m_XSBVertical.SetScrollBarRightArrowH(-1);
	m_XSBVertical.SetBorderColor(GetIoViewColor(ESCBackground));*/

	m_XSBHorzCustom.SetScrollBarLeftArrowH(-1);
	m_XSBHorzCustom.SetScrollBarRightArrowH(-1);
	m_XSBHorzCustom.SetBorderColor(GetIoViewColor(ESCBackground));

	m_XSBVertCustom.SetScrollBarLeftArrowH(-1);
	m_XSBVertCustom.SetScrollBarRightArrowH(-1);
	m_XSBVertCustom.SetBorderColor(GetIoViewColor(ESCBackground));

	// �������ݱ��
	m_GridCtrl.Create(CRect(0, 0, 0, 0), this, ID_REPORT_GRID);
	//m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);	
	m_GridCtrl.EnablePolygonCorner(false);

	m_CustomGridCtrl.Create(CRect(0, 0, 0, 0), this, ID_REPORT_GRID);
	m_CustomGridCtrl.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
	m_CustomGridCtrl.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
	m_CustomGridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_CustomGridCtrl.EnablePolygonCorner(false);

	// �����໥֮��Ĺ���
	m_XSBHorz.SetOwner(&m_GridCtrl);
	m_XSBVert.SetOwner(&m_GridCtrl);
	m_XSBVertical.SetOwner(&m_GridCtrl);
	m_GridCtrl.SetScrollBar(&m_XSBHorz, &m_XSBVert);
	m_XSBHorz.AddMsgListener(m_hWnd);	// ����������Ϣ - ��¼������Ϣ
	m_XSBVertical.AddMsgListener(m_hWnd);

	m_XSBHorzCustom.SetOwner(&m_CustomGridCtrl);
	m_XSBVertCustom.SetOwner(&m_CustomGridCtrl);
	m_CustomGridCtrl.SetScrollBar(&m_XSBHorzCustom, &m_XSBVertCustom);
	m_XSBHorzCustom.AddMsgListener(m_hWnd);	// ����������Ϣ - ��¼������Ϣ

	// ���ñ�ͷ
	m_GridCtrl.SetHeaderSort(FALSE);
	m_GridCtrl.SetUserCB(this);
	m_CustomGridCtrl.SetHeaderSort(FALSE);
	m_CustomGridCtrl.SetUserCB(this);

	// ��ʾ����ߵķ��
	m_GridCtrl.ShowGridLine(false);
	m_GridCtrl.SetDrawFixedCellGridLineAsNormal(TRUE);
	m_GridCtrl.SetRowHeightFixed(TRUE);		// �̶��и�
	m_GridCtrl.SetSingleRowSelection(FALSE);
	m_GridCtrl.SetDrawSelectedCellStyle(GVSDS_DEFAULT);

	m_CustomGridCtrl.ShowGridLine(false);
	m_CustomGridCtrl.SetDrawFixedCellGridLineAsNormal(TRUE);
	m_CustomGridCtrl.SetRowHeightFixed(TRUE);		// �̶��и�
	m_CustomGridCtrl.SetSingleRowSelection(FALSE);
	m_CustomGridCtrl.SetDrawSelectedCellStyle(GVSDS_DEFAULT);
	
	////////////////////////////////////////////////////////////////
	
	m_GridCtrl.SetFixedRowCount(1);
	m_CustomGridCtrl.SetFixedRowCount(1);


	// ��������
	LOGFONT *pFontNormal = GetIoViewFont(ESFSmall);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);

	m_CustomGridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_CustomGridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_CustomGridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_CustomGridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);

	m_GridCtrl.SetAutoHideFragmentaryCell(false);
	m_CustomGridCtrl.SetAutoHideFragmentaryCell(false);
	

	// ��һ������һ�е����� 
	SetRowHeightAccordingFont();

	// ��Ǽ���
	CMarkManager::Instance().AddMerchMarkChangeListener(this);

	//
	SetTimer(KUpdatePushMerchsTimerId, KTimerPeriodPushMerchs, NULL);


	InitialSimpleTabInfo();
	//SaveSimpleTabInfoToXml(sm_aSimpleTabInfos.GetData(), sm_aSimpleTabInfos.GetSize());
	InitialTabInfos();
	ReCreateTabWnd();	// �ȵ���ɳ�ʼ��ʱ��setTab
	PostMessage(UM_DO_INITFROMXML);

	//
	int32 iTab;
	T_BlockDesc::E_BlockType eType;

	if ( !TabIsValid(iTab, eType))
	{
		return 0;
	}

	//if ( T_BlockDesc::EBTUser == eType )
	//{
	//	m_GridCtrl.SetEditable(true);
	//}

	m_BtnMid.CreateButton(L"�༭", &m_RctSperator, this, NULL, 0, MID_BUTTON_ID);
	m_BtnMid.SetTextBkgColor(RGB(25,25,25), RGB(146,96,0), RGB(25,25,25));
	m_BtnMid.SetTextFrameColor(RGB(100,100,100), RGB(255,255,255), RGB(255,255,255));
	m_BtnMid.SetTextColor(RGB(255,255,255), RGB(255,255,255), RGB(255,255,255));


	m_CustomGridCtrl.SetGridLines(GVL_NONE);  
	m_GridCtrl.SetGridLines(GVL_NONE);  

	return 0;
}

void CIoViewReport::CreateBtnStock()
{
	m_BtnStockList.RemoveAll();
	m_BtnAddStock.CreateButton(L"���", &m_RctSperator, this, NULL, 0, MID_BUTTON_USER_BLOCK_ADD);
	m_BtnDelStock.CreateButton(L"ɾ��", &m_RctSperator, this, NULL, 0, MID_BUTTON_USER_BLOCK_DEL);
	m_BtnCleanStock.CreateButton(L"���", &m_RctSperator, this, NULL, 0, MID_BUTTON_USER_BLOCK_CLEN);
	m_BtnImportStock.CreateButton(L"����", &m_RctSperator, this, NULL, 0, MID_BUTTON_USER_BLOCK_IMP);
	m_BtnExportStock.CreateButton(L"����", &m_RctSperator, this, NULL, 0, MID_BUTTON_USER_BLOCK_EXP);

	m_BtnStockList.Add(m_BtnAddStock);
	m_BtnStockList.Add(m_BtnDelStock);
	m_BtnStockList.Add(m_BtnCleanStock);
	m_BtnStockList.Add(m_BtnImportStock);
	m_BtnStockList.Add(m_BtnExportStock);

	COLORREF clrFixed = GetIoViewColor(ESCGridFixedBk);
	COLORREF clrBack = GetIoViewColor(ESCBackground);
	for(int i = 0; i < m_BtnStockList.GetSize(); ++i)
	{
		m_BtnStockList.GetAt(i).SetTextBkgColor(RGB(42, 42, 50), RGB(42, 42, 50), RGB(230, 70, 70));
		m_BtnStockList.GetAt(i).SetTextFrameColor(RGB(0, 0, 5), RGB(0, 0, 5), RGB(230, 70, 70));
		m_BtnStockList.GetAt(i).SetTextColor(RGB(220,220,220), RGB(220,220,220), RGB(255,255,255));
	}
}

BOOL CIoViewReport::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (MID_BUTTON_ID == wParam)
	{
		CDlgEditReport dlg;
		if(IDOK == dlg.DoModal())
		{
			InitialCustomMerch();
		}
		m_BtnMid.MouseLeave();
	}

	if (m_bShowUserBlock)
	{
		OnBtnResponseEvent(wParam);
	}


	if (m_bIsShowTopTabbar)
	{
		OnTopTabBarResponseEvent(wParam);
	}
	return CIoViewBase::OnCommand(wParam, lParam);
}


void CIoViewReport::ReCalcSize() 
{	
	CRect RectClient;
	GetClientRect(&RectClient);
	m_rectClient = RectClient;

	//
	CRect RectHScroll(RectClient);	
	CRect RectTabWnd(RectClient);
	CRect RectGrid(RectClient);

	int32 iHeightTabBar = m_GuiTabWnd.GetFitHorW();
	int32 iWidthItem = m_GuiTabWnd.GetFitFontW() * 5;  // ÿһ��Ŀ��, һ��5����
	int32 iHeightScroll = 12;




	// ��ǩҳ��λ��
	RectTabWnd.bottom = RectClient.bottom;
	// Ϊ�˱����������ߣ��ټ���һ������
	RectTabWnd.bottom += 1;
	RectTabWnd.top    = RectTabWnd.bottom - iHeightTabBar;


	//--- wangyongxue ����Tab��ǩ���ٶ�̬���䳤��
	int32 iTabCount = m_SimpleTabInfoClass.aTabInfos.size();
	int32 iTabWidth = iWidthItem * KiFixTabCounts;	   // tab�Ŀ��
	int32 iAllowMaxWidth = RectClient.Width() - 80;	   // tab����������
	if (0 == iWidthItem)
	{
		iWidthItem = 70;
	}
	// tab�Ŀ�ȴ�����ͼ��ȼ�80 �͵�����ͼ��ȼ�80
	if (iTabWidth > iAllowMaxWidth)
	{
		iTabWidth = (iAllowMaxWidth/iWidthItem) * iWidthItem;
	}
	else if(iTabCount < KiFixTabCounts)
	{
		iTabWidth = iWidthItem * iTabCount;
	}


	// ������ǩ����λ��
	if (m_bIsShowTopTabbar)
	{
		int32 iHeightTopTabBar = 22;
		int iBtnWidth = iWidthItem;
		m_rcTopBtn = RectClient;

		// Ϊ�˱����������ߣ��ٴμ�ȥһ������
		m_rcTopBtn.top -= 1;
		m_rcTopBtn.bottom = iHeightTopTabBar;

		RectGrid.top  =  m_rcTopBtn.bottom;


		CRect rcTopBtn = m_rcTopBtn;
		rcTopBtn.right = rcTopBtn.left;
		rcTopBtn.bottom = iHeightTopTabBar;
		map<int32, CNCButton>::iterator iter;
		for (iter = m_mapTopTabbarBtn.begin(); iter != m_mapTopTabbarBtn.end(); iter++)
		{
			CNCButton &ncButton = iter->second;
			rcTopBtn.left = rcTopBtn.right -1;
			rcTopBtn.right += iWidthItem;
			ncButton.SetRect(rcTopBtn);
		}
	}

	if (m_bShowTab)
	{
		RectTabWnd.right = RectTabWnd.left + iTabWidth + 10;

		// �����������λ��
		RectHScroll.bottom = RectTabWnd.top;
		RectHScroll.top    = RectHScroll.bottom - iHeightScroll;
		//RectHScroll.left   = RectTabWnd.right + 1;
		//RectHScroll.right  = RectClient.right;

		// ����λ��
		RectGrid.bottom	   = RectHScroll.top;

		if (m_bShowCustomGrid)
		{
			RectGrid.bottom	 = (int32)(RectGrid.bottom *(5.0/9));
			RectGrid.bottom	-= 10;
			m_RctSperator = RectGrid;
			m_RctCustom = RectGrid;
			m_RctSperator.top = RectGrid.bottom;
			m_RctSperator.bottom = m_RctSperator.top + 20;
			m_RctCustom.top = m_RctSperator.bottom;
			m_RctCustom.bottom = RectTabWnd.top;
		}
	}
	else
	{
		RectTabWnd.right = RectTabWnd.left;

		// �����������λ��
		RectHScroll.bottom = RectTabWnd.bottom;
		RectHScroll.top    = RectHScroll.bottom - iHeightScroll;
		RectHScroll.left   = RectTabWnd.right;
		RectHScroll.right  = RectClient.right;

		// ����λ��
		RectGrid.bottom	   = RectHScroll.top;

		if (m_bShowCustomGrid)
		{
			RectGrid.bottom	 = (int32)(RectGrid.bottom *(5.0/9));
			RectGrid.bottom	-= 10;
			m_RctSperator = RectGrid;
			m_RctCustom = RectGrid;
			m_RctSperator.top = RectGrid.bottom;
			m_RctSperator.bottom = m_RctSperator.top + 20;
			m_RctCustom.top = m_RctSperator.bottom;
			m_RctCustom.bottom = RectHScroll.top;
		}

		// ��ѡ�ɵײ���ť�޸�
		if (m_bShowUserBlock && m_bIsUserBlock)
		{
			CClientDC dc(this);
			CSize sizeWidth = dc.GetTextExtent(_T("�����ѡ"));	// ���ĸ��ֽڣ����������ֽ�����

			//��������
			m_RctSperator = RectClient ;
			// Ϊ�˱����������ߣ��ٴμ���һ������
			m_RctSperator.bottom += 1;
			m_RctSperator.top  = m_RctSperator.bottom - 26;
			//m_RctSperator.right = m_RctSperator.left + sizeWidth.cx*4;

			RectHScroll.top = m_RctSperator.top - iHeightScroll;
			RectHScroll.bottom = m_RctSperator.top;
			RectHScroll.left = RectClient.left;
			RectHScroll.right = RectClient.right;
			// �����ƶ��������뱨�۱�
			//RectHScroll.left = m_RctSperator.right;
			RectGrid.bottom	   = m_RctSperator.top;
			m_RctCustom.bottom = m_RctSperator.top;
		}
	}

	//
	m_GuiTabWnd.SetALingTabs(CGuiTabWnd::ALN_BOTTOM);
	m_GuiTabWnd.MoveWindow(RectTabWnd);
	m_GuiTabWnd.ShowWindow(SW_SHOW);

	if(m_bIsShowGridVertScorll)	// �Ƿ���ʾ��ֱ������
	{
		RectGrid.right -= RectHScroll.Height();

		CRect RectVScroll;
		RectVScroll.left = RectGrid.right;
		RectVScroll.right = RectVScroll.left + RectHScroll.Height();
		RectVScroll.top = RectGrid.top;
		RectVScroll.bottom = RectGrid.bottom + iHeightTabBar - RectHScroll.Height();
		m_XSBVertical.SetSBRect(RectVScroll, TRUE);

		m_RectVert = RectVScroll;	// ���洹ֱ�������ĳߴ�
	}

	m_GridCtrl.MoveWindow(RectGrid);
	m_GridCtrl.ShowWindow(SW_SHOW);
	m_RectGrid = RectGrid;

	if (m_bShowCustomGrid)
	{
		m_CustomGridCtrl.MoveWindow(m_RctCustom);
		m_CustomGridCtrl.ShowWindow(SW_SHOW);
	}


	m_XSBHorz.SetSBRect(RectHScroll, TRUE);
	m_XSBHorz.ShowWindow(SW_SHOW);



	if (m_bShowUserBlock && m_bIsUserBlock)
	{
		m_GuiTabWnd.MoveWindow(CRect(0,0,0,0));
		m_GuiTabWnd.ShowWindow(SW_HIDE);
	}

	//
	CalcMaxGridRowCount();
	AutoSizeColFromXml();
	JudgeScrollVerVisbile(true);	// �ж��Ƿ�Ҫ��ʾ��ֱ������
}
void CIoViewReport::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);

	ReCalcSize();
}

void CIoViewReport::SetTabParams(int32 iTab /*= -1*/)
{
	// ֻ���� Y ����ı仯
	if ( m_GridCtrl.GetRowCount() <= 0 )
	{
		return;
	}

	if ( iTab < 0 || iTab >= m_aTabInfomations.GetSize())
	{
		return;
	}

	CRect RectGrid;
	m_GridCtrl.GetClientRect(&RectGrid);

	int32 iVisibleRowNum;

	// �ȼ�����ʵ�ɼ�������
	if ( m_GridCtrl.GetRowCount() == 1 )
	{
		iVisibleRowNum = RectGrid.Height() / m_GridCtrl.GetDefCellHeight();
		iVisibleRowNum += 1;
	}
	else
	{
		iVisibleRowNum = (RectGrid.Height() - m_GridCtrl.GetRowHeight(0)) / m_GridCtrl.GetRowHeight(1);

		// ��������������. ��һ����¶����Ҳ��ɼ�
		iVisibleRowNum += 1;

		// �ټ��ϱ�ͷ
		iVisibleRowNum += 1;
	}

	// ����ô��������, ��ȥ��ͷ��. ���ϰ�ȫ��
	iVisibleRowNum = iVisibleRowNum - 1 + KiDataNumAddForException;

	// ��������ҳ�������ֵ
	for ( int32 i = 0; i < m_aTabInfomations.GetSize(); i++ )
	{
		if ( -1 != iTab && i != iTab )
		{			
			continue;			
		}

		T_TabInfo stTabInfo = m_aTabInfomations[i];

		int32 iDataIndexBegin = stTabInfo.m_iDataIndexBegin;
		int32 iDataIndexEnd	  = stTabInfo.m_iDataIndexBegin + iVisibleRowNum - 1;
		int32 iMerchsSize	  = 0;

		if ( T_BlockDesc::EBTPhysical == stTabInfo.m_Block.m_eType )
		{
			if ( NULL == stTabInfo.m_Block.m_pMarket ) 
			{
				continue;
			}

			iMerchsSize = stTabInfo.m_Block.m_pMarket->m_MerchsPtr.GetSize();
		}
		else if ( T_BlockDesc::EBTUser == stTabInfo.m_Block.m_eType )
		{			
			CArray<CMerch*, CMerch*> aMerchs;
			CUserBlockManager::Instance()->GetMerchsInBlock(stTabInfo.m_Block.m_StrBlockName, aMerchs);	

			iMerchsSize = aMerchs.GetSize();
		}
		else if ( T_BlockDesc::EBTLogical == stTabInfo.m_Block.m_eType )
		{
			CArray<CMerch*, CMerch*> aMerchs;
			CSysBlockManager::Instance()->GetMerchsInBlock(stTabInfo.m_Block.m_StrBlockName, aMerchs);	

			iMerchsSize = aMerchs.GetSize();
		}
		else if ( T_BlockDesc::EBTSelect == stTabInfo.m_Block.m_eType )
		{
			// ����ѡ�ɵ���Ҫ�ȴ���Ʒ�б������������Ʒ�б���m_Block��
			iMerchsSize = stTabInfo.m_Block.m_aMerchs.GetSize();
		}
		else if ( T_BlockDesc::EBTBlockLogical == stTabInfo.m_Block.m_eType)
		{
			// ������� - ������Ʒ�ڴ�ʱ���Ѿ�������desc������
			iMerchsSize = stTabInfo.m_Block.m_aMerchs.GetSize();
		}
		else if ( T_BlockDesc::EBTMerchSort == stTabInfo.m_Block.m_eType)
		{
			// ����ǿ������Ҫ�ȴ���Ʒ�б������������Ʒ�б���m_Block��
			iMerchsSize = stTabInfo.m_Block.m_aMerchs.GetSize();
		}
		else if ( T_BlockDesc::EBTRecentView == stTabInfo.m_Block.m_eType)
		{
			iMerchsSize = stTabInfo.m_Block.m_aMerchs.GetSize();
		}
		else 
		{
			continue;
		}

		if ( 0 == iMerchsSize )
		{
			iDataIndexBegin = -1;
			iDataIndexEnd	= -1;
		}
		else
		{
			bool32 bValidBegin = true;
			if ( iDataIndexBegin < 0 || iDataIndexBegin >= iMerchsSize )
			{
				ASSERT(iDataIndexBegin == -1);	// �������ô���Чֵ�����ÿ�ʼ&����
				bValidBegin = false;	
				iDataIndexBegin = 0;
				iDataIndexEnd = iDataIndexBegin + iVisibleRowNum - 1;

				if ( iDataIndexEnd >= iMerchsSize )
				{
					iDataIndexEnd = iMerchsSize - 1;
				}
			}

			if ( iDataIndexEnd >= iMerchsSize )
			{
				iDataIndexEnd = iMerchsSize - 1;

				if ( !bValidBegin || m_iCurTab != i )
				{
					// ����Ҫ��ʾ����ҳ�հ��Լ�ÿ��block��ʱ����ʾ��һҳ��Ʒ
					// ֻ���ڵ�ǰblockû����Чbegin������²����õ�ǰbegin
					// ����block������ǰ��
					iDataIndexBegin = iDataIndexEnd - iVisibleRowNum + 1;
				}
				if ( iDataIndexBegin < 0 )
				{
					iDataIndexBegin = 0;
				}
			}
		}

		UpdateLocalDataIndex(i, iDataIndexBegin, iDataIndexEnd);		
	}
}

BOOL CIoViewReport::TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( IsWindowVisible())
	{
		if ( 13 == nChar && 1 == nRepCnt)
		{
			if( 28 == nFlags || 284 == nFlags)				
			{
				if (IsInCustomGrid())
				{
					m_bCustomGrid = TRUE;
					CCellRange cellRange = m_CustomGridCtrl.GetSelectedCellRange();	

					int32 iRow = cellRange.GetMaxRow();	
					int32 iCol = 1;

					CGridCellBase* pBaseCell = m_CustomGridCtrl.GetCell(0,iCol);
					if ( NULL == pBaseCell )
					{
						return FALSE;
					}

					CString StrHead = pBaseCell->GetText();
					//if ( 0 != StrHead.CompareNoCase(_T("����")) ) // ?
					if ( iRow >= m_CustomGridCtrl.GetFixedRowCount() && iRow < m_CustomGridCtrl.GetRowCount() )
					{
						OnDblClick();
					}
				}
				else
				{
					m_bCustomGrid = FALSE;
					CCellRange cellRange = m_GridCtrl.GetSelectedCellRange();	

					int32 iRow = cellRange.GetMaxRow();	
					int32 iCol = 1;

					int32 iTab;
					T_BlockDesc::E_BlockType eType;

					if ( !TabIsValid(iTab, eType))
					{
						return FALSE;
					}

					CGridCellBase* pBaseCell = m_GridCtrl.GetCell(0,iCol);
					if ( NULL == pBaseCell )
					{
						return FALSE;
					}

					CString StrHead = pBaseCell->GetText();
					//if ( 0 != StrHead.CompareNoCase(_T("����")) ) // ?
					if ( iRow >= m_GridCtrl.GetFixedRowCount() && iRow < m_GridCtrl.GetRowCount() )
					{	
						OnDblClick();
					}
				}
			}			
		}

		return FALSE;
	}

	return FALSE;
}

void CIoViewReport::OnIoViewActive()
{
	CIoViewBase::OnIoViewActive();

	m_bActive = IsActiveInFrame();

	//
	//OnVDataForceUpdate();
	RequestLastIgnoredReqData();

	SetChildFrameTitle();

	if (NULL != m_GridCtrl.GetSafeHwnd())
	{
		m_GridCtrl.SetFocus();
	}

	//
	int32 iTab;
	T_BlockDesc::E_BlockType eType;

	if ( !TabIsValid(iTab, eType) )
	{
		return;
	}

	//
	//CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();

	// ���μ��̾������ѡ�ɵĹ���
	// 	if ( T_BlockDesc::EBTUser == eType )
	// 	{
	// 		pMainFrm->SetHotkeyTarget(this);
	// 	}
	// 	else
	// 	{
	// 		pMainFrm->SetHotkeyTarget(NULL);
	// 	}

	//
	if ( m_GridCtrl.GetRowCount() >= 1 && m_GridCtrl.GetColumnCount() >= 1 )
	{
		CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, 0);
		if ( NULL != pCell )
		{
			pCell->SetParentActiveFlag(true);
			m_GridCtrl.Invalidate();			
		}
	}	
}

void CIoViewReport::OnIoViewDeactive()
{
	CIoViewBase::OnIoViewDeactive();
	m_bActive = false;

	//	���μ��̾������ѡ�ɹ���
	// 	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	// 	pMainFrm->SetHotkeyTarget(NULL);	

	if ( m_GridCtrl.GetRowCount() >= 1 && m_GridCtrl.GetColumnCount() >= 1 )
	{
		CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, 0);
		if ( NULL != pCell )
		{
			pCell->SetParentActiveFlag(false);
		}
	}

	//m_GridCtrl.ResetSelectedRange();
	m_GridCtrl.Invalidate();
}

void CIoViewReport::OnSelChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if ( !m_bLockSelChange )
	{
		m_iCurTab = m_GuiTabWnd.GetCurtab();
		if ( m_iCurTab >= m_aTabInfomations.GetSize() )
		{						
			*pResult = 0;
			return;
		}

		// ���¹�������λ��:
		//int32 iVScrollPos = m_GridCtrl.GetScrollPos32(SB_VERT);
		//int32 iHScrollPos = m_GridCtrl.GetScrollPos32(SB_HORZ);

		ASSERT(m_iCurTab < m_aTabInfomations.GetSize());
		//UpdateLocalXScrollPos(m_iCurTab, iHScrollPos);  // ��ʱm_iCurTab�Ѿ����л����µ�tab��

		m_iPreTab = m_iCurTab;
		TabChange();
	}

	*pResult = 0;
}



bool32 CIoViewReport::FromXmlTopBtn( TiXmlElement *pTiXmlElement )
{
	if (NULL == pTiXmlElement)
		return false;

	// ��ǰѡ������ѡ��Ĳ���

	m_mapTopTabbarGroup.clear();
	TiXmlElement *pMyEle = pTiXmlElement->FirstChildElement("TopTabbar");
	if ( NULL != pMyEle )
	{
		int iID = 0;
		for(TiXmlNode *pNode = pMyEle->FirstChild(); pNode; pNode = pNode->NextSibling())
		{
			if (!pNode)
			{
				return false;
			}

			const char *pcValue = pNode->Value();
			if (NULL == pcValue || strcmp("tabbar", pcValue) != 0)
			{
				continue;
			}

			T_TopTabbar stTopTabbar;

			TiXmlElement* pElement = (TiXmlElement*)pNode;
			const char *pcAttrValue = pElement->Attribute("name");
			if (pcAttrValue != NULL)
			{
				stTopTabbar.m_StrShowName  = _A2W(pcAttrValue);
			}

			pcAttrValue = pElement->Attribute("btnType");
			if (pcAttrValue != NULL)
			{
				stTopTabbar.m_iBtnType  = atoi(pcAttrValue);
			}

			pcAttrValue = pElement->Attribute("marketid");
			if (pcAttrValue != NULL)
			{
				stTopTabbar.m_iMarketID  = atoi(pcAttrValue);
			}

			pcAttrValue = pElement->Attribute("cfmname");
			if (pcAttrValue != NULL)
			{
				stTopTabbar.m_strCfmName  = _A2W(pcAttrValue);
			}

			// ��ͷ����
			pcAttrValue = pElement->Attribute("reportHeadType");
			if (pcAttrValue != NULL)
			{
				stTopTabbar.m_eReportHeadType = (E_ReportHeadType)atoi(pcAttrValue);
			}

			// �Ƿ�ѡ��
			pcAttrValue = pElement->Attribute("isSelected");
			if (pcAttrValue != NULL)
			{
				stTopTabbar.m_bSelected = atoi(pcAttrValue) == 1;
			}
			
			// �л����۱��ͷ
			if (stTopTabbar.m_bSelected)
			{
				SetReportHeadType(stTopTabbar.m_eReportHeadType);
			}

			pcAttrValue = pElement->Attribute("Blockid");
			if (pcAttrValue != NULL)
			{
				stTopTabbar.m_iBlockid  = atoi(pcAttrValue);
			}

			pcAttrValue = pElement->Attribute("BlockType");
			if (pcAttrValue != NULL)
			{
				stTopTabbar.m_iBlockType  = atoi(pcAttrValue);
			}


			//	ָ��ؼ���ʾ����ɫ
			const char* pIndexNomarlBkColor	= pElement->Attribute(KIoViewChartIndexNomarlBkColor);
			if (NULL != pIndexNomarlBkColor)
			{						
				stTopTabbar.m_colNoarml = StrToColorRef(pIndexNomarlBkColor);					
			}

			const char* pIndexPressBkColor	= pElement->Attribute(KIoViewChartIndexPressBkColor);
			if (NULL != pIndexPressBkColor)
			{						
				stTopTabbar.m_colPress = StrToColorRef(pIndexPressBkColor);					
			}

			const char* pIndexHotBkColor	= pElement->Attribute(KIoViewChartIndexHotBkColor);
			if (NULL != pIndexHotBkColor)
			{						
				stTopTabbar.m_colHot = StrToColorRef(pIndexHotBkColor);					
			}

			//	ָ��ؼ��ı�ɫ
			const char* pIndexNomarTextColor	= pElement->Attribute(KIoViewChartIndexNomarlTextColor);
			if (NULL != pIndexNomarTextColor)
			{						
				stTopTabbar.m_colTextNoarml = StrToColorRef(pIndexNomarTextColor);					
			}

			const char* pIndexPressTextColor	= pElement->Attribute(KIoViewChartIndexPressTextColor);
			if (NULL != pIndexPressTextColor)
			{						
				stTopTabbar.m_colTextPress = StrToColorRef(pIndexPressTextColor);					
			}

			const char* pIndexHotTextColor	= pElement->Attribute(KIoViewChartIndexHotTextColor);
			if (NULL != pIndexHotTextColor)
			{						
				stTopTabbar.m_colTextHot = StrToColorRef(pIndexHotTextColor);					
			}

			stTopTabbar.m_iBtnID = TOPTABBAR_BEGINID + iID++;
			m_mapTopTabbarGroup[stTopTabbar.m_iBtnID] = stTopTabbar;
		}
	}

	return true;
}


void CIoViewReport::InitTopBtn()
{
	m_mapTopTabbarBtn.clear();
	map<int32, T_TopTabbar>::iterator iter;
	for (iter = m_mapTopTabbarGroup.begin(); iter != m_mapTopTabbarGroup.end(); iter++)
	{
		T_TopTabbar &topTabbar = iter->second;
		CNCButton btnControl;
		CRect rcRect(0,0,0,0);
		btnControl.CreateButton(topTabbar.m_StrShowName, rcRect, this, NULL, 1, topTabbar.m_iBtnID);
		btnControl.SetTextColor(topTabbar.m_colTextNoarml, topTabbar.m_colTextHot, topTabbar.m_colTextPress);
		btnControl.SetTextBkgColor(topTabbar.m_colNoarml, topTabbar.m_colHot, topTabbar.m_colPress);
		btnControl.SetTextFrameColor(RGB(0,0,0), topTabbar.m_colHot, topTabbar.m_colPress);

		CNCButton::T_NcFont m_Font;
		m_Font.m_StrName = gFontFactory.GetExistFontName(L"����");
		m_Font.m_Size	 = 9;
		m_Font.m_iStyle	 = FontStyleRegular;	
		btnControl.SetFont(m_Font);
		btnControl.SetBtnBkgColor(GetIoViewColor(ESCBackground));
		
		//
		if (topTabbar.m_bSelected)
		{
			btnControl.SetCheck(TRUE);
		}
		
		CRect rtInset(1,1,1,1);
		btnControl.SetAreaOffset(&rtInset);

		m_mapTopTabbarBtn[topTabbar.m_iBtnID] = btnControl;
	}
}


//////////////////////////////////////////////////////////////////////////////////////////
//��WorkSpace��xml�ж���Blocks,����ȷ��Blocks��������������. Logic xml. user xml�д���

bool32 CIoViewReport::FromXml(TiXmlElement * pTiXmlElement)
{
	if (NULL == pTiXmlElement)
		return false;

	// �ж��ǲ���IoView�Ľڵ�
	const char *pcValue = pTiXmlElement->Value();
	if (NULL == pcValue || strcmp(GetXmlElementValue(), pcValue) != 0)
		return false;

	// �ж��ǲ��������Լ����ҵ����ͼ�Ľڵ�
	const char *pcAttrValue = pTiXmlElement->Attribute(GetXmlElementAttrIoViewType());
	CString StrIoViewString = _A2W(pcAttrValue);
	if (NULL == pcAttrValue || CIoViewManager::GetIoViewString(this).Compare(StrIoViewString) != 0)	// ���������Լ���ҵ��ڵ�
	{
		return false;
	}

	// �Ƿ���ʾ��ֱ������
	pcAttrValue = pTiXmlElement->Attribute(KStrViewReportShowGridVertScrool);
	if (pcAttrValue != NULL)
	{
		m_bIsShowGridVertScorll = atoi(pcAttrValue);
	}

	// �Ƿ���ʾtab��ǩ
	pcAttrValue = pTiXmlElement->Attribute(KStrViewReportShowTab);
	if (pcAttrValue != NULL)
	{
		m_bShowTab = atoi(pcAttrValue);
	}

	// �Ƿ����Tabbar��ʾtab��ǩ
	pcAttrValue = pTiXmlElement->Attribute(KStrViewReportShowTabByTabbar);
	if (pcAttrValue != NULL)
	{
		// ֻ��m_bShowTab=false, m_bShowTabByTabbar����Ч
		if (m_bShowTab)
		{
			m_bShowTabByTabbar = false;
		} 
		else
		{
			m_bShowTabByTabbar = atoi(pcAttrValue);
		}
		
	}

	// Ĭ����ʾ���г�
	pcAttrValue = pTiXmlElement->Attribute(GetXmlElementAttrMarketId());
	if (pcAttrValue != NULL)
	{
		m_iMarketID = atoi(pcAttrValue);
	}

	// �Ƿ�����
	pcAttrValue = pTiXmlElement->Attribute(KStrViewReportLock);
	if ((pcAttrValue!= NULL) && (strlen(pcAttrValue) > 0))
	{
		m_bLockReport = atoi(pcAttrValue);
	}
	
	// �Ƿ������
	pcAttrValue = pTiXmlElement->Attribute(KStrViewReportShowMoreColumn);
	if ((pcAttrValue!= NULL) && (strlen(pcAttrValue) > 0))
	{
		m_bShowMoreColumn = atoi(pcAttrValue);
	}

	// �Ƿ���ʾ������
	pcAttrValue = pTiXmlElement->Attribute(KStrViewReportShowGridLine);

	if ( NULL != pcAttrValue )
	{
		m_bShowGridLine = atoi(pcAttrValue);

		if ( m_bShowGridLine )
		{
			m_GridCtrl.ShowGridLine(true);
			m_CustomGridCtrl.ShowGridLine(true);
		}
		else
		{
			m_GridCtrl.ShowGridLine(false);
			m_CustomGridCtrl.ShowGridLine(false);
		}
	}

	// �Ƿ���ʾ�û��Զ�����
	pcAttrValue = pTiXmlElement->Attribute(KStrViewReportShowCustomGrid);

	if ( NULL != pcAttrValue )
	{
		m_bShowCustomGrid = atoi(pcAttrValue);
		if(!m_bShowCustomGrid)
		{
			m_CustomGridCtrl.ShowWindow(SW_HIDE);
		}
	}

	// �Ƿ���ʾģʽΪ�û���ѡ��ģʽ
	pcAttrValue = pTiXmlElement->Attribute(KStrViewReportShowUserBlock);

	if ( NULL != pcAttrValue )
	{
		m_bShowUserBlock = atoi(pcAttrValue);
	}

	if(m_bShowUserBlock)
	{
		CreateBtnStock();
	}

	// �Ƿ�����Ӧ�п�
	pcAttrValue = pTiXmlElement->Attribute(KStrXMLReportAutoSizeCol);

	if ( NULL != pcAttrValue )
	{
		m_bAutoSizeColFromXml = atoi(pcAttrValue);	
	}

	// �ж��ǲ���ESC ���۱�
	pcAttrValue = pTiXmlElement->Attribute(KStrViewReportBeEscReport);

	if ( NULL != pcAttrValue )
	{
		bool32 bEscReport = atoi(pcAttrValue);

		if ( bEscReport )
		{
			CString StrEscBlock = L"";

			// �ǵ�,��ȡ�������:
			pcAttrValue = pTiXmlElement->Attribute(KStrViewReportEscBlock);

			if ( NULL != pcAttrValue )
			{
				TCHAR TStrEscBlock[200];
				memset(TStrEscBlock, 0, sizeof(TStrEscBlock));

				MultiCharCoding2Unicode(EMCCUtf8, pcAttrValue, strlen(pcAttrValue), TStrEscBlock, sizeof(TStrEscBlock) / sizeof(TCHAR));

				StrEscBlock = TStrEscBlock;
			}
		}
	}

	SetFontsFromXml(pTiXmlElement);
	SetColorsFromXml(pTiXmlElement);
	// ��������
	LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);
	SetRowHeightAccordingFont();

	FromXmlExtra(pTiXmlElement);


	pcAttrValue = pTiXmlElement->Attribute(KStrViewReportIsReadCurTab);
	if (NULL == pcAttrValue || pcAttrValue[0] == '\0' )
	{
		m_iIsReadCurTab = -1;
	}
	else
	{
		// �������ǰ���߼��н��档
		// ��ֵ�Ļ����Ͷ�ȡcurtab���ԡ�
		m_iIsReadCurTab = atol ( pcAttrValue);
		if (1 == m_iIsReadCurTab)
		{
			m_iCurTab = -1;
			pcAttrValue = pTiXmlElement->Attribute(KStrViewReportTabName);
			if (NULL == pcAttrValue || pcAttrValue[0] == '\0' )
			{
				m_iCurTab = -1;
			}
			else
			{
				m_iCurTab = atol ( pcAttrValue);
			}
		}
	}

	pcAttrValue = pTiXmlElement->Attribute(KStrViewReportShowTopTabbar);
	if (NULL != pcAttrValue )
	{
		m_bIsShowTopTabbar = atol ( pcAttrValue);
		if (m_bIsShowTopTabbar)
		{
			FromXmlTopBtn(pTiXmlElement);
			InitTopBtn();
		}
	}
	
	//if ((0 == strcmp(pTiXmlElement->Value(), KIoViewChartToolBarIndex)))
	//{
	//	TiXmlNode *pChild = pElement->FirstChild();
	//	if (pChild != NULL)
	//	{
	//		// ָ��
	//		int32 iXSkip = 0 ;
	//		int32 iWidth = 80;			
	//		int iPos = 0;			

	//		m_mapBottomBtn.clear();
	//		for(; pChild; pChild = pChild->NextSibling())
	//		{
	//			bool32 bBlod = true;
	//			int32 iOpenCfm = 0;	// �Ƿ��Ǵ򿪰���
	//			std::wstring strCharacter(L"Arial");
	//			iWidth = 80;	//	
	//			float fSize = 9.0f;
	//			COLORREF colNoarml = RGB(86,82,80), colHot = RGB(41,36,33), colPress = RGB(41,36,33);
	//			COLORREF colTextNoarml = RGB(179,178,178), colTextHot = RGB(255,255,255), colTextPress = RGB(255, 255, 255);

	//			TiXmlElement* pElementChild = (TiXmlElement*)pChild;	
	//			const char* strIndexName = pElementChild->Attribute(KIoViewChartIndexName);
	//			if ( NULL == strIndexName )
	//			{
	//				continue;
	//			}

	//		}
	//	}
	//}


	/*
	// �ҵ�type����
	m_iCurTab = -1;
	pcAttrValue = pTiXmlElement->Attribute(KStrViewReportTabName);
	if (NULL == pcAttrValue || pcAttrValue[0] == '\0' )
	{
	m_iCurTab = -1;
	}
	else
	{
	m_iCurTab = atol ( pcAttrValue);
	}

	//
	m_eMarketReportType	= ERTNone;

	//
	CString StrBlock;


	// ������ѡ�ɱ������FromXml֮ǰAddһ��ѡ��Tab����ѡ��tab�����Ժ󱣴��xml�����ﴦ��һ�£�ѡ���Ժ�Ҫ����������ѡ�ɵ�����
	for(TiXmlNode *pNode = pTiXmlElement->FirstChild(); pNode; pNode = pNode->NextSibling())
	{
	if ( pNode->Type() == TiXmlNode::ELEMENT )
	{
	T_BlockDesc BlockDesc;			
	bool32 bValidBlock = true;

	//
	TiXmlElement* pElement = (TiXmlElement*)pNode;
	if ( NULL == pElement )
	{
	continue;
	}

	//
	const char* strType = pElement->Attribute(KStrViewReportTypeName);			
	if ( NULL == strType )
	{
	continue;
	}

	//
	const char* strBlock = pElement->Attribute(KStrViewReportValueName);
	if ( NULL == strBlock )
	{
	continue;
	}

	//
	if ( 0 == strcmp(strType, KStrViewReportTypeUserName))
	{
	StrBlock = _A2W(strBlock);

	if ( NULL != CUserBlockManager::Instance()->GetBlock(StrBlock) )
	{					
	BlockDesc.m_eType	= T_BlockDesc::EBTUser;
	BlockDesc.m_iMarketId = 0;
	BlockDesc.m_StrBlockName = StrBlock;			

	if ( !CUserBlockManager::Instance()->GetMerchsInBlock(StrBlock, BlockDesc.m_aMerchs) )
	{
	//ASSERT(0);
	bValidBlock = false;
	}
	}
	else
	{
	//ASSERT(0);
	bValidBlock = false;
	}
	}			
	else if ( 0 == strcmp(strType, KStrViewReportTypeSysName))
	{				 
	BlockDesc.m_eType = T_BlockDesc::EBTPhysical;
	BlockDesc.m_iMarketId = atol(strBlock);

	ASSERT(NULL != m_pAbsCenterManager);

	if ( !m_pAbsCenterManager->GetMerchManager().FindMarket(BlockDesc.m_iMarketId, BlockDesc.m_pMarket) )
	{
	//ASSERT(0);		
	bValidBlock = false;
	}	
	else
	{
	BlockDesc.m_StrBlockName = BlockDesc.m_pMarket->m_MarketInfo.m_StrCnName;
	}
	}			
	else if ( 0 == strcmp(strType, KStrViewReportTypeLogicName))
	{
	StrBlock = _A2W(strBlock);

	if ( NULL != CSysBlockManager::Instance()->GetBlock(StrBlock) )
	{					
	BlockDesc.m_eType = T_BlockDesc::EBTLogical;
	BlockDesc.m_iMarketId = 0;
	BlockDesc.m_StrBlockName = StrBlock;

	if ( !CSysBlockManager::Instance()->GetMerchsInBlock(StrBlock, BlockDesc.m_aMerchs) )
	{
	//ASSERT(0);
	bValidBlock = false;
	}
	}
	else
	{
	//ASSERT(0);
	bValidBlock = false;
	}
	}
	else if ( 0 == strcmp(strType, KStrViewReportTypeSelectName))
	{
	StrBlock = _A2W(strBlock);

	BlockDesc.m_eType = T_BlockDesc::EBTSelect;
	BlockDesc.m_iMarketId = 0;
	BlockDesc.m_StrBlockName = StrBlock;
	ASSERT( IsKindOf(RUNTIME_CLASS(CIoViewReportSelect)) );

	bValidBlock = false;		// ע�⣺�Ѿ���OnCreate����ˣ�����Ͳ������
	}
	else if ( 0 == strcmp(strType, KStrViewReportTypeBlockLogicName))
	{

	BlockDesc.m_eType = T_BlockDesc::EBTBlockLogical;
	BlockDesc.m_iMarketId = atol(strBlock);

	// �Ӱ������г�ʼ�� - ���ڰ������ʱ�Ĳ�ȷ���ԣ���ʱ��ʼ��ð����
	CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(BlockDesc.m_iMarketId);
	if ( NULL != pBlock )
	{
	BlockDesc.m_StrBlockName = pBlock->m_blockInfo.m_StrBlockName;
	BlockDesc.m_aMerchs.Append(pBlock->m_blockInfo.m_aSubMerchs);
	bValidBlock = true;
	}
	else
	{
	bValidBlock = false;	
	}
	}
	else
	{
	//ASSERT(0);
	bValidBlock = false;
	}

	//
	if ( bValidBlock )
	{
	T_TabInfo stTabInfo;
	stTabInfo.m_Block = BlockDesc;

	m_aTabInfomations.Add(stTabInfo);
	}			
	}
	}

	//
	RequestViewDataCurrentVisibleRowAsync();
	RequestViewDataSortAsync();

	ReCreateTabWnd();
	m_iPreTab = m_iCurTab;
	*/

	//Ŀǰ���۱�TAB�Ĵ�ͨ�����ƿ��л�ȡ
	// 	pcAttrValue = pTiXmlElement->Attribute(KStrViewReportSimpleTabClassId);
	// 	if ( NULL != pcAttrValue )
	// 	{
	// 		int32 iId = atol(pcAttrValue);
	// 		if ( iId != m_SimpleTabInfoClass.iId )
	// 		{
	// 			// ���³�ʼ����tabinfo��Ϣ
	//			m_SimpleTabInfoClass.iId = iId;
	//			InitialSimpleTabInfo();
	// 			InitialTabInfos();
	//			ReCreateTabWnd();	// �ȵ���ɳ�ʼ��ʱ��setTab
	// 		}
	// 	}

	/*	if (m_bShowCustomGrid)
	{
	ReCreateTabWnd();
	}

	m_GuiTabWnd.SetBkGround(false, GetIoViewColor(ESCBackground), 0, 0);

	// ����-1��ʾtab��һ���г�
	if(-1 != m_iMarketID)
	{
	if(KiIDTabUserOwn == m_iMarketID)	// -2 ��ʾ��ѡ
	{
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	pMain->OnHotKeyZXG();
	return true;
	}

	CAbsCenterManager* pAbsCenterManager = AfxGetDocument()->m_pAbsCenterManager;
	if (NULL == pAbsCenterManager)
	{
	return true;
	}

	// �ǲ��������г�
	CMarket* pMarket = NULL;
	if (pAbsCenterManager->GetMerchManager().FindMarket(m_iMarketID, pMarket) && NULL != pMarket)
	{
	T_BlockDesc BlockDesc;
	BlockDesc.m_eType			= T_BlockDesc::EBTPhysical;
	BlockDesc.m_iMarketId		= pMarket->m_MarketInfo.m_iMarketId;
	BlockDesc.m_StrBlockName	= pMarket->m_MarketInfo.m_StrCnName;
	BlockDesc.m_pMarket			= pMarket;		

	OpenBlock(BlockDesc);
	}

	// �ǲ����߼����
	CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(m_iMarketID);
	if ( NULL != pBlock )
	{
	T_BlockDesc desc;
	desc.m_StrBlockName = pBlock->m_blockInfo.m_StrBlockName;
	desc.m_eType		= T_BlockDesc::EBTBlockLogical;
	desc.m_iMarketId	= m_iMarketID;
	desc.m_aMerchs.Copy(pBlock->m_blockInfo.m_aSubMerchs);
	OpenBlock(desc);
	}
	}
	*/
	pcAttrValue = pTiXmlElement->Attribute(KStrViewReportSimpleTabClassId);
	if ( NULL != pcAttrValue )
	{
		int32 iId = atol(pcAttrValue);
		if ( iId != m_SimpleTabInfoClass.iId )
		{
			// ���³�ʼ����tabinfo��Ϣ
			m_SimpleTabInfoClass.iId = iId;
			InitialSimpleTabInfo();
			InitialTabInfos();
			ReCreateTabWnd();	// �ȵ���ɳ�ʼ��ʱ��setTab
		}
	}

	m_GuiTabWnd.SetBkGround(false, GetIoViewColor(ESCBackground), 0, 0);
	return true;
}

void CIoViewReport::SaveColoumWidth()
{
	if ( !m_GridCtrl.GetSafeHwnd())
	{
		return;
	}

	if ( m_GridCtrl.GetColumnCount() < 1 || m_GridCtrl.GetRowCount() < 1)
	{
		return;
	}

	for ( int32 i = 0 ; i < m_GridCtrl.GetColumnCount(); i ++)
	{		
		int32 iWidth = m_GridCtrl.GetColumnWidth(i);
		if ( iWidth > 0 )
		{
			if ( BeSpecial() )
			{
				CReportScheme::Instance()->SaveColumnWidth(ERTFutureSpot, i, iWidth);
			}	
			else
			{
				CReportScheme::Instance()->SaveColumnWidth(m_eMarketReportType, i, iWidth);
			}			
		}		
	}
}

void CIoViewReport::SetTab(int32 iCurTab)
{
	if ( iCurTab < 0 || iCurTab >= m_aTabInfomations.GetSize() )
	{
		return;
	}

	//
	if ( iCurTab <= (KiFixTabCounts - 1) || IsKindOfReportSelect())
	{
		m_GuiTabWnd.SetCurtab(iCurTab);	
	}
	else
	{
		m_GuiTabWnd.SetCurtab(KiFixTabCounts - 1);
	}
}

void CIoViewReport::SetEditAbleCol()
{
	CArray<int32,int32> aCols;
	aCols.RemoveAll();

	int32 iTab;
	T_BlockDesc::E_BlockType eType;

	if ( !TabIsValid(iTab, eType) )
	{
		return;
	}

	// ������༭
	m_GridCtrl.SetColsEditableFlag(false);
	m_GridCtrl.SetEditableCols(aCols);

	return;
}

CString CIoViewReport::ToXml()
{	
	// ��Ҫ������Ϣ��ʱ��,�����ͷ�Ŀ��:	
	// ��OnGridColWidthChanged���Ѿ������ˣ����ﲻ�ٱ����п�
	// SaveColoumWidth();

	//
	CString StrThis;

	bool32	bEscReport = false;
	CString StrEscBlockName = L"";

	if ( NULL != m_pAbsCenterManager )
	{
		// ���ǲ�ִ�У���ע�͵��������ٸ���
		//if ( 0 )
		//{
		//	bEscReport = true;
		//}

		if ( !bEscReport )
		{
			StrEscBlockName = L"";

		}
	}

	//
	if (m_bShowTabByTabbar)
	{
		m_bShowTab = false;
	}

	StrThis.Format( L"<%s %s=\"%s\" %s=\"%d\" %s=\"%d\"  %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%s\" %s=\"%s\" %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%d\" ", 
		CString(GetXmlElementValue()).GetBuffer(),
		CString(GetXmlElementAttrIoViewType()).GetBuffer(),	CIoViewManager::GetIoViewString(this).GetBuffer(),
		CString(KStrViewReportTabName).GetBuffer(), m_iCurTab,
		CString(KStrViewReportIsReadCurTab).GetBuffer(), m_iIsReadCurTab,
		CString(KStrViewReportShowGridVertScrool).GetBuffer(), m_bIsShowGridVertScorll,
		CString(KStrViewReportShowTab).GetBuffer(), m_bShowTab,
		CString(KStrViewReportShowGridLine).GetBuffer(), m_bShowGridLine,
		CString(KStrViewReportShowCustomGrid).GetBuffer(),m_bShowCustomGrid,
		CString(KStrViewReportShowUserBlock),m_bShowUserBlock,
		CString(KStrViewReportBeEscReport).GetBuffer(), bEscReport,
		CString(KStrViewReportEscBlock).GetBuffer(), StrEscBlockName.GetBuffer(),
		CString(GetXmlElementAttrShowTabName()).GetBuffer(), m_StrTabShowName.GetBuffer(),
		CString(KStrViewReportLock).GetBuffer(), m_bLockReport,
		CString(GetXmlElementAttrMarketId()).GetBuffer(), m_iMarketID,
		CString(KStrViewReportSimpleTabClassId), m_SimpleTabInfoClass.iId
		);

	CString StrFace;
	StrFace  = SaveColorsToXml();
	StrFace += SaveFontsToXml();

	StrThis += StrFace;
	StrThis += L">\n";

	StrThis += ToXmlExtra();

	int32 iTab;
	T_BlockDesc::E_BlockType eType;
	if ( TabIsValid(iTab, eType) )
	{
		int32 i,iSize = m_aTabInfomations.GetSize();

		for ( i = 0; i < iSize; i ++ )
		{
			CString StrType;

			T_BlockDesc BlockDesc = m_aTabInfomations.GetAt(i).m_Block;
			CString StrBlock	  = BlockDesc.m_StrBlockName;
			int32 iMarketId		  = BlockDesc.m_iMarketId;

			int32 iType = (int32)BlockDesc.m_eType;

			if ( 0 == iType )
			{
				StrType = CString(KStrViewReportTypeSysName);
				StrBlock.Format(L"%d",iMarketId);
			}

			if ( 1 == iType )
			{
				StrType = CString(KStrViewReportTypeUserName);
			}

			if ( 2 == iType )
			{
				StrType = CString(KStrViewReportTypeLogicName);	
			}

			if ( T_BlockDesc::EBTSelect == iType )
			{
				StrType = CString(KStrViewReportTypeSelectName);
			}

			if ( T_BlockDesc::EBTRecentView == iType)
			{
				StrType = CString(KStrViewReportTypeRecentViewName);
			}

			if ( T_BlockDesc::EBTMerchSort == iType)
			{
				StrType = CString(KStrViewReportTypeMerchSortName);
			}	

			if ( T_BlockDesc::EBTBlockLogical == iType )
			{
				StrType = CString(KStrViewReportTypeBlockLogicName);
				StrBlock.Format(_T("%d"), iMarketId);
			}

			CString StrSeg;
			StrSeg.Format(L"<BLOCK %s=\"%s\" %s=\"%s\"/>\n",
				CString(KStrViewReportValueName).GetBuffer(),
				StrBlock.GetBuffer(),
				CString(KStrViewReportTypeName).GetBuffer(),
				StrType.GetBuffer());

			StrThis += StrSeg;
		}
	}

	StrThis += L"</";
	StrThis += CString(GetXmlElementValue());
	StrThis += L">\n";
	return StrThis;
}

CString CIoViewReport::GetDefaultXML()
{
	CString StrThis;
	// 
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" >\n", 
		CString(GetXmlElementValue()).GetBuffer(), 
		CString(GetXmlElementAttrIoViewType()).GetBuffer(), 
		CIoViewManager::GetIoViewString(this).GetBuffer(),
		CString(GetXmlElementAttrMerchCode()).GetBuffer(),
		L"",
		CString(GetXmlElementAttrMarketId()).GetBuffer(), 
		L"-1");

	//
	StrThis += L"</";
	StrThis += CString(GetXmlElementValue());
	StrThis += L">\n";

	return StrThis;
}

void CIoViewReport::OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult)
{ 
	if (IsInCustomGrid())
	{
		m_bCustomGrid = TRUE;
	}
	else
	{
		m_bCustomGrid = FALSE;
	}
	CPoint pt;
	GetCursorPos(&pt);

	CNewMenu menu;	
	menu.LoadMenu(IDR_MENU_CHG);

	CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));

	pPopMenu->RemoveMenu(_T("�Զ���1"));
	pPopMenu->RemoveMenu(_T("�Զ���2"));
	pPopMenu->RemoveMenu(_T("�Զ���3"));
	pPopMenu->RemoveMenu(_T("�Զ���4"));

	//
	pPopMenu->LoadToolBar(g_awToolBarIconIDs);
	pPopMenu->AppendODMenu(L"����", MF_STRING, IDM_IOVIEWBASE_RETURN);
	pPopMenu->AppendODMenu(L"", MF_SEPARATOR);

	pPopMenu->AppendODMenu(_T("������Excel"),MF_STRING,IDM_USER1);
	pPopMenu->AppendODMenu(_T("ͬ����Excel"),MF_STRING,IDM_USER2);
	
	pPopMenu->AppendODMenu(_T("��������Ԥ��"), MF_STRING, IDM_SETUPALARM);
	pPopMenu->AppendODMenu(L"", MF_SEPARATOR);

	// ������ѡ��/ɾ����ѡ��
	//	pPopMenu->AppendODMenu(_T("������� CTRL+R"), MF_STRING, IDM_BELONG_BLOCK);

	// �عɳֲֻ���
	//	pPopMenu->AppendODMenu(_T("�عɳֲֻ��� 36"), MF_STRING, ID_ZHONGCANGCHIGU);

	int32 iTab;
	T_BlockDesc::E_BlockType eType;

	if ( TabIsValid(iTab, eType) )
	{
		//
		if (T_BlockDesc::EBTUser == eType )
		{
			// ��ǰ����ѡ�ɰ��,ɾ����ѡ��
			// ����ѡ���

			NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
			ASSERT(NULL != pItem);

			m_CellRButtonDown.row = pItem->iRow;
			m_CellRButtonDown.col = pItem->iColumn;

			pPopMenu->AppendODMenu(L"ɾ����ѡ DEL", MF_STRING, IDM_IOVIEWREPORT_DELOWNSEL);

			pPopMenu->AppendODMenu(L"����(SHIFT+��)", MF_STRING, IDM_IOVIEWREPORT_OWNSEL_UP);
			pPopMenu->AppendODMenu(L"����(SHIFT+��)", MF_STRING, IDM_IOVIEWREPORT_OWNSEL_DOWN);

			// ��ȡ �ϴ���ѡ��
			//const CString StrBlockName(m_aTabInfomations[iTab].m_Block.m_StrBlockName);
			//if ( StrBlockName == CUserBlockManager::Instance()->GetServerBlockName() )
			//{
			//			pPopMenu->AppendODMenu(L"", MF_SEPARATOR);
			//			CString StrName;
			//			StrName.Format(_T("����[%s]"), StrBlockName);
			//			pPopMenu->AppendODMenu(StrName, MF_STRING, IDM_IOVIEWREPORT_OWN_DOWNLOAD);
			//			StrName.Format(_T("�ϴ�[%s]"), StrBlockName);
			//			pPopMenu->AppendODMenu(StrName, MF_STRING, IDM_IOVIEWREPORT_OWN_UPLOAD);
			//}
		}
		else
		{
			//---wangyongxue 2017.05.08 ������ѡ�ɰ��,������ѡ��;����ѡ����ʾ��ɾ����ѡ�ɡ�
			CString StrDefaultBlock = CUserBlockManager::Instance()->GetServerBlockName();
			if ( !StrDefaultBlock.IsEmpty() )
			{
				CMerch *pMerch = GetGridSelMerch();

				if (!pMerch)
				{
					return;
				}

				bool32 bZixuan= CUserBlockManager::Instance()->BeMerchInBlock(pMerch, StrDefaultBlock);

				if (bZixuan)
				{
					pPopMenu->AppendODMenu(_T("ɾ����ѡ��"), MF_STRING, IDM_IOVIEWREPORT_DELOWNSEL);
				}
				else
				{
					pPopMenu->AppendODMenu(_T("���뵽��ѡ ALT+Z"), MF_STRING, ID_BLOCK_ADDTOFIRST);
				}
			}
			pPopMenu->AppendODMenu(_T("������ѡ��� CTRL+Z"), MF_STRING, ID_BLOCK_ADDTO);
			pPopMenu->AppendODMenu(_T("�Ӱ��ɾ�� ALT +D"), MF_STRING, ID_BLOCK_REMOVEFROM);

			// ��ǰ�ť
			pPopMenu->AppendMenu(MF_SEPARATOR);
			CNewMenu *pMarkPopMenu = pPopMenu->AppendODPopupMenu(_T("��ǵ�ǰ��Ʒ"));
			ASSERT( NULL != pMarkPopMenu );
			pMarkPopMenu->AppendMenu(MF_STRING, ID_MARK_CANCEL, _T("ȡ�����"));
			pMarkPopMenu->AppendMenu(MF_SEPARATOR);
			pMarkPopMenu->AppendMenu(MF_STRING, ID_MARK_TEXT,	_T("�������  CTRL+Q"));
			pMarkPopMenu->AppendMenu(MF_STRING, ID_MARK_1,		_T("��Ǣ�"));
			pMarkPopMenu->AppendMenu(MF_STRING, ID_MARK_2,		_T("��Ǣ�"));
			pMarkPopMenu->AppendMenu(MF_STRING, ID_MARK_3,		_T("��Ǣ�"));
			pMarkPopMenu->AppendMenu(MF_STRING, ID_MARK_4,		_T("��Ǣ�"));
			pMarkPopMenu->AppendMenu(MF_STRING, ID_MARK_5,		_T("��Ǣ�"));
			pMarkPopMenu->AppendMenu(MF_STRING, ID_MARK_6,		_T("��Ǣ�"));
			pMarkPopMenu->AppendMenu(MF_SEPARATOR);
			pMarkPopMenu->AppendMenu(MF_STRING, ID_MARK_MANAGER, _T("��ǹ���"));
		}	
	}

	pPopMenu->AppendODMenu(L"", MF_SEPARATOR);

	// ��������:
	CNewMenu* pIoViewPopMenu = pPopMenu->AppendODPopupMenu(L"��������");
	ASSERT(NULL != pIoViewPopMenu );
	AppendIoViewsMenu(pIoViewPopMenu, IsLockedSplit());

	// �ر�����:
	pPopMenu->AppendODMenu(L"�ر�����", MF_STRING, IDM_IOVIEWBASE_CLOSECUR);	

	// �����л�:	->��ΪF5�л�
	pPopMenu->AppendODMenu(L"�����л� F5", MF_STRING, ID_CHART_CHANGETAB);
	pPopMenu->AppendODMenu(L"", MF_SEPARATOR);

	// ѡ����Ʒ
	pPopMenu->AppendODMenu(L"ѡ����Ʒ", MF_STRING, IDM_IOVIEWBASE_MERCH_CHANGE);

	// ��ʾ������	
	if (m_bShowGridLine)
	{
		pPopMenu->AppendODMenu(L"����������",MF_STRING,IDM_IOVIEWREPORT_GRIDLINE);
	}
	else
	{
		pPopMenu->AppendODMenu(L"��ʾ������",MF_STRING,IDM_IOVIEWREPORT_GRIDLINE);
	}

	// �Զ���ҳ&����
	pPopMenu->AppendODMenu(L"�Զ���ҳ", MF_STRING, ID_REPORT_AUTOPAGE);
	if ( IsAutoPageStarted() )
	{
		pPopMenu->CheckMenuItem(ID_REPORT_AUTOPAGE, MF_BYCOMMAND |MF_CHECKED);
	}
	pPopMenu->AppendODMenu(L"��ҳʱ������", MF_STRING, ID_REPORT_AUTOPAGESETTING);


	pPopMenu->AppendODMenu(L"",MF_SEPARATOR);

	// ȫ��/�ָ�
	pPopMenu->AppendODMenu(L"ȫ��/�ָ� F7", MF_STRING, IDM_IOVIEWBASE_F7);
	pPopMenu->AppendODMenu(L"", MF_SEPARATOR);

	// ����Ŵ�
	pPopMenu->AppendODMenu(L"����Ŵ� CTRL+��", MF_STRING, IDM_IOVIEWBASE_FONT_BIGGER);

	// ������С
	pPopMenu->AppendODMenu(L"������С CTRL+��", MF_STRING, IDM_IOVIEWBASE_FONT_SMALLER);

	pPopMenu->AppendODMenu(L"", MF_SEPARATOR);

	// �ָ��
	CNewMenu* pSplitWndMenu = pPopMenu->AppendODPopupMenu(L"�ָ��");
	pSplitWndMenu->AppendODMenu(L"�������ͼ", MF_STRING, IDM_IOVIEWBASE_SPLIT_TOP);
	pSplitWndMenu->AppendODMenu(L"�������ͼ", MF_STRING, IDM_IOVIEWBASE_SPLIT_BOTTOM);
	pSplitWndMenu->AppendODMenu(L"�������ͼ", MF_STRING, IDM_IOVIEWBASE_SPLIT_LEFT);
	pSplitWndMenu->AppendODMenu(L"�������ͼ", MF_STRING, IDM_IOVIEWBASE_SPLIT_RIGHT);

	// �رմ���
	pPopMenu->AppendODMenu(L"�رմ���", MF_STRING, IDM_IOVIEWBASE_CLOSE);
	pPopMenu->AppendODMenu(L"", MF_SEPARATOR);

	// �������
	pPopMenu->AppendODMenu(L"�������", MF_STRING, ID_SETTING);

	// ���沼��
	pPopMenu->AppendODMenu(L"���沼��", MF_STRING, ID_LAYOUT_ADJUST);

	// ������������ָ�״̬����Ҫɾ��һЩ��ť
	CMPIChildFrame *pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pChildFrame && pChildFrame->IsLockedSplit() )
	{
		pChildFrame->RemoveSplitMenuItem(*pPopMenu);
	}

	pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, AfxGetMainWnd());
	menu.DestroyMenu();
}

void CIoViewReport::OnGridLButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	if (NULL != pItem)
	{
		int32 iRow = pItem->iRow;
		if (0 == iRow)	// �����ͷ������ ������
		{
			return;
		}
	}

	//BringToTop();
}

void CIoViewReport::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();

	{
		// ������ʱ����
		pMain->LoadSystemDefaultCfm(ID_PIC_TREND);

		//// ȫ��״̬����ԭ
		//if (bFullScreen)
		//{
		//	CGGTongView* pGGTongView = GetParentGGtongView();
		//	pMain->OnProcessF7(pGGTongView);
		//}
		//return;
	}

	//�ҵ���ǰѡ�е���Ʒ
	int32 iTab;
	T_BlockDesc::E_BlockType eType;
	if ( !TabIsValid(iTab, eType) )
	{
		return;
	}

	if ( NULL == m_pAbsCenterManager )
	{
		return;	
	}

	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;

	int32 iRow = pItem->iRow;

	//֪ͨ��Ʒ�����˸ı�
	CGridCellSys *pCell;
	if (IsInCustomGrid())
	{
		m_bCustomGrid = TRUE;
		pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, 0);
		if ( NULL == pCell || iRow < m_CustomGridCtrl.GetFixedRowCount() )
		{
			return;
		}
	}
	else
	{
		m_bCustomGrid = FALSE;
		pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
		if ( NULL == pCell || iRow < m_GridCtrl.GetFixedRowCount() )
		{
			return;
		}
	}

	CMerch *pMerch = (CMerch *)pCell->GetData();

	// ��ʱ��ת
	if ( NULL == pMerch )
	{
		return;
	}

	pMain->OnShowMerchInChart(pMerch, this);

	CGGTongView* pGGTongView = (CGGTongView*)pMain->FindGGTongView();
	pGGTongView->DealEnterKey();
}

void CIoViewReport::OnGridColWidthChanged(NMHDR *pNotifyStruct, LRESULT* pResult)
{	
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;

	int32 iCol = pItem->iColumn;
	int32 iWidth = pItem->iParam1;

	if (iCol < 0 && iCol >= m_GridCtrl.GetColumnCount())
		return;

	ASSERT(iWidth >= 0);

	if (iCol>=0)
	{		
		if ( BeSpecial() )
		{
			CReportScheme::Instance()->SaveColumnWidth(ERTFutureSpot,iCol,iWidth);		
			CReportScheme::Instance()->GetReportHeadInfoList(ERTFutureSpot,m_ReportHeadInfoList,m_iFixCol);
		}
		else
		{
			CReportScheme::Instance()->SaveColumnWidth(m_eMarketReportType,iCol,iWidth);		
			CReportScheme::Instance()->GetReportHeadInfoList(m_eMarketReportType,m_ReportHeadInfoList,m_iFixCol);
		}		
	}	
}

void CIoViewReport::OnGridKeyDownEnd(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	// ���¼�����ͷ��
	if (m_bCustomGrid)
	{	
		T_TabInfo stTabInfo = m_tabCustom;
		int32 iMerchSize = 0;
		iMerchSize = stTabInfo.m_Block.m_aMerchs.GetSize();

		int32 iBeginNow = stTabInfo.m_iDataIndexBegin;
		int32 iEndNow	= stTabInfo.m_iDataIndexEnd;

		if ( iEndNow == (iMerchSize - 1) )
		{
			// 			CustomGridMouseWheel(-1);
			// 			
			// 			CCellID cellFocus(1, 0);
			// 			m_CustomGridCtrl.SetFocusCell(cellFocus);
			// 			m_CustomGridCtrl.SetSelectedSingleRow(cellFocus.row);
			// 			m_CustomGridCtrl.BlindKeyFocusCell(true);

			return;
		}
		else
		{
			iBeginNow += 1;
			iEndNow	  += 1;
		}

		UpdateLocalDataIndex2(iBeginNow, iEndNow);
		ReSetCustomGridCellDataByIndex(0);

		if ( m_bRequestViewSort )
		{
			// ��������, ��������Ժ������ǰ�ɼ���ʵʱ����
			CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
			CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
			bool32 bCanReqSort = NULL==pApp?false:!pApp->m_bOffLine;
			bCanReqSort = bCanReqSort && (NULL==pMainFrame? false:pMainFrame->GetUserConnectCmdFlag()&EUCCF_ConnectDataServer);
			RequestViewDataSortAsync();
			if ( !bCanReqSort )
			{
				// ������ԶҲ�ز�������ʾ��
				UpdateCustomTableContent(TRUE, NULL, false);
				RequestViewDataCurrentVisibleRowAsync();
			}
		}
		else
		{
			// ����ǰ�ɼ�������
			UpdateCustomTableContent(TRUE, NULL, false);
			RequestViewDataCurrentVisibleRowAsync();
		}
	}
	else
	{
		int32 iTab;
		T_BlockDesc::E_BlockType eType;

		if ( !TabIsValid(iTab, eType) )
		{
			return;
		}

		T_TabInfo stTabInfo = m_aTabInfomations.GetAt(iTab);
		int32 iMerchSize = 0;

		if ( T_BlockDesc::EBTPhysical == eType )
		{
			iMerchSize = stTabInfo.m_Block.m_pMarket->m_MerchsPtr.GetSize();
		}
		else
		{
			iMerchSize = stTabInfo.m_Block.m_aMerchs.GetSize();
		}

		int32 iBeginNow = stTabInfo.m_iDataIndexBegin;
		int32 iEndNow	= stTabInfo.m_iDataIndexEnd;

		if ( iEndNow == (iMerchSize - 1) )
		{
			// 			OnMouseWheel(-1);
			// 			
			// 			CCellID cellFocus(1, 0);
			// 			m_GridCtrl.SetFocusCell(cellFocus);
			// 			m_GridCtrl.SetSelectedSingleRow(cellFocus.row);
			// 			m_GridCtrl.BlindKeyFocusCell(true);

			return;
		}
		else
		{
			iBeginNow += 1;
			iEndNow	  += 1;
		}

		UpdateLocalDataIndex(iTab, iBeginNow, iEndNow);
		ReSetGridCellDataByIndex();

		if ( m_bRequestViewSort )
		{
			// ��������, ��������Ժ������ǰ�ɼ���ʵʱ����
			CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
			CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
			bool32 bCanReqSort = NULL==pApp?false:!pApp->m_bOffLine;
			bCanReqSort = bCanReqSort && (NULL==pMainFrame? false:pMainFrame->GetUserConnectCmdFlag()&EUCCF_ConnectDataServer);
			RequestViewDataSortAsync();
			if ( !bCanReqSort )
			{
				// ������ԶҲ�ز�������ʾ��
				UpdateTableContent(TRUE, NULL, false);
				RequestViewDataCurrentVisibleRowAsync();
			}
		}
		else
		{
			// ����ǰ�ɼ�������
			UpdateTableContent(TRUE, NULL, false);
			RequestViewDataCurrentVisibleRowAsync();
		}
	}
}

void CIoViewReport::OnGridKeyUpEnd(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	// ���ϼ�����ͷ��
	if (m_bCustomGrid)
	{
		T_TabInfo stTabInfo = m_tabCustom;
		int32 iMerchSize = 0;

		iMerchSize = stTabInfo.m_Block.m_aMerchs.GetSize();

		int32 iBeginNow = stTabInfo.m_iDataIndexBegin;
		int32 iEndNow	= stTabInfo.m_iDataIndexEnd;

		if ( iBeginNow == 0 )
		{
			// 			CustomGridMouseWheel(1);
			// 			
			// 			CCellID cellFocus(m_CustomGridCtrl.GetRowCount() - 1, 0);
			// 			m_CustomGridCtrl.SetFocusCell(cellFocus);
			// 			m_CustomGridCtrl.SetSelectedSingleRow(cellFocus.row);
			// 			m_CustomGridCtrl.BlindKeyFocusCell(true);

			return;
		}
		else
		{
			iBeginNow -= 1;
			iEndNow	  -= 1;

			// �п��ܵ�ǰҳ��������������Ҫ���䵱ǰҳ�������
			// ���һ��������ȫ�ɼ�����grid���µĸ߶��㹻����һ�У���endnow������
			CCellRange rangeVis = m_CustomGridCtrl.GetVisibleNonFixedCellRange(NULL, NULL, FALSE);
			if ( rangeVis.IsValid()
				&& rangeVis.GetMaxRow() == m_CustomGridCtrl.GetRowCount()-1 )
			{
				CRect rcCell(0,0,0,0), rcGrid(0,0,0,0);
				m_CustomGridCtrl.GetClientRect(&rcGrid);
				CCellID cell(rangeVis.GetMaxRow(), rangeVis.GetMinCol());
				if ( m_CustomGridCtrl.GetCellRect(cell, &rcCell)
					&& rcGrid.bottom-rcCell.bottom >= rcCell.Height() )
				{
					iEndNow += 1;	// ����һ��
					m_CustomGridCtrl.InsertRow(_T(" "));
				}
			}
		}

		CCellID cellFocus(m_CustomGridCtrl.GetRowCount() - 1, 0);
		m_CustomGridCtrl.SetFocusCell(cellFocus);

		UpdateLocalDataIndex2(iBeginNow, iEndNow);
		ReSetCustomGridCellDataByIndex(0);
		UpdateCustomTableContent(TRUE, NULL, false);

		RequestViewDataCurrentVisibleRowAsync();
		RequestViewDataSortAsync();
	}
	else
	{
		int32 iTab;
		T_BlockDesc::E_BlockType eType;

		if ( !TabIsValid(iTab, eType) )
		{
			return;
		}

		T_TabInfo stTabInfo = m_aTabInfomations.GetAt(iTab);
		int32 iMerchSize = 0;

		if ( T_BlockDesc::EBTPhysical == eType )
		{
			iMerchSize = stTabInfo.m_Block.m_pMarket->m_MerchsPtr.GetSize();
		}
		else
		{
			iMerchSize = stTabInfo.m_Block.m_aMerchs.GetSize();
		}


		int32 iBeginNow = stTabInfo.m_iDataIndexBegin;
		int32 iEndNow	= stTabInfo.m_iDataIndexEnd;

		if ( iBeginNow == 0 )
		{
			// 			OnMouseWheel(1);
			// 			
			// 			CCellID cellFocus(m_GridCtrl.GetRowCount() - 1, 0);
			// 			m_GridCtrl.SetFocusCell(cellFocus);
			// 			m_GridCtrl.SetSelectedSingleRow(cellFocus.row);
			// 			m_GridCtrl.BlindKeyFocusCell(true);

			return;
		}
		else
		{
			iBeginNow -= 1;
			iEndNow	  -= 1;

			// �п��ܵ�ǰҳ��������������Ҫ���䵱ǰҳ�������
			// ���һ��������ȫ�ɼ�����grid���µĸ߶��㹻����һ�У���endnow������
			CCellRange rangeVis = m_GridCtrl.GetVisibleNonFixedCellRange(NULL, NULL, FALSE);
			if ( rangeVis.IsValid()
				&& rangeVis.GetMaxRow() == m_GridCtrl.GetRowCount()-1 )
			{
				CRect rcCell(0,0,0,0), rcGrid(0,0,0,0);
				m_GridCtrl.GetClientRect(&rcGrid);
				CCellID cell(rangeVis.GetMaxRow(), rangeVis.GetMinCol());
				if ( m_GridCtrl.GetCellRect(cell, &rcCell)
					&& rcGrid.bottom-rcCell.bottom >= rcCell.Height() )
				{
					iEndNow += 1;	// ����һ��
					m_GridCtrl.InsertRow(_T(" "));
				}
			}
		}

		CCellID cellFocus(m_GridCtrl.GetRowCount() - 1, 0);
		m_GridCtrl.SetFocusCell(cellFocus);

		UpdateLocalDataIndex(iTab, iBeginNow, iEndNow);
		ReSetGridCellDataByIndex();
		UpdateTableContent(TRUE, NULL, false);

		RequestViewDataCurrentVisibleRowAsync();
		RequestViewDataSortAsync();
	}
}

// ֪ͨ��ͼ�ı��ע����Ʒ
void CIoViewReport::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch)
		return;

	// �޸ĵ�ǰ�鿴����Ʒ
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
}

//
void CIoViewReport::OnVDataForceUpdate()
{
	if ( !RequestViewDataSort() )	// ������������������������������ڷ����������󣬷�������������
	{
		RequestViewDataCurrentVisibleRowAsync();
	}
}

void CIoViewReport::OnVDataRealtimePriceUpdate(IN CMerch *pMerch)
{
	if (NULL == pMerch)
		return;

	UpdateTableContent(FALSE, pMerch, true);	
}

// �������г����з����仯
// �������г����з����仯
void CIoViewReport::OnVDataReportUpdate(int32 iMarketId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount,  const CArray<CMerch*, CMerch*> &aMerchs)
{	
	CMarket* pMarket = NULL;
	if (m_pAbsCenterManager && (m_pAbsCenterManager->GetMerchManager().FindMarket(iMarketId,pMarket) && NULL != pMarket) )
	{
		m_TimeLast = m_pAbsCenterManager->GetServerTime();
	}

	int32 iTab;
	T_BlockDesc::E_BlockType eType;
	if ( !TabIsValid(iTab, eType) )
	{
		return;
	}

	if ( !m_bRequestViewSort )
	{
		return;
	}

	// 	if (IsInCustomGrid())
	// 	{
	// 		if ( m_MmiRequestSys.m_iMarketId == iMarketId
	// 			&& m_MmiRequestSys.m_iStart	 == iPosStart
	// 			&& m_MmiRequestSys.m_iCount	 == iOrgMerchCount//aMerchs.GetSize()		// �п���ʵ�ʷ��ص���Ʒ�б��д��ڿͻ���û��Ȩ�޻�ȡ����Ʒ��Ȼ��viewdata�޳�������������ֱȶ�ʧ�ܵ���� TODO		 
	// 			&& m_MmiRequestSys.m_bDescSort == bDescSort
	// 			&& m_MmiRequestSys.m_eMerchReportField == eMerchReportField )
	// 		{
	// 			ReSetCustomGridCellDataByMerchs(aMerchs);
	// 			UpdateCustomTableContent(TRUE, NULL, false);
	// 			
	// 			m_CustomGridCtrl.SetSortAscending(!m_MmiRequestSys.m_bDescSort);
	// 			m_CustomGridCtrl.SetSortColumn(m_iSortColumn);
	// 			
	// 			RequestViewDataCurrentVisibleRow();	
	// 			RequestViewDataSortTimerBegin();
	// 		}
	// 	}
	// 	else
	{
		if ( T_BlockDesc::EBTPhysical == eType )
		{
			if ( m_MmiRequestSys.m_iMarketId == iMarketId
				&& m_MmiRequestSys.m_iStart	 == iPosStart
				&& m_MmiRequestSys.m_iCount	 == iOrgMerchCount//aMerchs.GetSize()		// �п���ʵ�ʷ��ص���Ʒ�б��д��ڿͻ���û��Ȩ�޻�ȡ����Ʒ��Ȼ��viewdata�޳�������������ֱȶ�ʧ�ܵ���� TODO		 
				&& m_MmiRequestSys.m_bDescSort == bDescSort
				&& m_MmiRequestSys.m_eMerchReportField == eMerchReportField )
			{
				ReSetGridCellDataByMerchs(aMerchs);
				UpdateTableContent(TRUE, NULL, false);

				m_GridCtrl.SetSortAscending(!m_MmiRequestSys.m_bDescSort);
				m_GridCtrl.SetSortColumn(m_iSortColumn);

				RequestViewDataCurrentVisibleRow();	
				RequestViewDataSortTimerBegin();
			}
		}
	}
}

// ���������Ʒ�������б䶯
void CIoViewReport::OnVDataReportInBlockUpdate( int32 iBlockId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount, const CArray<CMerch *, CMerch *> &aMerchs )
{
	int32 iTab;
	T_BlockDesc::E_BlockType eType;
	if ( !TabIsValid(iTab, eType) )
	{
		return;
	}

	//if ( !m_bRequestViewSort )
	//{
	//	return;
	//}

	if ( T_BlockDesc::EBTBlockLogical == eType )
	{
		// ������� С֣������������Ҫ����
		CBlockLikeMarket	*pBlock = CBlockConfig::Instance()->FindBlock(m_aTabInfomations[iTab].m_Block.m_iMarketId);
		if ( NULL == pBlock )
		{
			return;
		}

		if ( m_MmiBlockRequestSys.m_iBlockId == iBlockId
			&& m_MmiBlockRequestSys.m_iStart	 == iPosStart		 
			&& m_MmiBlockRequestSys.m_bDescSort == bDescSort
			&& m_MmiBlockRequestSys.m_iCount <= iOrgMerchCount // ���������Ҳ��Ϊ��Ҫ���
			&& m_MmiBlockRequestSys.m_eMerchReportField == eMerchReportField )
		{
			// �õ������ͼ��Ҫ����Ʒ
			int32 iMaxRowCount = m_MmiBlockRequestSys.m_iCount;
			CArray<CMerch *, CMerch *>	aMerchMy;
			aMerchMy.SetSize(0, iMaxRowCount+1);
			for ( int i=0; i < aMerchs.GetSize() && i <= iMaxRowCount ; i++ )		// ���ɼ�����Ʒ
			{
				if ( pBlock->IsMerchInBlock(aMerchs[i]) )
				{
					aMerchMy.Add(aMerchs[i]);
				}
			}
			ReSetGridCellDataByMerchs(aMerchMy);
			UpdateTableContent(TRUE, NULL, false);

			m_GridCtrl.SetSortAscending(!m_MmiBlockRequestSys.m_bDescSort);
			m_GridCtrl.SetSortColumn(m_iSortColumn);

			RequestViewDataCurrentVisibleRow();	
			RequestViewDataSortTimerBegin();
		}
	}
	else if ( T_BlockDesc::EBTSelect == eType || T_BlockDesc::EBTMerchSort == eType)
	{
		// ����A������� С֣��������ѡ����Ʒ��Ҫ����ɸѡ
		// 		CBlockLikeMarket	*pBlock = CBlockConfig::Instance()->FindBlock(m_aTabInfomations[iTab].m_Block.m_iMarketId);
		// 		if ( NULL == pBlock )
		// 		{
		// 			return;
		// 		}

		if ( m_MmiBlockRequestSys.m_iBlockId == iBlockId
			&& m_MmiBlockRequestSys.m_iStart	 == iPosStart		 
			&& m_MmiBlockRequestSys.m_bDescSort == bDescSort
			&& m_MmiBlockRequestSys.m_iCount <= iOrgMerchCount	// ���������Ҳ��Ϊ��Ҫ���
			&& m_MmiBlockRequestSys.m_eMerchReportField == eMerchReportField )
		{
			// ����������������Ʒ������ �õ������ͼ��Ҫ����Ʒ
			CArray<CMerch *, CMerch *>	aMerchMy;
			aMerchMy.SetSize(0, aMerchs.GetSize());
			const CArray<CMerch *, CMerch *>	&aMerchsInBlock = m_aTabInfomations[iTab].m_Block.m_aMerchs;
			bool32	bEnd = false;
			const int32   iDataBegin = m_aTabInfomations[iTab].m_iDataIndexBegin;
			const int32	  iDataEnd = m_aTabInfomations[iTab].m_iDataIndexEnd;
			int32	iDataFind = 0;
			for ( int i=0; i < aMerchs.GetSize() && !bEnd; i++ )		// �������ڸ�ѡ�ɴ��ڵ���Ʒ
			{
				for ( int j=0; j < aMerchsInBlock.GetSize() ; j++ )
				{
					if ( aMerchsInBlock[j] == aMerchs[i] )
					{
						iDataFind++;
						if ( iDataFind > iDataBegin )
						{
							aMerchMy.Add(aMerchs[i]);
						}
						if ( iDataFind > iDataEnd )		// �����Ѿ�����
						{
							bEnd = true;
						}

						break;
					}
				}
			}

			if ( aMerchMy.GetSize() > 0 )
			{
				ReSetGridCellDataByMerchs(aMerchMy);
				UpdateTableContent(TRUE, NULL, false);

				m_GridCtrl.SetSortAscending(!m_MmiBlockRequestSys.m_bDescSort);
				m_GridCtrl.SetSortColumn(m_iSortColumn);

				RequestViewDataCurrentVisibleRow();	
				RequestViewDataSortTimerBegin();
			}
		}
	}
}

void CIoViewReport::OnVDataGridHeaderChanged(E_ReportType eReportType)
{
	TabChange();
}

E_MerchReportField ReportHeader2MerchReportFieldBeiJing( CReportScheme::E_ReportHeader eReportHeader,bool32& bTrans )
{
	typedef map<CReportScheme::E_ReportHeader, E_MerchReportField>	TransMap;
	static TransMap mapTrans;
	if ( mapTrans.empty() )		// ��ʼ��ת������
	{
		mapTrans[ CReportScheme::ERHPricePrevClose ]	= EMRFPricePrevClose;	// ����
		mapTrans[ CReportScheme::ERHPricePrevBalance ]	= EMRFPricePrevClose;

		mapTrans[ CReportScheme::ERHPriceOpen ]	= EMRFPriceOpen;	// ���̼�

		mapTrans[ CReportScheme::ERHPriceNew ]	= EMRFPriceNew;

		mapTrans[ CReportScheme::ERHPriceHigh ]	= EMRFPriceHigh;

		mapTrans[ CReportScheme::ERHPriceLow ]	= EMRFPriceLow;

		mapTrans[ CReportScheme::ERHVolumeCur ]	= EMRFVolumeCur;

		mapTrans[ CReportScheme::ERHVolumeTotal ]	= EMRFVolumeTotal;

		mapTrans[ CReportScheme::ERHAmount ]	= EMRFAmountTotal;

		mapTrans[ CReportScheme::ERHRiseFall ]	= EMRFRiseValue;

		mapTrans[ CReportScheme::ERHRange ]	= EMRFRisePercent;

		mapTrans[ CReportScheme::ERHSwing ]	= EMRFAmplitude;

		mapTrans[ CReportScheme::ERHVolumeRate ]	= EMRFVolRatio;

		mapTrans[ CReportScheme::ERHRate ]	= EMRFBidRatio;

		mapTrans[ CReportScheme::ERHSpeedRiseFall ]	= EMRFRiseRate;

		mapTrans[ CReportScheme::ERHBuyPrice ]	= EMRFPriceBuy;

		mapTrans[ CReportScheme::ERHSellPrice ]	= EMRFPriceSell;

		mapTrans[ CReportScheme::ERHPriceBalance ]	= EMRFPriceAverage;

		mapTrans[ CReportScheme::ERHHold ]	= EMRFHold;

		mapTrans[ CReportScheme::ERHAddPer ]	= EMRFCurHold;

		// xl 0821 ����		
		mapTrans[ CReportScheme::ERHCapitalFlow ]	= EMRFCapticalFlow;

		mapTrans[ CReportScheme::ERHChangeRate ]	= EMRFTradeRate;

		mapTrans[ CReportScheme::ERHMarketWinRate ]	= EMRFPeRate;

		mapTrans[ CReportScheme::ERHVolumeRate ]	= EMRFVolRatio;

		// ǿ���� 
		mapTrans[ CReportScheme::ERHPowerDegree ]	= EMRFIntensity;

		mapTrans[ CReportScheme::ERHBuySellRate ]	= EMRFInOutRatio;
	}

	const TransMap &mapC = mapTrans;

	TransMap::const_iterator it = mapC.find(eReportHeader);
	if ( it != mapC.end() )
	{
		bTrans = true;
		return it->second;
	}

	return EMRFCount;
}

E_ReportSortEx ReportHead2ReportSortEx(CReportScheme::E_ReportHeader eReportHeader, bool32& bTrans)
{
	bTrans = false;

	E_ReportSortEx eTypeReturn = ERSPIEnd;

	//
	if ( CReportScheme::ERHShortLineSelect == eReportHeader )
	{
		eTypeReturn = ERSPIShort;
		bTrans = true;
	}
	else if ( CReportScheme::ERHMidLineSelect == eReportHeader )
	{
		eTypeReturn = ERSPIMid;
		bTrans = true;
	}

	return eTypeReturn;
}

E_MerchReportField ReportHeader2MerchReportField( CReportScheme::E_ReportHeader eReportHeader,bool32& bTrans )
{
	bTrans = false;

	E_MerchReportField eRet = EMRFPriceNew;

	switch ( eReportHeader )
	{
	case CReportScheme::ERHRowNo:				// �к�
	case CReportScheme::ERHMerchCode:			// ����
	case CReportScheme::ERHMerchName:			// ����
	case CReportScheme::ERHTime:				// ʱ��
	case CReportScheme::ERHSelStockTime:        // ѡ��ʱ��
	case CReportScheme::ERHSelPrice:			// ��ѡ�۸�
	case CReportScheme::ERHSelRisePercent:		// ��ѡ���Ƿ�%
		break;	
		// 
	case CReportScheme::ERHPricePrevClose:		// ���ռ�
		eRet = EMRFPricePrevClose;
		bTrans = true;
		break;
	case CReportScheme::ERHPricePrevBalance:	// ����գ��ڻ������ƣ� ��ͬ�ڹ�Ʊ�����ռۣ�
		eRet = EMRFPricePreAvg;
		bTrans = true;
		break;
		// 
	case CReportScheme::ERHPriceOpen:			// ���̼�
		eRet = EMRFPriceOpen;
		bTrans = true;
		break;
		// 
	case CReportScheme::ERHPriceNew:			// ���¼�	
		eRet = EMRFPriceNew;
		bTrans = true;
		break;		
		// 
	case CReportScheme::ERHPriceHigh:			// ��� 
		eRet = EMRFPriceHigh;
		bTrans = true;
		break;
	case CReportScheme::ERHPriceLow:			// ���
		eRet = EMRFPriceLow;
		bTrans = true;
		break;
		//
	case CReportScheme::ERHVolumeCur:			// ����
		eRet = EMRFVolumeCur;
		bTrans = true;
		break;

		//
	case CReportScheme::ERHVolumeTotal:			// �ɽ���
		eRet = EMRFVolumeTotal;
		bTrans = true;
		break;

		// 
	case CReportScheme::ERHAmount:				// ���
		eRet = EMRFAmountTotal;
		bTrans = true;
		break;
	case CReportScheme::ERHRiseFall:			// �ǵ�
		eRet = EMRFRiseValue;
		bTrans = true;
		break;
	case CReportScheme::ERHRange:				// �Ƿ�%
		eRet = EMRFRisePercent;
		bTrans = true;
		break;
	case CReportScheme::ERHSwing:				// ���
		eRet = EMRFAmplitude;
		bTrans = true;
		break;
	case CReportScheme::ERHVolumeRate:			// ����
		eRet = EMRFVolRatio;
		bTrans = true;
		break;
	case CReportScheme::ERHPowerDegree:			// ǿ����
	case CReportScheme::ERHDKBallance:			// ���ƽ��
	case CReportScheme::ERHDWin:				// ��ͷ����
	case CReportScheme::ERHDLose:				// ��ͷͣ��
	case CReportScheme::ERHKWin:				// ��ͷ�ز�
	case CReportScheme::ERHKLose:				// ��ͷͣ��
		break;
	case CReportScheme::ERHRate:				// ί��
		eRet = EMRFBidRatio;
		bTrans = true;
		break;
	case CReportScheme::ERHSpeedRiseFall:		// �����ǵ�
		eRet = EMRFRiseRate;
		bTrans = true;
		break;
	case CReportScheme::ERHChangeRate:			// ������
	case CReportScheme::ERHMarketWinRate:		// ��ӯ��		
		break;
	case CReportScheme::ERHBuyPrice:			// �����
		eRet = EMRFPriceBuy;
		bTrans = true;
		break;
	case CReportScheme::ERHSellPrice:			// ������
		eRet = EMRFPriceSell;
		bTrans = true;
		break;
	case CReportScheme::ERHBuyAmount:			// ������
	case CReportScheme::ERHSellAmount:			// ������
	case CReportScheme::ERHDifferenceHold:		// �ֲֲ�
	case CReportScheme::ERHBuySellPrice:		// �����/������
	case CReportScheme::ERHBuySellVolume:		// ��/����
	case CReportScheme::ERHPreDone1:			// ǰ��1
	case CReportScheme::ERHPreDone2:			// ǰ��2
	case CReportScheme::ERHPreDone3:			// ǰ��3
	case CReportScheme::ERHPreDone4:			// ǰ��4
	case CReportScheme::ERHPreDone5:			// ǰ��5
		break;
	case CReportScheme::ERHPriceBalance:		// ����
		eRet = EMRFPriceAverage;
		bTrans = true;
		break;
	case CReportScheme::ERHHold:
		eRet = EMRFHold;
		bTrans = true;
		break;
	case CReportScheme::ERHAddPer:
		eRet = EMRFCurHold;
		bTrans = true;
		break;
	case CReportScheme::ERHAllCapital:
		eRet = EMRFAllStock;
		bTrans = true;
		break;
	case CReportScheme::ERHCircAsset:
		eRet = EMRFCircStock;
		bTrans = true;
		break;
	case CReportScheme::ERHAllAsset:
		eRet = EMRFAllAsset;
		bTrans = true;
		break;
	case CReportScheme::ERHFlowDebt:
		eRet = EMRFFlowDebt;
		bTrans = true;
		break;
	case CReportScheme::ERHPerFund:
		eRet = EMRFPerFund;
		bTrans = true;
		break;
	case CReportScheme::ERHBusinessProfit:
		eRet = EMRFBusinessProfit;
		bTrans = true;
		break;
	case CReportScheme::ERHPerNoDistribute:
		eRet = EMRFPerNoDistribute;
		bTrans = true;
		break;
	case CReportScheme::ERHPerIncomeYear:
		eRet = EMRFPerIncomeYear;
		bTrans = true;
		break;
	case CReportScheme::ERHPerPureAsset:
		eRet = EMRFPerPureAsset;
		bTrans = true;
		break;
	case CReportScheme::ERHChPerPureAsset:
		eRet = EMRFChPerPureAsset;
		bTrans = true;
		break;
	case CReportScheme::ERHDorRightRate:
		eRet = EMRFDorRightRate;
		bTrans = true;
		break;
	case CReportScheme::ERHCircMarketValue:
		eRet = EMRFCircMarketValue;
		bTrans = true;
		break;
	case CReportScheme::ERHAllMarketValue:
		eRet = EMRFAllMarketValue;
		bTrans = true;
		break;	
	default:
		break;
	}

	if ( !bTrans )
	{
		eRet = ReportHeader2MerchReportFieldBeiJing(eReportHeader, bTrans);		// ���������Ĳ鿴��case����
	}

	return eRet;
}

CReportScheme::E_ReportHeader MerchReportField2ReportHeader(  E_MerchReportField eField,bool32& bTrans )
{
	for ( int32 i = (int32)CReportScheme::ERHRowNo; i < (int32)CReportScheme::ERHCount; i++ )
	{
		CReportScheme::E_ReportHeader eHeader = (CReportScheme::E_ReportHeader)i;
		E_MerchReportField eField2 = ReportHeader2MerchReportField (eHeader, bTrans);

		if ( bTrans && eField2 == eField )
		{
			return eHeader;
		}
	}

	bTrans = false;
	return CReportScheme::ERHRowNo;
}

bool32 CIoViewReport::TabIsValid(int32& iTab, T_BlockDesc::E_BlockType& eType)
{
	int32 iSize = m_aTabInfomations.GetSize();

	if (m_iCurTab < 0 || m_iCurTab >= iSize)
	{
		return false;
	}

	iTab  = m_iCurTab;
	eType = m_aTabInfomations.GetAt(m_iCurTab).m_Block.m_eType;

	return true;
}

void CIoViewReport::OnFixedRowClickCB(CCellID& cell)
{
	if (IsInCustomGrid())
	{
		if ( CGGTongApp::m_bOffLine )
		{
			return;
		}

		if (!m_CustomGridCtrl.IsValid(cell))
		{
			return;
		}

		CCellID cell1(1,cell.col);

		if (!m_CustomGridCtrl.IsValid(cell1))
		{
			return;
		}    

		// ��ѡ��
		m_CustomGridCtrl.SetSortColumn(cell.col);
		m_CustomGridCtrl.SetHeaderSort(TRUE);

		CGridCellSys* pCell = (CGridCellSys*)m_CustomGridCtrl.GetCell(cell.row,cell.col);
		if ( NULL == pCell)
		{
			return;
		}
		SetCustomHeadInfomationList();

		bool32 bIsCodeCol = false;
		bool32 bIsNameCol = false;
		CString StrHeadName = m_ReportHeadInfoList.GetAt(cell.col).m_StrHeadNameCn;

		if ( StrHeadName == CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHMerchName) )
		{
			bIsNameCol = true;
		}
		else if ( StrHeadName == CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHMerchCode) )
		{
			bIsCodeCol = true;
		}

		//
		bool32 bUserSpecial = false;			// �Ƿ��û��������⴦�� ( ���ƺʹ���ָ�Ĭ��˳��)	
		StrHeadName = m_ReportHeadInfoList.GetAt(cell.col).m_StrHeadNameCn;

		if ( StrHeadName == CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHMerchName)
			|| StrHeadName == CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHMerchCode))
		{
			bUserSpecial = true;
		}

		static int sortType = 0;	// �õ������ͷ��������ʱ�ظ�ԭ������������
		if ( bUserSpecial || sortType >= 2)
		{
			m_bRequestViewSort = false;
			m_iSortColumn = -1;

			// ɾ����ǰ����
			m_CustomGridCtrl.DeleteNonFixedRows();

			InitialCustomMerch();

			m_CustomGridCtrl.Refresh();
			m_CustomGridCtrl.Invalidate();

			sortType = 0;
		}
		else
		{
			// ���ñ���������
			m_CustomGridCtrl.SetHeaderSort(TRUE);

			CPoint pt(0,0);
			m_CustomGridCtrl.OnFixedRowClick(cell, pt);

			m_CustomGridCtrl.SetSortColumn(cell.col);

			RequestViewDataCurrentVisibleRow();
			RequestViewDataSortTimerEnd();

			m_CustomGridCtrl.Invalidate();

			sortType++;
		}	

		return;
	}

	/////////////////////////////
	if ( CGGTongApp::m_bOffLine )
	{
		return;
	}

	if (!m_GridCtrl.IsValid(cell))
	{
		return;
	}

	CCellID cell1(1,cell.col);

	if (!m_GridCtrl.IsValid(cell1))
	{
		return;
	}    

	int32 iTab;
	T_BlockDesc::E_BlockType eType;

	if ( !TabIsValid(iTab, eType) )
	{
		return;
	}

	if ( T_BlockDesc::EBTUser == eType )
	{
		// ��ѡ��
		m_GridCtrl.SetSortColumn(cell.col);
		m_GridCtrl.SetHeaderSort(TRUE);
	}
	else
	{
		m_GridCtrl.SetHeaderSort(FALSE);
	}

	CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(cell.row,cell.col);
	if ( NULL == pCell)
	{
		return;
	}
	SetHeadInfomationList();

	bool32 bIsCodeCol = false;
	bool32 bIsNameCol = false;
	CString StrHeadName = m_ReportHeadInfoList.GetAt(cell.col).m_StrHeadNameCn;

	if ( StrHeadName == CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHMerchName) )
	{
		bIsNameCol = true;
	}
	else if ( StrHeadName == CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHMerchCode) )
	{
		bIsCodeCol = true;
	}

	//
	E_MerchReportField eField = (E_MerchReportField)pCell->GetData();

	// �������������ֶ�.
	CReportScheme::E_ReportHeader eTypeHead = CReportScheme::Instance()->GetReportHeaderEType(StrHeadName);
	if ( CReportScheme::ERHShortLineSelect == eTypeHead || CReportScheme::ERHMidLineSelect == eTypeHead )
	{
		bool32 bTrans = false;
		E_ReportSortEx eReportSortType = ReportHead2ReportSortEx(eTypeHead, bTrans);

		if ( bTrans)
		{
			// ����, ����ѡ��
			if ( m_iSortColumn != cell.col )
			{
				m_iSortColumn = cell.col;
				//m_MmiReqPlugInSort.m_bDescSort = true;//ȱʡ��һ��Ϊ����
				m_sortType = 1;
			}
			else
			{
				//m_MmiReqPlugInSort.m_bDescSort = !m_MmiReqPlugInSort.m_bDescSort;
				m_sortType++;
				if(m_sortType > 3)
				{
					m_sortType = 1;
				}
			}

			ResetTabBeginStartIndex(iTab);

			if(m_sortType == 1)
			{
				m_MmiReqPlugInSort.m_bDescSort = true;
			}
			else if(m_sortType == 2)
			{
				m_MmiReqPlugInSort.m_bDescSort = false;
			}

			if(m_sortType != 3)
			{
				m_MmiReqPlugInSort.m_eReportSortType = eReportSortType;			
				m_bRequestViewSort = true;
				RequestViewDataSort();
			}
			else
			{
				ResetSort();
			}
		}		
		return;
	}

	//
	bool32 bUserSpecial = false;										// �Ƿ��û��������⴦�� ( ���ƺʹ���ָ�Ĭ��˳��)	
	if ( T_BlockDesc::EBTUser == eType )
	{
		CString StrTmpHeadName = m_ReportHeadInfoList.GetAt(cell.col).m_StrHeadNameCn;

		if ( StrTmpHeadName == CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHMerchName)
			|| StrTmpHeadName == CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHMerchCode))
		{
			bUserSpecial = true;
		}
	}

	if ( bUserSpecial )
	{
		ResetSort();
	}
	else if ( (T_BlockDesc::EBTPhysical == eType) && (-1 != eField) )	// �˴�����������ԭ�����ı�д��,��������copy��
	{
		// �������ѡ��, ��Ҫ�������������ı�ͷ�ֶ�
		if ( m_iSortColumn != cell.col )
		{
			m_iSortColumn = cell.col;
			//m_MmiRequestSys.m_bDescSort = true;//ȱʡ��һ��Ϊ����
			m_sortType = 1;
		}
		else
		{
			//m_MmiRequestSys.m_bDescSort = !m_MmiRequestSys.m_bDescSort;
			m_sortType++;
			if(m_sortType > 3)
			{
				m_sortType = 1;
			}
		}

		m_MmiRequestSys.m_eMerchReportField = eField;

		if(m_sortType == 1)
		{
			m_MmiRequestSys.m_bDescSort = true;
		}
		else if(m_sortType == 2)
		{
			m_MmiRequestSys.m_bDescSort = false;
		}

		if(m_sortType != 3)
		{
			m_bRequestViewSort = true;
			RequestViewDataSort();
		}
		else
		{
			ResetSort();
		}
	} 
	else if ( (T_BlockDesc::EBTBlockLogical == eType) && (-1!=eField) )	// �������
	{
		// �������ѡ��,��Ҫ�������������ı�ͷ�ֶ�
		if ( bIsCodeCol || bIsNameCol )		// ����������ͷ��������
		{
			m_bRequestViewSort = false;
			m_iSortColumn = -1;

			// ɾ����ǰ����
			m_GridCtrl.DeleteNonFixedRows();

			// �������	- tabchange�лᴦ����user�й�
			TabChange();

			m_GridCtrl.Refresh();
		}
		else
		{
			if ( m_iSortColumn != cell.col )
			{
				m_iSortColumn = cell.col;
				m_sortType = 1;
				//m_MmiBlockRequestSys.m_bDescSort = true;//ȱʡ��һ��Ϊ����
			}
			else
			{
				m_sortType++;
				if(m_sortType > 3)
				{
					m_sortType = 1;
				}
				//m_MmiBlockRequestSys.m_bDescSort = !m_MmiBlockRequestSys.m_bDescSort;
			}

			ResetTabBeginStartIndex(iTab);

			m_MmiBlockRequestSys.m_eMerchReportField = eField;

			if(m_sortType == 1)
			{
				m_MmiBlockRequestSys.m_bDescSort = true;
			}
			else if(m_sortType == 2)
			{
				m_MmiBlockRequestSys.m_bDescSort = false;
			}

			if(m_sortType != 3)
			{
				m_bRequestViewSort = true;
				RequestViewDataSort();
			}
			else
			{
				ResetSort();
			}
		}
	} 
	else if ( (T_BlockDesc::EBTSelect == eType || T_BlockDesc::EBTMerchSort == eType) && (-1!=eField) )
	{
		// ����ѡ�����򣿣�
		// ���ñ���������	
		if ( bIsCodeCol || bIsNameCol )		// ����������ͷ��������
		{
			m_bRequestViewSort = false;
			m_iSortColumn = -1;

			// ɾ����ǰ����
			m_GridCtrl.DeleteNonFixedRows();

			// �������	- tabchange�лᴦ����user�й�
			TabChange();

			m_GridCtrl.Refresh();
		}
		else
		{
			CBlockLikeMarket *pBlock = CBlockConfig::Instance()->GetDefaultMarketClassBlock();
			if ( NULL == pBlock )
			{
				// û�л���A���壬�������
				m_bRequestViewSort = false;

				m_GridCtrl.SetHeaderSort(TRUE);

				CPoint pt(0,0);
				m_GridCtrl.OnFixedRowClick(cell,pt);

				RequestViewDataCurrentVisibleRow();
				RequestViewDataSortTimerEnd();

				m_GridCtrl.Invalidate();			
			}
			else
			{
				// �������ѡ��,��Ҫ�������������ı�ͷ�ֶ�
				if ( m_iSortColumn != cell.col )
				{
					m_iSortColumn = cell.col;
					m_MmiBlockRequestSys.m_bDescSort = true;//ȱʡ��һ��Ϊ����
				}
				else
				{
					m_MmiBlockRequestSys.m_bDescSort = !m_MmiBlockRequestSys.m_bDescSort;
				}

				// �µ��������ݴ�0��ʼ
				//UpdateLocalDataIndex(iTab, 0, m_aTabInfomations.GetAt(iTab).m_iDataIndexEnd - m_aTabInfomations.GetAt(iTab).m_iDataIndexBegin);
				ResetTabBeginStartIndex(iTab);

				m_MmiBlockRequestSys.m_eMerchReportField = eField;
				m_bRequestViewSort = true;
				if ( !RequestViewDataSort() && IsWindowVisible() )
				{
					ASSERT( 0 );
				}
			}
		}
	}
	else
	{
		static int sortType = 0;
		if(T_BlockDesc::EBTPhysical == eType || sortType >= 2)
		{
			ResetSort();

			sortType = 0;
		}
		else
		{
			// ���ñ���������		
			m_GridCtrl.SetHeaderSort(TRUE);

			CPoint pt(0,0);
			m_GridCtrl.OnFixedRowClick(cell,pt);

			if ( T_BlockDesc::EBTUser == eType )
			{
				m_GridCtrl.SetSortColumn(cell.col);
			}

			RequestViewDataCurrentVisibleRow();
			RequestViewDataSortTimerEnd();

			m_GridCtrl.Invalidate();
			sortType++;
		}
	}	
}

void CIoViewReport::OnFixedColumnClickCB(CCellID& cell)
{

}

void CIoViewReport::OnHScrollEnd()
{
	int32 iPos = m_GridCtrl.GetScrollPos32(SB_HORZ);
	UpdateLocalXScrollPos(m_iCurTab, iPos);
}

void CIoViewReport::OnVScrollEnd()
{
	RequestViewDataCurrentVisibleRowAsync();
	RequestViewDataSortAsync();
}

void CIoViewReport::OnCtrlMove( int32 x, int32 y )
{
}

bool32 CIoViewReport::OnEditEndCB ( int32 iRow,int32 iCol, const CString &StrOld, INOUT CString &StrNew )
{
	return false;
}

void CIoViewReport::OnGridMouseMove(CPoint pt)
{
}

bool32 CIoViewReport::BeBlindSelect()
{
	return CGGTongView::GetDragMerchFlag();
}

void CIoViewReport::OnDestroy()
{
	//	���μ��̾������ѡ�ɹ���
	// 	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	// 	pMainFrm->SetHotkeyTarget(pMainFrm);

	KillTimer(KRequestViewDataCurrentVisibleRowTimerId);
	KillTimer(KRequestViewDataSortTimerId);
	KillTimer(KRequestViewDataSortContinueTimerId);

	CMarkManager::Instance().RemoveMerchMarkChangeListener(this);
	CIoViewBase::OnDestroy();
}

void CIoViewReport::OnTimer(UINT nIDEvent) 
{
	if(!m_bShowNow)
	{
		return;
	}

	if ( nIDEvent == KRequestViewDataSortContinueTimerId )
	{
		KillTimer(KRequestViewDataSortTimerId);//Kill���Timer,����һ���ظ�.

		//����Ƿǿ����ڼ�,�򲻷�������.
		int32 iTab;
		T_BlockDesc::E_BlockType eType;
		if (!TabIsValid(iTab, eType))
		{
			return;
		}

		if ( T_BlockDesc::EBTPhysical == eType )
		{			
			CMarket* pMarket = m_aTabInfomations.GetAt(iTab).m_Block.m_pMarket;

			if ( NULL != pMarket )
			{
				CGmtTime TimeServer = m_pAbsCenterManager->GetServerTime();

				CMerch* pMerch = NULL;

				if (m_GridCtrl.GetRowCount() > 1)
				{
					pMerch = (CMerch*)m_GridCtrl.GetCell(1, 0);
				}

				if (NULL == pMerch)
				{
					return;
				}

				// �������̺�Ĳ���Ҫ������
				CMarketIOCTimeInfo RecentTradingDayTime;
				if (pMarket->GetRecentTradingDay(TimeServer, RecentTradingDayTime, pMerch->m_MerchInfo))
				{
					CGmtTime TimeRecentClose(RecentTradingDayTime.m_TimeClose.m_Time);
					TimeRecentClose += CGmtTimeSpan(0, 0, 2, 0);

					if (m_TimeLast >= TimeRecentClose)	
					{
						// ��������ʱ��2�����ˣ� �Ͳ���Ҫ�����������ˣ� 2�������ʱ����Ҫ�ǿ��Ƿ�����ʱ����ܴ���һ��ƫ���Ե��
						return;
					}
				}
			}				
		}
		else if ( T_BlockDesc::EBTBlockLogical == eType )
		{
			// ��ʧһ��cpu�ˣ��ж�������Ʒ�Ŀ�����ʱ��
			// ʵ��ֻ��Ҫ�ж�һ����Ʒ - ���ڱ��ؼ�������
			bool32 bIsInTrade = IsNowInTradeTimes(m_aTabInfomations[iTab].m_Block.m_aMerchs, true);
			if ( !bIsInTrade )
			{
				return;	// ��������
			}
		}

		RequestViewDataSort();
	}

	if ( nIDEvent == KRequestViewDataCurrentVisibleRowTimerId )
	{
		KillTimer(KRequestViewDataCurrentVisibleRowTimerId);
		RequestViewDataCurrentVisibleRow();
	}

	if ( nIDEvent == KRequestViewDataSortTimerId )
	{
		KillTimer(KRequestViewDataSortTimerId);
		RequestViewDataSort();
	}

	if ( nIDEvent == KUpdateExcelTimerId )
	{
		KillTimer(KUpdateExcelTimerId);

		int32 iTab;
		T_BlockDesc::E_BlockType eType;

		if ( !TabIsValid(iTab, eType) )
		{
			return;
		}

		T_BlockDesc BlockDesc = m_aTabInfomations.GetAt(iTab).m_Block;

		COLORREF clrRise = GetIoViewColor(ESCRise);
		COLORREF clrFall = GetIoViewColor(ESCFall);
		COLORREF clrKeep = GetIoViewColor(ESCKeep);

		AsyncGridToExcel(&m_GridCtrl, BlockDesc.m_StrBlockName, m_StrAsyncExcelFileName, -1, clrRise, clrFall, clrKeep);
	}

	if ( nIDEvent == KUpdatePushMerchsTimerId )
	{
		UpdatePushMerchs();
		// RequestViewDataCurrentVisibleRow();		
	}

	if ( nIDEvent == KTimerPeriodAutoPage )
	{
		if ( --m_iAutoPageTimerCount <= 0 )
		{
			m_iAutoPageTimerCount = m_iAutoPageTimerPeriod;
			// �����Զ���ҳ
			if ( m_bAutoPageDirection )
			{
				int32 iTab;
				T_BlockDesc::E_BlockType eType;
				bool32 bTabValid = false;
				int32 iEndPos = INT_MAX;
				if ( (bTabValid=TabIsValid(iTab, eType)) )
				{
					iEndPos = m_aTabInfomations[iTab].m_iDataIndexEnd;
				}
				OnMouseWheel(-1, TRUE);	// ���·�ҳ

				if ( bTabValid && iEndPos != INT_MAX )
				{
					if ( iEndPos == m_aTabInfomations[iTab].m_iDataIndexEnd && m_aTabInfomations[iTab].m_iDataIndexBegin > 0 )
					{
						// ����βҳ�����ˣ����Ϸ�ҳ
						m_bAutoPageDirection = !m_bAutoPageDirection;
						OnMouseWheel(1, TRUE);
					}
				}
			}
			else
			{
				int32 iTab;
				T_BlockDesc::E_BlockType eType;
				bool32 bTabValid = false;
				int32 iStartPos = INT_MAX;
				if ( (bTabValid=TabIsValid(iTab, eType)) )
				{
					iStartPos = m_aTabInfomations[iTab].m_iDataIndexBegin;
				}
				OnMouseWheel(1, TRUE);	// ���Ϸ�ҳ

				if ( bTabValid && iStartPos != INT_MAX )
				{
					if ( iStartPos == m_aTabInfomations[iTab].m_iDataIndexBegin )
					{
						// ������Ҷ
						m_bAutoPageDirection = !m_bAutoPageDirection;
					}
				}
			}
		}
		// ����Ƿ�����ʱ����
		if ( m_iAutoPageTimerPeriod != GetAutoPageTimerPeriod() )
		{
			StartAutoPage();	// ���¿�ʼ
		}
	}

	CIoViewBase::OnTimer(nIDEvent);
}

void CIoViewReport::RequestViewDataCurrentVisibleRow()
{
	//	
	UpdatePushMerchs();

	//
	int32 i = 0;
	CMmiReqRealtimePrice Req;
	CMmiRegisterPushPrice ReqPush;

	for ( i= 0; i < m_aSmartAttendMerchs.GetSize(); i++ )
	{
		CSmartAttendMerch SmartAttendMerch = m_aSmartAttendMerchs.GetAt(i);
		if ( NULL == SmartAttendMerch.m_pMerch )
		{
			continue;
		}

		if ( 0 == i )
		{
			Req.m_iMarketId			= SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
			Req.m_StrMerchCode		= SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;	

			ReqPush.m_iMarketId		= Req.m_iMarketId;
			ReqPush.m_StrMerchCode	= Req.m_StrMerchCode;
		}
		else
		{
			CMerchKey MerchKey;
			MerchKey.m_iMarketId	= SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
			MerchKey.m_StrMerchCode = SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;

			Req.m_aMerchMore.Add(MerchKey);					
			ReqPush.m_aMerchMore.Add(MerchKey);
		}
	}

	DoRequestViewData(Req);
	DoRequestViewData(ReqPush);

	//
	bool32 bNeedIndexReq = false;
	for ( i=0; i < m_ReportHeadInfoList.GetSize(); i++ )
	{
		CString StrHeadName = m_ReportHeadInfoList.GetAt(i).m_StrHeadNameCn;

		if ( StrHeadName == CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHShortLineSelect)
			|| StrHeadName == CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHMidLineSelect)
			|| StrHeadName == CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHAllNetAmount)
			|| StrHeadName == CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHNetAmountofthemain)
			)
		{
			bNeedIndexReq = true;
			break;
		}
	}
	if ( bNeedIndexReq ) 
	{
		// ����ѡ�����ݵķ�������
		CMmiReqMerchIndex	reqIndex;			// ��������
		CMmiReqPushPlugInMerchData	reqPush;	// ��������

		for ( int32 i = 0; i < m_aSmartAttendMerchs.GetSize(); i++ )
		{
			CSmartAttendMerch SmartAttendMerch = m_aSmartAttendMerchs.GetAt(i);
			if ( NULL == SmartAttendMerch.m_pMerch )
			{
				continue;
			}

			if ( 0 == i )
			{
				reqIndex.m_iMarketId		= SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
				reqIndex.m_StrMerchCode		= SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;						
				reqPush.m_iMarketID			= SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
				reqPush.m_StrMerchCode		= SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;
				reqPush.m_uType             = ECSTChooseStock;
			}
			else
			{
				CMerchKey MerchKey;
				MerchKey.m_iMarketId	= SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
				MerchKey.m_StrMerchCode = SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;

				reqIndex.m_aMerchMore.Add(MerchKey);					

				T_ReqPushMerchData PushData;
				PushData.m_iMarket = SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
				PushData.m_StrCode = SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;
				PushData.m_uType   = ECSTChooseStock;
				reqPush.m_aReqMore.Add(PushData);
			}
		}

		//	���û�й��ĵ���Ʒ���Ͳ���ͨ����
		if ( !m_aSmartAttendMerchs.IsEmpty() )
		{
			DoRequestViewData(reqIndex);
			DoRequestViewData(reqPush);
		}	
	}

	// ��������
	bool32 bNeedMainMasukuraReq = false;
	for ( i=0; i < m_ReportHeadInfoList.GetSize(); i++ )
	{
		CString StrHeadName = m_ReportHeadInfoList.GetAt(i).m_StrHeadNameCn;

		if ( StrHeadName == CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHTodayMasukuraProportion)
			|| StrHeadName == CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHTodayRanked)
			|| StrHeadName == CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHTodayRise)
			)
		{
			bNeedMainMasukuraReq = true;
			break;
		}
	}
	if ( bNeedMainMasukuraReq ) 
	{
		// ���������������ݵķ�������
		CMmiReqMainMasukura	reqMainMasukura;	// ��������
		CMmiReqPushPlugInMerchData	reqPush;	// ��������

		for ( int32 i = 0; i < m_aSmartAttendMerchs.GetSize(); i++ )
		{
			CSmartAttendMerch SmartAttendMerch = m_aSmartAttendMerchs.GetAt(i);
			if ( NULL == SmartAttendMerch.m_pMerch )
			{
				continue;
			}

			if ( 0 == i )
			{
				reqMainMasukura.m_iMarketId			= SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
				reqMainMasukura.m_StrMerchCode		= SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;						
				reqPush.m_iMarketID			= SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
				reqPush.m_StrMerchCode		= SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;
				reqPush.m_uType             = ECSTMainMasukura;
			}
			else
			{
				CMerchKey MerchKey;
				MerchKey.m_iMarketId	= SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
				MerchKey.m_StrMerchCode = SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;

				reqMainMasukura.m_aMerchMore.Add(MerchKey);					

				T_ReqPushMerchData PushData;
				PushData.m_iMarket = SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
				PushData.m_StrCode = SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;
				PushData.m_uType   = ECSTMainMasukura;
				reqPush.m_aReqMore.Add(PushData);
			}
		}

		//	���û�й��ĵ���Ʒ���Ͳ���ͨ����
		if ( !m_aSmartAttendMerchs.IsEmpty() )
		{
			DoRequestViewData(reqMainMasukura);
			reqPush.m_eCommTypePlugIn = ECTPIReqAddPushMainMasukura;
			DoRequestViewData(reqPush);
		}	
	}

	// �����������
	CMmiReqPublicFile	reqF10;
	reqF10.m_ePublicFileType = EPFTF10;
	for ( i= 0; i < m_aSmartAttendMerchs.GetSize(); i++ )
	{
		CSmartAttendMerch SmartAttendMerch = m_aSmartAttendMerchs.GetAt(i);
		if ( NULL == SmartAttendMerch.m_pMerch || !CheckFlag(SmartAttendMerch.m_iDataServiceTypes, EDSTGeneral) ) // ����ע��������
		{
			continue;
		}


		reqF10.m_iMarketId			= SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
		reqF10.m_StrMerchCode		= SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;						

		if ( !DoRequestViewData(reqF10) )
		{
			break;		// ����������������
		}
	}
}

bool32 CIoViewReport::RequestViewDataSort(bool32 bSendRequest)
{
	if ( m_iSortColumn >= 0 && m_iSortColumn < m_ReportHeadInfoList.GetSize() )
	{
		CString StrSortHeadName = m_ReportHeadInfoList.GetAt(m_iSortColumn).m_StrHeadNameCn;

		// 
		CReportScheme::E_ReportHeader eType = CReportScheme::Instance()->GetReportHeaderEType(StrSortHeadName);
		if ( CReportScheme::ERHShortLineSelect == eType || CReportScheme::ERHMidLineSelect == eType )
		{
			// ��ǰ��
			return (RequestViewDataSortPlugIn(bSendRequest));		
		}
	}

	//
	if ( !m_bRequestViewSort )
	{
		return false;
	}

	CRect RectGrid;
	m_GridCtrl.GetClientRect(&RectGrid);

	int32 iTab;
	T_BlockDesc::E_BlockType eType;

	if ( !TabIsValid(iTab, eType) )
	{
		return false;
	}

	m_MmiRequestSys.m_iStart = m_aTabInfomations.GetAt(iTab).m_iDataIndexBegin;
	m_MmiRequestSys.m_iCount = m_aTabInfomations.GetAt(iTab).m_iDataIndexEnd - m_aTabInfomations.GetAt(iTab).m_iDataIndexBegin + 1;

	if ( T_BlockDesc::EBTPhysical == eType )
	{
		if ( bSendRequest )
		{						
			DoRequestViewData(m_MmiRequestSys);
		}
	}
	else if ( T_BlockDesc::EBTBlockLogical == eType )
	{
		m_MmiBlockRequestSys.m_iStart = m_MmiRequestSys.m_iStart;
		m_MmiBlockRequestSys.m_iCount = m_MmiRequestSys.m_iCount;
		m_MmiBlockRequestSys.m_iBlockId = m_aTabInfomations[iTab].m_Block.m_iMarketId;
		if ( bSendRequest )
		{			
			DoRequestViewData(m_MmiBlockRequestSys);
		}	
	}
	else if ( T_BlockDesc::EBTSelect == eType || T_BlockDesc::EBTMerchSort == eType)
	{
		// ѡ����Ʒ���ǻ���A�ɵģ�����������A��Ʒ����
		CBlockLikeMarket *pBlock = CBlockConfig::Instance()->GetDefaultMarketClassBlock();
		if ( NULL == pBlock )
		{
			return false;
		}
		m_MmiBlockRequestSys.m_iBlockId = pBlock->m_blockInfo.m_iBlockId;
		m_MmiBlockRequestSys.m_iStart = 0;					
		m_MmiBlockRequestSys.m_iCount = pBlock->m_blockInfo.m_aSubMerchs.GetSize();	// ������Ʒ����ʵ�����п�������
		if ( bSendRequest )
		{			
			DoRequestViewData(m_MmiBlockRequestSys);
		}
	}

	return true;
}

bool32 CIoViewReport::RequestViewDataSortPlugIn(bool32 bSendRequest)
{
	if ( !m_bRequestViewSort )
	{
		return false;
	}

	CRect RectGrid;
	m_GridCtrl.GetClientRect(&RectGrid);

	int32 iTab;
	T_BlockDesc::E_BlockType eType;

	if ( !TabIsValid(iTab, eType) )
	{
		return false;
	}

	m_MmiReqPlugInSort.m_iStart = 0;
	m_MmiReqPlugInSort.m_iCount = -1;

	if ( bSendRequest )
	{						
		DoRequestViewData(m_MmiReqPlugInSort);
	}

	return true;
}

void CIoViewReport::UpdatePushMerchs()
{
	m_aSmartAttendMerchs.RemoveAll();

	if ( !IsWindowVisible() )
	{
		return;
	}

	//
	int32 iTab;
	T_BlockDesc::E_BlockType eType;

	if ( !TabIsValid(iTab, eType) )
	{
		return ;
	}

	if ( m_GridCtrl.GetRowCount() <= 1 )
	{
		return;
	}

	// �жϱ��۱�ͷ��ȷ���Ƿ���Ҫ��������
	bool32 bNeedF10 = false;
	if ( !CReportScheme::IsFuture(m_eMarketReportType) )
	{
		for ( int32 i=0; i < m_ReportHeadInfoList.GetSize() ; i++ )
		{
			if ( (m_ReportHeadInfoList[i].m_eReportHeader >= CReportScheme::ERHAllCapital 
				&& m_ReportHeadInfoList[i].m_eReportHeader <= CReportScheme::ERHAllMarketValue) 
				|| CReportScheme::ERHNetAmountofthemain ==  m_ReportHeadInfoList[i].m_eReportHeader)
			{
				bNeedF10 = true;
				break;
			}
		}
	}

	// ���⴦������ѡ�ɽ����ʱ��û��ʾ����ƷҲ����
	if (T_BlockDesc::EBTUser == eType)
	{

		//	��ȡ��ѡ�����������а�鼯����Ʒ
		CArray<T_Block, T_Block&>	arrUserAllBolck;
		arrUserAllBolck.RemoveAll();
		CUserBlockManager::Instance()->GetBlocks(arrUserAllBolck);
		for (int i =0; i < arrUserAllBolck.GetCount(); ++i)
		{
			const T_Block& subBlock = arrUserAllBolck.GetAt(i);
			for (int j =0; j < subBlock.m_aMerchs.GetCount(); ++j)
			{				
				CMerch *pMerch = subBlock.m_aMerchs[j];
				if(NULL == pMerch)
				{
					continue;
				}

				CSmartAttendMerch SmartAttendMerch;
				SmartAttendMerch.m_pMerch = pMerch;
				SmartAttendMerch.m_iDataServiceTypes = EDSTPrice;		// +F10�������ݹ�ע - �ݲ��Ż�
				if ( bNeedF10 )
				{
					SmartAttendMerch.m_iDataServiceTypes |=  EDSTGeneral;	// ���ڻ���Ʒ���Ӳ�������
				}
				m_aSmartAttendMerchs.Add(SmartAttendMerch);
			}
		}		
	}
	else
	{
		CCellID idTopLeft = m_GridCtrl.GetTopleftNonFixedCell();
		int minVisibleRow = idTopLeft.row;		

		CRect VisRect;
		CCellRange VisCellRange = m_GridCtrl.GetVisibleNonFixedCellRange(VisRect);
		int maxVisibleRow = VisCellRange.GetMaxRow();	

		
		for (int32 i = minVisibleRow; i <= maxVisibleRow; i ++ )
		{
			CGridCellSys *pFirstCell = (CGridCellSys *)m_GridCtrl.GetCell(i, 0);
			ASSERT(NULL != pFirstCell);
			//
			CMerch *pMerch = (CMerch *)pFirstCell->GetData();

			if(NULL == pMerch)
			{
				continue;
			}

			CSmartAttendMerch SmartAttendMerch;
			SmartAttendMerch.m_pMerch = pMerch;
			SmartAttendMerch.m_iDataServiceTypes = EDSTPrice;		// +F10�������ݹ�ע - �ݲ��Ż�
			if ( bNeedF10 )
			{
				SmartAttendMerch.m_iDataServiceTypes |=  EDSTGeneral;	// ���ڻ���Ʒ���Ӳ�������
			}
			m_aSmartAttendMerchs.Add(SmartAttendMerch);
		}

		////////////////////////////////////////////////
		if ( m_CustomGridCtrl.GetRowCount() <= 1 || !m_bShowCustomGrid)
		{
			return;
		}
		for (int i = 1; i < m_CustomGridCtrl.GetRowCount(); i ++ )
		{
			CGridCellSys *pFirstCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(i, 0);
			ASSERT(NULL != pFirstCell);
			//
			CMerch *pMerch = (CMerch *)pFirstCell->GetData();

			if(NULL == pMerch)
			{
				continue;
			}

			CSmartAttendMerch SmartAttendMerch;
			SmartAttendMerch.m_pMerch = pMerch;
			SmartAttendMerch.m_iDataServiceTypes = EDSTPrice;		// +F10�������ݹ�ע - �ݲ��Ż�
			if ( bNeedF10 )
			{
				SmartAttendMerch.m_iDataServiceTypes |=  EDSTGeneral;	// ���ڻ���Ʒ���Ӳ�������
			}
			m_aSmartAttendMerchs.Add(SmartAttendMerch);
		}
	}
}

void CIoViewReport::RequestViewDataSortAsync()
{
	KillTimer(KRequestViewDataSortTimerId);
	SetTimer(KRequestViewDataSortTimerId, KRequestViewDataSortTimerPeriod, NULL);
}

void CIoViewReport::RequestViewDataCurrentVisibleRowAsync()
{
	KillTimer(KRequestViewDataCurrentVisibleRowTimerId);
	SetTimer(KRequestViewDataCurrentVisibleRowTimerId, KRequestViewDataCurrentVisibleRowTimerPeriod, NULL);
}

void CIoViewReport::RequestViewDataSortTimerBegin()
{
	m_bRequestViewSort = true;
	KillTimer(KRequestViewDataSortContinueTimerId);
	SetTimer(KRequestViewDataSortContinueTimerId, KRequestViewDataSortContinueTimerPeriod, NULL);
}

void CIoViewReport::RequestViewDataSortTimerEnd()
{
	m_bRequestViewSort = false;
	KillTimer(KRequestViewDataSortContinueTimerId);
}

bool32 CIoViewReport::ValidBlock(T_BlockDesc& Block)
{
	// ���ģ�鴫��� T_BlockDesc ����, �����е����ݳ�Աû�и�ֵ.
	if ( T_BlockDesc::EBTPhysical == Block.m_eType )
	{
		if ( -1 == Block.m_iMarketId && NULL == Block.m_pMarket &&  Block.m_StrBlockName.GetLength() <= 0 )
		{
			return false;
		}

		bool32 bValid = false;

		if ( -1 != Block.m_iMarketId )
		{
			if (m_pAbsCenterManager && (m_pAbsCenterManager->GetMerchManager().FindMarket(Block.m_iMarketId, Block.m_pMarket)) )
			{
				Block.m_StrBlockName = Block.m_pMarket->m_MarketInfo.m_StrCnName;
				bValid = true;
			}			
		}

		if ( !bValid && NULL != Block.m_pMarket )
		{
			Block.m_iMarketId		= Block.m_pMarket->m_MarketInfo.m_iMarketId;
			Block.m_StrBlockName	= Block.m_pMarket->m_MarketInfo.m_StrCnName;

			bValid = true;
		}

		if ( !bValid && Block.m_StrBlockName.GetLength() > 0 )
		{
			if ( m_pAbsCenterManager && (m_pAbsCenterManager->GetMerchManager().FindMarket(Block.m_StrBlockName, Block.m_pMarket)) )
			{
				Block.m_iMarketId = Block.m_pMarket->m_MarketInfo.m_iMarketId;
				bValid = true;
			}
		}

		if ( !bValid )
		{
			return false;
		}		
	}
	else if ( T_BlockDesc::EBTUser == Block.m_eType || T_BlockDesc::EBTLogical == Block.m_eType )
	{ 
		if ( Block.m_StrBlockName.GetLength() <= 0 )
		{
			return false;
		}

		if ( T_BlockDesc::EBTUser == Block.m_eType )
		{
			if ( !CUserBlockManager::Instance()->GetMerchsInBlock(Block.m_StrBlockName, Block.m_aMerchs) )
			{
				return false;
			}
		}
		else
		{
			if ( !CSysBlockManager::Instance()->GetMerchsInBlock(Block.m_StrBlockName, Block.m_aMerchs) )
			{				
				return false;
			}
		}		
	}
	else if ( T_BlockDesc::EBTSelect == Block.m_eType || T_BlockDesc::EBTMerchSort == Block.m_eType)
	{
		// ����ѡ�� - �����������Ͷ��� CIoViewReportSelect ��
		ASSERT( IsKindOf(RUNTIME_CLASS(CIoViewReportSelect)) );
	}
	else if ( T_BlockDesc::EBTRecentView == Block.m_eType)
	{

	}
	else if ( T_BlockDesc::EBTBlockLogical == Block.m_eType )
	{
		// �ж��Ƿ���ڸð��
		CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(Block.m_iMarketId);
		return NULL != pBlock;
	}

	return true;
}

void CIoViewReport::ValidateTableInfo(int32 iIndex /*= -1*/)
{
	CMarket* pMarket;
	int32 i, iSize = m_aTabInfomations.GetSize();

	int32 iDelTabCount = 0;

	for ( i = iSize - 1; i >= 0; i-- )
	{
		if ( -1 != iIndex )
		{
			if ( i != iIndex )
			{
				continue;
			}
		}

		bool32 bBlockDeleted = false;

		//
		T_TabInfo   stTabInfo = m_aTabInfomations.GetAt(i);
		T_BlockDesc BlockDesc = stTabInfo.m_Block;

		switch ( BlockDesc.m_eType )
		{
		case T_BlockDesc::EBTPhysical:
			{
				// ������
				pMarket = NULL;
				if (m_pAbsCenterManager && (m_pAbsCenterManager->GetMerchManager().FindMarket(BlockDesc.m_iMarketId, pMarket) && NULL != pMarket) )
				{
					BlockDesc.m_pMarket = pMarket;

					if ( stTabInfo.m_iDataIndexBegin < 0 || stTabInfo.m_iDataIndexBegin >= pMarket->m_MerchsPtr.GetSize() )
					{
						stTabInfo.m_iDataIndexBegin = 0;
					}

					if ( stTabInfo.m_iDataIndexEnd < 0 || stTabInfo.m_iDataIndexEnd >= pMarket->m_MerchsPtr.GetSize() )
					{
						stTabInfo.m_iDataIndexEnd = pMarket->m_MerchsPtr.GetSize() - 1;
					}
				}
				else
				{
					//ASSERT(0);
					m_aTabInfomations.RemoveAt(i);
					bBlockDeleted = true;
				}
			}
			break;
		case T_BlockDesc::EBTUser:
			{
				// �û����
				if ( NULL != CUserBlockManager::Instance()->GetBlock(BlockDesc.m_StrBlockName) )
				{
					if ( !CUserBlockManager::Instance()->GetMerchsInBlock(BlockDesc.m_StrBlockName, BlockDesc.m_aMerchs) )
					{
						//ASSERT(0);
						m_aTabInfomations.RemoveAt(i);
						bBlockDeleted = true;
					}
					else
					{
						if ( BlockDesc.m_aMerchs.GetSize() > 0 )
						{
							if ( stTabInfo.m_iDataIndexBegin < 0 || stTabInfo.m_iDataIndexBegin >= BlockDesc.m_aMerchs.GetSize() )
							{
								stTabInfo.m_iDataIndexBegin = 0;
							}

							if ( stTabInfo.m_iDataIndexEnd < 0 || stTabInfo.m_iDataIndexEnd >= BlockDesc.m_aMerchs.GetSize() )
							{
								stTabInfo.m_iDataIndexEnd = BlockDesc.m_aMerchs.GetSize() - 1;
							}
						}
						else
						{
							stTabInfo.m_iDataIndexBegin = -1;
							stTabInfo.m_iDataIndexEnd	= -1;
						}						
					}
				}						
				else
				{
					//ASSERT(0);
					m_aTabInfomations.RemoveAt(i);
					bBlockDeleted = true;
				}
			}	
			break;			
		case T_BlockDesc::EBTLogical:			
			{
				// �߼����
				if ( NULL != CSysBlockManager::Instance()->GetBlock(BlockDesc.m_StrBlockName) )
				{
					CArray<CMerch*, CMerch*> aMerchs;

					if ( !CSysBlockManager::Instance()->GetMerchsInBlock(BlockDesc.m_StrBlockName, BlockDesc.m_aMerchs) )
					{
						//ASSERT(0);
						m_aTabInfomations.RemoveAt(i);
						bBlockDeleted = true;
					}
					else
					{
						if ( BlockDesc.m_aMerchs.GetSize() > 0 )
						{
							if ( stTabInfo.m_iDataIndexBegin < 0 || stTabInfo.m_iDataIndexBegin >= BlockDesc.m_aMerchs.GetSize() )
							{
								stTabInfo.m_iDataIndexBegin = 0;
							}

							if ( stTabInfo.m_iDataIndexEnd < 0 || stTabInfo.m_iDataIndexEnd >= BlockDesc.m_aMerchs.GetSize() )
							{
								stTabInfo.m_iDataIndexEnd = BlockDesc.m_aMerchs.GetSize() - 1;
							}
						}
						else
						{
							stTabInfo.m_iDataIndexBegin = -1;
							stTabInfo.m_iDataIndexEnd	= -1;
						}						
					}
				}
				else
				{
					//ASSERT(0);
					m_aTabInfomations.RemoveAt(i);
					bBlockDeleted = true;
				}
			}
			break;
		case T_BlockDesc::EBTSelect:
		case T_BlockDesc::EBTMerchSort:
			{
				// ����ѡ�� - ����ѡ�ɵ���Ʒ����aMerchs������, ��Ҫ����������UserStock���Ƶ�֪ͨ�ӿ�
				if ( BlockDesc.m_aMerchs.GetSize() > 0 )
				{
					if ( stTabInfo.m_iDataIndexBegin < 0 || stTabInfo.m_iDataIndexBegin >= BlockDesc.m_aMerchs.GetSize() )
					{
						stTabInfo.m_iDataIndexBegin = 0;
					}

					if ( stTabInfo.m_iDataIndexEnd < 0 || stTabInfo.m_iDataIndexEnd >= BlockDesc.m_aMerchs.GetSize() )
					{
						stTabInfo.m_iDataIndexEnd = BlockDesc.m_aMerchs.GetSize() - 1;
					}
				}
				else
				{
					stTabInfo.m_iDataIndexBegin = -1;
					stTabInfo.m_iDataIndexEnd	= -1;
				}	
			}	
			break;	
		case T_BlockDesc::EBTRecentView:
			{
				if ( BlockDesc.m_aMerchs.GetSize() > 0 )
				{
					if ( stTabInfo.m_iDataIndexBegin < 0 || stTabInfo.m_iDataIndexBegin >= BlockDesc.m_aMerchs.GetSize() )
					{
						stTabInfo.m_iDataIndexBegin = 0;
					}

					if ( stTabInfo.m_iDataIndexEnd < 0 || stTabInfo.m_iDataIndexEnd >= BlockDesc.m_aMerchs.GetSize() )
					{
						stTabInfo.m_iDataIndexEnd = BlockDesc.m_aMerchs.GetSize() - 1;
					}
				}
				else
				{
					stTabInfo.m_iDataIndexBegin = -1;
					stTabInfo.m_iDataIndexEnd	= -1;
				}	
			}
			break;
		case T_BlockDesc::EBTBlockLogical:
			{
				// �����߼����
				CBlockLikeMarket *pBlock =  CBlockConfig::Instance()->FindBlock(BlockDesc.m_iMarketId);
				if ( NULL != pBlock )
				{
					// ��Ʒ���ݲ���ȡ���ڰ�����������ݣ�����ʹ�þ�̬����
					if ( BlockDesc.m_aMerchs.GetSize() > 0 )
					{
						if ( stTabInfo.m_iDataIndexBegin < 0 || stTabInfo.m_iDataIndexBegin >= BlockDesc.m_aMerchs.GetSize() )
						{
							stTabInfo.m_iDataIndexBegin = 0;
						}

						if ( stTabInfo.m_iDataIndexEnd < 0 || stTabInfo.m_iDataIndexEnd >= BlockDesc.m_aMerchs.GetSize() )
						{
							stTabInfo.m_iDataIndexEnd = BlockDesc.m_aMerchs.GetSize() - 1;
						}
					}
					else
					{
						stTabInfo.m_iDataIndexBegin = -1;
						stTabInfo.m_iDataIndexEnd	= -1;
					}											
				}						
				else
				{
					m_aTabInfomations.RemoveAt(i);
					bBlockDeleted = true;
				}
			}	
			break;	
		default:
			;//ASSERT(0);
		}

		if ( bBlockDeleted )
		{
			iDelTabCount++;
			continue;		// ��鱻ɾ�������洦��������
		}

		UpdateLocalDataIndex(i, stTabInfo.m_iDataIndexBegin, stTabInfo.m_iDataIndexEnd);
		UpdateLocalBlock(i, BlockDesc);		

	}

	if ( iDelTabCount > 0 )
	{
		InitialTabInfos();	// ��ʼ��tabs
		ValidateTableInfo(-1);	// ��������У��
	}
}

bool32 CIoViewReport::SetHeadInfomationList()
{
	//��ȡҪ��ʾ�ı�ͷ

	int32 iTab;
	T_BlockDesc::E_BlockType eType;

	if ( !TabIsValid(iTab, eType) )
	{
		return false;
	}

	m_bIsUserBlock  = false;

	const T_TabInfo &tab = m_aTabInfomations[iTab];

	//SYS
	if ( T_BlockDesc::EBTPhysical == eType )
	{
		CMarket* pMarket = m_aTabInfomations[iTab].m_Block.m_pMarket;

		if ( NULL == pMarket )
		{
			// ֻ��ϵͳ���Ҫ���
			return false;
		}

		// fangz 1015 for bohai
		if ( BeSpecial() )
		{
			m_eMarketReportType = pMarket->m_MarketInfo.m_eMarketReportType;
			CReportScheme::Instance()->GetReportHeadInfoList(ERTFutureSpot, m_ReportHeadInfoList, m_iFixCol);
		}
		else if (ERHT_CapitalFlow == m_eReportHeadType)
		{
			m_eMarketReportType = ERTCapitalFlow;
			CReportScheme::Instance()->GetReportHeadInfoList(m_eMarketReportType,m_ReportHeadInfoList,m_iFixCol);
		}
		else if (ERHT_MainMasukura == m_eReportHeadType)
		{
			m_eMarketReportType = ERTMainMasukura;
			CReportScheme::Instance()->GetReportHeadInfoList(m_eMarketReportType,m_ReportHeadInfoList,m_iFixCol);
		}
		else if(ERHT_FinancialData == m_eReportHeadType)
		{
			m_eMarketReportType = ERTFinance;  
			CReportScheme::Instance()->GetReportHeadInfoList(m_eMarketReportType,m_ReportHeadInfoList,m_iFixCol);
		}
		else
		{
			m_eMarketReportType = pMarket->m_MarketInfo.m_eMarketReportType;
			CReportScheme::Instance()->GetReportHeadInfoList(m_eMarketReportType,m_ReportHeadInfoList,m_iFixCol);
		}		
	}

	//USER
	if ( T_BlockDesc::EBTUser == eType )
	{
		// ��ѡ�ɵı�ͷ����ع�Ʊ��һ��? ������ع�Ʊ�ı�ͷ���û�Ӱ�쵽��ѡ��,�������߼���ʹ��ϰ��

		CString StrBlockName   = m_aTabInfomations.GetAt(iTab).m_Block.m_StrBlockName;

		T_Block* pBlock = CUserBlockManager::Instance()->GetBlock(StrBlockName);
		if ( NULL != pBlock )
		{
			m_eMarketReportType = pBlock->m_eHeadType;
		}
		else
		{
			m_eMarketReportType = ERTCustom;
		}

		m_bIsUserBlock = true;

		//	
		CReportScheme::Instance()->GetReportHeadInfoList(m_eMarketReportType,m_ReportHeadInfoList,m_iFixCol);
		//m_GridCtrl.SetEditable(true);
	}

	if ( T_BlockDesc::EBTRecentView == eType )
	{
		m_eMarketReportType = ERTCustom;
		CReportScheme::Instance()->GetReportHeadInfoList(m_eMarketReportType,m_ReportHeadInfoList,m_iFixCol);
	}

	//Logic
	if ( T_BlockDesc::EBTLogical == eType )
	{
		// �߼����
		m_eMarketReportType = ERTStockCn;
		CReportScheme::Instance()->GetReportHeadInfoList(m_eMarketReportType,m_ReportHeadInfoList,m_iFixCol);
	}

	// ����ѡ��
	if ( T_BlockDesc::EBTSelect == eType )
	{
		// ����ѡ�ɵ�����ͷ
		if ( m_bShowMoreColumn )
		{
			m_eMarketReportType = ERTSelectStock;
		}
		else
		{
			m_eMarketReportType = ERTMerchSort;
		}

		CReportScheme::Instance()->GetReportHeadInfoList(m_eMarketReportType,m_ReportHeadInfoList,m_iFixCol);
	}

	// ����ǿ��
	if ( T_BlockDesc::EBTMerchSort == eType)
	{
		// ����ѡ�ɵ�����ͷ
		m_eMarketReportType = ERTMerchSort;
		CReportScheme::Instance()->GetReportHeadInfoList(m_eMarketReportType,m_ReportHeadInfoList,m_iFixCol);
	}

	// �¶�����߼����
	if ( T_BlockDesc::EBTBlockLogical == eType )
	{
		CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(tab.m_Block.m_iMarketId);
		if ( NULL != pBlock && ERHT_Count == m_eReportHeadType)
		{
			// �Ըð�������һ����Ʒ���г�Ϊ׼��Ĭ��FuturesCn
			m_eMarketReportType = pBlock->m_blockInfo.m_iType == CBlockInfo::typeFutureClassBlock ? ERTFuturesCn : ERTStockCn;
			if ( pBlock->m_blockInfo.m_aSubMerchs.GetSize() > 0 )
			{
				m_eMarketReportType = pBlock->m_blockInfo.m_aSubMerchs[0]->m_Market.m_MarketInfo.m_eMarketReportType;
			}
		}
		else if (ERHT_CapitalFlow == m_eReportHeadType)
		{
			m_eMarketReportType = ERTCapitalFlow;
		}
		else if (ERHT_MainMasukura == m_eReportHeadType)
		{
			m_eMarketReportType = ERTMainMasukura;
		}
		else if(ERHT_FinancialData == m_eReportHeadType)
		{
			m_eMarketReportType = ERTFinance;    // ��������
		}
		else
		{
			m_eMarketReportType = ERTStockCn;	// ȱʡ - ����֤ȯ
		}
		CReportScheme::Instance()->GetReportHeadInfoList(m_eMarketReportType,m_ReportHeadInfoList,m_iFixCol);
	}

	return true;
}

bool32 CIoViewReport::SetCustomHeadInfomationList()
{
	//��ȡҪ��ʾ�ı�ͷ

	//USER
	{
		// ��ѡ�ɵı�ͷ����ع�Ʊ��һ��? ������ع�Ʊ�ı�ͷ���û�Ӱ�쵽��ѡ��,�������߼���ʹ��ϰ��

		CString StrBlockName   = L"�ҵ���ѡ";

		T_Block* pBlock = CUserBlockManager::Instance()->GetBlock(StrBlockName);
		if ( NULL != pBlock )
		{
			m_eMarketReportType = pBlock->m_eHeadType;
		}
		else
		{
			m_eMarketReportType = ERTCustom;
		}

		//	
		CReportScheme::Instance()->GetReportHeadInfoList(m_eMarketReportType,m_ReportHeadInfoList,m_iFixCol);
	}

	return true;
}

void CIoViewReport::ReSetGridCellDataByIndex()
{	
	if ( m_iCurTab < 0 || m_iCurTab >= m_aTabInfomations.GetSize() )
	{
		return;
	}

	T_TabInfo stTabInfo = m_aTabInfomations.GetAt(m_iCurTab);

	int32 iRow = 1;
	for ( int32 i = stTabInfo.m_iDataIndexBegin; i <= stTabInfo.m_iDataIndexEnd && iRow < m_GridCtrl.GetRowCount(); i++, iRow++ )
	{
		CMerch* pMerch = NULL;

		if ( T_BlockDesc::EBTPhysical == stTabInfo.m_Block.m_eType )
		{
			pMerch = stTabInfo.m_Block.m_pMarket->m_MerchsPtr.GetAt(i);
		}
		else
		{
			pMerch = stTabInfo.m_Block.m_aMerchs.GetAt(i);
		}

		if ( NULL == pMerch )
		{
			continue;
		}

		CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRow, 0);
		if ( NULL == pCell )
		{
			continue;
		}

		pCell->SetData(LPARAM(pMerch));
	}
}

void CIoViewReport::ReSetCustomGridCellDataByIndex(int32 iTab)
{	
	// 	if ( iTab < 0 || iTab >= m_aTabInfomations.GetSize() )
	// 	{
	// 		//ASSERT(0);
	// 		return;
	// 	}

	T_TabInfo stTabInfo = m_tabCustom;//m_aTabInfomations.GetAt(iTab);

	int32 iRow = 1;
	for ( int32 i = stTabInfo.m_iDataIndexBegin; i <= stTabInfo.m_iDataIndexEnd && iRow < m_CustomGridCtrl.GetRowCount(); i++, iRow++ )
	{
		CMerch* pMerch = NULL;

		if ( T_BlockDesc::EBTPhysical == stTabInfo.m_Block.m_eType )
		{
			pMerch = stTabInfo.m_Block.m_pMarket->m_MerchsPtr.GetAt(i);
		}
		else
		{
			pMerch = stTabInfo.m_Block.m_aMerchs.GetAt(i);
		}

		if ( NULL == pMerch )
		{
			continue;
		}

		CGridCellSys* pCell = (CGridCellSys*)m_CustomGridCtrl.GetCell(iRow, 0);
		if ( NULL == pCell )
		{
			continue;
		}

		pCell->SetData(LPARAM(pMerch));
	}
}

void CIoViewReport::ReSetFinanceFlag()
{
	m_eReportHeadType = ERHT_Count;
	TabChange();
}

void CIoViewReport::ReSetGridContent()
{
	CAutoLockWindowUpdate  autoLock(&m_GridCtrl);

	// ����һЩ�������Ϣ(��ѡ���Ǳ��������)
	int32 iSortColumn = m_GridCtrl.GetSortColumn();
	bool32 bSort = m_GridCtrl.GetHeaderSort();

	m_GridCtrl.DeleteNonFixedRows();

	int32 i,iSize,iTab;
	T_BlockDesc::E_BlockType eType;

	if ( !TabIsValid(iTab, eType) )
	{
		//m_GridCtrl.UnlockWindowUpdate();
		return;
	}

	//
	ValidateTableInfo();

	//
	T_BlockDesc BlockDesc = m_aTabInfomations.GetAt(iTab).m_Block;

	// ����ÿһ��
	int32 iDataIndexBegin, iDataIndexEnd;
	if ( !GetCurrentDataIndex(iDataIndexBegin, iDataIndexEnd) )
	{
		//ASSERT(0);	
		//m_GridCtrl.UnlockWindowUpdate();
		return;
	}

	if ( -1 == iDataIndexBegin && -1 == iDataIndexEnd )
	{
		// ��Ʒ����Ϊ 0
		//m_GridCtrl.UnlockWindowUpdate();

		// Ϊ��ӭ����ѡ����������û����ѡ�ɵ�ʱ��ť������ʾ
		if (T_BlockDesc::EBTUser == eType)
		{
			UserBlockPagejump();
		}

		return;
	}

	//
	CArray<CMerch*, CMerch*> aMerchs;
	int32 iMerchNums = iDataIndexEnd - iDataIndexBegin + 1;
	aMerchs.SetSize(iMerchNums);

	if ( T_BlockDesc::EBTPhysical == eType )
	{
		m_MmiRequestSys.m_iMarketId = BlockDesc.m_iMarketId;
		memcpyex(aMerchs.GetData(), BlockDesc.m_pMarket->m_MerchsPtr.GetData() + iDataIndexBegin, sizeof(CMerch*) * iMerchNums);
	}
	else if ( T_BlockDesc::EBTUser == eType )
	{
		memcpyex(aMerchs.GetData(), BlockDesc.m_aMerchs.GetData() + iDataIndexBegin, sizeof(CMerch*) * iMerchNums);		
	}
	else if ( T_BlockDesc::EBTLogical == eType )
	{				
		memcpyex(aMerchs.GetData(), BlockDesc.m_aMerchs.GetData() + iDataIndexBegin, sizeof(CMerch*) * iMerchNums);
	}
	else if ( T_BlockDesc::EBTSelect == eType || T_BlockDesc::EBTMerchSort == eType )
	{
		// ����ѡ��Ŀǰû�ж���
		memcpyex(aMerchs.GetData(), BlockDesc.m_aMerchs.GetData() + iDataIndexBegin, sizeof(CMerch*) * iMerchNums);	
	}
	else if ( T_BlockDesc::EBTRecentView == eType )
	{
		memcpyex(aMerchs.GetData(), BlockDesc.m_aMerchs.GetData() + iDataIndexBegin, sizeof(CMerch*) * iMerchNums);	
	}
	else if ( T_BlockDesc::EBTBlockLogical == eType )
	{
		m_MmiBlockRequestSys.m_iBlockId = BlockDesc.m_iMarketId;
		memcpyex(aMerchs.GetData(), BlockDesc.m_aMerchs.GetData() + iDataIndexBegin, sizeof(CMerch*) * iMerchNums);	
	}

	iSize = aMerchs.GetSize();


	//
	m_GridCtrl.InsertRowBatchBegin();

	for ( i = 0; i < iSize; i ++ )
	{
		// ȷ������Ʒȷʵ���ں� ���������Ŀ
		CMerch* pMerch = aMerchs[i];		
		if (NULL != pMerch)
		{
			// �����Ŀ ( ����һ�����ɼ����ַ� " " ����, �Զ������иߵ�ʱ��, dc ȡ�õĳߴ���쳣)
			m_GridCtrl.InsertRowBatchForOne(L" ", -1);

			int iPos = m_GridCtrl.GetRowCount() - 1;
			CGridCellSys *pCell = (CGridCellSys *)m_GridCtrl.GetCell(iPos, 0);
			ASSERT(NULL != pCell);
			pCell->SetData((LPARAM)pMerch);		
		}
	}

	m_GridCtrl.InsertRowBatchEnd();	

	if ( bSort )
	{
		m_GridCtrl.SetHeaderSort(TRUE);
		m_GridCtrl.SetSortColumn(iSortColumn);		
	}

	m_GridCtrl.AutoSizeRows();	

	ResetGridFont();
	//m_GridCtrl.UnlockWindowUpdate();
}

void CIoViewReport::ReSetCustomGridContent()
{
	CAutoLockWindowUpdate  autoLock(&m_CustomGridCtrl);

	// ����һЩ�������Ϣ(��ѡ���Ǳ��������)
	int32 iSortColumn = m_CustomGridCtrl.GetSortColumn();
	bool32 bSort = m_CustomGridCtrl.GetHeaderSort();

	m_CustomGridCtrl.DeleteNonFixedRows();

	T_BlockDesc::E_BlockType eType = T_BlockDesc::EBTUser;
	E_TabInfoType eTabInfoType = ETITEntity;
	T_BlockDesc::E_BlockType eBlockType = T_BlockDesc::EBTUser;

	int32 iIndex = 1;
	CArray<T_Block, T_Block&> aBlocks;
	CUserBlockManager::Instance()->GetBlocks(aBlocks);

	if ( aBlocks.GetSize() <= 0 )
	{
		return;
	}

	if ( iIndex < 0 || iIndex >= aBlocks.GetSize() )
	{
		//ASSERT(0);
		iIndex = 0;
	}

	//
	T_Block Block = aBlocks[iIndex];

	T_TabInfo stTabNew;
	stTabNew.m_eTabType				= eTabInfoType;
	stTabNew.m_StrShowName			= Block.m_StrName;	
	stTabNew.m_Block.m_eType		= eBlockType;
	stTabNew.m_Block.m_iMarketId	= 0;
	stTabNew.m_Block.m_StrBlockName	= Block.m_StrName;
	stTabNew.m_Block.m_aMerchs.Copy(Block.m_aMerchs);

	// �û����
	if ( NULL != CUserBlockManager::Instance()->GetBlock(Block.m_StrName))
	{
		if ( CUserBlockManager::Instance()->GetMerchsInBlock(Block.m_StrName, Block.m_aMerchs) )
		{
			if ( Block.m_aMerchs.GetSize() > 0 )
			{
				if ( stTabNew.m_iDataIndexBegin < 0 || stTabNew.m_iDataIndexBegin >= Block.m_aMerchs.GetSize() )
				{
					stTabNew.m_iDataIndexBegin = 0;
				}

				if ( stTabNew.m_iDataIndexEnd < 0 || stTabNew.m_iDataIndexEnd >= Block.m_aMerchs.GetSize() )
				{
					stTabNew.m_iDataIndexEnd = Block.m_aMerchs.GetSize() - 1;
				}
			}
			else
			{
				stTabNew.m_iDataIndexBegin = -1;
				stTabNew.m_iDataIndexEnd	= -1;
			}					
		}
	}						

	//
	T_BlockDesc BlockDesc = stTabNew.m_Block;
	m_blockCustom = BlockDesc;

	// ����ÿһ��
	int32 iDataIndexBegin, iDataIndexEnd;
	iDataIndexBegin = stTabNew.m_iDataIndexBegin;
	iDataIndexEnd	= stTabNew.m_iDataIndexEnd;
	m_tabCustom = stTabNew;

	if ( -1 == iDataIndexBegin && -1 == iDataIndexEnd )
	{
		// ��Ʒ����Ϊ 0
		//m_CustomGridCtrl.UnlockWindowUpdate();
		return;
	}

	//
	CArray<CMerch*, CMerch*> aMerchs;
	int32 iMerchNums = iDataIndexEnd - iDataIndexBegin + 1;
	aMerchs.SetSize(iMerchNums);

	if ( T_BlockDesc::EBTPhysical == eType )
	{
		m_MmiRequestSys.m_iMarketId = BlockDesc.m_iMarketId;
		memcpyex(aMerchs.GetData(), BlockDesc.m_pMarket->m_MerchsPtr.GetData() + iDataIndexBegin, sizeof(CMerch*) * iMerchNums);
	}
	else if ( T_BlockDesc::EBTUser == eType )
	{
		memcpyex(aMerchs.GetData(), BlockDesc.m_aMerchs.GetData() + iDataIndexBegin, sizeof(CMerch*) * iMerchNums);		
	}
	else if ( T_BlockDesc::EBTLogical == eType )
	{				
		memcpyex(aMerchs.GetData(), BlockDesc.m_aMerchs.GetData() + iDataIndexBegin, sizeof(CMerch*) * iMerchNums);
	}
	else if ( T_BlockDesc::EBTSelect == eType || T_BlockDesc::EBTMerchSort == eType )
	{
		// ����ѡ��Ŀǰû�ж���
		memcpyex(aMerchs.GetData(), BlockDesc.m_aMerchs.GetData() + iDataIndexBegin, sizeof(CMerch*) * iMerchNums);	
	}
	else if ( T_BlockDesc::EBTRecentView == eType )
	{
		memcpyex(aMerchs.GetData(), BlockDesc.m_aMerchs.GetData() + iDataIndexBegin, sizeof(CMerch*) * iMerchNums);	
	}
	else if ( T_BlockDesc::EBTBlockLogical == eType )
	{
		m_MmiBlockRequestSys.m_iBlockId = BlockDesc.m_iMarketId;
		memcpyex(aMerchs.GetData(), BlockDesc.m_aMerchs.GetData() + iDataIndexBegin, sizeof(CMerch*) * iMerchNums);	
	}

	int32 iSize = 0;
	iSize = aMerchs.GetSize();

	//
	m_CustomGridCtrl.InsertRowBatchBegin();

	for (int32 i = 0; i < iSize; i ++ )
	{
		// ȷ������Ʒȷʵ���ں� ���������Ŀ
		CMerch* pMerch = aMerchs[i];

		if (NULL != pMerch)
		{
			// �����Ŀ ( ����һ�����ɼ����ַ� " " ����, �Զ������иߵ�ʱ��, dc ȡ�õĳߴ���쳣)
			m_CustomGridCtrl.InsertRowBatchForOne(L" ", -1);
			int iPos = m_CustomGridCtrl.GetRowCount() - 1;

			// �����ж�Ӧ����Ʒָ��
			CGridCellSys *pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iPos, 0);
			ASSERT(NULL != pCell);
			pCell->SetData((LPARAM)pMerch);			
		}
	}

	m_CustomGridCtrl.InsertRowBatchEnd();	

	if ( bSort )
	{
		m_CustomGridCtrl.SetHeaderSort(TRUE);
		m_CustomGridCtrl.SetSortColumn(iSortColumn);		
	}

	m_CustomGridCtrl.AutoSizeRows();	
}

void CIoViewReport::InitialCustomMerch()
{
	ASSERT(NULL != CReportScheme::Instance());

	m_CustomGridCtrl.EndEditing();
	m_CustomGridCtrl.SetEditable(false);
	m_CustomGridCtrl.SetSortColumn(-1);	// �����κ�������

	m_CustomGridCtrl.GetSelectedCellRange();	// ���浱ǰѡ��

	//
	m_CustomGridCtrl.SetScrollPos32(SB_VERT, 0);
	m_CustomGridCtrl.SetScrollPos32(SB_HORZ, 0);

	// ���ù�����λ��:
	m_XSBHorzCustom.ShowWindow(SW_HIDE);

	m_TimeLast = CGmtTime(0);
	m_bRequestViewSort = false;

	// ���	
	m_ReportHeadInfoList.RemoveAll();

	int32 i,iSize,iTab=0;

	iSize = m_aTabInfomations.GetSize();
	for (i=0; i<iSize; i++)
	{
		if (m_aTabInfomations[i].m_Block.m_eType == T_BlockDesc::EBTUser)
		{
			iTab = i;
			break;
		}
	}

	T_BlockDesc BlockDesc = m_aTabInfomations.GetAt(iTab).m_Block;

	// �õ���ͷ��Ϣ
	if ( !SetCustomHeadInfomationList() )
	{
		return;
	}

	//�趨��ͷ
	m_CustomGridCtrl.SetFixedColumnCount(m_iFixCol);
	iSize = m_ReportHeadInfoList.GetSize();

	//
	bool32 bFoundForceField = false;

	CStringArray HeaderNames;
	CArray<int32,int32> HeaderFields;	
	CArray<int32,int32> HeaderWidths;

	// ת��
	for ( i = 0; i < iSize; i++ )
	{
		CString StrText = m_ReportHeadInfoList[i].m_StrHeadNameCn;
		ASSERT(StrText.GetLength() > 0 && m_ReportHeadInfoList[i].m_iHeadWidth > 0);

		HeaderNames.Add(StrText);
		HeaderWidths.Add(m_ReportHeadInfoList[i].m_iHeadWidth);

		bool32 bTrans;
		CReportScheme::E_ReportHeader eHeadType = CReportScheme::Instance()->GetReportHeaderEType(StrText);
		E_MerchReportField eField = ReportHeader2MerchReportField(eHeadType, bTrans);

		if ( bTrans )
		{
			HeaderFields.Add((int32)eField);
			if ( BlockDesc.m_iFieldAdd == eField )
			{
				bFoundForceField = true;
			}
		}
		else
		{
			HeaderFields.Add(-1);
		}

		// ����ѡ�ɵ���������ǿ����ʾ���� xl 1220
		// ���г�����ָ�������г���Ʒ�Ķ�����һ�Ž�����ʾ
		bool32 bBuySellPriceExchange = false;
		{
			if ( T_BlockDesc::EBTPhysical == BlockDesc.m_eType )
			{
				bBuySellPriceExchange = IsBuySellPriceExchangeMarket(BlockDesc.m_iMarketId);
			}
			else
			{
				for ( int32 i=0; i < BlockDesc.m_aMerchs.GetSize() ; ++i )
				{
					if ( IsBuySellPriceExchangeMerch(BlockDesc.m_aMerchs[i]) )
					{
						bBuySellPriceExchange = true;
						break;
					}
				}
			}
		}

		if ( bBuySellPriceExchange )
		{
			if ( CReportScheme::ERHBuyPrice == eHeadType )
			{
				HeaderNames[ HeaderNames.GetUpperBound() ] = CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHSellPrice);
			}
			else if ( CReportScheme::ERHSellPrice == eHeadType )
			{
				HeaderNames[ HeaderNames.GetUpperBound() ] = CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHBuyPrice);
			}
		}
	}

	if ( -1 != BlockDesc.m_iFieldAdd && !bFoundForceField )
	{
		bool32 bTrans = false;
		CReportScheme::E_ReportHeader eType = MerchReportField2ReportHeader((E_MerchReportField)BlockDesc.m_iFieldAdd, bTrans);

		if ( bTrans )
		{
			// 
			BlockDesc.m_iTypeAdd = eType;
			UpdateLocalBlock(i, BlockDesc);

			CString StrText = CReportScheme::GetReportHeaderCnName(eType);
			HeaderNames.Add(StrText);
			HeaderFields.Add(BlockDesc.m_iFieldAdd);
			//HeaderWidths.Add(DEFUALTWIDTH);
			HeaderWidths.Add(CReportScheme::GetReportColDefaultWidth(eType));
		}
	} 

	// ���ñ��̶���
	m_CustomGridCtrl.SetFixedRowCount(1);
	iSize = HeaderNames.GetSize();
	m_CustomGridCtrl.SetColumnCount(iSize);

	// �����п�	
	for (i = 0; i < iSize; i++)
	{
		CGridCellSys * pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(0, i);
		DWORD dwFmt = DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX;
		pCell->SetFormat(dwFmt);
		CString StrText = HeaderNames.GetAt(i);

		// ���۱�����óɽ�������ɫ
		pCell->SetDefaultTextColor(ESCVolume);

		pCell->SetText(StrText);
		pCell->SetData(HeaderFields.GetAt(i));
		m_CustomGridCtrl.SetColumnWidth(i, HeaderWidths.GetAt(i));
		CReportScheme::E_ReportHeader eType = CReportScheme::Instance()->GetReportHeaderEType(StrText);
		if ( CReportScheme::ERHMerchName == eType
			|| CReportScheme::ERHMerchCode == eType
			|| CReportScheme::ERHRowNo == eType )
		{
			pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		}

		if (CReportScheme::ERHShortLineSelect == eType
			|| CReportScheme::ERHMidLineSelect == eType
			|| CReportScheme::ERHZixuan == eType)
		{
			CGridCellImage *pImgCell = (CGridCellImage *)m_CustomGridCtrl.GetCell(0, i);
			pImgCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		}
	}

	// ��������
	ReSetCustomGridContent();

	// ��������
	if ( !RequestViewDataSort() )
	{
		// ����ǰ��ʵʱ����
		RequestViewDataCurrentVisibleRowAsync();
	}

	// �����и�
	SetRowHeightAccordingFont();

	if (m_bAutoSizeColFromXml)
	{
		m_bSuccessAutoSizeColXml = false;
	}



	// ���±������
	UpdateCustomTableContent(TRUE, NULL, false);

	// ���ÿɱ༭��
	SetEditAbleCol();

	//	m_XSBHorzCustom.Invalidate();
	//	m_XSBHorzCustom.ShowWindow(SW_SHOW);

	if ( m_CustomGridCtrl.GetRowCount() > m_CustomGridCtrl.GetFixedRowCount() )
	{
		int32 iFixed = m_CustomGridCtrl.GetFixedRowCount();
		CCellRange cellFirst(iFixed, 0, iFixed, m_CustomGridCtrl.GetColumnCount() - 1);
		//m_CustomGridCtrl.SetSelectedRange(cellFirst);
		m_CustomGridCtrl.SetFocusCell(iFixed, m_CustomGridCtrl.GetFixedColumnCount());
		m_CustomGridCtrl.SetSelectedSingleRow(iFixed);
	}

	//
	RequestViewDataCurrentVisibleRowAsync();
	//
	//m_GridCtrl.SetFocus();

	// ������ǽ����г�, ����һ�¿��ֽ��׽���
	// 	int32 iTradeMarket = ((CGGTongApp*)AfxGetApp())->m_pConfigInfo->m_iTradeMarketID;
	// 	if (-1 != iTradeMarket && iTradeMarket != BlockDesc.m_iMarketId)
	// 	{
	// 		pMainFrm->OnQuickTrade(3, NULL, CReportScheme::ERHCount);
	// 	}	
}


void CIoViewReport::TabChange()
{
	ASSERT(NULL != CReportScheme::Instance());

	m_GridCtrl.EndEditing();
	m_GridCtrl.SetEditable(false);
	m_GridCtrl.SetSortColumn(-1);		// �����κ�������
	m_GridCtrl.GetSelectedCellRange();	// ���浱ǰѡ��

	//
	m_GridCtrl.SetScrollPos32(SB_VERT, 0);
	m_GridCtrl.SetScrollPos32(SB_HORZ, 0);

	// ��ȡ��ǰTab
	if ( m_iCurTab < 0 || m_iCurTab >= m_aTabInfomations.GetSize() )
	{
		return;
	}

	// ���д򿪲����ؾ����˺���~~
	// �򿪲�����������
	if ( m_aTabInfomations[m_iCurTab].m_iDataIndexBegin != 0 )
	{
		m_aTabInfomations[m_iCurTab].m_iDataIndexBegin = 0;
		SetTabParams(m_iCurTab);
	}

	// ���ù�����λ��:
	m_XSBHorz.ShowWindow(SW_HIDE);
	m_XSBVertical.ShowWindow(SW_HIDE);

	m_TimeLast = CGmtTime(0);
	m_bRequestViewSort = false;


	if (m_aTabInfomations.GetSize() <= 0)
	{
		return;
	}

	
	ReCalcGridSizeColFromXml();




//	if ( eType == T_BlockDesc::EBTSelect )
//	{
//		RequestViewData();	// ѡ����Ҫ����ѡ������
//	}

	

	// ��������
	ReSetGridContent();

	// ����������
	// 	bool32 bAlignEnd = m_aTabInfomations.GetAt(m_iCurTab).m_bAlignDataEnd;
	// 	if ( bAlignEnd )
	// 	{ // ����β��
	// 		CCellRange cellVisible = m_GridCtrl.GetVisibleNonFixedCellRange();
	// 		m_GridCtrl.EnsureVisible(m_GridCtrl.GetRowCount() - 1, cellVisible.GetMinCol());			
	// 	}

	// ��������
	if ( !RequestViewDataSort() )
	{
		// ����ǰ��ʵʱ����
		RequestViewDataCurrentVisibleRowAsync();
	}

	// �����и�
	SetRowHeightAccordingFont();

	if (m_bAutoSizeColFromXml)
	{
		m_bSuccessAutoSizeColXml = false;
	}

	AutoSizeColFromXml();

	// ���±������
	UpdateTableContent(TRUE, NULL, false);

	// ���ÿɱ༭��
	SetEditAbleCol();


	m_XSBHorz.Invalidate();
	m_XSBHorz.ShowWindow(SW_SHOW);

	JudgeScrollVerVisbile();

	if ( m_GridCtrl.GetRowCount() > m_GridCtrl.GetFixedRowCount() )
	{
		int32 iFixed = m_GridCtrl.GetFixedRowCount();
		CCellRange cellFirst(iFixed, 0, iFixed, m_GridCtrl.GetColumnCount() - 1);
		m_GridCtrl.SetFocusCell(iFixed, m_GridCtrl.GetFixedColumnCount());
		m_GridCtrl.SetSelectedSingleRow(iFixed);
	}


	//RequestViewDataCurrentVisibleRowAsync();
	
}

void CIoViewReport::ReCreateTabWnd()
{
	ValidateTableInfo();
	m_GuiTabWnd.DeleteAll();

	CString StrBlock;
	int32 i,iSize = m_aTabInfomations.GetSize();
	T_BlockDesc::E_BlockType eType;


	for ( i = 0; i < iSize; i ++ )
	{
		T_BlockDesc BlockDesc = m_aTabInfomations.GetAt(i).m_Block;

		eType    = BlockDesc.m_eType;

		if ( m_aTabInfomations.GetAt(i).m_StrShowName.GetLength() <= 0 )
		{
			StrBlock = BlockDesc.m_StrBlockName;
		}
		else
		{
			StrBlock = m_aTabInfomations.GetAt(i).m_StrShowName;
		}

		//
		if ( T_BlockDesc::EBTPhysical == eType )
		{			
			if ( NULL != BlockDesc.m_pMarket )
			{				
				BlockDesc.m_StrBlockName = BlockDesc.m_pMarket->m_MarketInfo.m_StrCnName;
				UpdateLocalBlock(i, BlockDesc);
			}
		}

		m_GuiTabWnd.Addtab(StrBlock, StrBlock, StrBlock);

		if ( i >= (KiFixTabCounts - 1))
		{
			break;
		}			 
	}

	if ( m_iCurTab < 0 )
	{
		m_iCurTab = 0;
	}
	else if ( m_iCurTab >= m_aTabInfomations.GetSize() )
	{
		m_iCurTab = m_aTabInfomations.GetSize() - 1;
	}

	m_bLockSelChange = true;
	SetTab(m_iCurTab);	
	m_bLockSelChange = false;

	if (m_bShowCustomGrid)
	{
		InitialCustomMerch();
	}

	/*TabChange();*/
}

bool32 CIoViewReport::GetCurrentDataIndex(int32& iIndexBegin, int32& iIndexEnd)
{
	iIndexBegin = 0;
	iIndexEnd	= 0;

	//
	if ( m_iCurTab < 0 || m_iCurTab >= m_aTabInfomations.GetSize() )
	{
		return false;
	}

	//
	iIndexBegin = m_aTabInfomations[m_iCurTab].m_iDataIndexBegin;
	iIndexEnd	= m_aTabInfomations[m_iCurTab].m_iDataIndexEnd;

	return true;
}

void CIoViewReport::UpdateLocalBlock(int32 iIndex, const T_BlockDesc& Block)
{
	T_TabInfo* pTabInfo = m_aTabInfomations.GetData();

	if ( NULL == pTabInfo || iIndex >= m_aTabInfomations.GetSize() )
	{
		return;
	}

	pTabInfo[iIndex].m_Block = Block;
}

void CIoViewReport::UpdateLocalXScrollPos(int32 iIndex, int32 iPos)
{
	T_TabInfo* pTabInfo = m_aTabInfomations.GetData();

	if ( NULL == pTabInfo || iIndex >= m_aTabInfomations.GetSize() )
	{		
		return;
	}

	pTabInfo[iIndex].m_iXScrollPos = iPos;
}

void CIoViewReport::UpdateLocalXScrollPos2(int32 iPos)
{
	m_tabCustom.m_iXScrollPos = iPos;
}

void CIoViewReport::UpdateLocalDataIndex2(int32 iBegin, int32 iEnd)
{
	m_tabCustom.m_iDataIndexBegin = iBegin;
	m_tabCustom.m_iDataIndexEnd   = iEnd;

	if ( IsWindow(m_CustomGridCtrl.m_hWnd) && m_CustomGridCtrl.IsWindowVisible() )
	{ 
		// ��¼��Ч�����ص�������
		int32 iMerchCount = 0;
		if ( NULL != m_tabCustom.m_Block.m_pMarket )
		{
			iMerchCount = m_tabCustom.m_Block.m_pMarket->m_MerchsPtr.GetSize();
		}
		else
		{
			iMerchCount = m_tabCustom.m_Block.m_aMerchs.GetSize();
		}
		m_tabCustom.m_bAlignDataEnd = iMerchCount -1 <= iEnd;
	}
}

void CIoViewReport::UpdateLocalDataIndex(int32 iIndex, int32 iBegin, int32 iEnd)
{
	T_TabInfo* pTabInfo = m_aTabInfomations.GetData();

	if ( NULL == pTabInfo || iIndex >= m_aTabInfomations.GetSize() )
	{
		return;
	}

	pTabInfo[iIndex].m_iDataIndexBegin = iBegin;
	pTabInfo[iIndex].m_iDataIndexEnd   = iEnd;

	if ( IsWindow(m_GridCtrl.m_hWnd) && m_GridCtrl.IsWindowVisible() )
	{ // ��¼��Ч�����ص�������
		int32 iMerchCount = 0;
		if ( NULL != pTabInfo[iIndex].m_Block.m_pMarket )
		{
			iMerchCount = pTabInfo[iIndex].m_Block.m_pMarket->m_MerchsPtr.GetSize();
		}
		else
		{
			iMerchCount = pTabInfo[iIndex].m_Block.m_aMerchs.GetSize();
		}
		pTabInfo[iIndex].m_bAlignDataEnd = iMerchCount -1 <= iEnd;
	}
}

void CIoViewReport::ReSetGridCellDataByMerchs(const CArray<CMerch*, CMerch*>& aMerchsSrc)
{
	int32 iTab;
	T_BlockDesc::E_BlockType eType;
	if ( !TabIsValid(iTab, eType) )
	{
		return;
	}
	const T_TabInfo &tab = m_aTabInfomations[iTab];
	MerchArray aMerchs;
	// �������������Զ�������, ����Ʒָ�븽��ÿ�е�һ�������
	{
		// �����ȡ��Ҫ����������
		aMerchs.Copy(aMerchsSrc);
		if ( aMerchs.GetSize() > tab.m_iDataIndexEnd-tab.m_iDataIndexBegin+1 )
		{
			aMerchs.SetSize(tab.m_iDataIndexEnd-tab.m_iDataIndexBegin+1);
		}
		m_GridCtrl.SetRowCount(tab.m_iDataIndexEnd-tab.m_iDataIndexBegin+1 + m_GridCtrl.GetFixedRowCount());	// ��Χ�Ƕ��پ��Ƕ�������, �п���û����ô���������¿���
	}

	CGridCellSys* pCell = NULL;
	int32 iCurMerchSize = aMerchs.GetSize();
	int32 iCurRowCount = m_GridCtrl.GetRowCount()-1;
	const int32 iMinSize = min(iCurMerchSize, iCurRowCount);

	if (0 == iMinSize)
	{
		m_GridCtrl.DeleteNonFixedRows();
	}

	for ( int32 i = 0; i < iMinSize; i++ )
	{
		pCell = (CGridCellSys *)m_GridCtrl.GetCell(i+1, 0);
		if ( NULL == pCell )
		{
			//ASSERT(0);
			continue;
		}

		CMerch* pMerch = aMerchs.GetAt(i);
		if ( NULL == pMerch )
		{
			//ASSERT(0);
			continue;
		}

		pCell->SetData((LPARAM)pMerch);
	}	

	ResetGridFont();
}

void CIoViewReport::ReSetCustomGridCellDataByMerchs(const CArray<CMerch*, CMerch*>& aMerchsSrc)
{
	const T_TabInfo &tab = m_tabCustom;
	MerchArray aMerchs;
	// �������������Զ�������, ����Ʒָ�븽��ÿ�е�һ�������
	{
		// �����ȡ��Ҫ����������
		aMerchs.Copy(aMerchsSrc);
		if ( aMerchs.GetSize() > tab.m_iDataIndexEnd-tab.m_iDataIndexBegin+1 )
		{
			aMerchs.SetSize(tab.m_iDataIndexEnd-tab.m_iDataIndexBegin+1);
		}
		m_CustomGridCtrl.SetRowCount(tab.m_iDataIndexEnd-tab.m_iDataIndexBegin+1 + m_CustomGridCtrl.GetFixedRowCount());	// ��Χ�Ƕ��پ��Ƕ�������, �п���û����ô���������¿���
	}

	CGridCellSys* pCell = NULL;
	int32 iTmpMerchSize = aMerchs.GetSize();
	int32 iRowCount = m_CustomGridCtrl.GetRowCount()-1;
	const int32 iMinSize = min(iTmpMerchSize, iRowCount);

	for ( int32 i = 0; i < iMinSize; i++ )
	{
		pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(i+1, 0);
		if ( NULL == pCell )
		{
			//ASSERT(0);
			continue;
		}

		CMerch* pMerch = aMerchs.GetAt(i);
		if ( NULL == pMerch )
		{
			//ASSERT(0);
			continue;
		}

		pCell->SetData((LPARAM)pMerch);
	}	
}

void CIoViewReport::UpdateCustomTableContent( BOOL bUpdateAllVisible, IN CMerch *pMerch, bool32 bBlink)
{
	if (!IsWindowVisible())
	{
		return;
	}
	// 	if ( m_bDelByBiSplitTrack )
	// 	{		
	// 		LockRedraw();
	// 		return;
	// 	}
	// 	else
	// 	{
	// 		UnLockRedraw();
	// 	}
	// 
	// 	//
	// 	if ( GetParentGGTongViewDragFlag() )
	// 	{
	// 		LockRedraw();
	// 		return;
	// 	}
	// 	else
	// 	{
	// 		UnLockRedraw();
	// 	}
	SetCustomHeadInfomationList();

	int32 i,iRow;
	// ��֤�Ϸ���

	T_BlockDesc::E_BlockType eType = T_BlockDesc::EBTUser;

	// �õ����
	T_BlockDesc BlockDesc = m_tabCustom.m_Block;

	// �Ƿ����ȫ����
	int32  iUpdateOneRow = -1;
	bool32 bUpdateOneRow = false;
	CMerch* pMerchUpdate = NULL;

	if ( !bUpdateAllVisible )
	{
		pMerchUpdate = pMerch;
		if ( NULL == pMerchUpdate )
		{
			return;
		}
	}

	for (iRow = 1; iRow < m_CustomGridCtrl.GetRowCount(); iRow++)
	{
		if ( !bUpdateAllVisible )
		{
			if ( bUpdateOneRow )
			{
				break;
			}
		}

		CGridCellSys *pFirstCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, 0);
		ASSERT(NULL != pFirstCell);

		//
		CMerch *pMerchRow = (CMerch *)pFirstCell->GetData();
		if ( NULL == pMerchRow )
		{
			//ASSERT(0);
			continue;
		}

		//
		if ( !bUpdateAllVisible )
		{
			if ( pMerchRow != pMerchUpdate )
			{
				continue;
			}
		}

		bUpdateOneRow = true;
		iUpdateOneRow = iRow;

		// �õ����¼۽ṹ
		CRealtimePrice RealtimePrice;
		if (NULL != pMerchRow && NULL != pMerchRow->m_pRealtimePrice)
		{
			RealtimePrice = *pMerchRow->m_pRealtimePrice;
		}

		float fPrevReferPrice = RealtimePrice.m_fPricePrevClose;
		if (ERTFuturesCn == pMerchRow->m_Market.m_MarketInfo.m_eMarketReportType)	// �����ڻ�ʹ���������Ϊ�ο��۸�
		{
			fPrevReferPrice = RealtimePrice.m_fPricePrevAvg;
		}

		// 
		CMerchExtendData ExtendData;
		if ( NULL != pMerchRow )
		{
			if ( NULL != pMerchRow->m_pMerchExtendData )
			{				
				ExtendData = *pMerchRow->m_pMerchExtendData;
			}					
		}	

		// ��������
		CFinanceData	FinanceData;
		if ( NULL != pMerchRow->m_pFinanceData )
		{
			FinanceData = *pMerchRow->m_pFinanceData;
		}

		// ѡ�ɱ�־
		bool32 bUseMerchIndex = false;
		MerchIndexMap::const_iterator it = m_mapMerchIndex.find(pMerchRow);
		T_RespMerchIndex RespMerchIndex;
		if ( it != m_mapMerchIndex.end() )
		{
			bUseMerchIndex = true;
			RespMerchIndex = it->second;
		}

		CGridCellSys* pCell;
		CGridCellSymbol *pCellSymbol;

		// ...fangz0926 ���� �ɽ�����������ɫ
		COLORREF ClrCurHold = GetIoViewColor(ESCVolume2);

		float fVal = 0.0;

		bool32 bIsFuture = CReportScheme::IsFuture(GetMerchKind(pMerchRow));
		bool32 bShowTranslate = !bIsFuture;

		// �����б�ͷ��ʾ�����ݣ� ��ʾ���������
		for (int32 iCol = 0; iCol < m_CustomGridCtrl.GetColumnCount(); iCol++)
		{
			// ��ͷ��( ���, ����, ����....)
			CReportScheme::E_ReportHeader ERHType;

			if ( -1 != BlockDesc.m_iTypeAdd && iCol == m_CustomGridCtrl.GetColumnCount() -1 )
			{
				ERHType = (CReportScheme::E_ReportHeader)BlockDesc.m_iTypeAdd;
			}
			else
			{
				ERHType = CReportScheme::Instance()->GetReportHeaderEType(m_ReportHeadInfoList[iCol].m_StrHeadNameCn);
			}

			// ��Ʒ���� (�����ڻ�, ����֤ȯ, ���...)
			E_ReportType eMerchKind = GetMerchKind(pMerchRow);	

			pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);

			//
			switch ( ERHType )
			{
			case CReportScheme::ERHRowNo:
				{
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrRowNo;
					StrRowNo.Format(L"%d", m_tabCustom.m_iDataIndexBegin + iRow);
					pCell->SetCompareFloat((float)iRow);
					pCell->SetTextClr(GetIoViewColor(ESCText));
					pCell->SetText(StrRowNo);
				}
				break;
			case CReportScheme::ERHMerchCode:
				{
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCell->SetTextClr(GetIoViewColor(ESCVolume));
					pCell->SetText(pMerchRow->m_MerchInfo.m_StrMerchCode);
					//					pCell->SetState(pCell->GetState()& ~GVIS_READONLY);
				}
				break;
			case CReportScheme::ERHMerchName:
				{

					/*					COLORREF clr;*/
					//
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);		
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCell->SetTextClr(COLO_NO_NAME_CODE);

					T_MarkData MarkData;
					GV_DRAWMARK_ITEM markItem;
					if ( CMarkManager::Instance().QueryMark(pMerchRow, MarkData)
						&& InitDrawMarkItem(pMerchRow, markItem) )
					{
						// ������ʾ�ִ�
						if ( EMT_Text ==  MarkData.m_eType )
						{
							pCell->SetState( pCell->GetState()|GVIS_SHOWTIPS );
							pCell->SetTipText(MarkData.m_StrTip);
							CString Str;
							Str.Format(_T("%s(%s) CTRL+Q�޸ı������")
								, pMerchRow->m_MerchInfo.m_StrMerchCnName.GetBuffer()
								, pMerchRow->m_MerchInfo.m_StrMerchCode.GetBuffer());
							pCell->SetTipTitle(Str);
						}
						else
						{
							pCell->SetState( pCell->GetState()&(~GVIS_SHOWTIPS) );
						}
					}
					else
					{
						markItem.eDrawMarkFlag = EDMF_None;
						pCell->SetState( pCell->GetState()&(~GVIS_SHOWTIPS) );
					}
					pCell->SetDrawMarkItem(markItem);

					// 
					// 					if( T_BlockDesc::EBTUser != eType)
					// 					{
					// 						T_Block* pBlock = CUserBlockManager::Instance()->GetBlock(pMerchRow);
					// 						if ( NULL != pBlock )
					// 						{
					// 							clr = pBlock->m_clrBlock;
					// 							
					// 							// ��ѡ������Ʒ������ɫ������ʾ:
					// 							if ( COLORNOTCOUSTOM == clr )
					// 							{
					// 								clr = GetIoViewColor(ESCText);								
					// 							}	
					// 
					// 							pCell->SetTextClr(clr);	
					// 						}						
					// 					}

					if (pMerchRow->m_StrMerchFakeName.IsEmpty())
					{
						pCell->SetText(pMerchRow->m_MerchInfo.m_StrMerchCnName);
					}
					else
					{
						pCell->SetText(pMerchRow->m_StrMerchFakeName);
					}
				}
				break;
			case CReportScheme::ERHPricePrevClose:			
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPricePrevClose = L"";
					StrPricePrevClose = Float2SymbolString(RealtimePrice.m_fPricePrevClose, RealtimePrice.m_fPricePrevClose, pMerchRow->m_MerchInfo.m_iSaveDec);	// ������ʾ��ƽ
					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPricePrevClose);
					}
					else
					{
						pCellSymbol->SetText(StrPricePrevClose);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_fPricePrevClose);
				}
				break;
			case CReportScheme::ERHPricePrevBalance:
				{
					// �����
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					if (CReportScheme::IsFuture(eMerchKind))
					{
						BUY_SELL_PLATE nType = CConfigInfo::Instance()->m_nBuySellPlate;
						if ((ERTMony==eMerchKind) && (YESTERDAY_CLOSE==nType))// ����ǡ������̡�,�������ó�������
						{
							CString StrPriceClose = Float2SymbolString(RealtimePrice.m_fPricePrevClose, RealtimePrice.m_fPricePrevClose, pMerchRow->m_MerchInfo.m_iSaveDec);

							if (bBlink)
							{
								pCellSymbol->SetBlinkText(StrPriceClose);
							}
							else
							{
								pCellSymbol->SetText(StrPriceClose);
							}

							pCellSymbol->SetCompareFloat(RealtimePrice.m_fPricePrevClose);	
						}
						else
						{
							CString StrPricePrevBlance = Float2SymbolString(RealtimePrice.m_fPricePrevAvg, RealtimePrice.m_fPricePrevAvg, pMerchRow->m_MerchInfo.m_iSaveDec);

							if (bBlink)
							{
								pCellSymbol->SetBlinkText(StrPricePrevBlance);
							}
							else
							{
								pCellSymbol->SetText(StrPricePrevBlance);
							}

							pCellSymbol->SetCompareFloat(RealtimePrice.m_fPricePrevAvg);	
						}		
					}
					else
					{
						pCellSymbol->SetText(L"--");
					}					
				}
				break;
			case CReportScheme::ERHPriceOpen:
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceOpen = L"";
					StrPriceOpen = Float2SymbolString(RealtimePrice.m_fPriceOpen, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceOpen);
					}
					else
					{
						pCellSymbol->SetText(StrPriceOpen);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_fPriceOpen);
				}
				break;
			case CReportScheme::ERHPriceNew:
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceNew = L"";
					StrPriceNew = Float2SymbolString(RealtimePrice.m_fPriceNew, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceNew);						
					}
					else
					{
						pCellSymbol->SetText(StrPriceNew);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_fPriceNew);
				}
				break;
			case CReportScheme::ERHPriceBalance:			// �����: ����ľ���
				{					
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceAvg = L"";
					StrPriceAvg = Float2SymbolString(RealtimePrice.m_fPriceAvg, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceAvg);
					}
					else
					{
						pCellSymbol->SetText(StrPriceAvg);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_fPriceAvg);
				}
				break;
			case CReportScheme::ERHPriceHigh:
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceHigh = L"";
					StrPriceHigh = Float2SymbolString(RealtimePrice.m_fPriceHigh, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);	

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceHigh);
					}
					else
					{
						pCellSymbol->SetText(StrPriceHigh);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_fPriceHigh);
				}
				break;
			case CReportScheme::ERHPriceLow:
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceLow = L"";
					StrPriceLow = Float2SymbolString(RealtimePrice.m_fPriceLow, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceLow);
					}
					else
					{
						pCellSymbol->SetText(StrPriceLow);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_fPriceLow);
				}
				break;
			case CReportScheme::ERHPriceSell5:
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceNow = L"";
					StrPriceNow = Float2SymbolString(RealtimePrice.m_astSellPrices[4].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceNow);						
					}
					else
					{
						pCellSymbol->SetText(StrPriceNow);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_astSellPrices[4].m_fPrice);
				}
				break;
			case CReportScheme::ERHPriceSell4:
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceNow = L"";
					StrPriceNow = Float2SymbolString(RealtimePrice.m_astSellPrices[3].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceNow);						
					}
					else
					{
						pCellSymbol->SetText(StrPriceNow);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_astSellPrices[3].m_fPrice);
				}
				break;
			case CReportScheme::ERHPriceSell3:
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceNow = L"";
					StrPriceNow = Float2SymbolString(RealtimePrice.m_astSellPrices[2].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceNow);						
					}
					else
					{
						pCellSymbol->SetText(StrPriceNow);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_astSellPrices[2].m_fPrice);
				}
				break;
			case CReportScheme::ERHPriceSell2:
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceNow = L"";
					StrPriceNow = Float2SymbolString(RealtimePrice.m_astSellPrices[1].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceNow);						
					}
					else
					{
						pCellSymbol->SetText(StrPriceNow);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_astSellPrices[1].m_fPrice);
				}
				break;
			case CReportScheme::ERHPriceSell1:
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceNow = L"";
					StrPriceNow = Float2SymbolString(RealtimePrice.m_astSellPrices[0].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceNow);						
					}
					else
					{
						pCellSymbol->SetText(StrPriceNow);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_astSellPrices[0].m_fPrice);
				}
				break;
			case CReportScheme::ERHPriceBuy5:
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceNow = L"";
					StrPriceNow = Float2SymbolString(RealtimePrice.m_astBuyPrices[4].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceNow);						
					}
					else
					{
						pCellSymbol->SetText(StrPriceNow);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_astBuyPrices[4].m_fPrice);
				}
				break;
			case CReportScheme::ERHPriceBuy4:
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceNow = L"";
					StrPriceNow = Float2SymbolString(RealtimePrice.m_astBuyPrices[3].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceNow);						
					}
					else
					{
						pCellSymbol->SetText(StrPriceNow);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_astBuyPrices[3].m_fPrice);
				}
				break;
			case CReportScheme::ERHPriceBuy3:
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceNow = L"";
					StrPriceNow = Float2SymbolString(RealtimePrice.m_astBuyPrices[2].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceNow);						
					}
					else
					{
						pCellSymbol->SetText(StrPriceNow);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_astBuyPrices[2].m_fPrice);
				}
				break;
			case CReportScheme::ERHPriceBuy2:
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceNow = L"";
					StrPriceNow = Float2SymbolString(RealtimePrice.m_astBuyPrices[1].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceNow);						
					}
					else
					{
						pCellSymbol->SetText(StrPriceNow);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_astBuyPrices[1].m_fPrice);
				}
				break;
			case CReportScheme::ERHPriceBuy1:
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceNow = L"";
					StrPriceNow = Float2SymbolString(RealtimePrice.m_astBuyPrices[0].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceNow);						
					}
					else
					{
						pCellSymbol->SetText(StrPriceNow);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_astBuyPrices[0].m_fPrice);
				}
				break;
			case CReportScheme::ERHVolSell5:
				{
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);				
					CString StrVolumeCur = Float2String(RealtimePrice.m_astSellPrices[4].m_fVolume, 0, bShowTranslate);
					pCell->SetText(StrVolumeCur);
					pCell->SetCompareFloat(RealtimePrice.m_astSellPrices[4].m_fVolume);
				}
				break;
			case CReportScheme::ERHVolSell4:
				{
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);				
					CString StrVolumeCur = Float2String(RealtimePrice.m_astSellPrices[3].m_fVolume, 0, bShowTranslate);
					pCell->SetText(StrVolumeCur);
					pCell->SetCompareFloat(RealtimePrice.m_astSellPrices[3].m_fVolume);
				}
				break;
			case CReportScheme::ERHVolSell3:
				{
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);				
					CString StrVolumeCur = Float2String(RealtimePrice.m_astSellPrices[2].m_fVolume, 0, bShowTranslate);
					pCell->SetText(StrVolumeCur);
					pCell->SetCompareFloat(RealtimePrice.m_astSellPrices[2].m_fVolume);
				}
				break;
			case CReportScheme::ERHVolSell2:
				{
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);				
					CString StrVolumeCur = Float2String(RealtimePrice.m_astSellPrices[1].m_fVolume, 0, bShowTranslate);
					pCell->SetText(StrVolumeCur);
					pCell->SetCompareFloat(RealtimePrice.m_astSellPrices[1].m_fVolume);
				}
				break;
			case CReportScheme::ERHVolSell1:
				{
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);				
					CString StrVolumeCur = Float2String(RealtimePrice.m_astSellPrices[0].m_fVolume, 0, bShowTranslate);
					pCell->SetText(StrVolumeCur);
					pCell->SetCompareFloat(RealtimePrice.m_astSellPrices[0].m_fVolume);
				}
				break;
			case CReportScheme::ERHVolBuy5:
				{
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);				
					CString StrVolumeCur = Float2String(RealtimePrice.m_astBuyPrices[4].m_fVolume, 0, bShowTranslate);
					pCell->SetText(StrVolumeCur);
					pCell->SetCompareFloat(RealtimePrice.m_astBuyPrices[4].m_fVolume);
				}
				break;
			case CReportScheme::ERHVolBuy4:
				{
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);				
					CString StrVolumeCur = Float2String(RealtimePrice.m_astBuyPrices[3].m_fVolume, 0, bShowTranslate);
					pCell->SetText(StrVolumeCur);
					pCell->SetCompareFloat(RealtimePrice.m_astBuyPrices[3].m_fVolume);
				}
				break;
			case CReportScheme::ERHVolBuy3:
				{
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);				
					CString StrVolumeCur = Float2String(RealtimePrice.m_astBuyPrices[2].m_fVolume, 0, bShowTranslate);
					pCell->SetText(StrVolumeCur);
					pCell->SetCompareFloat(RealtimePrice.m_astBuyPrices[2].m_fVolume);
				}
				break;
			case CReportScheme::ERHVolBuy2:
				{
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);				
					CString StrVolumeCur = Float2String(RealtimePrice.m_astBuyPrices[1].m_fVolume, 0, bShowTranslate);
					pCell->SetText(StrVolumeCur);
					pCell->SetCompareFloat(RealtimePrice.m_astBuyPrices[1].m_fVolume);
				}
				break;
			case CReportScheme::ERHVolBuy1:
				{
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);				
					CString StrVolumeCur = Float2String(RealtimePrice.m_astBuyPrices[0].m_fVolume, 0, bShowTranslate);
					pCell->SetText(StrVolumeCur);
					pCell->SetCompareFloat(RealtimePrice.m_astBuyPrices[0].m_fVolume);
				}
				break;
			case CReportScheme::ERHVolumeCur:
				{
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);				
					CString StrVolumeCur = Float2String(RealtimePrice.m_fVolumeCur, 0, bShowTranslate);
					pCell->SetText(StrVolumeCur);
					pCell->SetCompareFloat(RealtimePrice.m_fVolumeCur);
				}
				break;
			case CReportScheme::ERHVolumeTotal:
				{
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);					
					CString StrVolumeTotal = Float2String(RealtimePrice.m_fVolumeTotal, 0, bShowTranslate);
					pCell->SetText(StrVolumeTotal);
					pCell->SetCompareFloat(RealtimePrice.m_fVolumeTotal);
				}
				break;
			case CReportScheme::ERHAmount:			
				{
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetDefaultTextColor(ESCAmount);
					CString StrAmount = Float2String(RealtimePrice.m_fAmountTotal, 0, true);	// �������xx��
					pCell->SetText(StrAmount);
					pCell->SetCompareFloat(RealtimePrice.m_fAmountTotal);
				}
				break;
			case CReportScheme::ERHRiseFall:			// �ǵ�
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	

					CString StrRiseValue = L" -";
					if (0. != RealtimePrice.m_fPriceNew)
					{
						//StrRiseValue = Float2SymbolString(RealtimePrice.m_fPriceNew - fPrevReferPrice, 0, pMerchRow->m_MerchInfo.m_iSaveDec);	
						StrRiseValue = Float2SymbolString(RealtimePrice.m_fPriceNew - fPrevReferPrice, 0, pMerchRow->m_MerchInfo.m_iSaveDec, false, false);	
					}
					else if ( RealtimePrice.m_astBuyPrices[0].m_fPrice != 0.0f
						&& fPrevReferPrice != 0.0f )
					{
						// ��һ�ۼ����ǵ�
						StrRiseValue = Float2SymbolString(RealtimePrice.m_astBuyPrices[0].m_fPrice - fPrevReferPrice, 0, pMerchRow->m_MerchInfo.m_iSaveDec, false, false);	
					}
					pCellSymbol->SetText(StrRiseValue);
					pCellSymbol->SetCompareFloat(RealtimePrice.m_fPriceNew - fPrevReferPrice);
				}
				break;
			case CReportScheme::ERHIndustry:
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(RGB(0xff, 0xff, 0xff));
					CString StrIndustry = L"";

					// �õ����������Ϣ
					CBlockCollection::BlockArray aBlocks;
					CBlockConfig::Instance()->GetBlocksByMerch(pMerchRow->m_MerchInfo.m_iMarketId, pMerchRow->m_MerchInfo.m_StrMerchCode, aBlocks);

					bool32 bFind = false;

					for ( int32 i = 0; i < aBlocks.GetSize(); i++ )
					{
						if ( aBlocks[i]->m_blockCollection.m_StrName == L"��ҵ���" )
						{
							StrIndustry = aBlocks[i]->m_blockInfo.m_StrBlockName;
							bFind = true;
						}
					}

					if ( !bFind )
					{
						StrIndustry.Empty();
					}

					pCell->SetText(StrIndustry);
				}
				break;
			case CReportScheme::ERHZixuan:
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellImage));
					CGridCellImage *pImgCell = NULL;
					pImgCell = (CGridCellImage *)m_CustomGridCtrl.GetCell(iRow, iCol);

					if( T_BlockDesc::EBTUser != eType)
					{
						T_Block* pBlock = CUserBlockManager::Instance()->GetBlock(pMerchRow);
						if ( NULL != pBlock )
						{
							pImgCell->SetImage(IDR_PNG_REPORT_ZIXUAN, 1, CGridCellImage::ESCDelZiXuan);
							break;
						}						
					}

					pImgCell->SetImage(IDR_PNG_REPORT_ADDZIXUAN, 3, CGridCellImage::ECSAddZiXuan);
				}
				break;
			case CReportScheme::ERHRange:				// �Ƿ�%
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	

					CString StrPriceRisePercent = L" -";
					float   fRisePercent = 0.0;

					if (0. != RealtimePrice.m_fPriceNew && 0. != fPrevReferPrice)
					{
						fRisePercent = ((RealtimePrice.m_fPriceNew - fPrevReferPrice) / fPrevReferPrice) * 100.;
						StrPriceRisePercent = Float2SymbolString(fRisePercent, 0, 2, false, false);
					}
					else if ( RealtimePrice.m_astBuyPrices[0].m_fPrice != 0.0f
						&& fPrevReferPrice != 0.0f )
					{
						// ��һ�ۼ����ǵ�
						float fPriceNew = RealtimePrice.m_astBuyPrices[0].m_fPrice;
						fRisePercent = ((fPriceNew - fPrevReferPrice) / fPrevReferPrice) * 100.;
						StrPriceRisePercent = Float2SymbolString(fRisePercent, 0, 2, false, false);	
					}

					pCellSymbol->SetText(StrPriceRisePercent);
					pCellSymbol->SetCompareFloat(fRisePercent);
				}
				break;
			case CReportScheme::ERHSwing:				// ���
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	

					CString StrValue = L" -";
					float fValue = 0.;

					if ( 0. != RealtimePrice.m_fPriceHigh && 0. != RealtimePrice.m_fPriceLow && 0. != fPrevReferPrice)
					{
						fValue = (RealtimePrice.m_fPriceHigh - RealtimePrice.m_fPriceLow) * 100.0f / fPrevReferPrice;
						StrValue = Float2SymbolString(fValue, fValue, 2, false, false, false);
					}	

					pCellSymbol->SetText(StrValue);		
					pCellSymbol->SetCompareFloat(fValue);
				}
				break;
			case CReportScheme::ERHTime:				// ʱ��
				{
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat( DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

					CTime TimeCurrent(RealtimePrice.m_TimeCurrent.m_Time.GetTime());
					CString StrTime(_T("-"));
					if ( TimeCurrent.GetTime() != 0 )
					{
						StrTime.Format(L"%02d:%02d:%02d", 
							/*TimeCurrent.GetYear(), TimeCurrent.GetMonth(), TimeCurrent.GetDay(),*/
							TimeCurrent.GetHour(), TimeCurrent.GetMinute(), TimeCurrent.GetSecond());
					}
					pCell->SetText(StrTime);
				}
				break;
			case CReportScheme::ERHSelStockTime:        // ѡ��ʱ��
				{
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat( DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

					CTime TimeCurrent(pMerchRow->m_uiSelStockTime);
					CString StrTime(_T("-"));

					if ( 0 == TimeCurrent.GetTime())
					{
						// û��ȡ��ѡ��ʱ�����ʾ��ǰʱ��
						TimeCurrent = RealtimePrice.m_TimeCurrent.m_Time.GetTime();
					}

					if (0 != TimeCurrent.GetTime())
					{
						StrTime.Format(L"%02d:%02d:%02d", 
							/*TimeCurrent.GetYear(), TimeCurrent.GetMonth(), TimeCurrent.GetDay(),*/
							TimeCurrent.GetHour(), TimeCurrent.GetMinute(), TimeCurrent.GetSecond());
					}

					pCell->SetText(StrTime);
				}
				break;
			case CReportScheme::ERHSelPrice:		// ��ѡ�۸�
				{
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat( DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

					CString StrPrice = L"";
					StrPrice = Float2SymbolString(pMerchRow->m_fSelPrice, RealtimePrice.m_fPriceNew, pMerchRow->m_MerchInfo.m_iSaveDec);

					pCell->SetText(StrPrice);
				}
				break;
			case CReportScheme::ERHSelRisePercent:		// ��ѡ���Ƿ�% 
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	

					CString StrValue = L" -";
					float fRisePercent = 0.0;

					if ( 0. != pMerchRow->m_fSelPrice && 0. != RealtimePrice.m_fPriceNew)
					{
						fRisePercent = (RealtimePrice.m_fPriceNew - pMerchRow->m_fSelPrice) / pMerchRow->m_fSelPrice * 100;
						StrValue = Float2SymbolString( fRisePercent,  fRisePercent, 2, false, false, false);
					}	

					pCellSymbol->SetText(StrValue);		
					pCellSymbol->SetCompareFloat(fRisePercent);
				}
				break;
			case CReportScheme::ERHBuyPrice:			// �����(��1)
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
					pCellSymbol->SetOwnCursor(NULL);

					CString StrPriceBuy1 = L"";
					StrPriceBuy1 = Float2SymbolString(RealtimePrice.m_astBuyPrices[0].m_fPrice , fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceBuy1);
					}
					else
					{
						pCellSymbol->SetText(StrPriceBuy1);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_astBuyPrices[0].m_fPrice);

				}
				break;
			case CReportScheme::ERHSellPrice:			// ������(��1)
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);
					pCellSymbol->SetOwnCursor(NULL);

					CString StrPriceSell1 = L"";
					StrPriceSell1 = Float2SymbolString(RealtimePrice.m_astSellPrices[0].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceSell1);
					}
					else
					{
						pCellSymbol->SetText(StrPriceSell1);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_astSellPrices[0].m_fPrice);
				}
				break;
			case CReportScheme::ERHBuyAmount:			// ������(��1)
				{
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetDefaultTextColor(ESCVolume);
					CString StrVolumeBuy1 = Float2String(RealtimePrice.m_astBuyPrices[0].m_fVolume, 0, bShowTranslate);
					pCell->SetText(StrVolumeBuy1);
					pCell->SetCompareFloat(RealtimePrice.m_astBuyPrices[0].m_fVolume);
				}
				break;
			case CReportScheme::ERHSellAmount:			// ������(��1)
				{
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetDefaultTextColor(ESCVolume);
					CString StrVolumeSell1 = Float2String(RealtimePrice.m_astSellPrices[0].m_fVolume, 0, bShowTranslate);
					pCell->SetText(StrVolumeSell1);
					pCell->SetCompareFloat(RealtimePrice.m_astSellPrices[0].m_fVolume);
				}
				break;
			case CReportScheme::ERHRate:			  // ί��=��(ί������-ί������)��(ί������+ί������)����100%
				{
					float fRate = 0.0;
					int32 iBuyVolums  = 0;
					int32 iSellVolums = 0;

					for ( int32 i = 0 ; i < 5 ; i++)
					{
						iBuyVolums  += (int32)(RealtimePrice.m_astBuyPrices[i].m_fVolume);
						iSellVolums += (int32)(RealtimePrice.m_astSellPrices[i].m_fVolume);
					}

					if ( 0 != (iBuyVolums + iSellVolums) )
					{
						fRate = (float)(iBuyVolums - iSellVolums)*(float)100 / (iBuyVolums + iSellVolums);
					}

					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrRate = Float2SymbolString(fRate, 0.0, 2, false, true, false);

					pCellSymbol->SetText(StrRate);
					pCellSymbol->SetCompareFloat(fRate);
				}
				break;
			case CReportScheme::ERHHold:				// �ֲ�
				{					
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);					
					if (CReportScheme::IsFuture(eMerchKind))
					{
						CString StrHoldTotal = Float2String(RealtimePrice.m_fHoldTotal, 0, bShowTranslate);									
						pCell->SetText(StrHoldTotal);
						pCell->SetCompareFloat(RealtimePrice.m_fHoldTotal);					
					}
					else
					{
						pCell->SetText(L"-");
					}
				}
				break;				
			case CReportScheme::ERHDifferenceHold:		// �ֲֲ�(������=�ֲ���-��ֲ�)
				{
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);					
					if (CReportScheme::IsFuture(eMerchKind))
					{
						CString StrDifference = Float2String(RealtimePrice.m_fHoldTotal - RealtimePrice.m_fHoldPrev, 0, bShowTranslate);
						pCell->SetText(StrDifference);
						pCell->SetCompareFloat(RealtimePrice.m_fHoldTotal - RealtimePrice.m_fHoldPrev);				
					}
					else
					{
						pCell->SetText(L"-");
					}
				}
				break;
			case CReportScheme::ERHBuild:			 // �տ���=���ɽ���+�����֣�/2
				{	
					float fAddPerDay = RealtimePrice.m_fHoldTotal - RealtimePrice.m_fHoldPrev;
					float fBuild = ( RealtimePrice.m_fVolumeTotal + fAddPerDay ) / 2;

					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);
					CString StrBuild = Float2String(fBuild, 0, bShowTranslate);

					if (CReportScheme::IsFuture(eMerchKind))
					{
						pCell->SetText(StrBuild);
						pCell->SetCompareFloat(fBuild);					
					}
					else
					{
						pCell->SetText(L"-");
					}
				}
				break;
			case CReportScheme::ERHClear:			// ��ƽ��=���ɽ���-�����֣�/2
				{					
					float fAddPerDay = RealtimePrice.m_fHoldTotal - RealtimePrice.m_fHoldPrev;
					float fClear = ( RealtimePrice.m_fVolumeTotal - fAddPerDay ) / 2;

					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);
					// pCell->SetDefaultTextColor(ESCVolume);
					CString StrClear = Float2String(fClear, 0, bShowTranslate);

					if (CReportScheme::IsFuture(eMerchKind))
					{
						pCell->SetText(StrClear);
						pCell->SetCompareFloat(fClear);	
					}
					else
					{
						pCell->SetText(L"-");
					}

				}
				break;
			case CReportScheme::ERHAddPer:			// ��������=�ֲ���-ǰһ�ʳֲ���
				{										
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);
					// pCell->SetDefaultTextColor(ESCVolume);
					CString StrAddPer = Float2String( RealtimePrice.m_fHoldCur, 0, bShowTranslate);

					if (CReportScheme::IsFuture(eMerchKind))
					{
						pCell->SetText(StrAddPer);
						pCell->SetCompareFloat(RealtimePrice.m_fHoldCur);	
					}
					else
					{
						pCell->SetText(L"-");
					}

				}
				break;
			case CReportScheme::ERHBuildPer:		// ���ʿ���=������+�������֣�/2
				{
					float fBuildPer = (RealtimePrice.m_fVolumeCur + RealtimePrice.m_fHoldCur) / 2;

					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);
					// pCell->SetDefaultTextColor(ESCVolume);
					CString StrBuildPer = Float2String( fBuildPer, 0, bShowTranslate);

					if (CReportScheme::IsFuture(eMerchKind))
					{
						pCell->SetText(StrBuildPer);
						pCell->SetCompareFloat(fBuildPer);
					}
					else
					{
						pCell->SetText(L"-");
					}

				}
				break;
			case CReportScheme::ERHClearPer:		// ����ƽ��=������-�������֣�/2
				{
					float fClearPer = (RealtimePrice.m_fVolumeCur - RealtimePrice.m_fHoldCur) / 2;

					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);
					// pCell->SetDefaultTextColor(ESCVolume);
					CString StrClearPer = Float2String( fClearPer, 0, bShowTranslate);

					if (CReportScheme::IsFuture(eMerchKind))
					{
						pCell->SetText(StrClearPer);
						pCell->SetCompareFloat(fClearPer);	
					}
					else
					{
						pCell->SetText(L"-");
					}					
				}
				break;
			case CReportScheme::ERHBuyVolumn:
				{
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetDefaultTextColor(ESCVolume);
					CString StrBuyVolume = Float2String(RealtimePrice.m_fBuyVolume, 0, bShowTranslate);
					pCell->SetText(StrBuyVolume);
					pCell->SetCompareFloat(RealtimePrice.m_fBuyVolume);
				}
				break;
			case CReportScheme::ERHSellVolumn:
				{
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetDefaultTextColor(ESCVolume);
					CString StrSellVolume = Float2String(RealtimePrice.m_fSellVolume, 0, bShowTranslate);
					pCell->SetText(StrSellVolume);
					pCell->SetCompareFloat(RealtimePrice.m_fSellVolume);
				}
				break;		
			case CReportScheme::ERHShortLineSelect:		// ����ѡ�� ������ ������ ��Բ ����
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellImage));
					CGridCellImage *pImgCell = NULL;
					pImgCell = (CGridCellImage *)m_CustomGridCtrl.GetCell(iRow, iCol);
					//pImgCell->SetState(pImgCell->GetState() | GVIS_SHOWTIPS);

					if (bUseMerchIndex)
					{
						if (ESS0 == RespMerchIndex.m_usShort)
						{
							pImgCell->SetImage(IDR_PNG_REPORT_B);
						}
						else if (ESS1 == RespMerchIndex.m_usShort)
						{
							pImgCell->SetImage(IDR_PNG_REPORT_S);
						}
						else if (ESS2 == RespMerchIndex.m_usShort)
						{
							pImgCell->SetImage(IDR_PNG_REPORT_H);
						}
						else if (ESS3 == RespMerchIndex.m_usShort)
						{
							pImgCell->SetImage(IDR_PNG_REPORT_U);
						}

						if (RespMerchIndex.m_usShort >= ESS0 && RespMerchIndex.m_usShort <= ESS3)
						{
							pImgCell->SetTipText(s_KaChooseStockTips[RespMerchIndex.m_usShort]);
						}
					}
				}
				break;	
			case CReportScheme::ERHMidLineSelect:		// ����ѡ��
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellImage));
					CGridCellImage *pImgCell = NULL;
					pImgCell = (CGridCellImage *)m_CustomGridCtrl.GetCell(iRow, iCol);
				//pImgCell->SetState(pImgCell->GetState() | GVIS_SHOWTIPS);

					if (bUseMerchIndex)
					{
						if (ESS0 == RespMerchIndex.m_usMid)
						{
							pImgCell->SetImage(IDR_PNG_REPORT_B);
						}
						else if (ESS1 == RespMerchIndex.m_usMid)
						{
							pImgCell->SetImage(IDR_PNG_REPORT_S);
						}
						else if (ESS2 == RespMerchIndex.m_usMid)
						{
							pImgCell->SetImage(IDR_PNG_REPORT_H);
						}
						else if (ESS3 == RespMerchIndex.m_usMid)
						{
							pImgCell->SetImage(IDR_PNG_REPORT_U);
						}

						if (RespMerchIndex.m_usMid >= ESS0 && RespMerchIndex.m_usMid <= ESS3)
						{
							pImgCell->SetTipText(s_KaChooseStockTips[RespMerchIndex.m_usMid]);
						}
					}
				}
				break;	
			case CReportScheme::ERHCapitalFlow:		// �ʽ�����
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);

					CString StrCapticalFlow = Float2SymbolString(RealtimePrice.m_fCapticalFlow, 0.0, 2, true);
					pCellSymbol->SetText(StrCapticalFlow);
					pCellSymbol->SetCompareFloat(RealtimePrice.m_fCapticalFlow);
				}
				break;	
			case CReportScheme::ERHExBigNetAmount:		// ���󵥾�����(��Ԫ)
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);

					CString StrExBigNetAmount = Float2SymbolString(RespMerchIndex.m_fExBigNetAmount, 0.0, 2, true);
					pCellSymbol->SetText(StrExBigNetAmount);
					pCellSymbol->SetCompareFloat(RespMerchIndex.m_fExBigNetAmount);
				}
				break;
			case CReportScheme::ERHBigNetAmount:		// �󵥾�����(��Ԫ)
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);

					CString StrBigNetAmount = Float2SymbolString(RespMerchIndex.m_fBigNetAmount, 0.0, 2, true);
					pCellSymbol->SetText(StrBigNetAmount);
					pCellSymbol->SetCompareFloat(RespMerchIndex.m_fBigNetAmount);
				}
				break;
			case CReportScheme::ERHMidNetAmount:		// �е�������(��Ԫ)
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);

					CString StrMidNetAmount = Float2SymbolString(RespMerchIndex.m_fMidNetAmount, 0.0, 2, true);
					pCellSymbol->SetText(StrMidNetAmount);
					pCellSymbol->SetCompareFloat(RespMerchIndex.m_fMidNetAmount);
				}
				break;
			case CReportScheme::ERHSmallNetAmount:	    // С��������(��Ԫ)
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);

					CString StrSmallNetAmount = Float2SymbolString(RespMerchIndex.m_fSmallNetAmount, 0.0, 2, true);
					pCellSymbol->SetText(StrSmallNetAmount);
					pCellSymbol->SetCompareFloat(RespMerchIndex.m_fSmallNetAmount);
				}
				break;
			case CReportScheme::ERHChangeRate:		// ������
				{
					fVal = RealtimePrice.m_fTradeRate;

					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrChangeRate = Float2String(fVal, 2, true);
					pCell->SetText(StrChangeRate);
					pCell->SetCompareFloat(RealtimePrice.m_fTradeRate);
				}
				break;	
			case CReportScheme::ERHMarketWinRate:	// ��ӯ��
				{
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrPeRate = Float2String(RealtimePrice.m_fPeRate, 2, true);
					pCell->SetText(StrPeRate);
					pCell->SetCompareFloat(RealtimePrice.m_fPeRate);
				}
				break;	
			case CReportScheme::ERHVolumeRate:		// ����
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrVolRate = Float2SymbolString(RealtimePrice.m_fVolumeRate, 1.0, 2, bShowTranslate);
					pCellSymbol->SetText(StrVolRate);
					pCellSymbol->SetCompareFloat(RealtimePrice.m_fVolumeRate);
				}
				break;	
			case CReportScheme::ERHSpeedRiseFall:		// �����ǵ�
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrRiseRate = Float2SymbolString(RealtimePrice.m_fRiseRate, 0.0, 2, true);
					pCellSymbol->SetText(StrRiseRate);
					pCellSymbol->SetCompareFloat(RealtimePrice.m_fRiseRate);
				}
				break;	
			case CReportScheme::ERHBuySellRate:			// �����
				{
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					float fBuySellRate = 0.0;
					if ( RealtimePrice.m_fSellVolume != 0.0 )
					{
						fBuySellRate = RealtimePrice.m_fBuyVolume / RealtimePrice.m_fSellVolume;
					}
					CString StrPeRate = Float2String(fBuySellRate, 2, bShowTranslate);
					pCell->SetText(StrPeRate);
					pCell->SetCompareFloat(fBuySellRate);
				}
				break;	
			case CReportScheme::ERHAllCapital:				// �ܹɱ�
				{
					fVal = FinanceData.m_fAllCapical;
					fVal /= 10000.0;

					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrValue = Float2String(fVal, 2, false);
					pCell->SetText(StrValue);
					pCell->SetCompareFloat(fVal);
				}
				break;
			case CReportScheme::ERHCircAsset:				// ��ͨ�ɱ�
				{
					fVal = FinanceData.m_fCircAsset;
					fVal /= 10000.0;

					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrValue = Float2String(fVal, 2, false);
					pCell->SetText(StrValue);
					pCell->SetCompareFloat(fVal);
				}
				break;
			case CReportScheme::ERHAllAsset:				// ���ʲ�
				{
					fVal = FinanceData.m_fAllAsset;
					fVal /= 10000.0;

					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrValue = Float2String(fVal, 2, false);
					pCell->SetText(StrValue);
					pCell->SetCompareFloat(fVal);
				}
				break;
			case CReportScheme::ERHFlowDebt:				// ������ծ
				{
					fVal = FinanceData.m_fFlowDebt;
					fVal /= 10000.0;

					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrValue = Float2String(fVal, 2, false);
					pCell->SetText(StrValue);
					pCell->SetCompareFloat(fVal);
				}
				break;
			case CReportScheme::ERHPerFund:				// ÿ�ɹ�����
				{
					fVal = FinanceData.m_fPerFund;

					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrValue = Float2String(fVal, 2, true);
					pCell->SetText(StrValue);
					pCell->SetCompareFloat(fVal);
				}
				break;
			case CReportScheme::ERHBusinessProfit:				// Ӫҵ����
				{
					fVal = FinanceData.m_fBusinessProfit;
					fVal /= 10000.0;

					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrValue = Float2String(fVal, 2, false);
					pCell->SetText(StrValue);
					pCell->SetCompareFloat(fVal);
				}
				break;
			case CReportScheme::ERHPerNoDistribute:				// ÿ��δ����
				{
					fVal = FinanceData.m_fPerNoDistribute;

					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrValue = Float2String(fVal, 2, true);
					pCell->SetText(StrValue);
					pCell->SetCompareFloat(fVal);
				}
				break;
			case CReportScheme::ERHPerIncomeYear:				// ÿ������(��)
				{
					fVal = FinanceData.m_fPerIncomeYear;

					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrValue = Float2String(fVal, 2, true);
					if ( RealtimePrice.m_uiSeason <= 3 )
					{
						//lint --e(569)
						TCHAR chSeason = _T('��');
						chSeason += (TCHAR)RealtimePrice.m_uiSeason;
						StrValue += chSeason;
					}
					pCell->SetText(StrValue);
					pCell->SetCompareFloat(fVal);
				}
				break;
			case CReportScheme::ERHPerPureAsset:				// ÿ�ɾ��ʲ�
				{
					fVal = FinanceData.m_fPerPureAsset;

					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrValue = Float2String(fVal, 2, true);
					pCell->SetText(StrValue);
					pCell->SetCompareFloat(fVal);
				}
				break;
			case CReportScheme::ERHChPerPureAsset:				// ����ÿ�ɾ��ʲ�
				{
					fVal = FinanceData.m_fChPerPureAsset;

					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrValue = Float2String(fVal, 2, false, false);
					pCell->SetText(StrValue);
					pCell->SetCompareFloat(fVal);
				}
				break;
			case CReportScheme::ERHDorRightRate:				// �ɶ�Ȩ���
				{
					fVal = FinanceData.m_fDorRightRate;

					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrValue = Float2String(fVal, 2, false);
					pCell->SetText(StrValue);
					pCell->SetCompareFloat(fVal);
				}
				break;
			case CReportScheme::ERHCircMarketValue:				// ��ͨ��ֵ
				{
					if ( RealtimePrice.m_fPriceNew == 0.0f )
					{
						fVal = FinanceData.m_fCircAsset * fPrevReferPrice;
					}
					else
					{
						fVal = FinanceData.m_fCircAsset * RealtimePrice.m_fPriceNew;
					}
					fVal /= 10000.0;

					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrValue = Float2String(fVal, 2, false);
					pCell->SetText(StrValue);
					pCell->SetCompareFloat(fVal);
				}
				break;
			case CReportScheme::ERHAllMarketValue:				// ����ֵ
				{
					if ( RealtimePrice.m_fPriceNew == 0.0f )
					{
						fVal = FinanceData.m_fAllCapical * fPrevReferPrice;
					}
					else
					{
						fVal = FinanceData.m_fAllCapical * RealtimePrice.m_fPriceNew;
					}
					fVal /= 10000.0;

					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrValue = Float2String(fVal, 2, false);
					pCell->SetText(StrValue);
					pCell->SetCompareFloat(fVal);
				}
				break;
			case CReportScheme::ERHPowerDegree:				// ǿ����
				{
					CString StrValue = _T(" -");
					if ( NULL != m_pAbsCenterManager && m_pAbsCenterManager->GetMerchPowerValue(pMerchRow, fVal) )
					{
						StrValue = Float2String(fVal * 100, 2, false, false);
					}
					pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCell->SetText(StrValue);
					pCell->SetCompareFloat(fVal);
				}
				break;
			default:
				{
					m_CustomGridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_CustomGridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	

					float fdefault = 0.0;					
					CString StrValue = Float2SymbolString(fdefault,0.0,1,false);
					pCellSymbol->SetText(StrValue);					
				}
				break;
			}
		}
	}

	// ˢ�±��
	// �����ȫ�����»��߸��µ�һ��Row�ڵ�ǰ��Ļ,�����µ���:
	if ( bUpdateAllVisible )
	{
		m_CustomGridCtrl.Refresh();

		SetTimer(KUpdateExcelTimerId, KUpdateExcelTimerPeriod, NULL);
	}
	else
	{
		for ( i = 0; i < m_CustomGridCtrl.GetColumnCount(); i ++ )
		{
			m_CustomGridCtrl.InvalidateCellRect(iUpdateOneRow, i);
		}

		//COLORREF clrRise = GetIoViewColor(ESCRise);
		//COLORREF clrFall = GetIoViewColor(ESCFall);
		//COLORREF clrKeep = GetIoViewColor(ESCKeep);

		//AsyncGridToExcel (&m_GridCtrl, BlockDesc.m_StrBlockName, m_StrAsyncExcelFileName, iUpdateOneRow, clrRise, clrFall, clrKeep);
	}
}

void CIoViewReport::UpdateTableContent(BOOL bUpdateAllVisible, IN CMerch *pMerch, bool32 bBlink)
{
	if (!IsWindowVisible())
	{
		return;
	}
	if(m_bShowCustomGrid)
	{
		UpdateCustomTableContent(bUpdateAllVisible, pMerch, bBlink);
		SetHeadInfomationList();
	}

	// liuxt ��ע�͵�����֪����ʲô�����ô����ᵼ�������ָ�����ʱ�������ʾ�쳣
	//if ( m_bDelByBiSplitTrack )
	//{		
	//	LockRedraw();
	//	return;
	//}
	//else
	//{
	//	UnLockRedraw();
	//}

	////
	//if ( GetParentGGTongViewDragFlag() )
	//{
	//	LockRedraw();
	//	return;
	//}
	//else
	//{
	//	UnLockRedraw();
	//}

	int32 i,iRow;
	// ��֤�Ϸ���

	int32 iTab;
	T_BlockDesc::E_BlockType eType;

	if ( !TabIsValid(iTab, eType))
	{
		return;
	}

	// �õ����
	T_BlockDesc BlockDesc = m_aTabInfomations.GetAt(iTab).m_Block;

	// �Ƿ����ȫ����
	int32  iUpdateOneRow = -1;
	bool32 bUpdateOneRow = false;
	CMerch* pMerchUpdate = NULL;

	if ( !bUpdateAllVisible )
	{
		pMerchUpdate = pMerch;
		if ( NULL == pMerchUpdate )
		{
			return;
		}
	}

	for (iRow = 1; iRow < m_GridCtrl.GetRowCount(); iRow++)
	{
		if ( !bUpdateAllVisible )
		{
			if ( bUpdateOneRow )
			{
				break;
			}
		}

		CGridCellSys *pFirstCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
		ASSERT(NULL != pFirstCell);

		//
		CMerch *pMerchRow = (CMerch *)pFirstCell->GetData();
		if ( NULL == pMerchRow )
		{
			//ASSERT(0);
			continue;
		}

		//
		if ( !bUpdateAllVisible )
		{
			if ( pMerchRow != pMerchUpdate )
			{
				continue;
			}
		}

		bUpdateOneRow = true;
		iUpdateOneRow = iRow;

		// �õ����¼۽ṹ
		CRealtimePrice RealtimePrice;
		if (NULL != pMerchRow && NULL != pMerchRow->m_pRealtimePrice)
		{
			RealtimePrice = *pMerchRow->m_pRealtimePrice;
		}

		float fPrevReferPrice = RealtimePrice.m_fPricePrevClose;
		if (ERTFuturesCn == pMerchRow->m_Market.m_MarketInfo.m_eMarketReportType)	// �����ڻ�ʹ���������Ϊ�ο��۸�
		{
			fPrevReferPrice = RealtimePrice.m_fPricePrevAvg;
		}

		// 
		CMerchExtendData ExtendData;
		if ( NULL != pMerchRow )
		{
			if ( NULL != pMerchRow->m_pMerchExtendData )
			{				
				ExtendData = *pMerchRow->m_pMerchExtendData;
			}					
		}	

		// ��������
		CFinanceData	FinanceData;
		if ( NULL != pMerchRow->m_pFinanceData )
		{
			FinanceData = *pMerchRow->m_pFinanceData;
		}

		// ѡ�ɱ�־
		bool32 bUseMerchIndex = false;
		MerchIndexMap::const_iterator it = m_mapMerchIndex.find(pMerchRow);
		T_RespMerchIndex RespMerchIndex;
		if ( it != m_mapMerchIndex.end() )
		{
			bUseMerchIndex = true;
			RespMerchIndex = it->second;
		}

		// ������������
		bool32 bUseMainMasukura = false;
		MainMasukura::const_iterator itMainMasukura = m_mapMainMasukura.find(pMerchRow);
		T_RespMainMasukura RespMainMasukura;
		if ( itMainMasukura != m_mapMainMasukura.end() )
		{
			bUseMainMasukura = true;
			RespMainMasukura = itMainMasukura->second;
		}

		CGridCellSys* pCell;
		CGridCellSymbol *pCellSymbol;

		// ...fangz0926 ���� �ɽ�����������ɫ
		COLORREF ClrCurHold = GetIoViewColor(ESCVolume2);

		float fVal = 0.0;

		bool32 bIsFuture = CReportScheme::IsFuture(GetMerchKind(pMerchRow));
		bool32 bShowTranslate = !bIsFuture;

		// 
		if (m_ReportHeadInfoList.IsEmpty())
		{
			return;
		}

		// �����б�ͷ��ʾ�����ݣ� ��ʾ���������
		for (int32 iCol = 0; iCol < m_GridCtrl.GetColumnCount(); iCol++)
		{
			// ��ͷ��( ���, ����, ����....)
			CReportScheme::E_ReportHeader ERHType;

			if ( -1 != BlockDesc.m_iTypeAdd && iCol == m_GridCtrl.GetColumnCount() -1 )
			{
				ERHType = (CReportScheme::E_ReportHeader)BlockDesc.m_iTypeAdd;
			}
			else
			{
				ERHType = CReportScheme::Instance()->GetReportHeaderEType(m_ReportHeadInfoList[iCol].m_StrHeadNameCn);
			}

			// ��Ʒ���� (�����ڻ�, ����֤ȯ, ���...)
			E_ReportType eMerchKind = GetMerchKind(pMerchRow);	

			pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);

			//
			switch ( ERHType )
			{
			case CReportScheme::ERHRowNo:
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrRowNo;
					StrRowNo.Format(L"%d", m_aTabInfomations.GetAt(iTab).m_iDataIndexBegin + iRow);
					pCell->SetCompareFloat((float)iRow);
					pCell->SetTextClr(GetIoViewColor(ESCText));
					pCell->SetText(StrRowNo);
				}
				break;
			case CReportScheme::ERHMerchCode:
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCell->SetTextClr(GetIoViewColor(ESCText));
					pCell->SetText(pMerchRow->m_MerchInfo.m_StrMerchCode);
				}
				break;
			case CReportScheme::ERHMerchName:
				{
					// 					COLORREF clr;
					//
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);		
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCell->SetTextClr(COLO_NO_NAME_CODE);

					T_MarkData MarkData;
					GV_DRAWMARK_ITEM markItem;
					if ( CMarkManager::Instance().QueryMark(pMerchRow, MarkData)
						&& InitDrawMarkItem(pMerchRow, markItem) )
					{
						// ������ʾ�ִ�
						if ( EMT_Text ==  MarkData.m_eType )
						{
							pCell->SetState( pCell->GetState()|GVIS_SHOWTIPS );
							pCell->SetTipText(MarkData.m_StrTip);
							CString Str;
							Str.Format(_T("%s(%s) CTRL+Q�޸ı������")
								, pMerchRow->m_MerchInfo.m_StrMerchCnName.GetBuffer()
								, pMerchRow->m_MerchInfo.m_StrMerchCode.GetBuffer());
							pCell->SetTipTitle(Str);
						}
						else
						{
							pCell->SetState( pCell->GetState()&(~GVIS_SHOWTIPS) );
						}
					}
					else
					{
						markItem.eDrawMarkFlag = EDMF_None;
						pCell->SetState( pCell->GetState()&(~GVIS_SHOWTIPS) );
					}
					pCell->SetDrawMarkItem(markItem);


					// 					if( T_BlockDesc::EBTUser != eType)
					// 					{
					// 						T_Block* pBlock = CUserBlockManager::Instance()->GetBlock(pMerchRow);
					// 						if ( NULL != pBlock )
					// 						{
					// 							clr = pBlock->m_clrBlock;
					// 							
					// 							// ��ѡ������Ʒ������ɫ������ʾ:
					// 							if ( COLORNOTCOUSTOM == clr )
					// 							{
					// 								clr = GetIoViewColor(ESCText);								
					// 							}	
					// 
					// 							pCell->SetTextClr(clr);	
					// 						}						
					// 					}

					pCell->SetText(pMerchRow->m_MerchInfo.m_StrMerchCnName);
				}
				break;
			case CReportScheme::ERHPricePrevClose:			
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPricePrevClose = L"";
					StrPricePrevClose = Float2SymbolString(RealtimePrice.m_fPricePrevClose, RealtimePrice.m_fPricePrevClose, pMerchRow->m_MerchInfo.m_iSaveDec);	// ������ʾ��ƽ
					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPricePrevClose);
					}
					else
					{
						pCellSymbol->SetText(StrPricePrevClose);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_fPricePrevClose);
				}
				break;
			case CReportScheme::ERHPricePrevBalance:
				{
					// �����
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					if (CReportScheme::IsFuture(eMerchKind))
					{
						BUY_SELL_PLATE nType = CConfigInfo::Instance()->m_nBuySellPlate;
						if ((ERTMony==eMerchKind) && (YESTERDAY_CLOSE==nType))// ����ǡ������̡�,�������ó�������
						{
							CString StrPriceClose = Float2SymbolString(RealtimePrice.m_fPricePrevClose, RealtimePrice.m_fPricePrevClose, pMerchRow->m_MerchInfo.m_iSaveDec);

							if (bBlink)
							{
								pCellSymbol->SetBlinkText(StrPriceClose);
							}
							else
							{
								pCellSymbol->SetText(StrPriceClose);
							}

							pCellSymbol->SetCompareFloat(RealtimePrice.m_fPricePrevClose);	
						}
						else
						{
							CString StrPricePrevBlance = Float2SymbolString(RealtimePrice.m_fPricePrevAvg, RealtimePrice.m_fPricePrevAvg, pMerchRow->m_MerchInfo.m_iSaveDec);

							if (bBlink)
							{
								pCellSymbol->SetBlinkText(StrPricePrevBlance);
							}
							else
							{
								pCellSymbol->SetText(StrPricePrevBlance);
							}

							pCellSymbol->SetCompareFloat(RealtimePrice.m_fPricePrevAvg);	
						}	
					}
					else
					{
						pCellSymbol->SetText(L"--");
					}					
				}
				break;
			case CReportScheme::ERHPriceOpen:
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceOpen = L"";
					StrPriceOpen = Float2SymbolString(RealtimePrice.m_fPriceOpen, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceOpen);
					}
					else
					{
						pCellSymbol->SetText(StrPriceOpen);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_fPriceOpen);
				}
				break;
			case CReportScheme::ERHPriceNew:
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceNew = L"";
					StrPriceNew = Float2SymbolString(RealtimePrice.m_fPriceNew, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceNew);						
					}
					else
					{
						pCellSymbol->SetText(StrPriceNew);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_fPriceNew);
				}
				break;
			case CReportScheme::ERHPriceBalance:			// �����: ����ľ���
				{					
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceAvg = L"";
					StrPriceAvg = Float2SymbolString(RealtimePrice.m_fPriceAvg, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceAvg);
					}
					else
					{
						pCellSymbol->SetText(StrPriceAvg);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_fPriceAvg);
				}
				break;
			case CReportScheme::ERHPriceHigh:
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceHigh = L"";
					StrPriceHigh = Float2SymbolString(RealtimePrice.m_fPriceHigh, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);	

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceHigh);
					}
					else
					{
						pCellSymbol->SetText(StrPriceHigh);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_fPriceHigh);
				}
				break;
			case CReportScheme::ERHPriceLow:
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceLow = L"";
					StrPriceLow = Float2SymbolString(RealtimePrice.m_fPriceLow, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceLow);
					}
					else
					{
						pCellSymbol->SetText(StrPriceLow);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_fPriceLow);
				}
				break;
			case CReportScheme::ERHPriceSell5:
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceNow = L"";
					StrPriceNow = Float2SymbolString(RealtimePrice.m_astSellPrices[4].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceNow);						
					}
					else
					{
						pCellSymbol->SetText(StrPriceNow);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_astSellPrices[4].m_fPrice);
				}
				break;
			case CReportScheme::ERHPriceSell4:
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceNow = L"";
					StrPriceNow = Float2SymbolString(RealtimePrice.m_astSellPrices[3].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceNow);						
					}
					else
					{
						pCellSymbol->SetText(StrPriceNow);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_astSellPrices[3].m_fPrice);
				}
				break;
			case CReportScheme::ERHPriceSell3:
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceNow = L"";
					StrPriceNow = Float2SymbolString(RealtimePrice.m_astSellPrices[2].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceNow);						
					}
					else
					{
						pCellSymbol->SetText(StrPriceNow);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_astSellPrices[2].m_fPrice);
				}
				break;
			case CReportScheme::ERHPriceSell2:
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceNow = L"";
					StrPriceNow = Float2SymbolString(RealtimePrice.m_astSellPrices[1].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceNow);						
					}
					else
					{
						pCellSymbol->SetText(StrPriceNow);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_astSellPrices[1].m_fPrice);
				}
				break;
			case CReportScheme::ERHPriceSell1:
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceNow = L"";
					StrPriceNow = Float2SymbolString(RealtimePrice.m_astSellPrices[0].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceNow);						
					}
					else
					{
						pCellSymbol->SetText(StrPriceNow);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_astSellPrices[0].m_fPrice);
				}
				break;
			case CReportScheme::ERHPriceBuy5:
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceNow = L"";
					StrPriceNow = Float2SymbolString(RealtimePrice.m_astBuyPrices[4].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceNow);						
					}
					else
					{
						pCellSymbol->SetText(StrPriceNow);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_astBuyPrices[4].m_fPrice);
				}
				break;
			case CReportScheme::ERHPriceBuy4:
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceNow = L"";
					StrPriceNow = Float2SymbolString(RealtimePrice.m_astBuyPrices[3].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceNow);						
					}
					else
					{
						pCellSymbol->SetText(StrPriceNow);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_astBuyPrices[3].m_fPrice);
				}
				break;
			case CReportScheme::ERHPriceBuy3:
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceNow = L"";
					StrPriceNow = Float2SymbolString(RealtimePrice.m_astBuyPrices[2].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceNow);						
					}
					else
					{
						pCellSymbol->SetText(StrPriceNow);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_astBuyPrices[2].m_fPrice);
				}
				break;
			case CReportScheme::ERHPriceBuy2:
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceNow = L"";
					StrPriceNow = Float2SymbolString(RealtimePrice.m_astBuyPrices[1].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceNow);						
					}
					else
					{
						pCellSymbol->SetText(StrPriceNow);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_astBuyPrices[1].m_fPrice);
				}
				break;
			case CReportScheme::ERHPriceBuy1:
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrPriceNow = L"";
					StrPriceNow = Float2SymbolString(RealtimePrice.m_astBuyPrices[0].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceNow);						
					}
					else
					{
						pCellSymbol->SetText(StrPriceNow);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_astBuyPrices[0].m_fPrice);
				}
				break;
			case CReportScheme::ERHVolSell5:
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);				
					CString StrVolumeCur = Float2String(RealtimePrice.m_astSellPrices[4].m_fVolume, 0, bShowTranslate);
					pCell->SetText(StrVolumeCur);
					pCell->SetCompareFloat(RealtimePrice.m_astSellPrices[4].m_fVolume);
				}
				break;
			case CReportScheme::ERHVolSell4:
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);				
					CString StrVolumeCur = Float2String(RealtimePrice.m_astSellPrices[3].m_fVolume, 0, bShowTranslate);
					pCell->SetText(StrVolumeCur);
					pCell->SetCompareFloat(RealtimePrice.m_astSellPrices[3].m_fVolume);
				}
				break;
			case CReportScheme::ERHVolSell3:
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);				
					CString StrVolumeCur = Float2String(RealtimePrice.m_astSellPrices[2].m_fVolume, 0, bShowTranslate);
					pCell->SetText(StrVolumeCur);
					pCell->SetCompareFloat(RealtimePrice.m_astSellPrices[2].m_fVolume);
				}
				break;
			case CReportScheme::ERHVolSell2:
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);				
					CString StrVolumeCur = Float2String(RealtimePrice.m_astSellPrices[1].m_fVolume, 0, bShowTranslate);
					pCell->SetText(StrVolumeCur);
					pCell->SetCompareFloat(RealtimePrice.m_astSellPrices[1].m_fVolume);
				}
				break;
			case CReportScheme::ERHVolSell1:
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);				
					CString StrVolumeCur = Float2String(RealtimePrice.m_astSellPrices[0].m_fVolume, 0, bShowTranslate);
					pCell->SetText(StrVolumeCur);
					pCell->SetCompareFloat(RealtimePrice.m_astSellPrices[0].m_fVolume);
				}
				break;
			case CReportScheme::ERHVolBuy5:
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);				
					CString StrVolumeCur = Float2String(RealtimePrice.m_astBuyPrices[4].m_fVolume, 0, bShowTranslate);
					pCell->SetText(StrVolumeCur);
					pCell->SetCompareFloat(RealtimePrice.m_astBuyPrices[4].m_fVolume);
				}
				break;
			case CReportScheme::ERHVolBuy4:
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);				
					CString StrVolumeCur = Float2String(RealtimePrice.m_astBuyPrices[3].m_fVolume, 0, bShowTranslate);
					pCell->SetText(StrVolumeCur);
					pCell->SetCompareFloat(RealtimePrice.m_astBuyPrices[3].m_fVolume);
				}
				break;
			case CReportScheme::ERHVolBuy3:
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);				
					CString StrVolumeCur = Float2String(RealtimePrice.m_astBuyPrices[2].m_fVolume, 0, bShowTranslate);
					pCell->SetText(StrVolumeCur);
					pCell->SetCompareFloat(RealtimePrice.m_astBuyPrices[2].m_fVolume);
				}
				break;
			case CReportScheme::ERHVolBuy2:
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);				
					CString StrVolumeCur = Float2String(RealtimePrice.m_astBuyPrices[1].m_fVolume, 0, bShowTranslate);
					pCell->SetText(StrVolumeCur);
					pCell->SetCompareFloat(RealtimePrice.m_astBuyPrices[1].m_fVolume);
				}
				break;
			case CReportScheme::ERHVolBuy1:
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);				
					CString StrVolumeCur = Float2String(RealtimePrice.m_astBuyPrices[0].m_fVolume, 0, bShowTranslate);
					pCell->SetText(StrVolumeCur);
					pCell->SetCompareFloat(RealtimePrice.m_astBuyPrices[0].m_fVolume);
				}
				break;
			case CReportScheme::ERHVolumeCur:
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);				
					CString StrVolumeCur = Float2String(RealtimePrice.m_fVolumeCur, 0, bShowTranslate);
					pCell->SetText(StrVolumeCur);
					pCell->SetCompareFloat(RealtimePrice.m_fVolumeCur);
				}
				break;
			case CReportScheme::ERHVolumeTotal:
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					/*pCell->SetTextClr(ClrCurHold);*/	
					pCell->SetDefaultTextColor(ESCAmount);
					CString StrVolumeTotal = Float2String(RealtimePrice.m_fVolumeTotal, 0, bShowTranslate);
					pCell->SetText(StrVolumeTotal);
					pCell->SetCompareFloat(RealtimePrice.m_fVolumeTotal);
				}
				break;
			case CReportScheme::ERHAmount:			
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetDefaultTextColor(ESCAmount);
					CString StrAmount = Float2String(RealtimePrice.m_fAmountTotal, 0, true);	// �������xx��
					pCell->SetText(StrAmount);
					pCell->SetCompareFloat(RealtimePrice.m_fAmountTotal);
				}
				break;
			case CReportScheme::ERHRiseFall:			// �ǵ�
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	

					CString StrRiseValue = L" -";
					if (0. != RealtimePrice.m_fPriceNew)
					{
						//StrRiseValue = Float2SymbolString(RealtimePrice.m_fPriceNew - fPrevReferPrice, 0, pMerchRow->m_MerchInfo.m_iSaveDec);	
						StrRiseValue = Float2SymbolString(RealtimePrice.m_fPriceNew - fPrevReferPrice, 0, pMerchRow->m_MerchInfo.m_iSaveDec, false, false);	
					}
					else if ( RealtimePrice.m_astBuyPrices[0].m_fPrice != 0.0f
						&& fPrevReferPrice != 0.0f )
					{
						// ��һ�ۼ����ǵ�
						StrRiseValue = Float2SymbolString(RealtimePrice.m_astBuyPrices[0].m_fPrice - fPrevReferPrice, 0, pMerchRow->m_MerchInfo.m_iSaveDec, false, false);	
					}
					pCellSymbol->SetText(StrRiseValue);
					pCellSymbol->SetCompareFloat(RealtimePrice.m_fPriceNew - fPrevReferPrice);
				}
				break;
			case CReportScheme::ERHIndustry:
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCell->SetTextClr(RGB(0xff, 0xff, 0xff));
					CString StrIndustry = L"";

					// �õ����������Ϣ
					CBlockCollection::BlockArray aBlocks;
					CBlockConfig::Instance()->GetBlocksByMerch(pMerchRow->m_MerchInfo.m_iMarketId, pMerchRow->m_MerchInfo.m_StrMerchCode, aBlocks);

					bool32 bFind = false;

					for ( int32 i = 0; i < aBlocks.GetSize(); i++ )
					{
						if ( aBlocks[i]->m_blockCollection.m_StrName == L"��ҵ" )
						{
							StrIndustry = aBlocks[i]->m_blockInfo.m_StrBlockName;
							bFind = true;
						}
					}

					if ( !bFind )
					{
						StrIndustry.Empty();
					}
					pCell->SetText(StrIndustry);
				}
				break;
			case CReportScheme::ERHZixuan:
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellImage));
					
					CGridCellImage *pImgCell = NULL;
					pImgCell = (CGridCellImage *)m_GridCtrl.GetCell(iRow, iCol);

					if( T_BlockDesc::EBTUser != eType)
					{
						T_Block* pBlock = CUserBlockManager::Instance()->GetBlock(pMerchRow);
						if ( NULL != pBlock )
						{
							pImgCell->SetImage(IDR_PNG_REPORT_ZIXUAN, 1, CGridCellImage::ESCDelZiXuan);
						}
						else
						{
							pImgCell->SetImage(IDR_PNG_REPORT_ADDZIXUAN, 3, CGridCellImage::ECSAddZiXuan);
						}
					}
				}
				break;
			case CReportScheme::ERHRange:				// �Ƿ�%
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	

					CString StrPriceRisePercent = L" -";
					float   fRisePercent = 0.0;

					if (0. != RealtimePrice.m_fPriceNew && 0. != fPrevReferPrice)
					{
						fRisePercent = ((RealtimePrice.m_fPriceNew - fPrevReferPrice) / fPrevReferPrice) * 100.;
						StrPriceRisePercent = Float2SymbolString(fRisePercent, 0, 2, false, false);
					}
					else if ( RealtimePrice.m_astBuyPrices[0].m_fPrice != 0.0f
						&& fPrevReferPrice != 0.0f )
					{
						// ��һ�ۼ����ǵ�
						float fPriceNew = RealtimePrice.m_astBuyPrices[0].m_fPrice;
						fRisePercent = ((fPriceNew - fPrevReferPrice) / fPrevReferPrice) * 100.;
						StrPriceRisePercent = Float2SymbolString(fRisePercent, 0, 2, false, false);	
					}

					pCellSymbol->SetText(StrPriceRisePercent);
					pCellSymbol->SetCompareFloat(fRisePercent);
				}
				break;
			case CReportScheme::ERHSwing:				// ���
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	

					CString StrValue = L" -";
					float fValue = 0.;

					if ( 0. != RealtimePrice.m_fPriceHigh && 0. != RealtimePrice.m_fPriceLow && 0. != fPrevReferPrice)
					{
						fValue = (RealtimePrice.m_fPriceHigh - RealtimePrice.m_fPriceLow) * 100.0f / fPrevReferPrice;
						StrValue = Float2SymbolString(fValue, fValue, 2, false, false, false);
					}	

					pCellSymbol->SetText(StrValue);		
					pCellSymbol->SetCompareFloat(fValue);
				}
				break;
			case CReportScheme::ERHTime:				// ʱ��
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat( DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

					CTime TimeCurrent(RealtimePrice.m_TimeCurrent.m_Time.GetTime());
					CString StrTime(_T("-"));
					if ( TimeCurrent.GetTime() != 0 )
					{
						StrTime.Format(L"%02d:%02d:%02d", 
							/*TimeCurrent.GetYear(), TimeCurrent.GetMonth(), TimeCurrent.GetDay(),*/
							TimeCurrent.GetHour(), TimeCurrent.GetMinute(), TimeCurrent.GetSecond());
					}
					pCell->SetText(StrTime);
				}
				break;
			case CReportScheme::ERHSelStockTime:        // ѡ��ʱ��
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat( DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

					CTime TimeCurrent(pMerchRow->m_uiSelStockTime);
					CString StrTime(_T("-"));

					if ( 0 == TimeCurrent.GetTime())
					{
						// û��ȡ��ѡ��ʱ�����ʾ��ǰʱ��
						TimeCurrent = RealtimePrice.m_TimeCurrent.m_Time.GetTime();
					}

					if (0 != TimeCurrent.GetTime())
					{
						StrTime.Format(L"%02d:%02d:%02d", 
							/*TimeCurrent.GetYear(), TimeCurrent.GetMonth(), TimeCurrent.GetDay(),*/
							TimeCurrent.GetHour(), TimeCurrent.GetMinute(), TimeCurrent.GetSecond());
					}

					pCell->SetText(StrTime);
				}
				break;
			case CReportScheme::ERHSelPrice:		// ��ѡ�۸�
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat( DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

					CString StrPrice = L"-";
					StrPrice = Float2SymbolString(pMerchRow->m_fSelPrice, pMerchRow->m_fSelPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					pCell->SetText(StrPrice);
				}
				break;
			case CReportScheme::ERHSelRisePercent:		// ��ѡ���Ƿ�% 
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);	

					CString StrValue   = L"-";
					float fRisePercent = 0.0;

					if ( 0. != pMerchRow->m_fSelPrice  && 0. != RealtimePrice.m_fPriceNew)
					{
						fRisePercent = (RealtimePrice.m_fPriceNew - pMerchRow->m_fSelPrice) / pMerchRow->m_fSelPrice * 100;
						StrValue = Float2SymbolString( fRisePercent,  0, 2, false, false, false);
					}	

					pCellSymbol->SetText(StrValue);		
					pCellSymbol->SetCompareFloat(fRisePercent);
				}
				break;
			case CReportScheme::ERHBuyPrice:			// �����(��1)
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
					pCellSymbol->SetOwnCursor(NULL);

					CString StrPriceBuy1 = L"";
					StrPriceBuy1 = Float2SymbolString(RealtimePrice.m_astBuyPrices[0].m_fPrice , fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceBuy1);
					}
					else
					{
						pCellSymbol->SetText(StrPriceBuy1);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_astBuyPrices[0].m_fPrice);

				}
				break;
			case CReportScheme::ERHSellPrice:			// ������(��1)
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);
					pCellSymbol->SetOwnCursor(NULL);

					CString StrPriceSell1 = L"";
					StrPriceSell1 = Float2SymbolString(RealtimePrice.m_astSellPrices[0].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);

					if (bBlink)
					{
						pCellSymbol->SetBlinkText(StrPriceSell1);
					}
					else
					{
						pCellSymbol->SetText(StrPriceSell1);
					}

					pCellSymbol->SetCompareFloat(RealtimePrice.m_astSellPrices[0].m_fPrice);
				}
				break;
			case CReportScheme::ERHBuyAmount:			// ������(��1)
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetDefaultTextColor(ESCVolume);
					CString StrVolumeBuy1 = Float2String(RealtimePrice.m_astBuyPrices[0].m_fVolume, 0, bShowTranslate);
					pCell->SetText(StrVolumeBuy1);
					pCell->SetCompareFloat(RealtimePrice.m_astBuyPrices[0].m_fVolume);
				}
				break;
			case CReportScheme::ERHSellAmount:			// ������(��1)
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetDefaultTextColor(ESCVolume);
					CString StrVolumeSell1 = Float2String(RealtimePrice.m_astSellPrices[0].m_fVolume, 0, bShowTranslate);
					pCell->SetText(StrVolumeSell1);
					pCell->SetCompareFloat(RealtimePrice.m_astSellPrices[0].m_fVolume);
				}
				break;
			case CReportScheme::ERHRate:			  // ί��=��(ί������-ί������)��(ί������+ί������)����100%
				{
					float fRate = 0.0;
					int32 iBuyVolums  = 0;
					int32 iSellVolums = 0;

					for ( int32 i = 0 ; i < 5 ; i++)
					{
						iBuyVolums  += (int32)(RealtimePrice.m_astBuyPrices[i].m_fVolume);
						iSellVolums += (int32)(RealtimePrice.m_astSellPrices[i].m_fVolume);
					}

					if ( 0 != (iBuyVolums + iSellVolums) )
					{
						fRate = (float)(iBuyVolums - iSellVolums)*(float)100 / (iBuyVolums + iSellVolums);
					}

					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrRate = Float2SymbolString(fRate, 0.0, 2, false, true, false);

					pCellSymbol->SetText(StrRate);
					pCellSymbol->SetCompareFloat(fRate);
				}
				break;
			case CReportScheme::ERHHold:				// �ֲ�
				{					
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);					
					if (CReportScheme::IsFuture(eMerchKind))
					{
						CString StrHoldTotal = Float2String(RealtimePrice.m_fHoldTotal, 0, bShowTranslate);									
						pCell->SetText(StrHoldTotal);
						pCell->SetCompareFloat(RealtimePrice.m_fHoldTotal);					
					}
					else
					{
						pCell->SetText(L"-");
					}
				}
				break;				
			case CReportScheme::ERHDifferenceHold:		// �ֲֲ�(������=�ֲ���-��ֲ�)
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);					
					if (CReportScheme::IsFuture(eMerchKind))
					{
						CString StrDifference = Float2String(RealtimePrice.m_fHoldTotal - RealtimePrice.m_fHoldPrev, 0, bShowTranslate);
						pCell->SetText(StrDifference);
						pCell->SetCompareFloat(RealtimePrice.m_fHoldTotal - RealtimePrice.m_fHoldPrev);				
					}
					else
					{
						pCell->SetText(L"-");
					}
				}
				break;
			case CReportScheme::ERHBuild:			 // �տ���=���ɽ���+�����֣�/2
				{	
					float fAddPerDay = RealtimePrice.m_fHoldTotal - RealtimePrice.m_fHoldPrev;
					float fBuild = ( RealtimePrice.m_fVolumeTotal + fAddPerDay ) / 2;

					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);
					CString StrBuild = Float2String(fBuild, 0, bShowTranslate);

					if (CReportScheme::IsFuture(eMerchKind))
					{
						pCell->SetText(StrBuild);
						pCell->SetCompareFloat(fBuild);					
					}
					else
					{
						pCell->SetText(L"-");
					}
				}
				break;
			case CReportScheme::ERHClear:			// ��ƽ��=���ɽ���-�����֣�/2
				{					
					float fAddPerDay = RealtimePrice.m_fHoldTotal - RealtimePrice.m_fHoldPrev;
					float fClear = ( RealtimePrice.m_fVolumeTotal - fAddPerDay ) / 2;

					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);
					// pCell->SetDefaultTextColor(ESCVolume);
					CString StrClear = Float2String(fClear, 0, bShowTranslate);

					if (CReportScheme::IsFuture(eMerchKind))
					{
						pCell->SetText(StrClear);
						pCell->SetCompareFloat(fClear);	
					}
					else
					{
						pCell->SetText(L"-");
					}

				}
				break;
			case CReportScheme::ERHAddPer:			// ��������=�ֲ���-ǰһ�ʳֲ���
				{										
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);
					// pCell->SetDefaultTextColor(ESCVolume);
					CString StrAddPer = Float2String( RealtimePrice.m_fHoldCur, 0, bShowTranslate);

					if (CReportScheme::IsFuture(eMerchKind))
					{
						pCell->SetText(StrAddPer);
						pCell->SetCompareFloat(RealtimePrice.m_fHoldCur);	
					}
					else
					{
						pCell->SetText(L"-");
					}

				}
				break;
			case CReportScheme::ERHBuildPer:		// ���ʿ���=������+�������֣�/2
				{
					float fBuildPer = (RealtimePrice.m_fVolumeCur + RealtimePrice.m_fHoldCur) / 2;

					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);
					// pCell->SetDefaultTextColor(ESCVolume);
					CString StrBuildPer = Float2String( fBuildPer, 0, bShowTranslate);

					if (CReportScheme::IsFuture(eMerchKind))
					{
						pCell->SetText(StrBuildPer);
						pCell->SetCompareFloat(fBuildPer);
					}
					else
					{
						pCell->SetText(L"-");
					}

				}
				break;
			case CReportScheme::ERHClearPer:		// ����ƽ��=������-�������֣�/2
				{
					float fClearPer = (RealtimePrice.m_fVolumeCur - RealtimePrice.m_fHoldCur) / 2;

					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetTextClr(ClrCurHold);
					// pCell->SetDefaultTextColor(ESCVolume);
					CString StrClearPer = Float2String( fClearPer, 0, bShowTranslate);

					if (CReportScheme::IsFuture(eMerchKind))
					{
						pCell->SetText(StrClearPer);
						pCell->SetCompareFloat(fClearPer);	
					}
					else
					{
						pCell->SetText(L"-");
					}					
				}
				break;
			case CReportScheme::ERHBuyVolumn:
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetDefaultTextColor(ESCVolume);
					CString StrBuyVolume = Float2String(RealtimePrice.m_fBuyVolume, 0, bShowTranslate);
					pCell->SetText(StrBuyVolume);
					pCell->SetCompareFloat(RealtimePrice.m_fBuyVolume);
				}
				break;
			case CReportScheme::ERHSellVolumn:
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCell->SetDefaultTextColor(ESCVolume);
					CString StrSellVolume = Float2String(RealtimePrice.m_fSellVolume, 0, bShowTranslate);
					pCell->SetText(StrSellVolume);
					pCell->SetCompareFloat(RealtimePrice.m_fSellVolume);
				}
				break;		
			case CReportScheme::ERHShortLineSelect:		// ����ѡ�� ������ ������ ��Բ ����
				{
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellImage));
					CGridCellImage *pImgCell = NULL;
					pImgCell = (CGridCellImage *)m_GridCtrl.GetCell(iRow, iCol);
					//pImgCell->SetState(pImgCell->GetState() | GVIS_SHOWTIPS);

					if (bUseMerchIndex)
					{
						if (ESS0 == RespMerchIndex.m_usShort)
						{
							pImgCell->SetImage(IDR_PNG_REPORT_B);
						}
						else if (ESS1 == RespMerchIndex.m_usShort)
						{
							pImgCell->SetImage(IDR_PNG_REPORT_S);
						}
						else if (ESS2 == RespMerchIndex.m_usShort)
						{
							pImgCell->SetImage(IDR_PNG_REPORT_H);
						}
						else if (ESS3 == RespMerchIndex.m_usShort)
						{
							pImgCell->SetImage(IDR_PNG_REPORT_U);
						}

						if (RespMerchIndex.m_usShort >= ESS0 && RespMerchIndex.m_usShort <= ESS3)
						{
							pImgCell->SetTipText(s_KaChooseStockTips[RespMerchIndex.m_usShort]);
						}
					}
				}
				break;	
			case CReportScheme::ERHMidLineSelect:		// ����ѡ��
				{
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellImage));
					CGridCellImage *pImgCell = NULL;
					pImgCell = (CGridCellImage *)m_GridCtrl.GetCell(iRow, iCol);
					//pImgCell->SetState(pImgCell->GetState() | GVIS_SHOWTIPS);

					if (bUseMerchIndex)
					{
						if (ESS0 == RespMerchIndex.m_usMid)
						{
							pImgCell->SetImage(IDR_PNG_REPORT_B);
						}
						else if (ESS1 == RespMerchIndex.m_usMid)
						{
							pImgCell->SetImage(IDR_PNG_REPORT_S);
						}
						else if (ESS2 == RespMerchIndex.m_usMid)
						{
							pImgCell->SetImage(IDR_PNG_REPORT_H);
						}
						else if (ESS3 == RespMerchIndex.m_usMid)
						{
							pImgCell->SetImage(IDR_PNG_REPORT_U);
						}

						if (RespMerchIndex.m_usMid >= ESS0 && RespMerchIndex.m_usMid <= ESS3)
						{
							pImgCell->SetTipText(s_KaChooseStockTips[RespMerchIndex.m_usMid]);
						}
					}
				}
				break;	
			case CReportScheme::ERHCapitalFlow:		// �ʽ�����
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);

					CString StrCapticalFlow = Float2SymbolString(RealtimePrice.m_fCapticalFlow, 0.0, 2, true);
					pCellSymbol->SetText(StrCapticalFlow);
					pCellSymbol->SetCompareFloat(RealtimePrice.m_fCapticalFlow);
				}
				break;	
			case CReportScheme::ERHAllNetAmount:		// ������
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);
					
					if (bUseMerchIndex)
					{
						CString StrAllNetAmount = Float2SymbolString(RespMerchIndex.m_fAllNetAmount, 0.0, 2, true);
						pCellSymbol->SetText(StrAllNetAmount);
						pCellSymbol->SetCompareFloat(RespMerchIndex.m_fAllNetAmount);
					}
				}
				break;
			case CReportScheme::ERHExBigBuyAmount:		// ��������
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);
					
					if (bUseMerchIndex)
					{
						CString StrExBigBuyAmount = Float2SymbolString(RespMerchIndex.m_fExBigBuyAmount, 0.0, 2, true);
						pCellSymbol->SetText(StrExBigBuyAmount);
						pCellSymbol->SetCompareFloat(RespMerchIndex.m_fExBigBuyAmount);
					}
				}
				break;
			case CReportScheme::ERHExBigSellAmount:		// ��������
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);
					
					if (bUseMerchIndex)
					{
						CString StrExBigSellAmount = Float2SymbolString(RespMerchIndex.m_fExBigSellAmount, 0.0, 2, true);
						pCellSymbol->SetText(StrExBigSellAmount);
						pCellSymbol->SetCompareFloat(RespMerchIndex.m_fExBigSellAmount);
					}
				}
				break;
			case CReportScheme::ERHExBigNetAmount:		// ���󵥾���
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);
					
					if (bUseMerchIndex)
					{
						CString StrExBigNetAmount = Float2SymbolString(RespMerchIndex.m_fExBigNetAmount, 0.0, 2, true);
						pCellSymbol->SetText(StrExBigNetAmount);
						pCellSymbol->SetCompareFloat(RespMerchIndex.m_fExBigNetAmount);
					}
				}
				break;
			case CReportScheme::ERHExBigNetAmountProportion:		// ���󵥾���ռ�� 
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);
					
					if (bUseMerchIndex)
					{
						CString StrExBigNetAmountProportion = Float2SymbolString(RespMerchIndex.m_fExBigNetAmountProportion, 0.0, 2, true);
						pCellSymbol->SetText(StrExBigNetAmountProportion);
						pCellSymbol->SetCompareFloat(RespMerchIndex.m_fExBigNetAmountProportion);
					}
				}
				break;
			case CReportScheme::ERHExBigNetTotalAmount:			// �����ܶ�
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);
					
					if (bUseMerchIndex)
					{
						CString StrExBigNetTotalAmount = Float2SymbolString(RespMerchIndex.m_fExBigNetTotalAmount, 0.0, 2, true);
						pCellSymbol->SetText(StrExBigNetTotalAmount);
						pCellSymbol->SetCompareFloat(RespMerchIndex.m_fExBigNetTotalAmount);
					}
				}
				break;
			case CReportScheme::ERHExBigNetTotalAmountProportion:	// �����ܶ�ռ��
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);
					
					if (bUseMerchIndex)
					{
						CString StrExBigNetTotalAmountProportion = Float2SymbolString(RespMerchIndex.m_fExBigNetTotalAmountProportion, 0.0, 2, true);
						pCellSymbol->SetText(StrExBigNetTotalAmountProportion);
						pCellSymbol->SetCompareFloat(RespMerchIndex.m_fExBigNetTotalAmountProportion);
					}
				}
				break;
			case CReportScheme::ERHBigBuyAmount:		// ������
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);
					
					if (bUseMerchIndex)
					{	
						CString StrBigBuyAmount = Float2SymbolString(RespMerchIndex.m_fBigBuyAmount, 0.0, 2, true);
						pCellSymbol->SetText(StrBigBuyAmount);
						pCellSymbol->SetCompareFloat(RespMerchIndex.m_fBigBuyAmount);
					}
				}
				break;
			case CReportScheme::ERHBigSellAmount:		// ������
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);
					
					if (bUseMerchIndex)
					{
						CString StrBigSellAmount = Float2SymbolString(RespMerchIndex.m_fBigSellAmount, 0.0, 2, true);
						pCellSymbol->SetText(StrBigSellAmount);
						pCellSymbol->SetCompareFloat(RespMerchIndex.m_fBigSellAmount);
					}
				}
				break;
			case CReportScheme::ERHBigNetAmount:		// �󵥾���
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);
					
					if (bUseMerchIndex)
					{
						CString StrBigNetAmount = Float2SymbolString(RespMerchIndex.m_fBigNetAmount, 0.0, 2, true);
						pCellSymbol->SetText(StrBigNetAmount);
						pCellSymbol->SetCompareFloat(RespMerchIndex.m_fBigNetAmount);
					}
				}
				break;
			case CReportScheme::ERHBigNetAmountProportion:			// �󵥾���ռ�� 
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);
					
					if (bUseMerchIndex)
					{
						CString StrBigNetAmountProportion = Float2SymbolString(RespMerchIndex.m_fBigNetAmountProportion, 0.0, 2, true);
						pCellSymbol->SetText(StrBigNetAmountProportion);
						pCellSymbol->SetCompareFloat(RespMerchIndex.m_fBigNetAmountProportion);
					}
				}
				break;
			case CReportScheme::ERHMarketMargin:			// �о��� 
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					//pCell->SetCompareFloat((float)iRow);
					pCell->SetTextClr(COLO_NO_NAME_CODE);
					pCell->SetText(L"-");
				}
				break;
			case CReportScheme::ERHNetProfit:			// ������ 
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					//pCell->SetCompareFloat((float)iRow);
					pCell->SetTextClr(COLO_NO_NAME_CODE);
					pCell->SetText(L"-");
				}
				break;
			case CReportScheme::ERHTotalProfit:			// �����ܶ� 
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					//pCell->SetCompareFloat((float)iRow);
					pCell->SetTextClr(COLO_NO_NAME_CODE);
					pCell->SetText(L"-");
				}
				break;
			case CReportScheme::ERHassetsRate:			// ���ʲ������� 
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					//pCell->SetCompareFloat((float)iRow);
					pCell->SetTextClr(COLO_NO_NAME_CODE);
					pCell->SetText(L"-");
				}
				break;
			case CReportScheme::ERHNetProfitGrowthRate:			// ������������ 
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					//pCell->SetCompareFloat((float)iRow);
					pCell->SetTextClr(COLO_NO_NAME_CODE);
					pCell->SetText(L"-");
				}
				break;
			case CReportScheme::ERHNetAmountofthemain:			// �������� 
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);

					if ( bUseMerchIndex )
					{
						float fMainNetVolume = 0.0;
						if ( 0. != FinanceData.m_fCircAsset)
						{
							float fMainMainNetAmount = RespMerchIndex.m_fExBigNetAmount + RespMerchIndex.m_fBigNetAmount;
							fMainNetVolume = fMainMainNetAmount / FinanceData.m_fCircAsset * 100;
						}
						
						CString StrBigNetAmountProportion = Float2SymbolString(fMainNetVolume, 0.0, 2, true);
						pCellSymbol->SetText(StrBigNetAmountProportion);
						pCellSymbol->SetCompareFloat(fMainNetVolume);
					}
				}
				break;
			case CReportScheme::ERHBigNetTotalAmount:				// ���ܶ�
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);
					
					if (bUseMerchIndex)
					{
						CString StrBigNetTotalAmount = Float2SymbolString(RespMerchIndex.m_fBigNetTotalAmount, 0.0, 2, true);
						pCellSymbol->SetText(StrBigNetTotalAmount);
						pCellSymbol->SetCompareFloat(RespMerchIndex.m_fBigNetTotalAmount);
					}	
				}
				break;
			case CReportScheme::ERHBigNetTotalAmountProportion:		// ���ܶ�ռ��
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);
					
					if (bUseMerchIndex)
					{
						CString StrBigNetTotalAmountProportion = Float2SymbolString(RespMerchIndex.m_fBigNetTotalAmountProportion, 0.0, 2, true);
						pCellSymbol->SetText(StrBigNetTotalAmountProportion);
						pCellSymbol->SetCompareFloat(RespMerchIndex.m_fBigNetTotalAmountProportion);
					}
				}
				break;
			case CReportScheme::ERHMidBuyAmount:		// �е�����
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);
					
					if (bUseMerchIndex)
					{
						CString StrMidBuyAmount = Float2SymbolString(RespMerchIndex.m_fMidBuyAmount, 0.0, 2, true);
						pCellSymbol->SetText(StrMidBuyAmount);
						pCellSymbol->SetCompareFloat(RespMerchIndex.m_fMidBuyAmount);
					}
				}
				break;
			case CReportScheme::ERHMidSellAmount:		// �е�����
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);
					
					if (bUseMerchIndex)
					{
						CString StrMidSellAmount = Float2SymbolString(RespMerchIndex.m_fMidSellAmount, 0.0, 2, true);
						pCellSymbol->SetText(StrMidSellAmount);
						pCellSymbol->SetCompareFloat(RespMerchIndex.m_fMidSellAmount);
					}
				}
				break;
			case CReportScheme::ERHMidNetAmount:		// �е�����
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);
					
					if (bUseMerchIndex)
					{
						CString StrMidNetAmount = Float2SymbolString(RespMerchIndex.m_fMidNetAmount, 0.0, 2, true);
						pCellSymbol->SetText(StrMidNetAmount);
						pCellSymbol->SetCompareFloat(RespMerchIndex.m_fMidNetAmount);
					}		
				}
				break;
			case CReportScheme::ERHMidNetAmountProportion:			// �е�����ռ�� 
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);
					
					if (bUseMerchIndex)
					{
						CString StrMidNetAmountProportion = Float2SymbolString(RespMerchIndex.m_fMidNetAmountProportion, 0.0, 2, true);
						pCellSymbol->SetText(StrMidNetAmountProportion);
						pCellSymbol->SetCompareFloat(RespMerchIndex.m_fMidNetAmountProportion);
					}
				}
				break;
			case CReportScheme::ERHMidNetTotalAmount:				// �е��ܶ�
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);
					
					if (bUseMerchIndex)
					{
						CString StrMidNetTotalAmount = Float2SymbolString(RespMerchIndex.m_fMidNetTotalAmount, 0.0, 2, true);
						pCellSymbol->SetText(StrMidNetTotalAmount);
						pCellSymbol->SetCompareFloat(RespMerchIndex.m_fMidNetTotalAmount);
					}
				}
				break;
			case CReportScheme::ERHMidNetTotalAmountProportion:		// �е��ܶ�ռ��
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);

					if (bUseMerchIndex)
					{
						CString StrMidNetTotalAmountProportion = Float2SymbolString(RespMerchIndex.m_fMidNetTotalAmountProportion, 0.0, 2, true);
						pCellSymbol->SetText(StrMidNetTotalAmountProportion);
						pCellSymbol->SetCompareFloat(RespMerchIndex.m_fMidNetTotalAmountProportion);
					}
				}
				break;
			case CReportScheme::ERHSmallBuyAmount:		// С������
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);

					if (bUseMerchIndex)
					{
						CString StrSmallBuyAmount = Float2SymbolString(RespMerchIndex.m_fSmallBuyAmount, 0.0, 2, true);
						pCellSymbol->SetText(StrSmallBuyAmount);
						pCellSymbol->SetCompareFloat(RespMerchIndex.m_fSmallBuyAmount);
					}
				}
				break;
			case CReportScheme::ERHSmallSellAmount:		// С������
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);

					if (bUseMerchIndex)
					{
						CString StrSmallSellAmount = Float2SymbolString(RespMerchIndex.m_fSmallSellAmount, 0.0, 2, true);
						pCellSymbol->SetText(StrSmallSellAmount);
						pCellSymbol->SetCompareFloat(RespMerchIndex.m_fSmallSellAmount);
					}
				}
				break;
			case CReportScheme::ERHSmallNetAmount:	    // С������
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);

					if (bUseMerchIndex)
					{
						CString StrSmallNetAmount = Float2SymbolString(RespMerchIndex.m_fSmallNetAmount, 0.0, 2, true);
						pCellSymbol->SetText(StrSmallNetAmount);
						pCellSymbol->SetCompareFloat(RespMerchIndex.m_fSmallNetAmount);
					}
				}
				break;
			case CReportScheme::ERHSmallNetAmountProportion:		// С������ռ�� 
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);

					if (bUseMerchIndex)
					{
						CString StrSmallNetAmountProportion = Float2SymbolString(RespMerchIndex.m_fSmallNetAmountProportion, 0.0, 2, true);
						pCellSymbol->SetText(StrSmallNetAmountProportion);
						pCellSymbol->SetCompareFloat(RespMerchIndex.m_fSmallNetAmountProportion);
					}
				}	
				break;
			case CReportScheme::ERHSmallNetTotalAmount:				// С���ܶ�
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);

					if (bUseMerchIndex)
					{
						CString StrSmallNetTotalAmount = Float2SymbolString(RespMerchIndex.m_fSmallNetTotalAmount, 0.0, 2, true);
						pCellSymbol->SetText(StrSmallNetTotalAmount);
						pCellSymbol->SetCompareFloat(RespMerchIndex.m_fSmallNetTotalAmount);
					}
				}
				break;
			case CReportScheme::ERHSmallNetTotalAmountProportion:	// С���ܶ�ռ��
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);

					if (bUseMerchIndex)
					{
						CString StrSmallNetTotalAmountProportion = Float2SymbolString(RespMerchIndex.m_fSmallNetTotalAmountProportion, 0.0, 2, true);
						pCellSymbol->SetText(StrSmallNetTotalAmountProportion);
						pCellSymbol->SetCompareFloat(RespMerchIndex.m_fSmallNetTotalAmountProportion);
					}
				}
				break;
			case CReportScheme::ERHTodayMasukuraProportion:		// ��������ռ��
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);
					
					if (bUseMainMasukura)
					{
						CString StrTodayMasukuraProportion= Float2SymbolString(RespMainMasukura.m_fMasukuraProportion, 0.0, 2, true);
						pCellSymbol->SetText(StrTodayMasukuraProportion);
						pCellSymbol->SetCompareFloat(RespMainMasukura.m_fMasukuraProportion);
					}
				}
				break;
			case CReportScheme::ERHTodayRanked:					// ��������
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);
					
					if (bUseMainMasukura)
					{
						CString StrTodayRanked= Float2String(RespMainMasukura.m_iRanked, 0, bShowTranslate);
						pCellSymbol->SetText(StrTodayRanked);
						pCellSymbol->SetCompareFloat(RespMainMasukura.m_iRanked);
					}
				}
				break;
			case CReportScheme::ERHTodayRise:					// �����Ƿ�
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);

					if (bUseMainMasukura)
					{
						CString StrTodayRise= Float2SymbolString(RespMainMasukura.m_fRise, 0.0, 2, true);
						pCellSymbol->SetText(StrTodayRise);
						pCellSymbol->SetCompareFloat(RespMainMasukura.m_fRise);
					}
				}
				break;
			case CReportScheme::ERH2DaysMasukuraProportion:		// 2������ռ��
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);

					if (bUseMainMasukura)
					{
						CString Str2DaysMasukuraProportion= Float2SymbolString(RespMainMasukura.m_f2DaysMasukuraProportion, 0.0, 2, true);
						pCellSymbol->SetText(Str2DaysMasukuraProportion);
						pCellSymbol->SetCompareFloat(RespMainMasukura.m_f2DaysMasukuraProportion);
					}
				}
				break;
			case CReportScheme::ERH2DaysRanked:					// 2������
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);

					if (bUseMainMasukura)
					{
						CString Str2DaysRanked= Float2String(RespMainMasukura.m_i2DaysRanked, 0, bShowTranslate);
						pCellSymbol->SetText(Str2DaysRanked);
						pCellSymbol->SetCompareFloat(RespMainMasukura.m_i2DaysRanked);
					}
				}
				break;
			case CReportScheme::ERH2DaysRise:					// 2���Ƿ�
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);

					if (bUseMainMasukura)
					{
						CString Str2DaysRise= Float2SymbolString(RespMainMasukura.m_f2DaysRise, 0.0, 2, true);
						pCellSymbol->SetText(Str2DaysRise);
						pCellSymbol->SetCompareFloat(RespMainMasukura.m_f2DaysRise);
					}
				}
				break;
			case CReportScheme::ERH3DaysMasukuraProportion:		// 3������ռ��
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);

					if (bUseMainMasukura)
					{
						CString Str3DaysMasukuraProportion= Float2SymbolString(RespMainMasukura.m_f3DaysMasukuraProportion, 0.0, 2, true);
						pCellSymbol->SetText(Str3DaysMasukuraProportion);
						pCellSymbol->SetCompareFloat(RespMainMasukura.m_f3DaysMasukuraProportion);
					}
				}
				break;
			case CReportScheme::ERH3DaysRanked:					// 3������
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);

					if (bUseMainMasukura)
					{
						CString Str3DaysRanked= Float2String(RespMainMasukura.m_i3DaysRanked, 0, bShowTranslate);
						pCellSymbol->SetText(Str3DaysRanked);
						pCellSymbol->SetCompareFloat(RespMainMasukura.m_i3DaysRanked);
					}
				}
				break;
			case CReportScheme::ERH3DaysRise:					// 3���Ƿ�
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);

					if (bUseMainMasukura)
					{
						CString Str3DaysRise= Float2SymbolString(RespMainMasukura.m_f3DaysRise, 0.0, 2, true);
						pCellSymbol->SetText(Str3DaysRise);
						pCellSymbol->SetCompareFloat(RespMainMasukura.m_f3DaysRise);
					}
				}
				break;
			case CReportScheme::ERH5DaysMasukuraProportion:		// 5������ռ��
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);

					if (bUseMainMasukura)
					{
						CString Str5DaysMasukuraProportion= Float2SymbolString(RespMainMasukura.m_f5DaysMasukuraProportion, 0.0, 2, true);
						pCellSymbol->SetText(Str5DaysMasukuraProportion);
						pCellSymbol->SetCompareFloat(RespMainMasukura.m_f5DaysMasukuraProportion);
					}
				}
				break;
			case CReportScheme::ERH5DaysRanked:					// 5������
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);

					if (bUseMainMasukura)
					{
						CString Str5DaysRanked = Float2String(RespMainMasukura.m_i5DaysRanked, 0, bShowTranslate);
						pCellSymbol->SetText(Str5DaysRanked);
						pCellSymbol->SetCompareFloat(RespMainMasukura.m_i5DaysRanked);
					}
				}
				break;
			case CReportScheme::ERH5DaysRise:					// 5���Ƿ�
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);

					if (bUseMainMasukura)
					{
						CString Str5DaysRise= Float2SymbolString(RespMainMasukura.m_f5DaysRise, 0.0, 2, true);
						pCellSymbol->SetText(Str5DaysRise);
						pCellSymbol->SetCompareFloat(RespMainMasukura.m_f5DaysRise);
					}
				}
				break;
			case CReportScheme::ERH10DaysMasukuraProportion:	// 10������ռ��
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);

					if (bUseMainMasukura)
					{
						CString Str10DaysMasukuraProportion= Float2SymbolString(RespMainMasukura.m_f10DaysMasukuraProportion, 0.0, 2, true);
						pCellSymbol->SetText(Str10DaysMasukuraProportion);
						pCellSymbol->SetCompareFloat(RespMainMasukura.m_f10DaysMasukuraProportion);
					}
				}
				break;
			case CReportScheme::ERH10DaysRanked:				// 10������
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);

					if (bUseMainMasukura)
					{
						CString Str10DaysRanked = Float2String(RespMainMasukura.m_i10DaysRanked, 0, bShowTranslate);
						pCellSymbol->SetText(Str10DaysRanked);
						pCellSymbol->SetCompareFloat(RespMainMasukura.m_i10DaysRanked);
					}
				}
				break;
			case CReportScheme::ERH10DaysRise:					// 10���Ƿ�
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);

					if (bUseMainMasukura)
					{
						CString Str10DaysRise= Float2SymbolString(RespMainMasukura.m_f10DaysRise, 0.0, 2, true);
						pCellSymbol->SetText(Str10DaysRise);
						pCellSymbol->SetCompareFloat(RespMainMasukura.m_f10DaysRise);
					}
				}
				break;
			case CReportScheme::ERHChangeRate:		// ������
				{
					fVal = RealtimePrice.m_fTradeRate;

					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrChangeRate = Float2String(fVal, 2, true);
					pCell->SetText(StrChangeRate);
					pCell->SetCompareFloat(RealtimePrice.m_fTradeRate);
				}
				break;	
			case CReportScheme::ERHMarketWinRate:	// ��ӯ��
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrPeRate = Float2String(RealtimePrice.m_fPeRate, 2, true);
					pCell->SetText(StrPeRate);
					pCell->SetCompareFloat(RealtimePrice.m_fPeRate);
				}
				break;	
			case CReportScheme::ERHVolumeRate:		// ����
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrVolRate = Float2SymbolString(RealtimePrice.m_fVolumeRate, 1.0, 2, bShowTranslate);
					pCellSymbol->SetText(StrVolRate);
					pCellSymbol->SetCompareFloat(RealtimePrice.m_fVolumeRate);
				}
				break;	
			case CReportScheme::ERHSpeedRiseFall:		// �����ǵ�
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);

					CString StrRiseRate = Float2SymbolString(RealtimePrice.m_fRiseRate, 0.0, 2, true);
					pCellSymbol->SetText(StrRiseRate);
					pCellSymbol->SetCompareFloat(RealtimePrice.m_fRiseRate);
				}
				break;	
			case CReportScheme::ERHBuySellRate:			// �����
				{
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					float fBuySellRate = 0.0;
					if ( RealtimePrice.m_fSellVolume != 0.0 )
					{
						fBuySellRate = RealtimePrice.m_fBuyVolume / RealtimePrice.m_fSellVolume;
					}
					CString StrPeRate = Float2String(fBuySellRate, 2, bShowTranslate);
					pCell->SetText(StrPeRate);
					pCell->SetCompareFloat(fBuySellRate);
				}
				break;	
			case CReportScheme::ERHAllCapital:				// �ܹɱ�
				{
					fVal = FinanceData.m_fAllCapical;
					fVal /= 10000.0;

					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrValue = Float2String(fVal, 2, false);
					pCell->SetText(StrValue);
					pCell->SetCompareFloat(fVal);
				}
				break;
			case CReportScheme::ERHCircAsset:				// ��ͨ�ɱ�
				{
					fVal = FinanceData.m_fCircAsset;
					fVal /= 10000.0;

					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrValue = Float2String(fVal, 2, false);
					pCell->SetText(StrValue);
					pCell->SetCompareFloat(fVal);
				}
				break;
			case CReportScheme::ERHAllAsset:				// ���ʲ�
				{
					fVal = FinanceData.m_fAllAsset;
					fVal /= 10000.0;

					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrValue = Float2String(fVal, 2, false);
					pCell->SetText(StrValue);
					pCell->SetCompareFloat(fVal);
				}
				break;
			case CReportScheme::ERHFlowDebt:				// ������ծ
				{
					fVal = FinanceData.m_fFlowDebt;
					fVal /= 10000.0;

					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrValue = Float2String(fVal, 2, false);
					pCell->SetText(StrValue);
					pCell->SetCompareFloat(fVal);
				}
				break;
			case CReportScheme::ERHPerFund:				// ÿ�ɹ�����
				{
					fVal = FinanceData.m_fPerFund;

					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrValue = Float2String(fVal, 2, true);
					pCell->SetText(StrValue);
					pCell->SetCompareFloat(fVal);
				}
				break;
			case CReportScheme::ERHBusinessProfit:				// Ӫҵ����
				{
					fVal = FinanceData.m_fBusinessProfit;
					fVal /= 10000.0;

					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrValue = Float2String(fVal, 2, false);
					pCell->SetText(StrValue);
					pCell->SetCompareFloat(fVal);
				}
				break;
			case CReportScheme::ERHPerNoDistribute:				// ÿ��δ����
				{
					fVal = FinanceData.m_fPerNoDistribute;

					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrValue = Float2String(fVal, 2, true);
					pCell->SetText(StrValue);
					pCell->SetCompareFloat(fVal);
				}
				break;
			case CReportScheme::ERHPerIncomeYear:				// ÿ������(��)
				{
					fVal = FinanceData.m_fPerIncomeYear;

					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrValue = Float2String(fVal, 2, true);
					if ( RealtimePrice.m_uiSeason <= 3 )
					{
						//lint --e(569)
						TCHAR chSeason = _T('��');
						chSeason += (TCHAR)RealtimePrice.m_uiSeason;
						StrValue += chSeason;
					}
					pCell->SetText(StrValue);
					pCell->SetCompareFloat(fVal);
				}
				break;
			case CReportScheme::ERHPerPureAsset:				// ÿ�ɾ��ʲ�
				{
					fVal = FinanceData.m_fPerPureAsset;

					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrValue = Float2String(fVal, 2, true);
					pCell->SetText(StrValue);
					pCell->SetCompareFloat(fVal);
				}
				break;
			case CReportScheme::ERHChPerPureAsset:				// ����ÿ�ɾ��ʲ�
				{
					fVal = FinanceData.m_fChPerPureAsset;

					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrValue = Float2String(fVal, 2, false, false);
					pCell->SetText(StrValue);
					pCell->SetCompareFloat(fVal);
				}
				break;
			case CReportScheme::ERHDorRightRate:				// �ɶ�Ȩ���
				{
					fVal = FinanceData.m_fDorRightRate;

					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrValue = Float2String(fVal, 2, false);
					pCell->SetText(StrValue);
					pCell->SetCompareFloat(fVal);
				}
				break;
			case CReportScheme::ERHCircMarketValue:				// ��ͨ��ֵ
				{
					if ( RealtimePrice.m_fPriceNew == 0.0f )
					{
						fVal = FinanceData.m_fCircAsset * fPrevReferPrice;
					}
					else
					{
						fVal = FinanceData.m_fCircAsset * RealtimePrice.m_fPriceNew;
					}
					fVal /= 10000.0;

					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrValue = Float2String(fVal, 2, false);
					pCell->SetText(StrValue);
					pCell->SetCompareFloat(fVal);
				}
				break;
			case CReportScheme::ERHAllMarketValue:				// ����ֵ
				{
					if ( RealtimePrice.m_fPriceNew == 0.0f )
					{
						fVal = FinanceData.m_fAllCapical * fPrevReferPrice;
					}
					else
					{
						fVal = FinanceData.m_fAllCapical * RealtimePrice.m_fPriceNew;
					}
					fVal /= 10000.0;

					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					CString StrValue = Float2String(fVal, 2, false);
					pCell->SetText(StrValue);
					pCell->SetCompareFloat(fVal);
				}
				break;
			case CReportScheme::ERHPowerDegree:				// ǿ����
				{
					CString StrValue = _T(" -");
					if ( NULL != m_pAbsCenterManager && m_pAbsCenterManager->GetMerchPowerValue(pMerchRow, fVal) )
					{
						StrValue = Float2String(fVal * 100, 2, false, false);
					}
					pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, iCol);
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCell->SetText(StrValue);
					pCell->SetCompareFloat(fVal);
				}
				break;
			default:
				{
					m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
					pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
					pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

					pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	

					float fdefault = 0.0;					
					CString StrValue = Float2SymbolString(fdefault,0.0,1,false);
					pCellSymbol->SetText(StrValue);					
				}
				break;
			}
		}
	}

	// ˢ�±��
	// �����ȫ�����»��߸��µ�һ��Row�ڵ�ǰ��Ļ,�����µ���:
	if ( bUpdateAllVisible )
	{
		bool32 bHasData = m_GridCtrl.GetRowCount() > 1;
		ChangeGridSize(bHasData);

		m_GridCtrl.Refresh();

		SetTimer(KUpdateExcelTimerId, KUpdateExcelTimerPeriod, NULL);
	}
	else
	{
		for ( i = 0; i < m_GridCtrl.GetColumnCount(); i ++ )
		{
			m_GridCtrl.InvalidateCellRect(iUpdateOneRow, i);
		}

		//COLORREF clrRise = GetIoViewColor(ESCRise);
		//COLORREF clrFall = GetIoViewColor(ESCFall);
		//COLORREF clrKeep = GetIoViewColor(ESCKeep);

		//AsyncGridToExcel (&m_GridCtrl, BlockDesc.m_StrBlockName, m_StrAsyncExcelFileName, iUpdateOneRow, clrRise, clrFall, clrKeep);
	}
}

void  CIoViewReport::ExportUserBlockToExcel(CString StrDefaultFileName,CString StrTitle1,CString StrTitle2,CString StrTitle3)
{
	_Application ExcelApp;
	Workbooks workBooks;
	_Workbook workBook;
	Worksheets workSheets;
	_Worksheet workSheet;
	Range CurrRange;

	if (!ExcelApp.CreateDispatch(_T("Excel.Application"), NULL))
	{		
		::MessageBox(NULL, _T("����Excel����ʧ��."), AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return;
	}

	CString StrFileName;
	CTime tmpTime = CTime::GetCurrentTime();
	CString StrTime;
	StrTime.Format(_T("%02d��%d��%d��%dʱ%d��"),tmpTime.GetYear(),tmpTime.GetMonth(),tmpTime.GetDay(),tmpTime.GetHour(),tmpTime.GetMinute());

	StrFileName.Format(_T("%s%s.xls"),StrTitle1.GetBuffer(),StrTime.GetBuffer());

	//�ļ�����·��ѡ��
	CFileDialog dlg(FALSE,_T("xls"),StrFileName,NULL,_T("Microsoft Office Excel������(*.xls)|*.xls||"),NULL);

	if(GetVersion()<0x80000000)
	{
		dlg.m_ofn.lStructSize=88;
	}
	else
	{
		dlg.m_ofn.lStructSize=76;
	}

	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	CString StrFilePath = CPathFactory::GetPrivateExcelPath(pDoc->m_pAbsCenterManager->GetUserName());
	StrFilePath += StrTitle1;	
	StrFilePath += L".xls";

	// �õ�Excel �ļ���·��
	TCHAR TStrFilePath[MAX_PATH];
	lstrcpy(TStrFilePath, StrFilePath);
	_tcheck_if_mkdir(TStrFilePath);

	// ���ݵ�ǰ·��
	TCHAR TStrFilePathBk[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH-1, TStrFilePathBk);

	// ����ΪExcel ·��
	::SetCurrentDirectory(TStrFilePath);
	dlg.m_ofn.lpstrInitialDir = TStrFilePath;

	int32 ret = dlg.DoModal();

	if(ret!=IDOK)
	{
		return;
	}

	// ·�����ȥ
	::SetCurrentDirectory(TStrFilePathBk);
	StrFileName=dlg.GetPathName();

	//����Ϊ��ʾ
	//ExcelApp.SetVisible(TRUE);

	TRY 
	{
		workBooks.AttachDispatch(ExcelApp.GetWorkbooks());
		workBook.AttachDispatch(workBooks.Add(_variant_t(vtMissing)));
		workSheets=workBook.GetSheets();

		workSheet=workSheets.GetItem(COleVariant((short)1));
		//	workSheet.Activate();

		//�õ�ȫ��Cells����ʱ,CurrRange��cells�ļ���
		CurrRange.AttachDispatch(workSheet.GetCells());

		CurrRange.SetItem(_variant_t((long)(1)),_variant_t((long)(1)),_variant_t(StrTitle1));
		CurrRange.SetItem(_variant_t((long)(1)),_variant_t((long)(2)),_variant_t(StrTitle2));
		CurrRange.SetItem(_variant_t((long)(1)),_variant_t((long)(3)),_variant_t(StrTitle3));


		CArray<T_HeadInfo,T_HeadInfo> aReportHeadInfoList;
		CString StrBlockName   = L"�ҵ���ѡ";
		int32 iFixCol = 0;
		E_ReportType eReportType = ERTCustom;
		T_Block* pBlock = CUserBlockManager::Instance()->GetBlock(StrBlockName);
		if ( NULL != pBlock )
		{
			eReportType = pBlock->m_eHeadType;
		}
		else
		{
			return;
		}
		CReportScheme::Instance()->GetReportHeadInfoList(eReportType,aReportHeadInfoList,iFixCol);

		int iLastRow = 0;
		if ( aReportHeadInfoList.GetSize() > 0 )
		{
			//��ͷ
			int32 i, iSize = aReportHeadInfoList.GetSize();
			iSize = min(iSize, sizeof(s_ExcelRowNamesReport)/sizeof(s_ExcelRowNamesReport[0]));
			for ( i = 0; i < iSize; i ++ )
			{
				CString StrText;
				StrText = aReportHeadInfoList.GetAt(i).m_StrHeadNameCn;
				CurrRange.SetItem(_variant_t((long)(2)),_variant_t((long)(i+1)),_variant_t(StrText));
			}

		}


		for (int iRow = 0; iRow < pBlock->m_aMerchs.GetSize(); iRow++)
		{

			CMerch *pMerchRow = pBlock->m_aMerchs.GetAt(iRow);
			if ( NULL == pMerchRow )
			{
				//ASSERT(0);
				continue;
			}



			// �õ����¼۽ṹ
			CRealtimePrice RealtimePrice;
			if (NULL != pMerchRow && NULL != pMerchRow->m_pRealtimePrice)
			{
				RealtimePrice = *pMerchRow->m_pRealtimePrice;
			}

			float fPrevReferPrice = RealtimePrice.m_fPricePrevClose;
			if (ERTFuturesCn == pMerchRow->m_Market.m_MarketInfo.m_eMarketReportType)	// �����ڻ�ʹ���������Ϊ�ο��۸�
			{
				fPrevReferPrice = RealtimePrice.m_fPricePrevAvg;
			}

			// 
			CMerchExtendData ExtendData;
			if ( NULL != pMerchRow )
			{
				if ( NULL != pMerchRow->m_pMerchExtendData )
				{				
					ExtendData = *pMerchRow->m_pMerchExtendData;
				}					
			}	

			// ��������
			CFinanceData	FinanceData;
			if ( NULL != pMerchRow->m_pFinanceData )
			{
				FinanceData = *pMerchRow->m_pFinanceData;
			}


			// ѡ�ɱ�־
			bool32 bUseMerchIndex = false;
			MerchIndexMap::const_iterator it = m_mapMerchIndex.find(pMerchRow);
			T_RespMerchIndex RespMerchIndex;
			if ( it != m_mapMerchIndex.end() )
			{
				bUseMerchIndex = true;
				RespMerchIndex = it->second;
			}

			CStringArray aMerchIndexSelectStr; // ѡ�� ������ ������ ��Բ ����
			aMerchIndexSelectStr.Add(_T("��"));
			aMerchIndexSelectStr.Add(_T("��"));
			aMerchIndexSelectStr.Add(_T("��"));
			aMerchIndexSelectStr.Add(_T("��"));
			CArray<COLORREF, COLORREF> aMerchIndexColors;
			aMerchIndexColors.Add(RGB(250,0,0));
			aMerchIndexColors.Add(RGB(0,255,0));
			aMerchIndexColors.Add(RGB(255,255,0));
			aMerchIndexColors.Add(RGB(0,0,255));
			ASSERT( aMerchIndexColors.GetSize() == aMerchIndexSelectStr.GetSize() && aMerchIndexSelectStr.GetSize() == ESSCount );


			// ...fangz0926 ���� �ɽ�����������ɫ
			//COLORREF ClrCurHold = RGB(170, 170, 0);
			COLORREF ClrCurHold = GetIoViewColor(ESCVolume2);

			float fVal = 0.0;

			bool32 bIsFuture = CReportScheme::IsFuture(GetMerchKind(pMerchRow));
			bool32 bShowTranslate = !bIsFuture;

			// �����б�ͷ��ʾ�����ݣ� ��ʾ���������
			for (int32 iCol = 0; iCol < aReportHeadInfoList.GetSize(); iCol++)
			{
				// ��ͷ��( ���, ����, ����....)
				CReportScheme::E_ReportHeader ERHType;
				ERHType = CReportScheme::Instance()->GetReportHeaderEType(aReportHeadInfoList[iCol].m_StrHeadNameCn);

				// ��Ʒ���� (�����ڻ�, ����֤ȯ, ���...)
				E_ReportType eMerchKind = GetMerchKind(pMerchRow);	


				//
				CString  strDisData;
				COLORREF colDis = GetIoViewColor(ESCText);

				switch ( ERHType )
				{
				case CReportScheme::ERHRowNo:
					{
						strDisData.Format(L"%d", iRow+1);
						colDis = GetIoViewColor(ESCText);
					}
					break;
				case CReportScheme::ERHMerchCode:
					{		
						strDisData = pMerchRow->m_MerchInfo.m_StrMerchCode;
						colDis = GetIoViewColor(ESCText);
					}
					break;
				case CReportScheme::ERHMerchName:
					{
						strDisData = pMerchRow->m_MerchInfo.m_StrMerchCnName;
						colDis = COLO_NO_NAME_CODE;
					}
					break;
				case CReportScheme::ERHPricePrevClose:			
					{			
						CString StrPricePrevClose = L"";
						StrPricePrevClose = Float2SymbolString(RealtimePrice.m_fPricePrevClose, RealtimePrice.m_fPricePrevClose, pMerchRow->m_MerchInfo.m_iSaveDec);	// ������ʾ��ƽ

						strDisData = StrPricePrevClose;
						//colDis = GetIoViewColor(ESCVolume2)

					}
					break;
				case CReportScheme::ERHPricePrevBalance:
					{
						// �����
						if (CReportScheme::IsFuture(eMerchKind))
						{
							BUY_SELL_PLATE nType = CConfigInfo::Instance()->m_nBuySellPlate;
							if ((ERTMony==eMerchKind) && (YESTERDAY_CLOSE==nType))// ����ǡ������̡�,�������ó�������
							{
								CString StrPriceClose = Float2SymbolString(RealtimePrice.m_fPricePrevClose, RealtimePrice.m_fPricePrevClose, pMerchRow->m_MerchInfo.m_iSaveDec);
								strDisData = StrPriceClose;
								//colDis = GetIoViewColor(ESCVolume2)
							}
							else
							{
								CString StrPricePrevBlance = Float2SymbolString(RealtimePrice.m_fPricePrevAvg, RealtimePrice.m_fPricePrevAvg, pMerchRow->m_MerchInfo.m_iSaveDec);
								strDisData = StrPricePrevBlance;
								//colDis = GetIoViewColor(ESCVolume2)
							}	
						}
						else
						{
							strDisData = L"--";
						}					
					}
					break;
				case CReportScheme::ERHPriceOpen:
					{
						CString StrPriceOpen = L"";
						StrPriceOpen = Float2SymbolString(RealtimePrice.m_fPriceOpen, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);
						strDisData = StrPriceOpen;
					}
					break;
				case CReportScheme::ERHPriceNew:
					{
						CString StrPriceNew = L"";
						StrPriceNew = Float2SymbolString(RealtimePrice.m_fPriceNew, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);
						strDisData = StrPriceNew;	
					}
					break;
				case CReportScheme::ERHPriceBalance:			// �����: ����ľ���
					{					
						CString StrPriceAvg = L"";
						StrPriceAvg = Float2SymbolString(RealtimePrice.m_fPriceAvg, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);
						strDisData = StrPriceAvg;	
					}
					break;
				case CReportScheme::ERHPriceHigh:
					{
						CString StrPriceHigh = L"";
						StrPriceHigh = Float2SymbolString(RealtimePrice.m_fPriceHigh, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);	
						strDisData = StrPriceHigh;	
					}
					break;
				case CReportScheme::ERHPriceLow:
					{
						CString StrPriceLow = L"";
						StrPriceLow = Float2SymbolString(RealtimePrice.m_fPriceLow, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);
						strDisData = StrPriceLow;	
					}
					break;
				case CReportScheme::ERHPriceSell5:
					{
						CString StrPriceNow = L"";
						StrPriceNow = Float2SymbolString(RealtimePrice.m_astSellPrices[4].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);
						strDisData = StrPriceNow;	
					}
					break;
				case CReportScheme::ERHPriceSell4:
					{
						CString StrPriceNow = L"";
						StrPriceNow = Float2SymbolString(RealtimePrice.m_astSellPrices[3].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);
						strDisData = StrPriceNow;	
					}
					break;
				case CReportScheme::ERHPriceSell3:
					{
						CString StrPriceNow = L"";
						StrPriceNow = Float2SymbolString(RealtimePrice.m_astSellPrices[2].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);
						strDisData = StrPriceNow;
					}
					break;
				case CReportScheme::ERHPriceSell2:
					{
						CString StrPriceNow = L"";
						StrPriceNow = Float2SymbolString(RealtimePrice.m_astSellPrices[1].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);
						strDisData = StrPriceNow;					
					}
					break;
				case CReportScheme::ERHPriceSell1:
					{
						CString StrPriceNow = L"";
						StrPriceNow = Float2SymbolString(RealtimePrice.m_astSellPrices[0].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);
						strDisData = StrPriceNow;	
					}
					break;
				case CReportScheme::ERHPriceBuy5:
					{
						CString StrPriceNow = L"";
						StrPriceNow = Float2SymbolString(RealtimePrice.m_astBuyPrices[4].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);
						strDisData = StrPriceNow;	
					}
					break;
				case CReportScheme::ERHPriceBuy4:
					{
						CString StrPriceNow = L"";
						StrPriceNow = Float2SymbolString(RealtimePrice.m_astBuyPrices[3].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);
						strDisData = StrPriceNow;	
					}
					break;
				case CReportScheme::ERHPriceBuy3:
					{
						CString StrPriceNow = L"";
						StrPriceNow = Float2SymbolString(RealtimePrice.m_astBuyPrices[2].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);
						strDisData = StrPriceNow;	
					}
					break;
				case CReportScheme::ERHPriceBuy2:
					{
						CString StrPriceNow = L"";
						StrPriceNow = Float2SymbolString(RealtimePrice.m_astBuyPrices[1].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);
						strDisData = StrPriceNow;	
					}
					break;
				case CReportScheme::ERHPriceBuy1:
					{
						CString StrPriceNow = L"";
						StrPriceNow = Float2SymbolString(RealtimePrice.m_astBuyPrices[0].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);
						strDisData = StrPriceNow;	
					}
					break;
				case CReportScheme::ERHVolSell5:
					{			
						CString StrVolumeCur = Float2String(RealtimePrice.m_astSellPrices[4].m_fVolume, 0, bShowTranslate);
						strDisData = StrVolumeCur;
						colDis     = ClrCurHold;
					}
					break;
				case CReportScheme::ERHVolSell4:
					{			
						CString StrVolumeCur = Float2String(RealtimePrice.m_astSellPrices[3].m_fVolume, 0, bShowTranslate);
						strDisData = StrVolumeCur;
						colDis     = ClrCurHold;
					}
					break;
				case CReportScheme::ERHVolSell3:
					{			
						CString StrVolumeCur = Float2String(RealtimePrice.m_astSellPrices[2].m_fVolume, 0, bShowTranslate);
						strDisData = StrVolumeCur;
						colDis     = ClrCurHold;
					}
					break;
				case CReportScheme::ERHVolSell2:
					{			
						CString StrVolumeCur = Float2String(RealtimePrice.m_astSellPrices[1].m_fVolume, 0, bShowTranslate);
						strDisData = StrVolumeCur;
						colDis     = ClrCurHold;
					}
					break;
				case CReportScheme::ERHVolSell1:
					{			
						CString StrVolumeCur = Float2String(RealtimePrice.m_astSellPrices[0].m_fVolume, 0, bShowTranslate);
						strDisData = StrVolumeCur;
						colDis     = ClrCurHold;
					}
					break;
				case CReportScheme::ERHVolBuy5:
					{			
						CString StrVolumeCur = Float2String(RealtimePrice.m_astBuyPrices[4].m_fVolume, 0, bShowTranslate);
						strDisData = StrVolumeCur;
						colDis     = ClrCurHold;
					}
					break;
				case CReportScheme::ERHVolBuy4:
					{			
						CString StrVolumeCur = Float2String(RealtimePrice.m_astBuyPrices[3].m_fVolume, 0, bShowTranslate);
						strDisData = StrVolumeCur;
						colDis     = ClrCurHold;
					}
					break;
				case CReportScheme::ERHVolBuy3:
					{			
						CString StrVolumeCur = Float2String(RealtimePrice.m_astBuyPrices[2].m_fVolume, 0, bShowTranslate);
						strDisData = StrVolumeCur;
						colDis     = ClrCurHold;
					}
					break;
				case CReportScheme::ERHVolBuy2:
					{				
						CString StrVolumeCur = Float2String(RealtimePrice.m_astBuyPrices[1].m_fVolume, 0, bShowTranslate);
						strDisData = StrVolumeCur;
						colDis     = ClrCurHold;
					}
					break;
				case CReportScheme::ERHVolBuy1:
					{			
						CString StrVolumeCur = Float2String(RealtimePrice.m_astBuyPrices[0].m_fVolume, 0, bShowTranslate);
						strDisData = StrVolumeCur;
						colDis     = ClrCurHold;
					}
					break;
				case CReportScheme::ERHVolumeCur:
					{				
						CString StrVolumeCur = Float2String(RealtimePrice.m_fVolumeCur, 0, bShowTranslate);
						strDisData = StrVolumeCur;
						colDis     = ClrCurHold;
					}
					break;
				case CReportScheme::ERHVolumeTotal:
					{				
						CString StrVolumeTotal = Float2String(RealtimePrice.m_fVolumeTotal, 0, bShowTranslate);
						strDisData = StrVolumeTotal;
						colDis     = ClrCurHold;
					}
					break;
				case CReportScheme::ERHAmount:			
					{
						CString StrAmount = Float2String(RealtimePrice.m_fAmountTotal, 0, true);	// �������xx��
						strDisData = StrAmount;
						colDis     = GetIoViewColor(ESCAmount);
					}
					break;
				case CReportScheme::ERHRiseFall:			// �ǵ�
					{
						CString StrRiseValue = L" -";
						if (0. != RealtimePrice.m_fPriceNew)
						{
							//StrRiseValue = Float2SymbolString(RealtimePrice.m_fPriceNew - fPrevReferPrice, 0, pMerchRow->m_MerchInfo.m_iSaveDec);	
							StrRiseValue = Float2SymbolString(RealtimePrice.m_fPriceNew - fPrevReferPrice, 0, pMerchRow->m_MerchInfo.m_iSaveDec, false, false);	
						}
						else if ( RealtimePrice.m_astBuyPrices[0].m_fPrice != 0.0f
							&& fPrevReferPrice != 0.0f )
						{
							// ��һ�ۼ����ǵ�
							StrRiseValue = Float2SymbolString(RealtimePrice.m_astBuyPrices[0].m_fPrice - fPrevReferPrice, 0, pMerchRow->m_MerchInfo.m_iSaveDec, false, false);	
						}
						strDisData = StrRiseValue;
					}
					break;
				case CReportScheme::ERHIndustry:
					{
						CString StrIndustry = L"";
						// �õ����������Ϣ
						CBlockCollection::BlockArray aBlocks;
						CBlockConfig::Instance()->GetBlocksByMerch(pMerchRow->m_MerchInfo.m_iMarketId, pMerchRow->m_MerchInfo.m_StrMerchCode, aBlocks);

						bool32 bFind = false;

						for ( int32 i = 0; i < aBlocks.GetSize(); i++ )
						{
							if ( aBlocks[i]->m_blockCollection.m_StrName == L"��ҵ���" )
							{
								StrIndustry = aBlocks[i]->m_blockInfo.m_StrBlockName;
								bFind = true;
							}
						}

						if ( !bFind )
						{
							StrIndustry.Empty();
						}
						strDisData = StrIndustry; 
						colDis     = GetIoViewColor(ESCText);
					}
					break;
				case CReportScheme::ERHZixuan:
					{
						///////////////////////////////////////////////////////////
						// ������ѡ���ˣ�����ʾ��ѡ�ɱ�־

						/*m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellImage));
						CGridCellImage *pImgCell = NULL;
						pImgCell = (CGridCellImage *)m_GridCtrl.GetCell(iRow, iCol);

						if( T_BlockDesc::EBTUser != eType)
						{
						T_Block* pBlock = CUserBlockManager::Instance()->GetBlock(pMerchRow);
						if ( NULL != pBlock )
						{
						pImgCell->SetImage(IDR_PNG_REPORT_ZIXUAN);
						break;
						}						
						}

						pImgCell->SetImage(IDR_PNG_REPORT_ADDZIXUAN, 3, CGridCellImage::ECSAddZiXuan);*/
					}
					break;
				case CReportScheme::ERHRange:				// �Ƿ�%
					{
						CString StrPriceRisePercent = L" -";
						float   fRisePercent = 0.0;

						if (0. != RealtimePrice.m_fPriceNew && 0. != fPrevReferPrice)
						{
							fRisePercent = ((RealtimePrice.m_fPriceNew - fPrevReferPrice) / fPrevReferPrice) * 100.;
							StrPriceRisePercent = Float2SymbolString(fRisePercent, 0, 2, false, false);
						}
						else if ( RealtimePrice.m_astBuyPrices[0].m_fPrice != 0.0f
							&& fPrevReferPrice != 0.0f )
						{
							// ��һ�ۼ����ǵ�
							float fPriceNew = RealtimePrice.m_astBuyPrices[0].m_fPrice;
							fRisePercent = ((fPriceNew - fPrevReferPrice) / fPrevReferPrice) * 100.;
							StrPriceRisePercent = Float2SymbolString(fRisePercent, 0, 2, false, false);	
						}

						strDisData = StrPriceRisePercent; 
					}
					break;
				case CReportScheme::ERHSwing:				// ���
					{
						CString StrValue = L" -";
						float fValue = 0.;

						if ( 0. != RealtimePrice.m_fPriceHigh && 0. != RealtimePrice.m_fPriceLow && 0. != fPrevReferPrice)
						{
							fValue = (RealtimePrice.m_fPriceHigh - RealtimePrice.m_fPriceLow) * 100.0f / fPrevReferPrice;
							StrValue = Float2SymbolString(fValue, fValue, 2, false, false, false);
						}	

						strDisData = StrValue; 
					}
					break;
				case CReportScheme::ERHTime:				// ʱ��
					{
						CTime TimeCurrent(RealtimePrice.m_TimeCurrent.m_Time.GetTime());
						CString StrTime(_T("-"));
						if ( TimeCurrent.GetTime() != 0 )
						{
							StrTime.Format(L"%02d:%02d:%02d", 
								/*TimeCurrent.GetYear(), TimeCurrent.GetMonth(), TimeCurrent.GetDay(),*/
								TimeCurrent.GetHour(), TimeCurrent.GetMinute(), TimeCurrent.GetSecond());
						}

						strDisData = StrTime; 
					}
					break;
				case CReportScheme::ERHBuyPrice:			// �����(��1)
					{

						CString StrPriceBuy1 = L"";
						StrPriceBuy1 = Float2SymbolString(RealtimePrice.m_astBuyPrices[0].m_fPrice , fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);
						strDisData = StrPriceBuy1; 
					}
					break;
				case CReportScheme::ERHSellPrice:			// ������(��1)
					{
						CString StrPriceSell1 = L"";
						StrPriceSell1 = Float2SymbolString(RealtimePrice.m_astSellPrices[0].m_fPrice, fPrevReferPrice, pMerchRow->m_MerchInfo.m_iSaveDec);
						strDisData = StrPriceSell1; 
					}
					break;
				case CReportScheme::ERHBuyAmount:			// ������(��1)
					{
						CString StrVolumeBuy1 = Float2String(RealtimePrice.m_astBuyPrices[0].m_fVolume, 0, bShowTranslate);
						strDisData = StrVolumeBuy1; 
						colDis     = GetIoViewColor(ESCVolume);
					}
					break;
				case CReportScheme::ERHSellAmount:			// ������(��1)
					{
						CString StrVolumeSell1 = Float2String(RealtimePrice.m_astSellPrices[0].m_fVolume, 0, bShowTranslate);
						strDisData = StrVolumeSell1; 
						colDis     = GetIoViewColor(ESCVolume);
					}
					break;
				case CReportScheme::ERHRate:			  // ί��=��(ί������-ί������)��(ί������+ί������)����100%
					{
						float fRate = 0.0;
						int32 iBuyVolums  = 0;
						int32 iSellVolums = 0;

						for ( int32 i = 0 ; i < 5 ; i++)
						{
							iBuyVolums  += (int32)(RealtimePrice.m_astBuyPrices[i].m_fVolume);
							iSellVolums += (int32)(RealtimePrice.m_astSellPrices[i].m_fVolume);
						}

						if ( 0 != (iBuyVolums + iSellVolums) )
						{
							fRate = (float)(iBuyVolums - iSellVolums)*(float)100 / (iBuyVolums + iSellVolums);
						}

						CString StrRate = Float2SymbolString(fRate, 0.0, 2, false, true, false);
						strDisData = StrRate;
					}
					break;
				case CReportScheme::ERHHold:				// �ֲ�
					{										
						if (CReportScheme::IsFuture(eMerchKind))
						{
							CString StrHoldTotal = Float2String(RealtimePrice.m_fHoldTotal, 0, bShowTranslate);									
							strDisData = StrHoldTotal;
							colDis     =  ClrCurHold;
						}
						else
						{
							strDisData = L"-";
							colDis     =  ClrCurHold;
						}
					}
					break;				
				case CReportScheme::ERHDifferenceHold:		// �ֲֲ�(������=�ֲ���-��ֲ�)
					{					
						if (CReportScheme::IsFuture(eMerchKind))
						{
							CString StrDifference = Float2String(RealtimePrice.m_fHoldTotal - RealtimePrice.m_fHoldPrev, 0, bShowTranslate);
							strDisData = StrDifference;
							colDis     =  ClrCurHold;
						}
						else
						{
							strDisData = L"-";
							colDis     =  ClrCurHold;
						}
					}
					break;
				case CReportScheme::ERHBuild:			 // �տ���=���ɽ���+�����֣�/2
					{	
						float fAddPerDay = RealtimePrice.m_fHoldTotal - RealtimePrice.m_fHoldPrev;
						float fBuild = ( RealtimePrice.m_fVolumeTotal + fAddPerDay ) / 2;
						CString StrBuild = Float2String(fBuild, 0, bShowTranslate);
						if (CReportScheme::IsFuture(eMerchKind))
						{
							strDisData = StrBuild;
							colDis     =  ClrCurHold;
						}
						else
						{
							strDisData = L"-";
							colDis     =  ClrCurHold;
						}
					}
					break;
				case CReportScheme::ERHClear:			// ��ƽ��=���ɽ���-�����֣�/2
					{					
						float fAddPerDay = RealtimePrice.m_fHoldTotal - RealtimePrice.m_fHoldPrev;
						float fClear = ( RealtimePrice.m_fVolumeTotal - fAddPerDay ) / 2;
						// pCell->SetDefaultTextColor(ESCVolume);
						CString StrClear = Float2String(fClear, 0, bShowTranslate);
						if (CReportScheme::IsFuture(eMerchKind))
						{
							strDisData = StrClear;
							colDis     =  ClrCurHold;
						}
						else
						{
							strDisData = L"-";
							colDis     =  ClrCurHold;
						}

					}
					break;
				case CReportScheme::ERHAddPer:			// ��������=�ֲ���-ǰһ�ʳֲ���
					{										
						// pCell->SetDefaultTextColor(ESCVolume);
						CString StrAddPer = Float2String( RealtimePrice.m_fHoldCur, 0, bShowTranslate);

						if (CReportScheme::IsFuture(eMerchKind))
						{
							strDisData = StrAddPer;
							colDis     =  ClrCurHold;
						}
						else
						{
							strDisData = L"-";
							colDis     =  ClrCurHold;
						}

					}
					break;
				case CReportScheme::ERHBuildPer:		// ���ʿ���=������+�������֣�/2
					{
						float fBuildPer = (RealtimePrice.m_fVolumeCur + RealtimePrice.m_fHoldCur) / 2;
						// pCell->SetDefaultTextColor(ESCVolume);
						CString StrBuildPer = Float2String( fBuildPer, 0, bShowTranslate);

						if (CReportScheme::IsFuture(eMerchKind))
						{
							strDisData = StrBuildPer;
							colDis     =  ClrCurHold;
						}
						else
						{
							strDisData = L"-";
							colDis     =  ClrCurHold;
						}

					}
					break;
				case CReportScheme::ERHClearPer:		// ����ƽ��=������-�������֣�/2
					{
						float fClearPer = (RealtimePrice.m_fVolumeCur - RealtimePrice.m_fHoldCur) / 2;
						// pCell->SetDefaultTextColor(ESCVolume);
						CString StrClearPer = Float2String( fClearPer, 0, bShowTranslate);

						if (CReportScheme::IsFuture(eMerchKind))
						{
							strDisData = StrClearPer;
							colDis     =  ClrCurHold;
						}
						else
						{
							strDisData = L"-";
							colDis     =  ClrCurHold;
						}					
					}
					break;
				case CReportScheme::ERHBuyVolumn:
					{
						CString StrBuyVolume = Float2String(RealtimePrice.m_fBuyVolume, 0, bShowTranslate);
						strDisData = StrBuyVolume;
						colDis     =  GetIoViewColor(ESCVolume);
					}
					break;
				case CReportScheme::ERHSellVolumn:
					{
						CString StrSellVolume = Float2String(RealtimePrice.m_fSellVolume, 0, bShowTranslate);
						strDisData = StrSellVolume;
						colDis     =  GetIoViewColor(ESCVolume);
					}
					break;		
				case CReportScheme::ERHShortLineSelect:		// ����ѡ�� ������ ������ ��Բ ����
					{
						//////////////////////////////////////////////////////////
						// �ҵ���ѡû�ж���ѡ���ֶ�

						/*m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellImage));
						CGridCellImage *pImgCell = NULL;
						pImgCell = (CGridCellImage *)m_GridCtrl.GetCell(iRow, iCol);
						pImgCell->SetState(pImgCell->GetState() | GVIS_SHOWTIPS);

						if (bUseMerchIndex)
						{
						if (ESS0 == RespMerchIndex.m_usShort)
						{
						pImgCell->SetImage(IDR_PNG_REPORT_B);
						}
						else if (ESS1 == RespMerchIndex.m_usShort)
						{
						pImgCell->SetImage(IDR_PNG_REPORT_S);
						}
						else if (ESS2 == RespMerchIndex.m_usShort)
						{
						pImgCell->SetImage(IDR_PNG_REPORT_H);
						}
						else if (ESS3 == RespMerchIndex.m_usShort)
						{
						pImgCell->SetImage(IDR_PNG_REPORT_U);
						}*/

						/*if (RespMerchIndex.m_usShort >= ESS0 && RespMerchIndex.m_usShort <= ESS3)
						{
						pImgCell->SetTipText(s_KaChooseStockTips[RespMerchIndex.m_usShort]);
						}*/

						strDisData = L"";
						if (RespMerchIndex.m_usShort >= ESS0 && RespMerchIndex.m_usShort < ESSCount)
						{
							strDisData = s_KaChooseStockTips[RespMerchIndex.m_usShort];
						}

						//}
					}
					break;	
				case CReportScheme::ERHMidLineSelect:		// ����ѡ��
					{
						//////////////////////////////////////////////////////////
						// �ҵ���ѡû�ж���ѡ���ֶ�

						/*m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellImage));
						CGridCellImage *pImgCell = NULL;
						pImgCell = (CGridCellImage *)m_GridCtrl.GetCell(iRow, iCol);
						pImgCell->SetState(pImgCell->GetState() | GVIS_SHOWTIPS);

						if (bUseMerchIndex)
						{
						if (ESS0 == RespMerchIndex.m_usMid)
						{
						pImgCell->SetImage(IDR_PNG_REPORT_B);
						}
						else if (ESS1 == RespMerchIndex.m_usMid)
						{
						pImgCell->SetImage(IDR_PNG_REPORT_S);
						}
						else if (ESS2 == RespMerchIndex.m_usMid)
						{
						pImgCell->SetImage(IDR_PNG_REPORT_H);
						}
						else if (ESS3 == RespMerchIndex.m_usMid)
						{
						pImgCell->SetImage(IDR_PNG_REPORT_U);
						}

						if (RespMerchIndex.m_usMid >= ESS0 && RespMerchIndex.m_usMid <= ESS3)
						{
						pImgCell->SetTipText(s_KaChooseStockTips[RespMerchIndex.m_usMid]);
						}
						}*/

						strDisData = L"";
						if (RespMerchIndex.m_usMid >= ESS0 && RespMerchIndex.m_usMid < ESSCount)
						{
							strDisData = s_KaChooseStockTips[RespMerchIndex.m_usMid];
						}
					}
					break;	
				case CReportScheme::ERHCapitalFlow:		// �ʽ�����
					{
						CString StrCapticalFlow = Float2SymbolString(RealtimePrice.m_fCapticalFlow, 0.0, 2, true);
						strDisData = StrCapticalFlow;
					}
					break;	
				case CReportScheme::ERHExBigNetAmount:		// ���󵥾�����(��Ԫ)
					{
						CString StrExBigNetAmount = Float2SymbolString(RespMerchIndex.m_fExBigNetAmount, 0.0, 2, true);
						strDisData = StrExBigNetAmount;
					}
					break;
				case CReportScheme::ERHBigNetAmount:		// �󵥾�����(��Ԫ)
					{
						CString StrBigNetAmount = Float2SymbolString(RespMerchIndex.m_fBigNetAmount, 0.0, 2, true);
						strDisData = StrBigNetAmount;
					}
					break;
				case CReportScheme::ERHMidNetAmount:		// �е�������(��Ԫ)
					{
						CString StrMidNetAmount = Float2SymbolString(RespMerchIndex.m_fMidNetAmount, 0.0, 2, true);
						strDisData = StrMidNetAmount;
					}
					break;
				case CReportScheme::ERHSmallNetAmount:	    // С��������(��Ԫ)
					{
						CString StrSmallNetAmount = Float2SymbolString(RespMerchIndex.m_fSmallNetAmount, 0.0, 2, true);
						strDisData = StrSmallNetAmount;
					}
					break;
				case CReportScheme::ERHChangeRate:		// ������
					{
						fVal = RealtimePrice.m_fTradeRate;
						CString StrChangeRate = Float2String(fVal, 2, true);
						strDisData = StrChangeRate;
					}
					break;	
				case CReportScheme::ERHMarketWinRate:	// ��ӯ��
					{
						CString StrPeRate = Float2String(RealtimePrice.m_fPeRate, 2, true);
						strDisData = StrPeRate;
					}
					break;	
				case CReportScheme::ERHVolumeRate:		// ����
					{
						CString StrVolRate = Float2SymbolString(RealtimePrice.m_fVolumeRate, 1.0, 2, bShowTranslate);
						strDisData = StrVolRate;
					}
					break;	
				case CReportScheme::ERHSpeedRiseFall:		// �����ǵ�
					{
						CString StrRiseRate = Float2SymbolString(RealtimePrice.m_fRiseRate, 0.0, 2, true);
						strDisData = StrRiseRate;
					}
					break;	
				case CReportScheme::ERHBuySellRate:			// �����
					{
						float fBuySellRate = 0.0;
						if ( RealtimePrice.m_fSellVolume != 0.0 )
						{
							fBuySellRate = RealtimePrice.m_fBuyVolume / RealtimePrice.m_fSellVolume;
						}
						CString StrPeRate = Float2String(fBuySellRate, 2, bShowTranslate);
						strDisData = StrPeRate;
					}
					break;	
				case CReportScheme::ERHAllCapital:				// �ܹɱ�
					{
						fVal = FinanceData.m_fAllCapical;
						fVal /= 10000.0;
						CString StrValue = Float2String(fVal, 2, false);
						strDisData = StrValue;
					}
					break;
				case CReportScheme::ERHCircAsset:				// ��ͨ�ɱ�
					{
						fVal = FinanceData.m_fCircAsset;
						fVal /= 10000.0;
						CString StrValue = Float2String(fVal, 2, false);
						strDisData = StrValue;
					}
					break;
				case CReportScheme::ERHAllAsset:				// ���ʲ�
					{
						fVal = FinanceData.m_fAllAsset;
						fVal /= 10000.0;

						CString StrValue = Float2String(fVal, 2, false);
						strDisData = StrValue;
					}
					break;
				case CReportScheme::ERHFlowDebt:				// ������ծ
					{
						fVal = FinanceData.m_fFlowDebt;
						fVal /= 10000.0;

						CString StrValue = Float2String(fVal, 2, false);
						strDisData = StrValue;
					}
					break;
				case CReportScheme::ERHPerFund:				// ÿ�ɹ�����
					{
						fVal = FinanceData.m_fPerFund;

						CString StrValue = Float2String(fVal, 2, true);
						strDisData = StrValue;
					}
					break;
				case CReportScheme::ERHBusinessProfit:				// Ӫҵ����
					{
						fVal = FinanceData.m_fBusinessProfit;
						fVal /= 10000.0;
						CString StrValue = Float2String(fVal, 2, false);
						strDisData = StrValue;
					}
					break;
				case CReportScheme::ERHPerNoDistribute:				// ÿ��δ����
					{
						fVal = FinanceData.m_fPerNoDistribute;

						CString StrValue = Float2String(fVal, 2, true);
						strDisData = StrValue;
					}
					break;
				case CReportScheme::ERHPerIncomeYear:				// ÿ������(��)
					{
						fVal = FinanceData.m_fPerIncomeYear;

						CString StrValue = Float2String(fVal, 2, true);
						if ( RealtimePrice.m_uiSeason <= 3 )
						{
							//lint --e(569)
							TCHAR chSeason = _T('��');
							chSeason += (TCHAR)RealtimePrice.m_uiSeason;
							StrValue += chSeason;
						}
						strDisData = StrValue;
					}
					break;
				case CReportScheme::ERHPerPureAsset:				// ÿ�ɾ��ʲ�
					{
						fVal = FinanceData.m_fPerPureAsset;

						CString StrValue = Float2String(fVal, 2, true);
						strDisData = StrValue;
					}
					break;
				case CReportScheme::ERHChPerPureAsset:				// ����ÿ�ɾ��ʲ�
					{
						fVal = FinanceData.m_fChPerPureAsset;

						CString StrValue = Float2String(fVal, 2, false, false);
						strDisData = StrValue;
					}
					break;
				case CReportScheme::ERHDorRightRate:				// �ɶ�Ȩ���
					{
						fVal = FinanceData.m_fDorRightRate;

						CString StrValue = Float2String(fVal, 2, false);
						strDisData = StrValue;
					}
					break;
				case CReportScheme::ERHCircMarketValue:				// ��ͨ��ֵ
					{
						if ( RealtimePrice.m_fPriceNew == 0.0f )
						{
							fVal = FinanceData.m_fCircAsset * fPrevReferPrice;
						}
						else
						{
							fVal = FinanceData.m_fCircAsset * RealtimePrice.m_fPriceNew;
						}
						fVal /= 10000.0;

						CString StrValue = Float2String(fVal, 2, false);
						strDisData = StrValue;
					}
					break;
				case CReportScheme::ERHAllMarketValue:				// ����ֵ
					{
						if ( RealtimePrice.m_fPriceNew == 0.0f )
						{
							fVal = FinanceData.m_fAllCapical * fPrevReferPrice;
						}
						else
						{
							fVal = FinanceData.m_fAllCapical * RealtimePrice.m_fPriceNew;
						}
						fVal /= 10000.0;

						CString StrValue = Float2String(fVal, 2, false);
						strDisData = StrValue;
					}
					break;
				case CReportScheme::ERHPowerDegree:				// ǿ����
					{
						CString StrValue = _T(" -");
						if ( NULL != m_pAbsCenterManager && m_pAbsCenterManager->GetMerchPowerValue(pMerchRow, fVal) )
						{
							StrValue = Float2String(fVal * 100, 2, false, false);
						}
						strDisData = StrValue;
					}
					break;
				default:
					{
						float fdefault = 0.0;					
						CString StrValue = Float2SymbolString(fdefault,0.0,1,false);
						strDisData = StrValue;
					}
					break;
				}


				//int32 iRowCell = 1;

				//if ( j >= sizeof(s_ExcelRowNames)/sizeof(s_ExcelRowNames[0]) )
				//{
				//break;	// ��̫���ˣ��޷�����
				//}
				//CGridCellBase* pCell = pGridCtrl->GetCell(i,j);
				CString StrText = strDisData;
				COLORREF clr =  RGB(0,0,0);//pCell->GetTextClr();

				if ( StrText.Find(_T("-")) == 0 )
				{
					clr = RGB(0,127,0);	// ��
				}
				else if ( StrText.Find(_T("+")) == 0 )
				{
					clr = RGB(255,0,0);	// ��
				}
				StrText.TrimLeft(_T("-+"));

				//StrText.TrimLeft(_T("-+"));
				CurrRange.SetItem(_variant_t((long)(iRow+3)),_variant_t((long)(iCol+1)),_variant_t(StrText));
				CString StrCell;
				StrCell.Format(_T("%s%d"),CString(s_ExcelRowNamesReport[iCol]).GetBuffer(),iRow+3);
				Range cell(CurrRange.GetRange(_variant_t(StrCell),_variant_t(StrCell)));
				FontExecl font(cell.GetFont());
				font.SetColor(_variant_t((long)clr));
			}
		}
	}
	CATCH_ALL (e)
	{
		// �쳣���رգ���ʾ����
		workBook.Close(_variant_t(true),_variant_t(StrFileName),_variant_t(false));
		workBooks.Close();

		ExcelApp.Quit();

		ExcelApp.ReleaseDispatch();
		workBooks.ReleaseDispatch();
		workBook.ReleaseDispatch();
		workSheets.ReleaseDispatch();
		workSheet.ReleaseDispatch();
		CurrRange.ReleaseDispatch();

		CString StrErr;
		if ( e->GetErrorMessage(StrErr.GetBuffer(1024), 1024) )
		{
		}
		StrErr.ReleaseBuffer();
		if ( StrErr.IsEmpty() )
		{
			StrErr = _T("�ǳ���Ǹ������δ֪��ԭ����excel��������ʧ��");
		}
		AfxMessageBox(StrErr, MB_OK|MB_ICONERROR);
		return;
	}
	END_CATCH_ALL

		workBook.Close(_variant_t(true),_variant_t(StrFileName),_variant_t(false));
	workBooks.Close();

	ExcelApp.Quit();

	ExcelApp.ReleaseDispatch();
	workBooks.ReleaseDispatch();
	workBook.ReleaseDispatch();
	workSheets.ReleaseDispatch();
	workSheet.ReleaseDispatch();
	CurrRange.ReleaseDispatch();

	::MessageBox(NULL, _T("����EXCEL�ɹ�."), AfxGetApp()->m_pszAppName, MB_OK);	
}

void CIoViewReport::OnEsc()
{
	if ( m_iPreTab < 0 || m_iPreTab >= m_GuiTabWnd.GetCount() )
	{
		m_iPreTab = 0;
	}

	//SetTab(m_iPreTab);

	// �����е�tab[block]֮���л�
	if ( m_iCurTab >=0 && m_iCurTab < m_aTabInfomations.GetSize() )
	{
		m_iCurTab = (++m_iCurTab) % m_aTabInfomations.GetSize();
		SetTab(m_iCurTab);	// Tab�ؼ��л�

		if ( m_iCurTab >= KiFixTabCounts-1 )
		{
			TabChange();		// ����block�л�
		}
	}
}

bool32 CIoViewReport::GetStdMenuEnable(MSG* pMsg)
{
	return false;
}

void CIoViewReport::LockRedraw()
{
	if( m_bLockRedraw )
	{
		return;
	}

	m_bLockRedraw = true;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
	::SendMessage(m_GridCtrl.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
	if (m_bShowCustomGrid)
	{
		::SendMessage(m_CustomGridCtrl.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
	}
}

void CIoViewReport::UnLockRedraw()
{
	if ( !m_bLockRedraw )
	{
		return;
	}

	bool32 bVisiable = IsWindowVisible();

	m_bLockRedraw = false;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
	::SendMessage(m_GridCtrl.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
	if (m_bShowCustomGrid)
	{
		::SendMessage(m_CustomGridCtrl.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
	}
	UpdateTableContent(TRUE, NULL, FALSE);

	if ( !bVisiable )
	{
		ShowWindow(SW_HIDE);
	}
}

bool32 CIoViewReport::OnLButtonDown2(CPoint pt, int32 iTab)
{
	if ( iTab >= m_aTabInfomations.GetSize() )
	{
		return false;
	}

	const T_SimpleTabInfo* pSimpleTabInfo = GetSimpleTabInfo(iTab);

	if ( NULL == pSimpleTabInfo )
	{
		return false;
	}

	E_TabInfoType eTabType = pSimpleTabInfo->m_eTabType;

	//
	if ( ETITCollection == eTabType )
	{
		// �Ǽ������, ��Ҫ�����˵�		
		T_TabInfo stTabInfo = m_aTabInfomations.GetAt(iTab);

		//
		CNewMenu menu;		
		menu.LoadMenu(IDR_MENU_REPORT_NEW);
		menu.LoadToolBar(g_awToolBarIconIDs);

		CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
		ASSERT(NULL != pPopMenu);
		pPopMenu->LoadToolBar(g_awToolBarIconIDs);

		if ( T_BlockDesc::EBTBlockLogical == pSimpleTabInfo->m_eBlockType )
		{
			// �߼����˵�
			int32 iID = pSimpleTabInfo->m_iID;

			if ( KiIDTabAll == iID )
			{
				// ��ʾȫ��
				CBlockConfig::IdArray aIdAll;
				CBlockConfig::Instance()->GetCollectionIdArray(aIdAll);
				//
				for ( int32 i = 0; i < aIdAll.GetSize(); i++ )
				{
					if ( aIdAll[i] == KiIDTabAllExcept )
					{
						continue;
					}

					//
					CBlockCollection *pCol = CBlockConfig::Instance()->GetBlockCollectionById(aIdAll[i]);
					CBlockConfig::BlockArray aBlocks;
					pCol->GetValidBlocks(aBlocks);

					if ( aBlocks.GetSize() > 0 
						&& ( aBlocks[0]->m_blockInfo.GetBlockType() == CBlockInfo::typeNormalBlock				// ��ͨ || ����
						|| aBlocks[0]->m_blockInfo.GetBlockType() == CBlockInfo::typeMarketClassBlock )					
						)
					{
						CNewMenu *pColMenu = pPopMenu->AppendODPopupMenu(pCol->m_StrName);

						//
						ASSERT(NULL != pColMenu);

						//
						for ( int32 iBlock = 0; iBlock < aBlocks.GetSize(); iBlock++ )
						{						
							if ( NULL == aBlocks[iBlock] )
							{
								//ASSERT(0);
								continue;
							}

							//
							CString StrItem = aBlocks[iBlock]->m_blockInfo.m_StrBlockName;

							if ( StrItem.GetLength() > 7 )
							{
								StrItem = StrItem.Left(6) + _T("...");
							}

							//
							pColMenu->AppendODMenu(StrItem, MF_STRING, ID_IOVIEWREPORT_NEW_BEGIN + aBlocks[iBlock]->m_blockInfo.m_iBlockId);

							// ��ǵ�ǰ��ѡ��
							if ( stTabInfo.m_Block.m_iMarketId == aBlocks[iBlock]->m_blockInfo.m_iBlockId )
							{
								pPopMenu->CheckMenuItem(ID_IOVIEWREPORT_NEW_BEGIN + aBlocks[iBlock]->m_blockInfo.m_iBlockId, MF_BYCOMMAND | MF_CHECKED );
							}
						}

						MultiColumnMenu(*pColMenu, 20);
					}
				}				
			}
			else 
			{
				//
				CBlockCollection* pBlockCollection = CBlockConfig::Instance()->GetBlockCollectionById(iID);
				if ( NULL == pBlockCollection )
				{
					return false;
				}

				//
				for ( int32 i = 0; i < pBlockCollection->m_aBlocks.GetSize(); i++ )
				{
					CBlockLikeMarket* pBlock = pBlockCollection->m_aBlocks[i];
					if ( NULL == pBlock )
					{
						continue;
					}

					//
					pPopMenu->AppendODMenu(pBlock->m_blockInfo.m_StrBlockName, MF_STRING, ID_IOVIEWREPORT_NEW_BEGIN + pBlock->m_blockInfo.m_iBlockId);

					// ��ǵ�ǰѡ��
					if ( stTabInfo.m_Block.m_iMarketId == pBlock->m_blockInfo.m_iBlockId )
					{
						pPopMenu->CheckMenuItem(ID_IOVIEWREPORT_NEW_BEGIN + pBlock->m_blockInfo.m_iBlockId, MF_BYCOMMAND | MF_CHECKED );
					}	

					MultiColumnMenu(*pPopMenu, 20);

					// TRACE(L"�˵���: %s ID: %d \n", pBlock->m_blockInfo.m_StrBlockName, ID_IOVIEWREPORT_NEW_BEGIN + pBlock->m_blockInfo.m_iBlockId);
				}		
			}
		}
		else if ( T_BlockDesc::EBTUser == pSimpleTabInfo->m_eBlockType )
		{
			// �û����˵�
			CArray<T_Block, T_Block&> aBlocks;
			CUserBlockManager::Instance()->GetBlocks(aBlocks);

			//
			for ( int32 i = 0; i < aBlocks.GetSize(); i++ )
			{
				T_Block stBlock = aBlocks[i];
				pPopMenu->AppendODMenu(stBlock.m_StrName, MF_STRING | MF_BYCOMMAND, ID_IOVIEWREPORT_NEW_END - i);

				// ��ǵ�ǰѡ��
				if ( stTabInfo.m_Block.m_StrBlockName == stBlock.m_StrName )
				{
					pPopMenu->CheckMenuItem(ID_IOVIEWREPORT_NEW_END - i, MF_BYCOMMAND | MF_CHECKED);
				}				
			}

			//--- wangyongxue 20170112 ������Ҫ�����˵�����Ĭ����ʾ��һ����ѡ���
			// 			if (0 < aBlocks.GetSize())
			// 			{
			// 				m_iCurTabTmp = iTab;
			// 				OnIoViewReportMenuTabCollection(0);
			// 				return TRUE;
			// 			}
		}

		// ɾ����һ���ָ���
		pPopMenu->RemoveMenu(0, MF_BYPOSITION | MF_SEPARATOR);

		// ������ù̶�һ���ط������˵�
		pt = m_GuiTabWnd.GetLeftTopPoint(iTab);		

		pPopMenu->TrackPopupMenu(TPM_BOTTOMALIGN | TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL, pt.x, pt.y, AfxGetMainWnd());
		menu.DestroyMenu();	

		//
		m_iCurTabTmp = iTab;

		//
		return true;
	}

	if ( m_aTabInfomations.GetSize() > KiFixTabCounts && (iTab == KiFixTabCounts - 1) )
	{
		// ���ڹ̶�����. ��Ҫ�˵�������ʾ
		CNewMenu menu;
		menu.LoadMenu(IDR_MENU_BLOCK_MORE);
		menu.LoadToolBar(g_awToolBarIconIDs);

		CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
		ASSERT(NULL != pPopMenu);
		pPopMenu->LoadToolBar(g_awToolBarIconIDs);

		T_TabInfo* pTabInfo = (T_TabInfo*)m_aTabInfomations.GetData();

		for ( int32 i = (KiFixTabCounts - 1); i < m_aTabInfomations.GetSize(); i++ )
		{				
			pPopMenu->AppendODMenu(pTabInfo[i].m_Block.m_StrBlockName, MF_STRING, ID_MENU_IOVIEWREPORT_MORE_BGN1 + i);

			if ( m_iCurTab == i )
			{
				pPopMenu->CheckMenuItem(ID_MENU_IOVIEWREPORT_MORE_BGN1 + i, MF_BYCOMMAND|MF_CHECKED);
			}
		}

		// ɾ����һ���ָ���
		pPopMenu->RemoveMenu(0, MF_BYPOSITION | MF_SEPARATOR);

		// ������ù̶�һ���ط������˵�
		pt = m_GuiTabWnd.GetLeftTopPoint(iTab);		

		pPopMenu->TrackPopupMenu(TPM_BOTTOMALIGN | TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL, pt.x, pt.y, this);
		menu.DestroyMenu();		
		return true;
	}
	else if ( iTab >= m_aTabInfomations.GetSize() )
	{
		MenuBlockInsert();		// ���ֱ�ӶԻ���
		return true;

		m_GuiTabWnd.ClientToScreen(&pt);

		CNewMenu menu;
		menu.LoadMenu(IDR_MENU_BLOCK);
		menu.LoadToolBar(g_awToolBarIconIDs);

		CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
		pPopMenu->LoadToolBar(g_awToolBarIconIDs);
		pPopMenu->SetMenuTitle(_T("���˵�"), MFT_ROUND|MFT_LINE|MFT_CENTER|MFT_SIDE_TITLE);

		menu.DeleteMenu(IDM_BLOCK_SET,	 MF_BYCOMMAND);
		menu.DeleteMenu(IDM_BLOCK_CHANGE,MF_BYCOMMAND);
		menu.DeleteMenu(IDM_BLOCK_DELETE,MF_BYCOMMAND);

		pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, this);
		menu.DestroyMenu();	

		return true;
	}

	return false;
}

void CIoViewReport::OnRButtonDown2(CPoint pt, int32 iTab)
{
	return;

	m_GuiTabWnd.ClientToScreen(&pt);

	if ( m_aTabInfomations.GetSize() > KiFixTabCounts && (iTab == KiFixTabCounts - 1) )	
	{
		// �һ��� "����" ����
		CNewMenu menu;
		menu.LoadMenu(IDR_MENU_BLOCK_MORE_DEL);
		menu.LoadToolBar(g_awToolBarIconIDs);

		CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
		ASSERT(NULL != pPopMenu);
		pPopMenu->LoadToolBar(g_awToolBarIconIDs);

		T_TabInfo* pTabInfo = (T_TabInfo*)m_aTabInfomations.GetData();

		for ( int32 i = (KiFixTabCounts - 1); i < m_aTabInfomations.GetSize(); i++ )
		{				
			CString StrMenuName;
			StrMenuName.Format(L"ɾ�� [%s]", pTabInfo[i].m_Block.m_StrBlockName.GetBuffer());

			pPopMenu->AppendODMenu(StrMenuName, MF_STRING, ID_MENU_IOVIEWREPORT_MORE_BGN2 + i);						
		}

		//
		GetCursorPos(&pt);
		pPopMenu->RemoveMenu(0, MF_SEPARATOR);

		pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, this);
		menu.DestroyMenu();			
	}	
	else if ( iTab >= m_aTabInfomations.GetSize() )
	{
		// �յı�ǩҳ��
		CNewMenu menu;
		menu.LoadMenu(IDR_MENU_BLOCK);
		menu.LoadToolBar(g_awToolBarIconIDs);

		CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
		pPopMenu->LoadToolBar(g_awToolBarIconIDs);
		pPopMenu->SetMenuTitle(_T("���˵�"), MFT_ROUND|MFT_LINE|MFT_CENTER|MFT_SIDE_TITLE);

		menu.DeleteMenu(IDM_BLOCK_SET,	 MF_BYCOMMAND);
		menu.DeleteMenu(IDM_BLOCK_CHANGE,MF_BYCOMMAND);
		menu.DeleteMenu(IDM_BLOCK_DELETE,MF_BYCOMMAND);

		pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, this);
		menu.DestroyMenu();	
	}
	else
	{
		int32 iCurTab;
		T_BlockDesc::E_BlockType eTypeCur;
		if ( TabIsValid(iCurTab, eTypeCur) )
		{
			if ( m_bIsStockSelectedReport && ( eTypeCur == T_BlockDesc::EBTSelect || eTypeCur == T_BlockDesc::EBTMerchSort ) )		// ѡ�ɱ��۱������������� ��ʱ
			{
				return;
			}
		}

		// �����ı�ǩҳ��
		CNewMenu menu;
		menu.LoadMenu(IDR_MENU_BLOCK);
		menu.LoadToolBar(g_awToolBarIconIDs);

		CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
		pPopMenu->LoadToolBar(g_awToolBarIconIDs);
		pPopMenu->SetMenuTitle(_T("���˵�"), MFT_ROUND|MFT_LINE|MFT_CENTER|MFT_SIDE_TITLE);


		T_BlockDesc::E_BlockType eType;

		if ( !TabIsValid(iTab, eType))
		{
			menu.DeleteMenu(IDM_BLOCK_SET,	 MF_BYCOMMAND);
			menu.DeleteMenu(IDM_BLOCK_CHANGE,MF_BYCOMMAND);
			menu.DeleteMenu(IDM_BLOCK_DELETE,MF_BYCOMMAND);
		}
		else
		{
			T_BlockDesc BlockDesc = m_aTabInfomations.GetAt(iTab).m_Block;

			CString StrText;
			StrText = _T("����[") + BlockDesc.m_StrBlockName + _T("]");
			menu.ModifyODMenu(IDM_BLOCK_CHANGE,MF_BYCOMMAND|MF_STRING,IDM_BLOCK_CHANGE,StrText);

			if ( m_aTabInfomations.GetSize() == 1 )
			{
				menu.EnableMenuItem(IDM_BLOCK_DELETE,MF_BYCOMMAND|MF_GRAYED);
			}
			else
			{
				StrText = _T("�ر�[") + BlockDesc.m_StrBlockName + _T("]");
				menu.ModifyODMenu(IDM_BLOCK_DELETE,MF_BYCOMMAND|MF_STRING,IDM_BLOCK_DELETE,StrText);
			}
		}

		pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, this);
		menu.DestroyMenu();	
	}
}

bool32 CIoViewReport::BlindLButtonBeforeRButtonDown(int32 iTab)
{
	if ( iTab >= m_aTabInfomations.GetSize() )
	{
		// �Ҽ����ڿհ׵ı�ǩҳ��, ��Ҫ�л���ǩ
		return true;
	}

	return false;
}

void CIoViewReport::OnMenuExport ( UINT nID )
{
	int32 iTab;
	T_BlockDesc::E_BlockType eType;

	if ( TabIsValid(iTab, eType))
	{
		T_BlockDesc BlockDesc = m_aTabInfomations.GetAt(iTab).m_Block;

		if( nID == IDM_USER1 )
		{
			if (T_BlockDesc::EBTUser == eType)
			{
				ExportUserBlockToExcel(_T("���۱�"), BlockDesc.m_StrBlockName, _T(""), _T(""));
			}
			else
			{
				if (m_bShowCustomGrid)
				{
					ExportGridToExcel(&m_GridCtrl, &m_CustomGridCtrl, _T("���۱�"), BlockDesc.m_StrBlockName, _T(""), _T(""), true);
				}
				else
				{
					ExportGridToExcel(&m_GridCtrl, NULL, _T("���۱�"), BlockDesc.m_StrBlockName, _T(""), _T(""), true);
				}
			}	
		}

		if ( nID == IDM_USER2 )
		{			
			// 			StartAsyncExcel(&m_GridCtrl, BlockDesc.m_StrBlockName, m_StrAsyncExcelFileName);
			// 
			// 			COLORREF clrRise = GetIoViewColor(ESCRise);
			// 			COLORREF clrFall = GetIoViewColor(ESCFall);
			// 			COLORREF clrKeep = GetIoViewColor(ESCKeep);
			// 			
			// 			AsyncGridToExcel(&m_GridCtrl, BlockDesc.m_StrBlockName, m_StrAsyncExcelFileName, -1, clrRise, clrFall, clrKeep);

			// ��ȡ�����Ϣ - ��ʱ���в���Ӧ���Ƕ���ʼ�����˵�
			T_Block block;
			block.m_StrName = BlockDesc.m_StrBlockName;
			block.m_eHeadType = m_eMarketReportType;
			for ( int i=0; i < m_aSmartAttendMerchs.GetSize() ; i++ )
			{
				block.m_aMerchs.Add(m_aSmartAttendMerchs[i].m_pMerch);
			}
			if ( block.m_aMerchs.GetSize() > 0 )
			{
				CIoViewSyncExcel::Instance().AddSyncBlock(block);
			}
		}
	}
}

void CIoViewReport::OnMenuIoViewReport(UINT nID )
{
	// ��ͼ�������еĲ˵���
	switch ( nID )
	{
	case IDM_IOVIEWREPORT_GRIDLINE:
		{
			// ������
			OnIoViewReportGridLine();
		}
		break;
	case IDM_IOVIEWREPORT_DELOWNSEL:
		{
			// ɾ����ѡ��
			OnIoViewReportMenuDelOwnSel();
		}
		break;
	case IDM_IOVIEWREPORT_OWNSEL_UP:
		{
			// ��ѡ������
			OnIoViewReportMenuOwnSelUp();			
		}
		break;
	case IDM_IOVIEWREPORT_OWNSEL_DOWN:
		{
			// ��ѡ������
			OnIoViewReportMenuOwnSelDown();	
		}
		break;
	case IDM_IOVIEWREPORT_OWN_DOWNLOAD:
		{
			//			CUploadUserData::instance()->DownloadUserBlock();	// ��ѡ�ɻᷢ�����֪ͨ��ע��!!
		}
		break;
	case IDM_IOVIEWREPORT_OWN_UPLOAD:
		{
			//			CUploadUserData::instance()->UploadUserBlock();
		}
		break;
	}
}

void CIoViewReport::OnMenu( UINT nID )
{
	switch(nID)
	{
	case IDM_BLOCK_CHANGE:
		MenuBlockChange();
		break;
	case IDM_BLOCK_DELETE:
		MenuBlockDel();
		break;
	case IDM_BLOCK_INSERT:
		MenuBlockInsert();
		break;
	case IDM_BLOCK_SET:
		MenuBlockSetting();
		break;
	case IDM_BLOCK_CANCEL:
		MenuBlockCancel();
		break;
	default:
		break;
	}
}

void CIoViewReport::MenuBlockChange()
{
	int32 iTab;
	T_BlockDesc::E_BlockType eType;

	if ( !TabIsValid(iTab, eType) )
	{
		return;
	}

	T_BlockDesc  BlockDesc = m_aTabInfomations.GetAt(iTab).m_Block;
	CBlockSelect BlockSelect(BlockDesc);

	if ( IDOK == BlockSelect.DoModal() )
	{
		//�ж��ظ�?
		for ( int32 i = 0; i < m_aTabInfomations.GetSize(); i++ )
		{
			if ( m_aTabInfomations.GetAt(i).m_Block.m_StrBlockName == BlockSelect.m_BlockDesc.m_StrBlockName )
			{
				m_iCurTab = i;
				ReCreateTabWnd();
				return;
			}
		}
	}

	//
	if ( !ValidBlock(BlockSelect.m_BlockDesc) )
	{
		return;
	}

	// ǿ�ưѳ�ʼ�������Ϊ 0;
	UpdateLocalDataIndex(iTab, 0, 1);

	//
	UpdateLocalBlock(iTab, BlockSelect.m_BlockDesc);

	// ���ݴ�С ���ú��ʵ����
	SetTabParams(iTab);

	//
	ReCreateTabWnd();
}

void CIoViewReport::MenuBlockDel()
{
	int32 iTab;
	T_BlockDesc::E_BlockType eType;

	if ( !TabIsValid(iTab, eType) )
	{
		return;
	}

	m_aTabInfomations.RemoveAt(iTab);
	ReCreateTabWnd();
}

void CIoViewReport::MenuBlockInsert()
{
	T_BlockDesc BlockDesc;
	BlockDesc.m_eType = T_BlockDesc::EBTCount;

	CBlockSelect BlockSelect(BlockDesc);

	if ( IDOK == BlockSelect.DoModal() )
	{
		AddBlock(BlockSelect.m_BlockDesc);
	}
}

void CIoViewReport::MenuBlockSetting()
{
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	pMainFrm->OnBlockSet();
}

void CIoViewReport::MenuBlockCancel()
{

}

void CIoViewReport::AddBlock(const T_BlockDesc& BlockDesc)
{
	OpenBlock(BlockDesc, EMRFRisePercent, true);
}

void CIoViewReport::OpenBlock(const T_BlockDesc& BlockDesc)
{
	T_BlockDesc BlockTmp = BlockDesc;

	if ( !ValidBlock(BlockTmp) )
	{
		return;
	}

	int32 i, iSize = m_aTabInfomations.GetSize();
	bool32 bFound = false;

	for ( i = 0; i < iSize; i ++ )
	{
		T_BlockDesc Desc = m_aTabInfomations.GetAt(i).m_Block;

		switch ( BlockTmp.m_eType )
		{
		case T_BlockDesc::EBTPhysical:
			if ( BlockTmp.m_pMarket == Desc.m_pMarket )
			{				
				UpdateLocalBlock(i, BlockTmp);
				bFound = true;
			}
			break;
		case T_BlockDesc::EBTUser :
			{
				if ( BlockTmp.m_StrBlockName == Desc.m_StrBlockName )
				{				
					UpdateLocalBlock(i, BlockTmp);
					bFound = true;
				}
			}
			break;
		case T_BlockDesc::EBTRecentView:
			{
				if (  T_BlockDesc::EBTRecentView == Desc.m_eType )
				{				
					UpdateLocalBlock(i, BlockTmp);
					bFound = true;
				}
			}
			break;
		case T_BlockDesc::EBTLogical:
			if ( BlockTmp.m_StrBlockName == Desc.m_StrBlockName )
			{
				UpdateLocalBlock(i, BlockTmp);				
				bFound = true;
			}
			break;
		case T_BlockDesc::EBTSelect:		// ����ѡ�� - ����ѡ��ֻ��һ��tab
		case T_BlockDesc::EBTMerchSort:		// ����ǿ��
			//if ( BlockTmp.m_StrBlockName == Desc.m_StrBlockName )
			if ( T_BlockDesc::EBTSelect == Desc.m_eType || T_BlockDesc::EBTMerchSort == Desc.m_eType)	// �˴�������ǰReportSelect��OnCreateʱ��ʼ��ʱ���κ�tab��������tab�ˣ�������ж�
			{
				UpdateLocalBlock(i, BlockTmp);				
				bFound = true;
			}
			break;
		case T_BlockDesc::EBTBlockLogical:
			if ( BlockTmp.m_StrBlockName == Desc.m_StrBlockName )
			{
				UpdateLocalBlock(i, BlockTmp);				
				bFound = true;
			}
			break;
		default:
			break;
		}

		if ( bFound )
		{
			break;
		}
	}

	if ( !bFound
		&& T_BlockDesc::EBTUser == BlockDesc.m_eType )
	{
		// �û���� - �൱��tab�˵���ѡ��
		// ��tab��ʾ�û�ѡ��
		int32 iTab = 0;
		for ( ; iTab < m_SimpleTabInfoClass.aTabInfos.size() ; iTab++ )
		{
			if ( m_SimpleTabInfoClass.aTabInfos[iTab].m_iID == KiIDTabUserOwn )
			{
				m_GuiTabWnd.SetCurtab(iTab, true);
				break;
			}
		}
		bool32 bOpen = false;
		if ( iTab < m_SimpleTabInfoClass.aTabInfos.size() )
		{
			CArray<T_Block, T_Block&> aBlocks;
			CUserBlockManager::Instance()->GetBlocks(aBlocks);

			for ( int32 i=0; i < aBlocks.GetSize() ; i++ )
			{
				if ( BlockDesc.m_StrBlockName == aBlocks[i].m_StrName )
				{
					m_iCurTabTmp = iTab;
					OnIoViewReportMenuTabCollection(ID_IOVIEWREPORT_NEW_END - i);
					bOpen = true;
					break;
				}
			}
		}

		// û�еĻ�Ӧ���ǳ�����, ������Ĵ���
		ASSERT( bOpen );
		if ( bOpen )
		{
			return;
		}
	}

	//����ҵ����
	if ( bFound )
	{
		// ���´�
		/*if ( i != m_iCurTab)*/
		{
			m_iCurTab = i;
			SetTab(m_iCurTab);
			// ����ÿ����ǩ�ı�ͷ��һ���������Ҫ������
			TabChange();
		}	
	}
	else
	{
		// �Է���һ, ����Ʒ�б����һ��:
		if ( T_BlockDesc::EBTUser == BlockTmp.m_eType )
		{
			CUserBlockManager::Instance()->GetMerchsInBlock(BlockTmp.m_StrBlockName, BlockTmp.m_aMerchs);
		}
		else if ( T_BlockDesc::EBTLogical == BlockTmp.m_eType )
		{
			CSysBlockManager::Instance()->GetMerchsInBlock(BlockTmp.m_StrBlockName, BlockTmp.m_aMerchs);
		}
		else if ( T_BlockDesc::EBTSelect == BlockTmp.m_eType || T_BlockDesc::EBTMerchSort == BlockTmp.m_eType)
		{
			// ����ѡ��û����Ʒ�б���Ҫ�������󣬵ȴ���Ʒ�������ܾ�������Щ��Ʒ��Ŀǰ�޽ӿ�

		}
		else if ( T_BlockDesc::EBTRecentView == BlockTmp.m_eType)
		{
			//
		}
		else if ( T_BlockDesc::EBTBlockLogical == BlockTmp.m_eType )
		{
			// ֻ�������һ�δ�����������ݣ��Ժ󲻸���
		}

		// ɾ�����һ��
		if (m_aTabInfomations.GetSize() <= 0 )
		{
			//ASSERT(0);

			// ���³�ʼ��һ��
			InitialTabInfos();
		}

		if (m_aTabInfomations.GetSize() < 1)
		{
			return;
		}

		//
		m_aTabInfomations.RemoveAt(m_aTabInfomations.GetSize() - 1);

		//
		T_TabInfo stTabToAdd;
		stTabToAdd.m_Block = BlockTmp;

		//
		m_aTabInfomations.Add(stTabToAdd);
		m_iCurTab = m_aTabInfomations.GetSize() - 1;

		// ��������ѡ�ɱ�ǩλ��
		if (T_BlockDesc::EBTSelect == BlockTmp.m_eType || T_BlockDesc::EBTMerchSort == BlockTmp.m_eType)
		{
			m_iSelStockTab = m_iCurTab;
		}
		//
		SetTabParams(m_iCurTab);
		ReCreateTabWnd();
		
		// ��Ҫˢ������������Ʒ��Ϣ
		if (T_BlockDesc::EBTRecentView == BlockTmp.m_eType)
		{
			//
			TabChange();
		}
	}
}

void CIoViewReport::OpenBlock(const T_BlockDesc& BlockDesc, E_MerchReportField eMerchReportField, bool32 bDescSort)
{
	T_BlockDesc BlockTmp = BlockDesc;
	BlockTmp.m_iFieldAdd = eMerchReportField;
	OpenBlock (BlockTmp);

	// ���������ܳ�������ע�͵�
	//if ( (int32)eMerchReportField == -1 )
	//{
	//	return;
	//}

	//�ҵ�m_iSortColumn
	int32 i, iSize = m_GridCtrl.GetColumnCount();
	for ( i = 0; i < iSize; i++)
	{
		CGridCellSys * pCell = (CGridCellSys *)m_GridCtrl.GetCell(0, i);
		E_MerchReportField eField = (E_MerchReportField)pCell->GetData();
		if ( eField == eMerchReportField )
		{
			m_iSortColumn = i;
			break;
		}
	}
	if ( i == iSize )
	{
		return;
	}

	//��Ҫ���������� - ����ѡ�ɵ�������������ͨ����Ҫ��������
	if ( T_BlockDesc::EBTPhysical == BlockDesc.m_eType )
	{
		m_MmiRequestSys.m_bDescSort = bDescSort;
		m_MmiRequestSys.m_eMerchReportField = eMerchReportField;
		m_bRequestViewSort = true;
		RequestViewDataSort();
	}
	else if ( T_BlockDesc::EBTSelect == BlockDesc.m_eType || T_BlockDesc::EBTMerchSort == BlockTmp.m_eType)
	{
		ASSERT( 0 );	// ��û�д���������
		m_MmiBlockRequestSys.m_bDescSort = bDescSort;
		m_MmiBlockRequestSys.m_eMerchReportField = eMerchReportField;
		m_bRequestViewSort = true;
		RequestViewDataSort();
	}
	else if ( T_BlockDesc::EBTBlockLogical == BlockDesc.m_eType )
	{
		//ASSERT( 0 );	// ��û�д��������� - ��������
		m_MmiBlockRequestSys.m_bDescSort = bDescSort;
		m_MmiBlockRequestSys.m_eMerchReportField = eMerchReportField;
		m_bRequestViewSort = true;
		RequestViewDataSort();
	}
	else
	{
		m_GridCtrl.SetSortColumn(m_iSortColumn);
	}	
}

bool32 CIoViewReport::IsBlockExist(const CString& StrBlockName)
{
	for (int32 i = 0; i < m_aTabInfomations.GetSize(); i++)
	{
		if ( m_aTabInfomations[i].m_Block.m_StrBlockName == StrBlockName )
		{			
			return true;
		}
	}	

	return false;
}

bool32 CIoViewReport::SetTabByBlockName(const CString& StrBlockName)
{
	for (int32 i = 0; i < m_aTabInfomations.GetSize(); i++)
	{
		if ( m_aTabInfomations[i].m_Block.m_StrBlockName == StrBlockName )
		{
			SetTab(i); // ��ͼTab�л�
			if ( i < KiFixTabCounts-1 )
			{

			}
			else	// �ͺ����ǲ˵��л�һ�� -  block�л�
			{
				// ������ǰ��������λ��:
				//int32 iVScrollPos = m_GridCtrl.GetScrollPos32(SB_VERT);
				//int32 iHScrollPos = m_GridCtrl.GetScrollPos32(SB_HORZ);

				//UpdateLocalXScrollPos(m_iCurTab, iHScrollPos);

				int32 iCurTab = i;	
				m_iPreTab = m_iCurTab;
				m_iCurTab = iCurTab;

				TabChange();
			}
			//SetTab(i);			

			return true;
		}
	}				

	return false;
}

bool32 CIoViewReport::SetTabByBlockType(T_BlockDesc::E_BlockType eType)
{
	for (int32 i = 0; i < m_aTabInfomations.GetSize(); i++)
	{
		if ( m_aTabInfomations[i].m_Block.m_eType == eType )
		{			
			SetTab(i);
			return true;
		}
	}

	return false;
}

void CIoViewReport::SetColWidthAccordingFont()
{
	// 1: ���巢���仯��ʱ��
	// 2: ���ñ�ͷ��ʱ��( ֪����ͷ��֪������Щ��.)

	if ( m_eMarketReportType >= ERTCount)
	{
		return;
	}

	if ( m_ReportHeadInfoList.GetSize() <= 0)
	{
		return;
	}

	// �õ� dc
	CClientDC dc(this);

	// �����п�
	for ( int32 i = 0 ; i < m_ReportHeadInfoList.GetSize(); i++) 
	{
		// ��һ�еı�ͷ:
		CGridCellBase * pCell = m_GridCtrl.GetCell(0, i);

		if ( NULL == pCell )
		{
			return;
		}

		int32 iWidthHead,iWidthBody;
		iWidthHead = 30;
		iWidthBody = 0;

		CString StrHead = pCell->GetText();

		CFont * pFontBig = GetIoViewFontObject(ESFNormal);
		CFont * pOldFont = dc.SelectObject(pFontBig);

		CSize sizeHead = dc.GetTextExtent(StrHead);
		iWidthHead = sizeHead.cx;

		dc.SelectObject(pOldFont);

		if ( m_GridCtrl.GetRowCount() > 1)
		{
			CGridCellBase * pCellBody = m_GridCtrl.GetCell(1, i);

			CGridCellBase * pCellTest = m_GridCtrl.GetCell(1, 0);
			CString StrTest = pCellTest->GetText();

			if ( NULL != pCellBody )
			{
				CString StrBody = pCellBody->GetText();
				CFont * pFontNormal = GetIoViewFontObject(ESFNormal);
				pOldFont = dc.SelectObject(pFontNormal);

				CSize sizeBody = dc.GetTextExtent(StrBody);
				iWidthBody = sizeBody.cx * 4/3;

				dc.SelectObject(pOldFont);
			}			
		}

		int32 iWidth = iWidthHead > iWidthBody ? iWidthHead : iWidthBody;
		iWidth += 12;		

		// ���ñ��
		m_GridCtrl.SetColumnWidth(i,iWidth);

		// ���µ������ļ���

		if ( BeSpecial() )
		{
			CReportScheme::Instance()->SaveColumnWidth(ERTFutureSpot,i,iWidth);
		}
		else
		{
			CReportScheme::Instance()->SaveColumnWidth(m_eMarketReportType,i,iWidth);
		}

	}

	// ���豾�ص�����ֵ
	if ( BeSpecial() )
	{
		CReportScheme::Instance()->GetReportHeadInfoList(ERTFutureSpot, m_ReportHeadInfoList, m_iFixCol);	
	}
	else
	{
		CReportScheme::Instance()->GetReportHeadInfoList(m_eMarketReportType,m_ReportHeadInfoList,m_iFixCol);	
	}	
}

void CIoViewReport::SetRowHeightAccordingFont()
{
	// ���߰�... ��ǰ��ôû�����������?
	// ���߰�... ��ǰ��ôû�����������?
	//m_GridCtrl.AutoSizeRows();
	m_GridCtrl.SetDefCellHeight(28);
	m_GridCtrl.AutoSize();
	m_CustomGridCtrl.AutoSizeRows();
	if (0 != m_GridCtrl.GetFixedRowCount())
	{
		m_GridCtrl.SetRowHeight(0, 40);
	}

}

void CIoViewReport::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();	

	UpdateTableContent(true, NULL, false);	// ������Ʒȫ��������ʾ

	m_GuiTabWnd.SetBkGround(false, GetIoViewColor(ESCBackground), 0, 0);
	m_GuiTabWnd.RedrawWindow();
}

void CIoViewReport::OnIoViewFontChanged()
{
	CIoViewBase::OnIoViewFontChanged();

	// ��������
	LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);

	//
	//UpdateTableContent(TRUE, NULL, false);
	//	m_GridCtrl.AutoSize();

	CalcMaxGridRowCount();

	// ��������ֵ
	SetTabParams();
	AutoSizeColFromXml();

	// ����������
	ReSetGridContent();
	UpdateTableContent(TRUE, NULL, FALSE);

	RequestViewDataCurrentVisibleRowAsync();
	RequestViewDataSortAsync();

	Invalidate(TRUE);
}

/*
LRESULT CIoViewReport::OnMsgHotKey(WPARAM wParam, LPARAM lParam)
{
CHotKey *pHotKey = (CHotKey *)wParam;
if (NULL == pHotKey)
{
//ASSERT(0);
return 0;
}

if ( pHotKey->m_eHotKeyType != EHKTMerch )
{
::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), UM_HOTKEY, wParam, lParam);	// ����Ʒ��Ϣ����mainframe����
return 0;
}

int32 iPos = m_GridCtrl.GetScrollPos32(SB_VERT);

int32 iTab;
T_BlockDesc::E_BlockType eType;

if ( !TabIsValid(iTab, eType))
{
return 0;
}

if ( T_BlockDesc::EBTUser != eType )
{
return 0;
}

T_BlockDesc BlockDesc = m_aTabInfomations.GetAt(iTab).m_Block;



CHotKey HotKey = *pHotKey;
DEL(pHotKey);
pHotKey = &HotKey;

if (EHKTMerch == pHotKey->m_eHotKeyType)
{
CString StrHotKey ,StrName, StrMerchCode;
int32   iMarketId;
StrHotKey		= pHotKey->m_StrKey;
StrMerchCode	= pHotKey->m_StrParam1;
StrName			= pHotKey->m_StrSummary;
iMarketId		= pHotKey->m_iParam1;

CMerch* pMerch = NULL;

if ( m_pAbsCenterManager->GetMerchManager().FindMerch(StrMerchCode, iMarketId, pMerch) )
{
if ( NULL == pMerch )
{
return 0;
}

bool32 bExist = false;
CCellID CellToChoose;

// �������MerchCode��ͬ�����г���ͬ����Ʒbug��ͬʱ�޸�CellToChooseΪ�ҵ���cell
CellToChoose.row = INT_MAX;
for (int32 i = 0 ; i< m_aSmartAttendMerchs.GetSize(); i++)
{
CSmartAttendMerch &SmartAttendMerch =  m_aSmartAttendMerchs[i];
if (0 == lstrcmp(SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode, StrMerchCode)
&& pMerch->m_MerchInfo.m_iMarketId == SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId)
{
bExist = true;

CellToChoose.row = i+1;
CellToChoose.col = 0;

break;
}
}

if ( !bExist )
{				
CUserBlockManager::Instance()->AddMerchToUserBlock(pMerch, BlockDesc.m_StrBlockName);
TabChange();	// �����б�
}

int32 iRowCount = m_GridCtrl.GetRowCount();
if ( CellToChoose.row >= iRowCount - 1 )
{
CellToChoose.row = iRowCount - 2;
}
CellToChoose.col = 0;

// ѡ�и���  // ��ѡ��IoView�л��󣬷�IoVewReportҳ��ʧȥfocus����Ϊm_GridCtrlȡ����focus - -
m_GridCtrl.SetFocus();

if ( m_GridCtrl.IsCellVisible(CellToChoose) )
{					
m_GridCtrl.SetScrollPos32(SB_VERT, iPos, TRUE);
}
else
{
m_GridCtrl.EnsureVisible(CellToChoose);				
}

m_GridCtrl.SetSelectedRange(CellToChoose.row, 0, CellToChoose.row, m_GridCtrl.GetColumnCount()-1, true, true);			
m_GridCtrl.SetFocusCell(CellToChoose);

// �л���Ʒ // ����m_GridCtrl��ȡfocus��Ȼ������ҳ���ٻ�ȡ
{
OnDblClick();
}

// �ٴμ����ͼ
CMPIChildFrame* pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
CGGTongView* pGGTView  = GetParentGGtongView();
pFrame->SetActiveGGTongView(pGGTView);
((CMainFrame*)AfxGetMainWnd())->SetHotkeyTarget(this);		
}
}

return 0;
}
*/

void CIoViewReport::DragUserMerchPosition()
{
	int32 iTab;
	T_BlockDesc::E_BlockType eType;

	if (!TabIsValid(iTab, eType))
	{
		return;
	}

	bool32 bRmMerch = false; // �Ƿ���������Ʒ
	if (IsInCustomGrid())
	{
		bRmMerch = true;

		// ��ʼλ�ò���ͬһ���
		if (1 != m_iDragGridType)
		{
			return;
		}
	}
	else
	{
		// ��ʼλ�ò���ͬһ���
		if (0 != m_iDragGridType)
		{
			return;
		}
	}

	if (bRmMerch)
	{
		// ����״̬�²������������
		int32 iSortCol = m_CustomGridCtrl.GetSortColumn();
		if ( 0 <= iSortCol && iSortCol < m_CustomGridCtrl.GetSortColumn() )
		{
			return;
		}

		// ��һ����Ʒ
		CPoint ptCell;
		GetCursorPos(&ptCell);
		ScreenToClient(&ptCell);
		ptCell.y -= m_RctSperator.bottom;
		CCellID cellNext = m_CustomGridCtrl.GetCellFromPt(ptCell);
		if (!m_CustomGridCtrl.IsValid(cellNext))
		{
			return;
		}

		if ((cellNext.row<=0) || (m_iRowDragBegin<=0))
		{
			return;
		}

		CGridCellSys * pCellNext = (CGridCellSys *)m_CustomGridCtrl.GetCell(cellNext.row, 0);
		CMerch * pMerchNext = (CMerch *)pCellNext->GetData();

		// �϶�����
		bool32 bPre = false;
		if (m_iRowDragBegin > cellNext.row)
		{
			bPre = true;
		}

		// ������ͬ���ģ������������ֹ��ѡ����Ӧ
		m_bEnableUserBlockChangeResp = false;
		CString StrBlockName = _T("������Ʒ");
		CUserBlockManager::Instance()->ChangeOwnMerchPosition(StrBlockName, bPre, m_pDragMerchBegin, pMerchNext);
		m_bEnableUserBlockChangeResp = true;

		int32 iDataBegin = 0, iDataEnd = 0;
		GetCurrentDataIndex(iDataBegin, iDataEnd);

		// ����ChangeOwnMerchPosition��֪ͨ����ͼ������Ӧ���ֶ�����
		UpdateLocalDataIndex(m_iCurTab, iDataBegin, iDataEnd);
		ReSetGridCellDataByIndex();
		UpdateTableContent(TRUE, NULL, false);

		CCellID CellNow = cellNext;	

		// ����ѡ����
		m_CustomGridCtrl.EnsureVisible(CellNow);
		m_CustomGridCtrl.SetFocusCell(CellNow.row, CellNow.col);	
		m_CustomGridCtrl.SetSelectedSingleRow(CellNow.row);
	}
	else
	{
		if ( T_BlockDesc::EBTUser != eType )
		{
			return;
		}

		// ����״̬�²������������
		int32 iSortCol = m_GridCtrl.GetSortColumn();
		if ( 0 <= iSortCol && iSortCol < m_GridCtrl.GetSortColumn() )
		{
			return;
		}

		// ��һ����Ʒ
		CPoint ptCell;
		GetCursorPos(&ptCell);
		ScreenToClient(&ptCell);
		CCellID cellNext = m_GridCtrl.GetCellFromPt(ptCell);
		if (!m_GridCtrl.IsValid(cellNext))
		{
			return;
		}

		if ((cellNext.row<=0) || (m_iRowDragBegin<=0) || (m_iRowDragBegin == cellNext.row ))
		{
			return;
		}

		CGridCellSys * pCellNext = (CGridCellSys *)m_GridCtrl.GetCell(cellNext.row, 0);
		CMerch * pMerchNext = (CMerch *)pCellNext->GetData();

		// �϶�����
		bool32 bPre = false;
		if (m_iRowDragBegin > cellNext.row)
		{
			bPre = true;
		}

		// ������ͬ���ģ������������ֹ��ѡ����Ӧ
		m_bEnableUserBlockChangeResp = false;
		CString StrBlockName = m_aTabInfomations.GetAt(m_iCurTab).m_Block.m_StrBlockName;
		CUserBlockManager::Instance()->ChangeOwnMerchPosition(StrBlockName, bPre, m_pDragMerchBegin, pMerchNext);
		m_bEnableUserBlockChangeResp = true;

		int32 iDataBegin = 0, iDataEnd = 0;
		GetCurrentDataIndex(iDataBegin, iDataEnd);

		// ����ChangeOwnMerchPosition��֪ͨ����ͼ������Ӧ���ֶ�����
		UpdateLocalDataIndex(m_iCurTab, iDataBegin, iDataEnd);
		ReSetGridCellDataByIndex();
		UpdateTableContent(TRUE, NULL, false);

		CCellID CellNow = cellNext;	

		// ����ѡ����
		m_GridCtrl.EnsureVisible(CellNow);
		m_GridCtrl.SetFocusCell(CellNow.row, CellNow.col);	
		m_GridCtrl.SetSelectedSingleRow(CellNow.row);
	}
}

bool32	CIoViewReport::BeUserBlock()
{
	int32 iTab;
	T_BlockDesc::E_BlockType eType;

	if (!TabIsValid(iTab, eType))
	{
		return false;
	}

	if ( (T_BlockDesc::EBTUser == eType) || IsInCustomGrid() )
	{
		return true;
	}

	return false;
}

void CIoViewReport::SetDragMerchBegin(bool32 bClear)
{
	if (bClear)
	{
		m_pDragMerchBegin = NULL;
	}
	else
	{
		m_pDragMerchBegin = NULL;
		CPoint ptLBtnDown;
		GetCursorPos(&ptLBtnDown);
		ScreenToClient(&ptLBtnDown);

		if (IsInCustomGrid())	// ������Ʒ
		{
			ptLBtnDown.y -= m_RctSperator.bottom;
			CCellID cell = m_CustomGridCtrl.GetCellFromPt(ptLBtnDown);
			if (!m_CustomGridCtrl.IsValid(cell) || (cell.row <= 0))
			{
				return;
			}

			CGridCellSys * pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(cell.row, 0);
			m_pDragMerchBegin = (CMerch *)pCell->GetData();
			m_iRowDragBegin = cell.row;
			m_iDragGridType = 1;
		}
		else		// ��ѡ
		{
			CCellID cell = m_GridCtrl.GetCellFromPt(ptLBtnDown);
			if (!m_GridCtrl.IsValid(cell) || (cell.row <= 0))
			{
				return;
			}

			CGridCellSys * pCell = (CGridCellSys *)m_GridCtrl.GetCell(cell.row, 0);
			m_pDragMerchBegin = (CMerch *)pCell->GetData();
			m_iRowDragBegin = cell.row;
			m_iDragGridType = 0;
		}
	}
}

void CIoViewReport::AdjustUserMerchPosition(bool32 bUp)
{
	CCellID CellFocusBK = m_GridCtrl.GetFocusCell();

	if (!m_GridCtrl.IsValid(CellFocusBK))
	{
		return;
	}

	int32 iTab;
	T_BlockDesc::E_BlockType eType;

	if (!TabIsValid(iTab, eType))
	{
		return;
	}

	if ( T_BlockDesc::EBTUser != eType )
	{
		return;
	}

	// ����״̬�²������������
	int32 iSortCol = m_GridCtrl.GetSortColumn();
	if ( 0 <= iSortCol && iSortCol < m_GridCtrl.GetSortColumn() )
	{
		return;
	}

	CCellRange cellRange = m_GridCtrl.GetSelectedCellRange();

	int32 iRow = cellRange.GetMaxRow();

	if ( iRow >= m_GridCtrl.GetRowCount() || iRow < m_GridCtrl.GetFixedRowCount())		
	{
		// Ӧ���ǲ���ѡ���̶��е�
		return;
	}

	// �ҵ������Ʒ	
	CGridCellSys * pCellToDell = (CGridCellSys *)m_GridCtrl.GetCell(iRow,0);
	CMerch * pMerch = (CMerch *)pCellToDell->GetData();

	int32 iDataBegin, iDataEnd;
	iDataBegin = iDataEnd = 0;
	bool32 b  = GetCurrentDataIndex(iDataBegin, iDataEnd);
	ASSERT( b );
	/*int32 iHScrollPosBk =*/ m_GridCtrl.GetScrollPos32(SB_HORZ);

	if( bUp )
	{
		// ��һ�в�����
		if ( 1 == iRow && 0 != iDataBegin )
		{
			CAutoLockWindowUpdate lockWinUpdate(&m_GridCtrl);
			// ���ݻ����������ƶ� xl 0604 ��ʱ����
			::SendMessage(m_GridCtrl.GetSafeHwnd(), WM_KEYDOWN, VK_UP, 0);

			// �ٴ�ȷ���Ƿ��ƶ���λ��
			int32 iDataBegin2, iDataEnd2;
			bool32 bRightPos  = GetCurrentDataIndex(iDataBegin2, iDataEnd2);
			ASSERT( bRightPos );
			if ( bRightPos && iDataBegin > iDataBegin2 )
			{
				// �ƶ���λ�ã�������ǰѡ�е���Ϊ��ǰѡ��λ�õ�ƫ��
				int i = 1;
				for ( ; i < m_GridCtrl.GetRowCount(); i++ )  
				{
					CGridCellSys * pCurCellToDell = (CGridCellSys *)m_GridCtrl.GetCell(i,0);
					if ( pCurCellToDell->GetData() == (LPARAM)pMerch )
					{
						// �ҵ���
						CellFocusBK.row = i;
						//����focus - select
						m_GridCtrl.SetFocusCell(CellFocusBK);
						m_GridCtrl.SetSelectedSingleRow(i);
						break;
					}
				}
				if ( m_GridCtrl.GetRowCount() == i )
				{
					// �Ƴ���Χ�ˣ���ô�����أ�
					ASSERT( 0 );
					return;
				}
			}
			else
			{
				// û���ƶ������߾�Ȼ�����ƶ��ˣ��Ǿ����˰�
				ASSERT( 0 );
				return;
			}
			iDataBegin = iDataBegin2;
			iDataEnd   = iDataEnd2;
		}
		else if ( 1 == iRow )
		{
			// ����Ҳû��������
			return;
		}
	}
	else
	{
		// ���һ�в����� - -2 �ǿ��У���ô������ʱǿ���ƶ������һ����Ʒ��, �����ѱ�ȥ��
		if ( m_GridCtrl.GetRowCount() - 1 == iRow && m_aTabInfomations[m_iCurTab].m_Block.m_aMerchs.GetSize()-1 > iDataEnd )
		{
			// ��ѡ������Ϊ���һ��
			CAutoLockWindowUpdate lockWinUpdate(&m_GridCtrl);

			m_GridCtrl.SetFocusCell(m_GridCtrl.GetRowCount()-1, CellFocusBK.col);
			//m_GridCtrl.SetSelectedRange(m_GridCtrl.GetRowCount()-1, 0, m_GridCtrl.GetRowCount()-1, m_GridCtrl.GetColumnCount()-1, FALSE, TRUE);
			m_GridCtrl.SetSelectedSingleRow(m_GridCtrl.GetRowCount()-1);

			// ���ݻ����������ƶ� xl 0604 ��ʱ����
			::SendMessage(m_GridCtrl.GetSafeHwnd(), WM_KEYDOWN, VK_DOWN, 0);

			// �ٴ�ȷ���Ƿ��ƶ���λ��
			int32 iDataBegin2, iDataEnd2;
			bool32 bGetPos  = GetCurrentDataIndex(iDataBegin2, iDataEnd2);
			ASSERT( bGetPos );
			if ( bGetPos && iDataEnd < iDataEnd2 )
			{
				// �ƶ���λ�ã�������ǰѡ�е���Ϊ��ǰѡ��λ�õ�ƫ��
				int i = 1;
				for ( ; i < m_GridCtrl.GetRowCount(); i++ )  
				{
					CGridCellSys * pTmpCellToDell = (CGridCellSys *)m_GridCtrl.GetCell(i,0);
					if ( pTmpCellToDell->GetData() == (LPARAM)pMerch )
					{
						// �ҵ���
						CellFocusBK.row = i;
						//����focus - select
						m_GridCtrl.SetFocusCell(CellFocusBK);
						//m_GridCtrl.SetSelectedRange(i, 0, i, m_GridCtrl.GetColumnCount()-1, FALSE, TRUE);
						m_GridCtrl.SetSelectedSingleRow(i);
						break;
					}
				}
				if ( m_GridCtrl.GetRowCount() == i )
				{
					// �Ƴ���Χ�ˣ���ô�����أ�
					return;
				}
			}
			else
			{
				// û���ƶ������߾�Ȼ�����ƶ��ˣ��Ǿ����˰�
				return;
			}

			iDataBegin = iDataBegin2;
			iDataEnd   = iDataEnd2;
		}
		else if ( iRow == m_GridCtrl.GetRowCount() - 1 )
		{
			// �Ѿ������һ����
			return;
		}
	}

	if ( NULL == pMerch)
	{
		return;
	}

	// ������ͬ���ģ������������ֹ��ѡ����Ӧ
	m_bEnableUserBlockChangeResp = false;
	CString StrBlockName = m_aTabInfomations.GetAt(m_iCurTab).m_Block.m_StrBlockName;
	CUserBlockManager::Instance()->ChangeOwnMerchPosition(bUp, StrBlockName, pMerch);
	m_bEnableUserBlockChangeResp = true;

	// ����ChangeOwnMerchPosition��֪ͨ����ͼ������Ӧ���ֶ�����
	// ����������ָ�
	UpdateLocalDataIndex(m_iCurTab, iDataBegin, iDataEnd);
	//m_GridCtrl.SetScrollPos32(SB_HORZ, iHScrollPosBk);
	ReSetGridCellDataByIndex();
	UpdateTableContent(TRUE, NULL, false);

	CCellID CellNow = CellFocusBK;
	if ( bUp )
	{
		// ѡ�е�ǰ�е�ǰһ��				
		CellNow.row = CellFocusBK.row - 1;		
	}
	else
	{
		// ѡ�е�ǰ�еĺ�һ��				 		
		CellNow.row = CellFocusBK.row + 1;
	}	

	// ����ѡ����
	m_GridCtrl.EnsureVisible(CellNow);

	m_GridCtrl.SetFocusCell(CellNow.row, CellNow.col);	
	//m_GridCtrl.SetSelectedRange(CellNow.row, 0, CellNow.row, m_GridCtrl.GetColumnCount() - 1, true, true);
	m_GridCtrl.SetSelectedSingleRow(CellNow.row);
}

void CIoViewReport::OnIoViewReportMenuDelOwnSel()
{
	int32 iTab;
	T_BlockDesc::E_BlockType eType;
	if ( !TabIsValid(iTab, eType) )
	{
		return;
	}
	// �˵���Ӧ����,ɾ����ѡ��:
	// ɾ�����
	MerchArray aMerchs;
	GetMerchXmls(aMerchs);
	const int32 iSize = aMerchs.GetSize();
	if ( iSize > 1 )
	{
		if ( MessageBox(_T("��ȷ��Ҫ�ӵ�ǰ�����ɾ����Щ��Ʒ��"), _T("��ȷ��"), MB_YESNO|MB_ICONQUESTION) != IDYES )
		{
			return;
		}
	}

	//---wangyongxue ɾ����ѡ��
	CString StrBlockName = _T("");
	if (T_BlockDesc::EBTUser == eType)
	{
		StrBlockName = m_aTabInfomations[iTab].m_Block.m_StrBlockName;
	}
	else
	{
		StrBlockName = CUserBlockManager::Instance()->GetServerBlockName();
	}

	if ( !StrBlockName.IsEmpty() )
	{
		for ( int32 i=0; i < aMerchs.GetSize() ; i++ )
		{
			CMerch * pMerch = aMerchs[i];
			if ( NULL == pMerch )
			{
				continue;
			}

			CUserBlockManager::Instance()->DelMerchFromUserBlock(pMerch, StrBlockName, false);
		}

		CUserBlockManager::Instance()->SaveXmlFile();
		CUserBlockManager::Instance()->Notify(CSubjectUserBlock::EUBUMerch);
	}
}

void CIoViewReport::OnIoViewReportMenuDelALLOwnSel()
{
	// ���뵽Ĭ����ѡ���
	T_Block *pBlock = CUserBlockManager::Instance()->GetServerBlock();
	if ( NULL != pBlock && 0 == pBlock->m_StrName.CompareNoCase(_T("�ҵ���ѡ")))
	{
		if ( MessageBox(_T("���Ƿ�Ҫ�����ѡ�ɣ�"), _T("��ȷ��"), MB_YESNO|MB_ICONQUESTION) != IDYES )
		{
			return;
		}

		CUserBlockManager::Instance()->DelAllUserBlock(pBlock->m_StrName);
		CUserBlockManager::Instance()->SaveXmlFile();

		CUserBlockManager::Instance()->Notify(CSubjectUserBlock::EUBUMerch);
	}
}

void CIoViewReport::OnIoViewReportMenuOwnSelUp()
{
	// ������ѡ��
	AdjustUserMerchPosition(true);
}

void CIoViewReport::OnIoViewReportMenuOwnSelDown()
{
	// ������ѡ��
	AdjustUserMerchPosition(false);
}

void CIoViewReport::OnIoViewReportMenuBlockMoreLBtnDown(UINT nID)
{
	if ( m_aTabInfomations.GetSize() > KiFixTabCounts /*&& (m_iCurTab >= KiFixTabCounts - 1)*/ )
	{
		int32 iCurTab = nID - ID_MENU_IOVIEWREPORT_MORE_BGN1;
		m_iPreTab = m_iCurTab;
		m_iCurTab = iCurTab;

		SetTabParams(m_iCurTab);
		ReCreateTabWnd();
		TabChange();
	}	
}

void CIoViewReport::OnIoViewReportMenuBlockMoreRBtnDown( UINT nID )
{
	if ( m_aTabInfomations.GetSize() > KiFixTabCounts /*&& (m_iCurTab >= KiFixTabCounts - 1)*/ )
	{
		int32 iCurTab = nID - ID_MENU_IOVIEWREPORT_MORE_BGN2;

		// ɾ��
		m_aTabInfomations.RemoveAt(iCurTab);
		ReCreateTabWnd();
	}	
}


void CIoViewReport::OnIoViewReportMenuTabTopCollection( UINT nID )
{
	//

	 CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	 if (!pMainFrame || m_CurTopTabbar.m_iBlockid == -1)
	 {
		 return;
	 }
	//
	if ( T_BlockDesc::EBTUser == m_CurTopTabbar.m_iBlockType )
	{
		// ��ѡ��
		int32 iIndex = ID_IOVIEWREPORT_NEW_TOP_END - nID;

		CArray<T_Block, T_Block&> aBlocks;
		CUserBlockManager::Instance()->GetBlocks(aBlocks);

		if ( aBlocks.GetSize() <= 0 )
		{
			return;
		}

		if ( iIndex < 0 || iIndex >= aBlocks.GetSize() )
		{
			////ASSERT(0);
			iIndex = 0;
		}

		//
		T_Block Block = aBlocks[iIndex];

		T_BlockDesc BlockDesc;
		BlockDesc.m_eType		 = T_BlockDesc::EBTUser;
		BlockDesc.m_StrBlockName = Block.m_StrName;
		BlockDesc.m_iMarketId	 = -1;
		//linhc 20100917���Ĭ�Ͻ����һ����Ʒ
		BlockDesc.m_aMerchs.Copy(Block.m_aMerchs);
		pMainFrame->OnSpecifyBlock(BlockDesc);

		// ��¼�����Ϣ
		m_CurSelectBlockInfo.m_eType = T_BlockDesc::EBTUser;
		m_CurSelectBlockInfo.m_iMarketID = iIndex;
		m_CurSelectBlockInfo.m_StrBlockName = Block.m_StrName;

	}
	else if ( T_BlockDesc::EBTBlockLogical == m_CurTopTabbar.m_iBlockType )
	{
		// �߼����		
		int32 iID = nID - ID_IOVIEWREPORT_NEW_TOP_BEGIN;
		CBlockLikeMarket* pBlock = CBlockConfig::Instance()->FindBlock(iID);
		if ( NULL == pBlock )
		{
			return;
		}

		T_BlockDesc BlockDesc;
		BlockDesc.m_eType		 = T_BlockDesc::EBTBlockLogical;
		BlockDesc.m_StrBlockName = pBlock->m_blockInfo.m_StrBlockName;
		BlockDesc.m_iMarketId	 = -1;
		//linhc 20100917���Ĭ�Ͻ����һ����Ʒ
		if ( NULL != pBlock && pBlock->m_blockInfo.m_aSubMerchs.GetSize() > 0 )
		{   
			BlockDesc.m_aMerchs.Copy(pBlock->m_blockInfo.m_aSubMerchs);
		}
		pMainFrame->OnSpecifyBlock(BlockDesc);

		// ��¼�����Ϣ
		m_CurSelectBlockInfo.m_eType = T_BlockDesc::EBTBlockLogical;
		m_CurSelectBlockInfo.m_iMarketID = iID;
		m_CurSelectBlockInfo.m_StrBlockName = pBlock->m_blockInfo.m_StrBlockName;

	}
}

void CIoViewReport::OnIoViewReportMenuTabCollection( UINT nID )
{
	//
	if ( m_iCurTabTmp < 0 || m_iCurTabTmp >= m_aTabInfomations.GetSize() )
	{
		return;
	}

	//
	T_TabInfo stTabInfo = m_aTabInfomations.GetAt(m_iCurTabTmp);
	const T_SimpleTabInfo *pSimTab = GetSimpleTabInfo(m_iCurTabTmp);
	E_TabInfoType eTabInfoType = stTabInfo.m_eTabType;
	T_BlockDesc::E_BlockType eBlockType = stTabInfo.m_Block.m_eType;
	if ( NULL != pSimTab )
	{
		// ���������õ�Ϊ׼
		eTabInfoType = pSimTab->m_eTabType;
		eBlockType	= pSimTab->m_eBlockType;
	}

	//
	if ( ETITCollection != eTabInfoType )
	{
		return;
	}

	//
	if ( T_BlockDesc::EBTUser == eBlockType )
	{
		// ��ѡ��
		int32 iIndex = ID_IOVIEWREPORT_NEW_END - nID;

		CArray<T_Block, T_Block&> aBlocks;
		CUserBlockManager::Instance()->GetBlocks(aBlocks);

		if ( aBlocks.GetSize() <= 0 )
		{
			return;
		}

		if ( iIndex < 0 || iIndex >= aBlocks.GetSize() )
		{
			////ASSERT(0);
			iIndex = 0;
		}

		//
		T_Block Block = aBlocks[iIndex];

		T_TabInfo stTabNew;
		stTabNew.m_eTabType				= eTabInfoType;
		stTabNew.m_StrShowName			= stTabInfo.m_StrShowName;

		stTabNew.m_Block.m_eType		= eBlockType;
		stTabNew.m_Block.m_iMarketId	= 0;
		stTabNew.m_Block.m_StrBlockName	= Block.m_StrName;
		stTabNew.m_Block.m_aMerchs.Copy(Block.m_aMerchs);

		m_aTabInfomations.SetAt(m_iCurTabTmp, stTabNew);

		// ��¼�����Ϣ
		/*m_CurSelectBlockInfo.m_eType = T_BlockDesc::EBTUser;
		m_CurSelectBlockInfo.m_iMarketID = iIndex;
		m_CurSelectBlockInfo.m_StrBlockName = Block.m_StrName;*/
	}
	else if ( T_BlockDesc::EBTBlockLogical == eBlockType )
	{
		// �߼����		
		int32 iID = nID - ID_IOVIEWREPORT_NEW_BEGIN;

		CBlockLikeMarket* pBlock = CBlockConfig::Instance()->FindBlock(iID);
		if ( NULL == pBlock )
		{
			return;
		}

		T_TabInfo stTabNew;
		stTabNew.m_eTabType				= eTabInfoType;
		stTabNew.m_StrShowName			= stTabInfo.m_StrShowName;

		stTabNew.m_Block.m_eType		= eBlockType;
		stTabNew.m_Block.m_iMarketId	= pBlock->m_blockInfo.m_iBlockId;
		stTabNew.m_Block.m_StrBlockName	= pBlock->m_blockInfo.m_StrBlockName;
		stTabNew.m_Block.m_aMerchs.Copy(pBlock->m_blockInfo.m_aSubMerchs);

		m_aTabInfomations.SetAt(m_iCurTabTmp, stTabNew);

		// ��¼�����Ϣ
		/*m_CurSelectBlockInfo.m_eType = T_BlockDesc::EBTBlockLogical;
		m_CurSelectBlockInfo.m_iMarketID = iID;
		m_CurSelectBlockInfo.m_StrBlockName = pBlock->m_blockInfo.m_StrBlockName;*/
	}

	//
	m_iPreTab = m_iCurTab;
	m_iCurTab = m_iCurTabTmp;

	//
	SetTab(m_iCurTab);
	//SetTabParams(m_iCurTab);	// ���´�tab������
	//
	TabChange();
}

void CIoViewReport::SetGridSelByMerch(const CMerch* pMerch)
{
	// ���ݴ������Ʒ����,���õ�ǰ����ѡ����:
	// 1: �ҵ�����������

	if ( NULL == pMerch)
	{
		return;
	}

	int32 iTab;
	T_BlockDesc::E_BlockType eType;

	if ( !TabIsValid(iTab, eType) )
	{
		return;
	}
	//
	int32 i = 0;
	for ( i = 0 ; i < m_GridCtrl.GetRowCount(); i++)
	{
		CGridCellBase * pCell = m_GridCtrl.GetCell(i, 0);

		if (NULL == pCell)
		{
			continue;
		}

		if ( (CMerch*)pCell->GetData() == pMerch )
		{
			// �ҵ�:

			CCellID cell(i, 0);						
			//m_GridCtrl.SetSelectedRange(i, 0, i, m_GridCtrl.GetColumnCount() - 1, true, true);
			m_GridCtrl.SetSelectedSingleRow(i);
			m_GridCtrl.EnsureVisible(cell); 
			if ( m_GridCtrl.IsCellVisible(cell) )
			{
				//NULL;	
			}
			else
			{
				// ���ù�����λ��

			}

			return;
		}
	}

	// �����ǰ������״̬���򲻱ز�����
	if ( m_bRequestViewSort )
	{
		return;
	}

	// ���ڵ�ǰ��������, �Ҳ����˾��л� - �������л���
	T_TabInfo stTabInfo = m_aTabInfomations.GetAt(iTab);

	int32 iMerchSize	= 0;
	CMerch** pMerchList = NULL;

	if ( T_BlockDesc::EBTUser == eType || T_BlockDesc::EBTLogical == eType )
	{
		pMerchList = stTabInfo.m_Block.m_aMerchs.GetData();
		iMerchSize = stTabInfo.m_Block.m_aMerchs.GetSize();
	}
	else if ( T_BlockDesc::EBTPhysical == eType )
	{
		pMerchList = stTabInfo.m_Block.m_pMarket->m_MerchsPtr.GetData();
		iMerchSize = stTabInfo.m_Block.m_pMarket->m_MerchsPtr.GetSize();
	}
	else if ( T_BlockDesc::EBTSelect == eType || T_BlockDesc::EBTMerchSort == eType )
	{
		pMerchList = stTabInfo.m_Block.m_aMerchs.GetData();
		iMerchSize = stTabInfo.m_Block.m_aMerchs.GetSize();
	}
	else if ( T_BlockDesc::EBTRecentView == eType )
	{
		pMerchList = stTabInfo.m_Block.m_aMerchs.GetData();
		iMerchSize = stTabInfo.m_Block.m_aMerchs.GetSize();
	}
	else if ( T_BlockDesc::EBTBlockLogical == eType )
	{
		pMerchList = stTabInfo.m_Block.m_aMerchs.GetData();
		iMerchSize = stTabInfo.m_Block.m_aMerchs.GetSize();
	}

	int32 iNumData = stTabInfo.m_iDataIndexEnd - stTabInfo.m_iDataIndexBegin + 1;

	if(pMerchList)
	{
		for ( i = 0; i < iMerchSize; i++ )
		{
			CMerch* pMerchLocal = pMerchList[i];

			if ( pMerchLocal == pMerch )
			{
				// ��iBegin, iEnd���ڿɽ��ܵķ�Χ��
				int32 iBegin, iEnd, iPos;
				iBegin = i;
				iEnd   = i + iNumData - 1;
				if ( iEnd > iMerchSize - 1 )
				{
					iEnd = iMerchSize - 1;
					iBegin = iEnd - iNumData + 1;
					if ( iBegin < 0 )
					{
						iBegin = 0;
					}
				}
				iPos = i - iBegin + 1;

				UpdateLocalDataIndex(iTab, iBegin, iEnd);
				ValidateTableInfo(iTab);
				ReSetGridCellDataByIndex();

				//
				RequestViewDataCurrentVisibleRowAsync();
				RequestViewDataSortAsync();

				if (iPos < m_GridCtrl.GetRowCount())
				{
					//
					m_GridCtrl.EnsureVisible(iPos, 0);
					//m_GridCtrl.SetSelectedRange(iPos, 0, iPos, m_GridCtrl.GetColumnCount() - 1);
					m_GridCtrl.SetSelectedSingleRow(iPos);
				}
				
				return;
			}
		}
	}
}

CMerch* CIoViewReport::GetGridSelMerch()
{
	if (m_bCustomGrid)
	{
		POSITION pos = m_CustomGridCtrl.GetFirstSelectedRowPosition();
		int32 iRow = -1;
		while ( NULL != pos )
		{
			iRow = m_CustomGridCtrl.GetNextSelectedRow(pos);
		}

		if ( m_CustomGridCtrl.GetRowCount() <= 1 )
		{	
			return NULL;
		}

		// Ĭ��ȡ��һ�е���Ʒ
		if ( iRow <= 0 || iRow >= m_CustomGridCtrl.GetRowCount() )
		{		
			// ѡ�пɼ��еĵ�һ��
			CCellRange CellRange = m_CustomGridCtrl.GetVisibleNonFixedCellRange();
			if ( CellRange.IsValid() )
			{
				iRow = CellRange.GetMinRow();
			}
			if ( iRow <= 0 || iRow >= m_CustomGridCtrl.GetRowCount() )
			{
				return NULL;
			}
			//m_CustomGridCtrl.SetSelectedRange(iRow, 0, iRow, m_GridCtrl.GetColumnCount()-1, TRUE, TRUE);
			m_CustomGridCtrl.SetSelectedSingleRow(iRow);
		}

		//
		CGridCellSys* pCell = (CGridCellSys*)m_CustomGridCtrl.GetCell(iRow, 0);
		if ( NULL == pCell )
		{		
			return NULL;
		}

		//
		CMerch* pMerch = (CMerch*)pCell->GetData();

		//
		return pMerch;
	}
	else
	{
		POSITION pos = m_GridCtrl.GetFirstSelectedRowPosition();
		int32 iRow = -1;
		while ( NULL != pos )
		{
			iRow = m_GridCtrl.GetNextSelectedRow(pos);
		}
		// 	CCellRange CellRange = m_GridCtrl.GetSelectedCellRange();
		// 	int32 iRow = CellRange.GetMaxRow();

		if ( m_GridCtrl.GetRowCount() <= 1 )
		{	
			return NULL;
		}

		// Ĭ��ȡ��һ�е���Ʒ
		if ( iRow <= 0 || iRow >= m_GridCtrl.GetRowCount() )
		{		
			// ѡ�пɼ��еĵ�һ��
			CCellRange CellRange = m_GridCtrl.GetVisibleNonFixedCellRange();
			if ( CellRange.IsValid() )
			{
				iRow = CellRange.GetMinRow();
			}
			if ( iRow <= 0 || iRow >= m_GridCtrl.GetRowCount() )
			{
				return NULL;
			}
			//m_GridCtrl.SetSelectedRange(iRow, 0, iRow, m_GridCtrl.GetColumnCount()-1, TRUE, TRUE);
			m_GridCtrl.SetSelectedSingleRow(iRow);
		}

		//
		CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRow, 0);
		if ( NULL == pCell )
		{		
			return NULL;
		}

		//
		CMerch* pMerch = (CMerch*)pCell->GetData();

		//
		return pMerch;
	}
}

void CIoViewReport::OnDblClick()
{
	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if ( NULL != pMainFrame )
	{
		CMerch *pMerch = GetGridSelMerch();
		if ( NULL != pMerch )
		{
			pMainFrame->OnShowMerchInChart(pMerch, this);
		}
	}
}

void CIoViewReport::SetChildFrameTitle()
{
	CString StrTitle;
	StrTitle =  CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;

	CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pParentFrame)
	{
		pParentFrame->SetChildFrameTitle(StrTitle);
	}
}

bool32 CIoViewReport::GetPreMerch(IN CMerch *pMerch, OUT CMerch *&pMerchPre)
{	
	if (m_bCustomGrid)
	{
		pMerchPre = NULL;

		// ���ݱ��۱�������,�õ�����Ʒ��ǰһ����Ʒ
		// �������,��ѡ�ɽ������һ�����ſյ�.�����жϵ�ʱ��Ҫ���⴦��

		if ( NULL == pMerch)
		{
			return false;
		}

		T_TabInfo stTabInfo = m_tabCustom;

		int32 iMerchSize	= 0;
		CMerch** pMerchList = NULL;

		pMerchList = stTabInfo.m_Block.m_aMerchs.GetData();
		iMerchSize = stTabInfo.m_Block.m_aMerchs.GetSize();

		for ( int32 i = 0; i < iMerchSize; i++ )
		{
			CMerch* pMerchLocal = pMerchList[i];

			if ( pMerchLocal == pMerch )
			{
				if ( 0 == i )
				{
					pMerchPre = pMerchList[iMerchSize - 1];
				}
				else
				{
					pMerchPre = pMerchList[i - 1];
				}

				return true;				 
			}
		}
	}
	else
	{
		pMerchPre = NULL;

		// ���ݱ��۱�������,�õ�����Ʒ��ǰһ����Ʒ
		// �������,��ѡ�ɽ������һ�����ſյ�.�����жϵ�ʱ��Ҫ���⴦��

		if ( NULL == pMerch)
		{
			return false;
		}

		int32 iTab;
		T_BlockDesc::E_BlockType eType;

		if (!TabIsValid(iTab, eType))
		{
			return false;
		}



		// �ӱ�����ҵ������Ʒ:	
		bool32 bSort = m_bRequestViewSort || m_GridCtrl.GetSortColumn() != -1;
		for (int32 i = 0 ; i < m_GridCtrl.GetRowCount() ; i++)
		{
			CGridCellBase * pCell = m_GridCtrl.GetCell(i,0);
			if ( NULL != pCell)
			{
				CMerch * pMerchGrid = (CMerch *)pCell->GetData();
				if ( pMerchGrid == pMerch )
				{
					// �ҵ��˱���������Ʒ������λ��:
					if ( 0 == i )
					{
						// ��ͷ(������)
						//ASSERT(0);
						return false;
					}

					if ( 1 == i )
					{
						// ��һ����Ʒ��ǰһ����Ʒȡ���һ����������������״̬�µ����⴦��
						if ( !bSort )
						{
							return false;	// ��������base�еĴ���
						}
						CGridCellBase* pCellFind  = NULL;
						pCellFind  = m_GridCtrl.GetCell(m_GridCtrl.GetRowCount() - 1, 0);

						if ( NULL == pCellFind)
						{
							return false;
						}

						CMerch *pMerchFind = (CMerch *)pCellFind->GetData();
						if ( NULL == pMerchFind)
						{
							return false;
						}

						pMerchPre = pMerchFind;
						return true;
					}	
					else
					{
						// ���������,ȡǰһ������.
						if ( i-1 <=0 || i-1 >= m_GridCtrl.GetRowCount())
						{
							return false;
						}

						CGridCellBase* pCellFind  = m_GridCtrl.GetCell(i-1,0);
						if ( NULL == pCellFind)
						{
							return false;
						}

						CMerch *pMerchFind = (CMerch *)pCellFind->GetData();
						if ( NULL == pMerchFind)
						{
							return false;
						}

						pMerchPre = pMerchFind;

						return true;
					}
				}
			}
		}
	}

	return false;
}

bool32 CIoViewReport::GetAftMerch(IN CMerch *pMerch, OUT CMerch *&pMerchAfter)
{
	if (m_bCustomGrid)
	{
		pMerchAfter = NULL;

		// ���ݱ��۱�������,�õ�����Ʒ����һ����Ʒ
		// �������,��ѡ�ɽ������һ�����ſյ�.�����жϵ�ʱ��Ҫ���⴦��
		if ( NULL == pMerch)
		{
			return false;
		}

		T_TabInfo stTabInfo = m_tabCustom;

		int32 iMerchSize	= 0;
		CMerch** pMerchList = NULL;

		pMerchList = stTabInfo.m_Block.m_aMerchs.GetData();
		iMerchSize = stTabInfo.m_Block.m_aMerchs.GetSize();

		for ( int32 i = 0; i < iMerchSize; i++ )
		{
			CMerch* pMerchLocal = pMerchList[i];

			if ( pMerchLocal == pMerch )
			{
				if ( (iMerchSize - 1) == i )
				{
					pMerchAfter = pMerchList[0];
				}
				else
				{
					pMerchAfter = pMerchList[i + 1];
				}

				return true;				 
			}
		}
	}
	else
	{
		pMerchAfter = NULL;

		// ���ݱ��۱�������,�õ�����Ʒ����һ����Ʒ
		// �������,��ѡ�ɽ������һ�����ſյ�.�����жϵ�ʱ��Ҫ���⴦��
		if ( NULL == pMerch)
		{
			return false;
		}

		int32 iTab;
		T_BlockDesc::E_BlockType eType;

		if ( !TabIsValid(iTab, eType) )
		{
			return false;
		}

		// �ӱ�����ҵ������Ʒ:
		bool32 bSort = m_bRequestViewSort || m_GridCtrl.GetSortColumn() != -1;
		for ( int32 i = 0 ; i < m_GridCtrl.GetRowCount() ; i++)
		{
			CGridCellBase * pCell = m_GridCtrl.GetCell(i,0);
			if ( NULL != pCell)
			{
				CMerch * pMerchGrid = (CMerch *)pCell->GetData();
				if ( pMerchGrid == pMerch )
				{
					// �ҵ��˱���������Ʒ������λ��:

					bool32 bLastPosition = false;

					if ( m_GridCtrl.GetRowCount() - 1 == i )
					{
						bLastPosition = true;
					}

					if ( 0 == i )
					{
						// ��ͷ(������)
						return false;
					}

					if ( bLastPosition )
					{
						// ���һ����Ʒ�ĺ�һ����Ʒȡ��һ��
						if ( !bSort )
						{
							return false;	// ��������base�еĴ���
						}
						CGridCellBase* pCellFind  = m_GridCtrl.GetCell(1,0);

						if ( NULL == pCellFind)
						{
							return false;
						}

						CMerch   * pMerchFind = (CMerch *)pCellFind->GetData();

						if ( NULL == pMerchFind)
						{
							return false;
						}

						pMerchAfter = pMerchFind;
						return true;				
					}	
					else
					{
						// ���������,ȡ��һ������.

						if ( i+1 <=0 || i+1 >= m_GridCtrl.GetRowCount())
						{
							return false;
						}

						CGridCellBase* pCellFind  = m_GridCtrl.GetCell(i+1,0);

						if ( NULL == pCellFind)
						{
							return false;
						}

						CMerch   * pMerchFind = (CMerch *)pCellFind->GetData();

						if ( NULL == pMerchFind)
						{
							return false;
						}

						pMerchAfter = pMerchFind;					
						return true;
					}
				}
			}
		}
	}
	return false;	
}

LRESULT CIoViewReport::OnMessageTitleButton(WPARAM wParam,LPARAM lParam)
{
	UINT uID = (UINT)wParam;

	if ( 0 == uID)
	{
		// ��
		MenuBlockInsert();
	}
	else if(1 == uID)
	{
		// ɾ
		if(m_GuiTabWnd.GetCount() <= 1)
		{
			return 0;
		}

		MenuBlockDel();
	}
	else if ( 2 == uID)
	{
		// ��
		MenuBlockChange();
	}
	else if ( 3 == uID)
	{
		// ��
		MenuBlockSetting();
	}
	else if ( 4 == uID)
	{
		// F7
		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		pMainFrame->OnProcessF7();
	}
	else
	{
		//NULL;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////// 
//��ѡ�����
void CIoViewReport::OnUserBlockUpdate(CSubjectUserBlock::E_UserBlockUpdate eUpdateType)
{
	// ��ѡ�ɸ��µ�֪ͨ:
	// ֻ��������Ʒ
	// 1: ����ѡ���,���Ƿ������ѡ������Ʒ,���ò�ͬ����ɫ
	// 2: ��ѡ���,������Ʒ˳��:TabChange()ˢ�±��, ���±�ͷ	
	int32 iTab;
	T_BlockDesc::E_BlockType eType;
	if ( !TabIsValid(iTab, eType) )
	{
		return;
	}

	int32 iVScrollPosBk = m_GridCtrl.GetScrollPos32(SB_VERT);

	if ( CSubjectUserBlock::EUBUMerch == eUpdateType )
	{
		ValidateTableInfo();

		// ����ڵ���Ʒ������
		for ( int32 i = 0; i < m_aTabInfomations.GetSize(); i++)
		{
			if ( T_BlockDesc::EBTUser == m_aTabInfomations.GetAt(i).m_Block.m_eType )
			{
				// xl 0604 ����ԭ������ʾ��Χ���� 0002037
				//UpdateLocalDataIndex(i, 0, 1);
				SetTabParams(i);
			}
		}

		if ( T_BlockDesc::EBTUser == eType )
		{
			//
			if ( m_bEnableUserBlockChangeResp )	// ������tabchange
			{
				TabChange();
			}
		}
		else
		{
			UpdateTableContent(TRUE, NULL, FALSE);
		}
		if (m_bShowCustomGrid)
		{
			InitialCustomMerch();
		}
	}
	else if ( CSubjectUserBlock::EUBUBlock == eUpdateType )
	{
		// ��鱾�����Ϣ������

		// ������:
		CArray<T_Block, T_Block&> aBlocks;
		CUserBlockManager::Instance()->GetBlocks(aBlocks);

		for ( int32 i = 0; i < m_aTabInfomations.GetSize(); i++)
		{
			CString StrName = m_aTabInfomations.GetAt(i).m_Block.m_StrBlockName;

			for ( int32 j = 0; j < aBlocks.GetSize(); j++ )
			{
				T_Block stBlock = aBlocks.GetAt(j);

				if ( stBlock.m_bChangeName && stBlock.m_StrNameOld == StrName )
				{
					T_BlockDesc BlockDesc	 = m_aTabInfomations.GetAt(i).m_Block;					
					BlockDesc.m_StrBlockName = stBlock.m_StrName;

					UpdateLocalBlock(i, BlockDesc);

					if ( i > 0 && i < KiFixTabCounts-1 )
					{
						m_GuiTabWnd.SetTabsTitle(i, stBlock.m_StrName);
					}
					else if ( i == KiFixTabCounts-1 && i == m_aTabInfomations.GetSize()-1 )
					{
						m_GuiTabWnd.SetTabsTitle(i, stBlock.m_StrName);
					}
					break;
				}
			}	
		}

		//
		ReCreateTabWnd();		
	}

	m_GridCtrl.SetScrollPos32(SB_VERT, iVScrollPosBk);
}

void CIoViewReport::DelUserBlockMerchByRow(int32 iRow)
{
	if ( iRow >= m_GridCtrl.GetRowCount() || iRow < 0 )
		return;

	// ��ѡ����ɾ����֧��Ʊ
	CGridCellSys * pCellToDell = (CGridCellSys *)m_GridCtrl.GetCell(iRow,0);
	CMerch * pMerch = (CMerch *)pCellToDell->GetData();

	if ( NULL == pMerch )
	{
		return;
	}

	if ( m_aTabInfomations.GetSize() <= 0 || m_iCurTab >= m_aTabInfomations.GetSize() )
	{
		return;
	}

	CString StrBlockName = m_aTabInfomations.GetAt(m_iCurTab).m_Block.m_StrBlockName;
	CUserBlockManager::Instance()->DelMerchFromUserBlock(pMerch, StrBlockName);

	// ����Bug,�ָ���������
	// 	CGGTongApp * pApp = (CGGTongApp *)AfxGetApp();
	// 	
	// 	if ( !pApp->m_bBindHotkey )
	// 	{
	// 		pApp->m_bBindHotkey = TRUE;
	// 	}

	// �ٰ������ͼ����Ϊ������ͼ
	// 	CMPIChildFrame* pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	// 	CGGTongView* pGGTView  = GetParentGGtongView();
	// 	pFrame->SetActiveGGTongView(pGGTView);
	// 	((CMainFrame*)AfxGetMainWnd())->SetHotkeyTarget(this);

	m_GridCtrl.SetFocus();
	CCellID cellSelect;

	if ( iRow == m_GridCtrl.GetRowCount() && iRow >= 1 )
	{
		iRow -= 1;	
	}

	cellSelect.col = 0;
	cellSelect.row = iRow;

	m_GridCtrl.SelectRows(cellSelect);
	m_GridCtrl.SetFocusCell(cellSelect);	

	m_GridCtrl.AutoSizeRows();
}

void CIoViewReport::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	// ����ˮƽ������λ��
	if ( m_iCurTab < 0 || m_iCurTab >= m_aTabInfomations.GetSize() )
	{
		return;
	}

	// �����λ��
	int32 iHScrollPos = m_GridCtrl.GetScrollPos32(SB_HORZ);
	UpdateLocalXScrollPos(m_iCurTab, iHScrollPos);

	m_CustomGridCtrl.SetScrollPos32(SB_HORZ,nPos);
	m_CustomGridCtrl.OnHScroll(nSBCode,nPos,pScrollBar);
	m_CustomGridCtrl.Invalidate();
}

void CIoViewReport::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl* pSlider = (CSliderCtrl*)pScrollBar;

	if(ID_VERTICAL_REPORT == pSlider->GetDlgCtrlID())
	{
		if(nSBCode == SB_LINEUP)
		{
			OnMouseWheel(1);
		}
		else if(nSBCode == SB_LINEDOWN)
		{
			OnMouseWheel(-1);
		}
		else if(SB_THUMBTRACK == nSBCode)
		{
			int nMinPos, nMaxPos, nNowPos;
			m_XSBVertical.GetScrolRange(nMinPos, nMaxPos);
			nNowPos = m_XSBVertical.GetScrollPos();

			CCellID CellFocusBK = m_GridCtrl.GetFocusCell();
			T_TabInfo stTabInfo = m_aTabInfomations.GetAt(m_iCurTab);
			// �������������Ϊ m_GridCtrl.EnsureVisible ���䶯������һ��
			int32 iHScrolPosBk = m_GridCtrl.GetScrollPos32(SB_HORZ);
			// ����ɼ��У���������EnsureVisible�����ˮƽ����
			CCellRange VisibleCells = m_GridCtrl.GetVisibleNonFixedCellRange(NULL, FALSE, FALSE);
			int32 iVisibleCol = VisibleCells.GetMinCol();

			int32 iDataNumsPerPage		= m_iMaxGridRowCount;				// ��ǰ�������ʾ������

			int32 iIndexBeginNow;
			int32 iIndexEndNow;

			int32 iSizeMerchs;	
			if ( T_BlockDesc::EBTPhysical == stTabInfo.m_Block.m_eType )
			{
				iSizeMerchs = stTabInfo.m_Block.m_pMarket->m_MerchsPtr.GetSize();
			}
			else
			{
				iSizeMerchs = stTabInfo.m_Block.m_aMerchs.GetSize();
			}

			iIndexBeginNow = iSizeMerchs * nNowPos / abs(nMaxPos - nMinPos);
			if(iIndexBeginNow >= iSizeMerchs)
			{
				iIndexBeginNow = iSizeMerchs - 1;
			}

			iIndexEndNow = iIndexBeginNow + iDataNumsPerPage - 1;
			if ( iIndexEndNow >= iSizeMerchs )
			{
				iIndexEndNow = iSizeMerchs - 1;
			}

			m_GridCtrl.EnsureVisible(m_GridCtrl.GetFixedRowCount(), iVisibleCol);

			if (iIndexBeginNow == stTabInfo.m_iDataIndexBegin)	// δ�϶�
			{
				return;
			}

			UpdateLocalDataIndex(m_iCurTab, iIndexBeginNow, iIndexEndNow);
			//
			UpdateLocalXScrollPos(m_iCurTab, iHScrolPosBk);
			m_GridCtrl.SetScrollPos32(SB_HORZ, iHScrolPosBk);
			// �������ʵ��������
			m_GridCtrl.SetRowCount((iIndexEndNow - iIndexBeginNow + 1) + m_GridCtrl.GetFixedRowCount());
			ReSetGridCellDataByIndex();

			if ( m_bRequestViewSort )
			{
				// ��������, ��������Ժ������ǰ�ɼ���ʵʱ����
				CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
				CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
				bool32 bCanReqSort = NULL==pApp?false:!pApp->m_bOffLine;
				bCanReqSort = bCanReqSort && (NULL==pMainFrame? false:pMainFrame->GetUserConnectCmdFlag()&EUCCF_ConnectDataServer);
				RequestViewDataSortAsync();
				if ( !bCanReqSort )
				{
					// ������ԶҲ�ز�������ʾ��
					UpdateTableContent(TRUE, NULL, false);
					RequestViewDataCurrentVisibleRowAsync();
				}
			}
			else
			{	// ����ǰ�ɼ�������
				UpdateTableContent(TRUE, NULL, false);
				RequestViewDataCurrentVisibleRowAsync();
			}

			// �������ʱ��Ĭ�Ͻ�����
			CCellRange cellRange = m_GridCtrl.GetSelectedCellRange();
			int32 iCurSel = cellRange.GetMinRow();
			CGridCellSys * pCellToDell = NULL;

			if ( iCurSel > 0 || iCurSel <= m_GridCtrl.GetRowCount() - 1  )
			{   
				pCellToDell = (CGridCellSys *)m_GridCtrl.GetCell(iCurSel,0);
			}

			if ( NULL == pCellToDell )
			{
				m_GridCtrl.SetFocusCell(CellFocusBK);
				m_GridCtrl.SetSelectedSingleRow(1);
			}
		}
	}
}

void CIoViewReport::OnVDataPluginResp( const CMmiCommBase *pResp )
{
	if ( NULL == pResp || pResp->m_eCommType != ECTRespPlugIn )
	{
		return;
	}

	const CMmiCommBasePlugIn *pRespPlugin = (const CMmiCommBasePlugIn *)pResp;

	switch ( pRespPlugin->m_eCommTypePlugIn )
	{
	case ECTPIRespMerchIndex:
	case ECTPIRespAddPushMerchIndex:
		{
			// ���ͻذ�������Ļذ�һ����. ֻ�����Ͳ�ͬ
			const CMmiRespMerchIndex *pRespIndex = (const CMmiRespMerchIndex *)pRespPlugin;
			for ( int i=0; i < pRespIndex->m_aMerchIndexList.GetSize() ; i++ )
			{
				CMerch *pMerch = NULL;
				const T_RespMerchIndex &merchIndex = pRespIndex->m_aMerchIndexList[i];
				if ( m_pAbsCenterManager && !m_pAbsCenterManager->GetMerchManager().FindMerch(merchIndex.m_StrCode, merchIndex.m_uiMarket, pMerch))
				{
					continue;
				}

				m_mapMerchIndex[pMerch] = merchIndex;
				UpdateTableContent(FALSE, pMerch, FALSE);
			}

		}
		break;
	case ECTPIRespMerchSort:
		{
			const CMmiRespMerchSort* pRespMerchSort = (const CMmiRespMerchSort*)pRespPlugin;

			int32 iTab;
			T_BlockDesc::E_BlockType eType;
			if ( !TabIsValid(iTab, eType) )
			{
				return;
			}

			// �������������:
			if ( m_MmiReqPlugInSort.m_iMarketId == -1
				&& m_MmiReqPlugInSort.m_iStart	== pRespMerchSort->m_iStart		 
				&& m_MmiReqPlugInSort.m_bDescSort == pRespMerchSort->m_bDescSort				
				&& m_MmiReqPlugInSort.m_eReportSortType == pRespMerchSort->m_eReportSortType)
			{
				// ����������������Ʒ������ �õ������ͼ��Ҫ����Ʒ
				CArray<CMerch *, CMerch *>	aMerchMy;

				const CArray<CMerch *, CMerch *>	&aMerchsInBlock = m_aTabInfomations[iTab].m_Block.m_aMerchs;

				bool32	bEnd = false;
				const int32   iDataBegin = m_aTabInfomations[iTab].m_iDataIndexBegin;
				const int32	  iDataEnd = m_aTabInfomations[iTab].m_iDataIndexEnd;

				int32	iDataFind = 0;

				for ( int i=0; i < pRespMerchSort->m_aMerchs.GetSize() && !bEnd; i++ )		// �������ڸ�ѡ�ɴ��ڵ���Ʒ
				{
					CMerch* pMerchNow = NULL;
					m_pAbsCenterManager->GetMerchManager().FindMerch(pRespMerchSort->m_aMerchs[i].m_StrMerchCode, pRespMerchSort->m_aMerchs[i].m_iMarketId, pMerchNow);

					//
					if ( NULL == pMerchNow )
					{
						continue;
					}

					//
					for ( int j=0; j < aMerchsInBlock.GetSize() ; j++ )
					{
						if ( aMerchsInBlock[j] == pMerchNow )
						{
							iDataFind++;

							if ( iDataFind > iDataBegin )
							{
								aMerchMy.Add(pMerchNow);
							}

							if ( iDataFind > iDataEnd )		// �����Ѿ�����
							{
								bEnd = true;
							}

							break;
						}
					}
				}

				if ( aMerchMy.GetSize() > 0 )
				{
					ReSetGridCellDataByMerchs(aMerchMy);
					UpdateTableContent(TRUE, NULL, false);

					m_GridCtrl.SetSortAscending(!m_MmiReqPlugInSort.m_bDescSort);
					m_GridCtrl.SetSortColumn(m_iSortColumn);

					RequestViewDataCurrentVisibleRow();	
					RequestViewDataSortTimerBegin();
				}
			}
		}
		break;
	case ECTPIRespMainMasukura:
	case ECTPIRespAddPushMainMasukura:
		{
			// ���ͻذ�������Ļذ�һ����. ֻ�����Ͳ�ͬ
			const CMmiRespMainMasukura *pRespMainMasukura = (const CMmiRespMainMasukura *)pRespPlugin;
			for ( int i=0; i < pRespMainMasukura->m_aMainMasukuraList.GetSize() ; i++ )
			{
				CMerch *pMerch = NULL;
				const T_RespMainMasukura &MainMasukura = pRespMainMasukura->m_aMainMasukuraList[i];
				if ( m_pAbsCenterManager && !m_pAbsCenterManager->GetMerchManager().FindMerch(MainMasukura.m_StrCode, MainMasukura.m_uiMarket, pMerch))
				{
					continue;
				}

				m_mapMainMasukura[pMerch] = MainMasukura;
				UpdateTableContent(FALSE, pMerch, FALSE);
			}

		}
		break;
	default:
		break;
	}
}


void CIoViewReport::OnVDataQueryNewStockInfo(const char *pszRecvData)
{
	const char*pRec = pszRecvData;
	int a = 10;
}


void CIoViewReport::OnVDataPublicFileUpdate( IN CMerch *pMerch, E_PublicFileType ePublicFileType )
{
	if ( EPFTF10 == ePublicFileType )	// F10���ݸ���
	{
		UpdateTableContent(FALSE, pMerch, false);
	}
}

CMerch * CIoViewReport::GetMerchXml()
{   //linhc 20100913�����ǰ����û��ѡ����Ʒ���򷵻ػ����й�ע����Ʒ
	CMerch* pMerch = GetGridSelMerch();
	if ( NULL == pMerch )
	{
		pMerch = CIoViewBase::GetMerchXml();
	}
	return pMerch;
}

//linhc 20100909ѡ���иı���Ϣ
void CIoViewReport::OnGridSelRowChanged( NMHDR *pNotifyStruct, LRESULT* pResult )
{
	// ���ҵ�ǰ�����Ƿ���ͼ����ͼ��û�оͲ���Ӧ, �����б�ȫ����ʱ��Ҳ����Ӧ
	{
		CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
		CIoViewBase *pIoView = NULL;
		pIoView = pMainFrame->FindChartIoViewInFrame(NULL, this);
		if ( (NULL==pIoView) || pMainFrame->IsFullScreen())
		{
			return;
		}
	}

	int32 iTab;
	T_BlockDesc::E_BlockType eType;
	if ( !TabIsValid(iTab, eType) )
	{
		return;
	}

	if ( NULL == m_pAbsCenterManager )
	{
		return;	
	}

	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;

	int32 iRow = pItem->iRow;
	int32 iCol = pItem->iColumn;

	// ֪ͨ��Ʒ�����˸ı�
	CGridCellSys *pCell;
	if (IsInCustomGrid())
	{
		m_bCustomGrid = TRUE;
		pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(iRow, 0);
		if ( NULL == pCell || iRow < m_CustomGridCtrl.GetFixedRowCount() )
		{
			return;
		}
	}
	else
	{
		m_bCustomGrid = FALSE;
		pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
		if ( NULL == pCell || iRow < m_GridCtrl.GetFixedRowCount() )
		{
			return;
		}
	}

	CMerch *pMerch = (CMerch *)pCell->GetData();
	if ( NULL == pMerch )
	{
		//ASSERT(0);
		return;
	}

	CReportScheme::E_ReportHeader eHeader = CReportScheme::ERHCount;
	if ( iCol >=0 && iCol < m_ReportHeadInfoList.GetSize() )
	{
		eHeader = m_ReportHeadInfoList[iCol].m_eReportHeader;
	}

	// �ǲ���Ҫ����?
	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());

	// 	int32 iTradeMarket = m_pAbsCenterManager->GetTradeMarketID();
	// 	if (iTradeMarket == pMerch->m_MerchInfo.m_iMarketId)
	// 	{
	// 		// �����µ�
	// 		pMainFrame->OnQuickTrade(1, pMerch, eHeader);		
	// 	}

	//
	pMainFrame->OnShowMerchInChart(pMerch, this);

	if (IsInCustomGrid())
	{
		m_CustomGridCtrl.SetFocus();
	}
	else
	{
		m_GridCtrl.SetFocus();
	}

	//	FIXME:pMainFrame->OnShowMerchInChart(pMerch, this);���д��������´�����ͼ��������
	//	���б�ѡ����Ʒ��ʱ��ͬ�����´����г���ͼ�������г�K����ͼ����ʱ��ͼ��IDΪ100
	if ( NULL != pMainFrame && NULL != pMerch )
	{	
		//	��ȡ��ǰ�ӿ��
		CMDIChildWnd* pActiveMainFrm =(CMDIChildWnd*)pMainFrame->GetActiveFrame();	
		ASSERT(NULL != pActiveMainFrm);	
		vector<CIoViewBase*> vtKline;
		vtKline.clear();
		vector<CIoViewBase*> vtTrend;
		vtTrend.clear();

		//	��ѯ��ǰ�ӿ�������K�����ʱ��ͼ
		BOOL bRet = pMainFrame->FindIoViewInFrame(vtKline, ID_PIC_KLINE, pActiveMainFrm, false, true, 100);	//	��ѯ�ӿ������IDΪ100��K��	
		bRet &= pMainFrame->FindIoViewInFrame(vtTrend, ID_PIC_TREND, pActiveMainFrm, false, true, 100);	//	��ѯ�ӿ������IDΪ100�ķ�ʱ		
		if (bRet && 0 < vtKline.size() && 0 < vtTrend.size())	//	�������K�����ʱ��ͼ
		{
			//	������Ӧ���г�ID��ȡ���г�MERCH
			CMerch* pBigLikeMerch = NULL;
			if ( 1 == pMerch->m_Market.m_Breed.m_iBreedId )
			{				
				if ( 1004 == pMerch->m_Market.m_MarketInfo.m_iMarketId )
				{
					// 2��������С�������С��ָ��
					m_pAbsCenterManager->GetMerchManager().FindMerch(/*L"399005"*/L"399101", 1000, pBigLikeMerch);
				}
				else if ( 1008 == pMerch->m_Market.m_MarketInfo.m_iMarketId )
				{
					// 3�����ڴ�ҵ����ô�ҵ��ָ��
					m_pAbsCenterManager->GetMerchManager().FindMerch(/*L"399006"*/L"399102", 1000, pBigLikeMerch);
				}
				else
				{
					// 4�������ȫ���������ָ
					m_pAbsCenterManager->GetMerchManager().FindMerch(L"399001", 1000, pBigLikeMerch);
				}	
			}

			// ����������ϣ�����Ĭ�϶�����ָ֤��
			if ( NULL == pBigLikeMerch )
			{
				m_pAbsCenterManager->GetMerchManager().FindMerch(L"000001", 0, pBigLikeMerch);
			}
			//	������Ӧ����ͼ��Ʒ
			if (NULL != pBigLikeMerch)	//	�����г�
			{			
				//	���ĸ��´����г�K��
				for (unsigned int i = 0; i < vtKline.size(); ++i)
				{
					pMainFrame->OnViewMerchChanged(vtKline[i]->GetIoViewGroupId(), pBigLikeMerch);	
					vtKline[i]->ForceUpdateVisibleIoView();
				}

				//	���ĸ��´����г���ʱ
				for (unsigned int i = 0; i < vtTrend.size(); ++i)
				{
					pMainFrame->OnViewMerchChanged(vtKline[i]->GetIoViewGroupId(), pBigLikeMerch);	
					vtTrend[i]->ForceUpdateVisibleIoView();
				}				
				
			}
			
		}
	}
}

//--- wangyongxue 2016/8/29 �����๤�����ϡ��Ƿ���������Ӧ��Ϣ
LRESULT CIoViewReport::OnMessageRisePercent(WPARAM wParam,LPARAM lParam)
{
	CCellID cell(0, 3); // �Ƿ���
	if (!m_GridCtrl.IsValid(cell))
	{
		return 0;
	}

	m_eReportHeadType = ERHT_Count;

	if ( m_iSortColumn != cell.col )
	{
		m_iSortColumn = cell.col;
	}	

	CCellID cell1(1,cell.col);

	if (!m_GridCtrl.IsValid(cell1))
	{
		return 0;
	}    

	int32 iTab;
	T_BlockDesc::E_BlockType eType;

	if ( !TabIsValid(iTab, eType) )
	{
		return 0;
	}

	m_GridCtrl.SetHeaderSort(FALSE);
	CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(cell.row,cell.col);
	if ( NULL == pCell)
	{
		return 0;
	}
	TabChange();
	E_MerchReportField eField = (E_MerchReportField)pCell->GetData();

	ResetTabBeginStartIndex(iTab);

	m_MmiBlockRequestSys.m_eMerchReportField = eField;
	m_MmiBlockRequestSys.m_bDescSort = true;


	m_MmiBlockRequestSys.m_eMerchReportField = eField;

	// 	if(m_sortType != 1)
	// 	{
	m_MmiBlockRequestSys.m_bDescSort = true;
	/*}*/

	m_MmiBlockRequestSys.m_iStart = m_aTabInfomations.GetAt(iTab).m_iDataIndexBegin;
	m_MmiBlockRequestSys.m_iCount = m_aTabInfomations.GetAt(iTab).m_iDataIndexEnd - m_aTabInfomations.GetAt(iTab).m_iDataIndexBegin + 1;
	m_MmiBlockRequestSys.m_iBlockId = m_aTabInfomations[iTab].m_Block.m_iMarketId;

	m_bRequestViewSort = true;
	DoRequestViewData(m_MmiBlockRequestSys);

	return 0;
}

//--- wangyongxue 2016/8/30 �����๤�����ϡ��������ݡ���Ӧ��Ϣ
LRESULT CIoViewReport::OnMessageFinance(WPARAM wParam,LPARAM lParam)
{
	m_eReportHeadType  = (E_ReportHeadType)wParam;
	m_eMarketReportType = ERTFinance;
	//	
	CReportScheme::Instance()->GetReportHeadInfoList(m_eMarketReportType,m_ReportHeadInfoList,m_iFixCol);
	TabChange();

	return 0;
}

bool32 CIoViewReport::BeSpecial()
{
	int32 iTab;
	T_BlockDesc::E_BlockType eType;
	if (!TabIsValid(iTab, eType))
	{
		return false;
	}

	CMarket* pMarket = m_aTabInfomations.GetAt(iTab).m_Block.m_pMarket;

	if ( NULL == pMarket )
	{
		// ֻ��ϵͳ���Ҫ���
		return false;
	}

	return (pMarket->m_MarketInfo.m_iMarketId == 17001);
}

//
const T_SimpleTabInfo* CIoViewReport::GetSimpleTabInfo(int32 iTabIndex)
{
	for (unsigned int i = 0; i < m_SimpleTabInfoClass.aTabInfos.size(); i++)
	{
		if ( m_SimpleTabInfoClass.aTabInfos[i].m_iTabIndex == iTabIndex )
		{
			return &m_SimpleTabInfoClass.aTabInfos[i];
		}
	}

	return NULL;
}

void CIoViewReport::InitialTabInfos()
{
	int32 iTabCount = m_SimpleTabInfoClass.aTabInfos.size();
	m_aTabInfomations.RemoveAll();
	m_aTabInfomations.SetSize(0, iTabCount);
	//
	int32 iTabIndex = 0;

	//
	for ( int32 i = 0; i < iTabCount; i++, iTabIndex++ )
	{
		const T_SimpleTabInfo* pSimleTabInfo = &m_SimpleTabInfoClass.aTabInfos[i];

		//
		if ( iTabIndex == pSimleTabInfo->m_iTabIndex )
		{
			T_TabInfo tmp;
			m_aTabInfomations.Add(tmp);
			T_TabInfo &stTabInfo = m_aTabInfomations[m_aTabInfomations.GetUpperBound()];
			stTabInfo.m_eTabType	= pSimleTabInfo->m_eTabType;
			stTabInfo.m_StrShowName	= pSimleTabInfo->m_StrShowName;
			bool32 bFound = false;

			//
			if ( ETITCollection == pSimleTabInfo->m_eTabType )
			{		
				// �������͵Ĵ���
				if ( T_BlockDesc::EBTBlockLogical == pSimleTabInfo->m_eBlockType )
				{
					// �߼����, �ҵ�Ĭ�ϵ�, ��ֵ
					CBlockCollection* pBlockCollection = CBlockConfig::Instance()->GetBlockCollectionById(pSimleTabInfo->m_iID);

					if ( NULL != pBlockCollection )
					{
						CBlockLikeMarket* pBlock = pBlockCollection->FindBlock(pSimleTabInfo->m_iIdDefault);

						if ( NULL != pBlock )
						{
							stTabInfo.m_Block.m_iMarketId		= pBlock->m_blockInfo.m_iBlockId;
							stTabInfo.m_Block.m_eType			= pSimleTabInfo->m_eBlockType;
							stTabInfo.m_Block.m_StrBlockName	= pBlock->m_blockInfo.m_StrBlockName;
							stTabInfo.m_Block.m_aMerchs.Copy(pBlock->m_blockInfo.m_aSubMerchs);
							bFound = true;
						}
						else
						{
							// ... fangz 1214 ���������δ����
							//ASSERT(0);
						}
					}
					else
					{
						CBlockLikeMarket* pBlock = CBlockConfig::Instance()->FindBlock(pSimleTabInfo->m_iIdDefault);

						if ( NULL == pBlock )
						{
							// ����Ĭ�ϻ���a
							pBlock = CBlockConfig::Instance()->GetDefaultMarketClassBlock();
						}

						if ( NULL != pBlock )
						{
							stTabInfo.m_Block.m_iMarketId		= pBlock->m_blockInfo.m_iBlockId;
							stTabInfo.m_Block.m_eType			= pSimleTabInfo->m_eBlockType;
							stTabInfo.m_Block.m_StrBlockName	= pBlock->m_blockInfo.m_StrBlockName;
							stTabInfo.m_Block.m_aMerchs.Copy(pBlock->m_blockInfo.m_aSubMerchs);
							bFound = true;
						}
						else
						{
							// ... fangz 1214 ���������δ����
							//ASSERT(0);
						}
					}
				}
				else if ( T_BlockDesc::EBTUser == pSimleTabInfo->m_eBlockType )
				{
					// �û����, ����ѡ������
					T_Block* pDefaultBlock = CUserBlockManager::Instance()->GetServerBlock();
					if ( NULL != pDefaultBlock )
					{
						stTabInfo.m_Block.m_eType		= pSimleTabInfo->m_eBlockType;
						stTabInfo.m_Block.m_iMarketId	= 0;
						stTabInfo.m_Block.m_StrBlockName= pDefaultBlock->m_StrName;
						bFound = true;
					}
					else
					{
						CArray<T_Block, T_Block&> aBlock;
						CUserBlockManager::Instance()->GetBlocks(aBlock);

						if ( aBlock.GetSize() > 0 )
						{
							T_Block* pBlock = &aBlock.GetAt(0);

							//
							stTabInfo.m_Block.m_eType		= pSimleTabInfo->m_eBlockType;
							stTabInfo.m_Block.m_iMarketId	= 0;
							stTabInfo.m_Block.m_StrBlockName= pBlock->m_StrName;
							bFound = true;
						}	
						else
						{
							// ... fangz 1214 ���������δ����
							//ASSERT(0);
						}
					}
				}								
			}				
			else if ( ETITEntity == pSimleTabInfo->m_eTabType )
			{
				if ( T_BlockDesc::EBTPhysical == pSimleTabInfo->m_eBlockType )
				{
					// �����г�
					int32 iIdSystem = pSimleTabInfo->m_iID;

					if ( NULL == m_pAbsCenterManager )
					{
						continue;
					}

					//
					CMarket* pMarket = NULL;
					m_pAbsCenterManager->GetMerchManager().FindMarket(iIdSystem, pMarket);

					if ( NULL == pMarket )
					{
						////ASSERT(0);

						// ������, �Ҹ��汳��
						iIdSystem = 1001;
						m_pAbsCenterManager->GetMerchManager().FindMarket(iIdSystem, pMarket);						
					}

					if ( NULL != pMarket )
					{						
						stTabInfo.m_Block.m_StrBlockName = pMarket->m_MarketInfo.m_StrCnName;
						stTabInfo.m_Block.m_eType		= T_BlockDesc::EBTPhysical;
						stTabInfo.m_Block.m_iMarketId	= pMarket->m_MarketInfo.m_iMarketId;
						stTabInfo.m_Block.m_aMerchs.Copy(pMarket->m_MerchsPtr);	
						bFound = true;
					}					
				}
				else if ( T_BlockDesc::EBTBlockLogical == pSimleTabInfo->m_eBlockType )
				{
					// �߼����
					int32 iIdLogic = pSimleTabInfo->m_iID;

					CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(pSimleTabInfo->m_iID);
					if ( NULL == pBlock )
					{
						////ASSERT(0);

						// ������, �Ҹ��汳��
						CBlockConfig::Instance()->GetDefaultMarketClassBlockId(iIdLogic);								
						pBlock = CBlockConfig::Instance()->FindBlock(iIdLogic);
					}

					// 
					if ( NULL != pBlock )
					{						
						stTabInfo.m_Block.m_StrBlockName = pBlock->m_blockInfo.m_StrBlockName;
						stTabInfo.m_Block.m_eType		= T_BlockDesc::EBTBlockLogical;
						stTabInfo.m_Block.m_iMarketId	= iIdLogic;
						stTabInfo.m_Block.m_aMerchs.Copy(pBlock->m_blockInfo.m_aSubMerchs);	
						bFound = true;
					}
				}
			}			

			//
			//m_aTabInfomations.Add(stTabInfo);
			if ( !bFound )
			{
				// ��Ҫ�ҵ�һ�����������滻
				int32 iIdSystem = 1001;
				CMarket *pMarket = NULL;
				if(m_pAbsCenterManager)
				{
					m_pAbsCenterManager->GetMerchManager().FindMarket(iIdSystem, pMarket);	
					if ( NULL == pMarket )
					{
						// �ҵ���һ���г����滻
						if ( m_pAbsCenterManager->GetMerchManager().m_BreedListPtr.GetSize() > 0
							&& m_pAbsCenterManager->GetMerchManager().m_BreedListPtr[0]->m_MarketListPtr.GetSize() > 0 )
						{
							pMarket = m_pAbsCenterManager->GetMerchManager().m_BreedListPtr[0]->m_MarketListPtr[0];
						}
					}
				}

				if ( NULL != pMarket )
				{
					stTabInfo.m_Block.m_StrBlockName = pMarket->m_MarketInfo.m_StrCnName;
					stTabInfo.m_Block.m_eType		= T_BlockDesc::EBTPhysical;
					stTabInfo.m_Block.m_iMarketId	= pMarket->m_MarketInfo.m_iMarketId;
					stTabInfo.m_Block.m_aMerchs.Copy(pMarket->m_MerchsPtr);
				}
				else
				{
					m_aTabInfomations.RemoveAt(m_aTabInfomations.GetUpperBound());	// ֻ���Ƴ����һ����
				}
			}
		}
	}
}

int32 CIoViewReport::GetAutoPageTimerPeriod()
{
	static bool32 bInit = false;
	if ( !bInit )
	{
		std::string str;
		if ( CEtcXmlConfig::Instance().ReadEtcConfig(KStrXMLReportSection, KStrXMLReportAutoPagePeriodKey, NULL, str) )
		{
			s_iAutoPageTimerPeriodSecond = atoi(str.c_str());
			if ( s_iAutoPageTimerPeriodSecond < 4 )
			{
				s_iAutoPageTimerPeriodSecond = 4;
			}
			else if ( s_iAutoPageTimerPeriodSecond > 60 )
			{
				s_iAutoPageTimerPeriodSecond = 60;
			}
		}
	}
	return s_iAutoPageTimerPeriodSecond;
}

void CIoViewReport::SetAutoPageTimerPeriod( int32 iPeriodSecond )
{
	if ( iPeriodSecond < 4 )
	{
		iPeriodSecond = 4;
	}
	if ( iPeriodSecond > 60 )
	{
		iPeriodSecond = 60;
	}
	int32 iOld = s_iAutoPageTimerPeriodSecond;
	s_iAutoPageTimerPeriodSecond = iPeriodSecond;
	if ( iOld != s_iAutoPageTimerPeriodSecond )
	{
		char buf[100];
		_snprintf(buf, sizeof(buf), "%d", s_iAutoPageTimerPeriodSecond);
		CEtcXmlConfig::Instance().WriteEtcConfig(KStrXMLReportSection, KStrXMLReportAutoPagePeriodKey, buf);
	}
}

void CIoViewReport::OnAutoPage(UINT nId)
{
	if ( nId == ID_REPORT_AUTOPAGE )
	{
		if ( IsAutoPageStarted() )
		{
			StopAutoPage();
		}
		else
		{
			StartAutoPage(true);
		}
	}
	else
	{
		// �����Ի�������ʱ��
		CString StrInput;
		StrInput.Format(_T("%d"), GetAutoPageTimerPeriod());
		if ( CDlgInputShowName::GetInputString(StrInput, _T("�������Զ���ҳʱ��[4-60��]"), _T("�Զ���ҳʱ������")) == IDOK )
		{
			int32 iPeriod = _ttoi(StrInput);
			if ( iPeriod < 4 || iPeriod > 60 )
			{
				MessageBox(_T("�������ʱ�䲻����Ҫ�󣬱�����!"));
			}
			else
			{
				SetAutoPageTimerPeriod(iPeriod);
			}
		}
	}
}

void CIoViewReport::StartAutoPage(bool32 bPrompt/* = false*/)
{
	if ( bPrompt && MessageBox(_T("�Ƿ������Զ���ҳ��"), _T("��ʾ"), MB_ICONQUESTION |MB_YESNO) != IDYES )
	{
		StopAutoPage();
		return;
	}

	m_iAutoPageTimerPeriod = GetAutoPageTimerPeriod();
	m_iAutoPageTimerCount = m_iAutoPageTimerPeriod;
	SetTimer(KTimerPeriodAutoPage, 1000, NULL);
}

void CIoViewReport::StopAutoPage()
{
	m_iAutoPageTimerPeriod = INT_MAX;
	m_iAutoPageTimerCount = 0;
	KillTimer(KTimerPeriodAutoPage);
}

bool32 CIoViewReport::IsAutoPageStarted()
{
	return INT_MAX != m_iAutoPageTimerPeriod;
}

void CIoViewReport::OnMerchMarkChanged( int32 iMarketId, const CString &StrMerchCode, const T_MarkData &MarkOldData )
{
	CMerch *pMerch = ConvertMerchKeyToMerch(iMarketId, StrMerchCode);
	if ( NULL != pMerch )
	{
		T_MarkData MarkNowData;
		if ( !CMarkManager::Instance().QueryMark(iMarketId, StrMerchCode, MarkNowData)
			|| MarkNowData.m_eType != MarkOldData.m_eType
			|| EMT_Text == MarkNowData.m_eType )
		{
			// ���ͱ仯&�ı���Ҫ��ʾ����
			UpdateTableContent(FALSE, pMerch, FALSE);
		}
	}
}

void CIoViewReport::OnEscBackFrameMerch( CMerch *pMerch )
{
	if ( NULL != pMerch && m_bActive )	// ��������ͼ�����л�
	{
		SetGridSelByMerch(pMerch);
	}
}

CMerch* CIoViewReport::GetDragMerch()
{
	CMerch *pMerch = GetGridSelMerch();
	return pMerch;
}

bool32 CIoViewReport::BeValidDragArea(const CPoint& pt)
{
	CPoint ptRow = pt;
	ScreenToClient(&ptRow);

	if (IsInCustomGrid())
	{
		if ( NULL == m_CustomGridCtrl.GetSafeHwnd() )
		{
			return false;
		}

		// ��ǰ��겻�����ϻ��߳�������ֻ��һ�У�����false
		ptRow.y -= m_RctSperator.bottom;
		CCellID cell = m_CustomGridCtrl.GetCellFromPt(ptRow);
		if (!m_CustomGridCtrl.IsValid(cell) || (m_CustomGridCtrl.GetRowCount() <= 2))
		{
			return false;
		}

		//
		CRect RectGrid;
		m_CustomGridCtrl.GetWindowRect(RectGrid);

		//
		if ( RectGrid.PtInRect(pt) )
		{
			return true;
		}

		//
		return false;
	}
	else
	{
		if ( NULL == m_GridCtrl.GetSafeHwnd() )
		{
			return false;
		}

		// ��ǰ��겻�����ϻ��߳�������ֻ��һ�У�����false
		CCellID cell = m_GridCtrl.GetCellFromPt(ptRow);
		if (!m_GridCtrl.IsValid(cell) || (m_GridCtrl.GetRowCount() <= 2))
		{
			return false;
		}

		//
		CRect RectGrid;
		m_GridCtrl.GetWindowRect(RectGrid);

		//
		if ( RectGrid.PtInRect(pt) )
		{
			return true;
		}

		//
		return false;
	}
}

CMerch* CIoViewReport::GetNextMerch(CMerch* pMerchNow, bool32 bPre)
{
	CMerch* pMerchNext = NULL;

	if ( bPre )
	{
		GetPreMerch(pMerchNow, pMerchNext);
	}
	else
	{
		GetAftMerch(pMerchNow, pMerchNext);
	}

	if ( NULL == pMerchNext )
	{
		// ��������,����Ƿ������г�����鿴��ǰtab��Ϣ����tab��Ϣ����Ѱ��һ��(ѭ������)
		// ��������£�ǰ��Ӧ���Ѿ������˺��ʵ���Ʒ
		// �������Ʒ�����Ͳ���������б��У�ǰ�����������״̬Ҳ�᷵��NULL
		ASSERT( -1 == m_GridCtrl.GetSortColumn() && !m_bRequestViewSort );
		int32 iTab;
		T_BlockDesc::E_BlockType eType;
		if ( TabIsValid(iTab, eType) 
			&& T_BlockDesc::EBTPhysical != eType
			&& !m_bRequestViewSort )
		{
			const T_TabInfo &tab = m_aTabInfomations[iTab];
			int32 i=0;
			const int32 iMerchCount = tab.m_Block.m_aMerchs.GetSize();
			for ( ; i < iMerchCount ; ++i )
			{
				if ( tab.m_Block.m_aMerchs[i] == pMerchNow )
				{
					break;
				}
			}
			if ( i>=0 && i < iMerchCount )
			{
				if ( bPre )
				{
					pMerchNext = tab.m_Block.m_aMerchs[ (i-1+iMerchCount)%iMerchCount ];
				}
				else
				{
					pMerchNext = tab.m_Block.m_aMerchs[ (i+1)%iMerchCount ];
				}	
			}
		}
		if ( NULL == pMerchNext )
		{
			// ����û��Լ�����������Ʒ, �������Ʒ�ֲ��ڱ���Ʒ�б���, �� viewdata ��ȥ����һ��
			pMerchNext = CIoViewBase::GetNextMerch(pMerchNow, bPre);
		}
	}

	return pMerchNext;
}

void CIoViewReport::OnDoInitFromXml()
{

	ReCalcGridSizeColFromXml();
	ReCalcCustomGridSizeColFromXml();
	DoReportSizeChangeContent();
	AutoSizeColFromXml();
	

}

void CIoViewReport::DoReportSizeChangeContent()
{
		
	ReSetGridContent();

	if ( m_bRequestViewSort )
	{
		RequestViewDataSortAsync();
	}
	else
	{
		UpdateTableContent(TRUE, NULL, FALSE);
		RequestViewDataCurrentVisibleRowAsync();
	}

	if ( m_GridCtrl.GetHeaderSort() )
	{
		// ����ѡ����, Ҫ����һ��
		bool32 bAscend = m_GridCtrl.GetSortAscending();
		int32 iCol = m_GridCtrl.GetSortColumn();
		m_GridCtrl.SortItems(iCol, bAscend);
	}
}

void CIoViewReport::CalcMaxGridRowCount()
{
	m_iMaxGridRowCount = 0;

	CRect rcGrid(0,0,0,0);
	m_GridCtrl.GetClientRect(rcGrid);
	int32 iDefHeight = m_GridCtrl.GetDefCellHeight();
	if ( iDefHeight > 0 )
	{
		m_iMaxGridRowCount = rcGrid.Height() / iDefHeight;	// ԭ�����жำֵ��������stoneȡ��
	}

	//
	m_iMaxCustomGridRowCount = 0;
	CRect rcCustomGrid(0,0,0,0);
	m_CustomGridCtrl.GetClientRect(rcCustomGrid);
	iDefHeight = m_CustomGridCtrl.GetDefCellHeight();
	if ( iDefHeight > 0 )
	{
		m_iMaxCustomGridRowCount = rcCustomGrid.Height()/iDefHeight +5;	// ��΢�ำֵ����
	}
}

void CIoViewReport::ResetTabBeginStartIndex( int32 iTab )
{
	if ( iTab <0 || iTab >= m_aTabInfomations.GetSize() )
	{
		return;
	}

	m_aTabInfomations[iTab].m_iDataIndexBegin = -1;	// ��Ϊ��Чֵ����SetTabParams����
	SetTabParams(iTab);
}

void CIoViewReport::GetMerchXmls( OUT MerchArray &aMerchs )
{
	if (m_bCustomGrid)
	{
		aMerchs.RemoveAll();
		POSITION pos = m_CustomGridCtrl.GetFirstSelectedRowPosition();
		while ( NULL != pos )
		{
			int32 iRow = m_CustomGridCtrl.GetNextSelectedRow(pos);
			if ( iRow < m_CustomGridCtrl.GetFixedRowCount() )
			{
				continue;
			}

			CMerch *pMerch = (CMerch *)m_CustomGridCtrl.GetItemData(iRow, 0);
			if ( NULL == pMerch )
			{
				continue;
			}
			aMerchs.Add(pMerch);
		}

		if ( aMerchs.GetSize() <= 0 )
		{
			CIoViewBase::GetMerchXmls(aMerchs);
		}
		m_bCustomGrid = FALSE;
		return;
	}

	aMerchs.RemoveAll();
	POSITION pos = m_GridCtrl.GetFirstSelectedRowPosition();
	while ( NULL != pos )
	{
		int32 iRow = m_GridCtrl.GetNextSelectedRow(pos);
		if ( iRow < m_GridCtrl.GetFixedRowCount() )
		{
			continue;
		}

		CMerch *pMerch = (CMerch *)m_GridCtrl.GetItemData(iRow, 0);
		if ( NULL == pMerch )
		{
			continue;
		}
		aMerchs.Add(pMerch);
	}

	if ( aMerchs.GetSize() <= 0 )
	{
		CIoViewBase::GetMerchXmls(aMerchs);
	}
}

const TCHAR KStrSimpleTabFileName[] = _T("ReportSimpleTabInfo.xml");
const char KSZXMLTABINFOCLASS[] = "class";
const char KSZXMLTABINFOCLASSId[] = "id";
const char KSZXMLTABINFOCLASSName[] = "name";
const char KSZXMLTABINFO[]		= "tabinfo";
const char KSZXMLTABINFOID[]	= "id";
const char KSZXMLTABINFOIdDefault[]	= "IdDefault";
const char KSZXMLTABINFOTabIndex[]	= "TabIndex";
const char KSZXMLTABINFOTabType[]	= "TabType";
const char KSZXMLTABINFOShowName[]	= "ShowName";
const char KSZXMLTABINFOBlockType[]	= "BlockType";
bool32 SimpleTabInfoToXml( const T_SimpleTabInfo &tabInfo, OUT TiXmlElement &tiEleThis )
{
	tiEleThis.SetValue(KSZXMLTABINFO);

	char *pszTmp = CEtcXmlConfig::WideToMulti(tabInfo.m_StrShowName, CP_UTF8);
	if ( NULL == pszTmp )
	{
		return false;
	}
	tiEleThis.SetAttribute(KSZXMLTABINFOShowName, pszTmp);
	delete []pszTmp;
	pszTmp = NULL;

	char szBuf[20];
	sprintf(szBuf, "%d", tabInfo.m_iID);
	tiEleThis.SetAttribute(KSZXMLTABINFOID, szBuf);

	sprintf(szBuf, "%d", tabInfo.m_iIdDefault);
	tiEleThis.SetAttribute(KSZXMLTABINFOIdDefault, szBuf);

	sprintf(szBuf, "%d", tabInfo.m_iTabIndex);
	tiEleThis.SetAttribute(KSZXMLTABINFOTabIndex, szBuf);

	sprintf(szBuf, "%d", tabInfo.m_eTabType);
	tiEleThis.SetAttribute(KSZXMLTABINFOTabType, szBuf);

	sprintf(szBuf, "%d", tabInfo.m_eBlockType);
	tiEleThis.SetAttribute(KSZXMLTABINFOBlockType, szBuf);

	return true;
}

bool32 SimpleTabInfoFromXml( OUT T_SimpleTabInfo &tabInfo, IN TiXmlElement &tiEleThis )
{
	const char *pszAttri = NULL;

	pszAttri = tiEleThis.Value();
	// ��strcmpi �ĳ���strcmp��Ӧ��ûʲôӰ�죬��������
	if ( NULL == pszAttri || _stricmp(pszAttri, KSZXMLTABINFO) != 0 )
	{
		return false;
	}

	pszAttri = tiEleThis.Attribute(KSZXMLTABINFOShowName);
	wchar_t *pwszTmp = CEtcXmlConfig::MultiToWide(pszAttri, CP_UTF8);
	if ( NULL == pwszTmp )
	{
		return false;
	}
	tabInfo.m_StrShowName = pwszTmp;
	delete []pwszTmp;
	pwszTmp = NULL;

	pszAttri = tiEleThis.Attribute(KSZXMLTABINFOID);
	if ( NULL == pszAttri )
	{
		return false;
	}
	tabInfo.m_iID = atoi(pszAttri);

	pszAttri = tiEleThis.Attribute(KSZXMLTABINFOIdDefault);
	if ( NULL == pszAttri )
	{
		return false;
	}
	tabInfo.m_iIdDefault = atoi(pszAttri);

	pszAttri = tiEleThis.Attribute(KSZXMLTABINFOTabIndex);
	if ( NULL == pszAttri )
	{
		return false;
	}
	tabInfo.m_iTabIndex = atoi(pszAttri);

	pszAttri = tiEleThis.Attribute(KSZXMLTABINFOTabType);
	if ( NULL == pszAttri )
	{
		return false;
	}
	tabInfo.m_eTabType = (E_TabInfoType)atoi(pszAttri);

	pszAttri = tiEleThis.Attribute(KSZXMLTABINFOBlockType);
	if ( NULL == pszAttri )
	{
		return false;
	}
	tabInfo.m_eBlockType = (T_BlockDesc::E_BlockType)atoi(pszAttri);

	if ( tabInfo.m_eTabType >= ETITCount )
	{
		return false;
	}

	if ( tabInfo.m_eBlockType >= T_BlockDesc::EBTCount )
	{
		return false;
	}

	if ( tabInfo.m_iTabIndex < 0 )
	{
		ASSERT( 0 );	// ��Ч��������Ȼ����ƥ��
	}

	return true;
}

bool32 CIoViewReport::LoadSimpleTabInfoFromXml( OUT SimpleTabClassMap &mapTabClass )
{
	CString StrPath = CPathFactory::GetPublicConfigPath();
	StrPath += KStrSimpleTabFileName;

	TiXmlDocument tiDoc;
	char *pszFilePath = CEtcXmlConfig::WideToMulti(StrPath);
	if ( !tiDoc.LoadFile(pszFilePath) )
	{
		delete []pszFilePath;
		return false;
	}
	delete []pszFilePath;
	pszFilePath = NULL;

	TiXmlElement *pRoot = tiDoc.RootElement();
	if ( NULL == pRoot )
	{
		return false;
	}

	const char *pszAttri = NULL;
	for ( TiXmlElement *pEleClass=pRoot->FirstChildElement(KSZXMLTABINFOCLASS); NULL != pEleClass ; pEleClass=pEleClass->NextSiblingElement(KSZXMLTABINFOCLASS) )
	{
		T_SimpleTabInfoClass tabClass;
		tabClass.iId = 0;	// ����ʼ��Ĭ��

		pszAttri = pEleClass->Attribute(KSZXMLTABINFOCLASSId);
		if ( NULL != pszAttri )
		{
			tabClass.iId = atol(pszAttri);
		}

		pszAttri = pEleClass->Attribute(KSZXMLTABINFOCLASSName);
		if ( NULL != pszAttri )
		{
			wchar_t *pwszTmp = CEtcXmlConfig::MultiToWide(pszAttri, CP_UTF8);
			ASSERT( NULL != pwszTmp );
			if ( NULL != pwszTmp )
			{
				tabClass.StrName = pwszTmp;
			}
			delete []pwszTmp;
		}
		if ( 0 != tabClass.iId && tabClass.StrName.IsEmpty() )
		{
			ASSERT( 0 );	// û��ָ������
		}

		for ( TiXmlElement *pEle=pEleClass->FirstChildElement(KSZXMLTABINFO); NULL != pEle ; pEle=pEle->NextSiblingElement(KSZXMLTABINFO) )
		{
			T_SimpleTabInfo tab(1001, 1001,	10, L"", ETITEntity, T_BlockDesc::EBTPhysical);
			if ( SimpleTabInfoFromXml(tab, *pEle) )
			{
				bool32 bApp = true;
				for ( int32 i=tabClass.aTabInfos.size()-1; i >= 0 ; --i )
				{
					if ( tabClass.aTabInfos[i].m_iTabIndex < tab.m_iTabIndex )
					{
						tabClass.aTabInfos.insert(tabClass.aTabInfos.begin()+i, tab);
						bApp = false;
						break;
					}
					else if ( tabClass.aTabInfos[i].m_iTabIndex == tab.m_iTabIndex )
					{
						tabClass.aTabInfos[i] = tab;	// ��index���滻
						bApp = false;
						break;
					}
				}
				if ( bApp )
				{
					tabClass.aTabInfos.push_back(tab);
				}
			}
		}

		mapTabClass[tabClass.iId] = tabClass;
	}

	return true;
}

bool32 CIoViewReport::LoadSimpleTabInfoFromCommCfg(OUT SimpleTabClassMap &mapTabClass)
{
	auth::T_ReportTabInfo *pReportTable ;
	int iCount = 0;
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	pDoc->m_pAutherManager->GetInterface()->GetServiceDispose()->GetReportTabList(&pReportTable, iCount);

	T_SimpleTabInfoClass tabClass;
	tabClass.iId = 0;	// ����ʼ��Ĭ��
	tabClass.StrName = L"Ĭ��";

	for ( int i=0; i<iCount; i++ )
	{
		T_SimpleTabInfo tab(1001, 1001,	0, L"", ETITEntity, T_BlockDesc::EBTPhysical);
		auth::T_ReportTabInfo tReport = pReportTable[i];
		tab.m_iTabIndex = i;
		tab.m_iID = tReport.iMarketCode;
		tab.m_iIdDefault = tReport.iMarketCode;
		tab.m_StrShowName = tReport.wszMName;
		/*	if ( SimpleTabInfoFromXml(tab, *pEle) )*/
		{
			bool32 bApp = true;
			for ( int32 j=tabClass.aTabInfos.size()-1; j >= 0 ; --j )
			{
				if ( tabClass.aTabInfos[j].m_iTabIndex < tab.m_iTabIndex )
				{
					tabClass.aTabInfos.insert(tabClass.aTabInfos.begin()+j, tab);
					bApp = false;
					break;
				}
				else if ( tabClass.aTabInfos[j].m_iTabIndex == tab.m_iTabIndex )
				{
					tabClass.aTabInfos[j] = tab;	// ��index���滻
					bApp = false;
					break;
				}
			}
			if ( bApp )
			{
				tabClass.aTabInfos.push_back(tab);
			}
		}
	}
	pDoc->m_pAutherManager->GetInterface()->GetServiceDispose()->ReleaseData(pReportTable);

	mapTabClass[tabClass.iId] = tabClass;

	return true;
}	

void CIoViewReport::SaveSimpleTabInfoToXml( )
{
	TiXmlDocument tiDoc;

	TiXmlElement *pRoot = ConstructGGTongAppXmlDocHeader(tiDoc, "simpleTabInfo", NULL, NULL, NULL);	
	if ( NULL == pRoot )
	{
		return;
	}

	const SimpleTabClassMap &simMap = GetSimpleTabClassConfig();
	for ( SimpleTabClassMap::const_iterator it=simMap.begin(); it != simMap.end() ; ++it )
	{
		const T_SimpleTabInfoClass &tabClass = it->second;

		TiXmlElement tiEle(KSZXMLTABINFOCLASS);
		TiXmlElement *pTiClass = (TiXmlElement *)pRoot->InsertEndChild(tiEle);

		char szBuf[20];
		sprintf(szBuf, "%d", tabClass.iId);
		pTiClass->SetAttribute(KSZXMLTABINFOCLASSId, szBuf);

		char *pszTmp = CEtcXmlConfig::WideToMulti(tabClass.StrName, CP_UTF8);
		if ( NULL != pszTmp )
		{
			pTiClass->SetAttribute(KSZXMLTABINFOCLASSName, pszTmp);
			delete []pszTmp;
			pszTmp = NULL;
		}

		for ( unsigned int i=0; i < tabClass.aTabInfos.size() ; ++i )
		{
			TiXmlElement tiElement(KSZXMLTABINFO);
			if ( SimpleTabInfoToXml(tabClass.aTabInfos[i], tiElement) )
			{
				pTiClass->InsertEndChild(tiElement);
			}
		}
	}

	CString StrPath = CPathFactory::GetPublicConfigPath();
	StrPath += KStrSimpleTabFileName;
	char *pszFilePath = CEtcXmlConfig::WideToMulti(StrPath);
	tiDoc.SaveFile(pszFilePath);
	delete []pszFilePath;
	pszFilePath = NULL;
}

void CIoViewReport::InitialSimpleTabInfo()
{
	const SimpleTabClassMap &simMap = GetSimpleTabClassConfig();

	SimpleTabClassMap::const_iterator it = simMap.find(m_SimpleTabInfoClass.iId);
	if ( it == simMap.end() )
	{
		// û�У���ô����Ĭ�ϵ�
		ASSERT( 0 );
		it = simMap.find(SIMPLETABINFOCLASS_DEFAULTID);
	}

	m_SimpleTabInfoClass.aTabInfos.clear();
	if ( it == simMap.end() )
	{
		// Ĭ�ϵĶ�û��~
		ASSERT( 0 );
	}
	else
	{
		m_SimpleTabInfoClass.StrName = it->second.StrName;
		const SimpleTabInfoArray &aTmp = it->second.aTabInfos;

		for ( unsigned int j=0; j < aTmp.size() ; ++j )
		{
			T_SimpleTabInfo stTab;
			stTab.m_iTabIndex = j;
			m_SimpleTabInfoClass.aTabInfos.push_back(stTab);
		}

		// ʹ��xml�е����
		for ( unsigned int j=0; j < aTmp.size() ; ++j )
		{
			for ( unsigned int i=0; i < m_SimpleTabInfoClass.aTabInfos.size() ; ++i )
			{
				if ( m_SimpleTabInfoClass.aTabInfos[i].m_iTabIndex == aTmp[j].m_iTabIndex )
				{
					m_SimpleTabInfoClass.aTabInfos[i] = aTmp[j];

					if (((KiFixTabCounts-1)==m_SimpleTabInfoClass.aTabInfos[i].m_iTabIndex) && (KiFixTabCounts!=aTmp.size()))
					{
						m_SimpleTabInfoClass.aTabInfos[i].m_StrShowName = KStrFixTabName;
					}
					break;
				}
			}
		}
	}
}

const SimpleTabClassMap & CIoViewReport::GetSimpleTabClassConfig()
{
	if ( sm_mapSimpleTabConfig.empty() )
	{
		// ��ʼ�����
		// �ȸ����������Ĭ��
		T_SimpleTabInfoClass tabClass;
		tabClass.iId = SIMPLETABINFOCLASS_DEFAULTID;
		tabClass.StrName = _T("Ĭ��");
		for ( int32 i=0; i < s_KiTabCounts ; ++i )
		{
			tabClass.aTabInfos.push_back(s_KaTabInfos[i]);
		}
		sm_mapSimpleTabConfig[tabClass.iId] = tabClass;

		LoadSimpleTabInfoFromXml(sm_mapSimpleTabConfig);
		//	LoadSimpleTabInfoFromCommCfg(sm_mapSimpleTabConfig);
	}

	return sm_mapSimpleTabConfig;
}

void CIoViewReport::ChangeSimpleTabClass( int32 iNewClass )
{
	if ( iNewClass == m_SimpleTabInfoClass.iId )
	{
		return;	// ��ͬ
	}
	const SimpleTabClassMap &simMap = GetSimpleTabClassConfig();
	SimpleTabClassMap::const_iterator it = simMap.find(iNewClass);
	if ( it != simMap.end() )
	{
		// ���³�ʼ��
		m_SimpleTabInfoClass.iId = iNewClass;
		InitialSimpleTabInfo();
		InitialTabInfos();
		ReCreateTabWnd();
		Invalidate(TRUE);
	}
	else
	{
		ASSERT( 0 );	// û���������
	}
}

void CIoViewReport::AutoSizeColFromXml()
{
	if (!m_bAutoSizeColFromXml || m_bSuccessAutoSizeColXml)
	{
		return;
	}

	//
	int32 iWidthGrid = m_GridCtrl.GetVirtualWidth();
	int32 iWidthCustomGrid = m_CustomGridCtrl.GetVirtualWidth();
	CRect rt;
	GetClientRect(rt);

	if (rt.IsRectEmpty() && NULL != m_pIoViewManager)
	{
		m_pIoViewManager->GetClientRect(rt);
	}

	//
	int32 iWidthClient = rt.Width();

	// �ɸ�����Ҫ�ſ�ExpandColumnsToFit
	//if (iWidthGrid >0 && iWidthClient > 0 && iWidthGrid < iWidthClient)
	//{
	//	//m_GridCtrl.ExpandColumnsToFit(TRUE);
	//	// m_bSuccessAutoSizeColXml = true;

	//}
	//else
	//{
	//	ReCalcGridSizeColFromXml();
	//	//m_GridCtrl.ExpandColumnsToFit(TRUE);
	//}
	ReCalcGridSizeColFromXml();

	if (m_bShowCustomGrid)
	{
		if (iWidthCustomGrid >0 && iWidthClient > 0 && iWidthCustomGrid < iWidthClient)
		{
			m_CustomGridCtrl.ExpandColumnsToFit(FALSE);
			// m_bSuccessAutoSizeColXml = true;
		}
		else
		{
			//ReCalcCustomGridSizeColFromXml();
			//m_GridCtrl.ExpandColumnsToFit(TRUE);
		}
	}
}

void CIoViewReport::ReCalcGridSizeColFromXml()
{
	if (m_aTabInfomations.GetSize() <= 0)
	{
		return;
	}

	// ���	
	m_ReportHeadInfoList.RemoveAll();

	int32 i,iSize,iTab;
	T_BlockDesc::E_BlockType eType;

	if ( !TabIsValid(iTab, eType) )
	{
		return;
	}

	T_BlockDesc BlockDesc = m_aTabInfomations.GetAt(iTab).m_Block;

	// �õ���ͷ��Ϣ
	if ( !SetHeadInfomationList() )
	{
		return;
	}

	//�趨��ͷ
	m_GridCtrl.SetFixedColumnCount(m_iFixCol);
	iSize = m_ReportHeadInfoList.GetSize();

	//
	bool32 bFoundForceField = false;

	CStringArray HeaderNames;
	CArray<int32,int32> HeaderFields;	
	CArray<int32,int32> HeaderWidths;

	// ת��
	for ( i = 0; i < iSize; i++ )
	{
		CString StrText = m_ReportHeadInfoList[i].m_StrHeadNameCn;
		ASSERT(StrText.GetLength() > 0 && m_ReportHeadInfoList[i].m_iHeadWidth > 0);

		HeaderNames.Add(StrText);
		HeaderWidths.Add(m_ReportHeadInfoList[i].m_iHeadWidth);

		bool32 bTrans;
		CReportScheme::E_ReportHeader eHeadType = CReportScheme::Instance()->GetReportHeaderEType(StrText);
		E_MerchReportField eField = ReportHeader2MerchReportField(eHeadType, bTrans);

		if ( bTrans )
		{
			HeaderFields.Add((int32)eField);
			if ( BlockDesc.m_iFieldAdd == eField )
			{
				bFoundForceField = true;
			}
		}
		else
		{
			HeaderFields.Add(-1);
		}

		// ����ѡ�ɵ���������ǿ����ʾ���� xl 1220
		// ���г�����ָ�������г���Ʒ�Ķ�����һ�Ž�����ʾ
		bool32 bBuySellPriceExchange = false;
		{
			if ( T_BlockDesc::EBTPhysical == BlockDesc.m_eType )
			{
				bBuySellPriceExchange = IsBuySellPriceExchangeMarket(BlockDesc.m_iMarketId);
			}
			else
			{
				for ( int32 i=0; i < BlockDesc.m_aMerchs.GetSize() ; ++i )
				{
					if ( IsBuySellPriceExchangeMerch(BlockDesc.m_aMerchs[i]) )
					{
						bBuySellPriceExchange = true;
						break;
					}
				}
			}
		}

		if ( bBuySellPriceExchange )
		{
			if ( CReportScheme::ERHBuyPrice == eHeadType )
			{
				HeaderNames[ HeaderNames.GetUpperBound() ] = CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHSellPrice);
			}
			else if ( CReportScheme::ERHSellPrice == eHeadType )
			{
				HeaderNames[ HeaderNames.GetUpperBound() ] = CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHBuyPrice);
			}
		}
	}

	if ( -1 != BlockDesc.m_iFieldAdd && !bFoundForceField )
	{
		bool32 bTrans = false;
		CReportScheme::E_ReportHeader eCurType = MerchReportField2ReportHeader((E_MerchReportField)BlockDesc.m_iFieldAdd, bTrans);

		if ( bTrans )
		{
			// 
			BlockDesc.m_iTypeAdd = eCurType;
			UpdateLocalBlock(i, BlockDesc);

			CString StrText = CReportScheme::GetReportHeaderCnName(eCurType);
			HeaderNames.Add(StrText);
			HeaderFields.Add(BlockDesc.m_iFieldAdd);
			HeaderWidths.Add(CReportScheme::GetReportColDefaultWidth(eCurType));
		}
	} 

	// ���ñ��̶���
	m_GridCtrl.SetFixedRowCount(1);
	iSize = HeaderNames.GetSize();
	m_GridCtrl.SetColumnCount(iSize);
	m_GridCtrl.SetRowHeight(0, 40);
	// �����п�	
	for (i = 0; i < iSize; i++)
	{
		CGridCellSys * pCell = (CGridCellSys *)m_GridCtrl.GetCell(0, i);
		DWORD dwFmt = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX;
		pCell->SetFormat(dwFmt);
		CString StrText = HeaderNames.GetAt(i);

		// ���۱�����óɽ�������ɫ
		pCell->SetDefaultTextColor(ESCVolume);
		pCell->SetFont(GetColumnExLF());

		pCell->SetText(StrText);
		pCell->SetData(HeaderFields.GetAt(i));
		m_GridCtrl.SetColumnWidth(i, HeaderWidths.GetAt(i));
		CReportScheme::E_ReportHeader eTmpType = CReportScheme::Instance()->GetReportHeaderEType(StrText);
		if ( CReportScheme::ERHMerchName == eTmpType
			|| CReportScheme::ERHMerchCode == eTmpType
			|| CReportScheme::ERHRowNo == eTmpType )
		{
			pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		}

		if (CReportScheme::ERHShortLineSelect == eTmpType
			|| CReportScheme::ERHMidLineSelect == eTmpType
			|| CReportScheme::ERHZixuan == eTmpType)
		{
			CGridCellImage *pImgCell = (CGridCellImage *)m_GridCtrl.GetCell(0, i);
			pImgCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		}
	}
}

void CIoViewReport::ReCalcCustomGridSizeColFromXml()
{
	if (!m_bShowCustomGrid)
	{
		return;
	}

	int32 i,iSize,iTab=0;
	iSize = m_aTabInfomations.GetSize();
	if (iSize <= 0)
	{
		return;
	}



	for (i=0; i<iSize; i++)
	{
		if (m_aTabInfomations[i].m_Block.m_eType == T_BlockDesc::EBTUser)
		{
			iTab = i;
			break;
		}
	}

	T_BlockDesc BlockDesc = m_aTabInfomations.GetAt(iTab).m_Block;

	// ���	
	m_ReportHeadInfoList.RemoveAll();

	// �õ���ͷ��Ϣ
	if ( !SetCustomHeadInfomationList() )
	{
		return;
	}


	//�趨��ͷ
	m_CustomGridCtrl.SetFixedColumnCount(m_iFixCol);
	iSize = m_ReportHeadInfoList.GetSize();

	//
	bool32 bFoundForceField = false;

	CStringArray HeaderNames;
	CArray<int32,int32> HeaderFields;	
	CArray<int32,int32> HeaderWidths;

	// ת��
	for ( i = 0; i < iSize; i++ )
	{
		CString StrText = m_ReportHeadInfoList[i].m_StrHeadNameCn;
		ASSERT(StrText.GetLength() > 0 && m_ReportHeadInfoList[i].m_iHeadWidth > 0);

		HeaderNames.Add(StrText);
		HeaderWidths.Add(m_ReportHeadInfoList[i].m_iHeadWidth);

		bool32 bTrans;
		CReportScheme::E_ReportHeader eHeadType = CReportScheme::Instance()->GetReportHeaderEType(StrText);
		E_MerchReportField eField = ReportHeader2MerchReportField(eHeadType, bTrans);

		if ( bTrans )
		{
			HeaderFields.Add((int32)eField);
			if ( BlockDesc.m_iFieldAdd == eField )
			{
				bFoundForceField = true;
			}
		}
		else
		{
			HeaderFields.Add(-1);
		}

		// ����ѡ�ɵ���������ǿ����ʾ���� xl 1220
		// ���г�����ָ�������г���Ʒ�Ķ�����һ�Ž�����ʾ
		bool32 bBuySellPriceExchange = false;
		{
			if ( T_BlockDesc::EBTPhysical == BlockDesc.m_eType )
			{
				bBuySellPriceExchange = IsBuySellPriceExchangeMarket(BlockDesc.m_iMarketId);
			}
			else
			{
				for ( int32 i=0; i < BlockDesc.m_aMerchs.GetSize() ; ++i )
				{
					if ( IsBuySellPriceExchangeMerch(BlockDesc.m_aMerchs[i]) )
					{
						bBuySellPriceExchange = true;
						break;
					}
				}
			}
		}

		if ( bBuySellPriceExchange )
		{
			if ( CReportScheme::ERHBuyPrice == eHeadType )
			{
				HeaderNames[ HeaderNames.GetUpperBound() ] = CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHSellPrice);
			}
			else if ( CReportScheme::ERHSellPrice == eHeadType )
			{
				HeaderNames[ HeaderNames.GetUpperBound() ] = CReportScheme::Instance()->GetReportHeaderCnName(CReportScheme::ERHBuyPrice);
			}
		}
	}

	if ( -1 != BlockDesc.m_iFieldAdd && !bFoundForceField )
	{
		bool32 bTrans = false;
		CReportScheme::E_ReportHeader eType = MerchReportField2ReportHeader((E_MerchReportField)BlockDesc.m_iFieldAdd, bTrans);

		if ( bTrans )
		{
			// 
			BlockDesc.m_iTypeAdd = eType;
			UpdateLocalBlock(i, BlockDesc);

			CString StrText = CReportScheme::GetReportHeaderCnName(eType);
			HeaderNames.Add(StrText);
			HeaderFields.Add(BlockDesc.m_iFieldAdd);
			HeaderWidths.Add(CReportScheme::GetReportColDefaultWidth(eType));
		}
	} 

	// ���ñ��̶���
	m_CustomGridCtrl.SetFixedRowCount(1);
	iSize = HeaderNames.GetSize();
	m_CustomGridCtrl.SetColumnCount(iSize);

	// �����п�	
	for (i = 0; i < iSize; i++)
	{
		CGridCellSys * pCell = (CGridCellSys *)m_CustomGridCtrl.GetCell(0, i);
		DWORD dwFmt = DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX;
		pCell->SetFormat(dwFmt);
		CString StrText = HeaderNames.GetAt(i);

		// ���۱�����óɽ�������ɫ
		pCell->SetDefaultTextColor(ESCVolume);

		pCell->SetText(StrText);
		pCell->SetData(HeaderFields.GetAt(i));
		m_CustomGridCtrl.SetColumnWidth(i, HeaderWidths.GetAt(i));
		CReportScheme::E_ReportHeader eType = CReportScheme::Instance()->GetReportHeaderEType(StrText);
		if ( CReportScheme::ERHMerchName == eType
			|| CReportScheme::ERHMerchCode == eType
			|| CReportScheme::ERHRowNo == eType )
		{
			pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		}

		if (CReportScheme::ERHShortLineSelect == eType
			|| CReportScheme::ERHMidLineSelect == eType
			|| CReportScheme::ERHZixuan == eType)
		{
			CGridCellImage *pImgCell = (CGridCellImage *)m_CustomGridCtrl.GetCell(0, i);
			pImgCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		}
	}
	/*	SetHeadInfomationList();*/

	m_GridCtrl.ExpandColumnsToFit(FALSE);
}
LRESULT CIoViewReport::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: �ڴ����ר�ô����/����û���
	return __super::WindowProc(message, wParam, lParam);
}

BOOL CIoViewReport::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: �ڴ����ר�ô����/����û���
	LPNMHDR phdr = (LPNMHDR)lParam;
	NotifyMsg(TCN_SELCHANGE, 0x9999, OnSelChange)
		NotifyMsg(GVN_SELCHANGED, ID_REPORT_GRID,OnGridSelRowChanged) 
		NotifyMsg(NM_RCLICK, ID_REPORT_GRID,OnGridRButtonDown)
		NotifyMsg(NM_CLICK, ID_REPORT_GRID,OnGridLButtonDown)
		NotifyMsg(NM_DBLCLK, ID_REPORT_GRID, OnGridDblClick)
		NotifyMsg(GVN_COLWIDTHCHANGED, ID_REPORT_GRID, OnGridColWidthChanged)
		NotifyMsg(GVN_KEYDOWNEND, ID_REPORT_GRID, OnGridKeyDownEnd)
		NotifyMsg(GVN_KEYUPEND, ID_REPORT_GRID, OnGridKeyUpEnd)
		return __super::OnNotify(wParam, lParam, pResult);
}

void CIoViewReport::SetPosForScroll(int32 Keyboard)
{
	T_TabInfo stTabInfo = m_aTabInfomations.GetAt(m_iCurTab);
	// ����ɼ��У���������EnsureVisible�����ˮƽ����
	CCellRange VisibleCells = m_GridCtrl.GetVisibleNonFixedCellRange(NULL, FALSE, FALSE);
	//int32 iVisibleCol = VisibleCells.GetMinCol();

	int32 iDataNumsPerPage		= m_iMaxGridRowCount;				// ��ǰ�������ʾ������
	int32 iVisibleNowDataBegin	= stTabInfo.m_iDataIndexBegin;		// ���Ǳ�֤��һ����ʾ����begin����
	int32 iVisibleNowDataEnd	= stTabInfo.m_iDataIndexBegin + VisibleCells.GetRowSpan()-1; // �����ʾ��һ���ɼ�����(��)

	int32 iIndexBeginNow;
	int32 iIndexEndNow;

	int32 iSizeMerchs;	
	if ( T_BlockDesc::EBTPhysical == stTabInfo.m_Block.m_eType )
	{
		iSizeMerchs = stTabInfo.m_Block.m_pMarket->m_MerchsPtr.GetSize();
	}
	else
	{
		iSizeMerchs = stTabInfo.m_Block.m_aMerchs.GetSize();
	}

	int nMinPos, nMaxPos;
	m_XSBVertical.GetScrolRange(nMinPos, nMaxPos);

	int nPos = 0;
	if(VisibleCells.GetRowSpan() < iSizeMerchs)
	{
		if(VK_UP == Keyboard)
		{
			CCellID CellFocus = m_GridCtrl.GetFocusCell();
			nPos = abs(nMaxPos - nMinPos) * (iVisibleNowDataBegin - 1 + CellFocus.row) / (iSizeMerchs);
		}
		else if(VK_DOWN == Keyboard)
		{
			CCellID CellFocus = m_GridCtrl.GetFocusCell();
			nPos = abs(nMaxPos - nMinPos) * (iVisibleNowDataBegin + 1 + CellFocus.row) / (iSizeMerchs);
		}
		else if(VK_NEXT == Keyboard)
		{
			CCellID CellFocus = m_GridCtrl.GetFocusCell();
			iIndexBeginNow = iVisibleNowDataEnd + 1; // �Կɼ��еĺ��1����Ϊ��һ�У��ж�����ʾ����
			iIndexEndNow   = iIndexBeginNow + iDataNumsPerPage - 1;			
			//
			if ( iIndexEndNow >= iSizeMerchs )
			{
				iIndexEndNow	= iSizeMerchs - 1;		// ����������
				nPos = abs(nMaxPos - nMinPos);
			}
			else
			{
				nPos = abs(nMaxPos - nMinPos) * (iIndexBeginNow + CellFocus.row) / (iSizeMerchs);
			}
		}
		else if(VK_PRIOR == Keyboard)
		{
			if ( iVisibleNowDataBegin > 0 )
			{
				// �Ϸ�
				iIndexEndNow	= iVisibleNowDataBegin-1;
				iIndexBeginNow	= iIndexEndNow - iDataNumsPerPage + 1;		// ������ݿ�������Ч��
				ASSERT( iIndexEndNow <= iSizeMerchs-1 );
				if(iIndexBeginNow <= 0)
				{
					nPos = 0;
				}
				else
				{
					CCellID CellFocus = m_GridCtrl.GetFocusCell();
					nPos = abs(nMaxPos - nMinPos) * (iIndexBeginNow + CellFocus.row) / (iSizeMerchs);
				}
			}
		}
	}

	m_XSBVertical.SetScrollPos(nPos);
}

void CIoViewReport::JudgeScrollVerVisbile(bool bOnSize)
{
	if(m_bIsShowGridVertScorll)
	{
		if (m_aTabInfomations.GetSize() <= 0)
		{
			return;
		}

		T_TabInfo stTabInfo = m_aTabInfomations.GetAt(m_iCurTab);

		int32 iSizeMerchs;	
		if ( T_BlockDesc::EBTPhysical == stTabInfo.m_Block.m_eType )
		{
			iSizeMerchs = stTabInfo.m_Block.m_pMarket->m_MerchsPtr.GetSize();
		}
		else
		{
			iSizeMerchs = stTabInfo.m_Block.m_aMerchs.GetSize();
		}

		if((m_iMaxGridRowCount - 1 < iSizeMerchs))	// m_iMaxGridRowCount�ǰ����˱�ͷһ�е�
		{
			m_XSBVertical.SetScrollPos(0);
			m_XSBVertical.Invalidate();

			int32 iVisibleNowDataBegin = stTabInfo.m_iDataIndexBegin;	// ���Ǳ�֤��һ����ʾ����begin����
			int nMinPos, nMaxPos;
			m_XSBVertical.GetScrolRange(nMinPos, nMaxPos);
			int nPos = 0;
			if (iSizeMerchs - 1 > 0)
			{
				nPos = abs(nMaxPos - nMinPos) * iVisibleNowDataBegin / (iSizeMerchs - 1);
			}
			m_XSBVertical.SetScrollPos(nPos);

			m_XSBVertical.ShowWindow(SW_SHOW);
			m_RectGrid.right = m_RectVert.left;
			m_GridCtrl.MoveWindow(m_RectGrid);
		}
		else	// ����һҳ������ʾ�����е����
		{
			m_XSBVertical.ShowWindow(SW_HIDE);
			if(bOnSize)
			{
				ShowGridIndex(0);	// ��ֱ����������ʾ��ʱ�򣬾Ͱѱ���һ����Ϊ����0
			}
			m_RectGrid.right = m_rectClient.right;
			m_GridCtrl.MoveWindow(m_RectGrid);
		} 
	}
}

bool CIoViewReport::ShowGridIndex(int32 index)
{
	CCellID CellFocusBK = m_GridCtrl.GetFocusCell();
	T_TabInfo stTabInfo = m_aTabInfomations.GetAt(m_iCurTab);
	// �������������Ϊ m_GridCtrl.EnsureVisible ���䶯������һ��
	int32 iHScrolPosBk = m_GridCtrl.GetScrollPos32(SB_HORZ);
	// ����ɼ��У���������EnsureVisible�����ˮƽ����
	CCellRange VisibleCells = m_GridCtrl.GetVisibleNonFixedCellRange(NULL, FALSE, FALSE);
	int32 iVisibleCol = VisibleCells.GetMinCol();
	int32 iIndexBeginNow;
	int32 iIndexEndNow;

	int32 iSizeMerchs;	
	if ( T_BlockDesc::EBTPhysical == stTabInfo.m_Block.m_eType )
	{
		iSizeMerchs = stTabInfo.m_Block.m_pMarket->m_MerchsPtr.GetSize();
	}
	else
	{
		iSizeMerchs = stTabInfo.m_Block.m_aMerchs.GetSize();
	}

	iIndexBeginNow = index;
	if(iIndexBeginNow >= iSizeMerchs)
	{
		iIndexBeginNow = iSizeMerchs - 1;
	}

	iIndexEndNow = iIndexBeginNow + m_iMaxGridRowCount - 1;
	if ( iIndexEndNow >= iSizeMerchs )
	{
		iIndexEndNow = iSizeMerchs - 1;
	}

	m_GridCtrl.EnsureVisible(m_GridCtrl.GetFixedRowCount(), iVisibleCol);

	if (iIndexBeginNow == stTabInfo.m_iDataIndexBegin)	// δ�ı䣬�����������Ĳ���
	{
		return false;
	}

	UpdateLocalDataIndex(m_iCurTab, iIndexBeginNow, iIndexEndNow);
	//
	UpdateLocalXScrollPos(m_iCurTab, iHScrolPosBk);
	m_GridCtrl.SetScrollPos32(SB_HORZ, iHScrolPosBk);
	// �������ʵ��������
	m_GridCtrl.SetRowCount((iIndexEndNow - iIndexBeginNow + 1) + m_GridCtrl.GetFixedRowCount());
	ReSetGridCellDataByIndex();

	if ( m_bRequestViewSort )
	{
		// ��������, ��������Ժ������ǰ�ɼ���ʵʱ����
		CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
		CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
		bool32 bCanReqSort = NULL==pApp?false:!pApp->m_bOffLine;
		bCanReqSort = bCanReqSort && (NULL==pMainFrame? false:pMainFrame->GetUserConnectCmdFlag()&EUCCF_ConnectDataServer);
		RequestViewDataSortAsync();
		if ( !bCanReqSort )
		{
			// ������ԶҲ�ز�������ʾ��
			UpdateTableContent(TRUE, NULL, false);
			RequestViewDataCurrentVisibleRowAsync();
		}
	}
	else
	{	// ����ǰ�ɼ�������
		UpdateTableContent(TRUE, NULL, false);
		RequestViewDataCurrentVisibleRowAsync();
	}

	// �������ʱ��Ĭ�Ͻ�����
	CCellRange cellRange = m_GridCtrl.GetSelectedCellRange();
	int32 iCurSel = cellRange.GetMinRow();
	CGridCellSys * pCellToDell = NULL;

	if ( iCurSel > 0 || iCurSel <= m_GridCtrl.GetRowCount() - 1  )
	{   
		pCellToDell = (CGridCellSys *)m_GridCtrl.GetCell(iCurSel,0);
	}

	if ( NULL == pCellToDell )
	{
		m_GridCtrl.SetFocusCell(CellFocusBK);
		m_GridCtrl.SetSelectedSingleRow(1);
	}

	return true;
}

void CIoViewReport::ResetSort()
{
	m_bRequestViewSort = false;
	m_iSortColumn = -1;

	// ɾ����ǰ����
	m_GridCtrl.DeleteNonFixedRows();

	// �������	- tabchange�лᴦ����user�й�
	TabChange();

	m_GridCtrl.Refresh();
	m_GridCtrl.Invalidate();
}

void CIoViewReport::ChangeGridSize(bool32 bHasData)
{
	if (IsKindOfReportSelect())
	{
		if (bHasData)
		{
			m_bShowTextTip = false;
			m_GridCtrl.MoveWindow(m_RectGrid);
		}
		else
		{
			CRect rect(m_RectGrid);
			rect.top = m_RectGrid.top;
			rect.bottom = rect.top + (m_RectGrid.bottom - m_RectGrid.top)/3;
			m_bShowTextTip = true;
			m_GridCtrl.MoveWindow(rect);
		}
	}

	m_GridCtrl.Refresh();
}

void	CIoViewReport::ResetGridFont()
{
	//	�������е�Ԫ�������
	if (NULL == m_GridCtrl.GetSafeHwnd() || NULL == m_CustomGridCtrl.GetSafeHwnd())
	{
		return;
	}
	CGridCtrl* pGrid = NULL;
	if (m_bShowCustomGrid)
	{
		pGrid = &m_CustomGridCtrl;
	}
	else
	{
		pGrid = &m_GridCtrl;
	}
	
	
	if(NULL == pGrid)
	{
		return;
	}

	if(0 == pGrid->GetRowCount() - pGrid->GetFixedRowCount())
	{
		return ;
	}

	if (m_ReportHeadInfoList.IsEmpty())
	{
		return;
	}
	
	for (int i =pGrid->GetFixedRowCount(); i < pGrid->GetRowCount(); ++i)
	{
		for(int j =0; j < pGrid->GetColumnCount(); ++j)
		{
			//	�������������õ�Ԫ������
			CGridCellBase *pCell = m_GridCtrl.GetCell(i, j);
			if (NULL != pCell)
			{
				CReportScheme::E_ReportHeader ERHType  = CReportScheme::Instance()->GetReportHeaderEType(m_ReportHeadInfoList[j].m_StrHeadNameCn);
				switch (ERHType)
				{					
				case CReportScheme::E_ReportHeader::ERHMerchName:
				case CReportScheme::E_ReportHeader::ERHIndustry:
					pCell->SetFont(GetIoViewFont(ESFNormal));	
					break;
				default:
					pCell->SetFont(GetIoViewFont(ESFText));						
					break;
				}
			}			
		}
	}
}
void	CIoViewReport::ResetVisibleGridFont()
{
	//	�������е�Ԫ�������
	if (NULL == m_GridCtrl.GetSafeHwnd() || NULL == m_CustomGridCtrl.GetSafeHwnd())
	{
		return;
	}
	CGridCtrl* pGrid = NULL;
	if (m_bShowCustomGrid)
	{
		pGrid = &m_CustomGridCtrl;
	}
	else
	{
		pGrid = &m_GridCtrl;
	}


	if(NULL == pGrid)
	{
		return;
	}

	if(0 == pGrid->GetRowCount() - pGrid->GetFixedRowCount())
	{
		return ;
	}
	CCellRange rang = pGrid->GetVisibleNonFixedCellRange();
	for (int i = rang.GetMinRow(); i < rang.GetMaxRow() + 2; ++i)
	{
		if (i < pGrid->GetFixedRowCount())
		{
			continue;
		}

		for(int j =0; j < pGrid->GetColumnCount(); ++j)
		{
			//	�������������õ�Ԫ������
			CGridCellBase *pCell = m_GridCtrl.GetCell(i, j);
			if (NULL != pCell)
			{
				CReportScheme::E_ReportHeader ERHType  = CReportScheme::Instance()->GetReportHeaderEType(m_ReportHeadInfoList[j].m_StrHeadNameCn);
				switch (ERHType)
				{
				case CReportScheme::E_ReportHeader::ERHMerchName:
				case CReportScheme::E_ReportHeader::ERHIndustry:
					pCell->SetFont(GetIoViewFont(ESFNormal));	
					break;
				default:
					pCell->SetFont(GetIoViewFont(ESFText));						
					break;
				}
			}			
		}
	}
}



void CIoViewReport::UseSpecialRecordFont()
{
	LOGFONT lg;
	memset(&lg, 0, sizeof(lg));
	for (int i =0; i < sizeof(s_arrUserReportFont)/sizeof(s_arrUserReportFont[0]); ++i)
	{
		lg.lfHeight			= s_arrUserReportFont[i].iFontHeight;
		lg.lfWeight			= s_arrUserReportFont[i].iFontWeight;
		lg.lfCharSet		= s_arrUserReportFont[i].iFontCharset;
		lg.lfOutPrecision	= s_arrUserReportFont[i].iFontOutPrecision;
		_tcscpy_s(lg.lfFaceName, _ARRAYSIZE(lg.lfFaceName), s_arrUserReportFont[i].tFontFamily.GetBuffer());
		SetIoViewFontObject(s_arrUserReportFont[i].eOwnerType, lg);
	}
}

void CIoViewReport::SetReportHeadType(const E_ReportHeadType &eType)
{
	m_eReportHeadType = eType;
}