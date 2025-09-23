#pragma warning(disable:4786)
#include "stdafx.h"
#include "ShareFun.h"
#include "memdc.h"
#include "io.h"
#include "PathFactory.h"
#include "MerchManager.h"
#include "UserBlockManager.h"
#include "MerchManager.h"
#include "facescheme.h"
#include "GGTongView.h"
#include "IoViewManager.h"
#include "IoViewShare.h"
#include "IoViewTimeSale.h"
#include "GridCellSymbol.h"
#include "GridCellLevel2.h"
#include "GridCellCheck.h"
#include "DlgBlockSelect.h"
#include "DlgBlockSet.h"

#include "ColorStep.h"
#include "GridCtrlNormal.h"
#include "IoViewMainTimeSale.h"
#include "DlgMainTimeSale.h"
#include "LogFunctionTime.h"
#include "PluginFuncRight.h"
#include "ConfigInfo.h"
#include "FontFactory.h"

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
//    "fatal error C1001: INTERNAL COMPILER ERROR"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const int32 KUpdatePushMerchsTimerId						= 100005;				// ÿ�� n ����, ���¼���һ�ε�ǰ��������Ʒ
const int32 KTimerPeriodPushMerchs							= 1000 * 60;

const int32	KTimerIdCheckInitData							= 1001;					// ����ʼ�������Ƿ��Ѿ�����
const int32 KTimerPeriodCheckInitData						= 5000;							

const int32 KIDefaultMonitorCount							= 150;
const int32 KIDefaultMaxMonitorCount						= 3000;

const int32 KICurrentMerchBlockId							= -4;

const char KXmlStrCurTabKey[]								= "CurTab";					// ��ǰ��ѡ��

#define  ID_GRID_CTRL         12366

//////////////////////////////////////////////////////////////////////////
bool32 operator==(const T_RespMainMonitor &resp1, const T_RespMainMonitor &resp2)
{
	return resp1.m_Time == resp2.m_Time
		&& resp1.m_fCounts == resp2.m_fCounts
		&& resp1.m_fPrice == resp2.m_fPrice
		&& resp1.m_uBuy == resp2.m_uBuy;
}

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CIoViewMainTimeSale, CIoViewBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewMainTimeSale, CIoViewBase)
//{{AFX_MSG_MAP(CIoViewMainTimeSale)
ON_WM_PAINT()
ON_WM_CREATE()
ON_WM_SIZE()
ON_WM_TIMER()
ON_WM_LBUTTONDOWN()
ON_WM_CONTEXTMENU()
ON_MESSAGE_VOID(UM_DOINITIALIZE, OnDoInitialize)
//}}AFX_MSG_MAP
ON_NOTIFY(NM_RCLICK,ID_GRID_CTRL,OnGridRButtonDown)
ON_NOTIFY(NM_DBLCLK, ID_GRID_CTRL, OnGridDblClick)
ON_NOTIFY(GVN_COLWIDTHCHANGED, ID_GRID_CTRL, OnGridColWidthChanged)
ON_NOTIFY(GVN_KEYDOWNEND, ID_GRID_CTRL, OnGridKeyDownEnd)
ON_NOTIFY(GVN_KEYUPEND, ID_GRID_CTRL, OnGridKeyUpEnd)
END_MESSAGE_MAP()
///////////////////////////////////////////////////////////////////////////////

// ׼���Ż� fangz20100514

CIoViewMainTimeSale::CIoViewMainTimeSale()
:CIoViewBase()
{
	m_pParent		= NULL;
	m_rectClient	= CRect(-1,-1,-1,-1);

	m_RectBtn.SetRectEmpty();
	
	m_aUserBlockNames.RemoveAll();

	m_bDrawTitleString = true;

	m_bParentIsDialog = false;

	m_bInitialized =	false;

	m_iCurTab = 0;
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewMainTimeSale::~CIoViewMainTimeSale()
{
	// MainFrame �п�ݼ�
	CMainFrame* pMainFrame =(CMainFrame*)AfxGetMainWnd();
	if ( NULL != pMainFrame && pMainFrame->m_pKBParent == this )
	{
		pMainFrame->SetHotkeyTarget(NULL);
	}
	
	POSITION pos = m_lstMainTimeSales.GetHeadPosition();
	while ( pos != NULL )
	{
		delete m_lstMainTimeSales.GetNext(pos);
	}
	m_lstMainTimeSales.RemoveAll();
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
void CIoViewMainTimeSale::OnPaint()
{
	CPaintDC dcPaint(this); // device context for painting

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

	CRect rectClient;
	GetClientRect(&rectClient);

	COLORREF clrBk = GetIoViewColor(ESCBackground);
	COLORREF clrText = GetIoViewColor(ESCText);
	COLORREF clrAxis = GetIoViewColor(ESCChartAxisLine);
	COLORREF clrVol = GetIoViewColor(ESCVolume);

// 	if ( !CPluginFuncRight::Instance().IsUserHasRight(CPluginFuncRight::FuncMainTimeSaleMonitor, false) )
// 	{
// 		CMemDC dc(&dcPaint, rectClient);
// 		dc->FillSolidRect(rectClient, clrBk);
// 		dc->SelectObject(GetIoViewFontObject(ESFSmall));
// 		dc->SetTextColor(clrText);
// 		dc->SetBkMode(TRANSPARENT);
// 		CString StrRight = CPluginFuncRight::Instance().GetUserRightName(CPluginFuncRight::FuncMainTimeSaleMonitor);
// 		CString StrShow = _T("���߱�[")+StrRight+_T("]Ȩ��, ") + CVersionInfo::Instance()->GetPrompt();
// 		CRect rcDraw(rectClient);
// 		rcDraw.top = rectClient.CenterPoint().y;
// 		rcDraw.top -= 15;
// 		rcDraw.InflateRect(-2, 0);
// 		dc->DrawText(StrShow, rcDraw, DT_CENTER |DT_WORDBREAK );
// 	}
// 	else
	{
		CMemDC dc(&dcPaint, m_RectBtn);
		int32 iSaveDC = dc.SaveDC();
		
		dc->FillSolidRect(m_RectBtn, clrBk);
		CFont *pFontSmall = GetIoViewFontObject(ESFSmall);
		dc.SelectObject(pFontSmall);
		dc->SetBkMode(TRANSPARENT);
		dc->SetBkColor(clrBk);
		
		CBrush brhAxis;
		brhAxis.CreateSolidBrush(clrAxis);
		
		for ( int i=0; i < m_aTitleBtns.GetSize() ; i++ )
		{
			CRect rcDraw(m_aTitleBtns[i].m_RectBtn);
			CMDIChildWnd *pChildFrame = DYNAMIC_DOWNCAST(CMDIChildWnd, GetParentFrame());
			if ( 0 == i && pChildFrame != NULL )
			{
				CPoint ptLeftTop = rcDraw.TopLeft();
				MapWindowPoints(pChildFrame, &ptLeftTop, 1);
				if ( ptLeftTop.x > 0 )
				{
					rcDraw.left -= 1;	// ѹ�����һ������
				}
			}
			rcDraw.InflateRect(0, 1, 1, 0);
			//dc->FrameRect(rcDraw, &brhAxis);
			COLORREF clrDrawText = RGB(160, 160, 160);
			if ( i == m_iCurTab )
			{
				clrDrawText = clrVol;
			}
			dc->SetTextColor(clrDrawText);
			dc->DrawText(m_aTitleBtns[i].m_StrName, m_aTitleBtns[i].m_RectBtn, DT_SINGLELINE |DT_CENTER |DT_VCENTER);
		}
		
		dc->SetTextColor(RGB(160, 160, 160));
		CRect rcTitle = m_RectBtn;
		if ( m_aTitleBtns.GetSize() > 0 )
		{
			rcTitle.left = m_aTitleBtns[m_aTitleBtns.GetSize()-1].m_RectBtn.right;
		}
		if ( m_bDrawTitleString )
		{
			CRect rcDrawText = rcTitle;
			rcDrawText.right -= 5;
			dc->DrawText(_T("�������"), rcDrawText, DT_SINGLELINE |DT_RIGHT |DT_VCENTER);
			rcTitle.InflateRect(0, 1, 1, 0);
		}
		else
		{
			rcTitle.InflateRect(0, 1, 0, 0);
		}
		//dc->FrameRect(rcTitle, &brhAxis);
		
		if ( m_bActive )
		{
			//dc->FillSolidRect(m_RectBtn.left+3, m_RectBtn.top+3, 2, 2, clrVol);
		}
		
		// ���߽�
		//DrawCorner(dc, rectClient);

		dc.RestoreDC(iSaveDC);
	}
}

BOOL CIoViewMainTimeSale::PreTranslateMessage(MSG* pMsg)
{
	return CIoViewBase::PreTranslateMessage(pMsg);		// IoViewBase�ܸ���ioviewtype�����
}

int CIoViewMainTimeSale::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( NULL == m_pAbsCenterManager )	// �Լ�ע��
	{
		CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
		SetCenterManager(pApp->m_pDocument->m_pAbsCenterManager);
	}
	
	CIoViewBase::OnCreate(lpCreateStruct);
	
	InitialIoViewFace(this);

	// �����������
	m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10203);
	m_XSBVert.SetScrollRange(0, 10);

	m_XSBHorz.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10204);
	m_XSBHorz.SetScrollRange(0, 10);

	// �������ݱ��
	m_GridCtrl.Create(CRect(0, 0, 0, 0), this, ID_GRID_CTRL);
	m_GridCtrl.SetDefCellWidth(50);
	m_GridCtrl.EnableBlink(FALSE);			// ��ֹ��˸
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetBackClr(CLR_DEFAULT);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetTextClr(CLR_DEFAULT);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

	// ���ñ��ͼ��
	// m_ImageList.Create(MAKEINTRESOURCE(IDB_GRID_REPORT), 16, 1, RGB(255,255,255));

	// �����໥֮��Ĺ���
	m_XSBHorz.SetOwner(&m_GridCtrl);
	m_XSBVert.SetOwner(&m_GridCtrl);
	m_GridCtrl.SetScrollBar(&m_XSBHorz, &m_XSBVert);
	//m_XSBHorz.AddMsgListener(m_hWnd);	// ����������Ϣ - ��¼������Ϣ

	// ���ñ�ͷ
	m_GridCtrl.SetHeaderSort(FALSE);

	// ��ʾ����ߵķ��
	m_GridCtrl.ShowGridLine(FALSE);
	m_GridCtrl.SetSingleRowSelection(TRUE);
	m_GridCtrl.SetShowSelectWhenLoseFocus(FALSE);

	////////////////////////////////////////////////////////////////
