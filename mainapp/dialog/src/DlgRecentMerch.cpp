// DlgRecentMerch.cpp : implementation file
//
#include "stdafx.h"
#include "GridCellSys.h"
#include "GridCellSymbol.h"
#include "DlgRecentMerch.h"
#include "ShareFun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRecentMerch dialog

CDlgRecentMerch::CDlgRecentMerch(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgRecentMerch::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRecentMerch)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CDlgRecentMerch::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRecentMerch)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgRecentMerch, CDialogEx)
	//{{AFX_MSG_MAP(CDlgRecentMerch)
		// NOTE: the ClassWizard will add message map macros here
		ON_WM_CLOSE()
		ON_WM_SIZE()
		ON_NOTIFY(NM_DBLCLK, 54321, OnGridDblClick)
		ON_MESSAGE(UM_RecentMerch_Update,OnMsgRealtimeRecentMerchUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRecentMerch message handlers
BOOL CDlgRecentMerch::OnInitDialog()
{	
	SetPosition();
	CreateGrid();
	UpdateGrid();
	CDialog::OnInitDialog();
	return TRUE; 
}
void CDlgRecentMerch::OnOK()
{
	OnClose();
}
void CDlgRecentMerch::OnCancel()
{
	OnClose();
}
void CDlgRecentMerch::OnClose()
{
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	pWnd->m_pDlgRecentMerch = NULL;
	
	DestroyWindow();
}
void CDlgRecentMerch::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	delete this;	
}
void CDlgRecentMerch::OnSize(UINT nType, int cx, int cy)
{
	CRect rect;
	GetClientRect(&rect);
	if (m_GridCtrl.GetSafeHwnd())
	{
		m_GridCtrl.MoveWindow(&rect);
		m_GridCtrl.ExpandColumnsToFit();				
		m_GridCtrl.Invalidate();
	}	
	CDialogEx::OnSize(nType,cx,cy);	
}
BOOL CDlgRecentMerch::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
		{
			PostMessage(WM_CLOSE,0,0);
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
bool32 CDlgRecentMerch::CreateGrid()
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
		if (!m_GridCtrl.Create(CRect(0, 0, 0, 0), this, 54321))
			return false;
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	}
	// 清空表格内容
	m_GridCtrl.DeleteAllItems();
	
	// 设置列数
	if (!m_GridCtrl.SetColumnCount(4))
		return false;
	
	//  设置行数
	if (!m_GridCtrl.SetRowCount(11))	
		return false;
	
	// 设置表格图标
	// m_ImageList.Create(MAKEINTRESOURCE(IDB_NET_ONOFF), 16, 1, RGB(255,255,255));
	// m_GridCtrl.SetImageList(&m_ImageList);
	
	// 设置相互之间的关联			
	m_XSBHorz.SetOwner(&m_GridCtrl);
	m_XSBVert.SetOwner(&m_GridCtrl);
	m_GridCtrl.SetScrollBar(&m_XSBHorz, &m_XSBVert);						
	
	//
	
	CGridCellSys *	pCell =(CGridCellSys *)m_GridCtrl.GetCell(0, 0);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 			
	pCell->SetText(L"名称");

	pCell =(CGridCellSys *)m_GridCtrl.GetCell(0, 1);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 			
	pCell->SetText(L"最新价");
	
	pCell =(CGridCellSys *)m_GridCtrl.GetCell(0, 2);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 			
	pCell->SetText(L"昨收价");
	
	pCell =(CGridCellSys *)m_GridCtrl.GetCell(0, 3);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 			
	pCell->SetText(L"涨幅");
	
	//	
	m_GridCtrl.MoveWindow(&rect);
	m_GridCtrl.ExpandToFit();
	
	return true;
}

