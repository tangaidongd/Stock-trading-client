#include "stdafx.h"
#include "memdc.h"
#include "MPIChildFrame.h"
#include "IoViewManager.h"
#include "MerchManager.h"
#include "GridCellSymbol.h"
#include "facescheme.h"
#include "ShareFun.h"
#include "IoViewTimeSale.h"
#include "DialogExportSaleData.h"
#include "FontFactory.h"

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
							//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////
const int32 KShowTimeSaleCount		 = 10000;
const int32 KNormalShowTimeSaleCount = 50;

const int32 KMinRequestTimeSaleCount = 500;    // 如果无法确定显示区域数据大小，则选用这个值来申请数据

static const int32 KForceRequestTimeSalePage = INT_MAX - 1; // 强制申请当前数据量的最新的历史报价 - 以防止断线重连后丢失的部分最新报价成为了历史报价，但是我们没有申请

const int32 KTimerIdRefresh			= 6005;
const int32 KTimerPeriodRefresh		= 300;
//////////////////////////////////////////////////////////////////////

void TestRecordData(CArray<CTick, CTick>& aTick);

// 
CShowTick::CShowTick()
{
	m_eTickType			= ETTTrade;
	m_fPrice			= 0.;
	m_fVolume			= 0.;
	m_fHold				= 0.;
	m_eTradeKind		= CTick::ETKUnKnown;

	m_TimePrev			= CMsTime(0);
	m_fHoldTotalPrev	= 0.;
	m_fPricePre			= 0.;
	m_StrProperty		= L"-";
}

const CShowTick& CShowTick::operator=(const CTick &Tick)
{
	m_TimeCurrent	= Tick.m_TimeCurrent;		// 该时间为商品所在时区的时间， 仅对相关字段赋值， 其它均为0值， 系统里面对该字段的处理均按该规则处理， 比如仅精确度到分钟	
	m_eTickType		= Tick.m_eTickType;			// 类型
	m_fPrice		= Tick.m_fPrice;			// 价格
	m_fVolume		= Tick.m_fVolume;			// 现手
	m_fHold			= Tick.m_fHold;				// 总持仓量
	m_eTradeKind	= Tick.m_eTradeKind;		// 买卖盘信息
	
	m_TimePrev		= CMsTime(0);
	m_fHoldTotalPrev= 0.;
	m_fPricePre		= 0.;
	m_StrProperty	= L"-";

	return *this;	
}

void CShowTick::Calculate()
{
	// 
	float fHoldAdd		= m_fHold - m_fHoldTotalPrev;	// 单笔仓差
	
	// 计算性质显示内容
	m_StrProperty = L"-";
	if (fHoldAdd == 0.)	// 记录换手
	{
		if (CTick::ETKSell == m_eTradeKind)	
		{
			m_StrProperty = L"多换";	// 如果当前的成交为外盘，则记录当前开仓为多头换手　（界面上简称为“多换”）
		}
		else if (CTick::ETKBuy == m_eTradeKind)	
		{
			m_StrProperty = L"空换";	// 如果当前的成交为内盘，则记录当前开仓为空头换手　（界面上简称为“空换”）
		}
	}
	else if (m_fVolume == fHoldAdd)	
	{
		m_StrProperty = L"双开";		// 单笔成交量=单笔仓差，则记录为双边开仓　（界面上简称为“双开”）
	}
	else if (m_fVolume == -fHoldAdd)
	{
		m_StrProperty = L"双平";		// 单笔成交量=-单笔仓差，则记录为双边平仓　（界面上简称为“双平”）
	}
	else if (fHoldAdd < m_fVolume)
	{
		if (fHoldAdd > 0)
		{
			if (CTick::ETKSell == m_eTradeKind)	
			{
				m_StrProperty = L"多开";	// 如果当前的成交为外盘，则记录当前开仓为多头开仓　（界面上简称为“多开”）
			}
			else if (CTick::ETKBuy == m_eTradeKind)	
			{
				m_StrProperty = L"空开";	// 如果当前的成交为内盘，则记录当前开仓为空头开仓　（界面上简称为“空开”）
			}
		}
		else if (fHoldAdd < 0)
		{
			if (CTick::ETKSell == m_eTradeKind)	
			{
				m_StrProperty = L"空平";	// 如果当前的成交为外盘，则记录为空头平仓　（界面上简称为“空平”）
			}
			else if (CTick::ETKBuy == m_eTradeKind)	
			{
				m_StrProperty = L"多平";	// 如果当前的成交为内盘，则记录为多头平仓　（界面上简称为“多平”）
			}
		}
	}
}


/////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CIoViewTimeSale, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewTimeSale, CIoViewBase)
	//{{AFX_MSG_MAP(CIoViewTimeSale)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_COMMAND(ID_EXPORT_DATA,OnExportData)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewTimeSale::CIoViewTimeSale()
