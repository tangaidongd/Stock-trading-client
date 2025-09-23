#pragma warning(disable:4786)
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
#include "IoViewMainTimeSale.h"
#include "DlgMainTimeSale.h"
#include "LogFunctionTime.h"
#include "PluginFuncRight.h"
#include "ConfigInfo.h"
#include "FontFactory.h"

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
//    "fatal error C1001: INTERNAL COMPILER ERROR"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const int32 KUpdatePushMerchsTimerId						= 100005;				// 每隔 n 秒钟, 重新计算一次当前的推送商品
const int32 KTimerPeriodPushMerchs							= 1000 * 60;

const int32	KTimerIdCheckInitData							= 1001;					// 检查初始化数据是否已经请求到
const int32 KTimerPeriodCheckInitData						= 5000;							

const int32 KIDefaultMonitorCount							= 150;
const int32 KIDefaultMaxMonitorCount						= 3000;

const int32 KICurrentMerchBlockId							= -4;

const char KXmlStrCurTabKey[]								= "CurTab";					// 当前的选择

#define  ID_GRID_CTRL         12366

//////////////////////////////////////////////////////////////////////////
bool32 operator==(const T_RespMainMonitor &resp1, const T_RespMainMonitor &resp2)
{
	return resp1.m_Time == resp2.m_Time
		&& resp1.m_fCounts == resp2.m_fCounts
		&& resp1.m_fPrice == resp2.m_fPrice
		&& resp1.m_uBuy == resp2.m_uBuy;
}

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CIoViewMainTimeSale, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewMainTimeSale, CIoViewBase)
//{{AFX_MSG_MAP(CIoViewMainTimeSale)
ON_WM_PAINT()
ON_WM_CREATE()
ON_WM_SIZE()
ON_WM_TIMER()
ON_WM_LBUTTONDOWN()
ON_WM_CONTEXTMENU()
ON_MESSAGE_VOID(UM_DOINITIALIZE, OnDoInitialize)
//}}AFX_MSG_MAP
ON_NOTIFY(NM_RCLICK,ID_GRID_CTRL,OnGridRButtonDown)
ON_NOTIFY(NM_DBLCLK, ID_GRID_CTRL, OnGridDblClick)
ON_NOTIFY(GVN_COLWIDTHCHANGED, ID_GRID_CTRL, OnGridColWidthChanged)
ON_NOTIFY(GVN_KEYDOWNEND, ID_GRID_CTRL, OnGridKeyDownEnd)
ON_NOTIFY(GVN_KEYUPEND, ID_GRID_CTRL, OnGridKeyUpEnd)
END_MESSAGE_MAP()
///////////////////////////////////////////////////////////////////////////////

// 准备优化 fangz20100514

CIoViewMainTimeSale::CIoViewMainTimeSale()
:CIoViewBase()
{
	m_pParent		= NULL;
	m_rectClient	= CRect(-1,-1,-1,-1);

	m_RectBtn.SetRectEmpty();
	
	m_aUserBlockNames.RemoveAll();

	m_bDrawTitleString = true;

	m_bParentIsDialog = false;

	m_bInitialized =	false;

	m_iCurTab = 0;
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewMainTimeSale::~CIoViewMainTimeSale()
{
	// MainFrame 中快捷键
	CMainFrame* pMainFrame =(CMainFrame*)AfxGetMainWnd();
	if ( NULL != pMainFrame && pMainFrame->m_pKBParent == this )
	{
		pMainFrame->SetHotkeyTarget(NULL);
	}
	
	POSITION pos = m_lstMainTimeSales.GetHeadPosition();
	while ( pos != NULL )
	{
		delete m_lstMainTimeSales.GetNext(pos);
	}
	m_lstMainTimeSales.RemoveAll();
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
void CIoViewMainTimeSale::OnPaint()
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

	CRect rectClient;
	GetClientRect(&rectClient);

	COLORREF clrBk = GetIoViewColor(ESCBackground);
	COLORREF clrText = GetIoViewColor(ESCText);
	COLORREF clrAxis = GetIoViewColor(ESCChartAxisLine);
	COLORREF clrVol = GetIoViewColor(ESCVolume);

// 	if ( !CPluginFuncRight::Instance().IsUserHasRight(CPluginFuncRight::FuncMainTimeSaleMonitor, false) )
// 	{
// 		CMemDC dc(&dcPaint, rectClient);
// 		dc->FillSolidRect(rectClient, clrBk);
// 		dc->SelectObject(GetIoViewFontObject(ESFSmall));
// 		dc->SetTextColor(clrText);
// 		dc->SetBkMode(TRANSPARENT);
// 		CString StrRight = CPluginFuncRight::Instance().GetUserRightName(CPluginFuncRight::FuncMainTimeSaleMonitor);
// 		CString StrShow = _T("不具备[")+StrRight+_T("]权限, ") + CVersionInfo::Instance()->GetPrompt();
// 		CRect rcDraw(rectClient);
// 		rcDraw.top = rectClient.CenterPoint().y;
// 		rcDraw.top -= 15;
// 		rcDraw.InflateRect(-2, 0);
// 		dc->DrawText(StrShow, rcDraw, DT_CENTER |DT_WORDBREAK );
// 	}
// 	else
	{
		CMemDC dc(&dcPaint, m_RectBtn);
		int32 iSaveDC = dc.SaveDC();
		
		dc->FillSolidRect(m_RectBtn, clrBk);
		CFont *pFontSmall = GetIoViewFontObject(ESFSmall);
		dc.SelectObject(pFontSmall);
		dc->SetBkMode(TRANSPARENT);
		dc->SetBkColor(clrBk);
		
		CBrush brhAxis;
		brhAxis.CreateSolidBrush(clrAxis);
		
		for ( int i=0; i < m_aTitleBtns.GetSize() ; i++ )
		{
			CRect rcDraw(m_aTitleBtns[i].m_RectBtn);
			CMDIChildWnd *pChildFrame = DYNAMIC_DOWNCAST(CMDIChildWnd, GetParentFrame());
			if ( 0 == i && pChildFrame != NULL )
			{
				CPoint ptLeftTop = rcDraw.TopLeft();
				MapWindowPoints(pChildFrame, &ptLeftTop, 1);
				if ( ptLeftTop.x > 0 )
				{
					rcDraw.left -= 1;	// 压缩左边一个像素
				}
			}
			rcDraw.InflateRect(0, 1, 1, 0);
			//dc->FrameRect(rcDraw, &brhAxis);
			COLORREF clrDrawText = RGB(160, 160, 160);
			if ( i == m_iCurTab )
			{
				clrDrawText = clrVol;
			}
			dc->SetTextColor(clrDrawText);
			dc->DrawText(m_aTitleBtns[i].m_StrName, m_aTitleBtns[i].m_RectBtn, DT_SINGLELINE |DT_CENTER |DT_VCENTER);
		}
		
		dc->SetTextColor(RGB(160, 160, 160));
		CRect rcTitle = m_RectBtn;
		if ( m_aTitleBtns.GetSize() > 0 )
		{
			rcTitle.left = m_aTitleBtns[m_aTitleBtns.GetSize()-1].m_RectBtn.right;
		}
		if ( m_bDrawTitleString )
		{
			CRect rcDrawText = rcTitle;
			rcDrawText.right -= 5;
			dc->DrawText(_T("主力监控"), rcDrawText, DT_SINGLELINE |DT_RIGHT |DT_VCENTER);
			rcTitle.InflateRect(0, 1, 1, 0);
		}
		else
		{
			rcTitle.InflateRect(0, 1, 0, 0);
		}
		//dc->FrameRect(rcTitle, &brhAxis);
		
		if ( m_bActive )
		{
			//dc->FillSolidRect(m_RectBtn.left+3, m_RectBtn.top+3, 2, 2, clrVol);
		}
		
		// 画边角
		//DrawCorner(dc, rectClient);

		dc.RestoreDC(iSaveDC);
	}
}

BOOL CIoViewMainTimeSale::PreTranslateMessage(MSG* pMsg)
{
	return CIoViewBase::PreTranslateMessage(pMsg);		// IoViewBase能根据ioviewtype处理键
}

int CIoViewMainTimeSale::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( NULL == m_pAbsCenterManager )	// 自己注册
	{
		CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
		SetCenterManager(pApp->m_pDocument->m_pAbsCenterManager);
	}
	
	CIoViewBase::OnCreate(lpCreateStruct);
	
	InitialIoViewFace(this);

	// 创建横滚动条
	m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10203);
	m_XSBVert.SetScrollRange(0, 10);

	m_XSBHorz.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10204);
	m_XSBHorz.SetScrollRange(0, 10);

	// 创建数据表格
	m_GridCtrl.Create(CRect(0, 0, 0, 0), this, ID_GRID_CTRL);
	m_GridCtrl.SetDefCellWidth(50);
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
	m_XSBHorz.SetOwner(&m_GridCtrl);
	m_XSBVert.SetOwner(&m_GridCtrl);
	m_GridCtrl.SetScrollBar(&m_XSBHorz, &m_XSBVert);
	//m_XSBHorz.AddMsgListener(m_hWnd);	// 侦听滚动消息 - 记录滚动消息

	// 设置表头
	m_GridCtrl.SetHeaderSort(FALSE);

	// 显示表格线的风格
	m_GridCtrl.ShowGridLine(FALSE);
	m_GridCtrl.SetSingleRowSelection(TRUE);
	m_GridCtrl.SetShowSelectWhenLoseFocus(FALSE);

	////////////////////////////////////////////////////////////////
