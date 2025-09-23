// dlgmanual.cpp : implementation file
//

#include "stdafx.h"
#include "dlgmanual.h"
#include "GridCellBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int32 KiTimerIdAutoPing		= 1;			// 自动ping (主要是为了防止一直停留在界面不做操作被服务器T了. 没认证的时候不能发心跳包. 这个当心跳包发了)
static const int32 KiTimerPeriodAutoPing	= 1000 * 5;			

#define UM_OptimizeServerNotify	(WM_USER + 0x123)

/////////////////////////////////////////////////////////////////////////////
// CDlgManual dialog


CDlgManual::CDlgManual(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgManual::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgManual)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_aServers.RemoveAll();
	m_bForcePinging = false;
}

CDlgManual::~CDlgManual()
{
	// 向 COptimizeServer 注销自己
	COptimizeServer::Instance()->RemoveNotify(this);
}

void CDlgManual::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgManual)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgManual, CDialogEx)
	//{{AFX_MSG_MAP(CDlgManual)
	ON_BN_CLICKED(IDC_BUTTON_RECEHCK, OnButtonRecheck)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, OnButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnButtonDown)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_MESSAGE(UM_OptimizeServerNotify, OnMsgOptimizeServerNotify)
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_DBLCLK, 21235, OnGridDblClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgManual message handlers
BOOL CDlgManual::PreTranslateMessage(MSG* pMsg)
{
	if ( WM_KEYDOWN == pMsg->message)
	{
		if (VK_ESCAPE == pMsg->wParam || VK_RETURN == pMsg->wParam)
		{
			OnClose();
		}
	}	

	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgManual::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
	if ( m_Grid.GetSafeHwnd() )
	{
		m_Grid.ShowWindow(SW_SHOW);
		OnSizeChanged();		
	}
}

BOOL CDlgManual::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	
	// 向 COptimizeServer 注册自己
	COptimizeServer::Instance()->AddNotify(this);

	// 初始化表格
	ConstructGrid();
	UpDateGrid();

	//
	SetTimer(KiTimerIdAutoPing, KiTimerPeriodAutoPing, NULL);

	return TRUE;  	              
}

void CDlgManual::OnFinishAllPing()
{
	// Ping 完了
	SendMessage(UM_OptimizeServerNotify, ON_ALL_PING, NULL);

	//
	if ( m_bForcePinging )
	{
		SetTimer(KiTimerIdAutoPing, KiTimerPeriodAutoPing, NULL);
	}
}

void CDlgManual::OnFinishPingServer(CServerState stServer)
{
	CServerState* p = new CServerState();
	*p = stServer;

	//
	SendMessage(UM_OptimizeServerNotify, ON_PING_SERVER, (LPARAM)p);	
}

void CDlgManual::OnFinishSortServer()
{
	SendMessage(UM_OptimizeServerNotify, ON_SORT_SERVER, NULL);	
}

void CDlgManual::SetInitialParams(IN const arrServer& aServers)
{
	m_aServers.Copy(aServers);
}

void CDlgManual::ChangeControlState()
{
	if ( m_bForcePinging )
	{
		// 重新ping
		GetDlgItem(IDC_BUTTON_UP)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow(FALSE);

		GetDlgItem(IDC_BUTTON_RECEHCK)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(FALSE);

		m_Grid.EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_UP)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow(TRUE);
		
		GetDlgItem(IDC_BUTTON_RECEHCK)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(TRUE);
		
		m_Grid.EnableWindow(TRUE);
	}

//	UpdateData(FALSE);
}

