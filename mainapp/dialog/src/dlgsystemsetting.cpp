// dlgsystemsetting.cpp : implementation file
//

#include "stdafx.h"

#include "dlgsystemsetting.h"
#include "facescheme.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSystemSetting dialog


CDlgSystemSetting::CDlgSystemSetting(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgSystemSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSystemSetting)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgSystemSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSystemSetting)
	DDX_Control(pDX, IDC_TAB1, m_TabSheet);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSystemSetting, CDialogEx)
	//{{AFX_MSG_MAP(CDlgSystemSetting)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSystemSetting message handlers

BOOL CDlgSystemSetting::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetWindowText(_T("风格设置"));
	HICON hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,FALSE);
	
	m_TabSheet.AddPage(L"外观",&m_DlgSystemFace,IDD_DIALOG_SYSFACE);
	//m_TabSheet.AddPage(L"系统参数",&m_DlgSystemParameter,IDD_DIALOG_SYSPARAMETER);
	m_TabSheet.Show();
	return TRUE;  	              
}
void CDlgSystemSetting::OnOK() 
{	
	int32 iCurSel = m_TabSheet.GetCurSel();	
	CDlgSystemFace * pDlgSysFace =(CDlgSystemFace*)m_TabSheet.GetPage(iCurSel);
	pDlgSysFace->SaveSysFace();

	CDialog::OnOK();
}

void CDlgSystemSetting::OnCancel() 
{
	CDialog::OnCancel();
}
