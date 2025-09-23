// dlgklinequant.cpp : implementation file
//

#include "stdafx.h"
#include <algorithm>
#include "dlgklinequant.h"
#include "MerchManager.h"

#include "MPIChildFrame.h"
#include "IoViewPhaseSort.h"
#include "DlgPhaseSort.h"
#include "IoViewShare.h"
#include "IoViewKLine.h"
#include "FontFactory.h"
#include "formulaengine.h "

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CFontFactory gFontFactory;	// 全局的字体工厂变量
/////////////////////////////////////////////////////////////////////////////
// CDlgKLineQuant dialog

bool CompareIndexData(T_IndexAnalyseData elem1, T_IndexAnalyseData elem2)  
{  
	return elem1.m_fRiseFallRate < elem2.m_fRiseFallRate;  
}  


CDlgKLineQuant::CDlgKLineQuant(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgKLineQuant::IDD, pParent)
	, m_StrRelativeProfit(L"0.00%")
	, m_StrMaxProfit(L"0.00%")
	, m_iAttackCnt(0)
	, m_StrRisePeriodTotalProfit(L"0.00%")
	, m_StrSuccessRate(L"0.00%")
	, m_StrRisePeriodAvgProfit(L"0.00%")
	, m_StrMaxSingleLoss(L"0.00%")
	, m_StrCumulativeLoss(L"0.00%")
	, m_iFallPeriodPromptCnt(0)
	, m_StrShakePeriodTotalProfit(L"0.00%")
	, m_StrShakePeriodAvgProfit(L"0.00%")
	, m_iShakePeriodPromptCnt(0)
{
	//{{AFX_DATA_INIT(CDlgKLineQuant)
	m_StrNum = _T("");
	m_TimeBegin = 0;
	m_TimeEnd = 0;
	//}}AFX_DATA_INIT

	m_eInterval = ENTICount;
	m_pMerch = NULL;
	m_iUserDay = 0;
	m_bChangeMinStatus = true;
	m_iIndexShowPos   = 0;
	m_iIndexShowCount = 0;
}

CDlgKLineQuant::~CDlgKLineQuant()
{
	m_fontStaticText.DeleteObject();
	m_aIndexData.clear();
}


void CDlgKLineQuant::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgKLineQuant)
	DDX_Text(pDX, IDC_EDIT_TIME_BEGIN, m_StrTimeBegin);
	DDX_Text(pDX, IDC_EDIT_TIME_END, m_StrTimeEnd);
	DDX_Text(pDX, IDC_STATIC_NUM, m_StrNum);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_BEGIN, m_TimeBegin);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END, m_TimeEnd);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Text(pDX, IDC_EDIT_RELATIVE_PROFIT, m_StrRelativeProfit);
	DDX_Text(pDX, IDC_EDIT_RISE_MAXPROFIT, m_StrMaxProfit);
	DDX_Text(pDX, IDC_EDIT_ATTACK_COUNT, m_iAttackCnt);
	DDX_Text(pDX, IDC_EDIT_RISE_TOTALPROFIT,m_StrRisePeriodTotalProfit);
	DDX_Text(pDX, IDC_EDIT_SUCCESS_RATE, m_StrSuccessRate);
	DDX_Text(pDX, IDC_EDIT_RISE_AVGPROFIT, m_StrRisePeriodAvgProfit);
	DDX_Text(pDX, IDC_EDIT_MAX_SINGLE_LOSS, m_StrMaxSingleLoss);
	DDX_Text(pDX, IDC_EDIT_CUMULATIVE_LOSS, m_StrCumulativeLoss);
	DDX_Text(pDX, IDC_EDIT_FALL_PROMPTCNT, m_iFallPeriodPromptCnt);
	DDX_Text(pDX, IDC_EDIT_SHAKE_TOTALPROFIT, m_StrShakePeriodTotalProfit);
	DDX_Text(pDX, IDC_EDIT_SHAKE_AVGPROFIT, m_StrShakePeriodAvgProfit);
	DDX_Text(pDX, IDC_EDIT_SHAKE_PROMPTCNT, m_iShakePeriodPromptCnt);
}


