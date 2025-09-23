// dlgspirite.cpp : implementation file
//

#include "stdafx.h"

#include "dlgspirite.h"

#include "GridCellSys.h"
#include "StatusBarEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSpirite dialog


CDlgSpirite::CDlgSpirite(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSpirite::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSpirite)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgSpirite::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSpirite)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSpirite, CDialog)
	//{{AFX_MSG_MAP(CDlgSpirite)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSpirite message handlers

void CDlgSpirite::OnClose() 
{
	CMainFrame* pWnd    = (CMainFrame*)AfxGetMainWnd();
	pWnd->m_pDlgSpirite = NULL;
	DestroyWindow();
}
void CDlgSpirite::PostNcDestroy() 
{	
	CDialog::PostNcDestroy();
	delete this;	
}
BOOL CDlgSpirite::PreTranslateMessage(MSG* pMsg) 
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
BOOL CDlgSpirite::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
 	CRect rectIn,rectOut,rectMain,rectStatus;
  	
  	GetClientRect(rectIn);
  	AfxGetMainWnd()->GetClientRect(rectMain); 
  	AfxGetMainWnd()->ClientToScreen(rectMain);
  	((CMainFrame *)AfxGetMainWnd())->m_wndStatusBar.GetWindowRect(rectStatus);
  	
  	rectOut.left = rectMain.right - rectIn.Width() - 2;
  	rectOut.top  = rectMain.bottom - rectIn.Height() - rectStatus.Height() - 18  ;

  	rectOut.right = rectOut.left + rectIn.Width();
  	rectOut.bottom = rectOut.top + rectIn.Height() +15;
  	MoveWindow(rectOut);
	
	CreateGrid();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

bool32 CDlgSpirite::CreateGrid()
{
	CRect  rect;
	GetClientRect(&rect);
	m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10203);
	m_XSBVert.SetScrollRange(0, 10);
	m_XSBVert.ShowWindow(SW_SHOW);
	
	m_XSBHorz.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10204);
	m_XSBHorz.SetScrollRange(0, 0);
	m_XSBHorz.ShowWindow(SW_SHOW);


	if (NULL == m_GridCtrl.GetSafeHwnd())
	{
		if (!m_GridCtrl.Create(CRect(0, 0, 0, 0), this, 123456))
			return false;
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0,0,0));
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetTextClr(RGB(255,0,0));
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	}
	 
	if (!m_GridCtrl.SetColumnCount(5))
		return false;
	if (!m_GridCtrl.SetRowCount(10))
		return false;

	//m_GridCtrl.SetFixedRowCount(1);
	//m_GridCtrl.SetFixedColumnCount(1);
	
	// 设置表格图标
	m_ImageList.Create(MAKEINTRESOURCE(IDB_GRID_REPORT), 16, 1, RGB(255,255,255));
	m_GridCtrl.SetImageList(&m_ImageList);
	
	// 设置相互之间的关联
	
	m_XSBHorz.SetOwner(&m_GridCtrl);
	m_XSBVert.SetOwner(&m_GridCtrl);
	m_GridCtrl.SetScrollBar(&m_XSBHorz, &m_XSBVert);
	
	m_GridCtrl.MoveWindow(rect,true);
	m_GridCtrl.ExpandColumnsToFit();
	//m_GridCtrl.ExpandToFit();

	
	CGridCellSys *pCell = NULL;
	
	for (int32 i = 0; i< 5 ; i++)
	{
		pCell = (CGridCellSys *)m_GridCtrl.GetCell(0,i);
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
		//CString StrText;
		//StrText.Format(L"服务器%d",i);
		//pCell->SetText(StrText);
		pCell->SetText(L"111");
	}

	for (int32 j = 1; j < 10 ; j++)
	{
		pCell = (CGridCellSys *)m_GridCtrl.GetCell(j,0);
		pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
		CString StrText;
		StrText.Format(L"服务%d",j);
		pCell->SetText(StrText);				
	}

	


	return true;

	//... fangz0512 差单击列表发送消息的接口.
}