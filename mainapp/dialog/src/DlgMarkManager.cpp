#include "StdAfx.h"

#include "DlgMarkManager.h"
#include "DlgMarkText.h"
#include "UserBlockManager.h"
#include "DlgAddToBlock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgMarkManager dialog

CDlgMarkManager::CDlgMarkManager(CWnd* pParent /*=NULL*/)
: CDialogEx(CDlgMarkManager::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMarkManager)
	//}}AFX_DATA_INIT
}


void CDlgMarkManager::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMarkManager)
	DDX_Control(pDX, IDC_LIST1, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMarkManager, CDialogEx)
//{{AFX_MSG_MAP(CDlgMarkManager)
ON_BN_CLICKED(IDC_CHECK0, OnBtnChecked0)
ON_BN_CLICKED(IDC_CHECK1, OnBtnChecked1)
ON_BN_CLICKED(IDC_CHECK2, OnBtnChecked2)
ON_BN_CLICKED(IDC_CHECK3, OnBtnChecked3)
ON_BN_CLICKED(IDC_CHECK4, OnBtnChecked4)
ON_BN_CLICKED(IDC_CHECK5, OnBtnChecked5)
ON_BN_CLICKED(IDC_CHECK6, OnBtnChecked6)
ON_BN_CLICKED(IDC_BUTTON_ADDTOBLOCK, OnBtnAddToBlock)
ON_BN_CLICKED(IDC_BUTTON_MODIFYMARK, OnBtnModifyMark)
ON_BN_CLICKED(IDC_BUTTON_SELALL, OnBtnSelAll)
ON_BN_CLICKED(IDC_BUTTON_UNSELALL, OnBtnUnSelAll)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDlgMarkManager::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设定初始值
	m_List.SetExtendedStyle(m_List.GetExtendedStyle() |LVS_EX_CHECKBOXES |LVS_EX_FULLROWSELECT);
	m_List.InsertColumn(0, _T("商品代码"), LVCFMT_LEFT, 80);
	m_List.InsertColumn(1, _T("商品名称"), LVCFMT_LEFT, 80);
	m_List.InsertColumn(2, _T("标记"), LVCFMT_LEFT, 60);
	
	return TRUE;
}

void CDlgMarkManager::OnOK()
{

}

void CDlgMarkManager::PromptErrorInput( const CString &StrPrompt, CWnd *pWndFocus /*= NULL*/ )
{
	if ( !StrPrompt.IsEmpty() )
	{
		AfxMessageBox(StrPrompt, MB_ICONWARNING |MB_OK);
	}
	if ( NULL != pWndFocus )
	{
		pWndFocus->SetFocus();
		if ( pWndFocus->IsKindOf(RUNTIME_CLASS(CEdit)) )
		{
			((CEdit *)pWndFocus)->SetSel(0, -1);
		}
		else if ( pWndFocus->IsKindOf(RUNTIME_CLASS(CRichEditCtrl)) )
		{
			((CRichEditCtrl *)pWndFocus)->SetSel(0, -1);
		}
	}
}

bool32 CDlgMarkManager::UpdateRow( CMerch *pMerch, T_MarkData *pMarkData/* = NULL*/ )
{
	if ( NULL == pMerch )
	{
		return false;
	}

	T_MarkData MarkData;
	if ( NULL == pMarkData
		&& !CMarkManager::Instance().QueryMark(pMerch, MarkData) )
	{
		return false;
	}
	else if ( NULL == pMarkData )
	{
		pMarkData = &MarkData;	// 赋予取到的值
	}

	CString StrType = GetMarkTypeName(pMarkData->m_eType);
	const int32 iSize = m_List.GetItemCount();
	for ( int32 i=0; i < iSize ; i++ )
	{
		CMerch *pMerchInRow = (CMerch *)m_List.GetItemData(i);
		if ( pMerch == pMerchInRow )
		{
			// 更新这一行
			m_List.SetItemText(i, 0, pMerch->m_MerchInfo.m_StrMerchCode);
			m_List.SetItemText(i, 1, pMerch->m_MerchInfo.m_StrMerchCnName);
			m_List.SetItemText(i, 2, StrType);
			return true;
		}
	}

	int32 iLine = m_List.InsertItem(iSize, pMerch->m_MerchInfo.m_StrMerchCode);
	m_List.SetItemText(iLine, 1, pMerch->m_MerchInfo.m_StrMerchCnName);
	m_List.SetItemText(iLine, 2, StrType);
	m_List.SetItemData(iLine, (DWORD)pMerch);
	return true;
}

