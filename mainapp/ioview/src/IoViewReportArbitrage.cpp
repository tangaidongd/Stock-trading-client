// IoViewReportArbitrage.cpp: implementation of the CIoViewReportArbitrage class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "IoViewReportArbitrage.h"
#include "GridCellSys.h"
#include "GridCellSymbol.h"
#include "DlgArbitrage.h"
#include "IoViewManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// xml
static const char KXMLARBREPORTShowGridLines[] = "ShowGridLine";
#define GRID_ID 0x2026

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CIoViewReportArbitrage, CIoViewBase)

BEGIN_MESSAGE_MAP(CIoViewReportArbitrage, CIoViewBase)
ON_WM_CREATE()
ON_WM_PAINT()
ON_WM_SIZE()
ON_NOTIFY(NM_RCLICK, GRID_ID, OnGridRButtonDown)
ON_NOTIFY(NM_DBLCLK, GRID_ID, OnGridDblClick)
ON_COMMAND_RANGE(IDM_ARBITRAGE_BEGIN, IDM_ARBITRAGE_END, OnMenu)
ON_COMMAND_RANGE(IDM_ARBREPORT_GRIDLINE, IDM_ARBREPORT_GRIDLINE, OnMenu)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

// ��ͷ����
static const T_ArbitrageHeadObj s_KArbitrageHeadObjs[] = 
{
	T_ArbitrageHeadObj(EAHName,				L"����ģ��"),
    T_ArbitrageHeadObj(EAHPriceDifAccord,	L"�Լۼ۲�"),
	T_ArbitrageHeadObj(EAHPriceDifQuote,	L"�Ҽۼ۲�"),
	T_ArbitrageHeadObj(EAHPriceDifNew,		L"���¼۲�"),
	T_ArbitrageHeadObj(EAHRisePriceDif,		L"�۲��ǵ�"),
	T_ArbitrageHeadObj(EAHPriceMerchA,		L"A ���¼�"),
	T_ArbitrageHeadObj(EAHPriceMerchB,		L"B ���¼�"),
	T_ArbitrageHeadObj(EAHPriceMerchC,		L"C ���¼�"),
	T_ArbitrageHeadObj(EAHPriceDifOpen,		L"���̼۲�"),
	T_ArbitrageHeadObj(EAHPriceDifPreClose, L"���ռ۲�"),
	T_ArbitrageHeadObj(EAHPriceDifPreAvg,	L"���۲�"),
	T_ArbitrageHeadObj(EAHPriceDifAvg,		L"����۲�"),
};

// ��ͷ����
static const int32 s_KiArbitrageHeadObjCounts = sizeof(s_KArbitrageHeadObjs) / sizeof(T_ArbitrageHeadObj);

//
CIoViewReportArbitrage::CIoViewReportArbitrage()
{
	m_aArbitrages.RemoveAll();
	m_bShowGridLines = false;
}

CIoViewReportArbitrage::~CIoViewReportArbitrage()
{
	CArbitrageManage::Instance()->DelNotify(this);
}

int CIoViewReportArbitrage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CIoViewBase::OnCreate(lpCreateStruct);	
	InitialIoViewFace(this);

	// ����
	CreateTable();

	// ����֪ͨ
	CArbitrageManage::Instance()->AddNotify(this);

	//
	UpdateTableContent();

	return 0;
}

void CIoViewReportArbitrage::RequestViewData()
{
	if ( NULL == m_pAbsCenterManager )
	{
		return;
	}

	// �����ע��Ʒ
	m_aSmartAttendMerchs.RemoveAll();
	CSmartAttendMerch att;
	att.m_bNeedNews = false;
	att.m_iDataServiceTypes = EDSTPrice;
	
	//
	CArray<CMerch*, CMerch*> aMerchs;
	CArbitrageManage::Instance()->GetArbitrageMerchs(aMerchs);

	if ( aMerchs.GetSize() <= 0 )
	{
		return;
	}
	
	CMmiReqRealtimePrice Req;
	Req.m_iMarketId = -1;
	
	//
	for ( int32 i = 0; i < aMerchs.GetSize(); i++ )
	{
		CMerch* pMerch = aMerchs[i];
		if ( NULL == pMerch )
		{
			continue;
		}
		
		if ( -1 == Req.m_iMarketId )
		{
			Req.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
			Req.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
		}
		else
		{
			CMerchKey MerchKey;
			
			MerchKey.m_iMarketId	= pMerch->m_MerchInfo.m_iMarketId;
			MerchKey.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
			
			Req.m_aMerchMore.Add(MerchKey);
		}

		att.m_pMerch = pMerch;
		m_aSmartAttendMerchs.Add(att);
	}
	
	// ��������	
	m_pAbsCenterManager->RequestViewData(&Req);
}

