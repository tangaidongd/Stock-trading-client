#include "StdAfx.h"
#include "DlgTradeBidContainer.h"
#include "XmlShare.h"
#include "coding.h"
#include "PathFactory.h"
#include "FontFactory.h"
#include "DlgTip.h"
#include "DlgTradeClose.h"
#include <windowsx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
#define		IDC_DEF_BUTTON_OPEN			(WM_USER+100)
#define		IDC_DEF_BUTTON_CLOSE		(WM_USER+101)
#define		IDC_DEF_BUTTON_CANCEL		(WM_USER+102)
#define		IDC_DEF_BUTTON_OPENLEFT		(WM_USER+103)
#define		IDC_DEF_BUTTON_HOLD			(WM_USER+104)
#define		IDC_DEF_BUTTON_DETAIL		(WM_USER+105)
#define		IDC_DEF_BUTTON_TRANSFER		(WM_USER+106)
#define		IDC_DEF_BUTTON_HELP			(WM_USER+107)
#define		IDC_DEF_BUTTON_REFRESH		(WM_USER+108)
#define		IDC_DEF_BUTTON_MAXMIN		(WM_USER+109)
#define		IDC_DEF_TREE_MENU			(WM_USER+111)
#define		IDC_DEF_BUTTON_CLOSEWND		(WM_USER+112)
#define		IDC_DEF_BUTTON_SHIFT		(WM_USER+113)

#define		IDM_DEF_SERVER				(WM_USER+1000)

#define		COLOR_TREE_BK				RGB(236,233,216)		// 属性控件背景颜色
#define		COLOR_BK					RGB(240,241,235)		// 背景颜色
#define		COLOR_BUTTONT_BK			RGB(0xFC, 0xFD, 0xF5)	// 刷新、收缩、关闭三个按钮的背景色

	static	const	int		KLenButtonWidth			= 80;		// 上方按钮宽度
	static	const	int		KLenButtonHeight		= 16;		// 上方按钮高度	
	static	const	int		KLenButtonSpace			= 15;		// 上方按钮间隔
	static	const	int		KLenLeftButtonWidth		= 100;		// 左菜单按钮宽度
	static	const	int		KLenLeftButtonHeight	= 30;		// 左菜单按钮高度	
	static	const	int		KLenLeftButtonSpace		= 0;		// 左菜单按钮间隔	

	static	const	int		KLenTitleTop	=	35;
	static	const	int		KBlockSpace	=	18;	// 各个板块之间的空隙
	static	const	int		KNavigationBarWidth	= 180;	// 左侧导航栏的宽度
	static	const	int		KBottomSpace	= 5;
	static  const   int     KLenTradeDlgWidth  = 340;
	
	static	const	CBrush	KBrushTitleTop = RGB(255,0,0);
}

BEGIN_MESSAGE_MAP(CDlgTradeBidContainer, CDialog)
//{{AFX_MSG_MAP(CDlgTradeBidContainer)
ON_WM_ERASEBKGND()
ON_WM_SIZE()
ON_WM_PAINT()
ON_WM_CTLCOLOR()

ON_COMMAND(IDC_DEF_BUTTON_REFRESH, OnBtnRefresh)
ON_COMMAND(IDC_DEF_BUTTON_MAXMIN, OnBtnMaxMin)
ON_COMMAND_RANGE(IDM_DEF_SERVER, IDM_DEF_SERVER + 100, OnNetworkMenu)
ON_COMMAND(IDC_DEF_BUTTON_CLOSEWND, OnBtnCloseWnd)
ON_COMMAND(IDC_DEF_BUTTON_SHIFT, OnBtnShift)

ON_NOTIFY(TVN_SELCHANGED, IDC_DEF_TREE_MENU, OnTvnSelchangedWebTree)
ON_NOTIFY(NM_CLICK, IDC_DEF_TREE_MENU, OnClickTreeMenu)
ON_NOTIFY(TCN_SELCHANGE, 0x9999, OnTabSelChange)

ON_MESSAGE(WM_ENTRUSTTYPE_CLOSE,OnMsgEntrustTypeClose)
ON_MESSAGE(WM_CLOSETRADE,OnMsgCloseTrade)
ON_MESSAGE(WM_NETWORKSTATUS_CHANGE,OnMsgNetworkStatusChange)
ON_MESSAGE(WM_HOLDDETAIL_CLICK,OnHoldDetailClick)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CDlgTradeBidContainer::CDlgTradeBidContainer( CWnd* pParent, E_TradeLoginType eTpye) : CDialog(IDD, pParent)
{
	m_pTradeLoginInfo = NULL;
	m_bWndMaxMinStatus = true;
	m_bShowMenu = true;

	m_pBtnRefresh = NULL;
	m_pBtnMaxMin = NULL;
	m_pTreeMenu = NULL;
	m_pImagelist = NULL;

	m_iSelectedServer = 0;
	m_pBtnCloseWnd = NULL;

	m_TabGroup = Group_none;

	m_eTradeLoginType = eTpye;

	m_pBtnShift = NULL;
}

CDlgTradeBidContainer::~CDlgTradeBidContainer()
{
	if(m_pBtnShift != NULL)
	{
		delete m_pBtnShift;
		m_pBtnShift = NULL;
	}

	if (NULL!=m_pBtnRefresh)
	{
		delete m_pBtnRefresh;
		m_pBtnRefresh = NULL;
	}

	if (NULL!=m_pBtnMaxMin)
	{
		delete m_pBtnMaxMin;
		m_pBtnMaxMin = NULL;
	}

	if (NULL!=m_pBtnCloseWnd)
	{
		delete m_pBtnCloseWnd;
		m_pBtnCloseWnd = NULL;
	}

	if (NULL!=m_pTreeMenu)
	{
		delete m_pTreeMenu;
		m_pTreeMenu = NULL;
	}
	if (NULL!=m_pImagelist)
	{
		delete m_pImagelist;
		m_pImagelist = NULL;
	}
}

void CDlgTradeBidContainer::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTradeBidContainer)
	DDX_Control(pDX, IDC_STATIC_SERVERTYPE, m_staticDeal);
	DDX_Control(pDX, IDC_STATIC_OCCUPY, m_staticOccupy);
	DDX_Control(pDX, IDC_STATIC_OCCUPY_VALUE, m_staticOccupyValue);
	DDX_Control(pDX, IDC_STATIC_CURRENT_INTERSET, m_staticCurrentInsterest);
	DDX_Control(pDX, IDC_STATIC_CURRENT_INTERSET_VALUE, m_staticCurrentInsterestValue);
	DDX_Control(pDX, IDC_STATIC_FLOAT_GAIN_LOST, m_staticFloatGainLost);
	DDX_Control(pDX, IDC_STATIC_FLOAT_GAIN_LOST_VALUE, m_staticFloatGainLostValue);
	//}}AFX_DATA_MAP
}

