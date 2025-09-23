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
#include "IoViewStarry.h"
#include "IoViewBlockReport.h"
#include "IoViewReportRank.h"
#include "LogFunctionTime.h"
#include "DlgTimeSaleStatistic.h"
#include "MarkManager.h"

#include <set>
using std::set;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
//    "fatal error C1001: INTERNAL COMPILER ERROR"

const char* KStrViewBlockReportTabId			= "CURCLASSID";
const char* KStrViewBlockReportSortHeader		= "SORTHEADER";
const char* KStrViewBlockReportSortHeaderDesc	= "SORTHEADER_DESC";
const char* KStrViewBlockReportShowTitle		= "SHOWTITLE";
const char* KStrViewBlockReportUpdateKline		= "UPDATEKLINE";    //切换商品是否更新K线数据
const char* KStrViewBlockReportUpdateTrend		= "UPDATETREND";    //切换商品是否更新分时数据
const char* KStrViewBlockReportUpdateReport		= "UPDATEREPORT";   //切换商品是否更新报价表数据
const char* KStrViewReportShowGridVertScrool11	= "SHOWGRIDVERTSCROOL";

const int32 KRequestSortDataTimerId				= 100004;			// 排行请求，无推送，频繁发
const int32 KRequestSortDataTimerPeriod			= 1000 * 60;		// 由于板块数据计算并不频繁，so慢点发

const int32 KUpdatePushMerchsTimerId			= 100005;			// 每隔 5 秒钟, 重新计算一次当前的推送商品
const int32 KTimerPeriodPushMerchs				= 1000 * 60;

const int32 KWatchTimeChangeTimerId				= 100006;			// 每隔 n 秒钟, 重新查看时间是否跨天
const int32 KWatchTimeChangeTimerPeriod			= 60*1000;					

const int32 KChangeVisibleBlocksTimerId			= 100007;
const int32 KChangeVisibleBlocksTimerPeriod		= 260;

const int32 KStartChooseBlockTimerId			= 100008;
const int32 KStartChooseBlockTimerPeriod		= 1000;


#define  ID_GRID_CTRL         10205

	
IMPLEMENT_DYNCREATE(CIoViewBlockReport, CIoViewBase)
CIoViewBlockReport *CIoViewBlockReport::m_spThis = NULL;

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewBlockReport, CIoViewBase)
//{{AFX_MSG_MAP(CIoViewBlockReport)
ON_WM_PAINT()
ON_WM_CREATE()
ON_WM_SIZE()
ON_WM_TIMER()
ON_WM_HSCROLL()
ON_WM_VSCROLL()
ON_WM_CONTEXTMENU()
ON_NOTIFY(TCN_SELCHANGE, 0x9999, OnSelChange)
ON_MESSAGE_VOID(UM_BLOCK_DOINITIALIZE, OnDoBlockReportInitialize)
ON_MESSAGE(UM_POS_CHANGED, OnScrollPosChange)
//}}AFX_MSG_MAP
ON_NOTIFY(NM_RCLICK,ID_GRID_CTRL,OnGridRButtonDown)
ON_NOTIFY(NM_DBLCLK, ID_GRID_CTRL, OnGridDblClick)
ON_NOTIFY(NM_CLICK, ID_GRID_CTRL, OnGridDblClick)
ON_NOTIFY(GVN_GETDISPINFO, ID_GRID_CTRL, OnGridGetDispInfo)
ON_NOTIFY(GVN_SELCHANGED, ID_GRID_CTRL,OnGridSelRowChanged) 
ON_NOTIFY(GVN_ODCACHEHINT, ID_GRID_CTRL, OnGridCacheHint)
END_MESSAGE_MAP()
///////////////////////////////////////////////////////////////////////////////

// 准备优化 fangz20100514

CIoViewBlockReport::CIoViewBlockReport()
:CIoViewBase()
{
	m_pParent		= NULL;
	m_rectClient	= CRect(-1,-1,-1,-1);
	m_RectTitle.SetRectEmpty();

	m_iSortColumn		= -1;
	m_bRequestViewSort	= false;
	
	m_aUserBlockNames.RemoveAll();

	m_iBlockClassId = T_BlockReportTab::BCAll;
	
	m_bBlockReportInitialized = false;

	m_iMaxGridVisibleRow = 0;

	m_tmDisplay.tm_year = 0;

	m_iPreTab = m_iCurTab = -1;

	m_iSortTypeFromXml = 0;
	m_iSortColumnFromXml = -1;
	m_bShowTitle = false;
	m_bUpdateKline = true;
	m_bUpdateTrend = true;
	m_bUpdateReport = true;


	m_bIsShowGridVertScorll = false;
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewBlockReport::~CIoViewBlockReport()
{
// 	if ( NULL != m_pSubject )
// 	{
// 		m_pSubject->DelObserver(this);
// 	}
	
	// MainFrame 中快捷键
	CMainFrame* pMainFrame =(CMainFrame*)AfxGetMainWnd();
	if ( NULL != pMainFrame && pMainFrame->m_pKBParent == this )
	{
		pMainFrame->SetHotkeyTarget(NULL);
	}	
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
void CIoViewBlockReport::OnPaint()
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
	COLORREF clrAxis = GetIoViewColor(ESCChartAxisLine);
	COLORREF clrVol = GetIoViewColor(ESCVolume2);
	COLORREF clrBlock = GetIoViewColor(ESCRise);

	if ( clrBk == CFaceScheme::Instance()->GetSysColor(ESCBackground) )
	{
		//clrBk = RGB(44, 0, 0);
	}
	else
	{
		// 灰度
		CColorStep step;
		step.InitColorRange(clrBk, 32.0f, 32.0f, 32.0f);
		clrBk = step.GetColor(1);
	}

	CMemDC dc(&dcPaint, m_RectTitle);
	int32 iSaveDC = dc.SaveDC();

	CFont *pFontNormal = GetIoViewFontObject(ESFNormal);
	dc.SelectObject(pFontNormal);
	dc.FillSolidRect(m_RectTitle, clrBk);
	dc.SetBkColor(clrBk);
	dc.SetBkMode(TRANSPARENT);
	
	CRect rcDraw(m_RectTitle);
	rcDraw.left += 10;

	CString StrBlock, StrTime;
	CString StrWeekDay(_T("日一二三四五六"));
	T_BlockReportTab tab;
	if ( GetTabInfoByIndex(m_GuiTabWnd.GetCurtab(), tab) )
	{
		if ( tab.m_StrName.GetLength() > 2 && tab.m_StrName.Right(2) == _T("板块") )
		{
			StrBlock.Format(_T("热门板块分析 -- %s"), tab.m_StrName.GetBuffer());
		}
		else
		{
			StrBlock.Format(_T("热门板块分析 -- %s板块"), tab.m_StrName.GetBuffer());
		}
	}
	else
	{
		StrBlock = _T("热门板块分析");
	}
	
	tm &tmNowLocal = m_tmDisplay;
	StrTime.Format(_T("今日: %04d-%02d-%02d, %c"), tmNowLocal.tm_year+1900, tmNowLocal.tm_mon+1, tmNowLocal.tm_mday, StrWeekDay[tmNowLocal.tm_wday%StrWeekDay.GetLength()]);
	if (m_bShowTitle)
	{
		dc.SetTextColor(clrBlock);
		dc.DrawText(StrBlock, rcDraw, DT_LEFT |DT_SINGLELINE |DT_VCENTER);
		rcDraw.left = dc.GetTextExtent(StrBlock).cx + 20;

		if ( rcDraw.Width() > 0 )
		{
			dc.SetTextColor(clrVol);
			dc.DrawText(StrTime, rcDraw, DT_LEFT |DT_SINGLELINE |DT_VCENTER);
		}

		dc.FillSolidRect(rectClient.left, rcDraw.bottom-1, rectClient.Width(), 1, clrAxis);
		if ( m_bActive )
		{
			dc.FillSolidRect(3, 2, 2, 2, clrVol);
		}
	}

	CRect RectTab;
	m_GuiTabWnd.GetWindowRect(&RectTab);
	ScreenToClient(RectTab);	

	CRect RectDraw;
	RectDraw = rectClient;

	if ( 0 == m_aTabInfomations.GetSize() )
	{
		m_GuiTabWnd.ShowWindow(SW_HIDE);
		dc.FillSolidRect(&RectTab, GetIoViewColor(ESCBackground));
	}
	else
	{
		m_GuiTabWnd.ShowWindow(SW_SHOW);
	}

	RectDraw.top   = RectTab.top;
	RectDraw.left  = RectTab.right;	
	dc.FillSolidRect(&RectDraw, GetIoViewColor(ESCBackground));

	dc.MoveTo(RectTab.left, RectTab.top );
	dc.LineTo(RectDraw.right, RectTab.top );

	dc.MoveTo(RectTab.left, RectTab.bottom -1);
	dc.LineTo(RectDraw.right, RectTab.bottom -1);

	dc.RestoreDC(iSaveDC);
}

void CIoViewBlockReport::OnMouseWheel(short zDelta)
{
	
}

BOOL CIoViewBlockReport::PreTranslateMessage(MSG* pMsg)
{
	//if ( WM_KEYDOWN == pMsg->message )
	//{
	//	if ( VK_ESCAPE == pMsg->wParam )
	//	{
	//		// 单独页面不处理esc响应
	//		// 返回
	//		OnEsc();
	//		return TRUE;
	//	}			 
	//}
	//else if ( WM_MOUSEWHEEL == pMsg->message )
	//{
	//	// 拦截滚轮
 //		short zDelta = HIWORD(pMsg->wParam);
 //		zDelta /= 120;
 //
 //		// 自己处理
 //		OnMouseWheel(zDelta);
	//	return TRUE;
	//}

	return CIoViewBase::PreTranslateMessage(pMsg);
}

int CIoViewBlockReport::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CIoViewBase::OnCreate(lpCreateStruct);
	
	InitialIoViewFace(this);

	//创建Tab 条
	m_GuiTabWnd.Create(WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,0x9999);
	//m_GuiTabWnd.SetBkGround(false,GetIoViewColor(ESCBackground),0,0);
	m_GuiTabWnd.SetBkGround(false,RGB(42,42,50),0,0);
	m_GuiTabWnd.SetTabTextColor(RGB(182,183,185), RGB(228,229,230));
	m_GuiTabWnd.SetTabBkColor(RGB(42,42,50), RGB(230,70,70));

	// m_GuiTabWnd.SetBkGround(true,GetIoViewColor(ESCBackground),IDB_GUITAB_NORMAL,IDB_GUITAB_SELECTED);
	// m_GuiTabWnd.SetImageList(IDB_TAB, 16, 16, 0x0000ff);
	m_GuiTabWnd.SetUserCB(this);
	m_GuiTabWnd.SetALingTabs(CGuiTabWnd::ALN_BOTTOM);
	/*LOGFONT *pFont = GetIoViewFont(ESFNormal);
	if (pFont)
	{
		pFont->lfHeight = -13;
		m_GuiTabWnd.SetFontByLF(*pFont);
	}*/
	

	// 创建横滚动条
	m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10203);
	m_XSBVert.SetScrollRange(0, 10);

	m_XSBHorz.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10204);
	m_XSBHorz.SetScrollRange(0, 10);

	/*m_XSBVert.SetScrollBarLeftArrowH(-1);
	m_XSBVert.SetScrollBarRightArrowH(-1);
	m_XSBVert.SetBorderColor(GetIoViewColor(ESCBackground));

	m_XSBHorz.SetScrollBarLeftArrowH(-1);
	m_XSBHorz.SetScrollBarRightArrowH(-1);
	m_XSBHorz.SetBorderColor(GetIoViewColor(ESCBackground));*/

	// 创建数据表格
	m_GridCtrl.Create(CRect(0, 0, 0, 0), this, ID_GRID_CTRL);
