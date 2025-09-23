// DlgUserBlockAddMerch.cpp : implementation file
//

#include "stdafx.h"
#include "GGTong.h"
#include "DlgUserBlockAddMerch.h"


#define  LIST_DATA_COLORR RGB(0,0,0)
// CDlgUserBlockAddMerch dialog

//IMPLEMENT_DYNAMIC(CDlgUserBlockAddMerch, CDialog)

CDlgUserBlockAddMerch::CDlgUserBlockAddMerch(CWnd* pParent /*=NULL*/)
	:  CDialogEx(CDlgUserBlockAddMerch::IDD, pParent)
{

}

CDlgUserBlockAddMerch::~CDlgUserBlockAddMerch()
{
}

void CDlgUserBlockAddMerch::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MERCH, m_List);
}


BEGIN_MESSAGE_MAP(CDlgUserBlockAddMerch, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ADD_MERCH, OnBnClickedButtonAddMerch)
	ON_MESSAGE(UM_HOTKEY, OnMsgHotKey)
	ON_BN_CLICKED(IDC_BUTTON_DEL_MERCH, OnBnClickedButtonDelMerch)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnBnClickedButtonDown)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_MERCH, OnLvnItemchangedListMerch)
	ON_WM_MOVE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDlgUserBlockAddMerch message handlers

int CDlgUserBlockAddMerch::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}



BOOL CDlgUserBlockAddMerch::OnInitDialog()
{
	CDialog::OnInitDialog();

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,FALSE);
	//	CreateToolTips ( m_Tree.GetSafeHwnd(),"˫�����б༭.",NULL);

	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	pMainFrm->SetHotkeyTarget(this);			// �����ô��ڣ�������ǿ�Ʊ�־
	pMainFrm->SetForceFixHotkeyTarget(true);

	m_ImageList.Create ( IDB_FORMULA,16,1,RGB(255,255,255) );

	m_List.SetImageList( &m_ImageList,LVSIL_SMALL);
	m_List.SetExtendedStyle(m_List.GetExtendedStyle()/*|LVS_EX_FULLROWSELECT*/);  

	m_List.InsertColumn(0,_T("��Ʒ"),LVCFMT_LEFT,40);

	InitUserBlock();

	return FALSE;
}


void CDlgUserBlockAddMerch::InitUserBlock()
{
	int32 i = 0;		
	m_List.DeleteAllItems();

	// �õ�������
	CString StrBlockName = CUserBlockManager::Instance()->GetDefaultServerBlockName();
	T_Block* pBlock	= CUserBlockManager::Instance()->GetBlock(StrBlockName);
	if ( NULL == pBlock )
	{
		return;
	}

	// �õ���������е���Ʒ
	CArray<CMerch*, CMerch*> aMerchs;
	if ( !CUserBlockManager::Instance()->GetMerchsInBlock(StrBlockName, aMerchs) )
	{
		return;
	}

	// �����б����ɫ,�Լ�����Ϊ��ɫ��������ѡ�ɵ���
	m_List.SetTextColor(LIST_DATA_COLORR);
	for ( i = 0; i < aMerchs.GetSize(); i++ )
	{
		CMerch* pMerch = aMerchs[i];

		if ( NULL != pMerch )
		{
			int32 iListCnt = m_List.GetItemCount();
			m_List.InsertItem(iListCnt,pMerch->m_MerchInfo.m_StrMerchCnName,KMerchImageId);				
			m_List.SetItemData (iListCnt, (DWORD)pMerch);
		}
	}
}