BOOL CDlgTradeBidContainer::OnInitDialog()
{
	CDialog::OnInitDialog();

	// DlgQuery
	m_dlgQuery.m_pTradeLoginInfo = m_pTradeLoginInfo;
	m_dlgQuery.Create(this);
	m_dlgQuery.ShowWindow(SW_SHOW);
	m_dlgQuery.BringWindowToTop();
	
	// DlgClose
	m_DlgClose.m_pTradeQuery = &m_dlgQuery;	
	m_DlgClose.m_pTradeLoginInfo = m_pTradeLoginInfo;
	m_DlgClose.Create(this);
 	m_DlgClose.ShowWindow(SW_HIDE);
	
	// DlgOpen
	m_DlgOpen.m_pTradeQuery = &m_dlgQuery;		
	m_DlgOpen.m_pTradeLoginInfo = m_pTradeLoginInfo;
	m_DlgOpen.Create(this);
 	m_DlgOpen.ShowWindow(SW_HIDE);
	m_DlgOpen.BringWindowToTop();

	// DlgQuickOrder
	m_DlgQuickOrder.m_pTradeQuery = &m_dlgQuery;		
	m_DlgQuickOrder.m_pTradeLoginInfo = m_pTradeLoginInfo;
	m_DlgQuickOrder.Create(this);
 	m_DlgQuickOrder.ShowWindow(SW_HIDE);

// 	m_DlgChangePwd.m_pTradeLoginInfo = m_pTradeLoginInfo;
// 	m_DlgChangePwd.Create(this);
// 	m_DlgChangePwd.ShowWindow(SW_HIDE);

	if (m_GuiTab.Create(WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,0x9999))
	{
		m_GuiTab.ShowWindow(SW_HIDE);
		m_GuiTab.SetALingTabs(CGuiTabWnd::ALN_TOP);
		m_GuiTab.SetTabTextColor(RGB(0x4d, 0x4d, 0x4d),RGB(0x22, 0x7c, 0xed));
		m_GuiTab.SetBkGround(false,RGB(0xFC, 0xFD, 0xF5),0,0);
		m_GuiTab.SetFontByName(gFontFactory.GetExistFontName(L"微软雅黑"), GB2312_CHARSET);	//...
		m_GuiTab.SetFontHeight(-16);
	}

	CreateButton();	// 生成按钮
	RecalcLayout();	// 画各控件区域位置

	if (ETT_TRADE_FIRM == m_eTradeLoginType)
	{
		m_staticDeal.SetWindowText(L"实盘交易");
		m_staticDeal.SetBKColor(RGB(0xFA, 0xF6, 0xED));
	}
	else
	{
		m_staticDeal.SetWindowText(L"模拟交易");
		m_staticDeal.SetTextColor(RGB(34, 124, 237));
		m_staticDeal.SetBKColor(RGB(236, 239, 243));
	}
	m_staticDeal.AdjustStaticSize();

	m_staticOccupy.SetBKColor( RGB(0xFC, 0xFD, 0xF5) );
	m_staticOccupyValue.SetBKColor( RGB(0xFC, 0xFD, 0xF5) );

	m_staticCurrentInsterest.SetBKColor( RGB(0xFC, 0xFD, 0xF5) );
	m_staticCurrentInsterestValue.SetBKColor( RGB(0xFC, 0xFD, 0xF5) );

	m_staticFloatGainLost.SetBKColor( RGB(0xFC, 0xFD, 0xF5) );
	m_staticFloatGainLostValue.SetBKColor( RGB(0xFC, 0xFD, 0xF5) );
	m_staticFloatGainLostValue.SetTextColor( RGB(0xC9, 0x13, 0x1F) );

	return TRUE;
}

BOOL CDlgTradeBidContainer::OnEraseBkgnd( CDC* pDC )
{
	return true;
}

void CDlgTradeBidContainer::OnSize( UINT nType, int cx, int cy )
{
	CDialog::OnSize(nType, cx, cy);

	RecalcLayout();
}