BEGIN_MESSAGE_MAP(CDlgKLineQuant, CDialogEx)
	//{{AFX_MSG_MAP(CDlgKLineQuant)
	ON_BN_CLICKED(IDC_BUTTON_INTERVAL_SORT, OnButtonIntervalSort)
	ON_BN_CLICKED(IDC_BUTTON_BLOCK_SORT, OnButtonBlockSort)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_BEGIN, OnDatetimechangeDatetimepickerBegin)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_END, OnDatetimechangeDatetimepickerEnd)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_CONTROL_RANGE(BN_CLICKED,IDC_BUTTON_RISE_PERIOD, IDC_BUTTON_SHAKE_PERIOD, OnBtnPeriodClick)
	ON_WM_CTLCOLOR()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, &CDlgKLineQuant::OnNMDblclk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgKLineQuant message handlers
BOOL CDlgKLineQuant::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	RECT rcClient;
	RECT rcBtn;
	GetWindowRect(&rcClient);
	GetDlgItem(IDC_BUTTON_RISE_PERIOD)->GetWindowRect(&rcBtn);
	if (m_bChangeMinStatus)
	{
		rcClient.right = rcBtn.right - rcClient.left + 36 ;
		SetWindowPos(NULL, 0, 0, rcClient.right, rcClient.bottom - rcClient.top, SWP_NOMOVE);
	}
	//---初始化所需数据
	InitIndexParam();

	// 字体调整
	if ( m_fontStaticText.m_hObject == 0 )
	{
		LOGFONT lf = {0};
		lf.lfHeight = -13;
		lf.lfWeight = FW_BOLD;
		_tcscpy(lf.lfFaceName, gFontFactory.GetExistFontName(L"宋体"));///
		m_fontStaticText.CreateFontIndirect(&lf);
		ASSERT( m_fontStaticText.m_hObject );
	}
	// 初始化list控件
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_list.ModifyStyle(0, LVS_REPORT );
	m_list.InsertColumn(0 , L"开始日期", LVCFMT_LEFT, 72);
	m_list.InsertColumn(1 , L"结束日期", LVCFMT_LEFT, 72);
	m_list.InsertColumn(2 , L"收益", LVCFMT_LEFT, 70);
	
	UpdateShow();

	return TRUE;
}

BOOL CDlgKLineQuant::PreTranslateMessage(MSG* pMsg)
{
	if ( WM_KEYDOWN == pMsg->message)
	{
		if (VK_ESCAPE == pMsg->wParam || VK_RETURN == pMsg->wParam)
		{
			OnClose();
		}
	}	
	
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CDlgKLineQuant::OnButtonIntervalSort() 
{
	T_PhaseOpenBlockParam Param;
	CDlgPhaseSort::GetPhaseSortSetting(Param);
	CMainFrame* pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if ( NULL != pMainFrame )
	{
		if ( m_aKLines.GetSize() > 0 )
		{
			Param.m_TimeStart = m_aKLines[0].m_TimeCurrent;
			Param.m_TimeEnd = m_aKLines[m_aKLines.GetUpperBound()].m_TimeCurrent;
			pMainFrame->ShowPhaseSort(&Param);
		}
		else
		{
			pMainFrame->ShowPhaseSort(NULL);
		}
	}
}

void CDlgKLineQuant::OnButtonBlockSort() 
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CMPIChildFrame *pChildFrame = (CMPIChildFrame *)pMainFrame->MDIGetActive();

	pMainFrame->ShowHotBlockReport();

	//
// 	if ( NULL != pChildFrame )
// 	{
// 		if ( pChildFrame->IsLockedSplit() )
// 		{
// 			MessageBox(L"当前页面锁定, 请先解锁.", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
// 			return;
// 		}
// 	}

	//
	//pMainFrame->PostMessage(WM_COMMAND, ID_PIC_BLOCKREPORT, 0);
}

