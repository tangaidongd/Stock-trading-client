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
const int32 KiTitleTextRows			   = 6;						// �������ֵ�����	
const int32 KiTitleTextCols			   = 4;						// �������ֵ�����			
const int32 KiInitialReqKLineNums	   = 300;					// K�ߵ�����	
				
const int32 KTimerIDUpdateKLine1Sane  = 504;					// ��ʱ��1: ��һ��K �ߵ��Ƚ�ָ�궨ʱ��
const int32 KTimerIDUpdateKLine2Sane  = 505;					// ��ʱ��2: �ڶ���K �ߵ��Ƚ�ָ�궨ʱ�� 
const int32 KTimerIDUpdateKLine3Sane  = 506;					// ��ʱ��3: ������K �ߵ��Ƚ�ָ�궨ʱ��

const int32 KTimerUpdatePeriod		  = 1000;					// ����ʱ��

const char* KStrElementAttriCurIndex  = "CurIndex";				// ��ǰ��ʾ�� K ��
const char* KStrElementKLine		  = "Kline";				// K ��
const char* KStrElementAttriInterval  = "Interval";				// ����
const char* KStrElementAttriUserMins  = "UserMins";				// �û��Զ������
const char* KStrElementAttriUserDays  = "UserDays";				// �û��Զ�������

// 
const int32 KMaxMinuteKLineCount	  = 10000;					// �������ķ���K�����ݣ� �������ƵĻ��� ���������������������

//
const int32 KiStatisticaKLineNums	  = 200;					// ͳ�Ƶ�K �߸���

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
		// �Ҽ��˵�:
		CNewMenu Menu;
 		Menu.LoadMenu(IDR_MENU_DETAIL);
		Menu.LoadToolBar(g_awToolBarIconIDs);

 		CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, Menu.GetSubMenu(0));
 		
 		//
 		CMenu* pTempMenu = pPopMenu->GetSubMenu(L"��������");
 		CNewMenu * pIoViewPopMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );
 		ASSERT(NULL != pIoViewPopMenu );
 		AppendIoViewsMenu(pIoViewPopMenu, IsLockedSplit());
		
		// ��ѡ��
 		
 		//
 		pTempMenu = pPopMenu->GetSubMenu(_T("ѡ������")); 
 		ASSERT(NULL!=pTempMenu);
 		CNewMenu* pSubMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );
 		
 		int32 iCurCheck = IDM_CHART_KMINUTE + m_aIoViewDetailCells[m_iCurShow].m_eNodeInterval;
 		pSubMenu->CheckMenuItem(iCurCheck,MF_BYCOMMAND|MF_CHECKED); 
 
		pPopMenu->ModifyODMenu(L"ѡ������", L"ѡ������", IDB_TOOLBAR_KLINECYLE);

		// ������������ָ�״̬����Ҫɾ��һЩ��ť
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

	// ������ͼ
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
	
	// �����	
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

	// �����ɫ������
	InitialIoViewFace(this);
	CreateTable();

	return 0;
}

