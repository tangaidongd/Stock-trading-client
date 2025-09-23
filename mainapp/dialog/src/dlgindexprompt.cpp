// dlgindexprompt.cpp : implementation file
//

#include "stdafx.h"



#include "dlgindexprompt.h"
#include "StatusBarEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgIndexPrompt dialog


CDlgIndexPrompt::CDlgIndexPrompt(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgIndexPrompt::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgIndexPrompt)
	m_StrShow = _T("");
	//}}AFX_DATA_INIT
}

void CDlgIndexPrompt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgIndexPrompt)
	DDX_Text(pDX, IDC_STATIC_SHOW, m_StrShow);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgIndexPrompt, CDialogEx)
	//{{AFX_MSG_MAP(CDlgIndexPrompt)
		// NOTE: the ClassWizard will add message map macros here
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgIndexPrompt message handlers

BOOL CDlgIndexPrompt::PreTranslateMessage(MSG* pMsg)
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

BOOL CDlgIndexPrompt::OnInitDialog()
{
	// 设置窗体位置;
	
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
	
	// ... 超出正常尺寸范围的处理. 得到屏幕尺寸的函数?
	// GetSystemMetrics(),SM_CXFULLSCREEN,SM_CYFULLSCREEN 全屏幕窗口的窗口区域的宽度和高度
	
	int32 iCx = GetSystemMetrics(SM_CXFULLSCREEN);
	int32 iCy = GetSystemMetrics(SM_CYFULLSCREEN);
	
	if( rectOut.top < 0 )
	{
		rectOut.top=0;
	}
	if( rectOut.left < 0 )
	{
		rectOut.left=0;
		rectOut.right = rectOut.left + rectIn.Width();
	}
	if( rectOut.right > iCx ) 
	{
		rectOut.left  = iCx - rectIn.Width();
		rectOut.right = iCx-2;
	}
	if( rectOut.bottom > iCy ) 
	{	
		rectOut.top = iCy - rectIn.Height(); 
		rectOut.bottom = iCy + 20;
	}
	
	MoveWindow(rectOut);

	CDialogEx::OnInitDialog();
	return TRUE; 
}

BOOL CDlgIndexPrompt::OnEraseBkgnd(CDC* pDC)
{	
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(&rect,::GetSysColor(COLOR_BTNFACE));
	
	return FALSE;
}

void CDlgIndexPrompt::OnSize(UINT nType, int cx, int cy)
{
	CRect rectClinet;
	GetClientRect(rectClinet);

	CWnd* pWnd = GetDlgItem(IDC_STATIC_SHOW);
	
	if ( NULL != pWnd )
	{
		pWnd->MoveWindow(&rectClinet);
	}
	
	Invalidate();	
	CDialogEx::OnSize(nType, cx, cy);
}

void CDlgIndexPrompt::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	delete this;
}

void CDlgIndexPrompt::OnClose()
{
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	pWnd->m_pDlgIndexPrompt = NULL;
	
	DestroyWindow();
}

void CDlgIndexPrompt::ShowMessage(const CString& StrShow)
{
	m_StrShow = StrShow;
	UpdateData(FALSE);
}