// DlgWorksheet.cpp : implementation file
//

#include "stdafx.h"

#include "dlgworksheet.h"
#include "tinyxml.h"

#include "MPIChildFrame.h"
#include "ShareFun.h"
#include "pathfactory.h"
#include "WspManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define CFM_SYSTEMICON	(10) // 系统image用wsp
#define CFM_USERICON	(11) //
														
/////////////////////////////////////////////////////////////////////////////
// CDlgWorksheet dialog

 
CDlgOpenWorksheet::CDlgOpenWorksheet(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgOpenWorksheet::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgWorksheet)
	//}}AFX_DATA_INIT
}


void CDlgOpenWorksheet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgWorksheet)
	DDX_Control(pDX, IDC_LIST_SHOW, m_ListShow);
	//}}AFX_DATA_MAP
}
BEGIN_MESSAGE_MAP(CDlgOpenWorksheet, CDialogEx)
	//{{AFX_MSG_MAP(CDlgWorksheet)
	ON_LBN_DBLCLK(IDC_LIST_SHOW, OnDblclkListShow)
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT, OnButtonDefault)
	ON_LBN_SELCHANGE(IDC_LIST_SHOW, OnSelchangeListShow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWorksheet message handlers

void CDlgOpenWorksheet::OnDblclkListShow() 
{
	T_CfmFileInfo cfm;
	if ( GetSelectCfm(cfm) )
	{
		m_StrSelCfm = cfm.m_StrXmlName;
	}
	else
	{
		m_StrSelCfm.Empty();
	}

	// 先隐藏自己，避免加载后还显示 xl 0531
	//ShowWindow(SW_HIDE); // 置顶对话框不能hide，hide会导致主界面切换到后台
	MoveWindow(CRect(0,0,0,0));	// 缩小其面积，达到隐藏目的

	CDialog::OnOK();
}

void CDlgOpenWorksheet::OnClose() 
{
	CDialog::OnClose();
}

BOOL CDlgOpenWorksheet::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetWindowText(L"打开页面文件");

	CCfmManager::CfmInfoMap cfms;
	CCfmManager::Instance().QueryUserPrivateCfm(cfms);

	CCfmManager::CfmInfoMap::const_iterator it;
	for ( it = cfms.begin();it != cfms.end() ;it++ )
	{
		if ( !it->second.m_bSecret )
		{
			int iLine = m_ListShow.InsertString(-1, it->first);
			m_ListShow.SetItemColor(iLine, 0xff0000, RGB(200,200,200), it->second.m_bSystemDirHas ? CFM_SYSTEMICON : CFM_USERICON);
		}
	}	

	CCfmManager::Instance().QueryUserSystemCfm(cfms);
		
	for (it = cfms.begin();it != cfms.end() ;it++ )
	{
			if ( !it->second.m_bSecret )
			{
				int iLine = m_ListShow.InsertString(-1, it->first);
				m_ListShow.SetItemColor(iLine, 0xff0000, RGB(200,200,200), it->second.m_bSystemDirHas ? CFM_SYSTEMICON : CFM_USERICON);
			}
	}	

	// 设置一个默认选项
	m_ListShow.SetCurSel(0);
	OnSelchangeListShow();

	return TRUE;
}

void CDlgOpenWorksheet::OnOK() 
{		
	int32 iCur = m_ListShow.GetCurSel();
	
	if (iCur >= 0 && iCur < m_ListShow.GetCount() )
	{
		OnDblclkListShow();	// will close
	}	
	else
	{
		MessageBox(_T("请选择打开的页面!"), _T("请选择"), MB_OK);
	}
}

void CDlgOpenWorksheet::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
 	
 	CRect rect;
 	m_ListShow.GetClientRect(&rect);		
 	//dc.FillSolidRect(rect,RGB(255,0,0));

	// Do not call CDialog::OnPaint() for painting messages
}

void CDlgOpenWorksheet::OnButtonDefault() 
{
	T_CfmFileInfo cfm;
	if ( !GetSelectCfm(cfm) )
	{
		return;
	}

	DWORD dwRights = CCfmManager::Instance().GetCfmUserRights(cfm);
	ASSERT( (dwRights&CCfmManager::ECUR_Restore) == CCfmManager::ECUR_Restore );	// 要有得还原
	if ( (dwRights&CCfmManager::ECUR_Restore) != CCfmManager::ECUR_Restore )
	{
		// 没有还原
		return;
	}

	CString StrPrompt;
	StrPrompt.Format(_T("确认恢复页面: %s\r\n确认将丢弃您以前对该页面所做的修改！"), cfm.m_StrXmlName.GetBuffer());
	if ( MessageBox(StrPrompt, _T("提示"), MB_YESNO |MB_ICONQUESTION |MB_DEFBUTTON2) != IDYES )
	{
		return;
	}
	
	CCfmManager::Instance().DelUserCfm(cfm.m_StrXmlName);	// 通知删除就可以了, 再次加载时会加载系统的

	OnSelchangeListShow();	// 更新下按钮
}

void CDlgOpenWorksheet::OnSelchangeListShow() 
{
	CWnd * pWnd = GetDlgItem(IDC_BUTTON_DEFAULT);
	if ( NULL == pWnd )
	{
		return;
	}

	T_CfmFileInfo cfm;
	if ( !GetSelectCfm(cfm) )
	{
		//ASSERT( 0 );
		pWnd->EnableWindow(FALSE);
		return;
	}

	

	DWORD dwRights = CCfmManager::Instance().GetCfmUserRights(cfm);
	

	if ( (dwRights&CCfmManager::ECUR_Restore) == CCfmManager::ECUR_Restore )
	{
		pWnd->EnableWindow(TRUE);	// 系统目录中有
	}
	else
	{
		// 本身在系统目录，或者完全是用户自己的，采取点别的什么呢
		pWnd->EnableWindow(FALSE);
	}	
}

bool32 CDlgOpenWorksheet::GetSelectCfm( OUT T_CfmFileInfo &cfm )
{
	int32 iCurSel = m_ListShow.GetCurSel();
	
	if (iCurSel<0)
		return false;
	
	CString StrItem;
	m_ListShow.GetText(iCurSel, StrItem);

	return CCfmManager::Instance().QueryUserCfm(StrItem, cfm);
}
