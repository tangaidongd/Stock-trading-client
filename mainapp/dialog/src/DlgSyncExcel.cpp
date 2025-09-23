#include "StdAfx.h"
#include "DlgSyncExcel.h"
#include "MerchManager.h"
#include "NewMenu.h"

#include <map>
using std::map;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgSyncExcel::CDlgSyncExcel(CWnd *pParent) : CDialog(CDlgSyncExcel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSyncExcel)
	//}}AFX_DATA_INIT
}

void CDlgSyncExcel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSyncExcel)
	DDX_Control(pDX, IDC_TREE_MERCH, m_CtrlTreeMerch);
	//}}AFX_DATA_MAP
}

BOOL CDlgSyncExcel::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_CtrlTreeMerch.ModifyStyle(0, TVS_HASLINES |TVS_HASBUTTONS |TVS_LINESATROOT);

	if ( m_StrTitle.IsEmpty() )
	{
		GetWindowText(m_StrTitle);
	}
	if ( m_StrTitle.IsEmpty() )
	{
		m_StrTitle = _T("Excel同步商品");
	}

	// 初始化商品
	InitialTree();

	ShowMerchCount();

	CWnd *pWnd = AfxGetMainWnd();
	if ( IsWindow(pWnd->GetSafeHwnd()) )
	{
		CRect rcMain(0,0,0,0);
		pWnd->GetWindowRect(rcMain);
		CRect rcMy(0,0,0,0);
		GetWindowRect(rcMy);
		rcMain.left = rcMain.right - rcMy.Width();
		rcMain.top  = rcMain.bottom - rcMy.Height();
		if ( rcMain.Width() > 0 && rcMain.Height() > 0 )
		{
			MoveWindow(rcMain);
		}
	}
	//
	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgSyncExcel, CDialog)
//{{AFX_MSG_MAP(CDlgSyncExcel)
ON_NOTIFY(NM_RCLICK, IDC_TREE_MERCH, OnTreeMerchRClick)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDlgSyncExcel::SetSyncExcel( IN const CIoViewSyncExcel::BlockArray &aBlocks, OUT CIoViewSyncExcel::BlockArray &aOutBlocks )
{
	//if ( aBlocks.GetSize() > 0 )
	{
		m_aBlocksSync.Copy(aBlocks);
		if ( DoModal() == IDOK && IsBlockChange(m_aBlocksSync, aBlocks) )
		{
			aOutBlocks.Copy(m_aBlocksSync);
			return TRUE;
		}
	}

	return FALSE;
}

void CDlgSyncExcel::InitialTree()
{
	// 清空
	m_CtrlTreeMerch.DeleteAllItems();

	// 板块 - 商品
	for ( int i=0; i < m_aBlocksSync.GetSize() ; i++ )
	{
		HTREEITEM htBlock = NULL;
		const T_Block &block = m_aBlocksSync[i];
		htBlock = m_CtrlTreeMerch.InsertItem(block.m_StrName);
		ASSERT( NULL != htBlock );
		if ( NULL == htBlock )
		{
			continue;
		}
		for ( int iMerch =0; iMerch < block.m_aMerchs.GetSize() && NULL != htBlock ; iMerch++ )
		{
			CMerch *pMerch = block.m_aMerchs[iMerch];
			if ( NULL == pMerch )
			{
				continue;
			}
			HTREEITEM htMerch = m_CtrlTreeMerch.InsertItem(pMerch->m_MerchInfo.m_StrMerchCnName, htBlock);
			if ( NULL != htMerch )
			{
				m_CtrlTreeMerch.SetItemData(htMerch, (DWORD)pMerch);
			}
		}
		m_CtrlTreeMerch.SetItemData(htBlock, NULL);
	}
}