void CDlgRecentMerch::UpdateGrid()
{
	// 得到ViewData

	CGGTongDoc * pDoc = (CGGTongDoc *)AfxGetDocument();
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	if (!pAbsCenterManager)
	{
		return;
	}

	// 得到商品列表		
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();

	CArray<T_RecentMerch, T_RecentMerch&> aRecentMerchList;
	pMainFrame->GetRecentMerchList(aRecentMerchList);

	//////////////////////////////////////////////////////////////////////////
	// 发送实时数据请求
	
	CArray<CMerchKey, CMerchKey&> aMerchs;
	int32 i = 0;
	for ( i = 0; i < aRecentMerchList.GetSize(); i++)
	{
		CMerchKey MerchKey;
		MerchKey.m_StrMerchCode = aRecentMerchList[i].m_pMerch->m_MerchInfo.m_StrMerchCode;
		MerchKey.m_iMarketId	= aRecentMerchList[i].m_pMerch->m_MerchInfo.m_iMarketId;		
		aMerchs.Add(MerchKey);
	}
	
	if (aMerchs.GetSize() > 0)
	{
		CMmiReqRealtimePrice Req;
		Req.m_iMarketId		= aMerchs[0].m_iMarketId;
		Req.m_StrMerchCode	= aMerchs[0].m_StrMerchCode;
		
		aMerchs.RemoveAt(0, 1);
		Req.m_aMerchMore.Copy(aMerchs);
		
		pAbsCenterManager->RequestViewData((CMmiCommBase *)&Req);
	}

	//////////////////////////////////////////////////////////////////////////
	//
	for (i = 0 ; i < aRecentMerchList.GetSize(); i++)
	{
		T_RecentMerch &RM = aRecentMerchList[i];
		UpdateOneRow(i, RM.m_pMerch);		
	}	

	m_GridCtrl.Invalidate();
}

LRESULT CDlgRecentMerch::OnMsgRealtimeRecentMerchUpdate(WPARAM wParam, LPARAM lParam)
{
	CMerch *pMerch = (CMerch *)wParam;

	CArray<T_RecentMerch, T_RecentMerch&> aRecentMerchList;
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();	
	pMainFrame->GetRecentMerchList(aRecentMerchList);

	// 更新了实时价格数据以后,表格内容也要更新
	for ( int32 i = 0 ; i < aRecentMerchList.GetSize(); i++)
	{
		T_RecentMerch &RM = aRecentMerchList[i];
		if (RM.m_pMerch == pMerch)
		{
			UpdateOneRow(i, pMerch);
		}
	}
	m_GridCtrl.Invalidate();
	return TRUE;
}

void CDlgRecentMerch::UpdateOneRow(int32 iIndex, IN CMerch *pMerch)
{
	// 得到ViewData
	CGGTongDoc * pDoc = (CGGTongDoc *)AfxGetDocument();
	CAbsCenterManager  * pAbsCenterManager = NULL;
	if (NULL != pDoc)
	{
		pAbsCenterManager = pDoc->m_pAbsCenterManager;
		if ( NULL == pAbsCenterManager)
		{
			return;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	CString StrName,StrText;
	float fPriceNew,fPreClose,fRate;
	int32 iSaveDec;
	
	int32 iReturn = GetGridData(pAbsCenterManager, pMerch,StrName,fPriceNew,fPreClose,fRate,iSaveDec);
	if ( -1 == iReturn)
	{			
		fPriceNew = 0.0;
		fPreClose = 0.0;
		fRate = 0.0;
	}
	else if ( -2 == iReturn)
	{
		StrName = L"_";
		fPriceNew = 0.0;
		fPreClose = 0.0;
		fRate = 0.0;
	}
	
	CGridCellSys *	pCell =(CGridCellSys *)m_GridCtrl.GetCell(iIndex+1, 0);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 			
	pCell->SetText(StrName);
	
	m_GridCtrl.SetCellType(iIndex+1, 1, RUNTIME_CLASS(CGridCellSymbol));
	CGridCellSymbol *pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iIndex+1, 1);
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
	pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	StrText = Float2SymbolString(fPriceNew,fPreClose,iSaveDec);
	pCellSymbol->SetText(StrText);
	
	pCell =(CGridCellSys *)m_GridCtrl.GetCell(iIndex+1, 2);
	pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 			
	StrText = Float2String(fPreClose,iSaveDec);
	pCell->SetText(StrText);
	
//	if (0 >= iSaveDec)
	{
		iSaveDec = 2;
	}

	m_GridCtrl.SetCellType(iIndex+1, 3, RUNTIME_CLASS(CGridCellSymbol));
	pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iIndex+1, 3);
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);	
	pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);	
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSFall);
	StrText = Float2SymbolString(fRate,0,iSaveDec,FALSE,TRUE,TRUE);
	pCellSymbol->SetText(StrText);
	
}

