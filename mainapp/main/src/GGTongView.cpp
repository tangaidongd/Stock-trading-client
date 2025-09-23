// GGTongView.cpp : implementation of the CGGTongView class
//

#include "stdafx.h"



#include "MPIChildFrame.h"
#include "BiSplitterWnd.h"
#include "tinyxml.h"
#include "AdjustViewLayoutDlg.h"
#include "GGTongView.h"
#include "IoViewKLine.h"
#include "IoViewTrend.h"
//#include "IoViewNews.h"
#include "IoViewTextNews.h"

#include "memdc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// �������ص�xml�����������Ϣ
static const char * KStrElementValue				= "GGTongView";
static const char * KStrElementAttriActive			= "bActive";		// ÿ��TabSplit �¶���һ�������GGTongView, ������������ж��
static const char * KStrElementAttriActiveOnly		= "bActiveOnly";	// ϵͳ�˳�ʱ,�����Ǹ������ GGTongView, ֻ��һ��
static const char * KStrElementAttriF7Max			= "bF7Max";
static const char * KStrElementAttriBLockMinMax		= "bMinMax";		// �Ƿ����������Сֵ
static const char * KStrElementAttriLockMinX		= "MinX";			// ��СX��
static const char * KStrElementAttriLockMinY		= "MinY";			// ��СY��
static const char * KStrElementAttriLockMaxX		= "MaxX";			// ���X��
static const char * KStrElementAttriLockMaxY		= "MaxY";			// ���Y��
static const char * KStrElementAttriCanF7			= "CanF7";			// ����F7��ݼ�
/////////////////////////////////////////////////////////////////////////////
// CGGTongView
static const int    KiGtongViewBorderLeft			= 1;
static const int    KiGtongViewBorderTop			= 1;
static const int    KiGtongViewBorderRight			= 1;
static const int    KiGtongViewBorderBottom			= 1;

IMPLEMENT_DYNCREATE(CGGTongView, CView)

BEGIN_MESSAGE_MAP(CGGTongView, CView)
	//{{AFX_MSG_MAP(CGGTongView)
	ON_COMMAND(ID_WINDOW_SPLIT_ADD_RIGHT, OnWindowSplitAddRight)
	ON_COMMAND(ID_WINDOW_SPLIT_ADD_LEFT, OnWindowSplitAddLeft)
	ON_COMMAND(ID_WINDOW_SPLIT_ADD_TOP, OnWindowSplitAddTop)
	ON_COMMAND(ID_WINDOW_SPLIT_ADD_BOTTOM, OnWindowSplitAddBottom)
	ON_COMMAND(ID_WINDOW_SPLIT_ADD_TAB, OnWindowSplitAddTab)
	ON_COMMAND(ID_WINDOW_SPLIT_DEL, OnWindowSplitDel)
	ON_COMMAND(ID_VIEW_LOCKMINMAX, OnLockMinMax)
	ON_UPDATE_COMMAND_UI(ID_LAYOUT_ADJUST, OnUpdateLayoutAdjust)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LOCKMINMAX, OnUpdateLockMinMax)
	ON_COMMAND(ID_LAYOUT_ADJUST,OnLayOutAdjust)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_SETFOCUS()
	ON_WM_PAINT()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(UM_AddSplit,OnAddSplit)
	ON_MESSAGE(UM_ISKINDOFCGGTongView,OnIsKindOfCGGTongView)
	ON_MESSAGE(UM_SetActiveCrossFlag,OnSetActiveCrossFlag)
	ON_MESSAGE(UM_SetBiSplitTrackDelFlag,OnSetBiSplitTrackDelFlag)
	ON_MESSAGE(UM_TOXML,OnToXml)
	ON_WM_NCPAINT()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGGTongView construction/destruction

//
bool32 CGGTongView::m_bDragMerch		= false;
bool32 CGGTongView::m_bDragMerchLBDown	= false;
CGGTongView* CGGTongView::m_pGGTViewDragBegin = NULL;

CGGTongView::CGGTongView()
{
	// TODO: add construction code here
	m_bEmptyGGTongView = false;
	m_bPrepareDrag	   = false;	
	m_bLButtonDown	   = false;
	m_bValidAction	   = false;
	m_bActiveCross	   = false;			
	m_eDragDirection   = EDDNONE;

	m_ptDraging		   = CPoint(0,0);
	m_ptDragPre		   = CPoint(0,0);	

	m_bTracking		   = false;
	m_bFromXML		   = false;
	m_bDelByBiSplitTrack=false;

	m_bActiveFocus     = false;

	m_bLockMinMaxInfo  = false;
	m_bCanF7		   = true;
	ZeroMemory(&m_mmiLockMinMaxInfo, sizeof(m_mmiLockMinMaxInfo));	// ȫ0

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->AddGGTongView(this);

	CMPIChildFrame* pFrame = (CMPIChildFrame*)GetParentFrame();
	if (NULL != pFrame)
	{
		pFrame->m_aTest.Add(this);
	}
	m_bShowGtongVeiwBorder = true;
}

CGGTongView::~CGGTongView()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->DelGGTongView(this);

	CMPIChildFrame* pFrame = (CMPIChildFrame*)GetParentFrame();
	if (NULL != pFrame)
	{
		for (int32 i = 0; i < pFrame->m_aTest.GetSize(); i++ )
		{
			if ( pFrame->m_aTest[i] == this )
			{
				pFrame->m_aTest.RemoveAt(i);
				break;
			}
		}
	}
}

BOOL CGGTongView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CGGTongView drawing

void CGGTongView::OnDraw(CDC* pDC)
{
// 	CGGTongDoc* pDoc = GetDocument();
// 	ASSERT_VALID(pDoc);
// 	// TODO: add draw code for native data here
// 
// 	CWnd * pMainWnd = AfxGetApp()->m_pMainWnd;
// 	CMDIChildWnd* pFrameWnd = (CMDIChildWnd*)((CMainFrame*)pMainWnd)->MDIGetActive();
// 
// 	CRect RectClient;
// 	GetClientRect(&RectClient);
// 
// 	if (this == pFrameWnd->GetActiveView())
// 	{
// 		COLORREF clr =  CFaceScheme::Instance()->GetSysColor(ESCBackground);
// 		pDC->Draw3dRect(RectClient, clr, clr);
// 
// 		clr = CFaceScheme::Instance()->GetActiveColor();
// 		CPen PenActive(PS_SOLID, 1, clr);
// 		CPen* pOldPen = pDC->SelectObject(&PenActive);
// 		pDC->MoveTo(RectClient.TopLeft());
// 		pDC->LineTo(RectClient.right, RectClient.top );
// 		pDC->SelectObject(pOldPen);
// 		PenActive.DeleteObject();
// 	}
// 	else
// 	{
// 		COLORREF clr =  CFaceScheme::Instance()->GetSysColor(ESCBackground);
// 		pDC->Draw3dRect(RectClient, clr, clr);
// 	}
}

/////////////////////////////////////////////////////////////////////////////
// CGGTongView printing

BOOL CGGTongView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CGGTongView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CGGTongView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CGGTongView diagnostics

