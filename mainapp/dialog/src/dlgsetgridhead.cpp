// dlgsetgridhead.cpp : implementation file
//

#include "stdafx.h" 
#include "ReportScheme.h"
#include "dlgsetgridhead.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSetGridHead dialog

CDlgSetGridHead::CDlgSetGridHead(CWnd* pParent /*=NULL*/)
 	: CDialogEx(CDlgSetGridHead::IDD, pParent)
{
	m_iTabSelLast = 0;
	m_Callback.m_pParent = NULL;
}

void CDlgSetGridHead::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSetGridHead)
	DDX_Control(pDX, IDC_TAB_GRIDHEAD, m_TabSheet);
	//}}AFX_DATA_MAP
}
BEGIN_MESSAGE_MAP(CDlgSetGridHead, CDialogEx)
	//{{AFX_MSG_MAP(CDlgSetGridHead)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_GRIDHEAD, OnSelchangeTabGridhead)
	ON_BN_CLICKED(IDC_BUTTON_CONFIRM, OnButtonConfirm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSetGridHead message handlers

void CTabSetGridHeadCallback::PreCreate(CString StrTitle,int32 i)
{
	m_pParent->m_DlgSetMerch[i].m_StrTitle = StrTitle;
}
BOOL CDlgSetGridHead::OnInitDialog() 
{
	CDialog::OnInitDialog();

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,FALSE);

	m_Callback.m_pParent = this;
	m_TabSheet.AddPreCreate(&m_Callback);
	
	int32 i = 0;
	for (i = 0; i< ERTCount; i++)
	{
		E_ReportType  eReportType = (E_ReportType)(ERTFuturesCn+i);
		if ( ERTNone == eReportType )
		{
			continue;
		}
		m_TabSheet.AddPage(CReportScheme::Instance()->GetReportTypeCnName(eReportType),& m_DlgSetMerch[i],IDD_DIALOG_MERCH);
	}	
	m_TabSheet.Show();

	// 先创建出来,再赋值,否则会有Bug 	

	for ( i = 0; i< ERTCount; i++)
	{
		int32 iFixCol;		
		E_ReportType  eReportType = (E_ReportType)(ERTFuturesCn+i);
		CReportScheme::Instance()->GetReportHeadInfoList(eReportType,m_aHeadInfoList,iFixCol);
		m_DlgSetMerch[i].GetOwnHeadInfo(m_aHeadInfoList,iFixCol);		
	}	
	m_DlgSetMerch[0].m_CtrlListExist.SetFocus();
	m_DlgSetMerch[0].Show();
	return TRUE;  
}
void CDlgSetGridHead::OnSelchangeTabGridhead(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	int32 iCur = m_TabSheet.GetCurSel();
	E_ReportType  eReportType = (E_ReportType)(ERTFuturesCn+iCur);
	if ( ERTNone <= eReportType )
	{
		iCur += 1;	// 需要跳过那个空的report
	}
	m_TabSheet.SetCurSel(iCur);	
	m_DlgSetMerch[iCur].m_CtrlListExist.SetFocus();
	//m_DlgSetMerch[iCur].m_CtrlListExist.SetCurSel(1);
	m_DlgSetMerch[iCur].Show(eReportType);	
	
	*pResult = 0;
}
void CDlgSetGridHead::OnButtonConfirm() 
{
	// 最新的数据,用于覆盖 CReportScheme 的成员数据,同时保存Xml

	int32 i = 0;
	for ( i = 0; i<ERTCount; i++)
	{
		E_ReportType  eReportType = (E_ReportType)(ERTFuturesCn+i);

		if ( ERTNone == eReportType )
		{
			continue;
		}
		
		m_DlgSetMerch[i].UpdateData(true);
		int32 iFixCol = m_DlgSetMerch[i].m_iFixCol;
		int32 iSize   = m_DlgSetMerch[i].m_aHeadInfoList.GetSize();
		
		CArray<T_HeadInfo,T_HeadInfo> aHeaderList;
		
		for (int32 j=0; j < iSize; j++)
		{
			T_HeadInfo HeadInfo = m_DlgSetMerch[i].m_aHeadInfoList[j]; 
			aHeaderList.Add(HeadInfo);
			aHeaderList[j].m_StrHeadNameCn = CReportScheme::Instance()->HeadSummaryName2CnName(m_DlgSetMerch[i].m_aHeadInfoList[j].m_StrHeadNameCn);
			aHeaderList[j].m_iHeadWidth	= m_DlgSetMerch[i].m_aHeadInfoList[j].m_iHeadWidth;
			aHeaderList[j].m_eReportHeader	= CReportScheme::Instance()->GetReportHeaderEType(aHeaderList[j].m_StrHeadNameCn);
		}
		
		ASSERT(aHeaderList.GetSize()>0);
		ASSERT(iFixCol>=0 && iFixCol<=GHCOUNTS);

		CReportScheme::Instance()->ChangeHeadInfoData(eReportType,aHeaderList,iFixCol);							
	}

	// 通知表头发生了变化
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(NULL != pMainFrame);
	
	for ( i = 0; i<ERTCount; i++)
	{
		if ( ERTNone == (E_ReportType)i )
		{
			continue;
		}

		// 如果栏目发生了改变了才发消息
		if (m_DlgSetMerch[i].BeModify())
		{
			E_ReportType  eReportType = (E_ReportType)(ERTFuturesCn+i);		
			pMainFrame->PostMessage(UM_GridHeader_Change, (WPARAM)eReportType, 0);
		}
	}

	CReportScheme::Instance()->SaveDataToXml();
}

void CDlgSetGridHead::OnOK() 
{
	OnButtonConfirm();		
	CDialog::OnOK();
} 
