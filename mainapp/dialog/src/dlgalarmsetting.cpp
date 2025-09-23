// dlgalarmsetting.cpp : implementation file
#include "StdAfx.h"
#include "dlgalarmsetting.h"
#include "dlgalarmmodify.h"
#include "dlgalarmformular.h"
#include "DlgChooseStockVar.h"
#include "DlgArbitrage.h"
#include "dlgresponsibility.h"
#include "dlgarbitragesel.h"
#include "PathFactory.h"

// 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAlarmSetting dialog

static const int32 s_KiAddArbitrage	= 321;
static const int32 s_KiAddMerch		= 322;

CDlgAlarmSetting::CDlgAlarmSetting(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgAlarmSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAlarmSetting)
	m_RadioType = 0;
	m_RadioPrompt = 0;
	//}}AFX_DATA_INIT

	m_pAlarmCenter = NULL;

	m_bCheck7 = FALSE;
	m_bCheck8 = FALSE;

	//
	CGGTongDoc* pDoc = AfxGetDocument();
	m_pAbsCenterManager = pDoc->m_pAbsCenterManager;

	m_eListenType = ECTReqRealtimePrice;
	if ( NULL != m_pAbsCenterManager )
	{
		m_pAbsCenterManager->AddViewDataListner(this);
	}

	SetNeedChangeColor(true);
	SetCaptionBKColor(RGB(250, 250, 246));
	SetFrameColor(RGB(102, 102, 102));
	SetCaptionColor(RGB(10, 0, 4));
}

CDlgAlarmSetting::~CDlgAlarmSetting()
{
	if ( NULL != m_pAbsCenterManager )
	{
		m_pAbsCenterManager->DelViewDataListner(this);
	}
}

void CDlgAlarmSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAlarmSetting)
	DDX_Control(pDX, IDC_LIST_MERCH, m_ListMerch);
	DDX_Control(pDX, IDC_LIST_FORMULAR, m_ListFormular);
	DDX_Control(pDX, IDC_LIST_ARBITRAGE, m_ListArbitrage);
	DDX_Radio(pDX, IDC_RADIO1, m_RadioType);
	DDX_Radio(pDX, IDC_RADIO4, m_RadioPrompt);
	DDX_Check(pDX, IDC_CHECK7, m_bCheck7);
	DDX_Check(pDX, IDC_CHECK8, m_bCheck8);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAlarmSetting, CDialogEx)
	//{{AFX_MSG_MAP(CDlgAlarmSetting)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, OnButtonModify)
	ON_BN_CLICKED(IDC_BUTTON_REMOVEALL, OnButtonRemoveall)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_BN_CLICKED(IDC_RADIO8, OnRadio9)	
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_MERCH, OnDblclkListMerch)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FORMULAR, OnDblclkListFormular)
	ON_BN_CLICKED(IDC_RADIO4, OnRadio4)
	ON_BN_CLICKED(IDC_RADIO5, OnRadio5)
	ON_BN_CLICKED(IDC_RADIO6, OnRadio6)
	ON_BN_CLICKED(IDC_CHECK7, OnRadio7)
	ON_BN_CLICKED(IDC_CHECK8, OnRadio8)
	ON_COMMAND_RANGE(s_KiAddArbitrage, s_KiAddMerch, OnMenu)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAlarmSetting message handlers
BOOL CDlgAlarmSetting::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	DWORD styles = LVS_EX_FULLROWSELECT;

	//
	//lint --e{522}
	ListView_SetExtendedListViewStyleEx(m_ListMerch.m_hWnd, styles, styles);
	m_ListMerch.InsertColumn(0, L"品种代码", LVCFMT_CENTER, 90);
	m_ListMerch.InsertColumn(1, L"品种名称", LVCFMT_CENTER, 90);
	m_ListMerch.InsertColumn(2, L"昨收价", LVCFMT_CENTER, 90);
	m_ListMerch.InsertColumn(3, L"上破价", LVCFMT_CENTER, 90);
	m_ListMerch.InsertColumn(4, L"下破价", LVCFMT_CENTER, 90);
	m_ListMerch.InsertColumn(5, L"涨幅(%)", LVCFMT_CENTER, 60);
	m_ListMerch.InsertColumn(6, L"换手(%)", LVCFMT_CENTER, 60);
	m_ListMerch.InsertColumn(7, L"均线", LVCFMT_CENTER, 90);

