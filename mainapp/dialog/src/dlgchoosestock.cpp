// dlgchoosestock.cpp : implementation file
//

#include "stdafx.h"
#include "ShareFun.h"
#include "dlgchoosestock.h"
#include "dlgchoosestockblock.h"
#include "dialogprogresschoosestock.h"
#include "PathFactory.h"
//#include "OfflineDataManager.h"
#include "BlockManager.h"
#include "DlgChooseStockVar.h"
#include "IoViewShare.h"
#include <algorithm>
#include "DlgAddToBlock.h"
#include "dlgdownload.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
  
typedef struct T_CycleObject
{
public:
	T_CycleObject(E_NodeTimeInterval eCycle, const CString& StrCycleName)
	{
		m_eCycle = eCycle;
		m_StrCycleName = StrCycleName;
	}

	E_NodeTimeInterval m_eCycle;
	CString			   m_StrCycleName;
}T_CycleObject;

static const T_CycleObject s_aCycleObjects[] = 
{
	T_CycleObject(ENTIMinute,		L"1分钟"),
	T_CycleObject(ENTIDay,			L"日线"),
	T_CycleObject(ENTIMinute5,		L"5分钟"),
	T_CycleObject(ENTIMinute15,		L"15分钟"),
	T_CycleObject(ENTIMinute30,		L"30分钟"),
	T_CycleObject(ENTIMinute60,		L"60分钟"),
//	T_CycleObject(ENTIMinuteUser,	L"自定义分钟"),
//	T_CycleObject(ENTIDayUser,		L"自定义日线"),
	T_CycleObject(ENTIWeek,			L"周线"),
	T_CycleObject(ENTIMonth,		L"月线"),
	T_CycleObject(ENTIQuarter,		L"季线"),
	T_CycleObject(ENTIYear,			L"年线"),
};

static const int s_KiCycleCounts = sizeof(s_aCycleObjects) / sizeof(T_CycleObject);
/////////////////////////////////////////////////////////////////////////////
// CDlgChooseStock dialog

CDlgChooseStock::CDlgChooseStock(CWnd* pParent /*=NULL*/)
    : CDialogEx(CDlgChooseStock::IDD, pParent)
{
    //{{AFX_DATA_INIT(CDlgChooseStock)
    m_iTotal    = 0;
    m_iResult    = 0;
	m_StrNameP1 = _T("");
	m_StrNameP2 = _T("");
	m_StrNameP3 = _T("");
	m_StrNameP4 = _T("");
	m_TimeBegin = 0;
	m_TimeEnd = 0;
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
    
	//
	m_pFormulaLib = CFormulaLib::instance();
	m_bAndCondition = true;   
	m_aMerchToChoose.clear();
	m_aFormularsFinal.clear();
	m_pFomularNow = NULL;
	m_pSelectStockCenter = NULL;
	m_bWorking = false;
	m_bHistory = false;
}

CDlgChooseStock:: ~CDlgChooseStock()
{
    DEL(m_pSelectStockCenter);
}

void CDlgChooseStock::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDlgChooseStock)
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
    DDX_Control(pDX,IDC_COMBO_SEL_CONDITION,m_ComboCondition);
    DDX_Control(pDX,IDC_COMBO_CYCLE,m_ComboCycle);
    DDX_Control(pDX,IDC_LIST_CONDITION,m_ListCondition);
    DDX_Control(pDX,IDC_LIST_BLOCKALL,m_ListMerchAll);    
    DDX_Text(pDX,IDC_EDIT_TOTAL,m_iTotal);
    DDX_Text(pDX,IDC_EDIT_RESULT,m_iResult);
    DDX_Check(pDX,IDC_CHECK1,m_bHistory);
    DDX_Text(pDX, IDC_STATIC_PARAM1, m_StrNameP1);
	DDX_Text(pDX, IDC_STATIC_PARAM2, m_StrNameP2);
	DDX_Text(pDX, IDC_STATIC_PARAM3, m_StrNameP3);
	DDX_Text(pDX, IDC_STATIC_PARAM4, m_StrNameP4);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_TimeBegin);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER2, m_TimeEnd);
	DDX_Text(pDX, IDC_EDIT_PARAM, m_fP1);
	DDX_Text(pDX, IDC_EDIT_PARAM2, m_fP2);
	DDX_Text(pDX, IDC_EDIT_PARAM3, m_fP3);
	DDX_Text(pDX, IDC_EDIT_PARAM4, m_fP4);
	DDX_Text(pDX, IDC_STATIC_RANGE1, m_StrRangeP1);
	DDX_Text(pDX, IDC_STATIC_RANGE2, m_StrRangeP2);
	DDX_Text(pDX, IDC_STATIC_RANGE3, m_StrRangeP3);
	DDX_Text(pDX, IDC_STATIC_RANGE4, m_StrRangeP4);
	DDX_Radio(pDX, IDC_RADIO_P1, m_iRadio);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgChooseStock, CDialogEx)