void CIoViewReportArbitrage::OnVDataRealtimePriceUpdate(IN CMerch *pMerch)
{
	CArbitrageManage::ArbitrageSet sArb;
	CArbitrageManage::Instance()->GetArbitrageDatasByMerch(pMerch, sArb);

 	for ( int32 i = m_GridCtrl.GetFixedRowCount(); i < m_GridCtrl.GetRowCount(); i++ )
 	{
		CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(i, 0);
		if ( NULL == pCell )
		{
			ASSERT( 0 );
			continue;
		}
		
		//
		CArbitrage* pData = (CArbitrage*)pCell->GetData();

		if ( (NULL != pData) && (sArb.count(*pData)>0) )
		{
			// ����
 			UpdateOneRow(i, *pData);	
		}
 	}
 	
 	//
 	m_GridCtrl.RedrawWindow();
}

void CIoViewReportArbitrage::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	if ( !IsWindowVisible() )
	{
		return;
	}
	
	if ( GetParentGGTongViewDragFlag() || m_bDelByBiSplitTrack )
	{
		LockRedraw();
		return;
	}
	
	//
	UnLockRedraw();
	m_GridCtrl.RedrawWindow();
	m_XSBHorz.RedrawWindow();
	m_XSBVert.RedrawWindow();
}

void CIoViewReportArbitrage::OnSize(UINT nType, int cx, int cy)
{
	CIoViewBase::OnSize(nType, cx, cy);

	if ( NULL == m_GridCtrl.GetSafeHwnd() || NULL == m_XSBHorz.GetSafeHwnd() || NULL == m_XSBVert.GetSafeHwnd() )
	{
		return;
	}

	//
	CRect RectClient;
	GetClientRect(&RectClient);
	int32 iSizeScrol = 12;//m_XSBVert.GetFitHorW();	

	// ����
	CRect RectScrollH = RectClient;
	RectScrollH.top   = RectScrollH.bottom - iSizeScrol;

	// ���
	CRect RectGrid = RectClient;
	RectGrid.bottom= RectScrollH.top;

	// 
	m_GridCtrl.MoveWindow(&RectGrid);
	m_XSBHorz.SetSBRect(RectScrollH, TRUE);
	m_XSBHorz.ShowWindow(SW_SHOW);
	
	//
	// m_GridCtrl.ExpandColumnsToFit();
	m_GridCtrl.AutoSizeColumn(0, GVS_BOTH);
	m_GridCtrl.AutoSizeRows();
	m_GridCtrl.RedrawWindow();
}

void CIoViewReportArbitrage::OnMenu(UINT nID)
{
	if ( IDM_ADD_ARBITRAGE == nID )
	{
		// ����
		CDlgArbitrage Dlg;
		Dlg.SetInitParams(true, NULL);
		Dlg.DoModal();
	}
	else if ( IDM_MODIFY_ARBITRAGE == nID )
	{
		// �޸�
		CArbitrage* pArbitrage = GetCurrentSelArbitrage();
		if ( NULL == pArbitrage )
		{
			return;
		}

		CDlgArbitrage Dlg;
		Dlg.SetInitParams(false, pArbitrage);
		Dlg.DoModal();
	}
	else if ( IDM_DEL_ARBITRAGE == nID )
	{
		// ɾ��
 		CArbitrage* pArbitrage = GetCurrentSelArbitrage();
 		if ( NULL == pArbitrage )
 		{
 			return;
 		}
 
 		//
 		if ( IDYES == MessageBox(L"ȷ��ɾ��?", AfxGetApp()->m_pszAppName, MB_YESNO) )
 		{
 			CArbitrageManage::Instance()->DelArbitrage(*pArbitrage);
 		}		
	} 
	else if ( IDM_ARBREPORT_GRIDLINE == nID )
	{
		m_bShowGridLines = !m_bShowGridLines;
		m_GridCtrl.ShowGridLine(m_bShowGridLines);
	}
}

