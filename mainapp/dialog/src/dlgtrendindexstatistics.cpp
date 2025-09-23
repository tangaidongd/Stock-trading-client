// dlgtrendindexstatistics.cpp : implementation file
//
#include "stdafx.h"
#include "dlgtrendindexstatistics.h"
#include "GridCell.h"
#include "GridCellSys.h"
#include <afxtempl.h>
#include "IoViewKLine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTrendIndexstatistics dialog
 

CDlgTrendIndexstatistics::CDlgTrendIndexstatistics(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgTrendIndexstatistics::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTrendIndexstatistics)
	m_TimeBgYMD = 0;
	m_TimeEdHMS = 0;
	m_TimeBgHMS = 0;
	m_TimeEdYMD = 0;
	m_iRadio	= 0;
	m_fStability = 0.0f;
	m_fStrong = 0.0f;
	m_fWeek = 0.0f;
	m_iMA13 = 0;
	m_iMA5 = 0;
	m_iMA8 = 0;
	m_uiCdt5KLineNums = 0;
	m_fIntensityOC = 0.0f;
	m_fMA13QZ = 0.0f;
	m_fMA5QZ = 0.0f;
	m_fMA8QZ = 0.0f;
	m_bCdt5 = FALSE;
	m_bCdt2 = FALSE;
	m_bCdt3 = FALSE;
	m_bCdt4 = FALSE;
	m_fCdt5Times = 0.0f;
	//}}AFX_DATA_INIT
	
	m_iBegin		= 0;
	m_iBeginInitial = 0;
	m_iEnd			= 0;
	m_iEndInitial	= 0;

	m_bStrart		= false;
	m_bNewStatistica= false;
	m_pParentIoView = NULL;
	m_aTrendIndexNode.RemoveAll();
	m_iHeadIndex = 0;
}


void CDlgTrendIndexstatistics::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTrendIndexstatistics)
	DDX_Control(pDX, IDC_EDIT_GRIDRECT, m_Edit);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_BEGIN_YMD, m_TimeBgYMD);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END_HMS, m_TimeEdHMS);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_BEGIN_HMS, m_TimeBgHMS);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END_YMD, m_TimeEdYMD);
	DDX_Radio(pDX,IDC_RADIO1,m_iRadio);
	DDX_Text(pDX, IDC_EDIT_STABILITY, m_fStability);
	DDX_Text(pDX, IDC_EDIT_STRONG, m_fStrong);
	DDX_Text(pDX, IDC_EDIT_WEEK, m_fWeek);
	DDX_Text(pDX, IDC_EDIT_MA13, m_iMA13);
	DDX_Text(pDX, IDC_EDIT_MA5, m_iMA5);
	DDX_Text(pDX, IDC_EDIT_MA8, m_iMA8);
	DDX_Text(pDX, IDC_EDIT_CONDITION5_KLINENUM, m_uiCdt5KLineNums);
	DDX_Text(pDX, IDC_EDIT_INTENSITYOC, m_fIntensityOC);
	DDX_Text(pDX, IDC_EDIT_MA13QZ, m_fMA13QZ);
	DDX_Text(pDX, IDC_EDIT_MA5QZ, m_fMA5QZ);
	DDX_Text(pDX, IDC_EDIT_MA8QZ, m_fMA8QZ);
	DDX_Check(pDX, IDC_CHECK_CDT5, m_bCdt5);
	DDX_Check(pDX, IDC_CHECK_CDT2, m_bCdt2);
	DDX_Check(pDX, IDC_CHECK_CDT3, m_bCdt3);
	DDX_Check(pDX, IDC_CHECK_CDT4, m_bCdt4);
	DDX_Text(pDX, IDC_EDIT_CONDITION5_TIMES, m_fCdt5Times);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTrendIndexstatistics, CDialogEx)
	//{{AFX_MSG_MAP(CDlgTrendIndexstatistics)
	ON_BN_CLICKED(IDC_BUTTON_START, OnButtonStart)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_RADIO_0, OnRadio0)
	ON_BN_CLICKED(IDC_RADIO_1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO_2, OnRadio2)
	ON_BN_CLICKED(IDC_BUTTON_CONFIRM, OnButtonConfirm)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_RADIO_3, OnRadio3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTrendIndexstatistics message handlers
// CDialogProgressChooseStock message handlers

BOOL CDlgTrendIndexstatistics::OnInitDialog()
{
	CDialog::OnInitDialog();
	return TRUE;
}

void CDlgTrendIndexstatistics::Initial()
{
/*	m_TimeBgYMD  = CTime::GetCurrentTime();
	m_TimeBgHMS  = CTime::GetCurrentTime();
	m_TimeEdYMD  = CTime::GetCurrentTime();
	m_TimeEdHMS  = CTime::GetCurrentTime();

	m_iMA5		 = GetMA5MoveNums();
	m_iMA8		 = GetMA8MoveNums();
	m_iMA13		 = GetMA13MoveNums();
	m_fStability = GetStabilityTrend();
	m_fStrong	 = GetStrongTrend();
	m_fWeek		 = GetWeekTrend();
	m_fIntensityOC= GetOCTrend();
	m_fMA5QZ	 = GetMA5Weight();
	m_fMA8QZ	 = GetMA8Weight();
	m_fMA13QZ	 = GetMA13Weight();

	m_uiCdt5KLineNums = GetCdt5KLineNums();
	m_fCdt5Times	  = GetCdt5Times();
 
	if ( m_pParentIoView->m_bNeedCondition5 )
	{
		m_bCdt5 = true;
	}
	else
	{
		m_bCdt5 = false;
	}

	if ( m_pParentIoView->m_bNeedCondition4 )
	{
		m_bCdt4 = true;
	}
	else
	{
		m_bCdt4 = false;
	}

	if ( m_pParentIoView->m_bNeedCondition3 )
	{
		m_bCdt3 = true;
	}
	else
	{
		m_bCdt3 = false;
	}

	if ( m_pParentIoView->m_bNeedCondition2 )
	{
		m_bCdt2 = true;
	}
	else
	{
		m_bCdt2 = false;
	}
*/
	UpdateData(false);
	CreateTable();
}