// 	m_GridCtrl.SetFixedColumnCount(2);	// 序 名
// 	m_GridCtrl.SetFixedRowCount(1);
	m_GridCtrl.SetColumnCount(4);

	// 设置字体
	LOGFONT *pFontSmall = GetIoViewFont(ESFSmall);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontSmall);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontSmall);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontSmall);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontSmall);

	if ( IsWindow(GetParent()->GetSafeHwnd()) )
	{
		m_bParentIsDialog = (GetParent()->GetStyle() & WS_POPUP) != 0;
	}
	// 初始化各按钮
	T_TabBtn btn;
	int32 iInvalidId = CBlockInfo::GetInvalidId();
	btn.m_iBlockId = iInvalidId;
	btn.m_StrName = _T("所有");   // 沪深A - 约定沪深A发送-1作为板块名，等分类板块出来
	m_aTitleBtns.Add(btn);

	btn.m_iBlockId = iInvalidId;
	btn.m_StrName = _T("300");	  // 沪深300 - id需要寻找
	m_aTitleBtns.Add(btn);

	btn.m_iBlockId = iInvalidId;
	btn.m_StrName = _T("自选");	  // 自选股 - 所有的自选股都在里面了
	m_aTitleBtns.Add(btn);

	btn.m_iBlockId = KICurrentMerchBlockId;
	btn.m_StrName = _T("当前");	  // viewdata中的股票
	btn.m_bNeedReqPrivateData = true;		// 当前需要特殊数据申请 - 每次切换都申请
	m_aTitleBtns.Add(btn);

	if ( !m_bParentIsDialog )
	{
		btn.m_iBlockId = INT_MIN;
		btn.m_StrName = _T("浮出");
		m_aTitleBtns.Add(btn);
	}

	m_iCurTab = 0;
	InitializeBtnId();

	PostMessage(UM_DOINITIALIZE, 0, 0);

	// 这个时候可以初始化tab资源了的，板块在此时应当加载好了，如果没加载应当在do initialize中初始化板块和tab

	//
	SetTimer(KUpdatePushMerchsTimerId, KTimerPeriodPushMerchs, NULL);
//	SetTimer(KTimerIdCheckInitData, KTimerPeriodCheckInitData, NULL);

	CBlockConfig::Instance()->AddListener(this);

	return 0;
}

void CIoViewMainTimeSale::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);

	RecalcLayout();

	Invalidate(TRUE);
}

BOOL CIoViewMainTimeSale::TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return FALSE;
}

void CIoViewMainTimeSale::OnIoViewActive()
{
	m_bActive = IsActiveInFrame();

	//
	//OnVDataForceUpdate();
	RequestLastIgnoredReqData();

	SetChildFrameTitle();
	
	if (NULL != m_GridCtrl.GetSafeHwnd())
	{
		m_GridCtrl.SetFocus();
	}

	InvalidateRect(m_RectBtn);

	UpdateTableAllContent();
}

void CIoViewMainTimeSale::OnIoViewDeactive()
{
	m_bActive = false;
	
	m_GridCtrl.SetFocusCell(-1, -1);
	m_GridCtrl.SetSelectedRange(-1, -1, -1, -1, TRUE);

	InvalidateRect(m_RectBtn);
}

void CIoViewMainTimeSale::OnIoViewTabShow()
{
	RequestInitialData();
}

void CIoViewMainTimeSale::OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult)
{ 
	if ( !m_bParentIsDialog )
	{
		DoShowStdPopupMenu();	
	}
}

void CIoViewMainTimeSale::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW	*pGridView = (NM_GRIDVIEW *)pNotifyStruct;
	if ( NULL != pGridView && pGridView->iRow >= m_GridCtrl.GetFixedRowCount() )
	{
		T_MainTimeSale *pTMS = (T_MainTimeSale *)m_GridCtrl.GetItemData(pGridView->iRow, 0);
		if ( NULL != CheckMTS(pTMS) )
		{
			OnDblClick(pTMS->m_pMerch);
		}
	}
}

