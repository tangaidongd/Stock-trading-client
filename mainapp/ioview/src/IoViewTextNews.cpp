// IoViewTextNews.cpp : implementation file
//
#include "stdafx.h"
#include "IoViewTextNews.h"
#include "memdc.h"
#include "IoViewManager.h"
#include "GridCellSys.h"
#include "keyboarddlg.h"
#include "UtilTool.h"
#include "DlgNews.h"
#include "sharestructnews.h"
#include "DlgTodayComment.h"
#include "facescheme.h"
#include "CommentStatusBar.h"
#include "FontFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int32 s_kiGridIdNews	= 73;
static const int32 s_kiNewsCount    = 100;

static const int32 s_kiGridIdTodayComment = 75;

const int32 KNewsUpdateShowCheckTimerId		= 100109;			
const int32 KNewsUpdateShowCheckTimer		= 1000 ;			// 每隔 秒钟, 检测一次十分现实最后一页

const int32 KNewsPushTimerId				= 100111;			// 每隔 秒钟, 检测一次十分现实最后一页
const int32 KNewsPushTimer					= 1000 * 60 *2;		// 两分钟一次

const char* KViewTextShowTab   = "ShowTab";
/////////////////////////////////////////////////////////////////////////////
// CIoViewTextNews

IMPLEMENT_DYNCREATE(CIoViewTextNews, CIoViewBase)

static int st_nViewCount =0;
#define INVALID_ID -1

CIoViewTextNews::CIoViewTextNews()
{
	m_ptScroll.x = 0;
	m_ptScroll.y = 0;
	m_nOldID     = 0;

	m_bAddItem = FALSE;
	m_bAddItemToday = FALSE;
	m_nTiemrID = 0;

	m_unPushTimer = 0;

	m_listNewsTitle.RemoveAll();

	st_nViewCount ++;

	m_listNewUpdate.RemoveAll();
	m_listNewUpdateToday.RemoveAll();

	m_nNewUpdateList = 0;

	m_bUpdating = FALSE;

	m_dwGetDataTick = 0;
	m_dwGetDataTickToday = 0;

	m_iStartPost = 5;

	memset(&m_TodayFont, 0, sizeof(m_TodayFont));
	m_iIDJingping = -1;
	m_iTopBtnHovering = INVALID_ID;
	m_iTabHeight = 25;
	m_bShowTab = TRUE;
	m_iCurrJpSelRow = 0;
	m_iLeftWidth = 0;
	m_iCtlSpace = 0;

	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if ( NULL!=pApp && NULL!=pApp->m_pConfigInfo)
	{
		if (!pApp->m_pConfigInfo->m_bMember)
		{
			m_bShowTab = FALSE;
		}
	}
}

void  CIoViewTextNews::ClearData()
{
	m_listNewsTitle.RemoveAll();
	m_listNewUpdate.RemoveAll();
}

void  CIoViewTextNews::ClearDataToday()
{
	m_listTodayComment.RemoveAll();
	m_listNewUpdateToday.RemoveAll();
}

CIoViewTextNews::~CIoViewTextNews()
{
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if ( NULL != pApp && !pApp->m_bBindHotkey )
	{
		pApp->m_bBindHotkey = true;
	}

	ClearData();
	ClearDataToday();
}

BEGIN_MESSAGE_MAP(CIoViewTextNews, CIoViewBase)
	//{{AFX_MSG_MAP(CIoViewTextNews)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_VSCROLL()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave) 
	ON_MESSAGE(WM_LISTITME_CLICK, OnListItemClick)
	ON_MESSAGE(WM_LISTIME_FOCUS, OnListIsFocus)
	ON_MESSAGE(UM_POS_CHANGED, OnScrollPosChange)
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_DBLCLK, s_kiGridIdNews, OnGridDblClick)
	ON_NOTIFY(NM_DBLCLK, s_kiGridIdTodayComment, OnGridDblClickTodayComment)
END_MESSAGE_MAP()

BOOL CIoViewTextNews::PreTranslateMessage(MSG* pMsg) 
{
	if ( WM_RBUTTONDOWN == pMsg->message )
	{	
		CNewMenu Menu;
		Menu.LoadMenu(IDR_MENU_SINGLE_REPORT);
		Menu.LoadToolBar(g_awToolBarIconIDs);
		CNewMenu* pPopMenu = DYNAMIC_DOWNCAST(CNewMenu, Menu.GetSubMenu(0));

		CMenu* pTempMenu = pPopMenu->GetSubMenu(L"插入内容");
		CNewMenu * pIoViewPopMenu = DYNAMIC_DOWNCAST(CNewMenu, pTempMenu );
		ASSERT(NULL != pIoViewPopMenu );
		CIoViewBase::AppendIoViewsMenu(pIoViewPopMenu, IsLockedSplit());

		// 如果处在锁定分割状态，需要删除一些按钮
		CMPIChildFrame *pChildFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
		if ( NULL != pChildFrame && pChildFrame->IsLockedSplit() )
		{
			pChildFrame->RemoveSplitMenuItem(*pPopMenu);
		}

		//
		CPoint pt;
		GetCursorPos(&pt);

		pPopMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,pt.x, pt.y, AfxGetMainWnd());	
		pPopMenu->DestroyMenu();			

		return TRUE;		
	}

	return FALSE;
}

void CIoViewTextNews::OnPaint() 
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

	if (m_bShowTab)
	{
		DrawTopButton();
	}
	
	//
//  	if ( m_GridNewList.GetSafeHwnd() && m_GridNewList.IsWindowVisible() )
//  	{
//  		m_GridNewList.Invalidate();
//  	}
// 	
// 	if ( m_GridTodayList.GetSafeHwnd() && m_GridTodayList.IsWindowVisible())
// 	{
// 		m_GridTodayList.Invalidate(FALSE);
// 	}
}

int CIoViewTextNews::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	InitialIoViewFace(this);
	
	CreateTables();

	ASSERT( ::IsWindow(m_GridNewList.m_hWnd));
	if( !::IsWindow(m_GridNewList.m_hWnd) )
		return -1;

	//
	if (NULL != m_pAbsCenterManager)
	{
		m_iIDJingping = m_pAbsCenterManager->GetJinpingID();
	}

	//设置今评
	m_XSBVertToday.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10221);
	m_XSBVertToday.SetScrollRange(0, 10);
	
	m_XSBHorzToday.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10222);
	m_XSBHorzToday.SetScrollRange(0, 10);
	
	// 设置相互之间的关联
	m_XSBHorzToday.SetOwner(&m_GridTodayList);
	m_XSBVertToday.SetOwner(&m_GridTodayList);
	m_GridTodayList.SetScrollBar(&m_XSBHorzToday,&m_XSBVertToday);
	
	m_XSBVertToday.SetFaceChemeColor( GetIoViewColor(ESCBackground) );
	m_XSBHorzToday.SetFaceChemeColor( GetIoViewColor(ESCBackground) );
	
	m_XSBHorzToday.ShowWindow(SW_HIDE);
	m_XSBHorzToday.SetRedraw(FALSE);
	//today end
	
	// 创建横滚动条
	m_XSBVert.Create(SBS_VERT|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10219);
	m_XSBVert.SetScrollRange(0, 10);
	
	m_XSBHorz.Create(SBS_HORZ|WS_CHILD|SS_LEFT|SS_NOTIFY|WS_VISIBLE,this,CRect(0,0,0,0), 10219);
	m_XSBHorz.SetScrollRange(0, 10);
	
	// 设置相互之间的关联
	m_XSBHorz.SetOwner(&m_GridNewList);
	m_XSBVert.SetOwner(&m_GridNewList);
	m_GridNewList.SetScrollBar(&m_XSBHorz,&m_XSBVert);
	
	m_XSBVert.SetFaceChemeColor( GetIoViewColor(ESCBackground) );
	m_XSBHorz.SetFaceChemeColor( GetIoViewColor(ESCBackground) );
	
	//
	m_XSBHorz.AddMsgListener(this->GetSafeHwnd());

	SetGridColumnsWidth();

	if( m_nTiemrID == 0 )
	{
		m_nTiemrID = SetTimer(KNewsUpdateShowCheckTimerId,KNewsUpdateShowCheckTimer,NULL);
	}

	if( m_unPushTimer == 0 )
	{
		m_unPushTimer = SetTimer(KNewsPushTimerId,KNewsPushTimer,NULL);
	}

	if (m_bShowTab)
	{
		InitialTopBtns();
	}

	UrlParser urlParser(L"alias://calendar");
	m_wndCef.CreateWndCef(9880,this, CRect(0,0,0,0),urlParser.GetUrl());

	return 1;
}

