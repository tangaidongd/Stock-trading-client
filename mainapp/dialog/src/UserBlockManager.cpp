// UserBlockManager.cpp : implementation file
//

#include "stdafx.h"
#include "tinyxml.h"
#include "UserBlockManager.h"
#include "keyboarddlg.h"
#include "IoViewChart.h"
#include "ShareFun.h"
#include "PathFactory.h"
#include "hotkey.h"
#include "DlgBlockSet.h"
#include "dlgmerchsselect.h"
#include "keyboarddlg.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include "BlockManager.h"
#include "coding.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define INVALID_ID			       -1

void CheckAndSaveXml ( TiXmlDocument* pDoc)
{
	chmod(pDoc->Value(),_S_IWRITE);
	pDoc->SaveFile();
}

const char* GetUserBlockFileName()
{
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	CString StrPath = CPathFactory::GetUserBlocksFileFullName(pDoc->m_pAbsCenterManager->GetUserName());
	
	string sPath = _Unicode2MultiChar(StrPath);
	const char* strFile = sPath.c_str();

	if ( 0 != _access ( strFile, 0 ))
	{
		//CString StrDefaultXml = _T("<?xml version='1.0' encoding='utf-8' ?><XMLDATA version='1.0' app='ggtong' data='workspace'><block name='自选股1' hotkey='zxg1'></XMLDATA>");
		CString StrDefaultXml = _T("<?xml version='1.0' encoding='utf-8' ?>\n<XMLDATA version='1.0' app='ggtong' data='workspace'>\n<block name='我的自选' hotkey='wdzx' color='16776960' head='12' server='1'>\n</block>\n</XMLDATA>");	// 与手机名称相同, 默认的第一个板块是server同步的
		TiXmlDocument* pXmlDoc = new TiXmlDocument(strFile);
		const char* strDefaultXml = _Unicode2MultiChar(StrDefaultXml).c_str();
		pXmlDoc->Parse(strDefaultXml);
		pXmlDoc->SaveFile();
		delete pXmlDoc;
	}

	return strFile;
}

const char* KStrElementRootName			= "blocks";
const char* KStrElementMerchName		= "merch";
const char* KStrElementMerchAttriMarket = "market";
const char* KStrElementMerchAttriCode   = "code";

const char* KStrElementBlockName		= "block";
const char* KStrElementBlockAttriName   = "name";
const char* KStrElementBlockAttriHotKey = "hotkey";
const char* KStrElementBlockAttriServer = "server";

const char* KStrElementBlockAttriColor  = "color";
const char* KStrElementBlockAttriHead   = "head";


//Breed
const int32 KBreedImageId = 2;
const int32 KBreedImageSelectId = 2;

//Market
const int32 KMarketImageId = 3;
const int32 KMarketImageSelectId = 3;

//UserBlock
const int32 KUserBlockImageId = 2;
const int32 KUserBlockImageSelectId = 2;

//Merch
const int32 KMerchImageId = 4;
const int32 KMerchImageSelectId = 4;

//Group1
const int32 KFormulaGroup1ImageId = 2;
const int32 KFormulaGroup1ImageSelectId = 2;

//Group2
const int32 KFormulaGroup2ImageId = 3;
const int32 KFormulaGroup2ImageSelectId = 3;

//Formula
const int32 KFormulaImageId = 4;
const int32 KFormulaImageSelectId = 4;

//Formula Protected
const int32 KFormulaProtectedImageId = 4;
const int32 KFormulaProtectedImageSelectId = 4;

/////////////////////////////////////////////////////////////////////////////
// CDlgUserBlockManager dialog

CDlgUserBlockManager::CDlgUserBlockManager(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgUserBlockManager::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgUserBlockManager)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgUserBlockManager::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgUserBlockManager)
	DDX_Control(pDX, IDC_LIST_MERCH, m_List);
	DDX_Control(pDX, IDC_TREE_BLOCK, m_Tree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgUserBlockManager, CDialogEx)
	//{{AFX_MSG_MAP(CDlgUserBlockManager)
	ON_BN_CLICKED(IDC_BUTTON_ADD_BLOCK, OnButtonAddBlock)
	ON_BN_CLICKED(IDC_BUTTON_DEL_BLOCK, OnButtonDelBlock)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_BLOCK, OnButtonEditBlock)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_BLOCK, OnSelchangedTreeBlock)
	ON_MESSAGE(UM_HOTKEY, OnMsgHotKey)
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_ADD_MERCH, OnButtonAddMerch)
	ON_BN_CLICKED(IDC_BUTTON_UP,OnButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN,OnButtonDown)
	ON_WM_NCDESTROY()
	ON_BN_CLICKED(IDC_BUTTON_DEL_MERCH, OnButtonDelMerch)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_MERCH, OnDblclkList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgUserBlockManager message handlers
int CDlgUserBlockManager::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

void CDlgUserBlockManager::OnClose() 
{
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	pMainFrm->SetForceFixHotkeyTarget(false);		// 一定要先接触强制标志然后在设置NULL
	pMainFrm->SetHotkeyTarget(NULL);
	
	CDialog::OnClose();
}

void CDlgUserBlockManager::InitialShow()
{	
	CArray<T_Block, T_Block&> aBlocks;
	CUserBlockManager::Instance()->GetBlocks(aBlocks);

	for ( int32 i = 0; i < aBlocks.GetSize(); i++ )
	{
		T_Block stBlock = aBlocks.GetAt(i);
		m_Tree.InsertItem (stBlock.m_StrName, KUserBlockImageId,KUserBlockImageSelectId);
	}

	HTREEITEM hItem = m_Tree.GetFirstVisibleItem();
	if ( NULL != hItem )
	{
		m_Tree.SelectItem(hItem);
	}
}

