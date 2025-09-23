// AdjustViewLayoutDlg.cpp : implementation file
//
#include "stdafx.h"
#include "ShareFun.h"
#include "GGTongView.h"
#include "IoViewManager.h"
#include "IoViewChart.h"
#include "IoViewKLine.h"
#include "IoViewReport.h"
#include "IoViewTimeSale.h"
#include "IoViewTrend.h"
#include "IoViewKLine.h"
#include "IoViewTick.h"
#include "AdjustViewLayoutDlg.h"
#include "dlgwspname.h"
#include "dlgworkspaceset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdjustViewLayoutDlg dialog

CAdjustViewLayoutDlg::CAdjustViewLayoutDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAdjustViewLayoutDlg::IDD, pParent)
{	//{{AFX_DATA_INIT(CAdjustViewLayoutDlg)
	//}}AFX_DATA_INIT
	m_pView = NULL;
	m_bClickSetCheck = true;	
}

void CAdjustViewLayoutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdjustViewLayoutDlg)
	DDX_Control(pDX, IDC_COMBO_GROUP, m_ComboGroupId);
	DDX_Control(pDX, IDC_LIST_IOVIEW, m_List);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAdjustViewLayoutDlg, CDialogEx)

	//{{AFX_MSG_MAP(CAdjustViewLayoutDlg)
	ON_WM_CLOSE()
	ON_WM_KEYDOWN()
	ON_BN_CLICKED(IDC_BUTTON_ZSPLIT, OnButtonZsplit)
	ON_BN_CLICKED(IDC_BUTTON_HSPLIT, OnButtonHsplit)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_BN_CLICKED(IDC_RADIO_TOP, OnRadioTop)
	ON_BN_CLICKED(IDC_RADIO_LEFT, OnRadioLeft)
	ON_BN_CLICKED(IDC_RADIO_BOTTOM, OnRadioBottom)
	ON_BN_CLICKED(IDC_RADIO_RIGHT, OnRadioRight)
	ON_BN_CLICKED(IDC_RADIO_NONE, OnRadioNone)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_IOVIEW, OnItemchangedListIoview)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_ADDTAB, OnButtonAddtab)
	ON_BN_CLICKED(IDC_BUTTON_DELTAB, OnButtonDeltab)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_LIST_IOVIEW, OnItemchangingListIoview)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT, OnButtonImport)
	ON_BN_CLICKED(IDC_BUTTON_RIGHT, OnButtonRight)
	ON_BN_CLICKED(IDC_BUTTON_LEFT, OnButtonLeft)
	ON_BN_CLICKED(IDC_BUTTON_TABCTL, OnButtonTabCtrl)
	ON_CBN_SELCHANGE(IDC_COMBO_GROUP, OnSelchangeComboGroup)
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CAdjustViewLayoutDlg message handlers

BOOL CAdjustViewLayoutDlg::OnInitDialog() 
{
	m_bClickSetCheck = false;

	CWnd * pWnd = GetDlgItem(IDC_LIST_IOVIEW);
	CListCtrl * pListCtrl = (CListCtrl *)pWnd;
	pListCtrl->SetExtendedStyle(pListCtrl->GetExtendedStyle() | LVS_EX_CHECKBOXES);
	pListCtrl->InsertColumn(0, L"显示项目", LVCFMT_LEFT, 300, 0);
	
	// 根据结构体内容，添加列表项
	/*
	int32 iCount = CIoViewManager::GetIoViewObjectCount();
	for ( int32 i=0 ; i< iCount ; i++ )
	{
		CString  StrName = CIoViewManager::GetIoViewObject(i)->m_StrLongName;
		pListCtrl->InsertItem(i,StrName);
	}
	*/
	
	int32 i = 0;
	int32 iCount = CIoViewManager::GetIoViewGroupObjectCount();
	for( i = 0; i <iCount; i++)
	{
		CString StrName = CIoViewManager::GetIoViewGroupObject(i)->m_StrLongName;
		pListCtrl->InsertItem(i, StrName);
		pListCtrl->SetItemData(i, CIoViewManager::GetIoViewGroupObject(i)->m_eIoViewGroupType);
	}

	CDialog::OnInitDialog();

	m_bClickSetCheck = true;
	
	CenterWindow();
	
	// 初始化分组信息
	m_ComboGroupId.InsertString(0,L"无分组");
	m_ComboGroupId.InsertString(-1,L"分组一");
	m_ComboGroupId.InsertString(-1,L"分组二");
	m_ComboGroupId.InsertString(-1,L"分组三");
	m_ComboGroupId.InsertString(-1,L"分组四");
	m_ComboGroupId.InsertString(-1,L"分组五");
	m_ComboGroupId.InsertString(-1,L"1|2 分组组合");
	m_ComboGroupId.InsertString(-1,L"1|3 分组组合");
	m_ComboGroupId.InsertString(-1,L"2|3 分组组合");
	m_ComboGroupId.InsertString(-1,L"1|2|3分组组合");
	
	//for ( i=1;i<=MAXGROUPID;i++)
 //	{
 //		CString StrId;
 //		StrId.Format(L"%d",i);
 //		m_ComboGroupId.InsertString(-1,StrId);
	//}

	m_ComboGroupId.SetCurSel(0);

	// 
	if ( NULL != m_pView )
	{
		CWnd* pButton = GetDlgItem(IDC_BUTTON_TABCTL);
		
		if ( NULL != pButton )
		{
			if ( CIoViewManager::EVTNone == m_pView->m_IoViewManager.m_eViewTab )
			{
				pButton->SetWindowText(L"显示标签");
			}
			else if ( CIoViewManager::EVTBottom == m_pView->m_IoViewManager.m_eViewTab )
			{
				pButton->SetWindowText(L"隐藏标签");
			}
		}		
	}

	return TRUE; 
}

