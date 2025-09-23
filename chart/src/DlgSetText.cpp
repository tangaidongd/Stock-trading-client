#include "StdAfx.h"
#include "DlgSetText.h"
#include "TabSplitWnd.h"


CDlgSetText::CDlgSetText(CWnd* pParent /*=NULL*/)
: CDialogEx(CDlgSetText::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSetText)
	//}}AFX_DATA_INIT
}

void CDlgSetText::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSetText)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgSetText, CDialogEx)
	//{{AFX_MSG_MAP(CDlgSetText)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDlgSetText::OnOK()
{
	CWnd::GetDlgItem(IDC_EDIT1)->GetWindowText(m_StrText);
	CDialog::OnOK();
}

void CDlgSetText::OnCancel()
{
	CWnd::GetDlgItem(IDC_EDIT1)->GetWindowText(m_StrText);
	CDialog::OnCancel();
}

BOOL CDlgSetText::OnInitDialog()
{
	CDialog::OnInitDialog();
	UINT uID = CTabSplitWnd::m_pMainFram->SendMessage(UM_GetIDRMainFram);
	HICON hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(uID));
	SetIcon(hIcon,FALSE);

	return TRUE;
}
