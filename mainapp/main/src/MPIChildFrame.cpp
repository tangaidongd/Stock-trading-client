/*********************************************************
* Multi-Page Interface
* Version: 1.2
* Date: September 2, 2003
* Author: Michal Mecinski
* E-mail: mimec@mimec.w.pl
* WWW: http://www.mimec.w.pl
*
* Copyright (C) 2003 by Michal Mecinski
*********************************************************/

#include "stdafx.h" 
#include "afxpriv.h"
#include "BiSplitterWnd.h"
#include "BarSplitWnd.h"
#include "MPIChildFrame.h"

#include "TabSplitWnd.h"
#include "GGTongView.h"
#include "keyboarddlg.h"

#include "ShareFun.h"
#include "GdiPlusTS.h"

#include "facescheme.h"
	
#include "memdc.h"
#include  "tinyxml.h"

#include "WspManager.h"

#include "IoViewManager.h"

#ifdef TIXML_USE_STL
#include <iostream>
#include <sstream>
using namespace std;
#else
#include <stdio.h>
#endif



#if defined( WIN32 ) && defined( TUNE )
#include <crtdbg.h>
_CrtMemState startMemState;
_CrtMemState endMemState;
#endif


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// #ifdef _DEBUG
// #pragma comment(lib, "tinyxmld.lib")
// #else
// #pragma comment(lib, "tinyxml.lib")
// #endif
/*
	error 14: (Error -- Symbol 'KMsgChildFrameMaxSize' previously defined (line 330, file .\main\src\MainFrm.cpp))
*/
//lint -e{14}

const UINT KMsgChildFrameMaxSize = WM_USER + 2103;
const UINT KMsgChildFrameMinSize = WM_USER + 2104;

const UINT KTimerIdRefreshCaption	  = 123456;
const UINT KTimerPeriodRefreshCaption = 200;

const UINT KTimerIdRecalcLayout = 123458;
const UINT KTimerPeriodRecalcLayout = 20;

const int KFrameSpace = 9;

IMPLEMENT_DYNCREATE(CMPIChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CMPIChildFrame, CMDIChildWnd)
ON_WM_CREATE()
ON_WM_DESTROY()
ON_WM_CLOSE()
ON_WM_SIZE()
ON_WM_TIMER()
ON_WM_WINDOWPOSCHANGED()
ON_WM_MOUSEMOVE()
ON_WM_NCMOUSEMOVE()
ON_WM_NCPAINT()
ON_WM_NCACTIVATE()
ON_WM_NCCALCSIZE()
ON_WM_NCHITTEST()
ON_WM_NCLBUTTONDOWN()
ON_WM_NCLBUTTONUP()
ON_WM_NCLBUTTONDBLCLK()
ON_WM_GETMINMAXINFO()
ON_WM_ERASEBKGND()
ON_MESSAGE(KMsgChildFrameMaxSize,OnChildFrameMaxSize)
ON_MESSAGE(KMsgChildFrameMinSize,OnChildFrameMinSize)
ON_MESSAGE(GGT_WM_NCMOUSELEAVE, OnNcMouseLeave)
// Text
ON_MESSAGE(WM_SETTEXT, OnSetWindowText)
ON_MESSAGE(WM_GETTEXT, OnGetWindowText)
ON_MESSAGE(WM_GETTEXTLENGTH, OnGetWindowTextLength)

ON_MESSAGE(UM_SETGROUPID,OnSetGroupID)
ON_MESSAGE(UM_ISKINDOFCMPIChildFrame,OnIsKindOfCMpiChildFram)
ON_MESSAGE(UM_IsHideMode,OnIsHideMode)
ON_MESSAGE(UM_DoGGTongViewElement,OnDoGGTongViewElement)
ON_MESSAGE(UM_DoBiSplitElement,OnDoBiSplitElement)
ON_MESSAGE(UM_DelSplit,OnDelSplit)
ON_MESSAGE(UM_IsLockedSplit,OnIsLockedSplit)
ON_MESSAGE(UM_SetF7AutoLock,OnSetF7AutoLock)
ON_MESSAGE(UM_IsF7AutoLock,On_IsF7AutoLock)
END_MESSAGE_MAP()


//XML 数据
static const char * KStrElementValue				= "ChildFrame";
static const char * KStrElementAttriName			= "name";
static const char * KStrElementAttrGroupId			= "GroupId";
static const char * KStrElementAttriZIndex			= "zindex";
static const char * KStrElementAttriFlag			= "flag";
static const char * KStrElementAttriShowCmd			= "showCmd";
static const char * KStrElementAttriPtMinX			= "ptMinPosition.x";
static const char * KStrElementAttriPtMinY			= "ptMinPosition.y";
static const char * KStrElementAttriPtMaxX			= "ptMaxPosition.x";
static const char * KStrElementAttriPtMaxY			= "ptMaxPosition.y";

static const char * KStrElementAttriPtNorLeft		= "NormalPosition.left";
static const char * KStrElementAttriPtNorRight		= "NormalPosition.right";
static const char * KStrElementAttriPtNorTop		= "NormalPosition.top";							
static const char * KStrElementAttriPtNorBottom		= "NormalPosition.bottom";

static const char * KStrEleAttriLockedSplit			= "lockSplit";


// 颜色字体等属性
//////////////////////////////////////////////////////////////////////////
/// 颜色
static const char * KStrElementAttriStyleIndex		= "StyleIndex";
//////////////////////////////////////////////////////////////////////////

const int32 KChildFrameCaptionHeight    = 14; // 原15，现由于边框自己绘制，可以缩小边框

CMPIChildFrame* CMPIChildFrame::m_pThis = NULL;

CMPIChildFrame::CMPIChildFrame()
:m_pContext(NULL)
{
	m_rectLast.top		= 150;
	m_rectLast.left		= 150;
	m_rectLast.right	= 600;
	m_rectLast.bottom	= 600;
	
	m_bFixTitle			= false;
	m_bMaxSize			= false;
	m_bMinSize			= false;
	m_bHaveTab			= false;
	m_bHistoryChildFrame= false;

	m_iStyleIndex		= 1;
	m_StrTitle			= AfxGetApp()->m_pszAppName;
	m_iGroupID			= 1;
	m_pThis				= this;

	m_bWindowTextValid = false;

	m_bLockedSplit	= false;	// 默认不锁定分割

	m_bF7AutoLock = false;		// 默认不是F7锁定下
	m_bF7LockSplitOrg = false;

	m_pF7GGTongViewXml = NULL;

	m_bHideMode = false;	// 不在xml时隐藏
	m_bEnableRequestData = true;	// 允许数据请求
	
	IntialImageResource();
	//m_listControlBars.RemoveAll();
	m_aTest.RemoveAll();
}

CMPIChildFrame::~CMPIChildFrame()
{
//  ..fangz 1124 BUG
// 	POSITION pos = m_listSplitters.GetHeadPosition();
// 	while (pos)
// 		delete m_listSplitters.GetNext(pos);
// 
	DEL(m_pImageButtons);
	DEL(m_pButtonLayOut);
	DEL(m_pCaptionActive);
	DEL(m_pCaptionInActive);

	if (NULL != m_pContext)
	{
		delete m_pContext;
		m_pContext = NULL;
	}

// 	for ( int32 i = 0; i < m_listSplitters.GetSize(); i++)
// 	{
// 		CWnd* pWnd = m_listSplitters.GetAt(i);
// 		
// 		HWND hWnd = pWnd->GetSafeHwnd();
// 
// 		if ( NULL != pWnd && NULL != hWnd && IsWindow(hWnd) )
// 		{
// 			DEL(pWnd);
// 		}
// 	}
}

BOOL CMPIChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// disable closing or resizing the child window
	// zhangbo 0310 #add //
	cs.style &= ~WS_SYSMENU;
	cs.style &= ~WS_CAPTION;
	cs.style &= ~FWS_ADDTOTITLE;
	// 保留Min/MaxBox，在最大化，最小化行为时，系统可以自动调节 - xl
	//cs.style &= ~WS_MINIMIZEBOX;
	//cs.style &= ~WS_MAXIMIZEBOX;

	if(!CMDIChildWnd::PreCreateWindow(cs))
		return FALSE;
	return TRUE;
}
 
#ifdef _DEBUG
void CMPIChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CMPIChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}
#endif //_DEBUG

void CMPIChildFrame::IntialImageResource()
{
	m_eCurrentShowState		= ECSSNormal;
	
	m_pImageButtons			= NULL;
	m_pButtonLayOut			= NULL;
	m_pCaptionActive		= NULL;
	m_pCaptionInActive		= NULL;
	
	
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_CHILDFRAME_BTN, L"PNG", m_pImageButtons))
	{
		//ASSERT(0);
		m_pImageButtons = NULL;
	}	
	
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_CHILDFRAME_BTN_LAYOUT, L"PNG", m_pButtonLayOut))
	{
		//ASSERT(0);
		m_pImageButtons = NULL;
	}	
		
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_CHILDFRAME_CAPTION_ACTIVE, L"PNG", m_pCaptionActive))
	{
		//ASSERT(0);
		m_pImageButtons = NULL;
	}
	
	if ( !ImageFromIDResource(AfxGetResourceHandle(),IDR_PNG_CHILDFRAME_CAPTION_INACTIVE, L"PNG", m_pCaptionInActive))
	{
		//ASSERT(0);
		m_pImageButtons = NULL;
	}
}

void CMPIChildFrame::ActivateFrame(int nCmdShow) 
{
	CMDIChildWnd::ActivateFrame(nCmdShow);
}

void CMPIChildFrame::OnClose() 
{
	CMDIChildWnd::OnClose();
}

void CMPIChildFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	GetMDIFrame()->OnUpdateFrameTitle(FALSE);
	
	//  display child frame title instead of the document's title
	//	AfxSetWindowText(m_hWnd, m_strTitle);
}

int CMPIChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	
	// 去除OverlappedWindow，加入windowedge
	ModifyStyleEx(WS_EX_OVERLAPPEDWINDOW, WS_EX_WINDOWEDGE, 0);
	//ModifyStyle(0xc000,0);

	//
	CMenu* pSysMenu = GetSystemMenu(FALSE);	
	if(NULL != pSysMenu)
	{
		BOOL bRemove;
		bRemove = pSysMenu->RemoveMenu(SC_MINIMIZE,MF_BYCOMMAND);
		bRemove = pSysMenu->RemoveMenu(SC_MAXIMIZE,MF_BYCOMMAND);
		bRemove = pSysMenu->RemoveMenu(SC_CLOSE,MF_BYCOMMAND);
	}

	//CMainFrame* pFrame = (CMainFrame*)GetMDIFrame();
	//m_iGroupID	= pFrame->GetValidGroupID();

	// 修改重要: 股大将军等版本的新窗口默认分组为1, 不再为递增模式 xl 1207
	m_iGroupID = 1;

	return 0;
}

int32 CMPIChildFrame::GetGroupID()
{
	return m_iGroupID;
}

void CMPIChildFrame::SetGroupID(int32 iID, bool32 bChangeSub/* = false*/, bool32 bChangeMerch/* = false*/)
{
	if ( iID < 0 || iID > MAXGROUPID)
	{
		return;
	}

	if ( bChangeSub )	// 变更所有子 ioviewmanager
	{
		// 遍历
		
	}

	m_iGroupID = iID;
}

BOOL CMPIChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	m_pContext = new CMPICreateContext;
	memcpyex(m_pContext, (CMPICreateContext *)pContext,sizeof(CMPICreateContext));

	int nPos = 0;
	CreateClientHelper(nPos, NULL, 0, (CMPICreateContext*)pContext);

	return TRUE;
}

void CMPIChildFrame::OnUpdateFrameMenu(BOOL bActivate, CWnd* pActivateWnd, HMENU hMenuAlt)
{
	CMainFrame* pFrame = (CMainFrame*)GetMDIFrame();

	if (hMenuAlt == NULL)
		hMenuAlt = m_hMenuShared;
	// zhangbo 0115
	// change the menu in the main frame's rebar control
// 	if (hMenuAlt != NULL && bActivate)
// 		pFrame->UpdateMenu(hMenuAlt);
// 	else if (hMenuAlt != NULL && !bActivate && pActivateWnd == NULL)
// 		pFrame->UpdateMenu(pFrame->m_hMenuDefault);

	// 根据当前的锁定状态，变更mainframe的分析菜单
	if ( NULL != pFrame )
	{
		pFrame->ChangeAnalysisMenu(IsLockedSplit());
		//pFrame->DrawMenuBar();
	}
}

BOOL CMPIChildFrame::CreateClientHelper(int &nPos, CWnd *pParent, int nID, CMPICreateContext* pContext)
{	
	return TRUE;
}

