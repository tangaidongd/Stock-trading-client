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
#include "BiSplitterWnd.h"
//#include "GGTongView.h"
#include "ShareFun.h"
#include "TabSplitWnd.h"

#include "resource.h"
#include "NewMenu.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CTabSplitWnd, CSplitterWnd);

#define MAXGROUPID 16

//与该类相关的xml配置中相关信息
static const char * KStrElementValue		        = "TabSplitWnd";
static const char * KStrElmeAttrTabSplitWndAlign	= "Align";			
static const char * KStrElmeAttrTabSplitWndStyle	= "Style";                 
static const char * KStrElmeAttrTabSplitWndCurrent	= "Current";

static const char * KStrElementTableValue		    = "TabItem";
static const char * KStrElmTableItemAttriName		= "Name";
static const char * KStrElementAttrGroupId			= "GroupId";
CWnd* CTabSplitWnd::m_pMainFram = NULL;

CTabSplitWnd::CTabSplitWnd()
{
	m_cxSplitter	= 2;
	m_cySplitter	= 2;
	m_cxBorderShare = 0;
	m_cyBorderShare = 0;
	m_cxSplitterGap = 2;
	m_cySplitterGap = 2;
	m_nCurPage		= -1;
	m_bDelAll		= false;
	m_bfromxml		= false;
	m_iCurGroupID	= -1;
}

CTabSplitWnd::~CTabSplitWnd()
{
	m_ActiveViews.RemoveAll();
}

BEGIN_MESSAGE_MAP(CTabSplitWnd, CSplitterWnd)
	ON_NOTIFY(TCN_SELCHANGE, AFX_IDW_PANE_FIRST, OnSelChange)
	ON_COMMAND_RANGE(ID_TAB_BEGIN,ID_TAB_END,OnTabMenu)
END_MESSAGE_MAP()

int CTabSplitWnd::CreateTabs(int nResourceID)
{
// 	// create tab control
// 	if (!m_wndTabCtrl.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, AFX_IDW_PANE_FIRST))
// 		return 0;
// 
// 	// load tabs from resource and return number of tabs
// 	int iRet = m_wndTabCtrl.LoadTabs(nResourceID);
// 	m_ActiveViews.SetSize(m_wndTabCtrl.GetItemCount());
//
//	return iRet;

	return 0;
}

int CTabSplitWnd::CreateEmptyTabs()
{
	//if (!m_wndTabCtrl.Create(WS_CHILD | WS_VISIBLE | TCS_BOTTOM, CRect(0,0,0,0), this, AFX_IDW_PANE_FIRST))
 		//return 0;

	if (!m_wndTabCtrl.Create(WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,AFX_IDW_PANE_FIRST))
		return 0;

	m_wndTabCtrl.SetUserCB(this);

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_wndTabCtrl.StyleDispl(GUISTYLE_XP);
	m_wndTabCtrl.SetBkGround(true, 0, IDB_GUITAB_NORMAL, IDB_GUITAB_SELECTED);
	m_wndTabCtrl.SetImageList(IDB_TAB, 16, 16, 0x0000ff);
	m_wndTabCtrl.SetALingTabs(CGuiTabWnd::ALN_BOTTOM);
	m_wndTabCtrl.ShowWindow(SW_SHOW);
	
	return 1;
}

int CTabSplitWnd::HitTest(CPoint pt) const
{
	return 0;	// disable mouse dragging
}

void CTabSplitWnd::OnSelChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// handle notification from the tab control
	SetPage(m_wndTabCtrl.GetCurtab());	
	int32 iCurTab = m_wndTabCtrl.GetCurtab();
	m_iCurGroupID = m_aGroupIDs.GetAt(iCurTab);

	*pResult = 0;
}

void CTabSplitWnd::BindMainFram(CWnd *pMainFram)
{
	m_pMainFram = pMainFram;
}