//	m_ListMerch.InsertColumn(7, L"折溢(%)", LVCFMT_CENTER, 60);

	//
	ListView_SetExtendedListViewStyleEx(m_ListFormular.m_hWnd, styles, styles);
	m_ListFormular.InsertColumn(0, L"预警公式", LVCFMT_CENTER, 200);
	m_ListFormular.InsertColumn(1, L"公式参数", LVCFMT_CENTER, 300);


	ListView_SetExtendedListViewStyleEx(m_ListArbitrage.m_hWnd, styles, styles);
	m_ListArbitrage.InsertColumn(0, L"套利品种", LVCFMT_CENTER, 200);
	m_ListArbitrage.InsertColumn(1, L"上破价", LVCFMT_CENTER, 90);
	m_ListArbitrage.InsertColumn(2, L"下破价", LVCFMT_CENTER, 90);
	//
	if ( NULL != m_pAlarmCenter )
	{
		CAlarmCenter::E_AlarmType eType = m_pAlarmCenter->GetAlarmtType();
		m_RadioPrompt = AlarmType2Radio(eType);
		m_bCheck7 = m_pAlarmCenter->GetAlarmDlgType();
		m_bCheck8 = m_pAlarmCenter->GetAlarmStatusType();
	}
	
	//
	ShowCtrls(0);
	UpdateCtrls();
	UpdateData(FALSE);

	//
	return TRUE;
}

void CDlgAlarmSetting::OnRealtimePrice(const CRealtimePrice &RealtimePrice, int32 iCommunicationId, int32 iReqId)
{
	if ( NULL == m_pAbsCenterManager )
	{
		return;
	}

	//
	CMerch* pMerch = NULL;	
	m_pAbsCenterManager->GetMerchManager().FindMerch(RealtimePrice.m_StrMerchCode, RealtimePrice.m_iMarketId, pMerch);

	//
	for ( int32 i = 0; i < m_ListMerch.GetItemCount(); i++ )
	{
		CMerch* pMerchList = (CMerch*)m_ListMerch.GetItemData(i);

		if ( NULL == pMerchList )
		{
			continue;
		}

		//
		TCHAR StrTextNow[100];
		memset(StrTextNow, 0, sizeof(TCHAR) * 100);
		m_ListMerch.GetItemText(i, 2, StrTextNow, 100);

		if ( (pMerchList == pMerch) && (CString)StrTextNow == L"-" )
		{
			// 更新这一行的价格
			CString StrText = Float2String(pMerch->m_pRealtimePrice->m_fPricePrevClose, pMerch->m_MerchInfo.m_iSaveDec);
			m_ListMerch.SetItemText(i, 2, StrText);
		}
	}
}

