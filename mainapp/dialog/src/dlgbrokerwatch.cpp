// dlgbrokerwatch.cpp : implementation file
//

#include "stdafx.h"

#include "facescheme.h"
#include "dlgbrokerwatch.h"
#include "GridCellSys.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgBrokerWatch dialog


CDlgBrokerWatch::CDlgBrokerWatch(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgBrokerWatch::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgBrokerWatch)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgBrokerWatch::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBrokerWatch)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBrokerWatch, CDialogEx)
	//{{AFX_MSG_MAP(CDlgBrokerWatch)
		// NOTE: the ClassWizard will add message map macros here
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_NOTIFY(GVN_SELCHANGED,443216,OnGridSelChanged)
	ON_NOTIFY(NM_DBLCLK,443217,OnGridDblClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBrokerWatch message handlers
BOOL CDlgBrokerWatch::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CenterWindow();
	
	CreateGrid();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
BOOL CDlgBrokerWatch::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
		{
			PostMessage(WM_CLOSE,0,0);
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}
void CDlgBrokerWatch::OnClose()
{
	CMainFrame* pWnd		= (CMainFrame*)AfxGetMainWnd();
	pWnd->m_pDlgBrokerWatch = NULL;
	
	DestroyWindow();

}
void CDlgBrokerWatch::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	delete this;
}
bool32 CDlgBrokerWatch::CreateGrid()
{
	CRect rectGrid,rectClient;
	CWnd * pWnd = GetDlgItem(IDC_STATIC_GRID);
	pWnd->GetWindowRect(&rectGrid);
	GetClientRect(&rectClient);
	ScreenToClient(&rectGrid);
		
	m_XSBVertLeft.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 20203);
	m_XSBVertLeft.SetScrollRange(0, 10);
	m_XSBVertLeft.ShowWindow(SW_SHOW);
	
	m_XSBVertRight.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 20204);
	m_XSBVertRight.SetScrollRange(0, 0);
	m_XSBVertRight.ShowWindow(SW_SHOW);
	
	
	if (NULL == m_GridCtrlLeft.GetSafeHwnd())
	{
		if (!m_GridCtrlLeft.Create(CRect(0, 0, 0, 0), this, 443216))
			return false;
		m_GridCtrlLeft.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
		m_GridCtrlLeft.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
		m_GridCtrlLeft.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	}
	
	if (NULL == m_GridCtrlRight.GetSafeHwnd())
	{
		if (!m_GridCtrlRight.Create(CRect(0, 0, 0, 0), this, 443217))
			return false;
		m_GridCtrlRight.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
		m_GridCtrlRight.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
		m_GridCtrlRight.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	}


	// 设置列数
	if (!m_GridCtrlLeft.SetColumnCount(1))
		return false;
	if (!m_GridCtrlLeft.SetRowCount(100))
		return false;
	
	if (!m_GridCtrlRight.SetColumnCount(2))
		return false;
	if (!m_GridCtrlRight.SetRowCount(100))
		return false;

	m_GridCtrlLeft.SetRowHeight(0,40);
	m_GridCtrlRight.SetRowHeight(0,40);
	
	// 设置表格图标
	// m_ImageList.Create(MAKEINTRESOURCE(IDB_NET_ONOFF), 16, 1, RGB(255,255,255));
	// m_GridCtrlLeft.SetImageList(&m_ImageList);
	// m_GridCtrlRight.SetImageList(&m_ImageList);
	
	// 设置相互之间的关联			
	
	m_XSBVertLeft.SetOwner(&m_GridCtrlLeft);
	m_XSBVertRight.SetOwner(&m_GridCtrlRight);
	m_GridCtrlLeft.SetScrollBar(NULL, &m_XSBVertLeft);
	m_GridCtrlRight.SetScrollBar(NULL, &m_XSBVertRight);
	
	CRect rectGridLeft,rectGridRight;
	rectGridLeft = rectGrid;
	rectGridLeft.right = 100;

	rectGridRight = rectGrid;
	rectGridRight.left = rectGridLeft.right+1;

	m_GridCtrlLeft.MoveWindow(&rectGridLeft);
	m_GridCtrlLeft.ExpandColumnsToFit();

	
	m_GridCtrlRight.MoveWindow(&rectGridRight);
	m_GridCtrlRight.ExpandColumnsToFit();


	CGridCellSys * pCell = (CGridCellSys *)m_GridCtrlLeft.GetCell(0,0);
	pCell->SetTextClr(RGB(255,255,100));	
	LOGFONT * pLgFont = CFaceScheme::Instance()->GetSysFont(ESFBig);
	pCell->SetFont(pLgFont);
	pCell->SetText(L"经纪");

	pCell = (CGridCellSys *)m_GridCtrlLeft.GetCell(1,0);
	LOGFONT * pLgFontSmall = CFaceScheme::Instance()->GetSysFont(ESFSmall);
	pCell->SetFont(pLgFontSmall);
	pCell->SetText(L"01");

	pCell = (CGridCellSys *)m_GridCtrlRight.GetCell(0,0);
	pCell->SetTextClr(RGB(255,255,100));	
	pCell->SetFont(pLgFont);
	pCell->SetText(L"买盘信息(10)");

	pCell = (CGridCellSys *)m_GridCtrlRight.GetCell(0,1);
	pCell->SetTextClr(RGB(255,255,100));	
	pCell->SetFont(pLgFont);
	pCell->SetText(L"卖盘信息(20)");

	return true;
}
void CDlgBrokerWatch::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	// 双击这项,跳到相应的行情界面
}
void CDlgBrokerWatch::OnGridSelChanged(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	// 列表选择切换时,更新右边表格
}