// 	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetBackClr(CLR_DEFAULT);
// 	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetTextClr(CLR_DEFAULT);
	m_GridCtrl.SetTextColor(CLR_DEFAULT);
	m_GridCtrl.SetFixedTextColor(CLR_DEFAULT);
	m_GridCtrl.SetTextBkColor(CLR_DEFAULT);
	m_GridCtrl.SetBkColor(CLR_DEFAULT);
	m_GridCtrl.SetFixedBkColor(CLR_DEFAULT);
	m_GridCtrl.SetAutoHideFragmentaryCell(false);
	
	m_GridCtrl.SetDefCellWidth(60);
	m_GridCtrl.EnableBlink(FALSE);
	m_GridCtrl.SetVirtualMode(TRUE);
	m_GridCtrl.SetVirtualCompare(CompareRow);
	m_GridCtrl.SetDefaultCellType(RUNTIME_CLASS(CGridCellNormalSys));
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrl.EnablePolygonCorner(false);
	// 设置表格图标
	// m_ImageList.Create(MAKEINTRESOURCE(IDB_GRID_REPORT), 16, 1, RGB(255,255,255));



	// 设置相互之间的关联
	m_XSBHorz.SetOwner(&m_GridCtrl);
	m_XSBVert.SetOwner(&m_GridCtrl);
	m_GridCtrl.SetScrollBar(&m_XSBHorz, &m_XSBVert);
	m_XSBHorz.AddMsgListener(m_hWnd);	// 侦听滚动消息 - 记录滚动消息
	m_XSBVert.AddMsgListener(m_hWnd);	// 侦听滚动消息 - 即时发送数据请求

	// 设置表头
	m_GridCtrl.SetHeaderSort(FALSE);
	m_GridCtrl.SetUserCB(this);

	// 显示表格线的风格
	m_GridCtrl.ShowGridLine(false); 
	m_GridCtrl.SetInitSortAscending(FALSE);

	////////////////////////////////////////////////////////////////
	m_GridCtrl.SetFixedColumnCount(2);	// 序 名
	m_GridCtrl.SetFixedRowCount(1);	
	// 设置字体
	LOGFONT *pFontNormal = GetIoViewFont(ESFSmall);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);

	// 表头固定，可以初始化表头了
	UpdateTableHeader();

	//pCell->SetDrawParentActiveParams(m_iActiveXpos, m_iActiveYpos, m_ColorActive);


	PostMessage(UM_BLOCK_DOINITIALIZE, 0, 0);

	// 这个时候可以初始化tab资源了的，板块在此时应当加载好了，如果没加载应当在do initialize中初始化板块和tab
	InitializeTabs();	

	//
	SetTimer(KUpdatePushMerchsTimerId, KTimerPeriodPushMerchs, NULL);

	SetTimer(KWatchTimeChangeTimerId, KWatchTimeChangeTimerPeriod , NULL);
	SetTimer(KStartChooseBlockTimerId, KStartChooseBlockTimerPeriod , NULL);
	CGmtTime timeNow = m_pAbsCenterManager->GetServerTime();
	timeNow.GetLocalTm(&m_tmDisplay);
	WatchDayChange(false);

	// 板块侦听
	CBlockConfig::Instance()->AddListener(this);

	return 0;
}

void CIoViewBlockReport::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);

	RecalcLayout();
}

bool32	CIoViewBlockReport::IsPointInGrid()
{
	CPoint pt(0,0);
	GetCursorPos(&pt);
	CRect rt;
	m_GridCtrl.GetWindowRect(&rt);	
	if (rt.PtInRect(pt))
	{
		return TRUE;
	}
	return FALSE;
}


BOOL CIoViewBlockReport::TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	
	return FALSE;
}

void CIoViewBlockReport::OnIoViewActive()
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
	
	//
	int32 iTab;

	Invalidate(TRUE);

	if ( !TabIsValid(iTab) )
	{
		return;
	}	
}

void CIoViewBlockReport::OnIoViewDeactive()
{
	m_bActive = false;
	
	Invalidate(TRUE);
}

void CIoViewBlockReport::OnSelChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if ( m_bBlockReportInitialized )	// 初始化完毕
	{
		TabChanged();
		InvalidateRect(m_RectTitle);
	}

	*pResult = 1;
}

//////////////////////////////////////////////////////////////////////////////////////////
//从WorkSpace的xml中读入Blocks,并不确保Blocks的内容在物理板块. Logic xml. user xml中存在
bool32 CIoViewBlockReport::FromXml(TiXmlElement * pTiXmlElement)
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

	// 设置字体
	LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);
	SetRowHeightAccordingFont();

	pcAttrValue = pTiXmlElement->Attribute(KStrViewBlockReportTabId);
	if ( NULL != pcAttrValue )
	{
		m_iBlockClassId = atoi(pcAttrValue);
	}

	pcAttrValue = pTiXmlElement->Attribute(KStrViewBlockReportSortHeader);
	if ( NULL != pcAttrValue )
	{
		m_iSortColumnFromXml = atol(pcAttrValue);	
	}

	pcAttrValue = pTiXmlElement->Attribute(KStrViewBlockReportSortHeaderDesc);
	if ( NULL != pcAttrValue && m_iSortColumnFromXml != -1 )	// 不需要排序就不需要这个字段了
	{
		m_iSortTypeFromXml = atol(pcAttrValue);		// 取最后一次的状态
	}

	pcAttrValue = pTiXmlElement->Attribute(KStrViewBlockReportShowTitle);
	if ( NULL != pcAttrValue  )	// 是否显示标题文字
	{
		m_bShowTitle = atoi(pcAttrValue) != 0;		
	}

	pcAttrValue = pTiXmlElement->Attribute(KStrViewBlockReportUpdateKline);
	if ( NULL != pcAttrValue  )	// 是否更新K线
	{
		m_bUpdateKline = atoi(pcAttrValue) != 0;		
	}

	pcAttrValue = pTiXmlElement->Attribute(KStrViewBlockReportUpdateTrend);
	if ( NULL != pcAttrValue  )	// 是否更新分时
	{
		m_bUpdateTrend = atoi(pcAttrValue) != 0;		
	}

	pcAttrValue = pTiXmlElement->Attribute(KStrViewBlockReportUpdateReport);
	if ( NULL != pcAttrValue  )	// 是否更新报价表
	{
		m_bUpdateReport = atoi(pcAttrValue) != 0;		
	}

	// 是否显示垂直滚动条
	pcAttrValue = pTiXmlElement->Attribute(KStrViewReportShowGridVertScrool11);
	if (pcAttrValue != NULL)
	{
		m_bIsShowGridVertScorll = atoi(pcAttrValue);
	}



	// 此时Tab信息是初始化好了的
	int i=0;
	ASSERT( m_aTabInfomations.GetSize() > 0 );
	for ( i=0; i < m_aTabInfomations.GetSize() ; i++ )
	{
		if ( m_aTabInfomations[i].m_iBlockCollectionId == m_iBlockClassId )
		{
			break;
		}
	}
	if ( i >= m_aTabInfomations.GetSize() )
	{
		m_iBlockClassId = T_BlockReportTab::BCAll;
	}

	SetTabByClassId(m_iBlockClassId);		// 这个时候其实是不允许tab切换的

	m_GuiTabWnd.SetBkGround(false,GetIoViewColor(ESCBackground),0,0);

	return true;
}

CString CIoViewBlockReport::ToXml()
{	
	//
	CString StrThis;

	if ( !m_bRequestViewSort )
	{
		ASSERT( m_iSortColumn == m_GridCtrl.GetSortColumn() || -1 == m_iSortColumn);	// 清空这个, 拥有本地排序
		//m_iSortColumn = -1;
	}

	StrThis.Format( L"<%s %s=\"%s\" %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%s\" %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%d\"", 
					CString(GetXmlElementValue()).GetBuffer(),
					CString(GetXmlElementAttrIoViewType()).GetBuffer(),	CIoViewManager::GetIoViewString(this).GetBuffer(),
					CString(KStrViewBlockReportTabId).GetBuffer(),	m_iBlockClassId,
					CString(KStrViewReportShowGridVertScrool11).GetBuffer(), m_bIsShowGridVertScorll,
					CString(KStrViewBlockReportSortHeader).GetBuffer(),	m_iSortColumn,
					CString(KStrViewBlockReportSortHeaderDesc).GetBuffer(),	m_MmiRequestBlockSort.m_bDescSort ? _T("1") : _T("-1"),
					CString(KStrViewBlockReportShowTitle), m_bShowTitle,
					CString(KStrViewBlockReportUpdateKline), m_bUpdateKline,
					CString(KStrViewBlockReportUpdateTrend), m_bUpdateTrend,
					CString(KStrViewBlockReportUpdateReport), m_bUpdateReport
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

CString CIoViewBlockReport::GetDefaultXML()
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

void CIoViewBlockReport::OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult)
{ 
	CPoint pos(0, 0);
	GetCursorPos(&pos);
	DoTrackMenu(pos);
}
void CIoViewBlockReport::OnGridSelRowChanged( NMHDR *pNotifyStruct, LRESULT* pResult )
{
	
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	
	// 通知商品发生了改变
	if (IsPointInGrid())
	{				
		CCellRange cellRange = m_GridCtrl.GetSelectedCellRange();	

		int32 iRow = pItem->iRow;
		int32 iCol = 1;
	
		if ( iRow >= m_GridCtrl.GetFixedRowCount() && iRow < m_GridCtrl.GetRowCount() )
		{
			int32 iBlockId = (int32)m_GridCtrl.GetItemData(iRow, 0);
			OnDblClick(iBlockId);
		}
	}		
}
void CIoViewBlockReport::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW	*pGridView = (NM_GRIDVIEW *)pNotifyStruct;
	if(NULL != pGridView )
	{
		if ( pGridView->iRow >= m_GridCtrl.GetFixedRowCount() )
		{
			int32 iBlockId = (int32)m_GridCtrl.GetItemData(pGridView->iRow, 0);
			//OnDblClick(iBlockId);
		}
	}
}

// 通知视图改变关注的商品
void CIoViewBlockReport::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch)
		return;
	
	// 修改当前查看的商品
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
}

//
void CIoViewBlockReport::OnVDataForceUpdate()
{
	if ( m_bRequestViewSort )
	{
		RequestSortData();
	}
	else
	{
		RequestViewDataCurrentVisibleRow();
	}
}

void CIoViewBlockReport::OnVDataRealtimePriceUpdate(IN CMerch *pMerch)
{
		
}

// 当物理市场排行发生变化
// 当物理市场排行发生变化
void CIoViewBlockReport::OnVDataReportUpdate(int32 iMarketId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount,  const CArray<CMerch*, CMerch*> &aMerchs)
{	
	
}

void CIoViewBlockReport::OnVDataGridHeaderChanged(E_ReportType eReportType)
{
	
}

