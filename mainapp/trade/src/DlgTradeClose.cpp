// Trade.cpp : implementation file
//

#include "stdafx.h"
#include "DlgTradeClose.h"
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
// CDlgTradeClose dialog
namespace
{
#define IDC_DEF_EDIT_COMMINFO		WM_USER+199
#define IDC_DEF_DROP_ENTRUSTTYPE	WM_USER+200
#define IDC_DEF_DROP_COMMINFO		WM_USER+201
#define IDC_DEF_DROP_HAND			WM_USER+202
#define IDC_DEF_EDIT_HAND			WM_USER+203
#define IDC_DEF_BUTTON_MAX			WM_USER+204
#define IDC_DEF_SPIN_MAX			WM_USER+205
#define IDC_DEF_DROP_TRADEID		WM_USER+206
#define IDC_DEF_BUTTON_BUYSELL		WM_USER+207
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
#define IDC_DEF_CHECK_ALLOWBACK		WM_USER+227
#define IDC_DEF_EDIT_BACK				WM_USER+228
#define IDC_DEF_SPIN_BACK			WM_USER+229
#define IDC_DEF_EDIT_DETAIL			WM_USER+230
	

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
#define	IDC_DEF_STATIC_BUYSELL		WM_USER+313
#define IDC_DEF_STATIC_DETAIL		WM_USER+314
	
#define IDC_DEF_STATIC_HAND_BK			WM_USER+330
#define IDC_DEF_STATIC_POINT_BK			WM_USER+331
#define IDC_DEF_STATIC_BACK_BK			WM_USER+332
#define IDC_DEF_STATIC_DETAIL_BK		WM_USER+333
#define IDC_DEF_STATIC_STOPLOSS1_BK		WM_USER+334
#define IDC_DEF_STATIC_STOPLOSS2_BK		WM_USER+335
#define IDC_DEF_STATIC_STOPPROFIT1_BK		WM_USER+336
#define IDC_DEF_STATIC_STOPPROFIT2_BK		WM_USER+337
#define IDC_DEF_STATIC_CHECKSTOPLOSS    WM_USER+350
#define IDC_DEF_STATIC_CHECKSTOPPROFIT  WM_USER+351
#define IDC_DEF_STATIC_CHECKALLOWPOINT  WM_USER+352
#define IDC_DEF_STATIC_CHECKBEFOREORDER WM_USER+353
#define IDC_DEF_STATIC_CHECKBACK		WM_USER+354

#define BK_COLOR_BUYPRICE			 RGB(174, 202, 239)//RGB(174, 202, 239)
#define BK_COLOR_SELLPRICE			 RGB(244, 200, 199)//RGB(0, 100, 0)
#define TEXT_COLOR_BUYPRICE			 RGB(0, 0, 255)
#define TEXT_COLOR_SELLPRICE		 RGB(255, 0, 0)
#define BK_COLOR_EDIT			     RGB(0xe6,0xe6,0xe6)
#define TEXT_COLOR_STATIC_TIP		 RGB(0x22,0x7c,0xed)
	
static const TCHAR		KStrTradeCloseSection[]		= _T("TradeClose");
static const TCHAR		KStrTradeCloseKeyPoint[]	= _T("point");
static const TCHAR		KStrTradeCloseKeyCheck[]	= _T("check");

#define INVALID_ID -1
}

CDlgTradeClose::CDlgTradeClose(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTradeClose::IDD, pParent)
{
	m_bCheckAllowPoint = FALSE;
	m_bCheckBack = FALSE;
	m_EntrustType = EET_BUY;
	m_bCheckBeforeOrder = FALSE;
	m_iQty = 0;
	m_iQtyMax = 0;
	m_iQtyMin = 0;
	m_iPoint = 0;
	m_iBack = 0;
	m_iOpenQtyMax = 0;
	m_iOpenQtyMin = 0;
	m_iPointMax = 100;
	m_iPointMin = 0;
	m_iPointDef = 50;
	m_pTradeLoginInfo = NULL;

	m_pTradeBid = NULL;
	
	m_bInitCommInfo = false;
	m_iCloseChannel = 0;
	m_dPriceStep = 0;
	m_iPointCount = 0;
	
	m_pTradeQuery = NULL;
	m_iStaticValidity = 0;
	
	m_bCheckStopLoss = FALSE;
	m_bCheckStopProfit = FALSE;
	m_pHoldDetailQueryOut = NULL;
	
	m_dStopLoss1 = 0;
	m_dStopProfit1 = 0;

	m_dBuyPrice = 0;
	m_dSellPrice = 0;
	iMerchType = 0;

	m_iXButtonHovering = INVALID_ID;

	m_pImgCheck = NULL;
	m_pImgUncheck = NULL;
	m_pImgMax = NULL;
	m_pImgSell = NULL;
	m_pImgBuy = NULL;
	m_pImgOrder = NULL;
	m_bInitImg = false;

	//{{AFX_DATA_INIT(CTrade)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDlgTradeClose::~CDlgTradeClose()
{
	DEL(m_pImgCheck);
	DEL(m_pImgUncheck);
	DEL(m_pImgMax);
	DEL(m_pImgSell);
	DEL(m_pImgBuy);
	DEL(m_pImgOrder);
}

void CDlgTradeClose::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrade)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTradeClose, CDialog)
	//{{AFX_MSG_MAP(CTrade)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_NOTIFY(UDN_DELTAPOS, IDC_DEF_SPIN_MAX, OnSpinBtnHand)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DEF_SPIN_POINT, OnSpinBtnPoint)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DEF_SPIN_BACK, OnSpinBtnBack)
	ON_BN_CLICKED(IDC_DEF_CHECK_ALLOWPOINT, OnBnCheckAllowPoint)
	ON_BN_CLICKED(IDC_DEF_CHECK_ALLOWBACK, OnBnCheckAllowBack)
	ON_BN_CLICKED(IDC_DEF_CHECK_BEFOREORDER, OnBnCheckBeforeOrder)
	ON_BN_CLICKED(IDC_DEF_BUTTON_MAX,OnBtnMax)
	ON_CBN_SELCHANGE(IDC_DEF_DROP_COMMINFO,OnBtnDropCommInfo)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave) 
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_EN_SETFOCUS(IDC_DEF_EDIT_HAND, OnSetFocusEditHand)
	ON_EN_KILLFOCUS(IDC_DEF_EDIT_HAND, OnEnKillFocusEditHand)
	ON_BN_CLICKED(IDC_DEF_CHECK_STOPLOSS, OnBnCheckStopLoss)
	ON_BN_CLICKED(IDC_DEF_CHECK_STOPPROFIT, OnBnCheckStopProfit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DEF_SPIN_STOPLOSS, OnSpinBtnStopLoss)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DEF_SPIN_STOPPROFIT, OnSpinBtnStopProfit)
	ON_EN_CHANGE(IDC_DEF_EDIT_POINT, OnEnChangeEditPoint)
	ON_EN_CHANGE(IDC_DEF_EDIT_STOPLOSS1, OnEnChangeEditStopLoss1)
	ON_EN_CHANGE(IDC_DEF_EDIT_STOPPROFIT1, OnEnChangeEditStopProfit1)
	ON_EN_KILLFOCUS(IDC_DEF_EDIT_STOPLOSS1, OnEnKillFocusEditStopLoss1)
	ON_EN_KILLFOCUS(IDC_DEF_EDIT_STOPPROFIT1, OnEnKillFocusEditStopProfit1)
	ON_EN_SETFOCUS(IDC_DEF_EDIT_POINT, OnSetFocusEditPoint)
	ON_EN_SETFOCUS(IDC_DEF_EDIT_BACK, OnSetFocusEditBack)
	ON_EN_SETFOCUS(IDC_DEF_EDIT_PRICEMARKETPRICE, OnSetFocusPrice)
	ON_EN_SETFOCUS(IDC_DEF_EDIT_STOPLOSS2, OnSetFocusEditStopLoss2)
	ON_EN_SETFOCUS(IDC_DEF_EDIT_STOPPROFIT2, OnSetFocusEditStopProfit2)
//	ON_NOTIFY(UDN_DELTAPOS, IDC_DEF_EDIT_, OnSpinBtnHand)
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrade message handlers

BOOL CDlgTradeClose::OnInitDialog()
{
	CDialog::OnInitDialog();
	// �����������ʼ��ͼ����Դ��������OnSize�����г�ʼ������Ϊ
	// OnSize�е�����RecalcLayout�������ú������õ���ͼ�񣬶�OnSize�״�ִ��ʱ����OnInitDialog
	// ������ǰ�棬��ʱͼ���������ʼ����������
	//InitImgResource();
	CreateControl();	// �����ؼ�
	InitControl();
	RecalcLayout(false);	// ���ؼ�
	LoadTradeCloseInfo();

	return TRUE;
}

void CDlgTradeClose::InitImgResource()
{
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TRADE_CHECK, L"PNG", m_pImgCheck))
	{
		m_pImgCheck = NULL;
	}
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TRADE_UCHECK, L"PNG", m_pImgUncheck))
	{
		m_pImgUncheck = NULL;
	}
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_TRADE_MAX, L"PNG", m_pImgMax))
	{
		m_pImgMax = NULL;
	}
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_RADIOSELL, L"PNG", m_pImgSell))
	{
		m_pImgSell = NULL;
	}
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_RADIOBUY, L"PNG", m_pImgBuy))
	{
		m_pImgBuy = NULL;
	}
 	//if ( !ImageFromIDResource(IDR_PNG_TRADE_UCHECK, L"PNG", m_pImgOrder))
 	//{
 	//	m_pImgOrder = NULL;
 	//}
}

void CDlgTradeClose::CreateControl()
{
	CString strFontName = gFontFactory.GetExistFontName(L"΢���ź�");	//...
	
	//COLORREF colorDef = RGB(0x4d, 0x4d, 0x4d);
	//COLORREF colorTipDef = RGB(0x22, 0x7c, 0xed);
	
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
	lgFontBig.lfHeight = 20;
	
	LOGFONT	lgFontEditSmall = lgFont;
	LOGFONT	lgFontEditBig = lgFont;
	lgFontEditBig.lfHeight = 36;
	
	LOGFONT	lgFontEditMid = lgFont;
	lgFontEditMid.lfHeight = 18;
	
	m_fontSmall.CreateFontIndirect(&lgFont);
	m_fontBig.CreateFontIndirect(&lgFontBig);
	m_fontEditSmall.CreateFontIndirect(&lgFontEditSmall);
	m_fontEditBig.CreateFontIndirect(&lgFontEditBig);
	m_fontEditMid.CreateFontIndirect(&lgFontEditMid);	

	// ��Ʒ
	m_DropCommInfo.Create( WS_CHILD|CBS_DROPDOWNLIST, CRect(0, 0, 0, 200), this, IDC_DEF_DROP_COMMINFO);
	m_DropCommInfo.SetFont(&m_fontSmall);
	m_EditCommInfo.Create(ES_MULTILINE|ES_AUTOHSCROLL|WS_VISIBLE|WS_DISABLED, CRect(0, 0, 0, 200), this, IDC_DEF_EDIT_COMMINFO);
	m_EditCommInfo.SetFont(&m_fontSmall);
	m_StaticCommInfo.Create(L"��Ʒ:", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_COMMINFO);
	m_StaticCommInfo.ShowWindow(SW_SHOW);
	m_StaticCommInfo.SetFont(&m_fontSmall);
	// ����
	m_StaticBuySell.Create(L"����:", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_BUYSELL);
	m_StaticBuySell.ShowWindow(SW_SHOW);
	m_StaticBuySell.SetFont(&m_fontSmall);
	// ����
	m_DropHand.Create( WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST, CRect(0, 0, 0, 200), this, IDC_DEF_DROP_HAND);
	m_DropHand.ShowWindow(SW_SHOW);
	m_DropHand.SetFont(&m_fontSmall);
	m_StaticHand.Create(L"����:", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_HAND);
	m_StaticHand.ShowWindow(SW_SHOW);
	m_StaticHand.SetFont(&m_fontSmall);
	m_StaticBKHand.Create(L"", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_HAND_BK);
	m_StaticBKHand.ShowWindow(SW_SHOW);
	m_EditHand.Create(ES_MULTILINE|ES_AUTOHSCROLL|ES_NUMBER|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_EDIT_HAND);
	m_EditHand.ShowWindow(SW_SHOW);
	m_EditHand.SetFont(&m_fontSmall);
	m_SpinHand.Create(WS_CHILD|WS_VISIBLE|UDS_SETBUDDYINT,CRect(0, 0, 0, 0), this, IDC_DEF_SPIN_MAX);
	m_SpinHand.ShowWindow(SW_SHOW);
	m_StaticDetail.Create(L"��ϸ:", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_DETAIL);
	m_StaticDetail.SetFont(&m_fontSmall);
	m_EditDetail.Create(L"", WS_CHILD|WS_VISIBLE|SS_LEFT, CRect(0, 0, 0, 0), this, IDC_DEF_EDIT_DETAIL);
	m_EditDetail.SetFont(&m_fontSmall);

	m_StaticBKPoint.Create(L"", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_POINT_BK);
	m_StaticBKPoint.ShowWindow(SW_SHOW);
	m_EditPoint.Create(ES_MULTILINE|ES_AUTOHSCROLL|ES_NUMBER|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_EDIT_POINT);
	m_EditPoint.ShowWindow(SW_SHOW);
	m_EditPoint.SetFont(&m_fontSmall);
	m_SpinPoint.Create(WS_CHILD|WS_VISIBLE|UDS_SETBUDDYINT,CRect(0, 0, 0, 0), this, IDC_DEF_SPIN_POINT);
 	m_SpinPoint.ShowWindow(SW_SHOW);
	
	m_StaticBKBack.Create(L"", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_BACK_BK);
	m_StaticBKBack.ShowWindow(SW_SHOW);
	m_EditBack.Create(ES_MULTILINE|ES_AUTOHSCROLL|ES_NUMBER|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_EDIT_BACK);
	m_EditBack.ShowWindow(SW_SHOW);
	m_EditBack.SetFont(&m_fontSmall);
	m_SpinBack.Create(WS_CHILD|WS_VISIBLE|UDS_SETBUDDYINT,CRect(0, 0, 0, 0), this, IDC_DEF_SPIN_BACK);
	m_SpinBack.ShowWindow(SW_SHOW);

	//ָ��ƽ��
	m_StaticBKStopLoss1.Create(L"", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_STOPLOSS1_BK);
	m_StaticBKStopLoss1.ShowWindow(SW_SHOW);
	m_StaticBKStopLoss2.Create(L"", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_STOPLOSS2_BK);
	m_StaticBKStopLoss2.ShowWindow(SW_SHOW);
	m_EditStopLoss1.Create(ES_MULTILINE|ES_AUTOHSCROLL|WS_CHILD|WS_VISIBLE/*|WS_BORDER|ES_RIGHT*/|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_EDIT_STOPLOSS1);
	m_EditStopLoss1.SetFont(&m_fontSmall);
	m_EditStopLoss2.Create(ES_MULTILINE|ES_AUTOHSCROLL|WS_CHILD|/*ES_CENTER|*/WS_DISABLED|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_EDIT_STOPLOSS2);
	m_EditStopLoss2.SetFont(&m_fontSmall);
	m_SpinStopLoss.Create(WS_CHILD|WS_VISIBLE|UDS_SETBUDDYINT,CRect(0, 0, 0, 0), this, IDC_DEF_SPIN_STOPLOSS);
	m_SpinStopLoss.ShowWindow(SW_SHOW);
	m_StaticStopLoss.Create(L"<", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_STOPLOSS);
	m_StaticStopLoss.ShowWindow(SW_SHOW);
	m_StaticStopLoss.SetFont(&m_fontSmall);
	
	m_StaticBKStopProfit1.Create(L"", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_STOPPROFIT1_BK);
	m_StaticBKStopProfit1.ShowWindow(SW_SHOW);
	m_StaticBKStopProfit2.Create(L"", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_STOPPROFIT2_BK);
	m_StaticBKStopProfit2.ShowWindow(SW_SHOW);
	m_EditStopProfit1.Create(ES_MULTILINE|ES_AUTOHSCROLL|WS_CHILD|WS_VISIBLE/*|WS_BORDER*//*|ES_RIGHT*/|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_EDIT_STOPPROFIT1);
	m_EditStopProfit1.SetFont(&m_fontSmall);
	m_EditStopProfit2.Create(ES_MULTILINE|ES_AUTOHSCROLL|WS_CHILD/*|ES_CENTER*/|WS_DISABLED|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_EDIT_STOPPROFIT2);
	m_EditStopProfit2.SetFont(&m_fontSmall);
	m_SpinStopProfit.Create(WS_CHILD|WS_VISIBLE|UDS_SETBUDDYINT,CRect(0, 0, 0, 0), this, IDC_DEF_SPIN_STOPPROFIT);
	m_SpinStopProfit.ShowWindow(SW_SHOW);
	m_StaticStopProfit.Create(L">", WS_CHILD|WS_VISIBLE|SS_CENTER, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_STOPPROFIT);
	m_StaticStopProfit.ShowWindow(SW_SHOW);
	m_StaticStopProfit.SetFont(&m_fontSmall);

	m_StaticTip.Create(L"", WS_CHILD|WS_VISIBLE|SS_LEFT, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_TIP);
	m_StaticTip.ShowWindow(SW_SHOW);
 	m_StaticTip.SetFont(&m_fontSmall);
	
	m_StaticAfterOk.Create(L"���ڴ������Ժ�......", WS_CHILD|WS_VISIBLE|SS_LEFT, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_AFTEROK);
	m_StaticAfterOk.ShowWindow(SW_HIDE);
	m_StaticAfterOk.SetFont(&m_fontSmall);

	m_StaticCheckStopLoss.Create(L"ֹ��", WS_CHILD|WS_VISIBLE|SS_LEFT, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_CHECKSTOPLOSS);
	m_StaticCheckStopLoss.SetFont(&m_fontSmall);
	m_StaticCheckStopProfit.Create(L"ֹӯ", WS_CHILD|WS_VISIBLE|SS_LEFT, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_CHECKSTOPPROFIT);
	m_StaticCheckStopProfit.SetFont(&m_fontSmall);
	m_StaticCheckAllowPoint.Create(L"������", WS_CHILD|WS_VISIBLE|SS_LEFT, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_CHECKALLOWPOINT);
	m_StaticCheckAllowPoint.SetFont(&m_fontSmall);
	m_StaticCheckBeforeOrder.Create(L"�µ�ǰȷ��", WS_CHILD|WS_VISIBLE|SS_LEFT, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_CHECKBEFOREORDER);
	m_StaticCheckBeforeOrder.SetFont(&m_fontSmall);
	m_StaticCheckBack.Create(L"���ֽ���", WS_CHILD|WS_VISIBLE|SS_LEFT, CRect(0, 0, 0, 0), this, IDC_DEF_STATIC_CHECKBACK);
	m_StaticCheckBack.SetFont(&m_fontSmall);
}

