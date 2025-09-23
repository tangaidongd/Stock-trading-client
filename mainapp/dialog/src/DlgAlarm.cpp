// DlgAlarm.cpp : implementation file
//
#include "stdafx.h"
#include "DlgAlarm.h"
#include "GridCellSys.h"
#include "GridCellSymbol.h"
#include "dlgalarmsetting.h"
#include "FontFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define  GRID_BOTTOM_DISTANCE      48

/////////////////////////////////////////////////////////////////////////////
// CDlgAlarm dialog
CDlgAlarm::CDlgAlarm(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgAlarm::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAlarm)
	m_bCheck = FALSE;
	m_bPromptCheck = FALSE;
	m_pAlarmCenter = NULL;
	m_pAbsCenterManager = NULL;
	//}}AFX_DATA_INIT
	
	CGGTongDoc * pDoc = (CGGTongDoc *)AfxGetDocument();
	if (NULL != pDoc)
	{
		m_pAbsCenterManager = pDoc->m_pAbsCenterManager;
		m_pAlarmCenter = pDoc->m_pAarmCneter;
		m_pAlarmCenter->AddNotify(this);
	}

	SetNeedChangeColor(true);
	SetCaptionBKColor(RGB(250, 250, 246));
	SetFrameColor(RGB(102, 102, 102));
	SetCaptionColor(RGB(10, 0, 4));
}

CDlgAlarm::~CDlgAlarm()
{
	if ( NULL != m_pAlarmCenter )
	{
		m_pAlarmCenter->DelNotify(this);
	}
}

void CDlgAlarm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAlarm)
	DDX_Check(pDX, IDC_CHECK_ONOFF, m_bCheck);
	DDX_Check(pDX, IDC_CHECK_PROMPT, m_bPromptCheck);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgAlarm, CDialogEx)
	//{{AFX_MSG_MAP(CDlgAlarm)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_SET, OnButtonSet)
	ON_BN_CLICKED(IDC_CHECK_ONOFF, OnCheckOnoff)
	ON_BN_CLICKED(IDC_CHECK_PROMPT, OnCheckPrompt)
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, 54321, OnGridDblClick)			
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnButtonClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAlarm message handlers
BOOL CDlgAlarm::PreTranslateMessage(MSG* pMsg)
{
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CDlgAlarm::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	delete this;
}

void CDlgAlarm::OnSize(UINT nType, int cx, int cy)
{
	CRect rect;
	GetClientRect(&rect);
	rect.bottom -= GRID_BOTTOM_DISTANCE;
	if (m_GridCtrl.GetSafeHwnd())
	{
		m_GridCtrl.AutoSize();
		m_GridCtrl.MoveWindow(&rect);
		m_GridCtrl.Invalidate();
	}

	CRect rtCheckOnff, rtCheckPrompt, rtSet, rtClear;
	int iControlHeight = 20;     // 以上四个控件的高度
	int iCheckWidth = 75;
	int iBtnWidth = 60;

	if(rect.Height() > 0 && m_GridCtrl.GetSafeHwnd())
	{
		rtCheckOnff.top = rect.bottom + (GRID_BOTTOM_DISTANCE - iControlHeight) / 2;
		rtCheckOnff.bottom = rtCheckOnff.top + iControlHeight;
		rtCheckOnff.left = rect.left + 22;
		rtCheckOnff.right = rtCheckOnff.left + iCheckWidth;
		GetDlgItem(IDC_CHECK_ONOFF)->MoveWindow(rtCheckOnff);

		rtCheckPrompt = rtCheckOnff;
		rtCheckPrompt.left = rtCheckOnff.right + 25;
		rtCheckPrompt.right = rtCheckPrompt.left + iCheckWidth;
		GetDlgItem(IDC_CHECK_PROMPT)->MoveWindow(rtCheckPrompt);

		rtClear = rtCheckOnff;
		rtClear.right = rect.right - 15;
		rtClear.left = rtClear.right - iBtnWidth;
		GetDlgItem(IDC_BUTTON_CLEAR)->MoveWindow(rtClear);

		rtSet = rtCheckOnff;
		rtSet.right = rtClear.left - 10;
		rtSet.left = rtSet.right - iBtnWidth;
		GetDlgItem(IDC_BUTTON_SET)->MoveWindow(rtSet);

	}

	CDialogEx::OnSize(nType,cx,cy);
}

