#include "StdAfx.h"
#include "dlgtrendinterval.h"
#include "MerchManager.h"
#include "coding.h"
#include "IoViewTrend.h"
#include "PathFactory.h"
#include "ShareFun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTrendInterval dialog
CDlgTrendInterval::CDlgTrendInterval(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgTrendInterval::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTrendInterval)
	m_StrV1 = _T("");
	m_StrV2 = _T("");
	m_StrV3 = _T("");
	m_StrV4 = _T("");
	m_StrV5 = _T("");
	m_StrAvgVolumn = _T("");
	m_StrBuy = _T("");
	m_fClose = 0.0f;
	m_iCountMin = 0;
	m_iCountTick = 0;
	m_fHigh = 0.0f;
	m_fLow = 0.0f;
	m_StrNei = _T("");
	m_fOpen = 0.0f;
	m_StrRiseRate = _T("");
	m_StrSell = _T("");
	m_StrBigAll = _T("");
	m_StrShakeRate = _T("");
	m_StrTimeBegin = _T("");
	m_StrTimeEnd = _T("");
	m_StrVolumn = _T("");
	m_StrWai = _T("");
	m_iEditLimit = 500;
	m_fAvgPrice = 0.0f;
	m_iRadio = 0;
	m_iLimit = 50;
	//}}AFX_DATA_INIT

	//
	m_bTickEnough = false;
	m_pMerch = NULL;
	m_pIoViewTrend = NULL;

	//
	m_fPricePreClose = 0.;

	//
	m_aKLineSrc.RemoveAll();	
	m_aTicks.RemoveAll();

	//
	m_iLimit = m_iEditLimit;
	m_eListenType = ECTReqMerchTimeSales;

	//
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if ( NULL != pDoc )
	{
		pDoc->m_pAbsCenterManager->AddViewDataListner(this);
	}

	m_bTryReqTick = true;	// 允许尝试更新tick数据

	//
	m_iSortColumn = 3;
	m_bAscend	  = true;
}

CDlgTrendInterval::~CDlgTrendInterval()
{
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if ( NULL != pDoc )
	{
		pDoc->m_pAbsCenterManager->DelViewDataListner(this);
	}
}

void CDlgTrendInterval::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTrendInterval)
	DDX_Control(pDX, IDC_LIST_TICK, m_ListTick);
	DDX_Text(pDX, IDC_STATIC_V1, m_StrV1);
	DDX_Text(pDX, IDC_STATIC_V2, m_StrV2);
	DDX_Text(pDX, IDC_STATIC_V3, m_StrV3);
	DDX_Text(pDX, IDC_STATIC_V4, m_StrV4);
	DDX_Text(pDX, IDC_STATIC_V5, m_StrV5);
	DDX_Text(pDX, IDC_EDIT_AVG_VOLUMN, m_StrAvgVolumn);
	DDX_Text(pDX, IDC_EDIT_BIG_ALL, m_StrBigAll);
	DDX_Text(pDX, IDC_EDIT_BUY, m_StrBuy);
	DDX_Text(pDX, IDC_EDIT_CLOSE, m_fClose);
	DDX_Text(pDX, IDC_EDIT_COUNT_MIN, m_iCountMin);
	DDX_Text(pDX, IDC_EDIT_COUNT_TICK, m_iCountTick);
	DDX_Text(pDX, IDC_EDIT_HIGH, m_fHigh);
	DDX_Text(pDX, IDC_EDIT_LOW, m_fLow);
	DDX_Text(pDX, IDC_EDIT_NEI, m_StrNei);
	DDX_Text(pDX, IDC_EDIT_OPEN, m_fOpen);
	DDX_Text(pDX, IDC_EDIT_RISE_RATE, m_StrRiseRate);
	DDX_Text(pDX, IDC_EDIT_SELL, m_StrSell);
	DDX_Text(pDX, IDC_EDIT_SHAKE_RATE, m_StrShakeRate);
	DDX_Text(pDX, IDC_EDIT_TIME_BEGIN, m_StrTimeBegin);
	DDX_Text(pDX, IDC_EDIT_TIME_END, m_StrTimeEnd);
	DDX_Text(pDX, IDC_EDIT_VOLUMN, m_StrVolumn);
	DDX_Text(pDX, IDC_EDIT_WAI, m_StrWai);
	DDX_Text(pDX, IDC_EDIT_LIMIT, m_iEditLimit);
	DDX_Text(pDX, IDC_EDIT1_AVGPRICE, m_fAvgPrice);
	DDX_Radio(pDX, IDC_RADIO1, m_iRadio);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgTrendInterval, CDialogEx)
	//{{AFX_MSG_MAP(CDlgTrendInterval)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE, OnLimitChange)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_TICK, OnColumnclickListTick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTrendInterval message handlers
static const COLORREF s_KColor1 = RGB(200,0,0);
static const COLORREF s_KColor2 = RGB(120,0,0);
static const COLORREF s_KColor3 = RGB(0,190,0);
static const COLORREF s_KColor4 = RGB(0,110,0);
static const COLORREF s_KColor5 = RGB(0,0,0);

static const char*	s_KStrElementName		= "limit";
static const char*	s_KStrAttriValueName	= "value";

//
static const int32 s_KiMaxHeight	= 20;

//
void CDlgTrendInterval::OnDataRespMerchTimeSales(int iMmiReqId, IN const CMmiRespMerchTimeSales *pMmiRespMerchTimeSales)
{
	UpdateLoaclData();	
}

//
BOOL CDlgTrendInterval::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//
	FromXml();
	UpdateData(FALSE);

	//
	UpdateShow();

	// 
 	m_ImageList.Create(IDB_BITMAP_TRAND_INTERVAL, 16, 1, RGB(255,255,255));
 	m_ListTick.SetImageList(&m_ImageList, LVSIL_SMALL);

 	m_ListTick.InsertColumn(0, L"买卖方向");
 	m_ListTick.InsertColumn(1, L"成交价");
 	m_ListTick.InsertColumn(2, L"成交量");
 	m_ListTick.InsertColumn(3, L"日期");

	int iColumnCount = 4;
	
	for ( int32 i = 0; i < iColumnCount; i++)
	{
		m_ListTick.SetColumnWidth(i, LVSCW_AUTOSIZE);
		int iColumnWidth = m_ListTick.GetColumnWidth(i);
		m_ListTick.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
		int iHeaderWidth = m_ListTick.GetColumnWidth(i); 
		m_ListTick.SetColumnWidth(i, max(iColumnWidth, iHeaderWidth));
	}

	//
	m_Pie.SubclassDlgItem(IDC_STATIC_PIC, this); 	
	m_Pie.Reset();
	m_Pie.ShowWindow(SW_HIDE);

	return TRUE;
}