void CDlgTradeClose::InitControl()
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
	
	this->SetWindowText(L"ƽ�ֵ�");
	
	// �����
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
	// ���ֽ���
	if (m_bCheckBack)
	{
		m_EditBack.EnableWindow(TRUE);
		m_SpinBack.EnableWindow(TRUE);
	}
	else
	{
		m_EditBack.EnableWindow(FALSE);
		m_SpinBack.EnableWindow(FALSE);
	}
	
	// ��������
	m_eCloseType = ECT_Market;
	ShowHideControl_LimitPrice(SW_HIDE);
	ShowHideControl_MarketPrice(SW_SHOW);
	m_iStaticValidity = 0;
	
	m_EntrustType = EET_BUY;
	m_EditHand.SetWindowText(L"0");
	m_EditPoint.SetWindowText(L"50");
	m_EditBack.SetWindowText(L"1");

	// �޼�ί�в���
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

	// ���������Ϳ��Χ��������Ʒ����������ȷ����
	SetPrice();
	SetHandRange();
	
	UpdateData(FALSE);
}

// ����ؼ���ʼ��
void CDlgTradeClose::ResetInitControl(E_TradeType eType)
{
	if (NULL==m_pHoldDetailQueryOut)
	{
		if (ECT_Market == m_eCloseType)
		{
			m_mapButton[IDC_DEF_EDIT_PRICEMARKETPRICE].SetCaption(L"ƽ��");
		}
		else
		{
			m_mapButton[IDC_DEF_EDIT_PRICEMARKETPRICE].SetCaption(L"�µ�");
		}
		m_mapButton[IDC_DEF_EDIT_PRICEMARKETPRICE].EnableButton(FALSE,TRUE);
		m_mapButton[IDC_DEF_BUTTON_MAX].EnableButton(FALSE,TRUE);

		m_DropCommInfo.SetCurSel(-1);
		m_EditCommInfo.SetWindowText(L"");
		m_mapButton[IDC_DEF_BUTTON_BUYSELL].SetCaption(L"����");
		m_EditDetail.SetWindowText(L"");	
		m_EditHand.SetWindowText(L"0");
		m_EditPoint.SetWindowText(L"0");
		m_EditBack.SetWindowText(L"0");
		m_EditStopLoss2.SetWindowText(L"0");
		m_EditStopProfit2.SetWindowText(L"0");
		m_EditStopProfit1.SetWindowText(_T("0"));
		m_EditStopLoss1.SetWindowText(_T("0"));
		return;
	}

	m_mapButton[IDC_DEF_EDIT_PRICEMARKETPRICE].EnableButton(TRUE);

	string strCode;
	UnicodeToUtf8(m_pHoldDetailQueryOut->stock_code, strCode);
	//������Ʒ����
	m_DropCommInfo.SetCurSel(GetCommIndex(strCode.c_str()));
	CString strName = _T("");
 	m_DropCommInfo.GetWindowText(strName);
	m_DropCommInfo.EnableWindow(FALSE);
	m_EditCommInfo.SetWindowText(strName);
	//������������
	CString strBuySell = _T("");		 
	if (_T("B")==m_pHoldDetailQueryOut->entrust_bs)//��
	{
		strBuySell = _T("����");
		m_EntrustType = EET_SELL;
		m_StaticStopLoss.SetWindowText(_T("<"));
		m_StaticStopProfit.SetWindowText(_T(">"));
		m_mapButton[IDC_DEF_BUTTON_BUYSELL].SetTextColor(RGB(0x2e,0x99,0x2e), RGB(0x2e,0x99,0x2e), RGB(0x2e,0x99,0x2e));
		m_mapButton[IDC_DEF_BUTTON_BUYSELL].SetImage(m_pImgSell);
	}
	else/* if (2==m_pHoldDetailQueryOut->buySell)*///��
	{
		strBuySell = _T("����");
		m_EntrustType = EET_BUY;	
		m_StaticStopLoss.SetWindowText(_T(">"));
		m_StaticStopProfit.SetWindowText(_T("<"));
		m_mapButton[IDC_DEF_BUTTON_BUYSELL].SetTextColor(RGB(255,0,0), RGB(255,0,0), RGB(255,0,0));
		m_mapButton[IDC_DEF_BUTTON_BUYSELL].SetImage(m_pImgBuy);
	}
	m_mapButton[IDC_DEF_BUTTON_BUYSELL].SetCaption(strBuySell);

	//����
	CString strQty = _T("");
	strQty.Format(_T("%ld"),m_pHoldDetailQueryOut->hold_amount);
	m_EditHand.SetWindowText(strQty);
	//��ϸ
	CString strDetail = m_pHoldDetailQueryOut->hold_id;
	strDetail += _T(" ");
	strDetail += strBuySell;
	strDetail += _T(" ");
	strDetail += strName;
	m_EditDetail.SetWindowText(strDetail);		 

	// ��ʼ���ؼ�
	m_EditHand.SetWindowText(L"1");
	m_EditPoint.SetWindowText(L"50");
	m_EditBack.SetWindowText(L"1");
	//�м�
	m_bCheckAllowPoint = FALSE;
//	m_mapButton[IDC_DEF_CHECK_ALLOWPOINT].SetImage(m_pImgUncheck);
	m_EditPoint.EnableWindow(FALSE);
	m_SpinPoint.EnableWindow(FALSE);
	m_bCheckBack = FALSE;
//	m_mapButton[IDC_DEF_CHECK_ALLOWBACK].SetImage(m_pImgUncheck);
	m_EditBack.EnableWindow(FALSE);
	m_SpinBack.EnableWindow(FALSE);
	m_bCheckBeforeOrder = FALSE;
//	m_mapButton[IDC_DEF_CHECK_BEFOREORDER].SetImage(m_pImgUncheck);

	//ָ��
	m_bCheckStopProfit = FALSE;
	m_EditStopProfit1.EnableWindow(FALSE);
	m_EditStopProfit2.EnableWindow(FALSE);
	m_SpinStopProfit.EnableWindow(FALSE);
	m_bCheckStopLoss = FALSE;
	m_EditStopLoss1.EnableWindow(FALSE);
	m_EditStopLoss2.EnableWindow(FALSE);
	m_SpinStopLoss.EnableWindow(FALSE);

	m_dStopProfit1 = m_pHoldDetailQueryOut->stop_profit;
	m_dStopLoss1 = m_pHoldDetailQueryOut->stop_loss;
	CString strTmp;
	strTmp.Format(_T("%.2f"), m_dStopLoss1);
	m_EditStopLoss1.SetWindowText(strTmp);
	strTmp.Format(_T("%.2f"), m_dStopProfit1);
	m_EditStopProfit1.SetWindowText(strTmp);

	map<int, CNCButton>::iterator iter;
	for (iter=m_mapButton.begin(); iter!=m_mapButton.end(); ++iter)
	{
		if (IDC_DEF_CHECK_STOPPROFIT == iter->first)
		{
			iter->second.SetImage(m_pImgUncheck);
		}
		
		if (IDC_DEF_CHECK_STOPLOSS == iter->first)
		{
			iter->second.SetImage(m_pImgUncheck);
		}
	}
	
	SetPrice();	//��Ʒ�л�֪ͨ�ļ۸�
	
	if (ECT_Limit==eType)//ָ��
	{
		ShowHideControl_MarketPrice(SW_HIDE);
		ShowHideControl_LimitPrice(SW_SHOW);
	
		m_EditHand.EnableWindow(FALSE);
		m_SpinHand.EnableWindow(FALSE);
		m_mapButton[IDC_DEF_BUTTON_MAX].EnableButton(FALSE);
		m_iStaticValidity = 30;
	}
	else
	{
		ShowHideControl_LimitPrice(SW_HIDE);
		ShowHideControl_MarketPrice(SW_SHOW);
	
		m_EditHand.EnableWindow(TRUE);
		m_SpinHand.EnableWindow(TRUE);
		m_mapButton[IDC_DEF_BUTTON_MAX].EnableButton(TRUE);
		m_iStaticValidity = 0;
	}

	SetHandRange(); //���÷�Χ
	LoadTradeCloseInfo();

	if (m_bCheckBack)
	{
		m_mapButton[IDC_DEF_CHECK_ALLOWBACK].SetImage(m_pImgCheck);
		m_EditBack.EnableWindow(TRUE);
		m_SpinBack.EnableWindow(TRUE);
	}
	else
	{
		m_mapButton[IDC_DEF_CHECK_ALLOWBACK].SetImage(m_pImgUncheck);
		m_EditBack.EnableWindow(FALSE);
		m_EditBack.EnableWindow(FALSE);
	}

	UpdateData(FALSE);
//	PostMessage(WM_SIZE,0,0);
}

