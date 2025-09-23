// dlgarbiformular.cpp : implementation file
//

#include "stdafx.h"

#include "dlgarbiformular.h"
#include "ArbitrageFormula.h"
#include "MerchManager.h"
#include "DlgChooseStockVar.h"


#include "MerchManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgArbiFormular dialog
CDlgArbiFormular::MerchMap CDlgArbiFormular::m_smapUserInsertedMerch;

CDlgArbiFormular::CDlgArbiFormular(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgArbiFormular::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgArbiFormular)
	m_StrFormular = _T("");
	//}}AFX_DATA_INIT
	m_pMerch	= NULL;
}


void CDlgArbiFormular::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgArbiFormular)
	DDX_Text(pDX, IDC_EDIT_FORMULAR, m_StrFormular);
	DDX_Control(pDX, IDC_EDIT_FORMULAR, m_edtFormula);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgArbiFormular, CDialogEx)
	//{{AFX_MSG_MAP(CDlgArbiFormular)
	ON_BN_CLICKED(IDC_BUTTON_VALID, OnButtonValid)
	ON_BN_CLICKED(IDC_BUTTON_INSERTMERCH, OnButtonInsertOtherMerch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgArbiFormular message handlers
BOOL CDlgArbiFormular::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if ( !m_formula.GetFormula().IsEmpty() ) 
	{
		m_StrFormular = m_formula.GetFormula();
	}
	else
	{
		if ( NULL != m_pMerch )
		{
			m_StrFormular = m_pMerch->GetMerchVarName();
		}
	}

	UpdateData(FALSE);

	//
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_FORMULAR);
	if ( NULL != pEdit )
	{
		pEdit->SetFocus();
		pEdit->SetSel(m_StrFormular.GetLength(), m_StrFormular.GetLength());			
	}

	//
	return FALSE;
}

bool32 CDlgArbiFormular::CheckFormula(CArbitrageFormula &formula)
{
	m_aOtherMerch.clear();

	//
	ArbitrageVarPtrs vars;
	if ( !formula.TestFormula() )
	{
		return false;
	}

	formula.GetUnInitializedVariables(vars);

	ArbitrageVarPtrs::iterator it;
	for ( it=vars.begin(); it!=vars.end(); ++it )
	{
		const CString &StrVar = it->first;
		CMerch *pMerch = GetMerchByVarName(StrVar);
		if ( NULL!=pMerch )
		{
			if ( pMerch!=m_pMerch )
			{
				m_aOtherMerch.push_back( pMerch );	// 存下来这个编入了公式的商品
			}
			it->second = new CArbitrageFormulaVariableVar;	// 填一个表示我知道了的参数
		}
	}
	
	bool32 bParse = formula.TestFormula(vars);

	for ( it=vars.begin(); it != vars.end(); ++it )
	{
		if ( NULL != it->second )
		{
			CArbitrageFormulaVariableVar *pVar = it->second;
			delete pVar;
			it->second = NULL;	// 清空该变量的指针
		}
	}
	vars.clear();

	return bParse;
}

void CDlgArbiFormular::OnButtonValid() 
{
	UpdateData(TRUE);

	m_formula.SetFormula(m_StrFormular);
	bool32 bParse = CheckFormula(m_formula);
	if ( bParse )
	{
		MessageBox(L"公式合法!", AfxGetApp()->m_pszAppName, MB_OK);
	}
	else
	{
		CString StrPrompt;
		CString StrErr = m_formula.GetLastErrorText();
		StrPrompt.Format(L"错误信息:\n%s 位置:%d!", StrErr.GetBuffer(), m_formula.GetErrorPos());
		MessageBox(StrPrompt, AfxGetApp()->m_pszAppName, MB_ICONWARNING);	
	}
}

CString CDlgArbiFormular::GetFormular()
{
	return m_StrFormular;
}

