#include "StdAfx.h"

#include "DlgMarketRadarSetting.h"
#include "ShareFun.h"
#include <mmsystem.h>
#include "DlgChooseStockVar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDlgMarketRadarOption dialog

CDlgMarketRadarOption::IdMap CDlgMarketRadarOption::s_mapConditionIdBindCtrls;
CDlgMarketRadarOption::IdMap CDlgMarketRadarOption::s_mapConditionIds;
CDlgMarketRadarOption::IdMap CDlgMarketRadarOption::s_mapVoiceIds;
CDlgMarketRadarOption::IdMap CDlgMarketRadarOption::s_mapVoiceIdBindCtrls;

CDlgMarketRadarOption::CDlgMarketRadarOption(CWnd* pParent /*=NULL*/)
: CDialogEx(CDlgMarketRadarOption::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMarketRadarOption)
	//}}AFX_DATA_INIT

	m_bZoomData = FALSE;

	if ( s_mapConditionIds.empty() )
	{
		// 条件
		s_mapConditionIds.insert(IdMap::value_type(IDC_CHECK1, EMRC_TodayNewHigh));
		s_mapConditionIds.insert(IdMap::value_type(IDC_CHECK2, EMRC_TodayNewLow));
		s_mapConditionIds.insert(IdMap::value_type(IDC_CHECK3, EMRC_BigBuy));
		s_mapConditionIds.insert(IdMap::value_type(IDC_CHECK7, EMRC_BigSell));
		s_mapConditionIds.insert(IdMap::value_type(IDC_CHECK8, EMRC_LowOpenRise));
		s_mapConditionIds.insert(IdMap::value_type(IDC_CHECK9, EMRC_HighOpenFall));
		s_mapConditionIds.insert(IdMap::value_type(IDC_CHECK10, EMRC_RiseOver));
		s_mapConditionIds.insert(IdMap::value_type(IDC_CHECK11, EMRC_FallBelow));
		s_mapConditionIds.insert(IdMap::value_type(IDC_CHECK12, EMRC_SingleTradeRiseFall));
		s_mapConditionIds.insert(IdMap::value_type(IDC_CHECK13, EMRC_HugeAmount));
		s_mapConditionIds.insert(IdMap::value_type(IDC_CHECK14, EMRC_BigVol));
		s_mapConditionIds.insert(IdMap::value_type(IDC_CHECK15, EMRC_RiseFallMax));
		s_mapConditionIds.insert(IdMap::value_type(IDC_CHECK16, EMRC_BigVolInTime));

		// 条件相关子控件
		s_mapConditionIdBindCtrls.insert(IdMap::value_type(IDC_EDIT1, IDC_CHECK10));
		s_mapConditionIdBindCtrls.insert(IdMap::value_type(IDC_EDIT2, IDC_CHECK11));
		s_mapConditionIdBindCtrls.insert(IdMap::value_type(IDC_EDIT3, IDC_CHECK12));
		s_mapConditionIdBindCtrls.insert(IdMap::value_type(IDC_EDIT4, IDC_CHECK13));
		s_mapConditionIdBindCtrls.insert(IdMap::value_type(IDC_EDIT5, IDC_CHECK16));
		s_mapConditionIdBindCtrls.insert(IdMap::value_type(IDC_EDIT6, IDC_CHECK16));

		// 声音
		s_mapVoiceIds.insert(IdMap::value_type(IDC_RADIO1, EMRAVT_None));
		s_mapVoiceIds.insert(IdMap::value_type(IDC_RADIO2, EMRAVT_System));
		s_mapVoiceIds.insert(IdMap::value_type(IDC_RADIO3, EMRAVT_User));

		// 声音相关控件
		s_mapVoiceIdBindCtrls.insert(IdMap::value_type(IDC_BUTTON1, IDC_RADIO3));
		s_mapVoiceIdBindCtrls.insert(IdMap::value_type(IDC_BUTTON5, IDC_RADIO3));
		s_mapVoiceIdBindCtrls.insert(IdMap::value_type(IDC_EDIT8, IDC_RADIO3));
	}
}


void CDlgMarketRadarOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	if ( !pDX->m_bSaveAndValidate
		&& !m_bZoomData )
	{
		// 提交到控件，改写几个字段为%
		m_SubParam.m_fRiseOverValue *= 100;
		m_SubParam.m_fFallBelowValue *= 100;
		m_SubParam.m_fSingleTradeRiseFallValue *= 100;
		m_SubParam.m_fWhatIsBigVol *= 100;

		m_SubParam.m_fHugeAmountValue /= 10000;

		if ( m_SubParam.m_bShowDlgListAlarm )
		{
			m_SubParam.m_bShowDlgListAlarm = 1;
		}

		m_bZoomData = true;
	}
	//{{AFX_DATA_MAP(CDlgMarketRadarOption)
	DDX_Text(pDX, IDC_EDIT1, m_SubParam.m_fRiseOverValue);
	DDX_Text(pDX, IDC_EDIT2, m_SubParam.m_fFallBelowValue);
	DDX_Text(pDX, IDC_EDIT3, m_SubParam.m_fSingleTradeRiseFallValue);
	DDX_Text(pDX, IDC_EDIT4, m_SubParam.m_fHugeAmountValue);
	DDX_Text(pDX, IDC_EDIT5, m_SubParam.m_iBigVolTimeRange);
	DDX_Text(pDX, IDC_EDIT6, m_SubParam.m_iBigVolOverCount);
	DDX_Text(pDX, IDC_EDIT7, m_SubParam.m_fWhatIsBigVol);
	DDX_Text(pDX, IDC_EDIT8, m_SubParam.m_StrVoiceFilePath);
	DDX_Check(pDX, IDC_CHECK17, *reinterpret_cast<int *>(&m_SubParam.m_bShowDlgListAlarm));
	//}}AFX_DATA_MAP
	if ( pDX->m_bSaveAndValidate
		/*&& m_bZoomData*/ )
	{
		// 保存到数据，改写几个字段为实际数据
		m_SubParam.m_fRiseOverValue /= 100;
		m_SubParam.m_fFallBelowValue /= 100;
		m_SubParam.m_fSingleTradeRiseFallValue /= 100;
		m_SubParam.m_fWhatIsBigVol /= 100;

		m_SubParam.m_fHugeAmountValue *= 10000;

		m_bZoomData = false;
	}
}


BEGIN_MESSAGE_MAP(CDlgMarketRadarOption, CDialogEx)
//{{AFX_MSG_MAP(CDlgMarketRadarOption)
ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK1, IDC_CHECK16, OnBtnConditionClick)
ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO1, IDC_RADIO3, OnRadioVoiceClick)
ON_BN_CLICKED(IDC_BUTTON1, OnBtnFile)
ON_BN_CLICKED(IDC_BUTTON5, OnBtnTestVoice)
ON_BN_CLICKED(IDC_BUTTON2, OnResetToDefault)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDlgMarketRadarOption::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设定初始值
	// subParam已经设置好了
	UpdateSubParam(FALSE);	// 更新控件状态
	
	return TRUE;
}

void CDlgMarketRadarOption::PromptErrorInput( const CString &StrPrompt, CWnd *pWndFocus /*= NULL*/ )
{
	if ( !StrPrompt.IsEmpty() )
	{
		AfxMessageBox(StrPrompt, MB_ICONWARNING |MB_OK);
	}
	if ( NULL != pWndFocus )
	{
		pWndFocus->SetFocus();
		if ( pWndFocus->IsKindOf(RUNTIME_CLASS(CEdit)) )
		{
			((CEdit *)pWndFocus)->SetSel(0, -1);
		}
		else if ( pWndFocus->IsKindOf(RUNTIME_CLASS(CRichEditCtrl)) )
		{
			((CRichEditCtrl *)pWndFocus)->SetSel(0, -1);
		}
	}
}

