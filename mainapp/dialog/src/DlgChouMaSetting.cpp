#include "StdAfx.h"

#include "DlgChouMaSetting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgChouMaSetting dialog

CDlgChouMaSetting::CDlgChouMaSetting(CWnd* pParent /*=NULL*/)
: CDialogEx(CDlgChouMaSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgChouMaSetting)
	//}}AFX_DATA_INIT
}


void CDlgChouMaSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgChouMaSetting)
	DDX_Control(pDX, IDC_EDIT_CYCLE, m_EditCycle);
	DDX_Control(pDX, IDC_EDIT_ATTENUATION, m_EditAttenuation);
	DDX_Control(pDX, IDC_EDIT_DEFAULTTRADERATE, m_EditDefaultTradeRate);
	DDX_Control(pDX, IDC_TAB1, m_Tab);
	DDX_Control(pDX, IDC_LIST1, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgChouMaSetting, CDialogEx)
//{{AFX_MSG_MAP(CDlgChouMaSetting)
ON_BN_CLICKED(IDC_BUTTON_ADD, OnBtnAdd)
ON_BN_CLICKED(IDC_BUTTON_DEL, OnBtnDel)
ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnTabChange)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDlgChouMaSetting::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设定初始值
	CIoViewChouMa::GetCBFBParam(m_param);

	if ( m_param.m_eChengBenFenBuSuanFa == ECBFB_PingJun )
	{
		CheckDlgButton(IDC_RADIO_PINGJUN, BST_CHECKED);
	}
	else
	{
		CheckDlgButton(IDC_RADIO_SANJIAO, BST_CHECKED);
	}

	CString StrData;
	StrData.Format(_T("%f"), m_param.m_fTradeRateRatio);
	SetDlgItemText(IDC_EDIT_ATTENUATION, StrData);

	StrData.Format(_T("%f"), m_param.m_fDefaultTradeRate*100);
	SetDlgItemText(IDC_EDIT_DEFAULTTRADERATE, StrData);

	SetDlgItemText(IDC_EDIT_CYCLE, _T("40"));

	m_Tab.InsertItem(0, _T("远期成本分布"));
	m_Tab.InsertItem(1, _T("近期成本分布"));
	m_Tab.SetCurSel(0);

	FillCurList();
	
	return TRUE;
}

void CDlgChouMaSetting::OnOK()
{
	// 检查一下值
	if ( !CheckAttenuation(m_param.m_fTradeRateRatio) )
	{
		return;
	}

	if ( !CheckDefaultTradeRate(m_param.m_fDefaultTradeRate) )
	{
		return;
	}

	int iSuanFa = GetCheckedRadioButton(IDC_RADIO_PINGJUN, IDC_RADIO_SANJIAO);
	if ( iSuanFa == IDC_RADIO_PINGJUN )
	{
		m_param.m_eChengBenFenBuSuanFa = ECBFB_PingJun;
	}
	else
	{
		m_param.m_eChengBenFenBuSuanFa = ECBFB_SanJiao;
	}
	
	CDialogEx::OnOK();
}