void CMPIChildFrame::AddSplit(CView *pViewCur, CRuntimeClass *pNewViewClass, E_SplitDirection eSD, CMPICreateContext* pContext,double dRatio)
{
	ASSERT(NULL != pViewCur);
	ASSERT(NULL != pContext);
	ASSERT_POINTER(pNewViewClass, CRuntimeClass);
	
	if (SD_LEFT		!= eSD &&
		SD_RIGHT	!= eSD && 
		SD_TOP		!= eSD && 
		SD_BOTTOM	!= eSD &&
		SD_TAB		!= eSD)
	{
		return;
	}

	if (!pViewCur->IsKindOf(RUNTIME_CLASS(CView)))
		return;

	CWnd *pParent = pViewCur->GetParent();
	if (NULL == pParent)
		return;
	
	if (SD_TAB != eSD)
	{
		int iRowNew = 1, iColNew = 1;
		int iPaneRowNew = 0, iPaneColNew = 0;

		if (SD_LEFT	== eSD || SD_RIGHT == eSD)
		{
			iColNew = 2;
			if (SD_RIGHT == eSD)
				iPaneColNew = 1;
		}
		else if (SD_TOP == eSD || SD_BOTTOM == eSD)
		{
			iRowNew = 2;
			if (SD_BOTTOM == eSD)
				iPaneRowNew = 1;
		}

		// ...fangz 1123
		BSW		eDirection = VSPLITTER;
		int32	iPaneId	   = BSW_FIRST_PANE;
		
		if ( SD_LEFT == eSD || SD_RIGHT == eSD )
		{
			eDirection = VSPLITTER;
		}
		else if ( SD_TOP == eSD || SD_BOTTOM == eSD )
		{
			eDirection = HSPLITTER;
		}

		//
		if (SD_LEFT == eSD || SD_TOP == eSD)
		{
			iPaneId = BSW_FIRST_PANE;
		}
		else if (SD_BOTTOM == eSD || SD_RIGHT == eSD)
		{
			iPaneId = BSW_SECOND_PANE;
		}
		//

		// 
		if (pParent->IsKindOf(RUNTIME_CLASS(CBiSplitterWnd)))
		{
			static UINT	s_uiSplitID = 100;

			CBiSplitterWnd* pParentSplitWnd = (CBiSplitterWnd*)pParent;
			int iIDPane = pParentSplitWnd->GetViewPaneID(pViewCur);
			
			if ( -1 == iIDPane )
			{
				return;
			}
			
			// 新建的分割条
			CBiSplitterWnd* pNewSplit = (CBiSplitterWnd*)pParentSplitWnd->CreateView(RUNTIME_CLASS(CBiSplitterWnd), pViewCur->GetDlgCtrlID(), eDirection | PROPORTIONAL);
			if ( NULL == pNewSplit )
			{
				return;
			}
			
			// ID 号增 1
			++s_uiSplitID;		
			
			// 将新建的分割条设为原来的儿子
			pNewSplit->SetParent(pParentSplitWnd);
			pParentSplitWnd->AssignViewToPane(pNewSplit->GetDlgCtrlID(), iIDPane);		
			
			// 新建Pane
			pNewSplit->CreatePane(BSW_FIRST_PANE, THIN_BUTTON, L"0");
			pNewSplit->CreatePane(BSW_SECOND_PANE, THIN_BUTTON, L"1");	
			
			// 新建的视图
			CGGTongView* pViewNew  = (CGGTongView*)pNewSplit->CreateView(RUNTIME_CLASS(CGGTongView), 1);
			if ( NULL == pViewNew )
			{
				return;
			}
			
			pViewCur->SetParent(pNewSplit);
			pViewNew->SetParent(pNewSplit);
			
			//
			pViewCur->SetDlgCtrlID(0);
			int iID1 = pViewCur->GetDlgCtrlID();		
			int iID2 = pViewNew->GetDlgCtrlID();
			
			pNewSplit->AssignViewToPane(iID1, (1-iPaneId));
			pNewSplit->AssignViewToPane(iID2, iPaneId);	
			//

			//
// 			pParentSplitWnd->RecalcLayout();
// 			pParentSplitWnd->Invalidate();
			
			//
			pNewSplit->SetActivePane(iPaneId);
			//pNewSplit->RecalcLayout();
			pNewSplit->SetRadioXml(dRatio);
			pParentSplitWnd->RecalcLayout();
			pParentSplitWnd->Invalidate();
			

			// 保存
			m_listSplitters.Add(pNewSplit);
		}
		else if (pParent->IsKindOf (RUNTIME_CLASS(CTabSplitWnd)))
		{
			//////////////////////////////////////////////////////////////////////////
			// ...fangz 1123
			CTabSplitWnd *pParentSplit = (CTabSplitWnd *)pParent;
			
			//
			CBiSplitterWnd* pSplitter = new CBiSplitterWnd;
			if ( NULL == pSplitter )
			{
				return;
			}
			
			// 创建分割条
			if ( !pSplitter->Create(pParent, eDirection | PROPORTIONAL, pViewCur->GetDlgCtrlID()) )
			{
				return;
			}
			
			// 创建两个容器
			if ( !pSplitter->CreatePane(BSW_FIRST_PANE, THIN_BUTTON, L"2") ) 
			{
				pSplitter->DestroyWindow();
				DEL(pSplitter);
				return;
			}
			
			if ( !pSplitter->CreatePane(BSW_SECOND_PANE, THIN_BUTTON, L"1") )
			{
				pSplitter->DestroyWindow();
				DEL(pSplitter);
				return;
			}
			
			// 新建一个视图出来:
			CGGTongView* pViewAdd = (CGGTongView*)pSplitter->CreateView(RUNTIME_CLASS(CGGTongView), 1);
			
			if ( NULL == pViewAdd )
			{
				pSplitter->DestroyWindow();
				DEL(pSplitter);
				return;
			}
			
			// 设置互相之间的关联
			pViewCur->SetDlgCtrlID(0);
			pSplitter->SetParent(pParentSplit);
			pViewCur->SetParent(pSplitter);
			pViewAdd->SetParent(pSplitter);
			
			pSplitter->AssignViewToPane(pViewCur->GetDlgCtrlID(), (1 - iPaneId));
			pSplitter->AssignViewToPane(pViewAdd->GetDlgCtrlID(), iPaneId);
			
			// 去掉视图的边框属性
			pViewCur->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
			pViewAdd->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
			
			//
// 			pParentSplit->RecalcLayout();
// 			pParentSplit->Invalidate();		
			
			//
			pSplitter->SetActivePane(iPaneId);					
// 			pSplitter->RecalcLayout();			
// 			pSplitter->SetRadioX(dRatio);
// 			pSplitter->UpdateWindow();
			pSplitter->SetRadioXml(dRatio);
			pParentSplit->RecalcLayout();
			pParentSplit->Invalidate();
			
			// 保存
			m_listSplitters.Add(pSplitter);
		}
		else if (pParent->IsKindOf (RUNTIME_CLASS(CMPIChildFrame)))
		{			
			//////////////////////////////////////////////////////////////////////////
			// ...fangz 1123
			CBiSplitterWnd* pSplitter = new CBiSplitterWnd;
			if ( NULL == pSplitter )
			{
				return;
			}
			
			// 创建分割条
			if ( !pSplitter->Create(this, eDirection | PROPORTIONAL) )
			{
				return;
			}

			// 创建两个容器
			if ( !pSplitter->CreatePane(BSW_FIRST_PANE, THIN_BUTTON, L"2") ) 
			{
				pSplitter->DestroyWindow();
				DEL(pSplitter);
				return;
			}

			if ( !pSplitter->CreatePane(BSW_SECOND_PANE, THIN_BUTTON, L"1") )
			{
				pSplitter->DestroyWindow();
				DEL(pSplitter);
				return;
			}
			
			// 新建一个视图出来:
			CGGTongView* pViewAdd = (CGGTongView*)pSplitter->CreateView(RUNTIME_CLASS(CGGTongView), 1);
			
			if ( NULL == pViewAdd )
			{
				pSplitter->DestroyWindow();
				DEL(pSplitter);
				return;
			}

			// 设置互相之间的关联
			pViewCur->SetDlgCtrlID(0);
			pSplitter->SetParent(this);
			pViewCur->SetParent(pSplitter);
			pViewAdd->SetParent(pSplitter);
		
			pSplitter->AssignViewToPane(pViewCur->GetDlgCtrlID(), (1 - iPaneId));
			pSplitter->AssignViewToPane(pViewAdd->GetDlgCtrlID(), iPaneId);

			// 去掉视图的边框属性
			pViewCur->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
			pViewAdd->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
			
			//
// 			RecalcLayout();
// 			UpdateWindow();	

			pSplitter->SetActivePane(iPaneId);					
			pSplitter->SetRadioXml(dRatio);
// 			pSplitter->RecalcLayout();
// 			pSplitter->UpdateWindow();			
// 			pSplitter->SetRadio(dRatio);			
			RecalcLayout();
			Invalidate();

			// 保存			
			m_listSplitters.Add(pSplitter);
		}
	}
	else	// 增加标签
	{
		CWnd *pWndTop2 = pViewCur;
		pParent = pWndTop2->GetParent();
		
		while (1)
		{
			if (NULL == pParent)
				return;

			if (pParent->IsKindOf(RUNTIME_CLASS(CTabSplitWnd)))
				break;

			if (pParent->IsKindOf(RUNTIME_CLASS(CMPIChildFrame)))
				break;

			pWndTop2 = pParent;
			pParent = pParent->GetParent();
		}

		if (NULL == pParent)
			return;

		CMainFrame* pFrame = (CMainFrame*)GetMDIFrame();
		int32 iGroupID	 = pFrame->GetValidGroupID();

		if (pParent->IsKindOf(RUNTIME_CLASS(CTabSplitWnd)))	// 找到了一个标签， 在这个标签的基础上添加一个
		{
			CTabSplitWnd *pTabWnd = (CTabSplitWnd *)pParent;

			int32 iItemCount = pTabWnd->m_wndTabCtrl.GetCount();
	
			pTabWnd->m_wndTabCtrl.Addtab(AfxGetApp()->m_pszAppName,AfxGetApp()->m_pszAppName,AfxGetApp()->m_pszAppName);

			// 设置分组信息			
			pTabWnd->AddGroupID(iGroupID);
			pTabWnd->m_iCurGroupID = iGroupID;
			// create view object and window
			CView *pViewAdd = (CView*)pNewViewClass->CreateObject();
			if (!pViewAdd->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW & ~WS_BORDER, CRect(0,0,0,0), pTabWnd, iItemCount + 1, pContext))
				return;

			//////////////////////////////////////////////////////////////////////////
			//  增加一个标签页,同时增加这个标签页的活动视图数组,这个只有一个View,设为活动视图

			int32 iCounts = pTabWnd->m_wndTabCtrl.GetCount();
			pTabWnd->SetActiveChildView(iCounts-1,pViewAdd);
			ASSERT( pTabWnd->m_ActiveViews.GetSize() == iCounts);
			pTabWnd->m_wndTabCtrl.SetCurtab(iCounts-1);
			pTabWnd->RecalcLayout();		
		}
		else if (pParent->IsKindOf(RUNTIME_CLASS(CMPIChildFrame)))	// 没有标签， 建标签
		{
			// 记录下当前tab页对应的活动视图
			CView *pOldActiveView = GetActiveView();			
			//
			CTabSplitWnd* pTabWnd = new CTabSplitWnd;
		
			// ...as the childframe's child
			if (!pTabWnd->CreateStatic(this, 2, 1))
				return;
			
			// create the tab control and load tabs from toolbar resource
			if (!pTabWnd->CreateEmptyTabs())
				return;	
			

			// 记录下当前tab页对应的活动视图	
			ASSERT( NULL!= pOldActiveView);
			pTabWnd->m_ActiveViews.RemoveAll();
			pTabWnd->m_wndTabCtrl.Addtab(AfxGetApp()->m_pszAppName, AfxGetApp()->m_pszAppName, AfxGetApp()->m_pszAppName);			
			pTabWnd->SetActiveChildView(0, pOldActiveView);			
			
			// 设置分组信息,第一个标签用子窗口的ID,第二个用新申请到的ID
			pTabWnd->m_aGroupIDs.RemoveAll();
			pTabWnd->AddGroupID(m_iGroupID);
			
			iGroupID = pFrame->GetValidGroupID();
			pTabWnd->AddGroupID(iGroupID);			
			//
			pTabWnd->m_iCurGroupID = iGroupID;
			// set top2 window as child of pTabWnd

			pWndTop2->SetDlgCtrlID(1);
			pWndTop2->SetParent(pTabWnd);

			// create view object and window
			CView *pViewAdd = (CView*)pNewViewClass->CreateObject();
			if (!pViewAdd->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW & ~WS_BORDER, CRect(0,0,0,0), pTabWnd, 2, pContext))
				return;
			
			// 去掉视图的边框属性
			pWndTop2->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);

			// 活动视图
			pTabWnd->m_wndTabCtrl.Addtab(AfxGetApp()->m_pszAppName, AfxGetApp()->m_pszAppName, AfxGetApp()->m_pszAppName);
			pTabWnd->SetActiveChildView(1,pViewAdd);
			
			int32 iCounts = pTabWnd->m_wndTabCtrl.GetCount();
			ASSERT( pTabWnd->m_ActiveViews.GetSize() == iCounts);


			// 设置当前显示tab 
			pTabWnd->SetPage(0);	
			pTabWnd->m_wndTabCtrl.SetCurtab(0);
			pTabWnd->m_iCurGroupID = m_iGroupID;

			((CMPIChildFrame *)pParent)->RecalcLayout();
			pTabWnd->RecalcLayout();			
		}
	}
}

