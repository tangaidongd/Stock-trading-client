#include "StdAfx.h"

#include "DlgIE.h"
#include "FontFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgIE dialog

CDlgIE::CDlgIE(CWnd* pParent /*=NULL*/)
: CDialogEx(CDlgIE::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgIE)
	//}}AFX_DATA_INIT
}


void CDlgIE::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgIE)
	DDX_Control(pDX, IDC_RICHEDIT1, m_RichEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgIE, CDialogEx)
//{{AFX_MSG_MAP(CDlgIE)
ON_WM_SIZE()
ON_NOTIFY(EN_LINK, IDC_RICHEDIT, OnRichEditExLink)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDlgIE::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	unsigned int mask = ::SendMessage(m_RichEdit.m_hWnd, EM_GETEVENTMASK, 0, 0);   
	::SendMessage(m_RichEdit.m_hWnd, EM_SETEVENTMASK,  0, mask | ENM_LINK | ENM_MOUSEEVENTS | ENM_SCROLLEVENTS | ENM_KEYEVENTS);   
	::SendMessage(m_RichEdit.m_hWnd, EM_AUTOURLDETECT, true, 0);

	SetWindowText(m_StrTitle);

//	m_wndCef.SetUrl(m_StrUrl);
//	m_wndCef.Create(NULL,NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), this, 9878);
	m_wndCef.CreateWndCef(9878,this,CRect(0,0,0,0),m_StrUrl);


	RecalcLayout();
	
	return TRUE;
}

void CDlgIE::OnOK()
{
	// 检查一下值
	
	CDialogEx::OnOK();
}

void CDlgIE::PromptErrorInput( const CString &StrPrompt, CWnd *pWndFocus /*= NULL*/ )
{
	AfxMessageBox(StrPrompt, MB_ICONWARNING |MB_OK);
	if ( NULL != pWndFocus )
	{
		pWndFocus->SetFocus();
		if ( pWndFocus->IsKindOf(RUNTIME_CLASS(CEdit)) )
		{
			((CEdit *)pWndFocus)->SetSel(0, -1);
		}
		else if ( pWndFocus->IsKindOf(RUNTIME_CLASS(CRichEditCtrl)) )
		{
			((CRichEditCtrl *)pWndFocus)->SetSel(0, -1);
		}
	}
}

void CDlgIE::SetTitleUrl( const CString &StrTitle, const CString &StrUrl, const CString &StrContent )
{
	m_StrTitle = StrTitle;
	m_StrUrl = StrUrl;
	m_StrContent = StrContent;
	if ( NULL != m_hWnd )
	{
		SetWindowText(m_StrTitle);
		CString StrUrlTmp(StrUrl);
		StrUrlTmp.TrimLeft();
		StrUrlTmp.MakeUpper();
		if ( !StrUrl.IsEmpty()/* || StrContent.IsEmpty() */)
		{
			m_RichEdit.ShowWindow(SW_HIDE);
			m_wndCef.ShowWindow(SW_SHOW);
			m_wndCef.SetFocus();
			m_wndCef.OpenUrl(StrUrl);
		}
		else
		{
			m_RichEdit.ShowWindow(SW_SHOW);
			m_wndCef.ShowWindow(SW_HIDE);
			m_RichEdit.SetFocus();

			m_RichEdit.SetOneFormat(gFontFactory.GetExistFontName(L"微软雅黑"), RGB(230, 230, 250));	//...
			m_RichEdit.SetWindowText(StrContent);	// 设置字串，也不讲究格式话了
		}
	}
}

void CDlgIE::OnSize( UINT nType, int cx, int cy )
{
	CDialogEx::OnSize(nType, cx, cy);
	RecalcLayout();
}

void CDlgIE::RecalcLayout()
{
	if ( NULL != m_wndCef.m_hWnd )
	{
		CRect rc;
		GetClientRect(rc);
		m_wndCef.MoveWindow(rc);
		m_RichEdit.MoveWindow(rc);
		Invalidate();
	}
	
	if(m_RichEdit.GetSafeHwnd() && m_RichEdit.IsWindowVisible())
	{
		CRect rc;
		GetClientRect(rc);
		m_RichEdit.MoveWindow(rc);
		Invalidate();
	}
}

void CDlgIE::ShowDlgIE( const CString &StrTitle, const CString &StrUrl, const CString &StrContent )
{
	static CDlgIE s_dlg;
	if ( s_dlg.m_hWnd == NULL )
	{
		s_dlg.Create(IDD, AfxGetMainWnd());
		s_dlg.CenterWindow();
	}

	if ( s_dlg.m_hWnd != NULL )
	{
		s_dlg.SetTitleUrl(StrTitle, StrUrl, StrContent);
		s_dlg.ShowWindow(SW_SHOW);
	}
}

void CDlgIE::OnCancel()
{
	DestroyWindow();	// 销毁
}

void CDlgIE::OnRichEditExLink( NMHDR* in_pNotifyHeader, LRESULT* out_pResult )
{
	ENLINK* pENLink = (ENLINK*)in_pNotifyHeader;   
	*out_pResult = 0;   
	
	switch(pENLink->msg)   
	{   
	case WM_LBUTTONDOWN:   
		{   
			CString   StrUrl;   
			CHARRANGE stCharRange;   
			
			CRichEditCtrl * pTempEdit = (CRichEditCtrl*)CRichEditCtrl::FromHandle(pENLink->nmhdr.hwndFrom);
			if ( NULL == pTempEdit )
			{
				return;
			}
			
			pTempEdit->GetSel(stCharRange);   
			pTempEdit->SetSel(pENLink->chrg);
			
			StrUrl = pTempEdit->GetSelText();   
			pTempEdit->SetSel(stCharRange);   
			
			CWaitCursor WaitCursor;   
			
			ShellExecute(this->GetSafeHwnd(), L"open", StrUrl, NULL, NULL, SW_SHOWNORMAL);   
			*out_pResult   =   1   ;   
		}   
		break;   
	}   
}