void CDlgKLineQuant::SetKLineData(CIoViewKLine* pKLine, CMerch* pMerch, E_NodeTimeInterval eInterval, int32 iUserDay, int32 iIndexShowPos, int32 iIndexShowCnt, const CArray<CKLine, CKLine>& aKLineSrc)
{
	m_pIoViewKLine = pKLine;
	m_pMerch	 = pMerch; 
	m_eInterval  = eInterval; 
	m_iUserDay	 = iUserDay;
	m_aKLines.Copy(aKLineSrc); 
	m_iIndexShowPos   = iIndexShowPos;
	m_iIndexShowCount = iIndexShowCnt;
}

void CDlgKLineQuant::UpdateShow()
{
	CMainFrame *pMainFrm = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());

	int32 iSize = m_aKLines.GetSize();
	if ( iSize <= 0 )
	{
		return;
	}

	if ( BeMiniute() )
	{
		GetDlgItem(IDC_DATETIMEPICKER_BEGIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DATETIMEPICKER_END)->ShowWindow(SW_HIDE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_TIME_BEGIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_TIME_END)->ShowWindow(SW_HIDE);
	}

	// 开始时间
	CKLine* pKLine = (CKLine*)m_aKLines.GetData();

	m_StrTimeBegin = Time2String(pKLine[0].m_TimeCurrent, m_eInterval);
	m_StrTimeEnd   = Time2String(pKLine[iSize-1].m_TimeCurrent, m_eInterval);

	//
	m_TimeBegin	   = pKLine[0].m_TimeCurrent.GetTime();
	m_TimeEnd	   = pKLine[iSize-1].m_TimeCurrent.GetTime();

	if (pMainFrm)
	{
		int i = 0;
	}
	// K线个数
	m_StrNum.Format(L"%d", m_aKLines.GetSize());

	CString StrTemp = L"";
	float fRiseTotal = 0.0, fFallTotal = 0.0, fShakeTotal = 0.0;
	// 上升阶段
	if (m_aRisePeriod.size() > 0)
	{
		T_IndexAnalyseData RiseAnalyseData = *max_element(m_aRisePeriod.begin(),m_aRisePeriod.end(), CompareIndexData);
		RiseAnalyseData.m_fRiseFallRate *= 100;
		m_StrMaxProfit.Format(L"%0.2f%%",RiseAnalyseData.m_fRiseFallRate);
		m_iAttackCnt = m_aRisePeriod.size();
		
		int iCnt = m_aRisePeriod.size();
		for (int i=0; i<iCnt; i++)
		{
			fRiseTotal += m_aRisePeriod.at(i).m_fRiseFallRate;
		}
		fRiseTotal *= 100;
		m_StrRisePeriodTotalProfit.Format(L"%0.2f%%", fRiseTotal);
		m_StrSuccessRate = L"100%";
		float fAvgProfit = fRiseTotal/iCnt;
		m_StrRisePeriodAvgProfit.Format(L"%0.2f%%", fAvgProfit);
	}
	else
	{
		m_StrMaxProfit = L"0.00%";
		m_StrRisePeriodTotalProfit = L"0.00%";
		m_StrRisePeriodAvgProfit = L"0.00%";
		m_iAttackCnt = 0;
		m_StrSuccessRate = L"0.00%";
	}
	// 下降阶段
	if (m_aFallPeriod.size() > 0)
	{
		T_IndexAnalyseData FallAnalyseData = *min_element(m_aFallPeriod.begin(),m_aFallPeriod.end(), CompareIndexData);
		m_StrMaxSingleLoss.Format(L"%0.2f%%",abs(FallAnalyseData.m_fRiseFallRate * 100));
	
		int iCnt = m_aFallPeriod.size();
		for (int i=0; i<iCnt; i++)
		{
			fFallTotal += m_aFallPeriod.at(i).m_fRiseFallRate;
		}
		fFallTotal *= 100;
		m_StrCumulativeLoss.Format(L"%0.2f%%",abs(fFallTotal));

		m_iFallPeriodPromptCnt = iCnt;
	}
	else 
	{
		m_StrMaxSingleLoss = L"0.00%";
		m_StrCumulativeLoss = L"0.00%";
		m_iFallPeriodPromptCnt = 0;
	}
	// 震荡阶段
	if (m_aShakePeriod.size() > 0)
	{
		int iCnt = m_aShakePeriod.size();
		for (int i=0; i<iCnt; i++)
		{
			fShakeTotal += m_aShakePeriod.at(i).m_fRiseFallRate * 100;
		}
		
		m_StrShakePeriodTotalProfit.Format(L"%0.2f%%", fShakeTotal);
		float fAvgProfit = fShakeTotal/iCnt;
		m_StrShakePeriodAvgProfit.Format(L"%0.2f%%", fAvgProfit);
		m_iShakePeriodPromptCnt = iCnt;
	}
	else
	{
		m_StrShakePeriodTotalProfit = L"0.00%";
		m_StrShakePeriodAvgProfit   = L"0.00%";
		m_iShakePeriodPromptCnt = 0;
	}
	// 相对收益
	m_StrRelativeProfit.Format(L"%0.2f%%",(fRiseTotal + abs(fFallTotal) + fShakeTotal));
	//
	UpdateData(FALSE);
}

