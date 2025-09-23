// DialogExportSaleData.cpp : 实现文件
//
#include "stdafx.h"
#include "excel.h"
#include "comdef.h"
#include "atlbase.h"
#include "sharestruct.h"
#include "PathFactory.h"
#include "GridCellSymbol.h"
#include "FontFactory.h"
#include "coding.h"
#include "DialogExportSaleData.h"
using namespace excel;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CTRL_ID_GRID		10000
#define CTRL_ID_XB_VERT		10001
#define CTRL_ID_XB_HORZ		10002
#define CTRL_BUTTOUN		10003

#define TEXT_COLOR_RED			 RGB(255, 0, 0)
#define TEXT_COLOR_GRE			 RGB(0, 128, 0)
#define GRID_BK_COLOR			 RGB(255,255,255)
#define GRID_TEXT_COLOR			 RGB(77,77,77)
#define GRID_HEAD_COLOR		RGB(0xF3, 0xF1, 0xEB)	// 表头们的颜色
#define GRID_BORDER_COLOR	RGB(0xE6, 0xE6, 0xE6)

const byte Title_Height = 30;

const CString s_ExcelRowName[] =
{ 
	_T("A"), _T("B"), _T("C"), _T("D"), _T("E"), _T("F"), _T("G"), _T("H"), _T("I"), _T("J"), _T("K"), 
	_T("L"), _T("M"), _T("N"), _T("O"), _T("P"), _T("Q"), _T("R"), _T("S"), _T("T"), _T("U"), _T("V"),
	_T("W"), _T("X"), _T("Y"), _T("Z"),
	"AA", "AB", "AC", "AD", "AE", "AF", "AG", "AH", "AI", "AJ", "AK", "AL", "AM", "AN", "AO", "AP", "AQ",
	"AR", "AS", "AT", "AU", "AV", "AW", "AX", "AY", "AZ"
};

DWORD WINAPI ThreadProc (PVOID pParam);
/////////////////////////////////////////////////////////////////////////////
// CDialogExportSaleData 对话框

IMPLEMENT_DYNAMIC(CDialogExportSaleData, CDialogEx)

// CDialogExportSaleData dialog
CDialogExportSaleData::CDialogExportSaleData(CWnd* pParent /*=NULL*/) : CDialogEx(CDialogExportSaleData::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogExportSaleData)
	m_pAbsCenterManager = NULL;
	m_pGridCtrl = NULL;
	m_eListenType = ECTReqMerchTimeSales;
	//}}AFX_DATA_INIT
}

//lint --e{1540}
CDialogExportSaleData::~CDialogExportSaleData()
{
	
}

void CDialogExportSaleData::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogExportSaleData)
	DDX_Control(pDX, IDC_DATETIMEPICKER_BEGIN, m_dateBegin);
	DDX_Control(pDX, IDC_DATETIMEPICKER_END, m_dataEnd);
	DDX_Control(pDX, IDC_STATIC_PROCESS, m_staticProcessInfo);
	DDX_Control(pDX, IDC_BUTTON_EXPORT, m_buttonExport);
	DDX_Control(pDX, IDC_BUTTON_SEARCH, m_buutonSearch);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDialogExportSaleData, CDialogEx)
	//{{AFX_MSG_MAP(CDialogExportSaleData)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, OnButtonSearch)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, OnButtonExport)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_BEGIN, OnDatetimechangeDatetimepickerBegin)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_END, OnDatetimechangeDatetimepickerEnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CDialogExportSaleData message handlers

