// dlgchoosestockblock.cpp : implementation file
//

#include "stdafx.h"
#include "dlgchoosestockblock.h"
#include "DlgBlockSet.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include "BlockManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgChooseStockBlock dialog

CDlgChooseStockBlock::CDlgChooseStockBlock(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgChooseStockBlock::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgChooseStockBlock)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_iUserBlockNums = 0;
}

void CDlgChooseStockBlock::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgChooseStockBlock)
	DDX_Control(pDX, IDC_LIST, m_ListBlock);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgChooseStockBlock, CDialogEx)
	//{{AFX_MSG_MAP(CDlgChooseStockBlock)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_LBN_SELCHANGE(IDC_LIST,OnSelchangeList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgChooseStockBlock message handlers

//////////////////////////////////////////////////////////////////////////
// 系统函数
BOOL CDlgChooseStockBlock::OnInitDialog()
{
	CDialog::OnInitDialog();

	FillListBox();
	OnSelchangeList();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 自定义函数
void CDlgChooseStockBlock::FillListBox()
{
	CArray<T_Block, T_Block&> aBlocks;
	CUserBlockManager::Instance()->GetBlocks(aBlocks);

	for ( int32 i = 0; i < aBlocks.GetSize(); i++ )
	{
		T_Block stBlock = aBlocks.GetAt(i);

		CString StrBlockName = stBlock.m_StrName;
		m_ListBlock.InsertString(-1, StrBlockName);
	}
	
	//
 	if ( m_ListBlock.GetCount() > 0)
 	{
 		m_ListBlock.SetCurSel(0);
 	}
}

//////////////////////////////////////////////////////////////////////////
// 控件函数
void CDlgChooseStockBlock::OnSelchangeList()
{
// 	int32 iCurSel = m_ListBlock.GetCurSel();
// 	
// 	if ( iCurSel >= m_iUserBlockNums )
// 	{
// 		GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(true);
// 	}
// 	else
// 	{
// 		GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(false);
// 	}
	// xl 0708 当只有一个自选股时，不允许删除
	int32 iCount = m_ListBlock.GetCount();
	
	if ( iCount <= 1 )
	{
		GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(FALSE);			
	}
	else
	{
		int32 iCurSel = m_ListBlock.GetCurSel();
		BOOL bEnable = TRUE;
		if ( -1 != iCurSel )
		{
			CString StrBlockDel;
			m_ListBlock.GetText(iCurSel, StrBlockDel);
			
			T_Block *pBlockDel = CUserBlockManager::Instance()->GetBlock(StrBlockDel);
			if ( NULL != pBlockDel && CUserBlockManager::Instance()->IsServerBlock(*pBlockDel) )	// 不允许删除服务器相关板块
			{
				bEnable = FALSE;
			}
		}
		GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(bEnable);			
	}
}

void CDlgChooseStockBlock::OnButtonAdd() 
{
	CDlgBlockSet DlgBlockSet;
	m_BindHotkey.Release();

	if ( IDOK == DlgBlockSet.DoModal() )
	{
		if ( DlgBlockSet.m_StrName.GetLength() > 0 )
		{			
			T_Block stBlockToAdd;
			
			stBlockToAdd.m_StrName		= DlgBlockSet.m_StrName;
			stBlockToAdd.m_StrHotKey	= DlgBlockSet.m_StrHotKey;
			stBlockToAdd.m_clrBlock		= DlgBlockSet.GetColor();
			stBlockToAdd.m_eHeadType	= DlgBlockSet.GetHead();

			CUserBlockManager::Instance()->AddUserBlock(stBlockToAdd);

			//
			m_ListBlock.InsertString(-1, DlgBlockSet.m_StrName);
			OnSelchangeList();
			UpdateData(false);
		}
	}

	m_BindHotkey.AddRef();	
	m_ListBlock.SetCurSel(m_ListBlock.GetCount()-1);
}

void CDlgChooseStockBlock::OnButtonDel() 
{
	int32 iCurSel = m_ListBlock.GetCurSel();
	
	if ( iCurSel < 0 )
	{
		// 只能删除自己加的,不能删以前的.
		return;
	}

	int32 iCount = m_ListBlock.GetCount();
	
	if ( iCount <= 1 )
	{
		GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(FALSE);			
		return;
	}
	

	CString StrBlockDel;
	m_ListBlock.GetText(iCurSel, StrBlockDel);

	T_Block *pBlockDel = CUserBlockManager::Instance()->GetBlock(StrBlockDel);
	if ( NULL != pBlockDel && CUserBlockManager::Instance()->IsServerBlock(*pBlockDel) )	// 不允许删除服务器相关板块
	{
		MessageBox(_T("无法删除服务器同步板块！"), _T("删除板块"), MB_OK);
		return;
	}

	if ( IDNO == MessageBox(L"确定删除板块?", L"删除板块", MB_YESNO | MB_ICONWARNING) )
	{
		return;
	}

	CUserBlockManager::Instance()->DelUserBlock(StrBlockDel);

	m_ListBlock.DeleteString(iCurSel);
	m_ListBlock.SetCurSel(m_ListBlock.GetCount()-1);
	OnSelchangeList();
	
	UpdateData(false);
}

void CDlgChooseStockBlock::OnOK() 
{
	// 得到选中的板块
	int32 iCurSel = m_ListBlock.GetCurSel();
	if ( m_ListBlock.GetCount() > 0 && iCurSel >=0 )
	{		
		CString	StrBlockName;
		m_ListBlock.GetText(iCurSel,StrBlockName);

		m_BlockFinal.m_StrBlockName = StrBlockName;
		m_BlockFinal.m_eType		= T_BlockDesc::EBTUser;		
	}

	CDialog::OnOK();
}

void CDlgChooseStockBlock::OnClose()
{	
	CDialog::OnClose();
}