BOOL CDlgAlarm::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	CenterWindow();
	ConstructGrid();
	SetCheckFlag();

	UpdateData(FALSE);
	return TRUE; 
}

void CDlgAlarm::OnClose()
{
//	if ( m_bCheck )
//	{
		ShowWindow(SW_HIDE);
//	}
// 	else
// 	{
// 		CMainFrame* pWnd  = (CMainFrame*)AfxGetMainWnd();
// 		pWnd->m_pDlgAlarm = NULL;
// 				
// 		DestroyWindow();		
// 	}
}

void CDlgAlarm::OnCheckOnoff() 
{
	UpdateData(true);

	if (m_pAlarmCenter != NULL)
	{
		m_pAlarmCenter->SetAlarmFlag(m_bCheck);
	}
}

void CDlgAlarm::OnButtonSet() 
{
	CGGTongDoc* pDoc = AfxGetDocument();
	if ( NULL == pDoc )
	{
		return;
	}


	CAlarmCenter* pCenter = pDoc->m_pAarmCneter;
	if ( NULL == pCenter )
	{
		return;
	}
		
	CDlgAlarmSetting Dlg;
	Dlg.SetAlarmCenter(pCenter);
	Dlg.DoModal();
	

}

bool32 CDlgAlarm::ConstructGrid()
{
	CRect rect;
	GetClientRect(&rect);
	rect.bottom -= GRID_BOTTOM_DISTANCE;

	m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10203);
	m_XSBVert.SetScrollRange(0, 10);
	m_XSBVert.ShowWindow(SW_SHOW);
	
	m_XSBHorz.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10204);
	m_XSBHorz.SetScrollRange(0, 0);
	m_XSBHorz.ShowWindow(SW_SHOW);
		
	if (NULL == m_GridCtrl.GetSafeHwnd())
	{
		if (!m_GridCtrl.Create(CRect(0, 0, 0, 0), this, 54321))
			return false;
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(255,0,0));
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

		LOGFONT* pLogFt = CFaceScheme::Instance()->GetSysFont(ESFSmall);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pLogFt);
	}

	// 清空表格内容
	m_GridCtrl.DeleteAllItems();
	m_GridCtrl.SetNeedShowFixSpliter(FALSE);
	
	// 设置列数
	if (!m_GridCtrl.SetColumnCount(5))
		return false;
	
	//  设置行数
	if (!m_GridCtrl.SetFixedRowCount(1))	
		return false;
	
	// 设置相互之间的关联			
	m_XSBHorz.SetOwner(&m_GridCtrl);
	m_XSBVert.SetOwner(&m_GridCtrl);
	m_GridCtrl.SetScrollBar(&m_XSBHorz, &m_XSBVert);						

	//CFont* pFont = CFaceScheme::Instance()->GetSysFontObject(ESFSmall);

	CString strFontName = gFontFactory.GetExistFontName(L"宋体");

	LOGFONT lgFont = {0};
	lgFont.lfHeight =12;
	lgFont.lfWidth = 0;
	lgFont.lfEscapement = 0;
	lgFont.lfOrientation = 0;
	lgFont.lfWeight =  FW_NORMAL;
	lgFont.lfItalic = 0;
	lgFont.lfUnderline = 0;
	lgFont.lfStrikeOut = 0;
	lgFont.lfCharSet = ANSI_CHARSET;
	lgFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lgFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lgFont.lfQuality = PROOF_QUALITY;
	lgFont.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
	_tcscpy(lgFont.lfFaceName, strFontName);///

	CFont* pFont = CFaceScheme::Instance()->GetSysFontObject(ESFSmall);
	m_GridCtrl.SetFont(pFont);

	CGridCellSys *	pCell =(CGridCellSys *)m_GridCtrl.GetCell(0, 0);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 			
	pCell->SetText(L"时间");
	pCell->SetTextClr(RGB(240, 240, 240));
	pCell->SetFont(&lgFont);
	
	pCell =(CGridCellSys *)m_GridCtrl.GetCell(0, 1);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 			
	pCell->SetText(L"名称");
	pCell->SetTextClr(RGB(240, 240, 240));
	pCell->SetFont(&lgFont);
	
	pCell =(CGridCellSys *)m_GridCtrl.GetCell(0, 2);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 			
	pCell->SetText(L"价格");
	pCell->SetTextClr(RGB(240, 240, 240));
	pCell->SetFont(&lgFont);
	
	pCell =(CGridCellSys *)m_GridCtrl.GetCell(0, 3);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 			
	pCell->SetText(L"成交量");
	pCell->SetTextClr(RGB(240, 240, 240));
	pCell->SetFont(&lgFont);

	pCell =(CGridCellSys *)m_GridCtrl.GetCell(0, 4);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 			
	pCell->SetText(L"预警条件");
	pCell->SetTextClr(RGB(240, 240, 240));
	pCell->SetFont(&lgFont);

	m_GridCtrl.MoveWindow(&rect);

	CRect rtCheckOnff, rtCheckPrompt, rtSet, rtClear;
	int iControlHeight = 20;     // 以上四个控件的高度
	int iCheckWidth = 75;
	int iBtnWidth = 60;

	if(rect.Height() > 0 && m_GridCtrl.GetSafeHwnd())
	{
		rtCheckOnff.top = rect.bottom + (GRID_BOTTOM_DISTANCE - iControlHeight) / 2;
		rtCheckOnff.bottom = rtCheckOnff.top + iControlHeight;
		rtCheckOnff.left = rect.left + 22;
		rtCheckOnff.right = rtCheckOnff.left + iCheckWidth;
		GetDlgItem(IDC_CHECK_ONOFF)->MoveWindow(rtCheckOnff);

		rtCheckPrompt = rtCheckOnff;
		rtCheckPrompt.left = rtCheckOnff.right + 25;
		rtCheckPrompt.right = rtCheckPrompt.left + iCheckWidth;
		GetDlgItem(IDC_CHECK_PROMPT)->MoveWindow(rtCheckPrompt);

		rtClear = rtCheckOnff;
		rtClear.right = rect.right - 15;
		rtClear.left = rtClear.right - iBtnWidth;
		GetDlgItem(IDC_BUTTON_CLEAR)->MoveWindow(rtClear);

		rtSet = rtCheckOnff;
		rtSet.right = rtClear.left - 10;
		rtSet.left = rtSet.right - iBtnWidth;
		GetDlgItem(IDC_BUTTON_SET)->MoveWindow(rtSet);

	}


	//m_GridCtrl.ExpandColumnsToFit();
	SetColWidthAccordingFont();
	m_GridCtrl.AutoSizeRows();
	m_GridCtrl.Invalidate();

	return true;
}