void CTabSplitWnd::SetPage(int nPage)
{	
	int32 iCurTab =	m_wndTabCtrl.GetCurtab();
	if ( iCurTab < m_aGroupIDs.GetSize() )
	{
		m_iCurGroupID =	m_aGroupIDs.GetAt(iCurTab);
	}

	//
	if (m_bfromxml)
	{
		if (0 != nPage)
		{			
			// 最开始是第 0 个Tab
			int32    id = IdFromRowCol(1,0);
			CWnd * pOld = GetDlgItem(id);
			CWnd * pNew = GetDlgItem(nPage+1);
				
			if ( NULL == pOld || NULL == pNew )
			{
				return;
			}

			pOld->SetDlgCtrlID(m_nCurPage+1);
			pOld->ShowWindow(SW_HIDE);
			
			pNew->SetDlgCtrlID(id);
			pNew->ShowWindow(SW_SHOW);	
		}
		
		if ( nPage >= 0 && nPage < m_ActiveViews.GetSize() )
		{
			CView * pActiveChildView = m_ActiveViews[nPage];
			CFrameWnd *pFrameWnd = GetParentFrame();
			ASSERT(NULL != pFrameWnd);	
			if (NULL != pFrameWnd)
			{
				pFrameWnd->SetActiveView((CView*)pActiveChildView);						
			}			
		}

		m_bfromxml = false;		
	}
	else
	{		
		if (m_nCurPage == nPage)
			return;		
		
		// 记录下当前tab页对应的活动视图
		if (nPage >= 0 && nPage < m_wndTabCtrl.GetCount())
		{
			CView *pOldActiveView = GetParentFrame()->GetActiveView();
			SetActiveChildView(m_nCurPage, pOldActiveView);
		}
		
		// 		
		CWnd* pOld = GetDlgItem(IdFromRowCol(1, 0));
		CWnd* pNew = GetDlgItem(nPage+1);
		
		if ( NULL != pOld )
		{	// restore ID and hide old page
			pOld->SetDlgCtrlID(m_nCurPage+1);
			pOld->ShowWindow(SW_HIDE);
		}
		else
		{	// initial update - hide all pages
			for (int i=0; i<256; i++)
			{
				pOld = GetDlgItem(i+1);
				if (!pOld)
					break;
				pOld->ShowWindow(SW_HIDE);
			}
		}

		if ( NULL != pNew )
		{	// set ID of bottom splitter's pane
			pNew->SetDlgCtrlID(IdFromRowCol(1, 0));
			
			// show window
			pNew->ShowWindow(SW_SHOW);		
			
			// additional handling for view windows
			// 		if (pNew->IsKindOf(RUNTIME_CLASS(CView)))
			// 			GetParentFrame()->SetActiveView((CView*)pNew);
			
			CView *pActiveChildView = GetActiveChildView(nPage);
			if (NULL == pActiveChildView)
			{
				pActiveChildView = FindFirstChildView(nPage);
			}
			
			// CView *pActiveChildView = FindFirstChildView(nPage);
			
			ASSERT( NULL != pActiveChildView && ::IsWindow(pActiveChildView->GetSafeHwnd()) );
			if (NULL != pActiveChildView && ::IsWindow(pActiveChildView->GetSafeHwnd()))
			{
				CFrameWnd *pFrameWnd = GetParentFrame();
				ASSERT(NULL != pFrameWnd);
				if (NULL != pFrameWnd)
				{
					pFrameWnd->SetActiveView((CView*)pActiveChildView);			
				}
					
			}
		}		
	}
	
	m_nCurPage = nPage;			
	RecalcLayout();			
}

void CTabSplitWnd::RecalcLayout()
{
	CRect rcClient;
	GetClientRect(&rcClient);
	CRect rcTab = rcClient;

	// calculate the height of the tab control
	CRect rcAdjust(0, 0, 100, 1);

	if (m_bDelAll)
	{
		rcAdjust.top	= 0;
		rcAdjust.bottom = 0;
	}
	else
	{
		rcAdjust.top	= 0;
		rcAdjust.bottom = m_wndTabCtrl.GetFitHorW() + 1;						
	}

	rcTab.top	= rcTab.bottom - (rcAdjust.Height() - 1);
	rcTab.DeflateRect(1, 1);
	m_wndTabCtrl.MoveWindow(&rcTab);

	// reposition the tab window

	CRect rcWnd  = rcClient;
	rcWnd.bottom = rcTab.top;
	CWnd* pWnd = GetPane(1, 0);

	// adjust for frame display
	if (NULL != pWnd)
	{
//		if (!(pWnd->GetExStyle() & WS_EX_CLIENTEDGE) && !pWnd->IsKindOf(RUNTIME_CLASS(CSplitterWnd)))
//			rcWnd.DeflateRect(1, 1);
		// reposition current page
		pWnd->MoveWindow(&rcWnd);

	}

	
	DrawAllSplitBars(NULL, 0, 0);
}