// 	m_GridCtrl.SetFixedColumnCount(2);	// �� ��
// 	m_GridCtrl.SetFixedRowCount(1);
	m_GridCtrl.SetColumnCount(4);

	// ��������
	LOGFONT *pFontSmall = GetIoViewFont(ESFSmall);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontSmall);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontSmall);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontSmall);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontSmall);

	if ( IsWindow(GetParent()->GetSafeHwnd()) )
	{
		m_bParentIsDialog = (GetParent()->GetStyle() & WS_POPUP) != 0;
	}
	// ��ʼ������ť
	T_TabBtn btn;
	int32 iInvalidId = CBlockInfo::GetInvalidId();
	btn.m_iBlockId = iInvalidId;
	btn.m_StrName = _T("����");   // ����A - Լ������A����-1��Ϊ��������ȷ��������
	m_aTitleBtns.Add(btn);

	btn.m_iBlockId = iInvalidId;
	btn.m_StrName = _T("300");	  // ����300 - id��ҪѰ��
	m_aTitleBtns.Add(btn);

	btn.m_iBlockId = iInvalidId;
	btn.m_StrName = _T("��ѡ");	  // ��ѡ�� - ���е���ѡ�ɶ���������
	m_aTitleBtns.Add(btn);

	btn.m_iBlockId = KICurrentMerchBlockId;
	btn.m_StrName = _T("��ǰ");	  // viewdata�еĹ�Ʊ
	btn.m_bNeedReqPrivateData = true;		// ��ǰ��Ҫ������������ - ÿ���л�������
	m_aTitleBtns.Add(btn);

	if ( !m_bParentIsDialog )
	{
		btn.m_iBlockId = INT_MIN;
		btn.m_StrName = _T("����");
		m_aTitleBtns.Add(btn);
	}

	m_iCurTab = 0;
	InitializeBtnId();

	PostMessage(UM_DOINITIALIZE, 0, 0);

	// ���ʱ����Գ�ʼ��tab��Դ�˵ģ�����ڴ�ʱӦ�����غ��ˣ����û����Ӧ����do initialize�г�ʼ������tab

	//
	SetTimer(KUpdatePushMerchsTimerId, KTimerPeriodPushMerchs, NULL);
//	SetTimer(KTimerIdCheckInitData, KTimerPeriodCheckInitData, NULL);

	CBlockConfig::Instance()->AddListener(this);

	return 0;
}

void CIoViewMainTimeSale::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);

	RecalcLayout();

	Invalidate(TRUE);
}

BOOL CIoViewMainTimeSale::TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return FALSE;
}

void CIoViewMainTimeSale::OnIoViewActive()
{
	m_bActive = IsActiveInFrame();

	//
	//OnVDataForceUpdate();
	RequestLastIgnoredReqData();

	SetChildFrameTitle();
	
	if (NULL != m_GridCtrl.GetSafeHwnd())
	{
		m_GridCtrl.SetFocus();
	}

	InvalidateRect(m_RectBtn);

	UpdateTableAllContent();
}

void CIoViewMainTimeSale::OnIoViewDeactive()
{
	m_bActive = false;
	
	m_GridCtrl.SetFocusCell(-1, -1);
	m_GridCtrl.SetSelectedRange(-1, -1, -1, -1, TRUE);

	InvalidateRect(m_RectBtn);
}

void CIoViewMainTimeSale::OnIoViewTabShow()
{
	RequestInitialData();
}

void CIoViewMainTimeSale::OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult)
{ 
	if ( !m_bParentIsDialog )
	{
		DoShowStdPopupMenu();	
	}
}

