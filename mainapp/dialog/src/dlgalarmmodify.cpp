// dlgalarmmodify.cpp : implementation file
//

#include "stdafx.h"
#include "dlgalarmmodify.h"
#include "DlgAlarm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAlarmModify dialog
 

CDlgAlarmModify::CDlgAlarmModify(CWnd* pParent /*=NULL*/,E_AlarmType eType)
	: CDialogEx(CDlgAlarmModify::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAlarmModify)
	m_bTrend = FALSE;
	m_fPriceDown  = 0.0f;
	m_fPriceUp    = 0.0f;	
	m_fRiseRate   = 0.0f;
	m_fChangeRate = 0.0f;
	//}}AFX_DATA_INIT
	m_eAlarmType = eType;
}


void CDlgAlarmModify::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAlarmModify)
	DDX_Check(pDX, IDC_CHECK1, m_bTrend);
	DDX_Text(pDX, IDC_EDIT_DOWN, m_fPriceDown);
	DDX_Text(pDX, IDC_EDIT_RISERATE, m_fRiseRate);
	DDX_Text(pDX, IDC_EDIT_CHANGERATE, m_fChangeRate);
	DDX_Text(pDX, IDC_EDIT_UP, m_fPriceUp);	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAlarmModify, CDialogEx)
	//{{AFX_MSG_MAP(CDlgAlarmModify)	
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_ALLSETTING, &CDlgAlarmModify::OnClickedAllSetting)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAlarmModify message handlers
BOOL CDlgAlarmModify::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_fPriceUp		= m_stAlarmConditions.m_aFixedParams[0];
	m_fPriceDown	= m_stAlarmConditions.m_aFixedParams[1];
	m_fRiseRate     = m_stAlarmConditions.m_aFixedParams[2];
	m_fChangeRate   = m_stAlarmConditions.m_aFixedParams[3];
	m_bTrend		= m_stAlarmConditions.m_aFixedParams[4] == 1.0 ? true : false;
	
	//
	if (EAT_MERCH == m_eAlarmType)
	{
		GetDlgItem(IDC_EDIT_RISERATE)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_CHANGERATE)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_RISERATE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_CHANGERATE)->EnableWindow(FALSE);
	}
	//
	UpdateData(FALSE);

	return TRUE;
}

void CDlgAlarmModify::SetAlarmConditions(const CAlarmConditions& stAlarmConditions)
{
	m_stAlarmConditions = stAlarmConditions;
}

void CDlgAlarmModify::GetAlarmConditions(CAlarmConditions& stAlarmConditions)
{
	stAlarmConditions = m_stAlarmConditions;
}

bool32 CDlgAlarmModify::ValidParams(OUT CString& StrErr)
{
	UpdateData(TRUE);

	StrErr.Empty();

	bool32 bUnInitial = true;

	for ( int32 i = 0; i < FIX_ALM_CDS; i ++ )
	{
		if ( m_stAlarmConditions.m_aFixedParams[i] != 0. )
		{
			bUnInitial = false;
			break;
		}
	}

	if ( bUnInitial )
	{
		return true;
	}

	//
	if ( (0. != m_fPriceUp) || (0. != m_fPriceDown) )
	{
		if ( m_fPriceUp <= m_fPriceDown )
		{
			StrErr = L"上破价必须大于下破价";
			return false;
		}		
	}

	return true;
}

void CDlgAlarmModify::OnOK() 
{
	CString StrErr;
	
	if ( !ValidParams(StrErr) )
	{
		MessageBox(StrErr, AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return;
	}
	
	m_stAlarmConditions.m_aFixedParams[0] = m_fPriceUp;
	m_stAlarmConditions.m_aFixedParams[1] = m_fPriceDown;
	m_stAlarmConditions.m_aFixedParams[2] = m_fRiseRate;
	m_stAlarmConditions.m_aFixedParams[3] = m_fChangeRate;
	m_stAlarmConditions.m_aFixedParams[4] = m_bTrend;

	CDialogEx::OnOK();
}

void CDlgAlarmModify::OnClickedAllSetting()
{
	CMainFrame *pMainFrm = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	
	if ( !pMainFrm || !pMainFrm->m_pDlgAlarm)
	{
		return;
	}
	
 	if (pMainFrm->m_pDlgAlarm->IsWindowVisible())
 	{
 		pMainFrm->m_pDlgAlarm->ShowWindow(SW_HIDE);
 	}
 	else
 	{
 		pMainFrm->m_pDlgAlarm->ShowWindow(SW_SHOW);		
 	}

	PostMessage(WM_CLOSE, 0, 0);
}
