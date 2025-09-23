#include "stdafx.h"
#include "ShareFun.h"
#include "memdc.h"
#include "io.h"
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
#include "IoViewReportRank.h"
#include "LogFunctionTime.h"

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define   MIX_SCREEN_HEIGHT 900

const int32 KUpdatePushMerchsTimerId		= 100005;			// 每隔 n 秒钟, 请求排序 ?  - 无推送，需要频繁点
const int32 KTimerPeriodPushMerchs			= 1000 * 5;

const int32 KTimerIdReqSortAsync			= 100006;			// 延时请求排序数据
const int32 kTimerPeriodReqSortAsync		= 50;

// XML字段
const char* KStrViewRankMarketId			= "BLOCKID";		// 打开的市场id
const char* KStrViewRankField				= "RANKFIELD";		// 排行字段
const char* KStrViewRankDesc				= "RANKDESC";		// 排行序列

//////////////////////////////////////////////////////////////////////////
CIoViewReportRank::HeaderArray CIoViewReportRank::m_saHeaders;

IMPLEMENT_DYNCREATE(CIoViewReportRank, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewReportRank, CIoViewBase)
//{{AFX_MSG_MAP(CIoViewReportRank)
ON_WM_PAINT()
ON_WM_ERASEBKGND()
ON_WM_CREATE()
ON_WM_SIZE()
ON_WM_TIMER()
ON_WM_CONTEXTMENU()
ON_MESSAGE_VOID(UM_DOINITIALIZE, OnDoInitialize)
//}}AFX_MSG_MAP
ON_NOTIFY(NM_RCLICK,0x2020,OnGridRButtonDown)
ON_NOTIFY(NM_DBLCLK, 0x2020, OnGridDblClick)
ON_NOTIFY(GVN_SELCHANGED, 0x2020,OnGridSelRowChanged) 
ON_NOTIFY(GVN_COLWIDTHCHANGED, 0x2020, OnGridColWidthChanged)
ON_NOTIFY(GVN_KEYDOWNEND, 0x2020, OnGridKeyDownEnd)
ON_NOTIFY(GVN_KEYUPEND, 0x2020, OnGridKeyUpEnd)
ON_NOTIFY(GVN_GETDISPINFO, 0x2020, OnGridGetDispInfo)
END_MESSAGE_MAP()
///////////////////////////////////////////////////////////////////////////////

// 准备优化 fangz20100514