void CDlgUserBlockAddMerch::OnBnClickedButtonAddMerch()
{
	// �µĴ�����,ֱ�ӵ������̾��������:
	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();

	if ( NULL != pMainFrame->m_pDlgKeyBoard )		
	{
		// �ص�
		pMainFrame->m_pDlgKeyBoard->PostMessage(WM_CLOSE,0,0);
	}

	ASSERT( NULL == pMainFrame->m_pDlgKeyBoard );

	pMainFrame->m_pDlgKeyBoard = new CKeyBoardDlg(pMainFrame->m_HotKeyList, pMainFrame);	
	ASSERT(NULL != pMainFrame->m_pDlgKeyBoard);
	pMainFrame->m_pDlgKeyBoard->Create(IDD_KEYBOARD,pMainFrame);
	//pMainFrame->m_pDlgKeyBoard->SetAutoCloseFlag(false);

	// �ƶ��������ڵ����½�: MoveWindow ��ʱ��,�Ǹ��ݸ����ڵ�λ���ƶ���.
	CRect rectWnd;
	GetClientRect(&rectWnd);
	ClientToScreen(&rectWnd);


	CRect rectKey;
	pMainFrame->m_pDlgKeyBoard->GetWindowRect(&rectKey);
	int32 iHeight = rectKey.Height();
	int32 iWidth  = rectKey.Width();

	pMainFrame->m_pDlgKeyBoard->SetWindowPos(&wndTopMost,rectWnd.right - iWidth - 1 ,rectWnd.bottom - iHeight +  1,iWidth,iHeight,SWP_NOREDRAW|SWP_NOSIZE);
	pMainFrame->m_pDlgKeyBoard->ShowWindow(SW_SHOW);
	pMainFrame->SetHotkeyTarget(this);
}



