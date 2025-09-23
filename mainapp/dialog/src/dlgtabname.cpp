// dlgtabname.cpp : implementation file
//

#include "stdafx.h"

#include "dlgtabname.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTabName dialog


CDlgTabName::CDlgTabName(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgTabName::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTabName)
	m_StrName = _T("");
	//}}AFX_DATA_INIT
}


void CDlgTabName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTabName)
	DDX_Text(pDX, IDC_EDIT_NAME, m_StrName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTabName, CDialogEx)
	//{{AFX_MSG_MAP(CDlgTabName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTabName message handlers
BOOL CDlgTabName::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	CEdit *pEditCtrl = (CEdit *)GetDlgItem(IDC_EDIT_NAME);
	if (NULL != pEditCtrl)
	{
		pEditCtrl->SetSel(0, m_StrName.GetLength());
	}
	
	return	TRUE;
}

CString CDlgTabName::GetName()
{
	return m_StrName;
}

void CDlgTabName::SetName(CString StrName)
{
	m_StrName = StrName;
}

void CDlgTabName::OnOK() 
{		
	CDialog::OnOK();
}