CString CDlgAlarm::GetTimeString(/*const CGmtTime& stTime*/)
{
	if ( NULL == m_pAbsCenterManager )
	{
		return L"";
	}

	CGmtTime stTime = m_pAbsCenterManager->GetServerTime();

	//	
	if ( stTime.GetTime() <= 0 )
	{
		return L"";
	}
	
	//
	CString StrTime;
	StrTime.Format(L"%02d:%02d", stTime.GetHour() + 8, stTime.GetMinute());
	
	return StrTime;
}

void CDlgAlarm::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	
	int32 iRow = pItem->iRow;
	if ( 0 == iRow )	
		return;

	if ( NULL == m_pAbsCenterManager )
	{
		return;
	}

	CGridCellBase* pCell = m_GridCtrl.GetCell(iRow, 0);
	if ( NULL == pCell )
	{
		return;
	}

	int32 iFlag = BeArbitrageData(iRow);
	if ( -1 == iFlag )
	{
		//ASSERT(0);
		return;
	}

	//
	if ( 1 == iFlag )
	{
		CMerch* pMerch = (CMerch*)pCell->GetData();
		if ( NULL == pMerch )
		{
			return;
		}
		
		//
		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		if ( NULL!=pMainFrame )
		{
			pMainFrame->OnShowMerchInChart(pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, ID_PIC_TREND);
		}
	}
	else if ( 2 == iFlag )
	{
		CArbitrage* pArbitrage = (CArbitrage*)pCell->GetData();
		if ( NULL == pArbitrage )
		{
			return;
		}

		//
		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		if ( NULL!=pMainFrame )
		{
			pMainFrame->OnArbitrageF5(*pArbitrage);
		}
	}	
}

