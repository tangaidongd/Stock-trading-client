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
#include "DlgPhaseSort.h"
#include "IoViewPhaseSort.h"
#include "LogFunctionTime.h"

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
//    "fatal error C1001: INTERNAL COMPILER ERROR"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define  ID_CALCMSG_START  (UM_CALCMSG+EPSCM_Start)
#define  ID_CALCMSG_POS  (UM_CALCMSG+EPSCM_Pos)
#define  ID_CALCMSG_END  (UM_CALCMSG+EPSCM_End)
#define  ID_CALCMSG_REQUST_DATA  (UM_CALCMSG+EPSCM_RequestData)

#define  ID_GRDID_CTRL 12366

// xml
const char *KStrXMLIOPSTimeStart		= ("TimeStart");
const char *KStrXMLIOPSTimeEnd			= ("TimeEnd");
const char *KStrXMLIOPSPhaseSortType	= ("SortType");
const char *KStrXMLIOPSPreWeight		= ("PreWeight");
const char *KStrXMLIOPSBlockId			= ("BlockId");

const int32 KWatchTimeChangeTimerId						= 100006;	// ÿ�� n ����, ���²鿴ʱ���Ƿ����
const int32 KWatchTimeChangeTimerPeriod					= 60*1000;					

// �̶� Tab ��������Ŀ. �����һ��Tab ��Ϊ�����ʾǰ��û�еİ��

static const int32 KiIDTabAll			= -1;   // ȫ��
static const int32 KiIDTabUserOwn		= -2;	// ��ѡ�ɼ���
static const int32 KiIDTabChgable		= -3;	// ���һ��Tab �����Լ��ı��
static const int32 KiIDTabFenLei		= -4;	// ������

static const int32 KiIDTabAllExcept		= 2;	// ȫ����ȥ��������

// ��ǩҳ���鶨��
static const T_SimpleTabInfo  s_KaTabInfos[] = 
{
	T_SimpleTabInfo(KiIDTabFenLei,	2003,	0,		L"���з����",		ETITCollection,		T_BlockDesc::EBTBlockLogical),
	T_SimpleTabInfo(KiIDTabAll,		4075,	1,		L"��鼯�ϡ�",		ETITCollection,		T_BlockDesc::EBTBlockLogical),
	T_SimpleTabInfo(KiIDTabUserOwn,	-1,		2,		L"��ѡ����",		ETITCollection,		T_BlockDesc::EBTUser),
	T_SimpleTabInfo(2000,			2000,	3,		L"�������",				ETITEntity,			T_BlockDesc::EBTBlockLogical),
	T_SimpleTabInfo(2005,			2005,	4,		L"��С��ҵ",				ETITEntity,			T_BlockDesc::EBTBlockLogical),
	T_SimpleTabInfo(2006,			2006,	5,		L"��ҵ��",				ETITEntity,			T_BlockDesc::EBTBlockLogical),
	T_SimpleTabInfo(2007,			2007,	6,		L"����¹�",				ETITEntity,			T_BlockDesc::EBTBlockLogical),
	T_SimpleTabInfo(2008,			2008,	7,		L"�������",				ETITEntity,			T_BlockDesc::EBTBlockLogical),
	T_SimpleTabInfo(2010,			2010,	8,		L"����Ȩ֤",				ETITEntity,			T_BlockDesc::EBTBlockLogical),
};

static const int32 s_KiTabInfoCount = sizeof(s_KaTabInfos)/sizeof(T_SimpleTabInfo);

	
IMPLEMENT_DYNCREATE(CIoViewPhaseSort, CIoViewBase)
CIoViewPhaseSort *CIoViewPhaseSort::m_spThis = NULL;
CIoViewPhaseSort::MerchRequestTimeMap CIoViewPhaseSort::s_mapMerchReqTime;

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewPhaseSort, CIoViewBase)
//{{AFX_MSG_MAP(CIoViewPhaseSort)
ON_WM_PAINT()
ON_WM_CREATE()
ON_WM_SIZE()
ON_WM_TIMER()
ON_WM_CONTEXTMENU()
ON_NOTIFY(TCN_SELCHANGE, 0x9999, OnSelChange)
ON_MESSAGE_VOID(UM_BLOCK_DOINITIALIZE, OnDoBlockReportInitialize)
ON_MESSAGE_VOID(UM_DOCALC, OnMsgDoCalc)
ON_MESSAGE(ID_CALCMSG_START, OnMsgCalcStart)
ON_MESSAGE(ID_CALCMSG_POS, OnMsgCalcPos)
ON_MESSAGE(ID_CALCMSG_END, OnMsgCalcEnd)
ON_MESSAGE(ID_CALCMSG_REQUST_DATA, OnMsgCalcReqData)
//}}AFX_MSG_MAP
ON_NOTIFY(NM_RCLICK,ID_GRDID_CTRL,OnGridRButtonDown)
ON_NOTIFY(NM_DBLCLK, ID_GRDID_CTRL, OnGridDblClick)
ON_NOTIFY(GVN_COLWIDTHCHANGED, ID_GRDID_CTRL, OnGridColWidthChanged)
ON_NOTIFY(GVN_KEYDOWNEND, ID_GRDID_CTRL, OnGridKeyDownEnd)
ON_NOTIFY(GVN_KEYUPEND, ID_GRDID_CTRL, OnGridKeyUpEnd)
ON_NOTIFY(GVN_GETDISPINFO, ID_GRDID_CTRL, OnGridGetDispInfo)
ON_NOTIFY(GVN_ODCACHEHINT, ID_GRDID_CTRL, OnGridCacheHint)
END_MESSAGE_MAP()
///////////////////////////////////////////////////////////////////////////////

// ׼���Ż� fangz20100514

CIoViewPhaseSort::CIoViewPhaseSort()
:CIoViewBase()
{
	m_pParent		= NULL;
	m_rectClient	= CRect(-1,-1,-1,-1);
	m_RectTitle.SetRectEmpty();

	m_iSortColumn		= -1;
	
	m_aUserBlockNames.RemoveAll();

	m_PhaseOpenBlockParam.m_iBlockId = CBlockInfo::GetInvalidId();		// ������Ч��
	
	m_bBlockReportInitialized = false;		// ����ʼ��

	m_tmDisplay.tm_year = 0;

	m_iPreTab = m_iCurTab = -1;

	m_eStage = ES_Count;

	m_PhaseOpenBlockParam.m_TimeStart = m_PhaseOpenBlockParam.m_TimeEnd + CGmtTimeSpan(1,0,0,0);	// Ĭ�ϲ�����Ч��������Ĭ�Ͼʹ�
	m_PhaseBlockParamXml.m_TimeStart = m_PhaseBlockParamXml.m_TimeEnd + CGmtTimeSpan(1,0,0,0);
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewPhaseSort::~CIoViewPhaseSort()
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
void CIoViewPhaseSort::OnPaint()
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

	if ( clrBk == CFaceScheme::Instance()->GetSysColor(ESCBackground) )
	{
		clrBk = RGB(44, 0, 0);
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


	for ( int32 i=0; i < m_aTitleString.GetSize() ; i++ )
	{
		dc->SetTextColor(m_aTitleString[i].m_clr);
		dc->DrawText(m_aTitleString[i].m_StrTitle, rcDraw, DT_SINGLELINE |DT_LEFT |DT_VCENTER);
		CSize sizeText = dc->GetTextExtent(m_aTitleString[i].m_StrTitle);
		rcDraw.left += sizeText.cx;
	}

	dc.FillSolidRect(rectClient.left, m_RectTitle.bottom-1, rectClient.Width(), 1, clrAxis);

	if ( m_bActive )
	{
		dc.FillSolidRect(3, 2, 2, 2, clrVol);
	}

	dc.RestoreDC(iSaveDC);
}

BOOL CIoViewPhaseSort::PreTranslateMessage(MSG* pMsg)
{
	if ( WM_KEYDOWN == pMsg->message )
	{
		if ( VK_F8 == pMsg->wParam && m_PhaseOpenBlockParam.m_TimeStart <= m_PhaseOpenBlockParam.m_TimeEnd )
		{
			T_PhaseOpenBlockParam BlockParam = m_PhaseOpenBlockParam;
			OpenBlock(BlockParam, true);	// ���´�
			return TRUE;
		}
	}

	return CIoViewBase::PreTranslateMessage(pMsg);
}

int CIoViewPhaseSort::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CIoViewBase::OnCreate(lpCreateStruct);
	
	InitialIoViewFace(this);

	//����Tab ��
	m_GuiTabWnd.Create(WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,0x9999);
	m_GuiTabWnd.SetBkGround(false,GetIoViewColor(ESCBackground),0,0);
	// m_GuiTabWnd.SetImageList(IDB_TAB, 16, 16, 0x0000ff);
	m_GuiTabWnd.SetUserCB(this);
	m_GuiTabWnd.SetALingTabs(CGuiTabWnd::ALN_BOTTOM);

	// �����������
	m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10203);
	m_XSBVert.SetScrollRange(0, 10);

	m_XSBHorz.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10204);
	m_XSBHorz.SetScrollRange(0, 10);

	// �������ݱ��
	m_GridCtrl.Create(CRect(0, 0, 0, 0), this, ID_GRDID_CTRL);
// 	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetBackClr(CLR_DEFAULT);
// 	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetTextClr(CLR_DEFAULT);
	m_GridCtrl.SetTextColor(CLR_DEFAULT);
	m_GridCtrl.SetFixedTextColor(CLR_DEFAULT);
	m_GridCtrl.SetTextBkColor(CLR_DEFAULT);
	m_GridCtrl.SetFixedTextColor(CLR_DEFAULT);
	m_GridCtrl.SetBkColor(CLR_DEFAULT);
	m_GridCtrl.SetFixedBkColor(CLR_DEFAULT);
	
	m_GridCtrl.SetDefCellWidth(60);
	m_GridCtrl.EnableBlink(FALSE);
	m_GridCtrl.SetVirtualMode(TRUE);
	m_GridCtrl.SetVirtualCompare(CompareRow);
	m_GridCtrl.SetDefaultCellType(RUNTIME_CLASS(CGridCellNormalSys));
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE| DT_NOPREFIX);

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
	m_GridCtrl.SetDrawFixedCellGridLineAsNormal(TRUE); 
	m_GridCtrl.SetInitSortAscending(FALSE);

	////////////////////////////////////////////////////////////////
	m_GridCtrl.SetFixedColumnCount(2);	// �� ��
	m_GridCtrl.SetFixedRowCount(1);

	// ��������
	LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);

	PostMessage(UM_BLOCK_DOINITIALIZE, 0, 0);

	// ���ʱ����Գ�ʼ��tab��Դ�˵ģ�����ڴ�ʱӦ�����غ��ˣ����û����Ӧ����do initialize�г�ʼ������tab
	// �̶���tabҳ�����ڼ���
	InitializeTabs();	

	//
	InitializeTitleString();

	// ������ʾ��ʱ�䣬���Ҫ��ʾ�Ļ�
	SetTimer(KWatchTimeChangeTimerId, KWatchTimeChangeTimerPeriod , NULL);
	CGmtTime timeNow = m_pAbsCenterManager->GetServerTime();
	timeNow.GetLocalTm(&m_tmDisplay);

	// �������
	CBlockConfig::Instance()->AddListener(this);

	return 0;
}

void CIoViewPhaseSort::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);

	RecalcLayout();
}

BOOL CIoViewPhaseSort::TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return FALSE;
}

void CIoViewPhaseSort::OnIoViewActive()
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

	Invalidate(TRUE);
}

void CIoViewPhaseSort::OnIoViewDeactive()
{
	m_bActive = false;
	
	Invalidate(TRUE);
}

void CIoViewPhaseSort::OnSelChange(NMHDR* pNMHDR, LRESULT* pResult) 
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