#ifdef _DEBUG
void CGGTongView::AssertValid() const
{
	CView::AssertValid();
}

void CGGTongView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGGTongDoc* CGGTongView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGGTongDoc)));
	return (CGGTongDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGGTongView message handlers
void CGGTongView::OnWindowSplitAddLeft() 
{
	// TODO: Add your command handler code here
	AddSplit(CMPIChildFrame::SD_LEFT);
}

void CGGTongView::OnWindowSplitAddRight() 
{
	// TODO: Add your command handler code here
	AddSplit(CMPIChildFrame::SD_RIGHT);
}

void CGGTongView::OnWindowSplitAddTop() 
{
	// TODO: Add your command handler code here
	AddSplit(CMPIChildFrame::SD_TOP);	
}

void CGGTongView::OnWindowSplitAddBottom() 
{
	// TODO: Add your command handler code here
	AddSplit(CMPIChildFrame::SD_BOTTOM);
}

void CGGTongView::OnWindowSplitAddTab() 
{
	// TODO: Add your command handler code here
	AddSplit(CMPIChildFrame::SD_TAB);
}

void CGGTongView::AddSplit(CMPIChildFrame::E_SplitDirection eSD,double dRatio)
{
	// ��ͼ��С̫С��ʱ��,�������з�
	CRect rect;
	GetClientRect(&rect);
	
	if (eSD == CMPIChildFrame::SD_LEFT || eSD == CMPIChildFrame::SD_RIGHT )
	{
		if (rect.Width() <= VALIDAREASIZE*2)
		{
			MessageBox(L"��ͼ�ߴ��С,�����з�!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
			return;
		}
	}
	else if ( eSD == CMPIChildFrame::SD_TOP || eSD == CMPIChildFrame::SD_BOTTOM )
	{
		if (rect.Height() <= VALIDAREASIZE*2)
		{
			MessageBox(L"��ͼ�ߴ��С,�����з�!", AfxGetApp()->m_pszAppName, MB_ICONWARNING);
			return;
		}		
	}
	
	CMPIChildFrame *pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL == pChildFrame )
	{
		return;
	}

	pChildFrame->AddSplit(this, RUNTIME_CLASS(CGGTongView), eSD, pChildFrame->m_pContext, dRatio);	
}

void CGGTongView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	m_bActiveFocus = bActivate;

	//
	if (bActivate)
	{		
		m_IoViewManager.OnIoViewManagerActive();
		
		// �����TabSplittWnd Ҫʵʱ������m_ActiveView ��Ϣ;Ŀǰ�����л�ҳ���ʱ��ű���һ��
		CWnd * pWindow = GetParent();
		while (pWindow)
		{
			if (pWindow->IsKindOf(RUNTIME_CLASS(CMPIChildFrame)))
			{
				break;
			}
			if (pWindow->IsKindOf(RUNTIME_CLASS(CTabSplitWnd)))
			{
				CTabSplitWnd * pTab = (CTabSplitWnd *)pWindow;
				//				if (!pTab->m_bfromxml)
				//				{
				int32 iCurTab = pTab->m_wndTabCtrl.GetCurtab();
				if ( pTab->m_wndTabCtrl.GetCount() == pTab->m_ActiveViews.GetSize() )
				{
					pTab->m_ActiveViews.SetAt(iCurTab,this);
				}				
				break;
				//				}
			}
			pWindow = pWindow->GetParent();			
		}

		CIoViewBase * pIoView = m_IoViewManager.GetActiveIoView();
		if (NULL != pIoView)
		{
			pIoView->SetChildFrameTitle();
		}	
		
		if ( GetEmptyFlag() )
		{
			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			pMainFrame->SetEmptyGGTongView(this);
		}
	}
	else
	{
		m_IoViewManager.OnIoViewManagerDeactive();
	}

	CClientDC dc(this);
	DrawGGTongView(&dc);
	//
	if (NULL != pDeactiveView)
	{
		CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
		CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
		if ( bActivate && pWnd->m_pDlgAdjustViewLayout )
		{
			pWnd->m_pDlgAdjustViewLayout->SetView(this);	
		}
	}

	//
	
}

void CGGTongView::OnActivateFrame(UINT nState, CFrameWnd* pFrameWnd)
{
	// �����ڵ�ChildFrame �õ�����ʧȥ�����ʱ�����˺���:

	if ( WA_INACTIVE == nState )
	{
		ClearAllDragFlags();
	}
}

void CGGTongView::OnSetFocus(CWnd* pOldWnd) 
{
	//	CView::OnSetFocus(pOldWnd);

	// TODO: Add your message handler code here
	m_IoViewManager.OnIoViewManagerActive();
	// m_IoViewManager.SetFocus();
}

void CGGTongView::PostNcDestroy() 
{
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	if ( pWnd->m_pDlgAdjustViewLayout )
	{
		pWnd->m_pDlgAdjustViewLayout->SetView(NULL);
	}	

	CView::PostNcDestroy(); 
}

void CGGTongView::OnLayOutAdjust()
{
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	ASSERT(NULL != pWnd );
	if ( pWnd->m_pDlgAdjustViewLayout ) return;

	pWnd->m_pDlgAdjustViewLayout = new CAdjustViewLayoutDlg;
	pWnd->m_pDlgAdjustViewLayout->Create(IDD_LAYOUTADJUST,AfxGetMainWnd());
	pWnd->m_pDlgAdjustViewLayout->SetView(this);
	pWnd->m_pDlgAdjustViewLayout->ShowWindow(SW_SHOW);	
}

void CGGTongView::OnWindowSplitDel() 
{
	CWnd *pParent = GetParent();

	while (1)
	{
		if (NULL == pParent)
			return;

		if (pParent->IsKindOf(RUNTIME_CLASS(CMPIChildFrame)))
			break;

		pParent = pParent->GetParent();
	}

	if (NULL == pParent)
		return;

	CMPIChildFrame *pChildFrame = (CMPIChildFrame *)pParent;

	pChildFrame->DelSplit(this);
}

int CGGTongView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	ModifyStyleEx(WS_EX_CLIENTEDGE, 0, 0);

	// TODO: Add your specialized creation code here
	m_IoViewManager.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10204);
	
	return 0;
}

void CGGTongView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CRect rect(0, 0, cx, cy);
	// rect.DeflateRect(1, 1);
	
	CRect RectIoViewManager(rect);
	RectIoViewManager.DeflateRect(KiGtongViewBorderLeft,KiGtongViewBorderTop,KiGtongViewBorderRight,KiGtongViewBorderBottom);
	m_IoViewManager.MoveWindow(RectIoViewManager);

	if ( m_bDelByBiSplitTrack )
	{
		// �����϶�ɾ����ʱ��,��Ҫ������󻯱�־
		return;
	}

 	CRect rectThis;
 	CRect rectParent;
 
 	CWnd* pParent = GetParent();
 
 	if ( NULL != pParent )
 	{
 		pParent->GetClientRect(&rectParent);
 	}
 
 	GetClientRect(&rectThis);
 
	if ( rectThis.Width() == 0 || rectThis.Height() == 0 )
	{
		return;
	}

 	// F7״̬����Ҫ����		
	SetSubIoViewF7MaxFlag();
}