void CDlgArbiFormular::OnOK() 
{
	UpdateData(TRUE);

	//
	if ( !m_StrFormular.IsEmpty() )
	{	
		m_formula.SetFormula(m_StrFormular);
		if ( !CheckFormula(m_formula) )
		{
			CString StrErr;
			CString StrLastErr = m_formula.GetLastErrorText();
			StrErr.Format(_T("公式非法, 不会保存, 是否继续？\r\n错误内容: %s\r\n位置:%d"), StrLastErr.GetBuffer(), m_formula.GetErrorPos());
			if ( IDYES == MessageBox(StrErr, AfxGetApp()->m_pszAppName, MB_YESNO) )
			{
				m_StrFormular.Empty();
				CDialog::OnCancel();
				return;
			}
			else
			{
				return;
			}
		}
	}

	CDialog::OnOK();
}

void CDlgArbiFormular::SetFormular( CMerch* pMerch, CArbitrageFormula* pFormular, const CArbitrage::ArbMerchArray &aOtherMerch )
{
	// 对话框ok时，必定先checkformula，从而重新保存所有分析得到的othermerch
	m_aOtherMerch = aOtherMerch;

	m_pMerch = pMerch;
	if ( NULL==pFormular )
	{
		m_formula = CArbitrageFormula();
	}
	else
	{
		m_formula = *pFormular;
	}
	if ( NULL!=m_pMerch )
	{
		CString StrVarName = m_pMerch->GetMerchVarName();
		m_formula.AddSpecialVarName( StrVarName );
	}

	int iSize = (int)m_aOtherMerch.size();
	for ( int i=0; i < iSize; ++i )
	{
		CMerch *pOtherMerch = m_aOtherMerch[i];
		if (NULL == pOtherMerch)
		{
			continue;
		}

		CString StrVarName = pOtherMerch->GetMerchVarName();
		m_formula.AddSpecialVarName( StrVarName );
	}
}

void CDlgArbiFormular::OnButtonInsertOtherMerch()
{
	CDlgChooseStockVar::MerchArray aMerchs;
	if ( CDlgChooseStockVar::ChooseStockVar(aMerchs, false)
		&& aMerchs.GetSize() > 0 )
	{
		CMerch *pMerch = aMerchs[0];

		int iStart, iEnd;
		iStart = iEnd = -1;
		m_edtFormula.GetSel(iStart, iEnd);
		
		m_edtFormula.SetFocus();

		CString StrVarName = pMerch->GetMerchVarName();

		CString StrName = _T("(") + StrVarName + _T(")");
		m_edtFormula.ReplaceSel(StrName, TRUE);

		m_smapUserInsertedMerch[StrVarName] = pMerch; 
		if ( pMerch != m_pMerch )
		{
			m_formula.AddSpecialVarName( StrVarName );
		}
	}
}

CMerch		* CDlgArbiFormular::GetMerchByVarName( const CString &StrVar )
{
	MerchMap::iterator it = m_smapUserInsertedMerch.find(StrVar);
	if ( it != m_smapUserInsertedMerch.end() )
	{
		return it->second;
	}

	CGGTongDoc *pDoc = AfxGetDocument();
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	CMerchManager &manager = pAbsCenterManager->GetMerchManager();

	CMerch *pMerchFound = NULL;
	pMerchFound = NULL;
	
	for ( int32 i = 0; i < manager.m_BreedListPtr.GetSize(); i++ )
	{
		CBreed* pBreed = manager.m_BreedListPtr[i];
		if ( NULL == pBreed )
		{
			continue;
		}
		
		for ( int32 j = 0; j < pBreed->m_MarketListPtr.GetSize(); j++ )
		{
			CMarket* pMarket = pBreed->m_MarketListPtr[j];
			if ( NULL == pMarket )
			{
				continue;
			}
			
			//
			for ( int32 k=0; k < pMarket->m_MerchsPtr.GetSize() ; ++k )
			{
				CMerch *pMerch = pMarket->m_MerchsPtr[k];
				CString StrVarName = pMerch->GetMerchVarName();

				if ( NULL!=pMerch && StrVarName.CollateNoCase(StrVar)==0 )
				{
					pMerchFound = pMerch;
					m_smapUserInsertedMerch[ StrVar ] = pMerch;	// 缓存下
					return pMerchFound;
				}
			}
		}
	}
	return NULL;
}

void CDlgArbiFormular::GetFormulaOtherMerchs( OUT CArbitrage::ArbMerchArray &aMerchs )
{
	aMerchs = m_aOtherMerch;
}

