// Trade.cpp : implementation file
//

#include "stdafx.h"
#include "DlgTradeOpen.h"
#include "coding.h"
#include "DlgTip.h"
#include "ShareFun.h"
#include <sstream>
#include "XmlShare.h"
#include "PathFactory.h"
#include "FontFactory.h"
#include "GdiPlusTS.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTradeOpen dialog
namespace
{
#define IDC_DEF_GROUPBOX_FIRST	WM_USER+100
#define IDC_DEF_GROUPBOX_SECOND	WM_USER+101

#define IDC_DEF_DROP_ENTRUSTTYPE	WM_USER+200
#define IDC_DEF_DROP_COMMINFO		WM_USER+201
#define IDC_DEF_DROP_HAND			WM_USER+202
#define IDC_DEF_EDIT_HAND			WM_USER+203
#define IDC_DEF_BUTTON_MAX			WM_USER+204
#define IDC_DEF_SPIN_MAX			WM_USER+205
#define IDC_DEF_DROP_TRADEID		WM_USER+206
#define IDC_DEF_RADIO_BUY			WM_USER+207
#define IDC_DEF_RADIO_SELL			WM_USER+208
#define IDC_DEF_DROP_VALIDITY		WM_USER+209
#define IDC_DEF_EDIT_BUYPRICE		WM_USER+210
#define IDC_DEF_EDIT_SELLPRICE		WM_USER+211
#define IDC_DEF_EDIT_PRICE			WM_USER+212
#define IDC_DEF_SPIN_PRICE			WM_USER+213
#define IDC_DEF_CHECK_STOPLOSS		WM_USER+214
#define IDC_DEF_CHECK_STOPPROFIT	WM_USER+215
#define IDC_DEF_EDIT_STOPLOSS1		WM_USER+216
#define IDC_DEF_EDIT_STOPLOSS2		WM_USER+217
#define IDC_DEF_EDIT_STOPPROFIT1	WM_USER+218
#define IDC_DEF_EDIT_STOPPROFIT2	WM_USER+219
#define IDC_DEF_SPIN_STOPLOSS		WM_USER+220
#define IDC_DEF_SPIN_STOPPROFIT		WM_USER+221
#define IDC_DEF_CHECK_BEFOREORDER	WM_USER+222
#define IDC_DEF_CHECK_ALLOWPOINT	WM_USER+223
#define IDC_DEF_EDIT_PRICEMARKETPRICE	WM_USER+224
#define IDC_DEF_EDIT_POINT				WM_USER+225
#define IDC_DEF_SPIN_POINT			WM_USER+226

#define IDC_DEF_BUTTON_BUYOK		WM_USER+227
#define IDC_DEF_BUTTON_SELLOK		WM_USER+228
#define IDC_DEF_BUTTON_TRADEOK		WM_USER+229
	

#define IDC_DEF_STATIC_ENTRUSTTYPE	WM_USER+300
#define IDC_DEF_STATIC_COMMINFO		WM_USER+301
#define IDC_DEF_STATIC_HAND			WM_USER+302
#define IDC_DEF_STATIC_RANGE		WM_USER+303
#define IDC_DEF_STATIC_TRADEID		WM_USER+304
#define IDC_DEF_STATIC_VALIDITY		WM_USER+305
#define IDC_DEF_STATIC_PRICE		WM_USER+306
#define IDC_DEF_STATIC_MARGINTIP	WM_USER+307
#define IDC_DEF_STATIC_STOPLOSS		WM_USER+308
#define IDC_DEF_STATIC_STOPPROFIT	WM_USER+309
#define IDC_DEF_STATIC_TIP			WM_USER+310
#define IDC_DEF_STATIC_MARGINTIPMARKETPRICE			WM_USER+311
#define IDC_DEF_STATIC_AFTEROK		WM_USER+312
#define IDC_DEF_STATIC_BUYSELL		WM_USER+313


#define IDC_DEF_STATIC_CHECKSTOPLOSS    WM_USER+350
#define IDC_DEF_STATIC_CHECKSTOPPROFIT  WM_USER+351
#define IDC_DEF_STATIC_CHECKALLOWPOINT  WM_USER+352
#define IDC_DEF_STATIC_CHECKBEFOREORDER WM_USER+353

#define TIME_REFRESH_QUOTE			WM_USER+1000
	
#define IDC_DEF_STATIC_HAND_BK			WM_USER+330
#define IDC_DEF_STATIC_POINT_BK			WM_USER+331
#define IDC_DEF_STATIC_STOPLOSS1_BK		WM_USER+332
#define IDC_DEF_STATIC_STOPLOSS2_BK		WM_USER+333
#define IDC_DEF_STATIC_STOPPROFIT1_BK		WM_USER+334
#define IDC_DEF_STATIC_STOPPROFIT2_BK		WM_USER+335
#define IDC_DEF_STATIC_PRICE_BK			WM_USER+336

#define BK_COLOR_BUYPRICE			 RGB(244, 200, 199)
#define BK_COLOR_SELLPRICE			 RGB(174, 202, 239)
#define TEXT_COLOR_BUYPRICE			 RGB(255, 0, 0)
#define TEXT_COLOR_SELLPRICE		 RGB(0, 0, 255)
#define BK_COLOR_EDIT			     RGB(0xe6,0xe6,0xe6)
#define TEXT_COLOR_STATIC_TIP		 RGB(0x22,0x7c,0xed)

#define BK_COLOR_OPENDLG			 RGB(0xF2,0xF0,0xEB)


static const TCHAR		KStrTradeOpenSection[]		= _T("TradeOpen");
static const TCHAR		KStrTradeOpenKeyPoint[]			= _T("point");
static const TCHAR		KStrTradeOpenKeyCheck[]		= _T("check");
}

CDlgTradeOpen::CDlgTradeOpen(CWnd* pParent /*=NULL*/) : CDialog(CDlgTradeOpen::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTrade)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bCheckAllowPoint = FALSE;
	m_bCheckBeforeOrder = FALSE;
	m_bCheckStopLoss = FALSE;
	m_bCheckStopProfit = FALSE;
	m_EntrustType = EET_NONE;
	m_iQty = 1;
	m_iQtyMax = 0;
	m_iQtyMin = 0;
	m_iDefQtyMax = 0;
	m_iPoint = 50;
	m_iPointMax = 100;
	m_iPointMin = 0;
	m_iPointDef = 50;
	m_dPrice = 0;
	m_dStopLoss1 = 0;
	m_dStopProfit1 = 0;
	m_dPriceStep = 0;

	m_pTradeLoginInfo = NULL;
	m_iBuySellSpace = 140;
	m_iBuyRadioOffset = 80;
	m_iPointCount = 0;
	m_strBuyPrice = _T("");
	m_strSellPrice = _T("");
	
	m_pTradeBid = NULL;
	m_bInitCommInfo = false;
	m_bInitTradeID = false;
	m_pTradeQuery = NULL;
	m_dCurrentFund = 0;
	m_dCurrentMargin = 0;
	m_dMarginMarket = 0;
	m_dMarginLimit = 0;
	iMerchType = 0;
	m_maxHold =0;

	m_imgCheck = NULL;
	m_imgUncheck = NULL;
	m_imgCheck      = NULL;
	m_imgUncheck    = NULL;
	m_pImgTradeMax  = NULL;
	m_pImgBuyOK     = NULL;
	m_pImgRadioBuy  = NULL;
	m_pImgSellOK    = NULL;
	m_pImgRadioSell = NULL;

	m_eTradeType = ECT_Market;
}

void CDlgTradeOpen::DoDataExchange(CDataExchange* pDX)
{
//	CDialogEx::DoDataExchange(pDX);
//	DDV_MinMaxInt(pDX, m_iQty, 1, 300);
	//{{AFX_DATA_MAP(CTrade)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgTradeOpen, CDialog)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY(UDN_DELTAPOS, IDC_DEF_SPIN_MAX, OnSpinBtnHand)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DEF_SPIN_POINT, OnSpinBtnPoint)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DEF_SPIN_STOPLOSS, OnSpinBtnStopLoss)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DEF_SPIN_STOPPROFIT, OnSpinBtnStopProfit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DEF_SPIN_PRICE, OnSpinBtnPrice)
	ON_BN_CLICKED(IDC_DEF_CHECK_ALLOWPOINT, OnBnCheckAllowPoint)
	ON_BN_CLICKED(IDC_DEF_CHECK_BEFOREORDER, OnBnCheckBeforeOrder)
	ON_BN_CLICKED(IDC_DEF_CHECK_STOPLOSS, OnBnCheckStopLoss)
	ON_BN_CLICKED(IDC_DEF_CHECK_STOPPROFIT, OnBnCheckStopProfit)
	ON_BN_CLICKED(IDC_DEF_RADIO_BUY, OnBnRadioBuy)
	ON_BN_CLICKED(IDC_DEF_RADIO_SELL, OnBnRadioSell)
	ON_BN_CLICKED(IDC_DEF_BUTTON_MAX,OnBtnMax)
	ON_BN_CLICKED(IDOK,OnBtnOk)
	ON_CBN_SELCHANGE(IDC_DEF_DROP_COMMINFO,OnBtnDropCommInfo)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_DEF_EDIT_PRICE, OnEnChangeEditPrice)
	ON_EN_CHANGE(IDC_DEF_EDIT_POINT, OnEnChangeEditPoint)
	ON_EN_CHANGE(IDC_DEF_EDIT_STOPLOSS1, OnEnChangeEditStopLoss1)
	ON_EN_CHANGE(IDC_DEF_EDIT_STOPPROFIT1, OnEnChangeEditStopProfit1)
	ON_EN_KILLFOCUS(IDC_DEF_EDIT_PRICE, OnEnKillFocusEditPrice)
	ON_EN_KILLFOCUS(IDC_DEF_EDIT_STOPLOSS1, OnEnKillFocusEditStopLoss1)
	ON_EN_KILLFOCUS(IDC_DEF_EDIT_STOPPROFIT1, OnEnKillFocusEditStopProfit1)
	ON_EN_KILLFOCUS(IDC_DEF_EDIT_HAND, OnEnKillFocusEditHand)
	ON_EN_SETFOCUS(IDC_DEF_EDIT_HAND, OnSetFocusEditHand)
	ON_EN_SETFOCUS(IDC_DEF_EDIT_POINT, OnSetFocusEditPoint)
	ON_EN_SETFOCUS(IDC_DEF_EDIT_BUYPRICE, OnSetFocusEditBuyPrice)
	ON_EN_SETFOCUS(IDC_DEF_EDIT_SELLPRICE, OnSetFocusEditSellPrice)
	ON_EN_SETFOCUS(IDC_DEF_EDIT_STOPLOSS2, OnSetFocusEditStopLoss2)
	ON_EN_SETFOCUS(IDC_DEF_EDIT_STOPPROFIT2, OnSetFocusEditStopProfit2)	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrade message handlers
CDlgTradeOpen::~CDlgTradeOpen()
{
	DEL(m_imgCheck);
	DEL(m_imgUncheck);
	DEL(m_pImgTradeMax);
	DEL(m_pImgBuyOK);
	DEL(m_pImgRadioBuy);
	DEL(m_pImgSellOK);
	DEL(m_pImgRadioSell);
}

BOOL CDlgTradeOpen::OnInitDialog()
{
	ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TRADE_CHECK, L"PNG", m_imgCheck);
	ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TRADE_UCHECK, L"PNG", m_imgUncheck);

	CreateControl();	// 创建控件(由于关联了动态变量，则要在OnInitDialog()之前创建控件)

	CDialog::OnInitDialog();

	InitControl();
	RecalcLayout(false);	// 画控件

	LoadTradeOpenInfo();	// 读配置

	return TRUE;
}

