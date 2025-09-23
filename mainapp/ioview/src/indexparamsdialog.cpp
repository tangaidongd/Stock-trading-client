// indexparamsdialog.cpp : implementation file
//
#include "stdafx.h"
#include "ShareFun.h"
#include "indexparamsdialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIndexParamsDialog dialog

CIndexParamsDialog::CIndexParamsDialog(CFormularContent * pRet2 , CFormularContent* pRet1,CWnd* pParent /*=NULL*/)
	: CDialogEx(CIndexParamsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIndexParamsDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pContentRet1 = pRet1;
	m_pContentRet2 = pRet2;

	pIndexParam = NULL;
	m_bInit		= true;
	m_iRet		= 0;

	m_nEID[0][0] = IDC_EDIT11;
	m_nEID[0][1] = IDC_EDIT12;
	m_nEID[0][2] = IDC_EDIT13;
	m_nEID[0][3] = IDC_EDIT14;
	m_nEID[0][4] = IDC_EDIT15;
	
	m_nEID[1][0] = IDC_EDIT21;
	m_nEID[1][1] = IDC_EDIT22;
	m_nEID[1][2] = IDC_EDIT23;
	m_nEID[1][3] = IDC_EDIT24;
	m_nEID[1][4] = IDC_EDIT25;
	
	m_nEID[2][0] = IDC_EDIT31;
	m_nEID[2][1] = IDC_EDIT32;
	m_nEID[2][2] = IDC_EDIT33;
	m_nEID[2][3] = IDC_EDIT34;
	m_nEID[2][4] = IDC_EDIT35;
	
	m_nEID[3][0] = IDC_EDIT41;
	m_nEID[3][1] = IDC_EDIT42;
	m_nEID[3][2] = IDC_EDIT43;
	m_nEID[3][3] = IDC_EDIT44;
	m_nEID[3][4] = IDC_EDIT45;

	m_iLastSel = -1;
}

CIndexParamsDialog::~CIndexParamsDialog()
{
	if ( NULL != pIndexParam )
	{
		delete pIndexParam;
		pIndexParam = NULL;
	}
}

void CIndexParamsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIndexParamsDialog)
	DDX_Control(pDX, IDC_EDIT11, m_floatEdit11);
	DDX_Control(pDX, IDC_EDIT12, m_floatEdit12);
	DDX_Control(pDX, IDC_EDIT13, m_floatEdit13);
	DDX_Control(pDX, IDC_EDIT14, m_floatEdit14);
	DDX_Control(pDX, IDC_EDIT15, m_floatEdit15);

	DDX_Control(pDX, IDC_EDIT21, m_floatEdit21);
	DDX_Control(pDX, IDC_EDIT22, m_floatEdit22);
	DDX_Control(pDX, IDC_EDIT23, m_floatEdit23);
	DDX_Control(pDX, IDC_EDIT24, m_floatEdit24);
	DDX_Control(pDX, IDC_EDIT25, m_floatEdit25);

	DDX_Control(pDX, IDC_EDIT31, m_floatEdit31);
	DDX_Control(pDX, IDC_EDIT32, m_floatEdit32);
	DDX_Control(pDX, IDC_EDIT33, m_floatEdit33);
	DDX_Control(pDX, IDC_EDIT34, m_floatEdit34);
	DDX_Control(pDX, IDC_EDIT35, m_floatEdit35);

	DDX_Control(pDX, IDC_EDIT41, m_floatEdit41);
	DDX_Control(pDX, IDC_EDIT42, m_floatEdit42);
	DDX_Control(pDX, IDC_EDIT43, m_floatEdit43);
	DDX_Control(pDX, IDC_EDIT44, m_floatEdit44);
	DDX_Control(pDX, IDC_EDIT45, m_floatEdit45);
	DDX_Control(pDX, IDC_TAB1, m_tabParam);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIndexParamsDialog, CDialogEx)
	//{{AFX_MSG_MAP(CIndexParamsDialog)
		// NOTE: the ClassWizard will add message map macros here
	ON_BN_CLICKED(IDB_SAVEAS_DEFAULT, OnSaveasDefault)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnSelchangeTab1)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIndexParamsDialog message handlers
