// DlgTrace.cpp : implementation file
//
#include "stdafx.h"
#include "DlgTrace.h"
#include "ShareFun.h"
#include "SocketClient.h"
#include "coding.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTrace dialog

CDlgTrace::CDlgTrace(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTrace::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTrace)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_iRowIndex = 0;

	m_bInitlized = false;
	m_bStop	   = false;
	m_pLogFile = NULL;
	
	if ( 0 == _taccess(L"c:\\ggtonglog.txt", 0) )
	{
		CFile::Remove(L"c:\\ggtonglog.txt");	
	}
	
 	// m_pLogFile = fopen("c:\\ggtonglog.txt", "aw+");
}

CDlgTrace::~CDlgTrace()
{
	if ( NULL != m_pLogFile )
	{
		fclose(m_pLogFile);
	}
}

void CDlgTrace::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTrace)
	DDX_Control(pDX, IDOK, m_BtnOk);
	DDX_Control(pDX, IDC_CUSTOM_TRACE, m_GridTrace);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgTrace, CDialog)
	//{{AFX_MSG_MAP(CDlgTrace)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBtnSaveText)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	//}}AFX_MSG_MAP
	ON_MESSAGE(0x456, OnMsgAddTrace)
	ON_MESSAGE(0x457, OnMsgAddGoodTrace)
	ON_MESSAGE(0x458, OnMsgAddBadTrace)
	ON_MESSAGE(UM_MSGLOG, OnMsgLog)		
	ON_MESSAGE(0x460, OnMsgCheckTime)		
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTrace message handlers

void CDlgTrace::OnOK() 
{
	// TODO: Add extra validation here
	m_GridTrace.DeleteNonFixedRows();
	m_GridTrace.Invalidate();
		
//	CDialog::OnOK();
}

void CDlgTrace::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

BOOL CDlgTrace::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	ConstructGrid();
	OnSizeChanged();

	::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTrace::ConstructGrid()
{
	// Get the font
	CFont* pFont = m_GridTrace.GetFont();
	LOGFONT lf;
	pFont->GetLogFont(&lf);
	memcpyex(lf.lfFaceName, _T("Arial"), 6);
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	m_GridTrace.GetDefaultCell(TRUE, FALSE)->SetFont(&lf);
	m_GridTrace.GetDefaultCell(FALSE, TRUE)->SetFont(&lf);
	m_GridTrace.GetDefaultCell(TRUE, TRUE)->SetFont(&lf);
	m_GridTrace.GetDefaultCell(FALSE, FALSE)->SetFont(&lf);

	m_GridTrace.GetDefaultCell(TRUE, FALSE)->SetMargin(0);
	m_GridTrace.GetDefaultCell(FALSE, TRUE)->SetMargin(0);
	m_GridTrace.GetDefaultCell(TRUE, TRUE)->SetMargin(0);
	m_GridTrace.GetDefaultCell(FALSE, FALSE)->SetMargin(0);

	m_GridTrace.SetEditable(false);

	m_GridTrace.SetFixedRowCount(1);
	m_GridTrace.SetFixedColumnCount(1);

	m_GridTrace.SetColumnCount(3);
	CGridCellBase *pCell = NULL;
	pCell = m_GridTrace.GetCell(0, 0);
	pCell->SetText(L"序号");

	pCell = m_GridTrace.GetCell(0, 1);
	pCell->SetText(L"发生时间(ms)");

	pCell = m_GridTrace.GetCell(0, 2);
	pCell->SetText(L"事件描述");

	// 设置表格图标
	// m_ImageList.Create(MAKEINTRESOURCE(IDB_GRID_REPORT), 16, 1, RGB(255,255,255));
	// m_GridTrace.SetImageList(&m_ImageList);

	// 创建滚动条
	m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10203);
 	m_XSBVert.SetScrollRange(0, 10);

	m_XSBHorz.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10204);
 	m_XSBHorz.SetScrollRange(0, 0);
	m_XSBHorz.ShowWindow(SW_HIDE);

	// 设置相互之间的关联
	m_XSBHorz.SetOwner(&m_GridTrace);
	m_XSBVert.SetOwner(&m_GridTrace);
	m_GridTrace.SetScrollBar(&m_XSBHorz, &m_XSBVert);

	m_bInitlized = true;
}

