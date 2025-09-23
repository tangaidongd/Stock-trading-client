// dialogprogresschoosestock.cpp : implementation file
//
#include "stdafx.h"
#include "ShareFun.h"


#include "dialogprogresschoosestock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogProgressChooseStock dialog


CDialogProgressChooseStock::CDialogProgressChooseStock(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogProgressChooseStock::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogProgressChooseStock)
	m_StrMsg	= _T("");
	//}}AFX_DATA_INIT
	// m_iProgress = 0;
}


void CDialogProgressChooseStock::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogProgressChooseStock)
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Text(pDX, IDC_STATIC_MSG, m_StrMsg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogProgressChooseStock, CDialogEx)
	//{{AFX_MSG_MAP(CDialogProgressChooseStock)
		// NOTE: the ClassWizard will add message map macros here
	ON_MESSAGE(UM_Choose_Stock_Progress,OnMsgStepProgress)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogProgressChooseStock message handlers
BOOL CDialogProgressChooseStock::OnInitDialog()
{
	CDialog::OnInitDialog();


	return TRUE;
}

LRESULT CDialogProgressChooseStock::OnMsgStepProgress(WPARAM wParam , LPARAM lParam)
{
	CString * pStrMsg = (CString *)wParam;
	m_StrMsg = *pStrMsg;
	DEL(pStrMsg);

	m_Progress.StepIt();
	
	UpdateData(false);
	return TRUE;
}