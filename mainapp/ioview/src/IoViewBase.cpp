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

const int32 KTimerIdCheckWindowShow				= 4625;		// 定时器检查隐藏/显示状态切换 - 该方法期待改进
const int32 KTimerPeriodCheckWindowShow			= 100;

// 与该类相关的xml配置中相关信息
static const char * KStrElementValue			= "IoView";
static const char * KStrElementAttrIoViewType	= "IoViewType";

static const char * KStrElementAttrMarketId		= "MakretId";
static const char * KStrElementAttrMerchCode	= "MerchCode";
static const char * KStrElementAttrShowTabName	= "TABNAME";

const char* KIoViewKLineWeightType				= "WeightType";
//

// 功能按钮
static const T_FuctionButton s_astFuctionButtons[] =
{	
	T_FuctionButton( L"F7", L"全屏/还原视图",	IOVIEW_TITLE_F7_BUTTON_ID, IMAGEID_NOUSEIMAGE, IMAGEID_NOUSEIMAGE, IMAGEID_NOUSEIMAGE),
//	T_FuctionButton( L"+",  L"增加视图",		IOVIEW_TITLE_ADD_BUTTON_ID, IMAGEID_NOUSEIMAGE, IMAGEID_NOUSEIMAGE, IMAGEID_NOUSEIMAGE),
	T_FuctionButton( L"×", L"删除当前视图",	IOVIEW_TITLE_DEL_BUTTON_ID, IMAGEID_NOUSEIMAGE, IMAGEID_NOUSEIMAGE, IMAGEID_NOUSEIMAGE),
};

// 功能按钮个数
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

	//memset(m_aIoViewFont,  '0', sizeof(m_aIoViewFont)); // 构造函数
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

	// 视图激活亮点隐藏
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
	// 向ViewData注销自己
	if (NULL != m_pAbsCenterManager)
		m_pAbsCenterManager->RemoveViewListener(this);

	DEL(m_pDelayPageUpDown);


	CMainFrame*  pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if(pMainFrame)
		pMainFrame->RemoveIoView(this);

	// 清空这个值
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
				// 放大字体:
				ChangeFontByUpDownKey(true);
				return TRUE;
			}
			else if ( VK_DOWN == pMsg->wParam && IsCtrlPressed())
			{
				// 缩小字体:
				ChangeFontByUpDownKey(false);
				return TRUE;
			}
			else if ( VK_PRIOR == pMsg->wParam && EIVT_SingleMerch == GetIoViewType() )	// 非单窗口不适合此切换商品
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
			else if ( VK_NEXT == pMsg->wParam && EIVT_SingleMerch == GetIoViewType() )	// 非单窗口不适合此切换商品
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
				// 如果被 F7 放大了,优先处理
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

				// 稍后处理其他的ESC 事件
				return DealEscKey();								
			}
			else if ( 'Z' == pMsg->wParam )	// 自选股CTRL+Z & CTRL+SHIFT+Z
			{
				bool32 bCtrl = IsCtrlPressed();
				bool32 bIsShift = IsShiftPressed();
				bool32 bIsAlt  = (pMsg->lParam & (1<<29)) != 0;
				CMerch *pMerch = GetMerchXml();
				if ( bCtrl && bIsShift && NULL != pMerch )
				{
					// 删除自选股 
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
					// 加入自选股 - 从第一个中加入
					PostMessage(WM_COMMAND, ID_BLOCK_ADDTOFIRST, 0);
					return TRUE;
				}
				// continue
			}
			else if ( 'D' == pMsg->wParam && ((pMsg->lParam & (1<<29)) != 0) )
			{
				// ALT+D 删除
				CMerch *pMerch = GetMerchXml();
				if ( NULL != pMerch )
				{
					PostMessage(WM_COMMAND, ID_BLOCK_REMOVEFROM, 0);
					return TRUE;
				}
			}
			else if ( 'R' == pMsg->wParam && IsCtrlPressed() )
			{
				// 所属板块 CTRL + R
				::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, IDM_BELONG_BLOCK, 0);
				return TRUE;
			}
			else if ( 'Q' == pMsg->wParam && IsCtrlPressed() )
			{
				// 文字标记
				PostMessage(WM_COMMAND, ID_MARK_TEXT, 0);
				return TRUE;
			}
			else if ( VK_BACK == pMsg->wParam && EIVT_SingleMerch == GetIoViewType() )	// 非单窗口不适合返回最近商品
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
			if ( EIVT_SingleMerch == GetIoViewType() )	// 非单窗口不适合此切换商品
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
		// 向ViewData注销自己
		if (NULL != m_pAbsCenterManager)
		{
			m_pAbsCenterManager->RemoveViewListener(this);
		}
	}

	m_pAbsCenterManager = pAbsCenterManager;
	
	// 向ViewData注册自己
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
	
	return 0x10000000;                      // 必须保证与已经存在的GroupID & 计算值为FALSE
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
		// 刷新视图
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
		return;	// 非frame窗口下面，不能作出这些调用
	}

	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if ( NULL == pMainFrame )
	{
		return;
	}

	// 修改流程: 先切换到所需要的界面，然后在激活该窗口
	
	// 看是不是隐藏在Tab 下面

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
	// xl 0607 找到所属的GGTongView，如果F7了，且与当前activeIoView同frame/tabWnd，则必须先还原F7
	CGGTongView	*pF7View = (CGGTongView	*)pParentFrame->GetF7GGTongView();
	if ( NULL != pF7View
		&& pF7View != pView								// 不属于同一个GGTongView   // F7最大化了
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
		// 如果是报价表,要考虑板块的信息
		bool32 bFind = pReport->SetTabByBlockName(StrBlockName);

		if (!bFind)
		{
			// 可能这个板块删除了,那么显示第一个:
			pReport->GetTabWnd()->SetCurtab(0);
		}
		}
		//linhc 20100918添加默认选择当前的商品
		CMerch *pMerch = pReport->GetMerchXml();
		if( NULL != pMerch )
		{
            pReport->SetGridSelByMerch(pMerch);
		}	
	}

	// 激活视窗
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