bool32 CIoViewBlockReport::TabIsValid(int32& iTab)
{
	iTab = m_GuiTabWnd.GetCurtab();
	if ( iTab >=0 && iTab < m_aTabInfomations.GetSize() )
	{
		return true;
	}

	return false;
}

void CIoViewBlockReport::OnFixedRowClickCB(CCellID& cell)
{
	DoFixedRowClickCB(cell);
}

void CIoViewBlockReport::OnFixedColumnClickCB(CCellID& cell)
{

}

void CIoViewBlockReport::OnHScrollEnd()
{
	
}

void CIoViewBlockReport::OnVScrollEnd()
{
	
}

void CIoViewBlockReport::OnCtrlMove( int32 x, int32 y )
{
	
}

bool32 CIoViewBlockReport::OnEditEndCB ( int32 iRow,int32 iCol, const CString &StrOld, INOUT CString &StrNew )
{
	return false;
}

void CIoViewBlockReport::OnDestroy()
{	
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if ( NULL != pMainFrm && pMainFrm->m_pKBParent == this )
	{
		pMainFrm->SetHotkeyTarget(NULL);
	}

	CIoViewBase::OnDestroy();
}

void CIoViewBlockReport::OnTimer(UINT nIDEvent) 
{
	if(!m_bShowNow)
	{
		return;
	}

	if ( nIDEvent == KUpdatePushMerchsTimerId )
	{
		if ( m_bRequestViewSort )
		{
			//RequestSortData(); // 交给排行专业id
		}
		else
		{
			RequestViewDataCurrentVisibleRow();		
		}
	}
	else if ( nIDEvent == KRequestSortDataTimerId )
	{
		if ( m_bRequestViewSort )
		{
			RequestSortData();
		}
	}
	else if ( nIDEvent == KWatchTimeChangeTimerId )
	{
		WatchDayChange(true);
	}
	else if ( nIDEvent == KChangeVisibleBlocksTimerId )
	{
		RequestViewDataCurrentVisibleRow();
	}
	else if(nIDEvent == KStartChooseBlockTimerId)
	{
		KillTimer(KStartChooseBlockTimerId);

		if ( m_GridCtrl.GetRowCount() > m_GridCtrl.GetFixedRowCount() )
		{
			int32 iBlockId = (int32)m_GridCtrl.GetItemData(1, 0);
			OnDblClick(iBlockId);
		}
		else 
		{
			SetTimer(KStartChooseBlockTimerId, KStartChooseBlockTimerPeriod , NULL);
		}
	}
	CIoViewBase::OnTimer(nIDEvent);
}

void CIoViewBlockReport::RequestSortData()
{
	if ( !m_bRequestViewSort )
	{
		return;
	}

	// 请求数据 - -1表示所有板块
	m_MmiRequestBlockSort.m_iMarketId = -1; // m_iBlockClassId; // 发送沪深A股板块排行
	m_MmiRequestBlockSort.m_iStart = 0;
	m_MmiRequestBlockSort.m_iCount = -1;	// m_aShowBlockIds.GetSize();/*min(m_iMaxGridVisibleRow, m_aShowBlockIds.GetSize());*/
	RequestData(m_MmiRequestBlockSort);
}

void CIoViewBlockReport::RequestViewDataCurrentVisibleRow()
{
	// 全数据申请
	CBlockCollection *pCollection = GetCurrentCollection();
	if ( NULL != pCollection && pCollection->IsUserBlockCollection() )
	{
		return;	// 自选板块没有数据可请求的
	}
	CMmiReqLogicBlock req;
	req.m_iLogicBlockId = -1;
	RequestData(req);

// 	IdArray aBlockIds;
// 	int i;
// 	aBlockIds.Copy(m_aShowBlockIds);
// 	
// 	bool32 bFirst = true;
// 	m_mmiReqBlockData.m_aReqMore.RemoveAll();
// 	for ( i=0; i < aBlockIds.GetSize() ; i++ )
// 	{
// 		CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(aBlockIds[i]);
// 		if ( pBlock != NULL )
// 		{
// 			if ( bFirst )
// 			{
// 				m_mmiReqBlockData.m_iBlockID = -1;//aBlockIds[i];
// 				bFirst = false;
// 			}
// 			else
// 			{
// 				T_ReqPushBlockData d;
// 				d.m_iBlockId = aBlockIds[i];
// 				d.m_eType = ECSTBlock;
// 				m_mmiReqBlockData.m_aReqMore.Add(d);
// 			}
// 		}
// 		else
// 		{
// 			//ASSERT( 0 );
// 		}
// 	}
// 	ASSERT( (!m_bRequestViewSort && m_mmiReqBlockData.m_aReqMore.GetSize() >= aBlockIds.GetSize()-1) || m_bRequestViewSort );
// 	
// 	RequestData(m_mmiReqBlockData);
	
	KillTimer(KChangeVisibleBlocksTimerId);
	
}

void CIoViewBlockReport::OnUserBlockUpdate( CSubjectUserBlock::E_UserBlockUpdate eUpdateType )
{
	
}

void CIoViewBlockReport::SetChildFrameTitle()
{
	CString StrTitle;
	StrTitle =  CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;
	
	CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pParentFrame)
	{
		pParentFrame->SetChildFrameTitle(StrTitle);
	}
}

bool32 CIoViewBlockReport::GetStdMenuEnable( MSG* pMsg )
{
	return false;
}

void CIoViewBlockReport::OnDblClick(int32 iBlockId)
{
	CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(iBlockId);
	if ( NULL != pBlock )
	{
		CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
		if ( NULL == pMainFrame )
		{
			return;
		}
		CMerch *pMerch = GetBlockMerchXml();

		// 更换报价表，星空图，单个商品视图为领涨股票
		T_BlockDesc	desc;
		desc.m_StrBlockName = pBlock->m_blockInfo.m_StrBlockName;
		desc.m_aMerchs.Copy(pBlock->m_blockInfo.m_aSubMerchs);
		if ( CBlockConfig::Instance()->IsUserBlock(iBlockId) )	// 只支持这两种板块
		{
			desc.m_eType = T_BlockDesc::EBTUser;
		}
		else
		{
			desc.m_eType = T_BlockDesc::EBTBlockLogical;
			desc.m_iMarketId = pBlock->m_blockInfo.m_iBlockId;	// 板块id作为市场id
		}

		//pMainFrame->OnSpecifyBlock(desc);

		//
		const int32 iGroupId = GetIoViewGroupId();

		// 星空 - 存在才换
		for ( int32 i=0; i < pMainFrame->m_IoViewsPtr.GetSize() ; i++ )
		{
			CIoViewBase *pIoView = pMainFrame->m_IoViewsPtr[i];
			if ( NULL == pIoView )
			{
				continue;
			}

			CIoViewStarry *pStarry = DYNAMIC_DOWNCAST(CIoViewStarry, pIoView);
			if ( NULL != pStarry && pIoView->GetIoViewGroupId() == iGroupId )	// 星空图
			{
				pStarry->SetNewBlock(iBlockId)	;	// 可见重新申请数据
			}
		}
	
		// 单个
		if ( NULL != m_pAbsCenterManager && NULL != pMerch && iGroupId != -1 )	// 非独立视图
		{
			CMDIChildWnd* pActiveMainFrm =(CMDIChildWnd*)pMainFrame->GetActiveFrame();		
			if(m_bUpdateKline)
			{
				//	切换K线图
				CIoViewChart* pKlineBase =DYNAMIC_DOWNCAST(CIoViewChart, pMainFrame->FindIoViewInFrame(ID_PIC_KLINE,  pActiveMainFrm, false, true, GetIoViewGroupId()));
				if (NULL != pKlineBase)
				{
					bool bChange = true;
					if (pKlineBase->HaveLimitMerch())		//	如果视图已经限定了商品。需要查询是否是允许切换的商品
					{
						if (!pKlineBase->IsLimitMerch(pMerch))
						{
							bChange = false;
						}
					}

					if (bChange)
					{
						pMainFrame->OnViewMerchChanged(pKlineBase, pMerch);
						pKlineBase->ForceUpdateVisibleIoView();
					}				
					
				}
			}

			if(m_bUpdateTrend)
			{
				//	切换分时图商品
				CIoViewChart* pTrendBase = DYNAMIC_DOWNCAST(CIoViewChart,pMainFrame->FindIoViewInFrame(ID_PIC_TREND,  pActiveMainFrm, false, true, GetIoViewGroupId()));
				if (NULL != pTrendBase)
				{
					bool bChange = true;
					if (pTrendBase->HaveLimitMerch())		//	如果视图已经限定了商品。需要查询是否是允许切换的商品
					{
						if (!pTrendBase->IsLimitMerch(pMerch))
						{
							bChange = false;
						}
					}

					if (bChange)
					{
						pMainFrame->OnViewMerchChanged(pTrendBase, pMerch);
						pTrendBase->ForceUpdateVisibleIoView();
					}					
					
				}
			}

			if(m_bUpdateReport)
			{
				//	重新加载板块数据
				CIoViewBase* pReportBase = pMainFrame->FindIoViewInFrame(ID_PIC_REPORT,  pActiveMainFrm, false, true, GetIoViewGroupId());
				if (NULL != pReportBase)
				{
					CIoViewReport * pReport = DYNAMIC_DOWNCAST(CIoViewReport, pReportBase);
					bool bIsUserblock = pReport->IsShowUserBlock();
					if (NULL != pReport && !bIsUserblock)
					{
						pReport->SetTabByBlockName(desc.m_StrBlockName);
						pReport->AddBlock(desc);
						pReport->ReCreateTabWnd();							
					}				
				}
			}
		}
	}	
}

void CIoViewBlockReport::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();
	m_GuiTabWnd.SetBkGround(false,GetIoViewColor(ESCBackground),0,0);
	Invalidate();
	m_GridCtrl.Refresh();
}

void CIoViewBlockReport::OnIoViewFontChanged()
{
	CIoViewBase::OnIoViewFontChanged();

	LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);	

	RecalcLayout();
	Invalidate();
	m_GridCtrl.Refresh();
}

void CIoViewBlockReport::OnRButtonDown2( CPoint pt, int32 iTab )
{
	
}

bool32 CIoViewBlockReport::OnLButtonDown2( CPoint pt, int32 iTab )
{
	return false;
}

bool32 CIoViewBlockReport::BlindLButtonBeforeRButtonDown( int32 iTab )
{
	return false;
}

void CIoViewBlockReport::SetTab(int32 iCurTab, int32 iSortCol/*=-1*/, int32 iSortType/*=0*/)
{
	if ( iCurTab < 0 || iCurTab >= m_aTabInfomations.GetSize() )
	{
		return;
	}
	
	int32 iCur = m_GuiTabWnd.GetCurtab();
	m_GuiTabWnd.SetCurtab(iCurTab);
	if ( iCur == iCurTab )	// 此时tab不会发送消息，自己调用
	{
		TabChanged();
	}
	if ( iSortCol >= 0 )
	{
		SortColumn(iSortCol, iSortType);
	}
}

