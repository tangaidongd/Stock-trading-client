// dlgarbitragesel.cpp : implementation file
//

#include "stdafx.h"
#include "dlgarbitragesel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgArbitrageSel dialog


CDlgArbitrageSel::CDlgArbitrageSel(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgArbitrageSel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgArbitrageSel)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT	
	m_pArbitrageSel = NULL;
}


void CDlgArbitrageSel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgArbitrageSel)
	DDX_Control(pDX, IDC_LIST_ARBTRAGE_SEL, m_ListArbitrage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgArbitrageSel, CDialogEx)
	//{{AFX_MSG_MAP(CDlgArbitrageSel)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ARBTRAGE_SEL, OnDblclkList)
	ON_COMMAND(IDC_BUTTON_ARBITRADE_DEL, OnClickDel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgArbitrageSel message handlers
BOOL CDlgArbitrageSel::OnInitDialog()
{
	CDialog::OnInitDialog();

	//
	InitialCtrls();
	return CDialogEx::OnInitDialog();
}

void CDlgArbitrageSel::InitialCtrls()
{
	const CArray<CArbitrage, CArbitrage&>& aArbitrages = CArbitrageManage::Instance()->GetArbitrageDataRef();
	const CArbitrage* pData = (CArbitrage*)aArbitrages.GetData();

	CRect rectList;
	m_ListArbitrage.GetClientRect(&rectList);
	m_ListArbitrage.InsertColumn(0, L"套利商品");
	m_ListArbitrage.SetColumnWidth(0, rectList.Width());
	//

	for ( int32 i = 0; i < aArbitrages.GetSize(); i++ )
	{
		//
		int32 iCount = m_ListArbitrage.GetItemCount();
		int32 iIndex = m_ListArbitrage.InsertItem(iCount, (pData + i)->GetShowName());
		m_ListArbitrage.SetItemData(iIndex, DWORD(pData + i));
	}
}

void CDlgArbitrageSel::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult)
{
	OnOK();
	*pResult = 0;
}

void CDlgArbitrageSel::OnOK() 
{
	//
	POSITION pos = m_ListArbitrage.GetFirstSelectedItemPosition();
	while ( NULL != pos )
	{
		int iItem = m_ListArbitrage.GetNextSelectedItem(pos);
		if ( iItem < 0 )
		{
			MessageBox(L"请选择一个套利商品!");
			return;
		}

 		CArbitrage* p = (CArbitrage*)m_ListArbitrage.GetItemData(iItem);
 		if ( NULL == p )
 		{
 			//ASSERT(0);
 			return;
 		}

		m_pArbitrageSel = p;
	}

	//
	CDialogEx::OnOK();
}

void CDlgArbitrageSel::OnClickDel()
{
	POSITION pos = m_ListArbitrage.GetFirstSelectedItemPosition();
	if (NULL == pos)
	{
		return;
	}
	while ( NULL != pos )
	{
		int iItem = m_ListArbitrage.GetNextSelectedItem(pos);
		if ( iItem < 0 )
		{
			MessageBox(L"请选择一个套利商品!");
			return;
		}

		CArbitrage* p = (CArbitrage*)m_ListArbitrage.GetItemData(iItem);
		if ( NULL == p )
		{
			//ASSERT(0);
			return;
		}

		CArbitrageManage::Instance()->DelArbitrage(*p);		
		m_ListArbitrage.DeleteItem(iItem);
	}
}