void CIoViewTextNews::OnSize(UINT nType, int cx, int cy) 
{
	CIoViewBase::OnSize(nType, cx, cy);
	SetGridColumnsWidth();

	CRect rtClient;
	GetClientRect(&rtClient);

	CRect rtGridClient(rtClient);
	rtGridClient.top = rtClient.top ;
	if (m_bShowTab)
	{
		rtGridClient.top += m_iTabHeight;
	}

	rtGridClient.bottom = rtClient.bottom;

	if (m_GridNewList.GetSafeHwnd())
	{
		m_GridNewList.MoveWindow(rtGridClient);
	}

	if (m_GridTodayList.GetSafeHwnd())
	{
		m_GridTodayList.MoveWindow(rtGridClient);
	}

	if (m_wndCef.GetSafeHwnd())
	{
		m_wndCef.MoveWindow(rtGridClient);
	}
	
	if (!m_bShowTab)
	{
		return;
	}
	map<int, CNCButton>::iterator iter;
	for (iter=m_mapTopBtn.begin(); iter!=m_mapTopBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		int32 ID = btnControl.GetControlId();
		
		if (NEWS_MORE_BTN == ID)
		{
			int32 iWidth = 80;
			CRect rcClient;
			GetClientRect(&rcClient);
			CRect rcBtn;
			btnControl.GetRect(rcBtn);
			
			rcBtn.left = rcClient.right - iWidth;
			rcBtn.right = rcBtn.left + iWidth;
			btnControl.SetRect(&rcBtn);
			break;
		}
	}
}

void CIoViewTextNews::OnTimer(UINT nIDEvent) 
{	
	if( m_nTiemrID == nIDEvent )
	{
		int nCount = m_GridNewList.GetRowCount();
		int nCountCol = m_GridNewList.GetColumnCount();
		
		if( ::IsWindow(m_GridNewList.m_hWnd) && nCount > 1 && nCountCol>0)
		{
			if( nCount > 1 && nCountCol>0)
			{
				if( m_GridNewList.IsRowVisible(nCount-1) && m_nOldID >0)
				{
					UpdateDataFromScroll(0, TRUE);
				}
			}
			
			m_nNewUpdateList ++;
			//2秒更新一次
			if( m_nNewUpdateList >= 1 && !m_bUpdating)
				//处理推送闪烁
			{
				int nCt = m_listNewUpdate.GetSize();
				int n;
				for( n = 0 ; n < nCt ; n++ )
				{
					if( m_listNewUpdate[n].m_curUdateTimes >0 )
					{
						m_listNewUpdate[n].m_curUdateTimes--;
						//刷新显示
						SetNewUpateTimes(m_listNewUpdate[n].m_nRow,m_listNewUpdate[n].m_curUdateTimes, m_listNewUpdate[n].m_iInfoIndex);
					}
					else
					{
						//修改为正常颜色
						SetNewUpateTimes(m_listNewUpdate[n].m_nRow,0, m_listNewUpdate[n].m_iInfoIndex);
						//删除一个节点
						if( n == (nCt-1) )
						{
							m_listNewUpdate.RemoveAt(n);
						}
					}
				}
				//金评
				nCt = m_listNewUpdateToday.GetSize();
				for( n = 0 ; n < nCt ; n++ )
				{
					if( m_listNewUpdateToday[n].m_curUdateTimes >0 )
					{
						m_listNewUpdateToday[n].m_curUdateTimes--;
						//刷新显示
						SetNewUpateTimes(m_listNewUpdateToday[n].m_nRow,m_listNewUpdateToday[n].m_curUdateTimes, m_listNewUpdateToday[n].m_iInfoIndex);
					}
					else
					{
						//修改为正常颜色
						SetNewUpateTimes(m_listNewUpdateToday[n].m_nRow,0, m_listNewUpdateToday[n].m_iInfoIndex);
						//删除一个节点
						if( n == (nCt-1) )
						{
							m_listNewUpdateToday.RemoveAt(n);
						}
					}
				}
				
				m_nNewUpdateList = 0;
			}
			
			if( m_nNewUpdateList >5 )
				m_nNewUpdateList = 0;
		}
	}
	else  if( m_unPushTimer == nIDEvent)
	{
		ReuestPushData();
	}
	
	CIoViewBase::OnTimer(nIDEvent);
}

BOOL CIoViewTextNews::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CIoViewTextNews::OnDestroy() 
{
	st_nViewCount--;

	if( m_unPushTimer )
	{
		KillTimer(m_unPushTimer);
		m_unPushTimer = 0;
	}
	if( m_nTiemrID )
	{
		KillTimer(m_nTiemrID);
		m_nTiemrID = 0;
	}
	if( m_pAbsCenterManager && st_nViewCount == 0)
	{
		CMmmiNewsReqDelPush ReqDelPush;
		m_pAbsCenterManager->RequestNews(&ReqDelPush);
	}

	CIoViewBase::OnDestroy();
	// TODO: Add your message handler code here
}

BOOL CIoViewTextNews::TestKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ( IsWindowVisible() && VK_F7 == nChar)
	{
		CMainFrame* pMainFrame		= (CMainFrame*)AfxGetMainWnd();
		CGGTongView* pGGTongView	= GetParentGGtongView();
		
		//
		if ( NULL != pMainFrame && NULL != pGGTongView )
		{
			pMainFrame->OnProcessF7(pGGTongView);
			return TRUE;
		}		
	}
	
	return FALSE;
}

bool32	CIoViewTextNews::FromXml(TiXmlElement * pElement)
{
	if (NULL == pElement)
		return false;
	
	// 判读是不是IoView的节点
	const char *pcValue = pElement->Value();
	if (NULL == pcValue || strcmp(GetXmlElementValue(), pcValue) != 0)
		return false;
	
	// 判断是不是描述自己这个业务视图的节点
	const char *pcAttrValue = pElement->Attribute(GetXmlElementAttrIoViewType());
	if (NULL == pcAttrValue || CIoViewManager::GetIoViewString(this).Compare(CString(pcAttrValue)) != 0)	// 不是描述自己的业务节点
		return false;
	
	// 读取本业务视图特有的内容
	SetColorsFromXml(pElement);
	SetFontsFromXml(pElement);
	SetRowHeightAccordingFont();
	
	// 是否显示顶部工具栏
	m_bShowTab = TRUE;
	pcAttrValue = pElement->Attribute(KViewTextShowTab);
	if (NULL != pcAttrValue && pcAttrValue[0] != '\0' )
	{
		m_bShowTab = atol ( pcAttrValue);
	}

	return true;
}