CIoViewReportRank::CIoViewReportRank()
:CIoViewBase()
{
	m_pParent		= NULL;
	m_rectClient	= CRect(-1,-1,-1,-1);

	m_RectTitle.SetRectEmpty();
	
	m_aUserBlockNames.RemoveAll();

	m_iMaxGridVisibleRow = 0;

	m_iBlockId = CBlockConfig::GetDefaultMarketlClassBlockPseudoId();		// 沪深A， 需要找寻

	m_bInitialized = false;

	m_bShowBlockNameOnTitle = true;

	m_iScreenHeight = GetSystemMetrics(SM_CYSCREEN); 

	// 初始化可以选择的排行
	if ( m_saHeaders.GetSize() <= 0 )
	{
		// 涨 跌 振 5+ 5- 量 委比+ 委比- 金额
		T_Header header;
		m_saHeaders.Add(header);

		header.m_bDescSort = false;
		header.m_StrName = _T("今日跌幅排名");
		m_saHeaders.Add(header);

		header.m_eField = EMRFAmplitude;
		header.m_bDescSort = true;
		header.m_StrName = _T("今日振幅排名");
		m_saHeaders.Add(header);

		header.m_eField = EMRFRiseRate;
		header.m_bDescSort = true;
		header.m_StrName = _T("5分钟涨速排名");
		m_saHeaders.Add(header);

		header.m_eField = EMRFRiseRate;
		header.m_bDescSort = false;
		header.m_StrName = _T("5分钟跌速排名");
		m_saHeaders.Add(header);

		header.m_eField = EMRFVolRatio;
		header.m_bDescSort = true;
		header.m_StrName = _T("今日量比排名");
		m_saHeaders.Add(header);

		header.m_eField = EMRFBidRatio;
		header.m_bDescSort = true;
		header.m_StrName = _T("今日委比前排名");
		m_saHeaders.Add(header);

		header.m_eField = EMRFBidRatio;
		header.m_bDescSort = false;
		header.m_StrName = _T("今日委比后排名");
		m_saHeaders.Add(header);

		header.m_eField = EMRFAmountTotal;
		header.m_bDescSort = true;
		header.m_StrName = _T("今日总金额排名");
		m_saHeaders.Add(header);
	}

	m_headerCur = m_saHeaders[0];
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewReportRank::~CIoViewReportRank()
{
	// MainFrame 中快捷键
	CMainFrame* pMainFrame =(CMainFrame*)AfxGetMainWnd();
	if ( NULL != pMainFrame && pMainFrame->m_pKBParent == this )
	{
		pMainFrame->SetHotkeyTarget(NULL);
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
void CIoViewReportRank::OnPaint()
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

	int iSaveDC = dc.SaveDC();

	// 画标题
	CString StrTitle;
	GetTitleString(StrTitle);
	
	E_SysFont emFont = ESFNormal;
	if (m_iScreenHeight < MIX_SCREEN_HEIGHT)
	{
		emFont = ESFSmall;
	}
	dc.SelectObject(GetIoViewFontObject(emFont));
	dc.SetBkColor(GetIoViewColor(ESCBackground));
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(GetIoViewColor(ESCVolume));


	CRect rcClient;
	GetClientRect(rcClient);
	dc.FillSolidRect(rcClient, GetIoViewColor(ESCBackground));
	dc.FillSolidRect(m_RectTitle, GetIoViewColor(ESCGridFixedBk));
	dc.DrawText(StrTitle, m_RectTitle, DT_SINGLELINE |DT_VCENTER |DT_CENTER);

	// 去掉小白点
	if ( m_bActive )
	{
		//dc.FillSolidRect(3, 3, 2, 2, GetIoViewColor(ESCVolume));
	}

	dc.RestoreDC(iSaveDC);
}

BOOL CIoViewReportRank::PreTranslateMessage(MSG* pMsg)
{
	return CIoViewBase::PreTranslateMessage(pMsg);		// base能根据IoViewType处理合适的键
}

int CIoViewReportRank::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( NULL == m_pAbsCenterManager )	// 自己注册
	{
		CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
		SetCenterManager(pApp->m_pDocument->m_pAbsCenterManager);
	}
	
	CIoViewBase::OnCreate(lpCreateStruct);
	
	InitialIoViewFace(this);

	// 创建横滚动条
// 	m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10203);
// 	m_XSBVert.SetScrollRange(0, 10);
// 
// 	m_XSBHorz.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10204);
// 	m_XSBHorz.SetScrollRange(0, 10);

	// 创建数据表格
	m_GridCtrl.Create(CRect(0, 0, 0, 0), this, 0x2020);
	m_GridCtrl.SetDefaultCellType(RUNTIME_CLASS(CGridCellNormalSys));
	m_GridCtrl.SetDefCellWidth(50);
	m_GridCtrl.SetVirtualMode(TRUE);
	m_GridCtrl.SetDoubleBuffering(TRUE);
	//m_GridCtrl.SetVirtualCompare(*this);
	m_GridCtrl.EnableBlink(FALSE);			// 禁止闪烁
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetBackClr(CLR_DEFAULT);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetTextClr(CLR_DEFAULT);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

	// 设置表格图标
	// m_ImageList.Create(MAKEINTRESOURCE(IDB_GRID_REPORT), 16, 1, RGB(255,255,255));

	// 设置相互之间的关联
// 	m_XSBHorz.SetOwner(&m_GridCtrl);
// 	m_XSBVert.SetOwner(&m_GridCtrl);
// 	m_GridCtrl.SetScrollBar(&m_XSBHorz, &m_XSBVert);
	//m_XSBHorz.AddMsgListener(m_hWnd);	// 侦听滚动消息 - 记录滚动消息

	// 设置表头
	m_GridCtrl.SetHeaderSort(FALSE);

	// 显示表格线的风格
	m_GridCtrl.ShowGridLine(FALSE);
	m_GridCtrl.SetSingleRowSelection(TRUE);
	m_GridCtrl.SetShowSelectWhenLoseFocus(FALSE);		// 失去焦点不显示
    m_GridCtrl.SetIgnoreHeader(false);  

	////////////////////////////////////////////////////////////////
	//m_GridCtrl.SetFixedColumnCount(0);	// 序 名
	//m_GridCtrl.SetFixedRowCount(1);

	// 设置字体
	LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
	if (m_iScreenHeight < MIX_SCREEN_HEIGHT)
	{
		pFontNormal = GetIoViewFont(ESFSmall);
	}

	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);

	PostMessage(UM_DOINITIALIZE, 0, 0);

	//
	SetTimer(KUpdatePushMerchsTimerId, KTimerPeriodPushMerchs, NULL);

	CBlockConfig::Instance()->AddListener(this);
	return 0;
}

void CIoViewReportRank::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);

	RecalcLayout();
}

BOOL CIoViewReportRank::TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return FALSE;
}

void CIoViewReportRank::OnIoViewActive()
{
	m_bActive = IsActiveInFrame();

	//
	//OnVDataForceUpdate();		// 此时到底要不要请求排行呢？？
	RequestLastIgnoredReqData();

	SetChildFrameTitle();
	
	if (NULL != m_GridCtrl.GetSafeHwnd())
	{
		m_GridCtrl.SetFocus();
	}

	Invalidate(TRUE);
}

void CIoViewReportRank::OnIoViewDeactive()
{
	if ( m_bActive )
	{
		Invalidate(TRUE);
	}
	m_bActive = false;
	//m_GridCtrl.SetSelectedRange(-1, -1, -1, -1);
	//m_GridCtrl.SetFocusCell(-1, -1);
}

void CIoViewReportRank::OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult)
{ 
	CPoint pos(0, 0);
	GetCursorPos(&pos);
	DoTrackMenu(pos);
}

void CIoViewReportRank::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	OnDblClick();
}

void CIoViewReportRank::OnGridColWidthChanged(NMHDR *pNotifyStruct, LRESULT* pResult)
{	
	
}

void CIoViewReportRank::OnGridKeyDownEnd(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	// 向下键按到头了
}

void CIoViewReportRank::OnGridKeyUpEnd(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	// 向上键按到头了
}

// 通知视图改变关注的商品
void CIoViewReportRank::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch)
		return;
	
	// 修改当前查看的商品
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
}

//
void CIoViewReportRank::OnVDataForceUpdate()
{
	RequestSortViewData(true);
}

void CIoViewReportRank::OnDestroy()
{	
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if ( NULL != pMainFrm && pMainFrm->m_pKBParent == this )
	{
		pMainFrm->SetHotkeyTarget(NULL);
	}

	CBlockConfig::Instance()->RemoveListener(this);

	CIoViewBase::OnDestroy();
}

