#include "stdafx.h"
#include "IoViewManager.h"
#include "IoViewBase.h"
#include "IoViewExpTrend.h"
#include "GGTongView.h"
#include "MPIChildFrame.h"
#include "memdc.h"
//#include "OfflineDataManager.h"
#include "IoViewShare.h"
#include "dlgf10.h"
#include "DlgAddToBlock.h"
#include "DlgRemoveFromBlock.h"
#include "MarkManager.h"
#include "DlgMarkText.h"
#include "DlgMarkManager.h"
#include "DlgRecentMerch.h"
#include "DlgZhongCangChiGu.h"
#include "IoViewStarry.h"
#include "CCodeFile.h"
#include "IoViewDuoGuTongLie.h"
#include "IoViewTick.h"
#include "dlgalarmmodify.h"

#include <map>
using std::map;

//#pragma optimize("g", off)	// necessary in VC 6.0 to suppress (release mode)
							//    "fatal error C1001: INTERNAL COMPILER ERROR"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//
CIoViewBase::E_WeightType CIoViewBase::m_eWeightType = CIoViewBase::EWTNone;
CIoViewBase* CIoViewBase::m_pIoViewMerchChangeSrc	 = NULL;

CIoViewBase::MerchArray	CIoViewBase::sm_aBackMerchs;
CIoViewBase::MerchArray	CIoViewBase::sm_aPrevMerchs;
bool32		CIoViewBase::sm_bInBackMerchChanging = false;
//

const int32 KTimerIdCheckWindowShow				= 4625;		// ��ʱ���������/��ʾ״̬�л� - �÷����ڴ��Ľ�
const int32 KTimerPeriodCheckWindowShow			= 100;

// �������ص�xml�����������Ϣ
static const char * KStrElementValue			= "IoView";
static const char * KStrElementAttrIoViewType	= "IoViewType";

static const char * KStrElementAttrMarketId		= "MakretId";
static const char * KStrElementAttrMerchCode	= "MerchCode";
static const char * KStrElementAttrShowTabName	= "TABNAME";

const char* KIoViewKLineWeightType				= "WeightType";
//

// ���ܰ�ť
static const T_FuctionButton s_astFuctionButtons[] =
{	
	T_FuctionButton( L"F7", L"ȫ��/��ԭ��ͼ",	IOVIEW_TITLE_F7_BUTTON_ID, IMAGEID_NOUSEIMAGE, IMAGEID_NOUSEIMAGE, IMAGEID_NOUSEIMAGE),
//	T_FuctionButton( L"+",  L"������ͼ",		IOVIEW_TITLE_ADD_BUTTON_ID, IMAGEID_NOUSEIMAGE, IMAGEID_NOUSEIMAGE, IMAGEID_NOUSEIMAGE),
	T_FuctionButton( L"��", L"ɾ����ǰ��ͼ",	IOVIEW_TITLE_DEL_BUTTON_ID, IMAGEID_NOUSEIMAGE, IMAGEID_NOUSEIMAGE, IMAGEID_NOUSEIMAGE),
};

// ���ܰ�ť����
static const int KiFuctionButtonNums = sizeof(s_astFuctionButtons)/sizeof(T_FuctionButton);

///////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CIoViewBase, CControlBase)

///////////////////////////////////////////////////////////////////////////////
// message map
BEGIN_MESSAGE_MAP(CIoViewBase, CControlBase)
	//{{AFX_MSG_MAP(CIoViewBase)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_COMMAND_RANGE(ID_PIC_BEGIN, ID_PIC_END, OnMainFrameMenu )
	ON_COMMAND_RANGE(IDM_BLOCKMENU_BEGIN,IDM_BLOCKMENU_END,OnMainFrameMenu)  
	ON_COMMAND_RANGE(IDM_IOVIEWBASE_BEGIN, IDM_IOVIEWBASE_END, OnMainFrameMenu)
	ON_COMMAND_RANGE(IDC_CHART_ADD2BLOCK_BEGIN, IDC_CHART_ADD2BLOCK_END, OnMenu)
	ON_COMMAND_RANGE(ID_BLOCK_ADDTOFIRST, ID_BLOCK_ADDTO, OnBlockMenu)
	ON_COMMAND_RANGE(ID_MARK_CANCEL, ID_MARK_MANAGER, OnMenu)
	ON_COMMAND_RANGE(ID_ZHONGCANGCHIGU, ID_ZHONGCANGCHIGU, OnMenu)
	ON_COMMAND_RANGE(IDM_SETUPALARM, IDM_SETUPALARM, OnMenu)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewBase::CIoViewBase()
:m_pAbsCenterManager(NULL),
m_pIoViewManager(NULL)
{
	m_pParent			= NULL;
	m_rectClient		= CRect(-1,-1,-1,-1);
	m_pMerchXml			= NULL;

	//memset(m_aIoViewFont,  '0', sizeof(m_aIoViewFont)); // ���캯��
	memset(m_aIoViewColor, 0, sizeof(m_aIoViewColor));

	m_aIoViewFontSave.RemoveAll();
	m_aIoViewColorSave.RemoveAll();
	m_aSmartAttendMerchs.RemoveAll();
	
	m_bDelByBiSplitTrack = false;
	m_bLockRedraw		 = false;

	m_pDelayPageUpDown	 = NULL;

	m_bActive			 = false;
	m_iActiveXpos		 = 2;
	m_iActiveYpos		 = 2;

	// ��ͼ������������
	m_ColorActive		 = CIoViewBase::GetIoViewColor(ESCBackground);/*RGB(255, 255, 0);*/

	m_bHasIgnoreLastViewDataReq = false;

	m_bFirstRequestData  = true;

	m_StrTabShowName = _T("");

	m_bShowNow = true;
}

///////////////////////////////////////////////////////////////////////////////
// 
CIoViewBase::~CIoViewBase()
{
	// ��ViewDataע���Լ�
	if (NULL != m_pAbsCenterManager)
		m_pAbsCenterManager->RemoveViewListener(this);

	DEL(m_pDelayPageUpDown);


	CMainFrame*  pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if(pMainFrame)
		pMainFrame->RemoveIoView(this);

	// ������ֵ
	if( this == m_pIoViewMerchChangeSrc )
	{
		m_pIoViewMerchChangeSrc = NULL;
	}
}

void CIoViewBase::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CStatic::OnPaint() for painting messages
	CRect rect;
	GetClientRect(rect);

	CClientDC dc1(this);
	CMemDC memDC(&dc1, &rect);
	
	if ( m_bDelByBiSplitTrack )
	{
		memDC.FillSolidRect(&rect, RGB(0, 0, 255));
	}
	else
	{
		memDC.FillSolidRect(&rect, 0x000000);
	}
}

void CIoViewBase::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType,cx,cy);
	GetClientRect(m_rectClient);
}

bool32 CIoViewBase::GetParentGGTongViewDragFlag()
{
	CGGTongView* pView = GetParentGGtongView();

	if ( NULL == pView )
	{
		return false;
	}

	return pView->GetDragFlag();
}

int CIoViewBase::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;
	//
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	m_iGuid = pMainFrame->GetRandomNum();
	
	
	//
	int32 aiDelayKeys[] = { VK_PRIOR, VK_NEXT};
	m_pDelayPageUpDown = new CDelayKeyDown();
	ASSERT(NULL != m_pDelayPageUpDown);
	m_pDelayPageUpDown->InitialKeyDelayValue(aiDelayKeys, sizeof(aiDelayKeys) / sizeof(int32));


	SetTimer(KTimerIdCheckWindowShow, KTimerPeriodCheckWindowShow, NULL);

	return 0; 
}

BOOL CIoViewBase::PreTranslateMessage(MSG* pMsg) 
{
	if ( WM_RBUTTONDOWN == pMsg->message )
	{	
		if ( GetStdMenuEnable(pMsg))
		{
			DoShowStdPopupMenu();			
			return TRUE;
		}				
	}
	else
	{
		if (WM_KEYDOWN == pMsg->message || WM_SYSKEYDOWN == pMsg->message)
		{
			if ( VK_UP == pMsg->wParam && IsCtrlPressed())
			{
				// �Ŵ�����:
				ChangeFontByUpDownKey(true);
				return TRUE;
			}
			else if ( VK_DOWN == pMsg->wParam && IsCtrlPressed())
			{
				// ��С����:
				ChangeFontByUpDownKey(false);
				return TRUE;
			}
			else if ( VK_PRIOR == pMsg->wParam && EIVT_SingleMerch == GetIoViewType() )	// �ǵ����ڲ��ʺϴ��л���Ʒ
			{
				if ( m_pDelayPageUpDown->TestKeyDelay(VK_PRIOR) )
				{
					return FALSE;
				}

				CString StrBlockName;

				// 			
				if (NULL != m_pMerchXml)
				{
					CMerch *pMerch = m_pMerchXml;

					if ( this->IsKindOf(RUNTIME_CLASS(CIoViewExpTrend)) )
					{
						CIoViewExpTrend* pIoView = (CIoViewExpTrend*)this;
						pMerch = pIoView->m_pMerchReal;
						
						if ( NULL == pMerch )
						{
							pMerch = m_pMerchXml;
						}
					}
					
					//
					CIoViewBase* pIoViewMerchSrc = m_pIoViewMerchChangeSrc;
					if ( NULL == pIoViewMerchSrc )
					{
						pIoViewMerchSrc = this;
					}
				
					CMerch* pMerchPre = pIoViewMerchSrc->GetNextMerch(pMerch, true);
					if ( NULL != pMerchPre )
					{
						CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
						pFrame->OnViewMerchChangeBegin(this, pMerchPre);
						return TRUE;
					}
				}
			}
			else if ( VK_NEXT == pMsg->wParam && EIVT_SingleMerch == GetIoViewType() )	// �ǵ����ڲ��ʺϴ��л���Ʒ
			{
				if ( m_pDelayPageUpDown->TestKeyDelay(VK_NEXT) )
				{
					return FALSE;
				}

				CString StrBlockName;

				// 
				if (NULL != m_pMerchXml)
				{
					CMerch *pMerch = m_pMerchXml;
				
					if ( this->IsKindOf(RUNTIME_CLASS(CIoViewExpTrend)) )
					{
						CIoViewExpTrend* pIoView = (CIoViewExpTrend*)this;
						pMerch = pIoView->m_pMerchReal;
						
						if ( NULL == pMerch )
						{
							pMerch = m_pMerchXml;
						}
					}

					//
					CIoViewBase* pIoViewMerchSrc = m_pIoViewMerchChangeSrc;
					if ( NULL == pIoViewMerchSrc )
					{
						pIoViewMerchSrc = this;
					}
					
					CMerch* pMerchNext = pIoViewMerchSrc->GetNextMerch(pMerch, false);
					if ( NULL != pMerchNext )
					{
						CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
						pFrame->OnViewMerchChangeBegin(this, pMerchNext);
						return TRUE;
					}
				}
			}
			else if ( VK_ESCAPE == pMsg->wParam)
			{
				// ����� F7 �Ŵ���,���ȴ���
 				CGGTongView* pGGTongView = GetParentGGtongView();
				if ( NULL == pGGTongView )
				{
					return CControlBase::PreTranslateMessage(pMsg);
				}
 				
				if ( NULL != pGGTongView && pGGTongView->GetMaxF7Flag() )
 				{
 					CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
 					if (pMainFrame->OnProcessF7())
 					{
						return TRUE;
 					}
				}

				// �Ժ���������ESC �¼�
				return DealEscKey();								
			}
			else if ( 'Z' == pMsg->wParam )	// ��ѡ��CTRL+Z & CTRL+SHIFT+Z
			{
				bool32 bCtrl = IsCtrlPressed();
				bool32 bIsShift = IsShiftPressed();
				bool32 bIsAlt  = (pMsg->lParam & (1<<29)) != 0;
				CMerch *pMerch = GetMerchXml();
				if ( bCtrl && bIsShift && NULL != pMerch )
				{
					// ɾ����ѡ�� 
					PostMessage(WM_COMMAND, ID_BLOCK_REMOVEFROM, 0);
					return TRUE;
				}
				else if ( bCtrl && NULL != pMerch )
				{
					PostMessage(WM_COMMAND, ID_BLOCK_ADDTO, 0);
					return TRUE;
				}
				else if ( bIsAlt && NULL != pMerch )
				{
					// ������ѡ�� - �ӵ�һ���м���
					PostMessage(WM_COMMAND, ID_BLOCK_ADDTOFIRST, 0);
					return TRUE;
				}
				// continue
			}
			else if ( 'D' == pMsg->wParam && ((pMsg->lParam & (1<<29)) != 0) )
			{
				// ALT+D ɾ��
				CMerch *pMerch = GetMerchXml();
				if ( NULL != pMerch )
				{
					PostMessage(WM_COMMAND, ID_BLOCK_REMOVEFROM, 0);
					return TRUE;
				}
			}
			else if ( 'R' == pMsg->wParam && IsCtrlPressed() )
			{
				// ������� CTRL + R
				::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, IDM_BELONG_BLOCK, 0);
				return TRUE;
			}
			else if ( 'Q' == pMsg->wParam && IsCtrlPressed() )
			{
				// ���ֱ��
				PostMessage(WM_COMMAND, ID_MARK_TEXT, 0);
				return TRUE;
			}
			else if ( VK_BACK == pMsg->wParam && EIVT_SingleMerch == GetIoViewType() )	// �ǵ����ڲ��ʺϷ��������Ʒ
			{
				CMerch *pMerch = PeekLastBackMerch(true);
				if ( NULL != pMerch )
				{
					sm_bInBackMerchChanging = true;
					CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
					pFrame->OnViewMerchChangeBegin(this, pMerch);
					return TRUE;
				}
			}
// 			if ( VK_F6 == pMsg->wParam )
// 			{
// 				static sb = FALSE;
// 				if ( !sb )
// 				{
// 					SetTimer(510, 100, NULL);
// 					sb = !sb;
// 				}
// 				else
// 				{
// 					KillTimer(510);
// 					sb = !sb;
// 				}
// 				return TRUE;
// 			}
		}
		else if ( WM_KEYUP == pMsg->message|| WM_SYSKEYUP == pMsg->message )
		{
			if ( EIVT_SingleMerch == GetIoViewType() )	// �ǵ����ڲ��ʺϴ��л���Ʒ
			{
				CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
				if ( VK_NEXT == pMsg->wParam || VK_PRIOR == pMsg->wParam )
				{
					
					pFrame->OnViewMerchChangeEnd(this, m_pMerchXml);				
				}
				else if ( VK_BACK == pMsg->wParam && sm_bInBackMerchChanging )
				{
					sm_bInBackMerchChanging = false;
					pFrame->OnViewMerchChangeEnd(this, m_pMerchXml);
					return TRUE;
				}
			}
		}
	}

	return CControlBase::PreTranslateMessage(pMsg);
}