void CDlgTrendInterval::RequestData()
{
	if ( m_bTickEnough )
	{
		return;
	}

	//
	if ( NULL == m_pMerch )
	{
		return;
	}

	if ( !m_bTryReqTick )
	{
		return;	// 如果收到了与上次相同的tick数据，则不再次请求
	}

	//
	int32 iSizeKLine = m_aKLineSrc.GetSize();
	if ( iSizeKLine < 1 )
	{
		return;
	}

	// 起止时间
	CGmtTime TimeKLineBegin = m_aKLineSrc.GetAt(0).m_TimeCurrent;
	CGmtTime TimeKLineEnd   = m_aKLineSrc.GetAt(iSizeKLine - 1).m_TimeCurrent;

	CGmtTime TimeBeginEx;
	bool32 bSpecial = BeSpecialBegin(TimeKLineBegin, TimeBeginEx);
	if ( bSpecial )
	{
		TimeKLineBegin = TimeBeginEx;
	}

	CGmtTime TimeEndEx;
	bSpecial = BeSpecialEnd(TimeKLineEnd, TimeEndEx);
	if ( bSpecial )
	{
		TimeKLineEnd = TimeEndEx;
	}

	// 请求数据
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	
	//
	if ( NULL != pDoc->m_pAbsCenterManager )
	{
		CMmiReqMerchTimeSales Req;
		
		Req.m_iMarketId		= m_pMerch->m_MerchInfo.m_iMarketId;
		Req.m_StrMerchCode	= m_pMerch->m_MerchInfo.m_StrMerchCode;
		Req.m_eReqTimeType  = ERTYSpecifyTime;
		Req.m_TimeStart		= TimeKLineBegin;
		Req.m_TimeEnd		= TimeKLineEnd;
		
		CMerch *pMerch = NULL;
		if (!pDoc->m_pAbsCenterManager->GetMerchManager().FindMerch(Req.m_StrMerchCode, Req.m_iMarketId, pMerch))
		{
			return;
		}

		pDoc->m_pAbsCenterManager->RequestViewData(&Req);

		m_bTryReqTick = false;	// 尝试请求已经重置
	}
}

bool32 CDlgTrendInterval::BeSpecialBegin(IN const CGmtTime& Time, OUT CGmtTime& TimeBegin)
{
	if ( NULL == m_pMerch )
	{
		return false;
	}
	
	//
	CMarketIOCTimeInfo MarketIOCTimeInfo;
	if ( !m_pMerch->m_Market.GetRecentTradingDay(Time, MarketIOCTimeInfo, m_pMerch->m_MerchInfo) )
	{
		return false;
	}

	//
	if ( MarketIOCTimeInfo.m_TimeOpen.m_Time == Time )
	{
		TimeBegin = MarketIOCTimeInfo.m_TimeInit.m_Time;
		return true;
	}

	//
	return false;
}

bool32 CDlgTrendInterval::BeSpecialEnd(IN const CGmtTime& Time, OUT CGmtTime& TimeEnd)
{
	if ( NULL == m_pMerch )
	{
		return false;
	}
	
	//
	CMarketIOCTimeInfo MarketIOCTimeInfo;
	if ( !m_pMerch->m_Market.GetRecentTradingDay(Time, MarketIOCTimeInfo, m_pMerch->m_MerchInfo) )
	{
		return false;
	}
	
	//
	if ( MarketIOCTimeInfo.m_TimeClose.m_Time == Time )
	{
		TimeEnd = MarketIOCTimeInfo.m_TimeInit.m_Time + CGmtTimeSpan(0, 23, 59, 59);;
		return true;
	}
	
	//
	return false;
}

void CDlgTrendInterval::UpdateLoaclData()
{
	if ( m_bTickEnough )
	{
		return;
	}

	//
	if ( NULL == m_pMerch )
	{
		return;
	}

	//
	int32 iSizeKLine = m_aKLineSrc.GetSize();
	if ( iSizeKLine < 1 )
	{
		return;
	}

	//
	m_aTicks.RemoveAll();

	// 起止时间
	CGmtTime TimeKLineBegin = m_aKLineSrc.GetAt(0).m_TimeCurrent;
	CGmtTime TimeKLineEnd   = m_aKLineSrc.GetAt(iSizeKLine - 1).m_TimeCurrent;

	CGmtTime TimeBeginEx;
	bool32 bSpecial = BeSpecialBegin(TimeKLineBegin, TimeBeginEx);
	if ( bSpecial )
	{
		TimeKLineBegin = TimeBeginEx;
	}
	
	CGmtTime TimeEndEx;
	bSpecial = BeSpecialEnd(TimeKLineEnd, TimeEndEx);
	if ( bSpecial )
	{
		TimeKLineEnd = TimeEndEx;
	}

	// 现有数据的起止时间
	int32 iSizeTick = 0;

	if ( NULL != m_pMerch->m_pMerchTimeSales )
	{
		// 
		iSizeTick = m_pMerch->m_pMerchTimeSales->m_Ticks.GetSize();
		if ( iSizeTick > 1 )
		{
			CGmtTime TimeTickBegin = m_pMerch->m_pMerchTimeSales->m_Ticks.GetAt(0).m_TimeCurrent.m_Time;
			CGmtTime TimeTickEnd   = m_pMerch->m_pMerchTimeSales->m_Ticks.GetAt(iSizeTick - 1).m_TimeCurrent.m_Time;

			if ( !(m_TickStart == m_pMerch->m_pMerchTimeSales->m_Ticks[0])
				|| !(m_TickEnd == m_pMerch->m_pMerchTimeSales->m_Ticks[iSizeTick - 1]) )
			{
				// 仅当tick数据有变更的时候允许尝试再次请求数据，否则一直等到有更新吧
				m_bTryReqTick = true;
				m_TickStart = m_pMerch->m_pMerchTimeSales->m_Ticks[0];
				m_TickEnd	= m_pMerch->m_pMerchTimeSales->m_Ticks[iSizeTick - 1];
			}

			SaveMinute(TimeTickBegin);
			SaveMinute(TimeTickEnd);
			SaveMinute(TimeKLineBegin);
			SaveMinute(TimeKLineEnd);

			CMsTime MsTimeBegin(TimeKLineBegin);
			MsTimeBegin.m_uiMs = 0;
			MsTimeBegin.m_uiIndexMs = 0;
			
			CMsTime MsTimeEnd(TimeKLineEnd);
			MsTimeEnd.m_uiMs = 0;
			MsTimeEnd.m_uiIndexMs = 0;
			
			//
			int32 iPosBegin = m_pMerch->m_pMerchTimeSales->QuickFindTickWithBigOrEqualReferTime(m_pMerch->m_pMerchTimeSales->m_Ticks, MsTimeBegin);
			int32 iPosEnd	= m_pMerch->m_pMerchTimeSales->QuickFindTickWithSmallOrEqualReferTime(m_pMerch->m_pMerchTimeSales->m_Ticks, MsTimeEnd);

			// 数据足够了
			if ( TimeTickBegin <= TimeKLineBegin && TimeTickEnd >= TimeKLineEnd )
			{
				//
				m_bTickEnough = true;
			}

			int32 iSizeNew = iPosEnd - iPosBegin + 1;
			if ( iSizeNew <= 0 )
			{
				//ASSERT(0);
				return;
			}

			m_aTicks.SetSize(iSizeNew);
			
			CTick* pTickSrc	  = (CTick*)m_pMerch->m_pMerchTimeSales->m_Ticks.GetData();
			CTick* pTickLocal = (CTick*)m_aTicks.GetData();
			
			//
			memcpyex(pTickLocal, pTickSrc + iPosBegin, iSizeNew * sizeof(CTick));								
		}
	}

	//
	CalcStatisticData();

	//
	UpdateShow();
}