void CMPIChildFrame::DelSplit(CView *pViewCur)
{ 
	ASSERT(NULL != pViewCur);
	
	if (!pViewCur->IsKindOf(RUNTIME_CLASS(CView)))
		return;

	CWnd *pParent = pViewCur->GetParent();
	if ( NULL == pParent )
	{
		return;
	}

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	
 	if ( pViewCur->IsKindOf(RUNTIME_CLASS(CGGTongView)) )
 	{
 		CGGTongView* pView = (CGGTongView*)pViewCur;
 		if ( pView->GetMaxF7Flag() )
 		{
 			pMainFrame->OnProcessF7(pView);
 		}

		// xl 0608 此空白GGTongView已经删除，MainFrame如果需要Empty需要另外找吧
		if ( pMainFrame->GetEmptyGGTongView() == pView )
		{
			pMainFrame->SetEmptyGGTongView(NULL);
		}
 	}
	
	CWnd* pGrandPa = pParent->GetParent();
	if ( NULL == pGrandPa )
	{
		return;
	}

	// 
	if (pParent->IsKindOf(RUNTIME_CLASS(CBiSplitterWnd)))
	{
		CBiSplitterWnd* pParentBisplittwnd = (CBiSplitterWnd*)pParent;

		// 		
		int32 iDelPaneID   = pParentBisplittwnd->GetViewPaneID(pViewCur);
		
		if ( BSW_FIRST_PANE != iDelPaneID && BSW_SECOND_PANE != iDelPaneID )
		{
			// iD 非法
			return;
		}

		CWnd* pToDel	   = pViewCur;
		CWnd* pToLeave	   = pParentBisplittwnd->GetPaneView(1-iDelPaneID);
		
		if ( NULL == pToLeave )
		{
			return;
		}

		if ( pGrandPa->IsKindOf(RUNTIME_CLASS(CMPIChildFrame)) )
		{
			// 父亲是切分,父亲的父亲是 CMPIChildFrame. 删了以后,没有切分窗口了			
			pToLeave->SetParent(pGrandPa);			
			pToLeave->SetDlgCtrlID(pParentBisplittwnd->GetDlgCtrlID());

			pParentBisplittwnd->DestroyWindow();

			// 设置激活视图
			CView *pFirstActiveView = CMainFrame::FindGGTongView(pToLeave);
			if (NULL != pFirstActiveView)
			{
				SetActiveView(pFirstActiveView);
			}			
		}
		else if ( pGrandPa->IsKindOf(RUNTIME_CLASS(CTabSplitWnd)) )
		{
			// 父亲是切分,父亲的父亲是 TabSplitWnd.
			pToLeave->SetParent(pGrandPa);			
			pToLeave->SetDlgCtrlID(pParentBisplittwnd->GetDlgCtrlID());
			
			pParentBisplittwnd->DestroyWindow();
			
			// 设置激活视图
			CView *pFirstActiveView = CMainFrame::FindGGTongView(pToLeave);
			if (NULL != pFirstActiveView)
			{
				SetActiveView(pFirstActiveView);
			}		
		}
		else if ( pGrandPa->IsKindOf(RUNTIME_CLASS(CBiSplitterWnd)) )
		{
			// 父亲是切分,父亲的父亲还是切分			
			pToLeave->SetParent(pGrandPa);
			pToLeave->SetDlgCtrlID(pParentBisplittwnd->GetDlgCtrlID());

			CBiSplitterWnd* pGrandPaSplit = (CBiSplitterWnd*)pGrandPa;
			int iParentSplitPaneID = pGrandPaSplit->GetViewPaneID(pParentBisplittwnd);
			
			pGrandPaSplit->AssignViewToPane(pToLeave->GetDlgCtrlID(), iParentSplitPaneID);
			//
			pToDel->DestroyWindow();
			pParentBisplittwnd->DestroyWindow();

			// 设置激活视图 xl 0608 防止删除View后，突然失去输入焦点
			CView *pFirstActiveView = CMainFrame::FindGGTongView(pToLeave);
			if (NULL != pFirstActiveView)
			{
				SetActiveView(pFirstActiveView);
				if ( IsWindow(pFirstActiveView->GetSafeHwnd()) && pFirstActiveView->IsKindOf(RUNTIME_CLASS(CGGTongView)) )
				{
					CGGTongView *pGGTView = (CGGTongView *)pFirstActiveView;
					if ( pGGTView->GetEmptyFlag() && NULL == pMainFrame->GetEmptyGGTongView() )
					{
						// 设置MainFrame的EmptyGGTong
						pMainFrame->SetEmptyGGTongView(pGGTView);
					}
				}
			}
		}
	}

	//
 	if (pGrandPa->IsKindOf(RUNTIME_CLASS(CMPIChildFrame)))
 	{
 		((CMPIChildFrame *)pGrandPa)->RecalcLayout();
 	}
 	else if (pGrandPa->IsKindOf(RUNTIME_CLASS(CBiSplitterWnd)))
 	{
 		((CBiSplitterWnd *)pGrandPa)->RecalcLayout();
 	}
 	else if (pGrandPa->IsKindOf(RUNTIME_CLASS(CTabSplitWnd)))
 	{
 		((CTabSplitWnd *)pGrandPa)->RecalcLayout();
 	}

	pGrandPa->RedrawWindow();
}

const char *CMPIChildFrame::GetXmlElementValue()
{
	return KStrElementValue;
}

void CMPIChildFrame::PostUpdateWnd()
{
	ShowWindow(m_iShowFlag);
}

void CMPIChildFrame::SetChildFrameTitle(CString StrTitle,bool32 bFix)
{
	return;
	if (!bFix)
	{
		if ( m_bFixTitle)
		{
			//  NULL;
		}
		else
		{
			m_StrTitle = StrTitle;
			//if ( m_rectCaption.Height() >= KChildFrameCaptionHeight )
			{
				SetWindowText(StrTitle);  // 设置windowText导致标题闪烁- -
				DrawCustomNcClient();
			}
			//else
			{
				
			}
		}		
	}
	else
	{
		SetFixTitleFlag(true);		
		m_StrTitle = StrTitle;
		SetWindowText(StrTitle);  // 设置windowText导致标题闪烁- -
		DrawCustomNcClient();
	}
}

bool32 CMPIChildFrame::GetChildFrameFaceObjectFromXml(TiXmlElement * pRootElement,OUT T_ChildFrameFaceObject& ChildFaceObject)
{
	if (NULL == pRootElement)
	{
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// 如果这个child 是设置过颜色字体的,需要重新读取这些信息,并保存到FaceSchema 中

	ChildFaceObject.m_pChildFrame = this;

	const char * pcAttrValue = NULL;
	
	// 颜色等属性
	for ( int32 i = 0 ; i < CFaceScheme::Instance()->GetSysColorObjectCount();i ++)
	{
		CString StrAttriName  = CFaceScheme::Instance()->GetSysColorObject(i)->m_StrFileKeyName;

		char	aStrAttriName[100];
		UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
		pcAttrValue = pRootElement->Attribute(aStrAttriName);
		if (pcAttrValue)
		{
			ChildFaceObject.m_aColors[i] = atol(pcAttrValue);
		}	
		else
		{
			return false;
		}
	}
	
	// 字体属性
	//////////////////////////////////////////////////////////////////////////
	CString StrAttriName = CFaceScheme::Instance()->GetKeyNameNormalFontName();

	char	aStrAttriName[100];
	UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);

	pcAttrValue = pRootElement->Attribute(aStrAttriName);

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
			pcAttrValue = pRootElement->Attribute(aStrAttriName);
			KeyValue[i-4] = (pcAttrValue == NULL)?(CFaceScheme::Instance()->GetSysDefaultFontObject(i)->m_iKeyValue):atol(pcAttrValue);			

			if ( NULL == pcAttrValue)
			{
				// 出错就直接返回,不设置这个ChildFrame 的风格了.避免读出错误数据,或者本来就是没有数据的,非要给他加上默认数据
				return false;
			}
		}
		NormalFont.lfHeight		= KeyValue[0];
		NormalFont.lfWeight		= KeyValue[1];
		NormalFont.lfCharSet		= KeyValue[2];
		NormalFont.lfOutPrecision	= KeyValue[3];
		
		ChildFaceObject.m_aFonts[ESFNormal].m_stLogfont = NormalFont;
		ChildFaceObject.m_aFonts[ESFNormal].SetFont(&NormalFont);
	}

	StrAttriName = CFaceScheme::Instance()->GetKeyNameSmallFontName();

	memset(aStrAttriName,0,sizeof(aStrAttriName));
	UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
	pcAttrValue = pRootElement->Attribute(aStrAttriName);
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
			pcAttrValue = pRootElement->Attribute(aStrAttriName);
			KeyValue[i-8] = (pcAttrValue == NULL)?(CFaceScheme::Instance()->GetSysDefaultFontObject(i)->m_iKeyValue):atol(pcAttrValue);	
			
			if ( NULL == pcAttrValue)
			{			
				return false;
			}
		}
		SmallFont.lfHeight			= KeyValue[0];
		SmallFont.lfWeight			= KeyValue[1];
		SmallFont.lfCharSet			= KeyValue[2];
		SmallFont.lfOutPrecision	= KeyValue[3];

		ChildFaceObject.m_aFonts[ESFSmall].m_stLogfont = SmallFont;
		ChildFaceObject.m_aFonts[ESFSmall].SetFont(&SmallFont);
	}
	
	StrAttriName = CFaceScheme::Instance()->GetKeyNameBigFontName();

	memset(aStrAttriName,0,sizeof(aStrAttriName));
	UNICODE_2_MULTICHAR(EMCCUtf8,StrAttriName,aStrAttriName);
	pcAttrValue = pRootElement->Attribute(aStrAttriName);
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
			pcAttrValue = pRootElement->Attribute(aStrAttriName);
			KeyValue[i] = (pcAttrValue == NULL)?(CFaceScheme::Instance()->GetSysDefaultFontObject(i)->m_iKeyValue):atol(pcAttrValue);
			
			if ( NULL == pcAttrValue)
			{			
				return false;
			}
		}
		BigFont.lfHeight		= KeyValue[0];
		BigFont.lfWeight		= KeyValue[1];
		BigFont.lfCharSet		= KeyValue[2];
		BigFont.lfOutPrecision	= KeyValue[3];
		
		ChildFaceObject.m_aFonts[ESFBig].m_stLogfont = BigFont;
		ChildFaceObject.m_aFonts[ESFBig].SetFont(&BigFont);
	}
	
	return true;
}