void CDlgAlarmSetting::FillListMerch()
{
	if ( NULL == m_pAlarmCenter )
	{
		//ASSERT(0);
		return;
	}

	//
	mapMerchAlarms mapAlarms;
	m_pAlarmCenter->GetAlarmMap(mapAlarms);
	
	//
	m_ListMerch.DeleteAllItems();
	
	//
	for ( mapMerchAlarms::iterator it = mapAlarms.begin(); it != mapAlarms.end(); ++it )
	{
		CMerch* pMerch = it->first;
		CAlarmConditions stAlarmConditions = it->second;
		
		if ( NULL == pMerch )
		{
			continue;
		}
		
		//
		int32 iCount = m_ListMerch.GetItemCount();
		int32 iIndex = m_ListMerch.InsertItem(iCount, pMerch->m_MerchInfo.m_StrMerchCode);
		
		// 名称
		m_ListMerch.SetItemText(iIndex, 1, pMerch->m_MerchInfo.m_StrMerchCnName);
		
		// 昨收
		CString StrText = L"-";
		if ( NULL != pMerch->m_pRealtimePrice )
		{
			StrText = Float2String(pMerch->m_pRealtimePrice->m_fPricePrevClose, pMerch->m_MerchInfo.m_iSaveDec);
		}
		m_ListMerch.SetItemText(iIndex, 2, StrText);
		
		// 上破价
		StrText = Float2String(stAlarmConditions.m_aFixedParams[0], pMerch->m_MerchInfo.m_iSaveDec);
		m_ListMerch.SetItemText(iIndex, 3, StrText);
		
		// 上破价
		StrText = Float2String(stAlarmConditions.m_aFixedParams[1], pMerch->m_MerchInfo.m_iSaveDec);
		m_ListMerch.SetItemText(iIndex, 4, StrText);

		// 涨幅
		StrText = Float2String(stAlarmConditions.m_aFixedParams[2], pMerch->m_MerchInfo.m_iSaveDec);
		m_ListMerch.SetItemText(iIndex, 5, StrText);

		// 换手率(%)
		StrText = Float2String(stAlarmConditions.m_aFixedParams[3], pMerch->m_MerchInfo.m_iSaveDec);
		m_ListMerch.SetItemText(iIndex, 6, StrText);
		
		// 均线
		StrText = stAlarmConditions.m_aFixedParams[4] == 1.0 ? L"开" : L"关";
		m_ListMerch.SetItemText(iIndex, 7, StrText);
 		
// 		// ETF
// 		StrText = Float2String(stAlarmConditions.m_aFixedParams[4], pMerch->m_MerchInfo.m_iSaveDec, false, true, true);
// 		m_ListMerch.SetItemText(iIndex, 7, StrText);
		
		// 自定义数据
		m_ListMerch.SetItemData(iIndex, (DWORD)pMerch);
	}

	//
	m_ListMerch.RedrawWindow();
	m_ListMerch.SetFocus();
	UpdateData(FALSE);	
}

void CDlgAlarmSetting::FillListFormular()
{
	if ( NULL == m_pAlarmCenter )
	{
		//ASSERT(0);
		return;
	}

	//
	mapMerchAlarms mapAlarms;
	m_pAlarmCenter->GetAlarmMap(mapAlarms);
	
	CArray<CFormularContent*, CFormularContent*> aAlarmFormulars;
	m_pAlarmCenter->GetAlarmFormulars(aAlarmFormulars);

	//
	m_ListFormular.DeleteAllItems();
	
	//	
	for ( int32 i = 0; i < aAlarmFormulars.GetSize(); i++ )
	{
		CFormularContent* pContent = aAlarmFormulars.GetAt(i);
		if ( NULL == pContent )
		{
			continue;
		}
		
		//
		int32 iCounts = m_ListFormular.GetItemCount();
		int32 iIndex = m_ListFormular.InsertItem(iCounts, pContent->name);
		
		//
		CString StrParam;
		for ( int32 j = 0; j < pContent->numPara; j++ )
		{
			CString StrNow;
			if ( j == pContent->numPara - 1 )
			{
				StrNow.Format(L"%.2f", pContent->defaultVal[j]);
			}
			else
			{
				StrNow.Format(L"%.2f, ", pContent->defaultVal[j]);
			}
			
			StrParam += StrNow;
		}
		
		m_ListFormular.SetItemText(iIndex, 1, StrParam);
		
		//
		m_ListFormular.SetItemData(iIndex, (DWORD)pContent);
	}

	//
	m_ListFormular.RedrawWindow();
	m_ListFormular.SetFocus();
	UpdateData(FALSE);
}

void CDlgAlarmSetting::FillListArbitrage()
{	
	if ( NULL == m_pAlarmCenter )
	{
		//ASSERT(0);
		return;
	}

	// 套利的商品	
	mapArbitrageAlarms mapArbitrage;
	m_pAlarmCenter->GetArbitrageMap(mapArbitrage);

	m_ListArbitrage.DeleteAllItems();

	//
	for ( mapArbitrageAlarms::iterator it2 = mapArbitrage.begin(); it2 != mapArbitrage.end(); ++it2 )
	{
		CArbitrage* pstArbitrage = it2->first;
		CAlarmConditions stAlarmConditions = it2->second;

		if ( NULL == pstArbitrage )
		{
			continue;
		}
		
		CString StrText = L"-";
		//
		int32 iCount = m_ListArbitrage.GetItemCount();
		
		// 套利品种
		int32 iIndex = m_ListArbitrage.InsertItem(iCount,pstArbitrage->GetShowName());
		
		// 上破价
		StrText = Float2String(stAlarmConditions.m_aFixedParams[0], pstArbitrage->m_iSaveDec);
		m_ListArbitrage.SetItemText(iIndex, 1, StrText);

		// 下破价
		StrText = Float2String(stAlarmConditions.m_aFixedParams[1], pstArbitrage->m_iSaveDec);
		m_ListArbitrage.SetItemText(iIndex, 2, StrText);

		// 自定义数据
		m_ListArbitrage.SetItemData(iIndex, (DWORD)pstArbitrage);
	}

	//
	m_ListArbitrage.RedrawWindow();
	m_ListArbitrage.SetFocus();
	UpdateData(FALSE);
}

