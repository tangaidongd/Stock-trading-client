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
#include "IoViewStarry.h"
#include "IoViewBlockReport.h"
#include "IoViewReportRank.h"
#include "LogFunctionTime.h"
#include "DlgTimeSaleStatistic.h"
#include "MarkManager.h"

#include <set>
using std::set;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
//    "fatal error C1001: INTERNAL COMPILER ERROR"

const char* KStrViewBlockReportTabId			= "CURCLASSID";
const char* KStrViewBlockReportSortHeader		= "SORTHEADER";
const char* KStrViewBlockReportSortHeaderDesc	= "SORTHEADER_DESC";
const char* KStrViewBlockReportShowTitle		= "SHOWTITLE";
const char* KStrViewBlockReportUpdateKline		= "UPDATEKLINE";    //�л���Ʒ�Ƿ����K������
const char* KStrViewBlockReportUpdateTrend		= "UPDATETREND";    //�л���Ʒ�Ƿ���·�ʱ����
const char* KStrViewBlockReportUpdateReport		= "UPDATEREPORT";   //�л���Ʒ�Ƿ���±��۱�����
const char* KStrViewReportShowGridVertScrool11	= "SHOWGRIDVERTSCROOL";

const int32 KRequestSortDataTimerId				= 100004;			// �������������ͣ�Ƶ����
const int32 KRequestSortDataTimerPeriod			= 1000 * 60;		// ���ڰ�����ݼ��㲢��Ƶ����so���㷢

const int32 KUpdatePushMerchsTimerId			= 100005;			// ÿ�� 5 ����, ���¼���һ�ε�ǰ��������Ʒ
const int32 KTimerPeriodPushMerchs				= 1000 * 60;

const int32 KWatchTimeChangeTimerId				= 100006;			// ÿ�� n ����, ���²鿴ʱ���Ƿ����
const int32 KWatchTimeChangeTimerPeriod			= 60*1000;					

const int32 KChangeVisibleBlocksTimerId			= 100007;
const int32 KChangeVisibleBlocksTimerPeriod		= 260;

const int32 KStartChooseBlockTimerId			= 100008;
const int32 KStartChooseBlockTimerPeriod		= 1000;


#define  ID_GRID_CTRL         10205

	
IMPLEMENT_DYNCREATE(CIoViewBlockReport, CIoViewBase)
CIoViewBlockReport *CIoViewBlockReport::m_spThis = NULL;

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewBlockReport, CIoViewBase)
//{{AFX_MSG_MAP(CIoViewBlockReport)
ON_WM_PAINT()
ON_WM_CREATE()
ON_WM_SIZE()
ON_WM_TIMER()
ON_WM_HSCROLL()
ON_WM_VSCROLL()
ON_WM_CONTEXTMENU()
ON_NOTIFY(TCN_SELCHANGE, 0x9999, OnSelChange)
ON_MESSAGE_VOID(UM_BLOCK_DOINITIALIZE, OnDoBlockReportInitialize)
ON_MESSAGE(UM_POS_CHANGED, OnScrollPosChange)
//}}AFX_MSG_MAP
ON_NOTIFY(NM_RCLICK,ID_GRID_CTRL,OnGridRButtonDown)
ON_NOTIFY(NM_DBLCLK, ID_GRID_CTRL, OnGridDblClick)
ON_NOTIFY(NM_CLICK, ID_GRID_CTRL, OnGridDblClick)
ON_NOTIFY(GVN_GETDISPINFO, ID_GRID_CTRL, OnGridGetDispInfo)
ON_NOTIFY(GVN_SELCHANGED, ID_GRID_CTRL,OnGridSelRowChanged) 
ON_NOTIFY(GVN_ODCACHEHINT, ID_GRID_CTRL, OnGridCacheHint)
END_MESSAGE_MAP()
///////////////////////////////////////////////////////////////////////////////

// ׼���Ż� fangz20100514

CIoViewBlockReport::CIoViewBlockReport()
:CIoViewBase()
{
	m_pParent		= NULL;
	m_rectClient	= CRect(-1,-1,-1,-1);
	m_RectTitle.SetRectEmpty();

	m_iSortColumn		= -1;
	m_bRequestViewSort	= false;
	
	m_aUserBlockNames.RemoveAll();

	m_iBlockClassId = T_BlockReportTab::BCAll;
	
	m_bBlockReportInitialized = false;

	m_iMaxGridVisibleRow = 0;

	m_tmDisplay.tm_year = 0;

	m_iPreTab = m_iCurTab = -1;

	m_iSortTypeFromXml = 0;
	m_iSortColumnFromXml = -1;
	m_bShowTitle = false;
	m_bUpdateKline = true;
	m_bUpdateTrend = true;
	m_bUpdateReport = true;


	m_bIsShowGridVertScorll = false;
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewBlockReport::~CIoViewBlockReport()
{
// 	if ( NULL != m_pSubject )
// 	{
// 		m_pSubject->DelObserver(this);
// 	}
	
	// MainFrame �п�ݼ�
	CMainFrame* pMainFrame =(CMainFrame*)AfxGetMainWnd();
	if ( NULL != pMainFrame && pMainFrame->m_pKBParent == this )
	{
		pMainFrame->SetHotkeyTarget(NULL);
	}	
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
void CIoViewBlockReport::OnPaint()
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
	COLORREF clrAxis = GetIoViewColor(ESCChartAxisLine);
	COLORREF clrVol = GetIoViewColor(ESCVolume2);
	COLORREF clrBlock = GetIoViewColor(ESCRise);

	if ( clrBk == CFaceScheme::Instance()->GetSysColor(ESCBackground) )
	{
		//clrBk = RGB(44, 0, 0);
	}
	else
	{
		// �Ҷ�
		CColorStep step;
		step.InitColorRange(clrBk, 32.0f, 32.0f, 32.0f);
		clrBk = step.GetColor(1);
	}

	CMemDC dc(&dcPaint, m_RectTitle);
	int32 iSaveDC = dc.SaveDC();

	CFont *pFontNormal = GetIoViewFontObject(ESFNormal);
	dc.SelectObject(pFontNormal);
	dc.FillSolidRect(m_RectTitle, clrBk);
	dc.SetBkColor(clrBk);
	dc.SetBkMode(TRANSPARENT);
	
	CRect rcDraw(m_RectTitle);
	rcDraw.left += 10;

	CString StrBlock, StrTime;
	CString StrWeekDay(_T("��һ����������"));
	T_BlockReportTab tab;
	if ( GetTabInfoByIndex(m_GuiTabWnd.GetCurtab(), tab) )
	{
		if ( tab.m_StrName.GetLength() > 2 && tab.m_StrName.Right(2) == _T("���") )
		{
			StrBlock.Format(_T("���Ű����� -- %s"), tab.m_StrName.GetBuffer());
		}
		else
		{
			StrBlock.Format(_T("���Ű����� -- %s���"), tab.m_StrName.GetBuffer());
		}
	}
	else
	{
		StrBlock = _T("���Ű�����");
	}
	
	tm &tmNowLocal = m_tmDisplay;
	StrTime.Format(_T("����: %04d-%02d-%02d, %c"), tmNowLocal.tm_year+1900, tmNowLocal.tm_mon+1, tmNowLocal.tm_mday, StrWeekDay[tmNowLocal.tm_wday%StrWeekDay.GetLength()]);
	if (m_bShowTitle)
	{
		dc.SetTextColor(clrBlock);
		dc.DrawText(StrBlock, rcDraw, DT_LEFT |DT_SINGLELINE |DT_VCENTER);
		rcDraw.left = dc.GetTextExtent(StrBlock).cx + 20;

		if ( rcDraw.Width() > 0 )
		{
			dc.SetTextColor(clrVol);
			dc.DrawText(StrTime, rcDraw, DT_LEFT |DT_SINGLELINE |DT_VCENTER);
		}

		dc.FillSolidRect(rectClient.left, rcDraw.bottom-1, rectClient.Width(), 1, clrAxis);
		if ( m_bActive )
		{
			dc.FillSolidRect(3, 2, 2, 2, clrVol);
		}
	}

	CRect RectTab;
	m_GuiTabWnd.GetWindowRect(&RectTab);
	ScreenToClient(RectTab);	

	CRect RectDraw;
	RectDraw = rectClient;

	if ( 0 == m_aTabInfomations.GetSize() )
	{
		m_GuiTabWnd.ShowWindow(SW_HIDE);
		dc.FillSolidRect(&RectTab, GetIoViewColor(ESCBackground));
	}
	else
	{
		m_GuiTabWnd.ShowWindow(SW_SHOW);
	}

	RectDraw.top   = RectTab.top;
	RectDraw.left  = RectTab.right;	
	dc.FillSolidRect(&RectDraw, GetIoViewColor(ESCBackground));

	dc.MoveTo(RectTab.left, RectTab.top );
	dc.LineTo(RectDraw.right, RectTab.top );

	dc.MoveTo(RectTab.left, RectTab.bottom -1);
	dc.LineTo(RectDraw.right, RectTab.bottom -1);

	dc.RestoreDC(iSaveDC);
}

void CIoViewBlockReport::OnMouseWheel(short zDelta)
{
	
}

BOOL CIoViewBlockReport::PreTranslateMessage(MSG* pMsg)
{
	//if ( WM_KEYDOWN == pMsg->message )
	//{
	//	if ( VK_ESCAPE == pMsg->wParam )
	//	{
	//		// ����ҳ�治����esc��Ӧ
	//		// ����
	//		OnEsc();
	//		return TRUE;
	//	}			 
	//}
	//else if ( WM_MOUSEWHEEL == pMsg->message )
	//{
	//	// ���ع���
 //		short zDelta = HIWORD(pMsg->wParam);
 //		zDelta /= 120;
 //
 //		// �Լ�����
 //		OnMouseWheel(zDelta);
	//	return TRUE;
	//}

	return CIoViewBase::PreTranslateMessage(pMsg);
}

int CIoViewBlockReport::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CIoViewBase::OnCreate(lpCreateStruct);
	
	InitialIoViewFace(this);

	//����Tab ��
	m_GuiTabWnd.Create(WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,0x9999);
	//m_GuiTabWnd.SetBkGround(false,GetIoViewColor(ESCBackground),0,0);
	m_GuiTabWnd.SetBkGround(false,RGB(42,42,50),0,0);
	m_GuiTabWnd.SetTabTextColor(RGB(182,183,185), RGB(228,229,230));
	m_GuiTabWnd.SetTabBkColor(RGB(42,42,50), RGB(230,70,70));

	// m_GuiTabWnd.SetBkGround(true,GetIoViewColor(ESCBackground),IDB_GUITAB_NORMAL,IDB_GUITAB_SELECTED);
	// m_GuiTabWnd.SetImageList(IDB_TAB, 16, 16, 0x0000ff);
	m_GuiTabWnd.SetUserCB(this);
	m_GuiTabWnd.SetALingTabs(CGuiTabWnd::ALN_BOTTOM);
	/*LOGFONT *pFont = GetIoViewFont(ESFNormal);
	if (pFont)
	{
		pFont->lfHeight = -13;
		m_GuiTabWnd.SetFontByLF(*pFont);
	}*/
	

	// �����������
	m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10203);
	m_XSBVert.SetScrollRange(0, 10);

	m_XSBHorz.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10204);
	m_XSBHorz.SetScrollRange(0, 10);

	/*m_XSBVert.SetScrollBarLeftArrowH(-1);
	m_XSBVert.SetScrollBarRightArrowH(-1);
	m_XSBVert.SetBorderColor(GetIoViewColor(ESCBackground));

	m_XSBHorz.SetScrollBarLeftArrowH(-1);
	m_XSBHorz.SetScrollBarRightArrowH(-1);
	m_XSBHorz.SetBorderColor(GetIoViewColor(ESCBackground));*/

	// �������ݱ��
	m_GridCtrl.Create(CRect(0, 0, 0, 0), this, ID_GRID_CTRL);