void CIoViewReportArbitrage::OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	// �����˵�
	CPoint pt;
	GetCursorPos(&pt);
	
	CNewMenu menu;	
	menu.LoadMenu(IDR_MENU_REPORT_ARBITRAGE);
	CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
	pPopMenu->LoadToolBar(g_awToolBarIconIDs);

	if ( NULL != pPopMenu )
	{
		CArbitrage* pData = GetCurrentSelArbitrage();
		if ( NULL == pData )
		{
			pPopMenu->EnableMenuItem(IDM_MODIFY_ARBITRAGE, MF_BYCOMMAND | MF_GRAYED);
			pPopMenu->EnableMenuItem(IDM_DEL_ARBITRAGE, MF_BYCOMMAND | MF_GRAYED);
		}

		// ������
		if ( m_bShowGridLines )
		{
			pPopMenu->ModifyODMenu(_T("����������"), IDM_ARBREPORT_GRIDLINE);
		}

		// ��������:
		CNewMenu* pIoViewPopMenu = (CNewMenu *)pPopMenu->GetSubMenu(L"��������");
		ASSERT(NULL != pIoViewPopMenu );
		if ( NULL != pIoViewPopMenu )
		{
			AppendIoViewsMenu(pIoViewPopMenu, IsLockedSplit());
		}

		// ������������ָ�״̬����Ҫɾ��һЩ��ť
		CMPIChildFrame *pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
		if ( NULL != pChildFrame && pChildFrame->IsLockedSplit() )
		{
			pChildFrame->RemoveSplitMenuItem(*pPopMenu);
		}

		pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, AfxGetMainWnd());
		menu.DestroyMenu();
	}	
}

bool32 CIoViewReportArbitrage::CreateTable()
{
	// ���Դ������
	if (NULL == m_GridCtrl.GetSafeHwnd())
	{
		// �����������
		m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 11243);
		m_XSBVert.SetScrollRange(0, 10);
		
		m_XSBHorz.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10214);
		m_XSBHorz.SetScrollRange(0, 10);
		
		// �������ݱ��
		m_GridCtrl.Create(CRect(0, 0, 0, 0), this, GRID_ID);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetBackClr(0x00);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetTextClr(0xa0a0a0);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
		
		// �����໥֮��Ĺ���
		m_XSBHorz.SetOwner(&m_GridCtrl);
		m_XSBVert.SetOwner(&m_GridCtrl);
		m_GridCtrl.SetScrollBar(&m_XSBHorz, &m_XSBVert);
		m_XSBHorz.AddMsgListener(m_hWnd);	// ����������Ϣ - ��¼������Ϣ
		
		// ���ñ�ͷ
		m_GridCtrl.SetHeaderSort(FALSE);
		m_GridCtrl.SetUserCB(this);
		
		// ��ʾ����ߵķ��
		m_GridCtrl.ShowGridLine(false);
		m_GridCtrl.SetDrawFixedCellGridLineAsNormal(TRUE);
		m_GridCtrl.SetRowHeightFixed(TRUE);		// �̶��и�
		m_GridCtrl.SetSingleRowSelection(FALSE);
		m_GridCtrl.SetDrawSelectedCellStyle(GVSDS_DEFAULT);
		
		////////////////////////////////////////////////////////////////
		// ��ձ������
		m_GridCtrl.DeleteAllItems();
		m_GridCtrl.SetFixedRowCount(1);		
		m_GridCtrl.SetColumnCount(s_KiArbitrageHeadObjCounts);
		
		m_GridCtrl.EnableSelection(true);
		// ��������
		LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
		m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
		m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
		m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
		m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);
	}
	
	// ���ñ�ͷ
	for ( int32 i = 0; i < s_KiArbitrageHeadObjCounts; i++ )
	{
		E_ArbitrageHead eHead = s_KArbitrageHeadObjs[i].m_eArbitrageHead;
		CString	StrHeadName = s_KArbitrageHeadObjs[i].m_StrHeadName;
		
		CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(0, i);
		if ( NULL == pCell )
		{
			//ASSERT(0);
			continue;
		}
		
		//
		pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX); 	
		pCell->SetText(StrHeadName);
		pCell->SetData((DWORD)eHead);
	}

	return true;
}