void CDlgTradeBidContainer::RecalcLayout()
{
	CRect rc(0,0,0,0);
	GetClientRect(rc);
	
	if(m_staticDeal.GetSafeHwnd() == NULL)
	{
		return ;
	}

	// 类型（点正交易4个字）
	CRect rcSer(0,0,0,0);
	rcSer = m_staticDeal.AdjustStaticSize();
	int iSerWidth = rcSer.Width();
	int iSerHeight = rcSer.Height();
	rcSer.top		= rc.top + (KLenTitleTop - iSerHeight) / 2;
	rcSer.bottom	= rcSer.top + iSerHeight;
	rcSer.left		= 20;
	rcSer.right		= rcSer.left + iSerWidth;
	m_staticDeal.MoveWindow(rcSer);
	// 
	/* 上方按钮	*/	
	//const int iMaxMinHeightWidth = 16;
	
	CRect rcBtnCloseWnd(rc);	// 关闭按钮
	rcBtnCloseWnd.right = rc.right - KBlockSpace;	//
	rcBtnCloseWnd.left = rcBtnCloseWnd.right - KLenButtonHeight;
	rcBtnCloseWnd.top = rc.top + (KLenTitleTop - KLenButtonHeight) / 2;
	rcBtnCloseWnd.bottom = rcBtnCloseWnd.top + KLenButtonHeight;
	if (m_pBtnCloseWnd->m_hWnd)
	{
		m_pBtnCloseWnd->MoveWindow(rcBtnCloseWnd);
	}
	CRect rcBtnMaxMin(rcBtnCloseWnd);
	rcBtnMaxMin.right	= rcBtnCloseWnd.left - KLenButtonSpace;
	rcBtnMaxMin.left	= rcBtnMaxMin.right - KLenButtonHeight;
	if (m_pBtnMaxMin->m_hWnd)
	{
		m_pBtnMaxMin->MoveWindow(rcBtnMaxMin);
	}
	CRect rcBtnRefresh(rcBtnMaxMin);
	rcBtnRefresh.right = rcBtnMaxMin.left - KLenButtonSpace;
	rcBtnRefresh.left = rcBtnRefresh.right - KLenButtonHeight;
	if (m_pBtnRefresh->m_hWnd)
	{
		m_pBtnRefresh->MoveWindow(rcBtnRefresh);
	}
	// 切换按钮
	CRect rcBtnShift(rcBtnMaxMin);
	rcBtnShift.left = 3;
	rcBtnShift.right = rcBtnShift.left + rcBtnMaxMin.Width();
	if(m_pBtnShift->m_hWnd)
	{
		m_pBtnShift->MoveWindow(rcBtnShift);
	}
	//tree
	CRect rcTreeMenu(rc);
	rcTreeMenu.left = rc.left;
	rcTreeMenu.right = KNavigationBarWidth;	// 宽度
	rcTreeMenu.top = rc.top + KLenTitleTop + 1;
	rcTreeMenu.bottom = rc.bottom - 1;
	if (m_pTreeMenu->m_hWnd)
	{
		m_pTreeMenu->MoveWindow(rcTreeMenu);
	}
	
	/*CRect rcOccupy = */RecalcThreeStatic(rcBtnRefresh);
	int iStaticPos = 0;

	CRect rcTabGroup(rc);
	rcTabGroup.top = rc.top + 5;
	rcTabGroup.bottom = rc.top + KLenTitleTop + 1;

	//交易区域 
	CRect rcTaderDlg(rc);
	rcTaderDlg.left		= rcTreeMenu.right + KBlockSpace;
	rcTaderDlg.top		= KLenTitleTop;
	rcTaderDlg.right	= rcTaderDlg.left;
	rcTaderDlg.bottom	= rc.bottom - KBottomSpace;	// 底部到边缘的距离是KBottomSpace
	if (m_DlgOpen.IsWindowVisible())
	{
		rcTaderDlg.right = rcTaderDlg.left + KLenTradeDlgWidth;
		m_DlgOpen.MoveWindow(rcTaderDlg);
	}
	if (m_DlgClose.IsWindowVisible())
	{
		rcTaderDlg.right = rcTaderDlg.left + KLenTradeDlgWidth;
		m_DlgClose.MoveWindow(rcTaderDlg);
	}

	// 表格
	CRect rcQuery(rcTaderDlg);
	if(rcTaderDlg.left  != rcTaderDlg.right)
	{
		rcQuery.left	= rcTaderDlg.right + KBlockSpace;
		rcTabGroup.left =  rcTaderDlg.left;	
		rcTabGroup.right = rcTaderDlg.right;
	}
	else
	{
		rcQuery.left	= rcTaderDlg.right ;
		rcTabGroup.left		= rcQuery.left;
		rcTabGroup.right	= rcTabGroup.left + 480; 
		iStaticPos = rcTabGroup.Width() + KBlockSpace;
	}
	
	rcQuery.right	= rc.right - KBlockSpace;
	if (m_dlgQuery.m_hWnd && m_dlgQuery.IsWindowVisible())
	{
		m_dlgQuery.MoveWindow(rcQuery);
	}

	bool32 bGuiTabVisible = false;
	if (m_GuiTab.IsWindowVisible())
	{
		m_GuiTab.MoveWindow(rcTabGroup);
		bGuiTabVisible = true;
	}
	
	/* 在此处判断一下静态文本框的左边位置是否超过列表的左边位置，如果是则隐藏不显示
		m_staticOccupy; m_staticOccupyValue;
		m_staticCurrentInsterest; m_staticCurrentInsterestValue;
		m_staticFloatGainLost; m_staticFloatGainLostValue;
	*/
	CRect rcTemp;
	m_dlgQuery.GetWindowRect(rcQuery);

	// 如果tab是隐藏的话，下面判断的时候要去掉它的宽度
	int32 iSubWidth = 0;
	if (!bGuiTabVisible)
	{
		iSubWidth = rcTabGroup.Width();
	}

	m_staticOccupy.GetWindowRect(rcTemp);
	if(rcTemp.left < (rcQuery.left + iStaticPos - iSubWidth))
	{
		m_staticOccupy.ShowWindow(SW_HIDE);
		m_staticOccupyValue.ShowWindow(SW_HIDE);
	}
	else if(m_staticOccupy.IsWindowVisible() == false)
	{
		m_staticOccupy.ShowWindow(SW_SHOW);
		m_staticOccupyValue.ShowWindow(SW_SHOW);
	}

	m_staticCurrentInsterest.GetWindowRect(rcTemp);
	if(rcTemp.left < (rcQuery.left + iStaticPos - iSubWidth))
	{
		
		m_staticCurrentInsterest.ShowWindow(SW_HIDE);
		m_staticCurrentInsterestValue.ShowWindow(SW_HIDE);
	}
	else if(m_staticCurrentInsterest.IsWindowVisible() == false)
	{	
		m_staticCurrentInsterest.ShowWindow(SW_SHOW);
		m_staticCurrentInsterestValue.ShowWindow(SW_SHOW);
	}

	m_staticFloatGainLost.GetWindowRect(rcTemp);
	if(rcTemp.left < (rcQuery.left + iStaticPos - iSubWidth))
	{
		m_staticFloatGainLost.ShowWindow(SW_HIDE);
		m_staticFloatGainLostValue.ShowWindow(SW_HIDE);
	}
	else if(m_staticFloatGainLost.IsWindowVisible() == false)
	{
		m_staticFloatGainLost.ShowWindow(SW_SHOW);
		m_staticFloatGainLostValue.ShowWindow(SW_SHOW);
	}
}

CRect CDlgTradeBidContainer::RecalcThreeStatic(const CRect& rect)
{
	int nWidth = 0;
	CRect rcTemp, rcTempValue;
	
	// 浮动盈亏
	m_staticFloatGainLostValue.AdjustStaticSize(&nWidth);
	rcTempValue.right = rect.left - KLenButtonSpace;
	rcTempValue.left = rcTempValue.right - nWidth;
	rcTempValue.top = rect.top;
	rcTempValue.bottom = rect.bottom;
	m_staticFloatGainLost.AdjustStaticSize(&nWidth);
	rcTemp.right = rcTempValue.left;
	rcTemp.left = rcTemp.right - nWidth;
	rcTemp.top = rect.top;
	rcTemp.bottom = rect.bottom;
	m_staticFloatGainLost.MoveWindow(rcTemp);
	m_staticFloatGainLostValue.MoveWindow(rcTempValue);
	// 当前权益
	m_staticCurrentInsterestValue.AdjustStaticSize(&nWidth);
	rcTempValue.right = rcTemp.left - KLenButtonSpace;
	rcTempValue.left = rcTempValue.right - nWidth;
	m_staticCurrentInsterest.AdjustStaticSize(&nWidth);
	rcTemp.right = rcTempValue.left;
	rcTemp.left = rcTemp.right - nWidth;
	m_staticCurrentInsterest.MoveWindow(rcTemp);
	m_staticCurrentInsterestValue.MoveWindow(rcTempValue);
	//可用保证金
	CRect rcOccupy;
	m_staticOccupyValue.AdjustStaticSize(&nWidth);	
	rcTempValue.right = rcTemp.left - KLenButtonSpace;
	rcTempValue.left = rcTempValue.right - nWidth;
	m_staticOccupy.AdjustStaticSize(&nWidth);
	rcTemp.right = rcTempValue.left;
	rcTemp.left = rcTemp.right - nWidth;
	rcOccupy.CopyRect(rcTemp);
	m_staticOccupy.MoveWindow(rcTemp);
	m_staticOccupyValue.MoveWindow(rcTempValue);
	
	return rcOccupy;
}

void CDlgTradeBidContainer::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		
		// Center icon in client rectangle
		//int cxIcon = GetSystemMetrics(SM_CXICON);
		//int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		//int x = (rect.Width() - cxIcon + 1) / 2;
		//int y = (rect.Height() - cyIcon + 1) / 2;
		