void CDlgTradeOpen::CreateControl()
{
	CString strFontName = gFontFactory.GetExistFontName(L"微软雅黑");	//...
		
	LOGFONT lgFont = {0};
	lgFont.lfHeight =20;
    lgFont.lfWidth = 0;
    lgFont.lfEscapement = 0;
    lgFont.lfOrientation = 0;
    lgFont.lfWeight = FW_NORMAL;
    lgFont.lfItalic = 0;
    lgFont.lfUnderline = 0;
    lgFont.lfStrikeOut = 0;
	lgFont.lfCharSet = ANSI_CHARSET;
    lgFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lgFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lgFont.lfQuality = PROOF_QUALITY;
    lgFont.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
	_tcscpy(lgFont.lfFaceName, strFontName);///

	LOGFONT	lgFontBig = lgFont;
	lgFontBig.lfHeight = 15;

	LOGFONT	lgFontEditSmall = lgFont;
//	lgFontEditSmall.lfHeight = 21;

	LOGFONT	lgFontEditBig = lgFont;
	lgFontEditBig.lfHeight = 36;

	LOGFONT	lgFontEditMid = lgFont;
	lgFontEditMid.lfHeight = 18;

	m_fontSmall.CreateFontIndirect(&lgFont);
	m_fontBig.CreateFontIndirect(&lgFontBig);
	m_fontEditSmall.CreateFontIndirect(&lgFontEditSmall);
	m_fontEditBig.CreateFontIndirect(&lgFontEditBig);
	m_fontEditMid.CreateFontIndirect(&lgFontEditMid);	
	
	// 商品
	m_DropCommInfo.Create( WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|CBS_OWNERDRAWFIXED, CRect(0, 0, 0, 200), this, IDC_DEF_DROP_COMMINFO);
	m_DropCommInfo.ShowWindow(SW_SHOW);
	m_DropCommInfo.SetFont(&m_fontSmall);

	m_StaticCommInfo.Create(L"商 品:", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_COMMINFO);
	m_StaticCommInfo.ShowWindow(SW_SHOW);
	m_StaticCommInfo.SetFont(&m_fontSmall);
	
	// 手数
	m_StaticHand.Create(L"手 数:", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_HAND);
	m_StaticHand.ShowWindow(SW_SHOW);
	m_StaticHand.SetFont(&m_fontSmall);

	m_DropHand.Create( WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST, CRect(0, 0, 0, 200), this, IDC_DEF_DROP_HAND);
	m_DropHand.ShowWindow(SW_SHOW);
	m_DropHand.SetFont(&m_fontSmall);

	m_StaticBKHand.Create(L"", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_HAND_BK);
	m_StaticBKHand.ShowWindow(SW_SHOW);
	m_EditHand.Create(ES_MULTILINE|ES_AUTOHSCROLL|ES_NUMBER|ES_CENTER|WS_VISIBLE, CRect(0, 0, 100, 30), this, IDC_DEF_EDIT_HAND);
	m_EditHand.SetFont(&m_fontEditSmall);

	m_SpinHand.Create(WS_CHILD|WS_VISIBLE|UDS_SETBUDDYINT,CRect(0, 0, 0, 0), this, IDC_DEF_SPIN_MAX);
	m_SpinHand.ShowWindow(SW_SHOW);
	
	ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TRADE_MAX, L"PNG", m_pImgTradeMax);
	m_BtnMax.CreateButton(L"最大", CRect(0, 0, 0, 0), this,m_pImgTradeMax,2,IDC_DEF_BUTTON_MAX);
	m_BtnMax.SetTextColor(TEXT_COLOR_STATIC_TIP, TEXT_COLOR_STATIC_TIP, TEXT_COLOR_STATIC_TIP);

	CNCButton::T_NcFont	fontBuySell;
	fontBuySell.m_StrName = strFontName;
	fontBuySell.m_Size	 = 15;
	fontBuySell.m_iStyle = FontStyleRegular;	

	ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_BUY_OK, L"PNG", m_pImgBuyOK);
	m_BuyOK.CreateButton(L"", CRect(0, 0, 0, 0), this,m_pImgBuyOK,2, IDC_DEF_BUTTON_BUYOK);
	m_BuyOK.SetSubCaption(L"买入");
	m_BuyOK.SetFont(fontBuySell);
	m_BuyOK.SetTextOffPos(CPoint(0,4));
	m_BuyOK.SetSubTextOffPos(CPoint(0,2));

	ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_SELL_OK, L"PNG", m_pImgSellOK);
	m_SellOK.CreateButton(L"", CRect(0, 0, 0, 0), this,m_pImgSellOK,2, IDC_DEF_BUTTON_SELLOK);
	m_SellOK.SetSubCaption(L"卖出");
	m_SellOK.SetFont(fontBuySell);
	m_SellOK.SetTextOffPos(CPoint(0,4));
	m_SellOK.SetSubTextOffPos(CPoint(0,2));

	ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_RADIOBUY, L"PNG", m_pImgRadioBuy);
	m_RadioBuy.CreateButton(L"买 0", CRect(0, 0, 0, 0), this,m_pImgRadioBuy,4, IDC_DEF_RADIO_BUY);
	m_RadioBuy.SetTextColor(Color(51,50,254),Color(51,50,254),Color(51,50,254));
	m_RadioBuy.SetParentFocus(false);

	ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_RADIOSELL, L"PNG", m_pImgRadioSell);
	m_RadioSell.CreateButton(L"卖 0", CRect(0, 0, 0, 0), this,m_pImgRadioSell,4, IDC_DEF_RADIO_SELL);
	m_RadioSell.SetTextColor(Color(0x2e,0x99,0x2e),Color(0x2e,0x99,0x2e),Color(0x2e,0x99,0x2e));
	m_RadioSell.SetParentFocus(false);

	m_TradeOk.CreateButton(L"下 单", CRect(0, 0, 0, 0), this,NULL,3, IDC_DEF_BUTTON_TRADEOK);
	m_TradeOk.SetTextBkgColor(Color(237,124,34),Color(200,124,50),Color(237,124,34));
	m_TradeOk.SetTextFrameColor(Color(237,124,34),Color(200,124,50),Color(237,124,34));
	
	m_StaticPrice.Create(L"价 格:", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_PRICE);
	m_StaticPrice.ShowWindow(SW_SHOW);
	m_StaticPrice.SetFont(&m_fontSmall);
	
	m_StaticBKPrice.Create(L"", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_PRICE_BK);
	m_StaticBKPrice.ShowWindow(SW_SHOW);
	m_EditPrice.Create(WS_CHILD|WS_VISIBLE/*|WS_BORDER*//*|ES_RIGHT*/|ES_MULTILINE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_EDIT_PRICE);
	m_EditPrice.ShowWindow(SW_SHOW);
	m_EditPrice.SetFont(&m_fontSmall);
	m_SpinPrice.Create(WS_CHILD|WS_VISIBLE|UDS_SETBUDDYINT,CRect(0, 0, 0, 0), this, IDC_DEF_SPIN_PRICE);
	m_SpinPrice.ShowWindow(SW_SHOW);
	
	m_StaticMarginTip.Create(L"", WS_CHILD|WS_VISIBLE|SS_LEFT, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_MARGINTIP);
	m_StaticMarginTip.ShowWindow(SW_HIDE);
	m_StaticMarginTip.SetFont(&m_fontSmall);
	
	if (m_bCheckStopLoss)
	{
		m_CheckStopLoss.CreateButton(L"", CRect(0, 0, 0, 0), this,m_imgCheck, 3, IDC_DEF_CHECK_STOPLOSS);
	}
	else
	{
		m_CheckStopLoss.CreateButton(L"", CRect(0, 0, 0, 0), this,m_imgUncheck, 3, IDC_DEF_CHECK_STOPLOSS);
	}
	m_StaticCheckStopLoss.Create(L"止损", WS_CHILD|WS_VISIBLE|SS_LEFT, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_CHECKSTOPLOSS);
	m_StaticCheckStopLoss.SetFont(&m_fontSmall);
	m_StaticBKStopLoss1.Create(L"", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_STOPLOSS1_BK);
	m_StaticBKStopLoss1.ShowWindow(SW_SHOW);
	m_StaticBKStopLoss2.Create(L"", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_STOPLOSS2_BK);
	m_StaticBKStopLoss2.ShowWindow(SW_SHOW);
	m_EditStopLoss1.Create(WS_CHILD|WS_VISIBLE|ES_MULTILINE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_EDIT_STOPLOSS1);
	m_EditStopLoss1.ShowWindow(SW_SHOW);
	m_EditStopLoss1.SetFont(&m_fontSmall);
	m_EditStopLoss2.Create(WS_CHILD|WS_VISIBLE|ES_MULTILINE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_EDIT_STOPLOSS2);
	m_EditStopLoss2.ShowWindow(SW_SHOW);
	m_EditStopLoss2.SetFont(&m_fontSmall);
	m_SpinStopLoss.Create(WS_CHILD|WS_VISIBLE|UDS_SETBUDDYINT,CRect(0, 0, 0, 0), this, IDC_DEF_SPIN_STOPLOSS);
	m_SpinStopLoss.ShowWindow(SW_SHOW);
	m_StaticStopLoss.Create(L"<", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_STOPLOSS);
	m_StaticStopLoss.ShowWindow(SW_SHOW);
	m_StaticStopLoss.SetFont(&m_fontSmall);
	
	if (m_bCheckStopProfit)
	{
		m_CheckStopProfit.CreateButton(L"", CRect(0, 0, 0, 0), this,m_imgCheck, 3, IDC_DEF_CHECK_STOPPROFIT);
	}
	else
	{
		m_CheckStopProfit.CreateButton(L"", CRect(0, 0, 0, 0), this,m_imgUncheck, 3, IDC_DEF_CHECK_STOPPROFIT);
	}
	m_StaticCheckStopProfit.Create(L"止盈", WS_CHILD|WS_VISIBLE|SS_LEFT, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_CHECKSTOPPROFIT);
	m_StaticCheckStopProfit.SetFont(&m_fontSmall);
	m_StaticBKStopProfit1.Create(L"", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_STOPPROFIT1_BK);
	m_StaticBKStopProfit1.ShowWindow(SW_SHOW);
	m_StaticBKStopProfit2.Create(L"", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_STOPPROFIT2_BK);
	m_StaticBKStopProfit2.ShowWindow(SW_SHOW);
	m_EditStopProfit1.Create(WS_CHILD|WS_VISIBLE|ES_MULTILINE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_EDIT_STOPPROFIT1);
	m_EditStopProfit1.ShowWindow(SW_SHOW);
	m_EditStopProfit1.SetFont(&m_fontSmall);
	m_EditStopProfit2.Create(WS_CHILD|WS_VISIBLE|WS_DISABLED|ES_MULTILINE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_EDIT_STOPPROFIT2);
	m_EditStopProfit2.ShowWindow(SW_SHOW);
	m_EditStopProfit2.SetFont(&m_fontSmall);
	m_SpinStopProfit.Create(WS_CHILD|WS_VISIBLE|UDS_SETBUDDYINT,CRect(0, 0, 0, 0), this, IDC_DEF_SPIN_STOPPROFIT);
	m_SpinStopProfit.ShowWindow(SW_SHOW);
	m_StaticStopProfit.Create(L">", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_STOPPROFIT);
	m_StaticStopProfit.ShowWindow(SW_SHOW);
	m_StaticStopProfit.SetFont(&m_fontSmall);
	
	m_StaticTip.Create(L"", WS_CHILD|WS_VISIBLE|SS_LEFT, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_TIP);
	m_StaticTip.ShowWindow(SW_SHOW);
	m_StaticTip.SetFont(&m_fontSmall);
	
	m_EditPriceMarketPrice.Create(WS_CHILD|WS_VISIBLE/*|WS_BORDER*/|ES_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_EDIT_PRICEMARKETPRICE);
	m_EditPriceMarketPrice.ShowWindow(SW_HIDE);
	m_StaticMarginTipMarketPrice.Create(L"", WS_CHILD|WS_VISIBLE|SS_LEFT, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_MARGINTIPMARKETPRICE);
	m_StaticMarginTipMarketPrice.ShowWindow(SW_HIDE);
	m_StaticMarginTipMarketPrice.SetFont(&m_fontSmall);
	m_StaticCheckAllowPoint.Create(L"允许点差", WS_CHILD|WS_VISIBLE|SS_LEFT, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_CHECKALLOWPOINT);
	m_StaticCheckAllowPoint.SetFont(&m_fontSmall);
	m_StaticCheckBeforeOrder.Create(L"下单前确认", WS_CHILD|WS_VISIBLE|SS_LEFT, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_CHECKBEFOREORDER);
	m_StaticCheckBeforeOrder.SetFont(&m_fontSmall);
	if (m_bCheckAllowPoint)
	{
		m_CheckAllowPoint.CreateButton(L"", CRect(0, 0, 0, 0), this,m_imgCheck, 3, IDC_DEF_CHECK_ALLOWPOINT);
	}
	else
	{
		m_CheckAllowPoint.CreateButton(L"", CRect(0, 0, 0, 0), this,m_imgUncheck, 3, IDC_DEF_CHECK_ALLOWPOINT);
	}

	if (m_bCheckBeforeOrder)
	{
		m_CheckBeforeOrder.CreateButton(L"", CRect(0, 0, 0, 0), this,m_imgCheck, 3, IDC_DEF_CHECK_BEFOREORDER);
	}
	else
	{
		m_CheckBeforeOrder.CreateButton(L"", CRect(0, 0, 0, 0), this,m_imgUncheck, 3, IDC_DEF_CHECK_BEFOREORDER);
	}

	m_StaticBKPoint.Create(L"", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_POINT_BK);
	m_StaticBKPoint.ShowWindow(SW_SHOW);
	m_EditPoint.Create(ES_MULTILINE|ES_AUTOHSCROLL|ES_NUMBER|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_EDIT_POINT);
	m_EditPoint.ShowWindow(SW_SHOW);
	m_EditPoint.SetFont(&m_fontEditSmall);
	m_SpinPoint.Create(WS_CHILD|WS_VISIBLE|UDS_SETBUDDYINT,CRect(0, 0, 0, 0), this, IDC_DEF_SPIN_POINT);
	m_SpinPoint.ShowWindow(SW_SHOW);
	
	m_StaticAfterOk.Create(L"正在处理，请稍后......", WS_CHILD|WS_VISIBLE|SS_LEFT, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_AFTEROK);
	m_StaticAfterOk.ShowWindow(SW_HIDE);
	m_StaticAfterOk.SetFont(&m_fontSmall);

	m_StaticBuySell.Create(L"方 向:", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_BUYSELL);
	m_StaticBuySell.ShowWindow(SW_HIDE);
	m_StaticBuySell.SetFont(&m_fontSmall);
}

//初始化控件
void CDlgTradeOpen::InitControl()
{
	LOGFONT lgFont = {0};
	CFont *pFont = GetFont();
	if ( NULL!=pFont )
	{
		pFont->GetLogFont(&lgFont);
	}
	else
	{
		LOGFONT *plf = CFaceScheme::Instance()->GetSysFont(ESFSmall);
		lgFont = *plf;
	}
	
	this->SetWindowText(L"建仓单");
		
	if (m_bCheckAllowPoint)
	{
		m_EditPoint.EnableWindow(TRUE);
		m_SpinPoint.EnableWindow(TRUE);
	}
	else
	{
		m_EditPoint.EnableWindow(FALSE);
		m_SpinPoint.EnableWindow(FALSE);
	}

	m_DropCommInfo.SetCurSel(0);

	m_EditHand.SetWindowText(L"1");
	m_EditPoint.SetWindowText(L"50");
	m_EntrustType = EET_BUY;

	// 限价委托部分
	if (m_bCheckStopProfit)
	{
		m_EditStopProfit1.EnableWindow(TRUE);
		m_EditStopProfit2.EnableWindow(TRUE);
		m_SpinStopProfit.EnableWindow(TRUE);
	}
	else
	{
		m_EditStopProfit1.EnableWindow(FALSE);
		m_EditStopProfit2.EnableWindow(FALSE);
		m_SpinStopProfit.EnableWindow(FALSE);
	}
	if (m_bCheckStopLoss)
	{
		m_EditStopLoss1.EnableWindow(TRUE);
		m_EditStopLoss2.EnableWindow(TRUE);
		m_SpinStopLoss.EnableWindow(TRUE);
	}
	else
	{
		m_EditStopLoss1.EnableWindow(FALSE);
		m_EditStopLoss2.EnableWindow(FALSE);
		m_SpinStopLoss.EnableWindow(FALSE);
	}
	m_EditPrice.SetWindowText(L"0");
	m_EditStopLoss1.SetWindowText(L"0");
	m_EditStopProfit1.SetWindowText(L"0");

	SetPrice();
	SetRange();

	ShowHideControl_LimitPrice(SW_HIDE);
	ShowHideControl_MarketPrice(SW_SHOW);

	UpdateData(FALSE);
}

// 重设控件初始化
void CDlgTradeOpen::ResetInitControl(CString strCommID,bool bBuy)
{
	if (strCommID.IsEmpty())
	{
		m_DropCommInfo.SetCurSel(0);
	}
	else
	{
		string strTemp="";
		UnicodeToUtf8(strCommID, strTemp);

		int i = GetCommInfoTypeIndex(strTemp.c_str());
		if (i!=-1)
		{
			m_DropCommInfo.SetCurSel(i);
		}
		else
		{
			//ASSERT(0);
			return;
		}
	}

	if (ECT_Limit == m_eTradeType)
	{
		if (bBuy )
		{
			m_RadioBuy.SetCheck(TRUE);
		}
		else
		{
			m_RadioSell.SetCheck(TRUE);
		}
	}

//	m_EditPrice.SetWindowText(L"");
//	m_DropTradeID.SetCurSel(0);
//	m_StaticStopLoss.SetWindowText(_T("<"));
//	m_StaticStopProfit.SetWindowText(_T(">"));
//	m_EditHand.SetWindowText(L"1");
//	m_EditPoint.SetWindowText(L"50");
//	m_bCheckAllowPoint = TRUE;
//	m_CheckAllowPoint.SetCheck(m_bCheckAllowPoint);
//	m_EditPoint.EnableWindow(FALSE);
//	m_SpinPoint.EnableWindow(FALSE);
//	m_bCheckBeforeOrder = TRUE;
//	m_CheckBeforeOrder.SetCheck(m_bCheckBeforeOrder);

//	m_DropValidity.SetCurSel(0);
//	m_bCheckStopProfit = FALSE;
//	m_CheckStopProfit.SetCheck(m_bCheckStopProfit);
//	m_EditStopProfit1.EnableWindow(FALSE);
//	m_EditStopProfit2.EnableWindow(FALSE);
//	m_SpinStopProfit.EnableWindow(FALSE);
//	m_bCheckStopLoss = FALSE;
//	m_CheckStopLoss.SetCheck(m_bCheckStopLoss);
//	m_EditStopLoss1.EnableWindow(FALSE);
//	m_EditStopLoss2.EnableWindow(FALSE);
//	m_SpinStopLoss.EnableWindow(FALSE);

	
//	ShowHideControl_LimitPrice(SW_HIDE);
//	ShowHideControl_MarketPrice(SW_SHOW);
	//
//	LoadTradeOpenInfo();	// 读配置
	m_EditPrice.SetWindowText(L"0");
	m_EditHand.SetWindowText(L"1");
	m_EditStopLoss1.SetWindowText(_T("0"));//切换商品价格置0
	m_EditStopProfit1.SetWindowText(_T("0"));//切换商品价格置0
	SetPrice();
	UpdateData(FALSE);
	UpdataStopLossProfitShow();
}

// 指价委托-根据选择的品种获得限价的取值范围
void CDlgTradeOpen::GetLimitPrice(double &upPrice, double &downPrice)
{
	string strBuyPrice, strSellPrice;
	UnicodeToUtf8(m_strBuyPrice, strBuyPrice);
	UnicodeToUtf8(m_strSellPrice, strSellPrice);
	double dBuyPrice = atof(strSellPrice.c_str());
	double dSellPrice =atof(strBuyPrice.c_str());
		
	T_CommInfoType Infotype;
	if (!FindCommInfoType(m_DropCommInfo.GetCurSel(), Infotype))
	{
		return;
	}
	
	// 查对应商品信息
	int i = 0;
	T_TradeMerchInfo stOutComm;
	for (i=0; i<m_QueryCommInfoResultVector.size(); i++)
	{
		stOutComm = m_QueryCommInfoResultVector[i];
		string strCode;
		UnicodeToUtf8(stOutComm.stock_code, strCode);
		if (_stricmp(Infotype.chTypeValue, strCode.c_str())==0) //方向和代码都相同
		{
			upPrice = dBuyPrice + stOutComm.open_buy_pdd * stOutComm.price_minchange;
			downPrice = dSellPrice - stOutComm.open_sell_pdd * stOutComm.price_minchange;
			break;
		}
	}
}

