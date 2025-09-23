// dlgcustomstock.cpp : implementation file
//

#include "stdafx.h"

#include "dlgcustomstock.h"
#include "DlgChooseStockVar.h"
#include "dlgcustomcondition.h"
#include "DlgAddToBlock.h"
#include "IoViewReport.h"


#include <algorithm>

#include "dlgresponsibility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgCustomStock dialog
CDlgCustomStock::CDlgCustomStock(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgCustomStock::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgCustomStock)
	m_iMerchAll = 0;
	m_iMerchChoose = 0;
	m_iRadioAnd = 0;
	m_iRadioValue = 0;
	//}}AFX_DATA_INIT
	m_bWorking = false;	
	m_pFormulaLib = NULL;
	m_pSelectStockCenter = NULL;
	m_aMerchToChoose.clear();				
	m_aCustomConditions.RemoveAll();
}

CDlgCustomStock::~CDlgCustomStock()
{
	DEL(m_pSelectStockCenter);
}

void CDlgCustomStock::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCustomStock)
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
	DDX_Control(pDX, IDC_LIST_VALUE, m_ListValues);
	DDX_Control(pDX, IDC_LIST_MERCHS, m_ListMerchs);
	DDX_Control(pDX, IDC_LIST_CONDITION, m_ListCondition);
	DDX_Text(pDX, IDC_EDIT_MERCHS_ALL, m_iMerchAll);
	DDX_Text(pDX, IDC_EDIT_MERCHS_CHOOSE, m_iMerchChoose);
	DDX_Radio(pDX, IDC_RADIO_AND, m_iRadioAnd);
	DDX_Radio(pDX, IDC_RADIO_VALUE, m_iRadioValue);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgCustomStock, CDialogEx)
	//{{AFX_MSG_MAP(CDlgCustomStock)
	ON_BN_CLICKED(IDC_RADIO_VALUE, OnRadioValue)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, OnButtonModify)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_MERCH, OnButtonChangeMerch)
	ON_BN_CLICKED(IDC_BUTTON_DEL_ALL_MERCH, OnButtonDelAllMerch)
	ON_BN_CLICKED(IDC_BUTTON_ADD_TO, OnButtonAddTo)
	ON_BN_CLICKED(IDC_BUTTON_DO, OnButtonDo)	
	ON_LBN_DBLCLK(IDC_LIST_CONDITION, OnDblclkListCondition)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_VALUE, OnDblclkListValue)
	ON_MESSAGE(UM_Choose_Stock_Progress, OnMsgProgress)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
BOOL CDlgCustomStock::PreTranslateMessage(MSG* pMsg)
{
	if ( WM_KEYDOWN == pMsg->message )
	{
		if ( VK_RETURN == pMsg->wParam || VK_ESCAPE == pMsg->wParam )
		{			
			if ( StopChooseStock() )
			{
				PostMessage(WM_CLOSE, 0, 0);			
			}
			
			return TRUE;			
		}
	}
	
	return CDialogEx::PreTranslateMessage(pMsg);
}

BOOL CDlgCustomStock::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 默认沪深 A 股商品
	CBlockLikeMarket* p = CBlockConfig::Instance()->GetDefaultMarketClassBlock();
	if ( NULL != p )
	{
		for (int32 i = 0; i < p->m_blockInfo.m_aSubMerchs.GetSize(); i++)
		{
			if ( NULL != p->m_blockInfo.m_aSubMerchs.GetSize() )
			{
				//
				CMerch* pMerch = p->m_blockInfo.m_aSubMerchs.GetAt(i);
				if ( NULL != pMerch )
				{
					m_aMerchToChoose.insert(pMerch);
				}				
			}
		}
	}

	// 图标
	m_ImageList.Create(IDB_FORMULA, 16, 1, RGB(255,255,255));
	m_ListValues.SetImageList(&m_ImageList, LVSIL_SMALL);

	// 纵向滚动条
	DWORD dwStyle = m_ListValues.GetStyle(); 
	dwStyle &= ~0x0001; 
	::SetWindowLong(m_ListValues.m_hWnd, GWL_STYLE, dwStyle);

	// 填充列表框
	FillListValue();
	FillListBox();

	//
	m_pSelectStockCenter = new CSelectStockCenter(m_hWnd);

	//
	m_iMerchAll = m_aMerchToChoose.size();

	//
	UpdateData(FALSE);

	//
	return TRUE;
}

