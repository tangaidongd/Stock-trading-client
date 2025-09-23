// DlgRestoreWkSpace.cpp : implementation file
//

#include "stdafx.h"

#include "DlgRestoreWkSpace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRestoreWkSpace dialog


CDlgRestoreWkSpace::CDlgRestoreWkSpace(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgRestoreWkSpace::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRestoreWkSpace)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_CurrentCfmInfo.m_StrXmlName = L"";
}


void CDlgRestoreWkSpace::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRestoreWkSpace)
	DDX_Control(pDX, IDC_LIST_WKSPACE, m_ctrlWkSpaceList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRestoreWkSpace, CDialogEx)
	//{{AFX_MSG_MAP(CDlgRestoreWkSpace)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRestoreWkSpace message handlers

BOOL CDlgRestoreWkSpace::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	
//	CLanguage::GetInistance().TranslateDialog(this->m_hWnd, MAKEINTRESOURCE(IDD_DIALOG_RESTOREWKSPACE));
	
	m_ctrlWkSpaceList.SetExtendedStyle(m_ctrlWkSpaceList.GetExtendedStyle() | LVS_EX_CHECKBOXES);
	m_ctrlWkSpaceList.InsertColumn(0,L"系统屏幕列表", LVCFMT_LEFT, 260, 0);
	
	m_aCfms.RemoveAll();

	CCfmManager::CfmInfoMap cfms;
	//私有的版面文件 忽略
/*	CCfmManager::Instance().QueryUserPrivateCfm(cfms);
	
	for (CCfmManager::CfmInfoMap::const_iterator it = cfms.begin();
	it != cfms.end() ;
	it++ )
	{
		T_CfmFileInfo cfinfo = it->second;
		if ( cfinfo.m_bShortCutKey )
		{
			T_CfmFileInfo cfinfo = it->second;
			m_aCfms.Add( cfinfo );
		}
	}	
*/	
	//系统版面文件，我们需要的
	CCfmManager::CfmInfoMap::const_iterator it;
	CCfmManager::Instance().QueryUserSystemCfm(cfms);
	for (it = cfms.begin();
	it != cfms.end() ;
	it++ )
	{
		T_CfmFileInfo cfinfo = it->second;
		m_aCfms.Add( cfinfo );
	}

	//把系统版面名称添加到显示界面
	int nCount = m_aCfms.GetSize();
	for( int n = 0; n < nCount ; n++ )
	{
		m_ctrlWkSpaceList.InsertItem(n,m_aCfms[n].m_StrXmlName);
		m_ctrlWkSpaceList.SetItemData(n,n);
	}

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//恢复默认
void CDlgRestoreWkSpace::OnOK() 
{	
	UpdateData(TRUE);
	
	//检查是否要提示
	BOOL bCheck = FALSE;
	int nCount = m_ctrlWkSpaceList.GetItemCount();
	for( int i =0 ; i < nCount ; i++)
	{
		bCheck = m_ctrlWkSpaceList.GetCheck(i);
		if( bCheck )
		{
			break;
		}
	}
	//提示用户
	if( bCheck )
	{
		CString StrPrompt;
		//StrPrompt = _T("确认恢复系统页面，确认将丢弃您以前对该页面所做的修改！");
		StrPrompt = _T("确认恢复系统页面，确认将丢弃您以前对该页面所做的修改！"); 
		
		if ( MessageBox(StrPrompt, _T("提示"), MB_YESNO |MB_ICONQUESTION |MB_DEFBUTTON2) != IDYES )
		{
			return;
		}
		
		//恢复屏幕
		for( int i =0 ; i < nCount ; i++)
		{
			bCheck = m_ctrlWkSpaceList.GetCheck(i);
			if( bCheck )
			{
				//找出当前打开的
				bool32 bIsCurrent = CCfmManager::Instance().IsCurrentCfm(m_aCfms[i].m_StrXmlName);
				if( bIsCurrent )
				{
					m_CurrentCfmInfo  = m_aCfms[i];
				}
				CCfmManager::Instance().DelUserCfm(m_aCfms[i].m_StrXmlName,true);	// 通知删除就可以了, 再次加载时会加载系统的
			}
		}
	}
	
	CDialogEx::OnOK();
}