void CDlgTrace::OnSizeChanged()
{
	if (m_bInitlized)
	{
		CRect rect;
		GetClientRect(rect);

		CRect RectBtnOk;
		m_BtnOk.GetClientRect(RectBtnOk);
		int iBtnHeight = RectBtnOk.Height();
		int iBtnWidth = (rect.Width()-2)/3;
		
		CRect RectSB(rect);
		CRect RectGridCtrl(rect);
		RectBtnOk = rect;
		
		RectSB.bottom -= iBtnHeight;
		RectGridCtrl.bottom -= iBtnHeight;	
		RectBtnOk.top = RectBtnOk.bottom - iBtnHeight;
		RectBtnOk.right = RectBtnOk.left + iBtnWidth;

		//
		RectSB.left = RectSB.right - (m_XSBVert.GetFitHorW());
		RectGridCtrl.right = RectSB.left;


		//
		m_BtnOk.MoveWindow(RectBtnOk);

		CWnd *pBtnSave = GetDlgItem(IDC_BUTTON_SAVE);
		if ( NULL != pBtnSave )
		{
			CRect rcBtnSave = RectBtnOk;
			rcBtnSave.left = RectBtnOk.right+2;
			rcBtnSave.right = rcBtnSave.left + RectBtnOk.Width();
			pBtnSave->MoveWindow(rcBtnSave);
		}

		CWnd *pBtnStop = GetDlgItem(IDC_BUTTON_STOP);
		if ( NULL != pBtnStop )
		{
			CRect rcBtnStop = RectBtnOk;
			rcBtnStop.right = rect.right;
			rcBtnStop.left  = rcBtnStop.right - RectBtnOk.Width();
			pBtnStop->MoveWindow(rcBtnStop);
		}

		//
		m_XSBVert.SetSBRect(RectSB, TRUE);

		//
		m_GridTrace.MoveWindow(RectGridCtrl);

		int iWidth = RectGridCtrl.Width();
		int iWidth0 = 30;
		int iWidth1 = 125;
		int iWidth2 = iWidth - iWidth0 - iWidth1;

		m_GridTrace.ExpandColumnsToFit();
		m_GridTrace.SetColumnWidth(0, iWidth0);
		m_GridTrace.SetColumnWidth(1, iWidth1);
		m_GridTrace.SetColumnWidth(2, iWidth2);
	}
}

LRESULT CDlgTrace::OnMsgAddTrace(WPARAM wParam, LPARAM lParam)
{
	UINT uiTime = UINT(wParam);
	CString *pString = (CString *)lParam;
	CString StrTrace = *pString;
	DEL(pString);
	
	AddTrace(0, uiTime, StrTrace);

	return 0;
}

LRESULT CDlgTrace::OnMsgAddGoodTrace(WPARAM wParam, LPARAM lParam)
{
	UINT uiTime = UINT(wParam);
	CString *pString = (CString *)lParam;
	CString StrTrace = *pString;
	DEL(pString);
	
	AddTrace(1, uiTime, StrTrace);

	return 0;
}

LRESULT CDlgTrace::OnMsgAddBadTrace(WPARAM wParam, LPARAM lParam)
{
	UINT uiTime = UINT(wParam);
	CString *pString = (CString *)lParam;
	CString StrTrace = *pString;
	DEL(pString);
	
	AddTrace(-1, uiTime, StrTrace);

	return 0;
}

// fangz0301
LRESULT CDlgTrace::OnMsgLog(WPARAM wParam, LPARAM lParam)
{
	CString *pString = (CString *)lParam;
	CString StrTrace = *pString;
	DEL(pString);

 	if (NULL != m_pLogFile)
 	{
		string sTrace = _Unicode2MultiChar(StrTrace);	
		const char* strLog = sTrace.c_str();
 
 		fprintf(m_pLogFile, "%s \n", strLog);
 		fflush(m_pLogFile);
 	}
	
	return 0;
}

LRESULT CDlgTrace::OnMsgCheckTime(WPARAM wParam, LPARAM lParam)
{
	//UINT uiTime = UINT(wParam);
	CString *pString = (CString *)lParam;
	CString StrTrace = *pString;
	DEL(pString);
	
// 	if ( -1 != StrTrace.Find(L"==>]"))
// 	{
// 		AddTrace(-1, uiTime, StrTrace);
// 	}
// 	else
// 	{
// 		AddTrace(0, uiTime, StrTrace);
// 	}
	
	return 0;
}

UINT uiTimeBefore = MAXUINT;
bool32 bClear = false;