BOOL CDialogExportSaleData::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	CGGTongDoc *pDoc = AfxGetDocument();
	
	if ( NULL != pDoc && NULL != (m_pAbsCenterManager = pDoc->m_pAbsCenterManager) )
	{
		m_pAbsCenterManager->AddViewDataListner(this);
	}
	else
	{
		ASSERT( 0 );
	}

	SetWindowText(L"分笔数据导出");
	CTime cBeginTime = CTime::GetCurrentTime() - CTimeSpan(0, 24, 0, 0);
	m_dateBegin.SetTime(&cBeginTime);
	
	// 创建横纵滚动条
	m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), CTRL_ID_XB_VERT);
	m_XSBVert.SetScrollRange(0, 10);
	
	m_XSBHorz.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), CTRL_ID_XB_HORZ);
	m_XSBHorz.SetScrollRange(0, 10);
	
	// 创建数据表格
	m_pGridCtrl = new CGridCtrlNormal;
	m_pGridCtrl->Create(CRect(0, 0, 0, 0), this, CTRL_ID_GRID);
	m_pGridCtrl->SetListMode(TRUE);
	m_pGridCtrl->EnableToolTips(FALSE);
	m_pGridCtrl->EnableTitleTips(FALSE);
	m_pGridCtrl->SetSingleRowSelection(TRUE);
	m_pGridCtrl->SetGridBkColor(GRID_BK_COLOR);
	m_pGridCtrl->SetFixedBkColor(GRID_HEAD_COLOR);
	m_pGridCtrl->SetListHeaderCanClick(FALSE);
	m_pGridCtrl->GetDefaultCell(FALSE, FALSE)->SetBackClr(GRID_BK_COLOR);
	m_pGridCtrl->GetDefaultCell(FALSE, FALSE)->SetTextClr(GRID_TEXT_COLOR);

	// 设置表头
	m_pGridCtrl->SetHeaderSort(FALSE);
	//////////////////////////////////
	m_pGridCtrl->SetFixedRowCount(1);
	m_pGridCtrl->SetColumnCount(3);
	m_pGridCtrl->GetDefaultCell(FALSE, FALSE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	// 设置相互之间的关联
	m_XSBHorz.SetOwner(m_pGridCtrl);
	m_XSBVert.SetOwner(m_pGridCtrl);
	m_pGridCtrl->SetScrollBar(&m_XSBHorz, &m_XSBVert);
	// 显示表格
	CRect rectGrid, rectClient, rectButton, rcStatic;
	GetClientRect(rectClient);
	((CButton *)GetDlgItem(IDC_BUTTON_SEARCH))->GetWindowRect(rectButton);
	ScreenToClient(rectButton);

	const byte gap = 3;
	CWnd *pStatic = GetDlgItem(IDC_STATIC_PROCESS);
	pStatic->GetWindowRect(rcStatic);
	int nHeightStatic = rcStatic.Height();
	rcStatic.left = gap;
	rcStatic.right = rectClient.right - gap;
	rcStatic.bottom = rectClient.bottom - gap;
	rcStatic.top = rcStatic.bottom - nHeightStatic;
	pStatic->MoveWindow(rcStatic);
	rectGrid.left = gap;
	rectGrid.right = rectClient.right - gap;
	rectGrid.top = rectButton.bottom + gap;
	rectGrid.bottom = rcStatic.top - gap;
	m_pGridCtrl->MoveWindow(rectGrid);
	// 初始化表头显示
	TCHAR szTitle[][4] = {L"时间", L"价格", L"现手"};
	for(int i = 0; i < 3; i++)
	{

		CGridCellBase *pCell = m_pGridCtrl->GetCell(0, i);
		if(NULL == pCell)
		{
			//ASSERT(0);
			return FALSE;
		}
		pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		pCell->SetText(szTitle[i]);
		m_pGridCtrl->SetColumnWidth(i, rectGrid.Width() / 3);
	}
	m_pGridCtrl->AutoSizeRow(0);
	//m_pGridCtrl->SetRowHeight(0, 25);
	
	m_pGridCtrl->ShowWindow(SW_SHOW);
	
	return TRUE;
}

void CDialogExportSaleData::OnDestroy()
{
	// 清除一些数据
	CAbsCenterManager *pAbsCenterManager = m_pAbsCenterManager;
	if ( NULL != pAbsCenterManager)
	{
		pAbsCenterManager->DelViewDataListner(this);
	}
	else
	{
		ASSERT( 0 );
	}

	if(m_pGridCtrl)
	{
		delete m_pGridCtrl;
		m_pGridCtrl = NULL;
	}
	CDialogEx::OnDestroy();
}

void CDialogExportSaleData::OnButtonSearch() 
{
	CTime timeStart;
	m_dateBegin.GetTime(timeStart);	
	CTime timeEnd;
	m_dataEnd.GetTime(timeEnd);
	CTime timeTmp = timeEnd - CTimeSpan(0,24,0,0);

	if (timeStart > timeEnd)
	{
		AfxMessageBox(L"起始时间大于结束时间");
		return;
	}

	if (timeStart < timeTmp)
	{
		AfxMessageBox(L"超过最大时间间隔24小时");
		return;
	}

	m_timeStart = timeStart;
	m_timeEnd = timeEnd;

	CMmiReqMerchTimeSales info;
	info.m_eReqTimeType	= ERTYSpecifyTime;
	info.m_TimeEnd = CGmtTime(timeEnd.GetTime());
	info.m_TimeStart = CGmtTime(timeStart.GetTime());
	info.m_iMarketId	= m_MerchInfo.m_iMarketId;
	info.m_StrMerchCode = m_MerchInfo.m_StrMerchCode;

	if (m_pAbsCenterManager != NULL)
	{
		m_pAbsCenterManager->RequestViewData(&info);
	}
}

