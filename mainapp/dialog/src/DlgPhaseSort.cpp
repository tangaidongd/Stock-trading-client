#include "StdAfx.h"

#include "DlgPhaseSort.h"
#include "BlockConfig.h"
#include "XmlShare.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPhaseSort dialog

T_PhaseOpenBlockParam CDlgPhaseSort::s_PhaseBlockParam;		// 每次空缺默认的初始化参数

CDlgPhaseSort::CDlgPhaseSort(CWnd* pParent /*=NULL*/)
: CDialogEx(CDlgPhaseSort::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPhaseSort)
	//}}AFX_DATA_INIT
	m_bUseStaticParam = false;
}


void CDlgPhaseSort::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPhaseSort)
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_DTCStart);
	DDX_Control(pDX, IDC_DATETIMEPICKER2, m_DTCEnd);
	DDX_Control(pDX, IDC_CHECK_PREWEIGHT, m_BtnPreWeight);
	DDX_Control(pDX, IDC_COMBO_BLOCKLIST, m_CBBlockList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPhaseSort, CDialogEx)
//{{AFX_MSG_MAP(CDlgPhaseSort)
ON_BN_CLICKED(IDC_RADIO_OFTENBLOCK, OnBtnOftenClicked)
ON_BN_CLICKED(IDC_RADIO_NORMALBLOCK, OnBtnNormalClicked)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDlgPhaseSort::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设定初始值
	// 用户选择的1.1日是指当地时区1.1日，其希望的日K线是指当地时区1.1 0时还是gmt时间n.n 的0时？
	// 由于显示日K线时按照当地时区显示gmt时间，所以可以认为用户选择的1.1就是指对应gmt时间的n.n 的0时日K线
	CTime TStart(m_PhaseBlockParam.m_TimeStart.GetTime());
	CTime TEnd(m_PhaseBlockParam.m_TimeEnd.GetTime());
	m_DTCStart.SetTime(&TStart);
	m_DTCEnd.SetTime(&TEnd);

	// 选择的排行类型
	int32 iSortType =  IDC_RADIO_RISEFALL+ m_PhaseBlockParam.m_ePhaseSortType - EPST_RiseFallPecent;
	CheckDlgButton(iSortType, BST_CHECKED);

	if ( m_PhaseBlockParam.m_bDoPreWeight )
	{
		CheckDlgButton(IDC_CHECK_PREWEIGHT, BST_CHECKED);
	}

	
	// 分为分类板块&用户板块 vs 普通板块
	bool32 bUserSelIsOften = true;

	CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(m_PhaseBlockParam.m_iBlockId);
	if ( NULL != pBlock )
	{
		bUserSelIsOften = pBlock->m_blockCollection.IsMarketClassBlockCollection()
			|| pBlock->m_blockCollection.IsUserBlockCollection();
	}

	CheckDlgButton(bUserSelIsOften ? IDC_RADIO_OFTENBLOCK : IDC_RADIO_NORMALBLOCK, BST_CHECKED);

	FillCBBlockList(bUserSelIsOften);
	
	return TRUE;
}

void CDlgPhaseSort::OnOK()
{
	// 检查一下值
	CTime TStart, TEnd;
	m_DTCStart.GetTime(TStart);
	m_DTCEnd.GetTime(TEnd);
	if ( TEnd < TStart )
	{
		PromptErrorInput(_T("开始时间必须小于等于结束时间！！"), &m_DTCStart);
		return;
	}
	m_PhaseBlockParam.m_TimeStart = CGmtTime(TStart.GetTime());
	m_PhaseBlockParam.m_TimeEnd = CGmtTime(TEnd.GetTime());

	E_PhaseSortType ePST = 
		(E_PhaseSortType)(GetCheckedRadioButton(IDC_RADIO_RISEFALL, IDC_RADIO_RISEFALL+EPST_Count-1)-IDC_RADIO_RISEFALL);
	if ( ePST < EPST_Count )
	{
		m_PhaseBlockParam.m_ePhaseSortType = ePST;
	}
	else
	{
		ASSERT( 0 );
	}

	m_PhaseBlockParam.m_bDoPreWeight = m_BtnPreWeight.GetCheck() == BST_CHECKED;

	int32 iSel = m_CBBlockList.GetCurSel();
	if ( iSel >=0  )
	{
		int32 iBlockId = (int32)m_CBBlockList.GetItemData(iSel);
		if ( NULL != CBlockConfig::Instance()->FindBlock(iBlockId) )
		{
			m_PhaseBlockParam.m_iBlockId = iBlockId;
		}
	}

	if ( m_bUseStaticParam )
	{
		s_PhaseBlockParam = m_PhaseBlockParam;		// 保存静态的
		SavePhaseBlockParamStatic();
	}
	
	CDialogEx::OnOK();
}