// 	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetBackClr(CLR_DEFAULT);
// 	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetTextClr(CLR_DEFAULT);
	m_GridCtrl.SetTextColor(CLR_DEFAULT);
	m_GridCtrl.SetFixedTextColor(CLR_DEFAULT);
	m_GridCtrl.SetTextBkColor(CLR_DEFAULT);
	m_GridCtrl.SetBkColor(CLR_DEFAULT);
	m_GridCtrl.SetFixedBkColor(CLR_DEFAULT);
	m_GridCtrl.SetAutoHideFragmentaryCell(false);
	
	m_GridCtrl.SetDefCellWidth(60);
	m_GridCtrl.EnableBlink(FALSE);
	m_GridCtrl.SetVirtualMode(TRUE);
	m_GridCtrl.SetVirtualCompare(CompareRow);
	m_GridCtrl.SetDefaultCellType(RUNTIME_CLASS(CGridCellNormalSys));
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrl.EnablePolygonCorner(false);
	// ���ñ��ͼ��
	// m_ImageList.Create(MAKEINTRESOURCE(IDB_GRID_REPORT), 16, 1, RGB(255,255,255));



	// �����໥֮��Ĺ���
	m_XSBHorz.SetOwner(&m_GridCtrl);
	m_XSBVert.SetOwner(&m_GridCtrl);
	m_GridCtrl.SetScrollBar(&m_XSBHorz, &m_XSBVert);
	m_XSBHorz.AddMsgListener(m_hWnd);	// ����������Ϣ - ��¼������Ϣ
	m_XSBVert.AddMsgListener(m_hWnd);	// ����������Ϣ - ��ʱ������������

	// ���ñ�ͷ
	m_GridCtrl.SetHeaderSort(FALSE);
	m_GridCtrl.SetUserCB(this);

	// ��ʾ����ߵķ��
	m_GridCtrl.ShowGridLine(false); 
	m_GridCtrl.SetInitSortAscending(FALSE);

	////////////////////////////////////////////////////////////////
	m_GridCtrl.SetFixedColumnCount(2);	// �� ��
	m_GridCtrl.SetFixedRowCount(1);	
	// ��������
	LOGFONT *pFontNormal = GetIoViewFont(ESFSmall);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);

	// ��ͷ�̶������Գ�ʼ����ͷ��
	UpdateTableHeader();

	//pCell->SetDrawParentActiveParams(m_iActiveXpos, m_iActiveYpos, m_ColorActive);


	PostMessage(UM_BLOCK_DOINITIALIZE, 0, 0);

	// ���ʱ����Գ�ʼ��tab��Դ�˵ģ�����ڴ�ʱӦ�����غ��ˣ����û����Ӧ����do initialize�г�ʼ������tab
	InitializeTabs();	

	//
	SetTimer(KUpdatePushMerchsTimerId, KTimerPeriodPushMerchs, NULL);

	SetTimer(KWatchTimeChangeTimerId, KWatchTimeChangeTimerPeriod , NULL);
	SetTimer(KStartChooseBlockTimerId, KStartChooseBlockTimerPeriod , NULL);
	CGmtTime timeNow = m_pAbsCenterManager->GetServerTime();
	timeNow.GetLocalTm(&m_tmDisplay);
	WatchDayChange(false);

	// �������
	CBlockConfig::Instance()->AddListener(this);

	return 0;
}

void CIoViewBlockReport::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);

	RecalcLayout();
}

bool32	CIoViewBlockReport::IsPointInGrid()
{
	CPoint pt(0,0);
	GetCursorPos(&pt);
	CRect rt;
	m_GridCtrl.GetWindowRect(&rt);	
	if (rt.PtInRect(pt))
	{
		return TRUE;
	}
	return FALSE;
}


BOOL CIoViewBlockReport::TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	
	return FALSE;
}

void CIoViewBlockReport::OnIoViewActive()
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
	
	//
	int32 iTab;

	Invalidate(TRUE);

	if ( !TabIsValid(iTab) )
	{
		return;
	}	
}

void CIoViewBlockReport::OnIoViewDeactive()
{
	m_bActive = false;
	
	Invalidate(TRUE);
}

void CIoViewBlockReport::OnSelChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if ( m_bBlockReportInitialized )	// ��ʼ�����
	{
		TabChanged();
		InvalidateRect(m_RectTitle);
	}

	*pResult = 1;
}

//////////////////////////////////////////////////////////////////////////////////////////
//��WorkSpace��xml�ж���Blocks,����ȷ��Blocks��������������. Logic xml. user xml�д���
bool32 CIoViewBlockReport::FromXml(TiXmlElement * pTiXmlElement)
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

	// ��������
	LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);
	SetRowHeightAccordingFont();

	pcAttrValue = pTiXmlElement->Attribute(KStrViewBlockReportTabId);
	if ( NULL != pcAttrValue )
	{
		m_iBlockClassId = atoi(pcAttrValue);
	}

	pcAttrValue = pTiXmlElement->Attribute(KStrViewBlockReportSortHeader);
	if ( NULL != pcAttrValue )
	{
		m_iSortColumnFromXml = atol(pcAttrValue);	
	}

	pcAttrValue = pTiXmlElement->Attribute(KStrViewBlockReportSortHeaderDesc);
	if ( NULL != pcAttrValue && m_iSortColumnFromXml != -1 )	// ����Ҫ����Ͳ���Ҫ����ֶ���
	{
		m_iSortTypeFromXml = atol(pcAttrValue);		// ȡ���һ�ε�״̬
	}

	pcAttrValue = pTiXmlElement->Attribute(KStrViewBlockReportShowTitle);
	if ( NULL != pcAttrValue  )	// �Ƿ���ʾ��������
	{
		m_bShowTitle = atoi(pcAttrValue) != 0;		
	}

	pcAttrValue = pTiXmlElement->Attribute(KStrViewBlockReportUpdateKline);
	if ( NULL != pcAttrValue  )	// �Ƿ����K��
	{
		m_bUpdateKline = atoi(pcAttrValue) != 0;		
	}

	pcAttrValue = pTiXmlElement->Attribute(KStrViewBlockReportUpdateTrend);
	if ( NULL != pcAttrValue  )	// �Ƿ���·�ʱ
	{
		m_bUpdateTrend = atoi(pcAttrValue) != 0;		
	}

	pcAttrValue = pTiXmlElement->Attribute(KStrViewBlockReportUpdateReport);
	if ( NULL != pcAttrValue  )	// �Ƿ���±��۱�
	{
		m_bUpdateReport = atoi(pcAttrValue) != 0;		
	}

	// �Ƿ���ʾ��ֱ������
	pcAttrValue = pTiXmlElement->Attribute(KStrViewReportShowGridVertScrool11);
	if (pcAttrValue != NULL)
	{
		m_bIsShowGridVertScorll = atoi(pcAttrValue);
	}



	// ��ʱTab��Ϣ�ǳ�ʼ�����˵�
	int i=0;
	ASSERT( m_aTabInfomations.GetSize() > 0 );
	for ( i=0; i < m_aTabInfomations.GetSize() ; i++ )
	{
		if ( m_aTabInfomations[i].m_iBlockCollectionId == m_iBlockClassId )
		{
			break;
		}
	}
	if ( i >= m_aTabInfomations.GetSize() )
	{
		m_iBlockClassId = T_BlockReportTab::BCAll;
	}

	SetTabByClassId(m_iBlockClassId);		// ���ʱ����ʵ�ǲ�����tab�л���

	m_GuiTabWnd.SetBkGround(false,GetIoViewColor(ESCBackground),0,0);

	return true;
}

CString CIoViewBlockReport::ToXml()
{	
	//
	CString StrThis;

	if ( !m_bRequestViewSort )
	{
		ASSERT( m_iSortColumn == m_GridCtrl.GetSortColumn() || -1 == m_iSortColumn);	// ������, ӵ�б�������
		//m_iSortColumn = -1;
	}

	StrThis.Format( L"<%s %s=\"%s\" %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%s\" %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%d\"", 
					CString(GetXmlElementValue()).GetBuffer(),
					CString(GetXmlElementAttrIoViewType()).GetBuffer(),	CIoViewManager::GetIoViewString(this).GetBuffer(),
					CString(KStrViewBlockReportTabId).GetBuffer(),	m_iBlockClassId,
					CString(KStrViewReportShowGridVertScrool11).GetBuffer(), m_bIsShowGridVertScorll,
					CString(KStrViewBlockReportSortHeader).GetBuffer(),	m_iSortColumn,
					CString(KStrViewBlockReportSortHeaderDesc).GetBuffer(),	m_MmiRequestBlockSort.m_bDescSort ? _T("1") : _T("-1"),
					CString(KStrViewBlockReportShowTitle), m_bShowTitle,
					CString(KStrViewBlockReportUpdateKline), m_bUpdateKline,
					CString(KStrViewBlockReportUpdateTrend), m_bUpdateTrend,
					CString(KStrViewBlockReportUpdateReport), m_bUpdateReport
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

CString CIoViewBlockReport::GetDefaultXML()
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

void CIoViewBlockReport::OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult)
{ 
	CPoint pos(0, 0);
	GetCursorPos(&pos);
	DoTrackMenu(pos);
}
void CIoViewBlockReport::OnGridSelRowChanged( NMHDR *pNotifyStruct, LRESULT* pResult )
{
	
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	
	// ֪ͨ��Ʒ�����˸ı�
	if (IsPointInGrid())
	{				
		CCellRange cellRange = m_GridCtrl.GetSelectedCellRange();	

		int32 iRow = pItem->iRow;
		int32 iCol = 1;
	
		if ( iRow >= m_GridCtrl.GetFixedRowCount() && iRow < m_GridCtrl.GetRowCount() )
		{
			int32 iBlockId = (int32)m_GridCtrl.GetItemData(iRow, 0);
			OnDblClick(iBlockId);
		}
	}		
}
void CIoViewBlockReport::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW	*pGridView = (NM_GRIDVIEW *)pNotifyStruct;
	if(NULL != pGridView )
	{
		if ( pGridView->iRow >= m_GridCtrl.GetFixedRowCount() )
		{
			int32 iBlockId = (int32)m_GridCtrl.GetItemData(pGridView->iRow, 0);
			//OnDblClick(iBlockId);
		}
	}
}

// ֪ͨ��ͼ�ı��ע����Ʒ
void CIoViewBlockReport::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch)
		return;
	
	// �޸ĵ�ǰ�鿴����Ʒ
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
}

//
void CIoViewBlockReport::OnVDataForceUpdate()
{
	if ( m_bRequestViewSort )
	{
		RequestSortData();
	}
	else
	{
		RequestViewDataCurrentVisibleRow();
	}
}

void CIoViewBlockReport::OnVDataRealtimePriceUpdate(IN CMerch *pMerch)
{
		
}

// �������г����з����仯
// �������г����з����仯
void CIoViewBlockReport::OnVDataReportUpdate(int32 iMarketId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount,  const CArray<CMerch*, CMerch*> &aMerchs)
{	
	
}

void CIoViewBlockReport::OnVDataGridHeaderChanged(E_ReportType eReportType)
{
	
}

bool32 CIoViewBlockReport::TabIsValid(int32& iTab)
{
	iTab = m_GuiTabWnd.GetCurtab();
	if ( iTab >=0 && iTab < m_aTabInfomations.GetSize() )
	{
		return true;
	}

	return false;
}

