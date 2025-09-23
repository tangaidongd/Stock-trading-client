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
#include "IoViewTimeSale.h"
#include "GridCellSymbol.h"
#include "GridCellLevel2.h"
#include "GridCellCheck.h"
#include "DlgBlockSelect.h"
#include "DlgBlockSet.h"
#include "ColorStep.h"
#include "GridCtrlNormal.h"
#include "XLTraceFile.h"
#include "IoViewTimeSaleStatistic.h"
#include "LogFunctionTime.h"

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
//    "fatal error C1001: INTERNAL COMPILER ERROR"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int32 KUpdatePushMerchsTimerId	= 100005;			// 每隔 n 秒钟, 重新计算一次当前的推送商品
const int32 KTimerPeriodPushMerchs		= 1000 * 60 *2;

const int32	KSortAsyncTimerId			= 1006;
const int32	KSortAsyncTimerPeriod		= 50;
#define ID_GRID_CONTROL	10205

#define COLO_NO_NAME_CODE  RGB(240,248,136)

namespace
{
#define  INVALID_ID  -1
#define GRIDCTRL_MOVEDOWN_SIZE	    50

#define MID_BUTTON_PAIHANG_START	10001
#define MID_BUTTON_PAIHANG_PROMPT	10002
#define MID_BUTTON_PAIHANG_ONE		10003
#define MID_BUTTON_PAIHANG_THREE	10004
#define MID_BUTTON_PAIHANG_FIVE		10005
#define MID_BUTTON_PAIHANG_TEN		10006
#define MID_BUTTON_PAIHANG_END		10010
}

//////////////////////////////////////////////////////////////////////////
// 表头字段
const T_TimeSaleStatisticHeader KTimeSaleStatisticHeaders[]	=	
{
	T_TimeSaleStatisticHeader(_T("代码"),	ETSSHCode,		ERSPIEnd),		// 
	T_TimeSaleStatisticHeader(_T("名称"),	ETSSHName,		ERSPIEnd),		// 
	T_TimeSaleStatisticHeader(_T("最新\n(元)"), ETSSHPriceNew,	ERSPIEnd),		// 最新价排行需要另外的申请

	T_TimeSaleStatisticHeader(_T("超大单\n净买入\n(万股)"), ETSSHVolNetSuper, ERSPIExBigNetBuyVolume),		// 

	T_TimeSaleStatisticHeader(_T("超大单\n买入量\n(万股)"), ETSSHVolBuySuper, ERSPIExBigBuyVol),		// 
	T_TimeSaleStatisticHeader(_T("超大单\n卖出量\n(万股)"), ETSSHVolSellSuper, ERSPIExBigSellVol),		// 

	T_TimeSaleStatisticHeader(_T("大单\n买入量\n(万股)"), ETSSHVolBuyBig, ERSPIBigBuyVol),		// 
	T_TimeSaleStatisticHeader(_T("大单\n卖出量\n(万股)"), ETSSHVolSellBig, ERSPIBigSellVol),		// 

	T_TimeSaleStatisticHeader(_T("中单\n买入量\n(万股)"), ETSSHVolBuyMid, ERSPIMidBuyVol),		// 
	T_TimeSaleStatisticHeader(_T("中单\n买入量\n(万股)"), ETSSHVolSellMid, ERSPIMidSellVol),		// 

	T_TimeSaleStatisticHeader(_T("小单\n买入量\n(万股)"), ETSSHVolBuySmall, ERSPISmallBuyVol),		// 
	T_TimeSaleStatisticHeader(_T("小单\n买入量\n(万股)"), ETSSHVolSellSmall, ERSPISmallSellVol),		// 

	T_TimeSaleStatisticHeader(_T("中小单\n买入量\n(万股)"), ETSSHVolBuyMidSmall, ERSPIMnSBuyVol),		// 
	T_TimeSaleStatisticHeader(_T("中小单\n卖出量\n(万股)"), ETSSHVolSellMidSmall, ERSPIMnSSellVol),		// 

	T_TimeSaleStatisticHeader(_T("超大单\n买入额\n(万元)"), ETSSHAmountBuySuper, ERSPIExBigBuyAmount),		// 
	T_TimeSaleStatisticHeader(_T("超大单\n卖出额\n(万元)"), ETSSHAmountSellSuper, ERSPIExBigSellAmount),		// 
	T_TimeSaleStatisticHeader(_T("超大单\n净额\n(万元)"), ETSSHNetAmountSuper, ERSPIExBigNetAmount),		// 

	T_TimeSaleStatisticHeader(_T("大单\n买入额\n(万元)"), ETSSHAmountBuyBig, ERSPIBigBuyAmount),		// 
	T_TimeSaleStatisticHeader(_T("大单\n卖出额\n(万元)"), ETSSHAmountSellBig, ERSPIBigSellAmount),		// 
	T_TimeSaleStatisticHeader(_T("大单\n净额\n(万元)"), ETSSHNetAmountBig, ERSPIBigNetAmount),		// 

	T_TimeSaleStatisticHeader(_T("中单\n买入额\n(万元)"), ETSSHAmountBuyMid, ERSPIMidBuyAmount),		// 
	T_TimeSaleStatisticHeader(_T("中单\n卖出额\n(万元)"), ETSSHAmountSellMid, ERSPIMidSellAmount),		// 
	T_TimeSaleStatisticHeader(_T("中单\n净额\n(万元)"), ETSSHNetAmountMid, ERSPIMidNetAmount),		// 

	T_TimeSaleStatisticHeader(_T("小单\n买入额\n(万元)"), ETSSHAmountBuySmall, ERSPISmallBuyAmount),		// 
	T_TimeSaleStatisticHeader(_T("小单\n卖出额\n(万元)"), ETSSHAmountSellSmall, ERSPISmallSellAmount),		// 
	T_TimeSaleStatisticHeader(_T("小单\n净额\n(万元)"), ETSSHNetAmountSmall, ERSPISmallNetAmount),		// 

	T_TimeSaleStatisticHeader(_T("买入\n总金额\n(万元)"), ETSSHAmountTotalBuy, ERSPITotalBuyAmount),		// 
	T_TimeSaleStatisticHeader(_T("卖出\n总金额\n(万元)"), ETSSHAmountTotalSell, ERSPITotalSellAmount),		// 

	T_TimeSaleStatisticHeader(_T("超大单\n换手率\n(%)"), ETSSHTradeRateSuper, ERSPIExBigChange),		// 
	T_TimeSaleStatisticHeader(_T("大单\n换手率\n(%)"), ETSSHTradeRateBig, ERSPIBigChange),		// 
	T_TimeSaleStatisticHeader(_T("买入大单\n换手率\n(%)"), ETSSHTradeRateBuyBig, ERSPIBigBuyChange),		// 

	T_TimeSaleStatisticHeader(_T("成交笔数"), ETSSHTradeCount, ERSPITradeCounts),		// 
	T_TimeSaleStatisticHeader(_T("每笔金额\n(万元)"), ETSSHAmountPerTrans, ERSPIAmountPerTrade),		// 
	T_TimeSaleStatisticHeader(_T("每笔股数\n(股)"), ETSSHStocksPerTrans, ERSPIStockPerTrade),		// 

	T_TimeSaleStatisticHeader(_T("资金流向\n(万元)"), ETSSHCapitalFlow, ERSPIEnd),		// 资金流向排行另外申请
};

const int32 KTimeSaleStatisticHeadersCount = sizeof(KTimeSaleStatisticHeaders) / sizeof(KTimeSaleStatisticHeaders[0]);

//////////////////////////////////////////////////////////////////////////
CIoViewTimeSaleStatistic *CIoViewTimeSaleStatistic::m_pThis = NULL;
	
IMPLEMENT_DYNCREATE(CIoViewTimeSaleStatistic, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewTimeSaleStatistic, CIoViewBase)
//{{AFX_MSG_MAP(CIoViewTimeSaleStatistic)
ON_WM_PAINT()
ON_WM_ERASEBKGND()
ON_WM_CREATE()
ON_WM_SIZE()
ON_WM_TIMER()
ON_WM_CONTEXTMENU()
ON_WM_HSCROLL()
ON_WM_LBUTTONUP()
ON_WM_LBUTTONDOWN()
ON_MESSAGE_VOID(UM_DOINITIALIZE, OnDoInitialize)
ON_MESSAGE(UM_DOTRACEMENU, OnDoTrackMenu)
ON_MESSAGE(UM_POS_CHANGED, OnScrollPosChanged)
//}}AFX_MSG_MAP
ON_NOTIFY(NM_RCLICK,ID_GRID_CONTROL,OnGridRButtonDown)
ON_NOTIFY(NM_DBLCLK, ID_GRID_CONTROL, OnGridDblClick)
ON_NOTIFY(GVN_COLWIDTHCHANGED, ID_GRID_CONTROL, OnGridColWidthChanged)
ON_NOTIFY(GVN_KEYDOWNEND, ID_GRID_CONTROL, OnGridKeyDownEnd)
ON_NOTIFY(GVN_KEYUPEND, ID_GRID_CONTROL, OnGridKeyUpEnd)
ON_NOTIFY(GVN_GETDISPINFO, ID_GRID_CONTROL, OnGridGetDispInfo)
END_MESSAGE_MAP()
///////////////////////////////////////////////////////////////////////////////

// 准备优化 fangz20100514