bool32 CIoViewPhaseSort::FromXml(TiXmlElement * pTiXmlElement)
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

	// ��ȡ����ĵ�ǰ�������ȴ�init
	pcAttrValue = pTiXmlElement->Attribute(KStrXMLIOPSTimeStart);
	if ( NULL != pcAttrValue )
	{
		m_PhaseBlockParamXml.m_TimeStart = (time_t)atol(pcAttrValue);
	}
	pcAttrValue = pTiXmlElement->Attribute(KStrXMLIOPSTimeEnd);
	if ( NULL != pcAttrValue )
	{
		m_PhaseBlockParamXml.m_TimeEnd = (time_t)atol(pcAttrValue);
	}
	
	pcAttrValue = pTiXmlElement->Attribute(KStrXMLIOPSPhaseSortType);
	if ( NULL != pcAttrValue )
	{
		E_PhaseSortType eSort = (E_PhaseSortType)atoi(pcAttrValue);
		if ( eSort < EPST_Count )
		{
			m_PhaseBlockParamXml.m_ePhaseSortType = eSort;
		}
	}

	pcAttrValue = pTiXmlElement->Attribute(KStrXMLIOPSPreWeight);
	if ( NULL != pcAttrValue )
	{
		m_PhaseBlockParamXml.m_bDoPreWeight = 0 != atol(pcAttrValue);
	}

	pcAttrValue = pTiXmlElement->Attribute(KStrXMLIOPSBlockId);
	if ( NULL != pcAttrValue )
	{
		m_PhaseBlockParamXml.m_iBlockId = atol(pcAttrValue);
	}

	// ��ʱTab��Ϣ�ǳ�ʼ�����˵�
	m_GuiTabWnd.SetBkGround(false,GetIoViewColor(ESCBackground),0,0);

	return true;
}

CString CIoViewPhaseSort::ToXml()
{	
	//
	CString StrThis;
	int32 iTimeStart = m_PhaseOpenBlockParam.m_TimeStart.GetTime();
	int32 iTimeEnd   = m_PhaseOpenBlockParam.m_TimeEnd.GetTime();

	StrThis.Format( L"<%s %s=\"%s\" %s=\"%s\" %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%d\" ", 
					CString(GetXmlElementValue()).GetBuffer(),
					CString(GetXmlElementAttrIoViewType()).GetBuffer(),	CIoViewManager::GetIoViewString(this).GetBuffer(),
					CString(GetXmlElementAttrShowTabName()).GetBuffer(), m_StrTabShowName.GetBuffer(),
					CString(KStrXMLIOPSTimeStart).GetBuffer(), iTimeStart,
					CString(KStrXMLIOPSTimeEnd).GetBuffer(), iTimeEnd,
					CString(KStrXMLIOPSPhaseSortType).GetBuffer(), m_PhaseOpenBlockParam.m_ePhaseSortType,
					CString(KStrXMLIOPSPreWeight).GetBuffer(), m_PhaseOpenBlockParam.m_bDoPreWeight,
					CString(KStrXMLIOPSBlockId).GetBuffer(), m_PhaseOpenBlockParam.m_iBlockId
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

CString CIoViewPhaseSort::GetDefaultXML()
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

void CIoViewPhaseSort::OnGridRButtonDown(NMHDR *pNotifyStruct, LRESULT* pResult)
{ 
	CPoint pos(0, 0);
	GetCursorPos(&pos);
	DoTrackMenu(pos);
}

void CIoViewPhaseSort::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW	*pGridView = (NM_GRIDVIEW *)pNotifyStruct;
	if ( NULL != pGridView && pGridView->iRow >= m_GridCtrl.GetFixedRowCount() )
	{
		CMerch *pMerch = GetMerchXml();
		if ( NULL != pMerch )
		{
			CMainFrame *pMainFrame =DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
			if ( NULL != pMainFrame )
			{
				pMainFrame->OnShowMerchInChart(pMerch, this);
			}
		}
	}
}

void CIoViewPhaseSort::OnGridColWidthChanged(NMHDR *pNotifyStruct, LRESULT* pResult)
{	
	
}

void CIoViewPhaseSort::OnGridKeyDownEnd(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	// ���¼�����ͷ��
	
}

void CIoViewPhaseSort::OnGridKeyUpEnd(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	// ���ϼ�����ͷ��
	
}

// ֪ͨ��ͼ�ı��ע����Ʒ
void CIoViewPhaseSort::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	if (m_pMerchXml == pMerch)
		return;
	
	// �޸ĵ�ǰ�鿴����Ʒ
	m_pMerchXml					= pMerch;
	m_MerchXml.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
	m_MerchXml.m_StrMerchCode	= pMerch->m_MerchInfo.m_StrMerchCode;
}

//
void CIoViewPhaseSort::OnVDataForceUpdate()
{
	if ( m_aSmartAttendMerchs.GetSize() > 0 )
	{
		RequestAttendMerchs(true);		// ������������������
	}
	TryRequestMoreNeedPriceMerchs();
}

void CIoViewPhaseSort::OnVDataRealtimePriceUpdate(IN CMerch *pMerch)
{
	// ÿ���г���ʼ��Ҳ����0����������, ����, ��Ҫ�������������
	for ( int32 i=0; i < m_aSmartAttendMerchs.GetSize() ; i++ )
	{
		if (m_pAbsCenterManager && pMerch == m_aSmartAttendMerchs[i].m_pMerch )
		{
			s_mapMerchReqTime[pMerch] = m_pAbsCenterManager->GetServerTime();	// �����������, �������г���ʼ��Ҳ��������

			m_aSmartAttendMerchs.RemoveAt(i);	// ����ɾ���ظ�???
			if ( m_aSmartAttendMerchs.GetSize() <= 0 )
			{
				TryRequestMoreNeedPriceMerchs();	// ������������
			}

			if ( ES_WaitPrice == m_eStage )
			{
				// ���ڵȴ���������
				const int32 iAttSize = m_aSmartAttendMerchs.GetSize() + m_aMerchsNeedPrice.GetSize();
				const int32 iMaxSize = m_ParamForDisplay.m_aMerchsToCalc.GetSize();
				
				m_DlgWait.SetProgress(iMaxSize-iAttSize, iMaxSize);
				TRACE(_T("%d-%d\r\n"), iMaxSize-iAttSize, iMaxSize);
				if ( iAttSize <= 0 )
				{
					// ���
					m_eStage = ES_WaitCalc;
					PostMessage(UM_DOCALC, 0, 0);	// ��ʼ����
				}
			}
			break;
		}
	}
}

// �������г����з����仯
// �������г����з����仯
void CIoViewPhaseSort::OnVDataReportUpdate(int32 iMarketId, E_MerchReportField eMerchReportField, bool32 bDescSort, int32 iPosStart, int32 iOrgMerchCount,  const CArray<CMerch*, CMerch*> &aMerchs)
{	
	
}

void CIoViewPhaseSort::OnVDataGridHeaderChanged(E_ReportType eReportType)
{
	
}

void CIoViewPhaseSort::OnFixedRowClickCB(CCellID& cell)
{
	// ����ȫ���룬�����Ż�
    if (!m_GridCtrl.IsValid(cell) )
	{
		return;
	}

	CBlockLikeMarket *pBlock = GetCurrentBlock();
	if ( NULL == pBlock )
	{
		return;
	}

	const int iCol = cell.col;
	ASSERT( iCol >= 0 && iCol < m_aColumnHeaders.GetSize() );
	E_PhaseHeaderType eHeader = m_aColumnHeaders[iCol].m_eHeaderType;
	if ( eHeader == EPHT_No )
	{
		// ��ԭ����
		if ( m_GridCtrl.GetSortColumn() >= 0 )
		{
			m_GridCtrl.SetSortColumn(-1);
			m_GridCtrl.ResetVirtualOrder();
			m_GridCtrl.Refresh();
		}
		return;
	}

	{
		m_iSortColumn = iCol;
		
		m_spThis = this;
		m_GridCtrl.SetHeaderSort(TRUE);

		m_iSortColumn = cell.col;
		
		CPoint pt(0,0);
		m_GridCtrl.OnFixedRowClick(cell,pt);
		
		m_GridCtrl.Refresh();
	}

}

void CIoViewPhaseSort::OnFixedColumnClickCB(CCellID& cell)
{

}

void CIoViewPhaseSort::OnHScrollEnd()
{
	
}

void CIoViewPhaseSort::OnVScrollEnd()
{
	
}

void CIoViewPhaseSort::OnCtrlMove( int32 x, int32 y )
{
	
}

bool32 CIoViewPhaseSort::OnEditEndCB ( int32 iRow,int32 iCol, const CString &StrOld, INOUT CString &StrNew )
{
	return false;
}

void CIoViewPhaseSort::OnDestroy()
{	
	CBlockConfig::Instance()->RemoveListener(this);		// ֹͣ����

	if ( m_ParamForCalc.m_eResultFlag == EPSCRF_Calculating )
	{
		ASSERT( 0 );
		CancelPhaseSortCalc(&m_ParamForCalc);	// ֹͣ����, ����еĻ�
	}

	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if ( NULL != pMainFrm && pMainFrm->m_pKBParent == this )
	{
		pMainFrm->SetHotkeyTarget(NULL);
	}

	CIoViewBase::OnDestroy();
}

void CIoViewPhaseSort::OnTimer(UINT nIDEvent) 
{
	if(!m_bShowNow)
	{
		return;
	}

	if (nIDEvent == KWatchTimeChangeTimerId )
	{
		if(m_pAbsCenterManager)
		{
			CGmtTime timeNow = m_pAbsCenterManager->GetServerTime();
			tm tmLocal;
			timeNow.GetLocalTm(&tmLocal);
			if ( m_tmDisplay.tm_yday != tmLocal.tm_yday )
			{
				m_tmDisplay = tmLocal;
				InvalidateRect(m_RectTitle);
			}
		}
	}
	CIoViewBase::OnTimer(nIDEvent);
}

void CIoViewPhaseSort::SetChildFrameTitle()
{
	CString StrTitle =  CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;
	
	CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pParentFrame)
	{
		pParentFrame->SetChildFrameTitle(StrTitle);
	}
}

bool32 CIoViewPhaseSort::GetStdMenuEnable( MSG* pMsg )
{
	return false;
}

void CIoViewPhaseSort::LockRedraw()
{
	
}

void CIoViewPhaseSort::UnLockRedraw()
{
	
}

void CIoViewPhaseSort::OnDblClick(CMerch *pMerch)
{
	
}

void CIoViewPhaseSort::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();
	Invalidate();
	m_GuiTabWnd.SetBkGround(false,GetIoViewColor(ESCBackground),0,0);
	m_GridCtrl.Refresh();
}

void CIoViewPhaseSort::OnIoViewFontChanged()
{
	CIoViewBase::OnIoViewFontChanged();

	LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);	

	RecalcLayout();
	SetColWidthAccordingFont();
	SetRowHeightAccordingFont();
	Invalidate();
	m_GridCtrl.Refresh();
}

void CIoViewPhaseSort::OnRButtonDown2( CPoint pt, int32 iTab )
{
	
}