void CDlgChouMaSetting::PromptErrorInput( const CString &StrPrompt, CWnd *pWndFocus /*= NULL*/ )
{
	AfxMessageBox(StrPrompt, MB_ICONWARNING |MB_OK);
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

void CDlgChouMaSetting::FillCurList()
{
	m_List.ResetContent();

	CString StrFmt;
	ChengBenFenBuCycleArray &aCycles = GetCurCycles(&StrFmt);
	for (ChengBenFenBuCycleArray::iterator it=aCycles.begin(); it != aCycles.end() ; it++ )
	{
		if ( *it == 0 )
		{
			continue;	// 这个值不能显示
		}

		CString Str;
		Str.Format(StrFmt, *it);
		int iLine = m_List.AddString(Str);
		m_List.SetItemData(iLine, *it);
	}
}

ChengBenFenBuCycleArray & CDlgChouMaSetting::GetCurCycles( OUT CString *pStrFmt /*= NULL*/ )
{
	if ( m_Tab.GetCurSel() == 1 )
	{
		if ( NULL != pStrFmt )
		{
			*pStrFmt = _T("%d日内成本");
		}
		return m_param.m_aCBFBBlueGCycles;
	}

	if ( NULL != pStrFmt )
	{
		*pStrFmt = _T("%d日前成本");
	}
	return m_param.m_aCBFBRedYellowCycles;
}

void CDlgChouMaSetting::OnBtnAdd()
{
	int32 iCycle = 0;
	if ( !CheckCycle(iCycle) )
	{
		return;
	}

	ChengBenFenBuCycleArray &aCycles = GetCurCycles();
	bool32 bInsert = false;
	for (ChengBenFenBuCycleArray::iterator it=aCycles.begin(); it != aCycles.end() ; it++ )
	{
		if ( *it > iCycle )
		{
			aCycles.insert(it, iCycle);
			bInsert = true;
			break;
		}
		else if ( *it == iCycle )
		{
			return;	// 已经存在无需后续处理
		}
	}
	if ( !bInsert )
	{
		aCycles.push_back(iCycle);
	}

	FillCurList();
}

void CDlgChouMaSetting::OnBtnDel()
{
	int iCurSel = m_List.GetCurSel();
	if ( iCurSel < 0 )
	{
		return;
	}

	int32 iCycle = (int32)m_List.GetItemData(iCurSel);
	ChengBenFenBuCycleArray &aCycles = GetCurCycles();
	bool32 bDel = false;
	for (ChengBenFenBuCycleArray::iterator it=aCycles.begin(); it != aCycles.end() ; it++ )
	{
		if ( *it == iCycle )
		{
			aCycles.erase(it);
			bDel = true;
			break;
		}
	}
	if ( bDel )
	{
		FillCurList();
	}
}


void CDlgChouMaSetting::OnTabChange( NMHDR *pHdr, LRESULT *pResult )
{
	FillCurList();
}

bool32 CDlgChouMaSetting::CheckCycle( OUT int32 &iCycle, bool32 bPrompt/* = true*/ )
{
	iCycle = GetDlgItemInt(IDC_EDIT_CYCLE);
	if ( iCycle < 1 || iCycle > CHOUMA_MAX_KLINECOUNT )
	{
		if ( bPrompt )
		{
			CString StrPrompt;
			StrPrompt.Format(_T("周期必须填入 %d--%d 之内的值"), 1, CHOUMA_MAX_KLINECOUNT);
			PromptErrorInput(StrPrompt, &m_EditCycle);
		}
		return false;
	}
	return true;
}


bool32 CDlgChouMaSetting::CheckAttenuation( OUT float &fAttenuation, bool32 bPrompt /*= true*/ )
{
	CString StrData;
	m_EditAttenuation.GetWindowText(StrData);
	if (_stscanf(StrData, _T("%f"), &fAttenuation) != 1
		|| fAttenuation < CBFB_MIN_TRADERATERADIO
		|| fAttenuation > CBFB_MAX_TRADERATERADIO )
	{
		if ( bPrompt )
		{
			CString StrPrompt;
			StrPrompt.Format(_T("历史衰减系数必须填入 %0.1f--%0.1f 之内的值"), CBFB_MIN_TRADERATERADIO, CBFB_MAX_TRADERATERADIO);
			PromptErrorInput(StrPrompt, &m_EditAttenuation);
		}
		return false;
	}

	return true;
}

bool32 CDlgChouMaSetting::CheckDefaultTradeRate( OUT float &fDefaultTradeRate, bool32 bPrompt /*= true*/ )
{
	CString StrData;
	m_EditDefaultTradeRate.GetWindowText(StrData);				// 用户输入的是已经%数
	const float fMin = 0.0f;
	const float fMax = 99.9f;
	if (_stscanf(StrData, _T("%f"), &fDefaultTradeRate) != 1
		|| fDefaultTradeRate < fMin
		|| fDefaultTradeRate > fMax )
	{
		if ( bPrompt )
		{
			CString StrPrompt;
			StrPrompt.Format(_T("默认换手率必须填入 %0.1f--%0.1f%% 之内的值"), fMin, fMax);
			PromptErrorInput(StrPrompt, &m_EditAttenuation);
		}
		return false;
	}
	
	fDefaultTradeRate = fDefaultTradeRate/100;	// 除以%
	return true;
}

bool32 CDlgChouMaSetting::ChouMaSetting()
{
	CDlgChouMaSetting dlg(AfxGetMainWnd());
	if ( dlg.DoModal() == IDOK )
	{
		// 检查是否变更 TODO
		CIoViewChouMa::SetCBFBParam(dlg.m_param);
		return true;
	}
	return false;
}