void CDlgAlarmSetting::UpdateCtrls(int32 iRadio /* = -1 */)
{
	//
	if ( 0 == iRadio )
	{
		FillListMerch();
		
	}
	else if ( 1 == iRadio )
	{
		FillListFormular();		
	}
	else if (2 == iRadio)
	{
		FillListArbitrage();
	}
	else
	{
		FillListMerch();
		FillListFormular();
		FillListArbitrage();
	}
}

void CDlgAlarmSetting::ShowCtrls(int32 iRadio)
{
	if ( 0 == iRadio )
	{
		m_ListMerch.ShowWindow(SW_SHOW);
		m_ListFormular.ShowWindow(SW_HIDE);
		m_ListArbitrage.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_ADD)->SetWindowText(L"添加品种");
		GetDlgItem(IDC_BUTTON_ADD)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_MODIFY)->SetWindowText(L"修改品种");
		GetDlgItem(IDC_BUTTON_MODIFY)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_DEL)->SetWindowText(L"删除品种");
		GetDlgItem(IDC_BUTTON_DEL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_REMOVEALL)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_RADIO4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK8)->ShowWindow(SW_HIDE);
		
	}
	else if ( 1 == iRadio )
	{
		m_ListMerch.ShowWindow(SW_HIDE);
		m_ListFormular.ShowWindow(SW_SHOW);
		m_ListArbitrage.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_ADD)->SetWindowText(L"添加公式");
		GetDlgItem(IDC_BUTTON_ADD)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_MODIFY)->SetWindowText(L"修改设置");
		GetDlgItem(IDC_BUTTON_MODIFY)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_DEL)->SetWindowText(L"删除公式");
		GetDlgItem(IDC_BUTTON_DEL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_REMOVEALL)->ShowWindow(SW_SHOW);
		
		GetDlgItem(IDC_RADIO4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK8)->ShowWindow(SW_HIDE);
	}
	else if ( 2 == iRadio)
	{
		m_ListMerch.ShowWindow(SW_HIDE);
		m_ListFormular.ShowWindow(SW_HIDE);
		m_ListArbitrage.ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_ADD)->SetWindowText(L"添加条件");
		GetDlgItem(IDC_BUTTON_ADD)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_MODIFY)->SetWindowText(L"修改设置");
		GetDlgItem(IDC_BUTTON_MODIFY)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_DEL)->SetWindowText(L"删除条件");
		GetDlgItem(IDC_BUTTON_DEL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_REMOVEALL)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_RADIO4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK8)->ShowWindow(SW_HIDE);
	}
	else if ( 3 == iRadio )
	{
		m_ListMerch.ShowWindow(SW_HIDE);
		m_ListFormular.ShowWindow(SW_HIDE);
		m_ListArbitrage.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_ADD)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_MODIFY)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_DEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_REMOVEALL)->ShowWindow(SW_HIDE);
		
		GetDlgItem(IDC_RADIO4)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RADIO5)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RADIO6)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHECK7)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHECK8)->ShowWindow(SW_SHOW);
	}
}

CAlarmCenter::E_AlarmType CDlgAlarmSetting::Radio2AlarmType(int32 iRadio)
{	
	CAlarmCenter::E_AlarmType eType = CAlarmCenter::EATCounts;

	if ( 0 == iRadio )
	{
		eType = CAlarmCenter::EATNoSound;
	}
	else if ( 1 == iRadio )
	{
		eType = CAlarmCenter::EATSysSound;
	}
	else if ( 2 == iRadio )
	{
		eType = CAlarmCenter::EATCustomSound;
	}
	else if ( 3 == iRadio )
	{
		eType = CAlarmCenter::EATStatus;
	}
	else if ( 4 == iRadio )
	{
		eType = CAlarmCenter::EATDialog;
	}
	else
	{
		//ASSERT(0);
	}

	return eType;
}

