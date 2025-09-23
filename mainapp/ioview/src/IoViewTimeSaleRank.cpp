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
#include "IoViewTimeSaleRank.h"
#include "LogFunctionTime.h"

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int32 KUpdatePushMerchsTimerId		= 100005;			// 每隔 n 秒钟, 重新计算一次当前的推送商品
const int32 KTimerPeriodPushMerchs			= 1000 * 60 *2;

static const int32 KMinGridCtrlId = 0x700;
static const int32 KGridCtrlCount = CIoViewTimeSaleRank::T_Tab::HT_COUNT;

namespace
{
#define  INVALID_ID  -1
#define GRIDCTRL_MOVEDOWN_SIZE	    50

#define MID_BUTTON_PAIHANG_START	30001
#define MID_BUTTON_PAIHANG_PROMPT	30002
#define MID_BUTTON_PAIHANG_ONE		30003
#define MID_BUTTON_PAIHANG_THREE	30004
#define MID_BUTTON_PAIHANG_FIVE		30005
#define MID_BUTTON_PAIHANG_TEN		30006
#define MID_BUTTON_PAIHANG_END		30010
}


#define COLO_NO_NAME_CODE  RGB(240,248,136)

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CIoViewTimeSaleRank, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewTimeSaleRank, CIoViewBase)
//{{AFX_MSG_MAP(CIoViewTimeSaleRank)
ON_WM_PAINT()
ON_WM_ERASEBKGND()
ON_WM_CREATE()
ON_WM_SIZE()
ON_WM_TIMER()
ON_WM_CONTEXTMENU()
ON_MESSAGE_VOID(UM_DOINITIALIZE, OnDoInitialize)
ON_WM_HSCROLL()
ON_WM_LBUTTONUP()
ON_WM_LBUTTONDOWN()
//}}AFX_MSG_MAP
//linhc 20100909选择行改变消息
ON_NOTIFY_RANGE(GVN_SELCHANGED,KMinGridCtrlId,KMinGridCtrlId+KGridCtrlCount,OnGridSelRowChanged)
ON_NOTIFY_RANGE(NM_RCLICK, KMinGridCtrlId, KMinGridCtrlId+KGridCtrlCount, OnGridRButtonDown)
ON_NOTIFY_RANGE(NM_DBLCLK, KMinGridCtrlId, KMinGridCtrlId+KGridCtrlCount,  OnGridDblClick)
ON_NOTIFY_RANGE(GVN_COLWIDTHCHANGED, KMinGridCtrlId, KMinGridCtrlId+KGridCtrlCount,  OnGridColWidthChanged)
ON_NOTIFY_RANGE(GVN_KEYDOWNEND, KMinGridCtrlId, KMinGridCtrlId+KGridCtrlCount, OnGridKeyDownEnd)
ON_NOTIFY_RANGE(GVN_KEYUPEND, KMinGridCtrlId, KMinGridCtrlId+KGridCtrlCount, OnGridKeyUpEnd)
ON_NOTIFY_RANGE(GVN_GETDISPINFO, KMinGridCtrlId, KMinGridCtrlId+KGridCtrlCount, OnGridGetDispInfo)
END_MESSAGE_MAP()
///////////////////////////////////////////////////////////////////////////////

// 准备优化 fangz20100514

CIoViewTimeSaleRank::CIoViewTimeSaleRank()
:CIoViewBase()
{
	m_pParent		= NULL;
	m_rectClient	= CRect(-1,-1,-1,-1);
	
	m_aUserBlockNames.RemoveAll();

	m_bInitialized = false;

	m_pLastFocusGrid = NULL;

	m_iPeriod = 1;

	m_iBlockId = CBlockConfig::GetDefaultMarketlClassBlockPseudoId();		// 沪深A股请求

	m_pCurrentXSBHorz = NULL;
	m_pImgBtn = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewTimeSaleRank::~CIoViewTimeSaleRank()
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
void CIoViewTimeSaleRank::OnPaint()
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

	CRect rectClient;
	GetClientRect(&rectClient);

	COLORREF clrBk = GetIoViewColor(ESCBackground);
	COLORREF clrText = GetIoViewColor(ESCText);
	COLORREF clrAxis = GetIoViewColor(ESCChartAxisLine);
	COLORREF clrVol = GetIoViewColor(ESCVolume2);
	COLORREF clrBlock = GetIoViewColor(ESCRise);

	if ( clrBk == CFaceScheme::Instance()->GetSysColor(ESCBackground) )
	{
		clrBk = RGB(44, 0, 0);
	}
	else
	{
		// 灰度
		CColorStep step;
		step.InitColorRange(clrBk, 32.0f, 32.0f, 32.0f);
		clrBk = step.GetColor(1);
	}

	int32 iSaveDC = dc.SaveDC();

	CFont *pFontNormal = CFaceScheme::Instance()->GetSysFontObject(ESFBig);
	dc.SelectObject(pFontNormal);
	
	for ( int i=0; i < m_aTabInfos.GetSize() ; i++ )
	{
		m_aTabInfos[i].Draw(dc);
	}
	
	dc.RestoreDC(iSaveDC);
	RedrawCycleBtn(&dc);
}

BOOL CIoViewTimeSaleRank::PreTranslateMessage(MSG* pMsg)
{
	if ( WM_KEYDOWN == pMsg->message )
	{ 
		if ( VK_RETURN == pMsg->wParam )
		{
			if ( NULL != m_pLastFocusGrid
				&& pMsg->hwnd == m_pLastFocusGrid->GetSafeHwnd() )
			{
				CCellRange rangeSel = m_pLastFocusGrid->GetSelectedCellRange();
				if ( rangeSel.IsValid() 
					&& rangeSel.GetMinRow() >= m_pLastFocusGrid->GetFixedRowCount()
					&& rangeSel.GetMinRow() < m_pLastFocusGrid->GetRowCount() )
				{
					CMerch *pMerch = (CMerch *)m_pLastFocusGrid->GetItemData(rangeSel.GetMinRow(), 0);
					OnDblClick(pMerch);
					return TRUE;
				}
			}
		}
	}
	//else if ( WM_MOUSEWHEEL == pMsg->message )
	//{
	//	// 拦截滚轮
	//	short zDelta = HIWORD(pMsg->wParam);
	//	zDelta /= 120;

	//	// 自己处理
	//	//OnMouseWheel(zDelta);
	//	//return TRUE;
	//}
	else if ( WM_LBUTTONUP == pMsg->message )
	{
		// 设置最后一个gridfocus
		CWnd *pFocusWnd = GetFocus();
		for ( int i=0; i < m_aTabInfos.GetSize() && pFocusWnd != NULL ; i++ )
		{
			if ( m_aTabInfos[i].m_wndGrid.GetSafeHwnd() == pFocusWnd->GetSafeHwnd() )
			{
				m_pLastFocusGrid = &m_aTabInfos[i].m_wndGrid;
				break;
			}
		}
	}

	return CControlBase::PreTranslateMessage(pMsg);		// 越过IoViewBase的键盘处理
}