void CTabSplitWnd::DrawAllSplitBars(CDC* pDC, int cxInside, int cyInside)
{
	CRect rcClient;
	GetClientRect(&rcClient);
	OnDrawSplitter(pDC, splitBorder, &rcClient);
}

void CTabSplitWnd::SetActiveChildView(int iPage, CView *pActiveView)
{
	if (iPage < 0 || iPage >= m_wndTabCtrl.GetCount())
		return;

	if (iPage >= m_ActiveViews.GetSize())
	{
		m_ActiveViews.SetSize(iPage + 1);
	}

	m_ActiveViews.SetAt(iPage, pActiveView);
}

CView* CTabSplitWnd::GetActiveChildView(int iPage)
{
	if (iPage < 0 || iPage >= m_wndTabCtrl.GetCount())
		return NULL;

	if (iPage < 0 || iPage >= m_ActiveViews.GetSize())
		return NULL;

	return m_ActiveViews[iPage];
}

CView* CTabSplitWnd::FindFirstChildView(int iPage)
{
	if (iPage < 0 || iPage >= m_wndTabCtrl.GetCount())
		return NULL;

	CWnd *pWnd = NULL;
	if (iPage == m_wndTabCtrl.GetCurtab())
	{
		pWnd = GetDlgItem(IdFromRowCol(1, 0));
	}
	else
	{
		pWnd = GetDlgItem(iPage + 1);
	}
	ASSERT( NULL != pWnd);
	//UM_FindGGTongView
	CView* pFindView = NULL;
	if(m_pMainFram != NULL)
	{
		LRESULT lRes = m_pMainFram->SendMessage(UM_FindGGTongView,(WPARAM)pWnd,0);
		if(lRes)
		{
			pFindView = (CView*)lRes;
		}
	}
	return pFindView;//CMainFrame::FindGGTongView(pWnd);
}

void CTabSplitWnd::AddGroupID(int32 iID)
{
	if ( iID < 0 || iID > MAXGROUPID)
	{
		return;
	}

	m_aGroupIDs.Add(iID);
}

void CTabSplitWnd::DelGroupID(int32 iTab)
{
	int32 iCount = m_wndTabCtrl.GetCount();
	int32 iSize  = m_aGroupIDs.GetSize();

	if ( iCount != iSize )
	{
		//ASSERT(0);
		return;
	}

	if ( iTab < 0 || iTab >= iSize)
	{
		return;
	}

	m_aGroupIDs.RemoveAt(iTab);
}

int32 CTabSplitWnd::GetGroupID()
{
	return m_iCurGroupID;	
}