int32 CDlgAlarmSetting::AlarmType2Radio(CAlarmCenter::E_AlarmType eType)
{
	int32 iRadio = 0;
	
	if ( CAlarmCenter::EATNoSound == eType )
	{
		iRadio = 0;
	}
	else if ( CAlarmCenter::EATSysSound == eType )
	{
		iRadio = 1;
	}
	else if ( CAlarmCenter::EATCustomSound == eType )
	{
		iRadio = 2;
	}
	else if ( CAlarmCenter::EATStatus == eType )
	{
		iRadio = 3;
	}
	else if ( CAlarmCenter::EATDialog == eType )
	{
		iRadio = 4;
	}
	else
	{
		//ASSERT(0);
	}
	
	return iRadio;
}

void CDlgAlarmSetting::OnButtonAdd() 
{
	UpdateData(TRUE);

	if ( NULL == m_pAlarmCenter )
	{
		return;
	}

	if ( 0 == m_RadioType )
	{
		// 增加普通商品预警
		CBlockConfig::MerchArray aMerchs;
		if ( CDlgChooseStockVar::ChooseStockVar(aMerchs, false) )
		{
			if ( aMerchs.GetSize() <= 0 )
			{
				return;
			}
		}
		else
		{
			return;
		}

		//
		CDlgAlarmModify Dlg(this);

		if ( IDOK == Dlg.DoModal() )
		{
			CAlarmConditions stAlarmConditions;
			Dlg.GetAlarmConditions(stAlarmConditions);

			//
			if (m_pAlarmCenter != NULL)
			{
				m_pAlarmCenter->AddMerchAlarms(aMerchs[0], stAlarmConditions);
				FillListMerch();
				m_pAlarmCenter->ToXml();
			}
		}	
	}
	else if ( 1 == m_RadioType)
	{
		// 增加公式
		CDlgAlarmFormular Dlg;
		if ( IDOK == Dlg.DoModal() )
		{
			CFormularContent* pFormular = Dlg.GetFormular();
			if ( NULL == pFormular )
			{
				return;
			}

			//
			m_pAlarmCenter->AddFormularAlarms(pFormular);
		}
	}
	else
	{
		CArray<CArbitrage, CArbitrage&> aArbitrages;
		CArbitrageManage::Instance()->GetArbitrageDatas(aArbitrages);
		if ( aArbitrages.GetSize() <= 0 )
		{
			// 设置套利商品
			CDlgArbitrage Dlg;
			Dlg.SetInitParams(true, NULL);
			Dlg.DoModal();
		}

		// 增加套利预警
		CDlgArbitrageSel Dlg;
		CArbitrage* pstSel = NULL;
		if ( IDOK == Dlg.DoModal() )
		{
			pstSel = Dlg.GetSelArbitrage();
		}
		else
		{
			return;
		}

		//
		CDlgAlarmModify DlgAlarm(this,EAT_ARBITRAGE);

		if ( IDOK == DlgAlarm.DoModal() )
		{
			CAlarmConditions stAlarmConditions;
			DlgAlarm.GetAlarmConditions(stAlarmConditions);

			//
			if (m_pAlarmCenter != NULL)
			{
				m_pAlarmCenter->AddArbitrageAlarms(pstSel, stAlarmConditions);
				FillListArbitrage();
			}
		}	
	}

	//
	UpdateCtrls(m_RadioType);

	//
	m_pAlarmCenter->ToXml();
}