void CDlgTrendInterval::CalcStatisticData()
{
	// 统计 Tick 数据
	CTick* pTick = (CTick*)m_aTicks.GetData();
	int32 iSizeTick  = m_aTicks.GetSize();
	
	//
	if ( NULL == pTick || iSizeTick <= 0 )
	{
		return;
	}

	//
	m_stResult.Clear();
	
	//
	float fVolumnAll = 0.;
	m_stResult.m_fMinPrice = FLT_MAX;
	m_stResult.m_fMaxPrice = FLT_MIN;

	for ( int32 i = 0; i < iSizeTick; i++ )
	{
		CTick stTick = pTick[i];
		
		//
		fVolumnAll += stTick.m_fVolume;
		
		//
		m_stResult.m_fMinPrice = m_stResult.m_fMinPrice < stTick.m_fPrice ? m_stResult.m_fMinPrice : stTick.m_fPrice;
		m_stResult.m_fMaxPrice = m_stResult.m_fMaxPrice > stTick.m_fPrice ? m_stResult.m_fMaxPrice : stTick.m_fPrice;
		
		//
		if ( CTick::ETKBuy == stTick.m_eTradeKind )
		{
			// 内盘
			m_stResult.m_fNeipan += stTick.m_fVolume;

			// 这个价格对应的数据累加
			m_stResult.m_mapBuySell[stTick.m_fPrice].m_fBuyVolumn += stTick.m_fVolume;
		}
		else if ( CTick::ETKSell == stTick.m_eTradeKind )
		{
			// 外盘
			m_stResult.m_fWaipan += stTick.m_fVolume;			

			// 这个价格对应的数据累加
			m_stResult.m_mapBuySell[stTick.m_fPrice].m_fSellVolumn += stTick.m_fVolume;
		}
		else if ( CTick::ETKUnKnown == stTick.m_eTradeKind )
		{
			// 未知			
			m_stResult.m_iCountUnKnown	+= 1;
			m_stResult.m_fVolumnUnKnown	+= stTick.m_fVolume;
		}
		
		//
		if ( CTick::ETKUnKnown != stTick.m_eTradeKind )		
		{
			if ( stTick.m_fVolume >= m_iLimit )
			{
				// 大单
				m_stResult.m_iCountBig += 1;
				m_stResult.m_fVolumnBig+= stTick.m_fVolume;
				
				if ( CTick::ETKSell == stTick.m_eTradeKind)
				{
					// 主买大单				
					m_stResult.m_iCountBigBuy	 += 1;
					m_stResult.m_fVolumnBigBuy += stTick.m_fVolume;					
				}
				else if ( CTick::ETKBuy == stTick.m_eTradeKind )
				{
					// 主卖大单
					m_stResult.m_iCountBigSell += 1;
					m_stResult.m_fVolumnBigSell+= stTick.m_fVolume;
				}
			}
			else
			{
				// 小单
				m_stResult.m_iCountSmall += 1;
				m_stResult.m_fVolumnSmall += stTick.m_fVolume;
				
				if ( CTick::ETKSell == stTick.m_eTradeKind)
				{
					// 主买小单				
					m_stResult.m_iCountSmallBuy	 += 1;
					m_stResult.m_fVolumnSmallBuy += stTick.m_fVolume;					
				}
				else if ( CTick::ETKBuy == stTick.m_eTradeKind )
				{
					// 主卖小单
					m_stResult.m_iCountSmallSell += 1;
					m_stResult.m_fVolumnSmallSell+= stTick.m_fVolume;
				}
			}
		}
	}	
	
	// 主买大单比例
	m_stResult.m_fRateBigBuy = m_stResult.m_fVolumnBigBuy / fVolumnAll;
	
	// 主买小单比例
	m_stResult.m_fRateSmallBuy = m_stResult.m_fVolumnSmallBuy / fVolumnAll;
	
	// 主卖大单比例	
	m_stResult.m_fRateBigSell = m_stResult.m_fVolumnBigSell / fVolumnAll;
	
	// 主卖小单比例
	m_stResult.m_fRateSmallSell = m_stResult.m_fVolumnSmallSell / fVolumnAll;
	
	// 未知成交
	m_stResult.m_fRateUnKnown	= m_stResult.m_fVolumnUnKnown / fVolumnAll;
	
	// 最大买卖量:
	for ( map<float, T_TrendIntervalBS>::iterator it = m_stResult.m_mapBuySell.begin(); it != m_stResult.m_mapBuySell.end(); ++it )
	{
		float fBuyVolumn	= it->second.m_fBuyVolumn;
		float fSellVolumn	= it->second.m_fSellVolumn;

		//
		m_stResult.m_fMaxBuyVolumn	= m_stResult.m_fMaxBuyVolumn > fBuyVolumn ? m_stResult.m_fMaxBuyVolumn : fBuyVolumn;
		m_stResult.m_fMaxSellVolumn	= m_stResult.m_fMaxSellVolumn> fSellVolumn ? m_stResult.m_fMaxSellVolumn : fSellVolumn;
	}
}

void CDlgTrendInterval::UpdateShow(bool32 bForceUpdate/* = false*/)
{
	UpdateData(TRUE);

	//
	if ( 0 == m_iRadio )
	{
		ShowPage1();
	}
	else if ( 1 == m_iRadio )
	{
		ShowPage2(bForceUpdate);
	}
	else if ( 2 == m_iRadio )
	{
		ShowPage3();
	}
}