// 边角的长短长度和离left,top边界的高度
void CIoViewBase::GetPolygon(CRect rect, CPoint ptsLeft[7] ,CPoint ptsRight[7], int iMaxLength, int iMinLength, int iCornerBorderH)
{
	// 左上边角
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

	// 右上边角
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
		// 星空图总是关心所有的实时报价更新
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
	// 加视图菜单
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
			//CNewMenu * pNewMenu = pMenu->AppendODPopupMenu(L"报价列表 ");
			StrReport = StrMenuName;
			
			if ( bLockSplit )
			{
				// 锁定状态下，是允许打开普通报价表
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
						
						if (StrBreedName == L"香港证券" || StrBreedName == L"新加坡期货" || StrBreedName == L"全球指数")
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
				// 非锁定，则选择新建的报价表类型
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
								StrItem = _T("默认报价表类型");
							}
							else
							{
								StrItem.Format(_T("报价表类型%d"), it->second.iId);
							}
						}

						// 过滤掉“默认”, 默认的iId等于-1
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
			// 新闻资讯不显示
			UINT uID = CIoViewManager::GetIoViewObject(i)->m_uID;
			if ( (uID < ID_PIC_HIDE_INMENU)  && (ID_PIC_NEWS != uID))		// 只有小于这个id才能在菜单中显示
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
		
	// 报价表下面加子菜单
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

	// 初始化视图的颜色.每个视图创建的时候调用这个函数,可能是一开始程序启动也可能是后来运行中新建一个视图
	bool32 bGetData = false;

	if ( CFaceScheme::EFERSameTypeIoView == CFaceScheme::Instance()->m_eFaceEffectRange)
	{
		// 得到这种视图的数据

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
		// 得到这个子窗口的数据

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
		// 没有取道数据,使用默认值:
		memset(m_aIoViewColor,0,sizeof(m_aIoViewColor));
		memcpyex(m_aIoViewColor,CFaceScheme::Instance()->m_aSysColors,sizeof(m_aIoViewColor));
		
		// 报价表的文字默认是白色
		if ( pIoView->IsKindOf(RUNTIME_CLASS(CIoViewReport)) )
		{
			m_aIoViewColor[ESCText] = CFaceScheme::Instance()->GetSysColor(ESCAmount);
		}

		// 分时图和闪电图的成交量默认是金黄色
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

	// 在从XML 中得到自定义的属性
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
	// 字体属性
	// NormalFontName
	CString StrAttriName = CFaceScheme::Instance()->GetKeyNameNormalFontName();
	
	char	aStrAttriName[100];
	UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
	
	const char * pcAttrValue = pElement->Attribute(aStrAttriName);
	
	
	if (NULL != pcAttrValue)
	{
		// 从XML 中读取中字体数据
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
			// 读文件出错时,从CFaceScheme 中获取默认值
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
		// 从XML 中读取小字体数据
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
			// 读文件出错时,从CFaceScheme 中获取默认值
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
		// 从XML 中读取大字体数据
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
			// 读文件出错时,从CFaceScheme 中获取默认值
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
		// 从XML 中读取大字体数据
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
			// 读文件出错时,从CFaceScheme 中获取默认值
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
	// 前面可能经过了很多的设置,只在这里与默认的系统颜色比较,确定需要保存到XML的颜色
	m_aIoViewColorSave.RemoveAll();	
	
	int32 i = 0;
	for ( i = 0 ; i < CFaceScheme::Instance()->GetSysColorObjectCount(); i++)
	{
		E_SysColor eSysColor = (E_SysColor)i;
		if ( m_aIoViewColor[i] != CFaceScheme::Instance()->GetSysColor(eSysColor) )
		{
			// 保存
			T_SysColorSave ColorSave;
			
			ColorSave.m_Color			= m_aIoViewColor[i];
			ColorSave.m_StrFileKeyName	= CFaceScheme::GetSysColorObject(i)->m_StrFileKeyName;
			
			m_aIoViewColorSave.Add(ColorSave);
		}		
	}

	CString StrThis;

	// 颜色
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
	// 前面可能经过了很多的设置,只在这里与默认的系统字体比较,确定需要保存到XML的字体
	m_aIoViewFontSave.RemoveAll();

	int32 i = 0;
	for ( i = 0 ; i < ESFCount; i++)
	{
		E_SysFont eSysFont = (E_SysFont)i;

		// LOGFONT发生变化即认为字体有变化(实际有可能facename后面无效字符变化- -)
		if ( 0 != memcmp(&m_aIoViewFont[i].m_stLogfont, CFaceScheme::Instance()->GetSysFont(eSysFont), sizeof(LOGFONT)) )
		{
			// 保存
			T_SysFontSave FontSave;
			FontSave.m_eSysFont		= E_SysFont(i);
			FontSave.m_lfLogFont	= m_aIoViewFont[i].m_stLogfont;
			
			m_aIoViewFontSave.Add(FontSave);
		}
	}

	CString StrThis;

	// 字体
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
// 字体相关


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

//	列表头
LOGFONT*		CIoViewBase::GetColumnExLF(bool bBold /*= false*/)
{
	return &m_aIoViewFont[ESFSmall].m_stLogfont;	
}
//	中文行字体
LOGFONT*		CIoViewBase::GetChineseRowExLF(bool bBold /*= false*/)
{
	return &m_aIoViewFont[ESFSmall].m_stLogfont;
}
//	序号字体
LOGFONT*		CIoViewBase::GetIndexRowExLF(bool bBold/*= false*/)
{
	return &m_aIoViewFont[ESFText].m_stLogfont;
}
//	数字行字体
LOGFONT*		CIoViewBase::GetDigitRowExLF(bool bBold/*= false*/)
{
	return &m_aIoViewFont[ESFNormal].m_stLogfont;	
}
//	商品代码行字体
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
	
	// 由于是单线程，so可以static
	// 每个IoViewBase都有可能有不同的字体，最惨的就是每次都要重建
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
		// 相同则不创建
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
	// 使用赋值
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
	// 颜色设置修改了.从faceschema 中取得数据,重绘

	CFaceScheme::Instance()->GetNowUseColors(m_aIoViewColor);
	RedrawWindow();

	/*
	COLORREF aColorTemp[ESCCount];

	CFaceScheme::Instance()->GetNowUseColors(aColorTemp);
	
	// 跟本地现有数据做比较,得到需要保存到XML 的颜色,也就是重设 m_aIoViewColorSave 的值

	m_aIoViewColorSave.RemoveAll();	

	for ( int32 i = 0 ; i < CFaceScheme::Instance()->GetSysColorObjectCount(); i++)
	{
		if ( m_aIoViewColor[i] != aColorTemp[i] )
		{
			// 保存
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
		
		// 套利K与套利分时返回到套利报价表
		// 这个应该在最前面的，不过0622时只有上述两个视图，就放在这里了
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
	
	E_ReportType eReportType = ERTStockHk;	// 默认值

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
	// 从本地文件读除权数据:
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

	
	// 取文件路径:
	if ( !pDoc->m_pAbsDataManager->GetPublishFilePath(EPFTWeight, pMerch->m_MerchInfo.m_iMarketId , pMerch->m_MerchInfo.m_StrMerchCode, StrFilePath) )
	{
		return false;
	}
	
	// 读数据:
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
	
	// 文件的CRC32 值
	uiCrc32 = CCodeFile::crc_32(acBuffer, iDataLen);
	
	// 处理数据
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
			// 日期不同, 直接返回			
			return true;				
		}
		else
		{
			// 日期相同, 
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
	// 交由MainFrame处理
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
		CUserBlockManager::Instance()->GetBlocks(aBlocks);	// 直接获取最新的
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
		// 标记处理
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
		// 重仓持股
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

	// 如果这里没有调用的话，需要重新编译各子视图的message map
	CControlBase::PostNcDestroy();
	{	// 如果有F10关注此视图 - 暂时关闭f10视图 xl 0719
		if ( NULL != pMainFrm->m_pDlgF10 && pMainFrm->m_pDlgF10->GetIoView() == this )
		{
			pMainFrm->m_pDlgF10->PostMessage(WM_CLOSE, 0, 0);
		}

	}

	//delete this;	// 删除自身 - 由于以前代码复杂，现不改变删除格局
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
		else if ( !bIgnoreMissedKey )	// 要求添加一个NULL
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
	// 一般父窗口隐藏导致窗口隐藏不会接收到该消息
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
	if ( KTimerIdCheckWindowShow == nIDEvent )	// 该Timer有可能会比较消耗资源，考虑其它方法实现过滤不可见时请求(OnIdle???)
	{
		if ( m_bHasIgnoreLastViewDataReq && IsNowCanReqData() )		// 最后一个要发送的请求没有发出去 - 简单标志有请求未发，不做请求队列
		{
			OnVDataForceUpdate();		// 刷新请求
			//ASSERT( !m_bHasIgnoreLastViewDataReq );	// 应该清除请求了的
			m_bHasIgnoreLastViewDataReq = false;	//	防止不停的timer
		}
	}
	CControlBase::OnTimer(nIDEvent);
}

bool32 CIoViewBase::IsNowCanReqData()
{
	CMPIChildFrame *pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());	// !!mainframe窗口也是frame
	if ( NULL == pFrame )
	{
		// 非页面下只需要判断是否隐藏
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
		// 激活页面最大化，下面的视图铁定看不见，所以不必发送数据了
		return false;
	}

	//---wangyongxue 同一Frame下看不见的视图也不必发送数据了
	if ( m_pIoViewManager )		// 多股同列可能为NULL
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
		// 视图父frame暂时暗示不应当请求数据，就不请求数据
		return false;
	}
	
	return m_bShowNow;
	//return IsWindowVisible();
}

bool32 CIoViewBase::IsNowCanRefreshUI()
{
	CMPIChildFrame *pFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());	// !!mainframe窗口也是frame
	if ( NULL == pFrame )
	{
		// 非页面下只需要判断是否隐藏
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
		// 激活页面最大化，下面的视图铁定看不见
		return false;
	}

	// 同一Frame下看不见的视图也不必刷新了
	if ( m_pIoViewManager )		// 多股同列可能为NULL
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

		// 最后一个有效请求已经发送 - 
		//	有可能前面的请求被过滤了，但是后面请求还在发送，导致只发送了部分请求
		//  不过由于基本上请求整个消息系统是同步序列的，出现可能性比较小
		//  m_bHasIgnoreLastViewDataReq = false;		
		return true;
	}
	else
	{
		if ( bLogIfCancel )
		{
			m_bHasIgnoreLastViewDataReq = true;		// 作出标记，在视图可见时，timer调用force update 发送请求
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
	if ( IsNowCanReqData() && m_bHasIgnoreLastViewDataReq )		// 最后一个要发送的请求没有发出去 - 简单标志有请求未发，不走请求队列
	{
		OnVDataForceUpdate();		// 刷新请求
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
	
	CMenu* pTempMenu = pPopMenu->GetSubMenu(L"插入内容");
	CNewMenu * pIoViewPopMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );
	ASSERT(NULL != pIoViewPopMenu );
	AppendIoViewsMenu(pIoViewPopMenu, IsLockedSplit());
	
	// 自选股

	// tab切换
	if ( GetIoViewManager() != NULL
		&& GetIoViewManager()->m_IoViewsPtr.GetSize() == 1 )
	{
		pPopMenu->RemoveMenu(IDM_IOVIEWBASE_TAB, MF_BYCOMMAND);	// 只有一个就不显示了
	}

	// 如果处在锁定分割状态，需要删除一些按钮
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
		// 普通按钮
		if ( menu.GetMenuItemCount() > 0 )
		{
			menu.AppendMenu(MF_SEPARATOR);
		}
		// 插入内容:
// 		CNewMenu* pIoViewPopMenu = menu.AppendODPopupMenu(L"插入内容");
// 		ASSERT(NULL != pIoViewPopMenu );
// 		AppendIoViewsMenu(pIoViewPopMenu);
// 		
		// 关闭内容:
		menu.AppendMenu(MF_STRING, IDM_IOVIEWBASE_CLOSECUR, _T("关闭内容"));
		
		// 内容切换:	
		if ( GetIoViewManager() != NULL
			&& GetIoViewManager()->m_IoViewsPtr.GetSize() > 1 )
		{
			menu.AppendMenu(MF_STRING, IDM_IOVIEWBASE_TAB, _T("内容切换 TAB"));
		}

		menu.AppendMenu(MF_SEPARATOR);
		
		// 全屏/恢复
		menu.AppendMenu(MF_STRING, IDM_IOVIEWBASE_F7, L"全屏/恢复 F7");
		menu.AppendMenu(MF_SEPARATOR);
		
		
		// 分割窗口
		CMenu menuSplit;
		menuSplit.CreatePopupMenu();
		menuSplit.AppendMenu( MF_STRING, IDM_IOVIEWBASE_SPLIT_TOP,    L"添加上视图");
		menuSplit.AppendMenu( MF_STRING, IDM_IOVIEWBASE_SPLIT_BOTTOM, L"添加下视图");
		menuSplit.AppendMenu( MF_STRING, IDM_IOVIEWBASE_SPLIT_LEFT,   L"添加左视图");
		menuSplit.AppendMenu( MF_STRING, IDM_IOVIEWBASE_SPLIT_RIGHT,  L"添加右视图");
		menu.AppendMenu(MF_POPUP, (UINT)menuSplit.m_hMenu, _T("分割窗口"));
		menuSplit.Detach();
		
		// 关闭窗口
		menu.AppendMenu(MF_STRING, IDM_IOVIEWBASE_CLOSE, _T("关闭窗口"));
		menu.AppendMenu(MF_SEPARATOR);
		
		// 风格设置
		menu.AppendMenu(MF_STRING, ID_SETTING, _T("风格设置"));
		
		// 版面布局
		menu.AppendMenu(MF_STRING, ID_LAYOUT_ADJUST, _T("版面布局"));

		// 如果处在锁定分割状态，需要删除一些按钮
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
	// 普通按钮
	if ( menu.GetMenuItemCount() > 0 )
	{
		menu.AppendMenu(MF_SEPARATOR);
	}
	// 加入自选股
	CMerch *pMerch = GetMerchXml();
	if ( NULL != pMerch )
	{
		// 4+1个按钮 所属板块 重仓持股基金 加入[我的自选] 加入自选板块 从板块删除
	//	menu.AppendODMenu(_T("所属板块     CTRL+R"), MF_STRING, IDM_BELONG_BLOCK);

//		menu.AppendODMenu(_T("重仓持股基金     36"), MF_STRING, ID_ZHONGCANGCHIGU);

		CString StrDefaultBlock = CUserBlockManager::Instance()->GetServerBlockName();
		if ( !StrDefaultBlock.IsEmpty() )
		{
			CString StrMenuItem;
			StrMenuItem.Format(_T("加入[%s] ALT+Z"), StrDefaultBlock.GetBuffer());
			menu.AppendODMenu(StrMenuItem, MF_STRING, ID_BLOCK_ADDTOFIRST);
		}
		menu.AppendODMenu(_T("加入自选板块 CTRL+Z"), MF_STRING, ID_BLOCK_ADDTO);
		menu.AppendODMenu(_T("从板块删除    ALT+D"), MF_STRING, ID_BLOCK_REMOVEFROM);
		
		menu.AppendMenu(MF_SEPARATOR);

		// 标记按钮
		CNewMenu *pMarkPopMenu = menu.AppendODPopupMenu(_T("标记当前商品"));
		ASSERT( NULL != pMarkPopMenu );
		pMarkPopMenu->AppendMenu(MF_STRING, ID_MARK_CANCEL, _T("取消标记"));
		pMarkPopMenu->AppendMenu(MF_SEPARATOR);
		pMarkPopMenu->AppendMenu(MF_STRING, ID_MARK_TEXT,	_T("标记文字  CTRL+Q"));
		pMarkPopMenu->AppendMenu(MF_STRING, ID_MARK_1,		_T("标记①"));
		pMarkPopMenu->AppendMenu(MF_STRING, ID_MARK_2,		_T("标记②"));
		pMarkPopMenu->AppendMenu(MF_STRING, ID_MARK_3,		_T("标记③"));
		pMarkPopMenu->AppendMenu(MF_STRING, ID_MARK_4,		_T("标记④"));
		pMarkPopMenu->AppendMenu(MF_STRING, ID_MARK_5,		_T("标记⑤"));
		pMarkPopMenu->AppendMenu(MF_STRING, ID_MARK_6,		_T("标记⑥"));
		pMarkPopMenu->AppendMenu(MF_SEPARATOR);
		pMarkPopMenu->AppendMenu(MF_STRING, ID_MARK_MANAGER, _T("标记管理"));
		menu.AppendMenu(MF_SEPARATOR);
	}
	// 插入内容:
	CNewMenu* pIoViewPopMenu = menu.AppendODPopupMenu(L"插入内容");
	ASSERT(NULL != pIoViewPopMenu );
	AppendIoViewsMenu(pIoViewPopMenu, IsLockedSplit());
	
	// 关闭内容:
	menu.AppendMenu(MF_STRING, IDM_IOVIEWBASE_CLOSECUR, _T("关闭内容"));
	
	// 内容切换:	
	if ( GetIoViewManager() != NULL
		&& GetIoViewManager()->m_IoViewsPtr.GetSize() > 1 )
	{
		menu.AppendMenu(MF_STRING, IDM_IOVIEWBASE_TAB, _T("内容切换 TAB"));
	}
	menu.AppendMenu(MF_SEPARATOR);
	
	// 全屏/恢复
	menu.AppendODMenu(L"全屏/恢复 F7", MF_STRING, IDM_IOVIEWBASE_F7);
	menu.AppendODMenu(L"", MF_SEPARATOR);
	
	
	// 分割窗口
	CNewMenu *pMenuSplit = (pNewMenu->AppendODPopupMenu(_T("分割窗口")));
	ASSERT( pMenuSplit != NULL );
	pMenuSplit->AppendMenu( MF_STRING, IDM_IOVIEWBASE_SPLIT_TOP,    L"添加上视图");
	pMenuSplit->AppendMenu( MF_STRING, IDM_IOVIEWBASE_SPLIT_BOTTOM, L"添加下视图");
	pMenuSplit->AppendMenu( MF_STRING, IDM_IOVIEWBASE_SPLIT_LEFT,   L"添加左视图");
	pMenuSplit->AppendMenu( MF_STRING, IDM_IOVIEWBASE_SPLIT_RIGHT,  L"添加右视图");
	
	
	// 关闭窗口
	menu.AppendMenu(MF_STRING, IDM_IOVIEWBASE_CLOSE, _T("关闭窗口"));
	menu.AppendMenu(MF_SEPARATOR);
	
	// 风格设置
	//menu.AppendMenu(MF_STRING, ID_SETTING, _T("风格设置"));
	menu.AppendODMenu(L"风格设置", MF_STRING, ID_SETTING);
	
	// 版面布局
	//menu.AppendMenu(MF_STRING, ID_LAYOUT_ADJUST, _T("版面布局"));
	menu.AppendODMenu(L"版面布局", MF_STRING, ID_LAYOUT_ADJUST);

	// 如果处在锁定分割状态，需要删除一些按钮
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
		// 不是frame下的或者没有ioviewmanager的，直接false
		return false;
	}

	CGGTongView *pViewActive = DYNAMIC_DOWNCAST(CGGTongView, pFrame->GetActiveView());	
	if ( NULL == pViewActive )
	{
		return false;	// 无激活view，false
	}

	{
		// 无manager，检查是否存在祖先manager
		CGGTongView *pView = GetParentGGtongView();
		if ( pView != pViewActive )
		{
			return false;		// 不属于激活view， false
		}
	}

	if ( pViewActive->m_IoViewManager.GetActiveIoView() == this )		// 可见就是的了
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

	// 合并时间
	for ( i=0; i + 1 < aTimesTmp.GetSize() ; i += 2 )	// 开收成对出现
	{
		const CGmtTime &TimeOpen  = aTimesTmp[i];
		const CGmtTime &TimeClose = aTimesTmp[i+1];
		
		int32 j=0;
		bool32 bDeal = false;
		for ( j=0; j +1 < aTradeTimes.GetSize() ; j += 2 )
		{
			CGmtTime &Time1 = aTradeTimes[j];
			CGmtTime &Time2 = aTradeTimes[j+1];

			if ( Time1 > TimeOpen && Time1 < TimeClose )		// 新开盘时间比 原有的早 && 新收盘时间比原开盘晚
			{
				Time1 = TimeOpen;
				bDeal = true;
			}
			if ( Time2 > TimeOpen && Time2 < TimeClose )		// 新收盘时间 比原收晚 && 原收 比现开晚
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

	// 是否需要继续合并？
	
	// trace
// #ifdef _DEBUG
// 	TRACE(_T("合并商品集合开收盘时间(GMT): \r\n"));
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
		TimeCloseAft += CGmtTimeSpan(0, 0, 2, 0);		// // 超过收盘时间2分钟了，可能存在服务器数据时间偏差
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
		// 检查今天是否为交易日
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

	// 看有没有夜盘的商品
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

	// 只有当所有的市场都不在交易区间才认为不在交易时间段
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
			// 无法获取交易日信息，或者不在最近的交易日内
			iMarketNotInTradeTimes++;
		}
	}

	// 存在任意在交易时间内的视为所有的都在交易时间内
	return aMarkets.GetCount() > iMarketNotInTradeTimes;	// 无市场时返回false
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
			// 加入到默认自选板块
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
			// 选择板块
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
			// 从板块中删除
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
			// 2、深圳中小板调用中小板指数
			m_pAbsCenterManager->GetMerchManager().FindMerch(/*L"399005"*/L"399101", 1000, pMerchExp);
		}
		else if ( 1008 == pMerch->m_Market.m_MarketInfo.m_iMarketId )
		{
			// 3、深圳创业板调用创业板指数
			m_pAbsCenterManager->GetMerchManager().FindMerch(/*L"399006"*/L"399102", 1000, pMerchExp);
		}
		else
		{
			// 4、其余的全都调用深成指
			m_pAbsCenterManager->GetMerchManager().FindMerch(L"399001", 1000, pMerchExp);
		}	
	}
	
	// 默认都调上证指数
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
		return IsAttendData(pMerch, eDataServiceType);		// 需要它的推送，并且关注该类型
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
			// 2、深圳中小板调用中小板指数
			pAbsCenterManager->GetMerchManager().FindMerch(/*L"399005"*/L"399101", 1000, pMerchExp);
		}
		else if ( 1008 == pMerch->m_Market.m_MarketInfo.m_iMarketId )
		{
			// 3、深圳创业板调用创业板指数
			pAbsCenterManager->GetMerchManager().FindMerch(/*L"399006"*/L"399102", 1000, pMerchExp);
		}
		else
		{
			// 4、其余的全都调用深成指
			pAbsCenterManager->GetMerchManager().FindMerch(L"399001", 1000, pMerchExp);
		}	
	}
	
	// 默认都调上证指数
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
	// 上证指数
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
		// 无效类型，不绘制
		DrawMarkItem.eDrawMarkFlag = EDMF_None;
		return false;
	}

	// 从线16到线15...
	DrawMarkItem.eDrawMarkFlag = EDMF_Top;
	E_SysColor eColor = (E_SysColor)(ESCGuideLine16 - MarkData.m_eType);
	DrawMarkItem.clrMark = GetIoViewColor(eColor);
	if ( MarkData.m_eType == EMT_Text )
	{
		DrawMarkItem.strMark = _T("T");	// 文字绘制一个T
	}
	else
	{
		DrawMarkItem.strMark = MarkData.m_StrTip;	// 使用内部的①...
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

	// 绘制 浦发银行(600000) 部分
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
	CSize sizeTitle = dc.GetTextExtent(StrTitle);	// 不计算上标区域
	rcTitle.left = rcText.left + (rcText.Width() - sizeTitle.cx)/2;
	rcTitle.left = max(rcTitle.left, rcText.left);

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(clrName);
	dc.DrawText(StrMerchName, rcTitle, DT_SINGLELINE |DT_LEFT |DT_VCENTER);
	CSize sizeName = dc.GetTextExtent(StrMerchName);
	rcTitle.left += sizeName.cx + dc.GetTextCharacterExtra();

	if ( bDrawCode )
	{
		// 绘制单独的 (code)
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
		// 有标记需要绘制
		// 绘制标记
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
		// 加入自选股
		CMerch *pMerch = GetMerchXml();
		pCmdUI->Enable(NULL != pMerch);
		return;
	}
	else if ( nID >= ID_MARK_TEXT && nID <= ID_MARK_6 )
	{
		// 标记状态
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

	// 其它
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

	// 默认的处理, 按ViewData 中的顺序来取商品
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
			sm_aBackMerchs.RemoveAt(iUpper-1, 2);	// 同时移除最后一个与当前的商品
		}
		return pMerch;	// 倒数第二个为最后记录的商品
	}

	return NULL;
}