void CAdjustViewLayoutDlg::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
	delete this;
}

void CAdjustViewLayoutDlg::OnClose() 
{
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	pWnd->m_pDlgAdjustViewLayout = NULL;	
	DestroyWindow();	
}

void CAdjustViewLayoutDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{	
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CAdjustViewLayoutDlg::PreTranslateMessage(MSG* pMsg) 
{
	return CDialog::PreTranslateMessage(pMsg);
}

void CAdjustViewLayoutDlg::OnButtonZsplit() 
{
	CGGTongView * pGGTongView = GetView();
	pGGTongView->AddSplit(CMPIChildFrame::SD_TOP);
}

void CAdjustViewLayoutDlg::OnButtonHsplit() 
{
	CGGTongView * pGGTongView = GetView();
	pGGTongView->AddSplit(CMPIChildFrame::SD_BOTTOM);	
}

void CAdjustViewLayoutDlg::OnButtonRight() 
{
	CGGTongView * pGGTongView = GetView();
	pGGTongView->AddSplit(CMPIChildFrame::SD_RIGHT);		
}

void CAdjustViewLayoutDlg::OnButtonLeft() 
{
	CGGTongView * pGGTongView = GetView();
	pGGTongView->AddSplit(CMPIChildFrame::SD_LEFT);	
}

void CAdjustViewLayoutDlg::OnButtonTabCtrl()
{
	if ( NULL != m_pView )
	{
		CWnd* pButton = GetDlgItem(IDC_BUTTON_TABCTL);
		if ( NULL == pButton )
		{
			return;
		}

		//
		if ( CIoViewManager::EVTNone == m_pView->m_IoViewManager.m_eViewTab )
		{
			pButton->SetWindowText(L"隐藏标签");

			//
			m_pView->m_IoViewManager.SetTabAllign(CIoViewManager::EVTBottom);
		}
		else if ( CIoViewManager::EVTBottom == m_pView->m_IoViewManager.m_eViewTab )
		{
			pButton->SetWindowText(L"显示标签");

			//
			m_pView->m_IoViewManager.SetTabAllign(CIoViewManager::EVTNone);
		}	
		
		//
 		CRect rectIoViewManager;
 		m_pView->m_IoViewManager.GetClientRect(&rectIoViewManager);
 		m_pView->m_IoViewManager.PostMessage(WM_SIZE, 0, MAKELPARAM(rectIoViewManager.Width(), rectIoViewManager.Height()));
	}
}

void CAdjustViewLayoutDlg::OnButtonSave() 
{	
	// 	CFileDialog   openfile(false,NULL,L"我的布局文件.xml",NULL,L"*.xml");   
	// 	
	// 	openfile.m_ofn.lpstrInitialDir=L"D:\\";   
	// 	if(openfile.DoModal() == IDOK)  
	// 	{ 
	// 		CMainFrame * pMainFrame =(CMainFrame *)AfxGetMainWnd();
	// 		CMPIChildFrame * pCurrentChild =(CMPIChildFrame * )pMainFrame->GetActiveFrame();
	// 		pCurrentChild->ToXml(0,"D:\\我的布局文件.xml");	
	// 	}
	CDlgWspName Dlg;	
	AfxInitRichEdit();
	CString StrFileName;
	ASSERT( NULL!= m_pView);
	Dlg.GetCurrentView(m_pView);
	Dlg.SetNewFileType(CDlgWorkSpaceSet::EFTIoViewManger);
	Dlg.DoModal();
// 	if ( IDOK == Dlg.DoModal())
// 	{
// 		StrFileName += CMainFrame::GetWorkSpaceDir();
// 		StrFileName += Dlg.m_StrName;
// 		StrFileName += L".vmg";
// 		const char * KStrFileName = W2A(StrFileName);
// 		m_pView->m_IoViewManager.ToXmlForWorkSpace(KStrFileName);
//	}	
}
void CAdjustViewLayoutDlg::OnCancel() 
{
	PostMessage(WM_CLOSE,0,0);
}
void CAdjustViewLayoutDlg::OnOK() 
{
	PostMessage(WM_CLOSE,0,0);
	//PostMessage(WM_CLOSE,0,0);
}
CGGTongView* CAdjustViewLayoutDlg::GetView()
{
	return m_pView;
}
void CAdjustViewLayoutDlg::UnSetView()
{
	//... ChildFrame 关闭的时候销毁指针
	ASSERT( NULL!= m_pView);
	delete m_pView;
}
void CAdjustViewLayoutDlg::EnableCtrl ( bool32 bEnable )
{
	CWnd* pWnd = GetDlgItem(IDC_LIST_IOVIEW);
	pWnd->EnableWindow(bEnable);
}
void CAdjustViewLayoutDlg::SetView ( CGGTongView* pView )
{
	m_bClickSetCheck = false;
	if ( NULL == pView )
	{
		//EnableCtrl(false);
		OnClose();
		return;
	}
	else
	{
		//EnableCtrl(true);
	}

	m_pView = pView;
	ASSERT(NULL != m_pView);
	CWnd * pWnd = GetDlgItem(IDC_LIST_IOVIEW);	
	
	CListCtrl * pListCtrl = (CListCtrl *)pWnd;
	
	for( int32 iIndex = 0; iIndex<pListCtrl->GetItemCount(); iIndex++)
	{
		pListCtrl->SetCheck(iIndex,false);
	}
	
	// 
	if ( NULL != m_pView )
	{
		CWnd* pButton = GetDlgItem(IDC_BUTTON_TABCTL);
		
		if ( NULL != pButton )
		{
			if ( CIoViewManager::EVTNone == m_pView->m_IoViewManager.m_eViewTab )
			{
				pButton->SetWindowText(L"显示标签");
			}
			else if ( CIoViewManager::EVTBottom == m_pView->m_IoViewManager.m_eViewTab )
			{
				pButton->SetWindowText(L"隐藏标签");
			}
		}		
	}

	/*
	// 根据IoViewManager 的标记值初始化CheckBox
	CButton * pCheck = (CButton * )GetDlgItem(IDC_CHECK_CHOOSE);
	
	if (!m_pView->m_IoViewManager.m_bAutoHide)
	{
        pCheck->SetCheck(1);
	}
	else
	{
		pCheck->SetCheck(0);
	}
	*/

	// 设置GroupID的复选框的状态
	
	int32 iId = m_pView->m_IoViewManager.GetGroupID();
	int32 iCurSel = 0;
	switch(iId)
	{
	case EGI_NONE:
		iCurSel = 0;
		break;

	case EGI_FIRST:
		iCurSel = 1;
		break;

	case EGI_SECOND:
		iCurSel = 2;
		break;

	case EGI_THIRD:
		iCurSel = 3;
		break;

	case EGI_FOUR:
		iCurSel = 4;
		break;

	case EGI_FIVE:
		iCurSel = 5;
		break;

	case EGI_SIX:
		iCurSel = 6;
		break;

	case EGI_SEVEN:
		iCurSel = 7;
		break;

	case EGI_EIGHT:
		iCurSel = 8;
		break;

	case EGI_NINE:
		iCurSel = 9;
		break;

	default:
		break;
	}
	m_ComboGroupId.SetCurSel(iCurSel);	
	
	// 根据当前 pView 包含的IoView，设置单选框的状态
	TRACE(_T("激活视图: %d   "), pView);
	for (int32 i= 0; i<m_pView->m_IoViewManager.m_IoViewsPtr.GetSize();i++)
	{				
		CIoViewBase* pIoView	= m_pView->m_IoViewManager.m_IoViewsPtr[i];		
		CString StrGroupName    = CIoViewManager::GetIoViewGroupName(pIoView);

		TRACE(_T("视图: %s    "), pIoView->GetRuntimeClass()->m_lpszClassName);
			
		for (int32 j = 0 ; j < pListCtrl->GetItemCount(); j++)
		{
			CString StrText = pListCtrl->GetItemText(j,0);

			if ( 0 == StrGroupName.Compare(StrText))
			{
				pListCtrl->SetCheck(j,true);
			}
		}
	}
	TRACE(_T("\n"));
	/*
	// 根据当前 pView 包含的IoView，设置单选框的状态

	int32 iCount = CIoViewManager::GetIoViewObjectCount();

	for (int32 i= 0; i<m_pView->m_IoViewManager.m_IoViewsPtr.GetSize();i++)
	{				
		CString StrXml = m_pView->m_IoViewManager.GetIoViewString(m_pView->m_IoViewManager.m_IoViewsPtr[i]);

		for (int32 j=0;j< iCount ;j++)
		{
			CString StrXmlName = CIoViewManager::GetIoViewObject(j)->m_StrXmlName;

			if (  StrXmlName == StrXml)
			{
				CString StrWinText = CIoViewManager::GetIoViewObject(j)->m_StrLongName;
				
				for ( int32 k=0;k<pListCtrl->GetItemCount();k++)
				{
					if (0 == StrWinText.Compare(pListCtrl->GetItemText(k,0)))
					{
						pListCtrl->SetCheck(k,true);
					}
				}				
			}
		}		
	}	
	// 根据Tab 对齐方式，设置Radio 按钮状态
		CButton * pRadioWndRight = (CButton * )GetDlgItem(IDC_RADIO_RIGHT);
		CButton * pRadioWndLeft = (CButton * )GetDlgItem(IDC_RADIO_LEFT);
		CButton * pRadioWndTop = (CButton * )GetDlgItem(IDC_RADIO_TOP);
		CButton * pRadioWndBottom = (CButton * )GetDlgItem(IDC_RADIO_BOTTOM);
		CButton * pRadioWndNone = (CButton * )GetDlgItem(IDC_RADIO_NONE);
    if ( m_pView->m_IoViewManager.EVTRight == m_pView->m_IoViewManager.m_eViewTab)
    {
		pRadioWndLeft->SetCheck(false);
		pRadioWndRight->SetCheck(true);
		pRadioWndTop->SetCheck(false);
		pRadioWndBottom->SetCheck(false);
		pRadioWndNone->SetCheck(false);	
    }
	else if ( m_pView->m_IoViewManager.EVTLeft == m_pView->m_IoViewManager.m_eViewTab)
	{
		pRadioWndLeft->SetCheck(true);
		pRadioWndRight->SetCheck(false);
		pRadioWndTop->SetCheck(false);
		pRadioWndBottom->SetCheck(false);
		pRadioWndNone->SetCheck(false);	
	}
	else if ( m_pView->m_IoViewManager.EVTTop == m_pView->m_IoViewManager.m_eViewTab)
	{
		pRadioWndLeft->SetCheck(false);
		pRadioWndRight->SetCheck(false);
		pRadioWndTop->SetCheck(true);
		pRadioWndBottom->SetCheck(false);
		pRadioWndNone->SetCheck(false);			
	}
	
	else if ( m_pView->m_IoViewManager.EVTBottom == m_pView->m_IoViewManager.m_eViewTab)
	{
		pRadioWndLeft->SetCheck(false);
		pRadioWndRight->SetCheck(false);
		pRadioWndTop->SetCheck(false);
		pRadioWndBottom->SetCheck(true);
		pRadioWndNone->SetCheck(false);			
	}
	else //if ( m_pView->m_IoViewManager.EVTNone == m_pView->m_IoViewManager.m_eViewTab)
	{
		pRadioWndLeft->SetCheck(false);
		pRadioWndRight->SetCheck(false);
		pRadioWndTop->SetCheck(false);
		pRadioWndBottom->SetCheck(false);
		pRadioWndNone->SetCheck(true);		
	}	
	*/
	m_bClickSetCheck = true;		
}

void CAdjustViewLayoutDlg::OnItemchangedListIoview(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// 两种情况触发本事件,Setcheck() 和 Itemchanged(),需要处理的是后者,利用	m_bClickSetCheck 标记量屏蔽前一个事件.

	if ( m_bClickSetCheck )
	{		
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
		
		CListCtrl * pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST_IOVIEW);
		E_IoViewGroupType eType = (E_IoViewGroupType)pListCtrl->GetItemData(pNMListView->iItem);					

		if (  pNMListView->uNewState == 0x2000 && pNMListView->uOldState == 0x1000 )
		{	
			// 添加	
			DelAllIoViews();
			AddIoViews((int32)eType);						
		}	
		
		if(pNMListView->uNewState == 0x1000 && pNMListView->uOldState == 0x2000 )
		{				
			// 删除								
			DelIoViews((int32)eType);							
		}

		SetView(m_pView);
	}	
	
	*pResult = 0;
}
void CAdjustViewLayoutDlg::OnRadioTop() 
{	
	if (m_pView != NULL)
	{
		m_pView->m_IoViewManager.m_eViewTab = m_pView->m_IoViewManager.EVTTop;
	}
	// RefreshChildFrame();
}

