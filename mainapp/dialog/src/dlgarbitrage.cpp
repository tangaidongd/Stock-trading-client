// dlgarbitrage.cpp : implementation file
//

#include "stdafx.h"

#include "DlgArbitrage.h"
#include "DlgChooseStockVar.h"
#include "dlgarbiformular.h"
#include "ArbitrageManage.h"
#include "ArbitrageFormula.h"
#include "ShareFun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgArbitrage dialog


CDlgArbitrage::CDlgArbitrage(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgArbitrage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgArbitrage)
	m_StrFormularA = _T("");
	m_StrFormularB = _T("");
	m_StrFormularC = _T("");
	m_StrMerchA	   = _T("A多");
	m_StrMerchB	   = _T("B空");
	m_StrMerchC	   = _T("C多");
	m_uiSaveDec	   = 2;
	//}}AFX_DATA_INIT

	m_bAdd = false;
}


void CDlgArbitrage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgArbitrage)
	DDX_Control(pDX, IDC_COMBO_PRICE_TYPE, m_ComboPrice);
	DDX_Control(pDX, IDC_COMBO_ARBITRAGE_TYPE, m_ComboArbitrage);
	DDX_Text(pDX, IDC_EDIT_FORMULARA, m_StrFormularA);
	DDX_Text(pDX, IDC_EDIT_FORMULARB, m_StrFormularB);
	DDX_Text(pDX, IDC_EDIT_FORMULARC, m_StrFormularC);
	DDX_Text(pDX, IDC_STATIC_MERCHA, m_StrMerchA);
	DDX_Text(pDX, IDC_STATIC_MERCHB, m_StrMerchB);
	DDX_Text(pDX, IDC_STATIC_MERCHC, m_StrMerchC);
	DDX_Text(pDX, IDC_EDIT_SAVEDEC, m_uiSaveDec);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgArbitrage, CDialogEx)
	//{{AFX_MSG_MAP(CDlgArbitrage)
	ON_BN_CLICKED(IDC_BUTTON_CHOOSEA, OnButtonChooseA)
	ON_BN_CLICKED(IDC_BUTTON_CHOOSEB, OnButtonChooseB)
	ON_BN_CLICKED(IDC_BUTTON_CHOOSEC, OnButtonChooseC)
	ON_BN_CLICKED(IDC_BUTTON_CHANGEA, OnButtonChangeA)
	ON_BN_CLICKED(IDC_BUTTON_CHANGEB, OnButtonChangeB)
	ON_BN_CLICKED(IDC_BUTTON_CHANGEC, OnButtonChangeC)
	ON_CBN_SELCHANGE(IDC_COMBO_ARBITRAGE_TYPE, OnSelchangeComboArbitrageType)
	ON_EN_CHANGE(IDC_EDIT_SAVEDEC, OnChangeEditSavedec)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgArbitrage message handlers

BOOL CDlgArbitrage::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitialCtrls();

	return TRUE;
}

void CDlgArbitrage::SetInitParams(bool32 bAdd, CArbitrage* pArbitrage)
{
	if ( bAdd )
	{
		m_bAdd = true;
	}
	else
	{
		if ( NULL != pArbitrage )
		{
			m_bAdd = false;			
			m_stArbitrage = *pArbitrage;
			m_stArbitrageBk = *pArbitrage;
		}		
		else
		{
			//ASSERT(0);
		}
	}
}