CString CDlgKLineQuant::GetTimeString(const CGmtTime& Time, bool32 bOnlyDay /*= false*/)
{
	CString StrTime;
	
	if ( bOnlyDay )
	{
		StrTime.Format(L"%04d-%02d-%02d", Time.GetYear(), Time.GetMonth(), Time.GetDay());				   
	}
	else
	{
		StrTime.Format(L"%04d-%02d-%02d %02d:%02d:%02d",
			Time.GetYear(), Time.GetMonth(), Time.GetDay(), Time.GetHour(), Time.GetMinute(), Time.GetSecond());
	}
	
	return StrTime;
}

CString CDlgKLineQuant::GetTimeString(CTime& Time)
{
	CString StrTime;
	
	StrTime.Format(L"%04d-%02d-%02d", Time.GetYear(), Time.GetMonth(), Time.GetDay());				   
	
	return StrTime;
}

void CDlgKLineQuant::ValidTime(bool32 bBeginChange)
{
	if ( NULL == m_pMerch )
	{
		return;
	}
	
	//
	int32 iScale;
	E_NodeTimeInterval eIntervalCmp;
	E_KLineTypeBase	eKLineBase;

	if ( !GetTimeIntervalInfo(0, m_iUserDay, m_eInterval, eIntervalCmp, eKLineBase, iScale) )
	{
		return;
	}

	//
	int32 iPos;
	CMerchKLineNode* pKLineNode = NULL;
	m_pMerch->FindMerchKLineNode(eKLineBase, iPos, pKLineNode);
	if ( iPos < 0 || NULL == pKLineNode )
	{
		return;
	}

	//	
	int32 iSrcSize = pKLineNode->m_KLines.GetSize();
	if ( iSrcSize <= 0 )
	{
		return;
	}

	//
	CArray<CKLine, CKLine> aKLineSrc;
	aKLineSrc.SetSize(iSrcSize);
	CKLine* pKLineSrc = (CKLine*)aKLineSrc.GetData();
	memcpyex(pKLineSrc, pKLineNode->m_KLines.GetData(), sizeof(CKLine) * iSrcSize);
	
	//
	CArray<CKLine, CKLine> aKLineCombin;

	switch ( m_eInterval )
	{
	case ENTIWeek:
		{
			if (!CMerchKLineNode::CombinWeek(aKLineSrc, aKLineCombin))
			{
				ASSERT(0);
			}

			//
			aKLineSrc.Copy(aKLineCombin);
			iSrcSize = aKLineSrc.GetSize();
			pKLineSrc = (CKLine*)aKLineSrc.GetData();
		}
		break;
	case ENTIDayUser:
		{
			if (!CMerchKLineNode::CombinDayN(aKLineSrc, m_iUserDay, aKLineCombin))
			{
				ASSERT(0);
			}

			//
			aKLineSrc.Copy(aKLineCombin);
			iSrcSize = aKLineSrc.GetSize();
			pKLineSrc = (CKLine*)aKLineSrc.GetData();
		}
		break;
	case ENTIQuarter:
		{
			if (!CMerchKLineNode::CombinMonthN(aKLineSrc, 3, aKLineCombin))
			{
				ASSERT(0);
			}

			//
			aKLineSrc.Copy(aKLineCombin);
			iSrcSize = aKLineSrc.GetSize();
			pKLineSrc = (CKLine*)aKLineSrc.GetData();
		}
		break;
	case ENTIYear:
		{
			if (!CMerchKLineNode::CombinMonthN(aKLineSrc, 12, aKLineCombin))
			{
				ASSERT(0);
			}

			//
			aKLineSrc.Copy(aKLineCombin);
			iSrcSize = aKLineSrc.GetSize();
			pKLineSrc = (CKLine*)aKLineSrc.GetData();
		}
		break;
	default:
		break;
	}

	UpdateData(TRUE);
	//
	CTime TimeBgBk = m_TimeBegin;
	CTime TimeEdBk = m_TimeEnd;
	
	//
	if ( m_TimeBegin > m_TimeEnd )
	{
		if ( bBeginChange )
		{
			m_TimeBegin = m_TimeEnd;
		}
		else
		{
			m_TimeEnd = m_TimeBegin;
		}		
	}

	//
	int32 iIndexBegin = 0;
	int32 iIndexEnd	  = 0;

	//
	CGmtTime TimeBegin(m_TimeBegin.GetTime());
	CGmtTime TimeEnd(m_TimeEnd.GetTime());

	TRACE(L"原来的开始时间: %s 原来的结束时间: %s\n", GetTimeString(TimeBgBk), GetTimeString(TimeEdBk));
	TRACE(L"现在的开始时间: %s 现在的结束时间: %s\n", GetTimeString(m_TimeBegin), GetTimeString(m_TimeEnd));

	if ( TimeBgBk.GetTime() > TimeBegin.GetTime() )
	{
		// 往时间变小的方向
		iIndexBegin = CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(aKLineSrc, TimeBegin);

		TRACE(L"开始时间变小方向 序号: %d\n", iIndexBegin);
	}
	else /*if ( TimeBgBk.GetTime() < TimeBegin.GetTime() )*/
	{	
		// 往时间变大的方向
		iIndexBegin = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(aKLineSrc, TimeBegin);

		TRACE(L"开始时间变大方向 序号: %d\n", iIndexBegin);
	}
	
	if ( iIndexBegin < 0  )
	{
		m_TimeBegin = pKLineSrc[0].m_TimeCurrent.GetTime();
		iIndexBegin = 0;

		TRACE(L"开始时间序号非法, 调整为0\n");
	}

	//
	if ( TimeEdBk.GetTime() > TimeEnd.GetTime() )
	{
		// 往时间变小的方向
		iIndexEnd = CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(aKLineSrc, TimeEnd);

		TRACE(L"结束时间变小方向 序号: %d\n", iIndexEnd);
	}
	else /*if ( TimeEdBk.GetTime() < TimeEnd.GetTime() )*/
	{
		// 往时间变大的方向
		iIndexEnd = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(aKLineSrc, TimeEnd);

		TRACE(L"结束时间变大方向 序号: %d\n", iIndexEnd);
	}
	if ( iIndexEnd < 0 )
	{
		m_TimeEnd = pKLineSrc[iSrcSize - 1].m_TimeCurrent.GetTime();
		iIndexEnd = iSrcSize - 1;

		TRACE(L"开始时间序号非法, 调整为%d\n", iIndexEnd);
	}

	//
	m_aKLines.SetSize(iIndexEnd - iIndexBegin + 1);
	CKLine* pKLineLocal = (CKLine*)m_aKLines.GetData();
	m_iIndexShowPos   = iIndexBegin ;
	m_iIndexShowCount = iIndexEnd - iIndexBegin + 1;
	TRACE(L"所有K 线的起始时间: %s 结束时间: %s 个数: %d\n", GetTimeString(pKLineSrc[0].m_TimeCurrent, true), GetTimeString(pKLineSrc[iSrcSize-1].m_TimeCurrent, true), iSrcSize);
	//
	memcpyex(pKLineLocal, pKLineSrc + iIndexBegin, (iIndexEnd - iIndexBegin + 1) * sizeof(CKLine));
 
	//
	if ( NULL != m_pIoViewKLine )
	{
		int32 iSize = m_aKLines.GetSize(); 
		m_pIoViewKLine->OnIntervalTimeChange(pKLineLocal[0].m_TimeCurrent, pKLineLocal[iSize - 1].m_TimeCurrent);
	}

	InitIndexParam();
	//
	UpdateShow();

}