void CIoViewBlockReport::OnFixedRowClickCB(CCellID& cell)
{
	DoFixedRowClickCB(cell);
}

void CIoViewBlockReport::OnFixedColumnClickCB(CCellID& cell)
{

}

void CIoViewBlockReport::OnHScrollEnd()
{
	
}

void CIoViewBlockReport::OnVScrollEnd()
{
	
}

void CIoViewBlockReport::OnCtrlMove( int32 x, int32 y )
{
	
}

bool32 CIoViewBlockReport::OnEditEndCB ( int32 iRow,int32 iCol, const CString &StrOld, INOUT CString &StrNew )
{
	return false;
}

void CIoViewBlockReport::OnDestroy()
{	
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if ( NULL != pMainFrm && pMainFrm->m_pKBParent == this )
	{
		pMainFrm->SetHotkeyTarget(NULL);
	}

	CIoViewBase::OnDestroy();
}

void CIoViewBlockReport::OnTimer(UINT nIDEvent) 
{
	if(!m_bShowNow)
	{
		return;
	}

	if ( nIDEvent == KUpdatePushMerchsTimerId )
	{
		if ( m_bRequestViewSort )
		{
			//RequestSortData(); // ��������רҵid
		}
		else
		{
			RequestViewDataCurrentVisibleRow();		
		}
	}
	else if ( nIDEvent == KRequestSortDataTimerId )
	{
		if ( m_bRequestViewSort )
		{
			RequestSortData();
		}
	}
	else if ( nIDEvent == KWatchTimeChangeTimerId )
	{
		WatchDayChange(true);
	}
	else if ( nIDEvent == KChangeVisibleBlocksTimerId )
	{
		RequestViewDataCurrentVisibleRow();
	}
	else if(nIDEvent == KStartChooseBlockTimerId)
	{
		KillTimer(KStartChooseBlockTimerId);

		if ( m_GridCtrl.GetRowCount() > m_GridCtrl.GetFixedRowCount() )
		{
			int32 iBlockId = (int32)m_GridCtrl.GetItemData(1, 0);
			OnDblClick(iBlockId);
		}
		else 
		{
			SetTimer(KStartChooseBlockTimerId, KStartChooseBlockTimerPeriod , NULL);
		}
	}
	CIoViewBase::OnTimer(nIDEvent);
}

void CIoViewBlockReport::RequestSortData()
{
	if ( !m_bRequestViewSort )
	{
		return;
	}

	// �������� - -1��ʾ���а��
	m_MmiRequestBlockSort.m_iMarketId = -1; // m_iBlockClassId; // ���ͻ���A�ɰ������
	m_MmiRequestBlockSort.m_iStart = 0;
	m_MmiRequestBlockSort.m_iCount = -1;	// m_aShowBlockIds.GetSize();/*min(m_iMaxGridVisibleRow, m_aShowBlockIds.GetSize());*/
	RequestData(m_MmiRequestBlockSort);
}

void CIoViewBlockReport::RequestViewDataCurrentVisibleRow()
{
	// ȫ��������
	CBlockCollection *pCollection = GetCurrentCollection();
	if ( NULL != pCollection && pCollection->IsUserBlockCollection() )
	{
		return;	// ��ѡ���û�����ݿ������
	}
	CMmiReqLogicBlock req;
	req.m_iLogicBlockId = -1;
	RequestData(req);

// 	IdArray aBlockIds;
// 	int i;
// 	aBlockIds.Copy(m_aShowBlockIds);
// 	
// 	bool32 bFirst = true;
// 	m_mmiReqBlockData.m_aReqMore.RemoveAll();
// 	for ( i=0; i < aBlockIds.GetSize() ; i++ )
// 	{
// 		CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(aBlockIds[i]);
// 		if ( pBlock != NULL )
// 		{
// 			if ( bFirst )
// 			{
// 				m_mmiReqBlockData.m_iBlockID = -1;//aBlockIds[i];
// 				bFirst = false;
// 			}
// 			else
// 			{
// 				T_ReqPushBlockData d;
// 				d.m_iBlockId = aBlockIds[i];
// 				d.m_eType = ECSTBlock;
// 				m_mmiReqBlockData.m_aReqMore.Add(d);
// 			}
// 		}
// 		else
// 		{
// 			//ASSERT( 0 );
// 		}
// 	}
// 	ASSERT( (!m_bRequestViewSort && m_mmiReqBlockData.m_aReqMore.GetSize() >= aBlockIds.GetSize()-1) || m_bRequestViewSort );
// 	
// 	RequestData(m_mmiReqBlockData);
	
	KillTimer(KChangeVisibleBlocksTimerId);
	
}

void CIoViewBlockReport::OnUserBlockUpdate( CSubjectUserBlock::E_UserBlockUpdate eUpdateType )
{
	
}

void CIoViewBlockReport::SetChildFrameTitle()
{
	CString StrTitle;
	StrTitle =  CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;
	
	CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pParentFrame)
	{
		pParentFrame->SetChildFrameTitle(StrTitle);
	}
}

bool32 CIoViewBlockReport::GetStdMenuEnable( MSG* pMsg )
{
	return false;
}

void CIoViewBlockReport::OnDblClick(int32 iBlockId)
{
	CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(iBlockId);
	if ( NULL != pBlock )
	{
		CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
		if ( NULL == pMainFrame )
		{
			return;
		}
		CMerch *pMerch = GetBlockMerchXml();

		// �������۱��ǿ�ͼ��������Ʒ��ͼΪ���ǹ�Ʊ
		T_BlockDesc	desc;
		desc.m_StrBlockName = pBlock->m_blockInfo.m_StrBlockName;
		desc.m_aMerchs.Copy(pBlock->m_blockInfo.m_aSubMerchs);
		if ( CBlockConfig::Instance()->IsUserBlock(iBlockId) )	// ֻ֧�������ְ��
		{
			desc.m_eType = T_BlockDesc::EBTUser;
		}
		else
		{
			desc.m_eType = T_BlockDesc::EBTBlockLogical;
			desc.m_iMarketId = pBlock->m_blockInfo.m_iBlockId;	// ���id��Ϊ�г�id
		}

		//pMainFrame->OnSpecifyBlock(desc);

		//
		const int32 iGroupId = GetIoViewGroupId();

		// �ǿ� - ���ڲŻ�
		for ( int32 i=0; i < pMainFrame->m_IoViewsPtr.GetSize() ; i++ )
		{
			CIoViewBase *pIoView = pMainFrame->m_IoViewsPtr[i];
			if ( NULL == pIoView )
			{
				continue;
			}

			CIoViewStarry *pStarry = DYNAMIC_DOWNCAST(CIoViewStarry, pIoView);
			if ( NULL != pStarry && pIoView->GetIoViewGroupId() == iGroupId )	// �ǿ�ͼ
			{
				pStarry->SetNewBlock(iBlockId)	;	// �ɼ�������������
			}
		}
	
		// ����
		if ( NULL != m_pAbsCenterManager && NULL != pMerch && iGroupId != -1 )	// �Ƕ�����ͼ
		{
			CMDIChildWnd* pActiveMainFrm =(CMDIChildWnd*)pMainFrame->GetActiveFrame();		
			if(m_bUpdateKline)
			{
				//	�л�K��ͼ
				CIoViewChart* pKlineBase =DYNAMIC_DOWNCAST(CIoViewChart, pMainFrame->FindIoViewInFrame(ID_PIC_KLINE,  pActiveMainFrm, false, true, GetIoViewGroupId()));
				if (NULL != pKlineBase)
				{
					bool bChange = true;
					if (pKlineBase->HaveLimitMerch())		//	�����ͼ�Ѿ��޶�����Ʒ����Ҫ��ѯ�Ƿ��������л�����Ʒ
					{
						if (!pKlineBase->IsLimitMerch(pMerch))
						{
							bChange = false;
						}
					}

					if (bChange)
					{
						pMainFrame->OnViewMerchChanged(pKlineBase, pMerch);
						pKlineBase->ForceUpdateVisibleIoView();
					}				
					
				}
			}

			if(m_bUpdateTrend)
			{
				//	�л���ʱͼ��Ʒ
				CIoViewChart* pTrendBase = DYNAMIC_DOWNCAST(CIoViewChart,pMainFrame->FindIoViewInFrame(ID_PIC_TREND,  pActiveMainFrm, false, true, GetIoViewGroupId()));
				if (NULL != pTrendBase)
				{
					bool bChange = true;
					if (pTrendBase->HaveLimitMerch())		//	�����ͼ�Ѿ��޶�����Ʒ����Ҫ��ѯ�Ƿ��������л�����Ʒ
					{
						if (!pTrendBase->IsLimitMerch(pMerch))
						{
							bChange = false;
						}
					}

					if (bChange)
					{
						pMainFrame->OnViewMerchChanged(pTrendBase, pMerch);
						pTrendBase->ForceUpdateVisibleIoView();
					}					
					
				}
			}

			if(m_bUpdateReport)
			{
				//	���¼��ذ������
				CIoViewBase* pReportBase = pMainFrame->FindIoViewInFrame(ID_PIC_REPORT,  pActiveMainFrm, false, true, GetIoViewGroupId());
				if (NULL != pReportBase)
				{
					CIoViewReport * pReport = DYNAMIC_DOWNCAST(CIoViewReport, pReportBase);
					bool bIsUserblock = pReport->IsShowUserBlock();
					if (NULL != pReport && !bIsUserblock)
					{
						pReport->SetTabByBlockName(desc.m_StrBlockName);
						pReport->AddBlock(desc);
						pReport->ReCreateTabWnd();							
					}				
				}
			}
		}
	}	
}

void CIoViewBlockReport::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();
	m_GuiTabWnd.SetBkGround(false,GetIoViewColor(ESCBackground),0,0);
	Invalidate();
	m_GridCtrl.Refresh();
}

void CIoViewBlockReport::OnIoViewFontChanged()
{
	CIoViewBase::OnIoViewFontChanged();

	LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);	

	RecalcLayout();
	Invalidate();
	m_GridCtrl.Refresh();
}

void CIoViewBlockReport::OnRButtonDown2( CPoint pt, int32 iTab )
{
	
}

bool32 CIoViewBlockReport::OnLButtonDown2( CPoint pt, int32 iTab )
{
	return false;
}

bool32 CIoViewBlockReport::BlindLButtonBeforeRButtonDown( int32 iTab )
{
	return false;
}

void CIoViewBlockReport::SetTab(int32 iCurTab, int32 iSortCol/*=-1*/, int32 iSortType/*=0*/)
{
	if ( iCurTab < 0 || iCurTab >= m_aTabInfomations.GetSize() )
	{
		return;
	}
	
	int32 iCur = m_GuiTabWnd.GetCurtab();
	m_GuiTabWnd.SetCurtab(iCurTab);
	if ( iCur == iCurTab )	// ��ʱtab���ᷢ����Ϣ���Լ�����
	{
		TabChanged();
	}
	if ( iSortCol >= 0 )
	{
		SortColumn(iSortCol, iSortType);
	}
}

