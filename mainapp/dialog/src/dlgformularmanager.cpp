// dlgformularmanager.cpp : implementation file
//

#include "stdafx.h"
#include "dlgformularmanager.h"
#include "UserBlockManager.h"
#include "CTaiKlineDlgEditScreenStock.h"
#include "DlgFormularPwd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CDlgFormularManager dialog

// 技术指标
static const int KiImageIndexGroup1		= 0;
static const int KiImageIndexGroup2		= 1;
static const int KiImageIndexSys		= 2;
static const int KiImageIndexUser		= 3;
static const int KiImageIndexSysLock	= 4;
static const int KiImageIndexUserLock	= 5;

// 五彩 K 线
static const int KiImageClrKLineGroup1	= 6;
const int KiImageClrKLineSys			= 7;
const int KiImageClrKLineUser			= 8;
const int KiImageClrKLineSysLock		= 9;
const int KiImageClrKLineUserLock		= 10;

// 条件选股
static const int KiImageIndexCdtChoseGroup1		= 11;
static const int KiImageIndexCdtChoseGroup2		= 12;
static const int KiImageIndexCdtChoseSys		= 13;
static const int KiImageIndexCdtChoseUser		= 14;
static const int KiImageIndexCdtChoseSysLock	= 15;
static const int KiImageIndexCdtChoseUserLock	= 16;

// 交易系统
const int KiImageIndexTradeChoseGroup1			= 17;
const int KiImageIndexTradeChoseSys				= 18;
const int KiImageIndexTradeChoseUser			= 19;
const int KiImageIndexTradeChoseSysLock			= 20;
const int KiImageIndexTradeChoseUserLock		= 21;

// 菜单 ID
static const int KiMenuIdNormal			= 123;
static const int KiMenuIdCdtChose		= 124;
static const int KiMenuIdTradeChose		= 125;
static const int KiMenuIdColorKline		= 126;

CDlgFormularManager::CDlgFormularManager(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgFormularManager::IDD, pParent)
{
	m_iCurSel	  = 0;	
	m_iSortCol	  = 0;
	m_iSortDir	  = 0;
	m_pLib		  = CFormulaLib::instance();

	for ( int32 i = 0; i < FomularPageNums; i++ )
	{
		m_aStrSelectName[i] = L""; 
	}
	
	//
	if ( NULL == m_pLib )
	{
		//ASSERT(0);
	}

	m_bChooseFormular = false;
	m_pFormularChoose = NULL;
}

CDlgFormularManager::~CDlgFormularManager()
{
	DEL(m_pFormularChoose);
	// m_pLib在MainFrame里统一释放了
	//lint --e{1540}
}

void CDlgFormularManager::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFormularManager)
	DDX_Control(pDX, IDC_TREE_SYS, m_TreeSys);
	DDX_Control(pDX, IDC_TREE_GROUP, m_TreeGroup);
	DDX_Control(pDX, IDC_LIST_USER, m_ListUser);
	DDX_Control(pDX, IDC_LIST_ALL, m_ListAll);	
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgFormularManager, CDialogEx)
	//{{AFX_MSG_MAP(CDlgFormularManager)
	ON_BN_CLICKED(IDC_BUTTON_OFTEN, OnButtonOften)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, OnButtonModify)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnButtonNew)
	ON_BN_CLICKED(IDC_RADIO_ALL, OnRadioAll)
	ON_BN_CLICKED(IDC_RADIO_GROUP, OnRadioGroup)
	ON_BN_CLICKED(IDC_RADIO_SYS, OnRadioSys)
	ON_BN_CLICKED(IDC_RADIO_USER, OnRadioUser)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_GROUP, OnSelchangedTreeGroup)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SYS, OnSelchangedTreeSys)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_ALL, OnColumnclickListAll)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_USER, OnColumnclickListUser)
	ON_NOTIFY(NM_CLICK, IDC_LIST_ALL, OnClickListAll)
	ON_NOTIFY(NM_CLICK, IDC_LIST_USER, OnClickListUser)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ALL, OnDblclkListAll)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_USER, OnDblclkListUser)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_GROUP, OnDblclkTreeGroup)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_SYS, OnDblclkTreeSys)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ALL, OnListSelChanged)
	ON_COMMAND_RANGE(KiMenuIdNormal, KiMenuIdColorKline, OnMenu)
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BUTTON_USER_IMPORT, OnBnClickedButtonUserImport)
    ON_BN_CLICKED(IDC_BUTTON_USER_EXPORT, OnBnClickedButtonUserExport)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFormularManager message handlers

BOOL CDlgFormularManager::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_ImageList.Create (IDB_FORMULAR_NEW, 16, 1, RGB(255,255,255));
	
	m_TreeGroup.SetImageList(&m_ImageList, TVSIL_NORMAL);
	m_TreeSys.SetImageList(&m_ImageList, TVSIL_NORMAL);

	m_ListAll.SetImageList(&m_ImageList, LVSIL_SMALL);
	m_ListAll.SetExtendedStyle(m_ListAll.GetExtendedStyle()|LVS_EX_FULLROWSELECT);  

	m_ListUser.SetImageList(&m_ImageList, LVSIL_SMALL);
	m_ListUser.SetExtendedStyle(m_ListUser.GetExtendedStyle()|LVS_EX_FULLROWSELECT);  

	m_ListAll.InsertColumn(0, L"名称", LVCFMT_LEFT, 135);
	m_ListAll.InsertColumn(1, L"描述", LVCFMT_LEFT, 200);

	m_ListUser.InsertColumn(0, L"名称", LVCFMT_LEFT, 135);
	m_ListUser.InsertColumn(1, L"描述", LVCFMT_LEFT, 200);

	BuildListAll();	
	if (!m_bChooseFormular)
	{
		BuildTreeGroup();
		BuildTreeSys();
		BuildListUser();
	}

	m_iCurSel = 0;
	UpdateControls();

	((CButton*)GetDlgItem(IDC_RADIO_ALL))->SetCheck(1);

	//
	if (m_bChooseFormular)
	{
		GetDlgItem(IDC_BUTTON_NEW)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_MODIFY)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_DEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_OFTEN)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_BUTTON_USER_EXPORT)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_BUTTON_USER_IMPORT)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_RADIO_GROUP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_SYS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_USER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_ALL)->ShowWindow(SW_HIDE);

		CRect rtList, rtWnd;
		m_ListAll.GetWindowRect(rtList);
		GetClientRect(rtWnd);
		rtList.top = 10;
		rtList.bottom = rtWnd.bottom - 10;
		m_ListAll.MoveWindow(rtList);

		SetWindowText(_T("请选择主图指标"));
	}


	SetNeedChangeColor(true);
	SetCaptionBKColor(RGB(250, 250, 246));
	SetFrameColor(RGB(102, 102, 102));
	SetCaptionColor(RGB(10, 0, 4));

	return TRUE;
}

void CDlgFormularManager::UpdateButton(CTreeCtrl* pTree, HTREEITEM hItem)
{
	if ( NULL == pTree )
	{
		return;
	}

	CWnd* pDel = GetDlgItem(IDC_BUTTON_DEL);
	CWnd* pChg = GetDlgItem(IDC_BUTTON_MODIFY);	
	CWnd* pOft = GetDlgItem(IDC_BUTTON_OFTEN);
    CWnd* pExport = GetDlgItem(IDC_BUTTON_USER_EXPORT);
	
	CFormularContent* pContent = (CFormularContent*)pTree->GetItemData(hItem);
	if ( NULL == pContent )
	{
		pDel->EnableWindow(FALSE);
		pChg->EnableWindow(FALSE);	
		pOft->EnableWindow(FALSE);
        pExport->EnableWindow(FALSE);

		return;
	}

	
	if ( 1 == (DWORD)pContent || 2 == (DWORD)pContent )
	{
		pDel->EnableWindow(FALSE);
		pChg->EnableWindow(FALSE);	
		pOft->EnableWindow(FALSE);
        pExport->EnableWindow(FALSE);

		return;
	}
	
	pDel->EnableWindow();
	pDel->SetWindowText(_T("删除"));
	pChg->EnableWindow();	
	
	if ( pContent->m_bSystem )
	{
		if ( pContent->m_bCanRestore )
		{
			pDel->SetWindowText(_T("恢复"));
		}
		else
		{
			pDel->EnableWindow(FALSE);
		}
	}

	if ( EFTNormal == pContent->m_eFormularType )
	{
		pOft->EnableWindow();
	}
	else
	{
		pOft->EnableWindow(FALSE);
	}

    SetExportButtonStatus(pContent);

	UpdateData(FALSE);
}

void CDlgFormularManager::UpdateButton(CListCtrl* pList, int32 iItem)
{
	if ( NULL == pList )
	{
		return;
	}

	CWnd* pDel = GetDlgItem(IDC_BUTTON_DEL);
	CWnd* pChg = GetDlgItem(IDC_BUTTON_MODIFY);	
	CWnd* pOft = GetDlgItem(IDC_BUTTON_OFTEN);
    CWnd* pExport = GetDlgItem(IDC_BUTTON_USER_EXPORT);

	if ( iItem < 0 || iItem >= pList->GetItemCount() )
	{
		pDel->EnableWindow(FALSE);
		pChg->EnableWindow(FALSE);	
		pOft->EnableWindow(FALSE);
        pExport->EnableWindow(FALSE);

		return;
	}

	CFormularContent* pContent = (CFormularContent*)pList->GetItemData(iItem);
	if ( NULL == pContent )
	{
		pDel->EnableWindow(FALSE);
		pChg->EnableWindow(FALSE);	
		pOft->EnableWindow(FALSE);
        pExport->EnableWindow(FALSE);

		return;
	}

	
	
	pDel->EnableWindow();
	pDel->SetWindowText(_T("删除"));
	pChg->EnableWindow();	

	if ( pContent->m_bSystem )
	{
		if ( pContent->m_bCanRestore )
		{
			pDel->SetWindowText(_T("恢复"));
		}
		else
		{
			pDel->EnableWindow(FALSE);
		}
	}

	if ( EFTNormal == pContent->m_eFormularType )
	{
		pOft->EnableWindow();
	}
	else 
	{
		pOft->EnableWindow(FALSE);
	}

    SetExportButtonStatus(pContent);

	UpdateData(FALSE);
}

