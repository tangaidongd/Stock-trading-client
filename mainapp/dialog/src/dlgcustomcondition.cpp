// dlgcustomcondition.cpp : implementation file
//
#include "stdafx.h"
#include "dlgcustomcondition.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgCustomCondition dialog

CDlgCustomCondition::CDlgCustomCondition(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgCustomCondition::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgCustomCondition)
	m_iIndexCmp = -1;
	m_fValue = 0.0f;
	m_StrPost = _T("");
	m_StrName = _T("");
	m_bAdd = false;
	//}}AFX_DATA_INIT
}

void CDlgCustomCondition::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCustomCondition)
	DDX_CBIndex(pDX, IDC_COMBO_CMP, m_iIndexCmp);
	DDX_Text(pDX, IDC_EDIT1, m_fValue);
	DDX_Text(pDX, IDC_STATIC_POST, m_StrPost);
	DDX_Text(pDX, IDC_STATIC_NAME, m_StrName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgCustomCondition, CDialogEx)
	//{{AFX_MSG_MAP(CDlgCustomCondition)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCustomCondition message handlers
BOOL CDlgCustomCondition::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if ( m_bAdd )
	{
		SetWindowTextEx(L"增加定制条件");	
	}
	else
	{
		SetWindowTextEx(L"修改定制条件");	
	}

	m_fValue = m_stParma.m_fValue;	
	m_iIndexCmp = m_stParma.m_iCmp + 1;

	UpdateData(FALSE);
	return TRUE;
}

void CDlgCustomCondition::SetParmas(bool32 bAdd, const T_CustomCdtCell& stParam, const CString& StrName, const CString& StrPost)
{
	m_bAdd = bAdd;
	m_stParma = stParam;
	m_StrName = StrName;
	m_StrPost = StrPost;
}

void CDlgCustomCondition::GetResult(OUT T_CustomCdtCell& stParam)
{
	stParam = m_stParma;
}

void CDlgCustomCondition::OnOK() 
{
	UpdateData(TRUE);
	m_stParma.m_iCmp	= m_iIndexCmp - 1;
	m_stParma.m_fValue	= m_fValue;
	
	CDialogEx::OnOK();
}