BOOL CIndexParamsDialog::OnInitDialog() 
{
	HICON hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,FALSE);

	CString StrText;
	StrText.Format(_T("调整指标参数[%s]"),m_pContentRet1->name.GetBuffer());
	SetWindowText(StrText);

	pIndexParam = new CIndexParamData;

	pIndexParam->name	= m_pContentRet1->name;
	pIndexParam->numPara	= m_pContentRet1->numPara;
	IndexContentToParam(m_pContentRet1,pIndexParam);

	CDialog::OnInitDialog();

	m_tabParam.InsertItem (0,_T("参数1-4"));
	m_tabParam.InsertItem (1,_T("参数5-8"));
	m_tabParam.InsertItem (2,_T("参数9-12"));
	m_tabParam.InsertItem (3,_T("参数13-16"));
	m_tabParam.SetCurSel(0);
	m_iLastSel = -1;

	UpdateSel();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CIndexParamsDialog::UpdateParam()
{
	CFormularContent * pContent = NULL;
	if ( 1 == m_iRet)
	{
		pContent = m_pContentRet1;
	}
	else if ( 2 == m_iRet )
	{
		pContent = m_pContentRet2;
	}

	if (NULL == pContent)
	{
		return;
	}

	UpdateData();
	CString StrText;
	int i, n ,iSel;
	iSel = m_tabParam.GetCurSel();
	n = iSel * 4;

	if(NULL == pIndexParam)
	{
		return;
	}

	//保存到IndexParam中
	for ( i = 0; i < 4; i ++ )
	{
		if ( n + i >= pIndexParam->numPara )
		{
			break;
		}
		GetStringParam ( i, 0, pIndexParam->namePara[n+i]);
		GetStringParam ( i, 4, pIndexParam->descPara[n+i]);
		
		GetStringParam ( i, 2, StrText);
		pIndexParam->min[n+i] = StringToFloat(StrText);
		
		GetStringParam ( i, 1, StrText);
		pIndexParam->max[n+i] = StringToFloat(StrText);
		
		GetStringParam ( i, 3, StrText);
		
		float fValue = StringToFloat(StrText);
		float fMin   = pIndexParam->min[n+i];
		float fMax   = pIndexParam->max[n+i];
		
		if ( fValue < fMin )
		{
			fValue = fMin;
		}
		
		if ( fValue > fMax)
		{
			fValue = fMax;
		}
		
		if ( -1 != pContent->name.Find(L"MA"))
		{
			// MA 的话不能小于 1
			if ( fValue <= float(1.0))
			{
				// 先跟最小值比较
				if ( fMin > float(1.0))
				{
					fValue = fMin;
				}
				else
				{
					fValue = float(2.0);
				}			
			}
		}
		else
		{
			// 其他的不能小于 0
			
			if (fValue <= float(0.0))
			{
				if ( fMin > float(0.0))
				{
					fValue = fMin;
				}
				else
				{
					fValue = float(1.0);
				}				
			}
		}
		
		pIndexParam->defaultVal[n+i] = fValue;
	}
	
	IndexParamToContent(pIndexParam,pContent);


	if ( 2 == m_iRet )
	{
		T_IndexMapPtr* p;
		CFormulaLib* pLib = CFormulaLib::instance();

		//if ( pContent->m_bOften )
		//{
		//	pLib->m_Flags.AddOften(pContent->name);
		//}
		//else
		//{
		//	pLib->m_Flags.DelOften(pContent->name);
		//}
		//if ( pContent->m_bProtected )
		//{
		//	pLib->m_Flags.AddProtected(pContent->name);
		//}
		//else
		//{
		//	pLib->m_Flags.DelProtected(pContent->name);
		//}
		
//		CIndexParamData* pParam;
		//用户自编
		p = pLib->m_UserIndex.Get(pContent->name);
		if ( NULL != p )
		{
			pLib->m_UserIndex.Modify(pContent);
//			pParam = pLib->m_UserParams.GetParam(pContent->name);
// 			if ( NULL != pParam )
// 			{
// 				IndexContentToParam(pContent,pParam);
// 				pLib->m_UserParams.Modify(pParam);
// 			}
		}
		else
		{
			//用户之前修改过的系统指标
			p = pLib->m_ModifyIndex.Get(pContent->name);
			if ( NULL != p )
			{
				pLib->m_ModifyIndex.Modify(pContent);
// 				pParam = pLib->m_ModifyParams.GetParam(pContent->name);
// 				if ( NULL != pParam )
// 				{
// 					IndexContentToParam(pContent,pParam);
// 					pLib->m_ModifyParams.Modify(pParam);
// 				}
			}
			else
			{
				//还未修改过的系统指标
				CString StrGroup1,StrGroup2;
				if ( pLib->m_SysIndex.GetGroup(pContent,StrGroup1,StrGroup2) )
				{
					pLib->m_ModifyIndex.Add(StrGroup1,StrGroup2,pContent);
// 					pParam = pLib->m_SysParams.GetParam(pContent->name);
// 					if ( NULL != pParam )
// 					{
// 						IndexContentToParam(pContent,pParam);
// 						pLib->m_ModifyParams.Modify(pParam);
// 					}
				}			
			}
		}
	}
}
void CIndexParamsDialog::OnSaveasDefault() 
{
	m_iRet = 2;
	UpdateParam();

	//CFormulaLib* pLib = CFormulaLib::instance();
//	if ( NULL != pLib->m_UserParams.GetParam(pIndexParam->name))
//	{
//		pLib->m_UserParams.Modify(pIndexParam);
//	}
// 	else
// 	{
// 		if ( NULL != pLib->m_ModifyParams.GetParam(pIndexParam->name))
// 		{
// 			pLib->m_ModifyParams.Modify(pIndexParam);
// 		}
// 		else
// 		{
// 			if ( NULL != pLib->m_SysParams.GetParam(pIndexParam->name))
// 			{
// 				pLib->m_SysParams.Modify(pIndexParam);
// 			}
// 		}
// 	}

	CDialog::OnOK();
}