CIoViewTimeSaleStatistic::CIoViewTimeSaleStatistic()
:CIoViewBase()
{
	m_pParent		= NULL;
	m_rectClient	= CRect(-1,-1,-1,-1);

	m_MmiRequestSys.m_iMarketId = -1;
	m_MmiRequestSys.m_iStart = 0;
	m_MmiRequestSys.m_iCount = 0;
	m_MmiRequestSys.m_eReportSortType = ERSPIEnd;
	m_MmiRequestSys.m_bDescSort = TRUE;
	
	m_aUserBlockNames.RemoveAll();

	m_iMaxGridVisibleRow = 0;

	m_bInitialized = false;

	m_iPeriod = 1;
	m_pCurrentXSBHorz = NULL;
	m_pImgBtn = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewTimeSaleStatistic::~CIoViewTimeSaleStatistic()
{
	// MainFrame 中快捷键
	CMainFrame* pMainFrame =(CMainFrame*)AfxGetMainWnd();
	if ( NULL != pMainFrame && pMainFrame->m_pKBParent == this )
	{
		pMainFrame->SetHotkeyTarget(NULL);
	}
	DEL(m_pImgBtn);
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
void CIoViewTimeSaleStatistic::OnPaint()
{
	CPaintDC dcPaint(this); // device context for painting

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

	RedrawCycleBtn(&dcPaint);
}

void CIoViewTimeSaleStatistic::OnMouseWheel(short zDelta)
{
	//m_GridCtrl.SendMessage(WM_MOUSEWHEEL, MAKELONG(0, zDelta*120), 0);
}

BOOL CIoViewTimeSaleStatistic::PreTranslateMessage(MSG* pMsg)
{
	//if ( WM_MOUSEWHEEL == pMsg->message )
	//{
	//	// 拦截滚轮
	//	short zDelta = HIWORD(pMsg->wParam);
	//	zDelta /= 120;

	//	// 自己处理
	//	//OnMouseWheel(zDelta);
	//	//return TRUE;
	//}

	return CControlBase::PreTranslateMessage(pMsg);		// 越过IoViewBase的键盘处理
}

int CIoViewTimeSaleStatistic::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	ASSERT( NULL != m_pAbsCenterManager );
	if ( NULL == m_pAbsCenterManager )	// 自己注册
	{
		CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
		SetCenterManager(pApp->m_pDocument->m_pAbsCenterManager);
	}
	
	CIoViewBase::OnCreate(lpCreateStruct);
	
	InitialIoViewFace(this);

	//创建Tab 条
// 	m_GuiTabWnd.Create(WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,0x9999);
// 	m_GuiTabWnd.SetBkGround(false,GetIoViewColor(ESCBackground),0,0);
// 	// m_GuiTabWnd.SetBkGround(true,GetIoViewColor(ESCBackground),IDB_GUITAB_NORMAL,IDB_GUITAB_SELECTED);
// 	// m_GuiTabWnd.SetImageList(IDB_TAB, 16, 16, 0x0000ff);
// 	m_GuiTabWnd.SetUserCB(this);
// 	m_GuiTabWnd.SetALingTabs(CGuiTabWnd::ALN_BOTTOM);

	// 创建横滚动条
	m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10203);
	m_XSBVert.SetScrollRange(0, 10);

	m_XSBHorz.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10204);
	m_XSBHorz.SetScrollRange(0, 10);

	// 创建数据表格
	m_GridCtrl.Create(CRect(0, 0, 0, 0), this, ID_GRID_CONTROL);
	m_GridCtrl.SetDefaultCellType(RUNTIME_CLASS(CGridCellNormalSys));
	m_GridCtrl.SetDefCellWidth(50);
	m_GridCtrl.SetVirtualMode(TRUE);
	m_GridCtrl.SetDoubleBuffering(TRUE);
	//m_GridCtrl.SetVirtualCompare(*this);
	m_GridCtrl.EnableBlink(FALSE);			// 禁止闪烁
	m_GridCtrl.SetTextColor(COLO_NO_NAME_CODE);
	m_GridCtrl.SetFixedTextColor(COLO_NO_NAME_CODE);
	m_GridCtrl.SetTextBkColor(CLR_DEFAULT);
	m_GridCtrl.SetBkColor(CLR_DEFAULT);
	m_GridCtrl.SetFixedBkColor(CLR_DEFAULT);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFormat(DT_LEFT | DT_WORDBREAK| DT_NOPREFIX |DT_VCENTER);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFormat(DT_LEFT | DT_WORDBREAK| DT_NOPREFIX |DT_VCENTER);

	// 设置相互之间的关联
	m_XSBHorz.SetOwner(&m_GridCtrl);
	m_XSBVert.SetOwner(&m_GridCtrl);
	m_GridCtrl.SetScrollBar(&m_XSBHorz, &m_XSBVert);
	//XSBHorz.AddMsgListener(m_hWnd);	// 侦听滚动消息 - 记录滚动消息
	m_XSBVert.AddMsgListener(m_hWnd);	// 侦听滚动消息 - 更改可视商品

	SetXSBHorz(&m_XSBHorz);

	// 设置表头
	m_GridCtrl.SetHeaderSort(FALSE);
	m_GridCtrl.SetUserCB(this);

	// 显示表格线的风格
	m_GridCtrl.ShowGridLine(FALSE);
	m_GridCtrl.SetSingleRowSelection(TRUE);
	m_GridCtrl.SetListMode(TRUE);
	m_GridCtrl.SetDrawFixedCellGridLineAsNormal(TRUE);
	m_GridCtrl.SetInitSortAscending(FALSE);

	////////////////////////////////////////////////////////////////
	m_GridCtrl.SetFixedColumnCount(2);	// 序 名
	m_GridCtrl.SetFixedRowCount(1);

	// 设置字体
	LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);

	// 表头固定，可以初始化表头了
	UpdateTableHeader();

	//pCell->SetDrawParentActiveParams(m_iActiveXpos, m_iActiveYpos, m_ColorActive);

	PostMessage(UM_DOINITIALIZE, 0, 0);

	// 这个时候可以初始化tab资源了的，板块在此时应当加载好了，如果没加载应当在do initialize中初始化板块和tab
	//InitializeTabs();	

	//
	SetTimer(KUpdatePushMerchsTimerId, KTimerPeriodPushMerchs, NULL);

	CreateBtnList();

	if(m_mapBtnCycleList.size() > 0)
	{
		int iBtnID = MID_BUTTON_PAIHANG_ONE;
		switch(m_iPeriod)
		{
		case 1:
			iBtnID = MID_BUTTON_PAIHANG_ONE;
			break;

		case 3:
			iBtnID = MID_BUTTON_PAIHANG_THREE;
			break;

		case 5:
			iBtnID = MID_BUTTON_PAIHANG_FIVE;
			break;

		case 10:
			iBtnID = MID_BUTTON_PAIHANG_TEN;
			break;

		default:
			break;
		}

		map<int, CNCButton>::iterator iter;

		for (iter=m_mapBtnCycleList.begin(); iter!=m_mapBtnCycleList.end(); ++iter)
		{
			CNCButton &btnControl = iter->second;

			if (btnControl.GetControlId() == iBtnID)
			{
				btnControl.SetCheckStatus(TRUE);
			}
			else
			{
				btnControl.SetCheckStatus(FALSE);
			}
		}
	}


	return 0;
}

void CIoViewTimeSaleStatistic::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);

	RecalcLayout();
}

BOOL CIoViewTimeSaleStatistic::TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( IsWindowVisible())
	{
		if ( 13 == nChar && 1 == nRepCnt)
		{
			if( 28 == nFlags || 284 == nFlags)				
			{
				// 回车键
				CCellRange rangeSel = m_GridCtrl.GetSelectedCellRange();
				if ( rangeSel.IsValid() 
					&& rangeSel.GetMinRow() >= m_GridCtrl.GetFixedRowCount()
					&& rangeSel.GetMinRow() < m_GridCtrl.GetRowCount() )
				{
					CMerch *pMerch = (CMerch *)m_GridCtrl.GetItemData(rangeSel.GetMinRow(), 0);
					OnDblClick(pMerch);
					return TRUE;
				}
			}			
		}

		return FALSE;
	}

	return FALSE;
}

void CIoViewTimeSaleStatistic::OnIoViewActive()
{
	//
	//OnVDataForceUpdate();
	RequestLastIgnoredReqData();

	SetChildFrameTitle();
	
	if (NULL != m_GridCtrl.GetSafeHwnd())
	{
		m_GridCtrl.SetFocus();
	}

	m_bActive = IsActiveInFrame();

	Invalidate(TRUE);
}

void CIoViewTimeSaleStatistic::OnIoViewDeactive()
{
	m_bActive = false;
	
	Invalidate(TRUE);
}

void CIoViewTimeSaleStatistic::OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult)
{ 
	CPoint pos;
	GetCursorPos(&pos);
	PostMessage(UM_DOTRACEMENU, MAKEWPARAM(pos.x, pos.y), 0);
}

void CIoViewTimeSaleStatistic::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW	*pGridView = (NM_GRIDVIEW *)pNotifyStruct;
	if ( NULL == pGridView || pGridView->iRow < m_GridCtrl.GetFixedRowCount() )
	{
		return;
	}
	CMerch *pMerch = (CMerch *)m_GridCtrl.GetItemData(pGridView->iRow, 0);
	OnDblClick(pMerch);
}

void CIoViewTimeSaleStatistic::OnGridColWidthChanged(NMHDR *pNotifyStruct, LRESULT* pResult)
{	
	
}

void CIoViewTimeSaleStatistic::OnGridKeyDownEnd(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	// 向下键按到头了
}

void CIoViewTimeSaleStatistic::OnGridKeyUpEnd(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	// 向上键按到头了
}

// 通知视图改变关注的商品
void CIoViewTimeSaleStatistic::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch)
		return;
	
	// 修改当前查看的商品
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
}

//
void CIoViewTimeSaleStatistic::OnVDataForceUpdate()
{
	if ( m_tabInfo.IsSort() )
	{
		RequestSortData(true);		// 强制申请排行数据
	}
	RequestViewDataCurrentVisibleRow();
}

void CIoViewTimeSaleStatistic::OnFixedRowClickCB(CCellID& cell)
{
    if (!m_GridCtrl.IsValid(cell))
	{
		return;
	}
	// 序号不排序，仅排序数据，根据排序结果插入从索引0开始的值
	// 调用表格类的排序	
	if ( cell.col ==0 || cell.col == 1 )	// 代码 名称
	{
		if ( m_tabInfo.IsSort() )	// 如果以前有排序， 必须先清空表格 (虚模式是否可以不用？)
		{
			//m_GridCtrl.DeleteNonFixedRows(); // 删除数据，将滚动位置清0
			m_GridCtrl.SetRowCount(m_tabInfo.m_aMerchs.GetSize() + m_GridCtrl.GetFixedRowCount());
			m_tabInfo.ClearSort();		// 新的数据已经替换
			m_MmiRequestSys.m_eReportSortType = ERSPIEnd;
			m_GridCtrl.EnsureTopLeftCell(m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetFixedColumnCount());
		}

		m_pThis = this;
		m_GridCtrl.SetVirtualCompare(CompareRow);
		m_GridCtrl.SetHeaderSort(TRUE);
		
		CPoint pt(0,0);
		m_GridCtrl.OnFixedRowClick(cell,pt);
		
		
		m_GridCtrl.Refresh();

		RequestViewDataCurrentVisibleRow();
	}
	else
	{
		// 应该在此发出请求，等请求回来在做处理
		m_GridCtrl.SetHeaderSort(FALSE);	// 其它列禁用表格排序

		//m_GridCtrl.SetSortAscending(m_tabInfo.ChangeSortType(cell.col));
		//m_GridCtrl.SetSortColumn(cell.col);
		
		E_ReportSortPlugIn	eHeaderSort = GetColumnSortHeader(cell.col);
		if ( ERSPIEnd == eHeaderSort )
		{
			E_TimeSaleStatisticHeader eHeaderShow = GetColumnShowHeader(cell.col);
			if ( eHeaderShow == ETSSHPriceNew )		// 最新价排序不? 请求最新价沪深a板块排序
			{
				// 
				CBlockLikeMarket *pBlock = CBlockConfig::Instance()->GetDefaultMarketClassBlock();
				if ( NULL == pBlock )
				{
					return; // 没有沪深a就算了
				}
				CBlockLikeMarket *pBlockNow = GetCurrentBlock();
				if ( NULL == pBlockNow )
				{
					return;
				}
				BOOL bChangedCol = FALSE;
				m_MmiRequestBlockSys.m_iBlockId = pBlock->m_blockInfo.m_iBlockId;
				if ( pBlockNow == pBlock )
				{
					// 沪深A，没改变的化一般就是这个
				}
				else if ( pBlockNow->m_blockInfo.m_mapSubMerchMarkets.GetCount() == 1 )
				{
					// 沪深A下面的某个市场,现在的情况是必然是整个市场，此处以后变更需要更改，通过市场来调用
				}
				m_MmiRequestBlockSys.m_iStart = 0;					
				// 事实上，我们请求的是本地所有沪深a的商品排行，但是服务器商品集合有可能大于本地商品集合，所以数量处理是个麻烦
				//m_MmiRequestBlockSys.m_iCount = 5000;	
				m_MmiRequestBlockSys.m_iCount = pBlock->m_blockInfo.m_aSubMerchs.GetSize();	// 所有商品，其实还是有可能少了 - 最大商品数量
				m_MmiRequestBlockSys.m_eMerchReportField = EMRFPriceNew;
				m_MmiRequestBlockSys.m_bDescSort = !m_tabInfo.ChangeSortType(cell.col, &bChangedCol);
				if ( bChangedCol )
				{
					m_GridCtrl.ResetVirtualOrder(); // 重置虚模式下的行列序 - 虽然已经变更，但是在等待返回的时候，还是会出现用户滚动导致脱节的现象
					m_GridCtrl.EnsureTopLeftCell(m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetFixedColumnCount());
					m_GridCtrl.Refresh();
				}
				RequestSortData();
			}
			return;		// 不能排序 不进行任何处理
		}

		// 设置排序
		BOOL bChangedCol = FALSE;
		m_MmiRequestSys.m_iMarketId = m_tabInfo.m_iBlockId;
		m_MmiRequestSys.m_eReportSortType = eHeaderSort;
		m_MmiRequestSys.m_iStart = m_tabInfo.m_iStartRow;
		m_MmiRequestSys.m_iCount = m_iMaxGridVisibleRow;
		m_MmiRequestSys.m_bDescSort = !m_tabInfo.ChangeSortType(cell.col, &bChangedCol);

		if ( bChangedCol )	// 改变排序列应该重置可视数据
		{
			m_GridCtrl.ResetVirtualOrder(); // 重置虚模式下的行列序 - 虽然已经变更，但是在等待返回的时候，还是会出现用户滚动导致脱节的现象
			m_GridCtrl.EnsureTopLeftCell(m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetFixedColumnCount());
			m_GridCtrl.Refresh();
		}

		RequestSortData();
	}
}

void CIoViewTimeSaleStatistic::OnFixedColumnClickCB(CCellID& cell)
{

}

void CIoViewTimeSaleStatistic::OnHScrollEnd()
{
	
}

void CIoViewTimeSaleStatistic::OnVScrollEnd()
{
	
}

void CIoViewTimeSaleStatistic::OnCtrlMove( int32 x, int32 y )
{
	
}

bool32 CIoViewTimeSaleStatistic::OnEditEndCB ( int32 iRow,int32 iCol, const CString &StrOld, INOUT CString &StrNew )
{
	return false;
}