void CDlgTrendInterval::ShowPage1()
{
	//
	{
		GetDlgItem(IDC_LIST_TICK)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_STATIC_V1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_V2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_V3)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_V4)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_V5)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_AVG_VOLUMN)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_BIG_ALL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_BUY)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_CLOSE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_COUNT_MIN)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_COUNT_TICK)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_HIGH)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_LOW)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_NEI)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_OPEN)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_RISE_RATE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_SELL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_SHAKE_RATE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_VOLUMN)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_WAI)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_LIMIT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT1_AVGPRICE)->ShowWindow(SW_SHOW);
		
		//
		GetDlgItem(IDC_STATIC_1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_3)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_4)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_5)->ShowWindow(SW_SHOW);

		//
		GetDlgItem(IDC_STATIC2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC3)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC4)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC5)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC6)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC7)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC8)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC9)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC10)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC11)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC12)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC13)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC14)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC16)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC17)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC18)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC19)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC20)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC21)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC22)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC23)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC24)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC25)->ShowWindow(SW_SHOW);		
		GetDlgItem(IDC_STATIC_PIC)->ShowWindow(SW_SHOW);
	}
	
	//
	RequestData();

	CTick* pTick = (CTick*)m_aTicks.GetData();;
	int32 iSizeTick = m_aTicks.GetSize();
	
	//
	if ( NULL == pTick || iSizeTick <= 0 )
	{
		return;
	}

	// 
	int32 iSizeKLine = m_aKLineSrc.GetSize();
		
	if ( iSizeKLine < 1 )
	{
		return;
	}

	CKLine* pKLine = (CKLine*)m_aKLineSrc.GetData();

	//
	CGmtTime TimeBegin = pKLine[0].m_TimeCurrent;
	CGmtTime TimeEnd   = pKLine[iSizeKLine-1].m_TimeCurrent;

	CGmtTimeSpan timespan(0, 8, 0, 0);
	TimeBegin	   += timespan;
	TimeEnd		   += timespan;

	//
	m_StrTimeBegin.Format(L"%02d/%02d %02d:%02d", TimeBegin.GetMonth(), TimeBegin.GetDay(), TimeBegin.GetHour(), TimeBegin.GetMinute());  
	m_StrTimeEnd.Format(L"%02d/%02d %02d:%02d", TimeEnd.GetMonth(), TimeEnd.GetDay(), TimeEnd.GetHour(), TimeEnd.GetMinute());

	// 开盘
	m_fOpen		   = pKLine[0].m_fPriceOpen;
	
	// 收盘
	m_fClose	   = pKLine[iSizeKLine - 1].m_fPriceClose;
	
	// 
	float fVolumn = 0.;
	float fAmount = 0.;
	
	//
	m_fHigh = pKLine[0].m_fPriceHigh;
	m_fLow  = pKLine[0].m_fPriceLow;

	for ( int32 i = 0; i < iSizeKLine; i++ )
	{
		m_fHigh = m_fHigh > pKLine[i].m_fPriceHigh ? m_fHigh : pKLine[i].m_fPriceHigh;
		m_fLow  = m_fLow < pKLine[i].m_fPriceLow  ? m_fLow : pKLine[i].m_fPriceLow;
		
		// 成交量
		fVolumn += pKLine[i].m_fVolume;
		fAmount += pKLine[i].m_fAmount;
	}
	
	// 成交量
	m_StrVolumn = Float2String(fVolumn, 2, true);

	// 每笔均量
	m_StrAvgVolumn = Float2String(fVolumn / iSizeTick, 2, true);

	// 加权均价: 成交额/成交量
	if ( 0. != fVolumn )
	{
		m_fAvgPrice = fAmount / fVolumn;
		m_fAvgPrice/= 100.0f;
	}

	// 区间涨幅
	float fRiseRate = 0.;
	float fPreClose = m_fPricePreClose;
	if ( 0. == fPreClose )
	{
		fPreClose = pKLine[0].m_fPriceClose;
	}

	if ( 0. != fPreClose )
	{
		fRiseRate = (pKLine[iSizeKLine-1].m_fPriceClose - fPreClose) / fPreClose;
		fRiseRate*= 100.0f;
		m_StrRiseRate.Format(L"%.2f(%.2f%%)", (pKLine[iSizeKLine-1].m_fPriceClose - fPreClose), fRiseRate);
	}
	else
	{
		m_StrRiseRate.Empty();
	}
	
	// 区间振幅
	float fShakeRate = 0.;
	if ( 0. != m_fLow )
	{
		fShakeRate = (m_fHigh - m_fLow) / m_fLow;
		fShakeRate*= 100.0f;
		m_StrShakeRate.Format(L"%.2f(%.2f%%)", (m_fHigh - m_fLow), fShakeRate);
	}
	
	// 分钟数目
	m_iCountMin = iSizeKLine;

	// 分笔数目
	m_iCountTick = iSizeTick;

	// 统计信息

	// 内盘
	m_StrNei = Float2String(m_stResult.m_fNeipan, 2, true);

	// 外盘
	m_StrWai = Float2String(m_stResult.m_fWaipan, 2, true);

	// 大单成交
	m_StrBigAll.Format(L"%s[%d个]", Float2String(m_stResult.m_fVolumnBig, 2, true, false).GetBuffer(), m_stResult.m_iCountBig);

	// 主买大单
	m_StrBuy.Format(L"%s[%d个]", Float2String(m_stResult.m_fVolumnBigBuy, 2, true, false).GetBuffer(), m_stResult.m_iCountBigBuy);

	// 主卖大单
	m_StrSell.Format(L"%s[%d个]", Float2String(m_stResult.m_fVolumnBigSell, 2, true, false).GetBuffer(), m_stResult.m_iCountBigSell);

	// 主买大单比例
	m_StrV1.Format(L"%.0f%%", m_stResult.m_fRateBigBuy*100.f);

	// 主买小单比例
	m_StrV2.Format(L"%.0f%%", m_stResult.m_fRateSmallBuy*100.f);

	// 主卖大单比例	
	m_StrV3.Format(L"%.0f%%", m_stResult.m_fRateBigSell*100.f);
	
	// 主卖小单比例
	m_StrV4.Format(L"%.0f%%", m_stResult.m_fRateSmallSell*100.f);

	// 未知成交
	m_StrV5.Format(L"%.0f%%", m_stResult.m_fRateUnKnown*100.f);

	UpdateData(FALSE);
	//RedrawWindow();
	RedrawAsync();
}

