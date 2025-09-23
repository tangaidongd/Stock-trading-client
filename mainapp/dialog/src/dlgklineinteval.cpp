// dlgklineinteval.cpp : implementation file
//

#include "stdafx.h"

#include "dlgklineinteval.h"
#include "MerchManager.h"

#include "MPIChildFrame.h"
#include "IoViewPhaseSort.h"
#include "DlgPhaseSort.h"
#include "IoViewShare.h"
#include "IoViewKLine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgKLineInteval dialog


CDlgKLineInteval::CDlgKLineInteval(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgKLineInteval::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgKLineInteval)
	m_fClose = 0.0f;
	m_fHigh = 0.0f;
	m_fLow = 0.0f;
	m_fOpen = 0.0f;
	m_StrTimeBegin = _T("");
	m_StrTimeEnd = _T("");
	m_fAvgPrice = 0.0f;
	m_StrNum = _T("");
	m_StrVolumn = _T("");
	m_StrAmount = _T("");
	m_StrRiseRate = _T("");
	m_StrShakeRate = _T("");
	m_StrChange = _T("");
	m_TimeBegin = 0;
	m_TimeEnd = 0;
	//}}AFX_DATA_INIT

	m_eInterval = ENTICount;
	m_fCircAsset= 0.0;
	m_pMerch = NULL;
	m_iUserDay = 0;
	m_fPricePreClose = 0.;
	m_pIoViewKLine = NULL;
}


void CDlgKLineInteval::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgKLineInteval)
	DDX_Text(pDX, IDC_EDIT_CLOSE, m_fClose);
	DDX_Text(pDX, IDC_EDIT_HIGH, m_fHigh);
	DDX_Text(pDX, IDC_EDIT_LOW, m_fLow);
	DDX_Text(pDX, IDC_EDIT_OPEN, m_fOpen);
	DDX_Text(pDX, IDC_EDIT_TIME_BEGIN, m_StrTimeBegin);
	DDX_Text(pDX, IDC_EDIT_TIME_END, m_StrTimeEnd);
	DDX_Text(pDX, IDC_EDIT1_AVGPRICE, m_fAvgPrice);
	DDX_Text(pDX, IDC_STATIC_NUM, m_StrNum);
	DDX_Text(pDX, IDC_EDIT_VOLUMN, m_StrVolumn);
	DDX_Text(pDX, IDC_EDIT1_AMOUNT, m_StrAmount);
	DDX_Text(pDX, IDC_EDIT_RISE_RATE, m_StrRiseRate);
	DDX_Text(pDX, IDC_EDIT_SHAKE_RATE, m_StrShakeRate);
	DDX_Text(pDX, IDC_EDIT_CHANGE, m_StrChange);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_BEGIN, m_TimeBegin);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END, m_TimeEnd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgKLineInteval, CDialogEx)
	//{{AFX_MSG_MAP(CDlgKLineInteval)
	ON_BN_CLICKED(IDC_BUTTON_INTERVAL_SORT, OnButtonIntervalSort)
	ON_BN_CLICKED(IDC_BUTTON_BLOCK_SORT, OnButtonBlockSort)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_BEGIN, OnDatetimechangeDatetimepickerBegin)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_END, OnDatetimechangeDatetimepickerEnd)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgKLineInteval message handlers
BOOL CDlgKLineInteval::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	UpdateShow();

	//
	return TRUE;
}

BOOL CDlgKLineInteval::PreTranslateMessage(MSG* pMsg)
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

void CDlgKLineInteval::OnButtonIntervalSort() 
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

void CDlgKLineInteval::OnButtonBlockSort() 
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	//CMPIChildFrame *pChildFrame = (CMPIChildFrame *)pMainFrame->MDIGetActive();

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

void CDlgKLineInteval::SetKLineData(CIoViewKLine* pKLine, CMerch* pMerch, E_NodeTimeInterval eInterval, int32 iUserDay, float fCircAsset, float fPreClose, const CArray<CKLine, CKLine>& aKLineSrc)
{
	m_pIoViewKLine = pKLine;
	m_pMerch	 = pMerch; 
	m_eInterval  = eInterval; 
	m_iUserDay	 = iUserDay;
	m_fCircAsset = fCircAsset; 
	m_fPricePreClose = fPreClose;
	m_aKLines.Copy(aKLineSrc); 
}