void CIoViewTimeSaleStatistic::OnDestroy()
{	
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if ( NULL != pMainFrm && pMainFrm->m_pKBParent == this )
	{
		pMainFrm->SetHotkeyTarget(NULL);
	}

	CIoViewBase::OnDestroy();
}

void CIoViewTimeSaleStatistic::OnTimer(UINT nIDEvent) 
{
	if(!m_bShowNow)
	{
		return;
	}

	if ( nIDEvent == KUpdatePushMerchsTimerId )
	{	
		if ( m_tabInfo.IsSort() )
		{
			RequestSortData();
		}
		else
		{
			RequestViewDataCurrentVisibleRow();	
		}
	}
	else if ( KSortAsyncTimerId == nIDEvent )
	{
		UpdateVisibleMerchs();
		KillTimer(nIDEvent);
	}
	
	CIoViewBase::OnTimer(nIDEvent);
}

void CIoViewTimeSaleStatistic::RequestViewDataCurrentVisibleRow()
{
	CMmiReqPeriodTickEx			reqHis;
	CMmiReqPushPlugInMerchData	req;		// 直接请求推送数据

	{
		for ( MerchMap::iterator it = m_mapLastPushMerchs.begin() ; it != m_mapLastPushMerchs.end() ; it++ )
		{
			it->second--;		// 重置0 or -- ？
		}
	}
	
	
	// 选择多个商品
	CCellRange cellRange = m_GridCtrl.GetVisibleNonFixedCellRange();
	if ( cellRange.IsValid() )
	{
		int32 iMaxRow = cellRange.GetMaxRow();
		//iMaxRow += max(15, cellRange.GetRowSpan());		// 多请求一点，尽量将空数据减少
		iMaxRow += 5;
		if ( iMaxRow > m_tabInfo.m_aMerchs.GetSize() )
		{
			iMaxRow = m_tabInfo.m_aMerchs.GetSize();
		}
		
		m_aSmartAttendMerchs.RemoveAll();		// 添加实时报价
		CMmiReqRealtimePrice reqRealPrice;
		
		for ( int i=cellRange.GetMinRow(); i <= iMaxRow ; i++ )
		{
			CMerch *pMerch = (CMerch *)m_GridCtrl.GetItemData(i, 0);
			ASSERT( NULL ==pMerch || pMerch == CheckMerchInBlock(pMerch) );	// 有可能表格没有对应商品，因为排序的商品还没回来
			if ( pMerch == NULL )
			{
				continue;
			}
			
			MerchMap::iterator itMerch = m_mapLastPushMerchs.find(pMerch);
			if ( itMerch != m_mapLastPushMerchs.end() )
			{
				itMerch->second++;
			}
			else
			{
				m_mapLastPushMerchs[pMerch] = 1;
			}
			
			bool32 bReqHis = m_mapMerchData.count(pMerch) <= 0;	// 只有在没有该商品的历史数据下才请求历史
			
			if ( req.m_StrMerchCode.IsEmpty() )
			{
				req.m_iMarketID = pMerch->m_MerchInfo.m_iMarketId;	// 沪深A - X
				req.m_StrMerchCode = pMerch->m_MerchInfo.m_StrMerchCode;
				req.m_uType = ECSTTickEx;
				
				reqHis.m_iMarketId = req.m_iMarketID;				// 第一个就加上去吧
				reqHis.m_StrMerchCode = req.m_StrMerchCode;
				reqHis.m_uiPeriod = m_iPeriod;
			}
			else
			{
				T_ReqPushMerchData	rpmd;
				rpmd.m_iMarket = pMerch->m_MerchInfo.m_iMarketId;
				rpmd.m_StrCode = pMerch->m_MerchInfo.m_StrMerchCode;
				rpmd.m_uType = ECSTTickEx;
				req.m_aReqMore.Add( rpmd );
	
				//if ( bReqHis )			// 不需要发送的历史数据请求
				{
					CPeriodMerchKey key;
					key.m_iMarketId = rpmd.m_iMarket;
					key.m_StrMerchCode = rpmd.m_StrCode;
					key.m_uiPeriod = m_iPeriod;
					reqHis.m_aMerchMore.Add(key);
				}
			}
			
			// 实时报价关注
			CSmartAttendMerch smartMerch;
			smartMerch.m_iDataServiceTypes = EDSTPrice;
			smartMerch.m_pMerch = pMerch;
			m_aSmartAttendMerchs.Add(smartMerch);
			
			if ( reqRealPrice.m_StrMerchCode.IsEmpty() )
			{
				reqRealPrice.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
				reqRealPrice.m_StrMerchCode		= pMerch->m_MerchInfo.m_StrMerchCode;						
			}
			else
			{
				CMerchKey MerchKey;
				MerchKey.m_iMarketId	= pMerch->m_MerchInfo.m_iMarketId;
				MerchKey.m_StrMerchCode = pMerch->m_MerchInfo.m_StrMerchCode;
				
				reqRealPrice.m_aMerchMore.Add(MerchKey);					
			}
		}
		
		if ( !req.m_StrMerchCode.IsEmpty() )
		{
			//	CString StrTrace;
			//	StrTrace.Format(_T("Tickex his: %d"), 1 + reqHis.m_aMerchMore.GetSize());
			//	XLTraceFile::GetXLTraceFile(_T("tickex.log")).TraceWithTimestamp(StrTrace, TRUE);
			if(m_iPeriod == 1)
			{
				req.m_eCommTypePlugIn = ECTPIReqAddPushTickEx;
			}
			else if(m_iPeriod == 3)
			{
				req.m_eCommTypePlugIn = ECTPIReqAddPushMerchIndexEx3;
			}
			else if(m_iPeriod == 5)
			{
				req.m_eCommTypePlugIn = ECTPIReqAddPushMerchIndexEx5;
			}
			else if(m_iPeriod == 10)
			{
				req.m_eCommTypePlugIn = ECTPIReqAddPushMerchIndexEx10;
			}
			RequestData(reqHis);
			RequestData(req);
			RequestData(reqRealPrice);
		}
	}

	// 删除不用的推送???
	if ( 0 )
	{
		CMmiReqRemovePushPlugInMerchData reqDel(ECTPIReqDeletePushMerchData);
		reqDel.m_eType = ECSTTickEx;
		MerchMap mapMerch = m_mapLastPushMerchs;
		for ( MerchMap::iterator it = mapMerch.begin() ; it != mapMerch.end() ; it++ )
		{
			if ( it->second < 1 )	// 应当删除
			{
				CMerchKey key;
				key.m_iMarketId		= it->first->m_MerchInfo.m_iMarketId;
				key.m_StrMerchCode	= it->first->m_MerchInfo.m_StrMerchCode;
				reqDel.m_aMerchs.Add(key);
				m_mapLastPushMerchs.erase(it->first);
			}
		}
		if ( reqDel.m_aMerchs.GetSize() > 0 )
		{
			//TRACE(_T("Delete Tickex: %d\r\n"), reqDel.m_aMerchs.GetSize());
			RequestData(reqDel);		// 删除推送会删除所有关注这个商品此类型的推送，可能其它视图需要~~
		}
	}
}

void CIoViewTimeSaleStatistic::RequestSortData(bool32 bForce/* = false*/)
{
	if ( m_tabInfo.IsSort() )
	{
		E_ReportSortPlugIn	eHeaderSort = GetColumnSortHeader(m_tabInfo.m_iSortColumn);
		if ( ERSPIEnd == eHeaderSort )
		{
			E_TimeSaleStatisticHeader eHeaderShow = GetColumnShowHeader(m_tabInfo.m_iSortColumn);
			if ( eHeaderShow == ETSSHPriceNew )		// 最新价排序不? 请求最新价沪深a板块排序
			{
				RequestData(m_MmiRequestBlockSys, bForce);
			}
		}
		else
		{
			m_MmiRequestSys.m_iMarketId = -1;	// 强制 沪深a
			// 		int32 iBlockDefId = 0;
			// 		if ( CBlockConfig::Instance()->GetDefaultMarketClassBlockId(iBlockDefId) && m_tabInfo.m_iBlockId == iBlockDefId )
			// 		{
			// 	  		m_MmiRequestSys.m_iStart = m_tabInfo.m_iStartRow;
			//   		m_MmiRequestSys.m_iCount = m_iMaxGridVisibleRow;
			// 		}
			// 		else
			{
				m_MmiRequestSys.m_iStart = 0;
				m_MmiRequestSys.m_iCount = -1;
			}
			m_MmiRequestSys.m_uiPeriod = m_iPeriod;
		}
		
		RequestData(m_MmiRequestSys, bForce);
	}
}

void CIoViewTimeSaleStatistic::SetChildFrameTitle()
{
	
}

void CIoViewTimeSaleStatistic::OnDblClick(CMerch *pMerch)
{
	if ( pMerch != NULL )
	{
		CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
		pMainFrame->OnShowMerchInChart(pMerch, this);
	}
}

void CIoViewTimeSaleStatistic::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();
	m_GridCtrl.Refresh();
}

void CIoViewTimeSaleStatistic::OnIoViewFontChanged()
{
	CIoViewBase::OnIoViewFontChanged();

	LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);
	
	SetColWidthAccordingFont();
	RecalcLayout();
}

void CIoViewTimeSaleStatistic::OnRButtonDown2( CPoint pt, int32 iTab )
{
	
}

bool32 CIoViewTimeSaleStatistic::OnLButtonDown2( CPoint pt, int32 iTab )
{
	return false;
}

bool32 CIoViewTimeSaleStatistic::BlindLButtonBeforeRButtonDown( int32 iTab )
{
	return false;
}

void CIoViewTimeSaleStatistic::SetRowHeightAccordingFont()
{
	m_GridCtrl.AutoSizeRows();
}

void CIoViewTimeSaleStatistic::SetColWidthAccordingFont()
{
	// 很奇怪的写法，为什么要固定死，不把自动放出来。是有什么业务逻辑？
	// 将默认最大值从70修改为95像素，规避显示不全
	const int32 iConstColWidth = 95;
	CClientDC dc(this);
	// 使用固定cell的font
	CFont *pFontNormal =  GetIoViewFontObject(ESFNormal);//m_GridCtrl.GetDefaultCell(TRUE, TRUE)->GetFontObject();// 
	CFont *pOldFont = dc.SelectObject(pFontNormal);
	CSize sizeText = dc.GetTextExtent(_T("测试高宽"));
	dc.SelectObject(pOldFont);
	int32 iColWidth = (int32)(sizeText.cx * 1.15f);
	iColWidth = max(iColWidth, iConstColWidth);
	//m_GridCtrl.AutoSizeColumns();
	//m_GridCtrl.ExpandColumnsToFit();
	for ( int i=0; i < m_GridCtrl.GetColumnCount() ; i++ )	// 调整最小col
	{
		if ( m_GridCtrl.GetColumnWidth(i) > 0 /*&& m_GridCtrl.GetColumnWidth(i) < iColWidth*/ )
		{
			if ( i == 11 || i == 12 || i == 13 )
			{
				m_GridCtrl.SetColumnWidth(i, iColWidth + 20);
			}
			else
			{
				m_GridCtrl.SetColumnWidth(i, iColWidth);
			}
		}
	}

	//CRect rcGrid;
	//m_GridCtrl.GetClientRect(rcGrid);
	//int iLeftWidth = rcGrid.Width() - m_GridCtrl.GetVirtualWidth();
	//int iCol = 0;
	//if ( iLeftWidth > 0 && (iCol=m_GridCtrl.GetColumnCount()-1) >= 0 )
	//{
	//	m_GridCtrl.SetColumnWidth( iCol, m_GridCtrl.GetColumnWidth(iCol) + iLeftWidth );
	//}
}

void CIoViewTimeSaleStatistic::UpdateTableHeader()
{
	HeaderArray	aHeaders;
	GetColumnHeaders(aHeaders);
		
	m_GridCtrl.SetFixedRowCount(1);
	m_GridCtrl.DeleteNonFixedRows();
	m_GridCtrl.SetColumnCount(aHeaders.GetSize());	// 序号列, 名称

	SetColWidthAccordingFont();
	SetRowHeightAccordingFont();	// 由于固定行高的问题，必须只能在仅有表头时调用，否则n慢
	m_GridCtrl.Refresh();
}