void CIoViewBlockReport::TabChanged()
{
	// ��ʱTab�Ѿ��л���, m_iCurTab�Ѿ���pre tab
	int32 iTab = m_GuiTabWnd.GetCurtab();
	T_BlockReportTab tab;
	if ( GetTabInfoByIndex(iTab, tab) )
	{
		m_iPreTab = m_iCurTab;
		if ( m_iPreTab != -1 )	// ������Ч��tab��Ϣ
		{
			CCellID cellId = m_GridCtrl.GetTopleftNonFixedCell();
			if ( cellId.IsValid() )
			{
				UpdateTabInfo(m_iPreTab, cellId.row, cellId.col);
			}
		}
		m_iCurTab = iTab;
		ASSERT( tab.IsValid() );

		m_iBlockClassId = tab.m_iBlockCollectionId;

		// ȡ������������Ϣ
		m_bRequestViewSort = false;
		m_iSortColumn = -1;
		//m_MmiRequestSys = ...

		m_aShowBlockIds.RemoveAll();

		// ����block������ʾ����
		if ( m_iBlockClassId == T_BlockReportTab::BCAll )
		{
			// ȡ��ǰTab�����еĺϼ� - BCALL������0λ��
			for ( int i=1; i < m_aTabInfomations.GetSize() ; i++ )
			{
				IdArray aIds;
				CBlockCollection *pCollection = CBlockConfig::Instance()->GetBlockCollectionById(m_aTabInfomations[i].m_iBlockCollectionId);
				ASSERT( NULL != pCollection );
				if ( NULL != pCollection )
				{
					if ( pCollection->m_aBlocks.GetSize() <= 0 )
					{
						continue;	// 
					}
					int32 iBlockType = pCollection->m_aBlocks[0]->m_blockInfo.m_iType;
					if ( CBlockInfo::typeNormalBlock != iBlockType && CBlockInfo::typeUserBlock != iBlockType )
					{
						continue; // ��֧�ֵ����ͣ�ֻ����һ��
					}
					pCollection->GetValidBlockIdArray(aIds);
					m_aShowBlockIds.Append(aIds);
				}
			}
		}
		else
		{
			CBlockCollection *pCollection = CBlockConfig::Instance()->GetBlockCollectionById(m_iBlockClassId);
			ASSERT( NULL != pCollection );
			if ( NULL != pCollection )
			{
				pCollection->GetValidBlockIdArray(m_aShowBlockIds);
			}
		}

		m_aTabInfomations[iTab].m_aSubBlockIds.Copy(m_aShowBlockIds);	// ����ԭʼ���������Tab��
		
		UpdateTableAllContent();

		//m_GridCtrl.EnsureVisible(tab.m_iStartRow, tab.m_iColLeftVisible);
		int32 iStartRow, iStartCol;
		iStartRow = tab.m_iStartRow;
		iStartCol = tab.m_iColLeftVisible;
		if ( tab.m_iStartRow < m_GridCtrl.GetFixedRowCount() || tab.m_iColLeftVisible < m_GridCtrl.GetFixedColumnCount() || !m_GridCtrl.IsValid(iStartRow, iStartCol) )
		{
			iStartRow = m_GridCtrl.GetFixedRowCount();
			iStartCol = m_GridCtrl.GetFixedColumnCount();
		}
		m_GridCtrl.EnsureTopLeftCell(iStartRow, iStartCol);
		m_GridCtrl.SetFocusCell(iStartRow, iStartCol);
		m_GridCtrl.SetSelectedRange(iStartRow, 0, iStartRow, m_GridCtrl.GetColumnCount()-1);

		RequestViewDataCurrentVisibleRow();
		

		SortColumn(3, 1);	// ��������n��
	}
	else
	{
		ASSERT( 0 );
	}
}

void CIoViewBlockReport::SetRowHeightAccordingFont()
{	
	m_GridCtrl.SetDefCellHeight(28);
	m_GridCtrl.AutoSizeRows();	
	if (0 != m_GridCtrl.GetFixedRowCount())
	{
		m_GridCtrl.SetRowHeight(0, 40);
	}
}

void CIoViewBlockReport::SetColWidthAccordingFont()
{
	int32 iColWidth = 90;
	m_GridCtrl.AutoSizeColumns();
	for ( int i=0; i < m_GridCtrl.GetColumnCount() ; i++ )	// ������Сcol
	{
		if(i == 2)
		{
			iColWidth = 130;
		}
		else
		{
			iColWidth = 90;
		}

		if ( m_GridCtrl.GetColumnWidth(i) > 0 && m_GridCtrl.GetColumnWidth(i) < iColWidth )
		{
			m_GridCtrl.SetColumnWidth(i, iColWidth);
		}
	}
}

void CIoViewBlockReport::UpdateTableHeader()
{
	CStringArray aColNames;
	CBlockDataPseudo::GetDefaultColNames(aColNames);		// ��ͷ�̶�

	m_GridCtrl.SetFixedColumnCount(2);	// ����� ���ƹ̶�
	m_GridCtrl.SetFixedRowCount(1);	
	m_GridCtrl.DeleteNonFixedRows();
	m_GridCtrl.SetColumnCount(aColNames.GetSize());	// �����, ����...

	SetColWidthAccordingFont();
	SetRowHeightAccordingFont();
	m_GridCtrl.Refresh();
}

void CIoViewBlockReport::UpdateTableContent( int32 /*iClassId*/, int32 iBlockId, bool32 /*bBlink*/ )
{
	//DWORD dwTime = timeGetTime();
	// ����Ƿ�����ʾ��Χ�ڣ�
	CCellRange rangeVisible = m_GridCtrl.GetVisibleNonFixedCellRange();
	if ( !rangeVisible.IsValid() )
	{
		return;
	}

	int i = 0;
	for ( i=rangeVisible.GetMinRow(); i <= rangeVisible.GetMaxRow() ; i++ )
	{
		if ( (int32)(m_GridCtrl.GetItemData(i, 0)) == iBlockId )
		{
			break;
		}
	}
	if ( i >= rangeVisible.GetMaxRow() )
	{
		return;
	}

	m_GridCtrl.RedrawRow(i);		// ������һ����ʾ�Ϳ�����

	//TRACE(_T("����һ��: %dms\n"), timeGetTime()-dwTime);

// 	SetColWidthAccordingFont();
// 	m_GridCtrl.Refresh();
}

void CIoViewBlockReport::UpdateTableAllContent()
{
	m_GridCtrl.DeleteNonFixedRows();	
	//m_GridCtrl.ResetVirtualOrder();

	T_BlockReportTab tab;
	if ( !GetCurrentRowInfo(tab) )
	{
		return;
	}
	
	int32 iMaxRowCount = 0;
	// ��ģʽ�������ж��ӽ���
	iMaxRowCount = m_aShowBlockIds.GetSize();
	

	m_GridCtrl.SetRowCount(iMaxRowCount + m_GridCtrl.GetFixedRowCount());
	
	
	//SetColWidthAccordingFont();
	//SetRowHeightAccordingFont();
	m_GridCtrl.Refresh();
}

void CIoViewBlockReport::ResetBlockShowData(bool32 bResetTopLeft/*=true*/)
{
	T_BlockReportTab tab;
	if ( GetCurrentRowInfo(tab) )
	{
		KillTimer(KRequestSortDataTimerId);		// ֹͣ��ʱ����������

		m_iSortColumn = -1;
		m_bRequestViewSort = false;
		m_GridCtrl.SetSortColumn(-1);
		m_GridCtrl.SetHeaderSort(FALSE);
		
		if ( bResetTopLeft )
		{
			SetCurrentRowInfo(m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetFixedColumnCount());	// �������򱣴�����Ͻǿɼ�����
			m_aShowBlockIds.Copy(tab.m_aSubBlockIds);
			
			UpdateTableAllContent();
			
			//m_GridCtrl.EnsureVisible(m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetFixedColumnCount());
			m_GridCtrl.EnsureTopLeftCell(m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetFixedColumnCount());
		}
		else
		{
			m_aShowBlockIds.Copy(tab.m_aSubBlockIds);
			m_GridCtrl.SetRowCount(m_aShowBlockIds.GetSize() + m_GridCtrl.GetFixedRowCount());
			m_GridCtrl.ResetVirtualOrder();
			m_GridCtrl.Refresh();
		}
	}
}

bool32 CIoViewBlockReport::GetCurrentRowInfo(OUT T_BlockReportTab &tab)
{
	return GetTabInfoByIndex(m_GuiTabWnd.GetCurtab(), tab);
}

void CIoViewBlockReport::SetCurrentRowInfo( int32 iRowBegin, int32 iColLeft )
{
	int32 iCurTab = m_GuiTabWnd.GetCurtab();
	T_BlockReportTab tab;
	if ( GetTabInfoByIndex(iCurTab, tab) )
	{
		m_aTabInfomations[iCurTab].m_iStartRow = iRowBegin;
		m_aTabInfomations[iCurTab].m_iColLeftVisible = iColLeft;
	}
}

bool32 CIoViewBlockReport::GetTabInfoByIndex( int32 iIndex, OUT T_BlockReportTab &tabInfo )
{
	if ( iIndex >=0 && iIndex < m_aTabInfomations.GetSize() )
	{
		tabInfo = m_aTabInfomations[iIndex];
		return true;
	}
	return false;
}

void CIoViewBlockReport::OnDoBlockReportInitialize()
{
	m_bBlockReportInitialized = true;		// ������һ����־�����ڻ�����ʵ�ʳ�ʼ��

	int32 iSortColNow = m_iSortColumn;		// ����������
	bool32 bReqNow = m_bRequestViewSort;

	TabChanged();		// ��װ��һ�ΰ������
	if ( -1 != m_iSortColumnFromXml )
	{
		// xml��������״̬������ض��ɶ���Ч����������Ѿ����ⲿ�ı��ˣ�����Ҫ�ٴθı��ʼ��״̬
		SortColumn(m_iSortColumnFromXml, m_iSortTypeFromXml);	// type������0����Ϊ��ǰ����û�б���type
	}
	else if ( bReqNow )
	{
		// �����Ѿ�����ȥ�ˣ���û�д���ֻ��Ҫ�ָ������־�Ϳ�����
		m_iSortColumn = iSortColNow;		// �ָ�����
		m_bRequestViewSort = bReqNow;
	}
}

void CIoViewBlockReport::InitializeTabs()
{
	int i = 0;
	bool32 bInitOld = m_bBlockReportInitialized;
	m_bBlockReportInitialized = false;				// ��ֹtab��Ӧ

	m_GuiTabWnd.DeleteAll();
	m_aTabInfomations.RemoveAll();

	T_BlockReportTab tab;
	tab.m_iBlockCollectionId = T_BlockReportTab::BCAll;		// ���а�� ȡ��������������а��ϼ�
	tab.m_StrName = _T("ȫ��");
	m_aTabInfomations.Add(tab);

	
	IdArray aIds;
	CBlockConfig::Instance()->GetCollectionIdArray(aIds);

	int32	iMarketClassCollectionId;
	bool32 bHasMarketClassCollection = false;
	bHasMarketClassCollection = CBlockConfig::Instance()->GetMarketClassCollectionId(iMarketClassCollectionId);
	
	for ( i=0; i < aIds.GetSize() ; i++ )
	{
		// ��Ҫ�жϷ����飬�޳���ȥ - TODO
		if ( bHasMarketClassCollection && iMarketClassCollectionId == aIds[i] )
		{
			continue;		// �������еİ�鲻Ҫ
		}

		CBlockCollection *pCollection = CBlockConfig::Instance()->GetBlockCollectionById(aIds[i]);
		if ( NULL == pCollection || pCollection->m_aBlocks.GetSize() <= 0 )
		{
			continue;	// 
		}
		int32 iBlockType = pCollection->m_aBlocks[0]->m_blockInfo.m_iType;
		if ( CBlockInfo::typeNormalBlock != iBlockType && CBlockInfo::typeUserBlock != iBlockType )
		{
			continue; // ��֧�ֵ�����
		}

		T_BlockReportTab tempTab;
		tempTab.m_iBlockCollectionId = aIds[i];
		CBlockConfig::Instance()->GetCollectionName(aIds[i], tempTab.m_StrName);
		ASSERT( !tempTab.m_StrName.IsEmpty() );
		m_aTabInfomations.Add(tempTab);
	}
	
	// ����4��5��Ԫ��
	T_BlockReportTab tblockTemp;
	for (int i = 0; i < m_aTabInfomations.GetSize(); i++)
	{
		if (i == 0)
		{
			continue;
		}
		if (5 == m_aTabInfomations[i].m_iBlockCollectionId  && 4 == m_aTabInfomations[i-1].m_iBlockCollectionId)
		{
			tblockTemp = m_aTabInfomations[i];
			m_aTabInfomations[i] = m_aTabInfomations[i-1];
			m_aTabInfomations[i-1] = tblockTemp;
			break;
		}
	}

	for ( i=0; i < m_aTabInfomations.GetSize() ; i++ )
	{
		const T_BlockReportTab &tTab = m_aTabInfomations[i];
		CString StrName = tTab.m_StrName;
		m_GuiTabWnd.Addtab(StrName, StrName, StrName);
	}

	for ( i=0; i < m_aTabInfomations.GetSize() ; i++ )
	{
		const T_BlockReportTab &tTmptab = m_aTabInfomations[i];
		
		if ( m_iBlockClassId == tTmptab.m_iBlockCollectionId )
		{
			m_GuiTabWnd.SetCurtab(i);
			break;
		}
	}

	if ( i >= m_aTabInfomations.GetSize() )
	{
		m_iBlockClassId = T_BlockReportTab::BCAll;
		m_GuiTabWnd.SetCurtab(0);
	}

	m_bBlockReportInitialized = bInitOld;
}