HBRUSH CDlgAlarm::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_STATIC)
	{	
		pDC->SetTextColor(RGB(255, 0, 0));
	}

	return hbr;
}

void CDlgAlarm::OnCheckPrompt()
{
	UpdateData(TRUE);
	if ( NULL != m_pAlarmCenter )
	{
		m_pAlarmCenter->SetPromptFlag(!m_bPromptCheck);
	}
}

//////////////////////////////////////////////////////////////////////////
void CDlgAlarm::OnAlarmsChanged(void* pData, CAlarmNotify::E_AlarmNotify eNotify)
{
	if ( eNotify >= CAlarmNotify::EANCount || NULL == m_pAlarmCenter )
	{
		//ASSERT(0);
		return;
	}

	//
	if ( CAlarmNotify::EANDelMerch == eNotify )
	{
		if ( NULL == pData )
		{
			//ASSERT(0);
			return;				
		}

		//
		CMerch* pMerchSrc = (CMerch*)pData;

		// 删除某商品
		for ( int32 i = m_GridCtrl.GetRowCount() - 1; i >= 1; i-- )
		{
			CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(i, 0);
			if ( NULL == pCell )
			{
				continue;
			}

			CMerch* pMerchCell = (CMerch*)pCell->GetData();
			if ( NULL == pMerchCell )
			{
				continue;
			}

			// 删掉这个商品对应的表格项
			if ( pMerchCell == pMerchSrc )
			{
				m_GridCtrl.DeleteRow(i);
			}
		}
	}
	else if ( CAlarmNotify::EANDelArbitrage == eNotify )
	{
		if ( NULL == pData )
		{
			////ASSERT(0);
			return;				
		}
		
		//
		CArbitrage* pArbitrageSrc = (CArbitrage*)pData;
		
		// 删除某商品
		for ( int32 i = m_GridCtrl.GetRowCount() - 1; i >= 1; i-- )
		{
			CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(i, 0);
			if ( NULL == pCell )
			{
				continue;
			}
			
			CArbitrage* pArbitrageCell = (CArbitrage*)pCell->GetData();
			if ( NULL == pArbitrageCell )
			{
				continue;
			}
			
			// 删掉这个商品对应的表格项
			if ( pArbitrageCell == pArbitrageSrc )
			{
				m_GridCtrl.DeleteRow(i);
			}
		}
	}
	else if ( CAlarmNotify::EANDelAllMerch == eNotify )
	{
		// 删除所有商品
		m_GridCtrl.DeleteNonFixedRows();
	}	
	else if ( CAlarmNotify::EANAlarmOn == eNotify || CAlarmNotify::EANAlarmOff == eNotify )
	{
		// 警报打开或关闭
	}
	else if ( CAlarmNotify::EANPromptOn == eNotify || CAlarmNotify::EANPromptOff == eNotify )
	{
		// 提示打开或关闭
	}

	//
	m_GridCtrl.Refresh();
	m_GridCtrl.RedrawWindow();
}

