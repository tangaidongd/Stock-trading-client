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
	//	CreateToolTips ( m_Tree.GetSafeHwnd(),"双击进行编辑.",NULL);

	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	pMainFrm->SetHotkeyTarget(this);			// 先设置窗口，在设置强制标志
	pMainFrm->SetForceFixHotkeyTarget(true);

	m_ImageList.Create ( IDB_FORMULA,16,1,RGB(255,255,255) );

	m_List.SetImageList( &m_ImageList,LVSIL_SMALL);
	m_List.SetExtendedStyle(m_List.GetExtendedStyle()/*|LVS_EX_FULLROWSELECT*/);  

	m_List.InsertColumn(0,_T("商品"),LVCFMT_LEFT,40);

	InitUserBlock();

	return FALSE;
}


void CDlgUserBlockAddMerch::InitUserBlock()
{
	int32 i = 0;		
	m_List.DeleteAllItems();

	// 得到这个板块
	CString StrBlockName = CUserBlockManager::Instance()->GetDefaultServerBlockName();
	T_Block* pBlock	= CUserBlockManager::Instance()->GetBlock(StrBlockName);
	if ( NULL == pBlock )
	{
		return;
	}

	// 得到板块下所有的商品
	CArray<CMerch*, CMerch*> aMerchs;
	if ( !CUserBlockManager::Instance()->GetMerchsInBlock(StrBlockName, aMerchs) )
	{
		return;
	}

	// 设置列表的颜色,自己定义为黑色，不用自选股的了
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
	// 新的处理方法,直接弹出键盘精灵就行了:
	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();

	if ( NULL != pMainFrame->m_pDlgKeyBoard )		
	{
		// 关掉
		pMainFrame->m_pDlgKeyBoard->PostMessage(WM_CLOSE,0,0);
	}

	ASSERT( NULL == pMainFrame->m_pDlgKeyBoard );

	pMainFrame->m_pDlgKeyBoard = new CKeyBoardDlg(pMainFrame->m_HotKeyList, pMainFrame);	
	ASSERT(NULL != pMainFrame->m_pDlgKeyBoard);
	pMainFrame->m_pDlgKeyBoard->Create(IDD_KEYBOARD,pMainFrame);
	//pMainFrame->m_pDlgKeyBoard->SetAutoCloseFlag(false);

	// 移动到本窗口的右下角: MoveWindow 的时候,是根据父窗口的位置移动的.
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
			// 没添加的原因有可能已存在的可能，我们将光标定位到已存在的商品
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

	//先删除ItemData指针
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

	//每次获取第一个Select,并删除之.知道没有被Select的存在.
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

	// 选中列表项中下一个item,如果后面没有项了,那么选中最后一个.

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

	// 列表中上移一个位置:	
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

	// XML 文档中上移一个位置:
	CUserBlockManager::Instance()->ChangeOwnMerchPosition(true, StrBlockName, pMerch);

	//
	m_List.SetItemState(iItem-1,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
	m_List.EnsureVisible(iItem-1, FALSE); 
}

void CDlgUserBlockAddMerch::OnBnClickedButtonDown()
{
	CString StrBlockName = CUserBlockManager::Instance()->GetDefaultServerBlockName();

	// 列表中上移一个位置:	
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

	// XML 文档中上移一个位置:
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
	strNum.Format(_T("自选股（%d个）"), iCount);
	GetDlgItem(IDC_STATIC_NUM)->SetWindowText(strNum);

}


void CDlgUserBlockAddMerch::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	pMainFrm->SetForceFixHotkeyTarget(false);		// 一定要先接触强制标志然后在设置NULL
	pMainFrm->SetHotkeyTarget(NULL);

	CDialogEx::OnClose();
}