void CAdjustViewLayoutDlg::OnRadioLeft() 
{
	if (m_pView != NULL)
	{
		m_pView->m_IoViewManager.m_eViewTab = m_pView->m_IoViewManager.EVTLeft;
	}
	// RefreshChildFrame();	
}

void CAdjustViewLayoutDlg::OnRadioBottom() 
{
	if (m_pView != NULL)
	{
		m_pView->m_IoViewManager.m_eViewTab = m_pView->m_IoViewManager.EVTBottom;
	}
	// RefreshChildFrame();
}

void CAdjustViewLayoutDlg::OnRadioRight() 
{
	if (m_pView != NULL)
	{
		m_pView->m_IoViewManager.m_eViewTab = m_pView->m_IoViewManager.EVTRight;
	}
	// RefreshChildFrame();
}

void CAdjustViewLayoutDlg::OnRadioNone() 
{
	if (m_pView != NULL)
	{
		m_pView->m_IoViewManager.m_eViewTab = m_pView->m_IoViewManager.EVTNone;
	}
    // RefreshChildFrame();		
}

void CAdjustViewLayoutDlg::OnButtonDel() 
{
// 	CWnd *pParent = m_pView->GetParent();
// 	while (1)
// 	{
// 		if (NULL == pParent)
// 			return;
// 		if (pParent->IsKindOf(RUNTIME_CLASS(CMPIChildFrame)))
// 			break;
// 		pParent = pParent->GetParent();
// 	}
// 
// 	if (NULL == pParent)
// 		return;
// 
// 	CMPIChildFrame *pChildFrame = (CMPIChildFrame *)pParent;
// 	pChildFrame->DelSplit(m_pView);

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if ( NULL != pMainFrame )
	{
		pMainFrame->PostMessage(WM_COMMAND, IDM_IOVIEWBASE_CLOSECUR, 0);
	}
}

