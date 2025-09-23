// dlgcolorkline.cpp : implementation file
//

#include "stdafx.h"
#include "dlgcolorkline.h"
#include "dlgformularmanager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgColorKLine dialog


CDlgColorKLine::CDlgColorKLine(E_FormularType eType, CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgColorKLine::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgColorKLine)
	//}}AFX_DATA_INIT
	m_pLib = CFormulaLib::instance();
	m_pFormularSelected = NULL;
	m_eType = eType;
}


void CDlgColorKLine::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgColorKLine)
	DDX_Control(pDX, IDC_LIST, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgColorKLine, CDialogEx)
	//{{AFX_MSG_MAP(CDlgColorKLine)
	ON_NOTIFY(NM_CLICK, IDC_LIST, OnClickList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgColorKLine message handlers
BOOL CDlgColorKLine::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_ImageList.Create(IDB_FORMULAR_NEW, 16, 1, RGB(255,255,255));

	//	
	m_List.SetImageList(&m_ImageList, LVSIL_SMALL);
	m_List.SetExtendedStyle(m_List.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	InitialList();

	m_List.SetItemState(0, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);

	//
	return TRUE;
}

void CDlgColorKLine::InitialList()
{
	if ( NULL == m_pLib )
	{
		//ASSERT(0);
		return;
	}

	if ( EFTClrKLine == m_eType )
	{
		// 系统
		CIndexGroupClrKLine* pGroupClrKLine = NULL;
		
		int32 i = 0;
		
		if ( NULL != m_pLib->m_SysIndex.m_pGroupClrKLine )
		{
			pGroupClrKLine = m_pLib->m_SysIndex.m_pGroupClrKLine;
			
			for ( i = 0; i < pGroupClrKLine->m_ContentsClrKLine.GetSize(); i++ )
			{
				CFormularContent* pContent = pGroupClrKLine->m_ContentsClrKLine.GetAt(i);
				if ( NULL == pContent )
				{
					continue;
				}
				
				if ( pContent->m_bProtected )
				{
					m_List.InsertItem(i, pContent->name, KiImageClrKLineSysLock);
				}
				else
				{
					m_List.InsertItem(i, pContent->name, KiImageClrKLineSys);
				}
				
				m_List.SetItemData(i, (DWORD)pContent);
			}
		}
		
		// 用户
		if ( NULL != m_pLib->m_UserIndex.m_pGroupClrKLine )
		{
			pGroupClrKLine = m_pLib->m_UserIndex.m_pGroupClrKLine;
			
			for ( i = 0; i < pGroupClrKLine->m_ContentsClrKLine.GetSize(); i++ )
			{
				CFormularContent* pContent = pGroupClrKLine->m_ContentsClrKLine.GetAt(i);
				if ( NULL == pContent )
				{
					continue;
				}
				
				int32 iCount = m_List.GetItemCount();
				
				int32 iImageIndex = KiImageClrKLineUser;
				if ( pContent->m_bProtected )
				{
					iImageIndex = KiImageClrKLineUserLock;
				}
				
				m_List.InsertItem(iCount, pContent->name, iImageIndex);
				m_List.SetItemData(iCount, (DWORD)pContent);
			}
		}
	}
	else if ( EFTTradeChose == m_eType )
	{
		// 系统
		CIndexGroupTradeChose* pGroupTrade = NULL;
		
		int32 i = 0;
		
		if ( NULL != m_pLib->m_SysIndex.m_pGroupTradeChose )
		{
			pGroupTrade = m_pLib->m_SysIndex.m_pGroupTradeChose;
			
			for ( i = 0; i < pGroupTrade->m_ContentsTrade.GetSize(); i++ )
			{
				CFormularContent* pContent = pGroupTrade->m_ContentsTrade.GetAt(i);
				if ( NULL == pContent )
				{
					continue;
				}
				
				if ( pContent->m_bProtected )
				{
					m_List.InsertItem(i, pContent->name, KiImageIndexTradeChoseSysLock);
				}
				else
				{
					m_List.InsertItem(i, pContent->name, KiImageIndexTradeChoseSys);
				}
				
				m_List.SetItemData(i, (DWORD)pContent);
			}
		}
		
		// 用户
		if ( NULL != m_pLib->m_UserIndex.m_pGroupClrKLine )
		{
			pGroupTrade = m_pLib->m_UserIndex.m_pGroupTradeChose;
			
			for ( i = 0; i < pGroupTrade->m_ContentsTrade.GetSize(); i++ )
			{
				CFormularContent* pContent = pGroupTrade->m_ContentsTrade.GetAt(i);
				if ( NULL == pContent )
				{
					continue;
				}
				
				int32 iCount = m_List.GetItemCount();
				
				int32 iImageIndex = KiImageIndexTradeChoseUser;
				if ( pContent->m_bProtected )
				{
					iImageIndex = KiImageIndexTradeChoseUser;
				}
				
				m_List.InsertItem(iCount, pContent->name, iImageIndex);
				m_List.SetItemData(iCount, (DWORD)pContent);
			}
		}
	}
	
}

void CDlgColorKLine::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int id = m_List.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);	
	
	if ( id >= 0 )
	{
		CFormularContent* pContent = (CFormularContent*)m_List.GetItemData(id);
		if ( NULL != pContent )
		{
			GetDlgItem(IDC_STATIC_PROMPT)->SetWindowText(pContent->help);
			UpdateData(FALSE);
		}
	}
		
	*pResult = 0;
}

CFormularContent* CDlgColorKLine::GetFormular()
{
	return m_pFormularSelected;
}

void CDlgColorKLine::OnOK() 
{
	int id = m_List.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);	
	
	if ( id >= 0 )
	{
		m_pFormularSelected = (CFormularContent*)m_List.GetItemData(id);		
	}

	CDialogEx::OnOK();
}

void CDlgColorKLine::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	OnOK();
	*pResult = 0;
}