void CDlgFormularManager::UpdateControls()
{
	if ( 0 == m_iCurSel )
	{
		m_ListAll.ShowWindow(SW_SHOW);
		m_TreeGroup.ShowWindow(SW_HIDE);
		m_TreeSys.ShowWindow(SW_HIDE);
		m_ListUser.ShowWindow(SW_HIDE);

		m_ListAll.SetFocus();
	}
	else if ( 1 == m_iCurSel )
	{
		m_ListAll.ShowWindow(SW_HIDE);
		m_TreeGroup.ShowWindow(SW_SHOW);
		m_TreeSys.ShowWindow(SW_HIDE);
		m_ListUser.ShowWindow(SW_HIDE);

		m_TreeGroup.SetFocus();
	}
	else if ( 2 == m_iCurSel )
	{
		m_ListAll.ShowWindow(SW_HIDE);
		m_TreeGroup.ShowWindow(SW_HIDE);
		m_TreeSys.ShowWindow(SW_SHOW);
		m_ListUser.ShowWindow(SW_HIDE);

		m_TreeSys.SetFocus();
	}
	else if ( 3 == m_iCurSel )
	{
		m_ListAll.ShowWindow(SW_HIDE);
		m_TreeGroup.ShowWindow(SW_HIDE);
		m_TreeSys.ShowWindow(SW_HIDE);
		m_ListUser.ShowWindow(SW_SHOW);

		m_ListUser.SetFocus();
	}

	UpdateData(FALSE);
}

bool32 CDlgFormularManager::SameCutName(CString Str1, CString Str2)
{
	Str1.TrimRight(_T("[常用]"));
	Str2.TrimRight(_T("[常用]"));
	Str1.TrimRight(_T("[已修改]"));
	Str2.TrimRight(_T("[已修改]"));
	
	if ( Str1 == Str2 )
	{
		return true;
	}
	
	return false;
}

CFormularContent* CDlgFormularManager::GetCurrentSelectedItemData()
{
	if ( m_iCurSel < 0 || m_iCurSel >= 4 )
	{
		//ASSERT(0);
		return NULL;
	}

	CFormularContent* pRel = NULL;
	CListCtrl* pList = NULL;
	
	if ( 0 == m_iCurSel )
	{
		// 所有
		pList = &m_ListAll;
	}
	else if ( 3 == m_iCurSel )
	{
		// 用户
		pList = &m_ListUser;
	}

	if ( NULL != pList )
	{
		int id = pList->GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);	
		if ( -1 == id )
		{
			return NULL;
		}

		pRel = (CFormularContent*)pList->GetItemData(id);

		return pRel;
	}

	//
	CTreeCtrl* pTree = NULL;
	if ( 1 == m_iCurSel )
	{
		// 分组
		pTree = &m_TreeGroup;
	}
	else if ( 2 == m_iCurSel )
	{
		// 系统
		pTree = &m_TreeSys;
	}

	if ( NULL != pTree )
	{
		HTREEITEM hItem = pTree->GetSelectedItem();
		pRel = (CFormularContent*)pTree->GetItemData(hItem);

		return pRel;
	}

	return pRel;
}

void CDlgFormularManager::UpdateSelectedName(const CString& StrSelectedItem, int32 iIndex)
{
	if ( StrSelectedItem.GetLength() <= 0 ) 
	{
		return;
	}
	
	if ( -1 == iIndex )
	{
		iIndex = m_iCurSel;
	}
	
	if ( iIndex < 0 || iIndex >= FomularPageNums )
	{
		return;
	}

	// 更新	
	m_aStrSelectName[iIndex] = StrSelectedItem;
}

//
void CDlgFormularManager::ReBuildAll()
{
	BuildListAll();
	BuildTreeGroup();
	BuildTreeSys();
	BuildListUser();

	if ( 0 == m_iCurSel )
	{
		m_ListAll.SetFocus();
	}
	else if ( 1 == m_iCurSel )
	{
		m_TreeGroup.SetFocus();
	}
	else if ( 2 == m_iCurSel )
	{
		m_TreeSys.SetFocus();
	}
	else if ( 3 == m_iCurSel )
	{
		m_ListUser.SetFocus();
	}

	
}

//
void CDlgFormularManager::BuildListAll()
{
	m_ListAll.DeleteAllItems();

	//
	int32 iCurId = -1;

	//
	InsertToListByType(false, EFTNormal, iCurId);
	InsertToListByType(false, EFTCdtChose, iCurId);
	InsertToListByType(false, EFTTradeChose, iCurId);
	InsertToListByType(false, EFTClrKLine, iCurId);

	//////////////////////////////////////////////////////////////////////////
	if ( iCurId < 0 )
	{
		return;

		/*
		iCurId = 0;
		CString StrText = m_ListAll.GetItemText(0, 0);
		UpdateSelectedName(StrText, 0);
		*/
	}

	UINT n = m_ListAll.GetItemState(iCurId, (UINT)-1);
	m_ListAll.SetItemState(iCurId, n | LVIS_SELECTED | LVIS_FOCUSED, (UINT)-1);
	m_ListAll.EnsureVisible(iCurId, FALSE);	

	if ( 0 == m_iCurSel )
	{
		UpdateButton(&m_ListAll, iCurId);
	}	
}


void CDlgFormularManager::BuildListUser()
{
	m_ListUser.DeleteAllItems();
	
	int32 iCurId = -1;
	
	//
	InsertToListByType(true, EFTNormal, iCurId);
	InsertToListByType(true, EFTCdtChose, iCurId);
	InsertToListByType(true, EFTTradeChose, iCurId);
	InsertToListByType(true, EFTClrKLine, iCurId);
	
	//
	if ( m_ListUser.GetItemCount() <= 0 )
	{
		return;
	}
	
	if ( iCurId < 0 )
	{
		iCurId = 0;
		CString StrText = m_ListUser.GetItemText(0, 0);
		UpdateSelectedName(StrText, 3);
	}
	
	UINT n = m_ListUser.GetItemState(iCurId, (UINT)-1);
	m_ListUser.SetItemState(iCurId, n | LVIS_SELECTED | LVIS_FOCUSED, (UINT)-1);
	m_ListUser.EnsureVisible(iCurId, FALSE);
	
	//
	if ( 3 == m_iCurSel )
	{
		UpdateButton(&m_ListUser, iCurId);
	}	
}

void CDlgFormularManager::BuildTreeGroup()
{
	m_TreeGroup.DeleteAllItems();

	HTREEITEM hSelected = NULL;

	InsertToTreeByType(false, EFTNormal, hSelected);
	InsertToTreeByType(false, EFTCdtChose, hSelected);
	InsertToTreeByType(false, EFTTradeChose, hSelected);
	InsertToTreeByType(false, EFTClrKLine, hSelected);

	if ( NULL != hSelected )
	{		
		m_TreeGroup.SelectItem(hSelected);
		
		if ( 1 == m_iCurSel )
		{
			UpdateButton(&m_TreeGroup, hSelected);
		}		
	}
}

void CDlgFormularManager::BuildTreeSys()
{
	m_TreeSys.DeleteAllItems();

	HTREEITEM hSelected = NULL;

	InsertToTreeByType(true, EFTNormal, hSelected);
	InsertToTreeByType(true, EFTCdtChose, hSelected);
	InsertToTreeByType(true, EFTTradeChose, hSelected);
	InsertToTreeByType(true, EFTClrKLine, hSelected);

	//	
	if ( NULL != hSelected )
	{		
		m_TreeSys.SelectItem(hSelected);

		if ( 2 == m_iCurSel )
		{
			UpdateButton(&m_TreeSys, hSelected);
		}		
	}
}

void CDlgFormularManager::OnRadioAll() 
{
	m_iCurSel = 0;
	UpdateControls();

	int id = m_ListAll.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);	
	UpdateButton(&m_ListAll, id);
}

void CDlgFormularManager::OnRadioGroup() 
{
	m_iCurSel = 1;
	UpdateControls();
}

void CDlgFormularManager::OnRadioSys() 
{
	m_iCurSel = 2;
	UpdateControls();
}

void CDlgFormularManager::OnRadioUser() 
{	
	m_iCurSel = 3;
	UpdateControls();
	
	int id = m_ListUser.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);	
	UpdateButton(&m_ListUser, id);
}

void CDlgFormularManager::OnButtonOften() 
{
	//如果是常用,则改为不常用;
	//如果是不常用,则改为常用.

	CFormularContent* pContent = GetCurrentSelectedItemData();

	if ( NULL == pContent )
	{
		return;
	}

	if ( 1 == m_iCurSel || 2 == m_iCurSel )
	{
		if ( 1 == DWORD(pContent) || 2 == DWORD(pContent) )
		{
			return;
		}
	}

	CString StrName = pContent->name;

	if ( pContent->m_bOften )
	{
		pContent->ReMoveOften();
	}
	else
	{
		pContent->AddOften();
	}

	//	
	UpdateSelectedName(StrName);
	
	ReBuildAll();
}

