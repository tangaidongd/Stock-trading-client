// SimulationHomePage.cpp : implementation file
//
#include "stdafx.h"
#include "SimulationHomePage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSimulationHomePage dialog

CSimulationHomePage::CSimulationHomePage(CWnd* pParent /*=NULL*/) : CDialog(CSimulationHomePage::IDD, pParent)
{
	m_pTradeLoginInfo = NULL;
}

void CSimulationHomePage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNothingDlg)
	DDX_Control(pDX, IDC_STATIC_INITIAL_ASSET, m_staticInitialAsset);
	DDX_Control(pDX, IDC_STATIC_INITIAL_ASSET_VALUE, m_staticInitialAssetValue);
	DDX_Control(pDX, IDC_STATIC_TOTAL_ASSETS, m_staticTotalAssets);
	DDX_Control(pDX, IDC_STATIC_TOTAL_ASSETS_VALUE, m_staticTotalAssetsValue);
	DDX_Control(pDX, IDC_STATIC_PROFIT_LOSS_RATE, m_staticProfitLossRate);
	DDX_Control(pDX, IDC_STATIC_PROFIT_LOSS_RATE_VALUE, m_staticProfitLossRateValue);

	DDX_Control(pDX, IDC_STATIC_HAZARD_RATE, m_staticHazardRate);
	DDX_Control(pDX, IDC_STATIC_HAZARD_RATE_VALUE, m_staticHazardRateValue);
	DDX_Control(pDX, IDC_STATIC_AVAILABLE_DEPOSIT, m_staticAvailableDeposit);
	DDX_Control(pDX, IDC_STATIC_AVAILABLE_DEPOSIT_VALUE, m_staticAvailableDepositVlaue);
	DDX_Control(pDX, IDC_STATIC_FREEZE_DEPOSIT, m_staticFreezeDeposit);
	DDX_Control(pDX, IDC_STATIC_FREEZE_DEPOSIT_VALUE, m_staticFreezeDepositValue);

	DDX_Control(pDX, IDC_STATIC_YESTERDAY_EARNING_RATE, m_staticYesterdayEarningRate);
	DDX_Control(pDX, IDC_STATIC_YESTERDAY_EARNING_RATE_VALUE, m_staticYesterdayEarningRateValue);
	DDX_Control(pDX, IDC_STATIC_TOTAL_PROFIT_LOSS, m_staticTotalProfitLoss);
	DDX_Control(pDX, IDC_STATIC_TOTAL_PROFIT_LOSS_VALUE, m_staticTotalProfitLossValue);
	DDX_Control(pDX, IDC_STATIC_CONTINUE_PRFIT_DAYS, m_staticContinueProfitDays);
	DDX_Control(pDX, IDC_STATIC_CONTINUE_PRFIT_DAYS_VALUE, m_staticContinueProfitDaysValue);

	DDX_Control(pDX, IDC_STATIC_THIS_WEEK_EARNING_RATE, m_staticThisWeekProfitRate);
	DDX_Control(pDX, IDC_STATIC_THIS_WEEK_EARNING_RATE_VALUE, m_staticThisWeekProfitRateValue);
	DDX_Control(pDX, IDC_STATIC_OCCUPY_DEPOSIT, m_staticOccupyDeposit);
	DDX_Control(pDX, IDC_STATIC_OCCUPY_DEPOSIT_VALUE, m_staticOccupyDepositValue);
	DDX_Control(pDX, IDC_STATIC_PROFIT_BEYOND_TEN_DAYS, m_staticProfitBeyondTenDays);
	DDX_Control(pDX, IDC_STATIC_PROFIT_BEYOND_TEN_DAYS_VALUE, m_staticProfitBeyondTenDaysValue);
	
	//}}AFX_DATA_MAP 
}

BEGIN_MESSAGE_MAP(CSimulationHomePage, CDialog)
	//{{AFX_MSG_MAP(CSimulationHomePage)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimulationHomePage message handlers