bool32 CIoViewPhaseSort::OnLButtonDown2( CPoint pt, int32 iTab )
{
	// true����
	// �����е����ĸ�tab��ǰ��3����Ҫѡ��block�ģ�������ǵ���Ͳ���Ч��
	const T_SimpleTabInfo *pTab = GetSimpleTabInfo(iTab);
	if ( NULL != pTab && pTab->m_eTabType == ETITCollection )
	{
		// ��Ҫ�˵�
		CNewMenu menu;
		menu.CreatePopupMenu();

		CBlockLikeMarket *pBlockCur = GetCurrentBlock();
		
		typedef map<UINT, CBlockLikeMarket *> CmdToBlockMap;
		CmdToBlockMap mapIds;
		int32 iCmdBase = 10;
		switch ( pTab->m_iID )
		{
		case KiIDTabFenLei: // ����
			{
				CBlockConfig::BlockArray aBlocks;
				CBlockConfig::Instance()->GetMarketClassBlocks(aBlocks);
				for ( int32 i=0; i < aBlocks.GetSize() ; i++ )
				{
					menu.AppendMenu(MF_STRING, iCmdBase, aBlocks[i]->m_blockInfo.m_StrBlockName);
					if ( pBlockCur == aBlocks[i] )
					{
						menu.CheckMenuItem(iCmdBase, MF_CHECKED |MF_BYCOMMAND);
					}
					mapIds[iCmdBase++] = aBlocks[i];
				}

				// �����ҵ���ѡ
				T_Block *pBlockServer = CUserBlockManager::Instance()->GetServerBlock();
				if ( NULL != pBlockServer )
				{
					CBlockConfig::BlockArray aUserBlocks;
					CBlockConfig::Instance()->GetUserBlocks(aUserBlocks);
					CBlockLikeMarket *pBlockDefUser = NULL;
					for ( int32 i=0; i < aUserBlocks.GetSize() ; i++ )
					{
						if ( aUserBlocks[i]->m_blockInfo.m_StrBlockName == pBlockServer->m_StrName )
						{
							pBlockDefUser = aUserBlocks[i];
							break;
						}
					}
					if ( NULL != pBlockDefUser )
					{
						if ( menu.GetMenuItemCount() > 0 )
						{
							menu.AppendMenu(MF_SEPARATOR);
						}
						menu.AppendMenu(MF_STRING, iCmdBase, pBlockDefUser->m_blockInfo.m_StrBlockName);
						if ( pBlockCur == pBlockDefUser )
						{
							menu.CheckMenuItem(iCmdBase, MF_CHECKED |MF_BYCOMMAND);
						}
						mapIds[iCmdBase++] = pBlockDefUser;
					}
				}
			}
			break;
		case KiIDTabUserOwn:
			{
				CBlockConfig::BlockArray aBlocks;
				CBlockConfig::Instance()->GetUserBlocks(aBlocks);
				for ( int32 i=0; i < aBlocks.GetSize() ; i++ )
				{
					menu.AppendMenu(MF_STRING, iCmdBase, aBlocks[i]->m_blockInfo.m_StrBlockName);
					if ( pBlockCur == aBlocks[i] )
					{
						menu.CheckMenuItem(iCmdBase, MF_CHECKED |MF_BYCOMMAND);
					}
					mapIds[iCmdBase++] = aBlocks[i];
				}
			}
			break;
		case KiIDTabAll: // ����
			{
				// col - block
				// ��ʾȫ��
				CBlockConfig::IdArray aIdAll;
				CBlockConfig::Instance()->GetCollectionIdArray(aIdAll);
				//
				for ( int32 i = 0; i < aIdAll.GetSize(); i++ )
				{	
					//
					CBlockCollection *pCol = CBlockConfig::Instance()->GetBlockCollectionById(aIdAll[i]);
					if ( pCol->IsMarketClassBlockCollection() || pCol->IsUserBlockCollection() )
					{
						continue;
					}
					CBlockConfig::BlockArray aBlocks;
					pCol->GetValidBlocks(aBlocks);
					
					if ( aBlocks.GetSize() > 0 
						&& ( aBlocks[0]->m_blockInfo.GetBlockType() == CBlockInfo::typeNormalBlock				// ��ͨ || ����
						|| aBlocks[0]->m_blockInfo.GetBlockType() == CBlockInfo::typeMarketClassBlock )					
						)
					{
						CNewMenu *pColMenu = menu.AppendODPopupMenu(pCol->m_StrName);
						
						//
						ASSERT(NULL != pColMenu);
						
						//
						for ( int32 iBlock = 0; iBlock < aBlocks.GetSize(); iBlock++ )
						{						
							if ( NULL == aBlocks[iBlock] )
							{
								//ASSERT(0);
								continue;
							}
							
							//
							CString StrItem = aBlocks[iBlock]->m_blockInfo.m_StrBlockName;
							
							if ( StrItem.GetLength() > 7 )
							{
								StrItem = StrItem.Left(6) + _T("...");
							}
							
							//
							pColMenu->AppendODMenu(StrItem, MF_STRING, iCmdBase);
							if ( pBlockCur == aBlocks[iBlock] )
							{
								menu.CheckMenuItem(iCmdBase, MF_CHECKED |MF_BYCOMMAND);
							}
							mapIds[ iCmdBase++ ] = aBlocks[iBlock];
						}
						
						MultiColumnMenu(*pColMenu, 20);
					}
				}
			}
			break;
		}

		// �����˵�
		bool32 bSel = false;
		CPoint point = m_GuiTabWnd.GetLeftTopPoint(iTab);	
		int32 iRet = menu.TrackPopupMenu(TPM_BOTTOMALIGN |TPM_LEFTALIGN |TPM_NONOTIFY |TPM_RETURNCMD,
			point.x, point.y, AfxGetMainWnd());
		if ( iRet > 0 )
		{
			CmdToBlockMap::iterator it = mapIds.find(iRet);
			if ( it != mapIds.end() )
			{
				T_PhaseOpenBlockParam BlockParam = m_PhaseOpenBlockParam;
				BlockParam.m_iBlockId = it->second->m_blockInfo.m_iBlockId;
				bSel = OpenBlock(BlockParam, true);	// �˵�ѡ��������´�
			}
		}
		return !bSel;	// û��ѡ�������
	}
	return false;
}

bool32 CIoViewPhaseSort::BlindLButtonBeforeRButtonDown( int32 iTab )
{
	return true;	// �����Ҽ�
}

void CIoViewPhaseSort::SetTab(int32 iCurTab)
{
	int32 iCur = m_GuiTabWnd.GetCurtab();
	m_GuiTabWnd.SetCurtab(iCurTab);
	if ( iCur == iCurTab )	// ��ʱtab���ᷢ����Ϣ���Լ�����
	{
		TabChanged();
	}
}

void CIoViewPhaseSort::TabChanged()
{
	int32 iCurTab = m_GuiTabWnd.GetCurtab();
	const T_SimpleTabInfo *pTab = GetSimpleTabInfo(iCurTab);
	if ( NULL != pTab 
		&& pTab->m_eTabType == ETITEntity )
	{
		// ����ʵ��
		if ( NULL != CBlockConfig::Instance()->FindBlock(pTab->m_iID) )
		{
			T_PhaseOpenBlockParam BlockParam = m_PhaseOpenBlockParam;
			BlockParam.m_iBlockId = pTab->m_iID;
			OpenBlock( BlockParam, false );	// �������´�
		}
	}
}