BOOL CDlgMarketRadarOption::UpdateSubParam( BOOL bUpdate )
{
	// 更新实际数据
	BOOL bRet = !UpdateData(bUpdate);

	// 更新控件状态
	if ( bUpdate )
	{
		// 保存到数据
		// 条件
		IdMap::iterator it;
		for ( it=s_mapConditionIds.begin(); it != s_mapConditionIds.end() ; ++it )
		{
			try
			{
				BOOL bCheck = static_cast<CButton *>(GetDlgItem(it->first))->GetCheck() == BST_CHECKED;
				if ( bCheck )
				{
					m_SubParam.m_dwConditions |= it->second;
				}
				else
				{
					m_SubParam.m_dwConditions &= (~(it->second));
				}
			}
			catch (...)
			{
			}
		}

		// 声音
		for ( it=s_mapVoiceIds.begin(); it != s_mapVoiceIds.end() ; ++it )
		{
			try
			{
				BOOL bCheck = static_cast<CButton *>(GetDlgItem(it->first))->GetCheck() == BST_CHECKED;
				if ( bCheck )
				{
					m_SubParam.m_eVoiceType = (E_MarketRadarAlarmVoiceType)it->second;
				}
			}
			catch (...)
			{
			}
		}
	}
	else
	{
		IdMap::iterator it;
		// 保存到控件
		for ( it=s_mapConditionIds.begin(); it != s_mapConditionIds.end() ; ++it )
		{
			try
			{
				bool32 bCheck = CheckFlag(m_SubParam.m_dwConditions, it->second);
				static_cast<CButton *>(GetDlgItem(it->first))->SetCheck(bCheck ? BST_CHECKED: BST_UNCHECKED);
			}
			catch (...)
			{
			}
		}

		// 声音
		for ( it=s_mapVoiceIds.begin(); it != s_mapVoiceIds.end() ; ++it )
		{
			try
			{
				BOOL bCheck = it->second == m_SubParam.m_eVoiceType;
				static_cast<CButton *>(GetDlgItem(it->first))->SetCheck(bCheck ? BST_CHECKED: BST_UNCHECKED);
			}
			catch (...)
			{
			}
		}
	}

	UpdateCtrlStatus();	// 更新控件使能状态

	return bRet;
}

void CDlgMarketRadarOption::UpdateCtrlStatus()
{
	IdMap::iterator it;
	// 条件
	for (  it=s_mapConditionIdBindCtrls.begin(); it != s_mapConditionIdBindCtrls.end() ; ++it )
	{
		try
		{
			BOOL bCheck = static_cast<CButton *>(GetDlgItem(it->second))->GetCheck() == BST_CHECKED;
			GetDlgItem(it->first)->EnableWindow(bCheck);
		}
		catch (...)
		{
		}
	}

	// 声音
	for ( it=s_mapVoiceIdBindCtrls.begin(); it != s_mapVoiceIdBindCtrls.end() ; ++it )
	{
		try
		{
			BOOL bCheck = static_cast<CButton *>(GetDlgItem(it->second))->GetCheck() == BST_CHECKED;
			GetDlgItem(it->first)->EnableWindow(bCheck);
		}
		catch (...)
		{
		}
	}
}

void CDlgMarketRadarOption::OnBtnConditionClick( UINT nId )
{
	// 条件
	IdMap::iterator it = s_mapConditionIds.find(nId);
	if ( it != s_mapConditionIds.end() )
	{
		UpdateSubParam(TRUE);	// 更新值
	}
}

void CDlgMarketRadarOption::OnRadioVoiceClick( UINT nId )
{
	// 声音
	IdMap::iterator it = s_mapVoiceIds.find(nId);
	if ( it != s_mapVoiceIds.end() )
	{
		UpdateSubParam(TRUE);
	}
}

void CDlgMarketRadarOption::OnBtnFile()
{
	CFileDialog dlg(TRUE
		, NULL
		, NULL
		, OFN_ENABLESIZING|OFN_EXPLORER|OFN_NOCHANGEDIR|OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY
		, _T("声音文件 (*.wav)|*.wav|所有文件 (*.*)|*.*||"));
	if ( dlg.DoModal() == IDOK )
	{
		m_SubParam.m_StrVoiceFilePath = dlg.GetPathName();			// 手动更新，节约Update?
		SetDlgItemText(IDC_EDIT8, m_SubParam.m_StrVoiceFilePath);	
	}
}

void CDlgMarketRadarOption::OnBtnTestVoice()
{
	if ( m_SubParam.m_StrVoiceFilePath.IsEmpty() )
	{
		PromptErrorInput(_T("请选择声音文件！"), GetDlgItem(IDC_EDIT8));
		return;
	}

	PlaySound(m_SubParam.m_StrVoiceFilePath, NULL, SND_FILENAME |SND_ASYNC |SND_NOSTOP);
}

void CDlgMarketRadarOption::OnResetToDefault()
{
	E_MarketRadarAlarmVoiceType eVoice = m_SubParam.m_eVoiceType;
	CString	StrVoice = m_SubParam.m_StrVoiceFilePath;
	SetRadarSubParam(T_MarketRadarSubParameter());
	m_SubParam.m_eVoiceType = eVoice;
	m_SubParam.m_StrVoiceFilePath = StrVoice;
	UpdateSubParam(FALSE);
}

CDlgMarketRadarOption::~CDlgMarketRadarOption()
{
	
}

