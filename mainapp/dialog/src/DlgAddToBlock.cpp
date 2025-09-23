#include "StdAfx.h"

#include "DlgAddToBlock.h"
#include "DlgBlockSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAddToBlock dialog

CDlgAddToBlock::CDlgAddToBlock(CWnd* pParent /*=NULL*/)
: CDialogEx(CDlgAddToBlock::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAddToBlock)
	//}}AFX_DATA_INIT
}


void CDlgAddToBlock::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAddToBlock)
	DDX_Control(pDX, IDC_LIST1, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAddToBlock, CDialogEx)
//{{AFX_MSG_MAP(CDlgAddToBlock)
ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnListDBLClick)
ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnListSelChange)
ON_BN_CLICKED(IDC_BUTTON_BLOCKNEW, OnBtnBlockNew)
ON_BN_CLICKED(IDC_BUTTON_BLOCKMODIFY, OnBtnBlockModify)
ON_BN_CLICKED(IDC_BUTTON_BLOCKDEL, OnBtnBlockDel)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDlgAddToBlock::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	m_ImageList.Create ( IDB_FORMULA,16,1,RGB(255,255,255) );
	m_List.SetImageList(&m_ImageList, LVSIL_SMALL);

	FillList();
	UpdateBtnStatus();

	return TRUE;
}


void CDlgAddToBlock::OnOK()
{
	// 检查一下值
	int iSel = GetCurSelBlock(&m_block);
	if ( iSel >= 0 )
	{
		CDialogEx::OnOK();	// 退出
	}
}

void CDlgAddToBlock::EnableDlgItem( UINT uDlgItemId, BOOL bEnable )
{
	::EnableWindow(GetDlgItem(uDlgItemId)->GetSafeHwnd(), bEnable);
}

int CDlgAddToBlock::GetCurSelBlock(OUT T_Block *pBlock/* = NULL*/)
{
	int iSel = m_List.GetNextItem(-1, LVNI_SELECTED);
	if ( iSel >= 0 && NULL != pBlock )
	{
		CString StrBlock = m_List.GetItemText(iSel, 0);
		T_Block *pBlockUser = CUserBlockManager::Instance()->GetBlock(StrBlock);
		if ( NULL != pBlockUser )
		{
			*pBlock = *pBlockUser;
		}
	}
	return iSel;
}

void CDlgAddToBlock::UpdateBtnStatus()
{
	T_Block block;
	int iSel = GetCurSelBlock(&block);
	bool32 bEnable = iSel >=0;
	EnableDlgItem(IDC_BUTTON_BLOCKMODIFY, bEnable);
	EnableDlgItem(IDC_BUTTON_BLOCKDEL, bEnable && !block.m_bServerBlock && m_List.GetItemCount() > 1);

	EnableDlgItem(IDOK, bEnable);
}

void CDlgAddToBlock::FillList()
{
	T_Block block;
	int iSelOld = GetCurSelBlock(&block);
	bool32 bSelOld = false;

	m_List.DeleteAllItems();
	CArray<T_Block, T_Block &> aBlocks;
	CUserBlockManager::Instance()->GetBlocks(aBlocks);
	for ( int32 i=0; i < aBlocks.GetSize() ; i++ )
	{
		m_List.InsertItem(i, aBlocks[i].m_StrName, 2);
		if ( !bSelOld && iSelOld >= 0 && block.m_StrName == aBlocks[i].m_StrName )
		{
			m_List.SetItemState(i, LVNI_SELECTED, LVNI_SELECTED);
			iSelOld = i;
			bSelOld = true;
		}
	}

	if ( iSelOld >=0 && !bSelOld && iSelOld < m_List.GetItemCount() )
	{
		m_List.SetItemState(iSelOld, LVNI_SELECTED, LVNI_SELECTED);
	}
	else if ( !bSelOld )
	{
		m_List.SetItemState(0, LVNI_SELECTED, LVNI_SELECTED);
		iSelOld = 0;
	}

	m_List.EnsureVisible(iSelOld, FALSE);
}