bool32 CMPIChildFrame::FromXml( TiXmlElement * pTiXmlElement)
{
#ifdef _DEBUG
	DWORD dwTime = timeGetTime();
#endif
	if (NULL == pTiXmlElement)
		return false;
	
	const char *pcValue = pTiXmlElement->Value();
	if (NULL == pcValue || strcmp(KStrElementValue, pcValue) != 0)
		return false;
	
	// 一: 显示本身属性
	TiXmlElement *  pRootElement = pTiXmlElement;	
	
	WINDOWPLACEMENT aWp[256];
	memset(aWp, 0, sizeof(aWp));
	aWp[0].length = sizeof(WINDOWPLACEMENT);

	CString StrName = AfxGetApp()->m_pszAppName;
	const char *pcAttrValue = pTiXmlElement->Attribute(KStrElementAttriName);
	if (NULL != pcAttrValue && strlen(pcAttrValue) >0 )	StrName = _A2W(pcAttrValue);

	m_iGroupID = 0;
	pcAttrValue = pTiXmlElement->Attribute(KStrElementAttrGroupId);
	if (NULL != pcAttrValue)	m_iGroupID = atoi(pcAttrValue);
	
	pcAttrValue = pTiXmlElement->Attribute(KStrElementAttriFlag);
	if (NULL != pcAttrValue)	aWp[0].flags   =  atoi(pcAttrValue);
	
	pcAttrValue	= pRootElement->Attribute(KStrElementAttriShowCmd);
	if (NULL != pcAttrValue)		aWp[0].showCmd = atoi(pcAttrValue);

	if ( 0 == aWp[0].showCmd )
	{
		// 与以前的那个BeMax false兼容
		aWp[0].showCmd = SW_SHOWNORMAL;
	}

	pcAttrValue	= pRootElement->Attribute(KStrElementAttriPtMinX);
	if (NULL != pcAttrValue)		aWp[0].ptMinPosition.x = atol(pcAttrValue);

	pcAttrValue	= pRootElement->Attribute(KStrElementAttriPtMinY);
	if (NULL != pcAttrValue)		aWp[0].ptMinPosition.y = atol(pcAttrValue);

	pcAttrValue	= pRootElement->Attribute(KStrElementAttriPtMaxX);
	if (NULL != pcAttrValue)		aWp[0].ptMaxPosition.x = atol(pcAttrValue);

	pcAttrValue	= pRootElement->Attribute(KStrElementAttriPtMaxY);
	if (NULL != pcAttrValue)		aWp[0].ptMaxPosition.y = atol(pcAttrValue);

	pcAttrValue	= pRootElement->Attribute(KStrElementAttriPtNorLeft);
	if (NULL != pcAttrValue)		aWp[0].rcNormalPosition.left = atol(pcAttrValue);

	pcAttrValue	= pRootElement->Attribute(KStrElementAttriPtNorRight);
	if (NULL != pcAttrValue)		aWp[0].rcNormalPosition.right = atol(pcAttrValue);

	pcAttrValue	= pRootElement->Attribute(KStrElementAttriPtNorTop);
	if (NULL != pcAttrValue)		aWp[0].rcNormalPosition.top = atol(pcAttrValue);

	pcAttrValue	= pRootElement->Attribute(KStrElementAttriPtNorBottom);
	if (NULL != pcAttrValue)		aWp[0].rcNormalPosition.bottom = atol(pcAttrValue);
	
	//SetWindowPlacement(&aWp[0]);
//	SetWindowText(StrName);
	SetWindowText(NULL);	// 不要显示任何名字

	int32 iMax = aWp[0].showCmd;
		
	m_bMaxSize = (iMax == SW_SHOWMAXIMIZED) ? true : false;

	// 如果 m_bMaxSize == 1 ,说明上次退出是最大化状态.在不同分辨率机器上.判断此时子窗口状态,如果客户区大小和最大化的矩形
	// 大小相同,则返回,否则,需要将客户区调整到最大矩形大小
	// XL0006 已经不需要调整了，系统会根据SW_SHOWMAXMIZED自己调整，后面ioView..等会根据现在的属性初始化
	SetWindowPlacement(&aWp[0]);

	// 由于使用的是隐藏创建，so这里需要调用frame的InitialUpdateFrame
	InitialUpdateFrame(AfxGetDocument(), TRUE);
	
	// 颜色字体风格

	T_ChildFrameFaceObject Object;

	if ( GetChildFrameFaceObjectFromXml(pRootElement,Object))
	{
		CFaceScheme::Instance()->UpdateChildFrameFaceObjectList(Object);
	}

	// lock split
	pcAttrValue = pRootElement->Attribute(KStrEleAttriLockedSplit);
	if ( NULL != pcAttrValue )
	{
		//m_bLockedSplit = atoi(pcAttrValue) != 0;
		SetLockSplit(atoi(pcAttrValue) != 0);
	}

	// 二: 轮询子窗口, 仅处理一个有效的子节点
	bool32 bDoOk = false;	
	TiXmlElement *pChild = pRootElement->FirstChildElement();
	while (NULL != pChild)
	{
		pcValue = pChild->Value();
		if (strcmp(CTabSplitWnd::GetXmlElementValue(), pcValue) == 0)
		{
			bDoOk = DoTabSplitElement(pChild);
			if (bDoOk)
			{
				m_bHaveTab = true;
			}
			break;
		}
		else if (strcmp(CBiSplitterWnd::GetXmlElementValue(), pcValue) == 0)
		{
			bDoOk = DoBiSplitElement(pChild, NULL, 0);
			break;	
		}
		else if (strcmp(CGGTongView::GetXmlElementValue(), pcValue) == 0)
		{
			bDoOk = DoGGTongViewElement(pChild, NULL, 0);
			break;
		}
		else
		{
			//	NULL;
		}
		//		
		pChild = pChild->NextSiblingElement();
	}
#ifdef _DEBUG
	TRACE(_T("MPIChildFrame xml: %d ms\r\n"), timeGetTime()-dwTime);
#endif

	// 是否需要还原F7状态- - TODO

//	ForceRefresh();
	//////////////////////////////////////////////////////////////////////////
	// 设置最大化标志
	// 	int  iStatusBaxHeigth;
	// 	CRect rectMax;	
	// 	AfxGetMainWnd()->GetClientRect(&rectMax);
	// 	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	// 	iStatusBaxHeigth = pMainFrame->m_wndStatusBar.GetStatusBarHeight();
	// 	rectMax.bottom -= iStatusBaxHeigth;
	// 	
	// 	CRect rect;
	// 	
	// 	GetWindowRect(rect);
	// 	GetParent()->ScreenToClient(rect);
	// 
	// 	m_bMaxSize = (rectMax == rect)? true : false;
	//////////////////////////////////////////////////////////////////////////

	// 有新的东东产生了，调整一下布局
	RecalcLayout();

#ifdef _DEBUG
	TRACE(_T("MPIChildFrame 共: %d ms\r\n"), timeGetTime()-dwTime);
#endif
	return bDoOk;
	
}

// 我们约定, 只有childframe下面才可以有tabsplit, 所以, 父窗口指针为NULL
bool32 CMPIChildFrame::DoTabSplitElement(TiXmlElement *pElement)
{
	if (NULL == pElement)
		return FALSE;

	const char *pcValue = pElement->Value();
	if (strcmp(CTabSplitWnd::GetXmlElementValue(), pcValue) != 0)
		return false;

	// 创建tabsplit
	CTabSplitWnd* pTabWnd = new CTabSplitWnd;
	
	// ...as the childframe's child
	if (!pTabWnd->CreateStatic(this, 2, 1))
		return false;
	
	// create the tab control and load tabs from toolbar resource
	if (!pTabWnd->CreateEmptyTabs())
		return false;
	//
	return pTabWnd->FromXml(pElement);
}

// 当父亲是childframe时, pParent给NULL
bool32 CMPIChildFrame::DoBiSplitElement(TiXmlElement *pElement, CWnd *pParent, int32 iDlgItemId)
{	
	if (NULL == pElement)
	{
		return false;
	}

	const char *pcValue = pElement->Value();
	if ( NULL == pcValue || strcmp(CBiSplitterWnd::GetXmlElementValue(), pcValue) != 0 )
	{
		return false;
	}

	// 取参数
	bool32 bHorz  = true;
	double  fScale = 50;		// 50% 默认是对半开 xl 0607 精确比例保存
	bool32 bRightRect = false;
	bool32 bTopRect = false;
	bool32 bCanDrag = true;

	TiXmlAttribute *pAttr = pElement->FirstAttribute();	
	while (NULL != pAttr)
	{
		CString StrAttrName  = _A2W(pAttr->Name());
		CString StrAttrValue = _A2W(pAttr->Value());

		if ( CBiSplitterWnd::GetXmlElmAttrBiSplitIsH() == StrAttrName )
		{
			bHorz = (int32)_ttoi(StrAttrValue);
		}
		else if ( CBiSplitterWnd::GetXmlElmAttrBiSplitScale() == StrAttrName )
		{
			//iScale = _ttoi(StrAttrValue);
			fScale = _tcstod(StrAttrValue, NULL);
		}
		else if ( CBiSplitterWnd::GetXmlElmAttrBiSplitRightRect() == StrAttrName )
		{
			bRightRect = (int32)_ttoi(StrAttrValue);
		}
		else if ( CBiSplitterWnd::GetXmlElmAttrBiSplitTopRect() == StrAttrName )
		{
			bTopRect = (int32)_ttoi(StrAttrValue);
		}
		else if ( CBiSplitterWnd::GetXmlElmAttrBiSplitCanDrag() == StrAttrName )
		{
			bCanDrag = (int32)_ttoi(StrAttrValue);
		}
		//		
		pAttr = pAttr->Next();
	}

	// 新建分割条
	CBiSplitterWnd* pSplitter = new CBiSplitterWnd;
	pSplitter->SetAutoDelete(TRUE);
	int32 iDirection = !bHorz ? VSPLITTER : HSPLITTER;
	pSplitter->SetSplitterParam(bRightRect, bTopRect, bCanDrag);

	if ( NULL == pParent )
	{
		// 父亲是子窗口
		if ( !pSplitter->Create(this, iDirection | PROPORTIONAL) )
		{
			return false;
		}
	}
	else
	{
		// 父亲是SplitWnd
		if ( !pSplitter->Create(pParent, iDirection | PROPORTIONAL) )
		{
			return false;
		}
	}

	if (bRightRect)
	{
		// 创建容器
		if( !pSplitter->CreatePane(BSW_FIRST_PANE, RIGHT_BTN, L"1") ) 
		{
			pSplitter->DestroyWindow();
		}
	}
	else
	{
		// 创建容器
		if( !pSplitter->CreatePane(BSW_FIRST_PANE, THIN_BUTTON, L"1") ) 
		{
			pSplitter->DestroyWindow();
		}
	}

	if (bTopRect)
	{
		if( !pSplitter->CreatePane(BSW_SECOND_PANE, TOP_BTN, L"2") ) 
		{
			pSplitter->DestroyWindow();
		}
	}
	else
	{
		if( !pSplitter->CreatePane(BSW_SECOND_PANE, THIN_BUTTON, L"2") ) 
		{
			pSplitter->DestroyWindow();
		}
	}

	// 
	if ( NULL != pParent )
	{
		if ( pParent->IsKindOf(RUNTIME_CLASS(CBiSplitterWnd)) )
		{
			CBiSplitterWnd* pParentSplit = (CBiSplitterWnd*)pParent;		
			pSplitter->SetParent(pParentSplit);	
			pParentSplit->AssignViewToPane(pSplitter->GetDlgCtrlID(), iDlgItemId);
		}
		else
		{
			pSplitter->SetDlgCtrlID(iDlgItemId);
		}
	}

	// 分别取到两个子节点(必须有两个必须的节点, 不然就认为出错!!!)
	int32 iChildIndex = 0;

	TiXmlElement *pChild = pElement->FirstChildElement();

	while (NULL != pChild)
	{
		int32 iChildDlgItemId = 0;

		if (0 == iChildIndex)
		{
			iChildDlgItemId = 0;
		}
		else if (1 == iChildIndex)
		{
			iChildDlgItemId = 1;
		}
		else
		{
			break;
		}

		const char *pcSubValue = pChild->Value();
		if (strcmp(CBiSplitterWnd::GetXmlElementValue(), pcSubValue) == 0)
		{
			if (!DoBiSplitElement(pChild, pSplitter, iChildDlgItemId))
				return false;
		}
		else if (strcmp(CGGTongView::GetXmlElementValue(), pcSubValue) == 0)
		{
			if (!DoGGTongViewElement(pChild, pSplitter, iChildDlgItemId))
				return false;
		}

		// 
		iChildIndex++;
		pChild = pChild->NextSiblingElement();
	}

	if ( 2 != iChildIndex ) 
	{
		// 没有两个须要的子节点, 不行!
		pSplitter->DestroyWindow();
		CBiSplitterWnd* pParentSplit = (CBiSplitterWnd*)pParent;
		if (NULL != pParent && NULL != pParentSplit)
		{
			pParentSplit->m_panes[iDlgItemId].m_pView = NULL;
		}
		
		return false;
	}

	pSplitter->RecalcLayout();
	pSplitter->Invalidate();	

	pSplitter->SetRadioXml(fScale * 0.01);	
	pSplitter->Invalidate();

	// 保存
	m_listSplitters.Add(pSplitter);

	return TRUE;
}