void CIoViewMainTimeSale::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW	*pGridView = (NM_GRIDVIEW *)pNotifyStruct;
	if ( NULL != pGridView && pGridView->iRow >= m_GridCtrl.GetFixedRowCount() )
	{
		T_MainTimeSale *pTMS = (T_MainTimeSale *)m_GridCtrl.GetItemData(pGridView->iRow, 0);
		if ( NULL != CheckMTS(pTMS) )
		{
			OnDblClick(pTMS->m_pMerch);
		}
	}
}

void CIoViewMainTimeSale::OnGridColWidthChanged(NMHDR *pNotifyStruct, LRESULT* pResult)
{	
	
}

void CIoViewMainTimeSale::OnGridKeyDownEnd(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	// ���¼�����ͷ��
	
}

void CIoViewMainTimeSale::OnGridKeyUpEnd(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	// ���ϼ�����ͷ��
	
}

// ֪ͨ��ͼ�ı��ע����Ʒ
void CIoViewMainTimeSale::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch)
		return;
	
	// �޸ĵ�ǰ�鿴����Ʒ
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
}

//
void CIoViewMainTimeSale::OnVDataForceUpdate()
{
	if ( m_lstMainTimeSales.GetCount() < 100 )
	{
		RequestInitialData();		// �п�����ĳ���������������û������Ȼ��ͻȻ�Ͽ����ӣ����ӵ���ķ�������
	}
	RequestTabSpecialData();

	RequestPushViewData();		// �������ʼ
}


void CIoViewMainTimeSale::OnDestroy()
{	
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if ( NULL != pMainFrm && pMainFrm->m_pKBParent == this )
	{
		pMainFrm->SetHotkeyTarget(NULL);
	}

	CBlockConfig::Instance()->RemoveListener(this);

	CIoViewBase::OnDestroy();
}

void CIoViewMainTimeSale::OnTimer(UINT nIDEvent) 
{
	if(!m_bShowNow)
	{
		return;
	}

	if ( nIDEvent == KUpdatePushMerchsTimerId )
	{
		RequestPushViewData(true);		// �������ʼ - ��ʱǿ���������ͣ������п��ܶ�ʧ����
	}
	else if ( KTimerIdCheckInitData == nIDEvent )
	{
		if ( m_lstMainTimeSales.GetCount() < 100 )
		{
			if(m_bShowNow)
			{
				RequestInitialData();
			}
		}
		else
		{
			KillTimer(KTimerIdCheckInitData);
		}
	}
	
	CIoViewBase::OnTimer(nIDEvent);
}


void CIoViewMainTimeSale::RequestInitialData()
{
	// ��Ȩ�޲����������
// 	if ( !CPluginFuncRight::Instance().IsUserHasRight(CPluginFuncRight::FuncMainTimeSaleMonitor, false) )
// 	{
// 		return;
// 	}

	CMmiReqMainMonitor reqmain, req300;
	reqmain.m_iBlockId = m_aTitleBtns[0].m_iBlockId;		// ����id
	reqmain.m_iCount   = KIDefaultMonitorCount+20;								// ������
	reqmain.m_StrMerchCode.Empty();
	reqmain.m_aReqMore.RemoveAll();
	
	// ȫ���������3000����������޳�ǰ��n�����ݣ�����п���ĳЩ��ʼ�����ݲ����������
	// ���ڼ�ȫ��������������
	// ������������� 100������, ����300 100������ �Գ�ʼ���ʼ�����ݣ�
	//	����л�����ѡ�ɣ���ѡȡ��ѡ��100����Ʒ��ÿ����Ʒ���10�����ݣ�����
	//  ����л�����ǰ�����viewdata��������xml��Ʒ��ÿ����Ʒ�������20��
	ASSERT( m_aTitleBtns.GetSize() > 2 && m_aTitleBtns[1].m_iBlockId != -1 );
	req300.m_iCount = KIDefaultMonitorCount+20;
	req300.m_iBlockId = m_aTitleBtns[1].m_iBlockId;
	req300.m_StrMerchCode.Empty();
	
	DoRequestViewData(reqmain, true);
	DoRequestViewData(req300, true);

	RequestPushViewData(true);		// ��������
}

void CIoViewMainTimeSale::RequestPushViewData(bool32 bForce/* = false*/)
{
	// ��Ȩ�޲����������
// 	if ( !CPluginFuncRight::Instance().IsUserHasRight(CPluginFuncRight::FuncMainTimeSaleMonitor, false) )
// 	{
// 		return;
// 	}

	int32 iCurTab, iBlockId;
	if ( GetCurTab(iCurTab, iBlockId) )
	{
		// ������������
		CMmiReqPushPlugInMerchData	req;
		req.m_iMarketID = -1; // m_aTitleBtns[0].m_iBlockId;  // ʼ��Ϊ���е�blockid
		req.m_uType = ECSTMainMonitor;
		DoRequestViewData(req, bForce);
	}
}

void CIoViewMainTimeSale::SetChildFrameTitle()
{
	CString StrTitle;
	StrTitle =  CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;
	
	CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pParentFrame)
	{
		pParentFrame->SetChildFrameTitle(StrTitle);
	}
}

void CIoViewMainTimeSale::OnDblClick(CMerch *pMerch)
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	if ( NULL != pMainFrame )
	{
		pMainFrame->OnShowMerchInChart(pMerch, this);
	}
}

void CIoViewMainTimeSale::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();

	UpdateAllGridRow();

	Invalidate(TRUE);
	m_GridCtrl.Refresh();
}

void CIoViewMainTimeSale::OnIoViewFontChanged()
{
	CIoViewBase::OnIoViewFontChanged();

	// ��Ϊû�б�ͷ�����Բ��ܸı��п�
	RecalcLayout();
	
	Invalidate(TRUE);
}

void CIoViewMainTimeSale::SetRowHeightAccordingFont()
{
	for (int i =0; i < m_GridCtrl.GetRowCount(); ++i)
	{
		m_GridCtrl.SetRowHeight(i, 30);
	}
	//m_GridCtrl.AutoSizeRows();
}

void CIoViewMainTimeSale::SetColWidthAccordingFont()
{
	//m_GridCtrl.AutoSizeColumns();
	//m_GridCtrl.ExpandColumnsToFit();
	m_GridCtrl.SetColumnWidth(0, 60);
	m_GridCtrl.SetColumnWidth(1, 80);
	m_GridCtrl.SetColumnWidth(2, 75);
	CRect rcGrid;
	m_GridCtrl.GetClientRect(rcGrid);
	int iWidth = rcGrid.Width() - 215;
	if ( iWidth > 0 )
	{
		m_GridCtrl.SetColumnWidth(3, iWidth);
	}
}