void CIoViewBlockReport::TabChanged()
{
	// 此时Tab已经切换了, m_iCurTab已经是pre tab
	int32 iTab = m_GuiTabWnd.GetCurtab();
	T_BlockReportTab tab;
	if ( GetTabInfoByIndex(iTab, tab) )
	{
		m_iPreTab = m_iCurTab;
		if ( m_iPreTab != -1 )	// 保存有效的tab信息
		{
			CCellID cellId = m_GridCtrl.GetTopleftNonFixedCell();
			if ( cellId.IsValid() )
			{
				UpdateTabInfo(m_iPreTab, cellId.row, cellId.col);
			}
		}
		m_iCurTab = iTab;
		ASSERT( tab.IsValid() );

		m_iBlockClassId = tab.m_iBlockCollectionId;

		// 取消所有排序信息
		m_bRequestViewSort = false;
		m_iSortColumn = -1;
		//m_MmiRequestSys = ...

		m_aShowBlockIds.RemoveAll();

		// 所有block都是显示数据
		if ( m_iBlockClassId == T_BlockReportTab::BCAll )
		{
			// 取当前Tab上所有的合集 - BCALL总是在0位置
			for ( int i=1; i < m_aTabInfomations.GetSize() ; i++ )
			{
				IdArray aIds;
				CBlockCollection *pCollection = CBlockConfig::Instance()->GetBlockCollectionById(m_aTabInfomations[i].m_iBlockCollectionId);
				ASSERT( NULL != pCollection );
				if ( NULL != pCollection )
				{
					if ( pCollection->m_aBlocks.GetSize() <= 0 )
					{
						continue;	// 
					}
					int32 iBlockType = pCollection->m_aBlocks[0]->m_blockInfo.m_iType;
					if ( CBlockInfo::typeNormalBlock != iBlockType && CBlockInfo::typeUserBlock != iBlockType )
					{
						continue; // 不支持的类型，只考虑一个
					}
					pCollection->GetValidBlockIdArray(aIds);
					m_aShowBlockIds.Append(aIds);
				}
			}
		}
		else
		{
			CBlockCollection *pCollection = CBlockConfig::Instance()->GetBlockCollectionById(m_iBlockClassId);
			ASSERT( NULL != pCollection );
			if ( NULL != pCollection )
			{
				pCollection->GetValidBlockIdArray(m_aShowBlockIds);
			}
		}

		m_aTabInfomations[iTab].m_aSubBlockIds.Copy(m_aShowBlockIds);	// 保存原始板块数据于Tab中
		
		UpdateTableAllContent();

		//m_GridCtrl.EnsureVisible(tab.m_iStartRow, tab.m_iColLeftVisible);
		int32 iStartRow, iStartCol;
		iStartRow = tab.m_iStartRow;
		iStartCol = tab.m_iColLeftVisible;
		if ( tab.m_iStartRow < m_GridCtrl.GetFixedRowCount() || tab.m_iColLeftVisible < m_GridCtrl.GetFixedColumnCount() || !m_GridCtrl.IsValid(iStartRow, iStartCol) )
		{
			iStartRow = m_GridCtrl.GetFixedRowCount();
			iStartCol = m_GridCtrl.GetFixedColumnCount();
		}
		m_GridCtrl.EnsureTopLeftCell(iStartRow, iStartCol);
		m_GridCtrl.SetFocusCell(iStartRow, iStartCol);
		m_GridCtrl.SetSelectedRange(iStartRow, 0, iStartRow, m_GridCtrl.GetColumnCount()-1);

		RequestViewDataCurrentVisibleRow();
		

		SortColumn(3, 1);	// 总是排序n列
	}
	else
	{
		ASSERT( 0 );
	}
}

void CIoViewBlockReport::SetRowHeightAccordingFont()
{	
	m_GridCtrl.SetDefCellHeight(28);
	m_GridCtrl.AutoSizeRows();	
	if (0 != m_GridCtrl.GetFixedRowCount())
	{
		m_GridCtrl.SetRowHeight(0, 40);
	}
}

void CIoViewBlockReport::SetColWidthAccordingFont()
{
	int32 iColWidth = 90;
	m_GridCtrl.AutoSizeColumns();
	for ( int i=0; i < m_GridCtrl.GetColumnCount() ; i++ )	// 调整最小col
	{
		if(i == 2)
		{
			iColWidth = 130;
		}
		else
		{
			iColWidth = 90;
		}

		if ( m_GridCtrl.GetColumnWidth(i) > 0 && m_GridCtrl.GetColumnWidth(i) < iColWidth )
		{
			m_GridCtrl.SetColumnWidth(i, iColWidth);
		}
	}
}

void CIoViewBlockReport::UpdateTableHeader()
{
	CStringArray aColNames;
	CBlockDataPseudo::GetDefaultColNames(aColNames);		// 表头固定

	m_GridCtrl.SetFixedColumnCount(2);	// 序号列 名称固定
	m_GridCtrl.SetFixedRowCount(1);	
	m_GridCtrl.DeleteNonFixedRows();
	m_GridCtrl.SetColumnCount(aColNames.GetSize());	// 序号列, 名称...

	SetColWidthAccordingFont();
	SetRowHeightAccordingFont();
	m_GridCtrl.Refresh();
}

void CIoViewBlockReport::UpdateTableContent( int32 /*iClassId*/, int32 iBlockId, bool32 /*bBlink*/ )
{
	//DWORD dwTime = timeGetTime();
	// 检查是否在显示范围内，
	CCellRange rangeVisible = m_GridCtrl.GetVisibleNonFixedCellRange();
	if ( !rangeVisible.IsValid() )
	{
		return;
	}

	int i = 0;
	for ( i=rangeVisible.GetMinRow(); i <= rangeVisible.GetMaxRow() ; i++ )
	{
		if ( (int32)(m_GridCtrl.GetItemData(i, 0)) == iBlockId )
		{
			break;
		}
	}
	if ( i >= rangeVisible.GetMaxRow() )
	{
		return;
	}

	m_GridCtrl.RedrawRow(i);		// 更新这一行显示就可以了

	//TRACE(_T("绘制一行: %dms\n"), timeGetTime()-dwTime);

// 	SetColWidthAccordingFont();
// 	m_GridCtrl.Refresh();
}

void CIoViewBlockReport::UpdateTableAllContent()
{
	m_GridCtrl.DeleteNonFixedRows();	
	//m_GridCtrl.ResetVirtualOrder();

	T_BlockReportTab tab;
	if ( !GetCurrentRowInfo(tab) )
	{
		return;
	}
	
	int32 iMaxRowCount = 0;
	// 虚模式，所有行都加进来
	iMaxRowCount = m_aShowBlockIds.GetSize();
	

	m_GridCtrl.SetRowCount(iMaxRowCount + m_GridCtrl.GetFixedRowCount());
	
	
	//SetColWidthAccordingFont();
	//SetRowHeightAccordingFont();
	m_GridCtrl.Refresh();
}

void CIoViewBlockReport::ResetBlockShowData(bool32 bResetTopLeft/*=true*/)
{
	T_BlockReportTab tab;
	if ( GetCurrentRowInfo(tab) )
	{
		KillTimer(KRequestSortDataTimerId);		// 停止定时器请求排行

		m_iSortColumn = -1;
		m_bRequestViewSort = false;
		m_GridCtrl.SetSortColumn(-1);
		m_GridCtrl.SetHeaderSort(FALSE);
		
		if ( bResetTopLeft )
		{
			SetCurrentRowInfo(m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetFixedColumnCount());	// 将本区域保存的左上角可见重置
			m_aShowBlockIds.Copy(tab.m_aSubBlockIds);
			
			UpdateTableAllContent();
			
			//m_GridCtrl.EnsureVisible(m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetFixedColumnCount());
			m_GridCtrl.EnsureTopLeftCell(m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetFixedColumnCount());
		}
		else
		{
			m_aShowBlockIds.Copy(tab.m_aSubBlockIds);
			m_GridCtrl.SetRowCount(m_aShowBlockIds.GetSize() + m_GridCtrl.GetFixedRowCount());
			m_GridCtrl.ResetVirtualOrder();
			m_GridCtrl.Refresh();
		}
	}
}

bool32 CIoViewBlockReport::GetCurrentRowInfo(OUT T_BlockReportTab &tab)
{
	return GetTabInfoByIndex(m_GuiTabWnd.GetCurtab(), tab);
}

void CIoViewBlockReport::SetCurrentRowInfo( int32 iRowBegin, int32 iColLeft )
{
	int32 iCurTab = m_GuiTabWnd.GetCurtab();
	T_BlockReportTab tab;
	if ( GetTabInfoByIndex(iCurTab, tab) )
	{
		m_aTabInfomations[iCurTab].m_iStartRow = iRowBegin;
		m_aTabInfomations[iCurTab].m_iColLeftVisible = iColLeft;
	}
}

bool32 CIoViewBlockReport::GetTabInfoByIndex( int32 iIndex, OUT T_BlockReportTab &tabInfo )
{
	if ( iIndex >=0 && iIndex < m_aTabInfomations.GetSize() )
	{
		tabInfo = m_aTabInfomations[iIndex];
		return true;
	}
	return false;
}

void CIoViewBlockReport::OnDoBlockReportInitialize()
{
	m_bBlockReportInitialized = true;		// 仅仅是一个标志，现在还不做实质初始化

	int32 iSortColNow = m_iSortColumn;		// 现有排序列
	bool32 bReqNow = m_bRequestViewSort;

	TabChanged();		// 重装载一次板块数据
	if ( -1 != m_iSortColumnFromXml )
	{
		// xml中有排序状态，有则必定成对有效，否则可能已经被外部改变了，不需要再次改变初始化状态
		SortColumn(m_iSortColumnFromXml, m_iSortTypeFromXml);	// type可能是0，因为以前可能没有保存type
	}
	else if ( bReqNow )
	{
		// 请求已经发出去了，还没有处理，只需要恢复这个标志就可以了
		m_iSortColumn = iSortColNow;		// 恢复排序
		m_bRequestViewSort = bReqNow;
	}
}