void CIoViewReportArbitrage::UpdateOneRow(int32 iRow, const CArbitrage& stArbitrage, bool32 bUpdateData/* = false*/)
{
	if ( iRow <= 0 || iRow >= m_GridCtrl.GetRowCount() )
	{
		return;
	}

	// ����ÿ����ͷ���ֵ:
	for ( int32 j = 0; j < s_KiArbitrageHeadObjCounts; j++ )
	{
		CString StrName = stArbitrage.GetShowName();
		
		CGridCellSys* pCell0 = (CGridCellSys*)m_GridCtrl.GetCell(0, j);
		if ( NULL == pCell0 )
		{
			//ASSERT(0);
			continue;
		}
		
		E_ArbitrageHead eHeadType = (E_ArbitrageHead)pCell0->GetData();
		
		// �۸������ʾ�Ĳο��۸�Ϊ���ռ۲�
		float fAccordingPrice = stArbitrage.GetPrice(EAHPriceDifPreClose);
		
		//
		switch (eHeadType)
		{
		case EAHName:			// ����
			{
				CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRow, j);
				
				// ����
				if ( NULL != pCell )
				{
					pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
					pCell->SetText(StrName);
				}
			}
			break;
		case EAHRisePriceDif:		// �۲��ǵ�
			{
				SetCellValue(iRow, j, stArbitrage.GetPrice(eHeadType), 0.0f, eHeadType, stArbitrage.m_iSaveDec);	
			}
			break;
		case EAHPriceDifAccord:		// �Լۼ۲�
		case EAHPriceDifQuote:		// ���ۼ۲�
		case EAHPriceDifNew:		// ���¼۲�
		case EAHPriceMerchA:		// ��Ʒ A ���¼�
		case EAHPriceMerchB:		// ��Ʒ B ���¼�
		case EAHPriceMerchC:		// ��Ʒ C ���¼�
		case EAHPriceDifOpen:		// ���̼۲�
		case EAHPriceDifPreClose:	// ���ռ۲�
		case EAHPriceDifPreAvg:		// ���۲�
		case EAHPriceDifAvg:		// ����۲�
			{
				SetCellValue(iRow, j, stArbitrage.GetPrice(eHeadType), fAccordingPrice, eHeadType, stArbitrage.m_iSaveDec);					
			}
			break;
		default:
			break;
		}			
	}
	
	// �Զ�������
	if ( bUpdateData )
	{
		CGridCellSys* pCellData = (CGridCellSys*)m_GridCtrl.GetCell(iRow, 0);
		if ( NULL == pCellData )
		{
			return;
		}
		
		// 
		pCellData->SetData((DWORD)&stArbitrage);
	}	
}

int32 CIoViewReportArbitrage::GetRowIndex(const CArbitrage& stArbitrage)
{
	for ( int32 i = 1; i < m_GridCtrl.GetRowCount(); i++ )
	{
		CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(i, 0);
		if ( NULL == pCell )
		{
			return -1;
		}
		
		//
		CArbitrage* pData = (CArbitrage*)pCell->GetData();
		
		if ( (NULL != pData) && (*pData == stArbitrage) )
		{
			return i;
		}
	}

	return -1;
}

void CIoViewReportArbitrage::UpdatePushMerch()
{
	m_aSmartAttendMerchs.RemoveAll();

	//
	CArray<CMerch*, CMerch*> aMerchs;
	CArbitrageManage::Instance()->GetArbitrageMerchs(aMerchs);

	for ( int32 i = 0; i < aMerchs.GetSize(); i++ )
	{
		CMerch* pMerch = aMerchs[i];
		if ( NULL == pMerch )
		{
			continue;
		}

		//
		CSmartAttendMerch SmartAttendMerch;
		SmartAttendMerch.m_pMerch = pMerch;
		SmartAttendMerch.m_iDataServiceTypes = EDSTPrice;
		SmartAttendMerch.m_bNeedNews = false;

		//
		m_aSmartAttendMerchs.Add(SmartAttendMerch);
	}
}

void CIoViewReportArbitrage::UpdateTableContent()
{
	//
	UpdatePushMerch();
	RequestViewData();

	// ����ǹ̶���
	m_GridCtrl.DeleteNonFixedRows();	

	//
	CArbitrageManage::Instance()->GetArbitrageDatas(m_aArbitrages);
	CArbitrage* pData = (CArbitrage*)m_aArbitrages.GetData();

	//
	for ( int32 i = 0; i < m_aArbitrages.GetSize(); i++ )
	{
		//
		int32 iRowIndex = m_GridCtrl.InsertRow(L"");

		//
		UpdateOneRow(iRowIndex, pData[i], true);	
	}

	CCellRange SelRange = m_GridCtrl.GetSelectedCellRange();
	if ( !SelRange.IsValid() )
	{
		m_GridCtrl.SetSelectedSingleRow(m_GridCtrl.GetFixedRowCount());
	}
	
	//
	m_GridCtrl.Refresh();
	m_GridCtrl.AutoSizeColumn(0, GVS_BOTH);
	m_GridCtrl.AutoSizeRows();
	m_GridCtrl.RedrawWindow();
}

