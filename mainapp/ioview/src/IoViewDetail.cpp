#include "stdafx.h"
#include "memdc.h"
#include "IoViewManager.h"
#include "IoViewKLine.h"
#include "GridCellSymbol.h"
#include "facescheme.h"
#include "ReportScheme.h"
#include "ShareFun.h"
#include "IoViewTimeSale.h"
#include "MPIChildFrame.h"
#include "GridCellLevel2.h" 
#include "MerchManager.h"
#include "MPIChildFrame.h"
#include "dlgusercycleset.h"
#include "IoViewBase.h"
#include "IoViewDetail.h"

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif 
IMPLEMENT_DYNCREATE(CIoViewDetail, CIoViewBase)
///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewDetail, CIoViewBase)
//{{AFX_MSG_MAP(CIoViewDetail)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(UM_IoViewTitle_Button_LButtonDown, OnMessageTitleButton)
	ON_COMMAND_RANGE(IDM_CHART_CYCLE_BEGIN, IDM_CHART_CYCLE_END, OnMenu)
	ON_COMMAND_RANGE(IDM_IOVIEW_DETAIL_TREND, IDM_IOVIEW_DETAIL_SANE, OnIndexMenu)	
//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////
const int32 KiTitleTextRows			   = 6;						// 标题文字的行数	
const int32 KiTitleTextCols			   = 4;						// 标题文字的列数			
const int32 KiInitialReqKLineNums	   = 300;					// K线的条数	
				
const int32 KTimerIDUpdateKLine1Sane  = 504;					// 定时器1: 第一条K 线的稳健指标定时器
const int32 KTimerIDUpdateKLine2Sane  = 505;					// 定时器2: 第二条K 线的稳健指标定时器 
const int32 KTimerIDUpdateKLine3Sane  = 506;					// 定时器3: 第三条K 线的稳健指标定时器

const int32 KTimerUpdatePeriod		  = 1000;					// 更新时间

const char* KStrElementAttriCurIndex  = "CurIndex";				// 当前显示的 K 线
const char* KStrElementKLine		  = "Kline";				// K 线
const char* KStrElementAttriInterval  = "Interval";				// 周期
const char* KStrElementAttriUserMins  = "UserMins";				// 用户自定义分钟
const char* KStrElementAttriUserDays  = "UserDays";				// 用户自定义日线

// 
const int32 KMaxMinuteKLineCount	  = 10000;					// 最大允许的分钟K线数据， 不加限制的话， 连续开着软件会无限增长

//
const int32 KiStatisticaKLineNums	  = 200;					// 统计的K 线个数

//
CIoViewDetail::CIoViewDetail()
:CIoViewBase()
{
	m_iRowHeight  = -1;
	m_RectText	  = CRect(0,0,0,0);
	m_RectGrid	  = CRect(0,0,0,0);
	
	m_bActive	  = false;
	m_iCurShow	  = 0;
	m_TimePre	  = CTime(0);

	//
 	m_aIoViewDetailCells[0].m_eNodeInterval = ENTIMinute5;
 	m_aIoViewDetailCells[1].m_eNodeInterval = ENTIMinute60;
 	m_aIoViewDetailCells[2].m_eNodeInterval = ENTIDay;
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewDetail::~CIoViewDetail()
{

}

///////////////////////////////////////////////////////////////////////////////
BOOL CIoViewDetail::PreTranslateMessage(MSG* pMsg)
{	
	if ( WM_RBUTTONDOWN == pMsg->message )
	{
		// 右键菜单:
		CNewMenu Menu;
 		Menu.LoadMenu(IDR_MENU_DETAIL);
		Menu.LoadToolBar(g_awToolBarIconIDs);

 		CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, Menu.GetSubMenu(0));
 		
 		//
 		CMenu* pTempMenu = pPopMenu->GetSubMenu(L"插入内容");
 		CNewMenu * pIoViewPopMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );
 		ASSERT(NULL != pIoViewPopMenu );
 		AppendIoViewsMenu(pIoViewPopMenu, IsLockedSplit());
		
		// 自选股
 		
 		//
 		pTempMenu = pPopMenu->GetSubMenu(_T("选择周期")); 
 		ASSERT(NULL!=pTempMenu);
 		CNewMenu* pSubMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );
 		
 		int32 iCurCheck = IDM_CHART_KMINUTE + m_aIoViewDetailCells[m_iCurShow].m_eNodeInterval;
 		pSubMenu->CheckMenuItem(iCurCheck,MF_BYCOMMAND|MF_CHECKED); 
 
		pPopMenu->ModifyODMenu(L"选择周期", L"选择周期", IDB_TOOLBAR_KLINECYLE);

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

		return TRUE;
	}

	return CIoViewBase::PreTranslateMessage(pMsg);
}

// OnPaint
void CIoViewDetail::OnPaint()
{
	CPaintDC dcPaint(this); 

	if ( !IsWindowVisible() )
	{
		return;
	}

	//
	if ( GetParentGGTongViewDragFlag() ||  m_bDelByBiSplitTrack )
	{
		LockRedraw();
		return;
	}

	// 锁定绘图
	UnLockRedraw();
	
	//
	CRect rectClient;
	GetClientRect(&rectClient);

	CRect rectGrid;
	m_GridCtrl.GetWindowRect(rectGrid);
	ScreenToClient(&rectGrid);

	CRect rectDraw(rectClient);
	rectDraw.bottom = rectGrid.top;
	CMemDC dc(&dcPaint, rectDraw);
	
	COLORREF clrGridLine = GetIoViewColor(ESCChartAxisLine);
	CBrush brush(clrGridLine);
	
	CBrush* pOldBrush = dc.SelectObject(&brush);
	rectDraw = m_RectGrid;
	rectDraw.left -= 1;
	dc.FrameRect(rectDraw,&brush);
	dc.SelectObject(pOldBrush);
	brush.DeleteObject();
	
	if ( m_RectGrid.right < rectClient.right)
	{
		CRect RectRight = rectClient;
		RectRight.left	= m_RectGrid.right;
		
		dc.FillSolidRect(&RectRight, GetIoViewColor(ESCBackground));
	}
	
	if ( m_RectGrid.bottom < rectClient.bottom)
	{
		CRect RectBottom = rectClient;
		RectBottom.top = m_RectGrid.bottom;
		
		dc.FillSolidRect(&RectBottom, GetIoViewColor(ESCBackground));
	}
	
	CRect RectUp  = rectClient;
	RectUp.bottom = m_RectText.bottom;

	//
	dc.FillSolidRect(&RectUp, GetIoViewColor(ESCBackground));
	
	// 设标题	
	SetViewDetailString(dc);

	//
	m_GridCtrl.RedrawWindow();
}

BOOL CIoViewDetail::TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return FALSE;
}

int CIoViewDetail::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CIoViewBase::OnCreate(lpCreateStruct);

	// 获得颜色和字体
	InitialIoViewFace(this);
	CreateTable();

	return 0;
}

bool32 CIoViewDetail::CreateTable()
{
	// 尝试创建表格
	if (NULL == m_GridCtrl.GetSafeHwnd())
	{
		if (!m_GridCtrl.Create(CRect(0, 0, 0, 0), this, 20205))
			return false;
		
		m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10203);
		m_XSBVert.SetScrollRange(0, 10);
		
		m_XSBHorz.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10204);
		m_XSBHorz.SetScrollRange(0, 0);
		m_XSBHorz.ShowWindow(SW_HIDE);
	
		m_XSBHorz.SetOwner(&m_GridCtrl);
		m_XSBVert.SetOwner(&m_GridCtrl);
		m_GridCtrl.SetScrollBar(&m_XSBHorz, &m_XSBVert);
		
		// 清空表格内容
		m_GridCtrl.DeleteAllItems();
		m_GridCtrl.EnableSelection(false);
		m_GridCtrl.ExpandColumnsToFit();

		//
		m_GridCtrl.SetColumnCount(3);
		m_GridCtrl.SetRowCount(100);

		//
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	}
			
	// 设置行高
	SetRowHeightAccordingFont();
	return true;
}

void CIoViewDetail::OnLButtonDown(UINT nFlags, CPoint point)
{	
	for ( int32 i = 0 ; i < KLINE_NUMS; i++)
	{
		if ( m_aIoViewDetailCells[i].m_RectShow.PtInRect(point))
		{
			m_iCurShow = i;
			UpdateTableContent(3);
			RedrawWindow();
			break;
		}
	}
}

void CIoViewDetail::DealWithSpecial(const T_SaneIndexNode& Node, const CString& StrTime, const CString& StrPrice)
{
	if ( NULL == m_pMerchXml )
	{
		return;
	}
	
	int32 iRowCount = m_GridCtrl.GetRowCount();
	ASSERT(iRowCount>=1);
		
	bool32 bFuture = CReportScheme::Instance()->IsFuture(m_pMerchXml->m_Market.m_MarketInfo.m_eMarketReportType);

	if ( ESISDCKO == Node.m_eSaneIndexState )
	{
		// 多清空开, 第一列设置为空头清仓
		E_SysColor eColor = ESCKeep;
		
		// 时间:
		CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, 0);
		if ( NULL == pCell )
		{
			return;
		}
		
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		pCell->SetDefaultTextColor(eColor);
		pCell->SetText(StrTime);
		
		// 状态:
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, 1);
		if ( NULL == pCell )
		{
			return;
		}
		pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		
		T_SaneIndexNode NodeTemp = Node;
		NodeTemp.m_eSaneIndexState = ESISDTCC;
		
		CString StrPrompt = GetNodeStateString(bFuture, NodeTemp);
		pCell->SetDefaultTextColor(eColor);
		pCell->SetText(StrPrompt);
		
		// 价格
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, 2);
		if ( NULL == pCell )
		{
			return;
		}
		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		pCell->SetDefaultTextColor(eColor);
		pCell->SetText(StrPrice);
		
		// 插一行空头开仓的:
		m_GridCtrl.InsertRow(L"", 0);
		
		eColor = ESCFall;
		
		// 时间:
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, 0);
		if ( NULL == pCell )
		{
			return;
		}
		
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		pCell->SetDefaultTextColor(eColor);
		pCell->SetText(StrTime);
		
		// 状态:
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, 1);
		if ( NULL == pCell )
		{
			return;
		}
		pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		
		T_SaneIndexNode NodeTemp2 = Node;
		NodeTemp2.m_eSaneIndexState = ESISKTOC;
		
		StrPrompt = GetNodeStateString(bFuture, NodeTemp2);
		pCell->SetDefaultTextColor(eColor);
		pCell->SetText(StrPrompt);
		
		// 价格
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, 2);
		if ( NULL == pCell )
		{
			return;
		}
		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		pCell->SetDefaultTextColor(eColor);
		pCell->SetText(StrPrice);
	}
	else if ( ESISKCDO == Node.m_eSaneIndexState )
	{
		// 空清多开
		E_SysColor eColor = ESCKeep;
		
		// 时间:
		CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, 0);
		if ( NULL == pCell )
		{
			return;
		}
		
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		pCell->SetDefaultTextColor(eColor);
		pCell->SetText(StrTime);
		
		// 状态:
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, 1);
		if ( NULL == pCell )
		{
			return;
		}
		pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		
		T_SaneIndexNode NodeTemp = Node;
		NodeTemp.m_eSaneIndexState = ESISKTCC;
		
		CString StrPrompt = GetNodeStateString(bFuture, NodeTemp);
		pCell->SetDefaultTextColor(eColor);
		pCell->SetText(StrPrompt);
		
		// 价格
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, 2);
		if ( NULL == pCell )
		{
			return;
		}
		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		pCell->SetDefaultTextColor(eColor);
		pCell->SetText(StrPrice);
		
		// 插一行多头开仓的:
		m_GridCtrl.InsertRow(L"", 0);
		
		eColor = ESCRise;
		
		// 时间:
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, 0);
		if ( NULL == pCell )
		{
			return;
		}
		
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		pCell->SetDefaultTextColor(eColor);
		pCell->SetText(StrTime);
		
		// 状态:
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, 1);
		if ( NULL == pCell )
		{
			return;
		}
		pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		
		T_SaneIndexNode NodeTemp2 = Node;
		NodeTemp2.m_eSaneIndexState = ESISDTOC;
		
		StrPrompt = GetNodeStateString(bFuture, NodeTemp2);
		pCell->SetDefaultTextColor(eColor);
		pCell->SetText(StrPrompt);
		
		// 价格
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, 2);
		if ( NULL == pCell )
		{
			return;
		}
		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		pCell->SetDefaultTextColor(eColor);
		pCell->SetText(StrPrice);
	}
}