void CIoViewBlockReport::InitializeTabs()
{
	int i = 0;
	bool32 bInitOld = m_bBlockReportInitialized;
	m_bBlockReportInitialized = false;				// 禁止tab响应

	m_GuiTabWnd.DeleteAll();
	m_aTabInfomations.RemoveAll();

	T_BlockReportTab tab;
	tab.m_iBlockCollectionId = T_BlockReportTab::BCAll;		// 所有板块 取板块配置里面所有板块合集
	tab.m_StrName = _T("全部");
	m_aTabInfomations.Add(tab);

	
	IdArray aIds;
	CBlockConfig::Instance()->GetCollectionIdArray(aIds);

	int32	iMarketClassCollectionId;
	bool32 bHasMarketClassCollection = false;
	bHasMarketClassCollection = CBlockConfig::Instance()->GetMarketClassCollectionId(iMarketClassCollectionId);
	
	for ( i=0; i < aIds.GetSize() ; i++ )
	{
		// 需要判断分类板块，剔除出去 - TODO
		if ( bHasMarketClassCollection && iMarketClassCollectionId == aIds[i] )
		{
			continue;		// 分类板块中的板块不要
		}

		CBlockCollection *pCollection = CBlockConfig::Instance()->GetBlockCollectionById(aIds[i]);
		if ( NULL == pCollection || pCollection->m_aBlocks.GetSize() <= 0 )
		{
			continue;	// 
		}
		int32 iBlockType = pCollection->m_aBlocks[0]->m_blockInfo.m_iType;
		if ( CBlockInfo::typeNormalBlock != iBlockType && CBlockInfo::typeUserBlock != iBlockType )
		{
			continue; // 不支持的类型
		}

		T_BlockReportTab tempTab;
		tempTab.m_iBlockCollectionId = aIds[i];
		CBlockConfig::Instance()->GetCollectionName(aIds[i], tempTab.m_StrName);
		ASSERT( !tempTab.m_StrName.IsEmpty() );
		m_aTabInfomations.Add(tempTab);
	}
	
	// 交换4，5的元素
	T_BlockReportTab tblockTemp;
	for (int i = 0; i < m_aTabInfomations.GetSize(); i++)
	{
		if (i == 0)
		{
			continue;
		}
		if (5 == m_aTabInfomations[i].m_iBlockCollectionId  && 4 == m_aTabInfomations[i-1].m_iBlockCollectionId)
		{
			tblockTemp = m_aTabInfomations[i];
			m_aTabInfomations[i] = m_aTabInfomations[i-1];
			m_aTabInfomations[i-1] = tblockTemp;
			break;
		}
	}

	for ( i=0; i < m_aTabInfomations.GetSize() ; i++ )
	{
		const T_BlockReportTab &tTab = m_aTabInfomations[i];
		CString StrName = tTab.m_StrName;
		m_GuiTabWnd.Addtab(StrName, StrName, StrName);
	}

	for ( i=0; i < m_aTabInfomations.GetSize() ; i++ )
	{
		const T_BlockReportTab &tTmptab = m_aTabInfomations[i];
		
		if ( m_iBlockClassId == tTmptab.m_iBlockCollectionId )
		{
			m_GuiTabWnd.SetCurtab(i);
			break;
		}
	}

	if ( i >= m_aTabInfomations.GetSize() )
	{
		m_iBlockClassId = T_BlockReportTab::BCAll;
		m_GuiTabWnd.SetCurtab(0);
	}

	m_bBlockReportInitialized = bInitOld;
}

void CIoViewBlockReport::SetTabByClassId( int32 iClassId, int32 iSortCol/*=-1*/, int32 iSortType/*=0*/ )
{
	int i = 0;
	for ( i=0; i < m_aTabInfomations.GetSize() ; i++ )
	{
		if ( iClassId == m_aTabInfomations[i].m_iBlockCollectionId )
		{
			if ( m_GuiTabWnd.GetCount() > i )
			{
				m_GuiTabWnd.SetCurtab(i);		// 当前选择是否变更??
				if ( iSortCol >= 0 )
				{
					SortColumn(iSortCol, iSortType);
				}
				break;
			}
			else
			{
				ASSERT( 0 );
			}
		}
	}
}

void CIoViewBlockReport::RecalcLayout()
{
	m_RectTitle.SetRectEmpty();
	m_iMaxGridVisibleRow = 0;
	int32 iHeightScroll = 12;
	int32 iGuiTabHeight = 21;

	CRect rcClient;
	GetClientRect(rcClient);

	m_RectTitle = rcClient;

	CClientDC dc(this);
	CFont *pFontOld ;
	if (m_bShowTitle)
	{
		pFontOld = dc.SelectObject(GetIoViewFontObject(ESFNormal));
		CSize sizeTitle = dc.GetTextExtent(_T("标题高度测试"));
		dc.SelectObject(pFontOld);

		m_RectTitle.bottom = m_RectTitle.top + sizeTitle.cy + 10;

		rcClient.top = m_RectTitle.bottom;
	}

	CSize sizeTab = m_GuiTabWnd.GetWindowWishSize();
	sizeTab.cy = iGuiTabHeight;
	if ( rcClient.Height() < sizeTab.cy )
	{
		m_GuiTabWnd.MoveWindow(0,0,0,0);
		m_GridCtrl.MoveWindow(0,0,0,0);
		m_XSBHorz.MoveWindow(0,0,0,0);
		return;
	}
	
	// 优先满足tab，但是srcoll不能小于一个特定值, 如果总宽度小于该值，则所有宽度都是scroll的
	int32 iMaxTabWidth = rcClient.Width() - 125;	// 
	iMaxTabWidth = iMaxTabWidth < 0 ? 0 : iMaxTabWidth;

	sizeTab.cx = MIN(iMaxTabWidth, sizeTab.cx);		// 最多分1/1
	m_GuiTabWnd.MoveWindow(rcClient.left, rcClient.top -1, sizeTab.cx, sizeTab.cy);
	CRect rcHorz(rcClient.left , rcClient.bottom - iHeightScroll, rcClient.right, rcClient.bottom);
	m_XSBHorz.SetSBRect(rcHorz, TRUE);

	rcClient.bottom -= iHeightScroll;
	rcClient.top += sizeTab.cy;
	rcClient.top -= 1;



	if(m_bIsShowGridVertScorll)	// 是否显示垂直滚动条
	{
		rcClient.right -= iHeightScroll;

		CRect RectVScroll;
		RectVScroll.left = rcClient.right;
		RectVScroll.right = RectVScroll.left + iHeightScroll;
		RectVScroll.top = rcClient.top;
		RectVScroll.bottom = rcClient.bottom;
		m_XSBVert.SetSBRect(RectVScroll, TRUE);
	}



	m_GridCtrl.MoveWindow(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height());	// 不给竖直滚动分配了

	// 计算表格最大行 - 粗略计算  - 需要插入足够的数据
	pFontOld = dc.SelectObject(GetIoViewFontObject(ESFNormal));
	CSize sizeText = dc.GetTextExtent(_T("行高度测试"));
	dc.SelectObject(pFontOld);
	m_iMaxGridVisibleRow = rcClient.Height() / sizeText.cy + 1;	// 多算几个
	if ( m_iMaxGridVisibleRow < 0 )
	{
		m_iMaxGridVisibleRow = 0;
	}
}

void CIoViewBlockReport::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	
}

void CIoViewBlockReport::OnConfigInitialize( E_InitializeNotifyType eInitializeType )
{
	if ( Initialized == eInitializeType )
	{
		// 板块信息重建
		InitializeTabs();
		RecalcLayout();
		SetTabByClassId(m_iBlockClassId);
		
	}
}

void CIoViewBlockReport::OnBlockConfigChange( int32 iBlockId, E_BlockNotifyType eNotifyType )
{
	// 板块变更不理
	Invalidate(TRUE);	// 刷新下显示
}

void CIoViewBlockReport::OnGridGetDispInfo( NMHDR *pNotifyStruct, LRESULT *pResult )
{
	//DWORD dwTime = timeGetTime();
	if ( NULL != pNotifyStruct )
	{
		GV_DISPINFO	*pDisp = (GV_DISPINFO *)pNotifyStruct;
		// 尝试v mode
		
		GV_ITEM &item = pDisp->item;
		item.lParam = NULL;
		if ( item.row >=0 && item.row <= m_aShowBlockIds.GetSize() )
		{
			CString StrValue;
			
			CBlockDataPseudo::HeaderArray aHeaders;
			CBlockDataPseudo::GetDefaultColArray(aHeaders);
			CStringArray aColNames;
			CBlockDataPseudo::GetDefaultColNames(aColNames);
			ASSERT( item.col >=0 && item.col < aColNames.GetSize() );
			if ( item.row == 0 )	// 表头
			{
				//	设置列表头字体
				
				if ( item.col >=0 && item.col < aColNames.GetSize() )
				{
					item.strText = aColNames[item.col];
					item.lParam  = (LPARAM)aHeaders[item.col];
					item.crFgClr = GetIoViewColor(ESCVolume);
					memcpyex(&item.lfFont, GetColumnExLF(), sizeof(LOGFONT));
				}
// 				else if ( item.col == 0 )
// 				{
// 					item.strText = _T("          ");	// 序号不显示
// 					item.lParam  = (LPARAM)aHeaders[item.col];
// 				}
			}
			else
			{
				// 数据内容
				int32 iBlockId = m_aShowBlockIds[item.row-1];
				item.lParam = iBlockId;
				
				CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(iBlockId);
				
				// 应当按照表头来显示，不过没有改变表头的方法，就按照序号来显示
				CBlockDataPseudo::E_Header eHeader = aHeaders[item.col];
				memcpyex(&item.lfFont, GetIoViewFont(ESFText), sizeof(LOGFONT));
				switch(eHeader)
				{
				case CBlockDataPseudo::ShowRowNo:
					item.crFgClr = GetIoViewColor(ESCText);
					break;
				case CBlockDataPseudo::BlockName:
					item.crFgClr = RGB(245,253,170);
					memcpyex(&item.lfFont, GetIoViewFont(ESFNormal), sizeof(LOGFONT));
					break;
				case CBlockDataPseudo::BlockId:
					item.crFgClr = GetIoViewColor(ESCText);
					break;
				case CBlockDataPseudo::RiseMerchCount:
					item.crFgClr = GetIoViewColor(ESCRise);
					break;
				case CBlockDataPseudo::FallMerchCount:
					item.crFgClr = GetIoViewColor(ESCFall);
					break;
				default:						
					if (!(CBlockDataPseudo::CapitalFlow == eHeader   || CBlockDataPseudo::RiseRate == eHeader || CBlockDataPseudo::WeightRiseRate == eHeader ||CBlockDataPseudo::RiseRate 
						|| CBlockDataPseudo::RiseSpeed || CBlockDataPseudo::PrincipalPureNum || CBlockDataPseudo::PrincipalAmount || CBlockDataPseudo::AmountRate))
					{
						item.crFgClr = RGB(0xdc, 0xdc, 0xdc);
					}
					if (CBlockDataPseudo::RiseTopestMerch == eHeader)
					{
						memcpyex(&item.lfFont, GetIoViewFont(ESFNormal), sizeof(LOGFONT));
					}
					break;
				}
				if ( eHeader == CBlockDataPseudo::ShowRowNo )
				{
					// 序号，显示的行的序号，而不是实际数据行的序号
					int i = 0;
					for ( CGridCtrl::intlist::const_iterator it = m_GridCtrl.m_arRowOrder.begin(); it != m_GridCtrl.m_arRowOrder.end() ; it++, i++ )
					{
						if ( *it == item.row )
						{
							item.strText.Format(_T("%d  "), i);
							break;
						}
					}
					
				}
				else if ( pBlock != NULL )
				{					
					item.strText = _T(" -");
					CGridCellSymbol *pCell = (CGridCellSymbol *)m_GridCtrl.GetDefaultVirtualCell();
					ASSERT( pCell->IsKindOf(RUNTIME_CLASS(CGridCellSymbol)) );
					pCell->SetShowSymbol(CGridCellSymbol::ESSNone);

					switch (eHeader)
					{
					case CBlockDataPseudo::BlockName:		// 板块名称
						item.strText = pBlock->m_blockInfo.m_StrBlockName;						
						break;
					case CBlockDataPseudo::BlockId:			// 板块代码
						item.strText.Format(L"%d",pBlock->m_blockInfo.m_iBlockId);
						break;
					case CBlockDataPseudo::RiseRate:		// 涨幅
					case CBlockDataPseudo::RiseSpeed:		         // 涨速
					case CBlockDataPseudo::PrincipalPureNum:         // 主力净量
					case CBlockDataPseudo::PrincipalAmount:		     // 主力金额
					case CBlockDataPseudo::AmountRate:		         // 量比
						{
							pCell->SetShowSymbol(CGridCellSymbol::ESSFall);
							if ( pBlock->IsValidLogicBlockData() )
							{
								CBlockDataPseudo blockData(pBlock->m_logicBlockData);
								float fValue;
								if ( blockData.GetColValue(eHeader, &fValue) )
								{
									item.strText = Float2SymbolString(fValue, 0, 2, true);
								}
							}
						}
						break;
					case CBlockDataPseudo::RiseMerchCount:		     // 涨家数
					case CBlockDataPseudo::FallMerchCount:		     // 跌家数
						{
							if ( pBlock->IsValidLogicBlockData() )
							{
								CBlockDataPseudo blockData(pBlock->m_logicBlockData);
								float fValue;
								if ( blockData.GetColValue(eHeader, &fValue) )
								{
									CString strTmpValue = Float2String(fValue, 0, false, false);
									if(strTmpValue == L"0")
									{
										strTmpValue = L" -";
									}
									item.strText = strTmpValue;
								}
							}
						}
						break;
					case CBlockDataPseudo::RiseTopestMerch:         // 领涨股
						{
							COLORREF clr;
							item.crFgClr = GetIoViewColor(ESCAmount); //RGB(176,176,0);
							StrValue = pBlock->m_logicBlockData.m_StrMerchName;
							// 找寻该商品 - 是否是自选股
							CMerch *pMerchTopest = pBlock->m_blockInfo.FindMerchByCnName(StrValue);
							T_Block* pUserBlock = NULL;
							if ( NULL != pMerchTopest && (pUserBlock = CUserBlockManager::Instance()->GetBlock(pMerchTopest)) != NULL  )
							{
								clr = pUserBlock->m_clrBlock;

								// 自选板块的商品名字颜色特殊显示:
								if ( COLORNOTCOUSTOM != clr )
								{
									item.crFgClr = clr;
								}	
							}
							// 是否有标记
							GV_DRAWMARK_ITEM markItem;
							if ( InitDrawMarkItem(pMerchTopest, markItem) )
							{
								item.markItem = markItem;
							}

							item.strText = pBlock->m_logicBlockData.m_StrMerchName;	// 发过来的是代码还是名称
						}

						break;
					case CBlockDataPseudo::TotalHand:		      // 总手
					case CBlockDataPseudo::TotalAmount:		      // 总金额
					case CBlockDataPseudo::TotalMarketValue:	  // 总市值
					case CBlockDataPseudo::CircleMarketValue:	  // 流通市值
						{
							item.crFgClr = GetIoViewColor(ESCText); //RGB(176,176,0);
							if ( pBlock->IsValidLogicBlockData() )
							{
								CBlockDataPseudo blockData(pBlock->m_logicBlockData);
								float fValue;
								if ( blockData.GetColValue(eHeader, &fValue) )
								{
									item.strText = Float2SymbolString(fValue, 0, 2, true);
								}
							}
						}
						break;

					default:
						ASSERT( 0 );
					}
				}
			}
			
			if (item.col == 1 || item.col == 2 )
			{
				item.nFormat = DT_LEFT |DT_SINGLELINE |DT_VCENTER |DT_NOPREFIX;
			}
		}
		//TRACE(_T("热门板块: 获取显示cell(%d,%d) %d ms\n"), item.row, item.col, timeGetTime()-dwTime);
	}
	if ( NULL != *pResult )
	{
		*pResult = 1;
	}
}