BOOL CGGTongView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
	return CView::OnEraseBkgnd(pDC);
}


const char * CGGTongView::GetXmlElementValue()
{
	return KStrElementValue;
}

CString CGGTongView::ToXml(CGGTongView * pActiveView)
{
	CString StrThis;
	/*
		error 529: (Warning -- Symbol  not subsequently referenced)
	*/
// 	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
// 	CMPIChildFrame *pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());

	// �Ƿ񼤻���Ϣ?
    int32 iActive = (this == pActiveView) ? 1 : 0;
	//int32 iActiveOnly = (this == pMainFrame->GetActiveGGTongViewXml()) ? 1 : 0; // ÿ��������ҳ�����һ����������������mainframe
	int32 iActiveOnly = iActive;
	int32 iF7View = GetMaxF7Flag() ? 1 : 0;

	/*
		Warning -- Passing struct 'CStringT' to ellipsis
	*/
	//lint --e{437}
	//
	StrThis.Format(L"<%s %s=\"%d\" %s=\"%d\" %s=\"%d\" %s=\"%d\"  %s=\"%d\"  %s=\"%d\"  %s=\"%d\"  %s=\"%d\" %s=\"%d\">\n", CString(GetXmlElementValue()), 
												  CString(KStrElementAttriActive), iActive,
												  CString(KStrElementAttriActiveOnly), iActiveOnly,
												  CString(KStrElementAttriF7Max),  iF7View,
												  CString(KStrElementAttriBLockMinMax), m_bLockMinMaxInfo,
												  CString(KStrElementAttriLockMinX), m_mmiLockMinMaxInfo.ptMinTrackSize.x,
												  CString(KStrElementAttriLockMinY), m_mmiLockMinMaxInfo.ptMinTrackSize.y,
												  CString(KStrElementAttriLockMaxX), m_mmiLockMinMaxInfo.ptMaxTrackSize.x,
												  CString(KStrElementAttriLockMaxY), m_mmiLockMinMaxInfo.ptMaxTrackSize.y,
												   CString(KStrElementAttriCanF7), m_bCanF7
												  );	
	StrThis += m_IoViewManager.ToXml();
	StrThis += L"</";
	StrThis += CString(GetXmlElementValue());
	StrThis += L">\n";
	return StrThis;
}

bool32 CGGTongView::FromXml(CWnd *pParent,TiXmlElement *pElement)
{
	m_bFromXML = true;

	if (NULL == pElement)
		return false;

	// �ж��ǲ��������Լ���
	const char *pcValue = pElement->Value();
	if (NULL == pcValue || strcmp(GetXmlElementValue(), pcValue) != 0)
		return false;

	// Tab �еĻ��ͼ����	
	bool32  bActive = 0;
	const char *pcAttrValue = pElement->Attribute(KStrElementAttriActive);
	if ( NULL != pcAttrValue )
	{
		bActive = atoi(pcAttrValue);
		//CFrameWnd *pFrame = GetParentFrame();
		//if ( NULL != bActive )
		//{
		//	pFrame->SetActiveView(this);
		//}
	}

	CWnd * pWnd = pParent;
	CTabSplitWnd * pTab = NULL;

	while (pWnd)
	{
		if (pWnd->IsKindOf(RUNTIME_CLASS(CTabSplitWnd)))
		{
			pTab = (CTabSplitWnd *)pWnd;
			break;
		}

		pWnd = pWnd->GetParent();		
	}

	if (pTab)
	{
		if ( bActive )
		{			
			pTab->m_ActiveViews.Add(this);
		}		
	}

	// Mainframe �Ļ��ͼ����
	pcAttrValue = pElement->Attribute(KStrElementAttriActiveOnly);	
	if ( NULL != pcAttrValue )
	{
		bActive = atoi(pcAttrValue);
		if ( bActive )
		{
			CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			pMainFrame->SetActiveGGTongViewXml(this);
		}
	}

	// ��С������
	pcAttrValue = pElement->Attribute(KStrElementAttriBLockMinMax);
	if ( NULL != pcAttrValue )
	{
		m_bLockMinMaxInfo = atoi(pcAttrValue) != 0;
	}
	// F7����
	pcAttrValue = pElement->Attribute(KStrElementAttriCanF7);
	if ( NULL != pcAttrValue )
	{
		m_bCanF7 = atoi(pcAttrValue) != 0;
	}
	if ( m_bLockMinMaxInfo )
	{
		// ��ȡ����Ĵ�С
		m_mmiLockMinMaxInfo.ptMinTrackSize = CPoint(0, 0);	// �����ʼû������
		pcAttrValue = pElement->Attribute(KStrElementAttriLockMinX);
		if ( NULL != pcAttrValue )
		{
			m_mmiLockMinMaxInfo.ptMinTrackSize.x = atoi(pcAttrValue);
		}
		pcAttrValue = pElement->Attribute(KStrElementAttriLockMinY);
		if ( NULL != pcAttrValue )
		{
			m_mmiLockMinMaxInfo.ptMinTrackSize.y = atoi(pcAttrValue);
		}
		//--- wangyongxue 2016/06/24
		pcAttrValue = pElement->Attribute(KStrElementAttriLockMaxX);
		if ( NULL != pcAttrValue )
		{
			m_mmiLockMinMaxInfo.ptMaxTrackSize.x = atoi(pcAttrValue);
		}
		pcAttrValue = pElement->Attribute(KStrElementAttriLockMaxY);
		if ( NULL != pcAttrValue )
		{
			m_mmiLockMinMaxInfo.ptMaxTrackSize.y = atoi(pcAttrValue);
		}
	}

	// ��󻯱�־
	pcAttrValue = pElement->Attribute(KStrElementAttriF7Max);
	if ( NULL != pcAttrValue )
	{
		bool32 bF7View = atoi(pcAttrValue) != 0;
		if ( bF7View )
		{
			CMPIChildFrame *pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
			if ( NULL != pFrame )
			{
				pFrame->SetF7GGTongViewXml(this);	// ����xmlF7��־
			}
		}
	}
	
	// �����ӽڵ㣬 �ҵ���һ��������IoViewManager�ģ� ����IoViewManager����
	// һ��Ҫ�ҵ��� ������Ϊ����
	TiXmlElement *pElementIoViewManager = NULL;
	pElementIoViewManager = pElement->FirstChildElement();
	
	if (NULL == pElementIoViewManager)
		return false;

	pcValue = pElementIoViewManager->Value();
	if (NULL == pcValue || strcmp(m_IoViewManager.GetXmlElementValue(), pcValue) != 0)
		return false;
	
	m_bFromXML = false;

	return m_IoViewManager.FromXml(pElementIoViewManager);
}

CString CGGTongView::GetDefaultXML(int32 iGroupID,bool32 bBlank, CIoViewBase *pIoView)
{
	CString StrDefaultXML;	
	StrDefaultXML = L"<";
	StrDefaultXML += GetXmlElementValue();
	StrDefaultXML += L"> \n";

	StrDefaultXML += CIoViewManager::GetDefaultXML(iGroupID,bBlank, pIoView);

	StrDefaultXML += L"</";
	StrDefaultXML += GetXmlElementValue();
	StrDefaultXML += L"> \n";

    return StrDefaultXML;
}