void CDlgAlarmSetting::OnButtonDel() 
{
	UpdateData(TRUE);
	
	if ( NULL == m_pAlarmCenter )
	{
		return;
	}

	//
	if ( 0 == m_RadioType )
	{
		int id = m_ListMerch.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);	
		if ( -1 == id )
		{
			MessageBox(L"请选择商品", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
			return;
		}
		
		mapMerchAlarms mapMerchs;
		m_pAlarmCenter->GetAlarmMap(mapMerchs);
		int32 iSize = mapMerchs.size();

		if ( id < iSize )
		{
			//
			CMerch* pMerch = (CMerch*)m_ListMerch.GetItemData(id);
			if ( NULL == pMerch )
			{
				return;
			}
			
			//
			m_pAlarmCenter->DelMerchAlarms(pMerch);
		}

		//
		FillListMerch();

		//
		if ( id >= m_ListMerch.GetItemCount() )
		{
			id = m_ListMerch.GetItemCount() - 1;
		}

		UINT n = m_ListMerch.GetItemState(id, (UINT)-1);
		m_ListMerch.SetItemState(id, n | LVIS_SELECTED | LVIS_FOCUSED, (UINT)-1);
		m_ListMerch.EnsureVisible(id, FALSE);	
		m_ListMerch.SetFocus();
	}
	else if ( 1 == m_RadioType )
	{
		int id = m_ListFormular.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);	
		if ( -1 == id )
		{
			MessageBox(L"请选择公式", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
			return;
		}
		
		//
		CFormularContent* pContent = (CFormularContent*)m_ListFormular.GetItemData(id);
		if ( NULL == pContent )
		{
			return;
		}
		
		//
		m_pAlarmCenter->DelFormularAlarms(pContent);
		FillListFormular();

		//
		if ( id >= m_ListFormular.GetItemCount() )
		{
			id = m_ListFormular.GetItemCount() - 1;
		}
		
		UINT n = m_ListFormular.GetItemState(id, (UINT)-1);
		m_ListFormular.SetItemState(id, n | LVIS_SELECTED | LVIS_FOCUSED, (UINT)-1);
		m_ListFormular.EnsureVisible(id, FALSE);	
		m_ListFormular.SetFocus();
	}
	else if ( 2 == m_RadioType)
	{
		int id = m_ListArbitrage.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);	
		if ( -1 == id )
		{
			MessageBox(L"请选择商品", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
			return;
		}

		//
		CArbitrage* pArbitrage = (CArbitrage*)m_ListArbitrage.GetItemData(id);
		if ( NULL == pArbitrage )
		{
			return;
		}

		//
		m_pAlarmCenter->DelArbitrageAlarms(pArbitrage);
		
		//
		FillListArbitrage();

		//
		if ( id >= m_ListArbitrage.GetItemCount() )
		{
			id = m_ListArbitrage.GetItemCount() - 1;
		}

		UINT n = m_ListArbitrage.GetItemState(id, (UINT)-1);
		m_ListArbitrage.SetItemState(id, n | LVIS_SELECTED | LVIS_FOCUSED, (UINT)-1);
		m_ListArbitrage.EnsureVisible(id, FALSE);	
		m_ListArbitrage.SetFocus();
	}

	//
	UpdateData(FALSE);

	//
	m_pAlarmCenter->ToXml();
}