void CIoViewReportArbitrage::SetCellValue(int32 iRow, int32 iCol, float fPriceNow, float fAccordingPrice, E_ArbitrageHead eType, int32 iSaveDec)
{
	if ( iRow <= 0 || iRow >= m_GridCtrl.GetRowCount() || iCol < 0 || iCol >= m_GridCtrl.GetColumnCount() )
	{
		return;
	}

	m_GridCtrl.SetCellType(iRow, iCol, RUNTIME_CLASS(CGridCellSymbol));
	CGridCellSymbol* pCellSymbol = (CGridCellSymbol *)m_GridCtrl.GetCell(iRow, iCol);
	pCellSymbol->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	pCellSymbol->SetShowSymbol(CGridCellSymbol::ESSNone);
	
	CString StrText = L"";
	StrText = Float2SymbolString(fPriceNow, fAccordingPrice, iSaveDec, false, true, false, true);	// ������ʾ��ƽ
	pCellSymbol->SetBlinkText(StrText);
}

CArbitrage* CIoViewReportArbitrage::GetCurrentSelArbitrage()
{
	POSITION pos = m_GridCtrl.GetFirstSelectedRowPosition();
	int32 iRow = -1;
	while ( NULL != pos )
	{
		iRow = m_GridCtrl.GetNextSelectedRow(pos);
	}
	
	if ( m_GridCtrl.GetRowCount() <= 1 )
	{	
		return NULL;
	}
	
	// Ĭ��ȡ��һ�е���Ʒ
	if ( iRow <= 0 || iRow >= m_GridCtrl.GetRowCount() )
	{		
		return NULL;
	}
	
	//
	CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRow, 0);
	if ( NULL == pCell )
	{		
		return NULL;
	}
	
	//
	CArbitrage* pData = (CArbitrage*)pCell->GetData();
	
	//
	return pData;
}

void CIoViewReportArbitrage::OnArbitrageAdd(const CArbitrage& stArbitrage)
{
	// ����һ��
	UpdateTableContent();
}

void CIoViewReportArbitrage::OnArbitrageDel(const CArbitrage& stArbitrage)
{
	// ɾ��һ��
	UpdateTableContent();	
}

void CIoViewReportArbitrage::OnArbitrageModify(const CArbitrage& stArbitrageOld, const CArbitrage& stArbitrageNew)
{
	// �޸�
	UpdateTableContent();
}

BOOL CIoViewReportArbitrage::PreTranslateMessage( MSG* pMsg )
{
	if ( pMsg->message == WM_KEYDOWN )
	{
		if ( VK_F5 == pMsg->wParam )
		{
			OnF5();
			return TRUE;
		}
	}
	return CIoViewBase::PreTranslateMessage(pMsg);
}

void CIoViewReportArbitrage::OnF5()
{
	CArbitrage *pArb = GetCurrentSelArbitrage();
	if ( NULL != pArb )
	{
		ShowArbitrageChart(*pArb);
	}
}

void CIoViewReportArbitrage::OnGridDblClick( NMHDR *pNotifyStruct, LRESULT* pResult )
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	
	int32 iRow = pItem->iRow;
	if ( iRow < m_GridCtrl.GetFixedRowCount() )
	{
		return;
	}

	if ( iRow >= m_GridCtrl.GetRowCount() )
	{		
		return;
	}
	
	//
	CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(iRow, 0);
	if ( NULL == pCell )
	{		
		return;
	}
	
	//
	CArbitrage* pData = (CArbitrage*)pCell->GetData();
	if ( NULL != pData )
	{
		ShowArbitrageChart(*pData);
	}
}

void CIoViewReportArbitrage::ShowArbitrageChart( const CArbitrage &arb )
{
	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if ( NULL == pMainFrame )
	{
		return;
	}
	
	pMainFrame->OnArbitrageF5(arb, this);
}

CMerch * CIoViewReportArbitrage::GetMerchXml()
{
	CMerch *pMerch = NULL;
	CArbitrage *pArb = GetCurrentSelArbitrage();
	if ( NULL != pArb )
	{
		pMerch = pArb->m_MerchA.m_pMerch;
	}
	if ( NULL == pMerch )
	{
		pMerch = CIoViewBase::GetMerchXml();
	}
	return pMerch;
}