//{{AFX_MSG_MAP(CDlgChooseStock)
	ON_BN_CLICKED(IDC_BUTTON_PROMPT, OnButtonPrompt)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_ADDTO, OnButtonAddto)
	ON_BN_CLICKED(IDC_BUTTON_DO, OnButtonDo)
	ON_CBN_SELCHANGE(IDC_COMBO_SEL_CONDITION, OnSelchangeComboCondition)	
	ON_BN_CLICKED(IDC_CHECK1, OnCheck)
	ON_BN_CLICKED(IDC_RADIO_AND, OnRadioAnd)
	ON_BN_CLICKED(IDC_RADIO_OR, OnRadioOr)
	ON_BN_CLICKED(IDC_BUTTON_CHANGEMERCH, OnButtonChangemerch)
	ON_BN_CLICKED(IDC_RADIO_P1, OnRadioP1)
	ON_BN_CLICKED(IDC_RADIO_P2, OnRadioP2)
	ON_BN_CLICKED(IDC_RADIO_P3, OnRadioP3)
	ON_BN_CLICKED(IDC_RADIO_P4, OnRadioP4)
	ON_BN_CLICKED(IDC_BUTTON_DEL_MERCH, OnButtonDelMerch)
	ON_MESSAGE(UM_Choose_Stock_Progress, OnMsgProgress)
	ON_EN_CHANGE(IDC_EDIT_PARAM, OnChangeEditParam)
	ON_EN_CHANGE(IDC_EDIT_PARAM2, OnChangeEditParam2)
	ON_EN_CHANGE(IDC_EDIT_PARAM3, OnChangeEditParam3)
	ON_EN_CHANGE(IDC_EDIT_PARAM4, OnChangeEditParam4)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgChooseStock message handlers