:CIoViewBase()
{
	m_iSaveDec			= 2;

	m_aTicksShow.SetSize(0, KShowTimeSaleCount);
	m_eTimeSaleHeadType = ETHTCount;

	//
	m_bShowFull			= false;			
	m_bShowNewestFull	= true;	
	m_bRequestedTodayFirstData = false;

	m_iPerfectWidth		= 0;		
	m_iGridFullRow		= 0;				
	m_iGridFullCol		= 0;			
	m_iDataFullNums		= 0;		
	m_iGridFullColUints	= 0;

	m_iFullBeginIndex	= -1;
	m_iFullEndIndex		= -1;
	m_bActive			= false;

	m_bHistoryTimeSale = false;

	m_bSetTimer		   = FALSE;
	m_bMerchChange	   = FALSE;
	m_TimePre		   = ::time(NULL);
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewTimeSale::~CIoViewTimeSale()
{
}

CIoViewTimeSale::E_TimeSaleHeadType CIoViewTimeSale::GetMerchTimeSaleHeadType(E_ReportType eReportType)
{
	if (ERTFuturesCn == eReportType)
		return CIoViewTimeSale::ETHTFuture;

	return ETHTSimple;
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
BOOL CIoViewTimeSale::PreTranslateMessage(MSG* pMsg)
{
	if ( m_bHistoryTimeSale )
	{
		// 历史时，不处理这些消息
		return CControlBase::PreTranslateMessage(pMsg);
	}

	if ( WM_LBUTTONDBLCLK == pMsg->message )
	{
		// 双击的时候,最大化视图:
		CMainFrame* pMainFrame		= (CMainFrame*)AfxGetMainWnd();
		CGGTongView* pGGTongView	= GetParentGGtongView();
		
		//
		if ( NULL != pMainFrame && NULL != pGGTongView )
		{
			pMainFrame->OnProcessF7(pGGTongView);
			return TRUE;
		}
	}
	else if ( WM_KEYDOWN == pMsg->message )
	{
		if ( !m_bShowFull )
		{
			// 单列状态下，如果向上向下键，则默认将聚焦显示于临界行
			if ( VK_UP == pMsg->wParam )
			{
				CCellID idTopLeft = m_GridCtrl.GetTopleftNonFixedCell();
				if ( m_GridCtrl.IsValid(idTopLeft) )
				{
					m_GridCtrl.SetFocusCell(idTopLeft);
				}
			}
			else if ( VK_DOWN == pMsg->wParam )
			{
				CCellRange rangeVis = m_GridCtrl.GetVisibleNonFixedCellRange(NULL, FALSE, FALSE);
				if ( m_GridCtrl.IsValid(rangeVis) )
				{
					m_GridCtrl.SetFocusCell(rangeVis.GetMaxRow(), rangeVis.GetMinCol());
				}
			}
		}
	}

	return CIoViewBase::PreTranslateMessage(pMsg);
}

void CIoViewTimeSale::OnPaint()
{
	CPaintDC dc(this); // device context for painting	
	
	if ( !IsWindowVisible() )
	{
		return;
	}

	if ( GetParentGGTongViewDragFlag() || m_bDelByBiSplitTrack )
	{
		LockRedraw();
		return;
	}
	
	//
	UnLockRedraw();
	
	if ( m_GridCtrl.GetSafeHwnd() && !m_bShowFull )
	{
		m_GridCtrl.ShowWindow(SW_SHOW);	
		m_GridCtrlFull.ShowWindow(SW_HIDE);
		m_GridCtrl.RedrawWindow();
	}

	if ( m_GridCtrlFull.GetSafeHwnd() && m_bShowFull )
	{
		m_GridCtrlFull.ShowWindow(SW_SHOW);
		m_GridCtrl.ShowWindow(SW_HIDE);
		m_GridCtrlFull.RedrawWindow();
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnLButtonDown
void CIoViewTimeSale::OnLButtonDown(UINT nFlags, CPoint point)
{
	CStatic::OnLButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnLButtonUp
void CIoViewTimeSale::OnLButtonUp(UINT nFlags, CPoint point)
{
	CStatic::OnLButtonUp(nFlags, point);
}

void CIoViewTimeSale::OnLButtonDblClk(UINT nHitTest, CPoint point)
{
	CMainFrame* pMainFrame		= (CMainFrame*)AfxGetMainWnd();
	CGGTongView* pGGTongView	= GetParentGGtongView();
	
	//
	if ( NULL != pMainFrame && NULL != pGGTongView )
	{
		pMainFrame->OnProcessF7(pGGTongView);
	}

	CStatic::OnNcLButtonDblClk(nHitTest, point);
}
///////////////////////////////////////////////////////////////////////////////
// OnMouseMove
void CIoViewTimeSale::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint pt;
	pt.x = point.x;
	pt.y = point.y + 1;

	CStatic::OnMouseMove(nFlags, point);
}

BOOL CIoViewTimeSale::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{	
	// ...fangz1204 每次都进来两次? 暂时这么处理一下先
	// DefWindowProc OnMouseWheel行为：给父窗口发送WM_MOUSEWHEEL，返回FALSE
	// 由于Grid控件返回的是DefWindowProc的FALSE，于是CWnd::WindowProc又再次调用DefWindowProc
	// 于是两次调用产生了
	static bool32 bProcess = true;
	bProcess = !bProcess;

	// 当前如果是最大化显示.鼠标滚动时候翻页,清空重新显示数据
	
	if ( m_bShowFull && !bProcess )
	{		
		// 
		int32  iDir		 = zDelta / 120;	
		bool32 bNextPage = true;
		
		if ( iDir > 0 )
		{
			bNextPage = false;
		}

		//
		if ( -1 == m_iFullBeginIndex || -1 == m_iFullEndIndex )
		{
			// 还没正常初始化,不处理
			return CIoViewBase::OnMouseWheel(nFlags, zDelta, pt);
		}

		if ( m_bShowNewestFull && bNextPage )
		{
			// 已经是最新一页,再下翻.不处理
			return CIoViewBase::OnMouseWheel(nFlags, zDelta, pt);
		}

		// 

		// 由于zDelta 远离用户的滚动是 向上 为正，而OnPageScroll中是 负为向上，调转下符号
		OnPageScroll( - iDir );

		// 错误的情况
		
		return TRUE;
	}
	//else if ( !bProcess && !m_bShowFull )
	//{
	//	// 非多列模式
	//}

	return CIoViewBase::OnMouseWheel(nFlags, zDelta, pt);
}

void CIoViewTimeSale::OnIoViewFontChanged()
{	
	CIoViewBase::OnIoViewFontChanged();
	SetRowHeightAccordingFont();

	if ( !m_bShowFull )
	{
		m_GridCtrl.DeleteNonFixedRows();
		SetTableData();
	}

	// Full在这里，字体变小的化，可能数据不足
	OnPageScroll(0);

	m_GridCtrl.Refresh();
	m_GridCtrl.Invalidate();
}

void CIoViewTimeSale::LockRedraw()
{
	if ( m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = true;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
	::SendMessage(m_GridCtrl.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);	
}

void CIoViewTimeSale::UnLockRedraw()
{
	if ( !m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = false;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
	::SendMessage(m_GridCtrl.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
}

int CIoViewTimeSale::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CIoViewBase::OnCreate(lpCreateStruct);

	//
	InitialIoViewFace(this);

	// 创建横纵滚动条
 	m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10203);
 	m_XSBVert.SetScrollRange(0, 10);

	m_XSBHorz.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10204);
 	m_XSBHorz.SetScrollRange(0, 0);
	m_XSBHorz.ShowWindow(SW_HIDE);

	// 创建分笔数据表格
	{
		m_GridCtrl.Create(CRect(0, 0, 0, 0), this, 20205);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		m_GridCtrl.SetColumnCount(5);
		m_GridCtrl.SetColumnResize(FALSE);
		
		// 设置表格图标
		m_ImageList.Create(MAKEINTRESOURCE(IDB_GRID_REPORT), 16, 1, RGB(255,255,255));
		m_GridCtrl.SetImageList(&m_ImageList);
		
		// 设置相互之间的关联
		m_XSBHorz.SetOwner(&m_GridCtrl);
		m_XSBVert.SetOwner(&m_GridCtrl);
		m_GridCtrl.SetScrollBar(&m_XSBHorz, &m_XSBVert);
		
		// 设置表格风格
		m_GridCtrl.EnableSelection(false);
		m_GridCtrl.ShowGridLine(false);
		m_GridCtrl.SetDonotScrollToNonExistCell(GVL_BOTH);
	}

	// 创建最大化显示的表格
	{
		m_GridCtrlFull.Create(CRect(0, 0, 0, 0), this, 20206);
		m_GridCtrlFull.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
		m_GridCtrlFull.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
		m_GridCtrlFull.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		m_GridCtrlFull.SetColumnCount(1);
		m_GridCtrlFull.SetRowCount(1);
		m_GridCtrlFull.SetColumnResize(FALSE);
		
		// 设置表格图标
		m_GridCtrlFull.SetImageList(&m_ImageList);
		
		// 设置表格风格
		m_GridCtrlFull.EnableSelection(false);
		m_GridCtrlFull.ShowGridLine(false);
	}

	m_GridCtrl.EnablePolygonCorner(false);
	m_GridCtrlFull.EnablePolygonCorner(false);

	SetShowType(false);

	// 设置默认的表头
	SetTimeGridHead(ETHTSimple);
	SetRowHeightAccordingFont();
	
	return 0;
}

// 设置表头
void CIoViewTimeSale::SetTimeGridHead(E_TimeSaleHeadType eTimeSaleHeadType)
{
	if (NULL == m_GridCtrl.GetSafeHwnd())
		return;

	if ( !m_bShowFull )
	{
		// 正常显示的表格
		m_GridCtrl.DeleteAllItems();
		
		if (ETHTSimple == eTimeSaleHeadType)
		{
			m_GridCtrl.SetHeaderSort(FALSE);
			m_GridCtrl.SetFixedRowCount(0);
			m_GridCtrl.SetColumnCount(3);
		}
		else if (ETHTFuture == eTimeSaleHeadType)
		{
			m_GridCtrl.SetHeaderSort(FALSE);
			m_GridCtrl.SetFixedRowCount(0);
			m_GridCtrl.SetColumnCount(5);
		}
	}
	else
	{
		// 最大化的表格:
		CRect RectGrid;
		//
		if ( m_iGridFullColUints <= 1 )
		{
			return;
		}

		if (ETHTSimple == eTimeSaleHeadType)
		{
			int iCol = NoShowNewvol()? 2 : 3;
			for ( int32 i = 0 ; i < m_iGridFullColUints; i++ )
			{
				CGridCellSys *pCell = NULL;
				
				pCell = (CGridCellSys*)m_GridCtrlFull.GetCell(0, i*iCol + 0);
				pCell->SetText(L" 时间");
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE);
				pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD | pCell->GetCellRectDrawFlag());

				pCell = (CGridCellSys*)m_GridCtrlFull.GetCell(0, i*iCol + 1);
				pCell->SetText(L"价格 ");
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE);
				pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD | pCell->GetCellRectDrawFlag());

				if (iCol == 3)
				{
					pCell = (CGridCellSys*)m_GridCtrlFull.GetCell(0, i*iCol + 2);
					pCell->SetText(L"现手");
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE);		
					pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD | pCell->GetCellRectDrawFlag());
				}
			}	
			m_GridCtrlFull.ExpandColumnsToFit();
		}
		else if (ETHTFuture == eTimeSaleHeadType)
		{
			for ( int32 i = 0 ; i < m_iGridFullColUints; i++ )
			{				
				CGridCellSys *pCell = NULL;
				pCell = (CGridCellSys*)m_GridCtrlFull.GetCell(0, i*5 + 0);
				pCell->SetText(L" 时间");
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE);
				pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD | pCell->GetCellRectDrawFlag());

				pCell = (CGridCellSys*)m_GridCtrlFull.GetCell(0, i*5 + 1);
				pCell->SetText(L"价格");
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE);
				pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD | pCell->GetCellRectDrawFlag());

				pCell = (CGridCellSys*)m_GridCtrlFull.GetCell(0, i*5 + 2);
				pCell->SetText(L"现手");
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
				pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD | pCell->GetCellRectDrawFlag());

				pCell = (CGridCellSys*)m_GridCtrlFull.GetCell(0, i*5 + 3);
				pCell->SetText(L"仓差");
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
				pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD | pCell->GetCellRectDrawFlag());

				pCell = (CGridCellSys*)m_GridCtrlFull.GetCell(0, i*5 + 4);
				pCell->SetText(L"性质 ");
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE);		
				pCell->SetCellRectDrawFlag(DR_BOTTOM_SOILD | pCell->GetCellRectDrawFlag());
			}
			m_GridCtrlFull.ExpandColumnsToFit();
		}
	}
	
	//
	m_eTimeSaleHeadType = eTimeSaleHeadType;	
}

void CIoViewTimeSale::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);
			
	CRect rect = m_rectClient;

	if ( m_rectClient.Width() <= 0 || m_rectClient.Height() <= 0 )
	{
		return;
	}

 	CRect RectSB(rect);
	CRect RectGridCtrl(rect);
	CRect RectGridFull(rect);
	//
	m_GridCtrl.MoveWindow(RectGridCtrl);
	m_GridCtrl.ExpandColumnsToFit();
	//	
	bool32 bShowFullBk		= m_bShowFull;

	//
	m_GridCtrlFull.MoveWindow(RectGridFull);
	CalcFullGridParams();
	m_GridCtrlFull.ExpandColumnsToFit();

	//--- wangyongxue 2016/06/14 非全屏状态下都只显示一组数据
	if ( m_iGridFullColUints <= 1 )
	{
		SetShowType(false);
	}
	else
	{
		SetShowType(true);
	}

	// 三种情况要清空表格,重新填值

	// 理论上做到 m_iFullBeginIndex\endIndex\showNewest交给OnPageSroll控制, SetTableData修正具体数据

	// 1: 开始最大化显示,现在正常显示:
	if ( bShowFullBk && !m_bShowFull )
	{
		SetTimeGridHead(m_eTimeSaleHeadType);
		SetTableData();
	}

	// 2: 开始正常显示,现在最大化显示:
	if ( !bShowFullBk && m_bShowFull )
	{
		SetTimeGridHead(m_eTimeSaleHeadType);
		// 滚动到页尾
		OnPageScroll(INT_MAX);
	}

	// 3: 开始最大化显示,现在还是最大化显示,但是显示的参数变了:
	if ( bShowFullBk && m_bShowFull )
	{	
		SetTimeGridHead(m_eTimeSaleHeadType);
		OnPageScroll(0);
	}
}
 
BOOL CIoViewTimeSale::TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	static bool32 bProcess = true;
	bProcess = !bProcess;

	if ( IsWindowVisible() && VK_F1 == nChar && !bProcess )
	{
		CMainFrame* pMainFrame		= (CMainFrame*)AfxGetMainWnd();
		CGGTongView* pGGTongView	= GetParentGGtongView();
		
		//
		if ( NULL != pMainFrame && NULL != pGGTongView )
		{
			pMainFrame->OnProcessF7(pGGTongView);
			return TRUE;
		}		
	}
	else if ( VK_HOME == nChar && !bProcess )
	{
		// 第一页, 显示最老的
		if ( m_bShowFull )
		{			
			OnPageScroll(INT_MIN);
				
			return TRUE;
		}
	}
	else if ( VK_END == nChar && !bProcess )
	{
		// 最后一页, 显示最新的
		if ( m_bShowFull )
		{			
			OnPageScroll(INT_MAX);
			return TRUE;
		}
	}

	return FALSE;
}