// ���ü۸�
void CDlgTradeClose::SetPrice()
{
	T_CommInfoType Infotype;
	if (!FindCommInfoType(m_DropCommInfo.GetCurSel(),Infotype))
	{
	//	//ASSERT(0);
		return;
	}

	unsigned i = 0;
	int iPointCount = 0;
	T_TradeMerchInfo stOutComm;
	for (i=0;i<m_QueryCommInfoResultVector.size();i++)// ����Ʒ��Ϣ���˼۸񲨶�
	{
		stOutComm = m_QueryCommInfoResultVector[i];
		string strComm;
		UnicodeToUtf8(stOutComm.stock_code, strComm);
		if (_stricmp(Infotype.chTypeValue, strComm.c_str())==0) //����ʹ��붼��ͬ
		{
			m_iOpenQtyMax = stOutComm.max_entrust_amount; // �������ί��
			m_iOpenQtyMin = stOutComm.min_entrust_amount;// ������Сί��
			
// 			m_iPointMax = stOutComm.max_pdd;//�û����۵�����ֵ
// 			m_iPointMin = stOutComm.min_pdd;//�û����۵����Сֵ
			if (0 < stOutComm.default_pdd)
			{
				m_iPointDef = (int)(stOutComm.default_pdd);//�û����۵��Ĭ��ֵ
			}

			m_dPriceStep = stOutComm.price_minchange;	// ��С�䶯��λ
			iPointCount = GetPointCount(stOutComm.price_minchange);			
			m_iPointCount = iPointCount;	//С���������λ

			break;
		}
	}

	double dBuyPrice = 0;
	double dSellPrice = 0;
	for (i=0;i<m_QuotationResultVector.size();i++)// ���������
	{
		const T_RespQuote stOut = m_QuotationResultVector[i];
		string strCode;
		UnicodeToUtf8(stOut.stock_code, strCode);
		if (_stricmp(Infotype.chTypeValue, strCode.c_str())==0) //��Ʒ������ͬ
		{
			if (ETT_TRADE_SIMULATE == m_pTradeLoginInfo->eTradeLoginType)
			{
				dBuyPrice = stOut.buy_price;
				dSellPrice = stOut.sell_price;
			}
			else
			{
				if (_T("1") == stOutComm.price_mode)
				{
					dBuyPrice = stOut.last_price + stOutComm.buy_pdd*stOutComm.price_minchange;
					dSellPrice = stOut.last_price + stOutComm.sell_pdd*stOutComm.price_minchange;
				}
				else
				{
					dBuyPrice = stOut.buy_price + stOutComm.buy_pdd*stOutComm.price_minchange;
					dSellPrice = stOut.sell_price + stOutComm.sell_pdd*stOutComm.price_minchange;
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

	m_dBuyPrice = dBuyPrice;
	m_dSellPrice = dSellPrice;
	CString strPrice = _T(" ");
	CString strBuySell = _T(" ");
	if (EET_BUY==m_EntrustType) // ��
	{
		strPrice = Float2String(dBuyPrice, iPointCount);
		//	strPrice.Format(_T("%.2f"),stOut.buyPrice);
		strBuySell = _T("�� ");
	}
	else
	{// ��
		strPrice = Float2String(dSellPrice, iPointCount);
		strBuySell = _T("�� ");
		//strPrice.Format(_T("%.2f"),stOut.sellPrice);
	}
	
	if (ECT_Market == m_eCloseType)
	{
		m_mapButton[IDC_DEF_EDIT_PRICEMARKETPRICE].SetCaption(L"ƽ�� "+strPrice);
	}
	else
	{
		m_mapButton[IDC_DEF_BUTTON_BUYSELL].SetCaption(strBuySell+strPrice);
	}

	//�۸�䶯��ֹӯֹ����ű䶯
	UpdataStopLossProfitShow();
}

// ����������Χ
void CDlgTradeClose::SetHandRange()
{
	//Ŀǰֻ�� ResetInitControl�е���
	if (NULL==m_pHoldDetailQueryOut)
	{
		return;
	}
	m_iQtyMax = 0;
	m_iQtyMax = m_pHoldDetailQueryOut->hold_amount;
	m_iQtyMin=(m_iQtyMax>0)?1:0;

	CString str = _T("");
	str.Format(_T("%d"),m_iQtyMax);
	m_EditHand.SetWindowText(str);
}

bool CDlgTradeClose::FindCommInfoType( int iIndex, T_CommInfoType &commInfoType)
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

int CDlgTradeClose::GetCommIndex(const char* pCommID)
{
	if (NULL==pCommID)
	{
		//ASSERT(0);
		return 0;
	}

	int iIndex = 0;	//��¼����

	m_LockCommInfoType.lock();
	for (unsigned int i=0;i<m_CommInfoType.size();i++)
	{
		T_CommInfoType type = m_CommInfoType[i];
		if (_stricmp(pCommID,type.chTypeValue)==0)//�Ƚϴ���
		{
			iIndex = type.iIndex;
			break;
		}
		memset(&type,0,sizeof(T_CommInfoType));
	}
	m_LockCommInfoType.unlock();

 	return iIndex;
}

void CDlgTradeClose::RecalcLayout( bool bNeedDraw )
{
	const int iSpace = 10;	// �ؼ���Ի������ϡ��Ҽ��
	const int iStaticHeight = 30;
	const int iStaticWidth = 35;
	const int iStaticVSpace = 2;
	const int iStaticDropSpace = 5;	// �ؼ���static֮�����
	const int iEditHeight = 30;
	const int iButtonHeight = 30;
	const int iCheckWidth = 18;
	const int iCheckHeight = 18;

	CRect rc(0,0,0,0);
	GetClientRect(rc);
	m_mapButton.clear();
	
	// ��Ʒ
	CRect rectStaticCommInfo(rc);
	rectStaticCommInfo.left = rc.left + iSpace;
	rectStaticCommInfo.top = rc.top + iSpace / 2;
	rectStaticCommInfo.right = rectStaticCommInfo.left + iStaticWidth;
	rectStaticCommInfo.bottom = rectStaticCommInfo.top + iStaticHeight;
	if (m_StaticCommInfo.m_hWnd)
	{
		m_StaticCommInfo.MoveWindow(rectStaticCommInfo);
	}
	
	CRect rectDropCommInfo(rc);
	rectDropCommInfo.left = rectStaticCommInfo.right + iStaticDropSpace;
	rectDropCommInfo.top = rectStaticCommInfo.top - 2;
	rectDropCommInfo.right = rc.right - iSpace;
	rectDropCommInfo.bottom = rectDropCommInfo.top + iEditHeight;
	if (m_DropCommInfo.m_hWnd)
	{
		m_DropCommInfo.MoveWindow(rectDropCommInfo);
		m_EditCommInfo.MoveWindow(rectDropCommInfo);
		CRect rcTmpRt;
		m_EditCommInfo.GetClientRect(&rcTmpRt);
		rcTmpRt.DeflateRect(0,3,0,3);
		m_EditCommInfo.SetRectNP(&rcTmpRt);
	}

	if (ECT_Market == m_eCloseType) // �м�
	{
		// ����
		CRect rectStaticBuySell(rectStaticCommInfo);
		rectStaticBuySell.left = rectStaticCommInfo.left;
		rectStaticBuySell.top = rectStaticCommInfo.bottom + iStaticVSpace;
		rectStaticBuySell.right = rectStaticBuySell.left + iStaticWidth;
		rectStaticBuySell.bottom = rectStaticBuySell.top + iStaticHeight;
		if(m_StaticBuySell.m_hWnd)
		{
			m_StaticBuySell.MoveWindow(rectStaticBuySell);
		}
		
		CRect rectBuySell(rectStaticBuySell);
		rectBuySell.left = rectStaticBuySell.right + iStaticDropSpace;
		rectBuySell.top = rectDropCommInfo.bottom + iStaticVSpace;
		rectBuySell.right = rectBuySell.left + 125;
		rectBuySell.bottom = rectBuySell.top + 30;
		CString strBuySell = _T(" ");
		if (EET_SELL == m_EntrustType)
		{
			strBuySell = _T("����");
			AddButton(&rectBuySell, m_pImgSell, 4, IDC_DEF_BUTTON_BUYSELL, strBuySell);
		}
		else
		{
			strBuySell = _T("����");
			AddButton(&rectBuySell, m_pImgBuy, 4, IDC_DEF_BUTTON_BUYSELL, strBuySell);
		}

		// ����
		CRect rectStaticHand(rectStaticBuySell);
		rectStaticHand.left = rectStaticBuySell.left;
		rectStaticHand.top = rectStaticBuySell.bottom+iStaticVSpace + 5;
		rectStaticHand.right = rectStaticHand.left+iStaticWidth;
		rectStaticHand.bottom = rectStaticHand.top+iStaticHeight;
		if (m_StaticHand.m_hWnd)
		{
			m_StaticHand.MoveWindow(rectStaticHand);
		}

		const int iEdidHandWidth = 60;
		//const int iSpinHandWidth = 15;
		CRect rectEditHand(rectStaticHand);
		rectEditHand.left = rectStaticHand.right+iStaticDropSpace;
		rectEditHand.top = rectStaticHand.top - 5;
		rectEditHand.right = rectEditHand.left+iEdidHandWidth;
		rectEditHand.bottom = rectEditHand.top+iEditHeight;
		if (m_EditHand.m_hWnd)
		{
			m_EditHand.MoveWindow(rectEditHand);
			CRect rtRect;
			m_EditHand.GetClientRect(&rtRect);
			rtRect.DeflateRect(0,3,0,3);
			m_EditHand.SetRectNP(&rtRect);
		}
		CRect rectSpinHand(rectEditHand);
		rectSpinHand.top = rectEditHand.top + 1;
		rectSpinHand.bottom = rectSpinHand.top + 28;
		rectSpinHand.left = rectEditHand.right;
		rectSpinHand.right = rectSpinHand.left+15;
		if (m_SpinHand.m_hWnd)
		{
			m_SpinHand.MoveWindow(rectSpinHand);
		}
		CRect rectBtnMax(rectEditHand);
		rectBtnMax.left = rectSpinHand.right+5;
		rectBtnMax.top = rectEditHand.top;
		rectBtnMax.right = rectBtnMax.left+50;
		rectBtnMax.bottom =rectBtnMax.top+iButtonHeight;

		AddButton(&rectBtnMax, m_pImgMax, 2, IDC_DEF_BUTTON_MAX, L"���");
		if (NULL==m_pHoldDetailQueryOut)
		{
			m_mapButton[IDC_DEF_BUTTON_MAX].EnableButton(FALSE,TRUE);
		}

		CRect rectStaticBKHand(rectEditHand);
		rectStaticBKHand.left = rectEditHand.left-1;
		rectStaticBKHand.top = rectEditHand.top-1;
		rectStaticBKHand.right = rectEditHand.right +1;
		rectStaticBKHand.bottom = rectEditHand.bottom+1;
		if (m_StaticBKHand.m_hWnd)
		{
			m_StaticBKHand.MoveWindow(rectStaticBKHand);
		}

		//��ϸ
		CRect rectStaticDetail(rectStaticHand);
		rectStaticDetail.left = rectStaticHand.left;
		rectStaticDetail.top = rectStaticHand.bottom+iStaticVSpace;
		rectStaticDetail.right = rectStaticDetail.left+iStaticWidth;
		rectStaticDetail.bottom = rectStaticDetail.top+iStaticHeight;
		if (m_StaticDetail.m_hWnd)
		{
			m_StaticDetail.MoveWindow(rectStaticDetail);
		}
		
		CRect rectEditDetail(rectStaticDetail);
		rectEditDetail.left = rectStaticDetail.right + iStaticDropSpace;
		rectEditDetail.top = rectStaticDetail.top;
		rectEditDetail.right = rectDropCommInfo.right;
		rectEditDetail.bottom = rectEditDetail.top + iStaticHeight;
		if (m_EditDetail.m_hWnd)
		{
			m_EditDetail.MoveWindow(rectEditDetail);
		}

		// �м�ƽ��
		CRect rectEditPriceMarketPrice(rc);
		int iPriceEditWidth2 = rc.right - rc.left - iSpace * 2;
		rectEditPriceMarketPrice.left = rc.left + iSpace;
		rectEditPriceMarketPrice.top = rectEditDetail.bottom + 1;
		rectEditPriceMarketPrice.right = rectEditPriceMarketPrice.left + iPriceEditWidth2;
		rectEditPriceMarketPrice.bottom = rectEditPriceMarketPrice.top + 30;
		CString strPrice = _T(" ");
		if (EET_BUY==m_EntrustType) // ��
		{
			strPrice = Float2String(m_dBuyPrice, m_iPointCount);
		}
		else  // ��
		{
			strPrice = Float2String(m_dSellPrice, m_iPointCount);
		}

		if (NULL==m_pHoldDetailQueryOut)
		{
			AddButton(&rectEditPriceMarketPrice, m_pImgOrder, 3, IDC_DEF_EDIT_PRICEMARKETPRICE, L"ƽ��");
			m_mapButton[IDC_DEF_EDIT_PRICEMARKETPRICE].EnableButton(FALSE,TRUE);
		}
		else
		{
			AddButton(&rectEditPriceMarketPrice, m_pImgOrder, 3, IDC_DEF_EDIT_PRICEMARKETPRICE, L"ƽ�� "+strPrice);
		}

		// ���
		CRect rectCheckAllowPoint(rectEditPriceMarketPrice);
		rectCheckAllowPoint.left = rc.left+iSpace;
		rectCheckAllowPoint.top = rectEditPriceMarketPrice.bottom + iStaticVSpace + 10;
		rectCheckAllowPoint.right = rectCheckAllowPoint.left + iCheckWidth;
		rectCheckAllowPoint.bottom = rectCheckAllowPoint.top + iCheckHeight;
		if (m_bCheckAllowPoint)
		{
			AddButton(&rectCheckAllowPoint, m_pImgCheck, 3, IDC_DEF_CHECK_ALLOWPOINT, L"");
		}
		else
		{
			AddButton(&rectCheckAllowPoint, m_pImgUncheck, 3, IDC_DEF_CHECK_ALLOWPOINT, L"");
		}

		CRect rStaticCheckAllowPoint(rectCheckAllowPoint);
		rStaticCheckAllowPoint.left = rectCheckAllowPoint.right + 2;
		rStaticCheckAllowPoint.top = rectCheckAllowPoint.top;
		rStaticCheckAllowPoint.right = rStaticCheckAllowPoint.left + 60;
		rStaticCheckAllowPoint.bottom = rStaticCheckAllowPoint.top+iStaticHeight;
		if (m_StaticCheckAllowPoint.m_hWnd)
		{
			m_StaticCheckAllowPoint.MoveWindow(rStaticCheckAllowPoint);
		}

		CRect rectEditPoint(rStaticCheckAllowPoint);
		rectEditPoint.top = rStaticCheckAllowPoint.top - 4;
		rectEditPoint.bottom = rectEditPoint.top + iEditHeight;
		rectEditPoint.left = rStaticCheckAllowPoint.right + 1;
		rectEditPoint.right = rectEditPoint.left + 25;
		if (m_EditPoint.m_hWnd)
		{
			m_EditPoint.MoveWindow(rectEditPoint);
			CRect rcTmpRt;
			m_EditPoint.GetClientRect(&rcTmpRt);
			rcTmpRt.DeflateRect(0,3,0,3);
			m_EditPoint.SetRectNP(&rcTmpRt);
		}

		CRect rectSpinPoint(rectEditPoint);
		rectSpinPoint.top = rectEditPoint.top + 1;
		rectSpinPoint.bottom = rectSpinPoint.top + 28;
		rectSpinPoint.left = rectEditPoint.right;
		rectSpinPoint.right = rectSpinPoint.left + 15;
		if (m_SpinPoint.m_hWnd)
		{
			m_SpinPoint.MoveWindow(rectSpinPoint);
		}

		// �߿�
		CRect rectStaticBKPoint(rStaticCheckAllowPoint);
		rectStaticBKPoint.left = rectEditPoint.left-1;
		rectStaticBKPoint.top = rectEditPoint.top-1;
		rectStaticBKPoint.right = rectEditPoint.right+1;
		rectStaticBKPoint.bottom = rectEditPoint.bottom+1;
		if (m_StaticBKPoint.m_hWnd)
		{
			m_StaticBKPoint.MoveWindow(rectStaticBKPoint);
		}

		// ���ֽ���
		CRect rectSpinBack(rectSpinPoint);
		rectSpinBack.top = rectSpinPoint.top + 1;
		rectSpinBack.bottom = rectSpinBack.top + 28;
		rectSpinBack.right = rc.right - iSpace;
		rectSpinBack.left = rectSpinBack.right - 15;
		if (m_SpinBack.m_hWnd)
		{
			m_SpinBack.MoveWindow(rectSpinBack);
		}

		CRect rectEditBack(rectSpinBack);
		rectEditBack.right = rectSpinBack.left;
		rectEditBack.left = rectEditBack.right - 35;
		if (m_EditBack.m_hWnd)
		{
			m_EditBack.MoveWindow(rectEditBack);
			CRect rcRect;
			m_EditBack.GetClientRect(&rcRect);
			rcRect.DeflateRect(0,3,0,3);
			m_EditBack.SetRectNP(&rcRect);
		}

		CRect rectCheckAllowBack(rectEditBack);
		rectCheckAllowBack.top = rectEditBack.top + 4;
		rectCheckAllowBack.bottom = rectCheckAllowBack.top + iCheckHeight;
		rectCheckAllowBack.right = rectEditBack.left-70;
		rectCheckAllowBack.left = rectCheckAllowBack.right - iCheckWidth;
		if (m_bCheckBack)
		{
			AddButton(&rectCheckAllowBack, m_pImgCheck, 3, IDC_DEF_CHECK_ALLOWBACK, L"");
		}
		else
		{
			AddButton(&rectCheckAllowBack, m_pImgUncheck, 3, IDC_DEF_CHECK_ALLOWBACK, L"");
		}

		CRect rStaticCheckBack(rectCheckAllowBack);
		rStaticCheckBack.left = rectCheckAllowBack.right + 2;
		rStaticCheckBack.top = rectCheckAllowBack.top;
		rStaticCheckBack.right = rStaticCheckBack.left + 60;
		rStaticCheckBack.bottom = rStaticCheckBack.top+iStaticHeight;
		if (m_StaticCheckBack.m_hWnd)
		{
			m_StaticCheckBack.MoveWindow(rStaticCheckBack);
		}

		// �߿�
		CRect rectStaticBKBack(rectEditBack);
		rectStaticBKBack.left = rectEditBack.left-1;
		rectStaticBKBack.top = rectEditBack.top-1;
		rectStaticBKBack.right = rectEditBack.right+1;
		rectStaticBKBack.bottom = rectEditBack.bottom+1;
		if (m_StaticBKBack.m_hWnd)
		{
			m_StaticBKBack.MoveWindow(rectStaticBKBack);
		}

		// �µ�ǰȷ��
		CRect rectCheckBeforeOrder(rectCheckAllowPoint);
		rectCheckBeforeOrder.left = rectCheckAllowPoint.left;
		rectCheckBeforeOrder.top = rectCheckAllowPoint.bottom + iStaticVSpace + 10;
		rectCheckBeforeOrder.right = rectCheckBeforeOrder.left + iCheckWidth;
		rectCheckBeforeOrder.bottom = rectCheckBeforeOrder.top + iCheckHeight;
		if (m_bCheckBeforeOrder)
		{
			AddButton(&rectCheckBeforeOrder, m_pImgCheck, 3, IDC_DEF_CHECK_BEFOREORDER, L"");
		}
		else
		{
			AddButton(&rectCheckBeforeOrder, m_pImgUncheck, 3, IDC_DEF_CHECK_BEFOREORDER, L"");
		}

		CRect rStaticCheckBeforeOrder(rectCheckBeforeOrder);
		rStaticCheckBeforeOrder.left = rectCheckBeforeOrder.right + 2;
		rStaticCheckBeforeOrder.top = rectCheckBeforeOrder.top;
		rStaticCheckBeforeOrder.right = rStaticCheckBeforeOrder.left + 100;
		rStaticCheckBeforeOrder.bottom = rStaticCheckBeforeOrder.top+iStaticHeight;
		if (m_StaticCheckBeforeOrder.m_hWnd)
		{
			m_StaticCheckBeforeOrder.MoveWindow(rStaticCheckBeforeOrder);
		}

		// ��ʾ��Ϣ
		CRect rectStaticTip(rectCheckBeforeOrder);
		rectStaticTip.left = rectCheckBeforeOrder.left;
		rectStaticTip.top = rectCheckBeforeOrder.bottom + iStaticVSpace + 10;
		rectStaticTip.right = rc.right - iSpace;
		rectStaticTip.bottom = rectStaticTip.top + iStaticHeight;
		if (m_StaticTip.m_hWnd)
		{
			m_StaticTip.MoveWindow(rectStaticTip);
		}
	}
	else // ָ��
	{
		// ����
		CRect rectStaticBuySell(rectStaticCommInfo);
		rectStaticBuySell.left = rectStaticCommInfo.left;
		rectStaticBuySell.top = rectStaticCommInfo.bottom + iStaticVSpace;
		rectStaticBuySell.right = rectStaticBuySell.left + iStaticWidth;
		rectStaticBuySell.bottom = rectStaticBuySell.top + iStaticHeight;
		if (m_StaticBuySell.m_hWnd)
		{
			m_StaticBuySell.MoveWindow(rectStaticBuySell);
		}
		
		CRect rectBuySell(rectStaticBuySell);
		rectBuySell.left = rectStaticBuySell.right + iStaticDropSpace;
		rectBuySell.top = rectDropCommInfo.bottom + iStaticVSpace;
		rectBuySell.right = rectBuySell.left + 125;
		rectBuySell.bottom = rectBuySell.top + 30;
		CString strBuySell = _T(" ");	
		if (EET_SELL == m_EntrustType)
		{
			strBuySell = _T("�� ");
		}
		else
		{
			strBuySell = _T("�� ");
		}
		
		// ����
		const int iEdidHandWidth = 60;
		//const int iSpinHandWidth = 15;
		CRect rectEditHand(rectBuySell);
		rectEditHand.right = rc.right - iSpace;
		rectEditHand.left = rectEditHand.right - iEdidHandWidth;
		if (m_EditHand.m_hWnd)
		{
			m_EditHand.MoveWindow(rectEditHand);
			CRect rect;
			m_EditHand.GetClientRect(&rect);
			rect.DeflateRect(0,3,0,3);
			m_EditHand.SetRectNP(&rect);
		}

		CRect rectStaticHand(rectEditHand);
		rectStaticHand.top = rectStaticBuySell.top;
		rectStaticHand.bottom = rectStaticHand.top + iStaticHeight;
		rectStaticHand.right = rectEditHand.left - iSpace;
		rectStaticHand.left = rectStaticHand.right - iStaticWidth;
		if (m_StaticHand.m_hWnd)
		{
			m_StaticHand.MoveWindow(rectStaticHand);
		}

		CRect rectStaticBKHand(rectEditHand);
		rectStaticBKHand.left = rectEditHand.left-1;
		rectStaticBKHand.top = rectEditHand.top-1;
		rectStaticBKHand.right = rectEditHand.right +1;
		rectStaticBKHand.bottom = rectEditHand.bottom+1;
		if (m_StaticBKHand.m_hWnd)
		{
			m_StaticBKHand.MoveWindow(rectStaticBKHand);
		}
		
		//��ϸ
		CRect rectStaticDetail(rectStaticBuySell);
		rectStaticDetail.left = rectStaticBuySell.left;
		rectStaticDetail.top = rectStaticBuySell.bottom+iStaticVSpace;
		rectStaticDetail.right = rectStaticDetail.left+iStaticWidth;
		rectStaticDetail.bottom = rectStaticDetail.top+iStaticHeight;
		if (m_StaticDetail.m_hWnd)
		{
			m_StaticDetail.MoveWindow(rectStaticDetail);
		}
		
		CRect rectEditDetail(rectStaticDetail);
		rectEditDetail.left = rectStaticDetail.right+iStaticDropSpace;
		rectEditDetail.top = rectStaticDetail.top;//rectEditHand.bottom+iStaticVSpace + 5;
		rectEditDetail.right = rectDropCommInfo.right;
		rectEditDetail.bottom = rectEditDetail.top+iStaticHeight;
		if (m_EditDetail.m_hWnd)
		{
			m_EditDetail.MoveWindow(rectEditDetail);
		}

		// ֹ��
		CRect rectCheckStopLoss(rc);
		rectCheckStopLoss.left = rc.left+iSpace;
		rectCheckStopLoss.top = rectEditDetail.bottom + iStaticDropSpace;
		rectCheckStopLoss.right = rectCheckStopLoss.left + iCheckWidth;
		rectCheckStopLoss.bottom = rectCheckStopLoss.top + iCheckHeight;
		if (m_bCheckStopLoss)
		{
			AddButton(&rectCheckStopLoss, m_pImgCheck, 3, IDC_DEF_CHECK_STOPLOSS, L"");
		}
		else
		{
			AddButton(&rectCheckStopLoss, m_pImgUncheck, 3, IDC_DEF_CHECK_STOPLOSS, L"");
		}

		CRect rStaticCheckStopLoss(rectCheckStopLoss);
		rStaticCheckStopLoss.left = rectCheckStopLoss.right + 2;
		rStaticCheckStopLoss.top = rectCheckStopLoss.top;
		rStaticCheckStopLoss.right = rStaticCheckStopLoss.left + 35;
		rStaticCheckStopLoss.bottom = rStaticCheckStopLoss.top+iStaticHeight;
		if (m_StaticCheckStopLoss.m_hWnd)
		{
			m_StaticCheckStopLoss.MoveWindow(rStaticCheckStopLoss);
		}

		//�߿� �� ��-1  �� ��+2
		const int iEditStopLoss = 73;
		const int iSpinStopLoss = 15;
		CRect rectStaticBKStopLoss1(rStaticCheckStopLoss);
		rectStaticBKStopLoss1.left = rStaticCheckStopLoss.right+5-1;
		rectStaticBKStopLoss1.top = rStaticCheckStopLoss.top-1-5;
		rectStaticBKStopLoss1.right = rectStaticBKStopLoss1.left+iEditStopLoss+2;
		rectStaticBKStopLoss1.bottom = rectStaticBKStopLoss1.top+iEditHeight+2;
		if (m_StaticBKStopLoss1.m_hWnd)
		{
			m_StaticBKStopLoss1.MoveWindow(rectStaticBKStopLoss1);
		}
		CRect rectEditStopLoss1(rStaticCheckStopLoss);
		rectEditStopLoss1.left = rStaticCheckStopLoss.right+5;
		rectEditStopLoss1.top = rStaticCheckStopLoss.top-5;
		rectEditStopLoss1.right = rectEditStopLoss1.left+iEditStopLoss;
		rectEditStopLoss1.bottom = rectEditStopLoss1.top+iEditHeight;
		if (m_EditStopLoss1.m_hWnd)
		{
			m_EditStopLoss1.MoveWindow(rectEditStopLoss1);
			CRect rctTemp;
			m_EditStopLoss1.GetClientRect(&rctTemp);
			rctTemp.DeflateRect(0,3,0,3);
			m_EditStopLoss1.SetRectNP(&rctTemp);
		}
		CRect rectSpinStopLoss(rectEditStopLoss1);
		rectSpinStopLoss.left = rectEditStopLoss1.right;
		rectSpinStopLoss.top = rectEditStopLoss1.top+1;
		rectSpinStopLoss.right = rectSpinStopLoss.left+iSpinStopLoss;
		rectSpinStopLoss.bottom = rectSpinStopLoss.top + 28;
		if (m_SpinStopLoss.m_hWnd)
		{
			m_SpinStopLoss.MoveWindow(rectSpinStopLoss);
		}
		CRect rectStaticStopLoss(rectSpinStopLoss);
		rectStaticStopLoss.left = rectSpinStopLoss.right+15;
		rectStaticStopLoss.top = rStaticCheckStopLoss.top;
		rectStaticStopLoss.right = rectStaticStopLoss.left+10;
		rectStaticStopLoss.bottom = rStaticCheckStopLoss.bottom;
		if (m_StaticStopLoss.m_hWnd)
		{
			m_StaticStopLoss.MoveWindow(rectStaticStopLoss);
		}
		CRect rectStaticBKStopLoss2(rectStaticStopLoss);
		rectStaticBKStopLoss2.left = rectStaticStopLoss.right+15-1;
		rectStaticBKStopLoss2.top = rectEditStopLoss1.top-1;
		rectStaticBKStopLoss2.right = rc.right - iSpace + 2;
		rectStaticBKStopLoss2.bottom = rectStaticBKStopLoss2.top+iEditHeight+2;
		if (m_StaticBKStopLoss2.m_hWnd)
		{
			m_StaticBKStopLoss2.MoveWindow(rectStaticBKStopLoss2);
		}
		
		CRect rectEditStopLoss2(rectStaticStopLoss);
		rectEditStopLoss2.left = rectStaticStopLoss.right+15;
		rectEditStopLoss2.top = rectEditStopLoss1.top;
		rectEditStopLoss2.right = rc.right - iSpace;
		rectEditStopLoss2.bottom = rectEditStopLoss1.bottom;
		if (m_EditStopLoss2.m_hWnd)
		{
			m_EditStopLoss2.MoveWindow(rectEditStopLoss2);
			CRect rcTmp;
			m_EditStopLoss2.GetClientRect(&rcTmp);
			rcTmp.DeflateRect(0,3,0,3);
			m_EditStopLoss2.SetRectNP(&rcTmp);
		}

		// ֹӯ
		CRect rectCheckStopProfit(rc);
		rectCheckStopProfit.left = rectCheckStopLoss.left;
		rectCheckStopProfit.top = rectCheckStopLoss.bottom + iStaticDropSpace + 13;
		rectCheckStopProfit.right = rectCheckStopLoss.right;
		rectCheckStopProfit.bottom = rectCheckStopProfit.top+iCheckHeight;
		if (m_bCheckStopProfit)
		{
			AddButton(&rectCheckStopProfit, m_pImgCheck, 3, IDC_DEF_CHECK_STOPPROFIT, L"");
		}
		else
		{
			AddButton(&rectCheckStopProfit, m_pImgUncheck, 3, IDC_DEF_CHECK_STOPPROFIT, L"");
		}

		CRect rStaticCheckStopProfit(rectCheckStopProfit);
		rStaticCheckStopProfit.left = rectCheckStopProfit.right + 2;
		rStaticCheckStopProfit.top = rectCheckStopProfit.top;
		rStaticCheckStopProfit.right = rStaticCheckStopProfit.left + 35;
		rStaticCheckStopProfit.bottom = rStaticCheckStopProfit.top+iStaticHeight;
		if (m_StaticCheckStopProfit.m_hWnd)
		{
			m_StaticCheckStopProfit.MoveWindow(rStaticCheckStopProfit);
		}
		
		//�߿�
		CRect rectStaticBKStopProfit1(rectStaticStopLoss);
		rectStaticBKStopProfit1.left = rStaticCheckStopProfit.right+5-1;
		rectStaticBKStopProfit1.top = rStaticCheckStopProfit.top-1 - 5;
		rectStaticBKStopProfit1.right = rectStaticBKStopProfit1.left+iEditStopLoss+2;
		rectStaticBKStopProfit1.bottom = rectStaticBKStopProfit1.top+iEditHeight+2;
		if (m_StaticBKStopProfit1.m_hWnd)
		{
			m_StaticBKStopProfit1.MoveWindow(rectStaticBKStopProfit1);
		}
		
		CRect rectEditStopProfit1(rStaticCheckStopProfit);
		rectEditStopProfit1.left = rStaticCheckStopProfit.right+5;
		rectEditStopProfit1.top = rStaticCheckStopProfit.top-5;
		rectEditStopProfit1.right = rectEditStopProfit1.left+iEditStopLoss;
		rectEditStopProfit1.bottom = rectEditStopProfit1.top+iEditHeight;
		if (m_EditStopProfit1.m_hWnd)
		{
			m_EditStopProfit1.MoveWindow(rectEditStopProfit1);
			CRect rt;
			m_EditStopProfit1.GetClientRect(&rt);
			rt.DeflateRect(0,3,0,3);
			m_EditStopProfit1.SetRectNP(&rt);
		}
		CRect rectSpinStopProfit(rectEditStopProfit1);
		rectSpinStopProfit.left = rectEditStopProfit1.right;
		rectSpinStopProfit.top = rectEditStopProfit1.top+1;
		rectSpinStopProfit.right = rectSpinStopProfit.left+iSpinStopLoss;
		rectSpinStopProfit.bottom = rectSpinStopProfit.top + 28;
		if (m_SpinStopProfit.m_hWnd)
		{
			m_SpinStopProfit.MoveWindow(rectSpinStopProfit);
		}
		CRect rectStaticStopProfit(rectSpinStopProfit);
		rectStaticStopProfit.left = rectSpinStopProfit.right+15;
		rectStaticStopProfit.top = rStaticCheckStopProfit.top;
		rectStaticStopProfit.right = rectStaticStopProfit.left+10;
		rectStaticStopProfit.bottom = rStaticCheckStopProfit.bottom;
		if (m_StaticStopProfit.m_hWnd)
		{
			m_StaticStopProfit.MoveWindow(rectStaticStopProfit);
		}
		//�߿�
		CRect rectStaticBKStopProfit2(rectStaticStopLoss);
		rectStaticBKStopProfit2.left = rectStaticStopProfit.right+15-1;
		rectStaticBKStopProfit2.top = rectEditStopProfit1.top-1;
		rectStaticBKStopProfit2.right = rc.right - iSpace + 2;
		rectStaticBKStopProfit2.bottom = rectStaticBKStopProfit2.top+iEditHeight+2;
		if (m_StaticBKStopProfit2.m_hWnd)
		{
			m_StaticBKStopProfit2.MoveWindow(rectStaticBKStopProfit2);
		}
		CRect rectEditStopProfit2(rectStaticStopProfit);
		rectEditStopProfit2.left = rectStaticStopProfit.right+15;
		rectEditStopProfit2.top = rectEditStopProfit1.top;
		rectEditStopProfit2.right = rc.right - iSpace;
		rectEditStopProfit2.bottom = rectEditStopProfit1.bottom;
		if (m_EditStopProfit2.m_hWnd)
		{
			m_EditStopProfit2.MoveWindow(rectEditStopProfit2);
			CRect rct;
			m_EditStopProfit2.GetClientRect(&rct);
			rct.DeflateRect(0,3,0,3);
			m_EditStopProfit2.SetRectNP(&rct);
		}

		// �µ�
		CRect rectEditPriceMarketPrice(rc);
		int iPriceEditWidth2 = rc.right-rc.left-iSpace*2;
		rectEditPriceMarketPrice.left = rc.left+iSpace;
		rectEditPriceMarketPrice.top = rectCheckStopProfit.bottom + 20;
		rectEditPriceMarketPrice.right = rectEditPriceMarketPrice.left + iPriceEditWidth2;
		rectEditPriceMarketPrice.bottom = rectEditPriceMarketPrice.top + iButtonHeight;
		AddButton(&rectEditPriceMarketPrice, m_pImgOrder, 3, IDC_DEF_EDIT_PRICEMARKETPRICE, L"�µ�");
		CString strPrice = _T(" ");
		if (EET_BUY == m_EntrustType) // ��
		{
			strPrice = Float2String(m_dBuyPrice, m_iPointCount);
			AddButton(&rectBuySell, m_pImgBuy, 4, IDC_DEF_BUTTON_BUYSELL, strBuySell+strPrice);
		}
		else  // ��
		{
			strPrice = Float2String(m_dSellPrice, m_iPointCount);
			AddButton(&rectBuySell, m_pImgSell, 4, IDC_DEF_BUTTON_BUYSELL, strBuySell+strPrice);
		}

		// �µ�ǰȷ��
		CRect rectCheckBeforeOrder(rectEditPriceMarketPrice);
		rectCheckBeforeOrder.left = rectEditPriceMarketPrice.left;
		rectCheckBeforeOrder.top = rectEditPriceMarketPrice.bottom + iStaticVSpace*2;
		rectCheckBeforeOrder.right = rectCheckBeforeOrder.left + iCheckWidth;
		rectCheckBeforeOrder.bottom = rectCheckBeforeOrder.top + iCheckHeight;
		if (m_bCheckBeforeOrder)
		{
			AddButton(&rectCheckBeforeOrder, m_pImgCheck, 3, IDC_DEF_CHECK_BEFOREORDER, L"");
		}
		else
		{
			AddButton(&rectCheckBeforeOrder, m_pImgUncheck, 3, IDC_DEF_CHECK_BEFOREORDER, L"");
		}

		CRect rStaticCheckBeforeOrder(rectCheckBeforeOrder);
		rStaticCheckBeforeOrder.left = rectCheckBeforeOrder.right + 2;
		rStaticCheckBeforeOrder.top = rectCheckBeforeOrder.top;
		rStaticCheckBeforeOrder.right = rStaticCheckBeforeOrder.left + 100;
		rStaticCheckBeforeOrder.bottom = rStaticCheckBeforeOrder.top+iStaticHeight;
		if (m_StaticCheckBeforeOrder.m_hWnd)
		{
			m_StaticCheckBeforeOrder.MoveWindow(rStaticCheckBeforeOrder);
		}
		
		// ��ʾ��Ϣ
		CRect rectStaticTip(rectCheckBeforeOrder);
		rectStaticTip.left = rectCheckBeforeOrder.left;
		rectStaticTip.right = rc.right - iSpace;
		rectStaticTip.top = rectCheckBeforeOrder.bottom + iStaticVSpace + 3;
		rectStaticTip.bottom = rectStaticTip.top + iStaticHeight;
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

void CDlgTradeClose::OnSize( UINT nType, int cx, int cy )
{
	CDialog::OnSize(nType, cx, cy);

	if (!m_bInitImg)
	{
		InitImgResource();
		m_bInitImg = true;
	}
	
	RecalcLayout(false);
}

void CDlgTradeClose::OnSpinBtnHand( NMHDR *pNotify, LRESULT *pRes )
{
// 	m_EditHand.SetFocus();	//���������ù���

	LPNMUPDOWN pNMUpDown = (LPNMUPDOWN)pNotify;
	
	// ��ȡ��ǰֵ
	CString str = _T("");
	string strQty;
	m_EditHand.GetWindowText(str);
	UnicodeToUtf8(str, strQty);
	m_iQty = atoi(strQty.c_str());
	
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(pNMUpDown->iDelta >0/*== 1*/) // �����ֵΪ1 , ˵�������Spin�����¼�ͷ 
	{
		m_iQty -= 1;
		if (m_iQty<m_iQtyMin)
		{
			m_iQty = m_iQtyMin;
		}
	} 
	else if(pNMUpDown->iDelta <0/*== -1*/) // �����ֵΪ-1 , ˵�������Spin�����ϼ�ͷ 
	{
		m_iQty += 1;
		if (m_iQty>m_iQtyMax)
		{
			m_iQty = m_iQtyMax;
		}
    }
	
	//��ʾ
	str.Format(_T("%d"),m_iQty);
	m_EditHand.SetWindowText(str);
	m_EditHand.SetFocus();	//���������ù���
	//lint --e(570)
	m_EditHand.SetSel(-1);

	str.Format(_T("��ʾ����ƽ��������С%d�֡����%d�� ��"),m_iQtyMin,m_iQtyMax);
	m_StaticTip.SetWindowText(str);
 }

void CDlgTradeClose::OnEnKillFocusEditHand()
{
	m_StaticTip.SetWindowText(_T(""));
}

void CDlgTradeClose::OnSetFocusEditHand()
{
	CString str = _T("");
	str.Format(_T("��ʾ����ƽ��������С%d�֡����%d�� ��"),m_iQtyMin,m_iQtyMax);
	m_StaticTip.SetWindowText(str);
}

void CDlgTradeClose::OnSetFocusEditPoint()
{
	CString str = _T("");
	str.Format(_T("��ʾ�������õ�Χ��С%d�����%d ��"),m_iPointMin,m_iPointMax);
	m_StaticTip.SetWindowText(str);
}

void CDlgTradeClose::OnSetFocusEditBack()
{
	CString str = _T("");	
	str.Format(_T("��ʾ�����ֽ�������С%d�����%d ��"),m_iOpenQtyMin,m_iOpenQtyMax);
	m_StaticTip.SetWindowText(str);
}

// void CDlgTradeClose::OnSetFocusPrice()
// {
// 	GetDlgItem(IDC_DEF_STATIC_COMMINFO)->SetFocus();	//����ת��
// }
// 
// void CDlgTradeClose::OnSetFocusEditStopLoss2()
// {
// 	GetDlgItem(IDC_DEF_STATIC_ENTRUSTTYPE)->SetFocus();	//����ת��
// }
// 
// void CDlgTradeClose::OnSetFocusEditStopProfit2()
// {
// 	GetDlgItem(IDC_DEF_STATIC_ENTRUSTTYPE)->SetFocus();	//����ת��
// }

void CDlgTradeClose::OnSpinBtnPoint( NMHDR *pNotify, LRESULT *pRes )
{
	LPNMUPDOWN pNMUpDown = (LPNMUPDOWN)pNotify;
	
	// ��ȡ��ǰֵ
	CString str = _T("");
	string strPoint;
	m_EditPoint.GetWindowText(str);
	UnicodeToUtf8(str, strPoint);
	m_iPoint = atoi(strPoint.c_str());
	
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(pNMUpDown->iDelta >0/*== 1*/) // �����ֵΪ1 , ˵�������Spin�����¼�ͷ 
	{
		m_iPoint -= 1;
		if (m_iPoint<m_iPointMin)
		{
			m_iPoint = m_iPointMin;
		}
	} 
	else if(pNMUpDown->iDelta <0/*== -1*/) // �����ֵΪ-1 , ˵�������Spin�����ϼ�ͷ 
	{
		m_iPoint += 1;
		if (m_iPoint>m_iPointMax)
		{
			m_iPoint = m_iPointMax;
		}
    }
	
	//��ʾ
	str.Format(_T("%d"),m_iPoint);
	m_EditPoint.SetWindowText(str);

	str.Format(_T("��ʾ�������õ�Χ��С%d�����%d ��"),m_iPointMin,m_iPointMax);
	m_StaticTip.SetWindowText(str);
}

void CDlgTradeClose::OnSpinBtnBack( NMHDR *pNotify, LRESULT *pRes )
{
	LPNMUPDOWN pNMUpDown = (LPNMUPDOWN)pNotify;
	
	// ��ȡ��ǰֵ
	CString str = _T("");
	string strBack;
	m_EditBack.GetWindowText(str);
	UnicodeToUtf8(str, strBack);
	m_iBack = atoi(strBack.c_str());
	
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(pNMUpDown->iDelta >0/*== 1*/) // �����ֵΪ1 , ˵�������Spin�����¼�ͷ 
	{
		m_iBack -= 1;
		if (m_iBack<m_iOpenQtyMin)
		{
			m_iBack = m_iOpenQtyMin;
		}
	} 
	else if(pNMUpDown->iDelta <0/*== -1*/) // �����ֵΪ-1 , ˵�������Spin�����ϼ�ͷ 
	{
		m_iBack += 1;
		if (m_iBack>m_iOpenQtyMax)
		{
			m_iBack= m_iOpenQtyMax;
		}
    }
	
	//��ʾ
	str.Format(_T("%d"),m_iBack);
	m_EditBack.SetWindowText(str);

	str.Format(_T("��ʾ�����ֽ�������С%d�����%d ��"),m_iOpenQtyMin,m_iOpenQtyMax);
	m_StaticTip.SetWindowText(str);
}

void CDlgTradeClose::OnSpinBtnStopLoss( NMHDR *pNotify, LRESULT *pRes )
{
//	m_EditStopLoss1.SetFocus();	//���������ù���

	LPNMUPDOWN pNMUpDown = (LPNMUPDOWN)pNotify;
	
	// ��ȡ��ǰֵ
	CString str = _T("");
// 	string strQty;
// 	m_EditStopLoss1.GetWindowText(str);
// 	UnicodeToUtf8(str, strQty);
// 	double dStopLoss1 = atof(strQty.c_str());
	if (m_dStopLoss1>-0.000001&&m_dStopLoss1<0.000001)
	{
		string strQty;
		m_EditStopLoss2.GetWindowText(str);
		UnicodeToUtf8(str, strQty);
		m_dStopLoss1 = atof(strQty.c_str());
	}
	
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(pNMUpDown->iDelta >0/*== 1*/) // �����ֵΪ1 , ˵�������Spin�����¼�ͷ 
	{
		m_dStopLoss1 -= m_dPriceStep;
		if (m_dStopLoss1<=0)
		{
			m_dStopLoss1 = 0;
		}
	} 
	else if(pNMUpDown->iDelta <0/*== -1*/) // �����ֵΪ-1 , ˵�������Spin�����ϼ�ͷ 
	{
		m_dStopLoss1 += m_dPriceStep;
    }
	//��ʾ
	str = Float2String(m_dStopLoss1, m_iPointCount, FALSE, FALSE);
	m_EditStopLoss1.SetWindowText(str);
	m_EditStopLoss1.SetFocus();	//���������ù���
	//lint --e(570)
	m_EditStopLoss1.SetSel(-1);
}

void CDlgTradeClose::OnSpinBtnStopProfit( NMHDR *pNotify, LRESULT *pRes )
{	
// 	m_EditStopProfit1.SetFocus();	//���������ù���

	LPNMUPDOWN pNMUpDown = (LPNMUPDOWN)pNotify;
	
	// ��ȡ��ǰֵ
	CString str = _T("");
// 	string strQty;
// 	m_EditStopProfit1.GetWindowText(str);
// 	UnicodeToUtf8(str, strQty);
// 	double dStopProfit1 = atof(strQty.c_str());
	//Ϊ0ʱȡ��ǰ�۸�
	if (m_dStopProfit1>-0.000001&&m_dStopProfit1<0.000001)
	{
		string strQty;
		m_EditStopProfit2.GetWindowText(str);
		UnicodeToUtf8(str, strQty);
		m_dStopProfit1 = atof(strQty.c_str());
	}
	
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(pNMUpDown->iDelta >0/*== 1*/) // �����ֵΪ1 , ˵�������Spin�����¼�ͷ 
	{
		m_dStopProfit1 -= m_dPriceStep;
		if (m_dStopProfit1<=0)
		{
			m_dStopProfit1 = 0;
		}
	}
	else if(pNMUpDown->iDelta <0/*== -1*/) // �����ֵΪ-1 , ˵�������Spin�����ϼ�ͷ 
	{
		m_dStopProfit1 += m_dPriceStep;
    }
	//��ʾ
	str = Float2String(m_dStopProfit1, m_iPointCount, FALSE, FALSE);
	m_EditStopProfit1.SetWindowText(str);
	m_EditStopProfit1.SetFocus();	//���������ù���
	//lint --e(570)
	m_EditStopProfit1.SetSel(-1);
}

void CDlgTradeClose::OnEnChangeEditPoint()
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

void CDlgTradeClose::OnEnChangeEditStopLoss1()
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

void CDlgTradeClose::OnEnChangeEditStopProfit1()
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

void CDlgTradeClose::OnSetFocusPrice()
{
//	GetDlgItem(IDC_DEF_STATIC_COMMINFO)->SetFocus();	//����ת��
}

void CDlgTradeClose::OnSetFocusEditStopLoss2()
{
//	GetDlgItem(IDC_DEF_STATIC_ENTRUSTTYPE)->SetFocus();	//����ת��
}

void CDlgTradeClose::OnSetFocusEditStopProfit2()
{
//	GetDlgItem(IDC_DEF_STATIC_ENTRUSTTYPE)->SetFocus();	//����ת��
}

void CDlgTradeClose::OnEnKillFocusEditStopLoss1()
{
	CString strPrice = _T("");	
	strPrice = Float2String(m_dStopLoss1, m_iPointCount, FALSE, FALSE);
	m_EditStopLoss1.SetWindowText(strPrice);
	//lint --e(570)
	m_EditStopLoss1.SetSel(-1);
}

void CDlgTradeClose::OnEnKillFocusEditStopProfit1()
{
	CString strPrice = _T("");	
	strPrice = Float2String(m_dStopProfit1, m_iPointCount, FALSE, FALSE);
	m_EditStopProfit1.SetWindowText(strPrice);
	//lint --e(570)
	m_EditStopProfit1.SetSel(-1);
}

HBRUSH CDlgTradeClose::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO:  �ڴ˸��� DC ���κ�����
	switch(nCtlColor)
	{
	case CTLCOLOR_EDIT:
		{
			if ((pWnd->GetDlgCtrlID() == IDC_DEF_EDIT_STOPLOSS2)
				||(pWnd->GetDlgCtrlID() == IDC_DEF_EDIT_STOPPROFIT2))
			{
				static HBRUSH hbrEdit = ::CreateSolidBrush( BK_COLOR_BUYPRICE );
				pDC->SetBkColor( BK_COLOR_BUYPRICE );
				pDC->SetTextColor( TEXT_COLOR_BUYPRICE );
				return hbrEdit;	
			}

			if (IDC_DEF_EDIT_COMMINFO == pWnd->GetDlgCtrlID())
			{
				pDC->SetBkMode(TRANSPARENT);
				pDC->SetTextColor(RGB(0x4d,0x4d,0x4d));
			}
			static HBRUSH hbrEdit = ::CreateSolidBrush(RGB(255,255,255));
			pDC->SetBkColor(RGB(255,255,255));
			return hbrEdit;	
		}
		break;
	case CTLCOLOR_STATIC:
		{
			if ((pWnd->GetDlgCtrlID() == IDC_DEF_STATIC_TIP)||
				(pWnd->GetDlgCtrlID() == IDC_DEF_STATIC_DETAIL) || 
				(pWnd->GetDlgCtrlID() == IDC_DEF_EDIT_DETAIL))
			{
				pDC->SetBkMode(TRANSPARENT);
				pDC->SetTextColor(TEXT_COLOR_STATIC_TIP);
			}
			else if ((pWnd->GetDlgCtrlID() == IDC_DEF_STATIC_HAND_BK)
				||(pWnd->GetDlgCtrlID() == IDC_DEF_STATIC_POINT_BK)
				||(pWnd->GetDlgCtrlID() == IDC_DEF_STATIC_BACK_BK)
				||(pWnd->GetDlgCtrlID() == IDC_DEF_STATIC_STOPLOSS1_BK)
				||(pWnd->GetDlgCtrlID() == IDC_DEF_STATIC_STOPLOSS2_BK)
				||(pWnd->GetDlgCtrlID() == IDC_DEF_STATIC_STOPPROFIT1_BK)
				||(pWnd->GetDlgCtrlID() == IDC_DEF_STATIC_STOPPROFIT2_BK)
				||(pWnd->GetDlgCtrlID() == IDC_DEF_STATIC_DETAIL_BK))
			{
				static HBRUSH hbrh = ::CreateSolidBrush( BK_COLOR_EDIT );
				pDC->SetBkColor( BK_COLOR_EDIT);
				return hbrh;
			}
			else
			{
				pDC->SetBkMode(TRANSPARENT);
				pDC->SetTextColor(RGB(0x4d,0x4d,0x4d));
			}

			static HBRUSH hTmpBr = ::CreateSolidBrush(RGB(0xf2,0xf0,0xeb));
			pDC->SetBkColor(RGB(0xf2,0xf0,0xeb));

			if(m_pTradeLoginInfo->eTradeLoginType == ETT_TRADE_SIMULATE)
			{
				static HBRUSH hBrush = ::CreateSolidBrush(RGB(236, 239, 243));
				pDC->SetBkColor(RGB(236, 239, 243));
				return hBrush;
			}

			return hTmpBr;
		}
		break;
	default:
		break;
	}
	
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}

// ������
void CDlgTradeClose::OnBnCheckAllowPoint()
{
	//UpdateData(TRUE);
	if (!m_bCheckAllowPoint)
	{
		m_EditPoint.EnableWindow(TRUE);
		m_SpinPoint.EnableWindow(TRUE);
		m_bCheckAllowPoint = TRUE;
		m_mapButton[IDC_DEF_CHECK_ALLOWPOINT].SetImage(m_pImgCheck);
	}
	else
	{
		m_EditPoint.EnableWindow(FALSE);
		m_SpinPoint.EnableWindow(FALSE);
		m_bCheckAllowPoint = FALSE;
		m_mapButton[IDC_DEF_CHECK_ALLOWPOINT].SetImage(m_pImgUncheck);
	}
}

// �����ֽ���
void CDlgTradeClose::OnBnCheckAllowBack()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//UpdateData(TRUE);
	if (!m_bCheckBack)
	{
		m_EditBack.EnableWindow(TRUE);
		m_SpinBack.EnableWindow(TRUE);
		m_bCheckBack = TRUE;
		m_mapButton[IDC_DEF_CHECK_ALLOWBACK].SetImage(m_pImgCheck);
	}
	else
	{
		m_EditBack.EnableWindow(FALSE);
		m_SpinBack.EnableWindow(FALSE);
		m_bCheckBack = FALSE;
		m_mapButton[IDC_DEF_CHECK_ALLOWBACK].SetImage(m_pImgUncheck);
	}
}

// �µ�ǰȷ��
void CDlgTradeClose::OnBnCheckBeforeOrder()
{
	if (!m_bCheckBeforeOrder)
	{
		m_bCheckBeforeOrder = TRUE;
		m_mapButton[IDC_DEF_CHECK_BEFOREORDER].SetImage(m_pImgCheck);
	}
	else
	{
		m_bCheckBeforeOrder = FALSE;
		m_mapButton[IDC_DEF_CHECK_BEFOREORDER].SetImage(m_pImgUncheck);
	}
}

//ֹ��
void CDlgTradeClose::OnBnCheckStopLoss()
{
	if (!m_bCheckStopLoss)
	{
		m_EditStopLoss1.EnableWindow(TRUE);
		m_EditStopLoss2.EnableWindow(TRUE);
		m_SpinStopLoss.EnableWindow(TRUE);
		m_bCheckStopLoss = TRUE;
		m_mapButton[IDC_DEF_CHECK_STOPLOSS].SetImage(m_pImgCheck);
	}
	else
	{
		m_EditStopLoss1.EnableWindow(FALSE);
		m_EditStopLoss2.EnableWindow(FALSE);
		m_SpinStopLoss.EnableWindow(FALSE);
		m_bCheckStopLoss = FALSE;
		m_mapButton[IDC_DEF_CHECK_STOPLOSS].SetImage(m_pImgUncheck);
	}
}

// ֹӯ
void CDlgTradeClose::OnBnCheckStopProfit()
{
	if (!m_bCheckStopProfit)
	{
		m_EditStopProfit1.EnableWindow(TRUE);
		m_EditStopProfit2.EnableWindow(TRUE);
		m_SpinStopProfit.EnableWindow(TRUE);
		m_bCheckStopProfit = TRUE;
		m_mapButton[IDC_DEF_CHECK_STOPPROFIT].SetImage(m_pImgCheck);
	}
	else
	{
		m_EditStopProfit1.EnableWindow(FALSE);
		m_EditStopProfit2.EnableWindow(FALSE);
		m_SpinStopProfit.EnableWindow(FALSE);
		m_bCheckStopProfit = FALSE;
		m_mapButton[IDC_DEF_CHECK_STOPPROFIT].SetImage(m_pImgUncheck);
	}
}

void CDlgTradeClose::OnBtnDropCommInfo()
{
	SetPrice();	// ��۸�
	//SetHandRange();// ���÷�Χ
}

// ���ť
void CDlgTradeClose::OnBtnMax()
{
	CString str = _T("");
	str.Format(_T("%d"),m_iQtyMax);
	m_EditHand.SetWindowText(str);
}

// ȷ��
void CDlgTradeClose::OnBtnOk()
{	
	if (NULL==m_pTradeLoginInfo||NULL==m_pTradeBid||NULL==m_pTradeQuery||NULL==m_pHoldDetailQueryOut)
	{
		//ASSERT(0);
		return;
	}
	
	//���ж��Ƿ�����µ�����
	CString strWarn = _T("");
	if (0!=WarningMsg(strWarn))
	{
		CDlgTip dlg;
		dlg.m_strTipMsg = strWarn;
		dlg.m_eTipType = ETT_WAR;
		dlg.m_pCenterWnd = GetParent();	// ����Ҫ���е��Ĵ���ָ��

		if (IDOK==dlg.DoModal())
		{
		}
		
		return;
	}

	CClientReqEntrust reqEntrust;
	CClientReqSetStopLP reqSetStopLP;
//	memset(&reqEntrust, 0, sizeof(CClientReqEntrust));
	
	// ������
	FindOtherFirm(0, m_OtherID);	//Ĭ��ȡ��һ���������Ƕ�����

	CString strComm = _T("");
	CString strQty = _T("");
	CString strBuySell = _T("");
	CString strPrice = _T("");
	CString strStopLoss = _T("");
	CString strStopProfit = _T("");
	CString strHoldNumber = _T("");
	
	CString strUserID = m_pTradeLoginInfo->StrUser;
	CString strSessionID = m_pTradeLoginInfo->StrUserSessionID;
	CString strSessionInfo = m_pTradeLoginInfo->StrUserSessionInfo;

	int iMarketOrLimit = 0;//0-�м� 1-�޼�
	m_EditHand.GetWindowText(strQty);
 	UnicodeToUtf8(strQty, m_strQty);
	if (ECT_Market == m_eCloseType)
	{
		iMarketOrLimit = 0;
		reqEntrust.account = strUserID;
		reqEntrust.user_session_id = strSessionID;
		reqEntrust.user_session_info = strSessionInfo;

		// ����
		if (EET_BUY == m_EntrustType)
		{
			reqEntrust.entrust_bs = _T("B");
			strBuySell = _T("ƽ������");
			strPrice.Format(L"%.2f", m_dBuyPrice);
		}
		else if (EET_SELL==m_EntrustType)
		{
			reqEntrust.entrust_bs = _T("S");
			strBuySell = _T("ƽ������");
			strPrice.Format(L"%.2f", m_dSellPrice);
		}
		else
		{
			//ASSERT(0);
			return;
		}
		// ��Ʒ
		FindCommInfoType(m_DropCommInfo.GetCurSel(), m_CommInfo);
		reqEntrust.stock_code = m_CommInfo.chTypeValue;
		m_DropCommInfo.GetWindowText(strComm);
		// �۸�
	//	m_mapButton[IDC_DEF_EDIT_PRICEMARKETPRICE].GetCaption(strPrice);
 		UnicodeToUtf8(strPrice, m_strPrice);
		reqEntrust.close_price = atof((char*)m_strPrice.c_str());
		reqEntrust.entrust_price = m_strPrice.c_str();
		// ����
		reqEntrust.entrust_amount = atoi((char*)m_strQty.c_str());
		//���
		CString str = _T("");
		if (m_bCheckAllowPoint)//���õ��
		{
			m_EditPoint.GetWindowText(str);
 			UnicodeToUtf8(str, m_strPoint);
			reqEntrust.dot_diff = atof((char*)m_strPoint.c_str());
		}
		else
		{
			std::stringstream stream;
			stream << m_iPointDef;
			m_strPoint =stream.str();
			reqEntrust.dot_diff = atof((char*)m_strPoint.c_str());
		}
		
		//��������Ĭ��
		reqEntrust.eo_flag = _T("2");
		reqEntrust.entrust_type = "0";//�м�ί��
		reqEntrust.hold_id = m_pHoldDetailQueryOut->hold_id;	  //�ֲֵ���	 
		reqEntrust.mid_code = m_QueryUserInfoResult.mid_code;
		reqEntrust.entrust_prop = "0";
		reqEntrust.other_id = "";
		if (m_QueryTraderIDVector.size() > 0)
		{
			reqEntrust.other_id = m_QueryTraderIDVector[0].other_id;
		}
	}
	else if (ECT_Limit == m_eCloseType)//ָ��
	{
		iMarketOrLimit = 1;
		reqSetStopLP.account = strUserID;
		reqSetStopLP.user_session_id = strSessionID;
		reqSetStopLP.user_session_info = strSessionInfo;
		reqSetStopLP.mid_code = m_QueryUserInfoResult.mid_code;

		reqSetStopLP.hold_id = m_pHoldDetailQueryOut->hold_id;	  //�ֲֵ���
		strHoldNumber = m_pHoldDetailQueryOut->hold_id;	  //�ֲֵ���
		// ����
		if (EET_BUY == m_EntrustType)
		{
			reqSetStopLP.entrust_bs = _T("B");
			strBuySell = _T("����");
			strPrice.Format(L"%f", m_dBuyPrice);
		}
		else if (EET_SELL==m_EntrustType)
		{
			reqSetStopLP.entrust_bs = _T("S");
			strBuySell = _T("����");
			strPrice.Format(L"%f", m_dSellPrice);
		}
		else
		{
			//ASSERT(0);
			return;
		}
		// ��Ʒ
		FindCommInfoType(m_DropCommInfo.GetCurSel(), m_CommInfo);
		reqSetStopLP.stock_code = m_CommInfo.chTypeValue;
		m_DropCommInfo.GetWindowText(strComm);
		// �۸�
	//	m_mapButton[IDC_DEF_EDIT_PRICEMARKETPRICE].GetCaption(strPrice);
		UnicodeToUtf8(strPrice, m_strPrice);
		reqSetStopLP.entrust_price = atof((char*)m_strPrice.c_str());
		//ֹ��
		if (m_bCheckStopLoss)
		{
			m_EditStopLoss1.GetWindowText(strStopLoss);
			UnicodeToUtf8(strStopLoss, m_strStopLoss);
			double dStopLoss = atof(m_strStopLoss.c_str());
			CString str = Float2String(dStopLoss, m_iPointCount, FALSE, FALSE);	//��������
			UnicodeToUtf8(str, m_strStopLoss);
			reqSetStopLP.stop_loss = atof((char*)m_strStopLoss.c_str());
		}
		else
		{
			reqSetStopLP.stop_loss = 0;
		}
		//ֹӯ
		if (m_bCheckStopProfit)
		{
			m_EditStopProfit1.GetWindowText(strStopProfit);
			UnicodeToUtf8(strStopProfit, m_strStopProfit);
			double dStopProfit = atof(m_strStopProfit.c_str());
			CString str = Float2String(dStopProfit, m_iPointCount, FALSE, FALSE);	//��������
			UnicodeToUtf8(str, m_strStopProfit);
			reqSetStopLP.stop_profit = atof((char*)m_strStopProfit.c_str());
		}
		else
		{
			reqSetStopLP.stop_profit = 0;
		}
		reqSetStopLP.trade_flag = _T("2");// �µ���ʶ
	}
	else
	{
		//ASSERT(0);
		return;
	}

	//�µ�ǰȷ��
	if (m_bCheckBeforeOrder)
	{
		CString strTip=_T("");
		if (0==iMarketOrLimit)
		{
			if (m_bCheckBack)//�����ֽ���
			{
				CString str = _T("");
				m_EditBack.GetWindowText(str);
				UnicodeToUtf8(str, m_strBack);	//���ֽ�������

				strTip.Format(_T("��Ʒ: %s \r\n��Ʒ�۸�:%s \r\n��Ʒ����: %s \r\n������ʽ: %s \r\n���ֽ�������: %s \r\n\r\nȷ���µ���?"),
					strComm.GetBuffer(),strPrice.GetBuffer(),strQty.GetBuffer(),strBuySell.GetBuffer(),str.GetBuffer());
				strComm.ReleaseBuffer();
				strPrice.ReleaseBuffer();
				strQty.ReleaseBuffer();
				strBuySell.ReleaseBuffer();
				str.ReleaseBuffer();
			}
			else
			{
				strTip.Format(_T("��Ʒ: %s \r\n��Ʒ�۸�: %s \r\n��Ʒ����: %s \r\n������ʽ: %s \r\n\r\nȷ���µ���?"),
					strComm.GetBuffer(),strPrice.GetBuffer(),strQty.GetBuffer(),strBuySell.GetBuffer());
				strComm.ReleaseBuffer();
				strPrice.ReleaseBuffer();
				strQty.ReleaseBuffer();
				strBuySell.ReleaseBuffer();
			}
		}
		else if(1==iMarketOrLimit)//ָ��
		{
			if (m_bCheckStopLoss&&m_bCheckStopProfit)//ֹ��ֹӯ
			{
				strTip.Format(_T("��Ʒ: %s \r\n����: %s \r\n��Ʒ����: %s \r\n������ʽ: %s \r\nֹ���: %s \r\nֹӯ��: %s \r\n\r\nȷ���µ���?"),
					strComm.GetBuffer(),strHoldNumber.GetBuffer(),strQty.GetBuffer(),strBuySell.GetBuffer(),strStopLoss.GetBuffer(),strStopProfit.GetBuffer());
				strComm.ReleaseBuffer();
				strHoldNumber.ReleaseBuffer();
				strQty.ReleaseBuffer();
				strBuySell.ReleaseBuffer();
				strStopLoss.ReleaseBuffer();
				strStopProfit.ReleaseBuffer();
			}
			else if (!m_bCheckStopLoss&&m_bCheckStopProfit)//ֹӯ��ֹ��
			{
				strTip.Format(_T("��Ʒ: %s \r\n����: %s \r\n��Ʒ����: %s \r\n������ʽ: %s \r\nֹӯ��: %s \r\n\r\nȷ���µ���?"),
					strComm.GetBuffer(),strHoldNumber.GetBuffer(),strQty.GetBuffer(),strBuySell.GetBuffer(),strStopProfit.GetBuffer());
				strComm.ReleaseBuffer();
				strHoldNumber.ReleaseBuffer();
				strQty.ReleaseBuffer();
				strBuySell.ReleaseBuffer();
				strStopProfit.ReleaseBuffer();
			}
			else if (m_bCheckStopLoss&&!m_bCheckStopProfit)//ֹ��ֹӯ
			{
				strTip.Format(_T("��Ʒ: %s \r\n����: %s \r\n��Ʒ����: %s \r\n������ʽ: %s \r\nֹ���: %s \r\n\r\nȷ���µ���?"),
					strComm.GetBuffer(),strHoldNumber.GetBuffer(),strQty.GetBuffer(),strBuySell.GetBuffer(),strStopLoss.GetBuffer());
				strComm.ReleaseBuffer();
				strHoldNumber.ReleaseBuffer();
				strQty.ReleaseBuffer();
				strBuySell.ReleaseBuffer();
				strStopLoss.ReleaseBuffer();
			}
			else
			{
				strTip.Format(_T("��Ʒ: %s \r\n����: %s \r\n��Ʒ����: %s \r\n������ʽ: %s \r\n\r\nȷ���µ���?"),
					strComm.GetBuffer(),strHoldNumber.GetBuffer(),strQty.GetBuffer(),strBuySell.GetBuffer());
				strComm.ReleaseBuffer();
				strHoldNumber.ReleaseBuffer();
				strQty.ReleaseBuffer();
				strBuySell.ReleaseBuffer();
			}
		}

		CDlgTip dlg;
		dlg.m_strTitle = L"������Ϣ";
		dlg.m_strTipMsg = strTip;
		dlg.m_eTipType = ETT_TIP;
		dlg.m_pCenterWnd = GetParent();	// ����Ҫ���е��Ĵ���ָ��
		
		if (dlg.DoModal() == IDOK)
		{
			//NULL;
		}
		else
		{
			return;
		}

	}
	
	//������أ���ʾ�ȴ�
	ShowHideControl_MarketPrice(SW_HIDE);
	ShowHideControl_LimitPrice(SW_HIDE);
	m_StaticAfterOk.ShowWindow(SW_SHOW);
	
	// ��������̨����ʾ��Ϣ������д lcq ����
	//E_TipType eTipType = ETT_WAR;


	CString strTip = _T("");
	BOOL bSuc = FALSE;
	if (0==iMarketOrLimit)//�м�
	{
		// ģ�⽻�׵ķ��ֽ����ڿͻ���ʵ��
		if (ETT_TRADE_SIMULATE == m_pTradeLoginInfo->eTradeLoginType)
		{
			int iRet = m_pTradeQuery->ReqEntrust(reqEntrust, strTip);
			if (iRet)
			{	
				bSuc = TRUE;
			}

			// ���ֽ���
			if (m_bCheckBack)
			{
				CString str = _T("");
				m_EditBack.GetWindowText(str);
				UnicodeToUtf8(str, m_strBack);

				CClientReqEntrust reqBackEntrust = reqEntrust;
				reqBackEntrust.eo_flag = _T("0");
				reqBackEntrust.close_price = 0.;
				reqBackEntrust.hold_id = _T("");
				reqBackEntrust.entrust_amount = (int32)(atof((char*)m_strBack.c_str()));

				double dEntrustPrice = atof((char*)m_strPrice.c_str());
				if (!IsBackOpen(reqBackEntrust.stock_code, dEntrustPrice, reqBackEntrust.entrust_amount))
				{
					// ��ʾ
					CDlgTip dlg;
					dlg.m_strTipMsg = _T("�ʽ��㣬���ܷ��ֽ��֣�");
					dlg.m_eTipType = ETT_WAR;
					dlg.m_pCenterWnd = GetParent();
					dlg.DoModal();
				}

				CString strBackTip = _T("");
				int iBack = m_pTradeQuery->ReqEntrust(reqBackEntrust, strBackTip);
				if (!iBack)
				{
					// ��ʾ
					CDlgTip dlg;
					dlg.m_strTipMsg = strBackTip;
					dlg.m_eTipType = ETT_ERR;
					dlg.m_pCenterWnd = GetParent();	// ����Ҫ���е��Ĵ���ָ��
					dlg.DoModal();
				}
				else
				{
					m_pTradeQuery->m_iReqEntrustCnt++;
				}
			}
		}
		else
		{
			CString str = _T("");
			// ���ֽ���
			if (m_bCheckBack)
			{
				m_EditBack.GetWindowText(str);
				UnicodeToUtf8(str, m_strBack);
				reqEntrust.back_amount = (int32)(atof((char*)m_strBack.c_str()));
			}
			int iRet = m_pTradeQuery->ReqEntrust(reqEntrust, strTip);
			if (iRet)
			{	
				bSuc = TRUE;
			}
		}
	}
	else if (1 == iMarketOrLimit)//ָ��
	{
		if (m_pTradeQuery->SetLossProfit(&reqSetStopLP, strTip))
		{
			bSuc = TRUE;
		}
	}
	else
	{
		//ASSERT(0);
		return;
	}
	
	if (0==iMarketOrLimit)
	{
		ShowHideControl_LimitPrice(SW_HIDE);
		ShowHideControl_MarketPrice(SW_SHOW);
		m_StaticAfterOk.ShowWindow(SW_HIDE);
	}
	else if (1==iMarketOrLimit)
	{
		ShowHideControl_MarketPrice(SW_HIDE);
		ShowHideControl_LimitPrice(SW_SHOW);
		m_StaticAfterOk.ShowWindow(SW_HIDE);
	}

	if (!bSuc)
	{
		// ��ʾ
		CDlgTip dlg;
		dlg.m_strTipMsg = strTip;
		dlg.m_eTipType = ETT_ERR;
		dlg.m_pCenterWnd = GetParent();	// ����Ҫ���е��Ĵ���ָ��

		if (IDOK==dlg.DoModal())
		{
		}
		return;	// ���ر�
	}

	if (ECT_Market == m_eCloseType)
	{
		m_pHoldDetailQueryOut = NULL;
	}
}

void CDlgTradeClose::ShowHideControl_MarketPrice(int nCmdShow)
{
	m_EditPoint.ShowWindow(nCmdShow);
	m_SpinPoint.ShowWindow(nCmdShow);
	m_EditBack.ShowWindow(nCmdShow);
	m_SpinBack.ShowWindow(nCmdShow);
	m_StaticBKPoint.ShowWindow(nCmdShow);
	m_StaticBKBack.ShowWindow(nCmdShow);
	m_SpinHand.ShowWindow(nCmdShow);
	m_mapButton[IDC_DEF_BUTTON_MAX].SetVisiable(nCmdShow);
	m_mapButton[IDC_DEF_CHECK_ALLOWPOINT].SetVisiable(nCmdShow);
	m_mapButton[IDC_DEF_CHECK_ALLOWBACK].SetVisiable(nCmdShow);
	m_StaticCheckBack.ShowWindow(nCmdShow);
	m_StaticCheckAllowPoint.ShowWindow(nCmdShow);
	m_StaticTip.ShowWindow(nCmdShow);
}

void CDlgTradeClose::ShowHideControl_LimitPrice(int nCmdShow)
{
	m_mapButton[IDC_DEF_CHECK_STOPLOSS].SetVisiable(nCmdShow);
	m_mapButton[IDC_DEF_CHECK_STOPPROFIT].SetVisiable(nCmdShow);
	m_EditStopLoss1.ShowWindow(nCmdShow);
	m_EditStopLoss2.ShowWindow(nCmdShow);
	m_SpinStopLoss.ShowWindow(nCmdShow);
	m_StaticStopLoss.ShowWindow(nCmdShow);
	m_EditStopProfit1.ShowWindow(nCmdShow);
	m_EditStopProfit2.ShowWindow(nCmdShow);
	m_SpinStopProfit.ShowWindow(nCmdShow);
	m_StaticStopProfit.ShowWindow(nCmdShow);
	m_StaticBKStopProfit1.ShowWindow(nCmdShow);
	m_StaticBKStopProfit2.ShowWindow(nCmdShow);
	m_StaticBKStopLoss1.ShowWindow(nCmdShow);
	m_StaticBKStopLoss2.ShowWindow(nCmdShow);
	m_StaticCheckStopLoss.ShowWindow(nCmdShow);
	m_StaticCheckStopProfit.ShowWindow(nCmdShow);
}

void CDlgTradeClose::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

void CDlgTradeClose::OnPaint() 
{
	CPaintDC dc(this); 
	DrawButtons(&dc);
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDlgTradeClose::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDlgTradeClose::SetTradeBid( iTradeBid *pTradeBid )
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

void CDlgTradeClose::OnLoginStatusChanged(int iCurStatus, int iOldStatus)
{

}

bool32 CDlgTradeClose::OnInitQuery(CString &strTipMsg)
{
	return TRUE;
}

bool32 CDlgTradeClose::OnQueryUserInfo(CString &strTipMsg)
{
	return TRUE;
}

void CDlgTradeClose::OnInitCommInfo()
{
// 	if (NULL==m_pTradeBid)
// 	{
// 		return;
// 	}
// 	if (m_bInitCommInfo)
// 	{
// 		return;
// 	}
// 	// ��ʼ����Ʒ�б�	
// 	const QueryCommInfoResultVector &aQuery = m_pTradeBid->GetCacheCommInfo();
// 	const int iSize = aQuery.size();
// 	for ( int i=0; i<aQuery.size(); ++i )
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
// 	m_DropCommInfo.SetCurSel(0);
// 
// 	m_bInitCommInfo = true;
}

void CDlgTradeClose::OnQueryHoldDetailResponse()
{
	const QueryHoldDetailResultVector &aQuery = m_pTradeBid->GetCacheHoldDetail();
	int iSize = 0;
	iSize = aQuery.size();

	if ((NULL!=m_pHoldDetailQueryOut) && this->IsWindowVisible())
	{
		bool32 bFind = false;

		for (int i=0; i<iSize; i++)
		{
			if (aQuery[i].hold_id == m_pHoldDetailQueryOut->hold_id)
			{
				bFind = true;
				break;
			}
		}

		if (!bFind && (0<iSize))
		{
			if (ETT_TRADE_FIRM == m_pTradeLoginInfo->eTradeLoginType)
			{
				m_pHoldDetailQueryOut = (T_RespQueryHold*)&aQuery[0];
			}
			else
			{
				m_pHoldDetailQueryOut = (T_RespQueryHold*)&aQuery[iSize-1];
			}
			ResetInitControl(m_eCloseType);
			SetPrice();
		}
		return;
	}

	if ((NULL==m_pHoldDetailQueryOut) && (0<iSize))
	{
		if (ETT_TRADE_FIRM == m_pTradeLoginInfo->eTradeLoginType)
		{
			m_pHoldDetailQueryOut = (T_RespQueryHold*)&aQuery[0];
		}
		else
		{
			m_pHoldDetailQueryOut = (T_RespQueryHold*)&aQuery[iSize-1];
		}
		ResetInitControl(m_eCloseType);
		SetPrice();
		return;
	}
	
	if (NULL==m_pHoldDetailQueryOut)
	{
		ResetInitControl(m_eCloseType);
	}
}

void CDlgTradeClose::OnQueryHoldSummaryResponse()
{

}
// void CDlgTradeClose::OnQueryEntrustResponse()
// {
// 
// }

void CDlgTradeClose::OnQueryLimitEntrustResponse()
{
	
}

void CDlgTradeClose::OnQueryDealResponse()
{

}

void CDlgTradeClose::OnQueryCommInfoResponse()
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}
	if (!m_bInitCommInfo)
	{
		// ��ʼ����Ʒ�б�	
		const QueryCommInfoResultVector &aQuery = m_pTradeBid->GetCacheCommInfo();
		for ( unsigned int i=0; i<aQuery.size(); ++i )
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
// 			str.Format(_T("%s"),wstr.c_str());
			m_DropCommInfo.InsertString(i, (LPCTSTR)stOut.stock_name);
		}
		
		m_DropCommInfo.SetCurSel(0);
		CString strName = _T("");
		m_DropCommInfo.GetWindowText(strName);
		m_EditCommInfo.SetWindowText(strName);
		m_bInitCommInfo = true;
	}	

	m_QueryCommInfoResultVector = m_pTradeBid->GetCacheCommInfo();	
}

void CDlgTradeClose::OnQueryTraderIDResponse()
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}

	m_QueryTraderIDVector = m_pTradeBid->GetCacheTraderID();
}