void CIoViewMainTimeSale::UpdateTableAllContent()
{
	// ����focus cell
	CCellID	cellFocus = m_GridCtrl.GetFocusCell();
	CCellRange cellRange = m_GridCtrl.GetSelectedCellRange();
	m_GridCtrl.DeleteNonFixedRows();	

	// ������Ʒ��Ϣ

	// �õ����������
	while ( m_lstMainTimeSales.GetCount() > KIDefaultMaxMonitorCount )
	{
		delete m_lstMainTimeSales.RemoveHead();
	}

	m_GridCtrl.InsertRowBatchBegin();

	POSITION pos = m_lstMainTimeSales.GetTailPosition();
	int32 iRowCount = 0;
	while ( pos != NULL )
	{
		T_MainTimeSale *pMTS = m_lstMainTimeSales.GetPrev(pos);
		ASSERT( NULL != pMTS );
		if ( IsMerchInCurTab(pMTS->m_pMerch) )
		{
			if ( iRowCount >= KIDefaultMonitorCount )		// ֻ����һ����������
			{
				break;
			}
			int32 iNewRow = m_GridCtrl.InsertRowBatchForOne(_T("   "), iRowCount ==0? -1 : 0);
			UpdateGridRow(iNewRow, *pMTS);
			iRowCount++;
		}
		//else
		//{
		//	TRACE(_T("%d-%s-%s\r\n"), pMTS->m_pMerch->m_MerchInfo.m_iMarketId, pMTS->m_pMerch->m_MerchInfo.m_StrMerchCode, pMTS->m_pMerch->m_MerchInfo.m_StrMerchCnName);
		//}
	}
	m_GridCtrl.InsertRowBatchEnd();

	if ( m_GridCtrl.IsValid(cellFocus) )
	{
		m_GridCtrl.SetFocusCell(cellFocus); // ��֤�Ƿ�Ҫselect
	}
	if ( m_GridCtrl.IsValid(cellRange) )
	{
		m_GridCtrl.SetSelectedRange(cellRange);
	}

	if ( m_GridCtrl.GetRowCount() > 0 )
	{
		m_GridCtrl.EnsureVisible(m_GridCtrl.GetRowCount()-1, 0);
	}

	//SetRowHeightAccordingFont();
	SetColWidthAccordingFont();
	m_GridCtrl.Refresh();
}

void CIoViewMainTimeSale::OnDoInitialize()
{
	RequestInitialData();

	RequestTabSpecialData();

	m_bInitialized = true;
}

void CIoViewMainTimeSale::RecalcLayout()
{
// 	if ( !CPluginFuncRight::Instance().IsUserHasRight(CPluginFuncRight::FuncMainTimeSaleMonitor, false) )
// 	{
// 		// ���߱�Ȩ�ޣ�ȫ������
// 		m_RectBtn.SetRectEmpty();
// 		for ( int i=0; i < m_aTitleBtns.GetSize() ; i++ )
// 		{
// 			m_aTitleBtns[i].m_RectBtn.SetRectEmpty();
// 		}
// 		m_GridCtrl.MoveWindow(CRect(0,0,0,0));
// 		return;
// 	}
	CRect rcClient;
	GetClientRect(rcClient);

	CClientDC dc(this);
	CFont *pFontOld = dc.SelectObject(GetIoViewFontObject(ESFSmall));
	CSize sizeText = dc.GetTextExtent(_T("�и�"));
	dc.SelectObject(pFontOld);
	m_RectBtn = rcClient;
	m_RectBtn.bottom = m_RectBtn.top + sizeText.cy + 14;

	const int32 iBtnWidth = sizeText.cx + 10;
	for ( int i=0; i < m_aTitleBtns.GetSize() ; i++ )
	{
		CRect &rcBtn = m_aTitleBtns[i].m_RectBtn;
		rcBtn.left	 = m_RectBtn.left + i*iBtnWidth;
		rcBtn.right  = rcBtn.left + iBtnWidth;
		rcBtn.top	 = m_RectBtn.top;
		rcBtn.bottom = m_RectBtn.bottom;
	}
	
	rcClient.top = m_RectBtn.bottom;
//	CRect rcOld(0,0,0,0);
//	m_GridCtrl.GetClientRect(rcOld);
	m_GridCtrl.MoveWindow(rcClient);
	//if ( rcOld.Width() < rcClient.Width() )
	{
		SetColWidthAccordingFont();
		if ( m_GridCtrl.GetRowCount() > 0 )
		{
			m_GridCtrl.EnsureBottomRightCell(m_GridCtrl.GetRowCount()-1, m_GridCtrl.GetColumnCount()-1);	// ����಻�ܹ���
			m_GridCtrl.EnsureVisible(m_GridCtrl.GetRowCount()-1, 0);
		}
	}
	m_GridCtrl.Refresh();
}

void CIoViewMainTimeSale::OpenTab( int32 iTabPress )
{
	if ( iTabPress >=0 && iTabPress < m_aTitleBtns.GetSize() )
	{
		if ( m_aTitleBtns[iTabPress].m_iBlockId != INT_MIN )
		{
			m_iCurTab = iTabPress;
		}
	}

	RequestTabSpecialData();		// ����tab������Ҫ�ĳ�ʼ����

	UpdateTableAllContent();

	Invalidate();

	// ��䵱ǰ��Ʒ���� - Ӧ���������block���ݣ������ݻ�������
}

void CIoViewMainTimeSale::OnLButtonDown( UINT nFlags, CPoint point )
{
	int i=0;
	for ( i = 0; i < m_aTitleBtns.GetSize(); i++ )	// ���һ��Ϊ����
	{
		if ( m_aTitleBtns[i].m_RectBtn.PtInRect(point) )
		{
			if ( m_aTitleBtns[i].m_iBlockId != INT_MIN )
			{
				OpenTab(i);
			}
			else
			{
				// ����
				CDlgMainTimeSale::ShowPopupMainTimeSale(this);
			}
			break;
		}
	}
	
	CIoViewBase::OnLButtonDown(nFlags, point);
}

bool32 CIoViewMainTimeSale::FromXml( TiXmlElement *pTiXmlElement )
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
	
	SetFontsFromXml(pTiXmlElement);
	SetColorsFromXml(pTiXmlElement);

	pcAttrValue = pTiXmlElement->Attribute(KXmlStrCurTabKey);
	if ( NULL != pcAttrValue )
	{
		OpenTab(atoi(pcAttrValue));
	}
	
	return true;
}

CString CIoViewMainTimeSale::ToXml()
{	
	//
	CString StrThis;
	
	StrThis.Format( L"<%s %s=\"%s\" %s=\"%s\" %s=\"%d\" ", 
		CString(GetXmlElementValue()).GetBuffer(),
		CString(GetXmlElementAttrIoViewType()).GetBuffer(),	CIoViewManager::GetIoViewString(this).GetBuffer(),
		CString(GetXmlElementAttrShowTabName()).GetBuffer(), m_StrTabShowName.GetBuffer(),
		CString(KXmlStrCurTabKey).GetBuffer(), m_iCurTab
		);
	
	CString StrFace;
	StrFace  = SaveColorsToXml();
	StrFace += SaveFontsToXml();
	
	StrThis += StrFace;
	StrThis += L">\n";
	
	
	StrThis += L"</";
	StrThis += CString(GetXmlElementValue());
	StrThis += L">\n";
	return StrThis;
}

CString CIoViewMainTimeSale::GetDefaultXML()
{
	CString StrThis;
	// 
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" >\n", 
		CString(GetXmlElementValue()).GetBuffer(), 
		CString(GetXmlElementAttrIoViewType()).GetBuffer(), 
		CIoViewManager::GetIoViewString(this).GetBuffer(),
		CString(GetXmlElementAttrMerchCode()).GetBuffer(),
		L"",
		CString(GetXmlElementAttrMarketId()).GetBuffer(), 
		L"-1");
	
	//
	StrThis += L"</";
	StrThis += CString(GetXmlElementValue());
	StrThis += L">\n";
	
	return StrThis;
}