void CIoViewReportRank::OnTimer(UINT nIDEvent) 
{
	if(!m_bShowNow)
	{
		return;
	}

	if ( nIDEvent == KUpdatePushMerchsTimerId )
	{
		CBlockLikeMarket	*pBlock = GetCurrentBlock();
		if ( NULL != pBlock && pBlock->m_blockInfo.m_aSubMerchs.GetSize() > 0 )
		{
			if ( IsNowInTradeTimes(pBlock->m_blockInfo.m_aSubMerchs[0], true) )	// 仅检查一部分商品
			{
				RequestSortViewData();		// 请求排行数据	
			}
		}
	}
	else if ( KTimerIdReqSortAsync == nIDEvent )
	{
		RequestSortViewData();
	}
	
	CIoViewBase::OnTimer(nIDEvent);
}

void CIoViewReportRank::RequestSortViewData(bool32 bForce/* = false*/)
{
	KillTimer(KTimerIdReqSortAsync);

	m_MmiRequestSys.m_eMerchReportField = m_headerCur.m_eField;
	m_MmiRequestSys.m_bDescSort = m_headerCur.m_bDescSort;
	m_MmiRequestSys.m_iStart = 0;
	m_MmiRequestSys.m_iCount = m_iMaxGridVisibleRow;
	m_MmiRequestSys.m_iBlockId = m_iBlockId;		// 小郑服务器 照常发送

	RequestData(m_MmiRequestSys, bForce);
}

void CIoViewReportRank::RequestRealTimePrice()
{
	m_aSmartAttendMerchs.RemoveAll();

	if ( m_aMerchs.GetSize() > 0 )
	{
		CArray<CSmartAttendMerch, CSmartAttendMerch&> aSmart;
		for ( int i=0; i < m_aMerchs.GetSize() ; i++ )
		{
			if ( NULL == m_aMerchs[i] )
			{
				continue;
			}
			CMerch *pMerchAdd = m_aMerchs[i];
			int j=0;
			for ( j=0; j < m_aSmartAttendMerchs.GetSize() ; j++ )
			{
				CMerch *pMerchExist = m_aSmartAttendMerchs[j].m_pMerch;
				if ( pMerchAdd == pMerchExist || 
					(pMerchAdd->m_MerchInfo.m_iMarketId == pMerchExist->m_MerchInfo.m_iMarketId &&
					pMerchAdd->m_MerchInfo.m_StrMerchCode == pMerchExist->m_MerchInfo.m_StrMerchCode) )
				{
					break;
				}
			}
			if ( j >= m_aSmartAttendMerchs.GetSize() )
			{
				CSmartAttendMerch smart;
				smart.m_pMerch = pMerchAdd;
				smart.m_iDataServiceTypes = EDSTPrice;
				aSmart.Add(smart);
			}
		}
		
		if ( aSmart.GetSize() > 0 )
		{
			m_aSmartAttendMerchs.Append(aSmart);
			
			CMmiReqRealtimePrice Req;
			for ( int32 i = 0; i < m_aSmartAttendMerchs.GetSize(); i++ )
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
				}
				else
				{
					CMerchKey MerchKey;
					MerchKey.m_iMarketId	= SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
					MerchKey.m_StrMerchCode = SmartAttendMerch.m_pMerch->m_MerchInfo.m_StrMerchCode;
					
					Req.m_aMerchMore.Add(MerchKey);					
				}
			}
			
			RequestData(Req);
		}
	}
}

void CIoViewReportRank::SetChildFrameTitle()
{
	CString StrTitle;
	StrTitle =  CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;

	CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pParentFrame)
	{
		pParentFrame->SetChildFrameTitle(StrTitle);
	}
}

void CIoViewReportRank::OnDblClick()
{
	CMerch *pMerch = GetSelectMerch();
	if ( pMerch != NULL )
	{
		CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
		pMainFrame->OnShowMerchInChart(pMerch, this);
	}
}

void CIoViewReportRank::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();
	Invalidate(TRUE);
}

void CIoViewReportRank::OnIoViewFontChanged()
{
	CIoViewBase::OnIoViewFontChanged();

	LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
	if (m_iScreenHeight < MIX_SCREEN_HEIGHT)
	{
		pFontNormal = GetIoViewFont(ESFSmall);
	}

	//pFontNormal->lfHeight = CFaceScheme::Instance()->GetFontHeight(false,pFontNormal->lfHeight);	
	
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);

	RecalcLayout();
	Invalidate(TRUE);
}

void CIoViewReportRank::SetRowHeightAccordingFont()
{
	m_GridCtrl.AutoSizeRows();
}

void CIoViewReportRank::SetColWidthAccordingFont()
{
//	const int32 iColWidth = 80;	 // 由于没有固定表头，实际调整的宽度是指定的
	//m_GridCtrl.AutoSizeColumns();
	m_GridCtrl.ExpandColumnsToFit();

	//for ( int i=1; i < m_GridCtrl.GetColumnCount() ; i++ )	// 调整最小col
	//{
	//	if ( m_GridCtrl.GetColumnWidth(i) > 0 && m_GridCtrl.GetColumnWidth(i) < iColWidth )
	//	{
	//		m_GridCtrl.SetColumnWidth(i, iColWidth);
	//	}
	//}
}

void CIoViewReportRank::UpdateTableContent( CMerch *pMerch )
{
	SetColWidthAccordingFont();
	m_GridCtrl.Refresh();
}