void CDlgPhaseSort::PromptErrorInput( const CString &StrPrompt, CWnd *pWndFocus /*= NULL*/ )
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

void CDlgPhaseSort::SetPhaseBlockParam( const T_PhaseOpenBlockParam *pInputBlockParam )
{
	if ( NULL == pInputBlockParam )
	{
		LoadPhaseBlockParamStatic(true);	// 看是否有必要初始化
		m_PhaseBlockParam = s_PhaseBlockParam;
		m_bUseStaticParam = true;
	}
	else
	{
		m_PhaseBlockParam = *pInputBlockParam;
		m_bUseStaticParam = false;
	}
}

void CDlgPhaseSort::FillCBBlockList( bool32 bOften )
{
	// 分为分类板块&用户板块 vs 普通板块
	CBlockConfig::BlockArray aBlocks;
	if ( bOften )
	{
		CBlockConfig::BlockArray aUser;
		CBlockConfig::Instance()->GetMarketClassBlocks(aBlocks);
		CBlockConfig::Instance()->GetUserBlocks(aUser);
		aBlocks.Append(aUser);
	}
	else
	{
		CBlockConfig::Instance()->GetNormalBlocks(aBlocks);
	}
	
	CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(m_PhaseBlockParam.m_iBlockId);
	if ( NULL == pBlock )
	{
		return;
	}

	m_CBBlockList.ResetContent();

	int32 iSel = -1;
	for ( int32 i=0; i < aBlocks.GetSize() ; i++ )
	{
		int32 iIndex = m_CBBlockList.AddString(aBlocks[i]->m_blockInfo.m_StrBlockName);
		m_CBBlockList.SetItemData(iIndex, (DWORD)aBlocks[i]->m_blockInfo.m_iBlockId);
		if ( aBlocks[i] == pBlock )
		{
			iSel = iIndex;
		}
	}

	if ( iSel >= 0 )
	{
		m_CBBlockList.SetCurSel(iSel);
	}
	else
	{
		m_CBBlockList.SetCurSel(0);
	}
}

void CDlgPhaseSort::OnBtnOftenClicked()
{
	FillCBBlockList(true);
}

void CDlgPhaseSort::OnBtnNormalClicked()
{
	FillCBBlockList(false);
}

//////////////////////////////////////////////////////////////////////////
const TCHAR *KStrXMLDlgPhaseSort = _T("DlgPhaseSortSetting");
const TCHAR *KStrXMLDPSTimeStart = _T("TimeStart");
const TCHAR *KStrXMLDPSTimeEnd	= _T("TimeEnd");
const TCHAR *KStrXMLDPSPhaseSortType = _T("SortType");
const TCHAR *KStrXMLDPSPreWeight = _T("PreWeight");
const TCHAR *KStrXMLDPSBlockId	= _T("BlockId");