void CDlgAlarm::Alarm(CMerch* pMerch, const CString& StrPrompt)
{
	if ( NULL == pMerch || NULL == pMerch->m_pRealtimePrice )
	{
		//ASSERT(0);
		return;
	}

	// 先在表格里插一行
	int32 iRow = m_GridCtrl.GetRowCount();
	m_GridCtrl.InsertRow(L"");

	CString strFontName = gFontFactory.GetExistFontName(L"宋体");

	LOGFONT lgFont = {0};
	lgFont.lfHeight =14;
	lgFont.lfWidth = 0;
	lgFont.lfEscapement = 0;
	lgFont.lfOrientation = 0;
	lgFont.lfWeight =  FW_NORMAL;
	lgFont.lfItalic = 0;
	lgFont.lfUnderline = 0;
	lgFont.lfStrikeOut = 0;
	lgFont.lfCharSet = ANSI_CHARSET;
	lgFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lgFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lgFont.lfQuality = PROOF_QUALITY;
	lgFont.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
	_tcscpy(lgFont.lfFaceName, strFontName);///

	// 时间
	CGridCellSys* pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
	CString StrText = GetTimeString();
	pCell->SetText(StrText);
	pCell->SetFont(&lgFont);
	
	// 设置自定义数据
	pCell->SetData((LPARAM)pMerch);

	// 名称:
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 1);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 			
	pCell->SetText(pMerch->m_MerchInfo.m_StrMerchCnName);
	pCell->SetFont(&lgFont);

	// 价格
	m_GridCtrl.SetCellType(iRow, 2, RUNTIME_CLASS(CGridCellSymbol));
	CGridCellSymbol * pCellSymbol = (CGridCellSymbol*)m_GridCtrl.GetCell(iRow, 2);
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);
	pCellSymbol->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 			
	CString StrPriceNew = Float2SymbolString(pMerch->m_pRealtimePrice->m_fPriceNew, pMerch->m_pRealtimePrice->m_fPricePrevClose, pMerch->m_MerchInfo.m_iSaveDec);
	pCellSymbol->SetText(StrPriceNew);
	pCell->SetFont(&lgFont);

	// 成交量
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 3);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
	StrText = Float2String(pMerch->m_pRealtimePrice->m_fVolumeCur, pMerch->m_MerchInfo.m_iSaveDec);
	pCell->SetText(StrText);
	pCell->SetFont(&lgFont);

	// 条件
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 4);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
	pCell->SetText(StrPrompt);
	pCell->SetFont(&lgFont);

	//
	m_GridCtrl.RedrawWindow();

	//
	ActAlarm();
}

void CDlgAlarm::AlarmArbitrage(CArbitrage* pArbitrage, const CString& StrPrompt)
{
	if ( NULL == pArbitrage )
	{
		//ASSERT(0);
		return;
	}
	
	// 先在表格里插一行
	int32 iRow = m_GridCtrl.GetRowCount();
	m_GridCtrl.InsertRow(L"");
	
	CString strFontName = gFontFactory.GetExistFontName(L"宋体");

	LOGFONT lgFont = {0};
	lgFont.lfHeight =14;
	lgFont.lfWidth = 0;
	lgFont.lfEscapement = 0;
	lgFont.lfOrientation = 0;
	lgFont.lfWeight =  FW_NORMAL;
	lgFont.lfItalic = 0;
	lgFont.lfUnderline = 0;
	lgFont.lfStrikeOut = 0;
	lgFont.lfCharSet = ANSI_CHARSET;
	lgFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lgFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lgFont.lfQuality = PROOF_QUALITY;
	lgFont.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
	_tcscpy(lgFont.lfFaceName, strFontName);///

	// 时间
	CGridCellSys* pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 0);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
	CString StrText = GetTimeString();
	pCell->SetText(StrText);
	pCell->SetFont(&lgFont);
	
	// 设置自定义数据
	pCell->SetData((LPARAM)pArbitrage);
	
	// 名称:
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 1);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 			
	pCell->SetText(pArbitrage->GetShowName());
	pCell->SetFont(&lgFont);
	
	// 价格
	m_GridCtrl.SetCellType(iRow, 2, RUNTIME_CLASS(CGridCellSymbol));
	CGridCellSymbol * pCellSymbol = (CGridCellSymbol*)m_GridCtrl.GetCell(iRow, 2);
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);
	pCellSymbol->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 			
	CString StrPriceNew = Float2SymbolString(pArbitrage->GetPrice(EAHPriceDifNew), 0, pArbitrage->m_iSaveDec);
	pCellSymbol->SetText(StrPriceNew);
	pCell->SetFont(&lgFont);
	
	// 成交量
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 3);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
	StrText = L"";
	pCell->SetText(StrText);
	pCell->SetFont(&lgFont);
	
	// 条件
	pCell = (CGridCellSys *)m_GridCtrl.GetCell(iRow, 4);
	pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 
	pCell->SetText(StrPrompt);
	pCell->SetFont(&lgFont);
	
	//
	m_GridCtrl.RedrawWindow();
	
	//
	ActAlarm();
}