void CIoViewReportRank::UpdateTableAllContent()
{
	//m_GridCtrl.DeleteNonFixedRows();	
	m_GridCtrl.SetColumnCount(3);
	m_GridCtrl.SetRowCount(m_aMerchs.GetSize());

 	//CCellRange rangeVisible = m_GridCtrl.GetVisibleNonFixedCellRange(NULL, FALSE, FALSE);
 	//if ( rangeVisible.IsValid()
 	//	&& m_aMerchs.GetSize() > rangeVisible.GetRowSpan())
 	//{
 	//	m_GridCtrl.SetRowCount(m_GridCtrl.GetFixedRowCount() +rangeVisible.GetRowSpan());	// 截断不可见行
 	//}

	SetRowHeightAccordingFont();
	SetColWidthAccordingFont();
	m_GridCtrl.Refresh();

	CCellRange rangeSel = m_GridCtrl.GetSelectedCellRange();
	if ( !rangeSel.IsValid() && m_aMerchs.GetSize() > 0 )
	{
		m_GridCtrl.SetFocusCell(0, 0);
		m_GridCtrl.SetSelectedRange(0, 0, 0, 2);
	}
}

void CIoViewReportRank::OnDoInitialize()
{
	m_bInitialized = true;		// 仅仅是一个标志，现在还不做实质初始化

	OpenBlock(m_iBlockId);		// 重装载一次板块数据
}

void CIoViewReportRank::RecalcLayout()
{
	CRect rcClient;
	GetClientRect(rcClient);
	
	// 计算表格最大行 - 粗略计算  - 需要插入足够的数据
	int32 iOldRowCount = m_iMaxGridVisibleRow;
	CClientDC dc(this);
	E_SysFont emFont = ESFNormal;
	if (m_iScreenHeight < MIX_SCREEN_HEIGHT)
	{
		emFont = ESFSmall;
	}
	CFont *pFontOld = dc.SelectObject(GetIoViewFontObject(emFont));
	CSize sizeText = dc.GetTextExtent(_T("行高度测试"));
	dc.SelectObject(pFontOld);
	m_iMaxGridVisibleRow = rcClient.Height() / sizeText.cy + 1;	// 多算了不知道多少个
	if ( m_iMaxGridVisibleRow < 0 )
	{
		m_iMaxGridVisibleRow = 0;
	}
	if ( iOldRowCount < m_iMaxGridVisibleRow )
	{
		RequestSortViewDataAsync();
	}

	m_RectTitle = rcClient;
	m_RectTitle.bottom = m_RectTitle.top + sizeText.cy + 8;
	rcClient.top = m_RectTitle.bottom;

	CRect rcOld(0,0,0,0);
	m_GridCtrl.GetClientRect(rcOld);

	rcClient.DeflateRect(3,0,3,0);
	m_GridCtrl.MoveWindow(rcClient);
// 	if ( rcOld.Width() < rcClient.Width() )
// 	{
// 		SetColWidthAccordingFont();
		m_GridCtrl.ExpandColumnsToFit(TRUE);
//	}

	// 截断不可见的行
	//CCellRange rangeVisible = m_GridCtrl.GetVisibleNonFixedCellRange(NULL, FALSE, FALSE);
	//m_GridCtrl.SetRowCount(m_GridCtrl.GetFixedRowCount() + rangeVisible.GetRowSpan());
}

void CIoViewReportRank::OpenBlock( int32 iBlockId )
{
	CBlockLikeMarket *pBlock = GetBlock(iBlockId);
	
	if ( NULL == pBlock )
	{
		return;	// do nothing
	}

	// 等待数据返回再处理
	m_iBlockId = pBlock->m_blockInfo.m_iBlockId;
	//m_GridCtrl.DeleteNonFixedRows();

	// 取消realtime数据
	m_aSmartAttendMerchs.RemoveAll();

	RequestSortViewData(true);		// 申请数据

	Invalidate(TRUE);
}

CBlockLikeMarket* CIoViewReportRank::GetBlock( int32 iBlockId )
{
	return CBlockConfig::Instance()->FindBlock(iBlockId);
}

CBlockLikeMarket* CIoViewReportRank::GetCurrentBlock()
{
	return GetBlock(m_iBlockId);
}