// 当父亲是childframe时, pParent给NULL
bool32 CMPIChildFrame::DoGGTongViewElement(TiXmlElement *pElement, CWnd *pParent, int32 iDlgItemId)
{
	//////////////////////////////////////////////////////////////////////////
	//	
	//	 记录GGTongView 的是否激活? 每个Tab 下有且仅有一个GGTongView 是激活的.
	//    有多少个Tab 页就有多少个激活的GGTongView
	//		


	// get view's runtime class information
	CRuntimeClass* pViewClass = RUNTIME_CLASS(CGGTongView);
	ASSERT_POINTER(pViewClass, CRuntimeClass);

	CGGTongView *pView = NULL;

	if (NULL == pParent)
	{	// the only view in this window, perform default operation
		m_pContext->m_pNewViewClass = pViewClass;
		
		//return CMDIChildWnd::OnCreateClient(NULL, m_pContext);
		pView = (CGGTongView*)CreateView(m_pContext, AFX_IDW_PANE_FIRST);
	}
	else
	{
		// create view object and window
		pView = (CGGTongView*)pViewClass->CreateObject();
		if (NULL == pView)
			return false;

		if ( !pView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW & ~WS_BORDER, CRect(0,0,0,0), pParent, iDlgItemId, m_pContext) )
			return FALSE;
		
		if ( pParent->IsKindOf(RUNTIME_CLASS(CBiSplitterWnd)) )
		{
			CBiSplitterWnd* pParentSplit = (CBiSplitterWnd*)pParent;
			ASSERT(NULL != pParentSplit);
			
			pView->SetParent(pParentSplit);
			pParentSplit->AssignViewToPane(pView->GetDlgCtrlID(), iDlgItemId);		
		}		
	}

	if (NULL == pView)
		return FALSE;

	return pView->FromXml(pParent,pElement);
}

CString CMPIChildFrame::ToXmlForWorkSpace(const char * KpcFileName)
{
	CString StrThis;
	StrThis  = L"<?xml version =\"1.0\" encoding=\"UTF-8\"?> \n";	
	StrThis += L"<XMLDATA version=\"1.0\" app = \"ggtong\" data = \"ChildFrame\">";
	StrThis += ToXml(0 ,KpcFileName);
	StrThis += L"</XMLDATA>";

	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	pMainFrame->SaveXmlFile(KpcFileName,StrThis);

	return StrThis;
}

CString CMPIChildFrame::SaveChildFrameFaceObjectToXml()
{
	bool32  bFind	= false;
	CString StrThis = L"";
	T_ChildFrameFaceObject Object;

	int32 i = 0;
	for (int32 i =0 ; i < CFaceScheme::Instance()->m_aChildFrameFaceObjectList.GetSize(); i++)
	{
		Object = CFaceScheme::Instance()->m_aChildFrameFaceObjectList.GetAt(i);
		if ( NULL != Object.m_pChildFrame && this == Object.m_pChildFrame)
		{
			bFind = true;
			break;
		}
	}

	if ( !bFind)
	{
		return StrThis;
	}
	
	// 数组中有这个窗口的数据,保存到XML
	
	// 颜色
	
	CString StrColor = L"";
	/*
			error 437: (Warning -- Passing struct 'CStringT' to ellipsis)
	*/
	//lint --e{437}
	for ( i= 0 ; i < CFaceScheme::Instance()->GetSysColorObjectCount() ; i++ )
	{	
		CString StrKey;		
		StrKey.Format(L" %s=\"%i\" ", CFaceScheme::Instance()->GetSysColorObject(i)->m_StrFileKeyName ,Object.m_aColors[i]);		
		StrColor += StrKey;
	}
	
	StrThis += StrColor;
	
	// 字体
	CString StrFont = L"";
	CString StrFontName,StrHeight,StrWeight,StrCharSet,StrOutPrecision;	
	
	for ( i = 0 ; i< ESFCount ; i++)
	{					
		CString StrKey;
		
		CFaceScheme::Instance()->SetKeyNames(E_SysFont(i),StrFontName,StrHeight,StrWeight,StrCharSet,StrOutPrecision);
		
		StrKey.Format(L" %s=\"%s\" %s=\"%i\" %s=\"%i\" %s=\"%i\" %s=\"%i\"",
								StrFontName, Object.m_aFonts[i].m_stLogfont.lfFaceName,
								StrHeight,Object.m_aFonts[i].m_stLogfont.lfHeight,		
								StrWeight,Object.m_aFonts[i].m_stLogfont.lfWeight,
								StrCharSet,Object.m_aFonts[i].m_stLogfont.lfCharSet,
								StrOutPrecision,Object.m_aFonts[i].m_stLogfont.lfOutPrecision);
		StrFont += StrKey;
	}				
	
	StrThis += StrFont;
	
	return StrThis;
}

CString CMPIChildFrame::ToXml(int32 iZIndex ,const char * KpcFileName)
{	
	CWnd * pWndChild = GetWindow(GW_CHILD);
	
	CString StrThis;	
	CString StrName, StrZIndex, StrFlag, StrShowCmd;
	CString StrPtMinPosX, StrPtMinPosY, StrPtMaxPosX, StrPtMaxPosY;
	CString StrNomPosleft, StrNomPosright, StrNomPostop, StrNomPosbottom;
	CString StrLockedSplit;
	
	WINDOWPLACEMENT aWp[256];
	aWp[0].length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(&aWp[0]);
	
	GetWindowText(StrName);
	StrZIndex.Format(L"%d",iZIndex);
	 
	StrFlag.Format(L"%d", aWp[0].flags);
	StrShowCmd.Format(L"%d", aWp[0].showCmd);
	StrPtMinPosX.Format(L"%i", aWp[0].ptMinPosition.x);
	StrPtMinPosY.Format(L"%i", aWp[0].ptMinPosition.y);
	StrPtMaxPosX.Format(L"%i", aWp[0].ptMaxPosition.x);
	StrPtMaxPosY.Format(L"%i", aWp[0].ptMaxPosition.y);
	StrNomPosleft.Format(L"%i",aWp[0].rcNormalPosition.left);
	StrNomPosright.Format(L"%i",aWp[0].rcNormalPosition.right);
	StrNomPostop.Format(L"%i",aWp[0].rcNormalPosition.top);
	StrNomPosbottom.Format(L"%i",aWp[0].rcNormalPosition.bottom);
	
	StrLockedSplit = IsLockedSplit() ? _T("1") : _T("0");
	/*
		error 437: (Warning -- Passing struct 'CStringT' to ellipsis)
	*/
	//lint --e{437}
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%d\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" ",/*>\n*/
		CString(KStrElementValue),
		CString(KStrElementAttriName),
		StrName,
		CString(KStrElementAttrGroupId),
		m_iGroupID,
		CString(KStrElementAttriZIndex),
		StrZIndex,
		CString(KStrElementAttriFlag),
		StrFlag,
		CString(KStrElementAttriShowCmd),
		StrShowCmd,			  
		CString(KStrElementAttriPtMinX),
		StrPtMinPosX,
		CString(KStrElementAttriPtMinY),
		StrPtMinPosY,
		CString(KStrElementAttriPtMaxX),
		StrPtMaxPosX,
		CString(KStrElementAttriPtMaxY),
		StrPtMaxPosY,
		CString(KStrElementAttriPtNorLeft),
		StrNomPosleft,
		CString(KStrElementAttriPtNorRight),
		StrNomPosright,
		CString(KStrElementAttriPtNorTop),
		StrNomPostop,
		CString(KStrElementAttriPtNorBottom),
		StrNomPosbottom,
		CString(KStrEleAttriLockedSplit),
		StrLockedSplit
		);

	StrThis += SaveChildFrameFaceObjectToXml();
	StrThis += L">\n";

    CGGTongView * pActiveView =(CGGTongView *)GetActiveView();

	while (NULL != pWndChild)
	{
		if (pWndChild->IsKindOf(RUNTIME_CLASS(CTabSplitWnd)))
		{
			StrThis += ((CTabSplitWnd*)pWndChild)->ToXml();
		}
		else if (pWndChild->IsKindOf(RUNTIME_CLASS(CBiSplitterWnd)))
		{
			StrThis += ((CBiSplitterWnd*)pWndChild)->ToXml(pActiveView);
		}
		else if (pWndChild->IsKindOf(RUNTIME_CLASS(CGGTongView)))
		{
			StrThis += ((CGGTongView*)pWndChild)->ToXml(pActiveView);
		}
		else
		{
			//	NULL;
		}

		pWndChild = pWndChild->GetNextWindow();
	}
	
	StrThis += L"</";
	StrThis += KStrElementValue;
	StrThis += L"> \n";
			
	// 传入路径参数，保存当前ChildFrame 布局信息.
	if (NULL != KpcFileName)                       
	{		
		CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
		pMainFrame->SaveXmlFile(KpcFileName,StrThis);
	}

	return StrThis;
}

CString CMPIChildFrame::GetDefaultXML(bool32 bBlank,CRect RectPre,CIoViewBase *pIoView)
{
	CString StrDefaultXML;	 	
	CString StrName,StrGroupID,StrZIndex,StrFlag,StrShowCmd;
	CString StrPtMinPosX, StrPtMinPosY, StrPtMaxPosX, StrPtMaxPosY;
	CString StrNomPosleft, StrNomPosright, StrNomPostop, StrNomPosbottom;
	CString StrLockedSplit(_T("0"));
	
	//此时MainFrame还未用xml中指定的尺寸进行设置,所以无法读取ClientRect

	// 设置新建Child 的位置
	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CRect RectNow;

	BOOL bShouldMax;

	if ( pMainFrame->m_bShowHistory && NULL == pMainFrame->GetHistoryChild())
	{
		RectNow.left	= 0;
		RectNow.top		= 0;
		RectNow.right	= NEWWINDOWWIDTH;
		RectNow.bottom	= NEWWINDOWHEIGHT;
	}
	else
	{
		RectNow.left	= RectPre.left  + NEWWINDOWOFFSETX;
		RectNow.top		= RectPre.top	+ NEWWINDOWOFFSETY;
		RectNow.right	= RectNow.left  + NEWWINDOWWIDTH;
		RectNow.bottom	= RectNow.top   + NEWWINDOWHEIGHT;
		
		CRect rectMax;	
		AfxGetMainWnd()->GetClientRect(&rectMax);
		if ( rectMax.Width() > 10 && rectMax.Height() > 10)
		{
			if (RectNow.right >= rectMax.right || RectNow.bottom >= rectMax.bottom)
			{
				RectNow = CRect(0,0,NEWWINDOWWIDTH,NEWWINDOWHEIGHT);
			}
		}
	}

	int32 iGroupId  = 1;
	if ( NULL != m_pThis )
	{
		iGroupId = m_pThis->GetGroupID();
	}

	StrName				= AfxGetApp()->m_pszAppName;
	StrGroupID.Format(L"%d", iGroupId);
	StrZIndex			= L"0";
	StrFlag				= L"0";
	//StrShowCmd			= L"0";
	StrShowCmd			= L"1";      // SW_SHOWNORMAL
	bShouldMax = true;	// 总是要最大化
	if ( bShouldMax )
	{
		StrShowCmd.Format(_T("%d"), SW_SHOWMAXIMIZED);		// 应当最大化
	}
	StrPtMinPosX		= L"-1";
	StrPtMinPosY		= L"-1";
	StrPtMaxPosX		= L"-4";
	StrPtMaxPosY		= L"-30";  

	StrNomPosleft.Format(L"%d",RectNow.left);
	StrNomPosright.Format(L"%d",RectNow.right);
	StrNomPostop.Format(L"%d",RectNow.top);
	StrNomPosbottom.Format(L"%d",RectNow.bottom);
	/*
		error 437: (Warning -- Passing struct 'CStringT' to ellipsis)
	*/
	//lint --e{437}
	StrDefaultXML.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" >\n",
		CString(KStrElementValue),
		CString(KStrElementAttriName),
		StrName,		
		CString(KStrElementAttrGroupId),
		StrGroupID,
		CString(KStrElementAttriZIndex),
		StrZIndex,
		CString(KStrElementAttriFlag),
		StrFlag,
		CString(KStrElementAttriShowCmd),
		StrShowCmd,			  
		CString(KStrElementAttriPtMinX),
		StrPtMinPosX,
		CString(KStrElementAttriPtMinY),
		StrPtMinPosY,
		CString(KStrElementAttriPtMaxX),
		StrPtMaxPosX,
		CString(KStrElementAttriPtMaxY),
		StrPtMaxPosY,
		CString(KStrElementAttriPtNorLeft),
		StrNomPosleft,
		CString(KStrElementAttriPtNorRight),
		StrNomPosright,
		CString(KStrElementAttriPtNorTop),
		StrNomPostop,
		CString(KStrElementAttriPtNorBottom),
		StrNomPosbottom,
		CString(KStrEleAttriLockedSplit),
		StrLockedSplit
		);
	
	StrDefaultXML += CGGTongView::GetDefaultXML(iGroupId,bBlank, pIoView);
	
	StrDefaultXML += L"</";
	StrDefaultXML += KStrElementValue;
	StrDefaultXML += L">\n";
		
	return StrDefaultXML;
}