void CTabSplitWnd::DelTab(int iPage)
{
	if (iPage < 0 || iPage >= m_wndTabCtrl.GetCount())
		return;

	CFrameWnd *pParentFrame = GetParentFrame();
	//CMPIChildFrame *pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	//ASSERT(NULL != pParentFrame);

	if (m_wndTabCtrl.GetCount() == 1)
	{
		m_bDelAll = true;
		
		if ( m_aGroupIDs.GetSize() <= 0 )
		{
			return;
		}
		
		int32 iID = m_aGroupIDs.GetAt(iPage);
		pParentFrame->SendMessage(UM_SETGROUPID,iID,0);
		//pParentFrame->SetGroupID(iID);
		DelGroupID(iPage);
		ASSERT(0 == m_aGroupIDs.GetSize());
		
		// 恢复原来的页面与MPIChildFrame父子关系
		// 清除与MPIChildFrame的父子关系
		int	nSubID	=	IdFromRowCol(1,0);
		
		CRect RectClient;
		GetClientRect(&RectClient);
		
		CWnd *pWndSub	=	GetDlgItem( nSubID );
		ASSERT( NULL != pWndSub );
		SetParent( NULL );
		if(NULL != pWndSub)
		{
			pWndSub->SetParent( pParentFrame );
			pWndSub->SetDlgCtrlID( GetDlgCtrlID() );
		}		
		pParentFrame->RecalcLayout(TRUE);

		//调整子窗口大小
		//pWndSub->MoveWindow(&RectClient);
		
		//删除自己
		DestroyWindow();
		delete this;
		return;
	}
	else if (m_wndTabCtrl.GetCount() > 1)
	{
		// 分组信息
		
		DelGroupID(iPage);	
		
		int32 iBeforDelCounts = m_wndTabCtrl.GetCount();
		//////////////////////////////////////////////////////////////////////////
		// iPage: 从0 开始计数
		// ID:	  从1 开始计数
		// 当标签个数大于1 个时才能删除
		
		int32 idNow		= IdFromRowCol(1,0);  // 当前页的ID
		CWnd* pWndShow	= NULL;
		if (iPage != 0)
		{
			pWndShow	= GetDlgItem(1);	  // 删除的不是第一个Tab,则删除后显示第一个 		
		}
		else
		{
			pWndShow	= GetDlgItem(2);	  // 删除的是第一个Tab,则删除后显示第二个 		
		}

	

		CWnd *pWnd = GetDlgItem(idNow);

		if (NULL != pWnd)
		{
			pWnd->DestroyWindow();
		}

		if ( iPage >= 0 && iPage < m_ActiveViews.GetSize() )
		{
			m_ActiveViews.RemoveAt(iPage);
		}
		
		m_wndTabCtrl.DeleteTab(iPage);	
		
		// 被删除Tab 右边的ID 要顺次减1;
		// i = ipage+1 这个是被删除Tab 的ID,从这个ID 后一个Tab 开始ID要减1 ,所以计数器i 是(iPage+2)
		// 如果删除的是第一个Tab,那么会将下一个用于显示,所以从(iPage+3) 开始
		int32 iBegin = (iPage == 0)?(iPage+3):(iPage+2);
		
		int32 i = 0;
		for ( i = iBegin;i <= iBeforDelCounts; i++)
		{				
			if (NULL != GetDlgItem(i))
			{
				GetDlgItem(i)->SetDlgCtrlID(i-1);
			}
		
		}
		
		// 显示当前页
		ASSERT(NULL != pWndShow);
		if (NULL != pWndShow)
		{
			pWndShow->SetDlgCtrlID(idNow);		
			pWndShow->ShowWindow(SW_SHOW);
		}
	

		m_nCurPage = 0;
		m_wndTabCtrl.SetCurtab(0);

		for (i = 2; i<m_wndTabCtrl.GetCount();i++)
		{
			CWnd * pItemWnd = GetDlgItem(i);
			if ( NULL != pItemWnd )
			{
				pItemWnd->ShowWindow(SW_HIDE);
			}
		}

		// 显示激活的视图
		// 		CView *pActiveChildView = GetActiveChildView(iPage);
		// 		if (NULL == pActiveChildView)
		// 		{
		// 			pActiveChildView = FindFirstChildView(iPage);
		//		}		
		
		//		m_ActiveViews.RemoveAt(0);
 		CView *pActiveChildView		 = FindFirstChildView(0);
		
		ASSERT(NULL != pActiveChildView);				
		pParentFrame->SetActiveView((CView*)pActiveChildView);			
		
	}

	//
	RecalcLayout();
	Invalidate();
}