CString	CIoViewTextNews::ToXml()
{
	CString StrThis;

	// 
	StrThis.Format(L"<%s %s=\"%s\" %s=\"%s\" %s=\"%d\" ",/*>\n*/ // %s=\"%s\" %s=\"%s\" 
		CString(GetXmlElementValue()).GetBuffer(), 
		CString(GetXmlElementAttrIoViewType()).GetBuffer(), 
		CIoViewManager::GetIoViewString(this).GetBuffer(),
		CString(GetXmlElementAttrShowTabName()).GetBuffer(), m_StrTabShowName.GetBuffer(),
		CString(KViewTextShowTab).GetBuffer(),
		m_bShowTab
		);

	//
	CString StrFace;
	StrFace  = SaveColorsToXml();
	StrFace += SaveFontsToXml();
	
	StrThis += StrFace;
	StrThis += L">\n";
	//
	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";
	
	return StrThis;
}

CString	CIoViewTextNews::GetDefaultXML()
{
	CString StrThis;
	
	// 
	StrThis.Format(L"<%s %s=\"%s\"  >\n", //%s=\"%s\" %s=\"%s\"
		CString(GetXmlElementValue()).GetBuffer(), 
		CString(GetXmlElementAttrIoViewType()).GetBuffer(), 
		CIoViewManager::GetIoViewString(this).GetBuffer());

	StrThis += L"</";
	StrThis += GetXmlElementValue();
	StrThis += L">\n";
	
	return StrThis;
}

void CIoViewTextNews::SetChildFrameTitle()
{
	CString StrTitle;
	StrTitle =  CIoViewManager::FindIoViewObjectByIoViewPtr(this)->m_StrLongName;
		
	CMPIChildFrame * pParentFrame = DYNAMIC_DOWNCAST(CMPIChildFrame, GetParentFrame());
	if ( NULL != pParentFrame)
	{
		pParentFrame->SetChildFrameTitle(StrTitle);
	}
}

void CIoViewTextNews::OnIoViewActive()
{
	CIoViewBase::OnIoViewActive();

	SetChildFrameTitle();

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if (!m_bShowTab)
	{
		PostMessage(WM_COMMAND, pMainFrame->m_pNewWndTB->m_indexNews);
	}
	else
	{
		if (NULL != pMainFrame && NULL != pMainFrame->m_pNewWndTB)
		{
			map<int, CNCButton>::iterator iter;
			for (iter=m_mapTopBtn.begin(); iter!=m_mapTopBtn.end(); ++iter)
			{
				CNCButton &btnControl = iter->second;
				int32 ID = btnControl.GetControlId();
				bool32 bStatus = btnControl.GetCheck();
				if (ID == pMainFrame->m_pNewWndTB->m_indexNews)
				{
					btnControl.SetCheck(TRUE);
				}
				else if (bStatus)
				{
					btnControl.SetCheckStatus(FALSE);
				}
			}
		}
	}

	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if ( NULL != pApp )
	{
		pApp->m_bBindHotkey = false; //关掉键盘精灵
	}
}

void CIoViewTextNews::OnIoViewDeactive()
{
	CIoViewBase::OnIoViewDeactive();

	if ( m_GridNewList.GetSafeHwnd() )
	{
		m_GridNewList.RedrawWindow();
	}

	//
	CGGTongApp* pApp = (CGGTongApp*)AfxGetApp();
	if ( NULL != pApp && !pApp->m_bBindHotkey )
	{
		pApp->m_bBindHotkey = true;
	}
}

void CIoViewTextNews::LockRedraw()
{
	if ( m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = true;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);
	::SendMessage(m_GridNewList.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);	
	::SendMessage(m_GridTodayList.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(FALSE), 0L);	
}