void CIoViewMainTimeSale::OnGridColWidthChanged(NMHDR *pNotifyStruct, LRESULT* pResult)
{	
	
}

void CIoViewMainTimeSale::OnGridKeyDownEnd(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	// 向下键按到头了
	
}

void CIoViewMainTimeSale::OnGridKeyUpEnd(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	// 向上键按到头了
	
}

// 通知视图改变关注的商品
void CIoViewMainTimeSale::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch)
		return;
	
	// 修改当前查看的商品
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
}

//
void CIoViewMainTimeSale::OnVDataForceUpdate()
{
	if ( m_lstMainTimeSales.GetCount() < 100 )
	{
		RequestInitialData();		// 有可能向某服务器请求的数据没回来，然后突然断开连接，连接到别的服务器上
	}
	RequestTabSpecialData();

	RequestPushViewData();		// 不请求初始
}


void CIoViewMainTimeSale::OnDestroy()
{	
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if ( NULL != pMainFrm && pMainFrm->m_pKBParent == this )
	{
		pMainFrm->SetHotkeyTarget(NULL);
	}

	CBlockConfig::Instance()->RemoveListener(this);

	CIoViewBase::OnDestroy();
}

void CIoViewMainTimeSale::OnTimer(UINT nIDEvent) 
{
	if(!m_bShowNow)
	{
		return;
	}

	if ( nIDEvent == KUpdatePushMerchsTimerId )
	{
		RequestPushViewData(true);		// 不请求初始 - 定时强制申请推送，否则有可能丢失数据
	}
	else if ( KTimerIdCheckInitData == nIDEvent )
	{
		if ( m_lstMainTimeSales.GetCount() < 100 )
		{
			if(m_bShowNow)
			{
				RequestInitialData();
			}
		}
		else
		{
			KillTimer(KTimerIdCheckInitData);
		}
	}
	
	CIoViewBase::OnTimer(nIDEvent);
}


void CIoViewMainTimeSale::RequestInitialData()
{
	// 无权限不请求该数据
// 	if ( !CPluginFuncRight::Instance().IsUserHasRight(CPluginFuncRight::FuncMainTimeSaleMonitor, false) )
// 	{
// 		return;
// 	}

	CMmiReqMainMonitor reqmain, req300;
	reqmain.m_iBlockId = m_aTitleBtns[0].m_iBlockId;		// 所有id
	reqmain.m_iCount   = KIDefaultMonitorCount+20;								// 多请求
	reqmain.m_StrMerchCode.Empty();
	reqmain.m_aReqMore.RemoveAll();
	
	// 全局数据最多3000个，多余的剔除前面n个数据，因此有可能某些初始化数据不能申请进来
	// 打开期间全部申请所有数据
	// 请求沪深所有最近 100个数据, 沪深300 100个数据 以初始化最开始的数据，
	//	如果切换到自选股，则选取自选股100个商品，每个商品最多10个数据，申请
	//  如果切换到当前，则从viewdata中找所有xml商品，每个商品最多请求20个
	ASSERT( m_aTitleBtns.GetSize() > 2 && m_aTitleBtns[1].m_iBlockId != -1 );
	req300.m_iCount = KIDefaultMonitorCount+20;
	req300.m_iBlockId = m_aTitleBtns[1].m_iBlockId;
	req300.m_StrMerchCode.Empty();
	
	DoRequestViewData(reqmain, true);
	DoRequestViewData(req300, true);

	RequestPushViewData(true);		// 请求推送
}

void CIoViewMainTimeSale::RequestPushViewData(bool32 bForce/* = false*/)
{
	// 无权限不请求该数据
// 	if ( !CPluginFuncRight::Instance().IsUserHasRight(CPluginFuncRight::FuncMainTimeSaleMonitor, false) )
// 	{
// 		return;
// 	}

	int32 iCurTab, iBlockId;
	if ( GetCurTab(iCurTab, iBlockId) )
	{
		// 申请推送数据
		CMmiReqPushPlugInMerchData	req;
		req.m_iMarketID = -1; // m_aTitleBtns[0].m_iBlockId;  // 始终为所有的blockid
		req.m_uType = ECSTMainMonitor;
		DoRequestViewData(req, bForce);
	}
}

void CIoViewMainTimeSale::SetChildFrameTitle()
{
	CString StrTitle;
	StrTitle =  CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;
	
	CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pParentFrame)
	{
		pParentFrame->SetChildFrameTitle(StrTitle);
	}
}

void CIoViewMainTimeSale::OnDblClick(CMerch *pMerch)
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	if ( NULL != pMainFrame )
	{
		pMainFrame->OnShowMerchInChart(pMerch, this);
	}
}

void CIoViewMainTimeSale::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();

	UpdateAllGridRow();

	Invalidate(TRUE);
	m_GridCtrl.Refresh();
}

void CIoViewMainTimeSale::OnIoViewFontChanged()
{
	CIoViewBase::OnIoViewFontChanged();

	// 因为没有表头，所以不能改变列宽
	RecalcLayout();
	
	Invalidate(TRUE);
}

void CIoViewMainTimeSale::SetRowHeightAccordingFont()
{
	for (int i =0; i < m_GridCtrl.GetRowCount(); ++i)
	{
		m_GridCtrl.SetRowHeight(i, 30);
	}
	//m_GridCtrl.AutoSizeRows();
}

void CIoViewMainTimeSale::SetColWidthAccordingFont()
{
	//m_GridCtrl.AutoSizeColumns();
	//m_GridCtrl.ExpandColumnsToFit();
	m_GridCtrl.SetColumnWidth(0, 60);
	m_GridCtrl.SetColumnWidth(1, 80);
	m_GridCtrl.SetColumnWidth(2, 75);
	CRect rcGrid;
	m_GridCtrl.GetClientRect(rcGrid);
	int iWidth = rcGrid.Width() - 215;
	if ( iWidth > 0 )
	{
		m_GridCtrl.SetColumnWidth(3, iWidth);
	}
}