void CDlgTradeClose::OnQueryUserInfoResponse()
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}
	
	m_QueryUserInfoResult = m_pTradeBid->GetCacheUserInfo();
}

void CDlgTradeClose::OnDisConnectResponse()
{
	
}

void CDlgTradeClose::OnReqEntrustResponse(bool32 bShowTip)
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
	if (0 == resp.error_no) // �ɹ�
	{
		eTipType = ETT_SUC;

		// ֪ͨҳ��ˢ��
		if (NULL!=m_pTradeQuery)
		{
			m_pTradeQuery->EntrustResultNotify(true);
			m_pTradeQuery->DoQueryAsy(EQT_QueryHoldSummary);
			// �����˻���Ϣ
			m_pTradeQuery->DoQueryAsy(EQT_QueryUserInfo);
		}

		//��������
		SaveTradeCloseInfo();
	}

	if (bShowTip)
	{
		CDlgTip dlg;
		dlg.m_strTipMsg = resp.error_info;
		dlg.m_eTipType = eTipType;
		dlg.m_pCenterWnd = GetParent();	// ����Ҫ���е��Ĵ���ָ��
		dlg.DoModal();
	}
}

void CDlgTradeClose::OnReqCancelEntrustResponse()
{
	
}

void CDlgTradeClose::OnReqModifyPwdResponse()
{
	
}