void CIoViewMainTimeSale::OnConfigInitialize( E_InitializeNotifyType eInitializeType )
{
	InitializeBtnId();
}

void CIoViewMainTimeSale::OnBlockConfigChange( int32 iBlockId, E_BlockNotifyType eNotifyType )
{
	// ��ʱ������	
	RequestTabSpecialData();		// �û������� - �п���������µ����ݵȣ����������ݳ���һ��ֵ��ʵ����ǰ�����ݶ����������
	// ��Ҫ�����ز����Է�Ӧ�仯 �Ժ�������ֱ��Invalidate
	//Invalidate(TRUE);

	// ������ʾ
	for ( int i=0; i < m_GridCtrl.GetRowCount() ; i++ )
	{
		T_MainTimeSale *pMTS = (T_MainTimeSale *)m_GridCtrl.GetItemData(i, 0);
		if ( NULL != pMTS )
		{
			UpdateGridRow(i, *pMTS);
		}
	}
}

void CIoViewMainTimeSale::InitializeBtnId()
{
	// ����A 300 �鿴id�Ƿ��б仯
	ASSERT( CBlockConfig::Instance()->IsInitialized() );
	CBlockConfig::BlockArray aBlocks;
	
	// ����A������Լ��-1
	{
		m_aTitleBtns[0].m_iBlockId = -1;
	}
	CBlockConfig::Instance()->GetBlocksByName(_T("����300"), aBlocks);	// �᲻���������֣���
	ASSERT( aBlocks.GetSize() == 1 );
	if ( aBlocks.GetSize() > 0 )
	{
		m_aTitleBtns[1].m_iBlockId = aBlocks[0]->m_blockInfo.m_iBlockId;
	}
	if ( CBlockConfig::Instance()->GetUserBlockCollection() != NULL )
	{
		m_aTitleBtns[2].m_iBlockId = CBlockConfig::Instance()->GetUserBlockCollection()->m_iBlockCollectionId;	// �Զ���ʵ�����ǰ�鼯��
	}
}

bool32 CIoViewMainTimeSale::GetCurTab( OUT int32 &iCurTab, OUT int32 &iBlockId )
{
	if ( m_iCurTab >=0 && m_iCurTab <= 3 )		// ���� 300 ��ѡ ��ǰ
	{
		iCurTab = m_iCurTab;
		iBlockId = m_aTitleBtns[m_iCurTab].m_iBlockId;
		return true;
	}
	return false;
}

bool32 CIoViewMainTimeSale::IsMerchInCurTab( CMerch *pMerch )
{
	if ( pMerch == NULL )
	{
		return false;
	}

	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	if ( NULL == pMainFrame )
	{
		return false;
	}


	int32 iCurTab, iBlockId;
	if ( GetCurTab(iCurTab, iBlockId) )
	{
		MerchArray aMerchs;		// �򵥴���
		switch (iCurTab)
		{
		case BtnAll:		// ���� - ����ֱ�ӷ���true
			return true;

		case Btn300:		// 300
			{
				CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(iBlockId);
				if ( NULL != pBlock )
				{
					aMerchs.Copy(pBlock->m_blockInfo.m_aSubMerchs);
				}
			}
			break;
		case BtnUser:		// �Զ���
			{
				CBlockConfig::BlockArray aBlocks;
				CBlockConfig::Instance()->GetUserBlocks(aBlocks);
				for ( int i=0; i < aBlocks.GetSize() ; i++ )
				{
					aMerchs.Append(aBlocks[i]->m_blockInfo.m_aSubMerchs);
				}
			}
			break;
		case BtnCur:		// ��ǰ - ѡ��viewdata�����е�ioview��
			{
				if ( NULL != m_pAbsCenterManager )
				{
					for ( int i=0; i < pMainFrame->m_IoViewsPtr.GetSize() ; i++ )
					{
						CIoViewBase *pIoView = pMainFrame->m_IoViewsPtr[i];
						if ( NULL != pIoView )
						{
							CMerch *pMerchXml = pIoView->m_pMerchXml;
							if ( NULL != pMerchXml )
							{
								aMerchs.Add(pMerchXml);
							}
						}
					}
				}
			}
			break;
		default:
			ASSERT( 0 );
			// NULL
		}
		for ( int i=0; i < aMerchs.GetSize() ; i++ )
		{
			if ( aMerchs[i] == pMerch )
			{
				return true;
			}
		}
	}
	return false;
}

void CIoViewMainTimeSale::OnMainTimeSaleResp( const CMmiRespMainMonitor *pResp )
{
	// ʱ������
	if ( NULL == pResp || NULL == m_pAbsCenterManager)
	{
		return;
	}	
	
	// ����ʲô���ݣ�ֻҪ��û�еģ�ȫ������ - ��Ҫ���Ч��
	if ( pResp->m_eCommTypePlugIn == ECTPIRespMainMonitor )	// ��ʷ����
	{
		bool32 bUpdated = false;
		for ( mapMainMonitor::const_iterator it = pResp->m_mapMainMonitorData.begin() ; it != pResp->m_mapMainMonitorData.end(); it++ )
		{
			CMerch *pMerch;
			if ( !m_pAbsCenterManager->GetMerchManager().FindMerch(it->m_MerchKey.m_StrMerchCode, it->m_MerchKey.m_iMarketId,  pMerch) )
			{
				continue;
			}
			for ( MonitorArray::const_iterator itSub=it->m_aMainMonitor.begin(); itSub != it->m_aMainMonitor.end(); itSub++ )
			{
				bUpdated = InsertNewMainTimeSale(pMerch, *itSub, false) || bUpdated;
				//CTime Time(itSub->m_Time);
				//TRACE(_T("   %02d:%02d:%02d\n"), Time.GetHour(), Time.GetMinute(), Time.GetSecond());
			}

			//TRACE(_T("[%d]%d-%s-%s\r\n"), pResp->m_mapMainMonitorData.size(), it->m_MerchKey.m_iMarketId, it->m_MerchKey.m_StrMerchCode, pMerch->m_MerchInfo.m_StrMerchCnName);
		}

		if ( bUpdated )
		{
			UpdateTableAllContent();	// ��ʾ����

			RequestPushViewData(false);	// ��������
		}
	}
	else if ( pResp->m_eCommTypePlugIn == ECTPIRespAddPushMainMonitor )	// ��������
	{
		bool32 bUpdate = false;
		CCellID cellFocus = m_GridCtrl.GetFocusCell();
		T_MainTimeSale *pMTSSel = NULL;
		if ( m_GridCtrl.IsValid(cellFocus) )
		{
			pMTSSel = (T_MainTimeSale *)m_GridCtrl.GetItemData(cellFocus.row, 0);
		}

		for ( mapMainMonitor::const_iterator it = pResp->m_mapMainMonitorData.begin() ; it != pResp->m_mapMainMonitorData.end(); it++ )
		{
			CMerch *pMerch;
			if ( !m_pAbsCenterManager->GetMerchManager().FindMerch(it->m_MerchKey.m_StrMerchCode, it->m_MerchKey.m_iMarketId,  pMerch) )
			{
				continue;
			}
			for ( MonitorArray::const_iterator itSub=it->m_aMainMonitor.begin(); itSub != it->m_aMainMonitor.end(); itSub++ )
			{
				bUpdate = InsertNewMainTimeSale(pMerch, *itSub, true) || bUpdate;
				//TRACE(_T("MainMonitor time: %d\r\n"), itSub->m_Time);
			}
		}
		
		DeleteRedundantHead(true);		// ����������Ϣ 
		
		if ( pMTSSel != NULL )
		{
			for ( int i=0; i < m_GridCtrl.GetRowCount() ; i++ )
			{
				if ( pMTSSel == (T_MainTimeSale *)m_GridCtrl.GetItemData(i, 0) )
				{
					m_GridCtrl.SetFocusCell(i, cellFocus.col);
					m_GridCtrl.SetSelectedRange(i, 0, i, m_GridCtrl.GetColumnCount()-1, TRUE, TRUE);
				}
			}
		}
		m_GridCtrl.EnsureBottomRightCell(m_GridCtrl.GetRowCount()-1, m_GridCtrl.GetColumnCount()-1);
		m_GridCtrl.EnsureVisible(m_GridCtrl.GetRowCount()-1, 0);
	}
}