// 填充字段列表控件
void CDlgCustomStock::FillListValue()
{
	UpdateData(TRUE);
	int32 iIndex = m_iRadioValue;

	m_ListValues.DeleteAllItems();

	//
	if ( 0 == iIndex )
	{
		for ( int32 i = 0; i < s_KiCountsValues0; i++ )
		{
			CString StrText = CReportScheme::Instance()->GetReportHeaderCnName(s_KaValues0[i].m_eHeadType);
			int32 iList = m_ListValues.InsertItem(m_ListValues.GetItemCount(), StrText, 4);
			m_ListValues.SetItemData(iList, (DWORD)&s_KaValues0[i]);
		}
	}
	else if ( 1 == iIndex )
	{
		for ( int32 i = 0; i < s_KiCountsValues1; i++ )
		{
			CString StrText = CReportScheme::Instance()->GetReportHeaderCnName(s_KaValues1[i].m_eHeadType);
			int32 iList = m_ListValues.InsertItem(m_ListValues.GetItemCount(), StrText, 4);
			m_ListValues.SetItemData(iList, (DWORD)&s_KaValues1[i]);
		}
	}
	else if ( 2 == iIndex )
	{
		for ( int32 i = 0; i < s_KiCountsValues2; i++ )
		{
			CString StrText = s_KaValues2[i];
			int32 iList = m_ListValues.InsertItem(m_ListValues.GetItemCount(), StrText, 4);
			m_ListValues.SetItemData(iList, (DWORD)(i + 1000));
		}
	}
}

// 所有商品列表框
void CDlgCustomStock::FillListBox()
{
	// 填充为所有商品
	
	// 先清空
	while ( m_ListMerchs.GetCount() > 0 )
	{
		m_ListMerchs.DeleteString(0);
	}
	
	//
	for ( set<CMerch*, MerchCmp>::iterator it = m_aMerchToChoose.begin(); it != m_aMerchToChoose.end(); ++it )
	{
		CMerch* pMerch = *it;
		if ( NULL == pMerch )
		{
			continue;
		}
		
		//
		m_ListMerchs.InsertString(-1, pMerch->m_MerchInfo.m_StrMerchCnName);
	}	
	
	//
	m_iMerchAll = m_aMerchToChoose.size();
}

// 
void CDlgCustomStock::ReSetListBoxCdt()
{
	int32 iCurSel = m_ListCondition.GetCurSel();

	while ( m_ListCondition.GetCount() > 0 )
	{
		m_ListCondition.DeleteString(0);
	}

	if ( m_aCustomConditions.GetSize() <= 0 )
	{
		return;
	}
	
	//
	for ( int32 i = 0; i < m_aCustomConditions.GetSize(); i++ )
	{
		m_ListCondition.InsertString(-1, m_aCustomConditions.GetAt(i).GetString());
		m_ListCondition.SetItemData(i, i);
	}

	//
	if ( iCurSel >= 0 && iCurSel < m_ListCondition.GetCount() )
	{
		m_ListCondition.SetCurSel(iCurSel);
	}
	else
	{
		m_ListCondition.SetCurSel(0);
	}
}

// 清空进度条
void CDlgCustomStock::ReSetProgress()
{
	if ( m_Progress.GetSafeHwnd() )
	{
		m_Progress.SetForeColour(RGB(0, 255, 0));
		m_Progress.SetPos(0);
		m_Progress.SetWindowText(L"");
		
		RedrawWindow();
	}	
}

// 验证参数合法性
bool32 CDlgCustomStock::ValidChooseParams(OUT CString& StrErr)
{
	StrErr.Empty();
	
	// 判断条件
	if ( m_aCustomConditions.GetSize() <= 0 )
	{
		StrErr = L"请设置选股条件.";
        return false;
	}
	
    // 判断选股范围    
    if ( m_aMerchToChoose.empty() )
    {
        StrErr = L"请设置选股商品.";
        return false;
    }

    // 判断选股入板块
	// 该板块可能已经被删除
    if ( m_BlockFinal.m_StrName.IsEmpty()
		|| CUserBlockManager::Instance()->GetBlock(m_BlockFinal.m_StrName) == NULL )
    {
        StrErr = L"请设置选股入板块!";
        return false;
    }

	return true;
}

void CDlgCustomStock::OnRadioValue() 
{
	FillListValue();	
}

void CDlgCustomStock::OnRadio2() 
{
	FillListValue();
}

void CDlgCustomStock::OnRadio3() 
{
	FillListValue();	
}