// 通知视图改变关注的商品
void CIoViewTimeSale::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch)
		return;
	
	// 修改当前查看的商品
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;

	CalcFullGridParams();
	
	// 当前商品不为空的时候
	if (NULL != pMerch)
	{
		// 设置关注的商品信息
		m_aSmartAttendMerchs.RemoveAll();

		CSmartAttendMerch SmartAttendMerch;
		SmartAttendMerch.m_pMerch = pMerch;
		SmartAttendMerch.m_iDataServiceTypes = EDSTTimeSale | EDSTTick | EDSTKLine | EDSTPrice;
		m_aSmartAttendMerchs.Add(SmartAttendMerch);

		// 先清掉当前显示内容
		DeleteTableContent();

		// 清掉当前数据
		m_aTicksShow.SetSize(0);

		// 确定开收盘时间以及
		m_iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;

		// 如果需要， 改变标题
		E_TimeSaleHeadType eTimeSaleHeadType = GetMerchTimeSaleHeadType(pMerch->m_Market.m_MarketInfo.m_eMarketReportType);
		if (eTimeSaleHeadType != m_eTimeSaleHeadType)
		{
			SetTimeGridHead(eTimeSaleHeadType);
		}

		// 获取该商品走势相关的数据
		CMarketIOCTimeInfo RecentTradingDay;
		bool32 bTradingDayOk = false;
		if ( m_bHistoryTimeSale )
		{
			bTradingDayOk = pMerch->m_Market.GetSpecialTradingDayTime(m_TimeHistory, RecentTradingDay, pMerch->m_MerchInfo);
		}
		else
		{
			bTradingDayOk = pMerch->m_Market.GetRecentTradingDay(m_pAbsCenterManager->GetServerTime(), RecentTradingDay, pMerch->m_MerchInfo);
		}
		if ( bTradingDayOk )
		{
			m_TrendTradingDayInfo.Set(pMerch, RecentTradingDay);
			m_TrendTradingDayInfo.RecalcPrice(*pMerch);
			m_TrendTradingDayInfo.RecalcHold(*pMerch);
		}
		
		// 
		m_bMerchChange = TRUE;
		OnVDataMerchTimeSalesUpdate(pMerch);
	}
	else
	{
		// zhangbo 20090824 #待补充， 当心商品不存在时，清空当前显示数据
		//...
	}
}

//
void CIoViewTimeSale::OnVDataForceUpdate()
{
	RequestViewData();
}

void CIoViewTimeSale::OnVDataMerchTimeSalesUpdate(IN CMerch *pMerch)
{
	if (NULL == pMerch || pMerch != m_pMerchXml)
		return;

	if (!::IsWindow(m_GridCtrl.GetSafeHwnd())) 
		return;

	// 
	CMerch &Merch = *pMerch;
	if (NULL == Merch.m_pMerchTimeSales)
	{
		return;
	}
		
	if ( 0 == Merch.m_pMerchTimeSales->m_Ticks.GetSize() )
	{
		return;
	}

	// TestRecordData(Merch.m_pMerchTimeSales->m_Ticks);

	// 比较数据变化， 对于仅更后面的数据做判断
	int32 iPosDayStart = CMerchTimeSales::QuickFindTickWithBigOrEqualReferTime(Merch.m_pMerchTimeSales->m_Ticks, m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit);
	if (iPosDayStart < 0 || iPosDayStart >= Merch.m_pMerchTimeSales->m_Ticks.GetSize())
		return;

	if ( iPosDayStart > 0 )
	{
		// 今天以前的数据都出来了，由于现在是向前连贯申请数据，所以这个就标志着数据已经申请到最前面来了
		// 由于存在历史分笔，所以这个也是不确定的, 暂时去除此项 xl 110219
		// m_bRequestedTodayFirstData = true;
	}
	else
	{
		m_bRequestedTodayFirstData = false;
	}

	int32 iPosDayEnd = CMerchTimeSales::QuickFindTickWithSmallOrEqualReferTime(Merch.m_pMerchTimeSales->m_Ticks, m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeEnd);
	if (iPosDayEnd < 0 || iPosDayEnd >= Merch.m_pMerchTimeSales->m_Ticks.GetSize())
		return;

	if (iPosDayEnd < iPosDayStart)
	{
		TestRecordData(Merch.m_pMerchTimeSales->m_Ticks);
		return;
	}

	// 优化，绝大多数情况下， 该事件都是由于实时分笔数据更新引起的， 对这种情况做特殊判断

	CTick *pTickShow	= (CTick *)m_aTicksShow.GetData();
	int32 iCountShow	= m_aTicksShow.GetSize();

	CTick *pTickSrc		= (CTick *)Merch.m_pMerchTimeSales->m_Ticks.GetData();
	pTickSrc		   += iPosDayStart;
	int32 iCountSrc		= iPosDayEnd - iPosDayStart + 1;

	// 全部更新
	{
  		// 
 		if (iCountSrc > KShowTimeSaleCount + 1000)
 		{
			// 活跃商品会出现10062的情况
			TRACE(_T("商品tick数据量过大\r\n"));
 		}
 		
		// 直接复制就行了
 		m_aTicksShow.SetSize(iCountSrc);
 		pTickShow = (CTick *)m_aTicksShow.GetData();
 		iCountShow = m_aTicksShow.GetSize();
		
		memcpyex(pTickShow, pTickSrc, iCountShow * sizeof(CTick));

		CGmtTime timeNow = ::time(NULL);
		if (((timeNow.GetTime()-m_TimePre.GetTime()) < KTimerPeriodRefresh) && !m_bMerchChange)
		{
			m_TimePre = timeNow;
			if (!m_bSetTimer)
			{
				m_bSetTimer = TRUE;
				SetTimer(KTimerIdRefresh, KTimerPeriodRefresh, NULL);
			}
			return;
		}

		m_bMerchChange = FALSE;
		m_TimePre = timeNow;
		// 
		SetTableData();
	}
}

void CIoViewTimeSale::OnVDataMerchKLineUpdate(IN CMerch *pMerch)
{	
	if (NULL == pMerch || pMerch != m_pMerchXml)
		return;

	// 需要更新昨收价
	if (0. == m_TrendTradingDayInfo.m_fPricePrevClose && 0. == m_TrendTradingDayInfo.m_fPricePrevAvg && 0. == m_TrendTradingDayInfo.m_fHoldPrev)
	{
		float fBackupPricePrevClose = m_TrendTradingDayInfo.GetPrevReferPrice();
		float fBackupPricePrevHold	= m_TrendTradingDayInfo.GetPrevHold();

		m_TrendTradingDayInfo.RecalcPrice(*pMerch);
		m_TrendTradingDayInfo.RecalcHold(*pMerch);

		if (fBackupPricePrevClose != m_TrendTradingDayInfo.GetPrevReferPrice() || fBackupPricePrevHold != m_TrendTradingDayInfo.GetPrevHold())
		{
			SetTableData();
		}
	}
}

void CIoViewTimeSale::OnVDataRealtimePriceUpdate(IN CMerch *pMerch)
{
	if (NULL == pMerch || pMerch != m_pMerchXml)
		return;

	// 需要更新昨收价
	if (0. == m_TrendTradingDayInfo.m_fPricePrevClose && 0. == m_TrendTradingDayInfo.m_fPricePrevAvg && 0. == m_TrendTradingDayInfo.m_fHoldPrev)
	{
		float fBackupPricePrevClose = m_TrendTradingDayInfo.GetPrevReferPrice();
		float fBackupPricePrevHold	= m_TrendTradingDayInfo.GetPrevHold();

		m_TrendTradingDayInfo.RecalcPrice(*pMerch);
		m_TrendTradingDayInfo.RecalcHold(*pMerch);

		if (fBackupPricePrevClose != m_TrendTradingDayInfo.GetPrevReferPrice() || fBackupPricePrevHold != m_TrendTradingDayInfo.GetPrevHold())
		{
			SetTableData();
		}
	}
}