CString CDlgMarkManager::GetMarkTypeName( E_MarkType eType )
{
	switch (eType)
	{
	case EMT_Text:
		return _T("文字");
	case EMT_1:
		return _T("①");
	case EMT_2:
		return _T("②");
	case EMT_3:
		return _T("③");
	case EMT_4:
		return _T("④");
	case EMT_5:
		return _T("⑤");
	case EMT_6:
		return _T("⑥");
	}

	return _T("");
}

void CDlgMarkManager::OnBtnChecked0()
{
	CButton *pBtn = (CButton *)GetDlgItem(IDC_CHECK0);
	bool32 bApp = NULL == pBtn ? true : pBtn->GetCheck() == BST_CHECKED;
	AppendOrDelMarkType(EMT_Text, bApp);
}

void CDlgMarkManager::AppendOrDelMarkType(E_MarkType eType, bool32 bApp/* = true*/)
{
	const CMarkManager::MarkMap &markMap = CMarkManager::Instance().GetMarkMap();
	POSITION pos = markMap.GetStartPosition();
	CMerch *pMerch;
	T_MarkData MarkData;
	while ( NULL != pos )
	{
		markMap.GetNextAssoc(pos, pMerch, MarkData);
		if ( MarkData.m_eType == eType )
		{
			if ( bApp )
			{
				UpdateRow(pMerch, &MarkData);
			}
			else
			{
				RemoveRow(pMerch);
			}
		}
	}
}

void CDlgMarkManager::OnBtnChecked1()
{
	CButton *pBtn = (CButton *)GetDlgItem(IDC_CHECK1);
	bool32 bApp = NULL == pBtn ? true : pBtn->GetCheck() == BST_CHECKED;
	AppendOrDelMarkType(EMT_1, bApp);
}

void CDlgMarkManager::OnBtnChecked2()
{
	CButton *pBtn = (CButton *)GetDlgItem(IDC_CHECK2);
	bool32 bApp = NULL == pBtn ? true : pBtn->GetCheck() == BST_CHECKED;
	AppendOrDelMarkType(EMT_2, bApp);
}

void CDlgMarkManager::OnBtnChecked3()
{
	CButton *pBtn = (CButton *)GetDlgItem(IDC_CHECK3);
	bool32 bApp = NULL == pBtn ? true : pBtn->GetCheck() == BST_CHECKED;
	AppendOrDelMarkType(EMT_3, bApp);
}

void CDlgMarkManager::OnBtnChecked4()
{
	CButton *pBtn = (CButton *)GetDlgItem(IDC_CHECK4);
	bool32 bApp = NULL == pBtn ? true : pBtn->GetCheck() == BST_CHECKED;
	AppendOrDelMarkType(EMT_4, bApp);
}

void CDlgMarkManager::OnBtnChecked5()
{
	CButton *pBtn = (CButton *)GetDlgItem(IDC_CHECK5);
	bool32 bApp = NULL == pBtn ? true : pBtn->GetCheck() == BST_CHECKED;
	AppendOrDelMarkType(EMT_5, bApp);
}

void CDlgMarkManager::OnBtnChecked6()
{
	CButton *pBtn = (CButton *)GetDlgItem(IDC_CHECK6);
	bool32 bApp = NULL == pBtn ? true : pBtn->GetCheck() == BST_CHECKED;
	AppendOrDelMarkType(EMT_6, bApp);
}

void CDlgMarkManager::OnBtnAddToBlock()
{
	MerchArray aMerchs;
	if ( GetCheckedMerchs(aMerchs)<=0 )
	{
		PromptErrorInput(_T("请勾选要加入板块的商品！"), NULL);
		return;
	}

	// 选择板块
	T_Block block;
	if ( CDlgAddToBlock::GetUserSelBlock(block) )
	{
		for ( int32 i=0; i < aMerchs.GetSize() ; i++ )
		{
			CUserBlockManager::Instance()->AddMerchToUserBlock(aMerchs[i], block.m_StrName, false);
		}
		// 商品多的时候, 提高性能, 只通知和保存文件一次.
		CUserBlockManager::Instance()->SaveXmlFile();
		CUserBlockManager::Instance()->Notify(CSubjectUserBlock::EUBUMerch);
	}
}

