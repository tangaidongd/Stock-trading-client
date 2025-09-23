// DlgTimeWait.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgTimeWait.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// CDlgTimeWait 对话框

IMPLEMENT_DYNAMIC(CDlgTimeWait, CDialog)

CDlgTimeWait::CDlgTimeWait(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTimeWait::IDD, pParent)
{
	m_iTimeElapse = 0;
	m_bShowTimer = false;
	m_bUseProgress = false;
}

CDlgTimeWait::~CDlgTimeWait()
{
}

void CDlgTimeWait::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_ctrlProgress);
	DDX_Control(pDX, IDC_STATIC_PROMPT, m_ctrlTip);
}


BEGIN_MESSAGE_MAP(CDlgTimeWait, CDialog)
//	ON_NOTIFY(NM_CUSTOMDRAW, IDC_PROGRESS1, &CDlgTimeWait::OnNMCustomdrawProgress1)
ON_WM_TIMER()
END_MESSAGE_MAP()

int CDlgTimeWait::ShowTimeWaitDlg( bool bShowTimer, int iTimeElapse, bool bUseProgress, LPCTSTR lpszTitle, LPCTSTR lpszTip )
{
	m_bShowTimer = bShowTimer;
	m_bUseProgress = bUseProgress;
	m_iTimeElapse = iTimeElapse;
	m_StrTip = lpszTip;
	m_StrTitle = lpszTitle;

	return DoModal();
}

BOOL CDlgTimeWait::OnInitDialog()
{
	if ( !CDialog::OnInitDialog() )
	{
		return FALSE;
	}

	if ( !m_bUseProgress )
	{
		m_ctrlProgress.ShowWindow(SW_HIDE);
		CRect rc;
		GetWindowRect(rc);
		CRect rcP;
		m_ctrlProgress.GetClientRect(rcP);
		rc.bottom -= rcP.Height();
		//MoveWindow(rc);
	}

	m_ctrlTip.SetWindowText(m_StrTip);
	SetWindowText(m_StrTitle);

	if ( m_iTimeElapse < 1 )
	{
		m_bShowTimer = false;
	}
	else
	{
		SetTimer(1, 1000, NULL);
	}

	if ( m_bShowTimer )
	{
		ShowTimer();
	}

	return TRUE;
}


// CDlgTimeWait 消息处理程序

void CDlgTimeWait::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if ( 1 == nIDEvent )
	{
		ShowTimer();
		if ( --m_iTimeElapse < 0 )
		{
			EndDialog(IDCANCEL);
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void CDlgTimeWait::ShowTimer()
{
	if ( m_bShowTimer )
	{
		CWnd *pWnd = GetDlgItem(IDCANCEL);
		if ( pWnd->GetSafeHwnd() == NULL )
		{
			return;
		}

		if ( m_StrCancel.IsEmpty() )
		{
			pWnd->GetWindowText(m_StrCancel);
		}

		CString StrTime;
		StrTime.Format(_T("(%d)"), m_iTimeElapse);
		pWnd->SetWindowText(m_StrCancel + StrTime);
	}
}

void CDlgTimeWait::CancelDlg(int iResult)
{
	if ( IsWindow(m_hWnd) )
	{
		EndDialog(iResult);
	}
}

void CDlgTimeWait::SetProgress( int iPos, int iRange )
{
	if ( IsWindow(m_ctrlProgress.m_hWnd) )
	{
		m_ctrlProgress.SetRange32(0, iRange);
		m_ctrlProgress.SetPos(iPos);
	}
}

void CDlgTimeWait::SetTipText( const CString &StrTip )
{
	SetDlgItemText(IDC_STATIC_PROMPT, StrTip);
	m_StrTip = StrTip;
}

void CDlgTimeWait::AppendToShowTitle( const CString &StrAppend )
{
	CString Str = m_StrTitle + _T(" - [") + StrAppend + _T("]");
	SetWindowText(Str);
}

void CDlgTimeWait::SetTitle( const CString &StrTitle )
{
	m_StrTitle = StrTitle;

//--- wangyongxue 2016/09/29 这一句会引起程序错误，暂时先屏蔽掉算了
//	SetWindowText(StrTitle);
}