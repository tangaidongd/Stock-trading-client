#include "StdAfx.h"

#include "DlgChooseStockVar.h"
#include "BlockConfig.h"


#include "merchmanager.h"

#include "keyboarddlg.h"
#include "hotkey.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DLGCSV_BLOCK_BMP (2)
#define DLGCSV_MERCH_BMP (4)

/////////////////////////////////////////////////////////////////////////////
// CDlgChooseStockVar dialog

CDlgChooseStockVar::CDlgChooseStockVar(CWnd* pParent /*=NULL*/)
: CDialogEx(CDlgChooseStockVar::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgChooseStockVar)
	//}}AFX_DATA_INIT

	m_bLeftIsMarket = false;
	m_bMultiMerchSel = false;
	m_pInputMerch = NULL;
}


void CDlgChooseStockVar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgChooseStockVar)
	DDX_Control(pDX, IDC_LIST1, m_ListBlock);
	DDX_Control(pDX, IDC_LIST2, m_ListMerch);
	DDX_Control(pDX, IDC_TAB1, m_Tab);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgChooseStockVar, CDialogEx)
//{{AFX_MSG_MAP(CDlgChooseStockVar)
ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnTabChanged)
ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnLeftListSelChanged)
ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST2, OnGetDispInfo)
ON_NOTIFY(NM_DBLCLK, IDC_LIST2, OnRightListLDbClick)
ON_BN_CLICKED(ID_BUTTON_SELALL, OnBtnSelAll)
ON_MESSAGE(UM_HOTKEY, OnMsgHotKey)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDlgChooseStockVar::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_ImageList.Create ( IDB_FORMULA,16,1,RGB(255,255,255) );
	m_ListBlock.SetImageList(&m_ImageList, LVSIL_SMALL);
	m_ListMerch.SetImageList(&m_ImageList, LVSIL_SMALL);

	m_ListBlock.SetExtendedStyle(m_ListBlock.GetExtendedStyle() |LVS_EX_FULLROWSELECT);
	m_ListBlock.InsertColumn(0, _T("板块名称"));
	m_ListBlock.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);

	if ( !m_bMultiMerchSel )
	{
		m_ListMerch.ModifyStyle(0, LVS_SINGLESEL);
		::ShowWindow(GetDlgItem(ID_BUTTON_SELALL)->GetSafeHwnd(), SW_HIDE);
		SetDlgItemText(IDC_STATIC_TIP, _T("仅单选，可使用键盘精灵快速选择"));
	}
	else
	{
		m_ListMerch.ModifyStyle(LVS_SINGLESEL, 0);
		::ShowWindow(GetDlgItem(ID_BUTTON_SELALL)->GetSafeHwnd(), SW_SHOWNA);
		SetDlgItemText(IDC_STATIC_TIP, _T("可多选，使用键盘精灵仅快速选择一个商品"));
	}

	// 风格设置

	// 初始化tab
	m_Tab.InsertItem(0, _T("物理市场"));		// 0特殊处理

	CBlockConfig::IdArray aColIds;
	CBlockConfig::Instance()->GetCollectionIdArray(aColIds);
	for ( int32 i=0; i < aColIds.GetSize() ; i++ )
	{
		const CBlockCollection *pCol = CBlockConfig::Instance()->GetBlockCollectionById(aColIds[i]);
		if ( NULL != pCol )
		{
			m_Tab.InsertItem(TCIF_PARAM |TCIF_TEXT, 1+i, pCol->m_StrName, 0, (LPARAM)aColIds[i]);
		}
	}

	m_Tab.SetCurSel(0);
	if ( m_aInputBelongToBlocks.GetSize() > 0 && NULL != m_pInputMerch )	// 有所属板块
	{
		CString StrTitle;
		GetWindowText(StrTitle);
		CString StrMerch;
		StrMerch.Format(_T(" - [%s %s]"), m_pInputMerch->m_MerchInfo.m_StrMerchCnName.GetBuffer(), m_pInputMerch->m_MerchInfo.m_StrMerchCode.GetBuffer());
		StrTitle += StrMerch;
		SetWindowText(StrTitle);

		m_Tab.InsertItem(m_Tab.GetItemCount(), _T("所属板块"));

		m_Tab.SetCurSel(m_Tab.GetItemCount()-1);	// 选择到所属
	}

	

	FillBlockList();	// 填充第一个
	// 无商品，不需要操作

	return TRUE;
}