void CIoViewBlockReport::SetTabByClassId( int32 iClassId, int32 iSortCol/*=-1*/, int32 iSortType/*=0*/ )
{
	int i = 0;
	for ( i=0; i < m_aTabInfomations.GetSize() ; i++ )
	{
		if ( iClassId == m_aTabInfomations[i].m_iBlockCollectionId )
		{
			if ( m_GuiTabWnd.GetCount() > i )
			{
				m_GuiTabWnd.SetCurtab(i);		// ��ǰѡ���Ƿ���??
				if ( iSortCol >= 0 )
				{
					SortColumn(iSortCol, iSortType);
				}
				break;
			}
			else
			{
				ASSERT( 0 );
			}
		}
	}
}

void CIoViewBlockReport::RecalcLayout()
{
	m_RectTitle.SetRectEmpty();
	m_iMaxGridVisibleRow = 0;
	int32 iHeightScroll = 12;
	int32 iGuiTabHeight = 21;

	CRect rcClient;
	GetClientRect(rcClient);

	m_RectTitle = rcClient;

	CClientDC dc(this);
	CFont *pFontOld ;
	if (m_bShowTitle)
	{
		pFontOld = dc.SelectObject(GetIoViewFontObject(ESFNormal));
		CSize sizeTitle = dc.GetTextExtent(_T("����߶Ȳ���"));
		dc.SelectObject(pFontOld);

		m_RectTitle.bottom = m_RectTitle.top + sizeTitle.cy + 10;

		rcClient.top = m_RectTitle.bottom;
	}

	CSize sizeTab = m_GuiTabWnd.GetWindowWishSize();
	sizeTab.cy = iGuiTabHeight;
	if ( rcClient.Height() < sizeTab.cy )
	{
		m_GuiTabWnd.MoveWindow(0,0,0,0);
		m_GridCtrl.MoveWindow(0,0,0,0);
		m_XSBHorz.MoveWindow(0,0,0,0);
		return;
	}
	
	// ��������tab������srcoll����С��һ���ض�ֵ, ����ܿ��С�ڸ�ֵ�������п�ȶ���scroll��
	int32 iMaxTabWidth = rcClient.Width() - 125;	// 
	iMaxTabWidth = iMaxTabWidth < 0 ? 0 : iMaxTabWidth;

	sizeTab.cx = MIN(iMaxTabWidth, sizeTab.cx);		// ����1/1
	m_GuiTabWnd.MoveWindow(rcClient.left, rcClient.top -1, sizeTab.cx, sizeTab.cy);
	CRect rcHorz(rcClient.left , rcClient.bottom - iHeightScroll, rcClient.right, rcClient.bottom);
	m_XSBHorz.SetSBRect(rcHorz, TRUE);

	rcClient.bottom -= iHeightScroll;
	rcClient.top += sizeTab.cy;
	rcClient.top -= 1;



	if(m_bIsShowGridVertScorll)	// �Ƿ���ʾ��ֱ������
	{
		rcClient.right -= iHeightScroll;

		CRect RectVScroll;
		RectVScroll.left = rcClient.right;
		RectVScroll.right = RectVScroll.left + iHeightScroll;
		RectVScroll.top = rcClient.top;
		RectVScroll.bottom = rcClient.bottom;
		m_XSBVert.SetSBRect(RectVScroll, TRUE);
	}



	m_GridCtrl.MoveWindow(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height());	// ������ֱ����������

	// ����������� - ���Լ���  - ��Ҫ�����㹻������
	pFontOld = dc.SelectObject(GetIoViewFontObject(ESFNormal));
	CSize sizeText = dc.GetTextExtent(_T("�и߶Ȳ���"));
	dc.SelectObject(pFontOld);
	m_iMaxGridVisibleRow = rcClient.Height() / sizeText.cy + 1;	// ���㼸��
	if ( m_iMaxGridVisibleRow < 0 )
	{
		m_iMaxGridVisibleRow = 0;
	}
}

void CIoViewBlockReport::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	
}

void CIoViewBlockReport::OnConfigInitialize( E_InitializeNotifyType eInitializeType )
{
	if ( Initialized == eInitializeType )
	{
		// �����Ϣ�ؽ�
		InitializeTabs();
		RecalcLayout();
		SetTabByClassId(m_iBlockClassId);
		
	}
}

void CIoViewBlockReport::OnBlockConfigChange( int32 iBlockId, E_BlockNotifyType eNotifyType )
{
	// ���������
	Invalidate(TRUE);	// ˢ������ʾ
}

void CIoViewBlockReport::OnGridGetDispInfo( NMHDR *pNotifyStruct, LRESULT *pResult )
{
	//DWORD dwTime = timeGetTime();
	if ( NULL != pNotifyStruct )
	{
		GV_DISPINFO	*pDisp = (GV_DISPINFO *)pNotifyStruct;
		// ����v mode
		
		GV_ITEM &item = pDisp->item;
		item.lParam = NULL;
		if ( item.row >=0 && item.row <= m_aShowBlockIds.GetSize() )
		{
			CString StrValue;
			
			CBlockDataPseudo::HeaderArray aHeaders;
			CBlockDataPseudo::GetDefaultColArray(aHeaders);
			CStringArray aColNames;
			CBlockDataPseudo::GetDefaultColNames(aColNames);
			ASSERT( item.col >=0 && item.col < aColNames.GetSize() );
			if ( item.row == 0 )	// ��ͷ
			{
				//	�����б�ͷ����
				
				if ( item.col >=0 && item.col < aColNames.GetSize() )
				{
					item.strText = aColNames[item.col];
					item.lParam  = (LPARAM)aHeaders[item.col];
					item.crFgClr = GetIoViewColor(ESCVolume);
					memcpyex(&item.lfFont, GetColumnExLF(), sizeof(LOGFONT));
				}
// 				else if ( item.col == 0 )
// 				{
// 					item.strText = _T("          ");	// ��Ų���ʾ
// 					item.lParam  = (LPARAM)aHeaders[item.col];
// 				}
			}
			else
			{
				// ��������
				int32 iBlockId = m_aShowBlockIds[item.row-1];
				item.lParam = iBlockId;
				
				CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(iBlockId);
				
				// Ӧ�����ձ�ͷ����ʾ������û�иı��ͷ�ķ������Ͱ����������ʾ
				CBlockDataPseudo::E_Header eHeader = aHeaders[item.col];
				memcpyex(&item.lfFont, GetIoViewFont(ESFText), sizeof(LOGFONT));
				switch(eHeader)
				{
				case CBlockDataPseudo::ShowRowNo:
					item.crFgClr = GetIoViewColor(ESCText);
					break;
				case CBlockDataPseudo::BlockName:
					item.crFgClr = RGB(245,253,170);
					memcpyex(&item.lfFont, GetIoViewFont(ESFNormal), sizeof(LOGFONT));
					break;
				case CBlockDataPseudo::BlockId:
					item.crFgClr = GetIoViewColor(ESCText);
					break;
				case CBlockDataPseudo::RiseMerchCount:
					item.crFgClr = GetIoViewColor(ESCRise);
					break;
				case CBlockDataPseudo::FallMerchCount:
					item.crFgClr = GetIoViewColor(ESCFall);
					break;
				default:						
					if (!(CBlockDataPseudo::CapitalFlow == eHeader   || CBlockDataPseudo::RiseRate == eHeader || CBlockDataPseudo::WeightRiseRate == eHeader ||CBlockDataPseudo::RiseRate 
						|| CBlockDataPseudo::RiseSpeed || CBlockDataPseudo::PrincipalPureNum || CBlockDataPseudo::PrincipalAmount || CBlockDataPseudo::AmountRate))
					{
						item.crFgClr = RGB(0xdc, 0xdc, 0xdc);
					}
					if (CBlockDataPseudo::RiseTopestMerch == eHeader)
					{
						memcpyex(&item.lfFont, GetIoViewFont(ESFNormal), sizeof(LOGFONT));
					}
					break;
				}
				if ( eHeader == CBlockDataPseudo::ShowRowNo )
				{
					// ��ţ���ʾ���е���ţ�������ʵ�������е����
					int i = 0;
					for ( CGridCtrl::intlist::const_iterator it = m_GridCtrl.m_arRowOrder.begin(); it != m_GridCtrl.m_arRowOrder.end() ; it++, i++ )
					{
						if ( *it == item.row )
						{
							item.strText.Format(_T("%d  "), i);
							break;
						}
					}
					
				}
				else if ( pBlock != NULL )
				{					
					item.strText = _T(" -");
					CGridCellSymbol *pCell = (CGridCellSymbol *)m_GridCtrl.GetDefaultVirtualCell();
					ASSERT( pCell->IsKindOf(RUNTIME_CLASS(CGridCellSymbol)) );
					pCell->SetShowSymbol(CGridCellSymbol::ESSNone);

					switch (eHeader)
					{
					case CBlockDataPseudo::BlockName:		// �������
						item.strText = pBlock->m_blockInfo.m_StrBlockName;						
						break;
					case CBlockDataPseudo::BlockId:			// ������
						item.strText.Format(L"%d",pBlock->m_blockInfo.m_iBlockId);
						break;
					case CBlockDataPseudo::RiseRate:		// �Ƿ�
					case CBlockDataPseudo::RiseSpeed:		         // ����
					case CBlockDataPseudo::PrincipalPureNum:         // ��������
					case CBlockDataPseudo::PrincipalAmount:		     // �������
					case CBlockDataPseudo::AmountRate:		         // ����
						{
							pCell->SetShowSymbol(CGridCellSymbol::ESSFall);
							if ( pBlock->IsValidLogicBlockData() )
							{
								CBlockDataPseudo blockData(pBlock->m_logicBlockData);
								float fValue;
								if ( blockData.GetColValue(eHeader, &fValue) )
								{
									item.strText = Float2SymbolString(fValue, 0, 2, true);
								}
							}
						}
						break;
					case CBlockDataPseudo::RiseMerchCount:		     // �Ǽ���
					case CBlockDataPseudo::FallMerchCount:		     // ������
						{
							if ( pBlock->IsValidLogicBlockData() )
							{
								CBlockDataPseudo blockData(pBlock->m_logicBlockData);
								float fValue;
								if ( blockData.GetColValue(eHeader, &fValue) )
								{
									CString strTmpValue = Float2String(fValue, 0, false, false);
									if(strTmpValue == L"0")
									{
										strTmpValue = L" -";
									}
									item.strText = strTmpValue;
								}
							}
						}
						break;
					case CBlockDataPseudo::RiseTopestMerch:         // ���ǹ�
						{
							COLORREF clr;
							item.crFgClr = GetIoViewColor(ESCAmount); //RGB(176,176,0);
							StrValue = pBlock->m_logicBlockData.m_StrMerchName;
							// ��Ѱ����Ʒ - �Ƿ�����ѡ��
							CMerch *pMerchTopest = pBlock->m_blockInfo.FindMerchByCnName(StrValue);
							T_Block* pUserBlock = NULL;
							if ( NULL != pMerchTopest && (pUserBlock = CUserBlockManager::Instance()->GetBlock(pMerchTopest)) != NULL  )
							{
								clr = pUserBlock->m_clrBlock;

								// ��ѡ������Ʒ������ɫ������ʾ:
								if ( COLORNOTCOUSTOM != clr )
								{
									item.crFgClr = clr;
								}	
							}
							// �Ƿ��б��
							GV_DRAWMARK_ITEM markItem;
							if ( InitDrawMarkItem(pMerchTopest, markItem) )
							{
								item.markItem = markItem;
							}

							item.strText = pBlock->m_logicBlockData.m_StrMerchName;	// ���������Ǵ��뻹������
						}

						break;
					case CBlockDataPseudo::TotalHand:		      // ����
					case CBlockDataPseudo::TotalAmount:		      // �ܽ��
					case CBlockDataPseudo::TotalMarketValue:	  // ����ֵ
					case CBlockDataPseudo::CircleMarketValue:	  // ��ͨ��ֵ
						{
							item.crFgClr = GetIoViewColor(ESCText); //RGB(176,176,0);
							if ( pBlock->IsValidLogicBlockData() )
							{
								CBlockDataPseudo blockData(pBlock->m_logicBlockData);
								float fValue;
								if ( blockData.GetColValue(eHeader, &fValue) )
								{
									item.strText = Float2SymbolString(fValue, 0, 2, true);
								}
							}
						}
						break;

					default:
						ASSERT( 0 );
					}
				}
			}
			
			if (item.col == 1 || item.col == 2 )
			{
				item.nFormat = DT_LEFT |DT_SINGLELINE |DT_VCENTER |DT_NOPREFIX;
			}
		}
		//TRACE(_T("���Ű��: ��ȡ��ʾcell(%d,%d) %d ms\n"), item.row, item.col, timeGetTime()-dwTime);
	}
	if ( NULL != *pResult )
	{
		*pResult = 1;
	}
}