void CDlgTrendIndexstatistics::OnSize(UINT nType, int cx, int cy)
{	
	CDialogEx::OnSize(nType,cx,cy);
	SetPos();
}

void CDlgTrendIndexstatistics::SetPos()
{	
	CRect rectClient;
	if (!m_Edit.GetSafeHwnd())
	{
		return;
	}

	m_Edit.GetWindowRect(&rectClient);
	ScreenToClient(&rectClient);

	int32 iHeightHScroll = m_XSBHorz.GetFitHorW();
	int32 iWidthVScroll = m_XSBVert.GetFitHorW();
	
	if ( NULL != m_XSBHorz.GetSafeHwnd() && NULL != m_XSBVert.GetSafeHwnd())
	{		
		CRect RectHScroll;
		RectHScroll = rectClient;
		RectHScroll.top = RectHScroll.bottom - iHeightHScroll;
		
		m_XSBHorz.SetSBRect(RectHScroll, TRUE);
		m_XSBHorz.ShowWindow(SW_SHOW);
		
		CRect RectVScroll;
		RectVScroll = rectClient;
		RectVScroll.left = RectVScroll.right - iWidthVScroll;
		
		m_XSBVert.SetSBRect(RectVScroll, TRUE);
		m_XSBVert.ShowWindow(SW_SHOW);
	}
	
	CRect rectGrid = rectClient;
	rectGrid.bottom -= iHeightHScroll;
	rectGrid.right  -= iWidthVScroll;
	
	if ( NULL != m_GridCtrl.GetSafeHwnd())
	{
		m_GridCtrl.MoveWindow(&rectGrid);
	}
}

void CDlgTrendIndexstatistics::SetParentIoView(CIoViewKLine* pParent)
{
	m_pParentIoView = pParent;
}

void CDlgTrendIndexstatistics::SetIndexValue(const CArray<T_TrendIndexNode,T_TrendIndexNode>& aBigCyleTrendIndexValue,const CTimeSpan TimeSpanBigCyle,const CArray<T_TrendIndexNode,T_TrendIndexNode>& aTrendIndexValue,int32 iBegin,int32 iEnd)
{
	m_aTrendIndexNode.RemoveAll();
	m_aTrendIndexNode.Copy(aTrendIndexValue);

	m_aBigCyleTrendIndexNode.RemoveAll();
	m_aBigCyleTrendIndexNode.Copy(aBigCyleTrendIndexValue);

	m_TimeSpanBigCyle = TimeSpanBigCyle;
	m_iBeginInitial   = iBegin;
	m_iEndInitial	  = iEnd;
}

bool32 CDlgTrendIndexstatistics::CreateTable()
{
	CRect rect;
	GetClientRect(&rect);
	
	m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10203);
	m_XSBVert.SetScrollRange(0, 10);
	m_XSBVert.ShowWindow(SW_SHOW);
	
	m_XSBHorz.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10204);
	m_XSBHorz.SetScrollRange(0, 0);
	m_XSBHorz.ShowWindow(SW_SHOW);
	
	if (NULL == m_GridCtrl.GetSafeHwnd())
	{
		if (!m_GridCtrl.Create(CRect(0, 0, 0, 0), this, 663217))
			return false;
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	}
	
	// 设置相互之间的关联			
	m_XSBHorz.SetOwner(&m_GridCtrl);
	m_XSBVert.SetOwner(&m_GridCtrl);
	m_GridCtrl.SetScrollBar(&m_XSBHorz, &m_XSBVert);
	//
	SetTableHead(0);
	//
	SetPos();
	//
	m_GridCtrl.Invalidate(TRUE);
	m_GridCtrl.ShowWindow(SW_SHOW);
	return true;
}

bool32 CDlgTrendIndexstatistics::SetTableHead(int32 iIndex)
{
	m_iHeadIndex = iIndex;

	if ( 0 == iIndex )
	{
		// 总的统计
		
		// 清空表格内容
		m_GridCtrl.DeleteAllItems();
		
		// 设置列数
		if (!m_GridCtrl.SetColumnCount(17))
			return false;
		
		//  设置行数
		if (!m_GridCtrl.SetFixedRowCount(1))	
			return false;

		m_GridCtrl.SetHeaderSort(false);

		m_GridCtrl.SetColumnWidth(2,150);
		m_GridCtrl.SetColumnWidth(9,150);
		m_GridCtrl.SetColumnWidth(10,150);
		// 设置表头:
		CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,0);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"操作类型");
		}
		
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,1);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"持仓状态");
		}
		
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,2);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"时间");
		}
		
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,3);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"价格");
		}
		
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,4);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"MA5");
		}
		
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,5);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"MA8");
		}
		
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,6);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"MA13");
		}
		
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,7);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"加权MA");
		}
		
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,8);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"趋势强度");
		}

		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,9);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"当前K线时间");
		}
		
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,10);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"大周期K线时间");
		}	
		
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,11);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"操作提示[大]");
		}
		
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,12);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"持仓状态[大]");
		}

		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,13);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"价格[大]");
		}

		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,14);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"MA5[大]");
		}

		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,15);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"MA8[大]");
		}

		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,16);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"MA13[大]");
		}
	}
	else if ( 1 == iIndex)
	{
		// 清空表格内容
		m_GridCtrl.DeleteAllItems();
		
		// 设置列数
		if (!m_GridCtrl.SetColumnCount(8))
			return false;
		
		//  设置行数
		if (!m_GridCtrl.SetFixedRowCount(1))	
			return false;
		
		m_GridCtrl.SetHeaderSort(true);
		m_GridCtrl.SetCompareFunction(CGridCtrl::pfnCellNumericCompare);

		m_GridCtrl.SetColumnWidth(0,20);
		m_GridCtrl.SetColumnWidth(1,150);
		m_GridCtrl.SetColumnWidth(3,150);
		m_GridCtrl.SetColumnWidth(5,150);

		// 设置表头:
		CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,0);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"序");
		}
		
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,1);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"开仓时间");
		}
		
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,2);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"开仓价格");
		}
		
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,3);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"清仓时间");
		}
		
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,4);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"清仓价格");
		}
		
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,5);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"留仓时间");
		}
		
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,6);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"盈利价格差");
		}
		
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,7);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"盈利率(%)");
		}
	}
	else if ( 2 == iIndex )
	{
		// 清空表格内容
		m_GridCtrl.DeleteAllItems();
		
		// 设置列数
		if (!m_GridCtrl.SetColumnCount(5))
			return false;
		
		//  设置行数
		if (!m_GridCtrl.SetFixedRowCount(1))	
			return false;

		m_GridCtrl.SetHeaderSort(false);
		
		// 设置表头:
		CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,0);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"交易次数");
		}
		
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,1);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"盈利次数");
		}
		
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,2);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"亏损次数");
		}
		
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,3);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"准确率(%)");
		}
		
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(0,4);
		if ( NULL != pCell)
		{
			pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
			pCell->SetText(L"累计盈利率(%)");
		}
	}
	else if ( 3 == iIndex)
	{
		
	}

	return true;
}