void CDlgArbitrage::InitialCtrls()
{
	int32 iIndex = m_ComboArbitrage.InsertString(-1, L"两腿套利");
	if ( -1 != iIndex )
	{
		m_ComboArbitrage.SetItemData(iIndex, EABTwoMerch);
	}

	iIndex = m_ComboArbitrage.InsertString(-1, L"蝶式套利");
	if ( -1 != iIndex )
	{
		m_ComboArbitrage.SetItemData(iIndex, EABThreeMerch);
	}
	
	iIndex = m_ComboPrice.AddString(L"多/空");
	if ( -1 != iIndex )
	{
		m_ComboPrice.SetItemData(iIndex, EAPDiv);
	}

	iIndex = m_ComboPrice.AddString(L"多-空");
	if ( -1 != iIndex )
	{
		m_ComboPrice.SetItemData(iIndex, EAPDiff);
	}

	m_ComboArbitrage.SetCurSel(0);
	m_ComboPrice.SetCurSel(0);

	//
	if ( !m_bAdd )
	{
		int32 i;
		for ( i = 0; i < m_ComboArbitrage.GetCount(); i++ )
		{
			E_Arbitrage eType = (E_Arbitrage)m_ComboArbitrage.GetItemData(i);

			if ( eType == m_stArbitrage.m_eArbitrage )
			{
				m_ComboArbitrage.SetCurSel(i);
				break;
			}
		}

		for (i = 0; i < m_ComboPrice.GetCount(); i++ )
		{
			E_ArbPrice eType = (E_ArbPrice)m_ComboPrice.GetItemData(i);

			if ( eType == m_stArbitrage.m_eArbPrice )
			{
				m_ComboPrice.SetCurSel(i);
				break;
			}
		}

		//
		if ( NULL != m_stArbitrage.m_MerchA.m_pMerch )
		{
			m_StrMerchA = m_stArbitrage.m_MerchA.m_pMerch->m_MerchInfo.m_StrMerchCnName;
		}

		if ( NULL != m_stArbitrage.m_MerchA.m_pFormular )
		{			
			m_StrFormularA = m_stArbitrage.m_MerchA.m_pFormular->GetFormula();
		}
	
		//
		if ( NULL != m_stArbitrage.m_MerchB.m_pMerch )
		{
			m_StrMerchB = m_stArbitrage.m_MerchB.m_pMerch->m_MerchInfo.m_StrMerchCnName;
		}

		if ( NULL != m_stArbitrage.m_MerchB.m_pFormular )
		{
			m_StrFormularB = m_stArbitrage.m_MerchB.m_pFormular->GetFormula();
		}

		//
		if ( EABThreeMerch == m_stArbitrage.m_eArbitrage )
		{
			ShowCtrlMerchC(true);
			
			//
			if ( NULL != m_stArbitrage.m_MerchC.m_pMerch )
			{
				m_StrMerchC = m_stArbitrage.m_MerchC.m_pMerch->m_MerchInfo.m_StrMerchCnName;
			}
			
			if ( NULL != m_stArbitrage.m_MerchC.m_pFormular )
			{
				m_StrFormularC = m_stArbitrage.m_MerchC.m_pFormular->GetFormula();
			}			
		}

		//
		m_uiSaveDec = m_stArbitrage.m_iSaveDec;
	}

	//
	UpdateData(FALSE);
}

void CDlgArbitrage::ShowCtrlMerchC(bool32 bShow)
{
	GetDlgItem(IDC_STATIC_MERCHC)->ShowWindow(bShow);
	GetDlgItem(IDC_BUTTON_CHOOSEC)->ShowWindow(bShow);
	GetDlgItem(IDC_EDIT_FORMULARC)->ShowWindow(bShow);
	GetDlgItem(IDC_BUTTON_CHANGEC)->ShowWindow(bShow);
}