void CDlgFormularManager::OnButtonClose() 
{
	// TODO: Add your control notification handler code here
}

void CDlgFormularManager::OnButtonModify() 
{
	CFormularContent* pContent = GetCurrentSelectedItemData();

	if ( 1 == m_iCurSel || 2 == m_iCurSel )
	{
		if ( 1 == DWORD(pContent) || 2 == DWORD(pContent) )
		{
			return;
		}
	}

	T_IndexMapPtr* p = NULL;

	if ( NULL == pContent )
	{
		return;
	}

	CString StrName = pContent->name;
	pContent->bNew = FALSE;

	// xl 1115 移植到TaiKlineDLg中的OnInitialDlg中，如不匹配，则idcancel
// 	if ( pContent->m_bProtected )
// 	{
// 		CDlgFormularPwd Dlg;
// 		if ( IDOK == Dlg.DoModal() )
// 		{
// 			CString StrPwd = Dlg.GetPwd();
// 
// 			if ( 0 != StrPwd.Compare(pContent->password) )
// 			{
// 				MessageBox(L"密码错误!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
// 				return;
// 			}
// 		}
// 		else
// 		{
// 			return;
// 		}
// 	}

	CTaiKlineDlgEditScreenStock  diaeditzbgs(m_pLib, pContent);

	if ( IDOK == diaeditzbgs.DoModal() )
	{
		// 用户自编
		p = m_pLib->m_UserIndex.Get(StrName);

		if ( NULL != p )
		{
			pContent->Assign(diaeditzbgs.GetResultContent());
			m_pLib->m_UserIndex.Modify(pContent);
		}
		else
		{
			//用户之前修改过的系统指标
			p = m_pLib->m_ModifyIndex.Get(StrName);

			if ( NULL != p )
			{
				pContent->Assign(diaeditzbgs.GetResultContent());
				m_pLib->m_ModifyIndex.Modify(pContent);				
			}
			else
			{
				//还未修改过的系统指标
				CString StrGroup1, StrGroup2;
				
				if ( m_pLib->m_SysIndex.GetGroup(pContent, StrGroup1, StrGroup2) )
				{
					m_pLib->m_ModifyIndex.Add(StrGroup1, StrGroup2, &diaeditzbgs.GetResultContent());
				}
			}
		}

		//

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		ASSERT(NULL != pFrame);
		
		
		if ( EFTNormal == pContent->m_eFormularType )
		{
			if ( diaeditzbgs.m_bFormulaDataChanged )
			{
				pFrame->OnViewFormulaChanged(EFUTIndexUpdate,pContent->name);
			}
			else if ( diaeditzbgs.m_bParamChanged )
			{
				pFrame->OnViewFormulaChanged(EFUTParamUpdate,pContent->name);
			}
		}
		else if ( EFTClrKLine == pContent->m_eFormularType )
		{
			if ( diaeditzbgs.m_bFormulaDataChanged || diaeditzbgs.m_bParamChanged )
			{
				pFrame->OnViewFormulaChanged(EFUTClrKLineUpdate, pContent->name);
			}
		}
		else if ( EFTCdtChose == pContent->m_eFormularType )
		{
			if ( diaeditzbgs.m_bFormulaDataChanged || diaeditzbgs.m_bParamChanged )
			{
				pFrame->OnViewFormulaChanged(EFUTCdtChoseUpdate, pContent->name);
			}
		}
		else if ( EFTTradeChose == pContent->m_eFormularType )
		{
			if ( diaeditzbgs.m_bFormulaDataChanged || diaeditzbgs.m_bParamChanged )
			{
				pFrame->OnViewFormulaChanged(EFUTTradeChoseUpdate, pContent->name);
			}
		}
		
		//
		UpdateSelectedName(StrName);

		//
		ReBuildAll();	
	}	
}

void CDlgFormularManager::OnButtonDel() 
{
	CFormularContent* pContent = GetCurrentSelectedItemData();
	if ( NULL == pContent )
	{
		return;
	}

	if ( 1 == m_iCurSel || 2 == m_iCurSel )
	{
		if ( 1 == DWORD(pContent) || 2 == DWORD(pContent) )
		{
			return;
		}
	}

	CString StrTip;
	bool32 bDel = true;
	if ( pContent->m_bSystem )
	{
		if ( !pContent->m_bCanRestore )
		{
			return;
		}
		StrTip = _T("确认恢复?");
		bDel = false;
	}
	else
	{
		StrTip = _T("确认删除?");
	}

	if ( IDYES != MessageBox(StrTip, AfxGetApp()->m_pszAppName, MB_ICONQUESTION|MB_YESNO) )
	{
		return;
	}

	//
	CString StrName = pContent->name;
	
	//	
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	ASSERT(NULL != pFrame);

	if ( bDel )
	{
		if ( EFTNormal == pContent->m_eFormularType )
		{
			pFrame->OnViewFormulaChanged(EFUTIndexDelete, StrName);	
		}
		else if ( EFTClrKLine == pContent->m_eFormularType )
		{
			pFrame->OnViewFormulaChanged(EFUTClrKLineDelete, StrName);	
		}
		else if ( EFTCdtChose == pContent->m_eFormularType )
		{
			pFrame->OnViewFormulaChanged(EFUTCdtChoseDelete, pContent->name);
		}
		else if ( EFTTradeChose == pContent->m_eFormularType )
		{
			pFrame->OnViewFormulaChanged(EFUTTradeChoseDelete, pContent->name);
		}

		//
		m_pLib->m_UserIndex.Del(StrName);
	}
	else
	{
		// 从修改中删除
		CFormularContent *pContentTmp = pContent->Clone();
		m_pLib->m_ModifyIndex.Del(StrName);
		pContent = NULL;

		// 恢复动作
		if ( EFTNormal == pContentTmp->m_eFormularType )
		{
			pFrame->OnViewFormulaChanged(EFUTIndexUpdate, StrName);	
		}
		else if ( EFTClrKLine == pContentTmp->m_eFormularType )
		{
			pFrame->OnViewFormulaChanged(EFUTClrKLineUpdate, StrName);	
		}
		else if ( EFTCdtChose == pContentTmp->m_eFormularType )
		{
			pFrame->OnViewFormulaChanged(EFUTCdtChoseUpdate, StrName);
		}
		else if ( EFTTradeChose == pContentTmp->m_eFormularType )
		{
			pFrame->OnViewFormulaChanged(EFUTTradeChoseUpdate, StrName);
		}
		DEL(pContentTmp);
	}

	// 通知
	CUserBlockManager::Instance()->Notify(CSubjectUserBlock::EUBUIndex);

	ReBuildAll();
}

void CDlgFormularManager::OnButtonNew() 
{
	CNewMenu Menu;
	Menu.CreatePopupMenu();
	Menu.SetMenuBkColor(RGB(250, 250, 246));
	Menu.SetMenuBitmapBkColor(RGB(154, 154, 154));
	Menu.SetMenuBorderColor(RGB(250, 250, 246));
	Menu.SetMenuItemSelColor(RGB(154, 154, 154));
	Menu.SetMenuItemSelTextColor(RGB(0, 0, 0));
	Menu.SetMenuItemNorTextColor(RGB(0, 0, 0));
	Menu.SetMenuItemDisableTextColor(RGB(0, 0, 0));
	

	Menu.AppendODMenu(L"指标公式", MF_BYCOMMAND|MF_STRING, KiMenuIdNormal);
	Menu.AppendODMenu(L"选股公式", MF_BYCOMMAND|MF_STRING, KiMenuIdCdtChose);
	Menu.AppendODMenu(L"交易公式", MF_BYCOMMAND|MF_STRING, KiMenuIdTradeChose);
	Menu.AppendODMenu(L"五彩Ｋ线", MF_BYCOMMAND|MF_STRING, KiMenuIdColorKline);

	CRect rectButton;
	GetDlgItem(IDC_BUTTON_NEW)->GetWindowRect(&rectButton);

	CPoint pt(rectButton.right, rectButton.top);
	Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, this);
}