bool32 CDlgKLineQuant::BeMiniute()
{
	if ( ENTIMinute == m_eInterval || ENTIMinute5 == m_eInterval || ENTIMinute15 == m_eInterval || ENTIMinute30 == m_eInterval || ENTIMinute60 == m_eInterval || ENTIMinuteUser == m_eInterval )
	{
		return true;
	}

	return false;
}

void CDlgKLineQuant::OnDatetimechangeDatetimepickerBegin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	ValidTime(true);
	*pResult = 0;
}

void CDlgKLineQuant::OnDatetimechangeDatetimepickerEnd(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	ValidTime(false);
	*pResult = 0;
}

void CDlgKLineQuant::OnDestroy()
{
	if ( NULL != m_pIoViewKLine )
	{
		m_pIoViewKLine->ClearIntervalTime();
	}

	CDialogEx::OnDestroy();
}

void CDlgKLineQuant::OnBtnPeriodClick( UINT nId )
{
	RECT rcClient;
	RECT rcBtn;
	GetWindowRect(&rcClient);
	GetDlgItem(nId)->GetWindowRect(&rcBtn);
	if (m_bChangeMinStatus)
	{
		m_bChangeMinStatus = false;
		rcClient.right = rcBtn.right - rcClient.left + 290;
		SetWindowPos(NULL, 0, 0, rcClient.right, rcClient.bottom - rcClient.top, SWP_NOMOVE);
		GetDlgItem(nId)->SetWindowText(L"<<");
		
	}
	else
	{
		m_bChangeMinStatus = true;
		rcClient.right = rcBtn.right - rcClient.left + 36 ;
		SetWindowPos(NULL, 0, 0, rcClient.right, rcClient.bottom - rcClient.top, SWP_NOMOVE);
		GetDlgItem(nId)->SetWindowText(L">>");
	}
	UpdateBtnStatus(nId);
}