void CIoViewDetail::UpdateOneRow(int32 iUpdateFlag, int32 iRow, int32 iDataIndex)
{
	// 更新一行数据:

	if ( !m_GridCtrl.GetSafeHwnd())
	{
		return;
	}

	if ( iRow < 0 || iRow >= m_GridCtrl.GetRowCount() )
	{
		return;
	}
	
	if ( m_iCurShow < 0 || m_iCurShow >= KLINE_NUMS )
	{
		return;
	}

	if ( iDataIndex < 0 )
	{
		return;
	}

	CKLine* pKLine = (CKLine*)m_aIoViewDetailCells[m_iCurShow].m_aKLines.GetData();
	
	if ( NULL == pKLine )
	{
		return;
	}
	
	int32 iSize = m_aIoViewDetailCells[m_iCurShow].m_aKLines.GetSize();

	{
		T_SaneIndexNode* pSaneIndex   = (T_SaneIndexNode*)m_aIoViewDetailCells[m_iCurShow].m_aSaneIndexValues.GetData();
		
		if ( NULL == pSaneIndex || m_aIoViewDetailCells[m_iCurShow].m_aSaneIndexValues.GetSize() != iSize || iDataIndex >= iSize )
		{
			// 指标的数据跟K 线的对不上
			return;
		}
 		
  		if ( ESISDTCY == pSaneIndex[iDataIndex].m_eSaneIndexState || ESISKTCY == pSaneIndex[iDataIndex].m_eSaneIndexState
  		  || ESISKCGW == pSaneIndex[iDataIndex].m_eSaneIndexState || ESISNONE == pSaneIndex[iDataIndex].m_eSaneIndexState)
  		{
			if ( iUpdateFlag != 1)
			{
				m_GridCtrl.DeleteRow(iRow);
			}
  			return;
  		}
		
		m_GridCtrl.SetRowHeight(iRow, m_iRowHeight);
	
		// 时间
		CString StrTime;
		CTime Time(pKLine[iDataIndex].m_TimeCurrent.GetTime());
		
		if ( m_aIoViewDetailCells[m_iCurShow].m_eNodeInterval == ENTIDay
		  || m_aIoViewDetailCells[m_iCurShow].m_eNodeInterval == ENTIDayUser 
		  || m_aIoViewDetailCells[m_iCurShow].m_eNodeInterval == ENTIWeek 
		  || m_aIoViewDetailCells[m_iCurShow].m_eNodeInterval == ENTIMonth 
		  || m_aIoViewDetailCells[m_iCurShow].m_eNodeInterval == ENTIQuarter 
		  || m_aIoViewDetailCells[m_iCurShow].m_eNodeInterval == ENTIYear)
		{
			StrTime.Format(L"%02d-%02d", Time.GetMonth(), Time.GetDay());
		}
		else
		{
			if ( iSize >= 2)
			{
				// 表格前一项的时间:
			
				if ( m_TimePre.GetTime() <= 0 )
				{					
					StrTime.Format(L"%02d:%02d %02d/%02d",Time.GetHour(), Time.GetMinute(), Time.GetDay(), Time.GetMonth());
				}
				else
				{
					if ( Time.GetYear() == m_TimePre.GetYear() && Time.GetMonth() == m_TimePre.GetMonth() && Time.GetDay() == m_TimePre.GetDay() )
					{
						StrTime.Format(L"%02d:%02d", Time.GetHour(), Time.GetMinute());	
					}
					else
					{
						StrTime.Format(L"%02d:%02d %02d/%02d",Time.GetHour(), Time.GetMinute(), Time.GetDay(), Time.GetMonth());						
					}					
				}
			}
			else
			{
				StrTime.Format(L"%02d:%02d", Time.GetHour(), Time.GetMinute());
			}			
		}
			
		// 价格
		CString StrPrice;
		StrPrice = Float2String(pKLine[iDataIndex].m_fPriceClose, m_pMerchXml->m_MerchInfo.m_iSaveDec);

		//
 		if ( ESISDCKO == pSaneIndex[iDataIndex].m_eSaneIndexState || ESISKCDO == pSaneIndex[iDataIndex].m_eSaneIndexState )
 		{
 			// 多清空开 或者 空清多开
			DealWithSpecial(pSaneIndex[iDataIndex], StrTime, StrPrice);
 		}
 		else
 		{
			E_SysColor eColor = ESCKeep;
			
			if ( ESISDTOC == pSaneIndex[iDataIndex].m_eSaneIndexState || ESISDTCY == pSaneIndex[iDataIndex].m_eSaneIndexState )
			{
				eColor = ESCRise;
			}
			else if ( ESISKTOC == pSaneIndex[iDataIndex].m_eSaneIndexState || ESISKTCY == pSaneIndex[iDataIndex].m_eSaneIndexState )
			{		
				eColor = ESCFall;
			}
			
			// 时间:
			CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, 0);
			if ( NULL == pCell )
			{
				return;
			}			
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);			
			pCell->SetDefaultTextColor(eColor);
			pCell->SetText(StrTime);
			
			// 状态:
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, 1);
			if ( NULL == pCell )
			{
				return;
			}
			pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			
			CString StrPrompt;
			
			// 趋势指标
			bool32 bFuture = CReportScheme::Instance()->IsFuture(m_pMerchXml->m_Market.m_MarketInfo.m_eMarketReportType);

			StrPrompt = GetNodeStateString(bFuture, pSaneIndex[iDataIndex]);		
			
			pCell->SetDefaultTextColor(eColor);
			pCell->SetText(StrPrompt);
			
			// 价格
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, 2);
			if ( NULL == pCell )
			{
				return;
			}
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);			
			pCell->SetDefaultTextColor(eColor);
			pCell->SetText(StrPrice);			
		}

		// 保存时间
		m_TimePre = Time;
	}
}

void CIoViewDetail::UpdateTableContent(int32 iUpdateFlag)
{
	if ( m_bDelByBiSplitTrack )
	{
		LockRedraw();
	}
	else
	{
		UnLockRedraw();
	}

	//
	if ( GetParentGGTongViewDragFlag() )
	{
		LockRedraw();
	}
	else
	{
		UnLockRedraw();
	}

	// 1 : 最新一个变化了 2: 增加了一个 3: 全部更新
	if ( m_iCurShow < 0 || m_iCurShow >= KLINE_NUMS )
	{
		m_iCurShow = 0;
	}
	
	CKLine* pKLine = (CKLine*)m_aIoViewDetailCells[m_iCurShow].m_aKLines.GetData();
	if ( NULL == pKLine )
	{
		RequestViewData();
		return;
	} 

	int32 iKLineSize = m_aIoViewDetailCells[m_iCurShow].m_aKLines.GetSize();

	//////////////////////////////////////////////////////////////////////////	
	if ( 1 == iUpdateFlag )
	{
		// 更新最新一行
		UpdateOneRow(iUpdateFlag, 0, iKLineSize-1);
	}
	else if ( 2 == iUpdateFlag )
	{
		// 插一行
		int32 iRow = m_GridCtrl.GetRowCount();
		
		if ( 0 == iRow )
		{
			m_GridCtrl.InsertRow(L"");
		}
		else
		{
			m_GridCtrl.InsertRow(L"", 0);
		}
		//
		UpdateOneRow(iUpdateFlag, 0, iKLineSize-1);
	}
	else if ( 3 == iUpdateFlag )
	{
		// 更新整个表的内容
		m_XSBVert.ShowWindow(SW_HIDE);
		m_GridCtrl.DeleteNonFixedRows();

		int32 i = 0;
		for ( i = (GetMaxMoveNums() + 1) ; i < iKLineSize; i++ )
		{
			float fPreClose = 0.0;
			
			if ( i >= 1 )
			{
				fPreClose = pKLine[i-1].m_fPriceClose;
			}
		
			int32 iRow = m_GridCtrl.GetRowCount();

			if ( 0 == iRow )
			{
				m_GridCtrl.InsertRow(L"");
			}
			else
			{
				m_GridCtrl.InsertRow(L"", 0);
			}

			UpdateOneRow(iUpdateFlag, 0, i);
		}

		int32 iTest  = m_GridCtrl.GetRowCount();
		
		if ( iTest > 500 )
		{
			iTest = 500;
		}

		for ( i = 0 ; i < iTest ; i ++ )
		{
			m_GridCtrl.SetRowHeight(i, m_iRowHeight);
		}
		
		// m_XSBVert.ShowWindow(SW_SHOW);
		
		// 由于gridctrl home键需要一个focuscell，所有这里默认一个focus cell
		m_GridCtrl.SetFocusCell(0, 0);
	}

	m_GridCtrl.Invalidate();
	m_GridCtrl.Refresh();
	Invalidate();
}

void CIoViewDetail::OnSize(UINT nType, int cx, int cy) 
{		 
	CIoViewBase::OnSize(nType,cx,cy);
	CRect rectClient;
	GetClientRect(&rectClient);

	// 设置字体
	SetRowHeightAccordingFont();

	// 标题栏下面文字区域
	int32 iRectTextHeight = KiTitleTextRows * (m_iRowHeight + 5);

	m_RectText			= rectClient;
	m_RectText.bottom	= m_RectText.top + iRectTextHeight;

	// 表格区域
	m_RectGrid			= rectClient;
	m_RectGrid.top		= m_RectText.bottom;

	//////////////////////////////////////////////////////////////////////////

	if ( m_GridCtrl.GetSafeHwnd() )
	{
		m_XSBVert.ShowWindow(SW_HIDE);
		m_GridCtrl.MoveWindow(&m_RectGrid);
		m_GridCtrl.ExpandColumnsToFit();
	}	
}

void CIoViewDetail::SetViewDetailString()
{
	CClientDC dc(this);
	SetViewDetailString(dc);
}

