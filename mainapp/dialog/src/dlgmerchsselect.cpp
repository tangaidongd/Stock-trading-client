// dlgmerchsselect.cpp : implementation file
//

#include "stdafx.h"


#include "MerchManager.h"

#include "UserBlockManager.h"
#include "dlgmerchsselect.h"
#include "ShareFun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgMerchsSelect dialog


CDlgMerchsSelect::CDlgMerchsSelect(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgMerchsSelect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMerchsSelect)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgMerchsSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMerchsSelect)
	DDX_Control(pDX, IDC_TREE_BLOCK, m_Tree);
	DDX_Control(pDX, IDC_LIST_MERCH, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMerchsSelect, CDialogEx)
	//{{AFX_MSG_MAP(CDlgMerchsSelect)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_BLOCK, OnSelchangedTreeBlock)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMerchsSelect message handlers

void CDlgMerchsSelect::OnOK() 
{
	m_SelectNames.RemoveAll();
	m_SelectCodes.RemoveAll();
	m_SelectIds.RemoveAll();
	int32 i, iSize = m_List.GetItemCount();
	for ( i = 0; i < iSize; i ++ )
	{
		UINT state = m_List.GetItemState(i,LVIS_SELECTED);
		if ( state == LVIS_SELECTED )
		{
			m_SelectNames.Add(m_MerchNames.GetAt(i));
			m_SelectCodes.Add(m_MerchCodes.GetAt(i));
			m_SelectIds.Add(m_MerchIds.GetAt(i));
		}
	}
	if ( m_SelectCodes.GetSize() < 1 )
	{
		MessageBox(_T("您没有选中商品"), AfxGetApp()->m_pszAppName);
		return;
	}
	CDialog::OnOK();
}

BOOL CDlgMerchsSelect::OnInitDialog() 
{
	CDialog::OnInitDialog();
	HICON hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,FALSE);	
	
	m_ImageList.Create ( IDB_FORMULA,16,1,RGB(255,255,255) );
	m_Tree.SetImageList( &m_ImageList,TVSIL_NORMAL);
	m_List.SetImageList( &m_ImageList,LVSIL_SMALL);

	m_List.InsertColumn(0,_T("商品"),LVCFMT_LEFT,40);

	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;	
	ASSERT(NULL!= pDocument);
	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;

	int32 i, iSize = pAbsCenterManager->GetMerchManager().m_BreedListPtr.GetSize();
	for ( i = 0; i < iSize; i ++ )
	{
		CBreed* pBreed = pAbsCenterManager->GetMerchManager().m_BreedListPtr.GetAt(i);
		HTREEITEM hBreed = m_Tree.InsertItem(pBreed->m_StrBreedCnName,KBreedImageId,KBreedImageSelectId,NULL);
		//m_Tree.SetItemData(hBreed,pBreed->m_iBreedId);
		int32 j, iSize2 = pBreed->m_MarketListPtr.GetSize();
		for ( j = 0; j < iSize2; j ++ )
		{
			CMarket* pMarket = pBreed->m_MarketListPtr.GetAt(j);
			HTREEITEM hMarket = m_Tree.InsertItem(pMarket->m_MarketInfo.m_StrCnName,KMarketImageId,KMarketImageSelectId,hBreed);
			m_Tree.SetItemData(hMarket,pMarket->m_MarketInfo.m_iMarketId);
		}
	}
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMerchsSelect::OnSelchangedTreeBlock(NMHDR* pNMHDR, LRESULT* pResult) 
{
	bool32 bExportFirst10Merch = false;
	if ( 0 == _taccess(_T("./dump/export_first_10_merch"),0))
	{
		bExportFirst10Merch = true;
	}

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hSelect =  pNMTreeView->itemNew.hItem;
	if ( NULL != hSelect )
	{
		m_List.DeleteAllItems();
		m_MerchNames.RemoveAll();
		m_MerchCodes.RemoveAll();
		m_MerchIds.RemoveAll();
		DWORD iMarketId = m_Tree.GetItemData(hSelect);
		if ( iMarketId > 0 )
		{
			CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
			CGGTongDoc *pDocument = pApp->m_pDocument;	
			ASSERT(NULL!= pDocument);
			CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
			CMarket* pMarket = NULL;
			pAbsCenterManager->GetMerchManager().FindMarket(iMarketId,pMarket);
			int32 i, iSize = pMarket->m_MerchsPtr.GetSize();
			for ( i = 0; i < iSize; i ++ )
			{
				CMerch* pMerch = pMarket->m_MerchsPtr.GetAt(i);
				int32 iCount = m_List.GetItemCount();
				m_List.InsertItem(iCount,pMerch->m_MerchInfo.m_StrMerchCnName,KMerchImageId);
				m_MerchNames.Add(pMerch->m_MerchInfo.m_StrMerchCnName);
				m_MerchCodes.Add(pMerch->m_MerchInfo.m_StrMerchCode);
				m_MerchIds.Add(pMerch->m_MerchInfo.m_iMarketId);
				if ( bExportFirst10Merch && i < 10 )
				{
					CString StrLog;
					StrLog.Format(_T("%d %s %s"),
						pMerch->m_MerchInfo.m_iMarketId,
						pMerch->m_Market.m_MarketInfo.m_StrCnName.GetBuffer(),
						pMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer());
					_LogSingleString ( StrLog );
				}
			}
		}
	}
	*pResult = 0;
}