bool32 CIoViewBlockReport::UpdateTabInfo( int32 iTabIndex, int32 iRowFirst, int32 iColLeft )
{
	if ( iTabIndex >=0 && iTabIndex < m_aTabInfomations.GetSize() )
	{
		m_aTabInfomations[iTabIndex].m_iStartRow = iRowFirst;
		m_aTabInfomations[iTabIndex].m_iColLeftVisible = iColLeft;
		return true;
	}
	return false;
}

bool CIoViewBlockReport::CompareRow( int iRow1, int iRow2 )
{
	ASSERT( m_spThis != NULL );
	ASSERT( m_spThis->m_aShowBlockIds.GetSize() >= iRow1 && m_spThis->m_aShowBlockIds.GetSize() >= iRow2  );
	const int iIndex1 = iRow1-1;
	const int iIndex2 = iRow2-1;
	
	int32 iBlockId1 = m_spThis->m_aShowBlockIds[iIndex1];
	int32 iBlockId2 = m_spThis->m_aShowBlockIds[iIndex2];

	CBlockLikeMarket *pBlock1 = CBlockConfig::Instance()->FindBlock(iBlockId1);
	CBlockLikeMarket *pBlock2 = CBlockConfig::Instance()->FindBlock(iBlockId2);
	
	const int iCol = m_spThis->m_GridCtrl.m_CurCol;
	ASSERT( iCol != 0 );	// ��Ų�������
	CBlockDataPseudo::E_Header eHeader = CBlockDataPseudo::BlockName;
	CBlockDataPseudo::HeaderArray aHeaders;
	CBlockDataPseudo::GetDefaultColArray(aHeaders);	// ����û���ṩ���ñ�ͷ��ʹ��Ĭ�ϵ�
	eHeader = aHeaders[iCol];

	bool bRet = true;
	if ( pBlock1 == NULL && pBlock2 != NULL )
	{
		bRet = false;
	}
	else if ( pBlock1 != NULL && pBlock2 == NULL )
	{
		bRet = true;
	}
	else if ( pBlock1 == NULL && pBlock2 == NULL )
	{
		bRet = false;
	}
	else
	{
		switch (eHeader)
		{
		case CBlockDataPseudo::BlockName:		// �������
			{
				if(pBlock1 && pBlock2)
				{
					int iCmp = CompareString(LOCALE_SYSTEM_DEFAULT, 0, pBlock1->m_blockInfo.m_StrBlockName, -1,
						pBlock2->m_blockInfo.m_StrBlockName, -1);
					bRet = iCmp < CSTR_EQUAL;
				}
			}
			break;
		case CBlockDataPseudo::BlockId:
			{
				bRet = iBlockId1 < iBlockId2;
			}
			break;
		case CBlockDataPseudo::RiseRate:
		case CBlockDataPseudo::WeightRiseRate:
		case CBlockDataPseudo::TotalAmount:
		case CBlockDataPseudo::MarketRate:
		case CBlockDataPseudo::ChangeRate:
		case CBlockDataPseudo::MarketWinRateDync:
		case CBlockDataPseudo::CapitalFlow:
		case CBlockDataPseudo::MerchRise:
		case CBlockDataPseudo::RiseDays:
			{
				if(pBlock1 && pBlock2)
				{
					float fValue1, fValue2;
					fValue1 = fValue2 = FLT_MIN;
					if ( pBlock1->IsValidLogicBlockData() )
					{
						CBlockDataPseudo blockData(pBlock1->m_logicBlockData);
						blockData.GetColValue(eHeader, &fValue1);
					}
					if ( pBlock2->IsValidLogicBlockData() )
					{
						CBlockDataPseudo blockData(pBlock2->m_logicBlockData);
						blockData.GetColValue(eHeader, &fValue2);
					}
					bRet = fValue1 < fValue2;
				}
			}
			break;
		case CBlockDataPseudo::RiseMerchRate:
			{
				if(pBlock1 && pBlock2)
				{
					float fValue1, fValue2;
					fValue1 = fValue2 = FLT_MIN;
					if ( pBlock1->IsValidLogicBlockData() )
					{
						CBlockDataPseudo blockData(pBlock1->m_logicBlockData);
						float fRise, fFall;
						fRise = fFall = 0.0f;
						blockData.GetColValue(CBlockDataPseudo::RiseMerchCount, &fRise);
						blockData.GetColValue(CBlockDataPseudo::FallMerchCount, &fFall);
						if ( 0.0f != fFall )
						{
							fValue1 = fRise/fFall;
						}
						else
						{
							fValue1 = fRise*10000;	// �Ŵ�n��
						}
					}
					if ( pBlock2->IsValidLogicBlockData() )
					{
						CBlockDataPseudo blockData(pBlock2->m_logicBlockData);
						float fRise, fFall;
						fRise = fFall = 0.0f;
						blockData.GetColValue(CBlockDataPseudo::RiseMerchCount, &fRise);
						blockData.GetColValue(CBlockDataPseudo::FallMerchCount, &fFall);
						if ( 0.0f != fFall )
						{
							fValue2 = fRise/fFall;
						}
						else
						{
							fValue2 = fRise*10000;	// �Ŵ�n��
						}
					}
					if ( fValue1 == 0.0f && fValue2 == 0.0f )
					{
						bRet = pBlock1->m_blockInfo.m_aSubMerchs.GetSize() < pBlock2->m_blockInfo.m_aSubMerchs.GetSize();
					}
					else
					{
						bRet = fValue1 < fValue2;
					}
				}
			}
			break;
		case CBlockDataPseudo::RiseTopestMerch:
			{
				if(pBlock1 && pBlock2)
				{
					int iCmp = CompareString(LOCALE_SYSTEM_DEFAULT, 0, pBlock1->m_logicBlockData.m_StrMerchName, -1,
						pBlock2->m_logicBlockData.m_StrMerchName, -1);
					bRet = iCmp < CSTR_EQUAL;
				}
			}
			break;
		default:
			ASSERT( 0 );
		}
	}
	
	return bRet;
}

void CIoViewBlockReport::OnGridCacheHint( NMHDR *pNotifyStruct, LRESULT *pResult )
{
	// �����ǰ�����ֶ�Ϊ��������߱��������ֶΣ�����Ҫ��ᣬ���Ϊ�����������ֶΣ����鱾�ش��ڵ����������Ƿ��㹻
	// ��������Ҫ�ط��������󣬵ȴ����ݻع飬�����ˢ����ʾ
	// ����ؼ����������ݴ��뱻��ǰע�͵��ˣ��������������û���ˣ� ȫ��ע�͵�--chenfj
	/*GV_CACHEHINT *pCacheHint = (GV_CACHEHINT *)pNotifyStruct;
	if ( NULL != pCacheHint && m_bRequestViewSort && pCacheHint->range.IsValid() )	// ���û�з������������󣬲���Ҫ����
	{
		const CCellRange &cellRange = pCacheHint->range;
		// ����ķ�Χ����Ҫ���ݷ�Χ ʵ��������Ҫӳ��õ�
		if ( cellRange.IsValid() )
		{
			int32 iMinRow = cellRange.GetMinRow() - m_GridCtrl.GetFixedRowCount();
			int32 iMaxRow = cellRange.GetMaxRow() - m_GridCtrl.GetFixedRowCount();
			ASSERT( m_iMaxGridVisibleRow >= cellRange.GetRowSpan() );
			ASSERT( iMaxRow < m_aShowBlockIds.GetSize() );
			int32 iBlockSize =  m_aShowBlockIds.GetSize();
			iMaxRow = min(iMaxRow, iBlockSize);
			bool32 bRequestData = false;
			for ( int i=iMinRow; i <= iMaxRow ; i++ )
			{
				if ( m_aShowBlockIds[i] == CBlockInfo::GetInvalidId() )
				{
					bRequestData = true;
					break;
				}
			}
			if ( bRequestData )
			{
				// �������� - TODO
				// �������� - ������������
				// ȫ�������Ѿ����룬����ȵ�����������ʾ
				
				//m_MmiRequestSys.m_iMarketId = 1; // shang a
 			//	m_MmiRequestSys.m_iStart = iMinRow;
 			//	int32 iMaxLeaveDataCount = m_aShowBlockIds.GetSize() - iMinRow;
 			//	m_MmiRequestSys.m_iCount = min(m_iMaxGridVisibleRow, iMaxLeaveDataCount);
				//RequestData(m_MmiRequestSys);
			}
		}
	}
*/
	if ( NULL != pResult )
	{
		*pResult = 1;
	}
}

void CIoViewBlockReport::RequestData( CMmiCommBase &req )
{
	//if ( IsWindowVisible() )
	{
		DoRequestViewData(req);
	}
}

void CIoViewBlockReport::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	//RequestViewDataCurrentVisibleRow();
}