void CIoViewBase::SetCenterManager(CAbsCenterManager *pAbsCenterManager)
{
	if (pAbsCenterManager != m_pAbsCenterManager)
	{
		// ��ViewDataע���Լ�
		if (NULL != m_pAbsCenterManager)
		{
			m_pAbsCenterManager->RemoveViewListener(this);
		}
	}

	m_pAbsCenterManager = pAbsCenterManager;
	
	// ��ViewDataע���Լ�
	if (NULL != m_pAbsCenterManager)
	{
		m_pAbsCenterManager->AddViewListener(this);
	}

	CMainFrame*  pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->AddIoView(this);
}

int32 CIoViewBase::GetIoViewGroupId()
{
	if (NULL != m_pIoViewManager && ::IsWindow(m_pIoViewManager->GetSafeHwnd()))
		return m_pIoViewManager->GetGroupID();
	
	return 0x10000000;                      // ���뱣֤���Ѿ����ڵ�GroupID & ����ֵΪFALSE
}

int32 CIoViewBase::GetGuid()
{
	return m_iGuid;
}

void CIoViewBase::SetBiSplitTrackDelFlag(bool32 bDel)
{
	bool32 bDelByBiSplitTrackBk = m_bDelByBiSplitTrack;
	m_bDelByBiSplitTrack = bDel;
	
	if ( bDelByBiSplitTrackBk && !m_bDelByBiSplitTrack )
	{
		// ˢ����ͼ
		OnVDataForceUpdate();
	}
}

CGGTongView * CIoViewBase::GetParentGGtongView()
{
	CGGTongView * pGGtongView = NULL;
	CWnd * pParent = GetParent();
	
	while(pParent)
	{
		if ( pParent->IsKindOf(RUNTIME_CLASS(CGGTongView)))
		{
			pGGtongView = (CGGTongView *)pParent;
			break;
		}
		pParent = pParent->GetParent();
	}
	
	return pGGtongView;
}

void CIoViewBase::BringToTop(CString StrBlockName/* = L""*/)
{
	CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( pParentFrame == NULL || !pParentFrame->IsKindOf(RUNTIME_CLASS(CMPIChildFrame)) )
	{
		return;	// ��frame�������棬����������Щ����
	}

	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if ( NULL == pMainFrame )
	{
		return;
	}

	// �޸�����: ���л�������Ҫ�Ľ��棬Ȼ���ڼ���ô���
	
	// ���ǲ���������Tab ����

	CWnd * pParent = GetParent();
	while(pParent)
	{
		if (pParent->IsKindOf(RUNTIME_CLASS(CTabSplitWnd)))
		{
			CTabSplitWnd * pTab = (CTabSplitWnd*)pParent;
			pTab->SetPageByIoView(this);
			break;
		}
		pParent = pParent->GetParent();
	}

	CGGTongView * pView = GetParentGGtongView();
	// xl 0607 �ҵ�������GGTongView�����F7�ˣ����뵱ǰactiveIoViewͬframe/tabWnd��������Ȼ�ԭF7
	CGGTongView	*pF7View = (CGGTongView	*)pParentFrame->GetF7GGTongView();
	if ( NULL != pF7View
		&& pF7View != pView								// ������ͬһ��GGTongView   // F7�����
		)
	{
		bool32 bNeedF7Restore = true;
		
		if ( bNeedF7Restore )
		{
			pMainFrame->OnProcessF7(pF7View);
		}
	}
	
	//
	if ( NULL != m_pIoViewManager )
	{
		int32 iIndex = -1;
		for ( int32 i = 0 ; i< m_pIoViewManager->m_IoViewsPtr.GetSize(); i++)
		{
			if ( this == m_pIoViewManager->m_IoViewsPtr[i])
			{
				iIndex = i;
			}
		}
		
		if ( -1 == iIndex || iIndex >= m_pIoViewManager->m_IoViewsPtr.GetSize())
		{
			return;
		}
		else 
		{
			m_pIoViewManager->m_GuiTabWnd.SetCurtab(iIndex);	
			m_pIoViewManager->OnTabSelChange();
		}
	}

    if ( this->IsKindOf(RUNTIME_CLASS(CIoViewReport)) )
	{   
		CIoViewReport* pReport = (CIoViewReport*)this;
		if ( StrBlockName.GetLength() > 0 )
		{
		// ����Ǳ��۱�,Ҫ���ǰ�����Ϣ
		bool32 bFind = pReport->SetTabByBlockName(StrBlockName);

		if (!bFind)
		{
			// ����������ɾ����,��ô��ʾ��һ��:
			pReport->GetTabWnd()->SetCurtab(0);
		}
		}
		//linhc 20100918���Ĭ��ѡ��ǰ����Ʒ
		CMerch *pMerch = pReport->GetMerchXml();
		if( NULL != pMerch )
		{
            pReport->SetGridSelByMerch(pMerch);
		}	
	}

	// �����Ӵ�
	pMainFrame->MDIActivate(pParentFrame);

	if (NULL != pView)
	{
		if ( pParentFrame->IsIconic() )
		{
			pParentFrame->ShowWindow(SW_SHOWNA);
		}
		pParentFrame->SetActiveView(pView);
	}
}


// �߽ǵĳ��̳��Ⱥ���left,top�߽�ĸ߶�
void CIoViewBase::GetPolygon(CRect rect, CPoint ptsLeft[7] ,CPoint ptsRight[7], int iMaxLength, int iMinLength, int iCornerBorderH)
{
	// ���ϱ߽�
	ptsLeft[0].x = rect.left;
	ptsLeft[0].y = rect.top;
	ptsLeft[1].x = rect.left + iMaxLength;
	ptsLeft[1].y = rect.top;
	ptsLeft[2].x = rect.left + iMaxLength;
	ptsLeft[2].y = rect.top  + iCornerBorderH;
	ptsLeft[3].x = rect.left + iMinLength;
	ptsLeft[3].y = rect.top  + iCornerBorderH;
	ptsLeft[4].x = rect.left + iCornerBorderH;
	ptsLeft[4].y = rect.top  + iMinLength;
	ptsLeft[5].x = rect.left + iCornerBorderH;
	ptsLeft[5].y = rect.top  + iMaxLength;
	ptsLeft[6].x = rect.left;
	ptsLeft[6].y = rect.top  + iMaxLength;

	// ���ϱ߽�
	ptsRight[0].x = rect.right;
	ptsRight[0].y = rect.top;
	ptsRight[1].x = rect.right;
	ptsRight[1].y = rect.top + iMaxLength;
	ptsRight[2].x = rect.right - iCornerBorderH;
	ptsRight[2].y = rect.top + iMaxLength;
	ptsRight[3].x = rect.right - iCornerBorderH;
	ptsRight[3].y = rect.top + iMinLength ;
	ptsRight[4].x = rect.right - iMinLength;
	ptsRight[4].y = rect.top + iCornerBorderH;
	ptsRight[5].x = rect.right - iMaxLength;
	ptsRight[5].y = rect.top + iCornerBorderH;
	ptsRight[6].x = rect.right - iMaxLength;
	ptsRight[6].y = rect.top;
}


void CIoViewBase::DrawPolygonBorder(CMemDC *pMemDc, CRect rect,COLORREF clrFill /* = RGB(234, 23, 23)*/, COLORREF clrBorder /*= RGB(234, 23, 23)*/)
{
	if (pMemDc)
	{
		CPen penBlue(PS_SOLID, 1, clrBorder);
		CPen* pOldPen = pMemDc->SelectObject(&penBlue);
		CBrush brushRed(clrFill);
		CBrush* pOldBrush = pMemDc->SelectObject(&brushRed);

		CPoint ptsLeft[7];
		CPoint ptsRight[7];
		GetPolygon(rect, ptsLeft, ptsRight);

		// we specified.
		pMemDc->Polygon(ptsLeft, 7);
		pMemDc->Polygon(ptsRight, 7);

		// Put back the old objects.
		pMemDc->SelectObject(pOldPen);
		pMemDc->SelectObject(pOldBrush);
	}
}


void CIoViewBase::DrawCorner(CMemDC *pMemDc, CRect rcWindows)
{
	if (!pMemDc)
	{
		return;
	}

	CRect rcDrawRect;
	rcDrawRect= rcWindows;

	DrawPolygonBorder(pMemDc, rcDrawRect);
}


bool32 CIoViewBase::IsAttendData(IN CMerch *pMerch, E_DataServiceType eDataServiceType)
{
	for (int32 i = 0; i < m_aSmartAttendMerchs.GetSize(); i++)
	{
		CSmartAttendMerch &SmartAttendMerch = m_aSmartAttendMerchs[i];		
		if (pMerch == SmartAttendMerch.m_pMerch && (SmartAttendMerch.m_iDataServiceTypes & eDataServiceType) != 0)
		{
			return true;
		}
	}

	if ( eDataServiceType == EDSTPrice
		&& IsKindOf(RUNTIME_CLASS(CIoViewStarry)) )
	{
		// �ǿ�ͼ���ǹ������е�ʵʱ���۸���
		return true;
	}
	
	return false;
}

bool32 CIoViewBase::IsAttendNews(IN CMerch* pMerch)
{
	for (int32 i = 0; i < m_aSmartAttendMerchs.GetSize(); i++)
	{
		CSmartAttendMerch &SmartAttendMerch = m_aSmartAttendMerchs[i];		
		if (pMerch == SmartAttendMerch.m_pMerch && SmartAttendMerch.m_bNeedNews )
		{
			return true;
		}
	}
	
	return false;
}

void CIoViewBase::AppendIoViewsMenu(CNewMenu* pMenu, bool32 bLockSplit/*=false*/)
{
	// ����ͼ�˵�
	if ( NULL == pMenu )
	{
		return;
	}

	CString StrReport;

	for( int32 i = 0 ; i < CIoViewManager::GetIoViewObjectCount(); i++)
	{
		CString StrMenuName = CIoViewManager::GetIoViewObject(i)->m_StrLongName;
		
		if ( CIoViewManager::GetIoViewObject(i)->m_pIoViewClass == RUNTIME_CLASS(CIoViewReport) )
		{
			//
			CNewMenu * pNewMenu = pMenu->AppendODPopupMenu(StrMenuName);
			//CNewMenu * pNewMenu = pMenu->AppendODPopupMenu(L"�����б� ");
			StrReport = StrMenuName;
			
			if ( bLockSplit )
			{
				// ����״̬�£����������ͨ���۱�
				if ( NULL != pNewMenu )
				{
					CArray<T_BlockMenuInfo ,T_BlockMenuInfo&> aBlockMenuInfo; 
					CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
					pMainFrame->GetBlockMenuInfo(aBlockMenuInfo);
					
					CNewMenu * pMenuMerch = NULL;
					
					for (int32 i = 0; i<aBlockMenuInfo.GetSize(); i++)
					{			
						if (aBlockMenuInfo[i].iBlockMenuIDs == 0)
						{
							CString StrName = aBlockMenuInfo[i].StrBlockMenuNames;
							
							if ( NULL != pMenuMerch )
							{
								pMenuMerch->RemoveMenu(0, MF_BYPOSITION|MF_SEPARATOR);
							}
							
							pMenuMerch = pNewMenu->AppendODPopupMenu(StrName);
						}
						
						CString StrBreedName = aBlockMenuInfo[i].StrBlockMenuNames;
						
						if (StrBreedName == L"���֤ȯ" || StrBreedName == L"�¼����ڻ�" || StrBreedName == L"ȫ��ָ��")
						{
							pNewMenu->AppendODMenu(L"",MF_SEPARATOR);
						}
						
						if(pMenuMerch)
						{
							pMenuMerch->AppendODMenu(aBlockMenuInfo[i].StrBlockMenuNames,MF_STRING,aBlockMenuInfo[i].iBlockMenuIDs);					

							if ( i == (aBlockMenuInfo.GetSize() - 1))
							{
								pMenuMerch->RemoveMenu(0, MF_BYPOSITION|MF_SEPARATOR);
							}
						}
					}
				}
			}
			else
			{
				// ����������ѡ���½��ı��۱�����
				if ( NULL != pNewMenu )
				{
					const SimpleTabClassMap &simMap = CIoViewReport::GetSimpleTabClassConfig();
					for ( SimpleTabClassMap::const_iterator it=simMap.begin(); it != simMap.end() ; ++it )
					{
						CString StrItem = it->second.StrName;
						if ( StrItem.IsEmpty() )
						{
							if ( it->second.iId == SIMPLETABINFOCLASS_DEFAULTID )
							{
								StrItem = _T("Ĭ�ϱ��۱�����");
							}
							else
							{
								StrItem.Format(_T("���۱�����%d"), it->second.iId);
							}
						}

						// ���˵���Ĭ�ϡ�, Ĭ�ϵ�iId����-1
						if(it->second.iId >= 0)
						{
							pNewMenu->AppendODMenu(StrItem, MF_STRING|MF_BYCOMMAND, it->second.iId+ID_REPORT_TYPEBEGIN);
						}
					}
				}
			}
		}
		else
		{
			// ������Ѷ����ʾ
			UINT uID = CIoViewManager::GetIoViewObject(i)->m_uID;
			if ( (uID < ID_PIC_HIDE_INMENU)  && (ID_PIC_NEWS != uID))		// ֻ��С�����id�����ڲ˵�����ʾ
			{
				pMenu->AppendODMenu(StrMenuName, MF_STRING, ID_PIC_BEGIN + i);
			}
			if (ID_PIC_QR_CODE == uID)
			{
				pMenu->AppendODMenu(StrMenuName, MF_STRING, ID_PIC_QR_CODE);
			}
			if (ID_PIC_WEB_NEWS == uID)
			{
				pMenu->AppendODMenu(StrMenuName, MF_STRING, ID_PIC_WEB_NEWS);
			}
		}
	}
		
	// ���۱�������Ӳ˵�
	//pMenu->ModifyODMenu(StrReport, StrReport, IDB_TOOLBAR_REPORT);

	//
	if ( CheckFlag(pMenu->GetMenuState(0, MF_BYPOSITION), MF_SEPARATOR) )
	{
		pMenu->RemoveMenu(0, MF_BYPOSITION|MF_SEPARATOR);
	}	

	pMenu->LoadToolBar(g_awToolBarIconIDs);
}