bool32 CIoViewDetail::CreateTable()
{
	// ���Դ������
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
		
		// ��ձ������
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
			
	// �����и�
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
		// ����տ�, ��һ������Ϊ��ͷ���
		E_SysColor eColor = ESCKeep;
		
		// ʱ��:
		CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, 0);
		if ( NULL == pCell )
		{
			return;
		}
		
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		pCell->SetDefaultTextColor(eColor);
		pCell->SetText(StrTime);
		
		// ״̬:
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
		
		// �۸�
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, 2);
		if ( NULL == pCell )
		{
			return;
		}
		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		pCell->SetDefaultTextColor(eColor);
		pCell->SetText(StrPrice);
		
		// ��һ�п�ͷ���ֵ�:
		m_GridCtrl.InsertRow(L"", 0);
		
		eColor = ESCFall;
		
		// ʱ��:
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, 0);
		if ( NULL == pCell )
		{
			return;
		}
		
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		pCell->SetDefaultTextColor(eColor);
		pCell->SetText(StrTime);
		
		// ״̬:
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
		
		// �۸�
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
		// ����࿪
		E_SysColor eColor = ESCKeep;
		
		// ʱ��:
		CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, 0);
		if ( NULL == pCell )
		{
			return;
		}
		
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		pCell->SetDefaultTextColor(eColor);
		pCell->SetText(StrTime);
		
		// ״̬:
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
		
		// �۸�
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, 2);
		if ( NULL == pCell )
		{
			return;
		}
		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		pCell->SetDefaultTextColor(eColor);
		pCell->SetText(StrPrice);
		
		// ��һ�ж�ͷ���ֵ�:
		m_GridCtrl.InsertRow(L"", 0);
		
		eColor = ESCRise;
		
		// ʱ��:
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, 0);
		if ( NULL == pCell )
		{
			return;
		}
		
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		pCell->SetDefaultTextColor(eColor);
		pCell->SetText(StrTime);
		
		// ״̬:
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
		
		// �۸�
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
	// ����һ������:

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
			// ָ������ݸ�K �ߵĶԲ���
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
	
		// ʱ��
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
				// ���ǰһ���ʱ��:
			
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
			
		// �۸�
		CString StrPrice;
		StrPrice = Float2String(pKLine[iDataIndex].m_fPriceClose, m_pMerchXml->m_MerchInfo.m_iSaveDec);

		//
 		if ( ESISDCKO == pSaneIndex[iDataIndex].m_eSaneIndexState || ESISKCDO == pSaneIndex[iDataIndex].m_eSaneIndexState )
 		{
 			// ����տ� ���� ����࿪
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
			
			// ʱ��:
			CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, 0);
			if ( NULL == pCell )
			{
				return;
			}			
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);			
			pCell->SetDefaultTextColor(eColor);
			pCell->SetText(StrTime);
			
			// ״̬:
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, 1);
			if ( NULL == pCell )
			{
				return;
			}
			pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
			
			CString StrPrompt;
			
			// ����ָ��
			bool32 bFuture = CReportScheme::Instance()->IsFuture(m_pMerchXml->m_Market.m_MarketInfo.m_eMarketReportType);

			StrPrompt = GetNodeStateString(bFuture, pSaneIndex[iDataIndex]);		
			
			pCell->SetDefaultTextColor(eColor);
			pCell->SetText(StrPrompt);
			
			// �۸�
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, 2);
			if ( NULL == pCell )
			{
				return;
			}
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);			
			pCell->SetDefaultTextColor(eColor);
			pCell->SetText(StrPrice);			
		}

		// ����ʱ��
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

	// 1 : ����һ���仯�� 2: ������һ�� 3: ȫ������
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
		// ��������һ��
		UpdateOneRow(iUpdateFlag, 0, iKLineSize-1);
	}
	else if ( 2 == iUpdateFlag )
	{
		// ��һ��
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
		// ���������������
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
		
		// ����gridctrl home����Ҫһ��focuscell����������Ĭ��һ��focus cell
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

	// ��������
	SetRowHeightAccordingFont();

	// ������������������
	int32 iRectTextHeight = KiTitleTextRows * (m_iRowHeight + 5);

	m_RectText			= rectClient;
	m_RectText.bottom	= m_RectText.top + iRectTextHeight;

	// �������
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
	// ����ʹ��paint dc���������  0001752
	if ( NULL == m_pMerchXml )
	{
		// �����־	
		if ( m_bActive )
		{
			CRect rectActive(m_iActiveXpos, m_iActiveYpos, m_iActiveXpos * 2, m_iActiveYpos * 2);
			//dc.FillSolidRect(&rectActive, m_ColorActive);		
		}

		return;
	}

	// ���ñ�������:
	CRect RectClient;
	GetClientRect(&RectClient);
	RectClient.bottom = m_RectText.bottom; // ����ֻҪ��text�͹��ˣ���Ҫˢ��GridCtrl�Ĳ���

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
		// �ߴ��С��ʱ��,��Ҫ��ʾ
		return;
	}
	
	int32 iHeightPerRow = m_RectText.Height() / KiTitleTextRows;			// ÿ�еĸ߶�,ÿ����Ԫ��ĸ�,��											// �����ÿ����Ԫ֮��ļ��
	int32 iWidthAll	    = m_RectText.Width();								// ÿ�еĿ��
	
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

	// �����־	
	if ( m_bActive )
	{
		CRect rectActive(m_iActiveXpos, m_iActiveYpos, m_iActiveXpos * 2, m_iActiveYpos * 2);
		//dc.FillSolidRect(&rectActive, m_ColorActive);		
	}

	// ��һ��: ��Ʒ����
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

	// �����������
	{
		CPen  penLine(PS_SOLID, 1, GetIoViewColor(ESCChartAxisLine));
		CPen* penOld = dc.SelectObject(&penLine);
		dc.MoveTo(rectShow.left, rectShow.top + 1);
		dc.LineTo(rectShow.right, rectShow.top+ 1);
		dc.SelectObject(penOld);
		penLine.DeleteObject();
	}
	
	int32 iSkip = 5;

	// �ڶ�,��,��,��: �ڶ�,��,����K ��	
	for ( int32 i = 0; i < KLINE_NUMS; i++)
	{
		// �Ƚ���
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
		
		// ÿ��K ��ǰ��ĸ�ѡ��:
		
		// 1: ��ѡ�� ( һ������ )
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
			// ѡ��
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
		
		
		// 2: ���� ( �������� )
		CRect rectCyle = rect00;				
		rectCyle.left  = m_aIoViewDetailCells[i].m_RectShow.right + 1;
		
		CString StrMax = L"200����";
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
		
		// 3: ǿ��: (��������)
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
		
		// 4: �ȶ���: (��������)
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
		
		// 5: ����: (�ĸ�����)
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

	// ������: K �߸����ͽ��״���:
	if ( m_iCurShow < 0 || m_iCurShow >= KLINE_NUMS )
	{
		m_iCurShow = 0;
	}
	
	int32 iKLineNums = m_aIoViewDetailCells[m_iCurShow].m_iSaneStaticsNums;

	// K ����
	CRect rectKLineText = rectShow;
	rectKLineText.right = rectKLineText.left + iWidthCol0;
	rectKLineText.left += iSkip;
	StrText = L" K ����";
	dc.DrawText(StrText, &rectKLineText, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

	// ��Ŀ 
	CRect rectKLineNum = rectShow;
	rectKLineNum.left  = rectKLineText.right;
	rectKLineNum.right = rectKLineNum.left + iWidthCol1;
	StrText.Format(L"%d", iKLineNums);
	dc.SetTextColor(clrNum);
	dc.DrawText(StrText, &rectKLineNum, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
	dc.SetTextColor(clrText);

	// ������ 
	CRect rectTradeText = rectShow;
	rectTradeText.left  = rectKLineNum.right;
	rectTradeText.right = rectTradeText.left + iWidthCol2;
	StrText = L"������";
	dc.DrawText(StrText, &rectTradeText, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

	// ��Ŀ 
	CRect rectTradeNum = rectShow;
	rectTradeNum.left  = rectTradeText.right;
	rectTradeNum.right = rectTradeNum.left + iWidthCol3;
	StrText.Format(L"%d", m_aIoViewDetailCells[m_iCurShow].m_iTradeTimes);
	dc.SetTextColor(clrNum);
	dc.DrawText(StrText, &rectTradeNum, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
	dc.SetTextColor(clrText);

	rectShow.top    += iHeightPerRow;
	rectShow.bottom += iHeightPerRow;

	// ������: ����.׼ȷ��

	// ׼ȷ������ 
	CRect rectAccracyText = rectShow;
	rectAccracyText.right = rectAccracyText.left + iWidthCol0;
	rectAccracyText.left += iSkip;
	StrText = L"׼ȷ��";
	dc.DrawText(StrText, &rectAccracyText, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

	// ׼ȷ�� (��������)
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

	// ���������� 
	CRect rectProfitText = rectShow;
	rectProfitText.left  = rectAccracyRate.right;
	rectProfitText.right = rectProfitText.left  + iWidthCol2;
	StrText = L"������";
	dc.DrawText(StrText, &rectProfitText, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

	// ������ (��������)
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
	CFont* pOldFont = dc.SelectObject(pFontNormal); // fontΪҪ����߿������	
	
	CSize size = dc.GetTextExtent(L"һ������");
	m_iRowHeight = size.cy + 5;  
	
	// ��һ�е�
	CFont* pFontBig = GetIoViewFontObject(ESFBig);
	pOldFont = dc.SelectObject(pFontBig);
	size = dc.GetTextExtent(L"һ������");
	m_GridCtrl.SetRowHeight(0, (size.cy + 2));

	// ������
	for (int32 i = 0 ; i < m_GridCtrl.GetRowCount() ; i++)
	{
		m_GridCtrl.SetRowHeight(i,m_iRowHeight);
	}
}

void CIoViewDetail::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	// ֪ͨ��ͼ�ı��ע����Ʒ
	if (NULL == pMerch || m_pMerchXml == pMerch)
		return;
	
	if (NULL != pMerch)
	{
		// ���ù�ע����Ʒ��Ϣ
		m_aSmartAttendMerchs.RemoveAll();
		
		CSmartAttendMerch SmartAttendMerch;
		SmartAttendMerch.m_pMerch = pMerch;
		SmartAttendMerch.m_iDataServiceTypes = EDSTKLine;
		m_aSmartAttendMerchs.Add(SmartAttendMerch);
	}

	// �޸ĵ�ǰ�鿴����Ʒ
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;

	// ��ʼ����ʱ��,������������.�л���Ʒ��ʱ�����������Ʒ�б�,�ж��Ƿ��Ѿ��������ֵ	
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
	// ��������ֵ	
	
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

	// �ж��ǲ���IoView�Ľڵ�
	const char *pcValue = pElement->Value();
	if (NULL == pcValue || strcmp(GetXmlElementValue(), pcValue) != 0)
		return false;

	// �ж��ǲ��������Լ����ҵ����ͼ�Ľڵ�
	const char *pcAttrValue = pElement->Attribute(GetXmlElementAttrIoViewType());
	if (NULL == pcAttrValue || CIoViewManager::GetIoViewString(this).Compare(CString(pcAttrValue)) != 0)	// ���������Լ���ҵ��ڵ�
		return false;

	// ��ȡ��ҵ����ͼ���е�����
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

	// ��ǰ��ʾ��K ��
	pcAttrValue = pElement->Attribute(KStrElementAttriCurIndex);
	if ( NULL != pcAttrValue )
	{
		m_iCurShow = atoi(pcAttrValue);
	}
	//

	// Ȩ����Ϣ
	pcAttrValue = pElement->Attribute(KIoViewKLineWeightType);
	if ( NULL != pcAttrValue )
	{
		m_eWeightType = (CIoViewBase::E_WeightType)atoi(pcAttrValue);
		CIoViewBase::SetWeightType(m_eWeightType);
	}

	// ��ÿ�� K �ߵ�������Ϣ
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

				// ����
				pcAttrValue = pChildElement->Attribute(KStrElementAttriInterval);
				
				if ( NULL != pcAttrValue )
				{
					m_aIoViewDetailCells[iIndex].m_eNodeInterval = (E_NodeTimeInterval)atoi(pcAttrValue);						
				}

				// �Զ������
				pcAttrValue = pChildElement->Attribute(KStrElementAttriUserMins);

				if ( NULL != pcAttrValue )
				{
					m_aIoViewDetailCells[iIndex].m_iTimeUserMultipleMinutes = atoi(pcAttrValue);
				}

				// �Զ�������
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

	// ��Ʒ�����ı�
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

	// ������������
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
		// �����ǰ�ǲ���Ȩ��,ֱ�ӷ���		
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

	// 1: ��������û�г�Ȩ����
	if ( !pMerch->m_bHaveReadLocalWeightFile && 0 == pMerch->m_TimeLastUpdateWeightDatas.GetTime() )
	{
		// ���ض��ļ�ʧ�ܶ��Ҵӷ����������ݶ�ʧ�ܵ�ʱ��,
		// ��ͬʱ���� 1: �ٴζ������ļ� 2: �ٴη������������
		// �κ�һ��·���ɹ���,�Ͳ����ٶ��ļ���.���ǻ��ᷢ���������(2Сʱ�������)
		
		// ��һ�ε�ʱ��ӱ���ȡ,���ðѱ��ص�crc ֵ����������
		CArray<CWeightData,CWeightData&> aWeightData;
		UINT uiCrc32 = 0;
		
		if ( ReadWeightDataFromLocalFile(m_pMerchXml, aWeightData, uiCrc32) )
		{				
			pMerch->m_bHaveReadLocalWeightFile = true;
			
			// ����CMerch ������ֶε�ֵ				
			pMerch->m_uiWeightDataCRC = uiCrc32;
			
			int32 iSize = aWeightData.GetSize();
			pMerch->m_aWeightDatas.SetSize(iSize);				
			memcpyex(pMerch->m_aWeightDatas.GetData(), aWeightData.GetData(), sizeof(CWeightData) * iSize);					
		}	 
	}	
	
	// 2: �������������		
	CMmiReqPublicFile info;
	info.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	info.m_StrMerchCode		= pMerch->m_MerchInfo.m_StrMerchCode;
	info.m_ePublicFileType	= EPFTWeight;
	info.m_uiCRC32			= pMerch->m_uiWeightDataCRC;
		
	DoRequestViewData(info);
}

void CIoViewDetail::OnVDataPublicFileUpdate(IN CMerch *pMerch, E_PublicFileType ePublicFileType)
{
	// ...fangz1117  Ŀǰ UpdateMainMerchKLine ����ÿ�ζ� bUpdate = false ȫ����������,
	// �Ż��Ժ�,Ҫ�����Ȩ������,��Ҫ��Ϊ�����̺�û��K �������ʱ��������û�и���
	
	if (NULL == pMerch || pMerch != m_pMerchXml)
		return;
	
	if (EPFTWeight != ePublicFileType)	// �������Ȩ����
		return;
	
	// ������������
	OnVDataMerchKLineUpdate(pMerch);
}

void CIoViewDetail::OnVDataMerchKLineUpdate(IN CMerch *pMerch)
{
	// K �߸��µ�ʱ����Ҫ����,�����������е�����K ��,���ǽ������µ�ǰѡ�е��Ǹ�:
	// ��Ϊ: ��Ҫ��ͼ,������.�󲿷ֶ�������һ��K ��,������һ�����µ����,ÿ��K �߽�������һ��.�����кܴ�ļ�����,
	// ���Զ���һ��. ����: ���ֻ��һ��,��ô�л���ʱ��,��ȻҪȫ��������һ�����ڵ�,�ٶȷ�����
	
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
	
		// �Ȼ�ȡ��Ӧ��K��
		int32 iPosFound;
		CMerchKLineNode* pKLineRequest = NULL;
		pMerch->FindMerchKLineNode(eKLineType, iPosFound, pKLineRequest);
		
		// �����Ҳ���K�����ݣ� �ǾͲ���Ҫ��ʾ��
		if ( NULL == pKLineRequest || 0 == pKLineRequest->m_KLines.GetSize() )	
		{	
			return;
		}

		// ���,����,��������	
		CArray<CKLine,CKLine> aKLineBeforUpdate;	// ���ݵ�����,���ڸ����Ժ�ıȽ�
		aKLineBeforUpdate.RemoveAll();
		aKLineBeforUpdate.Copy(m_aIoViewDetailCells[i].m_aKLines);
	
		//
		{
			// ��������:
			int32 iTimeInitPerDay = 0, iTimeOpenPerDay = 0, iTimeClosePerDay = 0;
			
			CMerch *pTmpMerch = m_pMerchXml;
			
			if ( NULL == pTmpMerch	)
			{
				return;					
			}
			
			pTmpMerch->m_Market.m_MarketInfo.GetTimeInfo(iTimeInitPerDay, iTimeOpenPerDay, iTimeClosePerDay);				
			
			// �������:
						
			// ��Ȩ��Ȩ:
			CArray<CKLine, CKLine> aSrcKLines;
			aSrcKLines.Copy(pKLineRequest->m_KLines);
			
			// ���Ƿ�Ҫ����Ȩ����
			CArray<CKLine, CKLine> aWeightedKLines;
			
			if ( CIoViewKLine::EWTAft == m_eWeightType )
			{
				// ��Ȩ
				if (CMerchKLineNode::WeightKLine(aSrcKLines, pTmpMerch->m_aWeightDatas, false, aWeightedKLines))
				{
					aSrcKLines.Copy(aWeightedKLines);
				}		
			}
			else if ( CIoViewKLine::EWTPre == m_eWeightType )  
			{
				// ǰ��Ȩ
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

			// �������ݣ� ѹ������
			
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
					// ���������
				}
				break;
			default:
				//ASSERT(0);
				break;
			}	
		}

		// ���ø���ʱ��:
		// ����һ����ʱ��仯��,����������һ���򼸸�K ��,���� K �߷��ص��������˴�ı仯. Ҫ�������ʱ��:
		
		if( aKLineBeforUpdate.GetSize() <= 0 && m_aIoViewDetailCells[i].m_aKLines.GetSize() > 0 )
		{
			// ԭ��û����,��������:		
			SetUpdateTime(i);
		}
		else if ( aKLineBeforUpdate.GetSize() > 0 && m_aIoViewDetailCells[i].m_aKLines.GetSize() > 0 )
		{
			// 1: ����һ��K �ߵ�ʱ�䲻ͬ��(����һ�����������б仯)
			CGmtTime TimeKLineBefore = aKLineBeforUpdate.GetAt(aKLineBeforUpdate.GetSize() - 1).m_TimeCurrent;
			CGmtTime TimeKLineNow    = m_aIoViewDetailCells[i].m_aKLines.GetAt(m_aIoViewDetailCells[i].m_aKLines.GetSize()-1).m_TimeCurrent;
			
			if ( TimeKLineBefore != TimeKLineNow )
			{				
				SetUpdateTime(i);
			}					
			
			// 2: ����һ��K �ߵ����ںͷ��������ڵ�ʱ��,���ڲ�ͬ��(���ܹ�ҹ)
			// ԭ��: ���߻������ߵ�.��һ���ö�ʱ��,���ָ���ʱ�����������10 ��.���Բ�������ʱ��,���º���һֱû��������ʱ�������ø���ʱ���ʱ��
			if(m_pAbsCenterManager)
			{
				CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();

				if ( TimeKLineNow.GetDay() != TimeNow.GetDay() )
				{
					SetUpdateTime(i);
				}
			}
		}
	
		// ���µ�ǰ���ڵ�ָ��ֵ
		int32 iReval = CompareKLinesChange(aKLineBeforUpdate, m_aIoViewDetailCells[i].m_aKLines);
	
		{
			// ���Ƚ���ָ��:
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

			// ...fangz0705 ������Ҫ�Ż�.��û����,ȫ����������.			
			DEL(m_aIoViewDetailCells[i].m_pIndexMaForSane);
			m_aIoViewDetailCells[i].m_pIndexMaForSane = CalcIndexEMAForSane(m_aIoViewDetailCells[i].m_aKLines);
			
			int32 iSizeKLine = m_aIoViewDetailCells[i].m_aKLines.GetSize();

			if ( iSizeKLine < 1 || NULL == m_aIoViewDetailCells[i].m_pIndexMaForSane )
			{
				// ���ܼ�����Щ������ָ�� - �����ǳ��ִ�������������������������ݻ�ԭ�ɳ�ʼ
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
				// ���¼۱仯. ����һ��������, ����һ��������, ����������ָ��:
				if ( m_aIoViewDetailCells[i].m_aSaneIndexValues.GetSize() > 0 )
				{
					if ( UpDateLatestIndexForSane(m_aIoViewDetailCells[i].m_pIndexMaForSane, m_aIoViewDetailCells[i].m_aSaneIndexValues) )
					{
						// ����������								
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
				// ������һ��,�����µ�,�ӵ������������
				bool32 bKillTimer = false;

				// 1: ������һ��������
				CArray<CKLine, CKLine> aKLineTemp;
				aKLineTemp.Copy(m_aIoViewDetailCells[i].m_aKLines);
				int32 iSizeTemp = aKLineTemp.GetSize();
				
				if ( iSizeTemp > 0 )
				{
					aKLineTemp.RemoveAt(iSizeTemp - 1);
					CalcLatestSaneIndex(false, bJudgeKTOC, m_aIoViewDetailCells[i].m_pIndexMaForSane, aKLineTemp, m_aIoViewDetailCells[i].m_aSaneIndexValues, m_pAbsCenterManager->GetServerTime().GetTime(), bKillTimer);
					UpdateTableContent(1);
				}
				

				// 2: ��������һ��
				bool32 bOK = CalcLatestSaneIndex(true, bJudgeKTOC, m_aIoViewDetailCells[i].m_pIndexMaForSane, m_aIoViewDetailCells[i].m_aKLines, m_aIoViewDetailCells[i].m_aSaneIndexValues, m_pAbsCenterManager->GetServerTime().GetTime(), bKillTimer);
				
				if ( bOK )
				{
					// ����б�
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

				// �ı�ܴ�,��Ҫȫ������						
				bool32 bOK = CalcHistorySaneIndex(bJudgeKTOC, bPassUpdateTime, m_aIoViewDetailCells[i].m_pIndexMaForSane, m_aIoViewDetailCells[i].m_aKLines, m_aIoViewDetailCells[i].m_aSaneIndexValues);
				
				// ���㽻�״���,׼ȷ��,�����ʵ�ֵ
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
	// K �߸��µ�ʱ��,�ж����µ�K ����ԭ�������ʲô�仯:
	// ����ֵ: 0:��ͬ 1:����һ���仯 2:������һ�� 3:�����������,�д�ı仯

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
		// �ڵ�����ͬ,����ֻ������һ���ڵ�.��ʱ��Ҫ��ϸ�ж�,���ܸ����˶���ڵ��������ͬ,ֱ�ӷ���3;

		if ( iSizeAfter == iSizeBefore + 1)
		{
			// ����һ���ڵ�.�ж�һ���ǲ��ǳ������һ���ڵ�����,�����Ķ���ͬ,����ǵĻ�,�Ǿ��� 2

			KLineAfter.RemoveAt(iSizeAfter-1);
			pAfter  = (char*)KLineAfter.GetData();

			if ( 0 == memcmp(pBefore,pAfter,iSizeBefore*sizeof(CKLine)))
			{
				// �������һ��,������һ��,����������һ��
				return 2;
			}
			else
			{
				// ��һ��.
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
		// �ڵ������ͬ:
		if ( iSizeBefore == KMaxMinuteKLineCount )
		{
			// ����, 1W ���ڵ���: ���������ݲ嵽��һ��,�����һ���ߵ�.������ After ��ǰ 0~9999(ȥβ) �� Before �� 1~10000 �Ƚ�(ȥͷ)
			
			if ( 0 == memcmp(pBefore,pAfter,iSizeBefore*sizeof(CKLine)))
			{
				// ��ȫ��ͬ
				return 0;
			}

			KLineBefor.RemoveAt(0);
			KLineAfter.RemoveAt(iSizeAfter-1);

			pBefore = (char*)KLineBefor.GetData();
			pAfter  = (char*)KLineAfter.GetData();
			
			if ( 0 == memcmp(pBefore,pAfter,(iSizeBefore - 1)*sizeof(CKLine)))
			{
				// �������һ��,������һ��,����һ�������仯
				return 1;
			}
			else
			{
				// ��һ��
				return 3;
			}
		}
		else
		{
			// �������
			if ( 0 == memcmp(pBefore,pAfter,iSizeBefore*sizeof(CKLine)))
			{
				// ��ȫ��ͬ
				return 0;
			}
			
			//���������һ��ǰ���ǲ���һ����.
			
			KLineAfter.RemoveAt(iSizeAfter-1);
			KLineBefor.RemoveAt(iSizeBefore-1);
			
			pBefore = (char*)KLineBefor.GetData();
			pAfter  = (char*)KLineAfter.GetData();
			
			if ( 0 == memcmp(pBefore,pAfter,(iSizeBefore - 1)*sizeof(CKLine)))
			{
				// �������һ��,������һ��,����һ�������仯
				return 1;
			}
			else
			{
				// ��һ��.
				return 3;
			}
		}
					
	}
	
	return iReVal;
}

void CIoViewDetail::SetUpdateTime(int32 iIndex)
{
	// ����ʱ��: 1: ����һ��K �� 2: ��ǰ��һ�� K �ߵ����ڷ����˱仯 3: K �߱仯�ܴ� iReVal == 3
	if ( iIndex < 0 || iIndex >= KLINE_NUMS )
	{
		return;
	}

	int32 iKLineSize = m_aIoViewDetailCells[iIndex].m_aKLines.GetSize();
	if ( iKLineSize <= 0 )
	{
		return;
	}

	// ���ø���ʱ��:
	
	CGmtTime TimeNow;								// ��ǰ����һ��K �ߵ�ʱ��
	CGmtTime TimeClose;								// ��ǰ��Ʒ���������ʱ��
	
	bool32 bNeedTimer = true;						// �Ƿ���Ҫ��ʱ����������ָ��
		
	CMerch *pMerch = m_pMerchXml;
	
	if ( NULL == pMerch )
	{
		return;
	}

	CGmtTime TimeToUpdate = 0;

	// �õ���ʼʱ��:
	TimeNow = m_aIoViewDetailCells[iIndex].m_aKLines.GetAt(iKLineSize - 1).m_TimeCurrent;

	// ʱ�����,����
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
		// ��������Ѿ�������, �Ͳ�Ҫ�ٸ�����
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
				// ����Ҫ����ʵʱ����: 
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
				// ����Ҫ����ʵʱ����: 
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
				// ����Ҫ����ʵʱ����: 
				m_aIoViewDetailCells[iIndex].m_bNeedCalcSaneLatestValues	 = false;				
			}
		}		
	}
}

void CIoViewDetail::OnTimerUpdateLatestValuse(UINT uID)
{
	// ��ʱ��,����10 �����:
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

	// �õ���ǰʱ��
	CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();
	
	// 10 ���ʱ��.
	CGmtTimeSpan TimeSpan(0, 0, 0, SECONDSTOUPDATE);

	//
	int32 iInitial,iOpen,iClose;
	m_pMerchXml->m_Market.m_MarketInfo.GetTimeInfo(iInitial, iOpen, iClose);

	if ( KTimerIDUpdateKLine1Sane == uID )
	{
		// ����ĸ���ʱ��,���������ʱ��Ͳ�����
		CGmtTime TimeLast = m_aIoViewDetailCells[0].m_TimeToUpdateSane + TimeSpan;

		if ( BePassedCloseTime(m_pMerchXml) )
		{
			// ������, ����һ������һ��������(��ֹʵʱ����ʷ��һ��)
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
					// �Ѿ����һ����,��Ҫ������
					return;
				}
			}

			// �����һ�����Ƚ�����ָ��:						
			bool32 bKillTimer = false;
			
			if (CalcLatestSaneIndex(false, bJudgeKTOC, m_aIoViewDetailCells[0].m_pIndexMaForSane, m_aIoViewDetailCells[0].m_aKLines, m_aIoViewDetailCells[0].m_aSaneIndexValues, m_pAbsCenterManager->GetServerTime().GetTime(), bKillTimer))
			{
				ASSERT(bKillTimer);
			}

			if ( bKillTimer )
			{
				m_aIoViewDetailCells[0].m_bNeedCalcSaneLatestValues		= false;
			
				KillTimer(KTimerIDUpdateKLine1Sane);
				// �ж�һ�µ�ǰ���µĽڵ����������µ�һ���ڵ��Ƿ���ͬ
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
					// ����ͬ�Ľڵ�,�������һ��
					UpdateTableContent(1);
				}
				else
				{					
					// ����ͬһ��,Ҫ����һ��
					UpdateTableContent(2);
				}
				
				m_aIoViewDetailCells[0].m_aMerchsHaveCalcLatestValues.Add(m_pMerchXml);
			}
		}
		else if ( TimeNow > TimeLast )
		{
			// ��ʱ��,��Ҫ����.�������ø���ʱ��:
			m_aIoViewDetailCells[0].m_bNeedCalcSaneLatestValues = false;
			KillTimer(KTimerIDUpdateKLine1Sane);
			
			SetUpdateTime(0);
		}
	}
	else if ( KTimerIDUpdateKLine2Sane == uID )
	{
		// ����ĸ���ʱ��,���������ʱ��Ͳ�����
		CGmtTime TimeLast = m_aIoViewDetailCells[1].m_TimeToUpdateSane + TimeSpan;
		
		// ����ʱ��
		if ( BePassedCloseTime(m_pMerchXml) )
		{
			// ������, ����һ������һ��������(��ֹʵʱ����ʷ��һ��)
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
					// �Ѿ����һ����,��Ҫ������
					return;
				}
			}

			// ����ڶ������Ƚ�����ָ��:						
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
			// ��ʱ��,��Ҫ����.�������ø���ʱ��:
			m_aIoViewDetailCells[1].m_bNeedCalcSaneLatestValues = false;
			KillTimer(KTimerIDUpdateKLine2Sane);
			
			SetUpdateTime(1);
		}
	}
	else if ( KTimerIDUpdateKLine3Sane == uID )
	{
		// ����ĸ���ʱ��,���������ʱ��Ͳ�����
		CGmtTime TimeLast = m_aIoViewDetailCells[2].m_TimeToUpdateSane + TimeSpan;
		
		if ( BePassedCloseTime(m_pMerchXml) )
		{
			// ������, ����һ������һ��������(��ֹʵʱ����ʷ��һ��)
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
					// �Ѿ����һ����,��Ҫ������
					return;
				}
			}

			// ������������Ƚ�����ָ��:						
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
			// ��ʱ��,��Ҫ����.�������ø���ʱ��:
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
		// ѡ������:
		CNewMenu menu;
		menu.LoadMenu(IDR_MENU_KLINE);
		menu.LoadToolBar(g_awToolBarIconIDs);
		CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
		
		CMenu* pTempMenu = pPopMenu->GetSubMenu(_T("ѡ������")); 
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

		bSuccess = menu.AppendODMenu(L"�������", MF_STRING, IDM_IOVIEW_DETAIL_TREND);
	//	bSuccess = menu.AppendODMenu(L"���ָ��", MF_STRING, IDM_IOVIEW_DETAIL_SANE);

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

		// ��������
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