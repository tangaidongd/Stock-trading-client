// dlgalarmformular.cpp : implementation file
//

#include "stdafx.h"
#include "dlgalarmformular.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAlarmFormular dialog


CDlgAlarmFormular::CDlgAlarmFormular(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgAlarmFormular::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAlarmFormular)
		// NOTE: the ClassWizard will add member initialization here
	m_StrNameP1 = _T("");
	m_StrNameP2 = _T("");
	m_StrNameP3 = _T("");
	m_StrNameP4 = _T("");	
	m_fP1 = 0;
	m_fP2 = 0.0f;
	m_fP3 = 0.0f;
	m_fP4 = 0.0f;
	m_StrRangeP1 = _T("");
	m_StrRangeP2 = _T("");
	m_StrRangeP3 = _T("");
	m_StrRangeP4 = _T("");
	m_iRadio = 0;
	//}}AFX_DATA_INIT

	m_pFomularNow = NULL;
}


void CDlgAlarmFormular::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAlarmFormular)
	DDX_Control(pDX, IDC_COMBO1, m_ComboFormular);	
	DDX_Radio(pDX, IDC_RADIO_P1, m_iRadio);
	DDX_Text(pDX, IDC_STATIC_PARAM1, m_StrNameP1);
	DDX_Text(pDX, IDC_STATIC_PARAM2, m_StrNameP2);
	DDX_Text(pDX, IDC_STATIC_PARAM3, m_StrNameP3);
	DDX_Text(pDX, IDC_STATIC_PARAM4, m_StrNameP4);	
	DDX_Text(pDX, IDC_EDIT_PARAM, m_fP1);
	DDX_Text(pDX, IDC_EDIT_PARAM2, m_fP2);
	DDX_Text(pDX, IDC_EDIT_PARAM3, m_fP3);
	DDX_Text(pDX, IDC_EDIT_PARAM4, m_fP4);
	DDX_Text(pDX, IDC_STATIC_RANGE1, m_StrRangeP1);
	DDX_Text(pDX, IDC_STATIC_RANGE2, m_StrRangeP2);
	DDX_Text(pDX, IDC_STATIC_RANGE3, m_StrRangeP3);
	DDX_Text(pDX, IDC_STATIC_RANGE4, m_StrRangeP4);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAlarmFormular, CDialogEx)
	//{{AFX_MSG_MAP(CDlgAlarmFormular)
	ON_BN_CLICKED(IDC_RADIO_P1, OnRadioP1)
	ON_BN_CLICKED(IDC_RADIO_P2, OnRadioP2)
	ON_BN_CLICKED(IDC_RADIO_P3, OnRadioP3)
	ON_BN_CLICKED(IDC_RADIO_P4, OnRadioP4)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	ON_EN_CHANGE(IDC_EDIT_PARAM, OnChangeEditParam)
	ON_EN_CHANGE(IDC_EDIT_PARAM2, OnChangeEditParam2)
	ON_EN_CHANGE(IDC_EDIT_PARAM4, OnChangeEditParam4)
	ON_EN_CHANGE(IDC_EDIT_PARAM3, OnChangeEditParam3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAlarmFormular message handlers
BOOL CDlgAlarmFormular::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//
	if ( NULL != m_pFomularNow )
	{
		m_ComboFormular.EnableWindow(FALSE);
	}

	//
	FillComBoxFormular();
	
	//
	int32 iCurSel = 0;

	if ( NULL == m_pFomularNow )
	{
		m_ComboFormular.SetCurSel(iCurSel);	
	}
	else
	{
		for ( int32 i = 0; i < m_ComboFormular.GetCount(); i++ )
		{
			CFormularContent* pFormularNow = (CFormularContent*)m_ComboFormular.GetItemData(i);
			
			if ( pFormularNow == m_pFomularNow )
			{
				iCurSel = i;
				break;
			}
		}

		//
		m_ComboFormular.SetCurSel(iCurSel);
	}
	
	//
	OnSelchangeCombo1();

	//
	m_ComboFormular.SetFocus();
	return TRUE;
}

void CDlgAlarmFormular::FillComBoxFormular()
{
	// 取得所有的条件选股公式
	CFormulaLib* pFormulaLib = CFormulaLib::instance();
	if ( NULL == pFormulaLib )
	{
		//ASSERT(0);
		return;
	}
	
	//
	CArray<CFormularContent*, CFormularContent*> aFormulars;
	pFormulaLib->GetFomulars(EFTCdtChose, true, aFormulars);
	
	for ( int32 i = 0; i < aFormulars.GetSize(); i++ )
	{
		CFormularContent* pFormular = aFormulars.GetAt(i);
		
		if ( NULL == pFormular )
		{
			continue;
		}
		
		//
		if ( NULL != m_pFomularNow && pFormular->name == m_pFomularNow->name )
		{
			pFormular = m_pFomularNow;
		}

		//
		CString StrText;
		StrText.Format(L"%s - %s", pFormular->name.GetBuffer(), pFormular->explainBrief.GetBuffer());
		
		//
		m_ComboFormular.InsertString(-1, StrText);
		m_ComboFormular.SetItemData(i, (DWORD)pFormular);
	}	
	
	//
	if ( m_ComboFormular.GetCount() > 0 )
	{
		m_ComboFormular.SetCurSel(0);
	}	
}