void CDlgManual::ConstructGrid()
{	
	if ( !m_Grid.Create(CRect(0, 0, 0, 0), this, 21235) )
	{
		return;
	}

	// Get the font
	CFont* pFont = m_Grid.GetFont();
	LOGFONT lf;
	pFont->GetLogFont(&lf);
	memcpyex(lf.lfFaceName, _T("Arial"), 6);
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	m_Grid.GetDefaultCell(TRUE, FALSE)->SetFont(&lf);
	m_Grid.GetDefaultCell(FALSE, TRUE)->SetFont(&lf);
	m_Grid.GetDefaultCell(TRUE, TRUE)->SetFont(&lf);
	m_Grid.GetDefaultCell(FALSE, FALSE)->SetFont(&lf);
	
	m_Grid.GetDefaultCell(TRUE, FALSE)->SetMargin(0);
	m_Grid.GetDefaultCell(FALSE, TRUE)->SetMargin(0);
	m_Grid.GetDefaultCell(TRUE, TRUE)->SetMargin(0);
	m_Grid.GetDefaultCell(FALSE, FALSE)->SetMargin(0);

	m_Grid.SetEditable(false);
	m_Grid.SetFixedRowCount(1);
	m_Grid.SetColumnCount(6);
	
	CGridCellBase *pCell = NULL;
	pCell = m_Grid.GetCell(0, 0);
	ASSERT(NULL != pCell);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	pCell->SetText(L"名称");
	
	pCell = m_Grid.GetCell(0, 1);
	ASSERT(NULL != pCell);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	pCell->SetText(L"地址");
	
// 	pCell = m_Grid.GetCell(0, 2);
// 	ASSERT(NULL != pCell);
// 	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
// 	pCell->SetText(L"组");

	pCell = m_Grid.GetCell(0, 2);
	ASSERT(NULL != pCell);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	pCell->SetText(L"类型");

// 	pCell = m_Grid.GetCell(0, 4);
// 	ASSERT(NULL != pCell);
// 	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
// 	pCell->SetText(L"在线人数");

	pCell = m_Grid.GetCell(0, 3);
	ASSERT(NULL != pCell);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	pCell->SetText(L"最大在线");

	pCell = m_Grid.GetCell(0, 4);
	ASSERT(NULL != pCell);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	pCell->SetText(L"ping");

	pCell = m_Grid.GetCell(0, 5);
	ASSERT(NULL != pCell);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	pCell->SetText(L"综合得分");

	//隐藏地址和最大数
	m_Grid.SetColumnWidth(1,0);
	m_Grid.SetColumnWidth(3,0);
	//
	OnSizeChanged();

}

void CDlgManual::OnSizeChanged()
{
	CRect RectClient;
	GetClientRect(&RectClient);

	const int32 KiAdjustHigh  = 5;
	const int32 KiAdjustWidth = 5;
	
	CRect RectUp;
	GetDlgItem(IDC_BUTTON_UP)->GetWindowRect(RectUp);
	ScreenToClient(RectUp);

	int32 iButtonUpHeight = RectUp.Height();
	int32 iButtonUpWidth  = RectUp.Width();

	RectUp.bottom = RectClient.Height() / 2 - KiAdjustHigh;
	RectUp.top	  = RectUp.bottom - iButtonUpHeight;
	RectUp.right  = RectClient.right - KiAdjustWidth;
	RectUp.left	  = RectUp.right - iButtonUpWidth;
	
	GetDlgItem(IDC_BUTTON_UP)->MoveWindow(&RectUp);

	//
	CRect RectDown;
	GetDlgItem(IDC_BUTTON_DOWN)->GetWindowRect(RectDown);
	ScreenToClient(RectDown);

	RectDown.left	= RectUp.left;
	RectDown.right	= RectUp.right;
	RectDown.top	= RectClient.Height() / 2 + KiAdjustHigh;	
	RectDown.bottom	= RectDown.top + iButtonUpHeight;
	
	GetDlgItem(IDC_BUTTON_DOWN)->MoveWindow(&RectDown);

	//
	CRect RectRecheck;
	GetDlgItem(IDC_BUTTON_RECEHCK)->GetWindowRect(RectRecheck);
	ScreenToClient(RectRecheck);
	
	int32 iButtonRecheckHeight = RectRecheck.Height();
	int32 iButtonRecheckWidth  = RectRecheck.Width();

	RectRecheck.bottom = RectClient.bottom - KiAdjustHigh;
	RectRecheck.top	   = RectRecheck.bottom - iButtonRecheckHeight;
	RectRecheck.left   = RectClient.left + KiAdjustWidth;
	RectRecheck.right  = RectRecheck.left + iButtonRecheckWidth;

	GetDlgItem(IDC_BUTTON_RECEHCK)->MoveWindow(&RectRecheck);

	//
	CRect RectConnect;
	GetDlgItem(IDC_BUTTON_CONNECT)->GetWindowRect(RectConnect);
	ScreenToClient(RectConnect);

	RectConnect.top		= RectRecheck.top;
	RectConnect.bottom	= RectRecheck.bottom;
	RectConnect.left	= RectRecheck.right + KiAdjustWidth;
	RectConnect.right	= RectConnect.left + iButtonRecheckWidth;

	GetDlgItem(IDC_BUTTON_CONNECT)->MoveWindow(&RectConnect);

	//
	CRect RectGrid;
	RectGrid.top	= RectClient.top + 2;
	RectGrid.bottom = RectConnect.top - 2;
	RectGrid.left	= RectClient.left + 2;
	RectGrid.right	= RectDown.left - 2;

	m_Grid.MoveWindow(&RectGrid);
	m_Grid.ExpandColumnsToFit();
}

