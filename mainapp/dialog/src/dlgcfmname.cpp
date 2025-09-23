// dlgwspname.cpp : implementation file
//

#include "stdafx.h"

#include "dlgcfmname.h"

#include "ShareFun.h"

#include "WspManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CFM_SYSTEMICON	(10) // 系统image用wsp
#define CFM_USERICON	(11) //

/////////////////////////////////////////////////////////////////////////////
// CDlgCfmName dialog

 
CDlgCfmName::CDlgCfmName(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgCfmName::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgCfmName)
	m_StrName = _T("");
	//}}AFX_DATA_INIT

	m_dwDoneAction = DoneNone;
}


void CDlgCfmName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCfmName)
	DDX_Control(pDX, IDC_EDIT_NAME, m_edit);
	DDX_Control(pDX, IDC_LIST_EXIST, m_ListExist);
	DDX_Text(pDX, IDC_EDIT_NAME, m_StrName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCfmName, CDialogEx)
	//{{AFX_MSG_MAP(CDlgCfmName)
	ON_LBN_DBLCLK(IDC_LIST_EXIST, OnDblclkListExist)
	ON_COMMAND(IDC_BUTTON_DEL, OnButtonDel)
	ON_LBN_SELCHANGE(IDC_LIST_EXIST, OnSelchangeListShow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCfmName message handlers
BOOL CDlgCfmName::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if ( NULL == pApp )
	{
		EndDialog(IDCANCEL);
		return FALSE;
	}

	SetWindowText(L"设置名称");
	
	LoadCfmList();	

	m_edit.SetBackgroundColor(false,RGB(200,200,200));
	
	CHARFORMAT cf;   	
	cf.dwEffects = 0 ;	
	cf.crTextColor = RGB(0,0,200);     
	m_edit.SetDefaultCharFormat(cf);  	
	m_edit.SetEventMask(ENM_CHANGE);

	m_ListExist.SetCurSel(-1);
	
	return TRUE;  	
}

void CDlgCfmName::OnOK() 
{
	UpdateData(true);
	m_StrName.TrimLeft();
	m_StrName.TrimRight();

	if ( m_StrName.IsEmpty() )
	{
		// 不允许任何形式的名称冲突
		CString Str;
		Str.Format(_T("请输入页面名称"));
		MessageBox(Str, _T("错误"), MB_OK |MB_ICONINFORMATION);
		
		m_edit.SetSel(0, -1);
		m_edit.SetFocus();
		return;
	}

	if ( CCfmManager::Instance().IsNewXmlNameConflict(m_StrName) != CCfmManager::EWCT_None )
	{
		// 不允许任何形式的名称冲突
		CString Str;
		Str.Format(_T("[%s]与系统关键字或者已存在的页面文件名字冲突!\r\n请重新命名"), m_StrName.GetBuffer());
		MessageBox(Str, _T("错误"), MB_OK |MB_ICONERROR);

		m_edit.SetSel(0, -1);
		m_edit.SetFocus();
		return;
	}
	
	{
		m_dwDoneAction |= DoneAdd;
		CDialog::OnOK();
	}	
}

void CDlgCfmName::OnCancel() 
{
	CDialog::OnCancel();
}

void CDlgCfmName::OnButtonDel()
{
	T_CfmFileInfo cfm;
	if ( !GetSelectCfm(cfm) )
	{
		return;
	}

	DWORD dwRights = CCfmManager::Instance().GetCfmUserRights(cfm);

	if ( (dwRights&CCfmManager::ECUR_Delete) == 0 && (dwRights&CCfmManager::ECUR_Restore) == 0 )
	{
		CString Str;
		Str.Format(_T("不能删除或者恢复工作页面: %s"), cfm.m_StrXmlName.GetBuffer());
		MessageBox(Str, _T("提示"), MB_OK |MB_ICONWARNING);
		return;
	}

	CString StrPrompt;
	if ( (dwRights&CCfmManager::ECUR_Delete) != 0 )
	{
		StrPrompt.Format(_T("确认删除页面: %s"), cfm.m_StrXmlName.GetBuffer());
	}
	else
	{
		StrPrompt.Format(_T("确认恢复页面: %s\r\n确认将丢弃您以前对该页面所做的修改！"), cfm.m_StrXmlName.GetBuffer());
	}
	if ( MessageBox(StrPrompt, _T("提示"), MB_YESNO |MB_ICONQUESTION |MB_DEFBUTTON2) != IDYES )
	{
		return; 
	}

	m_dwDoneAction |= DoneDelete;
	if ( (dwRights&CCfmManager::ECUR_Restore) != 0 )
	{
		CCfmManager::Instance().DelUserCfm(cfm.m_StrXmlName);
		LoadCfmList();
	}
	else
	{
		CCfmManager::Instance().DelUserCfm(cfm.m_StrXmlName);
		LoadCfmList();
	}
}


void CDlgCfmName::OnDblclkListExist() 
{ 
	// 覆盖是不允许的，so不响应
}



bool32 CDlgCfmName::GetSelectCfm( OUT T_CfmFileInfo &cfm )
{
	int32 iCurSel = m_ListExist.GetCurSel();
	
	if (iCurSel<0)
		return false;
	
	CString StrItem;
	m_ListExist.GetText(iCurSel, StrItem);
	
	return CCfmManager::Instance().QueryUserCfm(StrItem, cfm);
}

void CDlgCfmName::LoadCfmList()
{
	m_ListExist.ResetContent();

	CCfmManager::CfmInfoMap cfms;
	CCfmManager::Instance().QueryUserPrivateCfm(cfms);
	
	CCfmManager::CfmInfoMap::const_iterator it;
	for (it = cfms.begin();it != cfms.end() ;it++ )
	{
		if ( !it->second.m_bSecret )
		{
			int iLine = m_ListExist.InsertString(-1, it->first);
			m_ListExist.SetItemColor(iLine, 0xff0000, RGB(200,200,200), it->second.m_bSystemDirHas ? CFM_SYSTEMICON : CFM_USERICON);
		}
	}	
	
	CCfmManager::Instance().QueryUserSystemCfm(cfms);
	
	for (it = cfms.begin();it != cfms.end() ;it++ )
	{
		if ( !it->second.m_bSecret )
		{
			int iLine = m_ListExist.InsertString(-1, it->first);
			m_ListExist.SetItemColor(iLine, 0xff0000, RGB(200,200,200), it->second.m_bSystemDirHas ? CFM_SYSTEMICON : CFM_USERICON);
		}
	}
	
	OnSelchangeListShow();
}

void CDlgCfmName::OnSelchangeListShow()
{
	CWnd *pWnd = GetDlgItem(IDC_BUTTON_DEL);
	if ( NULL == pWnd )
	{
		return;
	}

	T_CfmFileInfo cfm;
	DWORD dwRights = 0;
	if ( !GetSelectCfm(cfm) )
	{
		pWnd->SetWindowText(_T("删除"));
		pWnd->EnableWindow(FALSE);
		return;
	}

	dwRights = CCfmManager::Instance().GetCfmUserRights(cfm);

	if ( (dwRights&CCfmManager::ECUR_Restore) != 0 )
	{
		pWnd->EnableWindow(TRUE);
		pWnd->SetWindowText(_T("恢复"));
	}
	else if ( (dwRights&CCfmManager::ECUR_Delete) != 0 )
	{
		pWnd->EnableWindow(TRUE);
		pWnd->SetWindowText(_T("删除"));
	}
	else
	{
		pWnd->SetWindowText(_T("删除"));
		pWnd->EnableWindow(FALSE);
	}
}