void CIoViewBase::OnMainFrameMenu(UINT nID )
{
	AfxGetMainWnd()->PostMessage(WM_COMMAND, nID, 0);
}

const char * CIoViewBase::GetXmlElementValue()
{
	return KStrElementValue;
}

const char * CIoViewBase::GetXmlElementAttrIoViewType()
{
	return KStrElementAttrIoViewType;
}

const char * CIoViewBase::GetXmlElementAttrMarketId()
{
	return KStrElementAttrMarketId;
}

const char * CIoViewBase::GetXmlElementAttrMerchCode()
{
	return KStrElementAttrMerchCode;
}

const char * CIoViewBase::GetXmlElementAttrShowTabName()
{
	return KStrElementAttrShowTabName;
}
//////////////////////////////////////////////////////////////////////////
void CIoViewBase::InitialIoViewFace(CIoViewBase * pIoView)
{
	if ( NULL == pIoView)
	{
		return;
	}

	// ��ʼ����ͼ����ɫ.ÿ����ͼ������ʱ������������,������һ��ʼ��������Ҳ�����Ǻ����������½�һ����ͼ
	bool32 bGetData = false;

	if ( CFaceScheme::EFERSameTypeIoView == CFaceScheme::Instance()->m_eFaceEffectRange)
	{
		// �õ�������ͼ������

		T_IoViewTypeFaceObject IoViewTypeFaceObject;

		if ( CFaceScheme::Instance()->GetIoViewTypeFaceObject(pIoView,IoViewTypeFaceObject))
		{
			bGetData = true;
			
			memset(m_aIoViewColor,0,sizeof(m_aIoViewColor));
			memcpyex(m_aIoViewColor,IoViewTypeFaceObject.m_aColors,sizeof(m_aIoViewColor));

			for ( int32 i=0; i < ESFCount ; i++ )
			{
				m_aIoViewFont[i] = IoViewTypeFaceObject.m_aFonts[i];
			}
		}
	}
	else if ( CFaceScheme::EFERSameChildFrmae == CFaceScheme::Instance()->m_eFaceEffectRange)
	{
		// �õ�����Ӵ��ڵ�����

		CFrameWnd * pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, pIoView->GetParentFrame());
		T_ChildFrameFaceObject ChildFrameFaceObject;

		if ( NULL != pFrame && pFrame != AfxGetMainWnd())
		{
			if ( CFaceScheme::Instance()->GetChildFrameFaceObject((CMPIChildFrame*)pFrame,ChildFrameFaceObject) )
			{
				bGetData = true;
				
				memset(m_aIoViewColor,0,sizeof(m_aIoViewColor));
				memcpyex(m_aIoViewColor,ChildFrameFaceObject.m_aColors,sizeof(m_aIoViewColor));

				for ( int32 i=0; i < ESFCount ; i++ )
				{
					m_aIoViewFont[i] = ChildFrameFaceObject.m_aFonts[i];
				}
			}			
		}		
	}

	if ( !bGetData)
	{
		// û��ȡ������,ʹ��Ĭ��ֵ:
		memset(m_aIoViewColor,0,sizeof(m_aIoViewColor));
		memcpyex(m_aIoViewColor,CFaceScheme::Instance()->m_aSysColors,sizeof(m_aIoViewColor));
		
		// ���۱������Ĭ���ǰ�ɫ
		if ( pIoView->IsKindOf(RUNTIME_CLASS(CIoViewReport)) )
		{
			m_aIoViewColor[ESCText] = CFaceScheme::Instance()->GetSysColor(ESCAmount);
		}

		// ��ʱͼ������ͼ�ĳɽ���Ĭ���ǽ��ɫ
		if ( pIoView->IsKindOf(RUNTIME_CLASS(CIoViewTrend)) || pIoView->IsKindOf(RUNTIME_CLASS(CIoViewDuoGuTongLie)) 
			|| pIoView->IsKindOf(RUNTIME_CLASS(CIoViewTick)))
		{
			m_aIoViewColor[ESCVolume2] = RGB(170,170,0);
		}

		for ( int i=0; i < ESFCount ; i++ )
		{
			m_aIoViewFont[i] = CFaceScheme::Instance()->m_aSysFont[i];
		}
	}
}

void CIoViewBase::SetColorsFromXml(TiXmlElement * pElement)
{
	if(NULL == pElement)
	{
		return;
	}

	// �ڴ�XML �еõ��Զ��������
	for (int32 i = 0 ;i< CFaceScheme::Instance()->GetSysColorObjectCount();i ++)
	{
		CString StrAttriName  = CFaceScheme::Instance()->GetSysColorObject(i)->m_StrFileKeyName;

		char	aStrAttriName[100];
		UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
		const char * pcAttrValue = pElement->Attribute(aStrAttriName);
		if (pcAttrValue)
		{
			m_aIoViewColor[i] = StrToColorRef(pcAttrValue);
		}
	}
}

void CIoViewBase::SetFontsFromXml(TiXmlElement * pElement)
{
	if (NULL == pElement)
	{
		return;
	}
	// ��������
	// NormalFontName
	CString StrAttriName = CFaceScheme::Instance()->GetKeyNameNormalFontName();
	
	char	aStrAttriName[100];
	UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
	
	const char * pcAttrValue = pElement->Attribute(aStrAttriName);
	
	
	if (NULL != pcAttrValue)
	{
		// ��XML �ж�ȡ����������
		LOGFONT NormalFont;
		memset(&NormalFont, 0, sizeof(NormalFont));
		_tcscpy(NormalFont.lfFaceName,_A2W(pcAttrValue));
		LONG KeyValue[4];
		for (int32 i =4; i<8; i++)
		{
			StrAttriName = CFaceScheme::Instance()->GetSysDefaultFontObject(i)->m_StrFileKeyName;
			
			memset(aStrAttriName,0,sizeof(aStrAttriName));
			UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
			pcAttrValue = pElement->Attribute(aStrAttriName);
			// ���ļ�����ʱ,��CFaceScheme �л�ȡĬ��ֵ
			KeyValue[i-4] = (pcAttrValue == NULL)?(CFaceScheme::Instance()->GetSysDefaultFontObject(i)->m_iKeyValue):atol(pcAttrValue);			
		}
		NormalFont.lfHeight			= KeyValue[0];
		NormalFont.lfWeight			= KeyValue[1];
		NormalFont.lfCharSet		= KeyValue[2];
		NormalFont.lfOutPrecision	= KeyValue[3];
		
		m_aIoViewFont[ESFNormal].SetFont(&NormalFont);	
	}
	
	StrAttriName = CFaceScheme::Instance()->GetKeyNameSmallFontName();
	
	memset(aStrAttriName,0,sizeof(aStrAttriName));
	UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
	pcAttrValue = pElement->Attribute(aStrAttriName);
	if (NULL != pcAttrValue)
	{
		// ��XML �ж�ȡС��������
		LOGFONT SmallFont;
		memset(&SmallFont, 0, sizeof(SmallFont));
		_tcscpy(SmallFont.lfFaceName,_A2W(pcAttrValue));
		LONG KeyValue[4];
		for (int32 i =8; i<12; i++)
		{
			StrAttriName = CFaceScheme::Instance()->GetSysDefaultFontObject(i)->m_StrFileKeyName;
			
			memset(aStrAttriName,0,sizeof(aStrAttriName));
			UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
			pcAttrValue = pElement->Attribute(aStrAttriName);
			// ���ļ�����ʱ,��CFaceScheme �л�ȡĬ��ֵ
			KeyValue[i-8] = (pcAttrValue == NULL)?(CFaceScheme::Instance()->GetSysDefaultFontObject(i)->m_iKeyValue):atol(pcAttrValue);			
		}
		SmallFont.lfHeight			= KeyValue[0];
		SmallFont.lfWeight			= KeyValue[1];
		SmallFont.lfCharSet			= KeyValue[2];
		SmallFont.lfOutPrecision	= KeyValue[3];
		
		m_aIoViewFont[ESFSmall].SetFont(&SmallFont);
	}
	
	StrAttriName = CFaceScheme::Instance()->GetKeyNameBigFontName();
	
	memset(aStrAttriName,0,sizeof(aStrAttriName));
	UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
	pcAttrValue = pElement->Attribute(aStrAttriName);
	if (NULL != pcAttrValue)
	{
		// ��XML �ж�ȡ����������
		LOGFONT BigFont;
		memset(&BigFont, 0, sizeof(BigFont));
		_tcscpy(BigFont.lfFaceName,_A2W(pcAttrValue));
		LONG KeyValue[4];
		for (int32 i =0; i<4; i++)
		{
			StrAttriName = CFaceScheme::Instance()->GetSysDefaultFontObject(i)->m_StrFileKeyName;
			
			memset(aStrAttriName,0,sizeof(aStrAttriName));
			UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
			pcAttrValue = pElement->Attribute(aStrAttriName);
			// ���ļ�����ʱ,��CFaceScheme �л�ȡĬ��ֵ
			KeyValue[i] = (pcAttrValue == NULL)?(CFaceScheme::Instance()->GetSysDefaultFontObject(i)->m_iKeyValue):atol(pcAttrValue);			
		}
		BigFont.lfHeight		= KeyValue[0];
		BigFont.lfWeight		= KeyValue[1];
		BigFont.lfCharSet		= KeyValue[2];
		BigFont.lfOutPrecision	= KeyValue[3];
		
		m_aIoViewFont[ESFBig].SetFont(&BigFont);		
	}

	StrAttriName = CFaceScheme::Instance()->GetKeyNameTextFontName();

	memset(aStrAttriName,0,sizeof(aStrAttriName));
	UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
	pcAttrValue = pElement->Attribute(aStrAttriName);
	if (NULL != pcAttrValue)
	{
		// ��XML �ж�ȡ����������
		LOGFONT TextFont;
		memset(&TextFont, 0, sizeof(TextFont));
		_tcscpy(TextFont.lfFaceName,_A2W(pcAttrValue));
		LONG KeyValue[4];
		for(int i =12; i < 15; ++i)
		{
			StrAttriName = CFaceScheme::Instance()->GetSysDefaultFontObject(i)->m_StrFileKeyName;

			memset(aStrAttriName,0,sizeof(aStrAttriName));
			UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
			pcAttrValue = pElement->Attribute(aStrAttriName);
			// ���ļ�����ʱ,��CFaceScheme �л�ȡĬ��ֵ
			KeyValue[i - 12] = (pcAttrValue == NULL)?(CFaceScheme::Instance()->GetSysDefaultFontObject(i)->m_iKeyValue):atol(pcAttrValue);			
		}
		TextFont.lfHeight		= KeyValue[0];
		TextFont.lfWeight		= KeyValue[1];
		TextFont.lfCharSet		= KeyValue[2];
		TextFont.lfOutPrecision	= KeyValue[3];

		m_aIoViewFont[ESFText].SetFont(&TextFont);		
	}
}

CString CIoViewBase::SaveColorsToXml()
{
	// ǰ����ܾ����˺ܶ������,ֻ��������Ĭ�ϵ�ϵͳ��ɫ�Ƚ�,ȷ����Ҫ���浽XML����ɫ
	m_aIoViewColorSave.RemoveAll();	
	
	int32 i = 0;
	for ( i = 0 ; i < CFaceScheme::Instance()->GetSysColorObjectCount(); i++)
	{
		E_SysColor eSysColor = (E_SysColor)i;
		if ( m_aIoViewColor[i] != CFaceScheme::Instance()->GetSysColor(eSysColor) )
		{
			// ����
			T_SysColorSave ColorSave;
			
			ColorSave.m_Color			= m_aIoViewColor[i];
			ColorSave.m_StrFileKeyName	= CFaceScheme::GetSysColorObject(i)->m_StrFileKeyName;
			
			m_aIoViewColorSave.Add(ColorSave);
		}		
	}

	CString StrThis;

	// ��ɫ
	CString StrColor = L"";
	
	for ( i= 0 ; i < m_aIoViewColorSave.GetSize(); i++)
	{
		CString StrColorTmp;
		StrColorTmp = ColorRefToStr(m_aIoViewColorSave[i].m_Color);
		
		CString StrKey;		
		StrKey.Format(L" %s=\"%s\" ",m_aIoViewColorSave[i].m_StrFileKeyName.GetBuffer(), StrColorTmp.GetBuffer());	
		StrColor += StrKey;
	}
	
	StrThis += StrColor;

	return StrThis;
}