CString CTabSplitWnd::ToXml()
{
	CWnd *pWnd = NULL;
		
	CString StrThis;
	CString StrDual;
	CString StrGGTongView;
	CString StrAttriAlign;
	CString StrAttriStyle;
	
	
	int iAttriCurrent = m_wndTabCtrl.GetCurtab();
	
	//...   得到TABSPLIT 的 STYLE ALIGN 属性
	DWORD dwStyle = m_wndTabCtrl.GetStyleDispl();
	StrAttriStyle.Format(L"%d",dwStyle);
	
	StrThis.Format(L"<%s  %s=\"%s\" %s=\"%s\"  %s=\"%d\" > \n ", 
		CString(KStrElementValue).GetBuffer(),
		CString(KStrElmeAttrTabSplitWndAlign).GetBuffer(), 
		StrAttriAlign.GetBuffer() ,
		CString(KStrElmeAttrTabSplitWndStyle).GetBuffer(), 
		StrAttriStyle.GetBuffer() ,
		CString(KStrElmeAttrTabSplitWndCurrent).GetBuffer(),
		iAttriCurrent);
	
	for (int32 i = 0; i < m_wndTabCtrl.GetCount(); i++)
	{
		// 每个小TAB 新增一层节点
		//////////////////////////////////////////////////////////////////////////
		// 每个Tab 都有一个激活的GGTongView;需要保存这个信息 		

		//CGGTongView * pActiveView = NULL;
		CView *pActiveView = NULL; 
		if ( i < m_ActiveViews.GetSize() )
		{
			pActiveView = m_ActiveViews[i];
		}
				
		//////////////////////////////////////////////////////////////////////////

		CString StrTabItemAttrName= m_wndTabCtrl.GetTabsTitle(i);           //...  默认tab item test
		CString StrXmlTabItem;
		StrXmlTabItem.Format(L"<%s  %s=\"%s\"  %s =\"%d\" > \n" ,
			CString(KStrElementTableValue).GetBuffer(),
			CString(KStrElmTableItemAttriName).GetBuffer(), 
			StrTabItemAttrName.GetBuffer(),
			CString(KStrElementAttrGroupId).GetBuffer(),
			m_aGroupIDs.GetAt(i)
			);
		
		if (i == m_nCurPage)
		{
			pWnd = GetDlgItem(IdFromRowCol(1, 0));
		}
		else
		{
			pWnd = GetDlgItem(i + 1);
		}
		
		if (NULL != pWnd)
		{
			if (pWnd->IsKindOf(RUNTIME_CLASS(CBiSplitterWnd)))
			{
				CString StrBiSplit = ((CBiSplitterWnd*)pWnd)->ToXml(pActiveView);
				
				StrXmlTabItem += StrBiSplit;
			}
			//else if (pWnd->IsKindOf(RUNTIME_CLASS(CGGTongView)))
			else if (pWnd->SendMessage(UM_ISKINDOFCGGTongView))
			{
				/*CString StrGGTongView = ((CGGTongView*)pWnd)->ToXml(pActiveView);*/
				TCHAR * sz = (TCHAR *)pWnd->SendMessage(UM_TOXML,(WPARAM)pActiveView);
				if(sz != NULL)
				{
					StrXmlTabItem += sz;
					delete []sz;
				}		
			}
			
			
		}
		
		StrXmlTabItem += L"</";
		StrXmlTabItem += KStrElementTableValue;
		StrXmlTabItem += L"> \n";
		
		StrThis += StrXmlTabItem;
	}
	
	StrThis += L"</";
	StrThis += KStrElementValue;
	StrThis += L"> \n";
	
	return 	StrThis;

}
	