void CIoViewTimeSale::SetTableData()
{
	if ( m_bDelByBiSplitTrack )
	{
		LockRedraw();
		return;
	}
	else
	{
		UnLockRedraw();
	}

	//
 	if ( GetParentGGTongViewDragFlag() )
 	{
 		LockRedraw();		
 		return;
 	}
 	else
 	{
 		UnLockRedraw();		
 	}

	if ( m_bShowFull )
	{
		// 更新整个表的内容: (最大化显示)
		if ( !::IsWindow(m_GridCtrl.GetSafeHwnd()) )
		{
			return;
		}
			
		m_GridCtrlFull.DeleteNonFixedRows();
		
		m_GridCtrlFull.SetRowCount(m_iGridFullRow);
		m_GridCtrlFull.SetColumnCount(m_iGridFullCol);

		int32 iTickSize = m_aTicksShow.GetSize();

		if ( iTickSize > 0 )
		{
			COLORREF clrRise	= GetIoViewColor(ESCRise);
			COLORREF clrFall	= GetIoViewColor(ESCFall);
			COLORREF clrKeep	= GetIoViewColor(ESCKeep);
			COLORREF clrVolume	= GetIoViewColor(ESCVolume);
			
			CTick *pTickShow = (CTick *)m_aTicksShow.GetData();
			
			if ( iTickSize < m_iDataFullNums )
			{
				// 如果数据的个数不够一页显示,从最左上角开始挨个显示
				int32 iRowIndex		= 1;
				int32 iColUintIndex	= 0;

				// 开始结束的索引				
				m_iFullBeginIndex	= 0;
				m_iFullEndIndex		= iTickSize - 1;
				m_bShowNewestFull	= true;
				

				if ( !BeValidFullGridIndex() )
				{
					return;
				}

				for ( int32 i = m_iFullBeginIndex; i <= m_iFullEndIndex; i++ )
				{
					CShowTick ShowTick;
					ShowTick = pTickShow[i];
					
					if ( i > 0 )
					{
						ShowTick.m_TimePrev		  = pTickShow[i - 1].m_TimeCurrent;
						ShowTick.m_fHoldTotalPrev = pTickShow[i - 1].m_fHold;
						ShowTick.m_fPricePre	  = pTickShow[i - 1].m_fPrice;
					}
					else
					{
						ShowTick.m_fHoldTotalPrev = m_TrendTradingDayInfo.GetPrevHold();
						ShowTick.m_fPricePre	  = m_TrendTradingDayInfo.GetPrevReferPrice();
					}

					ShowTick.Calculate();

					//
					SetRowValue(ShowTick, iRowIndex, iColUintIndex, clrRise, clrFall, clrKeep, clrVolume);
					
					iRowIndex++;
					
					if ( iRowIndex == m_iGridFullRow )
					{
						iRowIndex		= 1;
						iColUintIndex   = iColUintIndex + 1;

						if ( iColUintIndex >= m_iGridFullColUints )
						{
							break;
						}
					}
				}
			}
			else
			{
				// 从最右下角开始
				int32 iRowIndex		= m_iGridFullRow - 1;
				int32 iColUintIndex	= m_iGridFullColUints -1;

				if ( -1 == m_iFullBeginIndex || -1 == m_iFullEndIndex || m_bShowNewestFull )
				{
					// 第一次的时候, 或者是现实最新一笔数据的时候
					m_iFullEndIndex		= iTickSize - 1;
					m_iFullBeginIndex	= m_iFullEndIndex - m_iDataFullNums + 1;
					m_bShowNewestFull	= true;
				}

				if ( !BeValidFullGridIndex() )
				{
					return;
				}

				//TRACE(_T("----------ShowTimeSale: %d - %d\r\n"), m_iFullBeginIndex, m_iFullEndIndex);
				for (int32 j = m_iFullEndIndex; j >= m_iFullBeginIndex; j-- )
				{
					
					// 挨个计算该如何显示
					CShowTick ShowTick;
					ShowTick = pTickShow[j];
					
					if ( j > 0 )
					{
						ShowTick.m_TimePrev		  = pTickShow[j - 1].m_TimeCurrent;
						ShowTick.m_fHoldTotalPrev = pTickShow[j - 1].m_fHold;
						ShowTick.m_fPricePre	  = pTickShow[j - 1].m_fPrice;
					}
					else
					{
						ShowTick.m_fHoldTotalPrev = m_TrendTradingDayInfo.GetPrevHold();
						ShowTick.m_fPricePre	  = m_TrendTradingDayInfo.GetPrevReferPrice();

					}
										
					ShowTick.Calculate();
					
					//
					SetRowValue(ShowTick, iRowIndex, iColUintIndex, clrRise, clrFall, clrKeep, clrVolume);
	
					iRowIndex--;

					if ( iRowIndex == 0 )
					{
						iRowIndex		= m_iGridFullRow - 1;
						iColUintIndex   = iColUintIndex - 1;					

						if ( iColUintIndex < 0 )
						{
							break;
						}
					}
				}
			}			
		}
		
		// 设置格线					
		for ( int32 iRow = 0; iRow < m_GridCtrlFull.GetRowCount(); iRow++ )
		{
			for ( int32 iCol = 0; iCol < m_GridCtrlFull.GetColumnCount(); iCol++ )
			{
				CGridCellSys* pCell = (CGridCellSys*)m_GridCtrlFull.GetCell(iRow, iCol);

				if ( iRow == (m_GridCtrlFull.GetRowCount() - 1) )
				{
					// 最后一行下面加横线
					pCell->SetCellRectDrawFlag( DR_BOTTOM_SOILD | pCell->GetCellRectDrawFlag());
				}

				if ( 0 == m_iGridFullColUints )
				{
					continue;
				}

				if ( 0 == ((iCol + 1) % ((m_GridCtrlFull.GetColumnCount() / m_iGridFullColUints))) )
				{
					// 单元列右边加上横线
					if ( 0 != iRow )
					{
						pCell->SetCellRectDrawFlag( DR_RIGHT_SOILD | pCell->GetCellRectDrawFlag());
					}					
				}
			}
		}

		//
		m_GridCtrlFull.ExpandColumnsToFit(TRUE);
		m_GridCtrlFull.Refresh();
		m_GridCtrlFull.Invalidate();	
		
		//
		SetRowHeightAccordingFont();
	}
	else
	{
		// 更新整个表的内容: (正常显示)
		// 由以前的最新的显示在上修改为最新的显示在下
		if ( !::IsWindow(m_GridCtrl.GetSafeHwnd()) )
			return;
		
		if ( m_XSBVert.GetSafeHwnd() && m_XSBVert.IsWindowVisible() )
		{
			m_XSBVert.ShowWindow(SW_HIDE);
		}
		
		m_GridCtrl.DeleteNonFixedRows();
		if (m_aTicksShow.GetSize() > 0)
		{
			COLORREF clrRise = GetIoViewColor(ESCRise);
			COLORREF clrFall = GetIoViewColor(ESCFall);
			COLORREF clrKeep = GetIoViewColor(ESCKeep);
			COLORREF clrVolume = GetIoViewColor(ESCVolume);
			
			//
			CTick *pTickShow = (CTick *)m_aTicksShow.GetData();
			int32 iFullSize  = m_aTicksShow.GetSize();

			//
			CArray<CTick, CTick> aTicksNormalShow;
			aTicksNormalShow.SetSize(KNormalShowTimeSaleCount);
			CTick* pTickNormalShow = (CTick *)aTicksNormalShow.GetData();

			// 复制最新的100 条数据,用于显示:
			if ( iFullSize > KNormalShowTimeSaleCount )
			{
				pTickShow += ( iFullSize - KNormalShowTimeSaleCount );					
				memcpyex(pTickNormalShow, pTickShow, sizeof(CTick)*KNormalShowTimeSaleCount);				

				//
				m_GridCtrl.SetRowCount(aTicksNormalShow.GetSize() + 1);
			}
			else
			{
				memcpyex(pTickNormalShow, pTickShow, sizeof(CTick)*iFullSize);		
				
				//
				aTicksNormalShow.SetSize(iFullSize);
				m_GridCtrl.SetRowCount(iFullSize + 1);
			}			
			
			//
			for (int32 i = 0; i < aTicksNormalShow.GetSize(); i++)
			{
				// 挨个计算该如何显示
				CShowTick ShowTick;
				ShowTick = pTickNormalShow[i];
				
				if (i > 0)
				{
					ShowTick.m_TimePrev		  = pTickNormalShow[i - 1].m_TimeCurrent;
					ShowTick.m_fHoldTotalPrev = pTickNormalShow[i - 1].m_fHold;
					ShowTick.m_fPricePre	  = pTickNormalShow[i - 1].m_fPrice;
				}
				else
				{
					ShowTick.m_fHoldTotalPrev = m_TrendTradingDayInfo.GetPrevHold();
					ShowTick.m_fPricePre	  = m_TrendTradingDayInfo.GetPrevReferPrice();
				}
				
				ShowTick.Calculate();
				//SetRowValue(ShowTick, iRowCount - i - 1, 0, clrRise, clrFall, clrKeep, clrVolume);
				SetRowValue(ShowTick, i + 1, 0, clrRise, clrFall, clrKeep, clrVolume);	// 预留一空行？

				for (int i =0; i < m_GridCtrl.GetRowCount(); ++i)
				{
					for (int j =0; j < m_GridCtrl.GetColumnCount(); ++j)
					{				
						CGridCellBase* pCell = m_GridCtrl.GetCell(i, j);
						if (NULL != pCell)
						{
							if (0 == j)
							{
								pCell->SetTextPadding(CRect(15,0,0,0));
							}

							if (m_GridCtrl.GetColumnCount()- 1 == j)
							{
								pCell->SetTextPadding(CRect(0,0,15,0));
							}
						}
					}
				}
			}
		}
		
		//
		//if ( m_XSBVert.GetSafeHwnd() && !m_XSBVert.IsWindowVisible() )
		//{
		//	m_XSBVert.ShowWindow(SW_SHOW);
		//}

		m_GridCtrl.DeleteRow(0);	// 不显示表头 - 虽然现在没有表头，但是在setRowValue的时候预留了一个空的表头

		m_GridCtrl.ExpandColumnsToFit(TRUE);
		//
		SetRowHeightAccordingFont();
		// 由于最后一行是最新数据，所以需要滚动
		if ( m_GridCtrl.GetRowCount() > 0 )
		{
			//m_GridCtrl.EnsureVisible(m_GridCtrl.GetRowCount()-1, 0);
			m_GridCtrl.EnsureBottomRightCell(m_GridCtrl.GetRowCount()-1, m_GridCtrl.GetColumnCount()-1);
		}
		//
		m_GridCtrl.Refresh();
	//	m_GridCtrl.Invalidate();		
	}	

	//
	SetActiveCell();
	Invalidate();
}