void CDlgMarketRadarOption::SetRadarSubParam( const T_MarketRadarSubParameter &SubParam )
{
	m_SubParam = SubParam;
	m_bZoomData = FALSE;
}




/////////////////////////////////////////////////////////////////////////////
// CDlgMarketRadarEtc dialog
CDlgMarketRadarEtc::IdMap CDlgMarketRadarEtc::s_mapMerchRangeBindCtrls;
CDlgMarketRadarEtc::IdMap CDlgMarketRadarEtc::s_mapMerchRangeIds;
MerchArray	CDlgMarketRadarEtc::s_aMerchRange;
CDlgMarketRadarEtc::MerchSet CDlgMarketRadarEtc::s_setMerchRange;
CDlgMarketRadarEtc::CDlgMarketRadarEtc(CWnd* pParent /*=NULL*/)
: CDialogEx(CDlgMarketRadarEtc::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMarketRadarEtc)
	//}}AFX_DATA_INIT
	
	if ( s_mapMerchRangeIds.empty() )
	{
		s_mapMerchRangeIds.insert(IdMap::value_type(IDC_RADIO1, EMRMR_HSA));
		s_mapMerchRangeIds.insert(IdMap::value_type(IDC_RADIO2, EMRMR_ZX));
		s_mapMerchRangeIds.insert(IdMap::value_type(IDC_RADIO3, EMRMR_MerchArray));

		s_mapMerchRangeBindCtrls.insert(IdMap::value_type(IDC_LIST1, IDC_RADIO3));
		s_mapMerchRangeBindCtrls.insert(IdMap::value_type(IDC_BUTTON1, IDC_RADIO3));
		s_mapMerchRangeBindCtrls.insert(IdMap::value_type(IDC_BUTTON5, IDC_RADIO3));
		s_mapMerchRangeBindCtrls.insert(IdMap::value_type(IDC_BUTTON6, IDC_RADIO3));
	}

	if ( s_aMerchRange.GetSize() <= 0 )
	{
		// 有可能本来就是空的, 那也不怎么费时~
		CMarketRadarCalc::Instance().ReadMerchArray(s_aMerchRange);	// 尝试读取文件
		for ( int32 i=s_aMerchRange.GetUpperBound(); i >= 0 ; --i )
		{
			MerchSet::_Pairib itb = s_setMerchRange.insert(s_aMerchRange[i]);
			if ( !itb.second )
			{
				s_aMerchRange.RemoveAt(i);
			}
		}
	}
}


void CDlgMarketRadarEtc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	if ( !pDX->m_bSaveAndValidate )
	{
		// 上传到控件
		for ( IdMap::iterator it=s_mapMerchRangeIds.begin(); it != s_mapMerchRangeIds.end() ; ++it )
		{
			BOOL bCheck = it->second == m_Param.m_eMerchRange;
			try
			{
				((CButton *)GetDlgItem(it->first))->SetCheck(bCheck ? BST_CHECKED : BST_UNCHECKED);
			}
			catch(...)
			{
			}
		}
	}

	//{{AFX_DATA_MAP(CDlgMarketRadarEtc)
	DDX_Control(pDX, IDC_LIST1, m_List);
	//}}AFX_DATA_MAP

	if ( pDX->m_bSaveAndValidate )
	{
		// 下载到数据
		// 使能控件
		for ( IdMap::iterator it=s_mapMerchRangeIds.begin(); it != s_mapMerchRangeIds.end() ; ++it )
		{
			try
			{
				BOOL bCheck = ((CButton *)GetDlgItem(it->first))->GetCheck() == BST_CHECKED;
				if ( bCheck )
				{
					m_Param.m_eMerchRange = (E_MarketRadarMerchRange)it->second;
				}
			}
			catch(...)
			{
			}
		}

		// 如果是自选商品，则拷贝商品数组到参数中
		if ( EMRMR_MerchArray == m_Param.m_eMerchRange )
		{
			m_Param.m_aMerchs.Copy(s_aMerchRange);
		}
	}

	// 使能控件
	for ( IdMap::iterator it=s_mapMerchRangeBindCtrls.begin(); it != s_mapMerchRangeBindCtrls.end() ; ++it )
	{
		try
		{
			BOOL bCheck = ((CButton *)GetDlgItem(it->second))->GetCheck() == BST_CHECKED;
			GetDlgItem(it->first)->EnableWindow(bCheck);
		}
		catch(...)
		{
		}
	}
}