CString CIoViewBase::SaveFontsToXml()
{
	// ǰ����ܾ����˺ܶ������,ֻ��������Ĭ�ϵ�ϵͳ����Ƚ�,ȷ����Ҫ���浽XML������
	m_aIoViewFontSave.RemoveAll();

	int32 i = 0;
	for ( i = 0 ; i < ESFCount; i++)
	{
		E_SysFont eSysFont = (E_SysFont)i;

		// LOGFONT�����仯����Ϊ�����б仯(ʵ���п���facename������Ч�ַ��仯- -)
		if ( 0 != memcmp(&m_aIoViewFont[i].m_stLogfont, CFaceScheme::Instance()->GetSysFont(eSysFont), sizeof(LOGFONT)) )
		{
			// ����
			T_SysFontSave FontSave;
			FontSave.m_eSysFont		= E_SysFont(i);
			FontSave.m_lfLogFont	= m_aIoViewFont[i].m_stLogfont;
			
			m_aIoViewFontSave.Add(FontSave);
		}
	}

	CString StrThis;

	// ����
	CString StrFont = L"";
	CString StrFontName,StrHeight,StrWeight,StrCharSet,StrOutPrecision;	
	
	for ( i = 0 ; i< m_aIoViewFontSave.GetSize();i++)
	{					
		CString StrKey;
		
		CFaceScheme::Instance()->SetKeyNames(m_aIoViewFontSave[i].m_eSysFont,StrFontName,StrHeight,StrWeight,StrCharSet,StrOutPrecision);
		
		StrKey.Format(L" %s=\"%s\" %s=\"%i\" %s=\"%i\" %s=\"%i\" %s=\"%i\"",
			StrFontName.GetBuffer(),m_aIoViewFontSave[i].m_lfLogFont.lfFaceName,
			StrHeight.GetBuffer(),m_aIoViewFontSave[i].m_lfLogFont.lfHeight,		
			StrWeight.GetBuffer(),m_aIoViewFontSave[i].m_lfLogFont.lfWeight,
			StrCharSet.GetBuffer(),m_aIoViewFontSave[i].m_lfLogFont.lfCharSet,
			StrOutPrecision.GetBuffer(),m_aIoViewFontSave[i].m_lfLogFont.lfOutPrecision);
		StrFont += StrKey;
	}				
	
	StrThis += StrFont;

	return StrThis;
}

//////////////////////////////////////////////////////////////////////////
// �������


COLORREF CIoViewBase::GetIoViewColor(E_SysColor eSysColor)
{
	ASSERT(eSysColor< ESCCount);
	return m_aIoViewColor[eSysColor];
}

LOGFONT* CIoViewBase::GetIoViewFont(E_SysFont eSysFont)
{
	if (/*eSysFont < 0 ||*/ eSysFont >= ESFCount)
		return &m_aIoViewFont[ESFNormal].m_stLogfont;

	return &m_aIoViewFont[eSysFont].m_stLogfont;
}

//	�б�ͷ
LOGFONT*		CIoViewBase::GetColumnExLF(bool bBold /*= false*/)
{
	return &m_aIoViewFont[ESFSmall].m_stLogfont;	
}
//	����������
LOGFONT*		CIoViewBase::GetChineseRowExLF(bool bBold /*= false*/)
{
	return &m_aIoViewFont[ESFSmall].m_stLogfont;
}
//	�������
LOGFONT*		CIoViewBase::GetIndexRowExLF(bool bBold/*= false*/)
{
	return &m_aIoViewFont[ESFText].m_stLogfont;
}
//	����������
LOGFONT*		CIoViewBase::GetDigitRowExLF(bool bBold/*= false*/)
{
	return &m_aIoViewFont[ESFNormal].m_stLogfont;	
}
//	��Ʒ����������
LOGFONT*		CIoViewBase::GetCodeRowExLF(bool bBold/*= false*/)
{
	return &m_aIoViewFont[ESFNormal].m_stLogfont;	
}
CFont* CIoViewBase::SetIoViewFontObject(E_SysFont eSysFont, LOGFONT& lf)
{
	if (/*eSysFont < 0 ||*/ eSysFont >= ESFCount)
	{			
		eSysFont = ESFNormal;
	}
	CFont* pFont = GetIoViewFontObject(eSysFont);
	if (NULL == pFont)
	{
		return NULL;
	}

	if (NULL != pFont->m_hObject)
	{
		pFont->DeleteObject();
	}
	m_aIoViewFont[eSysFont].SetFont(&lf);
	pFont->CreateFontIndirect(&lf);
	return pFont;
}
CFont* CIoViewBase::GetIoViewFontObject(E_SysFont eSysFont)
{
	if (/*eSysFont < 0 ||*/ eSysFont >= ESFCount)
	{			
		eSysFont = ESFNormal;
	}
	
	// �����ǵ��̣߳�so����static
	// ÿ��IoViewBase���п����в�ͬ�����壬��ҵľ���ÿ�ζ�Ҫ�ؽ�
	static CFont sFonts[ESFCount];
	CFont &sFont = sFonts[eSysFont];
	
	LOGFONT logFont = {0};
	
	const LOGFONT *pNowLogFont = NULL;
	pNowLogFont = &m_aIoViewFont[eSysFont].m_stLogfont;
	
	if ( NULL != sFont.m_hObject && sFont.GetLogFont(&logFont) != 0 /*&& memcmp(pNowLogFont, &logFont, sizeof(LOGFONT)) == 0 */
		&& (_tcscmp(pNowLogFont->lfFaceName, logFont.lfFaceName) == 0
		&& pNowLogFont->lfHeight == logFont.lfHeight
		&& pNowLogFont->lfWeight == logFont.lfWeight
		&& pNowLogFont->lfCharSet == logFont.lfCharSet
		&& pNowLogFont->lfOutPrecision == logFont.lfOrientation))
	{
		// ��ͬ�򲻴���
		return &sFont;
	}
	else
	{
		sFont.DeleteObject();
		CFaceScheme::CreateFontIndirect(sFont, pNowLogFont);
		return &sFont;
	}
}

void CIoViewBase::SetIoViewFontArray(CFontNode (&aSysFont)[ESFCount])
{			
	// ʹ�ø�ֵ
	for ( int32 i=0; i < ESFCount ; i++ )
	{
		m_aIoViewFont[i] = aSysFont[i];
	}
}

void CIoViewBase::SetIoViewFontArraySave(CArray<T_SysFontSave,T_SysFontSave>& aSysFontSave)
{
	m_aIoViewFontSave.RemoveAll();
	m_aIoViewFontSave.Copy(aSysFontSave);
}

void CIoViewBase::SetIoViewColorArray(COLORREF (&aSysColor)[ESCCount])
{
	memset(m_aIoViewColor,0,sizeof(m_aIoViewColor));
	memcpyex(m_aIoViewColor,aSysColor,sizeof(aSysColor));
}

void CIoViewBase::SetIoViewColorArraySave(CArray<T_SysColorSave,T_SysColorSave>& aSysColorSave)
{
	m_aIoViewColorSave.RemoveAll();
	m_aIoViewColorSave.Copy(aSysColorSave);
}
//
void CIoViewBase::OnIoViewColorChanged()
{
	// ��ɫ�����޸���.��faceschema ��ȡ������,�ػ�

	CFaceScheme::Instance()->GetNowUseColors(m_aIoViewColor);
	RedrawWindow();

	/*
	COLORREF aColorTemp[ESCCount];

	CFaceScheme::Instance()->GetNowUseColors(aColorTemp);
	
	// �����������������Ƚ�,�õ���Ҫ���浽XML ����ɫ,Ҳ�������� m_aIoViewColorSave ��ֵ

	m_aIoViewColorSave.RemoveAll();	

	for ( int32 i = 0 ; i < CFaceScheme::Instance()->GetSysColorObjectCount(); i++)
	{
		if ( m_aIoViewColor[i] != aColorTemp[i] )
		{
			// ����
			T_SysColorSave ColorSave;
			
			ColorSave.m_Color			= aColorTemp[i];
			ColorSave.m_StrFileKeyName	= CFaceScheme::GetSysColorObject(i)->m_StrFileKeyName;

			m_aIoViewColorSave.Add(ColorSave);
		}		
	}

	CFaceScheme::Instance()->GetNowUseColors(m_aIoViewColor);
	Invalidate();
	*/
}

void CIoViewBase::OnIoViewFontChanged()
{
	CFaceScheme::Instance()->GetNowUseFonts(m_aIoViewFont);
	RedrawWindow();		  
}

void CIoViewBase::ChangeFontByUpDownKey(bool32 bBigger)
{

	for ( int32 i = 0 ; i < ESFCount; i++)
	{
		m_aIoViewFont[i].m_stLogfont.lfHeight = CFaceScheme::Instance()->GetFontHeight(bBigger,m_aIoViewFont[i].m_stLogfont.lfHeight);			
	}
	
	CFaceScheme::Instance()->SetNowUseFonts(m_aIoViewFont);

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->PostMessage(UM_Font_Change,(WPARAM)this,0);
}

BOOL CIoViewBase::DealEscKey(bool32 bDealCross /*= true */)
{
	if ( !this->IsKindOf(RUNTIME_CLASS(CIoViewChart)))
	{
		return DealEscKeyReturnToReport();
	}
	else
	{
		if ( bDealCross )
		{
			CIoViewChart* pChart = (CIoViewChart*)this;
			if ( pChart->m_pRegionMain->IsActiveCross() )
			{
				return FALSE;
			}
		}
		
		// ����K��������ʱ���ص��������۱�
		// ���Ӧ������ǰ��ģ�����0622ʱֻ������������ͼ���ͷ���������
		if ( !OnSpecialEsc() )
		{
			return TRUE;
		}
	}

	return	DealEscKeyReturnToReport();
}

E_ReportType CIoViewBase::GetMerchKind(IN CMerch *pMerch)
{	
	ASSERT(NULL != m_pAbsCenterManager);
	
	E_ReportType eReportType = ERTStockHk;	// Ĭ��ֵ

	if (NULL != pMerch)
	{
		eReportType = pMerch->m_Market.m_MarketInfo.m_eMarketReportType;
	}
	
	return eReportType;
}

CIoViewBase::E_WeightType CIoViewBase::GetWeightType()
{
	return m_eWeightType;
}

void CIoViewBase::SetWeightType(CIoViewBase::E_WeightType eWeightType)
{
	if ( /*eWeightType < (CIoViewBase::E_WeightType)0 || */ eWeightType >= CIoViewBase::EWTCount )
	{
		m_eWeightType = CIoViewBase::EWTNone;
	}
	
	m_eWeightType = eWeightType;
}

bool32 CIoViewBase::ReadWeightDataFromLocalFile(IN CMerch* pMerch, OUT CArray<CWeightData,CWeightData&>& aWeightData, OUT UINT& uiCrc32)
{
	// �ӱ����ļ�����Ȩ����:
	CString StrFilePath = L"";
	
	aWeightData.SetSize(0);
	uiCrc32 = 0;
	
	if ( NULL == pMerch )
	{
		return false;			 
	}

	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	if( NULL == pDoc  || !pDoc->m_pAbsDataManager)
	{
		return false;
	}	

	
	// ȡ�ļ�·��:
	if ( !pDoc->m_pAbsDataManager->GetPublishFilePath(EPFTWeight, pMerch->m_MerchInfo.m_iMarketId , pMerch->m_MerchInfo.m_StrMerchCode, StrFilePath) )
	{
		return false;
	}
	
	// ������:
	CFile file;
	if ( !file.Open(StrFilePath, CFile::modeRead | CFile::typeBinary) )
	{
		return false;
	}
	
	int32 iDataLen = file.GetLength();
	if ( iDataLen <= 0 || iDataLen > 100*1024 )
	{
		ASSERT( iDataLen == 0 );
		file.Close();
		return false;
	}
	
	//
	char *acBuffer = new char[iDataLen];
	memset(acBuffer, 0, sizeof(acBuffer));
	
	if ( 0 == file.Read(acBuffer, iDataLen) )
	{
		delete []acBuffer;
		acBuffer = NULL;
		file.Close();
		return false;
	}
	
	// �ļ���CRC32 ֵ
	uiCrc32 = CCodeFile::crc_32(acBuffer, iDataLen);
	
	// ��������
	if ( !CMerch::ReadWeightDatasFromBuffer(acBuffer, iDataLen, aWeightData) )    
	{
		delete []acBuffer;
		acBuffer = NULL;
		file.Close();
		return false;
	}
	delete []acBuffer;
	acBuffer = NULL;
	
	//
	file.Close();		
	
	return true;
}

bool32 CIoViewBase::BePassedCloseTime(CMerch* pMerch)
{
	//
	CGGTongDoc* pDoc = (CGGTongDoc*)AfxGetDocument();
	CAbsCenterManager* pAbsCenterManager = pDoc->m_pAbsCenterManager;

	if ( NULL == pMerch || NULL == pAbsCenterManager )
	{
		//ASSERT(0);
		return false;
	}

	// 
	CGmtTime TimeNow = pAbsCenterManager->GetServerTime();

	CMarketIOCTimeInfo RecentTradingDayTime;
	if ( pMerch->m_Market.GetRecentTradingDay(TimeNow, RecentTradingDayTime, pMerch->m_MerchInfo) )
	{
		CGmtTime TimeRecentClose(RecentTradingDayTime.m_TimeClose.m_Time);
		
		CGmtTime TimeNowDay = TimeNow;
		SaveDay(TimeNowDay);
		
		CGmtTime TimeRecentCloseDay = TimeRecentClose;
		SaveDay(TimeRecentCloseDay);
		
		if ( TimeNowDay != TimeRecentCloseDay )
		{
			// ���ڲ�ͬ, ֱ�ӷ���			
			return true;				
		}
		else
		{
			// ������ͬ, 
			TimeRecentClose += CGmtTimeSpan(0, 0, 1, 0);
			
			if (TimeNow >= TimeRecentClose)	
			{				
				return true;
			}	
		}		
	}

	return false;
}