void CIoViewReportRank::OnGridGetDispInfo( NMHDR *pNotifyStruct, LRESULT *pResult )
{
	if ( NULL != pNotifyStruct )
	{
		GV_DISPINFO	*pDisp = (GV_DISPINFO *)pNotifyStruct;
		// 尝试v mode
		
		GV_ITEM &item = pDisp->item;
		if ( item.row >=0 && item.row < m_aMerchs.GetSize() )
		{
			CMerch *pMerch = m_aMerchs[item.row];

			if ( pMerch == NULL )
			{
				return;
			}
			
			CRealtimePrice	RealTimePrice;
			if ( NULL != pMerch->m_pRealtimePrice )
			{
				RealTimePrice = *pMerch->m_pRealtimePrice;
			}

			CGridCellSymbol *pCell = (CGridCellSymbol *)m_GridCtrl.GetDefaultVirtualCell();
			ASSERT( pCell->IsKindOf(RUNTIME_CLASS(CGridCellSymbol)) );
			pCell->SetShowSymbol(CGridCellSymbol::ESSNone);

			float fValue;
			CString StrValue;
			
			switch (item.col)
			{
			case 0:		// 商品名称
				{
					item.rtTextPadding = CRect(10,0,0,0);
					item.nFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX;
					pCell->SetShowSymbol(CGridCellSymbol::ESSNone);
				
					T_Block *pUserBlock = CUserBlockManager::Instance()->GetBlock(pMerch);
					if ( NULL != pUserBlock )
					{
						if ( pUserBlock->m_clrBlock != COLORNOTCOUSTOM )
						{
							item.crFgClr = pUserBlock->m_clrBlock;
						}
					}

					// 是否有标记
					GV_DRAWMARK_ITEM markItem;
					if ( InitDrawMarkItem(pMerch, markItem) )
					{
						item.markItem = markItem;
					}
					
					item.strText = pMerch->m_MerchInfo.m_StrMerchCnName;
					item.lParam = (LPARAM)pMerch;
				}
				break;
			case 1:		// 现价
				{
					StrValue = _T(" -");
					item.nFormat = DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX;
					pCell->SetShowSymbol(CGridCellSymbol::ESSNone);
					
					if ( RealTimePrice.m_fPriceNew != 0.0 && RealTimePrice.m_fPricePrevClose != 0.0 )
					{
						fValue = RealTimePrice.m_fPriceNew;
						StrValue = Float2SymbolString(fValue, RealTimePrice.m_fPricePrevClose, 2);
					}
					
					item.strText = StrValue;
				}
				break;
			case 2:		// 具体排名值
				{
					item.rtTextPadding = CRect(0,0,10,0);
					item.nFormat = DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX;
					// 涨 跌 振 5+ 5- 量 委比+ 委比- 金额
					switch (m_headerCur.m_eField)
					{
					case EMRFRisePercent:	// 涨跌
						{
							StrValue = _T(" -");
							pCell->SetShowSymbol(CGridCellSymbol::ESSFall);
							
							if ( RealTimePrice.m_fPriceNew != 0.0 && RealTimePrice.m_fPricePrevClose != 0.0 )
							{
								fValue = RealTimePrice.m_fPriceNew - RealTimePrice.m_fPricePrevClose;
								fValue = fValue / RealTimePrice.m_fPricePrevClose * 100.0;
								StrValue = Float2SymbolString(fValue, 0, 2, false, false);
							}

							item.strText = StrValue;
						}
						break;
					case EMRFAmplitude:		// 振幅
						{
							StrValue = _T(" -");
							pCell->SetShowSymbol(CGridCellSymbol::ESSFall);
							
							if (  RealTimePrice.m_fPriceLow != 0.0 && RealTimePrice.m_fPriceHigh != 0.0 && RealTimePrice.m_fPricePrevClose != 0.0 )
							{
								fValue = RealTimePrice.m_fPriceHigh - RealTimePrice.m_fPriceLow;
								fValue = fValue / RealTimePrice.m_fPricePrevClose * 100.0;
								StrValue = Float2SymbolString(fValue, 0, 2, false, false);
							}
							
							item.strText = StrValue;
						}
						break;
					case EMRFRiseRate:		// 5分钟速涨 - 不知道数据，暂时与涨跌同处理
						{
							StrValue = _T(" -");
							pCell->SetShowSymbol(CGridCellSymbol::ESSFall);

							fValue = RealTimePrice.m_fRiseRate;
							StrValue = Float2SymbolString(fValue, 0, 2, false, false);

							item.strText = StrValue;
						}
						break;
					case EMRFVolRatio:	// 量比 - 
						{
							StrValue = _T(" -");
							pCell->SetShowSymbol(CGridCellSymbol::ESSNone);

							fValue = RealTimePrice.m_fVolumeRate;
							StrValue = Float2SymbolString(fValue, 1.0f, 2);
							
							item.strText = StrValue;
						}
						break;
					case EMRFBidRatio:	// 委比＝(委买手数－委卖手数)/(委买手数＋委卖手数)×100％
						{
							StrValue = _T(" -");
							pCell->SetShowSymbol(CGridCellSymbol::ESSFall);

							float fRate = 0.0;
							int32 iBuyVolums  = 0;
							int32 iSellVolums = 0;
							
							for ( int32 i = 0 ; i < 5 ; i++)
							{
								iBuyVolums  += (int32)(RealTimePrice.m_astBuyPrices[i].m_fVolume);
								iSellVolums += (int32)(RealTimePrice.m_astSellPrices[i].m_fVolume);
							}
							
							if ( 0 != (iBuyVolums + iSellVolums) )
							{
								fRate = (float)(iBuyVolums - iSellVolums)*(float)100 / (iBuyVolums + iSellVolums);
							}
							
							pCell->SetShowSymbol(CGridCellSymbol::ESSNone);
							
							StrValue = Float2SymbolString(fRate, 0.0, 2, false, true, false);
							
							item.strText = StrValue;
						}
						break;
					case EMRFAmountTotal:	// 金额
						{
							StrValue = _T(" -");
							pCell->SetShowSymbol(CGridCellSymbol::ESSNone);

							fValue = RealTimePrice.m_fAmountTotal;

							StrValue = Float2String(fValue, 2, true);
							
							item.strText = StrValue;
						}
						break;
					}
				}
				break;
			default:
				ASSERT( 0 );
			}
		}
	}

	if ( NULL != *pResult )
	{
		*pResult = 1;
	}
}