void CDlgPhaseSort::SavePhaseBlockParamStatic()
{
	// 保存最后一次的结果
	CString StrValue;
	StrValue.Format(_T("%d"), s_PhaseBlockParam.m_TimeStart.GetTime());
	CEtcXmlConfig::Instance().WriteEtcConfig(KStrXMLDlgPhaseSort, KStrXMLDPSTimeStart, StrValue);
	StrValue.Format(_T("%d"), s_PhaseBlockParam.m_TimeEnd.GetTime());
	CEtcXmlConfig::Instance().WriteEtcConfig(KStrXMLDlgPhaseSort, KStrXMLDPSTimeEnd, StrValue);

	StrValue.Format(_T("%d"), s_PhaseBlockParam.m_ePhaseSortType);
	CEtcXmlConfig::Instance().WriteEtcConfig(KStrXMLDlgPhaseSort, KStrXMLDPSPhaseSortType, StrValue);

	StrValue.Format(_T("%d"), s_PhaseBlockParam.m_bDoPreWeight);
	CEtcXmlConfig::Instance().WriteEtcConfig(KStrXMLDlgPhaseSort, KStrXMLDPSPreWeight, StrValue);

	StrValue.Format(_T("%d"), s_PhaseBlockParam.m_iBlockId);
	CEtcXmlConfig::Instance().WriteEtcConfig(KStrXMLDlgPhaseSort, KStrXMLDPSBlockId, StrValue);
}

void CDlgPhaseSort::LoadPhaseBlockParamStatic( bool32 bOnlyFirst )
{
	// 读取最后一次的结果
	static bool32 s_bFirst = false;
	if ( !bOnlyFirst || s_bFirst )
	{
		s_bFirst = true;	// 仅尝试加载一次

		CString StrValue;
		if ( CEtcXmlConfig::Instance().ReadEtcConfig(KStrXMLDlgPhaseSort, KStrXMLDPSTimeStart, NULL, StrValue) )
		{
			time_t tStart = (time_t)_ttol(StrValue);
			if ( tStart >= 0 && tStart <= s_PhaseBlockParam.m_TimeEnd.GetTime() )
			{
				s_PhaseBlockParam.m_TimeStart = tStart;
			}
		}
		if ( CEtcXmlConfig::Instance().ReadEtcConfig(KStrXMLDlgPhaseSort, KStrXMLDPSTimeEnd, NULL, StrValue) )
		{
			//s_PhaseBlockParam.m_TimeEnd = (time_t)_ttol(StrValue);	// 结束时间默认当前
		}
		
		if ( CEtcXmlConfig::Instance().ReadEtcConfig(KStrXMLDlgPhaseSort, KStrXMLDPSPhaseSortType, NULL, StrValue) )
		{
			E_PhaseSortType eSort = (E_PhaseSortType)_ttoi(StrValue);
			if ( eSort < EPST_Count )
			{
				s_PhaseBlockParam.m_ePhaseSortType = eSort;
			}
		}
		
		if ( CEtcXmlConfig::Instance().ReadEtcConfig(KStrXMLDlgPhaseSort, KStrXMLDPSPreWeight, NULL, StrValue) )
		{
			s_PhaseBlockParam.m_bDoPreWeight = 0 != _ttol(StrValue);
		}
		
		if ( CEtcXmlConfig::Instance().ReadEtcConfig(KStrXMLDlgPhaseSort, KStrXMLDPSBlockId, NULL, StrValue) )
		{
			s_PhaseBlockParam.m_iBlockId = _ttol(StrValue);
		}
	}
}

bool32 CDlgPhaseSort::PhaseSortSetting( OUT T_PhaseOpenBlockParam &BlockParam, IN const T_PhaseOpenBlockParam *pInputBlockParam /*= NULL*/ )
{
	//CDlgPhaseSort dlg(AfxGetMainWnd());
	CDlgPhaseSort dlg;
	dlg.SetPhaseBlockParam(pInputBlockParam);
	if ( dlg.DoModal() == IDOK )
	{
		BlockParam = dlg.m_PhaseBlockParam;
		return true;
	}
	return false;
}

void CDlgPhaseSort::GetPhaseSortSetting( OUT T_PhaseOpenBlockParam &BlockParam )
{
	LoadPhaseBlockParamStatic(true);
	BlockParam = s_PhaseBlockParam;
}