bool32 CDlgArbitrage::GetMerch(int32 iIndex)
{
	CDlgChooseStockVar::MerchArray aMerchs;
	
	if ( CDlgChooseStockVar::ChooseStockVar(aMerchs, false, NULL, this) )
	{
		// 得到选择的商品		
		if ( 1 != aMerchs.GetSize() )
		{
			//ASSERT(0);
			return false;
		}
		
		CMerch* pMerch = aMerchs.GetAt(0);
		if ( NULL == pMerch )
		{
			return false;
		}

		//
		if ( 0 == iIndex )
		{
			if ( m_stArbitrage.m_MerchA.m_pMerch != pMerch )
			{
				// 选择了不同的商品，应当清除原来存在的公式
				DEL(m_stArbitrage.m_MerchA.m_pFormular);
				m_StrFormularA.Empty();
			}
			m_stArbitrage.m_MerchA.m_pMerch = pMerch;
			m_StrMerchA = pMerch->m_MerchInfo.m_StrMerchCnName;
		}
		else if ( 1 == iIndex )
		{
			if ( m_stArbitrage.m_MerchB.m_pMerch != pMerch )
			{
				// 选择了不同的商品，应当清除原来存在的公式
				DEL(m_stArbitrage.m_MerchB.m_pFormular);
				m_StrFormularB.Empty();
			}
			m_stArbitrage.m_MerchB.m_pMerch = pMerch;
			m_StrMerchB = pMerch->m_MerchInfo.m_StrMerchCnName;
		}
		else if ( 2 == iIndex )
		{
			if ( m_stArbitrage.m_MerchC.m_pMerch != pMerch )
			{
				// 选择了不同的商品，应当清除原来存在的公式
				DEL(m_stArbitrage.m_MerchC.m_pFormular);
				m_StrFormularC.Empty();
			}
			m_stArbitrage.m_MerchC.m_pMerch = pMerch;
			m_StrMerchC = pMerch->m_MerchInfo.m_StrMerchCnName;
		}
		else
		{
			//ASSERT(0);
			return false;
		}
	}

	UpdateData(FALSE);
	return true;
}