bool32 CIoViewReportRank::FromXml( TiXmlElement *pTiXmlElement )
{
	if (NULL == pTiXmlElement)
		return false;
	
	// 判读是不是IoView的节点
	const char *pcValue = pTiXmlElement->Value();
	if (NULL == pcValue || strcmp(GetXmlElementValue(), pcValue) != 0)
		return false;
	
	// 判断是不是描述自己这个业务视图的节点
	const char *pcAttrValue = pTiXmlElement->Attribute(GetXmlElementAttrIoViewType());
	CString StrIoViewString = _A2W(pcAttrValue);
	if (NULL == pcAttrValue || CIoViewManager::GetIoViewString(this).Compare(StrIoViewString) != 0)	// 不是描述自己的业务节点
		return false;

	SetFontsFromXml(pTiXmlElement);
	SetColorsFromXml(pTiXmlElement);
	
	// block
	pcAttrValue = pTiXmlElement->Attribute(KStrViewRankMarketId);
	if ( NULL != pcAttrValue )
	{
		// 是否正确
		int32 iBlockId = atoi(pcAttrValue);
		if ( GetBlock(iBlockId) != NULL )
		{
			m_iBlockId = iBlockId;
		}
	}

	T_Header header;
	pcAttrValue = pTiXmlElement->Attribute(KStrViewRankField);
	if ( NULL != pcAttrValue )
	{
		header.m_eField = (E_MerchReportField)atoi(pcAttrValue);
	}
	pcAttrValue = pTiXmlElement->Attribute(KStrViewRankDesc);
	if ( NULL != pcAttrValue )
	{
		header.m_bDescSort = atoi(pcAttrValue) != 0;
	}
	for ( int i=0; i < m_saHeaders.GetSize() ; i++ )
	{
		if ( m_saHeaders[i].m_eField == header.m_eField && m_saHeaders[i].m_bDescSort == header.m_bDescSort )
		{
			m_headerCur = m_saHeaders[i];
			break;
		}
	}
	// 无效的排行信息则不处理
	
	OpenBlock(m_iBlockId);

	return true;
}

CString CIoViewReportRank::ToXml()
{
	//
	CString StrThis;
	
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\"   %s=\"%d\" %s=\"%d\" %s=\"%d\" ", 
		CString(GetXmlElementValue()).GetBuffer(), 
		CString(GetXmlElementAttrIoViewType()).GetBuffer(), 
		CIoViewManager::GetIoViewString(this).GetBuffer(),
		CString(GetXmlElementAttrShowTabName()).GetBuffer(), m_StrTabShowName.GetBuffer(),
		CString(GetXmlElementAttrMerchCode()).GetBuffer(),
		L"",
		CString(GetXmlElementAttrMarketId()).GetBuffer(), 
		L"-1",
		CString(KStrViewRankMarketId).GetBuffer(), m_iBlockId,
		CString(KStrViewRankField).GetBuffer(), m_headerCur.m_eField,
		CString(KStrViewRankDesc).GetBuffer(), m_headerCur.m_bDescSort ? 1 : 0 );
	
	StrThis += SaveColorsToXml();
	StrThis += SaveFontsToXml();
	StrThis += L">\n";
	//
	StrThis += L"</";
	StrThis += CString(GetXmlElementValue());
	StrThis += L">\n";
	return StrThis;
}

CString CIoViewReportRank::GetDefaultXML()
{
	CString StrThis;
	
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\"   ", 
		CString(GetXmlElementValue()).GetBuffer(), 
		CString(GetXmlElementAttrIoViewType()).GetBuffer(), 
		CIoViewManager::GetIoViewString(this).GetBuffer(),
		CString(GetXmlElementAttrMerchCode()).GetBuffer(),
		L"",
		CString(GetXmlElementAttrMarketId()).GetBuffer(), 
		L"-1"
		);
	
	StrThis += L">\n";
	//
	StrThis += L"</";
	StrThis += CString(GetXmlElementValue());
	StrThis += L">\n";
	return StrThis;
}

void CIoViewReportRank::LockRedraw()
{
	
}

void CIoViewReportRank::UnLockRedraw()
{
	
}

void CIoViewReportRank::OnVDataReportUpdate( int32 iMarketId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount,  const CArray<CMerch*, CMerch*> &aMerchs )
{
	
}

void CIoViewReportRank::OnVDataReportInBlockUpdate( int32 iBlockId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount, const CArray<CMerch *, CMerch *> &aMerchs )
{
	//if ( iBlockId == -1 )
	{
		CBlockLikeMarket	*pBlock = GetCurrentBlock();
		if ( NULL == pBlock )
		{
			return;
		}

		if ( m_MmiRequestSys.m_iBlockId == iBlockId
			&& m_MmiRequestSys.m_iStart	 == iPosStart		 
			&& m_MmiRequestSys.m_bDescSort == bDescSort
			&& m_MmiRequestSys.m_eMerchReportField == eMerchReportField
			&& m_MmiRequestSys.m_iCount <= iOrgMerchCount )	// 数量量大的也认为是要求的
		{
			// 得到这个视图需要的商品
			MerchArray	aMerchMy;
			aMerchMy.SetSize(0, m_iMaxGridVisibleRow+1);
			for ( int i=0; i < aMerchs.GetSize() && i <= m_iMaxGridVisibleRow ; i++ )		// 最多可见个商品
			{
				if ( pBlock->IsMerchInBlock(aMerchs[i]) )
				{
					aMerchMy.Add(aMerchs[i]);
				}
			}
			m_aMerchs.Copy(aMerchMy);
			UpdateTableAllContent();	// 只有一个商品列表传过来，具体数据需要单独申请
			RequestRealTimePrice();
		}
	}
}

void CIoViewReportRank::OnVDataRealtimePriceUpdate( IN CMerch *pMerch )
{
	for ( int i=0; i < m_aMerchs.GetSize() ; i++ )
	{
		if ( m_aMerchs[i] == pMerch
			&& m_GridCtrl.GetRowCount()-m_GridCtrl.GetFixedRowCount() > i )		// 现在保证数据和显示顺序是一致的
		{
			ASSERT( m_GridCtrl.GetItemData(i, 0) == (LPARAM)pMerch );
			m_GridCtrl.RedrawRow(i);
		}
	}
}

