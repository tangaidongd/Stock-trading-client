#include "StdAfx.h"
#include "DlgInputShowName.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgInputShowName dialog
IMPLEMENT_DYNCREATE(CDlgInputShowName, CDialogEx)

CDlgInputShowName::CDlgInputShowName(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgInputShowName::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgInputShowName)
	m_StrTip  = _T("请输入显示字串");
	m_StrInput = _T("");
	//}}AFX_DATA_INIT
}

void CDlgInputShowName::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgInputShowName)
	DDX_Text(pDX, IDC_EDIT1, m_StrInput);
	DDX_Text(pDX, IDC_STATIC_TIP, m_StrTip);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgInputShowName, CDialogEx)
	//{{AFX_MSG_MAP(CDlgInputShowName)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgInputShowName message handlers



BOOL CDlgInputShowName::OnInitDialog()
{
	// 设置窗体位置;
	CDialogEx::OnInitDialog();
	UpdateData(TRUE);

	if ( !m_StrTitleOrg.IsEmpty() )
	{
		SetWindowText(m_StrTitleOrg);
	}
	if ( !m_StrTipOrg.IsEmpty() )
	{
		m_StrTip = m_StrTipOrg;
	}
	if ( !m_StrInputOrg.IsEmpty() )
	{
		m_StrInput = m_StrInputOrg;
	}
	UpdateData(FALSE);

	return TRUE; 
}

void CDlgInputShowName::PostNcDestroy()
{
	CDialogEx::PostNcDestroy();
}

int CDlgInputShowName::GetInputString( INOUT CString &StrInput, const CString &StrTip /*= _T("")*/, const CString &StrTitle /*= _T("")*/ )
{
	CDlgInputShowName	dlg(NULL);
	dlg.m_StrTipOrg = StrTip;
	dlg.m_StrTitleOrg = StrTitle;

	dlg.m_StrInputOrg = StrInput;
	int iRet = dlg.DoModal();
	if ( iRet == IDOK )
	{
		StrInput = dlg.m_StrInput;
	}
	else
	{
		// 如果是取消则不修改
		//StrInput.Empty();
	}


	return iRet;
}

void CDlgInputShowName::OnOK()
{
	UpdateData();
	if ( m_StrInput.IsEmpty() )
	{
		MessageBox(_T("您不能输入空的字串！"), _T("输入字串"), MB_OK |MB_ICONERROR);
		return;
	}

	CDialogEx::OnOK();
}