void CGGTongView::DrawGGTongView(CDC* pDC)
{	
	if ( !IsWindowVisible() )
	{
		// ���ɼ���ʱ��
		return;
	}

	if ( NULL == pDC )
	{
		return;
	}
	
	CRect rect;
	GetClientRect(&rect);
	COLORREF clr =  CFaceScheme::Instance()->GetSysColor(ESCBackground);
	
	if ( !m_bDelByBiSplitTrack )
	{		
		COLORREF clrText = CFaceScheme::Instance()->GetSysColor(ESCText);
	
		// ���ڿհ״��ڣ� ��ʾ��ʾ����
		if ( GetEmptyFlag() )
		{
			CRect rectText = rect;
			rectText.left += 7;

			pDC->FillSolidRect(&rect, clr);
			pDC->SetTextColor(clrText);	
			
			CFont* pOldFont = pDC->SelectObject(CFaceScheme::Instance()->GetSysFontObject(ESFSmall));
			int32 iOldMode = pDC->SetBkMode(TRANSPARENT);
			CString StrTip;
			if ( IsLockedSplit() )
			{
				StrTip = L"����[��ʼҳ������], Ȼ��������Ҽ����ñ�������ʾ����.";
			}
			else
			{
				//StrTip = L"��������Ҽ����ñ�������ʾ����.";
				StrTip = L"";
			}
			pDC->DrawText(StrTip, &rectText, DT_LEFT|DT_VCENTER|DT_WORDBREAK);
			
			pDC->SelectObject(pOldFont);
			pDC->SetBkMode(iOldMode);
		}		
	}
	
	{
		CRect rtClip;
		CRect rt(rect);
		pDC->GetClipBox(rtClip);			
		CRgn rgnBorder, rgnWhole, rgnInside,rgnClip;
		rgnWhole.CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom);
		rgnBorder.CreateRectRgn(0,0,0,0);		
		rgnInside.CreateRectRgn(rt.left + KiGtongViewBorderLeft, rt.top + KiGtongViewBorderTop, rt.right - KiGtongViewBorderRight   , rt.bottom- KiGtongViewBorderBottom);
		rgnBorder.CombineRgn(&rgnWhole, &rgnInside, RGN_DIFF);
		rgnClip.CreateRectRgn(rtClip.left, rtClip.top, rtClip.right, rtClip.bottom);
		pDC->SelectClipRgn(&rgnBorder);
		
		if (m_bShowGtongVeiwBorder)	
		{
			clr =  RGB(35,35,35);
		}		
		pDC->FillSolidRect(&rect,clr);	
		pDC->SelectClipRgn(&rgnClip, RGN_COPY);		
		rgnBorder.DeleteObject();
		rgnClip.DeleteObject();
		rgnInside.DeleteObject();
		rgnWhole.DeleteObject();
	}

	// ��ͼ������������
//	DrawGGTongFocusRect();
}

void CGGTongView::OnPaint()
{
 	CPaintDC dc(this);
	DrawGGTongView(&dc);
}

bool32 CGGTongView::GetEmptyFlag()
{		
	m_bEmptyGGTongView = m_IoViewManager.m_IoViewsPtr.GetSize()==0?true:false;
	return m_bEmptyGGTongView;
}

bool32 CGGTongView::GetDragFlag()
{
	if ( EDDNONE != m_eDragDirection )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool32 CGGTongView::GetMaxF7Flag()
{
	if ( !IsWindowVisible() )
	{
		return false;	// ���ɼ�����Ȼno f7
	}
	CMPIChildFrame *pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pChildFrame )
	{
		return this == pChildFrame->GetF7GGTongView();
	}
	return false;		 // Ĭ����F7
}

void CGGTongView::ClearAllDragFlags()
{
	E_DragDirection eDragDirectionBk = m_eDragDirection;

	// ��������ı�־
	m_bValidAction	 = false;
	m_bPrepareDrag	 = false;		
	m_bLButtonDown	 = false;

	m_eDragDirection = EDDNONE;	

 	if ( EDDNONE != eDragDirectionBk && EDDNONE == m_eDragDirection )
 	{ 		
 		int32 iCurShow = m_IoViewManager.m_GuiTabWnd.GetCurtab();
 		
 		if ( iCurShow < 0 || iCurShow >= m_IoViewManager.m_IoViewsPtr.GetSize() )
 		{
 			return;
 		}
 
 		m_IoViewManager.m_GuiTabWnd.SetCurtab(iCurShow);		
 		m_IoViewManager.OnTabSelChange();

		// ����״̬�ı��ʱ��,֪ͨ�������ͼ���� - SETREDRAW���ò��ɼ���ͼ�ɼ�����ǰ���ˢ����ʾ����������size��TODO :���lockredraw
		for ( int32 i = 0 ; i < m_IoViewManager.m_IoViewsPtr.GetSize(); i++ )
		{
			CIoViewBase* pIoView = m_IoViewManager.m_IoViewsPtr.GetAt(i);
			if ( pIoView->IsWindowVisible() )
			{
				pIoView->UnLockRedraw();	
			}
 		}
 	}	
}

void CGGTongView::SetSubIoViewF7MaxFlag()
{

}

void CGGTongView::SetBiSplitTrackDelFlag(bool32 bDel)
{
	m_bDelByBiSplitTrack = bDel;

	for ( int32 i = 0; i < m_IoViewManager.m_IoViewsPtr.GetSize(); i++ )
	{
		CIoViewBase* pIoView = m_IoViewManager.m_IoViewsPtr[i];
		pIoView->SetBiSplitTrackDelFlag(bDel);		
	}	
}

LRESULT CGGTongView::OnMouseLeave(WPARAM wParam,LPARAM lParam)
{	
	// 	m_bTracking		= false;
	// 	m_bPrepareDrag	= false;		
	return 0;
}


LRESULT CGGTongView::OnAddSplit(WPARAM wParam,LPARAM lParam)
{	
	CMPIChildFrame::E_SplitDirection eSD;
	switch(wParam)
	{
	case CMPIChildFrame::SD_TAB:eSD = CMPIChildFrame::SD_TAB;break;
	case CMPIChildFrame::SD_LEFT:eSD = CMPIChildFrame::SD_LEFT;break;
	case CMPIChildFrame::SD_RIGHT:eSD = CMPIChildFrame::SD_RIGHT;break;
	case CMPIChildFrame::SD_TOP:eSD = CMPIChildFrame::SD_TOP;break;
	case CMPIChildFrame::SD_BOTTOM:eSD = CMPIChildFrame::SD_BOTTOM;break;
	default:
		eSD = CMPIChildFrame::SD_TAB;
	}
	AddSplit(eSD);
	return 0;
}


LRESULT CGGTongView::OnSetActiveCrossFlag(WPARAM wParam,LPARAM lParam)
{
	SetActiveCrossFlag(bool32(wParam));
	return TRUE;
}

