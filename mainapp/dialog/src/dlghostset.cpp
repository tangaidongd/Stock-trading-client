// dlghostset.cpp : implementation file
//

#include "stdafx.h"

#include "dlghostset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgHostSet dialog
 

CDlgHostSet::CDlgHostSet(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgHostSet::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgHostSet)
	m_StrHostAddress = _T("");
	m_StrHostName = _T("");
	m_uiHostPort = 0;
	//}}AFX_DATA_INIT 
}


void CDlgHostSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgHostSet)
	DDX_Text(pDX, IDC_EDIT_HOSTADDRESS, m_StrHostAddress);
	DDX_Text(pDX, IDC_EDIT_HOSTNAME, m_StrHostName);
	DDX_Text(pDX, IDC_EDIT_HOSTPORT, m_uiHostPort);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgHostSet, CDialogEx)
	//{{AFX_MSG_MAP(CDlgHostSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgHostSet message handlers

void CDlgHostSet::OnCancel() 
{
	
	CDialog::OnCancel();
}

void CDlgHostSet::OnOK() 
{
	
	CDialog::OnOK();
}