// 
bool32 CTabSplitWnd::FromXml(TiXmlElement * pTiXmlElement)
{
	m_ActiveViews.RemoveAll();
	m_bfromxml = true;

	m_aGroupIDs.RemoveAll();

	if (NULL == pTiXmlElement)
		return false;
	
	const char *pcValue = pTiXmlElement->Value();
	if (NULL == pcValue || strcmp(KStrElementValue, pcValue) != 0)
		return false;
		
	pTiXmlElement->FirstAttribute();

	CString StrAttriAlign,StrAttriStyle;
	int32	iCurTabItem = 0;

	const char *pcAttrValue = pTiXmlElement->Attribute(KStrElmeAttrTabSplitWndAlign);
	if (NULL != pcAttrValue)
		StrAttriAlign = pcAttrValue;
	
	DWORD	dwStyle = GUISTYLE_XP;
	pcAttrValue = pTiXmlElement->Attribute(KStrElmeAttrTabSplitWndStyle);
	if (NULL != pcAttrValue)
	{
		StrAttriStyle = pcAttrValue;
		dwStyle = _ttol(StrAttriStyle);
	}
	m_wndTabCtrl.StyleDispl(dwStyle);

	pcAttrValue = pTiXmlElement->Attribute(KStrElmeAttrTabSplitWndCurrent);
	if (NULL != pcAttrValue)
		iCurTabItem = atoi(pcAttrValue);

	// 逐个检查子标签节点
	TiXmlElement *pItem = pTiXmlElement->FirstChildElement();
	while (NULL != pItem)
	{
		pcValue = pItem->Value();
		if (NULL == pcValue || strcmp(KStrElementTableValue, pcValue) != 0)
			continue;

		if (!DoTabItemElement(pItem))
			return false;

		pItem = pItem->NextSiblingElement();
	}

	// 处理选中条目

	m_wndTabCtrl.SetCurtab(iCurTabItem);
	if (iCurTabItem == 0)
	{
		SetPage(iCurTabItem);
	}
	// 刷新
	// XL0006 由frame控制刷新
// 	CRect Rect;
// 	GetClientRect(Rect);
// 	ClientToScreen(Rect);
// 	MoveWindow(Rect);
// 	RecalcLayout();
// 	Invalidate();

    return true;
}

bool32 CTabSplitWnd::DoTabItemElement(TiXmlElement *pElement)
{
	if (NULL == pElement)
		return false;

	CString StrValue = _A2W(pElement->Value());
	if (KStrElementTableValue!= StrValue)
		return false;

	// 找到最顶层的childframe
	bool32 bFoundChildFrame = false;
	CWnd *pWnd = GetParent();
	while (1)
	{
		if (NULL == pWnd)
			break;

		//if (pWnd->IsKindOf(RUNTIME_CLASS(CMPIChildFrame)))
		if(pWnd->SendMessage(UM_ISKINDOFCMPIChildFrame))
		{
			bFoundChildFrame = true;
			break;
		}

		pWnd = pWnd->GetParent();
	}
	
	if (!bFoundChildFrame)
		return false;

	//CMPIChildFrame *pChildFrame = (CMPIChildFrame *)pWnd;


	// 获得属性
	CString StrTabName = AfxGetApp()->m_pszAppName;	// 默认名字
	// TiXmlAttribute *pAttr = pElement->FirstAttribute();

	const char* pValue = pElement->Attribute(KStrElmTableItemAttriName);

	if (NULL != pValue)
	{
		StrTabName = _A2W(pValue);
	}

	pValue = pElement->Attribute(KStrElementAttrGroupId);

	if (NULL != pValue)
	{
		int32 iID = atoi(pValue);
		m_aGroupIDs.Add(iID);
	}
	else
	{
		m_aGroupIDs.Add(0);
	}

	/*
	while (NULL != pAttr)
	{
		CString StrAttrName = _A2W(pAttr->Name());
		CString StrAttrValue = _A2W(pAttr->Value());
		
		if ((KStrElmTableItemAttriName == StrAttrName) && (StrAttrValue.GetLength() > 0))
		{
			StrTabName = StrAttrValue;
		}
		else if ((KStrElementAttrGroupId == StrAttrName) && (StrAttrValue.GetLength() > 0 ))
		{
			int32 iID = atoi(pAttr->Value());
			m_aGroupIDs.Add(iID);
		}
		//
		pAttr = pAttr->Next();
	}
	*/

	// 增加一个标签
	int32 iCountTab = m_wndTabCtrl.GetCount();
	m_wndTabCtrl.Addtab(StrTabName,StrTabName,StrTabName);
		
	iCountTab = m_wndTabCtrl.GetCount();

	// 始终选中第一个tab项
	m_wndTabCtrl.SetCurtab(0);
	m_nCurPage = 0;

	// 再查询该标签的第一个有效的子标签, 获得该子标签什么类型的(BisSplit或ggtongview),分别处理
	TiXmlElement *pChild = pElement->FirstChildElement();
	while (NULL != pChild && NULL != pWnd)
	{
		StrValue = _A2W(pChild->Value());
		if ("GGTongView" == StrValue)
		{
			T_DoViewElementItem Do;
			Do.iDlgItemId = (1 == iCountTab) ? IdFromRowCol(1, 0) : iCountTab;
			Do.pSender = this;
			Do.pChild = pChild;
			return pWnd->SendMessage(UM_DoGGTongViewElement,(WPARAM)(&Do));
			//return //pChildFrame->DoGGTongViewElement(pChild, this, (1 == iCountTab) ? IdFromRowCol(1, 0) : iCountTab);
		}
		else if (CBiSplitterWnd::GetXmlElementValue() == StrValue)
		{
			T_DoViewElementItem Do;
			Do.iDlgItemId = (1 == iCountTab) ? IdFromRowCol(1, 0) : iCountTab;
			Do.pSender = this;
			Do.pChild = pChild;
			return pWnd->SendMessage(UM_DoBiSplitElement,(WPARAM)(&Do));
			//return pChildFrame->DoBiSplitElement(pChild, this, (1 == iCountTab) ? IdFromRowCol(1, 0) : iCountTab);
		}
		
		pChild = pChild->NextSiblingElement();
	}

	return false;
}

