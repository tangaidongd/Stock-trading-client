// dlgblockselect.cpp : implementation file
//

#include "stdafx.h"


#include "tinyxml.h"

#include "UserBlockManager.h"

#include "ShareFun.h"
#include "PathFactory.h"
#include "dlgblockselect.h"
#include "BlockManager.h"
#include "BlockConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgBlockSelect dialog

const int32 KTreeNullItemData = 100000;

CDlgBlockSelect::CDlgBlockSelect()
	: CPropertyPage(CDlgBlockSelect::IDD)
{
	//{{AFX_DATA_INIT(CDlgBlockSelect)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_iType = 0;
	m_pParent = NULL;
}

void CDlgBlockSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBlockSelect)
	DDX_Control(pDX, IDC_TREE_BLOCK, m_Tree);
	//}}AFX_DATA_MAP
}
BOOL CDlgBlockSelect::OnSetActive( )
{
	return CPropertyPage::OnSetActive();
}

BEGIN_MESSAGE_MAP(CDlgBlockSelect, CDialog)
	//{{AFX_MSG_MAP(CDlgBlockSelect)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_BLOCK, OnSelchangedTreeBlock)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBlockSelect message handlers

BOOL CDlgBlockSelect::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ImageList.Create (IDB_FORMULA, 16, 1, RGB(255,255,255));
	m_Tree.SetImageList(&m_ImageList, TVSIL_NORMAL);

	switch(m_iType)
	{
	case T_BlockDesc::EBTPhysical:
		UpdateSysBlock();
		break;
	case T_BlockDesc::EBTUser:
		UpdateUserBlock();
		break;
	case T_BlockDesc::EBTLogical:
		UpdateLogicBlock();
		break;
	case T_BlockDesc::EBTBlockLogical:		// T_BlockDesc::EBTBlockLogical
		UpdateServerLogicBlock();
		break;
	default:
		//ASSERT(0);
		break;
	}
	return TRUE;
}

void CDlgBlockSelect::OnSelchangedTreeBlock(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hSelect =  pNMTreeView->itemNew.hItem;
	if ( (hSelect!=NULL) && (m_pParent!=NULL) )
	{
		HTREEITEM hChild = m_Tree.GetChildItem(hSelect);
		CWnd* pWnd = m_pParent->GetDlgItem(IDOK);
		if ( NULL == pWnd )
		{
			return;
		}

		if ( NULL == hChild )
		{
			// 不是叶子节点
			pWnd->EnableWindow(TRUE);	
		}
		else
		{
			pWnd->EnableWindow(FALSE);
		}
	}
}

void CDlgBlockSelect::OnOK()
{
	CPropertyPage::OnOK();
}

void CDlgBlockSelect::UpdateUserBlock()
{
	m_Tree.DeleteAllItems();
	
	CArray<T_Block, T_Block&> aUserBlocks;
	CUserBlockManager::Instance()->GetBlocks(aUserBlocks);
	
	for ( int32 i = 0; i < aUserBlocks.GetSize(); i++ )
	{
		T_Block stBlock = aUserBlocks.GetAt(i);
		m_Tree.InsertItem(stBlock.m_StrName, KUserBlockImageId, KUserBlockImageSelectId);		
	}
}

void CDlgBlockSelect::UpdateLogicBlock()
{
	m_Tree.DeleteAllItems();
	
	CArray<T_Block, T_Block&> aSysBlocks;
	CSysBlockManager::Instance()->GetBlocks(aSysBlocks);
	
	for ( int32 i = 0; i < aSysBlocks.GetSize(); i++ )
	{
		T_Block stBlock = aSysBlocks.GetAt(i);
		m_Tree.InsertItem(stBlock.m_StrName, KUserBlockImageId, KUserBlockImageSelectId);		
	}
}