void CIoViewTimeSale::SetRowValue(const CShowTick &Tick, int32 iRowIndex, int32 iColUintIndex, COLORREF clrRise, COLORREF clrFall, COLORREF clrKeep, COLORREF clrVolume)
{
	if ( GetParentGGTongViewDragFlag() || m_bDelByBiSplitTrack )
	{
		LockRedraw();
		return;
	}
	else
	{
		UnLockRedraw();
	}
	
	if ( m_bDelByBiSplitTrack )
	{
		LockRedraw();
	}
	else
	{
		UnLockRedraw();
	}
	//
	if ( m_bShowFull )
	{
		if (iColUintIndex < 0 || iColUintIndex >= m_iGridFullColUints )
		{
			return;
		}
	}
	else
	{
		if (iRowIndex < 1 || iRowIndex >= m_GridCtrl.GetRowCount() || 0 != iColUintIndex )
		{
			return;
		}		
	}
	//
	COLORREF clrProperty = clrKeep;
	CString  StrFlag     = _T("");

	if (CTick::ETKBuy == Tick.m_eTradeKind)
	{
		clrProperty = clrFall;
		StrFlag     = "↓";
	}
	else if (CTick::ETKSell == Tick.m_eTradeKind)
	{
		clrProperty = clrRise;
		StrFlag     = "↑";
	}

	// 
	CGridCtrlSys* pGrid = NULL;
	
	if (m_bShowFull)
	{
		pGrid = &m_GridCtrlFull;
	}
	else
	{
		pGrid = &m_GridCtrl;
	}

	if ( NULL == pGrid )
	{
		return;
	}

	
	//  增加表格的左右边框的间距
	int iColumn = pGrid->GetColumnCount();
	ASSERT(iColumn > 0);
	for (int32 i=0; i<pGrid->GetRowCount(); i++)
	{		
		CGridCellBase* pCell = m_GridCtrl.GetCell(i, 0);
		if (NULL != pCell)
		{
			pCell->SetTextPadding(CRect(5,0,0,0));
		}

		pCell = m_GridCtrl.GetCell(i, iColumn - 1);
		if (NULL != pCell)
		{
			pCell->SetTextPadding(CRect(0,0,5,0));
		}
	}

	LOGFONT fontNumber;
	memset(&fontNumber, 0, sizeof(fontNumber));
	_tcscpy(fontNumber.lfFaceName, gFontFactory.GetExistFontName(_T("Arial")));///
	fontNumber.lfHeight  = -14;
	fontNumber.lfWeight = 560;
	//
	if (ETHTSimple == m_eTimeSaleHeadType)
	{
		int32 iColIndex = 0 + iColUintIndex * (NoShowNewvol()?2:3);
		bool32 bTimeChange = false;

		// 列1： 时间
		{
			//
			CGridCellSys *pCell = (CGridCellSys *)pGrid->GetCell(iRowIndex, iColIndex);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

			CString StrTime;
			CTime TimeCurrent(Tick.m_TimeCurrent.m_Time.GetTime());
			CTime TimePre(Tick.m_TimePrev.m_Time.GetTime());
			
			if ( TimeCurrent < 0 || TimePre < 0 )
			{
				return;
			}
			
			LOGFONT* pFontOld = pCell->GetFont();
			LOGFONT* pFontSmall = GetIoViewFont(ESFSmall);
			if ( NULL == pFontSmall )
			{
				pFontSmall = pFontOld;
			}
			
			// 显示 时分, 时分相同,显示秒
			if (TimeCurrent.GetHour() == TimePre.GetHour() && TimeCurrent.GetMinute() == TimePre.GetMinute() )
			{							
				// pCell->SetFont(pFontSmall);						
				// StrTime.Format(L"      :%02d", TimeCurrent.GetSecond());										
				StrTime.Format(L"%02d:%02d:%02d", TimeCurrent.GetHour(), TimeCurrent.GetMinute(), TimeCurrent.GetSecond());					
			}
			else
			{
				StrTime.Format(L"%02d:%02d:%02d", TimeCurrent.GetHour(), TimeCurrent.GetMinute(), TimeCurrent.GetSecond());					
				
				//bTimeChange = true;
				
				if ( m_bShowFull )
				{
					if ( 1 != iRowIndex )
					{
						//pCell->SetCellRectDrawFlag(DR_TOP_DOT | pCell->GetCellRectDrawFlag());
					}					
				}
				else
				{
					//pCell->SetCellRectDrawFlag(DR_TOP_DOT | pCell->GetCellRectDrawFlag());
				}										
			}				

			//
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrTime);
			iColIndex++;
		}
		 
		// 列2： 价格
		{
			pGrid->SetCellType(iRowIndex, iColIndex, RUNTIME_CLASS(CGridCellSymbol));
			CGridCellSymbol *pCellSymbol = (CGridCellSymbol *)pGrid->GetCell(iRowIndex, iColIndex);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);
			pCellSymbol->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

			if ( bTimeChange )
			{
				if ( m_bShowFull )
				{
					if ( 1 != iRowIndex )
					{
						pCellSymbol->SetCellRectDrawFlag(DR_TOP_DOT | pCellSymbol->GetCellRectDrawFlag());				
					}					
				}	
				else
				{
					pCellSymbol->SetCellRectDrawFlag(DR_TOP_DOT | pCellSymbol->GetCellRectDrawFlag());				
				}				
			}

			// 获取价格， 与昨收价比较判断区别正负

			CString StrPrice = Float2SymbolString(Tick.m_fPrice, Tick.m_fPricePre, m_iSaveDec);
			if(StrPrice == L"-")
			{
				StrPrice = L"";
			}
			else
			{
				if(abs(Tick.m_fPrice - Tick.m_fPricePre) > 0.000001f)
				{
					StrPrice += StrFlag;
				}
			}
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPrice);
			iColIndex++;
		}

		if(m_bShowFull)	// 全屏的情况下
		{
			// 列3： 现手
			if(!NoShowNewvol())
			{
				CGridCellSys *pCell = (CGridCellSys *)pGrid->GetCell(iRowIndex, iColIndex);
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

				if ( bTimeChange )
				{
					if ( m_bShowFull )
					{
						if ( 1 != iRowIndex )
						{
							pCell->SetCellRectDrawFlag(DR_TOP_DOT | pCell->GetCellRectDrawFlag());						
						}					
					}	
					else
					{
						pCell->SetCellRectDrawFlag(DR_TOP_DOT | pCell->GetCellRectDrawFlag());				
					}				
				}

				CString StrVolume = Float2String(Tick.m_fVolume, 0, true);		
				pCell->SetTextClr(clrProperty);
				pCell->SetFont(&fontNumber);
				pCell->SetText(StrVolume);

			}
		}
		else	// 非全屏时
		{
			// 列3： 现手
			CGridCellSys *pCell = (CGridCellSys *)pGrid->GetCell(iRowIndex, iColIndex);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

			if ( bTimeChange )
			{
				if ( m_bShowFull )
				{
					if ( 1 != iRowIndex )
					{
						pCell->SetCellRectDrawFlag(DR_TOP_DOT | pCell->GetCellRectDrawFlag());						
					}					
				}	
				else
				{
					pCell->SetCellRectDrawFlag(DR_TOP_DOT | pCell->GetCellRectDrawFlag());				
				}				
			}

			CString StrVolume = Float2String(Tick.m_fVolume, 0, true);		
			pCell->SetTextClr(clrProperty);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolume);

		}

	}
	else if (ETHTFuture == m_eTimeSaleHeadType)
	{
		bool32 bTimeChange = false;
		int32 iColIndex = 0 + iColUintIndex * 5;
		// 列1： 时间
		{
			//
			CGridCellSys *pCell = (CGridCellSys *)pGrid->GetCell(iRowIndex, iColIndex);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			CString StrTime;
			CTime TimeCurrent(Tick.m_TimeCurrent.m_Time.GetTime());
			CTime TimePre(Tick.m_TimePrev.m_Time.GetTime());
			
			if ( TimeCurrent < 0 || TimePre < 0 )
			{
				return;
			}

			LOGFONT* pFontOld = pCell->GetFont();
			LOGFONT* pFontSmall = GetIoViewFont(ESFSmall);
			if ( NULL == pFontSmall )
			{
				pFontSmall = pFontOld;
			}
	
			// 显示 时分, 时分相同,显示秒
			if (TimeCurrent.GetHour() == TimePre.GetHour() && TimeCurrent.GetMinute() == TimePre.GetMinute() )
			{				
				// pCell->SetFont(pFontSmall);		 			
				// StrTime.Format(L"      :%02d", TimeCurrent.GetSecond());										
				StrTime.Format(L"%02d:%02d", TimeCurrent.GetHour(), TimeCurrent.GetMinute());
			}
			else
			{
				//bTimeChange = true;
				StrTime.Format(L"%02d:%02d", TimeCurrent.GetHour(), TimeCurrent.GetMinute());
				
				if ( m_bShowFull )
				{
					if ( 1 != iRowIndex )
					{
						//pCell->SetCellRectDrawFlag(DR_TOP_DOT | pCell->GetCellRectDrawFlag());
					}					
				}
				else
				{
					//pCell->SetCellRectDrawFlag(DR_TOP_DOT | pCell->GetCellRectDrawFlag());
				}
			}				

			pCell->SetFont(&fontNumber);
			pCell->SetText(StrTime);
			iColIndex++;
		}
		
		// 列2： 价格
		{
			pGrid->SetCellType(iRowIndex, iColIndex, RUNTIME_CLASS(CGridCellSymbol));
			CGridCellSymbol *pCellSymbol = (CGridCellSymbol *)pGrid->GetCell(iRowIndex, iColIndex);
			pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);		
			pCellSymbol->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			
			if ( bTimeChange )
			{
				if ( m_bShowFull )
				{
					if ( 1 != iRowIndex )
					{
						pCellSymbol->SetCellRectDrawFlag(DR_TOP_DOT | pCellSymbol->GetCellRectDrawFlag());
					}					
				}
				else
				{
					pCellSymbol->SetCellRectDrawFlag(DR_TOP_DOT | pCellSymbol->GetCellRectDrawFlag());
				}				
			}
			// 获取价格， 与昨收价比较判断区别正负
			CString StrPrice = Float2SymbolString(Tick.m_fPrice, Tick.m_fPricePre, m_iSaveDec);
			if(StrPrice == L"-")
			{
				StrPrice = L"";
			}
			else
			{
				if(abs(Tick.m_fPrice - Tick.m_fPricePre) > 0.000001f)
				{
					StrPrice += L" " + StrFlag;
				}	
			}
			pCellSymbol->SetFont(&fontNumber);
			pCellSymbol->SetText(StrPrice);
			iColIndex++;
		}
		
		// 列3： 现手
		{
			CGridCellSys *pCell = (CGridCellSys *)pGrid->GetCell(iRowIndex, iColIndex);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			
			if ( bTimeChange )
			{
				if ( m_bShowFull )
				{
					if ( 1 != iRowIndex )
					{
						pCell->SetCellRectDrawFlag(DR_TOP_DOT | pCell->GetCellRectDrawFlag());
					}					
				}
				else
				{
					pCell->SetCellRectDrawFlag(DR_TOP_DOT | pCell->GetCellRectDrawFlag());
				}				
			}
			
			CString StrVolume = Float2String(Tick.m_fVolume, 0, true);
			if(StrVolume == L"-")
			{
				StrVolume = L" ";
			}
			pCell->SetTextClr(clrProperty);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrVolume);
			iColIndex++;
		}
		
		// 列4： 仓差
		{
			CGridCellSys *pCell = (CGridCellSys *)pGrid->GetCell(iRowIndex, iColIndex);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			
			if ( bTimeChange )
			{
				if ( m_bShowFull )
				{
					if ( 1 != iRowIndex )
					{
						pCell->SetCellRectDrawFlag(DR_TOP_DOT | pCell->GetCellRectDrawFlag());
					}					
				}
				else
				{
					pCell->SetCellRectDrawFlag(DR_TOP_DOT | pCell->GetCellRectDrawFlag());
				}				
			}

			CString StrHoldAdd = Float2String(Tick.m_fHold - Tick.m_fHoldTotalPrev, 0, false, false);
			pCell->SetTextClr(clrVolume);
			pCell->SetFont(&fontNumber);
			pCell->SetText(StrHoldAdd);
			iColIndex++;
		}
		
		// 列5： 性质
		{
			CGridCellSys *pCell = (CGridCellSys *)pGrid->GetCell(iRowIndex, iColIndex);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			
			if ( bTimeChange )
			{
				if ( m_bShowFull )
				{
					if ( 1 != iRowIndex )
					{
						pCell->SetCellRectDrawFlag(DR_TOP_DOT | pCell->GetCellRectDrawFlag());
					}					
				}
				else
				{
					pCell->SetCellRectDrawFlag(DR_TOP_DOT | pCell->GetCellRectDrawFlag());
				}				
			}

			pCell->SetTextClr(clrProperty);
			pCell->SetFont(&fontNumber);
			pCell->SetText(Tick.m_StrProperty+L" ");
		}
	}
}

void CIoViewTimeSale::SetShowType(bool32 bShowFull)
{
	if(bShowFull)	// 重新判断一下m_GridCtrl表格或m_GridCtrlFull表格选择
	{
		CRect RectGrid, RectMainWnd;
		m_GridCtrl.GetClientRect(&RectGrid);
		AfxGetMainWnd()->GetClientRect(&RectMainWnd);
	
		if(RectGrid.Width() < RectMainWnd.Width() / 2)	// 当列表宽度小于这个数时，不显示多列
		{
			bShowFull = false;
			//m_GridCtrl.SetColumnCount(NoShowNewvol()? 2 : 3);
		}
	}

	if ( m_bShowFull == bShowFull )
	{
		return;
	}

	m_bShowFull = bShowFull;
	
	if ( m_bShowFull )
	{
		m_GridCtrl.ShowWindow(SW_HIDE);
		m_XSBVert.ShowWindow(SW_HIDE);	
		m_GridCtrlFull.ShowWindow(SW_SHOW);
	}
	else
	{
		m_GridCtrl.ShowWindow(SW_SHOW);
		// m_XSBVert.ShowWindow(SW_SHOW);		
		m_GridCtrlFull.ShowWindow(SW_HIDE);
	}
}

