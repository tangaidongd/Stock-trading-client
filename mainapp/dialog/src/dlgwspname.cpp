// dlgwspname.cpp : implementation file
//

#include "stdafx.h"

#include "dlgwspname.h"

#include "ShareFun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgWspName dialog

 
CDlgWspName::CDlgWspName(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgWspName::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgWspName)
	m_StrName = _T("");
	//}}AFX_DATA_INIT
	m_pView = NULL;

	m_dwDoneAction = DoneNone;
	m_eFileType = CDlgWorkSpaceSet::EFTCount;
}

void CDlgWspName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgWspName)
	DDX_Control(pDX, IDC_EDIT_NAME, m_edit);
	DDX_Control(pDX, IDC_LIST_EXIST, m_ListExist);
	DDX_Text(pDX, IDC_EDIT_NAME, m_StrName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgWspName, CDialogEx)
	//{{AFX_MSG_MAP(CDlgWspName)
	ON_LBN_DBLCLK(IDC_LIST_EXIST, OnDblclkListExist)
	ON_COMMAND(IDC_BUTTON_DEL, OnButtonDel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWspName message handlers
BOOL CDlgWspName::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if ( NULL == pApp )
	{
		return FALSE;
	}

	CStringArray aFileXmlName;
	pApp->GetWspFileXmlNameArray(aFileXmlName);
	pApp->GetWspFilePathArray(m_aFileExist);

	CArray<T_WspFileInfo, T_WspFileInfo &> aWspInfos;
	aWspInfos.Copy(pApp->m_aWspFileInfo);
	ASSERT( aFileXmlName.GetSize() == aWspInfos.GetSize() );	// 必定相等且序相同
	
	if ( aFileXmlName.GetSize() > 0 )
	{
		int32 iFileType = (int32)m_eFileType;

		for (int32 i=m_aFileExist.GetSize() - 1; i >= 0 ; i--)
		{
			if ( aWspInfos[i].IsSpecialWsp() )
			{
				m_aWspSpecial.Add(aWspInfos[i]);

				m_aFileExist.RemoveAt(i);
				aFileXmlName.RemoveAt(i);

				continue;		// 特殊工作区不能出现在这里
			}
				
			int iRowNo = m_ListExist.InsertString(0, aFileXmlName[i]);
			if ( iRowNo != LB_ERR )
			{
				m_ListExist.SetItemColor(iRowNo, 0xff0000, RGB(200,200,200), iFileType);
			}
		}
	}

	/*m_edit.SetBackgroundColor(false,RGB(200,200,200));
	
	CHARFORMAT cf;   	
	cf.dwEffects = 0 ;	
	cf.crTextColor = RGB(0,0,200);     
	m_edit.SetDefaultCharFormat(cf);  	
	m_edit.SetEventMask(ENM_CHANGE);*/
	
	if (0 == m_ListExist.GetCount())
	{
		m_ListExist.PostMessage(WM_LBUTTONDOWN,0,0);
	}
	m_edit.SetFocus();
	return TRUE;  	
}