int32 CDlgRecentMerch::GetGridData(IN CAbsCenterManager * pAbsCenterManager, IN CMerch *pMerch, OUT CString & StrName, OUT float & fPriceNow, OUT float & fPreClose, OUT float & fRate, OUT int32 & iSaveDec)
{
	if ( NULL == pAbsCenterManager)	
		return false;
	
	CRealtimePrice  Realtimeprice;		
	iSaveDec = 0;	
	
	if (NULL != pMerch)
	{
		StrName    = pMerch->m_MerchInfo.m_StrMerchCnName;
		iSaveDec   = pMerch->m_MerchInfo.m_iSaveDec;
		
		CRealtimePrice * pRealtimePrice = pMerch->m_pRealtimePrice;	
		if (NULL != pRealtimePrice)
		{
			Realtimeprice = * pRealtimePrice;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -2;
	}
		
	// 获取昨收价 ,期货用昨结
	float fPricePrevAvg = Realtimeprice.m_fPricePrevAvg;
	fPriceNow  = Realtimeprice.m_fPriceNew;
	fPreClose  = Realtimeprice.m_fPricePrevClose;


	if ( ERTFuturesCn == pMerch->m_Market.m_MarketInfo.m_eMarketReportType )
	{
		if ( 0.0 == fPricePrevAvg || 0.0 == fPriceNow)
		{
			fRate = 0.0;
		}
		else
		{
			fRate = (fPriceNow - fPricePrevAvg)*100/ fPricePrevAvg;
		}
	}
	else
	{
		if ( 0.0 == fPreClose || 0.0 == fPriceNow)
		{
			fRate = 0.0;
		}
		else
		{
			fRate = (fPriceNow - fPreClose)*100/ fPreClose;
		}
	}

	return 1;
}

void CDlgRecentMerch::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();

	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	int32 iRow = pItem->iRow;
	if ( iRow == 0 )	
		return;
	
	CArray<T_RecentMerch, T_RecentMerch&> aRecentMerchList;
	pMainFrame->GetRecentMerchList(aRecentMerchList);
	
	int32 iIndex = iRow - 1;
	if ( iIndex  < 0 || iIndex >= aRecentMerchList.GetSize())
	{
		return;
	}
	T_RecentMerch &RM = aRecentMerchList[iIndex];

	pMainFrame->OnHotKeyMerch(RM.m_pMerch->m_MerchInfo.m_iMarketId, RM.m_pMerch->m_MerchInfo.m_StrMerchCode);

// 	CIoViewBase * pIoView = pMainFrame->FindActiveIoView();
// 	if (NULL == pIoView)	
// 	{
// 		pMainFrame->CreateIoViewByPicMenuID(ID_PIC_TREND, true);
// 		pIoView = pMainFrame->FindActiveIoView();
// 	}
// 
// 	if (NULL == pIoView || NULL == pAbsCenterManager)
// 	{
// 		return;
// 	}
// 	
// 	pAbsCenterManager->OnViewMerchChanged(pIoView, RM.m_pMerch);
}

void CDlgRecentMerch::SetPosition()
{
	CenterWindow();
}