void CDlgTradeOpen::SetPrice(bool32 bChangePrice)
{
	T_CommInfoType Infotype;
	if (!FindCommInfoType(m_DropCommInfo.GetCurSel(),Infotype))
	{
		//	//ASSERT(0);
		return;
	}
	
	// 查对应商品信息
	int i = 0;
	int iPointCount = 0;
	T_TradeMerchInfo stOutComm;
//	memset(&stOutComm, 0, sizeof(T_TradeMerchInfo));
	for (i=0; i<m_QueryCommInfoResultVector.size(); i++)// 从商品信息过滤价格波动
	{
		stOutComm = m_QueryCommInfoResultVector[i];
		string strCode;
		UnicodeToUtf8(stOutComm.stock_code, strCode);
		if (_stricmp(Infotype.chTypeValue, strCode.c_str())==0) //方向和代码都相同
		{
			m_iQtyMin = stOutComm.min_entrust_amount; // 单笔最小委托
			m_iQtyMax = stOutComm.max_entrust_amount;// 单笔最大委托
			m_iDefQtyMax = m_iQtyMax;

// 			m_iPointMax = stOutComm.max_pdd;//用户报价点差最大值
// 			m_iPointMin = stOutComm.min_pdd;//用户报价点差最小值
			if(0 < stOutComm.default_pdd)
			{
				m_iPointDef = (int)(stOutComm.default_pdd);//用户报价点差默认值
			}
				
			m_dPriceStep = stOutComm.price_minchange;
  			iPointCount = GetPointCount(stOutComm.price_minchange);			
			m_iPointCount = iPointCount;	//小数点后保留几位

			m_maxHold = stOutComm.max_hold_amount;
			break;
		}
	}
	
	//查对应行情信息
	T_RespQuote stOutQuotation;
	double dBuyPrice = 0;	// 买价
	double dSellPrice = 0;	// 卖价
//	memset(&stOutQuotation, 0, sizeof(T_RespQuote));
	for (i=0; i<m_QuotationResultVector.size(); i++)// 从行情过滤
	{
		stOutQuotation = m_QuotationResultVector[i];
		string strCode;
		UnicodeToUtf8(stOutQuotation.stock_code, strCode);
		if (_stricmp(Infotype.chTypeValue, strCode.c_str())==0) //商品代码相同
		{
			if (ETT_TRADE_SIMULATE == m_pTradeLoginInfo->eTradeLoginType)
			{
				dBuyPrice = stOutQuotation.buy_price;
				dSellPrice = stOutQuotation.sell_price;
			}
			else
			{
				if (_T("1") == stOutComm.price_mode)
				{
					dBuyPrice = stOutQuotation.last_price + stOutComm.buy_pdd*stOutComm.price_minchange;
					dSellPrice = stOutQuotation.last_price + stOutComm.sell_pdd*stOutComm.price_minchange;
				}
				else
				{
					dBuyPrice = stOutQuotation.buy_price + stOutComm.buy_pdd*stOutComm.price_minchange;
					dSellPrice = stOutQuotation.sell_price + stOutComm.sell_pdd*stOutComm.price_minchange;
				}
			}
			break;
		}
	}

	if (((dBuyPrice<0.0000001&&dBuyPrice>-0.0000001) ||(dSellPrice<0.0000001&&dSellPrice>-0.0000001)) && (ETT_TRADE_SIMULATE!=m_pTradeLoginInfo->eTradeLoginType))
	{
		if (_T("1") == stOutComm.price_mode)
		{
			dBuyPrice = stOutComm.last_price + stOutComm.buy_pdd*stOutComm.price_minchange;
			dSellPrice = stOutComm.last_price + stOutComm.sell_pdd*stOutComm.price_minchange;
		}
		else
		{
			dBuyPrice = stOutComm.buy_price + stOutComm.buy_pdd*stOutComm.price_minchange;
			dSellPrice = stOutComm.sell_price + stOutComm.sell_pdd*stOutComm.price_minchange;
		}
	}

	//根据市价和限价来不同显示买卖价
	CString strPrice = _T("");
	if (ECT_Market == m_eTradeType)//市价
	{
		m_strBuyPrice = Float2String(dBuyPrice, iPointCount);
		m_strSellPrice = Float2String(dSellPrice, iPointCount);
	
		m_BuyOK.SetCaption(m_strBuyPrice);
		m_SellOK.SetCaption(m_strSellPrice);

	}
	else if (ECT_Limit == m_eTradeType)//限价
	{
		if ((dBuyPrice<0.0000001&&dBuyPrice>-0.0000001) ||(dSellPrice<0.0000001&&dSellPrice>-0.0000001))
		{
			dBuyPrice = 0.;
			dSellPrice = 0.;
		}
		else
		{
			dBuyPrice = dBuyPrice + stOutComm.open_buy_pdd*stOutComm.price_minchange;
			dSellPrice = dSellPrice - stOutComm.open_sell_pdd*stOutComm.price_minchange;
		}
		m_strBuyPrice = Float2String(dBuyPrice, m_iPointCount);
		m_strSellPrice = Float2String(dSellPrice, m_iPointCount);
		
		CString strShowBuy = L"买 " + m_strBuyPrice;
		CString strShowSell = L"卖 " + m_strSellPrice;
		m_RadioBuy.SetCaption(strShowBuy);
		m_RadioSell.SetCaption(strShowSell);
	}

	if (EET_BUY == m_EntrustType)//买
	{
		m_dPrice = dBuyPrice;
	}
	else if (EET_SELL == m_EntrustType)
	{
		m_dPrice = dSellPrice;
	}
	else
	{
		//ASSERT(0);
		return;
	}

	if (ECT_Limit == m_eTradeType)//限价
	{
		CString str;
		m_EditPrice.GetWindowText(str);
		
		if (L"0"==str || bChangePrice)
		{
			str = Float2String(m_dPrice, m_iPointCount, FALSE, FALSE);
	 		m_EditPrice.SetWindowText(str);
		}
		
		UpdataStopLossProfitShow();
	}

	if (ETT_TRADE_SIMULATE == m_pTradeLoginInfo->eTradeLoginType)
	{
		m_iQtyMax = OpenMaxVol();
		if (0 >= m_iQtyMax)
		{
			m_iQtyMax = 1;
		}

		if (m_iQtyMax > m_iDefQtyMax )
		{
			m_iQtyMax = m_iDefQtyMax;
		}
	}

	UpdateData(FALSE);	
	UpdataMarginTip();
}

// 设置开平范围
void CDlgTradeOpen::SetRange()
{
	//具体要根据价格取值，要问正欣怎么算
	m_iQtyMin = (m_iQtyMax>0)?1:0;
	
	CString str = _T("");
	str.Format(_T("(可填范围:%d-%d)"),m_iQtyMin,m_iQtyMax);
//	m_StaticRange.SetWindowText(str);
	str.Format(_T("%d"),m_iQtyMin);
	m_EditHand.SetWindowText(str);
}

void CDlgTradeOpen::RecalcLayout( bool bNeedDraw )
{
	 int iStaticVSpace	= 10;				// 控件竖向间隔
	 int iSpace			= 10;				// 控件横向间隔
	 int iInset			= 10;				// 对话框周边间隔
	 int iButtonHeight	= 30;
	 int iButtonWidth	= 20;
	 int iSpinWidth		= 15;
	 int iBtnBuySellHeight	= 55;
	 int iBtnBuySellWidth	= 150;
	 int iRadioBuySellWidth = 125;

	int iCheckWidth = 18;
	int iCheckHeight = 18;

	int iStaticPos = 5;

	CRect rc(0,0,0,0);
	GetClientRect(rc);

	if (m_eTradeType == ECT_Market)
	{
		// 商品
		CRect rectStaticCommInfo(rc);
		rectStaticCommInfo.left = rc.left + iInset;
		rectStaticCommInfo.top = rc.top + iInset + iStaticPos;
		rectStaticCommInfo.right = rectStaticCommInfo.left + iButtonWidth * 3;
		rectStaticCommInfo.bottom = rectStaticCommInfo.top + iButtonHeight - iStaticPos;
		if (m_StaticCommInfo.m_hWnd)
		{
			m_StaticCommInfo.MoveWindow(rectStaticCommInfo);
		}
		
		CRect rectDropCommInfo(rectStaticCommInfo);
		rectDropCommInfo.left = rectStaticCommInfo.right+iSpace;
		rectDropCommInfo.top = rectStaticCommInfo.top-iStaticPos;
		rectDropCommInfo.right = rc.right-iInset;
		rectDropCommInfo.bottom = rectDropCommInfo.top+iButtonHeight;
		if (m_DropCommInfo.m_hWnd)
		{
			m_DropCommInfo.MoveWindow(rectDropCommInfo);
		}

		// 手数
		CRect rectStaticHand(rectStaticCommInfo);
		rectStaticHand.top = rectStaticCommInfo.bottom+iStaticVSpace + iStaticPos;
		rectStaticHand.bottom = rectStaticHand.top+iButtonHeight - iStaticPos;
		if (m_StaticHand.m_hWnd)
		{
			m_StaticHand.MoveWindow(rectStaticHand);
		}

		CRect rectStaticBKHand(rectStaticHand);
		rectStaticBKHand.left = rectStaticHand.right+iSpace;
		rectStaticBKHand.top = rectStaticHand.top - iStaticPos;
		rectStaticBKHand.right = rectStaticBKHand.left+iButtonWidth*4;
		rectStaticBKHand.bottom = rectStaticBKHand.top+iButtonHeight;
		if (m_StaticBKHand.m_hWnd)
		{
			m_StaticBKHand.MoveWindow(rectStaticBKHand);
		}
		CRect rectEditHand(rectStaticBKHand);
		rectEditHand.InflateRect(-1,-1,-1,-1);
		if (m_EditHand.m_hWnd)
		{
			m_EditHand.MoveWindow(rectEditHand);
			CRect rect;
			m_EditHand.GetClientRect(&rect);
			rect.DeflateRect(0,3,0,3);
			m_EditHand.SetRectNP(&rect);
		}
		CRect rectSpinHand(rectStaticBKHand);
		rectSpinHand.top = rectStaticBKHand.top + 1;
		rectSpinHand.bottom = rectSpinHand.top + 28;
		rectSpinHand.left = rectStaticBKHand.right+2;
		rectSpinHand.right = rectSpinHand.left+iSpinWidth;
		if (m_SpinHand.m_hWnd)
		{
			m_SpinHand.MoveWindow(rectSpinHand);
		}
		CRect rectBtnMax(rectSpinHand);
		rectBtnMax.left = rectSpinHand.right+2;
		rectBtnMax.right = rectBtnMax.left+50;
		rectBtnMax.bottom =rectBtnMax.top+iButtonHeight;
		m_BtnMax.SetRect(&rectBtnMax);

		// 买卖
		CRect rectBuyOk(rectStaticHand);
		rectBuyOk.top = rectStaticHand.bottom+iStaticVSpace;
		rectBuyOk.right = rectBuyOk.left+ iBtnBuySellWidth;
		rectBuyOk.bottom = rectBuyOk.top+ iBtnBuySellHeight;
		m_BuyOK.SetRect(rectBuyOk);
		
		CRect rectSellOk(rectBuyOk);
		rectSellOk.left = rectBuyOk.right+iInset*2;
		rectSellOk.right = rectSellOk.left + iBtnBuySellWidth;
		m_SellOK.SetRect(rectSellOk);
		
		//允许点差
		CRect rectCheckAllowPoint(rectBuyOk);
		rectCheckAllowPoint.left = rectBuyOk.left;
		rectCheckAllowPoint.top = rectBuyOk.bottom+iStaticVSpace + (iButtonHeight - iCheckHeight)/2;
		rectCheckAllowPoint.right = rectCheckAllowPoint.left+ iCheckWidth;
		rectCheckAllowPoint.bottom = rectCheckAllowPoint.top+iCheckHeight;
		m_CheckAllowPoint.SetRect(rectCheckAllowPoint);

		CRect rStaticCheckAllowPoint(rectCheckAllowPoint);
		rStaticCheckAllowPoint.left = rectCheckAllowPoint.right + 2;
		rStaticCheckAllowPoint.top = rectBuyOk.bottom+iStaticVSpace + iStaticPos;
		rStaticCheckAllowPoint.right = rStaticCheckAllowPoint.left+ 60;
		rStaticCheckAllowPoint.bottom = rStaticCheckAllowPoint.top+iButtonHeight - iStaticPos;
		if (m_StaticCheckAllowPoint.m_hWnd)
		{
			m_StaticCheckAllowPoint.MoveWindow(rStaticCheckAllowPoint);
		}

		CRect rectStaticBKPoint(rStaticCheckAllowPoint);
		rectStaticBKPoint.left = rStaticCheckAllowPoint.right;
		rectStaticBKPoint.top = rStaticCheckAllowPoint.top - iStaticPos;
		rectStaticBKPoint.right = rectStaticBKPoint.left + iButtonWidth*3;
		rectStaticBKPoint.bottom = rectStaticBKPoint.top + iButtonHeight;
		if (m_StaticBKPoint.m_hWnd)
		{
			m_StaticBKPoint.MoveWindow(rectStaticBKPoint);
		}
		CRect rectEditPoint(rectStaticBKPoint);
		rectEditPoint.InflateRect(-1,-1,-1,-1);
		if (m_EditPoint.m_hWnd)
		{
			m_EditPoint.MoveWindow(rectEditPoint);
			CRect rct;
			m_EditPoint.GetClientRect(&rct);
			rct.DeflateRect(0,3,0,3);
			m_EditPoint.SetRectNP(&rct);
		}
		CRect rectSpinPoint(rectStaticBKPoint);
		rectSpinPoint.top = rectStaticBKPoint.top + 1;
		rectSpinPoint.bottom = rectSpinPoint.top + 28;
		rectSpinPoint.left = rectStaticBKPoint.right + 1;
		rectSpinPoint.right = rectSpinPoint.left+iSpinWidth;
		if (m_SpinPoint.m_hWnd)
		{
			m_SpinPoint.MoveWindow(rectSpinPoint);
		}
		
		//下单前确认
		CRect rectCheckBeforeOrder(rectCheckAllowPoint);
		rectCheckBeforeOrder.left = rectSpinPoint.right + iSpace*3;
		rectCheckBeforeOrder.right = rectCheckBeforeOrder.left+iCheckWidth;
		m_CheckBeforeOrder.SetRect(rectCheckBeforeOrder);

		CRect rStaticCheckBeforeOrder(rStaticCheckAllowPoint);
		rStaticCheckBeforeOrder.left = rectCheckBeforeOrder.right + 2;
		rStaticCheckBeforeOrder.right = rStaticCheckBeforeOrder.left+ 100;
		if (m_StaticCheckBeforeOrder.m_hWnd)
		{
			m_StaticCheckBeforeOrder.MoveWindow(rStaticCheckBeforeOrder);
		}

		//保证金提示
		CRect rectMarginTipMarketPrice(rStaticCheckAllowPoint);
		rectMarginTipMarketPrice.left = rc.left + iInset;
		rectMarginTipMarketPrice.top = rStaticCheckAllowPoint.bottom + iStaticVSpace;
		rectMarginTipMarketPrice.right = rc.right - iInset;
		rectMarginTipMarketPrice.bottom = rectMarginTipMarketPrice.top+iButtonHeight - iStaticPos;
		if (m_StaticMarginTipMarketPrice.m_hWnd)
		{
			m_StaticMarginTipMarketPrice.MoveWindow(rectMarginTipMarketPrice);
		}

		CRect rectStaticTip(rectMarginTipMarketPrice);
		rectStaticTip.top = rectMarginTipMarketPrice.bottom + iStaticVSpace - 5;
		rectStaticTip.bottom = rectStaticTip.bottom + iButtonHeight;
		if (m_StaticTip.m_hWnd)
		{
			m_StaticTip.MoveWindow(rectStaticTip);
		}
	}
	else
	{
		iStaticVSpace = 3;
		// 商品
		CRect rectStaticCommInfo(rc);
		rectStaticCommInfo.left = rc.left + iInset;
		rectStaticCommInfo.top = rc.top + iInset;
		rectStaticCommInfo.right = rectStaticCommInfo.left + (int32)(iButtonWidth * 2.5);
		rectStaticCommInfo.bottom = rectStaticCommInfo.top + iButtonHeight - iStaticPos;
		if (m_StaticCommInfo.m_hWnd)
		{
			m_StaticCommInfo.MoveWindow(rectStaticCommInfo);
		}
		
		CRect rectDropCommInfo(rectStaticCommInfo);
		rectDropCommInfo.top = rectStaticCommInfo.top - iStaticPos;
		rectDropCommInfo.left = rectStaticCommInfo.right+iSpace;
		rectDropCommInfo.right = rc.right-iInset;
		if (m_DropCommInfo.m_hWnd)
		{
			m_DropCommInfo.MoveWindow(rectDropCommInfo);
		}
		
		CRect rectStaticBuySell(rectStaticCommInfo);
		rectStaticBuySell.top = rectStaticCommInfo.bottom+iStaticVSpace + iStaticPos - 2;
		rectStaticBuySell.bottom = rectStaticBuySell.top+iButtonHeight - iStaticPos;
		if (	m_StaticBuySell.m_hWnd)
		{
			m_StaticBuySell.MoveWindow(rectStaticBuySell);
		}

		// 买卖
		CRect rectBuyOk(rectStaticBuySell);
		rectBuyOk.top = rectStaticBuySell.top - iStaticPos;
		rectBuyOk.left = rectStaticBuySell.right + iSpace;
		rectBuyOk.right = rectBuyOk.left+iRadioBuySellWidth;
		m_RadioBuy.SetRect(rectBuyOk);
		
		CRect rectSellOk(rectBuyOk);
		rectSellOk.left = rectBuyOk.right+iSpace;
		rectSellOk.right = rectSellOk.left+iRadioBuySellWidth;
		m_RadioSell.SetRect(rectSellOk);

		//价格
		CRect rectStaticPrice(rectStaticBuySell);
		rectStaticPrice.top = rectStaticBuySell.bottom+iStaticVSpace + iStaticPos;
		rectStaticPrice.bottom = rectStaticPrice.top+iButtonHeight - iStaticPos;
		if (m_StaticPrice.m_hWnd)
		{
			m_StaticPrice.MoveWindow(rectStaticPrice);
		}
		
		CRect rectStaticBKPrice(rectStaticPrice);
		rectStaticBKPrice.top = rectStaticPrice.top - iStaticPos;
		rectStaticBKPrice.left = rectStaticPrice.right+ iSpace;
		rectStaticBKPrice.right = rectStaticBKPrice.left + (int32)(iButtonWidth*3.5);
		if (m_StaticBKPrice.m_hWnd)
		{
			m_StaticBKPrice.MoveWindow(rectStaticBKPrice);
		}
		CRect rectEditPrice(rectStaticBKPrice);
		rectEditPrice.InflateRect(-1,-1,-1,-1);
		if (m_EditPrice.m_hWnd)
		{
			m_EditPrice.MoveWindow(rectEditPrice);
			CRect rcRect;
			m_EditPrice.GetClientRect(&rcRect);
			rcRect.DeflateRect(0,3,0,3);
			m_EditPrice.SetRectNP(&rcRect);
		}
		CRect rectSpinPrice(rectStaticBKPrice);
		rectSpinPrice.top = rectStaticBKPrice.top + 1;
		rectSpinPrice.bottom = rectSpinPrice.top + 28;
		rectSpinPrice.left = rectStaticBKPrice.right + 1;
		rectSpinPrice.right = rectSpinPrice.left + iSpinWidth;
		if (m_SpinPrice.m_hWnd)
		{
			m_SpinPrice.MoveWindow(rectSpinPrice);
		}

		// 手数
		CRect rectStaticHand(rectStaticPrice);
		rectStaticHand.left = rectSpinPrice.right;
		rectStaticHand.right = rectStaticHand.left+(int32)(iButtonWidth*2.5);
		if (m_StaticHand.m_hWnd)
		{
			m_StaticHand.MoveWindow(rectStaticHand);
		}
		CRect rectStaticBKHand(rectStaticHand);
		rectStaticBKHand.top -= iStaticPos;
		rectStaticBKHand.left = rectStaticHand.right;
		rectStaticBKHand.right = rectStaticBKHand.left+iButtonWidth*3;
		if (m_StaticBKHand.m_hWnd)
		{
			m_StaticBKHand.MoveWindow(rectStaticBKHand);
		}
		CRect rectEditHand(rectStaticBKHand);
		rectEditHand.InflateRect(-1,-1,-1,-1);
		if (m_EditHand.m_hWnd)
		{
			m_EditHand.MoveWindow(rectEditHand);
			CRect rectTmp;
			m_EditHand.GetClientRect(&rectTmp);
			rectTmp.DeflateRect(0,3,0,3);
			m_EditHand.SetRectNP(&rectTmp);
		}
		CRect rectSpinHand(rectStaticBKHand);
		rectSpinHand.left = rectStaticBKHand.right + 1;
		rectSpinHand.right = rectSpinHand.left+iSpinWidth;
		if (m_SpinHand.m_hWnd)
		{
			m_SpinHand.MoveWindow(rectSpinHand);
		}
		CRect rectBtnMax(rectSpinHand);
		rectBtnMax.left = rectSpinHand.right+1;
		rectBtnMax.right = rc.right - iInset;
		m_BtnMax.SetRect(&rectBtnMax);

		// 	// 止损
		CRect rectCheckStopLoss(rectStaticPrice);
		
		rectCheckStopLoss.top = rectStaticPrice.bottom + iStaticVSpace + (iButtonHeight - iCheckHeight)/2;
		rectCheckStopLoss.right = rectCheckStopLoss.left+iCheckWidth;
		rectCheckStopLoss.bottom = rectCheckStopLoss.top+iCheckHeight;
		m_CheckStopLoss.SetRect(rectCheckStopLoss);

		CRect rStaticCheckStopLoss(rectCheckStopLoss);
		rStaticCheckStopLoss.left = rectCheckStopLoss.right + 2;
		rStaticCheckStopLoss.top = rectStaticPrice.bottom + iStaticVSpace + iStaticPos;
		rStaticCheckStopLoss.right = rStaticCheckStopLoss.left+ 40;
		rStaticCheckStopLoss.bottom = rStaticCheckStopLoss.top+iButtonHeight - iStaticPos;
		if (m_StaticCheckStopLoss.m_hWnd)
		{
			m_StaticCheckStopLoss.MoveWindow(rStaticCheckStopLoss);
		}
		CRect rectStaticBKStopLoss1(rectStaticBKPrice);
		rectStaticBKStopLoss1.top = rectStaticBKPrice.bottom + iStaticVSpace;
		rectStaticBKStopLoss1.bottom = rectStaticBKStopLoss1.top+iButtonHeight;
		if (m_StaticBKStopLoss1.m_hWnd)
		{
			m_StaticBKStopLoss1.MoveWindow(rectStaticBKStopLoss1);
		}
		CRect rectEditStopLoss1(rectStaticBKStopLoss1);
		rectEditStopLoss1.InflateRect(-1,-1,-1,-1);
		if (m_EditStopLoss1.m_hWnd)
		{
			m_EditStopLoss1.MoveWindow(rectEditStopLoss1);
			CRect rctTmp;
			m_EditStopLoss1.GetClientRect(&rctTmp);
			rctTmp.DeflateRect(0,3,0,3);
			m_EditStopLoss1.SetRectNP(&rctTmp);
		}
		CRect rectSpinStopLoss(rectStaticBKStopLoss1);
		rectSpinStopLoss.top = rectStaticBKStopLoss1.top + 1;
		rectSpinStopLoss.bottom = rectSpinStopLoss.top + 28;
		rectSpinStopLoss.left = rectStaticBKStopLoss1.right + 1;
		rectSpinStopLoss.right = rectSpinStopLoss.left+iSpinWidth;
		if (m_SpinStopLoss.m_hWnd)
		{
			m_SpinStopLoss.MoveWindow(rectSpinStopLoss);
		}

		CRect rectStaticStopLoss(rectStaticHand);
		rectStaticStopLoss.top = rectStaticHand.bottom + iStaticVSpace+3;
		rectStaticStopLoss.bottom = rectStaticStopLoss.top+iButtonHeight;
		if (m_StaticStopLoss.m_hWnd)
		{
			m_StaticStopLoss.MoveWindow(rectStaticStopLoss);
		}
		rectStaticStopLoss.top = rectStaticHand.bottom + iStaticVSpace;
		rectStaticStopLoss.bottom = rectStaticStopLoss.top+iButtonHeight;

		CRect rectStaticBKStopLoss2(rectStaticStopLoss);
		rectStaticBKStopLoss2.left = rectStaticStopLoss.right;
		rectStaticBKStopLoss2.right = rc.right - iInset;
		if (m_StaticBKStopLoss2.m_hWnd)
		{
			m_StaticBKStopLoss2.MoveWindow(rectStaticBKStopLoss2);
		}
		CRect rectEditStopLoss2(rectStaticBKStopLoss2);
		rectEditStopLoss2.InflateRect(-1,-1,-1,-1);
		if (m_EditStopLoss2.m_hWnd)
		{
			m_EditStopLoss2.MoveWindow(rectEditStopLoss2);
			CRect rcTmp;
			m_EditStopLoss2.GetClientRect(&rcTmp);
			rcTmp.DeflateRect(0,3,0,3);
			m_EditStopLoss2.SetRectNP(&rcTmp);
		}
		
		// 止止盈
		CRect rectCheckStopProfit(rectCheckStopLoss);
		rectCheckStopProfit.top = rectEditStopLoss2.bottom + iStaticVSpace + (iButtonHeight - iCheckHeight)/2;
		rectCheckStopProfit.bottom = rectCheckStopProfit.top+iCheckHeight;
		m_CheckStopProfit.SetRect(rectCheckStopProfit);

		CRect rStaticCheckStopProfit(rectCheckStopProfit);
		rStaticCheckStopProfit.left = rectCheckStopProfit.right + 2;
		rStaticCheckStopProfit.top = rectEditStopLoss2.bottom + iStaticVSpace + iStaticPos;
		rStaticCheckStopProfit.right = rStaticCheckStopProfit.left+ 40;
		rStaticCheckStopProfit.bottom = rStaticCheckStopProfit.top+iButtonHeight - iStaticPos;
		if (m_StaticCheckStopProfit.m_hWnd)
		{
			m_StaticCheckStopProfit.MoveWindow(rStaticCheckStopProfit);
		}
		
		//边框
		CRect rectStaticBKStopProfit1(rectStaticBKStopLoss1);
		rectStaticBKStopProfit1.top = rectStaticBKStopLoss1.bottom + iStaticVSpace;
		rectStaticBKStopProfit1.bottom = rectStaticBKStopProfit1.top + iButtonHeight;
		if (m_StaticBKStopProfit1.m_hWnd)
		{
			m_StaticBKStopProfit1.MoveWindow(rectStaticBKStopProfit1);
		}
		CRect rectEditStopProfit1(rectStaticBKStopProfit1);
		rectEditStopProfit1.InflateRect(-1,-1,-1,-1);
		if (m_EditStopProfit1.m_hWnd)
		{
			m_EditStopProfit1.MoveWindow(rectEditStopProfit1);
			CRect rt;
			m_EditStopProfit1.GetClientRect(&rt);
			rt.DeflateRect(0,3,0,3);
			m_EditStopProfit1.SetRectNP(&rt);
		}
		CRect rectSpinStopProfit(rectSpinStopLoss);
		rectSpinStopProfit.top = rectSpinStopLoss.bottom + iStaticVSpace+1;
		rectSpinStopProfit.bottom = rectSpinStopProfit.top + 28;
		if (m_SpinStopProfit.m_hWnd)
		{
			m_SpinStopProfit.MoveWindow(rectSpinStopProfit);
		}

		CRect rectStaticStopProfit(rectStaticStopLoss);
		rectStaticStopProfit.top = rectStaticStopLoss.bottom + iStaticVSpace + 3;
		rectStaticStopProfit.bottom = rectStaticStopProfit.top + iButtonHeight;
		if (m_StaticStopProfit.m_hWnd)
		{
			m_StaticStopProfit.MoveWindow(rectStaticStopProfit);
		}

		//边框
		CRect rectStaticBKStopProfit2(rectStaticBKStopLoss2);
		rectStaticBKStopProfit2.top = rectStaticBKStopLoss2.bottom + iStaticVSpace;
		rectStaticBKStopProfit2.bottom = rectStaticBKStopProfit2.top + iButtonHeight;
		if (m_StaticBKStopProfit2.m_hWnd)
		{
			m_StaticBKStopProfit2.MoveWindow(rectStaticBKStopProfit2);
		}
		CRect rectEditStopProfit2(rectStaticBKStopProfit2);
		rectEditStopProfit2.InflateRect(-1,-1,-1,-1);
		if (m_EditStopProfit2.m_hWnd)
		{
			m_EditStopProfit2.MoveWindow(rectEditStopProfit2);
			CRect rect;
			m_EditStopProfit2.GetClientRect(&rect);
			rect.DeflateRect(0,3,0,3);
			m_EditStopProfit2.SetRectNP(&rect);
		}

		CRect rectTradeOk(rectEditStopProfit2);
		rectTradeOk.left = rc.left + iInset;
		rectTradeOk.top = rectEditStopProfit2.bottom + iStaticVSpace*2;
		rectTradeOk.right = rc.right- iInset;
		rectTradeOk.bottom = rectTradeOk.top+iButtonHeight;
		m_TradeOk.SetRect(rectTradeOk);

		//允许点差
		CRect rectCheckAllowPoint(rectTradeOk);
		rectCheckAllowPoint.top = rectTradeOk.bottom+iStaticVSpace + (iButtonHeight - iCheckHeight)/2;
		rectCheckAllowPoint.right = rectCheckAllowPoint.left+ iCheckWidth;
		rectCheckAllowPoint.bottom = rectCheckAllowPoint.top+iCheckHeight;
		m_CheckAllowPoint.SetRect(rectCheckAllowPoint);

		CRect rStaticCheckAllowPoint(rectCheckAllowPoint);
		rStaticCheckAllowPoint.left = rectCheckAllowPoint.right + 2;
		rStaticCheckAllowPoint.top = rectTradeOk.bottom+iStaticVSpace + iStaticPos;
		rStaticCheckAllowPoint.right = rStaticCheckAllowPoint.left+ 60;
		rStaticCheckAllowPoint.bottom = rStaticCheckAllowPoint.top+iButtonHeight - iStaticPos;
		if (m_StaticCheckAllowPoint.m_hWnd)
		{
			m_StaticCheckAllowPoint.MoveWindow(rStaticCheckAllowPoint);
		}
	
		CRect rectStaticBKPoint(rStaticCheckAllowPoint);
		rectStaticBKPoint.top =  rStaticCheckAllowPoint.top - iStaticPos;
		rectStaticBKPoint.left = rStaticCheckAllowPoint.right + 2;
		rectStaticBKPoint.right = rectStaticBKPoint.left + iButtonWidth*3;
		if (m_StaticBKPoint.m_hWnd)
		{
			m_StaticBKPoint.MoveWindow(rectStaticBKPoint);
		}
		CRect rectEditPoint(rectStaticBKPoint);
		rectEditPoint.InflateRect(-1,-1,-1,-1);
		if (m_EditPoint.m_hWnd)
		{
			m_EditPoint.MoveWindow(rectEditPoint);
			CRect rct;
			m_EditPoint.GetClientRect(&rct);
			rct.DeflateRect(0,3,0,3);
			m_EditPoint.SetRectNP(&rct);
		}
		CRect rectSpinPoint(rectStaticBKPoint);
		rectSpinPoint.left = rectStaticBKPoint.right + 1;
		rectSpinPoint.right = rectSpinPoint.left+iSpinWidth;
		if (m_SpinPoint.m_hWnd)
		{
			m_SpinPoint.MoveWindow(rectSpinPoint);
		}
		
		//下单前确认
		CRect rectCheckBeforeOrder(rectSpinPoint);
		rectCheckBeforeOrder.left = rectSpinPoint.right + iSpace*2;
		rectCheckBeforeOrder.top = rectSpinPoint.top + (iButtonHeight - iCheckHeight)/2;
		rectCheckBeforeOrder.right = rectCheckBeforeOrder.left+iCheckWidth;
		rectCheckBeforeOrder.bottom = rectCheckBeforeOrder.top + iCheckHeight;
		m_CheckBeforeOrder.SetRect(rectCheckBeforeOrder);

		CRect rStaticCheckBeforeOrder(rectCheckBeforeOrder);
		rStaticCheckBeforeOrder.left = rectCheckBeforeOrder.right + 2;
		rStaticCheckBeforeOrder.top = rectSpinPoint.top + iStaticPos;
		rStaticCheckBeforeOrder.right = rStaticCheckBeforeOrder.left+ 100;
		rStaticCheckBeforeOrder.bottom = rectSpinPoint.bottom;
		if (m_StaticCheckBeforeOrder.m_hWnd)
		{
			m_StaticCheckBeforeOrder.MoveWindow(rStaticCheckBeforeOrder);
		}

		CRect rectMarginTip(rStaticCheckBeforeOrder);
		rectMarginTip.left = rc.left+ iInset;
		rectMarginTip.top = rStaticCheckBeforeOrder.bottom + 1;
		rectMarginTip.right = rc.right - iInset;
		rectMarginTip.bottom = rectMarginTip.top + 19;
		if (m_StaticMarginTip.m_hWnd)
		{
			m_StaticMarginTip.MoveWindow(rectMarginTip);
		}

		CRect rectStaticTip(rectMarginTip);
		rectStaticTip.top = rectMarginTip.bottom + 1;
		rectStaticTip.bottom = rectStaticTip.top + 19;
		if (m_StaticTip.m_hWnd)
		{
			m_StaticTip.MoveWindow(rectStaticTip);
		}
	}

	if ( bNeedDraw )
	{
		Invalidate(TRUE);
	}
}