void CDlgFormularManager::OnNewFormular(E_FormularType eFormularType)
{
	if (NULL == m_pLib)
	{
		return;
	}

	//只能在自编中添加
	CFormularContent* pContent = new CFormularContent;
	pContent->InitNew();
	pContent->m_eFormularType = eFormularType;
	//缺省设为常用 - 默认为常用, 如果用户在添加过程中取消了，则不强制为常用
	pContent->m_bOften = true;
	if ( eFormularType != EFTNormal )
	{
		pContent->m_bOften = FALSE;	// 其它类型指标不常用
	}
	CTaiKlineDlgEditScreenStock  diaeditzbgs(m_pLib,pContent);
	diaeditzbgs.SetNeedChangeColor(true);
	diaeditzbgs.SetCaptionBKColor(RGB(250, 250, 246));
	diaeditzbgs.SetFrameColor(RGB(102, 102, 102));
	diaeditzbgs.SetCaptionColor(RGB(10, 0, 4));


	if ( IDOK == diaeditzbgs.DoModal() )
	{
		pContent->Assign(diaeditzbgs.GetResultContent());
		if ( EFTNormal == eFormularType )
		{
			// 普通指标
			CIndexGroupNormal* pGroup1 = m_pLib->m_UserIndex.m_pGroupNormal;			
			CString StrGroup1 = pGroup1->m_StrName;
			
			if( pGroup1->m_Group2.GetSize() > 0 )
			{
				CIndexGroup2* pGroup2 = pGroup1->m_Group2.GetAt(0);
				CString StrGroup2 = pGroup2->m_StrName;
				
				m_pLib->m_UserIndex.Add(StrGroup1, StrGroup2, pContent);
			}
			else
			{
				//ASSERT(0);
			}			
		}
		else if ( EFTCdtChose == eFormularType )
		{
			// 条件选股指标
			CIndexGroupCdtChose* pGroup1 = m_pLib->m_UserIndex.m_pGroupCdtChose;			
			
			if ( NULL == pGroup1 )
			{
				pGroup1 = m_pLib->m_UserIndex.NewGroupCdtChose();
				if ( NULL == pGroup1 )
				{
					DEL(pContent);
					return;
				}
			}

			CString StrGroup1 = pGroup1->m_StrName;
			
			if( pGroup1->m_Group2.GetSize() > 0 )
			{
				CIndexGroup2* pGroup2 = pGroup1->m_Group2.GetAt(0);
				CString StrGroup2 = pGroup2->m_StrName;
				
				m_pLib->m_UserIndex.Add(StrGroup1, StrGroup2, pContent);
			}
			else
			{
				//ASSERT(0);
			}
		}
		else if ( EFTTradeChose == eFormularType )
		{
			// 交易指标
			CIndexGroupTradeChose* pGroup1 = m_pLib->m_UserIndex.m_pGroupTradeChose;	
			
			if ( NULL == pGroup1 )
			{		
				pGroup1 = m_pLib->m_UserIndex.NewGroupTradeChose();
				if ( NULL == pGroup1 )
				{
					DEL(pContent);
					return;	
				}							
			}
			
			CString StrGroup1 = pGroup1->m_StrName;				
			m_pLib->m_UserIndex.Add(StrGroup1, L"", pContent);
		}
		else if ( EFTClrKLine == eFormularType )
		{
			// 五彩 K 线
			CIndexGroupClrKLine* pGroup1 = m_pLib->m_UserIndex.m_pGroupClrKLine;	
			
			if ( NULL == pGroup1 )
			{
				pGroup1 = m_pLib->m_UserIndex.NewGroupClrKLine();
				if ( NULL == pGroup1 )
				{
					DEL(pContent);
					return;
				}
			}

			CString StrGroup1 = pGroup1->m_StrName;				
			m_pLib->m_UserIndex.Add(StrGroup1, L"", pContent);
		}


		// 通知
		CUserBlockManager::Instance()->Notify(CSubjectUserBlock::EUBUIndex);
				
		UpdateSelectedName(pContent->name);
		
		ReBuildAll();	

		delete pContent;	// Add为clone
		pContent = NULL;
	}
	else
	{
		DEL(pContent);
	}	
}

void CDlgFormularManager::OnOK() 
{
	if (m_bChooseFormular)
	{
		m_pFormularChoose = GetCurrentSelectedItemData();		
	}
	
	CDialog::OnOK();
}

void CDlgFormularManager::OnSelchangedTreeGroup(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	HTREEITEM hSelect = pNMTreeView->itemNew.hItem;		
	UpdateButton(&m_TreeGroup, hSelect);
	
	*pResult = 0;
}

void CDlgFormularManager::OnSelchangedTreeSys(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	HTREEITEM hSelect = pNMTreeView->itemNew.hItem;	
	UpdateButton(&m_TreeSys, hSelect);

	*pResult = 0;
}


int CALLBACK DlgFormularManagerListCompareFunc(LPARAM lParam1,LPARAM lParam2,LPARAM lParamSort)
{
	CDlgFormularManager *pDlg = (CDlgFormularManager *)lParamSort;
	CListCtrl* listCtrl = NULL;
	
	if ( 0 == pDlg->GetCurSel() )
	{
		listCtrl = &pDlg->m_ListAll;
	}
	else if ( 3 == pDlg->GetCurSel() )
	{
		listCtrl = &pDlg->m_ListUser;
	}

	if (NULL == listCtrl)
	{
		return 0;
	}
	
	//
	int isub= pDlg->GetSortCol();

	LVFINDINFO findInfo; 
	findInfo.flags = LVFI_PARAM; 
	findInfo.lParam = lParam1; 
	
	int iItem1 = listCtrl->FindItem(&findInfo, -1); 
	findInfo.lParam = lParam2; 
	
	int iItem2 = listCtrl->FindItem(&findInfo, -1); 
	
	CString strItem1 =listCtrl->GetItemText(iItem1,isub); 
	CString strItem2 =listCtrl->GetItemText(iItem2,isub);
	
	if( pDlg->GetSortDir())
		return strItem2.Compare(strItem1);
	else
		return -strItem2.Compare(strItem1);
}

void CDlgFormularManager::OnColumnclickListAll(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	m_iSortCol  = pNMListView->iSubItem;
	int sortnum = (int)GetProp(m_ListAll.GetSafeHwnd(), _T("SORT_COLUMN"));
	int sortasc = (int)GetProp(m_ListAll.GetSafeHwnd(), _T("SORT_DIRECTION"));
	
	if( sortnum == pNMListView->iSubItem )
	{
		sortasc = (sortasc+1) % 2;
		SetProp(m_ListAll.GetSafeHwnd(), _T("SORT_DIRECTION") , (HANDLE)sortasc);
	}
	
	SetProp(m_ListAll.GetSafeHwnd(),_T("SORT_COLUMN"),(HANDLE)pNMListView->iSubItem);
	
	m_iSortDir=sortasc;
	m_ListAll.SortItems(DlgFormularManagerListCompareFunc, (LPARAM)this);

	*pResult = 0;
}

void CDlgFormularManager::OnColumnclickListUser(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	//
	m_iSortCol  = pNMListView->iSubItem;
	int sortnum = (int)GetProp(m_ListUser.GetSafeHwnd(), _T("SORT_COLUMN"));
	int sortasc = (int)GetProp(m_ListUser.GetSafeHwnd(), _T("SORT_DIRECTION"));
	
	if( sortnum == pNMListView->iSubItem )
	{
		sortasc = (sortasc+1) % 2;
		SetProp(m_ListUser.GetSafeHwnd(), _T("SORT_DIRECTION") , (HANDLE)sortasc);
	}

	SetProp(m_ListUser.GetSafeHwnd(),_T("SORT_COLUMN"),(HANDLE)pNMListView->iSubItem);

	m_iSortDir=sortasc;
	m_ListUser.SortItems(DlgFormularManagerListCompareFunc, (LPARAM)this);
	
	*pResult = 0;
}

void CDlgFormularManager::OnClickListAll(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	int id = m_ListAll.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);	
	UpdateButton(&m_ListAll, id);
	
	*pResult = 0;
}

void CDlgFormularManager::OnClickListUser(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	int id = m_ListUser.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);	
	UpdateButton(&m_ListUser, id);
	
	*pResult = 0;
}

void CDlgFormularManager::OnDblclkListAll(NMHDR* pNMHDR, LRESULT* pResult)
{
	OnButtonModify();
	*pResult = 0;
}

void CDlgFormularManager::OnDblclkListUser(NMHDR* pNMHDR, LRESULT* pResult)
{
	OnButtonModify();
	*pResult = 0;
}

void CDlgFormularManager::OnDblclkTreeGroup(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	CFormularContent* pContent = GetCurrentSelectedItemData();
	
	if ( 1 == m_iCurSel || 2 == m_iCurSel )
	{
		if ( 1 == DWORD(pContent) || 2 == DWORD(pContent) )
		{
			*pResult = 0;
			return;
		}
	}
	
	OnButtonModify();
	*pResult = 1;
}

void CDlgFormularManager::OnDblclkTreeSys(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CFormularContent* pContent = GetCurrentSelectedItemData();
	
	if ( 1 == m_iCurSel || 2 == m_iCurSel )
	{
		if ( 1 == DWORD(pContent) || 2 == DWORD(pContent) )
		{
			*pResult = 0;
			return;
		}
	}
	
	OnButtonModify();
	*pResult = 1;
}

void CDlgFormularManager::OnMenu(UINT nID)
{
	if ( KiMenuIdNormal == nID )
	{
		OnNewFormular(EFTNormal);
	}
	else if ( KiMenuIdCdtChose == nID )
	{
		OnNewFormular(EFTCdtChose);
	}
	else if ( KiMenuIdTradeChose == nID )
	{
		OnNewFormular(EFTTradeChose);
	}
	else if ( KiMenuIdColorKline == nID )
	{
		OnNewFormular(EFTClrKLine);
	}	
	else
	{
		//ASSERT(0);
	}
}