LRESULT CGGTongView::OnIsKindOfCGGTongView(WPARAM wParam,LPARAM lParam)
{
	return TRUE;
}

LRESULT CGGTongView::OnToXml(WPARAM wParam,LPARAM lParam)
{
	CString str = ToXml((CGGTongView *)wParam);
	TCHAR *pstr = new TCHAR[str.GetLength()+1];
	memset(pstr,0,sizeof(TCHAR)*(str.GetLength()+1));
	lstrcpy(pstr,str);
	return (LRESULT)pstr;
}
LRESULT CGGTongView::OnSetBiSplitTrackDelFlag(WPARAM wParam,LPARAM lParam)
{
	SetBiSplitTrackDelFlag(bool32(wParam));
	return TRUE;
}

BOOL CGGTongView::PreTranslateMessage(MSG* pMsg)
{
	// �����ڵľ���
	CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	CRect rtParentFrame;
	pParentFrame->GetClientRect(&rtParentFrame);
	
	// ���childFrame �� TabSplitWnd...
	CTabSplitWnd * pTabParent = NULL;
	CWnd * pParent = GetParent();

	while (pParent)
	{
		if (pParent->IsKindOf(RUNTIME_CLASS(CMPIChildFrame)))
		{
			break;
		}
		else if (pParent->IsKindOf(RUNTIME_CLASS(CTabSplitWnd)))
		{
			pTabParent = (CTabSplitWnd *)pParent;
			break;
		}
		pParent = pParent->GetParent();
	}

	if ( NULL != pTabParent && !pTabParent->m_bDelAll )
	{
		int32 iHeight = pTabParent->m_wndTabCtrl.GetFitHorW();
		rtParentFrame.bottom -= iHeight;
	}

	CPoint ptForView; 
	GetCursorPos(&ptForView);
	ScreenToClient(&ptForView);

	CPoint ptForFrame;
	GetCursorPos(&ptForFrame);
	GetParentFrame()->ScreenToClient(&ptForFrame);


 	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
// 	CIoViewBase * pIoView   = pMainFrame->FindActiveIoView();

	bool32	bLockSplit = IsLockedSplit();

	if ( WM_RBUTTONDOWN == pMsg->message )
	{
		// �ڽ�������²��ܲ�����ͼ
		if (GetEmptyFlag() && !IsLockedSplit()) 
		{				
			pMainFrame->SetEmptyGGTongView(this);

			CNewMenu Menu;
			Menu.LoadMenu(IDR_MENU_EMPTY_VIEW);
			Menu.LoadToolBar(g_awToolBarIconIDs);

			CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, Menu.GetSubMenu(0));
			
			CMenu* pTempMenu = pPopMenu->GetSubMenu(L"��������");
			CNewMenu * pIoViewPopMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );
			ASSERT(NULL != pIoViewPopMenu );
			CIoViewBase::AppendIoViewsMenu(pIoViewPopMenu, IsLockedSplit());
			
			CPoint pt;
			GetCursorPos(&pt);
			pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, AfxGetMainWnd());	
			pPopMenu->DestroyMenu();			
			return TRUE;
		}
	}
	else if (WM_MOUSEMOVE == pMsg->message) // ���������
	{
		// 	if ( !m_bTracking )
		// 	{
		// 		// ע������뿪���¼���־
		// 		TRACKMOUSEEVENT tme;
		// 		tme.cbSize = sizeof(tme);
		// 		tme.hwndTrack = m_hWnd; 
		// 		tme.dwFlags = TME_LEAVE ;
		// 		tme.dwHoverTime = 1;
		// 		
		// 		m_bTracking = _TrackMouseEvent(&tme);		
		// 	}

		// ����������궯ʱ,���״̬

		if ( !bLockSplit )
		{
			if (!m_bLButtonDown)
			{
				if (ptForFrame.x <= rtParentFrame.left + SENSITIVESIZE )
				{
					m_bPrepareDrag = true;			
					HCURSOR hCurRight  = AfxGetApp()->LoadCursor(IDC_CUR_DRAG_RIGHT);
					::SetCursor(hCurRight);			
				}
				else if ( ptForFrame.x >= rtParentFrame.right - SENSITIVESIZE )
				{
					m_bPrepareDrag = true;
					HCURSOR	hCurLeft = AfxGetApp()->LoadCursor(IDC_CUR_DRAG_LEFT);
					::SetCursor(hCurLeft);			
				}
				else if( ptForFrame.y <= rtParentFrame.top + SENSITIVESIZE || ptForFrame.y >= rtParentFrame.bottom - SENSITIVESIZE)
				{
					m_bPrepareDrag = true;
					HCURSOR hCurUpDown	= AfxGetApp()->LoadCursor(IDC_CUR_DRAG_UPDOWN);			
					::SetCursor(hCurUpDown);			
				}
				else
				{
					m_bPrepareDrag = false;
				}
			}
			
			if (m_bLButtonDown && m_eDragDirection != EDDNONE)
			{
				// �������,�϶���ʱ��,��ش���
				// �����갴��,����ƶ�,��ö��������,����������
				// 1: ���������״ 2: ���� 3:�ж��Ƿ�����Ч����			
				
				CRect rtClient;
				GetClientRect(&rtClient);
				
				if ( m_eDragDirection == EDDLEFT )
				{	
					if ( ptForView.x < VALIDAREASIZE || ptForView.x > (rtClient.Width()-VALIDAREASIZE))
					{
						// �Ƿ�����
						HCURSOR	hCurLeftStop = AfxGetApp()->LoadCursor(IDC_CUR_DRAG_LEFT_STOP);
						::SetCursor(hCurLeftStop);
						m_bValidAction = false;
					}
					else
					{
						HCURSOR	hCurLeft = AfxGetApp()->LoadCursor(IDC_CUR_DRAG_LEFT);
						::SetCursor(hCurLeft);
						m_bValidAction = true;
					}
				}
				else if (m_eDragDirection == EDDRIGHT)
				{
					if ( ptForView.x < VALIDAREASIZE || ptForView.x > (rtClient.Width()-VALIDAREASIZE))
					{
						// �Ƿ�����
						HCURSOR	hCurRightStop = AfxGetApp()->LoadCursor(IDC_CUR_DRAG_RIGHT_STOP);
						::SetCursor(hCurRightStop);
						m_bValidAction = false;
					}
					else
					{
						HCURSOR hCurRight = AfxGetApp()->LoadCursor(IDC_CUR_DRAG_RIGHT);
						::SetCursor(hCurRight);
						m_bValidAction = true;
					}
				}
				else if (m_eDragDirection == EDDTOP || m_eDragDirection == EDDBOTTOM)
				{	
					if (ptForView.y < VALIDAREASIZE || ptForView.y > (rtClient.Height() - VALIDAREASIZE))
					{
						HCURSOR hCurUpDownStop	= AfxGetApp()->LoadCursor(IDC_CUR_DRAG_UPDOWN_STOP);							
						::SetCursor(hCurUpDownStop);
						m_bValidAction = false;
					}
					else
					{
						HCURSOR hCurUpDown	= AfxGetApp()->LoadCursor(IDC_CUR_DRAG_UPDOWN);							
						::SetCursor(hCurUpDown);
						m_bValidAction = true;
					}				
				}
				//////////////////////////////////////////////////////////////////////////
				// ��ǰ������,����������GGTonView �Ŀͻ�������
				//  ����,����Bug
				
				//  pMsg ������,����й�����,����ִ���,��������Ļ����,��ת����;
				
				m_ptDraging.x = ptForView.x;
				m_ptDraging.y = ptForView.y;
				
				DrawDragLine(m_ptDraging, m_eDragDirection);
			}	
			
			if ( m_bPrepareDrag )
			{
				return TRUE;
			}
			else
			{
				ClearAllDragFlags();
			} 	
			
			
		}
		
		//
		if ( m_bDragMerchLBDown )
		{
			m_bDragMerch = true;
			
			//HCURSOR hCur = AfxGetApp()->LoadCursor(IDC_CUR_DRAG_MERCH);							
			//::SetCursor(hCur);
			return TRUE; 
		}		
	}
	else if (WM_LBUTTONDOWN == pMsg->message)
	{
		if (GetEmptyFlag())
		{
			CMPIChildFrame * pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
			if (NULL != pChildFrame)
			{
				pChildFrame->SetChildFrameTitle(AfxGetApp()->m_pszAppName);
			}
			pMainFrame->SetEmptyGGTongView(this);
		}

		CPoint ptScreen;
		GetCursorPos(&ptScreen);
		
		if ( BeValidDragMerchBegin(ptScreen) )
		{
			m_bDragMerchLBDown = true;
			m_pGGTViewDragBegin = this;
		}
		else
		{
			ClearDragMerchFlags();
		}

		if ( !bLockSplit ) // ������
		{
			// �������,�ж������ķ���.   - 0001758 ����������������Ϣ��ֻ������Ҫ׷�ٵ�ʱ��capture xl
			// SetCapture();	
			m_bLButtonDown = true;
			
			if ( ptForFrame.x <= rtParentFrame.left + SENSITIVESIZE)
			{
				m_eDragDirection = EDDRIGHT;
			}
			
			if ( ptForFrame.x >= rtParentFrame.right - SENSITIVESIZE)
			{
				m_eDragDirection = EDDLEFT;
			}
			
			if (ptForFrame.y <= rtParentFrame.top + SENSITIVESIZE )
			{
				m_eDragDirection = EDDBOTTOM;
			}
			
			if ( ptForFrame.y >= rtParentFrame.bottom - SENSITIVESIZE )
			{
				m_eDragDirection = EDDTOP;
			}
			
			if ( EDDNONE != m_eDragDirection )
			{
				// ��captureת�Ƶ����� xl
				SetCapture();
				// �� IoView �е�OnPaint LockRedraw ��Ч
				RedrawWindow();
				return TRUE;
			}
			else
			{				
				m_bLButtonDown = false;	// ��Ч����ȡ����־
			}
		}
	}
	else if ( WM_LBUTTONUP == pMsg->message)
	{	
		// 0001758 �����ж��Ƿ��ǵ�ǰ������capture��������ǵĻ��������
		if ( GetCapture() == this )
		{
			ReleaseCapture();
			
			if (m_bValidAction)
			{		
				if (m_bLButtonDown && m_eDragDirection != EDDNONE && !bLockSplit)	// ���������
				{
					// �����갴��,����ƶ�,��ö��������,����������,��ʱ�ſ����,���ǽ�������
					// 1: �зִ��� 2: ��ԭ�����״
					
					//  pMsg ������,����й�����,����ִ���,��������Ļ����,��ת����;
					CRect rtClient;
					GetClientRect(&rtClient);
					
					CPoint pt;				
					GetCursorPos(&pt);
					ScreenToClient(&pt);
					
					if (pt.x < 0 || pt.y < 0)
					{
						return CView::PreTranslateMessage(pMsg);
					}
					
					double dRadio;
					
					switch(m_eDragDirection)
					{
					case EDDLEFT:
						{
							if (pt.x > VALIDAREASIZE)   // ���һ���쳣����Ĺؿ�.�ж���ӵ������Ƿ���ں���ֵ
							{
								dRadio = double(pt.x)/double(rtClient.Width());
								AddSplit(CMPIChildFrame::SD_RIGHT,dRadio);													
							}
						}
						break;
					case EDDRIGHT:
						{
							if((rtClient.Width() - pt.x) > VALIDAREASIZE )
							{
								dRadio = double(pt.x)/double(rtClient.Width());
								AddSplit(CMPIChildFrame::SD_LEFT,dRadio);
							}						
						}
						break;
						
					case EDDTOP:
						{
							if((rtClient.Height() - pt.y) > VALIDAREASIZE )
							{
								dRadio = double(pt.y)/double(rtClient.Height());
								AddSplit(CMPIChildFrame::SD_BOTTOM,dRadio);
							}						
						}
						break;
						
					case EDDBOTTOM:
						{
							if (pt.y > VALIDAREASIZE)
							{
								dRadio = double(pt.y)/double(rtClient.Height());
								AddSplit(CMPIChildFrame::SD_TOP,dRadio);
							}						
						}
						break;
					default:
						break;
					}
				}			
			}
			
			ClearAllDragFlags();
			CClientDC dc(this);
			DrawGGTongView(&dc);
		}

		// ���ǵ�ǰ���ڲ����
		if ( m_bDragMerchLBDown && m_bDragMerch && NULL != m_pGGTViewDragBegin )
		{
			// ������ǲ������Լ��ķ�Χ��
			CArray<CGGTongView*, CGGTongView*> aGGTongViews;
			
			pMainFrame->GetAllGGTongViews(aGGTongViews);

			CPoint pt;
			GetCursorPos(&pt);

			bool32 bFind = false;

			//
			CMPIChildFrame* pParentFrm = (CMPIChildFrame*)m_pGGTViewDragBegin->GetParentFrame();
			if ( NULL == pParentFrm )
			{
				ClearAllDragFlags();
			}
			else
			{
				//
				for ( int32 i = 0; i < aGGTongViews.GetSize(); i++ )
				{
					CGGTongView* p = aGGTongViews.GetAt(i);
					
					if ( NULL == p || !p->IsWindowVisible() )
					{
						continue;
					}
					
					//
					CMPIChildFrame* pParentFrmNow = (CMPIChildFrame*)p->GetParentFrame();
					if ( pParentFrm != pParentFrmNow )
					{
						continue;
					}

					//
					CRect RectWnd;
					p->GetWindowRect(RectWnd);
					
					if ( RectWnd.PtInRect(pt) )
					{
						//
						bFind = true;
						
						bool32 bDelView = p->OnDragMerchEnd();
						if ( bDelView )
						{
							return TRUE;
						}
					}
				}		
				
				//
				if ( !bFind )
				{
					//ASSERT(0);
				}
			}			
		}
		else
		{
			ClearDragMerchFlags();
		}
	}
	else if ( WM_KEYDOWN == pMsg->message && VK_RETURN == pMsg->wParam )
	{
		DealEnterKey();
		return TRUE;
	}

	return CView::PreTranslateMessage(pMsg);
}