void CDlgTradeOpen::OnSize( UINT nType, int cx, int cy )
{
	CDialog::OnSize(nType, cx, cy);
	
	RecalcLayout(false);
}

void CDlgTradeOpen::OnSpinBtnHand( NMHDR *pNotify, LRESULT *pRes )
{
//	m_EditHand.SetFocus();	//将焦点设置过来

	LPNMUPDOWN pNMUpDown = (LPNMUPDOWN)pNotify;

	// 获取当前值
	CString str = _T("");
	string strQty;
	m_EditHand.GetWindowText(str);
	UnicodeToUtf8(str, strQty);
	m_iQty = atoi(strQty.c_str());
	
	// TODO: 在此添加控件通知处理程序代码
	if(pNMUpDown->iDelta >0/*== 1*/) // 如果此值为1 , 说明点击了Spin的往下箭头 
	{

		m_iQty -= 1;
		if (m_iQty<m_iQtyMin)
		{
			m_iQty = m_iQtyMin;
		}
	} 
	else if(pNMUpDown->iDelta <0/*== -1*/) // 如果此值为-1 , 说明点击了Spin的往上箭头 
	{
		m_iQty += 1;
		if (m_iQty>m_iQtyMax)
		{
			m_iQty = m_iQtyMax;
		}
    }

	//显示
	str.Format(_T("%d"),m_iQty);
	m_EditHand.SetWindowText(str);
	m_EditHand.SetFocus();	//将焦点设置过来
	//lint --e(570)
	m_EditHand.SetSel(-1);

	str.Format(_T("提示：交易手数%d至%d ！"),m_iQtyMin,m_iQtyMax);
	m_StaticTip.SetWindowText(str);

	UpdataMarginTip();
 }

void CDlgTradeOpen::OnSpinBtnPoint( NMHDR *pNotify, LRESULT *pRes )
{
	LPNMUPDOWN pNMUpDown = (LPNMUPDOWN)pNotify;

	// 获取当前值
	CString str = _T("");
	string strQty;
	m_EditPoint.GetWindowText(str);
	UnicodeToUtf8(str, strQty);
	m_iPoint = atoi(strQty.c_str());

	// TODO: 在此添加控件通知处理程序代码
	if(pNMUpDown->iDelta >0/*== 1*/) // 如果此值为1 , 说明点击了Spin的往下箭头 
	{
		m_iPoint -= 1;
		if (m_iPoint<m_iPointMin)
		{
			m_iPoint = m_iPointMin;
		}
	} 
	else if(pNMUpDown->iDelta <0/*== -1*/) // 如果此值为-1 , 说明点击了Spin的往上箭头 
	{
		m_iPoint += 1;
		if (m_iPoint>m_iPointMax)
		{
			m_iPoint = m_iPointMax;
		}
    }
	//显示
	str.Format(_T("%d"),m_iPoint);
	m_EditPoint.SetWindowText(str);

	str.Format(_T("提示：可设置点差范围最小%d、最大%d ！"),m_iPointMin,m_iPointMax);
	m_StaticTip.SetWindowText(str);
 }

void CDlgTradeOpen::OnSpinBtnStopLoss( NMHDR *pNotify, LRESULT *pRes )
{
// 	m_EditStopLoss1.SetFocus();	//将焦点设置过来

	LPNMUPDOWN pNMUpDown = (LPNMUPDOWN)pNotify;
	
	// 获取当前值
 	CString str = _T("");
// 	string strQty;
// 	m_EditStopLoss1.GetWindowText(str);
// 	UnicodeToUtf8(str, strQty);
// 	m_dStopLoss1 = atof(strQty.c_str());
	//为0时取当前价格
	if (m_dStopLoss1>-0.000001&&m_dStopLoss1<0.000001)
	{
		string strQty;
		m_EditStopLoss2.GetWindowText(str);
		UnicodeToUtf8(str, strQty);
		m_dStopLoss1 = atof(strQty.c_str());
	}
	
	// TODO: 在此添加控件通知处理程序代码
	if(pNMUpDown-> iDelta>0/*== 1*/) // 如果此值为1 , 说明点击了Spin的往下箭头 
	{
		m_dStopLoss1 -= m_dPriceStep;
		if (m_dStopLoss1<=0)
		{
			m_dStopLoss1 = 0;
		}
	} 
	else if(pNMUpDown->iDelta <0/*== -1*/) // 如果此值为-1 , 说明点击了Spin的往上箭头 
	{
		m_dStopLoss1 += m_dPriceStep;
    }
	//显示
// 	CString str = _T("");
	str = Float2String(m_dStopLoss1, m_iPointCount, FALSE, FALSE);
	m_EditStopLoss1.SetWindowText(str);
	m_EditStopLoss1.SetFocus();	//将焦点设置过来
	//lint --e(570)
	m_EditStopLoss1.SetSel(-1);
}