//
void CDlgFormularManager::InsertToListByType(bool32 bUser, E_FormularType eType, OUT int32& iCurId)
{
	int32 i;
	POSITION pos;

	int32 iImageSysLock;
	int32 iImageSysNormal;
	int32 iImageUserLock;
	int32 iImageUserNormal;

	if ( EFTNormal == eType )
	{
		iImageSysLock	= KiImageIndexSysLock;
		iImageSysNormal	= KiImageIndexSys;

		iImageUserLock	= KiImageIndexUserLock;
		iImageUserNormal= KiImageIndexUser;
	}
	else if ( EFTCdtChose == eType )
	{
		iImageSysLock	= KiImageIndexCdtChoseSysLock;
		iImageSysNormal	= KiImageIndexCdtChoseSys;
			
		iImageUserLock	= KiImageIndexCdtChoseUserLock;
		iImageUserNormal= KiImageIndexCdtChoseUser;
			
	}
	else if ( EFTTradeChose == eType )
	{
		iImageSysLock	= KiImageIndexTradeChoseSysLock;
		iImageSysNormal	= KiImageIndexTradeChoseSys;
			
		iImageUserLock	= KiImageIndexTradeChoseUserLock;
		iImageUserNormal= KiImageIndexTradeChoseUser;
	}
	else if ( EFTClrKLine == eType )
	{
		iImageSysLock	= KiImageClrKLineSysLock;
		iImageSysNormal	= KiImageClrKLineSys;
			
		iImageUserLock	= KiImageClrKLineUserLock;
		iImageUserNormal= KiImageClrKLineUser;
	}
	else
	{
		//ASSERT(0);
		return;
	}

	int32 iPageIndex = -1;
	CListCtrl* pList = NULL;

	if ( bUser )
	{
		iPageIndex = 3;
		pList = &m_ListUser;		
	}
	else
	{
		iPageIndex = 0;
		pList = &m_ListAll;
	}

	if ( (iPageIndex < 0) || (NULL == pList) || (NULL == m_pLib))
	{
		return;
	}

	//
	if ( !bUser )
	{
		//系统的
		for ( pos = m_pLib->m_SysIndex.m_Names4Index.GetStartPosition(); pos != NULL; )
		{
			CString StrText;
			T_IndexMapPtr* p = NULL;
			m_pLib->m_SysIndex.m_Names4Index.GetNextAssoc(pos, StrText, (void*&)p);
			if ( NULL == p )
			{
				break;
			}
			
			CFormularContent* pContent = (CFormularContent*)p->p1;
			
			// 不是这种类型的不要插
			if ( eType != pContent->m_eFormularType )
			{
				continue;
			}

			if (m_bChooseFormular && CheckFlag(pContent->flag, CFormularContent::KAllowSub) )
			{
				continue;
			}
			
			StrText = pContent->name;
			
			if( !CFormulaLib::BeIndexShow(StrText) )
			{
				// 不让在界面上显示
				continue;
			}
			
			//判断在修改指标库中是否存在.
			p = m_pLib->m_ModifyIndex.Get(pContent->name);
			if ( NULL != p )
			{
				pContent = (CFormularContent*)p->p1;
				StrText += _T("[已修改]");
			}
			
			//pContent->m_bSystem = true;
			
			//加上是否常用
			if ( pContent->m_bOften)
			{
				StrText = StrText + _T("[常用]");
			}
			
			if ( pContent->m_bProtected )
			{			
				i = pList->InsertItem(pList->GetItemCount()+1, StrText, iImageSysLock);			
			}
			else
			{
				i = pList->InsertItem(pList->GetItemCount()+1, StrText, iImageSysNormal);
			}
			
			CString StrexplainBrief = pContent->explainBrief;
			pList->SetItemText(i, 1, StrexplainBrief);
			
			pList->SetItemData(i,(DWORD)pContent);
			
			if ( SameCutName(m_aStrSelectName[iPageIndex], pContent->name ) )
			{
				iCurId = i;
			}
		}
	}
	
	//自编的
	for ( pos = m_pLib->m_UserIndex.m_Names4Index.GetStartPosition(); pos != NULL; )
    {
        CString StrText;
        T_IndexMapPtr* p = NULL;
        m_pLib->m_UserIndex.m_Names4Index.GetNextAssoc(pos, StrText, (void*&)p);
		if ( NULL == p )
		{
			break;
		}
		CFormularContent* pContent = (CFormularContent*)p->p1;

		if (m_bChooseFormular && CheckFlag(pContent->flag, CFormularContent::KAllowSub) )
		{
			continue;
		}
		
		if ( eType != pContent->m_eFormularType )
		{
			continue;
		}

		if( !CFormulaLib::BeIndexShow(pContent->name) )
		{
			// 不让在界面上显示
			continue;
		}

		//加上是否常用
		if ( pContent->m_bOften)
		{
		 	StrText = StrText + _T("[常用]");
		}
		if ( pContent->m_bProtected )
		{		
			i = pList->InsertItem(pList->GetItemCount() + 1, StrText, iImageUserLock);		
		}
		else
		{			
			i = pList->InsertItem(pList->GetItemCount() + 1, StrText, iImageUserNormal);			
		}

		CString StrexplainBrief = pContent->explainBrief;
		pList->SetItemText(i, 1, StrexplainBrief);

		pList->SetItemData(i, (DWORD)pContent);
		if ( SameCutName ( m_aStrSelectName[iPageIndex], pContent->name ) )
		{
			iCurId = i;
		}
    }
}