void CDlgAlarmSetting::OnButtonModify() 
{
	UpdateData(TRUE);
	
	if ( NULL == m_pAlarmCenter )
	{
		return;
	}
	
	//
	if ( 0 == m_RadioType )
	{
		int id = m_ListMerch.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);	
		if ( -1 == id )
		{
			MessageBox(L"请选择商品", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
			return;
		}
		
		mapMerchAlarms mapMerchs;
		m_pAlarmCenter->GetAlarmMap(mapMerchs);
		int32 iSize = mapMerchs.size();

		//
		if ( id < iSize )
		{
			// 商品
			CMerch* pMerch = (CMerch*)m_ListMerch.GetItemData(id);
			if ( NULL == pMerch )
			{
				return;
			}
			
			// 找到这个商品的预警信息, 初始化对话框
			CAlarmConditions stAlarmConditions;
			if (!m_pAlarmCenter->GetMerchFixAlarms(pMerch, stAlarmConditions))
			{
				//ASSERT(0);
				return;
			}
			
			//
			CDlgAlarmModify Dlg;
			Dlg.SetAlarmConditions(stAlarmConditions);
			
			if ( IDOK == Dlg.DoModal() )
			{
				Dlg.GetAlarmConditions(stAlarmConditions);
			}
			
			// 修改
			m_pAlarmCenter->ModifyMerchAlarms(pMerch, stAlarmConditions);
		}
		
		//
		FillListMerch();
		//
		UINT n = m_ListMerch.GetItemState(id, (UINT)-1);
		m_ListMerch.SetItemState(id, n | LVIS_SELECTED | LVIS_FOCUSED, (UINT)-1);
		m_ListMerch.EnsureVisible(id, FALSE);	
		m_ListMerch.SetFocus();		
	}
	else if ( 1 == m_RadioType )
	{
		int id = m_ListFormular.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);	
		if ( -1 == id )
		{
			MessageBox(L"请选择公式", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
			return;
		}
		
		//
		CFormularContent* pContent = (CFormularContent*)m_ListFormular.GetItemData(id);
		if ( NULL == pContent )
		{
			return;
		}
		
		//
		CDlgAlarmFormular Dlg;
		Dlg.SetInitialFormular(pContent);

		if ( IDOK == Dlg.DoModal() )
		{
			CFormularContent* pResult = Dlg.GetFormular();

			// 修改
			if ( NULL != pResult )
			{
				m_pAlarmCenter->ModifyFormularAlarms(pResult);
			}			
		}
		
		//
		FillListFormular();

		//
		UINT n = m_ListFormular.GetItemState(id, (UINT)-1);
		m_ListFormular.SetItemState(id, n | LVIS_SELECTED | LVIS_FOCUSED, (UINT)-1);
		m_ListFormular.EnsureVisible(id, FALSE);	
		m_ListFormular.SetFocus();
	}
	else if ( 2 == m_RadioType)
	{
		int id = m_ListArbitrage.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);	
		if ( -1 == id )
		{
			MessageBox(L"请选择商品", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
			return;
		}

		
		// 套利
		CArbitrage* pArbitrage = (CArbitrage*)m_ListArbitrage.GetItemData(id);
		if ( NULL == pArbitrage )
		{
			return;
		}

		// 找到这个套利的预警信息, 初始化对话框
		CAlarmConditions stAlarmConditions;
		if (!m_pAlarmCenter->GetArbitrageFixAlarms(pArbitrage, stAlarmConditions))
		{
			//ASSERT(0);
			return;
		}

		//
		CDlgAlarmModify Dlg(NULL, EAT_ARBITRAGE);
		Dlg.SetAlarmConditions(stAlarmConditions);

		if ( IDOK == Dlg.DoModal() )
		{
			Dlg.GetAlarmConditions(stAlarmConditions);
		}

		// 修改
		m_pAlarmCenter->ModifyArbitrageAlarms(pArbitrage, stAlarmConditions);
		

		//
		FillListArbitrage();
		//
		UINT n = m_ListArbitrage.GetItemState(id, (UINT)-1);
		m_ListArbitrage.SetItemState(id, n | LVIS_SELECTED | LVIS_FOCUSED, (UINT)-1);
		m_ListArbitrage.EnsureVisible(id, FALSE);	
		m_ListArbitrage.SetFocus();	
	}

	UpdateData(FALSE);

	//
	m_pAlarmCenter->ToXml();
}

void CDlgAlarmSetting::OnButtonRemoveall() 
{
	if ( IDYES != MessageBox(L"清除所有项目?", AfxGetApp()->m_pszAppName, MB_YESNO) )
	{
		return;
	}

	if ( NULL == m_pAlarmCenter )
	{
		return;
	}

	//
	UpdateData(TRUE);

	//
	if ( 0 == m_RadioType )
	{
		m_pAlarmCenter->DelAllMerchAlarms();
		FillListMerch();
	}
	else if ( 1 == m_RadioType )
	{
		m_pAlarmCenter->DelAllFormularAlarms();
		FillListFormular();
	}
	else if ( 2 == m_RadioType)
	{
 		m_pAlarmCenter->DelAllArbitrageAlarms();
		FillListArbitrage();
	}

	//
	m_pAlarmCenter->ToXml();
}

void CDlgAlarmSetting::OnRadio1() 
{
	ShowCtrls(0);
}

void CDlgAlarmSetting::OnRadio2() 
{
	ShowCtrls(1);	
}

void CDlgAlarmSetting::OnRadio3() 
{
	ShowCtrls(3);	
}

void CDlgAlarmSetting::OnRadio9() 
{
	ShowCtrls(2);	
}

void CDlgAlarmSetting::OnDblclkListMerch(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnButtonModify();
	*pResult = 0;
}