BOOL CIoViewBase::DealEscKeyReturnToReport()
{
	// ����MainFrame����
	CMainFrame* pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if ( NULL != pMainFrame )
	{
		pMainFrame->OnEscBackFrame();
		return TRUE;
	}
	return FALSE;
}

void CIoViewBase::OnMenu( UINT nID )
{
	if ( nID >= IDC_CHART_ADD2BLOCK_BEGIN && nID < IDC_CHART_ADD2BLOCK_END )
	{
		CArray<T_Block, T_Block&> aBlocks;
		CUserBlockManager::Instance()->GetBlocks(aBlocks);	// ֱ�ӻ�ȡ���µ�
		int32 iIndex = nID - IDC_CHART_ADD2BLOCK_BEGIN;
		if ( iIndex >=0 && iIndex < aBlocks.GetSize() )
		{
			CString StrBlockName = aBlocks.GetAt(nID-IDC_CHART_ADD2BLOCK_BEGIN).m_StrName;
			CMerch *pMerch = GetMerchXml();
			if ( NULL == pMerch )
			{
				pMerch = m_pMerchXml;
			}
			CUserBlockManager::Instance()->AddMerchToUserBlock(pMerch, StrBlockName);
		}
		else
		{
			ASSERT( 0 );
		}
	}
	else if ( nID >= ID_MARK_CANCEL && nID <= ID_MARK_MANAGER )
	{
		// ��Ǵ���
		CMerch *pMerch = GetMerchXml();
		if (NULL == pMerch)
		{
			return;
		}
		switch ( nID )
		{
		case ID_MARK_CANCEL:
			CMarkManager::Instance().RemoveMark(pMerch);
			break;
		case ID_MARK_TEXT:
			{
				CDlgMarkText dlg;
				dlg.SetMerch(pMerch);
				dlg.DoModal();
			}
			break;
		case ID_MARK_MANAGER:
			{
				CDlgMarkManager dlg;
				dlg.DoModal();
			}
			break;
		case ID_MARK_1:
		case ID_MARK_2:
		case ID_MARK_3:
		case ID_MARK_4:
		case ID_MARK_5:
		case ID_MARK_6:
			{
				CMarkManager::Instance().SetMark(pMerch, (E_MarkType)(nID-ID_MARK_1+EMT_1));
			}
			break;
		default:
			ASSERT( 0 );
		}
	}
	else if ( ID_ZHONGCANGCHIGU == nID )
	{
		// �زֳֹ�
		CMerch *pMerch = GetMerchXml();
		if ( NULL != pMerch )
		{
			//
			CDlgZhongCangChiGu dlg(this);
			dlg.SetMerch(pMerch);
			dlg.DoModal();
		}
	}
	else if (IDM_SETUPALARM == nID)
	{
		ShowSetupAlarms();
	}
}

void CIoViewBase::PostNcDestroy()
{
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	if ( pMainFrm->m_pKBParent == this )
	{
		pMainFrm->SetHotkeyTarget(NULL);
	}

	// �������û�е��õĻ�����Ҫ���±��������ͼ��message map
	CControlBase::PostNcDestroy();
	{	// �����F10��ע����ͼ - ��ʱ�ر�f10��ͼ xl 0719
		if ( NULL != pMainFrm->m_pDlgF10 && pMainFrm->m_pDlgF10->GetIoView() == this )
		{
			pMainFrm->m_pDlgF10->PostMessage(WM_CLOSE, 0, 0);
		}

	}

	//delete this;	// ɾ������ - ������ǰ���븴�ӣ��ֲ��ı�ɾ�����
}

CMerch* CIoViewBase::ConvertMerchKeyToMerch( int32 iMarketId, const CString &StrMerchCode )
{
	if ( NULL != m_pAbsCenterManager )
	{
		CMerch *pMerch;
		if ( m_pAbsCenterManager->GetMerchManager().FindMerch(StrMerchCode, iMarketId, pMerch) )
		{
			return pMerch;
		}
	}
	return NULL;
}

int32 CIoViewBase::ConvertMerchKeysToMerchs( const CArray<CMerchKey, CMerchKey &> &aKeys, OUT CArray<CMerch *, CMerch *> &aMerchs, bool32 bIgnoreMissedKey )
{
	int32 iFindCount = 0;	
	for ( int i = 0; i < aKeys.GetSize() ; i++ )
	{
		CMerch *pMerch = ConvertMerchKeyToMerch(aKeys[i].m_iMarketId, aKeys[i].m_StrMerchCode);
		if ( NULL != pMerch )
		{
			iFindCount++;
			aMerchs.Add(pMerch);
		}
		else if ( !bIgnoreMissedKey )	// Ҫ�����һ��NULL
		{
			aMerchs.Add(NULL);
		}
		if ( NULL == pMerch )
		{
			
			TRACE(_T("Ignore merch key: %d - %s\r\n"), aKeys[i].m_iMarketId, CString(aKeys[i].m_StrMerchCode).GetBuffer());
		}
	}
	return iFindCount;
}

void CIoViewBase::OnShowWindow( BOOL bShow, UINT nStatus )
{
	// һ�㸸�������ص��´������ز�����յ�����Ϣ
	CControlBase::OnShowWindow(bShow, nStatus);
}

void CIoViewBase::SetKillerTimer(bool32 bSet)
{
	KillTimer(KTimerIdCheckWindowShow);

	if(bSet)
	{
		SetTimer(KTimerIdCheckWindowShow, KTimerPeriodCheckWindowShow, NULL);
	}
}

void CIoViewBase::OnTimer(UINT nIDEvent)
{
// 	if ( 510 == nIDEvent )
// 	{
// 		CPoint pt;
// 		CRect rc(0,0,0,0);
// 		GetCursorPos(&pt);
// 		GetClientRect(rc);
// 		ClientToScreen(&rc);
// 		pt.x -= 20;
// 		if ( pt.x < rc.left || pt.y >= rc.right )
// 		{
// 			pt.x = rc.right-1;
// 		}
// 		if ( pt.y < rc.top || pt.y >= rc.bottom )
// 		{
// 			pt.y = rc.top-10;
// 		}
// 		// 		INPUT inp = {0};
// 		// 		inp.type = INPUT_MOUSE;
// 		// 		inp.mi.dx = pt.x;
// 		// 		inp.mi.dy = pt.y;
// 		// 		inp.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTDOWN;
// 		// 		SendInput(1, &inp, sizeof(inp));
// 		SetCursorPos(pt.x, pt.y);
// 		mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTDOWN, pt.x, pt.y, 0, 0);
// 		mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTUP, pt.x, pt.y, 0, 0);
// 		if ( GetAsyncKeyState(VK_F6)&0x80000000 )
// 		{
// 			KillTimer(nIDEvent);
// 		}
// 	}
	if ( KTimerIdCheckWindowShow == nIDEvent )	// ��Timer�п��ܻ�Ƚ�������Դ��������������ʵ�ֹ��˲��ɼ�ʱ����(OnIdle???)
	{
		if ( m_bHasIgnoreLastViewDataReq && IsNowCanReqData() )		// ���һ��Ҫ���͵�����û�з���ȥ - �򵥱�־������δ���������������
		{
			OnVDataForceUpdate();		// ˢ������
			//ASSERT( !m_bHasIgnoreLastViewDataReq );	// Ӧ����������˵�
			m_bHasIgnoreLastViewDataReq = false;	//	��ֹ��ͣ��timer
		}
	}
	CControlBase::OnTimer(nIDEvent);
}

bool32 CIoViewBase::IsNowCanReqData()
{
	CMPIChildFrame *pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());	// !!mainframe����Ҳ��frame
	if ( NULL == pFrame )
	{
		// ��ҳ����ֻ��Ҫ�ж��Ƿ�����
		return IsWindowVisible();
	}
	
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	if ( NULL == pMainFrame )
	{
		return IsWindowVisible();
	}
	
	CFrameWnd	*pActiveFrame = pMainFrame->MDIGetActive(); 
	if ( NULL != pActiveFrame && pActiveFrame != pFrame  && pActiveFrame->IsZoomed() )
	{
		// ����ҳ����󻯣��������ͼ���������������Բ��ط���������
		return false;
	}

	//---wangyongxue ͬһFrame�¿���������ͼҲ���ط���������
	if ( m_pIoViewManager )		// ���ͬ�п���ΪNULL
	{
		int32 iCurShow = m_pIoViewManager->m_GuiTabWnd.GetCurtab();

		if ( iCurShow < 0 || iCurShow >= m_pIoViewManager->m_IoViewsPtr.GetSize() )
		{
			return false;
		}

		CIoViewBase *pIoView = m_pIoViewManager->m_IoViewsPtr[iCurShow];

		if ( m_StrTabShowName != pIoView->m_StrTabShowName )
		{
 			if (m_bFirstRequestData)
			{
				m_bFirstRequestData = false;
			}
			else
			{
				return false;
			}
		}
	}

	CMPIChildFrame *pMPI = DYNAMIC_DOWNCAST(CMPIChildFrame, pFrame);
	if ( NULL != pMPI 
		&& (!pMPI->IsEnableRequestData()||pMPI->IsHideMode()) )
	{
		// ��ͼ��frame��ʱ��ʾ��Ӧ���������ݣ��Ͳ���������
		return false;
	}
	
	return m_bShowNow;
	//return IsWindowVisible();
}

bool32 CIoViewBase::IsNowCanRefreshUI()
{
	CMPIChildFrame *pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());	// !!mainframe����Ҳ��frame
	if ( NULL == pFrame )
	{
		// ��ҳ����ֻ��Ҫ�ж��Ƿ�����
		return IsWindowVisible();
	}

	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	if ( NULL == pMainFrame )
	{
		return IsWindowVisible();
	}

	CFrameWnd	*pActiveFrame = pMainFrame->MDIGetActive(); 
	if ( NULL != pActiveFrame && pActiveFrame != pFrame  && pActiveFrame->IsZoomed() )
	{
		// ����ҳ����󻯣��������ͼ����������
		return false;
	}

	// ͬһFrame�¿���������ͼҲ����ˢ����
	if ( m_pIoViewManager )		// ���ͬ�п���ΪNULL
	{
		int32 iCurShow = m_pIoViewManager->m_GuiTabWnd.GetCurtab();

		if ( iCurShow < 0 || iCurShow >= m_pIoViewManager->m_IoViewsPtr.GetSize() )
		{
			return false;
		}

		CIoViewBase *pIoView = m_pIoViewManager->m_IoViewsPtr[iCurShow];

		if ( m_StrTabShowName != pIoView->m_StrTabShowName )
		{
			return false;
		}
	}

	return IsWindowVisible();
}

bool32 CIoViewBase::DoRequestViewData( CMmiCommBase &req, bool32 bForceReq /*= false*/, bool32 bLogIfCancel /*= true*/ )
{
	if ( (IsNowCanReqData() || bForceReq) && m_pAbsCenterManager != NULL )
	{
		m_pAbsCenterManager->RequestViewData(&req);

		// ���һ����Ч�����Ѿ����� - 
		//	�п���ǰ������󱻹����ˣ����Ǻ��������ڷ��ͣ�����ֻ�����˲�������
		//  �������ڻ���������������Ϣϵͳ��ͬ�����еģ����ֿ����ԱȽ�С
		//  m_bHasIgnoreLastViewDataReq = false;		
		return true;
	}
	else
	{
		if ( bLogIfCancel )
		{
			m_bHasIgnoreLastViewDataReq = true;		// ������ǣ�����ͼ�ɼ�ʱ��timer����force update ��������
		}
	}
	return false;
}

void CIoViewBase::SetHasIgnoreLastViewDataReqFlag( bool32 bIgnored )
{
	m_bHasIgnoreLastViewDataReq = bIgnored;
}

bool32 CIoViewBase::RequestLastIgnoredReqData()
{
	return false;
	if ( IsNowCanReqData() && m_bHasIgnoreLastViewDataReq )		// ���һ��Ҫ���͵�����û�з���ȥ - �򵥱�־������δ���������������
	{
		OnVDataForceUpdate();		// ˢ������
		return true;
	}
	return false;
}

CMerch * CIoViewBase::GetMerchXml()
{
	if ( NULL != this)
	{
		return m_pMerchXml;
	}
	return NULL;
}

void CIoViewBase::DoShowStdPopupMenu()
{
	CNewMenu Menu;
	Menu.LoadMenu(IDR_MENU_OTHER);
	Menu.LoadToolBar(g_awToolBarIconIDs);
	CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, Menu.GetSubMenu(0));
	
	CMenu* pTempMenu = pPopMenu->GetSubMenu(L"��������");
	CNewMenu * pIoViewPopMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );
	ASSERT(NULL != pIoViewPopMenu );
	AppendIoViewsMenu(pIoViewPopMenu, IsLockedSplit());
	
	// ��ѡ��

	// tab�л�
	if ( GetIoViewManager() != NULL
		&& GetIoViewManager()->m_IoViewsPtr.GetSize() == 1 )
	{
		pPopMenu->RemoveMenu(IDM_IOVIEWBASE_TAB, MF_BYCOMMAND);	// ֻ��һ���Ͳ���ʾ��
	}

	// ������������ָ�״̬����Ҫɾ��һЩ��ť
	CMPIChildFrame *pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pChildFrame && pChildFrame->IsLockedSplit() )
	{
		pChildFrame->RemoveSplitMenuItem(*pPopMenu);
	}
	
	CPoint pt;
	GetCursorPos(&pt);
	pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, AfxGetMainWnd());	
	pPopMenu->DestroyMenu();			
}

