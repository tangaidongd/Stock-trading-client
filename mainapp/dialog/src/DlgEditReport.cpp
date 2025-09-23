// DlgEditReport.cpp : implementation file
//

#include "stdafx.h"

#include "DlgEditReport.h"
#include "DlgChooseStockVar.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgEditReport dialog


CDlgEditReport::CDlgEditReport(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgEditReport::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgEditReport)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT	
	m_aMerch.RemoveAll();
}


void CDlgEditReport::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgEditReport)
		// NOTE: the ClassWizard will add DDX and DDV calls here
		DDX_Control(pDX, IDC_LIST_MERCHS, m_ListMerchs);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgEditReport, CDialogEx)
	//{{AFX_MSG_MAP(CDlgEditReport)
		// NOTE: the ClassWizard will add message map macros here
		ON_BN_CLICKED(IDC_BTN_ADD, OnButtonAdd)
		ON_BN_CLICKED(IDC_BTN_DEL, OnButtonDel)
		ON_BN_CLICKED(IDC_BTN_UP, OnButtonUp)
		ON_BN_CLICKED(IDC_BTN_DOWN, OnButtonDown)
		ON_BN_CLICKED(IDC_BTN_CLEAR, OnButtonClear)
		ON_BN_CLICKED(IDC_BTN_OK, OnButtonOK)
		ON_BN_CLICKED(IDC_BTN_CANCEL, OnButtonCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgEditReport message handlers

BOOL CDlgEditReport::OnInitDialog()
{
	CDialog::OnInitDialog();

	CenterWindow();
	m_ListMerchs.SetItemHeight(0, 20);
	InitialListMerchs();
	return TRUE; 
}

void CDlgEditReport::OnButtonAdd() 
{
	// 选择品种
	CDlgChooseStockVar::MerchArray aMerchs;
	if ( CDlgChooseStockVar::ChooseStockVar(aMerchs, TRUE) )
	{
		int32 index = m_aMerch.GetSize();
		bool32 bFlg = TRUE;
 		for ( int32 i = 0; i < aMerchs.GetSize(); i++ )
		{
			for (int32 j=0; j<m_aMerch.GetSize(); j++)
			{
				if (aMerchs[i]==m_aMerch[j])
				{
					bFlg = FALSE;
					break;
				}
			}
			if (bFlg)
			{
				m_aMerch.InsertAt(index++, aMerchs[i]);
			}
			else
			{
				bFlg = TRUE;
			}
 		}

		FillListBox();
		UpdateData(FALSE);
	}
}

void CDlgEditReport::OnButtonDel() 
{
	int32 iSel = m_ListMerchs.GetCurSel();
	int32 iSelCounts = m_ListMerchs.GetSelCount();
	if ( iSelCounts < 1 )
	{
		return;
	}

	INT32 *plselectedItem = new INT32[iSelCounts];
	m_ListMerchs.GetSelItems(iSelCounts, plselectedItem);
	for (int32 i = iSelCounts - 1; i >= 0; i--)
	{	
		m_aMerch.RemoveAt(plselectedItem[i]);
		m_ListMerchs.DeleteString(plselectedItem[i]);			
	}	
	m_ListMerchs.SetSel(iSel-iSelCounts+1);
	delete []plselectedItem;	
}

void CDlgEditReport::OnButtonUp() 
{
	int32 iCurSel = m_ListMerchs.GetCurSel();
	int32 iSelCount = m_ListMerchs.GetSelCount();
	CString StrText;
	if (1 == iSelCount)
	{
		if (iCurSel!=0)
		{		
			m_ListMerchs.GetText(iCurSel,StrText);
			m_ListMerchs.DeleteString(iCurSel);
			m_ListMerchs.InsertString(iCurSel-1,StrText);
			m_ListMerchs.SetSel(iCurSel-1,true);

			CMerch *pMerch =  m_aMerch.GetAt(iCurSel);
			m_aMerch.RemoveAt(iCurSel);
			m_aMerch.InsertAt((iCurSel-1), pMerch);
		}
	}
}

void CDlgEditReport::OnButtonDown() 
{
	int32 iCurSel = m_ListMerchs.GetCurSel();
	int32 iSelCount = m_ListMerchs.GetSelCount();
	CString StrText;
	if (1 == iSelCount)
	{
		if (iCurSel!=(m_ListMerchs.GetCount()-1))
		{		
			m_ListMerchs.GetText(iCurSel,StrText);
			m_ListMerchs.DeleteString(iCurSel);
			m_ListMerchs.InsertString(iCurSel+1,StrText);
			m_ListMerchs.SetSel(iCurSel+1,true);
			
			CMerch *pMerch =  m_aMerch.GetAt(iCurSel);
			m_aMerch.RemoveAt(iCurSel);
			m_aMerch.InsertAt((iCurSel+1), pMerch);
		}
	}
}

void CDlgEditReport::OnButtonClear() 
{
	if ( IDNO == MessageBox(L"确认清空所有热门商品?", AfxGetApp()->m_pszAppName, MB_YESNO))
	{
		return;
	}
	
	m_aMerch.RemoveAll();
	FillListBox();
	
	UpdateData(FALSE);
}

void CDlgEditReport::OnButtonOK() 
{
	SaveListMerchs();
	CDialog::OnOK();
}

void CDlgEditReport::OnButtonCancel() 
{
	CDialog::OnOK();
}

void CDlgEditReport::InitialListMerchs()
{
	int32 iIndex = 1;
	CArray<T_Block, T_Block&> aBlocks;
	CUserBlockManager::Instance()->GetBlocks(aBlocks);
	
	if ( aBlocks.GetSize() <= 0 )
	{
		//ASSERT(0);
		return;
	}
	
	if ( iIndex < 0 || iIndex >= aBlocks.GetSize() )
	{
		//ASSERT(0);
		iIndex = 0;
	}
	
	//
	T_Block Block = aBlocks[iIndex];
	
	for (int32 i=0; i<Block.m_aMerchs.GetSize(); i++)
	{
		m_aMerch.InsertAt(i, Block.m_aMerchs[i]);
	}

	FillListBox();
	UpdateData(FALSE);
}

void CDlgEditReport::SaveListMerchs()
{
	int32 iIndex = 1;
	CArray<T_Block, T_Block&> aBlocks;
	CUserBlockManager::Instance()->GetBlocks(aBlocks);
	
	if ( aBlocks.GetSize() <= 0 )
	{
		//ASSERT(0);
		return;
	}
	
	if ( iIndex < 0 || iIndex >= aBlocks.GetSize() )
	{
		//ASSERT(0);
		iIndex = 0;
	}
	int32 i = 0;
	//
	T_Block Block = aBlocks[iIndex];
	for (i=0; i<Block.m_aMerchs.GetSize(); i++)
	{
		CUserBlockManager::Instance()->DelMerchFromUserBlock(Block.m_aMerchs[i], Block.m_StrName);
	}

	for (i=0; i<m_aMerch.GetSize(); i++)
	{
		CMerch* pMerch = m_aMerch[i];
		if ( NULL == pMerch )
		{
			continue;
		}
		
		CUserBlockManager::Instance()->AddMerchToUserBlock(pMerch, Block.m_StrName);
	}	
}

// 所有商品列表框
void CDlgEditReport::FillListBox()
{
	// 填充为所有商品
	
	// 先清空
	while ( m_ListMerchs.GetCount() > 0 )
	{
		m_ListMerchs.DeleteString(0);
	}	

	for (int32 i=0; i<m_aMerch.GetSize(); i++)
	{
		CMerch* pMerch = m_aMerch[i];
		if ( NULL == pMerch )
		{
			continue;
		}
		
		CString strMerchName = pMerch->m_MerchInfo.m_StrMerchCnName;
		
		//
		m_ListMerchs.InsertString(-1, strMerchName);
	}	
}