void CIoViewTimeSaleStatistic::UpdateTableRowContent( CMerch *pMerch, bool32 bBlink )
{
	// TODO
	ASSERT( 0 );	// 没有实现 - 如果数据更新很频繁，可以实现

	SetColWidthAccordingFont();
	m_GridCtrl.Refresh();
}

void CIoViewTimeSaleStatistic::UpdateTableAllContent()
{
	//m_GridCtrl.DeleteNonFixedRows();	

	// 此时已经没有了排序信息
	ASSERT( m_tabInfo.m_eSort == T_Tab::SortNone );

	m_GridCtrl.SetRowCount(m_GridCtrl.GetFixedRowCount() + m_tabInfo.m_aMerchs.GetSize());

// 	m_GridCtrl.InsertRowBatchBegin();
// 	int32 i=0; 
// 	CString StrNo;
// 	for ( i=0; i < m_aMerchs.GetSize() ; i++ )
// 	{
// 		if ( m_aMerchs[i] == NULL )
// 		{
// 			continue;;
// 		}
// 		CMerch *pMerch = m_aMerchs[i];
// 		int32 iRow = m_GridCtrl.InsertRowBatchForOne(pMerch->m_MerchInfo.m_StrMerchCode);
// 		if ( iRow >= 0 )
// 		{
// 			m_GridCtrl.SetItemData(iRow, 0, (LPARAM)pMerch);
// 			m_GridCtrl.SetItemText(iRow, 1, pMerch->m_MerchInfo.m_StrMerchCnName);
// 
// 			UpdateTableMerchSimple(pMerch, false);
// 		}
// 	}
// 	m_GridCtrl.InsertRowBatchEnd();

	//SetRowHeightAccordingFont();
	SetColWidthAccordingFont();
	m_GridCtrl.Refresh();
}

void CIoViewTimeSaleStatistic::OnDoInitialize()
{
	m_bInitialized = true;		// 仅仅是一个标志，现在还不做实质初始化

	int32 iBlockId = m_tabInfo.m_iBlockId;

	// 
	CBlockLikeMarket	*pBlock = CBlockConfig::Instance()->GetDefaultMarketClassBlock();
	if ( NULL != pBlock )
	{
		iBlockId = pBlock->m_blockInfo.m_iBlockId;
	}

	OpenBlock(iBlockId);		// 重装载一次板块数据
}

void CIoViewTimeSaleStatistic::RecalcLayout()
{
	CRect rcClient;
	GetClientRect(rcClient);

	// 计算表格最大行 - 粗略计算  - 需要插入足够的数据
	int32 iOldRowCount = m_iMaxGridVisibleRow;
	CClientDC dc(this);
	CFont *pFontOld = dc.SelectObject(GetIoViewFontObject(ESFNormal));
	CSize sizeText = dc.GetTextExtent(_T("行高度测试"));
	dc.SelectObject(pFontOld);
	m_iMaxGridVisibleRow = rcClient.Height() / sizeText.cy + 1;	// 多算了不知道多少个
	if ( m_iMaxGridVisibleRow < 0 )
	{
		m_iMaxGridVisibleRow = 0;
	}
	m_tabInfo.m_iRowCount = m_iMaxGridVisibleRow;


	CRect rcOld(0,0,0,0);
	m_GridCtrl.GetClientRect(rcOld);
	rcClient.top += GRIDCTRL_MOVEDOWN_SIZE;
	m_GridCtrl.MoveWindow(rcClient);

	//if ( rcOld.Width() < rcClient.Width() )
	//{
	//	SetColWidthAccordingFont();
	//	m_GridCtrl.ExpandColumnsToFit(TRUE);
	//}

	if ( iOldRowCount < m_iMaxGridVisibleRow )
	{
		AppendShowMerchs(m_iMaxGridVisibleRow - iOldRowCount);
	}

	ShowOrHideXHorzBar();
}

void CIoViewTimeSaleStatistic::OpenBlock( int32 iBlockId )
{
	int32 iBlockOldId = m_tabInfo.m_iBlockId;
	CBlockLikeMarket *pBlock = GetBlock(iBlockId);
	if ( pBlock == NULL )
	{
		return;	// do nothing
	}

	m_tabInfo.m_iBlockId = pBlock->m_blockInfo.m_iBlockId;
	m_tabInfo.m_StrName = pBlock->m_blockInfo.m_StrBlockName;
	// 填充当前商品数据
	m_tabInfo.m_aMerchs.Copy(pBlock->m_blockInfo.m_aSubMerchs);
	m_tabInfo.ResetShowData();
	m_tabInfo.m_iStartRow = 0;
	m_tabInfo.m_iRowCount = m_iMaxGridVisibleRow;

	m_GridCtrl.SetSortColumn(-1);
	m_GridCtrl.DeleteNonFixedRows();	

	UpdateTableAllContent();

	// 默认排序第一个可排序字段
	E_ReportSortEx eSort = ERSPIEnd;
	for ( int32 i=0; i < m_GridCtrl.GetColumnCount() ; i++ )
	{
		eSort = GetColumnSortHeader(i);
		if ( eSort != ERSPIEnd )
		{
			// 有排序，清除现有显示数据
			//for ( int j=0; j < m_tabInfo.m_aMerchsVisible.GetSize() ; j++ )
			//{
			//	m_tabInfo.m_aMerchsVisible[j] = NULL;  // 要是长时间不回来排序太难看了
			//}

			CCellID idCell;
			idCell.row = 0;
			idCell.col = i;
			OnFixedRowClickCB(idCell);

			if ( m_GridCtrl.GetRowCount() > m_GridCtrl.GetFixedRowCount()
				&& iBlockId != iBlockOldId )
			{
				// 切换板块切换到第一个位置
				m_GridCtrl.EnsureTopLeftCell(m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetFixedColumnCount());
				m_GridCtrl.SetSelectedRange(m_GridCtrl.GetFixedRowCount(), 0, m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetColumnCount()-1);
				m_GridCtrl.SetFocusCell(m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetFixedColumnCount());
			}
			return; // 有排序就不发数据显示
			//break;
		}
	}

	if ( m_GridCtrl.GetRowCount() > m_GridCtrl.GetFixedRowCount()
		&& iBlockId != iBlockOldId )
	{
		// 切换板块切换到第一个位置
		m_GridCtrl.EnsureTopLeftCell(m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetFixedColumnCount());
		m_GridCtrl.SetSelectedRange(m_GridCtrl.GetFixedRowCount(), 0, m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetColumnCount()-1);
		m_GridCtrl.SetFocusCell(m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetFixedColumnCount());
	}
	
	RequestViewDataCurrentVisibleRow();		// 申请数据 - 是否存在隐藏不发的情况？
}

void CIoViewTimeSaleStatistic::AppendShowMerchs(int32 iMerchAppCount)		// 只能在有效数据后添加，无有效数据情况下不会添加任何
{
	if ( iMerchAppCount <= 0 || GetCurrentBlock() == NULL )
	{
		return;
	}

	// 发送数据请求就可以了
// 	if ( m_tabInfo.IsSort() )
// 	{
// 		RequestSortData();		// 不够需要发排序 - 排序全部数据已经申请
// 	}
// 	else
	{
		RequestViewDataCurrentVisibleRow();	// 不排序只要显示数据就可以了
	}

// 
// 	// 获取合适的需要显示的商品，有可能需要发送排序申请的请求，然后在显示
// 	// 不需要排序则选取商品队列中的商品，显示
// 	
// 	if ( m_tabInfo.m_eSort != T_Tab::SortNone )	// 有排序
// 	{
// 		// 偏移一定量，请求预计可见行的数据 不改变现在的偏移 数据回来后再处理, 在等待过程中可能发生任何改变当前状态的事情，需要处理
// 	}
// 	else	// 按照市场中商品的序列显示
// 	{
// 		CMerch *pMerchLast = NULL;
// 		CCellRange cellRange = m_GridCtrl.GetCellRange();
// 		CMerch *pMerch = (CMerch *)m_GridCtrl.GetItemData(m_GridCtrl.GetRowCount()-1, 0);
// 		
// 		CMarket *pMarket = GetCurrentMarket();
// 		int32 i=0;
// 		for ( i=0; i < pMarket->m_MerchsPtr.GetSize() ; i++ )
// 		{
// 			if ( pMarket->m_MerchsPtr[i] == pMerch )
// 			{
// 				break;
// 			}
// 		}
// 		int32 iMerchApped = 0;
// 		i++;	// 
// // 		for (  ; i < pMarket->m_MerchsPtr.GetSize() && iMerchApped < iMerchAppCount ; i++ )
// // 		{
// // 			CMerch *pMerchMarket = pMarket->m_MerchsPtr[i];
// // 			if ( NULL == pMerchMarket )
// // 			{
// // 				continue;
// // 			}
// // 			int32 iRow = m_GridCtrl.InsertRow(pMerchMarket->m_MerchInfo.m_StrMerchCode);
// // 			if ( iRow >= 0 )
// // 			{
// // 				m_GridCtrl.SetItemData(iRow, 0, (LPARAM)pMerchMarket);
// // 				m_GridCtrl.SetItemText(iRow, 1, pMerchMarket->m_MerchInfo.m_StrMerchCnName);
// // 				
// // 				UpdateTableMerchSimple(pMerchMarket, false);
// // 			}
// // 			iMerchApped++;
// // 		}
// 		iMerchApped = m_aMerchs.GetSize() - i;
// 		if ( iMerchApped > 0 )
// 		{
// 			//m_GridCtrl.SetRowCount(m_GridCtrl.GetRowCount() + iMerchApped);
// 		}
// 		if ( iMerchApped > 0 )
// 		{
// 			//m_GridCtrl.EnsureVisible(m_GridCtrl.GetRowCount()-1, 0);	// 头部对齐
// 		}
// 	}
	
}