int CIoViewTimeSaleRank::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	ASSERT( NULL != m_pAbsCenterManager );
	if ( NULL == m_pAbsCenterManager )	// 自己注册
	{
		CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
		SetCenterManager(pApp->m_pDocument->m_pAbsCenterManager);
	}
	
	CIoViewBase::OnCreate(lpCreateStruct);
	
	InitialIoViewFace(this);

	m_aTabInfos.SetSize(KGridCtrlCount);
	
	for ( int i=0; i < m_aTabInfos.GetSize() ; i++ )
	{
		m_aTabInfos[i].Create(this, i + KMinGridCtrlId);
		m_aTabInfos[i].m_eType = (T_Tab::E_HeaderType)(i);
	}

	// 表头固定，可以初始化表头了
	UpdateTableHeader();

	m_pLastFocusGrid = &m_aTabInfos[0].m_wndGrid;		// 默认第一个为聚焦

	PostMessage(UM_DOINITIALIZE, 0, 0);

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

void CIoViewTimeSaleRank::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);
	RecalcLayout();
	
	RedrawWindow();             // 立即更新, 避免子窗口刷新占用太长时间导致自己显示不全
}

BOOL CIoViewTimeSaleRank::TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return FALSE;
}

void CIoViewTimeSaleRank::OnIoViewActive()
{
	m_bActive = IsActiveInFrame();

	//
	//OnVDataForceUpdate();
	RequestLastIgnoredReqData();

	SetChildFrameTitle();
	
	if ( NULL != m_pLastFocusGrid )
	{
		m_pLastFocusGrid->SetFocus();
		//m_pLastFocusGrid->Refresh();
	}
	//else
	//{
	//	SetFocus();
	//}
	
	//Invalidate(TRUE);
}

void CIoViewTimeSaleRank::OnIoViewDeactive()
{
	m_bActive = false;
	
	//Invalidate(TRUE);
}

// 通知视图改变关注的商品
void CIoViewTimeSaleRank::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch)
		return;
	
	// 修改当前查看的商品
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
}

//
void CIoViewTimeSaleRank::OnVDataForceUpdate()
{
	//RequestViewDataCurrentVisibleRow();
	RequestSortData(true, m_iPeriod);
}

void CIoViewTimeSaleRank::OnDestroy()
{	
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if ( NULL != pMainFrm && pMainFrm->m_pKBParent == this )
	{
		pMainFrm->SetHotkeyTarget(NULL);
	}

	CIoViewBase::OnDestroy();
}

void CIoViewTimeSaleRank::OnTimer(UINT nIDEvent) 
{
	if(!m_bShowNow)
	{
		return;
	}

	if ( nIDEvent == KUpdatePushMerchsTimerId )
	{
		RequestSortData(false, m_iPeriod);
		//RequestViewDataCurrentVisibleRow();		
	}
	
	CIoViewBase::OnTimer(nIDEvent);
}

void CIoViewTimeSaleRank::RequestViewDataCurrentVisibleRow()
{
	for ( int i=0; i < m_aTabInfos.GetSize() ; i++ )
	{
		m_aTabInfos[i].RequestVisibleData();
	}
}

void CIoViewTimeSaleRank::RequestSortData(bool32 bForceReq/* = false*/, uint8 iPeriod)
{
	for ( int i=0; i < m_aTabInfos.GetSize() ; i++ )
	{
		m_aTabInfos[i].RequestSortData(bForceReq, iPeriod);
	}	
}

void CIoViewTimeSaleRank::SetChildFrameTitle()
{
	
}

void CIoViewTimeSaleRank::OnDblClick(CMerch *pMerch)
{
	if ( pMerch != NULL )
	{
		CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
		pMainFrame->OnShowMerchInChart(pMerch, this);
	}
}

void CIoViewTimeSaleRank::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();	
}

void CIoViewTimeSaleRank::OnIoViewFontChanged()
{
	CIoViewBase::OnIoViewFontChanged();

	for ( int i=0; i < m_aTabInfos.GetSize() ; i++ )
	{
		CGridCtrl *pCtrl = &m_aTabInfos[i].m_wndGrid;
		LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
		pCtrl->GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
		pCtrl->GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
		pCtrl->GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
		pCtrl->GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);

		//pCtrl->AutoSizeColumns(GVS_BOTH);
	}
	
	RecalcLayout();

	Invalidate();
}

void CIoViewTimeSaleRank::SetRowHeightAccordingFont()
{
	
}

void CIoViewTimeSaleRank::SetColWidthAccordingFont()
{
	
}

void CIoViewTimeSaleRank::UpdateTableHeader()
{
	for ( int i=0; i < m_aTabInfos.GetSize() ; i++ )
	{
		m_aTabInfos[i].UpdateTableHeader();
	}
}

void CIoViewTimeSaleRank::UpdateTableContent( CMerch *pMerch, bool32 bBlink )
{
	if ( NULL == pMerch )
	{
		return;
	}

	for ( int i=0; i < m_aTabInfos.GetSize() ; i++ )
	{
		m_aTabInfos[i].OnMerchShowDataUpdate(pMerch);
	}
}

void CIoViewTimeSaleRank::UpdateTableAllContent()
{
	Invalidate();		// 刷新就可以了
}

void CIoViewTimeSaleRank::OnDoInitialize()
{
	m_bInitialized = true;		// 仅仅是一个标志，现在还不做实质初始化

	OpenBlock(m_iBlockId);		// 重装载一次板块数据
}

