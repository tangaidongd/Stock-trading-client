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


#define CFM_SYSTEMICON	(10) // ϵͳimage��wsp
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

	// �������Լ���������غ���ʾ xl 0531
	//ShowWindow(SW_HIDE); // �ö��Ի�����hide��hide�ᵼ���������л�����̨
	MoveWindow(CRect(0,0,0,0));	// ��С��������ﵽ����Ŀ��

	CDialog::OnOK();
}

void CDlgOpenWorksheet::OnClose() 
{
	CDialog::OnClose();
}

BOOL CDlgOpenWorksheet::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetWindowText(L"��ҳ���ļ�");

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

	// ����һ��Ĭ��ѡ��
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
		MessageBox(_T("��ѡ��򿪵�ҳ��!"), _T("��ѡ��"), MB_OK);
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
	ASSERT( (dwRights&CCfmManager::ECUR_Restore) == CCfmManager::ECUR_Restore );	// Ҫ�еû�ԭ
	if ( (dwRights&CCfmManager::ECUR_Restore) != CCfmManager::ECUR_Restore )
	{
		// û�л�ԭ
		return;
	}

	CString StrPrompt;
	StrPrompt.Format(_T("ȷ�ϻָ�ҳ��: %s\r\nȷ�Ͻ���������ǰ�Ը�ҳ���������޸ģ�"), cfm.m_StrXmlName.GetBuffer());
	if ( MessageBox(StrPrompt, _T("��ʾ"), MB_YESNO |MB_ICONQUESTION |MB_DEFBUTTON2) != IDYES )
	{
		return;
	}
	
	CCfmManager::Instance().DelUserCfm(cfm.m_StrXmlName);	// ֪ͨɾ���Ϳ�����, �ٴμ���ʱ�����ϵͳ��

	OnSelchangeListShow();	// �����°�ť
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
		pWnd->EnableWindow(TRUE);	// ϵͳĿ¼����
	}
	else
	{
		// ������ϵͳĿ¼��������ȫ���û��Լ��ģ���ȡ����ʲô��
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
