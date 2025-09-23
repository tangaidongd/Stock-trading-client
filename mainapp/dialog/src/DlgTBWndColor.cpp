#include "StdAfx.h"
#include "DlgTBWndColor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
CDlgTBWndColor::CDlgTBWndColor(CWnd *pParent) : CDialog(CDlgTBWndColor::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTBWndColor)
	//}}AFX_DATA_INIT
}

void CDlgTBWndColor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTBWndColor)
	DDX_Control(pDX, IDC_COMBO_ITEM, m_CtrlComboItem);
	//}}AFX_DATA_MAP
}

BOOL CDlgTBWndColor::OnInitDialog()
{
	CDialog::OnInitDialog();

	if ( !m_StrTitle.IsEmpty() )
	{
		CString s;
		GetWindowText(s);
		SetWindowText(s + _T(" - ") + m_StrTitle);
	}

	CWnd *pWnd = GetDlgItem(IDC_STATIC_COLOR);
	if ( pWnd )
	{
		pWnd->ModifyStyle(0, SS_NOTIFY, 0);
	}
	
	// 初始化列表
	for ( TBWndColorItemMap::iterator it = m_mapColorItems.begin(); it != m_mapColorItems.end(); it++ )
	{
		CString s = it->second.m_StrDesc;
		int i = m_CtrlComboItem.AddString(it->second.m_StrDesc);
		ASSERT( -1 != i );
		if ( -1 != i )
		{
			m_CtrlComboItem.SetItemData(i, it->first);
		}
		if ( m_brhColor.m_hObject == NULL )
		{
			m_brhColor.CreateSolidBrush(it->second.m_clr);
			m_CtrlComboItem.SetCurSel(i);
		}
	}
	
	if ( pWnd )
	{
		pWnd->Invalidate(TRUE);
	}

	//
	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgTBWndColor, CDialog)
//{{AFX_MSG_MAP(CDlgTBWndColor)
ON_WM_CTLCOLOR()
ON_CBN_SELCHANGE(IDC_COMBO_ITEM, OnColorItemChange)
ON_STN_CLICKED(IDC_STATIC_COLOR, OnColorItemSet)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CDlgTBWndColor::SetColorItem( const TBWndColorItemMap &mapItemsIn, TBWndColorItemMap *pMapItemsOut/*=NULL*/, LPCTSTR lpszTitle/*=NULL*/ )
{
	int iRet = IDCANCEL;
	m_mapColorItems.clear();
	m_mapColorItems = mapItemsIn;
	if ( !m_mapColorItems.empty() )
	{
		if ( NULL != lpszTitle )
		{
			m_StrTitle = lpszTitle;
		}
		iRet = DoModal();
		if ( IDOK == iRet && pMapItemsOut )
		{
			pMapItemsOut->clear();
			*pMapItemsOut = m_mapColorItems;
		}
	}
	return iRet;
}

HBRUSH CDlgTBWndColor::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
	HBRUSH hBrush = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	if ( pWnd->GetDlgCtrlID() == IDC_STATIC_COLOR )
	{
		pDC->SetBkMode(TRANSPARENT);
		hBrush = m_brhColor;
	}
	return hBrush;
}

void CDlgTBWndColor::OnColorItemChange()
{
	COLORREF clr;
	if ( GetCurrentColor(clr) )
	{
		m_brhColor.DeleteObject();
		m_brhColor.CreateSolidBrush(clr);
		
		CWnd *pWnd = GetDlgItem(IDC_STATIC_COLOR);
		if ( pWnd )
		{
			pWnd->Invalidate(TRUE);
		}
	}
}

void CDlgTBWndColor::OnColorItemSet()
{
	COLORREF clr;
	if ( !GetCurrentColor(clr) )
	{
		MessageBox(_T("无法获取当前颜色"));
		return;
	}

	CColorDialog	dlg(clr, CC_ANYCOLOR|CC_FULLOPEN, this);
	if ( IDOK == dlg.DoModal() )
	{
		clr = dlg.GetColor();
		SetCurrentColor(clr);
	}
}

bool CDlgTBWndColor::GetCurrentColor( COLORREF &clr )
{
	bool bRet = false;
	int iIndex = m_CtrlComboItem.GetCurSel();
	if ( LB_ERR != iIndex )
	{
		int iKey = m_CtrlComboItem.GetItemData(iIndex);
		if ( m_mapColorItems.count(iKey) > 0 )
		{
			bRet = true;
			clr = m_mapColorItems[iKey].m_clr;
		}
	}
	return bRet;
}

int CDlgTBWndColor::SetCurrentColor( COLORREF clr )
{
	int clrOld = -1;
	int iIndex = m_CtrlComboItem.GetCurSel();
	if ( LB_ERR != iIndex )
	{
		int iKey = m_CtrlComboItem.GetItemData(iIndex);
		if ( m_mapColorItems.count(iKey) > 0 )
		{
			clrOld = (int)m_mapColorItems[iKey].m_clr;
			m_mapColorItems[iKey].m_clr = clr;
			if ( (DWORD)clrOld != clr )
			{
				m_brhColor.DeleteObject();
				m_brhColor.CreateSolidBrush(clr);
				CWnd *pWnd = GetDlgItem(IDC_STATIC_COLOR);
				if ( pWnd )
				{
					pWnd->Invalidate(TRUE);
				}
			}
		}
	}
	return clrOld;
}


#pragma warning(pop)