void CDlgTrendInterval::ShowPage2(bool32 bForceUpdate /*= false*/)
{
	{
		GetDlgItem(IDC_LIST_TICK)->ShowWindow(SW_SHOW);
		
		GetDlgItem(IDC_STATIC_V1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_V2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_V3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_V4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_V5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_AVG_VOLUMN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_BIG_ALL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_BUY)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_CLOSE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_COUNT_MIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_COUNT_TICK)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_HIGH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_LOW)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_NEI)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_OPEN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_RISE_RATE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_SELL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_SHAKE_RATE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_VOLUMN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_WAI)->ShowWindow(SW_HIDE);		
		GetDlgItem(IDC_EDIT1_AVGPRICE)->ShowWindow(SW_HIDE);
		
		//
		GetDlgItem(IDC_STATIC_1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_5)->ShowWindow(SW_HIDE);

		//
		GetDlgItem(IDC_STATIC2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC9)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC10)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC11)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC12)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC13)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC14)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC16)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC17)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC18)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC19)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC20)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC21)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC22)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC23)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC24)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC25)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_PIC)->ShowWindow(SW_HIDE);
	}

	RequestData();

	//
	if ( m_bTickEnough && m_ListTick.GetItemCount() > 0 && !bForceUpdate )
	{
		return;
	}

	//
	m_ListTick.DeleteAllItems();

	//
	for ( int32 i = 0; i < m_aTicks.GetSize(); i++ )
	{
		CTick stTick = m_aTicks[i];
		
		if ( stTick.m_fVolume < m_iLimit )
		{
			continue;
		}
		//		
		int32 iIndex = -1;

		int32 iCount = m_ListTick.GetItemCount();
		if ( CTick::ETKSell == stTick.m_eTradeKind)
		{
			// 主买
			iIndex = m_ListTick.InsertItem(iCount + 1, L"主买", 1);
		}
		else if ( CTick::ETKBuy == stTick.m_eTradeKind )
		{
			// 主卖			
			iIndex = m_ListTick.InsertItem(iCount + 1, L"主卖", 0);
		}
		else if ( CTick::ETKUnKnown == stTick.m_eTradeKind )
		{
			// 中性
			iIndex = m_ListTick.InsertItem(iCount + 1, L"中性", 0);
		}

		//
		if ( -1 == iIndex )
		{
			continue;
		}

		// 成交价
		m_ListTick.SetItemText(iIndex, 1, Float2String(stTick.m_fPrice, 2));
		
		// 成交量
		m_ListTick.SetItemText(iIndex, 2, Float2String(stTick.m_fVolume, 0));
		
		// 时间
		CString StrTime;
		StrTime.Format(L"%02d/%02d %02d:%02d", stTick.m_TimeCurrent.m_Time.GetMonth(), stTick.m_TimeCurrent.m_Time.GetDay(), stTick.m_TimeCurrent.m_Time.GetHour() + 8, stTick.m_TimeCurrent.m_Time.GetMinute());
		m_ListTick.SetItemText(iIndex, 3, StrTime);

		//
		ListCmpData* pItemData = new ListCmpData();
		pItemData->m_iFlag	= (int32)stTick.m_eTradeKind;
		pItemData->m_fPrice	= stTick.m_fPrice;
		pItemData->m_fVolume= stTick.m_fVolume;
		pItemData->m_lTime  = stTick.m_TimeCurrent.m_Time.GetTime();

		//
		m_ListTick.SetItemData(iIndex, (DWORD)pItemData);
	}	

	// 排序, 默认按时间降序排列
	Sort(3, false);
}

void CDlgTrendInterval::ShowPage3()
{
	{
		GetDlgItem(IDC_LIST_TICK)->ShowWindow(SW_HIDE);
		
		GetDlgItem(IDC_STATIC_V1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_V2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_V3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_V4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_V5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_AVG_VOLUMN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_BIG_ALL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_BUY)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_CLOSE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_COUNT_MIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_COUNT_TICK)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_HIGH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_LOW)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_NEI)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_OPEN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_RISE_RATE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_SELL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_SHAKE_RATE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_VOLUMN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_WAI)->ShowWindow(SW_HIDE);		
		GetDlgItem(IDC_EDIT1_AVGPRICE)->ShowWindow(SW_HIDE);	
		
		//
		GetDlgItem(IDC_STATIC_1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_5)->ShowWindow(SW_HIDE);

		//
		GetDlgItem(IDC_STATIC2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC9)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC10)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC11)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC12)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC13)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC14)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC16)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC17)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC18)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC19)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC20)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC21)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC22)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC23)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC24)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC25)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_PIC)->ShowWindow(SW_HIDE);
	}

	RequestData();

	//
	//RedrawWindow();
	//Invalidate();
	RedrawAsync();
}

void CDlgTrendInterval::OnRadio1()
{
	UpdateShow();
	Invalidate();
	KillTimer(1);
}

void CDlgTrendInterval::OnRadio2()
{
	UpdateShow();
}

void CDlgTrendInterval::OnRadio3()
{
	UpdateShow();
	Invalidate();
	KillTimer(1);
}

void CDlgTrendInterval::OnLimitChange()
{
	// 
	UpdateData(TRUE);

	m_iLimit = m_iEditLimit;

	//
	if ( m_iLimit < 5 )
	{
		m_iLimit = 5;
	}

	m_iEditLimit = m_iLimit;

	//
	ToXml();

	//
	CalcStatisticData();	
	
	//
	UpdateShow(true);
}