void CDlgKLineQuant::UpdateBtnStatus(UINT nID)
{
	for (UINT n=IDC_BUTTON_RISE_PERIOD; n<=IDC_BUTTON_SHAKE_PERIOD; n++)
	{
		if (m_bChangeMinStatus)
		{
			GetDlgItem(n)->EnableWindow(TRUE);
		}
		else if (n != nID && !m_bChangeMinStatus)
		{
			GetDlgItem(n)->EnableWindow(FALSE);
		}
	}
	ShowListData(nID);
}

void CDlgKLineQuant::ShowListData(UINT nID)
{
	switch(nID)
	{
	case IDC_BUTTON_RISE_PERIOD:
		{
			ChangeItemData(m_aRisePeriod);
		}
		break;
	case IDC_BUTTON_FALL_PERIOD:
		{
			ChangeItemData(m_aFallPeriod);
		}
		break;
	case IDC_BUTTON_SHAKE_PERIOD:
		{
			ChangeItemData(m_aShakePeriod);
		}
		break;
	}
}

void CDlgKLineQuant::ChangeItemData(const IndexAnalyseDataArray &arrAnalyseData)
{
	m_list.DeleteAllItems();
	CString StrTemp(L"");
	m_aItemData.clear();

	for(int i=0; i<arrAnalyseData.size(); i++)
	{
		CGmtTime timeStart(arrAnalyseData.at(i).m_iStartTime);
		CGmtTime timeEnd(arrAnalyseData.at(i).m_iEndTime);

		T_ListItemData itemData(timeStart, timeEnd);

		StrTemp.Format(L"%4d%02d%02d", itemData.timeStart.GetYear(), itemData.timeStart.GetMonth(),itemData.timeStart.GetDay());
		m_list.InsertItem(i,StrTemp );
		m_list.SetItemData(i,  (DWORD)i);
		m_aItemData.push_back(itemData);

		StrTemp.Format(L"%4d%02d%02d", itemData.timeEnd.GetYear(), itemData.timeEnd.GetMonth(),itemData.timeEnd.GetDay());
		m_list.SetItemText(i, 1, StrTemp);
		StrTemp.Format(L"%0.2f%%", arrAnalyseData.at(i).m_fRiseFallRate * 100);
		m_list.SetItemText(i, 2, StrTemp);
	}
}