void CMPIChildFrame::SetActiveGGTongView(CGGTongView *pGGTongView)
{
	CView *pView = CMainFrame::FindGGTongView(this, pGGTongView);
	if (NULL != pView)
	{
		SetActiveView(NULL);
		SetActiveView(pView);
	}
}

void CMPIChildFrame::OnTimer(UINT nIDEvent)
{
	if ( KTimerIdRecalcLayout == nIDEvent )
	{
		KillTimer(nIDEvent);
		//RecalcLayout();
		for (HWND hWndChild = ::GetTopWindow(m_hWnd); hWndChild != NULL;
		hWndChild = ::GetNextWindow(hWndChild, GW_HWNDNEXT))
		{
			CWnd* pWnd = CWnd::FromHandlePermanent(hWndChild);
			if ( NULL != pWnd )
			{
				CBiSplitterWnd *pSplit = DYNAMIC_DOWNCAST(CBiSplitterWnd, pWnd);
				if ( NULL != pSplit )
				{
					pSplit->RecalcLayout();	// 有可能是子窗口需要变更大小
				}
			}
		}
	}

	CMDIChildWnd::OnTimer(nIDEvent);
}


void CMPIChildFrame::OnNcPaint()
{	
	CWindowDC dc(this);
	CRect rectWindow;
	GetWindowRect(&rectWindow);
	ScreenToClient(&rectWindow);

	int32 iBorderWidth = GetSystemMetrics(SM_CXFRAME) - GetSystemMetrics(SM_CXBORDER);
	int32 iSpaceWidth = 10;
	COLORREF clrBk(RGB(30,30,30));
	COLORREF clrMid(RGB(26,25,30));

	CRect rtTop, rtRight, rtLeft, rtBottom;
	rtTop = rectWindow;
	rtTop.top = iBorderWidth;
	rtTop.bottom = rtTop.top + iSpaceWidth;

	rtRight = rectWindow;
	rtRight.right += iSpaceWidth; 
	rtRight.left = rtRight.right - iSpaceWidth;
	rtRight.bottom += iSpaceWidth;

	rtLeft = rectWindow;
	rtLeft.left = iBorderWidth;
	rtLeft.right = rtLeft.left + iSpaceWidth;

	rtBottom = rectWindow;
	rtBottom.bottom = rtRight.bottom;
	rtBottom.top = rtBottom.bottom - iSpaceWidth;

	dc.FillSolidRect(&rtTop, clrBk);
	dc.FillSolidRect(&rtRight, clrBk);
	dc.FillSolidRect(&rtLeft, clrBk);
	dc.FillSolidRect(&rtBottom, clrBk);
	rtTop.DeflateRect(1, 1, 0, 1);
	rtRight.DeflateRect(1, 1, 1, 0);
	rtLeft.DeflateRect(1, 1, 1, 0);
	rtBottom.DeflateRect(1, 1, 0, 1);
	dc.FillSolidRect(&rtTop, clrMid);
	dc.FillSolidRect(&rtRight, clrMid);
	dc.FillSolidRect(&rtLeft, clrMid);
	dc.FillSolidRect(&rtBottom, clrMid);

	// CMDIChildWnd::OnNcPaint();
	// 框架。。。
	//if ( !IsIconic() )
	//{
	//	// 非最小化才调用
	//	Default();
	//}

	//DrawCustomNcClient(true);
}

void CMPIChildFrame::FindGGtongView(CWnd *pWnd, bool32 bActive)
{
	if (pWnd->IsKindOf(RUNTIME_CLASS(CGGTongView)))
	{
		CGGTongView * pActiveView = NULL;
		pActiveView = (CGGTongView *)pWnd;

		if (pActiveView)
		{
			int32 iViewCnt = pActiveView->m_IoViewManager.m_IoViewsPtr.GetSize();

			for (int32 i=0; i<iViewCnt; i++)
			{
				if (pActiveView->m_IoViewManager.m_IoViewsPtr[i]->IsKindOf(RUNTIME_CLASS(CIoViewNews)))
				{
					CIoViewNews *pNew = (CIoViewNews *)pActiveView->m_IoViewManager.m_IoViewsPtr[i];
					pNew->ChangeWebStatus(bActive);
				}
			}
		}
	}
	else if (pWnd->IsKindOf(RUNTIME_CLASS(CBiSplitterWnd)))
	{
		CBiSplitterWnd *pBiSplitterWnd = NULL;
		pBiSplitterWnd =(CBiSplitterWnd *)pWnd;

		CWnd * pWnd1 = pBiSplitterWnd->GetPaneView(0);
		CWnd * pWndanother1 = pBiSplitterWnd->GetPaneView(1);

		if (NULL == pWnd1 || NULL == pWndanother1)
		{
			return;
		}

		FindGGtongView(pWnd1, bActive);
		FindGGtongView(pWndanother1, bActive);
	}
}

BOOL CMPIChildFrame::OnNcActivate(BOOL bActive)
{ 	
	if (!m_StrId.IsEmpty())
	{
		CWnd * pWndChild = GetWindow(GW_CHILD);
		CBiSplitterWnd *pBiSplitterWnd = NULL;

		while (NULL != pWndChild)
		{
			if (pWndChild->IsKindOf(RUNTIME_CLASS(CBiSplitterWnd)))
			{
				pBiSplitterWnd =(CBiSplitterWnd *)pWndChild;

				CWnd * pWnd			= pBiSplitterWnd->GetPaneView(0);
				CWnd * pWndanother  = pBiSplitterWnd->GetPaneView(1);

				if (NULL == pWnd || NULL == pWndanother)
				{
					break;
				}
				FindGGtongView(pWnd, bActive);
				FindGGtongView(pWndanother, bActive);
			}

			pWndChild = pWndChild->GetNextWindow();
		}
	}

	m_bActive = bActive;
	BOOL bRet = CMDIChildWnd::OnNcActivate(bActive);

	return bRet;
}

//#BUG_MARK_BEGIN [NO=XL0002 AUTHOR=xiali DATE=2010/04/12]
//					忽略其处理，会产生响应的Nc Mouse Move
LRESULT CMPIChildFrame::OnNcHitTest(CPoint point) 
{
	CPoint pt = point;
	ScreenToClient(&pt);
	
	LRESULT	htStatus	=	HTNOWHERE;
	if ( m_rectClose.PtInRect(pt))
	{
		htStatus = HTCLOSE;
	}
	else if ( m_rectMax.PtInRect(pt))
	{
		htStatus = HTMAXBUTTON;
	}
	else if (m_rectMin.PtInRect(pt))
	{
		htStatus = HTMINBUTTON;
	}
	else if ( m_rectCaption.PtInRect(pt))
	{
		htStatus = HTCAPTION;
	}
	
	if ( HTNOWHERE == htStatus )
	{
		htStatus	=	CMDIChildWnd::OnNcHitTest(point);
		if ( HTMINBUTTON == htStatus || HTMAXBUTTON == htStatus || HTCLOSE == htStatus || HTSYSMENU == htStatus )
		{
			htStatus = HTCAPTION;
		}
	}

	return htStatus;
}

LRESULT CMPIChildFrame::OnNcMouseLeave(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	// 该功能要求使用 Windows 2000 或更高版本。
	// 符号 _WIN32_WINNT 和 WINVER 必须 >= 0x0500。
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//清除所有显示的标志
	
	if ( ECSSNormal != m_eCurrentShowState )
	{
		m_eCurrentShowState	=	ECSSNormal;
		DrawCustomNcClient(false);
	}

	//CMDIChildWnd::OnNcMouseLeave();

	return	Default();
}


void CMPIChildFrame::CalcNcSize()
{
	
	CRect rc;
	GetWindowRect( rc );
	CRect rcClient;
	GetClientRect(rcClient);
	ClientToScreen(&rcClient);
	if (IsIconic())
	{
		rcClient = rc;
		rcClient.top = rc.bottom;
	}
	int32 iMaxCapHeight = rcClient.top - rc.top - GetSystemMetrics(SM_CYFRAME) + 1;   // 存在忽略标题栏的情况
	int32 iCapHeight = MIN(iMaxCapHeight, KChildFrameCaptionHeight);
	int32 iSkipX = GetSystemMetrics(SM_CXFRAME);
	m_rectCaption.top		= rc.top + GetSystemMetrics(SM_CYFRAME) - 1;
	m_rectCaption.left		= rc.left + iSkipX -1;
	m_rectCaption.bottom	= m_rectCaption.top + iCapHeight;
	m_rectCaption.right		= rc.right - iSkipX + 1;
	
	int32 iButtonSize = m_rectCaption.Height();
	
	int32 iButtonWidth = iButtonSize;
	if ( NULL != m_pImageButtons )
	{
		iButtonWidth = m_pImageButtons->GetWidth() / 12;
	}
	
	m_rectClose			= m_rectCaption;
	m_rectClose.right  -= 6;	// 偏离最右边 6 个pixel
	m_rectClose.left    = m_rectClose.right - iButtonWidth;
	
	
	m_rectMax		= m_rectClose;
	m_rectMax.right = m_rectClose.left -1;
	m_rectMax.left  =  m_rectMax.right - iButtonWidth;
	
	m_rectMin		= m_rectClose;
	m_rectMin.right = m_rectMax.left -1;
	m_rectMin.left  = m_rectMin.right - iButtonWidth;
	
	ScreenToClient(&m_rectCaption);
	ScreenToClient(&m_rectClose);
	ScreenToClient(&m_rectMax);
	ScreenToClient(&m_rectMin);
}

void CMPIChildFrame::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp)
{
 	CMDIChildWnd::OnNcCalcSize(bCalcValidRects, lpncsp);

//	DWORD dwStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
	

	CRect &rc  = (CRect&)lpncsp->rgrc[0]; 

	rc.top += KFrameSpace;
	rc.left   += KFrameSpace;   
	rc.bottom -= KFrameSpace;   
	rc.right  -= KFrameSpace;  


	if ( IsZoomed() )
	{
		// 最大化 - 不要标题栏 - 此时应当把NC按钮交由MainFrame处理 - MainFrame再交由ReBar处理
		CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
		pMain->OnChildFrameMaximized(true);
	}
	else if ( !IsIconic() )
	{
		// 非最小化 - 要标题栏, Iconic状态由系统控制 - MainFrame放弃ChildFrame的NC按钮
		lpncsp->rgrc[0].top += KChildFrameCaptionHeight;
	}
	//CalcNcSize();
}

void CMPIChildFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIChildWnd::OnSize(nType, cx, cy);
	goto ResetCPI;
	return;
	if ( IsIconic() )
	{
		// 最小化时，要改变最小化窗口的位置
		if ( NULL != GetParent() )
		{
			CRect rcParentClient;
			if ( ::GetClientRect(GetParent()->GetSafeHwnd(), &rcParentClient) )
			{
				CRect rcMe;
				GetWindowRect(&rcMe);
				GetParent()->ScreenToClient(&rcMe);
				bool32 bChange = false;
				if ( rcMe.bottom > rcParentClient.bottom )
				{
					rcMe.top = rcParentClient.bottom - rcMe.Height();
					bChange  = true;
				}
				if ( rcMe.left < rcParentClient.left )
				{
					rcMe.left = rcParentClient.left;
					bChange   = true;
				}
				if ( bChange )
				{
					GetParent()->ArrangeIconicWindows();
				}
			}
		}
	}
	else
	{
		// left top, right top 两个小弧线
		CRect rcWin;
		GetWindowRect(rcWin);
		ScreenToClient(&rcWin);
		rcWin.OffsetRect(-rcWin.left, -rcWin.top);
		CRgn rgnWin;
		int32 iHeight = 2;

		
		// Round总是达不到效果 ？左右总是不对称 11的时候为最接近，暂不解决
		rgnWin.CreateRoundRectRgn(rcWin.left, rcWin.top, rcWin.right, rcWin.top + 2 * iHeight, 2* iHeight, 2 * iHeight);

		
		CRgn rgnBottom;
		rgnBottom.CreateRectRgn(rcWin.left, rcWin.top + iHeight, rcWin.right, rcWin.bottom);
		rgnWin.CombineRgn(&rgnWin, &rgnBottom, RGN_OR);

		SetWindowRgn((HRGN)rgnWin.Detach(), TRUE);
	}

	CMDIChildWnd::OnSize(nType, cx, cy);
	CalcNcSize();
	