void CDlgTrendInterval::OnPaint()
{
	CPaintDC dc(this);

	if ( 0 == m_iRadio )
	{
		// 画示例颜色框
		CPen pen1(PS_SOLID, 1, s_KColor1);
		CPen pen2(PS_SOLID, 1, s_KColor2);
		CPen pen3(PS_SOLID, 1, s_KColor3);
		CPen pen4(PS_SOLID, 1, s_KColor4);	
		CPen pen5(PS_SOLID, 1, s_KColor5);
		CPen pen0(PS_SOLID, 1, RGB(255,255,255));

		CBrush brush1;
		brush1.CreateSolidBrush(s_KColor1);

		CBrush brush2(s_KColor2);
		CBrush brush3(s_KColor3);
		CBrush brush4(s_KColor4);
		CBrush brush5(s_KColor5);

		CRect Rect1, Rect2, Rect3, Rect4, Rect5;

		GetDlgItem(IDC_STATIC_1)->GetWindowRect(Rect1);
		ScreenToClient(Rect1);

		GetDlgItem(IDC_STATIC_2)->GetWindowRect(Rect2);
		ScreenToClient(Rect2);

		GetDlgItem(IDC_STATIC_3)->GetWindowRect(Rect3);
		ScreenToClient(Rect3);

		GetDlgItem(IDC_STATIC_4)->GetWindowRect(Rect4);
		ScreenToClient(Rect4);

		GetDlgItem(IDC_STATIC_5)->GetWindowRect(Rect5);
		ScreenToClient(Rect5);
		
		dc.FillSolidRect(Rect1, s_KColor1);
		dc.FillSolidRect(Rect2, s_KColor2);
		dc.FillSolidRect(Rect3, s_KColor3);
		dc.FillSolidRect(Rect4, s_KColor4);
		dc.FillSolidRect(Rect5, s_KColor5);
		
		 /*
		// 画饼图
		CRect Rect;
		GetDlgItem(IDC_STATIC_PIC)->GetWindowRect(Rect);
		ScreenToClient(Rect);

		dc.SelectObject(GetFont());

		CPoint ptCenter = Rect.CenterPoint();
		int32 iRadius = min(Rect.Width(), Rect.Height()) / 2;
		
		SIZE sizeCircle;
		sizeCircle.cx = iRadius;
		sizeCircle.cy = iRadius;
		CRect RectCircle(ptCenter, sizeCircle);

		// 第一部分, 主买大单
		CPen* pOldPen = dc.SelectObject(&pen0);
		CBrush* pOldBrush = dc.SelectObject(&brush1);

		float fAngerBegin = 0;
		float fAngleNow = m_stResult.m_fRateBigBuy * 360.0f;

		dc.BeginPath();
		dc.MoveTo(ptCenter);		
		dc.AngleArc(ptCenter.x, ptCenter.y, iRadius, fAngerBegin, fAngleNow);
		dc.LineTo(ptCenter);
		dc.EndPath();
		//dc.FillPath();
		dc.SelectClipPath(RGN_COPY);
		dc.FillRect(Rect, &brush1);

		CPoint pt(0,0);
		const double pi = 3.1415926535;
		dc.SetTextColor(RGB(255,255,255));
		dc.SetBkMode(TRANSPARENT);
		const CSize SizeText = dc.GetTextExtent(_T("100%"));
		CRgn rgnClip;
		CRect RectText;
		RectText.SetRect(0,0, SizeText.cx, SizeText.cy);
		pt.x = ptCenter.x + (int32)(iRadius*0.55*cos((fAngerBegin + fAngleNow/2)/180*pi));
		pt.y = ptCenter.y - (int32)(iRadius*0.55*sin((fAngerBegin + fAngleNow/2)/180*pi));
		RectText.OffsetRect(pt - RectText.CenterPoint());
		
		rgnClip.DeleteObject();
		rgnClip.CreateRectRgn(0,0,0,0);
		::GetClipRgn(dc.m_hDC, (HRGN)rgnClip.m_hObject);
		if ( IsRectInRgn(rgnClip, RectText) )
		{
			CString Str;
			Str.Format(_T("%0.0f%%"), m_stResult.m_fRateBigBuy*100);
			dc.DrawText(Str, RectText, DT_VCENTER |DT_SINGLELINE |DT_CENTER);
		}


		// 第二部分, 主买小单
		pOldPen = dc.SelectObject(&pen0);
		pOldBrush = dc.SelectObject(&brush2);
	
		fAngerBegin += fAngleNow;
		fAngleNow = m_stResult.m_fRateSmallBuy * 360.0f;
		
		dc.BeginPath();
		dc.MoveTo(ptCenter);
		dc.AngleArc(ptCenter.x, ptCenter.y, iRadius, fAngerBegin, fAngleNow);	
		dc.LineTo(ptCenter);
		dc.EndPath();
		//dc.FillPath();
		dc.SelectClipPath(RGN_COPY);
		dc.FillRect(Rect, &brush2);

		RectText.SetRect(0,0, SizeText.cx, SizeText.cy);
		pt.x = ptCenter.x + (int32)(iRadius*0.55*cos((fAngerBegin + fAngleNow/2)/180*pi));
		pt.y = ptCenter.y - (int32)(iRadius*0.55*sin((fAngerBegin + fAngleNow/2)/180*pi));
		RectText.OffsetRect(pt - RectText.CenterPoint());
		
		rgnClip.DeleteObject();
		rgnClip.CreateRectRgn(0,0,0,0);
		::GetClipRgn(dc.m_hDC, (HRGN)rgnClip.m_hObject);
		if ( IsRectInRgn(rgnClip, RectText) )
		{
			CString Str;
			Str.Format(_T("%0.0f%%"), m_stResult.m_fRateSmallBuy*100);
			dc.DrawText(Str, RectText, DT_VCENTER |DT_SINGLELINE |DT_CENTER);
		}
		
		//　第三部分, 主卖大单
		pOldPen = dc.SelectObject(&pen0);
		pOldBrush = dc.SelectObject(&brush3);
		
		fAngerBegin += fAngleNow;
		fAngleNow = m_stResult.m_fRateBigSell * 360.0f;
		
		
		dc.BeginPath();
		dc.MoveTo(ptCenter);
		dc.AngleArc(ptCenter.x, ptCenter.y, iRadius, fAngerBegin, fAngleNow);
		dc.LineTo(ptCenter);
		dc.EndPath();
		//dc.FillPath();
		dc.SelectClipPath(RGN_COPY);
		dc.FillRect(Rect, &brush3);

		RectText.SetRect(0,0, SizeText.cx, SizeText.cy);
		pt.x = ptCenter.x + (int32)(iRadius*0.55*cos((fAngerBegin + fAngleNow/2)/180*pi));
		pt.y = ptCenter.y - (int32)(iRadius*0.55*sin((fAngerBegin + fAngleNow/2)/180*pi));
		RectText.OffsetRect(pt - RectText.CenterPoint());
		
		rgnClip.DeleteObject();
		rgnClip.CreateRectRgn(0,0,0,0);
		::GetClipRgn(dc.m_hDC, (HRGN)rgnClip.m_hObject);
		if ( IsRectInRgn(rgnClip, RectText) )
		{
			CString Str;
			Str.Format(_T("%0.0f%%"), m_stResult.m_fRateBigSell*100);
			dc.DrawText(Str, RectText, DT_VCENTER |DT_SINGLELINE |DT_CENTER);
		}

		//　第四部分, 主卖小单
		pOldPen = dc.SelectObject(&pen0);
		pOldBrush = dc.SelectObject(&brush4);
		
		fAngerBegin += fAngleNow;
		fAngleNow = m_stResult.m_fRateSmallSell * 360.0f;
		
		dc.BeginPath();
		dc.MoveTo(ptCenter);
		dc.AngleArc(ptCenter.x, ptCenter.y, iRadius, fAngerBegin, fAngleNow);
		dc.LineTo(ptCenter);
		dc.EndPath();
		//dc.FillPath();
		dc.SelectClipPath(RGN_COPY);
		dc.FillRect(Rect, &brush4);

		RectText.SetRect(0,0, SizeText.cx, SizeText.cy);
		pt.x = ptCenter.x + (int32)(iRadius*0.55*cos((fAngerBegin + fAngleNow/2)/180*pi));
		pt.y = ptCenter.y - (int32)(iRadius*0.55*sin((fAngerBegin + fAngleNow/2)/180*pi));
		RectText.OffsetRect(pt - RectText.CenterPoint());
		
		rgnClip.DeleteObject();
		rgnClip.CreateRectRgn(0,0,0,0);
		::GetClipRgn(dc.m_hDC, (HRGN)rgnClip.m_hObject);
		if ( IsRectInRgn(rgnClip, RectText) )
		{
			CString Str;
			Str.Format(_T("%0.0f%%"), m_stResult.m_fRateSmallSell*100);
			dc.DrawText(Str, RectText, DT_VCENTER |DT_SINGLELINE |DT_CENTER);
		}

		//　第五部分, 未知
		pOldPen = dc.SelectObject(&pen0);
		pOldBrush = dc.SelectObject(&brush5);
		
		fAngerBegin += fAngleNow;
		fAngleNow = 360 - fAngerBegin;
		
		dc.BeginPath();
		dc.MoveTo(ptCenter);
		dc.AngleArc(ptCenter.x, ptCenter.y, iRadius, fAngerBegin, fAngleNow);
		dc.LineTo(ptCenter);
		dc.EndPath();
		//dc.FillPath();
		dc.SelectClipPath(RGN_COPY);
		dc.FillRect(Rect, &brush5);

		RectText.SetRect(0,0, SizeText.cx, SizeText.cy);
		pt.x = ptCenter.x + (int32)(iRadius*0.55*cos((fAngerBegin + fAngleNow/2)/180*pi));
		pt.y = ptCenter.y - (int32)(iRadius*0.55*sin((fAngerBegin + fAngleNow/2)/180*pi));
		RectText.OffsetRect(pt - RectText.CenterPoint());
		
		rgnClip.DeleteObject();
		rgnClip.CreateRectRgn(0,0,0,0);
		::GetClipRgn(dc.m_hDC, (HRGN)rgnClip.m_hObject);
		if ( IsRectInRgn(rgnClip, RectText) )
		{
			CString Str;
			Str.Format(_T("%0.0f%%"), fAngleNow/360*100);
			dc.DrawText(Str, RectText, DT_VCENTER |DT_SINGLELINE |DT_CENTER);
		}
		dc.SelectClipRgn(NULL);

		//
		dc.SelectObject(pOldPen);
		dc.SelectObject(pOldBrush);
		*/
		//
		{
			//填充饼图
			m_Pie.Reset();
			m_Pie.ShowWindow(SW_SHOW);

			//
			CString tmpStr = L"";
			// tmpStr.Format(L"主买大单:%2.0f%%", 100.0f * m_stResult.m_fRateBigBuy);
			m_Pie.AddPiece(RGB(200,0,0), RGB(255,255,255), m_stResult.m_fRateBigBuy, tmpStr);
			
 			// tmpStr.Format(L"主买小单:%2.0f%%", 100.0f * m_stResult.m_fRateSmallBuy);
 			m_Pie.AddPiece(RGB(120,0,0), RGB(255,255,255), m_stResult.m_fRateSmallBuy, tmpStr);
 			
 			// tmpStr.Format(L"主卖大单:%2.0f%%", 100.0f * m_stResult.m_fRateBigSell);
 			m_Pie.AddPiece(RGB(0,190,0), RGB(255,255,255), m_stResult.m_fRateBigSell, tmpStr);	
 			
 			// tmpStr.Format(L"主卖小单:%2.0f%%", 100.0f * m_stResult.m_fRateSmallSell);
 			m_Pie.AddPiece(RGB(0,110,0), RGB(255,255,255), m_stResult.m_fRateSmallSell, tmpStr);
 			
 			// tmpStr.Format(L"不明成交:%2.0f%%", 100.0f * m_stResult.m_fRateUnKnown);
 			m_Pie.AddPiece(RGB(0,0,0), RGB(255,255,255), m_stResult.m_fRateUnKnown, tmpStr);
		}
	}
	else if ( 1 == m_iRadio )
	{
		//
	}
	else if ( 2 == m_iRadio )
	{
		if ( m_stResult.m_fMaxBuyVolumn <= 0. || m_stResult.m_fMaxSellVolumn <= 0. )
		{
			return;
		}

		//
		CRect rectList;
		CWnd* pWnd = GetDlgItem(IDC_STATIC_PIC_PAGE3);
		if ( NULL == pWnd )
		{
			//ASSERT(0);
			return;
		}

		pWnd->GetWindowRect(rectList);
		ScreenToClient(rectList);

		// 纵向应该分多少格坐标
		int32 iCountsY = m_stResult.m_mapBuySell.size();
		if ( iCountsY <= 0 )
		{
			return;
		}
			
		//		
		dc.SelectObject(CFaceScheme::Instance()->GetSysFontObject(ESFSmall));
		CSize sizeText  = dc.GetTextExtent(L"200.000");
		
		int32 iLeftText = sizeText.cx;
		int32 iRighSkip	= 20;

		// 
		CRect rectCtrl;
		GetDlgItem(IDC_STATIC_PIC_PAGE3)->GetWindowRect(rectCtrl);
		ScreenToClient(rectCtrl);

		CRect rectGrid = rectCtrl;
		rectGrid.left += iLeftText;
		rectGrid.right-= iRighSkip;
		
		//
		CBrush brush(RGB(123,123,123));
		CBrush* pBrushOld = dc.SelectObject(&brush);
		dc.FrameRect(rectGrid, &brush);
		dc.SelectObject(pBrushOld);
		rectGrid.InflateRect(-1, -1);
	
		// 画图的地方		
		int32 iHeightPer = rectGrid.Height() / iCountsY;
		// 差了这么多像素.. 补回去
		int32 iDiffer = 0;

		int32 iSkip = 0;
		
		if ( iHeightPer > sizeText.cy && iHeightPer > s_KiMaxHeight )
		{
			iHeightPer = s_KiMaxHeight;

			iSkip = (rectGrid.Height() - iCountsY * s_KiMaxHeight) / (iCountsY + 1);			
		}
		else
		{
			iDiffer = rectGrid.Height() - (iCountsY * iHeightPer);
		}

		// 表格的中点
		CPoint ptCenterGrid = rectGrid.CenterPoint();

		// 当前矩形的底部 Y 坐标
		int32 iYNowBottom = rectGrid.bottom - iSkip;

		// 当前文字的底部坐标
		int32 iYNowTextBottom = rectGrid.bottom - iSkip;

		for ( map<float, T_TrendIntervalBS>::iterator it = m_stResult.m_mapBuySell.begin(); it != m_stResult.m_mapBuySell.end(); ++it )
		{
			// 当前的这个价格对应的矩形
			float fPriceNow   = it->first;
			float fBuyVolumn  = it->second.m_fBuyVolumn;
			float fSellVolumn = it->second.m_fSellVolumn;

			// 左边主买矩形
			CRect rectNowLeft;

			rectNowLeft.bottom = iYNowBottom;
			if ( iDiffer > 0 )
			{
				rectNowLeft.top    = iYNowBottom - iHeightPer - 1;
			}
			else
			{
				rectNowLeft.top    = iYNowBottom - iHeightPer;
			}

			rectNowLeft.right  = ptCenterGrid.x;
			rectNowLeft.left   = long(rectNowLeft.right - ((rectGrid.Width() * fBuyVolumn) / (2.0f * m_stResult.m_fMaxBuyVolumn)));

			if ( rectNowLeft.Width() > 0 )
			{
				dc.FillSolidRect(&rectNowLeft, RGB(245,0,0));			
				dc.Draw3dRect(&rectNowLeft, RGB(255,0,0), RGB(188,0,0));
			}
			
			// 右边主卖矩形
			CRect rectNowRight = rectNowLeft;

			rectNowRight.left  = ptCenterGrid.x;
			rectNowRight.right = long(rectNowRight.left + ((rectGrid.Width() * fSellVolumn) / (2.0f * m_stResult.m_fMaxSellVolumn)));

			if ( rectNowRight.Width() > 0 )
			{
				dc.FillSolidRect(&rectNowRight, RGB(0,255,0));			
				dc.Draw3dRect(&rectNowRight, RGB(0,255,0), RGB(0,150,0));
			}

			// 写字:
			{
				CRect rectNowText;

				rectNowText.left	= rectCtrl.left;
				rectNowText.right	= rectGrid.left ;
				rectNowText.bottom  = rectNowLeft.bottom;
				rectNowText.top		= rectNowText.bottom - sizeText.cy;

				if ( rectNowText.bottom <= iYNowTextBottom )
				{
					CString StrText;
					StrText.Format(L"%.2f", fPriceNow);		
					dc.SetBkMode(TRANSPARENT);
					dc.DrawText(StrText, rectNowText, DT_CENTER);

					iYNowTextBottom = rectNowText.top;
				}				
			}

			//
			iYNowBottom -= (rectNowLeft.Height() + iSkip);
			iDiffer --;
		}
	}
}