void CAdjustViewLayoutDlg::OnButtonAddtab() 
{	
	if (NULL == m_pView)
	{
		return;
	}

	CMPIChildFrame * pChildFrame =	DYNAMIC_DOWNCAST(CMPIChildFrame, m_pView->GetParentFrame());
	
	CWnd * pWnd = pChildFrame->GetNextWindow(GW_CHILD);
	if (pWnd->IsKindOf(RUNTIME_CLASS(CTabSplitWnd)))
	{
		CTabSplitWnd * pTabSplitWnd = (CTabSplitWnd *)pWnd;
		if (pTabSplitWnd->GetDelAll())
		{
			pTabSplitWnd->SetDelAll(false);
		}
	}
	
	m_pView->AddSplit(CMPIChildFrame::SD_TAB);

}

void CAdjustViewLayoutDlg::OnButtonDeltab()
{
	if (NULL == m_pView)
	{
		return;
	}

	CMPIChildFrame * pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, m_pView->GetParentFrame());
	
	CWnd * pWnd = pChildFrame->GetNextWindow(GW_CHILD);
	
	if ( pWnd->IsKindOf(RUNTIME_CLASS(CTabSplitWnd)))
	{
		CTabSplitWnd* pTabSplitWnd = (CTabSplitWnd*)pWnd;
		int32 iCurPage = pTabSplitWnd->m_wndTabCtrl.GetCurtab();
		pTabSplitWnd->DelTab(iCurPage);
	}
}

