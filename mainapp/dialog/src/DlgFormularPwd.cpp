// DlgFormularPwd.cpp : implementation file
//

#include "stdafx.h"

#include "DlgFormularPwd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgFormularPwd dialog


CDlgFormularPwd::CDlgFormularPwd(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgFormularPwd::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgFormularPwd)
	m_StrPwd = _T("");
	//}}AFX_DATA_INIT
}


void CDlgFormularPwd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFormularPwd)
	DDX_Text(pDX, IDC_EDIT1, m_StrPwd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgFormularPwd, CDialogEx)
	//{{AFX_MSG_MAP(CDlgFormularPwd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFormularPwd message handlers
CString CDlgFormularPwd::GetPwd()
{
	return m_StrPwdMy;
}

void CDlgFormularPwd::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	m_StrPwdMy = m_StrPwd;

	CDialogEx::OnOK();
}