// 		// Draw the icon
// 		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CRect rect;
		GetClientRect(rect);

		CPaintDC dc(this);
		CRect rectLeft(rect);	// 导航栏的位置
		rectLeft.right = rect.left + KNavigationBarWidth;	// 宽度为180
		if (ETT_TRADE_FIRM == m_eTradeLoginType)
		{
			dc.FillSolidRect(rectLeft, RGB(0xFA, 0xF6, 0xED));
		}
		else
		{
			dc.FillSolidRect(rectLeft, RGB(236, 239, 243));
		}
		// 
		Graphics graphics(this->m_hWnd);
		Pen penHor(Color(0xFF, 0xD9, 0xD3, 0xC2));
		graphics.DrawLine(&penHor, rectLeft.left, KLenTitleTop, rectLeft.right, KLenTitleTop);	// 水平分割线
		// 交易区域
		CRect rectRight(rect);
		rectRight.left = rectLeft.right;
		dc.FillSolidRect(rectRight, RGB(0xFC, 0xFD, 0xF5));
		Pen penVer(Color(0xFF, 0xD9, 0xD9, 0xD9));
		graphics.DrawLine(&penVer, rectLeft.right, rectLeft.top, rectLeft.right, rectRight.bottom);	// 垂直分割线

		CDialog::OnPaint();
	}
}

void CDlgTradeBidContainer::CreateButton()
{
	LOGFONT lgFont = {0};

	::GetObject((HFONT)GetStockObject(DEFAULT_GUI_FONT),sizeof(lgFont),&lgFont);

// 	lgFont.lfWeight = FW_MEDIUM; //（这两个参数是粗体的）
// 	lgFont.lfHeight = 15;

	m_fontButton.CreateFontIndirect(&lgFont);

	// 刷新 
	m_pBtnRefresh = new CButtonSTMain();
	m_pBtnRefresh->Create(L"",WS_CHILD|WS_VISIBLE/*|BS_PUSHBUTTON|BS_OWNERDRAW*/,CRect(0,0,1,1),this,IDC_DEF_BUTTON_REFRESH);
	m_pBtnRefresh->SetIcon(IDI_ICON_REFRESH_COLOR, IDI_ICON_REFRESH_BLACK);
	m_pBtnRefresh->ShowWindow(SW_SHOW);
	m_pBtnRefresh->SetFont(&m_fontButton);
	m_pBtnRefresh->SetColor(CButtonSTMain::BTNST_COLOR_BK_IN, COLOR_BUTTONT_BK);
	m_pBtnRefresh->SetColor(CButtonSTMain::BTNST_COLOR_BK_OUT, COLOR_BUTTONT_BK);
	m_pBtnRefresh->SetColor(CButtonSTMain::BTNST_COLOR_BK_FOCUS, COLOR_BUTTONT_BK);
	m_pBtnRefresh->SetShowText(TRUE);
	m_pBtnRefresh->DrawBorder(FALSE);

	// 缩放 
	m_pBtnMaxMin = new CButtonSTMain();
	m_pBtnMaxMin->Create(L"",WS_CHILD|WS_VISIBLE/*|BS_PUSHBUTTON|BS_OWNERDRAW*/,CRect(0,0,1,1),this,IDC_DEF_BUTTON_MAXMIN);
	m_pBtnMaxMin->SetIcon(IDI_ICON_FOLD_COLOR, IDI_ICON_FOLD_BLACK);
	m_pBtnMaxMin->ShowWindow(SW_SHOW);
	m_pBtnMaxMin->SetFont(&m_fontButton);
	m_pBtnMaxMin->SetColor(CButtonSTMain::BTNST_COLOR_BK_IN, COLOR_BUTTONT_BK);
	m_pBtnMaxMin->SetColor(CButtonSTMain::BTNST_COLOR_BK_OUT, COLOR_BUTTONT_BK);
	m_pBtnMaxMin->SetColor(CButtonSTMain::BTNST_COLOR_BK_FOCUS, COLOR_BUTTONT_BK);
	m_pBtnMaxMin->SetShowText(TRUE);
	m_pBtnMaxMin->DrawBorder(FALSE);

	// 关闭窗口 
	m_pBtnCloseWnd = new CButtonSTMain();
	m_pBtnCloseWnd->Create(L"",WS_CHILD|WS_VISIBLE/*|BS_PUSHBUTTON|BS_OWNERDRAW*/,CRect(0,0,1,1),this,IDC_DEF_BUTTON_CLOSEWND);
	m_pBtnCloseWnd->SetIcon(IDI_ICON_QUIT_COLOR, IDI_ICON_QUIT_BLACK);
	m_pBtnCloseWnd->ShowWindow(SW_SHOW);
	m_pBtnCloseWnd->SetFont(&m_fontButton);
	m_pBtnCloseWnd->SetColor(CButtonSTMain::BTNST_COLOR_BK_IN, COLOR_BUTTONT_BK);
	m_pBtnCloseWnd->SetColor(CButtonSTMain::BTNST_COLOR_BK_OUT, COLOR_BUTTONT_BK);
	m_pBtnCloseWnd->SetColor(CButtonSTMain::BTNST_COLOR_BK_FOCUS, COLOR_BUTTONT_BK);
	m_pBtnCloseWnd->SetShowText(TRUE);
	m_pBtnCloseWnd->DrawBorder(FALSE);

	// 切换按钮
	m_pBtnShift = new CButtonSTMain;
	m_pBtnShift->Create(L"",WS_CHILD|WS_VISIBLE/*|BS_PUSHBUTTON|BS_OWNERDRAW*/,CRect(0,0,1,1),this,IDC_DEF_BUTTON_SHIFT);
	if(m_eTradeLoginType == ETT_TRADE_SIMULATE)
	{
		m_pBtnShift->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pBtnShift->ShowWindow(SW_HIDE);
	}
	m_pBtnShift->SetFont(&m_fontButton);
	m_pBtnShift->SetIcon(IDI_ICON_HOME_COLOR, IDI_ICON_HOME_BLACK);
	m_pBtnShift->SetColor(CButtonSTMain::BTNST_COLOR_BK_IN, COLOR_BUTTONT_BK);
	m_pBtnShift->SetColor(CButtonSTMain::BTNST_COLOR_BK_OUT, COLOR_BUTTONT_BK);
	m_pBtnShift->SetColor(CButtonSTMain::BTNST_COLOR_BK_FOCUS, COLOR_BUTTONT_BK);
	m_pBtnShift->SetShowText(TRUE);
	m_pBtnShift->DrawBorder(FALSE);

	InitTree();
//	hRoot = m_pTreeMenu->InsertItem(_T("修改密码"), IDR_PNG_MODIFYPW_BLACK, IDR_PNG_MODIFYPW_COLOR);
//	m_pTreeMenu->SetItemData(hRoot, nItem++);
}