BOOL CDlgUserBlockManager::OnInitDialog() 
{
	CDialog::OnInitDialog();
	HICON hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,FALSE);
	//	CreateToolTips ( m_Tree.GetSafeHwnd(),"双击进行编辑.",NULL);

	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	pMainFrm->SetHotkeyTarget(this);			// 先设置窗口，在设置强制标志
	pMainFrm->SetForceFixHotkeyTarget(true);

	m_ImageList.Create ( IDB_FORMULA,16,1,RGB(255,255,255) );
	
	m_Tree.SetImageList( &m_ImageList,TVSIL_NORMAL);
	m_List.SetImageList( &m_ImageList,LVSIL_SMALL);
	m_List.SetExtendedStyle(m_List.GetExtendedStyle()/*|LVS_EX_FULLROWSELECT*/);  

	m_List.InsertColumn(0,_T("商品"),LVCFMT_LEFT,40);
	
	InitialShow();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgUserBlockManager::OnButtonAddBlock() 
{
	// fan
	m_BindHotkey.Release();

	HTREEITEM hSelect = NULL;

	CDlgBlockSet DlgBlockSet;
	DlgBlockSet.SetModifyFlag(false);
	
	if ( IDOK == DlgBlockSet.DoModal() )
	{
		m_Tree.InsertItem (DlgBlockSet.m_StrName,KUserBlockImageId,KUserBlockImageSelectId,hSelect);
		if ( NULL != hSelect )
		{
			m_Tree.Expand (hSelect, TVE_EXPAND);
		}
		
		int32 iCount = m_Tree.GetCount();
		
		if (iCount <= 1)
		{
			GetDlgItem(IDC_BUTTON_DEL_BLOCK)->EnableWindow(FALSE);			
		}
		else
		{
			GetDlgItem(IDC_BUTTON_DEL_BLOCK)->EnableWindow(TRUE);			
		}
		
		T_Block BlockToAdd;
		BlockToAdd.m_StrName	= DlgBlockSet.m_StrName;
		BlockToAdd.m_StrHotKey	= DlgBlockSet.m_StrHotKey;
		BlockToAdd.m_clrBlock	= DlgBlockSet.GetColor();
		BlockToAdd.m_eHeadType	= DlgBlockSet.GetHead();

		CUserBlockManager::Instance()->AddUserBlock(BlockToAdd);
	}

	m_BindHotkey.AddRef();
}

void CDlgUserBlockManager::OnButtonDelBlock()
{
	// fan
	HTREEITEM hSelect = m_Tree.GetSelectedItem();
	CString StrBlockName = m_Tree.GetItemText(hSelect);
	
	T_Block *pBlockDel = CUserBlockManager::Instance()->GetBlock(StrBlockName);
	if ( NULL != pBlockDel && CUserBlockManager::Instance()->IsServerBlock(*pBlockDel) )	// 不允许删除服务器相关板块
	{
		MessageBox(_T("无法删除服务器同步板块！"), _T("删除板块"), MB_OK);
		return;
	}

	if ( NULL == hSelect )
	{
		MessageBox(_T("请您先选中板块"), AfxGetApp()->m_pszAppName);
		return;
	}
	
	if ( IDNO == MessageBox(L"确定要删除吗?",L"删除板块",MB_YESNO))
	{
		return;
	}
	
	//
	HTREEITEM hParent = m_Tree.GetParentItem(hSelect);	
	m_Tree.DeleteItem(hSelect);
	
	//
	if ( NULL != hParent )
	{
		m_Tree.Expand (hParent, TVE_EXPAND);
	}
	
	HTREEITEM hFirst = m_Tree.GetFirstVisibleItem();
	if ( NULL != hFirst )
	{
		m_Tree.Select(hFirst,TVGN_CARET);
	}
	else
	{
		m_List.DeleteAllItems();
	}

	//	
	CUserBlockManager::Instance()->DelUserBlock(StrBlockName);	
}

void CDlgUserBlockManager::OnButtonEditBlock() 
{
	// fan
	HTREEITEM hSelect = m_Tree.GetSelectedItem();
	if ( NULL == hSelect )
	{
		MessageBox(_T("请您先选中板块"), AfxGetApp()->m_pszAppName);
		return;
	}

	CString StrBlockName = m_Tree.GetItemText(hSelect);
	T_Block* pBlock = CUserBlockManager::Instance()->GetBlock(StrBlockName);
	
	if ( NULL == pBlock )
	{
		MessageBox(_T("板块信息错误"), AfxGetApp()->m_pszAppName);
		return;
	}

	//	
	CDlgBlockSet DlgBlockSet;
	
	DlgBlockSet.m_StrName	= pBlock->m_StrName;
	DlgBlockSet.m_StrHotKey = pBlock->m_StrHotKey;
	DlgBlockSet.SetColor(pBlock->m_clrBlock);
	DlgBlockSet.SetHead(pBlock->m_eHeadType);

	DlgBlockSet.SetModifyFlag(true);
	m_BindHotkey.Release();
	
	if ( IDOK == DlgBlockSet.DoModal() )
	{
		if ( DlgBlockSet.m_StrName.GetLength() > 0 )
		{
			T_Block stBlockNew;
		
			stBlockNew.m_StrName	= DlgBlockSet.m_StrName;
			stBlockNew.m_StrHotKey	= DlgBlockSet.m_StrHotKey;
			stBlockNew.m_clrBlock	= DlgBlockSet.GetColor();
			stBlockNew.m_eHeadType	= DlgBlockSet.GetHead();

			//
			m_Tree.SetItemText(hSelect,DlgBlockSet.m_StrName);		
			m_List.SetTextColor(DlgBlockSet.GetColor());
			m_List.Invalidate();
			
			//			
			CUserBlockManager::Instance()->ModifyUserBlock(pBlock->m_StrName, stBlockNew);				
		}
	}
	
	m_BindHotkey.AddRef();
}