void CDlgCustomStock::OnButtonAdd() 
{
	int id = m_ListValues.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);	
	if ( -1 == id )
	{
		MessageBox(L"请选择条件", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return;
	}

	CString StrPostName;
	if ( 0 == m_iRadioValue || 1 == m_iRadioValue )
	{
		// 第一个和第二个标签
		T_CustomStockCdt* p = (T_CustomStockCdt*)m_ListValues.GetItemData(id);
		if ( NULL == p )
		{
			return;
		}

		//
		StrPostName = p->m_StrPostName;		
	}
	else if ( 2 == m_iRadioValue )
	{
		StrPostName = L"";
	}
	else 
	{
		//ASSERT(0);
		return;
	}

	//				
	CString StrName = m_ListValues.GetItemText(id, 0);
	
	T_CustomCdtCell stParam;	
	DWORD dwData = m_ListValues.GetItemData(id);

	if ( 1000 == dwData || 1001 == dwData || 1002 == dwData )
	{
		// 自定义的条件
		stParam.m_iIndex = (int32)dwData;
	}
	else
	{
		stParam.m_iIndex = ((T_CustomStockCdt*)dwData)->m_eHeadType;
	}

	CDlgCustomCondition Dlg;
	Dlg.SetParmas(true, stParam, StrName, StrPostName);

	if ( IDOK == Dlg.DoModal() )
	{
		Dlg.GetResult(stParam);
		
		// 插入右边的列表框
		m_aCustomConditions.Add(stParam);
	
		CString StrCdt = stParam.GetString();
		int32 iIndexCdt = m_ListCondition.InsertString(-1, StrCdt);

		m_ListCondition.SetItemData(iIndexCdt, m_aCustomConditions.GetSize() - 1);
	}
}

void CDlgCustomStock::OnButtonModify() 
{
	int32 iCurSel = m_ListCondition.GetCurSel();
	if ( iCurSel < 0 || iCurSel >= m_ListCondition.GetCount() || m_ListCondition.GetCount() != m_aCustomConditions.GetSize() )
	{
		//ASSERT(0);
		return;
	}

	//
	T_CustomCdtCell* pstParam = (T_CustomCdtCell*)m_aCustomConditions.GetData();

	//
	T_CustomCdtCell& stParam = pstParam[iCurSel];

	//
	CDlgCustomCondition Dlg;
	Dlg.SetParmas(false, stParam, stParam.GetName(), stParam.GetPostName());

	if ( IDOK == Dlg.DoModal() )
	{
		Dlg.GetResult(stParam);
	}
	
	//
	ReSetListBoxCdt();
}

void CDlgCustomStock::OnButtonDel() 
{
	int32 iCurSel = m_ListCondition.GetCurSel();
	if ( iCurSel < 0 || iCurSel >= m_ListCondition.GetCount() || m_ListCondition.GetCount() != m_aCustomConditions.GetSize() )
	{
		//ASSERT(0);
		return;
	}

	if ( IDNO == MessageBox(L"确认删除?", AfxGetApp()->m_pszAppName, MB_YESNO) )
	{
		return;
	}
	
	//
	m_aCustomConditions.RemoveAt(iCurSel);

	//
	ReSetListBoxCdt();
}

void CDlgCustomStock::OnButtonChangeMerch() 
{
	// 选择商品
	CDlgChooseStockVar::MerchArray aMerchs;
	if ( CDlgChooseStockVar::ChooseStockVar(aMerchs, true, NULL, this) )
	{
		set<CMerch*, MerchCmp> aMerchNow;
		
		//
		for ( int32 i = 0; i < aMerchs.GetSize(); i++ )
		{
			aMerchNow.insert(aMerchs[i]);
		}
		
		//
		set<CMerch*, MerchCmp> tmp;
		//lint --e{1025}
		set_union(m_aMerchToChoose.begin(), m_aMerchToChoose.end(), aMerchNow.begin(), aMerchNow.end(), inserter(tmp, tmp.end()));
		m_aMerchToChoose = tmp;
	}
	//
	FillListBox();
	UpdateData(FALSE);	
}

void CDlgCustomStock::OnButtonDelAllMerch() 
{
	if ( IDNO == MessageBox(L"确认清空所有商品?", AfxGetApp()->m_pszAppName, MB_YESNO) )
	{
		return;
	}
	
	//
	m_aMerchToChoose.clear();
	FillListBox();
	UpdateData(FALSE);
}

void CDlgCustomStock::OnButtonAddTo() 
{
	T_Block block;
	if ( CDlgAddToBlock::GetUserSelBlock(block) )
	{
		m_BlockFinal = block;		
	}	
}