void CIoViewPhaseSort::SetRowHeightAccordingFont()
{
	// �����б�ͷ��ȷ��ÿ���и�
	float fFixedBase = 0.0;
	float fNonFixedBase = 0.0f;
	for ( int32 i=0; i < m_aColumnHeaders.GetSize() ; i++ )
	{
		fFixedBase = max(fFixedBase, m_aColumnHeaders[i].m_fFixedRowHeightRatio);
		fNonFixedBase = max(fNonFixedBase, m_aColumnHeaders[i].m_fNonFixedRowHeightRatio);
	}

	// �������廹ԭ�и�
	LOGFONT *pFontNormal = GetIoViewFont(ESFNormal);
	m_GridCtrl.GetDefaultCell(FALSE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(FALSE, TRUE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, FALSE)->SetFont(pFontNormal);
	m_GridCtrl.GetDefaultCell(TRUE, TRUE)->SetFont(pFontNormal);

	int32 iFixedHeight = m_GridCtrl.GetDefCellHeight();
	int32 iNonFHeight  = iFixedHeight;
	if ( fFixedBase > 0.0f )
	{
		iFixedHeight = (int32)(fFixedBase * iFixedHeight);
	}
	if ( fNonFixedBase > 0.0f )
	{
		iNonFHeight = (int32)(fNonFixedBase * iNonFHeight);
	}
	
	//if ( iFixedHeight != m_GridCtrl.GetDefCellHeight() )
	{
		// ������ı�Ĭ�ϵ������е�
		CGridDefaultCell *pCell1 = DYNAMIC_DOWNCAST(CGridDefaultCell, m_GridCtrl.GetDefaultCell(TRUE, TRUE));
		CGridDefaultCell *pCell2 = DYNAMIC_DOWNCAST(CGridDefaultCell, m_GridCtrl.GetDefaultCell(TRUE, FALSE));
		if ( NULL != pCell1 )
		{
			pCell1->SetHeight(iFixedHeight);
		}
		if ( NULL != pCell2 )
		{
			pCell2->SetHeight(iFixedHeight);
		}

		for ( int32 i=0; i < m_GridCtrl.GetFixedRowCount() ; i++ )
		{
			m_GridCtrl.SetRowHeight(i, iFixedHeight);
		}
	}

	if ( iNonFHeight != m_GridCtrl.GetDefCellHeight() )
	{
		// ������ı�Ĭ�ϵ������е�
		CGridDefaultCell *pCell1 = DYNAMIC_DOWNCAST(CGridDefaultCell, m_GridCtrl.GetDefaultCell(FALSE, TRUE));
		CGridDefaultCell *pCell2 = DYNAMIC_DOWNCAST(CGridDefaultCell, m_GridCtrl.GetDefaultCell(FALSE, FALSE));
		if ( NULL != pCell1 )
		{
			pCell1->SetHeight(iNonFHeight);
		}
		if ( NULL != pCell2 )
		{
			pCell2->SetHeight(iNonFHeight);
		}
		
		for ( int32 i=m_GridCtrl.GetFixedRowCount(); i < m_GridCtrl.GetRowCount() ; i++ )
		{
			m_GridCtrl.SetRowHeight(i, iNonFHeight);
		}
	}
}

void CIoViewPhaseSort::SetColWidthAccordingFont()
{
	// ʹ���б��Լ�����Ը������ TODO
	CClientDC dc(this);
	CFont *pOldFont = dc.SelectObject(GetIoViewFontObject(ESFNormal));
	CSize sizeText = dc.GetTextExtent(_T("HW�Ų�"));
	dc.SelectObject(pOldFont);
	const int32 iBaseCharWidth = (sizeText.cx)/4;
	for ( int32 i=0; i < m_aColumnHeaders.GetSize() ; i++ )
	{
		float fRatio = m_aColumnHeaders[i].m_fPreferWidthByChar;
		int32 iWidth = (int32)(fRatio*iBaseCharWidth);
		if ( m_GridCtrl.GetColumnWidth(i) > 0 )
		{
			m_GridCtrl.SetColumnWidth(i, iWidth);
		}
	}
}

void CIoViewPhaseSort::UpdateTableHeader()
{
	m_GridCtrl.SetFixedColumnCount(3);	// ����� code,���ƹ̶�
	m_GridCtrl.SetFixedRowCount(1);
	m_GridCtrl.SetColumnCount(m_aColumnHeaders.GetSize());	// �����, ����...

	m_GridCtrl.SetSortColumn(-1);

	SetColWidthAccordingFont();
	SetRowHeightAccordingFont();
	m_GridCtrl.Refresh();
}

void CIoViewPhaseSort::UpdateTableAllContent()
{
	CCellID    cellId = m_GridCtrl.GetTopleftNonFixedCell();
	CCellRange cellRange = m_GridCtrl.GetSelectedCellRange();

	m_GridCtrl.DeleteNonFixedRows();	
	//m_GridCtrl.ResetVirtualOrder();
	
	m_GridCtrl.SetRowCount(m_GridCtrl.GetFixedRowCount() + m_ParamForDisplay.m_aCalcResults.GetSize());
	//SetColWidthAccordingFont();
	//SetRowHeightAccordingFont();

	if ( !m_GridCtrl.IsValid(cellId) )
	{
		cellId.row = m_GridCtrl.GetFixedRowCount();
		cellId.col = m_GridCtrl.GetFixedColumnCount();
	}
	if ( m_GridCtrl.IsValid(cellId) )
	{
		m_GridCtrl.EnsureTopLeftCell(cellId);
	}
	if ( m_GridCtrl.IsValid(cellRange) )
	{
		m_GridCtrl.SetSelectedRange(cellRange);
	}
	m_GridCtrl.Refresh();
}

void CIoViewPhaseSort::OnDoBlockReportInitialize()
{
	m_bBlockReportInitialized = true;		// ��ʼ������ˣ��Ժ������ʽ�Ĳ�����

	// ��װ��һ�ΰ������
	if ( m_PhaseBlockParamXml.m_TimeStart <= m_PhaseBlockParamXml.m_TimeEnd
		&& m_PhaseBlockParamXml != m_PhaseOpenBlockParam
		&& m_PhaseOpenBlockParam.m_TimeEnd < m_PhaseOpenBlockParam.m_TimeStart )
	{
		// ��ԭʼ������Ч������£���ʼ��xml�еİ��
		// ���ڽ׶�����ͨ��mainframe�еĲ˵� �ȵ���ҳ���ļ� ҳ���ļ�xml�� Ȼ����mainframe����open�򿪲�������
		// Ӧ���ǲ�����������
		ASSERT( 0 );
		//OpenBlock(m_PhaseBlockParamXml, false);
		//ChangeTabToCurrentBlock();
	}
}

void CIoViewPhaseSort::InitializeTabs()
{
	int i = 0;
	bool32 bInitOld = m_bBlockReportInitialized;
	m_bBlockReportInitialized = false;				// ��ֹtab��Ӧ

	m_GuiTabWnd.DeleteAll();
	for ( i=0; i < s_KiTabInfoCount ; i++ )
	{
		const T_SimpleTabInfo &tab = s_KaTabInfos[i];
		CString StrName = tab.m_StrShowName;
		m_GuiTabWnd.Addtab(StrName, StrName, StrName);
	}
	
	m_bBlockReportInitialized = bInitOld;
}

void CIoViewPhaseSort::SetTabByBlockId( int32 iBlockId )
{
	int i = 0;
	for ( i=0; i < s_KiTabInfoCount ; i++ )
	{
		if ( iBlockId == s_KaTabInfos[i].m_iIdDefault && s_KaTabInfos[i].m_eTabType == ETITEntity )		// ѡ�и�tab
		{
			if ( m_GuiTabWnd.GetCount() > i )
			{
				m_GuiTabWnd.SetCurtab(i);		// ��ǰѡ���Ƿ���??
				break;
			}
		}
	}
}

void CIoViewPhaseSort::ChangeTabToCurrentBlock()
{
	int i = 0;
	for ( i=0; i < s_KiTabInfoCount ; i++ )
	{
		if ( m_PhaseOpenBlockParam.m_iBlockId == s_KaTabInfos[i].m_iIdDefault && s_KaTabInfos[i].m_eTabType == ETITEntity )		// ѡ�и�tab
		{
			if ( m_GuiTabWnd.GetCount() > i )
			{
				bool32 bInit = m_bBlockReportInitialized;
				m_bBlockReportInitialized = false;	// ��ֹ��Ӧ
				m_GuiTabWnd.SetCurtab(i);		// ��ǰѡ���Ƿ���??
				m_bBlockReportInitialized = bInit;
				break;
			}
		}
	}
}

void CIoViewPhaseSort::RecalcLayout()
{
	m_RectTitle.SetRectEmpty();

	CRect rcClient;
	GetClientRect(rcClient);

	m_RectTitle = rcClient;

	CClientDC dc(this);
	CFont *pFontOld = dc.SelectObject(GetIoViewFontObject(ESFNormal));
	CSize sizeTitle = dc.GetTextExtent(_T("����߶Ȳ���"));
	dc.SelectObject(pFontOld);

	m_RectTitle.bottom = m_RectTitle.top + sizeTitle.cy + 10;

	rcClient.top = m_RectTitle.bottom;

	CSize sizeTab = m_GuiTabWnd.GetWindowWishSize();
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
	m_GuiTabWnd.MoveWindow(rcClient.left, rcClient.bottom - sizeTab.cy, sizeTab.cx, sizeTab.cy);
	CRect rcHorz(rcClient.left + sizeTab.cx, rcClient.bottom - sizeTab.cy, rcClient.right, rcClient.bottom);
	m_XSBHorz.SetSBRect(rcHorz, TRUE);

	rcClient.bottom -= sizeTab.cy;

	m_GridCtrl.MoveWindow(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height());	// ������ֱ����������
}

void CIoViewPhaseSort::OnConfigInitialize( E_InitializeNotifyType eInitializeType )
{
	if ( Initialized == eInitializeType )
	{
		// �����Ϣ�ؽ�
		InitializeTabs();
	}
}

void CIoViewPhaseSort::OnBlockConfigChange( int32 iBlockId, E_BlockNotifyType eNotifyType )
{
	// ���������
	Invalidate(TRUE);	// ˢ������ʾ
}

void CIoViewPhaseSort::OnGridGetDispInfo( NMHDR *pNotifyStruct, LRESULT *pResult )
{
	if ( NULL != pNotifyStruct )
	{
		GV_DISPINFO	*pDisp = (GV_DISPINFO *)pNotifyStruct;
		// ����v mode
		
		GV_ITEM &item = pDisp->item;
		item.lParam = NULL;

		CGridCellNormalSys *pCell = DYNAMIC_DOWNCAST(CGridCellNormalSys, m_GridCtrl.GetDefaultVirtualCell());
		ASSERT( pCell != NULL );
		if ( NULL != pCell )
		{
			pCell->SetShowSymbol(0x7);	// ȫ������ʾ
		}

		COLORREF clrText = GetIoViewColor(ESCKeep);
		COLORREF clrRise = GetIoViewColor(ESCRise);
		COLORREF clrFall = GetIoViewColor(ESCFall);
		item.crFgClr = clrText;	// ����Ĭ����ɫ
		if ( item.col >=0 && item.col < m_aColumnHeaders.GetSize() )
		{
			const T_ColumnHeader &header = m_aColumnHeaders[item.col];
			if ( item.row == 0 )
			{
				// ��ͷ
				item.strText = header.m_StrHeader;
			}
			else if ( item.row <= m_ParamForDisplay.m_aCalcResults.GetSize() && item.row > 0 )
			{
				int32 iIndex = item.row -1;
				const PhaseSortCalcResultArray &aResults = m_ParamForDisplay.m_aCalcResults;
				const T_PhaseSortCalcReslt &Res = aResults[iIndex];
				CMerch *pMerch = m_ParamForDisplay.m_aMerchsToCalc[iIndex];
				int32 iSaveDec = pMerch->m_MerchInfo.m_iSaveDec;
				if ( item.col == 0 )
				{
					item.lParam = (LPARAM)pMerch;	// ���ø���Ʒ��Ϊ0�е�PARAM
				}
				switch (header.m_eHeaderType)
				{
				case EPHT_No:		// ���
					// ��ǰ������
					{
						if ( m_GridCtrl.GetSortColumn() >= 0 )
						{
							int i = 0;
							for ( CGridCtrl::intlist::const_iterator it = m_GridCtrl.m_arRowOrder.begin(); it != m_GridCtrl.m_arRowOrder.end() ; it++, i++ )
							{
								if ( *it == item.row )
								{
									item.strText.Format(_T("%d"), i);
									break;
								}
							}
						}
						else
						{
							item.strText.Format(_T("%d"), item.row);
						}
					}
					break;
				case EPHT_Code:	// ����
					item.strText = pMerch->m_MerchInfo.m_StrMerchCode;
					break;
				case EPHT_Name:
					item.strText = pMerch->m_MerchInfo.m_StrMerchCnName;
					{
						// �Ƿ�����ѡ�� & ��ǰ����ǲ�����ѡ���
						const T_Block *pUserBlock = CUserBlockManager::Instance()->GetBlock(pMerch);
						CBlockLikeMarket *pBlockCur = GetCurrentBlock();
						if ( NULL != pUserBlock && pUserBlock->m_clrBlock != COLORNOTCOUSTOM
							&& ( NULL == pBlockCur || !pBlockCur->IsUserBlock() ))
						{
							/*item.crFgClr = pUserBlock->m_clrBlock;*/
						}

						// �Ƿ��б��
						GV_DRAWMARK_ITEM markItem;
						if ( InitDrawMarkItem(pMerch, markItem) )
						{
							item.markItem = markItem;
						}
					}
					break;
				case EPHT_PreClose:
					item.strText = Float2String(Res.fPreClose, iSaveDec, false, false);
					break;
				case EPHT_Close:
					item.strText = Float2String(Res.fClose, iSaveDec, false, false);
					break;
				case EPHT_High:
					item.strText = Float2String(Res.fHigh, iSaveDec, false, false);
					break;
				case EPHT_Low:
					item.strText = Float2String(Res.fLow, iSaveDec, false, false);
					break;
				case EPHT_Vol:
					item.strText = Float2String(Res.fVol*100, 0, true, false);	// ��
					item.crFgClr = GetIoViewColor(ESCVolume2);
					break;
				case EPHT_Amount:
					item.strText = Float2String(Res.fAmount, 0, true, false);
					item.crFgClr = GetIoViewColor(ESCAmount);
					break;
				case EPHT_MarketRate:
					item.strText = Float2String(Res.fMarketRate*100, 2, false, false);
					break;
				case EPHT_TradeRate:
					item.strText = Float2String(Res.fTradeRate*100, 2, false, false);
					break;
				case EPHT_VolChangeRate:
					item.strText = Float2String(Res.f5DayVolChangeRate*100, 2, false, false);
					break;

				case EPHT_RiseFall: // �ǵ�����
					{
						// ����
						float fRiseFall = Res.fClose - Res.fPreClose;
						float fRate = 0.0f;
						if ( Res.fPreClose != 0.0f )
						{
							fRate = fRiseFall/Res.fPreClose*100;
						}
						item.strText = Float2String(fRiseFall, iSaveDec, false, false) + _T("\r\n");
						item.strText += Float2String(fRate, iSaveDec, false, false, true);

						item.nFormat = DT_RIGHT;
						if ( fRate > 0.000001f )
						{
							item.crFgClr = clrRise;
						}
						else if ( fRate < -0.000001f )
						{
							item.crFgClr = clrFall;
						}
					}
					break;
				case EPHT_ShakeRate:	// �𵴷���
					{
						// ����
						float fShake = Res.fHigh - Res.fLow;
						float fRate = 0.0f;
						if ( Res.fLow != 0.0f )
						{
							fRate = fShake/Res.fLow*100;
						}
						item.strText = Float2String(fShake, iSaveDec, false, false) + _T("\r\n");
						item.strText += Float2String(fRate, iSaveDec, false, false, true);

						item.nFormat = DT_RIGHT;
					}
					break;
				default:
					ASSERT( 0 );
				}
			}
		}
	}
	
	if ( NULL != *pResult )
	{
		*pResult = 1;
	}
}


bool CIoViewPhaseSort::CompareRow( int iRow1, int iRow2 )
{
	ASSERT( m_spThis != NULL );
	const int iIndex1 = iRow1-1;
	const int iIndex2 = iRow2-1;

	const MerchArray &aMerchs = m_spThis->m_ParamForDisplay.m_aMerchsToCalc;
	const PhaseSortCalcResultArray &aResults = m_spThis->m_ParamForDisplay.m_aCalcResults;
	ASSERT( aMerchs.GetSize() == aResults.GetSize() );
	ASSERT( iIndex2 >= 0 && iIndex2 < aMerchs.GetSize() );
	ASSERT( iIndex1 >= 0 && iIndex1 < aMerchs.GetSize() );

	CMerch *pMerch1 = aMerchs[iIndex1];
	CMerch *pMerch2 = aMerchs[iIndex2];

	const T_PhaseSortCalcReslt &Res1 = aResults[iIndex1];
	const T_PhaseSortCalcReslt &Res2 = aResults[iIndex2];

	ASSERT( pMerch1 != NULL && pMerch2 != NULL );
	if ( NULL == pMerch1 || pMerch2 == NULL )
	{
		return false;	// ����
	}

	BOOL bAscend = m_spThis->m_GridCtrl.GetSortAscending();

	const int iCol = m_spThis->m_GridCtrl.m_CurCol;
	ASSERT( iCol >= 0 && iCol < m_spThis->m_aColumnHeaders.GetSize() );
	E_PhaseHeaderType eHeader = m_spThis->m_aColumnHeaders[iCol].m_eHeaderType;
	bool bRet = false;
	float fValue1 = 0.0f;
	float fValue2 = fValue1;
	bool32 bNeedZeroEndCmp = true; // �Ƿ�Ҫ����0Ҫ�������ıȽ�
	switch (eHeader)
	{
	case EPHT_No:
		ASSERT( 0 );
		break;
	case EPHT_Code:	// Ӣ����ĸ
		bRet = CompareString(LOCALE_SYSTEM_DEFAULT, 0, pMerch1->m_MerchInfo.m_StrMerchCode, -1,
			pMerch2->m_MerchInfo.m_StrMerchCode, -1) == CSTR_LESS_THAN;
		bNeedZeroEndCmp = false;
		break;
	case EPHT_Name:		// ����
		bRet = CompareString(LOCALE_SYSTEM_DEFAULT, 0, pMerch1->m_MerchInfo.m_StrMerchCnName, -1,
			pMerch2->m_MerchInfo.m_StrMerchCnName, -1)  == CSTR_LESS_THAN;
		bNeedZeroEndCmp = false;
		break;
	case EPHT_PreClose:
		//bRet = Res1.fPreClose > Res2.fPreClose;
		fValue1 = Res1.fPreClose;
		fValue2 = Res2.fPreClose;
		break;
	case EPHT_Close:
		//bRet = Res1.fClose > Res2.fClose;
		fValue1 = Res1.fClose;
		fValue2 = Res2.fClose;
		break;
	case EPHT_High:
		//bRet = Res1.fHigh > Res2.fHigh;
		fValue1 = Res1.fHigh;
		fValue2 = Res2.fHigh;
		break;
	case EPHT_Low:
		//bRet = Res1.fLow > Res2.fLow;
		fValue1 = Res1.fLow;
		fValue2 = Res2.fLow;
		break;
	case EPHT_Vol:
		//bRet = Res1.fVol > Res2.fVol;
		fValue1 = Res1.fVol;
		fValue2 = Res2.fVol;
		break;
	case EPHT_Amount:
		//bRet = Res1.fAmount > Res2.fAmount;
		fValue1 = Res1.fAmount;
		fValue2 = Res2.fAmount;
		break;
	case EPHT_MarketRate:
		//bRet = Res1.fMarketRate > Res2.fMarketRate;
		fValue1 = Res1.fMarketRate;
		fValue2 = Res2.fMarketRate;
		break;
	case EPHT_TradeRate:
		//bRet = Res1.fTradeRate > Res2.fTradeRate;
		fValue1 = Res1.fTradeRate;
		fValue2 = Res2.fTradeRate;
		break;
	case EPHT_VolChangeRate:
		{
			// 0.0f��Ҫ����������
			fValue1 = Res1.f5DayVolChangeRate;
			fValue2 = Res2.f5DayVolChangeRate;
		}
		break;
	case EPHT_RiseFall: // �ǵ�����
		{
			// ����
			// �����ǵ���������������0��Ҫ������
			float fRiseFall1 = Res1.fClose - Res1.fPreClose;
			float fRiseFall2 = Res2.fClose - Res2.fPreClose;			
			if ( Res1.fPreClose != 0.0f )
			{
				fValue1 = fRiseFall1/Res1.fPreClose;
			}
			if ( Res2.fPreClose != 0.0f )
			{
				fValue2 = fRiseFall2/Res2.fPreClose;
			}
		}
		break;
	case EPHT_ShakeRate:	// �𵴷���
		{
			// ����
			float fShake1 = Res1.fHigh - Res1.fLow;
			float fShake2 = Res2.fHigh - Res2.fLow;
			if ( Res1.fLow != 0.0f )
			{
				fValue1 = fShake1/Res1.fLow*100;
			}
			if ( Res2.fLow != 0.0f )
			{
				fValue2 = fShake2/Res2.fLow*100;
			}
		}
		break;
	default:
		ASSERT( 0 );
	}

	if ( bNeedZeroEndCmp )
	{
		if ( fValue1 != 0.0f && fValue2 != 0.0f )
		{
			bRet = fValue1 < fValue2;
		}
		else if ( fValue1 == 0.0f )
		{
			bRet = !bAscend;	// 0.0fС����
		}
		else
		{
			bRet = bAscend != FALSE;	// ���ܾ���- -
		}
	}

	return bRet;
}

void CIoViewPhaseSort::OnGridCacheHint( NMHDR *pNotifyStruct, LRESULT *pResult )
{
	// �����ǰ�����ֶ�Ϊ��������߱��������ֶΣ�����Ҫ��ᣬ���Ϊ�����������ֶΣ����鱾�ش��ڵ����������Ƿ��㹻
	// ��������Ҫ�ط��������󣬵ȴ����ݻع飬�����ˢ����ʾ
	//GV_CACHEHINT *pCacheHint = (GV_CACHEHINT *)pNotifyStruct;
	//if ( NULL != pCacheHint && pCacheHint->range.IsValid() )	// ���û�з������������󣬲���Ҫ����
	//{
	//	

	//}

	if ( NULL != pResult )
	{
		*pResult = 1;
	}
}

void CIoViewPhaseSort::OnContextMenu( CWnd* pWnd, CPoint pos )
{
	CRect rcClient;
	GetWindowRect(rcClient);
	if ( !rcClient.PtInRect(pos) )
	{
		pos = rcClient.TopLeft();
		DoTrackMenu(pos);
		return;
	}
	
	if ( pWnd->GetSafeHwnd() == m_hWnd )
	{
		DoTrackMenu(pos);
		return;
	}

	CIoViewBase::OnContextMenu(pWnd, pos);
	return;	
}

void CIoViewPhaseSort::DoTrackMenu(CPoint pos)
{
	CNewMenu menu;
	menu.CreatePopupMenu();
	

	int32 iCmd = 1;
	int32 iSortTypeBase = -1;
	if ( m_ParamForDisplay.m_aCalcResults.GetSize() > 0 )
	{
		// �����ݿ�������

		menu.AppendMenu(MF_STRING, iCmd++, _T("����Ʒ"));
		menu.SetDefaultItem(1, FALSE);
		menu.AppendMenu(MF_SEPARATOR);

		// ������
		BOOL bShowGrid = m_GridCtrl.GetGridLines() != GVL_NONE;
		menu.AppendMenu(MF_STRING, iCmd++, bShowGrid ? _T("����������"): _T("��ʾ������"));
		menu.AppendMenu(MF_SEPARATOR);
		
		// ��������Ȳ���
		menu.AppendMenu(MF_STRING, iCmd++, _T("�������в���"));
		menu.AppendMenu(MF_STRING, iCmd++, _T("���¼���   F8"));
		menu.AppendMenu(MF_SEPARATOR);
		
		// ��������������
		iCmd++;
		iSortTypeBase = iCmd;
		menu.AppendMenu(MF_STRING, iCmd+EPST_RiseFallPecent, _T("�ǵ�����"));
		menu.AppendMenu(MF_STRING, iCmd+EPST_TradeRate, _T("����������"));
		menu.AppendMenu(MF_STRING, iCmd+EPST_VolChangeRate, _T("�������"));
		menu.AppendMenu(MF_STRING, iCmd+EPST_ShakeRate, _T("�𵴷���"));
		menu.CheckMenuItem(iCmd+m_PhaseOpenBlockParam.m_ePhaseSortType, MF_CHECKED |MF_BYCOMMAND);
		iCmd += EPST_Count;
		menu.AppendMenu(MF_SEPARATOR);
	}
	else
	{
		// ��������Ȳ���
		iCmd++;	// ��������Ʒ
		menu.AppendMenu(MF_STRING, iCmd++, _T("�������в���"));
		menu.AppendMenu(MF_SEPARATOR);
	}
	
	// ��ͨ��ť
	AppendStdMenu(&menu);

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
			// ����Ʒ
			CMerch *pMerchXml = GetMerchXml();
			CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
			if ( NULL != pMainFrame )
			{
				pMainFrame->OnShowMerchInChart(pMerchXml, this);
			}
		}
		else if ( 2 == iRet )
		{
			BOOL bShowGrid = m_GridCtrl.GetGridLines() != GVL_NONE;
			m_GridCtrl.ShowGridLine(bShowGrid ? GVL_NONE : GVL_BOTH);
			m_GridCtrl.Refresh();
		}
		else if (3 == iRet)
		{
			// ���ò���
			T_PhaseOpenBlockParam BlockParam;
			if ( CDlgPhaseSort::PhaseSortSetting(BlockParam, &m_PhaseOpenBlockParam)
				&& m_PhaseOpenBlockParam != BlockParam )
			{
				if ( !m_PhaseOpenBlockParam.IsCalcParamSame(BlockParam) )
				{
					OpenBlock(BlockParam, true);	// ��Ҫ�ؼ����
				}
				else if ( m_PhaseOpenBlockParam.m_ePhaseSortType != BlockParam.m_ePhaseSortType )
				{
					SetSortColumnBySortType(BlockParam.m_ePhaseSortType);	// ������������
				}
			}
		}
		else if ( 4 == iRet )
		{
			T_PhaseOpenBlockParam BlockParam = m_PhaseOpenBlockParam;
			OpenBlock(BlockParam, true);	// ���´�
		}
		else if ( iSortTypeBase > 0 )
		{
			// �ı���������
			E_PhaseSortType eSort = (E_PhaseSortType)(iRet-iSortTypeBase);
			if ( eSort < EPST_Count )
			{
				SetSortColumnBySortType(eSort);
			}
		}
	}
	else
	{
		// ��ͨ����
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, MAKEWPARAM(iRet, 0), 0);
	}
}