void CAdjustViewLayoutDlg::RefreshChildFrame()
{
	if (NULL == m_pView)
	{
		return;
	}

	CWnd * pWnd = m_pView->GetParent();
	
	while ( ! pWnd->IsKindOf(RUNTIME_CLASS(CMPIChildFrame)) )
	{		
		pWnd = pWnd->GetParent();
	}	
	CRect rect;
	pWnd->GetWindowRect(rect);
	pWnd->GetParent()->ScreenToClient(rect);
	rect.right  +=1;
	rect.bottom += 1;	 
	pWnd->MoveWindow(rect);
	rect.right  -=1;
	rect.bottom -=1;
	pWnd->MoveWindow(rect);	
}

void CAdjustViewLayoutDlg::OnItemchangingListIoview(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	/*
	if ( m_bClickSetCheck )
	{
		if (pNMListView->uNewState == 0x1000 && pNMListView->uOldState == 0x2000 && 1 == m_pView->m_IoViewManager.m_IoViewsPtr.GetSize())
		{
			*pResult = 1;
		}
	}		
	*/
}		

void CAdjustViewLayoutDlg::OnButtonImport() 
{
	/*
	CStringArray aStrFileTitle,aStrFileDir;	
	CString StrDir = CMainFrame::GetWorkSpaceDir();
	
	aStrFileTitle.RemoveAll();
	aStrFileDir.RemoveAll();
	
	if(StrDir.Right(1) != "/")
		StrDir += "/";
	StrDir += "*.vmg";

	CFileFind file;
	BOOL bContinue = file.FindFile(StrDir);
	while (bContinue)
	{
		bContinue = file.FindNextFile();
		CString StrfileTitle = file.GetFileTitle();	
		CString StrfileDir   = file.GetFilePath();
		aStrFileTitle.Add(StrfileTitle);
		aStrFileDir.Add(StrfileDir);
	}
	CDlgWorkSpaceSet dlg;
	dlg.GetCurrentView(m_pView);
	dlg.GetFileInfoArray(aStrFileDir,aStrFileTitle);
	dlg.SetNewFileType(CDlgWorkSpaceSet::EFTIoViewManger);
	dlg.DoModal();
	// RefreshChildFrame();
	this->OnClose();
	*/
}