void CIoViewTextNews::UnLockRedraw()
{
	if ( !m_bLockRedraw )
	{
		return;
	}
	
	m_bLockRedraw = false;
	::SendMessage(GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
	::SendMessage(m_GridNewList.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
	::SendMessage(m_GridTodayList.GetSafeHwnd(), WM_SETREDRAW, (WPARAM)(TRUE), 0L);
}

void CIoViewTextNews::OnVDataForceUpdate()
{
	RequestViewData();
}

void CIoViewTextNews::RequestViewData( )
{
	ASSERT( m_pAbsCenterManager );
	if( m_pAbsCenterManager )
	{
		// 请求普通的新闻标题
 		CMmiNewsReqTitleList NewsReqTitle;
		NewsReqTitle.m_iInfoIndex = KiIndexNormal;
 		NewsReqTitle.m_iBeginPos = 0;
 		NewsReqTitle.m_iNeedCounts = s_kiNewsCount;
 		
 		m_pAbsCenterManager->RequestNews(&NewsReqTitle);

		// 请求金评的新闻标题
		CMmiNewsReqTitleList NewsReqJinpingTitle;
		NewsReqJinpingTitle.m_iInfoIndex = m_iIDJingping;
		NewsReqJinpingTitle.m_iBeginPos = 0;
		NewsReqJinpingTitle.m_iNeedCounts = s_kiNewsCount;
		
		m_pAbsCenterManager->RequestNews(&NewsReqJinpingTitle);

		ReuestPushData(false);
	}
}

void CIoViewTextNews::RequestViewDatFrom(int nBegin)
{
	ASSERT( m_pAbsCenterManager );
	if( m_pAbsCenterManager )
	{
		//请求新闻标题
		CMmiNewsReqTitleList NewsReqTitle;
		NewsReqTitle.m_iBeginPos = nBegin;
		NewsReqTitle.m_iNeedCounts = s_kiNewsCount;
		
		m_pAbsCenterManager->RequestNews(&NewsReqTitle);
	}
}

// 字体发生变化
void CIoViewTextNews::OnIoViewFontChanged()
{
	CIoViewBase::OnIoViewFontChanged();
	SetColorAndFont(TRUE);
	SetGridColumnsWidth();
	SetRowHeightAccordingFont();
	/*
	LOGFONT *pFontNormal = GetIoViewFont(ESFText);
	if (NULL == pFontNormal)
	{
		return;
	}

	//
	
	CSize szText2 = m_GridNewList.GetColumuExtent(pFontNormal);
	float flv = 1.0f;
	if( m_szTextSize.cx >0)
		flv = (float)szText2.cx/(float)m_szTextSize.cx;
	if( m_szTextSize.cx != szText2.cx)
	{
		m_GridNewList.AutoSizeColumnslv(flv);
		m_szTextSize = szText2;
	}
	
	m_GridNewList.Refresh();
	m_GridNewList.Invalidate();
	*/
	m_GridNewList.Invalidate();
	m_GridTodayList.Invalidate();
}

void CIoViewTextNews::OnIoViewColorChanged()
{
	CIoViewBase::OnIoViewColorChanged();
	
	SetColorAndFont(false);
	
	m_GridNewList.Invalidate();
	m_GridTodayList.Invalidate();
}

//lxp 通知界面更新
bool CIoViewTextNews::UiNotifyUpdate()
{
	SetColorAndFont(false);
	
	if ( m_GridNewList.GetSafeHwnd() )
	{
		m_GridNewList.RedrawWindow();
	}
		
	m_GridNewList.Invalidate();

	return true;
}

// 通知视图改变关注的商品
void CIoViewTextNews::OnVDataMerchChanged(int32 iMarketId, const CString &StrMerchCode, IN CMerch *pMerch)
{
	//CIoViewBase::UpdateIoViewActionOnVDataMerchChanged(iMarketId, StrMerchCode, pMerch);
}

CSize CIoViewTextNews::GetMinGridPriceSize(const bool &bSingle)
{
	return CSize(0,0);
}

void CIoViewTextNews::ReaclcLayOut()
{
	CRect rtClient;
	GetClientRect(rtClient);
	
	if (rtClient.IsRectEmpty())
	{
		return;
	}

	CSize szMin = GetMinGridPriceSize(true);

	CRect m_RectGridNewlist = rtClient;	

	CRect rtScroll(m_RectGridNewlist);
	rtScroll.top = rtScroll.bottom - 18 - 28;
	
	m_RectGridNewlist.top = rtScroll.top + 200;
	m_RectGridNewlist.bottom = rtScroll.top;
	m_GridNewList.MoveWindow(m_RectGridNewlist);
	
	// 判断是不是需要拉伸一下
	//if ( m_RectGridNewlist.Width() >= szMin.cx )			
	//{
	//	m_GridDay.ExpandColumnsToFit();	
	//}

	m_GridNewList.Invalidate();

	Invalidate(TRUE);
}

void CIoViewTextNews::CreateTables()
{
	if ( !m_GridNewList.GetSafeHwnd() )
	{		
		m_GridNewList.Create(CRect(0, 0, 0, 0), this, s_kiGridIdNews);
		m_GridNewList.IsCross(FALSE);
		m_GridNewList.SetColsActive(1);
		m_GridNewList.SetListTextColor(RGB(183, 183, 183), RGB(238,69,2));
	}

	if ( !m_GridTodayList.GetSafeHwnd())
	{
		m_GridTodayList.Create(CRect(0, 0, 0, 0), this, s_kiGridIdTodayComment);
		m_GridTodayList.IsCross(FALSE);		
		m_GridTodayList.SetColsActive(1);
		CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
		m_GridTodayList.SetListTextColor(pMain->m_wndCommentSBar.m_clrNorText, pMain->m_wndCommentSBar.m_clrSelText);
		m_GridTodayList.ShowWindow(FALSE);		
	}
}

void CIoViewTextNews::DeleteTableContent()
{
	if ( NULL != m_GridNewList.GetSafeHwnd() /*&& !m_bShowFull*/ )
	{
		m_GridNewList.DeleteAllItems();
	}		
	
	SetRowHeightAccordingFont();

	RedrawWindow();
}

void CIoViewTextNews::SetTableDataContent( IN listNewsInfoPtr& listTitles)
{
	if ( m_bDelByBiSplitTrack )
	{
		LockRedraw();
		return;
	}
	else
	{
		UnLockRedraw();
	}
	
	//
	if ( GetParentGGTongViewDragFlag() )
	{
		LockRedraw();		
		return;
	}
	else
	{
		UnLockRedraw();		
	}
	
	if( listTitles.GetSize() <= 0 )
	{
		return;
	}
	
	int nCountRow = 0;
	int nTempRow = 0;
	
	{
		m_GridNewList.DeleteAllItems();
		nCountRow = m_GridNewList.GetRowCount();
		nTempRow = listTitles.GetSize();
	}
	
	int32 iRowCount = nTempRow;//m_GridNewList.GetRowCount();
	for( int i = nCountRow ; i < iRowCount; i++ )
	{
		T_NewsInfo *NewInfo = &listTitles[i-nCountRow];
		ASSERT( NewInfo->m_iIndexID >=0 );
		
		m_GridNewList.InsertRow(i);
		SetRowValue(NewInfo, i, 0, KiIndexNormal);
		if( i == (iRowCount -1) )
		{
			m_nOldID = iRowCount;//NewInfo.m_iIndexID;
		}
	}

	SetGridColumnsWidth();
	m_GridNewList.Invalidate();
	SetRowHeightAccordingFont();		
}

void CIoViewTextNews::SetTableDataContentTodayComment( IN listNewsInfoPtr& listTitles)
{
	if ( m_bDelByBiSplitTrack )
	{
		LockRedraw();
		return;
	}
	else
	{
		UnLockRedraw();
	}
	
	//
	if ( GetParentGGTongViewDragFlag() )
	{
		LockRedraw();		
		return;
	}
	else
	{
		UnLockRedraw();		
	}
	
	if( listTitles.GetSize() <= 0 )
	{
		return;
	}
	
	int nCountRow = 0;
	int nTempRow = 0;
	
	{
		m_GridTodayList.DeleteAllItems();
		nCountRow = m_GridTodayList.GetRowCount();
		nTempRow = listTitles.GetSize();
	}
	
	int32 iRowCount = nTempRow;//m_GridTodayList.GetRowCount();
	for( int i = nCountRow ; i < iRowCount; i++ )
	{
		T_NewsInfo *NewInfo = &listTitles[i-nCountRow];
		//
		ASSERT( NewInfo->m_iIndexID >=0 );
		
		m_GridTodayList.InsertRow(i);
		SetRowValue(NewInfo, i, 0, m_iIDJingping);
		
		if( i == (iRowCount -1) )
		{
			m_nOldID = iRowCount;//NewInfo.m_iIndexID;
		}
	}
	SetGridColumnsWidth();
	SetRowHeightAccordingFont();
	m_GridTodayList.Invalidate();
}

void CIoViewTextNews::SetRowValue(T_NewsInfo *pNewInfo, int32 iRowIndex,int32 iColInex, int32 iInfoIndex)
{
	if( NULL == pNewInfo )
		return;
	
	LOGFONT* pFontNews   = GetIoViewFont(ESFText);
	memcpyex(&m_TodayFont, pFontNews, sizeof(m_TodayFont));
	LOGFONT* pFontToday  = &m_TodayFont;
	LOGFONT* pFontCell	 = NULL;

	//
	COLORREF  clrRef;
	if ( m_iIDJingping == iInfoIndex )
	{
		clrRef = RGB(255, 255, 60);
		pFontCell = pFontToday;
	}
	else
	{
		clrRef = GetIoViewColor(ESCText);
		pFontCell = pFontNews;
	}

	CString strShowText = pNewInfo->m_StrTimeUpdate;
	CTime timeTemp;
	if( 19 ==strShowText.GetLength())
	{
		CUtilTool::GetInistance().CharToTime(strShowText,timeTemp);
		SYSTEMTIME systime;	 
		GetLocalTime(&systime);	//本地时间
		if (systime.wDay == timeTemp.GetDay())
		{
			strShowText = timeTemp.Format(L"%H:%M");//Format(L"%Y-%m-%d %H:%M:%S");
		}
		else
		{
			strShowText = timeTemp.Format(L"%m-%d");//Format(L"%Y-%m-%d %H:%M:%S");
		}
	}
	if (KiIndexNormal == iInfoIndex)
	{
		m_GridNewList.SetColsFont(2, pFontCell);
		m_GridNewList.SetItemValue(iRowIndex, 2, strShowText);

		m_GridNewList.SetColsFont(1, pFontCell);
		strShowText = pNewInfo->m_StrContent;
		m_GridNewList.SetItemValue(iRowIndex, 1, strShowText, LPARAM(pNewInfo));

		m_GridNewList.SetItemValue(iRowIndex, 0,  L"■");
	}
	else
	{
		m_GridTodayList.SetColsFont(2, pFontCell);
		m_GridTodayList.SetItemValue(iRowIndex, 2, strShowText);
		
		m_GridTodayList.SetColsFont(1, pFontCell);
		strShowText = pNewInfo->m_StrContent;
		m_GridTodayList.SetItemValue(iRowIndex, 1, strShowText, LPARAM(pNewInfo));

		m_GridTodayList.SetItemValue(iRowIndex, 0,  L"■");
	}
}

void CIoViewTextNews::SetRowHeightAccordingFont()
{
//	m_GridNewList.AutoSizeRows();
//	m_GridTodayList.AutoSizeRows();
}

void CIoViewTextNews::SetGridColumnsWidth()
{
	CRect rtClient;
	GetClientRect(&rtClient);

	if (rtClient.Width() <= 0 || rtClient.Height() <= 0)
	{
		rtClient.left = 0;
		rtClient.right = 500;
	}
	//
	CFont ft;
	ft.CreateFontIndirect(&m_TodayFont);

	CSize szText = GetFontSize(L" 01-01 ", &ft);
	szText.cx += 10;
	int32 iColWidth1 = 30;

	//
	if ( NULL != m_GridTodayList.GetSafeHwnd() )
	{
		m_GridTodayList.SetColumnWidth(0, iColWidth1);
		m_GridTodayList.SetColumnWidth(1, rtClient.Width() - szText.cx - iColWidth1);
		m_GridTodayList.SetColumnWidth(2, szText.cx);
	}

	if ( NULL != m_GridNewList.GetSafeHwnd() )
	{
		m_GridNewList.SetColumnWidth(0, iColWidth1);
		m_GridNewList.SetColumnWidth(1, rtClient.Width() - szText.cx - iColWidth1);
		m_GridNewList.SetColumnWidth(2, szText.cx);
	}	
	ft.DeleteObject();
}

void CIoViewTextNews::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	if ( NULL == m_pAbsCenterManager )
	{
		return;	
	}

    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	if( NULL == pItem)
		return;

	int32 iRow = pItem->iRow;
	if( iRow <0 || iRow >= m_GridNewList.GetRowCount())
		return;

// 	// 通知商品发生了改变
// 	CGridCellSys *pCell = (CGridCellSys *)m_GridNewList.GetCell(iRow, 1);
// 	if ( NULL == pCell || iRow < m_GridNewList.GetFixedRowCount() )
// 	{
// 		return;
// 	}
// 	
// 	T_NewsInfo *pNewsInfo = (T_NewsInfo *)pCell->GetData();
// 	
// 	if ( pNewsInfo->m_iIndexID <= 0)
// 		return;
// 	if( pNewsInfo->m_StrTimeUpdate.IsEmpty())
// 			return ;
// 
// 	m_nDeitaNew = *pNewsInfo;
// 	//
// 	if( m_pAbsCenterManager )
// 	{
// 		CMmiNewsReqNewsContent NewsReqNewsContent ;
// 		NewsReqNewsContent.m_iIndexID = pNewsInfo->m_iIndexID;
// 		m_pAbsCenterManager->RequestNews(&NewsReqNewsContent);
// 	}
}

void CIoViewTextNews::OnGridDblClickTodayComment(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	if ( NULL == m_pAbsCenterManager )
	{
		return;	
	}
	
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	if( NULL == pItem )
		return;
	
	int32 iRow = pItem->iRow;
	if( iRow <0 || iRow >= m_GridTodayList.GetRowCount())
		return;
	
	// 通知商品发生了改变
// 	CGridCellSys *pCell = (CGridCellSys *)m_GridTodayList.GetCell(iRow, 1);
// 	if ( NULL == pCell || iRow < m_GridTodayList.GetFixedRowCount() )
// 	{
// 		return;
// 	}
// 	
// 	T_NewsInfo *pNewsInfo = (T_NewsInfo *)pCell->GetData();
// 	if ( NULL == pNewsInfo )
// 	{
// 		//ASSERT(0);
// 		return;
// 	}
// 	else 
// 	{
// 		if( pNewsInfo->m_iIndexID <= 0 )
// 			return ;
// 		if( pNewsInfo->m_StrTimeUpdate.IsEmpty())
// 			return ;
// 	}
// 	
// 	m_nDeitaNew = *pNewsInfo;
// 	//
// 	if( m_pAbsCenterManager )
// 	{
// 		CMmiNewsReqNewsContent NewsReqNewsContent ;
// 		NewsReqNewsContent.m_iIndexID = pNewsInfo->m_iIndexID;
// 		m_pAbsCenterManager->RequestNews(&NewsReqNewsContent);
// 	}
	
}

void CIoViewTextNews::OnVDataNewsTitleUpdate(IN listNewsInfo& listTitles)
{
	if( listTitles.GetSize() <= 0 )
	{
		return ;
	}

	int32 iInfoIndex = listTitles.GetAt(0).m_iInfoIndex;

	if (KiIndexNormal == iInfoIndex)
	{
		if( !m_bAddItem )
		{
			DWORD dwTick = ::GetTickCount();
			if( m_dwGetDataTick != 0 && ( dwTick - m_dwGetDataTick ) < 30*1000  && m_listNewsTitle.GetSize() >0 )
			{
				return ;
			}
			
			m_dwGetDataTick = dwTick;
			
			ClearData();
		}
		else
		{
			m_bAddItem = FALSE;
		}	
	}

	if (m_iIDJingping == iInfoIndex)
	{
		if( !m_bAddItemToday )
		{
			DWORD dwTick = ::GetTickCount();
			if( m_dwGetDataTickToday != 0 && ( dwTick - m_dwGetDataTickToday ) < 30*1000  && m_listTodayComment.GetSize() >0 )
			{
				return ;
			}
			
			m_dwGetDataTickToday = dwTick;
			
			ClearDataToday();//今评
		}
		else
		{
			m_bAddItemToday = FALSE;
		}	
	}

	//
	for( int i =0 ; i < listTitles.GetSize(); i++)
	{
		
		T_NewsInfo Temp = listTitles.GetAt(i);

		if ( m_iIDJingping == Temp.m_iInfoIndex)
		{
			m_listTodayComment.Add(Temp);
		}
		else 
		{
			m_listNewsTitle.Add(Temp);			
		}		
	}

	if (KiIndexNormal == iInfoIndex)
	{
		SetTableDataContent( m_listNewsTitle );
	}
	else if (m_iIDJingping == iInfoIndex)
	{
		SetTableDataContentTodayComment(m_listTodayComment);
	}
}

void CIoViewTextNews::OnVDataNewsContentUpdate(const T_NewsInfo& stNewsInfo)
{
	if( !m_bActive )
		return ;

	if (stNewsInfo.m_StrContent.GetLength() <= 0)
	{
		static const CString StrErr = L"抱歉，您访问的页面已经失效。";

		CDlgNews dlg;
		dlg.m_clrText = GetIoViewColor(ESCText);
		dlg.m_clrBackground = GetIoViewColor(ESCBackground);
		dlg.SetTitle(L"");
		dlg.SetNewsText(StrErr);
		
		CString strShowText = stNewsInfo.m_StrTimeUpdate;
		CTime timeTemp;
		CUtilTool::GetInistance().CharToTime(strShowText,timeTemp);
		strShowText = timeTemp.Format(L"%m/%d/%Y %H:%M");
		dlg.SetstrTime(L"");
		
		dlg.DoModal();
		//
		return;
	}

	if( m_nDeitaNew.m_iIndexID != stNewsInfo.m_iIndexID)
		return ;

	static const CString StrWeb1 = "www.";
	static const CString StrWeb2 = "http";

	CString StrContentHead = stNewsInfo.m_StrContent.Left(4);

	if ( 0 == StrContentHead.CompareNoCase(StrWeb1) || 0 == StrContentHead.CompareNoCase(StrWeb2) )
	{
// 		CDlgTodayCommentIE  dlgToday;
// 		dlgToday.SetTitleUrl(L"金评", stNewsInfo.m_StrContent);
// 		dlgToday.DoModal();

		CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
		if (NULL == pMain->m_pDlgJPNews  || NULL == pMain->m_pDlgJPNews->m_hWnd)
		{
			pMain->m_pDlgJPNews  = new CDlgJPNews;
			pMain->m_pDlgJPNews ->Create(IDD_DIALOG_JP_NEWS, this);
			pMain->m_pDlgJPNews ->CenterWindow();
		}
		
		CGuiTabWnd	*pGuiTopTabWnd = pMain->m_pDlgJPNews->GetTabWnd();
		int32 iPos = m_iCurrJpSelRow;//m_GridTodayList.GetCurrentRow();
		CString strCaption = m_GridTodayList.GetItemText(iPos, 1);
		CString strShort = strCaption.Left(7) + L"...";
		int iTabCnt = pMain->m_pDlgJPNews->GetTabCount();

		int iFind = -1;
		int iCnt = pGuiTopTabWnd->GetCount();
		for (int i=0; i<iCnt; i++)
		{
			if (strCaption == pGuiTopTabWnd->GetTabsTitle(i))  // 已经存在
			{
				if ((i+iTabCnt-1) <= iCnt)
				{
					pGuiTopTabWnd->SetShowBeginIndex(i, i+iTabCnt-1);
				}
				iFind = i;
				break;
			}
		}

		if (iFind == -1)
		{
			pGuiTopTabWnd->Addtab(strCaption, strShort, strCaption, stNewsInfo.m_StrContent);
			iFind = pGuiTopTabWnd->GetCount()-1;
		}
		
		pGuiTopTabWnd->SetCurtab(iFind);
		pMain->m_pDlgJPNews ->SetTabSize();
		pMain->m_pDlgJPNews ->ShowWindow(SW_SHOW);
		m_nDeitaNew.m_iIndexID = -1;
	}
	else
	{
		CDlgNews dlg;
		dlg.m_clrText = GetIoViewColor(ESCText);
		dlg.m_clrBackground = GetIoViewColor(ESCBackground);
		dlg.SetTitle( m_nDeitaNew.m_StrContent );
		dlg.SetNewsText(stNewsInfo.m_StrContent);
		
		CString strShowText = stNewsInfo.m_StrTimeUpdate;
		CTime timeTemp;
		CUtilTool::GetInistance().CharToTime(strShowText,timeTemp);
		strShowText = timeTemp.Format(L"%m/%d/%Y %H:%M");
		dlg.SetstrTime(strShowText);
		
		dlg.DoModal();
		m_nDeitaNew.m_iIndexID = -1;
	}
}
	
void CIoViewTextNews::OnVDataNewsPushTitle(const T_NewsInfo& stNewsInfo)
{
	//
	TRACE(L"PUSH NEW MSG: %d %s %s \n", stNewsInfo.m_iIndexID, CString(stNewsInfo.m_StrTimeUpdate).GetBuffer(), CString(stNewsInfo.m_StrContent).GetBuffer());		

	m_bUpdating = TRUE;
	
	T_NewsInfo  newsInfo = stNewsInfo;
	T_NewsInfo *pNewsInfo = NULL;
	if( newsInfo.m_iIndexID <= 0 )
	{
		return;
	}
	if( newsInfo.m_StrTimeUpdate.IsEmpty())
	{
		return ;
	}
	
	int nIndex = 0;

	if (KiIndexNormal == stNewsInfo.m_iInfoIndex)
	{
		m_listNewsTitle.Add(newsInfo);
		pNewsInfo = &m_listNewsTitle[m_listNewsTitle.GetSize() - 1];
		nIndex = m_GridNewList.InsertRow(0);
	}
	else if(m_iIDJingping == stNewsInfo.m_iInfoIndex)
	{
		m_listTodayComment.Add(newsInfo);
		pNewsInfo = &m_listTodayComment[m_listTodayComment.GetSize() - 1];
		nIndex = m_GridTodayList.InsertRow(0);
	}

	//增加闪烁处理
	{
		T_NewsInfoSAUpdate NewsInfoSAUpdate;
		NewsInfoSAUpdate.m_iIndexID = newsInfo.m_iIndexID;
		NewsInfoSAUpdate.m_nRow     = nIndex;
		NewsInfoSAUpdate.m_curUdateTimes = 7;
		NewsInfoSAUpdate.m_iInfoIndex = stNewsInfo.m_iInfoIndex;

		if ( KiIndexNormal == stNewsInfo.m_iInfoIndex )
		{
			int nCount = m_listNewUpdate.GetSize();
			for(int n =0 ; n < nCount ; n++ )
			{
				m_listNewUpdate[n].m_nRow +=1;
			}
			m_listNewUpdate.Add(NewsInfoSAUpdate);
		}
		else
		{
			int nCount = m_listNewUpdateToday.GetSize();
			for(int n =0 ; n < nCount ; n++ )
			{
				m_listNewUpdateToday[n].m_nRow +=1;
			}
			m_listNewUpdateToday.Add(NewsInfoSAUpdate);
		}		
	}
	
	//闪烁处理结束
	
	SetRowValue( pNewsInfo, nIndex, 0, stNewsInfo.m_iInfoIndex);
	SetRowHeightAccordingFont();

	m_bUpdating = FALSE;
	
	if (KiIndexNormal == stNewsInfo.m_iInfoIndex)
	{
		m_GridNewList.Invalidate();
	}
	else if(m_iIDJingping == stNewsInfo.m_iInfoIndex)
	{
		m_GridTodayList.Invalidate();
	}
}

void CIoViewTextNews::SetColorAndFont(bool bFont)
{
	if( !::IsWindow(m_GridNewList.m_hWnd))
		return;

	if(bFont)
	{
		// 设置字体
		LOGFONT *pFontNormal = GetIoViewFont(ESFText);
		
		m_GridNewList.SetColsFont(1, pFontNormal);
		m_GridNewList.SetColsFont(2, pFontNormal);
		
		//
		memcpyex(&m_TodayFont, pFontNormal, sizeof(m_TodayFont));
		if (pFontNormal->lfHeight<0)
		{
			m_TodayFont.lfHeight -= 2;
		}
		else
		{
			m_TodayFont.lfHeight += 2;
		}

		//
		m_GridTodayList.SetColsFont(1, pFontNormal);
		m_GridTodayList.SetColsFont(2, pFontNormal);
	}
	//else
	//{
	//	// 颜色
	//}
}

void CIoViewTextNews::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{	
	m_ptScroll.y = nPos;
	
	UpdateDataFromScroll(m_ptScroll.y);
	
	CIoViewBase::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CIoViewTextNews::UpdateDataFromScroll(int nPos,bool bForceUpdate)
{
	if( m_nOldID <= 0 )
		return;

	if( bForceUpdate )
	{
		m_bAddItem = TRUE;
		m_bAddItemToday = TRUE;
		RequestViewDatFrom(m_nOldID);
		m_nOldID = 0;
	}
	else
	{
		CRect rcClient;
		GetClientRect(&rcClient);

		CSize szHight = GetGridSize();
		if(  (szHight.cy - nPos) < (2* rcClient.Width()) )
		{
			m_bAddItem = TRUE;
			m_bAddItemToday = TRUE;
			RequestViewDatFrom(m_nOldID);
			m_nOldID = 0;
		}
	}
}

CSize CIoViewTextNews::GetGridSize()
{
	int nHeight = 0;
	int nCount = m_GridNewList.GetRowCount();
	for( int n = 0 ; n < nCount ; n++ )
	{
		nHeight += m_GridNewList.GetRowHeight(n);
	}
	return CSize(0,nHeight);
}

void CIoViewTextNews::ReuestPushData(bool bPushUpdate)
{
	if( m_pAbsCenterManager )
	{		
		if( bPushUpdate )
		{
			CMmmiNewsReqUpdatePush ReqUpdatePush;
			ReqUpdatePush.m_iIndexID = KiIndexNormal;
			m_pAbsCenterManager->RequestNews(&ReqUpdatePush);

			CMmmiNewsReqUpdatePush ReqUpdatePushJinping;
			ReqUpdatePushJinping.m_iIndexID = m_iIDJingping;
			m_pAbsCenterManager->RequestNews(&ReqUpdatePushJinping);
		}
		else
		{
			//请求推送
			CMmmiNewsReqAddPush NewsReqAddPush;
			m_pAbsCenterManager->RequestNews(&NewsReqAddPush);

			CMmmiNewsReqAddPush NewsReqAddPushJinping;
			NewsReqAddPushJinping.m_iIndexID = m_iIDJingping;
			m_pAbsCenterManager->RequestNews(&NewsReqAddPushJinping);
		}
	}
}

void CIoViewTextNews::SetNewUpateTimes(int nRow,int nTimes, int32 iInfoIndex)
{
// 	COLORREF clrProperty = GetIoViewColor(ESCText);
// 	COLORREF clrBackGround  = GetIoViewColor(ESCBackground);
// 	
// 	int32 iRowCount = 0;
// 	if (KiIndexNormal == iInfoIndex)
// 		iRowCount = m_GridNewList.GetRowCount();
// 	else
// 		iRowCount = m_GridTodayList.GetRowCount();
// 
// 	if( nRow < 0 || nRow >iRowCount )
// 		return;
// 
// 	if ( m_iIDJingping == iInfoIndex )
// 	{
// 		clrProperty = RGB(255, 255, 60);
// 	}
//  
// 	{
// 		CGridCellSys *pCell = NULL;
// 		if ( KiIndexNormal == iInfoIndex)
// 		 pCell = (CGridCellSys *)m_GridNewList.GetCell(nRow, 0);
// 		else 
// 			pCell = (CGridCellSys *)m_GridTodayList.GetCell(nRow, 0);
// 
// 		if( NULL != pCell )
// 		{
// 			pCell->SetBackClr(clrBackGround);
// 			if( nTimes == 0 )
// 			{
// 				pCell->SetTextClr( clrProperty);
// 			}
// 			else
// 			{
// 				pCell->SetTextClr(GetAniMovingColor(clrProperty,clrBackGround,nTimes) );
// 			}
// 		}
// 	}
// 	
// 	{
// 		CGridCellSys *pCell = NULL;
// 		if ( KiIndexNormal == iInfoIndex)
// 			pCell = (CGridCellSys *)m_GridNewList.GetCell(nRow, 1);
// 		else
// 			pCell = (CGridCellSys *)m_GridTodayList.GetCell(nRow, 1);
// 
// 		if( NULL  != pCell )
// 		{
// 			pCell->SetBackClr(clrBackGround);
// 			if( nTimes == 0 )
// 			{
// 				pCell->SetTextClr( clrProperty);
// 			}
// 			else
// 			{
// 				pCell->SetTextClr(GetAniMovingColor(clrProperty,clrBackGround,nTimes) );
// 			}
// 		}
// 	}
// 	
// 	if ( KiIndexNormal == iInfoIndex )
// 	{
// 		m_GridNewList.RedrawRow(nRow);
// 		m_GridNewList.Refresh();
// 	}
// 	else
// 	{
// 		m_GridTodayList.RedrawRow(nRow);
// 		m_GridTodayList.Refresh();
// 	}
}

COLORREF CIoViewTextNews::GetAniMovingColor(COLORREF ClrStart, COLORREF ClrEnd,int nStep)
{
	COLORREF ClrSmooth;
	if(nStep%2)
	{
		ClrSmooth = ClrStart;
	}
	else
	{
		COLORREF clFlicker = GetIoViewColor(ESCAmount);
		if( clFlicker == ClrStart)
		{
			ClrSmooth = 0x00FFFFFF - clFlicker;
		}
		else
		{
			ClrSmooth = clFlicker;
		}
	}
	return ClrSmooth;
}

CSize CIoViewTextNews::GetFontSize(CString strValue, CFont *pFont)
{
	CClientDC dc(this);
	CFont* pFontOld = dc.SelectObject(pFont);
	CSize sz = dc.GetTextExtent(strValue);
	dc.SelectObject(pFontOld);
	return sz;
}

void CIoViewTextNews::AdditionalAfterPaste()
{
   OnVDataForceUpdate();
}

LRESULT CIoViewTextNews::OnScrollPosChange(WPARAM w, LPARAM l)
{
// 	int iPos = m_XSBHorz.GetScrollPos();
// 	m_GridTodayList.SetScrollPos32(SB_HORZ, iPos);
// 	
// 	m_GridTodayList.Refresh();
// 	m_GridTodayList.RedrawWindow();
	
	return 1;
}

BOOL CIoViewTextNews::OnCommand(WPARAM wParam, LPARAM lParam)
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	switch (wParam)
	{
	case NEWS_JP_BTN:
		{
			pMainFrame->m_pNewWndTB->m_indexNews = 0;
		//	m_GridTodayList.SetScrollPos32(SB_VERT,0);
			m_GridNewList.ShowWindow(FALSE);
			m_GridTodayList.ShowWindow(TRUE);
			break;
		}
	case NEWS_GJS_BTN:
		{
			pMainFrame->m_pNewWndTB->m_indexNews = 1;
		//	m_GridNewList.SetScrollPos32(SB_VERT,0);
			m_GridNewList.ShowWindow(TRUE);
			m_GridTodayList.ShowWindow(FALSE);
			break;
		}
	case NEWS_CJRL_BTN:
		{
			pMainFrame->m_pNewWndTB->m_indexNews = 2;
			m_GridNewList.ShowWindow(FALSE);
			m_GridTodayList.ShowWindow(FALSE);

			break;
		}
	case NEWS_MORE_BTN:
		{
			CString strUrl = L"http://www.cfkd.net/";
			ShellExecute(0, L"open", strUrl, NULL, NULL, SW_NORMAL);
			break;
		}
	}

	return CIoViewBase::OnCommand(wParam, lParam);
}

void CIoViewTextNews::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (!m_bShowTab)
	{
		return;
	}

	int iButton = TTopButtonHitTest(point);
	if (iButton != m_iTopBtnHovering)
	{
		if (INVALID_ID != m_iTopBtnHovering)
		{
			m_mapTopBtn[m_iTopBtnHovering].MouseLeave();
			m_iTopBtnHovering = INVALID_ID;
		}
		
		if (INVALID_ID != iButton)
		{	
			m_iTopBtnHovering = iButton;
			m_mapTopBtn[m_iTopBtnHovering].MouseHover();
		}
	}

	// 响应 WM_MOUSELEAVE消息
	TRACKMOUSEEVENT csTME;
	csTME.cbSize	= sizeof (csTME);
	csTME.dwFlags	= TME_LEAVE;
	csTME.hwndTrack = m_hWnd ;		// 指定要追踪的窗口 
	::_TrackMouseEvent (&csTME);	// 开启Windows的WM_MOUSELEAVE事件支持 
	
	CIoViewBase::OnMouseMove(nFlags, point);
}