void CDlgTradeOpen::OnSpinBtnStopProfit( NMHDR *pNotify, LRESULT *pRes )
{	
// 	m_EditStopProfit1.SetFocus();	//将焦点设置过来

	LPNMUPDOWN pNMUpDown = (LPNMUPDOWN)pNotify;
	
	// 获取当前值
 	CString str = _T("");
// 	string strQty;
// 	m_EditStopProfit1.GetWindowText(str);
// 	UnicodeToUtf8(str, strQty);
// 	m_dStopProfit1 = atof(strQty.c_str());
	//为0时取当前价格
	if (m_dStopProfit1>-0.000001&&m_dStopProfit1<0.000001)
	{
		string strQty;
		m_EditStopProfit2.GetWindowText(str);
		UnicodeToUtf8(str, strQty);
		m_dStopProfit1 = atof(strQty.c_str());
	}
	
	// TODO: 在此添加控件通知处理程序代码
	if(pNMUpDown->iDelta >0/*== 1*/) // 如果此值为1 , 说明点击了Spin的往下箭头 
	{
		m_dStopProfit1 -= m_dPriceStep;
		if (m_dStopProfit1<=0)
		{
			m_dStopProfit1 = 0;
		}
	} 
	else if(pNMUpDown->iDelta <0/*== -1*/) // 如果此值为-1 , 说明点击了Spin的往上箭头 
	{
		m_dStopProfit1 += m_dPriceStep;
    }
	//显示
//	CString str = _T("");
	str = Float2String(m_dStopProfit1, m_iPointCount, FALSE, FALSE);
	m_EditStopProfit1.SetWindowText(str);
	m_EditStopProfit1.SetFocus();	//将焦点设置过来
	//lint --e(570)
	m_EditStopProfit1.SetSel(-1);
}

void CDlgTradeOpen::OnSpinBtnPrice( NMHDR *pNotify, LRESULT *pRes )
{	
	
	CString str = _T("");
	string strPrice;
	m_EditPrice.GetWindowText(str);
	UnicodeToUtf8(str, strPrice);
	double dPrice = atof(strPrice.c_str());
	
	LPNMUPDOWN pNMUpDown = (LPNMUPDOWN)pNotify;
	if(pNMUpDown->iDelta >0/* == 1*/) // 如果此值为1 , 说明点击了Spin的往下箭头 
	{
		dPrice -= m_dPriceStep;
		if (dPrice<=10)
		{
			dPrice = 10;
		}
	} 
	else if(pNMUpDown-> iDelta <0/*== -1*/) // 如果此值为-1 , 说明点击了Spin的往上箭头 
	{
		dPrice += m_dPriceStep;
    }

	//显示 
	str = Float2String(dPrice, m_iPointCount, FALSE, FALSE);
	m_EditPrice.SetWindowText(str);	
	m_EditPrice.SetFocus();	//将焦点设置过来
	//lint --e(570)
	m_EditPrice.SetSel(-1);
	UpdataStopLossProfitShow();
	UpdataMarginTip();
}

//价格输入框内容改变
void CDlgTradeOpen::OnEnChangeEditPrice()
{
	CString str = _T("");
	string strPrice;
	m_EditPrice.GetWindowText(str);
	UnicodeToUtf8(str, strPrice);
	double dPrice = atof(strPrice.c_str());
	if (dPrice<0)
	{
		m_EditPrice.SetWindowText(_T("0"));
	}

	UpdataStopLossProfitShow();
	UpdataMarginTip();
}

void CDlgTradeOpen::OnEnKillFocusEditPrice()
{
// 	CString strPrice = _T("");	
// 	strPrice = Float2String(m_dPrice, m_iPointCount, FALSE, FALSE);
// 	m_EditPrice.SetWindowText(strPrice);
// 	m_EditPrice.SetSel(-1);
}

void CDlgTradeOpen::OnEnChangeEditPoint()
{
	CString str = _T("");
	string strPoint;
	m_EditPoint.GetWindowText(str);
	UnicodeToUtf8(str, strPoint);
	int iPoint = atoi(strPoint.c_str());
	if (iPoint<0)
	{
		m_EditPoint.SetWindowText(_T("0"));
	}
//	UpdateData(FALSE);

	//lint --e(570)
	m_EditPoint.SetSel(-1);
}

void CDlgTradeOpen::OnEnChangeEditStopLoss1()
{
	CString str = _T("");
	string strPrice;
	m_EditStopLoss1.GetWindowText(str);
	UnicodeToUtf8(str, strPrice);
	m_dStopLoss1 = atof(strPrice.c_str());
	if (m_dStopLoss1<0)
	{
		m_EditStopLoss1.SetWindowText(_T("0"));
	}

	//lint --e(570)
	m_EditStopLoss1.SetSel(-1);
}

void CDlgTradeOpen::OnEnChangeEditStopProfit1()
{
	CString str = _T("");
	string strPrice;
	m_EditStopProfit1.GetWindowText(str);
	UnicodeToUtf8(str, strPrice);
	m_dStopProfit1 = atof(strPrice.c_str());	
	if (m_dStopProfit1<0)
	{
		m_EditStopProfit1.SetWindowText(_T("0"));
	}	

	//lint --e(570)
	m_EditStopProfit1.SetSel(-1);
}

void CDlgTradeOpen::OnEnKillFocusEditStopLoss1()
{
	CString strPrice = _T("");	
	strPrice = Float2String(m_dStopLoss1, m_iPointCount, FALSE, FALSE);
	m_EditStopLoss1.SetWindowText(strPrice);
	//lint --e(570)
	m_EditStopLoss1.SetSel(-1);
}

void CDlgTradeOpen::OnEnKillFocusEditStopProfit1()
{
	CString strPrice = _T("");	
	strPrice = Float2String(m_dStopProfit1, m_iPointCount, FALSE, FALSE);
	if (L"-" == strPrice)
	{
		strPrice = L"0";
	}
	m_EditStopProfit1.SetWindowText(strPrice);
	//lint --e(570)
	m_EditStopProfit1.SetSel(-1);
}

void CDlgTradeOpen::OnEnKillFocusEditHand()
{
	m_StaticTip.SetWindowText(_T(""));
}

void CDlgTradeOpen::OnSetFocusEditHand()
{
	if (ETT_TRADE_SIMULATE == m_pTradeLoginInfo->eTradeLoginType)
	{
		m_iQtyMax = OpenMaxVol();
		if (0 >= m_iQtyMax)
		{
			m_iQtyMax = 1;
		}

		if (m_iQtyMax > m_iDefQtyMax )
		{
			m_iQtyMax = m_iDefQtyMax;
		}
	}

	CString str = _T("");
	str.Format(_T("提示：交易手数%d至%d ！"),m_iQtyMin,m_iQtyMax);
	m_StaticTip.SetWindowText(str);
}

void CDlgTradeOpen::OnSetFocusEditPoint()
{
	CString str = _T("");
	str.Format(_T("提示：可设置点差范围最小%d、最大%d ！"),m_iPointMin,m_iPointMax);
	m_StaticTip.SetWindowText(str);
}

void CDlgTradeOpen::OnSetFocusEditBuyPrice()
{
//	GetDlgItem(IDC_DEF_STATIC_ENTRUSTTYPE)->SetFocus();	//焦点转移
}

void CDlgTradeOpen::OnSetFocusEditSellPrice()
{
//	GetDlgItem(IDC_DEF_STATIC_ENTRUSTTYPE)->SetFocus();	//焦点转移
}

void CDlgTradeOpen::OnSetFocusEditStopLoss2()
{
//	GetDlgItem(IDC_DEF_STATIC_ENTRUSTTYPE)->SetFocus();	//焦点转移
}

void CDlgTradeOpen::OnSetFocusEditStopProfit2()
{
//	GetDlgItem(IDC_DEF_STATIC_ENTRUSTTYPE)->SetFocus();	//焦点转移
}

HBRUSH CDlgTradeOpen::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	switch(nCtlColor)
	{
	case CTLCOLOR_EDIT:
		{
			if(pWnd->GetDlgCtrlID() == IDC_DEF_EDIT_BUYPRICE)
			{
				pDC->SetBkMode(TRANSPARENT);
				// 交易类型
				COLORREF colorBK = BK_COLOR_BUYPRICE;
				COLORREF colorTEXT = TEXT_COLOR_BUYPRICE;
				if (ECT_Market==m_eTradeType)
				{
					colorBK = BK_COLOR_BUYPRICE;
					colorTEXT = TEXT_COLOR_BUYPRICE;
				}
				else if (ECT_Limit==m_eTradeType)
				{
					colorBK = BK_COLOR_SELLPRICE;
					colorTEXT = TEXT_COLOR_SELLPRICE;
				}
				/*static */HBRUSH hbrEdit = ::CreateSolidBrush( colorBK );
				pDC->SetBkColor( colorBK );
				pDC->SetTextColor( colorTEXT );
				return hbrEdit;	
			}
			else if (pWnd->GetDlgCtrlID() == IDC_DEF_EDIT_SELLPRICE)
			{
				pDC->SetBkMode(TRANSPARENT);
				// 交易类型
			
				COLORREF colorBK = BK_COLOR_BUYPRICE;
				COLORREF colorTEXT = TEXT_COLOR_BUYPRICE;
				if (ECT_Market==m_eTradeType)
				{
					colorBK = BK_COLOR_SELLPRICE;
					colorTEXT = TEXT_COLOR_SELLPRICE;
				}
				else if (ECT_Limit==m_eTradeType)
				{
					colorBK = BK_COLOR_BUYPRICE;
					colorTEXT = TEXT_COLOR_BUYPRICE;
				}
				/*static */HBRUSH hbrEdit = ::CreateSolidBrush( colorBK );
				pDC->SetBkColor( colorBK );
				pDC->SetTextColor( colorTEXT );
				return hbrEdit;	
			}
			else if (pWnd->GetDlgCtrlID() == IDC_DEF_EDIT_PRICEMARKETPRICE)
 			{
				static HBRUSH hbrEdit = ::CreateSolidBrush( BK_COLOR_BUYPRICE );
				pDC->SetBkColor( BK_COLOR_BUYPRICE );
				pDC->SetTextColor( TEXT_COLOR_BUYPRICE );
				return hbrEdit;	
			}
			else if ((pWnd->GetDlgCtrlID() == IDC_DEF_EDIT_STOPLOSS2)
				||(pWnd->GetDlgCtrlID() == IDC_DEF_EDIT_STOPPROFIT2))
			{
				static HBRUSH hbrEdit = ::CreateSolidBrush( BK_COLOR_SELLPRICE );
				pDC->SetBkColor( BK_COLOR_SELLPRICE );
				pDC->SetTextColor( TEXT_COLOR_SELLPRICE );
				return hbrEdit;	
			}
			static HBRUSH hbrEdit = ::CreateSolidBrush(RGB(255,255,255));
			pDC->SetBkColor(RGB(255,255,255));
			return hbrEdit;	
		}
		break;
	case CTLCOLOR_STATIC:
		{
			if ((pWnd->GetDlgCtrlID() == IDC_DEF_STATIC_MARGINTIP)||
				(pWnd->GetDlgCtrlID() == IDC_DEF_STATIC_MARGINTIPMARKETPRICE)||
				(pWnd->GetDlgCtrlID() == IDC_DEF_STATIC_TIP))
			{
				pDC->SetBkMode(TRANSPARENT);
				pDC->SetTextColor(TEXT_COLOR_STATIC_TIP);	// 静态文本框文字的颜色 
			}
			else if ((pWnd->GetDlgCtrlID() == IDC_DEF_STATIC_HAND_BK)
				||(pWnd->GetDlgCtrlID() == IDC_DEF_STATIC_POINT_BK)
				||(pWnd->GetDlgCtrlID() == IDC_DEF_STATIC_STOPLOSS1_BK)
				||(pWnd->GetDlgCtrlID() == IDC_DEF_STATIC_STOPLOSS2_BK)
				||(pWnd->GetDlgCtrlID() == IDC_DEF_STATIC_STOPPROFIT1_BK)
				||(pWnd->GetDlgCtrlID() == IDC_DEF_STATIC_STOPPROFIT2_BK)
				||(pWnd->GetDlgCtrlID() == IDC_DEF_STATIC_PRICE_BK))
			{
				static HBRUSH hTpBr = ::CreateSolidBrush( BK_COLOR_EDIT );	// 边框颜色设置
				pDC->SetBkColor( BK_COLOR_EDIT );
				return hTpBr;
			}
			else
			{
				pDC->SetBkMode(TRANSPARENT);
				pDC->SetTextColor(RGB(0x4d,0x4d,0x4d));
			}

			static HBRUSH hTmpBrush = ::CreateSolidBrush(RGB(0xf2,0xf0,0xeb));
			pDC->SetBkColor(RGB(0xf2,0xf0,0xeb));

			if(m_pTradeLoginInfo->eTradeLoginType == ETT_TRADE_SIMULATE)
			{
				static HBRUSH hbrush = ::CreateSolidBrush(RGB(236, 239, 243));
				pDC->SetBkColor(RGB(236, 239, 243));
				return hbrush;
			}

			return hTmpBrush;
		}
		break;
	default:
		break;
	}
	
	return hbr;
}

// 指价委托时各控件显示
void CDlgTradeOpen::ShowHideControl_LimitPrice(int nCmdShow)
{
	m_StaticPrice.ShowWindow(nCmdShow);
	m_StaticBKPrice.ShowWindow(nCmdShow);
	m_EditPrice.ShowWindow(nCmdShow);
	m_SpinPrice.ShowWindow(nCmdShow);
	m_StaticMarginTip.ShowWindow(nCmdShow);
	m_StaticCheckStopLoss.ShowWindow(nCmdShow);
	m_EditStopLoss1.ShowWindow(nCmdShow);
	m_EditStopLoss2.ShowWindow(nCmdShow);
	m_SpinStopLoss.ShowWindow(nCmdShow);
	m_StaticStopLoss.ShowWindow(nCmdShow);
	m_StaticCheckStopProfit.ShowWindow(nCmdShow);
	m_EditStopProfit1.ShowWindow(nCmdShow);
	m_EditStopProfit2.ShowWindow(nCmdShow);
	m_SpinStopProfit.ShowWindow(nCmdShow);
	m_StaticStopProfit.ShowWindow(nCmdShow);
	m_StaticBKStopProfit1.ShowWindow(nCmdShow);
	m_StaticBKStopProfit2.ShowWindow(nCmdShow);
	m_StaticBKStopLoss1.ShowWindow(nCmdShow);
	m_StaticBKStopLoss2.ShowWindow(nCmdShow);
	m_StaticMarginTip.ShowWindow(nCmdShow);
	m_StaticBuySell.ShowWindow(nCmdShow);

	if (nCmdShow == SW_HIDE)
	{
		m_RadioBuy.SetVisiable(FALSE);
		m_RadioSell.SetVisiable(FALSE);
		m_TradeOk.SetVisiable(FALSE);
		m_CheckStopLoss.SetVisiable(FALSE);
		m_CheckStopProfit.SetVisiable(FALSE);
	}
	else
	{
		m_RadioBuy.SetVisiable(TRUE);
		m_RadioSell.SetVisiable(TRUE);
		m_TradeOk.SetVisiable(TRUE);
		m_CheckStopLoss.SetVisiable(TRUE);
		m_CheckStopProfit.SetVisiable(TRUE);
	}
}

// 市价委托时各控件显示
void CDlgTradeOpen::ShowHideControl_MarketPrice(int nCmdShow)
{
	m_StaticMarginTipMarketPrice.ShowWindow(nCmdShow);
	if (nCmdShow == SW_HIDE)
	{
		m_BuyOK.SetVisiable(FALSE);
		m_SellOK.SetVisiable(FALSE);
	}
	else
	{
		m_BuyOK.SetVisiable(TRUE);
		m_SellOK.SetVisiable(TRUE);
	}
	
}

// 允许成交价和报价的最大点差
void CDlgTradeOpen::OnBnCheckAllowPoint()
{
	// TODO: 在此添加控件通知处理程序代码
	//UpdateData(TRUE);
	if (!m_bCheckAllowPoint)
	{
		m_EditPoint.EnableWindow(TRUE);
		m_SpinPoint.EnableWindow(TRUE);
		m_bCheckAllowPoint = TRUE;
		m_CheckAllowPoint.SetImage(m_imgCheck);
	}
	else
	{
		m_EditPoint.EnableWindow(FALSE);
		m_SpinPoint.EnableWindow(FALSE);
		m_bCheckAllowPoint = FALSE;
		m_CheckAllowPoint.SetImage(m_imgUncheck);
	}
}

// 下单前确认
void CDlgTradeOpen::OnBnCheckBeforeOrder()
{
	// TODO: 在此添加控件通知处理程序代码
	
	if (!m_bCheckBeforeOrder)
	{
		m_bCheckBeforeOrder = TRUE;
		m_CheckBeforeOrder.SetImage(m_imgCheck);
	}
	else
	{
		m_bCheckBeforeOrder = FALSE;
		m_CheckBeforeOrder.SetImage(m_imgUncheck);
	}
}