void CDlgTradeBidContainer::InitTree()
{
	//导航树
	m_pTreeMenu = new CBeautifulTree();
	m_pTreeMenu->Create(WS_CHILD|TVS_FULLROWSELECT,CRect(0,0,0,0),this,IDC_DEF_TREE_MENU);
	m_pTreeMenu->ShowWindow(SW_SHOW);
	m_pTreeMenu->SetItemHeight(27);
	
	int nItem = 0;
	HTREEITEM hRoot, hRootChild;
	
	hRoot = m_pTreeMenu->InsertItem(_T("建仓"), IDR_PNG_OPEN_BLACK, IDR_PNG_OPEN_COLOR);
	m_pTreeMenu->SetItemData(hRoot, nItem++);
	
	hRoot = m_pTreeMenu->InsertItem(_T("平仓"), IDR_PNG_CLOSE_BLACK, IDR_PNG_CLOSE_COLOR);
	m_pTreeMenu->SetItemData(hRoot, nItem++);
	
	hRoot = m_pTreeMenu->InsertItem(_T("撤单"), IDR_PNG_REMOVE_BLACK, IDR_PNG_REMOVE_COLOR);
	m_pTreeMenu->SetItemData(hRoot, nItem++);
	
	hRoot = m_pTreeMenu->InsertItem(_T("查询"), IDR_PNG_SEARCH_BLACK, IDR_PNG_SEARCH_COLOR);
	m_pTreeMenu->SetItemData(hRoot, nItem++);
	
		hRootChild = m_pTreeMenu->InsertItem(_T("查持仓单"), 0, 0, hRoot);
		m_pTreeMenu->SetItemData(hRootChild, nItem++);

		hRootChild = m_pTreeMenu->InsertItem(_T("查持仓汇总"), 0, 0, hRoot);
		m_pTreeMenu->SetItemData(hRootChild, nItem++);
		
		hRootChild = m_pTreeMenu->InsertItem(_T("查指价单"), 0, 0, hRoot);
		m_pTreeMenu->SetItemData(hRootChild, nItem++);
		
		hRootChild = m_pTreeMenu->InsertItem(_T("查平仓单"), 0, 0, hRoot);
		m_pTreeMenu->SetItemData(hRootChild, nItem++);
	
	hRoot = m_pTreeMenu->InsertItem(_T("账户信息"), IDR_PNG_ACCOUNTINFO_BLACK, IDR_PNG_ACCOUNTINFO_COLOR);
	m_pTreeMenu->SetItemData(hRoot, nItem++);
	
	hRoot = m_pTreeMenu->InsertItem(_T("商品信息"), IDR_PNG_GOODS_BLACK, IDR_PNG_GOODS_COLOR);
	m_pTreeMenu->SetItemData(hRoot, nItem++);

	m_pTreeMenu->ExpandAllNode();
	m_pTreeMenu->SetScrollPos(SB_VERT, 0);	// 设置导航树的滚动条位置在最顶端

	m_pTreeMenu->SetTreeStyle(m_pTradeLoginInfo->eTradeLoginType);
}

//	建仓
void CDlgTradeBidContainer::OnBtnOpen()
{
	if (NULL==m_pTradeLoginInfo || m_TabGroup == Group_open)
	{
		return;
	}
	
	m_DlgClose.ShowWindow(SW_HIDE);
//	m_DlgChangePwd.ShowWindow(SW_HIDE);

	if ( !m_DlgOpen.IsWindowVisible())
	{
		m_TabGroup = Group_open;
		m_GuiTab.DeleteAll();
		m_GuiTab.Addtab(_T("市价建仓"),_T("市价建仓"),L"");
		m_GuiTab.Addtab(_T("指价建仓"),_T("指价建仓"),L"");
		m_GuiTab.SetCurtab(0);
		if(m_pTradeLoginInfo->eTradeLoginType == ETT_TRADE_SIMULATE)
		{
			m_GuiTab.SetTabBkColor(	RGB(236,239,243), RGB(236,239,243));
		}
		else
		{
			m_GuiTab.SetTabBkColor(	RGB(0xF2,0xF0,0xEB),RGB(0xF2,0xF0,0xEB));
		}
		m_GuiTab.SetTabFrameColor(RGB(0xE6, 0xE6, 0xE6));
		m_GuiTab.SetTabSpace(0);
		m_GuiTab.ShowWindow(SW_SHOW);
		m_dlgQuery.ShowWindow(SW_SHOW);
		m_DlgOpen.ResetInitControl(_T(""),true);
		m_DlgOpen.SetPrice();
		m_DlgOpen.SetRange();
		m_DlgOpen.ShowWindow(SW_SHOW);
	}
	
	m_dlgQuery.ShowQueryChange(EQT_QueryHoldDetail);

	return;
}

// 平仓(此平仓和右键菜单平仓有所不同，要注意)
void CDlgTradeBidContainer::OnBtnClose(E_TradeType eType)
{
	if (NULL==m_pTradeLoginInfo )
	{
		return;
	}

	if (!m_DlgClose.IsWindowVisible())
	{
	//	m_DlgChangePwd.ShowWindow(SW_HIDE);
		m_DlgOpen.ShowWindow(SW_HIDE);
		
		m_TabGroup = Group_close;
		m_GuiTab.DeleteAll();
		m_GuiTab.Addtab(_T("市价平仓"),_T("市价平仓"),L"");
		m_GuiTab.Addtab(_T("指价平仓"),_T("指价平仓"),L"");
		//m_GuiTab.SetTabBkColor(	RGB(0xF2,0xF0,0xEB),RGB(0xF2,0xF0,0xEB));
		if(m_pTradeLoginInfo->eTradeLoginType == ETT_TRADE_SIMULATE)
		{
			m_GuiTab.SetTabBkColor(	RGB(236,239,243), RGB(236,239,243));
		}
		else
		{
			m_GuiTab.SetTabBkColor(	RGB(0xF2,0xF0,0xEB),RGB(0xF2,0xF0,0xEB));
		}
		m_GuiTab.SetTabFrameColor(RGB(0xE6, 0xE6, 0xE6));
		m_GuiTab.SetTabSpace(0);
		m_GuiTab.ShowWindow(SW_SHOW);
//		m_DlgClose.ShowWindow(SW_SHOW);
		m_dlgQuery.ShowWindow(SW_SHOW);

		m_dlgQuery.ShowQueryChange(EQT_QueryHoldDetail);
	}

	if (eType == ECT_Market)
	{
		m_GuiTab.SetCurtab(0);
	}
	else
	{ 
		m_GuiTab.SetCurtab(1);
	}

	m_DlgClose.m_pHoldDetailQueryOut = (T_RespQueryHold*)m_dlgQuery.GetHoldDetailMainSelect();

	if (NULL == m_DlgClose.m_pHoldDetailQueryOut)
	{
		const QueryHoldDetailResultVector &aQuery = m_dlgQuery.m_pTradeBid->GetCacheHoldDetail();
		int iSize = 0;
		iSize = aQuery.size();
		if (0 < iSize)
		{
			m_DlgClose.m_pHoldDetailQueryOut = (T_RespQueryHold*)&aQuery[iSize-1];
		}

	}

	m_DlgClose.ResetInitControl(eType);
 	m_DlgClose.SetPrice();
	if (!m_DlgClose.IsWindowVisible())
	{
		m_DlgClose.ShowWindow(SW_SHOW);
	}
	return;
}