void CIoViewDetail::SetViewDetailString(CDC &dc)
{
	// 新增使用paint dc画相关文字  0001752
	if ( NULL == m_pMerchXml )
	{
		// 激活标志	
		if ( m_bActive )
		{
			CRect rectActive(m_iActiveXpos, m_iActiveYpos, m_iActiveXpos * 2, m_iActiveYpos * 2);
			//dc.FillSolidRect(&rectActive, m_ColorActive);		
		}

		return;
	}

	// 设置标题文字:
	CRect RectClient;
	GetClientRect(&RectClient);
	RectClient.bottom = m_RectText.bottom; // 背景只要到text就够了，不要刷新GridCtrl的部分

	dc.FillSolidRect(&RectClient, GetIoViewColor(ESCBackground));

	CFont* pOldFont = dc.SelectObject(GetIoViewFontObject(ESFNormal));
	int32  iOldBK	= dc.SetBkMode(TRANSPARENT);

	CString StrText		= L"";	
	COLORREF clrText	= GetIoViewColor(ESCText);
	COLORREF clrNum		= RGB(224, 128, 224);
	COLORREF clrTitle	= GetIoViewColor(ESCVolume);

	dc.SetTextColor(clrText);

	if ( m_RectText.Height() <= 10 || m_RectText.Width() <= 10 )
	{
		// 尺寸过小的时候,不要显示
		return;
	}
	
	int32 iHeightPerRow = m_RectText.Height() / KiTitleTextRows;			// 每行的高度,每个单元格的高,宽											// 横向的每个单元之间的间隔
	int32 iWidthAll	    = m_RectText.Width();								// 每列的宽度
	
	float fWidthCol0	= (float)iWidthAll*((float)6/19);	
	float fWidthCol1	= (float)iWidthAll*((float)4/19);
	float fWidthCol2	= (float)iWidthAll*((float)5/19);
	float fWidthCol3	= (float)iWidthAll*((float)4/19);

	int32 iWidthCol0	= (int32)fWidthCol0;
	int32 iWidthCol1	= (int32)fWidthCol1;
	int32 iWidthCol2	= (int32)fWidthCol2;
	int32 iWidthCol3	= (int32)fWidthCol3;

	CRect rectShow   = m_RectText;
	rectShow.bottom  = rectShow.top + iHeightPerRow;

	// 激活标志	
	if ( m_bActive )
	{
		CRect rectActive(m_iActiveXpos, m_iActiveYpos, m_iActiveXpos * 2, m_iActiveYpos * 2);
		//dc.FillSolidRect(&rectActive, m_ColorActive);		
	}

	// 第一行: 商品名称
	{
		//
		StrText.Format(L"%s", m_pMerchXml->m_MerchInfo.m_StrMerchCnName.GetBuffer());
	
		CRect rectTitle = rectShow;
		
		dc.SetTextColor(clrTitle);
		//dc.DrawText(StrText, &rectTitle, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		DrawMerchName(dc, m_pMerchXml, rectTitle, false);
		
		CSize sizeMerch;
		sizeMerch = dc.GetTextExtent(m_pMerchXml->m_MerchInfo.m_StrMerchCnName);

		//
		rectShow.top    += iHeightPerRow;
		rectShow.bottom += iHeightPerRow;
	}

	// 标题下面的线
	{
		CPen  penLine(PS_SOLID, 1, GetIoViewColor(ESCChartAxisLine));
		CPen* penOld = dc.SelectObject(&penLine);
		dc.MoveTo(rectShow.left, rectShow.top + 1);
		dc.LineTo(rectShow.right, rectShow.top+ 1);
		dc.SelectObject(penOld);
		penLine.DeleteObject();
	}
	
	int32 iSkip = 5;

	// 第二,三,四,行: 第二,三,四条K 线	
	for ( int32 i = 0; i < KLINE_NUMS; i++)
	{
		// 稳健型
		int32 iSize = m_aIoViewDetailCells[i].m_aSaneIndexValues.GetSize();
		
		bool32 bChecked = false;
		
		if ( m_iCurShow == i )			
		{
			bChecked = true;
		}
		
		if ( bChecked )
		{
			pOldFont = dc.SelectObject(GetIoViewFontObject(ESFBig));
		}		
		//
		
		COLORREF clr = GetIoViewColor(ESCKeep);
		T_SaneIndexNode NodeNow;
		bool32 bValidNode = false;
		
		if ( iSize >= 1 )
		{
			NodeNow = m_aIoViewDetailCells[i].m_aSaneIndexValues.GetAt(iSize-1);
			
			if ( ESMIDT == NodeNow.m_eSaneMarketInfo )
			{
				clr = GetIoViewColor(ESCRise);
			}
			else if ( ESMIKT == NodeNow.m_eSaneMarketInfo )
			{
				clr = GetIoViewColor(ESCFall);
			}
			
			StrText  = TimeInterval2String(m_aIoViewDetailCells[i].m_eNodeInterval, m_aIoViewDetailCells[i].m_iTimeUserMultipleMinutes, m_aIoViewDetailCells[i].m_iTimeUserMultipleDays);
			StrText += L" ";
			StrText += GetSaneNodeSummary(NodeNow);
			
			bValidNode = true;
		}
		
		dc.SetTextColor(clrText);	
		
		// 每条K 线前面的复选框:
		
		// 1: 复选框 ( 一个格子 )
		CRect rect00 = rectShow;
		rect00.right = rect00.left + iWidthCol0;
		
		m_aIoViewDetailCells[i].m_RectShow = rectShow;
		m_aIoViewDetailCells[i].m_RectShow.right = m_aIoViewDetailCells[i].m_RectShow.left + iHeightPerRow; 
		
		CPen PenCheckBox(PS_SOLID, 1, clrText);
		CPen PenChecked(PS_SOLID, 1, clrText);
		
		CPen* pOldPen = NULL;
		
		if ( bChecked )
		{
			pOldPen = dc.SelectObject(&PenChecked);
		}
		else
		{
			pOldPen = dc.SelectObject(&PenCheckBox);
		}
		
		//
		CRect rectFrame = m_aIoViewDetailCells[i].m_RectShow;
		
		rectFrame.top	 += iSkip;
		rectFrame.bottom -= iSkip;
		rectFrame.left   += iSkip;
		rectFrame.right  -= iSkip;
		
		if ( bChecked )
		{
			//dc.FillSolidRect(rectFrame, clrText);
		}
		
		dc.MoveTo(rectFrame.TopLeft());
		dc.LineTo(rectFrame.right, rectFrame.top);
		dc.LineTo(rectFrame.right, rectFrame.bottom);
		dc.LineTo(rectFrame.left, rectFrame.bottom);
		dc.LineTo(rectFrame.TopLeft());
		
		if ( bChecked )
		{
			CPen PenCheckSign(PS_SOLID, 2, clrNum);
			pOldPen = dc.SelectObject(&PenCheckSign);
			// 选中
			CRect RectSelect = rectFrame;
			RectSelect.top	  += (iSkip-2);
			RectSelect.bottom -= (iSkip-2);
			RectSelect.left   += (iSkip-2);
			RectSelect.right  -= (iSkip-2);
			
			dc.MoveTo(RectSelect.left , (RectSelect.top + RectSelect.bottom) / 2 );
			dc.LineTo((RectSelect.left + RectSelect.right)/2, RectSelect.bottom);
			dc.LineTo(RectSelect.right, RectSelect.top);
			
			dc.SelectObject(pOldPen);
			PenCheckSign.DeleteObject();
		}
		
		dc.SelectObject(pOldPen);
		
		if ( bChecked )
		{
			PenChecked.DeleteObject();						
		}
		else
		{
			PenCheckBox.DeleteObject();
		}
		
		
		// 2: 周期 ( 三个格子 )
		CRect rectCyle = rect00;				
		rectCyle.left  = m_aIoViewDetailCells[i].m_RectShow.right + 1;
		
		CString StrMax = L"200分钟";
		CSize sizeMaxCyle = dc.GetTextExtent(StrMax);
		if ( rectCyle.right > rectCyle.left + sizeMaxCyle.cx )
		{
			rectCyle.right = rectCyle.left + sizeMaxCyle.cx;
		}
		
		if ( bChecked )
		{
			dc.SetTextColor(clrTitle);
		}
		
		StrText = TimeInterval2String(m_aIoViewDetailCells[i].m_eNodeInterval, m_aIoViewDetailCells[i].m_iTimeUserMultipleMinutes, m_aIoViewDetailCells[i].m_iTimeUserMultipleDays);
		dc.DrawText(StrText, &rectCyle, DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
		
		//
		dc.SetTextColor(clr);	
		
		// 3: 强度: (两个格子)
		CRect rectIntensity = rectShow;
		rectIntensity.left  = rect00.right;
		rectIntensity.right = rectIntensity.left + iWidthCol1;
		if ( bValidNode )
		{
			StrText = GetSaneNodeIntensityString(NodeNow);
		}
		else
		{
			StrText = L"";
		}
		
		dc.DrawText(StrText, &rectIntensity, DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
		
		// 4: 稳定性: (三个格子)
		CRect rectStability = rectShow;
		rectStability.left  = rectIntensity.right;
		rectStability.right = rectStability.left  + iWidthCol2;
		if ( bValidNode )
		{
			StrText = GetSaneNodeStabilityString(NodeNow);
		}
		else
		{
			StrText = L"";
		}	
		
		dc.DrawText(StrText, &rectStability, DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
		
		// 5: 趋势: (四个格子)
		CRect rectTrend = rectShow;
		rectTrend.left  = rectStability.right ;
		rectTrend.right = rectTrend.left  + iWidthCol3;
		if ( bValidNode )
		{
			StrText = GetSaneNodeUpDownString(NodeNow);
		}
		else
		{
			StrText = L"";
		}
		
		dc.DrawText(StrText, &rectTrend, DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
		dc.SetTextColor(clrText);
		
		//
		rectShow.top    += iHeightPerRow;
		rectShow.bottom += iHeightPerRow;
		
		//		
		pOldFont = dc.SelectObject(GetIoViewFontObject(ESFNormal));		
	}

	CPen  penLine(PS_SOLID, 1, GetIoViewColor(ESCChartAxisLine));
	CPen* penOld = dc.SelectObject(&penLine);
	
	dc.MoveTo(m_RectText.left,  rectShow.top);
	dc.LineTo(m_RectText.right, rectShow.top);
	
	dc.MoveTo(m_RectText.left,  m_RectText.bottom-1);
	dc.LineTo(m_RectText.right, m_RectText.bottom-1);

	dc.SelectObject(penOld);
	penLine.DeleteObject();

	// 第五行: K 线个数和交易次数:
	if ( m_iCurShow < 0 || m_iCurShow >= KLINE_NUMS )
	{
		m_iCurShow = 0;
	}
	
	int32 iKLineNums = m_aIoViewDetailCells[m_iCurShow].m_iSaneStaticsNums;

	// K 线数
	CRect rectKLineText = rectShow;
	rectKLineText.right = rectKLineText.left + iWidthCol0;
	rectKLineText.left += iSkip;
	StrText = L" K 线数";
	dc.DrawText(StrText, &rectKLineText, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

	// 数目 
	CRect rectKLineNum = rectShow;
	rectKLineNum.left  = rectKLineText.right;
	rectKLineNum.right = rectKLineNum.left + iWidthCol1;
	StrText.Format(L"%d", iKLineNums);
	dc.SetTextColor(clrNum);
	dc.DrawText(StrText, &rectKLineNum, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
	dc.SetTextColor(clrText);

	// 交易数 
	CRect rectTradeText = rectShow;
	rectTradeText.left  = rectKLineNum.right;
	rectTradeText.right = rectTradeText.left + iWidthCol2;
	StrText = L"交易数";
	dc.DrawText(StrText, &rectTradeText, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

	// 数目 
	CRect rectTradeNum = rectShow;
	rectTradeNum.left  = rectTradeText.right;
	rectTradeNum.right = rectTradeNum.left + iWidthCol3;
	StrText.Format(L"%d", m_aIoViewDetailCells[m_iCurShow].m_iTradeTimes);
	dc.SetTextColor(clrNum);
	dc.DrawText(StrText, &rectTradeNum, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
	dc.SetTextColor(clrText);

	rectShow.top    += iHeightPerRow;
	rectShow.bottom += iHeightPerRow;

	// 第六行: 收益.准确率

	// 准确率文字 
	CRect rectAccracyText = rectShow;
	rectAccracyText.right = rectAccracyText.left + iWidthCol0;
	rectAccracyText.left += iSkip;
	StrText = L"准确率";
	dc.DrawText(StrText, &rectAccracyText, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

	// 准确率 (三个格子)
	CRect rectAccracyRate = rectShow;
	rectAccracyRate.left  = rectAccracyText.right;
	rectAccracyRate.right = rectAccracyRate.left + iWidthCol1;
	float fRate = (float)100 * m_aIoViewDetailCells[m_iCurShow].m_fAccuracyRate;	
	StrText.Format(L"%.2f%%",fRate);
	if ( fRate >= (float)50 )
	{
		dc.SetTextColor(GetIoViewColor(ESCRise));
	}
	else
	{
		dc.SetTextColor(clrNum);
	}

	dc.DrawText(StrText, &rectAccracyRate, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
	dc.SetTextColor(clrText);

	// 收益率文字 
	CRect rectProfitText = rectShow;
	rectProfitText.left  = rectAccracyRate.right;
	rectProfitText.right = rectProfitText.left  + iWidthCol2;
	StrText = L"收益率";
	dc.DrawText(StrText, &rectProfitText, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

	// 收益率 (三个格子)
	CRect rectProfitability = rectShow;
	rectProfitability.left  = rectProfitText.right;
	rectProfitability.right = rectProfitability.left + iWidthCol3;
	float fProfit = (float)100 * m_aIoViewDetailCells[m_iCurShow].m_fProfitability;
	if( fProfit >= 0.0 )
	{
		dc.SetTextColor(GetIoViewColor(ESCRise));
	}
	else
	{
		dc.SetTextColor(GetIoViewColor(ESCFall));
	}
	StrText.Format(L"%.2f%%",fProfit);
	dc.DrawText(StrText, &rectProfitability, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
	dc.SetTextColor(clrText);

	//
	dc.SelectObject(pOldFont);
	dc.SetBkMode(iOldBK);
}

void CIoViewDetail::SetRowHeightAccordingFont()
{
	if ( !m_GridCtrl.GetSafeHwnd() || m_GridCtrl.GetRowCount() <= 0) 
	{
		return;
	}

	CFont * pFontNormal = GetIoViewFontObject(ESFNormal);
	
	CClientDC dc(this);	
	CFont* pOldFont = dc.SelectObject(pFontNormal); // font为要得其高宽的字体	
	
	CSize size = dc.GetTextExtent(L"一二三四");
	m_iRowHeight = size.cy + 5;  
	
	// 第一行的
	CFont* pFontBig = GetIoViewFontObject(ESFBig);
	pOldFont = dc.SelectObject(pFontBig);
	size = dc.GetTextExtent(L"一二三四");
	m_GridCtrl.SetRowHeight(0, (size.cy + 2));

	// 其他行
	for (int32 i = 0 ; i < m_GridCtrl.GetRowCount() ; i++)
	{
		m_GridCtrl.SetRowHeight(i,m_iRowHeight);
	}
}

void CIoViewDetail::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	// 通知视图改变关注的商品
	if (NULL == pMerch || m_pMerchXml == pMerch)
		return;
	
	if (NULL != pMerch)
	{
		// 设置关注的商品信息
		m_aSmartAttendMerchs.RemoveAll();
		
		CSmartAttendMerch SmartAttendMerch;
		SmartAttendMerch.m_pMerch = pMerch;
		SmartAttendMerch.m_iDataServiceTypes = EDSTKLine;
		m_aSmartAttendMerchs.Add(SmartAttendMerch);
	}

	// 修改当前查看的商品
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;

	// 初始化的时候,把这个数组清空.切换商品的时候依据这个商品列表,判断是否已经算过最新值	
	for ( int32 i = 0 ; i < KLINE_NUMS; i++ )
	{
		m_aIoViewDetailCells[i].m_bNeedCalcSaneLatestValues = false; 
	
		m_aIoViewDetailCells[i].m_eHoldState				= EHSNONE;	
		m_aIoViewDetailCells[i].m_eActionPrompt				= EAPNONE;

		m_aIoViewDetailCells[i].m_iTradeTimes				= 0;		
		m_aIoViewDetailCells[i].m_fAccuracyRate				= 0.0;	
		m_aIoViewDetailCells[i].m_fProfitability			= 0.0;	
		
		m_aIoViewDetailCells[i].m_aSaneIndexValues.RemoveAll();
		m_aIoViewDetailCells[i].m_aMerchsHaveCalcLatestValues.RemoveAll();
	}

	//
	SetViewDetailString();
}

//
void CIoViewDetail::OnVDataForceUpdate()
{
	RequestViewData();	
}

void CIoViewDetail::OnIoViewFontChanged()
{
	// 设置字体值	
	
	CIoViewBase::OnIoViewFontChanged();

	SetRowHeightAccordingFont();
	m_GridCtrl.Refresh();
	m_GridCtrl.Invalidate();
}

void CIoViewDetail::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();
	SetViewDetailString();
	m_GridCtrl.Refresh();
	m_GridCtrl.Invalidate();
}

void CIoViewDetail::OnVDataRealtimePriceUpdate(IN CMerch *pMerch)
{

}

bool32 CIoViewDetail::FromXml(TiXmlElement * pElement)
{
	//
	SetFontsFromXml(pElement);
	SetColorsFromXml(pElement);
	SetRowHeightAccordingFont();
	//
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

	// 当前显示的K 线
	pcAttrValue = pElement->Attribute(KStrElementAttriCurIndex);
	if ( NULL != pcAttrValue )
	{
		m_iCurShow = atoi(pcAttrValue);
	}
	//

	// 权重信息
	pcAttrValue = pElement->Attribute(KIoViewKLineWeightType);
	if ( NULL != pcAttrValue )
	{
		m_eWeightType = (CIoViewBase::E_WeightType)atoi(pcAttrValue);
		CIoViewBase::SetWeightType(m_eWeightType);
	}

	// 读每条 K 线的配置信息
	TiXmlElement* pChildElement = pElement->FirstChildElement();
	
	if ( NULL != pChildElement )
	{
		if ( 0 == strcmp(KStrElementKLine, pChildElement->Value()))
		{
			pChildElement = pChildElement->FirstChildElement();

			int32 iIndex = 0;

			while ( NULL != pChildElement) 
			{
				if ( iIndex >= KLINE_NUMS )
				{
					break;
				}

				// 周期
				pcAttrValue = pChildElement->Attribute(KStrElementAttriInterval);
				
				if ( NULL != pcAttrValue )
				{
					m_aIoViewDetailCells[iIndex].m_eNodeInterval = (E_NodeTimeInterval)atoi(pcAttrValue);						
				}

				// 自定义分钟
				pcAttrValue = pChildElement->Attribute(KStrElementAttriUserMins);

				if ( NULL != pcAttrValue )
				{
					m_aIoViewDetailCells[iIndex].m_iTimeUserMultipleMinutes = atoi(pcAttrValue);
				}

				// 自定义日线
				pcAttrValue = pChildElement->Attribute(KStrElementAttriUserDays);
				
				if ( NULL != pcAttrValue )
				{
					m_aIoViewDetailCells[iIndex].m_iTimeUserMultipleDays = atoi(pcAttrValue);
				}
				
				//
				iIndex ++;
				pChildElement = pChildElement->NextSiblingElement();
			}
		}		 
	}

	// 商品发生改变
	OnVDataMerchChanged(iMarketId, StrMerchCode, pMerchFound);	
	return true;
}

CString CIoViewDetail::ToXml()
{
	CString StrThis;

	CString StrMarketId;
	StrMarketId.Format(L"%d", m_MerchXml.m_iMarketId);

	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%d\" %s = \"%d\" ", 
			CString(GetXmlElementValue()).GetBuffer(), 
			CString(GetXmlElementAttrIoViewType()).GetBuffer(),
			CIoViewManager::GetIoViewString(this).GetBuffer(),
			CString(GetXmlElementAttrShowTabName()).GetBuffer(), 
			m_StrTabShowName.GetBuffer(),
			CString(GetXmlElementAttrMerchCode()).GetBuffer(), 
			m_MerchXml.m_StrMerchCode.GetBuffer(),
			CString(GetXmlElementAttrMarketId()).GetBuffer(),
			StrMarketId.GetBuffer(),
			CString(KStrElementAttriCurIndex).GetBuffer(),
			m_iCurShow,
			CString(KIoViewKLineWeightType).GetBuffer(),
			CIoViewBase::GetWeightType());

	CString StrFace;
	StrFace  = SaveColorsToXml();
	StrFace += SaveFontsToXml();
	//
	StrThis += StrFace;
	//
	StrThis += L">\n";
	//
	CString StrKLine;
	StrKLine.Format(L"<%s>\n", CString(KStrElementKLine).GetBuffer());
	StrThis += StrKLine;

	for ( int32 i = 0 ; i < KLINE_NUMS; i++)
	{
		StrKLine.Format(L"<%s %s=\"%d\" %s=\"%d\" %s=\"%d\" />\n",
						CString(KStrElementKLine).GetBuffer(),
						CString(KStrElementAttriInterval).GetBuffer(), (int32)m_aIoViewDetailCells[i].m_eNodeInterval,
						CString(KStrElementAttriUserMins).GetBuffer(), m_aIoViewDetailCells[i].m_iTimeUserMultipleMinutes,
						CString(KStrElementAttriUserDays).GetBuffer(), m_aIoViewDetailCells[i].m_iTimeUserMultipleDays);
		StrThis += StrKLine;						
	}

	StrThis += L"</";
	StrThis += CString(KStrElementKLine);
	StrThis += L">\n";
	//
	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";

	return StrThis;
} 

CString CIoViewDetail::GetDefaultXML()
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

void CIoViewDetail::OnIoViewActive()
{
	m_bActive = IsActiveInFrame();

	//SetFocus();
	//OnVDataForceUpdate();
	RequestLastIgnoredReqData();

	m_GridCtrl.SetFocus();

	SetChildFrameTitle();
	RedrawWindow();
}

void CIoViewDetail::OnIoViewDeactive()
{
	m_bActive = false;
	RedrawWindow();
}

void CIoViewDetail::OnWeightTypeChange()
{
	RequestWeightData();

	// 更新所有数据
	OnVDataMerchKLineUpdate(m_pMerchXml);
}

void CIoViewDetail::LockRedraw()
{
	if ( m_bLockRedraw )
	{
		return;
	}

	m_bLockRedraw = true;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
	::SendMessage(m_GridCtrl.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
}

void CIoViewDetail::UnLockRedraw()
{
	if ( !m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = false;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
	::SendMessage(m_GridCtrl.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
}

void CIoViewDetail::OnTimer(UINT nIDEvent)
{	
	OnTimerUpdateLatestValuse(nIDEvent);	
	CIoViewBase::OnTimer(nIDEvent);
}
	
void CIoViewDetail::SetChildFrameTitle()
{
	CString StrTitle;
	if (NULL == m_pMerchXml)
	{
		StrTitle = CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;
	}
	else
	{		
		StrTitle = CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;
		StrTitle += L" ";
		StrTitle += m_pMerchXml->m_MerchInfo.m_StrMerchCnName;
	}
	
	CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pParentFrame)
	{
		pParentFrame->SetChildFrameTitle(StrTitle);
	}
}

//////////////////////////////////////////////////////////////////////////
void CIoViewDetail::RequestViewData()
{
// 	if ( !IsWindowVisible() )
// 	{
// 		return;
// 	}

	if ( NULL == m_pMerchXml || NULL == m_pAbsCenterManager)
	{
		return;
	}

	for ( int32 i = 0 ; i < KLINE_NUMS; i++)
	{
		E_NodeTimeInterval eNodeCompare		 = ENTIDay;
		E_KLineTypeBase	   eKLineTypeCompare = EKTBDay;
		int32			   iScale			 = 1;
		
		if ( !GetTimeIntervalInfo(m_aIoViewDetailCells[i].m_iTimeUserMultipleMinutes, m_aIoViewDetailCells[i].m_iTimeUserMultipleDays, m_aIoViewDetailCells[i].m_eNodeInterval, eNodeCompare, eKLineTypeCompare, iScale) )
		{
			continue;
		}
		 
		//
		CMmiReqMerchKLine infoself;	 
		
		infoself.m_iMarketId		= m_pMerchXml->m_MerchInfo.m_iMarketId;
		infoself.m_StrMerchCode		= m_pMerchXml->m_MerchInfo.m_StrMerchCode;
		infoself.m_eReqTimeType		= ERTYFrontCount;
		infoself.m_TimeSpecify		= m_pAbsCenterManager->GetServerTime();
		infoself.m_eKLineTypeBase	= eKLineTypeCompare;
		infoself.m_iFrontCount		= KiInitialReqKLineNums*iScale;

		DoRequestViewData(infoself);
	}

	//
	if ( EWTNone != m_eWeightType )
	{
		RequestWeightData();
	}	
}

void CIoViewDetail::RequestWeightData()
{
	//
	if ( EWTNone == m_eWeightType )
	{
		// 如果当前是不除权的,直接返回		
		return;
	}
	
	CMerch* pMerch = m_pMerchXml;
	
	if ( NULL == pMerch )
	{
		return;
	}
	 
	if ( CReportScheme::Instance()->IsFuture(pMerch->m_Market.m_MarketInfo.m_eMarketReportType) )
	{
		return;
	}

	// 1: 看本地有没有除权数据
	if ( !pMerch->m_bHaveReadLocalWeightFile && 0 == pMerch->m_TimeLastUpdateWeightDatas.GetTime() )
	{
		// 本地读文件失败而且从服务器得数据都失败的时候,
		// 会同时尝试 1: 再次读本地文件 2: 再次发请求给服务器
		// 任何一条路径成功后,就不会再读文件了.但是还会发请求服务器(2小时间隔限制)
		
		// 第一次的时候从本地取,设置把本地的crc 值发给服务器
		CArray<CWeightData,CWeightData&> aWeightData;
		UINT uiCrc32 = 0;
		
		if ( ReadWeightDataFromLocalFile(m_pMerchXml, aWeightData, uiCrc32) )
		{				
			pMerch->m_bHaveReadLocalWeightFile = true;
			
			// 设置CMerch 中相关字段的值				
			pMerch->m_uiWeightDataCRC = uiCrc32;
			
			int32 iSize = aWeightData.GetSize();
			pMerch->m_aWeightDatas.SetSize(iSize);				
			memcpyex(pMerch->m_aWeightDatas.GetData(), aWeightData.GetData(), sizeof(CWeightData) * iSize);					
		}	 
	}	
	
	// 2: 向服务器发请求		
	CMmiReqPublicFile info;
	info.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	info.m_StrMerchCode		= pMerch->m_MerchInfo.m_StrMerchCode;
	info.m_ePublicFileType	= EPFTWeight;
	info.m_uiCRC32			= pMerch->m_uiWeightDataCRC;
		
	DoRequestViewData(info);
}

void CIoViewDetail::OnVDataPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType)
{
	// ...fangz1117  目前 UpdateMainMerchKLine 中是每次都 bUpdate = false 全部数据重算,
	// 优化以后,要处理除权等数据,不要因为在收盘后没有K 线重算的时机而导致没有更新
	
	if (NULL == pMerch || pMerch != m_pMerchXml)
		return;
	
	if (EPFTWeight != ePublicFileType)	// 仅处理除权数据
		return;
	
	// 更新所有数据
	OnVDataMerchKLineUpdate(pMerch);
}

void CIoViewDetail::OnVDataMerchKLineUpdate(IN CMerch *pMerch)
{
	// K 线更新的时候需要重算,这里重算所有的三条K 线,不是仅仅更新当前选中的那个:
	// 因为: 不要绘图,纯计算.大部分都是新增一根K 线,或最新一根更新的情况,每根K 线仅仅计算一次.不会有很大的计算量,
	// 所以都算一下. 另外: 如果只算一根,那么切换的时候,必然要全部重算另一个周期的,速度反而慢
	
	if ( NULL == m_pMerchXml || pMerch != m_pMerchXml )
	{
		return;
	}
	
	for (int32 i = 0 ; i < KLINE_NUMS; i++)
	{
		E_NodeTimeInterval eNodeCompare		 = ENTIDay;
		E_KLineTypeBase	   eKLineTypeCompare = EKTBDay;
		int32			   iScale			 = 1;
		
		if ( !GetTimeIntervalInfo(m_aIoViewDetailCells[i].m_iTimeUserMultipleMinutes, m_aIoViewDetailCells[i].m_iTimeUserMultipleDays, m_aIoViewDetailCells[i].m_eNodeInterval, eNodeCompare, eKLineTypeCompare, iScale) )
		{
			continue;
		}

		E_KLineTypeBase eKLineType = eKLineTypeCompare;
	
		// 先获取对应的K线
		int32 iPosFound;
		CMerchKLineNode* pKLineRequest = NULL;
		pMerch->FindMerchKLineNode(eKLineType, iPosFound, pKLineRequest);
		
		// 根本找不到K线数据， 那就不需要显示了
		if ( NULL == pKLineRequest || 0 == pKLineRequest->m_KLines.GetSize() )	
		{	
			return;
		}

		// 清空,备份,设置数据	
		CArray<CKLine,CKLine> aKLineBeforUpdate;	// 备份的数据,用于更新以后的比较
		aKLineBeforUpdate.RemoveAll();
		aKLineBeforUpdate.Copy(m_aIoViewDetailCells[i].m_aKLines);
	
		//
		{
			// 更新数据:
			int32 iTimeInitPerDay = 0, iTimeOpenPerDay = 0, iTimeClosePerDay = 0;
			
			CMerch *pTmpMerch = m_pMerchXml;
			
			if ( NULL == pTmpMerch	)
			{
				return;					
			}
			
			pTmpMerch->m_Market.m_MarketInfo.GetTimeInfo(iTimeInitPerDay, iTimeOpenPerDay, iTimeClosePerDay);				
			
			// 清空数据:
						
			// 除权复权:
			CArray<CKLine, CKLine> aSrcKLines;
			aSrcKLines.Copy(pKLineRequest->m_KLines);
			
			// 看是否要做复权处理
			CArray<CKLine, CKLine> aWeightedKLines;
			
			if ( CIoViewKLine::EWTAft == m_eWeightType )
			{
				// 后复权
				if (CMerchKLineNode::WeightKLine(aSrcKLines, pTmpMerch->m_aWeightDatas, false, aWeightedKLines))
				{
					aSrcKLines.Copy(aWeightedKLines);
				}		
			}
			else if ( CIoViewKLine::EWTPre == m_eWeightType )  
			{
				// 前复权
				if (CMerchKLineNode::WeightKLine(aSrcKLines, pTmpMerch->m_aWeightDatas, true, aWeightedKLines))
				{
					aSrcKLines.Copy(aWeightedKLines);
				}
			}
			
			//
			CMarketIOCTimeInfo MarketIOCTimeInfo;
			if (NULL == m_pAbsCenterManager || !pTmpMerch->m_Market.GetRecentTradingDay(m_pAbsCenterManager->GetServerTime(), MarketIOCTimeInfo, pTmpMerch->m_MerchInfo))
			{
				return;
			}

			// 处理数据， 压缩数据
			
			int32 iMinUser = m_aIoViewDetailCells[i].m_iTimeUserMultipleMinutes;
			int32 iDayUser = m_aIoViewDetailCells[i].m_iTimeUserMultipleDays;
			
			switch (m_aIoViewDetailCells[i].m_eNodeInterval)
			{
			case ENTIMinute:
				{					
					m_aIoViewDetailCells[i].m_aKLines.SetSize(aSrcKLines.GetSize());
					memcpyex(m_aIoViewDetailCells[i].m_aKLines.GetData(), aSrcKLines.GetData(), sizeof(CKLine) * aSrcKLines.GetSize());									
				}
				break;
			case ENTIMinute5:
				{							
					m_aIoViewDetailCells[i].m_aKLines.SetSize(aSrcKLines.GetSize());
					memcpyex(m_aIoViewDetailCells[i].m_aKLines.GetData(), aSrcKLines.GetData(), sizeof(CKLine) * aSrcKLines.GetSize());													
				}
				break;					
			case ENTIMinute15:
				{
					if (!CMerchKLineNode::CombinMinuteN(pTmpMerch, 15, aSrcKLines, m_aIoViewDetailCells[i].m_aKLines))
					{
						//ASSERT(0);
					}	
				}
				break;
			case ENTIMinute30:
				{
					if (!CMerchKLineNode::CombinMinuteN(pTmpMerch, 30, aSrcKLines, m_aIoViewDetailCells[i].m_aKLines))
					{
						//ASSERT(0);
					}						
				}
				break;
			case ENTIMinute60:
				{						
					m_aIoViewDetailCells[i].m_aKLines.SetSize(pKLineRequest->m_KLines.GetSize());
					memcpyex(m_aIoViewDetailCells[i].m_aKLines.GetData(), aSrcKLines.GetData(), sizeof(CKLine) * aSrcKLines.GetSize());					
				}	
				break;
			case ENTIMinuteUser:
				{					
					if (!CMerchKLineNode::CombinMinuteN(pTmpMerch, iMinUser, aSrcKLines, m_aIoViewDetailCells[i].m_aKLines))
					{
						//ASSERT(0);
					}
				}	
				break;
			case ENTIDay:
				{								
					m_aIoViewDetailCells[i].m_aKLines.SetSize(aSrcKLines.GetSize());
 					memcpyex(m_aIoViewDetailCells[i].m_aKLines.GetData(), aSrcKLines.GetData(), sizeof(CKLine) * aSrcKLines.GetSize());								
				}
				break;
			case ENTIWeek:
				{
					if (!CMerchKLineNode::CombinWeek(aSrcKLines, m_aIoViewDetailCells[i].m_aKLines))
					{
						//ASSERT(0);
					}
				}
				break;
			case ENTIDayUser:
				{
					if (!CMerchKLineNode::CombinDayN(aSrcKLines, iDayUser, m_aIoViewDetailCells[i].m_aKLines))
					{
						//ASSERT(0);
					}				
				}
				break;
			case ENTIMonth:
				{
					m_aIoViewDetailCells[i].m_aKLines.SetSize(aSrcKLines.GetSize());
					memcpyex(m_aIoViewDetailCells[i].m_aKLines.GetData(), aSrcKLines.GetData(), sizeof(CKLine) * aSrcKLines.GetSize());				
				}
				break;								
			case ENTIQuarter:
				{
					if (!CMerchKLineNode::CombinMonthN(aSrcKLines, 3, m_aIoViewDetailCells[i].m_aKLines))
					{
						//ASSERT(0);
					}				
				}
				break;
			case ENTIYear:
				{
					if (!CMerchKLineNode::CombinMonthN(aSrcKLines, 12, m_aIoViewDetailCells[i].m_aKLines))
					{
						//ASSERT(0);
					}
					// 不算大周期
				}
				break;
			default:
				//ASSERT(0);
				break;
			}	
		}

		// 设置更新时间:
		// 最新一根的时间变化了,可能新增了一根或几根K 线,可能 K 线返回的数据有了大的变化. 要重设更新时间:
		
		if( aKLineBeforUpdate.GetSize() <= 0 && m_aIoViewDetailCells[i].m_aKLines.GetSize() > 0 )
		{
			// 原来没数据,现在有了:		
			SetUpdateTime(i);
		}
		else if ( aKLineBeforUpdate.GetSize() > 0 && m_aIoViewDetailCells[i].m_aKLines.GetSize() > 0 )
		{
			// 1: 最新一根K 线的时间不同了(新增一根或者数据有变化)
			CGmtTime TimeKLineBefore = aKLineBeforUpdate.GetAt(aKLineBeforUpdate.GetSize() - 1).m_TimeCurrent;
			CGmtTime TimeKLineNow    = m_aIoViewDetailCells[i].m_aKLines.GetAt(m_aIoViewDetailCells[i].m_aKLines.GetSize()-1).m_TimeCurrent;
			
			if ( TimeKLineBefore != TimeKLineNow )
			{				
				SetUpdateTime(i);
			}					
			
			// 2: 最新一根K 线的日期和服务器现在的时间,日期不同了(可能过夜)
			// 原因: 周线或者月线等.周一设置定时器,发现更新时间是周五最后10 秒.所以不启动定时器,导致后面一直没有启动定时器和设置更新时间的时机
			if(m_pAbsCenterManager)
			{
				CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();

				if ( TimeKLineNow.GetDay() != TimeNow.GetDay() )
				{
					SetUpdateTime(i);
				}
			}
		}
	
		// 更新当前周期的指标值
		int32 iReval = CompareKLinesChange(aKLineBeforUpdate, m_aIoViewDetailCells[i].m_aKLines);
	
		{
			// 算稳健型指标:
			E_ReportType eReportType = GetMerchKind(m_pMerchXml);
			bool32 bJudgeKTOC = false;
			
			if (CReportScheme::IsFuture(eReportType))
			{
				bJudgeKTOC = true;
			}
			
			if ( 0 == m_aIoViewDetailCells[i].m_aSaneIndexValues.GetSize() )
			{
				iReval = 3;
			}
		
			if ( 2 == iReval )
			{
				SetUpdateTime(i);
			}

			// ...fangz0705 这里需要优化.暂没处理,全部都重算了.			
			DEL(m_aIoViewDetailCells[i].m_pIndexMaForSane);
			m_aIoViewDetailCells[i].m_pIndexMaForSane = CalcIndexEMAForSane(m_aIoViewDetailCells[i].m_aKLines);
			
			int32 iSizeKLine = m_aIoViewDetailCells[i].m_aKLines.GetSize();

			if ( iSizeKLine < 1 || NULL == m_aIoViewDetailCells[i].m_pIndexMaForSane )
			{
				// 不能计算这些参数的指标 - 由于是出现错误导致这种情况，所以所有数据还原成初始
				m_aIoViewDetailCells[i].m_aSaneIndexValues.RemoveAll();
				m_aIoViewDetailCells[i].m_iSaneStaticsNums				= 0;
				m_aIoViewDetailCells[i].m_bTimeToUpdateLatestValues		= false;
				m_aIoViewDetailCells[i].m_bNeedCalcSaneLatestValues		= true;
				m_aIoViewDetailCells[i].m_pIndexMaForSane				= NULL;
				m_aIoViewDetailCells[i].m_aMerchsHaveCalcLatestValues.RemoveAll();
				m_aIoViewDetailCells[i].m_TimeToUpdate					= 0;
				m_aIoViewDetailCells[i].m_iTradeTimes					= 0;
				m_aIoViewDetailCells[i].m_fAccuracyRate					= 0.0;
				m_aIoViewDetailCells[i].m_fProfitability				= 0.0;
				m_aIoViewDetailCells[i].m_bCalcing						= false;

				m_GridCtrl.DeleteNonFixedRows();
				Invalidate(TRUE);
				continue;
			}

			if ( 1 == iReval)
			{
				// 最新价变化. 更新一下趋势线, 重算一下收益率, 不用算趋势指标:
				if ( m_aIoViewDetailCells[i].m_aSaneIndexValues.GetSize() > 0 )
				{
					if ( UpDateLatestIndexForSane(m_aIoViewDetailCells[i].m_pIndexMaForSane, m_aIoViewDetailCells[i].m_aSaneIndexValues) )
					{
						// 重算收益率								
						float fProfit;
						int32 iGoodTimes,iBadTimes;
						float fPriceLastClose = m_aIoViewDetailCells[i].m_aKLines.GetAt(iSizeKLine - 1).m_fPriceClose;
			
						StatisticaSaneValues2(KiStatisticaKLineNums, fPriceLastClose, m_aIoViewDetailCells[i].m_iSaneStaticsNums, m_aIoViewDetailCells[i].m_aSaneIndexValues, m_aIoViewDetailCells[i].m_iTradeTimes, iGoodTimes, iBadTimes, m_aIoViewDetailCells[i].m_fAccuracyRate, m_aIoViewDetailCells[i].m_fProfitability, fProfit);

						RedrawWindow();
					}
				}
			}
			else if ( 2 == iReval)
			{
				// 增加了一根,算最新的,加到本地数组最后
				bool32 bKillTimer = false;

				// 1: 修正上一根的数据
				CArray<CKLine, CKLine> aKLineTemp;
				aKLineTemp.Copy(m_aIoViewDetailCells[i].m_aKLines);
				int32 iSizeTemp = aKLineTemp.GetSize();
				
				if ( iSizeTemp > 0 )
				{
					aKLineTemp.RemoveAt(iSizeTemp - 1);
					CalcLatestSaneIndex(false, bJudgeKTOC, m_aIoViewDetailCells[i].m_pIndexMaForSane, aKLineTemp, m_aIoViewDetailCells[i].m_aSaneIndexValues, m_pAbsCenterManager->GetServerTime().GetTime(), bKillTimer);
					UpdateTableContent(1);
				}
				

				// 2: 算新增的一根
				bool32 bOK = CalcLatestSaneIndex(true, bJudgeKTOC, m_aIoViewDetailCells[i].m_pIndexMaForSane, m_aIoViewDetailCells[i].m_aKLines, m_aIoViewDetailCells[i].m_aSaneIndexValues, m_pAbsCenterManager->GetServerTime().GetTime(), bKillTimer);
				
				if ( bOK )
				{
					// 清空列表
					m_aIoViewDetailCells[i].m_aMerchsHaveCalcLatestValues.RemoveAll();

					int32 iGoodTimes,iBadTimes;
					float fProfit;
	
					if ( iSizeKLine > 0 )
					{
						float fPriceLastClose = m_aIoViewDetailCells[i].m_aKLines.GetAt(iSizeKLine - 1).m_fPriceClose;
						StatisticaSaneValues2(KiStatisticaKLineNums, fPriceLastClose, m_aIoViewDetailCells[i].m_iSaneStaticsNums, m_aIoViewDetailCells[i].m_aSaneIndexValues, m_aIoViewDetailCells[i].m_iTradeTimes, iGoodTimes, iBadTimes, m_aIoViewDetailCells[i].m_fAccuracyRate, m_aIoViewDetailCells[i].m_fProfitability, fProfit);
					}					
				}
				
				UpdateTableContent(2);
			}
			else if ( 3 == iReval)
			{
				//
				bool32 bPassUpdateTime = true;
				bool32 bNeedTimer	   = false;
				
				CGmtTime TimeNow = m_aIoViewDetailCells[i].m_aKLines.GetAt(iSizeKLine-1).m_TimeCurrent;
				CGmtTime TimeToUpdate;

				if ( !CIoViewKLine::GetKLineUpdateTime(m_pMerchXml, m_pAbsCenterManager, TimeNow, m_aIoViewDetailCells[i].m_eNodeInterval, m_aIoViewDetailCells[i].m_iTimeUserMultipleMinutes, bNeedTimer, TimeToUpdate, bPassUpdateTime) )
				{
					//ASSERT(0);
				}

				// 改变很大,需要全部重算						
				bool32 bOK = CalcHistorySaneIndex(bJudgeKTOC, bPassUpdateTime, m_aIoViewDetailCells[i].m_pIndexMaForSane, m_aIoViewDetailCells[i].m_aKLines, m_aIoViewDetailCells[i].m_aSaneIndexValues);
				
				// 重算交易次数,准确率,收益率等值
				if ( bOK)
				{
					int32 iGoodTimes,iBadTimes;
					float fProfit;
					int32 iKLineSize = m_aIoViewDetailCells[i].m_aKLines.GetSize();
					if ( iKLineSize > 0 )
					{
						float fPriceLastClose = m_aIoViewDetailCells[i].m_aKLines.GetAt(iKLineSize - 1).m_fPriceClose;
						StatisticaSaneValues2(KiStatisticaKLineNums, fPriceLastClose, m_aIoViewDetailCells[i].m_iSaneStaticsNums, m_aIoViewDetailCells[i].m_aSaneIndexValues, m_aIoViewDetailCells[i].m_iTradeTimes, iGoodTimes, iBadTimes, m_aIoViewDetailCells[i].m_fAccuracyRate, m_aIoViewDetailCells[i].m_fProfitability, fProfit);
					}
				}
				
				UpdateTableContent(3);
			}
		}		
	}
}

//
int32 CIoViewDetail::CompareKLinesChange(const CArray<CKLine,CKLine>& KLineBef, const CArray<CKLine,CKLine>& KLineAft)
{
	// K 线更新的时候,判断最新的K 线与原来相比有什么变化:
	// 返回值: 0:相同 1:最新一根变化 2:增加了一根 3:除了以上情况,有大的变化

	int32 iReVal = 3;

	CArray<CKLine,CKLine> KLineBefor;
	KLineBefor.RemoveAll();
	KLineBefor.Copy(KLineBef);
	CArray<CKLine,CKLine> KLineAfter;
	KLineAfter.RemoveAll();
	KLineAfter.Copy(KLineAft);

	int32 iSizeBefore = KLineBefor.GetSize();
	int32 iSizeAfter  = KLineAfter.GetSize(); 
	
	char* pBefore = (char*)KLineBefor.GetData();
	char* pAfter  = (char*)KLineAfter.GetData();

	if ( NULL == pBefore || NULL == pAfter)
	{
		return 3;
	}
	
	//
	if ( iSizeBefore != iSizeAfter)
	{
		// 节点数不同,可能只更新了一个节点.这时需要仔细判断,可能更新了多个节点个数大不相同,直接返回3;

		if ( iSizeAfter == iSizeBefore + 1)
		{
			// 多了一个节点.判断一下是不是除了最后一个节点以外,其他的都相同,如果是的话,那就是 2

			KLineAfter.RemoveAt(iSizeAfter-1);
			pAfter  = (char*)KLineAfter.GetData();

			if ( 0 == memcmp(pBefore,pAfter,iSizeBefore*sizeof(CKLine)))
			{
				// 除了最后一个,其他都一样,就是新增了一根
				return 2;
			}
			else
			{
				// 不一样.
				return 3;
			}			
		}
		else
		{
			return 3;
		}
	}
	else
	{
		// 节点个数相同:
		if ( iSizeBefore == KMaxMinuteKLineCount )
		{
			// 满了, 1W 个节点了: 新来的数据插到第一个,把最后一个踢掉.所以拿 After 的前 0~9999(去尾) 和 Before 的 1~10000 比较(去头)
			
			if ( 0 == memcmp(pBefore,pAfter,iSizeBefore*sizeof(CKLine)))
			{
				// 完全相同
				return 0;
			}

			KLineBefor.RemoveAt(0);
			KLineAfter.RemoveAt(iSizeAfter-1);

			pBefore = (char*)KLineBefor.GetData();
			pAfter  = (char*)KLineAfter.GetData();
			
			if ( 0 == memcmp(pBefore,pAfter,(iSizeBefore - 1)*sizeof(CKLine)))
			{
				// 除了最后一个,其他都一样,最新一根发生变化
				return 1;
			}
			else
			{
				// 不一样
				return 3;
			}
		}
		else
		{
			// 正常情况
			if ( 0 == memcmp(pBefore,pAfter,iSizeBefore*sizeof(CKLine)))
			{
				// 完全相同
				return 0;
			}
			
			//看除了最后一个前面是不是一样的.
			
			KLineAfter.RemoveAt(iSizeAfter-1);
			KLineBefor.RemoveAt(iSizeBefore-1);
			
			pBefore = (char*)KLineBefor.GetData();
			pAfter  = (char*)KLineAfter.GetData();
			
			if ( 0 == memcmp(pBefore,pAfter,(iSizeBefore - 1)*sizeof(CKLine)))
			{
				// 除了最后一个,其他都一样,最新一根发生变化
				return 1;
			}
			else
			{
				// 不一样.
				return 3;
			}
		}
					
	}
	
	return iReVal;
}

void CIoViewDetail::SetUpdateTime(int32 iIndex)
{
	// 进入时机: 1: 新增一根K 线 2: 当前这一根 K 线的日期发生了变化 3: K 线变化很大 iReVal == 3
	if ( iIndex < 0 || iIndex >= KLINE_NUMS )
	{
		return;
	}

	int32 iKLineSize = m_aIoViewDetailCells[iIndex].m_aKLines.GetSize();
	if ( iKLineSize <= 0 )
	{
		return;
	}

	// 设置更新时间:
	
	CGmtTime TimeNow;								// 当前最新一根K 线的时间
	CGmtTime TimeClose;								// 当前商品今天的收盘时间
	
	bool32 bNeedTimer = true;						// 是否需要定时器计算最新指标
		
	CMerch *pMerch = m_pMerchXml;
	
	if ( NULL == pMerch )
	{
		return;
	}

	CGmtTime TimeToUpdate = 0;

	// 得到起始时间:
	TimeNow = m_aIoViewDetailCells[iIndex].m_aKLines.GetAt(iKLineSize - 1).m_TimeCurrent;

	// 时间错误,返回
	if ( TimeNow.GetTime() <= 0 )
		return;

	//
	if ( 0 == iIndex )
	{		
		KillTimer(KTimerIDUpdateKLine1Sane);		
	}
	else if ( 1 == iIndex )
	{
		KillTimer(KTimerIDUpdateKLine2Sane);
	}
	else if ( 2 == iIndex )
	{
		KillTimer(KTimerIDUpdateKLine3Sane);
	}
	
	if ( BePassedCloseTime(m_pMerchXml) )
	{
		// 如果现在已经收盘了, 就不要再更新了
		m_aIoViewDetailCells[iIndex].m_bNeedCalcSaneLatestValues = false;			
		m_aIoViewDetailCells[iIndex].m_TimeToUpdate				 = 0;

		return;
	}

	// 
	bool32 bPassUpdateTime = false;
	if ( !CIoViewKLine::GetKLineUpdateTime(m_pMerchXml, m_pAbsCenterManager, TimeNow, m_aIoViewDetailCells[iIndex].m_eNodeInterval, m_aIoViewDetailCells[iIndex].m_iTimeUserMultipleMinutes, bNeedTimer, TimeToUpdate, bPassUpdateTime) )
	{
		return;
	}

	//	
	if ( 0 == iIndex )
	{
		{
			m_aIoViewDetailCells[iIndex].m_TimeToUpdateSane = TimeToUpdate;
			
			if( bNeedTimer )
			{
				m_aIoViewDetailCells[iIndex].m_bNeedCalcSaneLatestValues = true;
				SetTimer(KTimerIDUpdateKLine1Sane, KTimerUpdatePeriod, NULL);		
			}
			else
			{
				// 不需要更新实时数据: 
				m_aIoViewDetailCells[iIndex].m_bNeedCalcSaneLatestValues = false;
			}
		}			 
	}
	else if ( 1 == iIndex )
	{
		{
			m_aIoViewDetailCells[iIndex].m_TimeToUpdateSane = TimeToUpdate;
			
			if( bNeedTimer )
			{
				m_aIoViewDetailCells[iIndex].m_bNeedCalcSaneLatestValues = true;
				SetTimer(KTimerIDUpdateKLine2Sane, KTimerUpdatePeriod, NULL);		
			}
			else
			{
				// 不需要更新实时数据: 
				m_aIoViewDetailCells[iIndex].m_bNeedCalcSaneLatestValues	 = false;			
			}
		}		
	}
	else if ( 2 == iIndex )
	{	
		{
			m_aIoViewDetailCells[iIndex].m_TimeToUpdateSane = TimeToUpdate;
			
			if( bNeedTimer )
			{
				m_aIoViewDetailCells[iIndex].m_bNeedCalcSaneLatestValues = true;
				SetTimer(KTimerIDUpdateKLine3Sane, KTimerUpdatePeriod, NULL);		
			}
			else
			{
				// 不需要更新实时数据: 
				m_aIoViewDetailCells[iIndex].m_bNeedCalcSaneLatestValues	 = false;				
			}
		}		
	}
}

void CIoViewDetail::OnTimerUpdateLatestValuse(UINT uID)
{
	// 定时器,倒数10 秒更新:
	E_ReportType eReportType = GetMerchKind(m_pMerchXml);
	bool32 bJudgeKTOC = false;
	
	if (CReportScheme::IsFuture(eReportType))
	{
		bJudgeKTOC = true;
	}
		
	//
	if ( NULL == m_pMerchXml )
	{
		return;
	}

	// 得到当前时间
	CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();
	
	// 10 秒的时间.
	CGmtTimeSpan TimeSpan(0, 0, 0, SECONDSTOUPDATE);

	//
	int32 iInitial,iOpen,iClose;
	m_pMerchXml->m_Market.m_MarketInfo.GetTimeInfo(iInitial, iOpen, iClose);

	if ( KTimerIDUpdateKLine1Sane == uID )
	{
		// 最晚的更新时间,超过了这个时间就不更新
		CGmtTime TimeLast = m_aIoViewDetailCells[0].m_TimeToUpdateSane + TimeSpan;

		if ( BePassedCloseTime(m_pMerchXml) )
		{
			// 收盘了, 修正一下最新一根的数据(防止实时与历史不一致)
			bool32 bKillTimer = false;
									   
			CalcLatestSaneIndex(false, bJudgeKTOC, m_aIoViewDetailCells[0].m_pIndexMaForSane, m_aIoViewDetailCells[0].m_aKLines, m_aIoViewDetailCells[0].m_aSaneIndexValues, m_pAbsCenterManager->GetServerTime().GetTime(), bKillTimer);
			UpdateTableContent(1);
			
			if ( bKillTimer )
			{
				m_aIoViewDetailCells[0].m_bNeedCalcSaneLatestValues		= false;				
				KillTimer(KTimerIDUpdateKLine1Sane);
			}
		}
		else if ( TimeNow < m_aIoViewDetailCells[0].m_TimeToUpdateSane ) 
		{
			//NULL;
		} 
		else if ( TimeNow >= m_aIoViewDetailCells[0].m_TimeToUpdateSane && TimeNow <= TimeLast )
		{
			for ( int32 i = 0 ; i < m_aIoViewDetailCells[0].m_aMerchsHaveCalcLatestValues.GetSize(); i++)
			{
				if ( m_pMerchXml == m_aIoViewDetailCells[0].m_aMerchsHaveCalcLatestValues.GetAt(i) )
				{
					// 已经算过一次了,不要再算了
					return;
				}
			}

			// 计算第一条的稳健最新指标:						
			bool32 bKillTimer = false;
			
			if (CalcLatestSaneIndex(false, bJudgeKTOC, m_aIoViewDetailCells[0].m_pIndexMaForSane, m_aIoViewDetailCells[0].m_aKLines, m_aIoViewDetailCells[0].m_aSaneIndexValues, m_pAbsCenterManager->GetServerTime().GetTime(), bKillTimer))
			{
				ASSERT(bKillTimer);
			}

			if ( bKillTimer )
			{
				m_aIoViewDetailCells[0].m_bNeedCalcSaneLatestValues		= false;
			
				KillTimer(KTimerIDUpdateKLine1Sane);
				// 判断一下当前更新的节点跟表格中最新的一个节点是否相同
				bool32 bSameNode = false;

				int32 iSize = m_aIoViewDetailCells[0].m_aSaneIndexValues.GetSize();
				
				if ( iSize >= 1 )
				{							
					T_SaneIndexNode Node = m_aIoViewDetailCells[0].m_aSaneIndexValues.GetAt(iSize-1);
					
					if( Node.m_TimeKLine.GetTime() == m_TimePre.GetTime() )
					{
						bSameNode = true;
					}
				}						

				//
				if ( bSameNode )
				{
					// 是相同的节点,更新最后一行
					UpdateTableContent(1);
				}
				else
				{					
					// 不是同一个,要新增一行
					UpdateTableContent(2);
				}
				
				m_aIoViewDetailCells[0].m_aMerchsHaveCalcLatestValues.Add(m_pMerchXml);
			}
		}
		else if ( TimeNow > TimeLast )
		{
			// 过时了,不要更新.重新设置更新时间:
			m_aIoViewDetailCells[0].m_bNeedCalcSaneLatestValues = false;
			KillTimer(KTimerIDUpdateKLine1Sane);
			
			SetUpdateTime(0);
		}
	}
	else if ( KTimerIDUpdateKLine2Sane == uID )
	{
		// 最晚的更新时间,超过了这个时间就不更新
		CGmtTime TimeLast = m_aIoViewDetailCells[1].m_TimeToUpdateSane + TimeSpan;
		
		// 收盘时间
		if ( BePassedCloseTime(m_pMerchXml) )
		{
			// 收盘了, 修正一下最新一根的数据(防止实时与历史不一致)
			bool32 bKillTimer = false;
									   
			CalcLatestSaneIndex(false, bJudgeKTOC, m_aIoViewDetailCells[1].m_pIndexMaForSane, m_aIoViewDetailCells[1].m_aKLines, m_aIoViewDetailCells[1].m_aSaneIndexValues, m_pAbsCenterManager->GetServerTime().GetTime(), bKillTimer);
			UpdateTableContent(1);

			if ( bKillTimer )
			{
				m_aIoViewDetailCells[1].m_bNeedCalcSaneLatestValues		= false;
				KillTimer(KTimerIDUpdateKLine2Sane);
			}
		}
		else if ( TimeNow < m_aIoViewDetailCells[1].m_TimeToUpdateSane)
		{
			//NULL;
		}
		if ( TimeNow >= m_aIoViewDetailCells[1].m_TimeToUpdateSane && TimeNow < TimeLast)
		{
			for ( int32 i = 0 ; i < m_aIoViewDetailCells[1].m_aMerchsHaveCalcLatestValues.GetSize(); i++)
			{
				if ( m_pMerchXml == m_aIoViewDetailCells[1].m_aMerchsHaveCalcLatestValues.GetAt(i) )
				{
					// 已经算过一次了,不要再算了
					return;
				}
			}

			// 计算第二条的稳健最新指标:						
			bool32 bKillTimer = false;
			if (CalcLatestSaneIndex(false, bJudgeKTOC, m_aIoViewDetailCells[1].m_pIndexMaForSane, m_aIoViewDetailCells[1].m_aKLines, m_aIoViewDetailCells[1].m_aSaneIndexValues, m_pAbsCenterManager->GetServerTime().GetTime(), bKillTimer))
			{
				ASSERT(bKillTimer);
			}

			if ( bKillTimer )
			{
				m_aIoViewDetailCells[1].m_bNeedCalcSaneLatestValues		= false;
				m_aIoViewDetailCells[1].m_aMerchsHaveCalcLatestValues.Add(m_pMerchXml);

				KillTimer(KTimerIDUpdateKLine2Sane);
				UpdateTableContent(1);
			}
		}
		else if ( TimeNow > TimeLast )
		{
			// 过时了,不要更新.重新设置更新时间:
			m_aIoViewDetailCells[1].m_bNeedCalcSaneLatestValues = false;
			KillTimer(KTimerIDUpdateKLine2Sane);
			
			SetUpdateTime(1);
		}
	}
	else if ( KTimerIDUpdateKLine3Sane == uID )
	{
		// 最晚的更新时间,超过了这个时间就不更新
		CGmtTime TimeLast = m_aIoViewDetailCells[2].m_TimeToUpdateSane + TimeSpan;
		
		if ( BePassedCloseTime(m_pMerchXml) )
		{
			// 收盘了, 修正一下最新一根的数据(防止实时与历史不一致)
			bool32 bKillTimer = false;
									   
			CalcLatestSaneIndex(false, bJudgeKTOC, m_aIoViewDetailCells[2].m_pIndexMaForSane, m_aIoViewDetailCells[2].m_aKLines, m_aIoViewDetailCells[2].m_aSaneIndexValues, m_pAbsCenterManager->GetServerTime().GetTime(), bKillTimer);
			UpdateTableContent(1);

			if ( bKillTimer )
			{
				m_aIoViewDetailCells[2].m_bNeedCalcSaneLatestValues		= false;
				KillTimer(KTimerIDUpdateKLine3Sane);
			}
		}
		else if ( TimeNow < m_aIoViewDetailCells[2].m_TimeToUpdateSane)
		{
			//NULL;
		}
		if ( TimeNow >= m_aIoViewDetailCells[2].m_TimeToUpdateSane && TimeNow <= TimeLast)
		{
			for ( int32 i = 0 ; i < m_aIoViewDetailCells[2].m_aMerchsHaveCalcLatestValues.GetSize(); i++)
			{
				if ( m_pMerchXml == m_aIoViewDetailCells[2].m_aMerchsHaveCalcLatestValues.GetAt(i) )
				{
					// 已经算过一次了,不要再算了
					return;
				}
			}

			// 计算第三条的稳健最新指标:						
			bool32 bKillTimer = false;
			if (CalcLatestSaneIndex(false, bJudgeKTOC, m_aIoViewDetailCells[2].m_pIndexMaForSane, m_aIoViewDetailCells[2].m_aKLines, m_aIoViewDetailCells[2].m_aSaneIndexValues, m_pAbsCenterManager->GetServerTime().GetTime(), bKillTimer))
			{
				ASSERT(bKillTimer);
			}

			if ( bKillTimer )
			{
				m_aIoViewDetailCells[2].m_bNeedCalcSaneLatestValues		= false;
				m_aIoViewDetailCells[2].m_aMerchsHaveCalcLatestValues.Add(m_pMerchXml);

				KillTimer(KTimerIDUpdateKLine3Sane);
				UpdateTableContent(1);
			}
		}
		else if ( TimeNow > TimeLast )
		{
			// 过时了,不要更新.重新设置更新时间:
			m_aIoViewDetailCells[2].m_bNeedCalcSaneLatestValues = false;
			KillTimer(KTimerIDUpdateKLine3Sane);
			
			SetUpdateTime(2);
		}
	}
}

LRESULT CIoViewDetail::OnMessageTitleButton(WPARAM wParam,LPARAM lParam)
{
	if ( m_iCurShow < 0 || m_iCurShow >= KLINE_NUMS )
	{
		return 0;
	}
	//
	UINT uID = (UINT)wParam;
	//
	if ( 0 == uID )
	{
		// 选择周期:
		CNewMenu menu;
		menu.LoadMenu(IDR_MENU_KLINE);
		menu.LoadToolBar(g_awToolBarIconIDs);
		CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
		
		CMenu* pTempMenu = pPopMenu->GetSubMenu(_T("选择周期")); 
		ASSERT(NULL!=pTempMenu);
		CNewMenu* pSubMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );
		
		int32 iCurCheck = IDM_CHART_KMINUTE + m_aIoViewDetailCells[m_iCurShow].m_eNodeInterval;
		pSubMenu->CheckMenuItem(iCurCheck,MF_BYCOMMAND|MF_CHECKED); 
		
		CPoint pt;
		GetCursorPos(&pt);
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, AfxGetMainWnd());
		menu.DestroyMenu();
	}
	else if ( 1 == uID )
	{
		CNewMenu menu;
		bool32 bSuccess = menu.CreatePopupMenu();

		bSuccess = menu.AppendODMenu(L"金盾趋势", MF_STRING, IDM_IOVIEW_DETAIL_TREND);
	//	bSuccess = menu.AppendODMenu(L"金盾指标", MF_STRING, IDM_IOVIEW_DETAIL_SANE);

		bool bRun = false;
		if ( bRun/*m_aIoViewDetailCells[m_iCurShow].m_bShowTrend*/ )
		{
			menu.CheckMenuItem(IDM_IOVIEW_DETAIL_TREND, MF_BYCOMMAND | MF_CHECKED);
		}
		/*
		else
		{
			menu.CheckMenuItem(IDM_IOVIEW_DETAIL_SANE, MF_BYCOMMAND | MF_CHECKED);
		}
		*/
		CPoint pt;
		GetCursorPos(&pt);
		menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, AfxGetMainWnd());
		menu.DestroyMenu();
	}

	return 0;
}

bool32 CIoViewDetail::SetTimeInterval(E_NodeTimeInterval eNodeInterval)
{
	if ( m_iCurShow < 0 || m_iCurShow >= KLINE_NUMS || NULL == m_pMerchXml)
	{
		return false;
	}

	if ( eNodeInterval == m_aIoViewDetailCells[m_iCurShow].m_eNodeInterval )
	{
		return false;
	}

	m_aIoViewDetailCells[m_iCurShow].m_eNodeInterval = eNodeInterval;
	m_aIoViewDetailCells[m_iCurShow].m_aMerchsHaveCalcLatestValues.RemoveAll();
	SetUpdateTime(m_iCurShow);
	return true;
}

void CIoViewDetail::OnUserCircleChanged(E_NodeTimeInterval eNodeInterval, int32 iValue)
{
	if ( m_iCurShow < 0 || m_iCurShow >= KLINE_NUMS || NULL == m_pMerchXml)
	{
		return;
	}

	if ( ENTIMinuteUser == eNodeInterval)
	{
		if ( iValue >= USERSET_MINUET_MIN && iValue <= USERSET_MINUET_MAX )
		{
			m_aIoViewDetailCells[m_iCurShow].m_iTimeUserMultipleMinutes = iValue;
		}
	}
	else if (ENTIDayUser == eNodeInterval)
	{
		if ( iValue >= USERSET_DAY_MIN && iValue < USERSET_DAY_MAX )
		{
			m_aIoViewDetailCells[m_iCurShow].m_iTimeUserMultipleDays = iValue;
		}
	}

	OnVDataMerchKLineUpdate(m_pMerchXml);
}

void CIoViewDetail::OnMenu(UINT uID)
{
	if ( uID >= IDM_CHART_KMINUTE && uID <= IDM_CHART_KYEAR )
	{
		if ( NULL == m_pMerchXml )
		{
			return;
		}

		E_NodeTimeInterval NodeInterval = (E_NodeTimeInterval)(uID - IDM_CHART_KMINUTE);
		
		if ( ENTIDayUser == NodeInterval )
		{
			CDlgUserCycleSet Dlg;
			Dlg.SetIoViewParent(this);
			Dlg.SetInitialParam(ENTIDayUser, m_aIoViewDetailCells[m_iCurShow].m_iTimeUserMultipleDays);				
			if ( IDOK != Dlg.DoModal() )
			{
				return;
			}
		}
		else if ( ENTIMinuteUser == NodeInterval )
		{
			CDlgUserCycleSet Dlg;
			Dlg.SetIoViewParent(this);
			Dlg.SetInitialParam(ENTIMinuteUser, m_aIoViewDetailCells[m_iCurShow].m_iTimeUserMultipleMinutes);				
			if ( IDOK != Dlg.DoModal() )
			{
				return;
			}
		}
		
		if ( !SetTimeInterval(NodeInterval) )
		{
			return;
		}
		
		if ( m_GridCtrl.GetSafeHwnd() )
		{
			m_GridCtrl.DeleteNonFixedRows();
		}

		// 请求数据
		RequestViewData();
		
		//
		Invalidate();
	}
}

void CIoViewDetail::OnIndexMenu(UINT uID)
{
	if ( IDM_IOVIEW_DETAIL_SANE == uID )
	{
		if ( m_iCurShow < 0 || m_iCurShow >= KLINE_NUMS )
		{
			return;
		}

		E_ReportType eReportType = GetMerchKind(m_pMerchXml);
		bool32 bJudgeKTOC = false;
		
		if (CReportScheme::IsFuture(eReportType))
		{
			bJudgeKTOC = true;
		}
	
		//		
		if( 0 == m_aIoViewDetailCells[m_iCurShow].m_aSaneIndexValues.GetSize() || m_aIoViewDetailCells[m_iCurShow].m_aSaneIndexValues.GetSize() != m_aIoViewDetailCells[m_iCurShow].m_aKLines.GetSize())
		{
			DEL(m_aIoViewDetailCells[m_iCurShow].m_pIndexMaForSane);
			CalcIndexEMAForSane(m_aIoViewDetailCells[m_iCurShow].m_aKLines);	
			
			//
			bool32 bPassUpdateTime = true;
			bool32 bNeedTimer	   = false;
			int32  iSizeKLine =  m_aIoViewDetailCells[m_iCurShow].m_aKLines.GetSize();
			CGmtTime TimeNow = m_aIoViewDetailCells[m_iCurShow].m_aKLines.GetAt(iSizeKLine-1).m_TimeCurrent;
			CGmtTime TimeToUpdate;
			
			if ( !CIoViewKLine::GetKLineUpdateTime(m_pMerchXml, m_pAbsCenterManager, TimeNow, m_aIoViewDetailCells[m_iCurShow].m_eNodeInterval, m_aIoViewDetailCells[m_iCurShow].m_iTimeUserMultipleMinutes, bNeedTimer, TimeToUpdate, bPassUpdateTime) )
			{
				//ASSERT(0);
			}
			
			//
			CalcHistorySaneIndex(bJudgeKTOC, bPassUpdateTime, m_aIoViewDetailCells[m_iCurShow].m_pIndexMaForSane, m_aIoViewDetailCells[m_iCurShow].m_aKLines, m_aIoViewDetailCells[m_iCurShow].m_aSaneIndexValues);
		}
		
		UpdateTableContent(3);
	}
}