void CIoViewTimeSaleStatistic::OnGridGetDispInfo( NMHDR *pNotifyStruct, LRESULT *pResult )
{
	HeaderArray	aHeaders;
	GetColumnHeaders(aHeaders);		// 并不是严格需要

	if ( NULL != pNotifyStruct )
	{
		GV_DISPINFO	*pDisp = (GV_DISPINFO *)pNotifyStruct;
		// 尝试v mode
		// 排序可能处理为圈定得到的商品，保存商品数组，有多少就只让grid显示多少，怎么处理滚轮？排序拦截滚轮，先申请数据，数据回来在做处理
		ASSERT( pDisp->item.col >= 0 && pDisp->item.col < aHeaders.GetSize() );
		
		int iRow = pDisp->item.row;
		int iCol = pDisp->item.col;

		pDisp->item.lParam = NULL;		// 初始空

		CGridCellNormalSys *pCell = DYNAMIC_DOWNCAST(CGridCellNormalSys, m_GridCtrl.GetDefaultVirtualCell());
		if ( NULL != pCell )
		{
			pCell->SetShowSymbol(CGridCellSymbol::ESSNone);
			pCell->SetParentActiveFlag(false);
			pCell->SetDrawParentActiveParams(-1, -1, m_ColorActive);
			if ( pDisp->item.row == 0 && 0 == pDisp->item.col )
			{
				pCell->SetParentActiveFlag(m_bActive);
				pCell->SetDrawParentActiveParams(m_iActiveXpos, m_iActiveYpos, m_ColorActive);
			}
		}
		else
		{
			ASSERT( 0 );
		}

		// 头部
		if ( pDisp->item.row == 0 )
		{
			// 颜色
			// 某些字段只显示一行
			pDisp->item.nFormat |= DT_VCENTER;
			pDisp->item.crFgClr = GetIoViewColor(ESCVolume);
			pDisp->item.lParam = aHeaders[pDisp->item.col].m_eHeaderShow;		// 保存头部信息
			pDisp->item.strText = aHeaders[pDisp->item.col].m_StrName;
			if ( pDisp->item.col >= 2 )
			{
				pDisp->item.nFormat |= DT_RIGHT;	// 数据字段右对齐
			}
		}
		else
		{
			const MerchArray &aMerchShow = m_tabInfo.m_aMerchsVisible;
			//ASSERT( pDisp->item.row > 0 && pDisp->item.row <= aMerchShow.GetSize() );
			if ( pDisp->item.row < 0 || pDisp->item.row > aMerchShow.GetSize() )
			{
				return;		// 在板块切换时，会有getitem。。。
			}

			int iIndex = iRow - 1;

			// 数据部分
			// 主要处理DATA与TEXT，其它的这个视图都可以用默认的， 排序的未知商品导致很难处理，所以
			CMerch *pMerchShow = aMerchShow[iIndex];
			pDisp->item.lParam = (LPARAM)pMerchShow;	// 可能为NULL
			E_TimeSaleStatisticHeader eHeaderShow = aHeaders[pDisp->item.col].m_eHeaderShow;
			
			// 不使用默认数据
			DataMap::const_iterator it = m_mapMerchData.find(pMerchShow);
			if ( NULL != pMerchShow )
			{
				T_TimeSaleStatisticData dataShow;
				CRealtimePrice			realPrice;
				if ( it != m_mapMerchData.end() )
				{
					dataShow = it->second;
				} // 没有数据怎么办 TODO
				if ( NULL != pMerchShow->m_pRealtimePrice )
				{
					realPrice = *pMerchShow->m_pRealtimePrice;
				}

				pDisp->item.strText = _T(" -");
				//pDisp->item.crFgClr = GetIoViewColor(ESCText);
				//TRACE(_T("%d-%d text clr: %08X\r\n"), pDisp->item.row, pDisp->item.col, pDisp->item.crFgClr);
				

				if ( eHeaderShow >= ETSSHVolBuySuper && eHeaderShow <= ETSSHVolSellMidSmall )
				{
					pDisp->item.crFgClr = GetIoViewColor(ESCVolume2);
					//pDisp->item.crFgClr = RGB(192,192,0);
				}
				else if ( eHeaderShow == ETSSHAmountPerTrans || (eHeaderShow >= ETSSHAmountBuySuper && eHeaderShow <= ETSSHAmountTotalSell) )
				{
					pDisp->item.crFgClr = GetIoViewColor(ESCAmount);
				}

				const float cf1W = 10000.0;

				switch (eHeaderShow)
				{
				case ETSSHCode:
					pDisp->item.strText = pMerchShow->m_MerchInfo.m_StrMerchCode;
					break;
				case ETSSHName:
					{
						T_Block *pUserBlock = CUserBlockManager::Instance()->GetBlock(pMerchShow);
						if ( NULL != pUserBlock && pUserBlock->m_clrBlock != COLORNOTCOUSTOM )
						{
							pDisp->item.crFgClr = pUserBlock->m_clrBlock;
						}
						pDisp->item.strText = pMerchShow->m_MerchInfo.m_StrMerchCnName;

						// 是否有标记
						GV_DRAWMARK_ITEM markItem;
						if ( InitDrawMarkItem(pMerchShow, markItem) )
						{
							pDisp->item.markItem = markItem;
						}
					}
					break;
				case ETSSHPriceNew: // 最新价取RealTimePrice中的
					{
						if ( realPrice.m_fPriceNew != 0.0 )
						{
							if ( realPrice.m_fPriceNew > realPrice.m_fPricePrevClose )
							{
								pDisp->item.crFgClr = GetIoViewColor(ESCRise);
							}
							else if ( realPrice.m_fPriceNew < realPrice.m_fPricePrevClose )
							{
								pDisp->item.crFgClr = GetIoViewColor(ESCFall);
							}
						}
						pDisp->item.strText = Float2String(realPrice.m_fPriceNew, 2);
					}
					break;
				case ETSSHNetAmountSuper:
					{
						float fNet = (dataShow.m_tickEx.m_fExBigBuyAmount-dataShow.m_tickEx.m_fExBigSellAmount) / cf1W;
						if ( fNet > 0.0f )
						{
							pDisp->item.crFgClr = GetIoViewColor(ESCRise);
						}
						else if ( fNet < 0.0f )
						{
							pDisp->item.crFgClr = GetIoViewColor(ESCFall);
						}
						pDisp->item.strText = Float2String(fNet,  2);
						pCell->SetShowSymbol(CGridCellNormalSys::ESSFall);
					}
					break;
				case ETSSHVolNetSuper:
					{
						float fNet = (dataShow.m_tickEx.m_fExBigBuyVol-dataShow.m_tickEx.m_fExBigSellVol) / cf1W;
						if ( fNet > 0.0f )
						{
							pDisp->item.crFgClr = GetIoViewColor(ESCRise);
						}
						else if ( fNet < 0.0f )
						{
							pDisp->item.crFgClr = GetIoViewColor(ESCFall);
						}
						pDisp->item.strText = Float2String(fNet,  2);
						pCell->SetShowSymbol(CGridCellNormalSys::ESSFall);
					}
					break;
				case ETSSHVolBuySuper:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fExBigBuyVol / cf1W,  2);
					break;
				case ETSSHVolSellSuper:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fExBigSellVol / cf1W,  2);
					break;
				case ETSSHNetAmountBig:
					{
						float fNet = (dataShow.m_tickEx.m_fBigBuyAmount-dataShow.m_tickEx.m_fBigSellAmount) / cf1W;
						if ( fNet > 0.0f )
						{
							pDisp->item.crFgClr = GetIoViewColor(ESCRise);
						}
						else if ( fNet < 0.0f )
						{
							pDisp->item.crFgClr = GetIoViewColor(ESCFall);
						}
						pDisp->item.strText = Float2String(fNet,  2);
						pCell->SetShowSymbol(CGridCellNormalSys::ESSFall);
					}
					break;

				case ETSSHVolBuyBig:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fBigBuyVol / cf1W,  2);
					break;
				case ETSSHVolSellBig:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fBigSellVol / cf1W,  2);
					break;
				case ETSSHVolBuyMid:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fMidBuyVol / cf1W,  2);
					break;
				case ETSSHVolSellMid:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fMidSellVol / cf1W,  2);
					break;
				case ETSSHVolBuySmall:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fSmallBuyVol / cf1W,  2);
					break;
				case ETSSHVolSellSmall:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fSmallSellVol / cf1W,  2);
					break;
				case ETSSHNetAmountMid:
					{
						float fNet = (dataShow.m_tickEx.m_fMidBuyAmount-dataShow.m_tickEx.m_fMidSellAmount) / cf1W;
						if ( fNet > 0.0f )
						{
							pDisp->item.crFgClr = GetIoViewColor(ESCRise);
						}
						else if ( fNet < 0.0f )
						{
							pDisp->item.crFgClr = GetIoViewColor(ESCFall);
						}
						pDisp->item.strText = Float2String(fNet,  2);
						pCell->SetShowSymbol(CGridCellNormalSys::ESSFall);
					}
					break;
				case ETSSHNetAmountSmall:
					{
						float fNet = (dataShow.m_tickEx.m_fSmallBuyAmount-dataShow.m_tickEx.m_fSmallSellAmount) / cf1W;
						if ( fNet > 0.0f )
						{
							pDisp->item.crFgClr = GetIoViewColor(ESCRise);
						}
						else if ( fNet < 0.0f )
						{
							pDisp->item.crFgClr = GetIoViewColor(ESCFall);
						}
						pDisp->item.strText = Float2String(fNet,  2);
						pCell->SetShowSymbol(CGridCellNormalSys::ESSFall);
					}
					break;
				case ETSSHVolBuyMidSmall:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fMnSBuyVol / cf1W,  2);
					break;
				case ETSSHVolSellMidSmall:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fMnSSellVol / cf1W,  2);
					break;
				case ETSSHAmountBuySuper:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fExBigBuyAmount / cf1W,  2);
					break;
				case ETSSHAmountSellSuper:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fExBigSellAmount / cf1W,  2);
					break;
				case ETSSHAmountBuyBig:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fBigBuyAmount / cf1W,  2);
					break;
				case ETSSHAmountSellBig:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fBigSellAmount / cf1W,  2);
					break;
				case ETSSHAmountBuyMid:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fMidBuyAmount / cf1W,  2);
					break;
				case ETSSHAmountSellMid:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fMidSellAmount / cf1W,  2);
					break;
				case ETSSHAmountBuySmall:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fSmallBuyAmount / cf1W,  2);
					break;
				case ETSSHAmountSellSmall:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fSmallSellAmount / cf1W,  2);
					break;
				case ETSSHAmountTotalBuy:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fAllBuyAmount / cf1W,  2);
					// 颜色为涨色
					pDisp->item.crFgClr = GetIoViewColor(ESCRise);
					break;
				case ETSSHAmountTotalSell:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fAllSellAmount / cf1W,  2);
					// 颜色为跌色
					pDisp->item.crFgClr = GetIoViewColor(ESCFall);
					break;
				case ETSSHAmountPerTrans:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fAmountPerTrans / cf1W,  2);
					break;
				case ETSSHStocksPerTrans:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fStocksPerTrans,  2);
					break;
				
				case ETSSHTradeRateSuper:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fExBigChange * 100.0, 2);
					break;
				case ETSSHTradeRateBig:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fBigChange * 100.0, 2);
					break;
				case ETSSHTradeRateBuyBig:
					pDisp->item.strText = Float2String(dataShow.m_tickEx.m_fBigBuyChange * 100.0, 2);
					break;
				case ETSSHTradeCount:
					pDisp->item.strText.Format(_T(" %d"), dataShow.m_tickEx.m_uiTradeTimes);
					break;
				case ETSSHCapitalFlow:
					{
						float fFlow = dataShow.m_tickEx.m_fAllBuyAmount - dataShow.m_tickEx.m_fAllSellAmount;
						pDisp->item.strText = Float2SymbolString(fFlow, 0.0, 2, false, true);
					}
					break;

				default:
					// n多字段不能处理
					;
				}

				// symbol矫正-
				if ( pDisp->item.strText == _T("-") )
				{
					pDisp->item.strText = _T(" -");
				}

				if ( pDisp->item.col >= 2 )
				{
					pDisp->item.nFormat |= DT_RIGHT;	// 数据字段右对齐
				}
			}
		}
	}

	if ( NULL != *pResult )
	{
		*pResult = 1;
	}
}

bool CIoViewTimeSaleStatistic::CompareRow( int iRow1, int iRow2 )
{
	ASSERT( m_pThis != NULL );
	const MerchArray &aMerchs = m_pThis->m_tabInfo.m_aMerchsVisible;
	ASSERT( aMerchs.GetSize() >= iRow1 && aMerchs.GetSize() >= iRow2  );
	const int iIndex1 = iRow1-1;
	const int iIndex2 = iRow2-1;

	CMerch *pMerch1 = aMerchs[iIndex1];
	CMerch *pMerch2 = aMerchs[iIndex2];

	// 应该只有代码和名称才能进入这个排序，其它不能进入, 名称和代码排序时，可视商品是所有商品，不应出现有NULL的情况

	ASSERT( NULL != pMerch1 && NULL != pMerch2 );
	if ( NULL == pMerch1 || NULL == pMerch2 )
	{
		return false;
	}

	const int iCol = m_pThis->m_GridCtrl.m_CurCol;
	if ( iCol == 0 )	// 代码
	{
		//return pMerch1->m_MerchInfo.m_StrMerchCode > pMerch2->m_MerchInfo.m_StrMerchCode;
		return CompareString(LOCALE_SYSTEM_DEFAULT, 0, pMerch1->m_MerchInfo.m_StrMerchCode, -1
			, pMerch2->m_MerchInfo.m_StrMerchCode, -1) == CSTR_LESS_THAN;
	}
	else if ( iCol == 1 )
	{
		return CompareString(LOCALE_SYSTEM_DEFAULT, 0, pMerch1->m_MerchInfo.m_StrMerchCnName, -1
			, pMerch2->m_MerchInfo.m_StrMerchCnName, -1) == CSTR_LESS_THAN;
	}
	ASSERT( 0 );
	return true;
}

BOOL CIoViewTimeSaleStatistic::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;
}

void CIoViewTimeSaleStatistic::OnVDataRealtimePriceUpdate( IN CMerch *pMerch )
{
	CCellRange rangeVisible = m_GridCtrl.GetVisibleNonFixedCellRange();
	if ( !rangeVisible.IsValid() )
	{
		return;
	}
	for ( int i=rangeVisible.GetMinRow(); i < rangeVisible.GetMaxRow() ; i++ )
	{
		CMerch *pMerchRow = (CMerch *)m_GridCtrl.GetItemData(i, 0);
		if ( pMerchRow == pMerch )
		{
			//m_GridCtrl.RedrawRow(i);
			// 事实上只要更新实时报价表格就可以了 - 目前实时报价表格在2列
			for ( int j=0; j < m_GridCtrl.GetColumnCount() ; j++ )
			{
				if ( GetColumnShowHeader(j) == ETSSHPriceNew )
				{
					m_GridCtrl.RedrawCell(i, j);
					break;
				}
			}
			return;
		}
	}
}