CMerch * CIoViewPhaseSort::GetMerchXml()
{
	CMerch *pMerch = NULL;
	CCellRange rangeSel = m_GridCtrl.GetSelectedCellRange();

	if ( m_GridCtrl.IsValid(rangeSel) )
	{
		pMerch = (CMerch *)m_GridCtrl.GetItemData(rangeSel.GetMinRow(), 0);
		if ( NULL != pMerch )
		{
			return pMerch;
		}
	}
	
	return m_pMerchXml;
}

void CIoViewPhaseSort::InitializeTitleString()
{
	m_aTitleString.RemoveAll();

	// �׶����з���--�ǵ����� �г�:��ѡ�� ����:2010-03-14(��),2011-01-02(��) ���Ҽ����в���
	T_TitleString title;
	CString StrTitle;
	StrTitle.Format(_T("�׶����з��� -- %s "), GetPhaseSortTypeName(m_PhaseOpenBlockParam.m_ePhaseSortType).GetBuffer());
	title.m_StrTitle = StrTitle;
	title.m_clr = RGB(250,50,50);
	m_aTitleString.Add(title);

	COLORREF clrVol = GetIoViewColor(ESCVolume);
	COLORREF clrText = GetIoViewColor(ESCText);
	title.m_StrTitle = _T("�г�: ");
	title.m_clr = clrText;
	m_aTitleString.Add(title);
	CBlockLikeMarket *pBlock = GetCurrentBlock();
	if ( NULL == pBlock )
	{
		title.m_StrTitle = _T("       ");
	}
	else
	{
		title.m_StrTitle = pBlock->m_blockInfo.m_StrBlockName;
	}
	title.m_StrTitle += _T(" ");
	title.m_clr = clrVol;
	m_aTitleString.Add(title);

	CTime TimeStart(m_PhaseOpenBlockParam.m_TimeStart.GetTime());
	CTime TimeEnd(m_PhaseOpenBlockParam.m_TimeEnd.GetTime());
	title.m_StrTitle = _T("����: ");
	title.m_clr = clrText;
	m_aTitleString.Add(title);
	title.m_StrTitle.Format(_T("%04d-%02d-%02d(%s),%04d-%02d-%02d(%s) "),
		TimeStart.GetYear(), TimeStart.GetMonth(), TimeStart.GetDay(), GetWeekString(TimeStart).GetBuffer(),
		TimeEnd.GetYear(), TimeEnd.GetMonth(), TimeEnd.GetDay(), GetWeekString(TimeEnd).GetBuffer());
	title.m_clr = clrVol;
	m_aTitleString.Add(title);

	title.m_StrTitle = _T("���Ҽ����в���");
	title.m_clr = clrText;
	m_aTitleString.Add(title);
}