//止损
void CDlgTradeOpen::OnBnCheckStopLoss()
{
	if (!m_bCheckStopLoss)
	{
		m_EditStopLoss1.EnableWindow(TRUE);
		m_EditStopLoss2.EnableWindow(TRUE);
		m_SpinStopLoss.EnableWindow(TRUE);
		m_bCheckStopLoss = TRUE;
		m_CheckStopLoss.SetImage(m_imgCheck);
	}
	else
	{
		m_EditStopLoss1.EnableWindow(FALSE);
		m_EditStopLoss2.EnableWindow(FALSE);
		m_SpinStopLoss.EnableWindow(FALSE);
		m_bCheckStopLoss = FALSE;
		m_CheckStopLoss.SetImage(m_imgUncheck);
	}
}

// 止盈
void CDlgTradeOpen::OnBnCheckStopProfit()
{
	if (!m_bCheckStopProfit)
	{
		m_EditStopProfit1.EnableWindow(TRUE);
		m_EditStopProfit2.EnableWindow(TRUE);
		m_SpinStopProfit.EnableWindow(TRUE);
		m_bCheckStopProfit = TRUE;
		m_CheckStopProfit.SetImage(m_imgCheck);
	}
	else
	{
		m_EditStopProfit1.EnableWindow(FALSE);
		m_EditStopProfit2.EnableWindow(FALSE);
		m_SpinStopProfit.EnableWindow(FALSE);
		m_bCheckStopProfit = FALSE;
		m_CheckStopProfit.SetImage(m_imgUncheck);
	}
}
// 买
void CDlgTradeOpen::OnBnRadioBuy()
{
	// TODO: 在此添加控件通知处理程序代码
	m_EntrustType = EET_BUY;
	m_StaticStopLoss.SetWindowText(_T("<"));
	m_StaticStopProfit.SetWindowText(_T(">"));
	m_RadioBuy.SetCheckStatus(TRUE);
	m_RadioSell.SetCheckStatus(FALSE);

	SetPrice(true);	// 设价格
//	UpdataStopLossProfitShow();
}

// 卖
void CDlgTradeOpen::OnBnRadioSell()
{
	// TODO: 在此添加控件通知处理程序代码
	m_EntrustType = EET_SELL;
	m_StaticStopLoss.SetWindowText(_T(">"));
	m_StaticStopProfit.SetWindowText(_T("<"));

	m_RadioBuy.SetCheckStatus(FALSE);
	m_RadioSell.SetCheckStatus(TRUE);

	SetPrice(true);	// 设价格
//	UpdataStopLossProfitShow();
}

// 最大按钮
void CDlgTradeOpen::OnBtnMax()
{
	CString str = _T("");

	if (ETT_TRADE_SIMULATE == m_pTradeLoginInfo->eTradeLoginType)
	{
		m_iQtyMax = OpenMaxVol();
		if (0 >= m_iQtyMax)
		{
			m_iQtyMax = 1;
		}

		if (m_iQtyMax > m_iDefQtyMax )
		{
			m_iQtyMax = m_iDefQtyMax;
		}
	}

	str.Format(_T("%d"),m_iQtyMax);
	m_EditHand.SetWindowText(str);

	UpdataMarginTip();
}

void CDlgTradeOpen::OnBtnDropCommInfo()
{
//	m_EditPrice.SetWindowText(_T("0"));//切换商品价格置0
	m_EditHand.SetWindowText(L"1");
	m_EditStopLoss1.SetWindowText(_T("0"));//切换商品价格置0
	m_EditStopProfit1.SetWindowText(_T("0"));//切换商品价格置0


	SetPrice(true);	// 设价格
	SetRange();// 设置范围
}

// void CDlgTradeOpen::OnBtnDropEntrustType()
// {
// //	int iCur = m_DropEntrustType.GetCurSel();
// 
// 	TradeTypeChange((E_TradeType)iCur);
// }

// 确定
void CDlgTradeOpen::OnBtnOk()
{
	if (NULL==m_pTradeLoginInfo||NULL==m_pTradeBid)
	{
		//ASSERT(0);
		return;
	}
	
	//先判断是否符合下单条件
	CString strWarn = _T("");
	if (0!=WarningMsg(strWarn))
	{
		CDlgTip dlg;
		dlg.m_strTipMsg = strWarn;
		dlg.m_eTipType = ETT_WAR;
		dlg.m_pCenterWnd = GetParent();	// 设置要居中到的窗口指针

		if (IDOK==dlg.DoModal())
		{
		}
		
		return;
	}

	CClientReqEntrust order;
	
	CString str = _T("");
	CString strComm = _T("");
	// 商品
	FindCommInfoType(m_DropCommInfo.GetCurSel(), m_CommInfo);
//	m_DropCommInfo.GetWindowText(strComm);
	wstring strName;
	Utf8ToUnicode(m_CommInfo.chTypeName,strName);
	strComm = strName.c_str();

	// 手数
	CString strQty = _T("");
	m_EditHand.GetWindowText(strQty);
	UnicodeToUtf8(strQty, m_strQty);
	
	// 买卖
	CString strBuySell;
	CString strPrice;
	if (EET_BUY==m_EntrustType)
	{
		m_strBuySell = "B";
		//市价和指价价格框互换
		if (ECT_Market==m_eTradeType)//市价
		{
		//	m_EditBuyPrice.GetWindowText(strPrice);

			strPrice = m_BuyOK.GetCaption();
		}
		else if (ECT_Limit==m_eTradeType)//指价
		{
			m_EditPrice.GetWindowText(strPrice);

			// 模拟交易的指价单
			if (ETT_TRADE_SIMULATE == m_pTradeLoginInfo->eTradeLoginType)
			{
				string	sBuyPrice, sSellPrice;
				UnicodeToUtf8(m_strBuyPrice, sBuyPrice);
				UnicodeToUtf8(m_strSellPrice, sSellPrice);
				UnicodeToUtf8(strPrice, m_strPrice);
				double dBuyPrice = atof(sBuyPrice.c_str());
				double dSellPrice = atof(sSellPrice.c_str());
				double dPrice = atof(m_strPrice.c_str());

				if (dPrice >= dBuyPrice) // 高买
				{
					m_strBuySell = "E";
				}
				else if (dPrice <= dSellPrice)	 // 低买
				{
					m_strBuySell = "C";
				}
				else
				{
					CDlgTip dlg;
					dlg.m_strTipMsg = L"价格不符合条件";
					dlg.m_eTipType = ETT_WAR;
					dlg.m_pCenterWnd = GetParent();	// 设置要居中到的窗口指针
					dlg.DoModal();
					return;
				}
			}
		}
		else
		{
			//ASSERT(0);
			return;
		}
		UnicodeToUtf8(strPrice, m_strPrice);
		strBuySell = _T("建仓买入");
	}
	else if (EET_SELL==m_EntrustType)
	{
		m_strBuySell = "S";
		//市价和指价价格框互换
		if (ECT_Market==m_eTradeType)//市价
		{
			//m_EditSellPrice.GetWindowText(strPrice);
			strPrice = m_SellOK.GetCaption();
		}
		else if (ECT_Limit==m_eTradeType)//指价
		{
			m_EditPrice.GetWindowText(strPrice);

			// 模拟交易的指价单
			if (ETT_TRADE_SIMULATE == m_pTradeLoginInfo->eTradeLoginType)
			{
				string	sBuyPrice, sSellPrice;
				UnicodeToUtf8(m_strBuyPrice, sBuyPrice);
				UnicodeToUtf8(m_strSellPrice, sSellPrice);
				UnicodeToUtf8(strPrice, m_strPrice);
				double dBuyPrice = atof(sBuyPrice.c_str());
				double dSellPrice = atof(sSellPrice.c_str());
				double dPrice = atof(m_strPrice.c_str());

				if (dPrice >= dBuyPrice) // 高卖
				{
					m_strBuySell = "F";
				}
				else if (dPrice <= dSellPrice)	 // 低卖
				{
					m_strBuySell = "D";
				}
				else
				{
					CDlgTip dlg;
					dlg.m_strTipMsg = L"价格不符合条件";
					dlg.m_eTipType = ETT_WAR;
					dlg.m_pCenterWnd = GetParent();	// 设置要居中到的窗口指针
					dlg.DoModal();
					return;
				}
			}
		}
		else
		{
			//ASSERT(0);
			return;
		}
		UnicodeToUtf8(strPrice, m_strPrice);
		strBuySell = _T("建仓卖出");
	}
	else
	{
		//ASSERT(0);
		return;
	}

	if (ECT_Market==m_eTradeType)//市价建仓
	{
		if (m_bCheckAllowPoint)//设置点差
		{
			m_EditPoint.GetWindowText(str);
			UnicodeToUtf8(str, m_strPoint);
		}
		else
		{
			std::stringstream stream;
			stream << m_iPointDef;
			m_strPoint =stream.str();
		}
		order.dot_diff = atof((char*)m_strPoint.c_str());
	}
	else
	{
		//止损
		if (m_bCheckStopLoss)
		{
			m_EditStopLoss1.GetWindowText(str);
			UnicodeToUtf8(str, m_strStopLoss);
			double dStopLoss = atof(m_strStopLoss.c_str());
			str = Float2String(dStopLoss, m_iPointCount, FALSE, FALSE);	//四舍五入
			UnicodeToUtf8(str, m_strStopLoss);
			order.stop_loss = atof((char*)m_strStopLoss.c_str());
		}
		else
		{
			order.stop_loss = 0;
		}
		//止盈
		if (m_bCheckStopProfit)
		{
			m_EditStopProfit1.GetWindowText(str);
			UnicodeToUtf8(str, m_strStopProfit);
			double dStopProfit = atof(m_strStopProfit.c_str());
			str = Float2String(dStopProfit, m_iPointCount, FALSE, FALSE);	//四舍五入
			UnicodeToUtf8(str, m_strStopProfit);
			order.stop_profit = atof((char*)m_strStopProfit.c_str());
		}
		else
		{
			order.stop_profit = 0;
		}
	}

	order.account = m_pTradeLoginInfo->StrUser;
	order.entrust_amount = atoi((char*)m_strQty.c_str());
	order.entrust_bs = (char*)m_strBuySell.c_str();
	order.entrust_price = (char*)m_strPrice.c_str();
	order.entrust_prop = "0";
	if (ECT_Market==m_eTradeType)
	{
		order.entrust_type = "0";
	}
	else
	{
		order.entrust_type = "1";
	}
	order.eo_flag = _T("0");
	order.mid_code = m_QueryUserInfoResult.mid_code;

	order.other_id = "";
	if (m_QueryTraderIDVector.size() > 0)
	{
		order.other_id = m_QueryTraderIDVector[0].other_id;
	}

	order.stock_code = m_CommInfo.chTypeValue;
	order.user_session_id = m_pTradeLoginInfo->StrUserSessionID;
	order.user_session_info = m_pTradeLoginInfo->StrUserSessionInfo;
	order.hold_id = "";
	
	//下单前确认
	if (m_bCheckBeforeOrder)
	{
		CString strTip=_T("");
		strTip.Format(_T("商品: %s \r\n商品价格: %s \r\n商品数量: %s \r\n买卖方式: %s \r\n\r\n确定下单吗?"),
				   	  strComm.GetBuffer(), strPrice.GetBuffer(), strQty.GetBuffer(), strBuySell.GetBuffer());
		strComm.ReleaseBuffer();
		strPrice.ReleaseBuffer();
		strQty.ReleaseBuffer();
		strBuySell.ReleaseBuffer();

		CDlgTip dlg;
		dlg.m_strTitle = L"订单信息";
		dlg.m_strTipMsg = strTip;
		dlg.m_eTipType = ETT_TIP;
		dlg.m_pCenterWnd = GetParent();	// 设置要居中到的窗口指针
		
		if (dlg.DoModal() == IDOK)
		{
			//NULL;
		}
		else
		{
			return;
		}
	}
// 	//隐藏相关，显示等待
// 	ShowHideControl_LimitPrice(SW_HIDE);
// 	ShowHideControl_MarketPrice(SW_HIDE);
// 	m_StaticAfterOk.ShowWindow(SW_SHOW);
// 	m_RadioBuy.ShowWindow(SW_HIDE);
// 	m_RadioSell.ShowWindow(SW_HIDE);
// 	m_EditBuyPrice.ShowWindow(SW_HIDE);
//  	m_EditSellPrice.ShowWindow(SW_HIDE);

	// 不经过柜台的提示消息在这里写 lcq 待做
	CString strTip = _T("");
	BOOL bSuc = FALSE;
	if (m_pTradeQuery->ReqEntrust(order, strTip))
	{
		bSuc = TRUE;
	}
	
// 	m_StaticAfterOk.ShowWindow(SW_HIDE);
// 	m_RadioBuy.ShowWindow(SW_SHOW);
// 	m_RadioSell.ShowWindow(SW_SHOW);
// 	m_EditBuyPrice.ShowWindow(SW_SHOW);
// 	m_EditSellPrice.ShowWindow(SW_SHOW);
// 	if (ECT_Market==m_eTradeType)
// 	{
// 		ShowHideControl_LimitPrice(SW_HIDE);
// 		ShowHideControl_MarketPrice(SW_SHOW);
// 	}
// 	else if (ECT_Limit==m_eTradeType)
// 	{
// 		ShowHideControl_LimitPrice(SW_SHOW);
// 		ShowHideControl_MarketPrice(SW_HIDE);
// 	}

	if (!bSuc)
	{
		CDlgTip dlg;
		dlg.m_strTipMsg = strTip;
		dlg.m_eTipType = ETT_ERR;
		dlg.m_pCenterWnd = GetParent();	// 设置要居中到的窗口指针

		if (IDOK==dlg.DoModal())
		{
		}
		return;	// 不关闭
	}
}

bool CDlgTradeOpen::FindOtherFirm(int iIndex, T_CommInfoType &commInfoType)
{
	bool bFind = false;
	
	m_LockTradeID.lock();
	_CommInfoType::iterator it = m_TradeID.find( iIndex );
	if ( it!=m_TradeID.end() )
	{
		commInfoType = it->second;
		bFind = true;
	}
	m_LockTradeID.unlock();
	
	return bFind;
}

bool CDlgTradeOpen::FindCommInfoType(int iIndex, T_CommInfoType &commInfoType)
{
	bool bFind = false;
	
	m_LockCommInfoType.lock();
	_CommInfoType::iterator it = m_CommInfoType.find( iIndex );
	if ( it!=m_CommInfoType.end() )
	{
		commInfoType = it->second;
		bFind = true;
	}
	m_LockCommInfoType.unlock();
	
	return bFind;
}

int CDlgTradeOpen::GetCommInfoTypeIndex(const char* pValue)
{	
	m_LockCommInfoType.lock();
	for (int i=0;i<m_CommInfoType.size();i++)
	{
		const T_CommInfoType type = m_CommInfoType[i];
		if (strcmp(pValue,type.chTypeValue)==0)
		{
			return i;
		}
	}
	
	return -1;
}

void CDlgTradeOpen::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

void CDlgTradeOpen::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rect;
	GetClientRect(&rect);
	
	CBitmap bitmap;	
	CDC MemeDc;

	MemeDc.CreateCompatibleDC(&dc);	
	bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());

	CBitmap *pOldBitmap = MemeDc.SelectObject(&bitmap);
	if(m_pTradeLoginInfo->eTradeLoginType == ETT_TRADE_SIMULATE)
	{
		MemeDc.FillSolidRect(rect, RGB(236, 239, 243));
	}
	else
	{
		MemeDc.FillSolidRect(rect, RGB(0xF2, 0xF0, 0xEB));
	}
	
	Graphics graphics(MemeDc.m_hDC);
	Pen pen(Color(0xFF, 0xE6, 0xE6, 0xE6));
	Rect rcOut(rect.left, rect.top, rect.Width() - 1, rect.Height() - 1);
	graphics.DrawRectangle(&pen, rcOut);

	m_BuyOK.DrawButton(&graphics);
	m_SellOK.DrawButton(&graphics);
	m_RadioBuy.DrawButton(&graphics);
	m_RadioSell.DrawButton(&graphics);
	m_TradeOk.DrawButton(&graphics);
	m_BtnMax.DrawButton(&graphics);
	m_CheckStopLoss.DrawButton(&graphics);
	m_CheckStopProfit.DrawButton(&graphics);
	m_CheckAllowPoint.DrawButton(&graphics);
	m_CheckBeforeOrder.DrawButton(&graphics);

	dc.BitBlt( rect.left, rect.top, rect.Width(), rect.Height(), &MemeDc, 0, 0, SRCCOPY);
	
	MemeDc.SelectObject(pOldBitmap);
	MemeDc.DeleteDC();
	bitmap.DeleteObject();
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDlgTradeOpen::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDlgTradeOpen::OnTimer(UINT/*_PTR*/ nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent==TIME_REFRESH_QUOTE)
	{
	}

	CDialog::OnTimer(nIDEvent);
}

//获取小数点后精确的位数
int CDlgTradeOpen::GetPointCount(const double dSpread)
{
	CString strSpread = _T("");
	strSpread.Format(_T("%f"),dSpread);
	
	int iIndex = -1;
	int iRet = 0;
	iIndex=strSpread.Find('.');
	if (iIndex>=0)
	{
		strSpread.TrimRight('0');
		int iLen = strSpread.GetLength();
		if (iLen==iIndex+1)
		{//整数
            iRet = 0;	
		}
		else
		{//小数
			iRet = iLen-2;
		}
	}

	return iRet;
// 
// 	if (NULL==pStr)
// 	{
// 		return 0;
// 	}
// 
// 	int iCount = 0;
// 	const char *pTemp = pStr;
// 	bool bSuc = false;
// 	while (*pTemp!='\0')
// 	{
// 		if (bSuc)
// 		{
// 			iCount++;
// 		}
// 		if (*pTemp=='.')
// 		{
// 			bSuc = true;
// 		}
// 		pTemp++;
// 	}
// 
// 	return iCount;
}