BEGIN_MESSAGE_MAP(CDlgMarketRadarEtc, CDialogEx)
//{{AFX_MSG_MAP(CDlgMarketRadarEtc)
ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO1, IDC_RADIO3, OnRadioClick)
ON_BN_CLICKED(IDC_BUTTON1, OnBtnAdd)
ON_BN_CLICKED(IDC_BUTTON5, OnBtnRemove)
ON_BN_CLICKED(IDC_BUTTON6, OnBtnClear)
ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST1, OnGetDispInfo)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDlgMarketRadarEtc::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_ImageList.Create ( IDB_FORMULA,16,1,RGB(255,255,255) );
	m_List.SetImageList(&m_ImageList, LVSIL_SMALL);

	// 设定初始值
	// Param已经设置好了
	UpdateData(FALSE);

	FillList();
	
	return TRUE;
}

void CDlgMarketRadarEtc::PromptErrorInput( const CString &StrPrompt, CWnd *pWndFocus /*= NULL*/ )
{
	if ( !StrPrompt.IsEmpty() )
	{
		AfxMessageBox(StrPrompt, MB_ICONWARNING |MB_OK);
	}
	if ( NULL != pWndFocus )
	{
		pWndFocus->SetFocus();
		if ( pWndFocus->IsKindOf(RUNTIME_CLASS(CEdit)) )
		{
			((CEdit *)pWndFocus)->SetSel(0, -1);
		}
		else if ( pWndFocus->IsKindOf(RUNTIME_CLASS(CRichEditCtrl)) )
		{
			((CRichEditCtrl *)pWndFocus)->SetSel(0, -1);
		}
	}
}

CDlgMarketRadarEtc::~CDlgMarketRadarEtc()
{
	
}

void CDlgMarketRadarEtc::OnRadioClick( UINT nId )
{
	IdMap::iterator it = s_mapMerchRangeIds.find(nId);
	if ( it != s_mapMerchRangeIds.end() )
	{
		UpdateData(TRUE);
	}
}

void CDlgMarketRadarEtc::OnBtnAdd()
{
	MerchArray aMerchs;
	if ( CDlgChooseStockVar::ChooseStockVar(aMerchs, true, NULL, this) )
	{
		int32 iAdded = 0;
		for ( int32 i=0; i < aMerchs.GetSize() ; i++ )
		{
			MerchSet::_Pairib itb = s_setMerchRange.insert(aMerchs[i]);
			if ( itb.second )
			{
				s_aMerchRange.Add(aMerchs[i]);
				iAdded++;
			}
		}
		if ( iAdded > 0 )
		{
			FillList();
		}
	}
}

void CDlgMarketRadarEtc::OnBtnRemove()
{
	POSITION pos = m_List.GetFirstSelectedItemPosition();
	int32 iRemove = 0;
	while ( NULL != pos )
	{
		int32 iIndex = m_List.GetNextSelectedItem(pos);
		if ( iIndex >=0 && iIndex < s_aMerchRange.GetSize() )
		{
			CMerch *pMerch = s_aMerchRange[iIndex];
			s_aMerchRange.RemoveAt(iIndex);
			s_setMerchRange.erase(pMerch);
			iRemove++;
		}
	}
	if ( iRemove > 0 )
	{
		FillList();
	}
}

void CDlgMarketRadarEtc::OnBtnClear()
{
	// 清除自设列表, 是否设置标志让下次对话框不读取？
	s_aMerchRange.RemoveAll();
	s_setMerchRange.clear();
	CMarketRadarCalc::Instance().SaveMerchArray(s_aMerchRange);
	FillList();
}

void CDlgMarketRadarEtc::FillList()
{
	m_List.SetItemCount(s_aMerchRange.GetSize());
	m_List.Invalidate();
}