bool32 CDlgTrendInterval::IsRectInRgn( const CRgn &rgn, const CRect &rc )
{
	return rgn.PtInRegion(rc.TopLeft())
		&& rgn.PtInRegion(rc.BottomRight())
		&& rgn.PtInRegion(rc.left, rc.bottom)
		&& rgn.PtInRegion(rc.right, rc.top);
}

void CDlgTrendInterval::OnTimer( UINT nIDEvent )
{
	if ( 1 == nIDEvent )
	{
		Invalidate();
		KillTimer(1);
	}
}

void CDlgTrendInterval::RedrawAsync()
{
	SetTimer(1, 100, NULL);
}

void CDlgTrendInterval::OnDestroy()
{
	if ( NULL != m_pIoViewTrend )
	{
		m_pIoViewTrend->ClearIntervalTime();
	}

	CDialogEx::OnDestroy();
}

void CDlgTrendInterval::FromXml()
{
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	CString StrPath = CPathFactory::GetIntervalTrendPath(pDoc->m_pAbsCenterManager->GetUserName());
	if ( StrPath.IsEmpty() )
	{
		return;
	}

	string sPath = _Unicode2MultiChar(StrPath);
	const char* pStrPath = sPath.c_str();
	TiXmlDocument Doc(pStrPath);
	if ( !Doc.LoadFile() )
	{
		return;
	}

	//
	TiXmlElement* pRoot = Doc.RootElement();
	if ( NULL == pRoot )
	{
		return;
	}

	//
	TiXmlElement* pElementLimit = pRoot->FirstChildElement();
	if ( NULL == pElementLimit )
	{
		return;
	}

	const char* pStrValue = pElementLimit->Attribute(s_KStrAttriValueName);
	if ( NULL == pStrValue || strlen(pStrValue) <= 0 )
	{
		return;
	}

	//
	m_iLimit = atoi(pStrValue);
	if ( m_iLimit < 5 )
	{
		m_iLimit = 5;
	}

	//
	m_iEditLimit = m_iLimit;
}