void CIoViewBase::AppendStdMenu( CMenu *pMenu, DWORD/* dwFlag = 0*/ )
{
	if ( NULL != pMenu )
	{
		if ( pMenu->IsKindOf(RUNTIME_CLASS(CNewMenu)) )
		{
			CNewMenu *pNewMenu = NULL;
			pNewMenu = (DYNAMIC_DOWNCAST(CNewMenu, pMenu));
			AppendStdMenu(pNewMenu);
			return;
		}
		CMenu &menu = *pMenu;
		// ��ͨ��ť
		if ( menu.GetMenuItemCount() > 0 )
		{
			menu.AppendMenu(MF_SEPARATOR);
		}
		// ��������:
// 		CNewMenu* pIoViewPopMenu = menu.AppendODPopupMenu(L"��������");
// 		ASSERT(NULL != pIoViewPopMenu );
// 		AppendIoViewsMenu(pIoViewPopMenu);
// 		
		// �ر�����:
		menu.AppendMenu(MF_STRING, IDM_IOVIEWBASE_CLOSECUR, _T("�ر�����"));
		
		// �����л�:	
		if ( GetIoViewManager() != NULL
			&& GetIoViewManager()->m_IoViewsPtr.GetSize() > 1 )
		{
			menu.AppendMenu(MF_STRING, IDM_IOVIEWBASE_TAB, _T("�����л� TAB"));
		}

		menu.AppendMenu(MF_SEPARATOR);
		
		// ȫ��/�ָ�
		menu.AppendMenu(MF_STRING, IDM_IOVIEWBASE_F7, L"ȫ��/�ָ� F7");
		menu.AppendMenu(MF_SEPARATOR);
		
		
		// �ָ��
		CMenu menuSplit;
		menuSplit.CreatePopupMenu();
		menuSplit.AppendMenu( MF_STRING, IDM_IOVIEWBASE_SPLIT_TOP,    L"�������ͼ");
		menuSplit.AppendMenu( MF_STRING, IDM_IOVIEWBASE_SPLIT_BOTTOM, L"�������ͼ");
		menuSplit.AppendMenu( MF_STRING, IDM_IOVIEWBASE_SPLIT_LEFT,   L"�������ͼ");
		menuSplit.AppendMenu( MF_STRING, IDM_IOVIEWBASE_SPLIT_RIGHT,  L"�������ͼ");
		menu.AppendMenu(MF_POPUP, (UINT)menuSplit.m_hMenu, _T("�ָ��"));
		menuSplit.Detach();
		
		// �رմ���
		menu.AppendMenu(MF_STRING, IDM_IOVIEWBASE_CLOSE, _T("�رմ���"));
		menu.AppendMenu(MF_SEPARATOR);
		
		// �������
		menu.AppendMenu(MF_STRING, ID_SETTING, _T("�������"));
		
		// ���沼��
		menu.AppendMenu(MF_STRING, ID_LAYOUT_ADJUST, _T("���沼��"));

		// ������������ָ�״̬����Ҫɾ��һЩ��ť
		CMPIChildFrame *pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
		if ( NULL != pChildFrame && pChildFrame->IsLockedSplit() )
		{
			pChildFrame->RemoveSplitMenuItem(menu);
		}
	}
}

void CIoViewBase::AppendStdMenu( CNewMenu *pNewMenu, DWORD dwFlag /*= 0*/ )
{
	if ( NULL == pNewMenu )
	{
		return;
	}

	CNewMenu &menu = *pNewMenu;
	// ��ͨ��ť
	if ( menu.GetMenuItemCount() > 0 )
	{
		menu.AppendMenu(MF_SEPARATOR);
	}
	// ������ѡ��
	CMerch *pMerch = GetMerchXml();
	if ( NULL != pMerch )
	{
		// 4+1����ť ������� �زֳֹɻ��� ����[�ҵ���ѡ] ������ѡ��� �Ӱ��ɾ��
	//	menu.AppendODMenu(_T("�������     CTRL+R"), MF_STRING, IDM_BELONG_BLOCK);

//		menu.AppendODMenu(_T("�زֳֹɻ���     36"), MF_STRING, ID_ZHONGCANGCHIGU);

		CString StrDefaultBlock = CUserBlockManager::Instance()->GetServerBlockName();
		if ( !StrDefaultBlock.IsEmpty() )
		{
			CString StrMenuItem;
			StrMenuItem.Format(_T("����[%s] ALT+Z"), StrDefaultBlock.GetBuffer());
			menu.AppendODMenu(StrMenuItem, MF_STRING, ID_BLOCK_ADDTOFIRST);
		}
		menu.AppendODMenu(_T("������ѡ��� CTRL+Z"), MF_STRING, ID_BLOCK_ADDTO);
		menu.AppendODMenu(_T("�Ӱ��ɾ��    ALT+D"), MF_STRING, ID_BLOCK_REMOVEFROM);
		
		menu.AppendMenu(MF_SEPARATOR);

		// ��ǰ�ť
		CNewMenu *pMarkPopMenu = menu.AppendODPopupMenu(_T("��ǵ�ǰ��Ʒ"));
		ASSERT( NULL != pMarkPopMenu );
		pMarkPopMenu->AppendMenu(MF_STRING, ID_MARK_CANCEL, _T("ȡ�����"));
		pMarkPopMenu->AppendMenu(MF_SEPARATOR);
		pMarkPopMenu->AppendMenu(MF_STRING, ID_MARK_TEXT,	_T("�������  CTRL+Q"));
		pMarkPopMenu->AppendMenu(MF_STRING, ID_MARK_1,		_T("��Ǣ�"));
		pMarkPopMenu->AppendMenu(MF_STRING, ID_MARK_2,		_T("��Ǣ�"));
		pMarkPopMenu->AppendMenu(MF_STRING, ID_MARK_3,		_T("��Ǣ�"));
		pMarkPopMenu->AppendMenu(MF_STRING, ID_MARK_4,		_T("��Ǣ�"));
		pMarkPopMenu->AppendMenu(MF_STRING, ID_MARK_5,		_T("��Ǣ�"));
		pMarkPopMenu->AppendMenu(MF_STRING, ID_MARK_6,		_T("��Ǣ�"));
		pMarkPopMenu->AppendMenu(MF_SEPARATOR);
		pMarkPopMenu->AppendMenu(MF_STRING, ID_MARK_MANAGER, _T("��ǹ���"));
		menu.AppendMenu(MF_SEPARATOR);
	}
	// ��������:
	CNewMenu* pIoViewPopMenu = menu.AppendODPopupMenu(L"��������");
	ASSERT(NULL != pIoViewPopMenu );
	AppendIoViewsMenu(pIoViewPopMenu, IsLockedSplit());
	
	// �ر�����:
	menu.AppendMenu(MF_STRING, IDM_IOVIEWBASE_CLOSECUR, _T("�ر�����"));
	
	// �����л�:	
	if ( GetIoViewManager() != NULL
		&& GetIoViewManager()->m_IoViewsPtr.GetSize() > 1 )
	{
		menu.AppendMenu(MF_STRING, IDM_IOVIEWBASE_TAB, _T("�����л� TAB"));
	}
	menu.AppendMenu(MF_SEPARATOR);
	
	// ȫ��/�ָ�
	menu.AppendODMenu(L"ȫ��/�ָ� F7", MF_STRING, IDM_IOVIEWBASE_F7);
	menu.AppendODMenu(L"", MF_SEPARATOR);
	
	
	// �ָ��
	CNewMenu *pMenuSplit = (pNewMenu->AppendODPopupMenu(_T("�ָ��")));
	ASSERT( pMenuSplit != NULL );
	pMenuSplit->AppendMenu( MF_STRING, IDM_IOVIEWBASE_SPLIT_TOP,    L"�������ͼ");
	pMenuSplit->AppendMenu( MF_STRING, IDM_IOVIEWBASE_SPLIT_BOTTOM, L"�������ͼ");
	pMenuSplit->AppendMenu( MF_STRING, IDM_IOVIEWBASE_SPLIT_LEFT,   L"�������ͼ");
	pMenuSplit->AppendMenu( MF_STRING, IDM_IOVIEWBASE_SPLIT_RIGHT,  L"�������ͼ");
	
	
	// �رմ���
	menu.AppendMenu(MF_STRING, IDM_IOVIEWBASE_CLOSE, _T("�رմ���"));
	menu.AppendMenu(MF_SEPARATOR);
	
	// �������
	//menu.AppendMenu(MF_STRING, ID_SETTING, _T("�������"));
	menu.AppendODMenu(L"�������", MF_STRING, ID_SETTING);
	
	// ���沼��
	//menu.AppendMenu(MF_STRING, ID_LAYOUT_ADJUST, _T("���沼��"));
	menu.AppendODMenu(L"���沼��", MF_STRING, ID_LAYOUT_ADJUST);

	// ������������ָ�״̬����Ҫɾ��һЩ��ť
	CMPIChildFrame *pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pChildFrame  )
	{
		pChildFrame->RemoveSplitMenuItem(menu);
	}
	
	menu.LoadToolBar(g_awToolBarIconIDs);
}

bool32 CIoViewBase::IsActiveInFrame()
{
	CFrameWnd *pFrame = GetParentFrame();
	if ( NULL == pFrame )
	{
		// ����frame�µĻ���û��ioviewmanager�ģ�ֱ��false
		return false;
	}

	CGGTongView *pViewActive = DYNAMIC_DOWNCAST(CGGTongView, pFrame->GetActiveView());	
	if ( NULL == pViewActive )
	{
		return false;	// �޼���view��false
	}

	{
		// ��manager������Ƿ��������manager
		CGGTongView *pView = GetParentGGtongView();
		if ( pView != pViewActive )
		{
			return false;		// �����ڼ���view�� false
		}
	}

	if ( pViewActive->m_IoViewManager.GetActiveIoView() == this )		// �ɼ����ǵ���
	{
		return true;		
	}
	return false;
}

void CIoViewBase::GetTradeTimes(IN const CGmtTime &TimeSpecify, IN const MerchArray &aMerchs, OUT GmtTimeArray &aTradeTimes )
{
	aTradeTimes.RemoveAll();
	CGmtTime Time0(TimeSpecify);

	//
	CMapPtrToWord aMarkets;
	GmtTimeArray aTimesTmp;
	int32 i = 0;
	for ( i=0; i < aMerchs.GetSize() ; i++ )
	{
		if ( NULL == aMerchs[i] )
		{
			ASSERT( 0 );
			continue;
		}

		aMarkets[ & aMerchs[i]->m_Market ] = 0;

		//
		CMarketIOCTimeInfo IOCTime;
		if (aMerchs[i]->m_MerchInfo.m_iTradeTimeID > 0 && aMerchs[i]->m_MerchInfo.m_iTradeTimeType > 0)
		{
			if(aMerchs[i]->m_Market.GetSpecialTradingDayTime(Time0, IOCTime, aMerchs[i]->m_MerchInfo))
			{
				aTimesTmp.Append(IOCTime.m_aOCTimes);
			}
		}
	}

	//CGmtTime Time0;	
	POSITION pos = aMarkets.GetStartPosition();
	while ( pos != NULL )
	{
		CMarket *pMarket = NULL;
		WORD w = 0;
		aMarkets.GetNextAssoc(pos, (void *&)pMarket, w);

		ASSERT( NULL != pMarket );
		CMarketIOCTimeInfo IOCTime;
		CMerchInfo MerchInfo;
		MerchInfo.m_iTradeTimeID = -1;
		MerchInfo.m_iTradeTimeType = -1;

		if ( pMarket->GetSpecialTradingDayTime(Time0, IOCTime, MerchInfo)  )
		{
			aTimesTmp.Append( IOCTime.m_aOCTimes );
		}
	}

	// �ϲ�ʱ��
	for ( i=0; i + 1 < aTimesTmp.GetSize() ; i += 2 )	// ���ճɶԳ���
	{
		const CGmtTime &TimeOpen  = aTimesTmp[i];
		const CGmtTime &TimeClose = aTimesTmp[i+1];
		
		int32 j=0;
		bool32 bDeal = false;
		for ( j=0; j +1 < aTradeTimes.GetSize() ; j += 2 )
		{
			CGmtTime &Time1 = aTradeTimes[j];
			CGmtTime &Time2 = aTradeTimes[j+1];

			if ( Time1 > TimeOpen && Time1 < TimeClose )		// �¿���ʱ��� ԭ�е��� && ������ʱ���ԭ������
			{
				Time1 = TimeOpen;
				bDeal = true;
			}
			if ( Time2 > TimeOpen && Time2 < TimeClose )		// ������ʱ�� ��ԭ���� && ԭ�� ���ֿ���
			{
				Time2 = TimeClose;
				bDeal = true;
			}
			if ( bDeal )
			{
				break;
			}
		}
		if ( !bDeal )
		{
			aTradeTimes.Add( TimeOpen );
			aTradeTimes.Add( TimeClose );
		}
	}

	// �Ƿ���Ҫ�����ϲ���
	
	// trace
// #ifdef _DEBUG
// 	TRACE(_T("�ϲ���Ʒ���Ͽ�����ʱ��(GMT): \r\n"));
// 	for ( i=0; i+1 < aTradeTimes.GetSize() ; i += 2 )
// 	{
// 		const CGmtTime &TimeOpen  = aTradeTimes[i];
// 		const CGmtTime &TimeClose = aTradeTimes[i+1];
// 
// 		TRACE(_T("    %05d: %02d:%02d - %02d:%02d\r\n"), TimeOpen.GetHour(), TimeOpen.GetMinute(),
// 										TimeClose.GetHour(), TimeClose.GetMinute())	;
// 	}
// #endif
}