BOOL CDlgTradeOpen::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	CWnd* pWnd = GetFocus(); 
	if (pMsg->message==WM_KEYDOWN)
	{
		if(pWnd ==GetDlgItem(IDC_DEF_EDIT_BUYPRICE)
			||pWnd ==GetDlgItem(IDC_DEF_EDIT_SELLPRICE)
			||pWnd ==GetDlgItem(IDC_DEF_EDIT_STOPLOSS2)
			||pWnd ==GetDlgItem(IDC_DEF_EDIT_STOPPROFIT2))
		{
			pMsg->wParam = 0;
			pMsg->lParam = 0;
			pMsg->message = 0;
			return TRUE;
		}
	}
	if (pMsg->message==WM_LBUTTONDOWN)
	{
		if ((pWnd!=GetDlgItem(IDC_DEF_SPIN_MAX))
			&&(pWnd!=GetDlgItem(IDC_DEF_SPIN_POINT))
			&&(pWnd!=GetDlgItem(IDC_DEF_EDIT_POINT))
			&&(pWnd!=GetDlgItem(IDC_DEF_EDIT_HAND))
			)
		{
			m_StaticTip.SetWindowText(_T(""));
		}
	}

	if(pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN))
	{
		return true;
	}

	if (WM_SYSKEYDOWN == pMsg->message || WM_KEYDOWN == pMsg->message)
	{
		if ( VK_F12 == pMsg->wParam )
		{
			CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());

			CIoViewBase* p = pMainFrame->FindActiveIoView();
			if (NULL != p)
			{
				pMainFrame->OnQuickTrade(2, p->GetMerchXml());
			}	
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgTradeOpen::SetTradeBid( iTradeBid *pTradeBid )
{
	if ( m_pTradeBid )
	{
		m_pTradeBid->RemoveTradeBidNotify(this);
	}
	
	m_pTradeBid = pTradeBid;
	if (NULL==m_pTradeBid)
	{
		//ASSERT(0);
		return;
	}
	m_pTradeBid->AddTradeBidNotify(this);		
}

void CDlgTradeOpen::OnLoginStatusChanged(int iCurStatus, int iOldStatus)
{

}

bool32 CDlgTradeOpen::OnInitQuery(CString &strTipMsg)
{
	return TRUE;
}

bool32 CDlgTradeOpen::OnQueryUserInfo(CString &strTipMsg)
{
	return TRUE;
}

void CDlgTradeOpen::OnInitCommInfo()
{
// 	if (NULL==m_pTradeBid)
// 	{
// 		return;
// 	}
// 	if (m_bInitCommInfo)
// 	{
// 		return;
// 	}
// 	// 初始化商品列表
// 	int i = 0;
// 	const QueryCommInfoResultVector &aQuery = m_pTradeBid->GetCacheCommInfo();
// 	const int iSize = aQuery.size();
// 	for ( i=0; i<aQuery.size(); ++i )
// 	{
// 		CommodityQueryOut stOut = aQuery[i];
// 		T_CommInfoType type;
// 		type.iIndex = i;
// 		strncpy(type.chTypeValue,stOut.commodityID,sizeof(type.chTypeValue)-1);
// 		strncpy(type.chTypeName,stOut.commodityName,sizeof(type.chTypeName)-1);
// 		_CommInfoType::value_type value( type.iIndex, type );
// 		m_CommInfoType.insert( value );
// 		
// 		wstring wstr;
// 		Gbk32ToUnicode(stOut.commodityName,wstr);
// 		CString str;
// 		str.Format(_T("%s"),wstr.c_str());
// 		m_DropCommInfo.InsertString(i,str);
// 	}
// 
// 	//对方交易员
// 	const QueryOtherFirmResultVector &aQuery2 = m_pTradeBid->GetCacheOtherFirm();
// 	const int iSize2 = aQuery2.size();
// 	for ( i=0; i<aQuery2.size(); ++i )
// 	{
// 		OtherFirmQueryOut stOut = aQuery2[i];
// 		T_CommInfoType type;
// 		type.iIndex = i;
// 		strncpy(type.chTypeValue,stOut.memberID,sizeof(type.chTypeValue)-1);
// 		strncpy(type.chTypeName,stOut.memberName,sizeof(type.chTypeName)-1);
// 		_CommInfoType::value_type value( type.iIndex, type );
// 		m_TradeID.insert( value );
// 		
// 		wstring wstr;
// 		Gbk32ToUnicode(stOut.memberName,wstr);
// 		CString str;
// 		str.Format(_T("%s"),wstr.c_str());
// 		m_DropTradeID.InsertString(i,str);
// 	}
// 
// 	m_DropCommInfo.SetCurSel(0);
// 	m_DropTradeID.SetCurSel(0);

//	m_bInitCommInfo = true;
}

void CDlgTradeOpen::OnQueryHoldDetailResponse()
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}
	
	m_QueryHoldResultVector = m_pTradeBid->GetCacheHoldDetail();
}

void CDlgTradeOpen::OnQueryHoldSummaryResponse()
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}
	m_QueryHoldSummaryResultVector = m_pTradeBid->GetCacheHoldSummary();
}

// void CDlgTradeOpen::OnQueryEntrustResponse()
// {
// 
// }

void CDlgTradeOpen::OnQueryLimitEntrustResponse()
{
	
}

void CDlgTradeOpen::OnQueryDealResponse()
{

}

void CDlgTradeOpen::OnQueryCommInfoResponse()
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}
	
	if (!m_bInitCommInfo)	// 初始化列表一次
	{
		// 初始化商品列表
		int i = 0;
		const QueryCommInfoResultVector &aQuery = m_pTradeBid->GetCacheCommInfo();
		for ( i=0; i<aQuery.size(); ++i )
		{
			T_TradeMerchInfo stOut = aQuery[i];
			T_CommInfoType type;
			type.iIndex = i;
			string strCode, strName;
			UnicodeToUtf8(stOut.stock_code, strCode);
			UnicodeToUtf8(stOut.stock_name, strName);
			strncpy(type.chTypeValue, strCode.c_str(), sizeof(type.chTypeValue)-1);
			strncpy(type.chTypeName, strName.c_str(), sizeof(type.chTypeName)-1);
			_CommInfoType::value_type value( type.iIndex, type );
			m_CommInfoType.insert( value );
			
// 			wstring wstr;
// 			Gbk32ToUnicode((char *)(LPCTSTR)stOut.stock_name, wstr);
// 			CString str;
// 			str.Format(_T("%s"), wstr.c_str());
		//	m_DropCommInfo.InsertString(i, (LPCTSTR)stOut.stock_name);
			m_DropCommInfo.AddItemEx(stOut.stock_name);
		}
		m_DropCommInfo.SetCurSel(0);

		m_bInitCommInfo = true;
	}

	m_QueryCommInfoResultVector = m_pTradeBid->GetCacheCommInfo();	
}

void CDlgTradeOpen::OnQueryTraderIDResponse()
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}

	m_QueryTraderIDVector = m_pTradeBid->GetCacheTraderID();
}

void CDlgTradeOpen::OnQueryUserInfoResponse()
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}
	
	m_QueryUserInfoResult = m_pTradeBid->GetCacheUserInfo();

	m_dCurrentFund = m_QueryUserInfoResult.today_enable;
	m_dCurrentMargin = m_dCurrentFund + m_pTradeQuery->GetFloatPL();	//可用保证金,这里再加上浮动盈亏
}

void CDlgTradeOpen::OnDisConnectResponse()
{
	
}

void CDlgTradeOpen::OnReqEntrustResponse(bool32 bShowTip)
{
	if(!IsWindowVisible())
	{
		return;
	}

	if (m_pTradeBid->m_bReqQuick)
	{
		return;
	}

	E_TipType eTipType = ETT_ERR;
	CClientRespEntrust resp = m_pTradeBid->GetCacheReqEntrust();
	if (0 == resp.error_no) // 成功
	{
		eTipType = ETT_SUC;

		// 通知页面刷新
		if (NULL!=m_pTradeQuery)
		{
			m_pTradeQuery->EntrustResultNotify(true);
			m_pTradeQuery->QueryListData(EQT_QueryLimitEntrust);

			// 请求持仓汇总
			m_pTradeQuery->DoQueryAsy(EQT_QueryHoldSummary);

			// 请求账户信息
			m_pTradeQuery->DoQueryAsy(EQT_QueryUserInfo);
		}

		//保存配置
		SaveTradeOpenInfo();
	}

	if (bShowTip)
	{
		CDlgTip dlg;
		dlg.m_strTipMsg = resp.error_info;
		dlg.m_eTipType = eTipType;
		dlg.m_pCenterWnd = GetParent();	// 设置要居中到的窗口指针
		dlg.DoModal();
	}
}

void CDlgTradeOpen::OnReqCancelEntrustResponse()
{
	
}

void CDlgTradeOpen::OnReqModifyPwdResponse()
{
	
}

void CDlgTradeOpen::OnReqSetStopLPResponse()
{
	
}

void CDlgTradeOpen::OnReqCancelStopLPResponse()
{
	
}

// 查询行情
void CDlgTradeOpen::OnQueryQuotationResponse()
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}
	
	m_QuotationResultVector = m_pTradeBid->GetCacheQuotation();

	SetPrice();	// 行情更新，顺便更新下编辑框
}

int CDlgTradeOpen::GetHaveHoldNum()
{
	T_CommInfoType Infotype;
	if (!FindCommInfoType(m_DropCommInfo.GetCurSel(), Infotype))
	{
		return 0;
	}

	int count = 0;
	// 查持仓
	int i = 0;
	T_RespQueryHold stOutHold;
	for (i=0; i<m_QueryHoldResultVector.size(); i++)
	{
		stOutHold = m_QueryHoldResultVector[i];
		string strCode;
		UnicodeToUtf8(stOutHold.stock_code, strCode);
		if (0 == _stricmp(Infotype.chTypeValue, strCode.c_str()))
		{	
			count += stOutHold.hold_amount;
		}
	}
	return count;
}

// 更新建仓止损止盈
void CDlgTradeOpen::UpdataStopLossProfitShow()
{
	T_CommInfoType Infotype;
	if (!FindCommInfoType(m_DropCommInfo.GetCurSel(),Infotype))
	{
		//	//ASSERT(0);
		return;
	}
	
	// 查对应商品信息
	int i = 0;
	T_TradeMerchInfo stOutComm;
//	memset(&stOutComm, 0, sizeof(T_TradeMerchInfo));
	for (i=0; i<m_QueryCommInfoResultVector.size(); i++)
	{
		stOutComm = m_QueryCommInfoResultVector[i];
		string strCode;
		UnicodeToUtf8(stOutComm.stock_code, strCode);
		if (_stricmp(Infotype.chTypeValue, strCode.c_str())==0)
		{			
			break;
		}
	}


	CString str = _T("");
	string tmpPrice;
	m_EditPrice.GetWindowText(str);
	UnicodeToUtf8(str, tmpPrice);
	double dPrice = atof(tmpPrice.c_str());


	//止损止盈
	double dStopLoss = 0;
	double dStopProfit = 0;
 //	TradeFunction tradeFun;
// 	tradeFun.GetOpenStopLossAndProfit(stOutComm,m_dPrice,m_EntrustType,dStopLoss,dStopProfit);
	if (!(dPrice>-0.0000001&&dPrice<0.0000001))
	{
		// 限价建仓止盈止损价格取值范围
		// 买方向
		if (EET_BUY == m_EntrustType)
		{
			// 直接商品, 间接商品(实盘),  交叉商品
			if(_T("")==stOutComm.quote_type || _T("0")==stOutComm.quote_type || _T("2")==stOutComm.quote_type || _T("3")==stOutComm.quote_type)
			{
				dStopLoss = dPrice-stOutComm.buy_pdd*stOutComm.price_minchange
					+stOutComm.sell_pdd*stOutComm.price_minchange-stOutComm.stop_loss_pdd*stOutComm.price_minchange;
				dStopProfit = dPrice+stOutComm.stop_profit_pdd*stOutComm.price_minchange;
				
				m_StaticStopLoss.SetWindowText(_T("<"));
				m_StaticStopProfit.SetWindowText(_T(">"));
				iMerchType = 0;
			}
			else if (_T("1")==stOutComm.quote_type) // 间接商品（虚盘）
			{
				dStopLoss = dPrice-stOutComm.buy_pdd*stOutComm.price_minchange
					+stOutComm.sell_pdd*stOutComm.price_minchange+stOutComm.stop_profit_pdd*stOutComm.price_minchange;
				dStopProfit = dPrice - stOutComm.stop_loss_pdd*stOutComm.price_minchange;
				
				m_StaticStopLoss.SetWindowText(_T(">"));
				m_StaticStopProfit.SetWindowText(_T("<"));
				iMerchType = 1;
			}
		}
		else  // 卖方向
		{
			// 直接商品, 间接商品(实盘),  交叉商品
			if(_T("")==stOutComm.quote_type || _T("0")==stOutComm.quote_type || _T("2")==stOutComm.quote_type || _T("3")==stOutComm.quote_type)
			{
				dStopLoss = dPrice-stOutComm.sell_pdd*stOutComm.price_minchange+stOutComm.buy_pdd*stOutComm.price_minchange
					+stOutComm.stop_loss_pdd*stOutComm.price_minchange;
				dStopProfit = dPrice - stOutComm.stop_profit_pdd*stOutComm.price_minchange;
				
				m_StaticStopLoss.SetWindowText(_T(">"));
				m_StaticStopProfit.SetWindowText(_T("<"));
				iMerchType = 0;
			}
			else if (_T("1")==stOutComm.quote_type) // 间接商品（虚盘）
			{
				dStopLoss = dPrice-stOutComm.sell_pdd*stOutComm.price_minchange+stOutComm.buy_pdd*stOutComm.price_minchange
					-stOutComm.stop_loss_pdd*stOutComm.price_minchange;
				dStopProfit = dPrice + stOutComm.stop_profit_pdd*stOutComm.price_minchange;
				
				m_StaticStopLoss.SetWindowText(_T("<"));
				m_StaticStopProfit.SetWindowText(_T(">"));
				iMerchType = 1;
			}
		}
	}
	
	CString strPrice = _T("");
	strPrice = Float2String(dStopLoss, m_iPointCount, FALSE, FALSE);
	if (dStopLoss<=0)//小于0则显示0
	{
		m_EditStopLoss2.SetWindowText(_T("0"));
	}
	else
	{
		m_EditStopLoss2.SetWindowText(strPrice);
	}
// 	//为0时取当前价格
// 	if (m_dStopLoss1>-0.000001&&m_dStopLoss1<0.000001)
// 	{
// 		m_dStopLoss1 = dStopLoss;
// 	}
	//m_EditStopLoss1.SetWindowText(strPrice);
	strPrice = Float2String(dStopProfit, m_iPointCount, FALSE, FALSE);
	if (dStopProfit<=0)//小于0则显示0
	{
		m_EditStopProfit2.SetWindowText(_T("0"));
	}
	else
	{
		m_EditStopProfit2.SetWindowText(strPrice);
	}
// 	//为0时取当前价格
// 	if (m_dStopProfit1>-0.000001&&m_dStopProfit1<0.000001)
// 	{
// 		m_dStopProfit1 = dStopProfit;
// 	}
	//m_EditStopProfit1.SetWindowText(strPrice);
}

//占用保证金显示
void CDlgTradeOpen::UpdataMarginTip()
{
	T_CommInfoType Infotype;
	if (!FindCommInfoType(m_DropCommInfo.GetCurSel(),Infotype))
	{
		//	//ASSERT(0);
		return;
	}
	
	// 查对应商品信息
	int i = 0;
	T_TradeMerchInfo stOutComm;
//	memset(&stOutComm, 0, sizeof(T_TradeMerchInfo));
	for (i=0; i<m_QueryCommInfoResultVector.size(); i++)
	{
		stOutComm = m_QueryCommInfoResultVector[i];
		string strCode;
		UnicodeToUtf8(stOutComm.stock_code, strCode);
		if (_stricmp(Infotype.chTypeValue, strCode.c_str())==0)
		{			
			break;
		}
	}

	// 数量
	CString str = _T("");
	string strQty;
	m_EditHand.GetWindowText(str);
	UnicodeToUtf8(str, strQty);
	int iCount = atoi(strQty.c_str());

	// 交易单位
	string strVal;
	UnicodeToUtf8(stOutComm.trade_unit, strVal);
	double dtradeUnit = atof(strVal.c_str());
	
	//交易价格
	double dPrice = 0;
	if (ECT_Market==m_eTradeType)
	{
		str = m_BuyOK.GetCaption(); // 全部以买价算
		//if (EET_BUY==m_EntrustType)
		//{
		//	str = m_BuyOK.GetCaption();
		//}
		//else if (EET_SELL==m_EntrustType)
		//{
		//	str = m_SellOK.GetCaption();
		//}

		string strPrice;
		UnicodeToUtf8(str, strPrice);
		dPrice = atof(strPrice.c_str());
	}
	else
	{
		//获取价格框价格
		string strPrice;
		m_EditPrice.GetWindowText(str);
		UnicodeToUtf8(str, strPrice);
		dPrice = atof(strPrice.c_str());
	}

		
//	TradeFunction tradeFun;
 //	double dMargin = tradeFun.GetOpenMargin(dPrice,dtradeUnit,iCount,stOutComm.margin_ratio);

	//计算保证金
	double dMargin = 0;
	CString strTip;
	if (dtradeUnit != 0 && stOutComm.margin_ratio != 0)
	{
		dMargin = iCount*dtradeUnit*stOutComm.margin_ratio*dPrice;
		
		int iDec = GetPointCount(stOutComm.margin_ratio*100);
		CString strMargin = Float2String(stOutComm.margin_ratio*100, iDec, FALSE, FALSE);
		strTip.Format(_T("建仓保证金比例%s%s，所需占用保证金%.2f元"),strMargin.GetBuffer(),"%",dMargin);
		strMargin.ReleaseBuffer();
	}

	if (ECT_Market==m_eTradeType)
	{
		m_dMarginMarket = dMargin;	//市价
		m_StaticMarginTipMarketPrice.SetWindowText(strTip);
	}
	else
	{
		m_dMarginLimit = dMargin;	//限价
		m_StaticMarginTip.SetWindowText(strTip);
	}
}