void CDlgTrendIndexstatistics::UpdateTable()
{
	m_XSBHorz.SetScrollPos(0);
	m_XSBVert.SetScrollPos(0);

	m_GridCtrl.DeleteNonFixedRows();

	if ( -1 == m_iBegin || -1 == m_iEnd)
	{
		MessageBox(L"没有找到匹配时间!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
		return;
	}

	if ( 0 == m_iHeadIndex)
	{
		for ( int32 i = /*0*/m_iBegin ; i < /*m_aTrendIndexNode.GetSize()*/m_iEnd; i++)
		{
			int32 iRowCount = m_GridCtrl.GetRowCount();
			m_GridCtrl.InsertRow(L"222");
			
			T_TrendIndexNode *pNode    = &m_aTrendIndexNode.GetAt(i);
	
			if ( NULL == pNode)
			{
				continue;
			}
			
			// "操作类型" 		
			CString StrText;
			COLORREF clr = RGB(200,200,200);
			StrText = GetActionPromptString(pNode->m_eActionPrompt,clr);
			
			CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,0);
			if ( NULL != pCell)
			{
				pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
				pCell->SetTextClr(clr);
				pCell->SetText(StrText);
			}
			
			//"持仓状态"
			StrText = GetHoldStateString(pNode->m_eHoldState);
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,1);
			if ( NULL != pCell)
			{
				pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
				pCell->SetTextClr(clr);
				pCell->SetText(StrText);
			}
			
			// "时间"
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,2);
			
			if ( -1 != pNode->m_TimeAction.GetTime())
			{
				StrText.Format(L"%02d/%02d/%02d/ %02d:%02d:%02d",
					pNode->m_TimeAction.GetYear(),
					pNode->m_TimeAction.GetMonth(),
					pNode->m_TimeAction.GetDay(),
					pNode->m_TimeAction.GetHour(),
					pNode->m_TimeAction.GetMinute(),
					pNode->m_TimeAction.GetSecond());
			}
			else
			{
				StrText = L"-";
			}
			
			if ( NULL != pCell)
			{
				pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
				pCell->SetTextClr(clr);
				pCell->SetText(StrText);
			}
			
			// "价格"
			if ( !BeValidFloat(pNode->m_fPrice))
			{
				StrText = L"-";
			}
			else
			{
				StrText.Format(L"%.2f",pNode->m_fPrice);
			}
			
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,3);
			if ( NULL != pCell)
			{
				pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
				pCell->SetTextClr(clr);
				pCell->SetText(StrText);
			}
			
			// MA5
			if ( !BeValidFloat(pNode->m_fMA5))
			{
				StrText = L"-";
			}
			else
			{
				StrText.Format(L"%.2f",pNode->m_fMA5);
			}
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,4);
			if ( NULL != pCell)
			{
				pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
				pCell->SetTextClr(clr);
				pCell->SetText(StrText);
			}
			// MA8
			if ( !BeValidFloat(pNode->m_fMA8) )
			{
				StrText = L"-";
			}
			else
			{
				StrText.Format(L"%.2f",pNode->m_fMA8);
			}
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,5);
			if ( NULL != pCell)
			{
				pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
				pCell->SetTextClr(clr);
				pCell->SetText(StrText);
			}

			
			// MA13
			if ( !BeValidFloat(pNode->m_fMA13))
			{
				StrText = L"-";
			}
			else
			{
				StrText.Format(L"%.2f",pNode->m_fMA13);
	
			}
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,6);
			if ( NULL != pCell)
			{
				pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
				pCell->SetTextClr(clr);
				pCell->SetText(StrText);
			}			

			// 加权MA 
			if ( !BeValidFloat(pNode->m_fMAWeighted))
			{
				StrText = L"-";
			}
			else
			{
				StrText.Format(L"%.2f",pNode->m_fMAWeighted);
			}
			
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,7);
			if ( NULL != pCell)
			{
				pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
				pCell->SetTextClr(clr);
				pCell->SetText(StrText);
			}
			
			// 趋势强度
			if ( !BeValidFloat(pNode->m_fTrendIntensity) )
			{
				StrText = L"-";
			}
			else
			{
				StrText.Format(L"%.2f",pNode->m_fTrendIntensity);
			}			
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,8);
			if ( NULL != pCell)
			{
				pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
				pCell->SetTextClr(clr);
				pCell->SetText(StrText);
			}			
			//

			// 当前K 线时间			
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,9);
			if ( NULL != pCell)
			{
				StrText.Format(L"%02d/%02d/%02d/ %02d:%02d:%02d",
					pNode->m_TimeKLine.GetYear(),
					pNode->m_TimeKLine.GetMonth(),
					pNode->m_TimeKLine.GetDay(),
					pNode->m_TimeKLine.GetHour(),
					pNode->m_TimeKLine.GetMinute(),
					pNode->m_TimeKLine.GetSecond());

				pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
				pCell->SetTextClr(clr);
				pCell->SetText(StrText);
			}			

			// 大周期节点
			T_TrendIndexNode NodeBigCyle;
			bool32 bOK = GetBigCyleNode(pNode->m_TimeKLine,m_aBigCyleTrendIndexNode,m_TimeSpanBigCyle,NodeBigCyle);
			
			if (!bOK)
			{
				continue;
			}

			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,10);
			if ( NULL != pCell)
			{
				StrText.Format(L"%02d/%02d/%02d/ %02d:%02d:%02d",
					NodeBigCyle.m_TimeKLine.GetYear(),
					NodeBigCyle.m_TimeKLine.GetMonth(),
					NodeBigCyle.m_TimeKLine.GetDay(),
					NodeBigCyle.m_TimeKLine.GetHour(),
					NodeBigCyle.m_TimeKLine.GetMinute(),
					NodeBigCyle.m_TimeKLine.GetSecond());
				
				pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
				pCell->SetTextClr(clr);
				pCell->SetText(StrText);
			}
			
			// 大周期操作提示
			StrText = GetActionPromptString(NodeBigCyle.m_eActionPrompt);
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,11);
			if ( NULL != pCell)
			{
				pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
				pCell->SetTextClr(clr);
				pCell->SetText(StrText);
			}		

			// 大周期持仓状态
			StrText = GetHoldStateString(NodeBigCyle.m_eHoldState);
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,12);
			if ( NULL != pCell)
			{
				pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
				pCell->SetTextClr(clr);
				pCell->SetText(StrText);
			}

			// 大周期"价格"
			StrText.Format(L"%.2f",NodeBigCyle.m_fPrice);
			
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,13);
			if ( NULL != pCell)
			{
				pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
				pCell->SetTextClr(clr);
				pCell->SetText(StrText);
			}
			
			// 大周期MA5
			if ( NodeBigCyle.m_fMA5 > 1000000.0)
			{
				StrText = L"-";
			}
			else
			{
				StrText.Format(L"%.2f",NodeBigCyle.m_fMA5);
			}
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,14);
			if ( NULL != pCell)
			{
				pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
				pCell->SetTextClr(clr);
				pCell->SetText(StrText);
			}

			// 大周期MA8
			if ( NodeBigCyle.m_fMA8 > 100000.0)
			{
				StrText = L"-";
			}
			else
			{
				StrText.Format(L"%.2f",NodeBigCyle.m_fMA8);
			}
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,15);
			if ( NULL != pCell)
			{
				pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
				pCell->SetTextClr(clr);
				pCell->SetText(StrText);
			}
						
			// 大周期MA13
			if ( NodeBigCyle.m_fMA13 > 100000.0)
			{
				StrText = L"-";
			}
			else
			{
				StrText.Format(L"%.2f",NodeBigCyle.m_fMA13);
				
			}
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,16);
			if ( NULL != pCell)
			{
				pCell->SetFormat(DT_CENTER|DT_SINGLELINE);
				pCell->SetTextClr(clr);
				pCell->SetText(StrText);
			}
		}
	}
	else if ( 1 == m_iHeadIndex)
	{
		// 统计每一笔的数据:

		int32 i = 0;
		for ( i = /*0*/m_iBegin ; i < /*m_aTrendIndexNode.GetSize()*/m_iEnd; i++)
		{
			T_TrendIndexNode* pNodeBegin = NULL; 
			T_TrendIndexNode* pNodeEnd = NULL;

			if ( EAPDTOC == m_aTrendIndexNode.GetAt(i).m_eActionPrompt )
			{
				pNodeBegin = &m_aTrendIndexNode.GetAt(i);
											
				for ( int32 j = i; j < m_aTrendIndexNode.GetSize(); j++)
				{
					if ( EAPDTCC == m_aTrendIndexNode.GetAt(j).m_eActionPrompt)
					{
						// 找到了一次完整交易
						pNodeEnd = &m_aTrendIndexNode.GetAt(j);
						i = j;
						break;
					}
				}
			}
			else if ( EAPKTOC == m_aTrendIndexNode.GetAt(i).m_eActionPrompt)
			{	
				pNodeBegin = &m_aTrendIndexNode.GetAt(i);
				
				for ( int32 j = i; j < m_aTrendIndexNode.GetSize(); j++)
				{
					if ( EAPKTCC == m_aTrendIndexNode.GetAt(j).m_eActionPrompt)
					{
						// 找到了一次完整交易
						pNodeEnd = &m_aTrendIndexNode.GetAt(j);

						i = j;
						break;
					}
				}
			}
			
			if ( i == m_iEnd - 1)
			{
				// 最后一个比如果处于行情中,而且又不是清仓状态. 那么以这个节点的最新价作为清仓价,这个也作为一笔完整交易:
				T_TrendIndexNode NodeLast = m_aTrendIndexNode.GetAt(m_aTrendIndexNode.GetSize() -1);
				
				if ( EHSDTCY == NodeLast.m_eHoldState || EHSDTJC == NodeLast.m_eHoldState)
				{
					// 是多加,或多减, 往前找到多开那个节点:
					for ( int32 indexNode = m_aTrendIndexNode.GetSize() - 2; indexNode > 0 ; indexNode--)
					{
						T_TrendIndexNode NodeBegin = m_aTrendIndexNode.GetAt(indexNode);

						if ( indexNode - 1 < 0)
						{
							break;
						}

						T_TrendIndexNode NodeBeginPre = m_aTrendIndexNode.GetAt(indexNode-1);
							
						// 这个是多头开仓,而且这个的前一个不是多头开仓

						if ( EAPDTOC == NodeBegin.m_eActionPrompt && EAPDTOC != NodeBeginPre.m_eActionPrompt)
						{
							pNodeBegin = &NodeBegin;
							pNodeEnd   = &NodeLast;
							break;
						}
					}					
				}
				else if ( EHSKTCY == NodeLast.m_eHoldState || EHSKTJC == NodeLast.m_eHoldState )
				{
					// 是空加,或空减,往前找到空开那个节点:
					for ( int32 indexNode = m_aTrendIndexNode.GetSize() - 2; indexNode > 0 ; indexNode--)
					{
						T_TrendIndexNode NodeBegin = m_aTrendIndexNode.GetAt(indexNode);

						if ( indexNode - 1 < 0)
						{
							break;
						}
						
						T_TrendIndexNode NodeBeginPre = m_aTrendIndexNode.GetAt(indexNode-1);
						
						// 这个是多头开仓,而且这个的前一个不是空头开仓

						if ( EAPKTOC == NodeBegin.m_eActionPrompt && EAPKTOC != NodeBeginPre.m_eActionPrompt)
						{
							pNodeBegin = &NodeBegin;
							pNodeEnd   = &NodeLast;
							break;
						}
					}
				} 
			}

			// 表格插入数据:
			if ( NULL == pNodeBegin || NULL == pNodeEnd)
			{
				continue;
			}
			
			// 赚钱还是亏钱了的颜色

			float fPriceChange = 0.0;

			COLORREF clr = RGB(200,200,200);

			if ( EAPDTOC == pNodeBegin->m_eActionPrompt)
			{
				// 多头开仓
				fPriceChange = pNodeEnd->m_fPrice - pNodeBegin->m_fPrice;
			}
			else if ( EAPKTOC == pNodeBegin->m_eActionPrompt)
			{
				// 空头开仓
				fPriceChange = pNodeBegin->m_fPrice - pNodeEnd->m_fPrice;	
			}

			if ( fPriceChange > 0.0 )
			{
				clr = RGB(255,0,0);
			}
			else if ( fPriceChange < 0.0  )
			{
				clr = RGB(0,255,0);
			}

			int32 iRowCount = m_GridCtrl.GetRowCount();
			m_GridCtrl.InsertRow(L"123");
			
			// 序号
			CString StrText;
			StrText.Format(L"%d",iRowCount);
			CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,0);
			pCell->SetTextClr(clr);
			pCell->SetCompareFloat(iRowCount);
			pCell->SetText(StrText);
			
			// 开仓时间				
			if ( -1 != pNodeBegin->m_TimeAction.GetTime())
			{
				StrText.Format(L"%02d/%02d/%02d/ %02d:%02d:%02d",
					pNodeBegin->m_TimeAction.GetYear(),
					pNodeBegin->m_TimeAction.GetMonth(),
					pNodeBegin->m_TimeAction.GetDay(),
					pNodeBegin->m_TimeAction.GetHour(),
					pNodeBegin->m_TimeAction.GetMinute(),
					pNodeBegin->m_TimeAction.GetSecond());					
			}
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,1);
			pCell->SetTextClr(clr);
			pCell->SetText(StrText);
			
			// 开仓价格
			StrText.Format(L"%.2f",pNodeBegin->m_fPrice);
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,2);
			pCell->SetTextClr(clr);
			pCell->SetCompareFloat(pNodeBegin->m_fPrice);
			pCell->SetText(StrText);

			// 清仓时间
			if ( -1 != pNodeEnd->m_TimeAction.GetTime())
			{
				StrText.Format(L"%02d/%02d/%02d/ %02d:%02d:%02d",
					pNodeEnd->m_TimeAction.GetYear(),
					pNodeEnd->m_TimeAction.GetMonth(),
					pNodeEnd->m_TimeAction.GetDay(),
					pNodeEnd->m_TimeAction.GetHour(),
					pNodeEnd->m_TimeAction.GetMinute(),
					pNodeEnd->m_TimeAction.GetSecond());					
			}
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,3);
			pCell->SetTextClr(clr);
			pCell->SetText(StrText);
			
			// 清仓价格
			StrText.Format(L"%.2f",pNodeEnd->m_fPrice);
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,4);
			pCell->SetTextClr(clr);
			pCell->SetCompareFloat(pNodeEnd->m_fPrice);
			pCell->SetText(StrText);
			
			// 留仓时间:
			CTimeSpan TimeSpan = pNodeEnd->m_TimeAction - pNodeBegin->m_TimeAction;

			StrText.Format(L"%d天%d时%d分%d秒",				
				TimeSpan.GetDays(),
				TimeSpan.GetHours(),
				TimeSpan.GetMinutes(),
				TimeSpan.GetSeconds());	
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,5);
			pCell->SetTextClr(clr);
			pCell->SetText(StrText);

			// 盈利价格差
			StrText.Format(L"%.2f",fPriceChange);
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,6);
			pCell->SetTextClr(clr);
			pCell->SetCompareFloat(fPriceChange);
			pCell->SetText(StrText);

			// 盈利率
		
			float fCmp = 0.0;

			if ( pNodeBegin->m_fPrice != 0)
			{
				fCmp = fPriceChange*100/pNodeBegin->m_fPrice;
				StrText.Format(L"%.2f",fCmp);
				
			}
			else
			{
				fCmp	= 0.0;
				StrText = L"-";
			}
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,7);
			pCell->SetTextClr(clr);
			pCell->SetCompareFloat(fCmp);
			pCell->SetText(StrText);
		}	
		
		// 后面加上新的统计对应的值, 与以前做对比:
		int32 iRowCounts = m_GridCtrl.GetRowCount();
		m_GridCtrl.InsertRow(L"111");
		
		
		// 序号
		CString StrText;
		StrText.Format(L"%d",iRowCounts);
		CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCounts,0);
		pCell->SetTextClr(RGB(255,255,0));
		pCell->SetCompareFloat(iRowCounts);
		pCell->SetText(StrText);	
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCounts,1);
		pCell->SetTextClr(RGB(255,255,0));
		pCell->SetText(L"=======");						
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCounts,2);
		pCell->SetTextClr(RGB(255,255,0));
		pCell->SetText(L"新统计数据:");						
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCounts,3);
		pCell->SetTextClr(RGB(255,255,0));
		pCell->SetText(L"=======");						
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCounts,4);
		pCell->SetTextClr(RGB(255,255,0));
		pCell->SetText(L"=======");						
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCounts,5);
		pCell->SetTextClr(RGB(255,255,0));
		pCell->SetText(L"=======");						
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCounts,6);
		pCell->SetTextClr(RGB(255,255,0));
		pCell->SetText(L"=======");	
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCounts,7);
		pCell->SetTextClr(RGB(255,255,0));
		pCell->SetText(L"=======");	

		for ( i = m_iBegin ; i < m_iEnd; i++)
		{
			T_TrendIndexNode* pNodeBegin = NULL; 
			T_TrendIndexNode* pNodeEnd = NULL;
			
			if ( EAPDTOC == m_aTrendIndexNode.GetAt(i).m_eActionPrompt || EAPDTAC == m_aTrendIndexNode.GetAt(i).m_eActionPrompt || EAPKTOC == m_aTrendIndexNode.GetAt(i).m_eActionPrompt || EAPKTAC == m_aTrendIndexNode.GetAt(i).m_eActionPrompt)
			{
				// 开仓或者加仓为起点.找到下一个不是当前操作提示的.必然是减仓或者是清仓.做为一次完整的交易记录:
				
				pNodeBegin = &m_aTrendIndexNode.GetAt(i);
				
				for ( int32 j = i ; j < m_iEnd ; j++)
				{
					if ( j == m_iEnd - 1)
					{
						// 是最后一个节点了.就把这个当作是结束节点:
						pNodeEnd = &m_aTrendIndexNode.GetAt(j);	
						i = j;
						break;
					}
					else
					{
						if ( m_aTrendIndexNode.GetAt(j).m_eActionPrompt != pNodeBegin->m_eActionPrompt )
						{
							// 一次完整交易:
							pNodeEnd = &m_aTrendIndexNode.GetAt(j);	
							
							i = j;
							break;					
						}
					}										
				}
				
				if ( NULL == pNodeBegin || NULL == pNodeEnd)
				{
					continue;
				}
				
				// 赚钱还是亏钱了的颜色
				
				float fPriceChange = 0.0;
				
				COLORREF clr = RGB(200,200,200);
				
				if ( EAPDTOC == pNodeBegin->m_eActionPrompt || EAPDTAC == pNodeBegin->m_eActionPrompt)
				{
					// 多头开仓
					fPriceChange = pNodeEnd->m_fPrice - pNodeBegin->m_fPrice;
				}
				else if ( EAPKTOC == pNodeBegin->m_eActionPrompt || EAPKTAC == pNodeBegin->m_eActionPrompt)
				{
					// 空头开仓
					fPriceChange = pNodeBegin->m_fPrice - pNodeEnd->m_fPrice;	
				}
				
				if ( fPriceChange > 0.0 )
				{
					clr = RGB(255,0,0);
				}
				else if ( fPriceChange < 0.0  )
				{
					clr = RGB(0,255,0);
				}
				
				int32 iRowCount = m_GridCtrl.GetRowCount();
				m_GridCtrl.InsertRow(L"123");
				
				// 序号
				StrText.Format(L"%d",iRowCount);
				pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,0);
				pCell->SetTextClr(clr);
				pCell->SetCompareFloat(iRowCount);
				pCell->SetText(StrText);
				
				// 开仓时间				
				if ( -1 != pNodeBegin->m_TimeAction.GetTime())
				{
					StrText.Format(L"%02d/%02d/%02d/ %02d:%02d:%02d",
						pNodeBegin->m_TimeAction.GetYear(),
						pNodeBegin->m_TimeAction.GetMonth(),
						pNodeBegin->m_TimeAction.GetDay(),
						pNodeBegin->m_TimeAction.GetHour(),
						pNodeBegin->m_TimeAction.GetMinute(),
						pNodeBegin->m_TimeAction.GetSecond());					
				}
				pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,1);
				pCell->SetTextClr(clr);
				pCell->SetText(StrText);
				
				// 开仓价格
				StrText.Format(L"%.2f",pNodeBegin->m_fPrice);
				pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,2);
				pCell->SetTextClr(clr);
				pCell->SetCompareFloat(pNodeBegin->m_fPrice);
				pCell->SetText(StrText);
				
				// 清仓时间
				if ( -1 != pNodeEnd->m_TimeAction.GetTime())
				{
					StrText.Format(L"%02d/%02d/%02d/ %02d:%02d:%02d",
						pNodeEnd->m_TimeAction.GetYear(),
						pNodeEnd->m_TimeAction.GetMonth(),
						pNodeEnd->m_TimeAction.GetDay(),
						pNodeEnd->m_TimeAction.GetHour(),
						pNodeEnd->m_TimeAction.GetMinute(),
						pNodeEnd->m_TimeAction.GetSecond());					
				}
				pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,3);
				pCell->SetTextClr(clr);
				pCell->SetText(StrText);
				
				// 清仓价格
				StrText.Format(L"%.2f",pNodeEnd->m_fPrice);
				pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,4);
				pCell->SetTextClr(clr);
				pCell->SetCompareFloat(pNodeEnd->m_fPrice);
				pCell->SetText(StrText);
				
				// 留仓时间:
				CTimeSpan TimeSpan = pNodeEnd->m_TimeAction - pNodeBegin->m_TimeAction;
				
				StrText.Format(L"%d天%d时%d分%d秒",				
					TimeSpan.GetDays(),
					TimeSpan.GetHours(),
					TimeSpan.GetMinutes(),
					TimeSpan.GetSeconds());	
				pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,5);
				pCell->SetTextClr(clr);
				pCell->SetText(StrText);
				
				// 盈利价格差
				StrText.Format(L"%.2f",fPriceChange);
				pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,6);
				pCell->SetTextClr(clr);
				pCell->SetCompareFloat(fPriceChange);
				pCell->SetText(StrText);
				
				// 盈利率
				
				float fCmp = 0.0;
				
				if ( pNodeBegin->m_fPrice != 0)
				{
					fCmp = fPriceChange*100/pNodeBegin->m_fPrice;
					StrText.Format(L"%.2f",fCmp);
					
				}
				else
				{
					fCmp	= 0.0;
					StrText = L"-";
				}
				pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCount,7);
				pCell->SetTextClr(clr);
				pCell->SetCompareFloat(fCmp);
				pCell->SetText(StrText);
			}
		}
	}
	else if ( 2 == m_iHeadIndex)
	{
		int32 iNewSize = m_iEnd-m_iBegin;
		CArray<T_TrendIndexNode,T_TrendIndexNode> aTemp;
		aTemp.SetSize(iNewSize);

		char* p = (char*)m_aTrendIndexNode.GetData();
		p += sizeof(T_TrendIndexNode)*m_iBegin;

		char* p2 = (char*)aTemp.GetData();
		memcpyex(p2,p,sizeof(T_TrendIndexNode)*(iNewSize));
		
		int32 iTradeTimes,iGoodTimes,iBadTimes;
		float fAccuracyRate,fProfitability;

		int32 aaa;
		bool32 bOK = StatisticaTrendValues(1, aaa, aTemp,iTradeTimes,iGoodTimes,iBadTimes,fAccuracyRate,fProfitability);

		if ( bOK )
		{
			// 往表里加数据:
			int32 iRow = m_GridCtrl.GetRowCount();
			m_GridCtrl.InsertRow(L"123");

			CString StrText;
			StrText.Format(L"%d",iTradeTimes);
			CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRow,0);
			pCell->SetText(StrText);

			StrText.Format(L"%d",iGoodTimes);			
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRow,1);
			pCell->SetText(StrText);

			StrText.Format(L"%d",iBadTimes);
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRow,2);
			pCell->SetText(StrText);

			StrText.Format(L"%.2f",fAccuracyRate*100);
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRow,3);
			pCell->SetText(StrText);

			StrText.Format(L"%.2f",fProfitability*100);
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRow,4);
			pCell->SetText(StrText);
		}

		int32 iRowCounts = m_GridCtrl.GetRowCount();
		m_GridCtrl.InsertRow(L"123");

		CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCounts,0);
		pCell->SetTextClr(RGB(255,255,0));
		pCell->SetText(L"新");
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCounts,1);
		pCell->SetTextClr(RGB(255,255,0));
		pCell->SetText(L"的");
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCounts,2);
		pCell->SetTextClr(RGB(255,255,0));
		pCell->SetText(L"统");
		pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRowCounts,3);
		pCell->SetTextClr(RGB(255,255,0));
		pCell->SetText(L"计");
		
		bOK = StatisticaTrendValues2(aTemp,iTradeTimes,iGoodTimes,iBadTimes,fAccuracyRate,fProfitability);
		
		if ( bOK )
		{
			// 往表里加数据:
			int32 iRow = m_GridCtrl.GetRowCount();
			m_GridCtrl.InsertRow(L"123");
			
			CString StrText;
			StrText.Format(L"%d",iTradeTimes);
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRow,0);
			pCell->SetText(StrText);
			
			StrText.Format(L"%d",iGoodTimes);			
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRow,1);
			pCell->SetText(StrText);
			
			StrText.Format(L"%d",iBadTimes);
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRow,2);
			pCell->SetText(StrText);
			
			StrText.Format(L"%.2f",fAccuracyRate*100);
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRow,3);
			pCell->SetText(StrText);
			
			StrText.Format(L"%.2f",fProfitability*100);
			pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRow,4);
			pCell->SetText(StrText);
		}
	}
}
void CDlgTrendIndexstatistics::OnButtonStart() 
{
	m_bStrart = false;

	UpdateData(true);
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
	
	if ( 0 == m_iRadio)
	{
		m_iBegin = m_iBeginInitial;
		m_iEnd	 = m_iEndInitial;		
	}
	else if ( 1 == m_iRadio)
	{
		m_iBegin = 0;
		m_iEnd	 = m_aTrendIndexNode.GetSize();
	}
	else if ( 2 == m_iRadio)
	{
		m_iBegin = -1;
		m_iEnd   = -1;

		// 用户自己设的时间
		CTime TimeBegin(m_TimeBgYMD.GetYear(),m_TimeBgYMD.GetMonth(),m_TimeBgYMD.GetDay(),m_TimeBgHMS.GetHour(),m_TimeBgHMS.GetMinute(),m_TimeBgHMS.GetSecond());
		CTime TimeEnd(m_TimeEdYMD.GetYear(),m_TimeEdYMD.GetMonth(),m_TimeEdYMD.GetDay(),m_TimeEdHMS.GetHour(),m_TimeEdHMS.GetMinute(),m_TimeEdHMS.GetSecond());

		CString StrB;
		StrB.Format(L"%02d/%02d/%02d/ %02d:%02d:%02d",
			TimeBegin.GetYear(),
			TimeBegin.GetMonth(),
			TimeBegin.GetDay(),
			TimeBegin.GetHour(),
			TimeBegin.GetMinute(),
			TimeBegin.GetSecond());
		
		CString StrE;
		StrE.Format(L"%02d/%02d/%02d/ %02d:%02d:%02d",
			TimeEnd.GetYear(),
			TimeEnd.GetMonth(),
			TimeEnd.GetDay(),
			TimeEnd.GetHour(),
			TimeEnd.GetMinute(), 
			TimeEnd.GetSecond());

		for ( int32 i = 0 ; i < m_aTrendIndexNode.GetSize(); i++)
		{
			if ( -1 == m_aTrendIndexNode.GetAt(i).m_TimeAction.GetTime())
			{
				continue;
			}

			CString StrNode;
			StrNode.Format(L"%02d/%02d/%02d/ %02d:%02d:%02d",
				m_aTrendIndexNode.GetAt(i).m_TimeAction.GetYear(),
				m_aTrendIndexNode.GetAt(i).m_TimeAction.GetMonth(),
				m_aTrendIndexNode.GetAt(i).m_TimeAction.GetDay(),
				m_aTrendIndexNode.GetAt(i).m_TimeAction.GetHour(),
				m_aTrendIndexNode.GetAt(i).m_TimeAction.GetMinute(), 
				m_aTrendIndexNode.GetAt(i).m_TimeAction.GetSecond());

			if ( TimeBegin.GetTime() == m_aTrendIndexNode.GetAt(i).m_TimeAction.GetTime())
			{
				// 正好完全相等
				m_iBegin = i+1;
				continue;
			}
			else
			{
				// 世界上的事情总不会那么的完美

				if ( (i + 1) < m_aTrendIndexNode.GetSize())
				{
					// 如果是大于这个,小于下一个,那么就认为下一个个是 起点

					if (    TimeBegin.GetTime() > m_aTrendIndexNode.GetAt(i).m_TimeAction.GetTime()
						&& TimeBegin.GetTime() < m_aTrendIndexNode.GetAt(i+1).m_TimeAction.GetTime())
					{
						m_iBegin = i+1;
						continue;
					}
				}
			}
			
			if ( TimeEnd.GetTime() == m_aTrendIndexNode.GetAt(i).m_TimeAction.GetTime())
			{
				m_iEnd = i+1;
				continue;
			}
			else
			{
				// 世界上的事情总不会那么的完美
				
				if ( (i + 1) < m_aTrendIndexNode.GetSize())
				{
					// 如果是大于这个,小于下一个,那么就认为这个是 起点( 但是后面计数器遍历的时候,是 < 号,取不到这个节点,所以要再加1)
					
					if (   TimeEnd.GetTime() > m_aTrendIndexNode.GetAt(i).m_TimeAction.GetTime()
						&& TimeEnd.GetTime() < m_aTrendIndexNode.GetAt(i+1).m_TimeAction.GetTime())
					{
						m_iEnd = i+1;
						continue;
					}
				}
			}
		}

		// 开始时间比第一根还早,结束时间比最后一个还晚.那么取第一个和最后一个:
		CTime Time0 = m_aTrendIndexNode.GetAt(0).m_TimeAction.GetTime();
		CTime Time1 = m_aTrendIndexNode.GetAt( m_aTrendIndexNode.GetSize() - 1 ).m_TimeAction.GetTime();
		
		if ( TimeBegin.GetTime() <= Time0.GetTime() )
		{
			m_iBegin = 0;
		}

		if ( TimeEnd.GetTime() >= Time1.GetTime())
		{
			m_iEnd = m_aTrendIndexNode.GetSize();
		}
	}

	UpdateTable();

	m_GridCtrl.UpdateWindow();
	m_GridCtrl.Invalidate();
	m_GridCtrl.Refresh();

	GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);

	m_bStrart = true;
}