void CGGTongView::DrawDragLine(CPoint pt,E_DragDirection eDragDirection)
{	
	CRect rtClient;
	GetClientRect(&rtClient);
	CClientDC dc1(this);
	CMemDC dc(&dc1,&rtClient);

	dc.FillSolidRect(&rtClient,RGB(0,0,0));
	
	COLORREF clrLine = CFaceScheme::Instance()->GetSysColor(ESCChartAxisLine);

	if (eDragDirection == EDDLEFT || eDragDirection == EDDRIGHT)
	{
		// ������

		// ����ǰһ����
		/*
		CPen pen(PS_SOLID,2,clrLine);
		CPen * pOldPen = dc.SelectObject(&pen);

		int oldRop=dc.SetROP2(R2_XORPEN); 

		if (m_ptDragPre != CPoint(0,0))
		{
		dc.MoveTo(m_ptDragPre.x,0);
		dc.LineTo(m_ptDragPre.x,rtClient.Height());
		}

		dc.SetROP2(oldRop);

		dc.MoveTo(pt.x,0);
		dc.LineTo(pt.x,rtClient.Height());

		dc.SelectObject(pOldPen);
		pen.DeleteObject();
		*/

		CPen pen(PS_SOLID,2,clrLine);
		CPen * pOldPen = dc.SelectObject(&pen);
		
		dc.MoveTo(pt.x,0);
		dc.LineTo(pt.x,rtClient.Height());
		
		dc.SelectObject(pOldPen);
		pen.DeleteObject();		
	}
	else if ( eDragDirection == EDDTOP || eDragDirection == EDDBOTTOM )
	{
		// ������
		
		CPen pen(PS_SOLID,2,clrLine);
		CPen * pOldPen = dc.SelectObject(&pen);
		
		dc.MoveTo(0,pt.y);
		dc.LineTo(rtClient.Width(),pt.y);
		
		dc.SelectObject(pOldPen);
		pen.DeleteObject();

	/*	
		CPen pen(PS_SOLID,2,clrLine);
		CPen * pOldPen = dc.SelectObject(&pen);
		
		int oldRop=dc.SetROP2(R2_XORPEN); 
		
		if (m_ptDragPre != CPoint(0,0))
		{
			dc.MoveTo(0,m_ptDragPre.y);
			dc.LineTo(rtClient.Width(),m_ptDragPre.y);
		}
		
		dc.SetROP2(oldRop);
		
		dc.MoveTo(0,pt.y);
		dc.LineTo(rtClient.Width(),m_ptDragPre.y);
		
		dc.SelectObject(pOldPen);
		pen.DeleteObject();
	*/
	}
	m_ptDragPre = m_ptDraging;	
}