void CDlgMarkManager::OnBtnModifyMark()
{
	MerchArray aMerchs;
	if ( GetCheckedMerchs(aMerchs)<=0 )
	{
		PromptErrorInput(_T("请勾选需要改变标记的商品！"), NULL);
		return;
	}

	CRect rc(0,0,0,0);
	::GetWindowRect(GetDlgItem(IDC_BUTTON_MODIFYMARK)->GetSafeHwnd(), &rc);

	// 弹出菜单
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, ID_MARK_CANCEL, _T("取消标记"));
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_MARK_TEXT,	_T("标记文字"));
	menu.AppendMenu(MF_STRING, ID_MARK_1,		_T("标记①"));
	menu.AppendMenu(MF_STRING, ID_MARK_2,		_T("标记②"));
	menu.AppendMenu(MF_STRING, ID_MARK_3,		_T("标记③"));
	menu.AppendMenu(MF_STRING, ID_MARK_4,		_T("标记④"));
	menu.AppendMenu(MF_STRING, ID_MARK_5,		_T("标记⑤"));
	menu.AppendMenu(MF_STRING, ID_MARK_6,		_T("标记⑥"));
	const int iRet = menu.TrackPopupMenu(TPM_NONOTIFY |TPM_RETURNCMD |TPM_TOPALIGN |TPM_LEFTALIGN
		, rc.left
		, rc.bottom+1
		, AfxGetMainWnd()	);
	switch (iRet)
	{
	case ID_MARK_CANCEL:
		{
			for ( int32 i=0; i < aMerchs.GetSize() ; i++ )		// 会不会太多导致出现效率问题？
			{
				CMarkManager::Instance().RemoveMark(aMerchs[i]);
				RemoveRow(aMerchs[i]);
			}
		}
		break;
	case ID_MARK_TEXT:
		{
			CDlgMarkText dlg;
			for ( int32 i=0; i < aMerchs.GetSize() ; i++ )
			{
				dlg.SetMerch(aMerchs[i]);
				if ( dlg.DoModal() == IDOK ) // 每个商品都询问一次
				{
					// 可能是删除或者是添加了
					if ( dlg.IsMarkAdded() )
					{
						UpdateRow(aMerchs[i]);
					}
					else
					{
						RemoveRow(aMerchs[i]);
					}
				}
			}
		}
		break;
	case ID_MARK_1:
	case ID_MARK_2:
	case ID_MARK_3:
	case ID_MARK_4:
	case ID_MARK_5:
	case ID_MARK_6:
		{
			// 修改为某种标记
			for ( int32 i=0; i < aMerchs.GetSize() ; i++ )
			{
				CMarkManager::Instance().SetMark(aMerchs[i], (E_MarkType)(iRet-ID_MARK_1+EMT_1));
				UpdateRow(aMerchs[i]);
			}
		}
		break;
	default:
		ASSERT( iRet == 0 );
		break;
	}

}

void CDlgMarkManager::OnBtnSelAll()
{
	const int32 iSize = m_List.GetItemCount();
	for ( int32 i=0; i < iSize ; i++ )
	{
		m_List.SetCheck(i, TRUE);
	}
}

void CDlgMarkManager::OnBtnUnSelAll()
{
	const int32 iSize = m_List.GetItemCount();
	for ( int32 i=0; i < iSize ; i++ )
	{
		m_List.SetCheck(i, FALSE);
	}
}

void CDlgMarkManager::RemoveRow( CMerch *pMerch )
{
	if ( NULL == pMerch )
	{
		return;
	}
	
	const int32 iSize = m_List.GetItemCount();
	for ( int32 i=0; i < iSize ; i++ )
	{
		CMerch *pMerchInRow = (CMerch *)m_List.GetItemData(i);
		if ( pMerch == pMerchInRow )
		{
			m_List.DeleteItem(i);
			return;
		}
	}
}

int32 CDlgMarkManager::GetCheckedMerchs( OUT MerchArray &aMerchs )
{
	aMerchs.RemoveAll();
	const int32 iSize = m_List.GetItemCount();
	for ( int32 i=0; i < iSize ; i++ )
	{
		BOOL bCheck = m_List.GetCheck(i);
		if ( bCheck )
		{
			aMerchs.Add((CMerch *)m_List.GetItemData(i));
		}
	}
	return aMerchs.GetSize();
}