ResetCPI:
	// 强制window frame刷新
	RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW);
	
	// 与以前的相关函数兼容
	m_bMaxSize = SIZE_MAXIMIZED == nType;

	
	// 暂时先不实现 - OK
	// 如果自己是最小化，将其它非最小化的窗口还原
	// 如果自己是最大化，其它窗口全最大化
	// 如果自己还原，则将其它非最小化窗口全部还原

	if ( SIZE_MAXIMIZED == nType )
	{
		WINDOWPLACEMENT wnp;
		GetWindowPlacement(&wnp);
		CRect rc;
		GetWindowRect(rc);
		rc.OffsetRect(CPoint(wnp.ptMaxPosition)-rc.TopLeft());
		rc.top -= KChildFrameCaptionHeight;
		wnp.rcNormalPosition = rc;
		if ( wnp.showCmd != SW_HIDE )
		{
			wnp.showCmd = SW_SHOWNA;
		}
		SetWindowPlacement(&wnp);
	}
}

void CMPIChildFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
	CMDIChildWnd::OnGetMinMaxInfo(lpMMI);

	// 设定子窗口的最小尺寸,
	lpMMI->ptMinTrackSize.x = 100;   //宽   
	lpMMI->ptMinTrackSize.y = 200;   //高  
}

void CMPIChildFrame::OnWindowPosChanged( WINDOWPOS* lpwndpos )
{
	//CalcNcSize();
	CMDIChildWnd::OnWindowPosChanged(lpwndpos);
}

void CMPIChildFrame::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	E_CurrentShowState	eTemp	=	m_eCurrentShowState;
	m_eCurrentShowState = ECSSNormal;

	TRACKMOUSEEVENT	trackMouse	=	{0};
	trackMouse.cbSize		=	sizeof(trackMouse);
	trackMouse.dwFlags		=	GGT_TME_NONCLIENT | TME_LEAVE; //TME_NONCLIENT | TME_LEAVE; //TME_NONCLIENT是加上ncclient的意思
	trackMouse.hwndTrack	=	m_hWnd;
	BOOL bMouseTrack = _TrackMouseEvent( &trackMouse );
	ASSERT( bMouseTrack );
	
	
	if ( HTCLOSE == nHitTest )
	{
		m_eCurrentShowState = ECSSForcusClose;
	}
	else if ( HTMAXBUTTON == nHitTest )
	{
		m_eCurrentShowState = ECSSForcusMax;
	}
	else if ( HTMINBUTTON == nHitTest )
	{
		m_eCurrentShowState = ECSSForcusMin;
	}
	else
	{
	}
	
	if ( eTemp != m_eCurrentShowState )
	{
		DrawCustomNcClient(false);
	}
	
	CMDIChildWnd::OnNcMouseMove(nHitTest, point);
}

void CMPIChildFrame::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	m_eCurrentShowState = ECSSNormal;

	//忽略系统的
	switch ( nHitTest )
	{
	case HTMINBUTTON:
		{
			m_eCurrentShowState = ECSSPressMin;
			DrawCustomNcClient(false);
		}
		return;
	case HTMAXBUTTON:
		{
			m_eCurrentShowState = ECSSPressMax;
			DrawCustomNcClient(false);
		}
		return;
	case HTCLOSE:
		{
			m_eCurrentShowState = ECSSPressClose;
			DrawCustomNcClient(false);
		}
		return;
	case HTHELP:
		return;
	}

	CMDIChildWnd::OnNcLButtonDown(nHitTest,point);
}

void CMPIChildFrame::OnNcLButtonUp(UINT nHitTest, CPoint point)
{	
	m_eCurrentShowState = ECSSNormal;

	// 清空多余的LButtonUp
	MSG msg;
	while (PeekMessage(&msg, GetSafeHwnd(), WM_NCLBUTTONUP, WM_NCLBUTTONUP, PM_REMOVE))
	{
		// 空语句
		//	NULL;
	}

	if ( HTMINBUTTON == nHitTest )
	{
		SendMessage(KMsgChildFrameMinSize,0,0);	
		return;
	}
	else if ( HTMAXBUTTON == nHitTest )
	{							
		SendMessage(KMsgChildFrameMaxSize, 0, 0);
		return;
	}
	else if ( HTCLOSE == nHitTest )
	{		
		SendMessage(WM_SYSCOMMAND, SC_CLOSE, 0);
		return;
	}

	CMDIChildWnd::OnNcLButtonUp(nHitTest,point);
}


void CMPIChildFrame::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	if ( HTCAPTION == nHitTest )
	{
		if ( IsZoomed() || IsIconic() )
		{
			PostMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
		}
		else
		{
			PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
		}
		return;
	}
	CMDIChildWnd::OnNcLButtonDblClk(nHitTest,point);
}


LRESULT CMPIChildFrame::OnChildFrameMaxSize(WPARAM wParam, LPARAM lParam)
{
	if ( IsZoomed() )
	{
		SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
	}
	else
	{
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	}
	return 0;		
}

LRESULT CMPIChildFrame::OnChildFrameMinSize(WPARAM wParam, LPARAM lParam)
{
	if ( IsIconic() )
	{
		PostMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
	}
	else
	{
		PostMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	}

	return 0;
}

void CMPIChildFrame::ForceRefresh()
{
	CRect rect;

	GetWindowRect(rect);
	GetParent()->ScreenToClient(rect);
	
    //取得子窗口相对于父窗口客户区的位置和大小 - 这个的调用可能引发刷新频繁
	//RedrawWindow(rect, NULL, RDW_INVALIDATE | RDW_FRAME | RDW_ERASE | RDW_UPDATENOW |RDW_ALLCHILDREN );
	
	
	
	rect.bottom -= 2;
	rect.right  -= 2;
	MoveWindow(rect);
	
	rect.right  += 2;
	rect.bottom += 2;
	MoveWindow(rect);
	
	UpdateWindow();
	//RedrawWindow(rect , NULL, RDW_INVALIDATE | RDW_FRAME | RDW_ERASE | RDW_UPDATENOW |RDW_ALLCHILDREN );
}

BOOL CMPIChildFrame::OnEraseBkgnd( CDC* pDC )
{
	// Frame默认的有背景画刷
	return TRUE;
}

void CMPIChildFrame::DrawCustomNcClient(bool32 bAllDraw/* = true*/)
{
	return;
	// 设置WindowOrg
	CWindowDC dc(this);
	CRect rectWindow;
	GetWindowRect(&rectWindow);
	ScreenToClient(&rectWindow);

	CRect rectRedraw = m_rectCaption;
	CRect rectClient;
	GetClientRect(rectClient);
	rectRedraw = rectWindow;
	rectRedraw.bottom = rectClient.top;

	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	if ( NULL != pThreadState 
		&& pThreadState->m_lastSentMsg.message == WM_NCPAINT
		&& pThreadState->m_lastSentMsg.wParam != 1 )
	{
		// 设置ClipRegion，以防止闪烁
		HRGN hRgn;
		hRgn = CreateRectRgn(0,0,0,0);
		CombineRgn(hRgn, (HRGN)pThreadState->m_lastSentMsg.wParam, NULL, RGN_COPY);
		CRect rcWin;
		GetWindowRect(rcWin);
		OffsetRgn(hRgn, -rcWin.left, -rcWin.top);
		ExtSelectClipRgn(dc.GetSafeHdc(), hRgn, RGN_AND);
		DeleteObject(hRgn);
	}
	else
	{
		// 将ClientArea排除
		CRect rectExclude = rectClient;
		rectExclude.OffsetRect(-rectWindow.left, -rectWindow.top);
		dc.ExcludeClipRect( &rectExclude );
	}

	CPoint pointWindowOrgOld = dc.SetWindowOrg(rectWindow.left, rectWindow.top);

	Graphics GraphicsImage(dc.GetSafeHdc());
	Matrix transformMatrix;

	// 标题栏
	Image *pCaptionImage = m_pCaptionActive;
	
	if ( !m_bActive )
	{		
		pCaptionImage = m_pCaptionInActive;
	}

	if ( IsIconic() )
	{
		// 最小化时，需要覆盖整个
		//rectRedraw.bottom = rectWindow.bottom - GetSystemMetrics(SM_CYFRAME) + 2;
		rectRedraw = rectWindow;
		dc.DrawEdge(rectRedraw, EDGE_ETCHED, BF_RECT);
		rectRedraw.DeflateRect(2, 2);
	}
	else if ( bAllDraw )
	{
		// 普通情况下，绘制边框
		// 左侧边框
		Rect rcFrame;
		int32 iDrawHeight, iDrawStartY;

		rcFrame.X = rectWindow.left;
		rcFrame.Y = rectClient.bottom;
		rcFrame.Width = rectClient.Height() + 1;
		rcFrame.Height = rectClient.left - rectWindow.left;

		transformMatrix.RotateAt(-90.0f, PointF(rcFrame.X, rcFrame.Y));
		GraphicsImage.SetTransform(&transformMatrix);
		
		int32 iHeight = pCaptionImage->GetHeight();
		iDrawHeight = MIN(iHeight, rcFrame.Height);
		iHeight     = (pCaptionImage->GetHeight() - iDrawHeight) * 2/3;
		iDrawStartY = MAX(iHeight, 0);
		
		GraphicsImage.DrawImage(pCaptionImage, rcFrame, 0, iDrawStartY, pCaptionImage->GetWidth(), iDrawHeight, UnitPixel);

		// 右侧边框
		transformMatrix.Reset();
		transformMatrix.RotateAt(90.0f, PointF(rectWindow.right, rectClient.bottom));
		GraphicsImage.SetTransform(&transformMatrix);
		rcFrame.X = rectWindow.right - rcFrame.Width;
		GraphicsImage.DrawImage(pCaptionImage, rcFrame, 0, iDrawStartY, pCaptionImage->GetWidth(), iDrawHeight, UnitPixel);

		// 底边
		GraphicsImage.ResetTransform();
		rcFrame.X = rectWindow.left;
		rcFrame.Width = rectWindow.Width();
		rcFrame.Y = rectClient.bottom;
		rcFrame.Height = rectWindow.bottom - rectClient.bottom;
		GraphicsImage.DrawImage(pCaptionImage, rcFrame, 0, iDrawStartY, pCaptionImage->GetWidth(), iDrawHeight, UnitPixel);
	}

	// 自己刷一下
	//dc.FillSolidRect(&rectRedraw, RGB(255,0,0));
	// 使用DrawImage时，为什么总有些像素画不到？
	//DrawImage(GraphicsImage, pCaptionImage, rectRedraw, 1, 0, true);
	Rect rcCap(rectRedraw.left, rectRedraw.top, rectRedraw.Width(), rectRedraw.Height());
	GraphicsImage.DrawImage(pCaptionImage, rcCap, 2, 2, pCaptionImage->GetWidth() - 5, pCaptionImage->GetHeight() - 3, UnitPixel);
	if ( rcCap.Height > 0 )
	{
		// 绘制文字要用另外一个dc，不然在拖动的时候，dc的windoworg好像返回了原来的0，不知道为什么，拖动慢的时候没什么问题
		CWindowDC dcText(this);
		CFont * pFont	 = CFaceScheme::Instance()->GetSysFontObject(ESFSmall);
		CFont * pOldFont = dcText.SelectObject(pFont);
		dcText.SetBkMode(TRANSPARENT);
		dcText.SetWindowOrg(rectWindow.left, rectWindow.top);
		// 由于现在处理黑线是所有的全绘制，所以这个也要绘制
		//if ( !m_bNcMouseMove )
		{
			CRect RectTitle = m_rectCaption;
			RectTitle.top   = rectWindow.top; // 边框由自己绘制，节约一点y值
			RectTitle.left += 5;
			RectTitle.right = m_rectMin.left - 2;
			dcText.SetTextColor(RGB(252, 217, 11));
			if ( !IsIconic() )
			{
				dcText.DrawText(m_StrTitle,RectTitle,DT_LEFT |DT_VCENTER |DT_SINGLELINE  );
			}
			else
			{
				dcText.DrawText(m_StrTitle,RectTitle,DT_LEFT |DT_BOTTOM |DT_SINGLELINE  );
			}
			
			//dcText.DrawText(m_StrTitle, CRect(0, 0, 200, 20),DT_LEFT |DT_VCENTER );
			dcText.SelectObject(pOldFont);
		}
		dcText.SetWindowOrg(0, 0);
	}
	
	// 按钮:
	int32 iBtnCloseIndex = 3;
	int32 iBtnMaxIndex	 = 1;
	int32 iBtnMinIndex	 = 0;
	if ( IsZoomed() )
	{
		iBtnMaxIndex = 2;
	}
	else if ( IsIconic() )
	{
		iBtnMinIndex = 2;
	}

	if ( ECSSForcusClose == m_eCurrentShowState)
	{
		iBtnCloseIndex += 4;
	}
	else if ( ECSSPressClose == m_eCurrentShowState )
	{
		iBtnCloseIndex += 4*2;	
	}
	else if ( ECSSForcusMax == m_eCurrentShowState)
	{
		iBtnMaxIndex += 4;
	}
	else if ( ECSSPressMax == m_eCurrentShowState)
	{		
		iBtnMaxIndex += 4*2;
		
	}
	else if ( ECSSForcusMin == m_eCurrentShowState)
	{
		iBtnMinIndex += 4;
	}
	else if ( ECSSPressMin == m_eCurrentShowState)
	{
		iBtnMinIndex += 4*2;	
	}
	
	CRect rectClose = m_rectClose;
	CRect rectMax   = m_rectMax;
	CRect rectMin   = m_rectMin;
	if ( NULL != m_pImageButtons )
	{
		int32 iBtnHeight = m_pImageButtons->GetHeight();
		if ( m_rectClose.Height() > iBtnHeight )
		{
			int32 iBtnSkipY = (m_rectClose.Height() - iBtnHeight) / 2;
			rectClose.DeflateRect(0, iBtnSkipY);
			rectMax.DeflateRect(0, iBtnSkipY);
			rectMin.DeflateRect(0, iBtnSkipY);
		}
	}
	DrawImage(GraphicsImage, m_pImageButtons, rectClose, 12, iBtnCloseIndex, false);
	DrawImage(GraphicsImage, m_pImageButtons,   rectMax, 12, iBtnMaxIndex, false);
	DrawImage(GraphicsImage, m_pImageButtons,   rectMin, 12, iBtnMinIndex, false);

	dc.SetWindowOrg(pointWindowOrgOld);
}