//查寻
void CDlgTradeBidContainer::OnBtnQuery(E_QueryType eType)
{
	int iSetTab = -1;
	bool bAddTab = false;
	if (eType == EQT_QueryHoldDetail)
	{
		iSetTab = 0;
		if (m_TabGroup != Group_query)
		{
			bAddTab = true;
			m_TabGroup = Group_query;
		}
	}
	else if (eType == EQT_QueryHoldSummary)
	{
		iSetTab = 1;
		if (m_TabGroup != Group_query)
		{
			bAddTab = true;
			m_TabGroup = Group_query;
		}
	}
	else if (eType == EQT_QueryLimitEntrust)
	{
		iSetTab = 2;
		if (m_TabGroup != Group_query)
		{
			bAddTab = true;
			m_TabGroup = Group_query;
		}
	}
	else if (eType == EQT_QueryDeal)
	{
		iSetTab = 3;
		if (m_TabGroup != Group_query)
		{
			bAddTab = true;
			m_TabGroup = Group_query;
		}
	}
	else
	{
		m_TabGroup = Group_none;
	}
	
	m_dlgQuery.ShowWindow(SW_SHOW);
	m_DlgClose.ShowWindow(SW_HIDE);
	m_DlgOpen.ShowWindow(SW_HIDE);
	if ( m_TabGroup == Group_query)
	{	
		if (bAddTab)
		{
			m_GuiTab.DeleteAll();
			m_GuiTab.SetTabSpace(10);
			m_GuiTab.Addtab(_T("查持仓单"),_T("查持仓单"),L"");
			m_GuiTab.Addtab(_T("查持仓汇总"),_T("查持仓汇总"),L"");
			m_GuiTab.Addtab(_T("查指价单"),_T("查指价单"),L"");
			m_GuiTab.Addtab(_T("查平仓单"),_T("查平仓单"),L"");
			
			if(m_pTradeLoginInfo->eTradeLoginType == ETT_TRADE_SIMULATE)
			{
				m_GuiTab.SetTabBkColor(	RGB(236,239,243), RGB(243,241,235));
			}
			else
			{
				m_GuiTab.SetTabBkColor(	RGB(217,215,210),RGB(243,241,235));
			}
			m_GuiTab.SetTabFrameColor(RGB(0xE6, 0xE6, 0xE6));
		}
		m_GuiTab.SetCurtab(iSetTab);
		m_GuiTab.ShowWindow(SW_SHOW);
	}
	else
	{
		m_GuiTab.ShowWindow(SW_HIDE);
	}

	m_dlgQuery.ShowQueryChange(eType);
}

void CDlgTradeBidContainer::OnBtnChangePwd()
{
	m_DlgClose.ShowWindow(SW_HIDE);
	m_DlgOpen.ShowWindow(SW_HIDE);
	m_GuiTab.ShowWindow(SW_HIDE);
	m_dlgQuery.ShowWindow(SW_HIDE);
//	m_DlgChangePwd.ShowWindow(SW_SHOW);
}

// 刷新
void CDlgTradeBidContainer::OnBtnRefresh()
{
	m_dlgQuery.RefreshList();
}

// 缩放
void CDlgTradeBidContainer::OnBtnMaxMin()
{
	if (m_bWndMaxMinStatus)	//当前是大,要缩小
	{
		m_bWndMaxMinStatus = false;
		ShowHideControl(SW_HIDE);
		m_pBtnMaxMin->SetIcon(IDI_ICON_EXPAND_COLOR, IDI_ICON_EXPAND_BLACK);
	}
	else
	{
		m_bWndMaxMinStatus = true;
		ShowHideControl(SW_SHOW);
		m_pBtnMaxMin->SetIcon(IDI_ICON_FOLD_COLOR, IDI_ICON_FOLD_BLACK);
	}

	m_dlgQuery.WndMaxMin();
}

void CDlgTradeBidContainer::OnBtnShift()
{
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	pMain->m_pNewWndTB->DoOpenCfm(L"模拟交易");
}

//设置默认
void CDlgTradeBidContainer::SetDefMaxMin()
{
	m_bWndMaxMinStatus = true;
	ShowHideControl(SW_SHOW);
	m_pBtnMaxMin->SetIcon(IDI_ICON_HIDE2,IDI_ICON_HIDE1);
}

// 放大缩小时显示隐藏控件
void CDlgTradeBidContainer::ShowHideControl(int nCmdShow)
{
	m_pBtnRefresh->ShowWindow(nCmdShow);

	if (m_TabGroup != Group_none)
	{
		m_GuiTab.ShowWindow(nCmdShow);
	}
}

HBRUSH CDlgTradeBidContainer::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	switch(nCtlColor)
	{
	case CTLCOLOR_STATIC:
		{
			if ((pWnd->GetDlgCtrlID() == IDC_STATIC_SERVERTYPE))
			{
				static HBRUSH hbrStatic = ::CreateSolidBrush( COLOR_BK );
				pDC->SetBkColor( COLOR_BK );
			//	pDC->SetTextColor( 0 );
				return hbrStatic;
			}
		}
		break;
	default:
		break;
	}
	
	return hbr;
}

void CDlgTradeBidContainer::OnTvnSelchangedWebTree(NMHDR* pNMHDR, LRESULT* pResult) 
{

}

void CDlgTradeBidContainer::OnClickTreeMenu(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (NULL==m_pTreeMenu)
	{
		return;
	}

	DWORD   dwpos = GetMessagePos();   
	TVHITTESTINFO ht = {0};  
	ht.pt.x = GET_X_LPARAM(dwpos);
	ht.pt.y = GET_Y_LPARAM(dwpos);
	::MapWindowPoints(HWND_DESKTOP,pNMHDR->hwndFrom,&ht.pt,1); //把屏幕坐标转换成控件坐标
	   
	if (0 != TreeView_HitTest(pNMHDR->hwndFrom,(LPTV_HITTESTINFO)(&ht)))//确定点击的是哪一项
	{
		m_pTreeMenu->Select(ht.hItem, TVGN_CARET);
		HTREEITEM hItem = m_pTreeMenu->GetSelectedItem();
		if (NULL==hItem)
		{
			return;
		}

		m_pTreeMenu->ToggleNode();

		int iIndex = m_pTreeMenu->GetItemData(hItem);	// 父节点索引
		if (0 == iIndex)//建仓
		{
			OnBtnOpen();
		} 
		else if (1==iIndex)// 平仓
		{
			OnBtnClose(m_DlgClose.m_eCloseType);
		}
		else if (2==iIndex)// 撤单
		{
			OnBtnQuery(EQT_QueryCancel);
		}
		// 	else if (3==iIndex)
		// 	{
		// 		OnBtnQuery(EQT_QueryHoldDetail);
		// 	}
		else if (4==iIndex)// 查持仓单
		{
			OnBtnQuery(EQT_QueryHoldDetail);
		}
		else if (5 == iIndex)// 查持仓汇总
		{
			OnBtnQuery(EQT_QueryHoldSummary);
		}
		else if (6==iIndex )// 查指价单
		{
			OnBtnQuery(EQT_QueryLimitEntrust);
		}
		else if (7==iIndex )// 查平仓单
		{
			OnBtnQuery(EQT_QueryDeal);
		}
		else if (8==iIndex)// 用户信息
		{
			OnBtnQuery(EQT_QueryUserInfo);	
		}
		else if (9==iIndex)// 商品信息
		{
			OnBtnQuery(EQT_QueryCommInfo);
		}
		//  else if(9 == iIndex)	// 修改密码
		// 	{
		//  	//OnBtnQuery(EQT_QueryModifyPW);
		// 		OnBtnChangePwd();
		// 	}
		RecalcLayout();
	}
}