HBRUSH CDlgKLineQuant::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if ( IDC_STATIC_RELATIVE_PROFIT == pWnd->GetDlgCtrlID())
	{
		pDC->SetTextColor(RGB(255, 0,0));
		pDC->SelectObject(&m_fontStaticText);
	}
	
	return hbr;
}

void CDlgKLineQuant::InitIndexParam()
{
 	T_MerchNodeUserData* pData = m_pIoViewKLine->m_MerchParamArray.GetSize() > 0 ? m_pIoViewKLine->m_MerchParamArray[0] : NULL;
 	if ( NULL == pData )
 	{
 		ASSERT( 0 );
 		return ;
 	}
 	m_aIndexData.clear();
 	// 检查是否存在该index 
 	for ( int32 i=0; i < pData->aIndexs.GetSize() ; i++ )
 	{
 		// 找出短线或中线操作指标计算结果
 		if ( 0 == _tcscmp(pData->aIndexs[i]->strIndexName, L"短线操盘") || 0 == _tcscmp(pData->aIndexs[i]->strIndexName, L"中线操盘") )
 		{
 			// 当前屏幕显示的起始位置
 			int32 iIndexShowPosInFullList   = m_iIndexShowPos;					
 			int32 iIndexShowCountInFullList = m_iIndexShowCount;	
 			int32 iCnt = pData->aIndexs[i]->m_aIndexLines.GetSize();
 			
			bool32 bFind = false;
 			for (int j=0; j<iCnt; j++)
 			{
 				int32 iSize = pData->aIndexs[i]->m_aIndexLines[j].m_aNodesFull.GetSize();
 				for (int k=iIndexShowPosInFullList; k<iIndexShowPosInFullList+iIndexShowCountInFullList; k++)
 				{
 					T_IndexData IndexData; 
 					CNodeData nodeData = pData->aIndexs[i]->m_aIndexLines[j].m_aNodesFull[k];
 					if(0x00ffff == pData->aIndexs[i]->m_aIndexLines[j].m_aNodesFull[k].m_clrNode)
 					{
 						if ( !CheckFlag(pData->aIndexs[i]->m_aIndexLines[j].m_aNodesFull[k].m_iFlag, CNodeData::KValueInvalid))
 						{
 							IndexData.eClrType = EICT_YELLOW;		
 							IndexData.m_iTime  = pData->aIndexs[i]->m_aIndexLines[j].m_aNodesFull[k].m_iID;
 						}
						else
						{
							IndexData.eClrType = EICT_BLUE;
							IndexData.m_iTime  = pData->aIndexs[i]->m_aIndexLines[j].m_aNodesFull[k].m_iID;
						}
						bFind = true;
						IndexData.m_fClose = m_aKLines.GetAt(k - m_iIndexShowPos).m_fPriceClose;
//  					CString StrOutput;
//  					StrOutput.Format(L"type:%d Close:%lf  date:%d\r\n", IndexData.eClrType, IndexData.m_fClose, IndexData.m_iTime);
//  					OutputDebugString(StrOutput);
						m_aIndexData.push_back(IndexData);
 					}		
 				}
				if (bFind)
				{
					break;
				}
 			}
 		}
 	}
	if (!m_aIndexData.empty())
	{
		AnalyseIndexData();
	}
}

void CDlgKLineQuant::AnalyseIndexData()
{
	// 蓝转黄
	ParseBlueToYellowData();
	// 黄转蓝
	ParseYellowToBlueData();
}

