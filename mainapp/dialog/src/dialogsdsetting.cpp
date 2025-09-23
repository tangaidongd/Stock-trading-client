// dialogsdsetting.cpp : implementation file
//

#include "stdafx.h"

#include "GmtTime.h"
#include "tinyxml.h"
#include "region.h"
#include "ChartRegion.h"
#include "IoViewShare.h"
#include "SelfDraw.h"
#include "dialogsdsetting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogSDSetting dialog


CDialogSDSetting::CDialogSDSetting(CSelfDrawNode*& pNode,T_MerchNodeUserData* pData,double fYMin,double fYMax,CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogSDSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogSDSetting)
	m_val1 = 0.0f;
	m_val2 = 0.0f;
	m_val3 = 0.0f;
	//}}AFX_DATA_INIT
	m_pNode		= pNode;
	m_pData		= pData;
	m_fYMax		= fYMax;
	m_fYMin		= fYMin;
	m_iSel1		= 0;
	m_iSel2		= 0;
	m_iSel3		= 0;
	m_bStart	= true;
	m_bEnd		= true;
	m_bAdd		= true;
}

void CDialogSDSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogSDSetting)
	DDX_Control(pDX, IDC_COMBO3, m_list3);
	DDX_Control(pDX, IDC_COMBO2, m_list2);
	DDX_Control(pDX, IDC_COMBO1, m_list1);
	DDX_Text(pDX, IDC_EDIT1, m_val1);
	DDX_Text(pDX, IDC_EDIT2, m_val2);
	DDX_Text(pDX, IDC_EDIT3, m_val3);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogSDSetting, CDialogEx)
	//{{AFX_MSG_MAP(CDialogSDSetting)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO3, OnSelchangeCombo3)
	ON_CBN_SELCHANGE(IDC_COMBO2, OnSelchangeCombo2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogSDSetting message handlers

BOOL CDialogSDSetting::OnInitDialog() 
{
	CDialog::OnInitDialog();

	switch ( m_pNode->m_eType )
	{
	case ESDTLine:
		m_bAdd = false;
		break;
	case ESDTLineEx:
		m_bAdd = false;
		break;
	case ESDTJgtd:

		break;
	case ESDTPxzx:

		break;
	case ESDTYhx:
		m_bAdd = false;
		break;
	case ESDTHjfg:
		m_bAdd = false;	
		break;
	case ESDTBfb:
		m_bAdd = false;
		break;
	case ESDTBd:
		m_bAdd = false;
		break;
	case ESDTJx:
		m_bAdd = false;
		break;
	case ESDTJxhgd:
		m_bAdd = false;
		break;
	case ESDTYcxxhgd:
		m_bAdd = false;
		break;
	case ESDTXxhgd:
		m_bAdd = false;
		break;
	case ESDTZq:
		m_bAdd = false;
		break;
	case ESDTFblq:
		m_bEnd = false;
		m_bAdd = false;
		break;
	case ESDTZs:
		m_bAdd = false;
		break;
	case ESDTJejd:
		m_bAdd = false;
		break;
	case ESDTZbj:
		m_bEnd = false;
		m_bAdd = false;
		break;
	case ESDTDbj:
		m_bEnd = false;
		m_bAdd = false;
		break;
	case ESDTText:
		m_bEnd = false;
		m_bAdd = false;
		break;
	default:
		break;
	}
	m_iSel1 = FillList(&m_list1,m_pNode->m_id1);
	m_list1.SetCurSel(m_iSel1);
	m_val1 = m_pNode->m_fVal1;
	if ( !m_bEnd )
	{
		GetDlgItem(IDC_COMBO2)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT2)->EnableWindow(FALSE);
	}
	else
	{
		m_iSel2 = FillList(&m_list2,m_pNode->m_id2);
		m_list2.SetCurSel(m_iSel2);
		m_val2 = m_pNode->m_fVal2;
	}

	if ( !m_bAdd )
	{
		GetDlgItem(IDC_COMBO3)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT3)->EnableWindow(FALSE);
	}
	else
	{
		m_iSel3 = FillList(&m_list3,m_pNode->m_ide);
		m_list3.SetCurSel(m_iSel3);
		m_val3 = m_pNode->m_fVale;
	}

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
int32 CDialogSDSetting::FillList (CComboBox* pCombo,int32 iSelVal )
{
	CNodeSequence* pNodes = m_pData->m_pKLinesShow;

	int32 i,iSize = pNodes->GetSize();
	int32 iSel = 0;
	for ( i = 0; i < iSize; i ++ )
	{
		CNodeData NodeData;
		pNodes->GetAt(i,NodeData);
		if ( NodeData.m_iID == iSelVal )
		{
			iSel = i;
		}
		CGmtTime t(NodeData.m_iID);
		CString StrTime = Time2String(t,m_pData->m_eTimeIntervalFull);
		//StrTime.Format(_T("%04d年%02d月%02d日%02d时%02d分%02d秒"),t.GetYear(),t.GetMonth(),t.GetDay(),t.GetHour(),t.GetMinute(),t.GetSecond());
		pCombo->AddString(StrTime);
	}
	return iSel;
}
void CDialogSDSetting::OnSelchangeCombo1() 
{
	m_iSel1 = m_list1.GetCurSel();
}
void CDialogSDSetting::OnSelchangeCombo2() 
{
	m_iSel2 = m_list2.GetCurSel();
}

void CDialogSDSetting::OnSelchangeCombo3() 
{
	m_iSel3 = m_list3.GetCurSel();
}


void CDialogSDSetting::OnOK() 
{
	if ( m_val1 < m_fYMin )
	{
		m_val1 = m_fYMin;
	}
	if ( m_val1 > m_fYMax )
	{
		m_val1 = m_fYMax;
	}

	if ( m_val2 < m_fYMin )
	{
		m_val2 = m_fYMin;
	}
	if ( m_val2 > m_fYMax )
	{
		m_val2 = m_fYMax;
	}

	if ( m_val3 < m_fYMin )
	{
		m_val3 = m_fYMin;
	}
	if ( m_val3 > m_fYMax )
	{
		m_val3 = m_fYMax;
	}

	m_pNode->m_fVal1 = m_val1;
	if ( m_bEnd )
	{
		m_pNode->m_fVal2 = m_val2;
	}
	if ( m_bAdd )
	{
		m_pNode->m_fVale = m_val3;
	}

	CNodeSequence* pNodes = m_pData->m_pKLinesShow;
	int32 iSize = pNodes->GetSize();
	CNodeData NodeData;
	if ( m_iSel1 >= 0 && m_iSel1 < iSize )
	{
		pNodes->GetAt(m_iSel1,NodeData);
		m_pNode->m_id1 = NodeData.m_iID;
	}
	if ( m_bEnd )
	{
		if ( m_iSel2 >= 0 && m_iSel2 < iSize )
		{
			pNodes->GetAt(m_iSel2,NodeData);
			m_pNode->m_id2 = NodeData.m_iID;
		}
	}
	if ( m_bAdd )
	{
		if ( m_iSel3 >= 0 && m_iSel3 < iSize )
		{
			pNodes->GetAt(m_iSel3,NodeData);
			m_pNode->m_ide = NodeData.m_iID;
		}
	}
	CDialog::OnOK();
}