void CDlgMarketRadarEtc::OnGetDispInfo( NMHDR *pHdr, LRESULT *pResult )
{
	#define DLGCSV_MERCH_BMP (4)
	NMLVDISPINFO *pdi = (NMLVDISPINFO*)pHdr;
	if ( NULL != pdi )
	{
		const int iIndex = pdi->item.iItem;
		if ( iIndex >= 0 && iIndex < s_aMerchRange.GetSize() )
		{
			pdi->item.iImage = DLGCSV_MERCH_BMP;
			pdi->item.lParam = (LPARAM)s_aMerchRange[iIndex];
			
			if ( (pdi->item.mask &LVIF_TEXT) == LVIF_TEXT
				&& NULL != pdi->item.pszText 
				&& pdi->item.cchTextMax > 0 )
			{
				_tcsncpy(pdi->item.pszText, s_aMerchRange[iIndex]->m_MerchInfo.m_StrMerchCnName, pdi->item.cchTextMax-1);
				pdi->item.pszText[pdi->item.cchTextMax-1] = _T('\0');
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CDlgMarketRadarSetting dialog

CDlgMarketRadarSetting::CDlgMarketRadarSetting(CWnd* pParent /*=NULL*/)
: CDialogEx(CDlgMarketRadarSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMarketRadarSetting)
	//}}AFX_DATA_INIT
	
}


void CDlgMarketRadarSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	//{{AFX_DATA_MAP(CDlgMarketRadarSetting)
	DDX_Control(pDX, IDC_TAB1, m_Tab);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMarketRadarSetting, CDialogEx)
//{{AFX_MSG_MAP(CDlgMarketRadarSetting)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDlgMarketRadarSetting::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	// 设定初始值
	// Param已经设置好了
	
	// 创建两个子对话框

	m_dlgOption.Create(CDlgMarketRadarOption::IDD, this);
	m_dlgEtc.Create(CDlgMarketRadarEtc::IDD, this);

	m_Tab.InsertItem(TCIF_PARAM |TCIF_TEXT, 0, _T("雷达选项"), 0, (LPARAM)&m_dlgOption);
	m_Tab.InsertItem(TCIF_PARAM |TCIF_TEXT, 1, _T("其它选项"), 0, (LPARAM)&m_dlgEtc);

	m_Tab.SetCurSel(0);

	RecalcLayout();
	OnTabChanged();
	
	return TRUE;
}

void CDlgMarketRadarSetting::PromptErrorInput( const CString &StrPrompt, CWnd *pWndFocus /*= NULL*/ )
{
	if ( !StrPrompt.IsEmpty() )
	{
		AfxMessageBox(StrPrompt, MB_ICONWARNING |MB_OK);
	}
	if ( NULL != pWndFocus )
	{
		pWndFocus->SetFocus();
		if ( pWndFocus->IsKindOf(RUNTIME_CLASS(CEdit)) )
		{
			((CEdit *)pWndFocus)->SetSel(0, -1);
		}
		else if ( pWndFocus->IsKindOf(RUNTIME_CLASS(CRichEditCtrl)) )
		{
			((CRichEditCtrl *)pWndFocus)->SetSel(0, -1);
		}
	}
}

void CDlgMarketRadarSetting::SetRadarParam( const T_MarketRadarParameter &Param )
{
	m_Param = Param;

	m_dlgOption.SetRadarSubParam(m_Param.m_ParamSub);
	m_dlgEtc.SetRadarParam(m_Param);
}

void CDlgMarketRadarSetting::RecalcLayout()
{
	CRect rcTab;
	m_Tab.GetClientRect(rcTab);
	CRect rcItem;
	m_Tab.GetItemRect(0, rcItem);
	m_Tab.RecalcLayout(CRect(-1,0,-3,-3));

	/*
	rcTab.top = rcItem.bottom;

	m_Tab.ClientToScreen(&rcTab);
	ScreenToClient(&rcTab);
	m_dlgOption.MoveWindow(rcTab);
	m_dlgEtc.MoveWindow(rcTab);
	*/
}

void CDlgMarketRadarSetting::OnTabChanged()
{
	int iCurSel = m_Tab.GetCurSel();
	if ( iCurSel == 0 )
	{
		m_dlgOption.ShowWindow(SW_SHOW);
		m_dlgEtc.ShowWindow(SW_HIDE);
	}
	else
	{
		m_dlgOption.ShowWindow(SW_HIDE);
		m_dlgEtc.ShowWindow(SW_SHOW);
	}
}

CDlgMarketRadarSetting::~CDlgMarketRadarSetting()
{
	
}

void CDlgMarketRadarSetting::OnOK()
{
	if ( !m_dlgOption.UpdateData(TRUE)
		|| !m_dlgEtc.UpdateData(TRUE) )
	{
		return;	// 有无效数据
	}

	m_Param.m_ParamSub = m_dlgOption.GetRadarSubParam();
	m_Param.m_eMerchRange = m_dlgEtc.GetRadarParam().m_eMerchRange;
	m_Param.m_aMerchs.Copy(m_dlgEtc.GetRadarParam().m_aMerchs);

	CDialogEx::OnOK();
}