void CDlgTradeBidContainer::ShowHideModule(int nCmdShow)
{
	m_dlgQuery.ShowWindow(nCmdShow);
}

LRESULT CDlgTradeBidContainer::OnMsgEntrustTypeClose( WPARAM w, LPARAM l )
{

	E_TradeType eType = (E_TradeType)l;
	m_DlgClose.m_pHoldDetailQueryOut = (T_RespQueryHold*)w;
	OnBtnClose(eType);
	RecalcLayout();
	m_pTreeMenu->SelectItem(1);
		
	return 1;	
}


LRESULT CDlgTradeBidContainer::OnMsgCloseTrade( WPARAM w, LPARAM l )
{
	//关闭窗口时隐藏交易窗口(如果有的话)
//	m_DlgOpen.ShowWindow(SW_HIDE);	
//	m_DlgClose.ShowWindow(SW_HIDE);
	m_DlgQuickOrder.ShowWindow(SW_HIDE);
	return 1;
}

LRESULT CDlgTradeBidContainer::OnMsgNetworkStatusChange( WPARAM w, LPARAM l )
{
	if (w==1)//绿色
	{

	}
	else
	{//红色

	}
	
	return 1;
}

LRESULT CDlgTradeBidContainer::OnHoldDetailClick( WPARAM w, LPARAM l )
{
	if (m_TabGroup == Group_open)
	{
		if (m_dlgQuery.m_pHoldDetailMainSelect != NULL)
		{
			bool	bBuy = true;
			CString CommID = m_dlgQuery.m_pHoldDetailMainSelect->stock_code;
			m_DlgOpen.ResetInitControl(CommID,bBuy);
		}
	}
	else if (m_TabGroup == Group_close)
	{
		if (m_dlgQuery.m_pHoldDetailMainSelect != NULL)
		{
			m_DlgClose.m_pHoldDetailQueryOut = m_dlgQuery.m_pHoldDetailMainSelect;
			m_DlgClose.ResetInitControl(m_DlgClose.m_eCloseType);
		}
	}

	return 1;

}

void CDlgTradeBidContainer::InitNetworkServer()
{
	//读取配置
	FromXml();
}

void CDlgTradeBidContainer::OnNetworkMenu(UINT nID)
{
	int iLinkID = 0;

	switch (nID)
	{
	case IDM_DEF_SERVER:
		{
			iLinkID = 0;
		}
		break;
	case IDM_DEF_SERVER+1:
		{
			iLinkID = 1;
		}
		break;
	case IDM_DEF_SERVER+2:
		{
			iLinkID = 2;
		}
		break;
	default:
		break;
	}

	CArray<T_PlusInfo, T_PlusInfo> PlusInfo;
	//更新xml
	m_iSelectedServer = nID-IDM_DEF_SERVER;
	ToXml();
	//通知链路改变，设置链路
}

//获取当前链路
int CDlgTradeBidContainer::GetServerID()
{
	return m_iSelectedServer;
}

//保存xml
bool CDlgTradeBidContainer::ToXml()
{
	CString StrPath  = CPathFactory::GetServerInfoPath();
	
	if ( StrPath.IsEmpty() )
	{
		return FALSE;
	}

	string sPath = _Unicode2MultiChar(StrPath);
	const char* StrFilePath = sPath.c_str();
	
	TiXmlDocument myDocument = TiXmlDocument(StrFilePath);
	
	if ( !myDocument.LoadFile())
	{
		return FALSE;
	}
	// XMLDATA
	TiXmlElement* pRootElement = myDocument.RootElement();
	if ( NULL == pRootElement )
	{
		return FALSE;
	}
	
	// 网络设置
	TiXmlElement* pNetWorkSet = pRootElement->FirstChildElement(KStrElementAttriNetWorkSet);
	if( NULL != pNetWorkSet && NULL != pNetWorkSet->Value()  )
	{
 		ASSERT( 0 == strcmp(pNetWorkSet->Value(), KStrElementAttriNetWorkSet) );

 		TiXmlElement* pNetWork = pNetWorkSet->FirstChildElement(KStrElementAttriNetWork);
 		
		for ( int i=0; NULL!=pNetWork ; pNetWork = pNetWork->NextSiblingElement(KStrElementAttriNetWork), i++ )
		{
			if ( i != m_iSelectedServer)
			{
				pNetWork->SetAttribute(KStrElementAttriNetWorkSelected, "0");
			}
			else
			{
				pNetWork->SetAttribute(KStrElementAttriNetWorkSelected, "1");
			}			
		}		
	}

	myDocument.SaveFile();
	return true;
}