BOOL CSimulationHomePage::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_staticProfitLossRateValue.SetTextColor(RGB(0xC9, 0x13, 0x1F));
	m_staticYesterdayEarningRateValue.SetTextColor(RGB(0xC9, 0x13, 0x1F));
	m_staticTotalProfitLossValue.SetTextColor(RGB(0xC9, 0x13, 0x1F));
	m_staticThisWeekProfitRateValue.SetTextColor(RGB(0xC9, 0x13, 0x1F));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CSimulationHomePage::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN))
	{
		return true;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CSimulationHomePage::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rect;
	GetClientRect(&rect);

	CBitmap bitmap;	//定义一个位图对象
	CDC MemeDc;	//首先定义一个显示设备对象
	//创建兼容设备DC，不过这时还不能绘图，因为没有地方画
	MemeDc.CreateCompatibleDC(&dc);	
	//建立一个与屏幕显示兼容的位图，至于位图的大小嘛，可以用窗口的大小，也可以自己定义
	//（如：有滚动条时就要大于当前窗口的大小，在BitBlt时决定拷贝内存的哪部分到屏幕上）
	bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	//将位图选入到内存显示设备中(只有选入了位图的内存显示设备才有地方绘图，画到指定的位图)
	CBitmap *pOldBitmap = MemeDc.SelectObject(&bitmap);
	//先用背景色将位图清除干净
	if(m_pTradeLoginInfo != NULL && m_pTradeLoginInfo->eTradeLoginType == ETT_TRADE_SIMULATE)
	{
		MemeDc.FillSolidRect(rect, RGB(236, 239, 243));
	}
	else
	{
		MemeDc.FillSolidRect(rect, RGB(0xF1, 0xF2, 0xEA));
	}

	static byte bSetColor = true;	// 静态文本框背景，只设置一次。
	if(bSetColor && (m_pTradeLoginInfo != NULL) && (m_pTradeLoginInfo->eTradeLoginType == ETT_TRADE_SIMULATE))
	{
		m_staticInitialAsset.SetBKColor(RGB(236, 239, 243));
		m_staticHazardRate.SetBKColor(RGB(236, 239, 243));
		m_staticAvailableDeposit.SetBKColor(RGB(236, 239, 243));
		m_staticOccupyDeposit.SetBKColor(RGB(236, 239, 243));

		m_staticInitialAssetValue.SetBKColor(RGB(236, 239, 243));
		m_staticHazardRateValue.SetBKColor(RGB(236, 239, 243));
		m_staticAvailableDepositVlaue.SetBKColor(RGB(236, 239, 243));
		m_staticOccupyDepositValue.SetBKColor(RGB(236, 239, 243));

		bSetColor = false;
	}
	
	Graphics graphics(MemeDc.m_hDC);
	Pen pen(COLOR_OUT_FRAME);
	Rect rcOut(rect.left, rect.top, rect.Width() - 1, rect.Height() - 1);
	graphics.DrawRectangle(&pen, rcOut);

	dc.BitBlt( rect.left, rect.top, rect.Width(), rect.Height(), &MemeDc, 0, 0, SRCCOPY);
	
	MemeDc.SelectObject(pOldBitmap);
	MemeDc.DeleteDC();
	bitmap.DeleteObject();
}

void CSimulationHomePage::StaticCenterVer(CBeautifulStatic *pStatic)
{
	CRect rcClient, rcStatic;
	GetClientRect(rcClient);
	pStatic->GetWindowRect(rcStatic);
	ScreenToClient(rcStatic);

	int nHeight = rcStatic.Height();

	rcStatic.top = rcClient.top + (rcClient.Height() - nHeight) / 2;
	rcStatic.bottom = rcStatic.top + nHeight;
	pStatic->MoveWindow(rcStatic);
}