void CDlgFormularManager::InsertToTreeByType(bool32 bSys, IN E_FormularType eType, OUT HTREEITEM& hSelected)
{
	int32 i, j, k, iSize2, iSize3;
	
	CString StrText;

	//
	CTreeCtrl* pTree = NULL;
	int32 iPageIndex = -1;

	if ( bSys )
	{
		pTree = &m_TreeSys;
		iPageIndex = 2;
	}
	else 
	{
		pTree = &m_TreeGroup;
		iPageIndex = 1;
	}

	if ( NULL == pTree || iPageIndex < 0  || NULL == m_pLib)
	{
		return;
	}

	//
	if ( eType == EFTNormal )
	{
		// 技术指标
		if ( NULL != m_pLib->m_SysIndex.m_pGroupNormal )
		{
			CIndexGroupNormal* pGroup1 = m_pLib->m_SysIndex.m_pGroupNormal;
			
			HTREEITEM hItem1 = pTree->InsertItem(pGroup1->m_StrName, KiImageIndexGroup1, KiImageIndexGroup1);
			pTree->SetItemData(hItem1,1);
			
			if ( SameCutName (m_aStrSelectName[iPageIndex], pGroup1->m_StrName ))
			{
				hSelected = hItem1;
			}
			
			iSize2 = pGroup1->m_Group2.GetSize();
			
			for ( j = 0; j < iSize2; j ++ )
			{
				CIndexGroup2* pGroup2 = pGroup1->m_Group2.GetAt(j);
				HTREEITEM hItem2 = pTree->InsertItem(pGroup2->m_StrName,KiImageIndexGroup2,KiImageIndexGroup2,hItem1);
				pTree->SetItemData(hItem2,2);	
				if ( SameCutName ( m_aStrSelectName[iPageIndex], pGroup2->m_StrName ) )
				{
					hSelected = hItem2;
				}
				iSize3 = pGroup2->m_Contents.GetSize();
				
				for ( k = 0; k < iSize3; k ++ )
				{
					CFormularContent* pContent = pGroup2->m_Contents.GetAt(k);
					StrText = pContent->name;
					
					if( !CFormulaLib::BeIndexShow(StrText) )
					{
						// 不让在界面上显示
						continue;
					}
					
					//判断在修改指标库中是否存在.
					T_IndexMapPtr* p = m_pLib->m_ModifyIndex.Get(pContent->name);
					if ( NULL != p )
					{
						pContent = (CFormularContent*)p->p1;
						StrText += _T("[已修改]");
					}
					//pContent->m_bSystem = true;
					//加上是否常用
					if ( pContent->m_bOften)
					{
						StrText = StrText + _T("[常用]");
					}
					HTREEITEM hItemF;
					if ( pContent->m_bProtected )
					{
						hItemF = pTree->InsertItem(StrText, KiImageIndexSysLock, KiImageIndexSysLock, hItem2);
					}
					else
					{
						hItemF = pTree->InsertItem(StrText, KiImageIndexSys, KiImageIndexSys, hItem2);
					}
					pTree->SetItemData(hItemF,(DWORD)pContent);//!!!
					if ( SameCutName ( m_aStrSelectName[iPageIndex], pContent->name ) )
					{
						hSelected = hItemF;
					}
				}
			}	
		}

		//
		if ( !bSys )
		{
			if ( NULL != m_pLib->m_UserIndex.m_pGroupNormal )
			{
				CIndexGroupNormal* pGroup1 = m_pLib->m_UserIndex.m_pGroupNormal;
				
				HTREEITEM hItem1 = pTree->InsertItem(pGroup1->m_StrName,KiImageIndexGroup1,KiImageIndexGroup1);
				pTree->SetItemData(hItem1,1);
				
				if ( SameCutName ( m_aStrSelectName[iPageIndex], pGroup1->m_StrName ) )
				{
					hSelected = hItem1;
				}
				
				iSize2 = pGroup1->m_Group2.GetSize();
				
				for ( j = 0; j < iSize2; j ++ )
				{
					CIndexGroup2* pGroup2 = pGroup1->m_Group2.GetAt(j);
					HTREEITEM hItem2 = pTree->InsertItem(pGroup2->m_StrName,KiImageIndexGroup2,KiImageIndexGroup2,hItem1);
					pTree->SetItemData(hItem2,2);
					if ( SameCutName ( m_aStrSelectName[iPageIndex], pGroup2->m_StrName ) )
					{
						hSelected = hItem2;
					}
					iSize3 = pGroup2->m_Contents.GetSize();
					for ( k = 0; k < iSize3; k ++ )
					{
						CFormularContent* pContent = pGroup2->m_Contents.GetAt(k);		 		
						StrText = pContent->name;
						
						if( !CFormulaLib::BeIndexShow(StrText) )
						{
							// 不让在界面上显示
							continue;
						}
						
						//加上是否常用
						if ( pContent->m_bOften)
						{
							StrText = StrText + _T("[常用]");
						}
						
						HTREEITEM hItemF;
						if ( pContent->m_bProtected )
						{
							hItemF = pTree->InsertItem(StrText,KiImageIndexUserLock,KiImageIndexUserLock,hItem2);
						}
						else
						{
							hItemF = pTree->InsertItem(StrText,KiImageIndexUser,KiImageIndexUser,hItem2);
						}
						pTree->SetItemData(hItemF,(DWORD)pContent);//!!!
						if ( SameCutName ( m_aStrSelectName[iPageIndex], pContent->name ) )
						{
							hSelected = hItemF;
						}
					}
				}
			}
		}
	}
	else if ( eType == EFTCdtChose )
	{
		// 系统条件选股
		if ( NULL != m_pLib->m_SysIndex.m_pGroupCdtChose )
		{
			CIndexGroupCdtChose* pGroup1 = m_pLib->m_SysIndex.m_pGroupCdtChose;
			
			HTREEITEM hItem1 = pTree->InsertItem(pGroup1->m_StrName,KiImageIndexCdtChoseGroup1,KiImageIndexCdtChoseGroup1);
			pTree->SetItemData(hItem1,1);
			
			if ( SameCutName (m_aStrSelectName[iPageIndex], pGroup1->m_StrName ))
			{
				hSelected = hItem1;
			}
			
			iSize2 = pGroup1->m_Group2.GetSize();
			
			for ( j = 0; j < iSize2; j ++ )
			{
				CIndexGroup2* pGroup2 = pGroup1->m_Group2.GetAt(j);
				HTREEITEM hItem2 = pTree->InsertItem(pGroup2->m_StrName,KiImageIndexCdtChoseGroup2,KiImageIndexCdtChoseGroup2,hItem1);
				pTree->SetItemData(hItem2,2);	
				if ( SameCutName ( m_aStrSelectName[iPageIndex], pGroup2->m_StrName ) )
				{
					hSelected = hItem2;
				}
				iSize3 = pGroup2->m_Contents.GetSize();
				
				for ( k = 0; k < iSize3; k ++ )
				{
					CFormularContent* pContent = pGroup2->m_Contents.GetAt(k);
					StrText = pContent->name;
					
					if( !CFormulaLib::BeIndexShow(StrText) )
					{
						// 不让在界面上显示
						continue;
					}
					
					//判断在修改指标库中是否存在.
					T_IndexMapPtr* p = m_pLib->m_ModifyIndex.Get(pContent->name);
					if ( NULL != p )
					{
						pContent = (CFormularContent*)p->p1;
						StrText += _T("[已修改]");
					}
					//pContent->m_bSystem = true;
					//加上是否常用
					if ( pContent->m_bOften)
					{
						StrText = StrText + _T("[常用]");
					}
					HTREEITEM hItemF;
					if ( pContent->m_bProtected )
					{
						hItemF = pTree->InsertItem(StrText, KiImageIndexCdtChoseSysLock, KiImageIndexCdtChoseSysLock, hItem2);
					}
					else
					{
						hItemF = pTree->InsertItem(StrText, KiImageIndexCdtChoseSys, KiImageIndexCdtChoseSys, hItem2);
					}
					pTree->SetItemData(hItemF,(DWORD)pContent);//!!!
					if ( SameCutName ( m_aStrSelectName[iPageIndex], pContent->name ) )
					{
						hSelected = hItemF;
					}
				}
			}	
		}
		
		if ( !bSys )
		{
			// 用户自编选股
			if ( NULL != m_pLib->m_UserIndex.m_pGroupCdtChose )
			{
				CIndexGroupCdtChose* pGroup1 = m_pLib->m_UserIndex.m_pGroupCdtChose;
				
				HTREEITEM hItem1 = pTree->InsertItem(pGroup1->m_StrName,KiImageIndexCdtChoseGroup1,KiImageIndexCdtChoseGroup1);
				pTree->SetItemData(hItem1,1);
				
				if ( SameCutName (m_aStrSelectName[iPageIndex], pGroup1->m_StrName ))
				{
					hSelected = hItem1;
				}
				
				iSize2 = pGroup1->m_Group2.GetSize();
				
				for ( j = 0; j < iSize2; j ++ )
				{
					CIndexGroup2* pGroup2 = pGroup1->m_Group2.GetAt(j);
					HTREEITEM hItem2 = pTree->InsertItem(pGroup2->m_StrName,KiImageIndexCdtChoseGroup2,KiImageIndexCdtChoseGroup2,hItem1);
					pTree->SetItemData(hItem2,2);	
					if ( SameCutName ( m_aStrSelectName[iPageIndex], pGroup2->m_StrName ) )
					{
						hSelected = hItem2;
					}
					iSize3 = pGroup2->m_Contents.GetSize();
					
					for ( k = 0; k < iSize3; k ++ )
					{
						CFormularContent* pContent = pGroup2->m_Contents.GetAt(k);
						StrText = pContent->name;
						
						if( !CFormulaLib::BeIndexShow(StrText) )
						{
							// 不让在界面上显示
							continue;
						}
						
						//pContent->m_bSystem = true;
						//加上是否常用
						if ( pContent->m_bOften)
						{
							StrText = StrText + _T("[常用]");
						}
						HTREEITEM hItemF;
						if ( pContent->m_bProtected )
						{
							hItemF = pTree->InsertItem(StrText, KiImageIndexCdtChoseUserLock, KiImageIndexCdtChoseUserLock, hItem2);
						}
						else
						{
							hItemF = pTree->InsertItem(StrText, KiImageIndexCdtChoseUser, KiImageIndexCdtChoseUser, hItem2);
						}
						pTree->SetItemData(hItemF,(DWORD)pContent);//!!!
						if ( SameCutName ( m_aStrSelectName[iPageIndex], pContent->name ) )
						{
							hSelected = hItemF;
						}
					}
				}	
			}
		}		
	}
	else if ( eType == EFTTradeChose )
	{
		// 系统交易选股
		if ( NULL != m_pLib->m_SysIndex.m_pGroupTradeChose )
		{
			CIndexGroupTradeChose* pGroup1 = m_pLib->m_SysIndex.m_pGroupTradeChose;
			HTREEITEM hTree = pTree->InsertItem(pGroup1->m_StrName, KiImageIndexTradeChoseGroup1, KiImageIndexTradeChoseGroup1);
			pTree->SetItemData(hTree, 1);
			
			if ( SameCutName ( m_aStrSelectName[iPageIndex], pGroup1->m_StrName ) )
			{
				hSelected = hTree;
			}
			
			for ( i = 0; i < pGroup1->m_ContentsTrade.GetSize(); i++ )
			{
				CFormularContent* pContent = pGroup1->m_ContentsTrade.GetAt(i);
				StrText = pContent->name;
				
				if ( NULL == pContent )
				{
					continue;
				}
				
				if ( !CFormulaLib::BeIndexShow(StrText) )
				{
					continue;
				}
				//判断在修改指标库中是否存在.
				T_IndexMapPtr* p = m_pLib->m_ModifyIndex.Get(pContent->name);
				if ( NULL != p )
				{
					pContent = (CFormularContent*)p->p1;
					StrText += _T("[已修改]");
				}

				if ( pContent->m_bOften )
				{
					StrText += _T("[常用]");
				}
				
				//
				HTREEITEM hTreeF = NULL;
				if ( pContent->m_bProtected )
				{
					hTreeF = pTree->InsertItem(StrText, KiImageIndexTradeChoseSysLock, KiImageIndexTradeChoseSysLock, hTree);
				}
				else
				{
					hTreeF = pTree->InsertItem(StrText, KiImageIndexTradeChoseSys, KiImageIndexTradeChoseSys, hTree);
				}
				
				pTree->SetItemData(hTreeF, (DWORD)pContent);
				
				//
				if ( SameCutName ( m_aStrSelectName[iPageIndex], pContent->name ) )
				{
					hSelected = hTreeF;
				}
			}
		}
		
		if (!bSys)
		{
			// 用户交易系统
			if ( NULL != m_pLib->m_UserIndex.m_pGroupTradeChose )
			{
				CIndexGroupTradeChose* pGroup1 = m_pLib->m_UserIndex.m_pGroupTradeChose;
				HTREEITEM hTree = pTree->InsertItem(pGroup1->m_StrName, KiImageIndexTradeChoseGroup1, KiImageIndexTradeChoseGroup1);
				pTree->SetItemData(hTree, 1);
				
				if ( SameCutName ( m_aStrSelectName[iPageIndex], pGroup1->m_StrName ) )
				{
					hSelected = hTree;
				}
				
				for ( i = 0; i < pGroup1->m_ContentsTrade.GetSize(); i++ )
				{
					CFormularContent* pContent = pGroup1->m_ContentsTrade.GetAt(i);
					StrText = pContent->name;
					
					if ( NULL == pContent )
					{
						continue;
					}
					
					if ( !CFormulaLib::BeIndexShow(StrText) )
					{
						continue;
					}
					
					if ( pContent->m_bOften )
					{
						StrText += _T("[常用]");
					}
					
					//
					HTREEITEM hTreeF = NULL;
					if ( pContent->m_bProtected )
					{
						hTreeF = pTree->InsertItem(StrText, KiImageIndexTradeChoseUserLock, KiImageIndexTradeChoseUserLock, hTree);
					}
					else
					{
						hTreeF = pTree->InsertItem(StrText, KiImageIndexTradeChoseUser, KiImageIndexTradeChoseUser, hTree);
					}
					
					pTree->SetItemData(hTreeF, (DWORD)pContent);
					
					//
					if ( SameCutName ( m_aStrSelectName[iPageIndex], pContent->name ) )
					{
						hSelected = hTreeF;
					}
				}
			}
		}
	}
	else if ( eType == EFTClrKLine )
	{
		// 系统五彩K 线
		if ( NULL != m_pLib->m_SysIndex.m_pGroupClrKLine )
		{
			CIndexGroupClrKLine* pGroup1 = m_pLib->m_SysIndex.m_pGroupClrKLine;
			HTREEITEM hTree = pTree->InsertItem(pGroup1->m_StrName, KiImageClrKLineGroup1, KiImageClrKLineGroup1);
			pTree->SetItemData(hTree, 1);
			
			if ( SameCutName ( m_aStrSelectName[iPageIndex], pGroup1->m_StrName ) )
			{
				hSelected = hTree;
			}
			
			for ( i = 0; i < pGroup1->m_ContentsClrKLine.GetSize(); i++ )
			{
				CFormularContent* pContent = pGroup1->m_ContentsClrKLine.GetAt(i);
				StrText = pContent->name;
				
				if ( NULL == pContent )
				{
					continue;
				}
				
				if ( !CFormulaLib::BeIndexShow(StrText) )
				{
					continue;
				}
				
				//判断在修改指标库中是否存在.
				T_IndexMapPtr* p = m_pLib->m_ModifyIndex.Get(pContent->name);
				if ( NULL != p )
				{
					pContent = (CFormularContent*)p->p1;
					StrText += _T("[已修改]");
				}

				if ( pContent->m_bOften )
				{
					StrText += _T("[常用]");
				}
				
				//
				HTREEITEM hTreeF = NULL;
				if ( pContent->m_bProtected )
				{
					hTreeF = pTree->InsertItem(StrText, KiImageClrKLineSysLock, KiImageClrKLineSysLock, hTree);
				}
				else
				{
					hTreeF = pTree->InsertItem(StrText, KiImageClrKLineSys, KiImageClrKLineSys, hTree);
				}
				
				pTree->SetItemData(hTreeF, (DWORD)pContent);
				
				//
				if ( SameCutName ( m_aStrSelectName[iPageIndex], pContent->name ) )
				{
					hSelected = hTreeF;
				}
			}
		}
		
		if ( !bSys )
		{// 用户五彩K 线
			if ( NULL != m_pLib->m_UserIndex.m_pGroupClrKLine )
			{
				CIndexGroupClrKLine* pGroup1 = m_pLib->m_UserIndex.m_pGroupClrKLine;
				HTREEITEM hTree = pTree->InsertItem(pGroup1->m_StrName, KiImageClrKLineGroup1, KiImageClrKLineGroup1);
				pTree->SetItemData(hTree, 1);
				
				if ( SameCutName ( m_aStrSelectName[iPageIndex], pGroup1->m_StrName ) )
				{
					hSelected = hTree;
				}
				
				for ( i = 0; i < pGroup1->m_ContentsClrKLine.GetSize(); i++ )
				{
					CFormularContent* pContent = pGroup1->m_ContentsClrKLine.GetAt(i);
					StrText = pContent->name;
					
					if ( NULL == pContent )
					{
						continue;
					}
					
					if ( !CFormulaLib::BeIndexShow(StrText) )
					{
						continue;
					}
					
					if ( pContent->m_bOften )
					{
						StrText += _T("[常用]");
					}
					
					//
					HTREEITEM hTreeF = NULL;
					if ( pContent->m_bProtected )
					{
						hTreeF = pTree->InsertItem(StrText, KiImageClrKLineUserLock, KiImageClrKLineUserLock, hTree);
					}
					else
					{
						hTreeF = pTree->InsertItem(StrText, KiImageClrKLineUser, KiImageClrKLineUser, hTree);
					}
					
					pTree->SetItemData(hTreeF, (DWORD)pContent);
					
					//
					if ( SameCutName ( m_aStrSelectName[iPageIndex], pContent->name ) )
					{
						hSelected = hTreeF;
					}
				}
			}
		}		
	}
}