void CDlgCustomStock::OnDblclkListCondition() 
{
	OnButtonModify();	
}

void CDlgCustomStock::OnDblclkListValue(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	OnButtonAdd();
	*pResult = 0;
}

void CDlgCustomStock::OnButtonDo() 
{
	if ( !m_bWorking )
	{		
		BeginChooseStock();
	}
	else
	{
		StopChooseStock();
	}   
}

bool32 CDlgCustomStock::BeginChooseStock()
{
	UpdateData(TRUE);

	//
	CString StrErr;
	if ( !ValidChooseParams(StrErr) )
	{
		MessageBox(StrErr, AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return false;
	}
	
	//
	if ( NULL != m_pSelectStockCenter )
	{
		//
		m_bWorking = true;
		
		// 
		GetDlgItem(IDC_BUTTON_DO)->SetWindowText(L"停止选股");
		
		//
		m_Progress.SetRange(0, m_aMerchToChoose.size());
		m_Progress.SetPos(0);
		
		T_CustomSelectParam stParam;
				
		stParam.m_bAndCondition = (m_iRadioAnd == 0);
		stParam.m_aMerchs = m_aMerchToChoose;
		stParam.m_aCondtionCells.Copy(m_aCustomConditions);
		
		//
		m_pSelectStockCenter->SetCustomChooseParam(stParam);
		m_pSelectStockCenter->StartWork();
	}
	
    return true;
}

bool32 CDlgCustomStock::StopChooseStock()
{
	if ( !m_bWorking )
	{
		return true;
	}
	
	//
	if ( IDNO == MessageBox(L"是否停止选股?", AfxGetApp()->m_pszAppName, MB_YESNO) )
	{
		return false;
	}
	
	//
	if ( NULL != m_pSelectStockCenter )
	{
		m_pSelectStockCenter->StopWork();
	}
	
	return true;
}

void CDlgCustomStock::OnFinishChooseStock()
{
	if ( NULL == m_pSelectStockCenter )
	{
		return;
	}
	
	//
	m_bWorking = false;
	
	//
	set<CMerch*, MerchCmp> aResults;
	m_pSelectStockCenter->GetSelectResult(aResults);
	
	// 
	GetDlgItem(IDC_BUTTON_DO)->SetWindowText(L"执行选股");
	
	//
	m_iMerchChoose = aResults.size();
	UpdateData(FALSE);
	
	//	
	T_BlockDesc stBlockDesc;
	stBlockDesc.m_iMarketId = -1;
	stBlockDesc.m_eType = T_BlockDesc::EBTUser;
	stBlockDesc.m_StrBlockName = m_BlockFinal.m_StrName;
	
	// 更新这个板块的商品
	CArray<CMerch*, CMerch*> aMerchs;
	for ( set<CMerch*, MerchCmp>::iterator it = aResults.begin(); it != aResults.end(); ++it )
	{
		CMerch* pMerchNow = *it;
		if ( NULL != pMerchNow )
		{
			TRACE(L"选股结果: %d %s \n", pMerchNow->m_MerchInfo.m_iMarketId, pMerchNow->m_MerchInfo.m_StrMerchCode.GetBuffer());
			aMerchs.Add(pMerchNow);
		}	
	}
	
	// 更新这个自选股的信息
	CUserBlockManager::Instance()->ReplaceUserBlockMerchs(aMerchs, stBlockDesc.m_StrBlockName, false);
	
	// 打开这个板块
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if ( NULL != pMainFrame )
	{
		pMainFrame->OnSpecifyBlock(stBlockDesc, false);
	}	
}

LRESULT CDlgCustomStock::OnMsgProgress(WPARAM wParam , LPARAM lParam)
{
	CString* pStrMsg = (CString*)wParam;
	CString StrMsg = *pStrMsg;
	DEL(pStrMsg);
	
	//
	if ( 0 == int32(lParam) )
	{
		m_Progress.StepIt();
		
		CString StrPos = m_Progress.GetPosText();
		CString StrShow;
		StrShow.Format(L"%s [%s]", StrMsg.GetBuffer(), StrPos.GetBuffer());
		m_Progress.SetWindowText(StrShow);	
	}
	else if ( 1 == int32(lParam) )
	{
		ReSetProgress();
		m_Progress.SetWindowText(StrMsg);
		
		OnFinishChooseStock();
	}
	
	return 1;
}

void CDlgCustomStock::OnClose()
{
	StopChooseStock();
	//
	CDialogEx::OnClose();
}