int32 CIoViewReportArbitrage::SetCurrentSelArbitrage( const CArbitrage &arbToSel )
{
	int32 i = m_GridCtrl.GetFixedRowCount();
	const int32 iCount = m_GridCtrl.GetRowCount();
	for ( ; i < iCount ; ++i )
	{
		CGridCellSys* pCell = (CGridCellSys*)m_GridCtrl.GetCell(i, 0);
		if ( NULL != pCell )
		{		
			CArbitrage* pData = (CArbitrage*)pCell->GetData();
			if ( NULL != pData && *pData == arbToSel )
			{
				m_GridCtrl.SetSelectedSingleRow(i);
				return i;
			}	
		}
	}
	return -1;
}

bool32 CIoViewReportArbitrage::FromXml( TiXmlElement * pTiXmlElement )
{
	if (NULL == pTiXmlElement)
		return false;
	
	// �ж��ǲ���IoView�Ľڵ�
	const char *pcValue = pTiXmlElement->Value();
	if (NULL == pcValue || strcmp(GetXmlElementValue(), pcValue) != 0)
		return false;
	
	// �ж��ǲ��������Լ����ҵ����ͼ�Ľڵ�
	const char *pcAttrValue = pTiXmlElement->Attribute(GetXmlElementAttrIoViewType());
	CString StrIoViewString = _A2W(pcAttrValue);
	if (NULL == pcAttrValue || CIoViewManager::GetIoViewString(this).Compare(StrIoViewString) != 0)	// ���������Լ���ҵ��ڵ�
		return false;
	
	// �Ƿ���ʾ������
	pcAttrValue = pTiXmlElement->Attribute(KXMLARBREPORTShowGridLines);
	
	if ( NULL != pcAttrValue )
	{
		m_bShowGridLines = atoi(pcAttrValue);
		
		if ( m_bShowGridLines )
		{
			m_GridCtrl.ShowGridLine(true);
		}
		else
		{
			m_GridCtrl.ShowGridLine(false);
		}
	}

	return true;
}

CString CIoViewReportArbitrage::ToXml()
{
	CString StrThis;
	// 
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%d\" >\n", 
		CString(GetXmlElementValue()).GetBuffer(), 
		CString(GetXmlElementAttrIoViewType()).GetBuffer(), 
		CIoViewManager::GetIoViewString(this).GetBuffer(),
		CString(GetXmlElementAttrShowTabName()).GetBuffer(), m_StrTabShowName.GetBuffer(),
		CString(KXMLARBREPORTShowGridLines).GetBuffer(), m_bShowGridLines
		);
	
	//
	StrThis += L"</";
	StrThis += CString(GetXmlElementValue());
	StrThis += L">\n";
	
	return StrThis;
}

CString CIoViewReportArbitrage::GetDefaultXML()
{
	CString StrThis;
	// 
	StrThis.Format(L"<%s %s=\"%s\" >\n", 
		CString(GetXmlElementValue()).GetBuffer(), 
		CString(GetXmlElementAttrIoViewType()).GetBuffer(), 
		CIoViewManager::GetIoViewString(this).GetBuffer()
		);
	
	//
	StrThis += L"</";
	StrThis += CString(GetXmlElementValue());
	StrThis += L">\n";
	
	return StrThis;
}

void CIoViewReportArbitrage::OnIoViewActive()
{
	CIoViewBase::OnIoViewActive();

	m_bActive = IsActiveInFrame();
	
	//
	//OnVDataForceUpdate();
	RequestLastIgnoredReqData();
	
	SetChildFrameTitle();
	
	if (NULL != m_GridCtrl.GetSafeHwnd())
	{
		m_GridCtrl.SetFocus();
	}
}

void CIoViewReportArbitrage::OnIoViewDeactive()
{
	CIoViewBase::OnIoViewDeactive();

	m_bActive = false;
}

void CIoViewReportArbitrage::LockRedraw()
{
	if( m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = true;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
	::SendMessage(m_GridCtrl.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
}

void CIoViewReportArbitrage::UnLockRedraw()
{
	if ( !m_bLockRedraw )
	{
		return;
	}
	
	bool32 bVisiable = IsWindowVisible();
	
	m_bLockRedraw = false;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
	::SendMessage(m_GridCtrl.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
	
	UpdateTableContent();
	
	if ( !bVisiable )
	{
		ShowWindow(SW_HIDE);
	}
}

void CIoViewReportArbitrage::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();	
	
	UpdateTableContent();	// ������Ʒȫ��������ʾ
}

void CIoViewReportArbitrage::OnIoViewFontChanged()
{
	CIoViewBase::OnIoViewFontChanged();
	
	// ��������
	LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);
	
	UpdateTableContent();
}