LRESULT CIoViewTextNews::OnMouseLeave(WPARAM wParam, LPARAM lParam)       
{     	
	if (INVALID_ID != m_iTopBtnHovering)
	{
		m_mapTopBtn[m_iTopBtnHovering].MouseLeave();
		m_iTopBtnHovering = INVALID_ID;
	}

	return 0;       
}   

void CIoViewTextNews::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (!m_bShowTab)
	{
		CIoViewBase::OnLButtonDown(nFlags, point);
	}

	int32 iButton = TTopButtonHitTest(point);
	if (INVALID_ID != iButton)
	{
		if (NEWS_MORE_BTN == iButton)
		{
			m_mapTopBtn[iButton].LButtonDown();
		}
		else
		{
			bool32 bCheck = m_mapTopBtn[iButton].GetCheck();
			if (!bCheck)
			{
				m_mapTopBtn[iButton].SetCheck(TRUE);
			}
			
			map<int, CNCButton>::iterator iter;
			for (iter=m_mapTopBtn.begin(); iter!=m_mapTopBtn.end(); ++iter)
			{
				CNCButton &btn = iter->second;
				if (iButton != iter->first && btn.GetCheck())
				{
					btn.SetCheckStatus(FALSE);
				}
			}
		}
	}

	CIoViewBase::OnLButtonDown(nFlags, point);
}

