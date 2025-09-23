// dlgsystemparameter.cpp : implementation file
//

#include "stdafx.h"

#include "dlgsystemparameter.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSystemParameter dialog


CDlgSystemParameter::CDlgSystemParameter(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgSystemParameter::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSystemParameter)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgSystemParameter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSystemParameter)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSystemParameter, CDialogEx)
	//{{AFX_MSG_MAP(CDlgSystemParameter)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSystemParameter message handlers

BOOL CDlgSystemParameter::PreTranslateMessage(MSG* pMsg) 
{
	if ( WM_KEYDOWN == pMsg->message)
	{
		if ( VK_ESCAPE == pMsg->wParam)
		{
			return false;
		}
		if ( VK_RETURN == pMsg->wParam)
		{
			return true;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}