void CDlgUserBlockManager::OnSelchangedTreeBlock(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// fan
	int32 iCount = m_Tree.GetCount();
	
	if ( iCount <= 1 )
	{
		GetDlgItem(IDC_BUTTON_DEL_BLOCK)->EnableWindow(FALSE);			
	}
	else
	{
		GetDlgItem(IDC_BUTTON_DEL_BLOCK)->EnableWindow(TRUE);			
	}
	
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hSelect =  pNMTreeView->itemNew.hItem;
	
	if ( NULL != hSelect )
	{
		int32 i = 0;		
		m_List.DeleteAllItems();
		
		// 得到这个板块
		CString StrBlockName = m_Tree.GetItemText(hSelect);
		T_Block* pBlock	= CUserBlockManager::Instance()->GetBlock(StrBlockName);
		if ( NULL == pBlock )
		{
			*pResult = 0;
			return;
		}
		else if ( CUserBlockManager::Instance()->IsServerBlock(*pBlock) )
		{
			GetDlgItem(IDC_BUTTON_DEL_BLOCK)->EnableWindow(FALSE);		// 服务器相关板块不允许删除
		}
		
		// 得到板块下所有的商品
		CArray<CMerch*, CMerch*> aMerchs;
		if ( !CUserBlockManager::Instance()->GetMerchsInBlock(StrBlockName, aMerchs) )
		{
			*pResult = 0;
			return;
		}
		
		// 设置列表的颜色, 填充商品
		//m_List.SetTextColor(pBlock->m_clrBlock);
		m_List.SetTextColor(RGB(0,0,0));
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
	
	*pResult = 0;
}

LRESULT CDlgUserBlockManager::OnMsgHotKey(WPARAM wParam, LPARAM lParam)
{
	// fan
	HTREEITEM hSelect = m_Tree.GetSelectedItem();
	if ( NULL == hSelect )
	{
		MessageBox(_T("请您先选中板块"), AfxGetApp()->m_pszAppName);
		return 0;
	}

	CString StrBlockName  = m_Tree.GetItemText(hSelect);
	
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

		if ( CUserBlockManager::Instance()->AddMerchToUserBlock(pMerch, StrBlockName) )
		{			
			int32 iCount = m_List.GetItemCount();
			//
			m_List.InsertItem(iCount, StrName, KMerchImageId);
			m_List.SetItemData (iCount, (DWORD)pMerch);			
		}
	}
	
	DEL(pHotKey);
	
	return 0;	
}

void CDlgUserBlockManager::OnButtonAddMerch() 
{
	//
	HTREEITEM hSelect = m_Tree.GetSelectedItem();
	if ( NULL == hSelect )
	{
		MessageBox(_T("请您先选中板块"), AfxGetApp()->m_pszAppName);
		return;
	}

	CDlgMerchsSelect DlgMerchSelect;
	m_BindHotkey.Release();

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
	pMainFrame->m_pDlgKeyBoard->SetAutoCloseFlag(false);

	// 移动到本窗口的右下角: MoveWindow 的时候,是根据父窗口的位置移动的.
	CRect rectWnd;
	GetClientRect(&rectWnd);
	ClientToScreen(&rectWnd);


	CRect rectKey;
	pMainFrame->m_pDlgKeyBoard->GetWindowRect(&rectKey);
	int32 iHeight = rectKey.Height();
	int32 iWidth  = rectKey.Width();
	
	pMainFrame->m_pDlgKeyBoard->SetWindowPos(&wndTopMost,rectWnd.left - 3 ,rectWnd.bottom - iHeight + 3 ,iWidth,iHeight,SWP_NOREDRAW|SWP_NOSIZE);
	pMainFrame->m_pDlgKeyBoard->ShowWindow(SW_SHOW);
	pMainFrame->SetHotkeyTarget(this);
}