const char * CTabSplitWnd::GetXmlElementValue()
{
	return KStrElementValue;
}
 
void CTabSplitWnd::OnRButtonDown2(CPoint pt, int32 iTab)
{
	CNewMenu menu;
	menu.CreatePopupMenu();
	menu.AppendODMenu(L"标签页改名",MF_STRING,ID_TAB_RENAME);
	menu.AppendODMenu(L"增加标签页",MF_STRING,ID_TAB_ADD);

	
	//if (iTabCount>1)
	//{
		menu.AppendODMenu(L"删除标签页",MF_STRING,ID_TAB_DEL);
	//}
	int32 iCurTab = m_wndTabCtrl.GetCurtab();
	if ( 0 == iCurTab)
	{
		menu.AppendODMenu(L"",MF_SEPARATOR);
		CNewMenu * pPopMenu = menu.AppendODPopupMenu(L"标签页风格");
		pPopMenu->AppendODMenu(L"风格1",MF_STRING,ID_TAB_STYLE1);
		pPopMenu->AppendODMenu(L"风格2",MF_STRING,ID_TAB_STYLE2);
		pPopMenu->AppendODMenu(L"风格3",MF_STRING,ID_TAB_STYLE3);
	}

	m_wndTabCtrl.ClientToScreen(&pt);
	//CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, this);	
}
void CTabSplitWnd::OnTabMenu(UINT nID)
{
	int32 iTabCount = m_wndTabCtrl.GetCount();
	int32 iCurTab   = m_wndTabCtrl.GetCurtab();
	
	ASSERT(iCurTab>=0 && iCurTab<iTabCount);
	
	switch(nID)
	{
	case ID_TAB_RENAME:
		{	
 			CString StrOldName,StrNewName;
 
 			StrOldName = m_wndTabCtrl.GetTabsTitle(iCurTab);
			StrNewName = StrOldName;
			if(m_pMainFram)
			{
				LRESULT lRes = m_pMainFram->SendMessage(UM_TAB_RENAME,(WPARAM)StrOldName.GetBuffer(0),0);		
				if(lRes)
				{
					TCHAR *szNewName = (TCHAR*)lRes;
					StrNewName = szNewName;
					delete []szNewName;
				}
			}
//  			CDlgTabName dlg;			
//  			dlg.SetName(StrOldName);
//  			if ( IDOK == dlg.DoModal())
//  			{
//  			   StrNewName = dlg.GetName();
//  			}
// 			else
// 			{
// 				StrNewName = StrOldName;
// 			}
			m_wndTabCtrl.SetTabsTitle(iCurTab,StrNewName);			
			
		}
		break;
	case ID_TAB_ADD:
		{			
			// 调用对话框的按钮函数
			
			//CMainFrame  * pMainFrame = (CMainFrame *)AfxGetMainWnd();
			CView * pView	     = (CView *)m_pMainFram->SendMessage(UM_FindGGTongView,0,0);//pMainFrame->FindGGTongView();
			ASSERT( NULL != pView);
			if(NULL != pView)
			{
				//pView->AddSplit(CMPIChildFrame::SD_TAB);
				pView->SendMessage(UM_AddSplit,0);				
			}
			
		}
		break;
	case ID_TAB_DEL:
		{			
			DelTab(iCurTab);			
		}
		break;
	default:
		break;
	}
	if ( nID >= ID_TAB_STYLE1 && nID <= ID_TAB_STYLE8 )
	{
		// 设置风格
		DWORD dwStyle = GUISTYLE_XP + (DWORD)(nID-ID_TAB_STYLE1);
		m_wndTabCtrl.StyleDispl(dwStyle);
	}
}
void CTabSplitWnd::SetPageByIoView(CWnd * pIoView)
{
	// IoView 合法
	if ( NULL == pIoView)
	{
		return;
	}
	
	// 确定这个IoView 属于这个TabSplitWnd;
	bool32 bValid = false;
	
	CWnd * pParentWnd = pIoView->GetParent();
	
	if ( this == pParentWnd)
	{
		bValid = true;
	}
	if (!bValid)
	{
		while (pParentWnd)
		{
			pParentWnd = pParentWnd->GetParent();
			if (this == pParentWnd)
			{
				bValid = true;
				break;
			}
		}		
	}
	
	if (!bValid)
	{
		return;
	}
	
	// 判断这个IoView 在哪个Tab 页下,并显示出来;	
	
	int32 iPage		 = -1;
	int32 iCurTab	 = m_wndTabCtrl.GetCurtab();
	int32 iTabCounts = m_wndTabCtrl.GetCount();

	
	for (int32 i = 1 ; i<= iTabCounts ; i++)
	{
		CWnd * pWnd = NULL;
		if ( i != (iCurTab+1) )
		{
			// 每个Tab 的Id : 1,2,3,4,Now,6....			
			pWnd = GetDlgItem(i);
		
			CWnd * pWndParent = pIoView->GetParent();
			if (pWnd == pWndParent)
			{
				iPage = i;
				break;
			}
			while (pWndParent)
			{
				pWndParent = pWndParent->GetParent();
				if ( pWnd == pWndParent)
				{
					iPage = i;
					break;
				}
			}
		}
		else
		{
			// 如果就在当前页,不用理会.
			int32 iId = IdFromRowCol(1,0);
			pWnd = GetDlgItem(iId);
			
			CWnd * pWndParent = pIoView->GetParent();
			if (pWnd == pWndParent)
			{
				return;
			}
			while (pWndParent)
			{
				pWndParent = pWndParent->GetParent();
				if ( pWnd == pWndParent)
				{
					return;
				}
			}
		}			
	}
	if ( -1 == iPage || 0 == iPage)
	{
		// iPage 是 从1 开始计数(配合每个Tab页的 Id 的设置方式)
		return;
	}
	else
	{
		SetPage(iPage-1);
		m_wndTabCtrl.SetCurtab(iPage-1);
	}
}

int32 CTabSplitWnd::GetIoViewPage(CWnd * pIoView)
{
	// 得到 IoView 所在的页面序号,从 1 开始 计数(配合每个Tab页的 Id 的设置方式)
	
	if (NULL == pIoView)
	{
		return -1;
	}
	
	int32 iCurTab = m_wndTabCtrl.GetCurtab();
	int32 idNow	  = IdFromRowCol(1,0);

	CWnd * pWnd = GetDlgItem(idNow);

	CWnd * pParent = pIoView->GetParent();
	while( pParent )
	{
		if ( pParent == pWnd )
		{
			// 如果当前显示的就是他的父亲,直接返回
			return (iCurTab+1);
		}

		pParent = pParent->GetParent();
	}
	
	// 遍历当前页面以外的每个页面,判断是不是这个业务视图的父亲:

	pParent = pIoView->GetParent();

	for ( int32 i = 1 ; i < m_wndTabCtrl.GetCount(); i++)
	{
		if ( i != iCurTab + 1)
		{
			pWnd = GetDlgItem(i);

			while( pParent )
			{
				if ( pParent == pWnd)
				{
					return i;
				}				
				pParent = pParent->GetParent();
			}
		}		
	}
		
	return -1 ;
}