//lint --e{429}
bool32 CIoViewMainTimeSale::InsertNewMainTimeSale(CMerch *pMerch,  const T_RespMainMonitor &mainTM , bool32 bAddToGrid/* = false*/)
{
	// ����ʱ�����ҵ���Ӧ��λ�� - ������:)
	T_MainTimeSale *pMainTimeSale = new T_MainTimeSale;
	pMainTimeSale->m_pMerch = pMerch;
	pMainTimeSale->m_mainMonitor = mainTM;
	POSITION pos = m_lstMainTimeSales.GetHeadPosition();
	int i = 0;
	for (   ; pos != NULL ; m_lstMainTimeSales.GetNext(pos), i++ )
	{
		const T_MainTimeSale &mts = *m_lstMainTimeSales.GetAt(pos);
		
		if ( mts.m_mainMonitor.m_Time < mainTM.m_Time )		// ԭ�������ݱ����ڵ�С
		{
			continue;
		}
		else if ( mts.m_mainMonitor.m_Time == mainTM.m_Time )	// ��
		{
			if ( mts.m_pMerch == pMerch && mts.m_mainMonitor == mainTM )	// �ظ����ݲ���ӣ�ʱ�������������
			{
				DEL(pMainTimeSale);		// �ͷ�				
				return false;		// ��ͬ��
			}
		}
		else	// ����
		{
			// ��������Ѿ��������ǰ��Ӧ������������������
			if ( 0 == i && m_lstMainTimeSales.GetCount() >= KIDefaultMaxMonitorCount )
			{
				DEL(pMainTimeSale);
				return false;
			}

			m_lstMainTimeSales.InsertBefore(pos, pMainTimeSale);
			if ( bAddToGrid /*&& !m_bInitialized*/ )
			{
				// ���������һ��һ��������Ӧ��������ǰ�����ݵ�
				InsertGridRow(i, *pMainTimeSale);
			}
			return true;
		}
	}

	// ��ӵ����
	m_lstMainTimeSales.AddTail(pMainTimeSale);
	if ( bAddToGrid )
	{
		InsertGridRow(i, *pMainTimeSale);
	}

	// �õ����������
	return true;
}

int32 CIoViewMainTimeSale::InsertGridRow( int /*iRow*/, const T_MainTimeSale &mts , bool32 bDrawRow/* =false*/ )
{
	// ���ݵ�ǰ��ѡ�����ݣ����ʵ�λ����� - Ӧ��ֻ��ӵ���β��
	if ( IsMerchInCurTab(mts.m_pMerch) )
	{
		int i = 0;
		for ( i=0; i < m_GridCtrl.GetRowCount() ; i++ )
		{
			T_MainTimeSale *pMTS = (T_MainTimeSale *)m_GridCtrl.GetItemData(i, 0);
			ASSERT( NULL != CheckMTS(pMTS) );
			if ( NULL != pMTS )
			{
				if ( pMTS->m_mainMonitor.m_Time > mts.m_mainMonitor.m_Time )
				{
					break;
				}
			}
		}
		if ( i >= m_GridCtrl.GetRowCount() )
		{
			i = -1;
		}
//		CCellID cellFocus = m_GridCtrl.GetFocusCell();
		int32 iNewRow = m_GridCtrl.InsertRow(_T("   "), i);
		UpdateGridRow(iNewRow, mts);
		m_GridCtrl.EnsureVisible(m_GridCtrl.GetRowCount()-1, 0);
// 		if ( m_GridCtrl.IsValid(cellFocus) )
// 		{
// 			m_GridCtrl.SetFocusCell(cellFocus);
// 			//m_GridCtrl.SetSelectedRange(cellFocus.row, 0, cellFocus.row, m_GridCtrl.GetColumnCount()-1, TRUE, TRUE);
// 			m_GridCtrl.RedrawRow(cellFocus.row);
// 		}
		if ( bDrawRow )
		{
			m_GridCtrl.RedrawRow(iNewRow);
		}
		return iNewRow;
	}
	return -1;
}

void CIoViewMainTimeSale::DeleteRedundantHead( bool32 bDeleteInGrid /*= false*/ )
{
	bool32 bDeleted = false;
	//CCellID cellFocus = m_GridCtrl.GetFocusCell();
	while ( m_lstMainTimeSales.GetCount() > KIDefaultMaxMonitorCount )	// �ܵ�����������
	{
		T_MainTimeSale *pMTS = m_lstMainTimeSales.RemoveHead();
		for ( int i=0; i < m_GridCtrl.GetRowCount() ; i++ )
		{
			if ( m_GridCtrl.GetItemData(i, 0) == (LPARAM)pMTS )
			{
				m_GridCtrl.DeleteRow(i);
				bDeleted = true;
				break;
			}
		}
		delete pMTS;
	}

	if ( bDeleteInGrid )
	{
		while ( m_GridCtrl.GetRowCount() > KIDefaultMonitorCount )	// �����������
		{
			m_GridCtrl.DeleteRow(0);
			bDeleted = true;
		}
	}

	if ( bDeleted && bDeleteInGrid )
	{
// 		if ( m_GridCtrl.IsValid(cellFocus) )
// 		{
// 			m_GridCtrl.SetFocusCell(cellFocus);
// 		}
		m_GridCtrl.Refresh();
	}
}