void CIoViewMainTimeSale::UpdateTableAllContent()
{
	// 保存focus cell
	CCellID	cellFocus = m_GridCtrl.GetFocusCell();
	CCellRange cellRange = m_GridCtrl.GetSelectedCellRange();
	m_GridCtrl.DeleteNonFixedRows();	

	// 插入商品信息

	// 裁掉多余的数据
	while ( m_lstMainTimeSales.GetCount() > KIDefaultMaxMonitorCount )
	{
		delete m_lstMainTimeSales.RemoveHead();
	}

	m_GridCtrl.InsertRowBatchBegin();

	POSITION pos = m_lstMainTimeSales.GetTailPosition();
	int32 iRowCount = 0;
	while ( pos != NULL )
	{
		T_MainTimeSale *pMTS = m_lstMainTimeSales.GetPrev(pos);
		ASSERT( NULL != pMTS );
		if ( IsMerchInCurTab(pMTS->m_pMerch) )
		{
			if ( iRowCount >= KIDefaultMonitorCount )		// 只插入一定量的数据
			{
				break;
			}
			int32 iNewRow = m_GridCtrl.InsertRowBatchForOne(_T("   "), iRowCount ==0? -1 : 0);
			UpdateGridRow(iNewRow, *pMTS);
			iRowCount++;
		}
		//else
		//{
		//	TRACE(_T("%d-%s-%s\r\n"), pMTS->m_pMerch->m_MerchInfo.m_iMarketId, pMTS->m_pMerch->m_MerchInfo.m_StrMerchCode, pMTS->m_pMerch->m_MerchInfo.m_StrMerchCnName);
		//}
	}
	m_GridCtrl.InsertRowBatchEnd();

	if ( m_GridCtrl.IsValid(cellFocus) )
	{
		m_GridCtrl.SetFocusCell(cellFocus); // 验证是否还要select
	}
	if ( m_GridCtrl.IsValid(cellRange) )
	{
		m_GridCtrl.SetSelectedRange(cellRange);
	}

	if ( m_GridCtrl.GetRowCount() > 0 )
	{
		m_GridCtrl.EnsureVisible(m_GridCtrl.GetRowCount()-1, 0);
	}

	//SetRowHeightAccordingFont();
	SetColWidthAccordingFont();
	m_GridCtrl.Refresh();
}

void CIoViewMainTimeSale::OnDoInitialize()
{
	RequestInitialData();

	RequestTabSpecialData();

	m_bInitialized = true;
}

void CIoViewMainTimeSale::RecalcLayout()
{
// 	if ( !CPluginFuncRight::Instance().IsUserHasRight(CPluginFuncRight::FuncMainTimeSaleMonitor, false) )
// 	{
// 		// 不具备权限，全部都空
// 		m_RectBtn.SetRectEmpty();
// 		for ( int i=0; i < m_aTitleBtns.GetSize() ; i++ )
// 		{
// 			m_aTitleBtns[i].m_RectBtn.SetRectEmpty();
// 		}
// 		m_GridCtrl.MoveWindow(CRect(0,0,0,0));
// 		return;
// 	}
	CRect rcClient;
	GetClientRect(rcClient);

	CClientDC dc(this);
	CFont *pFontOld = dc.SelectObject(GetIoViewFontObject(ESFSmall));
	CSize sizeText = dc.GetTextExtent(_T("行高"));
	dc.SelectObject(pFontOld);
	m_RectBtn = rcClient;
	m_RectBtn.bottom = m_RectBtn.top + sizeText.cy + 14;

	const int32 iBtnWidth = sizeText.cx + 10;
	for ( int i=0; i < m_aTitleBtns.GetSize() ; i++ )
	{
		CRect &rcBtn = m_aTitleBtns[i].m_RectBtn;
		rcBtn.left	 = m_RectBtn.left + i*iBtnWidth;
		rcBtn.right  = rcBtn.left + iBtnWidth;
		rcBtn.top	 = m_RectBtn.top;
		rcBtn.bottom = m_RectBtn.bottom;
	}
	
	rcClient.top = m_RectBtn.bottom;
//	CRect rcOld(0,0,0,0);
//	m_GridCtrl.GetClientRect(rcOld);
	m_GridCtrl.MoveWindow(rcClient);
	//if ( rcOld.Width() < rcClient.Width() )
	{
		SetColWidthAccordingFont();
		if ( m_GridCtrl.GetRowCount() > 0 )
		{
			m_GridCtrl.EnsureBottomRightCell(m_GridCtrl.GetRowCount()-1, m_GridCtrl.GetColumnCount()-1);	// 最左侧不能滚动
			m_GridCtrl.EnsureVisible(m_GridCtrl.GetRowCount()-1, 0);
		}
	}
	m_GridCtrl.Refresh();
}

void CIoViewMainTimeSale::OpenTab( int32 iTabPress )
{
	if ( iTabPress >=0 && iTabPress < m_aTitleBtns.GetSize() )
	{
		if ( m_aTitleBtns[iTabPress].m_iBlockId != INT_MIN )
		{
			m_iCurTab = iTabPress;
		}
	}

	RequestTabSpecialData();		// 请求tab可能需要的初始数据

	UpdateTableAllContent();

	Invalidate();

	// 填充当前商品数据 - 应该先申请该block数据，等数据回来后处理
}

void CIoViewMainTimeSale::OnLButtonDown( UINT nFlags, CPoint point )
{
	int i=0;
	for ( i = 0; i < m_aTitleBtns.GetSize(); i++ )	// 最后一个为浮出
	{
		if ( m_aTitleBtns[i].m_RectBtn.PtInRect(point) )
		{
			if ( m_aTitleBtns[i].m_iBlockId != INT_MIN )
			{
				OpenTab(i);
			}
			else
			{
				// 浮出
				CDlgMainTimeSale::ShowPopupMainTimeSale(this);
			}
			break;
		}
	}
	
	CIoViewBase::OnLButtonDown(nFlags, point);
}

bool32 CIoViewMainTimeSale::FromXml( TiXmlElement *pTiXmlElement )
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

	pcAttrValue = pTiXmlElement->Attribute(KXmlStrCurTabKey);
	if ( NULL != pcAttrValue )
	{
		OpenTab(atoi(pcAttrValue));
	}
	
	return true;
}

CString CIoViewMainTimeSale::ToXml()
{	
	//
	CString StrThis;
	
	StrThis.Format( L"<%s %s=\"%s\" %s=\"%s\" %s=\"%d\" ", 
		CString(GetXmlElementValue()).GetBuffer(),
		CString(GetXmlElementAttrIoViewType()).GetBuffer(),	CIoViewManager::GetIoViewString(this).GetBuffer(),
		CString(GetXmlElementAttrShowTabName()).GetBuffer(), m_StrTabShowName.GetBuffer(),
		CString(KXmlStrCurTabKey).GetBuffer(), m_iCurTab
		);
	
	CString StrFace;
	StrFace  = SaveColorsToXml();
	StrFace += SaveFontsToXml();
	
	StrThis += StrFace;
	StrThis += L">\n";
	
	
	StrThis += L"</";
	StrThis += CString(GetXmlElementValue());
	StrThis += L">\n";
	return StrThis;
}

CString CIoViewMainTimeSale::GetDefaultXML()
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

void CIoViewMainTimeSale::OnConfigInitialize( E_InitializeNotifyType eInitializeType )
{
	InitializeBtnId();
}