void CDlgManual::UpDateGrid()
{
	if ( m_aServers.GetSize() <= 0 )
	{
		return;
	}

	//
	m_Grid.DeleteNonFixedRows();

	//
	for ( int32 i = 0; i < m_aServers.GetSize(); i++)
	{
		CServerState stServer = m_aServers[i];		
		InsertOneRow(stServer);
	}

	m_Grid.Refresh();
}

void CDlgManual::InsertOneRow(const CServerState& stServerState, int32 iRow /*= -1*/)
{	
	if ( iRow >= m_Grid.GetRowCount() )
	{
		iRow = -1;
	}

	m_Grid.InsertRow(L"", iRow);
	
	int32 iRowIndex = iRow;
	if ( -1 == iRow )
	{
		iRowIndex = m_Grid.GetRowCount() - 1;
	}
	
	//
	CString StrCell;
	
	// 名称
	CGridCellBase* pCell = m_Grid.GetCell(iRowIndex, 0);		
	ASSERT(NULL != pCell);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	pCell->SetText(stServerState.m_StrServerName);

	// 地址
	pCell = m_Grid.GetCell(iRowIndex, 1);		
	ASSERT(NULL != pCell);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	pCell->SetText(stServerState.m_StrServerAddr);
		
	// 组
// 	pCell = m_Grid.GetCell(iRowIndex, 2);		
// 	ASSERT(NULL != pCell);
// 	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
// 	StrCell.Format(L"%d", stServerState.m_iGroup);
// 	pCell->SetText(StrCell);
		
	// 类型
	pCell = m_Grid.GetCell(iRowIndex, 2);		
	ASSERT(NULL != pCell);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	StrCell = COptimizeServer::Instance()->GetNetTypeString((E_NetType)stServerState.m_iNetType);
	pCell->SetText(StrCell);
	
	// 在线
// 	pCell = m_Grid.GetCell(iRowIndex, 4);		
// 	ASSERT(NULL != pCell);
// 	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
// 	StrCell.Format(L"%d", stServerState.m_iOnLineCount);
// 	pCell->SetText(StrCell);
	
	// 最大在线
	pCell = m_Grid.GetCell(iRowIndex, 3);		
	ASSERT(NULL != pCell);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	StrCell.Format(L"%d", stServerState.m_iMaxCount);
	pCell->SetText(StrCell);
	
	// ping
	pCell = m_Grid.GetCell(iRowIndex, 4);		
	ASSERT(NULL != pCell);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

	if ( COptimizeServer::Instance()->GetMaxPingValue() == stServerState.m_uiPingValue )
	{
		StrCell = L"超时";
	}
	else
	{
		StrCell.Format(L"%u", stServerState.m_uiPingValue);
	}

	pCell->SetText(StrCell);
	
	// 得分
	pCell = m_Grid.GetCell(iRowIndex, 5);		
	ASSERT(NULL != pCell);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	StrCell.Format(L"%d", stServerState.m_iSortScore);

	pCell->SetText(StrCell);
}