void CDlgChooseStockVar::OnOK()
{
	// 检查一下值
	GetSelMerchs();
	if ( m_aMerchSel.GetSize() <= 0 )
	{
		return;
	}

	CDialogEx::OnOK();
}

void CDlgChooseStockVar::FillBlockList()
{
	if(GetSafeHwnd() == NULL) return ;
	CGGTongDoc *pDoc = AfxGetDocument();
	ASSERT( NULL != pDoc );
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return;
	}

	int iSel = m_Tab.GetCurSel();
	if ( iSel == 0 )
	{
		m_bLeftIsMarket = true;

		m_ListBlock.DeleteAllItems();
		// 物理市场
		// 遍历
		int32 iLine = 0;
		for ( int32 iBreed =0; iBreed < pAbsCenterManager->GetMerchManager().m_BreedListPtr.GetSize() ; iBreed++ )
		{
			CBreed *pBreed = pAbsCenterManager->GetMerchManager().m_BreedListPtr[iBreed];
			if ( NULL == pBreed )
			{
				continue;
			}

			for ( int32 iMarket=0; iMarket < pBreed->m_MarketListPtr.GetSize() ; iMarket++ )
			{
				CMarket *pMarket = pBreed->m_MarketListPtr[iMarket];
				if ( NULL == pMarket )
				{
					continue;
				}

				m_ListBlock.InsertItem(iLine, pMarket->m_MarketInfo.m_StrCnName, DLGCSV_BLOCK_BMP);
				m_ListBlock.SetItemData(iLine, (LPARAM)pMarket);
				iLine++;
			}
		}
	}
	else if ( m_aInputBelongToBlocks.GetSize() > 0 && iSel == m_Tab.GetItemCount()-1 )
	{
		// 最后一个是所属板块, 存的是板块id
		m_bLeftIsMarket = false;
		
		m_ListBlock.DeleteAllItems();
		int32 iLine = 0;
		for ( int32 i=0; i < m_aInputBelongToBlocks.GetSize() ; i++ )
		{
			CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(m_aInputBelongToBlocks[i]);
			if ( NULL == pBlock )
			{
				continue;
			}
			
			m_ListBlock.InsertItem(iLine, pBlock->m_blockInfo.m_StrBlockName, DLGCSV_BLOCK_BMP);
			m_ListBlock.SetItemData(iLine, (LPARAM)pBlock->m_blockInfo.m_iBlockId);
			iLine++;
		}
	}
	else if ( iSel > 0 )
	{
		// 板块
		TCITEM tci;
		ZeroMemory(&tci, sizeof(tci));
		tci.mask = TCIF_PARAM;
		if ( m_Tab.GetItem(iSel, &tci) )
		{
			int32 iColId = (int32)tci.lParam;
			CBlockCollection *pCol = CBlockConfig::Instance()->GetBlockCollectionById(iColId);
			if ( NULL != pCol )
			{
				m_bLeftIsMarket = false;
				m_ListBlock.DeleteAllItems();

				CBlockCollection::BlockArray aBlocks;
				pCol->GetValidBlocks(aBlocks);
				int32 iLine = 0;
				for ( int32 i=0; i < aBlocks.GetSize() ; i++ )
				{
					if ( NULL == aBlocks[i] )
					{
						continue;
					}
					
					m_ListBlock.InsertItem(iLine, aBlocks[i]->m_blockInfo.m_StrBlockName, DLGCSV_BLOCK_BMP);
					m_ListBlock.SetItemData(iLine, (LPARAM)aBlocks[i]->m_blockInfo.m_iBlockId);
					iLine++;
				}
			}
			else
			{
				ASSERT( 0 );
			}
		}
		else
		{
			ASSERT( 0 );
		}
	}
	else
	{
		ASSERT( 0 );
	}
	m_ListBlock.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
}

void CDlgChooseStockVar::FillMerchList()
{
	m_ListMerch.DeleteAllItems();
	m_ListMerch.SetItemCount(m_aMerchSrc.GetSize());
	m_ListMerch.Invalidate();
}