void CIoViewPhaseSort::InitializeColumnHeader( E_PhaseSortType eType )
{
	CArray<E_PhaseHeaderType, E_PhaseHeaderType> aHeaders;
	switch (eType)
	{
	case EPST_RiseFallPecent:	// �ǵ�
		{
			aHeaders.Add(EPHT_No);
			aHeaders.Add(EPHT_Code);
			aHeaders.Add(EPHT_Name);
			aHeaders.Add(EPHT_RiseFall);
			aHeaders.Add(EPHT_PreClose);
			aHeaders.Add(EPHT_High);
			aHeaders.Add(EPHT_Low);
			aHeaders.Add(EPHT_Close);
			aHeaders.Add(EPHT_ShakeRate);
			aHeaders.Add(EPHT_Vol);
			aHeaders.Add(EPHT_Amount);
			aHeaders.Add(EPHT_MarketRate);
			aHeaders.Add(EPHT_TradeRate);
			aHeaders.Add(EPHT_VolChangeRate);
		}
		break;
	case EPST_ShakeRate:
		{
			aHeaders.Add(EPHT_No);
			aHeaders.Add(EPHT_Code);
			aHeaders.Add(EPHT_Name);
			aHeaders.Add(EPHT_ShakeRate);
			aHeaders.Add(EPHT_PreClose);
			aHeaders.Add(EPHT_High);
			aHeaders.Add(EPHT_Low);
			aHeaders.Add(EPHT_Close);
			aHeaders.Add(EPHT_RiseFall);
			aHeaders.Add(EPHT_Vol);
			aHeaders.Add(EPHT_Amount);
			aHeaders.Add(EPHT_MarketRate);
			aHeaders.Add(EPHT_TradeRate);
			aHeaders.Add(EPHT_VolChangeRate);
		}
		break;
	case EPST_TradeRate:
		{
			aHeaders.Add(EPHT_No);
			aHeaders.Add(EPHT_Code);
			aHeaders.Add(EPHT_Name);
			aHeaders.Add(EPHT_TradeRate);
			aHeaders.Add(EPHT_PreClose);
			aHeaders.Add(EPHT_High);
			aHeaders.Add(EPHT_Low);
			aHeaders.Add(EPHT_Close);
			aHeaders.Add(EPHT_ShakeRate);
			aHeaders.Add(EPHT_Vol);
			aHeaders.Add(EPHT_Amount);
			aHeaders.Add(EPHT_MarketRate);
			aHeaders.Add(EPHT_RiseFall);
			aHeaders.Add(EPHT_VolChangeRate);
		}
		break;
	case EPST_VolChangeRate:
		{
			aHeaders.Add(EPHT_No);
			aHeaders.Add(EPHT_Code);
			aHeaders.Add(EPHT_Name);
			aHeaders.Add(EPHT_VolChangeRate);
			aHeaders.Add(EPHT_PreClose);
			aHeaders.Add(EPHT_High);
			aHeaders.Add(EPHT_Low);
			aHeaders.Add(EPHT_Close);
			aHeaders.Add(EPHT_ShakeRate);
			aHeaders.Add(EPHT_Vol);
			aHeaders.Add(EPHT_Amount);
			aHeaders.Add(EPHT_MarketRate);
			aHeaders.Add(EPHT_TradeRate);
			aHeaders.Add(EPHT_RiseFall);
		}
		break;
	default:
		ASSERT( 0 );
	}

	m_aColumnHeaders.RemoveAll();
	for ( int32 i=0; i < aHeaders.GetSize() ; i++ )
	{
		m_aColumnHeaders.Add( T_ColumnHeader(aHeaders[i]) );
	}
}

bool32 CIoViewPhaseSort::OpenBlock(const T_PhaseOpenBlockParam &BlockParam, bool32 bReOpen, bool32 bAskUserSetting/* = false*/)
{
	if ( !bReOpen && m_PhaseOpenBlockParam.IsCalcParamSame(BlockParam) )
	{
		return true;	// ����Ҫ�ظ���
	}

	CBlockLikeMarket *pBlock = CBlockConfig::Instance()->FindBlock(BlockParam.m_iBlockId);
	if ( NULL != pBlock
		&& BlockParam.m_TimeStart <= BlockParam.m_TimeEnd
		&& BlockParam.m_ePhaseSortType < EPST_Count )
	{
		ASSERT( m_ParamForCalc.m_eResultFlag != EPSCRF_Calculating );
		CancelPhaseSortCalc(&m_ParamForCalc);		// ��ֹ����

		m_PhaseOpenBlockParam = BlockParam;
		m_PhaseOpenBlockParam.m_iBlockId = pBlock->m_blockInfo.m_iBlockId;

		m_PhaseBlockParamXml = m_PhaseOpenBlockParam;		// ��xml�Ľ�����ã���ֹinitʱ�ٴδ�

		// ��ֵ�������
		m_ParamForCalc.m_TimeStart	= BlockParam.m_TimeStart;
		m_ParamForCalc.m_TimeEnd	= BlockParam.m_TimeEnd;
		m_ParamForCalc.m_bDoPreWeight = BlockParam.m_bDoPreWeight;
		m_ParamForCalc.m_aMerchsToCalc.Copy(pBlock->m_blockInfo.m_aSubMerchs);

		// ��������
		m_ParamForCalc.m_hWndMsg = m_hWnd;
		m_ParamForCalc.m_uMsgBase = UM_CALCMSG;
		m_ParamForCalc.m_pAbsDataManager = AfxGetDocument()->m_pAbsDataManager;

		// ��ʾ���ݳ�ʼ��
		m_ParamForDisplay.CopyResult(m_ParamForCalc);
		m_ParamForDisplay.m_aCalcResults.SetSize(m_ParamForDisplay.m_aMerchsToCalc.GetSize());
		ZeroMemory(m_ParamForDisplay.m_aCalcResults.GetData(), m_ParamForDisplay.m_aCalcResults.GetSize()*sizeof(T_PhaseSortCalcReslt));

		//ChangeTabToCurrentBlock();

		// �л���ʾ
		InitializeTitleString();
		InitializeColumnHeader(m_PhaseOpenBlockParam.m_ePhaseSortType);	// ��ʼ���б�ͷ
		UpdateTableHeader();	// ��ͷ��ʼ��
		SetRowHeightAccordingFont();	// ��ʼ���и�
		UpdateTableAllContent();	// ���������ˢ��
		m_GridCtrl.SetSortColumn(3);	// ��4�б�Ȼ����

		DWORD dwTime = timeGetTime();
		m_mapExpAmount.clear();
		RedrawWindow();	// ��ʱӦ����ʾһ��


		if ( m_ParamForCalc.m_aMerchsToCalc.GetSize() > 0 )	// ����Ʒ�ɼ���
		{
			// �������������ݿ�ʼ
			RequestExpKline();	// ����ָ��K��

			if ( m_PhaseOpenBlockParam.m_bDoPreWeight )
			{
				//RequestWeightData();	// �����Ȩ���� // �������Ȩ���ݣ��о��У�û�о�û��
			}

			int32 iReq = RequestAllPriceData();
			TRACE(_T("�׶����з���Ȩ��,ָ��K��,ʵʱ����: %d ms\r\n"), timeGetTime() - dwTime);
			int32 iRes = -1;
			if ( iReq > 0 )
			{
				m_eStage = ES_WaitPrice;
				iRes = m_DlgWait.ShowTimeWaitDlg(false, -1, true, _T("ˢ����������..."), _T("ˢ����������"));
			}
			else
			{
				m_eStage = ES_WaitCalc;
				//--- wangyongxue 2016/09/29 ʹ��PostMessage�����׳�ʧ�ܣ�����SendMessage
				//				PostMessage(UM_DOCALC, 0, 0);	// û����������ֱ�Ӽ���
				SendMessage(UM_DOCALC, 0, 0);	
				iRes = m_DlgWait.ShowTimeWaitDlg(false, -1, true, _T("���ڼ���..."), _T(""));	// ��ʾ���ڼ���
			}
			// ȡ�����ݹ�ע
			m_aMerchsNeedPrice.RemoveAll();
			m_aSmartAttendMerchs.RemoveAll();
			if ( iRes == IDCANCEL && ES_WaitPrice == m_eStage )
			{
				// ȡ������ˢ�£�һ����Ҫ��ʼ����
				m_eStage = ES_WaitCalc;
				PostMessage(UM_DOCALC, 0, 0);
				iRes = m_DlgWait.ShowTimeWaitDlg(false, -1, true, _T("��ʼ����..."), _T(""));	// ��ʾ��ʼ����
			}

			E_Stage eStateOld = m_eStage;
			m_eStage = ES_Count;	// ״̬���
			if ( iRes == IDCANCEL )
			{
				CancelPhaseSortCalc(&m_ParamForCalc);	// �жϼ���
			}
			ClearPhaseSortCalcParamResource(&m_ParamForCalc);	// ������Դ

			if ( ES_WaitPrice != eStateOld )
			{
				m_TimeLastReqPrice = m_pAbsCenterManager->GetServerTime(); // �����������

				ASSERT( ES_WaitCalc == eStateOld );
				// ������ϣ��鿴������
				if ( m_ParamForCalc.m_eResultFlag == EPSCRF_UserCancel )
				{
					MessageBox(_T("�û�ȡ�����㣬��������û�м��㣡"), _T("��ʾ"));
				}
				else if ( m_ParamForCalc.m_eResultFlag == EPSCRF_Error )
				{
					MessageBox(_T("���������"), _T("��ʾ"));
				}
				
				// ����ʲô��������������
				m_ParamForDisplay.CopyResult(m_ParamForCalc);

				// ��������
				CCellID cell(0, 3);	// �����б�Ȼ��������
				m_GridCtrl.SetSortAscending(TRUE);	// ��x��ʼ
				OnFixedRowClickCB(cell);
				Invalidate();	// ˢ����ʾ
			}
		}

		return true;
	}
	else if ( bAskUserSetting )
	{
		// ���ʱ��Ӧ����û�г�ʼ�������ģ����Ըõ����Ի���ѯ���û���ô���� TODO
		T_PhaseOpenBlockParam TmpParam = BlockParam;
		if ( CDlgPhaseSort::PhaseSortSetting(TmpParam, &BlockParam) )
		{
			return OpenBlock(TmpParam, bReOpen, false);		// ��Ҫ��ѯ����
		}
	}
	return false;
}