void CIoViewMainTimeSale::UpdateGridRow( int iRow, const T_MainTimeSale &mts )
{
	if ( !m_GridCtrl.IsValid(iRow, 0) )
	{
		return;
	}

	LOGFONT fontChinese;
	memset(&fontChinese, 0, sizeof(fontChinese));
	_tcscpy(fontChinese.lfFaceName, gFontFactory.GetExistFontName(_T("Arial")));///
	fontChinese.lfHeight  = -15;

	tm t = {0};
	CGmtTime	GmtTime(mts.m_mainMonitor.m_Time);
	GmtTime.GetLocalTm(&t);
	CString StrTime;
	COLORREF clr = GetIoViewColor(ESCRise);
	if ( mts.m_mainMonitor.m_uBuy != 0 )
	{
		clr = GetIoViewColor(ESCFall);
	}
	COLORREF clrMerch = clr;
	T_Block *pBlock = CUserBlockManager::Instance()->GetBlock(mts.m_pMerch);
	if ( NULL != pBlock && COLORNOTCOUSTOM != pBlock->m_clrBlock )
	{
		clrMerch = pBlock->m_clrBlock;
	}
	StrTime.Format(_T("%02d:%02d"), t.tm_hour, t.tm_min);
	m_GridCtrl.SetItemText(iRow, 0, StrTime);
	m_GridCtrl.SetItemFont(iRow, 0, &fontChinese);
	m_GridCtrl.SetItemFgColour(iRow, 0, clrMerch);
	m_GridCtrl.SetItemTextPadding(iRow, 0, CRect(5,0,0,0));

	ASSERT( iRow != -1 );
	m_GridCtrl.SetItemText(iRow, 1, mts.m_pMerch->m_MerchInfo.m_StrMerchCnName);
	m_GridCtrl.SetItemFont(iRow, 1, &fontChinese);
	m_GridCtrl.SetItemFgColour(iRow, 1, clrMerch);
	
	CString StrFlag;
	StrFlag = mts.m_mainMonitor.m_uBuy != 0 ? _T("��������") : _T("��������");
	m_GridCtrl.SetItemText(iRow, 2, StrFlag);
	m_GridCtrl.SetItemFont(iRow, 2, &fontChinese);
	m_GridCtrl.SetItemFgColour(iRow, 2, clr);
	
	CString StrPrice;
	StrPrice.Format(_T("%0.2f/%0.0f"), mts.m_mainMonitor.m_fPrice, mts.m_mainMonitor.m_fCounts / 100);
	m_GridCtrl.SetItemText(iRow, 3, StrPrice);
	m_GridCtrl.SetItemFont(iRow, 3, &fontChinese);
	m_GridCtrl.SetItemFgColour(iRow, 3, clr);

	m_GridCtrl.SetItemData(iRow, 0, (LPARAM)&mts);


	for (int j =0; j < m_GridCtrl.GetColumnCount(); ++j)
	{				
		CGridCellBase* pCell = m_GridCtrl.GetCell(iRow, j);
		if (NULL != pCell)
		{
			if (0 == j)
			{
				pCell->SetTextPadding(CRect(15,0,0,0));
			}

			if (m_GridCtrl.GetColumnCount()- 1 == j)
			{
				pCell->SetTextPadding(CRect(0,0,15,0));
			}
		}
	}
}

void CIoViewMainTimeSale::RequestData( CMmiCommBase &req )
{
	//if ( IsWindowVisible() && NULL != m_pAbsCenterManager )
	{
		//m_pAbsCenterManager->RequestViewData(&req);
		DoRequestViewData(req);
	}
}

CIoViewMainTimeSale::T_MainTimeSale  * CIoViewMainTimeSale::CheckMTS( T_MainTimeSale *pMTS ) const
{
	POSITION pos = m_lstMainTimeSales.GetHeadPosition();
	while ( pos != NULL )
	{
		if ( pMTS == m_lstMainTimeSales.GetNext(pos) )
		{
			return pMTS;
		}
	}
	return NULL;
}

