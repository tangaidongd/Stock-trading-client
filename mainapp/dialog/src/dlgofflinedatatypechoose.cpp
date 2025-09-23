// dlgofflinedatatypechoose.cpp : implementation file
//

#include "stdafx.h"

#include "dlgofflinedatatypechoose.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgOffLineDataTypeChoose dialog


CDlgOffLineDataTypeChoose::CDlgOffLineDataTypeChoose(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgOffLineDataTypeChoose::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgOffLineDataTypeChoose)
	m_iRatio = 0;
	//}}AFX_DATA_INIT
}


void CDlgOffLineDataTypeChoose::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOffLineDataTypeChoose)
	DDX_Radio(pDX, IDC_RADIO1, m_iRatio);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgOffLineDataTypeChoose, CDialogEx)
	//{{AFX_MSG_MAP(CDlgOffLineDataTypeChoose)
		// NOTE: the ClassWizard will add message map macros here	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOffLineDataTypeChoose message handlers
int32 CDlgOffLineDataTypeChoose::GetDownLoadType()
{	
	return m_iRatio;
}

void CDlgOffLineDataTypeChoose::OnOK()
{	
	UpdateData(true);
	CDialog::OnOK();
}