int32 CIoViewPhaseSort::RequestAllPriceData(bool32 bForce /*= false*/)
{
	if(NULL == m_pAbsCenterManager)
	{
		return 0;
	}
	// ��������ʱ�䣬����������������, ��һ�α�����
	// �������й�Ʊ����������
	CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();
	CGmtTimeSpan SpanDiscard(0, 0, 30, 0);	// ���ɷ��Ӻ󣬸�����ʧЧ, ��Ҫ��������
	
	m_aSmartAttendMerchs.RemoveAll();
	m_aMerchsNeedPrice.SetSize(0, m_ParamForDisplay.m_aMerchsToCalc.GetSize());
	const MerchArray &aMerchSrc = m_ParamForDisplay.m_aMerchsToCalc;
	const int32 iSize = aMerchSrc.GetSize();
	for ( int32 i=0; i < iSize ; i++ )
	{
		CMerch *pMerch = aMerchSrc[i];
		MerchRequestTimeMap::iterator it = s_mapMerchReqTime.find(pMerch);
		if ( it != s_mapMerchReqTime.end() && TimeNow - it->second < SpanDiscard )
		{
			continue;	// ������ǰ���Ѿ��������
		}
		if ( it != s_mapMerchReqTime.end()
			&& m_mapCircAssert.count(pMerch) > 0 )
		{
			// �������Ҹ���Ʒ����Ч��RealTimeֵ
			if ( !IsNowInTradeTimes(pMerch, true) )
			{
				continue;		// �ǽ���ʱ����ڵĹ��˵���
			}
		}

		m_aMerchsNeedPrice.Add(pMerch);	// ������Ҫ���µĶ���
	}

// 	for ( int32 i=0; i < m_pAbsCenterManager->GetMerchManager().m_BreedListPtr.GetSize() ; i++ )
// 	{
// 		CBreed *pBreed = m_pAbsCenterManager->GetMerchManager().m_BreedListPtr[i];
// 		if ( NULL == pBreed )
// 		{
// 			continue;
// 		}
// 
// 		for ( int32 j=0; j < pBreed->m_MarketListPtr.GetSize() ; j++ )
// 		{
// 			CMarket *pMarket = pBreed->m_MarketListPtr[j];
// 			if ( NULL != pMarket 
// 				&& (pMarket->m_MarketInfo.m_eMarketReportType == ERTStockCn
// 				|| pMarket->m_MarketInfo.m_eMarketReportType == ERTExp)
// 				)
// 			{
// 				// ���й��ڹ�Ʊ&&ָ�������鶼��Ҫ
// 				m_aMerchsNeedPrice.Append(pMarket->m_MerchsPtr);
// 			}
// 		}
// 	}

	int32 iNeedSize= m_aMerchsNeedPrice.GetSize();
	TryRequestMoreNeedPriceMerchs();

	return iNeedSize;
}

int32 CIoViewPhaseSort::TryRequestMoreNeedPriceMerchs()
{
	const int32 iMaxMerchs = 20;	// ��֤������ֻ����ô����Ʒ
	if ( m_aSmartAttendMerchs.GetSize() < iMaxMerchs && m_aMerchsNeedPrice.GetSize() > 0 )
	{
		CArray<CSmartAttendMerch, CSmartAttendMerch &> aTTtmp;
		const int32 iNeed = iMaxMerchs - m_aSmartAttendMerchs.GetSize();
		CSmartAttendMerch att;
		for ( int32 i=0; i < iNeed ; i++ )
		{
			if ( m_aMerchsNeedPrice.GetSize() <= 0 )
			{
				break;
			}
			att.m_iDataServiceTypes = EDSTPrice;
			att.m_pMerch = m_aMerchsNeedPrice[m_aMerchsNeedPrice.GetUpperBound()];
			aTTtmp.Add(att);

			m_aMerchsNeedPrice.RemoveAt( m_aMerchsNeedPrice.GetUpperBound() );	// �Ƴ���β������
		}

		RequestAttendMerchs(aTTtmp, true);	// ��������ǿ�Ʒ�����
		m_aSmartAttendMerchs.Append( aTTtmp );	// �Ѿ��������˾ͼ������
		return aTTtmp.GetSize();
	}
	return 0;
}

void CIoViewPhaseSort::RequestAttendMerchs(bool32 bForce)
{
	RequestAttendMerchs(m_aSmartAttendMerchs, bForce);
}

void CIoViewPhaseSort::RequestAttendMerchs( const CArray<CSmartAttendMerch, CSmartAttendMerch &> &aSmarts, bool32 bForce )
{
	CMmiReqRealtimePrice req;
	CSmartAttendMerch att;
	req.m_aMerchMore.SetSize(0, aSmarts.GetSize());
	
	if ( aSmarts.GetSize() > 0 )
	{
		req.m_iMarketId = aSmarts[0].m_pMerch->m_MerchInfo.m_iMarketId;
		req.m_StrMerchCode = aSmarts[0].m_pMerch->m_MerchInfo.m_StrMerchCode;
	}
	CMerchKey key;
	for ( int32 i=1; i < aSmarts.GetSize() ; i++ )
	{
		CMerch *pMerch = aSmarts[i].m_pMerch;
		key.m_iMarketId = pMerch->m_MerchInfo.m_iMarketId;
		key.m_StrMerchCode = pMerch->m_MerchInfo.m_StrMerchCode;
		req.m_aMerchMore.Add(key);
	}
	
	DoRequestViewData(req, bForce);
}

void CIoViewPhaseSort::RequestExpKline()
{
	if(NULL == m_pAbsCenterManager)
	{
		return;
	}
	// ���󼸸�ָ���ض���K�߹���
	MerchArray aMerchsExp;
	GetSpecialAccordExpMerch(aMerchsExp);
	CGmtTime TimeNow = m_pAbsCenterManager->GetServerTime();
	CGmtTime TimeStart = m_ParamForCalc.m_TimeStart;
	SaveDay(TimeStart);
	for ( int32 i=0; i < aMerchsExp.GetSize() ; i++ )
	{
		CMmiReqMerchKLine reqKline;
		reqKline.m_iMarketId = aMerchsExp[i]->m_MerchInfo.m_iMarketId;
		reqKline.m_StrMerchCode = aMerchsExp[i]->m_MerchInfo.m_StrMerchCode;
		
		reqKline.m_eKLineTypeBase = EKTBDay;	
		reqKline.m_eReqTimeType = ERTYSpecifyTime;
		reqKline.m_TimeStart = TimeStart;
		reqKline.m_TimeEnd = TimeNow;		// ��֤K������
		
		DoRequestViewData(reqKline);		// ��������K�ߣ�K����������ʵʱ�������ݹ��������Բ���Ӧ����
	}
}

void CIoViewPhaseSort::RequestWeightData()
{
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if( NULL == pDoc )
	{
		return;
	}
	CAbsDataManager *pAbsDataManager = pDoc->m_pAbsDataManager;
	if (!pAbsDataManager)
	{
		return;
	}

	CString StrFilePath;
	WIN32_FIND_DATA wfd = {0};
	HANDLE hFind = NULL;

	for ( int32 i=0; i < m_ParamForDisplay.m_aMerchsToCalc.GetSize() ; i++ )
	{
		CMerch *pMerch = m_ParamForDisplay.m_aMerchsToCalc[i];
		if ( CReportScheme::Instance()->IsFuture(pMerch->m_Market.m_MarketInfo.m_eMarketReportType) )
		{
			continue;
		}

		// �����Ȩ����
		// 1: ��������û�г�Ȩ����
		if ( pMerch->m_bHaveReadLocalWeightFile )
		{
			continue;
		}
		else if ( !pMerch->m_bHaveReadLocalWeightFile && 0 == pMerch->m_TimeLastUpdateWeightDatas.GetTime() )
		{
			// ���ж�
			// ȡ�ļ�·��:
			if ( pAbsDataManager->GetPublishFilePath(EPFTWeight, pMerch->m_MerchInfo.m_iMarketId , pMerch->m_MerchInfo.m_StrMerchCode, StrFilePath)
				&& (hFind=::FindFirstFile(StrFilePath, &wfd)) == INVALID_HANDLE_VALUE )
			{
				DEL_HANDLE(hFind);
				continue;	// ����֤������
			}
		}
		
		// 2: �������������		
		CMmiReqPublicFile info;
		info.m_iMarketId		= pMerch->m_MerchInfo.m_iMarketId;
		info.m_StrMerchCode		= pMerch->m_MerchInfo.m_StrMerchCode;
		info.m_ePublicFileType	= EPFTWeight;
		info.m_uiCRC32			= pMerch->m_uiWeightDataCRC;
		
		DoRequestViewData(info, true);	// ��ͼ�����˵�����
	}
}

void CIoViewPhaseSort::OnMsgDoCalc()
{
	// �п����Ǵ�����ˢ�¹����ģ�Ҳ�п�����ֱ�ӹ�����
	// ��������
	// ��ֵ��������
	if ( ES_WaitCalc != m_eStage )
	{
		return;
	}
	const int32 iSize = m_ParamForCalc.m_aMerchsToCalc.GetSize();
	m_ParamForCalc.m_aDataParamToCalc.SetSize(iSize);
	PhaseSortCalcDataParamArray &aData = m_ParamForCalc.m_aDataParamToCalc;
	MerchArray &aMerchs = m_ParamForCalc.m_aMerchsToCalc;
	for ( int32 i=0; i < iSize ; i++ )
	{
		CMerch *pMerch = aMerchs[i];
		aData[i].pMerch = pMerch;
		CalcPhaseRequestData(aData[i]);
	}

	// ��ʼ��������
	if ( !StartPhaseSortCalc(&m_ParamForCalc) )
	{
		if ( m_DlgWait.GetSafeHwnd() != NULL )
		{
			m_DlgWait.CancelDlg(IDCANCEL);
		}
		ASSERT( 0 );	// ʲô���
		return;
	}
	
	m_DlgWait.SetTitle(_T("��ʼ����..."));
	m_DlgWait.SetProgress(0, m_ParamForDisplay.m_aMerchsToCalc.GetSize()+1);
}

LRESULT CIoViewPhaseSort::OnMsgCalcStart( WPARAM w, LPARAM l )
{
	//
	if ( ES_WaitCalc != m_eStage )
	{
		return 0;
	}
	m_DlgWait.SetTitle(_T("���ڼ���..."));
	return 1;
}

LRESULT CIoViewPhaseSort::OnMsgCalcPos( WPARAM w, LPARAM l )
{
	if ( ES_WaitCalc != m_eStage )
	{
		return 0;
	}
	MSG msg = *GetCurrentMessage();
	while (::PeekMessage(&msg, m_hWnd, UM_CALCMSG+EPSCM_Pos, UM_CALCMSG+EPSCM_Pos, TRUE))
	{
		// ��ȡ���һ��
	}
	w = msg.wParam;
	l = msg.lParam;
	CString Str;
	int32 iCurPos = (int32)l;
	const int32 iSize = m_ParamForDisplay.m_aMerchsToCalc.GetSize();
	if ( iCurPos >= 0 && iCurPos < m_ParamForDisplay.m_aMerchsToCalc.GetSize() )
	{
		CString StrMerchName;
		StrMerchName = m_ParamForDisplay.m_aMerchsToCalc[iCurPos]->m_MerchInfo.m_StrMerchCnName;
		m_DlgWait.SetTipText(StrMerchName);
	}
	if ( iSize > 0 )
	{
		int32 iPec = l*100/iSize;
		Str.Format(_T("%d%%"), iPec);
		m_DlgWait.AppendToShowTitle(Str);
		m_DlgWait.SetProgress(iPec, 100);
	}
	return 1;	
}

LRESULT CIoViewPhaseSort::OnMsgCalcEnd( WPARAM w, LPARAM l )
{
	if ( ES_WaitCalc != m_eStage )
	{
		return 0;
	}
	m_DlgWait.CancelDlg(IDOK);		// ������
	return 1;
}

LRESULT CIoViewPhaseSort::OnMsgCalcReqData( WPARAM w, LPARAM l )
{
	// ��������
	if ( ES_WaitCalc != m_eStage )
	{
		return 0;
	}
	T_PhaseSortCalcRequestData *pReq = (T_PhaseSortCalcRequestData *)l;
	T_PhaseSortCalcParam *pParam = (T_PhaseSortCalcParam *)w;
	ASSERT( pParam == &m_ParamForCalc && pReq != NULL && pReq->pMerch != NULL );
	if ( pParam == &m_ParamForCalc && pReq != NULL && pReq->pMerch != NULL )
	{
		CalcPhaseRequestData(*pReq);
	}
	return 1;
}