BOOL CIoViewReportRank::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;
}

void CIoViewReportRank::GetTitleString( OUT CString &StrTitle )
{
	CBlockLikeMarket *pBlock = GetCurrentBlock();
	if ( NULL != pBlock && m_bShowBlockNameOnTitle )
	{
		StrTitle.Format(_T("%s  %s"), pBlock->m_blockInfo.m_StrBlockName.GetBuffer(), m_headerCur.m_StrName.GetBuffer());
	}
	else
	{
		StrTitle = m_headerCur.m_StrName;
	}
}

void CIoViewReportRank::RequestData( CMmiCommBase &req, bool32 bForce/* = false*/ )
{
	DoRequestViewData(req, bForce);
}

void CIoViewReportRank::OnContextMenu( CWnd* pWnd, CPoint pos )
{
	CRect rcClient;
	GetWindowRect(rcClient);
	if ( !rcClient.PtInRect(pos) )
	{
		pos = rcClient.TopLeft();
		DoTrackMenu(pos);
		return;
	}

	if ( pWnd->GetSafeHwnd() == m_GridCtrl.m_hWnd )
	{
		CIoViewBase::OnContextMenu(pWnd, pos);
		return;
	}
	
	DoTrackMenu(pos);
}

CMerch  * CIoViewReportRank::GetSelectMerch()
{
	CCellRange  cellRange = m_GridCtrl.GetSelectedCellRange();
	if ( cellRange.GetMinRow() >= 0 && cellRange.GetRowSpan() >= 0 )
	{
		return ((CMerch *)m_GridCtrl.GetItemData(cellRange.GetMinRow(), 0));
	}
	return NULL;
}

void CIoViewReportRank::DoTrackMenu(CPoint posScreen)
{
	if ( GetIoViewManager() == NULL || DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame()) == NULL )
	{
		return;	// childframe下才能菜单显示
	}

	CBlockConfig::BlockArray aBlockPhys;
	CBlockConfig::Instance()->GetMarketClassBlocks(aBlockPhys);	// 可能以后要过滤掉一些板块

	int32 i = 0;
	for ( i=0; i < aBlockPhys.GetSize() ; i++ )
	{
		if ( aBlockPhys[i]->m_blockInfo.m_iBlockId == m_iBlockId )
		{
			aBlockPhys.RemoveAt(i);
			break;
		}
	}
	
	int iCmd = 1;
	const int iMarketCmdBase = 10;
	const int iRankCmdBase = 1000;
	CNewMenu menu, menuMarket, menuRank;
	menu.CreatePopupMenu();
	menuMarket.CreatePopupMenu();
	menuRank.CreatePopupMenu();
	CMerch *pMerch = GetSelectMerch();
	if ( NULL != pMerch )
	{
		CString StrMerch;
		StrMerch.Format(_T("打开商品[%s]"), pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer());
		menu.AppendMenu(MF_STRING, iCmd, StrMerch);
		menu.SetDefaultItem(iCmd, FALSE);
		menu.AppendMenu(MF_SEPARATOR);
	}
	
	iCmd = iMarketCmdBase;
// 	for ( i=0; i < aMarkets.GetSize() ; i++ )
// 	{
// 		menuMarket.AppendMenu(MF_STRING, iCmd++, aMarkets[i]->m_MarketInfo.m_StrCnName);
// 	}
	for ( i=0; i < aBlockPhys.GetSize() ; i++ )
	{
		menuMarket.AppendMenu(MF_STRING, iCmd++, aBlockPhys[i]->m_blockInfo.m_StrBlockName);
	}
	if ( menuMarket.GetMenuItemCount() > 0 )
	{
		menu.AppendMenu(MF_STRING |MF_POPUP, (UINT)menuMarket.m_hMenu, _T("选择市场"));
	}

	iCmd = iRankCmdBase;
	HeaderArray aHeaders;
	for ( i=0; i < m_saHeaders.GetSize() ; i++ )
	{
		if ( m_headerCur.m_StrName != m_saHeaders[i].m_StrName )
		{
			menuRank.AppendMenu(MF_STRING, iCmd++, m_saHeaders[i].m_StrName);
			aHeaders.Add(m_saHeaders[i]);
		}
	}
	if ( menuRank.GetMenuItemCount() > 0 )
	{
		menu.AppendMenu(MF_STRING |MF_POPUP, (UINT)menuRank.m_hMenu, _T("选择排行"));
	}

	// 增加 风格设置 字体放大 字体缩小选项
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, IDM_IOVIEWBASE_FONT_BIGGER, _T("字体放大 CTRL+↑"));
	menu.AppendMenu(MF_STRING, IDM_IOVIEWBASE_FONT_SMALLER, _T("字体缩小 CTRL+↓"));

	AppendStdMenu(&menu);
	//menu.AppendMenu(MF_STRING, ID_SETTING, _T("风格设置"));

	// 如果处在锁定分割状态，需要删除一些按钮
	CMPIChildFrame *pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pChildFrame && pChildFrame->IsLockedSplit() )
	{
		pChildFrame->RemoveSplitMenuItem(menu);
	}

	int iRet = menu.TrackPopupMenu(TPM_TOPALIGN |TPM_LEFTALIGN |TPM_NONOTIFY |TPM_RETURNCMD, posScreen.x, posScreen.y, AfxGetMainWnd());
	if ( iRet > 0 )
	{
		if ( iRet >= iCmd  )
		{
			::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, iRet, NULL);
		}
		else if ( iRet < iMarketCmdBase )
		{
			OnDblClick();
		}
		else if ( iRet < iRankCmdBase )
		{
			iRet -= iMarketCmdBase;
// 			if ( iRet < aMarkets.GetSize() )
// 			{
// 				OpenBlock(aMarkets[iRet]->m_MarketInfo.m_iMarketId);
// 			}
			if ( iRet < aBlockPhys.GetSize() )
			{
				OpenBlock(aBlockPhys[iRet]->m_blockInfo.m_iBlockId);
			}
			else
			{
				ASSERT( 0 );
			}
		}
		else
		{
			iRet -= iRankCmdBase;
			if ( iRet < aHeaders.GetSize() )
			{
				ChangeRank(aHeaders[iRet]);
			}
			else
			{
				ASSERT( 0 );
			}
		}
	}
}