void CDlgBlockSelect::UpdateSysBlock()
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;	
	ASSERT(NULL!= pDocument);
	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;

	int32 i, iSize = pAbsCenterManager->GetMerchManager().m_BreedListPtr.GetSize();
	for ( i = 0; i < iSize; i ++ )
	{
		CBreed* pBreed = pAbsCenterManager->GetMerchManager().m_BreedListPtr.GetAt(i);
		HTREEITEM hBreed = m_Tree.InsertItem(pBreed->m_StrBreedCnName, KBreedImageId, KBreedImageSelectId, NULL);

		int32 j, iSize2 = pBreed->m_MarketListPtr.GetSize();

		for ( j = 0; j < iSize2; j ++ )
		{
			CMarket* pMarket = pBreed->m_MarketListPtr.GetAt(j);
			m_Tree.InsertItem(pMarket->m_MarketInfo.m_StrCnName,KMarketImageId,KMarketImageSelectId,hBreed);
		}
	}
}

void CDlgBlockSelect::UpdateServerLogicBlock()
{
	m_Tree.DeleteAllItems();

	CBlockConfig::IdArray aCollectionIds;
	CBlockConfig::Instance()->GetCollectionIdArray(aCollectionIds);

	for ( int i=0; i < aCollectionIds.GetSize() ; i++ )
	{
		CBlockCollection *pCol = CBlockConfig::Instance()->GetBlockCollectionById(aCollectionIds[i]);
		if ( NULL == pCol || pCol->IsUserBlockCollection() )	// 不插入自定
		{
			continue;
		}

		HTREEITEM hCol = m_Tree.InsertItem(pCol->m_StrName, KBreedImageId, KBreedImageSelectId, NULL, NULL);
		if ( NULL == hCol )
		{
			continue;
		}

		CBlockCollection::BlockArray aBlocks;
		pCol->GetValidBlocks(aBlocks);
		for ( int j=0; j < aBlocks.GetSize() ; j++ )
		{
			if ( NULL == aBlocks[j] )
			{
				break;
			}
			HTREEITEM hBlock = m_Tree.InsertItem(aBlocks[j]->m_blockInfo.m_StrBlockName, KMarketImageId, KMarketImageSelectId, hCol, NULL);
			if ( NULL != hBlock )
			{
				m_Tree.SetItemData(hBlock, aBlocks[j]->m_blockInfo.m_iBlockId);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CBlockSelect, CPropertySheetEX)

CBlockSelect::CBlockSelect(T_BlockDesc BlockDesc,LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheetEX(pszCaption, pParentWnd, iSelectPage)
{
	SetTitle(_T("选择板块"));

	m_BlockDesc.m_iMarketId    = BlockDesc.m_iMarketId;
	m_BlockDesc.m_eType		   = BlockDesc.m_eType;
	m_BlockDesc.m_StrBlockName = BlockDesc.m_StrBlockName;

	m_DialogSys.m_iType = T_BlockDesc::EBTPhysical;
	m_DialogSys.m_pParent = this;

	m_DialogUser.m_iType = T_BlockDesc::EBTUser;
	m_DialogUser.m_pParent = this;
	
	//m_DialogLogic.m_iType = 2;
	m_DialogLogic.m_iType = T_BlockDesc::EBTBlockLogical;
	m_DialogLogic.m_pParent = this;

	AddPages();
}

CBlockSelect::~CBlockSelect()
{

}

BOOL CBlockSelect::OnCommand(WPARAM wParam, LPARAM lParam)
{
    if ( wParam == IDOK && 0 != lParam )
    {
		CDlgBlockSelect* pDialog = (CDlgBlockSelect*)GetActivePage();
		HTREEITEM hSelect = pDialog->m_Tree.GetSelectedItem();
		
		if ( NULL == hSelect )
		{
			MessageBox(_T("请您先选中板块"), AfxGetApp()->m_pszAppName);
			return TRUE;
		}
		
		CString StrSelectBlock = pDialog->m_Tree.GetItemText(hSelect);
		
		m_BlockDesc.m_eType	= (T_BlockDesc::E_BlockType)pDialog->m_iType;

		if ( T_BlockDesc::EBTPhysical == m_BlockDesc.m_eType )
		{
			// 物理板块,这个id 号才有意义
			CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
			CGGTongDoc *pDocument = pApp->m_pDocument;	
			ASSERT(NULL!= pDocument);
			CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;

			CMarket* pMarket = NULL;
			pAbsCenterManager->GetMerchManager().FindMarket(StrSelectBlock, pMarket);

			if ( NULL == pMarket )
			{
				//ASSERT(0);
				MessageBox(_T("请您选择正确板块"), AfxGetApp()->m_pszAppName);	
				return FALSE;
			}

			m_BlockDesc.m_iMarketId	= pMarket->m_MarketInfo.m_iMarketId;
			m_BlockDesc.m_pMarket	= pMarket;
		}
		else if ( T_BlockDesc::EBTBlockLogical == m_BlockDesc.m_eType )
		{
			m_BlockDesc.m_iMarketId = (int32)pDialog->m_Tree.GetItemData(hSelect);
			CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(m_BlockDesc.m_iMarketId);
			if ( NULL == pBlock )
			{
				//ASSERT(0);
				MessageBox(_T("请您选择正确板块"), AfxGetApp()->m_pszAppName);	
				return FALSE;
			}

			m_BlockDesc.m_aMerchs.Copy(pBlock->m_blockInfo.m_aSubMerchs);
		}
		else
		{
			m_BlockDesc.m_iMarketId	= -1;
		}
		
		m_BlockDesc.m_StrBlockName = StrSelectBlock;
	}

	return CPropertySheet::OnCommand(wParam, lParam);
} 

BOOL CBlockSelect::OnInitDialog()
{
	CWnd* pBtn = GetDlgItem (ID_APPLY_NOW);
	if ( NULL != pBtn )
	{
		pBtn->ShowWindow(SW_HIDE);
		pBtn->EnableWindow(FALSE);
	}

	pBtn = GetDlgItem (IDCANCEL);
	if ( NULL != pBtn )
	{
		CRect rectBtn;
		pBtn->GetWindowRect(&rectBtn);
		ScreenToClient(&rectBtn);

		rectBtn.left	+= 50;
		rectBtn.right	+= 50;

		pBtn->MoveWindow(&rectBtn);		
	}

	pBtn = GetDlgItem (IDOK);
	if ( NULL != pBtn )
	{
		CRect rectBtn;
		pBtn->GetWindowRect(&rectBtn);
		ScreenToClient(&rectBtn);
		
		rectBtn.left	+= 50;
		rectBtn.right	+= 50;
		
		pBtn->MoveWindow(&rectBtn);	
	}

	BOOL bInit = CPropertySheet::OnInitDialog();

	TC_ITEM tcItem;   
	tcItem.mask = TCIF_TEXT;   
	tcItem.pszText = _T("物理板块");
	GetTabControl()->SetItem(0,&tcItem);

	tcItem.pszText = _T("股票板块");//_T("系统板块");
	GetTabControl()->SetItem(1,&tcItem);

	tcItem.pszText = _T("用户板块");
	GetTabControl()->SetItem(2,&tcItem);

	switch ( m_BlockDesc.m_eType )
	{
	case T_BlockDesc::EBTPhysical:
		SetActivePage(&m_DialogSys);
		break;
	case T_BlockDesc::EBTUser:
		SetActivePage(&m_DialogUser);
		break;
	case T_BlockDesc::EBTBlockLogical:
		SetActivePage(&m_DialogLogic);
		break;
	default:
		break;
	}

	return bInit;
}
void CBlockSelect::AddPages()
{
	AddPage(&m_DialogSys);
	AddPage(&m_DialogLogic);
	AddPage(&m_DialogUser);
}
BEGIN_MESSAGE_MAP(CBlockSelect, CPropertySheetEX)
	//{{AFX_MSG_MAP(CBlockSelect)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////////