void CIoViewMainTimeSale::OnBlockConfigChange( int32 iBlockId, E_BlockNotifyType eNotifyType )
{
	// 暂时不处理	
	RequestTabSpecialData();		// 用户板块更新 - 有可能添加了新的数据等，不过在数据超过一定值后，实际以前的数据都不能添加了
	// 需要数据重插入以反应变化 以后用虚表格直接Invalidate
	//Invalidate(TRUE);

	// 更新显示
	for ( int i=0; i < m_GridCtrl.GetRowCount() ; i++ )
	{
		T_MainTimeSale *pMTS = (T_MainTimeSale *)m_GridCtrl.GetItemData(i, 0);
		if ( NULL != pMTS )
		{
			UpdateGridRow(i, *pMTS);
		}
	}
}

void CIoViewMainTimeSale::InitializeBtnId()
{
	// 沪深A 300 查看id是否有变化
	ASSERT( CBlockConfig::Instance()->IsInitialized() );
	CBlockConfig::BlockArray aBlocks;
	
	// 沪深A股特殊约定-1
	{
		m_aTitleBtns[0].m_iBlockId = -1;
	}
	CBlockConfig::Instance()->GetBlocksByName(_T("沪深300"), aBlocks);	// 会不会叫这个名字？？
	ASSERT( aBlocks.GetSize() == 1 );
	if ( aBlocks.GetSize() > 0 )
	{
		m_aTitleBtns[1].m_iBlockId = aBlocks[0]->m_blockInfo.m_iBlockId;
	}
	if ( CBlockConfig::Instance()->GetUserBlockCollection() != NULL )
	{
		m_aTitleBtns[2].m_iBlockId = CBlockConfig::Instance()->GetUserBlockCollection()->m_iBlockCollectionId;	// 自定义实际上是板块集合
	}
}

bool32 CIoViewMainTimeSale::GetCurTab( OUT int32 &iCurTab, OUT int32 &iBlockId )
{
	if ( m_iCurTab >=0 && m_iCurTab <= 3 )		// 所有 300 自选 当前
	{
		iCurTab = m_iCurTab;
		iBlockId = m_aTitleBtns[m_iCurTab].m_iBlockId;
		return true;
	}
	return false;
}

bool32 CIoViewMainTimeSale::IsMerchInCurTab( CMerch *pMerch )
{
	if ( pMerch == NULL )
	{
		return false;
	}

	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	if ( NULL == pMainFrame )
	{
		return false;
	}


	int32 iCurTab, iBlockId;
	if ( GetCurTab(iCurTab, iBlockId) )
	{
		MerchArray aMerchs;		// 简单处理
		switch (iCurTab)
		{
		case BtnAll:		// 所有 - 可以直接返回true
			return true;

		case Btn300:		// 300
			{
				CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(iBlockId);
				if ( NULL != pBlock )
				{
					aMerchs.Copy(pBlock->m_blockInfo.m_aSubMerchs);
				}
			}
			break;
		case BtnUser:		// 自定义
			{
				CBlockConfig::BlockArray aBlocks;
				CBlockConfig::Instance()->GetUserBlocks(aBlocks);
				for ( int i=0; i < aBlocks.GetSize() ; i++ )
				{
					aMerchs.Append(aBlocks[i]->m_blockInfo.m_aSubMerchs);
				}
			}
			break;
		case BtnCur:		// 当前 - 选择viewdata中所有的ioview吧
			{
				if ( NULL != m_pAbsCenterManager )
				{
					for ( int i=0; i < pMainFrame->m_IoViewsPtr.GetSize() ; i++ )
					{
						CIoViewBase *pIoView = pMainFrame->m_IoViewsPtr[i];
						if ( NULL != pIoView )
						{
							CMerch *pMerchXml = pIoView->m_pMerchXml;
							if ( NULL != pMerchXml )
							{
								aMerchs.Add(pMerchXml);
							}
						}
					}
				}
			}
			break;
		default:
			ASSERT( 0 );
			// NULL
		}
		for ( int i=0; i < aMerchs.GetSize() ; i++ )
		{
			if ( aMerchs[i] == pMerch )
			{
				return true;
			}
		}
	}
	return false;
}

void CIoViewMainTimeSale::OnMainTimeSaleResp( const CMmiRespMainMonitor *pResp )
{
	// 时间排序
	if ( NULL == pResp || NULL == m_pAbsCenterManager)
	{
		return;
	}	
	
	// 不管什么数据，只要是没有的，全部插入 - 需要提高效率
	if ( pResp->m_eCommTypePlugIn == ECTPIRespMainMonitor )	// 历史数据
	{
		bool32 bUpdated = false;
		for ( mapMainMonitor::const_iterator it = pResp->m_mapMainMonitorData.begin() ; it != pResp->m_mapMainMonitorData.end(); it++ )
		{
			CMerch *pMerch;
			if ( !m_pAbsCenterManager->GetMerchManager().FindMerch(it->m_MerchKey.m_StrMerchCode, it->m_MerchKey.m_iMarketId,  pMerch) )
			{
				continue;
			}
			for ( MonitorArray::const_iterator itSub=it->m_aMainMonitor.begin(); itSub != it->m_aMainMonitor.end(); itSub++ )
			{
				bUpdated = InsertNewMainTimeSale(pMerch, *itSub, false) || bUpdated;
				//CTime Time(itSub->m_Time);
				//TRACE(_T("   %02d:%02d:%02d\n"), Time.GetHour(), Time.GetMinute(), Time.GetSecond());
			}

			//TRACE(_T("[%d]%d-%s-%s\r\n"), pResp->m_mapMainMonitorData.size(), it->m_MerchKey.m_iMarketId, it->m_MerchKey.m_StrMerchCode, pMerch->m_MerchInfo.m_StrMerchCnName);
		}

		if ( bUpdated )
		{
			UpdateTableAllContent();	// 显示数据

			RequestPushViewData(false);	// 请求推送
		}
	}
	else if ( pResp->m_eCommTypePlugIn == ECTPIRespAddPushMainMonitor )	// 推送数据
	{
		bool32 bUpdate = false;
		CCellID cellFocus = m_GridCtrl.GetFocusCell();
		T_MainTimeSale *pMTSSel = NULL;
		if ( m_GridCtrl.IsValid(cellFocus) )
		{
			pMTSSel = (T_MainTimeSale *)m_GridCtrl.GetItemData(cellFocus.row, 0);
		}

		for ( mapMainMonitor::const_iterator it = pResp->m_mapMainMonitorData.begin() ; it != pResp->m_mapMainMonitorData.end(); it++ )
		{
			CMerch *pMerch;
			if ( !m_pAbsCenterManager->GetMerchManager().FindMerch(it->m_MerchKey.m_StrMerchCode, it->m_MerchKey.m_iMarketId,  pMerch) )
			{
				continue;
			}
			for ( MonitorArray::const_iterator itSub=it->m_aMainMonitor.begin(); itSub != it->m_aMainMonitor.end(); itSub++ )
			{
				bUpdate = InsertNewMainTimeSale(pMerch, *itSub, true) || bUpdate;
				//TRACE(_T("MainMonitor time: %d\r\n"), itSub->m_Time);
			}
		}
		
		DeleteRedundantHead(true);		// 清除多余的信息 
		
		if ( pMTSSel != NULL )
		{
			for ( int i=0; i < m_GridCtrl.GetRowCount() ; i++ )
			{
				if ( pMTSSel == (T_MainTimeSale *)m_GridCtrl.GetItemData(i, 0) )
				{
					m_GridCtrl.SetFocusCell(i, cellFocus.col);
					m_GridCtrl.SetSelectedRange(i, 0, i, m_GridCtrl.GetColumnCount()-1, TRUE, TRUE);
				}
			}
		}
		m_GridCtrl.EnsureBottomRightCell(m_GridCtrl.GetRowCount()-1, m_GridCtrl.GetColumnCount()-1);
		m_GridCtrl.EnsureVisible(m_GridCtrl.GetRowCount()-1, 0);
	}
}

