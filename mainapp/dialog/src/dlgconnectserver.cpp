// dlgconnectserver.cpp : implementation file
//

#include "stdafx.h"

#include "dlgconnectserver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgConnectServer dialog


CDlgConnectServer::CDlgConnectServer(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgConnectServer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgConnectServer)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgConnectServer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConnectServer)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgConnectServer, CDialog)
	//{{AFX_MSG_MAP(CDlgConnectServer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgConnectServer message handlers

void CDlgConnectServer::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}