void CIoViewTimeSaleStatistic::OnVDataPluginResp( const CMmiCommBase *pResp )
{
	if ( NULL == pResp )
	{
		return;
	}

	if ( pResp->m_eCommType != ECTRespPlugIn )
	{
		return;
	}

	const CMmiCommBasePlugIn *pRespPlugin = (CMmiCommBasePlugIn *)pResp;
	if ( pRespPlugin->m_eCommTypePlugIn == ECTPIRespPeriodMerchSort )
	{
		OnMerchSortResp((const CMmiRespPeriodMerchSort *)pRespPlugin);
	}
	else if ( pRespPlugin->m_eCommTypePlugIn == ECTPIRespPeriodTickEx
	         || pRespPlugin->m_eCommTypePlugIn == ECTPIRespAddPushTickEx
			 || pRespPlugin->m_eCommTypePlugIn == ECTPIRespAddPushMerchIndexEx3 
			 || pRespPlugin->m_eCommTypePlugIn == ECTPIRespAddPushMerchIndexEx5
			 || pRespPlugin->m_eCommTypePlugIn == ECTPIRespAddPushMerchIndexEx10 )
	{
		OnTickExResp((const CMmiRespPeriodTickEx *)pResp);
	}
}

void CIoViewTimeSaleStatistic::OnTickExResp( const CMmiRespPeriodTickEx *pResp )
{
	CMerch  *pMerch = NULL;
	if ( NULL == pResp || NULL == m_pAbsCenterManager || pResp->m_aTickEx.GetSize() == 0 )
	{
		return;
	}
	
	int i = 0;
	// 更新所有商品的数据, 如果有可见的，刷新显示
	MerchArray	aMerchsUpdate;
	CArray<T_TickEx, const T_TickEx &>	aTickexesUpdate;
	for ( i=0; i < pResp->m_aTickEx.GetSize() ; i++ )
	{
		const T_TickEx	&tick = pResp->m_aTickEx[i];
		if ( !m_pAbsCenterManager->GetMerchManager().FindMerch(tick.m_StrMerchCode, tick.m_iMarketId, pMerch) )
		{
			continue;
		}
		
		if ( CheckMerchInBlock(pMerch) != NULL )
		{
			aMerchsUpdate.Add(pMerch);
			aTickexesUpdate.Add(tick);
		}
	}

	ASSERT( aMerchsUpdate.GetSize() == aTickexesUpdate.GetSize() );

	for ( i=0; i < aMerchsUpdate.GetSize() ; i++ )
	{
		T_TimeSaleStatisticData dataMy;
		dataMy.m_tickEx = aTickexesUpdate[i];
		m_mapMerchData[ aMerchsUpdate[i] ] = dataMy;		// 保存显示的数据

		//TRACE(_T("Merch Tick ex: %s\r\n"), aMerchsUpdate[i]->m_MerchInfo.m_StrMerchCnName);
	}

	for ( i=0; i < aMerchsUpdate.GetSize() ; i++ )
	{
		if ( IsMerchInMerchArray(aMerchsUpdate[i], m_tabInfo.m_aMerchsVisible)  )
		{
			m_GridCtrl.Refresh();	// 需要刷新显示
			break;
		}
	}
}

void CIoViewTimeSaleStatistic::OnMerchSortResp( const CMmiRespPeriodMerchSort *pResp )
{
	if ( NULL == pResp )
	{
		return;
	}

	if ( m_tabInfo.IsSort()
		&& (pResp->m_iMarketId == -1 /*|| pResp->m_iMarketId == 65535*/)		// 强制	-	由于是全部返回，只处理这
		&& pResp->m_eReportSortType == m_MmiRequestSys.m_eReportSortType
		&& pResp->m_bDescSort == m_MmiRequestSys.m_bDescSort
		&& pResp->m_iPeriod == m_MmiRequestSys.m_uiPeriod
		)
	{
		E_ReportSortPlugIn	eHeaderSort = GetColumnSortHeader(m_tabInfo.m_iSortColumn);
		if ( ERSPIEnd == eHeaderSort )
		{
			return;	// 不是需要的数据
		}
		// 筛选商品
		int i = 0;
		MerchArray aMerchMy, aMerchSrc, aMerchOrg;
		ConvertMerchKeysToMerchs(pResp->m_aMerchs, aMerchSrc, true);
		aMerchMy.SetSize(0, aMerchSrc.GetSize());
		aMerchOrg.Copy(m_tabInfo.m_aMerchs);
		for ( i=0; i < aMerchSrc.GetSize() ; i++ )
		{
			for ( int32 j=0; j < aMerchOrg.GetSize() ; j++ )
			{
				if ( aMerchSrc[i] == aMerchOrg[j] )
				{
					aMerchMy.Add(aMerchSrc[i]);
					aMerchOrg.RemoveAt(j);
					break;
				}
			}
		}
		//ASSERT( aMerchOrg.GetSize() == 0 );

		// 由于商品权限原因，可能服务器的商品数量和客户端有区别，所以实际客户端得到的可能要少于服务器的
		// 设置可视商品
		MerchArray aMerchVisible;
		aMerchVisible.Copy(aMerchMy);
		aMerchVisible.Append(aMerchOrg);	// 把剩余的商品加进去凑数

		ASSERT( aMerchVisible.GetSize() == m_tabInfo.m_aMerchs.GetSize() );
		m_tabInfo.m_aMerchsVisible.Copy(aMerchVisible);

		m_GridCtrl.SetSortColumn( m_tabInfo.m_iSortColumn );
		m_GridCtrl.SetSortAscending( !pResp->m_bDescSort );

		RequestViewDataCurrentVisibleRow();		// 请求可视数据

		m_GridCtrl.Refresh();
	}
}

void CIoViewTimeSaleStatistic::OnVDataPublicFileUpdate( IN CMerch *pMerch, E_PublicFileType ePublicFileType )
{
	
}

CMerch* CIoViewTimeSaleStatistic::CheckMerchInBlock( CMerch *pMerch )
{
	for ( int i=0; i < m_tabInfo.m_aMerchs.GetSize() ; i++ )
	{
		if ( m_tabInfo.m_aMerchs[i] == pMerch )
		{
			return pMerch;
		}
	}
	return NULL;
}

void CIoViewTimeSaleStatistic::GetColumnHeaders( OUT HeaderArray &aHeaders )
{
	
	// 	CStringArray aColNames;
	// // 	超大单买入量（万股）
	// // 		超大单卖出量（万股）
	// // 		大单买入量（万股）
	// // 		大单卖出量（万股）
	// // 		中单买入量（万股）
	// // 		中单卖出量（万股）
	// // 		小单买入量（万股）
	// // 		小单卖出量（万股）
	// // 		中小单买入量（万股）
	// // 		中小单卖出量（万股）
	// // 		超大单买入额（万元）
	// // 		超大单卖出额（万元）
	// // 		超大单净额
	// // 		大单买入额（万元）
	// // 		大单卖出额（万元）
	// // 		大单净额
	// // 		中单买入额（万元）
	// // 		中单卖出额（万元）
	// // 		中单净额
	// // 		小单买入额（万元）
	// // 		小单卖出额（万元）
	// // 		小单净额
	// // 		买入总额（万元）
	// // 		卖出总额（万元）
	// // 		超大单换手率%
	// // 		大单换手率%
	// // 		成交笔数
	// // 		每笔金额（万元）
	// // 		每笔股数（股）
	// // 		资金流向
	// 	aColNames.Add(_T("代码"));
	// 	aColNames.Add(_T("名称"));
	// 	aColNames.Add(_T("最新\n(元)"));
	// 	aColNames.Add(_T("超大单\n买入量\n(万股)"));
	// 	aColNames.Add(_T("超大单\n卖出量\n(万股)"));
	// 	aColNames.Add(_T("大单\n买入量\n(万股)"));
	// 	aColNames.Add(_T("大单\n卖出量\n(万股)"));
	// 	aColNames.Add(_T("中小单\n买入量\n(万股)"));
	// 	aColNames.Add(_T("中小单\n卖出量\n(万股)"));
	// 	aColNames.Add(_T("资金流向\n(万元)"));
	// 	aColNames.Add(_T("买入比例\n(%)"));
	// 	aColNames.Add(_T("成交金额\n(万元)"));
	// 	aColNames.Add(_T("超大单\n换手率\n(%)"));
	// 	aColNames.Add(_T("大单\n换手率\n(%)"));
	// 	aColNames.Add(_T("买入大单\n换手率\n(%)"));
	// 	aColNames.Add(_T("每笔股数\n(股)"));
// 	aColNames.Add(_T("每笔金额\n(万元)"));
	static CArray<E_TimeSaleStatisticHeader, E_TimeSaleStatisticHeader>	aEHeaders;
	if ( aEHeaders.GetSize() == 0 )
	{
		aEHeaders.Add(ETSSHCode);
		aEHeaders.Add(ETSSHName);
		aEHeaders.Add(ETSSHPriceNew);
		
		aEHeaders.Add(ETSSHNetAmountSuper);
		aEHeaders.Add(ETSSHVolNetSuper);
		aEHeaders.Add(ETSSHVolBuySuper);
		aEHeaders.Add(ETSSHVolSellSuper);
		
		aEHeaders.Add(ETSSHNetAmountBig);
		aEHeaders.Add(ETSSHVolBuyBig);
		aEHeaders.Add(ETSSHVolSellBig);

		aEHeaders.Add(ETSSHNetAmountMid);
		aEHeaders.Add(ETSSHNetAmountSmall);
		aEHeaders.Add(ETSSHVolBuyMidSmall);
		aEHeaders.Add(ETSSHVolSellMidSmall);
		
		aEHeaders.Add(ETSSHAmountTotalBuy);
		aEHeaders.Add(ETSSHAmountTotalSell);
		aEHeaders.Add(ETSSHAmountPerTrans);
		aEHeaders.Add(ETSSHStocksPerTrans);
		
		aEHeaders.Add(ETSSHTradeRateSuper);
		aEHeaders.Add(ETSSHTradeRateBig);
	}
	
	aHeaders.RemoveAll();
	for ( int i=0; i < aEHeaders.GetSize() ; i++ )
	{
		E_TimeSaleStatisticHeader eHeader = aEHeaders[i];
		for ( int j=0; j < KTimeSaleStatisticHeadersCount ; j++ )
		{
			if ( KTimeSaleStatisticHeaders[j].m_eHeaderShow == eHeader )
			{
				aHeaders.Add(KTimeSaleStatisticHeaders[j]);
				break;
			}
		}
	}
}

E_ReportSortPlugIn CIoViewTimeSaleStatistic::GetColumnSortHeader( int iCol )
{
	if ( !m_GridCtrl.IsValid(0, iCol) )
	{
		return ERSPIEnd;
	}

	E_TimeSaleStatisticHeader eHeaderShow = (E_TimeSaleStatisticHeader)m_GridCtrl.GetItemData(0, iCol);
	for ( int i=0; i < KTimeSaleStatisticHeadersCount ; i++ )
	{
		if ( KTimeSaleStatisticHeaders[i].m_eHeaderShow == eHeaderShow )
		{
			return KTimeSaleStatisticHeaders[i].m_eHeaderSort;
		}
	}
	return ERSPIEnd;
}

E_TimeSaleStatisticHeader CIoViewTimeSaleStatistic::GetColumnShowHeader( int iCol )
{
	if ( !m_GridCtrl.IsValid(0, iCol) )
	{
		return ETSSHCount;
	}
	
	E_TimeSaleStatisticHeader eHeaderShow = (E_TimeSaleStatisticHeader)m_GridCtrl.GetItemData(0, iCol);

	return eHeaderShow;
}

bool32 CIoViewTimeSaleStatistic::IsMerchInMerchArray( CMerch *pMerch, const MerchArray &aMerchs )
{
	for ( int i=0; i < aMerchs.GetSize() ; i++ )
	{
		if ( aMerchs[i] == pMerch )
		{
			return true;
		}
	}
	return false;
}

LRESULT CIoViewTimeSaleStatistic::OnScrollPosChanged( WPARAM w, LPARAM l )
{
	HWND hwnd = (HWND)w;
	CXScrollBar *pHorz = GetXSBHorz();
	if ( m_XSBVert.m_hWnd == hwnd )
	{
		KillTimer(KSortAsyncTimerId);
		SetTimer(KSortAsyncTimerId, KSortAsyncTimerPeriod, NULL);		// 重计算更新商品
	}
	else if ( pHorz != NULL && pHorz->GetSafeHwnd() == hwnd )
	{
		// 设置是否显示/隐藏水平滚动条
		ShowOrHideXHorzBar();
	}
	return 1;
}