void CDlgTradeClose::OnReqSetStopLPResponse()
{
	E_TipType eTipType = ETT_WAR;
	
	CString strTip = _T("");
	BOOL bSuc = FALSE;
	eTipType = ETT_ERR;
	CDlgTip dlg;
	
	CClientRespSetStopLP resp = m_pTradeBid->GetCacheReqSetStopLP();
	if (0 == resp.error_no) // �ɹ�
	{
		bSuc = TRUE;
		eTipType = ETT_SUC;
	}
	dlg.m_strTipMsg = resp.error_info;
	
	dlg.m_eTipType = eTipType;
	dlg.m_pCenterWnd = GetParent();	// ����Ҫ���е��Ĵ���ָ��

	if (IDOK==dlg.DoModal())
	{
	}
	
	if (!bSuc)
	{
		return;	// ���ر�
	}
	
	// ֪ͨҳ��ˢ��
	if (NULL!=m_pTradeQuery)
	{
		m_pTradeQuery->EntrustResultNotify(true);
	}
	
	//��������
	SaveTradeCloseInfo();
	
//	OnOK();		
}

void CDlgTradeClose::OnReqCancelStopLPResponse()
{
	
}

// ��ѯ����
void CDlgTradeClose::OnQueryQuotationResponse()
{
	if ( m_pTradeBid==NULL )
	{
		ASSERT( 0 );
		return;
	}
	
	m_QuotationResultVector = m_pTradeBid->GetCacheQuotation();

 	SetPrice();	// ������£�˳������±༭��
}