void CIoViewTimeSaleRank::RecalcLayout()
{
	CRect rcClient;
	GetClientRect(rcClient);

	ASSERT( KGridCtrlCount == m_aTabInfos.GetSize() );		// 两行4列
	const int32 iCol = 4;
	const int32 iMinCellWidth = 250;	// 每个单元格的最小宽度
	const CSize sizeVirtual = GetVirtualSize();
	const int32 iLeftPos = GetVirtualLeftPos();
	rcClient.left	= -iLeftPos;
	int32 iRight = rcClient.left + sizeVirtual.cx;		// 给与虚拟宽度
	if ( iRight < rcClient.right )
	{
		rcClient.left = rcClient.right - sizeVirtual.cx;
	}
	else
	{
		rcClient.right	= rcClient.left + sizeVirtual.cx;		// 给与虚拟宽度
	}

	int32 iRow = m_aTabInfos.GetSize() / iCol;
	if ( m_aTabInfos.GetSize() % iCol > 0 )
	{
		ASSERT( 0 );
		iRow++;
	}

	float fWidthPer = rcClient.Width() / (float)iCol;
	float fHeightPer = (rcClient.Height() - GRIDCTRL_MOVEDOWN_SIZE) / (float)iRow;

	int32 iLeft, iTop;
	iTop = rcClient.top + GRIDCTRL_MOVEDOWN_SIZE;
	int32 iTabIndex = 0;
	for ( int32 i=0; i < iRow && iTabIndex < m_aTabInfos.GetSize() ; i++ )
	{
		iLeft = rcClient.left;
		int32 iBottom = iTop + (int32)fHeightPer;
		if ( i == iRow - 1 )
		{
			iBottom = rcClient.bottom;
		}

		for ( int32 j=0; j < iCol ; j++ )
		{
			if ( iTabIndex >= m_aTabInfos.GetSize() )
			{
				break;
			}

			// 分配
			CRect rcTab(0,0,0,0);
			rcTab.left		= iLeft;
			rcTab.right		= rcTab.left + (int32)fWidthPer;
			rcTab.top		= iTop;
			rcTab.bottom	= iBottom;

			if ( j == iCol - 1 )
			{
				rcTab.right = rcClient.right;
			}

			m_aTabInfos[iTabIndex].MoveRect(rcTab, FALSE);
			
			iLeft = rcTab.right;
			iTabIndex++;
		}

		iTop += (int32)fHeightPer;
	}

	ShowOrHideXHorzBar();
	ResetXSBHorz();
}

void CIoViewTimeSaleRank::OpenBlock( int32 iBlockId )
{
	CBlockLikeMarket	*pBlock = GetBlock(iBlockId);
	if ( NULL == pBlock )
	{
		return;
	}

	m_iBlockId = iBlockId;

	// 获取板块数据, 以做过滤... TODO

	UpdateTableAllContent();

	RequestSortData(true, m_iPeriod);		// 强制申请排行数据
}

CBlockLikeMarket* CIoViewTimeSaleRank::GetBlock( int32 iBlockId )
{
	return CBlockConfig::Instance()->FindBlock(iBlockId);
}

CBlockLikeMarket* CIoViewTimeSaleRank::GetCurrentBlock()
{
	return GetBlock(m_iBlockId);
}

void CIoViewTimeSaleRank::OnGridGetDispInfo(UINT nId, NMHDR *pNotifyStruct, LRESULT *pResult )
{
	if ( nId < KMinGridCtrlId || nId >= KMinGridCtrlId + KGridCtrlCount )
	{
		return;
	}

	T_Tab &tab = m_aTabInfos[nId - KMinGridCtrlId];
	
	if ( NULL != pNotifyStruct )
	{
		GV_DISPINFO	*pDisp = (GV_DISPINFO *)pNotifyStruct;
		// 尝试v mode

		bool32	bValueOk = tab.GetCellValue(pDisp->item);
		ASSERT( bValueOk );
	}

	if ( NULL != *pResult )
	{
		*pResult = 1;
	}
}

void CIoViewTimeSaleRank::OnGridRButtonDown( UINT nId, NMHDR *pNotifyStruct, LRESULT* pResult )
{
	CGridCtrl *pCtrl = NULL;
	for ( int i=0; i < m_aTabInfos.GetSize() ; i++ )
	{
		if ( m_aTabInfos[i].m_wndGrid.GetDlgCtrlID() == nId )
		{
			pCtrl = &m_aTabInfos[i].m_wndGrid;
			break;
		}
	}
	if ( pCtrl != NULL && pNotifyStruct != NULL )
	{
		// 右键菜单
		CPoint pos;
		GetCursorPos(&pos);
		DoTrackMenu(pos);
	}
}

void CIoViewTimeSaleRank::OnGridDblClick( UINT nId, NMHDR *pNotifyStruct, LRESULT* pResult )
{
	CGridCtrl *pCtrl = NULL;
	for ( int i=0; i < m_aTabInfos.GetSize() ; i++ )
	{
		if ( m_aTabInfos[i].m_wndGrid.GetDlgCtrlID() == nId )
		{
			pCtrl = &m_aTabInfos[i].m_wndGrid;
			break;
		}
	}
	if ( pCtrl != NULL && pNotifyStruct != NULL )
	{
		NM_GRIDVIEW	*pGridView = (NM_GRIDVIEW *)pNotifyStruct;
		if ( pGridView->iRow >= pCtrl->GetFixedRowCount() )
		{
			CMerch *pMerch = (CMerch *)pCtrl->GetItemData(pGridView->iRow, 0);
			OnDblClick(pMerch);
		}
	}
}

void CIoViewTimeSaleRank::OnGridColWidthChanged( UINT nId, NMHDR *pNotifyStruct, LRESULT* pResult )
{
	
}

void CIoViewTimeSaleRank::OnGridKeyDownEnd( UINT nId, NMHDR *pNotifyStruct, LRESULT* pResult )
{
	
}

void CIoViewTimeSaleRank::OnGridKeyUpEnd( UINT nId, NMHDR *pNotifyStruct, LRESULT* pResult )
{
	
}

BOOL CIoViewTimeSaleRank::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;
}

void CIoViewTimeSaleRank::OnVDataPluginResp( const CMmiCommBase *pResp )
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

void CIoViewTimeSaleRank::OnMerchSortResp( const CMmiRespPeriodMerchSort *pResp )
{
	if ( NULL == pResp )
	{
		return;
	}

	MerchArray	aMerchsMy;
	for ( int iTab=0; iTab < m_aTabInfos.GetSize() ; iTab++ )
	{
		const CMmiReqPeriodMerchSort &req = m_aTabInfos[iTab].m_MmiRequestSys;
		if ( (pResp->m_iMarketId == -1 /*|| pResp->m_iMarketId == 65535*/)
			&& pResp->m_eReportSortType == req.m_eReportSortType
			&& pResp->m_iStart == req.m_iStart
			&& pResp->m_aMerchs.GetSize() == req.m_iCount		// 测试用所有商品都会返回来
			&& pResp->m_bDescSort == req.m_bDescSort
			&& pResp->m_iPeriod == req.m_uiPeriod
			)
		{
			if ( aMerchsMy.GetSize() <= 0 )
			{
				// 筛选商品
				int i = 0;
				MerchArray aMerchSrc;
				ConvertMerchKeysToMerchs(pResp->m_aMerchs, aMerchSrc, true);
				for ( i=0; i < aMerchSrc.GetSize() ; i++ )
				{
					aMerchsMy.Add(aMerchSrc[i]);
				}
			}

			// 
			m_aTabInfos[iTab].OnMerchSortUpdate(aMerchsMy, req.m_uiPeriod);
		}
	}
}