void CDlgKLineQuant::ParseBlueToYellowData()
{
	int32 iStartPos = 0;
	int32 iEndPos   = 0;
	bool32 bFindBlue = false;
	bool32 bHaveCompleteData = false;
	int32  iIndex = 0;
	m_aFallPeriod.clear();
	int32   iDataSize = m_aIndexData.size();
	for(int i=0; i< iDataSize; i++)
	{
		if (EICT_BLUE == m_aIndexData.at(i).eClrType)
		{
			if(!bFindBlue)
			{
				iStartPos = i;
				bFindBlue = true;
			}
			else if (iDataSize - 1 == i && iStartPos != i)
			{
				bFindBlue = true;
				iEndPos = i;
				bHaveCompleteData = true;
			}
		}
		else
		{
			if (bFindBlue)
			{
				bFindBlue = false;
				iEndPos   = i;
				bHaveCompleteData = true;
			}
		}
		//---取得前后节点数据后就开始保存了
		if(bHaveCompleteData)
		{
			bHaveCompleteData = false;
			T_IndexAnalyseData analyseData;
			analyseData.m_iStartTime = m_aIndexData.at(iStartPos).m_iTime;
			analyseData.m_iEndTime   = m_aIndexData.at(iEndPos).m_iTime;
			analyseData.m_fRiseFallRate = (m_aIndexData.at(iEndPos).m_fClose - m_aIndexData.at(iStartPos).m_fClose)/m_aIndexData.at(iStartPos).m_fClose;

			if (analyseData.m_fRiseFallRate + 0.03 < 1e-7)
			{
				m_aFallPeriod.push_back(analyseData);
			}
		}
	}
}

void CDlgKLineQuant::ParseYellowToBlueData()
{
	int32 iStartPos = 0;
	int32 iEndPos   = 0;
	bool32 bFindYellow = false;
	m_aShakePeriod.clear();
	m_aRisePeriod.clear();
	bool32 bHaveCompleteData = false;
	int32  iIndex = 0;
	int32   iDataSize = m_aIndexData.size();

	for(int i=0; i< iDataSize; i++)
	{
		if (EICT_YELLOW == m_aIndexData.at(i).eClrType)
		{
			if(!bFindYellow)
			{
				iStartPos = i;
				bFindYellow = true;
			}
			else if (iDataSize - 1 == i && iStartPos != i)
			{
				bFindYellow = true;
				iEndPos = i;
				bHaveCompleteData = true;
			}
		}
		else
		{
			if (bFindYellow)
			{
				bFindYellow = false;
				iEndPos   = i;
				bHaveCompleteData = true;
			}
		}
		//---取得前后节点数据后就开始保存了
		if(bHaveCompleteData)
		{
			bHaveCompleteData = false;
			T_IndexAnalyseData analyseData;
			analyseData.m_iStartTime = m_aIndexData.at(iStartPos).m_iTime;
			analyseData.m_iEndTime   = m_aIndexData.at(iEndPos).m_iTime;
			analyseData.m_fRiseFallRate = (m_aIndexData.at(iEndPos).m_fClose - m_aIndexData.at(iStartPos).m_fClose)/m_aIndexData.at(iStartPos).m_fClose;
			
			if (analyseData.m_fRiseFallRate - 0.03 <= 1e-7 && analyseData.m_fRiseFallRate + 0.03 >= 1e-7)
			{
				m_aShakePeriod.push_back(analyseData);
			}
			else if (analyseData.m_fRiseFallRate - 0.03 > 1e-7)
			{
				m_aRisePeriod.push_back(analyseData);
			}
			else if(analyseData.m_fRiseFallRate + 0.03 < 1e-7)
			{
				analyseData.m_fRiseFallRate = -0.03f;
				m_aShakePeriod.push_back(analyseData);
			}
		}
	}
}

void CDlgKLineQuant::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if ( NULL != m_pIoViewKLine  )
	{
		int32 iPos = (int32)m_list.GetItemData(pNMItemActivate->iItem);
	
		T_ListItemData itemData = m_aItemData.at(iPos);
		
		m_pIoViewKLine->SetIntervalTime(itemData.timeStart, itemData.timeEnd);

		// 显示刷新
		m_pIoViewKLine->ReDrawAysnc();
	}
	
	*pResult = 0;
}