void CDlgUserBlockManager::OnButtonDelMerch() 
{
	HTREEITEM hTree = m_Tree.GetSelectedItem();
	if ( NULL == hTree )
	{
		MessageBox(L"请选中板块", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return;
	}

	CString StrBlockName = m_Tree.GetItemText(hTree);

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

void CDlgUserBlockManager::OnButtonUp()
{
	HTREEITEM hitem = m_Tree.GetSelectedItem();
	if ( NULL == hitem )
	{
		return;
	}

	CString StrBlockName = m_Tree.GetItemText(hitem);

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

void CDlgUserBlockManager::OnButtonDown()
{
	HTREEITEM hitem = m_Tree.GetSelectedItem();
	if ( NULL == hitem )
	{
		return;
	}
	
	CString StrBlockName = m_Tree.GetItemText(hitem);
	
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

void CDlgUserBlockManager::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult)
{
	OnButtonDelMerch();

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////////////
CToolbarMC::CToolbarMC()
{
}

CToolbarMC::~CToolbarMC()
{
}

BOOL CToolbarMC::CreateEx2(CWnd* pParentWnd, DWORD dwCtrlStyle,DWORD dwStyle,CRect rcBorders,UINT nID)
{
	dwStyle &=~CBRS_SIZE_DYNAMIC;
	dwStyle |=CBRS_SIZE_FIXED;
	return CreateEx(pParentWnd,dwCtrlStyle,dwStyle,rcBorders,nID);
}

BOOL CToolbarMC::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;
}

void CToolbarMC::SetColumns(UINT nColumns)
{
  //     m_nColumns = nColumns;
       int nCount = GetToolBarCtrl().GetButtonCount();
       for (int i = 0; i < nCount; i++)
       {
              UINT nStyle = GetButtonStyle(i);
              BOOL bWrap = (((i + 1) % (int)nColumns) == 0);
              if (bWrap)
                     nStyle |= TBBS_WRAPPED; 
              else
                     nStyle &= ~TBBS_WRAPPED;
              SetButtonStyle(i, nStyle);
       }
       Invalidate();
//       GetParentFrame()->RecalcLayout();
}
//CToolBarCtrl;

BEGIN_MESSAGE_MAP(CToolbarMC, CToolBar)
	//{{AFX_MSG_MAP(CToolbarMC)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////////////
const int32 KiHideTipTimerID	 = 24667;
const int32 KiHideTipTimerPeriod = 2000;

static CDlgSDBar* sDlgSDBar = NULL;
void ShowSelfDrawBar ()
{
	CRect rcWnd, rcView, rcTmp;
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CIoViewBase *pIoViewActive = pMainFrame->FindActiveIoView();

	if ( NULL != sDlgSDBar )
	{
		if (sDlgSDBar->IsWindowVisible())
		{
			sDlgSDBar->ShowWindow(SW_HIDE);
		}
		else
		{
			if (NULL != pIoViewActive)
			{
				pIoViewActive->GetWindowRect(&rcView);
				sDlgSDBar->GetWindowRect(&rcWnd);
				
				rcTmp = rcWnd;
				rcWnd.left = rcView.right - rcTmp.Width();
				rcWnd.top = rcView.top + 25;
				rcWnd.right = rcWnd.left + rcTmp.Width();
				rcWnd.bottom = rcWnd.top + rcTmp.Height();
				sDlgSDBar->MoveWindow(rcWnd);
			}

			sDlgSDBar->ShowWindow(SW_SHOW);
		}

		return;
	}
	sDlgSDBar = new CDlgSDBar(AfxGetMainWnd());
	sDlgSDBar->Create(IDD_DIALOG_SELFDRAW,NULL); 

	if (NULL != pIoViewActive)
	{
		pIoViewActive->GetWindowRect(&rcView);
		sDlgSDBar->GetWindowRect(&rcWnd);
		
		rcTmp = rcWnd;
		rcWnd.left = rcView.right - rcTmp.Width();
		rcWnd.top = rcView.top + 25;
		rcWnd.right = rcWnd.left + rcTmp.Width();
		rcWnd.bottom = rcWnd.top + rcTmp.Height();
		sDlgSDBar->MoveWindow(rcWnd);
	}

	sDlgSDBar->ShowWindow(SW_SHOW);
}
void UpdateSelfDrawBar()
{
	if ( NULL != sDlgSDBar )
	{
		sDlgSDBar->ResetButton(true);
	}
}

CDlgSDBar::CDlgSDBar(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgSDBar::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSDBar)
	//}}AFX_DATA_INIT
	m_eSelfDrawType = ESDTInvalid;

	SetNeedChangeColor(true);
	SetCaptionBKColor(RGB(23, 23, 29));
	SetFrameColor(RGB(0, 0, 0));
	SetCaptionColor(RGB(255,255,255));
	SetNeedDrawSpliter(TRUE);
}

void CDlgSDBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSDBar)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgSDBar, CDialogEx)
	//{{AFX_MSG_MAP(CDlgSDBar)
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave) 
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDlgSDBar::OnDestroy()
{
	m_TipWnd.DestroyWindow();
	CDialog::OnDestroy();
	sDlgSDBar = NULL;
	delete this;
}
void CDlgSDBar::OnOK()
{
	CDialog::OnOK();
	DestroyWindow();
}
void CDlgSDBar::OnCancel()
{
	CDialog::OnCancel();
	DestroyWindow();
}
BOOL CDlgSDBar::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitBtns();

	m_TipWnd.Create(this);
    SetTimer(KiHideTipTimerID, KiHideTipTimerPeriod, NULL);
	return FALSE;
}

BOOL CDlgSDBar::OnEraseBkgnd( CDC* pDC )
{
	//CRect rc;
	//GetClientRect(rc);
	//pDC->FillSolidRect(rc, GetBackColor());
	
	return TRUE;
}

void CDlgSDBar::DrawButton()
{
	CClientDC dc(this);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bmp;
	CRect rcWnd;
	GetClientRect(&rcWnd);

	bmp.CreateCompatibleBitmap(&dc, rcWnd.Width(), rcWnd.Height());
	memDC.SelectObject(&bmp);
	memDC.FillSolidRect(0, 0, rcWnd.Width(), rcWnd.Height(), RGB(24,24,30));
	memDC.SetBkMode(TRANSPARENT);
	Gdiplus::Graphics graphics(memDC.GetSafeHdc());


	CRect rcPaint;
	dc.GetClipBox(&rcPaint);
	map<int, CNCButton>::iterator iter;
	CRect rcControl;

	for (iter=m_mapBtn.begin(); iter!=m_mapBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		btnControl.GetRect(rcControl);

		// 判断当前按钮是否需要重绘
		if (!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;
		}

		btnControl.DrawButton(&graphics);
	}

	//
	dc.BitBlt(0, 0, rcWnd.Width(), rcWnd.Height(), &memDC, 0, 0, SRCCOPY);
	dc.SelectClipRgn(NULL);
	memDC.DeleteDC();
	bmp.DeleteObject();
}

void CDlgSDBar::OnPaint()
{
	CPaintDC dc(this);
	DrawButton();
}

void CDlgSDBar::OnTimer(UINT nIDEvent)
{
	if ( nIDEvent == KiHideTipTimerID )
	{
		KillTimer(KiHideTipTimerID);
		//ShowTips();
		m_TipWnd.Hide();
	}
}