void CDlgAddToBlock::OnListDBLClick( NMHDR *pNMHDR, LRESULT *pResult )
{
	int iSel = GetCurSelBlock(&m_block);
	if ( iSel >= 0 )
	{
		CDialogEx::OnOK();	// 退出
	}
}

void CDlgAddToBlock::OnListSelChange( NMHDR *pNMHDR, LRESULT *pResult )
{
	UpdateBtnStatus();
}

void CDlgAddToBlock::OnBtnBlockNew()
{
	CDlgBlockSet DlgBlockSet;
	DlgBlockSet.SetModifyFlag(false);
	
	if ( IDOK == DlgBlockSet.DoModal() )
	{
		T_Block BlockToAdd;
		BlockToAdd.m_StrName	= DlgBlockSet.m_StrName;
		BlockToAdd.m_StrHotKey	= DlgBlockSet.m_StrHotKey;
		BlockToAdd.m_clrBlock	= DlgBlockSet.GetColor();
		BlockToAdd.m_eHeadType	= DlgBlockSet.GetHead();
		
		CUserBlockManager::Instance()->AddUserBlock(BlockToAdd);
		FillList();		// 刷新显示
		SelectBlock(BlockToAdd.m_StrName);
		UpdateBtnStatus();
	}
}

void CDlgAddToBlock::OnBtnBlockModify()
{
	T_Block block;
	int iSel = GetCurSelBlock(&block);
	if ( iSel < 0 )
	{
		return;
	}

	CDlgBlockSet DlgBlockSet;
	DlgBlockSet.m_StrName	= block.m_StrName;
	DlgBlockSet.m_StrHotKey = block.m_StrHotKey;
	DlgBlockSet.SetColor(block.m_clrBlock);
	DlgBlockSet.SetHead(block.m_eHeadType);
	
	DlgBlockSet.SetModifyFlag(true);
	
	if ( IDOK == DlgBlockSet.DoModal() )
	{
		T_Block BlockToAdd;
		BlockToAdd.m_StrName	= DlgBlockSet.m_StrName;
		BlockToAdd.m_StrHotKey	= DlgBlockSet.m_StrHotKey;
		BlockToAdd.m_clrBlock	= DlgBlockSet.GetColor();
		BlockToAdd.m_eHeadType	= DlgBlockSet.GetHead();
		
		CUserBlockManager::Instance()->ModifyUserBlock(block.m_StrName, BlockToAdd);
		if ( block.m_StrName != BlockToAdd.m_StrName )
		{
			FillList();		// 修改名称，刷新显示
		}
	}
}

void CDlgAddToBlock::OnBtnBlockDel()
{
	T_Block block;
	int iSel = GetCurSelBlock(&block);
	if ( iSel < 0 )
	{
		return;
	}

	CString StrPrompt;
	StrPrompt.Format(_T("您确认删除板块[%s]？"), block.m_StrName.GetBuffer());
	if ( AfxMessageBox(StrPrompt, MB_YESNO) != IDYES )
	{
		return;
	}

	CUserBlockManager::Instance()->DelUserBlock(block.m_StrName);
	m_List.DeleteItem(iSel);
}

void CDlgAddToBlock::SelectBlock( const CString &StrBlockName )
{
	LVFINDINFO lvfi;
	ZeroMemory(&lvfi, sizeof(lvfi));
	lvfi.flags = LVFI_STRING;
	lvfi.psz   = StrBlockName;
	int iIndex = m_List.FindItem(&lvfi, -1);
	if ( iIndex >= 0 )
	{
		m_List.SetItemState(iIndex, LVNI_SELECTED, LVNI_SELECTED);
		m_List.EnsureVisible(iIndex, FALSE);
	}
}

bool32 CDlgAddToBlock::GetUserSelBlock( OUT T_Block &block )
{
	CDlgAddToBlock dlg/*(AfxGetMainWnd())*/;
	if ( dlg.DoModal() == IDOK )
	{
		block = dlg.m_block;
		return true;
	}
	return false;
}