void CIoViewBlockReport::OnLogicBlockDataUpdate( const IdArray &aUpdatedBlockIds )
{
	CCellRange rangeVisible = m_GridCtrl.GetVisibleNonFixedCellRange();
	if ( !rangeVisible.IsValid() )
	{
		return;
	}

	//DWORD dwTime = timeGetTime();
	typedef set<int32> IdSet;
	IdSet ids;
	int32 i = 0;
	for ( i=0; i < m_aShowBlockIds.GetSize() ; ++i )
	{
		ids.insert(m_aShowBlockIds[i]);
	}
	IdArray aMyBlockIds;
	aMyBlockIds.SetSize(0, aUpdatedBlockIds.GetSize());
	for ( i=0; i < aUpdatedBlockIds.GetSize() ; ++i )
	{
		int32 iId = aUpdatedBlockIds[i];
		if ( ids.count(iId) > 0 )
		{
			aMyBlockIds.Add(iId);
		}
	}

	if ( aMyBlockIds.GetSize() > 20 )
	{
		m_GridCtrl.Refresh();
	}
	else
	{
		// ���ʺ���������
		for ( int i=0; i < aMyBlockIds.GetSize() ; i++ )
		{
			UpdateTableContent(0, aMyBlockIds[i], false);
		}
	}
	//TRACE(_T("����%d: %d ms\n"), aUpdatedBlockIds.GetSize(), timeGetTime()-dwTime);

	if ( aMyBlockIds.GetSize() > 0 )
	{
		DoLocalSort();	// ���Ա�������ĸ���
	}
}

void CIoViewBlockReport::OnBlockSortDataUpdate( const CMmiRespBlockSort *pRespBlockSort )
{
	ASSERT( NULL != pRespBlockSort );
	int32 iTab;
	if ( m_MmiRequestBlockSort.m_iStart == pRespBlockSort->m_iStart
		&& m_MmiRequestBlockSort.m_eBlockSortType == pRespBlockSort->m_eBlockSortType
		&& m_MmiRequestBlockSort.m_bDescSort == pRespBlockSort->m_bDescSort
		/*&& m_MmiRequestBlockSort.m_iCount == pReqBlockSort->m_iCount*/		// ȫ�����ݣ�����Ƚ�
		&& TabIsValid(iTab) )
	{
		if ( pRespBlockSort->m_aBlockIDs.GetSize() > 0 )
		{
			IdArray aIdMy;
			aIdMy.SetSize(0, m_aTabInfomations[iTab].m_aSubBlockIds.GetSize());
			IdArray aIdLocal;
			aIdLocal.Copy(m_aTabInfomations[iTab].m_aSubBlockIds);
			for ( int j=0; j < pRespBlockSort->m_aBlockIDs.GetSize() ; j++ )
			{
				int32 iBlockId = pRespBlockSort->m_aBlockIDs[j];
				for ( int i=0; i < aIdLocal.GetSize() ; i++ )
				{
					if ( aIdLocal[i] == iBlockId )
					{
						aIdMy.Add(iBlockId);
						aIdLocal.RemoveAt(i);
						break;
					}
				}
			}

			// ʣ�µı���id����û�з������������ˣ�ȫ�����뵽���
			aIdMy.Append(aIdLocal);
			
			{
				m_aShowBlockIds.Copy(aIdMy);		// ������ذ���뱾�ذ�����ݲ�һֱ��ô�� - ������ѡ�Ͳ���ӷ�����������
				m_GridCtrl.SetRowCount(m_GridCtrl.GetFixedRowCount() + m_aShowBlockIds.GetSize());

				m_GridCtrl.SetSortColumn(m_iSortColumn);
				m_GridCtrl.SetSortAscending(!m_MmiRequestBlockSort.m_bDescSort);

				RequestViewDataCurrentVisibleRow();	// �������������ݸ���
				m_GridCtrl.Refresh();
			}
		}
	}
}

LRESULT CIoViewBlockReport::OnScrollPosChange( WPARAM w, LPARAM l )
{
	//if ( (HWND)w == m_XSBVert.m_hWnd )
	//{
	//	// �������а�鶼�������Բ���Ҫ�����
	//	SetTimer(KChangeVisibleBlocksTimerId, KChangeVisibleBlocksTimerPeriod, NULL);	// ������ʱ��������ǰ�ɼ���Ʒ����
	//}

	return 1;
}

void CIoViewBlockReport::OnVDataPluginResp( const CMmiCommBase *pResp )
{
	if ( NULL == pResp )
	{
		return;
	}
	
	if ( pResp->m_eCommType != ECTRespPlugIn )
	{
		ASSERT( 0 );
		return;
	}
	
	const CMmiCommBasePlugIn *pRespPlugin = (CMmiCommBasePlugIn *)pResp;
	if ( pRespPlugin->m_eCommTypePlugIn == ECTPIRespBlockSort )
	{
		OnBlockSortDataUpdate((const CMmiRespBlockSort *)pRespPlugin);
	}
	else if ( pRespPlugin->m_eCommTypePlugIn == ECTPIRespBlock
		|| pRespPlugin->m_eCommTypePlugIn == ECTPIRespAddPushBlock )
	{
		OnBlockDataResp((const CMmiRespLogicBlock *)pResp);
	}
}

void CIoViewBlockReport::OnBlockDataResp( const CMmiRespLogicBlock *pResp )
{
	TRACE(_T("�յ�������ݸ���: %d ��\r\n"), pResp->m_aBlockData.GetSize());
	int i=0;
	CBlockConfig::IdArray aUpdatedBlockIds;
	for ( i=0; i < pResp->m_aBlockData.GetSize(); i++ )
	{
		CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(pResp->m_aBlockData[i].m_iBlockId);
		if ( pBlock != NULL )
		{
			pBlock->m_logicBlockData = pResp->m_aBlockData[i];
			aUpdatedBlockIds.Add(pResp->m_aBlockData[i].m_iBlockId);
		}
	}
	
	if ( aUpdatedBlockIds.GetSize() <= 0 )
	{
		return;
	}
	
	OnLogicBlockDataUpdate(aUpdatedBlockIds);
}

void CIoViewBlockReport::OnContextMenu( CWnd* pWnd, CPoint pos )
{
	CRect rcClient;
	GetWindowRect(rcClient);
	if ( !rcClient.PtInRect(pos) )
	{
		pos = rcClient.TopLeft();
		DoTrackMenu(pos);
		return;
	}
	
	if ( pWnd->GetSafeHwnd() == m_GridCtrl.m_hWnd )
	{
		CIoViewBase::OnContextMenu(pWnd, pos);
		return;
	}
	
	DoTrackMenu(pos);
}

void CIoViewBlockReport::DoTrackMenu(CPoint pos)
{
	CNewMenu menu;
	menu.CreatePopupMenu();
	
	int32 iCmd = 1;
	menu.AppendMenu(MF_STRING, iCmd++, _T("�򿪰��"));
	menu.SetDefaultItem(1, FALSE);
	menu.AppendMenu(MF_SEPARATOR);

	// ������
	BOOL bShowGrid = m_GridCtrl.GetGridLines() != GVL_NONE;
	menu.AppendMenu(MF_STRING, iCmd++, bShowGrid ? _T("����������"): _T("��ʾ������"));
	menu.AppendMenu(MF_SEPARATOR);
	
	for ( int i=0; i < m_aTabInfomations.GetSize() ; i++ )
	{
		menu.AppendMenu(MF_STRING, iCmd++, m_aTabInfomations[i].m_StrName);
	}

	// ��ͨ��ť
	if ( m_aTabInfomations.GetSize() > 0 )
	{
		menu.AppendMenu(MF_SEPARATOR);
	}
	// ��������:
 	CNewMenu* pIoViewPopMenu = menu.AppendODPopupMenu(L"��������");
 	ASSERT(NULL != pIoViewPopMenu );
 	AppendIoViewsMenu(pIoViewPopMenu, IsLockedSplit());
	
	// �ر�����:
	menu.AppendMenu(MF_STRING, IDM_IOVIEWBASE_CLOSECUR, _T("�ر�����"));
	
	// �����л�:	
	menu.AppendMenu(MF_STRING, IDM_IOVIEWBASE_TAB, _T("�����л� TAB"));
	menu.AppendMenu(MF_SEPARATOR);

	// ȫ��/�ָ�
	menu.AppendODMenu(L"ȫ��/�ָ� F7", MF_STRING, IDM_IOVIEWBASE_F7);
	menu.AppendODMenu(L"", MF_SEPARATOR);

	
	// �ָ��
	CNewMenu menuSplit;
	menuSplit.CreatePopupMenu();
	menuSplit.AppendMenu( MF_STRING, IDM_IOVIEWBASE_SPLIT_TOP,    L"�������ͼ");
	menuSplit.AppendMenu( MF_STRING, IDM_IOVIEWBASE_SPLIT_BOTTOM, L"�������ͼ");
	menuSplit.AppendMenu( MF_STRING, IDM_IOVIEWBASE_SPLIT_LEFT,   L"�������ͼ");
	menuSplit.AppendMenu( MF_STRING, IDM_IOVIEWBASE_SPLIT_RIGHT,  L"�������ͼ");
	menu.AppendMenu(MF_POPUP, (UINT)menuSplit.m_hMenu, _T("�ָ��"));
	
	// �رմ���
	menu.AppendMenu(MF_STRING, IDM_IOVIEWBASE_CLOSE, _T("�رմ���"));
	menu.AppendMenu(MF_SEPARATOR);
	
	// �������
	//menu.AppendMenu(MF_STRING, ID_SETTING, _T("�������"));
	menu.AppendODMenu(L"�������", MF_STRING, ID_SETTING);
	
	// ���沼��
	//menu.AppendMenu(MF_STRING, ID_LAYOUT_ADJUST, _T("���沼��"));
	menu.AppendODMenu(L"���沼��", MF_STRING, ID_LAYOUT_ADJUST);

	menu.LoadToolBar(g_awToolBarIconIDs);

	// ������������ָ�״̬����Ҫɾ��һЩ��ť
	CMPIChildFrame *pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pChildFrame && pChildFrame->IsLockedSplit() )
	{
		pChildFrame->RemoveSplitMenuItem(menu);
	}

	int32 iRet = menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_TOPALIGN |TPM_RETURNCMD |TPM_NONOTIFY, pos.x, pos.y, AfxGetMainWnd());
	if ( iRet > 0 && iRet < iCmd )
	{
		if ( iRet == 1 )
		{
			CCellRange rangeSel = m_GridCtrl.GetSelectedCellRange();
			int32 iBlockId;
			if ( rangeSel.IsValid() && (iBlockId = m_GridCtrl.GetItemData(rangeSel.GetMinRow(), 0)) != CBlockInfo::GetInvalidId() )
			{
				OnDblClick(iBlockId);
			}
		}
		else if ( 2 ==  iRet )
		{
			m_GridCtrl.ShowGridLine(bShowGrid ? GVL_NONE : GVL_BOTH);
			m_GridCtrl.Refresh();
		}
		else
		{
			iRet -= 3;
			if ( iRet >=0 && iRet < m_aTabInfomations.GetSize() )
			{
				SetTab(iRet);
			}
		}
	}
	else
	{
		// ��ͨ����
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, MAKEWPARAM(iRet, 0), 0);
	}
}

CMerch* CIoViewBlockReport::GetBlockMerchXml()
{
	CMerch *pMerch = NULL;
	CCellRange rangeSel = m_GridCtrl.GetSelectedCellRange();
	int32 iBlockId;
	if ( rangeSel.IsValid() && (iBlockId = m_GridCtrl.GetItemData(rangeSel.GetMinRow(), 0)) != CBlockInfo::GetInvalidId() )
	{
		CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(iBlockId);
		if ( NULL != pBlock && pBlock->m_blockInfo.m_pMerch )
		{
			pMerch = pBlock->m_blockInfo.m_pMerch;
		}
	}
	if ( NULL == pMerch )
	{
		pMerch = CIoViewBase::GetMerchXml();
	}
	return pMerch;
}