void CIoViewTimeSaleRank::OnTickExResp( const CMmiRespPeriodTickEx *pResp )
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
		
		{
			aMerchsUpdate.Add(pMerch);
			aTickexesUpdate.Add(tick);
		}
	}
	
	ASSERT( aMerchsUpdate.GetSize() == aTickexesUpdate.GetSize() );
	
	for ( i=0; i < aMerchsUpdate.GetSize() ; i++ )
	{
		m_mapMerchData[ aMerchsUpdate[i] ] = aTickexesUpdate[i];;		// 保存显示的数据
	}
	
	for ( i=0; i < aMerchsUpdate.GetSize() ; i++ )
	{
		pMerch = aMerchsUpdate[i];
		for ( int j=0; j < m_aTabInfos.GetSize() ; j++ )
		{
			m_aTabInfos[j].OnMerchShowDataUpdate(pMerch);
		}
	}
}

void CIoViewTimeSaleRank::RequestData( CMmiCommBase &req, bool32 bForce /*= false*/ )
{
	DoRequestViewData(req, bForce);
}

CMerch * CIoViewTimeSaleRank::GetMerchXml()
{
	if ( m_pLastFocusGrid != NULL )
	{
		CCellRange rangeSel = m_pLastFocusGrid->GetSelectedCellRange();
		if ( rangeSel.IsValid() )
		{
			return (CMerch *)m_pLastFocusGrid->GetItemData(rangeSel.GetMinRow(), 0);
		}
	}
	return CIoViewBase::GetMerchXml();
}

void CIoViewTimeSaleRank::DoTrackMenu( CPoint pos )
{
	if ( pos.x < 0 || pos.y < 0 )
	{
		GetCursorPos(&pos);
	}

	CNewMenu menu;
	menu.CreatePopupMenu();
	int32 iCmd = 1;
	
	CMerch *pMerchSel = GetMerchXml();
	if ( NULL != pMerchSel )
	{
		CString StrMerch;
		StrMerch.Format(_T("打开商品[%s]"), pMerchSel->m_MerchInfo.m_StrMerchCnName.GetBuffer());
		menu.AppendMenu(MF_STRING, iCmd++, StrMerch);
		menu.SetDefaultItem(1, FALSE);
	}
	
	AppendStdMenu(&menu);

	int32 iRet = menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_TOPALIGN |TPM_NONOTIFY |TPM_RETURNCMD, pos.x, pos.y, AfxGetMainWnd());
	if ( iRet > 0 )
	{
		if ( iRet == 1 )
		{
			OnDblClick(pMerchSel);
		}
		else
		{
			::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, iRet, 0);
		}
	}
}

