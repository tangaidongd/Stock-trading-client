// DlgBlockSet.cpp : implementation file
//

#include "stdafx.h"



#include "UserBlockManager.h"
#include "keyboarddlg.h"

#include "ShareFun.h"
#include "DlgBlockSet.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgBlockSet dialog


CDlgBlockSet::CDlgBlockSet(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgBlockSet::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgBlockSet)
	m_StrHotKey = _T("");
	m_StrName = _T("");
	//}}AFX_DATA_INIT
	m_bModify	= true;
	m_Color		= COLORNOTCOUSTOM;
	m_eHead		= ERTCustom;	
	m_bNeedColorText = false;
	m_bCheck = false;
}


void CDlgBlockSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBlockSet)
	DDX_Text(pDX, IDC_EDIT_HOTKEY, m_StrHotKey);
	DDX_Text(pDX, IDC_EDIT_NAME, m_StrName);
	DDX_Control(pDX, IDC_COMBO_HEAD, m_ComboHead);
	DDX_Control(pDX, IDC_STATIC_COLOR, m_StaticColor);
	DDX_Check(pDX, IDC_CHECK, m_bCheck);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBlockSet, CDialogEx)
	//{{AFX_MSG_MAP(CDlgBlockSet)
	ON_EN_CHANGE(IDC_EDIT_NAME, OnChangeName)
	ON_BN_CLICKED(IDC_CHECK, OnCheck)
	ON_MESSAGE(UM_SETCOLOR,OnSetColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBlockSet message handlers

BOOL CDlgBlockSet::OnInitDialog() 
{
	CDialog::OnInitDialog();
	HICON hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,FALSE);

	// TODO: Add extra initialization here
	m_StrNameBK = m_StrName;

	int32 iCurSel = 0;

	for ( int32 i = 0 ; i < ERTCount; i++)
	{
		E_ReportType eHead = (E_ReportType)i;
		CString StrHead = CReportScheme::GetReportTypeCnName(eHead);
		m_ComboHead.InsertString(-1,StrHead);

		if ( eHead == m_eHead)
		{
			iCurSel = i;
		}
	}

	m_ComboHead.SetCurSel(iCurSel);

	//
	m_StaticColor.m_bParentDlgFace = false;	
	
	if ( COLORNOTCOUSTOM == m_Color)
	{
		m_bCheck = false;
		GetDlgItem(IDC_STATIC_COLOR)->EnableWindow(FALSE);
	}
	else
	{
		m_bCheck = true;
		GetDlgItem(IDC_STATIC_COLOR)->EnableWindow(TRUE);
		m_StaticColor.SetColor(m_Color);
		m_StaticColor.Invalidate();
	}
	//
	UpdateData(false);
	//
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgBlockSet::SetColor(COLORREF clr)
{
	m_Color = clr;
}
LRESULT CDlgBlockSet::OnSetColor(WPARAM wParam,LPARAM lParam)
{
	COLORREF clr = COLORREF(wParam);
	SetColor(clr);
	return TRUE;
}
void CDlgBlockSet::SetHead(E_ReportType eHead)
{
	m_eHead = eHead;
}

E_ReportType CDlgBlockSet::GetHead()
{
	return m_eHead;
}

COLORREF CDlgBlockSet::GetColor()
{
	return m_Color;
}

void  CDlgBlockSet::SetModifyFlag(bool32 bModify)
{
	m_bModify = bModify;
}

//////////////////////////////////////////////////////////////////////////

void CDlgBlockSet::OnChangeName()
{
	UpdateData(TRUE);
	if ( m_StrName.GetLength() == 0 )
	{
		m_StrHotKey = L"";
		UpdateData(FALSE);
		return;
	}

	m_StrHotKey = ConvertHZToPY(m_StrName);
	UpdateData(FALSE);
}

void CDlgBlockSet::OnOK() 
{
	UpdateData(TRUE);

	if ( m_StrName.GetLength() < 1 || m_StrHotKey.GetLength() < 1 )
	{
		MessageBox(_T("名称或快捷键不能为空."), AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return;
	}

	if ( m_StrName.GetLength() > 20 )
	{
		MessageBox(_T("名称过长"), AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return;
	}

	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	CGGTongDoc *pDocument = pApp->m_pDocument;	
	ASSERT(NULL!= pDocument);
	CAbsCenterManager *pAbsCenterManager = pDocument->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return;
	}

	int32 i, iSize = pAbsCenterManager->GetMerchManager().m_BreedListPtr.GetSize();
	for ( i = 0; i < iSize; i ++ )
	{
		CBreed* pBreed = pAbsCenterManager->GetMerchManager().m_BreedListPtr.GetAt(i);
		int32 j, iSize2 = pBreed->m_MarketListPtr.GetSize();
		for ( j = 0; j < iSize2; j ++ )
		{
			CMarket* pMarket = pBreed->m_MarketListPtr.GetAt(j);
			if ( 0 == pMarket->m_MarketInfo.m_StrCnName.CompareNoCase(m_StrName) )
			{
				CString StrMsg = _T("与 [") + pBreed->m_StrBreedCnName + _T("] [") + m_StrName + _T("] 重复,不能添加.");
				MessageBox(StrMsg);
				return;
			}
		}
	}

	//
	bool32 bExist = false;
	CArray<T_Block, T_Block&> aBlocks;
	CUserBlockManager::Instance()->GetBlocks(aBlocks);
	
	for ( i = 0; i < aBlocks.GetSize(); i++ )
	{
		if ( m_StrName == aBlocks.GetAt(i).m_StrName )
		{
			bExist = true;
		}
	}

	if ( bExist )
	{
		if ( (m_bModify) && (m_StrName == m_StrNameBK) )
		{
		}
		else
		{
			MessageBox(L"与已有板块名称重复.", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
			return;
		}	
	}

	int32 iCurSel = m_ComboHead.GetCurSel();
	m_eHead = (E_ReportType)iCurSel;
	
	if (!m_bCheck)
	{
		m_Color = COLORNOTCOUSTOM;
	}

	CDialog::OnOK();
}

void CDlgBlockSet::OnCheck() 
{	
	UpdateData(true);
	
	if (!m_bCheck)
	{
		GetDlgItem(IDC_STATIC_COLOR)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_STATIC_COLOR)->EnableWindow(TRUE);
		m_StaticColor.SetColor(m_Color);
		m_StaticColor.Invalidate();
	}
	
	UpdateData(false);
}