bool32 CDlgTrendInterval::CreateDefaultXml()
{
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	CString StrPath = CPathFactory::GetIntervalTrendPath(pDoc->m_pAbsCenterManager->GetUserName());
	if ( StrPath.IsEmpty() )
	{
		return false;
	}

	CString StrXml;
	StrXml.Format(L"<?xml version =\"%s\" encoding=\"utf-8\" ?> \n", L"1.0");
	
	
	CString StrHead;
	StrHead = L"<XMLDATA version=\"1.0.1.0\" app = \"ggtong\" data = \"trend_interval\">\n";
	
	StrXml += StrHead;
	
	CString StrNode;
	StrNode.Format(L"<%s %s = \"%d\"/>\n", CString(s_KStrElementName).GetBuffer(), CString(s_KStrAttriValueName).GetBuffer(), m_iLimit);
	StrXml += StrNode;

	StrXml += L"</XMLDATA>";

	SaveXmlFile(_Unicode2MultiChar(StrPath).c_str(), StrXml);

	return true;
}

void CDlgTrendInterval::ToXml()
{
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	CString StrPath = CPathFactory::GetIntervalTrendPath(pDoc->m_pAbsCenterManager->GetUserName());
	if ( StrPath.IsEmpty() )
	{
		return;
	}
	
	TiXmlDocument Doc(_Unicode2MultiChar(StrPath).c_str());
	if ( !Doc.LoadFile() )
	{
		if ( !CreateDefaultXml() )
		{
			return;
		}		
	}
	
	//
	TiXmlElement* pRoot = Doc.RootElement();
	if ( NULL == pRoot )
	{
		return;
	}
	
	//
	TiXmlElement* pElementLimit = pRoot->FirstChildElement();
	if ( NULL == pElementLimit )
	{
		return;
	}
	
	//
	pElementLimit->SetAttribute(s_KStrAttriValueName, m_iLimit);
	Doc.SaveFile();
}

void CDlgTrendInterval::OnColumnclickListTick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	const int iColumn = pNMListView->iSubItem;
	Sort(iColumn, iColumn == m_iSortColumn ? !m_bAscend: TRUE );

	*pResult = 0;
}

void CDlgTrendInterval::Sort(int32 iCol, bool32 bAscend)
{
	m_iSortColumn = iCol;
	m_bAscend	  = bAscend;
	
	//	
	m_ListTick.SortItems(CompareFunction, (DWORD)this);
}

int CALLBACK CDlgTrendInterval::CompareFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamData)
{
	//
	CDlgTrendInterval* pThis = (CDlgTrendInterval*)lParamData;

	//
	ListCmpData* pData1 = (ListCmpData*)lParam1;
	ListCmpData* pData2 = (ListCmpData*)lParam2;

	if ( NULL == pData1 || NULL == pData2 )
	{
		//ASSERT(0);
		return -1;
	}

	//
	int32 iResult = 1;

	if ( 0 == pThis->m_iSortColumn )
	{
		// flag 排序
		if ( pData1->m_iFlag < pData2->m_iFlag )
		{
			iResult = 1;
		}
		else
		{
			iResult = -1;
		}
	}
	else if ( 1 == pThis->m_iSortColumn )
	{
		// 价格排序
		if ( pData1->m_fPrice < pData2->m_fPrice )
		{
			iResult = 1;
		}
		else
		{
			iResult = -1;
		}
	}
	else if ( 2 == pThis->m_iSortColumn )
	{
		// 成交量排序
		if ( pData1->m_fVolume < pData2->m_fVolume )
		{
			iResult = 1;
		}
		else
		{
			iResult = -1;
		}
	}
	else if ( 3 == pThis->m_iSortColumn )
	{
		// 时间排序
		if ( pData1->m_lTime < pData2->m_lTime )
		{
			iResult = 1;
		}
		else
		{
			iResult = -1;
		}
	}
		 
	//
	if ( pThis->m_bAscend )
	{
		iResult *= -1;
	}

	return iResult;
}