void CDlgWspName::OnOK() 
{
	UpdateData(true);
	m_StrName.TrimLeft();
	m_StrName.TrimRight();
	
	if ( 0 == m_StrName.GetLength() )
	{
		MessageBox(L"名称不能为空!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
	}
	else if ( m_StrName.GetLength() > 20 )
	{
		MessageBox(L"名称过长!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
	}
	else if ((m_StrName.Find(L".wsp")!=-1) 
		|| (m_StrName.Find(L".cfm")!=-1) 
		|| (m_StrName.Find(L".vmg")!=-1) 
		|| (m_StrName.Find(L"默认")!=-1)
		|| (m_StrName.Find(_T("system")) != -1)
		|| (IsSpecialWspName(m_StrName))
		)
	{
		MessageBox(L"名称中包含系统关键字,请重新命名!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
	}
	else if (FindExistFile(m_StrName))
	{
		m_dwDoneAction |= DoneAdd;
		if ( IDYES == MessageBox(L"此文件已存在,是否覆盖?", AfxGetApp()->m_pszAppName, MB_YESNO))
		{	
			ReMoveFile();
			CDialog::OnOK();			
 		}
	}
	else
	{
		m_dwDoneAction |= DoneAdd;
		CDialog::OnOK();
	}	
}

void CDlgWspName::OnCancel() 
{
	CDialog::OnCancel();
}

void CDlgWspName::OnButtonDel()
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	
	// 删除工作区文件:
	int32 iCurSel = m_ListExist.GetCurSel();

	if ( iCurSel < 0 || iCurSel >= m_ListExist.GetCount() )
	{
		MessageBox(L"请选中需要删除的工作区文件", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return;
	}

	//
	CString StrFilePath = m_aFileExist.GetAt(iCurSel);
	
	CString StrFileName;
	m_ListExist.GetText(iCurSel, StrFileName);
	
	CString StrCurUse;
	if ( NULL != pApp )
	{
		StrCurUse = pApp->m_StrCurWsp;
	}

	if ( 0 == StrFilePath.CompareNoCase(StrCurUse) )
	{
		MessageBox(L"不能删除当前正在使用的工作区文件", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return;
	}

	CString StrPrompt;
	StrPrompt.Format(L"确认删除文件: %s ?", StrFileName.GetBuffer());
	
	//
	if ( IDYES == MessageBox(StrPrompt, AfxGetApp()->m_pszAppName, MB_YESNO) )
	{
		m_dwDoneAction |= DoneDelete;
		m_ListExist.DeleteString(iCurSel);
		m_aFileExist.RemoveAt(iCurSel);

		CFile::Remove(StrFilePath);
	}
}

void CDlgWspName::GetCurrentView(CGGTongView * pView)
{
	ASSERT(NULL!=pView);
	m_pView = pView;
}

bool32 CDlgWspName::FindExistFile(CString StrFileName)
{	
/*	CString StrDir = CMainFrame::GetWorkSpaceDir();
	
	if(StrDir.Right(1) != "/")
	{
		StrDir += "/";
	}
	
	StrDir += StrFileName; 
	
	if (CDlgWorkSpaceSet::EFTWorkSpace == m_eFileType)
	{
		StrDir += L".wsp";
	}
	else if (CDlgWorkSpaceSet::EFTChildFrame == m_eFileType)
	{
		StrDir += L".cfm";
	}
	else if (CDlgWorkSpaceSet::EFTIoViewManger == m_eFileType)
	{
		StrDir += L".vmg";
	}

	m_StrfileDir = StrDir;
	CFileFind file;
	bool32 bFind = file.FindFile(StrDir);
*/
	for ( int32 i = 0 ; i < m_ListExist.GetCount(); i++ )
	{
		CString StrText;
		m_ListExist.GetText(i, StrText);

		if ( StrFileName.CompareNoCase(StrText) == 0 )	// 不区分大小写
		{
			return true;
		}
	}

	return false;	
}

void CDlgWspName::SetNewFileType(CDlgWorkSpaceSet::E_FileType eFileType)
{	
	ASSERT(eFileType < CDlgWorkSpaceSet::EFTCount);
	m_eFileType = eFileType;
} 

void CDlgWspName::OnDblclkListExist() 
{ 
	// 对于覆盖，因为存在文件名和实际名称不一定同一，所以删除了这个，登录时又会重新生成
	if ( IDYES == MessageBox(L"是否覆盖本文件?", AfxGetApp()->m_pszAppName, MB_YESNO) )
	{
		int32 iCurSel = m_ListExist.GetCurSel();
		m_ListExist.GetText(iCurSel, m_StrName);
		
		UpdateData(false);
		ReMoveFile();
		CDialog::OnOK();
	}
	else
	{
		return;
	}
}


void CDlgWspName::ReMoveFile()
{		
	int32 iCurSel = m_ListExist.GetCurSel();
	
	if ( iCurSel < 0 || iCurSel >= m_ListExist.GetCount() || m_ListExist.GetCount() != m_aFileExist.GetSize())
	{
		return;
	}
	
	CString StrPath = m_aFileExist[iCurSel];
	CFileFind file;
	
	m_StrSelectedWspPath = StrPath;	// 记录该文件名， 重新生成的文件会以这个作为文件名
	if (file.FindFile(StrPath))
	{
		CFile::Remove(StrPath);
	}	
}

bool32 CDlgWspName::IsSpecialWspName( CString StrFileName )
{
	for ( int i=0; i < m_aWspSpecial.GetSize() ; i++ )
	{
		if ( m_aWspSpecial[i].m_StrFileName.Find(StrFileName + _T(".")) == 0
			|| m_aWspSpecial[i].m_StrFileXmlName == StrFileName ) // 简单判断前半部分相同，视为相同
		{
			return true;
		}
	}
	return false;
}

/*
void CDlgWspName::ReWriteFile()
{		
	int32 iCurSel = m_ListExist.GetCurSel();
	
	if ( iCurSel < 0 || iCurSel >= m_ListExist.GetCount() || m_ListExist.GetCount() != m_aFileExist.GetSize())
	{
		return;
	}

	CString StrPath = m_aFileExist[iCurSel];
	CFileFind file;
	
	if (file.FindFile(StrPath))
	{
		CFile::Remove(StrPath);
	}	

	SaveFile(StrPath);	
}

void CDlgWspName::SaveFile(const CString& StrFilePath)
{
	if ( StrFilePath.GetLength() <= 0 )
	{
		return;
	}

	CMainFrame * pMainFrame =(CMainFrame *)AfxGetApp()->m_pMainWnd;
	CMPIChildFrame * pCurrentChild =(CMPIChildFrame * )pMainFrame->GetActiveFrame();
	ASSERT(pCurrentChild);
	
	if ( CDlgWorkSpaceSet::EFTWorkSpace == m_eFileType)
	{
		CGGTongApp * pApp = (CGGTongApp *)AfxGetApp();
		pApp->m_StrCurWsp = StrFilePath;
	
		char acFileName[MAX_PATH];
		UNICODE_2_MULTICHAR(EMCCSystem, StrFilePath, acFileName);

		pMainFrame->ToXml(acFileName);			
	}
}
*/