//��ȡС�����ȷ��λ��
int CDlgTradeClose::GetPointCount(const double dSpread)
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
		{//����
            iRet = 0;	
		}
		else
		{//С��
			iRet = iLen-2;
		}
	}
	
	return iRet;

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

bool CDlgTradeClose::FindOtherFirm( int iIndex, T_CommInfoType &commInfoType)
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

BOOL CDlgTradeClose::PreTranslateMessage(MSG* pMsg)
{
	CWnd* pWnd = GetFocus();

	if (pMsg->message==WM_KEYDOWN)
	{
		if(pWnd ==GetDlgItem(IDC_DEF_EDIT_PRICEMARKETPRICE)
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
			&&(pWnd!=GetDlgItem(IDC_DEF_SPIN_BACK))
			&&(pWnd!=GetDlgItem(IDC_DEF_EDIT_HAND))
			&&(pWnd!=GetDlgItem(IDC_DEF_EDIT_POINT))
			&&(pWnd!=GetDlgItem(IDC_DEF_EDIT_BACK))
		/*	&&(pWnd!=GetDlgItem(IDC_DEF_EDIT_HAND))*/)
		{
			m_StaticTip.SetWindowText(_T(""));
		}
		
// 		CWnd* pWnd = GetFocus();
// 		CString str=_T("");
// 		if ((pWnd==GetDlgItem(IDC_DEF_SPIN_MAX))
// 			||(pWnd==GetDlgItem(IDC_DEF_EDIT_HAND))
// 			||(pWnd==GetDlgItem(IDC_DEF_STATIC_HAND_BK)))
// 		{
// 			str.Format(_T("��ʾ����ƽ��������С%d�֡����%d�� ��"),m_iQtyMin,m_iQtyMax);
// 			m_StaticTip.SetWindowText(str);
// 		}
// 		else if ((pWnd==GetDlgItem(IDC_DEF_SPIN_POINT))
// 			||(pWnd==GetDlgItem(IDC_DEF_EDIT_POINT)))
// 		{
// 			str.Format(_T("��ʾ�������õ�Χ��С%d�����%d ��"),m_iPointMin,m_iPointMax);
// 			m_StaticTip.SetWindowText(str);
// 		}
// 		else if ((*pWnd==*GetDlgItem(IDC_DEF_SPIN_BACK))
// 			||(*pWnd==*GetDlgItem(IDC_DEF_EDIT_BACK)))
// 		{
// 			str.Format(_T("��ʾ�����ֽ�������С%d�����%d ��"),m_iPointMin,m_iOpenQtyMax);
// 			m_StaticTip.SetWindowText(str);
// 		}
// 		else
// 		{
// 			m_StaticTip.SetWindowText(_T("��ʾ"));
// 		}
// 
// 		UpdateData(FALSE);
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

// ���½���ֹ��ֹӯ�Լ�ռ�ñ�֤����ʾ
void CDlgTradeClose::UpdataStopLossProfitShow()
{
	T_CommInfoType Infotype;
	if (!FindCommInfoType(m_DropCommInfo.GetCurSel(),Infotype))
	{
		//	//ASSERT(0);
		return;
	}
	
	// ���Ӧ��Ʒ��Ϣ
	unsigned int i = 0;
	T_TradeMerchInfo stOutComm;
//	memset(&stOutComm, 0, sizeof(T_TradeMerchInfo));
	for (i=0; i<m_QueryCommInfoResultVector.size(); i++)
	{
		stOutComm = m_QueryCommInfoResultVector[i];
		string strComm;
		UnicodeToUtf8(stOutComm.stock_code, strComm);
		if (_stricmp(Infotype.chTypeValue, strComm.c_str())==0)
		{			
			break;
		}
	}
	//ֹ��ֹӯ
	double dStopLoss = 0;
	double dStopProfit = 0;
	double dPriceIn = 0;
	CString strPriceIn = _T("");
	string strTemp;
	if (EET_BUY==m_EntrustType) // ��
	{
		strPriceIn.Format(L"%.2f", m_dBuyPrice);
	}
	else
	{
		strPriceIn.Format(L"%.2f", m_dSellPrice);
	}
//	m_mapButton[IDC_DEF_EDIT_PRICEMARKETPRICE].GetCaption(strPriceIn);//ȡ��ǰ�м�
	UnicodeToUtf8(strPriceIn, strTemp);
	dPriceIn = atof(strTemp.c_str());

	// �޼�ƽ��ֹ��ֹӯȡֵ��Χ
	// ������
	if (EET_SELL == m_EntrustType)
	{
		// �Ǽ������
		if(_T("")==stOutComm.quote_type || _T("0")==stOutComm.quote_type || _T("2")==stOutComm.quote_type || _T("3")==stOutComm.quote_type)
		{
			dStopLoss = m_dSellPrice - stOutComm.stop_loss_pdd*stOutComm.price_minchange;
			dStopProfit = m_dBuyPrice + stOutComm.stop_profit_pdd*stOutComm.price_minchange;
			
			m_StaticStopLoss.SetWindowText(_T("<"));
			m_StaticStopProfit.SetWindowText(_T(">"));
			iMerchType = 0;
		}
		else if (_T("1")==stOutComm.quote_type) // �������
		{
			dStopLoss = m_dSellPrice + stOutComm.stop_loss_pdd*stOutComm.price_minchange;
			dStopProfit = m_dBuyPrice - stOutComm.stop_profit_pdd*stOutComm.price_minchange;
			
			m_StaticStopLoss.SetWindowText(_T(">"));
			m_StaticStopProfit.SetWindowText(_T("<"));
			iMerchType = 1;
		}
	}
	else  // ������
	{
		// �Ǽ������
		if(_T("")==stOutComm.quote_type || _T("0")==stOutComm.quote_type || _T("2")==stOutComm.quote_type || _T("3")==stOutComm.quote_type)
		{
			dStopLoss = m_dBuyPrice + stOutComm.stop_loss_pdd*stOutComm.price_minchange;
			dStopProfit = m_dSellPrice - stOutComm.stop_profit_pdd*stOutComm.price_minchange;
			
			m_StaticStopLoss.SetWindowText(_T(">"));
			m_StaticStopProfit.SetWindowText(_T("<"));
			iMerchType = 0;
		}
		else if (_T("1")==stOutComm.quote_type) // �������
		{
			dStopLoss = m_dBuyPrice - stOutComm.stop_loss_pdd*stOutComm.price_minchange;
			dStopProfit = m_dSellPrice + stOutComm.stop_profit_pdd*stOutComm.price_minchange;
			
			m_StaticStopLoss.SetWindowText(_T("<"));
			m_StaticStopProfit.SetWindowText(_T(">"));
			iMerchType = 1;
		}
	}
	
	CString strPrice = _T("");
	strPrice = Float2String(dStopLoss, m_iPointCount, FALSE, FALSE);
	m_EditStopLoss2.SetWindowText(strPrice);
// 	CString str = _T("");
// 	m_EditStopLoss1.GetWindowText(str);
// 	UnicodeToUtf8(str, strTemp);
// 	if (atof(strTemp.c_str())>-0.0000001&&atof(strTemp.c_str())<0.0000001)
// 	{
// 		m_EditStopLoss1.SetWindowText(strPrice);
// 	}
	strPrice = Float2String(dStopProfit, m_iPointCount, FALSE, FALSE);
	m_EditStopProfit2.SetWindowText(strPrice);
// 	m_EditStopProfit1.GetWindowText(str);
// 	UnicodeToUtf8(str, strTemp);
// 	if (atof(strTemp.c_str())>-0.0000001&&atof(strTemp.c_str())<0.0000001)
// 	{
// 		m_EditStopProfit1.SetWindowText(strPrice);
// 	}
}


int CDlgTradeClose::WarningMsg(CString &strWarn)
{
	if (L"1" == m_pTradeLoginInfo->StrQuoteFlag)
	{
		CMainFrame *pMain = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
		if (!pMain->m_wndStatusBar.m_bConnect)
		{
			strWarn = _T("û�����������������");
			return -1;
		}
	}

	if ((m_dBuyPrice>-0.000001&&m_dBuyPrice<0.000001) || (m_dSellPrice>-0.000001&&m_dSellPrice<0.000001))
	{
		strWarn = _T("�۸񲻷���������");
		return -1;
	}


	CString str = _T("");
	string strTemp;
	//�ж�����
	m_EditHand.GetWindowText(str);
	UnicodeToUtf8(str, strTemp);
	int iQty = atoi(strTemp.c_str());
	if (iQty>m_iQtyMax||iQty<m_iQtyMin)
	{
		strWarn.Format(_T("��������ȷ��ƽ�������� \r\n��ƽ��������С%d�֡����%d�֣�"),m_iQtyMin,m_iQtyMax);
		return -1;
	}
	
	if (ECT_Market == m_eCloseType)//�м�
	{
		if (m_bCheckAllowPoint)
		{
			//�жϵ��
			m_EditPoint.GetWindowText(str);
			UnicodeToUtf8(str, strTemp);
			int iPoint = atoi(strTemp.c_str());
			if (iPoint>m_iPointMax||iPoint<m_iPointMin)
			{
				strWarn.Format(_T("����д��ȷ��\r\n��Χ��С%d�����%d��"),m_iPointMin,m_iPointMax);
				return -1;
			}
		}
		if (m_bCheckBack)
		{
			//�жϷ��ֽ���
			m_EditBack.GetWindowText(str);
			UnicodeToUtf8(str, strTemp);
			int iBack = atoi(strTemp.c_str());
			if (iBack>m_iOpenQtyMax||iBack<m_iOpenQtyMin)
			{
				strWarn.Format(_T("����д��ȷ���ֽ���������\r\n�����÷��ֽ��ַ�Χ��С%d�����%d��"),m_iOpenQtyMin,m_iOpenQtyMax);
				return -1;
			}
		}
	}
	else if (ECT_Limit == m_eCloseType)//�޼�
	{//�ж�ֹ��ֹӯ�۸�
		
		if (!(m_bCheckStopLoss||m_bCheckStopProfit))//ֹӯֹ�𶼲���ѡ
		{
			strWarn = _T("������ֹӯֹ��������");
			return -1;
		}

		double dStopLoss1 = 0; //ֹ��1
		m_EditStopLoss1.GetWindowText(str);
		UnicodeToUtf8(str, strTemp);
		dStopLoss1 = atof(strTemp.c_str());
		double dStopLoss2 = 0;//ֹ��2
		m_EditStopLoss2.GetWindowText(str);
		UnicodeToUtf8(str, strTemp);
		dStopLoss2 = atof(strTemp.c_str());
		double dStopProfit1 = 0;//ֹӯ1
		m_EditStopProfit1.GetWindowText(str);
		UnicodeToUtf8(str, strTemp);
		dStopProfit1 = atof(strTemp.c_str());
		double dStopProfit2 = 0;//ֹӯ2
		m_EditStopProfit2.GetWindowText(str);
		UnicodeToUtf8(str, strTemp);
		dStopProfit2 = atof(strTemp.c_str());
		
		if (0 == iMerchType)
		{
			if (EET_SELL==m_EntrustType)//��
			{
				if (m_bCheckStopLoss)//ֹ��ѡ
				{
					if (dStopLoss1>=dStopLoss2)
					{
						strWarn = _T("ֹ��۸񲻷���������");
						return -1;
					}
				}
				if (m_bCheckStopProfit)//ֹӯ��ѡ
				{
					if (dStopProfit1<=dStopProfit2)
					{
						strWarn = _T("ֹӯ�۸񲻷���������");
						return -1;
					}
				}
			}
			else if (EET_BUY==m_EntrustType)//��
			{
				if (m_bCheckStopLoss)//ֹ��ѡ
				{
					if (dStopLoss1<=dStopLoss2)
					{
						strWarn = _T("ֹ��۸񲻷���������");
						return -1;
					}
				}
				if (m_bCheckStopProfit)//ֹӯ��ѡ
				{
					if (dStopProfit1>=dStopProfit2)
					{
						strWarn = _T("ֹӯ�۸񲻷���������");
						return -1;
					}
				}
			}
			else
			{
				//
			}
		}
		else
		{
			if (EET_SELL==m_EntrustType)//��
			{
				if (m_bCheckStopLoss)//ֹ��ѡ
				{
					if (dStopLoss1<=dStopLoss2)
					{
						strWarn = _T("ֹ��۸񲻷���������");
						return -1;
					}
				}
				if (m_bCheckStopProfit)//ֹӯ��ѡ
				{
					if (dStopProfit1>=dStopProfit2)
					{
						strWarn = _T("ֹӯ�۸񲻷���������");
						return -1;
					}
				}
			}
			else if (EET_BUY==m_EntrustType)//��
			{
				if (m_bCheckStopLoss)//ֹ��ѡ
				{
					if (dStopLoss1>=dStopLoss2)
					{
						strWarn = _T("ֹ��۸񲻷���������");
						return -1;
					}
				}
				if (m_bCheckStopProfit)//ֹӯ��ѡ
				{
					if (dStopProfit1<=dStopProfit2)
					{
						strWarn = _T("ֹӯ�۸񲻷���������");
						return -1;
					}
				}
			}
			else
			{
				//
			}
		}
	}
	
	return 0;
}

bool32 CDlgTradeClose::IsBackOpen(CString strCode, double dPrice, int iHand)
{
	// ���Ӧ��Ʒ��Ϣ
	T_TradeMerchInfo stOutComm;
	int32 iCommInfoCnt = m_QueryCommInfoResultVector.size();
	for (int32 i=0; i<iCommInfoCnt; i++)
	{
		stOutComm = m_QueryCommInfoResultVector[i];
		if (strCode == stOutComm.stock_code)
		{
			break;
		}
	}

	// ���׵�λ
	string strVal;
	UnicodeToUtf8(stOutComm.trade_unit, strVal);
	double dtradeUnit = atof(strVal.c_str());

	// ����ռ�ñ�֤��
	double dMargin = 0;
	if (dtradeUnit != 0 && stOutComm.margin_ratio != 0)
	{
		dMargin = iHand*dtradeUnit*stOutComm.margin_ratio*dPrice;
	}

	// ��ǰ���ñ�֤��
	double dCurrentMargin = 0;
	dCurrentMargin = m_QueryUserInfoResult.today_enable;

	if (dMargin > dCurrentMargin)
	{
		return false;
	}

	return true;
}

void CDlgTradeClose::LoadTradeCloseInfo()
{
	CString StrV;
	
	m_bCheckAllowPoint = TRUE;
	if ( CEtcXmlConfig::Instance().ReadEtcConfig(KStrTradeCloseSection, KStrTradeCloseKeyPoint, NULL, StrV) )
	{
		m_bCheckAllowPoint = _ttoi(StrV)!=0;
	}
	if (m_bCheckAllowPoint)
	{
		m_mapButton[IDC_DEF_CHECK_ALLOWPOINT].SetImage(m_pImgCheck);
	}
	else
	{
		m_mapButton[IDC_DEF_CHECK_ALLOWPOINT].SetImage(m_pImgUncheck);
	}

	//������ʾ
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
	if ( CEtcXmlConfig::Instance().ReadEtcConfig(KStrTradeCloseSection, KStrTradeCloseKeyCheck, NULL, StrV) )
	{
		m_bCheckBeforeOrder = _ttoi(StrV)!=0;
	}
	if (m_bCheckBeforeOrder)
	{
		m_mapButton[IDC_DEF_CHECK_BEFOREORDER].SetImage(m_pImgCheck);
	}
	else
	{
		m_mapButton[IDC_DEF_CHECK_BEFOREORDER].SetImage(m_pImgUncheck);
	}
}

void CDlgTradeClose::SaveTradeCloseInfo()
{
	CEtcXmlConfig::Instance().WriteEtcConfig(KStrTradeCloseSection, KStrTradeCloseKeyPoint, m_bCheckAllowPoint?_T("1"):_T("0"));
	
	CEtcXmlConfig::Instance().WriteEtcConfig(KStrTradeCloseSection, KStrTradeCloseKeyCheck, m_bCheckBeforeOrder?_T("1"):_T("0"));
}

void CDlgTradeClose::TradeTypeChange(E_TradeType  tradeType)
{
	if (ECT_Market==tradeType)//�м�
	{
		m_iStaticValidity = 0;
		ShowHideControl_LimitPrice(SW_HIDE);
		ShowHideControl_MarketPrice(SW_SHOW);
		m_EditHand.EnableWindow(TRUE);
		m_mapButton[IDC_DEF_BUTTON_MAX].EnableButton(TRUE);
	}
	else if (ECT_Limit==tradeType)
	{
		m_iStaticValidity = 30;
		ShowHideControl_MarketPrice(SW_HIDE);
		ShowHideControl_LimitPrice(SW_SHOW);
		m_EditHand.EnableWindow(FALSE);
		m_mapButton[IDC_DEF_BUTTON_MAX].EnableButton(FALSE);
	}
	else
	{
		//ASSERT(0);
		return;
	}

	m_eCloseType = tradeType;
//	PostMessage(WM_SIZE,0,0);
//	ResetInitControl(m_eCloseType);

	RecalcLayout(TRUE);
}

BOOL CDlgTradeClose::OnEraseBkgnd(CDC* pDC)
{
	return CDialog::OnEraseBkgnd(pDC);
}

void CDlgTradeClose::OnMouseMove(UINT nFlags, CPoint point) 
{
	int32 iButton = TButtonHitTest(point);
	
	if (iButton != m_iXButtonHovering)
	{
		if (INVALID_ID != m_iXButtonHovering)
		{
			m_mapButton[m_iXButtonHovering].MouseLeave();
			m_iXButtonHovering = INVALID_ID;
		}
		
		if (INVALID_ID != iButton)
		{	
			m_iXButtonHovering = iButton;
			m_mapButton[m_iXButtonHovering].MouseHover();
		}
	}

	// ��Ӧ WM_MOUSELEAVE��Ϣ
	TRACKMOUSEEVENT csTME;
	csTME.cbSize	= sizeof (csTME);
	csTME.dwFlags	= TME_LEAVE;
	csTME.hwndTrack = m_hWnd ;		// ָ��Ҫ׷�ٵĴ��� 
	::_TrackMouseEvent (&csTME);	// ����Windows��WM_MOUSELEAVE�¼�֧�� 
	
	CDialog::OnMouseMove(nFlags, point);
}

LRESULT CDlgTradeClose::OnMouseLeave(WPARAM wParam, LPARAM lParam)       
{     
	if (INVALID_ID != m_iXButtonHovering)
	{
		m_mapButton[m_iXButtonHovering].MouseLeave();
		m_iXButtonHovering = INVALID_ID;
	}

	return 0;       
} 

void CDlgTradeClose::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int32 iButton = TButtonHitTest(point);
	if (INVALID_ID != iButton)
	{
		m_mapButton[iButton].LButtonDown();
	}
}

void CDlgTradeClose::OnLButtonUp(UINT nFlags, CPoint point) 
{
	int32 iButton = TButtonHitTest(point);
	if (INVALID_ID != iButton)
	{
		m_mapButton[iButton].LButtonUp();
	}
}

BOOL CDlgTradeClose::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	int32 iID = (int32)wParam;
	std::map<int, CNCButton>::iterator it = m_mapButton.find(iID);
	if (m_mapButton.end() != it)
	{
		switch (iID)
		{
		case IDC_DEF_EDIT_PRICEMARKETPRICE:
			{	
				OnBtnOk();
				break;
			}
		default:
			break;
		}
	}

	return CDialog::OnCommand(wParam, lParam);
}