bool CDlgTradeBidContainer::FromXml()
{
	m_aPlusInfo.RemoveAll();

	CString StrPath  = CPathFactory::GetServerInfoPath();
	if ( StrPath.IsEmpty() )
	{
		return FALSE;
	}

	string sPath = _Unicode2MultiChar(StrPath);
	const char* StrFilePath = sPath.c_str();
	
	TiXmlDocument myDocument = TiXmlDocument(StrFilePath);
	
	if ( !myDocument.LoadFile())
	{
		return FALSE;
	}
	
	// XMLDATA
	TiXmlElement* pRootElement = myDocument.RootElement();
	if ( NULL == pRootElement )
	{
		return FALSE;
	}
	
	// 服务器信息
	TiXmlElement* pNetWorkSet = pRootElement->FirstChildElement(KStrElementAttriNetWorkSet);
	if( NULL != pNetWorkSet && NULL != pNetWorkSet->Value()  )
	{
		ASSERT( 0 == strcmp(pNetWorkSet->Value(), KStrElementAttriNetWorkSet) );

		TiXmlElement* pNetWork = pNetWorkSet->FirstChildElement(KStrElementAttriNetWork);
		for (int i=0 ; NULL!=pNetWork ; pNetWork = pNetWork->NextSiblingElement(KStrElementAttriNetWork) )
		{	
			T_PlusInfo plusInfo;
			
			// 名称:
			const char* StrName = pNetWork->Attribute(KStrElementAttriNetWorkName);
			
			if ( NULL == StrName )
			{
				continue;
			}
			
			// URL:
			const char* StrUrl = pNetWork->Attribute(KStrElementAttriNetWorkURL);
			
			if ( NULL == StrUrl )
			{
				continue;
			}
			//port
			const char* strPort = pNetWork->Attribute(KStrElementAttriNetWorkPort);
			
			//selected 
			const char* strSelected = pNetWork->Attribute(KStrElementAttriNetWorkSelected);
			
			// 转换编码:
			TCHAR TStrHostName[1024];
			
			memset(TStrHostName, 0, sizeof(TStrHostName));
			MultiCharCoding2Unicode(EMCCUtf8, StrName, strlen(StrName), TStrHostName, sizeof(TStrHostName) / sizeof(TCHAR));
			plusInfo.strName = TStrHostName;
			
			memset(TStrHostName, 0, sizeof(TStrHostName));
			MultiCharCoding2Unicode(EMCCUtf8, StrUrl, strlen(StrUrl), TStrHostName, sizeof(TStrHostName) / sizeof(TCHAR));
			plusInfo.strUrl = TStrHostName;
			
			if ( NULL != strPort )
				plusInfo.iPort   = atoi(strPort);
			
			if (NULL != strSelected)
			{
				plusInfo.iSelected = atoi(strSelected);
				if (0!=plusInfo.iSelected)
				{
					m_iSelectedServer = i;
				}
			}
			else
				plusInfo.iSelected = 0;
			
			//添加所有服务器信息
			m_aPlusInfo.Add(plusInfo);
			
			i++;			
		}
	}

	return true;
}
//关闭窗口
void CDlgTradeBidContainer::OnBtnCloseWnd()
{
	CDlgTip dlg;
	
	if (ETT_TRADE_FIRM == m_eTradeLoginType)
	{
		dlg.m_strTipMsg = L"确定是否退出交易？";
	}
	else
	{
		dlg.m_strTipMsg = L"确定是否退出模拟交易？";
	}
	dlg.m_strTitle = L"程序关闭";
	dlg.m_eTipType = ETT_TIP;
	dlg.m_pCenterWnd = this;	// 设置要居中到的窗口指针
	
	if (IDOK==dlg.DoModal())
	{
		if(m_pTreeMenu)
		{
			delete m_pTreeMenu;
			m_pTreeMenu = NULL;
		}
		
		InitTree();	// 树好像没有一个，如果已经有选中，再让它没有选中项的函数，所以只能重新创建一个
		
		m_dlgQuery.SetTradeStatus(ETLS_NotLogin);
		OnBtnQuery(EQT_QueryHome);
		RecalcLayout();
		
		m_dlgQuery.WndClose(2);
	}
	//GetParent()->PostMessage(WM_HIDETRADEWND,NULL,NULL);
}

void CDlgTradeBidContainer::ShowHideQuickOrder()
{	
	if (m_DlgQuickOrder.IsWindowVisible())
	{
		m_DlgQuickOrder.ShowWindow(SW_HIDE);
	}
	else
	{
		m_DlgQuickOrder.ShowWindow(SW_SHOW);
	}
}

void CDlgTradeBidContainer::HideQuickOrder()
{
	m_DlgQuickOrder.ShowWindow(SW_HIDE);
}

bool CDlgTradeBidContainer::IsVisibleQuickOrder()
{
	if (m_DlgQuickOrder.IsWindowVisible())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CDlgTradeBidContainer::TradeOpen(CString str,bool bBuy)
{
	m_DlgOpen.ShowWindow(SW_SHOW);
	m_DlgOpen.ResetInitControl(str,bBuy);
	m_DlgOpen.SetPrice();
	m_DlgOpen.SetRange();
}

BOOL CDlgTradeBidContainer::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN || pMsg->wParam == VK_UP ||pMsg->wParam == VK_DOWN ))
	{
		return true;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void  CDlgTradeBidContainer::OnTabSelChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int iCurSel = m_GuiTab.GetCurtab();
	if (m_TabGroup == Group_open)
	{
		if (iCurSel == 0)
		{
			m_DlgOpen.TradeTypeChange(ECT_Market);
		}
		else
		{
			m_DlgOpen.TradeTypeChange(ECT_Limit);
		}
	}
	else if (m_TabGroup == Group_close)
	{
		if (iCurSel == 0)
		{
			m_DlgClose.TradeTypeChange(ECT_Market);
		}
		else
		{
			m_DlgClose.TradeTypeChange(ECT_Limit);
		}
	}
	else if (m_TabGroup == Group_query)
	{
		if (iCurSel == 0)
		{
			m_dlgQuery.ShowQueryChange(EQT_QueryHoldDetail);
			m_pTreeMenu->SelectItem(4);
		}
		else if (iCurSel == 1)
		{
			m_dlgQuery.ShowQueryChange(EQT_QueryHoldSummary);
			m_pTreeMenu->SelectItem(5);
		}
		else if (iCurSel == 2)
		{
			m_dlgQuery.ShowQueryChange(EQT_QueryLimitEntrust);
			m_pTreeMenu->SelectItem(6);
		}
		else if (iCurSel == 3)
		{
			m_dlgQuery.ShowQueryChange(EQT_QueryDeal);
			m_pTreeMenu->SelectItem(7);
		}
	}	
	*pResult = 0;
}

void CDlgTradeBidContainer::ShowAccountInfo(T_CommUserInfoList *pList)
{
	// 可用保证金
	m_staticOccupyValue.SetWindowText(pList[1].pValue3);
	// 当前权益
	m_staticCurrentInsterestValue.SetWindowText(pList[3].pValue1);
	// 浮动盈亏
	m_staticFloatGainLostValue.SetWindowText(pList[0].pValue3);
	if(pList[0].pValue3.Find(L'-') != -1)	// 负值时
	{
		m_staticFloatGainLostValue.SetTextColor(RGB(0, 129, 0));
	}
	else
	{
		m_staticFloatGainLostValue.SetTextColor( RGB(0xC9, 0x13, 0x1F) );
	}

	if (L"0.00" == pList[0].pValue3)
	{
		m_staticFloatGainLostValue.SetTextColor( RGB(77,77,77) );
	}

	CRect rect;
	m_pBtnRefresh->GetWindowRect(rect);
	ScreenToClient(&rect);
	CRect rc = RecalcThreeStatic(rect);
	rc.right = rect.left;
	static int nPosition = 0;
	if((nPosition!=rc.left) && this->IsWindowVisible())	// 如果左边的位置发生变化, 并且当前窗口可见，则重绘一次
	{
		nPosition = rc.left;
		this->RedrawWindow(rc);
	}
}

void CDlgTradeBidContainer::SetGridHeadColor()
{
	m_dlgQuery.SetGridHeadColor();
}