void CAdjustViewLayoutDlg::OnSelchangeComboGroup() 
{
	CGGTongView * pView = GetView();
	if ( NULL == pView)
	{
		return;
	}

	int32 iCur = m_ComboGroupId.GetCurSel();

	int32 iGroupID = -1;
	switch(iCur)
	{
	case 0:
		iGroupID = EGI_NONE;
		break;

	case 1:
		iGroupID = EGI_FIRST;
		break;

	case 2:
		iGroupID = EGI_SECOND;
		break;

	case 3:
		iGroupID = EGI_THIRD;
		break;

	case 4:
		iGroupID = EGI_FOUR;
		break;

	case 5:
		iGroupID = EGI_FIVE;
		break;

	case 6:
		iGroupID = EGI_SIX;
		break;

	case 7:
		iGroupID = EGI_SEVEN;
		break;

	case 8:
		iGroupID = EGI_EIGHT;
		break;

	case 9:
		iGroupID = EGI_NINE;
		break;

	default:
		break;
	}

	ASSERT(NULL!= pView);
	ASSERT(iCur>=0&&iCur<=MAXGROUPID);
	if(-1 != iGroupID)
	{
		pView->m_IoViewManager.SetGroupID(iGroupID);	
	}
}

void CAdjustViewLayoutDlg::AddIoView(const T_IoViewObject* pIoViewObject)
{
	if ( (NULL==pIoViewObject) || (NULL == m_pView))
	{
		return;
	}

	if ( pIoViewObject->m_uID >= ID_PIC_HIDE_INMENU )
	{
		return;		// 隐藏的业务视图不能在这里创建
	}
	
	//
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->CreateIoViewByPicMenuID(pIoViewObject->m_uID, true, &m_pView->m_IoViewManager);
}