LRESULT CDlgUserBlockAddMerch::OnMsgHotKey(WPARAM wParam, LPARAM lParam)
{
	// fan
	CHotKey *pHotKey = (CHotKey *)wParam;
	ASSERT(NULL != pHotKey);

	if ( EHKTMerch == pHotKey->m_eHotKeyType )
	{
		CString StrHotKey ,StrName, StrMerchCode;
		int32   iMarketId;

		StrHotKey		= pHotKey->m_StrKey;
		StrMerchCode	= pHotKey->m_StrParam1;
		StrName			= pHotKey->m_StrSummary;
		iMarketId		= pHotKey->m_iParam1;		

		//
		CMerch* pMerch   = NULL;
		CGGTongDoc *pDoc = AfxGetDocument();
		pDoc->m_pAbsCenterManager->GetMerchManager().FindMerch(StrMerchCode, iMarketId, pMerch);

		CString StrBlockName  = CUserBlockManager::Instance()->GetDefaultServerBlockName();
		if ( CUserBlockManager::Instance()->AddMerchToUserBlock(pMerch, StrBlockName) )
		{			
			int32 iCount = m_List.GetItemCount();
			//
			m_List.InsertItem(iCount, StrName, KMerchImageId);
			m_List.SetItemData (iCount, (DWORD)pMerch);		
			m_List.EnsureVisible(iCount, FALSE);
			m_List.SetItemState(iCount,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
		}
		else
		{
			// û��ӵ�ԭ���п����Ѵ��ڵĿ��ܣ����ǽ���궨λ���Ѵ��ڵ���Ʒ
			for ( int32 i = 0; i < m_List.GetItemCount(); i++ )
			{
				CMerch* pMerch = (CMerch*)m_List.GetItemData(i);

				if ( NULL == pMerch )
				{
					continue;
				}
				if (pMerch->m_MerchInfo.m_StrMerchCnName.CompareNoCase(StrName) == 0)
				{
					m_List.EnsureVisible(i, FALSE); 
					m_List.SetItemState(i,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
					break;
				}		
			}
		}
	}

	DEL(pHotKey);

	return 0;	
}
void CDlgUserBlockAddMerch::OnBnClickedButtonDelMerch()
{
	
	CString StrBlockName = CUserBlockManager::Instance()->GetDefaultServerBlockName();

	//��ɾ��ItemDataָ��
	int32 iCurSel = 0;

	POSITION pos = m_List.GetFirstSelectedItemPosition();
	if ( NULL == pos )
	{
		return;
	}
	else
	{
		while (pos)
		{
			int32 iItem = m_List.GetNextSelectedItem(pos);
			if ( iItem >= 0 )
			{
				iCurSel = iItem;

				CMerch* pMerch = (CMerch *)m_List.GetItemData(iItem);				
				ASSERT(NULL != pMerch);

				CUserBlockManager::Instance()->DelMerchFromUserBlock(pMerch, StrBlockName);
			}
		}
	}

	//ÿ�λ�ȡ��һ��Select,��ɾ��֮.֪��û�б�Select�Ĵ���.
	pos = m_List.GetFirstSelectedItemPosition();
	if ( NULL == pos )
	{
		return;
	}
	else
	{
		while (pos)
		{
			int32 iItem = m_List.GetNextSelectedItem(pos);
			if ( iItem >= 0 )
			{
				m_List.DeleteItem(iItem);
			}
			pos = m_List.GetFirstSelectedItemPosition();
		}
	}

	// ѡ���б�������һ��item,�������û������,��ôѡ�����һ��.

	int32 iCount = m_List.GetItemCount();

	if ( iCurSel > iCount-1)
	{
		iCurSel = iCount - 1;
	}

	m_List.SetItemState(iCurSel,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
	m_List.EnsureVisible(iCurSel, FALSE); 
}

void CDlgUserBlockAddMerch::OnBnClickedButtonUp()
{
	CString StrBlockName = CUserBlockManager::Instance()->GetDefaultServerBlockName();

	// �б�������һ��λ��:	
	POSITION pos = m_List.GetFirstSelectedItemPosition();

	if (NULL == pos)
	{
		return;
	}

	int32 iItem  = m_List.GetNextSelectedItem(pos);

	if ( -1 == iItem || 0 == iItem)
	{
		m_List.SetItemState(0,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
		return;
	}

	CString StrText = m_List.GetItemText(iItem,0);
	CMerch* pMerch  = (CMerch*)m_List.GetItemData(iItem);	

	m_List.DeleteItem(iItem);

	m_List.InsertItem(iItem-1, StrText, KMerchImageId);
	m_List.SetItemData(iItem-1, (DWORD)pMerch);

	// XML �ĵ�������һ��λ��:
	CUserBlockManager::Instance()->ChangeOwnMerchPosition(true, StrBlockName, pMerch);

	//
	m_List.SetItemState(iItem-1,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
	m_List.EnsureVisible(iItem-1, FALSE); 
}

void CDlgUserBlockAddMerch::OnBnClickedButtonDown()
{
	CString StrBlockName = CUserBlockManager::Instance()->GetDefaultServerBlockName();

	// �б�������һ��λ��:	
	POSITION pos = m_List.GetFirstSelectedItemPosition();

	if ( NULL == pos)
	{
		return;
	}

	int32 iItemCount = m_List.GetItemCount();
	int32 iItem  = m_List.GetNextSelectedItem(pos);

	if ( -1 == iItem || iItemCount - 1 == iItem )
	{
		m_List.SetItemState(iItem,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
		return;
	}

	CString StrText  = m_List.GetItemText(iItem,0);
	CMerch* pMerch = (CMerch*)m_List.GetItemData(iItem);	

	m_List.DeleteItem(iItem);
	m_List.InsertItem(iItem + 1, StrText, KMerchImageId);
	m_List.SetItemData(iItem + 1, (DWORD)pMerch);

	// XML �ĵ�������һ��λ��:
	CUserBlockManager::Instance()->ChangeOwnMerchPosition(false, StrBlockName, pMerch);

	//
	m_List.SetItemState(iItem + 1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	m_List.EnsureVisible(iItem+ 1, FALSE); 
}

void CDlgUserBlockAddMerch::OnLvnItemchangedListMerch(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	int iCount = m_List.GetItemCount();
	CString strNum;
	strNum.Format(_T("��ѡ�ɣ�%d����"), iCount);
	GetDlgItem(IDC_STATIC_NUM)->SetWindowText(strNum);

}


void CDlgUserBlockAddMerch::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	pMainFrm->SetForceFixHotkeyTarget(false);		// һ��Ҫ�ȽӴ�ǿ�Ʊ�־Ȼ��������NULL
	pMainFrm->SetHotkeyTarget(NULL);

	CDialogEx::OnClose();
}