bool32 CIoViewBase::IsTimeInTradeTimes( IN const GmtTimeArray &aTradeTimes, IN const CGmtTime &TimeCheck )
{
	for ( int32 i=0; i + 1 < aTradeTimes.GetSize() ; i += 2 )
	{
		const CGmtTime &TimeOpen  = aTradeTimes[i];
		const CGmtTime &TimeClose = aTradeTimes[i+1];
		CGmtTime TimeCloseAft(TimeClose);
		TimeCloseAft += CGmtTimeSpan(0, 0, 2, 0);		// // ��������ʱ��2�����ˣ����ܴ��ڷ���������ʱ��ƫ��
		if ( TimeCheck >= TimeOpen && TimeCheck <= TimeCloseAft )
		{
			return true;
		}
	}
	return false;
}


bool32 CIoViewBase::GetNowServerTime( OUT CGmtTime &TimeNow )
{
	CAbsCenterManager *pAbsCenterManager = m_pAbsCenterManager;
	if ( NULL == pAbsCenterManager )
	{
		CGGTongDoc *pDoc = AfxGetDocument();
		if ( pDoc != NULL )
		{
			pAbsCenterManager = pDoc->m_pAbsCenterManager;
		}
	}
	if ( NULL == pAbsCenterManager )
	{
		return false;
	}
	TimeNow = pAbsCenterManager->GetServerTime();
	return true;
}


bool32 CIoViewBase::IsNowInTradeTimes( IN const GmtTimeArray &aTradeTimes )
{
	CGmtTime TimeNow;
	if ( !GetNowServerTime(TimeNow) )
	{
		ASSERT( 0 );
		return false;
	}

	return IsTimeInTradeTimes(aTradeTimes, TimeNow);
}

bool32 CIoViewBase::IsNowInTradeTimes( IN const MerchArray &aMerchs, bool32 bCheckTradeDay/* = false */)
{
	CGmtTime TimeNow;
	if ( !GetNowServerTime(TimeNow) )
	{
		ASSERT( 0 );
		return false;
	}

	if ( bCheckTradeDay )
	{
		// �������Ƿ�Ϊ������
		if ( !IsTimeInTradeDay(aMerchs, TimeNow) )
		{
			return false;
		}
	}

	GmtTimeArray aTradeTimes;
	GetTradeTimes(TimeNow, aMerchs, aTradeTimes);
	return IsTimeInTradeTimes(aTradeTimes, TimeNow);
}

bool32 CIoViewBase::IsNowInTradeTimes( CMerch *pMerch, bool32 bCheckTradeDay /*= false*/ )
{
	if ( NULL != pMerch )
	{
		MerchArray aMerchs;
		aMerchs.Add(pMerch);
		return IsNowInTradeTimes(aMerchs, bCheckTradeDay);
	}
	return false;
}

bool32 CIoViewBase::IsTimeInTradeDay( IN const MerchArray &aMerchs, const CGmtTime &TimeSpecify )
{
	CMapPtrToWord aMarkets;
	int32 i = 0;

	// ����û��ҹ�̵���Ʒ
	MerchArray aMerchSpecial;

	//
	for ( i=0; i < aMerchs.GetSize() ; i++ )
	{
		if ( NULL == aMerchs[i] )
		{
			ASSERT( 0 );
			continue;
		}
		aMarkets[ & aMerchs[i]->m_Market ] = 0;

		if (aMerchs[i]->m_MerchInfo.m_iTradeTimeID > 0 && aMerchs[i]->m_MerchInfo.m_iTradeTimeType > 0)
		{
			aMerchSpecial.Add(aMerchs[i]);
		}
	}

	//CGmtTime Time0;
	CGmtTime Time0(TimeSpecify);

	//
	for (i = 0; i < aMerchSpecial.GetSize(); i++)
	{
		//
		CMarketIOCTimeInfo IOCTime;
		if ( aMerchSpecial[i]->m_Market.GetRecentTradingDay(Time0, IOCTime, aMerchSpecial[i]->m_MerchInfo)
			&& IOCTime.m_TimeInit <= Time0
			&& IOCTime.m_TimeEnd >= Time0 )
		{
			return true;
		}
	}

	//
	POSITION pos = aMarkets.GetStartPosition();

	// ֻ�е����е��г������ڽ����������Ϊ���ڽ���ʱ���
	int iMarketNotInTradeTimes = 0;
	while ( pos != NULL )
	{
		CMarket *pMarket = NULL;
		WORD w = 0;
		aMarkets.GetNextAssoc(pos, (void *&)pMarket, w);
		
		ASSERT( NULL != pMarket );
		CMarketIOCTimeInfo IOCTime;
		
		CMerchInfo MerchInfo;
		MerchInfo.m_iTradeTimeID = -1;
		MerchInfo.m_iTradeTimeType = -1;

		if ( !pMarket->GetRecentTradingDay(Time0, IOCTime,MerchInfo)
			|| IOCTime.m_TimeInit > Time0
			|| IOCTime.m_TimeEnd < Time0 )
		{
			// �޷���ȡ��������Ϣ�����߲�������Ľ�������
			iMarketNotInTradeTimes++;
		}
	}

	// ���������ڽ���ʱ���ڵ���Ϊ���еĶ��ڽ���ʱ����
	return aMarkets.GetCount() > iMarketNotInTradeTimes;	// ���г�ʱ����false
}

void CIoViewBase::GetAttendMarketDataArray( OUT AttendMarketDataArray &aAttends )
{
	aAttends.RemoveAll();
	T_AttendMarketData data;
	for (int32 iIndexAttendMerch = 0; iIndexAttendMerch < m_aSmartAttendMerchs.GetSize(); iIndexAttendMerch++)
	{
		const CSmartAttendMerch &SmartAttendMerch = m_aSmartAttendMerchs[iIndexAttendMerch];
		if ( NULL == SmartAttendMerch.m_pMerch )
		{
			continue;
		}

		data.m_iMarketId = SmartAttendMerch.m_pMerch->m_MerchInfo.m_iMarketId;
		data.m_iEDSTypes = SmartAttendMerch.m_iDataServiceTypes;
		aAttends.Add(data);
	}
}

void CIoViewBase::OnBlockMenu( UINT nID )
{
	MerchArray aMerchs;
	GetMerchXmls(aMerchs);
	if ( aMerchs.GetSize() <= 0 )
	{
		return;
	}
	CMerch *pMerchXml = aMerchs[aMerchs.GetUpperBound()];
	if ( NULL == pMerchXml )
	{
		return;
	}

	switch ( nID )
	{
	case ID_BLOCK_ADDTOFIRST:
		{
			// ���뵽Ĭ����ѡ���
			T_Block *pBlock = CUserBlockManager::Instance()->GetServerBlock();
			if ( NULL != pBlock )
			{
				for ( int32 i=0; i < aMerchs.GetSize() ; ++i )
				{
					CUserBlockManager::Instance()->AddMerchToUserBlock(aMerchs[i], pBlock->m_StrName, false);
				}
				CUserBlockManager::Instance()->SaveXmlFile();
				CUserBlockManager::Instance()->Notify(CSubjectUserBlock::EUBUMerch);
			}
		}
		break;
	case ID_BLOCK_DELETEFROMFIRST:
		{
			T_Block *pBlock = CUserBlockManager::Instance()->GetServerBlock();
			
			if ( NULL != pBlock )
			{
				for ( int32 i=0; i < aMerchs.GetSize() ; ++i )
				{
					CUserBlockManager::Instance()->DelMerchFromUserBlock(aMerchs[i], pBlock->m_StrName, false);
				}
			}

			CUserBlockManager::Instance()->SaveXmlFile();
			CUserBlockManager::Instance()->Notify(CSubjectUserBlock::EUBUMerch);
		}
		break;
	case ID_BLOCK_ADDTO:
		{
			// ѡ����
			T_Block block;
			if ( CDlgAddToBlock::GetUserSelBlock(block) )
			{
				for ( int32 i=0; i < aMerchs.GetSize() ; ++i )
				{
					CUserBlockManager::Instance()->AddMerchToUserBlock(aMerchs[i], block.m_StrName, false);
				}
				CUserBlockManager::Instance()->SaveXmlFile();
				CUserBlockManager::Instance()->Notify(CSubjectUserBlock::EUBUMerch);
				//CUserBlockManager::Instance()->AddMerchToUserBlock(pMerchXml, block.m_StrName);
			}
		}
		break;
	case ID_BLOCK_REMOVEFROM:
		{
			// �Ӱ����ɾ��
			CStringArray aDelFromBlocks;
			if ( CDlgRemoveFromBlock::RemoveFromBlock(pMerchXml, aDelFromBlocks) )
			{
				for ( int32 i=0; i < aDelFromBlocks.GetSize() ; i++ )
				{
					CUserBlockManager::Instance()->DelMerchFromUserBlock(pMerchXml, aDelFromBlocks[i]);
				}
			}
		}
		break;
	default:
		ASSERT( 0 );
	}
}

DWORD CIoViewBase::GetNeedPushDataType()
{
	return 0xffffffff;
}

CMerch* CIoViewBase::GetMerchAccordExpMerch( CMerch* pMerch )
{
	if ( NULL == pMerch || NULL == m_pAbsCenterManager )
	{
		return NULL;
	}
	
	CMerch* pMerchExp = NULL;	
	if ( 1 == pMerch->m_Market.m_Breed.m_iBreedId )
	{				
		if ( 1004 == pMerch->m_Market.m_MarketInfo.m_iMarketId )
		{
			// 2��������С�������С��ָ��
			m_pAbsCenterManager->GetMerchManager().FindMerch(/*L"399005"*/L"399101", 1000, pMerchExp);
		}
		else if ( 1008 == pMerch->m_Market.m_MarketInfo.m_iMarketId )
		{
			// 3�����ڴ�ҵ����ô�ҵ��ָ��
			m_pAbsCenterManager->GetMerchManager().FindMerch(/*L"399006"*/L"399102", 1000, pMerchExp);
		}
		else
		{
			// 4�������ȫ���������ָ
			m_pAbsCenterManager->GetMerchManager().FindMerch(L"399001", 1000, pMerchExp);
		}	
	}
	
	// Ĭ�϶�����ָ֤��
	if ( NULL == pMerchExp )
	{
		m_pAbsCenterManager->GetMerchManager().FindMerch(L"000001", 0, pMerchExp);
	}
	
	//
	return pMerchExp;
}

bool32 CIoViewBase::IsNeedPushData( CMerch *pMerch, E_DataServiceType eDataServiceType )
{
	DWORD dwPush = GetNeedPushDataType();
	DWORD dwNeed = eDataServiceType;
	if ( (dwPush & dwNeed) == dwNeed && dwNeed != 0 )
	{
		return IsAttendData(pMerch, eDataServiceType);		// ��Ҫ�������ͣ����ҹ�ע������
	}
	return false;
}

CMerch* CIoViewBase::GetMerchAccordExpMerchStatic( CMerch* pMerch )
{
	if ( NULL == pMerch )
	{
		return NULL;
	}

	CGGTongDoc *pDoc = AfxGetDocument();
	if ( NULL == pDoc || NULL == pDoc->m_pAbsCenterManager )
	{
		return NULL;
	}

	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;
	CMerch* pMerchExp = NULL;	
	if ( 1 == pMerch->m_Market.m_Breed.m_iBreedId )
	{				
		if ( 1004 == pMerch->m_Market.m_MarketInfo.m_iMarketId )
		{
			// 2��������С�������С��ָ��
			pAbsCenterManager->GetMerchManager().FindMerch(/*L"399005"*/L"399101", 1000, pMerchExp);
		}
		else if ( 1008 == pMerch->m_Market.m_MarketInfo.m_iMarketId )
		{
			// 3�����ڴ�ҵ����ô�ҵ��ָ��
			pAbsCenterManager->GetMerchManager().FindMerch(/*L"399006"*/L"399102", 1000, pMerchExp);
		}
		else
		{
			// 4�������ȫ���������ָ
			pAbsCenterManager->GetMerchManager().FindMerch(L"399001", 1000, pMerchExp);
		}	
	}
	
	// Ĭ�϶�����ָ֤��
	if ( NULL == pMerchExp )
	{
		pAbsCenterManager->GetMerchManager().FindMerch(L"000001", 0, pMerchExp);
	}
	
	//
	return pMerchExp;
}

void CIoViewBase::GetSpecialAccordExpMerch( OUT MerchArray &aExpMerchs )
{
	aExpMerchs.RemoveAll();
	CGGTongDoc *pDoc = AfxGetDocument();
	if ( NULL == pDoc || NULL == pDoc->m_pAbsCenterManager )
	{
		return;
	}
	
	CAbsCenterManager *pAbsCenterManager = pDoc->m_pAbsCenterManager;

	CArray<CMerchKey , const CMerchKey &> aExpKeys;
	// ��ָ֤��
	aExpKeys.Add(CMerchKey(0, _T("000001")));
	aExpKeys.Add(CMerchKey(1000, _T("399001")));
	aExpKeys.Add(CMerchKey(1000, _T("399102")));
	aExpKeys.Add(CMerchKey(1000, _T("399101")));

	for ( int32 i=0; i < aExpKeys.GetSize() ; i++ )
	{
		CMerch *pExp = NULL;
		if ( pAbsCenterManager->GetMerchManager().FindMerch( aExpKeys[i].m_StrMerchCode, aExpKeys[i].m_iMarketId, pExp) )
		{
			aExpMerchs.Add(pExp);
		}
	}
}