void CIoViewReportRank::ChangeRank( const T_Header &headerRank )
{
	m_headerCur = headerRank;
	OpenBlock(m_iBlockId);
}

bool32 CIoViewReportRank::ChangeRank( E_RankType eRank )
{
	if ( eRank < m_saHeaders.GetSize() )
	{
		ChangeRank(m_saHeaders[eRank]);
		return true;
	}
	return false;
}

CMerch * CIoViewReportRank::GetMerchXml()
{
	CCellRange rangeSel = m_GridCtrl.GetSelectedCellRange();
	CMerch *pMerch = NULL;
	if ( rangeSel.IsValid() && (pMerch = (CMerch *)m_GridCtrl.GetItemData(rangeSel.GetMinRow(), 0)) != NULL )
	{
		return pMerch;
	}
	return CIoViewBase::GetMerchXml();
}

void CIoViewReportRank::OnBlockConfigChange( int32 iBlockId, E_BlockNotifyType eNotifyType )
{
	Invalidate(TRUE);
}

void CIoViewReportRank::SetShowBlockNameOnTitle( bool32 bShow /*= true*/ )
{
	bool32 bOld = m_bShowBlockNameOnTitle;
	m_bShowBlockNameOnTitle = bShow;
	if ( bOld != bShow )
	{
		Invalidate();
	}
}

void CIoViewReportRank::GetAttendMarketDataArray( OUT AttendMarketDataArray &aAttends )
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

bool32 CIoViewReportRank::GetCellCount( OUT int32 &iRowCount, OUT int32 &iColumnCount )
{
	CCellRange rangeVis = m_GridCtrl.GetVisibleNonFixedCellRange(NULL, FALSE, FALSE);
	if ( rangeVis.IsValid() )
	{
		iRowCount = rangeVis.GetRowSpan();
		iColumnCount = rangeVis.GetColSpan();
		return true;
	}
	iRowCount = iColumnCount = 0;
	return false;
}

bool32 CIoViewReportRank::SetFocusCell( int32 iRow, int32 iColumn, bool32 bSetFocus )
{
	CCellRange rangeVis = m_GridCtrl.GetVisibleNonFixedCellRange(NULL, FALSE, FALSE);
	if ( rangeVis.IsValid()
		&& rangeVis.InRange(iRow, iColumn) )
	{
		if ( bSetFocus )
		{
			m_GridCtrl.SetFocus();
		}
		m_GridCtrl.SetFocusCell(iRow, iColumn);
		m_GridCtrl.SetSelectedRange(iRow, 0, iRow, m_GridCtrl.GetColumnCount()-1);
		return true;
	}
	return false;
}

bool32 CIoViewReportRank::GetFocusCell( OUT int32 &iRow, OUT int32 &iColumn )
{
	CCellID cellId = m_GridCtrl.GetFocusCell();
	if ( cellId.IsValid() )
	{
		iRow = cellId.row;
		iColumn = cellId.col;
		return true;
	}
	return false;
}

void CIoViewReportRank::RequestSortViewDataAsync()
{
	SetTimer(KTimerIdReqSortAsync, kTimerPeriodReqSortAsync, NULL);
}

void CIoViewReportRank::OnGridSelRowChanged( NMHDR *pNotifyStruct, LRESULT* pResult )
{
	if (IsHaveKlineTrendFrame())
	{
		OnDblClick();
	}
}


bool CIoViewReportRank::IsHaveKlineTrendFrame()
{
	CMerch *pMerch = GetSelectMerch();
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	if (pMerch == NULL || pMainFrame == NULL)
	{
		return false;
	}
	CIoViewBase *pIoViewSrc = this;
	CMPIChildFrame *pChildFrame = NULL;
	if ( NULL == pIoViewSrc || NULL == pIoViewSrc->GetIoViewManager() )
	{
		pIoViewSrc = pMainFrame->FindActiveIoView();	// 找到激活的标准业务视图
		if ( NULL == pIoViewSrc )
		{
			// 无激活视图，则查找分时或者k线页面, 不存在则创建
			pChildFrame = pMainFrame->FindChartIoViewFrame(true, true);
			if ( NULL != pChildFrame )
			{
				pIoViewSrc = pMainFrame->FindActiveIoViewInFrame(pChildFrame);
			}
		}
		if ( NULL == pIoViewSrc )
		{
			return false;
		}
	}

	if (NULL == pIoViewSrc || NULL == pIoViewSrc->GetIoViewManager())
	{
		return false;
	}

	CAbsCenterManager *pAbsCenterManager = pIoViewSrc->GetCenterManager();
	if ( NULL == pAbsCenterManager )
	{
		return false; 
	}
	CIoViewBase *pChart = NULL;
	pChart = pMainFrame->FindChartIoViewInSameFrame(pIoViewSrc);	// 同窗口无结果，则需要新建页面
	if ( NULL == pChart )
	{
		return false;
	}
	
	return true;
}