void CIoViewTimeSaleStatistic::UpdateVisibleMerchs()
{
	KillTimer(KSortAsyncTimerId);

	CCellRange rangeVisible = m_GridCtrl.GetVisibleNonFixedCellRange();
	if ( rangeVisible.GetMinRow() != m_tabInfo.m_iStartRow ) // 滚动了
	{
		int32 iOldStart = m_tabInfo.m_iStartRow;
		m_tabInfo.m_iStartRow = rangeVisible.GetMinRow() - m_GridCtrl.GetFixedRowCount() - 9;	// 更改起点, 上面多预留几行商品缓存
		if ( m_tabInfo.m_iStartRow < 0  )
		{
			m_tabInfo.m_iStartRow = 0;
		}

// 		if ( m_tabInfo.IsSort()
// 			&& (rangeVisible.GetMaxRow() >= m_tabInfo.m_iStartRow + m_iMaxGridVisibleRow		// 超出最后一个申请显示数据的界限)
// 				|| rangeVisible.GetMinRow() < iOldStart	// 上滚
// 				) )
// 		{
// 			RequestSortData();
// 		}

		//if ( m_tabInfo.IsSort() )		// 由于起点不停的更新，so
		//{
		//	RequestSortData(); // 排行已经返回全部
		//}

		RequestViewDataCurrentVisibleRow();
	}
}

CBlockLikeMarket* CIoViewTimeSaleStatistic::GetCurrentBlock()
{
	return GetBlock(m_tabInfo.m_iBlockId);
}

CBlockLikeMarket* CIoViewTimeSaleStatistic::GetBlock( int32 iBlockId )
{
	// 现在我们没有沪深A股数据，怎么办呢？
	return CBlockConfig::Instance()->FindBlock(iBlockId);
}

void CIoViewTimeSaleStatistic::RequestData( CMmiCommBase &req, bool32 bForce /*= false*/ )
{
	DoRequestViewData(req);
}

void CIoViewTimeSaleStatistic::OnContextMenu( CWnd* pWnd, CPoint pos )
{
	CRect rcWin;
	GetWindowRect(rcWin);
	if ( !rcWin.PtInRect(pos) )
	{
		pos = rcWin.TopLeft();
	}

	// DoTrackMenu(pos);
}

void CIoViewTimeSaleStatistic::DoTrackMenu(CPoint pos)
{
	// 选择分类板块
	CBlockConfig::IdArray	aBlockIds;
	int32 i = 0;
	//CBlockLikeMarket *pBlockCur = CBlockConfig::Instance()->FindBlock(m_tabInfo.m_iBlockId);
	CBlockLikeMarket *pBlockSHSZA = CBlockConfig::Instance()->GetDefaultMarketClassBlock();
	if ( NULL == pBlockSHSZA )
	{
		return;
	}
	ASSERT( pBlockSHSZA->m_blockInfo.m_aSubBlockIds.GetSize() > 0 );
	aBlockIds.Add(pBlockSHSZA->m_blockInfo.m_iBlockId);
	aBlockIds.Append(pBlockSHSZA->m_blockInfo.m_aSubBlockIds);
	for ( i=0; i < aBlockIds.GetSize() ; i++ )
	{
		if ( aBlockIds[i] == m_tabInfo.m_iBlockId )
		{
			aBlockIds.RemoveAt(i);
			break;
		}
	}
	if ( aBlockIds.GetSize() <= 0 )
	{
		return;		// 没有则返回
	}
	
	CNewMenu menu;
	menu.CreatePopupMenu();
	int32 iCmd = 1;
	menu.AppendMenu(MF_STRING, iCmd++, _T("打开商品"));
	menu.SetDefaultItem(1, FALSE);

	BOOL bShowGrid = m_GridCtrl.GetGridLines() != GVL_NONE;
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, iCmd++, bShowGrid ? _T("隐藏网格线") : _T("显示网格线"));
	
	menu.AppendMenu(MF_SEPARATOR);
	for ( i=0; i < aBlockIds.GetSize() ; i++ )
	{
		menu.AppendMenu(MF_STRING, iCmd +i, CBlockConfig::Instance()->GetBlockName(aBlockIds[i]));
	}

	CWnd *pWndParent = GetParent();
	if ( NULL != pWndParent && pWndParent->IsKindOf(RUNTIME_CLASS(CIoViewBase)) )
	{
		AppendStdMenu(&menu);
	}
	
	int iRet = menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_TOPALIGN |TPM_NONOTIFY |TPM_RETURNCMD, pos.x, pos.y, AfxGetMainWnd());
	if ( iRet == 1 )
	{
		CMerch *pMerch = NULL;
		CCellRange cellRange = m_GridCtrl.GetSelectedCellRange();
		if ( cellRange.IsValid() && (pMerch = (CMerch *)m_GridCtrl.GetItemData(cellRange.GetMinRow(), 0)) != NULL )
		{
			OnDblClick(pMerch);
		}
	}
	else if ( iRet == 2 )
	{
		m_GridCtrl.ShowGridLine(!bShowGrid);
		m_GridCtrl.Refresh();
	}
	else if ( iRet >= iCmd )
	{
		iRet -= iCmd;
		if ( iRet >=0 && iRet < aBlockIds.GetSize() )
		{
			OpenBlock(aBlockIds[iRet]);
		}
		else
		{
			iRet += iCmd;	// 还原id
			::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, iRet, 0);
		}
	}
}

LRESULT CIoViewTimeSaleStatistic::OnDoTrackMenu( WPARAM w, LPARAM l )
{
	CPoint pos;
	pos.x = LOWORD(w);
	pos.y = HIWORD(w);
	DoTrackMenu(pos);
	return 1;
}

CMerch * CIoViewTimeSaleStatistic::GetMerchXml()
{
	CCellRange rangeSel = m_GridCtrl.GetSelectedCellRange();
	CMerch *pMerch = NULL;
	if ( rangeSel.IsValid() && (pMerch = (CMerch *)m_GridCtrl.GetItemData(rangeSel.GetMinRow(), 0)) != NULL )
	{
		return pMerch;
	}
	return CIoViewBase::GetMerchXml();
}

CXScrollBar* CIoViewTimeSaleStatistic::SetXSBHorz( CXScrollBar *pHorz /*= NULL*/ )
{
	CXScrollBar *pOld = m_pCurrentXSBHorz;
	if ( NULL == pHorz )
	{
		pHorz = &m_XSBHorz;
	}
	
	if ( IsWindow(pHorz->GetSafeHwnd()) )
	{
		if ( m_pCurrentXSBHorz != pHorz )
		{
			m_pCurrentXSBHorz = pHorz;
			m_pCurrentXSBHorz->SetOwner(&m_GridCtrl);
			m_GridCtrl.SetScrollBar(m_pCurrentXSBHorz, &m_XSBVert);	// 竖直条不提供给外面设置, 水平条不需要监听
			m_pCurrentXSBHorz->AddMsgListener(m_hWnd);	// 监听滚动，显示或者隐藏滚动条
			ShowOrHideXHorzBar();
		}
	}
	else
	{
		ASSERT( 0 );
	}
	return pOld;
}

CXScrollBar* CIoViewTimeSaleStatistic::GetXSBHorz() const
{
	return m_pCurrentXSBHorz;
}

void CIoViewTimeSaleStatistic::OnVDataReportInBlockUpdate( int32 iBlockId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount, const CArray<CMerch *, CMerch *> &aMerchs )
{
	bool32 bNeedSortData = FALSE;
	if ( m_tabInfo.IsSort() )
	{
		E_ReportSortPlugIn	eHeaderSort = GetColumnSortHeader(m_tabInfo.m_iSortColumn);
		if ( ERSPIEnd == eHeaderSort )
		{
			E_TimeSaleStatisticHeader eHeaderShow = GetColumnShowHeader(m_tabInfo.m_iSortColumn);
			if ( eHeaderShow == ETSSHPriceNew )		// 最新价排序不? 请求最新价沪深a板块排序
			{
				bNeedSortData = true;
			}
		}
	}
	if ( bNeedSortData
		&& iBlockId == m_MmiRequestBlockSys.m_iBlockId
		&& eMerchReportField == m_MmiRequestBlockSys.m_eMerchReportField
		&& bDescSort == m_MmiRequestBlockSys.m_bDescSort
		&& iPosStart == m_MmiRequestBlockSys.m_iStart
		&& iOrgMerchCount >= m_MmiRequestBlockSys.m_iCount	// 数量量大的也认为是要求的
		)
	{
		// 事实上，我们请求的是本地所有沪深a的商品排行，但是服务器商品集合有可能大于本地商品集合，所以数量处理是个麻烦
		// 筛选商品
		int i = 0;
		MerchArray aMerchMy, aMerchSrc, aMerchOrg;
		aMerchSrc.Copy(aMerchs);
		aMerchMy.SetSize(0, aMerchSrc.GetSize());
		aMerchOrg.Copy(m_tabInfo.m_aMerchs);
		for ( i=0; i < aMerchSrc.GetSize() ; i++ )
		{
			for ( int32 j=0; j < aMerchOrg.GetSize() ; j++ )
			{
				if ( aMerchSrc[i] == aMerchOrg[j] )
				{
					aMerchMy.Add(aMerchSrc[i]);
					aMerchOrg.RemoveAt(j);
					break;
				}
			}
		}
		//ASSERT( aMerchOrg.GetSize() == 0 );
		if ( aMerchOrg.GetSize() > 0 )
		{
			TRACE(_T("大单排行存在数据不能排行：%d. 可能原因为服务器商品与本地商品不一致!!\r\n"), aMerchOrg.GetSize());
		}
		
		// 由于商品权限原因，可能服务器的商品数量和客户端有区别，所以实际客户端得到的可能要少于服务器的
		// 设置可视商品
		MerchArray aMerchVisible;
		aMerchVisible.Copy(aMerchMy);
		aMerchVisible.Append(aMerchOrg);	// 把剩余的商品加进去凑数
		
		ASSERT( aMerchVisible.GetSize() == m_tabInfo.m_aMerchs.GetSize() );
		m_tabInfo.m_aMerchsVisible.Copy(aMerchVisible);
		
		m_GridCtrl.SetSortColumn( m_tabInfo.m_iSortColumn );
		m_GridCtrl.SetSortAscending( !bDescSort );
		
		RequestViewDataCurrentVisibleRow();		// 请求可视数据
		
		m_GridCtrl.Refresh();
	}
}

void CIoViewTimeSaleStatistic::LockRedraw()
{
	if ( !m_bLockRedraw )
	{
		if ( IsWindowVisible() )
		{
			SendMessage(WM_SETREDRAW, (WPARAM)FALSE, 0);
			CWnd *pChild = GetWindow(GW_CHILD);
			while ( NULL != pChild )
			{
				if ( pChild->IsWindowVisible() )
				{
					if ( pChild->IsKindOf(RUNTIME_CLASS(CIoViewBase)) )
					{
						(DYNAMIC_DOWNCAST(CIoViewBase, pChild))->LockRedraw();
					}
					pChild->SendMessage(WM_SETREDRAW, (WPARAM)FALSE, 0);
				}
				pChild = pChild->GetWindow(GW_HWNDNEXT);
			}
		}
		m_bLockRedraw = true;
	}
}

void CIoViewTimeSaleStatistic::UnLockRedraw()
{
	if ( m_bLockRedraw )
	{
		if ( IsWindowVisible() )
		{
			SendMessage(WM_SETREDRAW, (WPARAM)TRUE, 0);
			Invalidate();
			CWnd *pChild = GetWindow(GW_CHILD);
			while ( NULL != pChild )
			{
				if ( pChild->IsWindowVisible() )
				{
					pChild->SendMessage(WM_SETREDRAW, (WPARAM)TRUE, 0);		// 进行redraw设置会使hide变为show，so需要注意
					if ( pChild->IsKindOf(RUNTIME_CLASS(CIoViewBase)) )
					{
						(DYNAMIC_DOWNCAST(CIoViewBase, pChild))->UnLockRedraw();
					}
				}
				
				pChild = pChild->GetWindow(GW_HWNDNEXT);
			}
		}
		m_bLockRedraw = false;
	}
}