void CDlgChooseStockVar::OnGetDispInfo( NMHDR *pHdr, LRESULT *pResult )
{
	NMLVDISPINFO *pdi = (NMLVDISPINFO*)pHdr;
	if ( NULL != pdi )
	{
		const int iIndex = pdi->item.iItem;
		if ( iIndex >= 0 && iIndex < m_aMerchSrc.GetSize() )
		{
			pdi->item.iImage = DLGCSV_MERCH_BMP;
			pdi->item.lParam = (LPARAM)m_aMerchSrc[iIndex];

			if ( (pdi->item.mask &LVIF_TEXT) == LVIF_TEXT
				&& NULL != pdi->item.pszText 
				&& pdi->item.cchTextMax > 0 )
			{
				_tcsncpy(pdi->item.pszText, m_aMerchSrc[iIndex]->m_MerchInfo.m_StrMerchCnName, pdi->item.cchTextMax-1);
				pdi->item.pszText[pdi->item.cchTextMax-1] = _T('\0');
			}
		}
	}
}

void CDlgChooseStockVar::OnTabChanged( NMHDR *pHdr, LRESULT *pResult )
{
	FillBlockList();
	UpdateMerchSrcChange();
}

void CDlgChooseStockVar::ClearMerchSrc()
{
	if ( m_aMerchSrc.GetSize() > 0 )
	{
		m_aMerchSrc.RemoveAll();
		m_ListMerch.DeleteAllItems();
	}
}

void CDlgChooseStockVar::UpdateMerchSrcChange()
{
	int iSel = m_ListBlock.GetNextItem(-1, LVNI_SELECTED);
	if ( iSel >= 0 )
	{
		// 更新商品源
		DWORD dwParam = m_ListBlock.GetItemData(iSel);
		if ( m_bLeftIsMarket )
		{
			// 市场指针
			CMarket *pMarket = (CMarket *)dwParam;
			ASSERT( NULL != pMarket );
			if ( NULL != pMarket )
			{
				m_aMerchSrc.Copy( pMarket->m_MerchsPtr );
				FillMerchList();
			}
		}
		else
		{
			// 板块id
			int32 iId = (int32)dwParam;
			CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(iId);
			ASSERT( NULL != pBlock );
			if ( NULL != pBlock )
			{
				m_aMerchSrc.Copy( pBlock->m_blockInfo.m_aSubMerchs );
				FillMerchList();
			}
		}
	}
	else
	{
		ClearMerchSrc();
	}
}

void CDlgChooseStockVar::OnLeftListSelChanged( NMHDR *pHdr, LRESULT *pResult )
{
	UpdateMerchSrcChange();
}

void CDlgChooseStockVar::OnBtnSelAll()
{
	// 选择所有
	const int32 iCount = m_ListMerch.GetItemCount();
	for ( int i=0; i < iCount ; i++ )
	{
		m_ListMerch.SetItemState(i, LVNI_SELECTED, LVNI_SELECTED);
	}
}

void CDlgChooseStockVar::GetSelMerchs()
{
	// 选择列表框中选择的商品
	m_aMerchSel.RemoveAll();

	POSITION pos = m_ListMerch.GetFirstSelectedItemPosition();
	while ( NULL != pos )
	{
		int iItem = m_ListMerch.GetNextSelectedItem(pos);
		if ( iItem < 0 || iItem >= m_aMerchSrc.GetSize() )
		{
			continue;
		}

		m_aMerchSel.Add( m_aMerchSrc[iItem] );
	}
}

BOOL CDlgChooseStockVar::PreTranslateMessage( MSG* pMsg )
{
	if ( WM_CHAR == pMsg->message )
	{
		// 调出键盘精灵操作
		// 新的处理方法,直接弹出键盘精灵就行了:
		CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
		
		if ( NULL != pMainFrame->m_pDlgKeyBoard )		
		{
			// 关掉
			pMainFrame->m_pDlgKeyBoard->PostMessage(WM_CLOSE,0,0);
		}
		
		ASSERT( NULL == pMainFrame->m_pDlgKeyBoard );
		
		CGGTongApp * pApp = (CGGTongApp *)AfxGetApp();
		CGGTongDoc *pDoc  = pApp->m_pDocument;
		ASSERT(NULL != pDoc);
		
		//pMainFrame->m_pDlgKeyBoard = new CKeyBoardDlg(&pDoc->m_pAbsCenterManager->m_HotKeyList, pMainFrame);	
		pMainFrame->m_pDlgKeyBoard = new CKeyBoardDlg(pMainFrame->m_HotKeyList,pMainFrame,EHKTMerch);	
		ASSERT(NULL != pMainFrame->m_pDlgKeyBoard);
		pMainFrame->m_pDlgKeyBoard->m_Msg = *pMsg;	// 保存该消息
		pMainFrame->m_pDlgKeyBoard->Create(IDD_KEYBOARD,pMainFrame);
		pMainFrame->m_pDlgKeyBoard->SetAutoCloseFlag(true);	// 无论结果怎样，都只有一个商品
		
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

		return TRUE;
	}
	else
	{
		return CDialogEx::PreTranslateMessage(pMsg);
	}
}