void CIoViewTextNews::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (!m_bShowTab)
	{	
		CIoViewBase::OnLButtonUp(nFlags, point);
	}

	int32 iButton = TTopButtonHitTest(point);
	if (INVALID_ID != iButton)
	{
		m_mapTopBtn[iButton].LButtonUp();
	}

	CIoViewBase::OnLButtonUp(nFlags, point);
}

void CIoViewTextNews::InitialTopBtns()
{
	CGGTongApp *pApp = (CGGTongApp *)AfxGetApp();
	int32 iHeight = m_iTabHeight;
	int32 iYSkip = 0;
	int32 iXSkip = 0;

	CClientDC dc(this);
	CFont font;
	CFontFactory fontFacotry;
	font.CreateFont(-13,0,0,0,400,0,0,0,0,1,2,1,34, fontFacotry.GetExistFontName(L"微软雅黑"));//...
	CFont *pFontOld = dc.SelectObject(&font);
	
	CSize sizeText = dc.GetTextExtent(pApp->m_strNewsInfo1);
	m_iLeftWidth = iYSkip+sizeText.cx+10;
	CRect rcBtn(iXSkip, iYSkip, m_iLeftWidth, iYSkip+iHeight);
	AddTopButton(&rcBtn, NULL, 3, NEWS_JP_BTN, pApp->m_strNewsInfo1);

	sizeText = dc.GetTextExtent(pApp->m_strNewsInfo2);
	rcBtn.left = (iXSkip + m_iLeftWidth);
	rcBtn.right = rcBtn.left + sizeText.cx + 10;
	m_iLeftWidth = rcBtn.right;
	AddTopButton(&rcBtn, NULL, 3, NEWS_GJS_BTN, pApp->m_strNewsInfo2);

	sizeText = dc.GetTextExtent(pApp->m_strNewsInfo3);
	rcBtn.left = (iXSkip + m_iLeftWidth);
	rcBtn.right = rcBtn.left + sizeText.cx + 10;
	m_iLeftWidth = rcBtn.right;
//	AddTopButton(&rcBtn, NULL, 3, NEWS_CJRL_BTN, pApp->m_strNewsInfo3);

//	AddTopButton(&rcBtn, NULL, 3, NEWS_MORE_BTN, L"更多>>");

	dc.SelectObject(pFontOld);
	font.DeleteObject();
}