bool32 CIoViewTimeSaleStatistic::ShowOrHideXHorzBar()
{
	if ( GetXSBHorz() != NULL )
	{
		CCellRange rangeVisible = m_GridCtrl.GetVisibleNonFixedCellRange();
		if ( !IsWindowVisible()
			|| (rangeVisible.IsValid() 
			&& rangeVisible.GetColSpan() == m_GridCtrl.GetColumnCount() - m_GridCtrl.GetFixedColumnCount()) )
		{
			GetXSBHorz()->ShowWindow(SW_HIDE);
			return false;
		}
		else
		{
			GetXSBHorz()->ShowWindow(SW_SHOW);
			return true;
		}
	}
	return false;
}

void CIoViewTimeSaleStatistic::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	ShowOrHideXHorzBar();
}

void CIoViewTimeSaleStatistic::GetAttendMarketDataArray( OUT AttendMarketDataArray &aAttends )
{
	aAttends.RemoveAll();
	CBlockLikeMarket *pBlock = GetCurrentBlock();
	T_AttendMarketData data;
	if ( NULL != pBlock )
	{
		// 遍历所有的商品找出所有的市场- -，分类的除外
		CBlockInfo::E_BlockType eBlockType = pBlock->m_blockInfo.GetBlockType();
		switch ( eBlockType )
		{
		case CBlockInfo::typeMarketClassBlock:
		case CBlockInfo::typeNormalBlock:
		case CBlockInfo::typeUserBlock:
		case CBlockInfo::typeHSASubBlock:
			{
				// 全商品搜索
				typedef CMap<int32, int32, int32, int32> MarketMap;
				MarketMap ms;
				POSITION pos = pBlock->m_blockInfo.m_mapSubMerchMarkets.GetStartPosition();
				while ( NULL != pos )
				{
					int32 iMarket, iValue;
					pBlock->m_blockInfo.m_mapSubMerchMarkets.GetNextAssoc(pos, iMarket, iValue);
					
					data.m_iMarketId = iMarket;
					data.m_iEDSTypes = EDSTGeneral;
					
					aAttends.Add(data);
				}
			}
			break;
		default:
			ASSERT( 0 );
		}
	}
}

CMerch* CIoViewTimeSaleStatistic::GetNextMerch( CMerch* pMerchNow, bool32 bPre )
{
	CMerch *pMerchFind = NULL;
	CBlockLikeMarket *pBlock = GetCurrentBlock();
	if ( NULL == pBlock )
	{
		return CIoViewBase::GetNextMerch(pMerchNow, bPre);
	}
	// 取前一个商品
	for ( int32 i=m_tabInfo.m_aMerchsVisible.GetUpperBound() ; i >=0; --i )
	{
		if ( pMerchNow == m_tabInfo.m_aMerchsVisible[i] )
		{
			if ( bPre && i > 0 )
			{
				pMerchFind = m_tabInfo.m_aMerchsVisible[i-1];
			}
			else if ( !bPre && i < m_tabInfo.m_aMerchsVisible.GetUpperBound() )
			{
				pMerchFind = m_tabInfo.m_aMerchsVisible[i+1];
			}
			break;
		}
	}
	if ( NULL == pMerchFind && !m_tabInfo.IsSort() && m_GridCtrl.GetSortColumn() == -1 )
	{
		// 可视区域内无法获取该商品的下一个，则按板块中顺序取
		int32 iMerchCount = m_tabInfo.m_aMerchs.GetSize();
		for ( int32 i=0; i < iMerchCount ; ++i )
		{
			if ( pMerchNow == m_tabInfo.m_aMerchs[i] )
			{
				if ( bPre )
				{
					pMerchFind = m_tabInfo.m_aMerchs[ (i-1+iMerchCount)%iMerchCount ];
				}
				else
				{
					pMerchFind = m_tabInfo.m_aMerchs[ (i+1)%iMerchCount ];
				}
				break;
			}
		}
	}
	if ( NULL == pMerchFind )
	{
		return CIoViewBase::GetNextMerch(pMerchNow, bPre);
	}
	return pMerchFind;
}

void CIoViewTimeSaleStatistic::OnEscBackFrameMerch( CMerch *pMerch )
{
	// 取前一个商品
	for ( int32 i=0 ; i < m_tabInfo.m_aMerchsVisible.GetSize(); i++ )
	{
		if ( pMerch == m_tabInfo.m_aMerchsVisible[i] )
		{
			CCellRange rangeVisible = m_GridCtrl.GetVisibleNonFixedCellRange();
			int32 iRow = m_GridCtrl.GetFixedRowCount()+i;
			m_GridCtrl.EnsureVisible(iRow, rangeVisible.GetMinCol());
			CCellID cellFocus = m_GridCtrl.GetFocusCell();
			if ( cellFocus.IsValid() )
			{
				m_GridCtrl.SetFocusCell(iRow, cellFocus.col);
			}
			m_GridCtrl.SetSelectedRange(iRow, 0, iRow, m_GridCtrl.GetColumnCount()-1);
			break;
		}
	}
}

void CIoViewTimeSaleStatistic::RefreshView( int32 iPeriod )
{
	m_iPeriod = iPeriod;

	if ( m_tabInfo.IsSort() )
	{
		RequestSortData();
	}
	else
	{
		RequestViewDataCurrentVisibleRow();	
	}
}

BOOL CIoViewTimeSaleStatistic::OnCommand( WPARAM wParam, LPARAM lParam )
{
	int32 iID = (int32)wParam;
	std::map<int, CNCButton>::iterator itHome = m_mapBtnCycleList.find(iID);
	if (m_mapBtnCycleList.end() != itHome)
	{
		switch(iID)
		{
		case  MID_BUTTON_PAIHANG_ONE:
			m_iPeriod = 1;
			break;

		case  MID_BUTTON_PAIHANG_THREE:
			m_iPeriod = 3;
			break;

		case  MID_BUTTON_PAIHANG_FIVE:
			m_iPeriod = 5;
			break;

		case  MID_BUTTON_PAIHANG_TEN:
			m_iPeriod = 10;
			break;

		default:
			break;
		}
	}

	RefreshView(m_iPeriod);

	return CWnd::OnCommand(wParam, lParam);
}

void CIoViewTimeSaleStatistic::OnLButtonUp( UINT nFlags, CPoint point )
{
	int iButton = TNCButtonHitTest(point);

	if (INVALID_ID != iButton)
	{
		m_mapBtnCycleList[iButton].LButtonUp();
	}	
}

void CIoViewTimeSaleStatistic::OnLButtonDown( UINT nFlags, CPoint point )
{
	int iButton = TNCButtonHitTest(point);

	if (INVALID_ID != iButton)
	{
		m_mapBtnCycleList[iButton].LButtonDown();


		map<int, CNCButton>::iterator iter;

		for (iter=m_mapBtnCycleList.begin(); iter!=m_mapBtnCycleList.end(); ++iter)
		{
			CNCButton &btnControl = iter->second;

			if (btnControl.GetControlId() == iButton)
			{
				btnControl.SetCheckStatus(TRUE);
			}
			else
			{
				btnControl.SetCheckStatus(FALSE);
			}
		}
	}
}

void CIoViewTimeSaleStatistic::CreateBtnList()
{
	m_mapBtnCycleList.clear();

	CRect rcClient;
	GetClientRect(rcClient);
	rcClient.bottom = GRIDCTRL_MOVEDOWN_SIZE;

	CRect rcBtn(rcClient);

	int iDistance = 15;

	m_pImgBtn = Image::FromFile(_T("image//subbtn.png"));


	rcBtn.top	 =  (GRIDCTRL_MOVEDOWN_SIZE - m_pImgBtn->GetHeight() / 3) / 2;;
	int iBtnWidth = m_pImgBtn->GetWidth();
	// 因为是5个按钮所以乘以5
	rcBtn.left   =500;//rcClient.left +  (rcClient.Width() - (iBtnWidth + iDistance)* 5)/2;
	rcBtn.right  = rcBtn.left + iBtnWidth;
	rcBtn.bottom = rcBtn.top + m_pImgBtn->GetHeight() / 3;

	btnPrompt.CreateButton(L"可选周期:", rcBtn, this, NULL, 0, MID_BUTTON_PAIHANG_PROMPT);

	//提示按钮提示
	btnPrompt.SetTextBkgColor(RGB(37,40,45), RGB(37,40,45), RGB(37,40,45));
	btnPrompt.SetTextFrameColor(RGB(37,40,45), RGB(37,40,45), RGB(37,40,45));
	btnPrompt.SetTextColor(RGB(190,191,191), RGB(190,191,191), RGB(190,191,191));

	rcBtn.left = rcBtn.right + iDistance;
	rcBtn.right = rcBtn.left + iBtnWidth;
	AddNCButton(&rcBtn, m_pImgBtn, 3,  MID_BUTTON_PAIHANG_ONE, L"单日");

	rcBtn.left = rcBtn.right + iDistance;
	rcBtn.right = rcBtn.left + iBtnWidth;
	AddNCButton(&rcBtn, m_pImgBtn, 3,  MID_BUTTON_PAIHANG_THREE, L"3日");

	rcBtn.left = rcBtn.right + iDistance;
	rcBtn.right = rcBtn.left + iBtnWidth;
	AddNCButton(&rcBtn, m_pImgBtn, 3,  MID_BUTTON_PAIHANG_FIVE, L"5日");

	rcBtn.left = rcBtn.right + iDistance;
	rcBtn.right = rcBtn.left + iBtnWidth;
	AddNCButton(&rcBtn, m_pImgBtn, 3,  MID_BUTTON_PAIHANG_TEN, L"10日");
}

void CIoViewTimeSaleStatistic::RedrawCycleBtn( CPaintDC *pPainDC )
{
	//调整gridctrl位置
	CRect rcClient;
	GetClientRect(rcClient);
	rcClient.bottom  = rcClient.top+ GRIDCTRL_MOVEDOWN_SIZE + 1;

	CDC memDC;
	memDC.CreateCompatibleDC(pPainDC);
	CBitmap bmp;
	int nWidth = rcClient.Width();
	int nHeight = rcClient.Height();
	bmp.CreateCompatibleBitmap(pPainDC, rcClient.Width(), rcClient.Height());
	//memDC.FillSolidRect(rcClient.left ,rcClient.top, rcClient.Width(),rcClient.Height(),RGB(37,40,45));
	memDC.SelectObject(&bmp);
	memDC.SetBkMode(TRANSPARENT);

	Graphics graphics(memDC.GetSafeHdc());

	SolidBrush  brush(Color(37,40,45));
	graphics.FillRectangle(&brush, rcClient.left ,rcClient.top, rcClient.Width(),rcClient.Height());

	btnPrompt.DrawButton(&graphics);
	map<int, CNCButton>::iterator iter;
	for (iter=m_mapBtnCycleList.begin(); iter!=m_mapBtnCycleList.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		btnControl.DrawButton(&graphics);
	}

	pPainDC->BitBlt(rcClient.left ,rcClient.top, rcClient.Width(), rcClient.Height(), &memDC, 0, 0, SRCCOPY);
	pPainDC->SelectClipRgn(NULL);
	bmp.DeleteObject();
	memDC.DeleteDC();
}

void CIoViewTimeSaleStatistic::AddNCButton( LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption )
{
	CNCButton btnControl;
	btnControl.CreateButton(lpszCaption, lpRect, this, pImage, nCount, nID);
	btnControl.SetTextBkgColor(RGB(25,25,25), RGB(146,96,0), RGB(25,25,25));
	btnControl.SetTextFrameColor(RGB(100,100,100), RGB(255,255,255), RGB(255,255,255));
	btnControl.SetTextColor(RGB(190,191,191), RGB(255,255,255), RGB(255,255,255));
	m_mapBtnCycleList[nID] = btnControl;
}

int CIoViewTimeSaleStatistic::TNCButtonHitTest( CPoint point )
{
	map<int, CNCButton>::iterator iter;

	for (iter=m_mapBtnCycleList.begin(); iter!=m_mapBtnCycleList.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;

		if (btnControl.PtInButton(point))
		{
			return btnControl.GetControlId();
		}
	}

	return INVALID_ID;
}

// bool CIoViewTimeSaleStatistic::operator()( int iRow1, int iRow2 )
// {
// 	return CompareRow(iRow1, iRow2);
// }


//////////////////////////////////////////////////////////////////////////
//

CIoViewTimeSaleStatistic::T_TimeSaleStatisticData::T_TimeSaleStatisticData()
{
	// TickEx初始化 TODO	
}