void CDlgAlarm::ActAlarm()
{
	if (NULL == m_pAlarmCenter)
	{
		return;
	}

	if ( !m_pAlarmCenter->GetAlarmFlag() || !m_pAlarmCenter->GetPromptFlag() )
	{
		//
		return;
	}

	bool32 bShow = m_pAlarmCenter->GetAlarmDlgType();

	if ( bShow)
	{
		// 确保标志正确
		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		HWND hWnd = pMainFrame->GetSafeHwnd();
		
		CRect rectMain;
		pMainFrame->GetClientRect(rectMain);

		//
		HWND hForeWnd = ::GetForegroundWindow();
		DWORD dwForeID = ::GetWindowThreadProcessId(hForeWnd, NULL);
		DWORD dwCurID = ::GetCurrentThreadId();
		::AttachThreadInput(dwCurID, dwForeID, TRUE);
		::ShowWindow(hWnd, SW_SHOW);
		::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, rectMain.Width(), rectMain.Height(), SWP_NOMOVE | SWP_NOSIZE);
		::SetWindowPos(hWnd, HWND_NOTOPMOST,0, 0, rectMain.Width(), rectMain.Height(), SWP_NOMOVE| SWP_NOSIZE);
		::SetForegroundWindow(hWnd);
        ::AttachThreadInput(dwCurID, dwForeID, FALSE);

		//
		ShowWindow(SW_SHOW);	
		UpdateData(FALSE);
	}
}

void CDlgAlarm::SetCheckFlag()
{
	if ( NULL != m_pAlarmCenter )
	{
		m_bCheck = m_pAlarmCenter->GetAlarmFlag();
		m_bPromptCheck = !m_pAlarmCenter->GetPromptFlag();
	}

	UpdateData(FALSE);
}

int32 CDlgAlarm::BeArbitrageData(int32 iRow)
{
	if ( !m_GridCtrl.GetSafeHwnd() )
	{
		return -1;
	}

	//
	int32 iRowCounts = m_GridCtrl.GetRowCount();
	if ( iRow >= iRowCounts )
	{
		return -1;
	}

	//
	CGridCellBase* pCell = m_GridCtrl.GetCell(iRow, 1);
	if ( NULL == pCell )
	{
		return -1;
	}

	//
	CString StrText = pCell->GetText();

	//
	CArray<CArbitrage, CArbitrage&> aArbitrages;
	CArbitrageManage::Instance()->GetArbitrageDatas(aArbitrages);

	for ( int32 i = 0; i < aArbitrages.GetSize(); i++ )
	{
		CString StrArbitrage = aArbitrages.GetAt(i).GetShowName();

		//
		if ( 0 == StrText.CompareNoCase(StrArbitrage) )
		{
			return 2;
		}
	}

	return 1;
}

void CDlgAlarm::OnButtonClear() 
{
	m_GridCtrl.DeleteNonFixedRows();
	m_GridCtrl.Refresh();
	m_GridCtrl.RedrawWindow();
}

void CDlgAlarm::SetColWidthAccordingFont()
{
	int32 iColWidth = 105;
	m_GridCtrl.AutoSizeColumns();
	for ( int i=0; i < m_GridCtrl.GetColumnCount() ; i++ )	// 调整最小col
	{
		if (1 == i)
		{
			iColWidth = 180;
		}
		else if ( 0 == i || 2 == i || 3 == i)
		{
			iColWidth = 83;
		}
		else if( 4 == i)
		{
			iColWidth = 113;
		}
		else
		{
			iColWidth = 87;
		}

		m_GridCtrl.SetColumnWidth(i, iColWidth);
	}
}