void CIoViewTimeSale::SetActiveCell()
{
	//
	CGridCtrlSys* pGridCtrl = NULL;
	if ( m_bShowFull )
	{
		pGridCtrl = &m_GridCtrlFull;
	}
	else
	{
		pGridCtrl = &m_GridCtrl;
	}
	
	if ( NULL != pGridCtrl )
	{
		if ( pGridCtrl->GetRowCount() >= 1 && pGridCtrl->GetColumnCount() >= 1 )
		{
			if ( m_bShowFull )
			{
				// 多列模式下 0,0
				CGridCellSys* pCell = (CGridCellSys*)pGridCtrl->GetCell(0, 0);
				if ( NULL != pCell )
				{
					pCell->SetParentActiveFlag(m_bActive);
					pCell->SetDrawParentActiveParams(0, 0, m_ColorActive);					
				}
			}
			else
			{
				// 单列，最新
				// 由于每次数据更新都是重新载入，so不必考虑以前的清除
				CGridCellSys* pCell = (CGridCellSys*)pGridCtrl->GetCell(m_GridCtrl.GetRowCount()-1, 0);
				if ( NULL != pCell )
				{
					pCell->SetParentActiveFlag(m_bActive);
					pCell->SetDrawParentActiveParams(0, 0, m_ColorActive);					
				}
			}
		}
	}
}

void CIoViewTimeSale::UpDateFullGridNewestData()
{
	ASSERT( 0 );  //这个函数没被调用过

	if ( !m_bShowFull || !m_bShowNewestFull )
	{
		// 不是最大化显示,或者最新价不在显示之中的时候,直接返回:
		return;
	}

	if ( m_aTicksShow.GetSize() <= 0 )
	{
		return;
	}

	//
	if ( m_aTicksShow.GetSize() >= m_iDataFullNums )
	{
		// 满屏显示: 滚动到最新数据
		OnPageScroll(INT_MAX);
	}
	else
	{
		// 没有显示满: 还是一样的显示页尾
		OnPageScroll(INT_MAX);
	}
	
	if ( !BeValidFullGridIndex() )
	{
		// 索引值错误
		return;
	}

	// 
	RedrawWindow();
}

void CIoViewTimeSale::CalcFullGridParams()
{
	m_iPerfectWidth		= 0;	
	m_iGridFullRow		= 0;		
	m_iGridFullCol		= 0;		
	m_iDataFullNums		= 0;
	m_iGridFullColUints	= 0;
	//
	CClientDC dc(this);

	CRect RectGrid;
	m_GridCtrlFull.GetClientRect(&RectGrid);
		
	// 设置行高
	CFont * pFontNormal = GetIoViewFontObject(ESFNormal);
	CFont* pOldFont = dc.SelectObject(pFontNormal);
	CSize size = dc.GetTextExtent(L"123:456:789");

	if ( size.cy <= 0 )
	{
		return;
	}

	// 这个时机要早于SetHead. 所以自己算类型,不用类成员变量
	E_TimeSaleHeadType eTimeSaleHeadType = m_eTimeSaleHeadType;

	if ( NULL != m_pMerchXml )
	{
		eTimeSaleHeadType = GetMerchTimeSaleHeadType(m_pMerchXml->m_Market.m_MarketInfo.m_eMarketReportType);
	}	
	// 
	if ( ETHTSimple == eTimeSaleHeadType )
	{
		int iCol = NoShowNewvol()?2:3;
		m_iPerfectWidth		= iCol * size.cx;
		m_iGridFullColUints	= (RectGrid.Width() / m_iPerfectWidth);
		m_iGridFullCol		= m_iGridFullColUints * iCol; 
	}
	else if ( ETHTFuture == eTimeSaleHeadType )
	{
		m_iPerfectWidth		= 5 * size.cx;
		m_iGridFullColUints	= (RectGrid.Width() / m_iPerfectWidth);
		m_iGridFullCol		= m_iGridFullColUints * 5; 
	}

	//
	int32 iRowHeight0 = size.cy + 6;
	m_iGridFullRow    = (RectGrid.Height() - iRowHeight0) / (size.cy + 2);
	m_iGridFullRow	 += 1;

	// 
	m_iDataFullNums  = (m_iGridFullRow - 1) * m_iGridFullColUints; 
	
	//
	dc.SelectObject(pOldFont);

	if (m_iGridFullColUints <= 1)
	{
		SetShowType(false);
	}
	else
	{
		SetShowType(true);
	}

	//
	if ( m_iGridFullRow > 0 && m_iGridFullCol > 0 )
	{
		m_GridCtrlFull.SetRowCount(m_iGridFullRow);
		m_GridCtrlFull.SetColumnCount(m_iGridFullCol);

		m_GridCtrlFull.SetHeaderSort(FALSE);
		m_GridCtrlFull.SetFixedRowCount(1);		
	}	
}

bool32 CIoViewTimeSale::BeValidFullGridIndex()
{
	// 错误的情况
	if ( m_iFullBeginIndex < 0 || m_iFullBeginIndex >= m_aTicksShow.GetSize() )
	{		
		return false;
	}
	
	if ( m_iFullEndIndex < 0 || m_iFullEndIndex >= m_aTicksShow.GetSize() )
	{
		return false;
	}
	
	if ( m_iFullBeginIndex > m_iFullEndIndex )
	{
		return false;
	}
	
	if ( m_aTicksShow.GetSize() >= m_iDataFullNums )
	{
		if ( (m_iFullEndIndex - m_iFullBeginIndex + 1) != m_iDataFullNums )
		{
			return false;
		}
	}

	return true;
}

void CIoViewTimeSale::SetRowHeightAccordingFont()
{
	CClientDC dc(this);
	
	// 设置行高
	CFont * pFontNormal = GetIoViewFontObject(ESFNormal);
	CFont* pOldFont = dc.SelectObject(pFontNormal);
	CSize size = dc.GetTextExtent(L"一二");
	int32 iRowHeight0 = size.cy + 6;
	dc.SelectObject(pOldFont);

 	int32 iRowHeight = size.cy + 6; 
 
	int32 i = 0;
	for ( i = m_GridCtrl.GetRowCount() - 1; i > 0; i-- )
	{
		//
		m_GridCtrl.SetRowHeight(i, iRowHeight);
	}
	
	//
	for (i = m_GridCtrlFull.GetRowCount() - 1; i > 0; i-- )
	{
		//
		m_GridCtrlFull.SetRowHeight(i, iRowHeight);
	}
	
	if ( m_GridCtrl.GetRowCount() > 0 )
	{
		m_GridCtrl.SetRowHeight(0, iRowHeight0);
	}	

	if ( m_GridCtrlFull.GetRowCount() > 0 )
	{
		m_GridCtrlFull.SetRowHeight(0, iRowHeight0);
	}	
}

bool32 CIoViewTimeSale::FromXml(TiXmlElement * pElement)
{
	if (NULL == pElement)
		return false;

	// 判读是不是IoView的节点
	const char *pcValue = pElement->Value();
	if (NULL == pcValue || strcmp(GetXmlElementValue(), pcValue) != 0)
		return false;

	// 判断是不是描述自己这个业务视图的节点
	const char *pcAttrValue = pElement->Attribute(GetXmlElementAttrIoViewType());
	if (NULL == pcAttrValue || CIoViewManager::GetIoViewString(this).Compare(CString(pcAttrValue)) != 0)	// 不是描述自己的业务节点
		return false;
	
	// 读取本业务视图特有的内容
	SetFontsFromXml(pElement);
	SetColorsFromXml(pElement);
	SetRowHeightAccordingFont();

	//
	int32 iMarketId			= -1;
	CString StrMerchCode	= L"";

	pcAttrValue = pElement->Attribute(GetXmlElementAttrMarketId());
	if (NULL != pcAttrValue)
	{
		iMarketId = atoi(pcAttrValue);
	}

	pcAttrValue = pElement->Attribute(GetXmlElementAttrMerchCode());
	if (NULL != pcAttrValue)
	{
		StrMerchCode = pcAttrValue;
	}

	// 
	CMerch *pMerchFound = NULL;
	if (!m_pAbsCenterManager->GetMerchManager().FindMerch(StrMerchCode, iMarketId, pMerchFound))
	{
		pMerchFound = NULL;
	}
	
	// 商品发生改变
	OnVDataMerchChanged(iMarketId, StrMerchCode, pMerchFound);	

	return true;
}

CString CIoViewTimeSale::ToXml()
{
	CString StrThis;

	CString StrMarketId;
	StrMarketId.Format(L"%d", m_MerchXml.m_iMarketId);

	// 
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" ",/*>\n*/ 
		CString(GetXmlElementValue()).GetBuffer(), 
		CString(GetXmlElementAttrIoViewType()).GetBuffer(), 
		CIoViewManager::GetIoViewString(this).GetBuffer(),
		CString(GetXmlElementAttrShowTabName()).GetBuffer(),
		m_StrTabShowName.GetBuffer(),
		CString(GetXmlElementAttrMerchCode()).GetBuffer(),
		m_MerchXml.m_StrMerchCode.GetBuffer(),
		CString(GetXmlElementAttrMarketId()).GetBuffer(), 
		StrMarketId.GetBuffer());
	//
	CString StrFace;
	StrFace  = SaveColorsToXml();
	StrFace += SaveFontsToXml();
	
	StrThis += StrFace;
	StrThis += L">\n";
	//
	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";

	return StrThis;
}

CString CIoViewTimeSale::GetDefaultXML()
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
	StrThis += GetXmlElementValue();
	StrThis += L">\n";

	return StrThis;
}

void CIoViewTimeSale::DeleteTableContent()
{
	if ( NULL != m_GridCtrl.GetSafeHwnd() && !m_bShowFull )
	{
		m_GridCtrl.DeleteNonFixedRows();		
	}		

	if ( NULL != m_GridCtrlFull.GetSafeHwnd() && m_bShowFull )
	{
		m_GridCtrlFull.DeleteNonFixedRows();
	}

	SetRowHeightAccordingFont();
	RedrawWindow();
}

void CIoViewTimeSale::OnIoViewActive()
{
	m_bActive = IsActiveInFrame();
	//OnVDataForceUpdate();
	RequestLastIgnoredReqData();

	//TRACE(_T("active :%d\r\n"), m_bActive);
	//
	SetActiveCell();
	if ( m_bShowFull )
	{
		if ( m_GridCtrlFull.GetSafeHwnd() )
		{
			m_GridCtrlFull.RedrawWindow();
		}
	}
	else
	{
		if ( m_GridCtrl.GetSafeHwnd() )
		{
			m_GridCtrl.RedrawWindow();
		}
	}

	//
	if ( NULL != m_GridCtrl.GetSafeHwnd() )
	{
		m_GridCtrl.SetFocus();
	}
	
	SetChildFrameTitle();
}