void CDlgSDBar::InitBtns()
{
	// 从左到右排列的按钮,每行四个按钮
	int iTopSpace =17, iBottomSpace = 17, iLeftSpace = 10, iRightSpace = 10;
	int iXSpace = 9,iYSpace = 12;
	int iXPos = iLeftSpace;
	int iYPos = iTopSpace;
	Image* pImg = NULL;
    
	CRect rcBtn(0, 0, 0, 0);
	//线段
	pImg = Image::FromFile(L"image//Drawline//line.png");
	if(pImg && pImg->GetLastStatus() == Ok)
	{
		rcBtn.top = iYPos;
		rcBtn.bottom = rcBtn.top + pImg->GetHeight() / 3;
		rcBtn.left = iXPos;
		rcBtn.right = rcBtn.left + pImg->GetWidth();
		AddButton(&rcBtn, pImg, 3, IDT_SD_LINE, L"");
		iXPos += (pImg->GetWidth() + iXSpace);
	}

	pImg = Image::FromFile(L"image//Drawline//lineex.png");
	if(pImg && pImg->GetLastStatus() == Ok)
	{
		rcBtn.top = iYPos;
		rcBtn.bottom = rcBtn.top + pImg->GetHeight() / 3;
		rcBtn.left = iXPos;
		rcBtn.right = rcBtn.left + pImg->GetWidth();
		AddButton(&rcBtn, pImg, 3, IDT_SD_LINEEX, L"");
		iXPos += (pImg->GetWidth() + iXSpace);
	}

	pImg = Image::FromFile(L"image//Drawline//jgtd.png");
	if(pImg && pImg->GetLastStatus() == Ok)
	{
		rcBtn.top = iYPos;
		rcBtn.bottom = rcBtn.top + pImg->GetHeight() / 3;
		rcBtn.left = iXPos;
		rcBtn.right = rcBtn.left + pImg->GetWidth();
		AddButton(&rcBtn, pImg, 3, IDT_SD_JGTD, L"");
		iXPos += (pImg->GetWidth() + iXSpace);
	}

	pImg = Image::FromFile(L"image//Drawline//pxzx.png");
	if(pImg && pImg->GetLastStatus() == Ok)
	{
		rcBtn.top = iYPos;
		rcBtn.bottom = rcBtn.top + pImg->GetHeight() / 3;
		rcBtn.left = iXPos;
		rcBtn.right = rcBtn.left + pImg->GetWidth();
		AddButton(&rcBtn, pImg, 3, IDT_SD_PXZX, L"");
		iXPos = iLeftSpace;
		iYPos +=(pImg->GetHeight() / 3 + iYSpace);
	}

	pImg = Image::FromFile(L"image//Drawline//yhx.png");
	if(pImg && pImg->GetLastStatus() == Ok)
	{
		rcBtn.top = iYPos;
		rcBtn.bottom = rcBtn.top + pImg->GetHeight() / 3;
		rcBtn.left = iXPos;
		rcBtn.right = rcBtn.left + pImg->GetWidth();
		AddButton(&rcBtn, pImg, 3, IDT_SD_YHX, L"");
		iXPos += (pImg->GetWidth() + iXSpace);
	}

	pImg = Image::FromFile(L"image//Drawline//hjfg.png");
	if(pImg && pImg->GetLastStatus() == Ok)
	{
		rcBtn.top = iYPos;
		rcBtn.bottom = rcBtn.top + pImg->GetHeight() / 3;
		rcBtn.left = iXPos;
		rcBtn.right = rcBtn.left + pImg->GetWidth();
		AddButton(&rcBtn, pImg, 3, IDT_SD_HJFG, L"");
		iXPos += (pImg->GetWidth() + iXSpace);
	}

	pImg = Image::FromFile(L"image//Drawline//bfb.png");
	if(pImg && pImg->GetLastStatus() == Ok)
	{
		rcBtn.top = iYPos;
		rcBtn.bottom = rcBtn.top + pImg->GetHeight() / 3;
		rcBtn.left = iXPos;
		rcBtn.right = rcBtn.left + pImg->GetWidth();
		AddButton(&rcBtn, pImg, 3, IDT_SD_BFB, L"");
		iXPos += (pImg->GetWidth() + iXSpace);
	}

	pImg = Image::FromFile(L"image//Drawline//bd.png");
	if(pImg && pImg->GetLastStatus() == Ok)
	{
		rcBtn.top = iYPos;
		rcBtn.bottom = rcBtn.top + pImg->GetHeight() / 3;
		rcBtn.left = iXPos;
		rcBtn.right = rcBtn.left + pImg->GetWidth();
		AddButton(&rcBtn, pImg, 3, IDT_SD_BD, L"");
		iXPos = iLeftSpace;
		iYPos +=(pImg->GetHeight() / 3 + iYSpace);
	}

	pImg = Image::FromFile(L"image//Drawline//jx.png");
	if(pImg && pImg->GetLastStatus() == Ok)
	{
		rcBtn.top = iYPos;
		rcBtn.bottom = rcBtn.top + pImg->GetHeight() / 3;
		rcBtn.left = iXPos;
		rcBtn.right = rcBtn.left + pImg->GetWidth();
		AddButton(&rcBtn, pImg, 3, IDT_SD_JX, L"");
		iXPos += (pImg->GetWidth() + iXSpace);
	}

	pImg = Image::FromFile(L"image//Drawline//jxhgd.png");
	if(pImg && pImg->GetLastStatus() == Ok)
	{
		rcBtn.top = iYPos;
		rcBtn.bottom = rcBtn.top + pImg->GetHeight() / 3;
		rcBtn.left = iXPos;
		rcBtn.right = rcBtn.left + pImg->GetWidth();
		AddButton(&rcBtn, pImg, 3, IDT_SD_JXHGD, L"");
		iXPos += (pImg->GetWidth() + iXSpace);
	}

	pImg = Image::FromFile(L"image//Drawline//ycxxhgd.png");
	if(pImg && pImg->GetLastStatus() == Ok)
	{
		rcBtn.top = iYPos;
		rcBtn.bottom = rcBtn.top + pImg->GetHeight() / 3;
		rcBtn.left = iXPos;
		rcBtn.right = rcBtn.left + pImg->GetWidth();
		AddButton(&rcBtn, pImg, 3, IDT_SD_YCXXHGD, L"");
		iXPos += (pImg->GetWidth() + iXSpace);
	}

	pImg = Image::FromFile(L"image//Drawline//xxhgd.png");
	if(pImg && pImg->GetLastStatus() == Ok)
	{
		rcBtn.top = iYPos;
		rcBtn.bottom = rcBtn.top + pImg->GetHeight() / 3;
		rcBtn.left = iXPos;
		rcBtn.right = rcBtn.left + pImg->GetWidth();
		AddButton(&rcBtn, pImg, 3, IDT_SD_XXHGD, L"");
		iXPos = iLeftSpace;
		iYPos +=(pImg->GetHeight() / 3 + iYSpace);
	}

	pImg = Image::FromFile(L"image//Drawline//zq.png");
	if(pImg && pImg->GetLastStatus() == Ok)
	{
		rcBtn.top = iYPos;
		rcBtn.bottom = rcBtn.top + pImg->GetHeight() / 3;
		rcBtn.left = iXPos;
		rcBtn.right = rcBtn.left + pImg->GetWidth();
		AddButton(&rcBtn, pImg, 3, IDT_SD_ZQ, L"");
		iXPos += (pImg->GetWidth() + iXSpace);
	}

	pImg = Image::FromFile(L"image//Drawline//fblq.png");
	if(pImg && pImg->GetLastStatus() == Ok)
	{
		rcBtn.top = iYPos;
		rcBtn.bottom = rcBtn.top + pImg->GetHeight() / 3;
		rcBtn.left = iXPos;
		rcBtn.right = rcBtn.left + pImg->GetWidth();
		AddButton(&rcBtn, pImg, 3, IDT_SD_FBLQ, L"");
		iXPos += (pImg->GetWidth() + iXSpace);
	}

	pImg = Image::FromFile(L"image//Drawline//zs.png");
	if(pImg && pImg->GetLastStatus() == Ok)
	{
		rcBtn.top = iYPos;
		rcBtn.bottom = rcBtn.top + pImg->GetHeight() / 3;
		rcBtn.left = iXPos;
		rcBtn.right = rcBtn.left + pImg->GetWidth();
		AddButton(&rcBtn, pImg, 3, IDT_SD_ZS, L"");
		iXPos += (pImg->GetWidth() + iXSpace);
	}

	pImg = Image::FromFile(L"image//Drawline//jejd.png");
	if(pImg && pImg->GetLastStatus() == Ok)
	{
		rcBtn.top = iYPos;
		rcBtn.bottom = rcBtn.top + pImg->GetHeight() / 3;
		rcBtn.left = iXPos;
		rcBtn.right = rcBtn.left + pImg->GetWidth();
		AddButton(&rcBtn, pImg, 3, IDT_SD_JEJD, L"");
		iXPos = iLeftSpace;
		iYPos +=(pImg->GetHeight() / 3 + iYSpace);
	}

	pImg = Image::FromFile(L"image//Drawline//45up.png");
	if(pImg && pImg->GetLastStatus() == Ok)
	{
		rcBtn.top = iYPos;
		rcBtn.bottom = rcBtn.top + pImg->GetHeight() / 3;
		rcBtn.left = iXPos;
		rcBtn.right = rcBtn.left + pImg->GetWidth();
		AddButton(&rcBtn, pImg, 3, IDT_SD_45UP, L"");
		iXPos += (pImg->GetWidth() + iXSpace);
	}

	pImg = Image::FromFile(L"image//Drawline//45down.png");
	if(pImg && pImg->GetLastStatus() == Ok)
	{
		rcBtn.top = iYPos;
		rcBtn.bottom = rcBtn.top + pImg->GetHeight() / 3;
		rcBtn.left = iXPos;
		rcBtn.right = rcBtn.left + pImg->GetWidth();
		AddButton(&rcBtn, pImg, 3, IDT_SD_45DOWN, L"");
		iXPos += (pImg->GetWidth() + iXSpace);
	}

	pImg = Image::FromFile(L"image//Drawline//vertical.png");
	if(pImg && pImg->GetLastStatus() == Ok)
	{
		rcBtn.top = iYPos;
		rcBtn.bottom = rcBtn.top + pImg->GetHeight() / 3;
		rcBtn.left = iXPos;
		rcBtn.right = rcBtn.left + pImg->GetWidth();
		AddButton(&rcBtn, pImg, 3, IDT_SD_VERTICAL, L"");
		iXPos += (pImg->GetWidth() + iXSpace);
	}

	pImg = Image::FromFile(L"image//Drawline//horizontal.png");
	if(pImg && pImg->GetLastStatus() == Ok)
	{
		rcBtn.top = iYPos;
		rcBtn.bottom = rcBtn.top + pImg->GetHeight() / 3;
		rcBtn.left = iXPos;
		rcBtn.right = rcBtn.left + pImg->GetWidth();
		AddButton(&rcBtn, pImg, 3, IDT_SD_HORIZONTAL, L"");
		iXPos = iLeftSpace;
		iYPos +=(pImg->GetHeight() / 3 + iYSpace);
	}

	pImg = Image::FromFile(L"image//Drawline//zbj.png");
	if(pImg && pImg->GetLastStatus() == Ok)
	{
		rcBtn.top = iYPos;
		rcBtn.bottom = rcBtn.top + pImg->GetHeight() / 3;
		rcBtn.left = iXPos;
		rcBtn.right = rcBtn.left + pImg->GetWidth();
		AddButton(&rcBtn, pImg, 3, IDT_SD_ZBJ, L"");
		iXPos += (pImg->GetWidth() + iXSpace);
	}

	pImg = Image::FromFile(L"image//Drawline//dbj.png");
	if(pImg && pImg->GetLastStatus() == Ok)
	{
		rcBtn.top = iYPos;
		rcBtn.bottom = rcBtn.top + pImg->GetHeight() / 3;
		rcBtn.left = iXPos;
		rcBtn.right = rcBtn.left + pImg->GetWidth();
		AddButton(&rcBtn, pImg, 3, IDT_SD_DBJ, L"");
		iXPos += (pImg->GetWidth() + iXSpace);
	}

	pImg = Image::FromFile(L"image//Drawline//text.png");
	if(pImg && pImg->GetLastStatus() == Ok)
	{
		rcBtn.top = iYPos;
		rcBtn.bottom = rcBtn.top + pImg->GetHeight() / 3;
		rcBtn.left = iXPos;
		rcBtn.right = rcBtn.left + pImg->GetWidth();
		AddButton(&rcBtn, pImg, 3, IDT_SD_TEXT, L"");
		iXPos += (pImg->GetWidth() + iXSpace);
	}

	pImg = Image::FromFile(L"image//Drawline//color.png");
	if(pImg && pImg->GetLastStatus() == Ok)
	{
		rcBtn.top = iYPos;
		rcBtn.bottom = rcBtn.top + pImg->GetHeight() / 3;
		rcBtn.left = iXPos;
		rcBtn.right = rcBtn.left + pImg->GetWidth();
		AddButton(&rcBtn, pImg, 3, IDT_SD_COLOR, L"");
		iXPos = iLeftSpace;
		iYPos +=(pImg->GetHeight() / 3 + iYSpace);
	}

	pImg = Image::FromFile(L"image//Drawline//delete.png");
	if(pImg && pImg->GetLastStatus() == Ok)
	{
		rcBtn.top = iYPos;
		rcBtn.bottom = rcBtn.top + pImg->GetHeight() / 3;
		rcBtn.left = iXPos;
		rcBtn.right = rcBtn.left + pImg->GetWidth();
		AddButton(&rcBtn, pImg, 3, IDT_SD_DELETE, L"");
	}
}