void CDlgKLineInteval::UpdateShow()
{
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

	CKLine klineEnd = pKLine[0];
	m_StrTimeBegin = Time2String(pKLine[0].m_TimeCurrent, m_eInterval);
	CString str;
    CGmtTime gtTimeEnd;
	for (int i = 1; i < iSize; i++)
	{
		if(0 != pKLine[i].m_TimeCurrent.GetTime())
		{
			klineEnd = pKLine[i];
		}
		else
		{
			break;
		}
	}

	m_StrTimeEnd   = Time2String(klineEnd.m_TimeCurrent, m_eInterval);

	//
	m_TimeBegin	   = pKLine[0].m_TimeCurrent.GetTime();
	m_TimeEnd	   = klineEnd.m_TimeCurrent.GetTime();

	// 开盘价
	m_fOpen = pKLine[0].m_fPriceOpen;

	// 收盘价
	m_fClose = klineEnd.m_fPriceClose;

	// 最高最低
	m_fHigh = pKLine[0].m_fPriceHigh;
	m_fLow  = pKLine[0].m_fPriceLow;

	// 
	float fVolumn = 0.;
	float fAmount = 0.;

	//
	for ( int32 i = 0; i < iSize; i++ )
	{
		m_fHigh = m_fHigh > pKLine[i].m_fPriceHigh ? m_fHigh : pKLine[i].m_fPriceHigh;
		m_fLow  = m_fLow < pKLine[i].m_fPriceLow  ? m_fLow : pKLine[i].m_fPriceLow;

		// 成交量
		fVolumn += pKLine[i].m_fVolume;

		// 成交额
		fAmount += pKLine[i].m_fAmount;
	}

	//
	m_StrAmount = Float2String(fAmount, 2, true);
	m_StrVolumn = Float2String(fVolumn, 2, true);

	// 加权均价: 成交额/成交量
	if ( 0. != fVolumn )
	{
		m_fAvgPrice = fAmount / fVolumn;
		m_fAvgPrice/= 100.0f;
	}

	// 区间涨幅
	float fRiseRate = 0.;
	float fPreClose = m_fPricePreClose;
	if ( 0. == fPreClose )
	{
		fPreClose = pKLine[0].m_fPriceClose;
	}

	if ( 0. != fPreClose )
	{		
		fRiseRate = (klineEnd.m_fPriceClose - fPreClose) / fPreClose;
		fRiseRate*= 100.0f;
		m_StrRiseRate.Format(L"%.2f(%.2f%%)", (klineEnd.m_fPriceClose - fPreClose), fRiseRate);
	}
	else
	{
		m_StrRiseRate.Empty();
	}

	// 区间振幅
	float fShakeRate = 0.;
	if ( 0. != m_fLow )
	{
		fShakeRate = (m_fHigh - m_fLow) / m_fLow;
		fShakeRate*= 100.0f;
		m_StrShakeRate.Format(L"%.2f(%.2f%%)", (m_fHigh - m_fLow), fShakeRate);
	}

	// 区间换手
	if ( 0. != m_fCircAsset )
	{
		float fChange = fVolumn / m_fCircAsset;
		fChange *= 100.0f;
		fChange *= 100.0f;

		m_StrChange.Format(L"%.2f%%", fChange);
	}	
	else
	{
		m_StrChange.Empty();
	}

	// 个数
	m_StrNum.Format(L"%d", m_aKLines.GetSize());

	//
	UpdateData(FALSE);
}

CString GetTimeString(const CGmtTime& Time, bool32 bOnlyDay /*= false*/)
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

CString GetTimeString(CTime& Time)
{
	CString StrTime;
	
	StrTime.Format(L"%04d-%02d-%02d", Time.GetYear(), Time.GetMonth(), Time.GetDay());				   
	
	return StrTime;
}

void CDlgKLineInteval::ValidTime(bool32 bBeginChange)
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
				//ASSERT(0);
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
				//ASSERT(0);
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
				//ASSERT(0);
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
				//ASSERT(0);
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

	//
	CTime TimeBgBk = m_TimeBegin;
	CTime TimeEdBk = m_TimeEnd;

	//
	UpdateData(TRUE);

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

	TRACE(L"原来的开始时间: %s 原来的结束时间: %s\n", GetTimeString(TimeBgBk).GetBuffer(), GetTimeString(TimeEdBk).GetBuffer());
	TRACE(L"现在的开始时间: %s 现在的结束时间: %s\n", GetTimeString(m_TimeBegin).GetBuffer(), GetTimeString(m_TimeEnd).GetBuffer());

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

	TRACE(L"所有K 线的起始时间: %s 结束时间: %s 个数: %d\n", GetTimeString(pKLineSrc[0].m_TimeCurrent, true).GetBuffer(), GetTimeString(pKLineSrc[iSrcSize-1].m_TimeCurrent, true).GetBuffer(), iSrcSize);
	//
	memcpyex(pKLineLocal, pKLineSrc + iIndexBegin, (iIndexEnd - iIndexBegin + 1) * sizeof(CKLine));
 
	if ( 0 == iIndexBegin )
	{
		m_fPricePreClose = pKLineSrc[0].m_fPriceClose;
	}
	else
	{
		m_fPricePreClose = pKLineSrc[iIndexBegin - 1].m_fPriceClose;
	}

	//
	UpdateShow();

	//
	if ( NULL != m_pIoViewKLine )
	{
		int32 iSize = m_aKLines.GetSize(); 
		m_pIoViewKLine->OnIntervalTimeChange(pKLineLocal[0].m_TimeCurrent, pKLineLocal[iSize - 1].m_TimeCurrent);
	}
}

bool32 CDlgKLineInteval::BeMiniute()
{
	if ( ENTIMinute == m_eInterval || ENTIMinute5 == m_eInterval || ENTIMinute15 == m_eInterval || ENTIMinute30 == m_eInterval || ENTIMinute60 == m_eInterval || ENTIMinuteUser == m_eInterval )
	{
		return true;
	}

	return false;
}

void CDlgKLineInteval::OnDatetimechangeDatetimepickerBegin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	ValidTime(true);
	*pResult = 0;
}

void CDlgKLineInteval::OnDatetimechangeDatetimepickerEnd(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	ValidTime(false);
	*pResult = 0;
}

void CDlgKLineInteval::OnDestroy()
{
	if ( NULL != m_pIoViewKLine )
	{
		m_pIoViewKLine->ClearIntervalTime();
	}

	CDialogEx::OnDestroy();
}