void CDlgManual::UpDateOneRow(const CServerState& stServerState)
{
	//
	int32 iRowIndex = -1;

	for ( int32 i = 0; i < m_aServers.GetSize(); i++ )
	{
		CServerState stCell = m_aServers[i];

		if ( COptimizeServer::Instance()->BeServerIdentical(stServerState, stCell) )
		{
			iRowIndex = i + 1;
			break;
		}
	}

	if ( -1 == iRowIndex )
	{
		return;
	}
	
	//
	CString StrServerAddr = stServerState.m_StrServerAddr;
	TRACE(L"---> 更新表格: 服务器 %s:%d 得分: 【ping = %u】total: %d \n", StrServerAddr.GetBuffer(), stServerState.m_uiServerPort,
		  stServerState.m_uiPingValue, stServerState.m_iSortScore);

	//
	CString StrCell;
	
	// ping
	CGridCellBase* pCell = m_Grid.GetCell(iRowIndex, 4);		
	ASSERT(NULL != pCell);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);	

	if ( COptimizeServer::Instance()->GetMaxPingValue() == stServerState.m_uiPingValue )
	{
		StrCell = L"超时";
	}
	else
	{
		StrCell.Format(L"%u", stServerState.m_uiPingValue);
	}
	pCell->SetText(StrCell);

	// 得分
	pCell = m_Grid.GetCell(iRowIndex, 5);		
	ASSERT(NULL != pCell);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	StrCell.Format(L"%d", stServerState.m_iSortScore);
	
	pCell->SetText(StrCell);
	
	m_Grid.Refresh();
}

void CDlgManual::ForcePing()
{
	m_bForcePinging = true;
	ChangeControlState();
	
	//
	COptimizeServer::Instance()->SetForcePingFlag(true);
	COptimizeServer::Instance()->ForceRePingServer();
}

void CDlgManual::GetFinalResult(int32 iRowDbClick /*= -1*/)
{
	iRowDbClick -= 1;
	int32 i = 0;
	for ( i = 1; i < m_Grid.GetRowCount()-1; i++ )
	{
		CServerState Server = m_aServers.GetAt(i);
		
		//
		if ( (-1 != iRowDbClick) && (i == iRowDbClick) )
		{	
			m_aServers.RemoveAt(i);
			m_aServers.InsertAt(0, Server);
		}	
	}
	
	// 排序
	for ( i = 0; i < m_aServers.GetSize(); i++ )
	{
		m_aServers[i].m_iSortIndex = i;
	}
}

void CDlgManual::OnButtonRecheck() 
{
	KillTimer(KiTimerIdAutoPing);
	SetTimer(KiTimerIdAutoPing, KiTimerPeriodAutoPing, NULL);

	ForcePing();
}

void CDlgManual::OnButtonConnect() 
{
	OnClose();
}

void CDlgManual::OnButtonUp() 
{
	CCellRange cellRange = m_Grid.GetSelectedCellRange();
	int32 iCurSel = cellRange.GetMinRow();

	if ( iCurSel <= 1 || iCurSel >= m_Grid.GetRowCount() )
	{
		return;
	}

	CGridCellBase* pCell = m_Grid.GetCell(iCurSel, 0);
	if ( NULL == pCell )
	{
		return;
	}

	if ((iCurSel-1) < m_aServers.GetSize())
	{
		CServerState Server = m_aServers.GetAt(iCurSel-1);
		InsertOneRow(Server, iCurSel - 1);
		m_Grid.DeleteRow(iCurSel + 1);

		m_aServers[iCurSel-1] = m_aServers[iCurSel-2];
		m_aServers[iCurSel-2] = Server;
	}

	CCellRange cellRangeNow = cellRange;
	cellRangeNow.SetMinRow(cellRange.GetMinRow()-1);
	cellRangeNow.SetMaxRow(cellRange.GetMinRow()-1);

	m_Grid.SetSelectedRange(cellRangeNow);
	
	m_Grid.RedrawWindow();
}