void CDlgAlarmSetting::OnDblclkListFormular(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnButtonModify();	
	*pResult = 0;
}

void CDlgAlarmSetting::OnChangeRadioPrompt()
{
	if ( NULL != m_pAlarmCenter )
	{
		UpdateData(TRUE);
		CAlarmCenter::E_AlarmType eType = Radio2AlarmType(m_RadioPrompt);
		m_pAlarmCenter->SetAlarmType(eType);
	}
}

void CDlgAlarmSetting::OnRadio4() 
{
	OnChangeRadioPrompt();	
}

void CDlgAlarmSetting::OnRadio5() 
{
	OnChangeRadioPrompt();	
}

void CDlgAlarmSetting::OnRadio6() 
{
	if ( NULL == m_pAlarmCenter )
	{
		return;
	}

	//
	CFileDialog dlg(TRUE
		, NULL
		, NULL
		, OFN_ENABLESIZING|OFN_EXPLORER|OFN_NOCHANGEDIR|OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY
		, _T("声音文件 (*.wav)|*.wav|所有文件 (*.*)|*.*||"));
	if ( dlg.DoModal() == IDOK )
	{
		CString StrPath = dlg.GetPathName();
		
		//
		CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
		CString StrLocalPath = CPathFactory::GetCustomAlarmPath(pDoc->m_pAbsCenterManager->GetUserName());
		if (StrLocalPath.IsEmpty() )
		{
			return;
		}
		

		//
		::CopyFile(StrPath, StrLocalPath, FALSE);		
	}

	//
	OnChangeRadioPrompt();
}

void CDlgAlarmSetting::OnRadio7() 
{	
	if ( NULL != m_pAlarmCenter )
	{
		UpdateData(TRUE);
		m_pAlarmCenter->SetAlarmDlgType(m_bCheck7);
	}
}

void CDlgAlarmSetting::OnRadio8() 
{
	if ( NULL != m_pAlarmCenter )
	{
		UpdateData(TRUE);
		m_pAlarmCenter->SetAlarmStatusType(m_bCheck8);
	}
}

void CDlgAlarmSetting::OnMenu(UINT uID)
{
	if ( s_KiAddArbitrage == uID )
	{
		//
		CArray<CArbitrage, CArbitrage&> aArbitrages;
		CArbitrageManage::Instance()->GetArbitrageDatas(aArbitrages);
		if ( aArbitrages.GetSize() <= 0 )
		{
			MessageBox(L"请先设置套利商品!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
			return;
		}

		// 增加套利预警
		CDlgArbitrageSel Dlg;
		CArbitrage* pstSel = NULL;
		if ( IDOK == Dlg.DoModal() )
		{
			pstSel = Dlg.GetSelArbitrage();
		}
		else
		{
			return;
		}

		//
		CDlgAlarmModify DlgAlarm(this);
		
		if ( IDOK == DlgAlarm.DoModal() )
		{
			CAlarmConditions stAlarmConditions;
			DlgAlarm.GetAlarmConditions(stAlarmConditions);
			
			//
			if (m_pAlarmCenter != NULL)
			{
				m_pAlarmCenter->AddArbitrageAlarms(pstSel, stAlarmConditions);
				FillListMerch();
			}
		}	
	}
	else if ( s_KiAddMerch == uID )
	{
		// 增加普通商品预警
		CBlockConfig::MerchArray aMerchs;
		if ( CDlgChooseStockVar::ChooseStockVar(aMerchs, false) )
		{
			if ( aMerchs.GetSize() <= 0 )
			{
				return;
			}
		}
		else
		{
			return;
		}
		
		//
		CDlgAlarmModify Dlg(this);
		
		if ( IDOK == Dlg.DoModal() )
		{
			CAlarmConditions stAlarmConditions;
			Dlg.GetAlarmConditions(stAlarmConditions);
			
			//
			if (m_pAlarmCenter != NULL)
			{
				m_pAlarmCenter->AddMerchAlarms(aMerchs[0], stAlarmConditions);
				FillListMerch();
				m_pAlarmCenter->ToXml();
			}
		}	
	}
}

void CDlgAlarmSetting::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetClientRect(rect);
    dc.FillSolidRect(rect, RGB(220, 220, 220));
}