void CDialogExportSaleData::OnButtonExport() 
{
	if(m_pGridCtrl && m_pGridCtrl->GetRowCount() > 1)
	{
		HANDLE handle = CreateThread(NULL, 0, ThreadProc, this, 0, NULL); 
		DEL_HANDLE(handle);
		return;
	}

	AfxMessageBox(L"请先查看数据！");
}

void CDialogExportSaleData::SetMerchInfo(CMerchInfo& mrechInfo)
{
	m_MerchInfo.m_iMarketId = mrechInfo.m_iMarketId;
	m_MerchInfo.m_StrMerchCode = mrechInfo.m_StrMerchCode;
	m_MerchInfo.m_StrMerchCnName = mrechInfo.m_StrMerchCnName;
}

void CDialogExportSaleData::OnDataRespMerchTimeSales(int iMmiReqId, IN const CMmiRespMerchTimeSales *pMmiRespMerchTimeSales)
{
	if(!pMmiRespMerchTimeSales || NULL == m_pGridCtrl || NULL == m_pAbsCenterManager )
	{
		return ;
	}


	CMerch* pMerch = NULL;
	if(!m_pAbsCenterManager->GetMerchManager().FindMerch(pMmiRespMerchTimeSales->m_MerchTimeSales.m_StrMerchCode, pMmiRespMerchTimeSales->m_MerchTimeSales.m_iMarketId, pMerch))
	{
		return ;
	}

	CString strTemp;
	CTick *pTickShow = (CTick *)pMerch->m_pMerchTimeSales->m_Ticks.GetData();
	int iSize = pMerch->m_pMerchTimeSales->m_Ticks.GetSize();
	m_pGridCtrl->DeleteNonFixedRows();
	m_pGridCtrl->SetRowCount(1 + iSize);
	int iCountActual = 0;
	static float lastPrice = 0.0f;
	for ( int32 i = 1; i < m_pGridCtrl->GetRowCount(); i ++ )
	{
		CTime cCurrentTime(pTickShow[i - 1].m_TimeCurrent.m_Time.GetTime());
		if(cCurrentTime < m_timeStart || cCurrentTime > m_timeEnd)
		{
			continue;
		}
		strTemp.Format(L"%d/%02d/%02d-%02d:%02d:%02d",
					   cCurrentTime.GetYear(), cCurrentTime.GetMonth(), cCurrentTime.GetDay(), cCurrentTime.GetHour(), cCurrentTime.GetMinute(), cCurrentTime.GetSecond());
		CGridCellBase *pFirstCell	= m_pGridCtrl->GetCell(1+iCountActual, 0);
		CGridCellBase *pSecondCell	= m_pGridCtrl->GetCell(1+iCountActual, 1);
		CGridCellBase *pThirdCell	= m_pGridCtrl->GetCell(1+iCountActual, 2);
		ASSERT(NULL != pFirstCell);	
		ASSERT(NULL != pSecondCell);
		pFirstCell->SetText(strTemp);
		strTemp.Format(L"%g", pTickShow[i - 1].m_fPrice);
		pSecondCell->SetText(strTemp);
		if(iCountActual == 0)
		{
			lastPrice = pTickShow[i - 1].m_fPrice;
			if(pTickShow[i - 1].m_fPrice > m_pAbsCenterManager->GetClosePrice())
			{
				pSecondCell->SetTextClr(RGB(0, 230, 0));
			}
			else
			{
				pSecondCell->SetTextClr(RGB(255, 50, 50));
			}
			// 用第一条数据的时间作为文件名
			m_time = cCurrentTime;
		}
		else
		{
			if(pTickShow[i - 1].m_fPrice > lastPrice)
			{
				pSecondCell->SetTextClr(RGB(255, 50, 50));
			}
			else
			{
				pSecondCell->SetTextClr(RGB(0, 230, 0));
			}
			lastPrice = pTickShow[i - 1].m_fPrice;
		}
		strTemp.Format(L"%d", int(pTickShow[i - 1].m_fVolume + 0.5));
		if(strTemp == L"0")
		{
			strTemp = L"-";
		}
		pThirdCell->SetText(strTemp);
		iCountActual++;
	}
	if(iCountActual != iSize)
	{
		m_pGridCtrl->SetRowCount(1 + iCountActual);
	}
}

