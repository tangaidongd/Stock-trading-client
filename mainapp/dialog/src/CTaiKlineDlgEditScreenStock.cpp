// TechDlgEditScreenStock.cpp : implementation file
//

#include "stdafx.h"

#include <afxtempl.h>
#include "CFormularContent.h"
#include "IoViewShare.h"
// #include "CNavigatorApp.h"

// #include "CNavigatorDoc.h"
// 
#include "CFormularCompute.h"
#include "DIAEDITZBGSHS.h"
#include "CTaiKlineDlgEditScreenStock.h"

#include "DlgSelectZQ.h"
#include "ShareFun.h"
#include "DlgFormularPwd.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTaiKlineDlgEditScreenStock dialog
const CString KStrTradeEnterLong		= L"ENTERLONG";
const CString KStrTradeExitLong			= L"EXITLONG";
const CString KStrTradeEnterShort		= L"ENTERSHORT";
const CString KStrTradeExitShort		= L"EXITSHORT";
const CString KStrTradeDefault = KStrTradeEnterLong + L": ;\n" + KStrTradeExitLong + L": ;\n" + KStrTradeEnterShort + L": ;\n" + KStrTradeExitShort + L": ;";
bool g_bInitDlgEdit = false;
static WNDPROC	WndProcOldRichEdit;

BOOL CALLBACK WndProcNewRichEdit(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if( uMsg == WM_CHAR )
	{
		unsigned char cOld = (unsigned char)wParam;
		unsigned char cNew = toupper(cOld);
		
		return CallWindowProc(WndProcOldRichEdit, hwndDlg, uMsg, WPARAM(cNew), lParam);
	}
	
	return CallWindowProc(WndProcOldRichEdit, hwndDlg, uMsg, wParam, lParam);
}
// CTaiKlineDlgEditScreenStock message handlers
CTaiKlineDlgEditScreenStock::CTaiKlineDlgEditScreenStock(CFormulaLib* pLib,CFormularContent* p,CWnd* pParent )
: CDialogEx(CTaiKlineDlgEditScreenStock::IDD, pParent)
{

	m_pLib = pLib;

	m_bFormulaDataChanged = FALSE;
	m_bParamChanged = FALSE;
	pClone = p->Clone();
	m_pResultContent = p->Clone();

	m_nTabPre = 0;
	m_bInit = FALSE;

	if ( m_pResultContent->m_bProtected )
	{
		m_checkPassword = TRUE;
	}
	else
	{
		m_checkPassword = FALSE;
	}

	if ( m_pResultContent->m_bOften )
	{
		m_checkOften = TRUE;
	}
	else
	{
		m_checkOften = FALSE;
	}

	if ( !m_pResultContent->m_bProtected )
	{
		m_password = L"";
	}
	else
	{
		m_password = m_pResultContent->password;
	}

	m_name				= m_pResultContent->name;
	m_explainbrief		= m_pResultContent->explainBrief;
	m_help				= m_pResultContent->help;

	int i;
	for( i=0; i<PARAM_NUM; i++)
	{
		for(int j=0;j<4;j++)
			m_editParam[i][j] = _T("");
	}

	m_nLine[0] = IDC_EDIT_LINE1;
	m_nLine[1] = IDC_EDIT_LINE2;
	m_nLine[2] = IDC_EDIT_LINE3;
	m_nLine[3] = IDC_EDIT_LINE4;
	m_nLine[4] = IDC_EDIT_LINE5;
	m_nLine[5] = IDC_EDIT_LINE6;
	m_nLine[6] = IDC_EDIT_LINE7;

	m_nExtraY[0] = IDC_EDIT_LINE_EX1;
	m_nExtraY[1] = IDC_EDIT_LINE_EX2;
	m_nExtraY[2] = IDC_EDIT_LINE_EX3;
	m_nExtraY[3] = IDC_EDIT_LINE_EX4;

	m_nCID[0][0] = IDC_EDIT11;
	m_nCID[0][1] = IDC_EDIT12;
	m_nCID[0][2] = IDC_EDIT13;
	m_nCID[0][3] = IDC_EDIT14;
	m_nCID[0][4] = IDC_EDIT15;

	m_nCID[1][0] = IDC_EDIT21;
	m_nCID[1][1] = IDC_EDIT22;
	m_nCID[1][2] = IDC_EDIT23;
	m_nCID[1][3] = IDC_EDIT24;
	m_nCID[1][4] = IDC_EDIT25;
	
	m_nCID[2][0] = IDC_EDIT31;
	m_nCID[2][1] = IDC_EDIT32;
	m_nCID[2][2] = IDC_EDIT33;
	m_nCID[2][3] = IDC_EDIT34;
	m_nCID[2][4] = IDC_EDIT35;
	
	m_nCID[3][0] = IDC_EDIT41;
	m_nCID[3][1] = IDC_EDIT42;
	m_nCID[3][2] = IDC_EDIT43;
	m_nCID[3][3] = IDC_EDIT44;
	m_nCID[3][4] = IDC_EDIT45;

	errorposition = 0;
	numPara = 0;
	
	AfxInitRichEdit( );
	
}
CTaiKlineDlgEditScreenStock::~CTaiKlineDlgEditScreenStock()
{
	if ( NULL != pClone )
	{
		delete pClone;
		pClone = NULL;
	}
	if ( NULL != m_pResultContent )
	{
		delete m_pResultContent;
		m_pResultContent = NULL;
	}
}
void CTaiKlineDlgEditScreenStock::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTaiKlineDlgEditScreenStock)
	DDX_Control(pDX, IDOK, m_ok);
	DDX_Control(pDX, IDCANCEL, m_cancel);
	DDX_Control(pDX, IDC_DYHS, m_dyhs);
	DDX_Control(pDX, IDC_CSGS, m_csgs);	
	DDX_Control(pDX, IDC_BUTTON_CAN_USED, m_canused);
	DDX_Control(pDX, IDC_EDIT_LINE1, m_floatEditLine1);
	DDX_Control(pDX, IDC_EDIT_LINE2, m_floatEditLine2);
	DDX_Control(pDX, IDC_EDIT_LINE3, m_floatEditLine3);
	DDX_Control(pDX, IDC_EDIT_LINE4, m_floatEditLine4);
	DDX_Control(pDX, IDC_EDIT_LINE5, m_floatEditLine5);
	DDX_Control(pDX, IDC_EDIT_LINE6, m_floatEditLine6);
	DDX_Control(pDX, IDC_EDIT_LINE7, m_floatEditLine7);
	DDX_Control(pDX, IDC_EDIT12, m_floatEdit12);
	DDX_Control(pDX, IDC_EDIT13, m_floatEdit13);
	DDX_Control(pDX, IDC_EDIT14, m_floatEdit14);
	DDX_Control(pDX, IDC_EDIT15, m_floatEdit15);
	DDX_Control(pDX, IDC_EDIT22, m_floatEdit22);
	DDX_Control(pDX, IDC_EDIT23, m_floatEdit23);
	DDX_Control(pDX, IDC_EDIT24, m_floatEdit24);
	DDX_Control(pDX, IDC_EDIT25, m_floatEdit25);
	DDX_Control(pDX, IDC_EDIT32, m_floatEdit32);
	DDX_Control(pDX, IDC_EDIT33, m_floatEdit33);
	DDX_Control(pDX, IDC_EDIT34, m_floatEdit34);
	DDX_Control(pDX, IDC_EDIT35, m_floatEdit35);
	DDX_Control(pDX, IDC_EDIT42, m_floatEdit42);
	DDX_Control(pDX, IDC_EDIT43, m_floatEdit43);
	DDX_Control(pDX, IDC_EDIT44, m_floatEdit44);
	DDX_Control(pDX, IDC_EDIT45, m_floatEdit45);
	DDX_Control(pDX, IDC_TAB1, m_tabParam);
	DDX_Control(pDX, IDC_NAME, m_namecor);
	DDX_Control(pDX, IDC_PASSWORD, m_passwordcor);
	DDX_Control(pDX, IDC_INPUT, m_input_content);
	DDX_Check(pDX, IDC_CHECK_PASSWORD, m_checkPassword);
	DDX_Check(pDX, IDC_CHECK1, m_checkOften);
	DDX_Text(pDX, IDC_EXPLAINBRIEF, m_explainbrief);
	DDX_Text(pDX, IDC_MYHELP, m_help);
	DDX_Text(pDX, IDC_NAME, m_name);
	DDX_Text(pDX, IDC_PASSWORD, m_password);
	//}}AFX_DATA_MAP
	//DDV_MaxChars(pDX, m_name, 10);
}