void CIoViewTimeSale::OnIoViewDeactive()
{
	m_bActive = false;

	//
	SetActiveCell();

	if ( m_bShowFull )
	{
		if ( m_GridCtrlFull.GetSafeHwnd() )
		{
			m_GridCtrlFull.RedrawWindow();
		}
	}
	else
	{
		if ( m_GridCtrl.GetSafeHwnd() )
		{
			m_GridCtrl.RedrawWindow();
		}
	}
}

void  CIoViewTimeSale::SetChildFrameTitle()
{
	CString StrTitle;
	StrTitle =  CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;

	if (NULL != m_pMerchXml)
	{	
		StrTitle += L" ";
		StrTitle += m_pMerchXml->m_MerchInfo.m_StrMerchCnName;
	}
	
	CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pParentFrame)
	{
		pParentFrame->SetChildFrameTitle(StrTitle);
	}
}


int32 CIoViewTimeSale::CalcMaxVisibleTimeSaleCount()
{
	if ( !IsWindow(m_GridCtrl.GetSafeHwnd()) || !IsWindow(m_GridCtrlFull.GetSafeHwnd()) )
	{
		return 0;
	}
	
	if ( m_GridCtrl.IsWindowVisible() )
	{
		// 普通模式
		CClientDC dc(this);
		
		CRect RectGrid;
		m_GridCtrl.GetClientRect(&RectGrid);
		
		// 设置行高
		CFont * pFontNormal = GetIoViewFontObject(ESFNormal);
		CFont* pOldFont = dc.SelectObject(pFontNormal);
		CSize size = dc.GetTextExtent(L"最高");
		
		if ( size.cy <= 0 )
		{
			return 0;
		}
		
		//
		int32 iGridFullRow;
		int32 iRowHeight0 = size.cy + 6;
		iGridFullRow    = (RectGrid.Height() - iRowHeight0) / (size.cy + 2);
		iGridFullRow	 += 1;
		
		// 
		//
		dc.SelectObject(pOldFont);
		
		return iGridFullRow;
		//return KNormalShowTimeSaleCount;
	}
	else
	{
		// FullShow
		CClientDC dc(this);
		
		CRect RectGrid;
		m_GridCtrlFull.GetClientRect(&RectGrid);
		
		// 设置行高
		CFont * pFontNormal = GetIoViewFontObject(ESFNormal);
		CFont* pOldFont = dc.SelectObject(pFontNormal);
		CSize size = dc.GetTextExtent(L"一二三四");
		
		if ( size.cy <= 0 )
		{
			return 0;
		}
		
		// 这个时机未知. 所以自己算类型,不用类成员变量
		E_TimeSaleHeadType eTimeSaleHeadType = m_eTimeSaleHeadType;
		
		if ( NULL != m_pMerchXml )
		{
			eTimeSaleHeadType = GetMerchTimeSaleHeadType(m_pMerchXml->m_Market.m_MarketInfo.m_eMarketReportType);
		}	
		
		int32 iPerfectWidth,  iGridFullCol;
		int32 iGridFullColUnits = 0;
		int32 iGridFullRow, iDataFullNums;
		//
		if ( ETHTSimple == eTimeSaleHeadType )
		{
			int iCol = NoShowNewvol()? 2:3;
			iPerfectWidth		= iCol * size.cx;
			iGridFullColUnits	= (RectGrid.Width() / iPerfectWidth);
			iGridFullCol		= iGridFullColUnits * iCol; 
		}
		else if ( ETHTFuture == eTimeSaleHeadType )
		{
			iPerfectWidth		= 5 * size.cx;
			iGridFullColUnits	= (RectGrid.Width() / iPerfectWidth);
			iGridFullCol		= iGridFullColUnits * 5; 
		}
		
		//
		int32 iRowHeight0 = size.cy + 6;
		iGridFullRow    = (RectGrid.Height() - iRowHeight0) / (size.cy + 2);
		iGridFullRow	 += 1;
		
		// 
		iDataFullNums  = (iGridFullRow - 1) * iGridFullColUnits; 
		//
		dc.SelectObject(pOldFont);
		
		return iDataFullNums;
	}
	
	return 0;
}

void CIoViewTimeSale::OnPageScroll( int32 iScrollPage )
{
	// 如果是0则，看本地数据是否不足，不足则申请一下
	// 如果是 KForceRequestTimeSalePage 则强制申请当前数据量的数据
	if ( NULL == m_pMerchXml )
	{
		return;
	}

	if ( !m_bShowFull )
	{
		// 普通 最上为最新
		// 不用做处理，以前的代码可以自己切换
	}
	else
	{
		//if ( !m_GridCtrlFull.IsWindowVisible() || !IsWindowVisible() )
		//{
		//	ASSERT( 0 );  // 不应该出现的，但是它就是有，很悲剧
		//}

		// Full 最下为最新
		CGmtTime TimeInit	= m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time;
		CGmtTime TimeEnd	= m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeEnd.m_Time;//TimeInit + CGmtTimeSpan(0, 23, 59, 59);
		
		int32 iMaxVisibleDataCount = m_iDataFullNums;
		if ( iMaxVisibleDataCount < 1 )
		{
			// 如果这个时候没有得到初始化的话，计算一下 - 应该是size后才调用的
			ASSERT( 0 );
			iMaxVisibleDataCount = CalcMaxVisibleTimeSaleCount();
		}

		int32 iRequestDataCount = iMaxVisibleDataCount;//MAX(iMaxVisibleDataCount, KMinRequestTimeSaleCount);
		if ( iRequestDataCount <= 0 )
		{
			ASSERT( 0 );
			iMaxVisibleDataCount = 1;
			iRequestDataCount = KMinRequestTimeSaleCount;
		}

		// 申请原则：由于merch中Tick数据有缓存，所以在UpdateTimeSale里会有全部的缓存数据过来，如果不是采用连续的数据申请的话
		//		数据就会在上下翻页的时候出现断层，导致判断是否需要申请数据失败，而且也不能判定是否翻到了最新页
		//      不需要申请向下的数据，因为每次MerchChange时，会申请一次最新的数据，而前面所有的数据都会在前翻时申请
		if ( KForceRequestTimeSalePage == iScrollPage )
		{
			// 强制申请数据 - 不改变显示区域
			int iTickSize = m_aTicksShow.GetSize();
			iRequestDataCount = MAX(iRequestDataCount, iTickSize);
			CMmiReqMerchTimeSales info;
			info.m_iMarketId	= m_pMerchXml->m_MerchInfo.m_iMarketId;
			info.m_StrMerchCode = m_pMerchXml->m_MerchInfo.m_StrMerchCode;
			
			info.m_eReqTimeType	= ERTYFrontCount;
			info.m_TimeSpecify	= TimeEnd;
			info.m_iFrontCount  = iRequestDataCount;
			
			RequestTimeSaleData(info);
			return; // 等待数据回来在Update里面调用SetTable
		}
		else if ( INT_MIN == iScrollPage )
		{
			// 页首
			// 申请所有 数据 - 有可能所有数据都不足一页显示，所以需要SetTableData在之中辅组设置Newest标志
			CMmiReqMerchTimeSales info;
			info.m_iMarketId	= m_pMerchXml->m_MerchInfo.m_iMarketId;
			info.m_StrMerchCode = m_pMerchXml->m_MerchInfo.m_StrMerchCode;
			
			info.m_eReqTimeType	= ERTYSpecifyTime;
			info.m_TimeStart	= TimeInit;
			info.m_TimeEnd      = TimeEnd;
			
			RequestTimeSaleData(info);
			
			m_iFullBeginIndex = 0;
			m_iFullEndIndex   = m_iFullBeginIndex + iMaxVisibleDataCount - 1;
			m_bShowNewestFull = false;
			SetTableData();
			return;
		}
		else if ( INT_MAX == iScrollPage || m_aTicksShow.GetSize() < iRequestDataCount )
		{
			// 页尾(数据不足也是页尾！)
			if ( m_aTicksShow.GetSize() < iRequestDataCount )
			{
				// 数据不足，申请页尾 n 条数据
				CMmiReqMerchTimeSales info;
				info.m_iMarketId	= m_pMerchXml->m_MerchInfo.m_iMarketId;
				info.m_StrMerchCode = m_pMerchXml->m_MerchInfo.m_StrMerchCode;
				
				info.m_eReqTimeType	= ERTYFrontCount;
				info.m_TimeSpecify	= TimeEnd;
				info.m_iFrontCount  = iRequestDataCount;
				
				RequestTimeSaleData(info);
			}
			
			//m_iFullEndIndex = ??
			m_bShowNewestFull = true;
			// 具体值在SetTableData中辅助修正
			m_iFullBeginIndex = -1;
			m_iFullEndIndex   = -1;
			SetTableData();
			return;
		}
		else
		{
			// 以现在的位置开始卷动
			// 现在假设本地是有连续数据（含最新）的
			// 0 滚动 - 即放缩，放缩以m_iFullEndIndex为基础坐标

			int32 iStartPos = m_iFullBeginIndex + iScrollPage * iMaxVisibleDataCount;
			int32 iEndPos   = iStartPos + iMaxVisibleDataCount - 1;

			if ( 0 == iScrollPage )
			{
				// 0 - 放缩，以 m_iFullEndIndex 为基础坐标
				iEndPos = m_iFullEndIndex;
				iStartPos = iEndPos - iMaxVisibleDataCount + 1;
			}
			
			if ( iEndPos > m_aTicksShow.GetSize()-1 )
			{
				// 尾部数据不足 - 因为已经有了最新的数据，所以只做调整显示
				iEndPos = m_aTicksShow.GetSize() - 1;
				iStartPos = iEndPos - iMaxVisibleDataCount + 1;
			}
			else if ( iStartPos < 0 )
			{
				// 头部数据不足 - 以最后的时间做起点，向前申请 现有数据 + 所需数据 个 - 因为分笔的时间精确的特殊性质

				int32 iNeedDataCount = -iStartPos;
				iStartPos += iNeedDataCount;		// 0
				iEndPos	  += iNeedDataCount;        // iMaxVisibleDataCount;
				
				if ( !m_bRequestedTodayFirstData )
				{
					// TRACE(_T("---------TimeSale request header data\r\n"));

					CMmiReqMerchTimeSales info;
					info.m_iMarketId	= m_pMerchXml->m_MerchInfo.m_iMarketId;
					info.m_StrMerchCode = m_pMerchXml->m_MerchInfo.m_StrMerchCode;
					
					info.m_eReqTimeType	= ERTYFrontCount;
					info.m_TimeSpecify	= TimeEnd;
					info.m_iFrontCount  = iRequestDataCount + m_aTicksShow.GetSize();
					
					RequestTimeSaleData(info);
				}
				else
				{
					// TRACE(_T("---------TimeSale DONOT request header data\r\n"));
					// 今天的最前数据已经申请过了，不必要在申请了，调整一下显示区域就可以了
				}
			}
			else
			{
				// 应该是在现有数据里面的
			}

			if ( iEndPos > m_aTicksShow.GetSize()-1 )
			{
				iEndPos = m_aTicksShow.GetSize() - 1;
			}
			m_iFullBeginIndex = iStartPos;
			m_iFullEndIndex   = iEndPos;
			if ( iStartPos <= m_aTicksShow.GetSize() && iEndPos >= m_aTicksShow.GetSize()-1 )
			{
				m_bShowNewestFull = true;
			}
			else
			{
				m_bShowNewestFull = false;
			}

			SetTableData();
		}
	}
}