BOOL CDialogExportSaleData::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->wParam == VK_RETURN)
	{
		return true;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

DWORD WINAPI ThreadProc (PVOID pParam)
{
	CDialogExportSaleData *pWnd = (CDialogExportSaleData*)pParam;
	pWnd->ExportGridToExcel(pWnd->m_pGridCtrl);

	return 0;
}

void CDialogExportSaleData::OnClose()
{
	if(m_buttonExport.IsWindowEnabled())
	{
		CDialogEx::OnClose();
	}
	else
	{
		AfxMessageBox(_T("正在导出数据到EXCEL中."));
		return;
	}
}

void CDialogExportSaleData::ExportGridToExcel(CGridCtrlNormal* pGridCtrl)
{
	_Application ExcelApp;
	Workbooks workBooks;
	_Workbook workBook;
	Worksheets workSheets;
	_Worksheet workSheet;
	Range CurrRange;

	if (CoInitialize(NULL) != 0)
	{    
		AfxMessageBox(L"初始化COM支持库失败!");     
		return;
	}

	if (!ExcelApp.CreateDispatch(_T("Excel.Application"), NULL))
	{		
		AfxMessageBox(_T("启动Excel程序失败."));
		return;
	}
	
	CString StrFileName;
	CTime cCurrentTime = m_time;
	CString StrTime;
	StrTime.Format(_T("%02d年%d月%d日%d时%d分%d秒"),cCurrentTime.GetYear(),cCurrentTime.GetMonth(),cCurrentTime.GetDay(),cCurrentTime.GetHour(),cCurrentTime.GetMinute(),cCurrentTime.GetSecond());

	StrFileName.Format(_T("%s(%s)%s.xls"), m_MerchInfo.m_StrMerchCnName.GetBuffer(), m_MerchInfo.m_StrMerchCode.GetBuffer(), StrTime.GetBuffer());

	//文件名及路径选择
	CFileDialog dlg(FALSE,_T("xls"),StrFileName,NULL,_T("Microsoft Office Excel工作簿(*.xls)|*.xls||"),NULL);

	if(GetVersion() < 0x80000000)
	{
		dlg.m_ofn.lStructSize=88;
	}
	else
	{
		dlg.m_ofn.lStructSize=76;
	}
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	CString StrFilePath = CPathFactory::GetPrivateExcelPath(pDoc->m_pAbsCenterManager->GetUserName());	
	StrFilePath += L".xls";
	
	// 得到Excel 文件夹路径
	TCHAR TStrFilePath[MAX_PATH];
	lstrcpy(TStrFilePath, StrFilePath);
	_tcheck_if_mkdir(TStrFilePath);

	// 备份当前路径
	TCHAR TStrFilePathBk[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH-1, TStrFilePathBk);

	// 设置为Excel 路径
	::SetCurrentDirectory(TStrFilePath);
	dlg.m_ofn.lpstrInitialDir = TStrFilePath;

	int32 ret = dlg.DoModal();

	if(ret!=IDOK)
	{
		return;
	}
	
	// 路径设回去
	::SetCurrentDirectory(TStrFilePathBk);
	StrFileName=dlg.GetPathName();

	//设置为显示
	//ExcelApp.SetVisible(TRUE);
	TRY
	{
		workBooks.AttachDispatch(ExcelApp.GetWorkbooks());
		workBook.AttachDispatch(workBooks.Add(_variant_t(vtMissing)));
		workSheets=workBook.GetSheets();
		
		workSheet=workSheets.GetItem(COleVariant((short)1));
		//	workSheet.Activate();
		
		//得到全部Cells，此时,CurrRange是cells的集合
		CurrRange.AttachDispatch(workSheet.GetCells());
		
		/*
		设置单元格文本为自动换行 及排列方式
		range.AttachDispatch(sheet.GetUsedRange());//加载已使用的单元格
		range.SetWrapText(COleVariant((long)1));//设置单元格内的文本为自动换行
		//设置齐方式为水平垂直居中
		//水平对齐：默认＝1,居中＝-4108,左＝-4131,右＝-4152
		//垂直对齐：默认＝2,居中＝-4108,左＝-4160,右＝-4107
		range.SetHorizontalAlignment(COleVariant((long)-4108));
		range.SetVerticalAlignment(COleVariant((long)-4108));
		*/
		Range Col0(CurrRange.GetRange(_variant_t(_T("A1")),_variant_t(_T("A1"))));
		Col0.SetColumnWidth(_variant_t((long)22));
		Col0.SetNumberFormat(COleVariant(L"@")); //将单元格设置为文本类型
		Col0.SetHorizontalAlignment(COleVariant((long)-4108));
		Col0.SetVerticalAlignment(COleVariant((long)-4108));

		Range Col12(CurrRange.GetRange(_variant_t(_T("B1")),_variant_t(_T("C1"))));
		Col12.SetHorizontalAlignment(COleVariant((long)-4152));
		Col12.SetVerticalAlignment(COleVariant((long)-4107));

		if ( pGridCtrl->GetRowCount() > 0 )
		{
			m_buttonExport.EnableWindow(FALSE);
			m_buutonSearch.EnableWindow(FALSE);
			CString strProcessInfo;
			for (int i = 0; i < pGridCtrl->GetRowCount(); i ++ )
			{
				for ( int32 j = 0; j < pGridCtrl->GetColumnCount(); j ++ )
				{
					if (j >= sizeof(s_ExcelRowName)/sizeof(s_ExcelRowName[0]))
					{
						break;	// 列太多了，无法继续
					}
					CGridCellBase* pCell = pGridCtrl->GetCell(i,j);
					CString StrText = pCell->GetText();
					COLORREF clr = pCell->GetTextClr();
					CurrRange.SetItem(_variant_t((long)(i + 1)),_variant_t((long)(j+1)),_variant_t(StrText));
					if(j == 1)
					{
						CString StrCell;
						StrCell.Format(_T("%s%d"),CString(s_ExcelRowName[j]).GetBuffer(),i + 1);
						Range cell(CurrRange.GetRange(_variant_t(StrCell),_variant_t(StrCell)));
						FontExecl font(cell.GetFont());
						font.SetColor(_variant_t((long)clr));
					}
				}
				strProcessInfo.Format(L"正在导出数据到EXECL(%d/%d)", i + 1, pGridCtrl->GetRowCount());
				m_staticProcessInfo.SetWindowText(strProcessInfo);
			}
		}
	}
	CATCH_ALL (e)
	{
		// 异常，关闭，提示错误
		workBook.Close(_variant_t(true),_variant_t(StrFileName),_variant_t(false));
		workBooks.Close();
		
		ExcelApp.Quit();
		
		ExcelApp.ReleaseDispatch();
		workBooks.ReleaseDispatch();
		workBook.ReleaseDispatch();
		workSheets.ReleaseDispatch();
		workSheet.ReleaseDispatch();
		CurrRange.ReleaseDispatch();
		
		CString StrErr;
		if ( e->GetErrorMessage(StrErr.GetBuffer(1024), 1024) )
		{
		}
		StrErr.ReleaseBuffer();
		if ( StrErr.IsEmpty() )
		{
			StrErr = _T("非常抱歉！由于未知的原因导致excel导出数据失败");
		}
		m_staticProcessInfo.SetWindowText(StrErr);
		m_buttonExport.EnableWindow(TRUE);
		m_buutonSearch.EnableWindow(TRUE);

		return;
	}
	END_CATCH_ALL

	
	workBook.Close(_variant_t(true),_variant_t(StrFileName),_variant_t(false));
    workBooks.Close();
	
    ExcelApp.Quit();

	ExcelApp.ReleaseDispatch();
	workBooks.ReleaseDispatch();
	workBook.ReleaseDispatch();
	workSheets.ReleaseDispatch();
	workSheet.ReleaseDispatch();
	CurrRange.ReleaseDispatch();
	m_staticProcessInfo.SetWindowText(_T("导出成功."));
	
	m_buttonExport.EnableWindow(TRUE);
	m_buutonSearch.EnableWindow(TRUE);
}

void CDialogExportSaleData::OnDatetimechangeDatetimepickerBegin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CTime cBeginTime;
	m_dateBegin.GetTime(cBeginTime);
	cBeginTime += CTimeSpan(0, 24, 0, 0);
	m_dataEnd.SetTime(&cBeginTime);
	
	*pResult = 0;
}

void CDialogExportSaleData::OnDatetimechangeDatetimepickerEnd(NMHDR* pNMHDR, LRESULT* pResult)
{
	CTime cEndTime;
	m_dataEnd.GetTime(cEndTime);
	cEndTime -= CTimeSpan(0, 24, 0, 0);
	m_dateBegin.SetTime(&cEndTime);
	
	*pResult = 0;
}