void CAdjustViewLayoutDlg::AddIoViews(int32 iIndex)
{
	// 增加指定类型的视图:
	if ( iIndex < 0 || iIndex > E_IVGTCount)
	{
		return;
	}

	E_IoViewGroupType eGroupType = (E_IoViewGroupType)iIndex;
		
	for ( int32 i = CIoViewManager::GetIoViewObjectCount() - 1; i >= 0 ; i--)
	{
		// 
		const T_IoViewObject* pIoViewObject = CIoViewManager::GetIoViewObject(i);
		if ( pIoViewObject->m_eGroupType == eGroupType )
		{
			AddIoView(pIoViewObject);
		}				
	}
}

void CAdjustViewLayoutDlg::DelIoView(const CString& StrXmlName)
{
	//项目存在,则删除已有ioview.		
	if (NULL == m_pView)
	{
		return;
	}
	
	const T_IoViewObject *pstIoViewObject = m_pView->m_IoViewManager.FindIoViewObjectByXmlName(StrXmlName);	

	if (NULL == pstIoViewObject)
	{
		return;
	}
	
	for (int32 i = m_pView->m_IoViewManager.m_IoViewsPtr.GetSize() - 1; i >= 0; i--)
	{							
		CString StrTest = m_pView->m_IoViewManager.GetIoViewString(m_pView->m_IoViewManager.m_IoViewsPtr[i]);
		if ( StrXmlName == StrTest )
		{
			m_pView->m_IoViewManager.m_IoViewsPtr[i]->DestroyWindow();
			DEL(m_pView->m_IoViewManager.m_IoViewsPtr[i]);
			m_pView->m_IoViewManager.m_IoViewsPtr.RemoveAt(i,1);
			m_pView->m_IoViewManager.m_GuiTabWnd.DeleteTab(i);			
		}
	}

	int32 iCountTab  = m_pView->m_IoViewManager.m_GuiTabWnd.GetCount();
	int32 iCountView = m_pView->m_IoViewManager.m_IoViewsPtr.GetSize();

	if ( 0 == iCountTab || 0 == iCountView)
	{
		m_pView->RedrawWindow();
		return;
	}
	 
	int32 iCurView   = iCountView -1;

	m_pView->m_IoViewManager.m_GuiTabWnd.SetCurtab(iCurView);
	m_pView->m_IoViewManager.OnTabSelChange();			// 强制调用tab切换，因为setcur此时不一定会触发消息发送
														// 但是由于某些视图的setfocus导致view重新注册，从而此窗口再次刷新状态(出现状态不同步)
	m_pView->m_IoViewManager.m_IoViewsPtr[iCurView]->ShowWindow(SW_SHOW);
	m_pView->RedrawWindow();
}

void CAdjustViewLayoutDlg::DelIoViews(int32 iIndex)
{
	if ( iIndex < 0 || iIndex > E_IVGTCount)
	{
		return;
	}
	
	E_IoViewGroupType eGroupType = (E_IoViewGroupType)iIndex;
	
	for ( int32 i = 0 ; i < CIoViewManager::GetIoViewObjectCount(); i++)
	{
		// 
		const T_IoViewObject* pIoViewObject = CIoViewManager::GetIoViewObject(i);
		if ( pIoViewObject->m_eGroupType == eGroupType )
		{
			DelIoView(pIoViewObject->m_StrXmlName);
		}				
	}

	if (NULL == m_pView)
	{
		return;
	}

	// 重新注册视窗
	if ( IsWindow(m_pView->GetSafeHwnd()) )
	{
		SetView(m_pView);
	}
}

void CAdjustViewLayoutDlg::DelAllIoViews()
{
	if (NULL == m_pView)
	{
		return;
	}

	for (int32 i = m_pView->m_IoViewManager.m_IoViewsPtr.GetSize() - 1; i >= 0; i--)
	{							
		m_pView->m_IoViewManager.m_IoViewsPtr[i]->DestroyWindow();
		DEL(m_pView->m_IoViewManager.m_IoViewsPtr[i]);
		m_pView->m_IoViewManager.m_IoViewsPtr.RemoveAt(i,1);
		m_pView->m_IoViewManager.m_GuiTabWnd.DeleteTab(i);
	}
}