void CIoViewTimeSale::RequestViewData()
{
	if (NULL == m_pMerchXml)
		return;

// 	if ( !IsWindowVisible() )
// 	{
// 		//TRACE(_T("---------Ignore Invisible request!!\r\n"));
// 		return;
// 	}
	
	CGmtTime TimeInit	= m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeInit.m_Time;
	CGmtTime TimeEnd	= m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeEnd.m_Time;
	
	CGmtTime TimeServer = m_pAbsCenterManager->GetServerTime();
	if (!m_bHistoryTimeSale && (TimeServer < TimeInit || TimeServer > TimeEnd))	// 不是同一天， 需要重新算时间和昨收价等
	{
		// 
		m_iSaveDec = m_pMerchXml->m_MerchInfo.m_iSaveDec;
		
		// 获取该商品走势相关的数据
		CMarketIOCTimeInfo RecentTradingDay;
		if (m_pMerchXml->m_Market.GetRecentTradingDay(m_pAbsCenterManager->GetServerTime(), RecentTradingDay, m_pMerchXml->m_MerchInfo))
		{
			m_TrendTradingDayInfo.Set(m_pMerchXml, RecentTradingDay);
			m_TrendTradingDayInfo.RecalcPrice(*m_pMerchXml);
			m_TrendTradingDayInfo.RecalcHold(*m_pMerchXml);

			// 判断现有数据是否属于获得的临近交易日内交易时段的数据，不是，则清空该数据
			if ( m_aTicksShow.GetSize() > 0 )
			{
				// 分笔的时间应当大于当天市场的初始化时间 并且小于 其End时间
				if ( RecentTradingDay.m_TimeInit >  m_aTicksShow[0].m_TimeCurrent || RecentTradingDay.m_TimeEnd < m_aTicksShow[0].m_TimeCurrent )
				{
					// 交易日时间比现有数据早 或者 比现有数据晚
					// 先清掉当前显示内容
					DeleteTableContent();
					
					// 清掉当前数据
					m_aTicksShow.SetSize(0);
				}
			}
		}
	}
	
	// 请求指定时间的日K线， 以便获取当前显示走势图的昨收价， 今开价
	{
		ASSERT(m_TrendTradingDayInfo.m_bInit);
		
		// 
		CGmtTime TimeDay = m_TrendTradingDayInfo.m_MarketIOCTime.m_TimeClose.m_Time;
		SaveDay(TimeDay);
		
		CMmiReqMerchKLine info;
		info.m_eKLineTypeBase	= EKTBDay;
		info.m_iMarketId		= m_pMerchXml->m_MerchInfo.m_iMarketId;
		info.m_StrMerchCode		= m_pMerchXml->m_MerchInfo.m_StrMerchCode;
		info.m_eReqTimeType		= ERTYFrontCount;		// 为防止跨天，请求昨天&今天
		info.m_TimeStart		= TimeDay;
		//info.m_TimeEnd		= TimeDay;
		info.m_iFrontCount		= 2;			
		DoRequestViewData(info);
	}
	
	// 发RealtimePrice请求, 为取昨收价
	{
		CMmiReqRealtimePrice Req;
		Req.m_iMarketId			= m_pMerchXml->m_MerchInfo.m_iMarketId;
		Req.m_StrMerchCode		= m_pMerchXml->m_MerchInfo.m_StrMerchCode;
		DoRequestViewData(Req);
	}
	
	// 请求该商品的数据
	{
		if ( m_bShowFull )
		{
			// 判断一下是否需要申请数据 xl 0601 强制申请最后的历史数据
			OnPageScroll(KForceRequestTimeSalePage);
		}
		else
		{
			// 普通情况下，申请最新100个数据
			CMmiReqMerchTimeSales info;
			info.m_iMarketId	= m_pMerchXml->m_MerchInfo.m_iMarketId;
			info.m_StrMerchCode = m_pMerchXml->m_MerchInfo.m_StrMerchCode;
			
			info.m_eReqTimeType	= ERTYFrontCount;
			info.m_TimeSpecify	= TimeEnd;
			info.m_iFrontCount  = KNormalShowTimeSaleCount;
			
			DoRequestViewData(info);
		}
		
	}
	
	// 发RealtimeTick请求
	{
		CMmiReqRealtimeTick Req;
		Req.m_iMarketId			= m_pMerchXml->m_MerchInfo.m_iMarketId;
		Req.m_StrMerchCode		= m_pMerchXml->m_MerchInfo.m_StrMerchCode;
		DoRequestViewData(Req);
	}
	
}

void CIoViewTimeSale::RequestTimeSaleData( CMmiReqMerchTimeSales &req )
{
	DoRequestViewData(req);
}

void CIoViewTimeSale::SetHistoryTime( const CGmtTime &TimeAttend )
{
	m_bHistoryTimeSale	= true;
	m_TimeHistory	= TimeAttend;
	CMarketIOCTimeInfo RecentTradingDay;
	if ( NULL != m_pMerchXml 
		&& m_pMerchXml->m_Market.GetSpecialTradingDayTime(m_TimeHistory, RecentTradingDay, m_pMerchXml->m_MerchInfo)
		)
	{
		m_TrendTradingDayInfo.Set(m_pMerchXml, RecentTradingDay);
		m_TrendTradingDayInfo.RecalcPrice(*m_pMerchXml);
		m_TrendTradingDayInfo.RecalcHold(*m_pMerchXml);

		if ( m_GridCtrl.m_hWnd != NULL )
		{
			m_GridCtrl.EnsureVisible(0, 0);		// 不能保证总是第一个可见
			DeleteTableContent();		// 清除现有数据
		}
	}
	// 错过这次时机就只剩下merchchanged时机
}

bool32 CIoViewTimeSale::GetCurrentPerfectWidth( OUT int32 &iWidth )
{
	if ( m_iPerfectWidth != 0 )
	{
		iWidth = m_iPerfectWidth;
		return true;
	}
	// 是否尝试计算？？
	return false;
}

void TestRecordData(CArray<CTick, CTick>& aTick)
{
	return;
	static long lIndex = 0;
	
	FILE* pFile = NULL;
	if ( lIndex == 0 )
	{
		pFile = fopen("c:\\Tick.txt", "w");
	}
	else
	{
		pFile = fopen("c:\\Tick.txt", "aw+");
	}
	
	if ( NULL == pFile )
	{
		return;			
	}
	
	int32 iMax = 1000;
	
	fprintf(pFile, "================== beging[%d] ================== \r\n", lIndex);
	//
	for ( int32 i = aTick.GetSize() - 1; i >= 0; i--)
	{
		CTick Tick = aTick[i];	
		
		CString StrLog = L"[%05d]时间: [%ld]%02d:%02d 价格: %.2f 现手: %.0f \r\n";
		//StrLog.Format(L"[%05d]时间: [%ld]%02d:%02d 价格: %.2f 现手: %.0f \r\n", i, Tick.m_TimeCurrent.m_Time.GetTime(), Tick.m_TimeCurrent.m_Time.GetMinute(), Tick.m_TimeCurrent.m_Time.GetSecond(), Tick.m_fPrice, Tick.m_fVolume);
		//
		char* pStrLog = NULL;
		USES_CONVERSION;
		//
		pStrLog = _W2A(StrLog);
		
		fprintf(pFile, pStrLog, i, Tick.m_TimeCurrent.m_Time.GetTime(), Tick.m_TimeCurrent.m_Time.GetMinute(), Tick.m_TimeCurrent.m_Time.GetSecond(), Tick.m_fPrice, Tick.m_fVolume);

		iMax--;
		if ( iMax <= 0 )
		{
			break;
		}
	}
	
	fprintf(pFile, "================== end[%d] ================== \r\n", lIndex);	
	fclose(pFile);

	lIndex++;
}

void CIoViewTimeSale::DoShowStdPopupMenu()
{
	CNewMenu Menu;
	Menu.LoadMenu(IDR_MENU_TIMESALE);
	Menu.LoadToolBar(g_awToolBarIconIDs);
	CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, Menu.GetSubMenu(0));

	pPopMenu->EnableMenuItem(ID_EXPORT_DATA,true);
	CMenu* pTempMenu = pPopMenu->GetSubMenu(L"插入内容");
	CNewMenu * pIoViewPopMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );
	ASSERT(NULL != pIoViewPopMenu );
	CIoViewBase::AppendIoViewsMenu(pIoViewPopMenu, IsLockedSplit());

	// 如果处在锁定分割状态，需要删除一些按钮
	CMPIChildFrame *pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pChildFrame && pChildFrame->IsLockedSplit() )
	{
		pChildFrame->RemoveSplitMenuItem(*pPopMenu);
	}

	CPoint pt;
	GetCursorPos(&pt);
	pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, AfxGetMainWnd());	
	pPopMenu->DestroyMenu();	
}

void CIoViewTimeSale::OnExportData()
{
	if (m_pMerchXml != NULL)
	{
		CDialogExportSaleData dlg;
		dlg.SetMerchInfo(m_pMerchXml->m_MerchInfo);
		dlg.DoModal();
	}
}

bool32 CIoViewTimeSale::NoShowNewvol()
{
	E_ReportType eMerchKind = GetMerchKind(m_pMerchXml);
	if ((ERTMony == eMerchKind) && 0 == CConfigInfo::Instance()->m_bShowNewvol)	// 判断是否是“做市商”
	{
		return true;
	}
	return false;
}

void CIoViewTimeSale::OnTimer(UINT nIDEvent)
{
	if ( nIDEvent == KTimerIdRefresh )
	{
		KillTimer(KTimerIdRefresh);
		SetTableData();
		m_bSetTimer = FALSE;
	}
	CIoViewBase::OnTimer(nIDEvent);
}