BOOL CDlgChooseStock::PreTranslateMessage(MSG* pMsg)
{
	if ( WM_KEYDOWN == pMsg->message )
	{
		if ( VK_RETURN == pMsg->wParam || VK_ESCAPE == pMsg->wParam )
		{			
			if ( StopChooseStock() )
			{
				PostMessage(WM_CLOSE, 0, 0);			
			}
			
			return TRUE;			
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

BOOL CDlgChooseStock::OnInitDialog()
{
    CDialog::OnInitDialog();
    
	m_pSelectStockCenter = new CSelectStockCenter(m_hWnd);

	// 默认沪深 A 股商品
	CBlockLikeMarket* p = CBlockConfig::Instance()->GetDefaultMarketClassBlock();
	if ( NULL != p )
	{
		for (int32 i = 0; i < p->m_blockInfo.m_aSubMerchs.GetSize(); i++)
		{
			if ( NULL != p->m_blockInfo.m_aSubMerchs.GetSize() )
			{
				//
				CMerch* pMerch = p->m_blockInfo.m_aSubMerchs.GetAt(i);
				if ( NULL != pMerch )
				{
					m_aMerchToChoose.insert(pMerch);
				}				
			}
		}
	}

	//
    FillComBoxFormular();
    FillComBoxCycle();
	FillListBox();	
    FillDateEdit();
	ReSetProgress();

	//
    ((CButton *)GetDlgItem(IDC_RADIO_AND))->SetCheck(TRUE);
	m_ComboCondition.SetCurSel(0);
	OnSelchangeComboCondition();

	//
	m_iTotal = m_aMerchToChoose.size();

	//
    UpdateData(FALSE);
    return TRUE;  
}


void CDlgChooseStock::FillComBoxFormular()
{
	// 取得所有的条件选股公式
	if ( NULL == m_pFormulaLib )
	{
		//ASSERT(0);
		return;
	}

	//
	CArray<CFormularContent*, CFormularContent*> aFormulars;
	m_pFormulaLib->GetFomulars(EFTCdtChose, true, aFormulars);

	for ( int32 i = 0; i < aFormulars.GetSize(); i++ )
	{
		CFormularContent* pFormular = aFormulars.GetAt(i);
		
		if ( NULL == pFormular )
		{
			continue;
		}

		//
		CString StrText;
		StrText.Format(L"%s - %s", pFormular->name.GetBuffer(), pFormular->explainBrief.GetBuffer());
		
		//
		m_ComboCondition.InsertString(-1, StrText);
		m_ComboCondition.SetItemData(i, (DWORD)pFormular);
	}	

	//
	if ( m_ComboCondition.GetCount() > 0 )
	{
		m_ComboCondition.SetCurSel(0);
	}	
}

void CDlgChooseStock::FillComBoxCycle()
{	
	// 周期选择框
	for ( int32 i = 0; i < s_KiCycleCounts; i++ )
	{
		m_ComboCycle.InsertString(-1, s_aCycleObjects[i].m_StrCycleName);
	}

	//
	m_ComboCycle.SetCurSel(1);
}

void CDlgChooseStock::FillListBox()
{
	// 填充为所有商品
	
	// 先清空
	while ( m_ListMerchAll.GetCount() > 0 )
	{
		m_ListMerchAll.DeleteString(0);
	}

	//
	for ( set<CMerch*, MerchCmp>::iterator it = m_aMerchToChoose.begin(); it != m_aMerchToChoose.end(); ++it )
	{
		CMerch* pMerch = *it;
		if ( NULL == pMerch )
		{
			continue;
		}
	
		//
		m_ListMerchAll.InsertString(-1, pMerch->m_MerchInfo.m_StrMerchCnName);
	}	

	//
	m_iTotal = m_aMerchToChoose.size();
}

void CDlgChooseStock::FillDateEdit()
{
	// 初始化日期控件	
	m_TimeEnd = CTime::GetCurrentTime();

	CTimeSpan TimeSpan(7, 0, 0, 0);
	
	m_TimeBegin	= m_TimeEnd - TimeSpan;
}

void CDlgChooseStock::ReSetProgress()
{
	if ( m_Progress.GetSafeHwnd() )
	{
		m_Progress.SetForeColour(RGB(0, 255, 0));
		m_Progress.SetPos(0);
		m_Progress.SetWindowText(L"");
		
		RedrawWindow();
	}	
}

bool32 CDlgChooseStock::BeginChooseStock()
{	
	CString StrErr;
	if ( !ValidChooseParams(StrErr) )
	{
		MessageBox(StrErr, AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return false;
	}

	UpdateData(TRUE);
	int32 iCurSel = m_ComboCycle.GetCurSel();
	if ( iCurSel < 0 || iCurSel >= m_ComboCycle.GetCount() )
	{
		return false;
	}

	//
	if ( NULL != m_pSelectStockCenter )
	{
		//
		m_bWorking = true;
		
		// 
		GetDlgItem(IDC_BUTTON_DO)->SetWindowText(L"停止选股");

		//
		m_Progress.SetRange(0, m_aMerchToChoose.size());
		m_Progress.SetPos(0);

		T_ConditionSelectParam stParam;

		stParam.m_aFormulars = m_aFormularsFinal;
		stParam.m_aMerchs	 = m_aMerchToChoose;
		stParam.m_eInterval	 = s_aCycleObjects[iCurSel].m_eCycle;
		stParam.m_TimeBegin	 = m_TimeBegin;
		stParam.m_TimeEnd	 = m_TimeEnd;
		stParam.m_bHistory   = m_bHistory;
		stParam.m_bAndCondition = m_bAndCondition;
		
		m_pSelectStockCenter->SetConditionChooseParam(stParam);
		m_pSelectStockCenter->StartWork();
	}

    return true;
}

bool32 CDlgChooseStock::StopChooseStock()
{
	if ( !m_bWorking )
	{
		return true;
	}

	//
	if ( IDNO == MessageBox(L"是否停止选股?", AfxGetApp()->m_pszAppName, MB_YESNO) )
	{
		return false;
	}
	
	//
	if ( NULL != m_pSelectStockCenter )
	{
		m_pSelectStockCenter->StopWork();
	}

	return true;
}

void CDlgChooseStock::OnFinishChooseStock()
{
	if ( NULL == m_pSelectStockCenter )
	{
		return;
	}

	//
	m_bWorking = false;

	//
	set<CMerch*, MerchCmp> aResults;
	m_pSelectStockCenter->GetSelectResult(aResults);

	// 
	GetDlgItem(IDC_BUTTON_DO)->SetWindowText(L"执行选股");

	//
	m_iResult = aResults.size();
	UpdateData(FALSE);

	//	
	T_BlockDesc stBlockDesc;
	stBlockDesc.m_iMarketId = -1;
	stBlockDesc.m_eType = T_BlockDesc::EBTUser;
	stBlockDesc.m_StrBlockName = m_BlockFinal.m_StrName;

	// 更新这个板块的商品
	CArray<CMerch*, CMerch*> aMerchs;
	for ( set<CMerch*, MerchCmp>::iterator it = aResults.begin(); it != aResults.end(); ++it )
	{
		CMerch* pMerchNow = *it;
		if ( NULL != pMerchNow )
		{
			TRACE(L"选股结果: %d %s \n", pMerchNow->m_MerchInfo.m_iMarketId, pMerchNow->m_MerchInfo.m_StrMerchCode.GetBuffer());
			aMerchs.Add(pMerchNow);
		}	
	}

	// 更新这个自选股的信息
	CUserBlockManager::Instance()->ReplaceUserBlockMerchs(aMerchs, stBlockDesc.m_StrBlockName, false);

	// 打开这个板块
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if ( NULL != pMainFrame )
	{
		pMainFrame->OnSpecifyBlock(stBlockDesc, false);
	}	
}
/////////////////////////////////////////////////////////////////////////
// 控件函数
void CDlgChooseStock::OnButtonPrompt() 
{
    int32 iCurSel = m_ComboCondition.GetCurSel();
    
    if ( iCurSel < 0 || iCurSel >= m_ComboCondition.GetCount() )
    {
        return;
    }
    
	CFormularContent* pFormular = (CFormularContent*)m_ComboCondition.GetItemData(iCurSel);
	
	if ( NULL != pFormular )
	{		
		if ( pFormular->help.IsEmpty() )
		{
			MessageBox(L"此指标无用法注释", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		}
		else
		{
			MessageBox(pFormular->help, AfxGetApp()->m_pszAppName, MB_ICONINFORMATION);    
		}		
	}    
}

void CDlgChooseStock::OnRadioAnd() 
{
    m_bAndCondition = true;    
}

void CDlgChooseStock::OnRadioOr() 
{
    m_bAndCondition = false;    
}

void CDlgChooseStock::OnRadioP1() 
{
	SetRadioParam(0);
}

void CDlgChooseStock::OnRadioP2() 
{
	SetRadioParam(1);
}

void CDlgChooseStock::OnRadioP3() 
{
	SetRadioParam(2);	
}

void CDlgChooseStock::OnRadioP4() 
{
	SetRadioParam(3);	
}

void CDlgChooseStock::OnButtonAdd() 
{
    UpdateData(true);

    int32 iCurSel = m_ComboCondition.GetCurSel(); 
	if ( iCurSel < 0 || iCurSel >= m_ComboCondition.GetCount() )
	{
		return;
	}

	//
	CFormularContent* pFormular = (CFormularContent*)m_ComboCondition.GetItemData(iCurSel);
	if ( NULL == pFormular )
	{
		return;
	}

	// 
	if ( m_aFormularsFinal.count(pFormular) == 0 )
	{
		m_aFormularsFinal.insert(pFormular);
		m_ListCondition.InsertString(m_ListCondition.GetCount(), pFormular->name);
		m_ListCondition.SetItemData(m_ListCondition.GetCount() - 1, (DWORD)pFormular);
	}
}

void CDlgChooseStock::OnButtonDel() 
{
    int32 iCurSel = m_ListCondition.GetCurSel();
    
    if ( iCurSel < 0 || iCurSel >= m_ListCondition.GetCount() )
    {
        return;
    }
    
	//
	CFormularContent* pFormular = (CFormularContent*)m_ListCondition.GetItemData(iCurSel);
	if ( NULL == pFormular )
	{
		return;
	}

	//
    m_ListCondition.DeleteString(iCurSel);
	m_aFormularsFinal.erase(pFormular);
}


void CDlgChooseStock::OnButtonChangemerch() 
{
	// 选择商品
	CDlgChooseStockVar::MerchArray aMerchs;
	if ( CDlgChooseStockVar::ChooseStockVar(aMerchs, true, NULL, this) )
	{
		set<CMerch*, MerchCmp> aMerchNow;
		
		//
		for ( int32 i = 0; i < aMerchs.GetSize(); i++ )
		{
			aMerchNow.insert(aMerchs[i]);
		}

		//
		set<CMerch*, MerchCmp> tmp;
		//lint --e{1025}
		set_union(m_aMerchToChoose.begin(), m_aMerchToChoose.end(), aMerchNow.begin(), aMerchNow.end(), inserter(tmp, tmp.end()));
		m_aMerchToChoose = tmp;
	}
	
	//
	FillListBox();
	UpdateData(FALSE);
}

void CDlgChooseStock::OnButtonDelMerch() 
{
	if ( IDNO == MessageBox(L"确认清空所有商品?", AfxGetApp()->m_pszAppName, MB_YESNO) )
	{
		return;
	}

	//
	m_aMerchToChoose.clear();
	FillListBox();
	UpdateData(FALSE);
}

void CDlgChooseStock::OnButtonAddto() 
{
	T_Block block;
	if ( CDlgAddToBlock::GetUserSelBlock(block) )
	{
		m_BlockFinal = block;		
	}
}

void CDlgChooseStock::OnButtonDo() 
{
	if ( !m_bWorking )
	{		
		// 先判断数据是否完整:
		bool32 bLocalDataEnough = false;

		//
		int32 iCurSel = m_ComboCycle.GetCurSel();
		if ( iCurSel < 0 || iCurSel >= m_ComboCycle.GetCount() )
		{
			//ASSERT(0);
			return;
		}

		//
		T_CycleObject st = s_aCycleObjects[iCurSel];

		CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
		if ( NULL != pDoc && NULL != pDoc->m_pAbsCenterManager )
		{
			int32 iScale;
			E_KLineTypeBase eTypeBase;
			E_NodeTimeInterval eNodeTimeIntervalCompare;

			//
			GetTimeIntervalInfo(0, 0, st.m_eCycle, eNodeTimeIntervalCompare, eTypeBase, iScale);

			// ...fangz 0127 目前脱机只有这几个基本周期
			if ( EKTBHour == eTypeBase )
			{
				eTypeBase = EKTB5Min;
			}
			else if ( EKTBMonth == eTypeBase )
			{
				eTypeBase = EKTBDay;
			}
			
			//
			bLocalDataEnough = pDoc->m_pAbsCenterManager->BeLocalOfflineDataEnough(eTypeBase);		
		}

		if ( !bLocalDataEnough )
		{
			if ( IDYES == MessageBox(L"本地脱机数据不完整, 是否下载?", AfxGetApp()->m_pszAppName, MB_YESNO) )
			{
				// 弹出下载对话框
				CDlgDownLoad dlg(true);			
				dlg.DoModal();
			}
		}

		// 开始选股
		BeginChooseStock();
	}
	else
	{
		StopChooseStock();
	}   
}

void CDlgChooseStock::OnSelchangeComboCondition()
{
	int32 iCurSel = m_ComboCondition.GetCurSel();
	if ( iCurSel < 0 || iCurSel >= m_ComboCondition.GetCount() )
	{
		return;
	}

	//
	m_pFomularNow = (CFormularContent*)m_ComboCondition.GetItemData(iCurSel);
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

void CDlgChooseStock::OnCheck() 
{
    m_bHistory = !m_bHistory;

    if (m_bHistory)
    {
		MessageBox(L"历史阶段选股的含义:\n如果在设置的时间范围满足条件(不只是当天满足), 则选出这个商品", AfxGetApp()->m_pszAppName, MB_ICONINFORMATION);
		GetDlgItem(IDC_DATETIMEPICKER1)->EnableWindow(TRUE);
		GetDlgItem(IDC_DATETIMEPICKER2)->EnableWindow(TRUE);
    }
    else
    {
		GetDlgItem(IDC_DATETIMEPICKER1)->EnableWindow(FALSE);
		GetDlgItem(IDC_DATETIMEPICKER2)->EnableWindow(FALSE);
    }
}

void CDlgChooseStock::SetRadioParam(int32 iIndex)
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

void CDlgChooseStock::ModifyFormular(int32 iIndexParam)
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

bool32 CDlgChooseStock::ValidChooseParams(OUT CString& StrErr)
{
	StrErr.Empty();

	// 判断条件
	if ( m_aFormularsFinal.empty() )
	{
		StrErr = L"请设置选股条件.";
        return false;
	}
	
    // 判断选股范围    
    if ( m_aMerchToChoose.empty() )
    {
        StrErr = L"请设置选股商品.";
        return false;
    }
	
    // 判断时间
    if ( m_bHistory )
    {
		if ( m_TimeBegin > m_TimeEnd )
		{
			StrErr = L"请设置正确的时间.";
			return false;
		}
    }
	
    // 判断选股入板块
    if ( m_BlockFinal.m_StrName.IsEmpty()
		|| CUserBlockManager::Instance()->GetBlock(m_BlockFinal.m_StrName) == NULL )
    {
        StrErr = L"请设置选股入板块!";
        return false;
    }

	return true;
}

LRESULT CDlgChooseStock::OnMsgProgress(WPARAM wParam , LPARAM lParam)
{
	CString* pStrMsg = (CString*)wParam;
	CString StrMsg = *pStrMsg;
	DEL(pStrMsg);

	//
	if ( 0 == int32(lParam) )
	{
		m_Progress.StepIt();
		
		CString StrPos = m_Progress.GetPosText();
		CString StrShow;
		StrShow.Format(L"%s [%s]", StrMsg.GetBuffer(), StrPos.GetBuffer());
		m_Progress.SetWindowText(StrShow);	
	}
	else if ( 1 == int32(lParam) )
	{
		ReSetProgress();
		m_Progress.SetWindowText(StrMsg);

		OnFinishChooseStock();
	}

	return 1;
}

void CDlgChooseStock::OnChangeEditParam() 
{
	ModifyFormular(0);
}

void CDlgChooseStock::OnChangeEditParam2() 
{
	ModifyFormular(1);
}

void CDlgChooseStock::OnChangeEditParam3() 
{
	ModifyFormular(2);	
}

void CDlgChooseStock::OnChangeEditParam4() 
{
	ModifyFormular(3);
}

void CDlgChooseStock::OnClose()
{
	StopChooseStock();
	//
	CDialogEx::OnClose();
}