CMerch * CIoViewBlockReport::GetMerchXml()
{
	CMerch *pMerch = NULL;
	CCellRange rangeSel = m_GridCtrl.GetSelectedCellRange();
	int32 iBlockId;
	if ( rangeSel.IsValid() && (iBlockId = m_GridCtrl.GetItemData(rangeSel.GetMinRow(), 0)) != CBlockInfo::GetInvalidId() )
	{
		CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(iBlockId);
		if ( NULL != pBlock && !pBlock->m_logicBlockData.m_StrMerchName.IsEmpty() )
		{
			pMerch = pBlock->m_blockInfo.FindMerchByCnName(pBlock->m_logicBlockData.m_StrMerchName);
		}
	}
	if ( NULL == pMerch )
	{
		pMerch = CIoViewBase::GetMerchXml();
	}
	return pMerch;
}

void CIoViewBlockReport::GetAttendMarketDataArray( OUT AttendMarketDataArray &aAttends )
{
	aAttends.RemoveAll();
	CBlockCollection *pCollection = GetCurrentCollection();
	CBlockConfig::BlockArray aSubBlocks;
	if ( NULL == pCollection )
	{
		// �Ƿ�Ϊ���е�
		int32 iTab;
		if ( TabIsValid(iTab) && m_aTabInfomations[iTab].m_iBlockCollectionId == T_BlockReportTab::BCAll )
		{
			ASSERT( 0 == iTab );
			for ( int i=1; i < m_aTabInfomations.GetSize() ; i++ )
			{
				IdArray aIds;
				CBlockCollection *pTmpCollection = CBlockConfig::Instance()->GetBlockCollectionById(m_aTabInfomations[i].m_iBlockCollectionId);
				ASSERT( NULL != pTmpCollection );
				if ( NULL != pTmpCollection )
				{
					aSubBlocks.Append(pTmpCollection->m_aBlocks);
				}
			}
		}
		else
		{
			ASSERT( 0 );
		}
	}
	else
	{
		aSubBlocks.Copy(pCollection->m_aBlocks);
	}
	if ( aSubBlocks.GetSize() > 0 )
	{
		typedef CMap<int32, int32, int32, int32> MarketMap;
		MarketMap ms;
		T_AttendMarketData data;
		for ( int32 i=0; i < aSubBlocks.GetSize() ; i++ )
		{
			CBlockLikeMarket *pBlock = aSubBlocks[i];
			if ( NULL != pBlock )
			{
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
						POSITION pos = pBlock->m_blockInfo.m_mapSubMerchMarkets.GetStartPosition();
						while ( NULL != pos )
						{
							int32 iMarket, iValue;
							pBlock->m_blockInfo.m_mapSubMerchMarkets.GetNextAssoc(pos, iMarket, iValue);
							ASSERT( iValue > 0 );
							ms[iMarket] = EDSTGeneral;
						}
					}
					break;
				default:
					ASSERT( 0 );
				}
			}
		}

		POSITION pos = ms.GetStartPosition();
		while ( NULL != pos )
		{
			int32 iMarket, iDT;
			ms.GetNextAssoc(pos, iMarket, iDT);

			data.m_iMarketId = iMarket;
			data.m_iEDSTypes = iDT;
			aAttends.Add(data);
		}
	}
}

CBlockCollection * CIoViewBlockReport::GetCurrentCollection()
{
	int32 iTab;
	if ( TabIsValid(iTab) )
	{
		CBlockCollection *pCollection = CBlockConfig::Instance()->GetBlockCollectionById(m_aTabInfomations[iTab].m_iBlockCollectionId);
		return pCollection;
	}
	return NULL;
}

void CIoViewBlockReport::SortColumn( int32 iSortCol/*=-1*/, int32 iSortType/*=0*/ )
{
	CStringArray aCols;
	CBlockDataPseudo::GetDefaultColNames(aCols);
	if ( iSortCol >= 0 && iSortCol < aCols.GetSize() )
	{
		CCellID cell(0, iSortCol);
		DoFixedRowClickCB(cell, iSortType);
	}
}

void CIoViewBlockReport::DoFixedRowClickCB( CCellID& cell, int32 iSortType /*= 0*/ )
{
	// ����ȫ���룬�����Ż�
    if (!m_GridCtrl.IsValid(cell) )
	{
		return;
	}

	m_iSortColumnFromXml = -1;
	m_iSortTypeFromXml = 0;	// ����xml�е�������
	
	if ( cell.col == 0 )
	{
		if ( m_iSortColumn != -1 )
		{
			// ��ԭ
			ResetBlockShowData();
		}
		m_iSortColumn = -1;
		return;
	}
	// ��Ż�ԭ˳�����Ʊ��أ�����������
	// ��ѡ��? �����ֶζ��Ǳ�����	��ѡ����Ҫ�������й���֤ȯ���ݣ�Ȼ�����
	// ���ñ���������	
	bool32 bUseLocalSort = false;
	
	if ( 1 == cell.col )	
	{
		// ���� ����
		bUseLocalSort = true;
	}
	else if ( CBlockConfig::Instance()->IsUserCollection(m_iBlockClassId) )
	{
		// ��ѡ�� ����
		bUseLocalSort = true;
	}
	else
	{
		// ���Ƿ�
		// Ȩ�Ƿ�
		// ������
		// ��ӯ�� - ���������ݣ�������
		
		
		// ����ͷ
		CBlockDataPseudo::E_Header eHeader = (CBlockDataPseudo::E_Header)m_GridCtrl.GetItemData(0, cell.col);
		ASSERT( eHeader >= CBlockDataPseudo::ShowRowNo && eHeader < CBlockDataPseudo::HeaderCount );
		E_BlockSort eSortHeader;
		if ( CBlockDataPseudo::ConvertNativeHeaderToSortHeader(eHeader, eSortHeader) )	// ��Ҫ���������ݵ��ֶ�
		{
			// ����, �����ʾblock, �ӷ������������ݣ� �ȴ��ӷ��������ݻ�����������
			// ��0��ʼ��ʾ
			if ( m_iSortColumn != -1 && m_iSortColumn != cell.col )	// ����ǰ�������Ƚ�GridCtrl���������
			{
				//m_GridCtrl.DeleteNonFixedRows();	// ��Ȼ�鷳�����ǵ��÷���
				//m_GridCtrl.SetRowCount(m_GridCtrl.GetFixedRowCount() + m_aShowBlockIds.GetSize());
				ResetBlockShowData(false);	// ��ԭ��δ����״̬
			}
			
			// Ҳ���Բ�������ݣ�ֻ���룬�ȴ����ݻ�����Ȼ���滻��ʾ���ݣ������������
			for ( int i=0; i < m_aShowBlockIds.GetSize() ; i++ )
			{
				m_aShowBlockIds[i] = CBlockInfo::GetInvalidId();
			}
			
			SetTimer(KRequestSortDataTimerId, KRequestSortDataTimerPeriod, NULL);	// ������ʱ����������
			
			m_MmiRequestBlockSort.m_eBlockSortType = eSortHeader;
			if ( 0 == iSortType )
			{
				m_MmiRequestBlockSort.m_bDescSort = m_iSortColumn == cell.col ? !m_MmiRequestBlockSort.m_bDescSort : TRUE;
			}
			else
			{
				m_MmiRequestBlockSort.m_bDescSort = iSortType > 0 ? TRUE : FALSE;	// > 0 ����; <0 ����
			}
			
			m_bRequestViewSort = true;
			m_iSortColumn = cell.col;
			
			//m_GridCtrl.EnsureVisible(m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetFixedColumnCount());
			//m_GridCtrl.EnsureTopLeftCell(m_GridCtrl.GetFixedRowCount(), m_GridCtrl.GetFixedColumnCount());
			m_GridCtrl.EnsureVisible(m_GridCtrl.GetFixedRowCount(), cell.col);	// ��֤��һ�пɼ�
			m_GridCtrl.Refresh();
			
			RequestSortData();
		}
		else
		{
			bUseLocalSort = true;		// ���������ֶ�ʹ�ñ�񱾵�����
		}
	}
	
	if ( bUseLocalSort )
	{
		if ( m_iSortColumn != -1 && m_iSortColumn != cell.col )		// ����Ǵӷ����������ֶ��л��������Ҫ���������
		{
			ResetBlockShowData(false);
			//m_GridCtrl.EnsureVisible(m_GridCtrl.GetFixedRowCount(), cell.col);	// ��һ�пɼ�
		}
		m_bRequestViewSort = false;
		m_iSortColumn = -1;
		
		m_spThis = this;
		m_GridCtrl.SetHeaderSort(TRUE);
		
		m_iSortColumn = cell.col;
		
		CPoint pt(0,0);
		m_GridCtrl.OnFixedRowClick(cell,pt);
		
		m_GridCtrl.Refresh();
	}
}

void CIoViewBlockReport::WatchDayChange(bool32 bDrawIfChange /*= false*/)
{
	if(NULL == m_pAbsCenterManager)
	{
		return;
	}

	CGmtTime timeNow = m_pAbsCenterManager->GetServerTime();
	tm tmLocal;
	timeNow.GetLocalTm(&tmLocal);
	CMerch *pMerchSZA = NULL;
	if ( m_pAbsCenterManager->GetMerchManager().FindMerch(_T("000001"), 0, pMerchSZA) )
	{
		CMarketIOCTimeInfo ioc;
		pMerchSZA->m_Market.GetRecentTradingDay(timeNow, ioc, pMerchSZA->m_MerchInfo);
		ioc.m_TimeInit.m_Time.GetLocalTm(&tmLocal);	// ʹ�ý�������Ϣ
	}
	if ( m_tmDisplay.tm_yday != tmLocal.tm_yday )
	{
		m_tmDisplay = tmLocal;
		if ( bDrawIfChange )
		{
			InvalidateRect(m_RectTitle);
		}
	}
}

void CIoViewBlockReport::DoLocalSort()
{
	if ( -1 != m_iSortColumn
		&& !m_bRequestViewSort
		&& m_iSortColumn > 1 )	// �Ǵ��뱾��������
	{
		m_spThis = this;
		m_GridCtrl.SortItems(m_iSortColumn, m_GridCtrl.GetSortAscending());
	}
}

//////////////////////////////////////////////////////////////////////////
//
CIoViewBlockReport::T_BlockReportTab::T_BlockReportTab( const T_BlockReportTab &tab )
{
	m_iBlockCollectionId = tab.m_iBlockCollectionId;
	m_iColLeftVisible = tab.m_iColLeftVisible;
	m_iStartRow = tab.m_iStartRow;
	m_iRowCount = tab.m_iRowCount;
	m_StrName = tab.m_StrName;
	m_aSubBlockIds.Copy(tab.m_aSubBlockIds);
}

const CIoViewBlockReport::T_BlockReportTab & CIoViewBlockReport::T_BlockReportTab::operator=( const T_BlockReportTab &tab )
{
	if ( this == &tab )
	{
		return *this;
	}
	m_iBlockCollectionId = tab.m_iBlockCollectionId;
	m_iColLeftVisible = tab.m_iColLeftVisible;
	m_iStartRow = tab.m_iStartRow;
	m_iRowCount = tab.m_iRowCount;
	m_StrName = tab.m_StrName;
	m_aSubBlockIds.Copy(tab.m_aSubBlockIds);
	return *this;
}