void CIoViewTextNews::AddTopButton(LPRECT lpRect, Image *pImage, UINT nCount, UINT nID, LPCTSTR lpszCaption)
{
	CNCButton btnControl;
	btnControl.CreateButton(lpszCaption, lpRect, this, pImage, nCount, nID);
	btnControl.SetTextFrameColor(RGB(50,50,50), RGB(45,45,45), RGB(238,69,2));
	btnControl.SetTextColor(RGB(190,190,190), RGB(255,255,255), RGB(255,255,255));
	btnControl.SetTextBkgColor(RGB(25,25,25), RGB(45,45,45), RGB(238,69,2));
	
	if (NEWS_JP_BTN == nID)
	{
		btnControl.SetCheckStatus(true);
	}

	m_mapTopBtn[nID] = btnControl;
}

void CIoViewTextNews::DrawTopButton()
{
	CClientDC dc(this);
	
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bmp;

	CRect rcClient, rcCaption;
	GetClientRect(rcClient);
	rcCaption = rcClient;
	rcCaption.bottom = rcCaption.top + m_iTabHeight;
	COLORREF clrCaption	= RGB(25, 25, 25);
	
	bmp.CreateCompatibleBitmap(&dc, rcCaption.Width(), rcCaption.Height());
	memDC.SelectObject(&bmp);
	memDC.FillSolidRect(rcCaption, clrCaption);
	memDC.SetBkMode(TRANSPARENT);
	Gdiplus::Graphics graphics(memDC.GetSafeHdc());
	
	CRect rcPaint;
	dc.GetClipBox(&rcPaint);
	map<int, CNCButton>::iterator iter;
	CRect rcControl;
	
	for (iter=m_mapTopBtn.begin(); iter!=m_mapTopBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		btnControl.GetRect(rcControl);
		
		// 判断当前按钮是否需要重绘
		if (!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;
		}
		
		btnControl.DrawButton(&graphics);
	}
	
	//
	dc.BitBlt(0, 0, rcCaption.Width(), rcCaption.Height(), &memDC, 0, 0, SRCCOPY);
	dc.SelectClipRgn(NULL);
	bmp.DeleteObject();
	memDC.DeleteDC();
}