bool32 CIoViewBase::InitDrawMarkItem( const T_MarkData &MarkData, OUT GV_DRAWMARK_ITEM &DrawMarkItem )
{
	if ( /*MarkData.m_eType < 0 ||*/ MarkData.m_eType >= EMT_Count )
	{
		// ��Ч���ͣ�������
		DrawMarkItem.eDrawMarkFlag = EDMF_None;
		return false;
	}

	// ����16����15...
	DrawMarkItem.eDrawMarkFlag = EDMF_Top;
	E_SysColor eColor = (E_SysColor)(ESCGuideLine16 - MarkData.m_eType);
	DrawMarkItem.clrMark = GetIoViewColor(eColor);
	if ( MarkData.m_eType == EMT_Text )
	{
		DrawMarkItem.strMark = _T("T");	// ���ֻ���һ��T
	}
	else
	{
		DrawMarkItem.strMark = MarkData.m_StrTip;	// ʹ���ڲ��Ģ�...
	}
	return true;
}

bool32 CIoViewBase::InitDrawMarkItem( CMerch *pMerch, OUT GV_DRAWMARK_ITEM &DrawMarkItem )
{
	T_MarkData MarkData;
	if ( CMarkManager::Instance().QueryMark(pMerch, MarkData) )
	{
		return InitDrawMarkItem(MarkData, DrawMarkItem);
	}
	return false;
}

void CIoViewBase::DrawMerchName( CDC &dc, CMerch *pMerch, const CRect &rcText, bool32 bDrawCode/* = true*/, OUT CRect *pRectMark/*=NULL*/ )
{
	if ( NULL != pRectMark )
	{
		pRectMark->SetRectEmpty();
	}
	if ( NULL == pMerch || rcText.Width() <= 0 )
	{
		return;
	}

	CString StrMerchName = pMerch->m_MerchInfo.m_StrMerchCnName;
	CString StrMerchCode = pMerch->m_MerchInfo.m_StrMerchCode;
	T_Block *pBlock = CUserBlockManager::Instance()->GetBlock(pMerch);
	COLORREF clrName = RGB(162, 162, 0);
	COLORREF clrCode = GetIoViewColor(ESCVolume);
	if ( pBlock != NULL
		&& pBlock->m_clrBlock != CLR_DEFAULT )
	{
		clrName = pBlock->m_clrBlock;
	}
	else
	{
		clrName = clrCode;
	}

	// ���� �ַ�����(600000) ����
	CString StrTitle;
	if ( bDrawCode )
	{
		StrTitle.Format(_T("%s(%s)"), StrMerchName.GetBuffer(), StrMerchCode.GetBuffer());
	}
	else
	{
		StrTitle = StrMerchName;
	}

	CRect rcTitle(rcText);
	CSize sizeTitle = dc.GetTextExtent(StrTitle);	// �������ϱ�����
	rcTitle.left = rcText.left + (rcText.Width() - sizeTitle.cx)/2;
	rcTitle.left = max(rcTitle.left, rcText.left);

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(clrName);
	dc.DrawText(StrMerchName, rcTitle, DT_SINGLELINE |DT_LEFT |DT_VCENTER);
	CSize sizeName = dc.GetTextExtent(StrMerchName);
	rcTitle.left += sizeName.cx + dc.GetTextCharacterExtra();

	if ( bDrawCode )
	{
		// ���Ƶ����� (code)
		CString StrCode;
		StrCode.Format(_T("(%s)"), StrMerchCode.GetBuffer());
		dc.SetTextColor(clrCode);
		dc.DrawText(StrCode, rcTitle, DT_SINGLELINE |DT_LEFT |DT_VCENTER);
	}

	rcTitle.left = rcText.right - (rcText.Width() - sizeTitle.cx)/2 +dc.GetTextCharacterExtra();

	T_MarkData MarkData;
	GV_DRAWMARK_ITEM markItem;
	if ( rcTitle.Width() > 0
		&& CMarkManager::Instance().QueryMark(pMerch, MarkData)
		&& InitDrawMarkItem(MarkData, markItem) )
	{
		// �б����Ҫ����
		// ���Ʊ��
		CFont FontUpHalf;
		LOGFONT lf;
		ZeroMemory(&lf, sizeof(lf));
		dc.GetCurrentFont()->GetLogFont(&lf);
		_tcsncpy(lf.lfFaceName, _T("Tahoma"), LF_FACESIZE-1);
		lf.lfCharSet = DEFAULT_CHARSET;
		lf.lfHeight = -(abs(lf.lfHeight)*2/3);
		lf.lfHeight = min(-12, lf.lfHeight);
		//lf.lfWeight = FW_NORMAL;
		lf.lfWidth = 0;
		if ( FontUpHalf.CreateFontIndirect(&lf) )
		{
			CFont *pFontOld2 = dc.SelectObject(&FontUpHalf);
			if ( markItem.clrMark != CLR_DEFAULT )
			{
				dc.SetTextColor(markItem.clrMark);
			}
			else
			{
				dc.SetTextColor(GetIoViewColor( ESCAmount ));
			}
			
			DWORD nFmt = DT_SINGLELINE |DT_LEFT;
			if ( markItem.eDrawMarkFlag == EDMF_Bottom )
			{
				nFmt |= DT_BOTTOM;
			}
			else
			{
				nFmt |= DT_TOP;
			}
			dc.DrawText(markItem.strMark, rcTitle, nFmt);
			if ( NULL != pRectMark )
			{
				CSize sizeMark = dc.GetTextExtent(markItem.strMark);
				*pRectMark = rcTitle;
				pRectMark->right = rcTitle.left + sizeMark.cx;
				pRectMark->bottom = rcTitle.top + sizeMark.cy;
			}
			dc.SelectObject(pFontOld2);
		}
		else
		{
			ASSERT( 0 );
		}
	}
}

void CIoViewBase::OnUpdateIoViewMenu( CCmdUI *pCmdUI )
{
	if ( NULL == pCmdUI )
	{
		return;
	}

	UINT nID = pCmdUI->m_nID;

	if ( (nID >= IDC_CHART_ADD2BLOCK_BEGIN && nID <= IDC_CHART_ADD2BLOCK_END)
		|| (nID >= ID_BLOCK_ADDTOFIRST && nID <= ID_BLOCK_ADDTO) )
	{
		// ������ѡ��
		CMerch *pMerch = GetMerchXml();
		pCmdUI->Enable(NULL != pMerch);
		return;
	}
	else if ( nID >= ID_MARK_TEXT && nID <= ID_MARK_6 )
	{
		// ���״̬
		CMerch *pMerch = GetMerchXml();
		T_MarkData MarkData;
		pCmdUI->Enable(NULL != pMerch);
		if ( CMarkManager::Instance().QueryMark(pMerch, MarkData) )
		{
			pCmdUI->SetCheck( nID == (MarkData.m_eType+ID_MARK_TEXT) );
		}
		else
		{
			pCmdUI->SetCheck(0);
		}
		return;
	}

	// ����
}

void CIoViewBase::OnIoViewActive()
{
	m_bActive = IsActiveInFrame(); //test2013
	const T_IoViewObject*	pObject = CIoViewManager::FindIoViewObjectByIoViewPtr(this);
	if ( NULL == pObject )
	{
		return;
	}

	//
	if ( EIMCSTrue == pObject->m_eMerchChangeSrc )
	{
		m_pIoViewMerchChangeSrc = this;
	}
}

void CIoViewBase::OnIoViewDeactive()
{
}

CMerch* CIoViewBase::GetNextMerch(CMerch* pMerchNow, bool32 bPre)
{
	if ( NULL == pMerchNow || NULL == m_pAbsCenterManager )
	{
		return NULL;
	}

	// Ĭ�ϵĴ���, ��ViewData �е�˳����ȡ��Ʒ
	CMerch* pMerchNext = NULL;	

	if ( bPre )
	{
		if ( !m_pAbsCenterManager->GetMerchManager().FindPrevMerch(pMerchNow, pMerchNext) )
		{
			//ASSERT(0);
		}
	}
	else
	{
		if ( !m_pAbsCenterManager->GetMerchManager().FindNextMerch(pMerchNow, pMerchNext) )
		{
			//ASSERT(0);
		}
	}
	
	if ( NULL == pMerchNext )
	{
		//ASSERT(0);
	}

	//
	return pMerchNext;
}

CMerch* CIoViewBase::PeekLastBackMerch( bool32 bRemove/*=false*/ )
{
	const int32 iUpper = sm_aBackMerchs.GetUpperBound();
	if ( iUpper > 0 )
	{
		CMerch *pMerch = sm_aBackMerchs[iUpper-1];
		if ( bRemove )
		{
			sm_aBackMerchs.RemoveAt(iUpper-1, 2);	// ͬʱ�Ƴ����һ���뵱ǰ����Ʒ
		}
		return pMerch;	// �����ڶ���Ϊ����¼����Ʒ
	}

	return NULL;
}

void CIoViewBase::AddLastCurrentMerch( CMerch *pMerch )
{
	if ( NULL == pMerch )
	{
		return;
	}

	// ������Ʒ���������һ���洢���ǵ�ǰ����Ʒ

	const int32 iUpper = sm_aBackMerchs.GetUpperBound();
	if ( iUpper >= 0 && pMerch == sm_aBackMerchs[iUpper] )
	{
		return;	// ͬ��Ʒû��
	}

	for ( int32 i=iUpper-1; i >=0; --i )
	{
		if ( pMerch == sm_aBackMerchs[i] )
		{
			sm_aBackMerchs.RemoveAt(i);
			break;	// ��ֻ֤��һ����Ʒ�ں����������, �������Ʒ�ظ������ã�������뵽���
		}
	}

	sm_aBackMerchs.Add(pMerch);
}

CMerch* CIoViewBase::PeekLastPrevMerch( bool32 bRemove/*=false*/ )
{
	ASSERT( 0 );	// δʵ��ǰ�����
	return NULL;
}

CIoViewBase* CIoViewBase::GetActiveIoView()
{
	return this;
}

void CIoViewBase::RegisterHotKeyTarget( bool32 bReg )
{
	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if ( bReg )
	{
		if ( NULL != pMainFrame )
		{
			pMainFrame->SetHotkeyTarget(this);
		}
	}
	else
	{
		if ( NULL != pMainFrame
			&& pMainFrame->m_pKBParent == this )
		{
			pMainFrame->SetHotkeyTarget(NULL);
		}
	}
}

bool32 CIoViewBase::ParentIsIoView()
{
	CWnd *pParent = GetParent();
	if ( NULL != pParent
		&& pParent->IsKindOf(RUNTIME_CLASS(CIoViewBase)) )
	{
		return true;
	}
	return false;
}

void CIoViewBase::GetMerchXmls( OUT MerchArray &aMerchs )
{
	aMerchs.RemoveAll();
	CMerch *pMerch = GetMerchXml();
	if ( NULL != pMerch )
	{
		aMerchs.Add(pMerch);
	}
}

bool32 CIoViewBase::IsLockedSplit()
{
	CMPIChildFrame *pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pFrame )
	{
		return pFrame->IsLockedSplit();
	}
	return true;
}

void CIoViewBase::GetSmartAttendArray(OUT SmartAttendMerchArray &aAttends)
{
	if (!m_aSmartAttendMerchs.IsEmpty())
	{
		int arrSize = m_aSmartAttendMerchs.GetSize();
		for (int i = 0; i < arrSize; i++)
		{
			CSmartAttendMerch& attendMerch = m_aSmartAttendMerchs[i];
			aAttends.Add(attendMerch);
		}
	}	
}

void CIoViewBase::ForceUpdateVisibleIoView()
{
	if (IsWindowVisible())
	{
		OnVDataForceUpdate();
	}
	else
	{
		SetHasIgnoreLastViewDataReqFlag(true);		// ���ú����������־
	}
}

void CIoViewBase::ShowSetupAlarms()
{
	mapMerchAlarms mapMerchs;
	CGGTongDoc *pDoc = (CGGTongDoc *)AfxGetDocument();

	if ( !pDoc )
	{
		return;
	}

	CAlarmCenter *pAlarmCenter = pDoc->m_pAarmCneter;

	if (!pAlarmCenter)
	{
		return;
	}

	pAlarmCenter->GetAlarmMap(mapMerchs);

	// ��ȡ��ǰ��Ʒ(Ŀǰֻ֧�ֵ�����Ʒ)
	CMerch* pMerch = NULL;

	MerchArray aMerchs;
	GetMerchXmls(aMerchs);
	int iSize = aMerchs.GetSize();

	if (1 == iSize)
	{
		pMerch = aMerchs.GetAt(0);
	}
	else
	{
		return;
	}

	if ( NULL == pMerch )
	{
		return;
	}

	CDlgAlarmModify Dlg;

	// �ҵ������Ʒ��Ԥ����Ϣ, ��ʼ���Ի���
	CAlarmConditions stAlarmConditions;
	bool32 bExist = false;

	if (pAlarmCenter->GetMerchFixAlarms(pMerch, stAlarmConditions))
	{
		Dlg.SetAlarmConditions(stAlarmConditions);
		bExist = true;
	}

	//	
	if ( IDOK == Dlg.DoModal() )
	{
		Dlg.GetAlarmConditions(stAlarmConditions);
		
		if (bExist)
		{
			// �޸�
			pAlarmCenter->ModifyMerchAlarms(pMerch, stAlarmConditions);
		}
		else
		{			
			// ����
			pAlarmCenter->AddMerchAlarms(pMerch, stAlarmConditions);
			pAlarmCenter->ToXml();
		}

		// ����Ԥ��
		pAlarmCenter->SetAlarmFlag(true);
	}
}