void CDlgFormularManager::OnListSelChanged( NMHDR* pNMHDR, LRESULT* pResult )
{
	LPNMLISTVIEW pListView = (LPNMLISTVIEW)pNMHDR;
	if ( NULL == pListView )
	{
		return;
	}

	if ( pListView->hdr.hwndFrom == m_ListAll.GetSafeHwnd() )
	{
		int iSel = m_ListAll.GetNextItem(-1, LVNI_SELECTED);
		UpdateButton(&m_ListAll, iSel);
	}
	else if ( pListView->hdr.hwndFrom == m_ListUser.GetSafeHwnd() )
	{
		int iSel = m_ListUser.GetNextItem(-1, LVNI_SELECTED);
		UpdateButton(&m_ListUser, iSel);
	}
}



void CDlgFormularManager::SetExportButtonStatus( CFormularContent *pContent )
{
	if (NULL == m_pLib)
	{
		return;
	}

    CWnd* pExport = GetDlgItem(IDC_BUTTON_USER_EXPORT);
    T_IndexMapPtr* p = NULL;
    CString StrName;
    CFormularContent* pTmpContent;
    POSITION pos;
    for ( pos = m_pLib->m_UserIndex.m_Names4Index.GetStartPosition(); pos != NULL; )
    {
        p = NULL;
        m_pLib->m_UserIndex.m_Names4Index.GetNextAssoc(pos, StrName, (void*&)p);

        if ( NULL == p )
        {
            break;
        }
        pTmpContent = (CFormularContent*)p->p1;

        if ( NULL == pContent )
        {
            continue;
        }

        if(pContent == pTmpContent)
        {
            pExport->EnableWindow();
            return;
        }
    }

    pExport->EnableWindow(FALSE);
}

void CDlgFormularManager::SaveUserFomularToFile(CString filePath, CFormularContent *pContent )
{
    if((NULL==pContent) || (L""==filePath) || (NULL==m_pLib))
    {
        return;
    }

    T_IndexMapPtr* p = NULL;
    CString StrName;
    CFormularContent* pTmpContent;
    POSITION pos;
    TiXmlElement *pElement = NULL;
    for ( pos = m_pLib->m_UserIndex.m_Names4Index.GetStartPosition(); pos != NULL; )
    {
        p = NULL;
        m_pLib->m_UserIndex.m_Names4Index.GetNextAssoc(pos, StrName, (void*&)p);

        if ( NULL == p )
        {
            break;
        }
        pTmpContent = (CFormularContent*)p->p1;

        if ( NULL == pContent )
        {
            continue;
        }

        if(pContent == pTmpContent)
        {
            pElement = (TiXmlElement *)p->p2;
            break;
        }
    }

    CStdioFile file;
    CFileException expFile;
    if ( file.Open(filePath, CFile::modeCreate |CFile::modeWrite, &expFile) && (pElement!=NULL))
    {
        CString StrRootElement;		
		CString StrAttrVer = CString(GetXmlRootElementAttrVersion());
		CString StrVerVal = CString("1.0");
		CString StrAttrApp = CString(GetXmlRootElementAttrApp());
		CString StrAppVal = CString("ggtong");
		CString StrAttrData = CString(GetXmlRootElementAttrData());
		CString StrDataVal = CString("index");
		CString StrAttrType = CString(GetXmlRootElementAttrCodeType());
		CString StrTypeVal = CString("0");

        StrRootElement.Format(L"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n<XMLDATA %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\">\n",
            StrAttrVer.GetBuffer(), StrVerVal.GetBuffer(),
            StrAttrApp.GetBuffer(), StrAppVal.GetBuffer(),
            StrAttrData.GetBuffer(), StrDataVal.GetBuffer(),
            StrAttrType.GetBuffer(), StrTypeVal.GetBuffer()
            );	

        int32 iOutBufferSize = StrRootElement.GetLength();
        iOutBufferSize *= 2;
        iOutBufferSize += 100;		// 防止StrContent长度为0
        char *pcOutBuffer = new char[iOutBufferSize];
        memset(pcOutBuffer, 0, iOutBufferSize * sizeof(char));

        Unicode2MultiCharCoding(EMCCUtf8, StrRootElement, StrRootElement.GetLength(), pcOutBuffer, iOutBufferSize);

        TiXmlPrinter Printer;
        pElement->Accept(&Printer);			
        const char* pStr = Printer.CStr();

        CString strIndex;
        int len = strlen(pStr);
        TCHAR* c1 = (TCHAR*)malloc(sizeof(TCHAR)*len);
        MultiByteToWideChar( CP_ACP , 0 , pStr , len+1 , c1 , len+1);
        strIndex.Format(L"%s",c1);
		free(c1);

        CString strContent;
        E_FormularType eFormularType = pContent->m_eFormularType ;
        switch(eFormularType)
        {
        case EFTNormal:
            {
                // 普通指标
                CIndexGroupNormal* pGroup1 = m_pLib->m_UserIndex.m_pGroupNormal;			
                CString StrGroup1 = pGroup1->m_StrName;

                if( pGroup1->m_Group2.GetSize() > 0 )
                {
                    CIndexGroup2* pGroup2 = pGroup1->m_Group2.GetAt(0);
                    CString StrGroup2 = pGroup2->m_StrName;
                    strContent.Format(L"<group1 name= %s type=\"%d\"> \n<group2 name=\"%s\"> \n %s </group2> \n</group1>\n", 
                                       StrGroup1.GetBuffer(), 0, StrGroup2.GetBuffer(), strIndex.GetBuffer());
                }
                else
                {
                    //ASSERT(0);
                }	
            }
            break;
        case EFTCdtChose:
            {
                // 条件选股指标
                CIndexGroupCdtChose* pGroup1 = m_pLib->m_UserIndex.m_pGroupCdtChose;
                CString StrGroup1 = pGroup1->m_StrName;

                if( pGroup1->m_Group2.GetSize() > 0 )
                {
                    CIndexGroup2* pGroup2 = pGroup1->m_Group2.GetAt(0);
                    CString StrGroup2 = pGroup2->m_StrName;
                    strContent.Format(L"<group1 name= %s type=\"%d\"> \n<group2 name=\"%s\"> \n %s </group2> \n</group1>\n", 
                        StrGroup1.GetBuffer(), 2, StrGroup2.GetBuffer(), strIndex.GetBuffer());
                }
                else
                {
                    //ASSERT(0);
                }
            }
            break;

        case EFTTradeChose:
            {
                // 交易指标
                CIndexGroupTradeChose* pGroup1 = m_pLib->m_UserIndex.m_pGroupTradeChose;	
                CString StrGroup1 = pGroup1->m_StrName;				
                strContent.Format(L"<group1 name= %s type=\"%d\"> \n %s </group1>\n", 
                    StrGroup1.GetBuffer(), 3, strIndex.GetBuffer());
            }
            break;

        case EFTClrKLine:
            {
                // 五彩 K 线
                CIndexGroupClrKLine* pGroup1 = m_pLib->m_UserIndex.m_pGroupClrKLine;	

                CString StrGroup1 = pGroup1->m_StrName;				
                strContent.Format(L"<group1 name= %s type=\"%d\"> \n %s </group1>\n", 
                    StrGroup1.GetBuffer(), 1, strIndex.GetBuffer());
            }
            break;
        }

        int32 iContentBufferSize = strContent.GetLength();
        iContentBufferSize *= 2;
        iContentBufferSize += 10;		// 防止StrContent长度为0
        char *pcContentBuffer = new char[iContentBufferSize];
        memset(pcContentBuffer, 0, iContentBufferSize * sizeof(char));

        Unicode2MultiCharCoding(EMCCUtf8, strContent, strContent.GetLength(), pcContentBuffer, iContentBufferSize);

        CString StrEnd = L"</XMLDATA>";
        int32 iOutBufferSize2 = StrEnd.GetLength();
        iOutBufferSize2 *= 2;
        iOutBufferSize2 += 10;		// 防止StrContent长度为0
        char *pcOutBuffer2 = new char[iOutBufferSize2];
        memset(pcOutBuffer2, 0, iOutBufferSize2 * sizeof(char));

        Unicode2MultiCharCoding(EMCCUtf8, StrEnd, StrEnd.GetLength(), pcOutBuffer2, iOutBufferSize2);

        file.Write(pcOutBuffer, strlen(pcOutBuffer));
        file.Write(pcContentBuffer, strlen(pcContentBuffer));
        file.Write(pcOutBuffer2, strlen(pcOutBuffer2));

        file.Close();

        DEL_ARRAY(pcOutBuffer);
        DEL_ARRAY(pcContentBuffer);
        DEL_ARRAY(pcOutBuffer2);

        //if ( MessageBox(_T("导出成功，是否打开此文件？"), _T("提示"), MB_YESNO |MB_ICONQUESTION)
        //    == IDYES )
        //{
        //    ShellExecute(NULL, _T("open"), StrPath, NULL, NULL, SW_SHOW);
        //}
    }
    else
    {
        expFile.ReportError();
    }
}