//lint --e{429}
bool32 CIoViewMainTimeSale::InsertNewMainTimeSale(CMerch *pMerch,  const T_RespMainMonitor &mainTM , bool32 bAddToGrid/* = false*/)
{
	// 按照时间序，找到对应的位置 - 慢慢找:)
	T_MainTimeSale *pMainTimeSale = new T_MainTimeSale;
	pMainTimeSale->m_pMerch = pMerch;
	pMainTimeSale->m_mainMonitor = mainTM;
	POSITION pos = m_lstMainTimeSales.GetHeadPosition();
	int i = 0;
	for (   ; pos != NULL ; m_lstMainTimeSales.GetNext(pos), i++ )
	{
		const T_MainTimeSale &mts = *m_lstMainTimeSales.GetAt(pos);
		
		if ( mts.m_mainMonitor.m_Time < mainTM.m_Time )		// 原来的数据比现在的小
		{
			continue;
		}
		else if ( mts.m_mainMonitor.m_Time == mainTM.m_Time )	// 等
		{
			if ( mts.m_pMerch == pMerch && mts.m_mainMonitor == mainTM )	// 重复数据不添加，时间相等则插入后面
			{
				DEL(pMainTimeSale);		// 释放				
				return false;		// 相同的
			}
		}
		else	// 大于
		{
			// 如果数量已经最大，且是前面应当抛弃的数据则抛弃
			if ( 0 == i && m_lstMainTimeSales.GetCount() >= KIDefaultMaxMonitorCount )
			{
				DEL(pMainTimeSale);
				return false;
			}

			m_lstMainTimeSales.InsertBefore(pos, pMainTimeSale);
			if ( bAddToGrid /*&& !m_bInitialized*/ )
			{
				// 如果数据是一条一条过来，应当不出现前面数据的
				InsertGridRow(i, *pMainTimeSale);
			}
			return true;
		}
	}

	// 添加到最后
	m_lstMainTimeSales.AddTail(pMainTimeSale);
	if ( bAddToGrid )
	{
		InsertGridRow(i, *pMainTimeSale);
	}

	// 裁掉多余的数据
	return true;
}

int32 CIoViewMainTimeSale::InsertGridRow( int /*iRow*/, const T_MainTimeSale &mts , bool32 bDrawRow/* =false*/ )
{
	// 根据当前的选择内容，在适当位置添加 - 应该只添加到结尾的
	if ( IsMerchInCurTab(mts.m_pMerch) )
	{
		int i = 0;
		for ( i=0; i < m_GridCtrl.GetRowCount() ; i++ )
		{
			T_MainTimeSale *pMTS = (T_MainTimeSale *)m_GridCtrl.GetItemData(i, 0);
			ASSERT( NULL != CheckMTS(pMTS) );
			if ( NULL != pMTS )
			{
				if ( pMTS->m_mainMonitor.m_Time > mts.m_mainMonitor.m_Time )
				{
					break;
				}
			}
		}
		if ( i >= m_GridCtrl.GetRowCount() )
		{
			i = -1;
		}
//		CCellID cellFocus = m_GridCtrl.GetFocusCell();
		int32 iNewRow = m_GridCtrl.InsertRow(_T("   "), i);
		UpdateGridRow(iNewRow, mts);
		m_GridCtrl.EnsureVisible(m_GridCtrl.GetRowCount()-1, 0);
// 		if ( m_GridCtrl.IsValid(cellFocus) )
// 		{
// 			m_GridCtrl.SetFocusCell(cellFocus);
// 			//m_GridCtrl.SetSelectedRange(cellFocus.row, 0, cellFocus.row, m_GridCtrl.GetColumnCount()-1, TRUE, TRUE);
// 			m_GridCtrl.RedrawRow(cellFocus.row);
// 		}
		if ( bDrawRow )
		{
			m_GridCtrl.RedrawRow(iNewRow);
		}
		return iNewRow;
	}
	return -1;
}

void CIoViewMainTimeSale::DeleteRedundantHead( bool32 bDeleteInGrid /*= false*/ )
{
	bool32 bDeleted = false;
	//CCellID cellFocus = m_GridCtrl.GetFocusCell();
	while ( m_lstMainTimeSales.GetCount() > KIDefaultMaxMonitorCount )	// 总的数据量超标
	{
		T_MainTimeSale *pMTS = m_lstMainTimeSales.RemoveHead();
		for ( int i=0; i < m_GridCtrl.GetRowCount() ; i++ )
		{
			if ( m_GridCtrl.GetItemData(i, 0) == (LPARAM)pMTS )
			{
				m_GridCtrl.DeleteRow(i);
				bDeleted = true;
				break;
			}
		}
		delete pMTS;
	}

	if ( bDeleteInGrid )
	{
		while ( m_GridCtrl.GetRowCount() > KIDefaultMonitorCount )	// 表格数据量超
		{
			m_GridCtrl.DeleteRow(0);
			bDeleted = true;
		}
	}

	if ( bDeleted && bDeleteInGrid )
	{
// 		if ( m_GridCtrl.IsValid(cellFocus) )
// 		{
// 			m_GridCtrl.SetFocusCell(cellFocus);
// 		}
		m_GridCtrl.Refresh();
	}
}