int	 CIoViewTextNews::TTopButtonHitTest(CPoint point)
{
	map<int, CNCButton>::iterator iter;
	
	for (iter=m_mapTopBtn.begin(); iter!=m_mapTopBtn.end(); ++iter)
	{
		CNCButton &btnControl = iter->second;
		
		// 点point是否在已绘制的按钮区域内
		if (btnControl.PtInButton(point))
		{
			return btnControl.GetControlId();
		}
	}
	
	return INVALID_ID;
}

LRESULT CIoViewTextNews::OnListIsFocus(WPARAM wParam, LPARAM lParam)  
{
	bool32 bRet = FALSE;
	CMainFrame *pMainFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (pMainFrame&&pMainFrame->m_pDlgKeyBoard)
	{
		if (!pMainFrame->m_pDlgKeyBoard->IsWindowVisible())
		{
			bRet = TRUE;
		}
	}
	else
	{
		bRet = TRUE;
	}


	if (lParam == s_kiGridIdNews)
	{
		m_GridNewList.IsSetFocus(bRet);
	}
	else if(lParam == s_kiGridIdTodayComment)
	{
		m_GridTodayList.IsSetFocus(bRet);
	}
	return 0;  
}

LRESULT CIoViewTextNews::OnListItemClick(WPARAM wParam, LPARAM lParam)  
{
	if (lParam == s_kiGridIdNews) // 普通资讯
	{	
		if ( NULL == m_pAbsCenterManager )
		{
			return 0;	
		}
		
		T_NewsInfo *pNewsInfo = (T_NewsInfo *)m_GridNewList.GetData(wParam, 1);
		
		if ( pNewsInfo->m_iIndexID <= 0)
			return 0;
		if( pNewsInfo->m_StrTimeUpdate.IsEmpty())
			return 0;
		
		m_nDeitaNew = *pNewsInfo;
		//
		if( m_pAbsCenterManager )
		{
			CMmiNewsReqNewsContent NewsReqNewsContent ;
			NewsReqNewsContent.m_iIndexID = pNewsInfo->m_iIndexID;
			m_pAbsCenterManager->RequestNews(&NewsReqNewsContent);
		}
	}
	else if(lParam == s_kiGridIdTodayComment) // 金评
	{
		if ( NULL == m_pAbsCenterManager )
		{
			return 0;	
		}
		
		m_iCurrJpSelRow = wParam;
		T_NewsInfo *pNewsInfo = (T_NewsInfo *)m_GridTodayList.GetData(wParam, 1);
		if ( NULL == pNewsInfo )
		{
			return 0;
		}
		else 
		{
			if( pNewsInfo->m_iIndexID <= 0 )
				return 0;
			if( pNewsInfo->m_StrTimeUpdate.IsEmpty())
				return 0;
		}
		
		m_nDeitaNew = *pNewsInfo;
		//
		if( m_pAbsCenterManager )
		{
			CMmiNewsReqNewsContent NewsReqNewsContent ;
			NewsReqNewsContent.m_iIndexID = pNewsInfo->m_iIndexID;
			m_pAbsCenterManager->RequestNews(&NewsReqNewsContent);
		}
	}
	return 0;  
}