bool32 CIoViewBlockReport::UpdateTabInfo( int32 iTabIndex, int32 iRowFirst, int32 iColLeft )
{
	if ( iTabIndex >=0 && iTabIndex < m_aTabInfomations.GetSize() )
	{
		m_aTabInfomations[iTabIndex].m_iStartRow = iRowFirst;
		m_aTabInfomations[iTabIndex].m_iColLeftVisible = iColLeft;
		return true;
	}
	return false;
}

bool CIoViewBlockReport::CompareRow( int iRow1, int iRow2 )
{
	ASSERT( m_spThis != NULL );
	ASSERT( m_spThis->m_aShowBlockIds.GetSize() >= iRow1 && m_spThis->m_aShowBlockIds.GetSize() >= iRow2  );
	const int iIndex1 = iRow1-1;
	const int iIndex2 = iRow2-1;
	
	int32 iBlockId1 = m_spThis->m_aShowBlockIds[iIndex1];
	int32 iBlockId2 = m_spThis->m_aShowBlockIds[iIndex2];

	CBlockLikeMarket *pBlock1 = CBlockConfig::Instance()->FindBlock(iBlockId1);
	CBlockLikeMarket *pBlock2 = CBlockConfig::Instance()->FindBlock(iBlockId2);
	
	const int iCol = m_spThis->m_GridCtrl.m_CurCol;
	ASSERT( iCol != 0 );	// 序号不能排序
	CBlockDataPseudo::E_Header eHeader = CBlockDataPseudo::BlockName;
	CBlockDataPseudo::HeaderArray aHeaders;
	CBlockDataPseudo::GetDefaultColArray(aHeaders);	// 现在没有提供设置表头，使用默认的
	eHeader = aHeaders[iCol];

	bool bRet = true;
	if ( pBlock1 == NULL && pBlock2 != NULL )
	{
		bRet = false;
	}
	else if ( pBlock1 != NULL && pBlock2 == NULL )
	{
		bRet = true;
	}
	else if ( pBlock1 == NULL && pBlock2 == NULL )
	{
		bRet = false;
	}
	else
	{
		switch (eHeader)
		{
		case CBlockDataPseudo::BlockName:		// 板块名称
			{
				if(pBlock1 && pBlock2)
				{
					int iCmp = CompareString(LOCALE_SYSTEM_DEFAULT, 0, pBlock1->m_blockInfo.m_StrBlockName, -1,
						pBlock2->m_blockInfo.m_StrBlockName, -1);
					bRet = iCmp < CSTR_EQUAL;
				}
			}
			break;
		case CBlockDataPseudo::BlockId:
			{
				bRet = iBlockId1 < iBlockId2;
			}
			break;
		case CBlockDataPseudo::RiseRate:
		case CBlockDataPseudo::WeightRiseRate:
		case CBlockDataPseudo::TotalAmount:
		case CBlockDataPseudo::MarketRate:
		case CBlockDataPseudo::ChangeRate:
		case CBlockDataPseudo::MarketWinRateDync:
		case CBlockDataPseudo::CapitalFlow:
		case CBlockDataPseudo::MerchRise:
		case CBlockDataPseudo::RiseDays:
			{
				if(pBlock1 && pBlock2)
				{
					float fValue1, fValue2;
					fValue1 = fValue2 = FLT_MIN;
					if ( pBlock1->IsValidLogicBlockData() )
					{
						CBlockDataPseudo blockData(pBlock1->m_logicBlockData);
						blockData.GetColValue(eHeader, &fValue1);
					}
					if ( pBlock2->IsValidLogicBlockData() )
					{
						CBlockDataPseudo blockData(pBlock2->m_logicBlockData);
						blockData.GetColValue(eHeader, &fValue2);
					}
					bRet = fValue1 < fValue2;
				}
			}
			break;
		case CBlockDataPseudo::RiseMerchRate:
			{
				if(pBlock1 && pBlock2)
				{
					float fValue1, fValue2;
					fValue1 = fValue2 = FLT_MIN;
					if ( pBlock1->IsValidLogicBlockData() )
					{
						CBlockDataPseudo blockData(pBlock1->m_logicBlockData);
						float fRise, fFall;
						fRise = fFall = 0.0f;
						blockData.GetColValue(CBlockDataPseudo::RiseMerchCount, &fRise);
						blockData.GetColValue(CBlockDataPseudo::FallMerchCount, &fFall);
						if ( 0.0f != fFall )
						{
							fValue1 = fRise/fFall;
						}
						else
						{
							fValue1 = fRise*10000;	// 放大n倍
						}
					}
					if ( pBlock2->IsValidLogicBlockData() )
					{
						CBlockDataPseudo blockData(pBlock2->m_logicBlockData);
						float fRise, fFall;
						fRise = fFall = 0.0f;
						blockData.GetColValue(CBlockDataPseudo::RiseMerchCount, &fRise);
						blockData.GetColValue(CBlockDataPseudo::FallMerchCount, &fFall);
						if ( 0.0f != fFall )
						{
							fValue2 = fRise/fFall;
						}
						else
						{
							fValue2 = fRise*10000;	// 放大n倍
						}
					}
					if ( fValue1 == 0.0f && fValue2 == 0.0f )
					{
						bRet = pBlock1->m_blockInfo.m_aSubMerchs.GetSize() < pBlock2->m_blockInfo.m_aSubMerchs.GetSize();
					}
					else
					{
						bRet = fValue1 < fValue2;
					}
				}
			}
			break;
		case CBlockDataPseudo::RiseTopestMerch:
			{
				if(pBlock1 && pBlock2)
				{
					int iCmp = CompareString(LOCALE_SYSTEM_DEFAULT, 0, pBlock1->m_logicBlockData.m_StrMerchName, -1,
						pBlock2->m_logicBlockData.m_StrMerchName, -1);
					bRet = iCmp < CSTR_EQUAL;
				}
			}
			break;
		default:
			ASSERT( 0 );
		}
	}
	
	return bRet;
}

void CIoViewBlockReport::OnGridCacheHint( NMHDR *pNotifyStruct, LRESULT *pResult )
{
	// 如果当前排序字段为非排序或者本地排序字段，则不需要理会，如果为服务器排序字段，则检查本地存在的排行数据是否足够
	// 不够则需要重发排行请求，等待数据回归，则进行刷新显示
	// 里面关键的请求数据代码被以前注释掉了，所以这个函数都没用了， 全部注释掉--chenfj
	/*GV_CACHEHINT *pCacheHint = (GV_CACHEHINT *)pNotifyStruct;
	if ( NULL != pCacheHint && m_bRequestViewSort && pCacheHint->range.IsValid() )	// 如果没有服务器数据需求，不需要处理
	{
		const CCellRange &cellRange = pCacheHint->range;
		// 这里的范围是需要数据范围 实际数据需要映射得到
		if ( cellRange.IsValid() )
		{
			int32 iMinRow = cellRange.GetMinRow() - m_GridCtrl.GetFixedRowCount();
			int32 iMaxRow = cellRange.GetMaxRow() - m_GridCtrl.GetFixedRowCount();
			ASSERT( m_iMaxGridVisibleRow >= cellRange.GetRowSpan() );
			ASSERT( iMaxRow < m_aShowBlockIds.GetSize() );
			int32 iBlockSize =  m_aShowBlockIds.GetSize();
			iMaxRow = min(iMaxRow, iBlockSize);
			bool32 bRequestData = false;
			for ( int i=iMinRow; i <= iMaxRow ; i++ )
			{
				if ( m_aShowBlockIds[i] == CBlockInfo::GetInvalidId() )
				{
					bRequestData = true;
					break;
				}
			}
			if ( bRequestData )
			{
				// 申请数据 - TODO
				// 请求数据 - 随便请求点数据
				// 全部数据已经申请，必须等到回来才能显示
				
				//m_MmiRequestSys.m_iMarketId = 1; // shang a
 			//	m_MmiRequestSys.m_iStart = iMinRow;
 			//	int32 iMaxLeaveDataCount = m_aShowBlockIds.GetSize() - iMinRow;
 			//	m_MmiRequestSys.m_iCount = min(m_iMaxGridVisibleRow, iMaxLeaveDataCount);
				//RequestData(m_MmiRequestSys);
			}
		}
	}
*/
	if ( NULL != pResult )
	{
		*pResult = 1;
	}
}