void CIoViewTimeSaleRank::OnContextMenu( CWnd* pWnd, CPoint pos )
{
	CRect rcWin(0,0,0,0);
	GetWindowRect(rcWin);
	if ( pos.x < 0 || pos.y < 0 )
	{
		pos = rcWin.TopLeft();
	}

	ScreenToClient(&pos);
	for ( int i=0; i < m_aTabInfos.GetSize() ; i++ )
	{
		if ( m_aTabInfos[i].m_RectTitle.PtInRect(pos) )
		{
			ClientToScreen(&pos);
			DoTrackMenu(pos);
			return;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CStringArray CIoViewTimeSaleRank::T_Tab::m_sStrHeaderTypeNames;
CStringArray CIoViewTimeSaleRank::T_Tab::m_sStrHeaderTypeTitles;
CIoViewTimeSaleRank::T_Tab::SortHeaderArray	CIoViewTimeSaleRank::T_Tab::m_sSortHeaders;

CIoViewTimeSaleRank::T_Tab::T_Tab()
{
	m_eType = HugeSaleChangeRate;
	m_RectTitle.SetRectEmpty();
	m_RectTab.SetRectEmpty();
	m_pwndRank = NULL;

	// 		1）超大单换手率10强：超大单换手率排名前10
	// 			2）大单换手率10强：大单换手率排名前10
	// 			3）超大单买入金额10强：超大单买入额前10名
	// 			4）大单买出金额10强：大单买入额前10名
	// 			5）成交笔数10强：略 
	// 			6）每笔金额10强：略 
	// 			7）每笔股数10强： 略
	// 			8）买入比例10强：略
	if ( m_sStrHeaderTypeNames.GetSize() <=0 )
	{
		m_sStrHeaderTypeNames.RemoveAll();
		m_sStrHeaderTypeTitles.RemoveAll();
		m_sSortHeaders.RemoveAll();

		m_sStrHeaderTypeTitles.Add(_T("超大单换手率10强"));
		m_sStrHeaderTypeTitles.Add(_T("大单换手率10强"));
		m_sStrHeaderTypeTitles.Add(_T("超大单买入金额10强"));
		m_sStrHeaderTypeTitles.Add(_T("大单买入金额10强"));
		m_sStrHeaderTypeTitles.Add(_T("成交笔数10强"));
		m_sStrHeaderTypeTitles.Add(_T("每笔金额10强"));
		m_sStrHeaderTypeTitles.Add(_T("每笔股数10强"));
		m_sStrHeaderTypeTitles.Add(_T("买入比例10强"));

		m_sStrHeaderTypeNames.Add(_T("超大单换手率(%)"));
		m_sStrHeaderTypeNames.Add(_T("大单换手率(%)"));
		m_sStrHeaderTypeNames.Add(_T("超大单买入金额(万元)"));
		m_sStrHeaderTypeNames.Add(_T("大单买入金额(万元)"));
		m_sStrHeaderTypeNames.Add(_T("成交笔数(笔)"));
		m_sStrHeaderTypeNames.Add(_T("每笔金额(万元)"));
		m_sStrHeaderTypeNames.Add(_T("每笔股数(股)"));
		m_sStrHeaderTypeNames.Add(_T("买入比例(%)"));

		m_sSortHeaders.Add(ERSPIExBigChange);
		m_sSortHeaders.Add(ERSPIBigChange);
		m_sSortHeaders.Add(ERSPIExBigBuyAmount);
		m_sSortHeaders.Add(ERSPIBigBuyAmount);
		m_sSortHeaders.Add(ERSPITradeCounts);
		m_sSortHeaders.Add(ERSPIAmountPerTrade);
		m_sSortHeaders.Add(ERSPIStockPerTrade);
		//m_sSortHeaders.Add(ERSPIExBigChange);	// 买入比例 TODO
		m_sSortHeaders.Add(ERSPIBuyRate);	// 买入比例 TODO

		ASSERT( m_sSortHeaders.GetSize() == m_sStrHeaderTypeNames.GetSize() && m_sSortHeaders.GetSize() == m_sStrHeaderTypeTitles.GetSize() );
	}
}

CIoViewTimeSaleRank::T_Tab::~T_Tab()
{
	if ( m_wndGrid.m_hWnd )
	{
		m_wndGrid.DestroyWindow();
	}
}

void CIoViewTimeSaleRank::T_Tab::MoveRect( const CRect &rc, bool32 bDraw/*=TRUE*/ )
{
	const int32 iTitleHeight = 30;
	m_RectTitle = rc;
	m_RectTitle.bottom = m_RectTitle.top + iTitleHeight;

	m_RectTab = rc;

	CRect rcGrid(rc);
	CRect rcOld;
	rcGrid.top = m_RectTitle.bottom;
	if ( m_wndGrid.GetGridLines() == GVL_NONE )
	{
		rcGrid.right = rcGrid.right-1;		// 右侧留一像素绘制分割线
		rcGrid.bottom = rcGrid.bottom-1;	// 底侧留一
	}
	m_wndGrid.GetClientRect(rcOld);
	m_wndGrid.MoveWindow(rcGrid, bDraw);

// 	if ( rcGrid.Width() > rcOld.Width() )
// 	{
// 		m_wndGrid.ExpandColumnsToFit();
// 	}
// 	else
// 	{
// 		m_wndGrid.AutoSizeColumns();
// 	}
	
	const int32 iConstBaseWidth = 70;
	CClientDC dc(m_pwndRank);
	CFont *pFontOld = dc.SelectObject(m_pwndRank->GetIoViewFontObject(ESFNormal));
	CSize sizeText = dc.GetTextExtent(_T("测试高宽高"));
	dc.SelectObject(pFontOld);
	int32 iColWidth = (int32)(sizeText.cx * 1.15f);
	iColWidth = max(iColWidth, iConstBaseWidth);
	m_wndGrid.SetColumnWidth(0, iColWidth);
	m_wndGrid.SetColumnWidth(1, iColWidth);
	//m_wndGrid.ExpandColumnsToFit(TRUE);
	int iLeave = rc.Width() - m_wndGrid.GetColumnWidth(0) - m_wndGrid.GetColumnWidth(1);
	if ( iLeave < iColWidth )
	{
		iLeave = iColWidth;
	}
	m_wndGrid.SetColumnWidth(2, iLeave);

	if ( rcOld.Height() != rcGrid.Height() )
	{	
		m_wndGrid.AutoSizeRows();
		if ( m_wndGrid.GetVirtualHeight() < rcGrid.Height() && m_aMerchs.GetSize() > 0 )
		{
			m_wndGrid.ExpandRowsToFit(TRUE);
		}
	}
}

void CIoViewTimeSaleRank::T_Tab::Draw( CDC &dc )
{
	CRect rcDraw(m_RectTitle);

	CIoViewBase *pWndGrand = NULL;
	pWndGrand = DYNAMIC_DOWNCAST(CIoViewBase, m_pwndRank->GetParent());

	COLORREF clrSeparator = m_pwndRank->GetIoViewColor(ESCGridLine);
	//if ( NULL == pWndGrand )	// 非ioview下
	//{
	//	dc.FillSolidRect(rcDraw, GetSysColor(COLOR_3DFACE));
	//	COLORREF clrText = GetSysColor(COLOR_WINDOWTEXT);
	//	dc.SetTextColor(clrText);
	//	clrSeparator = RGB(192,192,192);
	//}
	//else
	{
		COLORREF clrBk = m_pwndRank->GetIoViewColor(ESCBackground);
		COLORREF clrRed = RGB(44, 0, 0);	// 暗红
		if ( clrBk != CFaceScheme::Instance()->GetSysColor(ESCBackground) )
		{
			// 灰度
			CColorStep step;
			step.InitColorRange(clrBk, 42.0f, 42.0f, 42.0f);
			clrRed = step.GetColor(1);
		}
		CColorStep step;
		step.InitColorRange(clrRed, clrBk, rcDraw.Width()/2);
		for ( int i=rcDraw.left; i < rcDraw.right ; i++ )
		{
			dc.FillSolidRect(i, rcDraw.top, 1, rcDraw.Height(), RGB(57, 60, 67)/*step.NextColor()*/);
		}
		dc.SetTextColor(m_pwndRank->GetIoViewColor(ESCVolume));
		dc.SetBkMode(TRANSPARENT);
	}
	dc.DrawText(GetTitleString(), m_RectTitle, DT_SINGLELINE |DT_CENTER |DT_VCENTER |DT_WORD_ELLIPSIS);

	CRect rcGrid(0,0,0,0);
	m_wndGrid.GetClientRect(rcGrid);
	if ( m_RectTitle.Width() == rcGrid.Width()+1 )
	{
		// 需要补充绘制分割线 2*底边+右侧边
		dc.FillSolidRect(m_RectTitle.left, m_RectTitle.bottom-1, m_RectTitle.Width(), 1, clrSeparator);	// grid顶
		dc.FillSolidRect(m_RectTitle.right-1, m_RectTitle.top, 1, m_RectTab.Height(), clrSeparator); // 右侧
		dc.FillSolidRect(m_RectTitle.left, m_RectTab.bottom-1, m_RectTitle.Width(), 1, clrSeparator); // grid底
	}
}

BOOL CIoViewTimeSaleRank::T_Tab::Create( CIoViewTimeSaleRank *pParent, int nId )
{
	ASSERT( IsWindow(pParent->GetSafeHwnd()) );
	m_pwndRank = pParent;

	// 创建数据表格
	m_wndGrid.Create(CRect(0, 0, 0, 0), pParent, nId);
	//m_wndGrid.SetDefaultCellType(RUNTIME_CLASS(CGridCellNormal));
	m_wndGrid.SetDefaultCellType(RUNTIME_CLASS(CGridCellNormalSys));
	m_wndGrid.SetDefCellWidth(80);
	m_wndGrid.SetVirtualMode(TRUE);
	m_wndGrid.SetDoubleBuffering(TRUE);
	//m_wndGrid.SetVirtualCompare(*this);
	m_wndGrid.EnableBlink(FALSE);			// 禁止闪烁
	m_wndGrid.SetTextColor(COLO_NO_NAME_CODE);
	m_wndGrid.SetFixedTextColor(COLO_NO_NAME_CODE);
	m_wndGrid.SetTextBkColor(CLR_DEFAULT);
	m_wndGrid.SetBkColor(CLR_DEFAULT);
	m_wndGrid.SetFixedBkColor(CLR_DEFAULT);
// 	m_wndGrid.SetBkColor(GetSysColor(COLOR_WINDOW));
// 	m_wndGrid.SetFixedBkColor(GetSysColor(COLOR_WINDOW));
// 	m_wndGrid.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
// 	m_wndGrid.SetFixedTextColor(GetSysColor(COLOR_WINDOWTEXT));
// 	m_wndGrid.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
// 	m_wndGrid.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
	m_wndGrid.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_wndGrid.GetDefaultCell(TRUE, FALSE)->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_wndGrid.GetDefaultCell(FALSE, TRUE)->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_wndGrid.GetDefaultCell(TRUE, TRUE)->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

	// 创建横滚动条
	//m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE, &m_wndGrid, CRect(0,0,0,0), 10203);
	//m_XSBVert.SetScrollRange(0, 10);
	
	//m_XSBHorz.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE, &m_wndGrid, CRect(0,0,0,0), 10204);
	//m_XSBHorz.SetScrollRange(0, 10);
	
	// 设置相互之间的关联
	//m_XSBHorz.SetOwner(&m_wndGrid);
	//m_XSBVert.SetOwner(&m_wndGrid);
	//m_wndGrid.SetScrollBar(&m_XSBHorz, &m_XSBVert);
	
	// 设置表头
	m_wndGrid.SetHeaderSort(FALSE);
	m_wndGrid.SetUserCB(this);
	
	// 显示表格线的风格
	m_wndGrid.ShowGridLine(FALSE);
	m_wndGrid.SetSingleRowSelection(TRUE);
	m_wndGrid.SetDrawFixedCellGridLineAsNormal(TRUE);

	m_wndGrid.SetListMode(TRUE);
	m_wndGrid.SetAutoHideFragmentaryCell(FALSE);	// 禁止部分表格隐藏
	m_wndGrid.SetShowSelectWhenLoseFocus(FALSE);
	m_wndGrid.SetColumnResize(FALSE);
	m_wndGrid.SetRowResize(FALSE);
	
	// 设置字体
	LOGFONT *pFontNormal = m_pwndRank->GetIoViewFont(ESFNormal);
	m_wndGrid.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_wndGrid.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_wndGrid.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_wndGrid.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);

	m_wndGrid.SetDefCellWidth(50);

	return TRUE;
}

E_ReportSortPlugIn CIoViewTimeSaleRank::T_Tab::GetSortHeader()
{
	if ( m_sSortHeaders.GetSize() > (int)m_eType )
	{
		return m_sSortHeaders[m_eType];
	}
	ASSERT( 0 );
	return ERSPIEnd;
}

CString CIoViewTimeSaleRank::T_Tab::GetTitleString()
{
	if ( m_sStrHeaderTypeTitles.GetSize() > (int)m_eType )
	{
		return m_sStrHeaderTypeTitles[m_eType];
	}
	ASSERT( 0 );
	return CString();
}

CString CIoViewTimeSaleRank::T_Tab::GetNameString()
{
	if ( m_sStrHeaderTypeNames.GetSize() > (int)m_eType )
	{
		return m_sStrHeaderTypeNames[m_eType];
	}
	ASSERT( 0 );
	return CString();
}

void CIoViewTimeSaleRank::T_Tab::UpdateTableHeader()
{
	ASSERT( m_wndGrid.m_hWnd != NULL );
	
	m_wndGrid.SetFixedRowCount(1);
	m_wndGrid.SetFixedColumnCount(1);	// 代码固定区别
	m_wndGrid.DeleteNonFixedRows();

	m_wndGrid.SetColumnCount(3);

	m_wndGrid.SetDefCellHeight(32);

	m_wndGrid.Refresh();
}

void CIoViewTimeSaleRank::T_Tab::UpdateTableContent( const MerchArray &aMerchs )
{
	ASSERT( m_wndGrid.m_hWnd != NULL );

	m_aMerchs.Copy(aMerchs);
	m_wndGrid.SetRowCount(m_wndGrid.GetFixedRowCount() + aMerchs.GetSize());

	m_wndGrid.AutoSizeRows();

	CRect rcClient(0,0,0,0);
	m_wndGrid.GetClientRect(rcClient);
	if ( m_wndGrid.GetVirtualHeight() < rcClient.Height() && m_aMerchs.GetSize() > 0 )
	{
		//	不要填充行了，结果导致行高太大了
		//m_wndGrid.ExpandRowsToFit(TRUE);
	}
	CCellRange rangeSel = m_wndGrid.GetSelectedCellRange();
	if ( !rangeSel.IsValid()
		&& aMerchs.GetSize() > 0 )
	{
		m_wndGrid.SetFocusCell(m_wndGrid.GetFixedRowCount(), m_wndGrid.GetFixedColumnCount());
		m_wndGrid.SetSelectedRange(m_wndGrid.GetFixedRowCount(), 0, m_wndGrid.GetFixedRowCount(), m_wndGrid.GetColumnCount()-1);
	}
	m_wndGrid.Refresh();
}

bool32 CIoViewTimeSaleRank::T_Tab::GetCellValue(OUT GV_ITEM &item)
{
	// 修改边距
	item.rtTextPadding = CRect(15,0,0,0);

	// 估计数据是成组的，所以不需要单个商品更新，不设置data
	// 头部
	CStringArray aNames;
	aNames.Add( _T("代码") );
	aNames.Add( _T("名称") );
	aNames.Add( GetNameString() );

	ASSERT( item.col >=0 && item.col < aNames.GetSize() );
	ASSERT( item.row >=0 && item.row < m_aMerchs.GetSize()+1 );
	if ( item.row == 0 )
	{
		item.crFgClr = m_pwndRank->GetIoViewColor(ESCVolume);
		item.lParam = NULL;
		item.strText = aNames[item.col];
		if ( item.col == 2 )
		{
			item.nFormat |= DT_CENTER;
		}
	}
	else
	{
		int iIndex = item.row - 1;

		// 数据部分
		CMerch *pMerchShow = m_aMerchs[iIndex];		// 有可能为NULL
		
		if ( item.col == 0 )		// 主要处理DATA与TEXT，其它的这个视图都可以用默认的， 排序的未知商品导致很难处理，所以
		{
			item.lParam = (LPARAM)pMerchShow;
		}
		else
		{
			item.lParam = NULL;
		}

		if ( NULL != pMerchShow )
		{
			switch (item.col)
			{
			case 0:
				item.strText = pMerchShow->m_MerchInfo.m_StrMerchCode;
				break;
			case 1:
				{
					T_Block *pUserBlock = CUserBlockManager::Instance()->GetBlock(pMerchShow);
					if ( NULL != pUserBlock && pUserBlock->m_clrBlock != COLORNOTCOUSTOM )
					{
						item.crFgClr = pUserBlock->m_clrBlock;
					}
					item.strText = pMerchShow->m_MerchInfo.m_StrMerchCnName;

					// 是否有标记
					GV_DRAWMARK_ITEM markItem;
					if ( m_pwndRank->InitDrawMarkItem(pMerchShow, markItem) )
					{
						item.markItem = markItem;
					}
				}
				break;
			default:
				// n多字段不能处理
				{
					const float cf1W = 10000.0;

					item.crFgClr = m_pwndRank->GetIoViewColor(ESCRise);

					item.strText = _T(" -");
					item.nFormat |= DT_CENTER;
					T_TickEx	tickex;
					MerchDataMap::const_iterator it = m_pwndRank->m_mapMerchData.find(pMerchShow);
					if ( it != m_pwndRank->m_mapMerchData.end() )
					{
						tickex = it->second;
					}
					switch (m_eType)
					{
					case	HugeSaleChangeRate:
						item.strText = Float2String(tickex.m_fExBigChange * 100.0, 2);
						break;
					case BigSaleChangeRate:
						item.strText = Float2String(tickex.m_fBigChange * 100.0, 2);
						break;
					case HugeBuyAmount:
						item.strText = Float2String(tickex.m_fExBigBuyAmount / cf1W, 2);
						break;
					case BigBuyAmount:
						item.strText = Float2String(tickex.m_fBigBuyAmount / cf1W, 2);
						break;
					case SaleCount:
						item.strText.Format(_T("%d"), tickex.m_uiTradeTimes);
						break;
					case AmountPerSale:
						item.strText = Float2String(tickex.m_fAmountPerTrans / cf1W, 2);
						break;
					case UnitPerSale:
						item.strText = Float2String(tickex.m_fStocksPerTrans, 0);
						break;
					case BuyRate:
						{
// 							float fVolBuy, fVolSell;
// 							fVolBuy = tickex.m_fExBigBuyVol + tickex.m_fBigBuyVol + tickex.m_fMnSBuyVol;
// 							fVolSell = tickex.m_fExBigSellVol + tickex.m_fBigSellVol + tickex.m_fMnSSellVol;
// 							float fVolTotal = fVolBuy + fVolSell;
// 							if (  !_isnan(fVolTotal) && fVolTotal != 0.0 )
// 							{
// 								item.strText = Float2String(fVolBuy / fVolTotal * 100, 2, false, false);
// 							}

							item.strText = Float2String(tickex.m_fBuyRate * 100, 2);
						}
						break;
					default:
						ASSERT( 0 );
					}

					if ( item.strText == _T("-") )
					{
						item.strText = _T(" -");
					}
				}
				;
			}
		}
		
	}
	return true;
}

void CIoViewTimeSaleRank::T_Tab::RequestVisibleData(uint8 iPeriod )
{
	CMmiReqPeriodTickEx			reqHis;
	CMmiReqPushPlugInMerchData	req;		// 直接请求推送数据
	
	// 选择多个商品
	
	for ( int i=0; i < m_aMerchs.GetSize() ; i++ )
	{
		CMerch *pMerch = m_aMerchs[i];
		if ( pMerch == NULL )
		{
			continue;
		}
		
		if ( req.m_StrMerchCode.IsEmpty() )
		{
			req.m_iMarketID = pMerch->m_MerchInfo.m_iMarketId;	// 沪深A
			req.m_StrMerchCode = pMerch->m_MerchInfo.m_StrMerchCode;
			req.m_uType = ECSTTickEx;

			reqHis.m_iMarketId = req.m_iMarketID;
			reqHis.m_StrMerchCode = req.m_StrMerchCode;
			reqHis.m_uiPeriod = iPeriod;
		}
		else
		{
			T_ReqPushMerchData	rpmd;
			rpmd.m_iMarket = pMerch->m_MerchInfo.m_iMarketId;
			rpmd.m_StrCode = pMerch->m_MerchInfo.m_StrMerchCode;
			rpmd.m_uType = ECSTTickEx;
			req.m_aReqMore.Add( rpmd );

			CPeriodMerchKey key;
			key.m_iMarketId = rpmd.m_iMarket;
			key.m_StrMerchCode = rpmd.m_StrCode;
			key.m_uiPeriod = iPeriod;
			reqHis.m_aMerchMore.Add(key);
		}
	}
	
	if ( !req.m_StrMerchCode.IsEmpty() && NULL != m_pwndRank )
	{
		m_pwndRank->RequestData(reqHis);

		if(iPeriod == 1)
		{
			req.m_eCommTypePlugIn = ECTPIReqAddPushTickEx;
		}
		else if(iPeriod == 3)
		{
			req.m_eCommTypePlugIn = ECTPIReqAddPushMerchIndexEx3;
		}
		else if(iPeriod == 5)
		{
			req.m_eCommTypePlugIn = ECTPIReqAddPushMerchIndexEx5;
		}
		else if(iPeriod == 10)
		{
			req.m_eCommTypePlugIn = ECTPIReqAddPushMerchIndexEx10;
		}
		m_pwndRank->RequestData(req);
	}
}

void CIoViewTimeSaleRank::T_Tab::RequestSortData(bool32 bForceReq/* = false*/, uint8 iPeriod)
{
	m_MmiRequestSys.m_iMarketId = -1;	// 强制 沪深a
	m_MmiRequestSys.m_iStart = 0;
	m_MmiRequestSys.m_iCount = 10;		// 指定10个
	m_MmiRequestSys.m_bDescSort = TRUE;
	m_MmiRequestSys.m_eReportSortType = GetSortHeader();
	m_MmiRequestSys.m_uiPeriod = iPeriod;
	
	if ( NULL != m_pwndRank )
	{
		m_pwndRank->RequestData(m_MmiRequestSys, bForceReq);
	}
}

void CIoViewTimeSaleRank::T_Tab::OnMerchShowDataUpdate( CMerch *pMerch, bool32 bDrawNow /*= true*/ )
{
	if ( pMerch == NULL )
	{
		return;
	}
	
	bool32 bDataUpdate = false;
	int i = 0;
	for ( i=0; i < m_aMerchs.GetSize() ; i++ )
	{
		if ( pMerch == m_aMerchs[i] )
		{
			bDataUpdate  = true;
			break;
		}
	}

	if ( bDrawNow && bDataUpdate )
	{
		m_wndGrid.RedrawRow(i+m_wndGrid.GetFixedRowCount());
	}
	else if ( !bDrawNow && bDataUpdate )
	{
		m_wndGrid.Refresh();
	}
}

void CIoViewTimeSaleRank::T_Tab::OnMerchSortUpdate( const MerchArray &aMerchsSort, uint8 iPeriod)
{
	if ( aMerchsSort.GetSize() > m_MmiRequestSys.m_iCount && m_MmiRequestSys.m_iCount > 0 )
	{
		MerchArray aMerchsSub;
		aMerchsSub.Append(aMerchsSort);
		aMerchsSub.SetSize(m_MmiRequestSys.m_iCount);
		UpdateTableContent(aMerchsSub);
	}
	else
	{
		UpdateTableContent(aMerchsSort);
	}
	RequestVisibleData(iPeriod);	// 申请可视数据
}

//linhc 20100911选择行改变消息0915修改
void CIoViewTimeSaleRank::OnGridSelRowChanged(UINT nId, NMHDR *pNotifyStruct, LRESULT* pResult )
{

}

void CIoViewTimeSaleRank::GetAttendMarketDataArray( OUT AttendMarketDataArray &aAttends )
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

CSize CIoViewTimeSaleRank::GetVirtualSize()
{
	CRect rcClient;
	GetClientRect(rcClient);
	
	const int32 iCol = 4;
	const int32 iMinCellWidth = 250;	// 每个单元格的最小宽度
	int32 iWidth = rcClient.Width();
	const int32 iRectWidth = max(iMinCellWidth*iCol, iWidth);

	return CSize(iRectWidth, 0);	// 无高度限制
}

bool32 CIoViewTimeSaleRank::ShowOrHideXHorzBar()
{
	bool32 bShow = false;
	CRect rcClient(0,0,0,0);
	CSize sizeVir(0,0);
	if ( IsWindowVisible() )
	{
		GetClientRect(rcClient);
		sizeVir = GetVirtualSize();
		if ( rcClient.Width() < sizeVir.cx )
		{
			bShow = true;
		}
	}
	CXScrollBar *pHorz = GetXSBHorz();
	if ( bShow && NULL != pHorz )
	{
		pHorz->ShowWindow(SW_SHOW);
	}
	else if ( NULL != pHorz )
	{
		pHorz->ShowWindow(SW_HIDE);
		ResetXSBHorz(true);
	}
	return bShow;
}

CXScrollBar		* CIoViewTimeSaleRank::SetXSBHorz( CXScrollBar *pHorz /*= NULL*/ )
{
	CXScrollBar *pOld = m_pCurrentXSBHorz;
	
	if ( IsWindow(pHorz->GetSafeHwnd()) )
	{
		if ( m_pCurrentXSBHorz != pHorz )
		{
			m_pCurrentXSBHorz = pHorz;
			m_pCurrentXSBHorz->SetOwner(this);
			ResetXSBHorz();
			ShowOrHideXHorzBar();
		}
	}
	else
	{
		m_pCurrentXSBHorz = NULL;
	}
	return pOld;
}

void CIoViewTimeSaleRank::ResetXSBHorz(bool32 bResetPos/*=false*/)
{
	CXScrollBar *pHorz = GetXSBHorz();
	if ( NULL != pHorz )
	{
		CRect rcClient(0,0,0,0);
		CSize sizeVir(0,0);
		GetClientRect(rcClient);
		sizeVir = GetVirtualSize();
		int32 iMaxPos = sizeVir.cx-rcClient.Width();
		if ( bResetPos )
		{
			pHorz->SetScrollPos(0, TRUE);
		}
		pHorz->SetScrollRange(0, iMaxPos>0?iMaxPos:0, TRUE);
	}
}

int32 CIoViewTimeSaleRank::GetVirtualLeftPos()
{
	CXScrollBar *pHorz = GetXSBHorz();
	if ( NULL != pHorz )
	{
		return pHorz->GetScrollPos();
	}
	return 0;
}

void CIoViewTimeSaleRank::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	RecalcLayout();
	RedrawWindow();
}

void CIoViewTimeSaleRank::RefreshView( int32 iPeriod )
{
	m_iPeriod = iPeriod;
	RequestSortData(false, m_iPeriod);
}

void CIoViewTimeSaleRank::CreateBtnList()
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

void CIoViewTimeSaleRank::RedrawCycleBtn( CPaintDC *pPainDC )
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

void CIoViewTimeSaleRank::AddNCButton( LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption )
{
	CNCButton btnControl;
	btnControl.CreateButton(lpszCaption, lpRect, this, pImage, nCount, nID);
	btnControl.SetTextBkgColor(RGB(25,25,25), RGB(146,96,0), RGB(25,25,25));
	btnControl.SetTextFrameColor(RGB(100,100,100), RGB(255,255,255), RGB(255,255,255));
	btnControl.SetTextColor(RGB(190,191,191), RGB(255,255,255), RGB(255,255,255));
	m_mapBtnCycleList[nID] = btnControl;
}

int CIoViewTimeSaleRank::TNCButtonHitTest( CPoint point )
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

void CIoViewTimeSaleRank::OnLButtonUp( UINT nFlags, CPoint point )
{
	int iButton = TNCButtonHitTest(point);

	if (INVALID_ID != iButton)
	{
		m_mapBtnCycleList[iButton].LButtonUp();
	}	
}

void CIoViewTimeSaleRank::OnLButtonDown( UINT nFlags, CPoint point )
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

BOOL CIoViewTimeSaleRank::OnCommand( WPARAM wParam, LPARAM lParam )
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
