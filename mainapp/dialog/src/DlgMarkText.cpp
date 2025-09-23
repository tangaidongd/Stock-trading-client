#include "StdAfx.h"

#include "DlgMarkText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgMarkText dialog

CDlgMarkText::CDlgMarkText(CWnd* pParent /*=NULL*/)
: CDialogEx(CDlgMarkText::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMarkText)
	//}}AFX_DATA_INIT

	m_pMerch = NULL;
	m_bMarkAdded = false;
}


void CDlgMarkText::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMarkText)
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMarkText, CDialogEx)
//{{AFX_MSG_MAP(CDlgMarkText)
ON_BN_CLICKED(IDC_BUTTON_DEL, OnBtnDel)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDlgMarkText::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �趨��ʼֵ
	if ( NULL == m_pMerch )
	{
		EndDialog(IDCANCEL);
		return TRUE;
	}

	CString StrTitle;
	GetWindowText(StrTitle);
	CString StrTitleNew;
	StrTitleNew.Format(_T("%s - %s[%s]"), StrTitle.GetBuffer(), m_pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), m_pMerch->m_MerchInfo.m_StrMerchCode.GetBuffer());
	SetWindowText(StrTitleNew);

	// �����������
	T_MarkData MarkData;
	if ( CMarkManager::Instance().QueryMark(m_pMerch, MarkData)
		&& MarkData.m_eType == EMT_Text )
	{
		m_Edit.SetWindowText(MarkData.m_StrTip);
	}
	
	return TRUE;
}

void CDlgMarkText::OnOK()
{
	// ���һ��ֵ
	T_MarkData MarkData;
	MarkData.m_eType = EMT_Text;
	m_Edit.GetWindowText(MarkData.m_StrTip);
	if ( MarkData.m_StrTip.IsEmpty() )
	{
		PromptErrorInput(_T(""), &m_Edit);	// ����ʾ��ȡ���˴β���
		return;
	}
	else if ( MarkData.m_StrTip.GetLength() > 2048 )
	{
		PromptErrorInput(_T("���ֱ�����ݲ��ܳ���2048���ַ���"), &m_Edit);
		return;
	}
	m_bMarkAdded = CMarkManager::Instance().SetMark(m_pMerch, MarkData);
	
	CDialogEx::OnOK();
}

void CDlgMarkText::PromptErrorInput( const CString &StrPrompt, CWnd *pWndFocus /*= NULL*/ )
{
	if ( !StrPrompt.IsEmpty() )
	{
		AfxMessageBox(StrPrompt, MB_ICONWARNING |MB_OK);
	}
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

void CDlgMarkText::OnBtnDel()
{
	// ɾ���ı����
	CMarkManager::Instance().RemoveMark(m_pMerch);
	m_bMarkAdded = false;
	EndDialog(IDOK);
}