int32 CGGTongView::FindIoView(bool32 bKline)
{
	// �ҷ�ʱ,����K����ͼ

	int32 iSize = m_IoViewManager.m_IoViewsPtr.GetSize();
	
	if (iSize <= 1)
	{
		return -1;
	}

	for ( int32 i = 0 ; i< iSize; i++)
	{

		CIoViewBase * pIoView = m_IoViewManager.m_IoViewsPtr[i];

		if (bKline)
		{
			if ( pIoView->IsKindOf(RUNTIME_CLASS(CIoViewKLine)))
			{
				return i;		
			}
		}
		else
		{
			if ( pIoView->IsKindOf(RUNTIME_CLASS(CIoViewTrend)))
			{
				return i;
			}
		}
		
	}

	return -1;
}

void CGGTongView::DealEnterKey()
{
	// ��ǰ�Ƿ�ʱ����,�л���K ��. ��ǰ��K ��,�л�����ʱ����. û�еĻ��½�һ��;
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	int32 iIoViewNum = m_IoViewManager.m_IoViewsPtr.GetSize();

	if (iIoViewNum < 1)
	{
		return;
	}
	int32 iCurTab	 = m_IoViewManager.m_GuiTabWnd.GetCurtab();
	if ( iCurTab < 0 )
	{
		return;
	}
	CIoViewBase * pCurIoView = m_IoViewManager.m_IoViewsPtr.GetAt(iCurTab);

	if ( pCurIoView->IsKindOf(RUNTIME_CLASS(CIoViewKLine)))
	{
		// �����ǰ��KLine
		CIoViewKLine * pIoViewKLine = (CIoViewKLine *)pCurIoView;
	
		CChartRegion * pMainRegion = pIoViewKLine->m_pRegionMain;
		if ( CheckFlag(pMainRegion->m_iFlag,CChartRegion::KCrossAble) && pMainRegion->IsActiveCross() && ENTIDay == pIoViewKLine->GetTimeInterval())
		{
			// �����ǰ���� K ��,����ʮ�ֹ�꼤���ʱ��,������ʷ��ʱ����ͼ
			// �õ���ǰ��ʮ�ֹ�����ڵ��Ǹ��ڵ�;

			CKLine KLineNow;
			pIoViewKLine->GetCrossKLine(KLineNow);
			pMainFrame->DealHistoryTrendDlg(pIoViewKLine, KLineNow);
		}
		else
		{
			pMainFrame->OnShowMerchInNextChart(pCurIoView);
// 			int32 iPos = FindIoView(false);
// 			if ( iPos < 0 || iPos >= iIoViewNum)
// 			{
// 				// û�ҵ�,�½�һ����ʱ����,��ǰ����ʾ;
// 				pMainFrame->CreateIoViewByPicMenuID(ID_PIC_TREND, true);
// 			}
// 			else
// 			{
// 				// �ҵ���,ǰ����ʾ
// 				m_IoViewManager.m_GuiTabWnd.SetCurtab(iPos);
// 			}
		}
	}
	else if ( pCurIoView->IsKindOf(RUNTIME_CLASS(CIoViewTrend)))
	{
		// �����ǰ�Ƿ�ʱ����
		pMainFrame->OnShowMerchInNextChart(pCurIoView);
// 		int32 iPos = FindIoView(true);
// 
// 		if ( iPos < 0 || iPos >= iIoViewNum)
// 		{
// 			// û�ҵ�,�½�һ��KLine ��ͼ,��ǰ����ʾ;
// 			pMainFrame->CreateIoViewByPicMenuID(ID_PIC_KLINE, true);
// 		}
// 		else
// 		{
// 			// �ҵ���,ǰ����ʾ
// 			m_IoViewManager.m_GuiTabWnd.SetCurtab(iPos);
// 		}
	}
	else
	{
		// ������ҵ����ͼ�����;
		
		return;
	}			
}


void CGGTongView::OnNcPaint()
{
	CView::OnNcPaint();
}

void CGGTongView::DrawGGTongFocusRect()
{
	if ( !GetEmptyFlag() || !m_bActiveFocus )
	{
		return;
	}
	
	COLORREF clr = RGB(255, 255, 0);
		
	CRect rect;
	GetClientRect(rect);

	CClientDC dc(this);
	CRect rectActive = rect;
	rectActive.top		+= 2;
	rectActive.left		+= 2;
	rectActive.bottom	= rectActive.top + 2;
	rectActive.right	= rectActive.left + 2;
	
	dc.FillSolidRect(&rectActive, clr);
}