void CDlgSDBar::AddButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption)
{
	ASSERT(pImage);
	CNCButton btnControl; 
	btnControl.CreateButton(lpszCaption, lpRect, this, pImage, nCount, nID);
	btnControl.SetParentFocus(FALSE);
	btnControl.SetFont(_T("宋体"),10.5, FontStyleBold);
    btnControl.SetClientDC(TRUE);
	m_mapBtn[nID] = btnControl;
}

int	 CDlgSDBar::TButtonHitTest(CPoint point)
{
	map<int, CNCButton>::iterator iter;

	// 遍历所有按钮
	for (iter=m_mapBtn.begin(); iter!=m_mapBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;

		// 点point是否在已绘制的按钮区域内
		if (btnControl.PtInButton(point) && btnControl.IsEnable())
		{
			return btnControl.GetControlId();
		}
	}

	return INVALID_ID;
}


void CDlgSDBar::OnMouseMove(UINT nFlags, CPoint point)
{
	int32 iButton = TButtonHitTest(point);

	if (iButton != m_iXButtonHovering)
	{
		if (INVALID_ID != m_iXButtonHovering)
		{
			m_mapBtn[m_iXButtonHovering].MouseLeave();
			m_iXButtonHovering = INVALID_ID;
		}

		if (INVALID_ID != iButton)
		{	
			m_mapBtn[m_iXButtonHovering].MouseHover();		
		}
	}

	// 响应 WM_MOUSELEAVE消息
	TRACKMOUSEEVENT csTME;
	csTME.cbSize	= sizeof (csTME);
	csTME.dwFlags	= TME_LEAVE;
	csTME.hwndTrack = m_hWnd ;		// 指定要追踪的窗口 
	::_TrackMouseEvent (&csTME);	// 开启Windows的WM_MOUSELEAVE事件支持 

	CDialog::OnMouseMove(nFlags,point);
}