void CDlgFormularManager::OnBnClickedButtonUserImport()
{
    // TODO: 在此添加控件通知处理程序代码
    CString StrFilter = L"xml文件 (*.xml)|*.xml|All Files (*.*)|*.*||";
    CFileDialog dlg(TRUE,NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,StrFilter, this);   //OFN_NOCHANGEDIR
    if(GetVersion()<0x80000000)
    {
        dlg.m_ofn.lStructSize=88;
    }
    else
    {
        dlg.m_ofn.lStructSize=76;
    }

    TCHAR strDir[MAX_PATH];
    ::GetCurrentDirectory(MAX_PATH-1,strDir);

    if ( dlg.DoModal() == IDOK )
    {
        ::SetCurrentDirectory(strDir);
        CString StrPath = dlg.GetPathName();
        int len = WideCharToMultiByte( CP_ACP , 0 , StrPath , StrPath.GetLength() , NULL , 0 , NULL , NULL );
        char* pPath =new char[len+1];
        len = WideCharToMultiByte(  CP_ACP , 0 , StrPath , StrPath.GetLength() , pPath , len +1 , NULL ,NULL );
        pPath[len] = 0;

        CIndexContentXml UserIndex;
        if ( !UserIndex.Load(pPath) )
        {
			DEL_ARRAY(pPath);
            return;
        }

        DEL_ARRAY(pPath);

        CFormularContent* pContent = NULL;
        if((UserIndex.m_Names4Index.GetSize()>0) && (m_pLib!=NULL))
        {
            T_IndexMapPtr *p = NULL;
            CString StrName;
            POSITION pos = UserIndex.m_Names4Index.GetStartPosition();
            UserIndex.m_Names4Index.GetNextAssoc(pos, StrName, (void*&)p);
            if ( NULL == p )
            {
                return;
            }

            pContent = (CFormularContent*)p->p1;

            if( NULL != UserIndex.m_pGroupNormal)
            {
                // 普通指标
                CIndexGroupNormal* pGroup1 = m_pLib->m_UserIndex.m_pGroupNormal;			
                CString StrGroup1 = pGroup1->m_StrName;

                if( pGroup1->m_Group2.GetSize() > 0 )
                {
                    CIndexGroup2* pGroup2 = pGroup1->m_Group2.GetAt(0);
                    CString StrGroup2 = pGroup2->m_StrName;

                    m_pLib->m_UserIndex.Add(StrGroup1, StrGroup2, pContent);
                }
                else
                {
                    //ASSERT(0);
                }	
            }
            else if(NULL != UserIndex.m_pGroupCdtChose)
            {
                // 条件选股指标
                CIndexGroupCdtChose* pGroup1 = m_pLib->m_UserIndex.m_pGroupCdtChose;			

                if ( NULL == pGroup1 )
                {
                    pGroup1 = m_pLib->m_UserIndex.NewGroupCdtChose();
                    if ( NULL == pGroup1 )
                    {
                        //ASSERT(0);
                        return;
                    }
                }

                CString StrGroup1 = pGroup1->m_StrName;

                if( pGroup1->m_Group2.GetSize() > 0 )
                {
                    CIndexGroup2* pGroup2 = pGroup1->m_Group2.GetAt(0);
                    CString StrGroup2 = pGroup2->m_StrName;

                    m_pLib->m_UserIndex.Add(StrGroup1, StrGroup2, pContent);
                }
                else
                {
                    //ASSERT(0);
                }
            }
            else if(NULL != UserIndex.m_pGroupTradeChose)
            {
                // 交易指标
                CIndexGroupTradeChose* pGroup1 = m_pLib->m_UserIndex.m_pGroupTradeChose;	

                if ( NULL == pGroup1 )
                {		
                    pGroup1 = m_pLib->m_UserIndex.NewGroupTradeChose();
                    if ( NULL == pGroup1 )
                    {
                        //ASSERT(0);
                        return;	
                    }							
                }

                CString StrGroup1 = pGroup1->m_StrName;				
                m_pLib->m_UserIndex.Add(StrGroup1, L"", pContent);
            }
            else if(NULL != UserIndex.m_pGroupClrKLine)
            {
                // 五彩 K 线
                CIndexGroupClrKLine* pGroup1 = m_pLib->m_UserIndex.m_pGroupClrKLine;	

                if ( NULL == pGroup1 )
                {
                    pGroup1 = m_pLib->m_UserIndex.NewGroupClrKLine();
                    if ( NULL == pGroup1 )
                    {
                        //ASSERT(0);
                        return;
                    }
                }

                CString StrGroup1 = pGroup1->m_StrName;				
                m_pLib->m_UserIndex.Add(StrGroup1, L"", pContent);
            }

            UpdateSelectedName(pContent->name);
            ReBuildAll();	
        }
    }
}

void CDlgFormularManager::OnBnClickedButtonUserExport()
{
    // TODO: 在此添加控件通知处理程序代码
    CFormularContent* pContent = GetCurrentSelectedItemData();

    if ( 1 == m_iCurSel || 2 == m_iCurSel )
    {
        if ( 1 == DWORD(pContent) || 2 == DWORD(pContent) )
        {
            return;
        }
    }

    if ( NULL == pContent || pContent->name == L"")
    {
        return;
    }

    CFileDialog dlg(FALSE,_T("xml"), pContent->name,NULL,
        _T("用户指标文件(*.xml)"), NULL);
    if(GetVersion()<0x80000000)
    {
        dlg.m_ofn.lStructSize=88;
    }
    else
    {
        dlg.m_ofn.lStructSize=76;
    }

    TCHAR strDir[MAX_PATH];
    ::GetCurrentDirectory(MAX_PATH-1,strDir);

    if ( dlg.DoModal() == IDOK )
    {
        ::SetCurrentDirectory(strDir);

        CString StrPath = dlg.GetPathName();
        SaveUserFomularToFile(StrPath, pContent);
    }
}

