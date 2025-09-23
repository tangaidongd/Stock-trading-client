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
	m_ctrlWkSpaceList.InsertColumn(0,L"ϵͳ��Ļ�б�", LVCFMT_LEFT, 260, 0);
	
	m_aCfms.RemoveAll();

	CCfmManager::CfmInfoMap cfms;
	//˽�еİ����ļ� ����
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
	//ϵͳ�����ļ���������Ҫ��
	CCfmManager::CfmInfoMap::const_iterator it;
	CCfmManager::Instance().QueryUserSystemCfm(cfms);
	for (it = cfms.begin();
	it != cfms.end() ;
	it++ )
	{
		T_CfmFileInfo cfinfo = it->second;
		m_aCfms.Add( cfinfo );
	}

	//��ϵͳ����������ӵ���ʾ����
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


//�ָ�Ĭ��
void CDlgRestoreWkSpace::OnOK() 
{	
	UpdateData(TRUE);
	
	//����Ƿ�Ҫ��ʾ
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
	//��ʾ�û�
	if( bCheck )
	{
		CString StrPrompt;
		//StrPrompt = _T("ȷ�ϻָ�ϵͳҳ�棬ȷ�Ͻ���������ǰ�Ը�ҳ���������޸ģ�");
		StrPrompt = _T("ȷ�ϻָ�ϵͳҳ�棬ȷ�Ͻ���������ǰ�Ը�ҳ���������޸ģ�"); 
		
		if ( MessageBox(StrPrompt, _T("��ʾ"), MB_YESNO |MB_ICONQUESTION |MB_DEFBUTTON2) != IDYES )
		{
			return;
		}
		
		//�ָ���Ļ
		for( int i =0 ; i < nCount ; i++)
		{
			bCheck = m_ctrlWkSpaceList.GetCheck(i);
			if( bCheck )
			{
				//�ҳ���ǰ�򿪵�
				bool32 bIsCurrent = CCfmManager::Instance().IsCurrentCfm(m_aCfms[i].m_StrXmlName);
				if( bIsCurrent )
				{
					m_CurrentCfmInfo  = m_aCfms[i];
				}
				CCfmManager::Instance().DelUserCfm(m_aCfms[i].m_StrXmlName,true);	// ֪ͨɾ���Ϳ�����, �ٴμ���ʱ�����ϵͳ��
			}
		}
	}
	
	CDialogEx::OnOK();
}