void CDlgTrendIndexstatistics::OnRadio0() 
{
	// 总的统计
	if ( m_bStrart)
	{
		SetTableHead(0);
		UpdateTable();
	}
}

void CDlgTrendIndexstatistics::OnRadio1() 
{
	// 统计1
	if ( m_bStrart)
	{
		SetTableHead(1);
		UpdateTable();
	}
}

void CDlgTrendIndexstatistics::OnRadio2() 
{
	// 统计2
	if ( m_bStrart)
	{
		SetTableHead(2);		
		UpdateTable();
	}
}

void CDlgTrendIndexstatistics::OnRadio3() 
{
	if ( m_bStrart)
	{
		SetTableHead(0);
		UpdateTable();
	}	
}

void CDlgTrendIndexstatistics::OnButtonConfirm() 
{
/*	UpdateData(true);
	SetTrendIndexParam(m_fStrong,m_fWeek,m_fIntensityOC,m_fStability,m_iMA5,m_iMA8,m_iMA13,m_fMA5QZ,m_fMA8QZ,m_fMA13QZ,m_uiCdt5KLineNums,m_fCdt5Times);	
	
	if ( m_bCdt2)
	{
		m_pParentIoView->m_bNeedCondition2 = true;
	}
	else
	{
		m_pParentIoView->m_bNeedCondition2 = false;
	}

	if ( m_bCdt3)
	{
		m_pParentIoView->m_bNeedCondition3 = true;
	}
	else
	{
		m_pParentIoView->m_bNeedCondition3 = false;
	}

	if ( m_bCdt4)
	{
		m_pParentIoView->m_bNeedCondition4 = true;
	}
	else
	{
		m_pParentIoView->m_bNeedCondition4 = false;
	}

	if ( m_bCdt5)
	{
		m_pParentIoView->m_bNeedCondition5 = true;
	}
	else
	{
		m_pParentIoView->m_bNeedCondition5 = false;
	}
	
	m_pParentIoView->CalcHistoryTrendIndex();
	m_pParentIoView->OnConditionOpenClose();
*/
	OnClose();
}

void CDlgTrendIndexstatistics::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	delete this;
}

void CDlgTrendIndexstatistics::OnClose()
{
	if (m_pParentIoView != NULL)
	{
		m_pParentIoView->m_pDlgTrendIndex = NULL;	
	}

	DestroyWindow();
}