bool32 CDlgArbitrage::GetFormular(int32 iIndex)
{
	CDlgArbiFormular Dlg;

	//
	CMerch * pMerch = NULL;
	CArbitrageFormula** pFormular = NULL;
	CString* pStr = NULL;
	CArbitrage::T_ArbMerch *pArbMerch = NULL;

	//
	if ( 0 == iIndex )
	{
		pMerch = m_stArbitrage.m_MerchA.m_pMerch;
		pFormular = &m_stArbitrage.m_MerchA.m_pFormular;
		pStr = &m_StrFormularA;
		pArbMerch = &m_stArbitrage.m_MerchA;
	}
	else if ( 1 == iIndex )
	{
		pMerch = m_stArbitrage.m_MerchB.m_pMerch;
		pFormular = &m_stArbitrage.m_MerchB.m_pFormular;
		pStr = &m_StrFormularB;
		pArbMerch = &m_stArbitrage.m_MerchB;
	}
	else if ( 2 == iIndex )
	{
		pMerch = m_stArbitrage.m_MerchC.m_pMerch;
		pFormular = &m_stArbitrage.m_MerchC.m_pFormular;
		pStr = &m_StrFormularC;
		pArbMerch = &m_stArbitrage.m_MerchC;
	}
	else
	{
		//ASSERT(0);
		return false;
	}

	if ( NULL == pMerch )
	{
		MessageBox(L"请先选择商品!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return false;
	}

	//
	Dlg.SetFormular(pMerch, *pFormular, pArbMerch->m_aOtherMerch);
	
	//
	if ( IDOK == Dlg.DoModal() )
	{
		CString StrFormula = Dlg.GetFormular();
		
		if ( !StrFormula.IsEmpty() )
		{
			if ( NULL == *pFormular )
			{
				// 原来没有
				*pFormular = new CArbitrageFormula();					
			}
			
			Dlg.GetFormula(**pFormular);
			// 插入商品最后做消除重复与多余处理
			CArbitrage::ArbMerchArray aOtherMerch;
			Dlg.GetFormulaOtherMerchs(aOtherMerch);
			pArbMerch->m_aOtherMerch = aOtherMerch;
			
			if ( NULL != pStr )
			{
				*pStr = StrFormula;
			}
		}	
		else
		{
			// 如果公式是空的, 那么把以前的去掉
			DEL(*pFormular);
			*pStr = L"";
		}
	}	

	UpdateData(FALSE);
	return true;
}

bool32 CDlgArbitrage::GeneralArbitrage()
{
	UpdateData(TRUE);

	// 套利模型
	int32 iCurSel = m_ComboArbitrage.GetCurSel();
	if ( iCurSel < 0 )
	{
		//ASSERT(0);
		MessageBox(L"请选择套利模型", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return false;
	}

	m_stArbitrage.m_eArbitrage = (E_Arbitrage)m_ComboArbitrage.GetItemData(iCurSel);

	// 价差模型
	iCurSel = m_ComboPrice.GetCurSel();
	if ( iCurSel < 0 )
	{
		//ASSERT(0);
		MessageBox(L"请选择价差模型", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return false;
	}
	
	m_stArbitrage.m_eArbPrice = (E_ArbPrice)m_ComboPrice.GetItemData(iCurSel);

	//
	if ( NULL == m_stArbitrage.m_MerchA.m_pMerch )
	{
		////ASSERT(0);
		MessageBox(L"请选择商品(A多)", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return false;
	}

	if ( NULL == m_stArbitrage.m_MerchB.m_pMerch )
	{
		//ASSERT(0);
		MessageBox(L"请选择商品(B空)", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return false;
	}

	if ( EABThreeMerch == m_stArbitrage.m_eArbitrage )
	{
		if ( NULL == m_stArbitrage.m_MerchC.m_pMerch )
		{
			//ASSERT(0);
			MessageBox(L"请选择商品(C多)", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
			return false;
		}
	}
	else
	{
		m_stArbitrage.m_MerchC.m_pMerch = NULL;
		DEL(m_stArbitrage.m_MerchC.m_pFormular);
	}

	if ( m_stArbitrage.m_MerchA.m_pMerch == m_stArbitrage.m_MerchB.m_pMerch || m_stArbitrage.m_MerchA.m_pMerch == m_stArbitrage.m_MerchC.m_pMerch || m_stArbitrage.m_MerchB.m_pMerch == m_stArbitrage.m_MerchC.m_pMerch )
	{
		MessageBox(L"不能选择重复商品!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return false;
	}

	if ( m_uiSaveDec < 1 || m_uiSaveDec > 4 )
	{
		MessageBox(L"保留位数应该在 1 到 4 之间!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return false;
	}

	m_stArbitrage.m_iSaveDec = m_uiSaveDec;

	return true;
}

void CDlgArbitrage::OnButtonChooseA() 
{
	GetMerch(0);
}

void CDlgArbitrage::OnButtonChooseB() 
{
	GetMerch(1);	
}

void CDlgArbitrage::OnButtonChooseC() 
{
	GetMerch(2);	
}

void CDlgArbitrage::OnButtonChangeA() 
{
	GetFormular(0);
}

void CDlgArbitrage::OnButtonChangeB() 
{
	GetFormular(1);
}

void CDlgArbitrage::OnButtonChangeC() 
{
	GetFormular(2);
}

void CDlgArbitrage::OnOK() 
{
	if ( !GeneralArbitrage() )
	{
		return;
	}

	if ( m_bAdd )
	{
		CArbitrageManage::Instance()->AddArbitrage(m_stArbitrage);
	}
	else
	{
		CArbitrageManage::Instance()->ModifyArbitrage(m_stArbitrageBk, m_stArbitrage);
	}

	CDialog::OnOK();
}

void CDlgArbitrage::OnSelchangeComboArbitrageType() 
{
	int32 iCurSel = m_ComboArbitrage.GetCurSel();

	E_Arbitrage eType = (E_Arbitrage)m_ComboArbitrage.GetItemData(iCurSel);
	
	if ( EABTwoMerch == eType )
	{
		ShowCtrlMerchC(false);
	}
	else if ( EABThreeMerch == eType )
	{
		ShowCtrlMerchC(true);
	}
	else
	{
		//ASSERT(0);
	}
}

void CDlgArbitrage::OnChangeEditSavedec() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if ( m_uiSaveDec < 1 || m_uiSaveDec > 4 )
	{
		m_uiSaveDec = 2;	
	}
	
	UpdateData(FALSE);
}