void CIoViewBlockReport::RequestData( CMmiCommBase &req )
{
	//if ( IsWindowVisible() )
	{
		DoRequestViewData(req);
	}
}

void CIoViewBlockReport::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	//RequestViewDataCurrentVisibleRow();
}

void CIoViewBlockReport::OnLogicBlockDataUpdate( const IdArray &aUpdatedBlockIds )
{
	CCellRange rangeVisible = m_GridCtrl.GetVisibleNonFixedCellRange();
	if ( !rangeVisible.IsValid() )
	{
		return;
	}

	//DWORD dwTime = timeGetTime();
	typedef set<int32> IdSet;
	IdSet ids;
	int32 i = 0;
	for ( i=0; i < m_aShowBlockIds.GetSize() ; ++i )
	{
		ids.insert(m_aShowBlockIds[i]);
	}
	IdArray aMyBlockIds;
	aMyBlockIds.SetSize(0, aUpdatedBlockIds.GetSize());
	for ( i=0; i < aUpdatedBlockIds.GetSize() ; ++i )
	{
		int32 iId = aUpdatedBlockIds[i];
		if ( ids.count(iId) > 0 )
		{
			aMyBlockIds.Add(iId);
		}
	}

	if ( aMyBlockIds.GetSize() > 20 )
	{
		m_GridCtrl.Refresh();
	}
	else
	{
		// 仅适合少量更新
		for ( int i=0; i < aMyBlockIds.GetSize() ; i++ )
		{
			UpdateTableContent(0, aMyBlockIds[i], false);
		}
	}
	//TRACE(_T("更新%d: %d ms\n"), aUpdatedBlockIds.GetSize(), timeGetTime()-dwTime);

	if ( aMyBlockIds.GetSize() > 0 )
	{
		DoLocalSort();	// 尝试本地排序的更新
	}
}

void CIoViewBlockReport::OnBlockSortDataUpdate( const CMmiRespBlockSort *pRespBlockSort )
{
	ASSERT( NULL != pRespBlockSort );
	int32 iTab;
	if ( m_MmiRequestBlockSort.m_iStart == pRespBlockSort->m_iStart
		&& m_MmiRequestBlockSort.m_eBlockSortType == pRespBlockSort->m_eBlockSortType
		&& m_MmiRequestBlockSort.m_bDescSort == pRespBlockSort->m_bDescSort
		/*&& m_MmiRequestBlockSort.m_iCount == pReqBlockSort->m_iCount*/		// 全部数据，不予比较
		&& TabIsValid(iTab) )
	{
		if ( pRespBlockSort->m_aBlockIDs.GetSize() > 0 )
		{
			IdArray aIdMy;
			aIdMy.SetSize(0, m_aTabInfomations[iTab].m_aSubBlockIds.GetSize());
			IdArray aIdLocal;
			aIdLocal.Copy(m_aTabInfomations[iTab].m_aSubBlockIds);
			for ( int j=0; j < pRespBlockSort->m_aBlockIDs.GetSize() ; j++ )
			{
				int32 iBlockId = pRespBlockSort->m_aBlockIDs[j];
				for ( int i=0; i < aIdLocal.GetSize() ; i++ )
				{
					if ( aIdLocal[i] == iBlockId )
					{
						aIdMy.Add(iBlockId);
						aIdLocal.RemoveAt(i);
						break;
					}
				}
			}

			// 剩下的本地id就是没有返回来的数据了，全部插入到最后
			aIdMy.Append(aIdLocal);
			
			{
				m_aShowBlockIds.Copy(aIdMy);		// 如果返回板块与本地板块数据不一直怎么办 - 比如自选就不会从服务器传过来
				m_GridCtrl.SetRowCount(m_GridCtrl.GetFixedRowCount() + m_aShowBlockIds.GetSize());

				m_GridCtrl.SetSortColumn(m_iSortColumn);
				m_GridCtrl.SetSortAscending(!m_MmiRequestBlockSort.m_bDescSort);

				RequestViewDataCurrentVisibleRow();	// 排序在请求数据更新
				m_GridCtrl.Refresh();
			}
		}
	}
}

LRESULT CIoViewBlockReport::OnScrollPosChange( WPARAM w, LPARAM l )
{
	//if ( (HWND)w == m_XSBVert.m_hWnd )
	//{
	//	// 现在所有板块都请求，所以不需要这个了
	//	SetTimer(KChangeVisibleBlocksTimerId, KChangeVisibleBlocksTimerPeriod, NULL);	// 启动定时器，请求当前可见商品数据
	//}

	return 1;
}

void CIoViewBlockReport::OnVDataPluginResp( const CMmiCommBase *pResp )
{
	if ( NULL == pResp )
	{
		return;
	}
	
	if ( pResp->m_eCommType != ECTRespPlugIn )
	{
		ASSERT( 0 );
		return;
	}
	
	const CMmiCommBasePlugIn *pRespPlugin = (CMmiCommBasePlugIn *)pResp;
	if ( pRespPlugin->m_eCommTypePlugIn == ECTPIRespBlockSort )
	{
		OnBlockSortDataUpdate((const CMmiRespBlockSort *)pRespPlugin);
	}
	else if ( pRespPlugin->m_eCommTypePlugIn == ECTPIRespBlock
		|| pRespPlugin->m_eCommTypePlugIn == ECTPIRespAddPushBlock )
	{
		OnBlockDataResp((const CMmiRespLogicBlock *)pResp);
	}
}

void CIoViewBlockReport::OnBlockDataResp( const CMmiRespLogicBlock *pResp )
{
	TRACE(_T("收到板块数据个数: %d 个\r\n"), pResp->m_aBlockData.GetSize());
	int i=0;
	CBlockConfig::IdArray aUpdatedBlockIds;
	for ( i=0; i < pResp->m_aBlockData.GetSize(); i++ )
	{
		CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(pResp->m_aBlockData[i].m_iBlockId);
		if ( pBlock != NULL )
		{
			pBlock->m_logicBlockData = pResp->m_aBlockData[i];
			aUpdatedBlockIds.Add(pResp->m_aBlockData[i].m_iBlockId);
		}
	}
	
	if ( aUpdatedBlockIds.GetSize() <= 0 )
	{
		return;
	}
	
	OnLogicBlockDataUpdate(aUpdatedBlockIds);
}

void CIoViewBlockReport::OnContextMenu( CWnd* pWnd, CPoint pos )
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

void CIoViewBlockReport::DoTrackMenu(CPoint pos)
{
	CNewMenu menu;
	menu.CreatePopupMenu();
	
	int32 iCmd = 1;
	menu.AppendMenu(MF_STRING, iCmd++, _T("打开板块"));
	menu.SetDefaultItem(1, FALSE);
	menu.AppendMenu(MF_SEPARATOR);

	// 网格线
	BOOL bShowGrid = m_GridCtrl.GetGridLines() != GVL_NONE;
	menu.AppendMenu(MF_STRING, iCmd++, bShowGrid ? _T("隐藏网格线"): _T("显示网格线"));
	menu.AppendMenu(MF_SEPARATOR);
	
	for ( int i=0; i < m_aTabInfomations.GetSize() ; i++ )
	{
		menu.AppendMenu(MF_STRING, iCmd++, m_aTabInfomations[i].m_StrName);
	}

	// 普通按钮
	if ( m_aTabInfomations.GetSize() > 0 )
	{
		menu.AppendMenu(MF_SEPARATOR);
	}
	// 插入内容:
 	CNewMenu* pIoViewPopMenu = menu.AppendODPopupMenu(L"插入内容");
 	ASSERT(NULL != pIoViewPopMenu );
 	AppendIoViewsMenu(pIoViewPopMenu, IsLockedSplit());
	
	// 关闭内容:
	menu.AppendMenu(MF_STRING, IDM_IOVIEWBASE_CLOSECUR, _T("关闭内容"));
	
	// 内容切换:	
	menu.AppendMenu(MF_STRING, IDM_IOVIEWBASE_TAB, _T("内容切换 TAB"));
	menu.AppendMenu(MF_SEPARATOR);

	// 全屏/恢复
	menu.AppendODMenu(L"全屏/恢复 F7", MF_STRING, IDM_IOVIEWBASE_F7);
	menu.AppendODMenu(L"", MF_SEPARATOR);

	
	// 分割窗口
	CNewMenu menuSplit;
	menuSplit.CreatePopupMenu();
	menuSplit.AppendMenu( MF_STRING, IDM_IOVIEWBASE_SPLIT_TOP,    L"添加上视图");
	menuSplit.AppendMenu( MF_STRING, IDM_IOVIEWBASE_SPLIT_BOTTOM, L"添加下视图");
	menuSplit.AppendMenu( MF_STRING, IDM_IOVIEWBASE_SPLIT_LEFT,   L"添加左视图");
	menuSplit.AppendMenu( MF_STRING, IDM_IOVIEWBASE_SPLIT_RIGHT,  L"添加右视图");
	menu.AppendMenu(MF_POPUP, (UINT)menuSplit.m_hMenu, _T("分割窗口"));
	
	// 关闭窗口
	menu.AppendMenu(MF_STRING, IDM_IOVIEWBASE_CLOSE, _T("关闭窗口"));
	menu.AppendMenu(MF_SEPARATOR);
	
	// 风格设置
	//menu.AppendMenu(MF_STRING, ID_SETTING, _T("风格设置"));
	menu.AppendODMenu(L"风格设置", MF_STRING, ID_SETTING);
	
	// 版面布局
	//menu.AppendMenu(MF_STRING, ID_LAYOUT_ADJUST, _T("版面布局"));
	menu.AppendODMenu(L"版面布局", MF_STRING, ID_LAYOUT_ADJUST);

	menu.LoadToolBar(g_awToolBarIconIDs);

	// 如果处在锁定分割状态，需要删除一些按钮
	CMPIChildFrame *pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pChildFrame && pChildFrame->IsLockedSplit() )
	{
		pChildFrame->RemoveSplitMenuItem(menu);
	}

	int32 iRet = menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_TOPALIGN |TPM_RETURNCMD |TPM_NONOTIFY, pos.x, pos.y, AfxGetMainWnd());
	if ( iRet > 0 && iRet < iCmd )
	{
		if ( iRet == 1 )
		{
			CCellRange rangeSel = m_GridCtrl.GetSelectedCellRange();
			int32 iBlockId;
			if ( rangeSel.IsValid() && (iBlockId = m_GridCtrl.GetItemData(rangeSel.GetMinRow(), 0)) != CBlockInfo::GetInvalidId() )
			{
				OnDblClick(iBlockId);
			}
		}
		else if ( 2 ==  iRet )
		{
			m_GridCtrl.ShowGridLine(bShowGrid ? GVL_NONE : GVL_BOTH);
			m_GridCtrl.Refresh();
		}
		else
		{
			iRet -= 3;
			if ( iRet >=0 && iRet < m_aTabInfomations.GetSize() )
			{
				SetTab(iRet);
			}
		}
	}
	else
	{
		// 普通功能
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, MAKEWPARAM(iRet, 0), 0);
	}
}