void CSimulationHomePage::AdjustAllStaticSize()
{
	int nWidth;
	const int nSpace = 5;
	CRect rect, rectValue;

	rect = m_staticInitialAsset.AdjustStaticSize();	//
	rectValue = m_staticInitialAssetValue.AdjustStaticSize();
	nWidth = rectValue.Width();
	rectValue.left = rect.right + nSpace;
	rectValue.right = rectValue.left + nWidth;
	ScreenToClient(rectValue);
	m_staticInitialAssetValue.MoveWindow(rectValue);

	StaticCenterVer(&m_staticInitialAsset);
	StaticCenterVer(&m_staticInitialAssetValue);

// 	rect = m_staticTotalAssets.AdjustStaticSize();
// 	rectValue = m_staticTotalAssetsValue.AdjustStaticSize();
// 	nWidth = rectValue.Width();
// 	rectValue.left = rect.right + nSpace;
// 	rectValue.right = rectValue.left + nWidth;
// 	ScreenToClient(rectValue);
// 	m_staticTotalAssetsValue.MoveWindow(rectValue);
	
// 	rect = m_staticProfitLossRate.AdjustStaticSize();
// 	rectValue = m_staticProfitLossRateValue.AdjustStaticSize();
// 	nWidth = rectValue.Width();
// 	rectValue.left = rect.right + nSpace;
// 	rectValue.right = rectValue.left + nWidth;
// 	ScreenToClient(rectValue);
// 	m_staticProfitLossRateValue.MoveWindow(rectValue);
	
	rect = m_staticHazardRate.AdjustStaticSize();//
	rectValue = m_staticHazardRateValue.AdjustStaticSize();
	nWidth = rectValue.Width();
	rectValue.left = rect.right + nSpace;
	rectValue.right = rectValue.left + nWidth;
	ScreenToClient(rectValue);
	m_staticHazardRateValue.MoveWindow(rectValue);

	StaticCenterVer(&m_staticHazardRate);
	StaticCenterVer(&m_staticHazardRateValue);
	
	rect = m_staticAvailableDeposit.AdjustStaticSize();//
	rectValue = m_staticAvailableDepositVlaue.AdjustStaticSize();
	nWidth = rectValue.Width();
	rectValue.left = rect.right + nSpace;
	rectValue.right = rectValue.left + nWidth;
	ScreenToClient(rectValue);
	m_staticAvailableDepositVlaue.MoveWindow(rectValue);

	StaticCenterVer(&m_staticAvailableDeposit);
	StaticCenterVer(&m_staticAvailableDepositVlaue);
	
// 	rect = m_staticFreezeDeposit.AdjustStaticSize();
// 	rectValue = m_staticFreezeDepositValue.AdjustStaticSize();
// 	nWidth = rectValue.Width();
// 	rectValue.left = rect.right + nSpace;
// 	rectValue.right = rectValue.left + nWidth;
// 	ScreenToClient(rectValue);
// 	m_staticFreezeDepositValue.MoveWindow(rectValue);
// 	
// 	rect = m_staticYesterdayEarningRate.AdjustStaticSize();
// 	rectValue = m_staticYesterdayEarningRateValue.AdjustStaticSize();
// 	nWidth = rectValue.Width();
// 	rectValue.left = rect.right + nSpace;
// 	rectValue.right = rectValue.left + nWidth;
// 	ScreenToClient(rectValue);
// 	m_staticYesterdayEarningRateValue.MoveWindow(rectValue);
// 	
// 	rect = m_staticTotalProfitLoss.AdjustStaticSize();
// 	rectValue = m_staticTotalProfitLossValue.AdjustStaticSize();
// 	nWidth = rectValue.Width();
// 	rectValue.left = rect.right + nSpace;
// 	rectValue.right = rectValue.left + nWidth;
// 	ScreenToClient(rectValue);
// 	m_staticTotalProfitLossValue.MoveWindow(rectValue);
// 	
// 	rect = m_staticContinueProfitDays.AdjustStaticSize();
// 	rectValue = m_staticContinueProfitDaysValue.AdjustStaticSize();
// 	nWidth = rectValue.Width();
// 	rectValue.left = rect.right + nSpace;
// 	rectValue.right = rectValue.left + nWidth;
// 	ScreenToClient(rectValue);
// 	m_staticContinueProfitDaysValue.MoveWindow(rectValue);
// 	
// 	rect = m_staticThisWeekProfitRate.AdjustStaticSize();
// 	rectValue = m_staticThisWeekProfitRateValue.AdjustStaticSize();
// 	nWidth = rectValue.Width();
// 	rectValue.left = rect.right + nSpace;
// 	rectValue.right = rectValue.left + nWidth;
// 	ScreenToClient(rectValue);
// 	m_staticThisWeekProfitRateValue.MoveWindow(rectValue);
	
	rect = m_staticOccupyDeposit.AdjustStaticSize();//
	rectValue = m_staticOccupyDepositValue.AdjustStaticSize();
	nWidth = rectValue.Width();
	rectValue.left = rect.right + nSpace;
	rectValue.right = rectValue.left + nWidth;
	ScreenToClient(rectValue);
	m_staticOccupyDepositValue.MoveWindow(rectValue);

	StaticCenterVer(&m_staticOccupyDeposit);
	StaticCenterVer(&m_staticOccupyDepositValue);
	
// 	rect = m_staticProfitBeyondTenDays.AdjustStaticSize();
// 	rectValue = m_staticProfitBeyondTenDaysValue.AdjustStaticSize();
// 	nWidth = rectValue.Width();
// 	rectValue.left = rect.right + nSpace;
// 	rectValue.right = rectValue.left + nWidth;
// 	ScreenToClient(rectValue);
// 	m_staticProfitBeyondTenDaysValue.MoveWindow(rectValue);
}

void CSimulationHomePage::SetClientRespUserInfo(T_CommUserInfoList *pList)
{
	// 初始资产
	m_staticInitialAssetValue.SetWindowText(pList[2].pValue1);
	// 总资产
	//m_staticTotalAssetsValue.SetWindowText(L"--");
	// 盈亏比例
	//m_staticProfitLossRateValue.SetWindowText(L"--");
	// 风险率
	m_staticHazardRateValue.SetWindowText(pList[4].pValue2);
	// 可用保证金
	m_staticAvailableDepositVlaue.SetWindowText(pList[1].pValue3);
	// 冻结保证金
	//m_staticFreezeDepositValue.SetWindowText(pList[3].pValue3);
	// 昨天收益率
	//m_staticYesterdayEarningRateValue.SetWindowText(L"--");
	// 总盈亏
	//m_staticTotalProfitLossValue.SetWindowText(L"--");
	// 本周连续盈利最大天数
	//m_staticContinueProfitDaysValue.SetWindowText(L"--");
	// 本周收益率
	//m_staticThisWeekProfitRateValue.SetWindowText(L"--");
	// 占用保证金
	m_staticOccupyDepositValue.SetWindowText(pList[2].pValue3);
	// 本周日收益率大于10 %天数
	//m_staticProfitBeyondTenDaysValue.SetWindowText(L"--");

	// 数据变化后，重新调整一下各个静态文本框的大小位置
	AdjustAllStaticSize();
}