void CDlgAlarmFormular::SetRadioParam(int32 iIndex)
{
	if ( iIndex < 0 || iIndex > 3 || NULL == m_pFomularNow )
	{
		return;
	}
	
	//
	m_StrNameP1.Empty();
	m_StrNameP2.Empty();
	m_StrNameP3.Empty();
	m_StrNameP4.Empty();
	
	m_StrRangeP1.Empty();
	m_StrRangeP2.Empty();
	m_StrRangeP3.Empty();
	m_StrRangeP4.Empty();
	
	m_fP1 = 0.;
	m_fP2 = 0.;
	m_fP3 = 0.;
	m_fP4 = 0.;
	
	//
	GetDlgItem(IDC_EDIT_PARAM)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PARAM2)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PARAM3)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PARAM4)->EnableWindow(FALSE);
	
	//
	if ( m_pFomularNow->numPara <= 0 )
	{
		// 不能设置参数			
		UpdateData(FALSE);	
		return;
	}
	
	CString* StrNames[4]	= { &m_StrNameP1, &m_StrNameP2, &m_StrNameP3, &m_StrNameP4 };
	CButton* pButtons[4]	= { (CButton*)GetDlgItem(IDC_EDIT_PARAM), (CButton*)GetDlgItem(IDC_EDIT_PARAM2), (CButton*)GetDlgItem(IDC_EDIT_PARAM3), (CButton*)GetDlgItem(IDC_EDIT_PARAM4)};
	float* fParams[4]		= { &m_fP1, &m_fP2, &m_fP3, &m_fP4 };
	CString* StrRanges[4]	= { &m_StrRangeP1, &m_StrRangeP2, &m_StrRangeP3, &m_StrRangeP4 };
	
	//
	int32 iIndexBeing = iIndex * 4;
	
	for ( int32 iIndexCtrl = 0, iIndexParam = iIndexBeing; iIndexParam < m_pFomularNow->numPara; iIndexCtrl++, iIndexParam++)
	{
		*(StrNames[iIndexCtrl]) = m_pFomularNow->namePara[iIndexParam];
		*(fParams[iIndexCtrl])  = m_pFomularNow->defaultVal[iIndexParam];
		
		CString StrRange;
		StrRange.Format(L"%.2f-%.2f", m_pFomularNow->min[iIndexParam], m_pFomularNow->max[iIndexParam]);
		*(StrRanges[iIndexCtrl]) = StrRange;
		
		pButtons[iIndexCtrl]->EnableWindow(TRUE);		
	}
	
	//
	m_iRadio = iIndex;
	UpdateData(FALSE);
}

void CDlgAlarmFormular::OnRadioP1() 
{
	SetRadioParam(0);
}

void CDlgAlarmFormular::OnRadioP2() 
{
	SetRadioParam(1);	
}

void CDlgAlarmFormular::OnRadioP3() 
{
	SetRadioParam(2);	
}

void CDlgAlarmFormular::OnRadioP4() 
{
	SetRadioParam(3);	
}

void CDlgAlarmFormular::OnSelchangeCombo1() 
{
	int32 iCurSel = m_ComboFormular.GetCurSel();
	if ( iCurSel < 0 || iCurSel >= m_ComboFormular.GetCount() )
	{
		return;
	}
	
	//
	m_pFomularNow = (CFormularContent*)m_ComboFormular.GetItemData(iCurSel);
	if ( NULL == m_pFomularNow )
	{
		return;
	}
	
	//
	SetRadioParam(0);
	((CButton*)GetDlgItem(IDC_RADIO_P1))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_RADIO_P2))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_P3))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_P4))->SetCheck(0);	
}

void CDlgAlarmFormular::ModifyFormular(int32 iIndexParam)
{
	// 
	UpdateData(TRUE);
	
	//
	if ( m_iRadio < 0 || m_iRadio > 3 || iIndexParam < 0 || iIndexParam > 3 )
	{
		//ASSERT(0);
		return;
	}
	
	if ( NULL == m_pFomularNow )
	{
		//ASSERT(0);
		return;
	}
	
	//
	float fNew = m_fP1;
	
	if ( 0 == iIndexParam )
	{
		fNew = m_fP1;
	}
	else if ( 1 == iIndexParam )
	{
		fNew = m_fP2;
	}
	else if ( 2 == iIndexParam )
	{
		fNew = m_fP3;
	}
	else if ( 3 == iIndexParam )
	{
		fNew = m_fP4;
	}
	
	//
	int32 iIndexFomularParam = 4 * m_iRadio + iIndexParam;
	if ( iIndexFomularParam < 0 || iIndexFomularParam >= m_pFomularNow->numPara )
	{
		//ASSERT(0);
		return;
	}
	
	fNew = fNew > m_pFomularNow->max[iIndexFomularParam] ? m_pFomularNow->max[iIndexFomularParam] : fNew;
	fNew = fNew < m_pFomularNow->min[iIndexFomularParam] ? m_pFomularNow->min[iIndexFomularParam] : fNew;
	
	//
	m_pFomularNow->defaultVal[iIndexFomularParam] = fNew;	
	
	//
	if ( 0 == iIndexParam )
	{
		m_fP1 = fNew;
	}
	else if ( 1 == iIndexParam )
	{
		m_fP2 = fNew;
	}
	else if ( 2 == iIndexParam )
	{
		m_fP3 = fNew;
	}
	else if ( 3 == iIndexParam )
	{
		m_fP4 = fNew;
	}
	
	UpdateData(FALSE);
}

void CDlgAlarmFormular::OnOK() 
{
	CDialog::OnOK();
}

void CDlgAlarmFormular::OnChangeEditParam() 
{
	ModifyFormular(0);
}

void CDlgAlarmFormular::OnChangeEditParam2() 
{
	ModifyFormular(1);
}

void CDlgAlarmFormular::OnChangeEditParam3() 
{
	ModifyFormular(2);
}

void CDlgAlarmFormular::OnChangeEditParam4() 
{
	ModifyFormular(3);
}