bool32 CIoViewPhaseSort::CalcPhaseRequestData( INOUT T_PhaseSortCalcRequestData &reqData )
{
	if ( NULL == reqData.pMerch )
	{
		return false;
	}

	CMerch *pMerch = reqData.pMerch;
	reqData.fCircAssert = reqData.fMarketAmount = 0.0f;
	if ( NULL != pMerch->m_pRealtimePrice )
	{
		reqData.m_RealTimePrice = *pMerch->m_pRealtimePrice;		// �洢����ʵʱ����
	}
	// ��ͨ�ɱ�
	float fTradeRate = 0.0f;
	if ( NULL != pMerch->m_pFinanceData && 0.0f != pMerch->m_pFinanceData->m_fCircAsset )	// ���������е�
	{
		reqData.fCircAssert = pMerch->m_pFinanceData->m_fCircAsset;
		m_mapCircAssert[pMerch] = reqData.fCircAssert;		// �洢��
	}
	else if ( NULL != pMerch->m_pRealtimePrice && 0.0f != (fTradeRate=pMerch->m_pRealtimePrice->m_fTradeRate) )
	{
		// *100�� *100�ٷֱ�
		reqData.fCircAssert =  pMerch->m_pRealtimePrice->m_fVolumeTotal/fTradeRate*10000;	// ������ͨ��, server�Ļ����ʾ���%�˵�
		
		m_mapCircAssert[pMerch] = reqData.fCircAssert;		// �洢��
	}
	else
	{
		MerchFloatDataMap::iterator it = m_mapCircAssert.find(pMerch);
		if ( it != m_mapCircAssert.end() )
		{
			reqData.fCircAssert = it->second;		// ʹ�ô洢��
		}
		else
		{
			// �Ƿ��ԴӲ��������ļ��ж�ȡ
			CString StrPath;
			if (!AfxGetDocument()->m_pAbsDataManager)
			{
				return false;
			}
			AfxGetDocument()->m_pAbsDataManager->GetPublishFilePath(EPFTF10, pMerch->m_MerchInfo.m_iMarketId, pMerch->m_MerchInfo.m_StrMerchCode, StrPath);
			
			CStdioFile FileFinance;
			CFinanceData stFinance;
			if ( FileFinance.Open(StrPath, CFile::modeRead | CFile::typeBinary) )
			{
				int32 iSize = FileFinance.Read(&stFinance, sizeof(stFinance));
				if ( iSize == sizeof(stFinance) )
				{
					reqData.fCircAssert = stFinance.m_fCircAsset;
				}			
			}
		}
	}
	
	// �г����???����K����
	CMerch *pExpMerch = GetMerchAccordExpMerchStatic(pMerch);
	if ( NULL != pExpMerch )
	{
		MerchFloatDataMap::iterator it = m_mapExpAmount.find(pExpMerch);
		if ( it != m_mapExpAmount.end() )
		{
			reqData.fMarketAmount = it->second;
		}
		else
		{
			// ����K�߼���
			CGmtTime TimeStart = m_ParamForCalc.m_TimeStart;
			CGmtTime TimeEnd = m_ParamForCalc.m_TimeEnd;
			SaveDay(TimeStart);
			
			int32 iPos;
			CMerchKLineNode *pKlines = NULL;
			if ( pExpMerch->FindMerchKLineNode(EKTBDay, iPos, pKlines) && NULL != pKlines )
			{
				const int32 iPosStart = CMerchKLineNode::QuickFindKLineWithBigOrEqualReferTime(pKlines->m_KLines, TimeStart);
				const int32 iPosEnd = CMerchKLineNode::QuickFindKLineWithSmallOrEqualReferTime(pKlines->m_KLines, TimeEnd);
				if ( iPosStart >= 0
					&& iPosEnd >= iPosStart
					&& iPosEnd < pKlines->m_KLines.GetSize() )
				{
					float fAmount = 0.0f;
					for ( int32 i=iPosStart; i <= iPosEnd ; i++ )
					{
						fAmount += pKlines->m_KLines[i].m_fAmount;
					}
					if ( fAmount > 0.0f )
					{
						m_mapExpAmount[pExpMerch] = fAmount;
						reqData.fMarketAmount = fAmount;
					}
				}
			}
		}
	}

	return reqData.fMarketAmount != 0.0f && reqData.fCircAssert != 0.0f;
}

const T_SimpleTabInfo* CIoViewPhaseSort::GetSimpleTabInfo( int32 iTabIndex )
{
	if ( iTabIndex >= 0 && iTabIndex < s_KiTabInfoCount )
	{
		return s_KaTabInfos + iTabIndex;
	}
	ASSERT( 0 );
	return NULL;
}

void CIoViewPhaseSort::GetAttendMarketDataArray( OUT AttendMarketDataArray &aAttends )
{
	CIoViewBase::GetAttendMarketDataArray(aAttends);	// ������ͼ����û�й�ע��Ʒʱ��ʵ����Ҳ����Ҫ��ע����Ʒ��
}

CBlockLikeMarket * CIoViewPhaseSort::GetCurrentBlock()
{
	return CBlockConfig::Instance()->FindBlock(m_PhaseOpenBlockParam.m_iBlockId);
}

void CIoViewPhaseSort::SetSortColumnBySortType( E_PhaseSortType eType )
{
	E_PhaseHeaderType eHeader = EPHT_Count;
	switch (eType)
	{
	case EPST_RiseFallPecent:		// �ǵ���
		{
			eHeader = EPHT_RiseFall;
		}
		break;
	case EPST_TradeRate:		// ������
		{
			eHeader = EPHT_TradeRate;
		}
		break;
	case EPST_VolChangeRate:		// ����
		{
			eHeader = EPHT_VolChangeRate;
		}
		break;
	case EPST_ShakeRate:		// ��
		{
			eHeader = EPHT_ShakeRate;
		}
		break;
	default:
		ASSERT( 0 );
		return;
	}
	m_PhaseOpenBlockParam.m_ePhaseSortType = eType;
	InitializeTitleString();
	InitializeColumnHeader(m_PhaseOpenBlockParam.m_ePhaseSortType);	// ��ʼ���б�ͷ
	UpdateTableHeader();	// ��ͷ��ʼ��

	for ( int32 i=0; i < m_aColumnHeaders.GetSize() ; i++ )
	{
		if ( m_aColumnHeaders[i].m_eHeaderType == eHeader )
		{
			// ��������
			CCellID cell(0, i);
			m_GridCtrl.SetSortAscending(TRUE);	// ��ʼ������
			OnFixedRowClickCB(cell);
			break;
		}
	}
	Invalidate();
	m_GridCtrl.Refresh();
}

//////////////////////////////////////////////////////////////////////////
// 
CArray<CIoViewPhaseSort::T_ColumnHeader, const CIoViewPhaseSort::T_ColumnHeader &> CIoViewPhaseSort::T_ColumnHeader::s_aInitHeaders;
void CIoViewPhaseSort::T_ColumnHeader::InitStatic()
{
	if ( s_aInitHeaders.GetSize() <= 0 )
	{
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_No,			_T("    "),		false, 3.0f) );
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_Code,		_T("����"), false, 5.0f) );
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_Name,		_T("����"), false, 5.8f) );
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_RiseFall,	_T("�ǵ�����"), false, 7.3f, 1.0f, 1.5f) );
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_TradeRate,	_T("������%"), false, 6.0f) );
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_VolChangeRate, _T("5������%"), false, 6.0f) );
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_ShakeRate,	_T("�𵴷���"), false, 7.3f, 1.0f, 1.5f) );
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_PreClose,	_T("ǰ����"), false, 5.0f) );
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_High,		_T("���"), false, 5.0f) );
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_Low,		_T("���"), false, 5.0f) );
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_Close,		_T("����"), false, 5.0f) );
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_Vol,		_T("�ɽ���"), false, 5.0f) );
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_Amount,		_T("�ܽ��"), false, 6.0f) );
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_MarketRate, _T("�г���"), false, 5.0f) );
		s_aInitHeaders.Add( T_ColumnHeader(EPHT_Count, _T("")) );		// ��Чֵ
	}
}

CIoViewPhaseSort::T_ColumnHeader::T_ColumnHeader( E_PhaseHeaderType eHeaderType, LPCTSTR pszName, bool32 bSort, float fWidthByChar, float fFixedRowHeight, float fNonFixedRowHeight )
{
	m_eHeaderType = eHeaderType;
	m_StrHeader = pszName;
	m_bNeedSort = bSort;
	m_fPreferWidthByChar = fWidthByChar;
	m_fFixedRowHeightRatio = fFixedRowHeight;
	m_fNonFixedRowHeightRatio = fNonFixedRowHeight;
}

CIoViewPhaseSort::T_ColumnHeader::T_ColumnHeader( E_PhaseHeaderType eHeaderType )
{
	if ( !Initialize(eHeaderType) )
	{
		Initialize(EPHT_Count);
	}
}

CIoViewPhaseSort::T_ColumnHeader::T_ColumnHeader()
{
	m_eHeaderType = EPHT_Count;
	m_fPreferWidthByChar = 0.;
	m_fFixedRowHeightRatio = 0.;
	m_fNonFixedRowHeightRatio = 0.;

	m_bNeedSort = false;
}

bool32 CIoViewPhaseSort::T_ColumnHeader::Initialize( E_PhaseHeaderType eHeaderType )
{
	InitStatic();

	for ( int32 i=0; i < s_aInitHeaders.GetSize() ; i++ )
	{
		if ( s_aInitHeaders[i].m_eHeaderType == eHeaderType )
		{
			*this = s_aInitHeaders[i];
			return true;
		}
	}

	return false;
}

bool32 CIoViewPhaseSort::T_ColumnHeader::Initialize( const CString &StrHeader )
{
	InitStatic();
	
	for ( int32 i=0; i < s_aInitHeaders.GetSize() ; i++ )
	{
		if ( s_aInitHeaders[i].m_StrHeader == StrHeader )
		{
			*this = s_aInitHeaders[i];
			return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

bool32 T_PhaseOpenBlockParam::IsCalcParamSame( const T_PhaseOpenBlockParam &pa1 ) const
{
	return m_iBlockId == pa1.m_iBlockId
		&& m_TimeStart == pa1.m_TimeStart
		&& m_TimeEnd == pa1.m_TimeEnd
		&& ((m_bDoPreWeight && pa1.m_bDoPreWeight)|| (!m_bDoPreWeight&&!pa1.m_bDoPreWeight))
		   ;
}

bool32 operator==( const T_PhaseOpenBlockParam &pa1, const T_PhaseOpenBlockParam &pa2 )
{
	return memcmp((void*)(&pa1), (void*)(&pa2), sizeof(T_PhaseOpenBlockParam)) == 0;
}

bool32 operator!=( const T_PhaseOpenBlockParam &pa1, const T_PhaseOpenBlockParam &pa2 )
{
	return !(pa1==pa2);
}

CString GetPhaseSortTypeName( E_PhaseSortType eType )
{
	switch (eType)
	{
	case EPST_RiseFallPecent:	// �ǵ�
		{
			return _T("�ǵ�����");
		}
		//break;
	case EPST_ShakeRate:
		return _T("�𵴷���");
	case EPST_TradeRate:
		return _T("����������");
	case EPST_VolChangeRate:
		return _T("�������");
	default:
		//ASSERT( 0 );
		break;
	}

	return _T("");
}

T_PhaseOpenBlockParam::T_PhaseOpenBlockParam()
: m_TimeEnd(time(NULL))
{
	m_iBlockId = CBlockConfig::GetDefaultMarketlClassBlockPseudoId();
	m_bDoPreWeight = true;	// ��Ȼ��ʱ�������Ȩֵ
	m_ePhaseSortType = EPST_RiseFallPecent;
	m_TimeStart = m_TimeEnd - CGmtTimeSpan(30, 0, 0, 0);
	SaveDay(m_TimeStart);
}