bool32 CGGTongView::IsLockedSplit() const
{
	CMPIChildFrame *pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pFrame )
	{
		return pFrame->IsLockedSplit();
	}
	return true;		// ��childframe�½�ֹ�ָ�
}

void CGGTongView::OnUpdateLayoutAdjust( CCmdUI *pCmdUI )
{
	if ( IsLockedSplit() )
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(TRUE);
	}
}

void CGGTongView::OnGetMinMaxInfo( MINMAXINFO* lpMMI )
{
	CView::OnGetMinMaxInfo(lpMMI);
	if ( NULL != lpMMI && m_bLockMinMaxInfo )
	{
		// ��ѯ�ʵ�ǰ��ͼ�Ƿ�������Ӧ��С����������ͼ��Ϊ׼
		CIoViewBase *pIoViewActive = m_IoViewManager.GetActiveIoView();
		MINMAXINFO mmi = *lpMMI;
		if ( NULL != pIoViewActive )
		{
			pIoViewActive->SendMessage(WM_GETMINMAXINFO, NULL, (LPARAM)lpMMI);
		}
		if ( memcmp(&mmi, lpMMI, sizeof(mmi)) != 0 )
		{
			// ʹ����ͼ��
			// view��������С��ϢҲͬ�����
			m_mmiLockMinMaxInfo = *lpMMI;
		}
		else
		{
			// ʹ������Ӵ��ڵ�
			lpMMI->ptMinTrackSize = m_mmiLockMinMaxInfo.ptMinTrackSize;	// ����Сֵ��Ч
			if ( m_mmiLockMinMaxInfo.ptMaxTrackSize.x > 0 )
			{
				lpMMI->ptMaxTrackSize.x = m_mmiLockMinMaxInfo.ptMaxTrackSize.x;
			}
			if ( m_mmiLockMinMaxInfo.ptMaxTrackSize.y > 0 )
			{
				lpMMI->ptMaxTrackSize.y = m_mmiLockMinMaxInfo.ptMaxTrackSize.y;
			}
		}
	}
	else
	{
		ASSERT( NULL != lpMMI );
	}
}

void CGGTongView::OnUpdateLockMinMax( CCmdUI *pCmdUI )
{
	if ( NULL != pCmdUI )
	{
		// ����������½�ֹ�˰�ť, ���еĴ��ڻ���䲻�����
		pCmdUI->SetCheck(m_bLockMinMaxInfo ? 1 : 0);

		bool32 bLock = IsLockedSplit();
		bool32 bF7 = GetMaxF7Flag();
		pCmdUI->Enable(m_bLockMinMaxInfo || (!bF7 && bLock) );	// ��������ˣ����������ر�, f7��Ҳ��ֹ
	}
}

void CGGTongView::OnLockMinMax()
{
	m_bLockMinMaxInfo = !m_bLockMinMaxInfo;
	if ( m_bLockMinMaxInfo )
	{
		CRect rc;
		GetWindowRect(rc);
		m_mmiLockMinMaxInfo.ptMinTrackSize.x = rc.Width();
		m_mmiLockMinMaxInfo.ptMinTrackSize.y = rc.Height();

		CMPIChildFrame *pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
		if ( NULL != pFrame )
		{
			pFrame->RecalcLayoutAsync();
		}
	}
	else
	{
		ZeroMemory(&m_mmiLockMinMaxInfo, sizeof(m_mmiLockMinMaxInfo));
	}
}

bool32 CGGTongView::BeValidDragMerchBegin(CPoint pt)
{
	// 
	CRect RectScreen;
	GetWindowRect(RectScreen);

	// �ָ��������������ռ��һ��GGTView �ĵط�
	RectScreen.InflateRect(-5, -5);
	if ( !RectScreen.PtInRect(pt) )
	{
		return false;
	}

	if ( m_bEmptyGGTongView )
	{
		return true;
	}

	//
	CIoViewBase* pIoView = m_IoViewManager.GetActiveIoView();
	if ( NULL == pIoView )
	{
		return false;
	}

	//
	const T_IoViewObject* pObj = CIoViewManager::FindIoViewObjectByIoViewPtr(pIoView);
	if ( NULL == pObj )
	{
		return false;
	}

	//
	if ( !pObj->m_bAllowDragAway )
	{
		return false;
	}

	if ( !pIoView->BeValidDragArea(pt) )
	{
		return false;
	}

	return true;
}

void CGGTongView::ClearDragMerchFlags()
{
	//
	m_bDragMerchLBDown = false;
	m_bDragMerch = false;
	m_pGGTViewDragBegin = NULL;

	HCURSOR hCurNow = ::GetCursor();
	HCURSOR hArrow = ::LoadCursor(NULL, IDC_ARROW);

	//
	if ( hCurNow != hArrow )
	{		
		::SetCursor(hArrow);
	}
}

bool32 CGGTongView::OnDragMerchEnd()
{
	//
	CGGTongView* pTmpDragBegin = m_pGGTViewDragBegin;

	//
	ClearDragMerchFlags();

	//
	if ( this == pTmpDragBegin || NULL == pTmpDragBegin )
	{
		return false;
	}

	CRect RectClient;
	GetClientRect(RectClient);
	
	// ���óɶ�����ͼ
//	m_IoViewManager.SetGroupID(0);	

	//
	CIoViewBase* pIoView = pTmpDragBegin->m_IoViewManager.GetActiveIoView();
	if ( NULL == pIoView || NULL == pIoView->GetDragMerch() )
	{
		// ��ʼ���Ǹ��յ���ͼ, �ѵ�ǰ��Ҳ���
		m_IoViewManager.RemoveAllIoView();
		RedrawWindow();
		return true;
	}

	//
	CMerch* pMerchDragBegin = pIoView->GetDragMerch();
	
	if ( m_bEmptyGGTongView )
	{
		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();

		//
		CIoViewBase* pIoViewNew = pMainFrame->CreateIoViewByPicMenuID(ID_PIC_KLINE, true, &m_IoViewManager);

		if ( NULL != pIoViewNew )
		{
			pIoViewNew->OnVDataMerchChanged(pMerchDragBegin->m_MerchInfo.m_iMarketId, pMerchDragBegin->m_MerchInfo.m_StrMerchCode, pMerchDragBegin);
			pIoViewNew->OnVDataForceUpdate();
		}
	}
	else
	{
		//
		for ( int32 i = 0; i < m_IoViewManager.m_IoViewsPtr.GetSize(); i++ )
		{
			CIoViewBase* pIoViewBase = m_IoViewManager.m_IoViewsPtr.GetAt(i);
			
			if ( NULL == pIoViewBase )
			{
				continue;
			}
			
			//
			pIoViewBase->OnVDataMerchChanged(pMerchDragBegin->m_MerchInfo.m_iMarketId, pMerchDragBegin->m_MerchInfo.m_StrMerchCode, pMerchDragBegin);
			pIoViewBase->OnVDataForceUpdate();
		}
	}

	return false;
}