void CIoViewMainTimeSale::LockRedraw()
{
	if( m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = true;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
	::SendMessage(m_GridCtrl.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
}

void CIoViewMainTimeSale::UnLockRedraw()
{
	if ( !m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = false;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
	::SendMessage(m_GridCtrl.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);

	Invalidate();
}

void CIoViewMainTimeSale::OnVDataPluginResp( const CMmiCommBase *pResp )
{
	if ( NULL == pResp )
	{
		return;
	}
	
	if ( pResp->m_eCommType != ECTRespPlugIn )
	{
		return;
	}

	// ��ͼ���ɼ���ʱ��ˢ�½���	--chenfj
	if(!IsNowCanRefreshUI())
	{
		return;
	}

	// ��Ȩ�޲����ո�����
// 	if ( !CPluginFuncRight::Instance().IsUserHasRight(CPluginFuncRight::FuncMainTimeSaleMonitor, false) )
// 	{
// 		return;
// 	}
	
	const CMmiCommBasePlugIn *pRespPlugin = (CMmiCommBasePlugIn *)pResp;
	if ( pRespPlugin->m_eCommTypePlugIn == ECTPIRespMainMonitor
		|| pRespPlugin->m_eCommTypePlugIn == ECTPIRespAddPushMainMonitor )
	{
		OnMainTimeSaleResp((const CMmiRespMainMonitor *)pResp);
	}
}

bool32 CIoViewMainTimeSale::GetCurBlockMerchs( OUT MerchArray &aMerchs )
{
	int32 iTab, iBlockId;
	if ( GetCurTab(iTab, iBlockId) )
	{
		return GetBlockMerchsByTabIndex(iTab, aMerchs);
	}
	return false;
}

bool32 CIoViewMainTimeSale::GetBlockMerchsByTabIndex( int32 iTab, OUT MerchArray &aMerchs )
{
	aMerchs.RemoveAll();

	CBlockLikeMarket *pBlockDef = CBlockConfig::Instance()->GetDefaultMarketClassBlock();
	if ( NULL == pBlockDef )	// �޻���a����̸
	{
		return false;
	}
	switch (iTab)
	{
	case BtnAll:		// ���� - ����A - ����Ҫ�ṩ���ַ���so�����ṩ
		ASSERT( 0 );
		break;
	case Btn300:		// ����300 - ����Ҫ ���ṩ
		ASSERT( 0 );
		break;
	case BtnUser:		// ��ѡ - 
		{
			CBlockConfig::BlockArray aBlocks;
			CBlockConfig::Instance()->GetUserBlocks(aBlocks);
			aMerchs.SetSize(0, 100);
			int i = 0;
			for ( i=0; i < aBlocks.GetSize() ; i++ )
			{
				aMerchs.Append(aBlocks[i]->m_blockInfo.m_aSubMerchs);
			}
			// ���˵�����A����û�е���Ʒ
			for ( i = aMerchs.GetSize()-1; i >= 0 ; i-- )	// Ч�ʺܵͣ����Ľ�
			{
				if ( !pBlockDef->IsMerchInBlock(aMerchs[i]) )
				{
					aMerchs.RemoveAt(i);	// ���ǻ���A�ģ����
				}
			}
		}
		break;
	case BtnCur:		// ��ǰ - ��view Data��
		{
			CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
			if ( NULL == pMainFrame )
			{
				break;
			}

			for (int  i=0; i < pMainFrame->m_IoViewsPtr.GetSize() ; i++ )
			{
				if ( pMainFrame->m_IoViewsPtr[i] != NULL && pMainFrame->m_IoViewsPtr[i]->m_pMerchXml != NULL )
				{
					aMerchs.Add(pMainFrame->m_IoViewsPtr[i]->m_pMerchXml);
				}
			}
			// ���˵�����A����û�е���Ʒ
			for (int  i = aMerchs.GetSize()-1; i >= 0 ; i-- )	// Ч�ʺܵͣ����Ľ�
			{
				if ( !pBlockDef->IsMerchInBlock(aMerchs[i]) )
				{
					aMerchs.RemoveAt(i);	// ���ǻ���A�ģ����
				}
			}
		}
		break;
	}
	return aMerchs.GetSize() > 0;
}

void CIoViewMainTimeSale::RequestUserStockData()
{
	int32 iTab, iBlockId;
	MerchArray aMerchs;
	CBlockLikeMarket *pBlockDef = CBlockConfig::Instance()->GetDefaultMarketClassBlock();
	if ( NULL == pBlockDef )
	{
		return;		// û��鶨�壬��������
	}
	const int32 iMaxCount = 100;

	if ( GetCurTab(iTab, iBlockId) && BtnUser == iTab && GetCurBlockMerchs(aMerchs) && aMerchs.GetSize() > 0 )	// ��ѡ
	{
		CMmiReqMainMonitor reqUser;
		for ( int i=0; i < aMerchs.GetSize() && i <= iMaxCount ; i++ )
		{
			if ( reqUser.m_StrMerchCode.IsEmpty() )
			{
				reqUser.m_iCount = 10;	// �Զ���������10��
				reqUser.m_StrMerchCode = aMerchs[i]->m_MerchInfo.m_StrMerchCode;
				reqUser.m_iBlockId = aMerchs[i]->m_MerchInfo.m_iMarketId;
			}
			else
			{
				T_ReqMainMonitor mm;
				mm.m_iBlockId = aMerchs[i]->m_MerchInfo.m_iMarketId;
				mm.m_iCount = 10;
				mm.m_StrMerchCode = aMerchs[i]->m_MerchInfo.m_StrMerchCode;
				reqUser.m_aReqMore.Add(mm);
			}
		}
		RequestData(reqUser);
	}
}

void CIoViewMainTimeSale::RequestCurrentStockData()
{
	int32 iTab, iBlockId;
	MerchArray aMerchs;
	CBlockLikeMarket *pBlockDef = CBlockConfig::Instance()->GetDefaultMarketClassBlock();
	if ( NULL == pBlockDef )
	{
		return;		// û��鶨�壬��������
	}
	const int32 iMaxCount = 50;

	if ( GetCurTab(iTab, iBlockId) && BtnCur == iTab && GetCurBlockMerchs(aMerchs) && aMerchs.GetSize() > 0 )	// ��ѡ
	{
		CMmiReqMainMonitor reqCur;
		reqCur.m_iBlockId = pBlockDef->m_blockInfo.m_iBlockId;
		for ( int i=0; i < aMerchs.GetSize() && i <= iMaxCount ; i++ )
		{
			if ( reqCur.m_StrMerchCode.IsEmpty() )
			{
				reqCur.m_iCount = 20;	// ������20��
				reqCur.m_StrMerchCode = aMerchs[i]->m_MerchInfo.m_StrMerchCode;
				reqCur.m_iBlockId = aMerchs[i]->m_MerchInfo.m_iMarketId;
			}
			else
			{
				T_ReqMainMonitor mm;
				mm.m_iBlockId = aMerchs[i]->m_MerchInfo.m_iMarketId;
				mm.m_iCount = 20;
				mm.m_StrMerchCode = aMerchs[i]->m_MerchInfo.m_StrMerchCode;
				reqCur.m_aReqMore.Add(mm);
			}
		}
		RequestData(reqCur);
	}
}

void CIoViewMainTimeSale::RequestTabSpecialData()
{
	int32 iTab, iBlockId;
	if ( GetCurTab(iTab, iBlockId) )
	{
		switch (iTab)
		{
		case BtnUser:
			RequestUserStockData();
			break;
		case BtnCur:
			RequestCurrentStockData();
			break;
		}
	}
}

CMerch * CIoViewMainTimeSale::GetMerchXml()
{
	CCellRange rangeSel = m_GridCtrl.GetSelectedCellRange();
	T_MainTimeSale *pMTS = NULL;
	if ( rangeSel.IsValid() && (pMTS = (T_MainTimeSale *)m_GridCtrl.GetItemData(rangeSel.GetMinRow(), 0)) != NULL )
	{
		return pMTS->m_pMerch;
	}
	return CIoViewBase::GetMerchXml();
}

bool32 CIoViewMainTimeSale::GetStdMenuEnable( MSG* pMsg )
{
	//return !m_bParentIsDialog;	// �Ի�������˵�
	return false;
}

void CIoViewMainTimeSale::GetAttendMarketDataArray( OUT AttendMarketDataArray &aAttends )
{
	aAttends.RemoveAll();
	CBlockLikeMarket *pBlock = CBlockConfig::Instance()->GetDefaultMarketClassBlock();;
	T_AttendMarketData data;
	if ( NULL != pBlock )
	{
		// ���ܵ�ǰ���ĸ���ť���������л���a���г�
		// �������е���Ʒ�ҳ����е��г�- -������ĳ���
		CBlockInfo::E_BlockType eBlockType = pBlock->m_blockInfo.GetBlockType();
		switch ( eBlockType )
		{
		case CBlockInfo::typeMarketClassBlock:
		case CBlockInfo::typeNormalBlock:
		case CBlockInfo::typeUserBlock:
		case CBlockInfo::typeHSASubBlock:
			{
				// ȫ��Ʒ����
				typedef CMap<int32, int32, int32, int32> MarketMap;
				MarketMap ms;
				POSITION pos = pBlock->m_blockInfo.m_mapSubMerchMarkets.GetStartPosition();
				while ( NULL != pos )
				{
					int32 iMarket, iValue;
					pBlock->m_blockInfo.m_mapSubMerchMarkets.GetNextAssoc(pos, iMarket, iValue);
					
					data.m_iMarketId = iMarket;
					data.m_iEDSTypes = EDSTGeneral;
					
					aAttends.Add(data);
				}
			}
			break;
		default:
			ASSERT( 0 );
		}
	}
}

void CIoViewMainTimeSale::UpdateAllGridRow()
{
	const int32 iCount = m_GridCtrl.GetRowCount();
	for ( int32 i=0; i < iCount ; i++ )
	{
		T_MainTimeSale *pMTS = (T_MainTimeSale *)m_GridCtrl.GetItemData(i, 0);
		if ( NULL != pMTS )
		{
			UpdateGridRow(i, *pMTS);
		}
	}
}

void CIoViewMainTimeSale::OnContextMenu( CWnd* pWnd, CPoint pos )
{
	if ( m_RectBtn.IsRectEmpty() )
	{
		DoShowStdPopupMenu();	// ��ʱΪ���������gridû����ʾ
	}
}

//////////////////////////////////////////////////////////////////////////

CIoViewMainTimeSale::T_TabBtn::T_TabBtn( const T_TabBtn &tab )
{
	m_RectBtn = tab.m_RectBtn;
	m_StrName = tab.m_StrName;
	m_iBlockId = tab.m_iBlockId;
	m_bNeedReqPrivateData = tab.m_bNeedReqPrivateData;
}

const CIoViewMainTimeSale::T_TabBtn & CIoViewMainTimeSale::T_TabBtn::operator=( const T_TabBtn &tab )
{
	if ( &tab == this )
	{
		return tab;
	}

	m_RectBtn = tab.m_RectBtn;
	m_StrName = tab.m_StrName;
	m_iBlockId = tab.m_iBlockId;
	m_bNeedReqPrivateData = tab.m_bNeedReqPrivateData;

	return *this;
}