BEGIN_MESSAGE_MAP(CTaiKlineDlgEditScreenStock, CDialogEx)
//{{AFX_MSG_MAP(CTaiKlineDlgEditScreenStock)
ON_BN_CLICKED(IDC_CSGS, OnTestFormula)
ON_BN_CLICKED(IDC_DYHS, OnAddFunc)
ON_BN_CLICKED(IDB_RESTORE,OnRestore)
ON_BN_CLICKED(IDC_ZTDJ, OnZt)
ON_BN_CLICKED(IDC_FT, OnFt)
ON_WM_PAINT()
ON_BN_CLICKED(IDC_CHECK_PASSWORD, OnCheckPassword)
ON_WM_CTLCOLOR()
ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnSelchangeTab1)
ON_BN_CLICKED(IDC_BUTTON_CAN_USED, OnButtonCanUsed)
ON_WM_HELPINFO()
ON_EN_CHANGE(IDC_INPUT, OnChangeInput)
ON_WM_SIZE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTaiKlineDlgEditScreenStock message handlers

///////////////////////////////////////////////////////////////////
//      功能说明：测试公式
//      入口参数：无
//      返回参数：无
/////////////////////////////////////////////////////////////////////////////
void CTaiKlineDlgEditScreenStock::OnTestFormula()
{
	TestFormula ( TRUE );
} 
void CTaiKlineDlgEditScreenStock::TestFormula( BOOL bAlert)
{
	bool errpram=false;
	UpdateData();
	CString input=_T("");
	m_input_content.GetWindowText(input);
	input.Remove('\r');
	input.Remove('\n');

	int i=0, j=0;
	for(int flag=0;flag<1;flag++)
	{
		//m_edit 的数值保存到 m_editParam
		int n = m_tabParam.GetCurSel();
		for(i=0;i<4;i++)
		{
			for(j=0;j<5;j++)
			{
				m_editParam[4*n+i][j]=GetStringParam(i,j);
			}
		}
		//检测参数合法性
		for(i=0;i<PARAM_NUM;i++)
		{
			bool bEmpty = true;
			for(j=1;j<4;j++)
			{
				if(m_editParam[i][j] != _T(""))
				{
					bEmpty = false;
					break;
				}
			}
			
			//test name
			if(bEmpty == false)
			{
				if(m_editParam[i][0]==_T(""))
				{
					errorposition=2000+(i+1)*10+1;
					errormessage=_T("参数名不能空.");
					errpram=true;
					break;
				}
				else if(!CheckParmNameInput(m_editParam[i][0],errormessage))
				{
					errorposition=2000+(i+1)*10+1;
					errpram=true;
					break;
				}
			}
			else
			{
				m_editParam[i][0]==_T("");
				for(int k=i+1;k<PARAM_NUM;k++)				// huhe*
				{
					for(int l=0;l<4;l++)
						m_editParam[k][l]==_T("");
				}
				break;
			}
			
			//test param
			for(j=1;j<4;j++)
			{
				if(m_editParam[i][j] == "")
				{
					errorposition=2000+(i+1)*10+1+j;
					errormessage=_T("输入值不能空.");
					errpram=true;
					break;
				}
				else
				{
					if(!CheckParmInput(m_editParam[i][j]))
					{
						errorposition=2000+(i+1)*10+1+j;
						errormessage=_T("输入值不合法.");
						errpram=true;
						break;
					}
				}
				//
				if(j==3)
				{
					if(StringToFloat(m_editParam[i][3])>StringToFloat(m_editParam[i][1])||StringToFloat(m_editParam[i][3])<StringToFloat(m_editParam[i][2]))
					{
						errorposition=2014;
						errormessage=_T("缺省值不在合法的范围.");
						errpram=true;
						break;
					}
				}
			}
		}
	}
	
	CString formal=input;
	formal.Remove(' ');
	if(formal.GetLength()==0)
	{
		errormessage=_T("公式内容不能为空.");
		m_input_content.SetFocus();
		UpdateData(FALSE);
		return;
	}
	else
	{
		formal=input;
	}
	if(formal.GetAt(formal.GetLength()-1)==';')
		formal.Delete(formal.GetLength()-1);

	// 重新加载
	formal.Empty();
	m_input_content.GetWindowText(formal);
	formal.MakeLower();
	

	Kline kline;
	CFormularCompute *equation=new CFormularCompute(0,0,&kline,formal);
	
	for(i=0;i<PARAM_NUM;i++)
	{
		if(m_editParam[i][0]!=_T("")&&m_editParam[i][3]!="")
		{  
			float fpram=StringToFloat(m_editParam[i][3]);
			m_editParam[i][0].MakeLower();
			int addpra=equation->AddPara(m_editParam[i][0],fpram);
			switch(addpra)
			{
			case(1):
				errorposition=2011;
				errpram=true;
				errormessage=_T("参数个数太多.");
				break;
				
			case(2):
				errorposition=2011;
				errpram=true;
				errormessage=_T("参数名重复或是系统关键字.");
				break;
				
			}
			if(errpram == true)
				break;
			
		}
	}
	
	if(errpram)
	{
		errorposition-=2000;         //将参数的错误号换成EDIT的序号
		switch(errorposition)
		{
		case 11:
			GetDlgItem(IDC_EDIT11)->SetFocus();
			break;
		case 12:
			GetDlgItem(IDC_EDIT12)->SetFocus();
			break;
		case 13:
			GetDlgItem(IDC_EDIT13)->SetFocus();
			break;
		case 14:
			GetDlgItem(IDC_EDIT14)->SetFocus();
			break;
			
		case 21:
			GetDlgItem(IDC_EDIT21)->SetFocus();
			break;
		case 22:
			GetDlgItem(IDC_EDIT22)->SetFocus();
			break;
		case 23:
			GetDlgItem(IDC_EDIT23)->SetFocus();
			break;
		case 24:
			GetDlgItem(IDC_EDIT24)->SetFocus();
			break;
			
		case 31:
			GetDlgItem(IDC_EDIT31)->SetFocus();
			break;
		case 32:
			GetDlgItem(IDC_EDIT32)->SetFocus();
			break;
		case 33:
			GetDlgItem(IDC_EDIT33)->SetFocus();
			break;
		case 34:
			GetDlgItem(IDC_EDIT34)->SetFocus();
			break;
			
		case 41:
			GetDlgItem(IDC_EDIT41)->SetFocus();
			break;
		case 42:
			GetDlgItem(IDC_EDIT42)->SetFocus();
			break;
		case 43:
			GetDlgItem(IDC_EDIT43)->SetFocus();
			break;
		case 44:
			GetDlgItem(IDC_EDIT44)->SetFocus();
			break;
		default:
			break;
		}
		if(errorposition==-1)
		{
			if ( bAlert )
			{
				MessageBox(_T("测试通过."), AfxGetApp()->m_pszAppName);
			}
		}
		else
		{
			if ( bAlert )
			{
				MessageBox(errormessage, AfxGetApp()->m_pszAppName, MB_ICONWARNING);
			}
		}

		delete equation;
		UpdateData(FALSE);		
		return;
	}
	
	equation->Devide();
	
	errorposition=equation->m_errpos;
	errormessage=equation->m_errmsg;
	if(errorposition==-1)
	{
		if(equation->m_RlineNum==0)
		{
			errorposition=formal.GetLength();
			errormessage=_T("公式必须至少画一条线.");
		}
		else if(equation->m_RlineNum>NUM_LINE_TOT)
		{
			errorposition=formal.GetLength();
			errormessage=_T("公式最多只能计算32条线.");
		}
	}
	
	if ( EFTCdtChose == m_pResultContent->m_eFormularType )
	{
		// ...0114 fangz 这个应该在 Devid() 内部判断处理, 外部只根据 Devid 给的错误消息做提示

		// 只能输出一条
		if (equation->m_RlineNum < 1)
		{
			errorposition = formal.GetLength();
			errormessage = _T("条件选股公式至少需要一个输出条件.");
		}
		else if ( equation->m_RlineNum > 1 )
		{
			errorposition = formal.GetLength();
			errormessage = _T("条件选股公式只能有一个输出条件.");
		}
	}
	else if ( EFTTradeChose == m_pResultContent->m_eFormularType )
	{
		// ...0114 fangz 这个应该在 Devid() 内部判断处理, 外部只根据 Devid 给的错误消息做提示

		for ( i = 0; i < equation->m_RlineNum; i++ )
		{
			CString StrLineName = equation->m_RlineName[i];

			int a0 = StrLineName.CompareNoCase(KStrTradeEnterLong);
			int a1 = StrLineName.CompareNoCase(KStrTradeExitLong);
			int a2 = StrLineName.CompareNoCase(KStrTradeEnterShort);
			int a3 = StrLineName.CompareNoCase(KStrTradeExitShort);

			if ( a0 != 0 && a1 != 0 && a2 != 0 && a3 != 0 )			  
			{
				// 名称不对:
				errorposition = formal.GetLength();
				errormessage  = _T("交易公式输出变量名错误.");
			}
		}		
	}

	delete equation;
	if(errorposition!=-1)
	{
		// 目测此处sel遇到中文有偏移量的bug
		//m_input_content.SetSel(errorposition,errorposition);
		m_input_content.SetRESel(errorposition, errorposition);
		long b,e;
		m_input_content.GetSel(b,e);
		long lng = m_input_content.LineFromChar(errorposition);
		if(lng>=0)
		{
			int nln = m_input_content.GetFirstVisibleLine();
			if(nln>lng)
				m_input_content.LineScroll(lng-nln ,  0 );
			else if(nln<lng)
			{
				m_input_content.LineScroll(lng-nln-1 ,  0 );
			}
		}
	}
	if(errorposition==-1)
	{
		if ( bAlert )
		{
			MessageBox(_T("测试通过."), AfxGetApp()->m_pszAppName);
		}
	}
	else
	{
		if ( bAlert )
		{
			MessageBox(errormessage, AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		}
	}
	
	m_input_content.SetFocus();	
	UpdateData(FALSE);	
}

//保存用户对公式的修改
void CTaiKlineDlgEditScreenStock::OnOK() 
{
    UpdateData();
	
	int i = 0;
	//pContent->name
	CString name=m_name;
	name.Remove(' ');
	bool isnameok=true;
	for( i=0;i<m_name.GetLength();i++)
	{
		TCHAR test=m_name.GetAt(i);
		if((test&0x80)!=0)
		{
			i=i+1;
			continue;
		}
// 		if(!((test>='a'&&test<='z')||test=='#'||test=='_'||test=='%'||test == '(' || test == ')'||(test>='A'&&test<='Z')||(test>='0'&&test<='9')))
// 		{
// 			isnameok=false;
// 			break;
// 		}
// 		if(i==0&&test>='0'&&test<='9')
// 		{
// 			isnameok=false;
// 			break;
// 		}
	}
	if(!isnameok||name==_T("")||name==_T("STOCKMODEKLINE")||name==_T("STOCKFILTER"))
	{
		MessageBox(_T("指标名为空或非法,\n请重新输入"), AfxGetApp()->m_pszAppName, MB_OK);
		m_namecor.SetFocus();
		return;
	}
	//pContent->bNew
	if ( m_pResultContent->bNew )
	{
		if(FindIndicator(m_name))
		{
			MessageBox(m_name+_T("名称已存在,请重新选择."), AfxGetApp()->m_pszAppName, MB_ICONWARNING);
			return;
		}
	}

	//pContent->formular
	TestFormula(FALSE);
	if( errorposition != -1 )
	{
		MessageBox(_T("您编辑的公式没有通过语法检查, 请单击[测试公式]获得帮助."), AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return;
	}
	
	m_pResultContent->name=m_name;
	CString input=_T("");
	m_input_content.GetWindowText(input);
	//input.Remove('\n');
	//input.Remove('\r');
	m_pResultContent->formular=input;

	//pContext->numPara
	int pramnum=0;
	for(i=0;i<PARAM_NUM;i++)
	{
		if(m_editParam[i][0]!=""&&m_editParam[i][3]!="")
		{
			pramnum++;
		}
		else
		{
			break;
		}
	}

	m_pResultContent->numPara=pramnum;

	for(i=0;i<pramnum;i++)
	{
		m_pResultContent->namePara[i]=m_editParam[i][0];
		m_pResultContent->namePara[i].MakeLower();
		m_pResultContent->descPara[i]=m_editParam[i][4];
		m_pResultContent->max[i]=StringToFloat(m_editParam[i][1]);
		m_pResultContent->min[i]=StringToFloat(m_editParam[i][2]);
		
		float fValue = StringToFloat(m_editParam[i][3]);
		float fMin   = m_pResultContent->min[i];
		float fMax   = m_pResultContent->max[i];
		/*
		if ( -1 != name.Find(L"MA"))
		{
			// fangz 0422 MA 指标值负数时候Bug,补丁式修改
			

			if ( fValue <= (float)1.0 || fValue >= (float)1000.0 )
			{
				// 参数非法的时候,设置为 2 
				pContent->defaultVal[i]= (float)2.0;		
			}
			else
			{
				pContent->defaultVal[i]=StringToFloat(m_editParam[i][3]);
			}
		}
		else
		{
			// 指标的参数设置不能为0;
			if ( fValue <= (float)0.0)
			{
				pContent->defaultVal[i] = (float)1.0;
			}
			else
			{
				pContent->defaultVal[i] = StringToFloat(m_editParam[i][3]);
			}			
		}
		*/

		if ( fValue < fMin )
		{
			fValue = fMin;
		}
		
		if ( fValue > fMax)
		{
			fValue = fMax;
		}

		if ( -1 != m_pResultContent->name.Find(L"MA"))
		{
			// MA 的话不能小于 1
			if (fValue < float(1.0))
			{
				// 先跟最小值比较
				if ( fMin > float(1))
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
			if (fValue <= float(0.0))
			{				
				// 其他的不能小于 0
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
		
		m_pResultContent->defaultVal[i] = fValue;
		
		m_pResultContent->stepLen[i] = 0.0f;
	}

	m_pResultContent->numLine = 0;
	
	for ( i = 0; i < 7; i ++ )
	{
		CWnd* pWndLine = GetDlgItem(m_nLine[i]);
		CString StrLine;
		pWndLine->GetWindowText(StrLine);
		if ( StrLine.GetLength() < 1 )
		{
			break;
		}
		m_pResultContent->line[m_pResultContent->numLine] = StringToFloat ( StrLine );
		m_pResultContent->numLine++;
	}

	m_pResultContent->numExtraY = 0;
	for ( i = 0; i < 4; i++ )
	{
		CWnd* pWndExtraY = GetDlgItem(m_nExtraY[i]);
		CString StrLine;
		if ( NULL != pWndExtraY )
		{
			pWndExtraY->GetWindowText(StrLine);
			if ( StrLine.GetLength() < 1 )
			{
				break;
			}
			m_pResultContent->lineExtraY[m_pResultContent->numExtraY] = StringToFloat(StrLine);
			m_pResultContent->numExtraY++;
		}
	}

	m_pResultContent->InitDefaultValArray();
	if(pramnum!=m_pResultContent->defaultValArray.GetSize())
		m_pResultContent->AddDefaultValToArray();

	//flag
	i=GetCheckedRadioButton(IDC_ZTDJ,IDC_FT);
	if(i==IDC_ZTDJ)
	{
		AddFlag(m_pResultContent->flag,CFormularContent::KAllowMain);
		RemoveFlag(m_pResultContent->flag,CFormularContent::KAllowSub);
	}
	else
	{
		AddFlag(m_pResultContent->flag,CFormularContent::KAllowSub);
		RemoveFlag(m_pResultContent->flag,CFormularContent::KAllowMain);
	}
	if ( m_checkPassword )
	{
		m_pResultContent->m_bProtected = true;
	}
	else
	{
		m_pResultContent->m_bProtected = false;
	}
	if ( m_checkOften )
	{
		m_pResultContent->m_bOften = true;
	}
	else
	{
		m_pResultContent->m_bOften = false;
	}

	m_pResultContent->explainBrief=m_explainbrief;	
	if((m_checkPassword==TRUE))
		m_pResultContent->password=m_password;

	m_pResultContent->help=m_help;	

	if ( !m_pResultContent->EqualFormulaData(pClone))
	{
		m_bFormulaDataChanged = TRUE;
	}

	if ( !m_pResultContent->EqualParam(pClone))
	{
		m_bParamChanged = TRUE;
	}

	CDialog::OnOK();
}

void CTaiKlineDlgEditScreenStock::OnRestore() 
{

}

void CTaiKlineDlgEditScreenStock::OnZt()
{
	int i = 0;
	for ( i = 0; i < 7; i++ )
	{
		CWnd* pWnd = GetDlgItem(m_nLine[i]);
		if ( NULL != pWnd )
		{
			pWnd->EnableWindow(FALSE);
		}
	}

	for ( i = 0; i < 4; i++ )
	{
		CWnd* pWnd = GetDlgItem(m_nExtraY[i]);
		if ( NULL != pWnd )
		{
			pWnd->EnableWindow(FALSE);
		}
	}
}

void CTaiKlineDlgEditScreenStock::OnFt()
{
	int i = 0;
	for ( i = 0; i < 7; i++ )
	{
		CWnd* pWnd = GetDlgItem(m_nLine[i]);
		if ( NULL != pWnd )
		{
			pWnd->EnableWindow(TRUE);
		}
	}
	
	for ( i = 0; i < 4; i++ )
	{
		CWnd* pWnd = GetDlgItem(m_nExtraY[i]);
		if ( NULL != pWnd )
		{
			pWnd->EnableWindow(TRUE);
		}
	}
}
///////////////////////////////////////////////////////////////////
//      功能说明：增加函数
//      入口参数：无
//      返回参数：无
/////////////////////////////////////////////////////////////////////////////
void CTaiKlineDlgEditScreenStock::OnAddFunc() 
{
	UpdateData();
	CDialogEDITZBGSHS m_diazbgshs;
	if(m_diazbgshs.DoModal()==IDOK)
	{
		m_input_content.SetFocus();
		functionstr=m_diazbgshs.m_selectedstr;
		int length=functionstr.GetLength();
		int i = 0;
		for(i=0;i<length;i++)
		{
			TCHAR test=functionstr.GetAt(i);//cuipeng
			if(test=='-'&&functionstr.GetAt(i+1)=='-')
				break;
		}
		if(i==length)
			functionstr=_T("");
		else
			functionstr=functionstr.Left(i);
		m_input_content.ReplaceSel(functionstr);
	}
}

BOOL CTaiKlineDlgEditScreenStock::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if ( m_pResultContent->m_bProtected )
	{
		bool32 bOk = false;
		CDlgFormularPwd Dlg;
		if ( IDOK == Dlg.DoModal() )
		{
			CString StrPwd = Dlg.GetPwd();
			
			if ( 0 != StrPwd.Compare(m_pResultContent->password) )
			{
				MessageBox(L"密码错误!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
			}
			else
			{
				bOk = true;
			}
		}
		if ( !bOk )
		{
			PostMessage(WM_COMMAND, IDCANCEL, (LPARAM)(GetDlgItem(IDCANCEL)->GetSafeHwnd())); // 让其在显示之后立即消失
		}
	}

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,FALSE);

	CString StrText;
	if ( EFTNormal == m_pResultContent->m_eFormularType )
	{
		if ( m_pResultContent->name.IsEmpty() )
		{
			StrText.Format(_T("新建技术指标公式"));
		}
		else
		{
			StrText.Format(_T("技术指标公式[%s]"),m_pResultContent->name.GetBuffer());
		}		
	}
	else if ( EFTCdtChose == m_pResultContent->m_eFormularType )
	{
		if ( m_pResultContent->name.IsEmpty() )
		{
			StrText.Format(_T("新建条件选股指标公式"));
		}
		else
		{
			StrText.Format(_T("条件选股指标公式[%s]"),m_pResultContent->name.GetBuffer());
		}
	}
	else if ( EFTTradeChose == m_pResultContent->m_eFormularType )
	{
		if ( m_pResultContent->name.IsEmpty() )
		{
			StrText.Format(_T("新建交易指标公式"));
		}
		else
		{
			StrText.Format(_T("交易指标公式[%s]"),m_pResultContent->name.GetBuffer());
		}
	}
	else if ( EFTClrKLine == m_pResultContent->m_eFormularType )
	{
		if ( m_pResultContent->name.IsEmpty() )
		{
			StrText.Format(_T("新建五彩 K 线公式"));
		}
		else
		{
			StrText.Format(_T("五彩 K 线公式[%s]"),m_pResultContent->name.GetBuffer());
		}
	}

	
	SetWindowTextEx(StrText);

	UpdateData(FALSE);

	// 设置 RichEdit 字体
	LOGFONT LogFont;
	memset(&LogFont, 0, sizeof(LogFont));
	
	LogFont.lfHeight	= -13;
	LogFont.lfWeight	= 400;
	LogFont.lfCharSet	= 134;
	LogFont.lfQuality	= 1;
	LogFont.lfOutPrecision		= 1;
	LogFont.lfClipPrecision	=2;
	LogFont.lfPitchAndFamily	= 1;

	lstrcpy(LogFont.lfFaceName, L"Fixedsys");
	
	CFont stFont;
	stFont.CreateFontIndirect(&LogFont);	
	m_input_content.SetFont(&stFont);


	// 设置 RichEdit 大写
	WndProcOldRichEdit = (WNDPROC)SetWindowLong(m_input_content.GetSafeHwnd(), GWL_WNDPROC, (long)WndProcNewRichEdit);

	//
	if(m_bInit == FALSE)
	{
		m_tabParam.InsertItem (0,_T("参数1-4"));
		m_tabParam.InsertItem (1,_T("参数5-8"));
		m_tabParam.InsertItem (2,_T("参数9-12"));	// huhe+ 加到16个参数设置
		m_tabParam.InsertItem (3,_T("参数13-16"));	// huhe+
		m_bInit = TRUE;
	}

	if(CheckFlag(m_pResultContent->flag,CFormularContent::KAllowMain))
		CheckRadioButton(IDC_ZTDJ,IDC_FT,IDC_ZTDJ);
	else 
		CheckRadioButton(IDC_ZTDJ,IDC_FT,IDC_FT);

	if(!m_pResultContent->bNew)
		m_namecor.SetReadOnly(TRUE);

	if ( m_pResultContent->m_bSystem && m_pResultContent->m_bCanRestore )
	{
		GetDlgItem(IDB_RESTORE)->EnableWindow();
	}
	else
	{
		GetDlgItem(IDB_RESTORE)->EnableWindow(FALSE);
	}

	//将指标公式的参数初始化
	CString input = m_pResultContent->formular;
	CString input2;
	int i = 0;
 	for ( i = 0; i < input.GetLength(); i ++ )
 	{
 		TCHAR tc = input.GetAt(i);
 		input2 += tc;
 		if (tc == ';' && i != input.GetLength()-1 && (i + 1) < input.GetLength() )
 		{
			TCHAR tcNext = input.GetAt(i+1);
			if ( tcNext == '\r' || tcNext == '\n' )
			{
				continue;
			}

 			input2 += _T("\r");
 		}
 	}
	
	m_input_content.SetWindowText(input2);	
	m_input_content.SetFocus();

	if ( m_pResultContent->formular.IsEmpty() && EFTTradeChose == m_pResultContent->m_eFormularType )
	{
		m_input_content.SetWindowText(KStrTradeDefault);
	}

	//temp initiate
	m_pResultContent->InitDefaultValArray();
	m_checkOften = m_pResultContent->m_bOften;

	for ( i = 0; i < m_pResultContent->numLine; i ++ )
	{
		CString StrLine;
		ShowData(StrLine,m_pResultContent->line[i]);
		CWnd* pWnd = GetDlgItem(m_nLine[i]);
		if ( NULL != pWnd )
		{
			pWnd->SetWindowText(StrLine);
		}		
	}

	for ( i = 0; i < m_pResultContent->numExtraY; i++ )
	{
		CString StrY;
		ShowData(StrY, m_pResultContent->lineExtraY[i]);
		CWnd* pWnd = GetDlgItem(m_nExtraY[i]);
		if ( NULL != pWnd )
		{
			pWnd->SetWindowText(StrY);
		}
	}

	ParamIntoOther(0);
	ParamIntoOther(2);

	UpdateData();
	DoChangeInput() ;
	m_input_content.SetEventMask(m_input_content.GetEventMask() |ENM_CHANGE);
	g_bInitDlgEdit = true;


	if ( EFTCdtChose   == m_pResultContent->m_eFormularType 
	  || EFTTradeChose == m_pResultContent->m_eFormularType 
	  || EFTClrKLine   == m_pResultContent->m_eFormularType 
	   )
	{
		// 五彩K 线, 部分控件禁用, 隐藏
		GetDlgItem(IDC_ZTDJ)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_Y)->ShowWindow(SW_HIDE);

		for( i = 0; i < 7; i++ )
		{
			CWnd* pWnd = GetDlgItem(m_nLine[i]);
			if ( NULL != pWnd )
			{
				pWnd->ShowWindow(SW_HIDE);
			}
		}
	
		GetDlgItem(IDC_STATIC_Y2)->ShowWindow(SW_HIDE);
		for ( i = 0; i < 4; i++ )
		{
			CWnd* pWnd = GetDlgItem(m_nExtraY[i]);
			if ( NULL != pWnd )
			{
				pWnd->ShowWindow(SW_HIDE);
			}
		}
		
		GetDlgItem(IDC_CHECK1)->ShowWindow(SW_HIDE);	
		GetDlgItem(IDC_BUTTON_CAN_USED)->ShowWindow(SW_HIDE);	

		CRect RectEdit;
		m_input_content.GetWindowRect(&RectEdit);
		ScreenToClient(&RectEdit);
	
		CRect Rect2;
		GetDlgItem(IDC_EDIT_LINE_EX1)->GetWindowRect(&Rect2);
		ScreenToClient(&Rect2);

		RectEdit.bottom = Rect2.bottom;
		m_input_content.MoveWindow(&RectEdit);
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////
//      功能说明：浮点数转换为字符串
//      入口参数：fdata：浮点数
//      返回参数：str:返回字符串
/////////////////////////////////////////////////////////////////////////////
void CTaiKlineDlgEditScreenStock::ShowData(CString &str, float fdata)
{
	if(-1000 == fdata)
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

///////////////////////////////////////////////////////////////////
//      功能说明：字符串转换为浮点数
//      入口参数：str:字符串
//      返回参数：返回浮点数
/////////////////////////////////////////////////////////////////////////////
float CTaiKlineDlgEditScreenStock::StringToFloat(CString str)
{
	TCHAR * test;
	int length=str.GetLength();
	test=str.GetBuffer(length);
	char* strtest = _W2A(test);
	return (float)atof(strtest);
}

void CTaiKlineDlgEditScreenStock::OnPaint() 
{
	CDialogEx::OnPaint();

	// Do not call CDialog::OnPaint() for painting messages
}

///////////////////////////////////////////////////////////////////
//      功能说明：测试参数数据是否合法
//      入口参数：parminput:参数数据
//      返回参数：返回是否正确
/////////////////////////////////////////////////////////////////////////////
void CTaiKlineDlgEditScreenStock::OnCheckPassword() 
{
	if(IsDlgButtonChecked(IDC_CHECK_PASSWORD))
	{
		m_passwordcor.EnableWindow(TRUE);
		m_passwordcor.SetFocus();
	}
	else
	{
		m_passwordcor.SetWindowText(L"");
		m_passwordcor.EnableWindow(FALSE);
	}
}

///////////////////////////////////////////////////////////////////
//      功能说明：测试参数数据是否合法
//      入口参数：parminput:参数数据
//      返回参数：返回是否正确
/////////////////////////////////////////////////////////////////////////////
BOOL CTaiKlineDlgEditScreenStock::CheckParmInput(CString parminput)
{
	int inputlength=parminput.GetLength();
	TCHAR test;
	for(int i=0;i<inputlength;i++)
	{
		
		test=parminput.GetAt(i);
		if(!(test=='-'||test=='+'||test=='.'||(test>='0'&&test<='9')))
		{
			return FALSE;
		}
		
	}
	return TRUE;
	
}

///////////////////////////////////////////////////////////////////
//      功能说明：检测参数名称是否合法
//      入口参数：parname:参数名称，errmessage：错误信息
//      返回参数：返回是否正确
/////////////////////////////////////////////////////////////////////////////
BOOL CTaiKlineDlgEditScreenStock::CheckParmNameInput(CString parname, CString &errmessage)
{
	parname.Remove(' ');
	TCHAR test=parname.GetAt(0);
	//lint --e{514}
	if((test&0x80 == 0 )&& ((test<'A')||(test>'z')||(test=='_')))
	{
		errormessage=_T("参数名必须以字母开头");
		return FALSE;
	}
	int length=parname.GetLength();
	for(int i=1;i<length;i++)
	{
		test=parname.GetAt(i);
		if((test>='0'&&test<='9')||(test>='A'&&test<='z')||test=='_'||((BYTE)test)>=0x80 )
			continue;
		else
		{
			errormessage=_T("参数名组成非法");
			return FALSE;
		}
	}
	return TRUE;
	
}

void CTaiKlineDlgEditScreenStock::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}
//////////////////////////////////////////////////////////////////////////////
//      功能说明：参数1-4与参数5-8之间的切换处理
//      入口参数：无
//      返回参数：无返回数
/////////////////////////////////////////////////////////////////////////////
void CTaiKlineDlgEditScreenStock::OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	//把参数信息保存到m_editParam中
	UpdateData();
	int n = m_tabParam.GetCurSel();
	int i = 0, j = 0;
	if(m_nTabPre!=n)
	{
		for(i=0;i<4;i++)
		{
			for(j=0;j<5;j++)
			{
				m_editParam[4*m_nTabPre+i][j]=GetStringParam(i,j);
			}
		}

		if(n > m_nTabPre)// 判断前面的参数是否已经设置
		{
			for(i=0; i<4*n; i++)
			{
				if(m_editParam[i][0] == _T(""))
				{	
					// 若前面某个参数为空
					m_tabParam.SetCurSel(m_nTabPre);// 选择原来的页
					n = m_nTabPre;
					break;
				}
			}
		}

		if(n != m_nTabPre)// 若前面的参数都已设置
		{
			for(i=0;i<4;i++)
			{
				for(j=0;j<5;j++)
				{
					SetStringParam(i,j,m_editParam[4*n+i][j]);
				}
			}
			UpdateData(FALSE);
		}
	}
	m_nTabPre = n;
	*pResult = 0;
}

///////////////////////////////////////////////////////////////////
//      功能说明：参数转换
//      入口参数：nFlag=0:前四个参数输出到编辑框,nFlag=1:后四个参数输出到编辑框,nFlag=2:所有参数输出到缓存
//      返回参数：无返回数
/////////////////////////////////////////////////////////////////////////////
void CTaiKlineDlgEditScreenStock::ParamIntoOther(int nFlag)//
{
	int i,j;
	if(nFlag >1)
	{
		if(m_pResultContent->numPara > PARAM_NUM)					// huhe* 加到16个参数设置
			m_pResultContent->numPara = 0;
		for(i=0; i<PARAM_NUM; i++)							// huhe* 加到16个参数设置
		{
			if(i<m_pResultContent->numPara)
			{
				m_editParam[i][0]=m_pResultContent->namePara[i];
				m_editParam[i][4]=m_pResultContent->descPara[i];
				ShowData(m_editParam[i][1],m_pResultContent->max[i]);
				ShowData(m_editParam[i][2],m_pResultContent->min[i]);
				float f = m_pResultContent->defaultVal[i];
				ShowData(m_editParam[i][3],f);
			}
			else
			{
				for(j=0;j<4;j++)
					m_editParam[i][j]=_T("");
			}
			;
		}
	}
	else
	{
		if(m_pResultContent->numPara > PARAM_NUM)					// huhe* 加到16个参数设置
			m_pResultContent->numPara = 0;
		int nBegin = 0;
		if(nFlag==1&&m_pResultContent->numPara>4)
			nBegin = 4;
		for(i=nBegin;i<4+nBegin;i++)
		{
			if(i<m_pResultContent->numPara-nBegin)
			{
				SetStringParam(i,0,m_pResultContent->namePara[i]);
				SetStringParam(i,4,m_pResultContent->descPara[i]);
				CString s;
				ShowData(s,m_pResultContent->max[i]);
				SetStringParam(i,1,s);
				ShowData(s,m_pResultContent->min[i]);
				SetStringParam(i,2,s);
				
				float f = m_pResultContent->defaultVal[i];
				ShowData(s,f);
				SetStringParam(i,3,s);
			}
			else
				for(j=0;j<4;j++)
					SetStringParam(i,j,_T(""));
		}
	}
}

///////////////////////////////////////////////////////////////////
//      功能说明：改变技术指标数据的指标可用时间周期的内容
//      入口参数：无
//      返回参数：无返回数
/////////////////////////////////////////////////////////////////////////////
void CTaiKlineDlgEditScreenStock::OnButtonCanUsed() 
{
	CDlgSelectZQ dlg;
	dlg.flag = m_pResultContent->flag;
	if ( IDOK == dlg.DoModal() )
	{
		RemoveFlag(m_pResultContent->flag, CFormularContent::KAllowMinute);
		RemoveFlag(m_pResultContent->flag, CFormularContent::KAllowDay);
		RemoveFlag(m_pResultContent->flag, CFormularContent::KAllowMinute5);
		RemoveFlag(m_pResultContent->flag, CFormularContent::KAllowMinute15);
		RemoveFlag(m_pResultContent->flag, CFormularContent::KAllowMinute30);
		RemoveFlag(m_pResultContent->flag, CFormularContent::KAllowMinute60);
		RemoveFlag(m_pResultContent->flag, CFormularContent::KAllowMinute180);
		RemoveFlag(m_pResultContent->flag, CFormularContent::KAllowMinute240);
		RemoveFlag(m_pResultContent->flag, CFormularContent::KAllowMinuteUser);
		RemoveFlag(m_pResultContent->flag, CFormularContent::KAllowDayUser);
		RemoveFlag(m_pResultContent->flag, CFormularContent::KAllowWeek);
		RemoveFlag(m_pResultContent->flag, CFormularContent::KAllowMonth);
		RemoveFlag(m_pResultContent->flag, CFormularContent::KAllowQuarter);
		RemoveFlag(m_pResultContent->flag, CFormularContent::KAllowYear);

		AddFlag(m_pResultContent->flag, dlg.flag);
	}
}

CString CTaiKlineDlgEditScreenStock::GetStringParam(int x, int y)
{
	CString s =_T("");

	int iLenAll = sizeof(m_nCID)/sizeof(int);
	int iLenSecond = sizeof(m_nCID[0]) / sizeof(int);
	int iLenFirst = iLenAll / iLenSecond;

	if ((x>=0 && x<iLenFirst) && (y>=0 && y<iLenSecond))
	{
		GetDlgItem(m_nCID[x][y])->GetWindowText(s);
	}

	return s;
}

void CTaiKlineDlgEditScreenStock::SetStringParam(int x, int y, CString sIn)
{
	int iLenAll = sizeof(m_nCID)/sizeof(int);
	int iLenSecond = sizeof(m_nCID[0]) / sizeof(int);
	int iLenFirst = iLenAll / iLenSecond;

	if ((x>=0 && x<iLenFirst) && (y>=0 && y<iLenSecond))
	{
		GetDlgItem(m_nCID[x][y])->SetWindowText(sIn);
	}
}

BOOL CTaiKlineDlgEditScreenStock::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
	
	//	HtmlHelp(m_hWnd,"stock.chm",HH_HELP_CONTEXT,CTaiKlineIndicatorsManage::IDD);
	//	return TRUE;//
	//	DoHtmlHelp(this);
	return TRUE;
}

BOOL CTaiKlineDlgEditScreenStock::FindIndicator(CString sName)
{
	if ( m_pLib->m_SysIndex.Get(sName))
	{
		return TRUE;
	}
	if ( m_pLib->m_UserIndex.Get(sName))
	{
		return TRUE;
	}
	return FALSE;
}

void CTaiKlineDlgEditScreenStock::OnChangeInput() 
{
	UpdateData();
	if(g_bInitDlgEdit)
		DoChangeInput();	
}

void CTaiKlineDlgEditScreenStock::DoChangeInput()
{
	// TODO: Add your control notification handler code here
	CString formal=_T("");
	m_input_content.GetWindowText(formal);
	
	if(formal.GetLength()==0)
	{
		return;
	}
	formal.MakeLower();

	// 比较上次设置颜色的K线部分
	int32 iLastSetColorPos = 0;
	int32 iLastEndSetColorPos = formal.GetLength();	// 表示当前字串的尾部相同字串的开始坐标
	int32 iFormula1 = m_StrLastSetColorFormula.GetLength();
	int32 iFormula2 = formal.GetLength();
	int32 iMinLen = min(iFormula1, iFormula2);
	{
		int32 i = 0;
		for ( i=0; i < iMinLen ; ++i,++iLastSetColorPos )
		{
			if ( m_StrLastSetColorFormula.GetAt(i) != formal.GetAt(i) )
			{
				break;
			}
		}
		if ( iLastSetColorPos < m_StrLastSetColorFormula.GetLength() )
		{
			// 后面比较出尾部相同的地方, 以 ; 作为分割开始符
			int32 iEndLast = m_StrLastSetColorFormula.GetLength();
			for ( i=0; i < (iMinLen-iLastSetColorPos) ; ++i, --iLastEndSetColorPos, --iEndLast )
			{
				if ( m_StrLastSetColorFormula.GetAt(iEndLast-1) != formal.GetAt(iLastEndSetColorPos-1) )
				{
					break;
				}
			}
			// 找到语句结束符
			for ( ; iLastEndSetColorPos > 0 && iLastEndSetColorPos<formal.GetLength() ; ++iLastEndSetColorPos )
			{
				if ( formal.GetAt(iLastEndSetColorPos-1) == ';' )
				{
					break;
				}
			}
		}
	}
	if ( iLastSetColorPos>=formal.GetLength() )
	{
		return;	// 没有变化
	}
	m_StrLastSetColorFormula = formal;

	Kline kline;
	CFormularCompute equation(0,0,&kline,formal);
	equation.m_bShowColor = true;
	
	bool errpram = false;
	for(int i=0; i<PARAM_NUM; i++)
	{
		if(m_editParam[i][0]!=_T("")&&m_editParam[i][3]!=_T(""))
		{  
			float fpram=StringToFloat(m_editParam[i][3]);
			m_editParam[i][0].MakeLower();
			int addpra=equation.AddPara(m_editParam[i][0],fpram);
			switch(addpra)
			{
			case(1):
			case(2):
				errpram=true;
				break;
			}
			if(errpram == true)
			{
				MessageBox(_T("请先设置正确参数！"), AfxGetApp()->m_pszAppName, MB_ICONWARNING);
				return;
			}
		}
		else
		{
			break;
		}
	}
	equation.Devide();
	
	//取得位置颜色信息
	if(equation.m_errpos==-1 && equation.m_pKindPos )
	{
		//formal;
		COLORREF  clr[10] = {RGB(0,0,255),RGB(0,150,0),
			RGB(120,0,0),RGB(255,0,0),
			RGB(0,255,255),RGB(0,0,120)};
		
		m_input_content.SetRedraw(FALSE);
		long nStartChar,nEndChar;
		m_input_content.GetSel( nStartChar, nEndChar ) ;
		int32 iFirstLine = m_input_content.GetFirstVisibleLine();
	
// 		USES_CONVERSION;
// 		char* pStrFormal = W2A(formal);

		/*
		for(int k = 0;k<equation.m_nSizeKindPos ;k++)
		{
			int b = equation.m_pKindPos[k].m_nPos;
			int e ;
			if(k == equation.m_nSizeKindPos -1)
				e = formal.GetLength ()-1;
			else
				e = equation.m_pKindPos[k+1].m_nPos;
			if(e<0) e = 0;
			if(e>formal.GetLength ()-1)
				e = formal.GetLength ()-1;
			if(b<0) b = 0;
			if(b>formal.GetLength ()-1)
				b = formal.GetLength ()-1;
			
			m_input_content.SetSel(b,e);
			if(equation.m_pKindPos[k].m_nKind<10
				&&equation.m_pKindPos[k].m_nKind>=0)
				m_input_content.SetSelectColor( clr[equation.m_pKindPos[k].m_nKind]);			
		}
		*/
		//int32 iLen = strlen(pStrFormal);
		int32 iLen = formal.GetLength();

		for(int k = 0;k<equation.m_nSizeKindPos ;k++)
		{
			int b = equation.m_pKindPos[k].m_nPos;
			int e ;
			if(k == equation.m_nSizeKindPos -1)
				e = iLen;
			else
				e = equation.m_pKindPos[k+1].m_nPos;
			if(e<0) e = 0;
			if(e>iLen)
				e = iLen;
			if(b<0) b = 0;
			if(b>iLen)
				b = iLen;
			
			if ( (b>=iLastSetColorPos && e>=b)
				&& (e<=iLastEndSetColorPos) )
			{
				m_input_content.SetRESel(b,e);
				//m_input_content.SetSel(b,e);
				if(equation.m_pKindPos[k].m_nKind<10
					&&equation.m_pKindPos[k].m_nKind>=0)
				{
					m_input_content.SetSelectColor( clr[equation.m_pKindPos[k].m_nKind]);			
				}
			}
			else if ( b > iLastEndSetColorPos )
			{
				// 后面的相同，没必要判断了
				break;
			}
		}

		m_input_content.SetSel( nStartChar, nEndChar ) ;
		//	  m_input_content.HideSelection(TRUE, FALSE);
		int32 iFirstLine2 = m_input_content.GetFirstVisibleLine();
		if ( iFirstLine2==iFirstLine )
		{
			//SetSel(-1,-1);
			// 由于不能保证显示字符的绝对位置，所以尽量不调用这个
			//SetCaretPos( pt );	
		}
		else
		{
			// 尝试调整至同一行
		}
		
		m_input_content.SetRedraw();
		m_input_content.RedrawWindow();
	}
}

void CTaiKlineDlgEditScreenStock::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CRect r;
	if(m_input_content.m_hWnd )
	{
		GetDlgItem (IDC_EDIT41)->GetWindowRect(r);
		this->ScreenToClient (r);
		//		m_input_content.GetWindowRect(r);
		CRect r2(5,r.bottom+12,cx-5,cy-5);
		m_input_content.MoveWindow(r2);
	}
}

HBRUSH CTaiKlineDlgEditScreenStock::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if (nCtlColor == CTLCOLOR_STATIC && m_bNeedColorText)
	{	
		pDC->SetTextColor(RGB(0,0,255));
	}

	if (CTLCOLOR_BTN == nCtlColor)
	{
		pDC->SetBkMode(TRANSPARENT);
		HBRUSH solidBrush = CreateSolidBrush(GetBackColor());
		return solidBrush;
	}
	
	int32 iID = pWnd->GetDlgCtrlID();
	if (IDC_STATIC_NAME!=iID && IDC_STATIC_MIN!=iID && IDC_STATIC_MAX!=iID && IDC_STATIC_DEF!=iID  && IDC_STATIC_PARAM != iID )
	{
		if (CTLCOLOR_STATIC == nCtlColor)
		{
			HBRUSH solidBrush = CreateSolidBrush(GetBackColor());
			pDC->SetBkMode(TRANSPARENT);
			return solidBrush;
		}
	}
	
	return hbr;
}

///////////////////////////////////////////////////////////////////////////