void CDlgSyncExcel::OnTreeMerchRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

	CMenu menu;
	menu.CreatePopupMenu();
	int iCmd = 1;
	menu.AppendMenu(MF_STRING, iCmd++, _T("删除选中商品或板块"));
	CPoint pt(-1,-1);
	GetCursorPos(&pt);
	int iSel = 0;
	iSel = menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_TOPALIGN |TPM_RETURNCMD |TPM_NONOTIFY, pt.x, pt.y, this);
	if ( 1 == iSel )	// 删除
	{
		CPoint ptHT(pt);
		m_CtrlTreeMerch.ScreenToClient(&ptHT);
		HTREEITEM hSelItem = m_CtrlTreeMerch.HitTest(ptHT);
		//HTREEITEM hSelItem = m_CtrlTreeMerch.GetSelectedItem();
		if ( NULL != hSelItem )
		{
			DWORD dwData = m_CtrlTreeMerch.GetItemData(hSelItem);
			if ( 0 == dwData )	// 板块
			{
				CString StrBlock = m_CtrlTreeMerch.GetItemText(hSelItem);
				for ( int i=0; i < m_aBlocksSync.GetSize() ; i++ )
				{
					if ( m_aBlocksSync[i].m_StrName == StrBlock )
					{
						m_aBlocksSync.RemoveAt(i);	// 删除该板块
						break;
					}
				}

				m_CtrlTreeMerch.DeleteItem(hSelItem);
			}
			else // 商品
			{
				CMerch *pMerchDel = (CMerch *)dwData;
				HTREEITEM hBlock = m_CtrlTreeMerch.GetParentItem(hSelItem);
				bool bDelBlock = false;
				if ( NULL != hBlock )
				{
					CString StrBlock = m_CtrlTreeMerch.GetItemText(hBlock);
					int iBlock=0;
					for ( iBlock=0; iBlock < m_aBlocksSync.GetSize() ; iBlock++ )
					{
						if ( m_aBlocksSync[iBlock].m_StrName == StrBlock )
						{
							break;
						}
					}
					if ( iBlock >= m_aBlocksSync.GetSize() )
					{
						ASSERT( 0 );
					}
					else
					{
						for ( int i=0; i < m_aBlocksSync[iBlock].m_aMerchs.GetSize() ; i++ )
						{
							if ( pMerchDel == m_aBlocksSync[iBlock].m_aMerchs[i] )
							{
								m_aBlocksSync[iBlock].m_aMerchs.RemoveAt(i);
								break;
							}
						}

						bDelBlock = m_aBlocksSync[iBlock].m_aMerchs.GetSize() == 0;
						if ( bDelBlock )
						{
							m_aBlocksSync.RemoveAt(iBlock);
						}
					}
				}

				// 总是要删除HTTREEITEM
				m_CtrlTreeMerch.DeleteItem(hSelItem);
				if ( bDelBlock )
				{
					m_CtrlTreeMerch.DeleteItem(hBlock);
				}
			}
			
			ShowMerchCount();
		}
	}
}

void CDlgSyncExcel::ShowMerchCount()
{
	CString StrTitle;
	int iCount = 0;
	for ( int i=0; i < m_aBlocksSync.GetSize() ; i++ )
	{
		iCount += m_aBlocksSync[i].m_aMerchs.GetSize();
	}
	StrTitle.Format(_T("%s(%d-%d)"), m_StrTitle.GetBuffer(), m_aBlocksSync.GetSize(), iCount);
	SetWindowText(StrTitle);
}

BOOL CDlgSyncExcel::IsBlockChange( const BlockArray &aBlock1, const BlockArray &aBlock2 )
{
	if ( aBlock2.GetSize() != aBlock1.GetSize() ) // 假设block组内没有重复的block
	{
		return TRUE;
	}

	for ( int i=0; i < aBlock1.GetSize() ; i++ )
	{
		const T_Block &block1 = aBlock1[i];
		int j=0;
		for ( j=0; j < aBlock2.GetSize() ; j++ )
		{
			const T_Block &b2 = aBlock2[j];
			if ( b2.m_StrName == block1.m_StrName )	// 找到相同名字的block了
			{
				if ( IsMerchArrayChange(b2.m_aMerchs, block1.m_aMerchs) )
				{
					return TRUE;
				}
			}
		}

		if ( j >= aBlock2.GetSize() )
		{
			return TRUE;		// 找不到同名的block
		}
	}

	return FALSE;
}

BOOL CDlgSyncExcel::IsMerchArrayChange( const MerchPtrArray &aMerch1, const MerchPtrArray &aMerch2 )
{
	int i = 0;
	for ( i=0; i < aMerch1.GetSize() ; i++ )
	{
		const CMerch *pMerch1 = aMerch1[i];
		int j=0;
		for ( j=0; j < aMerch2.GetSize() ; j++ )
		{
			const CMerch *pMerch2 = aMerch2[j];
			if ( CompareMerch(pMerch1, pMerch2) )
			{
				break;
			}
		}
		if ( j >= aMerch2.GetSize() )
		{
			return TRUE;
		}
	}

	// merch1有的merch2都有 - 看merch2里面有merch1没的没
	for ( i=0; i < aMerch2.GetSize() ; i++ )
	{
		const CMerch *pMerch2 = aMerch1[i];
		int j=0;
		for ( j=0; j < aMerch1.GetSize() ; j++ )
		{
			const CMerch *pMerch1 = aMerch1[j];
			if ( CompareMerch(pMerch1, pMerch2) )
			{
				break;
			}
		}
		if ( j >= aMerch1.GetSize() )
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

bool CDlgSyncExcel::CompareMerch( const CMerch *pMerch1, const CMerch *pMerch2 )
{
	if ( pMerch1 == pMerch2 )
	{
		return true;
	}

	if ( pMerch1 == NULL || pMerch2 == NULL )
	{
		return false;
	}

	if ( pMerch2->m_Market.m_MarketInfo.m_iMarketId == pMerch1->m_Market.m_MarketInfo.m_iMarketId &&
		pMerch2->m_MerchInfo.m_StrMerchCode == pMerch1->m_MerchInfo.m_StrMerchCode )
	{
		return true;
	}
	return false;
}


//#pragma warning(pop)