void CDlgTradeClose::DrawButtons(CDC* pDC)
{
//	Gdiplus::Graphics graphics(pDC->GetSafeHdc());
	CRect rect;
	GetClientRect(&rect);
	
	CBitmap bitmap;	
	CDC MemeDc;
	
	MemeDc.CreateCompatibleDC(pDC);	
	bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	
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
	
	CRect rcPaint;
	pDC->GetClipBox(&rcPaint);
	map<int, CNCButton>::iterator iter;
	CRect rcControl;
	
	// �������а�ť
	for (iter=m_mapButton.begin(); iter!=m_mapButton.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		btnControl.GetRect(rcControl);
		
		// �жϵ�ǰ��ť�Ƿ���Ҫ�ػ�
		if (!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;
		}
		
		btnControl.DrawButton(&graphics);
	}

	pDC->BitBlt( rect.left, rect.top, rect.Width(), rect.Height(), &MemeDc, 0, 0, SRCCOPY);
	
	MemeDc.SelectObject(pOldBitmap);
	MemeDc.DeleteDC();
	bitmap.DeleteObject();
}

int CDlgTradeClose::TButtonHitTest(CPoint point)
{
	map<int, CNCButton>::iterator iter;
	
	// �������а�ť
	for (iter=m_mapButton.begin(); iter!=m_mapButton.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		
		// ��point�Ƿ����ѻ��Ƶİ�ť������
		if (btnControl.PtInButton(point) && btnControl.GetCreate())
		{
			return btnControl.GetControlId();
		}
	}
	
	return INVALID_ID;
}