void CIoViewMainTimeSale::UpdateGridRow( int iRow, const T_MainTimeSale &mts )
{
	if ( !m_GridCtrl.IsValid(iRow, 0) )
	{
		return;
	}

	LOGFONT fontChinese;
	memset(&fontChinese, 0, sizeof(fontChinese));
	_tcscpy(fontChinese.lfFaceName, gFontFactory.GetExistFontName(_T("Arial")));///
	fontChinese.lfHeight  = -15;

	tm t = {0};
	CGmtTime	GmtTime(mts.m_mainMonitor.m_Time);
	GmtTime.GetLocalTm(&t);
	CString StrTime;
	COLORREF clr = GetIoViewColor(ESCRise);
	if ( mts.m_mainMonitor.m_uBuy != 0 )
	{
		clr = GetIoViewColor(ESCFall);
	}
	COLORREF clrMerch = clr;
	T_Block *pBlock = CUserBlockManager::Instance()->GetBlock(mts.m_pMerch);
	if ( NULL != pBlock && COLORNOTCOUSTOM != pBlock->m_clrBlock )
	{
		clrMerch = pBlock->m_clrBlock;
	}
	StrTime.Format(_T("%02d:%02d"), t.tm_hour, t.tm_min);
	m_GridCtrl.SetItemText(iRow, 0, StrTime);
	m_GridCtrl.SetItemFont(iRow, 0, &fontChinese);
	m_GridCtrl.SetItemFgColour(iRow, 0, clrMerch);
	m_GridCtrl.SetItemTextPadding(iRow, 0, CRect(5,0,0,0));

	ASSERT( iRow != -1 );
	m_GridCtrl.SetItemText(iRow, 1, mts.m_pMerch->m_MerchInfo.m_StrMerchCnName);
	m_GridCtrl.SetItemFont(iRow, 1, &fontChinese);
	m_GridCtrl.SetItemFgColour(iRow, 1, clrMerch);
	
	CString StrFlag;
	StrFlag = mts.m_mainMonitor.m_uBuy != 0 ? _T("主力卖出") : _T("主力买入");
	m_GridCtrl.SetItemText(iRow, 2, StrFlag);
	m_GridCtrl.SetItemFont(iRow, 2, &fontChinese);
	m_GridCtrl.SetItemFgColour(iRow, 2, clr);
	
	CString StrPrice;
	StrPrice.Format(_T("%0.2f/%0.0f"), mts.m_mainMonitor.m_fPrice, mts.m_mainMonitor.m_fCounts / 100);
	m_GridCtrl.SetItemText(iRow, 3, StrPrice);
	m_GridCtrl.SetItemFont(iRow, 3, &fontChinese);
	m_GridCtrl.SetItemFgColour(iRow, 3, clr);

	m_GridCtrl.SetItemData(iRow, 0, (LPARAM)&mts);


	for (int j =0; j < m_GridCtrl.GetColumnCount(); ++j)
	{				
		CGridCellBase* pCell = m_GridCtrl.GetCell(iRow, j);
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

void CIoViewMainTimeSale::RequestData( CMmiCommBase &req )
{
	//if ( IsWindowVisible() && NULL != m_pAbsCenterManager )
	{
		//m_pAbsCenterManager->RequestViewData(&req);
		DoRequestViewData(req);
	}
}

CIoViewMainTimeSale::T_MainTimeSale  * CIoViewMainTimeSale::CheckMTS( T_MainTimeSale *pMTS ) const
{
	POSITION pos = m_lstMainTimeSales.GetHeadPosition();
	while ( pos != NULL )
	{
		if ( pMTS == m_lstMainTimeSales.GetNext(pos) )
		{
			return pMTS;
		}
	}
	return NULL;
}

void CIoViewMainTimeSale::LockRedraw()
{
	if( m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = true;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
	::SendMessage(m_GridCtrl.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
}

void CIoViewMainTimeSale::UnLockRedraw()
{
	if ( !m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = false;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
	::SendMessage(m_GridCtrl.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);

	Invalidate();
}

void CIoViewMainTimeSale::OnVDataPluginResp( const CMmiCommBase *pResp )
{
	if ( NULL == pResp )
	{
		return;
	}
	
	if ( pResp->m_eCommType != ECTRespPlugIn )
	{
		return;
	}

	// 视图不可见的时候不刷新界面	--chenfj
	if(!IsNowCanRefreshUI())
	{
		return;
	}

	// 无权限不接收该数据
// 	if ( !CPluginFuncRight::Instance().IsUserHasRight(CPluginFuncRight::FuncMainTimeSaleMonitor, false) )
// 	{
// 		return;
// 	}
	
	const CMmiCommBasePlugIn *pRespPlugin = (CMmiCommBasePlugIn *)pResp;
	if ( pRespPlugin->m_eCommTypePlugIn == ECTPIRespMainMonitor
		|| pRespPlugin->m_eCommTypePlugIn == ECTPIRespAddPushMainMonitor )
	{
		OnMainTimeSaleResp((const CMmiRespMainMonitor *)pResp);
	}
}

bool32 CIoViewMainTimeSale::GetCurBlockMerchs( OUT MerchArray &aMerchs )
{
	int32 iTab, iBlockId;
	if ( GetCurTab(iTab, iBlockId) )
	{
		return GetBlockMerchsByTabIndex(iTab, aMerchs);
	}
	return false;
}

bool32 CIoViewMainTimeSale::GetBlockMerchsByTabIndex( int32 iTab, OUT MerchArray &aMerchs )
{
	aMerchs.RemoveAll();

	CBlockLikeMarket *pBlockDef = CBlockConfig::Instance()->GetDefaultMarketClassBlock();
	if ( NULL == pBlockDef )	// 无沪深a，免谈
	{
		return false;
	}
	switch (iTab)
	{
	case BtnAll:		// 所有 - 沪深A - 不需要提供这种服务，so，不提供
		ASSERT( 0 );
		break;
	case Btn300:		// 沪深300 - 不需要 不提供
		ASSERT( 0 );
		break;
	case BtnUser:		// 自选 - 
		{
			CBlockConfig::BlockArray aBlocks;
			CBlockConfig::Instance()->GetUserBlocks(aBlocks);
			aMerchs.SetSize(0, 100);
			int i = 0;
			for ( i=0; i < aBlocks.GetSize() ; i++ )
			{
				aMerchs.Append(aBlocks[i]->m_blockInfo.m_aSubMerchs);
			}
			// 过滤掉沪深A股中没有的商品
			for ( i = aMerchs.GetSize()-1; i >= 0 ; i-- )	// 效率很低，待改进
			{
				if ( !pBlockDef->IsMerchInBlock(aMerchs[i]) )
				{
					aMerchs.RemoveAt(i);	// 不是沪深A的，清除
				}
			}
		}
		break;
	case BtnCur:		// 当前 - 从view Data找
		{
			CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
			if ( NULL == pMainFrame )
			{
				break;
			}

			for (int  i=0; i < pMainFrame->m_IoViewsPtr.GetSize() ; i++ )
			{
				if ( pMainFrame->m_IoViewsPtr[i] != NULL && pMainFrame->m_IoViewsPtr[i]->m_pMerchXml != NULL )
				{
					aMerchs.Add(pMainFrame->m_IoViewsPtr[i]->m_pMerchXml);
				}
			}
			// 过滤掉沪深A股中没有的商品
			for (int  i = aMerchs.GetSize()-1; i >= 0 ; i-- )	// 效率很低，待改进
			{
				if ( !pBlockDef->IsMerchInBlock(aMerchs[i]) )
				{
					aMerchs.RemoveAt(i);	// 不是沪深A的，清除
				}
			}
		}
		break;
	}
	return aMerchs.GetSize() > 0;
}

void CIoViewMainTimeSale::RequestUserStockData()
{
	int32 iTab, iBlockId;
	MerchArray aMerchs;
	CBlockLikeMarket *pBlockDef = CBlockConfig::Instance()->GetDefaultMarketClassBlock();
	if ( NULL == pBlockDef )
	{
		return;		// 没板块定义，不请求了
	}
	const int32 iMaxCount = 100;

	if ( GetCurTab(iTab, iBlockId) && BtnUser == iTab && GetCurBlockMerchs(aMerchs) && aMerchs.GetSize() > 0 )	// 自选
	{
		CMmiReqMainMonitor reqUser;
		for ( int i=0; i < aMerchs.GetSize() && i <= iMaxCount ; i++ )
		{
			if ( reqUser.m_StrMerchCode.IsEmpty() )
			{
				reqUser.m_iCount = 10;	// 自定义最多最近10个
				reqUser.m_StrMerchCode = aMerchs[i]->m_MerchInfo.m_StrMerchCode;
				reqUser.m_iBlockId = aMerchs[i]->m_MerchInfo.m_iMarketId;
			}
			else
			{
				T_ReqMainMonitor mm;
				mm.m_iBlockId = aMerchs[i]->m_MerchInfo.m_iMarketId;
				mm.m_iCount = 10;
				mm.m_StrMerchCode = aMerchs[i]->m_MerchInfo.m_StrMerchCode;
				reqUser.m_aReqMore.Add(mm);
			}
		}
		RequestData(reqUser);
	}
}

void CIoViewMainTimeSale::RequestCurrentStockData()
{
	int32 iTab, iBlockId;
	MerchArray aMerchs;
	CBlockLikeMarket *pBlockDef = CBlockConfig::Instance()->GetDefaultMarketClassBlock();
	if ( NULL == pBlockDef )
	{
		return;		// 没板块定义，不请求了
	}
	const int32 iMaxCount = 50;

	if ( GetCurTab(iTab, iBlockId) && BtnCur == iTab && GetCurBlockMerchs(aMerchs) && aMerchs.GetSize() > 0 )	// 自选
	{
		CMmiReqMainMonitor reqCur;
		reqCur.m_iBlockId = pBlockDef->m_blockInfo.m_iBlockId;
		for ( int i=0; i < aMerchs.GetSize() && i <= iMaxCount ; i++ )
		{
			if ( reqCur.m_StrMerchCode.IsEmpty() )
			{
				reqCur.m_iCount = 20;	// 最多最近20个
				reqCur.m_StrMerchCode = aMerchs[i]->m_MerchInfo.m_StrMerchCode;
				reqCur.m_iBlockId = aMerchs[i]->m_MerchInfo.m_iMarketId;
			}
			else
			{
				T_ReqMainMonitor mm;
				mm.m_iBlockId = aMerchs[i]->m_MerchInfo.m_iMarketId;
				mm.m_iCount = 20;
				mm.m_StrMerchCode = aMerchs[i]->m_MerchInfo.m_StrMerchCode;
				reqCur.m_aReqMore.Add(mm);
			}
		}
		RequestData(reqCur);
	}
}

void CIoViewMainTimeSale::RequestTabSpecialData()
{
	int32 iTab, iBlockId;
	if ( GetCurTab(iTab, iBlockId) )
	{
		switch (iTab)
		{
		case BtnUser:
			RequestUserStockData();
			break;
		case BtnCur:
			RequestCurrentStockData();
			break;
		}
	}
}

CMerch * CIoViewMainTimeSale::GetMerchXml()
{
	CCellRange rangeSel = m_GridCtrl.GetSelectedCellRange();
	T_MainTimeSale *pMTS = NULL;
	if ( rangeSel.IsValid() && (pMTS = (T_MainTimeSale *)m_GridCtrl.GetItemData(rangeSel.GetMinRow(), 0)) != NULL )
	{
		return pMTS->m_pMerch;
	}
	return CIoViewBase::GetMerchXml();
}

bool32 CIoViewMainTimeSale::GetStdMenuEnable( MSG* pMsg )
{
	//return !m_bParentIsDialog;	// 对话框不允许菜单
	return false;
}

void CIoViewMainTimeSale::GetAttendMarketDataArray( OUT AttendMarketDataArray &aAttends )
{
	aAttends.RemoveAll();
	CBlockLikeMarket *pBlock = CBlockConfig::Instance()->GetDefaultMarketClassBlock();;
	T_AttendMarketData data;
	if ( NULL != pBlock )
	{
		// 不管当前是哪个按钮，关心所有沪深a的市场
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

void CIoViewMainTimeSale::UpdateAllGridRow()
{
	const int32 iCount = m_GridCtrl.GetRowCount();
	for ( int32 i=0; i < iCount ; i++ )
	{
		T_MainTimeSale *pMTS = (T_MainTimeSale *)m_GridCtrl.GetItemData(i, 0);
		if ( NULL != pMTS )
		{
			UpdateGridRow(i, *pMTS);
		}
	}
}

void CIoViewMainTimeSale::OnContextMenu( CWnd* pWnd, CPoint pos )
{
	if ( m_RectBtn.IsRectEmpty() )
	{
		DoShowStdPopupMenu();	// 此时为特殊情况，grid没有显示
	}
}

//////////////////////////////////////////////////////////////////////////

CIoViewMainTimeSale::T_TabBtn::T_TabBtn( const T_TabBtn &tab )
{
	m_RectBtn = tab.m_RectBtn;
	m_StrName = tab.m_StrName;
	m_iBlockId = tab.m_iBlockId;
	m_bNeedReqPrivateData = tab.m_bNeedReqPrivateData;
}

const CIoViewMainTimeSale::T_TabBtn & CIoViewMainTimeSale::T_TabBtn::operator=( const T_TabBtn &tab )
{
	if ( &tab == this )
	{
		return tab;
	}

	m_RectBtn = tab.m_RectBtn;
	m_StrName = tab.m_StrName;
	m_iBlockId = tab.m_iBlockId;
	m_bNeedReqPrivateData = tab.m_bNeedReqPrivateData;

	return *this;
}
