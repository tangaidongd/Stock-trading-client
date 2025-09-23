#include "StdAfx.h"

#include "DlgNewNote.h"
#include "GmtTime.h"


#include "MerchManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgNewNote dialog

CDlgNewNote::CDlgNewNote(CWnd* pParent /*=NULL*/)
: CDialogEx(CDlgNewNote::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgNewNote)
	//}}AFX_DATA_INIT
}


void CDlgNewNote::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgNewNote)
	DDX_Control(pDX, IDC_EDIT_TITLE, m_EditTitle);
	DDX_Control(pDX, IDC_RICHEDIT1, m_EditContent);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_DateTimePick);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgNewNote, CDialogEx)
//{{AFX_MSG_MAP(CDlgNewNote)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDlgNewNote::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设定note的初始值
	m_EditTitle.SetWindowText(m_note.m_StrTitle);
	m_EditContent.SetWindowText(m_note.m_StrContent);

	int iWeatherId = m_note.m_iWeather - EUNW_SunShine + IDC_RADIO_SUN;
	
	CWnd *pWndRadio = GetDlgItem(iWeatherId);
	ASSERT( NULL != pWndRadio );
	if ( NULL != pWndRadio )
	{
		((CButton *)pWndRadio)->SetCheck(BST_CHECKED);
	}

	CTime tC(m_note.m_Time.GetTime());
	m_DateTimePick.SetTime(&tC);

	SetWindowText(GetDlgTitle());

	return TRUE;
}

void CDlgNewNote::InitNote( const T_UserNoteInfo &note )
{
	m_note = note;
}

void CDlgNewNote::OnOK()
{
	// 检查一下值
	const int32 iTitleLength = m_EditTitle.GetWindowTextLength();
	if ( iTitleLength > USERNOTE_MAX_TITLE_LENGTH )
	{
		CString StrPrompt;
		StrPrompt.Format(_T("标题不能超出 %d 个字符，您输入了 %d 字符"), USERNOTE_MAX_TITLE_LENGTH, iTitleLength);
		PromptErrorInput(StrPrompt, &m_EditTitle);
		return;
	}
	else if ( 0 == iTitleLength )
	{
		CString StrPrompt;
		StrPrompt.Format(_T("标题不能为空"));
		PromptErrorInput(StrPrompt, &m_EditTitle);
		return;
	}
	const int32 iContentLength = m_EditContent.GetWindowTextLength();
	if ( iContentLength > USERNTOE_MAX_CONTENT_LENGTH )
	{
		CString StrPrompt;
		StrPrompt.Format(_T("内容不能超出 %d 个字符，您输入了 %d 字符"), USERNTOE_MAX_CONTENT_LENGTH, iContentLength);
		PromptErrorInput(StrPrompt, &m_EditContent);
		return;
	}

	int iWeather = GetCheckedRadioButton(IDC_RADIO_SUN, IDC_RADIO_SNOW) - IDC_RADIO_SUN;
	if ( 0 > iWeather )
	{
		iWeather = EUNW_SunShine;
	}
	m_note.m_iWeather = iWeather;

	CTime tC;
	m_DateTimePick.GetTime(tC);
	m_note.m_Time = tC.GetTime();

	m_EditTitle.GetWindowText(m_note.m_StrTitle);
	m_EditContent.GetWindowText(m_note.m_StrContent);


	CDialogEx::OnOK();
}

void CDlgNewNote::PromptErrorInput( const CString &StrPrompt, CWnd *pWndFocus /*= NULL*/ )
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

CString CDlgNewNote::GetDlgTitle()
{
	CString Str;
	CString StrAction;
	if ( m_note.m_iUniqueId != 0 )
	{
		StrAction = _T("编辑投资日记");
	}
	else
	{
		StrAction = _T("撰写投资日记");
	}

	Str = StrAction;
	if ( !m_note.m_MerchKey.m_StrMerchCode.IsEmpty() )
	{
		CMerch *pMerch = NULL;
		if ( AfxGetDocument() != NULL && AfxGetDocument()->m_pAbsCenterManager != NULL )
		{
			CAbsCenterManager *pAbsCenterManager = AfxGetDocument()->m_pAbsCenterManager;
			if (!pAbsCenterManager)
			{
				return Str;
			}

			if ( pAbsCenterManager->GetMerchManager().FindMerch(m_note.m_MerchKey.m_StrMerchCode, m_note.m_MerchKey.m_iMarketId, pMerch) )
			{
				Str.Format(_T("%s [%s-%s]"), StrAction.GetBuffer(), pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), m_note.m_MerchKey.m_StrMerchCode.GetBuffer());
			}
			else
			{
				Str.Format(_T("%s [%s]"), StrAction.GetBuffer(), m_note.m_MerchKey.m_StrMerchCode.GetBuffer());
			}
		}
	}
	return Str;
}

bool32 CDlgNewNote::ModifyNote( INOUT T_UserNoteInfo &note )
{
	CDlgNewNote dlg(AfxGetMainWnd());
	
	dlg.InitNote(note);
	if ( dlg.DoModal() == IDOK )
	{
		note = dlg.m_note;
		return true;
	}
	return false;
}