CMerch* CIoViewBlockReport::GetBlockMerchXml()
{
	CMerch *pMerch = NULL;
	CCellRange rangeSel = m_GridCtrl.GetSelectedCellRange();
	int32 iBlockId;
	if ( rangeSel.IsValid() && (iBlockId = m_GridCtrl.GetItemData(rangeSel.GetMinRow(), 0)) != CBlockInfo::GetInvalidId() )
	{
		CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(iBlockId);
		if ( NULL != pBlock && pBlock->m_blockInfo.m_pMerch )
		{
			pMerch = pBlock->m_blockInfo.m_pMerch;
		}
	}
	if ( NULL == pMerch )
	{
		pMerch = CIoViewBase::GetMerchXml();
	}
	return pMerch;
}

CMerch * CIoViewBlockReport::GetMerchXml()
{
	CMerch *pMerch = NULL;
	CCellRange rangeSel = m_GridCtrl.GetSelectedCellRange();
	int32 iBlockId;
	if ( rangeSel.IsValid() && (iBlockId = m_GridCtrl.GetItemData(rangeSel.GetMinRow(), 0)) != CBlockInfo::GetInvalidId() )
	{
		CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(iBlockId);
		if ( NULL != pBlock && !pBlock->m_logicBlockData.m_StrMerchName.IsEmpty() )
		{
			pMerch = pBlock->m_blockInfo.FindMerchByCnName(pBlock->m_logicBlockData.m_StrMerchName);
		}
	}
	if ( NULL == pMerch )
	{
		pMerch = CIoViewBase::GetMerchXml();
	}
	return pMerch;
}

void CIoViewBlockReport::GetAttendMarketDataArray( OUT AttendMarketDataArray &aAttends )
{
	aAttends.RemoveAll();
	CBlockCollection *pCollection = GetCurrentCollection();
	CBlockConfig::BlockArray aSubBlocks;
	if ( NULL == pCollection )
	{
		// 是否为所有的
		int32 iTab;
		if ( TabIsValid(iTab) && m_aTabInfomations[iTab].m_iBlockCollectionId == T_BlockReportTab::BCAll )
		{
			ASSERT( 0 == iTab );
			for ( int i=1; i < m_aTabInfomations.GetSize() ; i++ )
			{
				IdArray aIds;
				CBlockCollection *pTmpCollection = CBlockConfig::Instance()->GetBlockCollectionById(m_aTabInfomations[i].m_iBlockCollectionId);
				ASSERT( NULL != pTmpCollection );
				if ( NULL != pTmpCollection )
				{
					aSubBlocks.Append(pTmpCollection->m_aBlocks);
				}
			}
		}
		else
		{
			ASSERT( 0 );
		}
	}
	else
	{
		aSubBlocks.Copy(pCollection->m_aBlocks);
	}
	if ( aSubBlocks.GetSize() > 0 )
	{
		typedef CMap<int32, int32, int32, int32> MarketMap;
		MarketMap ms;
		T_AttendMarketData data;
		for ( int32 i=0; i < aSubBlocks.GetSize() ; i++ )
		{
			CBlockLikeMarket *pBlock = aSubBlocks[i];
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
						POSITION pos = pBlock->m_blockInfo.m_mapSubMerchMarkets.GetStartPosition();
						while ( NULL != pos )
						{
							int32 iMarket, iValue;
							pBlock->m_blockInfo.m_mapSubMerchMarkets.GetNextAssoc(pos, iMarket, iValue);
							ASSERT( iValue > 0 );
							ms[iMarket] = EDSTGeneral;
						}
					}
					break;
				default:
					ASSERT( 0 );
				}
			}
		}

		POSITION pos = ms.GetStartPosition();
		while ( NULL != pos )
		{
			int32 iMarket, iDT;
			ms.GetNextAssoc(pos, iMarket, iDT);

			data.m_iMarketId = iMarket;
			data.m_iEDSTypes = iDT;
			aAttends.Add(data);
		}
	}
}

CBlockCollection * CIoViewBlockReport::GetCurrentCollection()
{
	int32 iTab;
	if ( TabIsValid(iTab) )
	{
		CBlockCollection *pCollection = CBlockConfig::Instance()->GetBlockCollectionById(m_aTabInfomations[iTab].m_iBlockCollectionId);
		return pCollection;
	}
	return NULL;
}

void CIoViewBlockReport::SortColumn( int32 iSortCol/*=-1*/, int32 iSortType/*=0*/ )
{
	CStringArray aCols;
	CBlockDataPseudo::GetDefaultColNames(aCols);
	if ( iSortCol >= 0 && iSortCol < aCols.GetSize() )
	{
		CCellID cell(0, iSortCol);
		DoFixedRowClickCB(cell, iSortType);
	}
}

void CIoViewBlockReport::DoFixedRowClickCB( CCellID& cell, int32 iSortType /*= 0*/ )
{
	// 数据全申请，不做优化
    if (!m_GridCtrl.IsValid(cell) )
	{
		return;
	}

	m_iSortColumnFromXml = -1;
	m_iSortTypeFromXml = 0;	// 重置xml中的排序标记
	
	if ( cell.col == 0 )
	{
		if ( m_iSortColumn != -1 )
		{
			// 还原
			ResetBlockShowData();
		}
		m_iSortColumn = -1;
		return;
	}
	// 序号还原顺序，名称本地，其它服务器
	// 自选股? 所有字段都是本地排	自选股需要跟踪所有国内证券数据，然后计算
	// 调用表格类的排序	
	bool32 bUseLocalSort = false;
	
	if ( 1 == cell.col )	
	{
		// 名称 本地
		bUseLocalSort = true;
	}
	else if ( CBlockConfig::Instance()->IsUserCollection(m_iBlockClassId) )
	{
		// 自选股 本地
		bUseLocalSort = true;
	}
	else
	{
		// 均涨幅
		// 权涨幅
		// 换手率
		// 市盈率 - 有排行数据，其它的
		
		
		// 检查表头
		CBlockDataPseudo::E_Header eHeader = (CBlockDataPseudo::E_Header)m_GridCtrl.GetItemData(0, cell.col);
		ASSERT( eHeader >= CBlockDataPseudo::ShowRowNo && eHeader < CBlockDataPseudo::HeaderCount );
		E_BlockSort eSortHeader;
		if ( CBlockDataPseudo::ConvertNativeHeaderToSortHeader(eHeader, eSortHeader) )	// 需要服务器数据的字段
		{
			// 其它, 清除显示block, 从服务器申请数据， 等待从服务器数据回来，在设置
			// 从0开始显示
			if ( m_iSortColumn != -1 && m_iSortColumn != cell.col )	// 排序前，必须先将GridCtrl的序列清空
			{
				//m_GridCtrl.DeleteNonFixedRows();	// 虽然麻烦，还是调用方法
				//m_GridCtrl.SetRowCount(m_GridCtrl.GetFixedRowCount() + m_aShowBlockIds.GetSize());
				ResetBlockShowData(false);	// 还原到未排序状态
			}
			
			// 也可以不清除数据，只申请，等待数据回来，然后替换显示数据，其它部分清除
			for ( int i=0; i < m_aShowBlockIds.GetSize() ; i++ )
			{
				m_aShowBlockIds[i] = CBlockInfo::GetInvalidId();
			}
			
			SetTimer(KRequestSortDataTimerId, KRequestSortDataTimerPeriod, NULL);	// 启动定时器请求排行
			
			m_MmiRequestBlockSort.m_eBlockSortType = eSortHeader;
			if ( 0 == iSortType )
			{
				m_MmiRequestBlockSort.m_bDescSort = m_iSortColumn == cell.col ? !m_MmiRequestBlockSort.m_bDescSort : TRUE;
			}
			else
			{
				m_MmiRequestBlockSort.m_bDescSort = iSortType > 0 ? TRUE : FALSE;	// > 0 降序; <0 升序
			}
			
			m_bRequestViewSort = true;
			m_iSortColumn = cell.col;
			
			//m_GridCtrl.EnsureVisible(m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetFixedColumnCount());
			//m_GridCtrl.EnsureTopLeftCell(m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetFixedColumnCount());
			m_GridCtrl.EnsureVisible(m_GridCtrl.GetFixedRowCount(), cell.col);	// 保证第一行可见
			m_GridCtrl.Refresh();
			
			RequestSortData();
		}
		else
		{
			bUseLocalSort = true;		// 其它任意字段使用表格本地排序
		}
	}
	
	if ( bUseLocalSort )
	{
		if ( m_iSortColumn != -1 && m_iSortColumn != cell.col )		// 如果是从服务器排序字段切换到这里，需要先填充数据
		{
			ResetBlockShowData(false);
			//m_GridCtrl.EnsureVisible(m_GridCtrl.GetFixedRowCount(), cell.col);	// 第一行可见
		}
		m_bRequestViewSort = false;
		m_iSortColumn = -1;
		
		m_spThis = this;
		m_GridCtrl.SetHeaderSort(TRUE);
		
		m_iSortColumn = cell.col;
		
		CPoint pt(0,0);
		m_GridCtrl.OnFixedRowClick(cell,pt);
		
		m_GridCtrl.Refresh();
	}
}

void CIoViewBlockReport::WatchDayChange(bool32 bDrawIfChange /*= false*/)
{
	if(NULL == m_pAbsCenterManager)
	{
		return;
	}

	CGmtTime timeNow = m_pAbsCenterManager->GetServerTime();
	tm tmLocal;
	timeNow.GetLocalTm(&tmLocal);
	CMerch *pMerchSZA = NULL;
	if ( m_pAbsCenterManager->GetMerchManager().FindMerch(_T("000001"), 0, pMerchSZA) )
	{
		CMarketIOCTimeInfo ioc;
		pMerchSZA->m_Market.GetRecentTradingDay(timeNow, ioc, pMerchSZA->m_MerchInfo);
		ioc.m_TimeInit.m_Time.GetLocalTm(&tmLocal);	// 使用交易日信息
	}
	if ( m_tmDisplay.tm_yday != tmLocal.tm_yday )
	{
		m_tmDisplay = tmLocal;
		if ( bDrawIfChange )
		{
			InvalidateRect(m_RectTitle);
		}
	}
}

void CIoViewBlockReport::DoLocalSort()
{
	if ( -1 != m_iSortColumn
		&& !m_bRequestViewSort
		&& m_iSortColumn > 1 )	// 非代码本地排序列
	{
		m_spThis = this;
		m_GridCtrl.SortItems(m_iSortColumn, m_GridCtrl.GetSortAscending());
	}
}

//////////////////////////////////////////////////////////////////////////
//
CIoViewBlockReport::T_BlockReportTab::T_BlockReportTab( const T_BlockReportTab &tab )
{
	m_iBlockCollectionId = tab.m_iBlockCollectionId;
	m_iColLeftVisible = tab.m_iColLeftVisible;
	m_iStartRow = tab.m_iStartRow;
	m_iRowCount = tab.m_iRowCount;
	m_StrName = tab.m_StrName;
	m_aSubBlockIds.Copy(tab.m_aSubBlockIds);
}

const CIoViewBlockReport::T_BlockReportTab & CIoViewBlockReport::T_BlockReportTab::operator=( const T_BlockReportTab &tab )
{
	if ( this == &tab )
	{
		return *this;
	}
	m_iBlockCollectionId = tab.m_iBlockCollectionId;
	m_iColLeftVisible = tab.m_iColLeftVisible;
	m_iStartRow = tab.m_iStartRow;
	m_iRowCount = tab.m_iRowCount;
	m_StrName = tab.m_StrName;
	m_aSubBlockIds.Copy(tab.m_aSubBlockIds);
	return *this;
}