void CIndexParamsDialog::OnOK() 
{
	m_iRet = 1;
	UpdateParam();

	CDialog::OnOK();
}

void CIndexParamsDialog::UpdateSel()
{
	UpdateData();
	CString StrText;
	int i, n, iSel = m_tabParam.GetCurSel();
	if ( iSel == m_iLastSel )
	{
		return;
	}
	if(NULL == pIndexParam)
	{
		return;
	}
	if ( m_bInit )
	{
		m_bInit = false;
	}
	else
	{
		n = m_iLastSel * 4;
		//保存到IndexParam中
		for ( i = 0; i < 4; i ++ )
		{
			if ( n + i >= pIndexParam->numPara )
			{
				break;
			}
			if(n + i < 0)
			{
				break;
			}
			GetStringParam ( i, 0, pIndexParam->namePara[n+i]);
			GetStringParam ( i, 4, pIndexParam->descPara[n+i]);

			GetStringParam ( i, 2, StrText);
			pIndexParam->min[n+i] = StringToFloat(StrText);

			GetStringParam ( i, 1, StrText);
			pIndexParam->max[n+i] = StringToFloat(StrText);

			GetStringParam ( i, 3, StrText);
			pIndexParam->defaultVal[n+i] = StringToFloat(StrText);
		}
	}
	//从IndexParam中取出
	n = iSel * 4;
	for ( i = 0; i < 4; i ++ )
	{
		if ( n + i >= pIndexParam->numPara )
		{
			//清空
			SetStringParam ( i, 0, _T(""));
			SetStringParam ( i, 1, _T(""));
			SetStringParam ( i, 2, _T(""));
			SetStringParam ( i, 3, _T(""));
			SetStringParam ( i, 4, _T(""));
			
		}
		else
		{
			if(n + i < 0)
			{
				break;
			}
			SetStringParam ( i, 0, pIndexParam->namePara[n+i]);
			StrText = pIndexParam->descPara[n+i];
			if ( StrText.GetLength() > 0 )
			{
				SetStringParam ( i, 4, StrText);
			}

			ShowData(StrText,pIndexParam->min[n+i]);
			SetStringParam ( i, 2, StrText);

			ShowData(StrText,pIndexParam->max[n+i]);
			SetStringParam ( i, 1, StrText);

			ShowData(StrText,pIndexParam->defaultVal[n+i]);
			SetStringParam ( i, 3, StrText);	
		}
	}
	m_iLastSel = iSel;
}
void CIndexParamsDialog::OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateSel();
	*pResult = 0;
}

void CIndexParamsDialog::GetStringParam(int x, int y, CString& s)
{
	GetDlgItem(m_nEID[x][y])->GetWindowText(s);
}

void CIndexParamsDialog::SetStringParam(int x, int y, CString sIn)
{
	GetDlgItem(m_nEID[x][y])->SetWindowText(sIn);
}

void CIndexParamsDialog::ShowData(CString &str, float fdata)
{
	if(fdata==-1000)
	{
		str=_T("");
	}
	else
	{
		str.Format(_T("%.4f"),fdata);
		str.TrimRight (_T("0"));
		str.TrimRight (_T("."));
	}
}

float CIndexParamsDialog::StringToFloat(CString str)
{
	TCHAR * test;
	int length=str.GetLength();
	test=str.GetBuffer(length);
	char* strtest = _W2A(test);
	return (float)atof(strtest);
}

int32 CIndexParamsDialog::GetRet()
{
	return m_iRet;
}

HBRUSH CIndexParamsDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_BTN)
	{
		HBRUSH solidBrush = CreateSolidBrush(GetBackColor());		
		return solidBrush;
	}
	
	return hbr;
}
