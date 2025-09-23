// dlgbuding.cpp : implementation file
//

#include "stdafx.h"
#include "dlgbuding.h"
#include "DlgTrace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgbuding dialog


CDlgbuding::CDlgbuding(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgbuding::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgbuding)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pDlgTrace = NULL;
}


void CDlgbuding::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgbuding)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgbuding, CDialog)
	//{{AFX_MSG_MAP(CDlgbuding)
		// NOTE: the ClassWizard will add message map macros here
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgbuding message handlers
BOOL CDlgbuding::OnInitDialog()
{
	CDialog::OnInitDialog();

#ifdef TRACE_DLG
	// 创建trace对话框
	m_pDlgTrace = new CDlgTrace();
	m_pDlgTrace->Create(IDD_TRACE, this);
	CRect rt(0,0,500,700);
	m_pDlgTrace->SetWindowPos(&wndTop, 0, 0, rt.Width(), rt.Height(), SWP_HIDEWINDOW);

	g_hwndTrace = m_pDlgTrace->GetSafeHwnd();
	
	CString StrHwnd;
	StrHwnd.Format(L"%d", (long)g_hwndTrace);
	::WritePrivateProfileString(L"TRACE", L"HWND",	StrHwnd, L"./trace.ini");
#endif

	return TRUE;
}

void CDlgbuding::OnDestroy()
{
	if (NULL != m_pDlgTrace)
	{
		m_pDlgTrace->DestroyWindow();
		DEL(m_pDlgTrace);
	}
}