LRESULT CMPIChildFrame::OnSetWindowText( WPARAM w, LPARAM l )
{
	LPCTSTR pcsz = (LPCTSTR)l;	// 可能为NULL
	
	m_StrWindowText = pcsz;
	m_bWindowTextValid = true;
	
	
	Default();
	
	return TRUE;
}

LRESULT CMPIChildFrame::OnGetWindowText( WPARAM w, LPARAM l )
{
	if ( m_bWindowTextValid )
	{
		LPTSTR pszBuf = (LPTSTR)l;
		int iLen = (int)w;
		if ( NULL != pszBuf && iLen > 0 )
		{
			_tcsncpy(pszBuf, m_StrWindowText, iLen);
			pszBuf[iLen-1] = _T('\0');
			int32 iTextLen = m_StrWindowText.GetLength();
			return min(iTextLen, iLen-1);
		}
		return 0;
	}
	else
	{
		return Default();
	}
}

LRESULT CMPIChildFrame::OnSetGroupID(WPARAM wParam,LPARAM lParam)
{
	SetGroupID(int32(wParam));
	return TRUE;
}


LRESULT CMPIChildFrame::OnIsHideMode(WPARAM wParam,LPARAM lParam)
{
	return IsHideMode();
}

LRESULT CMPIChildFrame::OnIsKindOfCMpiChildFram(WPARAM wParam,LPARAM lParam)
{
	return TRUE;
}
LRESULT CMPIChildFrame::OnIsLockedSplit(WPARAM wParam,LPARAM lParam)
{
	return IsLockedSplit();
}
LRESULT CMPIChildFrame::On_IsF7AutoLock(WPARAM wParam,LPARAM lParam)
{
	return IsF7AutoLock();
}
LRESULT CMPIChildFrame::OnSetF7AutoLock(WPARAM wParam,LPARAM lParam)
{
	SetF7AutoLock(bool32(wParam));
	return TRUE;
}
LRESULT CMPIChildFrame::OnDelSplit(WPARAM wParam,LPARAM lParam)
{
	DelSplit((CView *)wParam);
	return TRUE;
}
LRESULT CMPIChildFrame::OnDoGGTongViewElement(WPARAM wParam,LPARAM lParam)
{
	T_DoViewElementItem *pDo = (T_DoViewElementItem *)wParam;
	return DoGGTongViewElement(pDo->pChild,pDo->pSender,pDo->iDlgItemId);
}

LRESULT CMPIChildFrame::OnDoBiSplitElement(WPARAM wParam,LPARAM lParam)
{
	T_DoViewElementItem *pDo = (T_DoViewElementItem *)wParam;
	return DoBiSplitElement(pDo->pChild,pDo->pSender,pDo->iDlgItemId);
}

LRESULT CMPIChildFrame::OnGetWindowTextLength( WPARAM w, LPARAM l )
{
	if ( m_bWindowTextValid )
	{
		return m_StrWindowText.GetLength();
	}
	else
	{
		return Default();
	}
}

bool32 CMPIChildFrame::IsLockedSplit() const
{
	return m_bLockedSplit;
}

void CMPIChildFrame::SetLockSplit( bool32 bLock /*= false*/ )
{
	int32 iOld = m_bLockedSplit ? 1 : 0;
	m_bLockedSplit = bLock;
	int32 iNew = m_bLockedSplit ? 1 : 0;

	if ( iOld != iNew && GetMDIFrame()->MDIGetActive() == this )
	{
		OnUpdateFrameMenu(true, this, NULL);	// 更新菜单
	}
}

CString CMPIChildFrame::GetIdString() const
{
	return m_StrId;
}

void CMPIChildFrame::SetIdString( const CString &StrId /*= _T("")*/ )
{
	m_StrId = StrId;
}

void CMPIChildFrame::ChangSubGroupId( CWnd *pWnd, int32 iId, bool32 bChangeMerch )
{
	if ( NULL == pWnd )
	{
		return;
	}

	// 临时
	CWnd *pSub = NULL;
	for ( pSub=pWnd->GetTopWindow(); NULL != pSub ; pSub = pSub->GetNextWindow() )
	{
		if ( pSub->IsKindOf(RUNTIME_CLASS(CIoViewManager)) )
		{
			CIoViewManager *pMananger = (CIoViewManager *)pSub;
			pMananger->ChangeGroupId(iId, bChangeMerch);

			continue;	// manager下sub无意义
		}

		// it's sub
		ChangSubGroupId(pSub, iId, bChangeMerch);
	}
}

void CMPIChildFrame::OnDestroy()
{

	// 是否提示保存还是怎么的？
	// 自动保存页面 - 不保存 ？？？
	if ( !GetIdString().IsEmpty() )
	{
		//CCfmManager::Instance().SaveCfm(GetIdString(), this);
	}
	
	if (m_bHistoryChildFrame)
	{
		CMainFrame * pMainFrame    = (CMainFrame *)AfxGetMainWnd();
		pMainFrame->m_bShowHistory = false;
		pMainFrame->SetHistoryChildNULL();
	}
	
	// FaceSchema 中删除这个Child 相关的信息:
	
	CFaceScheme::Instance()->DelChildFrameFaceObject(this);	

	CMDIChildWnd::OnDestroy();
}

void CMPIChildFrame::SetF7AutoLock( bool32 bF7Lock )
{
	// 由于有n个split窗口，所以F7时会有n次的相同调用~~
	if ( (bF7Lock && m_bF7AutoLock)
		|| (!bF7Lock && !m_bF7AutoLock) )
	{
		return;	// 相同不处理
	}

	if ( bF7Lock )
	{
		m_bF7LockSplitOrg = IsLockedSplit();	// 备份原状态
		//m_bLockedSplit = true;				// 锁定分割
		SetLockSplit(true);
		m_bF7AutoLock = bF7Lock;
	}
	else
	{
		//m_bLockedSplit = m_bF7LockSplitOrg;	// 还原以前
		SetLockSplit(m_bF7LockSplitOrg);
		m_bF7AutoLock = bF7Lock;
	}
}

bool32 CMPIChildFrame::IsF7AutoLock() const
{
	return m_bF7AutoLock;
}

CGGTongView			* CMPIChildFrame::GetF7GGTongView() const
{
	if ( IsF7AutoLock() )
	{
		// 只能查找哪个是鬼F7了
		CWnd *pChild = GetTopWindow();
		bool32 bFindBi = false;
		while ( NULL != pChild )
		{
			if ( pChild->IsKindOf(RUNTIME_CLASS(CBiSplitterWnd)) )
			{
				bFindBi = true;
				CBiSplitterWnd *pBi = (CBiSplitterWnd *)pChild;
				// 哪个是F9的
				pChild = pBi->GetF9MaxPane();	// 有可能是NULL // 实际f9窗口可能仅为一个pane
			}
			else if ( pChild->IsKindOf(RUNTIME_CLASS(CGGTongView)) )
			{
				CGGTongView *pView = (CGGTongView *)pChild;
				return pView;		// 那就是这个了
			}
			else if ( !bFindBi )
			{
				// 没有找到bi窗口就继续
				pChild = pChild->GetNextWindow(GW_HWNDNEXT);
			}
			else
			{
				// 不属于任何类型且已经是bi窗口的子，则不用继续搜索了
				break;
			}
		}
	}
	return NULL;
}

void CMPIChildFrame::SetF7GGTongViewXml( CGGTongView *pView )
{
	m_pF7GGTongViewXml = pView;
}

int32 CMPIChildFrame::RemoveSplitMenuItem( CMenu &menu )
{
	if ( menu.m_hMenu == NULL )
	{
		return 0;
	}

	CArray<int32, int32>	aDelPos;
	const int32 iMenuCount = menu.GetMenuItemCount();
	MENUITEMINFO minfo;
	ZeroMemory(&minfo, sizeof(minfo));
	minfo.cbSize = sizeof(minfo);
	minfo.fMask = MIIM_ID |0x00000100/*MIIM_FTYPE*/;
	bool32 bPreIsSeparator = true;
	bool32 bIsLock = IsLockedSplit();
	int32 i = 0;
	for ( i=0; i < iMenuCount ; i++ )
	{
		if ( menu.GetMenuItemInfo(i, &minfo, TRUE) )
		{
			if ( bIsLock )
			{
				// 不需要的菜单
				switch (minfo.wID)
				{
				case IDM_IOVIEWBASE_CLOSECUR:
				case IDM_IOVIEWBASE_CLOSE:
				case ID_LAYOUT_ADJUST:
					aDelPos.Add(i);
					continue;		// 这些即将要删除的就好像不存在一样，不要影响到分割条的判断
					break;
				}
				
				// 判断几个submenu 分割窗口 & 插入内容
				MENUITEMINFO minfo2;
				ZeroMemory(&minfo2, sizeof(minfo2));
				minfo2.cbSize = sizeof(minfo2);
				minfo2.fMask = 0x00000040/*MIIM_STRING*/;
				if ( menu.GetMenuItemInfo(i, &minfo2, TRUE) && minfo2.cch > 0 )
				{
					TCHAR *pBuf = new TCHAR[minfo2.cch +1];
					minfo2.dwTypeData = pBuf;
					++minfo2.cch;
					bool bDel = false;
					if ( menu.GetMenuItemInfo(i, &minfo2, TRUE) 
						&& 
						(_tcscmp(pBuf, _T("分割窗口"))==0 || _tcscmp(pBuf, _T("插入内容"))==0)
						)
					{
						aDelPos.Add(i);	// 这些即将要删除的就好像不存在一样，不要影响到分割条的判断
						bDel = true;
					}
					delete []pBuf;
					if ( bDel )
					{
						continue;
					}
				}
			}

			// 多余的分割条
			bool32 bIsSeparator = (minfo.fType & MF_SEPARATOR) == MF_SEPARATOR;
			if ( bIsSeparator && bPreIsSeparator )
			{
				aDelPos.Add(i);
			}
			else if ( bIsSeparator )
			{
				bPreIsSeparator = true;
			}
			else
			{
				bPreIsSeparator = false;
			}
		}
	}

	// 删除菜单
	int32 iDelCount = 0;
	CNewMenu *pNewMenu = DYNAMIC_DOWNCAST(CNewMenu, &menu);
	if ( pNewMenu != NULL )
	{
		for ( i=aDelPos.GetSize()-1; i >= 0 ; i-- )
		{
			if (pNewMenu->DeleteMenu(aDelPos[i], MF_BYPOSITION))	// 使用Remove
			{
				iDelCount++;
			}
		}
	}
	else
	{
		for ( i=aDelPos.GetSize()-1; i >= 0 ; i-- )
		{
			if (menu.DeleteMenu(aDelPos[i], MF_BYPOSITION))
			{
				iDelCount++;
			}
		}
	}

	return iDelCount;
}

void CMPIChildFrame::RecalcLayout( BOOL bNotify /*= TRUE*/ )
{
	//KillTimer(KTimerIdRecalcLayout);

	CMDIChildWnd::RecalcLayout(bNotify);
}

void CMPIChildFrame::RecalcLayoutAsync()
{
	SetTimer(KTimerIdRecalcLayout, KTimerPeriodRecalcLayout, NULL);
}