void CDlgManual::OnButtonDown() 
{
	CCellRange cellRange = m_Grid.GetSelectedCellRange();
	int32 iCurSel = cellRange.GetMinRow();
	
	if ( iCurSel <= 0 || iCurSel >= m_Grid.GetRowCount() - 1  )
	{
		return;
	}
	
	CGridCellBase* pCell = m_Grid.GetCell(iCurSel, 0);
	if ( NULL == pCell )
	{
		return;
	}
	
	if ((iCurSel-1) < m_aServers.GetSize())
	{
		CServerState Server = m_aServers.GetAt(iCurSel-1);
		m_Grid.DeleteRow(iCurSel);
		InsertOneRow(Server, iCurSel + 1);

		m_aServers[iCurSel-1] = m_aServers[iCurSel];
		m_aServers[iCurSel] = Server;
	}
	
	CCellRange cellRangeNow = cellRange;
	cellRangeNow.SetMinRow(cellRange.GetMinRow() + 1);
	cellRangeNow.SetMaxRow(cellRange.GetMinRow() + 1);	
	m_Grid.SetSelectedRange(cellRangeNow);
	
	m_Grid.RedrawWindow();
}
 
void CDlgManual::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	
	int32 iRow = pItem->iRow;
	int32 iCol = pItem->iColumn;

	if ( !m_Grid.IsValid(iRow, iCol) )
	{
		return;
	}

	//
	GetFinalResult(iRow);
	COptimizeServer::Instance()->SetManualSortResult(m_aServers);
	CDialog::OnOK();
}

void CDlgManual::OnClose()
{
	if ( m_bForcePinging )
	{
		return;
	}

	// 置顶情况下隐藏会导致mainframe跟着消失一会，造成闪烁
	//ShowWindow(SW_HIDE);

	KillTimer(KiTimerIdAutoPing);

	//
	CCellRange cell = m_Grid.GetSelectedCellRange();
	
	if ( !cell.IsValid() )
	{
		GetFinalResult();
	}
	else
	{
		GetFinalResult(cell.GetMinRow());
	}
	
	//
	COptimizeServer::Instance()->SetManualSortResult(m_aServers);
	CDialog::OnOK();
}

void CDlgManual::OnTimer(UINT nIDEvent)
{
	if ( nIDEvent == KiTimerIdAutoPing )
	{
		KillTimer(nIDEvent);
		ForcePing();
	}
}


LRESULT CDlgManual::OnMsgOptimizeServerNotify( WPARAM w, LPARAM l )
{
	int iNotify = (int)w;
	switch (iNotify)
	{
	case ON_SORT_SERVER:
		{
			if ( m_bForcePinging )
			{
				// 更新界面显示
				COptimizeServer::Instance()->SetForcePingFlag(false);
				COptimizeServer::Instance()->GetSortedServers(m_aServers);
				
				//
				m_bForcePinging = false;
				ChangeControlState();
				
				UpDateGrid();			
			}
		}
		break;
	case ON_ALL_PING:
		{
			// 暂无处理
		}
		break;
	case ON_PING_SERVER:
		{
			CServerState* pServer = (CServerState*)l;
			ASSERT( NULL != pServer );

			// Ping 完某台服务器
			if ( m_bForcePinging && NULL != pServer )
			{
				CServerState stServerState = *pServer;

				//
				int32 iScore = COptimizeServer::Instance()->CalcServerScore(stServerState);
				stServerState.m_iSortScore = iScore;
				
				UpDateOneRow(stServerState);		

				//
				DEL(pServer);
			}
		}
		break;
	default:
		ASSERT( 0 );
	}
	
	return 1;
}