LRESULT CDlgChooseStockVar::OnMsgHotKey( WPARAM w, LPARAM l )
{
	CHotKey *pHotKey = (CHotKey *)w;
	ASSERT(NULL != pHotKey);

	// 取消MainFrame中的窗口指针
	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if ( NULL != pMainFrame && pMainFrame->m_pKBParent == this )
	{
		pMainFrame->SetHotkeyTarget(NULL);
	}
	
	if ( EHKTMerch == pHotKey->m_eHotKeyType )
	{
		CString StrHotKey ,StrName, StrMerchCode;
		int32   iMarketId;
		
		StrHotKey		= pHotKey->m_StrKey;
		StrMerchCode	= pHotKey->m_StrParam1;
		StrName			= pHotKey->m_StrSummary;
		iMarketId		= pHotKey->m_iParam1;		
		DEL(pHotKey);
		
		//
		CMerch* pMerch   = NULL;
		CGGTongDoc *pDoc = AfxGetDocument();
		pDoc->m_pAbsCenterManager->GetMerchManager().FindMerch(StrMerchCode, iMarketId, pMerch);
		
		if ( pMerch != NULL )
		{
			m_aMerchSel.RemoveAll();
			m_aMerchSel.Add(pMerch);
			EndDialog(IDOK);	// 结束对话框
		}
	}
	else
	{
		DEL(pHotKey);
	}
	
	
	return 0;
}

bool32 CDlgChooseStockVar::ChooseStockVar( OUT MerchArray &aMerchSel, bool32 bMultiMerchSel /*= true*/, CMerch *pInputMerch/* = NULL*/, CWnd *pWndParent/*=NULL*/ )
{
	aMerchSel.RemoveAll();

	if ( NULL == pWndParent )
	{
		pWndParent = AfxGetMainWnd();
	}
	CDlgChooseStockVar dlg(pWndParent);
	dlg.SetInputMerch(pInputMerch);
	dlg.m_bMultiMerchSel = bMultiMerchSel;
	if ( dlg.DoModal() == IDOK )
	{
		aMerchSel.Copy(dlg.m_aMerchSel);
		ASSERT( aMerchSel.GetSize() > 0 );
		return true;
	}
	return false;
}

void CDlgChooseStockVar::OnRightListLDbClick( NMHDR *pHdr, LRESULT *pResult )
{
	GetSelMerchs();
	if ( m_aMerchSel.GetSize() > 0 )
	{
		EndDialog(IDOK);
	}
}

void CDlgChooseStockVar::SetInputMerch( CMerch *pMerch )
{
	FillBlockList();
	return;

	/*
	m_pInputMerch = pMerch;
	m_aInputBelongToBlocks.RemoveAll();
	if ( NULL != pMerch )
	{
		// 查找该商品的所属板块, 不查找分类板块
		for ( CBlockConfig::BlockMap::const_iterator it=CBlockConfig::Instance()->m_mapBlocks.begin(); it != CBlockConfig::Instance()->m_mapBlocks.end() ; it++ )
		{
			if ( !it->second->IsValidBlock() 
				|| (it->second->m_blockInfo.GetBlockType() != CBlockInfo::typeNormalBlock
				&& it->second->m_blockInfo.GetBlockType() != CBlockInfo::typeUserBlock)  )
			{
				continue;
			}
			
			if ( it->second->FindMerch(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode) != NULL )
			{
				m_aInputBelongToBlocks.Add(it->first);
			}
		}
	}
	*/
}