LRESULT CDlgSDBar::OnMouseLeave(WPARAM wParam, LPARAM lParam)       
{     
	if (INVALID_ID != m_iXButtonHovering)
	{
		m_mapBtn[m_iXButtonHovering].MouseLeave();	
		m_iXButtonHovering = INVALID_ID;
	}

	return 0;       
} 
void CDlgSDBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int32 iButton = TButtonHitTest(point);
	if (INVALID_ID != iButton)
	{
		m_mapBtn[iButton].LButtonDown();
	}
	CDialog::OnLButtonDown(nFlags, point);
}

void CDlgSDBar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	int32 iButton = TButtonHitTest(point);
	if (INVALID_ID != iButton)
	{
		m_mapBtn[iButton].LButtonUp();
	}

	CDialog::OnLButtonUp(nFlags, point);
}

BOOL CDlgSDBar::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	int32 iID = (int32)wParam;
	std::map<int, CNCButton>::iterator it = m_mapBtn.find(iID);
	if (m_mapBtn.end() != it)
	{
		ResetButton(false);
		//m_mapBtn[iID].EnableButton(FALSE);

		if ( iID == IDT_SD_DELETE )
		{
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			CIoViewBase* pBase = pFrame->FindActiveIoView();
			if ( NULL != pBase )
			{
				if ( pBase->IsKindOf(RUNTIME_CLASS(CIoViewChart)))
				{
					CIoViewChart* pChart = (CIoViewChart*)pBase;
					pChart->RemoveAllSelfDrawCurve();
				}
			}
			return CDialog::OnCommand(wParam, lParam);
		}

		m_eSelfDrawType = E_SelfDrawType(iID - IDT_SD_BEGIN );

		CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
		CIoViewBase* pActive = pMain->FindActiveIoView();
		CIoViewChart* pChart = NULL;
		if (NULL != pActive && pActive->IsKindOf(RUNTIME_CLASS(CIoViewChart)))
		{
			pChart = (CIoViewChart*)pActive;
		}

		if(ESDTColor == m_eSelfDrawType)
		{
			// 设置颜色
			if (NULL == pChart)
			{
				::MessageBox(AfxGetMainWnd()->GetSafeHwnd(), L"没有激活视图可以设置", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
			}
			else
			{
				CSelfDrawCurve* pSelfDrawCurve = pChart->GetPickedSelfDrawCurve();
				if (NULL == pSelfDrawCurve)
				{
					::MessageBox(AfxGetMainWnd()->GetSafeHwnd(), L"请选中需要设置的自画线", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
				}
				else
				{
					//
					CColorDialog dlgColor(pSelfDrawCurve->GetNode()->m_clr);

					if (IDOK == dlgColor.DoModal())
					{
						UpdateSelfDrawBar();

						COLORREF color = dlgColor.GetColor();	
						pChart->PostMessage(UM_SELFDRAW_Color_Change, color, 0);
					}
				}
			}

			m_mapBtn[iID].EnableButton(TRUE);
			Invalidate();
		}
		else
		{
			CChartRegion::SetSelfDrawType(m_eSelfDrawType, pChart);
		}

		::SetCursor(AfxGetApp()->LoadCursor(IDC_CUR_SDCHG));
	}

	return CDialog::OnCommand(wParam, lParam);
}

BOOL CDlgSDBar::PreTranslateMessage(MSG* pMsg)
{
	CString StrText;
	if ( pMsg->message == WM_MOUSEMOVE)
	{
		CPoint point; 
		GetCursorPos(&point);
		ScreenToClient(&point);

		CRect rectClient;
		GetClientRect(&rectClient);

		if (!PtInRect(&rectClient, point))
		{
			return FALSE;
		}

		// 判断需要显示文字
		CRect rcControl;
		int iBtnIndex = -1;
		for (int i = IDT_SD_LINE; i <= IDT_SD_DELETE; i++)
		{
			CNCButton &btnControl = m_mapBtn[i];
			btnControl.GetRect(rcControl);

			if (rcControl.PtInRect(point))
			{
				iBtnIndex = i;
				break;
			}
		}
		if(iBtnIndex == -1)
		{
			return FALSE;
		}

		int32 id = m_mapBtn[iBtnIndex].GetControlId();//ctrl.HitTest(&pt);
		
		if ( id >= 0 )
		{
			switch ( id )
			{
			case IDT_SD_LINE:
				StrText = _T("线段");
				break;
			case IDT_SD_LINEEX:
				StrText = _T("直线");
				break;
			case IDT_SD_JGTD:
				StrText = _T("价格通道线");
				break;
			case IDT_SD_PXZX:
				StrText = _T("平行直线");
				break;
			case IDT_SD_YHX:
				StrText = _T("圆弧线");
				break;
			case IDT_SD_HJFG:
				StrText = _T("黄金分割线");
				break;
			case IDT_SD_BFB:
				StrText = _T("百分比线");
				break;
			case IDT_SD_BD:
				StrText = _T("波段线");
				break;
			case IDT_SD_JX:
				StrText = _T("矩形");
				break;
			case IDT_SD_JXHGD:
				StrText = _T("矩形回归带");
				break;
			case IDT_SD_YCXXHGD:
				StrText = _T("延长线型回归带");
				break;
			case IDT_SD_XXHGD:
				StrText = _T("线型回归带");
				break;
			case IDT_SD_ZQ:
				StrText = _T("周期线");
				break;
			case IDT_SD_FBLQ:
				StrText = _T("费波拉契线");
				break;
			case IDT_SD_ZS:
				StrText = _T("阻速线");
				break;
			case IDT_SD_JEJD:
				StrText = _T("江恩角度线");
				break;
			case IDT_SD_ZBJ:
				StrText = _T("涨标记");
				break;
			case IDT_SD_DBJ:
				StrText = _T("跌标记");
				break;
			case IDT_SD_TEXT:
				StrText = _T("文字注释");
				break;
			case IDT_SD_DELETE:
				StrText = _T("删除全部");
				break;
			case IDT_SD_45UP:
				StrText = _T("45°向上");
				break;
			case IDT_SD_45DOWN:
				StrText = _T("45°向下");
				break;
			case IDT_SD_VERTICAL:
				StrText = _T("竖线");
				break;
			case IDT_SD_HORIZONTAL:
				StrText = _T("水平线");
				break;
			case IDT_SD_COLOR:
				StrText = _T("设置颜色");
				break;
			default:
				break;

			}
		}
	}
	if ( StrText.GetLength() > 0 )
	{
		CPoint point;
		GetCursorPos(&point);
		ScreenToClient(&point);

		CString StrTemp;
		if ( !m_TipWnd.IsWindowVisible())
		{
			m_TipWnd.Show(point,StrText);
		}
		else
		{
			m_TipWnd.GetWindowText(StrTemp);
			if ( StrTemp != StrText )
			{
				m_TipWnd.Show(point,StrText);
			}
		}

		KillTimer(KiHideTipTimerID);
		SetTimer(KiHideTipTimerID, KiHideTipTimerPeriod, NULL);	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgSDBar::ResetButton(bool32 bLayout)
{
	for (int32 i = IDT_SD_LINE; i <= IDT_SD_DELETE; i++)
	{
		m_mapBtn[i].EnableButton(TRUE);

	}
	if ( bLayout )
	{
		Invalidate();
	}
}


///////////////////////////////////////////////////////////////////////
// *****************************************************************
// 左边树的节点保存的是对应的XML 节点信息
// 右边列表的节点保存的是商品信息 ,包括: StrMerchCode 和 iMarketId
// *****************************************************************


