void CIoViewBase::AddLastCurrentMerch( CMerch *pMerch )
{
	if ( NULL == pMerch )
	{
		return;
	}

	// 后向商品队列中最后一个存储的是当前的商品

	const int32 iUpper = sm_aBackMerchs.GetUpperBound();
	if ( iUpper >= 0 && pMerch == sm_aBackMerchs[iUpper] )
	{
		return;	// 同商品没变
	}

	for ( int32 i=iUpper-1; i >=0; --i )
	{
		if ( pMerch == sm_aBackMerchs[i] )
		{
			sm_aBackMerchs.RemoveAt(i);
			break;	// 保证只有一个商品在后向队列里面, 如果该商品重复被调用，则将其加入到最后
		}
	}

	sm_aBackMerchs.Add(pMerch);
}

CMerch* CIoViewBase::PeekLastPrevMerch( bool32 bRemove/*=false*/ )
{
	ASSERT( 0 );	// 未实现前向调用
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
		SetHasIgnoreLastViewDataReqFlag(true);		// 设置忽略了请求标志
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

	// 获取当前商品(目前只支持单个商品)
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

	// 找到这个商品的预警信息, 初始化对话框
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
			// 修改
			pAlarmCenter->ModifyMerchAlarms(pMerch, stAlarmConditions);
		}
		else
		{			
			// 新增
			pAlarmCenter->AddMerchAlarms(pMerch, stAlarmConditions);
			pAlarmCenter->ToXml();
		}

		// 开启预警
		pAlarmCenter->SetAlarmFlag(true);
	}
}