void CDlgTrace::AddTrace(int iTraceType, UINT uiTime, const CString &StrTrace)
{
	if ( m_bStop )
	{
		return;
	}

	if (m_GridTrace.GetRowCount() == 1)
	{
		m_GridTrace.InsertRow(L"");
	}
	else
	{
		m_GridTrace.InsertRow(L"", 1);
	}

	CGridCellBase *pCell0 = m_GridTrace.GetCell(1, 0);
	CString StrIndex;
	StrIndex.Format(L"%d", m_iRowIndex++);
	pCell0->SetText(StrIndex);

	CGridCellBase *pCell1 = m_GridTrace.GetCell(1, 1);
	UINT uiTimePrev = uiTime;
	if (m_GridTrace.GetRowCount() > 2)
	{
		CGridCellBase *pCellPrev = m_GridTrace.GetCell(2, 1);
		CString StrTimePrev = pCellPrev->GetText();
		uiTimePrev = _wtoi(StrTimePrev);
	}
	
	CString StrTime;
	StrTime.Format(L"%d + %d", uiTime, uiTime - uiTimePrev);
	
	if ( -1 != StrTrace.Find(L"赋值") )
	{
		uiTimeBefore = uiTime;
		StrTime += L" [B]";
	}

	if ( (StrTrace.Find(L"解包成功")!=-1) && (uiTimeBefore != MAXUINT) )
	{
		CString StrTimeElse;
		StrTimeElse.Format(L"[E %d]", uiTime - uiTimeBefore);
		
		StrTime += StrTimeElse;
		uiTimeBefore = MAXUINT;
	}

	pCell1->SetText(StrTime);

	CGridCellBase *pCell2 = m_GridTrace.GetCell(1, 2);
	pCell2->SetText(StrTrace);
	
	//
	if (0 == iTraceType)
	{
		pCell0->SetBackClr(0xffffff);
		pCell1->SetBackClr(0xffffff);
		pCell2->SetBackClr(0xffffff);
	}
	else if (iTraceType > 0)
	{
		pCell0->SetBackClr(0x00ff00);
		pCell1->SetBackClr(0x00ff00);
		pCell2->SetBackClr(0x00ff00);
	}
	else
	{
		pCell0->SetTextClr(0x0000ff);
		pCell1->SetTextClr(0x0000ff);
		pCell2->SetTextClr(0x0000ff);
	}

	
 	int iRowCount = m_GridTrace.GetRowCount();
 	if (iRowCount > 2001)
 	{ 		
 		for (int i = 2001; i < iRowCount; i++)
 		{
 			m_GridTrace.DeleteRow(2001);	
 		}	
 	}

	m_GridTrace.Invalidate();
	m_GridTrace.Refresh();
}

void CDlgTrace::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here

	OnSizeChanged();
}

int CDlgTrace::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	return 0;
}

void CDlgTrace::OnBtnSaveText()
{
	// 另存文本 && 打开它
	if ( m_GridTrace.m_hWnd != NULL )
	{
		// 新来的总是在前面, 文件按照log文件一般的前面的是以前的记录，新来的记录在后面存入
		const int32 iRowCount = m_GridTrace.GetRowCount();
		const int32 iColCount = m_GridTrace.GetColumnCount();
		if ( iRowCount - m_GridTrace.GetFixedRowCount() > 0
			&& iColCount > 0 )
		{
			CStdioFile file;
			const CString StrFileName(_T("traceSave.txt"));
			UINT uFlag = CFile::modeWrite |CFile::modeCreate |CFile::typeText;
			if ( _taccess(StrFileName, 0) != 0 
				|| (MessageBox(_T("是否清除以前文件的内容？"), _T("提示"), MB_YESNO |MB_ICONQUESTION |MB_DEFBUTTON2) != IDYES )
				)
			{
				// 不存在该文件 或者 不需要截断
				uFlag |= CFile::modeNoTruncate;
			}
			CFileException expFile;
			if ( !file.Open(StrFileName, uFlag, &expFile) )
			{
				expFile.ReportError();
				return;
			}

			file.SeekToEnd();

			CString StrRow;
			char szBuf[1000];
			for ( int32 iRow= iRowCount-1; iRow >= m_GridTrace.GetFixedRowCount() ; iRow-- )
			{
				StrRow.Empty();
				for ( int32 iCol=0; iCol < iColCount ; iCol++ )
				{
					StrRow += m_GridTrace.GetItemText(iRow, iCol) + _T("\t");
				}
				StrRow += _T("\n");

				// 转成mbcs保存
				int iLen = WideCharToMultiByte(CP_ACP, 0, StrRow, -1, szBuf, sizeof(szBuf), NULL, NULL);
				if ( iLen > 0 )
				{
					file.Write(szBuf, iLen-1);
				}
			}

			file.Close();

			ShellExecute(NULL, _T("open"), StrFileName, NULL, NULL, SW_SHOW);
		}
	}
}

void CDlgTrace::OnButtonStop() 
{
	m_bStop = !m_bStop;
	
	//
	CWnd* pWnd = GetDlgItem(IDC_BUTTON_STOP);
	if ( NULL == pWnd )
	{
		return;
	}

	//
	if ( m_bStop )
	{	
		pWnd->SetWindowText(L"继续");
	}	
	else
	{
		pWnd->SetWindowText(L"暂停");
	}
}
