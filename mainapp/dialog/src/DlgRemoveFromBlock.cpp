#include "StdAfx.h"

#include "DlgRemoveFromBlock.h"
#include "BlockConfig.h"
#include "merchmanager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRemoveFromBlock dialog

CDlgRemoveFromBlock::CDlgRemoveFromBlock(CWnd* pParent /*=NULL*/)
: CDialogEx(CDlgRemoveFromBlock::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRemoveFromBlock)
	//}}AFX_DATA_INIT

	m_pMerchDel = NULL;
}


void CDlgRemoveFromBlock::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRemoveFromBlock)
	DDX_Control(pDX, IDC_LIST1, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRemoveFromBlock, CDialogEx)
//{{AFX_MSG_MAP(CDlgRemoveFromBlock)
ON_BN_CLICKED(IDC_BUTTON_SEL_ALL, OnBtnCheckAll)
ON_BN_CLICKED(IDC_BUTTON_UNSEL_ALL, OnBtnUnCheckAll)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDlgRemoveFromBlock::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ASSERT( m_pMerchDel != NULL );
	if ( NULL == m_pMerchDel )
	{
		EndDialog(IDCANCEL);
		return TRUE;
	}

	m_List.SetExtendedStyle(m_List.GetExtendedStyle() |LVS_EX_GRIDLINES |LVS_EX_CHECKBOXES |LVS_EX_FULLROWSELECT);

	m_List.InsertColumn(0, _T("板块简称"));
	m_List.InsertColumn(1, _T("板块名称"));
	m_List.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	m_List.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

	if ( FillList() <= 0)
	{
		MessageBox(_T("该商品不属于任何自选板块！"), AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		EndDialog(IDCANCEL);
	}

	CString StrTitle;
	GetWindowText(StrTitle);
	StrTitle += _T(" [") + m_pMerchDel->m_MerchInfo.m_StrMerchCnName + _T("]");
	SetWindowText(StrTitle);

	return TRUE;
}


void CDlgRemoveFromBlock::OnOK()
{
	// 检查一下值
	GetCheckBlocks();
	if ( m_aBlockNames.GetSize() > 0 )
	{
		CDialogEx::OnOK();
	}
	else
	{
		MessageBox(_T("您需要先选择要从哪些板块中移出！"));
	}
}

int CDlgRemoveFromBlock::FillList()
{
	m_List.DeleteAllItems();

	CBlockCollection *pCol = CBlockConfig::Instance()->GetUserBlockCollection();
	if ( NULL == pCol )
	{
		return -1;
	}

	CBlockCollection::BlockArray aBlocks;
	pCol->GetBlockByMerch(m_pMerchDel, aBlocks);
	for ( int32 i=0; i < aBlocks.GetSize() ; i++ )
	{
		int iLine = m_List.InsertItem(i, aBlocks[i]->m_blockInfo.m_StrBlockShortCut, 1); // 0-no check box, 1-uncheck, 2-check
		if ( iLine >= 0 )
		{
			m_List.SetItemText(iLine, 1, aBlocks[i]->m_blockInfo.m_StrBlockName);
		}
	}
	return aBlocks.GetSize();
}

void CDlgRemoveFromBlock::CheckAllBlocks( bool32 bCheck )
{
	const int iCount = m_List.GetItemCount();
	for ( int i=0; i < iCount ; i++ )
	{
		m_List.SetCheck(i, bCheck);
	}
}

void CDlgRemoveFromBlock::GetCheckBlocks()
{
	m_aBlockNames.RemoveAll();

	const int iCount = m_List.GetItemCount();
	for ( int i=0; i < iCount ; i++ )
	{
		if ( m_List.GetCheck(i) )
		{
			m_aBlockNames.Add( m_List.GetItemText(i, 1) );
		}
	}
}

void CDlgRemoveFromBlock::OnBtnCheckAll()
{
	CheckAllBlocks(TRUE);
}

void CDlgRemoveFromBlock::OnBtnUnCheckAll()
{
	CheckAllBlocks(FALSE);
}

bool32 CDlgRemoveFromBlock::RemoveFromBlock( CMerch *pMerchDel, OUT CStringArray &aBlockNames )
{
	if ( NULL == pMerchDel )
	{
		return false;
	}

	CDlgRemoveFromBlock dlg(AfxGetMainWnd());
	dlg.m_pMerchDel = pMerchDel;
	if ( dlg.DoModal() == IDOK )
	{
		aBlockNames.Copy(dlg.m_aBlockNames);
		return true;
	}
	return false;
}