void CDlgTradeClose::AddButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption)
{
	CNCButton btnControl;
	btnControl.CreateButton(lpszCaption, lpRect, this, pImage, nCount, nID);
	if (IDC_DEF_EDIT_PRICEMARKETPRICE == nID)
	{
		btnControl.SetTextBkgColor(Color(237,124,34),Color(200,124,50),Color(237,124,34));
		btnControl.SetTextFrameColor(Color(237,124,34),Color(200,124,50),Color(237,124,34));
		btnControl.SetTextColor(RGB(255,255,255), RGB(255,255,255), RGB(255,255,255));
	}

	if (IDC_DEF_BUTTON_MAX == nID)
	{
		btnControl.SetTextColor(TEXT_COLOR_STATIC_TIP, TEXT_COLOR_STATIC_TIP, TEXT_COLOR_STATIC_TIP);
	}

	if (IDC_DEF_BUTTON_BUYSELL == nID)
	{
		if (EET_SELL == m_EntrustType)
		{
			btnControl.SetTextColor(RGB(0,255,0), RGB(0,255,0), RGB(0,255,0));
		}
		else
		{
			btnControl.SetTextColor(RGB(255,0,0), RGB(255,0,0), RGB(255,0,0));
		}
		btnControl.SetCheck(TRUE,FALSE);
	}

	m_mapButton[nID] = btnControl;
}