int CDlgTradeOpen::WarningMsg(CString &strWarn)
{
	if (L"1" == m_pTradeLoginInfo->StrQuoteFlag)
	{
		CMainFrame *pMain = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
		if (!pMain->m_wndStatusBar.m_bConnect)
		{
			strWarn = _T("没有连接行情服务器！");
			return -1;
		}
	}

	CString str = _T("");
	string strTemp;
	//判断手数
	m_EditHand.GetWindowText(str);
	UnicodeToUtf8(str, strTemp);
	int iQty = atoi(strTemp.c_str());
	if (iQty>m_iQtyMax||iQty<m_iQtyMin)
	{
		strWarn.Format(_T("请输入正确的交易手数。 \r\n交易手数最小%d手、最大%d手！"),m_iQtyMin,m_iQtyMax);
		return -1;
	}

	if (m_bCheckAllowPoint)
	{
		//判断点差
		m_EditPoint.GetWindowText(str);
		UnicodeToUtf8(str, strTemp);
		int iPoint = atoi(strTemp.c_str());
		if (iPoint>m_iPointMax||iPoint<m_iPointMin)
		{
			strWarn.Format(_T("请填写正确点差。\r\n点差范围最小%d、最大%d！"),m_iPointMin,m_iPointMax);
			return -1;
		}
	}

	if (ECT_Market==m_eTradeType)//市价
	{
		//判断保证金
		if ((m_dMarginMarket>m_dCurrentMargin) || (0>=iQty))
		{
			strWarn.Format(_T("可用保证金不足！"));
			return -1;
		}
		// 判断持仓量
// 		if ((GetHaveHoldNum() + iQty) > m_maxHold)
// 		{
// 			strWarn.Format(_T("将超过最大持仓量！"));
// 			return -1;
// 		}

		string	sBuyPrice, sSellPrice;
		UnicodeToUtf8(m_strBuyPrice, sBuyPrice);
		UnicodeToUtf8(m_strSellPrice, sSellPrice);
		double dBuyPrice = atof(sBuyPrice.c_str());
		double dSellPrice = atof(sSellPrice.c_str());
		if ((dBuyPrice>-0.000001&&dBuyPrice<0.000001) || (dSellPrice>-0.000001&&dSellPrice<0.000001))
		{
			strWarn = _T("价格不符合条件！");
			return -1;
		}
	}
	else if (ECT_Limit==m_eTradeType)//限价
	{
		//判断委托价格（等于0的话则提示）
		CString strPrice = _T("");
		string	strPrice2;
		m_EditPrice.GetWindowText(strPrice);
		UnicodeToUtf8(strPrice, strPrice2);
		double dPrice = atof(strPrice2.c_str());
		if (dPrice>-0.000001&&dPrice<0.000001)
		{
			strWarn = _T("价格不符合条件！");
			return -1;
		}
		else
		{
			// 模拟交易，委托价格要在价格区间之外
			if (ETT_TRADE_SIMULATE == m_pTradeLoginInfo->eTradeLoginType)
			{
				string	sBuyPrice, sSellPrice;
				UnicodeToUtf8(m_strBuyPrice, sBuyPrice);
				UnicodeToUtf8(m_strSellPrice, sSellPrice);
				double dBuyPrice = atof(sBuyPrice.c_str());
				double dSellPrice = atof(sSellPrice.c_str());

				if ((dPrice<dBuyPrice) && (dPrice>dSellPrice))
				{
					strWarn = _T("价格不符合条件！");
					return -1;
				}

				if((dBuyPrice>-0.000001&&dBuyPrice<0.000001) || (dSellPrice>-0.000001&&dSellPrice<0.000001))
				{
					strWarn = _T("价格不符合条件！");
					return -1;
				}
			}
		}

		//判断止损止盈价格
		double dStopLoss1 = 0; //止损1
		m_EditStopLoss1.GetWindowText(str);
		UnicodeToUtf8(str, strTemp);
		dStopLoss1 = atof(strTemp.c_str());
		double dStopLoss2 = 0;//止损2
		m_EditStopLoss2.GetWindowText(str);
		UnicodeToUtf8(str, strTemp);
		dStopLoss2 = atof(strTemp.c_str());
		double dStopProfit1 = 0;//止盈1
		m_EditStopProfit1.GetWindowText(str);
		UnicodeToUtf8(str, strTemp);
		dStopProfit1 = atof(strTemp.c_str());
		double dStopProfit2 = 0;//止盈2
		m_EditStopProfit2.GetWindowText(str);
		UnicodeToUtf8(str, strTemp);
		dStopProfit2 = atof(strTemp.c_str());

		if (0 == iMerchType)
		{
			if (EET_BUY==m_EntrustType)//买
			{
				if (m_bCheckStopLoss)//止损勾选
				{
					if (dStopLoss1>=dStopLoss2)
					{
						strWarn = _T("止损价格不符合条件！");
						return -1;
					}
				}
				if (m_bCheckStopProfit)//止盈勾选
				{
					if (dStopProfit1<=dStopProfit2)
					{
						strWarn = _T("止盈价格不符合条件！");
						return -1;
					}
				}
			}
			else if (EET_SELL==m_EntrustType)//卖
			{
				if (m_bCheckStopLoss)//止损勾选
				{
					if (dStopLoss1<=dStopLoss2)
					{
						strWarn = _T("止损价格不符合条件！");
						return -1;
					}
				}
				if (m_bCheckStopProfit)//止盈勾选
				{
					if (dStopProfit1>=dStopProfit2)
					{
						strWarn = _T("止盈价格不符合条件！");
						return -1;
					}
				}
			}
			else
			{
			}
		}
		else
		{
			if (EET_BUY==m_EntrustType)//买
			{
				if (m_bCheckStopLoss)//止损勾选
				{
					if (dStopLoss1<=dStopLoss2)
					{
						strWarn = _T("止损价格不符合条件！");
						return -1;
					}
				}
				if (m_bCheckStopProfit)//止盈勾选
				{
					if (dStopProfit1>=dStopProfit2)
					{
						strWarn = _T("止盈价格不符合条件！");
						return -1;
					}
				}
			}
			else if (EET_SELL==m_EntrustType)//卖
			{
				if (m_bCheckStopLoss)//止损勾选
				{
					if (dStopLoss1>=dStopLoss2)
					{
						strWarn = _T("止损价格不符合条件！");
						return -1;
					}
				}
				if (m_bCheckStopProfit)//止盈勾选
				{
					if (dStopProfit1<=dStopProfit2)
					{
						strWarn = _T("止盈价格不符合条件！");
						return -1;
					}
				}
			}
			else
			{
			}
		}
		
		//判断保证金
		if ((m_dMarginLimit>m_dCurrentMargin) || (0>=iQty))
		{
			strWarn.Format(_T("可用保证金不足！"));
			return -1;
		}
	}

	return 0;
}

void CDlgTradeOpen::LoadTradeOpenInfo()
{
	CString StrV;
	
	m_bCheckAllowPoint = TRUE;
	if ( CEtcXmlConfig::Instance().ReadEtcConfig(KStrTradeOpenSection, KStrTradeOpenKeyPoint, NULL, StrV) )
	{
		m_bCheckAllowPoint = _ttoi(StrV)!=0;
	}
	if (m_bCheckAllowPoint)
	{
		m_CheckAllowPoint.SetImage(m_imgCheck);
	}
	else
	{
		m_CheckAllowPoint.SetImage(m_imgUncheck);
	}

	//界面显示
	if (m_bCheckAllowPoint)
	{
		m_EditPoint.EnableWindow(TRUE);
		m_SpinPoint.EnableWindow(TRUE);
	}
	else
	{
		m_EditPoint.EnableWindow(FALSE);
		m_SpinPoint.EnableWindow(FALSE);
	}
	
	m_bCheckBeforeOrder = TRUE;
	if ( CEtcXmlConfig::Instance().ReadEtcConfig(KStrTradeOpenSection, KStrTradeOpenKeyCheck, NULL, StrV) )
	{
		m_bCheckBeforeOrder = _ttoi(StrV)!=0;
	}
	if (m_bCheckBeforeOrder)
	{
		m_CheckBeforeOrder.SetImage(m_imgCheck);
	}
	else
	{
		m_CheckBeforeOrder.SetImage(m_imgUncheck);
	}
}

void CDlgTradeOpen::SaveTradeOpenInfo()
{
	CEtcXmlConfig::Instance().WriteEtcConfig(KStrTradeOpenSection, KStrTradeOpenKeyPoint, m_bCheckAllowPoint?_T("1"):_T("0"));
	
	CEtcXmlConfig::Instance().WriteEtcConfig(KStrTradeOpenSection, KStrTradeOpenKeyCheck, m_bCheckBeforeOrder?_T("1"):_T("0"));
}

void CDlgTradeOpen::TradeTypeChange(E_TradeType tradeType)
{
	m_eTradeType = tradeType;
	if ( ECT_Market ==tradeType)//市价
	{
		ShowHideControl_MarketPrice(SW_SHOW);
		ShowHideControl_LimitPrice(SW_HIDE);
		m_iBuySellSpace = 140;
		m_iBuyRadioOffset = 80;
		//if (0 == m_QuotationResultVector.size()) // 没行情时，休市
		{
			SetPrice();
		}
	}
	else if (ECT_Limit == tradeType)//限价
	{
		ShowHideControl_MarketPrice(SW_HIDE);
		ShowHideControl_LimitPrice(SW_SHOW);
		m_EditPrice.SetWindowText(_T("0"));//切换商品价格置0
		m_EditStopLoss1.SetWindowText(_T("0"));//切换商品价格置0
		m_EditStopProfit1.SetWindowText(_T("0"));//切换商品价格置0

		if (m_EntrustType == EET_BUY)
		{
			m_RadioBuy.SetCheckStatus(TRUE);
			m_RadioSell.SetCheckStatus(FALSE);
		}
		else if (m_EntrustType == EET_SELL)
		{
			m_RadioBuy.SetCheckStatus(FALSE);
			m_RadioSell.SetCheckStatus(TRUE);
		}


		m_iBuySellSpace = 20;
		m_iBuyRadioOffset = 40;
		m_dPrice = 0;//置0
		m_dStopProfit1 = 0;
		m_dStopLoss1 = 0;
		SetPrice();
		UpdataStopLossProfitShow();
	}
	else
	{
		//ASSERT(0);
		return;
	}

	
	RecalcLayout(true);
//	RedrawWindow();
}

void CDlgTradeOpen::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_BuyOK.PtInButton(point))
	{
		m_BuyOK.LButtonUp();
	}
	else if (m_SellOK.PtInButton(point))
	{
		m_SellOK.LButtonUp();
	}
	else if (m_RadioBuy.PtInButton(point))
	{
		m_RadioBuy.LButtonUp();
	}
	else if (m_RadioSell.PtInButton(point))
	{
		m_RadioSell.LButtonUp();
	}
	else if (m_TradeOk.PtInButton(point))
	{
		m_TradeOk.LButtonUp();
	}
	else if (m_BtnMax.PtInButton(point))
	{
		m_BtnMax.LButtonUp();
	}
	else if (m_CheckStopLoss.PtInButton(point))
	{
		m_CheckStopLoss.LButtonUp();
	}
	else if (m_CheckStopProfit.PtInButton(point))
	{
		m_CheckStopProfit.LButtonUp();
	}
	else if (m_CheckAllowPoint.PtInButton(point))
	{
		m_CheckAllowPoint.LButtonUp();
	}
	else if (m_CheckBeforeOrder.PtInButton(point))
	{
		m_CheckBeforeOrder.LButtonUp();
	}
}

void CDlgTradeOpen::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_BuyOK.PtInButton(point))
	{
		m_BuyOK.LButtonDown();
	}
	
	else if (m_SellOK.PtInButton(point))
	{
		m_SellOK.LButtonDown();
	}

	else if (m_RadioSell.PtInButton(point))
	{
		m_RadioSell.LButtonDown();
	}
	
	else if (m_RadioBuy.PtInButton(point))
	{
		m_RadioBuy.LButtonDown();
	}
	else if (m_TradeOk.PtInButton(point))
	{
		m_TradeOk.LButtonDown();
	}
	else if (m_BtnMax.PtInButton(point))
	{
		m_BtnMax.LButtonDown();
	}

	if (m_CheckStopLoss.PtInButton(point))
	{
		m_CheckStopLoss.LButtonDown();
	}
	if (m_CheckStopProfit.PtInButton(point))
	{
		m_CheckStopProfit.LButtonDown();
	}
	if (m_CheckAllowPoint.PtInButton(point))
	{
		m_CheckAllowPoint.LButtonDown();
	}
	if (m_CheckBeforeOrder.PtInButton(point))
	{
		m_CheckBeforeOrder.LButtonDown();
	}
}

void CDlgTradeOpen::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_BuyOK.PtInButton(point))
	{
		m_BuyOK.MouseHover();
	}
	else
	{
		m_BuyOK.MouseLeave();
	}

	if (m_SellOK.PtInButton(point))
	{
		m_SellOK.MouseHover();
	}
	else
	{
		m_SellOK.MouseLeave();
	}

	if (m_RadioSell.PtInButton(point))
	{
		m_RadioSell.MouseHover();
	}
	else
	{
		m_RadioSell.MouseLeave();
	}

	if (m_RadioBuy.PtInButton(point))
	{
		m_RadioBuy.MouseHover();
	}
	else
	{
		m_RadioBuy.MouseLeave();
	}
	if (m_TradeOk.PtInButton(point))
	{
		m_TradeOk.MouseHover();
	}
	else
	{
		m_TradeOk.MouseLeave();
	}

	if (m_BtnMax.PtInButton(point))
	{
		m_BtnMax.MouseHover();
	}
	else
	{
		m_BtnMax.MouseLeave();
	}

	if (m_CheckStopLoss.PtInButton(point))
	{
		m_CheckStopLoss.MouseHover();
	}
	else
	{
		m_CheckStopLoss.MouseLeave();
	}
	if (m_CheckStopProfit.PtInButton(point))
	{
		m_CheckStopProfit.MouseHover();
	}
	else
	{
		m_CheckStopProfit.MouseLeave();
	}
	if (m_CheckAllowPoint.PtInButton(point))
	{
		m_CheckAllowPoint.MouseHover();
	}
	else
	{
		m_CheckAllowPoint.MouseLeave();
	}
	if (m_CheckBeforeOrder.PtInButton(point))
	{
		m_CheckBeforeOrder.MouseHover();
	}
	else
	{
		m_CheckBeforeOrder.MouseLeave();
	}
}

BOOL CDlgTradeOpen::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int32 iID = (int32)wParam;
	if (iID == IDC_DEF_BUTTON_BUYOK)
	{
		m_EntrustType = EET_BUY;
		OnBtnOk();
	}
	else if (iID == IDC_DEF_BUTTON_SELLOK)
	{
		m_EntrustType = EET_SELL;
		OnBtnOk();
	}

	if (iID == IDC_DEF_BUTTON_TRADEOK)
	{
		OnBtnOk();
	}

	return CDialog::OnCommand(wParam,lParam);
}

int CDlgTradeOpen::OpenMaxVol()
{
	T_CommInfoType Infotype;
	if (!FindCommInfoType(m_DropCommInfo.GetCurSel(),Infotype))
	{
		return 0;
	}

	// 查对应商品信息
	T_TradeMerchInfo stOutComm;
	for (int i=0; i<m_QueryCommInfoResultVector.size(); i++)
	{
		stOutComm = m_QueryCommInfoResultVector[i];
		string strCode;
		UnicodeToUtf8(stOutComm.stock_code, strCode);
		if (_stricmp(Infotype.chTypeValue, strCode.c_str())==0)
		{			
			break;
		}
	}

	// 交易单位
	string strVal;
	UnicodeToUtf8(stOutComm.trade_unit, strVal);
	double dtradeUnit = atof(strVal.c_str());

	//交易价格
	double dPrice = 0;
	CString str = _T("");
	if (ECT_Market == m_eTradeType)
	{
		str = m_BuyOK.GetCaption();
		string sPrice;
		UnicodeToUtf8(str, sPrice);
		dPrice = atof(sPrice.c_str());
	}
	else
	{
		//获取价格框价格
		string sPrice;
		m_EditPrice.GetWindowText(str);
		UnicodeToUtf8(str, sPrice);
		dPrice = atof(sPrice.c_str());
	}

	//计算保证金
	double dMargin = 0, dFee = 0;
	if (dtradeUnit != 0 && stOutComm.margin_ratio != 0)
	{
		dFee =  m_pTradeQuery->m_dFeeScale * dPrice * dtradeUnit; 
		if (ECT_Market == m_eTradeType)
		{
			dMargin = dtradeUnit*stOutComm.margin_ratio*dPrice + dtradeUnit*stOutComm.buy_pdd*stOutComm.price_minchange;
		}
		else
		{
			dMargin = dtradeUnit*stOutComm.margin_ratio*dPrice;
		}
	}

	int iRet = 0;
	if ( dMargin>0 && m_dCurrentMargin>0)
	{
		double dFloat = m_dCurrentMargin;
		/*if (m_dCurrentMargin > m_dCurrentFund)
		{
			dFloat = m_dCurrentFund;
		}*/
		iRet = (int)(dFloat / (dMargin+dFee));
	}

	return iRet;
}